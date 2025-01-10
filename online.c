#include "online.h"
#include "arith.h"
#include "circ.h"
#include "com.h"
#include "hosts.h"
#include "msg.h"
#include "pre.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// here we want to open a share:
// to do this, players
//  1. broadcast openings to plain shares
//  2. commit to mac candidate differences
//  3. open commitments
//  4. check the commitments; if they don't verify abort
//  5. if the sum is not 0, abort
fp secure_open(struct host **conns, int num_conns, fp delta, struct share *s) {
  broadcast(conns, num_conns, fp_to_packet(s->s));
  packet *recved_shares = recv_broadcasts(conns, num_conns);
  fp candidate = from(0);
  candidate = add(candidate, s->s);
  for (int i = 0; i < num_conns; i++) {
    packet_to_fp((packet *)recved_shares + i);
    candidate = add(candidate, packet_to_fp(&recved_shares[i]));
  }

  fp com_rand = rand_fp();
  fp sshare = add(mul(delta, candidate), neg(s->s_mac));
  char *com = commit(sshare, com_rand);
  broadcast(conns, num_conns, to_packet((uint8_t *)com, 32));

  packet *recved_coms = recv_broadcasts(conns, num_conns);

  broadcast(conns, num_conns, fp_to_packet(sshare));
  packet *shares = recv_broadcasts(conns, num_conns);

  broadcast(conns, num_conns, fp_to_packet(com_rand));
  packet *com_open = recv_broadcasts(conns, num_conns);

  fp mac_check = from(0);
  mac_check = add(mac_check, sshare);

  for (int i = 0; i < num_conns; i++) {
    check_open(packet_to_fp(&shares[i]), packet_to_fp(&com_open[i]),
               (char *)recved_coms[i].data);
    mac_check = add(mac_check, packet_to_fp(&shares[i]));
  }

  assert(is_zero(mac_check));

  return candidate;
};

// Add a constant to a shared value [x] -> [x + c]
// a single arbitrary party should do the s -> s + c update
// everyone updates macs: you may assume this party is named "p0"
struct share *const_add(struct share *s, fp constant, fp delta,
                        char *glob_name) {

  struct share *out = malloc(sizeof(struct share));
  out->s = s->s;
  if (strcmp(glob_name, "p0") == 0) {
    out->s = add(out->s, constant);
  }
  out->s_mac = add(s->s_mac, mul(delta, constant));
  return out;
};

// Multiply a constant to a shared value [x] -> [cx]
struct share *const_mul(struct share *s, fp constant) {
  fp o = mul(s->s, constant);
  fp o_mac = mul(s->s_mac, constant);
  struct share *out = malloc(sizeof(struct share));
  out->s = o;
  out->s_mac = o_mac;
  return out;
};

// Do a local share add ([x], [y]) -> [x + y]
struct share *local_add(struct share *s1, struct share *s2) {
  fp o = add(s1->s, s2->s);
  fp o_mac = add(s1->s_mac, s2->s_mac);
  struct share *out = malloc(sizeof(struct share));
  out->s = o;
  out->s_mac = o_mac;
  return out;
};

// Do a local share sub ([x], [y]) -> [x - y]
struct share *local_sub(struct share *s1, struct share *s2) {
  fp o = add(s1->s, neg(s2->s));
  fp o_mac = add(s1->s_mac, neg(s2->s_mac));
  struct share *out = malloc(sizeof(struct share));
  out->s = o;
  out->s_mac = o_mac;
  return out;
};

// Use the opening and a triple to do a multiplication
// Do a multiplication ([x], [y]) -> [xy]
struct share *multiply(struct host **conns, int num_conns, fp delta,
                       char *glob_name, struct share *s1, struct share *s2,
                       struct triple *t) {
  struct share *share_d = local_sub(s1, t->a);
  fp d = secure_open(conns, num_conns, delta, share_d);
  struct share *share_e = local_sub(s2, t->b);
  fp e = secure_open(conns, num_conns, delta, share_e);
  struct share *ab = const_add(
      local_add(local_add(const_mul(t->a, e), const_mul(t->b, d)), t->ab),
      mul(d, e), delta, glob_name);

  return ab;
};

// Proceed through g_wires and evaluate each wire until you hit the end.
void spdz_online(struct host **conns, int num_conns, char *glob_name, fp delta,
                 struct wires *g_wires, struct randv *auth_rand,
                 struct triple *auth_triples) {
  printf("Online phase starting...\n");
  struct triple *at = auth_triples;

  struct wires *a;

  for (a = g_wires; a != NULL; a = a->next) {
    if (a->w->operation == 0) {
      printf("[%s] (%s + %s) ", a->w->wname, a->w->in1->wname,
             a->w->in2->wname);
      struct share *s1 = a->w->in1->share;
      struct share *s2 = a->w->in2->share;
      if (a->w->in1->operation != 3 && a->w->in2->operation != 3) {
        a->w->share = local_add(s1, s2);
      } else if (a->w->in1->operation == 3 && a->w->in2->operation == 3) {
        a->w->val = add(a->w->in1->val, a->w->in2->val);
        a->w->operation = 3;
      } else if (a->w->in1->operation == 3) {
        a->w->share =
            const_add(a->w->in2->share, a->w->in1->val, delta, glob_name);
      } else {
        a->w->share =
            const_add(a->w->in1->share, a->w->in2->val, delta, glob_name);
      }
      // printf("%lu %lu\n", a->w->share->s->val, a->w->share->s_mac->val);
      printf("\n");
    }
    if (a->w->operation == 1) {
      printf("[%s] (%s * %s) ", a->w->wname, a->w->in1->wname,
             a->w->in2->wname);
      struct share *s1 = a->w->in1->share;
      struct share *s2 = a->w->in2->share;
      if (a->w->in1->operation != 3 && a->w->in2->operation != 3) {
        a->w->share = multiply(conns, num_conns, delta, glob_name, s1, s2, at);
        at = at->next; // use up a triple!
      } else if (a->w->in1->operation == 3 && a->w->in2->operation == 3) {
        a->w->val = mul(a->w->in1->val, a->w->in2->val);
        a->w->operation = 3;
      } else if (a->w->in1->operation == 3) {
        a->w->share = const_mul(a->w->in2->share, a->w->in1->val);
      } else {
        a->w->share = const_mul(a->w->in1->share, a->w->in2->val);
      }
      // printf("%lu %lu\n", a->w->share->s->val, a->w->share->s_mac->val);
      printf("\n");
    }
    if (a->w->operation == 2) {
      struct randv *r;
      struct randv *b;
      for (b = auth_rand; b != NULL; b = b->next) {
        if (strcmp(b->wname, a->w->wname) == 0) {
          r = b;
        }
      }

      if (strcmp(a->w->input_name, glob_name) == 0) {
        printf("Input: broadcasting...\n");
        fp dv = add(a->w->val, neg(r->value));
        broadcast(conns, num_conns, fp_to_packet(dv));
        struct share *s = const_add(r->s, dv, delta, glob_name);
        a->w->share = s;
      } else {
        printf("Input: recieving broadcast...\n");

        struct host *con = {0};
        for (int m = 0; m < num_conns; m++) {
          if (strcmp(conns[m]->name, a->w->input_name) == 0) {
            con = conns[m];
          }
        }

        fp diff = packet_to_fp(recv_single_broadcast(con));
        struct share *s = const_add(r->s, diff, delta, glob_name);
        a->w->share = s;
      }
    }

    if (a->w->is_output == 1) {
      fp out = secure_open(conns, num_conns, delta, a->w->share);
      printf("OUTPUT [%s] %lu\n", a->w->wname, get_output(out));
    }
  }
}

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
fp *secure_open(struct host **conns, int num_conns, fp *delta,
                struct share *s) {

  // TODO: implement this
};

// Add a constant to a shared value [x] -> [x + c]
// a single arbitrary party should do the s -> s + c update: you may assume this
// party is named "p0" everyone updates macs!
struct share *const_add(struct share *s, fp *constant, fp *delta,
                        char *glob_name) {
  // TODO: implement this

};

// Multiply a constant to a shared value [x] -> [cx]
struct share *const_mul(struct share *s, fp *constant) {
  // TODO: implement this
};

// Do a local share add ([x], [y]) -> [x + y]
struct share *local_add(struct share *s1, struct share *s2) {
  // TODO: implement this
};

// Do a local share sub ([x], [y]) -> [x - y]
struct share *local_sub(struct share *s1, struct share *s2) {
  // TODO: implement this
  // (not strictly necessary, but useful for mult)
};

// Use the opening and a triple to do a multiplication
// Do a multiplication ([x], [y]) -> [xy]
struct share *multiply(struct host **conns, int num_conns, fp *delta,
                       char *glob_name, struct share *s1, struct share *s2,
                       struct triple *t) {
  // TODO: implement this
};

// Arguments:
//      1. conns is the list of connections.  [ This is for broadcasting ].
//      2. num_conns is the number of connections.  [ For broadcasting ]
//      3. glob_name is the global name of you (the party).  [ For const add ]..
//      4. delta is the SPDZ global mac share.
//      5. g_wires is the global linked list of wires in eval order.
//      6. auth_rand is a list of random shares [r] (along with input vals)
//      7. auth_triples is a list of beaver triples [a], [b], [ab].
// Proceed through g_wires and evaluate each wire until you hit the end.
void spdz_online(struct host **conns, int num_conns, char *glob_name, fp *delta,
                 struct wires *g_wires, struct randv *auth_rand,
                 struct triple *auth_triples) {

  // 1. loop through all the wires in the g_wires linked list
  //
  // 2. Check what operation is the wire from: do the operation (multiply, add)
  //          dont forget the edge case when one of the input wires is a
  //          constant!
  //
  // 3. (within the previous check): if the wire is input, use up an auth_rand
  // random share and produce shares of the input
  // 4. (within the previous check): if the wire is output, calculate it and
  // then at the end do a secure opening to find the output value.

  // NOTE: don't worry about freeing or managing stuff, we don't care about
  // memory leaks or anything lol this aint a C class
}

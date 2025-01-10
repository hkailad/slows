#ifndef H_MSG
#define H_MSG

#include "arith.h"
#include "hosts.h"
#include <stdint.h>

typedef struct {
  uint8_t *data;
  uint8_t len;
} packet;

fp packet_to_fp(packet *p);

packet *fp_to_packet(fp elem);

packet *to_packet(uint8_t *data, uint8_t len);

void broadcast(struct host **conns, int num_conns, packet *data);

packet *recv_broadcasts(struct host **conns, int num_conns);

packet *recv_single_broadcast(struct host *con);

#endif

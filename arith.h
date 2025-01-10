
#ifndef H_ARITH
#define H_ARITH

#include <stdint.h>

// typedef struct {
//   uint64_t val;
// } fp;

typedef uint64_t fp;

struct share {
  fp s;
  fp s_mac;
};

fp rand_fp();

fp from(uint64_t v);
fp add(fp a, fp b);
fp mul(fp a, fp b);
fp neg(fp a);

int is_zero(fp x);

uint64_t get_output(fp o);

#endif

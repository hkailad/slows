#include "arith.h"
#include <stdio.h>
#include <stdlib.h>

// Field: 18446744073709551557

const uint64_t MOD = 18446744073709551557ULL;

fp *rand_fp() {
  uint64_t v = ((uint64_t)rand() << 32) | ((uint64_t)rand());
  v = v % MOD;
  fp *x = malloc(sizeof(fp));
  x->val = v;
  return x;
}

fp *from(uint64_t v) {
  fp *x = malloc(sizeof(fp));
  x->val = v % MOD;
  return x;
}

fp *add(fp *a, fp *b) {
  fp *x = malloc(sizeof(fp));
  x->val = (a->val + b->val);
  if (x->val > MOD || (x->val < a->val && x->val < b->val)) {
    x->val -= MOD;
  }
  x->val %= MOD;
  return x;
}

fp *mul(fp *a, fp *b) {
  uint64_t v = b->val;
  uint64_t r = a->val;
  uint64_t o = 0;
  while (v > 0) {
    if (v % 2 == 1) {
      uint64_t tmp = o + r;
      if (tmp > MOD || (tmp < o && tmp < r)) {
        tmp -= MOD;
      }
      o = tmp % MOD;
    }
    // r + r
    uint64_t tmp = r + r;
    if (tmp > MOD || (tmp < r)) {
      tmp -= MOD;
    }
    r = tmp % MOD;

    v >>= 1;
  }
  o = o % MOD;
  fp *x = malloc(sizeof(fp));
  x->val = o;
  return x;
}

fp *neg(fp *a) {
  fp *x = malloc(sizeof(fp));
  x->val = MOD - a->val;
  return x;
}

int is_zero(fp *x) { return (x->val == 0); }

uint64_t get_output(fp *x) { return x->val; }

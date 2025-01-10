#ifndef H_PRE
#define H_PRE

#include "arith.h"

struct triple {
  struct triple *next;
  struct share *a;
  struct share *b;
  struct share *ab;
};

struct randv {
  struct randv *next;
  char *wname;
  struct share *s;
  int is_value; // if the value is set
  fp value;     // r (if applicable; ie wire is the user's input)
};

void init_preprocess(char *file);

#endif

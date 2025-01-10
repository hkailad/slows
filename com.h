
#ifndef H_COM
#define H_COM

#include "arith.h"
#include <openssl/sha.h>

// returns a char[32] with the commitment
char *commit(fp data, fp random);

void check_open(fp data, fp random, char *commitment);

#endif

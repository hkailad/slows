#ifndef H_CIRC
#define H_CIRC

#include "arith.h"

struct wire {
  char *wname; // wire name
  struct wire *in1;
  struct wire *in2;
  int depth; // the circuit forms a DAG -> this is the DAG depth
  fp val;    // IF constant, this is the constant value on the wire
             // IF input (for this party), this is the value for that input
  int operation;
  // 0 for addition, 1 for multiplication, 2 for input, 3 for constant
  // if its an input wire or constant wire, in1 and in2 are null.

  int is_output;
  char *input_name; // name of user who provides input if wire is input wire

  struct share *share; // fill this in when evaluating the circuit
};

struct wires {
  struct wires *next;
  struct wire *w;
};

void init_circuit(char *file);

#endif

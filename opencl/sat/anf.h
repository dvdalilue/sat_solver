//
//  anf.h
//  sat
//
//  Created by David Lilue on 16/06/2018.
//

#ifndef ANF

#include "proposition.h"

#define VAR(p) new_var((void *) p)

#define cnst(p) p->prop->constant->value
#define var(p) p->prop->variable->var

typedef enum {AND_ANF, XOR_ANF} OpANF;

typedef enum {None, Left, Right, All} Mode;

typedef struct _constant {
    int value;
} Constant;

typedef struct _variable {
    void *var;
} Variable;

typedef struct _binary_anf {
    OpANF op;
    struct _anf *lhs;
    struct _anf *rhs;
} BinaryANF;

typedef union _anf_data {
    Constant *constant;
    Variable *variable;
    BinaryANF *binary;
} ANFData;

typedef struct _anf {
    int kind; // 0: Constant, 1: Variable, 2: Binary ANF
    ANFData *prop;
} ANF;

ANF* new_const (int cnst);
ANF* new_var (void *var);
ANF* new_bin_anf (OpANF op, ANF *p, ANF *q);
ANF* xor_anf (ANF *p, ANF *q);
ANF* and_anf (ANF *p, ANF *q);
ANF* distr_anf (ANF *p, ANF *q, Mode lv);
ANF* anf (Proposition *p);
ANF* reduce (ANF *p);
void free_anf (ANF *p);
void destroy_anf (ANF *p);
void anf_to_s (ANF *p);

#endif /* anf */

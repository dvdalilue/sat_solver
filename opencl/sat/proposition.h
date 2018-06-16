//
//  proposition.h
//  sat
//
//  Created by David Lilue on 13/06/2018.
//

#ifndef Proposition

#include <stdio.h>

#define STM(x) new_stm((void *) x)

#define op(p) p->prop->binary->op
#define lhs(p) p->prop->binary->lhs
#define rhs(p) p->prop->binary->rhs
#define unneg(n) n->prop->neg->p
#define value(p) p->prop->stm->value

typedef enum {AND, OR, IMPLIE, EQ} Operation;

typedef struct _binary_op {
    Operation op;
    struct _prop *lhs;
    struct _prop *rhs;
} BinaryOperation;

typedef struct _neg_op {
    struct _prop *p;
} Negation;

typedef struct _stm {
    void *value;
} Statement;

typedef union _prop_union {
    Statement *stm;
    Negation *neg;
    BinaryOperation *binary;
} PropData;

typedef struct _prop {
    int kind;
    union _prop_union *prop;
} Proposition;

Proposition* new_stm (void *v);
Proposition* new_neg (Proposition *p);
Proposition* new_bin (Operation op, Proposition *p, Proposition *q);
void destroy_prop (Proposition *p);
void free_stm (Proposition *p);
void free_neg (Proposition *p);
void free_bin (Proposition *p);
void free_prop (Proposition *p);
void prop_to_s (Proposition *p);

#endif /* proposition */

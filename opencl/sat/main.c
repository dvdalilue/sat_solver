//
//  main.c
//  sat
//
//  Created by David Lilue on 05/06/2018.
//

#include <stdio.h>
#include <stdlib.h>

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

void to_s(Proposition *p) {
    switch (p->kind) {
        case 0:
            fprintf(stdout, "%c", (char) p->prop->stm->value);
            break;
        case 1:
            fprintf(stdout, "neg (");
            to_s(p->prop->neg->p);
            fprintf(stdout, ")");
            break;
        case 2:
            fprintf(stdout, "(");
            to_s(p->prop->binary->lhs);
            switch (p->prop->binary->op) {
                case 0:
                    fprintf(stdout, " and ");
                    break;
                case 1:
                    fprintf(stdout, " or ");
                    break;
                case 2:
                    fprintf(stdout, " then ");
                    break;
                case 3:
                    fprintf(stdout, " eq ");
                    break;
                default:
                    fprintf(stdout, " op ");
                    break;
            }
            to_s(p->prop->binary->rhs);
            fprintf(stdout, ")");
            break;
        default:
            fprintf(stdout, "NA");
            break;
    }
}

Proposition* stm(void *v) {
    Proposition *p = (Proposition *) malloc(sizeof(Proposition));
    PropData *d = (PropData *) malloc(sizeof(PropData));
    Statement *s = (Statement *) malloc(sizeof(Statement));
    
    s->value = v;
    d->stm = s;
    
    p->kind = 0;
    p->prop = d;
    
    return p;
}

Proposition * neg(Proposition *p) {
    Proposition *r = (Proposition *) malloc(sizeof(Proposition));
    PropData *d = (PropData *) malloc(sizeof(PropData));
    Negation *n = (Negation *) malloc(sizeof(Negation));
    
    n->p = p;
    d->neg = n;
    r->kind = 1;
    r->prop = d;
    
    return r;
}

Proposition* bin(Operation op, Proposition *p, Proposition *q) {
    Proposition *r = (Proposition *) malloc(sizeof(Proposition));
    PropData *d = (PropData *) malloc(sizeof(PropData));
    BinaryOperation *s = (BinaryOperation *) malloc(sizeof(BinaryOperation));
    
    s->op = op;
    s->lhs = p;
    s->rhs = q;
    
    d->binary = s;
    
    r->kind = 2;
    r->prop = d;
    
    return r;
}

Proposition* impl_free(Proposition *p) {
    Proposition *actual = p;

    switch (actual->kind) {
        case 0:
            return p;
            break;
        case 1:
            return neg(impl_free(actual->prop->neg->p));
            break;
        case 2:
            if (actual->prop->binary->op == IMPLIE) {
                return bin(OR, neg(impl_free(actual->prop->binary->lhs)), impl_free(actual->prop->binary->rhs));
            } else if (actual->prop->binary->op == EQ) {
                return bin(AND,
                           bin(OR, neg(impl_free(actual->prop->binary->lhs)), impl_free(actual->prop->binary->rhs)),
                           bin(OR, neg(impl_free(actual->prop->binary->rhs)), impl_free(actual->prop->binary->lhs)));
            } else {
                return bin(actual->prop->binary->op,
                           impl_free(actual->prop->binary->lhs),
                           impl_free(actual->prop->binary->rhs));
            }
            break;
    }
    return p;
}

int main (int argc, const char * argv[]) {
    Proposition *p = stm((void *) 'P');
    Proposition *q = stm((void *) 'Q');
    
    Proposition *modus_ponen = bin(IMPLIE, bin(AND, bin(IMPLIE, p, q), p), q);

    to_s(impl_free(modus_ponen));
    
    fprintf(stdout, "\n");
    return 0;
}

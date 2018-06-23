//
//  anf_lib.c
//  sat
//
//  Created by David Lilue on 19/06/2018.
//

#include <stdlib.h>
#include <stdio.h>
#include "anf_lib.h"

ANF* new_const (int cnst) {
    ANF *a = (ANF *) malloc(sizeof(ANF));
    ANFData *d = (ANFData *) malloc(sizeof(ANFData));
    Constant *c = (Constant *) malloc(sizeof(Constant));
    
    c->value = cnst;
    d->constant = c;
    
    a->kind = 0;
    a->prop = d;
    
    return a;
}

ANF* new_var (void *var) {
    ANF *p = (ANF *) malloc(sizeof(ANF));
    ANFData *d = (ANFData *) malloc(sizeof(ANFData));
    Variable *v = (Variable *) malloc(sizeof(Variable));
    
    v->var = var;
    d->variable = v;
    
    p->kind = 1;
    p->prop = d;
    
    return p;
}

ANF* new_bin_anf (OpANF op, ANF *p, ANF *q) {
    ANF *r = (ANF *) malloc(sizeof(ANF));
    ANFData *d = (ANFData *) malloc(sizeof(ANFData));
    BinaryANF *b = (BinaryANF *) malloc(sizeof(BinaryANF));
    
    b->op = op;
    b->lhs = p;
    b->rhs = q;
    
    d->binary = b;
    
    r->kind = 2;
    r->prop = d;
    
    return r;
}

ANF* xor_anf (ANF *p, ANF *q) {
    return new_bin_anf(XOR_ANF, p, q);
}

ANF* and_anf (ANF *p, ANF *q) {
    return new_bin_anf(AND_ANF, p, q);
}

ANF* distr_anf (ANF *p, ANF *q) {
    ANF *result = NULL;
    ANF *lhs_xor = NULL,
        *rhs_xor = NULL;
    
    if (p->kind == 2 && op(p) == XOR_ANF) {
        lhs_xor = distr_anf(lhs(p), copy(q));
        rhs_xor = distr_anf(rhs(p), q);

        result = xor_anf(lhs_xor, rhs_xor);
        free_anf(p);
    } else if (q->kind == 2 && op(q) == XOR_ANF) {
        lhs_xor = distr_anf(copy(p), lhs(q));
        rhs_xor = distr_anf(p, rhs(q));

        result = xor_anf(lhs_xor, rhs_xor);
        free_anf(q);
    } else {
        result = and_anf(p, q);
    }
    return result;
}

ANF* or_to_xor (ANF *lhs, ANF *rhs) {
    return xor_anf(lhs, xor_anf(rhs, distr_anf(copy(lhs), copy(rhs))));
}

ANF* implie_to_xor (ANF *lhs, ANF *rhs) {
    return xor_anf(new_const(1), xor_anf(lhs, distr_anf(copy(lhs), rhs)));    
}

ANF* eq_to_xor (ANF *lhs, ANF *rhs) {
    return distr_anf(
                xor_anf(new_const(1),
                    xor_anf(lhs,
                        distr_anf(copy(lhs), copy(rhs)))),
                xor_anf(new_const(1),
                    xor_anf(rhs,
                        distr_anf(copy(rhs), copy(lhs)))));
}

ANF* reduce (ANF *p) {
    return NULL;
}

ANF* copy (ANF *p) {
    switch (p->kind) {
        case 0:
            return new_const(cnst(p));
            break;
        case 1:
            return new_var(var(p));
            break;
        case 2:
            return new_bin_anf(op(p), copy(lhs(p)), copy(rhs(p)));
            break;
    }
    printf("THIS NEVER HAS TO HAPPEN\n");
    return NULL;
}

void free_anf (ANF *p) {
    switch (p->kind) {
        case 0:
            free(p->prop->constant);
            break;
        case 1:
            free(p->prop->variable);
            break;
        case 2:
            free(p->prop->binary);
            break;
    }
    free(p->prop);
    free(p);
}

void anf_to_s (ANF *p) {
    switch (p->kind) {
        case 0:
            fprintf(stdout, "%d", cnst(p));
            break;
        case 1:
            fprintf(stdout, "%c", (char) var(p));
            break;
        case 2:
            fprintf(stdout, "(");
            anf_to_s(lhs(p));
            switch (op(p)) {
                case 0:
                    fprintf(stdout, " and ");
                    break;
                case 1:
                    fprintf(stdout, " xor ");
                    break;
                default:
                    fprintf(stdout, " op ");
                    break;
            }
            anf_to_s(rhs(p));
            fprintf(stdout, ")");
            break;
        default:
            fprintf(stdout, "NA");
            break;
    }
}

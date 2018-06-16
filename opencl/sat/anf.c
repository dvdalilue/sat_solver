//
//  anf.c
//  sat
//
//  Created by David Lilue on 16/06/2018.
//

#include <stdlib.h>
#include <stdio.h>
#include "anf.h"
#include "stack.h"

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

ANF* distr_anf (ANF *p, ANF *q) {
    ANF *result = NULL;

    if (p->kind == 2 && op(p) == XOR_ANF) {
        result = new_bin_anf(XOR_ANF, distr_anf(lhs(p), q), distr_anf(rhs(p), q));
        // free_bin(p);
    } else if (q->kind == 2 && op(p) == XOR_ANF) {
        result = new_bin_anf(XOR_ANF, distr_anf(p, lhs(q)), distr_anf(p, rhs(q)));
        // free_bin(q);
    } else {
        result = new_bin_anf(AND_ANF, p, q);
    }
    return result;
}

ANF* xor_anf (ANF *p, ANF *q) {
    return new_bin_anf(XOR_ANF, p, q);
}

ANF* and_anf (ANF *p, ANF *q) {
    return new_bin_anf(AND_ANF, p, q);
}

ANF* anf (Proposition *p) {
    ANF *result = NULL;
    ANF *lhs_anf = NULL;
    ANF *rhs_anf = NULL;
    
    switch (p->kind) {
        case 0: // Statement
            result = new_var(p->prop->stm->value);
            // free_stm(p);
            break;
        case 1: // Negation
            result = xor_anf(new_const(1), anf(desneg(p)));
            // free_neg(p);
            break;
        case 2: // Binary operation
            switch (op(p)) {
                case AND:
                    result = distr_anf(anf(lhs(p)), anf(rhs(p)));
                    break;
                case OR:
                    lhs_anf = anf(lhs(p));
                    rhs_anf = anf(rhs(p));
                    result = xor_anf(lhs_anf, xor_anf(rhs_anf, distr_anf(lhs_anf, rhs_anf)));
                    break;
                case IMPLIE:
                    lhs_anf = anf(lhs(p));
                    rhs_anf = anf(rhs(p));
                    result = xor_anf(new_const(1), xor_anf(lhs_anf, distr_anf(lhs_anf, rhs_anf)));
                    break;
                case EQ:
                    result = anf(new_bin(AND,
                                         new_bin(IMPLIE, lhs(p), rhs(p)),
                                         new_bin(IMPLIE, rhs(p), lhs(p))));
                    break;
            }
            // free_bin(p);
            break;
    }
    return result;
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

void destroy_anf (ANF *p) {
    ANF *aux;
    Stack *s; stack_new(&s);
    Stack *anf_unique; stack_new(&anf_unique);

    push(s, (void *) p);

    while (!is_empty(s)) {
        aux = (ANF *) top(s);
        switch (aux->kind) {
            case 0:
                push_if_unique(anf_unique, (void *) aux);
                break;
            case 1:
                push_if_unique(anf_unique, (void *) aux);
                break;
            case 2:
                push(s, (void *) aux->prop->binary->lhs);
                push(s, (void *) aux->prop->binary->rhs);
                push_if_unique(anf_unique, (void *) aux);
                break;
        }
    }
    
    while (!is_empty(anf_unique)) {
        aux = (ANF *) top(anf_unique);
        free_anf(aux);
    }
    
    destroy_stack(&s);
    destroy_stack(&anf_unique);
}

ANF* reduce (ANF *p) {
    return NULL;
}

void anf_to_s (ANF *p) {
    switch (p->kind) {
        case 0:
            fprintf(stdout, "%d", p->prop->constant->value);
            break;
        case 1:
            fprintf(stdout, "%c", (char) p->prop->variable->var);
            break;
        case 2:
            fprintf(stdout, "(");
            anf_to_s(p->prop->binary->lhs);
            switch (p->prop->binary->op) {
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
            anf_to_s(p->prop->binary->rhs);
            fprintf(stdout, ")");
            break;
        default:
            fprintf(stdout, "NA");
            break;
    }
}

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

ANF* xor_anf (ANF *p, ANF *q) {
    return new_bin_anf(XOR_ANF, p, q);
}

ANF* and_anf (ANF *p, ANF *q) {
    return new_bin_anf(AND_ANF, p, q);
}

ANF* distr_anf (ANF *p, ANF *q) {
    ANF *result = NULL;
    
    if (p->kind == 2 && op(p) == XOR_ANF) {
        result = xor_anf(distr_anf(lhs(p), copy(q)), distr_anf(rhs(p), q));
        free_anf(p);
    } else if (q->kind == 2 && op(q) == XOR_ANF) {
        result = xor_anf(distr_anf(copy(p), lhs(q)), distr_anf(p, rhs(q)));
        free_anf(q);
    } else {
        result = and_anf(p, q);
    }
    return result;
}

ANF* anf (Proposition *p) {
    ANF *result = NULL;
    ANF *lhs_anf = NULL;
    ANF *rhs_anf = NULL;
    
    switch (p->kind) {
        case 0: // Statement
            result = new_var(value(p));
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
                    result = xor_anf(lhs_anf, xor_anf(rhs_anf, distr_anf(copy(lhs_anf), copy(rhs_anf))));
                    break;
                case IMPLIE:
                    lhs_anf = anf(lhs(p));
                    rhs_anf = anf(rhs(p));
                    result = xor_anf(new_const(1), xor_anf(lhs_anf, distr_anf(copy(lhs_anf), rhs_anf)));
                    break;
                case EQ:
                    lhs_anf = anf(lhs(p));
                    rhs_anf = anf(rhs(p));
                    result = distr_anf(
                                xor_anf(new_const(1), xor_anf(lhs_anf, distr_anf(copy(lhs_anf), copy(rhs_anf)))),
                                xor_anf(new_const(1), xor_anf(rhs_anf, distr_anf(copy(rhs_anf), copy(lhs_anf)))));
                    break;
            }
            // free_bin(p);
            break;
    }
    return result;
}

ANF* anf_it (Proposition *p) {
    //    ANF *result = NULL;
    Proposition *tmp = NULL;
    
    Stack *s; stack_new(&s);
    Stack *f; stack_new(&f);
    
    push(s, (void *) p);
    
    while (!is_empty(s)) {
        tmp = (Proposition *) top(s);
        
        switch (tmp->kind) {
            case 0:
                new_var(value(tmp));
                break;
            case 1:
                
                break;
            case 2:
                
                break;
                
            default:
                break;
        }
    }
    
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

void destroy_anf (ANF *p) {
    ANF *aux;
    Stack *s; stack_new(&s);
    Stack *anf_unique; stack_new(&anf_unique);

    push(s, (void *) p);

    while (!is_empty(s)) {
        aux = (ANF *) top(s);
        if (aux->kind == 2) {
            push(s, (void *) lhs(aux));
            push(s, (void *) rhs(aux));
        }
        push_if_unique(anf_unique, (void *) aux);
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

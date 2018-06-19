//
//  anf.c
//  sat
//
//  Created by David Lilue on 16/06/2018.
//

#include <stdlib.h>
#include <stdio.h>
#include "anf.h"

ANF* anf (Proposition *p) {
    ANF *result = NULL;
    ANF *lhs_anf = NULL;
    ANF *rhs_anf = NULL;
    
    switch (p->kind) {
        case 0: // Statement
            result = new_var(value(p));
            break;
        case 1: // Negation
            result = xor_anf(new_const(1), anf(desneg(p)));
            break;
        case 2: // Binary operation
            switch (op(p)) {
                case AND:
                    result = distr_anf(anf(lhs(p)), anf(rhs(p)));
                    break;
                case OR:
                    lhs_anf = anf(lhs(p));
                    rhs_anf = anf(rhs(p));
                    result = xor_anf(lhs_anf,
                                xor_anf(rhs_anf,
                                    distr_anf(copy(lhs_anf), copy(rhs_anf))));
                    break;
                case IMPLIE:
                    lhs_anf = anf(lhs(p));
                    rhs_anf = anf(rhs(p));
                    result = xor_anf(new_const(1),
                                xor_anf(lhs_anf,
                                    distr_anf(copy(lhs_anf), rhs_anf)));
                    break;
                case EQ:
                    lhs_anf = anf(lhs(p));
                    rhs_anf = anf(rhs(p));
                    result = distr_anf(
                                xor_anf(new_const(1),
                                    xor_anf(lhs_anf,
                                        distr_anf(copy(lhs_anf), copy(rhs_anf)))),
                                xor_anf(new_const(1),
                                    xor_anf(rhs_anf,
                                        distr_anf(copy(rhs_anf), copy(lhs_anf)))));
                    break;
            }
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
                push(s, (void *) desneg(p));
                break;
            case 2:
                push(s, (void *) lhs(p));
                push(s, (void *) rhs(p));
                break;
        }
    }
    
    return NULL;
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

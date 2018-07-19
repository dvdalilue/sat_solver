//
//  anf.c
//  sat
//
//  Created by David Lilue on 16/06/2018.
//

#include <stdlib.h>
#include <stdio.h>
#include "anf.h"

ANF* rpn_calculator (Stack *rpn);
Stack* prop_to_anf_rpn (Proposition *prop);

ANF* anf (Proposition *p) {
    ANF *lhs_anf = NULL;
    ANF *rhs_anf = NULL;

    switch (p->kind) {
        case 0: // Statement
            return new_var(value(p));
        case 1: // Negation
            return xor_anf(new_const(1), anf(desneg(p)));
        case 2: // Binary operation
            lhs_anf = anf(lhs(p));
            rhs_anf = anf(rhs(p));
            switch (op(p)) {
                case AND:
                    return distr_anf(lhs_anf, rhs_anf);
                case OR:
                    return or_to_xor(lhs_anf, rhs_anf);
                case IMPLIE:
                    return implie_to_xor(lhs_anf, rhs_anf);
                case EQUAL:
                    return eq_to_xor(lhs_anf, rhs_anf);
            }
    }
    return NULL;
}

ANF* anf_it (Proposition *p) {
    return rpn_calculator(prop_to_anf_rpn(p));
}

Stack* prop_to_anf_rpn (Proposition *prop) {
    Proposition *current = NULL;

    Stack *prop_s; stack_new(&prop_s);
    Stack *rpn; stack_new(&rpn);

    push(prop_s, (void *) prop);
    
    while (!is_empty(prop_s)) {
        current = (Proposition *) top(prop_s);
        
        switch (current->kind) {
            case 0:
                push(rpn, (void *) new_rpn_opn(new_var(value(current))));
                break;
            case 1:
                push(rpn, (void *) new_rpn_opr(&xor_anf));
                push(rpn, (void *) new_rpn_opn(new_const(1)));
                push(prop_s, (void *) desneg(current));
                break;
            case 2:
                switch (op(current)) {
                    case AND:
                        push(rpn, (void *) new_rpn_opr(&distr_anf));
                        break;
                    case OR:
                        push(rpn, (void *) new_rpn_opr(&or_to_xor));
                        break;
                    case IMPLIE:
                        push(rpn, (void *) new_rpn_opr(&implie_to_xor));
                        break;
                    case EQUAL:
                        push(rpn, (void *) new_rpn_opr(&eq_to_xor));
                        break;
                }
                push(prop_s, (void *) lhs(current));
                push(prop_s, (void *) rhs(current));
                break;
        }
    }

    destroy_stack(&prop_s);

    return rpn;
} 

ANF* rpn_calculator (Stack *rpn) {
    Stack *operands; stack_new(&operands);
    ANF_RPN *current = NULL;

    ANF *aux_1 = NULL,
        *aux_2 = NULL;

    while (!is_empty(rpn)) {
        current = (ANF_RPN *) top(rpn);

        if (is_empty(rpn) && current->kind == OPN) {
            push(operands, (void *) current->anf->opn->anf);
            free_rpn(current);
            break;
        }

        switch (current->kind) {
            case OPR:
                aux_2 = (ANF *) top(operands);
                aux_1 = (ANF *) top(operands);
                push(operands, (void *) opr(current)(aux_1, aux_2));
                break;
            case OPN:
                push(operands, (void *) current->anf->opn->anf);
                break;
        }
        free_rpn(current);
    }

    aux_1 = (ANF *) top(operands);

    destroy_stack(&operands);
    destroy_stack(&rpn);

    return aux_1;
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

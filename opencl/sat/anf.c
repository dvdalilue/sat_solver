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

ANF_RPN* new_rpn_opn (ANF *v) {
    ANF_RPN *rpn = (ANF_RPN *) malloc(sizeof(ANF_RPN));
    ANF_RPN_UNION *u = (ANF_RPN_UNION *) malloc(sizeof(ANF_RPN_UNION));
    ANF_RPN_OPN *opn = (ANF_RPN_OPN *) malloc(sizeof(ANF_RPN_OPN));
    
    opn->anf = v;
    u->opn = opn;
    
    rpn->kind = OPN;
    rpn->anf = u;
    
    return rpn;
}

ANF_RPN* new_rpn_opr (ANF *(*operation)(ANF*, ANF*)) {
    ANF_RPN *rpn = (ANF_RPN *) malloc(sizeof(ANF_RPN));
    ANF_RPN_UNION *u = (ANF_RPN_UNION *) malloc(sizeof(ANF_RPN_UNION));
    ANF_RPN_OPR *opr = (ANF_RPN_OPR *) malloc(sizeof(ANF_RPN_OPR));
    
    opr->operation = operation;
    u->opr = opr;
    
    rpn->kind = OPR;
    rpn->anf = u;
    
    return rpn;
}

void free_rpn (ANF_RPN *rpn) {
    switch (rpn->kind) {
        case OPN:
            free(rpn->anf->opn);
            break;
        case OPR:
            free(rpn->anf->opr);
            break;
    }
    free(rpn->anf);
    free(rpn);
}

ANF* anf_it (Proposition *p) {
    Proposition *tmp = NULL;
    ANF_RPN *current;
    ANF *op1, *op2;
    
    Stack *s; stack_new(&s);
    Stack *f; stack_new(&f);
    
    push(s, (void *) p);
    
    while (!is_empty(s)) {
        tmp = (Proposition *) top(s);
        
        switch (tmp->kind) {
            case 0:
                push(f, (void *) new_rpn_opn(new_var(value(tmp))));
                break;
            case 1:
                push(f, (void *) new_rpn_opr(&xor_anf));
                push(f, (void *) new_rpn_opn(new_const(1)));
                push(s, (void *) desneg(tmp));
                break;
            case 2:
                push(s, (void *) lhs(tmp));
                push(s, (void *) rhs(tmp));
                break;
        }
    }

    while (!is_empty(f)) {
        current = (ANF_RPN *) top(f);

        if (is_empty(f) && current->kind == OPN) {
            push(f, (void *) current->anf->opn->anf);
            free_rpn(current);
            break;
        }

        switch (current->kind) {
            case OPR:
                op1 = (ANF *) top(s);
                op2 = (ANF *) top(s);
                push(f, (void *) new_rpn_opn(opr(current)(op1, op2)));
                break;
            case OPN:
                push(s, (void *) current->anf->opn->anf);
                break;
        }
        free_rpn(current);
    }
    
    op1 = (ANF *) top(f);
    
    destroy_stack(&s);
    destroy_stack(&f);

    return op1;
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

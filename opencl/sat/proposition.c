//
//  proposition.c
//  sat
//
//  Created by David Lilue on 13/06/2018.
//

#include <stdio.h>
#include <stdlib.h>
#include "proposition.h"
#include "stack.h"

/*
 * Creates a new statement using a given value and the struct
 * type is set to 0 (Statement).
 */
Proposition* new_stm (void *v) {
    Proposition *p = (Proposition *) malloc(sizeof(Proposition));
    PropData *d = (PropData *) malloc(sizeof(PropData));
    Statement *s = (Statement *) malloc(sizeof(Statement));

    s->value = v;
    d->stm = s;

    p->kind = 0;
    p->prop = d;

    return p;
}

/*
 * Creates a new negation using a given proposition and the
 * struct type is set to 1 (Negation).
 */
Proposition* new_neg (Proposition *p) {
    Proposition *r = (Proposition *) malloc(sizeof(Proposition));
    PropData *d = (PropData *) malloc(sizeof(PropData));
    Negation *n = (Negation *) malloc(sizeof(Negation));

    n->p = p;
    d->neg = n;
    r->kind = 1;
    r->prop = d;

    return r;
}

/*
 * Creates a new binary operation using two given operands
 * (propositions) and the struct type is set to 2
 * (Binary Operation).
 */
Proposition* new_bin (Operation op, Proposition *p, Proposition *q) {
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

/*
 * Copy a proposition and creates another with the same structure
 */
Proposition* copy_prop (Proposition *p) {
    switch (p->kind) {
        case 0:
            return new_stm(value(p));
            break;
        case 1:
            return new_neg(copy_prop(desneg(p)));
            break;
        case 2:
            return new_bin(op(p), copy_prop(lhs(p)), copy_prop(rhs(p)));
            break;
    }
    printf("THIS NEVER HAS TO HAPPEN\n");
    return NULL;
}

/*
 * Free the memory used by a proposition of kind 'statement'.
 */
void free_stm (Proposition *p) {
    free(p->prop->stm);
    free(p->prop);
    free(p);
}

/*
 * Free the memory used by a proposition of kind 'negation'.
 */
void free_neg (Proposition *p) {
    free(p->prop->neg);
    free(p->prop);
    free(p);
}

/*
 * Free the memory used by a proposition of kind
 * 'binary operation'.
 */
void free_bin (Proposition *p) {
    free(p->prop->binary);
    free(p->prop);
    free(p);
}

/*
 * Free the memory used of any proposition depending of its
 * kind.
 */
void free_prop (Proposition *p) {
    switch (p->kind) {
        case 0:
            free_stm(p);
            break;
        case 1:
            free_neg(p);
            break;
        case 2:
            free_bin(p);
            break;
    }
}

/*
 * Destroys a proposition recursively simulated with stack,
 * freeing the memory only one time per pointer, because the
 * same pointer could be used more than once. This traversal is
 * following a DFS implementation to the right side of the
 * tree.
 */
void destroy_prop (Proposition *p) {
    Proposition *aux;
    Stack *s; stack_new(&s);
    Stack *stm_unique; stack_new(&stm_unique);

    push(s, (void *) p);

    while (!is_empty(s)) {
        aux = (Proposition *) top(s);
        switch (aux->kind) {
            case 0:
                push_if_unique(stm_unique, (void *) aux);
                break;
            case 1:
                push(s, (void *) desneg(aux));
                push_if_unique(stm_unique, (void *) aux);
                break;
            case 2:
                push(s, (void *) lhs(aux));
                push(s, (void *) rhs(aux));
                push_if_unique(stm_unique, (void *) aux);
                break;
        }
    }

    while (!is_empty(stm_unique)) {
        aux = (Proposition *) top(stm_unique);
        free_prop(aux);
    }

    destroy_stack(&s);
    destroy_stack(&stm_unique);
}

/*
 * Prints a proposition recursively.
 */
void prop_to_s (Proposition *p) {
    switch (p->kind) {
        case 0:
            fprintf(stdout, "%d", p->prop->stm->value);
            break;
        case 1:
            fprintf(stdout, "neg (");
            prop_to_s(p->prop->neg->p);
            fprintf(stdout, ")");
            break;
        case 2:
            fprintf(stdout, "(");
            prop_to_s(p->prop->binary->lhs);
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
            prop_to_s(p->prop->binary->rhs);
            fprintf(stdout, ")");
            break;
        default:
            fprintf(stdout, "NA");
            break;
    }
}

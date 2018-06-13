//
//  proposition.c
//  sat
//
//  Created by David Lilue on 13/06/2018.
//

#include <stdlib.h>
#include "proposition.h"
#include "stack.h"

Proposition* new_stm(void *v) {
    Proposition *p = (Proposition *) malloc(sizeof(Proposition));
    PropData *d = (PropData *) malloc(sizeof(PropData));
    Statement *s = (Statement *) malloc(sizeof(Statement));
    
    s->value = v;
    d->stm = s;
    
    p->kind = 0;
    p->prop = d;
    
    return p;
}

Proposition* new_neg(Proposition *p) {
    Proposition *r = (Proposition *) malloc(sizeof(Proposition));
    PropData *d = (PropData *) malloc(sizeof(PropData));
    Negation *n = (Negation *) malloc(sizeof(Negation));
    
    n->p = p;
    d->neg = n;
    r->kind = 1;
    r->prop = d;
    
    return r;
}

Proposition* new_bin(Operation op, Proposition *p, Proposition *q) {
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

void free_stm(Proposition *p) {
    free(p->prop->stm);
    free(p->prop);
    free(p);
}

void free_neg(Proposition *p) {
    free(p->prop->neg);
    free(p->prop);
    free(p);
}
void free_bin(Proposition *p) {
    free(p->prop->binary);
    free(p->prop);
    free(p);
}

void destroy_prop(Proposition *p) {
    Proposition *aux;
    Stack *s; stack_new(&s);
    
    push(s, (void *) p);
    
    while (!is_empty(s)) {
        aux = (Proposition *) top(s);
        switch (aux->kind) {
            case 0:
                free_stm(aux);
                break;
            case 1:
                push(s, (void *) unneg(aux));
                free_neg(aux);
                break;
            case 2:
                push(s, (void *) lhs(aux));
                push(s, (void *) rhs(aux));
                free_bin(aux);
                break;
        }
    }
    destroy_stack(&s);
}

void prop_to_s(Proposition *p) {
    switch (p->kind) {
        case 0:
            fprintf(stdout, "%c", (char) p->prop->stm->value);
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

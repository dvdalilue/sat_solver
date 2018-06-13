//
//  stack.c
//  sat
//
//  Created by David Lilue on 13/06/2018.
//

#include "stack.h"
#include <stdlib.h>
#include <stdio.h>

void stack_new (Stack **s) {
    *s = (Stack *) malloc (sizeof(Stack));
    (*s)->top = NULL;
    (*s)->size = 0;
}

int is_empty (Stack *s) {
    return s->size ? 0 : 1;
}

void push (Stack *s, void *n) {
    Elem *aux = (Elem *) malloc (sizeof(Elem));
    
    aux->value = n;
    aux->next = s->top;
    
    s->top = aux;
    s->size++;
}

void* top (Stack *s) {
    if (is_empty(s)) {
        fprintf(stdout, "*** Error: retriving top on empty stack\n");
        exit(1);
    }
    
    Elem *tmp = s->top;
    void *val = tmp->value;
    
    s->top = s->top->next;
    s->size--;
    
    free(tmp);
    
    return val;
}

void destroy(Stack **s) {
    Elem *aux = (*s)->top;
    Elem *tmp = NULL;
    
    while (aux != NULL) {
        tmp = aux->next;
        free(aux);
        aux = tmp;
    }
    
    free(*s);
}

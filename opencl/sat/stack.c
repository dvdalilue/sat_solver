//
//  stack.c
//  sat
//
//  Created by David Lilue on 13/06/2018.
//

#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

/*
 * Create a new stack using a double pointer to a stack.
 * Allocates the memory needed and initialize the top pointer
 * and size to 0.
 */
void stack_new (Stack **s) {
    *s = (Stack *) malloc (sizeof(Stack));
    (*s)->top = NULL;
    (*s)->size = 0;
}

/*
 * Checks if the stack is empty. Returns 1 if so, otherwise 0.
 */
int is_empty (Stack *s) {
    return s->size ? 0 : 1;
}

/*
 * Checks if some value is currently in the stack. From the top
 * until reach NULL. Returning 1 if 'v' is in the stack,
 * otherwise 0.
 */
int member (Stack *s, void *v) {
    Elem *current = s->top;

    while (current != NULL) {
        if (current->value == v) {
            return 1;
        }
        current = current->next;
    }

    return 0;
}

/*
 * Push a new value in the top of the stack. This new top
 * points to the older top and the size of the stack increase.
 */
void push (Stack *s, void *n) {
    Elem *aux = (Elem *) malloc (sizeof(Elem));

    aux->value = n;
    aux->next = s->top;

    s->top = aux;
    s->size++;
}

/*
 * Push a new value on the top of the stack if is not member.
 */
void push_if_unique (Stack *s, void *v) {
    if (!member(s, v)) {
        push(s, v);
    }
}

/*
 * Returns the top value of the stack. If the stack is empty,
 * the excution terminates with an error message.
 */
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

/*
 * Free all the memory used by the stack.
 */
void destroy_stack (Stack **s) {
    Elem *aux = (*s)->top;
    Elem *tmp = NULL;

    while (aux != NULL) {
        tmp = aux->next;
        free(aux);
        aux = tmp;
    }

    free(*s);
}

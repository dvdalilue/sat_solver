//
//  stack.h
//  sat
//
//  Created by David Lilue on 13/06/2018.
//

#ifndef Stack

typedef struct _elem {
    void *value;
    struct _elem *next;
} Elem;

typedef struct _stk {
    struct _elem *top;
    int size;
} Stack;

void stack_new (Stack **s);
int is_empty (Stack *s);
void push (Stack *s, void *v);
void push_if_unique (Stack *s, void *v);
void* top (Stack *s);
void destroy_stack (Stack **s);

#endif /* stack */

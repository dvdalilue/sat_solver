//
//  array.h
//  sat
//
//  Created by David Lilue on 29/06/2018.
//

#ifndef Array

#define index(a, i, t) ((t*) a->elements)[i]

#define array_new(a, a_size, type) ({ \
            *a = (Array *) malloc(sizeof(Array)); \
            (*a)->capacity = a_size; \
            (*a)->size = 0; \
            (*a)->elements = (void *) malloc(sizeof(type) * a_size); \
        })

typedef struct _array {
    size_t capacity;
    int size;
    void *elements;
} Array;

void array_init (Array *ar);
void array_free (Array *ar);

#endif /* array_h */

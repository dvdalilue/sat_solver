//
//  array.c
//  sat
//
//  Created by David Lilue on 29/06/2018.
//

#include <stdlib.h>
#include "array.h"

void array_init(Array *a) {
    for (int i = 0; i < a->size; i++) {
        index(a, i, void *) = NULL;
    }
}

void array_free (Array *array) {
    free(array->elements);
    free(array);
}

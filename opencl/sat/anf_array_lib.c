//
//  anf_array_lib.c
//  sat
//
//  Created by David Lilue on 29/06/2018.
//

#include <stdlib.h>
#include <stdio.h>

#include "anf_array_lib.h"

ANF_Array* empty_anf_array (void) {
    ANF_Array *anf_a = (ANF_Array *) malloc(sizeof(ANF_Array));
    array_new(&anf_a->xors, 7, ANF_BitString*);
    array_init(anf_a->xors);
    anf_a->components = 0;

    return anf_a;
}

ANF_BitString* new_bitstring (void) {
    ANF_BitString *bs = (ANF_BitString *) malloc(sizeof(ANF_BitString));
    
    array_new(&bs->bstring, 128, char);
    for (int i = 0; i < 128; i++) {
        index(bs->bstring, i, char) = 0;
    }
    bs->bits_on = 0;
    bs->bstring->size = bs->bstring->capacity;

    return bs;
}

ANF_Array* new_anf_array (void) {
    ANF_Array *anf_a = empty_anf_array();
    ANF_BitString *bs = new_bitstring();
    
    add_xor_comp(anf_a, bs);
    
    return anf_a;
}

ANF_Array* one (void) {
    return new_anf_array();
}

ANF_Array* zero (void) {
    ANF_Array *z = new_anf_array();
    
    for (int i = 0; i < 128; i++) {
        get_bs_comp(z, 0, i) = -1;
    }

    return z;
}

ANF_Array* new_poly (int var) {
    ANF_Array *p = new_anf_array();
    
    get_bs_comp(p, 0, var) = 1;
    get_anf_bs(p, 0)->bits_on++;
    
    return p;
}

ANF_BitString* array_get_xor (Array *array, int i) {
  if (i >= array->size || i < 0) {
    printf("Index %d out of bounds for array of size %d\n", i, array->size);
    exit(1);
  }
  return index(array, i, ANF_BitString*);
}

ANF_BitString* get_anf_bs (ANF_Array *anf, int i) {
    return array_get_xor(anf->xors, i);
}

void array_double_capacity_if_full (Array *array) {
    if (array->size >= array->capacity) {
        array->capacity *= 2;
        array->elements = (void *) realloc(
                                    array->elements,
                                    sizeof(ANF_BitString*) * array->capacity);
    }
}

void array_add_xor (Array *array, ANF_BitString *bs) {
    array_double_capacity_if_full(array);

    index(array, array->size++, ANF_BitString*) = bs;
}

void add_xor_comp (ANF_Array *p, ANF_BitString *bs) {
    array_add_xor(p->xors, bs);
    p->components++;
}

Ordering compare_bs (ANF_BitString *x, ANF_BitString *y) {
    if (x->bits_on < y->bits_on) {
        return LT;
    } else if (x->bits_on > y->bits_on) {
        return GT;
    } else {
        if (index(x->bstring, 0, char) == -1  &&
            index(y->bstring, 0, char) == -1) { return EQ; }
        if (index(x->bstring, 0, char) == -1) { return LT; }
        if (index(y->bstring, 0, char) == -1) { return GT; }
        if (x->bits_on == 0) { return EQ; }

        for (int i = 0; i < x->bstring->size; i++) {
            if (index(x->bstring, i, char) == 1 &&
                index(y->bstring, i, char) == 0)
            {
                return LT;
            } else if (index(x->bstring, i, char) == 0 &&
                       index(y->bstring, i, char) == 1)
            {
                return GT;
            }
        }
    }

    return EQ;
}

ANF_BitString* copy_bs (ANF_BitString *bs) {
    ANF_BitString *cp_bs = new_bitstring();

    for (int i = 0; i < bs->bstring->size; i++) {
        index(cp_bs->bstring, i, char) = index(bs->bstring, i, char);
    }

    cp_bs->bits_on = bs->bits_on;   

    return cp_bs;
}

void free_anf_array (ANF_Array *p) {
    array_free(p->xors);
    free(p);
}

void free_anf_array_deep (ANF_Array *p) {
    for (int i = 0; i < p->xors->size; i++) {
        if (dig_anf_bs(p, i) != NULL) {
            array_free(dig_anf_bs(p, i)->bstring);
            free(dig_anf_bs(p, i));
        }
    }

    free_anf_array(p);
}

void print_bs (ANF_BitString *bs) {
    if (bs->bits_on == 0 && index(bs->bstring, 0, char) == 0) {
        printf("1");
    } else {
        for (int j = 0; j < bs->bstring->size; j++) {
            if (index(bs->bstring, j, char) != 0)
                printf("%c", j);
        }        
    }
}

void print_anf_array (ANF_Array *p) {
    int i = 0;
    ANF_BitString *current_bs = NULL;

    while (i < p->components) {
        print_bs(get_anf_bs(p, i++));

        if (i < p->components)
            printf(" xor ");
    }
    printf("\n");
}

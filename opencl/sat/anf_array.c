//
//  anf_array.c
//  sat
//
//  Created by David Lilue on 23/06/2018.
//

#include <stdlib.h>
#include <stdio.h>
#include "anf_array.h"

void array_init(Array *a) {
    for (int i = 0; i < a->size; i++) {
        index(a, i, void *) = NULL;
    }
}

ANF_Array* new_anf_array (void) {
    ANF_Array *anf_a = (ANF_Array *) malloc(sizeof(ANF_Array));
    ANF_BitString *bs = (ANF_BitString *) malloc(sizeof(ANF_BitString));
    
    array_new(&bs->bstring, 128, char);
    for (int i = 0; i < 128; i++) {
        index(bs->bstring, i, char) = 0;
    }
    bs->bits_on = 0;
    
    array_new(&anf_a->xors, 7, ANF_BitString*);
    array_init(anf_a->xors);
    
    index(anf_a->xors, 0, ANF_BitString*) = bs;
    anf_a->components = 1;
    
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

//ANF_Array* xor_anf_array (ANF_Array *p, ANF_Array *q) {
//    
//}

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

void free_array(Array *a) {
    free(a->elements);
    free(a);
}

void free_anf_array (ANF_Array *p) {
    for (int i = 0; i < p->xors->size; i++) {
        if (index(p->xors, i, ANF_BitString*) != NULL) {
            free_array(index(p->xors, i, ANF_BitString*)->bstring);
            free(index(p->xors, i, ANF_BitString*));
        }
    }
    
    free_array(p->xors);
    free(p);
}

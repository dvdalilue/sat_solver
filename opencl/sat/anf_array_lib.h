//
//  anf_array_lib.h
//  sat
//
//  Created by David Lilue on 29/06/2018.
//

#include <stdlib.h>

#include "array.h"
#include "proposition.h"

#define dig_anf_bs(anf, i) index(anf->xors, i, ANF_BitString *)
#define get_bs_comp(anf, i, j) index(dig_anf_bs(anf, i)->bstring, j, char)

typedef enum {LT, GT, EQ} Ordering;

typedef struct _anf_bit_string {
    int bits_on;
    Array *bstring;
} ANF_BitString;

typedef struct _anf_xor_array {
    int components;
    Array *xors;
} ANF_Array;

ANF_Array* empty_anf_array (void);
ANF_BitString* new_bitstring (void);
ANF_Array* new_anf_array (void);
ANF_Array* one (void);
ANF_Array* zero (void);
ANF_Array* new_poly (int var);
ANF_BitString* get_anf_bs (ANF_Array *anf, int i);
Ordering compare_bs (ANF_BitString *x, ANF_BitString *y);
ANF_BitString* copy_bs (ANF_BitString *bs);
void add_xor_comp (ANF_Array *p, ANF_BitString *bs);
void free_anf_array (ANF_Array *p);
void free_anf_array_deep (ANF_Array *p);
void print_anf_array (ANF_Array *p);
void print_bs (ANF_BitString *bs);

//
//  anf_array.h
//  sat
//
//  Created by David Lilue on 23/06/2018.
//

#ifndef Array

#define index(a, i, t) ((t*) a->elements)[i]
#define array_new(a, a_size, type) ({ \
            *a = (Array *) malloc(sizeof(Array)); \
            (*a)->size = a_size; \
            (*a)->elements = (void *) malloc(sizeof(type) * a_size); \
        })
#define get_anf_bs(anf, i) index(anf->xors, i, ANF_BitString *)
#define get_bs(anf, i) get_anf_bs(anf, i)->bstring
#define get_bs_comp(anf, i, j) index(get_bs(anf, i), j, char)
#define min(a, b) a < b ? a : b

typedef enum {LT, GT, EQ} Ordering;

typedef struct _array {
    int size;
    void *elements;
} Array;

typedef struct _anf_bit_string {
    int bits_on;
    Array *bstring;
} ANF_BitString;

typedef struct _anf_xor_array {
    int components;
    Array *xors;
} ANF_Array;

void array_init (Array *ar);

ANF_Array* new_anf_array (void);
ANF_Array* one (void);
ANF_Array* zero (void);
ANF_Array* new_poly (int var);
ANF_Array* xor_anf_array (ANF_Array *p, ANF_Array *q);

Ordering compare_bs (ANF_BitString *x, ANF_BitString *y);

////ANF_Array* new_bin_anf (OpANF op, ANF_Array *p, ANF_Array *q);
//ANF_Array* and_anf (ANF_Array *p, ANF_Array *q);
//ANF_Array* distr_anf (ANF_Array *p, ANF_Array *q);
//ANF_Array* or_to_xor (ANF_Array *lhs, ANF_Array *rhs);
//ANF_Array* implie_to_xor (ANF_Array *lhs, ANF_Array *rhs);
//ANF_Array* eq_to_xor (ANF_Array *lhs, ANF_Array *rhs);
////ANF_Array* reduce (ANF_Array *p);
////ANF_Array* copy (ANF_Array *p);

void free_anf_array (ANF_Array *p);
//void anf_to_s (ANF_Array *p);

#endif /* array_h */

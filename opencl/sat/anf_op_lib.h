//
//  anf_opencl.h
//  sat
//
//  Created by David Lilue on 07/07/2018.
//

#ifndef ANF

#define BS_SIZE 35

typedef enum {LT, GT, EQ} Ordering;

typedef struct _anf_op {
    int capacity;
    int monomials;
    int *order;
    char *bstring;
} ANF;

char* new_bitstring (void);
ANF* empty_anf (void);
ANF* one (void);
ANF* new_poly (int var);
void add_xor_comp (ANF *p, char *bs);
void add_xor_comp_envious (ANF *p, char *bs);
char* get_anf_bs (ANF *anf, int i);
Ordering compare_bs (char *x, char *y);
void merge_sort_anf (ANF *p);
void free_anf (ANF *p);
void print_anf (ANF *p);
void print_bs (char *bs);

#endif /* ANF */

//
//  anf_opencl.h
//  sat
//
//  Created by David Lilue on 07/07/2018.
//

#ifndef ANF_OpenCL

#define BS_SIZE 128

typedef enum {LT, GT, EQ} Ordering;

typedef struct _anf_opencl {
    int capacity;
    int monomials;
    int *order;
    int *bits_on;
    char *bstring;
} ANF_OpenCL;

char* new_bitstring (void);
ANF_OpenCL* empty_anf (void);
ANF_OpenCL* one (void);
ANF_OpenCL* new_poly (int var);
void add_xor_comp (ANF_OpenCL *p, char *bs, int bits_on);
void add_xor_comp_envious (ANF_OpenCL *p, char *bs, int bits_on);
char* get_anf_bs (ANF_OpenCL *anf, int i);
Ordering compare_bs (char *x, char *y, int bits_x, int bits_y);
void merge_sort_anf (ANF_OpenCL *p);
void free_anf_opencl (ANF_OpenCL *p);
void print_anf_opencl (ANF_OpenCL *p);
void print_bs (char *bs, int bits_on);

#endif /* ANF_OpenCL */

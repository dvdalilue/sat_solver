//
//  anf_opencl.c
//  sat
//
//  Created by David Lilue on 07/07/2018.
//

#include <stdio.h>
#include <stdlib.h>

#include "anf_opencl.h"

char* new_bitstring (void) {
    char *bs = (char *) malloc(sizeof(char) * BS_SIZE);
    
    for (int i = 0; i < BS_SIZE; i++) { bs[i] = 0; }

    return bs;
}

void anf_init (ANF_OpenCL *polynomial, int capacity) {
    for (int i = polynomial->monomials; i < capacity; i++) {
        for (int j = 0; j < BS_SIZE; j++) {
            polynomial->bstring[j + i * BS_SIZE] = 0;
        }
        polynomial->bits_on[i] = 0;
        polynomial->order[i] = -1;
    }
}

ANF_OpenCL* empty_anf (void) {
    ANF_OpenCL *result = (ANF_OpenCL *) malloc(sizeof(ANF_OpenCL));
    
    int capacity = 7;
    
    result->capacity = capacity;
    result->monomials = 0;
    result->bits_on = (int *) malloc(sizeof(int) * capacity);
    result->order = (int *) malloc(sizeof(int) * capacity);
    result->bstring = (char *) malloc(sizeof(char) * capacity * BS_SIZE);

    anf_init(result, capacity);

    return result;
}

ANF_OpenCL* one (void) {
    ANF_OpenCL *result = empty_anf();
    
    result->monomials = 1;
    result->order[0] = 0;

    return result;
}

ANF_OpenCL* new_poly (int var) {
    ANF_OpenCL *poly = empty_anf();
    
    poly->monomials = 1;
    poly->bstring[var] = 1;
    poly->bits_on[0] = 1;
    poly->order[0] = 0;
    
    return poly;
}

void array_double_capacity_if_full (ANF_OpenCL *p) {
    if (p->monomials >= p->capacity) {
        p->capacity *= 2;
        p->bits_on = (int *) realloc(p->bits_on, sizeof(int) * p->capacity);
        p->order = (int *) realloc(p->order, sizeof(int) * p->capacity);
        p->bstring = (char *) realloc(p->bstring,
                                      sizeof(char) * p->capacity * BS_SIZE);
    }
    
    anf_init(p, p->capacity);
}

void add_xor_comp (ANF_OpenCL *p, char *bs, int bits_on) {
    int i = p->monomials * BS_SIZE,
    j = 0,
    bit_counter = 0;

    array_double_capacity_if_full(p);

    while (bit_counter < bits_on && j < BS_SIZE) {
        p->bstring[i++] = bs[j];
        if (bs[j++] == 1) bit_counter++;
    }
    p->order[p->monomials] = p->monomials;
    p->bits_on[p->monomials] = bits_on;
    p->monomials++;
}

void shift_left_from (ANF_OpenCL *p, int from) {
    char *left, *right;

    int mon_removed = p->order[from];

    for (int i = from; i < p->monomials - 1; i++) {
        p->order[i] = p->order[i+1];
        p->order[i] > mon_removed ? p->order[i]-- : 0;
    }

    p->order[p->monomials - 1] = -1;

    for (int i = mon_removed; i < p->monomials - 1; i++) {
        left = get_anf_bs(p, i);
        right = get_anf_bs(p, i + 1);

        for (int j = 0; j < BS_SIZE; j++) {
            left[j] = right[j];
        }

        p->bits_on[i] = p->bits_on[i + 1];
    }

    p->bits_on[p->monomials - 1] = 0;
}

int rm_bs (ANF_OpenCL *p, char *bs, int bits_on) {
    for (int i = 0; i < p->monomials; i++) {
        switch (compare_bs(get_anf_bs(p, p->order[i]), bs, p->bits_on[p->order[i]], bits_on)) {
            case EQ:
                shift_left_from(p, i);
                p->monomials--;
                return 1;
        }
    }
    return 0;
}

void add_xor_comp_envious (ANF_OpenCL *p, char *bs, int bits_on) {
    if (!rm_bs(p, bs, bits_on)) {
        add_xor_comp(p, bs, bits_on);
    }
}

char* get_anf_bs (ANF_OpenCL *anf, int i) {
    return (anf->bstring + i * BS_SIZE);
}

Ordering compare_bs (char *x, char *y, int bits_x, int bits_y) {
    if (bits_x < bits_y) {
        return LT;
    } else if (bits_x > bits_y) {
        return GT;
    } else if (bits_x != 0) {
        for (int i = 0; i < BS_SIZE; i++) {
            if (x[i] == 1 && y[i] == 0) {
                return LT;
            } else if (x[i] == 0 && y[i] == 1) {
                return GT;
            }
        }
    }

    return EQ;
}

void merge (ANF_OpenCL *p, int a, int b, int c) {
    int n_ls = b - a + 1;
    int n_rs = c - b;
    int i, j, k;

    int *left = (int *) malloc(sizeof(int) * n_ls);
    int *right = (int *) malloc(sizeof(int) * n_rs);

    for (i = 0; i < n_ls; i++)
        left[i] = p->order[a + i];

    for (j = 0; j < n_rs; j++)
        right[j] = p->order[b + 1 + j];

    i = j = 0;
    k = a;

    while (i < n_ls && j < n_rs) {
        switch (compare_bs(
                           get_anf_bs(p, left[i]),
                           get_anf_bs(p, right[j]),
                           p->bits_on[left[i]],
                           p->bits_on[right[j]])) {
            case LT:
            case EQ:
                p->order[k++] = left[i++];
                break;
            case GT:
                p->order[k++] = right[j++];
                break;
        }
    }
    
    while (i < n_ls) p->order[k++] = left[i++];
    while (j < n_rs) p->order[k++] = right[j++];
    
    free(left);
    free(right);
}

void merge_sort_aux (ANF_OpenCL *p, int a, int c) {
    if (a < c) {
        int b = (a + c) / 2;
        merge_sort_aux(p, a, b);
        merge_sort_aux(p, b + 1, c);
        merge(p, a, b, c);
    }
}

void merge_sort_anf (ANF_OpenCL *p) {
    merge_sort_aux(p, 0, p->monomials - 1);
}

void free_anf_opencl (ANF_OpenCL *p) {
    free(p->bits_on);
    free(p->order);
    free(p->bstring);
    free(p);
}

void print_anf_opencl (ANF_OpenCL *p) {
    for (int k = 0; k < p->monomials; k++) {
        print_bs(get_anf_bs(p, k));

        if (k < p->monomials - 1) {
            printf(" xor ");
        }
    }
    printf("\n");
}

void print_bs (char *bs) {
    for (int i = 0; i < BS_SIZE; i++) {
        if (bs[i] == 1)
            printf("%c", i);
    }
}

//
//  anf_opencl.c
//  sat
//
//  Created by David Lilue on 07/07/2018.
//

#include <stdio.h>
#include <stdlib.h>

#include "anf_op_lib.h"

/*
 * Creates and initialize and new bit string of size 'BS_SIZE'
 */
char* new_bitstring (void) {
    char *bs = (char *) malloc(sizeof(char) * BS_SIZE);

    for (int i = 0; i < BS_SIZE; i++) { bs[i] = 0; }

    return bs;
}

/*
 * Initialize an ANF polynomial from the current free monomial
 * space.
 */
void anf_init (ANF *polynomial, int capacity) {
    for (int i = polynomial->monomials; i < capacity; i++) {
        for (int j = 0; j < BS_SIZE; j++) {
            polynomial->bstring[j + i * BS_SIZE] = 0;
        }
        polynomial->order[i] = -1;
    }
}

/*
 * Creates a new polynomial with capacity of 7 monomials.
 */
ANF* empty_anf (void) {
    ANF *result = (ANF *) malloc(sizeof(ANF));

    int capacity = 7;

    result->capacity = capacity;
    result->monomials = 0;
    result->order = (int *) malloc(sizeof(int) * capacity);
    result->bstring = (char *) malloc(sizeof(char) * capacity * BS_SIZE);

    anf_init(result, capacity);

    return result;
}

/*
 * Creates a polynomial with only one monomial representing 1.
 */
ANF* one (void) {
    ANF *result = empty_anf();

    result->monomials = 1;
    result->order[0] = 0;

    return result;
}

/*
 * Creates a polynomial turning on the bit 'var' in the first
 * monomial.
 */
ANF* new_poly (int var) {
    ANF *poly = empty_anf();

    poly->monomials = 1;
    poly->bstring[var] = 1;
    poly->order[0] = 0;

    return poly;
}

/*
 * Checks if the polynomial has filled its capacity with
 * monomials and doubles it.
 */
void array_double_capacity_if_full (ANF *p) {
    if (p->monomials >= p->capacity) {
        p->capacity *= 2;
        p->order = (int *) realloc(p->order, sizeof(int) * p->capacity);
        p->bstring = (char *) realloc(p->bstring,
                                      sizeof(char) * p->capacity * BS_SIZE);
    }

    anf_init(p, p->capacity);
}

/*
 * Adds a bit string to the polynomial
 */
void add_xor_comp (ANF *p, char *bs) {
    int i = p->monomials * BS_SIZE,
        j = 0;

    array_double_capacity_if_full(p);

    while (j < BS_SIZE) {
        p->bstring[i++] = bs[j++];
    }

    p->order[p->monomials] = p->monomials;
    p->monomials++;
}

/*
 * Removes the monomial in the position 'from' by shifting left
 * the bit strings and their corresponding order.
 */
void shift_left_from (ANF *p, int from) {
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
    }
}

/*
 * Removes the first monomial in 'p' equal to 'bs'. Returning 1
 * if it's removed, 0 otherwise.
 */
int rm_bs (ANF *p, char *bs) {
    for (int i = 0; i < p->monomials; i++) {
        switch (compare_bs(get_anf_bs(p, p->order[i]), bs)) {
            case EQ:
                shift_left_from(p, i);
                p->monomials--;
                return 1;
        }
    }
    return 0;
}

/*
 * Adds a monomial to the polynomial if it is not present,
 * otherwise removing the first monomial equal to 'bs'.
 */
void add_xor_comp_envious (ANF *p, char *bs) {
    if (!rm_bs(p, bs)) {
        add_xor_comp(p, bs);
    }
}

/*
 * Gets the memory pointer of the bit string in the position
 * 'i' of the polynomial.
 */
char* get_anf_bs (ANF *anf, int i) {
    return (anf->bstring + i * BS_SIZE);
}

/*
 * Compare two bit strings using lexical order. Returning 'LT'
 * if x < y, 'GT' if x > y and 'EQ' if x == y.
 */
Ordering compare_bs (char *x, char *y) {
    for (int i = 0; i < BS_SIZE; i++) {
        if (x[i] == 1 && y[i] == 0) {
            return LT;
        } else if (x[i] == 0 && y[i] == 1) {
            return GT;
        }
    }

    return EQ;
}

/*
 * Merge algorithm from MergeSort
 */
void merge (ANF *p, int a, int b, int c) {
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
        switch (compare_bs(get_anf_bs(p, left[i]),
                           get_anf_bs(p, right[j]))) {
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

/*
 * Auxiliary MergeSort function
 */
void merge_sort_aux (ANF *p, int a, int c) {
    if (a < c) {
        int b = (a + c) / 2;
        merge_sort_aux(p, a, b);
        merge_sort_aux(p, b + 1, c);
        merge(p, a, b, c);
    }
}

/*
 * MergeSort for ANF
 */
void merge_sort_anf (ANF *p) {
    merge_sort_aux(p, 0, p->monomials - 1);
}

/*
 * Free the memory used by a polynomial ANF
 */
void free_anf (ANF *p) {
    free(p->order);
    free(p->bstring);
    free(p);
}

/*
 * Prints a polynomial. '0' is printed if there is no monomials.
 */
void print_anf (ANF *p) {
    for (int k = 0; k < p->monomials; k++) {
        print_bs(get_anf_bs(p, p->order[k]));

        if (k < p->monomials - 1) {
            printf(" xor ");
        }
    }

    if (p->monomials == 0) { printf("0"); }
    printf("\n");
}

/*
 * Prints a bit string, enclosing turned on bits in
 * parentheses and separinting each monomial with 'xor'. If
 * there is no bit turned on, '1' is printed.
 */
void print_bs (char *bs) {
    int bit_counter = 0;

    for (int i = 0; i < BS_SIZE; i++) {
        if (bs[i] == 1) {
            printf("(%d)", i);
            bit_counter++;
        }
    }
    if (bit_counter == 0) printf("1");
}

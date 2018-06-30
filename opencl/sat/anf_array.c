//
//  anf_array.c
//  sat
//
//  Created by David Lilue on 23/06/2018.
//

#include <stdlib.h>
#include <stdio.h>
#include "anf_array.h"

ANF_Array* xor_anf_array (ANF_Array *p, ANF_Array *q) {
    ANF_Array *merge = empty_anf_array();

    int i = 0,
        j = 0;

    while (i < p->components && j < q->components) {
        switch (compare_bs(get_anf_bs(p, i), get_anf_bs(q, j))) {
            case EQ:
                i++; j++;
                break;
            case LT:
                add_xor_comp(merge, copy_bs(get_anf_bs(p, i++)));
                break;
            case GT:
                add_xor_comp(merge, copy_bs(get_anf_bs(q, j++)));
                break;
        }
    }

    while (i < p->components)
        add_xor_comp(merge, copy_bs(get_anf_bs(p, i++)));

    while (j < q->components)
        add_xor_comp(merge, copy_bs(get_anf_bs(q, j++)));

    return merge;
}

void merge (ANF_Array *p, int a, int b, int c) {
    int n_ls = b - a + 1;
    int n_rs = c - b;
    int i, j, k;

    ANF_BitString **left =
        (ANF_BitString **) malloc(sizeof(ANF_BitString*) * n_ls);
    ANF_BitString **right =
        (ANF_BitString **) malloc(sizeof(ANF_BitString*) * n_rs);

    for (i = 0; i < n_ls; i++)
        left[i] = get_anf_bs(p, a + i);

    for (j = 0; j < n_rs; j++)
        right[j] = get_anf_bs(p, b + 1 + j);

    i = j = 0;
    k = a;

    while (i < n_ls && j < n_rs) {
        switch (compare_bs(left[i], right[j])) {
            case EQ:
            case LT:
                dig_anf_bs(p, k++) = left[i++];
                break;
            case GT:
                dig_anf_bs(p, k++) = right[j++];
                break;
        }
    }

    while (i < n_ls) dig_anf_bs(p, k++) = left[i++];
    while (j < n_rs) dig_anf_bs(p, k++) = right[j++];

    free(left);
    free(right);
}

void merge_sort_aux (ANF_Array *p, int a, int c) {
    if (a < c) {
        int b = (a + c) / 2;
        merge_sort_aux(p, a, b);
        merge_sort_aux(p, b + 1, c);
        merge(p, a, b, c);
    }
}

void merge_sort_anf (ANF_Array *p) {
    merge_sort_aux(p, 0, p->xors->size - 1);
}

ANF_Array* map_anf_bs (ANF_BitString *bs, ANF_Array *p) {
    ANF_Array *maped = empty_anf_array();

    for (int i = 0; i < p->xors->size; i++) {
        add_xor_comp(maped, and_bs(get_anf_bs(p, i), bs));
    }

    return maped;
}

ANF_BitString* and_bs (ANF_BitString *x, ANF_BitString *y) {
    ANF_BitString* and = new_bitstring();

    for (int i = 0; i < x->bstring->size; i++) {
        if (index(x->bstring, i, char) | index(y->bstring, i, char)) {
            index(and->bstring, i, char) = 1;
            and->bits_on++;
        }
    }

    return and;
}

ANF_Array* and_anf_array (ANF_Array *p, ANF_Array *q) {
    ANF_Array *result = empty_anf_array();
    ANF_Array *prev = NULL;
    ANF_Array *maped = NULL;
    for (int i = 0; i < p->xors->size; i++) {
        prev = result;
        maped = map_anf_bs(get_anf_bs(p, i), q);
        merge_sort_anf(maped);
        result = xor_anf_array(prev, maped);
        free_anf_array_deep(maped);
        free_anf_array_deep(prev);
    }

    return result;
}

ANF_Array* copy_anf_array (ANF_Array *p) {
    ANF_Array *cp_anf = empty_anf_array();

    for (int i = 0; i < p->components; i++) {
        add_xor_comp(cp_anf, copy_bs(get_anf_bs(p, i)));
    }

    return cp_anf;
}

ANF_Array* or_to_xor_array (ANF_Array *lhs, ANF_Array *rhs) {
    ANF_Array *result = NULL,
              *tmp = NULL;

    tmp = and_anf_array(lhs, rhs);
    result = xor_anf_array(rhs, tmp);
    free_anf_array_deep(tmp);
    tmp = result;
    result = xor_anf_array(lhs, tmp);
    free_anf_array_deep(tmp);

    return result;
}

ANF_Array* implie_to_xor_array (ANF_Array *lhs, ANF_Array *rhs) {
    ANF_Array *result = NULL,
              *tmp = NULL,
              *local_one = one();

    tmp = and_anf_array(lhs, rhs);
    result = xor_anf_array(lhs, tmp);
    free_anf_array_deep(tmp);
    tmp = result;
    result = xor_anf_array(local_one, tmp);
    free_anf_array_deep(tmp);
    free_anf_array_deep(local_one);

    return result;
}

ANF_Array* eq_to_xor_array (ANF_Array *lhs, ANF_Array *rhs) {
    ANF_Array *result = NULL,
              *aux = NULL,
              *tmp_l = NULL,
              *tmp_r = NULL,
              *local_one = one();

    aux = and_anf_array(lhs, rhs);
    tmp_l = xor_anf_array(lhs, aux);
    free_anf_array_deep(aux);
    aux = tmp_l;
    tmp_l = xor_anf_array(local_one, aux);
    free_anf_array_deep(aux);

    aux = and_anf_array(rhs, lhs);
    tmp_r = xor_anf_array(rhs, aux);
    free_anf_array_deep(aux);
    aux = tmp_r;
    tmp_r = xor_anf_array(local_one, aux);
    free_anf_array_deep(aux);

    result = and_anf_array(tmp_l, tmp_r);
    free_anf_array_deep(tmp_l);
    free_anf_array_deep(tmp_r);
    free_anf_array_deep(local_one);

    return result;
}

ANF_Array* anf_array (Proposition *p) {
    ANF_Array *lhs_anf = NULL;
    ANF_Array *rhs_anf = NULL;
    ANF_Array *result = NULL;

    switch (p->kind) {
        case 0: // Statement
            return new_poly((int) value(p));
        case 1: // Negation
            rhs_anf = anf_array(desneg(p));
            lhs_anf = one();
            result = xor_anf_array(lhs_anf, rhs_anf);
            free_anf_array_deep(rhs_anf);
            free_anf_array_deep(lhs_anf);
            break;
        case 2: // Binary operation
            lhs_anf = anf_array(lhs(p));
            rhs_anf = anf_array(rhs(p));
            switch (op(p)) {
                case AND:
                    result = and_anf_array(lhs_anf, rhs_anf);
                    break;
                case OR:
                    result = or_to_xor_array(lhs_anf, rhs_anf);
                    break;
                case IMPLIE:
                    result = implie_to_xor_array(lhs_anf, rhs_anf);
                    break;
                case EQUAL:
                    result = eq_to_xor_array(lhs_anf, rhs_anf);
                    break;
            }
            free_anf_array_deep(lhs_anf);
            free_anf_array_deep(rhs_anf);
    }
    print_anf_array(result);
    return result;
}

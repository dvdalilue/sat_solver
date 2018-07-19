//
//  anf_op.c
//  sat
//
//  Created by David Lilue on 08/07/2018.
//

#include <stdlib.h>
#include <stdio.h>
#include "anf_op.h"

char* and_bs (char *x, char *y);

/*
 * Takes two polynomials and merge (similar to MergeSort) both
 * into a new one. It is assumed that both are ordered and,
 * when equal monomials are found, they're ignored from the new
 * polynomial.
 *
 * Note: each monomial is unique in its polynomial.
 */
ANF* xor_anf_op (ANF *p, ANF *q) {
    ANF *merge = empty_anf();

    int i = 0,
        j = 0;

    while (i < p->monomials && j < q->monomials) {
        switch (compare_bs(get_anf_bs(p, p->order[i]),
                           get_anf_bs(q, q->order[j]))) {
            case EQ:
                i++; j++;
                break;
            case LT:
                add_xor_comp(merge, get_anf_bs(p, p->order[i]));
                i++;
                break;
            case GT:
                add_xor_comp(merge, get_anf_bs(q, q->order[j]));
                j++;
                break;
        }
    }

    while (i < p->monomials) {
        add_xor_comp(merge, get_anf_bs(p, p->order[i]));
        i++;
    }

    while (j < q->monomials) {
        add_xor_comp(merge, get_anf_bs(q, q->order[j]));
        j++;
    }

    return merge;
}

/*
 * Similar to the distributive property, makes the 'AND'
 * operation between each monomial and 'bs', returning a new
 * polynomial with unique monomials but disordered.
 */
ANF* map_anf_bs (char *bs, ANF *p) {
    ANF *result = empty_anf();
    char *aux = NULL;

    for (int i = 0; i < p->monomials; i++) {
        aux = and_bs(get_anf_bs(p, i), bs);
        add_xor_comp_envious(result, aux);
        free(aux);
    }

    return result;
}

/*
 * Returns a bit string resulting from turning on the turned on
 * bits from 'x' and 'y'.
 */
char* and_bs (char *x, char *y) {
    char *result = (char *) malloc(sizeof(char) * BS_SIZE);

    for (int i = 0; i < BS_SIZE; i++) {
        result[i] = x[i] | y[i];
    }

    return result;
}

/*
 * This function computes the multiplication beteween the
 * polynomials 'p' and 'q'. Using a classic and naive approach
 * resulting in time complexity equal to O(n^2).
 */
ANF* and_anf_op (ANF *p, ANF *q) {
    if (p->monomials == 0 || q->monomials == 0)
        return empty_anf();

    ANF *result = map_bs_gpu(get_anf_bs(p, 0), q);
    merge_sort_anf(result);
    ANF *prev = NULL;
    ANF *maped = NULL;

    for (int i = 1; i < p->monomials; i++) {
        prev = result;
        maped = map_bs_gpu(get_anf_bs(p, i), q);
        merge_sort_anf(maped);
        result = xor_anf_op(prev, maped);
        free_anf(maped);
        free_anf(prev);
    }

    return result;
}

/*
 * Returns the equivaltent form in ANF from the 'OR' operation
 * between two previously transformed proposition into
 * polynomials.
 *
 * Note:
 *
 *     P 'Or' Q ==> P 'xor' Q 'xor' 'PQ'
 */
ANF* or_to_xor_op (ANF *lhs, ANF *rhs) {
    ANF *result = NULL,
        *tmp = NULL;

    tmp = and_anf_op(lhs, rhs);
    result = xor_anf_op(rhs, tmp);
    free_anf(tmp);
    tmp = result;
    result = xor_anf_op(lhs, tmp);
    free_anf(tmp);

    return result;
}

/*
 * Returns the equivaltent form in ANF from the 'IMPLIE'
 * operation between two previously transformed proposition
 * into polynomials.
 *
 * Note:
 *
 *     P 'Implie' Q ==> 1 'xor' P 'xor' 'PQ'
 */
ANF* implie_to_xor_op (ANF *lhs, ANF *rhs) {
    ANF *result = NULL,
        *tmp = NULL,
        *local_one = one();

    tmp = and_anf_op(lhs, rhs);
    result = xor_anf_op(lhs, tmp);
    free_anf(tmp);
    tmp = result;
    result = xor_anf_op(local_one, tmp);
    free_anf(tmp);
    free_anf(local_one);

    return result;
}

/*
 * Returns the equivaltent form in ANF from the 'EQ'
 * operation between two previously transformed proposition
 * into polynomials.
 *
 * Note:
 *
 *     P 'Eq' Q ==> 1 'xor' Q 'xor' 'PQ'
 */
ANF* eq_to_xor_op (ANF *lhs, ANF *rhs) {
    ANF *result = NULL,
        *tmp = NULL,
        *local_one = one();

    tmp = and_anf_op(lhs, rhs);
    result = xor_anf_op(rhs, tmp);
    free_anf(tmp);
    tmp = result;
    result = xor_anf_op(local_one, tmp);
    free_anf(tmp);
    free_anf(local_one);

    return result;
}

/*
 * Transform a proposition into a polynomial in ANF. Going
 * recursively to the leafs and reconstructing the polynomial
 * depending on the operation.
 *
 * Note:
 *
 *     'Neg' P ==> 1 'xor' P
 */
ANF* anf_op (Proposition *p) {
    ANF *lhs_anf = NULL;
    ANF *rhs_anf = NULL;
    ANF *result = NULL;

    switch (p->kind) {
        case 0: // Statement
            return new_poly((int) value(p));
        case 1: // Negation
            rhs_anf = anf_op(desneg(p));
            lhs_anf = one();
            result = xor_anf_op(lhs_anf, rhs_anf);
            free_anf(rhs_anf);
            free_anf(lhs_anf);
            break;
        case 2: // Binary operation
            lhs_anf = anf_op(lhs(p));
            rhs_anf = anf_op(rhs(p));
            switch (op(p)) {
                case AND:
                    result = and_anf_op(lhs_anf, rhs_anf);
                    break;
                case OR:
                    result = or_to_xor_op(lhs_anf, rhs_anf);
                    break;
                case IMPLIE:
                    result = implie_to_xor_op(lhs_anf, rhs_anf);
                    break;
                case EQUAL:
                    result = eq_to_xor_op(lhs_anf, rhs_anf);
                    break;
            }
            free_anf(lhs_anf);
            free_anf(rhs_anf);
    }
    return result;
}

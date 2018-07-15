//
//  anf_op.c
//  sat
//
//  Created by David Lilue on 08/07/2018.
//

#include <stdlib.h>
#include <stdio.h>
#include "anf_op.h"

char* and_bs (char *x, char *y, int *bits_on);

ANF* xor_anf_op (ANF *p, ANF *q) {
    ANF *merge = empty_anf();

    int i = 0,
        j = 0;

    while (i < p->monomials && j < q->monomials) {
        switch (compare_bs(
                            get_anf_bs(p, p->order[i]),
                            get_anf_bs(q, q->order[j]),
                            p->bits_on[p->order[i]],
                            q->bits_on[q->order[j]])) {
            case EQ:
                i++; j++;
                break;
            case LT:
                add_xor_comp(merge, get_anf_bs(p, p->order[i]), p->bits_on[p->order[i]]);
                i++;
                break;
            case GT:
                add_xor_comp(merge, get_anf_bs(q, q->order[j]), q->bits_on[q->order[j]]);
                j++;
                break;
        }
    }

    while (i < p->monomials) {
        add_xor_comp(merge, get_anf_bs(p, p->order[i]), p->bits_on[p->order[i]]);
        i++;
    }

    while (j < q->monomials) {
        add_xor_comp(merge, get_anf_bs(q, q->order[j]), q->bits_on[q->order[j]]);
        j++;
    }

    return merge;
}

ANF* map_anf_bs (char *bs, ANF *p) {
    ANF *result = empty_anf();
    char *aux = NULL;
    int bts;

    for (int i = 0; i < p->monomials; i++) {
        bts = 0;
        aux = and_bs(get_anf_bs(p, i), bs, &bts);
        add_xor_comp_envious(result, aux, bts);
        free(aux);
    }

    return result;
}

char* and_bs (char *x, char *y, int *bits_on) {
    char *result = (char *) malloc(sizeof(char) * BS_SIZE);

    for (int i = 0; i < BS_SIZE; i++) {
        if (result[i] = x[i] | y[i]) (*bits_on)++;
    }

    return result;
}

ANF* and_anf_op (ANF *p, ANF *q) {
    if (p->monomials == 0 || q->monomials == 0)
        return empty_anf();

    ANF *result = map_anf_bs(get_anf_bs(p, 0), q);
    ANF *prev = NULL;
    ANF *maped = NULL;

    for (int i = 1; i < p->monomials; i++) {
        prev = result;
        maped = map_anf_bs(get_anf_bs(p, i), q);
        merge_sort_anf(maped);
        result = xor_anf_op(prev, maped);
        free_anf_opencl(maped);
        free_anf_opencl(prev);
    }

    return result;
}

ANF* or_to_xor_op (ANF *lhs, ANF *rhs) {
    ANF *result = NULL,
               *tmp = NULL;

    tmp = and_anf_op(lhs, rhs);
    result = xor_anf_op(rhs, tmp);
    free_anf_opencl(tmp);
    tmp = result;
    result = xor_anf_op(lhs, tmp);
    free_anf_opencl(tmp);

    return result;
}

ANF* implie_to_xor_op (ANF *lhs, ANF *rhs) {
    ANF *result = NULL,
              *tmp = NULL,
              *local_one = one();

    tmp = and_anf_op(lhs, rhs);
    result = xor_anf_op(lhs, tmp);
    free_anf_opencl(tmp);
    tmp = result;
    result = xor_anf_op(local_one, tmp);
    free_anf_opencl(tmp);
    free_anf_opencl(local_one);

    return result;
}

ANF* eq_to_xor_op (ANF *lhs, ANF *rhs) {
    ANF *result = NULL,
              *aux = NULL,
              *tmp_l = NULL,
              *tmp_r = NULL,
              *local_one = one();

    aux = and_anf_op(lhs, rhs);
    tmp_l = xor_anf_op(lhs, aux);
    free_anf_opencl(aux);
    aux = tmp_l;
    tmp_l = xor_anf_op(local_one, aux);
    free_anf_opencl(aux);

    aux = and_anf_op(rhs, lhs);
    tmp_r = xor_anf_op(rhs, aux);
    free_anf_opencl(aux);
    aux = tmp_r;
    tmp_r = xor_anf_op(local_one, aux);
    free_anf_opencl(aux);

    result = and_anf_op(tmp_l, tmp_r);
    free_anf_opencl(tmp_l);
    free_anf_opencl(tmp_r);
    free_anf_opencl(local_one);

    return result;
}

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
            free_anf_opencl(rhs_anf);
            free_anf_opencl(lhs_anf);
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
            free_anf_opencl(lhs_anf);
            free_anf_opencl(rhs_anf);
    }
    return result;
}

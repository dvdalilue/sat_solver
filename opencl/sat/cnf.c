//
//  cnf.c
//  sat
//
//  Created by David Lilue on 16/06/2018.
//

#include <stdlib.h>
#include "cnf.h"

/*
 * Apply the distributive property 'OR over AND', assuming that
 * the given propositions 'p' and 'q' correspond to the left
 * and right side, respectevily, of a binary operation 'OR'. If
 * 'p' or 'q' does not match with the binary operation 'AND',
 * an 'OR' is constructed.
 */
Proposition* distr_cnf (Proposition *p, Proposition *q) {
    Proposition *result = NULL;
    Proposition *lhs_and = NULL,
                *rhs_and = NULL;

    if (p->kind == 2 && op(p) == AND) {
        // The recursive call is assigned to a variable because in Linux the
        // order is not determistic, or is reversed from Mac OSX. Same in the
        // other conditional branch.
        lhs_and = distr_cnf(lhs(p), copy_prop(q));
        rhs_and = distr_cnf(rhs(p), q);

        result = new_bin(AND, lhs_and, rhs_and);
        free_bin(p);
    } else if (q->kind == 2 && op(q) == AND) {
        lhs_and = distr_cnf(copy_prop(p), lhs(q));
        rhs_and = distr_cnf(p, rhs(q));

        result = new_bin(AND, lhs_and, rhs_and);
        free_bin(q);
    } else {
        result = new_bin(OR, p, q);
    }
    return result;
}

/*
 * Goes recursively through the proposition and apply the
 * distributive property when an 'OR' is found. Returning the
 * CNF of the proposition.
 */
Proposition* cnf_aux (Proposition *p) {
    Proposition *result = p;

    if (p->kind == 2) {
        if (op(p) == AND) {
            result = new_bin(AND, cnf_aux(lhs(p)), cnf_aux(rhs(p)));
        } else {
            result = distr_cnf(cnf_aux(lhs(p)), cnf_aux(rhs(p)));
        }
        free_bin(p);
    }
    return result;
}

/*
 * In order to have a proposition in CNF, it should be in NNF
 * before.
 */
Proposition* cnf (Proposition *p) {
    return cnf_aux(nnf(p));
}

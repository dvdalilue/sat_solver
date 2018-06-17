//
//  cnf.c
//  sat
//
//  Created by David Lilue on 16/06/2018.
//

#include <stdlib.h>
#include "cnf.h"

Proposition* distr_cnf (Proposition *p, Proposition *q) {
    Proposition *result = NULL;
    
    if (p->kind == 2 && op(p) == AND) {
        result = new_bin(AND, distr_cnf(lhs(p), q), distr_cnf(rhs(p), q));
        free_bin(p);
    } else if (q->kind == 2 && op(q) == AND) {
        result = new_bin(AND, distr_cnf(p, lhs(q)), distr_cnf(p, rhs(q)));
        free_bin(q);
    } else {
        result = new_bin(OR, p, q);
    }
    return result;
}

Proposition* cnf (Proposition *p) {
    Proposition *result;
    p = nnf(p);
    result = p;
    if (p->kind == 2) {
        if (op(p) == AND) {
            result = new_bin(AND, cnf(lhs(p)), cnf(rhs(p)));
        } else {
            result = distr_cnf(cnf(lhs(p)), cnf(rhs(p)));
        }
        free_bin(p);
    }
    return result;
}

//
//  cnf.c
//  sat
//
//  Created by David Lilue on 16/06/2018.
//

#include <stdlib.h>
#include "cnf.h"

Proposition* distr (Proposition *p, Proposition *q) {
    Proposition *result = NULL;
    
    if (p->kind == 2 && op(p) == AND) {
        result = new_bin(AND, distr(lhs(p), q), distr(rhs(p), q));
        free_bin(p);
    } else if (q->kind == 2 && op(p) == AND) {
        result = new_bin(AND, distr(p, lhs(q)), distr(p, rhs(q)));
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
            result = distr(cnf(lhs(p)), cnf(rhs(p)));
        }
        free_bin(p);
    }
    return result;
}

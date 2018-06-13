//
//  main.c
//  sat
//
//  Created by David Lilue on 05/06/2018.
//

#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include "proposition.h"

Proposition* impl_free (Proposition *p) {
    Proposition *actual = p;
    Proposition *result = p;

    switch (actual->kind) {
        case 1:
            result = new_neg(impl_free(unneg(actual)));
            free_neg(actual);
            break;
        case 2:
            if (op(actual) == IMPLIE) {
                result = new_bin(OR,
                           new_neg(impl_free(lhs(actual))),
                                impl_free(rhs(actual))
                        );
            } else if (op(actual) == EQ) {
                result = new_bin(AND,
                           new_bin(OR,
                                new_neg(impl_free(lhs(actual))),
                                     impl_free(rhs(actual))),
                           new_bin(OR,
                                new_neg(impl_free(rhs(actual))),
                                    impl_free(lhs(actual)))
                           );
            } else {
                result = new_bin(op(actual),
                           impl_free(lhs(actual)),
                           impl_free(rhs(actual)));
            }
            free_bin(actual);
            break;
    }
    return result;
}

Proposition *nnf (Proposition *p) {
    Proposition *actual = impl_free(p);
    Proposition *result = actual;
    
    switch (actual->kind) {
        case 1:
            switch (unneg(actual)->kind) {
                case 1:
                    result = nnf(unneg(unneg(actual)));
                    free_neg(unneg(actual));
                    free_neg(actual);
                    break;
                case 2:
                    switch (op(unneg(actual))) {
                        case AND:
                            result = new_bin(OR, nnf(new_neg(lhs(unneg(actual)))), nnf(new_neg(rhs(unneg(actual)))));
                            break;
                        case OR:
                            result = new_bin(AND, nnf(new_neg(lhs(unneg(actual)))), nnf(new_neg(rhs(unneg(actual)))));
                            break;
                        default:
                            result = new_bin(op(unneg(actual)), nnf(lhs(unneg(actual))), nnf(rhs(unneg(actual))));
                            break;
                    }
                    free_bin(unneg(actual));
                    free_neg(actual);
                    break;
            }
            break;
        case 2:
            result = new_bin(op(actual),
                             nnf(lhs(actual)),
                             nnf(rhs(actual)));
            free_bin(actual);
            break;
    }
    return result;
}

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
            // TODO: free memory
        } else {
            result = distr(cnf(lhs(p)), cnf(rhs(p)));
        }
    }
    return result;
}

int main (int argc, const char * argv[]) {
//    Proposition *tmp = NULL;
    Proposition *modus_ponen = new_bin(IMPLIE,
                                       new_bin(AND,
                                           new_bin(IMPLIE,
                                               STM('P'),
                                               STM('Q')),
                                           STM('P')),
                                       STM('Q'));

    modus_ponen = cnf(modus_ponen);
    prop_to_s(modus_ponen);
    fprintf(stdout, "\n");
    destroy_prop(modus_ponen);
    
    return 0;
}

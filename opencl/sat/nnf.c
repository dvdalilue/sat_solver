//
//  nnf.c
//  sat
//
//  Created by David Lilue on 16/06/2018.
//

#include <stdio.h>
#include <stdlib.h>
#include "nnf.h"

/*
 * Implication elimination from a proposition following the
 * theorem 'P -> Q == Neg P Or Q'. Going recursively through
 * the propostion and changing when needed. In case of
 * equivalence, a conjunction of implications (both directions)
 * is used and replacing them with the previous theorem.
 */
Proposition* impl_free (Proposition *p) {
    switch (p->kind) {
        case 0:
            return STM(value(p));
        case 1:
            return new_neg(impl_free(desneg(p)));
        case 2:
            if (op(p) == IMPLIE) {
                return new_bin(OR,
                           new_neg(impl_free(lhs(p))),
                                impl_free(rhs(p))
                        );
            } else if (op(p) ==  EQUAL) {
                return new_bin(AND,
                           new_bin(OR,
                                new_neg(impl_free(rhs(p))),
                                impl_free(lhs(p))),
                           new_bin(OR,
                                new_neg(impl_free(lhs(p))),
                                impl_free(rhs(p)))
                           );
            } else {
                return new_bin(op(p),
                            impl_free(lhs(p)),
                            impl_free(rhs(p)));
            }
    }
    return NULL; // This should never happend
}

/*
 * Takes a proposition and removes double negation while
 * apllying the DeMorgan's theorems
 */
Proposition* nnf_aux (Proposition *p) {
    Proposition *current = p;
    Proposition *result = current;

    switch (current->kind) {
        case 1:
            switch (desneg(current)->kind) {
                case 1: // Neg Neg P == P
                    result = nnf_aux(desneg(desneg(current)));
                    free_neg(desneg(current));
                    free_neg(current);
                    break;
                case 2:
                    switch (op(desneg(current))) {
                        case AND: // Neg (P And Q) == Neg P Or Neg Q
                            result = new_bin(OR,
                                        nnf_aux(new_neg(lhs(desneg(current)))),
                                        nnf_aux(new_neg(rhs(desneg(current)))));
                            break;
                        case OR: // Neg (P Or Q) == Neg P And Neg Q
                            result = new_bin(AND,
                                        nnf_aux(new_neg(lhs(desneg(current)))),
                                        nnf_aux(new_neg(rhs(desneg(current)))));
                            break;
                        default:
                            result = new_neg(
                                        new_bin(op(desneg(current)),
                                            nnf_aux(lhs(desneg(current))),
                                            nnf_aux(rhs(desneg(current)))));
                            break;
                    }
                    free_bin(desneg(current));
                    free_neg(current);
                    break;
            }
            break;
        case 2:
            result = new_bin(op(current),
                             nnf_aux(lhs(current)),
                             nnf_aux(rhs(current)));
            free_bin(current);
            break;
    }
    return result;
}

/*
 * In order to have a proposition in NNF, it should be without
 * implications before.
 */
Proposition* nnf (Proposition *p) {
    return nnf_aux(impl_free(p));
}

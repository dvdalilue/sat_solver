//
//  nnf.c
//  sat
//
//  Created by David Lilue on 16/06/2018.
//

#include <stdio.h>
#include <stdlib.h>
#include "nnf.h"

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
            } else if (op(p) ==  EQ) {
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

Proposition* nnf (Proposition *p) {
    Proposition *current = impl_free(p);
    Proposition *result = current;
    
    switch (current->kind) {
        case 1:
            switch (desneg(current)->kind) {
                case 1:
                    result = nnf(desneg(desneg(current)));
                    free_neg(desneg(current));
                    free_neg(current);
                    break;
                case 2:
                    switch (op(desneg(current))) {
                        case AND:
                            result = new_bin(OR,
                                        nnf(new_neg(lhs(desneg(current)))),
                                        nnf(new_neg(rhs(desneg(current)))));
                            break;
                        case OR:
                            result = new_bin(AND,
                                        nnf(new_neg(lhs(desneg(current)))),
                                        nnf(new_neg(rhs(desneg(current)))));
                            break;
                        default:
                            result = new_bin(op(desneg(current)),
                                        nnf(lhs(desneg(current))),
                                        nnf(rhs(desneg(current))));
                            break;
                    }
                    free_bin(desneg(current));
                    free_neg(current);
                    break;
            }
            break;
        case 2:
            result = new_bin(op(current),
                             nnf(lhs(current)),
                             nnf(rhs(current)));
            free_bin(current);
            break;
    }
    return result;
}

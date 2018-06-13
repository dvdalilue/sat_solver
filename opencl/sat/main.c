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

Proposition* impl_free(Proposition *p) {
    Proposition *actual = p;
    Proposition *result = NULL;

    switch (actual->kind) {
        case 0:
            result = p;
            break;
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

Proposition *nnf(Proposition *p) {
    Proposition *actual = p;
    
    switch (actual->kind) {
        case 1:
            break;
        case 2:
            break;
    }
    return p;
}

int main (int argc, const char * argv[]) {
    Proposition *modus_ponen = new_bin(IMPLIE,
                                       new_bin(AND,
                                           new_bin(IMPLIE,
                                               STM('P'),
                                               STM('Q')),
                                           STM('P')),
                                       STM('Q'));

    modus_ponen = impl_free(modus_ponen);
    prop_to_s(modus_ponen);
    fprintf(stdout, "\n");
    destroy_prop(modus_ponen);
    
    return 0;
}

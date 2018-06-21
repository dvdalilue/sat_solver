//
//  main.c
//  sat
//
//  Created by David Lilue on 05/06/2018.
//

#include <stdio.h>
#include <stdlib.h>
#include "anf.h"

int main (int argc, const char * argv[]) {
    Proposition *q = STM('Q');
    Proposition *p = new_bin(OR, STM('P'), q);
    Proposition *test_1 = new_bin(IMPLIE,
                            new_bin(AND,
                                new_bin(IMPLIE,
                                    p,
                                    new_neg(q)),
                                p),
                            new_neg(q));

    // Proposition *modus_ponen = new_bin(IMPLIE,
    //                             new_bin(AND,
    //                                 new_bin(IMPLIE,
    //                                     STM('P'),
    //                                     STM('Q'),
    //                                 STM('P')),
    //                             STM('Q'));

    ANF *aux = anf_it(test_1);

    // prop_to_s(modus_ponen);
    // fprintf(stdout, "\n");
    anf_to_s(aux);
    fprintf(stdout, "\n");

    destroy_prop(test_1);
    destroy_anf(aux);

    return 0;

    // ANF *p = VAR('P');
    // ANF *q = VAR('Q');
    // ANF *aux =
    //    and_anf(
    //        xor_anf(
    //            xor_anf(
    //                xor_anf(
    //                    and_anf(new_const(1), VAR('Q')),
    //                    and_anf(new_const(1), p)),
    //                xor_anf(
    //                    and_anf(VAR('P'), new_const(1)),
    //                    and_anf(new_const(1), q))),
    //            xor_anf(
    //                and_anf(new_const(1), new_const(1)),
    //                and_anf(new_const(1),
    //                    xor_anf(
    //                        and_anf(p, new_const(1)),
    //                        and_anf(new_const(1), q))))),
    //        q);

    // return 0;
}

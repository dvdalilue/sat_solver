//
//  anf_rpn.c
//  sat
//
//  Created by David Lilue on 23/06/2018.
//

#include <stdlib.h>
#include "anf_rpn.h"

ANF_RPN* new_rpn_opn (ANF *v) {
    ANF_RPN *rpn = (ANF_RPN *) malloc(sizeof(ANF_RPN));
    ANF_RPN_UNION *u = (ANF_RPN_UNION *) malloc(sizeof(ANF_RPN_UNION));
    ANF_RPN_OPN *opn = (ANF_RPN_OPN *) malloc(sizeof(ANF_RPN_OPN));
    
    opn->anf = v;
    u->opn = opn;
    
    rpn->kind = OPN;
    rpn->anf = u;
    
    return rpn;
}

ANF_RPN* new_rpn_opr (ANF *(*operation)(ANF*, ANF*)) {
    ANF_RPN *rpn = (ANF_RPN *) malloc(sizeof(ANF_RPN));
    ANF_RPN_UNION *u = (ANF_RPN_UNION *) malloc(sizeof(ANF_RPN_UNION));
    ANF_RPN_OPR *opr = (ANF_RPN_OPR *) malloc(sizeof(ANF_RPN_OPR));
    
    opr->operation = operation;
    u->opr = opr;
    
    rpn->kind = OPR;
    rpn->anf = u;
    
    return rpn;
}

void free_rpn (ANF_RPN *rpn) {
    switch (rpn->kind) {
        case OPN:
            free(rpn->anf->opn);
            break;
        case OPR:
            free(rpn->anf->opr);
            break;
    }
    free(rpn->anf);
    free(rpn);
}
//
//  anf.h
//  sat
//
//  Created by David Lilue on 16/06/2018.
//

#ifndef ANF_RPN

#include "anf_lib.h"

#define opr(rpn) rpn->anf->opr->operation

typedef enum {OPN, OPR} RPN_TYPE;

typedef struct _anf_rpn_opn {
    ANF *anf;
} ANF_RPN_OPN;

typedef struct _anf_rpn_opr {
    ANF *(*operation)(ANF*, ANF*);
} ANF_RPN_OPR;

typedef union _anf_rpn_union {
    ANF_RPN_OPN *opn;
    ANF_RPN_OPR *opr;
} ANF_RPN_UNION;

typedef struct _anf_rpn {
    RPN_TYPE kind;
    ANF_RPN_UNION *anf;
} ANF_RPN;

ANF* anf (Proposition *p);
ANF* anf_it (Proposition *p);
void destroy_anf (ANF *p);

#endif /* anf */

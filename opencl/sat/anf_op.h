//
//  anf_op.h
//  sat
//
//  Created by David Lilue on 08/07/2018.
//

#include "proposition.h"
#include "anf_gpu.h"

ANF* xor_anf_op (ANF *p, ANF *q);
ANF* and_anf_op (ANF *p, ANF *q);
ANF* map_bs_cpu (char *bs, ANF *p);
ANF* anf_op (Proposition *p);

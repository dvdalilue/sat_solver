//
//  anf_op.h
//  sat
//
//  Created by David Lilue on 08/07/2018.
//

#include "proposition.h"
#include "anf_opencl.h"

ANF_OpenCL* xor_anf_op (ANF_OpenCL *p, ANF_OpenCL *q);
ANF_OpenCL* and_anf_op (ANF_OpenCL *p, ANF_OpenCL *q);
ANF_OpenCL* map_anf_bs (char *bs, ANF_OpenCL *p);
ANF_OpenCL* anf_op (Proposition *p);

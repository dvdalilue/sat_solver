//
//  anf_gpu.h
//  sat
//
//  Created by David Lilue on 04/07/2018.
//

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "anf_op_lib.h"

ANF* map_bs_gpu (char *bs, ANF *p);
void releaseAll (void);
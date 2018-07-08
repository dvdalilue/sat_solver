//
//  anf_gpu.h
//  sat
//
//  Created by David Lilue on 04/07/2018.
//

#include <OpenCL/opencl.h>

void and_bs_gpu(dispatch_queue_t queue, char *x, char *y, int length, char *result);

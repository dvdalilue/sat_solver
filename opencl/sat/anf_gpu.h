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

// void and_bs_gpu(dispatch_queue_t queue, char *x, char *y, int length, char *result);

int first_test(void);
/***** GCL Generated File *********************/
/* Automatically generated file, do not edit! */
/**********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <dispatch/dispatch.h>
#include <OpenCL/opencl.h>
#include <OpenCL/gcl_priv.h>
#include "kernel.cl.h"

static void initBlocks(void);

// Initialize static data structures
static block_kernel_pair pair_map[1] = {
      { NULL, NULL }
};

static block_kernel_map bmap = { 0, 1, initBlocks, pair_map };

// Block function
void (^and_bs_gpu_kernel)(const cl_ndrange *ndrange, cl_char* bs_x, cl_char* bs_y, cl_char* bs_out) =
^(const cl_ndrange *ndrange, cl_char* bs_x, cl_char* bs_y, cl_char* bs_out) {
  int err = 0;
  cl_kernel k = bmap.map[0].kernel;
  if (!k) {
    initBlocks();
    k = bmap.map[0].kernel;
  }
  if (!k)
    gcl_log_fatal("kernel and_bs_gpu does not exist for device");
  kargs_struct kargs;
  gclCreateArgsAPPLE(k, &kargs);
  err |= gclSetKernelArgMemAPPLE(k, 0, bs_x, &kargs);
  err |= gclSetKernelArgMemAPPLE(k, 1, bs_y, &kargs);
  err |= gclSetKernelArgMemAPPLE(k, 2, bs_out, &kargs);
  gcl_log_cl_fatal(err, "setting argument for and_bs_gpu failed");
  err = gclExecKernelAPPLE(k, ndrange, &kargs);
  gcl_log_cl_fatal(err, "Executing and_bs_gpu failed");
  gclDeleteArgsAPPLE(k, &kargs);
};

// Initialization functions
static void initBlocks(void) {
  const char* build_opts = "";
  static dispatch_once_t once;
  dispatch_once(&once,
    ^{ int err = gclBuildProgramBinaryAPPLE("OpenCL/kernel.cl", "", &bmap, build_opts);
       if (!err) {
          assert(bmap.map[0].block_ptr == and_bs_gpu_kernel && "mismatch block");
          bmap.map[0].kernel = clCreateKernel(bmap.program, "and_bs_gpu", &err);
       }
     });
}

__attribute__((constructor))
static void RegisterMap(void) {
  gclRegisterBlockKernelMap(&bmap);
  bmap.map[0].block_ptr = and_bs_gpu_kernel;
}


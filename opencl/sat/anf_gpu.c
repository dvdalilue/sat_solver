//
//  anf_gpu.c
//  sat
//
//  Created by David Lilue on 04/07/2018.
//

#include <stdio.h>

#include "kernel.cl.h"
#include "anf_gpu.h"

void and_bs_gpu(dispatch_queue_t queue, char *x, char *y, int length, char *result) {
//    cl_device_id device_id;
//
//    if (clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL)) {
//        fprintf(stdout, "Something went wrong getting device ID");
//        exit(1);
//    }
//
//    dispatch_queue_t queue = gcl_create_dispatch_queue(CL_DEVICE_TYPE_GPU, NULL);
//
//    if (queue == NULL) {
//        fprintf(stdout, "No GPU queue where to dispatch");
//        exit(1);
//    }

    void *mem_in_x  = gcl_malloc(sizeof(char) * length, x,
                               CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    void *mem_in_y  = gcl_malloc(sizeof(char) * length, y,
                               CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    void *mem_out = gcl_malloc(sizeof(char) * length, NULL,
                               CL_MEM_WRITE_ONLY);

    dispatch_sync(queue, ^{
        cl_ndrange range = {
            1,          // The number of dimensions to use.
            
            {0, 0, 0},  // The offset in each dimension.  To specify
                        // that all the data is processed, this is 0
                        // in the test case.
            
            {length, 0, 0}, // The global range—this is how many items
                            // IN TOTAL in each dimension you want to
                            // process.
            
            NULL        // The local size of each workgroup.  This
                        // determines the number of work items per
                        // workgroup.  It indirectly affects the
                        // number of workgroups, since the global
                        // size / local size yields the number of
                        // workgroups.  In this test case, there are
                        // NUM_VALUE / wgs workgroups.
        };

        and_bs_gpu_kernel(&range, (cl_char *) mem_in_x, (cl_char *) mem_in_y, (cl_char *) mem_out);
        
        gcl_memcpy(result, mem_out, sizeof(char) * length);
    });
    
    gcl_free(mem_in_x);
    gcl_free(mem_in_y);
    gcl_free(mem_out);

//    dispatch_release(queue);
}

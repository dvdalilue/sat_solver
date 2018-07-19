//
//  kernel.cl
//  sat_solver
//
//  Created by David Lilue on 05/06/2018.
//

#define BS_SIZE 35 // search at anf_op_lib.h

// Kernel block.
__kernel void map_anf_gpu(__global const char *bit_string, __global char *polynomial) {
    int i = get_global_id(0) * BS_SIZE;
    int k = 0;

    while (k < BS_SIZE) {
        polynomial[i + k] = bit_string[k] | polynomial[i + k];
        k++;
    }
}

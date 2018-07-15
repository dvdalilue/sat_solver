//
//  kernel.cl
//  sat_solver
//
//  Created by David Lilue on 05/06/2018.
//

#define BS_SIZE 128

// Kernel block.
__kernel void vector_add(__global const int *A, __global const int *B, __global int *C) {
 
    // Get the index of the current element to be processed
    int i = get_global_id(0);

    // Do the operation
    C[i] = A[i] + B[i];
}

__kernel void vector_minus(__global const int *A, __global const int *B, __global int *C) {
 
    // Get the index of the current element to be processed
    int i = get_global_id(0);

    // Do the operation
    C[i] = A[i] - B[i];
}

__kernel void map_anf_gpu(__global const char *bit_string, __global const char *polynomial, __global char *result) {
    int i = get_global_id(0) * BS_SIZE;
    int k = 0;

    while (k < BS_SIZE) {
        result[k] = bit_string[k] | polynomial[i + k];
        k++;
    }
}

//
//  anf_gpu.c
//  sat
//
//  Created by David Lilue on 04/07/2018.
//

#include <stdio.h>
#include <stdlib.h>

#include "anf_gpu.h"

#define MAX_SOURCE_SIZE (0x1000000)

cl_command_queue command_queue = NULL;
cl_device_id device_id = NULL;
cl_context context = NULL;
cl_program program = NULL;

static void print_device_info(cl_device_id device) {
    char name[128];
    char vendor[128];
    
    clGetDeviceInfo(device, CL_DEVICE_NAME, 128, name, NULL);
    clGetDeviceInfo(device, CL_DEVICE_VENDOR, 128, vendor, NULL);

    fprintf(stdout, "%s : %s\n", name, vendor);
}

cl_device_id get_device_id (cl_int *ret) {
    if (device_id != NULL) return device_id;

    // Get platform and device information
    cl_platform_id platform_id = NULL;

    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;

    (*ret) = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    (*ret) = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, 
            &device_id, &ret_num_devices);

    return device_id;

}

cl_context get_context (cl_device_id device_id, cl_int *ret) {
    if (context == NULL) {
        context = clCreateContext(NULL, 1, &device_id, NULL, NULL, ret);
    }

    return context;
}

cl_command_queue get_command_queue (cl_context context, cl_device_id device_id, cl_int *ret) {
    if (command_queue == NULL) {
        command_queue = clCreateCommandQueue(context, device_id, 0, ret);
    }

    return command_queue;
}

cl_program load_program (const char *file_path, cl_context context, cl_int *ret) {
    if (program != NULL) return program;

    // Load the kernel source code into the array source_str
    FILE *fp = fopen(file_path, "r");

    char *source_str;
    size_t source_size;

    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }

    source_str = (char *) malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);

    fclose(fp);

    // Create a program from the kernel source
    program = clCreateProgramWithSource(context, 1, 
            (const char **) &source_str, (const size_t *) &source_size, ret);

    // Build the program
    (*ret) = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    switch ((*ret)) {
        case CL_INVALID_PROGRAM:
            printf("CL_INVALID_PROGRAM: %s\n", file_path);
            break;
        case CL_INVALID_VALUE:
            printf("CL_INVALID_VALUE: %s\n", file_path);
            break;
        case CL_INVALID_DEVICE:
            printf("CL_INVALID_DEVICE: %s\n", file_path);
            break;
        case CL_INVALID_BINARY:
            printf("CL_INVALID_BINARY: %s\n", file_path);
            break;
        case CL_INVALID_BUILD_OPTIONS:
            printf("CL_INVALID_BUILD_OPTIONS: %s\n", file_path);
            break;
        case CL_INVALID_OPERATION:
            printf("CL_INVALID_OPERATION: %s\n", file_path);
            break;
        case CL_COMPILER_NOT_AVAILABLE:
            printf("CL_COMPILER_NOT_AVAILABLE: %s\n", file_path);
            break;
        case CL_BUILD_PROGRAM_FAILURE:
            printf("CL_BUILD_PROGRAM_FAILURE: %s\n", file_path);
            break;
        case CL_OUT_OF_HOST_MEMORY:
            printf("CL_OUT_OF_HOST_MEMORY: %s\n", file_path);
            break;
    }

    return program;
}

int first_test(void) {
    // Create the two input vectors
    int i;
    const int LIST_SIZE = 3122;
    int *A = (int *) malloc(sizeof(int)*LIST_SIZE);
    int *B = (int *) malloc(sizeof(int)*LIST_SIZE);

    for(i = 0; i < LIST_SIZE; i++) {
        A[i] = i;
        B[i] = LIST_SIZE - i;
    }

    // Get platform and device information
    cl_int ret;
    device_id = get_device_id(&ret);

    // Create an OpenCL context
    context = get_context(device_id, &ret);

    // Create a command queue
    command_queue = get_command_queue(context, device_id, &ret);

    // Create memory buffers on the device for each vector 
    cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, 
            LIST_SIZE * sizeof(int), NULL, &ret);
    cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
            LIST_SIZE * sizeof(int), NULL, &ret);
    cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 
            LIST_SIZE * sizeof(int), NULL, &ret);

    // Copy the lists A and B to their respective memory buffers
    ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0,
            LIST_SIZE * sizeof(int), A, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0,
            LIST_SIZE * sizeof(int), B, 0, NULL, NULL);

    // Create a program from the kernel source
    program = load_program("kernel.cl", context, &ret);

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "vector_add", &ret);

    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);

    // Execute the OpenCL kernel on the list
    size_t global_item_size = LIST_SIZE; // Process the entire lists
    size_t local_item_size = 1000; // Divide work items into groups of 64
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, 
            &global_item_size, NULL, 0, NULL, NULL);

    // Read the memory buffer C on the device to the local variable C
    int *C = (int*) malloc(sizeof(int)*LIST_SIZE);
    ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0, 
            LIST_SIZE * sizeof(int), C, 0, NULL, NULL);

    // Display the result to the screen
    // for(i = 0; i < LIST_SIZE; i++)
    //     printf("%d + %d = %d\n", A[i], B[i], C[i]);

    // Clean up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(a_mem_obj);
    ret = clReleaseMemObject(b_mem_obj);
    ret = clReleaseMemObject(c_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(A);
    free(B);
    free(C);
    return 0;
}

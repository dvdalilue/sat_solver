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

/*
 *    Global OpenCL Variables
 */

// Device ID
cl_device_id device_id = NULL;
// Context
cl_context context = NULL;
// Command Queue
cl_command_queue command_queue = NULL;
// Program
cl_program program = NULL;

/*
 * Prints the device information 'name : vendor'
 */
static void print_device_info(cl_device_id device) {
    char name[128];
    char vendor[128];

    clGetDeviceInfo(device, CL_DEVICE_NAME, 128, name, NULL);
    clGetDeviceInfo(device, CL_DEVICE_VENDOR, 128, vendor, NULL);

    fprintf(stdout, "%s : %s\n", name, vendor);
}

/*
 * Returns the value of 'device_id', if it is not NULL.
 * Otherwise, gets the GPU device ID, it is assigned to
 * 'device_id' and returned.
 */
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

/*
 * Returns the value of 'context', if it is not NULL.
 * Otherwise, gets the context given a device ID, it is
 * assigned to 'context' and returned.
 */
cl_context get_context (cl_device_id device_id, cl_int *ret) {
    if (context == NULL) {
        context = clCreateContext(NULL, 1, &device_id, NULL, NULL, ret);
    }

    return context;
}

/*
 * Returns the value of 'command_queue', if it is not NULL.
 * Otherwise, gets the command queue given a context and device
 * ID, it is assigned to 'command_queue' and returned.
 */
cl_command_queue get_command_queue (cl_context context, cl_device_id device_id, cl_int *ret) {
    if (command_queue == NULL) {
        command_queue = clCreateCommandQueue(context, device_id, 0, ret);
    }

    return command_queue;
}

/*
 * Returns the value of 'program', if it is not NULL.
 * Otherwise, load and build a OpenCL kernel program given a
 * file path and context, it is assigned to 'program', checks
 * if an error occurred and returns the program built.
 */
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

/*
 * Takes the concatenation of n bit strings where
 * n = 'monomials', and creates polynomial (ANF). It is assumed
 * that the size of 'bstring' is evenly diveded by BS_SIZE.
 */
ANF* bstringToANF (char *bstring, int monomials) {
    ANF *result = empty_anf();

    for (int i = 0; i < monomials; i++) {
        add_xor_comp_envious(result, (bstring + i * BS_SIZE));
    }

    free(bstring);

    return result;
}

/*
 * GPU implementation of 'map_anf_bs'. See anf_op.c file.
 */
ANF* map_bs_gpu (char *bs, ANF *p) {
    cl_int ret;
    // Get device information
    device_id = get_device_id(&ret);
    // Create an OpenCL context
    context = get_context(device_id, &ret);
    // Create a command queue
    command_queue = get_command_queue(context, device_id, &ret);

    // Create memory buffers on the device for each vector
    cl_mem gpu_bs = clCreateBuffer(context, CL_MEM_READ_ONLY,
            BS_SIZE * sizeof(char), NULL, &ret);
    cl_mem gpu_poly = clCreateBuffer(context, CL_MEM_READ_WRITE,
            p->monomials * sizeof(char) * BS_SIZE, NULL, &ret);

    // Copy order and bit string to their respective memory buffers
    ret = clEnqueueWriteBuffer(command_queue, gpu_bs, CL_TRUE, 0,
            BS_SIZE * sizeof(char), bs, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, gpu_poly, CL_TRUE, 0,
            p->monomials * sizeof(char) * BS_SIZE, p->bstring, 0, NULL, NULL);

    // Create a program from the kernel source
    program = load_program("kernel.cl", context, &ret);

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "map_anf_gpu", &ret);

    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *) &gpu_bs);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *) &gpu_poly);

    // Execute the OpenCL kernel on the list
    size_t global_item_size = p->monomials; // Process the entire polynomial
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
            &global_item_size, NULL, 0, NULL, NULL);

    // Read the memory buffer gpu_poly on the device to the local variable poly
    char *anf_p = (char *) malloc(sizeof(char) * p->monomials * BS_SIZE);

    ret = clEnqueueReadBuffer(command_queue, gpu_poly, CL_TRUE, 0,
            p->monomials * sizeof(char) * BS_SIZE, anf_p, 0, NULL, NULL);

    // Clean up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseMemObject(gpu_bs);
    ret = clReleaseMemObject(gpu_poly);

    return bstringToANF(anf_p, p->monomials);
}

/*
 * Release the OpenCL context, queue and program.
 */
void releaseAll (void) {
    cl_int ret;

    ret = clReleaseContext(context);
    ret = clReleaseProgram(program);
    ret = clReleaseCommandQueue(command_queue);
}

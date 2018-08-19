#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include "pgm.h"
#include <OpenCL/OpenCL.h>

#define MAX_SOURCE_SIZE    16384

int main(int argc, char **argv)
{
    PGMData original, final;

    int iterations = 100;

    if(argc >= 2){
        iterations = atoi(argv[1]);
    }

    readPGM((argc >= 3) ? argv[2] : "image.pgm", &original);
    int width = original.width,
    height = original.height,
    imageSize = width * height;

    // Branje datoteke
    FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen("kernel.cl", "r");
    if (!fp)
    {
        fprintf(stderr, ":-(#\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    source_str[source_size] = '\0';
    fclose(fp);

    // Podatki o platformi
    cl_platform_id    platform_id[10];
    cl_uint            ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, platform_id, &ret_num_platforms);

    // Podatki o napravi
    cl_device_id    device_id[1];
    cl_uint            ret_num_devices;
    ret = clGetDeviceIDs(platform_id[0], CL_DEVICE_TYPE_GPU, 1,
                         device_id, &ret_num_devices);

    // Kontekst
    cl_context context = clCreateContext(NULL, 1, &device_id[0], NULL, NULL, &ret);

    // Ukazna vrsta
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id[0], 0, &ret);

    double start = omp_get_wtime();
    // Alokacija pomnilnika na napravi
    cl_mem final_mem_obj = clCreateBuffer(context, 0, imageSize * sizeof(int), NULL, &ret); // HOST_READ_ONLY
    cl_mem original_mem_obj = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, imageSize * sizeof(int), original.image, &ret); // HOST_WRITE_ONLY
    cl_mem sobel_mem_obj = clCreateBuffer(context, 0, imageSize * sizeof(int), NULL, &ret);
    cl_mem seam_mem_obj = clCreateBuffer(context, CL_MEM_HOST_NO_ACCESS, height * sizeof(int), NULL, &ret);

    // Priprava programa
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, NULL, &ret);

    // Prevajanje
    ret = clBuildProgram(program, 1, &device_id[0], NULL, NULL, NULL);

    // Log
    size_t build_log_len;
    char *build_log;
    ret = clGetProgramBuildInfo(program, device_id[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &build_log_len);
    build_log = (char *)malloc(sizeof(char)*(build_log_len + 1));
    ret = clGetProgramBuildInfo(program, device_id[0], CL_PROGRAM_BUILD_LOG, build_log_len, build_log, NULL);
    printf("%s\n", build_log);
    free(build_log);


    // scepec: priprava objekta
    cl_kernel sobelKernel = clCreateKernel(program, "sobelPixel", &ret);
    cl_kernel cumulativesKernel = clCreateKernel(program, "calculateCumulatives", &ret);
    cl_kernel removeSeamKernel = clCreateKernel(program, "removeSeam", &ret);

    // Delitev dela sobel
    size_t local_size_sobel[] = {16, 16};

    //Delitev dela cumulatives
    size_t local_size_cumulatives[] = {256};
    size_t global_size_cumulatives[] = {256};

    for (int i = 0; i < iterations; i++) {
        // SOBEL START
        // Delitev dela sobel
        size_t group_count_sobel[] = {(width - 1) / 16 + 1, (height - 1) / 16 + 1};
        size_t global_size_sobel[] = {group_count_sobel[0] * 16, group_count_sobel[1] * 16};

        // Sobel scepec argumenti
        ret = clSetKernelArg(sobelKernel, 0, sizeof(cl_mem), (i % 2 == 0 ) ? (void *)&original_mem_obj : (void *)&final_mem_obj);
        ret |= clSetKernelArg(sobelKernel, 1, sizeof(cl_mem), (void *)&sobel_mem_obj);
        ret |= clSetKernelArg(sobelKernel, 2, sizeof(cl_int), (void *)&width);
        ret |= clSetKernelArg(sobelKernel, 3, sizeof(cl_int), (void *)&height);

        // Zagon scepca sobel
        ret = clEnqueueNDRangeKernel(command_queue, sobelKernel, 2, NULL, global_size_sobel, local_size_sobel, 0, NULL, NULL);
        // SOBEL END

        // KUMULATIVE + SEAM SEARCH START
        // Kumulative scepec argumenti
        ret = clSetKernelArg(cumulativesKernel, 0, sizeof(cl_mem), (void *)&sobel_mem_obj);
        ret |= clSetKernelArg(cumulativesKernel, 1, sizeof(cl_mem), (void *)&seam_mem_obj);
        ret |= clSetKernelArg(cumulativesKernel, 2, sizeof(cl_int), (void *)&width);
        ret |= clSetKernelArg(cumulativesKernel, 3, sizeof(cl_int), (void *)&height);

        // Zagon scepca cumulatives
        ret = clEnqueueNDRangeKernel(command_queue, cumulativesKernel, 1, NULL, global_size_cumulatives, local_size_cumulatives, 0, NULL, NULL);
        // END KUMULATIVE + SEAM SEARCH

        // START REMOVE SEAM
        // Delitev dela remove seam
        width--;
        size_t group_count_remove[] = {(width - 1) / 16 + 1, (height - 1) / 16 + 1};
        size_t global_size_remove[] = {group_count_remove[0] * 16, group_count_remove[1] * 16};

        // Remove seam scepec argumenti
        ret = clSetKernelArg(removeSeamKernel, 0, sizeof(cl_mem), (i % 2 == 0 ) ? (void *)&original_mem_obj : (void *)&final_mem_obj);
        ret |= clSetKernelArg(removeSeamKernel, 1, sizeof(cl_mem),(i % 2 == 0 ) ? (void *)&final_mem_obj : (void *)&original_mem_obj);
        ret |= clSetKernelArg(removeSeamKernel, 2, sizeof(cl_mem), (void *)&seam_mem_obj);
        ret |= clSetKernelArg(removeSeamKernel, 3, sizeof(cl_int), (void *)&width);
        ret |= clSetKernelArg(removeSeamKernel, 4, sizeof(cl_int), (void *)&height);

        // Zagon scepca removeSeams
        ret = clEnqueueNDRangeKernel(command_queue, removeSeamKernel, 2, NULL, global_size_remove, local_size_sobel, 0, NULL, NULL);
        // END REMOVE SEAM
    }

    final.width = width;
    final.height = height;
    final.max_gray = original.max_gray;
    final.image = (int *)malloc(final.height * final.width * sizeof(int));

    ret = clEnqueueReadBuffer(command_queue, (iterations % 2 == 0) ? original_mem_obj : final_mem_obj, CL_TRUE, 0, final.height * final.width * sizeof(int), final.image, 0, NULL, NULL);

    // clean up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(sobelKernel);
    ret = clReleaseKernel(cumulativesKernel);
    ret = clReleaseKernel(removeSeamKernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(original_mem_obj);
    ret = clReleaseMemObject(final_mem_obj);
    ret = clReleaseMemObject(sobel_mem_obj);
    ret = clReleaseMemObject(seam_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);

    printf("Time GPU: %.20g\n", omp_get_wtime() - start);

    writePGM("seamGPU.pgm", &final);

    return 0;
}

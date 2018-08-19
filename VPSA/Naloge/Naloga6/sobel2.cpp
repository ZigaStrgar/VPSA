// "s"cepec preberemo iz datoteke
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include "pgm.h"
#include <OpenCL/OpenCL.h>

#define WORKGROUP_SIZE    (256)
#define MAX_SOURCE_SIZE    16384
#define REPETITIONS 100

void sobelGPU(PGMData *in, PGMData *out)
{
    int width = in->width,
    height = in->height,
    imageSize = width * height;
    
    // Branje datoteke
    FILE *fp;
    char *source_str;
    size_t source_size;
    
    fp = fopen("sobel2.cl", "r");
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
    cl_platform_id    platform_id[1];
    cl_uint            ret_num_platforms;
    char            *buf;
    size_t            buf_len;
    cl_int ret = clGetPlatformIDs(1, platform_id, &ret_num_platforms);
    // max. "stevilo platform, kazalec na platforme, dejansko "stevilo platform
    
    // Podatki o napravi
    cl_device_id    device_id[1];
    cl_uint            ret_num_devices;
    // Delali bomo s platform_id[0] na GPU
    ret = clGetDeviceIDs(platform_id[0], CL_DEVICE_TYPE_GPU, 1,
                         device_id, &ret_num_devices);
    // izbrana platforma, tip naprave, koliko naprav nas zanima
    // kazalec na naprave, dejansko "stevilo naprav
    
    // Kontekst
    cl_context context = clCreateContext(NULL, 1, &device_id[0], NULL, NULL, &ret);
    // kontekst: vklju"cene platforme - NULL je privzeta, "stevilo naprav,
    // kazalci na naprave, kazalec na call-back funkcijo v primeru napake
    // dodatni parametri funkcije, "stevilka napake
    
    // Ukazna vrsta
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id[0], 0, &ret);
    // kontekst, naprava, INORDER/OUTOFORDER, napake
    
    // Delitev dela
    size_t local_item_size = WORKGROUP_SIZE;
    size_t num_groups = ((imageSize - 1) / local_item_size + 1);
    size_t global_item_size = num_groups*local_item_size;
    
    // Alokacija pomnilnika na napravi
    cl_mem input_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, imageSize * sizeof(int), in->image, &ret);
    cl_mem output_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, imageSize * sizeof(int), NULL, &ret);
    // kontekst, nacin, koliko, lokacija na hostu, napaka
    
    // Priprava programa
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, NULL, &ret);
    // kontekst, stevilo kazalcev na kodo, kazalci na kodo,
    // stringi so NULL terminated, napaka
    
    // Prevajanje
    ret = clBuildProgram(program, 1, &device_id[0], NULL, NULL, NULL);
    // program, stevilo naprav, lista naprav, opcije pri prevajanju,
    // kazalec na funkcijo, uporabni"ski argumenti
    
    // Log
    size_t build_log_len;
    char *build_log;
    ret = clGetProgramBuildInfo(program, device_id[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &build_log_len);
    // program, naprava, tip izpisa,
    // maksimalna dolzina niza, kazalec na niz, dejanska dolzina niza
    build_log = (char *)malloc(sizeof(char)*(build_log_len + 1));
    ret = clGetProgramBuildInfo(program, device_id[0], CL_PROGRAM_BUILD_LOG, build_log_len, build_log, NULL);
    printf("%s\n", build_log);
    free(build_log);
    
    // scepec: priprava objekta
    cl_kernel kernel = clCreateKernel(program, "pixel", &ret);
    // program, ime scepca, napaka
    
    // scepec argumenti
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&input_mem_obj);
    ret |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&output_mem_obj);
    ret |= clSetKernelArg(kernel, 2, sizeof(cl_int), (void *)&width);
    ret |= clSetKernelArg(kernel, 3, sizeof(cl_int), (void *)&height);
    // scepec, stevilka argumenta, velikost podatkov, kazalec na podatke
    
    // scepec: zagon
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
    // vrsta, scepec, dimenzionalnost, mora biti NULL,
    // kazalec na stevilo vseh niti, kazalec na lokalno stevilo niti,
    // dogodki, ki se morajo zgoditi pred klicem
    
    // Kopiranje rezultatov
    ret = clEnqueueReadBuffer(command_queue, output_mem_obj, CL_TRUE, 0, imageSize * sizeof(int), out->image, 0, NULL, NULL);
    // branje v pomnilnik iz naparave, 0 = offset
    // zadnji trije - dogodki, ki se morajo zgoditi prej
    
    // clean shit up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(input_mem_obj);
    ret = clReleaseMemObject(output_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
}

int main(int argc, char *argv[])
{
    PGMData slikaInput, slikaGPU;
    
    readPGM("desktop.pgm", &slikaInput);
    slikaGPU.height = slikaInput.height;
    slikaGPU.width = slikaInput.width;
    slikaGPU.max_gray = slikaInput.max_gray;
    
    slikaGPU.image = (int *)malloc(slikaGPU.height * slikaGPU.width * sizeof(int));
    
    double start = omp_get_wtime();
    
    sobelGPU(&slikaInput, &slikaGPU);
    
    printf("Time GPU: %.20g\n", omp_get_wtime() - start);
    
    writePGM("sobelGPU.pgm", &slikaGPU);
    
    return 0;
}


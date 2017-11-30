#include <stdio.h>
#include "common/book.h"

#define N 10

__global__ void vectorAddGPU(float *a, float *b, float *c){
    int tid = threadIdx.x;
    
    if(tid < N){
        c[tid] = a[tid] + b[tid];
    }
}

void vectorAddSerial(float *a, float *b, float *c){
    int tid = 0;
    
    while(tid < N){
        c[tid] = a[tid] + b[tid];
        tid++;
    }
}

int main(void){
    float *h_a, *h_b, *h_c;
    float *d_a, *d_b, *d_c;
    
    h_a = (float*) malloc(N*sizeof(float));
    h_b = (float*) malloc(N*sizeof(float));
    h_c = (float*) malloc(N*sizeof(float));
    
    cudaMalloc( (void**) &d_a, N*sizeof(float));
    cudaMalloc( (void**) &d_b, N*sizeof(float));
    cudaMalloc( (void**) &d_c, N*sizeof(float));
    
    for(int i = 0; i < N; i++){
        h_a[i] = (float)-i;
        h_b[i] = (float)(i*i);
    }
    
    cudaMemcpy(d_a, h_a, N * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, h_b, N * sizeof(float), cudaMemcpyHostToDevice);
    
    //vectorAddSerial(a,b,c);
    vectorAddGPU<<<1, N>>>(d_a, d_b, d_c);
    
    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess) {
        printf("%s \n", cudaGetErrorString(err));
    }
    
    cudaThreadSynchronize();
    
    cudaMemcpy(h_c, d_c, N*sizeof(float), cudaMemcpyDeviceToHost);
    
    for (int i = 0; i < N; i++) {
        printf("%f + %f = %f \n", h_a[i], h_b[i], h_c[i]);
    }
    
    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);
    
    free(h_a);
    free(h_b);
    free(h_c);
    
    return 0;
}


#include "common/book.h"

int main(void){
    cudaDeviceProp devprop;
    int count;
    
    cudaGetDeviceCount(&count);
    
    for (int i = 0; i < count; i++) {
        cudaGetDeviceProperties(&devprop, i);
        
        printf("  ------ Info o CUDA napravi %d ------  ", i);
        printf("Ime naprave: %s \n", devprop.name);
        printf("Hitrost ure: %d \n", devprop.clockRate);
        printf("Kolicina globalnega pomnilnika: %d \n", devprop.totalGlobalMem);
        printf("Stevilo multiprocesorjev SM: %d\n", devprop.multiProcessorCount);
        printf("Kolicina skupnega pomnilnika v SM: %d \n", devprop.sharedMemPerMultiprocessor);
        printf("Stevilo registrov v SM: %d \n", devprop.regsPerMultiprocessor);
        
        printf("Max stevilo niti v bloku: %d \n", devprop.maxThreadsPerBlock);
        printf("Stevilo registrov na blok: %d \n", devprop.regsPerBlock);
        printf("Kolicina skupnega pomnilnika za blok: %d \n", devprop.sharedMemPerBlock);
        printf("Max stevilo niti na SM: %d \n", devprop.maxThreadsPerMultiProcessor);
        printf("Velikost snopa (warp): %d\n", devprop.warpSize);
    }
}

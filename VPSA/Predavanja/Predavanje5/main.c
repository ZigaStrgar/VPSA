#include <stdio.h>
#include <omp.h>

#define NTHREADS 4
#define N 100000000

double vectorA[N];
double vectorB[N];
double scalarProduct = 0;
double resultVector[NTHREADS];

int main(int argc, const char * argv[]) {
    omp_set_num_threads(NTHREADS);
    
    for (int i = 0; i < NTHREADS; i++) {
        resultVector[i] = 0;
    }
    
    for (int i = 0; i < N; i++) {
        vectorA[i] = 1;
        vectorB[i] = 1;
    }
    
    #pragma omp parallel for reduction(+:scalarProduct)
    for (int i = 0; i < N; i++) {
        //resultVector[omp_get_thread_num()] += vectorA[i] * vectorB[i];
        scalarProduct += vectorA[i] * vectorB[i];
    }
    
    /*for (int i = 0; i < NTHREADS; i++) {
        scalarProduct += resultVector[i];
    }*/
    
    printf("Skalarni produkt: %f\n", scalarProduct);
    
    /*#pragma omp parallel
    {
    printf("Pozdravljen svet, sem nit številka %d\n", omp_get_thread_num());
    }
     */
    
    
}

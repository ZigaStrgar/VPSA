#include <stdio.h>
#include <omp.h>
#include <time.h>

#define MAX 100000

int divisorsSum(int n);

int main(int argc, const char * argv[]) {
    int result = 0;
    
    double start = omp_get_wtime();
#pragma omp parallel for reduction(+:result) schedule(dynamic)
    for (int i = 1; i <= MAX; i++) {
        int b = divisorsSum(i);
        if((i < b) && (b <= MAX) && (divisorsSum(b) == i)){
            result += b + i;
        }
    }
    printf("Parallel Time: %f\n", omp_get_wtime() - start);
    
    result = 0;
    start = omp_get_wtime();
    for (int i = 1; i <= MAX; i++) {
        int b = divisorsSum(i);
        if((i < b) && (b <= MAX) && (divisorsSum(b) == i)){
            result += b + i;
        }
    }
    printf("Serial Time: %f\n", omp_get_wtime() - start);
    
    printf("Skupni sum: %d\n", result);
}

int divisorsSum(int n){
    int r = 0;
    
    for (int i = 1; i <= n/2; i++) {
        if(n%i == 0){
            r += i;
        }
    }
    
    return r;
}

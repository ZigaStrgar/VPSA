//
//  main.c
//  Naloga5
//
//  Created by Ziga Strgar on 16/11/2017.
//  Copyright © 2017 Ziga Strgar. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "omp.h"

#define N 100000000
#define MIN 100
#define MAX 15000
#define NUMTHREADS 8
#define STRIPLENGTH N/NUMTHREADS

double k[N], kp[N];

int main(int argc, const char * argv[]) {
    srand(time(NULL));
    
    long long *x = malloc(N*sizeof(long long)), *y = malloc(N*sizeof(long long));
    double max[NUMTHREADS];
    long long r;
    
    for (size_t i = 0; i < N; i++) {
        //r = rand() % (MAX - MIN) + MIN * i;
        x[i] = i + 1;
        y[i] = (i+1) * (i+1); // r
    }
    
    k[0] = y[0] / x[0];
    kp[0] = y[0] / x[0];
    double seenHeight = y[0];
    long long numberOfSeen = 1;
    double ratio;
    
    // SERIAL
    double starts = omp_get_wtime();
    for (size_t i = 1; i < N; i++) {
        ratio = y[i] / x[i];
        k[i] = (k[i-1] < ratio) ? ratio : k[i-1];
        if(k[i-1] < k[i]){
            seenHeight += y[i]-k[i-1]*x[i];
            numberOfSeen++;
        }
    }
    double ends = omp_get_wtime();
    // END SERIAL
    
    double seenHeightP = y[0];
    long long numberOfSeenP = 1;
    
    // PARALLEL
    omp_set_num_threads(NUMTHREADS);
    double startp = omp_get_wtime();
#pragma omp parallel
    {
        int id = omp_get_thread_num();
        size_t start = id * STRIPLENGTH;
        double localMax = 0;
        double ratiop;
        
        for (int i = start; i < start + STRIPLENGTH; i++) {
            ratiop = y[i] / x[i];
            kp[i] = ratiop;
            if(ratiop > localMax){
                localMax = kp[i];
            } else {
                kp[i] = localMax;
            }
        }
        
        max[id] = localMax;
    }
    
    for (int i = 1; i < NUMTHREADS; i++) {
        if(max[i-1] > max[i]){
            max[i] = max[i-1];
        }
    }
    
#pragma omp parallel reduction(+:seenHeightP,numberOfSeenP)
    {
        int id = omp_get_thread_num();
        size_t start = id * STRIPLENGTH;
        double top = (id == 0) ? kp[0] : max[id - 1];
        
        for (int i = (id == 0) ? 1 : start; i < start + STRIPLENGTH; i++) {
            if(kp[i] > top){
                seenHeightP += y[i] - top * x[i];
                numberOfSeenP += 1;
                top = kp[i];
            }
        }
    }
    double endp = omp_get_wtime();
    // END PARALLEL
    
    printf("Seen height: %f, number: %d\n", seenHeight, numberOfSeen);
    printf("Seen height: %f, number: %d\n", seenHeightP, numberOfSeenP);
    printf("Pohitritev: %f, serijsko: %f, paralelno: %f", (ends - starts) / (endp - startp), ends - starts, endp - startp);
    
    free(x);
    free(y);
    
    return 0;
}

#include <stdio.h>
#include "pthread.h"
#include "pthread_barrier.h"

// uporabi štoparico ne clock_t

#define NTHREADS 4
#define VECTORLENGTH 8
#define STRIPLENGTH VECTORLENGTH/NTHREADS

double vecA[VECTORLENGTH];
double vecB[VECTORLENGTH];
pthread_t threads[NTHREADS];
pthread_barrier_t barrier;

struct arguments{
    int id;
    int iteration;
    double result;
    double *pa;
    double *pb;
};

struct arguments arguments[2];

void *scalarProduct(void *arg);

int main(int argc, const char * argv[]) {
    double result = 0;
    
    for (int i = 0; i < VECTORLENGTH; i++) {
        vecA[i] = 1;
        vecB[i] = 1;
    }
    
    pthread_barrier_init(&barrier, NULL, NTHREADS);
    
    for (int i = 0; i < NTHREADS; i++) {
        arguments[i].id = i;
        arguments[i].iteration = 1;
        arguments[i].pa = vecA + i * STRIPLENGTH;
        arguments[i].pb = vecB + i * STRIPLENGTH;
        pthread_create(&threads[i], NULL, *scalarProduct, (void *)&arguments[i]);
    }
    
    for (int i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
        //result += arguments[i].result;
    }
}

void *scalarProduct(void *arg){
    struct arguments *parg = arg;
    
    double *pStripA = parg->pa, *pStripB = parg->pa, result = 0.0;
    
    for (int i=0; i < STRIPLENGTH; i++) {
        result += pStripA[i] * pStripB[i];
    }
    
    parg->pa[0] = result;
    pthread_barrier_wait(&barrier);
    
    if(parg->id > NTHREADS / parg->iteration)
        return NULL;
    
    parg->iteration++;
    
    
    
    return NULL;
}

#include <stdio.h>
#include <pthread.h>
#include "pthread_barrier.h"

#include <errno.h>

#ifdef __APPLE__

#define __unused __attribute__((unused))

int
pthread_barrierattr_init(pthread_barrierattr_t *attr __unused)
{
    return 0;
}

int
pthread_barrierattr_destroy(pthread_barrierattr_t *attr __unused)
{
    return 0;
}

int
pthread_barrierattr_getpshared(const pthread_barrierattr_t *restrict attr __unused,
                               int *restrict pshared)
{
    *pshared = PTHREAD_PROCESS_PRIVATE;
    return 0;
}

int
pthread_barrierattr_setpshared(pthread_barrierattr_t *attr __unused,
                               int pshared)
{
    if (pshared != PTHREAD_PROCESS_PRIVATE) {
        errno = EINVAL;
        return -1;
    }
    return 0;
}

int
pthread_barrier_init(pthread_barrier_t *restrict barrier,
                     const pthread_barrierattr_t *restrict attr __unused,
                     unsigned count)
{
    if (count == 0) {
        errno = EINVAL;
        return -1;
    }
    
    if (pthread_mutex_init(&barrier->mutex, 0) < 0) {
        return -1;
    }
    if (pthread_cond_init(&barrier->cond, 0) < 0) {
        int errno_save = errno;
        pthread_mutex_destroy(&barrier->mutex);
        errno = errno_save;
        return -1;
    }
    
    barrier->limit = count;
    barrier->count = 0;
    barrier->phase = 0;
    
    return 0;
}

int
pthread_barrier_destroy(pthread_barrier_t *barrier)
{
    pthread_mutex_destroy(&barrier->mutex);
    pthread_cond_destroy(&barrier->cond);
    return 0;
}

int
pthread_barrier_wait(pthread_barrier_t *barrier)
{
    pthread_mutex_lock(&barrier->mutex);
    barrier->count++;
    if (barrier->count >= barrier->limit) {
        barrier->phase++;
        barrier->count = 0;
        pthread_cond_broadcast(&barrier->cond);
        pthread_mutex_unlock(&barrier->mutex);
        return PTHREAD_BARRIER_SERIAL_THREAD;
    } else {
        unsigned phase = barrier->phase;
        do
            pthread_cond_wait(&barrier->cond, &barrier->mutex);
        while (phase == barrier->phase);
        pthread_mutex_unlock(&barrier->mutex);
        return 0;
    }
}

#endif /* __APPLE__ */

#define NTHREADS 32
#define NELEMENTS 1024
#define STRIPLENGTH NELEMENTS/NTHREADS

pthread_t threads[NTHREADS];
pthread_barrier_t barrier;

struct argumenti{
    int id;
    double* pa;
    double* pb;
    double result;
};

struct argumenti args[NTHREADS];

double vecA[NELEMENTS];
double vecB[NELEMENTS];

double partialResults[NTHREADS];

void *funkcija_za_nit(void *arg);

int main(int argc, const char * argv[]) {
    pthread_barrier_init(&barrier, NULL, NTHREADS);
    
    for (int i = 0; i < NELEMENTS; i++) {
        vecA[i] = 1.0;
        vecB[i] = 1.0;
    }
    
    for (int i = 0; i < NTHREADS; i++) {
        args[i].id = i;
        args[i].pa = vecA + i * STRIPLENGTH;
        args[i].pb = vecB + i * STRIPLENGTH;
        
        pthread_create(&threads[i], NULL, *funkcija_za_nit, (void *)&args[i]);
    }
    
    for (int i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("Skalarni produkt vektorjev: %f\n", partialResults[0]);
    
    pthread_barrier_destroy(&barrier);
    
    return 0;
}

void *funkcija_za_nit(void *arg){
    struct argumenti *parg = arg; // Vseen priporočamo casting (struct argumenti*)
    
    double *pStripA = parg->pa, *pStripB = parg->pa, result = 0.0;
    
    for (int i=0; i < STRIPLENGTH; i++) {
        //midResult =  *(pStripA + i) * *(pStripB + i);
        result += pStripA[i] * pStripB[i];
    }
    
    partialResults[parg->id] = result;
    pthread_barrier_wait(&barrier);
    
    /* 2. DEL - Redukcijsko seštevanje delnih produktov */
    
    int working_threads = NTHREADS / 2;
    while(working_threads > 0){
        if(parg->id < working_threads){
            partialResults[parg->id] += partialResults[parg->id + working_threads];
        }
        pthread_barrier_wait(&barrier);
        working_threads /= 2;
    }
    
    return NULL;
}


#include <stdio.h>
#include <pthread.h>

#define NTHREADS 10

pthread_t nit[NTHREADS];

struct argumenti{
    int id;
    double* pa;
    double* pb;
    double* pc;
};

struct argumenti args[NTHREADS];

double vecA[NTHREADS];
double vecB[NTHREADS];
double vecC[NTHREADS];

void *funkcija_za_nit(void *arg);

int main(int argc, const char * argv[]) {
    for (int i = 0; i < NTHREADS; i++) {
        vecA[i] = 1.0 + i;
        vecB[i] = 1.0 + 2*i;
        vecC[i] = 0;
        
        args[i].id = i;
        args[i].pa = &vecA[i];
        args[i].pb = &vecB[i];
        args[i].pc = &vecC[i];
        
        pthread_create(&nit[i], NULL, *funkcija_za_nit, (void *)&args[i]);
    }
    
    for (int i = 0; i < NTHREADS; i++) {
        pthread_join(nit[i], NULL);
    }
    
    for (int i = 0; i < NTHREADS; i++) {
        printf("c[%d] = %f \n", i, vecC[i]);
    }
    
    return 0;
}

void *funkcija_za_nit(void *arg){
    struct argumenti *parg = arg; // Vseen priporočamo casting (struct argumenti*)
    double a, b;
    
    a = *parg->pa;
    b = *parg->pb;
    
    *parg->pc = a + b;
    
    //printf("Pozdravljen, sem nit številka: %d!\n", parg->id);
    return NULL;
}

#include <stdio.h>
#include <pthread.h>

#define NTHREADS 10
#define NELEMENTS 10000000
#define STRIPLENGTH NELEMENTS/NTHREADS

pthread_t threads[NTHREADS];

//pthread_mutex_t lock;

struct argumenti{
    int id;
    double* pa;
    double* pb;
    double result;
};

struct argumenti args[NTHREADS];

double vecA[NELEMENTS];
double vecB[NELEMENTS];
double vecC[NELEMENTS];

void *funkcija_za_nit(void *arg);

int main(int argc, const char * argv[]) {
    double scalarProduct = 0;
    //pthread_mutex_init(&lock, NULL);
    
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
        scalarProduct += args[i].result;
    }
    
    printf("Skalarni produkt vektorjev: %f\n", scalarProduct);
    
    return 0;
}

void *funkcija_za_nit(void *arg){
    struct argumenti *parg = arg; // Vseen priporočamo casting (struct argumenti*)
    
    double *pStripA = parg->pa, *pStripB = parg->pa, result = 0.0;
    
    for (int i=0; i < STRIPLENGTH; i++) {
        //midResult =  *(pStripA + i) * *(pStripB + i);
        //while(pthread_mutex_lock(&lock));
        result += pStripA[i] * pStripB[i];
        //pthread_mutex_unlock(&lock);
    }
    
    parg->result = result;
    
    //printf("Pozdravljen, sem nit številka: %d in je moj delni produkt enak %f!\n", parg->id, result);
    
    return NULL;
}


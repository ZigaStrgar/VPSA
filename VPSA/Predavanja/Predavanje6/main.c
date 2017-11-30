#include <stdio.h>
#include <omp.h>
#include <unistd.h>

#define NTHREADS 5

void *think_and_eat(void *arg);
void *think_and_eat_safe();

omp_lock_t locks[NTHREADS];

int main(int argc, const char * argv[]) {
    omp_set_num_threads(NTHREADS);
    for (int i = 0; i < NTHREADS; i++) {
        omp_init_lock(&locks[i]);
    }
    
#pragma omp parallel
    {
        think_and_eat_safe();
    }
    
    for (int i = 0; i < NTHREADS; i++) {
        omp_destroy_lock(&locks[i]);
    }
    
    return 0;
}

void *think_and_eat_safe(){
    int philosopher_id = omp_get_thread_num();
    for (int i = 0; i < 1000; i++) {
        printf("Sem filozof %d in razmisljam...\n", philosopher_id);
        usleep(philosopher_id * 2000 + 42);
        
        while(1){
            omp_set_lock(&locks[philosopher_id]);
            //printf("Sem filozof %d in sem pograbil levo palčko...\n", philosopher_id);
            if(omp_test_lock(&locks[(philosopher_id + 1) % NTHREADS])){
                //printf("Sem filozof %d in sem pograbil desno palčko...\n", philosopher_id);
                
                printf("Sem filozof %d in jem...\n", philosopher_id);
                usleep(philosopher_id * 117870 + 56);
                
                omp_unset_lock(&locks[(philosopher_id + 1) % NTHREADS]);
                omp_unset_lock(&locks[philosopher_id]);
                
                break;
            }
            omp_unset_lock(&locks[philosopher_id]);
        }
    }
    
    return NULL;
}




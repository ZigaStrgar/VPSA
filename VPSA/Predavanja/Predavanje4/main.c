#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NTHREADS 5

pthread_t philosophers[NTHREADS];
pthread_mutex_t locks[NTHREADS];

struct arguments{
    int id;
};

struct arguments args[NTHREADS];


void *think_and_eat(void *arg);
void *think_and_eat_safe(void *arg);

int main(int argc, const char * argv[]) {
    for (int i = 0; i < NTHREADS; i++) {
        pthread_mutex_init(&locks[i], NULL);
    }
    
    for (int i = 0; i < NTHREADS; i++) {
        args[i].id = i;
        
        pthread_create(&philosophers[i], NULL, *think_and_eat_safe, (void *)&args[i]);
    }
    
    for (int i = 0; i < NTHREADS; i++) {
        pthread_join(philosophers[i], NULL);
    }
    
    return 0;
}


void *think_and_eat(void *arg){
    struct arguments *parg = arg; // Vseen priporočamo casting (struct argumenti*)
    
    int philosopher_id = parg->id;
    
    //while(1){
    // 1. Think
    for (int i = 0; i < 1000; i++) {
        printf("Sem filozof %d in razmisljam...\n", philosopher_id);
        usleep(philosopher_id * 2000 + 42);
        // 2. HUNGRY
        // 2a. Take left stick
        pthread_mutex_lock(&locks[philosopher_id]);
        printf("Sem filozof %d in sem pograbil levo palčko...\n", philosopher_id);
        // 2b. Take right stick
        usleep(philosopher_id * 270 * i + 1276 * i);
        pthread_mutex_lock(&locks[(philosopher_id + 1) % NTHREADS]);
        // 3. EAT
        printf("Sem filozof %d in jem...\n", philosopher_id);
        usleep(philosopher_id * 78700 + 56);
        // 4. Release the sticks
        pthread_mutex_unlock(&locks[(philosopher_id + 1) % NTHREADS]);
        pthread_mutex_unlock(&locks[philosopher_id]);
    }
    
    //}
    
    return NULL;
}

void *think_and_eat_safe(void *arg){
    struct arguments *parg = arg; // Vseen priporočamo casting (struct argumenti*)
    
    int philosopher_id = parg->id;

    for (int i = 0; i < 100; i++) {
        printf("Sem filozof %d in razmisljam...\n", philosopher_id);
        usleep(philosopher_id * 2000 + 42);
        while(1){
            pthread_mutex_lock(&locks[philosopher_id]);
            usleep(philosopher_id * 270 * i + 1276 * i);
            printf("Sem filozof %d in sem pograbil levo palčko...\n", philosopher_id);
            if(pthread_mutex_trylock(&locks[(philosopher_id + 1) % NTHREADS]) == 0){
                printf("Sem filozof %d in jem...\n", philosopher_id);
                usleep(philosopher_id * 7870 + 56);
                
                break;
            }
            pthread_mutex_unlock(&locks[philosopher_id]);
            usleep(philosopher_id * 270 * i + 1276 * i);
        }
        
        pthread_mutex_unlock(&locks[(philosopher_id + 1) % NTHREADS]);
        pthread_mutex_unlock(&locks[philosopher_id]);
    }

    return NULL;
}



#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>

int cmpfunc (const void * a, const void * b) {
    return ( *(int*)a - *(int*)b );
}

int main(int argc, char *argv[])
{
    int my_rank; // rank (oznaka) procesa
    int num_of_processes; // ötevilo procesov
    MPI_Status status; // status sprejema
    
    MPI_Init(&argc, &argv); // inicializacija MPI okolja
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); // poizvedba po ranku procesa
    MPI_Comm_size(MPI_COMM_WORLD, &num_of_processes); // poizvedba po ötevilu procesov
#define N 10000
    
    int *sendcnts;
    int *displs;
    int *recvbuf;
    int i;
    int j;
    sendcnts = (int *) malloc(num_of_processes * sizeof(int));
    displs = (int *) malloc(num_of_processes * sizeof(int));
    
    int table[N];
    int sorted[N];
    
    srand(time(NULL));
    
    i = 0;
    int stripSize = N / num_of_processes;
    int mod = N % num_of_processes;
    while(i < num_of_processes){
        sendcnts[i] = (i != num_of_processes - 1) ? stripSize : stripSize + mod;
        displs[i] = i * stripSize;
        i++;
    }
    
    if(my_rank == 0){
        i = 0;
        while(i < N){
            table[i] = rand() % N;
            i++;
        }
    }
    
    recvbuf = (int *) malloc(sendcnts[my_rank] * sizeof(int));
    
    MPI_Scatterv(table, sendcnts, displs, MPI_INT, recvbuf, sendcnts[my_rank], MPI_INT, 0, MPI_COMM_WORLD);
    
    qsort(recvbuf, sendcnts[my_rank], sizeof(int), cmpfunc);
    
    MPI_Gatherv(recvbuf, sendcnts[my_rank], MPI_INT, table, sendcnts, displs, MPI_INT, 0, MPI_COMM_WORLD);
    int is_sorted = 1;
    if(my_rank == 0){
        for(i = 0; i < N; i++){
            int min = 2147483647;
            int min_index = -1;
            for(j = 0; j < num_of_processes; j++){
                if(sendcnts[j] > 0){
                    if(table[displs[j]] < min){
                        min = table[displs[j]];
                        min_index = j;
                    }
                }
            }
            sorted[i] = min;
            sendcnts[min_index]--;
            displs[min_index]++;
        }
        
        for(i = 1;i < N; i++){
            if(sorted[i-1] > sorted[i]){
                is_sorted = 0;
                break;
            }
        }
        if(is_sorted == 1){
            printf("Sorted correclty\n");
        } else {
            printf("Error sorting array\n");
        }
    }
    
    MPI_Finalize();
    
    free(sendcnts);
    free(displs);
    free(recvbuf);
    
    return 0;
}

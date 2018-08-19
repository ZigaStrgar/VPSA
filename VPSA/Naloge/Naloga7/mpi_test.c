#include <stdio.h>
#include <string.h> 
#include <mpi.h>

int main(int argc, char *argv[]) 
{ 
    int my_rank; // rank (oznaka) procesa
    int num_of_processes; // število procesov
    int source; // rank pošiljatelja
    int destination; // rank sprejemnika
    int tag = 0; // zaznamek sporoèila
    char message[100]; // rezerviran prostor za sporoèilo
    MPI_Status status; // status sprejema
    
    MPI_Init(&argc, &argv); // inicializacija MPI okolja
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); // poizvedba po ranku procesa
    MPI_Comm_size(MPI_COMM_WORLD, &num_of_processes); // poizvedba po številu procesov
    
    if( my_rank != 0 ) // procesi z rankom != 0 imajo enako funkcijo
    {
        MPI_Recv(message, 100, MPI_CHAR, my_rank - 1, tag, MPI_COMM_WORLD, &status);
        sprintf(message + strlen(message), " - %d", my_rank);
        if(my_rank == num_of_processes - 1){
            MPI_Send(message, (int)strlen(message)+1, MPI_CHAR,
                     0, tag, MPI_COMM_WORLD);
        }
        else{
            MPI_Send(message, (int)strlen(message)+1, MPI_CHAR,
                     my_rank + 1, tag, MPI_COMM_WORLD);
        }
    }
    else // proces z rankom == 0 se obnaša drugaèe
    {
        sprintf(message, "%d", my_rank);
        if(my_rank == num_of_processes - 1){
            MPI_Send(message, (int)strlen(message)+1, MPI_CHAR,
                     0, tag, MPI_COMM_WORLD);
        }
        else{
            MPI_Send(message, (int)strlen(message)+1, MPI_CHAR,
                     my_rank + 1, tag, MPI_COMM_WORLD);
        }
        MPI_Recv(message, 100, MPI_CHAR, num_of_processes - 1, tag, MPI_COMM_WORLD, &status);
        sprintf(message + strlen(message), " - %d", my_rank);
        printf("%s\n", message);
        fflush(stdout);
    }
    MPI_Finalize();
    
    return 0;
} 

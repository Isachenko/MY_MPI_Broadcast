#include <stdio.h>
#include <mpich/mpi.h>
#include "bcast.h"

#define COLOR_RESET "\e[m"
#define COLOR_GREEN "\e[32m"
#define COLOR_RED  "\033[22;31m"

#define MASTER_NUM 5

int rank, size;
int vectorSize;
double* vector;


void output(int vectorSize, double* vector, int color)
{
    printf(color?COLOR_GREEN "vector: \n" COLOR_RESET:COLOR_RED "vector: \n" COLOR_RESET);
    for(int i = 0; i < vectorSize; ++i) {
        printf(color?COLOR_GREEN "%.1lf " COLOR_RESET :COLOR_RED "%lf " COLOR_RESET, vector[i] );
    }
    printf("\n");
}

void goMaster(int* argc, char ***argv) {
    //Init---------------------------------
    printf(COLOR_GREEN "Master start\n" COLOR_RESET);

    if (*argc != 2) {
        printf("Set Param: VectorFileName\n");
        throw 101;
    }

    //vector
    char* vectorFileName = (*argv)[1];
    FILE* vectorFile = fopen(vectorFileName, "r");
    fscanf(vectorFile, "%d", &vectorSize);
    vector = new double[vectorSize];
    for(int i = 0; i < vectorSize; ++i) {
        fscanf(vectorFile, "%lf", vector + i);
    }

    //send vector
    printf(COLOR_GREEN "Master: bcast1\n" COLOR_RESET);
    bcast(&vectorSize, 1, MPI_INT, MASTER_NUM, MPI_COMM_WORLD);

    printf(COLOR_GREEN "Master: bcast2\n" COLOR_RESET);
    bcast(vector, vectorSize, MPI_DOUBLE, MASTER_NUM, MPI_COMM_WORLD);
    output(vectorSize, vector, 0);

    printf(COLOR_GREEN "Master: finished\n" COLOR_RESET);
}


void goSlave()
{
    printf(COLOR_RED "Slave start\n" COLOR_RESET);
    printf(COLOR_RED "Slave: bcast1\n" COLOR_RESET);
    bcast(&vectorSize, 1, MPI_INT, MASTER_NUM, MPI_COMM_WORLD);
    vector = new double[vectorSize];
    printf(COLOR_RED "Slave: bcast2\n" COLOR_RESET);
    bcast(vector, vectorSize, MPI_DOUBLE, MASTER_NUM, MPI_COMM_WORLD);
    printf("vector size: %d\n", vectorSize);
    output(vectorSize, vector, 1);
    printf(COLOR_RED "Slave: finished\n" COLOR_RESET);
}

int main (int argc, char **argv)
{

    MPI_Init (&argc, &argv);	/* starts MPI */
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */
    MPI_Comm_size (MPI_COMM_WORLD, &size);	/* get number of processes */
    if (rank == MASTER_NUM) {
        goMaster(&argc, &argv);
    }
    else {
        goSlave();
    }
    MPI_Finalize();
    return 0;
}

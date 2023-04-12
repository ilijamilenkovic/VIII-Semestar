#include <mpi.h>
#include <stdio.h>
void main(int argc, char* argv[]) {
    printf("OVDE 0");
int i, p,color,rank,rankc,size,key;
MPI_Comm newcomm;
printf("OVDE 1");
MPI_Init(&argc,&argv);
MPI_Comm_size(MPI_COMM_WORLD, &p);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
color=rank%2;//key=7;
printf("OVDE 2");
MPI_Comm_split(MPI_COMM_WORLD,color,rank,&newcomm);
printf("OVDE 3");
MPI_Comm_size(newcomm, &size);
MPI_Comm_rank(newcomm, &rankc);
printf("moj rank je: %d, moj key je %d, moj rankc je %d moj color je %d moj size je %d\n",rank,rank,rankc,color,size);
MPI_Finalize();
}
/*Napisati MPI program kojim se kreira dvodimenzionalna Cartesian struktura sa n vrsta i m
kolona. Svaki od nxm procesa izračunava sumu identifikatora njegovog gornjeg i donjeg
suseda. Obezbediti da se u master procesu na kraju programa nađe ukupna izračunata suma*/

#include <stdio.h>
#include <mpi.h>
#define N 4
#define M 3
#define root 0
#define v 0
int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    int top, bottom,local_sum, world_rank, global_sum,cartesian_rank;
    int dims[] = { N,M }, periods[] = {true,true};
    MPI_Comm cartesian;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, false, &cartesian);
    MPI_Cart_shift(cartesian, 1, 1, &top, &bottom);
    local_sum = top + bottom;
    MPI_Comm_rank(cartesian, &cartesian_rank);
    printf("P[%d]: top = %d, bottom = %d, sum = %d\n", cartesian_rank,top,bottom, local_sum);
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);
    if (world_rank == root)
        printf("Ukupna suma je %d\n", global_sum);

    MPI_Finalize();
}


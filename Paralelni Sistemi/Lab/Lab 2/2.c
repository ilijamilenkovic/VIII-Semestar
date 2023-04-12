/*Napisati MPI program kojim se kreira dvodimenzionalna Cartesian struktura sa n vrsta i m
kolona. U svakom od nxm procesa odštampati identifikatore procesa njegovog levog i desnog
suseda na udaljenosti 2. Smatrati da su procesi u prvoj i poslednjoj koloni jedne vrste susedni.*/

#include <stdio.h>
#include <mpi.h>
#define N 4
#define M 3
int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    MPI_Comm cartesian_comm;
    int my_rank, reorder, left_neighbour, right_neighbour;
    int dims[2], periods[2];
    dims[0] = N;
    dims[1] = M;
    periods[0] = true;
    periods[1] = true;
    reorder = true;

    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &cartesian_comm);
    MPI_Comm_rank(cartesian_comm, &my_rank);
    MPI_Cart_shift(cartesian_comm, 0, 2, &left_neighbour, &right_neighbour);
    

    printf("%d: left: %d, right: %d", my_rank, left_neighbour, right_neighbour);


    MPI_Finalize();
}
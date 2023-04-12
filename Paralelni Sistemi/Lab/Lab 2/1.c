/*Napisati MPI program koji kreira komunikator koji se sastoji od dijagonalnih procesa u
kvadratnoj mreži procesa. Iz master procesa novog komunikatora poslati poruku svim ostalim
procesima. Svaki proces novog komunikatora treba da prikaže primljenu poruku, identifikator
procesa u novom komunikatoru i stari identifikator procesa*/

#include <stdio.h>
#include <mpi.h>
#include <math.h>
#define N 4
#define root 0
struct message {
    int size;
    char* payload;
};

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    int my_rank, reorder;
    int dim[2], period[2], coords[2];
    MPI_Comm cartesian_quadratic, diagonal;
    dim[0] = dim[1] = N;
    period[0] = period[1] = true;
    reorder = false;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dim, period, reorder, &cartesian_quadratic);
    MPI_Comm_rank(cartesian_quadratic, &my_rank);
    MPI_Cart_coords(cartesian_quadratic, my_rank, 2, coords);
    MPI_Comm_split(cartesian_quadratic, coords[0] == coords[1], N, &diagonal);

    if (coords[0] == coords[1]) {
        int my_diagonal_rank;
        MPI_Comm_rank(diagonal, &my_diagonal_rank);
        int message;
        if (my_diagonal_rank == root)
            message = 12311;

        MPI_Bcast(&message, 1, MPI_INT, root, diagonal);
        printf("P%d: Message: %d; world_rank: %d; diagonal_rank: %d\n", my_rank, message, my_rank, my_diagonal_rank);
    }



    MPI_Finalize();
}
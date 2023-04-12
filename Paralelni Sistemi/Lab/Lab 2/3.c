/*Napisati MPI program koji kreira komunikator comm1 koji se sastoji od svih procesa sa
identifikatorima deljivim sa 3. Master proces (P0) svim procesima ove grupe šalje po jednu
vrstu matrice A. Odštampati identifikatore procesa koji pripadaju comm1 i čija je suma
elemenata primljene vrste matrice A manja od zadate vrednosti v.*/

#include <stdio.h>
#include <mpi.h>
#define N 4
#define M 3
#define root 0
#define v 0
int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    int my_rank, size;
    int matrix[N][N];
    int matrixRow[N];
    MPI_Comm comm1;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_split(MPI_COMM_WORLD, my_rank % 3, 16, &comm1);
    if (my_rank % 3 == 0) {
        //Matrix initialization
        if (my_rank == root) {
            int num = 1;
            for (int i = 0; i < N; i++)
                for (int j = 0; j < N; j++)
                    matrix[i][j] = num++;
        }
        MPI_Scatter(matrix, N, MPI_INT, matrixRow, N, MPI_INT, root, comm1);
        int sum = 0;
        
        
        for (int i = 0; i < N; i++)
            sum += matrixRow[i];


        if (sum > v) {
            int comm1_rank;
            MPI_Comm_rank(comm1, &comm1_rank);
            printf("P[%d]: sum = %d, world_rank = %d, comm1_rank = %d", my_rank, sum, my_rank, comm1_rank);
        }
        

    }

    MPI_Finalize();
}


/*Slanje nulte kolone procesa 0 u prvu vrstu procesa 1*/

#include <mpi.h>
#include <stdio.h>
#define n 10
int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int my_rank;
    int mat[n][n];

    int num=0;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            mat[i][j] = num++;
    


    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    if(my_rank == 0){
        MPI_Datatype new_type;
        MPI_Type_vector(n,1,n,MPI_INT, &new_type);
        MPI_Type_commit(&new_type);
        MPI_Send(mat,1,new_type,1,1,MPI_COMM_WORLD);
    }
    else if(my_rank == 1){
        //int recv_buff[n];
        MPI_Status status;
        MPI_Recv(mat,n,MPI_INT,0,1,MPI_COMM_WORLD,&status);
        printf("Proces 1 matrica nakon prijema: \n");
        for(int i = 0; i < n; i++){
            for (int j = 0; j < n; j++)
            {
                printf("%d ", mat[i][j]);
            }
            printf("\n");
        }
    }




    MPI_Finalize();}

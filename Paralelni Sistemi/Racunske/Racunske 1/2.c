/*Napisati program kojim se elementi gornje trougaone matrice Anxn
procesa 0, šalju u gornju trougaonu matricu Tnxn
procesa 1.*/

#include "mpi.h"
#include "stdio.h"
#define n 10

int main(int argc, char **argv){
    MPI_Init(&argc, &argv);
    int my_rank;
    int mat[n][n];
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int arr_of_dissplacements[n];
    int arr_of_block_lens[n];

    for(int i = 0; i < n; i++){
        arr_of_dissplacements[i] = i*n + i;
        arr_of_block_lens[i] = n-i;
    }
    MPI_Datatype new_type;
    MPI_Type_indexed(n,arr_of_block_lens,arr_of_dissplacements,MPI_INT,&new_type);
    MPI_Type_commit(&new_type);




    if(my_rank == 0){
        int num_for_insert = 1;
       
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                mat[i][j] = (j < i) ? 0 : num_for_insert++;
    

        
        
        // for (int i = 0; i < n; i++)
        // {
        //     for(int j = 0; j < n; j++){
        //         printf("%d\t", mat[i][j]);
        //     }
        //     printf("\n");
        // }

        MPI_Send(mat,1,new_type,1,0,MPI_COMM_WORLD);
        
        
    }
    else if(my_rank == 1)
    {
        for(int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                mat[i][j] = 1;

        MPI_Status status;
        MPI_Recv(mat,1,new_type,0,0,MPI_COMM_WORLD, &status);
        
        for (int i = 0; i < n; i++)
        {
            for(int j = 0; j < n; j++){
                printf("%d\t", mat[i][j]);
            }
            printf("\n");
        }
            
            
        
        
    }

    MPI_Finalize();
}
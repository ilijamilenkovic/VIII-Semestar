/*Proces 0 kreira matricu reda n i šalje i-om  procesu  po  dve  kvazidijagonale  matrice,  obe 
na  udaljenosti  i  od  glavne  dijagonale.  Proces  i  kreira  svoju  matricu  tako  što  smešta 
primljene  dijagonale  u  prvu  i  drugu  kolonu  matrice  a  ostala  mesta    popunjava  nulama. 
Napisati MPI program koji realizuje opisanu komunikaciju korišćenjem izvedenih tipova 
podataka i prikazuje vrednosti odgovarajućih kolona*/

#include "mpi.h"
#include "stdio.h"
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#define n 6

int main(int argc, char **argv){
    srand(time(NULL));
    MPI_Init(&argc, &argv);
    int my_rank, size;
    int block_count;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int arr_of_dissplacements_top[size-1];
    int arr_of_dissplacements_bottom[size-1];
    int arr_of_block_lens[size-1];
    int recv_arr[2*(size-1)];
    
    int mat[n][n];
    //inicijalizacija
    if(my_rank == 0){
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                mat[i][j] = rand() % 10;
                printf("%d\t", mat[i][j]);
            }
            printf("\n");
            
        }

        for (int i = 1; i < size; i++)
        {
            block_count = n-i;
            int pom = 0;

            
            // printf("Proces %d arr_of_displacement:\n", i);
            for (int j = 0; j < size-i; j++)
            {
                arr_of_dissplacements_top[j] = i + j * (n+1);
                arr_of_dissplacements_bottom[j] = i * n + j*(n+1);
                
                arr_of_block_lens[pom++] = 1;
            }
            printf("Top:\n");
            for (int j = 0; j < size-i; j++)
            {
                printf("%d ", arr_of_dissplacements_top[j]);
            }

            printf("\nBottom:\n");
            for (int j = 0; j < size-i; j++)
            {
                printf("%d ", arr_of_dissplacements_bottom[j]);
            }
            

            // printf("\n\n");
            //gornja dijagonala
            MPI_Datatype new_type;
            MPI_Type_indexed(block_count, arr_of_block_lens, arr_of_dissplacements_top, MPI_INT, &new_type);
            MPI_Type_commit(&new_type);
            MPI_Send(mat, 1, new_type, i, 0, MPI_COMM_WORLD);


            //donja dijagonala
            MPI_Datatype new_type_bottom;
            MPI_Type_indexed(block_count, arr_of_block_lens, arr_of_dissplacements_bottom, MPI_INT, &new_type_bottom);
            MPI_Type_commit(&new_type_bottom);
            MPI_Send(mat, 1, new_type_bottom, i, 0, MPI_COMM_WORLD);

            
        
        }
            
    }
    //ostali procesi
    else{

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                mat[i][j] = 0;
                
            }
            printf("\n");
        }
            
        
        MPI_Status status;
        // MPI_Recv(recv_arr, n-my_rank, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        // MPI_Recv(&recv_arr[n-my_rank], n-my_rank, MPI_INT, 0,0, MPI_COMM_WORLD,&status );

        MPI_Datatype new_type, new_type_resized;
        MPI_Type_vector(n-my_rank,1,n,MPI_INT,&new_type);
        MPI_Type_create_resized(new_type, 0, sizeof(int), &new_type_resized);
        MPI_Type_commit(&new_type_resized);
        MPI_Recv(&mat[0][0],1,new_type_resized,0,0,MPI_COMM_WORLD, &status);
        MPI_Recv(&mat[0][1], 1, new_type_resized,0,0, MPI_COMM_WORLD, &status);


        //NECE DA SE SINHRONIZUJE DA STAMPA KAKO TREBA
        sleep(my_rank);
        printf("Proces %d: \n",my_rank);
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                
                printf("%d \t", mat[i][j]);
            }
            

        }  
        printf("\n");          
        

        
        

    }

    




    MPI_Finalize();

}
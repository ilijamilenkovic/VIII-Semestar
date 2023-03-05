/*Napisati  MPI  program  koji  pronalazi  maksimalnu  vrednost  u  delu  matrice  reda  n  (n-
parno) koga čine vrste matrice sa parnim indeksom (i=0,2,4,...). Matrica je inicijalizovana u master procesu (P0).
 Svaki proces treba da dobije elemente vrsta sa parnim indeksom iz 
odgovarajućih n/p kolona (p-broj procesa, n deljivo sa p) i nađe lokalni maksimum. 
Na taj  način,  P0  dobija  elemente  vrsta  sa  parnim  indeksom  iz  prvih  n/p  kolona  i  nalazi lokalni 
maksimum, P1 dobija elemente kolona sa parnim indeksom iz sledećih n/p kolona 
i  nalazi  lokalni  maksimum  itd.  Nakon toga, u master procesu se izračunava i  na  ekranu prikazuje 
globalni maksimum u traženom delu matrice. Zadatak  realizovati korišćenjem isključivo grupnih operacija i 
izvedenih tipova podataka.*/

#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define n 4

int main(int argc, char **argv)
{
    srand(time(NULL));   // Initialization, should only be called once.
    

    MPI_Init(&argc, &argv);

    int my_rank, num_of_processes;
    int mat[n][n];

    MPI_Comm_size(MPI_COMM_WORLD, &num_of_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    //Broj elemenata po koloni * broj kolona
    int sending_size = (n / 2) * (n / num_of_processes);
    int local_arr[sending_size];

   
    //kreiranje tipa
    MPI_Datatype new_type, new_type_resized;
    MPI_Type_vector(n/num_of_processes, n/num_of_processes, 2*n ,MPI_INT, &new_type);
    MPI_Type_commit(&new_type);
    MPI_Type_create_resized(new_type, 0, n/num_of_processes * sizeof(int) ,&new_type_resized);
    MPI_Type_commit(&new_type_resized);
    
    //inicijalizacija matrice
    if(my_rank == 0){

        for(int i = 0; i < n; i++){        
            for (int j = 0; j < n; j++){    
                mat[i][j] = rand() % 1024;
                printf("%d\t", mat[i][j]);
            }
            printf("\n");
        }     
        printf("\n\n");
    }
    MPI_Scatter(mat, 1, new_type_resized,local_arr, sending_size, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < sending_size; i++)
    {
        printf("Process %d got %d\n", my_rank, local_arr[i]);
    }
    printf("\n\n");
    


    //trazenje i prikazivanje minimuma:

    struct {
        int rank;
        int maximum;
        
    } local_maximum_info, global_maximum_info;

    int max = local_arr[0];
    
    for(int i = 1; i < sending_size; i++)
        if(local_arr[i] > max)
            max = local_arr[i];
    
    local_maximum_info.maximum = max;
    local_maximum_info.rank = my_rank;

    MPI_Reduce(&local_maximum_info, &global_maximum_info,1,MPI_DOUBLE_INT,MPI_MAXLOC,0,MPI_COMM_WORLD);
    
    if(my_rank == 0){
        printf("Maximum is %d and it's from process %d\n", global_maximum_info.maximum, global_maximum_info.rank);
    }


    




    MPI_Finalize();
}
/* Napisati MPI program koji pronalazi minimalnu vrednost u delu matrice reda n (n-parno) 
koga čine kolone matrice sa parnim indeksom (j=0,2,4,...). Matrica  je  inicijalizovana  u 
master procesu (P0). Svaki proces treba da dobije elemente kolona sa parnim indeksom iz 
odgovarajućih n/p vrsta (p-broj procesa, n deljivo sa p) i nađe lokalni minimum. Na taj način, 
P0  dobija  elemente  kolona  sa  parnim  indeksom  iz  prvih  n/p  vrsta  i  nalazi  lokalni minimum, 
 P1  dobija elemente kolona sa parnim indeksom iz sledećih n/p vrsta i nalazi lokalni minimum itd. 
 Nakon toga, u master procesu se izračunava i na ekranu prikazuje globalni minimum u traženom delu matrice. 
 Zadatak    realizovati korišćenjem isključivo 
grupnih operacija i izvedenih tipova podataka.*/

#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define n 16

int main(int argc, char **argv)
{
    srand(time(NULL));   // Initialization, should only be called once.
    

    MPI_Init(&argc, &argv);

    int my_rank, num_of_processes;
    int mat[n][n];

    MPI_Comm_size(MPI_COMM_WORLD, &num_of_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    //Broj elemenata po vrsti * broj vrsta
    int sending_size = (n / 2) * (n / num_of_processes);
    int local_arr[sending_size];

   
    //kreiranje tipa
    MPI_Datatype new_type, new_type_resized;
    MPI_Type_vector(sending_size, 1, 2 ,MPI_INT, &new_type);
    MPI_Type_commit(&new_type);
    MPI_Type_create_resized(new_type, 0, sending_size * 2 * sizeof(int) ,&new_type_resized);
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
    }
    MPI_Scatter(mat, 1, new_type_resized,local_arr, (n / 2) * (n / num_of_processes), MPI_INT, 0, MPI_COMM_WORLD);



    //trazenje i prikazivanje minimuma:

    struct {
        int rank;
        int minimum;
        
    } local_minimum_info, global_minimum_info;

    int min = local_arr[0];
    
    for(int i = 1; i < sending_size; i++)
        if(local_arr[i] < min)
            min = local_arr[i];
    
    local_minimum_info.minimum = min;
    local_minimum_info.rank = my_rank;

    MPI_Reduce(&local_minimum_info, &global_minimum_info,1,MPI_DOUBLE_INT,MPI_MINLOC,0,MPI_COMM_WORLD);

    if(my_rank == 0){
        printf("Minimum is %d and it's from process %d\n", global_minimum_info.minimum, global_minimum_info.rank);
    }


    




    MPI_Finalize();
}
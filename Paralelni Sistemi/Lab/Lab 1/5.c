/*Napisati  MPI  program  koji  realizuje  množenje  matrice  Amxn  i  matrice  Bnxk,  čime  se 
dobija rezultujuća matrica Cmxk. Množenje se obavlja tako što master proces šalje svakom 
procesu celu matricu A i po k/p kolona matrice B (p-broj procesa, k je deljivo sa p). Svi 
procesi učestvuju u izračunavanju. Konačni rezultat množenja se nalazi u master procesu 
koji ga i prikazuje. Predvideti da se slanje k/p kolona matrice B svakom procesu obavlja 
odjednom  i  to  direktno  iz  matrice  B.  Zadatak  rešiti  korišćenjem  grupnih  operacija  i 
izvedenih tipova podataka*/

#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#define m 3
#define n 2
#define k 6
#define root 0

int main(int argc, char **argv)
{
    srand(time(NULL)); 
    MPI_Init(&argc, &argv);
    int my_rank,size;
    int A[m][n];
    int B[n][k];
    int C[m][k];
    
    
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int B_temp[n][k/size];
    int C_temp[m][k/size];
    //int recv_buffer_B[n * (k / size)];

    if(my_rank == root){
        //mat A init
        printf("Matrica A: \n");
        for (int i = 0; i < m; i++){
            for (int j = 0; j < n; j++){ 
                A[i][j] = rand() % 10;
                printf("%d\t", A[i][j]);
            }
            printf("\n");
        }

        //mat B init
        printf("Matrica B: \n");
        for (int i = 0; i < n; i++){
            for (int j = 0; j < k; j++){ 
                B[i][j] = rand() % 10;
                printf("%d\t", B[i][j]);
            }
            printf("\n");
        }
    }

    //Slanje cele matrice A
    MPI_Bcast(A, m * n, MPI_INT, root, MPI_COMM_WORLD);

    //Slanje po k/p kolona matrice B    
    MPI_Datatype new_type, new_type_resized;
    MPI_Type_vector(n, k / size, k, MPI_INT, &new_type);
    MPI_Type_commit(&new_type);
    MPI_Type_create_resized(new_type, 0, (k / size) * sizeof(int), &new_type_resized);
    MPI_Type_commit(&new_type_resized);
    
    MPI_Scatter(B, 1, new_type_resized, B_temp,  n*(k/size), MPI_INT, root, MPI_COMM_WORLD);

    //Izracunavanje:
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < k / size; j++)
        {
            C_temp[i][j] = 0;
            for (int l = 0; l < n; l++)
            {
                C_temp[i][j] += (A[i][l]*B_temp[l][j]);
            }
            
        }
        
    }
     
    MPI_Datatype return_type, return_type_resized;
    MPI_Type_vector(m, k / size, k, MPI_INT, &return_type);
    MPI_Type_commit(&return_type);
    MPI_Type_create_resized(return_type, 0, (k / size) * sizeof(int), &return_type_resized);
    MPI_Type_commit(&return_type_resized);

    MPI_Gather(C_temp, m*(k/size), MPI_INT, C, 1, return_type_resized, root, MPI_COMM_WORLD);


    if(my_rank == root){
    printf("Rezultat:\n-----------------\n");

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < k; j++)
        {
            printf("%d\t", C[i][j]);
        }
        
        printf("\n");
    }
    }

    MPI_Finalize();

}
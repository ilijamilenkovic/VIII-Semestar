/*Napisati  MPI  program  koji  realizuje  množenje  matrica  A  i  B  reda  n,  čime  se  dobija 
rezultujuća matrica C. Nakon toga, u matrici C pronaći maksimalnu vrednost elemenata svake kolone.
 Množenje se obavlja tako što master proces šalje svakom procesu radniku 
po jednu vrstu prve matrice i celu drugu matricu. Svaki proces računa po jednu  vrstu 
rezultujuće  matrice  i  šalje  master  procesu.  Svi  procesi  učestvuju u  izračunavanju. 
Štampati dobijenu matricu kao i maksimalne vrednosti elemenata svake kolone. Zadatak 
rešiti korišćenjem grupnih operacija*/
#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#define n 3
#define root 0

int main(int argc, char **argv)
{
    srand(time(NULL)); 
    MPI_Init(&argc, &argv);
    int my_rank,size, local_max;
    int A[n][n], B[n][n], C[n][n], A_row[n], C_row[n], C_col[n], max[n];

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

      if(my_rank == root){
        //mat A init
        printf("Matrica A: \n");
        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++){ 
                A[i][j] = rand() % 10;
                printf("%d\t", B[i][j]);
            }
            printf("\n");
        }

        //mat B init
        printf("Matrica B: \n");
        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++){ 
                B[i][j] = rand() % 10;
                printf("%d\t", B[i][j]);
            }
            printf("\n");
        }
    }

    //slanje po jedne vrste matrice A

    MPI_Scatter(A,n,MPI_INT,A_row,n,MPI_INT,root,MPI_COMM_WORLD);

    MPI_Bcast(B, n*n, MPI_INT, root, MPI_COMM_WORLD);

    
    for (int j = 0; j < n; j++)
    {
        C_row[j] = 0;
        for (int l = 0; l < n; l++)
        {
            C_row[j] += A_row[l]*B[l][j];
        }
        
    }

    MPI_Gather(C_row, n, MPI_INT, C, n, MPI_INT, root, MPI_COMM_WORLD);

    if(my_rank == root){
        printf("Rezultat: \n");
        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++){ 
              
                printf("%d\t", C[i][j]);
            }
            printf("\n");
        }
    }
        
    MPI_Datatype new_type, new_type_resized;
    MPI_Type_vector(n, 1, n, MPI_INT, &new_type);
    MPI_Type_commit(&new_type);
    MPI_Type_create_resized(new_type, 0, sizeof(int), &new_type_resized);
    MPI_Type_commit(&new_type_resized);

    MPI_Scatter(C, 1, new_type_resized, C_col, n, MPI_INT, root, MPI_COMM_WORLD);

    local_max = C_col[0];
    for (int i = 1; i < n; i++) 
        if(C_col[i] > local_max)
            local_max = C_col[i];

    MPI_Gather(&local_max,1,MPI_INT,max,1,MPI_INT,root,MPI_COMM_WORLD);

    if(my_rank == 0){
        printf("Maksimumi: \n");
        for (int i = 0; i < n; i++)
        {
            printf("%d ", max[i]);
        }
        printf("\n");
        
    }    
    
    

    
    

    MPI_Finalize();
}
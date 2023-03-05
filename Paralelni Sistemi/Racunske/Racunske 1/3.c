/* Napisati MPI program koji čita celobrojne (integer) i vrednosti duple
tačnosti (double-precision) sa standardnog ulaza i saopštava to svim ostalim
procesima*/

#include "mpi.h"
#include "stdio.h"

int main(int argc, char** argv){
    MPI_Init(&argc, &argv);
    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
    struct 
    {
        int a;
        double b;
    } my_struct;

    int arr_of_blocklens[2];
    MPI_Aint arr_of_disps[2];
    MPI_Datatype old_types[2];

    arr_of_blocklens[0] = arr_of_blocklens[1] = 1;
    old_types[0] = MPI_INT;
    old_types[1] = MPI_DOUBLE;

    MPI_Get_address(&my_struct.a,&arr_of_disps[0]);
    MPI_Get_address(&my_struct.b,&arr_of_disps[1]);

    arr_of_disps[1] = arr_of_disps[1] - arr_of_disps[0];
    arr_of_disps[0] = 0;
    MPI_Datatype new_type;
    MPI_Type_create_struct(2,arr_of_blocklens,arr_of_disps,old_types,&new_type);
    MPI_Type_commit(&new_type);

    if(my_rank == 0){
        //pribavi podatke sa ulaza i salji ih kroz bcast

        scanf("%d %f", &my_struct.a, &my_struct.b);
        printf("Proces %d je primio podatke %d, %lf korisnika.\n", my_rank,my_struct.a, my_struct.b);
       
    }

    MPI_Bcast(&my_struct,1,new_type,0,MPI_COMM_WORLD);
        //primi podatke
    
    printf("Proces %d je primio podatke %d, %f od procesa 0\n", my_rank,my_struct.a, my_struct.b);
    



    MPI_Finalize();

}
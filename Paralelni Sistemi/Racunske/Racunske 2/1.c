// Zad. Napisati MPI program kojim se vrši podela procesa članova
// komunikatora MPI_COMM_WORLD u dve grupe: grupu procesa sa neparnim
// identifikatorima i grupu procesa sa parnim identifikatorima

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
void main(int argc, char* argv[]) {
    MPI_Group WORLD_GROUP, ODD_GROUP, EVEN_GROUP;
    int total_number_of_processes, number_of_even, number_of_odd, my_rank, odd_rank, even_rank;
    int* odd_members;
    MPI_Init(&argc,&argv);
    //1. get the number of processes in WORLD_GROUP and initialize WORLD_GROUP 
    MPI_Comm_size(MPI_COMM_WORLD, &total_number_of_processes);
    MPI_Comm_group(MPI_COMM_WORLD, &WORLD_GROUP);
    //2. get the rank of each process
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    //3. make odd group
    number_of_odd = total_number_of_processes/2;
    odd_members = (int*)malloc(number_of_odd);
    
    for (int i = 0; i < number_of_odd; i++)
        odd_members[i] = i * 2 + 1;
    
    MPI_Group_incl(WORLD_GROUP, number_of_odd, odd_members, &ODD_GROUP);
    
    
    //4. make even group
    number_of_even = total_number_of_processes - number_of_odd;
    MPI_Group_excl(WORLD_GROUP, number_of_odd, odd_members, &EVEN_GROUP);

    MPI_Group_rank(ODD_GROUP, &odd_rank);
    MPI_Group_rank(EVEN_GROUP, &even_rank);
    printf("moj rank je: %d, moj even rank je %d, moj odd rank je %d\n",my_rank,even_rank, odd_rank);
    MPI_Finalize();

    free(odd_members);
}
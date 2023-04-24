/*April 2023*/
/*
Ima bag pri racunici, al je distribucija dobra.	

*/

#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#define n 8
#define p 16
#define q 4
#define s 2
#define master 0



void printMatrix(int* mat, int dim1, int dim2);
void initMatrix(int* mat, int dim1, int dim2);

struct loc_struct {
	int value;
	int rank;
};

int main(int argc, char** argv) {
	
	MPI_Init(&argc, &argv);
	MPI_Comm process_matrix, row,col;
	MPI_Datatype row_of_a_block, block; //row_of_a_block je 1 red 1 bloka. Blok ima s redova
	int my_rank;
	int A[n][n], b[n], local_A[s][s], local_b[s], local_result[s], local_result_row[n], result[n];
	int dims[2], periods[2];
	dims[0] = dims[1] = q;
	periods[0] = periods[1] = false;

	
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, false, &process_matrix);
	MPI_Comm_rank(process_matrix, &my_rank);
	

	//Kreiranje tipa: 

	//Kreiranje reda bloka
	MPI_Type_vector(s, 1, q, MPI_INT, &row_of_a_block);
	//mozda sledece 2 linije ne trebaju
	MPI_Type_create_resized(row_of_a_block, 0, sizeof(int), &row_of_a_block);
	MPI_Type_commit(&row_of_a_block);

	//Kreiranje celog bloka (sastavljen od s redova):
	MPI_Type_vector(s, 1, n, row_of_a_block, &block);
	MPI_Type_create_resized(block, 0, sizeof(int), &block);
	MPI_Type_commit(&block);

	MPI_Comm_split(process_matrix, my_rank / q, my_rank, &row);
	MPI_Comm_split(process_matrix, my_rank % q, my_rank, &col);

	if (my_rank == master) {
		//Inicijalizacija matrice A
		initMatrix(A[0], n, n);
		//inicijalizacija vektora b
		initMatrix(b, 1, n);
		printf("Matrica A:\n");
		printMatrix(A[0], n, n);
		printf("\n");
		printf("Vektor b:\n");
		printMatrix(b, 1, n);
		printf("\n");

		
		//Slanje matrice A:
		
		int dest = 1;
		for (int i = 0; i < q; i++)
			for (int j = (i==0) ? 1 : 0; j < q; j++) //ako se salje prva vrsta, onda ne saljemo nultom procesu da ne bi blokirali program, otud ternarni operator
				MPI_Send(&A[s*i][j], 1, block, dest++, 0, process_matrix);//salju se elementi iz svake druge vrste (s*i)
			
		

		//ODVAJANJE ELEMENATA ZA PROCES 0
		for (int i = 0; i < s; i++)	
			for (int j = 0; j < s; j++)
				local_A[i][j] = A[i][j * q];	
	}

	//Primanje delova matrice A
	else
		MPI_Recv(&local_A, s * s, MPI_INT, master, 0, process_matrix, MPI_STATUSES_IGNORE);
		
	

	//Slanje vektora b:
	//Svaki proces dobija S elemenata
	//row_of_a_block tip koji smo napravili bas odgovara ovde :)
	//saljemo podatke vektora prvom redu, pa oni to distribuiraju po svojoj koloni, jer procesi jedne kolone primaju iste elemente vektora
	if (my_rank / q == 0) {
		MPI_Scatter(b, 1, row_of_a_block, local_b, s, MPI_INT, 0, row);
	}
	MPI_Bcast(local_b, s, MPI_INT, 0, col);

	/*
	printf("P%d je primio:\n", my_rank);
	printMatrix(local_b, 1, s);
	printf("\n");
	*/

	//Lokalno mnozenje:
	for (int i = 0; i < s; i++)
	{
		local_result[i] = 0;
		for (int j = 0; j < s; j++)
			local_result[i] += (local_A[i][j] * local_b[j]);
	}
	MPI_Gather(local_result,  s, MPI_INT, local_result_row,  s, MPI_INT, 0, row);
	if (my_rank % q == 0) {
		MPI_Reduce(local_result_row, result, n, MPI_INT, MPI_SUM, 0, col);
		if (my_rank == 0) {
			printf("P%d: Rezultat\n", my_rank);
			printMatrix(result, 1, n);
		}
	}

	
		

	



	
	
	MPI_Finalize();
}

void printMatrix(int* mat, int dim1, int dim2) {
	for (int i = 0; i < dim1; i++)
	{
		for (int j = 0; j < dim2; j++)
			printf("%d\t", mat[i * dim1 + j]);
		
		printf("\n");
	}
}

void initMatrix(int* mat, int dim1, int dim2) {
	srand(time(NULL));

	for (int i = 0; i < dim1; i++)
	{
		for (int j = 0; j < dim2; j++)
		{
			mat[i * dim1 + j] = rand() % 10;
		}

	}
}




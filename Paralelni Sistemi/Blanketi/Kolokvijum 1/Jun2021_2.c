#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#define n 8

#define master 0

void printSquareMatrix(int* mat, int dim);

int main(int argc, char** argv) {
	srand(time(NULL));

	MPI_Init(&argc, &argv);
	int my_rank, size_of_comm;
	MPI_Comm process_matrix, row_comm, col_comm;
	MPI_Datatype block_temp, block;
	int A[n][n], B[n][n], C[n][n], C_test[n][n],
		local_block_A[2][2], local_block_B[2][2], 
		local_row_of_blocks[2][n], local_col_of_blocks[n][2],
		local_result[2][2],
		local_result_row[2][n];

	//Kreiranje matrice procesa
	int dims[2], periods[2];
	dims[0] = dims[1] = 4;
	periods[0] = periods[1] = false;
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, false, &process_matrix);

	MPI_Comm_size(process_matrix, &size_of_comm);
	MPI_Comm_rank(process_matrix, &my_rank);

	//Kreiranje strukture bloka
	int q = sqrt(size_of_comm);//=sqrt(16)
	int k = n / q; //8/4
	MPI_Type_vector(k, k, n, MPI_INT, &block_temp);
	MPI_Type_create_resized(block_temp, 0, k, &block);
	MPI_Type_commit(&block);


	if (my_rank == master) {
		//Inicijalizacija matrica
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
			{
				A[i][j] = rand() % 10;
				B[i][j] = rand() % 10;
			}

		printf("A:\n");
		printSquareMatrix(A[0], n);
		printf("\n");

		printf("B:\n");
		printSquareMatrix(B[0], n);
		printf("\n");


		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
			{
				C_test[i][j] = 0;
				for (int l = 0; l < n; l++)
					C_test[i][j] += A[i][l] * B[l][j];

			}
		//Slanje blokova
		int dest = 1;	
		for (int i = 0; i < q; i++)
			for (int j = (i == 0) ? 1 : 0; j < q; j++) {
				MPI_Send(&A[i * k][j * k], 1, block, dest, 0, process_matrix);
				MPI_Send(&B[i * k][j * k], 1, block, dest++, 0, process_matrix);
				
			}
		//local_block_A i local_block_B za P0
		for (int i = 0; i < k; i++)
			for (int j = 0; j < k; j++)
			{
				local_block_A[i][j] = A[i][j];
				local_block_B[i][j] = B[i][j];
			}

	}
	else {
		MPI_Recv(local_block_A, k * k, MPI_INT, master, 0, process_matrix, MPI_STATUSES_IGNORE);
		MPI_Recv(local_block_B, k * k, MPI_INT, master, 0, process_matrix, MPI_STATUSES_IGNORE);

		
	}

	//Slanje vrsta matrice A
	
	// 
	//1. Napravi komunikator za vrste
	MPI_Comm_split(process_matrix, my_rank / q, my_rank, &row_comm); //q je sqrt(size_of_comm) -> 4
	//MPI_Datatype row_block;
	

	//2. Odredi mastera
	int my_row_rank;
	MPI_Comm_rank(row_comm, &my_row_rank);

	//3. Gather local_block_A u local_row_of_blocks, send type je cela matrica, a recv je block
	
	//ZASTO OVO NE RADI?
	//MPI_Gather(local_block_A, k* k, MPI_INT, local_row_of_blocks, 1, block, 0, row_comm);
	
	MPI_Gather(local_block_A, k, MPI_INT, local_row_of_blocks, k, MPI_INT, 0, row_comm);
	MPI_Gather(&local_block_A[1][0], k, MPI_INT, &local_row_of_blocks[1][0], k, MPI_INT, 0, row_comm);
	

	//4. Bcast local_row_of_blocks
	MPI_Bcast(local_row_of_blocks, k* n, MPI_INT, 0, row_comm);
	

	//Slanje kolona matrice B
	//1. Napravi komunikator za kolone
	MPI_Comm_split(process_matrix, my_rank % q, my_rank, &col_comm); //q je sqrt(size_of_comm) -> 4

	//2. Odredi mastera
	int my_col_rank;
	MPI_Comm_rank(col_comm, &my_col_rank);

	//3. Gather local_block_B u local_col_of_blocks
	MPI_Gather(local_block_B, k*k, MPI_INT, local_col_of_blocks, k*k, MPI_INT, 0, col_comm);
	//MPI_Gather(&local_block_B[1][0], k, MPI_INT, &local_col_of_blocks[1][0], k, MPI_INT, 0, col_comm);

	//4. Bcast local_col_of_blocks
	MPI_Bcast(local_col_of_blocks, k* n, MPI_INT, 0, col_comm);
	

	//Racunanje
	//Svaki proces ima jedinstvenu kombinaciju vrsta i kolona i na osnovu nje racuna svoj blok rezultata koji je kxk
	for (int i = 0; i < k; i++)
		for (int j = 0; j < k; j++)
		{
			local_result[i][j] = 0;
			for (int l = 0; l < n; l++)
				local_result[i][j] += local_row_of_blocks[i][l] * local_col_of_blocks[l][j];
			
		}
	//Supljanje rezultata
	// 
	//ZASTO NE RADI?
	//MPI_Gather(local_result, k* k, MPI_INT, local_result_row, 1, block, 0, row_comm);
	MPI_Gather(local_result, k, MPI_INT, local_result_row, k, MPI_INT, 0, row_comm);
	MPI_Gather(&local_result[1][0], k, MPI_INT, &local_result_row[1][0], k, MPI_INT, 0, row_comm);
	
	MPI_Gather(&local_result_row, 2 * n, MPI_INT, &C[0][0], 2 * n, MPI_INT, 0, col_comm);

	
	if (my_rank == 0) {
		printf("P%d: RESULT: \n", my_rank);
		printSquareMatrix(C[0], n);
	}
	MPI_Finalize();
}

void printSquareMatrix(int* mat, int dim) {
	for (int i = 0; i < dim; i++)
	{
		for (int j = 0; j < dim; j++)
			printf("%d\t", mat[i * dim + j]);
		
		printf("\n");
	}
}
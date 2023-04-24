#include <mpi.h>
#include <stdio.h>
struct Student
{
	int indeks;
	char ime[20];
	char prezime[20];
	float prosek;
	int godina_studija;

};

int main(int argc, char** argv) {
	
	MPI_Init(&argc, &argv);
	Student student;
	int my_rank;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	if (my_rank == 0) {
		printf("Unesi indeks:\n");
		scanf_s("%d", &student.indeks);
		printf("Unesi ime:\n");
		scanf_s("%s", &student.ime);
		printf("Unesi prezime:\n");
		scanf_s("%s", &student.prezime);
		printf("Unesi prosek:\n");
		scanf_s("%f", &student.prosek);
		printf("Unesi godinu studija:\n");
		scanf_s("%d", &student.godina_studija);
	}

	MPI_Datatype student_struct;
	int blocklens[5];
	MPI_Aint indices[5];
	MPI_Datatype old_types[5];

	blocklens[0] = blocklens[3] = blocklens[4] = 1;
	blocklens[1] = blocklens[2] = 20;
	old_types[0] = MPI_INT;
	old_types[1] = MPI_CHAR;
	old_types[2] = MPI_CHAR;
	old_types[3] = MPI_FLOAT;
	old_types[4] = MPI_INT;

	MPI_Address(&student.indeks, &indices[0]);
	MPI_Address(&student.ime, &indices[1]);
	MPI_Address(&student.prezime, &indices[2]);
	MPI_Address(&student.prosek, &indices[3]);
	MPI_Address(&student.godina_studija, &indices[4]);

	for (int i = 1; i < 4; i++)
		indices[i] -= indices[0];
	indices[0] = 0;

	MPI_Type_struct(5, blocklens, indices, old_types, &student_struct);
	MPI_Bcast(&student, 1, student_struct, 0, MPI_COMM_WORLD);

	printf("P%d: indeks = %d, ime = %s, prezime = %s, prosek = %f, godina = %d", my_rank, student.indeks, student.ime, student.prezime, student.prosek, student.godina_studija);
	
	MPI_Finalize();
}
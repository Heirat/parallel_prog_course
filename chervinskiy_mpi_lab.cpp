#include <iostream>
#include <fstream>
#include <string>
#include <mpi.h>
#include <stdlib.h>
#include <cstdlib>

using namespace std;

void generateFile(int N) {
	ofstream result;
	result.open("C.txt");
	int i, j;
	srand((unsigned)time(NULL));
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			result << (double)rand() / RAND_MAX * 10 << " ";
		}
		result << endl;
	}
	result.close();
}

int main(int argc, char** argv)
{	
	int N = 1000;
	int rank, size, count, i, j, t, th_num;

	// Смещение, если размер матрицы не делится поровну на кол-во процессов, то остатки уйдут последнему
	int surplus = 0; 
	double calc_time, reading_time;	

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status status;

	ifstream file1, file2, file3;
	ofstream result;
	
	
	double* A = new double[N * N];
	double* B = new double[N * N];
	double* C = new double[N * N];

	// Разделяем порции данных между всеми процессами
	count = N / size;
	if (rank == size - 1 && N % size != 0) {
		surplus = N % size;
	}

	//нулевой процесс считывает данные и отправляет данные всем остальным процессам
	if (rank == 0) {
		cout << "Dimension of the matrices is " << N << "x" << N << endl;
		// открываем файлы с числами и заполняем матрицы
		cout << "Started data reading" << endl;
		reading_time = MPI_Wtime();
		file1.open("A.txt");
		for (i = 0; i < N; i++) {
			for (j = 0; j < N; j++) {
				file1 >> A[i * N + j];
			}
		}
		file1.close();

		file2.open("B.txt");
		for (i = 0; i < N; i++) {
			for (j = 0; j < N; j++) {
				file2 >> B[i * N + j];
			}
		}
		file2.close();

		reading_time = MPI_Wtime() - reading_time;
		cout << "Time spent on reading: " << reading_time << endl;

		cout << "Started calculation" << endl;
		calc_time = MPI_Wtime();


		for (th_num = 1; th_num < size; th_num++) {

			MPI_Send(&A[th_num * count * N], (th_num == size - 1) ? (count + N % size) * N : count * N, MPI_DOUBLE, th_num, 1, MPI_COMM_WORLD);
			MPI_Send(&B[0], N * N, MPI_DOUBLE, th_num, 2, MPI_COMM_WORLD);
		}
	}

	else {

		MPI_Recv(&A[rank * count * N], (count + surplus) * N, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&B[0], N * N, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, &status);
	}

	
	for (i = rank * count; i < count * (rank + 1) + surplus; i++) {
		for (j = 0; j < N; j++) {
			C[i * N + j] = 0;
			for (t = 0; t < N; t++) {
				C[i * N + j] += A[i * N + t] * B[t * N + j];
			}
		}
	}

	delete[] A;
	delete[] B;


	if (rank != 0) {
		MPI_Send(&C[rank * count * N], (count + surplus) * N, MPI_DOUBLE, 0, 4, MPI_COMM_WORLD);
	}
	else {

		for (th_num = 1; th_num < size; th_num++) {
			MPI_Recv(&C[th_num * count * N], (th_num == size - 1) ? (count + N % size) * N : count * N, MPI_DOUBLE, th_num, 4, MPI_COMM_WORLD, &status);
		}

		calc_time = MPI_Wtime() - calc_time;
		cout << "Time spent on calculation: " << calc_time << endl;
		
		/*

		result.open("result.txt");
		for (i = 0; i < N; i++) {
			for (j = 0; j < N; j++) {
				result << C[i * N + j] << " ";
			}
			result << endl;
		}
		result.close();
		*/
	}

	MPI_Finalize();
	delete[] C;	
	return 0;
}



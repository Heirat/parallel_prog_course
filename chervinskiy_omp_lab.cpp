#include <iostream>
#include <time.h>
#include <fstream>
#include <vector>
#include <omp.h>

using namespace std;

int main()
{
	//размерность матриц
	int N = 1000;

	// количество процессов
	int threads_num;

	// объявляем матрицы
	
	double** A = NULL, ** B = NULL, ** C = NULL;
	ifstream file1, file2, file3;
	ofstream result;

	cout << "Dimension of the matrices is " << N << "x" << N << endl;

	cout << "Enter the number of processes: ";
	cin >> threads_num;

	omp_set_num_threads(threads_num);

	// выделяем память под наши матрицы
	A = new double* [N];
	B = new double* [N];
	C = new double* [N];

	for (int i = 0; i < N; i++){
		A[i] = new double[N];
		B[i] = new double[N];
		C[i] = new double[N];
	}

	// открываем файлы с числами и заполняем матрицы
	cout << "Started data reading" << endl;
	double reading_time = omp_get_wtime();

	file1.open("A.txt");
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			file1 >> A[i][j];
		}
	}
	file1.close();

	file2.open("B.txt");
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			file2 >> B[i][j];
		}
	}
	file2.close();

	reading_time = omp_get_wtime() - reading_time;
	cout << "Time spent on reading: " << reading_time << endl;

	
	cout << "Started calculation" << endl;
	double calc_time = omp_get_wtime();

	int i, j, t;

#pragma omp parallel num_threads(threads_num)
	{
#pragma omp for private (i,j,t)
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				C[i][j] = 0;
				for (int t = 0; t < N; t++) {
					C[i][j] += A[i][t] * B[t][j];
				}
			}
		}
	}


	calc_time = omp_get_wtime() - calc_time;
	cout << "Time spent on calculation: " << calc_time << endl;

	/*
	result.open("res2.txt");
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			result << C[i][j] << " ";
		}
		result << endl;
	}
	result.close();
	*/
	system("pause");
	return 0;
}

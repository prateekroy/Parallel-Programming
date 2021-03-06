#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <mutex>
#include <math.h>
using namespace std;

std::mutex mtx;
time_t seconds;
int getRandomInt(){
	int max = 100; int min = -100;
    int randomval = min + (rand() % static_cast<int>(max - min + 1));
    return randomval;
} 

void getCoordOfProcessor(int index, int p, int& x, int& y){
	x = index/p;
	y = index%p;
}

int getIndexOfProcessor(int x, int y, int p){
	return (p*x)+y;
}

int **alloc_2d_int(int rows, int cols) {
    int *data = (int *)malloc(rows*cols*sizeof(int));
    int **array= (int **)malloc(rows*sizeof(int*));
    for (int i=0; i<rows; i++)
        array[i] = &(data[cols*i]);

    for (int i = 0; i < rows; ++i)
    {
    	for (int j = 0; j < cols; ++j)
    	{
    		array[i][j] = 0;
    	}
    }

    return array;
}

void copyMatrix(int** src, int** dest, int sn, int sm, int dn, int dm, int n, int m){
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			dest[i+dn][j+dm] = src[i+sn][j+sm];
		}
	}
}

void multiplyMatrix(int** C, int** A, int** B, int n, int m){
	for (int i = 0; i < n; ++i)
	{
		for (int k = 0; k < m; ++k)
		{
			for (int j = 0; j < n; ++j)
			{
				C[i][j] += A[i][k]*B[k][j];
			}
		}
	}
}

void printMatrix(int** mat, int n, int m){
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			cout << mat[i][j] << " ";
		}
		cout << endl;
	}
}

int** Split(int** mat, int si, int sj, int n){
	int** a = alloc_2d_int(n, n);
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			a[i][j] = mat[si+i][sj+j];
		}
	}
	return a;
}

#include <cilk/cilk.h>
void multiplyMatrixPar(int** C, int** A, int** B, int n, int m){
	#pragma cilk grainsize = 1
	cilk_for (int i = 0; i < n; ++i)
	{
		for (int k = 0; k < m; ++k)
		{
			for (int j = 0; j < n; ++j)
			{
				C[i][j] += A[i][k]*B[k][j];
			}
		}
	}
}

void rec_matrix_mul(int **a, int **b, int **c, int ai, int aj, int bi, int bj, int ci, int cj,int n){

	if (n < 512)
	{
		// c[ci][cj] += a[ai][aj] * b[bi][bj];

	    cilk_for (int i = ci; i < ci+n; i++)
	    {
	    	for (int k = bj; k < bj+n; ++k)
	        {
	        	for (int j = aj; j < aj+n; j++)
	        	{
	        		c[i][j] += a[i][k]*b[k][j];
	        	}
			}
	    }			
	}
	else
	{

		cilk_spawn rec_matrix_mul(a, b, c, ai, aj, bi, bj, ci, cj, n/2);

		cilk_spawn rec_matrix_mul(a, b, c, ai, aj, bi, bj+n/2, ci, cj + n/2, n/2);

		cilk_spawn rec_matrix_mul(a, b, c, ai+n/2, aj, bi, bj, ci+n/2, cj, n/2);

		cilk_spawn rec_matrix_mul(a, b, c, ai+n/2, aj, bi, bj+n/2, ci+n/2, cj+n/2, n/2);			


		cilk_sync;


		cilk_spawn rec_matrix_mul(a, b, c, ai, aj+n/2, bi+n/2, bj, ci, cj, n/2);

		cilk_spawn rec_matrix_mul(a, b, c, ai, aj+n/2, bi+n/2, bj+n/2, ci, cj + n/2, n/2);

		cilk_spawn rec_matrix_mul(a, b, c, ai+n/2, aj+n/2, bi+n/2, bj, ci+n/2, cj, n/2);

		cilk_spawn rec_matrix_mul(a, b, c, ai+n/2, aj+n/2, bi+n/2, bj+n/2, ci+n/2, cj+n/2, n/2);	

		cilk_sync;

	}

}

void Collaborate(int** C, int n, int processors){



    int seg_size = n/processors;

    for (int p = 1; p < processors*processors; ++p)
    {
    	int** rM = alloc_2d_int(seg_size, seg_size);
    	// cout << "Waiting for receive " << seg_size*seg_size << endl;
    	MPI_Recv(&rM[0][0], seg_size*seg_size, MPI_INT, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	    int x, y;
	    getCoordOfProcessor(p, processors, x, y);    	

	    copyMatrix(rM, C, 0, 0, x*seg_size, y*seg_size, seg_size, seg_size);
    }

    // printMatrix(C, n, n);
}

void Verify(int** C, int** A, int** B, int n, int m){
		
	int** serial_C = alloc_2d_int(n, m);
	multiplyMatrix(serial_C, A, B, n, m);

	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			if (serial_C[i][j] != C[i][j])
			{
				cout << "Answer Wrong\n";
				return;
			}
		}
	}

	cout << "Perfecto!!" << endl;
}

void MM_Rotate_Gen(int** C, int** A, int** B, int n, int p){
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int x, y;
    getCoordOfProcessor(world_rank, p, x, y);

    int seg_size = n/p;


    int** lA = Split(A, x*seg_size, y*seg_size, seg_size);
    int** lB = Split(B, x*seg_size, y*seg_size, seg_size);
    int** lC = alloc_2d_int(seg_size, seg_size);


    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

	MPI_Comm col_comm, row_comm;
	//Column communicator
	int color = world_rank%p;
	MPI_Comm_split(MPI_COMM_WORLD, color, world_rank, &col_comm);
	int col_rank, col_size;
	MPI_Comm_rank(col_comm, &col_rank);
	MPI_Comm_size(col_comm, &col_size);

	//Row communicator
	color = world_rank/p;
	MPI_Comm_split(MPI_COMM_WORLD, color, world_rank, &row_comm);
	int row_rank, row_size;
	MPI_Comm_rank(row_comm, &row_rank);
	MPI_Comm_size(row_comm, &row_size);

    int** sA = alloc_2d_int(seg_size, seg_size);
    int** sB = alloc_2d_int(seg_size, seg_size);


    for (int l = 1; l <= p; ++l)
    {
    	int k = l - 1;  

    	copyMatrix(lA, sA, 0, 0, 0, 0, seg_size, seg_size);
    	MPI_Bcast(&sA[0][0], seg_size*seg_size, MPI_INT, k, row_comm);
    	// cout << "1 " << world_rank << endl;

    	copyMatrix(lB, sB, 0, 0, 0, 0, seg_size, seg_size);
    	MPI_Bcast(&sB[0][0], seg_size*seg_size, MPI_INT, k, col_comm);
    	// cout << "2 " << world_rank << endl;

    	// multiplyMatrixPar(lC, sA, sB, seg_size, seg_size);
    	multiplyMatrix(lC, sA, sB, seg_size, seg_size);
  		// rec_matrix_mul(sA, sB, lC, 0, 0, 0, 0, 0, 0, seg_size);

    }
 
	    MPI_Barrier(MPI_COMM_WORLD);
	    double end_time = MPI_Wtime();

    //Debug log
    if (world_rank == 2)
    {

    	// printMatrix(lC, seg_size, seg_size);
    	// cout << "\n***********************\n";
    	// printMatrix(lB, seg_size, seg_size);
    }

    if (world_rank == 0)
    {
		//Master
		// copyMatrix(lC, C, 0, 0, x*seg_size, y*seg_size, seg_size, seg_size);

		// //Wait for other processors and collaborate
		// Collaborate(C, n, p);	

	    // MPI_Barrier(MPI_COMM_WORLD);
	    // double end_time = MPI_Wtime();

		// Verify(C, A, B, n, n);
		printf( "Time taken : %lf seconds \n" , end_time - start_time );

    }
    else
    {
    	// cout << "Sent" << world_rank << " " << seg_size*seg_size << endl;
   		// MPI_Send(&lC[0][0], seg_size*seg_size, MPI_INT, 0, 0, MPI_COMM_WORLD);    	
    }

	// MPI_Barrier(MPI_COMM_WORLD);

	// if (world_rank == 2)
	// {
 //    	multiplyMatrix(C, A, B, n, n);
	// 	printMatrix(C, n, n);
	// }

}


//mpirun -n (rootp*rootp) ./run
int main(int argc, char const *argv[])
{
	if (argc != 3)
	{
		cout << "mpirun -n (rootp*rootp) ./run L K";
		return 0;
	}

    srand((unsigned int)seconds);
    time(&seconds);

	MPI_Init(NULL, NULL);
	
	int L = atoi(argv[1]);
	int K = atoi(argv[2]);
	int rootp = pow(2, L);		//it is rootp
	int n = pow(2, K);
	int** A = alloc_2d_int(n,n);
	int** B = alloc_2d_int(n,n);
	int** C = alloc_2d_int(n,n);

	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			A[i][j] = getRandomInt();
			B[i][j] = getRandomInt();
		}
	}


	MM_Rotate_Gen(C, A, B, n, rootp);

	// Try();



	MPI_Finalize();


	return 0;
}
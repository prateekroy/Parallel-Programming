// Rec-MM ( X, Y ) { X and Y are n × n matrices,
// where n = 2k for integer k ≥ 0 }
// 2. if n = 1 then
// 4. else
// 5. Z11 ← Rec-MM ( X11, Y11 ) + Rec-MM ( X12, Y21 )
// 6. Z12 ← Rec-MM ( X11, Y12 ) + Rec-MM ( X12, Y22 )
// 7. Z21 ← Rec-MM ( X21, Y11 ) + Rec-MM ( X22, Y21 )
// 8. Z22 ← Rec-MM ( X21, Y12 ) + Rec-MM ( X22, Y22 )
// 10. return Z

// https://github.com/sajjadpoores/matrix_multiplication2/blob/master/matrix%20mul.cpp

#include <iostream>
#include <chrono>
#include <cilk/cilk.h>
using namespace std;
using namespace std::chrono;




void rec_matrix_mul(int **a, int **b, int **c, int ai, int aj, int bi, int bj, int ci, int cj,int n){

	if (n == 128)
	{
		// c[ci][cj] += a[ai][aj] * b[bi][bj];

	    for (int i = ci; i < ci+n; i++)
	    {
	        for (int j = aj; j < aj+n; j++)
	        {
	        	for (int k = bj; k < bj+n; ++k)
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

#include <papi.h>

void question1i(int **a, int **b, int **c, int ai, int aj, int bi, int bj, int ci, int cj,int n){

    int events[1];
    events[0] = PAPI_L1_TCM;
    // events[1] = PAPI_L2_TCM;
    long long counts[1];
    int retval = PAPI_query_event(PAPI_L1_TCM);
    PAPI_start_counters(events,1);

	rec_matrix_mul(a, b, c, 0, 0, 0, 0, 0, 0, n);

    PAPI_stop_counters(counts,1);
    cout <<"L1 count "<<counts[0] << endl;
    PAPI_shutdown();
}

int main(int argc, char const *argv[])
{
	int n = 1024;
	int** x = new int*[n];
	int** y = new int*[n];
	int** z = new int*[n];

	for (int i = 0; i < n; ++i)
	{
		x[i] = new int[n];
		y[i] = new int[n];
		z[i] = new int[n];
		for (int j = 0; j < n; ++j)
		{
			x[i][j] = j;
			y[i][j] = j;
			z[i][j] = 0;
		}
	}

	auto start = high_resolution_clock::now();
	// rec_matrix_mul(x, y, z, 0, 0, 0, 0, 0, 0, n);
	question1i(x, y, z, 0, 0, 0, 0, 0, 0, n);
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout <<"Matrix multiplication : "<<duration.count() << endl;

	// for (int i = 0; i < n; ++i)
	// {
	// 	for (int j = 0; j < n; ++j)
	// 	{
	// 		cout << z[i][j] << " ";
	// 	}
	// 	cout << endl;
	// }
	return 0;
}
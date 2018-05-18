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
#include <cilk/cilk_api.h>
#include "ThreadPool.h"
#include <unistd.h>
#include <papi.h>
#include <map>
using namespace std;
using namespace std::chrono;



int totaljobs = 0;



int basehit;
volatile bool completedFlag = false;
class JobState;
class superjob;
map<int*, JobState*> hashy;
pthread_mutex_t hashLock;
pthread_mutex_t headache;

class JobState{
public:
	int type;
	int* parent_sync;
	int** a;
	int** b; 
	int** c; 
	int ai; int aj; int bi; int bj; int ci; int cj;
	int n;
	pthread_mutex_t* parent_lock;
	bool base;
	int* current_sync;
	pthread_mutex_t* current_lock;
	
	JobState(int _type, int** _a, int** _b, int** _c, int _ai, int _aj, int _bi, int _bj, int _ci, int _cj, int _n, int* _current_sync, pthread_mutex_t* _current_lock, int* _parent_sync, pthread_mutex_t* _parent_lock){
		type = _type;
		a = _a;
		b = _b; 
		c = _c; 
		ai = _ai; 
		aj = _aj; 
		bi = _bi; 
		bj = _bj; 
		ci = _ci; 
		cj = _cj;
		n = _n;
		parent_sync = _parent_sync;
		parent_lock = _parent_lock;
		current_sync = _current_sync;
		current_lock = _current_lock;
		base = false;		
	}

};



class superjob: public job{
public:
	int type;
	int* parent_sync;
	int** a;
	int** b; 
	int** c; 
	int ai; int aj; int bi; int bj; int ci; int cj;
	int n;
	thread_pool* tp;
	pthread_mutex_t* sync_lock;



	superjob(int _type, int id, int** _a, int** _b, int** _c, int _ai, int _aj, int _bi, int _bj, int _ci, int _cj, int _n, int* _sync1, thread_pool* _tp, pthread_mutex_t* _sync_lock)
	:job(id)
	{
		type = _type;
		parent_sync = _sync1;
		a = _a;
		b = _b; 
		c = _c; 
		ai = _ai; 
		aj = _aj; 
		bi = _bi; 
		bj = _bj; 
		ci = _ci; 
		cj = _cj;
		n = _n;
		tp = _tp;
		sync_lock = _sync_lock;
	}

	void firstjob(int tid){
		// tid = 0;
		if (n == basehit)
		{
			// cout << "Hit 1 job base\n";
			// c[ci][cj] += a[ai][aj] * b[bi][bj];

			// cout << ci << " " << cj << " " << ai << " " << aj << " " << bi << " " << bj << " " << endl;

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


			while (1)
			{

				pthread_mutex_lock(&hashLock);
				if (hashy.find(parent_sync) == hashy.end())
				{
					cout << "Bug while executing 22\n";
					pthread_mutex_unlock(&hashLock);
					pthread_mutex_unlock(sync_lock);
					return;
				}

				JobState* parentState = hashy[parent_sync];

				pthread_mutex_unlock(&hashLock);





				pthread_mutex_lock(parentState->current_lock);
				if (parentState->base)
				{
					cout << "Game Over\n";
					completedFlag = true;
					break;
				}

				parent_sync = parentState->current_sync;
				(*parent_sync)--;
				// cout << "Completed this task" << *parentState->current_sync<< "\n";
				if (*parent_sync != 0)
				{
					pthread_mutex_unlock(parentState->current_lock);
					break;
				}

				pthread_mutex_lock(&hashLock);
				hashy.erase(parent_sync);
				pthread_mutex_unlock(&hashLock);

				parent_sync = parentState->parent_sync;
				pthread_mutex_unlock(parentState->current_lock);






				if (parentState->type == 1)
				{
					//spawn 4 sub tasks
					// cout << "Executing second half for n = " << parentState->n << " " << tid << endl;
					int* curr_sync = new int;
					*curr_sync = 4;
					pthread_mutex_t* __sync_lock = new pthread_mutex_t;
					pthread_mutex_init(__sync_lock, NULL);			


					pthread_mutex_lock(&hashLock);
					hashy[curr_sync] = new JobState(2, parentState->a, parentState->b, parentState->c, parentState->ai, parentState->aj, parentState->bi, parentState->bj, parentState->ci, parentState->cj, parentState->n, curr_sync, __sync_lock, parentState->parent_sync, parentState->parent_lock);
					pthread_mutex_unlock(&hashLock);						

					//asign 4 jobs
					superjob* sj1 = new superjob(1, ++totaljobs, parentState->a, parentState->b, parentState->c, parentState->ai, parentState->aj+parentState->n/2, parentState->bi+parentState->n/2, parentState->bj, parentState->ci, parentState->cj, parentState->n/2, curr_sync, tp, __sync_lock);
					tp->assignJob(sj1, tid);	

					superjob* sj2 = new superjob(1, ++totaljobs, parentState->a, parentState->b, parentState->c, parentState->ai, parentState->aj+parentState->n/2, parentState->bi+parentState->n/2, parentState->bj+parentState->n/2, parentState->ci, parentState->cj + parentState->n/2, parentState->n/2, curr_sync, tp, __sync_lock);
					tp->assignJob(sj2, tid);	

					superjob* sj3 = new superjob(1, ++totaljobs, parentState->a, parentState->b, parentState->c, parentState->ai+parentState->n/2, parentState->aj+parentState->n/2, parentState->bi+parentState->n/2, parentState->bj, parentState->ci+parentState->n/2, parentState->cj, parentState->n/2, curr_sync, tp, __sync_lock);
					tp->assignJob(sj3, tid);	

					superjob* sj4 = new superjob(1, ++totaljobs, parentState->a, parentState->b, parentState->c, parentState->ai+parentState->n/2, parentState->aj+parentState->n/2, parentState->bi+parentState->n/2, parentState->bj+parentState->n/2, parentState->ci+parentState->n/2, parentState->cj+parentState->n/2, parentState->n/2, curr_sync, tp, __sync_lock);
					tp->assignJob(sj4, tid);	
					// cout << "Executed second half for n = " << parentState->n << " " << tid << endl;
					break;
				}			


			}


			

		}
		else
		{

			//baby create _a11, _b11, _c11
			int* curr_sync = new int;
			*curr_sync = 4;
			pthread_mutex_t* _sync_lock = new pthread_mutex_t;
			pthread_mutex_init(_sync_lock, NULL);		
			//lock
			pthread_mutex_lock(&hashLock);
			hashy[curr_sync] = new JobState(1, a, b, c, ai, aj, bi, bj, ci, cj, n, curr_sync, _sync_lock, parent_sync, sync_lock);
			pthread_mutex_unlock(&hashLock);

			//asign 4 jobs
			superjob* j1 = new superjob(1, ++totaljobs, a, b, c, ai, aj, bi, bj, ci, cj, n/2, curr_sync, tp, _sync_lock);
			tp->assignJob(j1, tid);	

			superjob* j2 = new superjob(1, ++totaljobs, a, b, c, ai, aj, bi, bj+n/2, ci, cj + n/2, n/2, curr_sync, tp, _sync_lock);
			tp->assignJob(j2, tid);	

			superjob* j3 = new superjob(1, ++totaljobs, a, b, c, ai+n/2, aj, bi, bj, ci+n/2, cj, n/2, curr_sync, tp, _sync_lock);
			tp->assignJob(j3, tid);	

			superjob* j4 = new superjob(1, ++totaljobs, a, b, c, ai+n/2, aj, bi, bj+n/2, ci+n/2, cj+n/2, n/2, curr_sync, tp, _sync_lock);
			tp->assignJob(j4, tid);						

		
		}
		

	}

	void working(void* param){
		int tid = *((int *)param);
		if (type == 1)
		{
			firstjob(tid);
		}

	}


};




void matrix_mul(int **a, int **b, int **c, int ai, int aj, int bi, int bj, int ci, int cj,int n, thread_pool* tp){


	// int* basesync = new int;
	// *basesync = 4;
	// pthread_mutex_t* base_lock = new pthread_mutex_t;
	// pthread_mutex_init(base_lock, NULL);	

	// pthread_mutex_lock(&hashLock);
	// JobState* nj = new JobState(1, a, b, c, ai, aj, bi, bj, ci, cj, n, basesync, base_lock, NULL, NULL);
	// nj->base = 1;
	// hashy[basesync] = nj;
	// pthread_mutex_unlock(&hashLock);


	int* _sync1 = new int;
	*_sync1 = 4;	
	pthread_mutex_t* _sync_lock = new pthread_mutex_t;
	pthread_mutex_init(_sync_lock, NULL);		

	pthread_mutex_lock(&hashLock);
	JobState* nj = new JobState(1, a, b, c, ai, aj, bi, bj, ci, cj, n, _sync1, _sync_lock, NULL, NULL);
	nj->base = true;
	hashy[_sync1] = nj;
	pthread_mutex_unlock(&hashLock);


	superjob* j1 = new superjob(1, ++totaljobs, a, b, c, ai, aj, bi, bj, ci, cj, n, _sync1, tp, _sync_lock );
	tp->assignJob(j1, 0);

	// superjob* j2 = new superjob(1, ++totaljobs, a, b, c, ai, aj, bi, bj+n/2, ci, cj + n/2, n/2, _sync1, tp, _sync_lock );
	// tp->assignJob(j2, 0);

	// superjob* j3 = new superjob(1, ++totaljobs, a, b, c, ai+n/2, aj, bi, bj, ci+n/2, cj, n/2, _sync1, tp, _sync_lock );
	// tp->assignJob(j3, 0);

	// superjob* j4 = new superjob(1, ++totaljobs, a, b, c, ai+n/2, aj, bi, bj+n/2, ci+n/2, cj+n/2, n/2, _sync1, tp, _sync_lock );
	// tp->assignJob(j4, 0);			
	// if (n == 1)
	// {
	// 	c[ci][cj] += a[ai][aj] * b[bi][bj];
	// }
	// else
	// {

	// 	cilk_spawn matrix_mul(a, b, c, ai, aj, bi, bj, ci, cj, n/2, tp);

	// 	cilk_spawn matrix_mul(a, b, c, ai, aj, bi, bj+n/2, ci, cj + n/2, n/2, tp);

	// 	cilk_spawn matrix_mul(a, b, c, ai+n/2, aj, bi, bj, ci+n/2, cj, n/2, tp);

	// 	cilk_spawn matrix_mul(a, b, c, ai+n/2, aj, bi, bj+n/2, ci+n/2, cj+n/2, n/2, tp);			

	// 	cilk_sync;

	// 	cilk_spawn matrix_mul(a, b, c, ai, aj+n/2, bi+n/2, bj, ci, cj, n/2, tp);

	// 	cilk_spawn matrix_mul(a, b, c, ai, aj+n/2, bi+n/2, bj+n/2, ci, cj + n/2, n/2, tp);

	// 	cilk_spawn matrix_mul(a, b, c, ai+n/2, aj+n/2, bi+n/2, bj, ci+n/2, cj, n/2, tp);

	// 	cilk_spawn matrix_mul(a, b, c, ai+n/2, aj+n/2, bi+n/2, bj+n/2, ci+n/2, cj+n/2, n/2, tp);	

	// 	cilk_sync;

	// }

}


void ultimate_matmul(int **a, int **b, int **c, int ai, int aj, int bi, int bj, int ci, int cj,int n, thread_pool* tp){

	if (n == 1)
	{
		c[ci][cj] += a[ai][aj] * b[bi][bj];
	}
	else
	{

		cilk_spawn ultimate_matmul(a, b, c, ai, aj, bi, bj, ci, cj, n/2, tp);

		cilk_spawn ultimate_matmul(a, b, c, ai, aj, bi, bj+n/2, ci, cj + n/2, n/2, tp);

		cilk_spawn ultimate_matmul(a, b, c, ai+n/2, aj, bi, bj, ci+n/2, cj, n/2, tp);

		cilk_spawn ultimate_matmul(a, b, c, ai+n/2, aj, bi, bj+n/2, ci+n/2, cj+n/2, n/2, tp);			

		cilk_sync;

		cilk_spawn ultimate_matmul(a, b, c, ai, aj+n/2, bi+n/2, bj, ci, cj, n/2, tp);

		cilk_spawn ultimate_matmul(a, b, c, ai, aj+n/2, bi+n/2, bj+n/2, ci, cj + n/2, n/2, tp);

		cilk_spawn ultimate_matmul(a, b, c, ai+n/2, aj+n/2, bi+n/2, bj, ci+n/2, cj, n/2, tp);

		cilk_spawn ultimate_matmul(a, b, c, ai+n/2, aj+n/2, bi+n/2, bj+n/2, ci+n/2, cj+n/2, n/2, tp);	

		cilk_sync;

	}
}

void serial_matmul(int **a, int **b, int **c, int ai, int aj, int bi, int bj, int ci, int cj,int n, thread_pool* tp){

	if (n == 1024)
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

		serial_matmul(a, b, c, ai, aj, bi, bj, ci, cj, n/2, tp);

		serial_matmul(a, b, c, ai, aj, bi, bj+n/2, ci, cj + n/2, n/2, tp);

		serial_matmul(a, b, c, ai+n/2, aj, bi, bj, ci+n/2, cj, n/2, tp);

		serial_matmul(a, b, c, ai+n/2, aj, bi, bj+n/2, ci+n/2, cj+n/2, n/2, tp);			


		serial_matmul(a, b, c, ai, aj+n/2, bi+n/2, bj, ci, cj, n/2, tp);

		serial_matmul(a, b, c, ai, aj+n/2, bi+n/2, bj+n/2, ci, cj + n/2, n/2, tp);

		serial_matmul(a, b, c, ai+n/2, aj+n/2, bi+n/2, bj, ci+n/2, cj, n/2, tp);

		serial_matmul(a, b, c, ai+n/2, aj+n/2, bi+n/2, bj+n/2, ci+n/2, cj+n/2, n/2, tp);	

	}
}

void shareon(thread_pool* tp){

	// sleep(5);
	tp->shareon();
}


void sleep10(){
	sleep(4);
}


int main(int argc, char const *argv[])
{

	fast_srand(time(NULL));
	
	int cores = __cilkrts_get_nworkers();
	cout << "Cores available : " << cores << endl;

	thread_pool tp(68);
	tp.start();


	
	basehit = 128;
	int n = 2048;
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

	// shareon(&tp);

	// cilk_sync;
	int events[1];
	events[0] = PAPI_L2_TCM;

	long long counts[1];

	int retval = PAPI_query_event(PAPI_L2_TCM);

	auto start = high_resolution_clock::now();

	PAPI_start_counters(events,1);

	matrix_mul(x, y, z, 0, 0, 0, 0, 0, 0, n, &tp);
	// ultimate_matmul(x, y, z, 0, 0, 0, 0, 0, 0, n, &tp);
	// serial_matmul(x, y, z, 0, 0, 0, 0, 0, 0, n, NULL);



	// for (int i = 0; i < n; ++i)
	// {
	// 	for (int j = 0; j < n; ++j)
	// 	{
	// 		cout << z[i][j] << " ";
	// 	}
	// 	cout << endl;
	// }

	// sleep(2);
	// shareon(&tp);
	// sleep(10);
	// cilk_spawn sleep10();

	// cilk_sync;

	while(!completedFlag){

	}

	PAPI_stop_counters(counts, 1);

	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);

	// sleep(10);
	// cout << "I am here";
	// while(1);
	// while(!tp.empty()){
	// 	cout << "Still Working\n";
	// }

	// for (int i = 0; i < n; ++i)
	// {
	// 	for (int j = 0; j < n; ++j)
	// 	{
	// 		cout << z[i][j] << " ";
	// 	}
	// 	cout << endl;
	// }


	tp.terminate();
	cout <<"\nMatrix multiplication : "<<duration.count() << endl;

	cout << "L2 count : " << counts[0] << endl;

	// sleep(5);

	// int xy;
	// cin >> xy;

	return 0;
}


#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <algorithm>
#include <papi.h>
#include <cilk/cilk.h>
#include <math.h>
using namespace std;
using namespace std::chrono;

int  n = pow(2,10);
void readInput(int  (*mat) [n][n]);
void iter_mm_ijk(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n);
void iter_mm_ikj(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n);
void iter_mm_jki(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n);
void iter_mm_jik(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n);
void iter_mm_kij(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n);
void iter_mm_kji(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n);
void initialize(int  (*z) [n][n]);
void compare(int (*dummy) [n][n],int (*z) [n][n]);

void dummy_iter_mm_kij(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n);
void dummy2_iter_mm_kij(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n);
void dummy3_iter_mm_kij(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n);
void dummy4_iter_mm_kij(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n);
void dummy5_iter_mm_kij(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n);
void dummy6_iter_mm_kij(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n);
void dummy7_iter_mm_kij(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n);

void question2Exp(int(*p_dummy) [n][n], int (*p_x) [n][n],int (*p_y) [n][n],int (*p_z) [n][n],int n);
void question2(int(*p_dummy) [n][n], int (*p_x) [n][n],int (*p_y) [n][n],int (*p_z) [n][n],int n);
void question1i(int(*p_dummy) [n][n], int (*p_x) [n][n],int (*p_y) [n][n],int (*p_z) [n][n],int n);
void question1(int(*p_dummy) [n][n], int (*p_x) [n][n],int (*p_y) [n][n],int (*p_z) [n][n],int n);

void dummy_iter_mm_ikj(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n);
void dummy2_iter_mm_ikj(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n);
void dummy3_iter_mm_ikj(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n);
void dummy4_iter_mm_ikj(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n);
void dummy5_iter_mm_ikj(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n);
void dummy6_iter_mm_ikj(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n);
void dummy7_iter_mm_ikj(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n);

int  main(){
        int  x [n][n];
        int  y [n][n];
        int  z [n][n];
        int  dummy [n][n];
        int  (*p_x) [n][n] = &x;
        int  (*p_y) [n][n] = &y;
        int  (*p_z) [n][n] = &z;
        int  (*p_dummy) [n][n] = &dummy;

        readInput(p_x);
        readInput(p_y);
        initialize(p_z);
        initialize(p_dummy);
        int retval;

        // retval = PAPI_library_init(PAPI_VER_CURRENT);
        // if (retval != PAPI_VER_CURRENT) {
        //         printf("Error! PAPI_library_init %d\n",retval);
        // }

        // retval = PAPI_query_event(PAPI_L1_TCM);
        // if (retval != PAPI_OK) {
        //         printf("L1 TCM not available\n");
        // }
        // retval = PAPI_query_event(PAPI_L2_TCM);
        // if (retval != PAPI_OK) {
        //         printf("L2 TCM not available\n");
        // }

/*
        __cilkrts_set_param("nworkers","64");
        int workers = __cilkrts_get_nworkers();
        cout<<"Get workers "<<workers<<endl;
*/


//      question2Exp(p_dummy,p_x,p_y,p_z,n);
//      question2(p_dummy,p_x,p_y,p_z,n);
        
       question1i(p_dummy,p_x,p_y,p_z,n);


        // question1(p_dummy, p_z,p_x,p_y,n);



/*
        if ((ret = PAPI_read_counters(values, 2)) != PAPI_OK) {
                fprintf(stderr, "PAPI failed to read counters: %s\n", PAPI_strerror(ret));
                exit(1);
        }
        cout<<"L1 : "<<values[0]<<" L2: "<<values[0]<<endl;
*/      cout<<"Done with multiplication";
        return  0;
}

void question1(int(*p_dummy) [n][n], int (*p_x) [n][n], int (*p_y) [n][n], int (*p_z) [n][n], int n){

        int events[1];
        events[0] = PAPI_L2_TCM;
        long long counts[1];
        int retval = PAPI_query_event(PAPI_L2_TCM);
       

        PAPI_start_counters(events,1);
        // iter_mm_kij(p_z,p_x,p_y,n);

        // initialize(p_z);
        // iter_mm_ijk(p_z,p_x,p_y,n);

        // initialize(p_z);
        // iter_mm_ikj(p_z,p_x,p_y,n);

        // initialize(p_z);
        // iter_mm_jik(p_z,p_x,p_y,n);

        // initialize(p_z);
        // iter_mm_jki(p_z,p_x,p_y,n);

        // initialize(p_z);
        iter_mm_kji(p_z,p_x,p_y,n);

        PAPI_stop_counters(counts,1);
        cout <<"L2 count "<<counts[0] << endl;
        PAPI_shutdown();        

}

void question2(int(*p_dummy) [n][n], int (*p_x) [n][n],int (*p_y) [n][n],int (*p_z) [n][n],int n){

	//kij
	initialize(p_z);
	dummy5_iter_mm_kij(p_z,p_x,p_y,n);	

	initialize(p_z);
	dummy6_iter_mm_kij(p_z,p_x,p_y,n);	

	initialize(p_z);
	dummy7_iter_mm_kij(p_z,p_x,p_y,n);	

	//ikj
	initialize(p_z);
	dummy3_iter_mm_ikj(p_z,p_x,p_y,n);	
	
	initialize(p_z);
	dummy4_iter_mm_ikj(p_z,p_x,p_y,n);	
	
	initialize(p_z);
	dummy7_iter_mm_ikj(p_z,p_x,p_y,n);	
}

void question1i(int(*p_dummy) [n][n], int (*p_x) [n][n],int (*p_y) [n][n],int (*p_z) [n][n],int n){

        initialize(p_z);
        int events[1];
        events[0] = PAPI_L2_TCM;
        // events[1] = PAPI_L2_TCM;
        long long counts[1];
        int retval,quiet;
        retval = PAPI_query_event(PAPI_L2_TCM);
        PAPI_start_counters(events,1);
        dummy3_iter_mm_ikj(p_z,p_x,p_y,n);

        PAPI_stop_counters(counts,1);
        cout <<"L2 count "<<counts[0] << endl;
        PAPI_shutdown();
}

void question2Exp(int(*p_dummy) [n][n], int (*p_x) [n][n],int (*p_y) [n][n],int (*p_z) [n][n],int n){

//////////////////////////////////////////////////////////////////////////////////
	initialize(p_dummy);
	iter_mm_kij(p_dummy,p_x,p_y,n);

	dummy_iter_mm_kij(p_z,p_x,p_y,n);	
	compare(p_dummy,p_z);
	cout<<"Done 1"<<endl;

	initialize(p_z);
	dummy2_iter_mm_kij(p_z,p_x,p_y,n);	
	compare(p_dummy,p_z);
	cout<<"Done 2"<<endl;

	initialize(p_z);
	dummy3_iter_mm_kij(p_z,p_x,p_y,n);	
	compare(p_dummy,p_z);
	cout<<"Done 3"<<endl;

	initialize(p_z);
	dummy4_iter_mm_kij(p_z,p_x,p_y,n);	
	compare(p_dummy,p_z);
	cout<<"Done 4"<<endl;
	
	initialize(p_z);
	dummy5_iter_mm_kij(p_z,p_x,p_y,n);	
	compare(p_dummy,p_z);
	cout<<"Done 5"<<endl;

	initialize(p_z);
	dummy6_iter_mm_kij(p_z,p_x,p_y,n);	
	compare(p_dummy,p_z);
	cout<<"Done 6"<<endl;

	initialize(p_z);
	dummy7_iter_mm_kij(p_z,p_x,p_y,n);	
	compare(p_dummy,p_z);
	cout<<"Done 7"<<endl;
///////////////////////////////////////////////////////////////////////////////////////////////

	initialize(p_dummy);
	iter_mm_ikj(p_dummy,p_x,p_y,n);	

	dummy_iter_mm_kij(p_z,p_x,p_y,n);	
	compare(p_dummy,p_z);
	cout<<"Done 1"<<endl;

	initialize(p_z);
	dummy2_iter_mm_kij(p_z,p_x,p_y,n);	
	compare(p_dummy,p_z);
	cout<<"Done 2"<<endl;

	initialize(p_z);
	dummy3_iter_mm_kij(p_z,p_x,p_y,n);	
	compare(p_dummy,p_z);
	cout<<"Done 3"<<endl;

	initialize(p_z);
	dummy4_iter_mm_kij(p_z,p_x,p_y,n);	
	compare(p_dummy,p_z);
	cout<<"Done 4"<<endl;
	
	initialize(p_z);
	dummy5_iter_mm_kij(p_z,p_x,p_y,n);	
	compare(p_dummy,p_z);
	cout<<"Done 5"<<endl;

	initialize(p_z);
	dummy6_iter_mm_kij(p_z,p_x,p_y,n);	
	compare(p_dummy,p_z);
	cout<<"Done 6"<<endl;

	initialize(p_z);
	dummy7_iter_mm_kij(p_z,p_x,p_y,n);	
	compare(p_dummy,p_z);
	cout<<"Done 7"<<endl;

}


void dummy_iter_mm_ikj(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n){
	auto start = high_resolution_clock::now();
	cilk_for(int  i=0;i<n;i++){
	cilk_for(int  k=0;k<n;k++){
	cilk_for(int  j=0;j<n;j++){
		(*z)[i][j]= (*z)[i][j] + (*x)[i][k] * (*y)[k][j];
	}
	}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout <<"Matrix kij_dummy: "<<duration.count() << endl;

}

void dummy2_iter_mm_ikj(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n){
	auto start = high_resolution_clock::now();
	cilk_for(int  i=0;i<n;i++){
	cilk_for(int  k=0;k<n;k++){
	for(int  j=0;j<n;j++){
		(*z)[i][j]= (*z)[i][j] + (*x)[i][k] * (*y)[k][j];
	}
	}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout <<"Matrix kij_dummy: "<<duration.count() << endl;

}

void dummy3_iter_mm_ikj(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n){
	auto start = high_resolution_clock::now();
	cilk_for(int  i=0;i<n;i++){
	for(int  k=0;k<n;k++){
	for(int  j=0;j<n;j++){
		(*z)[i][j]= (*z)[i][j] + (*x)[i][k] * (*y)[k][j];
	}
	}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout <<"Matrix ikj_dummy 3: "<<duration.count() << endl;

}

void dummy4_iter_mm_ikj(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n){
	auto start = high_resolution_clock::now();
	cilk_for(int  i=0;i<n;i++){
	for(int  k=0;k<n;k++){
	cilk_for(int  j=0;j<n;j++){
		(*z)[i][j]= (*z)[i][j] + (*x)[i][k] * (*y)[k][j];
	}
	}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout <<"Matrix ikj_dummy 4: "<<duration.count() << endl;

}

void dummy5_iter_mm_ikj(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n){
	auto start = high_resolution_clock::now();
	for(int  i=0;i<n;i++){
	cilk_for(int  k=0;k<n;k++){
	cilk_for(int  j=0;j<n;j++){
		(*z)[i][j]= (*z)[i][j] + (*x)[i][k] * (*y)[k][j];
	}
	}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout <<"Matrix ikj_dummy 5: "<<duration.count() << endl;

}

void dummy6_iter_mm_ikj(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n){
	auto start = high_resolution_clock::now();
	for(int  i=0;i<n;i++){
	cilk_for(int  k=0;k<n;k++){
	for(int  j=0;j<n;j++){
		(*z)[i][j]= (*z)[i][j] + (*x)[i][k] * (*y)[k][j];
	}
	}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout <<"Matrix kij_dummy: "<<duration.count() << endl;

}


void dummy7_iter_mm_ikj(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n){
	auto start = high_resolution_clock::now();
	for(int  i=0;i<n;i++){
	for(int  k=0;k<n;k++){
	cilk_for(int  j=0;j<n;j++){
		(*z)[i][j]= (*z)[i][j] + (*x)[i][k] * (*y)[k][j];
	}
	}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout <<"Matrix ikj_dummy 7: "<<duration.count() << endl;
}


//---------------------------------------------------------------------


void dummy_iter_mm_kij(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n){
	auto start = high_resolution_clock::now();
	cilk_for(int  k=0;k<n;k++){
	cilk_for(int  i=0;i<n;i++){
	cilk_for(int  j=0;j<n;j++){
		(*z)[i][j]= (*z)[i][j] + (*x)[i][k] * (*y)[k][j];
	}
	}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout <<"Matrix kij_dummy: "<<duration.count() << endl;

}

void dummy2_iter_mm_kij(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n){
	auto start = high_resolution_clock::now();
	cilk_for(int  k=0;k<n;k++){
	cilk_for(int  i=0;i<n;i++){
	for(int  j=0;j<n;j++){
		(*z)[i][j]= (*z)[i][j] + (*x)[i][k] * (*y)[k][j];
	}
	}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout <<"Matrix kij_dummy: "<<duration.count() << endl;

}

void dummy3_iter_mm_kij(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n){
	auto start = high_resolution_clock::now();
	cilk_for(int  k=0;k<n;k++){
	for(int  i=0;i<n;i++){
	for(int  j=0;j<n;j++){
		(*z)[i][j]= (*z)[i][j] + (*x)[i][k] * (*y)[k][j];
	}
	}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout <<"Matrix kij_dummy: "<<duration.count() << endl;

}

void dummy4_iter_mm_kij(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n){
	auto start = high_resolution_clock::now();
	cilk_for(int  k=0;k<n;k++){
	for(int  i=0;i<n;i++){
	cilk_for(int  j=0;j<n;j++){
		(*z)[i][j]= (*z)[i][j] + (*x)[i][k] * (*y)[k][j];
	}
	}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout <<"Matrix kij_dummy: "<<duration.count() << endl;

}

void dummy5_iter_mm_kij(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n){
	auto start = high_resolution_clock::now();
	for(int  k=0;k<n;k++){
	cilk_for(int  i=0;i<n;i++){
	cilk_for(int  j=0;j<n;j++){
		(*z)[i][j]= (*z)[i][j] + (*x)[i][k] * (*y)[k][j];
	}
	}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout <<"Matrix kij_dummy 5: "<<duration.count() << endl;

}

void dummy6_iter_mm_kij(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n){
	auto start = high_resolution_clock::now();
	for(int  k=0;k<n;k++){
	cilk_for(int  i=0;i<n;i++){
	for(int  j=0;j<n;j++){
		(*z)[i][j]= (*z)[i][j] + (*x)[i][k] * (*y)[k][j];
	}
	}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout <<"Matrix kij_dummy 6: "<<duration.count() << endl;

}


void dummy7_iter_mm_kij(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n){
	auto start = high_resolution_clock::now();
	for(int  k=0;k<n;k++){
	for(int  i=0;i<n;i++){
	cilk_for(int  j=0;j<n;j++){
		(*z)[i][j]= (*z)[i][j] + (*x)[i][k] * (*y)[k][j];
	}
	}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout <<"Matrix kij_dummy 7: "<<duration.count() << endl;
}

void compare(int (*dummy) [n][n],int (*z) [n][n]){
	for(int i=0;i<n;i++)
	for(int j=0;j<n;j++)
		if((*dummy)[i][j]!=(*z)[i][j]){
			cout<<"Matrices dont match"<<endl;
			return;
		}

}

void readInput(int  (*mat) [n][n]){
	int  min = 10;
	int  max = 100;
//	cout<<"Input Matrix";
	//Code referred from : https://stackoverflow.com/questions/5008804/generating-random-int eger-from-a-range
	for(int  i=0;i<n;i++){
	for(int  j=0;j<n;j++){
	int  randInt = min + (rand() % static_cast<int>(max - min + 1));
		(*mat)[i][j]=randInt;
	//	(*mat)[i][j]=j;
	//	cout<<" "<<(*mat)[i][j]<<" ";
	//	(*mat)[i][j]=1;
	}
	//	cout<<endl;
	}
}

void printMatrix(int (*z) [n][n]){
	
	for(int  i=0;i<n;i++){
	for(int  j=0;j<n;j++){
		cout<<" "<<(*z)[i][j]<<" ";
	}
		cout<<endl;
	}
}
void initialize(int  (*z) [n][n]){
	
	for(int  i=0;i<n;i++){
	for(int  j=0;j<n;j++){
		(*z)[i][j]=0;
	}
	}
}
void __attribute__ ((optimize("O0"))) iter_mm_ijk(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n){
	auto start = high_resolution_clock::now();
	for(int  i=0;i<n;i++){
	for(int  j=0;j<n;j++){
	for(int  k=0;k<n;k++){
	//	(*z)[i][j]= (*z)[i][j] + (*x)[i][k] * (*y)[k][j];
		(*z)[i][j]= (*z)[i][j] + (*x)[i][k] * (*y)[k][j];
	}
	}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout <<"Matrix ijk: "<<duration.count() << endl;
//	printMatrix(&op);
}

//TODO fastest
void __attribute__ ((optimize("O0"))) iter_mm_ikj(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n){
	auto start = high_resolution_clock::now();
	for(int  i=0;i<n;i++){
	for(int  k=0;k<n;k++){
	for(int  j=0;j<n;j++){
	//	(*z)[i][j]= (*z)[i][j] + (*x)[i][k] * (*y)[k][j];
		(*z)[i][j]= (*z)[i][j] + (*x)[i][k] * (*y)[k][j];
	}
	}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout <<"Matrix ikj: "<<duration.count() << endl;
//	printMatrix(&op);
}
void __attribute__ ((optimize("O0"))) iter_mm_jik(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n){
	auto start = high_resolution_clock::now();
	for(int  j=0;j<n;j++){
	for(int  i=0;i<n;i++){
	for(int  k=0;k<n;k++){
		(*z)[i][j]= (*z)[i][j] + (*x)[i][k] * (*y)[k][j];
	}
	}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout <<"Matrix jik: "<<duration.count() << endl;
//	printMatrix(z);
}
void __attribute__ ((optimize("O0"))) iter_mm_jki(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n){
	auto start = high_resolution_clock::now();
	for(int  j=0;j<n;j++){
	for(int  k=0;k<n;k++){
	for(int  i=0;i<n;i++){
		(*z)[i][j]= (*z)[i][j] + (*x)[i][k] * (*y)[k][j];
	}
	}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout <<"Matrix jki: "<<duration.count() << endl;
//	printMatrix(z);
}

//TODO fastest
void __attribute__ ((optimize("O0"))) iter_mm_kij(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n){
	auto start = high_resolution_clock::now();
	for(int  k=0;k<n;k++){
	for(int  i=0;i<n;i++){
	for(int  j=0;j<n;j++){
		(*z)[i][j]= (*z)[i][j] + (*x)[i][k] * (*y)[k][j];
	}
	}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout <<"Matrix kij: "<<duration.count() << endl;
//	printMatrix(z);
}
void __attribute__ ((optimize("O0"))) iter_mm_kji(int  (*z) [n][n],int (*x) [n][n],int (*y) [n][n],int n){
	auto start = high_resolution_clock::now();
	for(int  k=0;k<n;k++){
	for(int  j=0;j<n;j++){
	for(int  i=0;i<n;i++){
		(*z)[i][j]= (*z)[i][j] + (*x)[i][k] * (*y)[k][j];
	}
	}
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout <<"Matrix kji: "<<duration.count() << endl;
//	printMatrix(z);
}

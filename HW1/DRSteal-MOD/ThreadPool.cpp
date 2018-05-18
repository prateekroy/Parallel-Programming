//Reference : https://13abyknight.wordpress.com/2013/03/20/a-simple-thread-pool-c-implementation-on-linux/
#include <iostream>
#include <queue>
#include <pthread.h>
#include "ThreadPool.h"
 
using namespace std;
 

bool stealon = true;





// Compute a pseudorandom integer.
// Output value in range [0, 32767]
inline int fast_rand(void) {
    g_seed = (214013*g_seed+2531011);
    return (g_seed>>16)&0x43;
}


void thread_pool::shareon(){
    stealon = true;
}

thread_pool::thread_pool(){
    numOfThreads = 2;
    for (int i = 0; i < numOfThreads; ++i)
    {
        threads.push_back(new worker_thread(i, this));
    }
}

thread_pool::thread_pool(int num){
    numOfThreads = num;
    for (int i = 0; i < numOfThreads; ++i)
    {
        threads.push_back(new worker_thread(i, this));
    }
}

void thread_pool::terminate(){
    for (int i = 0; i < numOfThreads; ++i)
    {
        threads[i]->terminate();
    }
}

void worker_thread::terminate(){
    pthread_cancel(thread);
}


void thread_pool::start()
{
    for(int i = 0; i < numOfThreads; i++)
    {
        threads[i]->start();
    }
}

void thread_pool::assignJob(job* _job_, int threadid)
{
    // static int threadid = 0;
    //some huerestic now trying round robin
    threads[threadid]->assignJob(_job_);
    if (threadid >= numOfThreads)
    {
        threadid = 0;
    }
}

bool thread_pool::empty(){
    for (int i = 0; i < numOfThreads; ++i)
    {
        pthread_mutex_lock(&threads[i]->jobDequeue_lock);
        // cout << "Checking for " << i<< endl;
        if (!threads[i]->jobDequeue.empty())
        {
            pthread_mutex_unlock(&threads[i]->jobDequeue_lock);
            // cout << "Queue " << i << " is not empty\n";
            return false;
        }
        pthread_mutex_unlock(&threads[i]->jobDequeue_lock);
    }    
    return true;
}

#include <random>
int thread_pool::getRandomNumber(){

    return fast_rand();

    std::random_device rd; // obtain a random number from hardware
    std::mt19937 eng(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, numOfThreads-1); // define the range
    return distr(eng);
}

job* thread_pool::StealTask(worker_thread* p, int mytid){

    if (!stealon)
    {
        return NULL;
    }
    //Implement your steal algo
    //dont look for own queue here else u will get a deadlock

    int i = getRandomNumber();
    while(i >= numOfThreads){
        // cout << i << endl;
        i = getRandomNumber();
    }

    int j = getRandomNumber();
    while(j >= numOfThreads || j == i){
        // cout << i << endl;
        j = getRandomNumber();
    }    
    // cout << i << endl;
    

    // for (int i = 0; i < numOfThreads; ++i)
    // {
        if (i == mytid || j == mytid)
        {
            // cout << "Do check for " << i << endl;
            return NULL;
        }
        
        int choice = i;
        if (threads[i]->jobDequeue.size() > threads[j]->jobDequeue.size())
        {
            choice = i;
        }

        if (!threads[choice]->jobDequeue.empty())
        {
            // cout << "--------------Thread " << p->tid << " trying to steal Task from------" << i << endl;
            pthread_mutex_lock(&threads[choice]->jobDequeue_lock);
            job* jo = NULL;
            if (!threads[choice]->jobDequeue.empty())
            {
                jo = threads[choice]->jobDequeue.back();
                threads[choice]->jobDequeue.pop_back();
            }
            pthread_mutex_unlock(&threads[choice]->jobDequeue_lock);
            // cout << "--------------Thread " << p->tid << " steal Task from " << i << " success------ jobid " << j->jobID << endl;
            return jo;
        }
        // pthread_mutex_unlock(&threads[i]->jobDequeue_lock);
    // }
    return NULL;
}

thread_pool::thread_pool(const thread_pool &tp){

    cout << "Copy constr";
    for (int i = 0; i < tp.threads.size(); ++i)
    {
        threads.push_back(tp.threads[i]);
    }
    numOfThreads = tp.numOfThreads;
}


worker_thread::worker_thread(int _tid, thread_pool* _parentPool){
    // :tid(_tid), parentPool(_parentPool)

    tid = _tid;
    parentPool = _parentPool;
    
    // thread = new pthread_t[1];
    // jobDequeue_lock = PTHREAD_MUTEX_INITIALIZER;
    // jobDequeue_cond = PTHREAD_COND_INITIALIZER;
}

worker_thread::~worker_thread(){
    // delete thread;
    // thread = NULL;
}

void worker_thread::assignJob(job *_job_){
    pthread_mutex_lock(&jobDequeue_lock);
    jobDequeue.push_back(_job_);
    pthread_mutex_unlock(&jobDequeue_lock);
    // pthread_cond_signal(&jobDequeue_cond);   
}

bool worker_thread::loadJob(job*& _job_, worker_thread* p)
{
    pthread_mutex_lock(&jobDequeue_lock);

    // while(jobDequeue.empty() && (_job_ = StealTask(p)) && !_job_){
    //     pthread_cond_wait(&jobDequeue_cond, &jobDequeue_lock);
    // }

    // while(jobDequeue.empty())
    //     pthread_cond_wait(&jobDequeue_cond, &jobDequeue_lock);

    //first look for own job queue
    if (!jobDequeue.empty())
    {
        _job_ = jobDequeue.front();
        jobDequeue.pop_front();
        pthread_mutex_unlock(&jobDequeue_lock);
        return true;
    }
    
    if((_job_ = StealTask(p)) && _job_){
        pthread_mutex_unlock(&jobDequeue_lock);
        return true;
    }

    //if own job queue is empty look for other threads job queue and extract from back
    // _job_ = StealTask(p);
    pthread_mutex_unlock(&jobDequeue_lock);
    return true;
}

void worker_thread::start(){

    pthread_create(&thread, NULL, &worker_thread::threadExecute, (void *)this);
    // cout << "Thread:" << tid << " is alive now!\n";

}

void *worker_thread::threadExecute(void *param)
{
    worker_thread *p = (worker_thread *)param;
    job *oneJob = NULL;
    while(p->loadJob(oneJob, p))
    {
        if(oneJob)
            oneJob->working((void*)&p->tid);
        // delete oneJob;
        oneJob = NULL;
    }
    return NULL;
}

job* worker_thread::StealTask(worker_thread* p){
    return parentPool->StealTask(p, tid);
}
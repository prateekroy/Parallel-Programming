//Reference: https://13abyknight.wordpress.com/2013/03/20/a-simple-thread-pool-c-implementation-on-linux/
#ifndef __THREADPOOL_H_
#define __THREADPOOL_H_

#include <iostream>
#include <queue>
#include <pthread.h>

using namespace std;

static unsigned int g_seed;
// Used to seed the generator.           
inline void fast_srand(int seed) {
    g_seed = seed;
}

class job
{
public:
    job(int id) : jobID(id) {}
    // virtual ~job(){}
    //will be overrided by specific JOB
    void virtual working(void* param) = 0;
    int jobID;

private:
    static int finished_jobs;
    static pthread_mutex_t jobLock;
};

class thread_pool;

class worker_thread{
    pthread_t thread;
    bool loadJob(job*& _job_, worker_thread* p);
    static void *threadExecute(void *);
    job* StealTask(worker_thread* p);
    thread_pool* parentPool;

public:
    worker_thread(int _tid, thread_pool* _parentPool);
    ~worker_thread();
    void assignJob(job *_job_);
    void start();
    void terminate();
    pthread_mutex_t jobDequeue_lock;
    pthread_cond_t jobDequeue_cond;
    deque<job*> jobDequeue;
    int tid;
};
 
//user needs to use this class
class thread_pool
{
public:
    thread_pool();
    thread_pool(int num);
    thread_pool(const thread_pool &tp);
    virtual ~thread_pool() {};
    void assignJob(job *_job_, int tid);   //some huerestic to assign job to which thread round robin
    void start();
    void terminate();
    void shareon();
    job* StealTask(worker_thread* p, int mytid);
    bool empty();
    int getRandomNumber();

private:
    vector<worker_thread*> threads;
    int numOfThreads;
};




#endif
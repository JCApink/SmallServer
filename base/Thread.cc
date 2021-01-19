#include <unistd.h>
#include <sys/syscall.h>
#include <assert.h>
#include <pthread.h>
#include "Thread.h"
#include "ThreadData.h"
#include <iostream>


namespace CurrentThread{
    
__thread int t_cacheTid = 0;
__thread char t_tidString[32];
__thread int t_tidStringLength = 6;
__thread const char* t_threadName = "default";

}

pid_t gettid(){
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

void CurrentThread::cacheTid(){
    if(t_cacheTid == 0){
        t_cacheTid = gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d", t_cacheTid);
    }
}

struct ThreadData{
    typedef Thread::ThreadFunc Threadfunc;
    Threadfunc Func;
    std::string Name;
    pid_t* Tid;
    CountDownLatch* Countdown;

    ThreadData(const Threadfunc& func, const std::string& name, pid_t* tid, CountDownLatch* countdown)
        :Func(func)
        ,Name(name)
        ,Tid(tid)
        ,Countdown(countdown){

        }

    void runInThread(){
        *Tid = CurrentThread::tid();
        Tid = NULL;
        Countdown->countDown();
        Countdown = NULL;
        CurrentThread::t_threadName = Name.empty() ? "Thread" : Name.c_str();
        Func();
        CurrentThread::t_threadName = "finish";
    }
};

void* startThread(void* obj){
    ThreadData* data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return NULL;
}

Thread::Thread(const ThreadFunc& func, const std::string& name)
    :start_(false)
    ,join_(false)
    ,pthreadId(0)
    ,p_name(name)
    ,Func(func)
    ,tid(0)
    ,countdown(1){
        setDefaultName();
}
Thread::~Thread(){
    if(start_ && !join_){
        pthread_detach(pthreadId);
    }
}

void Thread::setDefaultName(){
    if(p_name.empty()){
        char buf[32];
        snprintf(buf, sizeof buf, "Thread");
        p_name = buf;
    }
}
void Thread::start(){
    assert(!start_);
    start_ = true;
    ThreadData *data = new ThreadData(Func, p_name, &tid, &countdown);
    if(pthread_create(&pthreadId, NULL, &startThread, data)){
        start_ = false;
        delete data;
    }
    else{

        countdown.wait();
        
        assert(tid > 0);
    }
    
}

int Thread::join(){
    assert(start_);
    assert(!join_);
    join_ = true;
    return pthread_join(pthreadId, NULL);
}


bool Thread::getStart() const{
    return start_;
}
pid_t Thread::gettid() const{
    return tid;
}

const std::string& Thread::getname() const{
    return p_name;
}


    


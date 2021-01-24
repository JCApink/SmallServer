#pragma once
#include <memory>
#include <pthread.h>
#include "noncopyable.h"
#include "MutexLock.h"


class Condition : public Noncopyable{
public:
    typedef std::shared_ptr<Condition> ptr;

    Condition(MutexLock& mutex);
    ~Condition();

    void wait();
    bool waitforsecond(int seconds);

    void notify();
    void notifyAll();


private:
    MutexLock& Mutex;
    pthread_cond_t cond;
    
};




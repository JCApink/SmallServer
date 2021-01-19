#pragma once
#include <memory>
#include <pthread.h>
#include "noncopyable.h"



class MutexLock: public Noncopyable{
public:
    typedef std::shared_ptr<MutexLock> ptr;

    explicit MutexLock();
    ~MutexLock();

    void lock();
    void unlock();

    pthread_mutex_t* getMutex();

private:
    pthread_mutex_t mutex;

};

class MutexLockGuard : public Noncopyable{
public:
    explicit MutexLockGuard(MutexLock& mutex);
    
    ~MutexLockGuard();

private:
    MutexLock& Mutex;
};


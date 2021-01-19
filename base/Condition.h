#pragma once
#include <memory>
#include <pthread.h>
#include "noncopyable.h"
#include "MutexLock.h"
/*
    条件变量锁 包含基本的条件变量锁的操作
    线程等待：void wait();
    线程等待 包含秒数：bool waitforsecond(int seconds);

    单个线程唤醒：void notify();
    全部线程唤醒：void notifyAll();
*/

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




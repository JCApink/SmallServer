#pragma once
#include <memory>
#include "noncopyable.h"
#include "Condition.h"
#include "MutexLock.h"


class CountDownLatch : public Noncopyable{
public:
    typedef std::shared_ptr<CountDownLatch> ptr;

    CountDownLatch(int c);
    ~CountDownLatch();

    void wait();
    void countDown();


private:

    mutable MutexLock Mutex;
    Condition condition;
    int count;
};





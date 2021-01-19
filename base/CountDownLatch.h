#pragma once
#include <memory>
#include "noncopyable.h"
#include "Condition.h"
#include "MutexLock.h"
/*
    倒计时（CountDownLatch）它有2个应用：
1：主线程发起多个子进程，等这些子线程各自都完成一定的任务之后，主线程才继续执行。通常用于主线程等待多个子线程完成初始化。
2：主线程发起多个子线程，子线程都等待主线程，主线程完成其他一些任务之后通知所有子线程开始执行。通常用于多个子线程等待主线程发出“起跑”命令。

*/

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





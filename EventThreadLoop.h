#pragma once
#include <memory>
#include "EventLoop.h"
#include "base/Condition.h"
#include "base/MutexLock.h"
#include "base/Thread.h"
#include "base/noncopyable.h"


class EventThreadLoop{
public:
    typedef std::shared_ptr<EventThreadLoop> ptr;
    EventThreadLoop();
    ~EventThreadLoop();

    EventLoop* start();
    void ThreadFunc();
private:
    EventLoop* eventloops;
    bool quit;
    Thread thread;
    MutexLock mutex;
    Condition cond;
};


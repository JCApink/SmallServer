#ifndef EVENTTHREADLOOP_H
#define EVENTTHREADLOOP_H
#include <memory>
#include "EventLoop.h"
#include "base/Thread.h"
#include "base/Condition.h"


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
#endif

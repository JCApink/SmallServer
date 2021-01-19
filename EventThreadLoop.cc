#include "EventThreadLoop.h"
#include <functional>
#include <assert.h>



EventThreadLoop::EventThreadLoop()
    :eventloops(nullptr)
    ,quit(false)
    ,thread(std::bind(&EventThreadLoop::ThreadFunc, this), "EventLoopThread")
    ,mutex()
    ,cond(mutex){

}

EventThreadLoop::~EventThreadLoop(){

}

EventLoop* EventThreadLoop::start(){
    assert(!thread.getStart());
    thread.start();
    {
        MutexLockGuard lock(mutex);
        while(eventloops == nullptr){
            cond.wait();
        }
    }
    return eventloops;
}

void EventThreadLoop::ThreadFunc(){
    EventLoop eventloop;
    {
        MutexLockGuard lock(mutex);
        eventloops = &eventloop;
        cond.notify();
    }
    eventloop.loop();
    eventloops = nullptr;
}

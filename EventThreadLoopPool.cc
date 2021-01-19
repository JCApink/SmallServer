#include "EventThreadLoopPool.h"
#include "EventThreadLoop.h"
#include <assert.h>


    
EventThreadLoopPool::EventThreadLoopPool(EventLoop* baseeventloop, int num)
    :BaseEventLoop(baseeventloop)
    ,start(false)
    ,ThreadNum(num)
    ,next(0){
    assert(ThreadNum >= 0);
}

void EventThreadLoopPool::startbaseloop(){
    //BaseEventLoop->assertInCurrentThread();
    start = true;
    for(int i = 0; i < ThreadNum; ++i){
        EventThreadLoop::ptr tmp(new EventThreadLoop());
        eventloops.push_back(tmp->start());
    }
    //std::cout << "EventThreadLoopPool:22 size: " << eventloops.size() << std::endl;
}

EventLoop* EventThreadLoopPool::getnextloop(){
    BaseEventLoop->assertInCurrentThread();
    assert(start);
    EventLoop* loop;
    if(!eventloops.empty()){
        loop = eventloops[next];
        next = (next + 1) % ThreadNum;
        //std::cout <<"EventThreadLoopPool:31 next: " << next << std::endl;
    }
    return loop;
}

EventThreadLoopPool::~EventThreadLoopPool(){
    
}

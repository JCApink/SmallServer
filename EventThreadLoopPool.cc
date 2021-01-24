#include "EventThreadLoopPool.h"

EventThreadLoopPool::EventThreadLoopPool(EventLoop* baseeventloop, int num)
    :BaseEventLoop(baseeventloop)
    ,start(false)
    ,ThreadNum(num)
    ,next(0){
    assert(ThreadNum >= 0);
}

void EventThreadLoopPool::startbaseloop(){
    start = true;
    for(int i = 0; i < ThreadNum; ++i){
        EventThreadLoop::ptr tmp(new EventThreadLoop());
        eventloops.push_back(tmp->start());
    }
}

EventLoop* EventThreadLoopPool::getnextloop(){
    BaseEventLoop->assertInCurrentThread();
    assert(start);
    EventLoop* loop;
    if(!eventloops.empty()){
        loop = eventloops[next];
        next = (next + 1) % ThreadNum;
    }
    return loop;
}

EventThreadLoopPool::~EventThreadLoopPool(){
    
}

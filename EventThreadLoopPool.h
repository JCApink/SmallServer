#ifndef EVENTTHREADLOOPPOOL_H
#define EVENTTHREADLOOPPOOL_H
#include "EventLoop.h"
#include <memory>
#include <vector>



class EventThreadLoopPool{
public:
    typedef std::shared_ptr<EventThreadLoopPool> ptr;
    EventThreadLoopPool(EventLoop* baseeventloop, int num);
    ~EventThreadLoopPool();

    void startbaseloop();
    EventLoop* getnextloop();
private:
    EventLoop* BaseEventLoop;
    bool start;
    int ThreadNum;
    int next;
    std::vector<EventLoop*> eventloops;

};
#endif

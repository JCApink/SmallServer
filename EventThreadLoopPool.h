#pragma once

#include "EventLoop.h"
#include "EventThreadLoop.h"

#include <memory>
#include <vector>
#include "base/Log.h"
#include "base/noncopyable.h"


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


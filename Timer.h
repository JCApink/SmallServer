#pragma once
#include <iostream>
#include <unistd.h>
#include <deque>
#include <memory>
#include <queue>
#include "HttpData.h"
#include "base/MutexLock.h"
#include "base/noncopyable.h"

class HttpData;
class TimerNode{
public:
    
    typedef std::shared_ptr<TimerNode> ptr;
    TimerNode(std::shared_ptr<HttpData> RequestData, int timeout);
    ~TimerNode();
    TimerNode(TimerNode &tn);
    void update(int timeout);
    bool isValid();
    void clearReq();
    void setDeleted();
    bool isDeleted() const;
    size_t getExpTime() const;

private:
    bool deleted_;
    size_t expiredTime_;
    std::shared_ptr<HttpData> SPHttpData;
};

struct TimerCmp {
    bool operator()(std::shared_ptr<TimerNode> &a, std::shared_ptr<TimerNode> &b) const {
        return a->getExpTime() > b->getExpTime();
    }
};

class TimerManager {
public:
    
    TimerManager();
    ~TimerManager();
    void addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout);
    void handleExpiredEvent();

private:
    typedef std::shared_ptr<TimerNode> SPTimerNode;
    std::priority_queue<SPTimerNode, std::deque<SPTimerNode>, TimerCmp>
        timerNodeQueue;
};





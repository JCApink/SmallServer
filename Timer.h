#ifndef TIMER_H
#define TIMER_H


#include <memory>
#include <queue>
#include "Channel.h"
#include "HttpData.h"


class HttpData;
class Epoll;

class Timer{
public:
    
    typedef std::shared_ptr<Timer> ptr;
    Timer(std::shared_ptr<HttpData> RequestData, int timeout);
    ~Timer();
    Timer(Timer &tn);
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
    bool operator()(Timer::ptr &a, Timer::ptr &b) const {
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
    typedef Timer::ptr SPTimerNode;
    std::priority_queue<SPTimerNode, std::deque<SPTimerNode>, TimerCmp>
        timerNodeQueue;
};
#endif

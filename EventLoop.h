#ifndef EVENTLOOP_H
#define EVENTLOOP_H
#include <memory>

#include "base/Thread.h"
#include "base/ThreadData.h"
#include "base/MutexLock.h"
#include "Channel.h"
#include "Epoll.h"
class Channel;

typedef std::shared_ptr<Channel> SP_Channel;   
typedef std::function<void()> CallBack;

class Epoll;
class EventLoop{
public:
    typedef std::shared_ptr<EventLoop> ptr;
    EventLoop();
    ~EventLoop();

    void wakeupwrite();
    void wakeupread();
    void handleConn();

    void runInLoop(CallBack function);
    void queueInLoop(CallBack function);
    void DoingFunctions();
    
    void loop();
    bool InCurrentThread();
    void assertInCurrentThread();
    
    SP_Channel getChannel();
    Epoll* getepoll();
private:
    bool looping;
    bool quit;
    bool doingfunctions;
    int wakeupfd;
    const pid_t threadId;
    Epoll* epoll;
    SP_Channel wakeupchannel;
    std::vector<std::function<void()>> functions; 
    MutexLock mutex;
};

#endif

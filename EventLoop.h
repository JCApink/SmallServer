#pragma once
#include <memory>



#include <functional>
#include <memory>
#include <vector>
#include "Channel.h"
#include "Epoll.h"
#include "Util.h"


#include "base/Thread.h"



#include "base/ThreadData.h"
#include "base/MutexLock.h"
#include "base/Log.h"

//class Channel;

//typedef std::shared_ptr<Channel> SP_Channel;   
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
    std::shared_ptr<Epoll> getepoll();
private:
    bool looping;
    bool quit;
    bool doingfunctions;
    int wakeupfd;
    const pid_t threadId;
    std::shared_ptr<Epoll> epoll;
    std::shared_ptr<Channel> wakeupchannel;
    std::vector<std::function<void()>> functions; 
    MutexLock mutex;
};



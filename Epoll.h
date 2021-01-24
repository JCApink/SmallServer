#pragma once
#include <sys/epoll.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include "Channel.h"
#include "HttpData.h"
#include "Timer.h"
//#include "Timer.h"


//#include "HttpData.h"
#define MAXFD 10000
#define EPOLLWAIT_TIME 10000

//class TimerManager;
class Epoll{
public:
    typedef std::shared_ptr<Epoll> ptr;
    Epoll();
    ~Epoll();

    void epoll_add(SP_Channel request, int timeout = 0);
    void epoll_mod(SP_Channel request, int timeout = 0);
    void epoll_del(SP_Channel request, int timeout = 0);

    std::vector<std::shared_ptr<Channel>> epoll();

    std::vector<SP_Channel> getevents(int event_count);

    int getEpollFd();
    void timesolve();
    void addtime(SP_Channel channel, int timeout);

private:
    int epollFd;
    std::vector<epoll_event> events;
    TimerManager timerma;
    std::shared_ptr<Channel> FdChannel[MAXFD];
    std::shared_ptr<HttpData> FdHttp[MAXFD];
};


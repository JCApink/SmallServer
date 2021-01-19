#pragma once
#include <memory>
#include <vector>
#include <sys/epoll.h>
#include "Timer.h"
#include "Channel.h"
#define MAXFD 10000
#define EPOLLWAIT_TIME 10000


class TimerManager;
class HttpData;
class Channel;
typedef std::shared_ptr<Channel> SP_Channel;
typedef std::shared_ptr<HttpData> HttpDataptr;

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
    std::shared_ptr<TimerManager> timerma;
    std::shared_ptr<Channel> FdChannel[MAXFD];
    HttpDataptr FdHttp[MAXFD];
};


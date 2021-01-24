#pragma once
#include <sys/epoll.h>
#include <sys/epoll.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include "Timer.h"

class HttpData;
class EventLoop;
class Channel{
public:
    typedef std::function<void()> CallBack;
    Channel(EventLoop* eventloop);
    Channel(EventLoop* eventloop, int fd);
    ~Channel();

    /*不同的事件的处理函数*/
    void setReadHandle(CallBack &&readhandle);
    void setWriteHandle(CallBack &&writehandle);
    void setErrorHandle(CallBack &&errorhandle);
    void setConnHandle(CallBack &&connhandle);

    void doReadHandle();
    void doWriteHandle();
    void doErrorHandle();
    void doConnHandle();

    /*文件描述符*/
    void setFd(int fd);
    int getFd();

    void setEvents(uint32_t event);
    void setRevents(uint32_t revent);
    uint32_t& getRevent();
    uint32_t& getEvent();

    void setHttpData(std::shared_ptr<HttpData> h);
    std::shared_ptr<HttpData> getHttpData();

    void doAllRevents();
    
private:
    EventLoop *eventloop;
    int Fd;
    uint32_t Events;
    uint32_t Revents;
    std::weak_ptr<HttpData> httpdata;
    CallBack ReadHandle;
    CallBack WriteHandle;
    CallBack ErrorHandle;
    CallBack ConnHandle;
};
typedef std::shared_ptr<Channel> SP_Channel;


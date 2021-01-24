#pragma once

#include <memory>
#include "Channel.h"
#include "EventLoop.h"
#include "EventThreadLoopPool.h"
//#include "Channel.h"
//#include "Util.h"
#define MAX_FD 10000
class TcpServer{
public:
    typedef std::shared_ptr<TcpServer> ptr;
    TcpServer(EventLoop* eventloop, int num, int port);
    ~TcpServer();
    void start();
    void newConnect();
    void handleConn();
private:
    EventLoop* eventloop;
    EventThreadLoopPool::ptr threadpool;
    int ThreadNum;
    bool starting;
    std::shared_ptr<Channel> channel;
    int port;
    int ListenFd;

};

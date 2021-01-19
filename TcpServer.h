#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <memory>
#include "EventLoop.h"
#include "EventThreadLoopPool.h"
#include "Channel.h"
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
    SP_Channel channel;
    int port;
    int ListenFd;

};

#endif
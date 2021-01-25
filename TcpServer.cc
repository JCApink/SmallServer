#include "TcpServer.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <functional>
#include "Util.h"
#include "base/Log.h"



int newsocket(int port){
    const char* ip = "127.0.0.1";
    struct sockaddr_in address;
    bzero( &address, sizeof( address ) );
    address.sin_family = AF_INET;
    inet_pton( AF_INET, ip, &address.sin_addr );
    address.sin_port = htons( port );
    
    int listenfd = socket( PF_INET, SOCK_STREAM, 0 );
    assert( listenfd >= 0 );
    
    int ret = bind( listenfd, ( struct sockaddr* )&address, sizeof( address ) );
    assert( ret != -1 );
    ret = listen(listenfd, 2048);
    assert(ret != -1);
    return listenfd;
}

void signal_for_sigpipe() {
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if (sigaction(SIGPIPE, &sa, NULL)){
            return;
    }
}
TcpServer::TcpServer(EventLoop* e, int num, int p)
        :eventloop(e)
        ,threadpool(new EventThreadLoopPool(e, num))
        ,ThreadNum(num)
        ,starting(false)
        ,port(port)
        ,ListenFd(newsocket(p))
        ,channel(new Channel(e)){
    int n = setNonBlock(ListenFd);
    assert(n != -1);
    channel->setFd(ListenFd);
    signal_for_sigpipe();
}

TcpServer::~TcpServer(){

}

void TcpServer::start(){
    threadpool->startbaseloop();
    channel->setEvents(EPOLLIN | EPOLLET);
    channel->setReadHandle(std::bind(&TcpServer::newConnect, this));
    channel->setConnHandle(std::bind(&TcpServer::handleConn, this));
    eventloop->getepoll()->epoll_add(channel, 0);
    starting = true;
}

void TcpServer::newConnect(){
    struct sockaddr_in client_address;
    socklen_t client_addrlength = sizeof( client_address );
    int accept_fd;
    while((accept_fd = accept( ListenFd, ( struct sockaddr* )&client_address, &client_addrlength )) > 0){
        if(accept_fd > MAX_FD){
            close(accept_fd);
        }
        EventLoop* loop = threadpool->getnextloop();
        int ret = setNonBlock(accept_fd);
        assert(ret != -1);
        //std::cout << "accept_fd" << accept_fd << std::endl;
        std::shared_ptr<HttpData> httpdata(new HttpData(loop, accept_fd));
        httpdata->getChannel()->setHttpData(httpdata);
        //下面这里是处理主线程与子线程的交互问题，因为loop获取的是子线程的loop但是执行时主线程执行，所以这里必须将事件交给
        //子线程进行处理。
        loop->queueInLoop(std::bind(&HttpData::newConnectEvent, httpdata));
    }
    channel->setEvents(EPOLLIN | EPOLLET);
}

void TcpServer::handleConn(){
    eventloop->getepoll()->epoll_mod(channel, 0);
}


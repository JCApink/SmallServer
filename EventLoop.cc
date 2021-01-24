#include "EventLoop.h"
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <iostream>
#include "Util.h"
#include "base/Log.h"

int creatEventfd() {
    int fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    assert(fd != -1);
    return fd;
}

EventLoop::EventLoop()
        :looping(false)
        ,quit(true)
        ,doingfunctions(false)
        ,wakeupfd(creatEventfd())
        ,threadId(CurrentThread::tid())
        ,epoll(new Epoll())
        ,wakeupchannel(new Channel(this, wakeupfd)){
    wakeupchannel->setEvents(EPOLLIN | EPOLLET);
    wakeupchannel->setReadHandle(std::bind(&EventLoop::wakeupread, this));
    wakeupchannel->setConnHandle(std::bind(&EventLoop::handleConn, this));
    epoll->epoll_add(wakeupchannel, 0);
}

EventLoop::~EventLoop(){
    close(wakeupfd);
}

void EventLoop::wakeupwrite(){
    int one  = 1;
    size_t n = writen(wakeupfd, (char*)&one, sizeof(one));
    if(n != sizeof(one)){
        LOG << "WARN: WRITE IN wakeup NOT sizeof(one) BITS" ;
    }
}

void EventLoop::wakeupread(){
    int one;
    size_t n = readn(wakeupfd, &one ,sizeof(one));
    if(n != sizeof(one)){
        LOG << "WARN: READ IN wakeup NOT sizeof(one) BITS" ;
    }
    wakeupchannel->setEvents(EPOLLIN | EPOLLET);
}

void EventLoop::handleConn(){
    epoll->epoll_mod(wakeupchannel, 0);
}

void EventLoop::runInLoop(CallBack function){
    if(InCurrentThread()) {
        function();
    } else {
        queueInLoop(function);
    }
}

void EventLoop::queueInLoop(CallBack function){
    {
        MutexLockGuard lock(mutex);
        LOG << "INFO: QUEUEINLOOP";
        functions.emplace_back(std::move(function));
    }
    if(!InCurrentThread() || doingfunctions){
        wakeupwrite();
    }
}

void EventLoop::DoingFunctions(){
    std::vector<std::function<void()>> tmp;
    doingfunctions = true;
    {
        MutexLockGuard lock(mutex);
        tmp.swap(functions);
    }
    for(int i = 0; i < tmp.size(); ++i){
        tmp[i]();
    }
    doingfunctions = false;
}

void EventLoop::loop(){
    assert(!looping);
    assertInCurrentThread();
    looping = true;
    quit = false;
    std::vector<std::shared_ptr<Channel>> channels;
    //std::cout << "EventLoop:102 Fd: " << wakeupfd << std::endl;
    while(!quit){
        channels.clear();
        channels = epoll->epoll();
        for(auto& channel: channels){
            channel->doAllRevents();
        }
        DoingFunctions();
        epoll->timesolve();
        //超时处理
    }
    looping = false;
}

bool EventLoop::InCurrentThread(){
    //std::cout << "threadId:" << threadId << "Current:" << CurrentThread::tid() << std::endl;
    return threadId == CurrentThread::tid();
}

void EventLoop::assertInCurrentThread(){
    assert(InCurrentThread());
}

SP_Channel EventLoop::getChannel(){
    return wakeupchannel;
}

std::shared_ptr<Epoll> EventLoop::getepoll(){
    return epoll;
}


#include "Channel.h"

#include <unistd.h>
#include <cstdlib>
#include <iostream>

#include <queue>

#include "Epoll.h"
#include "EventLoop.h"
#include "Util.h"


Channel::Channel(EventLoop* e)
    :eventloop(e)
    ,Fd(0)
    ,Events(0)
    ,Revents(0){
        
}

Channel::Channel(EventLoop* e, int fd)
    :eventloop(e)
    ,Fd(fd)
    ,Events(0)
    ,Revents(0){
        //std::cout << "Channel:17 Fd: " << Fd << std::endl;
}

Channel::~Channel(){

}

void Channel::setReadHandle(CallBack &&readhandle){
    ReadHandle = std::move(readhandle);
}

void Channel::setWriteHandle(CallBack &&writehandle){
    WriteHandle = std::move(writehandle);
}

void Channel::setErrorHandle(CallBack &&errorhandle){
    ErrorHandle = std::move(errorhandle);
}

void Channel::setConnHandle(CallBack &&connhandle){
    ConnHandle = std::move(connhandle);
}

void Channel::doReadHandle(){
    if(ReadHandle){
        ReadHandle();
    }
}
void Channel::doWriteHandle(){
    if(WriteHandle){
        WriteHandle();
    }
}

void Channel::doErrorHandle(){
    if(ErrorHandle){
        ErrorHandle();
    }
}

void Channel::doConnHandle(){
    if(ConnHandle){
        ConnHandle();
    }
}

void Channel::setFd(int fd){
    Fd = fd;
}

int Channel::getFd(){
    return Fd;
}

void Channel::setEvents(uint32_t event){
    Events = event;
}

void Channel::setRevents(uint32_t revent){
    Revents = revent;
}

uint32_t& Channel::getRevent(){
    return Revents;
}

uint32_t& Channel::getEvent(){
    return Events;
}

void Channel::setHttpData(std::shared_ptr<HttpData> h){
    httpdata = h;
}

std::shared_ptr<HttpData> Channel::getHttpData(){
    std::shared_ptr<HttpData> ret(httpdata.lock());
    return ret;
}

void Channel::doAllRevents(){
    if((Revents & EPOLLHUP) && !(Revents & EPOLLIN)){
        return;
    }
    if(Revents & EPOLLERR){
        if(ErrorHandle){
            ErrorHandle();
        }
        return;
    }
    if(Revents & (EPOLLIN | EPOLLPRI | EPOLLRDHUP) ){
        ReadHandle();
    }
    if(Revents & EPOLLOUT){
        WriteHandle();
    }
    ConnHandle();
}


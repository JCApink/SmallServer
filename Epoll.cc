#include "Epoll.h"
#include <iostream>
#include <stdio.h>

#include "base/Log.h"

static int EPOLLFD = MAXFD;
static int EPOLLWAIT = EPOLLWAIT_TIME;
static int n = 0;
Epoll::Epoll()
    : epollFd(epoll_create1(EPOLL_CLOEXEC))
    , events(MAXFD){
    assert(epollFd > 0);
}

Epoll::~Epoll(){
    close(epollFd);
}

void Epoll::epoll_add(std::shared_ptr<Channel> request, int timeout){
    int fd = request->getFd();
    if (timeout > 0){
        FdHttp[fd] = request->getHttpData();
    }
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvent();
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event) != 0){
        std::cout  << "ERROR: ADD EVENT ERROR" << std::endl;
    }
    FdChannel[fd] = request;
}

void Epoll::epoll_mod(std::shared_ptr<Channel> request, int timeout){
    if (timeout > 0){
    }
    int fd = request->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvent();
    if (epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &event)){
        LOG << "MOD EVENT ERROR";
        FdChannel[fd].reset();
    }
}

void Epoll::epoll_del(SP_Channel request, int timeout){
    int fd = request->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvent();
    if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, &event)){
        LOG << "DELETE EVENT ERROR";
    }   
    FdChannel[fd].reset();
    FdHttp[fd].reset();
}

std::vector<std::shared_ptr<Channel>> Epoll::epoll(){
    while(true){
        int event_count = epoll_wait(epollFd, &*events.begin(), events.size(), EPOLLWAIT);
        if (event_count < 0){
            assert(event_count < 0);
            LOG << "EPOLL WAIT ERROR";
        }
        std::vector<std::shared_ptr<Channel>> channels = getevents(event_count);
        if (event_count > 0){
            return channels;
        }
    }
}

std::vector<std::shared_ptr<Channel>> Epoll::getevents(int num){
    std::vector<std::shared_ptr<Channel>> channels;
    for (int i = 0; i < num; ++i){
        int fd = events[i].data.fd;
        std::shared_ptr<Channel> channel_fd = FdChannel[fd];
        if (channel_fd){
            channel_fd->setRevents(events[i].events);
            channel_fd->setEvents((uint32_t) 0);
            channels.push_back(channel_fd);
        }
        else{
            LOG << "EVENT ERROR";
        }
    }
    return channels;
}

int Epoll::getEpollFd(){
    return epollFd;
}

void Epoll::timesolve() {
    timerma->handleExpiredEvent();
}

void Epoll::addtime(SP_Channel channel, int timeout) {
    std::shared_ptr<HttpData> http = channel->getHttpData();
    if(http) {
        timerma->addTimer(http, timeout);
    } else {
        LOG << "add time fault";
    }
}

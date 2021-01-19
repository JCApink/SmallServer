#pragma once
#include <memory>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <fcntl.h>
#include <iostream>
#include <unordered_map>
#include "base/FixBuffer.h"
#include "EventLoop.h"
#include "Timer.h"



#define BUFFER_SIZE 4096
#define TIMEOUT 2000
#define LONGTIMEOUT 2000 * 5 * 1000
class Channel;
class EventLoop;
class Timer;

typedef std::shared_ptr<Channel> SP_Channel;

enum CHECK_STATE { 
    CHECK_STATE_REQUESTLINE = 0, 
    CHECK_STATE_HEADER, 
    CHECK_STATE_CONTENT,
    CHECK_STATE_KEEP
};

enum LINE_STATUS { 
    LINE_OK = 0, 
    LINE_BAD, 
    LINE_OPEN
};

enum HTTP_CODE { 
    NO_REQUEST, 
    GET_REQUEST, 
    BAD_REQUEST, 
    FORBIDDEN_REQUEST, 
    INTERNAL_ERROR, 
    CLOSED_CONNECTION 
};
class HttpData{
public:
    typedef std::shared_ptr<HttpData> ptr;
    HttpData(EventLoop* e, int fd);
    ~HttpData();
    SP_Channel getChannel();
    void reset();
    void seperateTimer();
    void linkTimer(std::shared_ptr<Timer> mtimer);
    void closeHandle();
    void newConnectEvent();
    void ReadHandle();
    //void WriteHandle();
    void ConnHandle();
    
    void WriteHandle(int fd, int err_num, std::string short_msg);
    LINE_STATUS parse_line(char* buffer, int& checked_index, int& read_index );
    HTTP_CODE parse_requestline( char* szTemp, CHECK_STATE& checkstate );
    HTTP_CODE parse_headers( char* szTemp);
    HTTP_CODE parse_keep(char* szTemp, CHECK_STATE& checkstate);
    HTTP_CODE parse_content( char* buffer, int& checked_index, CHECK_STATE& checkstate, int& read_index, int& start_line );
private:
    EventLoop* eventloop;
    int FD;
    SP_Channel channel;
    std::string InBuffer;
    std::string OutBuffer;
    const int timeout = TIMEOUT;
    HTTP_CODE httpstate;
    std::shared_ptr<Timer>  timer;
    bool keeplive;
};


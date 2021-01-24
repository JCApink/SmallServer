#include "HttpData.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <iostream>
#include "Channel.h"
#include "EventLoop.h"
#include "Util.h"
#include "time.h"





//static const char* szret[] = { "I get a correct result\n", "Something wrong\n" };


HttpData::HttpData(EventLoop* e, int fd)
    :eventloop(e)
    ,FD(fd)
    ,channel(new Channel(e, fd))
    ,httpstate(NO_REQUEST)
    ,keeplive(false){
        channel->setReadHandle(std::bind(&HttpData::ReadHandle, this));
        //channel->setWriteHandle(std::bind(&HttpData::WriteHandle, this));
        channel->setConnHandle(std::bind(&HttpData::ConnHandle, this));
}

HttpData::~HttpData(){
    //std::cout << "close:" << FD << std::endl;
    close(FD);
}

SP_Channel HttpData::getChannel(){
    return channel;
}
void HttpData::reset(){
    if(timer.lock()) {
        std::shared_ptr<TimerNode> mytime(timer.lock());
        mytime->clearReq();
        mytime.reset();
    }
}

void HttpData::seperateTimer(){
    if(timer.lock()) {
        std::shared_ptr<TimerNode> mytime(timer.lock());
        mytime->clearReq();
        mytime.reset();
    }
}

void HttpData::linkTimer(std::shared_ptr<TimerNode> mtimer){
    timer = mtimer;
}

void HttpData::closeHandle(){
    httpstate = CLOSED_CONNECTION;
    eventloop->getepoll()->epoll_del(channel, 0);
}

void HttpData::newConnectEvent(){
    channel->setEvents(EPOLLET | EPOLLIN);
    LOG << "INFO: NEW CONNECTCHANNEL IN HTTPDATA ";
    LOG << "INFO: EpollFd: ";
    eventloop->getepoll()->epoll_add(channel, timeout);
}

void HttpData::ReadHandle(){
    uint32_t event = channel->getEvent();
    int data_read = 0;
    int read_index = 0;
    int checked_index = 0;
    int start_line = 0;
    CHECK_STATE checkstate = CHECK_STATE_REQUESTLINE;
    data_read = readn(FD, InBuffer);
    //std::cout << "number" << std::endl;
    //std::cout << InBuffer << std::endl;
    if(httpstate == CLOSED_CONNECTION){
        InBuffer.clear();
    }
    if(data_read < 0){
        LOG << "ERROR";
    } else {
        read_index += data_read;
        HTTP_CODE httpstate = parse_content( &InBuffer.front(), checked_index, checkstate, read_index, start_line );
        if( httpstate == NO_REQUEST || httpstate == GET_REQUEST) {
            WriteHandle(FD, 200, "SUCESS");
        } else {
            //std::cout << "write" << std::endl;
            WriteHandle(FD, 404, "ERROR");
        }
    }
}
/*
void HttpData::WriteHandle(){
    if(httpstate == GET_REQUEST){
        uint32_t event = channel->getEvent();
        std::cout << "out"<<std::endl;
    }
}
*/
void HttpData::ConnHandle(){
    //std::cout << "time" << std::endl;
    seperateTimer();
    //std::cout << "time" << std::endl;
    uint32_t& event = channel->getEvent();
    if(httpstate == GET_REQUEST){
        if(event != 0){
            int time = timeout;
            if(keeplive){
                time = LONGTIMEOUT;
            }
            if((event & EPOLLIN) && (event & EPOLLOUT)) {
                event = uint32_t (0);
                event |= EPOLLOUT;
            }
            event |= EPOLLET;
            eventloop->getepoll()->epoll_mod(channel, time);
        } else if (keeplive) {
            event |= (EPOLLIN | EPOLLET);
            int time = LONGTIMEOUT;
            eventloop->getepoll()->epoll_mod(channel, time);
        } else {
            event |= (EPOLLIN | EPOLLET);
            int time = (LONGTIMEOUT >> 1); 
            eventloop->getepoll()->epoll_mod(channel, time);
        }
    } else {
        //std::cout << "close" << std::endl;
        eventloop->runInLoop(std::bind(&HttpData::closeHandle, this));
    }

}

/*
GET / HTTP/1.0
Connection: Keep-Alive(可选)
Host: 127.0.0.1
User-Agent: ApacheBench/2.3
Accept: *//*

*/

LINE_STATUS HttpData::parse_line( char* buffer, int& checked_index, int& read_index ) {
    char temp;
    for ( ; checked_index < read_index; ++checked_index ){
        temp = buffer[ checked_index ];
        if ( temp == '\r' ){   
            if ( ( checked_index + 1 ) == read_index ){
                return LINE_OPEN;
            }
            else if ( buffer[ checked_index + 1 ] == '\n' ){
                buffer[ checked_index++ ] = '\0';
                buffer[ checked_index++ ] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
        else if( temp == '\n' ){
            if( ( checked_index > 1 ) &&  buffer[ checked_index - 1 ] == '\r' ){
                buffer[ checked_index-1 ] = '\0';
                buffer[ checked_index++ ] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
    }
    return LINE_OPEN;
}

/*
GET / HTTP/1.0
Connection: Keep-Alive
Host: 127.0.0.1
User-Agent: ApacheBench/2.3
Accept: *//*
*/

HTTP_CODE HttpData::parse_requestline( char* szTemp, CHECK_STATE& checkstate ){
    char* szURL = strpbrk( szTemp, " \t" );
    if ( ! szURL ){
        return BAD_REQUEST;
    }
    *szURL++ = '\0';
    char* szMethod = szTemp;
    if ( strcasecmp( szMethod, "GET" ) == 0 ){
        OutBuffer += "GET";
    } else {
        return BAD_REQUEST;
    }
    szURL += strspn( szURL, " \t" );
    char* szVersion = strpbrk( szURL, " \t" );
    if ( ! szVersion ){
        return BAD_REQUEST;
    }
    *szVersion++ = '\0';
    szVersion += strspn( szVersion, " \t" );
    if ( strcasecmp( szVersion, "HTTP/1.0" ) != 0 ){
        return BAD_REQUEST;
    }
    if ( strncasecmp( szURL, "http://", 7 ) == 0 ){
        std::cout << "http" << std::endl;
        szURL += 7;
        szURL = strchr( szURL, '/' );
    }
    if ( ! szURL || szURL[ 0 ] != '/' ){
        return BAD_REQUEST;
    }
    checkstate = CHECK_STATE_KEEP;
    return NO_REQUEST;
}

HTTP_CODE HttpData::parse_keep(char* szTemp, CHECK_STATE& checkstate) {
    //std::cout << szTemp[0] << std::endl;
    if ( szTemp[ 0 ] == '\0' ){
        return GET_REQUEST;
    }
    else if ( strncasecmp( szTemp, "Connection:", 10 ) == 0 ){
        szTemp += 12;
        if( strncasecmp( szTemp, "Keep-Alive", 10 ) == 0) {
            keeplive = true;
        }
    }else{
        //printf( "I can not handle this header\n" );
    }
    checkstate = CHECK_STATE_HEADER;
    return NO_REQUEST;
}

HTTP_CODE HttpData::parse_headers( char* szTemp ){
    if ( szTemp[ 0 ] == '\0' ){
        return GET_REQUEST;
    }
    else if ( strncasecmp( szTemp, "Host:", 5 ) == 0 ){
        //std::cout << "host" << std::endl;
        szTemp += 5;
        szTemp += strspn( szTemp, " \t" );
    }else{
        //printf( "I can not handle this header\n" );
    }
    return NO_REQUEST;
}

HTTP_CODE HttpData::parse_content( char* buffer, int& checked_index, CHECK_STATE& checkstate, int& read_index, int& start_line ){
    LINE_STATUS linestatus = LINE_OK;
    HTTP_CODE retcode = NO_REQUEST;
    while( ( linestatus = parse_line( buffer, checked_index, read_index ) ) == LINE_OK ){ 
        char* szTemp = buffer + start_line;
        start_line = checked_index;
        switch ( checkstate ){
            case CHECK_STATE_REQUESTLINE:{
                retcode = parse_requestline(szTemp, checkstate);
                if ( retcode == BAD_REQUEST ){
                    return BAD_REQUEST;
                }
                break;
            }
            
            case CHECK_STATE_KEEP: {
                retcode = parse_keep(szTemp, checkstate);
                break;
            }
            case CHECK_STATE_HEADER:{
                retcode = parse_headers( szTemp );
                if ( retcode == BAD_REQUEST ){
                    return BAD_REQUEST;
                }
                else if ( retcode == GET_REQUEST ){
                    return GET_REQUEST;
                }
                break;
            }
            default:{
                return INTERNAL_ERROR;
            }
        }
    }
    if( linestatus == LINE_OPEN ){
        return NO_REQUEST;
    }
    else{
        return BAD_REQUEST;
    }
}

void HttpData::WriteHandle(int fd, int err_num, std::string short_msg) {
    short_msg = " " + short_msg;
    char send_buff[4096];
    std::string body_buff, header_buff;
    body_buff += "<html><title>Welcome</title>";
    body_buff += "<body bgcolor=\"ffffff\">";
    body_buff += std::to_string(err_num) + short_msg;
    body_buff += "<hr><em> My Web Server</em>\n</body></html>";

    header_buff += "HTTP/1.0 " + std::to_string(err_num) + short_msg + "\r\n";
    header_buff += "Content-Type: text/html\r\n";
    header_buff += "Connection: Close\r\n";
    header_buff += "Content-Length: " + std::to_string(body_buff.size()) + "\r\n";
    header_buff += "\r\n";
    sprintf(send_buff, "%s", header_buff.c_str());
    writen(fd, send_buff, strlen(send_buff));
    sprintf(send_buff, "%s", body_buff.c_str());
    writen(fd, send_buff, strlen(send_buff));
}

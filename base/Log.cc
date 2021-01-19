#include "Log.h"
#include <sys/time.h>
#include "Thread.h"
#include "AsyncLogging.h"
#include <iostream>

static AsynacLogging *Asynac;
static pthread_once_t once_control = PTHREAD_ONCE_INIT;

std::string Log::FileName = "./test.txt";

void once_init(){
    Asynac = new AsynacLogging(Log::getFilename());
    Asynac->start();
}

void output(const char* data, int len){
    //std::cout << "output" <<std::endl;
    pthread_once(&once_control, &once_init);
    Asynac->append(data, len);
}

Log::Log(std::string filename, int line)
    :logImpl(filename, line)
    {
}
Log::~Log(){
    logImpl.logstream<< " " << logImpl.FileName <<  ":"  << logImpl.Line << "\n";

    const LogStream::Buffer& buf(stream().getbuffer());

    output(buf.head(), buf.length());
    
}

void Log::setFilename(std::string& filename){
    FileName = filename;
}

std::string Log::getFilename(){
        return FileName;
}

LogStream &Log::stream(){
    return logImpl.logstream;
}   

Log::LogImpl::LogImpl(std::string filename, int line)
    :FileName(filename)
    ,Line(line){
        setTime();
}

Log::LogImpl::~LogImpl(){

}



void Log::LogImpl::setTime(){
    struct timeval tv;
    time_t time;

    char time_buf[26] = {0};
    gettimeofday(&tv, NULL);

    time = tv.tv_sec;
    struct tm* p_time = localtime(&time);

    strftime(time_buf,26, "%Y-%m-%d %H:%M:%S\n", p_time);
    logstream << time_buf;
}



#pragma once
#include <memory>
#include "FileAppender.h"
#include "MutexLock.h"

#define FLUST_NUMBER 1024


class LogFile : public Noncopyable{
public:
    typedef std::shared_ptr<LogFile> ptr;

    LogFile(std::string filename, int f_num = FLUST_NUMBER);
    ~LogFile();

    void append(const char* data, int len);

    void flush();

    void append_unlock(const char* data, int len);

private:
    std::string FileName;
    const int flush_num;
    int flush_count;

    FileAppender::ptr file;
    MutexLock::ptr Mutex;
    


};



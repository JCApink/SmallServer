#pragma once
#include <memory>
#include "FileAppender.h"
#include "MutexLock.h"

#define FLUST_NUMBER 1024

/*
    这是一个连接数据写入到文件中的类 含有FileAppender类
    提供接口:
        线程安全的写：append(const char* data, int len)
        线程不安全的写：append_unlock(const char* data, int len)
        写数量达到一定数量进行刷新：flush()
*/

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



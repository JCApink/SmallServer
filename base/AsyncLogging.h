#pragma once
#include <memory>
#include <vector>
#include "FixBuffer.h"
#include "Thread.h"
#include "MutexLock.h"
#include "Condition.h"
/*
    整个Log的核心模块 负责将LOG内容写入文件中 利用"双缓存技术" 
    提供接口:
        添加内容：void append(const char* data, int len);

        设置开始：void start();
        结束设置：void stop();
        线程入口：void threadFunc();


*/


class AsynacLogging{
public:
    AsynacLogging(const std::string& filename, int time = 2);
    ~AsynacLogging();

    void append(const char* data, int len);

    void start();
    void stop();
    void threadFunc();
private:
    
    typedef FixBuffer::ptr Buffer;
    typedef std::vector<Buffer> BufferVector;

    const int flush_time;

    bool runIn;
    std::string FileName;
    Thread thread;
    MutexLock Mutex;
    Condition condition;
    Buffer cur_buffer;
    Buffer next_buffer;
    BufferVector buffers; 

    CountDownLatch countdown;

};


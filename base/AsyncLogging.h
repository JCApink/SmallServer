#pragma once
#include <memory>
#include <vector>
#include "FixBuffer.h"
#include "Thread.h"
#include "MutexLock.h"
#include "Condition.h"

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


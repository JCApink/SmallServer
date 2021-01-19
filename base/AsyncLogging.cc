#include <assert.h>
#include <iostream>
#include "AsyncLogging.h"
#include "LogFile.h"


AsynacLogging::AsynacLogging(const std::string& filename,int time)
    :flush_time(time)
    ,runIn(false)
    ,FileName(filename)
    ,thread(std::bind(&AsynacLogging::threadFunc, this), "Log")
    ,Mutex()
    ,condition(Mutex)
    ,cur_buffer(new FixBuffer)
    ,next_buffer(new FixBuffer)
    ,buffers()
    ,countdown(1) {
        assert(filename.size() > 1);
        cur_buffer->bzero();
        next_buffer->bzero();
        buffers.reserve(16);
}
AsynacLogging::~AsynacLogging(){
    if(runIn) {
        stop();
    }
}

void AsynacLogging::append(const char* data, int len) {
    MutexLockGuard lock(Mutex);
    if(cur_buffer->empty() > len) {
        cur_buffer->append(data, len);
    } else {
        buffers.push_back(cur_buffer);
        cur_buffer.reset();
        if(next_buffer) {
            cur_buffer = std::move(next_buffer);
        }
        else {
            cur_buffer.reset(new FixBuffer);
        }
        cur_buffer->append(data, len);
        condition.notify();
    }

}

void AsynacLogging::start(){
    runIn = true;
    thread.start();
    //countdown.wait();
}

void AsynacLogging::stop(){
    runIn = false;
    condition.notify();
    thread.join();
}

void AsynacLogging::threadFunc(){
    assert(runIn == true);
    countdown.countDown();
    LogFile file(FileName);
    Buffer newBufferA(new FixBuffer);
    Buffer newBufferB(new FixBuffer);
    newBufferA->bzero();
    newBufferB->bzero();
    BufferVector bufferswrite;
    bufferswrite.reserve(16);
    while(runIn){
        assert(newBufferA && newBufferA->length() == 0);
        assert(newBufferB && newBufferB->length() == 0);
        assert(bufferswrite.empty());
        {
            MutexLockGuard lock(Mutex);
            //std::cout << pthread_self() << std::endl;
            if(buffers.empty()){
                condition.waitforsecond(flush_time);
            }
            buffers.push_back(cur_buffer);
            cur_buffer.reset();
            cur_buffer = std::move(newBufferA);
            bufferswrite.swap(buffers);
            if(!next_buffer){
                next_buffer = std::move(newBufferB);
            }
        }
        assert(!bufferswrite.empty());
        if(bufferswrite.size() > 25){
            bufferswrite.erase(bufferswrite.begin() + 2, bufferswrite.end());
        }
        for(size_t i = 0; i < bufferswrite.size(); ++i){
            file.append(bufferswrite[i]->head(), bufferswrite[i]->length());
        }
        if(bufferswrite.size() > 2){
            bufferswrite.resize(2);
        }
        if(!newBufferA){
            assert(!bufferswrite.empty());
            newBufferA = bufferswrite.back();
            bufferswrite.pop_back();
            newBufferA->reset();
        }
        if(!newBufferB){
            assert(!bufferswrite.empty());
            newBufferB = bufferswrite.back();
            bufferswrite.pop_back();
            newBufferB->reset();
        }
        bufferswrite.clear();
        file.flush();
    }
    file.flush();

}


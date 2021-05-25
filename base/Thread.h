#pragma once 
#include <memory>
#include <functional>
#include "noncopyable.h"
#include "CountDownLatch.h"

class Thread : public Noncopyable{
public:
    typedef std::shared_ptr<Thread> ptr;

    typedef std::function<void()> ThreadFunc;

    Thread(const ThreadFunc&, const std::string& name = std::string());
    ~Thread();

    void setDefaultName();

    void start();
    int join();
    
    bool getStart() const;
    pid_t gettid() const;
    const std::string& getname() const;

private:
    bool start_;
    bool join_;

    pthread_t pthreadId;

    std::string p_name;
    ThreadFunc Func;
    pid_t tid;


    CountDownLatch countdown;
};



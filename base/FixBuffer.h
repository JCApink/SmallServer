#pragma once
#include <memory>
#include "noncopyable.h"

#define SIZE 4000*1000
/*
    Buffer类 可以执行一系列添加数据进缓冲区的操作

    提供接口：
        添加数据进buffer中 append(const char* buf, int len)
        取出第一个buffer的位置 const char* head() const;
        计算当前buffer剩余空间 const int empty();
        计算buffer数据大小const int length();
        计算当前buffer的数据
        获取当前数据位置 char* getcur();
        添加数据进buffer后的位置变化 void add(size_t len);
        重设当前buffer空间 void reset();
        初始化buffer void bzero();

*/

class FixBuffer : public Noncopyable{
public:
    typedef std::shared_ptr<FixBuffer> ptr;
    
    FixBuffer();
    ~FixBuffer();
    
    void append(const char* buf, size_t len);

    const char* head() const;

    char* gethead();

    int empty() const;

    int length() const;

    char* getcur();

    void add(size_t len);

    void reset();

    void bzero();
    
    void show();
private:
    char data[SIZE];

    const char* end() const;

    char* cur;

};


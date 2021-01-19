#pragma once
#include <memory>
#include "FixBuffer.h"
#include "noncopyable.h"
/*
    将不同类型进行格式化的类 全部转换成字符串类型,并且存在Buffer中
    提供接口:
        添加数据进Buffer中:void append(const char* data, int len);
        获取当前Buffer：const FixBuffer::ptr& getbuffer() const;
        清空Buffer：void resetBuffer();
        Int类型的转换：size_t convert(char buf[], T n);
        Int格式化：void formatInt(T v);
        各种类型的转换：
*/


class LogStream : public Noncopyable{
public:
    typedef std::shared_ptr<LogStream> ptr;
    typedef FixBuffer Buffer;
    
    LogStream();
    ~LogStream();

    void append(const char* data, int len);
    const FixBuffer& getbuffer() const;
    void resetBuffer();

    template<class T>
    size_t convert(char buf[], T n);

    template<class T>
    void formatInt(T v);
    
    LogStream& operator<<(bool n);
    LogStream& operator<<(short n);
    LogStream& operator<<(unsigned short n);
    LogStream& operator<<(int n);
    LogStream& operator<<(unsigned int n);
    LogStream& operator<<(long n);
    LogStream& operator<<(unsigned long n);
    LogStream& operator<<(long long n);
    LogStream& operator<<(unsigned long long n);
    
    LogStream& operator<<(float n);
    LogStream& operator<<(double n);
    LogStream& operator<<(long double n);

    LogStream& operator<<(char n);
    LogStream& operator<<(const char* n);
    LogStream& operator<<(const unsigned char* n);
    LogStream& operator<<(const std::string& n);

private:
    Buffer buffer;
    static const int KMax = 32;

    
    
};



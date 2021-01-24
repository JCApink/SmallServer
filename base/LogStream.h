#pragma once
#include <memory>
#include "FixBuffer.h"
#include "noncopyable.h"

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



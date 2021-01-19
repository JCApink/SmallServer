#include "LogStream.h"
#include <string.h>
#include <stdio.h>
#include <algorithm>


const char digits[] = "9876543210123456789";
const char* zero = digits + 9;

LogStream::LogStream(){

}

LogStream::~LogStream(){

}

template<class T>
size_t LogStream::convert(char buf[], T n){
    T tmp = n;
    char* p = buf;
    do{
        int a = static_cast<int>(tmp%10);
        tmp /= 10;
        *p++ = zero[a];
    }while (tmp);
    
    if(n < 0){
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);
    return p - buf;
}

template<class T>
void LogStream::formatInt(T n){
    if(buffer.empty() >= KMax){
        size_t len = convert(buffer.getcur(), n);
        buffer.add(len);
    }
}

LogStream& LogStream::operator<<(bool n){
    append(n ? "1" : "0", 1);
    return *this;
}
LogStream& LogStream::operator<<(short n){
    *this << static_cast<int> (n);
    return *this;
}
LogStream& LogStream::operator<<(unsigned short n){
    *this << static_cast<int> (n);
    return *this;
}
LogStream& LogStream::operator<<(int n){
    formatInt(n);
    return *this;
}
LogStream& LogStream::operator<<(unsigned int n){
    formatInt(n);
    return *this;
}
LogStream& LogStream::operator<<(long n){
    formatInt(n);
    return *this;
}
LogStream& LogStream::operator<<(unsigned long n){
    formatInt(n);
    return *this;
}
LogStream& LogStream::operator<<(long long n){
    formatInt(n);
    return *this;
}
LogStream& LogStream::operator<<(unsigned long long n){
    formatInt(n);
    return *this;
}

LogStream& LogStream::operator<<(float n){
    *this << static_cast<double>(n);
    return *this;
}
LogStream& LogStream::operator<<(double n){
    if(buffer.empty() > KMax){
        size_t len = snprintf(buffer.getcur(), KMax, "%.12g", n);
        buffer.add(len);
    }
    return *this;
}
LogStream& LogStream::operator<<(long double n){
    if(buffer.empty() > KMax){
        size_t len = snprintf(buffer.getcur(), KMax, "%.12Lg", n);
        buffer.add(len);
    }
    return *this;
}

LogStream& LogStream::operator<<(char n){
    append(&n, 1);
    return *this;

}
LogStream& LogStream::operator<<(const char* n){
    if(n){
        append(n, strlen(n));
    }
    else{
        append("(null)", 4);
    }
    return *this;
}
LogStream& LogStream::operator<<(const unsigned char* n){
    *this << reinterpret_cast<const char*>(n);
    return *this;
}
LogStream& LogStream::operator<<(const std::string& n){
    append(n.c_str(), n.length());
    return *this;
}

void LogStream::append(const char* data, int len){
    buffer.append(data, len);
}

const FixBuffer& LogStream::getbuffer() const{
    return buffer;
} 

void LogStream::resetBuffer(){
    buffer.reset();
}



#pragma once
#include <memory>
#include "noncopyable.h"

#define SIZE 4000*1000


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


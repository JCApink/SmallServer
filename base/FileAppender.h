#pragma once
#include <memory>
#include "noncopyable.h"

class FileAppender : public Noncopyable{
public:
    typedef std::shared_ptr<FileAppender> ptr;

    explicit FileAppender(std::string filename);

    ~FileAppender();

    void append(const char *logline, size_t len);

    void flush();


private:

    FILE *fp;

    char buffer[64*1024];



};



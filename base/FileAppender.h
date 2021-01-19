#pragma once
#include <memory>
#include "noncopyable.h"
/*
    这是一个写入文件的类 自己创建一个文件缓冲区大小为 64*1024

    提供接口：
        缓冲区刷新：flush()
        写入文件中：append(const char *logline, size_t len)

*/

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



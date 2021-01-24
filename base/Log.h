#pragma once
#include <memory>
#include "noncopyable.h"
#include "LogStream.h"
#include "AsyncLogging.h"

class Asynclogging;
#define LOG Log(__FILE__, __LINE__).stream()

class Log : public Noncopyable{
public:
    typedef std::shared_ptr<Log> ptr;

    Log(std::string filename, int line);
    ~Log();

    static void setFilename(std::string& filename);
    static std::string getFilename();

    LogStream &stream();

private:
    class LogImpl{
    public:
        typedef std::shared_ptr<LogImpl> ptr; 

        LogImpl(std::string filename, int line);
        ~LogImpl();

        void setTime();

        std::string FileName;
        int Line;
        LogStream logstream;

    };

    LogImpl logImpl;
    

    static std::string FileName;
    int Line;


}; 




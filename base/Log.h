#pragma once
#include <memory>
#include "noncopyable.h"
#include "LogStream.h"
#include "AsyncLogging.h"
/*
    日志类  启动AsyncLogging进行日志的输出
    里面包含一个输出类 进行内容与LogStream连接
    接口:
        设置文件名：static void setFilename(std::string& filename);
        获取文件名：static std::string getFilename() 
        获取LogStream进行文件输出：LogStream &stream();

        日志时间格式化：void setTime();

*/

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




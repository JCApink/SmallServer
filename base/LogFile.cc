#include "LogFile.h"
#include "FileAppender.h"

LogFile::LogFile(std::string filename, int f_num)
    :FileName(filename)
    ,flush_num(f_num)
    ,flush_count(0){
        Mutex.reset(new MutexLock());
        file.reset(new FileAppender(filename));
}
LogFile::~LogFile(){

}

void LogFile::append(const char* data, int len){
    MutexLockGuard lock(*Mutex);
    append_unlock(data, len);
}

void LogFile::flush(){
    MutexLockGuard lock(*Mutex);
    file->flush();
}
void LogFile::append_unlock(const char* data, int len){
    file->append(data, len);

    ++flush_count;

    if(flush_count >= flush_num){
        flush_count = 0;
        file->flush();
    }
}



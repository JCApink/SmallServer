#include "FileAppender.h"


FileAppender::FileAppender(std::string filename)
    :fp(fopen(filename.c_str(),"ae")){
        setbuffer(fp, buffer, sizeof(buffer));

}
FileAppender::~FileAppender(){
    fclose(fp);
}

void FileAppender::append(const char *logline, size_t len){
    size_t n = 0;
    while(len){
        n = fwrite_unlocked(logline + n, 1, len, fp);
        if(!n){
            int err = ferror(fp);
            if(err){
                fprintf(stderr, "write file fail");
            }
            
            break;
        }
        len -=n;
    }
}

void FileAppender::flush(){
    fflush(fp);
}



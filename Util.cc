#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <iostream>
#include "Util.h"
#include "base/FixBuffer.h"
#include "base/Log.h"
#include <iostream>

static const int MAX_BUFFER = 4096; 


ssize_t readn(int fd, void* buffer, size_t n){
    char *ptr = (char*) buffer;
    size_t sum = 0;
    size_t readsum = 0;
    while(n > 0){
        if((readsum = read(fd, ptr, n)) < 0){
            if(errno == EINTR){
                readsum = 0;
            }else if(errno == EAGAIN){
                return sum;
            }else{
                return -1;
            }
        }else if(readsum == 0){
            break;
        }
        sum += readsum;
        n -= readsum;
        ptr += readsum;
    }
    return sum;
}

ssize_t readn(int fd, std::string& Buffer){
    ssize_t readsum = 0;
    ssize_t sum = 0;
    while(true){
        char buff[MAX_BUFFER];
        if((readsum = read(fd, buff, MAX_BUFFER)) < 0){
            if(errno == EINTR){
                continue;
            }else if(errno == EAGAIN){
                return sum;
            }else{
                std::cout << "READ ERROR" << std::endl;
                return -1;
            }
        }
        else if(readsum == 0){
            break;
        }
        sum += readsum;
        Buffer += std::string(buff, readsum);
    }
    return sum;
}

ssize_t writen(int fd, void* buffer, size_t n){
    char *ptr = (char*) buffer;
    size_t sum = 0;
    size_t writesum = 0;
    while(n > 0){
        if((writesum = write(fd, ptr, n)) < 0){
            if(errno == EINTR){
                writesum = 0;
            }else if(errno == EAGAIN){
                return sum;
            }else{
                return -1;
            }
        }else if(writesum == 0){
            break;
        }
        sum += writesum;
        n -= writesum;
        ptr += writesum;
    }
    return sum;
}

ssize_t writen(int fd, std::string& Buffer){
    size_t nleft = Buffer.size();
    ssize_t nwritten = 0;
    ssize_t writeSum = 0;
    const char *ptr = Buffer.c_str();
    while (nleft > 0) {
        if ((nwritten = write(fd, ptr, nleft)) <= 0) {
            if (nwritten < 0) {
                if (errno == EINTR) {
                    nwritten = 0;
                    continue;
                } else if (errno == EAGAIN) {
                    break;
                } else {
                    return -1;
                }
            }
        }
        writeSum += nwritten;
        nleft -= nwritten;
        ptr += nwritten;
        //std::cout << nwritten << std::endl;
    }
    if (writeSum == static_cast<int>(Buffer.size())) {
        Buffer.clear();
    } else {
        Buffer = Buffer.substr(writeSum);
    }
    return writeSum;
}

int setNonBlock(int& fd){
    int flag;
    flag = fcntl(fd, F_GETFL, 0);
    if(fd == -1){
        LOG << "GET FD ERROR";
        assert(fd != -1);
    }
    flag |= O_NONBLOCK;

    if(fcntl(fd, F_SETFL, flag) == -1){
        assert(fd != -1);
        LOG << "SET FD ERROR";
        return -1;
    }
    return 0;
}



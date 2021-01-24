#pragma once
#include <sys/types.h>
#include "base/FixBuffer.h"
#include "base/Log.h"





ssize_t readn(int fd, void* buffer, size_t n);

ssize_t readn(int fd, std::string& Buffer);

ssize_t writen(int fd, void* buffer, size_t n);
ssize_t writen(int fd, std::string& Buffer);

int setNonBlock(int &fd);



#include "FixBuffer.h"
#include <string.h>


FixBuffer::FixBuffer()
    :cur(data){

}
FixBuffer::~FixBuffer(){

}

void FixBuffer::append(const char* buf, size_t len){
    if(empty() >(int) len){
        memcpy(cur, buf, len);
        cur += len;
    }
}

const char* FixBuffer::head() const{
    return data;
}

char* FixBuffer::gethead(){
    return data;
}

int FixBuffer::empty() const{
    return static_cast<int>(end() - cur);
}

int FixBuffer::length() const{
    return static_cast<int>(cur - data);
}

char* FixBuffer::getcur(){
    return cur;
}

void FixBuffer::add(size_t len){
    cur += len;
}

void FixBuffer::reset(){
    cur = data;
}

void FixBuffer::bzero(){
    memset(data, 0, sizeof(data));
}

const char* FixBuffer::end() const{
    return data + sizeof data;
}

void FixBuffer::show(){
    printf("%s\n", data);
}


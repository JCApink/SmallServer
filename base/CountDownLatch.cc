#include "CountDownLatch.h"
#include <iostream>



//static int n = 0;
CountDownLatch::CountDownLatch(int c)
    :Mutex()
    ,condition(Mutex)
    ,count(c){

}
CountDownLatch::~CountDownLatch(){

}

void CountDownLatch::wait(){
    MutexLockGuard lock(Mutex);
    while (count > 0) {
        condition.wait();
    }
    

}
void CountDownLatch::countDown() {
    MutexLockGuard lock(Mutex);
    --count;
    if(count == 0){
        condition.notifyAll();
    }
}



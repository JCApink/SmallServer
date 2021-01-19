#include "Condition.h"



Condition::Condition(MutexLock& mutex)
    :Mutex(mutex){
        pthread_cond_init(&cond, NULL);
}

Condition::~Condition(){
    pthread_cond_destroy(&cond);
}

void Condition::wait(){
    pthread_cond_wait(&cond, Mutex.getMutex());
}   

bool Condition::waitforsecond(int seconds){
    static timespec abstime;
    clock_gettime(CLOCK_REALTIME, &abstime);
    abstime.tv_sec += static_cast<time_t>(seconds);
    return ETIMEDOUT == pthread_cond_timedwait(&cond, Mutex.getMutex(), &abstime);
}

void Condition::notify(){
    pthread_cond_signal(&cond);
}

void Condition::notifyAll(){
    pthread_cond_broadcast(&cond);
}



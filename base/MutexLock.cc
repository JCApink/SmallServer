#include "MutexLock.h"


MutexLock::MutexLock(){
    pthread_mutex_init(&mutex, NULL);
}
MutexLock::~MutexLock(){
    pthread_mutex_destroy(&mutex);
}

void MutexLock::lock(){
    pthread_mutex_lock(&mutex);
}
void MutexLock::unlock(){
    pthread_mutex_unlock(&mutex);
}

pthread_mutex_t* MutexLock::getMutex(){
    return &mutex;
}

MutexLockGuard::MutexLockGuard(MutexLock& mutex)
    :Mutex(mutex){
        Mutex.lock();
}
    
MutexLockGuard::~MutexLockGuard(){
    Mutex.unlock();
}



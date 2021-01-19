#include "Timer.h"
#include <sys/time.h>


Timer::Timer(std::shared_ptr<HttpData> requestData, int timeout)
    :deleted_(false)
    ,SPHttpData(requestData) {
        struct timeval now;
        gettimeofday(&now, NULL);
        // 以毫秒计
        expiredTime_ =
                    (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
}

Timer::~Timer(){
    if (SPHttpData) {
        SPHttpData->closeHandle();
    }
}

Timer::Timer(Timer &tn)
    : SPHttpData(tn.SPHttpData), expiredTime_(0){

}

void Timer::update(int timeout){
    struct timeval now;
    gettimeofday(&now, NULL);
    expiredTime_ =
            (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
}
bool Timer::isValid(){
    struct timeval now;
    gettimeofday(&now, NULL);
    size_t temp = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000));
    if (temp < expiredTime_){
        return true;
    }
    else {
        this->setDeleted();
        return false;
    }
}
void Timer::clearReq(){
    SPHttpData.reset();
    this->setDeleted();
}
void Timer::setDeleted(){ 
    deleted_ = true; 
}
bool Timer::isDeleted() const{
    return deleted_; 
}

size_t Timer::getExpTime() const{
    return expiredTime_; 
}

TimerManager::TimerManager() {}

TimerManager::~TimerManager() {}

void TimerManager::addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout) {
    SPTimerNode new_node(new Timer(SPHttpData, timeout));
    timerNodeQueue.push(new_node);
    SPHttpData->linkTimer(new_node);
}

void TimerManager::handleExpiredEvent() {
  // MutexLockGuard locker(lock);
    while (!timerNodeQueue.empty()) {
        SPTimerNode ptimer_now = timerNodeQueue.top();
        if (ptimer_now->isDeleted()){
            timerNodeQueue.pop();
        }else if (ptimer_now->isValid() == false){
            timerNodeQueue.pop();
        }else{
            break;
        }
    }

}


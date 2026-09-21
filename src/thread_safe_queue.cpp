#include "thread_safe_queue.h"

bool ThreadSafeQueue::push(int value){
    {
        std::lock_guard<std::mutex> guard(mutex_);

        if(closed_){
            return false;
        }

        items_.push(value);
    }
    cv_.notify_one();
    return true;
}

bool ThreadSafeQueue::wait_and_pop(int &value){
    std::unique_lock<std::mutex> guard(mutex_);

    while(items_.empty() && !closed_){
        cv_.wait(guard);
    }
    
    if(items_.empty() && closed_){
        return false;
    }

    value = items_.front();
    items_.pop();
    return true;
}

void ThreadSafeQueue::close(){
    {
        std::lock_guard<std::mutex> guard(mutex_);
        closed_ = true;
    }
    cv_.notify_all();
}
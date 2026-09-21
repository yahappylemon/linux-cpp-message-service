#include <iostream>
#include <mutex>
#include <thread>
#include <functional>
#include <atomic>
#include "thread_safe_queue.h"

const int message_count = 5;
std::mutex output_mutex;

void produce(ThreadSafeQueue &queue){
    for(int i=1;i<=message_count;i++){
        queue.push(i);
    }
    queue.close();
}

void consume(ThreadSafeQueue &queue, int consumer_id){
    int value = 0;

    while(queue.wait_and_pop(value)){
        std::lock_guard<std::mutex> guard(output_mutex);
        std::cout<<"consumer "<<consumer_id<<" got: "<<value<<"\n";
    }

    {
        std::lock_guard<std::mutex> guard(output_mutex);
        std::cout<<"consumer "<<consumer_id<<" finished"<<"\n";
    }

}

int main(){
    ThreadSafeQueue queue;

    std::thread producer_thread(produce, std::ref(queue));
    std::thread consumer_thread_1(consume, std::ref(queue), 1);
    std::thread consumer_thread_2(consume, std::ref(queue), 2);

    producer_thread.join();
    consumer_thread_1.join();
    consumer_thread_2.join();
    
    std::cout<<"all messages processed\n";

    return 0;
}
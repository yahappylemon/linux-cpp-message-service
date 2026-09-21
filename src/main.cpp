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

void consume(
    ThreadSafeQueue &queue,
    int consumer_id,
    std::atomic<int> &consumed_count
){
    int value = 0;

    while(queue.wait_and_pop(value)){
        consumed_count++;

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
    std::atomic<int> consumed_count = 0;

    std::thread producer_thread(produce, std::ref(queue));
    std::thread consumer_thread_1(
        consume,
        std::ref(queue),
        1,
        std::ref(consumed_count)
    );
    std::thread consumer_thread_2(
        consume,
        std::ref(queue),
        2,
        std::ref(consumed_count)
    );

    producer_thread.join();
    consumer_thread_1.join();
    consumer_thread_2.join();
    
    std::cout<<"produced: "<<message_count
             <<", consumed: "<<consumed_count.load()<<"\n";

    return 0;
}
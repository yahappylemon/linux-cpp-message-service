#include <iostream>
#include <thread>
#include <functional>
#include <chrono>
#include <algorithm>
#include <vector>
#include <mutex>
#include "thread_safe_queue.h"

void received_one(ThreadSafeQueue &queue, bool &received, int &value){
    received = queue.wait_and_pop(value);
}

void collect_all(ThreadSafeQueue &queue,std::vector<int> &received_values, std::mutex &received_mutex){
    int value = 0;
    while(queue.wait_and_pop(value)){
        std::lock_guard<std::mutex> guard(received_mutex);
        received_values.push_back(value);
    }
}

void produce_values(ThreadSafeQueue &queue, int count){
    for(int value = 1; value <= count; value++){
        queue.push(value);
    }

    queue.close();
}

bool test_fifo(){
    ThreadSafeQueue queue;
    
    bool pushed_10 = queue.push(10);
    bool pushed_20 = queue.push(20);
    bool pushed_30 = queue.push(30);

    queue.close();

    int first = 0;
    int second = 0;
    int third = 0;
    int extra = 0;

    bool got_first = queue.wait_and_pop(first);
    bool got_second = queue.wait_and_pop(second);
    bool got_third = queue.wait_and_pop(third);
    bool no_more_data = !queue.wait_and_pop(extra);

    return pushed_10 && pushed_20 && pushed_30 && 
    got_first && got_second && got_third &&
    first == 10 && second == 20 && third == 30 &&
    no_more_data;
}

bool test_push_after_close(){
    ThreadSafeQueue queue;

    queue.close();

    return !queue.push(99);
}

bool test_repeated_close(){
    ThreadSafeQueue queue;

    queue.close();
    queue.close();

    return !queue.push(100);
}

bool test_push_wakes_waiter(){
    ThreadSafeQueue queue;
    int value = 0;
    bool received = false;

    std::thread consumer_thread(received_one, std::ref(queue), std::ref(received), std::ref(value));
    // 讓 main thread 暫停約 100 毫秒
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    bool pushed = queue.push(4);
    queue.close();

    consumer_thread.join();

    return pushed && received && value==4;
}

bool test_two_consumers_drain_and_exit(){
    ThreadSafeQueue queue;
    std::vector<int> received_values;
    std::mutex received_mutex;

    std::thread producer_thread(produce_values, std::ref(queue), 5);

    std::thread consumer_thread_1(
        collect_all,
        std::ref(queue),
        std::ref(received_values),
        std::ref(received_mutex)
    );

    std::thread consumer_thread_2(
        collect_all,
        std::ref(queue),
        std::ref(received_values),
        std::ref(received_mutex)
    );

    producer_thread.join();
    consumer_thread_1.join();
    consumer_thread_2.join();

    std::sort(received_values.begin(), received_values.end());

    std::vector<int> expected_values = {1, 2, 3, 4, 5};

    return received_values == expected_values;
}

bool test_empty_close_wakes_all_consumers(){
    ThreadSafeQueue queue;
    std::vector<int> received_values;
    std::mutex received_mutex;

    std::thread consumer_thread_1(
        collect_all,
        std::ref(queue),
        std::ref(received_values),
        std::ref(received_mutex)
    );

    std::thread consumer_thread_2(
        collect_all,
        std::ref(queue),
        std::ref(received_values),
        std::ref(received_mutex)
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    queue.close();

    consumer_thread_1.join();
    consumer_thread_2.join();

    return received_values.empty();
}


int main(){
    bool fifo_passed = test_fifo();
    bool push_after_close_passed = test_push_after_close();
    bool repeated_close_passed = test_repeated_close();
    bool push_wakes_waiter_passed = test_push_wakes_waiter();
    bool test_two_consumers_drain_and_exit_passed = test_two_consumers_drain_and_exit();
    bool test_empty_close_wakes_all_consumers_passed = test_empty_close_wakes_all_consumers();

    if(fifo_passed){
        std::cout<<"PASS: FIFO and close-drain\n";
    }else{
        std::cout<<"FAIL: FIFO and close-drain\n";
    }

    if(push_after_close_passed){
        std::cout<<"PASS: push after close is rejected\n";
    }else{
        std::cout<<"FAIL: push after close is rejected\n";
    }

    if(repeated_close_passed){
        std::cout<<"PASS: repeated close is safe\n";
    }else{
        std::cout<<"FAIL: repeated close is safe\n";
    }

    if(push_wakes_waiter_passed){
        std::cout<<"PASS: push wakes the sleeping thread\n";
    }else{
        std::cout<<"FAIL: push wakes the sleeping thread\n";
    }

    if(test_two_consumers_drain_and_exit_passed){
        std::cout<<"PASS: both consumers get all the data\n";
    }else{
        std::cout<<"FAIL: both consumers get all the data\n";
    }

    if(test_empty_close_wakes_all_consumers_passed){
        std::cout<<"PASS: empty close wakes both consumers\n";
    }else{
        std::cout<<"FAIL: empty close wakes both consumers\n";
    }

    return fifo_passed && push_after_close_passed &&
       repeated_close_passed && push_wakes_waiter_passed && test_two_consumers_drain_and_exit_passed && test_empty_close_wakes_all_consumers_passed ? 0 : 1;
}
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
using namespace std;

class MutexQueue{
public:
    void push(int value){
        // 故意加上一個scope，先解鎖再通知，避免thread收到通知時，mutex還沒解鎖
        {
            lock_guard<mutex> guard(mutex_);
            items_.push(value);
        }
        cv_.notify_one();
    }
    // bool safe_pop(int &value){
    //     // lock_guard 的規則：建立時鎖住，離開 scope 才解鎖
    //     lock_guard<mutex> guard(mutex_);
    //     if(items_.empty()){
    //         return false;
    //     }
    //     value = items_.front();
    //     items_.pop();
    //     return true;
    // }
    void wait_and_pop(int &value){
        // unique_lock 的規則：可以被 condition_variable::wait 操作
        unique_lock<mutex> guard(mutex_);
        // predicate：「停止等待的條件」
        while(items_.empty()){
            cv_.wait(guard);
        }
        value = items_.front();
        items_.pop();
    }
private:
    queue<int> items_;
    mutex mutex_;
    condition_variable cv_;
};

const int message_count = 5;

void produce(MutexQueue &queue){
    for(int i=1;i<=message_count;i++){
        queue.push(i);
    }
}

// void consume(MutexQueue &queue){
//     int value = 0;
//     int received = 0;
//     while(received<message_count){
//         if(queue.safe_pop(value)){
//             cout<<"consumer got: " <<value<<endl;
//             received++;
//         }else{
//             // queue 是空的 暫時讓出 CPU
//             this_thread::yield();
//         }
//     }
// }

void consume(MutexQueue &queue){
    int value = 0;
    int received = 0;
    while(received<message_count){
        queue.wait_and_pop(value);
        cout<<"consumer got: " <<value<<endl;
        received++;
    }
}

int main(){
    MutexQueue queue;
    // queue.push(10);
    // queue.push(20);
    // queue.push(30);
    
    // int value = 0;
    // while(queue.safe_pop(value)){
    //     cout<<value<<endl;
    // }
    // cout<<"queue is empty!"<<endl;

    // ref(): 要求 thread 不要 copy，要借用原物件
    thread producer_thread(produce, ref(queue));
    thread consumer_thread(consume, ref(queue));

    producer_thread.join();
    consumer_thread.join();

    cout << "all messages processed" << endl;

    return 0;
}
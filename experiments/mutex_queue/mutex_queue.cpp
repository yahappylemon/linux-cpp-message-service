#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
using namespace std;

class MutexQueue{
public:
    bool push(int value){
        // 故意加上一個scope，先解鎖再通知，避免thread收到通知時，mutex還沒解鎖
        // 「確認還沒 close」與「把資料放入 queue」是一個不可被其他 thread 插隊的整體動作
        {
            lock_guard<mutex> guard(mutex_);

            // 不能放在 lock 範圍外面，因為讀取時 closed_ 狀態可能剛好改變
            if(closed_){
                return false;
            }

            items_.push(value);
        }
        cv_.notify_one();
        return true;
    }
    void close(){
        {
            lock_guard<mutex> guard(mutex_);
            // closed_ == true ：之後不會有新資料，空 queue 時應離開
            closed_ = true;
        }
        cv_.notify_all();
    }
    bool wait_and_pop(int &value){
        // unique_lock 的規則：可以被 condition_variable::wait 操作
        unique_lock<mutex> guard(mutex_);
        // predicate：「停止等待的條件」，closed_ || !items_.empty()
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
private:
    queue<int> items_;
    mutex mutex_;
    condition_variable cv_;
    // closed_ == false：之後可能還有新資料，空 queue 時應等待
    bool closed_ = false;
};

const int message_count = 5;
mutex output_mutex;

void produce(MutexQueue &queue){
    for(int i=1;i<=message_count;i++){
        queue.push(i);
    }
    queue.close();
}

void consume(MutexQueue &queue, int consumer_id){
    int value = 0;
    while(queue.wait_and_pop(value)){
        {
            lock_guard<mutex> guard(output_mutex);
            cout<<"consumer "<<consumer_id<<" got: "<<value<<endl;
        }
    }
    {
        lock_guard<mutex> guard(output_mutex);
        cout << "consumer "<<consumer_id<<" finished" << endl;  
    }
}

int main(){
    MutexQueue queue;
    // 測試「close 時已有資料仍會全部處理完」：
    // 暫時註解 producer_thread，consumer 建立後會依序 pop ，最後才結束
    // queue.push(10);
    // queue.push(20);
    // queue.push(30);
    // queue.close();

    // ref(): 要求 thread 不要 copy，要借用原物件
    thread producer_thread(produce, ref(queue));
    thread consumer_thread_1(consume, ref(queue), 1);
    thread consumer_thread_2(consume, ref(queue), 2);

    producer_thread.join();
    // 測試「空 queue close 不會卡住」：
    // 暫時註解 producer_thread，走到 if(items_.empty() && closed_)結束
    // queue.close();
    consumer_thread_1.join();
    consumer_thread_2.join();

    cout << "all messages processed" << endl;
    return 0;
}
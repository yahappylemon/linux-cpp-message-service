// header 即使被多個檔案 include，也只會被編譯器納入一次
#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

class ThreadSafeQueue{
public:
    bool push(int value);
    bool wait_and_pop(int &value);
    void close();
private:
    std::queue<int> items_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool closed_ = false;
};

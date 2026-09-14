#include <iostream>
#include <thread>
using namespace std;

int counter = 0;
constexpr int increments_per_thread = 100000;

void increment_without_mutex() {
    for (int i = 0; i < increments_per_thread; ++i) {
        int value = counter;          // 讀 shared counter
        this_thread::yield();    // 故意增加兩個 thread 交錯的機會
        counter = value + 1;          // 寫回 shared counter
    }
}

int main() {
    thread first(increment_without_mutex);
    thread second(increment_without_mutex);

    first.join();
    second.join();

    cout << "Expected: " << increments_per_thread * 2 << '\n';
    cout << "Actual:   " << counter << '\n';
}

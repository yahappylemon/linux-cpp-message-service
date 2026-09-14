#include <iostream>
#include <mutex>
#include <thread>
using namespace std;

int counter = 0;
constexpr int increments_per_thread = 100000;
mutex counter_mutex;

void increment_with_mutex() {
    for (int i = 0; i < increments_per_thread; ++i) {
        lock_guard<mutex> lock(counter_mutex);
        ++counter;
    }
}

int main() {
    thread first(increment_with_mutex);
    thread second(increment_with_mutex);

    first.join();
    second.join();

    cout << "Expected: " << increments_per_thread * 2 << '\n';
    cout << "Actual:   " << counter << '\n';
}

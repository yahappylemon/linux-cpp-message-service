#include <iostream>
#include <queue>
using namespace std;

class SimpleQueue{
public:
    void push(int value){
        items_.push(value);
        return;
    }
    bool pop(int &value){
        if(items_.empty()){
            return false;
        }
        value = items_.front();
        items_.pop();
        return true;
    }
private:
    queue<int> items_;
};


int main(){
    SimpleQueue queue;

    queue.push(10);
    queue.push(20);
    queue.push(30);

    int value = 0;

    while(queue.pop(value)){
        cout<<value<<endl;
    }

    cout << "queue is empty!"<<endl;

    return 0;
}
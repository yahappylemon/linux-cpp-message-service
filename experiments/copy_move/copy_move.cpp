#include <iostream>
#include <string>
#include <utility>
using namespace std;

class Message{
public:
    Message(string text): text_(text){
        cout<<"created: "<<text_<<endl;
    }

    Message(const Message &message): text_(message.text_){
        cout<<"copied: "<<text_<<endl;
    }

    Message(Message &&message): text_(std::move(message.text_)){
        cout<<"moved: "<<text_<<endl;
    }

    ~Message(){
        cout<<"destroyed"<<endl;
    }

private:
    string text_;

};

int main(){
    Message first("hello");
    // 兩邊都需要資料 → copy
    Message second = first;
    // 舊地方不需要、但新地方需要資料 → move
    Message third = std::move(first);
    return 0;
}
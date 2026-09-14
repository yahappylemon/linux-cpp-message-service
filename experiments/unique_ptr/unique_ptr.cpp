#include <iostream>
#include <string>
#include <memory>
#include <utility>
using namespace std;

class Message{
public:
    Message(string txt):text_(txt){
        cout<<"create: "<<text_<<endl;
    };
    ~Message(){
        cout<<"destroy: "<<text_<<endl;
    };
    void getMessage(){
        cout<<"printing: "<<text_<<endl;
    }
private:
    string text_;
};

void consume(unique_ptr<Message> msg){
    msg->getMessage();
}

int main(){
    unique_ptr<Message> msg = make_unique<Message>("hello");

    // 把 msg 從「有名稱、通常不能隨便拿走內容的變數」轉成「允許別人接手資源的值」
    consume(std::move(msg));

    // 移動後來源會變成空的 nullptr
    if(msg==nullptr){
        cout<<"transferred"<<endl;
    }
    return 0;
}
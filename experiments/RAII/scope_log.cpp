#include <iostream>
using namespace std;

class ScopeLog {
public:
    ScopeLog() {
        cout << "constructor\n";
    }

    ~ScopeLog() {
        cout << "destructor\n";
    }
};

void test(bool leave_early) {
    ScopeLog log;

    cout << "inside function\n";

    if (leave_early) {
        cout << "early return\n";
        return;
    }

    cout << "normal return\n";
}

int main() {
    test(true);
    cout << "---\n";
    test(false);
}
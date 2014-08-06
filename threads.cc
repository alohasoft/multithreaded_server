#include <string>
#include <iostream>
#include <thread>

void f() {
    std::cout << "Thread with a function." << std::endl;
}

struct c {
    explicit c(const std::string& s) : s_(s) {
    }
    void operator()() const {
        std::cout << "Thread within a class: " << s_ << std::endl;
    }
    std::string s_;
};

int main() {
    std::thread(f).join();
    std::thread(c("foo")).join();
    std::thread(c("bar")).join();
    std::cout << "Main thread." << std::endl;
    return 0;
}

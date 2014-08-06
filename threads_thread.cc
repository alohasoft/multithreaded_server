#include <string>
#include <iostream>
#include <thread>
#include <mutex>

std::mutex mutex;

void f() {
    std::lock_guard<std::mutex> lock(mutex);
    std::cout << "Thread with a function." << std::endl;
}

struct c {
    explicit c(const std::string& s, size_t ms) : s_(s), ms_(ms) {
    }
    void operator()() const {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms_));
        {
            std::lock_guard<std::mutex> lock(mutex);
            std::cout << "Thread within a class: " << s_ << std::endl;
        }
    }
    std::string s_;
    size_t ms_;
};

int main() {
    std::thread t1(f);
    std::thread t2(c("bar", 125));
    std::thread t3(c("foo", 75));
    {
        std::lock_guard<std::mutex> lock(mutex);
        std::cout << "Main thread." << std::endl;
    }
    t1.join();
    t2.join();
    t3.join();
    return 0;
}

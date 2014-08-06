#include <string>
#include <iostream>
#include <future>
#include <mutex>

std::mutex mutex;

void f() {
    std::lock_guard<std::mutex> lock(mutex);
    std::cout << "Thread with a function." << std::endl;
}

void g(const std::string& s, size_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    {
        std::lock_guard<std::mutex> lock(mutex);
        std::cout << "Thread within a class: " << s << std::endl;
    }
}

int main() {
    std::future<void> f1 = std::async(std::launch::async, f);
    std::future<void> f3 = std::async(std::launch::async, g, "bar", 125);
    std::future<void> f2 = std::async(std::launch::async, g, "foo", 75);
    {
        std::lock_guard<std::mutex> lock(mutex);
        std::cout << "Main thread." << std::endl;
    }
    f1.wait();
    f2.wait();
    f3.wait();
    return 0;
}

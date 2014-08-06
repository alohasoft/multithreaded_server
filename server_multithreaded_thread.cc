// Multithreaded server.
// Test with `telnet localhost 8080` started concurrently from multiple terminals.

#include <string>
#include <iostream>
#include <thread>

#include <boost/asio.hpp>

#include "handler.h"

using boost::asio::ip::tcp;

struct request_handler {
    explicit request_handler(std::unique_ptr<tcp::socket>&& socket) : socket_(std::move(socket)) {
    }
    void operator()() const {
        handle_request_sync(*socket_.get());
    }
    std::unique_ptr<tcp::socket> socket_;
};

int main() {
    try {
        boost::asio::io_service io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 8080));
        for (;;) {
            std::unique_ptr<tcp::socket> socket(new tcp::socket(io_service));
            acceptor.accept(*socket.get());
            std::thread(request_handler(std::move(socket))).detach();
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

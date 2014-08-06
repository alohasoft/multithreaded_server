// Single-threaded server.
// Test with `telnet localhost 8080`.

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include <boost/asio.hpp>

#include "handler.h"

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_service io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 8080));
        for (;;) {
            tcp::socket socket(io_service);
            acceptor.accept(socket);
            handle_request_sync(socket);
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

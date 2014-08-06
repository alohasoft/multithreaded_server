// Multithreaded server.
// Test with `telnet localhost 8080` started concurrently from multiple terminals.

#include <iostream>
#include <thread>

#include <boost/asio.hpp>

#include "handler.h"

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_service io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 8080));
        for (;;) {
            std::unique_ptr<tcp::socket> socket(new tcp::socket(io_service));
            acceptor.accept(*socket);
            std::thread(handle_request_sync<std::unique_ptr<tcp::socket>>, std::move(socket)).detach();
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

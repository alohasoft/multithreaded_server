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
        request_handler handler;
        for (;;) {
            std::unique_ptr<tcp::socket> socket(new tcp::socket(io_service));
            acceptor.accept(*socket);
            std::thread(&request_handler::handle_request_sync, &handler, std::move(socket)).detach();
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

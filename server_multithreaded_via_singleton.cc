// Multithreaded server.
// Test with `telnet localhost 8080` started concurrently from multiple terminals.

#include <iostream>
#include <thread>

#include <boost/asio.hpp>

#include "handler.h"

struct TCPServer {
    using tcp = boost::asio::ip::tcp;

    struct PerPortConnectionAccepter {
        boost::asio::io_service io_service;
        tcp::acceptor acceptor;
        PerPortConnectionAccepter(size_t port) : io_service(), acceptor(io_service, tcp::endpoint(tcp::v4(), port)) {
            std::thread(&PerPortConnectionAccepter::ServingThread, this).detach();
        }
        void ServingThread() {
            request_handler handler;
            for (;;) {
                std::unique_ptr<tcp::socket> socket(new tcp::socket(io_service));
                acceptor.accept(*socket);
                std::thread(&request_handler::handle_request_sync, &handler, std::move(socket)).detach();
            }
        }
        PerPortConnectionAccepter() = delete;
        PerPortConnectionAccepter(const PerPortConnectionAccepter&) = delete;
        void operator=(const PerPortConnectionAccepter&) = delete;
    };

    std::map<size_t, std::unique_ptr<PerPortConnectionAccepter>> by_port_;

    PerPortConnectionAccepter& operator[](size_t port) {
        std::unique_ptr<PerPortConnectionAccepter>& ref = by_port_[port];
        if (!ref) {
            std::cout << "Creating server on port " << port << std::endl;
            ref.reset(new PerPortConnectionAccepter(port));
            std::cout << "Created server on port " << port << std::endl;
        }
        return *ref;
    }

    static TCPServer& Instance() {
        static TCPServer instance;
        return instance;
    }
};

int main() {
    TCPServer::Instance()[8080];
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    return 0;
}

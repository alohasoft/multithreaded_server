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

        struct Handler {
            virtual void HandleRequest(std::unique_ptr<tcp::socket>&& socket) = 0;
        };
        template <typename T> struct UserHandlerWrapper : Handler {
            T& user_handler;
            explicit UserHandlerWrapper(T& user_handler) : user_handler(user_handler) {
            }
            virtual void HandleRequest(std::unique_ptr<tcp::socket>&& socket) {
                user_handler.HandleRequestSync(std::move(socket));
            }
            UserHandlerWrapper() = delete;
            UserHandlerWrapper(const UserHandlerWrapper&) = delete;
            void operator=(const UserHandlerWrapper&) = delete;
        };
        std::unique_ptr<Handler> current_handler;

        PerPortConnectionAccepter(size_t port) : io_service(), acceptor(io_service, tcp::endpoint(tcp::v4(), port)) {
            std::thread(&PerPortConnectionAccepter::ServingThread, this).detach();
        }
        void ServingThread() {
            for (;;) {
                std::unique_ptr<tcp::socket> socket(new tcp::socket(io_service));
                acceptor.accept(*socket);
                if (current_handler) {
                    std::thread(&Handler::HandleRequest, current_handler.get(), std::move(socket)).detach();
                } else {
                    std::string message = "500\n";
                    boost::asio::write(
                        *socket, boost::asio::buffer(message), boost::asio::transfer_all());  //, ignored_error);
                }
            }
        }
        PerPortConnectionAccepter() = delete;
        PerPortConnectionAccepter(const PerPortConnectionAccepter&) = delete;
        void operator=(const PerPortConnectionAccepter&) = delete;

        template <typename T> void RegisterHandler(T& handler) {
            assert(!current_handler);
            current_handler.reset(new UserHandlerWrapper<T>(handler));
        }

        void UnregisterHandler() {
            assert(current_handler);
            current_handler.reset(nullptr);
            assert(!current_handler);
        }
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

    struct ScopedHandlerRegisterer {
        size_t port;
        template <typename T> ScopedHandlerRegisterer(size_t port, T& handler) : port(port) {
            Instance()[port].RegisterHandler<T>(handler);
        }
        ~ScopedHandlerRegisterer() {
            Instance()[port].UnregisterHandler();
        }
    };
};

struct HTTPRequestHandler {
    std::string prefix;
    HTTPRequestHandler(const std::string& prefix = "Echo") : prefix(prefix) {
    }
    void HandleRequestSync(std::unique_ptr<tcp::socket>&& socket) {
        std::string user_message;
        char c;
        while (true) {
            boost::asio::read(*socket, boost::asio::buffer(&c, 1), boost::asio::transfer_all());
            if (c == '\n' || c == '\r') {
                break;
            }
            user_message += c;
        }

        const std::string html =
            "<!doctype html>\n"
            "<h1>Hello, World!</h1>\n"
            "<h2>" +
            prefix +
            "</h2>\n"
            "<pre>" +
            user_message + "</pre>\n";

        std::ostringstream os;
        os << "HTTP/1.1 200 OK\n";
        os << "Content-type: text/html\n";
        os << "Content-length: " << html.length() << "\n";
        os << "\n";
        os << html;

        const std::string message = os.str();
        boost::asio::write(*socket, boost::asio::buffer(message), boost::asio::transfer_all());
    }
};

int main() {
    HTTPRequestHandler handler1("foo");
    HTTPRequestHandler handler2("bar");
    HTTPRequestHandler handler3("baz");

    TCPServer::Instance()[8080].RegisterHandler(handler1);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    TCPServer::Instance()[8080].UnregisterHandler();

    {
        TCPServer::ScopedHandlerRegisterer scope(8080, handler2);
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }

    TCPServer::Instance()[8080].RegisterHandler(handler3);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    TCPServer::Instance()[8080].UnregisterHandler();

    return 0;
}

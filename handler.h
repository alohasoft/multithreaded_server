#ifndef HANDLER_H
#define HANDLER_H

#include <sstream>
#include <string>
#include <chrono>
#include <thread>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

struct request_handler {
    std::string prefix;
    request_handler(const std::string& prefix = "Echo") : prefix(prefix) {
    }
    void handle_request_sync(std::unique_ptr<tcp::socket>&& socket) {
        boost::system::error_code ignored_error;
        {
            std::string message = "Type a string to echo first: ";
            boost::asio::write(*socket, boost::asio::buffer(message), boost::asio::transfer_all(), ignored_error);
        }
        {
            std::string s;
            char c;
            while (true) {
                boost::asio::read(*socket, boost::asio::buffer(&c, 1), boost::asio::transfer_all(), ignored_error);
                if (c == '\n' || c == '\r') {
                    break;
                }
                s += c;
            }
            s = prefix + ": " + s + "\n";
            boost::asio::write(*socket, boost::asio::buffer(s), boost::asio::transfer_all(), ignored_error);
        }
        for (int i = 5; i > 0; --i) {
            std::ostringstream os;
            os << "Done in " << i << " seconds.\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            std::string message = os.str();
            boost::asio::write(*socket, boost::asio::buffer(message), boost::asio::transfer_all(), ignored_error);
        }
    }
};

#endif

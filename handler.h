#ifndef HANDLER_H
#define HANDLER_H

#include <sstream>
#include <string>
#include <chrono>
#include <thread>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

// `socket` should support dereferencing. Accepts pointer or unique_ptr<>.
template <typename T> void handle_request_sync(T&& socket) {
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
        s = "Echo: " + s + "\n";
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

#endif

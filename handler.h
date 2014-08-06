#ifndef HANDLER_H
#define HANDLER_H

#include <thread>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

void handle_request_sync(tcp::socket& socket) {
    for (int i = 1; i <= 10; ++i) {
        std::ostringstream os;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        os << "Output " << i << " "
           << "\n";
        std::string message = os.str();
        boost::system::error_code ignored_error;
        boost::asio::write(socket, boost::asio::buffer(message), boost::asio::transfer_all(), ignored_error);
    }
}

#endif

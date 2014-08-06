// Single-threaded server.
// Test with `telnet localhost 8080`.

#include <string>
#include <iostream>
#include <thread>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_service io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 8080));
        for (;;) {
            tcp::socket socket(io_service);
            acceptor.accept(socket);

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
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

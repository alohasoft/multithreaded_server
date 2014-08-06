#include <iostream>

#include <boost/asio.hpp>

int main() {
    try {
        boost::asio::io_service io_service;

        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), "localhost", "8080");
        boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);

        boost::asio::ip::tcp::socket socket(io_service);
        socket.connect(*iterator);

        std::string message = "ACK\n";
        boost::asio::write(socket, boost::asio::buffer(message));

        bool done = false;
        while (!done) {
            char c;
            boost::system::error_code error;
            size_t length = socket.read_some(boost::asio::buffer(&c, 1), error);
            if (error == boost::asio::error::eof) {
                done = true;
            } else if (error) {
                throw boost::system::system_error(error);
            } else if (length != 1) {
                throw std::logic_error("Must have read one byte.");
            }
            std::cout << c;
        }
        std::cout << std::endl;

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

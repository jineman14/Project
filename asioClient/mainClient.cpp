#include <iostream>
#include "boost//asio.hpp"

const char SERVER_IP[] = "127.0.0.1";
const uint32_t PORT_NUMBER = 31400;

int main()
{
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(SERVER_IP), PORT_NUMBER);

    boost::system::error_code connect_error;
    boost::asio::ip::tcp::socket socket(io_service);
    socket.connect(endpoint, connect_error);

    if (connect_error)
    {
        std::cout << "연결 실패 error no: " << connect_error.value() << ", Message: " << connect_error.message() << std::endl;
    }

    getchar();
    return 0;
}
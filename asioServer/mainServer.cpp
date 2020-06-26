#include <iostream>
#include "boost/asio.hpp"

const char SERVER_IP[] = "127.0.0.1";
const uint32_t PORT_NUMBER = 31400;

int main()
{
    boost::asio::io_context io_service;
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), PORT_NUMBER);
    boost::asio::ip::tcp::acceptor acceptor(io_service, endpoint);

    boost::asio::ip::tcp::socket socket(io_service);
    acceptor.accept(socket);

    std::cout << "클라이언트 접속" << std::endl;

    while (true)
    {
        std::array<char, 128> buf;
        buf.fill(0);
        boost::system::error_code error;
        // read message (sync)
        size_t len = socket.read_some(boost::asio::buffer(buf), error);

        if (error)
        {
            if (error == boost::asio::error::eof)
            {
                std::cout << "클라이언트와 연결이 끊어졌습니다." << std::endl;
            }
            else
            {
                std::cout << "error no: " << error.value() << " error Message: " << error.message() << std::endl;
            }
            break;
        }

        std::cout << "클라이언트에서 받은 메시지 : " << &buf[0] << std::endl;
        char szMessage[128] = { 0, };
        sprintf_s(szMessage, 128 - 1, "Re::%s", &buf[0]);
        int nMsgLen = static_cast<int>(strnlen_s(szMessage, 128 - 1));

        boost::system::error_code ignored_error;;
        socket.write_some(boost::asio::buffer(szMessage, nMsgLen), ignored_error);
        std::cout << "클라이언트에 보낸 메시지: " << szMessage << std::endl;;
    }

    getchar();
    return 0;
}
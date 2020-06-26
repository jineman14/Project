#include "stdafx.h"
#include "ChatClient.h"

int main()
{
	boost::asio::io_context io_service;

	auto endpoint = boost::asio::ip::tcp::endpoint(
		boost::asio::ip::make_address("127.0.0.1"),
		PORT_NUMBER);

	ChatClient Cliet(io_service);
	Cliet.Connect(endpoint);

	boost::thread thread(boost::bind(&boost::asio::io_context::run, &io_service));


	char szInputMessage[MAX_MESSAGE_LEN * 2] = { 0, };

	while (std::cin.getline(szInputMessage, MAX_MESSAGE_LEN))
	{
		if (strnlen_s(szInputMessage, MAX_MESSAGE_LEN) == 0)
		{
			break;
		}

		if (Cliet.IsConnecting() == false)
		{
			std::cout << "서버와 연결되지 않았습니다" << std::endl;
			continue;
		}

		if (Cliet.IsLogin() == false)
		{
			Packet::LoginReq SendPkt;
			SendPkt.Init();
			strncpy_s(SendPkt.szName, MAX_NAME_LEN, szInputMessage, MAX_NAME_LEN - 1);

			Cliet.PostSend(false, SendPkt.nSize, (char*)&SendPkt);
		}
		else
		{
			Packet::ChatReq SendPkt;
			SendPkt.Init();
			strncpy_s(SendPkt.szMessage, MAX_MESSAGE_LEN, szInputMessage, MAX_MESSAGE_LEN - 1);

			Cliet.PostSend(false, SendPkt.nSize, (char*)&SendPkt);
		}
	}

	io_service.stop();

	Cliet.Close();

	thread.join();

	std::cout << "클라이언트를 종료해 주세요" << std::endl;

	return 0;
}
#pragma once 

#include "Handler.h"

class Session;
class ChatServer
{
public:
	ChatServer( boost::asio::io_context& io_service )
		: m_acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), PORT_NUMBER))
	{
		m_bIsAccepting = false;
	}

	~ChatServer();

	void Init(const int nMaxSessionCount);

	void Start()
	{
		std::cout << "서버 시작....." << std::endl;

		PostAccept();
	}

	void CloseSession(const int nSessionID);
	void ProcessPacket(const int nSessionID, const char*pData);

	SessionPtr GetSession(int sessionId);
	void SendAll(PACKET_HEADER& data, bool bImmediately = false);
private:
	bool PostAccept();

	void handle_accept(Session* pSession, const boost::system::error_code& error);

	int m_nSeqNumber;
	
	bool m_bIsAccepting;

	boost::asio::ip::tcp::acceptor m_acceptor;

	std::vector< Session* > m_SessionList;
	std::deque< int > m_SessionQueue;
	
	Handler handler_;
};




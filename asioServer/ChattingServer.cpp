#include "stdafx.h"
#include "ChattingServer.h"
#include "ServerSession.h"

ChatServer::~ChatServer()
{
	for (size_t i = 0; i < m_SessionList.size(); ++i)
	{
		if (m_SessionList[i]->Socket().is_open())
		{
			m_SessionList[i]->Socket().close();
		}

		delete m_SessionList[i];
	}
}

void ChatServer::Init(const int nMaxSessionCount)
{
	// 빈 세션 미리 할당해두기
	for (int i = 0; i < nMaxSessionCount; ++i)
	{
		Session* pSession = new Session(i, (boost::asio::io_context&)m_acceptor.get_executor().context(), this);
		m_SessionList.push_back(pSession);
		m_SessionQueue.push_back(i);
	}

	handler_.Init(this);
}

void ChatServer::CloseSession(const int nSessionID)
{
	std::cout << "클라이언트 접속 종료. 세션 ID: " << nSessionID << std::endl;

	m_SessionList[nSessionID]->Socket().close();

	m_SessionQueue.push_back(nSessionID);

	if (m_bIsAccepting == false)
	{
		PostAccept();
	}
}

void ChatServer::ProcessPacket(const int nSessionID, const char*pData)
{
	PACKET_HEADER* pheader = (PACKET_HEADER*)pData;
	SessionPtr session = GetSession(nSessionID);

	handler_.Dispatch(pheader->msgId, session, pheader);

	return;
}

SessionPtr ChatServer::GetSession(int sessionId)
{
	if (static_cast<int>(m_SessionList.size()) <= sessionId)
	{
		return nullptr;
	}

	return m_SessionList[sessionId];
}

void ChatServer::SendAll(PACKET_HEADER & data, bool bImmediately /*= false*/)
{
	for (auto session : m_SessionList)
	{
		if (nullptr == session)
		{
			continue;
		}

		if (session->Socket().is_open())
		{
			session->Send(data);
		}
	}
}

bool ChatServer::PostAccept()
{
	if (m_SessionQueue.empty())
	{
		m_bIsAccepting = false;
		return false;
	}

	m_bIsAccepting = true;
	int nSessionID = m_SessionQueue.front();

	m_SessionQueue.pop_front();

	m_acceptor.async_accept(m_SessionList[nSessionID]->Socket(),
		boost::bind(&ChatServer::handle_accept,
			this,
			m_SessionList[nSessionID],
			boost::asio::placeholders::error)
	);

	return true;
}

void ChatServer::handle_accept(Session * pSession, const boost::system::error_code & error)
{
	if (!error)
	{
		std::cout << "클라이언트 접속 성공. SessionID: " << pSession->SessionID() << std::endl;

		pSession->Init();
		pSession->PostReceive();

		PostAccept();
	}
	else
	{
		std::cout << "error No: " << error.value() << " error Message: " << error.message() << std::endl;
	}
}

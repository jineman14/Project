#include "stdafx.h"
#include "Handler.h"
#include "ChattingServer.h"


void Handler::Init(ChatServer * server)
{
	server_ = server;
	dispatcher_.Clear();
	dispatcher_.AddHandler(MessageId::LoginReq, std::bind(&Handler::OnMsg_LoginReq, this, std::placeholders::_1, std::placeholders::_2));
	dispatcher_.AddHandler(MessageId::ChatReq, std::bind(&Handler::OnMsg_ChatReq, this, std::placeholders::_1, std::placeholders::_2));
	dispatcher_.AddHandler(MessageId::AttackReq, std::bind(&Handler::OnMsg_AttackReq, this, std::placeholders::_1, std::placeholders::_2));
}

bool Handler::Dispatch(MSGID msgId, SessionPtr session, PACKET_HEADER * data)
{
	if(nullptr == server_)
	{
		return false;
	}

	Dispatcher< SessionPtr >::HandlerFunction handler;
	if (false == dispatcher_.GetHandler(msgId, handler))
	{
		return false;
	}

	handler(session, data);
	return true;
}

void Handler::OnMsg_LoginReq(SessionPtr session, PACKET_HEADER* data)
{
	if (nullptr == session)
	{
		return;
	}
	Packet::LoginReq* packet = (Packet::LoginReq*)data;
	session->SetName(packet->szName);

	std::cout << "클라이언트 로그인 성공 Name: " << session->GetName() << std::endl;

	Packet::LoginAns ans;
	ans.Init();
	ans.bIsSuccess = true;

	session->Send(ans);
}

void Handler::OnMsg_ChatReq(SessionPtr session, PACKET_HEADER* data)
{
	if (nullptr == server_)
	{
		return;
	}


	if (nullptr == session)
	{
		return;
	}

	Packet::ChatReq* packet = (Packet::ChatReq*)data;

	Packet::ChatNoti noti;
	noti.SetName(session->GetName());
	noti.SetMessage(packet->szMessage);

	server_->SendAll(noti);
}

void Handler::OnMsg_AttackReq(SessionPtr session, PACKET_HEADER * data)
{
	if (nullptr == session)
	{
		return;
	}

	// todo
		// 로그작성
	Json::Value val;
	val["id"] = session->SessionID();
	val["reqType"] = "attack";
	val["regDate"] = _time32(nullptr);
	// 보내기?

	Packet::AttackAns ans;

	session->Send(ans);
}

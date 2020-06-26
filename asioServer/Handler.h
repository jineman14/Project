#pragma once

#include "Dispatcher.h"

class ChatServer;
class Session;
class Handler
{
public:
	Handler() {}
	~Handler() {}

	void Init(ChatServer* server);
	bool Dispatch(MSGID msgId, SessionPtr session, PACKET_HEADER* data);

private:
	void OnMsg_LoginReq(SessionPtr session, PACKET_HEADER* data);
	void OnMsg_ChatReq(SessionPtr session, PACKET_HEADER* data);
	void OnMsg_AttackReq(SessionPtr session, PACKET_HEADER* data);

private:
	Dispatcher<SessionPtr > dispatcher_;
	ChatServer* server_;
};
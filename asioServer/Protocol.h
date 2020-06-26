#pragma once 

const unsigned short PORT_NUMBER = 31452;

const int MAX_RECEIVE_BUFFER_LEN = 512;

const int MAX_NAME_LEN = 17;
const int MAX_MESSAGE_LEN = 129;

using MSGID = unsigned short;

struct PACKET_HEADER
{
	MSGID msgId = 0;
	short nSize = 0;
};

enum MessageId
{
	NONE_MESSAGE = 0,

	// PKT_REQ_IN
	LoginReq = 1,
	// PKT_RES_IN
	LoginAns = 2,

	ChatReq = 3,
	ChatAns = 4,
	ChatNoti = 5,

	AttackReq = 10,
	AttackAns = 11,

	MAX_MESSAGE
};

namespace Packet {
	struct AttackReq : public PACKET_HEADER
	{
		void Init()
		{
			msgId = MessageId::AttackReq;
			nSize = sizeof(AttackReq);
		}
		uint32_t value = 0;
	};

	struct AttackAns : public PACKET_HEADER
	{
		AttackAns()
		{
			msgId = MessageId::AttackAns;
			nSize = sizeof(AttackAns);
		}
	};

	struct LoginReq : public PACKET_HEADER
	{
		void Init()
		{
			msgId = MessageId::LoginReq;
			nSize = sizeof(LoginReq);
			memset(szName, 0, MAX_NAME_LEN);
		}

		char szName[MAX_NAME_LEN];
	};

	struct LoginAns : public PACKET_HEADER
	{
		void Init()
		{
			msgId = MessageId::LoginAns;
			nSize = sizeof(LoginAns);
			bIsSuccess = false;
		}

		bool bIsSuccess;
	};

	struct ChatReq : public PACKET_HEADER
	{
		void Init()
		{
			msgId = MessageId::ChatReq;
			nSize = sizeof(ChatReq);
			memset(szMessage, 0, MAX_MESSAGE_LEN);
		}

		char szMessage[MAX_MESSAGE_LEN];
	};

	struct ChatNoti : public PACKET_HEADER
	{
		ChatNoti()
		{
			msgId = MessageId::ChatNoti;
			nSize = sizeof(ChatNoti);
			memset(szName, 0, MAX_NAME_LEN);
			memset(szMessage, 0, MAX_MESSAGE_LEN);
		}

		void SetName(const char* name)
		{
			strncpy_s(szName, MAX_NAME_LEN, name, MAX_NAME_LEN - 1);
		}
		void SetMessage(const char* msg)
		{
			strncpy_s(szMessage, MAX_MESSAGE_LEN, msg, MAX_MESSAGE_LEN - 1);
		}

		char szName[MAX_NAME_LEN];
		char szMessage[MAX_MESSAGE_LEN];
	};
}
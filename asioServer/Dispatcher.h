#pragma once

#include "ServerSession.h"

template<typename _TID>
class Dispatcher
{
public:
	using HandlerFunction = std::function<void(_TID, PACKET_HEADER*)>;
	using HandlerMap = std::unordered_map<MSGID, HandlerFunction>;

	Dispatcher()
	{
		handlerMap_.clear();
	}
	~Dispatcher() {}

	void AddHandler(MSGID msgId, HandlerFunction handler)
	{
		std::pair<HandlerMap::iterator, bool > ib = handlerMap_.insert(HandlerMap::value_type(msgId, handler));
		if (false == ib.second)
		{
			assert(!"핸들러 중복 등록했음");
		}
	}
	bool RemoveHandler(MSGID msgId)
	{
		auto itr = handlerMap_.find(msgId);
		if (itr != handlerMap_.end())
		{
			handlerMap_.erase(itr);
			return true;
		}
		return false;
	}

	bool GetHandler(MSGID msgId, HandlerFunction& handler)
	{
		auto itr = handlerMap_.find(msgId);
		if (itr != handlerMap_.end())
		{
			handler = itr->second;
			return true;
		}
		return false;
	}

	void Clear()
	{
		handlerMap_.clear();
	}

private:
	HandlerMap handlerMap_;
};
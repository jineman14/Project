#include "stdafx.h"
#include "DBPool.h"

namespace Common {
	sql::Driver* DBPool::sqlDriver_ = nullptr;

	DBPool::~DBPool()
	{
		if (!freeCons_.empty() || !accuCons_.empty())
		{
			Clear();
		}
	}

	size_t DBPool::Init(const std::string & ip, uint16_t port, const std::string & id, const std::string & pw, const std::string & dbName, uint32_t count)
	{
		ip_ = ip;
		port_ = port;
		id_ = id;
		pw_ = pw;
		dbName_ = dbName;
		sessionCount_ = count;

		if (nullptr == sqlDriver_)
		{
			sqlDriver_ = get_driver_instance();
		}

		return 1;
	}

	void DBPool::Clear()
	{
		for (ConnList::iterator i = freeCons_.begin(); i != freeCons_.end(); ++i)
		{
			SQLConnectionPtr con = *i;
			if (con)
			{
				con->Close();
			}
		}
		freeCons_.clear();

		for (ConnList::iterator i = accuCons_.begin(); i != accuCons_.end(); ++i)
		{
			SQLConnectionPtr con = *i;
			if (con)
			{
				con->Close();
			}
		}
		accuCons_.clear();

		ip_.clear();
		port_ = 0;
		id_.clear();
		pw_.clear();
		dbName_.clear();
		sessionCount_ = 0;
	}

	void DBPool::ReturnConnection(SQLConnectionPtr con, bool reconnect)
	{
		if (reconnect)
		{
			if (con->IsMasterDB())
			{
				thisThreadSpecificPool->Delete(dbIndex_);
			}
			else
			{
				thisThreadSpecificSlavePool->Delete(dbIndex_);
			}
		}
	}

	SQLConnectionPtr DBPool::createConnection()
	{
		try
		{
			std::stringstream ss;
			ss << "tcp://" << ip_ << ":" << port_;
			sql::ConnectOptionsMap connOptionMap;

			connOptionMap["userName"] = sql::SQLString(id_);
			connOptionMap["hostName"] = sql::SQLString(ss.str());
			connOptionMap["password"] = sql::SQLString(pw_);
			connOptionMap["schema"] = sql::SQLString(dbName_);
			connOptionMap[sql::SQLString("CLIENT_FOUND_ROWS")] = true;
			connOptionMap["OPT_RECONNECT"] = true;

			SQLConnectionPtr con = std::make_shared<SQLConnection>(sqlDriver_->connect(connOptionMap));
			con->Init();

			return con;
		}
		catch (sql::SQLException& e)
		{
			std::cout << L"DBPool::createConnection: sql exception occured!! [errorCode:" << e.getErrorCode() << L"][errorMsg:" << e.what() << L"]" << std::endl;
			return SQLConnectionPtr();
		}
		catch (std::exception& e)
		{
			std::cout << L"DBPool::createConnection: std::exception occured!! [what:" << e.what() << L"]" << std::endl;
			return SQLConnectionPtr();
		}
		catch (...)
		{
			std::cout << L"DBPool::createConnection: exception occured!! - ignore." << std::endl;
			return SQLConnectionPtr();
		}
	}

	// ==[ThreadSpecificDBConnections]==

	bool ThreadSpecificDBConnections::Add(uint32_t index, DBPoolPtr pool, SQLConnectionPtr connection)
	{
		assert(threadId_ == ::GetCurrentThreadId());

		Delete(index);

		std::pair< ConnectionMap::iterator, bool > result =
			connections_.insert(std::make_pair(index, std::make_pair(pool, connection)));
		assert(result.second);

		return result.second;
	}

	void ThreadSpecificDBConnections::Delete(uint32_t index)
	{
		assert(threadId_ == ::GetCurrentThreadId());

		connections_.erase(index);
	}

	std::pair<DBPoolPtr, SQLConnectionPtr> ThreadSpecificDBConnections::Get(uint32_t index)
	{
		assert(threadId_ == ::GetCurrentThreadId());

		ConnectionMap::iterator it = connections_.find(index);
		if (it != connections_.end())
		{
			return (*it).second;
		}

		return std::make_pair(DBPoolPtr(), SQLConnectionPtr());
	}

	void ThreadSpecificDBConnections::GetAll(std::vector<std::pair<uint32_t, SQLConnectionPtr>>& connections)
	{
		assert(threadId_ == ::GetCurrentThreadId());

		for (ConnectionMap::iterator it = connections_.begin(); it != connections_.end(); ++it)
		{
			connections.push_back(std::make_pair((*it).first, (*it).second.second));
		}
	}
}

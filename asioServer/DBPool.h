#pragma once

#include "SQLConnection.h"
#include <boost/noncopyable.hpp>

namespace Common {
	class DBPool
	{
		typedef std::list<SQLConnectionPtr> ConnList;
	public:
		DBPool(uint32_t dbIndex) : dbIndex_(dbIndex) {}
		~DBPool();

		size_t Init(const std::string& ip, uint16_t port, const std::string& id, const std::string& pw, const std::string& dbName, uint32_t count);
		void Clear();

		void ReturnConnection(SQLConnectionPtr con, bool reconnect = false);
		uint32_t GetConnectionCount() { return sessionCount_; }

		SQLConnectionPtr createConnection();
	private:
		uint32_t dbIndex_ = 0;
		std::string ip_ = "";
		uint16_t port_ = 0;

		std::string id_ = "";			// login id
		std::string pw_ = "";			// login password
		std::string dbName_ = "";		// database name
		uint32_t sessionCount_ = 0;		// db connection count

		ConnList freeCons_;		// ready to use
		ConnList accuCons_;		// now using

		static sql::Driver* sqlDriver_;
	};
	typedef std::shared_ptr<DBPool> DBPoolPtr;

	// Thread Local Storage
	class ThreadSpecificDBConnections : boost::noncopyable
	{
	public:

		ThreadSpecificDBConnections(DWORD threadId) :
			threadId_(threadId)
		{
			connections_.reserve(100);
		}

		~ThreadSpecificDBConnections()
		{
		}

		bool Add(uint32_t index, DBPoolPtr pool, SQLConnectionPtr connection);
		void Delete(uint32_t index);
		std::pair< DBPoolPtr, SQLConnectionPtr > Get(uint32_t index);
		void GetAll(std::vector< std::pair< uint32_t, SQLConnectionPtr > >& connections);
		

	private:

		typedef std::unordered_map<uint32_t, std::pair< DBPoolPtr, SQLConnectionPtr> > ConnectionMap;

		DWORD threadId_;
		ConnectionMap connections_;
	};
	static boost::thread_specific_ptr<ThreadSpecificDBConnections> thisThreadSpecificPool;
	static boost::thread_specific_ptr<ThreadSpecificDBConnections> thisThreadSpecificSlavePool;

	static void ThisThreadSpecificPoolLazyCreate(bool isMaster)
	{
		if (isMaster)
		{
			if (NULL == thisThreadSpecificPool.get())
			{
				thisThreadSpecificPool.reset(new ThreadSpecificDBConnections(::GetCurrentThreadId()));
			}
		}
		else
		{
			if (NULL == thisThreadSpecificSlavePool.get())
			{
				thisThreadSpecificSlavePool.reset(new ThreadSpecificDBConnections(::GetCurrentThreadId()));
			}
		}
	}

	static void ThisThreadDBConnections(std::vector< std::pair< uint32_t, SQLConnectionPtr > >& connections, bool isMaster)
	{
		ThisThreadSpecificPoolLazyCreate(isMaster);

		if (isMaster)
		{
			thisThreadSpecificPool->GetAll(connections);
		}
		else
		{
			thisThreadSpecificSlavePool->GetAll(connections);
		}
	}

	static void UnregisterThisThreadDBConnection(uint32_t dbIndex, bool isMaster)
	{
		ThisThreadSpecificPoolLazyCreate(isMaster);

		if (isMaster)
		{
			thisThreadSpecificPool->Delete(dbIndex);
		}
		else
		{
			thisThreadSpecificSlavePool->Delete(dbIndex);
		}
	}
}
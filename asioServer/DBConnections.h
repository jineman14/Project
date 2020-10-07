#pragma once
#include "DBPool.h"

namespace Common {

	// sql error state
	struct LastErrorState
	{
		LastErrorState() : errorCode_(0), needReconnect_(false) { }

		std::string errorString_;	// e.what();
		int errorCode_;				// e.getErrorCode();
		// 재접속을 위한 내부 변수
		bool needReconnect_;
	};

	class DBConnectionBasePtr
	{
	public:

		DBConnectionBasePtr(uint32_t dbIndex, int timeout = INFINITE, bool isSlave = false);
		DBConnectionBasePtr(SQLConnectionPtr connection) :
			con_(connection)
		{
		}
		virtual ~DBConnectionBasePtr();

		bool IsConnected() { return pool_ && con_; }
		void CheckReconnect(sql::SQLException& e)
		{
			if (e.getErrorCode() == 2006 || e.getErrorCode() == 2013)
			{
				needReconnect_ = true;
			}
		}

		SQLConnectionPtr operator->() { return con_; }
		uint32_t GetDBIndex() { return dbIndex_; }
		DBPoolPtr GetDBPool() { return pool_; }

		bool IsValid() const
		{
			return nullptr != con_ && !con_->IsClosed();
		}

	protected:
		void initMaster(int timeout);
		void initSlave(int timeout);

	protected:
		SQLConnectionPtr con_ = nullptr;
		DBPoolPtr pool_;
		uint32_t dbIndex_ = 0;
		bool needReconnect_ = false;
		LastErrorState errorState_;
	};

	class GlobalDBConnectionPtr : public DBConnectionBasePtr
	{
	public:
		GlobalDBConnectionPtr(int timeout = INFINITE, bool isSlave = false) : DBConnectionBasePtr(0, timeout, isSlave) {}

		GlobalDBConnectionPtr(SQLConnectionPtr connection) : DBConnectionBasePtr(connection) { }
	};

	class DBConnectionPtr : public DBConnectionBasePtr
	{
	public:
		DBConnectionPtr(uint32_t dbIndex, int timeout = INFINITE, bool isSlave = false) : DBConnectionBasePtr(dbIndex, timeout, isSlave) {}

		DBConnectionPtr(SQLConnectionPtr connection) : DBConnectionBasePtr(connection) { }
	};

	// Slave
	class SlaveGlobalDBConnectionPtr : public GlobalDBConnectionPtr
	{
	public:
		SlaveGlobalDBConnectionPtr(int timeout = INFINITE) : GlobalDBConnectionPtr(timeout, true) {}
	};

	class SlaveDBConnectionPtr : public DBConnectionPtr
	{
	public:
		SlaveDBConnectionPtr(uint32_t dbIndex, int timeout = INFINITE) : DBConnectionPtr(dbIndex, timeout, true) {}
	};
}
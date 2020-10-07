#include "stdafx.h"
#include "DBConnections.h"
#include "DBPoolManager.h"

namespace Common {

	// ==[DBConnectionBasePtr]==

	DBConnectionBasePtr::DBConnectionBasePtr(uint32_t dbIndex, int timeout, bool isSlave) :
		dbIndex_(dbIndex)
	{
		if (false == isSlave)
		{
			initMaster(timeout);
		}
		else
		{
			initSlave(timeout);
		}
	}

	DBConnectionBasePtr::~DBConnectionBasePtr()
	{
		if (con_ && pool_)
		{
			errorState_.needReconnect_ = false;

			pool_->ReturnConnection(con_, needReconnect_);
		}

		con_.reset();
		pool_.reset();
	}

	void DBConnectionBasePtr::initMaster(int timeout)
	{
		ThisThreadSpecificPoolLazyCreate(true);

		std::pair<DBPoolPtr, SQLConnectionPtr> cachedConnection = thisThreadSpecificPool->Get(dbIndex_);
		if (NULL != cachedConnection.first && NULL != cachedConnection.second)
		{
			pool_ = cachedConnection.first;
			con_ = cachedConnection.second;

			return;
		}

		pool_ = DBPoolManager::Instance().GetDBPool(dbIndex_);
		if (!pool_ && dbIndex_ > 0)
		{
			// TODO
		}

		while (!pool_)
		{
			pool_ = DBPoolManager::Instance().GetDBPool(dbIndex_);

			if (NULL == pool_)
			{
				::Sleep(10);
			}

		}

		con_ = pool_->createConnection();
		if (NULL != con_)
		{
			con_->SetMasterDB(true);

			thisThreadSpecificPool->Add(dbIndex_, pool_, con_);
		}
	}

	void DBConnectionBasePtr::initSlave(int timeout)
	{
		// TODO
	}
}
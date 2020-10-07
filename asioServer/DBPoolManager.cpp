#include "stdafx.h"
#include "DBPoolManager.h"

namespace Common {

	DBPoolManager & DBPoolManager::Instance()
	{
		static DBPoolManager _manager;
		return _manager;
	}

	void DBPoolManager::Init(DBPoolPtr mainDBPool)
	{
		assert(mainDBPool);

		mainDB_ = mainDBPool;

		totalConCount_ += mainDB_->GetConnectionCount();
	}

	void DBPoolManager::Clear()
	{
		if (mainDB_)
		{
			mainDB_->Clear();
		}
		mainDB_.reset();

		if (operationDB_)
		{
			operationDB_->Clear();
		}
		operationDB_.reset();

		CriticalSection::Lock locker(lock_);

		for (int i = 0, count = (int)dbList_.size(); i < count; i++)
		{
			DBPoolShard& pool = dbList_[i];
			pool.pool_->Clear();
		}
		dbList_.clear();

		totalConCount_ = 0;
	}

	void DBPoolManager::SetOperationDBPool(DBPoolPtr dbPool)
	{
		assert(dbPool);

		operationDB_ = dbPool;

		totalConCount_ += operationDB_->GetConnectionCount();
	}

	bool DBPoolManager::Add(DBPoolPtr dbPool, uint32_t dbIndex)
	{
		assert(dbPool);
		assert(dbIndex);

		CriticalSection::Lock locker(lock_);

		// 중복 체크
		int count = (int)dbList_.size();
		for (int i = 0; i < count; i++)
		{
			DBPoolShard& db = dbList_[i];
			if (db.dbIndex_ == dbIndex)
			{
				// 중복
				std::cout << L"[DBPoolManager::Add] db config is overlapped!! [dbIndex:" << dbIndex << "]" << std::endl;
				return false;
			}

			totalConCount_ += db.pool_->GetConnectionCount();
		}

		// 추가
		DBPoolShard shard;
		shard.dbIndex_ = dbIndex;
		shard.pool_ = dbPool;

		dbList_.push_back(shard);
		dbIndexs_.push_back(dbIndex);

		return true;
	}

	DBPoolPtr DBPoolManager::GetDBPool(uint32_t dbIndex)
	{
		CriticalSection::Lock locker(lock_);

		if (dbIndex == 0)
		{
			return mainDB_;
		}
		else if (dbIndex == 1)
		{
			return operationDB_;
		}

		int count = (int)dbList_.size();
		for (int i = 0; i < count; i++)
		{
			DBPoolShard& db = dbList_[i];
			if (db.dbIndex_ == dbIndex)
			{
				return db.pool_;
			}
		}

		return DBPoolPtr();
	}

	uint32_t DBPoolManager::GetTotalConnectionCount()
	{
		CriticalSection::Lock locker(lock_);
		return totalConCount_;
	}

	DBPoolPtr DBPoolManager::GetMainDBPool()
	{
		return GetDBPool(0);
	}

	void DBPoolManager::GetAllDBIndex(std::vector<uint32_t>& dbIndexs)
	{
		CriticalSection::Lock locker(lock_);

		std::copy(dbIndexs_.begin(), dbIndexs_.end(), std::back_inserter(dbIndexs));
	}

	SlaveDBPoolManager & SlaveDBPoolManager::Instance()
	{
		static SlaveDBPoolManager _manager;
		return _manager;
	}
}

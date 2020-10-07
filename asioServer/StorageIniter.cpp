#include "stdafx.h"
#include "DBPoolManager.h"
#include "StorageIniter.h"
#include "DBConnections.h"

namespace Common {

	bool StorageIniter::LoadConfig()
	{
		if (false == LoadConfig_DB())
		{
			return false;
		}

		return true;
	}

	bool StorageIniter::LoadConfig_DB()
	{
		// todo db info
		return false;
	}

	bool StorageIniter::InitGlobalDB()
	{
		Common::DBPoolPtr pool = initDB(dbGlobal_);
		if (nullptr == pool)
		{
			return false;
		}

		dbPoolManager_.Init(pool);

		isInitGlobalDB_ = true;

		return true;
	}

	bool StorageIniter::InitUserDBs()
	{
		GlobalDBConnectionPtr dbConnection;
		if (!dbConnection.IsConnected())
		{
			return false;
		}
		
		// todo
		// db query select userDBs

		std::vector< StorageInfo > infos;
		for (std::size_t i = 0; i < infos.size(); ++i)
		{
			if (0 >= InitNonGlobalDB(infos[i]))
			{
				return false;
			}

		}
		return true;
	}

	bool StorageIniter::InitLogDBs()
	{
		GlobalDBConnectionPtr dbConnection;
		if (!dbConnection.IsConnected())
		{
			return false;
		}

		// todo
		// db query select logDBs
		std::vector< StorageInfo > infos;
		for (std::size_t i = 0; i < infos.size(); ++i)
		{
			if (0 >= InitNonGlobalDB(infos[i]))
			{
				return false;
			}
		}

		return true;
	}

	size_t StorageIniter::InitNonGlobalDB(StorageInfo & info)
	{
		if (info.storageType_ != StorageInfo::DB_USER && info.storageType_ != StorageInfo::DB_LOG)
		{
			return 0;
		}

		// 기존에 없거나, connecting_이 true이면 생성 절차로 넘어감
		{
			CriticalSection::Lock locker(lock_);
			StorageMap::iterator i = dbServers_.find(info.dbIndex_);
			if (i != dbServers_.end())
			{
				StorageInfo& storageInfo = i->second;
				if (false == storageInfo.connecting_)
				{
					// 이미 초기화 완료된 풀
					return storageInfo.poolSize_;
				}

				storageInfo = info;
				storageInfo.connecting_ = true;
			}
			else
			{
				// 신규 생성
				info.connecting_ = true;
				dbServers_[info.dbIndex_] = info;
			}
		}

		{
			// NOTE: id/pass는 global꺼랑 같은거 쓴다.
			info.id_ = dbGlobal_.id_;
			info.password_ = dbGlobal_.password_;

			Common::DBPoolPtr pool = initDB(info);
			if (!pool)
			{
				removeUserDB(info.dbIndex_);
				return 0;
			}

			if (false == dbPoolManager_.Add(pool, info.dbIndex_))
			{
				// NOTE: Add 실패는 dbindex 중복으로 인한거라, 이미 풀이 있는거임. 그래서 removeUserDB 하지 않음.
			}
		}

		// 생성 완료
		{
			CriticalSection::Lock locker(lock_);
			dbServers_[info.dbIndex_].connecting_ = false;
		}
		return info.poolSize_;
	}

	Common::DBPoolPtr StorageIniter::initDB(Common::StorageInfo & info)
	{
		Common::DBPoolPtr pool = std::make_shared<Common::DBPool>(info.dbIndex_);
		if (0 == pool->Init(info.ip_.c_str(), info.port_, info.id_.c_str(), info.password_.c_str(), info.database_.c_str(), static_cast<uint32_t>(info.poolSize_)))
		{
			return Common::DBPoolPtr();
		}

		return pool;
	}

	void StorageIniter::removeUserDB(uint32_t dbIndex)
	{
		CriticalSection::Lock locker(lock_);
		StorageMap::iterator i = dbServers_.find(dbIndex);
		if (i != dbServers_.end())
		{
			StorageInfo& info = i->second;

			dbServers_.erase(i);
		}
	}

	StorageIniter & StorageIniter::Instance()
	{
		static StorageIniter _instance;
		return _instance;
	}

	StorageIniter::StorageIniter() : dbPoolManager_(DBPoolManager::Instance())
	{
	}

	StorageIniter::StorageIniter(Common::DBPoolManager & dbPoolManager) : dbPoolManager_(dbPoolManager)
	{
	}

	SlaveStorageIniter & SlaveStorageIniter::Instance()
	{
		static SlaveStorageIniter _instance(Common::SlaveDBPoolManager::Instance());
		return _instance;
	}
}
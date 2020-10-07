#pragma once
#include "DBPool.h"

namespace Common {
	class DBPoolManager
	{
	public:

		~DBPoolManager() {}

		static DBPoolManager& Instance();

		void Init(DBPoolPtr mainDBPool);

		void Clear();

		void SetOperationDBPool(DBPoolPtr dbPool);

		bool Add(DBPoolPtr dbPool, uint32_t dbIndex);

		DBPoolPtr GetDBPool(uint32_t dbIndex);

		uint32_t GetTotalConnectionCount();

		DBPoolPtr GetMainDBPool();

		void GetAllDBIndex(std::vector<uint32_t>& dbIndexs);

	protected:

		DBPoolManager() {}


		struct DBPoolShard
		{
			DBPoolShard() : dbIndex_(0), connecting_(false) {}

			uint32_t dbIndex_;
			bool connecting_;
			DBPoolPtr pool_;
		};

		DBPoolPtr mainDB_;
		DBPoolPtr operationDB_;
		std::vector<DBPoolShard> dbList_;
		volatile uint32_t totalConCount_ = 0;
		std::vector<uint32_t> dbIndexs_;
		CriticalSection lock_;
	};

	class SlaveDBPoolManager : public DBPoolManager
	{
	public:
		static SlaveDBPoolManager& Instance();
		
	};
}
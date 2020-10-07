#pragma once


namespace Common {
	struct StorageInfo
	{
		StorageInfo() {}

		enum StorageType
		{
			STORAGE_NONE = 0,
			DB_GLOBAL,
			DB_USER,
			DB_LOG
		};

		bool SetStorageType(const std::string& dbType)
		{
			if (dbType == "L")
			{
				storageType_ = StorageInfo::DB_LOG;
			}
			else if (dbType == "U")
			{
				storageType_ = StorageInfo::DB_USER;
			}
			else
			{
				return false;
			}

			return true;

		}
		uint32_t dbIndex_ = 0;
		std::string ip_ = "";
		uint16_t port_ = 0;
		std::string id_ = "";
		std::string password_ = "";
		std::string database_ = "";
		size_t poolSize_ = 0;
		StorageType storageType_ = STORAGE_NONE;
		volatile bool connecting_ = false;	// 풀생성중이라 기다려야 함
	};

	class StorageIniter
	{
		friend class SlaveStorageIniter;

	public:
		static StorageIniter& Instance();

		bool LoadConfig();
		virtual bool LoadConfig_DB();

		virtual bool InitGlobalDB();
		virtual bool InitUserDBs();
		virtual bool InitLogDBs();
		virtual size_t InitNonGlobalDB(StorageInfo& info);

	protected:
		StorageIniter();
		StorageIniter(Common::DBPoolManager& dbPoolManager);

		Common::DBPoolPtr initDB(Common::StorageInfo& info);
		void removeUserDB(uint32_t dbIndex);

	protected:
		bool isInitGlobalDB_ = false;
		StorageInfo dbGlobal_;
		
		using StorageMap = std::map<uint32_t, StorageInfo>;
		StorageMap dbServers_;

		CriticalSection lock_;
		Common::DBPoolManager& dbPoolManager_;
	};

	class SlaveStorageIniter : public StorageIniter
	{
	public:
		static SlaveStorageIniter& Instance();
		SlaveStorageIniter(Common::DBPoolManager& dbPoolManager) : StorageIniter(dbPoolManager) {}

	};
}
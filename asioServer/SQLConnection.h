#pragma once

#include <jdbc/mysql_connection.h>

namespace Common {
	typedef sql::PreparedStatement* PreparedStatementPtr;
	typedef std::shared_ptr<sql::Statement> StatementPtr;

	typedef std::unordered_map<uint32_t, PreparedStatementPtr> PreparedStatements;
	typedef std::unordered_map<uint64_t, PreparedStatementPtr> PreparedStatementsWithCrc64;

	typedef std::shared_ptr<sql::Statement> StatementPtr;

	class SQLConnection
	{
	public:
		SQLConnection(sql::Connection* con);
		virtual ~SQLConnection();

		void Init();

		StatementPtr CreateStatement(const std::string& statement);

		PreparedStatementPtr PrepareStatement(const std::string& statement);
		PreparedStatementPtr PrepareStatement(uint32_t queryId, const char* statement);

		const std::string& GetLastQuery() { return lastQuery_; }

		void Commit() { con_->commit(); }
		void Rollback() { con_->rollback(); }
		void Close() { con_->close(); }
		bool IsClosed() { return con_->isClosed(); }	// 郴何利栏肺 犁立加
		bool IsValid() { return con_->isValid(); }
		bool Reconnect() { return con_->reconnect(); }

		bool IsMasterDB() const
		{
			return isMasterDB_;
		}

		/*void SetTransaction(DBTransaction* transaction)
		{
			transaction_ = transaction;
		}*/

		void SetMasterDB(bool isMasterDB)
		{
			isMasterDB_ = isMasterDB;
		}
	private:
		sql::Connection* con_;

		PreparedStatementsWithCrc64 preparedStatements_;
		PreparedStatements map_;

#if _DEBUG
		std::map<uint32_t, std::string> qmap_;
#endif

		std::string lastQuery_ = "";
		bool isMasterDB_ = false;

		// TODO
		//DBTransaction* transaction_;
	};
	typedef std::shared_ptr<SQLConnection> SQLConnectionPtr;
}
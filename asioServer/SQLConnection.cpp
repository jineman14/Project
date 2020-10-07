#include "stdafx.h"
#include "SQLConnection.h"
#include <jdbc/cppconn/prepared_statement.h>

namespace Common {

	PreparedStatementPtr SQLConnection::PrepareStatement(const std::string & statement)
	{
		assert(!statement.empty());

		lastQuery_ = statement;

		const uint64_t hash = crc64(0, (const unsigned char*)statement.c_str(), statement.length());

		PreparedStatementsWithCrc64::iterator it = preparedStatements_.find(hash);
		if (it != preparedStatements_.end())
		{
			return (*it).second;
		}

		PreparedStatementPtr preparedStatement = con_->prepareStatement(statement.c_str());
		preparedStatements_.insert(PreparedStatementsWithCrc64::value_type(hash, preparedStatement));

		return preparedStatement;
	}

	PreparedStatementPtr SQLConnection::PrepareStatement(uint32_t queryId, const char * statement)
	{
		lastQuery_ = statement;

		PreparedStatementPtr stmt = map_[queryId];
		if (!stmt)
		{
			stmt = con_->prepareStatement(statement);

			map_[queryId] = stmt;
#if _DEBUG
			qmap_.insert(std::map<uint32_t, std::string>::value_type(queryId, lastQuery_));
#endif
		}

#if _DEBUG
		if (qmap_[queryId].compare(lastQuery_))
		{
			assert(!"[SQLConnection][prepareStatement] query conflict!!!");
		}
#endif

		return stmt;
	}

	void SQLConnection::Init()
	{
		con_->setTransactionIsolation(sql::TRANSACTION_READ_COMMITTED);

#ifdef _DEBUG
		sql::enum_transaction_isolation ti = con_->getTransactionIsolation();
		assert(ti == sql::TRANSACTION_READ_COMMITTED);
#endif
	}

	StatementPtr SQLConnection::CreateStatement(const std::string& statement)
	{
		lastQuery_ = statement;

		return StatementPtr(con_->createStatement());
	}

	SQLConnection::SQLConnection(sql::Connection * con) :
		con_(con)
	{
		preparedStatements_.clear();
		preparedStatements_.reserve(100);

		map_.clear();
		map_.reserve(100);
	}

	SQLConnection::~SQLConnection()
	{
		{
			auto it = map_.begin();
			while (it != map_.end())
			{
				delete (*it).second;

				++it;
			}
			map_.clear();
		}
		{
			auto it = preparedStatements_.begin();
			while (it != preparedStatements_.end())
			{
				delete (*it).second;

				++it;
			}
			preparedStatements_.clear();
		}

		delete con_;
	}
}

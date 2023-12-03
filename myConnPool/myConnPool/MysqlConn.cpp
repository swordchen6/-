#include "MysqlConn.h"

MysqlConn::MysqlConn()
{
	m_conn = mysql_init(nullptr);
	mysql_set_character_set(m_conn, "utf8");
}

MysqlConn::~MysqlConn()
{
	if (m_conn != nullptr)
	{
		mysql_close(m_conn);
	}
	freeResult();
}

bool MysqlConn::connect(std::string user, std::string passwd, std::string db, std::string ip, unsigned short port)
{
	MYSQL* ptr = mysql_real_connect(m_conn, ip.c_str(), user.c_str(), passwd.c_str(), db.c_str(), port, NULL, 0);

	return ptr != nullptr;
}

bool MysqlConn::update(std::string sqlstn)
{
	if (mysql_query(m_conn, sqlstn.c_str()))
	{
		return false;
	}
	return true;
}

bool MysqlConn::query(std::string sqlstn)
{
	freeResult();
	if (mysql_query(m_conn, sqlstn.c_str()))
	{
		return false;
	}
	m_result = mysql_store_result(m_conn);
	return true;
}

bool MysqlConn::next()
{
	if (m_result != nullptr)
	{
		m_row = mysql_fetch_row(m_result);
		if (m_row != nullptr)
			return true;
	}
	return false;
}

std::string MysqlConn::value(int index)
{
	int colCount = mysql_num_fields(m_result);
	if (index >= colCount || index < 0)
	{
		return std::string();
	}
	char* val = m_row[index];
	unsigned long length = mysql_fetch_lengths(m_result)[index];
	return std::string(val, length);
}

bool MysqlConn::transaction()
{
	return mysql_autocommit(m_conn, false);
}

bool MysqlConn::commit()
{
	return mysql_commit(m_conn);
}

bool MysqlConn::rollBack()
{
	return mysql_rollback(m_conn);
}

void MysqlConn::refreshAliveTime()
{
	m_alivetime = std::chrono::steady_clock::now();
}

long long MysqlConn::getAliveTime()
{
	std::chrono::nanoseconds res = std::chrono::steady_clock::now() - m_alivetime;
	std::chrono::milliseconds millisec = std::chrono::duration_cast<std::chrono::milliseconds>(res);
	return millisec.count();
}

void MysqlConn::freeResult()
{
	if (m_result)
	{
		mysql_free_result(m_result);
		m_result = nullptr;
	}
}


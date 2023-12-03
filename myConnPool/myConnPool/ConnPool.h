#pragma once
#include<queue>
#include<mutex>
#include<condition_variable>
#include<json/json.h>
#include<fstream>
#include<thread>
#include<memory>
#include"MysqlConn.h"
class ConnPool
{
public:
	static ConnPool* getConnectPool();
	std::shared_ptr<MysqlConn> getConn();
	~ConnPool();
private:
	//数据库服务器ip地址
	std::string m_ip;
	//数据库服务器用户名
	std::string m_user;
	//数据库服务器密码
	std::string m_passwd;
	//数据库服务器数据库名
	std::string m_dbName;
	//数据库服务器端口
	unsigned short m_port;
	//连接池连接的最小数量
	int m_minSize;
	//连接池连接的最大数量
	int m_maxSize;
	//已存的连接数，注意queue里面时连接池有的连接数
	// 而m_size是加上已分配的连接数后连接数
	int m_size;
	//等待连接池生产连接的查看时间
	int m_timeout;
	//连接的最大空闲时间
	int m_maxIdleTime;
	//连接池开关
	bool poolSwitch;
	std::mutex m_mutex;
	std::condition_variable m_cond;
	//连接队列
	std::queue<MysqlConn*> m_connQueue;
	ConnPool();
	ConnPool(const ConnPool& obj)=delete;
	ConnPool& operator=(const ConnPool& obj) = delete;
	//转换json文件
	bool parseJsonFile();
	//生产连接
	void produceConn();
	//回收连接
	void recycleConn();
	//增加连接
	void addConn(); 
};


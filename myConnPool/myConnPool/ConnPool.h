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
	//���ݿ������ip��ַ
	std::string m_ip;
	//���ݿ�������û���
	std::string m_user;
	//���ݿ����������
	std::string m_passwd;
	//���ݿ���������ݿ���
	std::string m_dbName;
	//���ݿ�������˿�
	unsigned short m_port;
	//���ӳ����ӵ���С����
	int m_minSize;
	//���ӳ����ӵ��������
	int m_maxSize;
	//�Ѵ����������ע��queue����ʱ���ӳ��е�������
	// ��m_size�Ǽ����ѷ������������������
	int m_size;
	//�ȴ����ӳ��������ӵĲ鿴ʱ��
	int m_timeout;
	//���ӵ�������ʱ��
	int m_maxIdleTime;
	//���ӳؿ���
	bool poolSwitch;
	std::mutex m_mutex;
	std::condition_variable m_cond;
	//���Ӷ���
	std::queue<MysqlConn*> m_connQueue;
	ConnPool();
	ConnPool(const ConnPool& obj)=delete;
	ConnPool& operator=(const ConnPool& obj) = delete;
	//ת��json�ļ�
	bool parseJsonFile();
	//��������
	void produceConn();
	//��������
	void recycleConn();
	//��������
	void addConn(); 
};


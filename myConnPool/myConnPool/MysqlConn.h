#pragma once
#include<string>
#include <mysql.h>
#include<chrono>
class MysqlConn
{
public:
	//��ʼ�����ݿ�����
	MysqlConn();
	//�ͷ����ݿ�����
	~MysqlConn();
	//�������ݿ�
	bool connect(std::string user, std::string passwd, std::string db,
		std::string ip, unsigned short port = 3306);
	//�������ݿ⣬��ɾ��
	bool update(std::string sqlstn);
	//��ѯ���ݿ�
	bool query(std::string sqlstn);
	//������ѯ�õ��Ľ����
	bool next();
	//�õ�������е��ֶ�ֵ
	std::string value(int index);
	//�������
	bool transaction();
	//�ύ���� 
	bool commit();
	//����ع�
	bool rollBack();
	//ˢ����ʼ�Ŀ���ʱ���
	void refreshAliveTime();
	//�������Ӵ�����ʱ��
	long long getAliveTime();
private:
	MYSQL* m_conn = nullptr;
	MYSQL_RES* m_result = nullptr;
	MYSQL_ROW m_row = nullptr;
	std::chrono::steady_clock::time_point m_alivetime;
	void freeResult();
};




#include <iostream>
#include"ConnPool.h"
using namespace std;
//单线程：使用/不使用数据库连接池
//多线程：使用/不使用数据库连接池
void op1(int begin, int end)
{
	for (int i = begin; i < end; i++)
	{
		MysqlConn conn;
		conn.connect("root", "1234", "test", "192.168.188.88");
		char sql[1024] = { 0 };
		sprintf(sql, "insert into person values(%d,25,'man','tom')", i);
		conn.update(sql);
	}
}
void op2(ConnPool* pool,int begin, int end)
{
	for (int i = begin; i < end; i++)
	{
		shared_ptr<MysqlConn> conn = pool->getConn();
		char sql[1024] = { 0 };
		sprintf(sql, "insert into person values(%d,25,'man','tom')", i);
		conn->update(sql);
	}
}
void test1()
{
#if 0
	//35290640100纳秒
	chrono::steady_clock::time_point begin = chrono::steady_clock::now();
	op1(0, 5000);
	chrono::steady_clock::time_point end = chrono::steady_clock::now();
	auto length = end - begin;
	std::cout << "非连接池，单线程，用时：" << length.count() << "纳秒" << endl;
#else
	//3658401400纳秒
	ConnPool* pool = ConnPool::getConnectPool();
	chrono::steady_clock::time_point begin = chrono::steady_clock::now();
	op2(pool,0, 5000);
	chrono::steady_clock::time_point end = chrono::steady_clock::now();
	auto length = end - begin;
	cout << "非连接池，单线程，用时：" << length.count() << "纳秒" << endl;
#endif
}
void test2()
{
#if 0
	//14940646600纳秒
	chrono::steady_clock::time_point begin = chrono::steady_clock::now();
	thread t1(op1, 0, 1000);
	thread t2(op1, 1000, 2000);
	thread t3(op1, 2000, 3000);
	thread t4(op1, 3000, 4000);
	thread t5(op1, 4000, 5000);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	chrono::steady_clock::time_point end = chrono::steady_clock::now();
	auto length = end - begin;
	cout << "非连接池，多线程，用时：" << length.count() << "纳秒" << endl;
#else
	//1279062300纳秒
	ConnPool* pool = ConnPool::getConnectPool();
	chrono::steady_clock::time_point begin = chrono::steady_clock::now();
	thread t1(op2, pool, 0, 1000);
	thread t2(op2, pool, 1000, 2000);
	thread t3(op2, pool, 2000, 3000);
	thread t4(op2, pool, 3000, 4000);
	thread t5(op2, pool, 4000, 5000);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	chrono::steady_clock::time_point end = chrono::steady_clock::now();
	auto length = end - begin;
	cout << "连接池，多线程，用时：" << length.count() << "纳秒" << endl;
#endif
}
int main()
{
	//test1();
	test2();
	return 0;
}

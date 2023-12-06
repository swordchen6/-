#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__
#include"TaskQueue.h"
#include<iostream>
class ThreadPool
{
public:
	//创建线程池并初始化
	ThreadPool(int min,int max);
	//销毁线程池
	~ThreadPool();
	void addTask(Task task);
	int getBusyNum();
	int getAliveNum();
private:
	//工作线程
	static void* worker(void* arg);
	//管理线程
	static void* manager(void* arg);
	//单个线程退出
	void threadExit();
private:
	//任务队列
	TaskQueue* taskQ;
	pthread_t managerID;//管理者线程
	pthread_t* threadIDs;//工作的线程ID
	int minNum;//最小线程数量
	int maxNum;//最大线程数量
	int busyNum;//忙的线程数量
	int liveNum;//存活线程数量
	int exitNum;//要销毁的线程数量
	pthread_mutex_t mutexPool;//锁整个的线程池
	pthread_cond_t notEmpty;//任务队列不为空
	bool shutdown;//要不要销毁线程池
	static constexpr int DEFAULT_TIME = 3;
	static constexpr int NUMBER = 2;
};
#endif

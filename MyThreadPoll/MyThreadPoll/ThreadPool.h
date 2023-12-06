#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__
#include"TaskQueue.h"
#include<iostream>
class ThreadPool
{
public:
	//�����̳߳ز���ʼ��
	ThreadPool(int min,int max);
	//�����̳߳�
	~ThreadPool();
	void addTask(Task task);
	int getBusyNum();
	int getAliveNum();
private:
	//�����߳�
	static void* worker(void* arg);
	//�����߳�
	static void* manager(void* arg);
	//�����߳��˳�
	void threadExit();
private:
	//�������
	TaskQueue* taskQ;
	pthread_t managerID;//�������߳�
	pthread_t* threadIDs;//�������߳�ID
	int minNum;//��С�߳�����
	int maxNum;//����߳�����
	int busyNum;//æ���߳�����
	int liveNum;//����߳�����
	int exitNum;//Ҫ���ٵ��߳�����
	pthread_mutex_t mutexPool;//���������̳߳�
	pthread_cond_t notEmpty;//������в�Ϊ��
	bool shutdown;//Ҫ��Ҫ�����̳߳�
	static constexpr int DEFAULT_TIME = 3;
	static constexpr int NUMBER = 2;
};
#endif

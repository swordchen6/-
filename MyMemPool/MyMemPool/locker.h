#pragma once
#include<pthread.h>
//�������ķ�װ
class locker
{
private:
	pthread_mutex_t mutex;
public:
	locker();
	~locker();
	void lock();
	void unlock();
};


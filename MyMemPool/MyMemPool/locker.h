#pragma once
#include<pthread.h>
//»¥³âËøµÄ·â×°
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


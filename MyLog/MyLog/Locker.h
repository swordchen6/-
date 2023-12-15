#pragma once
#include<pthread.h>
#include<semaphore.h>
#include<exception>
class Mutex
{
public:
	Mutex();
	~Mutex();
	bool lock();
	bool unlock();
	pthread_mutex_t* getMutex();
private:
	pthread_mutex_t mutex;
};

class Cond
{
public:
	Cond();
	~Cond();
	bool wait(pthread_mutex_t* m_mutex);
	bool signal();
	bool broadcast();
private:
	pthread_cond_t cond;
};

class Sem
{
public:
	Sem();
	Sem(int num);
	~Sem();
	bool wait();
	bool post();
private:
	sem_t sem;
};


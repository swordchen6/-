#include "ThreadPool.h"
#include<string.h>
#include<string>
#include<unistd.h>
ThreadPool::ThreadPool(int min, int max)
{
	do
	{
		taskQ = new TaskQueue;
		if (taskQ == nullptr)
		{
			std::cout << "new taskQ fail...." << std::endl;
			break;
		}
		threadIDs = new pthread_t[max];
		if (threadIDs == nullptr)
		{
			std::cout << "new threadIDs fail..." << std::endl;
			break;
		}
		memset(threadIDs, 0, sizeof(pthread_t) * max);
		minNum = min;
		maxNum = max;
		busyNum = 0;
		liveNum = min;
		exitNum = 0;
		if (pthread_mutex_init(&mutexPool, NULL) != 0 || pthread_cond_init(&notEmpty, NULL) != 0)
		{
			std::cout << "mutex or cond fail...." << std::endl;
			break;
		}
		shutdown = false;
		pthread_create(&managerID, NULL, manager, this);
		for (int i = 0; i < min; i++)
		{
			pthread_create(&threadIDs[i], NULL, worker, this);
		}
		return;
	} while (0);
	if (threadIDs)
		delete[]threadIDs;
	if (taskQ)
		delete taskQ;

}

ThreadPool::~ThreadPool()
{
	shutdown = true;
	pthread_join(managerID, NULL);
	for (int i = 0; i < liveNum; i++)
	{
		pthread_cond_signal(&notEmpty);
	}
	if (taskQ)
		delete taskQ;
	if (threadIDs)
		delete[]threadIDs;
	pthread_mutex_destroy(&mutexPool);
	pthread_cond_destroy(&notEmpty);
}

void ThreadPool::addTask(Task task)
{
	if (shutdown)
	{
		pthread_mutex_unlock(&mutexPool);
		return;
	}
	taskQ->addTask(task);
	pthread_cond_signal(&notEmpty);
}

int ThreadPool::getBusyNum() 
{
	pthread_mutex_lock(&mutexPool);
	int busy= busyNum;
	pthread_mutex_unlock(&mutexPool);
	return busy;
}

int ThreadPool::getAliveNum() 
{
	pthread_mutex_lock(&mutexPool);
	int live= liveNum;
	pthread_mutex_unlock(&mutexPool);
	return live;
}

void* ThreadPool::worker(void* arg)
{
	ThreadPool* pool = static_cast<ThreadPool*>(arg);
	while (true)
	{
		pthread_mutex_lock(&pool->mutexPool);
		while (pool->taskQ->taskNumber() == 0 && !pool->shutdown)
		{
			pthread_cond_wait(&pool->notEmpty, &pool->mutexPool);
			if (pool->exitNum > 0)
			{
				pool->exitNum--;
				if (pool->liveNum > pool->minNum)
				{
					pool->liveNum--;
					pthread_mutex_unlock(&pool->mutexPool);
					pool->threadExit();
				}
			}
		}
		if (pool->shutdown)
		{
			pthread_mutex_unlock(&pool -> mutexPool);
			pool->threadExit();
		}

		Task task=pool->taskQ->takeTask();
		pool->busyNum++;
		pthread_mutex_unlock(&pool->mutexPool);
		std::cout << "thread " << pthread_self() << "start working...." << std::endl;
		task.function(task.arg);
		delete task.arg;
		task.arg = nullptr;
		std::cout << "thread " << pthread_self() << "end working...." << std::endl;
		pthread_mutex_lock(&pool->mutexPool);
		pool->busyNum--;
		pthread_mutex_unlock(&pool->mutexPool);
	}
	return NULL;
}

void* ThreadPool::manager(void* arg)
{
	ThreadPool* pool = static_cast<ThreadPool*>(arg);
	while (!pool->shutdown)
	{
		sleep(DEFAULT_TIME);
		pthread_mutex_lock(&pool->mutexPool);
		int queueSize = pool->taskQ->taskNumber();
		int livaNum = pool->liveNum;
		int busyNum = pool->busyNum;
		pthread_mutex_unlock(&pool->mutexPool);

		if (queueSize > livaNum && livaNum < pool->maxNum)
		{
			pthread_mutex_lock(&pool->mutexPool);
			int counter = 0;
			for (int i = 0; i < pool->maxNum && counter < NUMBER && pool->liveNum < pool->maxNum; i++)
			{
				if (pool->threadIDs[i] == 0)
				{
					pthread_create(&pool->threadIDs[i], NULL, worker, pool);
					counter++;
					pool->liveNum++;
				}
			}
			pthread_mutex_unlock(&pool->mutexPool);
		}
		if (busyNum * 2 < livaNum && livaNum > pool->minNum)
		{
			pthread_mutex_lock(&pool->mutexPool);
			pool->exitNum = NUMBER;
			pthread_mutex_unlock(&pool->mutexPool);
			for (int i = 0; i < NUMBER; i++)
			{
				pthread_cond_signal(&pool->notEmpty);
			}
		}
	}
	return NULL;
}

void ThreadPool::threadExit()
{
	pthread_t tid = pthread_self();
	for (int i = 0; i < maxNum; i++)
	{
		if (threadIDs[i] == tid)
		{
			threadIDs[i] == 0;
			std::cout << "threadExit called" <<tid<< std::endl;
		}
	}
	pthread_exit(NULL);
}

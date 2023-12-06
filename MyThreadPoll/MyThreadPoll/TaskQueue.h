#ifndef __TASK_QUEUE_H__
#define __TASK_QUEUE_H__
#include<queue>
#include<pthread.h>
using callback = void (*)(void*);
struct Task
{
	void (*function)(void* arg);
	void* arg;
	Task()
	{
		function = nullptr;
		arg = nullptr;
	}
	Task(callback f, void* arg)
	{
		this->arg = arg;
		function = f;
	}
};
class TaskQueue
{
public:
	TaskQueue();
	~TaskQueue();
	//添加任务
	void addTask(Task task);
	void addTask(callback f,void* arg);
	//取出一个任务
	Task takeTask();
	//获取当前任务数
	int taskNumber()const;
private:

	pthread_mutex_t m_mutex;
	std::queue<Task> m_taskQ;

};

#endif
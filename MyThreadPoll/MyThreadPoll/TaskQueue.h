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
	//�������
	void addTask(Task task);
	void addTask(callback f,void* arg);
	//ȡ��һ������
	Task takeTask();
	//��ȡ��ǰ������
	int taskNumber()const;
private:

	pthread_mutex_t m_mutex;
	std::queue<Task> m_taskQ;

};

#endif
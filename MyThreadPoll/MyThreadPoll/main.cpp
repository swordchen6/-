#include <iostream>
#include"ThreadPool.h"
#include<unistd.h>
using namespace std;
void taskFunc(void* arg)
{
    int num = *(int*)arg;
    cout << "thread " << pthread_self() << "is working,and number is "<<num << endl;
    sleep(1);
}
int main()
{
    ThreadPool pool(3, 10);
    for (int i = 0; i < 100; i++)
    {
        int* num = new int;
        *num = i + 100;
        pool.addTask(Task(taskFunc, num));
    }
    sleep(30);
    return 0;
}
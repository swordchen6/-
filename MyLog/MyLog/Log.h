#pragma once
#include<stdio.h>
#include<string>
#include<string.h>
#include<queue>
#include<stdarg.h>
#include<time.h>
#include<unistd.h>
#include<map>
#include<utility>
#include"Locker.h"
class Log
{
public:
	static Log* get_instance();
	bool init(const char* f, bool m, int max_r);//初始化
	~Log();
	void write_log(int level, const char* format, ...);
	//同步直接写到文件内，异步写到缓冲里，对缓冲进行加锁
	//为什么要对缓冲加锁而不是文件加锁呢，是因为对文件写是磁盘操作
	//速度满，缓冲是内存操作，速度快
private:
	Log();//
	Log(const Log&) = delete;//
	Log& operator=(const Log&) = delete;//
	static const int LENGTH = 128;
	static std::vector<std::string> get_level;

	bool mode;//是否异步,true为异步，false为同步
	static void* async_write(void* arg);//异步线程的函数

	std::queue<std::pair<std::string, FILE*>> recond_buffer;
	void push(const std::pair<std::string, FILE*>& obj);
	bool pop(std::pair<std::string, FILE*>& obj);

	struct tm get_time();
	void change_file();

	char new_log_row[2 * LENGTH];//存储处理后的要写到文件的每一行字符串
	std::map<FILE*, int> times;
	FILE* fp;
	int today;

	int max_row;
	int row_count;
	char path_name[LENGTH];
	char file_name[LENGTH];
	char late_name[2 * LENGTH];
	//日志类安全
	Mutex log_mutex;
	Cond recond_cond;
	Mutex recond_mutex;
};
#define LOG_DEBUG(format, ...) Log::get_instance()->write_log(0, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) Log::get_instance()->write_log(1, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) Log::get_instance()->write_log(2, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) Log::get_instance()->write_log(3, format, ##__VA_ARGS__)

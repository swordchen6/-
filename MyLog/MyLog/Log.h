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
	bool init(const char* f, bool m, int max_r);//��ʼ��
	~Log();
	void write_log(int level, const char* format, ...);
	//ͬ��ֱ��д���ļ��ڣ��첽д��������Ի�����м���
	//ΪʲôҪ�Ի�������������ļ������أ�����Ϊ���ļ�д�Ǵ��̲���
	//�ٶ������������ڴ�������ٶȿ�
private:
	Log();//
	Log(const Log&) = delete;//
	Log& operator=(const Log&) = delete;//
	static const int LENGTH = 128;
	static std::vector<std::string> get_level;

	bool mode;//�Ƿ��첽,trueΪ�첽��falseΪͬ��
	static void* async_write(void* arg);//�첽�̵߳ĺ���

	std::queue<std::pair<std::string, FILE*>> recond_buffer;
	void push(const std::pair<std::string, FILE*>& obj);
	bool pop(std::pair<std::string, FILE*>& obj);

	struct tm get_time();
	void change_file();

	char new_log_row[2 * LENGTH];//�洢������Ҫд���ļ���ÿһ���ַ���
	std::map<FILE*, int> times;
	FILE* fp;
	int today;

	int max_row;
	int row_count;
	char path_name[LENGTH];
	char file_name[LENGTH];
	char late_name[2 * LENGTH];
	//��־�లȫ
	Mutex log_mutex;
	Cond recond_cond;
	Mutex recond_mutex;
};
#define LOG_DEBUG(format, ...) Log::get_instance()->write_log(0, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) Log::get_instance()->write_log(1, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) Log::get_instance()->write_log(2, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) Log::get_instance()->write_log(3, format, ##__VA_ARGS__)

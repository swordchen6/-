#include "Log.h"
std::vector<std::string> Log::get_level{ "DEBUF", "INFO", "WARN", "ERROR" };
Log* Log::get_instance()
{
	static Log temp;
	return &temp;
}

Log::~Log()
{
	//在日志写完析构
	while (!recond_buffer.empty()) sleep(3);
	//结束子线程
	recond_cond.signal();
}

void* Log::async_write(void* arg)
{
	Log* temp = Log::get_instance();
	std::pair<std::string, FILE*> get;
	while (temp->pop(get))
	{
		temp->log_mutex.lock();
		fputs(get.first.c_str(), get.second);
		temp->times[get.second]++;
		if (temp->times[get.second] == temp->max_row)
		{
			temp->times.erase(get.second);
			fflush(get.second);
			fclose(get.second);
		}
		temp->log_mutex.unlock();
	}
}

bool Log::init(const char* f, bool m, int max_r)
{
	fp = NULL;
	mode = m;
	max_row = max_r;
	if (mode)
	{
		pthread_t tid;
		pthread_create(&tid, NULL, async_write, NULL);
		pthread_detach(tid);
	}
	memset(path_name, 0, LENGTH);
	memset(file_name, 0, LENGTH);
	memset(late_name, 0, 2 * LENGTH);
	//区分路径和文件名
	if (f == NULL)
	{
		getcwd(path_name, LENGTH);
		snprintf(file_name, LENGTH, "%s", "log.txt");
	}
	else
	{
		const char* middle = strrchr(f, '/');
		strcpy(file_name, middle + 1);
		strncpy(path_name, f, middle - f);
	}
	//获取文件名处理后并且和时间有关名字的文件名
	change_file();
	times[fp] = 0;
	row_count = 0;
	return true;
}

void Log::push(const std::pair<std::string, FILE*>& obj)
{
	recond_mutex.lock();
	recond_buffer.push(obj);
	recond_mutex.unlock();
	recond_cond.signal();
}

bool Log::pop(std::pair<std::string, FILE*>& obj)
{
	recond_mutex.lock();
	while (recond_buffer.empty())
	{
		recond_cond.wait(recond_mutex.getMutex());
		if (recond_buffer.empty())
		{
			recond_mutex.unlock();
			return false;
		}
	}
	obj = recond_buffer.front();
	recond_buffer.pop();
	recond_mutex.unlock();
	return true;
}

void Log::write_log(int t, const char* format, ...)
{
	struct tm time = get_time();
	log_mutex.lock();
	if (today != time.tm_mday || (row_count % max_row == 0&&row_count>0))
	{
		change_file();
		times[fp] = 0;
	}
	//拼接日志等级
	int length1 = snprintf(new_log_row, 2 * LENGTH, "%s ", get_level[t].c_str());
	//拼接时分秒
	int length2 = snprintf(new_log_row + length1, 2 * LENGTH  - length1, "%02d_%02d_%02d ",
		time.tm_hour, time.tm_min, time.tm_sec);
	//拼接要写的信息
	va_list valist;
	va_start(valist, format);
	int length3 = vsnprintf(new_log_row + length1 + length2, 2 * LENGTH - length1 - length2, format, valist);
	va_end(valist);
	new_log_row[length1 + length2 + length3] = '\n';
	new_log_row[length1 + length2 + length3 + 1] = '\0';
	//两种写文件方式
	if (mode)
	{
		recond_buffer.push(std::make_pair(new_log_row, fp));
	}
	else
	{
		fputs(new_log_row, fp);
	}
	row_count++;
	log_mutex.unlock();
}

Log::Log()
{

}

tm Log::get_time()
{
	time_t temp = time(NULL);
	struct tm* time = localtime(&temp);
	return *time;
}

void Log::change_file()
{
	struct tm time = get_time();
	today = time.tm_mday;
	snprintf(late_name, 2 * LENGTH, "page_%d__%d_%02d_%02d_%s", row_count / max_row, 
		time.tm_year + 1900,time.tm_mon + 1, time.tm_mday, file_name);
	fp = fopen(late_name, "a");
	if (!fp)
	{
		perror("change file error");
		throw std::exception();
	}
}

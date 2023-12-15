看过TinyWebServer，感觉它的日志类似乎有点问题
1.第一个问题
它创建的异步写线程开启后，既没有join也没有detach。
当主线程结束，内存收回，子线程还在非法的内存上继续跑，可能发生double free or corruption的错误。
2.第二个问题
################################
void *async_write_log()
{
  string single_log;
  //从阻塞队列中取出一条日志内容，写入文件
  while (m_log_queue->pop(single_log))
  {
    m_mutex.lock();
    fputs(single_log.c_str(), m_fp);
    m_mutex.unlock();
  }
}
################################
下面是write_log的函数部分
m_mutex.lock();
//更新现有行数
 m_count++;
//日志不是今天或写入的日志行数是最大行的倍数
//m_split_lines为最大行数
if (m_today != my_tm.tm_mday || m_count % m_split_lines == 0)
{
  fflush(m_fp);
  fclose(m_fp);
  //分文件处理......
  m_fp = fopen(new_log, "a");
}
m_mutex.unlock();
################################
不知道发现没有，两个函数的m_fp不同步，就是说假如一下子来10个线程，我们设置的最大行数为5，可以看到write_log函数一直增大m_count
当m_count大于5时，上面异步写到磁盘的函数到现在还没有写，write_log函数就关闭m_fp，重新创建文件，m_fp被改了，那么异步写函数就会往新的
m_fp里写，因此创建的文件可能达不到最大行数就已经被关闭了。

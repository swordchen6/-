#include "ConnPool.h"
        
ConnPool* ConnPool::getConnectPool()
{
    static ConnPool pool;
    return &pool;
}

std::shared_ptr<MysqlConn> ConnPool::getConn()
{
    std::unique_lock<std::mutex> locker(m_mutex);
    while (m_connQueue.empty())
    {
        if (std::cv_status::timeout == m_cond.wait_for(locker, std::chrono::milliseconds(m_timeout)))
        {
            if (m_connQueue.empty())
            {
                continue;
            }
        }
    }
    //将连接拿回线程池
    std::shared_ptr<MysqlConn> connptr(m_connQueue.front(), [this](MysqlConn* conn) {
        std::lock_guard<std::mutex> locker(m_mutex);
        conn->refreshAliveTime();
        m_connQueue.push(conn);
        });
    m_connQueue.pop();
    m_cond.notify_all();
    return connptr;
}

ConnPool::~ConnPool()
{
    std::unique_lock<std::mutex> locker(m_mutex);
    poolSwitch = false;
    while (m_size--)
    {
        if (!m_connQueue.empty())
        {
            MysqlConn* conn = m_connQueue.front();
            m_connQueue.pop();
            delete conn;
            continue;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    locker.unlock();
}

ConnPool::ConnPool()
{
    //加载配置文件
    if (!parseJsonFile())
    {
        return;
    }
    for (int i = 0; i < m_minSize; i++)
    {
        addConn();
    }
    poolSwitch = true;
    m_size = m_minSize;
    std::thread producer(&ConnPool::produceConn, this);
    std::thread recycler(&ConnPool::recycleConn, this);
    producer.detach();
    recycler.detach();
}

bool ConnPool::parseJsonFile()
{
    std::ifstream ifs("dbconf.json");
    Json::Reader rd;
    Json::Value root;
    rd.parse(ifs, root);
    if (root.isObject())
    {
        m_ip = root["ip"].asString();
        m_port = root["port"].asInt();
        m_user = root["userName"].asString();
        m_passwd = root["password"].asString();
        m_dbName = root["dbName"].asString();
        m_minSize = root["minSize"].asInt();
        m_maxSize = root["maxSize"].asInt();
        m_maxIdleTime = root["maxIdleTime"].asInt();
        m_timeout = root["timeout"].asInt();
        return true;
    }
    return false;
}

void ConnPool::produceConn()
{
    while (poolSwitch)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        //建立一个算法，当size<m_minSize并且m_size<m_maxSize时生产连接
        while (m_connQueue.size() >= m_minSize)
        {
            m_cond.wait(locker);
        }
        if (m_size < m_maxSize)
        {
            addConn();
            m_size++;
            m_cond.notify_all();
        }
    }
}

void ConnPool::recycleConn()
{
    while (poolSwitch)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::lock_guard<std::mutex> locker(m_mutex);
        while (m_connQueue.size() > m_minSize)
        {
            MysqlConn* conn = m_connQueue.front();
            if (conn->getAliveTime() >= m_maxIdleTime)
            {
                m_connQueue.pop();
                m_size--;
                delete conn;
            }
            else
                break;
        }
    }
}

void ConnPool::addConn()
{
    MysqlConn* conn = new MysqlConn;
    conn->connect(m_user, m_passwd, m_dbName, m_ip, m_port);
    conn->refreshAliveTime();
    m_connQueue.push(conn);
}

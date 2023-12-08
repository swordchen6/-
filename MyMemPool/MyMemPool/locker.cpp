#include "locker.h"
#include <cerrno>
locker::locker()
{
	pthread_mutex_init(&mutex, NULL);
}

locker::~locker()
{
	pthread_mutex_destroy(&mutex);
}

void locker::lock()
{
	pthread_mutex_lock(&mutex);
}

void locker::unlock()
{
	pthread_mutex_unlock(&mutex);
}

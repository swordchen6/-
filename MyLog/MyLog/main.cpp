#include <cstdio>
#include"Log.h"
int main()
{
    Log::get_instance()->init(NULL, true, 5);
    for (int i = 0; i < 15; i++)
    {
        LOG_DEBUG("%s", "it is a error");
    }
    sleep(3);
    return 0;
}
#include "RealClass.h"
int RealClass::count = 0;
MemPool<sizeof(RealClass), 2> mp;
void* RealClass::operator new(size_t size)
{
    return mp.newMen();
}

void RealClass::operator delete(void* p)
{
    mp.deleteMem(p);
}


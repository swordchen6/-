#include "TestClass.h"
template<typename T, int size>
T* TestClass<T,size>::newT()
{
	return static_cast<T*>(mp.newMen());
}
template<typename T, int size>
void TestClass<T,size>::deleteT(void* ptr)
{
	mp.deleteMem(ptr);
}

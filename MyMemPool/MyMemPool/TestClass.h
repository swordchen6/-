#pragma once
#include<iostream>
#include"MemPool.h"
template<typename T,int size>
class TestClass
{
private:
	static MemPool<sizeof(T), size> mp;
public:
	TestClass()=default;
	~TestClass() = default;
	T* newT();
	void deleteT(void* ptr);
};
template<typename T, int size>
MemPool<sizeof(T), size> TestClass<T,size>::mp;


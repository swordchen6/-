#pragma once
#include"MemPool.h"
#include<iostream>
class RealClass
{
	static int count ;
	int No;
public:
	RealClass() 
	{
		No = count;
		count++;
	}
	void print() 
	{
		std::cout << this << ": ";
		std::cout << "the " << No << "th object" << std::endl;
	}
	void* operator new(size_t size);
	void operator delete(void* p);
};

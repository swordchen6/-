#pragma once
#include<cstddef>
#include"locker.h"
template<int nodeSize,int nodeNum>
class MemPool
{
public:
	MemPool();
	~MemPool();
	//分配内存
	void* newMen();
	//销毁内存
	void deleteMem(void* ptr);
private:
	struct FreeNode
	{
		FreeNode* pNext;
		char data[nodeSize];
	};
	struct MemBlock
	{
		MemBlock* pNext;
		FreeNode node[nodeNum];
	};
	locker mutex;
	FreeNode* freeNodeHeader;
	MemBlock* memBlockHeader;
};

template<int nodeSize, int nodeNum>
MemPool<nodeSize, nodeNum>::MemPool()
{
	freeNodeHeader = nullptr;
	memBlockHeader = nullptr;
}

template<int nodeSize, int nodeNum>
MemPool<nodeSize, nodeNum>::~MemPool()
{
	MemBlock* ptr;
	mutex.lock();
	while (memBlockHeader)
	{
		ptr = memBlockHeader->pNext;
		delete memBlockHeader;
		memBlockHeader = ptr;
	}
	mutex.unlock();
}

template<int nodeSize, int nodeNum>
void* MemPool<nodeSize, nodeNum>::newMen()
{
	mutex.lock();
	//如果空闲节点为空，说明内存块已经用完，或者刚开始还没分配内存块
	if (freeNodeHeader == nullptr)
	{
		MemBlock* newBlock = new MemBlock;
		newBlock->pNext = nullptr;
		//将一个内存块之间的节点连接
		freeNodeHeader = &(newBlock->node[0]);

		for (int i = 1; i < nodeNum; i++)
		{
			newBlock->node[i - 1].pNext = &(newBlock->node[i]);
		}
		newBlock->node[nodeNum - 1].pNext = nullptr;
		//如果是刚开始分配内存块
		if (memBlockHeader == nullptr)
		{
			memBlockHeader = newBlock;
		}
		else
		{
			newBlock->pNext = memBlockHeader;
			memBlockHeader = newBlock;
		}
	}
	//将一个空闲节点分配出去，并将该节点从空闲链表上移除
	void* freeNode = freeNodeHeader;
	freeNodeHeader = freeNodeHeader->pNext;
	mutex.unlock();
	return freeNode;
}

template<int nodeSize, int nodeNum>
void MemPool<nodeSize, nodeNum>::deleteMem(void* ptr)
{
	FreeNode* pNode = static_cast<FreeNode*>(ptr);
	mutex.lock();
	//利用头插法，将空闲节点插回空闲链表上
	pNode->pNext = freeNodeHeader;
	freeNodeHeader = pNode;
	mutex.unlock();
}





#pragma once
#include<cstddef>
#include"locker.h"
template<int nodeSize,int nodeNum>
class MemPool
{
public:
	MemPool();
	~MemPool();
	//�����ڴ�
	void* newMen();
	//�����ڴ�
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
	//������нڵ�Ϊ�գ�˵���ڴ���Ѿ����꣬���߸տ�ʼ��û�����ڴ��
	if (freeNodeHeader == nullptr)
	{
		MemBlock* newBlock = new MemBlock;
		newBlock->pNext = nullptr;
		//��һ���ڴ��֮��Ľڵ�����
		freeNodeHeader = &(newBlock->node[0]);

		for (int i = 1; i < nodeNum; i++)
		{
			newBlock->node[i - 1].pNext = &(newBlock->node[i]);
		}
		newBlock->node[nodeNum - 1].pNext = nullptr;
		//����Ǹտ�ʼ�����ڴ��
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
	//��һ�����нڵ�����ȥ�������ýڵ�ӿ����������Ƴ�
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
	//����ͷ�巨�������нڵ��ؿ���������
	pNode->pNext = freeNodeHeader;
	freeNodeHeader = pNode;
	mutex.unlock();
}





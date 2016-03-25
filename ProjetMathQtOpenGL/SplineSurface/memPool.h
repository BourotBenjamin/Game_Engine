#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <cstdint>
#include <memory>

static const uint64_t GIGABYTE = 1073741824;

struct memoryBlock
{
	int * ptr;
	uint64_t cap;

	memoryBlock(int * p, uint64_t cap) : ptr(p), cap(cap){}
};

class MemoryManager
{
public:
	MemoryManager(uint64_t size = GIGABYTE) : memsize(size)
	{
		ptr = (int*) malloc(size);
		memBlck.reserve(1000);
		freeMemory = ptr;
	}

	~MemoryManager()
	{
		free(ptr);
	}

	template<class R>
	R * allocation(uint64_t capacity)
	{
		R * tmp = (R*) freeMemory;
		freeMemory += capacity * sizeof(R);
		memBlck.emplace_back(memoryBlock(tmp, capacity));
		return tmp;
	}



private:
	int * ptr;
	int * freeMemory;
	uint64_t memsize;
	std::vector<memoryBlock> memBlck;
};

#endif
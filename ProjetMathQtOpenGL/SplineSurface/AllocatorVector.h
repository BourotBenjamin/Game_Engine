#ifndef ALLOCATOR_VECTOR_H
#define ALLOCATOR_VECTOR_H

#include <cstdint>
#include <vector>
#include "memPool.h"


template<class R>
class AllocatorVector
{
public:
	AllocatorVector(MemoryManager& memo, uint64_t capacity) :mem(memo), capacity(capacity)
	{
		//get free memory pointer from pool
		ptr = mem.allocation<R>(capacity* sizeof(R));

		//reserve alive vector to keep track of entities and set it to false
		alive.reserve(capacity);
		for (int i = 0; i < capacity; ++i)
			alive.push_back(false);
	}

	//placement new in 1 free space and return ptr
	template<typename... Args>
	R * allocation(Args... args)//R&& obj)
	{
		int i = 0;
		while (alive[i] == true)
			++i;

		//no free space
		if (i == capacity)
			return nullptr;

		//placement new on the right space
		 new(ptr + i) R(args...);

		//update alive to true
		alive[i] = true;

		//reutrn new object
		return ptr + i;

	}

	void desalocate(void * p)
	{
		void * tmp = ptr;
		while (tmp != p)
		{
			++i;
			++tmp;
		}
		alive[i] = false;

	}

	R * getFirst(){ return ptr; }
	uint64_t getCapacity(){ return capacity; }

private:
	MemoryManager& mem;
	uint64_t capacity;
	R * ptr;
	std::vector<bool> alive;
};

#endif

//R a un pointeur vers le prochain dispo
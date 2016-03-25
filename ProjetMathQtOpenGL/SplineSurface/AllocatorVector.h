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
		ptr = mem.allocation<R>(capacity);
		alive.reserve(capacity);
		for (int i = 0; i < capacity; ++i)
			alive.push_back(false);
	}

	template <typename... Args>
	R * allocation(Args... args)
	{
		int i = 0;
		while (alive[i] == true)
			++i;
		if (i == capacity)
			return nullptr;

		new(ptr + i) R(args...);
		alive[i] = true;
		return ptr + i;

	}

	void desalocate(int index)
	{
		alive[i] = false;
	}

private:
	MemoryManager& mem;
	uint64_t capacity;
	R * ptr;
	std::vector<bool> alive;
};

#endif

//R a un pointeur vers le prochain dispo
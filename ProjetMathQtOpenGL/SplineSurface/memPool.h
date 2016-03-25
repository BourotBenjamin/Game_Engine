#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <cstdint>
#include <memory>

static const uint64_t GIGABYTE = 1073741824;
static const uint32_t MAMMEMBLOCK = 1000;

struct memoryBlock
{
	int * ptr;
	uint64_t cap;

	memoryBlock(int * p, uint64_t cap) : ptr(p), cap(cap){}
	memoryBlock(memoryBlock&& m){ ptr = std::move(m.ptr); cap = std::move(m.cap); }
};

class MemoryManager
{
public:
	MemoryManager(uint64_t size = GIGABYTE) : memsize(size)
	{
		//reserve memory
		ptr = (int *) malloc(size);
		ptr[0] = 0;
		//set free memory pointer
		freeMemory = ptr;

		//reserve memory mangement vector
		memBlck.reserve(MAMMEMBLOCK);
		alive.reserve(MAMMEMBLOCK);
		for (int i = 0; i < MAMMEMBLOCK; ++i)
			alive.push_back(false);
		
	}

	~MemoryManager()
	{
		//free all memory
		free(ptr);
	}

	template<class R>
	R * allocation(uint64_t capacity)
	{
		int i = 0;
		bool ok = false;
		auto it = memBlck.begin();
		for (; it != memBlck.end();++it)
		{
			if (!alive[i] && it->cap > capacity)
			{
				break;
				ok = true;
			}
				
			++i;
		}
		//set the existing memory block to the new value and create a new one for the delta
		if (ok)
		{
			//add a new memoryblock after the fitting of the old one
			it = memBlck.emplace(it+1, memoryBlock((int*) memBlck[i].ptr + capacity, memBlck[i].cap - capacity));
			(it - 1)->cap = capacity;

			//return ptr
			return (R*) (it - 1)->ptr;
		}
		else
		{
			//save current free memory
			R * tmp = (R*) freeMemory;

			//move freememory pointer to the next free space
			freeMemory += capacity;

			//get the last index for alive vector
			int j = 0;
			for (auto it = memBlck.begin(); it != memBlck.end(); ++it)
				++j;

			//add block to the memory block vector
			memBlck.emplace_back((int*) tmp, capacity);

			alive[j] = true;

			return tmp;
		}
	}

	template<class R>
	void release(R * p)
	{
		//look for the right block to delete
		int i = 0;
		for (; i < memBlck.size(); ++i)
		{
			if (p == memBlck[i].ptr)
				break;
		}

		//if block is not the last one we keep the block for future realoc
		if (memBlck[i].ptr != memBlck.back())
			alive[i] = false;

		//if it's last we delete block
		else
		{
			alive[i] = false;

			//set free pointer to the block ptr
			freeMemory = memBlck.back().ptr;
			memBlck.pop_back();
		}
	}



private:
	int * ptr;
	int * freeMemory;
	uint64_t memsize;
	std::vector<memoryBlock > memBlck;
	std::vector<bool> alive;
};

#endif
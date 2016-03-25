#ifndef WORKER_H
#define WORKER_H

#include <condition_variable>
//#include "threadPool.h"

class ThreadPool;

// our worker thread objects
class Worker {
public:
	Worker(ThreadPool &s) : pool(s) { }
	void operator()();

private:
	ThreadPool &pool;
	uint32_t structSize;
	int sizeOfPool;
	void * start;
};

#endif
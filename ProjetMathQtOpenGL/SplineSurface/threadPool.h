#ifndef THREADPOOL_H
#define THREADPOOL_H


#include "worker.h"
#include <iostream>
#include <atomic>
#include <thread>
#include <cstdint>
#include <vector>
#include <queue>
#include <condition_variable>


struct Job
{
	void * ptr;
	uint32_t size;
	uint32_t number;
	std::function < void(void *, int)> func;
};

// the actual thread pool
class ThreadPool {
public:
	ThreadPool(size_t threads, int sizeOfPool)
		: stop(false)
	{
		for (size_t i = 0; i<threads; ++i)
			workers.push_back(std::thread(Worker(*this)));
	}

	void enqueue(const Job &job)
	{
		{ // acquire lock
			std::unique_lock<std::mutex> lock(queue_mutex);

			// add the task
			jobs.push_back(job);
		} // release lock

		// wake up one thread
		condition.notify_one();
	}

	template<class R>
	void assign(R * ptr, uint32_t size, std::function<void(int, int)> f)
	{
		uint32_t nbTask = size / workers.size();
		for (size_t i = 0; i < workers.size(); ++i) {
			// Assign properties to the job.
			Job job;
			job.ptr = ptr;
			job.size = size;
			job.number = nbTask;
			job.func = f;

			// Job push in queue.
			enqueue(job);

			//std::thread workers[i](f);
			ptr += nbtask;
		}
		while (completedJobs != workers.size()) {
			std::this_thread::yield();
		}
	}

	~ThreadPool()
	{
		// stop all threads
		stop = true;
		condition.notify_all();

		// join them
		for (size_t i = 0; i<workers.size(); ++i)
			workers[i].join();
	}
private:
	friend class Worker;

	// need to keep track of threads so we can join them
	std::vector< std::thread > workers;

	// the task queue
	std::deque<Job> jobs;
	std::atomic<int> completedJobs;
	// synchronization
	std::mutex queue_mutex;
	std::condition_variable condition;
	bool stop;
};

#endif
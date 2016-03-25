#include "worker.h"

#include "threadPool.h"

void Worker::operator()(){
	//std::function<void(int, int)> job;
	Job job;
	while (true)
	{
		{   // acquire lock
			std::unique_lock<std::mutex> lock(pool.queue_mutex);

			// look for a work item
			while (!pool.stop && pool.jobs.empty())
			{ // if there are none wait for notification
				pool.condition.wait(lock);
			}

			if (pool.stop) // exit if the pool is stopped
				return;

			// get the task from the queue
			job = pool.jobs.front();
			pool.jobs.pop_front();

		}   // release lock

		// execute the task
		job.func(job.ptr, job.number);
		pool.completedJobs++;
	}
}
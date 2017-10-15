/*
 * BP_ThreadDispatcher.cc
 *
 * Thread dispatcher class.  This class was written with care.
 *
 */

// include main includes
#include "../../../include/BP-Main.h"

#ifdef BP_BLOCKCLASS_INCLUDE_USE_OLD_PTHREAD_METHODS

// class constructor
BP_ThreadDispatcher::BP_ThreadDispatcher(P_BP_THREAD_DISPATCHER_RUN_JOBS jobs_stack, size_t job_count)
{

	// create new block for operating on threads
	this->thread_block = new Block();

	// create new block for operating on jobs
	this->jobs_stack = jobs_stack;

	// set number of jobs
	this->jobs_count = job_count;


}

// class deconstructor
BP_ThreadDispatcher::~BP_ThreadDispatcher()
{

	// delete the thread block
	delete this->thread_block;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%% Class Main Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Dispatch routines.  If called more then once with the same parameters
// this function checks that the thread queue contains space for a job
// to be dispatched and redispatches a job.
BP_ERROR_T BP_ThreadDispatcher::dispatchRoutines()
{

	if(!this->jobs_stack)
	{
		this->AddErr("Error in dispatchRoutines(): No job stack to dispatch from (NULL obj).");
		return ERR_FAILURE;
	}

	// always check the thread block
	if(!this->thread_block)
	{
		this->thread_block = new Block();
		if(!this->thread_block)
		{
			this->AddErr("Error in dispatchRoutines(): Null thread block.");
			return ERR_FAILURE;
		}
	}

	// now run each job
	size_t x = 0;
	for(; x < this->jobs_count; x++)
	{

		// create individual threads
		this->thread_block->CreateNewThread
		(
				this->jobs_stack[x].routine,
				this->jobs_stack[x].parameters,
				1,
				this->jobs_stack[x].thread_slot_description
		);

	}

	// return indicating success
	return ERR_SUCCESS;

}

// While loop that simply waits while the dispatched routines are
// all running.  When one stops, that thread is returned and can
// be redispatched again.
P_THREAD_LIST BP_ThreadDispatcher::waitTillThreadAvailable()
{

	// loop counter
	size_t x = 0;

	// wait for a job to return
	do
	{

		// walk the jobs and check the status of each
		for(x = 0; x < this->jobs_count; x++)
		{

			// set thread list
			P_THREAD_LIST job_thread = this->thread_block->GetThreadPtrByDescription
			(
					this->jobs_stack[x].thread_slot_description,
					strnlen(this->jobs_stack[x].thread_slot_description, 2048)
			);

			// error if we cannot find the thread
			if(!job_thread)
			{
				this->AddErr("Error in waitTillThreadAvailable: Job thread is not found.");
				return NULL;
			}

			// if a thread has exited return with the thread data

			printf("\n Checking thread id: %u", job_thread->threadId);

			if(pthread_tryjoin_np(job_thread->threadId, NULL))
			{
				return job_thread;
			}


			int ret = pthread_kill(job_thread->threadId, 0);
			if(ret == ESRCH)
			{

			}

		}

		// sleep so as not to get crazy with polling
		usleep(100);

	} while(1);

	return NULL;
}

// Redispatches a returned thread.
BP_ERROR_T BP_ThreadDispatcher::redispatchThread(P_THREAD_LIST thread)
{

	if(!thread)
		return ERR_FAILURE;

	// run the redispatch thread
	return this->thread_block->redispatchThread(thread);

}

#endif

/*
 * BP_ThreadDispatcher.h
 *
 *  Created on: Nov 24, 2011
 *      Author: root
 */

#ifndef BP_THREADDISPATCHER_H_
#define BP_THREADDISPATCHER_H_



//! Simple thread stack used for operating with the thread dispatcher.  This stack
// data is only used for launching threads.  Internal blockparty thread structures are
// used for holding real data.
typedef struct _BP_THREAD_DISPATCHER_RUN_JOBS {

	// pointer to the routine to dispatch
	BP_PTHREAD_FN_PTR routine;

	// parameters for the routine
	void *parameters;

	// unique description
	char *thread_slot_description;

} BP_THREAD_DISPATCHER_RUN_JOBS, *P_BP_THREAD_DISPATCHER_RUN_JOBS;


//! Simple class for handling dispatching threads.
class BP_ThreadDispatcher : public BP_BaseError
{

public:

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%% Parent-Only Data %%%%%%%%%%%%%%%%%%%%%%%%%%%

	// thread block object for working with threads
	Block * thread_block;

	// jobs stack used for dispatching
	P_BP_THREAD_DISPATCHER_RUN_JOBS jobs_stack;

	// job count
	size_t jobs_count;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%% Class CTOR/DTOR %%%%%%%%%%%%%%%%%%%%%%%%%%%

public:

	// class constructor
	BP_ThreadDispatcher(P_BP_THREAD_DISPATCHER_RUN_JOBS jobs_stack, size_t job_count);

	// class deconstructor
	~BP_ThreadDispatcher();

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%% Main Class Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Dispatch routines.  If called more then once with the same parameters
	// this function checks that the thread queue contains space for a job
	// to be dispatched and redispatches a job.
	BP_ERROR_T dispatchRoutines();

	// While loop that simply waits while the dispatched routines are
	// all running.  When one stops, that thread is returned and can
	// be redispatched again.
	P_THREAD_LIST waitTillThreadAvailable();

	// Redispatches a returned thread.
	BP_ERROR_T redispatchThread(P_THREAD_LIST thread);

};


#endif /* BP_THREADDISPATCHER_H_ */

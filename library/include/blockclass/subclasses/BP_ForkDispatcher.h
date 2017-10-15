/*
 * BP_ForkDispatcher.h
 *
 *  Created on: Nov 18, 2011
 *      Author: root
 */

#ifndef BP_FORKDISPATCHER_H_
#define BP_FORKDISPATCHER_H_

// create the dispatch policies to be used
typedef enum
{

	BP_FORK_DISPATCH_POLICY_NO_BLOCK,
	BP_FORK_DISPATCH_POLICY_BLOCK

} BP_FORK_DISPATCH_POLICY;


// wait policy
typedef enum
{
	BP_FORK_WAIT_POLICY_WAIT_ALL,
	BP_FORK_WAIT_POLICY_RETURN_ON_EXIT

} BP_FORK_WAIT_POLICY;

//! pid information structure
typedef struct _BP_FORKDISPATCH_PARENT_PID_INFO {

	// process pid
	pid_t pid;

	// exited (true if processes exited, this is different from status)
	bool exited;

	// exit status
	int exit_status;

	// flag indicating whether or not process has signaled
	bool signaled;

	// signal
	int last_signal;

} BP_FORKDISPATCH_PARENT_PID_INFO, *P_BP_FORKDISPATCH_PARENT_PID_INFO;


//! Simple class to handling fork/vforking() multiple child jobs, and watching their statuses.
class BP_ForkDispatcher : public BP_BaseError
{

public:

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%% Parent-Only Data %%%%%%%%%%%%%%%%%%%%%%%%%%%

	// toggle this to true if you want to use vfork() instead of fork.
	bool use_vfork;

	// maximum running pids to ever launch (will never exceed this)
	size_t max_running_pids;

	// dispatch policy
	BP_FORK_DISPATCH_POLICY dispatch_policy;

	// dispatch wait policy
	BP_FORK_WAIT_POLICY wait_policy;

	// stack of currently running pids
	P_BP_FORKDISPATCH_PARENT_PID_INFO running_pids;
	size_t                            running_pid_n;

	// enable this to monitor exited pids (on by default, watch for leaks)
	bool enable_exit_pid_monitoring;


	// top pid for dispatcher
	pid_t top_pid;

	// current pid for running process (forked)
	pid_t curr_pid;

	// current fork pid index
	pid_t curr_fork_index;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%% Class Methods %%%%%%%%%%%%%%%%%%%%%%%%

	// class constructor
	BP_ForkDispatcher(size_t fork_count, BP_FORK_DISPATCH_POLICY policy);

	// class deconstructor
	~BP_ForkDispatcher();


	// runs all configured forks in the class (aka dispatch all).  Will
	// never run more than the classes maximum running pids object.
	BP_ERROR_T runForks();

	// overprototyped runForks routine to take parameters
	BP_ERROR_T runForks(size_t fork_count, BP_FORK_DISPATCH_POLICY policy);

	// waits for any dispatched pid to return/signal.
	size_t waitForAny();

	// debugging display function for visualizing
	bool displayInfo();

};



#endif /* BP_FORKDISPATCHER_H_ */

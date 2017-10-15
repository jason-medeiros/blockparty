/*
 * BP_ForkDispatcher.cc
 *
 *  Created on: Nov 18, 2011
 *      Author: root
 */

// include main includes
#include "../../../include/BP-Main.h"

// class constructor
BP_ForkDispatcher::BP_ForkDispatcher(size_t fork_count, BP_FORK_DISPATCH_POLICY policy)
{
	// set parameters
	this->max_running_pids = fork_count;
	this->dispatch_policy  = policy;
	this->wait_policy      = BP_FORK_WAIT_POLICY_WAIT_ALL;

	// nullify class pointers
	this->use_vfork = false;

	// zero out running pids
	this->running_pids = NULL;
	this->running_pid_n = 0;

	// set pid monitoring to 0
	this->enable_exit_pid_monitoring = 0;

	// top pid for dispatcher
	this->top_pid = getpid();

	// current pid for running process (forked)
	this->curr_pid = 0;

	// current fork pid index
	this->curr_fork_index = 0;


}
// overprototyped runForks routine to take parameters
BP_ERROR_T BP_ForkDispatcher::runForks(size_t fork_count, BP_FORK_DISPATCH_POLICY policy)
{

	// get current pid
	pid_t top_pid = getpid();

	// run fork loop
	size_t i = 0;
	for(; i < fork_count; i++)
	{

		// pid for fork
		pid_t pid = 0;

		// only fork from the top pid
		if(getpid() == top_pid)
		{

			// perform actual fork/vfork
			// if(this->use_vfork)
			//	pid = vfork();
			// else
			//	pid = fork();

			// *****************************


		     pid_t pid1;
		     pid_t pid2;
		     int status;

		     if (pid1 = fork()) {
		             /* parent process A */
		             waitpid(pid1, &status, NULL);
		     } else if (!pid1) {
		             /* child process B */
		             if (pid2 = fork()) {
		                     exit(0);
		             } else if (!pid2) {

						// CHILD:
						// any pid that isn't the top pid, gets top pid marked here
         				this->top_pid = top_pid;
         				return ERR_FAILURE;

		             } else {
		                     /* error */
		             }
		     } else {
		             /* error */
		     }

			// ****************************

			// only store pids if we're the parent
			if(getpid() == top_pid)
			{

				// store pid in parent
				this->running_pid_n++;
				this->running_pids = (P_BP_FORKDISPATCH_PARENT_PID_INFO) bprealloc(this->running_pids, sizeof(BP_FORKDISPATCH_PARENT_PID_INFO) * (this->running_pid_n+1));

				// truncate array
				memset((void *) &this->running_pids[this->running_pid_n-1], 0x00, sizeof(BP_FORKDISPATCH_PARENT_PID_INFO));
				memset((void *) &this->running_pids[this->running_pid_n],   0x00, sizeof(BP_FORKDISPATCH_PARENT_PID_INFO));

				// store pid in array
				this->running_pids[this->running_pid_n-1].pid = pid;

			}
			else
			{

				// any pid that isn't the top pid, gets top pid marked here
				this->top_pid = top_pid;

			}

		}
		else
		{
		}

	}

	// return success if in the parent (final check)
	if(getpid() == top_pid)
		return ERR_SUCCESS;
	else
		return ERR_FAILURE;

}


// runs all configured forks in the class (aka dispatch all).  Will
// never run more than the classes maximum running pids object.
BP_ERROR_T BP_ForkDispatcher::runForks()
{


	// get current pid
	pid_t top_pid = getpid();

	// run fork loop
	size_t i = 0;
	for(; i < this->max_running_pids; i++)
	{

		// pid for fork
		pid_t pid = 0;

		// only fork from the top pid
		if(getpid() == top_pid)
		{

			// perform actual fork/vfork
			if(this->use_vfork)
				pid = vfork();
			else
				pid = fork();

			// only store pids if we're the parent
			if(getpid() == top_pid)
			{

				// store pid in parent
				this->running_pid_n++;
				this->running_pids = (P_BP_FORKDISPATCH_PARENT_PID_INFO) bprealloc(this->running_pids, sizeof(BP_FORKDISPATCH_PARENT_PID_INFO) * (this->running_pid_n+1));

				// truncate array
				memset((void *) &this->running_pids[this->running_pid_n-1], 0x00, sizeof(BP_FORKDISPATCH_PARENT_PID_INFO));
				memset((void *) &this->running_pids[this->running_pid_n],   0x00, sizeof(BP_FORKDISPATCH_PARENT_PID_INFO));

				// store pid in array
				this->running_pids[this->running_pid_n-1].pid = pid;

			}
			else
			{
				this->top_pid = top_pid;
			}

		}
		else /* now in child */
		{

			// set current pid in class
			this->curr_pid = getpid();

			// set fork index for easy lookups
			this->curr_fork_index = i;

			// fork with err success if we are the child
			if(pid == (pid_t) 0)
				return ERR_FAILURE;

		}

	}

	// return success if in the parent (final check)
	if(getpid() == top_pid)
		return ERR_SUCCESS;
	else
		return ERR_FAILURE;

}

// waits for any pid in the running list (useful for loops)
size_t BP_ForkDispatcher::waitForAny()
{

	// get real current pid before comparison
	pid_t cur_pid = getpid();

	// can only wait from the top pid
	if(cur_pid != this->top_pid)
		return ERR_FAILURE;

	// child exit status
	int childExitStatus = 0;

	// while loop
	while(1)
	{

		// only wait if we have pids to wait on
		if(this->running_pid_n)
		if(this->running_pids)
		{

			// set exit status to 0
			childExitStatus = 0;

			// wait onn any child process
			pid_t ws = waitpid( -1, &childExitStatus, 0);
			if (ws == -1)
			{
				return -1;
			}

			// will hold the pid info structure
			P_BP_FORKDISPATCH_PARENT_PID_INFO p_info = NULL;

			// loop to calculate which pid structure to use
			size_t i = 0;
			for(; i < this->running_pid_n; i++)
			{

				if(this->running_pids[i].pid == ws)
				{
					p_info = (P_BP_FORKDISPATCH_PARENT_PID_INFO) &this->running_pids[i];
					break;
				}

			}

			if( WIFEXITED(childExitStatus) )
			{

				// if the process has exited, store exit status
				p_info->exited      = true;
				p_info->exit_status = WEXITSTATUS(childExitStatus);

				if(this->wait_policy == BP_FORK_WAIT_POLICY_RETURN_ON_EXIT)
					return p_info->pid;

			}

			if( WIFSIGNALED(childExitStatus) )
			{

				// if the process has exited by signal
				p_info->signaled    = true;
				p_info->exited      = true;
				p_info->last_signal = WTERMSIG(childExitStatus);

				if(this->wait_policy == BP_FORK_WAIT_POLICY_RETURN_ON_EXIT)
					return p_info->pid;

			}

		}

	}

	return 0;
}

BP_ForkDispatcher::~BP_ForkDispatcher()
{
	if(this->running_pids)
		bpfree(this->running_pids);
}

// debugging display function for visualizing
bool BP_ForkDispatcher::displayInfo()
{

	// display fork information
	printf("\n [+] Fork Dispatcher Info for %p", this);
	printf("\n\t using vfork: %s", this->use_vfork ? "yes" : "no");
	printf("\n\t max pids to dispatch: %u", this->max_running_pids);
	printf("\n\t dispatch policy: %u", this->dispatch_policy);

	// if we have running pids, walk and display them
	if(this->running_pid_n)
	if(this->running_pids)
	{

		// display running pid table
		printf("\n\t [+] Running PID table:");

		// iterate pids
		size_t i = 0;
		for(; i < this->running_pid_n; i++)
		{
			printf("\n\t\t [+] %u", this->running_pids[i].pid);
			printf("\n\t\t\t exited:      %s", this->running_pids[i].exited ? "yes" : "no");
			printf("\n\t\t\t exit status: %u", this->running_pids[i].exit_status);
			printf("\n\t\t\t signaled:    %s", this->running_pids[i].signaled ? "yes" : "no");
			printf("\n\t\t\t last signal: %u", this->running_pids[i].last_signal);

		}

	}

	// return indicating success
	return true;
}

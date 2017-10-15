/*
 * BP_CLTRunMonitor.cc
 *
 *  Created on: Sep 3, 2013
 *      Author: root
 */


// application runtime monitor
#include "../../../../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %% Constructor / Deconstructor %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// global os profiling session.  This is a singleton and is only ever
// allocated once.  It is thread safe and can be shared between any
// number of threads.  That said, don't abuse it (aka constant snapshots
// or you'll run into contention problems simply due to the amount of
// data you'll be processing.
extern P_BP_OS_PROFILING_SESSION  global_osprof_session;

// Class constructor.
BP_CLTRunMonitor::BP_CLTRunMonitor(int argc, char ** argv, char **envp)
{

	// initialize the local lock
	sem_init(&this->lock, 1, 1);

	// lock semaphore
	BP_CLT_RUN_MONITOR_LOCK;

	// ensure we have a global os profiling session (only one is ever
	// created in the entire application, no matter how many threads)
	if(!global_osprof_session)
		global_osprof_session = BP_CreateNewOSProfiler();

	// check to see that we have a global os profiling session (should
	// have one at this point) and exit with failed init code.
	if(!global_osprof_session)
	{
		this->clt_run_monitor_init_ok = false;
		return;
	}

	// use the global osprofiler session
	this->osprof_session = global_osprof_session;

	// if we couldn't create a new session, exit and set
	// init to false.
	if(!this->osprof_session)
	{
		this->clt_run_monitor_init_ok = false;
		return;
	}

	// create new thread safe linked list allocator for use
	// in this class.
	this->m = BP_CreateLinkL((char *) "BP_CLTRunMonitor Memory Allocator", BP_TRUE);

	// set application entry variables
	this->argc = argc;
	this->argv = argv;
	this->envp = envp;

	// set process search results to null
	this->process_search_indexes_n = 0;
	this->process_search_indexes   = NULL;

	// set init to to ok
	this->clt_run_monitor_init_ok = true;

	// unlock semaphore
	BP_CLT_RUN_MONITOR_UNLOCK;

}

// deconstructor
BP_CLTRunMonitor::~BP_CLTRunMonitor()
{

	// lock semaphore
	BP_CLT_RUN_MONITOR_LOCK;

	/*
	// destroy the os profiler if necessary
	if(this->osprof_session)
		BP_DestroyOSProfiler(this->osprof_session);

	// set the profile session to null
	this->osprof_session = NULL;
	*/

	// check to see if we have assigned an allocator
	if(this->m)
	{
		BP_DestroyLinkL(this->m, BP_TRUE);
		this->m = NULL;
	}

	// unlock the session
	BP_CLT_RUN_MONITOR_UNLOCK;

	// destroy the used semaphore
	sem_destroy(&this->lock);

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Utility Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// update the snapshot and refreshes all statistical information
BP_ERROR_T BP_CLTRunMonitor::updateSnapshot()
{

	// ensure we have a session
	if(!this->osprof_session)
		return ERR_FAILURE;

	// lock
	sem_wait(&this->lock);

	// update snapshot information
	BP_OSProfTakeSnapshotUpdateAllEntries(this->osprof_session);

	// set search indexes to zero
	this->process_search_indexes_n = 0;

	// unlock
	sem_post(&this->lock);

	// return indicating success
	return ERR_SUCCESS;

}


// Starts profiling if it's not already started.
BP_ERROR_T BP_CLTRunMonitor::startProfiling()
{

	// lock the profiler
	BP_CLT_RUN_MONITOR_LOCK;

	// if we don't have a singleton, create one
	if(!global_osprof_session)
	{
		// create new operating system profiler
		global_osprof_session = BP_CreateNewOSProfiler();

	}

	// set the session directly
	this->osprof_session = global_osprof_session;

	// set flat indicating we are running
	this->os_profiling_status = BP_CLT_OSPROF_RUN_STATUS_RUNNING;

	// unlock the profiler
	BP_CLT_RUN_MONITOR_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;

}


// Shuts down the global os profiling singleton (destroy/free).  This
// effectively prevents any CLT_RunMonitors from doing any profiling. It's
// important to note that this is not called from the class deconstructor.
// It must be called directly.
BP_ERROR_T BP_CLTRunMonitor::shutdownProfiling()
{

	// If we have no session, or we have already shut things down, just return.
	if(!this->osprof_session)
		return ERR_FAILURE;
	if(this->os_profiling_status == BP_CLT_OSPROF_RUN_STATUS_SHUTDOWN)
		return ERR_FAILURE;

	// lock the profiler
	BP_CLT_RUN_MONITOR_LOCK;

	// destroy the os profiler (also free's session memory)
	BP_DestroyOSProfiler(this->osprof_session);

	// set singleton pointers to NULL
	this->osprof_session  = NULL;
	global_osprof_session = NULL;

	// set flat indicating we are shut down
	this->os_profiling_status = BP_CLT_OSPROF_RUN_STATUS_SHUTDOWN;

	// lock the profiler
	BP_CLT_RUN_MONITOR_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%% Process Searching %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// search processes based on regexps (set NULL parameters for empty search).  This
// method returns in this->process_search_indexes(_n).
BP_ERROR_T BP_CLTRunMonitor::searchProcesses(char * regex_proc_name, char * regex_proc_title, pid_t pid)
{

	// lock
	BP_CLT_RUN_MONITOR_LOCK;

	// destroy indexes if they already exist
	if(this->process_search_indexes)
	{
		tq_free(this->osprof_session->m, this->process_search_indexes);
		this->process_search_indexes = NULL;
	}

	// reset index count
	this->process_search_indexes_n = 0;

	// search processes and titles for regex.  Returns
	size_t * results   = NULL;
	size_t   results_n = 0;

	// search processes
	results = BP_OSProfSearchProcessesPCRE(this->osprof_session, regex_proc_name, regex_proc_title, pid);

	// check if we have results
	size_t n = 0;
	if(results)
	{
		// count indexes till truncation
		for(; results[n] != OSPROF_SEARCH_INDEX_TRUNCATION; n++)
		{
			// can do whatever here
		}

	}

	// set class values from calculated searches
	this->process_search_indexes   = results;
	this->process_search_indexes_n = n;

	// unlock
	BP_CLT_RUN_MONITOR_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;

}


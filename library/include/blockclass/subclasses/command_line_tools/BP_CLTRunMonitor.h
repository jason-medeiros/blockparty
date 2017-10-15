/*
 * BP_CLTRunMonitor.h
 *
 *  Created on: Sep 3, 2013
 *      Author: root
 */

#ifndef BP_CLTRUNMONITOR_H_
#define BP_CLTRUNMONITOR_H_

// simple lock / unlock expanding macros
#define BP_CLT_RUN_MONITOR_LOCK    sem_wait(&this->lock)
#define BP_CLT_RUN_MONITOR_UNLOCK  sem_post(&this->lock)

// typedef enumerators for dealing with runstatus indicators.
typedef enum
{
	BP_CLT_OSPROF_RUN_STATUS_UNSET,
	BP_CLT_OSPROF_RUN_STATUS_RUNNING,
	BP_CLT_OSPROF_RUN_STATUS_SHUTDOWN
} BP_CLT_OSPROF_RUN_STATUS;

// application run monitor class
class BP_CLTRunMonitor : public Block
{

public:

	// flag indicating successful init
	bool clt_run_monitor_init_ok;

	// thread safe lock
	sem_t lock;

	// memory allocator
	P_BP_LINKL m;

	// OS profiling session
	P_BP_OS_PROFILING_SESSION osprof_session;

	// application monitor (these  values
	// are NOT duplicated.  Since there is only ever
	// one app monitor class, it's safe to use values
	// directly.  Only use if init ok.
	int     argc;
	char ** argv;
	char ** envp;

	// create application run monitor from program in
	BP_CLTRunMonitor(int argc, char ** argv, char **envp);
	~BP_CLTRunMonitor();

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Utility Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enum value of profiling status (see typedef above class)
	BP_CLT_OSPROF_RUN_STATUS os_profiling_status;

	// NOTE: The following routines directly operate on the global
	//       osprofiling singleton associated with this class.  So long
	//       as you are working with this class, you will be working with
	//       that global singleton.  This design was intended to prevent
	//       redundant interfacing with OS stat information.  That said, if
	//       you absolutely must have more than one interface, you can examine
	//       the BP_OSProfiling.(h/cc) files for information on how to use the
	//       underlying C capability directly.

	// update the snapshot and refreshes all statistical information
	BP_ERROR_T updateSnapshot();

	// Starts profiling if it's not already started.
	BP_ERROR_T startProfiling();

	// Shuts down the global os profiling singleton (destroy/free).  This
	// effectively prevents any CLT_RunMonitors from doing any profiling. It's
	// important to note that this is not called from the class deconstructor.
	// It must be called directly.
	BP_ERROR_T shutdownProfiling();

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%% Search Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// === Process Search Routines ===========

	// when you search for a process in the process list
	size_t * process_search_indexes;
	size_t   process_search_indexes_n;

	// search for processes (results are stored in the indices above).  Set either
	// regex_proc_name or regex_proc_title to NULL, to disable matching.  Set pid to -1 to
	// disable pid matching.  Set to NULL, NULL, -1 to get all pids.
	BP_ERROR_T searchProcesses(char * regex_proc_name, char * regex_proc_title, pid_t pid);



};




#endif /* BP_CLTRUNMONITOR_H_ */

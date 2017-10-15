/*
 * BP-LinuxBacktrace.h
 *
 *  Created on: Sep 9, 2015
 *      Author: root
 */

#ifndef INCLUDE_DEBUGGING_BP_LINUXBACKTRACE_H_
#define INCLUDE_DEBUGGING_BP_LINUXBACKTRACE_H_

// Basic structure for holding debug backtraces.  Only useful in Linux.
typedef struct __BP_LINUX_DEBUG_BACKTRACE {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% System Hash Registry %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// used for allocations and locks
	P_BP_HASH_TABLE_REGISTRY system_hreg;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Trace and Trace Symbols %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// trace (allocated via system hreg)
	char ** trace;

	// symbols (allocated via system hreg)
	char ** symbols;

	// the full depth of the trace
	size_t trace_depth_n;


} BP_LINUX_DEBUG_BACKTRACE, *P_BP_LINUX_DEBUG_BACKTRACE;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create and Destroy Backtraces %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// create new backtrace from current position of execution (max depth is how far
// it will go when trying to create the trace)
P_BP_LINUX_DEBUG_BACKTRACE BP_LinuxDebugCreateBacktrace(size_t max_depth = 1024);

// Destroy a backtrace and all associated memory.
BP_ERROR_T BP_LinuxDebugDestroyBacktrace(P_BP_LINUX_DEBUG_BACKTRACE bt);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Simple Utility to Create/Print/Destroy a Backtrace %%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// simple routine which creates/prints/destroys a backtrace.
BP_ERROR_T BP_LinuxDebugBacktraceSimple(size_t display_tab_depth);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Backtrace %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to display a backtrace if one is set
BP_ERROR_T BP_LinuxDebugDisplayBacktrace
(
		P_BP_LINUX_DEBUG_BACKTRACE bt,
		size_t                     display_tab_depth_n     = 0,
		size_t                     start_at_trace_index_n  = 0,
		size_t                     display_up_till_depth_n = 0,
		BP_BOOL                    display_header_data     = BP_TRUE,
		BP_BOOL                    display_trace_data      = BP_TRUE
);



#endif /* INCLUDE_DEBUGGING_BP_LINUXBACKTRACE_H_ */

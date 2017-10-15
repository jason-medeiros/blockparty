/*
 * BP-LinuxBacktrace.cc
 *
 *  Created on: Sep 9, 2015
 *      Author: root
 */

// include blockparty main
#include "../../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create and Destroy Backtraces %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// create new backtrace from current position of execution (max depth is how far
// it will go when trying to create the trace)
P_BP_LINUX_DEBUG_BACKTRACE BP_LinuxDebugCreateBacktrace(size_t max_depth)
{

	// ensure we have a max depth set
	if(!max_depth)
		return NULL;

	// create system hash registry
	P_BP_HASH_TABLE_REGISTRY system_hreg = BP_CreateHashTableRegistry();
	if(!system_hreg)
		return NULL;

	// allocate a new trace buffer
	P_BP_LINUX_DEBUG_BACKTRACE bt = (P_BP_LINUX_DEBUG_BACKTRACE) ht_calloc(system_hreg, sizeof(BP_LINUX_DEBUG_BACKTRACE), 1);
	if(!bt)
	{
		BP_DestroyHashTableRegistry(system_hreg);
		return NULL;
	}

	// set system hreg so we can easily clean things up
	bt->system_hreg = system_hreg;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Perform Actual Backtrace %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// allocate space for the trace data
	bt->trace = (char **) ht_calloc(system_hreg, sizeof(char *) * (max_depth * 1), 1);
	if(!bt->trace)
	{
		BP_DestroyHashTableRegistry(system_hreg);
		return NULL;
	}

	// gather backtrace (use int, not size_t)
	int bt_size = backtrace((void **) bt->trace, max_depth);

	// gather symbols
	char ** bt_syms = backtrace_symbols((void **) bt->trace, max_depth);
	if(!bt_syms)
	{
		BP_DestroyHashTableRegistry(system_hreg);
		return NULL;
	}

	// allocate space for the symbols
	bt->symbols = (char **) ht_calloc(system_hreg, sizeof(char *) * (bt_size+1), 1);

	// iterator
	size_t n = 0;

	// add all symbols
	for(; n < bt_size; n++)
	{

		// duplicate the symbol
		bt->symbols[n] = ht_strdup(system_hreg, bt_syms[n]);

	}


	// set the trace depth from the size
	bt->trace_depth_n = bt_size;


	// return the filled out backtrace (ready for display)
	return bt;

}

// Destroy a backtrace and all associated memory.
BP_ERROR_T BP_LinuxDebugDestroyBacktrace(P_BP_LINUX_DEBUG_BACKTRACE bt)
{

	// ensure we have a trace
	if(!bt)
		return ERR_FAILURE;

	// ensure the trace has a hreg
	if(!bt->system_hreg)
		return ERR_FAILURE;

	// destroy the hreg
	BP_DestroyHashTableRegistry(bt->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Simple Utility to Create/Print/Destroy a Backtrace %%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// simple routine which creates/prints/destroys a backtrace.
BP_ERROR_T BP_LinuxDebugBacktraceSimple(size_t display_tab_depth)
{

    // create a backtrace at depth 0 and display it
    P_BP_LINUX_DEBUG_BACKTRACE bt = BP_LinuxDebugCreateBacktrace();

    // display the backtrace
    BP_LinuxDebugDisplayBacktrace
	(
   		bt,
		display_tab_depth,
   		0,
   		100,
   		BP_TRUE,
   		BP_TRUE
	);

    // destroy the backtrace
    BP_LinuxDebugDestroyBacktrace(bt);

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Backtrace %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to display a backtrace if one is set
BP_ERROR_T BP_LinuxDebugDisplayBacktrace
(
		P_BP_LINUX_DEBUG_BACKTRACE bt,
		size_t                     display_tab_depth_n,
		size_t                     start_at_trace_index_n,
		size_t                     display_up_till_depth_n,
		BP_BOOL                    display_header_data,
		BP_BOOL                    display_trace_data
)
{

	// basic null checks
	if(!bt)
		return ERR_FAILURE;
	if(!bt->system_hreg)
		return ERR_FAILURE;
	if(!bt->symbols)
		return ERR_FAILURE;
	if(!bt->trace)
		return ERR_FAILURE;
	if(!bt->trace_depth_n)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Tab Buffer %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set tab buffer
	char * tab_buff = NULL;

	// ensure we have a tab depth
	if(display_tab_depth_n)
	{

		// create a new linkl
		P_BP_LINKL tab_tq = BP_CreateLinkL(".", BP_TRUE);

		// declare tab iterator
		size_t tab_iter = 0;
		for(; tab_iter < display_tab_depth_n; tab_iter++)
		{
			tq_strdup(tab_tq, "\t", BPLN);
		}

		// flatten the join string elements
		BP_LinkLFlattenJoinStringElements(tab_tq);

		// set tab buffer
		if(tab_tq->flat_buff)
			tab_buff = (char *) tab_tq->flat_buff;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Walk Backtrace and Allocate %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// display the header data
	if(display_header_data)
	{

		// display header
		printf("%s [+] Displaying Stack Backtrace:", tab_buff ? tab_buff : "");

	}

	// iterate through trace data
	size_t bt_iter = 0;
	for(; bt_iter < bt->trace_depth_n; bt_iter++)
	{

		// display symbol
		if(bt->symbols[bt_iter])
			printf("\n%s   [%u] %s", tab_buff ? tab_buff : "", bt_iter, bt->symbols[bt_iter]);

	}

	// return indicating success
	return ERR_SUCCESS;

}

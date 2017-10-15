/*
 * BP_OSProfiling.cc
 *
 *  Created on: Sep 2, 2013
 *      Author: root
 */



// include main header
#include "../../include/BP-Main.h"
#include <statgrab.h>

// global os profiling session.  This is a singleton and is only ever
// allocated once.  It is thread safe and can be shared between any
// number of threads.  That said, don't abuse it (aka constant snapshots
// or you'll run into contention problems simply due to the amount of
// data you'll be processing.
P_BP_OS_PROFILING_SESSION  global_osprof_session = NULL;

// Global flag letting us know that we have already created an os profiler session.  Ideally
// the profiler should always be a singleton.  Pure singleton.  One only.
size_t os_profiler_already_created_once = 0;

// Create a new empty osprof_session suitable for os profiling.
P_BP_OS_PROFILING_SESSION BP_CreateNewOSProfiler()
{

	if(!global_osprof_session)
	{

		// create new osprof_session structure
		global_osprof_session = (P_BP_OS_PROFILING_SESSION) bpcalloc(sizeof(BP_OS_PROFILING_SESSION), 1);

		// memset(global_osprof_session->lock, 0x00, sizeof(sem_t));

	}
	else
	{
		printf("\n [+] Return OSProf singleton");
		return global_osprof_session;
	}


	// check for a osprof_session
	if(!global_osprof_session)
		return NULL;



	// initialize the osprof_session semaphore
	sem_init(&global_osprof_session->lock, 1, 1);

	// init statgrab library
	sg_init(0);

	// create a new thread safe allocator
	global_osprof_session->m = BP_CreateLinkL("OS Profiling Memory Allocator", BP_TRUE);

	// set initialized flag
	global_osprof_session->initialized = P_BP_OS_PROFILING_SESSION_INITIALIZED;

	// set flag
	os_profiler_already_created_once = 1;

	// display creation message
	printf("\n [+] Global OS Profiler Created Successfully.");

	// return the osprof_session
	return global_osprof_session;

}

// Destroy a profiling osprof_session
BP_ERROR_T BP_DestroyOSProfiler(P_BP_OS_PROFILING_SESSION osprof_session)
{

	// check the osprof_session
	if(!osprof_session)
		return ERR_FAILURE;

	// lock session
	BP_OSPROF_SESSION_LOCK;

	// destroy all osprof_session leftover memory
	BP_DestroyLinkL(osprof_session->m, BP_TRUE);

	// reset profiler created flag here
	os_profiler_already_created_once = 0;

	// unlock just before destroy
	BP_OSPROF_SESSION_UNLOCK;

	// unlock the osprof_session lock
	sem_destroy(&osprof_session->lock);

	// call shutdown
	sg_shutdown();

	// free the session itself
	bpfree(osprof_session);

	// return indicating success
	return ERR_SUCCESS;

}


BP_ERROR_T BP_OSProfTakeSnapshotUpdateAllEntries(P_BP_OS_PROFILING_SESSION osprof_session)
{

	// ensure we have a session
	if(!osprof_session)
		return ERR_FAILURE;


	// printf("\n In BP_OSProfTakeSnapshotUpdateAllEntries!");

	// take snapshot
	BP_OSProfTakeSnapshot(osprof_session);

	// gen all info
	BP_OSProfGenHostInfo(osprof_session);
	BP_OSProfGenProcessStats(osprof_session);
	BP_OSProfGenCPUPercent(osprof_session);
	BP_OSProfGenCPUDiff(osprof_session);
	BP_OSProfGenLoadStats(osprof_session);
	BP_OSProfGenNetworkIOStats(osprof_session);
	BP_OSProfGenNetworkIfaceStats(osprof_session);
	BP_OSProfGenVirtualMemStats(osprof_session);
	BP_OSProfGenPageStats(osprof_session);
	BP_OSProfGenFSStats(osprof_session);
	BP_OSProfGenDiskIOStats(osprof_session);
	BP_OSProfGenUserStats(osprof_session);

	// printf("\n Exiting BP_OSProfTakeSnapshotUpdateAllEntries");


	// return indicating success
	return ERR_SUCCESS;

}

// Runs the sg_snapshot routine to generate a basic snapshot.
BP_ERROR_T BP_OSProfTakeSnapshot(P_BP_OS_PROFILING_SESSION osprof_session)
{

	if(!osprof_session)
		return ERR_FAILURE;

	// lock profiling session
	BP_OSPROF_SESSION_LOCK;

	// run snapshot routine
	sg_snapshot();

	// unlock profiling session
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Host Information Stats (sg_host_info) %%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// generate general host info
BP_ERROR_T BP_OSProfGenHostInfo(P_BP_OS_PROFILING_SESSION osprof_session)
{

	if(!osprof_session)
		return ERR_FAILURE;

	// lock the os session semaphore
	BP_OSPROF_SESSION_LOCK;

	// retrieve host information
	osprof_session->host_stats = sg_get_host_info(&osprof_session->host_stats_n);

	// unlock the os session semaphore
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;

}


// generate network interface stats
BP_ERROR_T BP_OSProfGenNetworkIfaceStats(P_BP_OS_PROFILING_SESSION osprof_session)
{

	// check that we have an os profiling session
	if(!osprof_session)
		return ERR_FAILURE;

	// lock the os session semaphore
	BP_OSPROF_SESSION_LOCK;

	// retrieve network interface stats
	size_t network_iface_stats_n = 0;
	osprof_session->network_iface_stats = sg_get_network_iface_stats(&osprof_session->network_iface_stats_n);

	// unlock the os session semaphore
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;

}

// generate network interface stats
BP_ERROR_T BP_OSProfGenProcessStats(P_BP_OS_PROFILING_SESSION osprof_session)
{

	// check that we have an os profiling session
	if(!osprof_session)
		return ERR_FAILURE;

	// lock the os session semaphore
	BP_OSPROF_SESSION_LOCK;

	// retrieve network interface stats
	osprof_session->process_stats = sg_get_process_stats(&osprof_session->process_stats_n);

	// unlock the os session semaphore
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;

}

// generate CPU information
BP_ERROR_T BP_OSProfGenCPUPercent(P_BP_OS_PROFILING_SESSION osprof_session)
{

	// check that we have an os profiling session
	if(!osprof_session)
		return ERR_FAILURE;

	// lock the os session semaphore
	BP_OSPROF_SESSION_LOCK;

	// retrieve the cpu percents
	osprof_session->cpu_percent = sg_get_cpu_percents(&osprof_session->cpu_percent_n);

	// unlock the os session semaphore
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;
}

// generate CPU information (diffs ->sg_cpu_percents with ->sg_cpu_stats)
BP_ERROR_T BP_OSProfGenCPUDiff(P_BP_OS_PROFILING_SESSION osprof_session)
{

	// check that we have an os profiling session
	if(!osprof_session)
		return ERR_FAILURE;

	// lock the os session semaphore
	BP_OSPROF_SESSION_LOCK;

	// cpu_diff_stats = sg_get_cpu_stats_diff(NULL) );
	// cpu_percent = sg_get_cpu_percents_of(sg_last_diff_cpu_percent, NULL);



	// get stats diff
	osprof_session->cpu_diff_stats = sg_get_cpu_stats_diff(&osprof_session->cpu_diff_stats_n);

	// unlock the os session semaphore
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;

}

// generate system load information
BP_ERROR_T BP_OSProfGenLoadStats(P_BP_OS_PROFILING_SESSION osprof_session)
{
	// check that we have an os profiling session
	if(!osprof_session)
		return ERR_FAILURE;

	// lock the os session semaphore
	BP_OSPROF_SESSION_LOCK;

	// fill in load stats
	osprof_session->load_stats = sg_get_load_stats(&osprof_session->load_stats_n);

	// unlock the os session semaphore
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;
}

// generate network io stats
BP_ERROR_T BP_OSProfGenNetworkIOStats(P_BP_OS_PROFILING_SESSION osprof_session)
{

	if(!osprof_session)
		return ERR_FAILURE;

	// lock session before display
	BP_OSPROF_SESSION_LOCK;

	// fetch network IO stats
	osprof_session->network_io_stats = sg_get_network_io_stats_diff(&osprof_session->network_io_stats_n);

	// unlock
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;

}


// generate Virtual Memory information
BP_ERROR_T BP_OSProfGenVirtualMemStats(P_BP_OS_PROFILING_SESSION osprof_session)
{
	if(!osprof_session)
		return ERR_FAILURE;

	// lock session before display
	BP_OSPROF_SESSION_LOCK;

	// get mem and swap stats
	osprof_session->mem_stats  = sg_get_mem_stats(&osprof_session->mem_stats_n);
	osprof_session->swap_stats = sg_get_swap_stats(&osprof_session->swap_stats_n);

	// unlock
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;
}


// generate page statistic information
BP_ERROR_T BP_OSProfGenPageStats(P_BP_OS_PROFILING_SESSION osprof_session)
{
	if(!osprof_session)
		return ERR_FAILURE;

	// lock session before display
	BP_OSPROF_SESSION_LOCK;

	// get mem and swap stats
	osprof_session->page_stats  = sg_get_page_stats_diff(&osprof_session->page_stats_n);

	// unlock
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;
}

// generate filesystem statistic information
BP_ERROR_T BP_OSProfGenFSStats(P_BP_OS_PROFILING_SESSION osprof_session)
{

	if(!osprof_session)
		return ERR_FAILURE;

	// lock session before display
	BP_OSPROF_SESSION_LOCK;

	// get mem and swap stats
	osprof_session->fs_stats  = sg_get_fs_stats(&osprof_session->fs_stats_n);

	// unlock
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;
}

// generate filesystem statistic information
BP_ERROR_T BP_OSProfGenDiskIOStats(P_BP_OS_PROFILING_SESSION osprof_session)
{

	if(!osprof_session)
		return ERR_FAILURE;

	// lock session before display
	BP_OSPROF_SESSION_LOCK;

	// get mem and swap stats
	osprof_session->diskio_stats  = sg_get_disk_io_stats_diff(&osprof_session->diskio_stats_n);

	// unlock
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;

}


// generate user information about a system
BP_ERROR_T BP_OSProfGenUserStats(P_BP_OS_PROFILING_SESSION osprof_session)
{

	if(!osprof_session)
		return ERR_FAILURE;

	// lock session before display
	BP_OSPROF_SESSION_LOCK;

	// get mem and swap stats
	osprof_session->users  = sg_get_user_stats(&osprof_session->users_n);

	// unlock
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% OSProf Search Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// NOTE: Search routines return (size_t *) which is a (size_t)-1 terminated
//       stack of indexes.  It is not NULL terminated because NULL is a zero
//       index in this case.

// search processes and titles for regex.  Returns
size_t * BP_OSProfSearchProcessesPCRE
(
	P_BP_OS_PROFILING_SESSION osprof_session,
	char * regex_proc_name,
	char * regex_proc_title,
	pid_t pid
)
{

	if(!osprof_session)
		return NULL;

	// lock session before display
	BP_OSPROF_SESSION_LOCK;

	// create return index stack
	size_t * ret_indexes = NULL;
	size_t   ret_index_n = 0;

	// walk all processes in the list and search on regexps
	size_t n = 0;
	for(; n < osprof_session->process_stats_n; n++)
	{

		// if we're doing pid matching, run checks here
		if(pid != (pid_t) -1)
		if(pid != osprof_session->process_stats[n].pid)
			continue;

		// set regexp match
		BP_REGEX_MATCH_INFO match = {0};
		memset(&match, 0x00, sizeof(match));

		// flags for indicating if we match
		size_t proc_name_matched  = 0;
		size_t proc_title_matched = 0;

		if(regex_proc_name)
		if(osprof_session->process_stats[n].process_name)
		if(BP_MatchRegularExpressionPCRE(osprof_session->process_stats[n].process_name, bpstrlen(osprof_session->process_stats[n].process_name), regex_proc_name, 0, 1024, &match, 0) == ERR_SUCCESS)
		{
			proc_name_matched=1;
		}

		// destroy regexp match info
		BP_DestroyRegexpMatchInfo(&match);
		memset(&match, 0x00, sizeof(match));

		// check title regex
		if(regex_proc_title)
		if(osprof_session->process_stats[n].proctitle)
		if(BP_MatchRegularExpressionPCRE(osprof_session->process_stats[n].proctitle, bpstrlen(osprof_session->process_stats[n].proctitle), regex_proc_title, 0, 1024, &match, 0) == ERR_SUCCESS)
		{
			proc_title_matched=1;
		}

		// destroy regexp match info
		BP_DestroyRegexpMatchInfo(&match);
		memset(&match, 0x00, sizeof(match));


		// === Now Run Match Check Logic =================

		// check to see if proc name is matched
		if(regex_proc_name)
		if(!proc_name_matched)
			continue;

		// check to see if proc title is set
		if(regex_proc_title)
		if(!proc_title_matched)
			continue;

		// increment the index count
		ret_index_n++;

		// at this point we've confirmed a match
		ret_indexes = (size_t *) tq_realloc(osprof_session->m, ret_indexes, sizeof(size_t) * (ret_index_n+1), BPLN);

		// set the ret index and truncate (for quick searching)
		ret_indexes[ret_index_n-1] = n;
		ret_indexes[ret_index_n]   = OSPROF_SEARCH_INDEX_TRUNCATION;

	}

	// unlock session after display
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ret_indexes;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% OSProf Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// display general host information
BP_ERROR_T BP_OSProfDisplayHostInfo(P_BP_OS_PROFILING_SESSION osprof_session)
{

	if(!osprof_session)
		return ERR_FAILURE;

	// lock session before display
	BP_OSPROF_SESSION_LOCK;

	if(osprof_session->host_stats)
	{
		printf("\n [+] Displaying Host Information: %p", osprof_session->host_stats);
		printf("\n\t OS Name:       %s", osprof_session->host_stats->os_name);
		printf("\n\t OS Release:    %s", osprof_session->host_stats->os_release);
		printf("\n\t OS Version:    %s", osprof_session->host_stats->os_version);
		printf("\n\t OS HWPlatform: %s", osprof_session->host_stats->platform);
		printf("\n\t Hostname:      %s", osprof_session->host_stats->hostname);
		printf("\n\t Uptime:        %lld", osprof_session->host_stats->uptime);
	}

	// unlock session after display
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;
}

// display network interface statistics
BP_ERROR_T BP_OSProfDisplayNetworkIfaceStats(P_BP_OS_PROFILING_SESSION osprof_session)
{
	if(!osprof_session)
		return ERR_FAILURE;

	if(!osprof_session->network_iface_stats)
			return ERR_FAILURE;

	// lock session before display
	BP_OSPROF_SESSION_LOCK;

	printf("\n [+] Displaying Network Interfaces:");
	// walk interfaces and display
	size_t n = 0;
	for(; n < osprof_session->network_iface_stats_n; n++)
	{
		printf("\n [+] Interface: %s", osprof_session->network_iface_stats[n].interface_name);
		printf("\n\t Link Speed (MB):  %i", osprof_session->network_iface_stats[n].speed);
		printf("\n\t Link Status:      %s", osprof_session->network_iface_stats[n].up ? "up" : "down" );
		printf("\n\t Link Duplex:      %s", (osprof_session->network_iface_stats[n].duplex == SG_IFACE_DUPLEX_FULL) ? "Full Duplex" : "Half Duplex (or unknown)");
	}
	printf("\n");
	// unlock
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;
}

// display process statistics
BP_ERROR_T BP_OSProfDisplayProcessStats(P_BP_OS_PROFILING_SESSION osprof_session)
{
	if(!osprof_session)
		return ERR_FAILURE;

	if(!osprof_session->process_stats)
		return ERR_FAILURE;

	// lock session before display
	BP_OSPROF_SESSION_LOCK;

	// display banner
	printf("\n [+] OSProf - Displaying Process Information: ");

	printf
	(
		"\n%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n",
		"pid",
		"ppid",
		"pgid",
		"uid",
		"euid",
		"gid",
		"egid",
		"size",
		"res",
		"time",
		"cpu",
		"nice",
		"state",
		"name",
		"title"
	);

	// walk processes and display
	size_t n = 0;
	for(; n < osprof_session->process_stats_n; n++)
	{
		char *state = NULL;

		sg_process_stats * ps = &osprof_session->process_stats[n];
		switch (ps->state)
		{
			case SG_PROCESS_STATE_RUNNING:
				state = "RUNNING";
				break;
			case SG_PROCESS_STATE_SLEEPING:
				state = "SLEEPING";
				break;
			case SG_PROCESS_STATE_STOPPED:
				state = "STOPPED";
				break;
			case SG_PROCESS_STATE_ZOMBIE:
				state = "ZOMBIE";
				break;
			case SG_PROCESS_STATE_UNKNOWN:
			default:
				state = "UNKNOWN";
				break;
		}

		printf("%5u %5u %5u %5u %5u %5u %5u %5uM %5uM %8ds %10f %4d %-8s %s %s\n",
			(unsigned)ps->pid,   // 1
			(unsigned)ps->parent,// 2
			(unsigned)ps->pgid,  // 3
			(unsigned)ps->uid,   // 4
			(unsigned)ps->euid,  // 5
			(unsigned)ps->gid,   // 6
			(unsigned)ps->egid,  // 7
			(unsigned)(ps->proc_size / (1024*1024)), // 8
			(unsigned)(ps->proc_resident / (1024*1024)), // 9
			(unsigned)ps->time_spent, // 10
			(float)ps->cpu_percent, // 11
			ps->nice,            // 12
			state,               // 13
			ps->process_name,    // 14
			ps->proctitle ? ps->proctitle : "" // 15
		);


	}
	printf("\n");

	// unlock
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;
}



// Display CPU Last Percentage
BP_ERROR_T BP_OSProfDisplayCPUPercent(P_BP_OS_PROFILING_SESSION osprof_session)
{
	if(!osprof_session)
		return ERR_FAILURE;

	if(!osprof_session->cpu_percent)
		return ERR_FAILURE;

	// lock session before display
	BP_OSPROF_SESSION_LOCK;

	// display banner
	printf("\n [+] OSProf - Displaying CPU Percentages: ");
	printf("\n\t idle:   %f",   osprof_session->cpu_percent->idle);
	printf("\n\t iowait: %f",   osprof_session->cpu_percent->iowait);
	printf("\n\t kernel: %f",   osprof_session->cpu_percent->kernel);
	printf("\n\t nice:   %f",   osprof_session->cpu_percent->nice);
	printf("\n\t swap:   %f",   osprof_session->cpu_percent->swap);
	printf("\n\t time:   %lld", osprof_session->cpu_percent->time_taken);
	printf("\n\t user:   %f",   osprof_session->cpu_percent->user);
	printf("\n");

	// unlock
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;
}

// Display last CPU Diff
BP_ERROR_T BP_OSProfDisplayCPUDiffPercentages(P_BP_OS_PROFILING_SESSION osprof_session)
{
	if(!osprof_session)
		return ERR_FAILURE;

	if(!osprof_session->cpu_diff_percents)
		return ERR_FAILURE;

	// lock session before display
	BP_OSPROF_SESSION_LOCK;

	// display banner
	printf("\n [+] OSProf - DIFF CPU Percentages: ");
	printf("\n\t idle:   %f",   osprof_session->cpu_diff_percents->idle);
	printf("\n\t iowait: %f",   osprof_session->cpu_diff_percents->iowait);
	printf("\n\t kernel: %f",   osprof_session->cpu_diff_percents->kernel);
	printf("\n\t nice:   %f",   osprof_session->cpu_diff_percents->nice);
	printf("\n\t swap:   %f",   osprof_session->cpu_diff_percents->swap);
	printf("\n\t time:   %lld", osprof_session->cpu_diff_percents->time_taken);
	printf("\n\t user:   %f",   osprof_session->cpu_diff_percents->user);
	printf("\n");

	// unlock
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;
}

// Display last CPU Diff
BP_ERROR_T BP_OSProfDisplayCPUDiffStats(P_BP_OS_PROFILING_SESSION osprof_session)
{
	if(!osprof_session)
		return ERR_FAILURE;

	if(!osprof_session->cpu_diff_stats)
		return ERR_FAILURE;

	// lock session before display
	BP_OSPROF_SESSION_LOCK;

	// display banner
	printf("\n [+] OSProf - DIFF CPU Stats: ");
	printf("\n\t idle:   %llu",   osprof_session->cpu_diff_stats->idle);
	printf("\n\t iowait: %llu",   osprof_session->cpu_diff_stats->iowait);
	printf("\n\t kernel: %llu",   osprof_session->cpu_diff_stats->kernel);
	printf("\n\t nice:   %llu",   osprof_session->cpu_diff_stats->nice);
	printf("\n\t swap:   %llu",   osprof_session->cpu_diff_stats->swap);
	printf("\n\t time:   %llu", osprof_session->cpu_diff_stats->systime);
	printf("\n\t time:   %llu", osprof_session->cpu_diff_stats->total);
	printf("\n\t user:   %llu",   osprof_session->cpu_diff_stats->user);
	printf("\n");

	// unlock
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;
}

// Display load information
BP_ERROR_T BP_OSProfDisplayLoadStats(P_BP_OS_PROFILING_SESSION osprof_session)
{

	if(!osprof_session)
		return ERR_FAILURE;

	if(!osprof_session->load_stats)
		return ERR_FAILURE;

	// lock session before display
	BP_OSPROF_SESSION_LOCK;

	// display banner
	printf("\n [+] OSProf - Load Information: ");
	printf("\n\t load 1:   %5.2f",   osprof_session->load_stats->min1);
	printf("\n\t load 5:   %5.2f",   osprof_session->load_stats->min5);
	printf("\n\t load 15:  %5.2f",   osprof_session->load_stats->min15);
	printf("\n");

	// unlock
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;
}

// generate network io stats
BP_ERROR_T BP_OSProfDisplayNetworkIOStats(P_BP_OS_PROFILING_SESSION osprof_session)
{

	if(!osprof_session)
		return ERR_FAILURE;

	if(!osprof_session->network_io_stats)
		return ERR_FAILURE;

	// lock session before display
	BP_OSPROF_SESSION_LOCK;

	// display banner
	printf("\n [+] OSProf - Network IO Information: ");

	size_t n = 0;
	for(; n < osprof_session->network_io_stats_n; n++)
	{

		printf("\n [+] Interface: %s", osprof_session->network_io_stats[n].interface_name);
		printf("\n\t tx:         %lld", osprof_session->network_io_stats[n].tx);
		printf("\n\t rx:         %lld", osprof_session->network_io_stats[n].rx);
		printf("\n\t collisions: %lld", osprof_session->network_io_stats[n].collisions);
		printf("\n\t ierrors:    %lld", osprof_session->network_io_stats[n].ierrors);
		printf("\n\t ipackets:   %lld", osprof_session->network_io_stats[n].ipackets);
		printf("\n\t oerrors:    %lld", osprof_session->network_io_stats[n].oerrors);
		printf("\n\t opackets:   %lld", osprof_session->network_io_stats[n].opackets);
		printf("\n\t systime:    %lld", osprof_session->network_io_stats[n].systime);


		printf("\n");
	}

	// unlock
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;
}

// generate VM information
BP_ERROR_T BP_OSProfDisplayVirtualMemStats(P_BP_OS_PROFILING_SESSION osprof_session)
{
	if(!osprof_session)
		return ERR_FAILURE;

	// ensure we have stat pointers
	if(!osprof_session->mem_stats)
		return ERR_FAILURE;
	if(!osprof_session->swap_stats)
		return ERR_FAILURE;

	// lock session before display
	BP_OSPROF_SESSION_LOCK;

	// display banner
	printf("\n [+] OSProf - Memory Info: ");
	printf("\ntotal mem:  %llu",   osprof_session->mem_stats->total);
	printf("\nused  mem:  %llu",   osprof_session->mem_stats->used);
	printf("\ncache mem:  %llu",   osprof_session->mem_stats->cache);
	printf("\nfree  mem:  %llu",   osprof_session->mem_stats->free);
	printf("\ntotal swap: %llu",   osprof_session->swap_stats->total);
	printf("\nused  swap: %llu",   osprof_session->swap_stats->used);
	printf("\nfree  swap: %llu",   osprof_session->swap_stats->free);
	printf("\n");

	// unlock
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;

}


// generate page statistic information
BP_ERROR_T BP_OSProfDisplayPageStats(P_BP_OS_PROFILING_SESSION osprof_session)
{
	if(!osprof_session)
		return ERR_FAILURE;

	// ensure we have stat pointers
	if(!osprof_session->mem_stats)
		return ERR_FAILURE;
	if(!osprof_session->swap_stats)
		return ERR_FAILURE;

	// lock session before display
	BP_OSPROF_SESSION_LOCK;

	// display banner
	printf("\n [+] OSProf - Page Stats: ");
	printf("\n\t    pages in:   %llu",   osprof_session->page_stats->pages_pagein);
	printf("\n\t    pages out:  %llu",   osprof_session->page_stats->pages_pageout);
	printf("\n");

	// unlock
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;

}

// generate filesystem statistic information
BP_ERROR_T BP_OSProfDisplayFSStats(P_BP_OS_PROFILING_SESSION osprof_session)
{

	if(!osprof_session)
		return ERR_FAILURE;

	if(!osprof_session->fs_stats)
		return ERR_FAILURE;

	// lock session before display
	BP_OSPROF_SESSION_LOCK;

	// display banner
	printf("\n [+] OSProf - Filesystem Stats: ");
	printf( "\n%-16s %-24s %-8s %16s %16s %16s %7s %7s %7s %7s %9s %9s %7s %7s %7s %7s",
			"device", "mountpt", "fstype", "size", "used", "avail",
			"i-total", "i-used", "i-free", "i-avail",
			"io size", "block size",
			"b-total", "b-used", "b-free", "b-avail");

	size_t n = 0;
	for(; n < osprof_session->fs_stats_n; n++)
	{

		printf( "\n%-16s %-24s %-8s %16llu %16llu %16llu %7llu %7llu %7llu %7llu %9llu %9llu %7llu %7llu %7llu %7llu",
			osprof_session->fs_stats[n].device_name,
			osprof_session->fs_stats[n].mnt_point,
			osprof_session->fs_stats[n].fs_type,
			osprof_session->fs_stats[n].size,
			osprof_session->fs_stats[n].used,
			osprof_session->fs_stats[n].avail,
			osprof_session->fs_stats[n].total_inodes,
			osprof_session->fs_stats[n].used_inodes,
			osprof_session->fs_stats[n].free_inodes,
			osprof_session->fs_stats[n].avail_inodes,
			osprof_session->fs_stats[n].io_size,
			osprof_session->fs_stats[n].block_size,
			osprof_session->fs_stats[n].total_blocks,
			osprof_session->fs_stats[n].used_blocks,
			osprof_session->fs_stats[n].free_blocks,
			osprof_session->fs_stats[n].avail_blocks
		);

		printf("\n");

	}

	// unlock
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;

}

// generate filesystem statistic information
BP_ERROR_T BP_OSProfDisplayDiskIOStats(P_BP_OS_PROFILING_SESSION osprof_session)
{

	if(!osprof_session)
		return ERR_FAILURE;

	if(!osprof_session->diskio_stats)
		return ERR_FAILURE;

	// lock session before display
	BP_OSPROF_SESSION_LOCK;

	// display banner
	printf("\n [+] OSProf - Disk IO Stats: ");

	size_t n = 0;
	for(; n < osprof_session->diskio_stats_n; n++)
	{

		printf("\n [+] Disk: %s", osprof_session->diskio_stats[n].disk_name);
		printf("\n\t disk read:  %llu ", osprof_session->diskio_stats[n].read_bytes);
		printf("\n\t disk write: %llu ", osprof_session->diskio_stats[n].write_bytes);
		printf("\n\t systime:    %llu ", osprof_session->diskio_stats[n].systime);

		printf("\n");

	}

	// unlock
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;
}

// generate user information about a system
BP_ERROR_T BP_OSProfDisplayUserStats(P_BP_OS_PROFILING_SESSION osprof_session)
{
	if(!osprof_session)
		return ERR_FAILURE;

	if(!osprof_session->users)
		return ERR_FAILURE;

	// lock session before display
	BP_OSPROF_SESSION_LOCK;

	// display banner
	printf("\n [+] OSProf - User Information: ");

	printf("\n [+] user entries: %i", osprof_session->users_n);
	// walk entries
	size_t n = 0;
	for(; n < osprof_session->users_n; n++)
	{

		printf("\n [+] User Entry:");
		printf("\n\t username:    %s", osprof_session->users[n].login_name);
		printf("\n\t device:      %s", osprof_session->users[n].device);
		printf("\n\t hostname:    %s", osprof_session->users[n].hostname);
		printf("\n\t record_id:   %s", osprof_session->users[n].record_id);


		printf("\n");

	}

	// unlock
	BP_OSPROF_SESSION_UNLOCK;

	// return indicating success
	return ERR_SUCCESS;
}


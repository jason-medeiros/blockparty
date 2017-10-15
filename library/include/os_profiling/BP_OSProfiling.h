/*
 * BP_OSProfiling.h
 *
 *  Created on: Sep 2, 2013
 *      Author: root
 */

#ifndef BP_OSPROFILING_H_
#define BP_OSPROFILING_H_

// status of profiler
typedef enum
{
	P_BP_OS_PROFILING_SESSION_UNSET,
	P_BP_OS_PROFILING_SESSION_INITIALIZED,
	P_BP_OS_PROFILING_SESSION_GENERIC_ERROR
} P_BP_OS_PROFILING_SESSION_STATUS;


// Blockparty OS profiling information osprof_session.  Used to store operating
// system profiling data.
typedef struct __BP_OS_PROFILING_SESSION {

	// get host information (done)
	sg_host_info *host_stats;
	size_t        host_stats_n;

	// process information (done)
	sg_process_stats *process_stats;
	size_t            process_stats_n;

	// cpu information
	sg_cpu_percents *cpu_percent;
	size_t           cpu_percent_n;

	// cpu differential stats
	sg_cpu_percents *cpu_diff_percents;
	size_t           cpu_diff_percents_n;

	sg_cpu_stats    *cpu_diff_stats;
	size_t           cpu_diff_stats_n;

	// system load information
	sg_load_stats *load_stats;
	size_t         load_stats_n;

	// network I/O stats
	sg_network_io_stats *network_io_stats;
	size_t               network_io_stats_n;

	// network interface stats
	sg_network_iface_stats *network_iface_stats;
	size_t                  network_iface_stats_n;

	// memory statistics
	sg_mem_stats *mem_stats;
	size_t        mem_stats_n;

	// swap statistics
	sg_swap_stats *swap_stats;
	size_t         swap_stats_n;


	// calculate page statistics
	sg_page_stats *page_stats;
	size_t         page_stats_n;

	// filesystem stats
	sg_fs_stats *  fs_stats;
	size_t         fs_stats_n;

	// disk I/O stats
	sg_disk_io_stats *diskio_stats;
	size_t            diskio_stats_n;

	// user system user information
	sg_user_stats *users;
	size_t         users_n;


	// === Session Diagnostic Data ==========

	// flag indicating we're initialized
	P_BP_OS_PROFILING_SESSION_STATUS initialized;

	// thread safe semaphore lock
	sem_t lock;

	// osprof_session memory allocator
	P_BP_LINKL m;

} BP_OS_PROFILING_SESSION, *P_BP_OS_PROFILING_SESSION;

// OS semaphore locks expansion macros
#define BP_OSPROF_SESSION_UNLOCK sem_post(&osprof_session->lock)
#define BP_OSPROF_SESSION_LOCK   sem_wait(&osprof_session->lock)

// Create a new empty osprof_session suitable for os profiling.
P_BP_OS_PROFILING_SESSION BP_CreateNewOSProfiler();

// Destroy a profiling osprof_session
BP_ERROR_T BP_DestroyOSProfiler(P_BP_OS_PROFILING_SESSION osprof_session);

// Runs the sg_snapshot routine to generate a basic snapshot.
BP_ERROR_T BP_OSProfTakeSnapshot(P_BP_OS_PROFILING_SESSION osprof_session);

// This calls all Gen* functions below.
BP_ERROR_T BP_OSProfTakeSnapshotUpdateAllEntries(P_BP_OS_PROFILING_SESSION osprof_session);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% OSProf Stat Generators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// generate general host information
BP_ERROR_T BP_OSProfGenHostInfo(P_BP_OS_PROFILING_SESSION osprof_session);

// generate network interface stats
BP_ERROR_T BP_OSProfGenProcessStats(P_BP_OS_PROFILING_SESSION osprof_session);

// generate CPU information
BP_ERROR_T BP_OSProfGenCPUPercent(P_BP_OS_PROFILING_SESSION osprof_session);

// generate CPU information (diffs ->sg_cpu_percents with ->sg_cpu_stats)
BP_ERROR_T BP_OSProfGenCPUDiff(P_BP_OS_PROFILING_SESSION osprof_session);

// generate system load information
BP_ERROR_T BP_OSProfGenLoadStats(P_BP_OS_PROFILING_SESSION osprof_session);

// generate network io stats
BP_ERROR_T BP_OSProfGenNetworkIOStats(P_BP_OS_PROFILING_SESSION osprof_session);

// generate network interface stats
BP_ERROR_T BP_OSProfGenNetworkIfaceStats(P_BP_OS_PROFILING_SESSION osprof_session);

// generate VM information
BP_ERROR_T BP_OSProfGenVirtualMemStats(P_BP_OS_PROFILING_SESSION osprof_session);

// generate page statistic information
BP_ERROR_T BP_OSProfGenPageStats(P_BP_OS_PROFILING_SESSION osprof_session);

// generate filesystem statistic information
BP_ERROR_T BP_OSProfGenFSStats(P_BP_OS_PROFILING_SESSION osprof_session);

// generate filesystem statistic information
BP_ERROR_T BP_OSProfGenDiskIOStats(P_BP_OS_PROFILING_SESSION osprof_session);

// generate user information about a system
BP_ERROR_T BP_OSProfGenUserStats(P_BP_OS_PROFILING_SESSION osprof_session);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% OSProf Search Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// NOTE: Search routines return (size_t *) which is a (size_t)-1 terminated
//       stack of indexes into the relevant stack.  It is not NULL terminated
//       because NULL is a zero index in this case.
#define OSPROF_SEARCH_INDEX_TRUNCATION (size_t) -1

// search processes and titles for regex.  Returns
size_t * BP_OSProfSearchProcessesPCRE(P_BP_OS_PROFILING_SESSION osprof_session, char * regex_proc_name, char * regex_proc_title, pid_t pid);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% OSProf Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// display general host information
BP_ERROR_T BP_OSProfDisplayHostInfo(P_BP_OS_PROFILING_SESSION osprof_session);

// display network interface statistics
BP_ERROR_T BP_OSProfDisplayNetworkIfaceStats(P_BP_OS_PROFILING_SESSION osprof_session);

// display process statistics
BP_ERROR_T BP_OSProfDisplayProcessStats(P_BP_OS_PROFILING_SESSION osprof_session);

// Display CPU Last Percentage
BP_ERROR_T BP_OSProfDisplayCPUPercent(P_BP_OS_PROFILING_SESSION osprof_session);

// Display last CPU Diff
BP_ERROR_T BP_OSProfDisplayCPUDiffPercentages(P_BP_OS_PROFILING_SESSION osprof_session);

// Display last CPU Diff Stats
BP_ERROR_T BP_OSProfDisplayCPUDiffStats(P_BP_OS_PROFILING_SESSION osprof_session);

// Display load information
BP_ERROR_T BP_OSProfDisplayLoadStats(P_BP_OS_PROFILING_SESSION osprof_session);

// Display network io stats
BP_ERROR_T BP_OSProfDisplayNetworkIOStats(P_BP_OS_PROFILING_SESSION osprof_session);

// generate VM information
BP_ERROR_T BP_OSProfDisplayVirtualMemStats(P_BP_OS_PROFILING_SESSION osprof_session);

// generate page statistic information
BP_ERROR_T BP_OSProfDisplayPageStats(P_BP_OS_PROFILING_SESSION osprof_session);

// generate filesystem statistic information
BP_ERROR_T BP_OSProfDisplayFSStats(P_BP_OS_PROFILING_SESSION osprof_session);

// generate filesystem statistic information
BP_ERROR_T BP_OSProfDisplayDiskIOStats(P_BP_OS_PROFILING_SESSION osprof_session);

// generate user information about a system
BP_ERROR_T BP_OSProfDisplayUserStats(P_BP_OS_PROFILING_SESSION osprof_session);



#endif /* BP_OSPROFILING_H_ */

/*
 * BP-NMAP.h
 *
 *  Created on: Jul 19, 2016
 *      Author: root
 */

#ifndef INCLUDE_NETWORK_NMAP_BP_NMAP_H_
#define INCLUDE_NETWORK_NMAP_BP_NMAP_H_



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% NMAP Thread Locking Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// enter critical section for thread (relies on a managed thread pointer)
#define BP_NMAP_THREAD_ENTER_CRITICAL_SECTION\
	{\
		BP_HASH_TABLE_ENTER_CRITICAL_SECTION(managed_thread->system_hreg_ptr);\
	}\

// leave critical section for thread (relies on a managed_thread pointer)
#define BP_NMAP_THREAD_LEAVE_CRITICAL_SECTION\
	{\
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(managed_thread->system_hreg_ptr);\
	}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% NMAP Static Strings %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// string indicating that the nmap process is done
#define NMAP_SCAN_FINISHED_INDICATOR "Nmap done"

// This is the name of the event processor for handling tor events.
#define NMAP_EVENT_PROCESSOR_NAME "BP_TorEventProcessor"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% NMAP Sessions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// event types
typedef enum
{
	BP_NMAP_EVENT_UNSET,
	BP_NMAP_EVENT_START_NMAP,
	BP_NMAP_EVENT_STOP_NMAP,
	BP_NMAP_EVENT_RESTART_NMAP
} BP_NMAP_EVENT_TYPE;

// event return codes
typedef enum
{
	BP_NMAP_EVENT_RETURN_FAILURE,
	BP_NMAP_EVENT_RETURN_SUCCESS
} BP_NMAP_EVENT_RETURN_CODE;

// event status types
typedef enum
{

	BP_NMAP_EVENT_STATUS_UNSTARTED,
	BP_NMAP_EVENT_STATUS_STARTING_UP,
	BP_NMAP_EVENT_STATUS_RUNNING,
	BP_NMAP_EVENT_STATUS_STOPPING,
	BP_NMAP_EVENT_STATUS_FINISHED

} BP_NMAP_EVENT_STATUS;

typedef enum
{
	BP_NMAP_SCAN_TYPE_UNSET,
	BP_NMAP_SCAN_TYPE_SYN_SCAN,
	BP_NMAP_SCAN_TYPE_TCP_CONNECT_SCAN,
	BP_NMAP_SCAN_TYPE_TCP_CONNECT_SCAN_WITH_PROXYCHAINS,
	BP_NMAP_SCAN_TYPE_NULL_SCAN,
	BP_NMAP_SCAN_TYPE_FIN_SCAN,
	BP_NMAP_SCAN_TYPE_XMAS_SCAN

} BP_NMAP_SCAN_TYPE;


typedef struct __BP_NMAP {


	// event manager
	P_BP_EVENT_MANAGER evm;

	// uuid of the event used to start this nmap session.  Used for main event loop pickups.
	char * start_event_uuid;

	// The uuid of the last stop request.  Used for main event loop pickups.
	char * stop_event_uuid;

	// current running status as reported by the event manager
	BP_NMAP_EVENT_STATUS run_status;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% System Registries %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// system hash table registry
	P_BP_HASH_TABLE_REGISTRY system_hreg;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Threads %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Command and control sessions are started within threads, this
	// manager is used to control/dispatch them.
	P_BP_THREAD_MANAGER nmap_threads;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Command / Control Session %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// flag indicating if the command and control session has started ok
	BP_BOOL cc_started_ok;
	BP_BOOL cc_exited_ok;

	// file descriptor set used by the cc session to manipulate the
	P_BP_FDSET nmap_app;

	// command and control session for manipulating tor
	P_BP_CC_SESSION cc_session;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Tor Environment %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// envp passed through to all nmap
	char ** nmap_bin_envp;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Basic Configurations %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// path on the filesystem to find the proxychains-ng binary (needed for running nmap securely)
	char *proxychains_ng_bin_path;

	// path on the filesystem to find the proxychains-ng configuration (note, this software will NEVER
	// use the proxychains-ng default)
	char *proxychains_ng_conf_path;

	// path on the filesystem to find the nmap binary
	char *nmap_bin_path;

	// sha1 of the binary being used to nmap
	char *nmap_bin_sha1;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Service Run Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// start time
	timeval service_start_time;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% NMAP Custom Configuration %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ip address to scan (only ever scan one IP at a time to prevent potential sidechannel nonsense.  If you want to
	// scan more than one host at a time, simply create more nmap sessions and give each session it's own IP)
	char *ip_to_scan;

	// Type of scan to perform
	BP_NMAP_SCAN_TYPE scan_type;

	// This is the directory the nmap xml output will be stored.
	char *output_directory;


} BP_NMAP, *P_BP_NMAP;


// nmap event structures used to communicate events
typedef struct __BP_NMAP_EVENT_DATA {

	// type of event being sent
	BP_NMAP_EVENT_TYPE event_type;

	//  nmap session used
	P_BP_NMAP nmap;

	// return code for the specified event data
	BP_NMAP_EVENT_RETURN_CODE return_code;

	// event status
	BP_NMAP_EVENT_STATUS status;

	// event return data
	void *nmap_event_return_data;


} BP_NMAP_EVENT_DATA, *P_BP_NMAP_EVENT_DATA;


// Create new nmap session.
P_BP_NMAP BP_NmapCreateSession
(
	P_BP_EVENT_MANAGER evm,
	char               *nmap_bin_path,
	char              **nmap_bin_envp,
	BP_BOOL             use_proxychains_ng,
	char                *proxychains_ng_bin_path,
	char                *proxychains_ng_conf_file_path,
	BP_NMAP_SCAN_TYPE   scan_type,
	BP_BOOL             create_nmap_bin_sha1_hashes = BP_TRUE,
	char               *sha1_compare_match_buff = NULL
);

// Destroy nmap session
BP_ERROR_T BP_NmapDestroySession(P_BP_NMAP nmap);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Event Manager Callbacks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// thread entry point for our application handler.  This must be
// added to whichever event handler you want to hook it into at runtime
// in order for it to work. (BP_EventMgrAddProcessor)
BP_EVENT_PROCESSOR_FUNC( BP_NmapThreadCallbackEntryPoint );

// Load/register all event processors within a provided event manager.  This
// essentially primes the event recievers so that events can be dispatched.
BP_ERROR_T BP_NmapRegisterEventProcessorsWithEVM(P_BP_EVENT_MANAGER evm);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Start And Stop Nmap %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to start the nmap service if it's unstarted
BP_ERROR_T BP_NmapStart(P_BP_NMAP nmap);

// attempt to stop the nmap service if it's started
BP_ERROR_T BP_NmapStop(P_BP_NMAP nmap);




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Nmap Event Pickup (from main application loop) %%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This picks up events in the main loop and processes/destroys them.
BP_ERROR_T BP_NmapPickupEventsFromMainLoop(P_BP_NMAP nmap);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// display Nmap information
BP_ERROR_T BP_NmapDisplayInfo(P_BP_NMAP nmap);


#endif /* INCLUDE_NETWORK_NMAP_BP_NMAP_H_ */

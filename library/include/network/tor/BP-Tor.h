/*
 * BP-Tor.h
 *
 *  Created on: Dec 18, 2015
 *      Author: root
 */

#ifndef INCLUDE_NETWORK_TOR_BP_TOR_H_
#define INCLUDE_NETWORK_TOR_BP_TOR_H_

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Tor Thread Locking Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// enter critical section for thread (relies on a managed thread pointer)
#define BP_TOR_THREAD_ENTER_CRITICAL_SECTION\
	{\
		BP_HASH_TABLE_ENTER_CRITICAL_SECTION(managed_thread->system_hreg_ptr);                                            \
	}\

// leave critical section for thread (relies on a managed_thread pointer)
#define BP_TOR_THREAD_LEAVE_CRITICAL_SECTION\
	{\
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(managed_thread->system_hreg_ptr);\
	}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Tor Static Strings %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// When tor is finished bootstrapping, it will output this string.
#define TOR_FINISHED_BOOTSTRAPPING_INDICATOR_0_2_4 "Bootstrapped 100%: Done."

// This is the name of the event processor for handling tor events.
#define TOR_EVENT_PROCESSOR_NAME "GR4_TorEventProcessor"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Tor Sessions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// event types
typedef enum
{
	BP_TOR_EVENT_UNSET,
	BP_TOR_EVENT_START_TOR,
	BP_TOR_EVENT_STOP_TOR,
	BP_TOR_EVENT_RESTART_TOR,
	BP_TOR_EVENT_RECONFIGURE_TOR
} BP_TOR_EVENT_TYPE;

// event return codes
typedef enum
{
	BP_TOR_EVENT_RETURN_FAILURE,
	BP_TOR_EVENT_RETURN_SUCCESS

} BP_TOR_EVENT_RETURN_CODE;

// event status types
typedef enum
{
	BP_TOR_EVENT_STATUS_UNSTARTED,
	BP_TOR_EVENT_STATUS_STARTING_UP,
	BP_TOR_EVENT_STATUS_RUNNING,
	BP_TOR_EVENT_STATUS_STOPPING,
	BP_TOR_EVENT_STATUS_FINISHED
} BP_TOR_EVENT_STATUS;



// This structure holds information relevant to tor on the system.
typedef struct __BP_TOR {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Event Manager This Session Is Hooked Into %%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// event manager
	P_BP_EVENT_MANAGER evm;

	// The uuid of the event used to start this tor session.  Used for main event loop pickups.
	char * start_event_uuid;

	// The uuid of the last stop request.  used for main event loop pickup.
	char * stop_event_uuid;


	// current running status as reported by the event manager
	BP_TOR_EVENT_STATUS run_status;




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% System Registries %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// system hreg for this tor session
	P_BP_HASH_TABLE_REGISTRY system_hreg;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Threads %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Command and control sessions are started within threads, this
	// manager is used to control/dispatch them.
	P_BP_THREAD_MANAGER tor_threads;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Basic Configurations %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// path on the filesystem to find the tor binary
	char *tor_bin_path;

	// sha1 hash for the tor binary
	char *tor_bin_sha1;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Service Run Information %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// start time
	timeval service_start_time;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% torrc %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// --- torrc socks configuration ---------------------

	// socks proxy port to setup
	unsigned short socks_proxy_port;

	// bind address
	char *socks_bind_address;


	// --- torrc hidden service configurations -----------

	// Filesystem path to the directory you want to use to hold the
	// configuration information for your service.
	char         * service_dir;

	// TCP port on which to offer the hidden service.
	unsigned short service_port;

	// Local address to bind tor to.
	char         * bind_local_addr;

	// TCP port on which the hidden service will be translated.
	unsigned short bind_local_port;


	// --- torrc data directory and control port ----------

	// data directory for tor
	char * data_directory;

	// control port for tor
	unsigned short control_port;


	// --- torrc buffer -----------------------------------

	// buffer containing the entire torrc to be written to disk
	char *torrc_buff;

	// size of the buffer to be written
	size_t torrc_size;

	// --- torrc storage options --------------------------

	// path to where you want to store the torrc file (must exist on filesystem)
	char *torrc_parent_dir_path;

	// filename which will be stored in the parent directory
	char *torrc_file_name;

	// full filepath to torrc
	char *torrc_full_file_path;

	// indicator that the torrc was successfully written out to the disk
	BP_BOOL torrc_written_to_disk;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Command / Control Session %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// flag indicating if the command and control session has started ok
	BP_BOOL cc_started_ok;
	BP_BOOL cc_exited_ok;

	// file descriptor set used by the cc session to manipulate the
	P_BP_FDSET tor_app;

	// command and control session for manipulating tor
	P_BP_CC_SESSION cc_session;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Tor Environment %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// envp passed through to all tor
	char ** tor_bin_envp;


} BP_TOR, *P_BP_TOR;


// tor event structures are used to communicate event
typedef struct __BP_TOR_EVENT_DATA {

	// tor event type
	BP_TOR_EVENT_TYPE event_type;

	// tor structure to utilize
	P_BP_TOR tor;

	// return code for the specified event data
	BP_TOR_EVENT_RETURN_CODE return_code;

	// tor event status
	BP_TOR_EVENT_STATUS status;

	// event return data
	void *tor_event_return_data;

} BP_TOR_EVENT_DATA, *P_BP_TOR_EVENT_DATA;




// Create a new tor session.  If create_tor_bin_sha1_hashes is set to BP_TRUE (default)
// we will attempt to read the binary into memory and hash it before running it (for
// application integrity purposes).  If the sha1_compare_match_buff routine is
P_BP_TOR BP_TorCreateSession
(
		P_BP_EVENT_MANAGER evm,
		char   *tor_bin_path,
		char  **tor_bin_envp,
		BP_BOOL create_tor_bin_sha1_hashes = BP_TRUE,
		char *  sha1_compare_match_buff    = NULL
);

// destroy the tor session
BP_ERROR_T BP_TorDestroySession(P_BP_TOR tor);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Configurators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Configure hidden service information for the hidden service.
BP_ERROR_T BP_TorConfigHiddenService
(
	P_BP_TOR       tor,
	char         * service_dir,
	unsigned short service_port,
	char         * bind_local_addr,
	unsigned short bind_local_port
);

// Configure hidden service information for the hidden service.
BP_ERROR_T BP_TorConfigSOCKSProxy
(
	P_BP_TOR       tor,
	char *         socks_bind_address,
	unsigned short socks_proxy_port
);

// Set confiuration control port and data directory
BP_ERROR_T BP_TorConfigControlPortAndDataDirectory
(
	P_BP_TOR       tor,
	char         * data_directory,
	unsigned short control_port
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Generate Torrc %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This attempts to populate the torrc buffer with data set via configurators.
BP_ERROR_T BP_TorGenerateTorrc
(
	P_BP_TOR tor,
	char   * torrc_parent_dir_path,
	char    *torrc_file_name
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Event Manager Callbacks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// thread entry point for our application handler.  This must be
// added to whichever event handler you want to hook it into at runtime
// in order for it to work. (BP_EventMgrAddProcessor)
BP_EVENT_PROCESSOR_FUNC( BP_TorThreadCallbackEntryPoint );

// Load/register all event processors within a provided event manager.  This
// essentially primes the event recievers so that events can be dispatched.
BP_ERROR_T BP_TorRegisterEventProcessorsWithEVM(P_BP_EVENT_MANAGER evm);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Tor Event System Integration %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// This will attempt to create an event in the provided event manager.
BP_ERROR_T BP_TorCreateEvent
(
	P_BP_EVENT_MANAGER evm,
	P_BP_TOR           tor,
	BP_TOR_EVENT_TYPE  event_type,
	void *             event_data,
	size_t             event_data_size
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Start And Stop Tor %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempt to start the tor service if it's unstarted.  Torrc filename and
// parent dir determine where the run-time generated torrc will be stored/overwritten.
//
BP_ERROR_T BP_TorStart
(
		P_BP_TOR  tor
);

// attempt to stop the tor service if it's started
BP_ERROR_T BP_TorStop
(
		P_BP_TOR  tor
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Tor Event Pickup (from main application loop) %%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This picks up events in the main loop and processes/destroys them.
BP_ERROR_T BP_TorPickupEventsFromMainLoop
(
		P_BP_TOR tor
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// display tor information
BP_ERROR_T BP_TorDisplayInfo(P_BP_TOR tor);







#endif /* INCLUDE_NETWORK_TOR_BP_TOR_H_ */

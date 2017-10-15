/*
 * BP-Tor.cc
 *
 *  Created on: Dec 28, 2015
 *      Author: root
 */


#include "../../../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create and Destroy Tor Sessions %%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Create a new tor session
P_BP_TOR BP_TorCreateSession
(
		P_BP_EVENT_MANAGER evm,
		char   *tor_bin_path,
		char  **tor_bin_envp,
		BP_BOOL create_tor_bin_sha1_hashes,
		char *  sha1_compare_match_buff
)
{

	// ensure we have an event manager
	if(!evm)
		return NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Path/Env Checks %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure we have a bin path to tor
	if(!tor_bin_path)
		return NULL;
	if(!BP_StringIsReasonableForUnixEXT4Filename(tor_bin_path, bpstrlen(tor_bin_path), BP_TRUE))
		return NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Allocate/Fill Structures %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new system hreg to track allocations
	P_BP_HASH_TABLE_REGISTRY system_hreg = BP_CreateHashTableRegistry();
	if(!system_hreg)
		return NULL;

	// allocate new tor structure
	P_BP_TOR tor = (P_BP_TOR) ht_calloc(system_hreg, sizeof(BP_TOR), 1);
	if(!tor)
	{

		// destroy the hash table registry and exit
		BP_DestroyHashTableRegistry(system_hreg);
		return NULL;

	}

	// set system hreg
	tor->system_hreg = system_hreg;

	// link in the event manager
	tor->evm = evm;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run SHA1 Check If Desired %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if app dev wants hashes, generate them here
	if(create_tor_bin_sha1_hashes)
	{

		// declare ret buffer and zero it
		char sha1_ret_buffer[41];
		BP_ZERO_STACK(sha1_ret_buffer);

		// attempt to generate
		if
		(
			!BP_FileSHA1Contents
			(
				tor_bin_path,
				(char *) &sha1_ret_buffer
			)
		)
		{

			// destroy the hash table registry and exit
			BP_DestroyHashTableRegistry(system_hreg);
			return NULL;

		}

		// display the binary path
		printf("\n [+] TOR bin hash: %s", sha1_ret_buffer);

		// attempt to compare matched buffers
		if(sha1_compare_match_buff)
		{

			printf("\n [+] TOR integrity check hash: %s", sha1_compare_match_buff);

			// attempt to compare hashes to ensure things are as expected
			if(memcmp(sha1_compare_match_buff, (char *) &sha1_ret_buffer, 40) != 0)
			{

				// display that the failure information
				printf("\n [!!] TOR integrity check failed.");

				// destroy the hash table registry and exit
				BP_DestroyHashTableRegistry(system_hreg);
				return NULL;

			}

		}

		// attempt to set sha1
		tor->tor_bin_sha1 = (char *) ht_strdup(tor->system_hreg, (char *) &sha1_ret_buffer);

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Other Binary Parameters %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set tor bin path
	tor->tor_bin_path = ht_strdup(tor->system_hreg, tor_bin_path);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Envp %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// count the passed in envp
	size_t envp_n = BP_CountNullTerminatedPtrArray(tor_bin_envp);

	// if we have parameters, go ahead and store them
	if(envp_n)
	{

		// marker to ensure that all envp strings are reasonable/printable.
		BP_BOOL envp_verification_ok = BP_TRUE;

		// iterate through strings and duplicate them into the
		size_t envp_iter = 0;
		for(; tor_bin_envp[envp_iter]; envp_iter++)
		{

			// ensure envp string is reasonable/printable
			if(!BP_StringIsReasonablePrintableString(tor_bin_envp[envp_iter], bpstrlen(tor_bin_envp[envp_iter]), BP_FALSE, BP_FALSE))
			{

				// set bad envp marker and exit loop
				envp_verification_ok = BP_FALSE;
				break;

			}

		}

		// go ahead and set envp after verificiation passes
		if(envp_verification_ok)
		{

			// allocate space to hold string pointers using system hreg (easy cleanup)
			tor->tor_bin_envp = (char **) ht_calloc
			(
				tor->system_hreg,
				(envp_n+1) * sizeof(char *),
				1
			);

			// iterate through strings and duplicate them into position now that verification
			// and allocation has completed.
			for(envp_iter = 0; tor_bin_envp[envp_iter]; envp_iter++)
			{

				// duplicate the envp position
				tor->tor_bin_envp[envp_iter] = (char *) ht_strdup(tor->system_hreg, tor_bin_envp[envp_iter]);

			}

		}

	}

	// return the new tor structure
	return tor;


}


// destroy the tor session
BP_ERROR_T BP_TorDestroySession(P_BP_TOR tor)
{

	// check if we have a tor session
	if(!tor)
		return ERR_FAILURE;

	// destroy the system hreg (frees all chunks)
	return BP_DestroyHashTableRegistry(tor->system_hreg);

}



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
)
{

	// Run basic null checks
	if(!tor)
		return ERR_FAILURE;
	if(!service_dir)
		return ERR_FAILURE;
	if(!bind_local_addr)
		return ERR_FAILURE;
	if(!bind_local_port)
		return ERR_FAILURE;

	// attempt to check composition of, and stat, the service directory
	if(!BP_StringIsReasonableForUnixEXT4Directory(service_dir, bpstrlen(service_dir), BP_TRUE))
		return ERR_FAILURE;

	// ensure the bind addr string is an ip version 4 addr
	if(!BP_StringIsIPv4Address(bind_local_addr))
		return ERR_FAILURE;


	// set service directory
	if(tor->service_dir)
		ht_free(tor->system_hreg, tor->service_dir);
	tor->service_dir = ht_strdup(tor->system_hreg, service_dir);

	// set the service port
	tor->service_port = service_port;

	// destroy local addr if it's already set
	if(tor->bind_local_addr)
		ht_free(tor->system_hreg, tor->bind_local_addr);

	// set the local address
	tor->bind_local_addr = ht_strdup(tor->system_hreg, bind_local_addr);

	// set the local port
	tor->bind_local_port = bind_local_port;


	// return indicating success
	return ERR_SUCCESS;

}

// Configure hidden service information for the hidden service.
BP_ERROR_T BP_TorConfigSOCKSProxy
(
	P_BP_TOR       tor,
	char *         socks_bind_address,
	unsigned short socks_proxy_port
)
{

	// ensure we have a tor structure and a bind port
	if(!tor)
		return ERR_FAILURE;
	if(!socks_proxy_port)
		return ERR_FAILURE;

	// ensure the string is an ipv4 address
	if(!BP_StringIsIPv4Address(socks_bind_address))
		return ERR_FAILURE;

	// set the bind address
	if(tor->socks_bind_address)
		ht_free(tor->system_hreg, tor->socks_bind_address);
	tor->socks_bind_address = (char *) ht_strdup(tor->system_hreg, socks_bind_address);

	// set the proxy port
	tor->socks_proxy_port = socks_proxy_port;

	// return indicating success after port is set
	return ERR_SUCCESS;

}

// Set confiuration control port and data directory
BP_ERROR_T BP_TorConfigControlPortAndDataDirectory
(
		P_BP_TOR       tor,
		char         * data_directory,
		unsigned short control_port
)
{

	// ensure we have a tor session
	if(!tor)
		return ERR_FAILURE;

	// ensure the directory exists and is reasonable
	if(!BP_StringIsReasonableForUnixEXT4Directory(data_directory, bpstrlen(data_directory), BP_TRUE))
		return ERR_FAILURE;
	if(!control_port)
		return ERR_FAILURE;

	// set the data directory
	tor->data_directory = ht_strdup(tor->system_hreg, data_directory);

	// set the control port
	tor->control_port = control_port;


	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Generate Torrc %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This attempts to populate the torrc buffer with data set via configurators.
BP_ERROR_T BP_TorGenerateTorrc
(
		P_BP_TOR tor,
		char *torrc_parent_dir_path,
		char    *torrc_file_name
)
{


	// ensure we have a tor session
	if(!tor)
		return ERR_FAILURE;

	// ensure the parent dir path is set
	if(!torrc_parent_dir_path)
		return ERR_FAILURE;

	// ensure string is reasonable/printable
	if(!BP_StringIsReasonablePrintableString(torrc_parent_dir_path, bpstrlen(torrc_parent_dir_path), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// ensure string is reasonable for a ext4
	if(!BP_StringIsReasonableForUnixEXT4Filename(torrc_file_name, bpstrlen(torrc_file_name), BP_FALSE))
	{
		printf("\n Error, torrc file name is bad: %s", torrc_file_name);
		return ERR_FAILURE;
	}

	// ensure string is reasonable (runs stat)
	if(!BP_StringIsReasonableForUnixEXT4Directory(torrc_parent_dir_path, bpstrlen(torrc_parent_dir_path), BP_TRUE))
	{
		printf("\n Error, parent directory path is bad?: %s", torrc_parent_dir_path);
		return ERR_FAILURE;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Path and Filename %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set torrc parent directory path (after stat check)
	tor->torrc_parent_dir_path = (char *) ht_strdup(tor->system_hreg, torrc_parent_dir_path);

	// set filename
	tor->torrc_file_name = (char *) ht_strdup(tor->system_hreg, torrc_file_name);

	// join name and path to create full-path
	char *tor_full_file_path = BP_JoinStrings(tor->torrc_parent_dir_path, tor->torrc_file_name);

	// duplicate full filepath in system hreg
	tor->torrc_full_file_path = (char *) ht_strdup(tor->system_hreg, tor_full_file_path);

	// destroy the joined string
	bpfree(tor_full_file_path);





	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Socks Proxy Configuration %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create linkl for flattening buffers into one output buffer
	P_BP_LINKL tq = BP_CreateLinkL(".", BP_TRUE);

	// add proxy line if necessary
	if(tor->socks_proxy_port)
	{

		// --- add socks port line -----------

		// create/zero out buffer to hold proxy port information
		char proxy_port_line[128];
		BP_ZERO_STACK(proxy_port_line);

		// set the port
		sprintf((char *) &proxy_port_line, "SocksPort %u\n", tor->socks_proxy_port);

		// add proxy port line to our buffer
		tq_strdup(tq, (char *) &proxy_port_line, BPLN);

		// --- add socks bind address line -----------

		// create/zero out buffer to hold proxy port information
		char socks_bind_addr_line[128];
		BP_ZERO_STACK(socks_bind_addr_line);

		// set the port
		sprintf((char *) &socks_bind_addr_line, "SocksBindAddress %s\n", tor->socks_bind_address);

		// add proxy port line to our buffer
		tq_strdup(tq, (char *) &socks_bind_addr_line, BPLN);

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Service Information %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



	if(tor->service_dir)
	{

		size_t service_dir_len = bpstrlen(tor->service_dir);

		// --- add hidden service dir line -----------

		// create/zero out buffer to hold proxy port information
		char hidden_service_dir_line[128+service_dir_len];
		BP_ZERO_STACK(hidden_service_dir_line);

		// set the service directory
		sprintf((char *) &hidden_service_dir_line, "HiddenServiceDir %s\n", tor->service_dir);

		// add service directory line
		tq_strdup(tq, (char *) &hidden_service_dir_line, BPLN);


		// --- add hidden service port line -----------

		// create/zero out buffer to hold proxy port information
		char hidden_service_port_line[512];
		BP_ZERO_STACK(hidden_service_port_line);

		// set the service directory
		sprintf((char *) &hidden_service_port_line, "HiddenServicePort %u %s:%u\n", tor->service_port, tor->bind_local_addr, tor->bind_local_port);

		// add service directory line
		tq_strdup(tq, (char *) &hidden_service_port_line, BPLN);

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Data Directory and Control Port %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(tor->data_directory)
	{
		size_t data_dir_len = bpstrlen(tor->data_directory);

		// --- add hidden service data dir line -----------

		// create/zero out buffer to hold data dir information
		char hidden_service_data_dir_line[128+data_dir_len];
		BP_ZERO_STACK(hidden_service_data_dir_line);

		// set the service directory
		sprintf((char *) &hidden_service_data_dir_line, "DataDirectory %s\n", tor->data_directory);

		// add service directory line
		tq_strdup(tq, (char *) &hidden_service_data_dir_line, BPLN);


		// --- add hidden service port line -----------

		// create/zero out buffer to hold proxy port information
		char hidden_service_control_port_line[512];
		BP_ZERO_STACK(hidden_service_control_port_line);

		// set the service directory
		sprintf((char *) &hidden_service_control_port_line, "ControlPort %u\n", tor->control_port);

		// add service directory line
		tq_strdup(tq, (char *) &hidden_service_control_port_line, BPLN);

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Flatten Elements to Create File %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to flatten string elements
	BP_LinkLFlattenJoinStringElements(tq);


	// ensure the buffer is flattened
	if(tq->flat_buff)
	{


		// ensure the torrc is reasonable/printable at this point.
		if(!BP_StringIsReasonablePrintableString((char *) tq->flat_buff, bpstrlen((char *) tq->flat_buff)-1, BP_TRUE, BP_TRUE))
		{
			printf("\n Failed to Generate Torrc: %s", tq->flat_buff);
			BP_DestroyLinkL(tq, BP_TRUE);
			return ERR_FAILURE;
		}

		// allocate space for tor buffer
		tor->torrc_buff = (char *) ht_calloc(tor->system_hreg, tq->flat_buff_n, 1);

		// copy buffer
		memcpy(tor->torrc_buff, tq->flat_buff, tq->flat_buff_n);

		// set the torrc size from the flat buffer calculation
		tor->torrc_size = bpstrlen((char *) tq->flat_buff)-1;

	}

	// destroy the linkl and free all associated memory
	BP_DestroyLinkL(tq, BP_TRUE);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Write File To Disk %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to unlink the file before writing
	unlink(tor->torrc_full_file_path);

	// attempt to write contents to disk
	if(!BP_FilePutContents(tor->torrc_full_file_path, tor->torrc_buff, tor->torrc_size, BP_FALSE))
	{

		// set write indicator to false (overwriting any previous change)
		tor->torrc_written_to_disk = BP_FALSE;
		return ERR_FAILURE;

	}

	// set write indicator to true if torrc was written to disk ok
	tor->torrc_written_to_disk = BP_TRUE;

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Event Manager Callbacks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// thread entry point for our application handler.  This must be
// added to whichever event handler you want to hook it into at runtime
// in order for it to work. (BP_EventMgrAddProcessor)
BP_EVENT_PROCESSOR_FUNC( BP_TorThreadCallbackEntryPoint )
{

	// processor entry macro
	BP_EVENT_PROCESSOR_ENTRY;

	// set tor event data
	P_BP_TOR_EVENT_DATA tor_event_data = (P_BP_TOR_EVENT_DATA) event->event_data;
	if(!tor_event_data)
	{
		BP_EVENT_PROCESSOR_EXIT;
	}

	// printf("\n HERE HERE THIS IS THE EVENT DATA POINTER: %p (event) - %p (tor event)", event, tor_event_data);

	// set tor handle
	P_BP_TOR tor = tor_event_data->tor;
	if(!tor)
	{

		// ensure we have tor
		printf("\n [!!] Critical: tor session has not been passed in.");
		BP_EVENT_PROCESSOR_EXIT;

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Process Start Event %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// process start event if set
	if(tor_event_data->event_type == BP_TOR_EVENT_START_TOR)
	{

		// set the tor running status to starting up
		tor_event_data->status = BP_TOR_EVENT_STATUS_STARTING_UP;

		// return code for the specified event data
		BP_TOR_EVENT_RETURN_CODE return_code;

		// tor event status
		BP_TOR_EVENT_STATUS status;



		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Set Exec Parameters and Execute Binary %%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// printf("\n Tor file path?: %s - service_dir: %s", tor->torrc_full_file_path, tor->service_dir);

		// set argv for application.
		char *tor_params[] =
		{
				tor->tor_bin_path,
				"-f",
				tor->torrc_full_file_path,
				NULL
		};

		// exec the file nvp)
		tor->tor_app = BP_FExec
		(
				tor->tor_bin_path,
				(char **) &tor_params,
				tor->tor_bin_envp,
				BP_TRUE
		);


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Create Basic CC Session To Start TOR Service %%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// set cc session
		P_BP_CC_SESSION tor_cc_session = BP_CCNewSession
		(
				tor->tor_app,
			   "command_control_interface_ut",
			   BP_CC_SHELL_LINUX_EXECVE
		);

		// display reads and writes (values can be changed inbetween
		// command runs to disable output for certain commands, or
		// enable output for others)
		tor_cc_session->display_reads_from_app_stdout = BP_TRUE;
		tor_cc_session->display_reads_from_app_stderr = BP_TRUE;
		tor_cc_session->display_writes_to_app_stdin   = BP_FALSE;


		// Set stdout polling timeout (values are additive and can be reset
		// between dispatched commands to change the select() responsiveness
		// of the app being controlled.)
		tor_cc_session->select_child_stdout_timeout_sec  = 30;
		tor_cc_session->select_child_stdout_timeout_usec = 500;

		// declare return value
		BP_ERROR_T ret_val = ERR_FAILURE;



		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% 2) Add Auth Try/Expect Logic %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


		// Add some arbitrary commands to the command orchard.
		BP_CCAddCommand(tor_cc_session, "start_and_monitor_tor");

		// set the expected shell prompt (it should be a $ at the end of the line.  It's just
		// coincidence that the end of line match char is $ as well)
		char *tor_error_str = "Bootstrapped 100%: Done.";

		// Add some try/expect branches to some commands
		ret_val = BP_CCAddTryExpectToCommand
		(
			tor_cc_session,
			"start_and_monitor_tor",
			NULL,
			0,
			tor_error_str,
			bpstrlen(tor_error_str),
			BP_CC_MATCH_BUFF_POLICY_MEM_SEARCH
		);

		// attempt to detect password field
		if(!BP_CCDispatcher(tor_cc_session, "start_and_monitor_tor", NULL))
		{

			printf("\n [!!] Command-and-Control Session Dispatcher failed for start_and_monitor_tor.");

			// call reset command to reset terminal (terminal gets messed up somehow)
			exit(0);

		}


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Display CC Information After %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// set service start time now that it's officially started
		gettimeofday(&tor->service_start_time, NULL);

		// dbg: attempt to visualize sessions
		// BP_CCVisualizeSession(tor_cc_session, BP_TRUE, BP_TRUE);

		// tor display message
		printf("\n [+] TOR started and dispatcher has completed.  We are bootstrapped at 100%%");

		// set the tor running status to starting up
		tor_event_data->status = BP_TOR_EVENT_STATUS_RUNNING;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Process Stop Event %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// process stop event if set
	if(tor_event_data->event_type == BP_TOR_EVENT_STOP_TOR)
	{

		// set event status
		tor_event_data->status = BP_TOR_EVENT_STATUS_STOPPING;


	    // check if the app is still running
	    BP_BOOL still_running = BP_TRUE;

	    do
	    {

	    	// check if application is still running
	    	still_running = BP_FExecApplicationStillRunning(tor->tor_app) ? BP_TRUE : BP_FALSE;

	    	// if the application is running, attempt to kill it and sleep one sec
	    	if(still_running)
	    	{
	    		BP_FExecKillApplication(tor->tor_app);
	    		sleep(1);
	    	}

	    } while(still_running);

	    // set event status
	    tor_event_data->status = BP_TOR_EVENT_STATUS_FINISHED;

	    printf("\n [+] Tor process has stopped completely.");

	}



	// processor entry macro
	BP_EVENT_PROCESSOR_EXIT;

}


// Load/register all event processors within a provided event manager.  This
// essentially primes the event recievers so that events can be dispatched.
BP_ERROR_T BP_TorRegisterEventProcessorsWithEVM
(
	P_BP_EVENT_MANAGER evm
)
{

	// ensure we have an event manager
	if(!evm)
		return ERR_FAILURE;

	//attempt to lookup the tor processor
	P_BP_EVENT_PROCESSOR tor_processor = BP_EventMgrLookupProcessorByName(evm, TOR_EVENT_PROCESSOR_NAME);
	if(!tor_processor)
	{

		// attempt to add the processor if it doesn't exist
		BP_EventMgrAddProcessor
		(
				evm,
				TOR_EVENT_PROCESSOR_NAME,
				"Tor event processor.",
				BP_EVENT_PROCESSOR_COMPUTE_TARGET_TYPE_CPU_THREAD,
				BP_TorThreadCallbackEntryPoint
		);


	}
	// attempt to lookup again after addition attempt
	tor_processor = BP_EventMgrLookupProcessorByName(evm, TOR_EVENT_PROCESSOR_NAME);
	if(!tor_processor)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Start And Stop Tor %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to start the tor service if it's unstarted
BP_ERROR_T BP_TorStart
(
		P_BP_TOR           tor
)
{

	// ensure
	if(!tor)
		return ERR_FAILURE;
	if(!tor->evm)
		return ERR_FAILURE;
	if(!tor->torrc_buff)
		return ERR_FAILURE;
	if(!tor->torrc_size)
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(tor->torrc_buff, tor->torrc_size, BP_TRUE, BP_FALSE))
		return ERR_FAILURE;


	//attempt to lookup the tor processor
	P_BP_EVENT_PROCESSOR tor_processor = BP_EventMgrLookupProcessorByName(tor->evm, TOR_EVENT_PROCESSOR_NAME);

	// display processor lookup
	printf("\n [+] tor_processor lookup %p", tor_processor);

	// set the run status as starting up
	tor->run_status = BP_TOR_EVENT_STATUS_STARTING_UP;

	// if we don't have an event manager entry, add it now
	if(!tor_processor)
		return ERR_FAILURE;


	// create new tor event
	P_BP_TOR_EVENT_DATA tor_event = (P_BP_TOR_EVENT_DATA) ht_calloc(tor->system_hreg, sizeof(BP_TOR_EVENT_DATA), 1);
	if(!tor_event)
	{

		// return indicating failure if we can't alloc
		return ERR_FAILURE;

	}

	// set event as a start event
	tor_event->event_type = BP_TOR_EVENT_START_TOR;

	// set tor reference
	tor_event->tor = tor;

	// create temporary uuid for the event
	char *tmp_uuid = BP_GenRandomUUID();

	// add start tor
	BP_ERROR_T event_create_ret = BP_EventMgrCreateEvent
	(
			tor->evm,
			(P_BP_EVENT_PROCESSOR) tor_processor,
			(P_BP_EVENT_PROCESSOR) NULL,
			BP_EVENT_MESSAGES_IN,
			BP_EVENT_TYPE_MARKER_SYSTEM_CORE_EVENT,
			/*"GR4_Start_Tor_API_Server"*/	tmp_uuid,
			"Starting Tor Process",
			(void *) tor_event,
			sizeof(tor_event),
			BP_EVENT_PRIORITY_HIGH
	);


	// dispatch event check
	if(!event_create_ret)
	{
		printf("\n [!!] Tor start event dispatch failed.");
	}

	printf("\n [+] Tor startup dispatch succeeded!");

	// attempt to duplicate event uuid
	tor->start_event_uuid = ht_strdup(tor->system_hreg, (char *) tmp_uuid);

	// destroy temporary uuid before returning
	bpfree(tmp_uuid);
	tmp_uuid = NULL;

	// return the creation return value
	return event_create_ret;

}

// attempt to stop the tor service if it's started
BP_ERROR_T BP_TorStop(P_BP_TOR tor)
{

	// run basic null checks
	if(!tor)
		return ERR_FAILURE;
	if(!tor->evm)
		return ERR_FAILURE;
	if(!tor->tor_app)
		return ERR_FAILURE;


	// return indicating failure if the process is not running
	if(tor->run_status != BP_TOR_EVENT_STATUS_RUNNING)
		return ERR_FAILURE;

	// set run status as stopping
	tor->run_status = BP_TOR_EVENT_STATUS_STOPPING;

	//attempt to lookup the tor processor
	P_BP_EVENT_PROCESSOR tor_processor = BP_EventMgrLookupProcessorByName(tor->evm, TOR_EVENT_PROCESSOR_NAME);

	// display processor lookup
	printf("\n [+] tor_processor lookup %p", tor_processor);

	// if we don't have an event manager entry, add it now
	if(!tor_processor)
		return ERR_FAILURE;


	// create new tor event
	P_BP_TOR_EVENT_DATA tor_event = (P_BP_TOR_EVENT_DATA) ht_calloc(tor->system_hreg, sizeof(BP_TOR_EVENT_DATA), 1);
	if(!tor_event)
	{

		// return indicating failure if we can't alloc
		return ERR_FAILURE;

	}

	// set event as a stop event
	tor_event->event_type = BP_TOR_EVENT_STOP_TOR;

	// set tor reference
	tor_event->tor = tor;

	// create temporary uuid for the event
	char *tmp_uuid = BP_GenRandomUUID();

	// add start tor
	BP_ERROR_T event_create_ret = BP_EventMgrCreateEvent
	(
			tor->evm,
			(P_BP_EVENT_PROCESSOR) tor_processor,
			(P_BP_EVENT_PROCESSOR) NULL,
			BP_EVENT_MESSAGES_IN,
			BP_EVENT_TYPE_MARKER_SYSTEM_CORE_EVENT,
			/*"GR4_Start_Tor_API_Server"*/	tmp_uuid,
			"Stopping Tor Process",
			(void *) tor_event,
			sizeof(tor_event),
			BP_EVENT_PRIORITY_HIGH
	);

	// dispatch event check
	if(!event_create_ret)
	{
		printf("\n [!!] Tor stop event dispatch failed.");
	}

	printf("\n [+] Tor stop dispatch succeeded!");

	// attempt to duplicate event uuid
	tor->stop_event_uuid = ht_strdup(tor->system_hreg, (char *) tmp_uuid);

	// destroy temporary uuid before returning
	bpfree(tmp_uuid);
	tmp_uuid = NULL;

	// return the creation return value
	return event_create_ret;


}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Tor Event Pickup (from main application loop) %%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This picks up events in the main loop and processes/destroys them.
BP_ERROR_T BP_TorPickupEventsFromMainLoop(P_BP_TOR tor)
{

	// basic null checks
	if(!tor)
		return ERR_FAILURE;
	if(!tor->evm)
		return ERR_FAILURE;

	// ensure we have an event registry in the manager
	if(!tor->evm->events_polled_for_main_loop)
	{
		printf("\n [!!] Tor has no events to process here.");
		return ERR_FAILURE;
	}

	printf("\n [+] Attempting to pickup tor events from main loop!!");


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Process Start Event %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we have a start event uuid, go ahead and attempt an event lookup
	if(tor->start_event_uuid)
	{

		// attempt to pickup the uuid
		P_BP_EVENT start_event = BP_EventLoopPickupEventFromPollRegistryByName
		(
			tor->evm,
			tor->start_event_uuid
		);


		// process start event
		if(start_event)
		{

			// set start event data
			P_BP_TOR_EVENT_DATA start_event_data = (P_BP_TOR_EVENT_DATA) start_event->event_data;

			// if the event reports that we're running, mark it so
			if(start_event_data->status	== BP_TOR_EVENT_STATUS_RUNNING)
			{

				// set tor status to running
				tor->run_status = BP_TOR_EVENT_STATUS_RUNNING;

				// destroy start uuid
				ht_free(tor->system_hreg, tor->start_event_uuid);
				tor->start_event_uuid = NULL;

			}

		}

	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Process Stop Event %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we have a stop event uuid, go ahead and attempt an event lookup
	if(tor->stop_event_uuid)
	{

		// attempt to pickup the uuid
		P_BP_EVENT stop_event = BP_EventLoopPickupEventFromPollRegistryByName
		(
			tor->evm,
			tor->stop_event_uuid
		);


		// process stop event
		if(stop_event)
		{

			// set stop event data
			P_BP_TOR_EVENT_DATA stop_event_data = (P_BP_TOR_EVENT_DATA) stop_event->event_data;

			// if the event reports that we're running, mark it so
			if(stop_event_data->status	== BP_TOR_EVENT_STATUS_FINISHED)
			{

				// set tor status to finished
				tor->run_status = BP_TOR_EVENT_STATUS_FINISHED;

				// destroy stop uuid
				ht_free(tor->system_hreg, tor->stop_event_uuid);
				tor->stop_event_uuid = NULL;

			}

		}

	}


	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// display tor information
BP_ERROR_T BP_TorDisplayInfo(P_BP_TOR tor)
{

	// ensure we have a session
	if(!tor)
		return ERR_FAILURE;


	printf("\n [+] TOR Session     %p", tor);
	printf("\n\t system_hreg:      %p", tor->system_hreg);
	printf("\n\t tor_bin_path:     %s", tor->tor_bin_path);
	printf("\n\t tor_bin_sha1:     %s", tor->tor_bin_sha1);

	printf("\n\t socks_proxy_port: %u", tor->socks_proxy_port);
	printf("\n\t service_dir:      %s", tor->service_dir);
	printf("\n\t service_port:     %u", tor->service_port);
	printf("\n\t bind_local_addr:  %s", tor->bind_local_addr);
	printf("\n\t bind_local_port:  %u", tor->bind_local_port);
	printf("\n\t tor_bin_envp:     %p (shown below if non-null value)", tor->tor_bin_envp);

	// display envp
	if(tor->tor_bin_envp)
	{
		BP_DisplayStringArray
		(
				"\t envp[n]-> ",
				1,
				2,
				tor->tor_bin_envp
		);
	}

	printf("\n\t torrc_size:       %u", tor->torrc_size);
	printf("\n\t torrc_buff:       \n%s", tor->torrc_buff);

	// return indicating success
	return ERR_SUCCESS;

}



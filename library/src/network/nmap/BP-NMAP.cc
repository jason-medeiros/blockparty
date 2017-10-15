/*
 * BP-NMAP.cc
 *
 *  Created on: Jul 19, 2016
 *      Author: root
 */


#include "../../../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create and Destroy NMAP Sessions %%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


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
		BP_BOOL             create_nmap_bin_sha1_hashes,
		char               *sha1_compare_match_buff
)
{

	// ensure we have an event manager
	if(!evm)
		return NULL;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Path/Env Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure the file parameter is set
	if(!nmap_bin_path)
		return NULL;

	// ensure the file is reasonable and exists at the current position
	if(!BP_StringIsReasonableForUnixEXT4Filename(nmap_bin_path, bpstrlen(nmap_bin_path), BP_TRUE))
		return NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Allocate/Fill Structures %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new hreg to track allocs
	P_BP_HASH_TABLE_REGISTRY system_hreg = BP_CreateHashTableRegistry();
	if(!system_hreg)
		return NULL;

	// allocate new nmap session structure
	P_BP_NMAP nmap = (P_BP_NMAP) ht_calloc(system_hreg, sizeof(BP_NMAP), 1);
	if(!nmap)
	{

		// destroy the hash table registry and exit
		BP_DestroyHashTableRegistry(system_hreg);
		return NULL;

	}

	// self reference the system hreg
	nmap->system_hreg = system_hreg;

	// set the event manager as the one passed in
	nmap->evm = evm;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run SHA1 check if specified %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(create_nmap_bin_sha1_hashes)
	{

		// declare and zero ret buffer on stack
		char sha1_ret_buffer[41];

		// sero out the memory
		BP_ZERO_STACK(sha1_ret_buffer);

		// attempt to create sha1 of nmap bin
		if(!BP_FileSHA1Contents(nmap_bin_path, (char *) &sha1_ret_buffer))
		{
			BP_DestroyHashTableRegistry( nmap->system_hreg);
			return NULL;
		}

		// display the binary path
		printf("\n [+] TOR bin hash: %s", sha1_ret_buffer);

		// attempt to compare matched buffers
		if(sha1_compare_match_buff)
		{

			printf("\n [+] NMAP integrity check hash: %s", sha1_compare_match_buff);

			// attempt to compare hashes to ensure things are as expected
			if(memcmp(sha1_compare_match_buff, (char *) &sha1_ret_buffer, 40) != 0)
			{

				// display that the failure information
				printf("\n [!!] NMAP integrity check failed.");

				// destroy the hash table registry and exit
				BP_DestroyHashTableRegistry(system_hreg);
				return NULL;

			}

		}

		// attempt to set sha1
		nmap->nmap_bin_sha1 = (char *) ht_strdup(nmap->system_hreg, (char *) &sha1_ret_buffer);


	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Other Bin Related Parameters %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the bin path directly
	nmap->nmap_bin_path = ht_strdup(nmap->system_hreg, nmap_bin_path);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Envp %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// count elements in envp
	size_t envp_n = BP_CountNullTerminatedPtrArray(nmap_bin_envp);

	if(envp_n)
	{
		BP_BOOL envp_verification_ok = BP_TRUE;


		// iterate through strings and duplicate them into the
		size_t envp_iter = 0;
		for(; nmap_bin_envp[envp_iter]; envp_iter++)
		{

			// ensure envp string is reasonable/printable
			if(!BP_StringIsReasonablePrintableString(nmap_bin_envp[envp_iter], bpstrlen(nmap_bin_envp[envp_iter]), BP_FALSE, BP_FALSE))
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
			nmap->nmap_bin_envp = (char **) ht_calloc
			(
				nmap->system_hreg,
				(envp_n+1) * sizeof(char *),
				1
			);

			// iterate through strings and duplicate them into position now that verification
			// and allocation has completed.
			for(envp_iter = 0; nmap_bin_envp[envp_iter]; envp_iter++)
			{

				// duplicate the envp position
				nmap->nmap_bin_envp[envp_iter] = (char *) ht_strdup(nmap->system_hreg, nmap_bin_envp[envp_iter]);

			}

		}

	}

	// return the session
	return nmap;

}


// destroy the nmap session
BP_ERROR_T BP_NmapDestroySession(P_BP_NMAP nmap)
{

	// ensure we have a session
	if(!nmap)
		return ERR_FAILURE;

	// destroy hash table registry
	return BP_DestroyHashTableRegistry(nmap->system_hreg);

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Event Manager Callbacks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// thread entry point for our application handler.  This must be
// added to whichever event handler you want to hook it into at runtime
// in order for it to work. (BP_EventMgrAddProcessor)
BP_EVENT_PROCESSOR_FUNC( BP_NmapThreadCallbackEntryPoint )
{

	// processor entry macro
	BP_EVENT_PROCESSOR_ENTRY;

	// set nmap event data
	P_BP_NMAP_EVENT_DATA nmap_event_data = (P_BP_NMAP_EVENT_DATA) event->event_data;
	if(!nmap_event_data)
	{
		BP_EVENT_PROCESSOR_EXIT;
	}

	// printf("\n HERE HERE THIS IS THE EVENT DATA POINTER: %p (event) - %p (tor event)", event, tor_event_data);

	// set nmap handle
	P_BP_NMAP nmap = nmap_event_data->nmap;
	if(!nmap)
	{

		// ensure we have nmap
		printf("\n [!!] Critical: nmap session has not been passed into the nmap event processor function.");
		BP_EVENT_PROCESSOR_EXIT;

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Process Start Event %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// process start event if set
	if(nmap_event_data->event_type == BP_NMAP_EVENT_START_NMAP)
	{

		// set the tor running status to starting up
		nmap_event_data->status = BP_NMAP_EVENT_STATUS_STARTING_UP;

		// return code for the specified event data
		BP_NMAP_EVENT_RETURN_CODE return_code;

		// tor event status
		BP_NMAP_EVENT_STATUS status;



		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Set Exec Parameters and Execute Binary %%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// set nmap binary path
		printf("\n NMAP bin path", nmap->nmap_bin_path);

		// set argv for application.
		char *nmap_params[] =
		{
				nmap->nmap_bin_path,
				"-sS",
				"localhost",
				NULL
		};

		// exec the file nvp)
		nmap->nmap_app = BP_FExec
		(
				nmap->nmap_bin_path,
				(char **) &nmap_params,
				nmap->nmap_bin_envp,
				BP_TRUE
		);


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Create Basic CC Session To Start TOR Service %%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// set cc session
		P_BP_CC_SESSION nmap_cc_session = BP_CCNewSession
		(
				nmap->nmap_app,
			   "nmap_cc_session",
			   BP_CC_SHELL_LINUX_EXECVE
		);

		// display reads and writes (values can be changed inbetween
		// command runs to disable output for certain commands, or
		// enable output for others)
		nmap_cc_session->display_reads_from_app_stdout = BP_TRUE;
		nmap_cc_session->display_reads_from_app_stderr = BP_TRUE;
		nmap_cc_session->display_writes_to_app_stdin   = BP_FALSE;


		// Set stdout polling timeout (values are additive and can be reset
		// between dispatched commands to change the select() responsiveness
		// of the app being controlled.)
		nmap_cc_session->select_child_stdout_timeout_sec  = 30;
		nmap_cc_session->select_child_stdout_timeout_usec = 500;

		// declare return value
		BP_ERROR_T ret_val = ERR_FAILURE;

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% 2) Add Auth Try/Expect Logic %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// set the cc command name here
		char *cc_command_name = "start_and_monitor_nmap";

		// Add some arbitrary commands to the command orchard.
		BP_CCAddCommand(nmap_cc_session, cc_command_name);

		// Add some try/expect branches to some commands
		ret_val = BP_CCAddTryExpectToCommand
		(
				nmap_cc_session,
				"start_and_monitor_nmap",
				NULL,
				0,
				"Nmap done",
				bpstrlen("Nmap done"),
				BP_CC_MATCH_BUFF_POLICY_MEM_SEARCH
		);

		// start dispatcher
		if(!BP_CCDispatcher(nmap_cc_session, "start_and_monitor_nmap", NULL))
		{
				printf("\n [!!] Dispatcher failed for start_and_monitor_nmap.");
				exit(0);
		}


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Display CC Information After %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// set service start time now that it's officially started
		gettimeofday(&nmap->service_start_time, NULL);

		// dbg: attempt to visualize sessions
		BP_CCVisualizeSession(nmap_cc_session, BP_TRUE, BP_TRUE);

		// display completion message
		printf("\n [+] NMAP dispatcher has completed.");

		// set indicator to running now that the process is up
		nmap_event_data->status = BP_NMAP_EVENT_STATUS_RUNNING;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Process Stop Event %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// process stop event if set
	if(nmap_event_data->event_type == BP_NMAP_EVENT_STOP_NMAP)
	{

		// set event status
		nmap_event_data->status = BP_NMAP_EVENT_STATUS_STOPPING;


	    // check if the app is still running
	    BP_BOOL still_running = BP_TRUE;

	    do
	    {

	    	// check if application is still running
	    	still_running = BP_FExecApplicationStillRunning(nmap->nmap_app) ? BP_TRUE : BP_FALSE;

	    	// if the application is running, attempt to kill it and sleep one sec
	    	if(still_running)
	    	{
	    		BP_FExecKillApplication(nmap->nmap_app);
	    		sleep(1);
	    	}

	    } while(still_running);

	    // set event status
	    nmap_event_data->status = BP_NMAP_EVENT_STATUS_FINISHED;

	    // display message when stopped
	    printf("\n [+] Nmap process has stopped completely.");

	}

	// processor entry macro
	BP_EVENT_PROCESSOR_EXIT;

}



// Load/register all event processors within a provided event manager.  This
// essentially primes the event recievers so that events can be dispatched.
BP_ERROR_T BP_NmapRegisterEventProcessorsWithEVM
(
	P_BP_EVENT_MANAGER evm
)
{

	// ensure we have an event manager
	if(!evm)
		return ERR_FAILURE;

	//attempt to lookup the nmap processor
	P_BP_EVENT_PROCESSOR nmap_processor = BP_EventMgrLookupProcessorByName(evm, NMAP_EVENT_PROCESSOR_NAME);
	if(!nmap_processor)
	{

		// attempt to add the processor if it doesn't exist
		BP_EventMgrAddProcessor
		(
				evm,
				NMAP_EVENT_PROCESSOR_NAME,
				"Nmap event processor.",
				BP_EVENT_PROCESSOR_COMPUTE_TARGET_TYPE_CPU_THREAD,
				BP_NmapThreadCallbackEntryPoint
		);


	}
	// attempt to lookup again after addition attempt
	nmap_processor = BP_EventMgrLookupProcessorByName(evm, NMAP_EVENT_PROCESSOR_NAME);
	if(!nmap_processor)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Start And Stop Nmap %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to start the nmap service if it's unstarted
BP_ERROR_T BP_NmapStart
(
		P_BP_NMAP           nmap
)
{

	// ensure
	if(!nmap)
		return ERR_FAILURE;
	if(!nmap->evm)
		return ERR_FAILURE;


	//attempt to lookup the nmap processor
	P_BP_EVENT_PROCESSOR nmap_processor = BP_EventMgrLookupProcessorByName(nmap->evm, NMAP_EVENT_PROCESSOR_NAME);

	// display processor lookup
	printf("\n [+] nmap_processor lookup %p", nmap_processor);

	// set the run status as starting up
	nmap->run_status = BP_NMAP_EVENT_STATUS_STARTING_UP;

	// if we don't have an event manager entry, add it now
	if(!nmap_processor)
		return ERR_FAILURE;


	// create new nmap event
	P_BP_NMAP_EVENT_DATA nmap_event = (P_BP_NMAP_EVENT_DATA) ht_calloc(nmap->system_hreg, sizeof(BP_NMAP_EVENT_DATA), 1);
	if(!nmap_event)
	{

		// return indicating failure if we can't alloc
		return ERR_FAILURE;

	}

	// set event as a start event
	nmap_event->event_type = BP_NMAP_EVENT_START_NMAP;

	// set nmap reference
	nmap_event->nmap = nmap;

	// create temporary uuid for the event
	char *tmp_uuid = BP_GenRandomUUID();

	// add start tor
	BP_ERROR_T event_create_ret = BP_EventMgrCreateEvent
	(
			nmap->evm,
			(P_BP_EVENT_PROCESSOR) nmap_processor,
			(P_BP_EVENT_PROCESSOR) NULL,
			BP_EVENT_MESSAGES_IN,
			BP_EVENT_TYPE_MARKER_SYSTEM_CORE_EVENT,
			/*"GR4_Start_Tor_API_Server"*/	tmp_uuid,
			"Starting Nmap Process",
			(void *) nmap_event,
			sizeof(nmap_event),
			BP_EVENT_PRIORITY_HIGH
	);


	// dispatch event check
	if(!event_create_ret)
	{
		printf("\n [!!] Nmap start event dispatch failed.");
	}

	// display success message
	printf("\n [+] Nmap startup dispatch succeeded!");

	// attempt to duplicate event uuid
	nmap->start_event_uuid = ht_strdup(nmap->system_hreg, (char *) tmp_uuid);

	// destroy temporary uuid before returning
	bpfree(tmp_uuid);
	tmp_uuid = NULL;

	// return the creation return value
	return event_create_ret;

}



// attempt to stop the nmap service if it's started
BP_ERROR_T BP_NmapStop(P_BP_NMAP nmap)
{


	// run basic null checks
	if(!nmap)
		return ERR_FAILURE;
	if(!nmap->evm)
		return ERR_FAILURE;
	if(!nmap->nmap_app)
		return ERR_FAILURE;


	// return indicating failure if the process is not running
	if(nmap->run_status != BP_NMAP_EVENT_STATUS_RUNNING)
		return ERR_FAILURE;

	// set run status as stopping
	nmap->run_status = BP_NMAP_EVENT_STATUS_STOPPING;

	//attempt to lookup the nmap processor
	P_BP_EVENT_PROCESSOR nmap_processor = BP_EventMgrLookupProcessorByName(nmap->evm, NMAP_EVENT_PROCESSOR_NAME);

	// display processor lookup
	printf("\n [+] nmap_processor lookup %p", nmap_processor);

	// ensure we have a processor
	if(!nmap_processor)
		return ERR_FAILURE;


	// create new nmap event
	P_BP_NMAP_EVENT_DATA nmap_event = (P_BP_NMAP_EVENT_DATA) ht_calloc(nmap->system_hreg, sizeof(BP_NMAP_EVENT_DATA), 1);
	if(!nmap_event)
	{

		// return indicating failure if we can't alloc
		return ERR_FAILURE;

	}

	// set event as a stop event
	nmap_event->event_type = BP_NMAP_EVENT_STOP_NMAP;

	// set nmap reference
	nmap_event->nmap = nmap;

	// create temporary uuid for the event
	char *tmp_uuid = BP_GenRandomUUID();

	// add start nmap
	BP_ERROR_T event_create_ret = BP_EventMgrCreateEvent
	(
			nmap->evm,
			(P_BP_EVENT_PROCESSOR) nmap_processor,
			(P_BP_EVENT_PROCESSOR) NULL,
			BP_EVENT_MESSAGES_IN,
			BP_EVENT_TYPE_MARKER_SYSTEM_CORE_EVENT,
			/*"GR4_Start_Tor_API_Server"*/	tmp_uuid,
			"Stopping Nmap Process",
			(void *) nmap_event,
			sizeof(nmap_event),
			BP_EVENT_PRIORITY_HIGH
	);

	// dispatch event check
	if(!event_create_ret)
	{
		printf("\n [!!] Nmap stop event dispatch failed.");
	}

	printf("\n [+] Nmap stop dispatch succeeded!");

	// attempt to duplicate event uuid
	nmap->stop_event_uuid = ht_strdup(nmap->system_hreg, (char *) tmp_uuid);

	// destroy temporary uuid before returning
	bpfree(tmp_uuid);
	tmp_uuid = NULL;

	// return the creation return value
	return event_create_ret;


}




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Nmap Event Pickup (from main application loop) %%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This picks up events in the main loop and processes/destroys them.
BP_ERROR_T BP_NmapPickupEventsFromMainLoop(P_BP_NMAP nmap)
{

	// basic null checks
	if(!nmap)
		return ERR_FAILURE;
	if(!nmap->evm)
		return ERR_FAILURE;

	// ensure we have an event registry in the manager
	if(!nmap->evm->events_polled_for_main_loop)
	{
		printf("\n [!!] map has no events to process here.");
		return ERR_FAILURE;
	}

	printf("\n [+] Attempting to pickup nmap events from main loop!!");


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Process Start Event %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we have a start event uuid, go ahead and attempt an event lookup
	if(nmap->start_event_uuid)
	{

		// attempt to pickup the uuid
		P_BP_EVENT start_event = BP_EventLoopPickupEventFromPollRegistryByName
		(
			nmap->evm,
			nmap->start_event_uuid
		);


		// process start event
		if(start_event)
		{

			// set start event data
			P_BP_NMAP_EVENT_DATA start_event_data = (P_BP_NMAP_EVENT_DATA) start_event->event_data;

			// if the event reports that we're running, mark it so
			if(start_event_data->status	== BP_NMAP_EVENT_STATUS_RUNNING)
			{

				// set  status to running
				nmap->run_status = BP_NMAP_EVENT_STATUS_RUNNING;

				// destroy start uuid
				ht_free(nmap->system_hreg, nmap->start_event_uuid);
				nmap->start_event_uuid = NULL;

			}

		}

	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Process Stop Event %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we have a stop event uuid, go ahead and attempt an event lookup
	if(nmap->stop_event_uuid)
	{

		// attempt to pickup the uuid
		P_BP_EVENT stop_event = BP_EventLoopPickupEventFromPollRegistryByName
		(
			nmap->evm,
			nmap->stop_event_uuid
		);


		// process stop event
		if(stop_event)
		{

			// set stop event data
			P_BP_NMAP_EVENT_DATA stop_event_data = (P_BP_NMAP_EVENT_DATA) stop_event->event_data;

			// if the event reports that we're running, mark it so
			if(stop_event_data->status	== BP_NMAP_EVENT_STATUS_FINISHED)
			{

				// set nmap status to finished
				nmap->run_status = BP_NMAP_EVENT_STATUS_FINISHED;

				// destroy stop uuid
				ht_free(nmap->system_hreg, nmap->stop_event_uuid);
				nmap->stop_event_uuid = NULL;

			}

		}

	}


	// return indicating success
	return ERR_SUCCESS;

}





// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// display Nmap information
BP_ERROR_T BP_NmapDisplayInfo(P_BP_NMAP nmap)
{

	// ensure we have a session
	if(!nmap)
		return ERR_FAILURE;


	printf("\n [+] Nmap Session     %p", nmap);
	printf("\n\t system_hreg:       %p", nmap->system_hreg);
	printf("\n\t tor_bin_path:      %s", nmap->nmap_bin_path);
	printf("\n\t tor_bin_sha1:      %s", nmap->nmap_bin_sha1);
	printf("\n\t tor_bin_envp:      %p (shown below if non-null value)", nmap->nmap_bin_envp);

	// display envp
	if(nmap->nmap_bin_envp)
	{
		BP_DisplayStringArray
		(
				"\t envp[n]-> ",
				1,
				2,
				nmap->nmap_bin_envp
		);
	}

	// return indicating success
	return ERR_SUCCESS;

}



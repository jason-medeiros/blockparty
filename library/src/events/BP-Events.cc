/*
 * BP-Events.cc
 *
 *  Created on: Nov 30, 2014
 *      Author: root
 */


// Blockparty main include file
#include "../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Global Hash Registry Related Routines %%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This is called to ensure that the /system/event_managers/ table has
// been setup correctly.  It simply searches for an existing semaphore
// table and if it doesn't exist, adds it to /system/.
BP_ERROR_T BP_EventMgrEnsureWorkingGlobalHashTableEnvirionment()
{

	// ensure the hash table has been initialized tok
	if(!(BP_GLOBAL_HASH_TABLE_INIT_OK))
	{
		printf("\n [!!] Error: Attempted to operate on blockparty event manager even though the global hash tables were not initialized.  Please call BP_GLOBAL_HASH_TABLE_INIT before attempting to use blockparty event managment.");
		return ERR_FAILURE;
	}

	// enter critical section before lookup
	BP_GHT_ENTER_CRITICAL_SECTION;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Hash Table Lookups %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Simple flag to indicate via logic below whether or not the /system/event_managment table
	// exists already.
	size_t eventmgr_table_exists = 0;

    // create a table path on the stack for ease of use
	char * table_path_actual[10] = {0};

	// reference to the local stack
	BP_HASH_TABLE_TRAVERSAL_PATH table_path = (char **) &table_path_actual;

	// set path to /system/event_managment
	table_path[0] = "system";
	table_path[1] = "event_managment";
	table_path[2] = NULL;

	// attempt lookup for /system/semaphores
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);

	// display lookup results
	if(tmp_lookup)
		eventmgr_table_exists = BP_TRUE;
	else
		eventmgr_table_exists = BP_FALSE;

	// if the table exists, immediately  return OK
	if(eventmgr_table_exists == BP_TRUE)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Tables If Necessary %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// First check if the system path exists.  If not, create it.
	table_path[0] = "system";
	table_path[1] = NULL;

	// lookup system
	tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);

	// if it's not there, create it
	if(!tmp_lookup)
	{

		// add the table
		BP_HashRegAddTableByPath(NULL, table_path);

		// ensure it exists
		tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);

		// ensure we can lookup the table, and exit if we
		// failed for some reason.
		if(!tmp_lookup)
			BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}

	// now check for the semaphore path
	table_path[0] = "system";
	table_path[1] = "event_managment";
	table_path[2] = NULL;

	// add the table
	BP_HashRegAddTableByPath(NULL, table_path);

	// ensure it exists
	tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);

	// ensure we can lookup the table, and exit if we
	// failed for some reason.
	if(!tmp_lookup)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// leave critical section before returning
	BP_GHT_LEAVE_CRITICAL_SECTION;

	// return indicating success
	return ERR_SUCCESS;

}



// will search the event manager to determine if an event manager exists with the given name.  This
// routine returns a hash table keyed entry.
P_BP_HASH_TABLE_KEYED_ENTRY BP_EventMgrLookupManagerKeyedEntryByNameInGHT(char *mgr_name)
{

	// ensure we have a manager name
	if(!mgr_name)
		return NULL;

	// exit immediately if the name isn't reasonable
	if(BP_StringIsReasonablePrintableString(mgr_name, bpstrlen(mgr_name), BP_FALSE, BP_FALSE) != ERR_SUCCESS)
		return NULL;

	 // create a table path on the stack for ease of use
	char * table_path_actual[10] = {0};

	// reference to the local stack
	BP_HASH_TABLE_TRAVERSAL_PATH table_path = (char **) &table_path_actual;

	// set path to /system/event_managment
	table_path[0] = "system";
	table_path[1] = "event_managment";
	table_path[2] = mgr_name;
	table_path[3] = NULL;

	// attempt lookup for /system/event_managment/mgr_name
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);

	// ensure we have a correct lookup
	if(tmp_lookup)
		return tmp_lookup;

	// return NULL on failure
	return NULL;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Lookup Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// simple routine to retrive an event manager by it's name
P_BP_EVENT_MANAGER BP_EventMgrLookupManagerByName(char *mgr_name)
{

	// first see if a keyed entry exists for this manager
	if(!BP_EventMgrLookupManagerKeyedEntryByNameInGHT(mgr_name))
		return NULL;


	 // create a table path on the stack for ease of use
	char * table_path_actual[10] = {0};

	// reference to the local stack
	BP_HASH_TABLE_TRAVERSAL_PATH table_path = (char **) &table_path_actual;

	// set path to /system/event_managment
	table_path[0] = "system";
	table_path[1] = "event_managment";
	table_path[2] = mgr_name;
	table_path[3] = "manager";
	table_path[4] = NULL;


	// attempt lookup for /system/event_managment/mgr_name
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);

	// ensure we have a correct lookup
	if(tmp_lookup)
	{

		// return the manager (casted) where necessary
		return (P_BP_EVENT_MANAGER) tmp_lookup->data;

	}

	// return null on failure
	return NULL;

}

//! Attempts to lookup the keyed entry table for the event workers threads.
P_BP_HASH_TABLE_KEYED_ENTRY BP_EventMgrLookupWorkersTable(P_BP_EVENT_MANAGER event_manager)
{


	// ensure we have a correct working environment within the event manager
	BP_EVENTMGR_VALIDATE_WORKING_ENVIRONMENT(NULL);

	// ensure we have a manager
	if(!event_manager)
		return NULL;

	// manager name is necessary for lookup
	if(!event_manager->name)
		return NULL;

	// enter critical section before lookup
	BP_GHT_ENTER_CRITICAL_SECTION;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Lookup Workers Table for Manager %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create a table path on the stack for ease of use
	char * table_path_actual[10] = {0};

	// reference to the local stack
	BP_HASH_TABLE_TRAVERSAL_PATH table_path = (char **) &table_path_actual;

	// set path to /system/event_managment/uuid/workers/
	table_path[0] = "system";
	table_path[1] = "event_managment";
	table_path[2] = event_manager->name;
	table_path[3] = "workers";

	// attempt lookup for /system/event_managment/uuid/workers/
	P_BP_HASH_TABLE_KEYED_ENTRY workers_table_lookup = BP_HashRegLookupTableByPath(NULL, table_path);

	// If we can't lookup the workers table, we somehow have a corrupt table.  Return
	// immediately.
	if(!workers_table_lookup)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	// leave the critical section before exiting routine
	BP_GHT_LEAVE_CRITICAL_SECTION;

	// return the workers table
	return workers_table_lookup;


}


//! Lookup an event processor by name.
P_BP_EVENT_PROCESSOR BP_EventMgrLookupProcessorByName(P_BP_EVENT_MANAGER event_manager, char *processor_name)
{


	// ensure we have a correct working environment within the event manager
	BP_EVENTMGR_VALIDATE_WORKING_ENVIRONMENT(NULL);

	// ensure we have a manager
	if(!event_manager)
		return NULL;

	// manager name is necessary for lookup
	if(!event_manager->name)
		return NULL;

	// ensure string is reasonable/printable
	if(!BP_StringIsReasonablePrintableString(processor_name, bpstrlen(processor_name), BP_FALSE, BP_FALSE))
		return NULL;

	// enter critical section before lookup
	BP_GHT_ENTER_CRITICAL_SECTION;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Lookup Processors Table for Manager %%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create a table path on the stack for ease of use
	char * table_path_actual[10] = {0};

	// reference to the local stack
	BP_HASH_TABLE_TRAVERSAL_PATH table_path = (char **) &table_path_actual;

	// set path
	table_path[0] = "system";
	table_path[1] = "event_managment";
	table_path[2] = event_manager->name;
	table_path[3] = "processors";
	table_path[4] = processor_name;
	table_path[5] = "entry";
	table_path[6] = NULL;


	// attempt lookup for the processor
	P_BP_HASH_TABLE_KEYED_ENTRY processor_table_lookup = BP_HashRegLookupTableEntryByPath(NULL, table_path);

	// If we can't lookup the workers table, we somehow have a corrupt table.  Return
	// immediately.
	if(!processor_table_lookup)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	if(!processor_table_lookup->data)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	// extract the processor from the lookup; create reference
	P_BP_EVENT_PROCESSOR processor = (P_BP_EVENT_PROCESSOR) processor_table_lookup->data;

	// leave the critical section before exiting routine
	BP_GHT_LEAVE_CRITICAL_SECTION;

	// return the processor table table
	return processor;


}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Event Manager Create / Destroy %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Create a new event manager
P_BP_EVENT_MANAGER BP_EventMgrCreate(char * mgr_name, char *mgr_description)
{


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Ensure Name/Description are Reasonable %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure that the name and description are reasonable
	BP_BOOL name_is_reasonable = BP_StringIsReasonablePrintableString(mgr_name,        bpstrlen(mgr_name),        BP_FALSE, BP_FALSE);
	BP_BOOL desc_is_reasonable = BP_StringIsReasonablePrintableString(mgr_description, bpstrlen(mgr_description), BP_FALSE, BP_FALSE);

	// ensure the name and description are reasonable
	if(!name_is_reasonable)
		return NULL;
	if(!desc_is_reasonable)
		return NULL;

	// ensure we have a correct working environment within the event manager
	BP_EVENTMGR_VALIDATE_WORKING_ENVIRONMENT(NULL);

	// if the manager already exists, exit immediately
	if(BP_EventMgrLookupManagerByName(mgr_name))
		return NULL;

	// enter critical section to manipulate ght
	BP_GHT_ENTER_CRITICAL_SECTION;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add New Manager Hash Table %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// create table path and zero it
	char *table_path[10];
	BP_ZERO_STACK(table_path);

	// add locks table here
	table_path[0] = "system";
	table_path[1] = "event_managment";
	table_path[2] = mgr_name;
	table_path[3] = NULL;

	// add the table
	BP_ERROR_T err_status = BP_HashRegAddTableByPath(NULL, table_path);

	// Ensure that the table exists.  Exit if not, as it should've been
	// created with the above table add operation.
	if(err_status == ERR_FAILURE)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	// attempt to add data to the table
	err_status = BP_HashRegAddDataToTableByPath
	(
		NULL,
		table_path,
		"manager",
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_EVENT_MANAGER)
	);






	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create events_in Table %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// events
	table_path[0] = "system";
	table_path[1] = "event_managment";
	table_path[2] = mgr_name;
	table_path[3] = "events_in";
	table_path[4] = NULL;
	table_path[5] = NULL;

	// add the events table
	err_status = BP_HashRegAddTableByPath(NULL, table_path);


	// --- Add Individual Priority Tables ---

	// no priority (only executed if no others exist)
	table_path[4] = "BP_EVENT_PRIORITY_NONE";
	err_status = BP_HashRegAddTableByPath(NULL, table_path);

	// very low priority
	table_path[4] = "BP_EVENT_PRIORITY_VERY_LOW";
	err_status = BP_HashRegAddTableByPath(NULL, table_path);

	// low priority
	table_path[4] = "BP_EVENT_PRIORITY_LOW";
	err_status = BP_HashRegAddTableByPath(NULL, table_path);

	// mid normal priority
	table_path[4] = "BP_EVENT_PRIORITY_MID_NORMAL";
	err_status = BP_HashRegAddTableByPath(NULL, table_path);

	// normal priority
	table_path[4] = "BP_EVENT_PRIORITY_NORMAL";
	err_status = BP_HashRegAddTableByPath(NULL, table_path);

	// mid high priority
	table_path[4] = "BP_EVENT_PRIORITY_MID_HIGH";
	err_status = BP_HashRegAddTableByPath(NULL, table_path);

	// high priority
	table_path[4] = "BP_EVENT_PRIORITY_HIGH";
	err_status = BP_HashRegAddTableByPath(NULL, table_path);

	// system priority
	table_path[4] = "BP_EVENT_PRIORITY_SYSTEM";
	err_status = BP_HashRegAddTableByPath(NULL, table_path);

	// critical system priority (executed before ANYTHING else)
	table_path[4] = "BP_EVENT_PRIORITY_CRITICAL_SYSTEM";
	err_status = BP_HashRegAddTableByPath(NULL, table_path);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create events_out Table %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// events out
	table_path[0] = "system";
	table_path[1] = "event_managment";
	table_path[2] = mgr_name;
	table_path[3] = "events_out";
	table_path[4] = NULL;
	table_path[5] = NULL;

	// add the events table
	err_status = BP_HashRegAddTableByPath(NULL, table_path);


	// --- Add Individual Priority Tables ---

	// no priority (only executed if no others exist)
	table_path[4] = "BP_EVENT_PRIORITY_NONE";
	err_status = BP_HashRegAddTableByPath(NULL, table_path);

	// very low priority
	table_path[4] = "BP_EVENT_PRIORITY_VERY_LOW";
	err_status = BP_HashRegAddTableByPath(NULL, table_path);

	// low priority
	table_path[4] = "BP_EVENT_PRIORITY_LOW";
	err_status = BP_HashRegAddTableByPath(NULL, table_path);

	// mid normal priority
	table_path[4] = "BP_EVENT_PRIORITY_MID_NORMAL";
	err_status = BP_HashRegAddTableByPath(NULL, table_path);

	// normal priority
	table_path[4] = "BP_EVENT_PRIORITY_NORMAL";
	err_status = BP_HashRegAddTableByPath(NULL, table_path);

	// mid high priority
	table_path[4] = "BP_EVENT_PRIORITY_MID_HIGH";
	err_status = BP_HashRegAddTableByPath(NULL, table_path);

	// high priority
	table_path[4] = "BP_EVENT_PRIORITY_HIGH";
	err_status = BP_HashRegAddTableByPath(NULL, table_path);

	// system priority
	table_path[4] = "BP_EVENT_PRIORITY_SYSTEM";
	err_status = BP_HashRegAddTableByPath(NULL, table_path);

	// critical system priority (executed before ANYTHING else)
	table_path[4] = "BP_EVENT_PRIORITY_CRITICAL_SYSTEM";
	err_status = BP_HashRegAddTableByPath(NULL, table_path);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Worker Threads Table %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// */events/workers
	table_path[0] = "system";
	table_path[1] = "event_managment";
	table_path[2] = mgr_name;
	table_path[3] = "workers";
	table_path[4] = NULL;

	// add the events table
	err_status = BP_HashRegAddTableByPath(NULL, table_path);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Routes Table %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// routes
	table_path[0] = "system";
	table_path[1] = "event_managment";
	table_path[2] = mgr_name;
	table_path[3] = "processors";
	table_path[4] = NULL;
	table_path[5] = NULL;

	// add the routes table
	err_status = BP_HashRegAddTableByPath(NULL, table_path);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Verify New Manager Exists and Assign Initial Vals %%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add locks table here
	table_path[0] = "system";
	table_path[1] = "event_managment";
	table_path[2] = mgr_name;
	table_path[3] = "manager";
	table_path[4] = NULL;

	// attempt to retrieve the manager entry
	P_BP_HASH_TABLE_KEYED_ENTRY mgr_entry = BP_HashRegLookupTableEntryByPath(NULL, table_path);

	// exit immediately if we have no entry
	if(!mgr_entry)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	if(!mgr_entry->data)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	// create reference and return
	P_BP_EVENT_MANAGER mgr = (P_BP_EVENT_MANAGER) mgr_entry->data;

	// set the name of the manager and the managers description
	mgr->name        = (char *) bpstrdup(mgr_name);
	mgr->description = (char *) bpstrdup(mgr_description);
	mgr->uuid        = (char *) NEW_RANDOM_UUID;

	// create new thread manager
	mgr->thread_manager = BP_CreateThreadManager("event_manager_threadmgr", ".");


	// set the status as available
	mgr->event_loop_state.status = BP_EVENT_LOOP_STATE_STATUS_AVAILABLE;



	// set the event manager lock
	mgr->event_mgr_lock = BP_SemaphoreInitDefault();

	// set the acquisition locks
	mgr->events_in_acquire_lock = BP_SemaphoreInitDefault();

	// init the loop state lock
	mgr->event_loop_state.lock = BP_SemaphoreInitDefault();


	// init events manager polling table for main loop; should never fail
	if(!BP_EventMgrEventsPolledHashRegInit(mgr))
	{
		printf("\n [!!] Event polling hash table (main loop) did not initialize.");
	}

	// lock the lock by default (threads wait on lock to determine
	// when events are available.)
	BP_SemaphoreWait(mgr->event_loop_state.lock);

	// leave the critical section
	BP_GHT_LEAVE_CRITICAL_SECTION;


	// return the new manager
	return mgr;

}


// Destroy an event manager.
BP_ERROR_T BP_EventMgrDestroy(P_BP_EVENT_MANAGER event_manager)
{

	// ensure we have a correct working environment within the event manager
	BP_EVENTMGR_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	// destroy the event manager
	if(!event_manager)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Search Event Manager %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Will return a processor
P_BP_EVENT_PROCESSOR BP_EventMgrEventProcessorLookupByName
(
		P_BP_EVENT_MANAGER mgr,
		char *name
)
{


	// ensure we have an event manager
	if(!mgr)
		return NULL;
	if(!name)
		return NULL;




	// return the processor
	return (P_BP_EVENT_PROCESSOR) NULL;

}





// add an event worker thread
BP_ERROR_T BP_EventMgrAddWorker(P_BP_EVENT_MANAGER mgr, char * worker_name, char * worker_description)
{

	// ensure we have a correct working environment within the event manager
	BP_EVENTMGR_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	// ensure we have a manager
	if(!mgr)
		return ERR_FAILURE;

	// manager name is necessary for lookup
	if(!mgr->name)
		return ERR_FAILURE;

	// ensure that name and description are reasonable printables
	if(BP_StringIsReasonablePrintableString(worker_name,        bpstrlen(worker_name),        BP_FALSE, BP_FALSE) == ERR_FAILURE)
		return ERR_FAILURE;
	if(BP_StringIsReasonablePrintableString(worker_description, bpstrlen(worker_description), BP_FALSE, BP_FALSE) == ERR_FAILURE)
		return ERR_FAILURE;





	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Lookup Workers Table for Manager %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt lookup for /system/event_managment/uuid/workers/
	P_BP_HASH_TABLE_KEYED_ENTRY worker_table = BP_EventMgrLookupWorkersTable(mgr);

	// enter critical section only after lookup of workers table (workers table routing
	// enters critical as well)
	BP_GHT_ENTER_CRITICAL_SECTION;

	// If we can't lookup the workers table, we somehow have a corrupt table.  Return
	// immediately.
	if(!worker_table)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create New Worker Entry %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create a table path on the stack for ease of use
	char * table_path_actual[10] = {0};

	// reference to the local stack
	BP_HASH_TABLE_TRAVERSAL_PATH table_path = (char **) &table_path_actual;

	// add table
	table_path[0] = "system";
	table_path[1] = "event_managment";
	table_path[2] = mgr->name;
	table_path[3] = "workers";
	table_path[4] = worker_name;
	table_path[5] = NULL;

	// add the table path
	BP_HashRegAddTableByPath(NULL,table_path);

	// attempt to lookup the table by path
	P_BP_HASH_TABLE_KEYED_ENTRY worker_name_lookup = BP_HashRegLookupTableByPath(NULL, table_path);

	// ensure we have a worker name lookup
	if(!worker_name_lookup)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create New Worker %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create table path for adding element
	table_path[0] = "system";
	table_path[1] = "event_managment";
	table_path[2] = mgr->name;
	table_path[3] = "workers";
	table_path[4] = worker_name;
	table_path[5] = NULL;

	// attempt to create new managed worker structure
	BP_ERROR_T success_fail = BP_HashRegAddDataToTableByPath
	(
		NULL,
		table_path,
		"entry",
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_EVENT_WORKER)
	);

	// ensure addition was successful
	if(success_fail == ERR_FAILURE)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// create table path for adding element
	table_path[0] = "system";
	table_path[1] = "event_managment";
	table_path[2] = mgr->name;
	table_path[3] = "workers";
	table_path[4] = worker_name;
	table_path[5] = "entry";
	table_path[6] = NULL;

	// attempt to get a handle on the new entry
	P_BP_HASH_TABLE_KEYED_ENTRY new_worker_keyed_entry = BP_HashRegLookupTableEntryByPath
	(
		NULL,
		table_path
	);

	// ensure we can lookup the new worker entry
	if(!new_worker_keyed_entry)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// get reference to new worker
	P_BP_EVENT_WORKER new_worker = (P_BP_EVENT_WORKER) new_worker_keyed_entry->data;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Assign Worker Details before Dispatch %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// --- Set Descriptors

	// copy in the name (previously validated)
	new_worker->name = bpstrdup(worker_name);

	// copy in the description (previously validated)
	new_worker->description = bpstrdup(worker_description);

	// --- Create 3 Worker Registries

	// create worker hash table registry
	new_worker->worker_hreg = BP_CreateHashTableRegistry();

	// create javascript hash table registry
	new_worker->js_persistent_hreg = BP_CreateHashTableRegistry();

	// create output transformation registry
	new_worker->output_transformation_hreg = BP_CreateHashTableRegistry();

	// --- Set errata

	// set the event manager
	new_worker->event_manager = (void *) mgr;

	// set the parent thread id (worker thread id is set from inside worker)
	new_worker->parent_thread_id = pthread_self();



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create The Thread %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// Create the thread using the native thread manager.  Pass in the event manager
	BP_CreateThread
	(
			mgr->thread_manager,
			BP_EventMgrWorkerThread_thInit,
			new_worker,
			sizeof(P_BP_EVENT_WORKER),
			BP_FALSE
	);

	// create a new worker semaphore (must leave ght critical section to do this
	new_worker->lock = BP_SemaphoreInitDefault();

	// leave critical section before exiting
	BP_GHT_LEAVE_CRITICAL_SECTION;

	// premature exit
	return ERR_SUCCESS;

}


// Start all unstarted workers
BP_ERROR_T BP_EventMgrStartWorkers(P_BP_EVENT_MANAGER mgr)
{


	// ensure we have a correct working environment within the event manager
	BP_EVENTMGR_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	// ensure we have a manager
	if(!mgr)
		return ERR_FAILURE;

	// start all unstarted threads
	BP_StartAllUnstartedThreads(mgr->thread_manager);

	// return indicating success
	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Main Loop Event Poll Queue Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Initializes the mgr->events_polled_for_main_loop hash registry with an empty/correct structure.
BP_ERROR_T BP_EventMgrEventsPolledHashRegInit(P_BP_EVENT_MANAGER mgr)
{

	// ensure we have a manager
	if(!mgr)
		return ERR_FAILURE;

	// if we already have a hash table, exit
	if(mgr->events_polled_for_main_loop)
		return ERR_FAILURE;

	// create new hash table registry
	mgr->events_polled_for_main_loop = BP_CreateHashTableRegistry();
	if(!mgr->events_polled_for_main_loop)
		return ERR_FAILURE;

	// create arbitrary table path
	char *table_path[10];
	BP_ZERO_STACK(table_path);

	// table path idx
	size_t table_path_idx = 0;

	// set events table
	table_path[table_path_idx] = "events"; table_path_idx++;

	// attempt to add the table by specified path
	if(!BP_HashRegAddTableByPath(mgr->events_polled_for_main_loop, table_path))
	{
		return ERR_FAILURE;
	}


	// return indicating success
	return ERR_SUCCESS;

}


// add event to the poll hash table so it can be processed by the main loop
BP_ERROR_T BP_EventManagerEventsPolledAddEvent
(
	P_BP_EVENT_MANAGER mgr,
	P_BP_EVENT         event
)
{

	// ensure we have a manager and an event
	if(!mgr)
		return ERR_FAILURE;
	if(!event)
		return ERR_FAILURE;

	// ensure the events polling table has been created (should be created on init)
	if(!mgr->events_polled_for_main_loop)
		return ERR_FAILURE;

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(mgr->events_polled_for_main_loop);

	// create arbtrary table path and zero it
	char *table_path[10];
	BP_ZERO_STACK(table_path);

	// set initial path to zero
	table_path[0] = "events";

	printf("\n DBGCHECK Got Event Name here: %s", event->name);

	// attempt to add data to the table
	BP_HashRegAddDataToTableByPath
	(
		mgr->events_polled_for_main_loop,
		(char **) &table_path,
		event->name,
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		event,
		sizeof(BP_EVENT)
	);


	// leave critical section and return indicating success
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(mgr->events_polled_for_main_loop, ERR_SUCCESS)

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Event Pickup and Checkin %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// -----------------------------------------------------
// --- Pickup Events From A Worker Thread --------------
// -----------------------------------------------------

// Pickup specific event from the current worker thread.
P_BP_EVENT BP_EventMgrPickupEventFromWorker
(
		P_BP_EVENT_WORKER       worker,
		BP_EVENT_MESSAGE_IN_OUT pickup_from_queue,
		BP_EVENT_PRIORITY       event_priority
)
{

	// ensure we have a worker, a queue, and a priority
	if(!worker)
		return NULL;
	if(!pickup_from_queue)
		return NULL;
	if(!event_priority)
		return NULL;

	// set event manager
	P_BP_EVENT_MANAGER mgr = (P_BP_EVENT_MANAGER) worker->event_manager;

	// enter critical ght section
	BP_GHT_ENTER_CRITICAL_SECTION;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Stringify The Event Priority (for lookup) %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// reference which will contain a static string
	char * priority_selected = NULL;

	// switch on the event priority and
	switch(event_priority)
	{

		case BP_EVENT_PRIORITY_VERY_LOW:
			priority_selected = "BP_EVENT_PRIORITY_VERY_LOW";
			break;

		case BP_EVENT_PRIORITY_LOW:
			priority_selected = "BP_EVENT_PRIORITY_LOW";
			break;

		case BP_EVENT_PRIORITY_MID_NORMAL:
			priority_selected = "BP_EVENT_PRIORITY_MID_NORMAL";
			break;

		case BP_EVENT_PRIORITY_NORMAL:
			priority_selected = "BP_EVENT_PRIORITY_NORMAL";
			break;

		case BP_EVENT_PRIORITY_MID_HIGH:
			priority_selected = "BP_EVENT_PRIORITY_MID_HIGH";
			break;

		case BP_EVENT_PRIORITY_HIGH:
			priority_selected = "BP_EVENT_PRIORITY_HIGH";
			break;

		case BP_EVENT_PRIORITY_SYSTEM:
			priority_selected = "BP_EVENT_PRIORITY_SYSTEM";
			break;

		case BP_EVENT_PRIORITY_CRITICAL_SYSTEM:
			priority_selected = "BP_EVENT_PRIORITY_CRITICAL_SYSTEM";
			break;

		// Unlock and exit with failure if we cannot
		// map a priority.
		default:
			BP_EVENTS_SEM_UNLOCK(mgr->lock);
			BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	}

	// unlock and exit if there is no priority (should never be able
	// to get here, but still adding the check anyway)
	if(!priority_selected)
	{
		BP_EVENTS_SEM_UNLOCK(mgr->lock);
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Lookup Events In Table %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create table path on the stack
	char * table_path_actual[10] = {0};

	// set a reference to the correct type
	BP_HASH_TABLE_TRAVERSAL_PATH table_path = (char **) &table_path_actual;

	// set the table path
	table_path[0] = "system";
	table_path[1] = "event_managment";
	table_path[2] = mgr->name;

	// switch to determine which queue to use
	switch(pickup_from_queue)
	{

		// use messages in queue
		case BP_EVENT_MESSAGES_IN:
			table_path[3] = "events_in";
			break;

		// messages out
		case BP_EVENT_MESSAGES_OUT:
			table_path[3] = "events_out";
			break;

		// if it's not one of the message queues, unlock and exit with
		// failure code.
		default:
			BP_EVENTS_SEM_UNLOCK(mgr->lock);
			BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	}

	// set the 4th position as the selected priority
	table_path[4] = priority_selected;

	// force truncation of table
	table_path[5] = NULL;

	// attempt to lookup the newly created entry
	P_BP_HASH_TABLE_KEYED_ENTRY lookup_entry = BP_HashRegLookupTableByPath
	(
		NULL,
		table_path
	);

	// exit if we couldn't look up the entry
	if(!lookup_entry)
	{
		BP_EVENTS_SEM_UNLOCK(mgr->lock);
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);
	}

	// set lookup table data
	P_BP_HASH_TABLE_KEYED_ENTRY lookup_table = (P_BP_HASH_TABLE_KEYED_ENTRY) lookup_entry->data;

	// set lookup table
	if(!lookup_table)
	{
			BP_EVENTS_SEM_UNLOCK(mgr->lock);
			BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Iterate Events %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// temporary iterator values used during the HASH_ITER loop below
	P_BP_HASH_TABLE_KEYED_ENTRY entry       = NULL;
	P_BP_HASH_TABLE_KEYED_ENTRY tmp         = NULL;

	// next event to process (set in loop below)
	P_BP_HASH_TABLE_KEYED_ENTRY next_event = NULL;

	// iterate through all hashes
	HASH_ITER(hh, lookup_table, entry, tmp)
	{

		// if we have an entry, set next_event and exit immediately
		if(entry->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{
			next_event = (P_BP_HASH_TABLE_KEYED_ENTRY) entry;
			break;

		}

	}

	// if there is no next event to process, exit immediately
	if(!next_event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	// if we could pick up an event, set the returned event as the data (allocate first since
	// destroying the entry frees the associated memory)
	P_BP_EVENT picked_up_event = (P_BP_EVENT) bpcalloc(sizeof(BP_EVENT), 1);

	// copy in the data
	memcpy(picked_up_event, next_event->data, sizeof(BP_EVENT));

	// set the worker after pickup/copy
	picked_up_event->worker = (P_BP_EVENT_WORKER) worker;

	// Remove the event from the queue (aka destroy it from table).  This
	// free's the event, and removes the entry from the queue table.
	BP_HashRegDestroyEntry(NULL, next_event);

	// leave the critical ght section
	BP_GHT_LEAVE_CRITICAL_SECTION;

	// return the new entry
	return picked_up_event;

}


// Pickup the highest priority event from the current worker thread
P_BP_EVENT BP_EventMgrPickupHighestPriorityEventFromWorker
(
		P_BP_EVENT_WORKER worker,
		BP_EVENT_MESSAGE_IN_OUT pickup_from_queue
)
{

	// ensure we have a worker and a pickup queue designated
	if(!worker)
		return NULL;
	if(!pickup_from_queue)
		return NULL;

	// set event
	P_BP_EVENT event = NULL;

	// enter critical section
	BP_GHT_ENTER_CRITICAL_SECTION;


	// --- Critical System ------------------

	// attempt to pick up a critical system event (highest level of event)
	event = BP_EventMgrPickupEventFromWorker
	(
			worker,
			pickup_from_queue,
			BP_EVENT_PRIORITY_CRITICAL_SYSTEM
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// --- System

	// attempt to pick up a system event
	event = BP_EventMgrPickupEventFromWorker
	(
			worker,
			pickup_from_queue,
			BP_EVENT_PRIORITY_SYSTEM
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// --- High

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromWorker
	(
			worker,
			pickup_from_queue,
			BP_EVENT_PRIORITY_HIGH
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// --- Mid High

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromWorker
	(
			worker,
			pickup_from_queue,
			BP_EVENT_PRIORITY_MID_HIGH
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// --- Normal

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromWorker
	(
			worker,
			pickup_from_queue,
			BP_EVENT_PRIORITY_NORMAL
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// --- Mid Normal

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromWorker
	(
			worker,
			pickup_from_queue,
			BP_EVENT_PRIORITY_MID_NORMAL
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// --- Low

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromWorker
	(
			worker,
			pickup_from_queue,
			BP_EVENT_PRIORITY_LOW
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// --- Very Low

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromWorker
	(
			worker,
			pickup_from_queue,
			BP_EVENT_PRIORITY_VERY_LOW
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// exit critical section
	BP_GHT_LEAVE_CRITICAL_SECTION;

	// return null if no events can be found
	return NULL;

}

// Pickup the lowest priority event from the current worker thread
P_BP_EVENT BP_EventMgrPickupLowestPriorityEventFromWorker
(
		P_BP_EVENT_WORKER worker,
		BP_EVENT_MESSAGE_IN_OUT pickup_from_queue
)
{

	// ensure we have a worker and a pickup queue designated
	if(!worker)
		return NULL;
	if(!pickup_from_queue)
		return NULL;

	// set event
	P_BP_EVENT event = NULL;

	// enter critical section
	BP_GHT_ENTER_CRITICAL_SECTION;



	// --- Very Low

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromWorker
	(
			worker,
			pickup_from_queue,
			BP_EVENT_PRIORITY_VERY_LOW
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// --- Low

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromWorker
	(
			worker,
			pickup_from_queue,
			BP_EVENT_PRIORITY_LOW
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);


	// --- Mid Normal

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromWorker
	(
			worker,
			pickup_from_queue,
			BP_EVENT_PRIORITY_MID_NORMAL
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// --- Normal

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromWorker
	(
			worker,
			pickup_from_queue,
			BP_EVENT_PRIORITY_NORMAL
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);


	// --- Mid High

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromWorker
	(
			worker,
			pickup_from_queue,
			BP_EVENT_PRIORITY_MID_HIGH
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// --- High

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromWorker
	(
			worker,
			pickup_from_queue,
			BP_EVENT_PRIORITY_HIGH
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);


	// --- System

	// attempt to pick up a system event
	event = BP_EventMgrPickupEventFromWorker
	(
			worker,
			pickup_from_queue,
			BP_EVENT_PRIORITY_SYSTEM
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);


	// --- Critical System

	// attempt to pick up a system event
	event = BP_EventMgrPickupEventFromWorker
	(
			worker,
			pickup_from_queue,
			BP_EVENT_PRIORITY_CRITICAL_SYSTEM
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// exit critical section
	BP_GHT_LEAVE_CRITICAL_SECTION;

	// return null if no events can be found
	return NULL;

}

// -----------------------------------------------------
// --- Pickup Events From Main Event Loop --------------
// -----------------------------------------------------


// Pickup event from main loop.
P_BP_EVENT BP_EventMgrPickupEventFromMainLoop
(
		P_BP_EVENT_MANAGER      mgr,
		BP_EVENT_MESSAGE_IN_OUT pickup_from_queue,
		BP_EVENT_PRIORITY       event_priority
)
{


	// ensure we have an event manager, a queue, and a priority
	if(!mgr)
		return NULL;
	if(!pickup_from_queue)
		return NULL;
	if(!event_priority)
		return NULL;

	// enter critical ght section
	BP_GHT_ENTER_CRITICAL_SECTION;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Stringify The Event Priority (for lookup) %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// reference which will contain a static string
	char * priority_selected = NULL;

	// switch on the event priority and
	switch(event_priority)
	{

		case BP_EVENT_PRIORITY_VERY_LOW:
			priority_selected = "BP_EVENT_PRIORITY_VERY_LOW";
			break;

		case BP_EVENT_PRIORITY_LOW:
			priority_selected = "BP_EVENT_PRIORITY_LOW";
			break;

		case BP_EVENT_PRIORITY_MID_NORMAL:
			priority_selected = "BP_EVENT_PRIORITY_MID_NORMAL";
			break;

		case BP_EVENT_PRIORITY_NORMAL:
			priority_selected = "BP_EVENT_PRIORITY_NORMAL";
			break;

		case BP_EVENT_PRIORITY_MID_HIGH:
			priority_selected = "BP_EVENT_PRIORITY_MID_HIGH";
			break;

		case BP_EVENT_PRIORITY_HIGH:
			priority_selected = "BP_EVENT_PRIORITY_HIGH";
			break;

		case BP_EVENT_PRIORITY_SYSTEM:
			priority_selected = "BP_EVENT_PRIORITY_SYSTEM";
			break;

		case BP_EVENT_PRIORITY_CRITICAL_SYSTEM:
			priority_selected = "BP_EVENT_PRIORITY_CRITICAL_SYSTEM";
			break;

		// Unlock and exit with failure if we cannot
		// map a priority.
		default:
			BP_EVENTS_SEM_UNLOCK(mgr->lock);
			BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	}


	// unlock and exit if there is no priority (should never be able
	// to get here, but still adding the check anyway)
	if(!priority_selected)
	{
		BP_EVENTS_SEM_UNLOCK(mgr->lock);
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Lookup Events In Table %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create table path on the stack
	char * table_path_actual[10] = {0};
	BP_ZERO_STACK(table_path_actual);

	// set a reference to the correct type
	BP_HASH_TABLE_TRAVERSAL_PATH table_path = (char **) &table_path_actual;

	// set the table path
	table_path[0] = "system";
	table_path[1] = "event_managment";
	table_path[2] = mgr->name;

	// switch to determine which queue to use
	switch(pickup_from_queue)
	{

		// use messages in queue
		case BP_EVENT_MESSAGES_IN:
			table_path[3] = "events_in";
			break;

		// messages out
		case BP_EVENT_MESSAGES_OUT:
			table_path[3] = "events_out";
			break;

		// if it's not one of the message queues, unlock and exit with
		// failure code.
		default:
			BP_EVENTS_SEM_UNLOCK(mgr->lock);
			BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	}

	// set the 4th position as the selected priority
	table_path[4] = priority_selected;

	// force truncation of table
	table_path[5] = NULL;

	// attempt to lookup the newly created entry
	P_BP_HASH_TABLE_KEYED_ENTRY lookup_entry = BP_HashRegLookupTableByPath
	(
		NULL,
		table_path
	);

	// exit if we couldn't look up the entry
	if(!lookup_entry)
	{
		BP_EVENTS_SEM_UNLOCK(mgr->lock);
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);
	}

	// set lookup table data
	P_BP_HASH_TABLE_KEYED_ENTRY lookup_table = (P_BP_HASH_TABLE_KEYED_ENTRY) lookup_entry->data;

	// set lookup table
	if(!lookup_table)
	{
			BP_EVENTS_SEM_UNLOCK(mgr->lock);
			BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Iterate Events %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// temporary iterator values used during the HASH_ITER loop below
	P_BP_HASH_TABLE_KEYED_ENTRY entry       = NULL;
	P_BP_HASH_TABLE_KEYED_ENTRY tmp         = NULL;

	// next event to process (set in loop below)
	P_BP_HASH_TABLE_KEYED_ENTRY next_event = NULL;

	// iterate through all hashes
	HASH_ITER(hh, lookup_table, entry, tmp)
	{

		// if we have an entry, set next_event and exit immediately
		if(entry->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{
			next_event = (P_BP_HASH_TABLE_KEYED_ENTRY) entry;
			break;

		}

	}

	// if there is no next event to process, exit immediately
	if(!next_event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	// if we could pick up an event, set the returned event as the data (allocate first since
	// destroying the entry frees the associated memory)
	P_BP_EVENT picked_up_event = (P_BP_EVENT) bpcalloc(sizeof(BP_EVENT), 1);

	// copy in the data
	memcpy(picked_up_event, next_event->data, sizeof(BP_EVENT));

	// Remove the event from the queue (aka destroy it from table).  This
	// free's the event, and removes the entry from the queue table.
	BP_HashRegDestroyEntry(NULL, next_event);

	// leave the critical ght section
	BP_GHT_LEAVE_CRITICAL_SECTION;

	// return the new entry
	return picked_up_event;

}

// Pickup the highest priority event from main loop.
P_BP_EVENT BP_EventMgrPickupHighestPriorityEventFromMainLoop
(
		P_BP_EVENT_MANAGER      mgr,
		BP_EVENT_MESSAGE_IN_OUT pickup_from_queue
)
{

	// ensure we have a worker and a pickup queue designated
	if(!mgr)
		return NULL;
	if(!pickup_from_queue)
		return NULL;

	// set event
	P_BP_EVENT event = NULL;

	// enter critical section
	BP_GHT_ENTER_CRITICAL_SECTION;


	// --- Critical System ------------------

	// attempt to pick up a critical system event (highest level of event)
	event = BP_EventMgrPickupEventFromMainLoop
	(
			mgr,
			pickup_from_queue,
			BP_EVENT_PRIORITY_CRITICAL_SYSTEM
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// --- System

	// attempt to pick up a system event
	event = BP_EventMgrPickupEventFromMainLoop
	(
			mgr,
			pickup_from_queue,
			BP_EVENT_PRIORITY_SYSTEM
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// --- High

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromMainLoop
	(
			mgr,
			pickup_from_queue,
			BP_EVENT_PRIORITY_HIGH
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// --- Mid High

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromMainLoop
	(
			mgr,
			pickup_from_queue,
			BP_EVENT_PRIORITY_MID_HIGH
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// --- Normal

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromMainLoop
	(
			mgr,
			pickup_from_queue,
			BP_EVENT_PRIORITY_NORMAL
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// --- Mid Normal

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromMainLoop
	(
			mgr,
			pickup_from_queue,
			BP_EVENT_PRIORITY_MID_NORMAL
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// --- Low

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromMainLoop
	(
			mgr,
			pickup_from_queue,
			BP_EVENT_PRIORITY_LOW
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// --- Very Low

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromMainLoop
	(
			mgr,
			pickup_from_queue,
			BP_EVENT_PRIORITY_VERY_LOW
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// exit critical section
	BP_GHT_LEAVE_CRITICAL_SECTION;

	// return null if no events can be found
	return NULL;

}

// Pickup the lowest priority event from the current worker thread
P_BP_EVENT BP_EventMgrPickupLowestPriorityEventFromMainLoop
(
		P_BP_EVENT_MANAGER      mgr,
		BP_EVENT_MESSAGE_IN_OUT pickup_from_queue
)
{

	// ensure we have a mgr and a pickup queue designated
	if(!mgr)
		return NULL;
	if(!pickup_from_queue)
		return NULL;

	// set event
	P_BP_EVENT event = NULL;

	// enter critical section
	BP_GHT_ENTER_CRITICAL_SECTION;



	// --- Very Low

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromMainLoop
	(
			mgr,
			pickup_from_queue,
			BP_EVENT_PRIORITY_VERY_LOW
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// --- Low

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromMainLoop
	(
			mgr,
			pickup_from_queue,
			BP_EVENT_PRIORITY_LOW
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);


	// --- Mid Normal

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromMainLoop
	(
			mgr,
			pickup_from_queue,
			BP_EVENT_PRIORITY_MID_NORMAL
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// --- Normal

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromMainLoop
	(
			mgr,
			pickup_from_queue,
			BP_EVENT_PRIORITY_NORMAL
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);


	// --- Mid High

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromMainLoop
	(
			mgr,
			pickup_from_queue,
			BP_EVENT_PRIORITY_MID_HIGH
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// --- High

	// attempt to pick up event
	event = BP_EventMgrPickupEventFromMainLoop
	(
			mgr,
			pickup_from_queue,
			BP_EVENT_PRIORITY_HIGH
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);


	// --- System

	// attempt to pick up a system event
	event = BP_EventMgrPickupEventFromMainLoop
	(
			mgr,
			pickup_from_queue,
			BP_EVENT_PRIORITY_SYSTEM
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);


	// --- Critical System

	// attempt to pick up a system event
	event = BP_EventMgrPickupEventFromMainLoop
	(
			mgr,
			pickup_from_queue,
			BP_EVENT_PRIORITY_CRITICAL_SYSTEM
	);

	// return the event if it was looked up ok
	if(event)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(event);

	// exit critical section
	BP_GHT_LEAVE_CRITICAL_SECTION;

	// return null if no events can be found
	return NULL;

}



// -----------------------------------------------------
// --- Checkins ----------------------------------------
// -----------------------------------------------------

// This takes an existing event and attempts to check it into a queue (works similarly to BP_EventMgrCreateEvent).
BP_ERROR_T BP_EventMgrCheckinProcessedEvent
(
	P_BP_EVENT_WORKER       worker,
	P_BP_EVENT              event_to_checkin,
	BP_EVENT_MESSAGE_IN_OUT checkin_to_queue,
	BP_EVENT_PRIORITY       event_priority
)
{

	// run basic parameter checks
	if(!worker)
		return ERR_FAILURE;
	if(!worker->event_manager)
		return ERR_FAILURE;
	if(!checkin_to_queue)
		return ERR_FAILURE;
	if(!event_priority)
		return ERR_FAILURE;
	if(!event_to_checkin)
		return ERR_FAILURE;

	// enter critical section
	BP_GHT_ENTER_CRITICAL_SECTION;

	// set the event manager
	P_BP_EVENT_MANAGER mgr = (P_BP_EVENT_MANAGER) worker->event_manager;

	// ensure names are reasonable printables
	if(!BP_StringIsReasonablePrintableString(worker->name, bpstrlen(worker->name), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(mgr->name,    bpstrlen(mgr->name),    BP_FALSE, BP_FALSE))
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(event_to_checkin->name, bpstrlen(event_to_checkin->name), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Stringify The Event Priority (for lookup) %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// reference which will contain a static string
	char * priority_selected = NULL;

	// switch on the event priority and
	switch(event_priority)
	{

		case BP_EVENT_PRIORITY_VERY_LOW:
			priority_selected = "BP_EVENT_PRIORITY_VERY_LOW";
			break;

		case BP_EVENT_PRIORITY_LOW:
			priority_selected = "BP_EVENT_PRIORITY_LOW";
			break;

		case BP_EVENT_PRIORITY_MID_NORMAL:
			priority_selected = "BP_EVENT_PRIORITY_MID_NORMAL";
			break;

		case BP_EVENT_PRIORITY_NORMAL:
			priority_selected = "BP_EVENT_PRIORITY_NORMAL";
			break;

		case BP_EVENT_PRIORITY_MID_HIGH:
			priority_selected = "BP_EVENT_PRIORITY_MID_HIGH";
			break;

		case BP_EVENT_PRIORITY_HIGH:
			priority_selected = "BP_EVENT_PRIORITY_HIGH";
			break;

		case BP_EVENT_PRIORITY_SYSTEM:
			priority_selected = "BP_EVENT_PRIORITY_SYSTEM";
			break;

		case BP_EVENT_PRIORITY_CRITICAL_SYSTEM:
			priority_selected = "BP_EVENT_PRIORITY_CRITICAL_SYSTEM";
			break;

		// Unlock and exit with failure if we cannot
		// map a priority.
		default:
			BP_EVENTS_SEM_UNLOCK(mgr->lock);
			BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}

	// unlock and exit if there is no priority (should never be able
	// to get here, but still adding the check anyway)
	if(!priority_selected)
	{
		BP_EVENTS_SEM_UNLOCK(mgr->lock);
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Lookup Events In Table %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create table path on the stack
	char * table_path_actual[10] = {0};

	// set a reference to the correct type
	BP_HASH_TABLE_TRAVERSAL_PATH table_path = (char **) &table_path_actual;

	// set the table path
	table_path[0] = "system";
	table_path[1] = "event_managment";
	table_path[2] = mgr->name;



	// switch to determine which queue to use
	switch(checkin_to_queue)
	{

		// use messages in queue
		case BP_EVENT_MESSAGES_IN:
			table_path[3] = "events_in";
			break;

		// messages out
		case BP_EVENT_MESSAGES_OUT:
			table_path[3] = "events_out";
			break;

		// if it's not one of the message queues, unlock and exit with
		// failure code.
		default:
			BP_EVENTS_SEM_UNLOCK(mgr->lock);
			BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}

	// set the 4th position as the selected priority
	table_path[4] = priority_selected;

	// force truncation of table
	table_path[5] = NULL;



	// Create new binary blob in the table, name it after the processor
	// name.
	if
	(
		BP_HashRegAddDataToTableByPath
		(
			NULL,
			table_path,
			event_to_checkin->name,
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
			NULL,
			sizeof(BP_EVENT)
		) != ERR_SUCCESS
	)
	{
		BP_EVENTS_SEM_UNLOCK(mgr->lock);
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// Since we're now looking up an entry, we have to have
	// an entry name for lookup, so set that here.  Also truncate
	// next position in path.
	table_path[5] = event_to_checkin->name;
	table_path[6] = NULL;

	// attempt to lookup the newly created entry
	P_BP_HASH_TABLE_KEYED_ENTRY new_entry = BP_HashRegLookupTableEntryByPath
	(
		NULL,
		table_path
	);

	// exit if we couldn't look up the entry
	if(!new_entry)
	{
		BP_EVENTS_SEM_UNLOCK(mgr->lock);
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// set new event as data
	P_BP_EVENT new_event = (P_BP_EVENT) new_entry->data;

	// copy in the new event
	memcpy(new_event, event_to_checkin, sizeof(BP_EVENT));

	// leave critical section
	BP_GHT_LEAVE_CRITICAL_SECTION;

	// return indicating success
	return ERR_SUCCESS;

}
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Event Creation %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// create events
BP_ERROR_T BP_EventMgrCreateEvent
(
		P_BP_EVENT_MANAGER         mgr,
		P_BP_EVENT_PROCESSOR       event_processor,
		P_BP_EVENT_PROCESSOR       event_output_transformation_processor,
		BP_EVENT_MESSAGE_IN_OUT    append_to_queue,
		BP_EVENT_TYPE_MARKER       event_type,
		char *                     event_name,
		char *                     event_description,
		void *                     event_data,
		size_t                     event_data_size,
		BP_EVENT_PRIORITY          event_priority
)
{


	// ensure we have a correct working environment within the event manager
	BP_EVENTMGR_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check Values %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure we have a manager and a tail queue allocator
	if(!mgr)
		return ERR_FAILURE;
	if(!event_processor)
		return ERR_FAILURE;
	if(!append_to_queue)
		return ERR_FAILURE;
	if(!event_name)
		return ERR_FAILURE;
	if(!event_description)
		return ERR_FAILURE;
	if(!event_data)
		return ERR_FAILURE;
	if(!event_data_size)
		return ERR_FAILURE;
	if(!event_priority)
		return ERR_FAILURE;

	// lock the semaphore if all iniital checks passed
	BP_EVENTS_SEM_LOCK(mgr->lock);

	// enter critical GHT section since we'll be working on the ght
	BP_GHT_ENTER_CRITICAL_SECTION;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Stringify The Event Priority %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// reference which will contain a static string
	char * priority_selected = NULL;

	// switch on the event priority and
	switch(event_priority)
	{

		case BP_EVENT_PRIORITY_VERY_LOW:
			priority_selected = "BP_EVENT_PRIORITY_VERY_LOW";
			break;

		case BP_EVENT_PRIORITY_LOW:
			priority_selected = "BP_EVENT_PRIORITY_LOW";
			break;

		case BP_EVENT_PRIORITY_MID_NORMAL:
			priority_selected = "BP_EVENT_PRIORITY_MID_NORMAL";
			break;

		case BP_EVENT_PRIORITY_NORMAL:
			priority_selected = "BP_EVENT_PRIORITY_NORMAL";
			break;

		case BP_EVENT_PRIORITY_MID_HIGH:
			priority_selected = "BP_EVENT_PRIORITY_MID_HIGH";
			break;

		case BP_EVENT_PRIORITY_HIGH:
			priority_selected = "BP_EVENT_PRIORITY_HIGH";
			break;

		case BP_EVENT_PRIORITY_SYSTEM:
			priority_selected = "BP_EVENT_PRIORITY_SYSTEM";
			break;

		case BP_EVENT_PRIORITY_CRITICAL_SYSTEM:
			priority_selected = "BP_EVENT_PRIORITY_CRITICAL_SYSTEM";
			break;

		// Unlock and exit with failure if we cannot
		// map a priority.
		default:
			BP_EVENTS_SEM_UNLOCK(mgr->lock);
			return ERR_FAILURE;

	}

	// unlock and exit if there is no priority (should never be able
	// to get here, but still adding the check anyway)
	if(!priority_selected)
	{
		BP_EVENTS_SEM_UNLOCK(mgr->lock);
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Lookup Events In Table %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create table path on the stack
	char * table_path_actual[10] = {0};

	// set a reference to the correct type
	BP_HASH_TABLE_TRAVERSAL_PATH table_path = (char **) &table_path_actual;

	// set the table path
	table_path[0] = "system";
	table_path[1] = "event_managment";
	table_path[2] = mgr->name;



	// switch to determine which queue to use
	switch(append_to_queue)
	{

		// use messages in queue
		case BP_EVENT_MESSAGES_IN:
			table_path[3] = "events_in";
			break;

		// messages out
		case BP_EVENT_MESSAGES_OUT:
			table_path[3] = "events_out";
			break;

		// if it's not one of the message queues, unlock and exit with
		// failure code.
		default:
			BP_EVENTS_SEM_UNLOCK(mgr->lock);
			BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}

	// set the 4th position as the selected priority
	table_path[4] = priority_selected;

	// force truncation of table
	table_path[5] = NULL;



	// Create new binary blob in the table, name it after the processor
	// name.
	if
	(
		BP_HashRegAddDataToTableByPath
		(
			NULL,
			table_path,
			event_name,
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
			NULL,
			sizeof(BP_EVENT)
		) != ERR_SUCCESS
	)
	{
		BP_EVENTS_SEM_UNLOCK(mgr->lock);
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// Since we're now looking up an entry, we have to have
	// an entry name for lookup, so set that here.  Also truncate
	// next position in path.
	table_path[5] = event_name;
	table_path[6] = NULL;

	// attempt to lookup the newly created entry
	P_BP_HASH_TABLE_KEYED_ENTRY new_entry = BP_HashRegLookupTableEntryByPath
	(
		NULL,
		table_path
	);

	// exit if we couldn't look up the entry
	if(!new_entry)
	{
		BP_EVENTS_SEM_UNLOCK(mgr->lock);
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Allocate Space for Event %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create space for the new event
	P_BP_EVENT event = (P_BP_EVENT) new_entry->data;

	// duplicate the name
	event->name        = (char *) bpstrdup(event_name);

	// duplicate the description
	event->description = (char *) bpstrdup(event_description);

	// set event prioririty
    event->priority = event_priority;

	// set the event status as waiting for processing
	event->status = BP_EVENT_STATUS_INITIALIZED_AWAITING_PROCESSING;

	// Set entry data directly.
	event->event_data = (void *) event_data;

	// Set entry data size
	event->event_data_size = event_data_size;

	// set the processor
	event->processor    = event_processor;

	// set the output transformation processor
	event->ot_processor = event_output_transformation_processor;

	// set the dispatch state as waiting for dispatch
	event->dispatch_state = BP_EVENT_DISPATCH_STATE_WAITING_FOR_DISPATCH;

	// set the event type
	event->type = event_type;

	// increment the total number of events int
	mgr->events_in_total++;




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Unlock Event Loop State / Exit %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the status as available
	mgr->event_loop_state.status = BP_EVENT_LOOP_STATE_STATUS_AVAILABLE;

	// leave the critical section before exiting
	BP_GHT_LEAVE_CRITICAL_SECTION;

	// unlock state
	BP_EVENTS_SEM_UNLOCK(mgr->lock);


	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Work Distribution Calculators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Event Loop Poll %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// This routine is to be used in the main event loop.  When it is called, it examines the events_in
// table and determines whether or not there are events to be processed.
BP_ERROR_T BP_EventLoopPollForEventsAndLockForMainLoop(P_BP_EVENT_MANAGER mgr)
{

	// ensure we have a correct working environment within the event manager
	BP_EVENTMGR_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	// ensure we have a manager
	if(!mgr)
		return ERR_FAILURE;

	// attempt to lock the manager to pickup events
	BP_SemaphorePost(mgr->event_loop_state.lock);


	// Attempt to pickup event from main loop.
	P_BP_EVENT picked_up_event = NULL;

	// total number of events picked up
	size_t event_pickup_count = 0;

	// loop until we can't pickup any more events
	for(;;)
	{


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Pickup Event from events_out %%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// attempt to pickup highest priority events
		picked_up_event = BP_EventMgrPickupHighestPriorityEventFromMainLoop
		(
			mgr,
			BP_EVENT_MESSAGES_OUT
		);

		// ensure we could pickup an event
		if(!picked_up_event)
			break;


		printf("\n [!!DBG] Got picked up event?: %p - %p", picked_up_event, mgr->events_polled_for_main_loop);

		printf("\n Event Info: %s - %s", picked_up_event->name, picked_up_event->description);


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Store Picked Up Events in Poll HashReg %%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// add event to the event manager
		BP_EventManagerEventsPolledAddEvent(mgr, picked_up_event);

		printf("\n [+] Events Registry Here:");
		BP_HashRegDisplay(mgr->events_polled_for_main_loop, BP_FALSE, BP_TRUE);
		BP_HashRegDisplay(NULL, BP_FALSE, BP_TRUE);

	}


	/*
	P_BP_EVENT BP_EventMgrPickupEventFromWorker
	(
			P_BP_EVENT_WORKER       worker,
			BP_EVENT_MESSAGE_IN_OUT pickup_from_queue,
			BP_EVENT_PRIORITY       event_priority
	)
	*/


	// always return err success unless we simply could not poll (aka missing parameters above)
	return ERR_SUCCESS;

}

// This should be called at the foot of the event loop after  BP_EventLoopPollForEventsAndLockForMainLoop has been called.
BP_ERROR_T BP_EventLoopUnlockForWorkers(P_BP_EVENT_MANAGER mgr)
{

	// ensure we have a correct working environment within the event manager
	BP_EVENTMGR_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	// ensure we have a manager
	if(!mgr)
		return ERR_FAILURE;

	// lock before continuing loop
	BP_SemaphoreWait(mgr->event_loop_state.lock);

	// always return err success
	return ERR_SUCCESS;

}


// Attempts to pickup an event (removing it from the registry) by name.  Once the event is picked up,
// the pickup routine must destroy the event itself using it's own destructors.  It will no longer be
// tracked by the event system.
P_BP_EVENT BP_EventLoopPickupEventFromPollRegistryByName
(
	P_BP_EVENT_MANAGER mgr,
	char              *event_name
)
{

	// run basic checks
	if(!mgr)
		return NULL;
	if(!event_name)
		return NULL;
	if(!mgr->events_polled_for_main_loop)
		return NULL;
	if(!BP_StringIsReasonablePrintableString(event_name, bpstrlen(event_name), BP_FALSE, BP_FALSE))
		return NULL;

	// attempt to enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(mgr->events_polled_for_main_loop);

	// declare and zero out table path
	char *table_path[10];
	BP_ZERO_STACK(table_path);

	// set table path entries
	table_path[0] = "events";
	table_path[1] = event_name;

	// attempt lookup by key name
	P_BP_HASH_TABLE_KEYED_ENTRY entry_lookup = BP_HashRegLookupTableEntryByPath(mgr->events_polled_for_main_loop, (char **) &table_path);
	if(!entry_lookup)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(mgr->events_polled_for_main_loop, NULL);


	// optional debug display of the manager
	printf("\n [+] Entry Lookup Table Here?: %p - %s", entry_lookup, entry_lookup->key);
	BP_HashRegDisplay(mgr->events_polled_for_main_loop, BP_FALSE, BP_TRUE);



	// set event lookup
	P_BP_EVENT event_lookup = (P_BP_EVENT) entry_lookup->data;

	printf("\n Got Event Lookup During Pickup: %p - %s", event_lookup, event_lookup->name);

	// destroy hash registry entry here (frees memory)
	BP_HashRegDestroyEntry(mgr->events_polled_for_main_loop, entry_lookup);

	// return the looked up event
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(mgr->events_polled_for_main_loop, event_lookup);


}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Add Event Message Processors %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// add processor to the event manager (adds to processor registry)
BP_ERROR_T BP_EventMgrAddProcessor
(
		P_BP_EVENT_MANAGER mgr,
		char *processor_name,
		char *processor_description,
		BP_EVENT_PROCESSOR_COMPUTE_TARGET_TYPE compute_target_type,
		BP_EVENT_PROCESSOR_FUNC_T processor_entry_point
)
{

	// ensure we have a correct working environment within the event manager
	BP_EVENTMGR_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	// ensure we have a manager
	if(!mgr)
		return ERR_FAILURE;

	// ensure we have a name and description
	if(!processor_name)
		return ERR_FAILURE;
	if(!processor_description)
		return ERR_FAILURE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create New Processor Entry %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create a table path on the stack for ease of use
	char * table_path_actual[10] = {0};

	// reference to the local stack
	BP_HASH_TABLE_TRAVERSAL_PATH table_path = (char **) &table_path_actual;

	// add table
	table_path[0] = "system";
	table_path[1] = "event_managment";
	table_path[2] = mgr->name;
	table_path[3] = "processors";
	table_path[4] = processor_name;
	table_path[5] = NULL;

	// add the table path
	BP_HashRegAddTableByPath(NULL,table_path);

	// attempt to lookup the table by path
	P_BP_HASH_TABLE_KEYED_ENTRY processor_name_lookup = BP_HashRegLookupTableByPath(NULL, table_path);

	// ensure we have a processor name lookup
	if(!processor_name_lookup)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create New Worker %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create table path for adding element
	table_path[0] = "system";
	table_path[1] = "event_managment";
	table_path[2] = mgr->name;
	table_path[3] = "processors";
	table_path[4] = processor_name;
	table_path[5] = NULL;

	// attempt to create new managed worker structure
	BP_ERROR_T success_fail = BP_HashRegAddDataToTableByPath
	(
		NULL,
		table_path,
		"entry",
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_EVENT_PROCESSOR)
	);

	// ensure addition was successful
	if(success_fail == ERR_FAILURE)
		return ERR_FAILURE;

	// create table path for lookup
	table_path[0] = "system";
	table_path[1] = "event_managment";
	table_path[2] = mgr->name;
	table_path[3] = "processors";
	table_path[4] = processor_name;
	table_path[5] = "entry";
	table_path[6] = NULL;

	// attempt to lookup the table by path
	P_BP_HASH_TABLE_KEYED_ENTRY data_name_lookup = BP_HashRegLookupTableEntryByPath(NULL, table_path);

	// ensure we have a data lookup
	if(!data_name_lookup)
		return ERR_FAILURE;

	// dereference data
	P_BP_EVENT_PROCESSOR new_processor = (P_BP_EVENT_PROCESSOR) data_name_lookup->data;

	// ensure we have a new processor
	if(!new_processor)
		return ERR_FAILURE;


	/*
	P_BP_EVENT_MANAGER mgr,
		char *processor_name,
		char *processor_description,
		BP_EVENT_PROCESSOR_COMPUTE_TARGET_TYPE compute_target_type,
		BP_EVENT_PROCESSOR_CONSUMER_FUNC_T consumer,
		BP_EVENT_PROCESSOR_PRODUCER_FUNC_T producer,
		size_t max_messages_in,
		size_t max_messages_out
	*/

	// set new uuid, name, and description
	new_processor->uuid        = NEW_RANDOM_UUID;
	new_processor->name        = bpstrdup(processor_name);
	new_processor->description = bpstrdup(processor_description);

	// consumer and producer
	new_processor->entry_point = processor_entry_point;

	// set compute target type
	new_processor->compute_target_type = compute_target_type;


	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Debug/Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// display an event manager
BP_ERROR_T BP_EventMgrDisplay
(
		P_BP_EVENT_MANAGER mgr,
		BP_BOOL display_processor_reg,
		BP_BOOL display_events_reg
)
{

	// ensure we have a correct working environment within the event manager
	BP_EVENTMGR_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	// ensure we have a manager to display
	if(!mgr)
		return ERR_FAILURE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Event Manager to Stdout %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// display header
	printf("\n [+] Displaying Event Manager %p", mgr);

	printf("\n\t lock: %p", mgr->lock);
	printf("\n\t event_mgr_lock: %p", mgr->event_mgr_lock);
	printf("\n\t thread_manager: %p", mgr->thread_manager);

	// display string settings
	printf("\n\t uuid: %p", mgr->uuid);
	printf("\n\t name: %p", mgr->name);
	printf("\n\t description: %s", mgr->description);

	// event loop stat will require custom function
	// printf("\n\t event_loop_state:        %p", mgr->event_loop_state);


	printf("\n\t events_in_acquire_lock:  %p", mgr->events_in_acquire_lock);
	printf("\n\t events_out_acquire_lock: %p", mgr->events_out_acquire_lock);
	printf("\n\t events_in_total:         %u", mgr->events_in_total);
	printf("\n\t events_out_total:        %u", mgr->events_out_total);

	// event counts will require custom display functions because
	// they are structures.
	// printf("\n\t events_in_counts:        %u", mgr->events_in_counts);
	// printf("\n\t events_out_counts:       %u", mgr->events_out_counts);


	// return indicating success
	return ERR_SUCCESS;

}


// display an event managers processor registry
BP_ERROR_T BP_EventMgrDisplayProcessorRegistry(P_BP_EVENT_MANAGER mgr, BP_BOOL display_registry_entries)
{

	// ensure we have a correct working environment within the event manager
	BP_EVENTMGR_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	// ensure we have a manager
	if(!mgr)
		return ERR_FAILURE;


	// return indicating success
	return ERR_SUCCESS;

}

// Display an event processor.  Requires a valid manager to use as a reference
// point when displaying data.
BP_ERROR_T BP_EventMgrDisplayProcessor(P_BP_EVENT_MANAGER mgr, P_BP_EVENT_PROCESSOR processor)
{

	// ensure we have a correct working environment within the event manager
	BP_EVENTMGR_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	if(!mgr)
		return ERR_FAILURE;
	if(!processor)
		return ERR_FAILURE;

	printf("\n\t [+] Event Processor %p");
	printf("\n\t     lock:                %p", &processor->lock);
	printf("\n\t     uuid:                %s", processor->uuid);
	printf("\n\t     name:                %s", processor->name);
	printf("\n\t     description:         %s", processor->description);
	printf("\n\t     compute_target_type: ");

	switch(processor->compute_target_type)
	{
		case BP_EVENT_PROCESSOR_COMPUTE_TARGET_TYPE_CPU_THREAD:
			printf("BP_EVENT_PROCESSOR_COMPUTE_TARGET_TYPE_CPU_THREAD");
			break;
		case BP_EVENT_PROCESSOR_COMPUTE_TARGET_TYPE_GPU_THREAD:
			printf("BP_EVENT_PROCESSOR_COMPUTE_TARGET_TYPE_GPU_THREAD");
			break;
		default:
			break;
	}


	printf("\n");


	// return indicating success
	return ERR_SUCCESS;

}


// Displays weight distribution information
BP_ERROR_T BP_EventMgrDisplayWorkerWeightDistributionInfo(P_BP_EVENT_MANAGER mgr)
{

	// ensure we have a correct working environment within the event manager
	BP_EVENTMGR_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	// ensure we have a manager
	if(!mgr)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}


// Displays the worker registry in the event manager.
BP_ERROR_T BP_EventMgrDisplayWorkerRegistry(P_BP_EVENT_MANAGER mgr, BP_BOOL display_registry_entries)
{

	// ensure we have a correct working environment within the event manager
	BP_EVENTMGR_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	if(!mgr)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;
}

// Display a single worker entry
BP_ERROR_T BP_EventMgrDisplayWorker(P_BP_EVENT_MANAGER mgr, P_BP_EVENT_WORKER worker)
{

	// ensure we have a correct working environment within the event manager
	BP_EVENTMGR_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	// ensure we have a manager and a worker
	if(!mgr)
		return ERR_FAILURE;
	if(!worker)
		return ERR_FAILURE;

	// display the worker
	printf("\n\t [+] Worker %p", worker);
	printf("\n\t\t lock:                   %p", &worker->lock);
	printf("\n\t\t total_events_processed: %u", worker->total_events_processed_n);

	// return indicating success
	return ERR_SUCCESS;

}

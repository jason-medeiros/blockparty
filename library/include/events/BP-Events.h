/*
 * BP-Events.h
 *
 *  Created on: Nov 30, 2014
 *      Author: root
 */

#ifndef BP_EVENTS_H_
#define BP_EVENTS_H_



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Event Semaphore Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//! initializes (creates) a new semaphore
#define BP_EVENTS_SEM_INIT(sem_to_lock) sem_to_lock = BP_SemaphoreInitDefault()

//! locks a semaphore
#define BP_EVENTS_SEM_LOCK(sem_to_lock) BP_SemaphoreWait(sem_to_lock) ? ERR_FAILURE : ERR_SUCCESS

//! tries to lock a semaphore (non-blocking)
#define BP_EVENTS_SEM_TRYLOCK(sem_to_lock) BP_SemaphoreTryWait(sem_to_lock) ? ERR_FAILURE : ERR_SUCCESS

//! unlocks a semaphore
#define BP_EVENTS_SEM_UNLOCK(sem_to_unlock) BP_SemaphorePost(sem_to_unlock) ? ERR_FAILURE : ERR_SUCCESS

//! destroys a semaphore (needs to be implemented)
#define BP_EVENTS_DESTROY_SEM(sem_to_destroy) BP_SemaphoreDestroy(sem_to_destroy)

//! Returns zero if it's unlocked (or error), return 1 if it's locked.
#define BP_EVENTS_SEM_IS_LOCKED(sem_to_check) BP_SemGetValue(sem_to_check) ? ERR_SUCCESS : ERR_FAILURE



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Computer Target Types %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//! computational target type (cpu, gpu)
typedef enum BP_EVENT_PROCESSOR_COMPUTE_TARGET_TYPE
{
	BP_EVENT_PROCESSOR_COMPUTE_TARGET_TYPE_UNSET,
	BP_EVENT_PROCESSOR_COMPUTE_TARGET_TYPE_CPU_THREAD,
	BP_EVENT_PROCESSOR_COMPUTE_TARGET_TYPE_GPU_THREAD
};


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Simple Type Definitions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//! Message ID type
typedef size_t BP_EVENT_MESSAGE_ID_T;

//! Processor ID type
typedef size_t BP_EVENT_PROCESSOR_ID_T;

//! specifies which (generic) event message stack to manipulate messages from
typedef enum BP_EVENT_MESSAGE_IN_OUT
{
	BP_EVENT_MESSAGES_UNSET,
	BP_EVENT_MESSAGES_IN,
	BP_EVENT_MESSAGES_OUT
};


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Event Loop Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//! open event loop macro
#define BP_EVENT_LOOP while(1)

//! open worker loop macro
#define BP_EVENT_WORKER_THREAD_LOOP while(1)


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Function Pointer Typedefs and Macros %%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//! consumer function pointer type definition
typedef void * (*BP_EVENT_PROCESSOR_FUNC_T)(void *);

//! simple definition for a new event processor function
#define BP_EVENT_PROCESSOR_FUNC(f_name) void * f_name(void *data)


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Processor Route Function Macros %%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//! Processor route function exit macro.q
#define BP_EVENT_PROCESSOR_EXIT \
	{\
		BP_EVENTS_SEM_UNLOCK(event->lock); \
		return (void *) event;\
	}

//! Processor route function entry macro.
#define BP_EVENT_PROCESSOR_ENTRY                                                              \
	                                                                                          \
	P_BP_EVENT event = (P_BP_EVENT) data;                                                     \
	                                                                                          \
	if(!event)                                                                                \
	     return NULL;                                                                         \
	                                                                                          \
	BP_EVENTS_SEM_LOCK(event->lock);                                                          \
	if(!event->event_data)                                                                    \
	{                                                                                         \
		BP_EVENT_PROCESSOR_EXIT;                                                              \
	}                                                                                         \
	                                                                                          \
	P_BP_JS_EVENT_DATA js_event_data   = (P_BP_JS_EVENT_DATA) event->event_data;              \
	char *             javascript_code = js_event_data->javascript_code;                      \
	P_BP_EVENT_WORKER  worker          = (P_BP_EVENT_WORKER) event->worker;                   \
	P_BP_JS_MANAGER    js_mgr          = NULL;                                                \
	                                                                                          \
	if(event->type == BP_EVENT_TYPE_MARKER_SYSTEM_JS_EVENT)                                   \
	{                                                                                         \
	                                                                                          \
		if(!worker->system_js_processor)                                                      \
		{                                                                                     \
	                js_mgr = BP_JSCreateNewManager                                            \
                	(                                                                         \
        	                worker->name,                                                     \
	                        "Worker thread System Javascript Manager.",                       \
                        	BP_JS_CONTEXT_SYSTEM,                                             \
                	        BP_JS_PRIVILEGE_LEVEL_ADMIN,                                      \
        	                NULL                                                              \
	                );                                                                        \
	                worker->system_js_processor = js_mgr;                                     \
	                                                                                          \
		}                                                                                     \
		else                                                                                  \
		{                                                                                     \
			js_mgr = (P_BP_JS_MANAGER) worker->system_js_processor;                           \
		}                                                                                     \
	}                                                                                         \
	else if(event->type == BP_EVENT_TYPE_MARKER_DOM_JS_EVENT)                                 \
	{                                                                                         \
		                                                                                      \
        if(!worker->dom_js_processor)                                                         \
        {                                                                                     \
        	                                                                                  \
                   js_mgr = BP_JSCreateNewManager                                             \
                   (                                                                          \
                           worker->name,                                                      \
                           "Worker thread DOM Javascript Manager.",                           \
                           BP_JS_CONTEXT_SYSTEM,                                              \
                           BP_JS_PRIVILEGE_LEVEL_ADMIN,                                       \
                           NULL                                                               \
                    );                                                                        \
                     worker->dom_js_processor = js_mgr;                                       \
                }                                                                            \
                else                                                                         \
                {                                                                           \
                        js_mgr = (P_BP_JS_MANAGER) worker->dom_js_processor;               \
                }                                                                           \
				                                                                        \
	}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Event Messages %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


//! CPU route difficulty enumerator.
typedef enum BP_EVENT_PROCESSOR_CPU_ROUTE_DIFFICULTY
{
	BP_EVENT_PROCESSOR_CPU_ROUTE_DIFFICULTY_NONE,
	BP_EVENT_PROCESSOR_CPU_ROUTE_DIFFICULTY_ALMOST_NO_COST,
	BP_EVENT_PROCESSOR_CPU_ROUTE_DIFFICULTY_VERY_LOW_COST,
	BP_EVENT_PROCESSOR_CPU_ROUTE_DIFFICULTY_LOW_COST,
	BP_EVENT_PROCESSOR_CPU_ROUTE_DIFFICULTY_MID_LOW_COST,
	BP_EVENT_PROCESSOR_CPU_ROUTE_DIFFICULTY_MID_COST,
	BP_EVENT_PROCESSOR_CPU_ROUTE_DIFFICULTY_MID_HIGH_COST,
	BP_EVENT_PROCESSOR_CPU_ROUTE_DIFFICULTY_HIGH_COST,
	BP_EVENT_PROCESSOR_CPU_ROUTE_DIFFICULTY_VERY_HIGH_COST,
	BP_EVENT_PROCESSOR_CPU_ROUTE_DIFFICULTY_NEAR_FULL_COST
};

//! GPU route difficulty enumerator.
typedef enum BP_EVENT_PROCESSOR_GPU_ROUTE_DIFFICULTY
{
	BP_EVENT_PROCESSOR_GPU_ROUTE_DIFFICULTY_NONE,
	BP_EVENT_PROCESSOR_GPU_ROUTE_DIFFICULTY_ALMOST_NO_COST,
	BP_EVENT_PROCESSOR_GPU_ROUTE_DIFFICULTY_VERY_LOW_COST,
	BP_EVENT_PROCESSOR_GPU_ROUTE_DIFFICULTY_LOW_COST,
	BP_EVENT_PROCESSOR_GPU_ROUTE_DIFFICULTY_MID_LOW_COST,
	BP_EVENT_PROCESSOR_GPU_ROUTE_DIFFICULTY_MID_COST,
	BP_EVENT_PROCESSOR_GPU_ROUTE_DIFFICULTY_MID_HIGH_COST,
	BP_EVENT_PROCESSOR_GPU_ROUTE_DIFFICULTY_HIGH_COST,
	BP_EVENT_PROCESSOR_GPU_ROUTE_DIFFICULTY_VERY_HIGH_COST,
	BP_EVENT_PROCESSOR_GPU_ROUTE_DIFFICULTY_NEAR_FULL_COST
};


//! Event status enumerator.
typedef enum BP_EVENT_STATUS {
	BP_EVENT_STATUS_NONE,
	BP_EVENT_STATUS_INITIALIZED_AWAITING_PROCESSING,
	BP_EVENT_STATUS_CONSUMER_PROCESSING,
	BP_EVENT_STATUS_CONSUMER_EXITED_NORMALLY,
	BP_EVENT_STATUS_CONSUMER_EXITED_ERROR,
	BP_EVENT_STATUS_PRODUCER_PROCESSING,
	BP_EVENT_STATUS_PRODUCER_EXITED_NORMALLY,
	BP_EVENT_STATUS_PRODUCER_EXITED_ERROR
};

//! Event message status enumerator.
typedef enum BP_EVENT_MESSAGE_STATUS {
	BP_EVENT_MESSAGE_STATUS_NONE,
	BP_EVENT_STATUS_WAITING_FOR_DELIVERY,
	BP_EVENT_STATUS_READY_FOR_PICKUP
};

//! Event message priority enumerator.
typedef enum BP_EVENT_PRIORITY {
	BP_EVENT_PRIORITY_NONE,
	BP_EVENT_PRIORITY_VERY_LOW,
	BP_EVENT_PRIORITY_LOW,
	BP_EVENT_PRIORITY_MID_NORMAL,
	BP_EVENT_PRIORITY_NORMAL,
	BP_EVENT_PRIORITY_MID_HIGH,
	BP_EVENT_PRIORITY_HIGH,
	BP_EVENT_PRIORITY_SYSTEM,
	BP_EVENT_PRIORITY_CRITICAL_SYSTEM
};





// event processor
typedef struct __P_BP_EVENT_PROCESSOR {

	//! semaphore lock
	P_BP_SEMAPHORE_INFO lock;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Identifiers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	//! uuid
	char *uuid;

	//! name
	char *name;

	//! description
	char *description;

	//! compute target type
	BP_EVENT_PROCESSOR_COMPUTE_TARGET_TYPE compute_target_type;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Callback %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	//! consumer function
	BP_EVENT_PROCESSOR_FUNC_T entry_point;


} BP_EVENT_PROCESSOR, *P_BP_EVENT_PROCESSOR;


// state of the dispatch
typedef enum BP_EVENT_DISPATCH_STATE
{
	BP_EVENT_DISPATCH_STATE_UNSET,
	BP_EVENT_DISPATCH_STATE_WAITING_FOR_DISPATCH,
	BP_EVENT_DISPATCH_STATE_WAITING_FOR_OUTPUT_TRANSFORM,
	BP_EVENT_DISPATCH_STATE_COMPLETED
};

// Type of event being operated on.  The type determines
// which JIT actions will be performed.  JS events require
// the worker thread to have an active javascript manager
// session up for executing JS type events.  Generic events
// on the other hand do not run this preliminary check.
typedef enum BP_EVENT_TYPE_MARKER
{
	BP_EVENT_TYPE_MARKER_UNSET,
	BP_EVENT_TYPE_MARKER_GENERIC_EVENT,
	BP_EVENT_TYPE_MARKER_SYSTEM_CORE_EVENT,
	BP_EVENT_TYPE_MARKER_SYSTEM_JS_EVENT,
	BP_EVENT_TYPE_MARKER_DOM_JS_EVENT
};

//! Blockparty event structure.
typedef struct __BP_EVENT {


	//! event semapohre lock
	P_BP_SEMAPHORE_INFO lock;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Type Marker %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This marker designates what type of event this is.  Types are
	// enumerated in the BP_EVENT_TYPE_MARKER enum above.
	BP_EVENT_TYPE_MARKER type;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Human Readable Markers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	//! name of this event
	char * name;

	//! description of this event
	char * description;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Processor and Route Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	//! processor to use.  This is casted to an event processor, so we
	//! don't have to forward declare anything.
	P_BP_EVENT_PROCESSOR processor;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Output Processor %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	//! An optional output transformation processor
	P_BP_EVENT_PROCESSOR ot_processor;




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Event Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// event data to pass to event processor
	void * event_data;

	// event data size being passed
	size_t event_data_size;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Dispatch Info %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// state of the dispatched event
	BP_EVENT_DISPATCH_STATE dispatch_state;

	// Void casted worker pointer.
	void * worker; /* P_BP_EVENT_WORKER */

	// id of the thread processing this event
	pthread_t processed_thread_id;

	// id of the thread that did the output transformation for this event (usually same as processed
	// thread id)
	pthread_t ot_processed_thread_id;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Priority/Status Markers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	//! current status of the event
	BP_EVENT_STATUS status;

	//! event priority
	BP_EVENT_PRIORITY priority;



} BP_EVENT, *P_BP_EVENT;






// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Event Worker Registry %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//! Event worker thread
typedef struct __BP_EVENT_WORKER {


	// worker lock
	P_BP_SEMAPHORE_INFO lock;

	// reverse lookup pointer to the event manager associated with this
	// event worker (null if unset).  Must be cast to (P_BP_EVENT_MANAGER)
	void * event_manager;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Thread Info %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// the pthread id of the parent thread
	pthread_t parent_thread_id;

	// the pthread id of the running thread
	pthread_t worker_thread_id;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Javascript Processors %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// system js processor associated with this worker thread
	void * system_js_processor; /*P_BP_JS_MANAGER*/

	// dom js processor associated with this worker thread
	void * dom_js_processor; /*P_BP_JS_MANAGER*/


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Worker Descriptors %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	//! name of this worker thread
	char *name;

	//! description of this worker thread
	char *description;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Worker Hash Registry Pointer %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// worker hash registry
	P_BP_HASH_TABLE_REGISTRY worker_hreg;

	// javascript persistent hash registry
	P_BP_HASH_TABLE_REGISTRY js_persistent_hreg;

	// output transformation hash registry (used for creating output streams
	// between events and output mechanisms)
	P_BP_HASH_TABLE_REGISTRY output_transformation_hreg;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Number of Events Writen %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	//! total number of events processed by this worker
	size_t total_events_processed_n;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Flags %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if the worker exited, this is set
	BP_BOOL exited;




} BP_EVENT_WORKER, *P_BP_EVENT_WORKER;


//! weight distribution entry
typedef struct __BP_EVENT_WORKER_WEIGHT_DISTRIBUTION_ENTRY {

	// this is the worker itself
	P_BP_EVENT_WORKER worker;

	// work weight on the cpu
	size_t work_weight_cpu;

	// work weight on the gpu
	size_t work_weight_gpu;

} BP_EVENT_WORKER_WEIGHT_DISTRIBUTION_ENTRY, *P_BP_EVENT_WORKER_WEIGHT_DISTRIBUTION_ENTRY;


//! set the worker weight distribution
typedef struct __BP_EVENT_WORKER_WEIGHT_DISTRIBUTION {


	//! this is the total work weight each worker currently has
	size_t            total_work_weight_cpu;

	//! this is the total work weight each gpu worker currently has
	size_t            total_work_weight_gpu;

	//! total number of events processed (since start of event manager)
	size_t            total_events_processed;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Quick Reference Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	//! lightest worker available (least cpu work in terms of event messages)
	P_BP_EVENT_WORKER_WEIGHT_DISTRIBUTION_ENTRY lightest_worker_cpu;
	//! lightest worker available (least cpu work in terms of event messages with open slots)
	P_BP_EVENT_WORKER_WEIGHT_DISTRIBUTION_ENTRY lightest_worker_cpu_with_available_message_slots;

	//! heaviest worker available (most cpu work in terms of event messages)
	P_BP_EVENT_WORKER_WEIGHT_DISTRIBUTION_ENTRY heaviest_worker_cpu;
	//! heaviest worker available (most cpu work in terms of event messages) with open slots
	P_BP_EVENT_WORKER_WEIGHT_DISTRIBUTION_ENTRY heaviest_worker_cpu_with_available_message_slots;

	//! lightest worker available (least gpu work in terms of event messages)
	P_BP_EVENT_WORKER_WEIGHT_DISTRIBUTION_ENTRY lightest_worker_gpu;
	//! lightest worker available (least gpu work in terms of event messages) with open slots.
	P_BP_EVENT_WORKER_WEIGHT_DISTRIBUTION_ENTRY lightest_worker_gpu_with_available_message_slots;

	//! heaviest worker available (most gpu work in terms of event messages)
	P_BP_EVENT_WORKER_WEIGHT_DISTRIBUTION_ENTRY heaviest_worker_gpu;
	//! heaviest worker available (most gpu work in terms of event messages) with available slots
	P_BP_EVENT_WORKER_WEIGHT_DISTRIBUTION_ENTRY heaviest_worker_gpu_with_available_message_slots;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Weight Distribution Entries %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	//! weight distribution entries
	P_BP_EVENT_WORKER_WEIGHT_DISTRIBUTION_ENTRY entries;

	//! number of entries in the table
	size_t                                       entries_n;


} BP_EVENT_WORKER_WEIGHT_DISTRIBUTION, *P_BP_EVENT_WORKER_WEIGHT_DISTRIBUTION;




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Event Loop State %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//! Event loop state.
typedef enum BP_EVENT_LOOP_STATE_STATUS
{
	BP_EVENT_LOOP_STATE_STATUS_NONE,
	BP_EVENT_LOOP_STATE_MANAGER_CREATED_LOOP_UNSTARTED,
	BP_EVENT_LOOP_STATE_STATUS_AVAILABLE,
	BP_EVENT_LOOP_STATE_STATUS_WAITING_FOR_EVENTS,
	BP_EVENT_LOOP_STATE_STATUS_READING_EVENTS,
	BP_EVENT_LOOP_STATE_STATUS_ADDING_EVENTS,
	BP_EVENT_LOOP_STATE_STATUS_MOVING_EVENTS,
	BP_EVENT_LOOP_STATE_STATUS_ACTIVATING_EVENT_ROUTES,
	BP_EVENT_LOOP_STATE_STATUS_SHUTTING_DOWN

};


//! This structure exists within the event manager to synchronize event
//! handling.
typedef struct __BP_EVENT_LOOP_STATE {

	//! event loop structure lock
	P_BP_SEMAPHORE_INFO lock;

	//! status of the event loop
	BP_EVENT_LOOP_STATE_STATUS status;

} BP_EVENT_LOOP_STATE, *P_BP_EVENT_LOOP_STATE;

//! The event queue counts allow us to monitor the count of events of
//! differing priorities.
typedef struct __BP_EVENT_COUNTS {

	// BP_EVENT_PRIORITY_VERY_LOW
	size_t very_low;

	// BP_EVENT_PRIORITY_LOW
	size_t low;

	// BP_EVENT_PRIORITY_MID_NORMAL
	size_t mid_normal;

	// BP_EVENT_PRIORITY_NORMAL
	size_t normal;

	// BP_EVENT_PRIORITY_MID_HIGH
	size_t mid_high;

	// BP_EVENT_PRIORITY_HIGH
	size_t high;

	// BP_EVENT_PRIORITY_SYSTEM
	size_t system;

	// BP_EVENT_PRIORITY_CRITICAL_SYSTEM
	size_t critical_system;

} BP_EVENT_COUNTS, *P_BP_EVENT_COUNTS;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Event Manager %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//! Main event manager structure, contains everything required for the event system to work.  This system was
//! changed from normal arrays to hash tables in 2015.
typedef struct __BP_EVENT_MANAGER {


	//! tailqueue allocator used for the event manager
	P_BP_LINKL tq;

	//! event manager semaphore lock
	P_BP_SEMAPHORE_INFO lock;

	//! event manager lock
	P_BP_SEMAPHORE_INFO event_mgr_lock;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Thread Manager for Workers %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Blockparty native thread manager used for event worker threads.
	P_BP_THREAD_MANAGER thread_manager;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Event Manager Descriptors %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	//! uuid associated with this event manager (created automatically)
	char * uuid;

	//! name of this event manager (can be left unset)
	char * name;

	//! description of this event manager (can be left unset)
	char * description;

	// -- loop state --

	//! keeps track of events within an event loop
	BP_EVENT_LOOP_STATE event_loop_state;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Event Queue Aquisition Locks %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// lock used for acquiring events from the in pool
	P_BP_SEMAPHORE_INFO events_in_acquire_lock;

	// lock used for acquiring events from the out pool
	P_BP_SEMAPHORE_INFO events_out_acquire_lock;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Main Loop Event Poll Queue %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This is a hash table registry which is populated when BP_EventLoopPollForEventsAndUnlockForWorkers is
	// called.  When it's called it pulls data out of the events_out table and stores it in the ht:/events/ path
	// in order of highest priority to lowest priority.  In the main loop, the output processors can then be called
	// on each of the events stored in this new hash table.

	// events polled for main loop
	P_BP_HASH_TABLE_REGISTRY events_polled_for_main_loop;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Internal Counters %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// total number of events in the event out queue
	size_t events_in_total;

	// total number of events in the events out queue
	size_t events_out_total;


	// counts for in events
	BP_EVENT_COUNTS events_in_counts;

	// counts for out events
	BP_EVENT_COUNTS events_out_counts;




} BP_EVENT_MANAGER, *P_BP_EVENT_MANAGER;




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Quality of Life Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//! Macro to validate the we have a correct working environment within the
//! global hash registry.
#define BP_EVENTMGR_VALIDATE_WORKING_ENVIRONMENT(ret) \
	if(BP_EventMgrEnsureWorkingGlobalHashTableEnvirionment() == ERR_FAILURE) \
	{\
		printf("\n [!!] Error: Could not validate event manager working environment within global hash table.  Critical error.");\
		return ret;\
	}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Global Hash Registry Related Routines %%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//! This is called to ensure that the /system/event_managment/ table has
//! been setup correctly.  It simply searches for an existing semaphore
//! table and if it doesn't exist, adds it to /system/.
BP_ERROR_T BP_EventMgrEnsureWorkingGlobalHashTableEnvirionment();


//! will search the event manager to determine if an event manager exists with the given name.
P_BP_HASH_TABLE_KEYED_ENTRY BP_EventMgrLookupManagerKeyedEntryByNameInGHT(char *mgr_name);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Lookup Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//! simple routine to retrive an event manager by it's name
P_BP_EVENT_MANAGER BP_EventMgrLookupManagerByName(char *mgr_name);

//! Attempts to lookup the keyed entry table for the event workers threads.
P_BP_HASH_TABLE_KEYED_ENTRY BP_EventMgrLookupWorkersTable(P_BP_EVENT_MANAGER event_manager);

//! Lookup an event processor by name.
P_BP_EVENT_PROCESSOR BP_EventMgrLookupProcessorByName(P_BP_EVENT_MANAGER event_manager, char *processor_name);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Event Manager Create / Destroy %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Create a new event manager
P_BP_EVENT_MANAGER BP_EventMgrCreate(char * mgr_name, char *mgr_description);

// Destroy an event manager.
BP_ERROR_T BP_EventMgrDestroy(P_BP_EVENT_MANAGER event_manager);





// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Event Worker Threads %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// data passed into the event worker thread
typedef struct __BP_EVENT_WORKER_THREAD_DATA {


	// worker information associated with one worker
	P_BP_EVENT_WORKER worker;

} BP_EVENT_WORKER_THREAD_DATA, *P_BP_EVENT_WORKER_THREAD_DATA;


// exit the worker thread
#define BP_EVENT_WORKER_EXIT_THREAD                                                 \
	{                                                                               \
		                                                                            \
		BP_MANAGED_THREAD_LOCK;                                                     \
		if(local_worker)                                                            \
			local_worker->exited = BP_TRUE;                                         \
		managed_thread->current_status = BP_THREAD_STATUS_EXITED;                   \
		BP_MANAGED_THREAD_UNLOCK;                                                   \
		return NULL;                                                                     \
	}

// enter the worker thread
#define BP_EVENT_WORKER_ENTER_THREAD                                                               \
	                                                                                               \
	P_BP_MANAGED_THREAD managed_thread = (P_BP_MANAGED_THREAD) data;                               \
	                                                                                               \
	BP_MANAGED_THREAD_LOCK;                                                                        \
	managed_thread->current_status = BP_THREAD_STATUS_RUNNING;                                     \
	BP_MANAGED_THREAD_UNLOCK;                                                                      \
	                                                                                               \
	BP_EVENTMGR_VALIDATE_WORKING_ENVIRONMENT(NULL);                                                \
	P_BP_EVENT_WORKER local_worker = (P_BP_EVENT_WORKER)  managed_thread->thread_parameter;        \
	if(!local_worker)                                                                              \
	{                                                                                              \
		BP_EVENT_WORKER_EXIT_THREAD;                                                               \
	}                                                                                              \
	                                                                                               \
	P_BP_EVENT_MANAGER evm         = (P_BP_EVENT_MANAGER) local_worker->event_manager;             \
	                                                                                               \
	if(!evm)                                                                                       \
	{                                                                                              \
		BP_EVENT_WORKER_EXIT_THREAD;                                                               \
	}                                                                                              \
	if(!evm->name)                                                                                 \
	{                                                                                              \
		BP_EVENT_WORKER_EXIT_THREAD;                                                               \
	}                                                                                              \
	if(!BP_StringIsReasonablePrintableString(evm->name, bpstrlen(evm->name), BP_FALSE, BP_FALSE))    \
	{                                                                                              \
		BP_EVENT_WORKER_EXIT_THREAD;                                                               \
	}                                                                                              \
	local_worker->worker_thread_id = pthread_self();



// event worker thread
void * BP_EventMgrWorkerThread_thInit(void *data);

// add an event worker thread
BP_ERROR_T BP_EventMgrAddWorker
(
		P_BP_EVENT_MANAGER mgr,
		char * worker_name,
		char * worker_description
);

// Start all unstarted workers
BP_ERROR_T BP_EventMgrStartWorkers(P_BP_EVENT_MANAGER mgr);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Main Loop Event Poll Queue Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Initializes the mgr->events_polled_for_main_loop hash registry with an empty/correct structure.
BP_ERROR_T BP_EventMgrEventsPolledHashRegInit(P_BP_EVENT_MANAGER mgr);

// add event to the poll hash table
BP_ERROR_T BP_EventManagerEventsPolledAddEvent(P_BP_EVENT_MANAGER mgr, P_BP_EVENT event);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Event Pickup and Checkin %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// -----------------------------------------------------
// --- Pickup Events From A Worker Thread --------------
// -----------------------------------------------------

// These routines are primarily utilized for worker threads, in which threads will lock the queue
// ask one of these routines for an event, and capture them for processor dispatch.

// Pickup specific event from the current worker thread.
P_BP_EVENT BP_EventMgrPickupEventFromWorker
(
		P_BP_EVENT_WORKER        worker,
		BP_EVENT_MESSAGE_IN_OUT  pickup_from_queue,
		BP_EVENT_PRIORITY        event_priority
);

// Pickup the highest priority event from the current worker thread
P_BP_EVENT BP_EventMgrPickupHighestPriorityEventFromWorker
(
		P_BP_EVENT_WORKER worker,
		BP_EVENT_MESSAGE_IN_OUT pickup_from_queue
);

// Pickup the lowest priority event from the current worker thread
P_BP_EVENT BP_EventMgrPickupLowestPriorityEventFromWorker
(
		P_BP_EVENT_WORKER worker,
		BP_EVENT_MESSAGE_IN_OUT pickup_from_queue
);


// -----------------------------------------------------
// --- Pickup Events From Main Event Loop --------------
// -----------------------------------------------------

// These routines are primarily used for event completion polling in
// the main thread.  When events are done processing, they are moved into the:
//
// GHT:/system/event_managment/mgr_name_here/events_out/priority_level_here/
//
// path, with mgr_name_here and priority_level_here substituted respectfully
// in the path with whatever values are relevant to your implementation.  These
// routines pull those events out of the events_out table and return them to the
// main event loop for further processing and dispatch.

// Pickup event from main loop.
P_BP_EVENT BP_EventMgrPickupEventFromMainLoop
(
		P_BP_EVENT_MANAGER      mgr,
		BP_EVENT_MESSAGE_IN_OUT pickup_from_queue,
		BP_EVENT_PRIORITY       event_priority
);

// Pickup the highest priority event from main loop.
P_BP_EVENT BP_EventMgrPickupHighestPriorityEventFromMainLoop
(
		P_BP_EVENT_MANAGER      mgr,
		BP_EVENT_MESSAGE_IN_OUT pickup_from_queue
);

// Pickup the lowest priority event from the current worker thread
P_BP_EVENT BP_EventMgrPickupLowestPriorityEventFromMainLoop
(
		P_BP_EVENT_MANAGER      mgr,
		BP_EVENT_MESSAGE_IN_OUT pickup_from_queue
);



// -----------------------------------------------------
// --- Checkins ----------------------------------------
// -----------------------------------------------------

// This takes an existing event and attempts to check it into a queue (works similarly to BP_EventMgrCreateEvent).
BP_ERROR_T BP_EventMgrCheckinProcessedEvent
(
	P_BP_EVENT_WORKER worker,
	P_BP_EVENT event_to_checkin,
	BP_EVENT_MESSAGE_IN_OUT checkin_to_queue,
	BP_EVENT_PRIORITY event_priority
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Event Creation %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

typedef enum BP_EVENT_QUEUE_SELECTOR
{
	BP_EVENT_QUEUE_SELECTOR_UNSET,
	BP_EVENT_QUEUE_SELECTOR_OUT,
	BP_EVENT_QUEUE_SELECTOR_IN
};

// Create an event.
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
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Event Loop Poll %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// This routine is to be used in the main event loop.  When it is called, it examines the events_in
// table and determines whether or not there are events to be processed.  It then locks the event manager
// so main thread operations can take place.
BP_ERROR_T BP_EventLoopPollForEventsAndLockForMainLoop(P_BP_EVENT_MANAGER mgr);

// This should be called at the foot of the event loop after  BP_EventLoopPollForEventsAndLockForMainLoop has been called.
BP_ERROR_T BP_EventLoopUnlockForWorkers(P_BP_EVENT_MANAGER mgr);



// Attempts to pickup an event (removing it from the registry) by name.  Once the event is picked up,
// the pickup routine must destroy the event itself using it's own destructors.  It will no longer be
// tracked by the event system.
P_BP_EVENT BP_EventLoopPickupEventFromPollRegistryByName
(
		P_BP_EVENT_MANAGER mgr,
		char              *event_name
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Add Event Message Processors and Add Routes %%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// add processor to the event manager (adds to processor registry)
BP_ERROR_T BP_EventMgrAddProcessor
(
	P_BP_EVENT_MANAGER mgr,
	char *processor_name,
	char *processor_description,
	BP_EVENT_PROCESSOR_COMPUTE_TARGET_TYPE compute_target_type,
	BP_EVENT_PROCESSOR_FUNC_T     processor_entry_point
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Debug/Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// display an event manager
BP_ERROR_T BP_EventMgrDisplay
(
		P_BP_EVENT_MANAGER mgr,
		BP_BOOL display_processor_reg,
		BP_BOOL display_events_reg
);

// display an event managers processor registry
BP_ERROR_T BP_EventMgrDisplayProcessorRegistry
(
		P_BP_EVENT_MANAGER mgr,
		BP_BOOL            display_registry_entries = BP_TRUE
);

// Display an event processor.  Requires a valid manager to use as a reference
// point when displaying data.
BP_ERROR_T BP_EventMgrDisplayProcessor
(
		P_BP_EVENT_MANAGER   mgr,
		P_BP_EVENT_PROCESSOR processor
);

// Displays processor routes.
BP_ERROR_T BP_EventMgrDisplayProcessorRoutes
(
		P_BP_EVENT_MANAGER   mgr,
		P_BP_EVENT_PROCESSOR processor
);

// Displays the worker registry in the event manager.
BP_ERROR_T BP_EventMgrDisplayWorkerRegistry
(
		P_BP_EVENT_MANAGER mgr,
		BP_BOOL            display_registry_entries = BP_TRUE
);

// Display a single worker entry
BP_ERROR_T BP_EventMgrDisplayWorker
(
		P_BP_EVENT_MANAGER mgr,
		P_BP_EVENT_WORKER worker
);

// Displays weight distribution information
BP_ERROR_T BP_EventMgrDisplayWorkerWeightDistributionInfo
(
		P_BP_EVENT_MANAGER mgr
);



#endif /* BP_EVENTS_H_ */

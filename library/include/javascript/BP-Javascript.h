/*
 * BP-Javascript.h
 *
 *  Created on: May 14, 2015
 *      Author: root
 */

#ifndef INCLUDE_JAVASCRIPT_BP_JAVASCRIPT_H_
#define INCLUDE_JAVASCRIPT_BP_JAVASCRIPT_H_


// privilege level of current manager
typedef enum BP_JS_PRIVILEGE_LEVEL_T
{
	BP_JS_PRIVILEGE_LEVEL_UNSET,
	BP_JS_PRIVILEGE_LEVEL_ADMIN,
	BP_JS_PRIVILEGE_LEVEL_USER
};

// context in which script is to run (
typedef enum BP_JS_RUN_CONTEXT_T
{
	BP_JS_CONTEXT_UNSET,
	BP_JS_CONTEXT_SYSTEM,
	BP_JS_CONTEXT_HTML_DOM
};


// Javascript Manager.  Holds information relative to the parsing
// of javascript.
typedef struct __BP_JS_MANAGER {



	// semaphore lock
	P_BP_SEMAPHORE_INFO lock;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Identifiers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// name of this manager
	char * name;

	// description of this manager
	char * description;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Context and Priv %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// run context (determines what context the script will be run, which
	// essentially determines which javascript functions will be available
	// to scripts which are running.)
	BP_JS_RUN_CONTEXT_T execution_context;

	// The privilege level to run at.
	BP_JS_PRIVILEGE_LEVEL_T privilege_level;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Spidermonkey Vars %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// the js runtime associated with this manager.
	JSRuntime *runtime;

	// the js context associated with this manager
	JSContext *cx;

	// the thread id associated with this manager
	pthread_t thread_id;

} BP_JS_MANAGER, *P_BP_JS_MANAGER;





// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Function Pointer Typedefs %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// javascript error reporter function pointer
typedef void (*BP_JS_REPORTER_FUNC_T)(JSContext *cx, const char *message, JSErrorReport *report);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Default Callbacks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// default error reporter
void BP_JSDefaultErrorReporter(JSContext *cx, const char *message, JSErrorReport *report);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Quality of Life Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Macro to validate the we have a correct working environment within the
// global hash registry.
#define JS_VALIDATE_WORKING_ENVIRONMENT(ret_val) \
	if(BP_JSEnsureWorkingGlobalHashTableEnvirionment() == ERR_FAILURE) \
	{\
		printf("\n [!!] Error: Could not validate javascript working environment within global hash table.  Critical error.");\
		return ret_val;\
	}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Environment Validation %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// This is called to ensure that the /system/javascript table has been created
BP_ERROR_T BP_JSEnsureWorkingGlobalHashTableEnvirionment();


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Creating/Working with Events Suitible for Event System %%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//! Privileges which are used for determining which javascript events
//! can be used in which ways.
typedef struct __BP_JS_EVENT_PRIVILEGES {



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% General Security %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// able to run shell commands
	BP_BOOL shell_access;

	// able to read from the filesystem
	BP_BOOL file_read_access;

	// able to write to the filesystem
	BP_BOOL file_write_access;

	// able to execute files
	BP_BOOL file_execute_access;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% System Hash Table Access %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// --- System GHT hash table --------------------------------

	// can access the ght in read mode
	BP_BOOL system_ght_read_access;

	// can access the ght in write mode
	BP_BOOL system_ght_write_access;


	// --- Local worker managment data hash table ---------------

	// can access the local worker thread hash table in read mode
	BP_BOOL local_worker_thread_ht_read_access;

	// can access the local worker thread hash table in write mode
	BP_BOOL local_worker_thread_ht_write_access;


	// --- Local worker persistent javascript hash table --------

	// can access the local persistent javascript hash table in read mode
	BP_BOOL local_worker_javascript_persistant_ht_read_access;

	// can access the local persistent javascript hash table in write mode
	BP_BOOL local_worker_javascript_persistant_ht_write_access;


	// --- Output transformations hash table --------------------

	// can access the ght in read mode
	BP_BOOL output_transformation_read_access;

	// can access the ght in write mode
	BP_BOOL output_transformation_write_access;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Network Permissions %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// --- Standard networking options ---------------

	// determines if javascript can use blockparty networking
	BP_BOOL networking_access;

	// determines if javascript can use raw sockets
	BP_BOOL raw_sockets_access;

	// determines if javascript can go promiscuous
	BP_BOOL promiscuous_access;


	// --- cURL options -----------------------------

	// determines if curl is enabled
	BP_BOOL curl_enabled;


	// --- Proxy Information -----------------------

	// determines if the proxy database is enabled
	BP_BOOL proxy_modification_enabled;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Result Database %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// read access to the result database
	BP_BOOL result_db_read_access;

	// write access to the result database
	BP_BOOL result_db_write_access;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Output Transforms %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// output transformations read access
	BP_BOOL output_transformations_read_access;

	// output transformations write access
	BP_BOOL output_transformations_write_access;



} BP_JS_EVENT_PRIVILEGES, *P_BP_JS_EVENT_PRIVILEGES;

//! Javascript event structure
typedef struct __BP_JS_EVENT_DATA {


	//! code to be processed
	char *javascript_code;

	//! total size of the code
	size_t code_size;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Flags %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	//! run context
	BP_JS_RUN_CONTEXT_T run_context;

	//! privilege level
	BP_JS_PRIVILEGE_LEVEL_T privilege_level;

	//! privileges associated with this event
	BP_JS_EVENT_PRIVILEGES privileges;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Processed Info %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// information regarding event processing
	BP_BOOL event_has_been_processed;



} BP_JS_EVENT_DATA, *P_BP_JS_EVENT_DATA;


// Creates a javascript event within the event manager.
BP_ERROR_T BP_JSCreateEvent
(
		P_BP_EVENT_MANAGER         event_manager,
		P_BP_EVENT_PROCESSOR       processor,
		P_BP_EVENT_PROCESSOR       output_transformation_processor,
		BP_EVENT_MESSAGE_IN_OUT    append_to_queue,
		char *                     event_name,
		char *                     event_description,
		char *                     event_javascript_code,
		size_t                     event_code_size,
		BP_JS_RUN_CONTEXT_T        event_run_ctx,
		BP_JS_PRIVILEGE_LEVEL_T    event_privilege_level,
		P_BP_JS_EVENT_PRIVILEGES   event_privileges,
		BP_EVENT_PRIORITY          event_priority
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Lookup Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Lookup global javascript runtime and return it as a pointer.  Searches
// the GHT.
JSRuntime * BP_JSLookupGlobalRuntime();



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create or Destroy Javascript Managers %%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Create a new javascript manager
P_BP_JS_MANAGER BP_JSCreateNewManager
(
	char * mgr_name,
	char *mgr_description,
	BP_JS_RUN_CONTEXT_T execution_context,
	BP_JS_PRIVILEGE_LEVEL_T privilege_level,
	BP_JS_REPORTER_FUNC_T reporter_function
);

// Destroy a javascript manager
BP_ERROR_T BP_JSDestroyManager
(
	P_BP_JS_MANAGER mgr_to_destroy
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Run Javascript Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to run a script
BP_ERROR_T BP_JSRunScript(P_BP_JS_MANAGER mgr, char *script_to_run, size_t script_len);

// Attempts to run a script from a file path.
BP_ERROR_T BP_JSRunScriptFromFile(P_BP_JS_MANAGER mgr, char *path_to_script);


#endif /* INCLUDE_JAVASCRIPT_BP_JAVASCRIPT_H_ */

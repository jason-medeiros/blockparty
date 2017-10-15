/*
 * BP-Javascript.cc
 *
 *  Created on: May 14, 2015
 *      Author: root
 */

// include blockparty main
#include "../include/BP-Main.h"

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Global Default Class Definition %%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Default global class used when spinning up instances.
static JSClass BP_JS_DEFAULT_GLOBAL_CLASS = {
    "global",
    JSCLASS_GLOBAL_FLAGS,
    JS_PropertyStub,
    JS_DeletePropertyStub,
    JS_PropertyStub,
    JS_StrictPropertyStub,
    JS_EnumerateStub,
    JS_ResolveStub,
    JS_ConvertStub,
    nullptr, nullptr, nullptr, nullptr,
    JS_GlobalObjectTraceHook
};


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Global System Level Function Imports %%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// externaly defined system function table
extern JSFunctionSpec BP_JS_GLOBAL_SYSTEM_CONTEXT_FUNCTION_TABLE;

// externally defined admin function table
extern JSFunctionSpec BP_JS_GLOBAL_ADMIN_PRIV_FUNCTION_TABLE;

// externally defined user function table
extern JSFunctionSpec BP_JS_GLOBAL_USER_PRIV_FUNCTION_TABLE;

// externally defined dom function table
extern JSFunctionSpec BP_JS_GLOBAL_DOM_PRIV_FUNCTION_TABLE;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Default Callbacks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// default error reporter
void BP_JSDefaultErrorReporter(JSContext *cx, const char *message, JSErrorReport *report)
{

	// display the message to standard out
	printf
	(
			"\n [!!] %s:%u:%s\n",
			report->filename ? report->filename : "[internal_builtin]",
			(unsigned int) report->lineno,
			message
	);


}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Environment Validation %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



// This is called to ensure that the /system/javascript table has
// been setup correctly.  It simply searches for an existing semaphore
// table and if it doesn't exist, adds it to /system/.
BP_ERROR_T BP_JSEnsureWorkingGlobalHashTableEnvirionment()
{

	// ensure the hash table has been initialized tok
	if(!(BP_GLOBAL_HASH_TABLE_INIT_OK))
	{
		printf("\n [!!] Error: Attempted to create a semaphore even though the global hash tables were not initialized.  Please call BP_GLOBAL_HASH_TABLE_INIT before attempting to use blockparty semaphores.");
		return ERR_FAILURE;
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Hash Table Lookups %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Simple flag to indicate via logic below whether or not the /system/semaphores table
	// exists already.
	size_t semaphore_table_exists = 0;

    // create a table path on the stack for ease of use
	char * table_path_actual[10] = {0};

	// reference to the local stack
	BP_HASH_TABLE_TRAVERSAL_PATH table_path = (char **) &table_path_actual;

	// set path to /system/javascript
	table_path[0] = "system";
	table_path[1] = "javascript";
	table_path[2] = "managers";
	table_path[3] = NULL;

	// attempt lookup for /system/semaphores
	if(BP_HashRegLookupTableByPath(NULL, table_path))
		return ERR_SUCCESS;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Tables If Necessary %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// First check if the system path exists.  If not, create it.
	table_path[0] = "system";
	table_path[1] = NULL;

	// lookup system
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);

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
			return ERR_FAILURE;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Lookups %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// now check for the javascript path
	table_path[0] = "system";
	table_path[1] = "javascript";
	table_path[2] = NULL;

	// add the table
	BP_HashRegAddTableByPath(NULL, table_path);

	// ensure it exists
	tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);

	// ensure we can lookup the table, and exit if we
	// failed for some reason.
	if(!tmp_lookup)
		return ERR_FAILURE;




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Lookups %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// now check for the javascript path
	table_path[0] = "system";
	table_path[1] = "javascript";
	table_path[2] = "managers";
	table_path[3] = NULL;

	// add the table
	BP_HashRegAddTableByPath(NULL, table_path);

	// ensure it exists
	tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);

	// ensure we can lookup the table, and exit if we
	// failed for some reason.
	if(!tmp_lookup)
		return ERR_FAILURE;


	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Lookup Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Lookup global javascript runtime and return it as a pointer.  Searches
// the GHT.
JSRuntime * BP_JSLookupGlobalRuntime()
{

	// validate we have a working environment
	JS_VALIDATE_WORKING_ENVIRONMENT(NULL);

	// Simple flag to indicate via logic below whether or not the runtime
	// exists already.
	size_t runtime_exists = 0;

	// create a table path on the stack for ease of use
	char * table_path_actual[10] = {0};

	// reference to the local stack
	BP_HASH_TABLE_TRAVERSAL_PATH table_path = (char **) &table_path_actual;

	// set path to /system/javascript
	table_path[0] = "system";
	table_path[1] = "javascript";
	table_path[2] = "runtime";
	table_path[3] = NULL;

	// attempt to lookup the runtime in the GHT
	P_BP_HASH_TABLE_KEYED_ENTRY runtime_entry = BP_HashRegLookupTableEntryByPath(NULL, table_path);

	// ensure we could fetch the entry
	if(!runtime_entry)
		return NULL;

	// ensure that the entry has data
	if(!runtime_entry->data)
		return NULL;

	// set the runtime from the data pointer of the lookup
	JSRuntime * runtime = (JSRuntime *) runtime_entry->data;

	// return indicating success
	return runtime;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Creating/Working with Events Suitible for Event System %%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Javascript event structure
typedef struct __BP_JS_EVENT {

	// code to be processed
	char *javascript_code;

	// total size of the code
	size_t code_size;


} BP_JS_EVENT, *P_BP_JS_EVENT;


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
)
{


	if(!event_manager)
		return ERR_FAILURE;
	if(!processor)
		return ERR_FAILURE;
	if(!output_transformation_processor)
		return ERR_FAILURE;
	if(!append_to_queue)
		return ERR_FAILURE;
	if(!event_name)
		return ERR_FAILURE;
	if(!event_description)
		return ERR_FAILURE;
	if(!event_javascript_code)
		return ERR_FAILURE;
	if(!event_code_size)
		return ERR_FAILURE;
	if(!event_priority)
		return ERR_FAILURE;


	// ensure that the code matches the size provided
	if(bpstrlen(event_javascript_code) != event_code_size)
		return ERR_FAILURE;

	// ensure the code has no insane characters
	if(!BP_StringIsReasonablePrintableString(event_javascript_code, event_code_size, BP_TRUE, BP_TRUE))
		return ERR_FAILURE;


	// create new event data
	P_BP_JS_EVENT_DATA new_event = (P_BP_JS_EVENT_DATA) bpcalloc(sizeof(BP_JS_EVENT_DATA), 1);


	// set javascript code and size
	new_event->javascript_code = bpstrdup(event_javascript_code);
	new_event->code_size       = event_code_size;

	// create type marker
	BP_EVENT_TYPE_MARKER type_marker = BP_EVENT_TYPE_MARKER_UNSET;

	// switch on the event running context type to determine which
	// marker is the correct one to use.
	switch(event_run_ctx)
	{

		// set context to system if its a system event
		case BP_JS_CONTEXT_SYSTEM:
			type_marker = BP_EVENT_TYPE_MARKER_SYSTEM_JS_EVENT;
			break;

		// set context to dom if it's a dom event
		case BP_JS_CONTEXT_HTML_DOM:
			type_marker = BP_EVENT_TYPE_MARKER_DOM_JS_EVENT;
			break;

		// default is no context
		default:
			type_marker = BP_EVENT_TYPE_MARKER_UNSET;
			break;

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Dispatch Event %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new event
	BP_ERROR_T err_status = BP_EventMgrCreateEvent
	(
		event_manager,
		processor,
		output_transformation_processor,
		append_to_queue,
		type_marker,
		event_name,
		event_description,
		(void *) new_event,
		sizeof(BP_JS_EVENT_DATA),
		event_priority
	);

	// ensure event was created ok
	if(!err_status)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create or Destroy Javascript Managers %%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Create a new javascript manager
P_BP_JS_MANAGER BP_JSCreateNewManager
(
	char *                  mgr_name,
	char *                  mgr_description,
	BP_JS_RUN_CONTEXT_T     execution_context,
	BP_JS_PRIVILEGE_LEVEL_T privilege_level,
	BP_JS_REPORTER_FUNC_T   reporter_function
)
{

	// validate we have a working environment
	JS_VALIDATE_WORKING_ENVIRONMENT(NULL);

	// ensure we have a manager name and description
	if(!mgr_name || !mgr_description)
		return NULL;

	// ensure the name and desc are reasonable printables
	if(BP_StringIsReasonablePrintableString(mgr_name, bpstrlen(mgr_name), BP_FALSE, BP_FALSE) != ERR_SUCCESS)
		return NULL;

	// ensure the name and desc are reasonable printables
	if(BP_StringIsReasonablePrintableString(mgr_description, bpstrlen(mgr_description), BP_FALSE, BP_FALSE) != ERR_SUCCESS)
		return NULL;

	// enter critical section before creating javascript manager
	BP_GHT_ENTER_CRITICAL_SECTION;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create New Entry for Manager %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create a table path on the stack for ease of use
	char * table_path_actual[10] = {0};

	// reference to the local stack
	BP_HASH_TABLE_TRAVERSAL_PATH table_path = (char **) &table_path_actual;

	// now check for the javascript path
	table_path[0] = "system";
	table_path[1] = "javascript";
	table_path[2] = "managers";
	table_path[3] = mgr_name;
	table_path[4] = NULL;

	// ensure it exists
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);

	// ensure we can lookup the table, and exit if we
	// failed for some reason.
	if(tmp_lookup)
	{
		printf("\n [!!] Error: A javascript manager by this name already exists.");
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);
	}

	// add the table
	BP_HashRegAddTableByPath(NULL, table_path);

	// ensure it exists
	tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);

	// ensure we can lookup the table, and exit if we
	// failed for some reason.
	if(!tmp_lookup)
	{
		printf("\n [!!] Error: When adding a new javascript manager, we were not able to lookup the manager after attempted addition.  This is critical.");
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% set the runtime for the manager %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// now check for the javascript path
	table_path[0] = "system";
	table_path[1] = "javascript";
	table_path[2] = "managers";
	table_path[3] = mgr_name;
	table_path[4] = NULL;


	// create new jsruntime
	JSRuntime *rt = JS_NewRuntime(8L * 1024 * 1024, JS_USE_HELPER_THREADS);
	if(!rt)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	// add data to the table (link in the runtime pointer)
	BP_HashRegAddDataToTableByPath(NULL, table_path, "runtime", BP_HASH_TABLE_KEYED_ENTRY_TYPE_C_DATASTRUCTURE_EXTERNALLY_MANAGED, (void *) rt, sizeof(rt));


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Lookups %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// now check for the javascript path
	table_path[0] = "system";
	table_path[1] = "javascript";
	table_path[2] = "managers";
	table_path[3] = mgr_name;
	table_path[4] = NULL;


	// add data to the table (link in the runtime pointer)
	BP_HashRegAddDataToTableByPath
	(
		NULL,
		table_path,
		"manager",
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_JS_MANAGER)
	);

	// now check for the javascript path
	table_path[0] = "system";
	table_path[1] = "javascript";
	table_path[2] = "managers";
	table_path[3] = mgr_name;
	table_path[4] = "manager";
	table_path[5] = NULL;

	// lookup the new data entry
	tmp_lookup = BP_HashRegLookupTableEntryByPath(NULL, table_path);

	// dereference data
	P_BP_JS_MANAGER js_mgr = (P_BP_JS_MANAGER) tmp_lookup->data;

	// ensure we have a new manager
	if(!js_mgr)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Assign Manager Settings %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the name
	js_mgr->name = bpstrdup(mgr_name);

	// set the description
	js_mgr->description = bpstrdup(mgr_description);

	// init new lock
	js_mgr->lock = BP_SemaphoreInitDefault();

	// set context
	js_mgr->execution_context = execution_context;

	// set privilege level
	js_mgr->privilege_level = privilege_level;

	// set the runtime]
	js_mgr->runtime = rt;

	// the thread id associated with this manager/runtime
	js_mgr->thread_id = pthread_self();



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create New Context %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new context
	js_mgr->cx = JS_NewContext(js_mgr->runtime, 8192);

	// ensure we created a new context
	if(!js_mgr->cx)
		return NULL;

	// set the error reporter function
	if(!reporter_function)
		JS_SetErrorReporter(js_mgr->cx, BP_JSDefaultErrorReporter);
	else
		JS_SetErrorReporter(js_mgr->cx, reporter_function);

	// leave critical section
	BP_GHT_LEAVE_CRITICAL_SECTION;



	// return the new manager
	return js_mgr;

}


// Destroy a javascript manager
BP_ERROR_T BP_JSDestroyManager
(
	P_BP_JS_MANAGER mgr_to_destroy
)
{

	// ensure we have a manager
	if(!mgr_to_destroy)
		return ERR_FAILURE;

	// enter critical section before modifying GHT
	BP_GHT_ENTER_CRITICAL_SECTION;

	// lock the manager
	BP_SemaphoreTryWait(mgr_to_destroy->lock);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Lookup the Manager in GHT %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create a table path on the stack for ease of use
	char * table_path_actual[10] = {0};

	// reference to the local stack
	BP_HASH_TABLE_TRAVERSAL_PATH table_path = (char **) &table_path_actual;

	// now check for the javascript path
	table_path[0] = "system";
	table_path[1] = "javascript";
	table_path[2] = "managers";
	table_path[3] = mgr_to_destroy->name;
	table_path[4] = "manager";


	// lookup manager entry
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = BP_HashRegLookupTableEntryByPath(NULL, table_path);

	// ensure we can lookup the table, and exit if we
	// failed for some reason.
	if(!tmp_lookup)
	{
		printf("\n [!!] Error: Attempted to destroy a javascript manager that did not exist.");
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}


	// set the looked up manager
	P_BP_JS_MANAGER lookup_mgr = (P_BP_JS_MANAGER) tmp_lookup->data;
	if(!lookup_mgr)
	{
		printf("\n [!!] Error: Attempted to destroy a javascript manager that routed to a NULL pointer.  This is a critical error.");
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Context %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// destroy context
	if(lookup_mgr->cx)
	{
		JS_DestroyContext(lookup_mgr->cx);
		lookup_mgr->cx = NULL;
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Runtime %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// destroy runtime if one has been set
	if(lookup_mgr->runtime)
	{

		// destroy the runtime
		JS_DestroyRuntime(lookup_mgr->runtime);
		lookup_mgr->runtime = NULL;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Name and Description %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// destroy name/desc if they were set
	if(lookup_mgr->name)
		bpfree(lookup_mgr->name);
	if(lookup_mgr->description)
		bpfree(lookup_mgr->description);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Zero Extant Memory %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// save the lock before zeroing
	P_BP_SEMAPHORE_INFO preserved_lock = lookup_mgr->lock;

	// zero the manager
	memset(lookup_mgr, 0x00, sizeof(BP_JS_MANAGER));



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy GHT Entries %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// now check for the javascript path
	table_path[0] = "system";
	table_path[1] = "javascript";
	table_path[2] = "managers";
	table_path[3] = mgr_to_destroy->name;


	// destroy table and all sub entries
	BP_ERROR_T ret_status = BP_HashRegDestroyTableByPath(NULL, table_path);

	// ensure we can lookup the table, and exit if we
	// failed for some reason.
	if(!ret_status)
	{
		printf("\n [!!] Failed to dstroy javascript manager tables!.");
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}



	// unlock the semaphore and destroy it immediately after
	BP_SemaphorePost(preserved_lock);

	// destroy preserved semaphore
	BP_SemaphoreDestroy(preserved_lock);

	// exit critical section
	BP_GHT_LEAVE_CRITICAL_SECTION;

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Run Javascript Through Manager %%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to run a script
BP_ERROR_T BP_JSRunScript(P_BP_JS_MANAGER mgr, char *script_to_run, size_t script_len)
{

	// ensure we have a manager and script
	if(!mgr)
		return ERR_FAILURE;
	if(!script_to_run)
		return ERR_FAILURE;
	if(!script_len)
		return ERR_FAILURE;

	// First ensure that the script is reasonable
	if(BP_StringIsReasonablePrintableString(script_to_run, script_len, BP_TRUE, BP_TRUE) == ERR_FAILURE)
		return ERR_FAILURE;

	// wait on the managers lock
	BP_SemaphoreWait(mgr->lock);

	// Get current thread id
	pthread_t curr_thread = pthread_self();

	// Ensure we are only operating on a manager within the same thread
	// of origin.
	if(curr_thread != mgr->thread_id)
	{
		BP_SemaphorePost(mgr->lock);
		return ERR_FAILURE;
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Execution Compartment %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Enter a request before running anything in the context
	JSAutoRequest ar(mgr->cx);

	// create global object with new compartment
	JS::RootedObject global(mgr->cx);

	// create global
	global = JS_NewGlobalObject(mgr->cx, &BP_JS_DEFAULT_GLOBAL_CLASS, nullptr, JS::DontFireOnNewGlobalHook);

	// exit if we can't create the global object
	if(!global)
	{
		BP_SemaphorePost(mgr->lock);
		return ERR_FAILURE;
	}

	// set rval
	JS::RootedValue rval(mgr->cx);

	// autocompartment
	JSAutoCompartment ac(mgr->cx, global);

	// init standard classes
	if(!JS_InitStandardClasses(mgr->cx, global))
	{
		BP_SemaphorePost(mgr->lock);
		return ERR_FAILURE;
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Functions Based on Priv / Context %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add functions based on privilege levels
	switch(mgr->privilege_level)
	{

		// add admin and user functions if we're admin
		case BP_JS_PRIVILEGE_LEVEL_ADMIN:
			JS_DefineFunctions(mgr->cx, global, &BP_JS_GLOBAL_ADMIN_PRIV_FUNCTION_TABLE);
			JS_DefineFunctions(mgr->cx, global, &BP_JS_GLOBAL_USER_PRIV_FUNCTION_TABLE);
			break;

		// only add user functions if we're user level
		case BP_JS_PRIVILEGE_LEVEL_USER:
			JS_DefineFunctions(mgr->cx, global, &BP_JS_GLOBAL_USER_PRIV_FUNCTION_TABLE);
			break;

		// do nothing
		default:
			break;

	}

	// switch on the execution context and define functions appropriately
	switch(mgr->execution_context)
	{

		// add system functions here
		case BP_JS_CONTEXT_SYSTEM:
			JS_DefineFunctions(mgr->cx, global, &BP_JS_GLOBAL_SYSTEM_CONTEXT_FUNCTION_TABLE);
			break;

		// if we're not in the system context, add DOM objects
		case BP_JS_CONTEXT_HTML_DOM:
			JS_DefineFunctions(mgr->cx, global, &BP_JS_GLOBAL_DOM_PRIV_FUNCTION_TABLE);
			break;

		// exit
		default:
			break;

	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Execute Script %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// evaluate code within current context using new global
	bool ok = JS_EvaluateScript(mgr->cx, global, script_to_run, script_len, ".", 1, &rval);

	// if the script failed to execute, return indicating so
	if(!ok)
	{

		// add error if desired
		// JS_ReportError(cx,"Script failed to execute.");

		// try to collect garbage if there is any available
		JS_MaybeGC(mgr->cx);

		// unlock before exit
		BP_SemaphorePost(mgr->lock);

		// return indicating failure
		return ERR_FAILURE;

	}

	// try to collect garbage if there is any available
	JS_MaybeGC(mgr->cx);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Exit %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// unlock before exit
	BP_SemaphorePost(mgr->lock);

	// return indicating success
	return ERR_SUCCESS;
}

// Attempts to run a script from a given file path.
BP_ERROR_T BP_JSRunScriptFromFile(P_BP_JS_MANAGER mgr, char *path_to_script)
{

	// ensure we have a script path
	if(!path_to_script)
		return ERR_FAILURE;

	// ensure the path is reasonable
	if(!BP_StringIsReasonablePrintableString(path_to_script, bpstrlen(path_to_script), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// ensure the script exists
	if(BP_FileAtPathExistsAndIsReadable(path_to_script) != ERR_SUCCESS)
		return ERR_FAILURE;

	// retrieve script content
	char * script_content = BP_FileGetContents(path_to_script);

	// ensure we have script content
	if(!script_content)
		return ERR_FAILURE;

	// run the script
	if(BP_JSRunScript(mgr, script_content, bpstrlen(script_content)-1) == ERR_FAILURE)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;
}


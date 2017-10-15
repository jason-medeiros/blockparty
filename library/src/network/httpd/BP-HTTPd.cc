/*
 * BP-HTTPd.cc
 *
 *
 * This is the main file associated with the libmicrohttpd interface of blockparty.
 *
 *  Created on: Jul 6, 2015
 *      Author: root
 */




#include "../../../include/BP-Main.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Global Hash Registry Related Routines %%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This is called to ensure that the /system/httpd table has
// been setup correctly.  It simply searches for an existing semaphore
// table and if it doesn't exist, adds it to /system/.
BP_ERROR_T BP_HTTPdEnsureWorkingGlobalHashTableEnvirionment()
{


	// ensure the hash table has been initialized tok
	if(!(BP_GLOBAL_HASH_TABLE_INIT_OK))
	{
		printf("\n [!!] Error: Attempted to create a semaphore even though the global hash tables were not initialized.  Please call BP_GLOBAL_HASH_TABLE_INIT before attempting to use blockparty semaphores.");
		return ERR_FAILURE;
	}

	// enter ght critical section
	BP_GHT_ENTER_CRITICAL_SECTION;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Hash Table Lookups %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Simple flag to indicate via logic below whether or not the /system/semaphores table
	// exists already.
	size_t httpd_table_exists = 0;

    // create a table path on the stack for ease of use
	char * table_path_actual[10] = {0};

	// reference to the local stack
	BP_HASH_TABLE_TRAVERSAL_PATH table_path = (char **) &table_path_actual;

	// set path to /system/httpd
	table_path[0] = "system";
	table_path[1] = "httpd";
	table_path[2] = NULL;

	// attempt lookup for /system/semaphores
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);

	// display lookup results
	if(tmp_lookup)
		httpd_table_exists = BP_TRUE;
	else
		httpd_table_exists = BP_FALSE;

	// if the table exists, immediately  return OK
	if(httpd_table_exists == BP_TRUE)
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
	table_path[1] = "httpd";
	table_path[2] = NULL;

	// add the table
	BP_HashRegAddTableByPath(NULL, table_path);

	// ensure it exists
	tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);

	// ensure we can lookup the table, and exit if we
	// failed for some reason.
	if(!tmp_lookup)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// return indicating success
	BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create and Destroy %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Attempt to create a new httpd session provided a given event manager.  The event
// manager will be used to broker out work to the event system.
P_BP_HTTPD_SESSION BP_HTTPdCreateSession
(
		char *                    identifier,
		unsigned int              start_flags,
		unsigned short            port,
		MHD_AcceptPolicyCallback  apc,
		MHD_AccessHandlerCallback dh,
		struct MHD_OptionItem *   runtime_options,
		void                    * extra_data,
		size_t                    extra_data_len
)
{

	// ensure we have an identifier
	if(!identifier)
		return NULL;
	if(!port)
		return NULL;


	// ensure the name identifier is reasonable/printable
	if(BP_StringIsReasonablePrintableString(identifier, bpstrlen(identifier), BP_FALSE, BP_FALSE) != BP_TRUE)
		return NULL;

	// ensure we have a correct working environment (creates if one doesn't exist)
	BP_HTTPD_VALIDATE_WORKING_ENVIRONMENT(NULL);

	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt Lookup %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section
	BP_GHT_ENTER_CRITICAL_SECTION;

	// set the initial path
	table_path[0] = "system";
	table_path[1] = "httpd";
	table_path[2] = identifier;



	// attempt to lookup the httpd table in the GHT
	tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);
	if(tmp_lookup)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	// add the table
	BP_HashRegAddTableByPath(NULL, table_path);

	// check for tmp lookup again after addition
	tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);

	// exit if we added things ok, if not return failure immediately
	if(!tmp_lookup)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Add HTTPd To Registry %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the initial path
	table_path[0] = "system";
	table_path[1] = "httpd";
	table_path[2] = identifier;

	// attempt to add data to the table
	BP_ERROR_T err_status = BP_HashRegAddDataToTableByPath
	(
		NULL,
		table_path,
		"session",
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_HTTPD_SESSION)
	);

	// set path to /system/httpd/SELECTEDNAME/session
	table_path[0] = "system";
	table_path[1] = "httpd";
	table_path[2] = identifier;
	table_path[3] = "session";
	table_path[4] = NULL;

	// declare the lookup entry
	P_BP_HASH_TABLE_KEYED_ENTRY lookup_entry = NULL;

	// attempt to get entry
	lookup_entry = BP_HashRegLookupTableEntryByPath(NULL,table_path);

	// ensure we can lookup the new entry
	if(!lookup_entry)
	{
		printf("\n [!!] Error: Could not lookup the httpd session entry using the provided path.  This is critical and should never occur.  Possible dangling httpd table exists.");
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);
	}

	// set semaphore from data pointer
	P_BP_HTTPD_SESSION httpd = (P_BP_HTTPD_SESSION) lookup_entry->data;

	// ensure we have data
	if(!httpd)
	{
		printf("\n [!!] Error: Created httpd entry, but the data pointer is empty.  This indicates a bad allocation and should never occur.");
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Session Members %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the identifier here
	httpd->identifier = (char *) bpstrdup(identifier);

	// set the start flags
	httpd->start_flags = start_flags;

	// set the port
	httpd->port = port;


	// --- Create Registries ---

	// create new hash table registry for the httpd to use
	httpd->system_hreg = BP_CreateHashTableRegistry();

	// create new hash registry for holding requests
	httpd->request_hreg = BP_CreateHashTableRegistry();



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Accept Policy Handler %%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the auth
	httpd->apc     =  apc;

	// the passed cls will always self reference
	// the httpd.
	httpd->apc_cls = (void *) httpd;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Default Req Handler %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the default handler
	httpd->dh     = dh;

	// set the library routine as default here
	if(!httpd->dh)
		httpd->dh = BP_HTTPdDefaultAccessHandlerCallback;

	// the passed cls for the default handler will always be the httpd
	httpd->dh_cls = (void *) httpd;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Runtime Options %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the startup options in the structure from those passed in
	size_t n = 0;
	if(runtime_options)
	for(; runtime_options[n].option; n++)
	{

		// never try to take more than the specified number of options
		if(n >= BP_MHD_MAX_HTTPD_OPTIONS)
			break;

		// set options 1 to 1
		httpd->runtime_options[n].option    = runtime_options[n].option;
		httpd->runtime_options[n].value     = runtime_options[n].value;
		httpd->runtime_options[n].ptr_value = runtime_options[n].ptr_value;

		// set httpd members as duplicates from these vals
		if    (httpd->runtime_options[n].option == MHD_OPTION_HTTPS_MEM_KEY)
			httpd->key_pem = bpstrdup((char *)  httpd->runtime_options[n].ptr_value);
		else if(httpd->runtime_options[n].option == MHD_OPTION_HTTPS_MEM_CERT)
			httpd->cert_pem = bpstrdup((char *) httpd->runtime_options[n].ptr_value);

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Cookie Basic Settings %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set random cookie key on server start
	httpd->cookie_key = BP_GenRandomASCIICharString(7, BP_GEN_RANDOM_ASCII_STRING_TYPE_ALPHA_CAPS_OR_LOWER, NULL);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Initial Hash Registry Values %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create the initial hash registry layouts
	BP_HTTPdCreateInitialHashRegistryLayouts(httpd);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Extra Data If Necessary %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set extra data
	if(extra_data && extra_data_len)
	{
		httpd->extra_data     = extra_data;
		httpd->extra_data_len = extra_data_len;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Unlock and Return Successfully %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave the ght critical section after creation completes
	BP_GHT_LEAVE_CRITICAL_SECTION;

	// return the httpd session
	return httpd;


}

// TODO: attempt to destroy a httpd session
BP_ERROR_T BP_HTTPdDestroySession(P_BP_HTTPD_SESSION httpd)
{


	// return indicating success
	return ERR_SUCCESS;

}





// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% HTTPd Hash Registry Operations %%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// ALL REGISTRIES: creates all required hash registry layouts for any given httpd
BP_ERROR_T BP_HTTPdCreateInitialHashRegistryLayouts(P_BP_HTTPD_SESSION httpd)
{


	// run basic checks
	if(!httpd)
		return ERR_FAILURE;
	if(!httpd->system_hreg)
		return ERR_FAILURE;
	if(!httpd->request_hreg)
		return ERR_FAILURE;

	// create system hash registry layout
	if(BP_HTTPdCreateInitialSystemHashRegistryLayout(httpd) != ERR_SUCCESS)
		return ERR_FAILURE;

	// create request hash registry layout
	if(BP_HTTPdCreateInitialRequestHashRegistryLayout(httpd) != ERR_SUCCESS)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// SYSTEM REGISTRY: This function is called directly from BP_HTTPdCreateSession and should not be
// called directly without full understanding of what you're doing.  This function
// creates the basic hash table layout expected of a webserver.
BP_ERROR_T BP_HTTPdCreateInitialSystemHashRegistryLayout(P_BP_HTTPD_SESSION httpd)
{

	// ensure we have a session structure and a registry to work with
	if(!httpd)
		return ERR_FAILURE;
	if(!httpd->system_hreg)
		return ERR_FAILURE;


	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Basic System Hash Registry Layout %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// enter critical section before making changes to the httpd hash
	// table registry.
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(httpd->system_hreg);

	// initial tables to create
	char * initial_tables[] =
	{
			"users",
			"requests",
			"request_handlers",
			"sessions",
			"api_keys",
			"blacklist",
			"whitelist",
			"log",
			NULL
	};

	// attempt to loop and create all required initial paths
	size_t n = 0;
	for(; initial_tables[n]; n++)
	{

		// set the path
		table_path[0] = initial_tables[n];
		table_path[1] = NULL;

		// attempt to lookup the table in the GHT
		tmp_lookup = BP_HashRegLookupTableByPath(httpd->system_hreg, table_path);
		if(tmp_lookup)
			break;

		// add the table
		BP_HashRegAddTableByPath(httpd->system_hreg, table_path);

		// check for tmp lookup again after addition
		tmp_lookup = BP_HashRegLookupTableByPath(httpd->system_hreg, table_path);

		// exit if we added things ok, if not return failure immediately
		if(!tmp_lookup)
			break;

	}

	// leave critical section before exiting
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(httpd->system_hreg);


	// return indicating success
	return ERR_SUCCESS;

}


// REQUEST REGISTRY: This function is called directly from BP_HTTPdCreateSession and should not be
// called directly without full understanding of what you're doing.  This function
// creates the basic hash table layout expected of a webserver.
BP_ERROR_T BP_HTTPdCreateInitialRequestHashRegistryLayout(P_BP_HTTPD_SESSION httpd)
{

	// ensure we have a session structure and a registry to work with
	if(!httpd)
		return ERR_FAILURE;
	if(!httpd->request_hreg)
		return ERR_FAILURE;


	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Basic System Hash Registry Layout %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// enter critical section before making changes to the httpd hash
	// table registry.
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(httpd->request_hreg);

	// initial tables to create
	char * initial_tables[] =
	{
			"requests",
			"ip_addresses",
			NULL
	};

	// attempt to loop and create all required initial paths
	size_t n = 0;
	for(; initial_tables[n]; n++)
	{

		// set the path
		table_path[0] = initial_tables[n];
		table_path[1] = NULL;

		// attempt to lookup the table in the GHT
		tmp_lookup = BP_HashRegLookupTableByPath(httpd->request_hreg, table_path);
		if(tmp_lookup)
			break;

		// add the table
		BP_HashRegAddTableByPath(httpd->request_hreg, table_path);

		// check for tmp lookup again after addition
		tmp_lookup = BP_HashRegLookupTableByPath(httpd->request_hreg, table_path);

		// exit if we added things ok, if not return failure immediately
		if(!tmp_lookup)
			break;

	}

	// leave critical section before exiting
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(httpd->request_hreg);


	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Start and Stop Httpd %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// start the httpd server
BP_ERROR_T BP_HTTPdStartServer(P_BP_HTTPD_SESSION httpd)
{

	// ensure we have a correct working environment (creates if one doesn't exist)
	BP_HTTPD_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	// dbg: verbosity
	printf("\n [dbg] Attempting to start httpd server");

	// ensure we have a httpd
	if(!httpd)
		return ERR_FAILURE;

	// ensure the httpd has an identifier
	if(!httpd->identifier)
		return ERR_FAILURE;

	// ensure the session we have matches the one we have in the GHT
	if(httpd != BP_HTTPdLookupByName(httpd->identifier))
		return ERR_FAILURE;

	// dbg: display passed checks message
	printf("\n [dbg] Passed initial HTTPd startup checks OK!");

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt MHD Start %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(httpd->runtime_options)
	{

		// attempt to start the daemon directly
		httpd->daemon = MHD_start_daemon
		(
			httpd->start_flags,
			httpd->port,
			httpd->apc,
			httpd->apc_cls,
			httpd->dh ? httpd->dh : BP_HTTPdDefaultAccessHandlerCallback,
			httpd->dh_cls,
			MHD_OPTION_ARRAY,
			httpd->runtime_options,
			MHD_OPTION_END
		);

	}
	else
	{

		// attempt to start the daemon directly
		httpd->daemon = MHD_start_daemon
		(
			httpd->start_flags,
			httpd->port,
			httpd->apc,
			httpd->apc_cls,
			httpd->dh ? httpd->dh : BP_HTTPdDefaultAccessHandlerCallback,
			httpd->dh_cls,
			MHD_OPTION_END
		);

	}

	if(!httpd->daemon)
	{
		printf("\n [!!] Failed to start httpd server");
		return ERR_FAILURE;
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check for EPOLL Flags %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we're using epoll, start the internal epoll loop here
	if(httpd->start_flags & MHD_USE_EPOLL)
	{


	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Mark Server as Started %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// set run state to stopped
	httpd->run_state = BP_HTTPD_SERVER_STATE_STARTED;


	printf("\n [+] Starting httpd: %p (daemon)", httpd->daemon);

	// return indicating success
	return ERR_SUCCESS;

}

// stop the httpd server
BP_ERROR_T BP_HTTPdStopServer(P_BP_HTTPD_SESSION httpd)
{

	// ensure we have a correct working environment (creates if one doesn't exist)
	BP_HTTPD_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	// ensure we have a httpd
	if(!httpd)
		return ERR_FAILURE;

	// ensure the httpd has an identifier
	if(!httpd->identifier)
		return ERR_FAILURE;

	// ensure the session we have matches the one we have in the GHT
	if(httpd != BP_HTTPdLookupByName(httpd->identifier))
		return ERR_FAILURE;

	// enxure we have a daemon
	if(!httpd->daemon)
		return ERR_FAILURE;

	// stop the daemon
	MHD_stop_daemon(httpd->daemon);

	// set run state to stopped
	httpd->run_state = BP_HTTPD_SERVER_STATE_STOPPED;

	// return indicating success
	return ERR_SUCCESS;

}


// this should be called directly before stopping the server (if
// you're being polite).  This lets existing clients finish up
// before stopping things outright.
BP_ERROR_T BP_HTTPdQuiesceServer(P_BP_HTTPD_SESSION httpd)
{

	// ensure we have a correct working environment (creates if one doesn't exist)
	BP_HTTPD_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	// ensure we have a httpd
	if(!httpd)
		return ERR_FAILURE;

	// ensure the httpd has an identifier
	if(!httpd->identifier)
		return ERR_FAILURE;

	// ensure the session we have matches the one we have in the GHT
	if(httpd != BP_HTTPdLookupByName(httpd->identifier))
		return ERR_FAILURE;

	// enxure we have a daemon
	if(!httpd->daemon)
		return ERR_FAILURE;


	// stop the daemon
	MHD_quiesce_daemon(httpd->daemon);

	// set run state to quieted
	httpd->run_state = BP_HTTPD_SERVER_STATE_QUIETED;


	// return indicating success
	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Lookup Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempts to lookup a httpd session by name
P_BP_HTTPD_SESSION BP_HTTPdLookupByName(char * name_identifier)
{

	// first ensure the string is reasonable/printable
	if(BP_StringIsReasonablePrintableString(name_identifier, bpstrlen(name_identifier), BP_FALSE, BP_FALSE) == BP_FALSE)
		return NULL;


	// ensure we have a correct working environment (creates if one doesn't exist)
	BP_HTTPD_VALIDATE_WORKING_ENVIRONMENT(NULL);


	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt Lookup %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section
	BP_GHT_ENTER_CRITICAL_SECTION;

	// set the initial path
	table_path[0] = "system";
	table_path[1] = "httpd";
	table_path[2] =  name_identifier;
	table_path[3] = "session";


	// attempt to lookup the httpd in the GHT
	tmp_lookup = BP_HashRegLookupTableEntryByPath(NULL, table_path);

	// exit if we added things ok, if not return failure immediately
	if(!tmp_lookup)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	// if there is no data lookup exit immediately
	if(!tmp_lookup->data)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);


	// leave the critical section before returning
	BP_GHT_LEAVE_CRITICAL_SECTION;

	// return the looked up data
	return (P_BP_HTTPD_SESSION) tmp_lookup->data;

}









// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Data Extraction %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Extracts relevant GET data into the hash table.  Relevant data is determined by examining the content
// of extract_names and matching them up with MHT internal retrieval routines.
BP_ERROR_T BP_HTTPdExtractGetDataFromRequest(P_BP_HTTPD_SESSION httpd, char **extract_names)
{


	// return indicating success
	return ERR_SUCCESS;

}

// Extracts relevant POST data into the hash table.  Relevant data is determined by examining the content
// of extract_names and matching them up with MHT internal retrieval routines.
BP_ERROR_T BP_HTTPdExtractPostDataFromRequest(P_BP_HTTPD_SESSION httpd, char **extract_names)
{

	// return indicating success
	return ERR_SUCCESS;

}

// Extracts relevant COOKIE data into the hash table.  Relevant data is determined by examining the content
// of extract_names and matching them up with MHT internal retrieval routines.
BP_ERROR_T BP_HTTPdExtractCookieDataFromRequest(P_BP_HTTPD_SESSION httpd, char **extract_names)
{


	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% General HTTPd Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to determine a httpd method using available conveinence macros.
BP_HTTPD_METHOD BP_HTTPdDetermineMethod(char *method)
{

	// ensure we have a method
	if(!method)
		return BP_HTTPD_METHOD_UNSET;

	// ensure the method is reasonable/printable
	if(BP_StringIsReasonablePrintableString(method, bpstrlen(method), BP_FALSE, BP_FALSE) != BP_TRUE)
		return BP_HTTPD_METHOD_UNSET;

	// longest method is 7 characters, so only check up to 8 chars
	size_t method_len = strnlen(method, 8);
	if(method_len > 7)
		return BP_HTTPD_METHOD_UNSET;

	// GET method
	if(BP_HTTPD_RH_METHOD_IS_GET)
		return BP_HTTPD_METHOD_GET;

	// POST method
	if(BP_HTTPD_RH_METHOD_IS_POST)
		return BP_HTTPD_METHOD_POST;

	// PUT method
	if(BP_HTTPD_RH_METHOD_IS_PUT)
		return BP_HTTPD_METHOD_PUT;

	// CONNECT method
	if(BP_HTTPD_RH_METHOD_IS_CONNECT)
		return BP_HTTPD_METHOD_CONNECT;

	// DELETE method
	if(BP_HTTPD_RH_METHOD_IS_DELETE)
		return BP_HTTPD_METHOD_DELETE;

	// HEAD method
	if(BP_HTTPD_RH_METHOD_IS_HEAD)
		return BP_HTTPD_METHOD_HEAD;

	// OPTIONS method
	if(BP_HTTPD_RH_METHOD_IS_OPTIONS)
		return BP_HTTPD_METHOD_OPTIONS;

	// TRACE method
	if(BP_HTTPD_RH_METHOD_IS_TRACE)
		return BP_HTTPD_METHOD_TRACE;


	// if none of the methods are available
	return BP_HTTPD_METHOD_UNSET;

}



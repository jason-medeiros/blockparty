/*
 * BP-HTTPdRequests.cc
 *
 *  Created on: Jul 16, 2015
 *      Author: root
 */



#include "../../../include/BP-Main.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% HTTP Request Tracking %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Adds a request to the request registry.
P_BP_HTTPD_REQUEST BP_HTTPdRequestAdd
(
		P_BP_HTTPD_SESSION         httpd,
		P_BP_HTTPD_REQUEST_HANDLER request_handler,
		struct MHD_Connection *    connection,
		const char *               url,
		const char *               method,
		const char *               version
)
{

	// run basic sanity checks
	if(!httpd)
		return NULL;
	if(!httpd->request_hreg)
		return NULL;
	if(!request_handler)
		return NULL;
	if(!connection)
		return NULL;
	if(!url)
		return NULL;
	if(!method)
		return NULL;
	if(!version)
		return NULL;

	// connection info lookup
	const MHD_ConnectionInfo * info_lookup = NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Initial Connection Determinations %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this flag is used to indicate if a connection is https
	BP_BOOL is_https = BP_FALSE;

	// Get https protocol.  If the return is null, we have a normal
	// http connection.  If it's non-null, we have a HTTPS connection.
	info_lookup = MHD_get_connection_info(connection, MHD_CONNECTION_INFO_PROTOCOL);
	if(info_lookup)
		is_https = BP_TRUE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Extract HTTPS Data If Necessary %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// secure client proto
	BP_HTTPD_REQUEST_TLS_VERSION secure_client_proto_version = BP_HTTPD_REQUEST_TLS_VERSION_UNSET;

	// determine protocol information if we're https
	if(is_https)
	{

		// get protocol
		info_lookup = MHD_get_connection_info(connection, MHD_CONNECTION_INFO_PROTOCOL);

		// switch on the lookup to determine the correct ssl type.
		switch(info_lookup->protocol)
		{

			case GNUTLS_SSL3:
				secure_client_proto_version = BP_HTTPD_REQUEST_TLS_VERSION_SSLV3;
				break;

			case GNUTLS_TLS1_0:
				secure_client_proto_version = BP_HTTPD_REQUEST_TLS_VERSION_TLS1_0;
				break;

			case GNUTLS_TLS1_1:
				secure_client_proto_version = BP_HTTPD_REQUEST_TLS_VERSION_TLS1_1;
				break;

			case GNUTLS_TLS1_2:
				secure_client_proto_version = BP_HTTPD_REQUEST_TLS_VERSION_TLS1_2;
				break;

			default:
				secure_client_proto_version = BP_HTTPD_REQUEST_TLS_VERSION_TLS1_BADVAL;
				break;

		}

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Extract IP Information From Connection %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to lookup the sockaddr information
	info_lookup = MHD_get_connection_info(connection, MHD_CONNECTION_INFO_CLIENT_ADDRESS);
	if(!info_lookup)
		return NULL;

	// ensure it's an IPv4 address (only IP4 supported as of now)
	if(info_lookup->client_addr->sa_family != AF_INET)
		return NULL;


	// static buffer to hold ip address
	char ip_address[128] = {0};

	// set the sockaddr in from the client addr structure
	struct sockaddr_in * sockaddr_struct = (struct sockaddr_in *) info_lookup->client_addr;

	// convert ip address
	inet_ntop(info_lookup->client_addr->sa_family, &sockaddr_struct->sin_addr, (char *) &ip_address, INET_ADDRSTRLEN);

	// NOTE: At this point we have verified that the request is reasonably legit.



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Request Struct in Registry %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create a new request
	P_BP_HTTPD_REQUEST request = BP_HTTPdRequestCreateNewEmpty(httpd);

	// duplicate the ip address
	request->ip_address = bpstrdup(ip_address);

	// set the httpd
	request->httpd = httpd;

	// set the request handler
	request->request_handler = request_handler;

	// set https parameters
	if(is_https)
	{

		// mark the request as being https
		request->secure_client_request = BP_TRUE;

		// set the secure client protocol version
		request->secure_client_proto_version = secure_client_proto_version;

	}

	// ensure we have a method
	request->http_method = BP_HTTPdDetermineMethod((char *) method);

	// if the request method is post, create a post processor
	if(request->http_method == BP_HTTPD_METHOD_POST)
	{

		// create new
		request->pp = MHD_create_post_processor(connection, 1024, BP_HTTPdDefaultPostIteratorCallback, (void *) request);

	}

	// create a new hash table registry
	request->arg_hreg = BP_CreateHashTableRegistry();

	// create the initial layout for arguments (GET/POST/COOKIE)
	BP_HTTPdCreateInitialRequestArgumentHashRegistryLayout(request);

	// at this point we can mark the request as valid and exit, providing
	// the request back to the host.
	request->request_is_valid = BP_TRUE;

	printf("\n Finished creating request in hreg.");
	printf("\n");

	// return the request
	return request;


}



// Attempts to create a new empty request structure.  This routine exists simply
// because the creation logic can be kinda long and it's better to have this logic
// isolated.  Makes the code easier to follow (it was previoulsy within BP_HTTPdRequestAdd).
P_BP_HTTPD_REQUEST BP_HTTPdRequestCreateNewEmpty(P_BP_HTTPD_SESSION httpd)
{

	if(!httpd)
		return NULL;
	if(!httpd->request_hreg)
		return NULL;




	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// enter critical section before making changes to the httpd hash
	// table registry.
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(httpd->request_hreg);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Create New User Structure %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the initial path
	table_path[0] = "requests";
	table_path[1] = NULL;

	// generate new random uuid as unique key
	char * new_uuid = BP_GenRandomUUID();

	// attempt to add data to the table
	BP_ERROR_T err_status = BP_HashRegAddDataToTableByPath
	(
		httpd->request_hreg,
		table_path,
		new_uuid,
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_HTTPD_REQUEST)
	);


	// lookup the request
	P_BP_HTTPD_REQUEST request = BP_HTTPdRequestLookupByUUID(httpd, new_uuid);
	if(!request)
	{
		bpfree(new_uuid);
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(httpd->request_hreg, NULL);
	}

	// set the new uuid in the request here
	request->uuid = new_uuid;

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(httpd->request_hreg);

	// return the looked up request
	return request;

}

// Attempts to destroy a request (called after a request is dispatched automatically, shouldn't be called
// directly unless you know what you're doing).  Destroys all relevant data.
BP_ERROR_T BP_HTTPdRequestDestroy(P_BP_HTTPD_SESSION httpd, P_BP_HTTPD_REQUEST request)
{

	if(!httpd)
		return ERR_FAILURE;
	if(!httpd->request_hreg)
		return ERR_FAILURE;
	if(!request)
		return ERR_FAILURE;


	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// enter critical section before making changes to the httpd hash
	// table registry.
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(httpd->request_hreg);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Arg Registry %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Destroy Arguments First
	if(request->arg_hreg)
	{
		BP_HTTPdRequestDestroyArgRegistry(request);
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Output JSON if Necessary %%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// destroy output
	if(request->json_output)
	{
		BP_JSONDestroy(request->json_output);
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Member Data %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// destroy hostname
	if(request->hostname)
	{
		bpfree(request->hostname);
		request->hostname = NULL;
	}

	// destroy ip address
	if(request->ip_address)
	{
		bpfree(request->ip_address);
		request->ip_address = NULL;
	}

	// Destroy the libmicrohttpd post processor here (if it's set; it shouldn't be set)
	if(request->pp)
	{

		// destroy postprocessor here
		MHD_destroy_post_processor(request->pp);

		// set to null
		request->pp = NULL;

	}

	// destroy uploaded file full path if set (deprecated i think)
	if(request->post_uploaded_full_file_path)
	{
		bpfree(request->post_uploaded_full_file_path);
		request->post_uploaded_full_file_path = NULL;
	}

	// create space on stack to save the request uuid (needed for deletion)
	char saved_uuid[BP_UUID_LEN+10];
	BP_ZERO_STACK(saved_uuid);

	// save uuid if present (should always be present)
	if(request->uuid)
	{
		// copy in the uuid (so we can delete it from parent hreg)
		memcpy((char *) &saved_uuid, request->uuid, BP_UUID_LEN);

		// free heap memory
		bpfree(request->uuid);
		request->uuid = NULL;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Remove From Request Registry %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the requests table
	table_path[0] = "requests";
	table_path[1] = (char *) &saved_uuid;
	table_path[2] = NULL;

	// attempt to lookup the entry to delete
	P_BP_HASH_TABLE_KEYED_ENTRY request_entry = BP_HashRegLookupTableEntryByPath(httpd->request_hreg, table_path);

	// if we can't look it up, exit immediately
	if(!request_entry)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(httpd->request_hreg, ERR_FAILURE);


	// destroy the entry
	BP_HashRegDestroyEntry(httpd->request_hreg, request_entry);

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(httpd->request_hreg);

	// return indicating success
	return ERR_SUCCESS;

}

// Internal function used to destroy the argument registry.  Called automatically via BP_HTTPdRequestDestroy()
BP_ERROR_T BP_HTTPdRequestDestroyArgRegistry(P_BP_HTTPD_REQUEST request)
{

	// ensure we have a request and arguments
	if(!request)
		return ERR_FAILURE;
	if(!request->arg_hreg)
		return ERR_FAILURE;

	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;



	// enter critical section before making changes to the httpd hash
	// table registry.
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(request->arg_hreg);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Iterate Through the Registry %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// request->arg_hreg->hive->hh

	// destroy posted arguments if any exist
	BP_HTTPdRequestDestroyPostArgs(request);

	// destroy the get arguments if any exist
	BP_HTTPdRequestDestroyGetArgs(request);

	// destroy cookie arguments if any exist
	BP_HTTPdRequestDestroyCookieArgs(request);


	// VERBOSITY/DEBUGGING: display the argument registry
	// BP_HashRegDisplay(request->arg_hreg, BP_FALSE, BP_TRUE);

	// destroy the hash table registry
	BP_DestroyHashTableRegistry(request->arg_hreg);

	// ensure the request registry is set to null
	request->arg_hreg = NULL;




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Exit After All Are Destroyed %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(request->arg_hreg);


	// return indicating success
	return ERR_SUCCESS;
}


// --- Individual Request Var Table Destructors ----

// Internal function to destroy post arguments.  Destroys data and closes
BP_ERROR_T BP_HTTPdRequestDestroyPostArgs(P_BP_HTTPD_REQUEST request)
{

	// ensure we have a request
	if(!request)
		return ERR_FAILURE;
	if(!request->arg_hreg)
		return ERR_FAILURE;

	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// enter critical section before making changes to the httpd hash
	// table registry.
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(request->arg_hreg);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Iterate Through the Registry %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the top path to post
	table_path[0] = "POST";
	table_path[1] = NULL;

	// lookup the post table
	tmp_lookup = BP_HashRegLookupTableByPath(request->arg_hreg, table_path);

	// ensure we can lookup the required value
	if(!tmp_lookup)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(request->arg_hreg, ERR_FAILURE);

	// destroy data entries if necessary
	if(tmp_lookup->data)
	{

		// move the tmp lookup to the hash table children (since it's a table)
		tmp_lookup = (P_BP_HASH_TABLE_KEYED_ENTRY) tmp_lookup->data;

		// temporary iterator values used during the HASH_ITER loop below
		P_BP_HASH_TABLE_KEYED_ENTRY entry = NULL;
		P_BP_HASH_TABLE_KEYED_ENTRY tmp   = NULL;

		// iterate through all hashes and display
		HASH_ITER(hh, tmp_lookup, entry, tmp)
		{

			// set variable
			P_BP_HTTPD_REQUEST_VAR tmp_var = (P_BP_HTTPD_REQUEST_VAR) entry->data;

			// if the var is set, destroy its members
			if(tmp_var)
			{

				BP_HTTPdRequestVarDestroyMembers(tmp_var);
			}

		}

	}
	// destroy table by path (clears out all allocated structures)
	BP_HashRegDestroyTableByPath(request->arg_hreg, table_path);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Exit After All Are Destroyed %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(request->arg_hreg);


	// return indicating success
	return ERR_SUCCESS;

}


// Internal function to destroy HTTP GET arguments.  Destroys data and closes
BP_ERROR_T BP_HTTPdRequestDestroyGetArgs(P_BP_HTTPD_REQUEST request)
{

	// ensure we have a request
	if(!request)
		return ERR_FAILURE;
	if(!request->arg_hreg)
		return ERR_FAILURE;

	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// enter critical section before making changes to the httpd hash
	// table registry.
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(request->arg_hreg);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Iterate Through the Registry %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the top path to get
	table_path[0] = "GET";
	table_path[1] = NULL;

	// lookup the post table
	tmp_lookup = BP_HashRegLookupTableByPath(request->arg_hreg, table_path);

	// ensure we can lookup the required value
	if(!tmp_lookup)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(request->arg_hreg, ERR_FAILURE);

	// destroy data entries if necessary
	if(tmp_lookup->data)
	{

		// move the tmp lookup to the hash table children (since it's a table)
		tmp_lookup = (P_BP_HASH_TABLE_KEYED_ENTRY) tmp_lookup->data;

		// temporary iterator values used during the HASH_ITER loop below
		P_BP_HASH_TABLE_KEYED_ENTRY entry = NULL;
		P_BP_HASH_TABLE_KEYED_ENTRY tmp   = NULL;

		// iterate through all hashes and display
		HASH_ITER(hh, tmp_lookup, entry, tmp)
		{

			// set variable
			P_BP_HTTPD_REQUEST_VAR tmp_var = (P_BP_HTTPD_REQUEST_VAR) entry->data;

			// if the var is set, destroy its members
			if(tmp_var)
			{

				BP_HTTPdRequestVarDestroyMembers(tmp_var);
			}

		}

	}
	// destroy table by path (clears out all allocated structures)
	BP_HashRegDestroyTableByPath(request->arg_hreg, table_path);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Exit After All Are Destroyed %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(request->arg_hreg);

	// return indicating success
	return ERR_SUCCESS;

}

// Internal function to destroy cookie arguments.  Destroys data and closes
BP_ERROR_T BP_HTTPdRequestDestroyCookieArgs(P_BP_HTTPD_REQUEST request)
{

	// ensure we have a request
	if(!request)
		return ERR_FAILURE;
	if(!request->arg_hreg)
		return ERR_FAILURE;

	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// enter critical section before making changes to the httpd hash
	// table registry.
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(request->arg_hreg);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Iterate Through the Registry %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the top path to cookie
	table_path[0] = "COOKIE";
	table_path[1] = NULL;

	// lookup the post table
	tmp_lookup = BP_HashRegLookupTableByPath(request->arg_hreg, table_path);

	// ensure we can lookup the required value
	if(!tmp_lookup)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(request->arg_hreg, ERR_FAILURE);

	// destroy data entries if necessary
	if(tmp_lookup->data)
	{

		// move the tmp lookup to the hash table children (since it's a table)
		tmp_lookup = (P_BP_HASH_TABLE_KEYED_ENTRY) tmp_lookup->data;

		// temporary iterator values used during the HASH_ITER loop below
		P_BP_HASH_TABLE_KEYED_ENTRY entry = NULL;
		P_BP_HASH_TABLE_KEYED_ENTRY tmp   = NULL;

		// iterate through all hashes and display
		HASH_ITER(hh, tmp_lookup, entry, tmp)
		{

			// set variable
			P_BP_HTTPD_REQUEST_VAR tmp_var = (P_BP_HTTPD_REQUEST_VAR) entry->data;

			// if the var is set, destroy its members
			if(tmp_var)
			{

				BP_HTTPdRequestVarDestroyMembers(tmp_var);
			}

		}

	}

	// destroy table by path (clears out all allocated structures)
	BP_HashRegDestroyTableByPath(request->arg_hreg, table_path);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Exit After All Are Destroyed %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(request->arg_hreg);

	// return indicating success
	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Request Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to lookup a request by it's uuid
P_BP_HTTPD_REQUEST BP_HTTPdRequestLookupByUUID(P_BP_HTTPD_SESSION httpd, char * uuid)
{

	if(!httpd)
		return NULL;
	if(!httpd->request_hreg)
		return NULL;
	if(!uuid)
		return NULL;


	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// enter critical section before making changes to the httpd hash
	// table registry.
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(httpd->request_hreg);


	// set the table path for lookup
	table_path[0] = "requests";
	table_path[1] = uuid;
	table_path[2] = NULL;

	// attempt to get entry
	P_BP_HASH_TABLE_KEYED_ENTRY lookup_entry = BP_HashRegLookupTableEntryByPath(httpd->request_hreg,table_path);

	// ensure we can lookup the new entry
	if(!lookup_entry)
	{
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(httpd->request_hreg, NULL);
	}

	// set user
	P_BP_HTTPD_REQUEST request = (P_BP_HTTPD_REQUEST) lookup_entry->data;


	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(httpd->request_hreg);

	// return the looked up request
	return request;

}



// REQUEST REGISTRY: This function is called when a new request is added. It ensures
// that the request registry has request specific tables created.
BP_ERROR_T BP_HTTPdCreateInitialRequestArgumentHashRegistryLayout(P_BP_HTTPD_REQUEST request)
{

	// ensure we have a request
	if(!request)
		return ERR_FAILURE;

	// ensure we have an argument registry
	if(!request->arg_hreg)
		return ERR_FAILURE;

	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Basic Argument Hash Registry Layout %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// enter critical section before making changes to the httpd hash
	// table registry.
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(request->arg_hreg);

	// initial tables to create
	char * initial_tables[] =
	{
			"POST",
			"GET",
			"COOKIE",
			"HEADER",
			NULL
	};

	// attempt to loop and create all required initial paths
	size_t n = 0;
	for(; initial_tables[n]; n++)
	{

		// set the path
		table_path[0] = initial_tables[n];
		table_path[1] = NULL;

		// attempt to lookup the table in the table
		tmp_lookup = BP_HashRegLookupTableByPath(request->arg_hreg, table_path);
		if(tmp_lookup)
			break;

		// add the table
		BP_HashRegAddTableByPath(request->arg_hreg, table_path);

		// check for tmp lookup again after addition
		tmp_lookup = BP_HashRegLookupTableByPath(request->arg_hreg, table_path);

		// exit if we added things ok, if not return failure immediately
		if(!tmp_lookup)
			break;

	}

	// leave critical section before exiting
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(request->arg_hreg);

	// return indicating success
	return ERR_SUCCESS;

}


// -- encoding checks ----

// Attempt to translate encoding string to a restricted encoding set (all others
// fail immediately).
BP_HTTPD_REQUEST_CONTENT_TYPE BP_HTTPdRequestGetContentType(char * content_type)
{

	// ensure we have an encoding
	if(!content_type)
		return BP_HTTPD_REQUEST_CONTENT_TYPE_UNSET;

	// ensure encoding is reasonable
	if(!BP_StringIsReasonablePrintableString(content_type, bpstrlen(content_type), BP_FALSE, BP_FALSE))
		return BP_HTTPD_REQUEST_CONTENT_TYPE_UNSET;

	// check to see if it's form encoded
	if(BP_HTTPD_CONTENT_TYPE_IS_APPLICATION_X_WWW_FORM_URLENCODED(content_type))
		return BP_HTTPD_REQUEST_CONTENT_TYPE_APPLICATION_X_WWW_FORM_URLENCODED;

	// check to see if it's multipart form data
	if(BP_HTTPD_POST_ENCODING_IS_MULTIPART_FORM_DATA(content_type))
		return BP_HTTPD_REQUEST_CONTENT_TYPE_MULTIPART_FORM_DATA;

	// check to see if it's an octet stream
	if(BP_HTTPD_CONTENT_TYPE_IS_APPLICATION_OCTET_STREAM(content_type))
		return BP_HTTPD_REQUEST_CONTENT_TYPE_APPLICATION_OCTET_STREAM;


	// return unset if we can't match explicitly
	return BP_HTTPD_REQUEST_CONTENT_TYPE_UNSET;

}


// -- request argument registry addrs ----


// Adds a variable to the registry according to type.
BP_ERROR_T BP_HTTPdRequestVarAddToArgRegistry
(
		P_BP_HTTPD_REQUEST             request,
		BP_HTTPD_REQUEST_VAR_TYPE      type,
		BP_HTTPD_REQUEST_CONTENT_TYPE  content_type,
		char *                         name,
		char *                         data,
		char *                         data_file,
		size_t                         data_size
)
{

	// ensure we have a request and argument registry
	if(!request)
		return ERR_FAILURE;
	if(!request->arg_hreg)
		return ERR_FAILURE;

	// basic checks
	if(!type)
		return ERR_FAILURE;
	if(!content_type)
		return ERR_FAILURE;
	if(!name)
		return ERR_FAILURE;


	// variable table selected
	char * var_table = NULL;

	// switch on the variable type
	switch(type)
	{

		// http post tables
		case BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA:
			var_table = "POST";
			break;
		case BP_HTTPD_REQUEST_VAR_TYPE_POST_TMPFILE:
			var_table = "POST";
			break;

		// type is httpd header
		case BP_HTTPD_REQUEST_VAR_TYPE_HEADER:
			var_table = "HEADER";
			break;

		// http get table
		case BP_HTTPD_REQUEST_VAR_TYPE_GET:
			var_table = "GET";
			break;

		// cookie table
		case BP_HTTPD_REQUEST_VAR_TYPE_COOKIE:
			var_table = "COOKIE";
			break;

		// exit immediately if we have a type mismatch
		default:
			return ERR_FAILURE;
	}

	// ensure we have a table selected
	if(!var_table)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Basic Checks / Lookup %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to lookup the var (checks if it already exists)
	P_BP_HTTPD_REQUEST_VAR var = BP_HTTPdRequestVarArgRegistryLookup(request, type, name);
	if(var)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add New Var %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section for the argument registry
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(request->arg_hreg);


	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// set initial path
	table_path[0] = var_table;
	table_path[1] = NULL;

	// add a new request var
	BP_ERROR_T retval = BP_HashRegAddDataToTableByPath
	(
			request->arg_hreg,
			table_path,
			name,
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
			NULL,
			sizeof(BP_HTTPD_REQUEST_VAR)
	);

	// exit if we cant create the new item
	if(!retval)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(request->arg_hreg, ERR_FAILURE);

	// attempt lookup again, this time we can modify it
	var = BP_HTTPdRequestVarArgRegistryLookup(request, type, name);
	if(!var)
		return ERR_FAILURE;

	// set content type
	var->content_type = content_type;

	// set type
	var->type = type;

	// set name
	if(name)
		var->name = bpstrdup(name);

	// initialize fd as -1
	var->file_descriptor = -1;

	// set data file
	if(data_file)
	{

		// set the data file path
		var->data_file = bpstrdup(data_file);

		// open the file for appending
		var->file_descriptor =  BP_FDCreateSecureTempFile(var->data_file);

	}



	// leave the critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(request->arg_hreg);

	// return indicating success
	return ERR_SUCCESS;

}


// Appends data to an existing request variable.  This is primarily used
// during the post processor.
BP_ERROR_T BP_HTTPdRequestVarAppendData
(
		P_BP_HTTPD_REQUEST             request,
		P_BP_HTTPD_REQUEST_VAR         var,
		char *                         data,
		size_t                         data_size
)
{

	// enusre we have a request (unnecessary, but kept here in case we
	// want to add cross features between request and var structs)
	if(!request)
		return ERR_FAILURE;

	// ensure we have a var
	if(!var)
		return ERR_FAILURE;


	// open the file if necessary
	if(var->data_file)
	{

		// if we have data, open file and append
		if(data && data_size)
		{

			// ensure we can open it
			if(var->file_descriptor == -1)
				return ERR_FAILURE;

			// write data to file (append)
			BP_FDWrite(var->file_descriptor, (char *) data, data_size);


			// flush data to file
			BP_FDFlush(var->file_descriptor);

			// increment the size by the current write portion
			var->file_handle_data_written += data_size;

			// set the storage flag to true if we write any data ever
			var->content_stored_in_file = BP_TRUE;

		}

	}
	else
	{

		// if we have data, alloc and copy
		if(data && data_size)
		{

			// attempt attempt block reallocation to the correct size (or init with initial size)
			var->data = (char *) bprealloc((void *) var->data, (var->data_size + data_size));
			if(!var->data)
				return ERR_FAILURE;

			// if the data was allocated, copy it in
			if(var->data)
			{

				// copy in the data
				memcpy(&var->data[var->data_size], data, data_size);

				// set data size only if we make a copy
				var->data_size += data_size;

			}

		}

	}



	// return indicating success
	return ERR_SUCCESS;


}


// Marks a variable as ready (flag within var request structure)
BP_ERROR_T BP_HTTPdRequestVarMarkReady(P_BP_HTTPD_REQUEST_VAR request_var)
{

	// ensure we have a request var
	if(!request_var)
		return ERR_FAILURE;

	// mark the request as ready
	request_var->ready = BP_TRUE;

	// return indicating success
	return ERR_SUCCESS;

}


// Attempts to lookup an argument from the argument registry
P_BP_HTTPD_REQUEST_VAR BP_HTTPdRequestVarArgRegistryLookup
(
		P_BP_HTTPD_REQUEST             request,
		BP_HTTPD_REQUEST_VAR_TYPE      type,
		char *                         name
)
{

	// ensure we have a request and a registry
	if(!request)
		return NULL;
	if(!request->arg_hreg)
		return NULL;
	if(!name)
		return NULL;
	if(!BP_StringIsReasonablePrintableString(name, bpstrlen(name), BP_FALSE, BP_FALSE))
		return NULL;

	// variable table selected
	char * var_table = NULL;

	// switch on the type
	switch(type)
	{

		// http post tables
		case BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA:
			var_table = "POST";
			break;
		case BP_HTTPD_REQUEST_VAR_TYPE_POST_TMPFILE:
			var_table = "POST";
			break;

		// header table
		case BP_HTTPD_REQUEST_VAR_TYPE_HEADER:
			var_table = "HEADER";
			break;

		// http get table
		case BP_HTTPD_REQUEST_VAR_TYPE_GET:
			var_table = "GET";
			break;

		// cookie table
		case BP_HTTPD_REQUEST_VAR_TYPE_COOKIE:
			var_table = "COOKIE";
			break;

		// exit immediately if we have a type mismatch
		default:
			return NULL;
	}

	// ensure we have a table selected
	if(!var_table)
		return NULL;



	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// set initial path
	table_path[0] = var_table;
	table_path[1] = name;
	table_path[2] = NULL;

	// enter critical section for the argument registry
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(request->arg_hreg);

	// attempt lookup
	tmp_lookup = BP_HashRegLookupTableEntryByPath(request->arg_hreg, table_path);

	// ensure we have a lookup
	if(!tmp_lookup)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(request->arg_hreg, NULL);

	// ensure there is data to return
	if(!tmp_lookup->data)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(request->arg_hreg, NULL);

	// set the return val
	P_BP_HTTPD_REQUEST_VAR retval = (P_BP_HTTPD_REQUEST_VAR) tmp_lookup->data;

	// leave the critical section for the argument registry
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(request->arg_hreg);

	// return the value
	return retval;

}

// Attempts to lookup a request variable using case insensitive search. (requires
// linear search iterating through request table, slower than non case insensitive version)
P_BP_HTTPD_REQUEST_VAR BP_HTTPdRequestVarArgRegistryLookupCaseInsensitive
(
		P_BP_HTTPD_REQUEST             request,
		BP_HTTPD_REQUEST_VAR_TYPE      type,
		char *                         name
)
{

	// ensure we have a request and a registry
	if(!request)
		return NULL;
	if(!request->arg_hreg)
		return NULL;
	if(!name)
		return NULL;
	if(!BP_StringIsReasonablePrintableString(name, bpstrlen(name), BP_FALSE, BP_FALSE))
		return NULL;

	// variable table selected
	char * var_table = NULL;

	// switch on the type
	switch(type)
	{

		// http post tables
		case BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA:
			var_table = "POST";
			break;
		case BP_HTTPD_REQUEST_VAR_TYPE_POST_TMPFILE:
			var_table = "POST";
			break;

		// header table
		case BP_HTTPD_REQUEST_VAR_TYPE_HEADER:
			var_table = "HEADER";
			break;

		// http get table
		case BP_HTTPD_REQUEST_VAR_TYPE_GET:
			var_table = "GET";
			break;

		// cookie table
		case BP_HTTPD_REQUEST_VAR_TYPE_COOKIE:
			var_table = "COOKIE";
			break;

		// exit immediately if we have a type mismatch
		default:
			return NULL;
	}

	// ensure we have a table selected
	if(!var_table)
		return NULL;

	// create lowercase name
	char * lower_case_name = BP_StringToLowerCase(name, bpstrlen(name));
	if(!lower_case_name)
		return NULL;


	// enter critical section for the argument registry
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(request->arg_hreg);

	// attempt to lookup the request table
	P_BP_HTKE request_table_lookup = BP_HashRegLookupTableByPath
	(
			request->arg_hreg,
			BP_PATH_OPEN
				var_table
			BP_PATH_CLOSE
	);

	// ensure the lookup is a table
	if(request_table_lookup->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(request->arg_hreg, NULL);

	// ensure the table has entries
	if(!request_table_lookup->data)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(request->arg_hreg, NULL);

	// request var passed back
	P_BP_HTTPD_REQUEST_VAR ret_request_var = NULL;

	// iterate through the request table which was looked up
	BP_HASH_ITER_OPEN((P_BP_HTKE) request_table_lookup->data, var_iter)
	{

		printf("\n here 0");
		// ensure the entry is not a table
		if(ht_entry_is_table(var_iter))
			continue;
		printf("\n here 1.5");
		// ensure the entry is carrying data
		if(!var_iter->data)
			continue;

		// create lowercase key and name
		char *tmp_lowercase_key  = BP_StringToLowerCase(var_iter->key, bpstrlen(var_iter->key));
		char *tmp_lowercase_name = BP_StringToLowerCase(name, bpstrlen(name));

		printf("\n here 1");

		// ensure we have both parts
		if(!tmp_lowercase_key)
			continue;
		if(!tmp_lowercase_name)
			continue;

		// ensure the strings match as reasonable strings
		if(BP_StringsMatchAsReasonableStrings(tmp_lowercase_key, tmp_lowercase_name))
		{

			bpfree(tmp_lowercase_key);
			bpfree(tmp_lowercase_name);

			// return the request variable
			ret_request_var = (P_BP_HTTPD_REQUEST_VAR) var_iter->data;
			break;

		}

		// reset lower case vars on loop
		bpfree(tmp_lowercase_key);
		bpfree(tmp_lowercase_name);


	}
	BP_HASH_ITER_CLOSE;

	// leave critical section before returning
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(request->arg_hreg);

	// return the value
	return ret_request_var;

}
// Destroy a request var members and close file descriptors if necessary.
BP_ERROR_T BP_HTTPdRequestVarDestroyMembers(P_BP_HTTPD_REQUEST_VAR request_var)
{

	// ensure we have request
	if(!request_var)
		return ERR_FAILURE;

	// destroy data if set
	if(request_var->data)
	{
		bpfree(request_var->data);
		request_var->data = NULL;
	}

	// destroy data file if set
	if(request_var->data_file)
	{
		bpfree(request_var->data_file);
		request_var->data_file = NULL;
	}

	// destroy file descriptor if set
	if(request_var->file_descriptor != -1)
	{

		// close the file descriptor if one is set
		BP_FDClose(request_var->file_descriptor);
		request_var->file_descriptor = -1;
	}

	// destroy name if set
	if(request_var->name)
	{
		bpfree(request_var->name);
		request_var->name = NULL;
	}

	// return indicating success
	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Var Type Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// simply checks the argument tables and determines whether or not a request has arguments of the
// specified type.
BP_BOOL BP_HTTPdRequestContainsArgumentType
(
	P_BP_HTTPD_REQUEST             request,
	BP_HTTPD_REQUEST_VAR_TYPE      type
)
{

	// run basic null checks
	if(!request)
		return BP_FALSE;
	if(!request->arg_hreg)
		return BP_FALSE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Lookup Path %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set lookup path
	char *lookup_path[10];
	BP_ZERO_STACK(lookup_path);


	// switch on the provided type
	switch(type)
	{

		case BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA:
			lookup_path[0] = "POST";
			break;
		case BP_HTTPD_REQUEST_VAR_TYPE_GET:
			lookup_path[0] = "GET";
			break;
		case BP_HTTPD_REQUEST_VAR_TYPE_COOKIE:
			lookup_path[0] = "COOKIE";
			break;
		case BP_HTTPD_REQUEST_VAR_TYPE_HEADER:
			lookup_path[0] = "HEADER";
			break;
		default:
			break;
	}

	// attempt to lookup the entry
	P_BP_HASH_TABLE_KEYED_ENTRY lookup_entry = BP_HashRegLookupTableByPath(request->arg_hreg, (char **) &lookup_path);
	if(!lookup_entry)
		return BP_FALSE;

	// if there are no entries, the data member will be set to null
	if(!lookup_entry->data)
		return BP_FALSE;

	// return indicating that there are entries
	return BP_TRUE;

}


// Request contains POST arguments.
BP_BOOL BP_HTTPdRequestContainsPOST(P_BP_HTTPD_REQUEST request)
{

	return BP_HTTPdRequestContainsArgumentType
	(
		request,
		BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA
	);

}

// Request contains GET arguments.
BP_BOOL BP_HTTPdRequestContainsGET(P_BP_HTTPD_REQUEST request)
{
	return BP_HTTPdRequestContainsArgumentType
	(
		request,
		BP_HTTPD_REQUEST_VAR_TYPE_GET
	);
}

// Request contains sent headers.
BP_BOOL BP_HTTPdRequestContainsHEADER(P_BP_HTTPD_REQUEST request)
{
	return BP_HTTPdRequestContainsArgumentType
	(
		request,
		BP_HTTPD_REQUEST_VAR_TYPE_HEADER
	);
}

// Request contains sent cookie values.
BP_BOOL BP_HTTPdRequestContainsCOOKIE(P_BP_HTTPD_REQUEST request)
{
	return BP_HTTPdRequestContainsArgumentType
	(
		request,
		BP_HTTPD_REQUEST_VAR_TYPE_COOKIE
	);
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Request Exit/Cleanup %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// THIS FUNCTIONALITY IS IMPLEMENTED VIA MACROS IN BP-HTTPdHelperMacros.h.

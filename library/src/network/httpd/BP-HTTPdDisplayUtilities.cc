/*
 * BP-HTTPdDisplayUtilities.cc
 *
 *  Created on: Jul 17, 2015
 *      Author: root
 */


#include "../../../include/BP-Main.h"

// Display an entire httpd session
BP_ERROR_T BP_HTTPdDisplay(P_BP_HTTPD_SESSION httpd, BP_BOOL display_system_registry, BP_BOOL display_request_registry, BP_BOOL display_request_details)
{

	// ensure we have a session pointer
	if(!httpd)
		return ERR_FAILURE;

	printf("\n [+] HTTPd Session %p (%s)", httpd, httpd->identifier);
	printf("\n\t identifier:      %s", httpd->identifier);
	printf("\n\t run_state:       ");
	switch(httpd->run_state)
	{

		case BP_HTTPD_SERVER_STATE_STARTED:
			printf("BP_HTTPD_SERVER_STATE_STARTED");
			break;
		case BP_HTTPD_SERVER_STATE_QUIETED:
			printf("BP_HTTPD_SERVER_STATE_QUIETED");
			break;
		case BP_HTTPD_SERVER_STATE_STOPPED:
			printf("BP_HTTPD_SERVER_STATE_STOPPED");
			break;
		case BP_HTTPD_SERVER_STATE_UNSET:
			printf("BP_HTTPD_SERVER_STATE_UNSET");
			break;
		default:
			printf("BADVAL");
			break;
	}

	printf("\n\t daemon:          %p", httpd->daemon);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Start Flags %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	printf("\n\t flags:           ");

	if(httpd->start_flags & MHD_USE_DEBUG)
		printf("MHD_USE_DEBUG ");

	if(httpd->start_flags & MHD_USE_SSL)
		printf("MHD_USE_SSL ");

	if(httpd->start_flags & MHD_USE_SELECT_INTERNALLY)
		printf("MHD_USE_SELECT_INTERNALLY ");

	if(httpd->start_flags & MHD_USE_THREAD_PER_CONNECTION)
		printf("MHD_USE_THREAD_PER_CONNECTION ");

	// port
	printf("\n\t port:            %u", httpd->port);

	// apc callback
	printf("\n\t apc:             %p", httpd->apc);
	printf("\n\t apc_cls:         %p", httpd->apc_cls);

	// default access handler callback
	printf("\n\t dh:              %p", httpd->dh);
	printf("\n\t dh_cls:          %p", httpd->dh_cls);

	// key pem
	printf("\n\t key_pem:         %p", httpd->key_pem);

	// cert pem
	printf("\n\t cert_pem:        %p", httpd->cert_pem);

	printf("\n\t runtime_options: %p", httpd->runtime_options);

	printf("\n\t system_reg:      %p", httpd->system_hreg);

	printf("\n\t request_hreg:    %p", httpd->request_hreg);

	printf("\n\t event_hreg:      %p", httpd->event_hreg);


	// display the system registry if the user requested it
	if(display_system_registry)
	{

		// display users
		BP_HTTPdDisplayUsers(httpd);

		// display sessions
		BP_HTTPdDisplaySessions(httpd);

		// Display request handlers
		BP_HTTPdDisplayRequestHandlers(httpd);

		// display requests
		BP_HTTPdDisplayRequests(httpd);

	}

	// return indicating success
	return ERR_SUCCESS;

}


// --- System Registry Display Routines ----------

BP_ERROR_T BP_HTTPdDisplayUsers(P_BP_HTTPD_SESSION httpd)
{

	// ensure we have a httpd and a system registry
	if(!httpd)
		return ERR_FAILURE;
	if(!httpd->system_hreg)
		return ERR_FAILURE;

	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// set the table path
	table_path[0] = "users";
	table_path[1] = NULL;

	// enter system hreg critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(httpd->system_hreg);

	// lookup the post table
	tmp_lookup = BP_HashRegLookupTableByPath(httpd->system_hreg, table_path);

	// iterate through users
	if(tmp_lookup->data)
	{


		// add header
		printf("\n\n\t\t [+] Displaying Users");

		// move the tmp lookup to the hash table children (since it's a table)
		tmp_lookup = (P_BP_HASH_TABLE_KEYED_ENTRY) tmp_lookup->data;

		// temporary iterator values used during the HASH_ITER loop below
		P_BP_HASH_TABLE_KEYED_ENTRY entry = NULL;
		P_BP_HASH_TABLE_KEYED_ENTRY tmp   = NULL;

		// iterate through all hashes and display
		HASH_ITER(hh, tmp_lookup, entry, tmp)
		{

			// set variable
			P_BP_HTTPD_USER user = (P_BP_HTTPD_USER) entry->data;

			// if the var is set, destroy its members
			if(user)
			{

				// display user (add newline before each entry)
				printf("\n");
				BP_HTTPdDisplayUser(user, BP_FALSE);

			}

		}

	}

	// printf("\n SYSTEM REG HERE:");
	// BP_HashRegDisplay(httpd->system_hreg, BP_FALSE, BP_TRUE);


	// leave system hreg critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(httpd->system_hreg);


	return ERR_SUCCESS;
}


// Display Requests in the system registry
BP_ERROR_T BP_HTTPdDisplayRequests(P_BP_HTTPD_SESSION httpd)
{

	// ensure we have a httpd and a system registry
	if(!httpd)
		return ERR_FAILURE;
	if(!httpd->request_hreg)
		return ERR_FAILURE;

	// printf("\n REQUEST HREG HERE:");
	// BP_HashRegDisplay(httpd->request_hreg, BP_FALSE, BP_TRUE);

	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// set the table path
	table_path[0] = "requests";
	table_path[1] = NULL;

	// enter request hreg critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(httpd->request_hreg);

	// lookup the post table
	tmp_lookup = BP_HashRegLookupTableByPath(httpd->request_hreg, table_path);

	// iterate through users
	if(tmp_lookup->data)
	{


		// add header
		printf("\n\n\t\t [+] Requests");

		// move the tmp lookup to the hash table children (since it's a table)
		tmp_lookup = (P_BP_HASH_TABLE_KEYED_ENTRY) tmp_lookup->data;

		// temporary iterator values used during the HASH_ITER loop below
		P_BP_HASH_TABLE_KEYED_ENTRY entry = NULL;
		P_BP_HASH_TABLE_KEYED_ENTRY tmp   = NULL;

		// iterate through all hashes and display
		HASH_ITER(hh, tmp_lookup, entry, tmp)
		{

			// set variable
			P_BP_HTTPD_REQUEST request = (P_BP_HTTPD_REQUEST) entry->data;

			// if the request handler is set, display it
			if(request)
			{

				// display user (add newline before each entry)
				printf("\n");
				BP_HTTPdDisplayRequest(request);
				// BP_HTTPdDisplayUser(user, BP_FALSE);

			}

		}

	}

	// leave request hreg critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(httpd->request_hreg);

	// return indicating success
	return ERR_SUCCESS;

}

// Display request handlers within the system registry
BP_ERROR_T BP_HTTPdDisplayRequestHandlers(P_BP_HTTPD_SESSION httpd)
{

	// ensure we have a httpd and a system registry
	if(!httpd)
		return ERR_FAILURE;
	if(!httpd->system_hreg)
		return ERR_FAILURE;

	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// set the table path
	table_path[0] = "request_handlers";
	table_path[1] = NULL;

	// enter system hreg critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(httpd->system_hreg);

	// lookup the post table
	tmp_lookup = BP_HashRegLookupTableByPath(httpd->system_hreg, table_path);

	// iterate through users
	if(tmp_lookup->data)
	{


		// add header
		printf("\n\n\t\t [+] Request Handlers");

		// move the tmp lookup to the hash table children (since it's a table)
		tmp_lookup = (P_BP_HASH_TABLE_KEYED_ENTRY) tmp_lookup->data;

		// temporary iterator values used during the HASH_ITER loop below
		P_BP_HASH_TABLE_KEYED_ENTRY entry = NULL;
		P_BP_HASH_TABLE_KEYED_ENTRY tmp   = NULL;

		// iterate through all hashes and display
		HASH_ITER(hh, tmp_lookup, entry, tmp)
		{

			// set variable
			P_BP_HTTPD_REQUEST_HANDLER request_handler = (P_BP_HTTPD_REQUEST_HANDLER) entry->data;

			// if the request handler is set, display it
			if(request_handler)
			{

				// display user (add newline before each entry)
				printf("\n");
				BP_HTTPdDisplayRequestHandler(request_handler);
				// BP_HTTPdDisplayUser(user, BP_FALSE);

			}

		}

	}

	// leave system hreg critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(httpd->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}


// Display sessions within the system registry
BP_ERROR_T BP_HTTPdDisplaySessions(P_BP_HTTPD_SESSION httpd)
{

	// ensure we have a httpd and a system registry
	if(!httpd)
		return ERR_FAILURE;
	if(!httpd->system_hreg)
		return ERR_FAILURE;

	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// set the table path
	table_path[0] = "sessions";
	table_path[1] = NULL;

	// enter system hreg critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(httpd->system_hreg);

	// lookup the post table
	tmp_lookup = BP_HashRegLookupTableByPath(httpd->system_hreg, table_path);

	// iterate through users
	if(tmp_lookup->data)
	{


		// add header
		printf("\n\n\t\t [+] Displaying User Sessions");

		// move the tmp lookup to the hash table children (since it's a table)
		tmp_lookup = (P_BP_HASH_TABLE_KEYED_ENTRY) tmp_lookup->data;

		// temporary iterator values used during the HASH_ITER loop below
		P_BP_HASH_TABLE_KEYED_ENTRY entry = NULL;
		P_BP_HASH_TABLE_KEYED_ENTRY tmp   = NULL;

		// iterate through all hashes and display
		HASH_ITER(hh, tmp_lookup, entry, tmp)
		{

			// set variable
			P_BP_HTTPD_USER_SESSION user_session = (P_BP_HTTPD_USER_SESSION) entry->data;

			// if the var is set, destroy its members
			if(user_session)
			{

				// display user (add newline before each entry)
				printf("\n");
				BP_HTTPdDisplayUserSession(user_session);

			}

		}

	}

	// printf("\n SYSTEM REG HERE:");
	// BP_HashRegDisplay(httpd->system_hreg, BP_FALSE, BP_TRUE);


	// leave system hreg critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(httpd->system_hreg);


	// return indicating success
	return ERR_SUCCESS;

}


// Display log within the system registry
BP_ERROR_T BP_HTTPdDisplayLog(P_BP_HTTPD_SESSION httpd)
{


	// return indicating success
	return ERR_SUCCESS;

}

// --- Specific Datatype Display Routines ---

// display a user
BP_ERROR_T BP_HTTPdDisplayUser(P_BP_HTTPD_USER user, BP_BOOL redact)
{

	if(!user)
		return ERR_FAILURE;

	printf("\n\t\t\t [+] user: %s", user->username);
	printf("\n\t\t\t     bio:           %s", user->bio);
	printf("\n\t\t\t     email:         %s", user->email);
	printf("\n\t\t\t     password_hash: %p", user->password_hash);
	printf("\n\t\t\t     password_salt: %p", user->password_salt);
	printf("\n\t\t\t     hash_type:     ");

	switch(user->hash_type)
	{

		case BP_HTTPD_USER_PASSWORD_HASH_TYPE_UNSET:
			printf("BP_HTTPD_USER_PASSWORD_HASH_TYPE_UNSET");
			break;

		case BP_HTTPD_USER_PASSWORD_HASH_TYPE_MD5:
			printf("BP_HTTPD_USER_PASSWORD_HASH_TYPE_MD5");
			break;

		case BP_HTTPD_USER_PASSWORD_HASH_TYPE_SHA1:
			printf("BP_HTTPD_USER_PASSWORD_HASH_TYPE_SHA1");
			break;

		case BP_HTTPD_USER_PASSWORD_HASH_TYPE_SHA256:
			printf("BP_HTTPD_USER_PASSWORD_HASH_TYPE_SHA256");
			break;

		default:
			printf("BADVAL");
			break;
	}


	printf("\n\t\t\t     role:          %s", user->role);
	printf("\n\t\t\t     type:          ");

	switch(user->type)
	{

		case BP_HTTPD_USER_TYPE_GLOBALDB:
			printf("BP_HTTPD_USER_TYPE_GLOBALDB");
			break;

		case BP_HTTPD_USER_TYPE_LOCAL:
			printf("BP_HTTPD_USER_TYPE_LOCAL");
			break;

		case BP_HTTPD_USER_TYPE_UNSET:
			printf("BP_HTTPD_USER_TYPE_UNSET");
			break;

		default:
			printf("BADVAL");
			break;
	}


	printf("\n\t\t\t     access:        ");
	switch(user->access)
	{
		case BP_HTTPD_USER_ACCESS_UNSET:
			printf("BP_HTTPD_USER_ACCESS_UNSET");
			break;
		case BP_HTTPD_USER_ACCESS_GOD:
			printf("BP_HTTPD_USER_ACCESS_GOD");
			break;
		case BP_HTTPD_USER_ACCESS_DEMIGOD:
			printf("BP_HTTPD_USER_ACCESS_DEMIGOD");
			break;
		case BP_HTTPD_USER_ACCESS_SEMIGOD:
			printf("BP_HTTPD_USER_ACCESS_SEMIGOD");
			break;
		case BP_HTTPD_USER_ACCESS_MORTAL:
			printf("BP_HTTPD_USER_ACCESS_MORTAL");
			break;
		case BP_HTTPD_USER_ACCESS_GUEST:
			printf("BP_HTTPD_USER_ACCESS_GUEST");
			break;
		default:
			printf("BADVAL");
			break;
	}

	return ERR_SUCCESS;
}

// display a request handler
BP_ERROR_T BP_HTTPdDisplayRequestHandler(P_BP_HTTPD_REQUEST_HANDLER request_handler)
{

	if(!request_handler)
		return ERR_FAILURE;


	printf("\n\t\t\t [+] request_handler %p - %s", request_handler, request_handler->handler_path);
	printf("\n\t\t\t     handler_path:                           %s", request_handler->handler_path);
	printf("\n\t\t\t     handler:                                %p", request_handler->handler);
	printf("\n\t\t\t     supported_http_methods:                 ");

	// check GET
	if(BP_FLAG_IS_SET(request_handler->supported_http_methods, BP_HTTPD_METHOD_GET))
		printf("BP_HTTPD_METHOD_GET ");

	// check POST
	if(BP_FLAG_IS_SET(request_handler->supported_http_methods, BP_HTTPD_METHOD_POST))
		printf("BP_HTTPD_METHOD_POST ");

	// check PUT
	if(BP_FLAG_IS_SET(request_handler->supported_http_methods, BP_HTTPD_METHOD_PUT))
		printf("BP_HTTPD_METHOD_PUT ");

	// check CONNECT
	if(BP_FLAG_IS_SET(request_handler->supported_http_methods, BP_HTTPD_METHOD_CONNECT))
		printf("BP_HTTPD_METHOD_CONNECT ");

	// check DELETE
	if(BP_FLAG_IS_SET(request_handler->supported_http_methods, BP_HTTPD_METHOD_DELETE))
		printf("BP_HTTPD_METHOD_DELETE ");

	// check HEAD
	if(BP_FLAG_IS_SET(request_handler->supported_http_methods, BP_HTTPD_METHOD_HEAD))
		printf("BP_HTTPD_METHOD_HEAD ");

	// check OPTIONS
	if(BP_FLAG_IS_SET(request_handler->supported_http_methods, BP_HTTPD_METHOD_OPTIONS))
		printf("BP_HTTPD_METHOD_OPTIONS ");

	// check TRACE
	if(BP_FLAG_IS_SET(request_handler->supported_http_methods, BP_HTTPD_METHOD_TRACE))
		printf("BP_HTTPD_METHOD_TRACE ");



	printf("\n\t\t\t     posted_max_data_size_before_autofail:   %u", request_handler->posted_max_data_size_before_autofail);
	printf("\n\t\t\t     posted_data_chunk_read_size:            %u", request_handler->posted_data_chunk_read_size);
	printf("\n\t\t\t     posted_data_tmp_directory_literal_path: %s", request_handler->posted_data_tmp_directory_literal_path);
	printf("\n\t\t\t     use_tmpfile_for_multipart_form_data:    %s", request_handler->use_tmpfile_for_multipart_form_data ? "yes" : "no");
	printf("\n\t\t\t     use_tmpfile_for_octet_stream:           %s", request_handler->use_tmpfile_for_octet_stream ? "yes" : "no");
	printf("\n\t\t\t     activation_n:                           %u", request_handler->activation_n);
	printf("\n\t\t\t     error_n:                                %u", request_handler->error_n);
	printf("\n\t\t\t     completed_n:                            %u", request_handler->completed_n);
	// return indicating success
	return ERR_SUCCESS;

}

// display request
BP_ERROR_T BP_HTTPdDisplayRequest(P_BP_HTTPD_REQUEST request)
{

	// ensure we have a request to display
	if(!request)
		return ERR_FAILURE;

	printf("\n\t\t\t [+] request %p", request);
	printf("\n\t\t\t     uuid:                                   %s", request->uuid);
	printf("\n\t\t\t     httpd:                                  %p", request->httpd);
	printf("\n\t\t\t     request_handler:                        %p - %s", request->request_handler, request->request_handler ? request->request_handler->handler_path : "unset");

	printf("\n\t\t\t     http_method:                            ");

	switch(request->http_method)
	{
	case BP_HTTPD_METHOD_UNSET:
		printf("BP_HTTPD_METHOD_UNSET");
		break;
	case BP_HTTPD_METHOD_GET:
		printf("BP_HTTPD_METHOD_GET");
		break;
	case BP_HTTPD_METHOD_POST:
		printf("BP_HTTPD_METHOD_POST");
		break;
	case BP_HTTPD_METHOD_PUT:
		printf("BP_HTTPD_METHOD_PUT");
		break;
	case BP_HTTPD_METHOD_CONNECT:
		printf("BP_HTTPD_METHOD_CONNECT");
		break;
	case BP_HTTPD_METHOD_DELETE:
		printf("BP_HTTPD_METHOD_DELETE");
		break;
	case BP_HTTPD_METHOD_HEAD:
		printf("BP_HTTPD_METHOD_HEAD");
		break;
	case BP_HTTPD_METHOD_OPTIONS:
		printf("BP_HTTPD_METHOD_OPTIONS");
		break;
	case BP_HTTPD_METHOD_TRACE:
		printf("BP_HTTPD_METHOD_TRACE");
		break;
	default:
		printf("BADVAL");
		break;
	}


	printf("\n\t\t\t     secure_client_request:                  %s", request->secure_client_request ? "yes" : "no");

	printf("\n\t\t\t     secure_client_proto_version:            ");

	switch(request->secure_client_proto_version)
	{

		case BP_HTTPD_REQUEST_TLS_VERSION_UNSET:
			printf("BP_HTTPD_REQUEST_TLS_VERSION_UNSET");
			break;

		case BP_HTTPD_REQUEST_TLS_VERSION_SSLV3:
			printf("BP_HTTPD_REQUEST_TLS_VERSION_SSLV3");
			break;

		case BP_HTTPD_REQUEST_TLS_VERSION_TLS1_0:
			printf("BP_HTTPD_REQUEST_TLS_VERSION_TLS1_0");
			break;

		case BP_HTTPD_REQUEST_TLS_VERSION_TLS1_1:
			printf("BP_HTTPD_REQUEST_TLS_VERSION_TLS1_1");
			break;

		case BP_HTTPD_REQUEST_TLS_VERSION_TLS1_2:
			printf("BP_HTTPD_REQUEST_TLS_VERSION_TLS1_2");
			break;

		default:
			printf("BADVAL");
			break;
	}

	printf("\n\t\t\t     hostname:                               %s", request->hostname);
	printf("\n\t\t\t     ip_address:                             %s", request->ip_address);
	printf("\n\t\t\t     arg_hreg:                               %p", request->arg_hreg);
	printf("\n\t\t\t     pp:                                     %p", request->pp);
	printf("\n\t\t\t     post_processing_started:                %s", request->post_processing_started ? "yes" : "no");
	printf("\n\t\t\t     post_processing_completed:              %s", request->post_processing_completed ? "yes" : "no");

	printf("\n\t\t\t     post_encoding:                          ");
	switch(request->post_encoding)
	{
		case BP_HTTPD_REQUEST_CONTENT_TYPE_UNSET:
			printf("BP_HTTPD_REQUEST_CONTENT_TYPE_UNSET");
			break;
		case BP_HTTPD_REQUEST_CONTENT_TYPE_APPLICATION_X_WWW_FORM_URLENCODED:
			printf("BP_HTTPD_REQUEST_CONTENT_TYPE_APPLICATION_X_WWW_FORM_URLENCODED");
			break;
		case BP_HTTPD_REQUEST_CONTENT_TYPE_APPLICATION_OCTET_STREAM:
			printf("BP_HTTPD_REQUEST_CONTENT_TYPE_APPLICATION_OCTET_STREAM");
			break;
		case BP_HTTPD_REQUEST_CONTENT_TYPE_MULTIPART_FORM_DATA:
			printf("BP_HTTPD_REQUEST_CONTENT_TYPE_MULTIPART_FORM_DATA");
			break;
		default:
			printf("BADVAL");
			break;
	}
	printf("\n\t\t\t     post_data_is_file:                      %s", request->post_data_is_file ? "yes":"no");
	printf("\n\t\t\t     post_uploaded_full_file_path:           %s", request->post_uploaded_full_file_path);
	printf("\n\t\t\t     request_is_valid:                       %s", request->request_is_valid ? "yes":"no");



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Request Parameters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we have an argument registry, attempt to display arguments
	if(request->arg_hreg)
	{

		BP_HTTPdDisplayRequestHeaderArgs(request);
		BP_HTTPdDisplayRequestGetArgs(request);
		BP_HTTPdDisplayRequestPostArgs(request);
		BP_HTTPdDisplayRequestCookieArgs(request);

	}

	// return indicating success
	return ERR_SUCCESS;

}

// display a user session structure
BP_ERROR_T BP_HTTPdDisplayUserSession(P_BP_HTTPD_USER_SESSION user_session)
{

	// ensure we have a session structure
	if(!user_session)
		return ERR_FAILURE;
	if(!user_session->user)
		return ERR_FAILURE;

	printf("\n\t\t\t [+] user_session %p (%s)", user_session, user_session->user->username);
	printf("\n\t\t\t     identifier:         %s", user_session->identifier);
	printf("\n\t\t\t     issued_time:        %u", user_session->issued_time);
	printf
	(
			"\n\t\t\t     life_in_seconds:    %u (%u elapsed)",
			user_session->life_in_seconds,
			(BP_TimeGetSecondsSinceUnixEpoch() - user_session->issued_time)
	);
	printf("\n\t\t\t     user:               %p", user_session->user);


	// return indicating success
	return ERR_SUCCESS;

}



// --- Request Argument Registry Display Routines ----------

// display request argument registry POST
BP_ERROR_T BP_HTTPdDisplayRequestPostArgs(P_BP_HTTPD_REQUEST request)
{
	// ensure we have a httpd and a system registry
	if(!request)
		return ERR_FAILURE;
	if(!request->arg_hreg)
		return ERR_FAILURE;

	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// set the table path
	table_path[0] = "POST";
	table_path[1] = NULL;

	// enter system hreg critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(request->arg_hreg);

	// lookup the post table
	tmp_lookup = BP_HashRegLookupTableByPath(request->arg_hreg, table_path);

	// iterate through users
	if(tmp_lookup->data)
	{


		// add header
		printf("\n\n\t\t\t [+] Posted Arguments");

		// move the tmp lookup to the hash table children (since it's a table)
		tmp_lookup = (P_BP_HASH_TABLE_KEYED_ENTRY) tmp_lookup->data;

		// temporary iterator values used during the HASH_ITER loop below
		P_BP_HASH_TABLE_KEYED_ENTRY entry = NULL;
		P_BP_HASH_TABLE_KEYED_ENTRY tmp   = NULL;

		// iterate through all hashes and display
		HASH_ITER(hh, tmp_lookup, entry, tmp)
		{

			// set variable
			P_BP_HTTPD_REQUEST_VAR request_var = (P_BP_HTTPD_REQUEST_VAR) entry->data;

			// if the request handler is set, display it
			if(request_var)
			{

				// display user (add newline before each entry)
				printf("\n");
				BP_HTTPdDisplayRequestVar(request_var);
				// BP_HTTPdDisplayUser(user, BP_FALSE);

			}

		}

	}

	// leave system hreg critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(request->arg_hreg);

	// return indicating success
	return ERR_SUCCESS;


}

// display request argument registry GET
BP_ERROR_T BP_HTTPdDisplayRequestGetArgs(P_BP_HTTPD_REQUEST request)
{

	// ensure we have a httpd and a system registry
	if(!request)
		return ERR_FAILURE;
	if(!request->arg_hreg)
		return ERR_FAILURE;

	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// set the table path
	table_path[0] = "GET";
	table_path[1] = NULL;

	// enter system hreg critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(request->arg_hreg);

	// lookup the post table
	tmp_lookup = BP_HashRegLookupTableByPath(request->arg_hreg, table_path);

	// iterate through users
	if(tmp_lookup->data)
	{


		// add header
		printf("\n\n\t\t\t [+] HTTP Get Arguments");

		// move the tmp lookup to the hash table children (since it's a table)
		tmp_lookup = (P_BP_HASH_TABLE_KEYED_ENTRY) tmp_lookup->data;

		// temporary iterator values used during the HASH_ITER loop below
		P_BP_HASH_TABLE_KEYED_ENTRY entry = NULL;
		P_BP_HASH_TABLE_KEYED_ENTRY tmp   = NULL;

		// iterate through all hashes and display
		HASH_ITER(hh, tmp_lookup, entry, tmp)
		{

			// set variable
			P_BP_HTTPD_REQUEST_VAR request_var = (P_BP_HTTPD_REQUEST_VAR) entry->data;

			// if the request handler is set, display it
			if(request_var)
			{

				// display user (add newline before each entry)
				printf("\n");
				BP_HTTPdDisplayRequestVar(request_var);
				// BP_HTTPdDisplayUser(user, BP_FALSE);

			}

		}

	}

	// leave system hreg critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(request->arg_hreg);

	// return indicating success
	return ERR_SUCCESS;

}

// display request argument registry COOKIE
BP_ERROR_T BP_HTTPdDisplayRequestCookieArgs(P_BP_HTTPD_REQUEST request)
{

	// ensure we have a httpd and a system registry
	if(!request)
		return ERR_FAILURE;
	if(!request->arg_hreg)
		return ERR_FAILURE;

	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// set the table path
	table_path[0] = "COOKIE";
	table_path[1] = NULL;

	// enter system hreg critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(request->arg_hreg);

	// lookup the post table
	tmp_lookup = BP_HashRegLookupTableByPath(request->arg_hreg, table_path);

	// iterate through users
	if(tmp_lookup->data)
	{


		// add header
		printf("\n\n\t\t\t [+] HTTP Cookie Arguments");

		// move the tmp lookup to the hash table children (since it's a table)
		tmp_lookup = (P_BP_HASH_TABLE_KEYED_ENTRY) tmp_lookup->data;

		// temporary iterator values used during the HASH_ITER loop below
		P_BP_HASH_TABLE_KEYED_ENTRY entry = NULL;
		P_BP_HASH_TABLE_KEYED_ENTRY tmp   = NULL;

		// iterate through all hashes and display
		HASH_ITER(hh, tmp_lookup, entry, tmp)
		{

			// set variable
			P_BP_HTTPD_REQUEST_VAR request_var = (P_BP_HTTPD_REQUEST_VAR) entry->data;

			// if the request handler is set, display it
			if(request_var)
			{

				// display user (add newline before each entry)
				printf("\n");
				BP_HTTPdDisplayRequestVar(request_var);
				// BP_HTTPdDisplayUser(user, BP_FALSE);

			}

		}

	}

	// leave system hreg critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(request->arg_hreg);

	// return indicating success
	return ERR_SUCCESS;

}


// display request argument registry HEADER
BP_ERROR_T BP_HTTPdDisplayRequestHeaderArgs(P_BP_HTTPD_REQUEST request)
{

	// ensure we have a httpd and a system registry
	if(!request)
		return ERR_FAILURE;
	if(!request->arg_hreg)
		return ERR_FAILURE;

	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// set the table path
	table_path[0] = "HEADER";
	table_path[1] = NULL;

	// enter system hreg critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(request->arg_hreg);

	// lookup the post table
	tmp_lookup = BP_HashRegLookupTableByPath(request->arg_hreg, table_path);

	// iterate through users
	if(tmp_lookup->data)
	{


		// add header
		printf("\n\n\t\t\t [+] HTTP Header Arguments");

		// move the tmp lookup to the hash table children (since it's a table)
		tmp_lookup = (P_BP_HASH_TABLE_KEYED_ENTRY) tmp_lookup->data;

		// temporary iterator values used during the HASH_ITER loop below
		P_BP_HASH_TABLE_KEYED_ENTRY entry = NULL;
		P_BP_HASH_TABLE_KEYED_ENTRY tmp   = NULL;

		// iterate through all hashes and display
		HASH_ITER(hh, tmp_lookup, entry, tmp)
		{

			// set variable
			P_BP_HTTPD_REQUEST_VAR request_var = (P_BP_HTTPD_REQUEST_VAR) entry->data;

			// if the request handler is set, display it
			if(request_var)
			{

				// display user (add newline before each entry)
				printf("\n");
				BP_HTTPdDisplayRequestVar(request_var);
				// BP_HTTPdDisplayUser(user, BP_FALSE);

			}

		}

	}

	// leave system hreg critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(request->arg_hreg);

	// return indicating success
	return ERR_SUCCESS;

}



// --- Display Request Argument -----------

// Display a single request variable
BP_ERROR_T BP_HTTPdDisplayRequestVar(P_BP_HTTPD_REQUEST_VAR request_var)
{

	// ensure we have a request variable
	if(!request_var)
		return ERR_FAILURE;

	printf("\n\n\t\t\t\t [+] Argument %p (%s)", request_var, request_var->name);


	printf("\n\t\t\t\t\t content_type:             ");
	switch(request_var->content_type)
	{
		case BP_HTTPD_REQUEST_CONTENT_TYPE_UNSET:
			printf("BP_HTTPD_REQUEST_CONTENT_TYPE_UNSET");
			break;
		case BP_HTTPD_REQUEST_CONTENT_TYPE_APPLICATION_X_WWW_FORM_URLENCODED:
			printf("BP_HTTPD_REQUEST_CONTENT_TYPE_APPLICATION_X_WWW_FORM_URLENCODED");
			break;
		case BP_HTTPD_REQUEST_CONTENT_TYPE_APPLICATION_OCTET_STREAM:
			printf("BP_HTTPD_REQUEST_CONTENT_TYPE_APPLICATION_OCTET_STREAM");
			break;
		case BP_HTTPD_REQUEST_CONTENT_TYPE_MULTIPART_FORM_DATA:
			printf("BP_HTTPD_REQUEST_CONTENT_TYPE_MULTIPART_FORM_DATA");
			break;
		default:
			printf("BADVAL");
			break;
	}

	printf("\n\t\t\t\t\t type:                     ");
	switch(request_var->type)
	{

		case BP_HTTPD_REQUEST_VAR_TYPE_UNSET:
			printf("BP_HTTPD_REQUEST_VAR_TYPE_UNSET");
			break;
		case BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA:
			printf("BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA");
			break;
		case BP_HTTPD_REQUEST_VAR_TYPE_POST_TMPFILE:
			printf("BP_HTTPD_REQUEST_VAR_TYPE_POST_TMPFILE");
			break;
		case BP_HTTPD_REQUEST_VAR_TYPE_GET:
			printf("BP_HTTPD_REQUEST_VAR_TYPE_GET");
			break;
		case BP_HTTPD_REQUEST_VAR_TYPE_COOKIE:
			printf("BP_HTTPD_REQUEST_VAR_TYPE_COOKIE");
			break;
		case BP_HTTPD_REQUEST_VAR_TYPE_HEADER:
			printf("BP_HTTPD_REQUEST_VAR_TYPE_HEADER");
			break;
		default:
			printf("BADVAL");
			break;

	}

	printf("\n\t\t\t\t\t name:                     %s", request_var->name);
	printf("\n\t\t\t\t\t content_stored_in_file:   %s", request_var->content_stored_in_file ? "yes" : "no");
	printf("\n\t\t\t\t\t ready:                    %s", request_var->ready ? "yes" : "no");
	printf("\n\t\t\t\t\t data:                     %p", request_var->data);
	printf("\n\t\t\t\t\t data_file:                %p (%s)", request_var->data_file, request_var->data_file ? request_var->data_file : "unset");
	printf("\n\t\t\t\t\t data_size:                %u", request_var->data_size);
	printf("\n\t\t\t\t\t file_descriptor:          %i", request_var->file_descriptor);
	printf("\n\t\t\t\t\t file_handle_data_written: %u", request_var->file_handle_data_written);

	// return indicating success
	return ERR_SUCCESS;
}

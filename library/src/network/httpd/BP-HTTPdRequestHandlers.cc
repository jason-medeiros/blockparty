/*
 * BP-HTTPdRequestHandlers.cc
 *
 *  Created on: Jul 17, 2015
 *      Author: root
 */

#include "../../../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Request Handlers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Request handlers are stored within the httpd session hash table and
// are used to process individual routes to various functions.  For example
// to map a handler to a request you could do something like:
//
//   BP_HTTPdRequestHandl(httpd, "/new/", your_handler_function_here);
//
// Every time the /new/ path is identified in a request, the your_handler_function_here
// function will be executed.
//

// This adds a request handler to the httpd.
BP_ERROR_T BP_HTTPdRequestHandlerAdd
(
		P_BP_HTTPD_SESSION                       httpd,
		char *                                   handler_path,
		BP_HTTPdRequestHandlerFptr               handler,
		P_BP_HTTPD_REQUEST_HANDLER_SETUP_CONFIGS setup_configs,
		void *                                   external_handler_copy_parameter,
		size_t                                   external_handler_copy_parameter_size_n,
		BP_HTTPdLoginFormCallback                login_form_cb
)
{


	// ensure all parameters are non-null.
	if(!httpd)
		return ERR_FAILURE;
	if(!handler_path)
		return ERR_FAILURE;
	if(!handler)
		return ERR_FAILURE;
	if(!setup_configs)
		return ERR_FAILURE;

	// Ensure that if they passed in a tmp directory, it's reasonable.
	if(setup_configs->posted_data_tmp_directory_literal_path)
	{

		// ensure the string is a reasonable printable string
		if(!BP_StringIsReasonablePrintableString(setup_configs->posted_data_tmp_directory_literal_path, bpstrlen(setup_configs->posted_data_tmp_directory_literal_path), BP_FALSE, BP_FALSE))
			return ERR_FAILURE;


	}

	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check if User Exists %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section before making changes to the httpd hash
	// table registry.
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(httpd->system_hreg);

	// attempt to lookup user by username only (cannot have duplicate usernames)
	P_BP_HTTPD_REQUEST_HANDLER request_handler = BP_HTTPdRequestHandlerLookup
	(
		httpd,
		handler_path
	);

	// if the user already exists, exit immediately
	if(request_handler)
	{
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(httpd->system_hreg, ERR_FAILURE);
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Create New User Structure %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the initial path
	table_path[0] = "request_handlers";
	table_path[1] = NULL;

	// attempt to add data to the table
	BP_ERROR_T err_status = BP_HashRegAddDataToTableByPath
	(
		httpd->system_hreg,
		table_path,
		handler_path,
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_HTTPD_REQUEST_HANDLER)
	);

	// attempt to lookup the user after adding
	request_handler = BP_HTTPdRequestHandlerLookup(httpd, handler_path);
	if(!request_handler)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(httpd->system_hreg, ERR_FAILURE);



	// set the request handler (pointer 1 to 1 copy)
	request_handler->handler = handler;

	// set handler path
	request_handler->handler_path = bpstrdup(handler_path);

	// set the supported http methods
	request_handler->supported_http_methods = setup_configs->supported_http_methods;


	// set maximum data size
	request_handler->posted_max_data_size_before_autofail   =  setup_configs->posted_max_data_size_before_autofail;

	// set read size
	request_handler->posted_data_chunk_read_size            =  setup_configs->posted_data_chunk_read_size;



	// tmpfile on multipart form data
	request_handler->use_tmpfile_for_multipart_form_data = setup_configs->use_tmpfile_for_multipart_form_data;

	// tmpfile on octet stream data
	request_handler->use_tmpfile_for_octet_stream        = setup_configs->use_tmpfile_for_octet_stream;

	// set max key and val lengths for retrieving HTTP GET (hard limiter)
	request_handler->http_get_max_key_len = setup_configs->http_get_max_key_len;
	request_handler->http_get_max_val_len = setup_configs->http_get_max_val_len;

	// set cookie max lengths
	request_handler->http_cookie_max_key_len = setup_configs->http_cookie_max_key_len;
	request_handler->http_cookie_max_val_len = setup_configs->http_cookie_max_val_len;

	// set header max lengths
	request_handler->http_header_max_count = setup_configs->http_header_max_count;
	request_handler->http_header_max_key_len = setup_configs->http_header_max_key_len;
	request_handler->http_header_max_val_len = setup_configs->http_header_max_val_len;

	// set temp dir (value is checked on entry)
	if(setup_configs->posted_data_tmp_directory_literal_path)
	request_handler->posted_data_tmp_directory_literal_path =  bpstrdup(setup_configs->posted_data_tmp_directory_literal_path);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Login Callback %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set login callback if desired
	if(login_form_cb)
		request_handler->login_callback = login_form_cb;

	// set default if there is no other login callback
	if(!request_handler->login_callback)
		request_handler->login_callback = BP_HTTPdDefaultLoginFormCallback;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set External Data if Necessary %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure we have handler
	if(external_handler_copy_parameter &&  external_handler_copy_parameter_size_n)
	{

		// allocate memory via hash registry
		request_handler->external_handler_parameter = ht_calloc(httpd->system_hreg, external_handler_copy_parameter_size_n+1, 1);

		// copy in parameter
		memcpy(request_handler->external_handler_parameter, external_handler_copy_parameter, external_handler_copy_parameter_size_n);

		// set the external handler parameter size
		request_handler->external_handler_parameter_size_n = external_handler_copy_parameter_size_n;


	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Leave Critical Section and Exit %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(httpd->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}




// Attempts to lookup request handler within the table.
P_BP_HTTPD_REQUEST_HANDLER BP_HTTPdRequestHandlerLookup
(
		P_BP_HTTPD_SESSION httpd,
		char *             handler
)
{

	// ensure we have a session structure and a registry to work with
	if(!httpd)
		return NULL;
	if(!httpd->system_hreg)
		return NULL;


	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Sanity Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Ensure we have a handler
	if(!handler)
		return NULL;

	// check sanity on non-optional values
	if(BP_StringIsReasonablePrintableString(handler, bpstrlen(handler), BP_FALSE, BP_FALSE) != BP_TRUE)
		return NULL;




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check if User Exists %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section before making changes to the httpd hash
	// table registry.
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(httpd->system_hreg);


	// declare the lookup entry
	P_BP_HASH_TABLE_KEYED_ENTRY lookup_entry = NULL;

	// set the table path for lookup
	table_path[0] = "request_handlers";
	table_path[1] = handler;
	table_path[2] = NULL;

	// attempt to get entry
	lookup_entry = BP_HashRegLookupTableEntryByPath(httpd->system_hreg,table_path);

	// ensure we can lookup the new entry
	if(!lookup_entry)
	{
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(httpd->system_hreg, NULL);
	}

	// set request handler
	P_BP_HTTPD_REQUEST_HANDLER request_handler = (P_BP_HTTPD_REQUEST_HANDLER) lookup_entry->data;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Unlock and Exit %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section before exiting
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(httpd->system_hreg);

	// return null if the record cannot be found
	return request_handler;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Request Handler Auto Addition Routines %%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Add gif handlers from a provided directory.
BP_ERROR_T BP_HTTPdAddGIFHandlersFromFilesystemDir
(
	P_BP_HTTPD_SESSION httpd,
	char * load_dir,
	BP_BOOL recurse,
	BP_BOOL require_valid_user
)
{

	// ensure we have a http daemon
	if(!httpd)
		return ERR_FAILURE;

	// ensure we have a load directory
	if(!load_dir)
		return ERR_FAILURE;

	if(!BP_StringIsReasonablePrintableString(load_dir, bpstrlen(load_dir), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to open/crawl Filesystem Registry %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // Open a handle to the registry
	P_BP_FSREG fs_reg = BP_FSRegOpen(load_dir, BP_TRUE, 100);
	if(!fs_reg)
		return ERR_FAILURE;

	// enter the critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

    // set file policy here
    BP_FSREG_POLICY_FLAG_SET file_policy_flag_set[] =
    {
//          {BP_FS_FILE_POLICY_FLAG_IGNORE_ALL,                        BP_TRUE},
            {BP_FS_FILE_POLICY_FLAG_ONLY,                              BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_NOT_OWNED_BY_CURRENT_USER,         BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_OWNER_HAS_HIGHER_UID_THAN_CURRENT, BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_OWNER_HAS_LOWER_UID_THAN_CURRENT,  BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_OWNED_BY_CURRENT_USER,             BP_TRUE},
            {BP_FS_FILE_POLICY_FLAG_READABLE_BY_CURRENT_USER,          BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_WRITABLE_BY_CURRENT_USER,          BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_EXECUTABLE_BY_CURRENT_USER,        BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_SUID,                              BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_SGID,                              BP_TRUE},
            BP_FSREG_FLAG_SET_TERMINATOR
    };

    // attempt to set flags
    BP_FSRegPolicySetFlags
    (
            fs_reg,
            BP_FSREG_POLICY_TYPE_FILE,
            (P_BP_FSREG_POLICY_FLAG_SET) &file_policy_flag_set
    );

    // add filename regexp match
    BP_FSRegAddPregexPolicy(fs_reg, BP_FSREG_PREG_POLICY_TYPE_FNAME_MATCH, ".*[\\.][gG][iI][fF]$");

    // Crawl the filesystem for all files and fill out registry.
    BP_FSRegCrawl(fs_reg);



    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Process Crawl Returns %%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // Retrieve all files as a null ptr terminated char ** of reasonable/printable strings.
    char ** files = BP_FSRegRetrieveEntryFullPathsAsBPCallocedNullTerminatedPointerArray(fs_reg, NULL);
    if(!files)
    {

    	// leave critical section
    	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE;)

		// destroy the filesystem registry
		BP_DestroyHashTableRegistry(fs_reg->hreg);

    	return ERR_FAILURE;
    }


    // supported methods
	BP_FLAGS_T supported_methods = 0;

	// set flags
	BP_SET_FLAG(supported_methods, BP_HTTPD_METHOD_GET);

	// enable post if this requires a valid user
	if( require_valid_user == BP_TRUE)
	{
		BP_SET_FLAG(supported_methods, BP_HTTPD_METHOD_POST);
	}

    // setup configs
    BP_HTTPD_REQUEST_HANDLER_SETUP_CONFIGS js_path_setup_configs;

    // set configurations
    js_path_setup_configs.supported_http_methods                  = supported_methods;
    js_path_setup_configs.posted_max_data_size_before_autofail    = BP_KILOBYTE * 500;
    js_path_setup_configs.posted_data_chunk_read_size             = BP_KILOBYTE * 2;
    js_path_setup_configs.posted_data_tmp_directory_literal_path  = "/tmp/blockpartytmp/";
    js_path_setup_configs.use_tmpfile_for_multipart_form_data     = BP_TRUE;
    js_path_setup_configs.use_tmpfile_for_octet_stream            = BP_TRUE;
    js_path_setup_configs.http_get_max_key_len                    = 100;
    js_path_setup_configs.http_get_max_val_len                    = 100;
    js_path_setup_configs.http_cookie_max_key_len                 = 200;
    js_path_setup_configs.http_cookie_max_val_len                 = 200;
    js_path_setup_configs.http_header_max_count                   = 30;
    js_path_setup_configs.http_header_max_key_len                 = 100;
    js_path_setup_configs.http_header_max_val_len                 = 200;


    // load all files
    size_t n = 0;
    for(; files[n]; n++)
    {


    	if(bpstrlen(files[n]) >= bpstrlen(fs_reg->top_dir))
    	{

    		// tmp path
    		char * tmp_handler_path = &files[n][bpstrlen(fs_reg->top_dir)-1];

    		printf("\n Adding GIF request handler here?: %s ", files[n]);

    		// add the root request handler
			BP_HTTPdRequestHandlerAdd
			(
					httpd,
					tmp_handler_path,
					BP_GIFStaticRequestHandler,
					&js_path_setup_configs,
					files[n],
					bpstrlen(files[n]),
					NULL
			);

    	}

    }


	// leave the critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating success
	return ERR_SUCCESS;

}


// static request handler for handling png
BP_ERROR_T BP_HTTPdAddPNGHandlersFromFilesystemDir
(
	P_BP_HTTPD_SESSION httpd,
	char * load_dir,
	BP_BOOL recurse,
	BP_BOOL require_valid_user
)
{

	// ensure we have a http daemon
	if(!httpd)
		return ERR_FAILURE;

	// ensure we have a load directory
	if(!load_dir)
		return ERR_FAILURE;

	// ensure load is reasonable
	if(!BP_StringIsReasonablePrintableString(load_dir, bpstrlen(load_dir), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// ensure load dir is there
	if(!BP_StringIsReasonableForUnixEXT4Directory(load_dir, bpstrlen(load_dir), BP_TRUE))
		return ERR_FAILURE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to open/crawl Filesystem Registry %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // Open a handle to the registry
	P_BP_FSREG fs_reg = BP_FSRegOpen(load_dir, BP_TRUE, 100);
	if(!fs_reg)
		return ERR_FAILURE;

	// enter the critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

    // set file policy here
    BP_FSREG_POLICY_FLAG_SET file_policy_flag_set[] =
    {
//          {BP_FS_FILE_POLICY_FLAG_IGNORE_ALL,                        BP_TRUE},
            {BP_FS_FILE_POLICY_FLAG_ONLY,                              BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_NOT_OWNED_BY_CURRENT_USER,         BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_OWNER_HAS_HIGHER_UID_THAN_CURRENT, BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_OWNER_HAS_LOWER_UID_THAN_CURRENT,  BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_OWNED_BY_CURRENT_USER,             BP_TRUE},
            {BP_FS_FILE_POLICY_FLAG_READABLE_BY_CURRENT_USER,          BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_WRITABLE_BY_CURRENT_USER,          BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_EXECUTABLE_BY_CURRENT_USER,        BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_SUID,                              BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_SGID,                              BP_TRUE},
            BP_FSREG_FLAG_SET_TERMINATOR
    };

    // attempt to set flags
    BP_FSRegPolicySetFlags
    (
            fs_reg,
            BP_FSREG_POLICY_TYPE_FILE,
            (P_BP_FSREG_POLICY_FLAG_SET) &file_policy_flag_set
    );

    // add filename regexp match
    BP_FSRegAddPregexPolicy(fs_reg, BP_FSREG_PREG_POLICY_TYPE_FNAME_MATCH, ".*[\\.][pP][nN][gG]$");

    // Crawl the filesystem for all files and fill out registry.
    BP_FSRegCrawl(fs_reg);



    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Process Crawl Returns %%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // Retrieve all files as a null ptr terminated char ** of reasonable/printable strings.
    char ** files = BP_FSRegRetrieveEntryFullPathsAsBPCallocedNullTerminatedPointerArray(fs_reg, NULL);
    if(!files)
    {

    	// leave critical section
    	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE;)

		// destroy the filesystem registry
		BP_DestroyHashTableRegistry(fs_reg->hreg);

    	return ERR_FAILURE;
    }


    // supported methods
	BP_FLAGS_T supported_methods = 0;

	// set flags
	BP_SET_FLAG(supported_methods, BP_HTTPD_METHOD_GET);

	// enable post if this requires a valid user
	if( require_valid_user == BP_TRUE)
	{
		BP_SET_FLAG(supported_methods, BP_HTTPD_METHOD_POST);
	}

    // setup configs
    BP_HTTPD_REQUEST_HANDLER_SETUP_CONFIGS js_path_setup_configs;

    // set configurations
    js_path_setup_configs.supported_http_methods                  = supported_methods;
    js_path_setup_configs.posted_max_data_size_before_autofail    = BP_KILOBYTE * 500;
    js_path_setup_configs.posted_data_chunk_read_size             = BP_KILOBYTE * 2;
    js_path_setup_configs.posted_data_tmp_directory_literal_path  = "/tmp/blockpartytmp/";
    js_path_setup_configs.use_tmpfile_for_multipart_form_data     = BP_TRUE;
    js_path_setup_configs.use_tmpfile_for_octet_stream            = BP_TRUE;
    js_path_setup_configs.http_get_max_key_len                    = 100;
    js_path_setup_configs.http_get_max_val_len                    = 100;
    js_path_setup_configs.http_cookie_max_key_len                 = 200;
    js_path_setup_configs.http_cookie_max_val_len                 = 200;
    js_path_setup_configs.http_header_max_count                   = 30;
    js_path_setup_configs.http_header_max_key_len                 = 100;
    js_path_setup_configs.http_header_max_val_len                 = 200;


    // load all files
    size_t n = 0;
    for(; files[n]; n++)
    {


    	if(bpstrlen(files[n]) >= bpstrlen(fs_reg->top_dir))
    	{

    		// tmp path
    		char * tmp_handler_path = &files[n][bpstrlen(fs_reg->top_dir)-1];

    		printf("\n Adding PNG request handler here?: %s ", files[n]);


    		if( require_valid_user == BP_FALSE)
    		{
    			// add the root request handler
    			BP_HTTPdRequestHandlerAdd
				(
					httpd,
					tmp_handler_path,
					BP_PNGStaticRequestHandler,
					&js_path_setup_configs,
					files[n],
					bpstrlen(files[n]),
					NULL
				);
    		}
    		else if ( require_valid_user == BP_TRUE)
    		{
    			// add the root request handler
    			BP_HTTPdRequestHandlerAdd
				(
					httpd,
					tmp_handler_path,
					BP_PNGStaticRequestHandler_require_valid_user,
					&js_path_setup_configs,
					files[n],
					bpstrlen(files[n]),
					NULL
				);
    		}

    	}

    }


	// leave the critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating success
	return ERR_SUCCESS;

}




// This uses the blockparty filesystem registry to get a list of files which
// appear to be valid javascript, and loads them into memory buffers.  Once
// created, the buffers are used to serve dynamic javascript based on handler
// configuration.
BP_ERROR_T BP_HTTPdAddJavascriptHandlersFromFilesystemDir
(
	P_BP_HTTPD_SESSION httpd,
	char *             load_dir,
	BP_BOOL            recurse,
	BP_BOOL require_valid_user
)
{


	// ensure we have a http daemon
	if(!httpd)
		return ERR_FAILURE;

	// ensure we have a load directory
	if(!load_dir)
		return ERR_FAILURE;

	if(!BP_StringIsReasonablePrintableString(load_dir, bpstrlen(load_dir), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to open/crawl Filesystem Registry %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // Open a handle to the registry
	P_BP_FSREG fs_reg = BP_FSRegOpen(load_dir, BP_TRUE, 100);
	if(!fs_reg)
		return ERR_FAILURE;

	// enter the critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

    // set file policy here
    BP_FSREG_POLICY_FLAG_SET file_policy_flag_set[] =
    {
//          {BP_FS_FILE_POLICY_FLAG_IGNORE_ALL,                        BP_TRUE},
            {BP_FS_FILE_POLICY_FLAG_ONLY,                              BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_NOT_OWNED_BY_CURRENT_USER,         BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_OWNER_HAS_HIGHER_UID_THAN_CURRENT, BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_OWNER_HAS_LOWER_UID_THAN_CURRENT,  BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_OWNED_BY_CURRENT_USER,             BP_TRUE},
            {BP_FS_FILE_POLICY_FLAG_READABLE_BY_CURRENT_USER,          BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_WRITABLE_BY_CURRENT_USER,          BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_EXECUTABLE_BY_CURRENT_USER,        BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_SUID,                              BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_SGID,                              BP_TRUE},
            BP_FSREG_FLAG_SET_TERMINATOR
    };

    // attempt to set flags
    BP_FSRegPolicySetFlags
    (
            fs_reg,
            BP_FSREG_POLICY_TYPE_FILE,
            (P_BP_FSREG_POLICY_FLAG_SET) &file_policy_flag_set
    );

    // add filename regexp match
    BP_FSRegAddPregexPolicy(fs_reg, BP_FSREG_PREG_POLICY_TYPE_FNAME_MATCH, ".*[\\.][jJ][sS]$");

    // Crawl the filesystem for all files and fill out registry.
    BP_FSRegCrawl(fs_reg);



    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Process Crawl Returns %%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // Retrieve all files as a null ptr terminated char ** of reasonable/printable strings.
    char ** files = BP_FSRegRetrieveEntryFullPathsAsBPCallocedNullTerminatedPointerArray(fs_reg, NULL);
    if(!files)
    {

    	// leave critical section
    	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE;)

		// destroy the filesystem registry
		BP_DestroyHashTableRegistry(fs_reg->hreg);

    	return ERR_FAILURE;
    }


    // supported methods
	BP_FLAGS_T supported_methods = 0;

	// set flags
	BP_SET_FLAG(supported_methods, BP_HTTPD_METHOD_GET);

	// enable post if this requires a valid user
	if( require_valid_user == BP_TRUE)
	{
		BP_SET_FLAG(supported_methods, BP_HTTPD_METHOD_POST);
	}

    // setup configs
    BP_HTTPD_REQUEST_HANDLER_SETUP_CONFIGS js_path_setup_configs;

    // set configurations
    js_path_setup_configs.supported_http_methods                  = supported_methods;
    js_path_setup_configs.posted_max_data_size_before_autofail    = BP_KILOBYTE * 500;
    js_path_setup_configs.posted_data_chunk_read_size             = BP_KILOBYTE * 2;
    js_path_setup_configs.posted_data_tmp_directory_literal_path  = "/tmp/blockpartytmp/";
    js_path_setup_configs.use_tmpfile_for_multipart_form_data     = BP_TRUE;
    js_path_setup_configs.use_tmpfile_for_octet_stream            = BP_TRUE;
    js_path_setup_configs.http_get_max_key_len                    = 100;
    js_path_setup_configs.http_get_max_val_len                    = 100;
    js_path_setup_configs.http_cookie_max_key_len                 = 200;
    js_path_setup_configs.http_cookie_max_val_len                 = 200;
    js_path_setup_configs.http_header_max_count                   = 30;
    js_path_setup_configs.http_header_max_key_len                 = 100;
    js_path_setup_configs.http_header_max_val_len                 = 200;


    // load all files
    size_t n = 0;
    for(; files[n]; n++)
    {


    	if(bpstrlen(files[n]) >= bpstrlen(fs_reg->top_dir))
    	{

    		// tmp path
    		char * tmp_handler_path = &files[n][bpstrlen(fs_reg->top_dir)-1];




			if( require_valid_user == BP_FALSE)
			{

				// add the root request handler
				BP_HTTPdRequestHandlerAdd
				(
						httpd,
						tmp_handler_path,
						BP_JavascriptStaticRequestHandler,
						&js_path_setup_configs,
						files[n],
						bpstrlen(files[n]),
						NULL
				);

			}
			else if ( require_valid_user == BP_TRUE)
			{

				// add the root request handler
				BP_HTTPdRequestHandlerAdd
				(
						httpd,
						tmp_handler_path,
						BP_JavascriptStaticRequestHandler_require_valid_user,
						&js_path_setup_configs,
						files[n],
						bpstrlen(files[n]),
						NULL
				);

			}

    	}

    }


	// leave the critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating success
	return ERR_SUCCESS;


}


// This attempts to load all css files from a directory
BP_ERROR_T BP_HTTPdAddCSSHandlersFromFilesystemDir
(
	P_BP_HTTPD_SESSION httpd,
	char * load_dir,
	BP_BOOL recurse,
	BP_BOOL require_valid_user
)
{

	// ensure we have a http daemon
	if(!httpd)
		return ERR_FAILURE;

	// ensure we have a load directory
	if(!load_dir)
		return ERR_FAILURE;

	if(!BP_StringIsReasonablePrintableString(load_dir, bpstrlen(load_dir), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to open/crawl Filesystem Registry %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Open a handle to the registry
	P_BP_FSREG fs_reg = BP_FSRegOpen(load_dir, BP_TRUE, 100);
	if(!fs_reg)
		return ERR_FAILURE;

	// enter the critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// set file policy here
	BP_FSREG_POLICY_FLAG_SET file_policy_flag_set[] =
	{
//          {BP_FS_FILE_POLICY_FLAG_IGNORE_ALL,                        BP_TRUE},
			{BP_FS_FILE_POLICY_FLAG_ONLY,                              BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_NOT_OWNED_BY_CURRENT_USER,         BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_OWNER_HAS_HIGHER_UID_THAN_CURRENT, BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_OWNER_HAS_LOWER_UID_THAN_CURRENT,  BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_OWNED_BY_CURRENT_USER,             BP_TRUE},
			{BP_FS_FILE_POLICY_FLAG_READABLE_BY_CURRENT_USER,          BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_WRITABLE_BY_CURRENT_USER,          BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_EXECUTABLE_BY_CURRENT_USER,        BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_SUID,                              BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_SGID,                              BP_TRUE},
			BP_FSREG_FLAG_SET_TERMINATOR
	};

	// attempt to set flags
	BP_FSRegPolicySetFlags
	(
			fs_reg,
			BP_FSREG_POLICY_TYPE_FILE,
			(P_BP_FSREG_POLICY_FLAG_SET) &file_policy_flag_set
	);

	// add filename regexp match
	BP_FSRegAddPregexPolicy(fs_reg, BP_FSREG_PREG_POLICY_TYPE_FNAME_MATCH, ".*[\\.][cC][sS][sS]$");

	// Crawl the filesystem for all files and fill out registry.
	BP_FSRegCrawl(fs_reg);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Process Crawl Returns %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Retrieve all files as a null ptr terminated char ** of reasonable/printable strings.
	char ** files = BP_FSRegRetrieveEntryFullPathsAsBPCallocedNullTerminatedPointerArray(fs_reg, NULL);
	if(!files)
	{

		// leave critical section
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE;)

		// destroy the filesystem registry
		BP_DestroyHashTableRegistry(fs_reg->hreg);

		return ERR_FAILURE;
	}


	// supported methods
	BP_FLAGS_T supported_methods = 0;

	// set flags
	BP_SET_FLAG(supported_methods, BP_HTTPD_METHOD_GET);

	// enable post if this requires a valid user
	if( require_valid_user == BP_TRUE)
	{
		BP_SET_FLAG(supported_methods, BP_HTTPD_METHOD_POST);
	}


	// setup configs
	BP_HTTPD_REQUEST_HANDLER_SETUP_CONFIGS js_path_setup_configs;

	// set configurations
	js_path_setup_configs.supported_http_methods                  = supported_methods;
	js_path_setup_configs.posted_max_data_size_before_autofail    = BP_KILOBYTE * 500;
	js_path_setup_configs.posted_data_chunk_read_size             = BP_KILOBYTE * 2;
	js_path_setup_configs.posted_data_tmp_directory_literal_path  = "/tmp/blockpartytmp/";
	js_path_setup_configs.use_tmpfile_for_multipart_form_data     = BP_TRUE;
	js_path_setup_configs.use_tmpfile_for_octet_stream            = BP_TRUE;
	js_path_setup_configs.http_get_max_key_len                    = 100;
	js_path_setup_configs.http_get_max_val_len                    = 100;
	js_path_setup_configs.http_cookie_max_key_len                 = 200;
	js_path_setup_configs.http_cookie_max_val_len                 = 200;
	js_path_setup_configs.http_header_max_count                   = 30;
	js_path_setup_configs.http_header_max_key_len                 = 100;
	js_path_setup_configs.http_header_max_val_len                 = 200;


	// load all files
	size_t n = 0;
	for(; files[n]; n++)
	{


		if(bpstrlen(files[n]) >= bpstrlen(fs_reg->top_dir))
		{

			// tmp path
			char * tmp_handler_path = &files[n][bpstrlen(fs_reg->top_dir)-1];


			if( require_valid_user == BP_FALSE)
			{

				// add the root request handler
				BP_HTTPdRequestHandlerAdd
				(
						httpd,
						tmp_handler_path,
						BP_CSSStaticRequestHandler,
						&js_path_setup_configs,
						files[n],
						bpstrlen(files[n]),
						NULL
				);

			}
			else if ( require_valid_user == BP_TRUE)
			{

				// add the root request handler
				BP_HTTPdRequestHandlerAdd
				(
						httpd,
						tmp_handler_path,
						BP_CSSStaticRequestHandler_require_valid_user,
						&js_path_setup_configs,
						files[n],
						bpstrlen(files[n]),
						NULL
				);

			}
		}

	}


	// leave the critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating success
	return ERR_SUCCESS;

}

// This attempts to load all arbitrary text file types from a directory.
BP_ERROR_T BP_HTTPdAddTextHandlersFromFilesystemDir
(
	P_BP_HTTPD_SESSION httpd,
	char * load_dir,
	BP_BOOL recurse,
	char *  filename_pregexp,
	BP_BOOL require_valid_user
)
{

	// ensure we have a http daemon
	if(!httpd)
		return ERR_FAILURE;

	// ensure we have a load directory
	if(!load_dir)
		return ERR_FAILURE;

	if(!BP_StringIsReasonablePrintableString(load_dir, bpstrlen(load_dir), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	if(!BP_StringIsReasonablePrintableString(filename_pregexp, bpstrlen(filename_pregexp), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to open/crawl Filesystem Registry %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// Open a handle to the registry
	P_BP_FSREG fs_reg = BP_FSRegOpen(load_dir, BP_TRUE, 100);
	if(!fs_reg)
		return ERR_FAILURE;


	// enter the critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// set file policy here
	BP_FSREG_POLICY_FLAG_SET file_policy_flag_set[] =
	{
//          {BP_FS_FILE_POLICY_FLAG_IGNORE_ALL,                        BP_TRUE},
			{BP_FS_FILE_POLICY_FLAG_ONLY,                              BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_NOT_OWNED_BY_CURRENT_USER,         BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_OWNER_HAS_HIGHER_UID_THAN_CURRENT, BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_OWNER_HAS_LOWER_UID_THAN_CURRENT,  BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_OWNED_BY_CURRENT_USER,             BP_TRUE},
			{BP_FS_FILE_POLICY_FLAG_READABLE_BY_CURRENT_USER,          BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_WRITABLE_BY_CURRENT_USER,          BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_EXECUTABLE_BY_CURRENT_USER,        BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_SUID,                              BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_SGID,                              BP_TRUE},
			BP_FSREG_FLAG_SET_TERMINATOR
	};

	// attempt to set flags
	BP_FSRegPolicySetFlags
	(
			fs_reg,
			BP_FSREG_POLICY_TYPE_FILE,
			(P_BP_FSREG_POLICY_FLAG_SET) &file_policy_flag_set
	);

	// add filename regexp match
	BP_FSRegAddPregexPolicy(fs_reg, BP_FSREG_PREG_POLICY_TYPE_FNAME_MATCH, filename_pregexp);
	// BP_FSRegAddPregexPolicy(fs_reg, BP_FSREG_PREG_POLICY_TYPE_FNAME_MATCH, ".*[\\.][lL][eE][sS][sS]$");
	// BP_FSRegAddPregexPolicy(fs_reg, BP_FSREG_PREG_POLICY_TYPE_FNAME_MATCH, ".*");


	// Crawl the filesystem for all files and fill out registry.
	BP_FSRegCrawl(fs_reg);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Process Crawl Returns %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Retrieve all files as a null ptr terminated char ** of reasonable/printable strings.
	char ** files = BP_FSRegRetrieveEntryFullPathsAsBPCallocedNullTerminatedPointerArray(fs_reg, NULL);
	if(!files)
	{

		printf("\n [CUSTOM] Failed to get paths as array");

		// leave critical section
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE;)

		// destroy the filesystem registry
		BP_DestroyHashTableRegistry(fs_reg->hreg);

		// return indicating failure
		return ERR_FAILURE;

	}


	// supported methods
	BP_FLAGS_T supported_methods = 0;

	// set flags
	BP_SET_FLAG(supported_methods, BP_HTTPD_METHOD_GET);

	// enable post if this requires a valid user
	if( require_valid_user == BP_TRUE)
	{
		BP_SET_FLAG(supported_methods, BP_HTTPD_METHOD_POST);
	}

	// setup configs
	BP_HTTPD_REQUEST_HANDLER_SETUP_CONFIGS js_path_setup_configs;

	// set configurations
	js_path_setup_configs.supported_http_methods                  = supported_methods;
	js_path_setup_configs.posted_max_data_size_before_autofail    = BP_KILOBYTE * 500;
	js_path_setup_configs.posted_data_chunk_read_size             = BP_KILOBYTE * 2;
	js_path_setup_configs.posted_data_tmp_directory_literal_path  = "/tmp/blockpartytmp/";
	js_path_setup_configs.use_tmpfile_for_multipart_form_data     = BP_TRUE;
	js_path_setup_configs.use_tmpfile_for_octet_stream            = BP_TRUE;
	js_path_setup_configs.http_get_max_key_len                    = 100;
	js_path_setup_configs.http_get_max_val_len                    = 100;
	js_path_setup_configs.http_cookie_max_key_len                 = 200;
	js_path_setup_configs.http_cookie_max_val_len                 = 200;
	js_path_setup_configs.http_header_max_count                   = 30;
	js_path_setup_configs.http_header_max_key_len                 = 100;
	js_path_setup_configs.http_header_max_val_len                 = 200;


	// load all files
	size_t n = 0;
	for(; files[n]; n++)
	{


		if(bpstrlen(files[n]) >= bpstrlen(fs_reg->top_dir))
		{

			// tmp path
			char * tmp_handler_path = &files[n][bpstrlen(fs_reg->top_dir)-1];

			printf("\n [CUSTOM] Adding less format?: %s", tmp_handler_path);


			if( require_valid_user == BP_FALSE)
			{

				// add the root request handler
				BP_HTTPdRequestHandlerAdd
				(
						httpd,
						tmp_handler_path,
						BP_JavascriptStaticRequestHandler,
						&js_path_setup_configs,
						files[n],
						bpstrlen(files[n]),
						NULL
				);

			}
			else if ( require_valid_user == BP_TRUE)
			{

				// add the root request handler
				BP_HTTPdRequestHandlerAdd
				(
						httpd,
						tmp_handler_path,
						BP_JavascriptStaticRequestHandler_require_valid_user,
						&js_path_setup_configs,
						files[n],
						bpstrlen(files[n]),
						NULL
				);
			}
		}

	}


	// leave the critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating success
	return ERR_SUCCESS;

}

// This attempts to load all html types from filesystem.
BP_ERROR_T BP_HTTPdAddHtmlHandlersFromFilesystemDir
(
	P_BP_HTTPD_SESSION httpd,
	char * load_dir,
	BP_BOOL recurse,
	BP_BOOL require_valid_user
)
{

	// ensure we have a http daemon
	if(!httpd)
		return ERR_FAILURE;

	// ensure we have a load directory
	if(!load_dir)
		return ERR_FAILURE;

	if(!BP_StringIsReasonablePrintableString(load_dir, bpstrlen(load_dir), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to open/crawl Filesystem Registry %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// Open a handle to the registry
	P_BP_FSREG fs_reg = BP_FSRegOpen(load_dir, BP_TRUE, 100);
	if(!fs_reg)
		return ERR_FAILURE;


	// enter the critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// set file policy here
	BP_FSREG_POLICY_FLAG_SET file_policy_flag_set[] =
	{
//          {BP_FS_FILE_POLICY_FLAG_IGNORE_ALL,                        BP_TRUE},
			{BP_FS_FILE_POLICY_FLAG_ONLY,                              BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_NOT_OWNED_BY_CURRENT_USER,         BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_OWNER_HAS_HIGHER_UID_THAN_CURRENT, BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_OWNER_HAS_LOWER_UID_THAN_CURRENT,  BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_OWNED_BY_CURRENT_USER,             BP_TRUE},
			{BP_FS_FILE_POLICY_FLAG_READABLE_BY_CURRENT_USER,          BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_WRITABLE_BY_CURRENT_USER,          BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_EXECUTABLE_BY_CURRENT_USER,        BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_SUID,                              BP_TRUE},
//          {BP_FS_FILE_POLICY_FLAG_SGID,                              BP_TRUE},
			BP_FSREG_FLAG_SET_TERMINATOR
	};

	// attempt to set flags
	BP_FSRegPolicySetFlags
	(
			fs_reg,
			BP_FSREG_POLICY_TYPE_FILE,
			(P_BP_FSREG_POLICY_FLAG_SET) &file_policy_flag_set
	);

	// add filename regexp match
	// BP_FSRegAddPregexPolicy(fs_reg, BP_FSREG_PREG_POLICY_TYPE_FNAME_MATCH, filename_pregexp);
	BP_FSRegAddPregexPolicy(fs_reg, BP_FSREG_PREG_POLICY_TYPE_FNAME_MATCH, ".*[\\.][hH][tT][mM][lL]$");
	// BP_FSRegAddPregexPolicy(fs_reg, BP_FSREG_PREG_POLICY_TYPE_FNAME_MATCH, ".*");


	// Crawl the filesystem for all files and fill out registry.
	BP_FSRegCrawl(fs_reg);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Process Crawl Returns %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Retrieve all files as a null ptr terminated char ** of reasonable/printable strings.
	char ** files = BP_FSRegRetrieveEntryFullPathsAsBPCallocedNullTerminatedPointerArray(fs_reg, NULL);
	if(!files)
	{

		printf("\n [HTML] Failed to get paths as array");

		// leave critical section
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE;)

		// destroy the filesystem registry
		BP_DestroyHashTableRegistry(fs_reg->hreg);

		// return indicating failure
		return ERR_FAILURE;

	}


	// supported methods
	BP_FLAGS_T supported_methods = 0;

	// set flags
	BP_SET_FLAG(supported_methods, BP_HTTPD_METHOD_GET);

	// enable post if this requires a valid user
	if( require_valid_user == BP_TRUE)
	{
		BP_SET_FLAG(supported_methods, BP_HTTPD_METHOD_POST);
	}

	// setup configs
	BP_HTTPD_REQUEST_HANDLER_SETUP_CONFIGS js_path_setup_configs;

	// set configurations
	js_path_setup_configs.supported_http_methods                  = supported_methods;
	js_path_setup_configs.posted_max_data_size_before_autofail    = BP_KILOBYTE * 500;
	js_path_setup_configs.posted_data_chunk_read_size             = BP_KILOBYTE * 2;
	js_path_setup_configs.posted_data_tmp_directory_literal_path  = "/tmp/blockpartytmp/";
	js_path_setup_configs.use_tmpfile_for_multipart_form_data     = BP_TRUE;
	js_path_setup_configs.use_tmpfile_for_octet_stream            = BP_TRUE;
	js_path_setup_configs.http_get_max_key_len                    = 100;
	js_path_setup_configs.http_get_max_val_len                    = 100;
	js_path_setup_configs.http_cookie_max_key_len                 = 200;
	js_path_setup_configs.http_cookie_max_val_len                 = 200;
	js_path_setup_configs.http_header_max_count                   = 30;
	js_path_setup_configs.http_header_max_key_len                 = 100;
	js_path_setup_configs.http_header_max_val_len                 = 200;


	// load all files
	size_t n = 0;
	for(; files[n]; n++)
	{


		if(bpstrlen(files[n]) >= bpstrlen(fs_reg->top_dir))
		{

			// tmp path
			char * tmp_handler_path = &files[n][bpstrlen(fs_reg->top_dir)-1];


			if( require_valid_user == BP_FALSE)
			{

				// add the root request handler
				BP_HTTPdRequestHandlerAdd
				(
						httpd,
						tmp_handler_path,
						BP_HTMLStaticRequestHandler,
						&js_path_setup_configs,
						files[n],
						bpstrlen(files[n]),
						NULL
				);

			}
			else if ( require_valid_user == BP_TRUE)
			{

				// add the root request handler
				BP_HTTPdRequestHandlerAdd
				(
						httpd,
						tmp_handler_path,
						BP_HTMLStaticRequestHandler_require_valid_user,
						&js_path_setup_configs,
						files[n],
						bpstrlen(files[n]),
						NULL
				);
			}
		}

	}


	// leave the critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating success
	return ERR_SUCCESS;

}




// This method will dispatch a request handler.  This should only be called from
// within a MHD request access handler callback (eg: BP_HTTPdDefaultAccessHandlerCallback).
BP_ERROR_T BP_HTTPdRequestHandlerDispatch
(
		P_BP_HTTPD_REQUEST     request,
		char *                 handler,
		struct MHD_Connection *connection,
		const char *           url,
		const char *           method,
		const char *           version,
		const char *           upload_data,
		size_t *               upload_data_size,
		void **                con_cls
)
{

	// ensure we have a request and httpd
	if(!request)
		return ERR_FAILURE;
	if(!request->httpd)
		return ERR_FAILURE;


	// the httpd must have a hash registry associated with it
	if(!request->httpd->system_hreg)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Basic Sanity Checks %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// run basic checks
	if(!url)
			return ERR_FAILURE;
	if(!method)
			return ERR_FAILURE;
	if(!version)
			return ERR_FAILURE;

	// URL string check
	if(BP_StringIsReasonablePrintableString((char *) url, bpstrlen((char *) url), BP_FALSE, BP_FALSE) != BP_TRUE)
	{
			return ERR_FAILURE;
	}

	// Method string check.
	if(BP_StringIsReasonablePrintableString((char *) method, bpstrlen((char *) method), BP_FALSE, BP_FALSE) != BP_TRUE)
	{
			return ERR_FAILURE;
	}

	// Version string check.
	if(BP_StringIsReasonablePrintableString((char *) version, bpstrlen((char *) version), BP_FALSE, BP_FALSE) != BP_TRUE)
	{
			return ERR_FAILURE;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Lookup Request Handler and Dispatch %%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to lookup the handler in the hash table
	P_BP_HTTPD_REQUEST_HANDLER request_handler = BP_HTTPdRequestHandlerLookup(request->httpd, (char *) url);

	// ensure we have a request handler
	if(!request_handler)
		return ERR_FAILURE;

	// attempt to get the current method of the post
	BP_HTTPD_METHOD curr_request_method = BP_HTTPdDetermineMethod((char *) method);

	// printf("\n Current Method is?: %u", curr_request_method);

	// check to see if the method is supported
	if(!(BP_FLAG_IS_SET(request_handler->supported_http_methods, curr_request_method)))
	{

		printf("\n [+] UNSUPPORTED METHOD DETECTED");
		printf("\n");
		return ERR_FAILURE;
	}


	// increment the total activation counts
	request_handler->activation_n++;

	// call the handler directly
	BP_ERROR_T handler_ret_status = request_handler->handler
	(
			request,
			connection,
			url,
			method,
			version,
			upload_data,
			upload_data_size,
			con_cls
	);

	// if the request failed, increment the error count, else increment the completed count
	if(handler_ret_status == ERR_FAILURE)
		request_handler->error_n++;
	else
		request_handler->completed_n++;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Do Request Cleanup and Exit %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// return indicating success
	return ERR_SUCCESS;

}



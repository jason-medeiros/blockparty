/*
 * BP-HTTPdDefaultCallbacks.cc
 *
 *  Created on: Jul 16, 2015
 *      Author: root
 */



#include "../../../include/BP-Main.h"

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Default LOGIN Form Callback %%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Login form default callback
int BP_HTTPdDefaultLoginFormCallback
(
		/* P_BP_HTTPD_REQUEST */ void * request,
		struct MHD_Connection *         connection,
		char *                          reason_for_login_header,
		char *                          reason_for_login_body,
		char *                          reason_for_login_footer
)
{

	// ensure we have a request
	if(!request)
		return ERR_FAILURE;

	// ensure we have a connection
	if(!connection)
		return ERR_FAILURE;

	// ensure we text values
	if(!reason_for_login_header)
		return ERR_FAILURE;
	if(!reason_for_login_body)
		return ERR_FAILURE;
	if(!reason_for_login_footer)
		return ERR_FAILURE;

	// set request
	P_BP_HTTPD_REQUEST req = (P_BP_HTTPD_REQUEST) request;

    // create template subs
    BP_HTTPD_REQUEST_RESPONSE_TEMPLATE_SUBSTITUTION tmpl_subs[5];
    BP_ZERO_STACK(tmpl_subs);

    // number of template substituions
    size_t tmpl_subs_n = 0;


    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Perform Substitutions %%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // add page id substition
    tmpl_subs[tmpl_subs_n].replace_this = "{login_form_css_class}";
    tmpl_subs[tmpl_subs_n].replace_with = "login_form";
    tmpl_subs_n++;

    // add user substitution
    tmpl_subs[tmpl_subs_n].replace_this = "{login_form_user_input_name}";
    tmpl_subs[tmpl_subs_n].replace_with = "lu";
    tmpl_subs_n++;

    // add pass substitution
    tmpl_subs[tmpl_subs_n].replace_this = "{login_form_pass_input_name}";
    tmpl_subs[tmpl_subs_n].replace_with = "lp";
    tmpl_subs_n++;

    // -- prepare

    // add response to templated buffer
    BP_HTTPdRequestResponseAppendTemplatedBuffer
    (
    		req,
			BP_HTTPD_TMPL_DEFAULT_LOGIN_FORM,
            bpstrlen(BP_HTTPD_TMPL_DEFAULT_LOGIN_FORM),
            (P_BP_HTTPD_REQUEST_RESPONSE_TEMPLATE_SUBSTITUTION) &tmpl_subs,
            tmpl_subs_n
    );

    // -- send

    // send the prepared response (along with cookie data)
    BP_HTTPdRequestResponseSendPrepared
    (
    		req,
            connection,
            BP_HTTPD_RESPONSE_CODE_200_OK
    );


	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Default HEADER Iterator %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int BP_HTTPdDefaultHeaderIteratorCallback
(
		void *cls,
		MHD_ValueKind kind,
        const char *key ,
		const char* value
)
{

	// ensure we have an opaque request structure to work with
	if(!cls)
		return MHD_YES;

	// resolve opaque ptr here
	P_BP_HTTPD_REQUEST request = (P_BP_HTTPD_REQUEST) cls;



	// printf("\n SHEEET HERE:");
	// BP_HTTPdDisplayRequest(request);

	// ensure we have a value (how do we know len?)
	if(!value)
		return MHD_YES;

	// ensure it's a http get argument
	if(kind != MHD_HEADER_KIND)
		return MHD_YES;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Reasonable String Checks %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure that the key is reasonable
	if(!BP_StringIsReasonablePrintableString((char *) key, bpstrlen(key), BP_FALSE, BP_FALSE))
		return MHD_YES;

	// ensure that the value is reasonable
	if(!BP_StringIsReasonablePrintableString((char *) value, bpstrlen(value), BP_FALSE, BP_FALSE))
		return MHD_YES;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Length  Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if the var key is too long, exit immediately
	if(strnlen(key, request->request_handler->http_header_max_key_len+1) >= request->request_handler->http_header_max_key_len)
		return MHD_YES;

	// if the var value is too long, exit immediately
	if(strnlen(value, request->request_handler->http_header_max_val_len+1) >= request->request_handler->http_header_max_val_len)
		return MHD_YES;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add To Registry %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to add variable to argument registry as new file path type
	BP_ERROR_T retstatus = BP_HTTPdRequestVarAddToArgRegistry
	(
		request,
		BP_HTTPD_REQUEST_VAR_TYPE_HEADER,
		BP_HTTPD_REQUEST_CONTENT_TYPE_APPLICATION_X_WWW_FORM_URLENCODED,
		(char *) key,
		NULL,
		NULL,
		0
	);

	// attempt lookup after add to ensure it's there
	P_BP_HTTPD_REQUEST_VAR var = BP_HTTPdRequestVarArgRegistryLookup(request, BP_HTTPD_REQUEST_VAR_TYPE_HEADER, (char *) key);
	if(!var)
		return MHD_YES;

	// append data to the new var (append by bpstrlen since checks already passed earlier for sanity)
	BP_HTTPdRequestVarAppendData
	(
			request,
			var,
			(char *) value,
			bpstrlen(value)
	);

	// return indicating success
	return MHD_YES;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Default COOKIE Iterator %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int BP_HTTPdDefaultCookieIteratorCallback
(
		void *cls,
		MHD_ValueKind kind,
        const char *key ,
		const char* value
)
{

	// ensure we have an opaque request structure to work with
	if(!cls)
		return MHD_YES;

	// resolve opaque ptr here
	P_BP_HTTPD_REQUEST request = (P_BP_HTTPD_REQUEST) cls;

	// ensure we have a value (how do we know len?)
	if(!value)
		return MHD_YES;

	// ensure it's a http get argument
	if(kind != MHD_COOKIE_KIND)
		return MHD_YES;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Reasonable String Checks %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure that the key is reasonable
	if(!BP_StringIsReasonablePrintableString((char *) key, bpstrlen(key), BP_FALSE, BP_FALSE))
		return MHD_YES;

	// ensure that the value is reasonable
	if(!BP_StringIsReasonablePrintableString((char *) value, bpstrlen(value), BP_FALSE, BP_FALSE))
		return MHD_YES;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Length  Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if the var key is too long, exit immediately
	if(strnlen(key, request->request_handler->http_cookie_max_key_len+1) >= request->request_handler->http_cookie_max_key_len)
		return MHD_YES;

	// if the var value is too long, exit immediately
	if(strnlen(value, request->request_handler->http_cookie_max_val_len+1) >= request->request_handler->http_cookie_max_val_len)
		return MHD_YES;

	// printf("\n PASSED ALL HTTP GET CHECKS FOR %s", key);
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add To Registry %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to add variable to argument registry as new file path type
	BP_ERROR_T retstatus = BP_HTTPdRequestVarAddToArgRegistry
	(
		request,
		BP_HTTPD_REQUEST_VAR_TYPE_COOKIE,
		BP_HTTPD_REQUEST_CONTENT_TYPE_APPLICATION_X_WWW_FORM_URLENCODED,
		(char *) key,
		NULL,
		NULL,
		0
	);

	// attempt lookup after add to ensure it's there
	P_BP_HTTPD_REQUEST_VAR var = BP_HTTPdRequestVarArgRegistryLookup(request, BP_HTTPD_REQUEST_VAR_TYPE_COOKIE, (char *) key);
	if(!var)
		return MHD_YES;

	// append data to the new var (append by bpstrlen since checks already passed earlier for sanity)
	BP_HTTPdRequestVarAppendData
	(
			request,
			var,
			(char *) value,
			bpstrlen(value)
	);

	// return indicating success
	return MHD_YES;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Default HTTP GET Iterator %%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int BP_HTTPdDefaultGetIteratorCallback
(
		void *cls,
		MHD_ValueKind kind,
        const char *key ,
		const char* value
)
{

	// ensure we have an opaque request structure to work with
	if(!cls)
		return MHD_YES;

	// resolve opaque ptr here
	P_BP_HTTPD_REQUEST request = (P_BP_HTTPD_REQUEST) cls;

	// ensure we have a value (how do we know len?)
	if(!value)
		return MHD_YES;

	// ensure it's a http get argument
	if(kind != MHD_GET_ARGUMENT_KIND)
		return MHD_YES;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Reasonable String Checks %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure that the key is reasonable
	if(!BP_StringIsReasonablePrintableString((char *) key, bpstrlen(key), BP_FALSE, BP_FALSE))
		return MHD_YES;

	// ensure that the value is reasonable
	if(!BP_StringIsReasonablePrintableString((char *) value, bpstrlen(value), BP_FALSE, BP_FALSE))
		return MHD_YES;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Length  Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if the var key is too long, exit immediately
	if(strnlen(key, request->request_handler->http_get_max_key_len+1) >= request->request_handler->http_get_max_key_len)
		return MHD_YES;

	// if the var value is too long, exit immediately
	if(strnlen(value, request->request_handler->http_get_max_val_len+1) >= request->request_handler->http_get_max_val_len)
		return MHD_YES;

	// printf("\n PASSED ALL HTTP GET CHECKS FOR %s", key);
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add To Registry %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to add variable to argument registry as new file path type
	BP_ERROR_T retstatus = BP_HTTPdRequestVarAddToArgRegistry
	(
		request,
		BP_HTTPD_REQUEST_VAR_TYPE_GET,
		BP_HTTPD_REQUEST_CONTENT_TYPE_APPLICATION_X_WWW_FORM_URLENCODED,
		(char *) key,
		NULL,
		NULL,
		0
	);

	// attempt lookup after add to ensure it's there
	P_BP_HTTPD_REQUEST_VAR var = BP_HTTPdRequestVarArgRegistryLookup(request, BP_HTTPD_REQUEST_VAR_TYPE_GET, (char *) key);
	if(!var)
		return MHD_YES;

	// append data to the new var (append by bpstrlen since checks already passed earlier for sanity)
	BP_HTTPdRequestVarAppendData
	(
			request,
			var,
			(char *) value,
			bpstrlen(value)
	);

	// return indicating success
	return MHD_YES;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Default HTTP POST Iterator %%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This is the default post iterator callback that gets called
// when processing http post data with the default access handler.
int BP_HTTPdDefaultPostIteratorCallback
(
		void *cls,
		enum MHD_ValueKind kind,
		const char * key,
		const char * filename,
		const char * content_type,
		const char * transfer_encoding,
		const char * data,
		uint64_t off,
		size_t size
)
{


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Preliminary Data Checks %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure we have a request parameter
	if(!cls)
		return MHD_YES;

	// get request from cls
	P_BP_HTTPD_REQUEST request = (P_BP_HTTPD_REQUEST) cls;

	// ensure we have a key to work with
	if(!key)
		return MHD_YES;

	// ensure key is reasonable/printable
	if(!BP_StringIsReasonablePrintableString((char *) key, bpstrlen((char *) key), BP_FALSE, BP_FALSE))
		return MHD_YES;

	// mark the post processing as completed ok
	if(off && !size)
	{
		// printf("\n EXITING AT SIZE!");
		request->post_processing_completed = BP_TRUE;
		return MHD_YES;
	}

	// DO NOT exit immediately if we have our size DO NOT UNCOMMENT.  We need
	// to close the file handle below if the size is zero, so we can't just
	// exclude it here.
	// if(!size)
	// {
	// 	return MHD_YES;
	// }

	// set the post processor as started
	if(!request->post_processing_started)
		request->post_processing_started = BP_TRUE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Process Post Var Types Based on Content Type %%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// get the encoded content type
	BP_HTTPD_REQUEST_CONTENT_TYPE content_type_id = BP_HTTPdRequestGetContentType((char *) content_type);

	// create initial var
	P_BP_HTTPD_REQUEST_VAR var = (P_BP_HTTPD_REQUEST_VAR) NULL;

	// switch on the encoding type
	switch(content_type_id)
	{

		// octet streams are people uploading files
		case BP_HTTPD_REQUEST_CONTENT_TYPE_APPLICATION_OCTET_STREAM:

			// if we can't look it up, create it
			if(request->request_handler->use_tmpfile_for_multipart_form_data)
			{

				// if we can look up our var as a post tmpfile
				var = BP_HTTPdRequestVarArgRegistryLookup(request, BP_HTTPD_REQUEST_VAR_TYPE_POST_TMPFILE, (char *) key);
				if(var)
					break;

				// create new tmp uuid
				char new_tmp_uuid[BP_UUID_LEN+5];
				BP_ZERO_STACK(new_tmp_uuid);

				// generate new random uuid
				BP_GenRandomUUID_StoreInBuff((char *) &new_tmp_uuid);

				// set path elements for new temp file
				char * path_elems[] =
				{
					request->request_handler->posted_data_tmp_directory_literal_path,
					new_tmp_uuid,
					"-XXXXXX"
				};

				// generate the temp filepath template
				char * file_path = BP_JoinStringArray((char **) &path_elems, 3, NULL);

				// attempt to add variable to argument registry as new file path type
				BP_ERROR_T retstatus = BP_HTTPdRequestVarAddToArgRegistry
				(
					request,
					BP_HTTPD_REQUEST_VAR_TYPE_POST_TMPFILE,
					content_type_id,
					(char *) key,
					NULL,
					file_path,
					0
				);

				// check that the variable was added to the registry
				if(retstatus == ERR_FAILURE)
				{
					// printf("\n Failed to add var to registry.");
					return MHD_YES;
				}

				// attempt lookup after add to ensure it's there
				var = BP_HTTPdRequestVarArgRegistryLookup(request, BP_HTTPD_REQUEST_VAR_TYPE_POST_TMPFILE, (char *) key);

			}
			else
			{

				// attempt to add variable to argument registry as new file path type
				BP_ERROR_T retstatus = BP_HTTPdRequestVarAddToArgRegistry
				(
					request,
					BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA,
					content_type_id,
					(char *) key,
					NULL,
					NULL,
					0
				);

				// check that the variable was added to the registry
				if(retstatus == ERR_FAILURE)
				{
					// printf("\n Failed to add var to registry.");
					return MHD_YES;
				}

				// attempt lookup after add to ensure it's there
				var = BP_HTTPdRequestVarArgRegistryLookup(request, BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA, (char *) key);

			}


			// exit switch
			break;

		// attempt to create a new form post
		case BP_HTTPD_REQUEST_CONTENT_TYPE_MULTIPART_FORM_DATA:

			// attempt to create a tmpfile type for posted data
			if(request->request_handler->use_tmpfile_for_multipart_form_data)
			{

				// if we can look up our var as a post tmpfile
				var = BP_HTTPdRequestVarArgRegistryLookup(request, BP_HTTPD_REQUEST_VAR_TYPE_POST_TMPFILE, (char *) key);
				if(var)
					break;

				// create new tmp uuid
				char new_tmp_uuid[BP_UUID_LEN+5];
				BP_ZERO_STACK(new_tmp_uuid);

				// generate new random uuid
				BP_GenRandomUUID_StoreInBuff((char *) &new_tmp_uuid);


				// set path elements for new temp file
				char * path_elems[] =
				{
					request->request_handler->posted_data_tmp_directory_literal_path,
					new_tmp_uuid,
					"-XXXXXX"
				};

				// generate the temp filepath template
				char * file_path = BP_JoinStringArray((char **) &path_elems, 3, NULL);


				// attempt to add variable to argument registry as new file path type
				BP_ERROR_T retstatus = BP_HTTPdRequestVarAddToArgRegistry
				(
					request,
					BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA,
					content_type_id,
					(char *) key,
					NULL,
					file_path,
					0
				);

				// ensure the variable can be added successfully
				if(retstatus == ERR_FAILURE)
				{
					// printf("\n Failed to add var to registry.");
					return MHD_YES;
				}

				// attempt lookup after add to ensure it's there
				var = BP_HTTPdRequestVarArgRegistryLookup(request, BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA, (char *) key);

			}
			else
			{

				// attempt to add variable to argument registry as new file path type
				BP_ERROR_T retstatus = BP_HTTPdRequestVarAddToArgRegistry
				(
					request,
					BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA,
					content_type_id,
					(char *) key,
					NULL,
					NULL,
					0
				);

				// check that the variable was added to the registry
				if(retstatus == ERR_FAILURE)
				{
					// printf("\n Failed to add var to registry.");
					return MHD_YES;
				}

				// attempt lookup after add to ensure it's there
				var = BP_HTTPdRequestVarArgRegistryLookup(request, BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA, (char *) key);

			}
			break;

		case BP_HTTPD_REQUEST_CONTENT_TYPE_APPLICATION_X_WWW_FORM_URLENCODED:
			{

				// attempt to add variable to argument registry as new file path type
				BP_ERROR_T retstatus = BP_HTTPdRequestVarAddToArgRegistry
				(
					request,
					BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA,
					content_type_id,
					(char *) key,
					NULL,
					NULL,
					0
				);

				// check that the variable was added to the registry
				if(retstatus == ERR_FAILURE)
				{
					// printf("\n Failed to add var to registry.");
					return MHD_YES;
				}

				// attempt lookup after add to ensure it's there
				var = BP_HTTPdRequestVarArgRegistryLookup(request, BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA, (char *) key);

			}
			break;

		default:

			// set default content type as urlencoded form
			content_type_id = BP_HTTPD_REQUEST_CONTENT_TYPE_APPLICATION_X_WWW_FORM_URLENCODED;

			{
				// attempt to add variable to argument registry as new file path type
				BP_ERROR_T retstatus = BP_HTTPdRequestVarAddToArgRegistry
				(
					request,
					BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA,
					content_type_id,
					(char *) key,
					NULL,
					NULL,
					0
				);

				// check that the variable was added to the registry
				if(retstatus == ERR_FAILURE)
				{
					// printf("\n Failed to add var to registry.");
					return MHD_YES;
				}

				// attempt lookup after add to ensure it's there
				var = BP_HTTPdRequestVarArgRegistryLookup(request, BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA, (char *) key);

				// printf("\n Running default variable addition11111?: %p", var);

			}

			break;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Data to Posted Vars %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// we should have a var at all times at this point
	if(!var)
		return MHD_YES;

	// save last processed var here
	request->internal_last_request_var_processed = (void *) var;

	// append data to the created variable here.  If the variable type is specified
	// as a file or data type, either will be selected and data will added either
	// in memory, or as a file.  This selected behavior is determined when you create
	// a HTTPd Request Handler.
	BP_HTTPdRequestVarAppendData
	(
			request,
			var,
			(char *) data,
			size
	);



	// display the registry
	// BP_HashRegDisplay(request->arg_hreg, BP_FALSE, BP_TRUE);

	/*
	// Adds a variable to the registry according to type.
	BP_ERROR_T BP_HTTPdRequestVarAddToArgRegistry
	(
			P_BP_HTTPD_REQUEST             request,
			BP_HTTPD_REQUEST_VAR_TYPE      type,
			encoding,
			key,
			char *                         data,
			char *                         data_file,
			size_t                         data_size
	)
	*/

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Handle Post Type %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// handle post data type
	//if(request->post_data_is_file)
	//{

		/*
		// zero out data
		char * data_out = (char *) NULL;
		if(size)
		{
			data_out = (char *) bpcalloc(size+1, 1);
			memcpy(data_out, data, size);
		}

		printf("\n -> key:               %s", key);
		printf("\n -> filename:          %s", filename);
		printf("\n -> content_type:      %s", content_type);
		printf("\n -> transfer_encoding: %s", transfer_encoding);
		printf("\n -> data:              %s", data_out);
		printf("\n -> off:               %u", off);
		printf("\n -> size:              %u", size);
		printf("\n");

		// destroy alloced buffer if necessary
		if(data_out)
			bpfree(data_out);
		*/

	//}

	// printf("\n EXITING AT END?!");

	// return indicating success
	return MHD_YES;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Default Access Handler Callback %%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Debug/stress test connection number.  Do not remove.
size_t conn_nr = 0;

// The following access handler callback will be used if none is provided.  It
// simply acts as a generic (but very capable) access handler that routes requests
// based on the /request_handlers/ found within the httpd hreg.
int  BP_HTTPdDefaultAccessHandlerCallback
(
        void *                  cls,
        struct MHD_Connection * connection,
        const char *            url,
        const char *            method,
        const char *            version,
        const char *            upload_data,
        size_t     *            upload_data_size,
        void       **           con_cls
)
{



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Immediate Disqualifiers %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	 // run basic checks
	if(!url)
		return MHD_NO;
	if(!method)
		return MHD_NO;
	if(!version)
		return MHD_NO;


	// URL string check
	if(BP_StringIsReasonablePrintableString((char *) url, bpstrlen((char *) url), BP_FALSE, BP_FALSE) != BP_TRUE)
	{
			return MHD_NO;
	}

	// Method string check.
	if(BP_StringIsReasonablePrintableString((char *) method, bpstrlen((char *) method), BP_FALSE, BP_FALSE) != BP_TRUE)
	{
			return MHD_NO;
	}

	// Version string check.
	if(BP_StringIsReasonablePrintableString((char *) version, bpstrlen((char *) version), BP_FALSE, BP_FALSE) != BP_TRUE)
	{
			return MHD_NO;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Request Handler Lookup %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the localized httpd pointer
	P_BP_HTTPD_SESSION httpd = (P_BP_HTTPD_SESSION) cls;

	// attempt to lookup the handler in the hash table
	P_BP_HTTPD_REQUEST_HANDLER request_handler = BP_HTTPdRequestHandlerLookup(httpd, (char *) url);

	// ensure we have a request handler
	if(!request_handler)
		return MHD_NO;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% CLS Selector %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this will hold the populate request structure
	P_BP_HTTPD_REQUEST request = NULL;

	// check to see if we're at the tail of the request chain. Typically
	// there are at least two passes per each request.  This ensures that we pass the
	// httpd session along through each pass.
	if(!*con_cls)
	{

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Create Request Object %%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// attempt to add the request to the queue
		P_BP_HTTPD_REQUEST request = BP_HTTPdRequestAdd
		(
			httpd,
			request_handler,
			connection,
			url,
			method,
			version
		);

		// if the request failed basic sanity or composition, we
		// exit immediately.
		if(!request)
			return MHD_NO;

		// set marker to new request value
		*con_cls = (void *) request;

		// return indicating that we want to finish the request (this
		// will loop the request into the second (finishing) part)
		return MHD_YES;

	}
	else
	{

		// set the request structure from con_cls at this point.
		request = (P_BP_HTTPD_REQUEST)*con_cls;

		if(request->http_method == BP_HTTPD_METHOD_POST)
		{

			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Process POST Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// Because of the way MHD works, the first request sent only
			// sends header information.  This allows us to pregen a request
			// for use and pass it through again if post data is detected.


			// attempt to process data based on the callback
			MHD_post_process(request->pp, upload_data, *upload_data_size);

			// if we have post data continue to gather it
			if(*upload_data_size)
			{
				*upload_data_size = 0;
				return MHD_YES;
			}

			// destroy the post processor
			MHD_destroy_post_processor(request->pp);

			// set pp to null (prevents dual free)
			request->pp = NULL;

		}

	}

	// ensure we have a httpd session
	if(!cls)
	{

		// reset the session on continue here
		if(*con_cls)
			cls = *con_cls;
		else
			return MHD_NO;

	}

	// --- End Iterative Post Processing ------------

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Do Rate Limit Calculations %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// TODO: Wait for email response from MHD developers to see how to get
	//       the full raw request data and size.


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Retrieve Headers %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to retrieve connection values
	int val_header_status = MHD_get_connection_values(connection, MHD_HEADER_KIND, BP_HTTPdDefaultHeaderIteratorCallback, (void *) request);
	if(val_header_status < 0)
	{
		// printf("\n UNABLE TO RETRIEVE HEADER VALS");
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Retrieve Cookies %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to retrieve connection values
	int val_cookie_status = MHD_get_connection_values(connection, MHD_COOKIE_KIND, BP_HTTPdDefaultCookieIteratorCallback, (void *) request);
	if(val_cookie_status < 0)
	{
		// printf("\n UNABLE TO RETRIEVE COOKIE VALS");
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Retrieve HTTP Get Vars %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to retrieve connection values
	int val_get_status = MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND, BP_HTTPdDefaultGetIteratorCallback, (void *) request);
	if(val_get_status < 0)
	{
		// printf("\n UNABLE TO GET GET VARS");
	}




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Cookie -> Session Checks %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// session (filled dynamically)
	P_BP_HTTPD_USER_SESSION session = (P_BP_HTTPD_USER_SESSION) NULL;

	// if we have arguments, attempt session lookup from cookie values
	if(request->arg_hreg)
	{


		// Attempts to lookup an argument from the argument registry
		P_BP_HTTPD_REQUEST_VAR session_key = BP_HTTPdRequestVarArgRegistryLookup
		(
				request,
				BP_HTTPD_REQUEST_VAR_TYPE_COOKIE,
				request->httpd->cookie_key
		);

		// if we found our session key, attempt to lookup value
		if(session_key)
		{

			if(session_key->data)
			{

				printf("\n SESSION KEY?: %p - %s", session_key, session_key->data);
				printf("\n ");

				// attempt to lookup the session
				session = BP_HTTPdSessionAuthUserFromSessionId
				(
						httpd,
						session_key->data
				);

			}

		}

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt Auth from User Password %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(!session)
	{

		// Attempts to lookup an argument from the argument registry
		P_BP_HTTPD_REQUEST_VAR user_key = BP_HTTPdRequestVarArgRegistryLookup
		(
				request,
				BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA,
				"lu"
		);

		// Attempts to lookup an argument from the argument registry
		P_BP_HTTPD_REQUEST_VAR pass_key = BP_HTTPdRequestVarArgRegistryLookup
		(
				request,
				BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA,
				"lp"
		);

		// if there is a user key and pass key we can attempt a lookup
		if(user_key && pass_key)
		{

			// attempt to authenticate the user from creds if they're passed in
			session = BP_HTTPdSessionAuthUserFromCreds
			(
					httpd,
					user_key->data,
					pass_key->data
			);

		}

	}


	// set the session here if it was previously filled
	if(session)
		request->session = (void *) session;


    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Run Basic Sanity Checks %%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// IMPORTANT NOTE: At this point we only work with the httpd via the requests pointer
	httpd = request->httpd;

	// the httpd must have a hash registry associated with it
	if(!httpd->system_hreg)
		return MHD_NO;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Optional Inline Benchmarking for Testing %%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This is mostly for testing libmicrohttpd itself.  I use this little block
	// to stress test how many connections the libmicrohttpd library can handle
	// by setting do_premature_exit_for_benchmark to non-zero.  It causes the application
	// to exit immediately after dumping a static page.

    // set the response buffer
    char *response_buff = "<html>blah</html>";

    size_t do_premature_exit_for_benchmark = 0;
    if(do_premature_exit_for_benchmark)
    {

    	// display connection number
    	conn_nr++;
    	printf("\n Servicing connection: %u", conn_nr);

		// create arbitrary response
		struct MHD_Response *response = MHD_create_response_from_buffer
		(
				bpstrlen (response_buff),
				(void *) response_buff,
				MHD_RESPMEM_MUST_COPY
		);

		// queue the response out
		int ret = MHD_queue_response
		(
				connection,
				MHD_HTTP_OK,
				response
		);

		// destroy the response
		MHD_destroy_response (response);

		// return the yes indicator
		return MHD_YES;

    }




    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Lookup Request Processor %%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // dbg: request message
    printf("\n !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    printf("\n !!! ATTEMPTING REQUEST: %s ", url);
    printf("\n !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

    // attempt to dispatch the request to the appropriate handler
    BP_ERROR_T request_status = BP_HTTPdRequestHandlerDispatch
	(
			request,
			(char *) url,
			connection,
			url,
			method,
			version,
			upload_data,
			upload_data_size,
			con_cls
	);


    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Post Request Logging and Cleanup %%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


    if(!request->is_websocket_and_request_destruction_must_be_handled_externally)
    {

    	// destroy the request before exit
		printf("\n !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		printf("\n !!! EXITING/DESTROYING REQUEST !!!!!!!!!!!!!!!!!!!!!!!!!");
		printf("\n !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		printf("\n");

		// destroy the request
       	BP_HTTPdRequestDestroy(httpd, request);

    }
    else
    {
    	// destroy the request before exit
		printf("\n !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		printf("\n !!! EXITING REQUEST HANDLER FOR WEBSOCKET (data not destroyed) !!!!!!!");
		printf("\n !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		printf("\n");

    }

    // return indicating success
    return MHD_YES;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Static Request Handlers Which Requrie Valid Users %%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



// static request handler for handling gif
BP_ERROR_T BP_GIFStaticRequestHandler_require_valid_user
(
		void *                 request,
        struct MHD_Connection *connection,
        const char *           url,
        const char *           method,
        const char *           version,
        const char *           upload_data,
        size_t     *           upload_data_size,
        void       **          con_cls
)
{

	// enter the request handler
	BP_HTTPD_ENTER_REQUEST_HANDLER;

	// ensure we have a passed in pointer
	if(!request)
		return ERR_FAILURE;

	// create req structure from void request pointer
	P_BP_HTTPD_REQUEST req = (P_BP_HTTPD_REQUEST) request;

	/* ensure we have a httpd to work with */
	if(!req->httpd)
		return ERR_FAILURE;

	/* ensure we have a connection to work with */
	if(!connection)
		return ERR_FAILURE;

	/* ensure we have a url to work with */
	if(!url)
		return ERR_FAILURE;

	/* ensure we have a method to work with */
	if(!method)
		return ERR_FAILURE;

	/* ensure we have a version to work with */
	if(!version)
		return ERR_FAILURE;




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% process authentication %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// session (filled dynamically)
	P_BP_HTTPD_USER_SESSION session = (P_BP_HTTPD_USER_SESSION) req->session;

	// if no session, go ahead and generate login form
	if(!session)
	{

		// send the login form
		BP_HTTPdRequestResponseSendLoginForm
		(
			req,
			connection,
			"login_header",
			"login_body",
			"login_footer"
		);

		// immediately exit after sending the login
		return ERR_SUCCESS;

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Generate Html Output %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create an arbitrary response buffer
	char * response_buff      = NULL;
	char * response_buff_b64  = NULL;
	size_t response_buff_size = 0;

	// check handler for parameters
	if(req->request_handler->external_handler_parameter)
	{

		// set response buffer
		response_buff      = BP_FileGetContents((char *) req->request_handler->external_handler_parameter);
		response_buff_size = BP_FGetSize((char *) req->request_handler->external_handler_parameter);

	}
	else
	{

		// return indicating failure (should always have a buff)
		return ERR_FAILURE;

	}

	// ensure we hav a response buffer
	if(!response_buff)
	{
		return ERR_SUCCESS;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Response %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set request response as binary buffer
	BP_HTTPdRequestAppendBinaryBuffer
	(
			req,
			response_buff,
			response_buff_size
	);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add PNG Datatype %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set content length
	char content_length_str[128];
	BP_ZERO_STACK(content_length_str);

	// set the content length string
	sprintf(content_length_str, "%u", response_buff_size);

	// add content length header
	BP_HTTPdAddExtraResponseHeaderToRequest(req,"content-length",	content_length_str);

	// add content type header
	BP_HTTPdAddExtraResponseHeaderToRequest(req,"content-type",	"image/gif");



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Send Response %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// send the prepared response (along with cookie data)
	BP_HTTPdRequestResponseSendPrepared
	(
		req,
		connection,
		BP_HTTPD_RESPONSE_CODE_200_OK
	);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Relevant Data and Exit %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// free the allocated buffer
	bpfree((void *) response_buff);

	// return indicating success (request completed)
	return ERR_SUCCESS;
}


// static request handler for handling png
BP_ERROR_T BP_PNGStaticRequestHandler_require_valid_user
(
		void *                 request,
        struct MHD_Connection *connection,
        const char *           url,
        const char *           method,
        const char *           version,
        const char *           upload_data,
        size_t     *           upload_data_size,
        void       **          con_cls
)
{

	// enter the request handler
	BP_HTTPD_ENTER_REQUEST_HANDLER;

	// ensure we have a passed in pointer
	if(!request)
		return ERR_FAILURE;

	// create req structure from void request pointer
	P_BP_HTTPD_REQUEST req = (P_BP_HTTPD_REQUEST) request;

	/* ensure we have a httpd to work with */
	if(!req->httpd)
		return ERR_FAILURE;

	/* ensure we have a connection to work with */
	if(!connection)
		return ERR_FAILURE;

	/* ensure we have a url to work with */
	if(!url)
		return ERR_FAILURE;

	/* ensure we have a method to work with */
	if(!method)
		return ERR_FAILURE;

	/* ensure we have a version to work with */
	if(!version)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% process authentication %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// session (filled dynamically)
	P_BP_HTTPD_USER_SESSION session = (P_BP_HTTPD_USER_SESSION) req->session;

	// if no session, go ahead and generate login form
	if(!session)
	{

		// send the login form
		BP_HTTPdRequestResponseSendLoginForm
		(
			req,
			connection,
			"login_header",
			"login_body",
			"login_footer"
		);

		// immediately exit after sending the login
		return ERR_SUCCESS;

	}




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Generate Html Output %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create an arbitrary response buffer
	char * response_buff      = NULL;
	char * response_buff_b64  = NULL;
	size_t response_buff_size = 0;

	// printf("\n [PNG] Getting here");

	if(req->request_handler->external_handler_parameter)
	{

		// set response buffer
		response_buff      = BP_FileGetContents((char *) req->request_handler->external_handler_parameter);
		response_buff_size = BP_FGetSize((char *) req->request_handler->external_handler_parameter);

		// base64 encode value if we have a response buffer
		// if(response_buff && response_buff_size)
		// 	response_buff_b64 = BP_Base64Encode(response_buff, response_buff_size);

		// printf("\n[PNG] Got response param: %s - %p - %u", req->request_handler->external_handler_parameter, response_buff, response_buff_size);
	}
	else
	{

		// printf("\n [PNG] FAILING?!!!!");
		// return indicating failure (should always have a buff)
		return ERR_FAILURE;

	}

	// ensure we hav a response buffer
	if(!response_buff)
	{
		return ERR_SUCCESS;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Response %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set request response as binary buffer
	BP_HTTPdRequestAppendBinaryBuffer
	(
			req,
			response_buff,
			response_buff_size
	);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add PNG Datatype %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set content length
	char content_length_str[128];
	BP_ZERO_STACK(content_length_str);

	// set the content length string
	sprintf(content_length_str, "%u", response_buff_size);

	// printf("\n [PNG] Got Response Buffer Size?: %u", response_buff_size);

	// add content length header
	BP_HTTPdAddExtraResponseHeaderToRequest(req,"content-length",	content_length_str);

	// add content type header
	BP_HTTPdAddExtraResponseHeaderToRequest(req,"content-type",	"image/png");



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Send Response %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// send the prepared response (along with cookie data)
	BP_HTTPdRequestResponseSendPrepared
	(
		req,
		connection,
		BP_HTTPD_RESPONSE_CODE_200_OK
	);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Relevant Data and Exit %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// free the allocated buffer
	bpfree((void *) response_buff);

	// return indicating success (request completed)
	return ERR_SUCCESS;

}


// This is the default javascript handler used for serving javascript
// files loaded with BP_HTTPdAddJavascriptHandlersFromFilesystemDir above.
BP_ERROR_T BP_JavascriptStaticRequestHandler_require_valid_user
(
		void *                 request,
        struct MHD_Connection *connection,
        const char *           url,
        const char *           method,
        const char *           version,
        const char *           upload_data,
        size_t     *           upload_data_size,
        void       **          con_cls
)
{


	// enter the request handler
	BP_HTTPD_ENTER_REQUEST_HANDLER;

	// ensure we have a passed in pointer
	if(!request)
		return ERR_FAILURE;

	// create req structure from void request pointer
	P_BP_HTTPD_REQUEST req = (P_BP_HTTPD_REQUEST) request;

	/* ensure we have a httpd to work with */
	if(!req->httpd)
		return ERR_FAILURE;

	/* ensure we have a connection to work with */
	if(!connection)
		return ERR_FAILURE;

	/* ensure we have a url to work with */
	if(!url)
		return ERR_FAILURE;

	/* ensure we have a method to work with */
	if(!method)
		return ERR_FAILURE;

	/* ensure we have a version to work with */
	if(!version)
		return ERR_FAILURE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% process authentication %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// session (filled dynamically)
	P_BP_HTTPD_USER_SESSION session = (P_BP_HTTPD_USER_SESSION) req->session;

	// if no session, go ahead and generate login form
	if(!session)
	{

		// send the login form
		BP_HTTPdRequestResponseSendLoginForm
		(
			req,
			connection,
			"login_header",
			"login_body",
			"login_footer"
		);

		// immediately exit after sending the login
		return ERR_SUCCESS;

	}




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Generate Html Output %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create an arbitrary response buffer
	char * response_buff      = NULL;
	char * response_buff_b64  = NULL;
	size_t response_buff_size = 0;
	printf("\n [JS] Getting here");
	if(req->request_handler->external_handler_parameter)
	{

		// set response buffer
		response_buff      = BP_FileGetContents((char *) req->request_handler->external_handler_parameter);
		response_buff_size = BP_FGetSize((char *) req->request_handler->external_handler_parameter);

		// base64 encode value if we have a response buffer
		// if(response_buff && response_buff_size)
		// 	response_buff_b64 = BP_Base64Encode(response_buff, response_buff_size);

		printf("\n[JS] Got response param: %s - %p - %u", req->request_handler->external_handler_parameter, response_buff, response_buff_size);
	}
	else
	{

		printf("\n [JS] FAILING?!!!!");
		// return indicating failure (should always have a buff)
		return ERR_FAILURE;
	}

	// ensure we hav a response buffer
	if(!response_buff)
	{
		return ERR_SUCCESS;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Response %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set request response as binary buffer
	BP_HTTPdRequestAppendBinaryBuffer
	(
			req,
			response_buff,
			response_buff_size
	);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add PNG Datatype %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set content length
	char content_length_str[128];
	BP_ZERO_STACK(content_length_str);

	// set the content length string
	sprintf(content_length_str, "%u", response_buff_size);

	printf("\n [JS] Got Response Buffer Size?: %u", response_buff_size);

	// add content length header
	BP_HTTPdAddExtraResponseHeaderToRequest(req,"content-length",	content_length_str);

	// add content type header
	BP_HTTPdAddExtraResponseHeaderToRequest(req,"content-type",	"text/plain");



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Send Response %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// send the prepared response (along with cookie data)
	BP_HTTPdRequestResponseSendPrepared
	(
		req,
		connection,
		BP_HTTPD_RESPONSE_CODE_200_OK
	);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Relevant Data and Exit %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// free the allocated buffer
	bpfree((void *) response_buff);

	// return indicating success (request completed)
	return ERR_SUCCESS;

}

// This is the default CSS type request handler.
BP_ERROR_T BP_CSSStaticRequestHandler_require_valid_user
(
		void *                 request,
        struct MHD_Connection *connection,
        const char *           url,
        const char *           method,
        const char *           version,
        const char *           upload_data,
        size_t     *           upload_data_size,
        void       **          con_cls
)
{


	// enter the request handler
	BP_HTTPD_ENTER_REQUEST_HANDLER;

	// ensure we have a passed in pointer
	if(!request)
		return ERR_FAILURE;

	// create req structure from void request pointer
	P_BP_HTTPD_REQUEST req = (P_BP_HTTPD_REQUEST) request;

	/* ensure we have a httpd to work with */
	if(!req->httpd)
		return ERR_FAILURE;

	/* ensure we have a connection to work with */
	if(!connection)
		return ERR_FAILURE;

	/* ensure we have a url to work with */
	if(!url)
		return ERR_FAILURE;

	/* ensure we have a method to work with */
	if(!method)
		return ERR_FAILURE;

	/* ensure we have a version to work with */
	if(!version)
		return ERR_FAILURE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% process authentication %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// session (filled dynamically)
	P_BP_HTTPD_USER_SESSION session = (P_BP_HTTPD_USER_SESSION) req->session;

	// if no session, go ahead and generate login form
	if(!session)
	{

		// send the login form
		BP_HTTPdRequestResponseSendLoginForm
		(
			req,
			connection,
			"login_header",
			"login_body",
			"login_footer"
		);

		// immediately exit after sending the login
		return ERR_SUCCESS;

	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Generate Html Output %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create an arbitrary response buffer
	char * response_buff      = NULL;
	char * response_buff_b64  = NULL;
	size_t response_buff_size = 0;
	printf("\n [CSS] Getting here");
	if(req->request_handler->external_handler_parameter)
	{

		// set response buffer
		response_buff      = BP_FileGetContents((char *) req->request_handler->external_handler_parameter);
		response_buff_size = BP_FGetSize((char *) req->request_handler->external_handler_parameter);

		// base64 encode value if we have a response buffer
		// if(response_buff && response_buff_size)
		// 	response_buff_b64 = BP_Base64Encode(response_buff, response_buff_size);

		printf("\n[CSS] Got response param: %s - %p - %u", req->request_handler->external_handler_parameter, response_buff, response_buff_size);
	}
	else
	{

		printf("\n [CSS] FAILING?!!!!");
		// return indicating failure (should always have a buff)
		return ERR_FAILURE;
	}

	// ensure we hav a response buffer
	if(!response_buff)
	{
		return ERR_SUCCESS;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Response %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set request response as binary buffer
	BP_HTTPdRequestAppendBinaryBuffer
	(
			req,
			response_buff,
			response_buff_size
	);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add PNG Datatype %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set content length
	char content_length_str[128];
	BP_ZERO_STACK(content_length_str);

	// set the content length string
	sprintf(content_length_str, "%u", response_buff_size);

	printf("\n [CSS] Got Response Buffer Size?: %u", response_buff_size);

	// add content length header
	BP_HTTPdAddExtraResponseHeaderToRequest(req,"content-length",	content_length_str);

	// add content type header
	BP_HTTPdAddExtraResponseHeaderToRequest(req,"content-type",	"text/css; charset=utf-8");



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Send Response %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// send the prepared response (along with cookie data)
	BP_HTTPdRequestResponseSendPrepared
	(
		req,
		connection,
		BP_HTTPD_RESPONSE_CODE_200_OK
	);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Relevant Data and Exit %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// free the allocated buffer
	bpfree((void *) response_buff);

	// return indicating success (request completed)
	return ERR_SUCCESS;

}


// HTML Static Request Handler
BP_ERROR_T BP_HTMLStaticRequestHandler_require_valid_user
(
		void *                 request,
        struct MHD_Connection *connection,
        const char *           url,
        const char *           method,
        const char *           version,
        const char *           upload_data,
        size_t     *           upload_data_size,
        void       **          con_cls
)
{


	// enter the request handler
	BP_HTTPD_ENTER_REQUEST_HANDLER;

	// ensure we have a passed in pointer
	if(!request)
		return ERR_FAILURE;

	// create req structure from void request pointer
	P_BP_HTTPD_REQUEST req = (P_BP_HTTPD_REQUEST) request;

	/* ensure we have a httpd to work with */
	if(!req->httpd)
		return ERR_FAILURE;

	/* ensure we have a connection to work with */
	if(!connection)
		return ERR_FAILURE;

	/* ensure we have a url to work with */
	if(!url)
		return ERR_FAILURE;

	/* ensure we have a method to work with */
	if(!method)
		return ERR_FAILURE;

	/* ensure we have a version to work with */
	if(!version)
		return ERR_FAILURE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% process authentication %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// session (filled dynamically)
	P_BP_HTTPD_USER_SESSION session = (P_BP_HTTPD_USER_SESSION) req->session;

	// if no session, go ahead and generate login form
	if(!session)
	{

		// send the login form
		BP_HTTPdRequestResponseSendLoginForm
		(
			req,
			connection,
			"login_header",
			"login_body",
			"login_footer"
		);

		// immediately exit after sending the login
		return ERR_SUCCESS;

	}




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Generate Html Output %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create an arbitrary response buffer
	char * response_buff      = NULL;
	char * response_buff_b64  = NULL;
	size_t response_buff_size = 0;
	printf("\n [HTML] Getting here");
	if(req->request_handler->external_handler_parameter)
	{

		// set response buffer
		response_buff      = BP_FileGetContents((char *) req->request_handler->external_handler_parameter);
		response_buff_size = BP_FGetSize((char *) req->request_handler->external_handler_parameter);

		// base64 encode value if we have a response buffer
		// if(response_buff && response_buff_size)
		// 	response_buff_b64 = BP_Base64Encode(response_buff, response_buff_size);

		printf("\n[HTML] Got response param: %s - %p - %u", req->request_handler->external_handler_parameter, response_buff, response_buff_size);
	}
	else
	{

		printf("\n [HTML] FAILING?!!!!");
		// return indicating failure (should always have a buff)
		return ERR_FAILURE;
	}

	// ensure we hav a response buffer
	if(!response_buff)
	{
		return ERR_SUCCESS;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Response %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set request response as binary buffer
	BP_HTTPdRequestAppendBinaryBuffer
	(
			req,
			response_buff,
			response_buff_size
	);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add PNG Datatype %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set content length
	char content_length_str[128];
	BP_ZERO_STACK(content_length_str);

	// set the content length string
	sprintf(content_length_str, "%u", response_buff_size);

	printf("\n [HTML] Got Response Buffer Size?: %u", response_buff_size);

	// add content length header
	BP_HTTPdAddExtraResponseHeaderToRequest(req,"content-length",	content_length_str);

	// add content type header
	BP_HTTPdAddExtraResponseHeaderToRequest(req,"content-type",	"text/html");



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Send Response %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// send the prepared response (along with cookie data)
	BP_HTTPdRequestResponseSendPrepared
	(
		req,
		connection,
		BP_HTTPD_RESPONSE_CODE_200_OK
	);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Relevant Data and Exit %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// free the allocated buffer
	bpfree((void *) response_buff);

	// return indicating success (request completed)
	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Unauthenticated Static Request Handlers %%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// static request handler for handling gif
BP_ERROR_T BP_GIFStaticRequestHandler
(
		void *                 request,
        struct MHD_Connection *connection,
        const char *           url,
        const char *           method,
        const char *           version,
        const char *           upload_data,
        size_t     *           upload_data_size,
        void       **          con_cls
)
{

	// enter the request handler
	BP_HTTPD_ENTER_REQUEST_HANDLER;

	// ensure we have a passed in pointer
	if(!request)
		return ERR_FAILURE;

	// create req structure from void request pointer
	P_BP_HTTPD_REQUEST req = (P_BP_HTTPD_REQUEST) request;

	/* ensure we have a httpd to work with */
	if(!req->httpd)
		return ERR_FAILURE;

	/* ensure we have a connection to work with */
	if(!connection)
		return ERR_FAILURE;

	/* ensure we have a url to work with */
	if(!url)
		return ERR_FAILURE;

	/* ensure we have a method to work with */
	if(!method)
		return ERR_FAILURE;

	/* ensure we have a version to work with */
	if(!version)
		return ERR_FAILURE;

	// session (filled dynamically)
	P_BP_HTTPD_USER_SESSION session = (P_BP_HTTPD_USER_SESSION) req->session;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Generate Html Output %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create an arbitrary response buffer
	char * response_buff      = NULL;
	char * response_buff_b64  = NULL;
	size_t response_buff_size = 0;

	// check handler for parameters
	if(req->request_handler->external_handler_parameter)
	{

		// set response buffer
		response_buff      = BP_FileGetContents((char *) req->request_handler->external_handler_parameter);
		response_buff_size = BP_FGetSize((char *) req->request_handler->external_handler_parameter);

	}
	else
	{

		// return indicating failure (should always have a buff)
		return ERR_FAILURE;

	}

	// ensure we hav a response buffer
	if(!response_buff)
	{
		return ERR_SUCCESS;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Response %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set request response as binary buffer
	BP_HTTPdRequestAppendBinaryBuffer
	(
			req,
			response_buff,
			response_buff_size
	);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add PNG Datatype %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set content length
	char content_length_str[128];
	BP_ZERO_STACK(content_length_str);

	// set the content length string
	sprintf(content_length_str, "%u", response_buff_size);

	// add content length header
	BP_HTTPdAddExtraResponseHeaderToRequest(req,"content-length",	content_length_str);

	// add content type header
	BP_HTTPdAddExtraResponseHeaderToRequest(req,"content-type",	"image/gif");



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Send Response %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// send the prepared response (along with cookie data)
	BP_HTTPdRequestResponseSendPrepared
	(
		req,
		connection,
		BP_HTTPD_RESPONSE_CODE_200_OK
	);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Relevant Data and Exit %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// free the allocated buffer
	bpfree((void *) response_buff);

	// return indicating success (request completed)
	return ERR_SUCCESS;
}


// static request handler for handling png
BP_ERROR_T BP_PNGStaticRequestHandler
(
		void *                 request,
        struct MHD_Connection *connection,
        const char *           url,
        const char *           method,
        const char *           version,
        const char *           upload_data,
        size_t     *           upload_data_size,
        void       **          con_cls
)
{

	// enter the request handler
	BP_HTTPD_ENTER_REQUEST_HANDLER;

	// ensure we have a passed in pointer
	if(!request)
		return ERR_FAILURE;

	// create req structure from void request pointer
	P_BP_HTTPD_REQUEST req = (P_BP_HTTPD_REQUEST) request;

	/* ensure we have a httpd to work with */
	if(!req->httpd)
		return ERR_FAILURE;

	/* ensure we have a connection to work with */
	if(!connection)
		return ERR_FAILURE;

	/* ensure we have a url to work with */
	if(!url)
		return ERR_FAILURE;

	/* ensure we have a method to work with */
	if(!method)
		return ERR_FAILURE;

	/* ensure we have a version to work with */
	if(!version)
		return ERR_FAILURE;

	// session (filled dynamically)
	P_BP_HTTPD_USER_SESSION session = (P_BP_HTTPD_USER_SESSION) req->session;

	// if no session, go ahead and generate login form
	// if(!session)
	// 	return ERR_SUCCESS;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Generate Html Output %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create an arbitrary response buffer
	char * response_buff      = NULL;
	char * response_buff_b64  = NULL;
	size_t response_buff_size = 0;

	// printf("\n [PNG] Getting here");

	if(req->request_handler->external_handler_parameter)
	{

		// set response buffer
		response_buff      = BP_FileGetContents((char *) req->request_handler->external_handler_parameter);
		response_buff_size = BP_FGetSize((char *) req->request_handler->external_handler_parameter);

		// base64 encode value if we have a response buffer
		// if(response_buff && response_buff_size)
		// 	response_buff_b64 = BP_Base64Encode(response_buff, response_buff_size);

		// printf("\n[PNG] Got response param: %s - %p - %u", req->request_handler->external_handler_parameter, response_buff, response_buff_size);
	}
	else
	{

		// printf("\n [PNG] FAILING?!!!!");
		// return indicating failure (should always have a buff)
		return ERR_FAILURE;

	}

	// ensure we hav a response buffer
	if(!response_buff)
	{
		return ERR_SUCCESS;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Response %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set request response as binary buffer
	BP_HTTPdRequestAppendBinaryBuffer
	(
			req,
			response_buff,
			response_buff_size
	);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add PNG Datatype %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set content length
	char content_length_str[128];
	BP_ZERO_STACK(content_length_str);

	// set the content length string
	sprintf(content_length_str, "%u", response_buff_size);

	// printf("\n [PNG] Got Response Buffer Size?: %u", response_buff_size);

	// add content length header
	BP_HTTPdAddExtraResponseHeaderToRequest(req,"content-length",	content_length_str);

	// add content type header
	BP_HTTPdAddExtraResponseHeaderToRequest(req,"content-type",	"image/png");



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Send Response %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// send the prepared response (along with cookie data)
	BP_HTTPdRequestResponseSendPrepared
	(
		req,
		connection,
		BP_HTTPD_RESPONSE_CODE_200_OK
	);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Relevant Data and Exit %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// free the allocated buffer
	bpfree((void *) response_buff);

	// return indicating success (request completed)
	return ERR_SUCCESS;
}


// This is the default javascript handler used for serving javascript
// files loaded with BP_HTTPdAddJavascriptHandlersFromFilesystemDir above.
BP_ERROR_T BP_JavascriptStaticRequestHandler
(
		void *                 request,
        struct MHD_Connection *connection,
        const char *           url,
        const char *           method,
        const char *           version,
        const char *           upload_data,
        size_t     *           upload_data_size,
        void       **          con_cls
)
{


	// enter the request handler
	BP_HTTPD_ENTER_REQUEST_HANDLER;

	// ensure we have a passed in pointer
	if(!request)
		return ERR_FAILURE;

	// create req structure from void request pointer
	P_BP_HTTPD_REQUEST req = (P_BP_HTTPD_REQUEST) request;

	/* ensure we have a httpd to work with */
	if(!req->httpd)
		return ERR_FAILURE;

	/* ensure we have a connection to work with */
	if(!connection)
		return ERR_FAILURE;

	/* ensure we have a url to work with */
	if(!url)
		return ERR_FAILURE;

	/* ensure we have a method to work with */
	if(!method)
		return ERR_FAILURE;

	/* ensure we have a version to work with */
	if(!version)
		return ERR_FAILURE;

	// session (filled dynamically)
	P_BP_HTTPD_USER_SESSION session = (P_BP_HTTPD_USER_SESSION) req->session;

	// if no session, go ahead and generate login form
	// if(!session)
	// 	return ERR_SUCCESS;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Generate Html Output %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create an arbitrary response buffer
	char * response_buff      = NULL;
	char * response_buff_b64  = NULL;
	size_t response_buff_size = 0;
	printf("\n [JS] Getting here");
	if(req->request_handler->external_handler_parameter)
	{

		// set response buffer
		response_buff      = BP_FileGetContents((char *) req->request_handler->external_handler_parameter);
		response_buff_size = BP_FGetSize((char *) req->request_handler->external_handler_parameter);

		// base64 encode value if we have a response buffer
		// if(response_buff && response_buff_size)
		// 	response_buff_b64 = BP_Base64Encode(response_buff, response_buff_size);

		printf("\n[JS] Got response param: %s - %p - %u", req->request_handler->external_handler_parameter, response_buff, response_buff_size);
	}
	else
	{

		printf("\n [JS] FAILING?!!!!");
		// return indicating failure (should always have a buff)
		return ERR_FAILURE;
	}

	// ensure we hav a response buffer
	if(!response_buff)
	{
		return ERR_SUCCESS;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Response %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set request response as binary buffer
	BP_HTTPdRequestAppendBinaryBuffer
	(
			req,
			response_buff,
			response_buff_size
	);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add PNG Datatype %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set content length
	char content_length_str[128];
	BP_ZERO_STACK(content_length_str);

	// set the content length string
	sprintf(content_length_str, "%u", response_buff_size);

	printf("\n [JS] Got Response Buffer Size?: %u", response_buff_size);

	// add content length header
	BP_HTTPdAddExtraResponseHeaderToRequest(req,"content-length",	content_length_str);

	// add content type header
	BP_HTTPdAddExtraResponseHeaderToRequest(req,"content-type",	"text/plain");



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Send Response %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// send the prepared response (along with cookie data)
	BP_HTTPdRequestResponseSendPrepared
	(
		req,
		connection,
		BP_HTTPD_RESPONSE_CODE_200_OK
	);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Relevant Data and Exit %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// free the allocated buffer
	bpfree((void *) response_buff);

	// return indicating success (request completed)
	return ERR_SUCCESS;

}

// This is the default CSS type request handler.
BP_ERROR_T BP_CSSStaticRequestHandler
(
		void *                 request,
        struct MHD_Connection *connection,
        const char *           url,
        const char *           method,
        const char *           version,
        const char *           upload_data,
        size_t     *           upload_data_size,
        void       **          con_cls
)
{


	// enter the request handler
	BP_HTTPD_ENTER_REQUEST_HANDLER;

	// ensure we have a passed in pointer
	if(!request)
		return ERR_FAILURE;

	// create req structure from void request pointer
	P_BP_HTTPD_REQUEST req = (P_BP_HTTPD_REQUEST) request;

	/* ensure we have a httpd to work with */
	if(!req->httpd)
		return ERR_FAILURE;

	/* ensure we have a connection to work with */
	if(!connection)
		return ERR_FAILURE;

	/* ensure we have a url to work with */
	if(!url)
		return ERR_FAILURE;

	/* ensure we have a method to work with */
	if(!method)
		return ERR_FAILURE;

	/* ensure we have a version to work with */
	if(!version)
		return ERR_FAILURE;

	// session (filled dynamically)
	P_BP_HTTPD_USER_SESSION session = (P_BP_HTTPD_USER_SESSION) req->session;

	// if no session, go ahead and generate login form
	// if(!session)
	// 	return ERR_SUCCESS;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Generate Html Output %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create an arbitrary response buffer
	char * response_buff      = NULL;
	char * response_buff_b64  = NULL;
	size_t response_buff_size = 0;
	printf("\n [CSS] Getting here");
	if(req->request_handler->external_handler_parameter)
	{

		// set response buffer
		response_buff      = BP_FileGetContents((char *) req->request_handler->external_handler_parameter);
		response_buff_size = BP_FGetSize((char *) req->request_handler->external_handler_parameter);

		// base64 encode value if we have a response buffer
		// if(response_buff && response_buff_size)
		// 	response_buff_b64 = BP_Base64Encode(response_buff, response_buff_size);

		printf("\n[CSS] Got response param: %s - %p - %u", req->request_handler->external_handler_parameter, response_buff, response_buff_size);
	}
	else
	{

		printf("\n [CSS] FAILING?!!!!");
		// return indicating failure (should always have a buff)
		return ERR_FAILURE;
	}

	// ensure we hav a response buffer
	if(!response_buff)
	{
		return ERR_SUCCESS;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Response %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set request response as binary buffer
	BP_HTTPdRequestAppendBinaryBuffer
	(
			req,
			response_buff,
			response_buff_size
	);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add PNG Datatype %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set content length
	char content_length_str[128];
	BP_ZERO_STACK(content_length_str);

	// set the content length string
	sprintf(content_length_str, "%u", response_buff_size);

	printf("\n [CSS] Got Response Buffer Size?: %u", response_buff_size);

	// add content length header
	BP_HTTPdAddExtraResponseHeaderToRequest(req,"content-length",	content_length_str);

	// add content type header
	BP_HTTPdAddExtraResponseHeaderToRequest(req,"content-type",	"text/css; charset=utf-8");



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Send Response %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// send the prepared response (along with cookie data)
	BP_HTTPdRequestResponseSendPrepared
	(
		req,
		connection,
		BP_HTTPD_RESPONSE_CODE_200_OK
	);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Relevant Data and Exit %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// free the allocated buffer
	bpfree((void *) response_buff);

	// return indicating success (request completed)
	return ERR_SUCCESS;

}


// HTML Static Request Handler
BP_ERROR_T BP_HTMLStaticRequestHandler
(
		void *                 request,
        struct MHD_Connection *connection,
        const char *           url,
        const char *           method,
        const char *           version,
        const char *           upload_data,
        size_t     *           upload_data_size,
        void       **          con_cls
)
{


	// enter the request handler
	BP_HTTPD_ENTER_REQUEST_HANDLER;

	// ensure we have a passed in pointer
	if(!request)
		return ERR_FAILURE;

	// create req structure from void request pointer
	P_BP_HTTPD_REQUEST req = (P_BP_HTTPD_REQUEST) request;

	/* ensure we have a httpd to work with */
	if(!req->httpd)
		return ERR_FAILURE;

	/* ensure we have a connection to work with */
	if(!connection)
		return ERR_FAILURE;

	/* ensure we have a url to work with */
	if(!url)
		return ERR_FAILURE;

	/* ensure we have a method to work with */
	if(!method)
		return ERR_FAILURE;

	/* ensure we have a version to work with */
	if(!version)
		return ERR_FAILURE;

	// session (filled dynamically)
	P_BP_HTTPD_USER_SESSION session = (P_BP_HTTPD_USER_SESSION) req->session;

	// if no session, go ahead and generate login form
	// if(!session)
	// 	return ERR_SUCCESS;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Generate Html Output %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create an arbitrary response buffer
	char * response_buff      = NULL;
	char * response_buff_b64  = NULL;
	size_t response_buff_size = 0;
	printf("\n [HTML] Getting here");
	if(req->request_handler->external_handler_parameter)
	{

		// set response buffer
		response_buff      = BP_FileGetContents((char *) req->request_handler->external_handler_parameter);
		response_buff_size = BP_FGetSize((char *) req->request_handler->external_handler_parameter);

		// base64 encode value if we have a response buffer
		// if(response_buff && response_buff_size)
		// 	response_buff_b64 = BP_Base64Encode(response_buff, response_buff_size);

		printf("\n[HTML] Got response param: %s - %p - %u", req->request_handler->external_handler_parameter, response_buff, response_buff_size);
	}
	else
	{

		printf("\n [HTML] FAILING?!!!!");
		// return indicating failure (should always have a buff)
		return ERR_FAILURE;
	}

	// ensure we hav a response buffer
	if(!response_buff)
	{
		return ERR_SUCCESS;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Response %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set request response as binary buffer
	BP_HTTPdRequestAppendBinaryBuffer
	(
			req,
			response_buff,
			response_buff_size
	);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add PNG Datatype %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set content length
	char content_length_str[128];
	BP_ZERO_STACK(content_length_str);

	// set the content length string
	sprintf(content_length_str, "%u", response_buff_size);

	printf("\n [HTML] Got Response Buffer Size?: %u", response_buff_size);

	// add content length header
	BP_HTTPdAddExtraResponseHeaderToRequest(req,"content-length",	content_length_str);

	// add content type header
	BP_HTTPdAddExtraResponseHeaderToRequest(req,"content-type",	"text/html");



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Send Response %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// send the prepared response (along with cookie data)
	BP_HTTPdRequestResponseSendPrepared
	(
		req,
		connection,
		BP_HTTPD_RESPONSE_CODE_200_OK
	);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Relevant Data and Exit %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// free the allocated buffer
	bpfree((void *) response_buff);

	// return indicating success (request completed)
	return ERR_SUCCESS;

}

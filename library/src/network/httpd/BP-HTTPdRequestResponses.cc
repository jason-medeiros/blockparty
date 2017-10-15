/*
 * BP-HTTPdRequestResponses.cc
 *
 *  Created on: Jul 19, 2015
 *      Author: root
 */


// main include file
#include "../../../include/BP-Main.h"

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Request Responses %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Append a templated buffer to the response currently held in the provided
// request.
BP_ERROR_T BP_HTTPdRequestResponseAppendTemplatedBuffer
(
		P_BP_HTTPD_REQUEST                                request,
		char *                                            buffer,
		size_t                                            buffer_size,
		P_BP_HTTPD_REQUEST_RESPONSE_TEMPLATE_SUBSTITUTION substitutions,
		size_t                                            substitution_count
)
{

	// ensure we have a request
	if(!request)
		return ERR_FAILURE;
	if(!buffer)
		return ERR_FAILURE;
	if(!buffer_size)
		return ERR_FAILURE;

	// Since this is a response item, we want to prevent ANY non-ascii from being dumped
	// to the page.
	if(!BP_StringIsReasonablePrintableString(buffer, buffer_size, BP_TRUE, BP_FALSE))
	{
		return ERR_FAILURE;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Reallocate Buffer %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// new buffer space size (never allocate extra space here or you risk a leak)
	size_t new_buff_size = buffer_size;

	// attempt reallocating a buffer (plus some extra pad just in case)
	request->response_buff = (char *) bprealloc(request->response, request->response_buff_size + new_buff_size);

	// ensure we have response buffer allocated
	if(!request->response_buff)
		return ERR_FAILURE;

	// copy in the buffer
	memcpy((void *) &request->response_buff[request->response_buff_size], buffer, buffer_size);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Perform Substitutions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create space for find strings
	char ** sub_find    = (char **) bpcalloc((substitution_count + 1) * sizeof(char *), 1);

	// create space for replace strings
	char ** sub_replace = (char **) bpcalloc((substitution_count + 1) * sizeof(char *), 1);

	// destroy if we can't allocate one or the other
	if(!sub_find || !sub_replace)
	{

		// destroy either array if one couldn't be allocated
		if(sub_find)
			bpfree(sub_find);
		if(sub_replace)
			bpfree(sub_replace);

		// exit
		return ERR_FAILURE;
	}

	// create substitution array
	size_t n = 0;
	size_t internal_idx = 0;
	for(; n < substitution_count; n++)
	{

		// if we have replacements, add them
		if(substitutions[n].replace_this && substitutions[n].replace_with)
		{

			// ensure replacement is reasonable
			if
			(
				BP_StringIsReasonablePrintableString(substitutions[n].replace_this, bpstrlen(substitutions[n].replace_this), BP_TRUE, BP_FALSE) &&
				BP_StringIsReasonablePrintableString(substitutions[n].replace_with, bpstrlen(substitutions[n].replace_with), BP_TRUE, BP_FALSE)
			)
			{

				// set find and replace
				sub_find[internal_idx]    = substitutions[n].replace_this;
				sub_replace[internal_idx] = substitutions[n].replace_with;

				// increment the internal index
				internal_idx++;

			}

		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Perform Substitutions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// // perform substitutions if necessary
	char * subbed_tmpl = (char *) NULL;
	if(internal_idx)
	{

		// attempt to create substituted template
		subbed_tmpl = BP_GenerateSubstitutedTemplate(request->response_buff, sub_find, sub_replace);

		// if we were able to generate the substituted template
		if(subbed_tmpl)
		{

			// response buffer
			if(request->response_buff)
			{
				bpfree(request->response_buff);
				request->response_buff_size = 0;
			}

			// set response (bpstrlen is fine due to previous sanity checks)
			request->response_buff      = subbed_tmpl;
			request->response_buff_size = bpstrlen(subbed_tmpl);

		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Final Checks and Destroy %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// destroy find and replace stacks if they were previously assigned
	if(sub_find)
		bpfree(sub_find);
	if(sub_replace)
		bpfree(sub_replace);

	// run final check
	if(request->response_buff)
	{

		// ensure the string is a reasonable printable (newlines/carrige rets ok)
		if(!BP_StringIsReasonablePrintableString(request->response_buff, request->response_buff_size, BP_TRUE, BP_TRUE))
		{

			// destroy the response buffer
			bpfree(request->response_buff);

			// set the response buffer to null
			request->response_buff      = NULL;
			request->response_buff_size = 0;

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}


// This routine is used for outputting a binary buffer directly to the page.  This is particularly
// useful for serving binary data such as images/tarballs/etc.
BP_ERROR_T BP_HTTPdRequestAppendBinaryBuffer
(
		P_BP_HTTPD_REQUEST                                request,
		char *                                            buffer,
		size_t                                            buffer_size
)
{

	// ensure we have a request
	if(!request)
		return ERR_FAILURE;
	if(!buffer)
		return ERR_FAILURE;
	if(!buffer_size)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Reallocate Buffer %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// new buffer space size (don't allocate any extra or it can lead to memory leaks)
	size_t new_buff_size = buffer_size;

	// attempt reallocating a buffer (plus some extra pad just in case)
	request->response_buff = (char *) bprealloc(request->response, request->response_buff_size + new_buff_size);

	// ensure we have response buffer allocated
	if(!request->response_buff)
		return ERR_FAILURE;

	// copy in the buffer directly (don't zero since copy is direct)
	memcpy((void *) &request->response_buff[request->response_buff_size], buffer, buffer_size);

	// increment the new response size
	request->response_buff_size = request->response_buff_size + new_buff_size;

	// return indicating success
	return ERR_SUCCESS;

}

// Send the login form.
BP_ERROR_T BP_HTTPdRequestResponseSendLoginForm
(
		P_BP_HTTPD_REQUEST      request,
		struct MHD_Connection  *connection,
		char *                  reason_for_login_header,
		char *                  reason_for_login_body,
		char *                  reason_for_login_footer
)
{

	// ensure we have a request
	if(!request)
		return ERR_FAILURE;

	// ensure we have a connection
	if(!connection)
		return ERR_FAILURE;

	// login callback
	if(!request->request_handler->login_callback)
		return ERR_FAILURE;

	// activate callback
	request->request_handler->login_callback
	(
			(void *) request,
			connection,
			reason_for_login_header,
			reason_for_login_body,
			reason_for_login_footer
	);

	// return indicating success
	return ERR_SUCCESS;

}


// Sends a prepared response.
BP_ERROR_T BP_HTTPdRequestResponseSendPrepared
(
		P_BP_HTTPD_REQUEST      request,
		struct MHD_Connection  *connection,
		BP_HTTPD_RESPONSE_CODE  http_response_code
)
{


	// ensure we have a request
	if(!request)
		return ERR_FAILURE;

	// ensure we have a daemon
	if(!request->httpd)
		return ERR_FAILURE;

	// ensure we have a connection
	if(!connection)
		return ERR_FAILURE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Response From Buffer %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create arbitrary response (mark for copy, this means the data will be
	// copied from the buffer before response, preventing any possibility
	// for immediate corruption.)
	request->response = MHD_create_response_from_buffer
	(
		  request->response_buff_size,
		  (void *) request->response_buff,
		  MHD_RESPMEM_MUST_COPY
	);

	// ensure we can respond
	if(!request->response)
		return ERR_FAILURE;

	// if we have a session, ensure it's valid
	P_BP_HTTPD_USER_SESSION user_session = (P_BP_HTTPD_USER_SESSION) request->session;
	if(user_session)
	{

		// run basic checks before setting cookie
		if
		(
				!BP_HTTPdSessionExpired(user_session) &&
				request->httpd->cookie_key            &&
				user_session->identifier
		)
		{

			// only send cookie if this is the first time we're starting a session
			if(!user_session->cookie_sent)
			{

				// array to build the cookie (ensure path is set to / or cookie wont work)
				char * cookie_array[] =
				{
						request->httpd->cookie_key,
						"=",
						user_session->identifier,
						"; path=/",
						NULL
				};

				// join the strings to create the final cookie
				char * cookie_str_final = BP_JoinStringArray(cookie_array, 4, NULL);

				// add the cookie
				MHD_add_response_header(request->response, MHD_HTTP_HEADER_SET_COOKIE, cookie_str_final);

				// set the cookie sent val to true
				user_session->cookie_sent = BP_TRUE;

			}

		}

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Extra Headers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we have extra headers, send them
	if(request->extra_headers)
	{

		// set lookup path
		char * headers_path[] = {"headers", NULL};

		// walk headers
		BP_HASH_ITER_FROM_LOOKUP_PATH_OPEN(request->extra_headers, (char **) &headers_path, header_iter)
		{

			// attempt to add response header here
			MHD_add_response_header(request->response, (char *) header_iter->key, (char *) header_iter->data);

		} BP_HASH_ITER_FROM_LOOKUP_PATH_CLOSE;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Queue Response and Send %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// queue the response out
	int ret = MHD_queue_response
	(
		  connection,
		  http_response_code,
		  request->response
	);

	// destroy the response
	MHD_destroy_response (request->response);

	// set to null after destruction
	request->response = NULL;

	// return indicating success
	return ERR_SUCCESS;


}




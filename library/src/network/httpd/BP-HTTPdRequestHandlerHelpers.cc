/*
 * BP-HTTPdRequestHandlerHelpers.cc
 *
 *  Created on: Apr 2, 2016
 *      Author: root
 */

// main include file
#include "../../../include/BP-Main.h"


/*
 * This file contains the rewrites for long-form macros found within BP-HTTPdHelperMacros.h.  This
 * rewrite occured because it was too difficult to expect developers to actually debug macros.  Macros
 * do not create their own stack frames, they do not debug like a normal application would.  I figured
 * there was some easy hack around it, because they are so absolutely useful, that maybe there was a hack
 * where you could debug them through runtime expansion.  There is not.  This, is awful, but can be
 * fixed with, sigh, a rewrite.  So, this file is that rewrite.
 */


// This function runs secure auth entry requests.  This is a rewrite of BP_HTTPD_REQUEST_HANDLER_SECURE_AUTH_ENTRY_SETUP_JSON.
BP_ERROR_T BP_HTTPDRequestHandlerSecureAuthEntrySetupJSON
(
		void                          * request, /* P_BP_HTTPD_REQUEST */
		struct MHD_Connection *         connection,
		const char *                    url,
		const char *                    method,
		const char *                    version,
		const char *                    upload_data,
		size_t     *                    upload_data_size,
		void       **                   con_cls
)
{

	// ensure we have a request parameter
	if(!request)
		return ERR_FAILURE;

	// convert type so we can have an easier time using it
	P_BP_HTTPD_REQUEST req = (P_BP_HTTPD_REQUEST) request;

	// ensure we have an attached httpd
	if(!req->httpd)
		return ERR_FAILURE;

	// ensure we have a connection
	if(!connection)
		return ERR_FAILURE;

	// ensure we have a url
	if(!url)
		return ERR_FAILURE;

	// ensure we have a HTTP method
	if(!method)
		return ERR_FAILURE;

	// ensure we have a HTTP version
	if(!version)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Request Handler Parameters %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// zero out the parameters buffer within the request (static sized)
	BP_ZERO_STACK(req->params);

	// set parameters directly, do not copy or modify these values, they are
	// here for reference use only.
	req->params.connection       = connection;
	req->params.url              = url;
	req->params.method           = method;
	req->params.version          = version;
	req->params.upload_data      = upload_data;
	req->params.upload_data_size = upload_data_size;
	req->params.con_cls          = con_cls;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Init JSON For Request and Run JSON Checks %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to create the JSON output
    req->json_output = BP_JSONCreate();

    // ensure we have an output managment structure
    if(!req->json_output)
	{
    	BP_HTTPdRequestHandlerRespondJSONFromStaticText(req, ERR_FAILURE, "{\n\t\"auth\": \"no_json_object_could_be_created\"\n}");
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run XSRF Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// override check pass token
	req->xsrf_checks_passed_ok = BP_FALSE;

	char * xsrf_var = "xsrf";

	// attempt to get the xsrf key var from either GET or POST
	P_BP_HTTPD_REQUEST_VAR xsrf_key_var = BP_HTTPdRequestVarArgRegistryLookup(req, BP_HTTPD_REQUEST_VAR_TYPE_GET, (char *) xsrf_var);
	if(!xsrf_key_var)
		xsrf_key_var = BP_HTTPdRequestVarArgRegistryLookup(req, BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA, (char *) xsrf_var);

	// set default check value to true, it will be disqualified below if necessary
	req->xsrf_checks_passed_ok = BP_TRUE;

	// if there is no key var
	if(!xsrf_key_var)
	{
		req->xsrf_checks_passed_ok = BP_FALSE;
	}

	// if the token hasn't been disqualified yet, check to ensure it's reasonable/printable
	if(req->xsrf_checks_passed_ok)
	{
		if(!BP_StringIsReasonablePrintableString(xsrf_key_var->data, xsrf_key_var->data_size, BP_FALSE, BP_FALSE))
		{
			req->xsrf_checks_passed_ok = BP_FALSE;
		}
	}

	// if the checks were passed ok
	if(req->xsrf_checks_passed_ok)
	{
		if(!BP_HTTPdSessionXSRFTokenLookup((P_BP_HTTPD_USER_SESSION) req->session, (char *) xsrf_key_var->data))
		{
			req->xsrf_checks_passed_ok = BP_FALSE;
		}
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Logout Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attampt to run logout checks
	P_BP_HTTPD_REQUEST_VAR logout_var = __POST(req, "logout");
	if(logout_var && req->xsrf_checks_passed_ok)
	{
		if(req->session)
		{
			BP_HTTPdSessionDestroy(req->httpd, (P_BP_HTTPD_USER_SESSION) req->session);
			req->session = NULL;
		}
		BP_HTTPdRequestHandlerRespondJSONFromStaticText(req, ERR_FAILURE, "{\n\t\"auth\": \"logged_out\"\n}");
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run U/P Login Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to run login checks
	P_BP_HTTPD_REQUEST_VAR user_var = __POST(req, "lu");
	P_BP_HTTPD_REQUEST_VAR pass_var = __POST(req, "lp");

	// if we have a user and pass var
	if(user_var && pass_var && req->xsrf_checks_passed_ok)
	{

		// ensure we have a user var and a pass var
		if(user_var && pass_var)
		{

			// destroy session if it exists
			if(req->session)
			{
				BP_HTTPdSessionDestroy(req->httpd, (P_BP_HTTPD_USER_SESSION) req->session);
				req->session = NULL;
			}

			// attempt to authenticate the user from creds if they're passed in
			req->session = BP_HTTPdSessionAuthUserFromCreds
			(
				req->httpd,
				user_var->data,
				pass_var->data
			);

			// if we are unable to create our session from user creds, exit immediately
			if(!req->session)
			{
				// send bad auth message
				BP_HTTPdRequestHandlerRespondJSONFromStaticText(req, ERR_FAILURE, "{\n\t\"auth\": \"bad_auth\"\n}");

			}

		}

	}




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run API Auth Login Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	char *api_key_name = "api_key";

	if(!req->session)
	{

		P_BP_HTTPD_REQUEST_VAR api_key_var = BP_HTTPdRequestVarArgRegistryLookup(req, BP_HTTPD_REQUEST_VAR_TYPE_GET, (char *) api_key_name);

		if(api_key_var)
		{

			if(!BP_StringIsReasonablePrintableString(api_key_var->data, api_key_var->data_size, BP_FALSE, BP_FALSE))
			{

				BP_HTTPdRequestHandlerRespondJSONFromStaticText(req, ERR_FAILURE, "{\n\t\"auth\": \"api_invalid_auth\"\n}");

			}

			P_BP_HTTPD_API_KEY api_key_lookup = BP_HTTPdSessionLookupAPIKeyByIdentifier
			(
				req->httpd,
				api_key_var->data
			);

			if(!api_key_lookup && !req->session)
			{

				BP_HTTPdRequestHandlerRespondJSONFromStaticText(req, ERR_FAILURE, "{\n\t\"auth\": \"api_bad_key\"\n}");

			}

			if(!req->session && api_key_lookup)
			{

				req->session               =   api_key_lookup->user_session;

				P_BP_HTTPD_USER_SESSION tmp_session = (P_BP_HTTPD_USER_SESSION) api_key_lookup->user_session;

				tmp_session->type                   =   BP_HTTPD_USER_SESSION_TYPE_AUTHENTICATED_API_KEY;

			}

		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Anon Session if All Other Session Creation Fails %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we have no session, attempt to create an anonymous guest session
	if(!req->session)
	{

			req->session = (void *) BP_HTTPdSessionCreateAnonymousGuest(req->httpd);
			if(!req->session)
			{

				// send bad anon guest message
				BP_HTTPdRequestHandlerRespondJSONFromStaticText(req, ERR_FAILURE, "{\n\t\"auth\": \"bad_anon_guest_auth\"\n}");

			}

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Session Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure we have a session
	if(!req->session)
	{
		BP_HTTPdRequestHandlerRespondJSONFromStaticText(req, ERR_FAILURE, "{\n\t\"auth\": \"no_session\"\n}");
	}


	if(BP_HTTPdSessionExpired((P_BP_HTTPD_USER_SESSION) req->session))
	{

		BP_HTTPdSessionDestroy(req->httpd, (P_BP_HTTPD_USER_SESSION) req->session);

		req->session = NULL;

		BP_HTTPD_CHECK_LOGIN   (req);

		BP_HTTPD_CHECK_API_AUTH(req, "api_key");

		if(!req->session)
		{

			BP_HTTPdRequestHandlerRespondJSONFromStaticText(req, ERR_FAILURE, "{\n\t\"auth\": \"no_session\"\n}");

		}

	}


	// return indicating success
	return ERR_SUCCESS;

}



// respond json from static text (this was originally BP_HTTPdRequestHandlerRespondJSONFromStaticText)
BP_ERROR_T BP_HTTPdRequestHandlerRespondJSONFromStaticText(P_BP_HTTPD_REQUEST req, BP_ERROR_T ret_val, char *ret_json)
{

	// basic null checks
	if(!req)
		return ERR_FAILURE;
	if(!ret_json)
		return ERR_FAILURE;

	// ensure we have json output
	if(req->json_output)
	{

		// set response type to json
		BP_HTTPdAddExtraResponseHeaderToRequest(req, "Content-Type",	"application/json; charset=utf8");

		// append the binary buffer to the output response
		BP_HTTPdRequestAppendBinaryBuffer
		(
				req,
				ret_json,
				bpstrlen(ret_json)
		);

		// send the output response
		BP_HTTPdRequestResponseSendPrepared
		(
			req,
			req->params.connection,
			BP_HTTPD_RESPONSE_CODE_200_OK
		);

		// return the defined return value
		return ret_val;

	}
	else
	{

		// set response to json
		BP_HTTPdAddExtraResponseHeaderToRequest(req, "Content-Type",	"application/json; charset=utf8");

		// append binary buffer
		BP_HTTPdRequestAppendBinaryBuffer
		(
				req,
				"general_request_failure",
				bpstrlen("general_request_failure")
		);

		// send the response to the client
		BP_HTTPdRequestResponseSendPrepared
		(
			req,
			req->params.connection,
			BP_HTTPD_RESPONSE_CODE_200_OK
		);

		// return the defined return value
		return ret_val;

	}

	// return indicating success (should technically never get here)
	return ERR_SUCCESS;

}

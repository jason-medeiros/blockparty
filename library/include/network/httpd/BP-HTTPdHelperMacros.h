#ifndef INCLUDE_NETWORK_HTTPD_BP_HTTPD_HELPER_MACROS_H_
#define INCLUDE_NETWORK_HTTPD_BP_HTTPD_HELPER_MACROS_H_


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Why Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Since most of these routines are going to be called from request handlers an
// absolutely immense number of times (billions probably), speed counts.  Macros
// force this code to appear inline, allowing us to generate a glue code which is
// both fast and useful.


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Request Gettr Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// create post data handler
#define __POST(request, data_index)\
	BP_HTTPdRequestVarArgRegistryLookup(req, BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA, (char *) data_index)

// create get data handler
#define __GET(request, data_index)\
	BP_HTTPdRequestVarArgRegistryLookup(req, BP_HTTPD_REQUEST_VAR_TYPE_GET, (char *) data_index)

// create session data handler (when the request gets to the handler, the session data has been populated ok already)
#define __SESSION(req, data_index)\
	BP_HTTPdSessionLookupDataByKey((P_BP_HTTPD_USER_SESSION) req->session, (char *) data_index) req->session)



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Return Data Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Return from request handler utilizing a static prepared buffer.
#define BP_HTTPD_REQUEST_HANDLER_RETURN_JSON_FROM_STATIC_TEXT(req, ret_val, ret_json)\
	if(req->json_output->json_out)\
	{                                                                                                           \
		/* set response header to application/json */                                                           \
		BP_HTTPdAddExtraResponseHeaderToRequest(req, "Content-Type",	"application/json; charset=utf8");      \
		/* append the binary buffer */                                                                          \
		BP_HTTPdRequestAppendBinaryBuffer                                                                       \
		(                                                                                                       \
				req,                                                                                            \
				ret_json,                                                                                       \
				bpstrlen(ret_json)                                                                                \
		);                                                                                                      \
		/* send the prepared response */                                                                        \
		BP_HTTPdRequestResponseSendPrepared                                                                     \
		(                                                                                                       \
			req,                                                                                                \
			connection,                                                                                         \
			BP_HTTPD_RESPONSE_CODE_200_OK                                                                       \
		); 																										\
		return ret_val;\
	}\
	else\
	{\
		/* set response header to application/json */                                                           \
		BP_HTTPdAddExtraResponseHeaderToRequest(req, "Content-Type",	"application/json; charset=utf8");      \
		/* append the binary buffer */                                                                          \
		BP_HTTPdRequestAppendBinaryBuffer                                                                       \
		(                                                                                                       \
				req,                                                                                            \
				"general_request_failure",                                                                      \
				bpstrlen("general_request_failure")                                                               \
		);                                                                                                      \
		/* send the prepared response */                                                                        \
		BP_HTTPdRequestResponseSendPrepared                                                                     \
		(                                                                                                       \
			req,                                                                                                \
			connection,                                                                                         \
			BP_HTTPD_RESPONSE_CODE_200_OK                                                                       \
		); 																										\
		return ret_val;                                                                                         \
	}


// Return from a request handler using a json object found within the request itself.  The request
// is destroyed after exit, and the json along with it.
#define BP_HTTPD_REQUEST_HANDLER_RETURN_JSON(req, ret_val)\
		/* set response header to application/json */                                                           \
		BP_HTTPdAddExtraResponseHeaderToRequest(req, "Content-Type",	"application/json; charset=utf8");      \
		/* finalize output */                                                                                   \
		BP_JSONFinalizeOutput(req->json_output);                                                                \
		/* append the binary buffer */                                                                          \
		BP_HTTPdRequestAppendBinaryBuffer                                                                       \
		(                                                                                                       \
				req,                                                                                            \
				req->json_output->json_out,                                                                     \
				bpstrlen(req->json_output->json_out)                                                              \
		);                                                                                                      \
		/* send the prepared response */                                                                        \
		BP_HTTPdRequestResponseSendPrepared                                                                     \
		(                                                                                                       \
			req,                                                                                                \
			connection,                                                                                         \
			BP_HTTPD_RESPONSE_CODE_200_OK                                                                       \
		); 																										\
		return ret_val;

// simple macro to return text (not json) from a handler.
#define BP_HTTPD_REQUEST_HANDLER_RETURN_TEXT(req, ret_val, ret_text)\
		/* set response header to application/json */                                                           \
		BP_HTTPdAddExtraResponseHeaderToRequest(req, "Content-Type",	"application/text; charset=utf8");      \
		/* append the binary buffer */                                                                          \
		BP_HTTPdRequestAppendBinaryBuffer                                                                       \
		(                                                                                                       \
				req,                                                                                            \
				ret_text,                                                                                       \
				bpstrlen(ret_text)                                                                                \
		);                                                                                                      \
		/* send the prepared response */                                                                        \
		BP_HTTPdRequestResponseSendPrepared                                                                     \
		(                                                                                                       \
			req,                                                                                                \
			connection,                                                                                         \
			BP_HTTPD_RESPONSE_CODE_200_OK                                                                       \
		); 																										\
		return ret_val;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Auth Helper Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// These macros are designed to help assist with authentication
// related functionality for blockparty httpd servers.


// This attempts to see if a user has passed in a posted "logout" parameter.  If that's the
// case, go ahead and process logout.
#define BP_HTTPD_CHECK_LOGOUT(req)\
	P_BP_HTTPD_REQUEST_VAR logout_var = __POST(req, "logout");                                                      \
	if(logout_var && req->xsrf_checks_passed_ok)                                                                    \
	{                                                                                                               \
		if(req->session)                                                                                            \
		{                                                                                                           \
			BP_HTTPdSessionDestroy(req->httpd, (P_BP_HTTPD_USER_SESSION) req->session);                             \
			req->session = NULL;                                                                                    \
		}                                                                                                           \
		BP_HTTPD_REQUEST_HANDLER_RETURN_JSON_FROM_STATIC_TEXT(req, ERR_FAILURE, "{\n\t\"auth\": \"logged_out\"\n}");\
	}


// attempts to check login to see if the user is logging in
#define BP_HTTPD_CHECK_LOGIN(req)\
/* check for user and pass vars */                                                         \
	P_BP_HTTPD_REQUEST_VAR user_var = __POST(req, "lu");                                   \
	P_BP_HTTPD_REQUEST_VAR pass_var = __POST(req, "lp");                                   \
	/* if we have a user and pass var */                                                   \
	if(user_var && pass_var && req->xsrf_checks_passed_ok)                                                               \
	{                                                                                      \
		if(user_var && pass_var)                                                           \
		{                                                                                  \
			/* destroy session if it exists */                                             \
			if(req->session)                                                               \
			{                                                                              \
				BP_HTTPdSessionDestroy(req->httpd, (P_BP_HTTPD_USER_SESSION) req->session);\
				req->session = NULL;                                                       \
			}                                                                              \
			/* attempt to authenticate the user from creds if they're passed in */         \
			req->session = BP_HTTPdSessionAuthUserFromCreds                                \
			(                                                                              \
				req->httpd,                                                                \
				user_var->data,                                                            \
				pass_var->data                                                             \
			);                                                                             \
			if(!req->session)                                                              \
			{                                                                              \
				/* send bad auth message */                                                \
				BP_HTTPD_REQUEST_HANDLER_RETURN_JSON_FROM_STATIC_TEXT(req, ERR_FAILURE, "{\n\t\"auth\": \"bad_auth\"\n}");\
			}                                                                              \
		}                                                                                  \
	}                                                                                      \
	if(!req->session)                                                                      \
	{                                                                                      \
		                                                                                   \
			req->session = (void *) BP_HTTPdSessionCreateAnonymousGuest(req->httpd);       \
			if(!req->session)                                                              \
			{                                                                              \
				/* send bad auth message */                                                \
				BP_HTTPD_REQUEST_HANDLER_RETURN_JSON_FROM_STATIC_TEXT(req, ERR_FAILURE, "{\n\t\"auth\": \"bad_anon_guest_auth\"\n}");\
			}                                                                              \
	}

// This runs API authentication checks if an API key is present.
#define BP_HTTPD_CHECK_API_AUTH(auth_request, api_key_name) \
	if(!auth_request->session)                                                                                                                         \
	{                                                                                                                                                  \
		P_BP_HTTPD_REQUEST_VAR api_key_var = BP_HTTPdRequestVarArgRegistryLookup(auth_request, BP_HTTPD_REQUEST_VAR_TYPE_GET, (char *) api_key_name);  \
		if(api_key_var)                                                                                                                                \
		{                                                                                                                                              \
			if(!BP_StringIsReasonablePrintableString(api_key_var->data, api_key_var->data_size, BP_FALSE, BP_FALSE))                                   \
			{                                                                                                                                          \
				BP_HTTPD_REQUEST_HANDLER_RETURN_JSON_FROM_STATIC_TEXT(auth_request, ERR_FAILURE, "{\n\t\"auth\": \"api_invalid_auth\"\n}");                             \
			}                                                                                                                                          \
			P_BP_HTTPD_API_KEY api_key_lookup = BP_HTTPdSessionLookupAPIKeyByIdentifier                                                                \
			(                                                                                                                                          \
				auth_request->httpd,                                                                                                                   \
				api_key_var->data                                                                                                                      \
			);                                                                                                                                         \
			if(!api_key_lookup && !auth_request->session)                                                                                              \
			{                                                                                                                                          \
				BP_HTTPD_REQUEST_HANDLER_RETURN_JSON_FROM_STATIC_TEXT(auth_request, ERR_FAILURE, "{\n\t\"auth\": \"api_bad_key\"\n}");                 \
			}                                                                                                                                          \
			if(!auth_request->session && api_key_lookup)                                                                                               \
			{                                                                                                                                          \
				auth_request->session               =   api_key_lookup->user_session;                                                                  \
				P_BP_HTTPD_USER_SESSION tmp_session = (P_BP_HTTPD_USER_SESSION) api_key_lookup->user_session;                                          \
				tmp_session->type                   =   BP_HTTPD_USER_SESSION_TYPE_AUTHENTICATED_API_KEY;                                              \
			}                                                                                                                                          \
		}                                                                                                                                              \
	}


// run all xsrf checks
#define BP_HTTPD_RUN_XSRF_CHECKS(req, xsrf_var)\
		/* overwrite check pass flag on macro init */                                                                                             \
		req->xsrf_checks_passed_ok = BP_FALSE;                                                                                                    \
		/* pull xsrf var from get or post */		                                                                                              \
		P_BP_HTTPD_REQUEST_VAR xsrf_key_var = BP_HTTPdRequestVarArgRegistryLookup(req, BP_HTTPD_REQUEST_VAR_TYPE_GET, (char *) xsrf_var);         \
		if(!xsrf_key_var)                                                                                                                         \
			xsrf_key_var = BP_HTTPdRequestVarArgRegistryLookup(req, BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA, (char *) xsrf_var);                      \
			                                                                                                                                      \
        req->xsrf_checks_passed_ok = BP_TRUE;                                                                                                     \
		/* if we don't have a token lookup, exit immediately */                                                                                   \
		if(!xsrf_key_var)																													      \
		{                                                                                                                                         \
			req->xsrf_checks_passed_ok = BP_FALSE;                                                                                                \
		}                                                                                                                                         \
		if(req->xsrf_checks_passed_ok)                                                                                                            \
		if(!BP_StringIsReasonablePrintableString(xsrf_key_var->data, xsrf_key_var->data_size, BP_FALSE, BP_FALSE))                                \
		{                                                                                                                                         \
			req->xsrf_checks_passed_ok = BP_FALSE;                                                                                                \
		}                                                                                                                                         \
		if(req->xsrf_checks_passed_ok)                                                                                                            \
		if(!BP_HTTPdSessionXSRFTokenLookup((P_BP_HTTPD_USER_SESSION) req->session, (char *) xsrf_key_var->data))                                  \
		{                                                                                                                                         \
			req->xsrf_checks_passed_ok = BP_FALSE;                                                                                                \
		}                                                                                                                                         \




// run all authentication checks
#define BP_HTTPD_RUN_AUTH_CHECKS(req)\
	{   \
		BP_HTTPD_RUN_XSRF_CHECKS(req, "xsrf");                                                                               \
		BP_HTTPD_CHECK_LOGOUT  (req);                                                                                        \
		BP_HTTPD_CHECK_LOGIN   (req);                                                                                        \
		BP_HTTPD_CHECK_API_AUTH(req, "api_key");                                                                             \
		if(!req->session)                                                                                                    \
		{                                                                                                                    \
			BP_HTTPD_REQUEST_HANDLER_RETURN_JSON_FROM_STATIC_TEXT(req, ERR_FAILURE, "{\n\t\"auth\": \"no_session\"\n}");     \
		}                                                                                                                    \
		if(BP_HTTPdSessionExpired((P_BP_HTTPD_USER_SESSION) req->session))                                                   \
		{                                                                                                                    \
			BP_HTTPdSessionDestroy(req->httpd, (P_BP_HTTPD_USER_SESSION) req->session);                                      \
			req->session = NULL;                                                                                             \
			BP_HTTPD_CHECK_LOGIN   (req);                                                                                    \
			BP_HTTPD_CHECK_API_AUTH(req, "api_key");                                                                         \
			if(!req->session)                                                                                                \
			{                                                                                                                \
				BP_HTTPD_REQUEST_HANDLER_RETURN_JSON_FROM_STATIC_TEXT(req, ERR_FAILURE, "{\n\t\"auth\": \"no_session\"\n}"); \
			}                                                                                                                \
		}                                                                                                                    \
	}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Handler Init Environments %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// init json output for json request
#define BP_HTTPD_REQUEST_HANDLER_INIT_JSON(req)\
        req->json_output = BP_JSONCreate();                                                                                                  \
        if(!req->json_output)                                                                                                                \
		{                                                                                                                                    \
        	BP_HTTPD_REQUEST_HANDLER_RETURN_JSON_FROM_STATIC_TEXT(req, ERR_FAILURE, "{\n\t\"auth\": \"no_json_object_could_be_created\"\n}");\
		}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% General Request Handler Entry Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This auto-sets-up the correct working environment for a request handler which will respond back
// with JSON.
#define BP_HTTPD_REQUEST_HANDLER_SECURE_AUTH_ENTRY_SETUP_JSON\
	BP_REQUEST_HANDLER_BASIC_PROLOGUE;          \
	BP_HTTPD_RUN_AUTH_CHECKS(req);	            \
	BP_HTTPD_REQUEST_HANDLER_INIT_JSON(req);    \



#endif //INCLUDE_NETWORK_HTTPD_BP_HTTPD_HELPER_MACROS_H_

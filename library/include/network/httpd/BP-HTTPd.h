/*
 * BP-HTTPd.h
 *
 *  Created on: Jul 6, 2015
 *      Author: root
 */

#ifndef INCLUDE_NETWORK_HTTPD_BP_HTTPD_H_
#define INCLUDE_NETWORK_HTTPD_BP_HTTPD_H_


// This is used to determine the size of options
// which will be available for each httpd.
#define BP_MHD_MAX_HTTPD_OPTIONS 64



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Library Default HTML Templates %%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// default login form
#define BP_HTTPD_TMPL_DEFAULT_LOGIN_FORM\
	"<table border=0 cellpadding=0 cellspacing=0 class=\"{login_form_css_class}\">"\
	"<form method=POST>"\
	"<tr><td>login:<td> <input type=textbox name=\"{login_form_user_input_name}\"> "\
	"<tr><td>password:<td> <input type=password name=\"{login_form_pass_input_name}\"> "\
	"<tr><td>&nbsp;<td><input type=submit value='login'></input>"\
	"</form>"\
	"</table>\0"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Response Codes %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Restricted enumeration.
typedef enum
{
	BP_HTTPD_RESPONSE_CODE_UNSET                                    = 0,
	BP_HTTPD_RESPONSE_CODE_100_CONTINUE                             = MHD_HTTP_CONTINUE,
	BP_HTTPD_RESPONSE_CODE_101_SWITCHING_PROTOCOLS                  = MHD_HTTP_SWITCHING_PROTOCOLS,
	BP_HTTPD_RESPONSE_CODE_102_PROCESSING                           = MHD_HTTP_PROCESSING,
	BP_HTTPD_RESPONSE_CODE_200_OK                                   = MHD_HTTP_OK,
	BP_HTTPD_RESPONSE_CODE_201_CREATED                              = MHD_HTTP_CREATED,
	BP_HTTPD_RESPONSE_CODE_202_ACCEPTED                             = MHD_HTTP_ACCEPTED,
	BP_HTTPD_RESPONSE_CODE_203_NON_AUTHORITATAVE_INFORMATION        = MHD_HTTP_NON_AUTHORITATIVE_INFORMATION,
	BP_HTTPD_RESPONSE_CODE_204_NO_CONTENT                           = MHD_HTTP_NO_CONTENT,
	BP_HTTPD_RESPONSE_CODE_205_RESET_CONTENT                        = MHD_HTTP_RESET_CONTENT,
	BP_HTTPD_RESPONSE_CODE_206_PARTIAL_CONTENT                      = MHD_HTTP_PARTIAL_CONTENT,
	BP_HTTPD_RESPONSE_CODE_207_MULTI_STATUS                         = MHD_HTTP_MULTI_STATUS,
	BP_HTTPD_RESPONSE_CODE_300_MULTIPLE_CHOICES                     = MHD_HTTP_MULTIPLE_CHOICES,
	BP_HTTPD_RESPONSE_CODE_301_MOVED_PERMANENTLY                    = MHD_HTTP_MOVED_PERMANENTLY,
	BP_HTTPD_RESPONSE_CODE_302_FOUND                                = MHD_HTTP_FOUND,
	BP_HTTPD_RESPONSE_CODE_303_SEE_OTHER                            = MHD_HTTP_SEE_OTHER,
	BP_HTTPD_RESPONSE_CODE_304_NOT_MODIFIED                         = MHD_HTTP_NOT_MODIFIED,
	BP_HTTPD_RESPONSE_CODE_305_USE_PROXY                            = MHD_HTTP_USE_PROXY,
	BP_HTTPD_RESPONSE_CODE_306_SWITCH_PROXY                         = MHD_HTTP_SWITCH_PROXY,
	BP_HTTPD_RESPONSE_CODE_307_TEMPORARY_REDIRECT                   = MHD_HTTP_TEMPORARY_REDIRECT,
	BP_HTTPD_RESPONSE_CODE_400_BAD_REQUEST                          = MHD_HTTP_BAD_REQUEST,
	BP_HTTPD_RESPONSE_CODE_401_UNAUTHORIZED                         = MHD_HTTP_UNAUTHORIZED,
	BP_HTTPD_RESPONSE_CODE_402_PAYMENT_REQUIRED                     = MHD_HTTP_PAYMENT_REQUIRED,
	BP_HTTPD_RESPONSE_CODE_403_FORBIDDEN                            = MHD_HTTP_FORBIDDEN,
	BP_HTTPD_RESPONSE_CODE_404_NOT_FOUND                            = MHD_HTTP_NOT_FOUND,
	BP_HTTPD_RESPONSE_CODE_405_METHOD_NOT_ALLOWED                   = MHD_HTTP_METHOD_NOT_ALLOWED,
	BP_HTTPD_RESPONSE_CODE_406_METHOD_NOT_ACCEPTABLE                = MHD_HTTP_METHOD_NOT_ACCEPTABLE,
	BP_HTTPD_RESPONSE_CODE_407_PROXY_AUTHENTICATION_REQUIRED        = MHD_HTTP_PROXY_AUTHENTICATION_REQUIRED,
	BP_HTTPD_RESPONSE_CODE_408_REQUEST_TIMEOUT                      = MHD_HTTP_REQUEST_TIMEOUT,
	BP_HTTPD_RESPONSE_CODE_409_CONFLICT                             = MHD_HTTP_CONFLICT,
	BP_HTTPD_RESPONSE_CODE_410_GONE                                 = MHD_HTTP_GONE,
	BP_HTTPD_RESPONSE_CODE_411_LENGTH_REQUIRED                      = MHD_HTTP_LENGTH_REQUIRED,
	BP_HTTPD_RESPONSE_CODE_412_PRECONDITION_FAILED                  = MHD_HTTP_PRECONDITION_FAILED,
	BP_HTTPD_RESPONSE_CODE_413_REQUEST_ENTITY_TOO_LARGE             = MHD_HTTP_REQUEST_ENTITY_TOO_LARGE,
	BP_HTTPD_RESPONSE_CODE_414_REQUEST_URI_TOO_LONG                 = MHD_HTTP_REQUEST_URI_TOO_LONG,
	BP_HTTPD_RESPONSE_CODE_415_UNSUPPORTED_MEDIA_TYPE               = MHD_HTTP_UNSUPPORTED_MEDIA_TYPE,
	BP_HTTPD_RESPONSE_CODE_416_REQUESTED_RANGE_NOT_SATISFIABLE      = MHD_HTTP_REQUESTED_RANGE_NOT_SATISFIABLE,
	BP_HTTPD_RESPONSE_CODE_417_EXPECTATION_FAILED                   = MHD_HTTP_EXPECTATION_FAILED,
	BP_HTTPD_RESPONSE_CODE_422_UNPROCESSABLE_ENTITY                 = MHD_HTTP_UNPROCESSABLE_ENTITY,
	BP_HTTPD_RESPONSE_CODE_423_LOCKED                               = MHD_HTTP_LOCKED,
	BP_HTTPD_RESPONSE_CODE_424_FAILED_DEPENDENCY                    = MHD_HTTP_FAILED_DEPENDENCY,
	BP_HTTPD_RESPONSE_CODE_425_UNORDERED_COLLECTION                 = MHD_HTTP_UNORDERED_COLLECTION,
	BP_HTTPD_RESPONSE_CODE_426_UPGRADE_REQUIRED                     = MHD_HTTP_UPGRADE_REQUIRED,
	BP_HTTPD_RESPONSE_CODE_444_NO_RESPONSE                          = MHD_HTTP_NO_RESPONSE,
	BP_HTTPD_RESPONSE_CODE_449_RETRY_WITH                           = MHD_HTTP_RETRY_WITH,
	BP_HTTPD_RESPONSE_CODE_450_BLOCKED_BY_WINDOWS_PARENTAL_CONTROLS = MHD_HTTP_BLOCKED_BY_WINDOWS_PARENTAL_CONTROLS,
	BP_HTTPD_RESPONSE_CODE_451_UNAVAILABLE_FOR_LEGAL_REASONS        = MHD_HTTP_UNAVAILABLE_FOR_LEGAL_REASONS,
	BP_HTTPD_RESPONSE_CODE_500_INTERNAL_SERVER_ERROR                = MHD_HTTP_INTERNAL_SERVER_ERROR,
	BP_HTTPD_RESPONSE_CODE_501_NOT_IMPLEMENTED                      = MHD_HTTP_NOT_IMPLEMENTED,
	BP_HTTPD_RESPONSE_CODE_502_BAD_GATEWAY                          = MHD_HTTP_BAD_GATEWAY,
	BP_HTTPD_RESPONSE_CODE_503_SERVICE_UNAVAILABLE                  = MHD_HTTP_SERVICE_UNAVAILABLE,
	BP_HTTPD_RESPONSE_CODE_504_GATEWAY_TIMEOUT                      = MHD_HTTP_GATEWAY_TIMEOUT,
	BP_HTTPD_RESPONSE_CODE_505_HTTP_VERSION_NOT_SUPPORTED           = MHD_HTTP_HTTP_VERSION_NOT_SUPPORTED,
	BP_HTTPD_RESPONSE_CODE_506_VARIANT_ALSO_NEGOTIATES              = MHD_HTTP_VARIANT_ALSO_NEGOTIATES,
	BP_HTTPD_RESPONSE_CODE_507_INSUFFICIENT_STORAGE                 = MHD_HTTP_INSUFFICIENT_STORAGE,
	BP_HTTPD_RESPONSE_CODE_509_BANDWIDTH_LIMIT_EXCEEDED             = MHD_HTTP_BANDWIDTH_LIMIT_EXCEEDED,
	BP_HTTPD_RESPONSE_CODE_510_NOT_EXTENDED                         = MHD_HTTP_NOT_EXTENDED

} BP_HTTPD_RESPONSE_CODE;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Quality of Life Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Macro to validate the we have a correct working environment within the
// global hash registry.
#define BP_HTTPD_VALIDATE_WORKING_ENVIRONMENT(ret) \
	if(BP_HTTPdEnsureWorkingGlobalHashTableEnvirionment() == ERR_FAILURE) \
	{\
		printf("\n [!!] Error: Could not validate semaphore working environment within global hash table.  Critical error.");\
		return ret;\
	}



// --- HTTP Method Check Macros ---

// HTTP POST method check
#define BP_HTTPD_METHOD_IS_POST(method) \
	( !strcmp(method, MHD_HTTP_METHOD_POST) ? BP_TRUE : BP_FALSE )

// HTTP GET method check
#define BP_HTTPD_METHOD_IS_GET(method) \
	( !strcmp(method, MHD_HTTP_METHOD_GET) ? BP_TRUE : BP_FALSE )

// HTTP PUT method check
#define BP_HTTPD_METHOD_IS_PUT(method) \
	( !strcmp(method, MHD_HTTP_METHOD_PUT) ? BP_TRUE : BP_FALSE )

// HTTP CONNECT method check
#define BP_HTTPD_METHOD_IS_CONNECT(method) \
	( !strcmp(method, MHD_HTTP_METHOD_CONNECT) ? BP_TRUE : BP_FALSE )

// HTTP DELETE method check
#define BP_HTTPD_METHOD_IS_DELETE(method) \
	( !strcmp(method, MHD_HTTP_METHOD_DELETE) ? BP_TRUE : BP_FALSE )

// HTTP HEAD method check
#define BP_HTTPD_METHOD_IS_HEAD(method) \
	( !strcmp(method, MHD_HTTP_METHOD_HEAD) ? BP_TRUE : BP_FALSE )

// HTTP OPTIONS method check
#define BP_HTTPD_METHOD_IS_OPTIONS(method) \
	( !strcmp(method, MHD_HTTP_METHOD_OPTIONS) ? BP_TRUE : BP_FALSE )

// HTTP TRACE method check
#define BP_HTTPD_METHOD_IS_TRACE(method) \
	( !strcmp(method, MHD_HTTP_METHOD_TRACE) ? BP_TRUE : BP_FALSE )


// --- HTTP Form Mime Type Check Macros ---



// form urlencoded
#define BP_HTTPD_CONTENT_TYPE_APPLICATION_X_WWW_FORM_URLENCODED "application/x-www-form-urlencoded"

// file upload octet stream
#define BP_HTTPD_CONTENT_TYPE_APPLICATION_OCTET_STREAM "application/octet-stream"

// form multipart uploads
#define BP_HTTPD_CONTENT_TYPE_MULTIPART_FORM_DATA "multipart/form-data"


// application/x-www-form-urlencoded check macro
#define BP_HTTPD_CONTENT_TYPE_IS_APPLICATION_X_WWW_FORM_URLENCODED(encoding) \
		( !strcmp(encoding, BP_HTTPD_CONTENT_TYPE_APPLICATION_X_WWW_FORM_URLENCODED) ? BP_TRUE : BP_FALSE )

// application/octet-stream check macro
#define BP_HTTPD_CONTENT_TYPE_IS_APPLICATION_OCTET_STREAM(encoding) \
		( !strcmp(encoding, BP_HTTPD_CONTENT_TYPE_APPLICATION_OCTET_STREAM) ? BP_TRUE : BP_FALSE )

// multipart/form-data check macro
#define BP_HTTPD_POST_ENCODING_IS_MULTIPART_FORM_DATA(encoding) \
		( !strcmp(encoding, BP_HTTPD_CONTENT_TYPE_MULTIPART_FORM_DATA) ? BP_TRUE : BP_FALSE )


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Request Handler Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// basic prologue to admin api server tests
#define BP_REQUEST_HANDLER_BASIC_PROLOGUE \
	BP_HTTPD_ENTER_REQUEST_HANDLER;                        \
	if(!request)                                           \
		return ERR_FAILURE;                                \
	P_BP_HTTPD_REQUEST req = (P_BP_HTTPD_REQUEST) request; \
	if(!req->httpd)                                        \
		return ERR_FAILURE;                                \
	if(!connection)                                        \
		return ERR_FAILURE;                                \
	if(!url)                                               \
		return ERR_FAILURE;                                \
	if(!method)                                            \
		return ERR_FAILURE;                                \
	if(!version)                                           \
		return ERR_FAILURE;

// define macro for adding the default request handler
#define BP_HTTPD_ADD_DEFAULT_REQUEST_HANDLER(httpd, handler_prefix, api_handler)\
		BP_FLAGS_T api_handler##supported_methods = 0; \
        BP_SET_FLAG(api_handler##supported_methods, BP_HTTPD_METHOD_GET);\
        BP_SET_FLAG(api_handler##supported_methods, BP_HTTPD_METHOD_POST);\
        BP_HTTPD_REQUEST_HANDLER_SETUP_CONFIGS api_handler##_setup_configs;\
        api_handler##_setup_configs.supported_http_methods                  = api_handler##supported_methods;\
        api_handler##_setup_configs.posted_max_data_size_before_autofail    = BP_KILOBYTE * 500;\
        api_handler##_setup_configs.posted_data_chunk_read_size             = BP_KILOBYTE * 2;\
        api_handler##_setup_configs.posted_data_tmp_directory_literal_path  = (char *)"/tmp/blockpartytmp/";\
        api_handler##_setup_configs.use_tmpfile_for_multipart_form_data     = BP_TRUE;\
        api_handler##_setup_configs.use_tmpfile_for_octet_stream            = BP_TRUE;\
        api_handler##_setup_configs.http_get_max_key_len                    = 100;\
        api_handler##_setup_configs.http_get_max_val_len                    = 100;\
        api_handler##_setup_configs.http_cookie_max_key_len                 = 200;\
        api_handler##_setup_configs.http_cookie_max_val_len                 = 200; \
        api_handler##_setup_configs.http_header_max_count                   = 30;  \
        api_handler##_setup_configs.http_header_max_key_len                 = 100; \
        api_handler##_setup_configs.http_header_max_val_len                 = 200; \
        BP_HTTPdRequestHandlerAdd                                                  \
        (                                                                          \
                        httpd,                                                     \
                        (char *) "/",                                              \
                        handler_prefix ## _ ##api_handler,                         \
                        &api_handler##_setup_configs,                              \
                        NULL,                                                      \
                        0,                                                         \
                        NULL                                                       \
        );

// macro to add an existing request handler to a httpd add a request handler
#define BP_HTTPD_ADD_REQUEST_HANDLER(httpd, handler_prefix, api_handler) \
        BP_FLAGS_T api_handler##supported_methods = 0; \
        BP_SET_FLAG(api_handler##supported_methods, BP_HTTPD_METHOD_GET);\
        BP_SET_FLAG(api_handler##supported_methods, BP_HTTPD_METHOD_POST);\
        BP_HTTPD_REQUEST_HANDLER_SETUP_CONFIGS api_handler##_setup_configs;\
        api_handler##_setup_configs.supported_http_methods                  = api_handler##supported_methods;\
        api_handler##_setup_configs.posted_max_data_size_before_autofail    = BP_KILOBYTE * 500;\
        api_handler##_setup_configs.posted_data_chunk_read_size             = BP_KILOBYTE * 2;\
        api_handler##_setup_configs.posted_data_tmp_directory_literal_path  = (char *)"/tmp/blockpartytmp/";\
        api_handler##_setup_configs.use_tmpfile_for_multipart_form_data     = BP_TRUE;\
        api_handler##_setup_configs.use_tmpfile_for_octet_stream            = BP_TRUE;\
        api_handler##_setup_configs.http_get_max_key_len                    = 100;\
        api_handler##_setup_configs.http_get_max_val_len                    = 100;\
        api_handler##_setup_configs.http_cookie_max_key_len                 = 200;\
        api_handler##_setup_configs.http_cookie_max_val_len                 = 200; \
        api_handler##_setup_configs.http_header_max_count                   = 30;  \
        api_handler##_setup_configs.http_header_max_key_len                 = 100; \
        api_handler##_setup_configs.http_header_max_val_len                 = 200; \
        BP_HTTPdRequestHandlerAdd                                                  \
        (                                                                          \
                        httpd,                                                     \
                        (char *) "/"#api_handler"/",                               \
                        handler_prefix ## _ ##api_handler,                         \
                        &api_handler##_setup_configs,                              \
                        NULL,                                                      \
                        0,                                                         \
                        NULL                                                       \
        );



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Httpd Critical Section Macros %%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// enter critical section
#define BP_HTTPD_ENTER_CRITICAL_SECTION\
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(httpd->system_hreg);

// leave critical section
#define BP_HTTPD_LEAVE_CRITICAL_SECTION\
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(httpd->system_hreg);

// simple utility to leave critical section and return from function
#define BP_HTTPD_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(val)\
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(httpd->system_hreg, val);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% General Enumerations %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Server state enumeration typedef
typedef enum
{

	BP_HTTPD_SERVER_STATE_UNSET,
	BP_HTTPD_SERVER_STATE_STARTED,
	BP_HTTPD_SERVER_STATE_QUIETED,
	BP_HTTPD_SERVER_STATE_STOPPED

} BP_HTTPD_SERVER_STATE;

// type associated with a user session
typedef enum
{

	BP_HTTPD_USER_SESSION_TYPE_UNSET,
	BP_HTTPD_USER_SESSION_TYPE_ANONYMOUS_GUEST,
	BP_HTTPD_USER_SESSION_TYPE_AUTHENTICATED_USER,
	BP_HTTPD_USER_SESSION_TYPE_AUTHENTICATED_API_KEY

} BP_HTTPD_USER_SESSION_TYPE;

// Enumerator for the different types of post requests.
typedef enum
{

	BP_HTTPD_METHOD_UNSET,
	BP_HTTPD_METHOD_GET,
	BP_HTTPD_METHOD_POST,
	BP_HTTPD_METHOD_PUT,
	BP_HTTPD_METHOD_CONNECT,
	BP_HTTPD_METHOD_DELETE,
	BP_HTTPD_METHOD_HEAD,
	BP_HTTPD_METHOD_OPTIONS,
	BP_HTTPD_METHOD_TRACE

} BP_HTTPD_METHOD;


typedef enum
{

	BP_HTTPD_REQUEST_TLS_VERSION_UNSET,
	BP_HTTPD_REQUEST_TLS_VERSION_SSLV3,
	BP_HTTPD_REQUEST_TLS_VERSION_TLS1_0,
	BP_HTTPD_REQUEST_TLS_VERSION_TLS1_1,
	BP_HTTPD_REQUEST_TLS_VERSION_TLS1_2,
	BP_HTTPD_REQUEST_TLS_VERSION_TLS1_BADVAL

} BP_HTTPD_REQUEST_TLS_VERSION;


typedef enum
{

	BP_HTTPD_REQUEST_CONTENT_TYPE_UNSET,
	BP_HTTPD_REQUEST_CONTENT_TYPE_APPLICATION_X_WWW_FORM_URLENCODED,
	BP_HTTPD_REQUEST_CONTENT_TYPE_APPLICATION_OCTET_STREAM,
	BP_HTTPD_REQUEST_CONTENT_TYPE_MULTIPART_FORM_DATA

} BP_HTTPD_REQUEST_CONTENT_TYPE;


// the type of variable (used as confirmation when retrieving vars from tables)
typedef enum
{

	BP_HTTPD_REQUEST_VAR_TYPE_UNSET,
	BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA,
	BP_HTTPD_REQUEST_VAR_TYPE_POST_TMPFILE,
	BP_HTTPD_REQUEST_VAR_TYPE_GET,
	BP_HTTPD_REQUEST_VAR_TYPE_COOKIE,
	BP_HTTPD_REQUEST_VAR_TYPE_HEADER

} BP_HTTPD_REQUEST_VAR_TYPE;



// User access level enumeration
typedef enum
{

	BP_HTTPD_USER_ACCESS_UNSET,
	BP_HTTPD_USER_ACCESS_ANONYMOUS_GUEST,
	BP_HTTPD_USER_ACCESS_GOD,
	BP_HTTPD_USER_ACCESS_DEMIGOD,
	BP_HTTPD_USER_ACCESS_SEMIGOD,
	BP_HTTPD_USER_ACCESS_MORTAL,
	BP_HTTPD_USER_ACCESS_GUEST

} BP_HTTPD_USER_ACCESS;

// User type enumeration.
typedef enum
{

	BP_HTTPD_USER_TYPE_UNSET,
	BP_HTTPD_USER_TYPE_ANONYMOUS_GUEST,
	BP_HTTPD_USER_TYPE_LOCAL,
	BP_HTTPD_USER_TYPE_GLOBALDB

} BP_HTTPD_USER_TYPE;

// User hash algorithms supported (SHA256 should be used unless
// some strange esoteric reason demands you use another)
typedef enum
{

	BP_HTTPD_USER_PASSWORD_HASH_TYPE_UNSET,
	BP_HTTPD_USER_PASSWORD_HASH_TYPE_MD5,
	BP_HTTPD_USER_PASSWORD_HASH_TYPE_SHA1,
	BP_HTTPD_USER_PASSWORD_HASH_TYPE_SHA256

} BP_HTTPD_USER_PASSWORD_HASH_TYPE;

// user status
typedef enum
{

	BP_HTTPD_USER_STATUS_UNSET,
	BP_HTTPD_USER_STATUS_NORMAL,
	BP_HTTPD_USER_STATUS_ON_TIMEOUT,
	BP_HTTPD_USER_STATUS_BANNED

} BP_HTTPD_USER_STATUS;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Function Pointer Definitions %%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This function pointer is used for handling individual requests.  You need to
// implement functions of this type in your application, and then add them into
// your sessions hash registry by calling the BP_HTTPdAddRequestHandler function.
typedef BP_ERROR_T (*BP_HTTPdRequestHandlerFptr)
(
		/* P_BP_HTTPD_REQUEST */ void * request,
        struct MHD_Connection *         connection,
        const char *                    url,
        const char *                    method,
        const char *                    version,
        const char *                    upload_data,
        size_t     *                    upload_data_size,
        void       **                   con_cls
);

// This function pointer is used as a callback when a user attempts
// to access a protected resource without the correct credentials.
typedef int (*BP_HTTPdLoginFormCallback)
(
		/* P_BP_HTTPD_REQUEST */ void * request,
		struct MHD_Connection *         connection,
		char *                          reason_for_login_header,
		char *                          reason_for_login_body,
		char *                          reason_for_login_footer
);




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Main Structures %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// structure to house all the relevant structures within
// a httpd session.  Most structures defined above will (when
// instantiated) be placed into the httpd_hreg registry defined
// in the structure below.

typedef struct __BP_HTTPD_SESSION {



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Identifier Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this is the name selected by the user when a session is created
	char *identifier;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Running State Information %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// current running state
	BP_HTTPD_SERVER_STATE run_state;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% HTTPd Structures %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// daemon structure
	struct MHD_Daemon *daemon;

	// OR-ed combination of MHD Option values
	unsigned int start_flags;

	// port to bind to
	unsigned short port;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Cookie Vars %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// cookie key
	char * cookie_key;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Accept Policy %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// callback to call to check which clients will be allowed to connect (NULL means any ip is allowed)
	MHD_AcceptPolicyCallback apc;

	// extra argument to apc  (this will always point to a P_BP_HTTPD_SESSION self-reference)
	void *apc_cls;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Default Access Handler Callback %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// default handler for all uri's
	MHD_AccessHandlerCallback dh;

	// extra argument to default handler (this will always point to a P_BP_HTTPD_SESSION self-reference)
	void *dh_cls;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Configuration Information %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// the actual server key to use (not a file path)
	char * key_pem;

	// the cert pem to use
	char * cert_pem;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% MHD Options (VArgs) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// startup options for the server
	struct MHD_OptionItem runtime_options[BP_MHD_MAX_HTTPD_OPTIONS];



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Registries Specific to this HTTPd %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// hash table registry associated with this httpd session
	P_BP_HASH_TABLE_REGISTRY system_hreg;

	// hash table registry associated with holding request data.  Requests
	// are stored within their own hash table to prevent bottlenecking with
	// the main hreg.
	P_BP_HASH_TABLE_REGISTRY request_hreg;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Blockparty Event System Connector Reg %%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// hash table registry associated with handling events (defaults to GHT)
	P_BP_HASH_TABLE_REGISTRY event_hreg;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% HTTPd Promises Session %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// httpd promises are a way to asynchronously handle requests from
	// browsers.  Promises are provided via opaque linking here.
	/* P_BP_HTTPD_PROMISES_SESSION */ void * promises_session;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Extra Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// extra data passed in from the outside (typically used for passing application data in through requests)
	void  * extra_data;

	// length of the external data being passed in
	size_t  extra_data_len;



} BP_HTTPD_SESSION, *P_BP_HTTPD_SESSION;


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
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Default COOKIE Iterator %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int BP_HTTPdDefaultCookieIteratorCallback
(
		void *cls,
		MHD_ValueKind kind,
        const char *key ,
		const char* value
);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Default HTTP GET Iterator %%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int BP_HTTPdDefaultGetIteratorCallback
(
		void *cls,
		MHD_ValueKind kind,
        const char *key ,
		const char* value
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Default HTTP Post Iterator %%%%%%%%%%%%%%%%%%%%%%%%%%%
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
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Default Access Handler Callback %%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// this is used to declare access handler callbacks
#define BP_HTTPD_DECLARE_ACCESS_HANDLER_CALLBACK(callback_function_name) \
	BP_ERROR_T callback_function_name                                         \
	(                                                                         \
		void                          * request, /* P_BP_HTTPD_REQUEST */     \
		struct MHD_Connection *         connection,                           \
		const char *                    url,                                  \
		const char *                    method,                               \
		const char *                    version,                              \
		const char *                    upload_data,                          \
		size_t     *                    upload_data_size,                     \
		void       **                   con_cls                               \
	)


// Use this in the actual source file to declare your callback.
//
// IMPORTANT LIB-DEV NOTE: Do not change this an int return typedef.  This routine uses
// MHD_YES and MHD_NO as return types, so they are not compatible with BP_ERROR_T
#define BP_HTTPD_ACCESS_HANDLER_CALLBACK(callback_function_name) \
	int callback_function_name                                                \
	(                                                                         \
		void                          * request, /* P_BP_HTTPD_REQUEST */     \
		struct MHD_Connection *         connection,                           \
		const char *                    url,                                  \
		const char *                    method,                               \
		const char *                    version,                              \
		const char *                    upload_data,                          \
		size_t     *                    upload_data_size,                     \
		void       **                   con_cls                               \
	)


// utility macro for access handler parameters
#define BP_HTTPD_ACCESS_HANDLER_PARAMS \
		request,          \
		connection,       \
		url,              \
		method,           \
		version,          \
		upload_data,      \
		upload_data_size, \
		con_cls


// The following access handler callback will be used if none is provided.  It
// simply acts as a generic (but very capable) access handler that routes requests
// based on the /request_handlers/ found within the httpd hreg.

// Lib-Dev Note: Do not use macros to define this.  This needs to have an int return type
// because it returns MHD_YES or MHD_NO, which are incompatible with BP_ERROR_T.
int BP_HTTPdDefaultAccessHandlerCallback
(
	void *                 cls,
	struct MHD_Connection *connection,
	const char *           url,
	const char *           method,
	const char *           version,
	const char *           upload_data,
	size_t *               upload_data_size,
	void **                con_cls
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create and Destroy %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Create a httpd session (web server).  Doesn't actually
// start the server.  It just allocates all the required structures
// and sets up the session to be ready to run.
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
);

// attempt to destroy a httpd session
BP_ERROR_T BP_HTTPdDestroySession(P_BP_HTTPD_SESSION httpd);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% HTTPd Hash Registry Operations %%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



// ALL REGISTRIES: Creates all required hash registry layouts for the provided httpd.
// This function is called directly from BP_HTTPdCreateSession and should not be
// called directly without full understanding of what you're doing.  This function
// creates the basic hash table layout expected of a webserver.
BP_ERROR_T BP_HTTPdCreateInitialHashRegistryLayouts(P_BP_HTTPD_SESSION httpd);

// SYSTEM REGISTRY: This function is called directly from BP_HTTPdCreateSession and should not be
// called directly without full understanding of what you're doing.  This function
// creates the basic hash table layout expected of a webserver.
BP_ERROR_T BP_HTTPdCreateInitialSystemHashRegistryLayout(P_BP_HTTPD_SESSION httpd);


// REQUEST REGISTRY: This function is called directly from BP_HTTPdCreateSession and should not be
// called directly without full understanding of what you're doing.  This function
// creates the basic hash table layout expected of a webserver.
BP_ERROR_T BP_HTTPdCreateInitialRequestHashRegistryLayout(P_BP_HTTPD_SESSION httpd);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Start and Stop Httpd %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// start the httpd server
BP_ERROR_T BP_HTTPdStartServer(P_BP_HTTPD_SESSION httpd);

// this should be called directly before stopping the server (if
// you're being polite).  This lets existing clients finish up
// before stopping things outright.
BP_ERROR_T BP_HTTPdQuiesceServer(P_BP_HTTPD_SESSION httpd);

// stop the httpd server
BP_ERROR_T BP_HTTPdStopServer(P_BP_HTTPD_SESSION httpd);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Lookup Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempts to lookup a httpd session by name
P_BP_HTTPD_SESSION BP_HTTPdLookupByName(char * name_identifier);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Connection Rate Limits (structures) %%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



// Connection rate limiters are used to associate
typedef struct __BP_HTTPD_DATA_RATE_LIMITER {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Timers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// initial timetamp
	BP_SECONDS_SINCE_UNIX_EPOCH initial_timestamp;

	// the last sample timestamp (zero if there is no last sample)
	BP_SECONDS_SINCE_UNIX_EPOCH last_sample_timestamp;

	// current sample timestamp (set during timer initialization to the same value as the initial timestamp)
	BP_SECONDS_SINCE_UNIX_EPOCH current_sample_timestamp;

	// --- difference ---------------------------

	// calculated difference in seconds between current and last
	// timestamp, which is used for calculating transfer rates.
	size_t difference_in_seconds_between_current_and_last_timestamp;

	// Calculated difference in seconds between current and first
	// timestamp.
	size_t difference_in_seconds_between_initial_and_current_timestamp;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% User Data Rate Limits %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// --- user send rate ----------

	// maximum total send rate for user
	uint64_t max_total_send_transfer_rate_for_user;
	uint64_t max_total_send_transfer_rate_for_user_has_been_exceeded;

	// current send rate for user
	uint64_t curr_total_send_transfer_rate_for_user;

	// total number of bytes that we've sent to the user
	uint64_t total_send_bytes_for_user;


	// --- user recv rate ----------

	// maximum total recv rate for user
	uint64_t max_total_recv_transfer_rate_for_user;
	uint64_t max_total_recv_transfer_rate_for_user_has_been_exceeded;

	// current recieve rate for user
	uint64_t curr_total_recv_transfer_rate_for_user;

	// total recieve bytes for user
	uint64_t total_recv_bytes_for_user;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Request Handler Rate Limits %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// --- request handler activation limiters ---------

	// total number of request handler activations per second
	uint64_t  max_request_handler_activations_per_second;
	BP_BOOL   max_request_handler_activations_per_second_has_been_exceeded;

	// current request handler activations per second
	uint64_t  curr_request_handler_activations_per_second;

	// total number of request handler activations
	uint64_t total_request_handler_activations;


	// --- request handler rate limiters ------------

	// total number of response bytes per second
	uint64_t  max_request_handler_response_bytes_per_second;
	BP_BOOL   max_request_handler_response_bytes_per_second_has_been_exceeded;

	// current request handler response bytes per second
	uint64_t  curr_request_handler_response_bytes_per_second;

	// total number of response bytes sent
	uint64_t total_request_handler_response_bytes_sent;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Connection Frame Rate Limits %%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// --- recv frames ----------------------

	// maximum recieve frames per second
	uint64_t  max_websocket_recv_frames_per_second;
	BP_BOOL   max_websocket_recv_frames_per_second_has_been_exceeded;

	// current recieve frames per second
	uint64_t curr_websocket_recv_frames_per_second;

	// total numbr of frames recieved
	uint64_t total_websocket_recv_frames;

	// this is used to preserve recieve frames between intervals
	uint64_t curr_websocket_recv_frames_between_measurement;

	// todo: average recieved frames per second since initial timestamp
	uint64_t avg_websocket_recv_frames_per_second_since_initial_timestamp;


	// --- send frames ----------------------

	// maximum send frames per second
	uint64_t  max_websocket_send_frames_per_second;
	BP_BOOL   max_websocket_send_frames_per_second_has_been_exceeded;

	// current send frames per second
	uint64_t curr_websocket_send_frames_per_second;

	// total numbr of frames sent
	uint64_t total_websocket_send_frames;

	// this is used to preserve sent frames between intervals
	uint64_t curr_websocket_send_frames_between_measurement;

	// todo: average sent frames per second since initial timestamp
	uint64_t avg_websocket_send_frames_per_second_since_initial_timestamp;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Connection Data Rate Limits %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// --- recv bytes -----------------------

	// maximum recieve bytes per second
	uint64_t  max_websocket_recv_bytes_per_second;
	BP_BOOL   max_websocket_recv_bytes_per_second_has_been_exceeded;

	// Current recieve bytes per second as per the
	// last calculation.
	uint64_t  curr_websocket_recv_bytes_per_second;

	// total number of bytes recieved on this websocket
	uint64_t total_websocket_recv_bytes;

	// this is used to preserve recieve bytes between intervals
	uint64_t curr_websocket_recv_bytes_between_measurement;

	// todo: average recieved bytes per second since initial timestamp
	uint64_t avg_websocket_recv_bytes_per_second_since_initial_timestamp;


	// --- send bytes -----------------------

	// maximum send bytes per second
	uint64_t  max_websocket_send_bytes_per_second;
	BP_BOOL   max_websocket_send_bytes_per_second_has_been_exceeded;

	// Current send bytes per second as per the
	// last calculation.
	uint64_t  curr_websocket_send_bytes_per_second;

	// total number of bytes sent out of this websocket
	uint64_t total_websocket_send_bytes;

	// current websocket send bytes between measurement
	uint64_t curr_websocket_send_bytes_between_measurement;

	// todo: average sent bytes per second since initial timestamp
	uint64_t avg_websocket_send_bytes_per_second_since_initial_timestamp;


} BP_HTTPD_DATA_RATE_LIMITER, *P_BP_HTTPD_DATA_RATE_LIMITER;


// IMPORTANT NOTE: Rate Limit Calculation Routines are near the bottom of this file due to
//                 various structure definitions needing to be in place before use.



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Request Handlers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

typedef struct __BP_HTTPD_REQUEST_HANDLER_PARAMS {

	struct MHD_Connection *   connection;
	const char *              url;
	const char *              method;
	const char *              version;
	const char *              upload_data;
	size_t     *              upload_data_size;
	void       **             con_cls;

} BP_HTTPD_REQUEST_HANDLER_PARAMS, *P_BP_HTTPD_REQUEST_HANDLER_PARAMS;


// This structure holds information regarding request handlers.  These are stored
// as bins within the httpds hreg under the /request_handlers/ top level table.
typedef struct __BP_HTTPD_REQUEST_HANDLER {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Handler and Path %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// the activation path of the handler
	char * handler_path;

	// request handler
	BP_HTTPdRequestHandlerFptr handler;

	// this contains a set of binary flags corresponding to
	// BP_HTTPD_METHOD enumerated flags.
	BP_FLAGS_T supported_http_methods;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Custom Parameter %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// custom handler parameter (typically used as an "app" pointer pass through
	// so that the handler has instant global context upon init)
	void * external_handler_parameter;
	size_t external_handler_parameter_size_n;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Handler HEADER Limitations %%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this limits how many headers can be recieved before
	// headers are stopped.
	size_t http_header_max_count;

	// maximum length of a header key
	size_t http_header_max_key_len;

	// maximum length of a header value
	size_t http_header_max_val_len;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Data Rate Limits %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Set to BP_TRUE when data rate limits are enabled for users.
	BP_BOOL data_rate_limits_enabled;

	// Data rate limits for user (imposed globally for the user)
	BP_HTTPD_DATA_RATE_LIMITER data_rate_limits;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Handler COOKIE Limitations %%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// maximum key length
	size_t http_cookie_max_key_len;

	// maximum value length
	size_t http_cookie_max_val_len;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Handler GET Limitations %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// maximum key length
	size_t http_get_max_key_len;

	// maximum value length
	size_t http_get_max_val_len;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Handler POST Limitations %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// a handler will fail if this much data is uploaded
	size_t  posted_max_data_size_before_autofail;

	// this is the number of bytes to attempt to read at once
	// through the MHD post interface.
	size_t  posted_data_chunk_read_size;

	// this is the directory on the filesystem (literal path please) where
	// you want to hold uploaded tmp files.
	char * posted_data_tmp_directory_literal_path;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Behavior Modifiers %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Enabling these will cause the webserver to use files for
	// holding data while uploading multipart form data or octet stream
	// data.  This can DRASTICALY reduce the memory overhead of the
	// application if a lot of uploading is going on.  You can
	// set them via BP_HTTPdRequestHandlerAdd.

	// This will attempt to use a temporary file for file being sent
	// as multipart form data.
	BP_BOOL use_tmpfile_for_multipart_form_data;

	// This will attempt to use a temporary
	BP_BOOL use_tmpfile_for_octet_stream;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Authentiation Information %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if the request handler requires an authentication
	BP_BOOL auth_required;

	// Authentication handler of
	void * auth_handler;

	// the login form generator used for autogenerating
	// a login form.
	BP_HTTPdLoginFormCallback login_callback;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Handler Activation Information %%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// total number of activations (route was chosen)
	size_t activation_n;

	// number of times an activation results in error
	size_t error_n;

	// number of successful completions this handler is seeing
	size_t completed_n;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Request Handler Help Information %%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if the help has been populated ok, this is set
	BP_BOOL help_populated_ok;

	// help information registry
	P_BP_HASH_TABLE_REGISTRY help_registry;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Parameter Filters %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// registry parameter matrix
	P_BP_HASH_TABLE_REGISTRY parameter_filters;


} BP_HTTPD_REQUEST_HANDLER, *P_BP_HTTPD_REQUEST_HANDLER;





// Rqeust handler setup configurations.
typedef struct __BP_HTTPD_REQUEST_HANDLER_SETUP_CONFIGS {

	BP_FLAGS_T            supported_http_methods;
	size_t                posted_max_data_size_before_autofail;
	size_t                posted_data_chunk_read_size;
	char *                posted_data_tmp_directory_literal_path;
	BP_BOOL               use_tmpfile_for_multipart_form_data;
	BP_BOOL               use_tmpfile_for_octet_stream;
	size_t                http_get_max_key_len;
	size_t                http_get_max_val_len;
	size_t                http_cookie_max_key_len;
	size_t                http_cookie_max_val_len;
	size_t                http_header_max_count;
	size_t                http_header_max_key_len;
	size_t                http_header_max_val_len;

} BP_HTTPD_REQUEST_HANDLER_SETUP_CONFIGS, *P_BP_HTTPD_REQUEST_HANDLER_SETUP_CONFIGS;



// Request handlers are stored within the httpd session hash table and
// are used to process individual routes to various functions.  For example
// to map a handler to a request you could do something like:
//
//   BP_HTTPdAddRequestHandler(httpd, "/new/", your_handler_function_here);
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
);

// attempts to retrieve a pointer to a httpd request handler
P_BP_HTTPD_REQUEST_HANDLER BP_HTTPdRequestHandlerLookup
(
	P_BP_HTTPD_SESSION httpd,
	char *             handler
);






// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Request Handler Auto Addition Routines %%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Add gif handlers from a provided directory.
BP_ERROR_T BP_HTTPdAddGIFHandlersFromFilesystemDir
(
	P_BP_HTTPD_SESSION httpd,
	char * load_dir,
	BP_BOOL recurse,
	BP_BOOL require_valid_user = BP_TRUE
);

// Add png handlers from a provided dir
BP_ERROR_T BP_HTTPdAddPNGHandlersFromFilesystemDir
(
	P_BP_HTTPD_SESSION httpd,
	char * load_dir,
	BP_BOOL recurse,
	BP_BOOL require_valid_user = BP_TRUE
);

// This uses the blockparty filesystem registry to get a list of files which
// appear to be valid javascript, and loads them into memory buffers.  Once
// created, the buffers are used to serve dynamic javascript based on handler
// configuration.
BP_ERROR_T BP_HTTPdAddJavascriptHandlersFromFilesystemDir
(
	P_BP_HTTPD_SESSION httpd,
	char * load_dir,
	BP_BOOL recurse,
	BP_BOOL require_valid_user = BP_TRUE
);

// This attempts to load all css files from a directory
BP_ERROR_T BP_HTTPdAddCSSHandlersFromFilesystemDir
(
	P_BP_HTTPD_SESSION httpd,
	char * load_dir,
	BP_BOOL recurse,
	BP_BOOL require_valid_user = BP_TRUE
);

// This attempts to load all html types from filesystem.
BP_ERROR_T BP_HTTPdAddHtmlHandlersFromFilesystemDir
(
	P_BP_HTTPD_SESSION httpd,
	char * load_dir,
	BP_BOOL recurse,
	BP_BOOL require_valid_user = BP_TRUE
);

// This attempts to load all arbitrary text file types from a directory.
BP_ERROR_T BP_HTTPdAddTextHandlersFromFilesystemDir
(
	P_BP_HTTPD_SESSION httpd,
	char * load_dir,
	BP_BOOL recurse,
	char *  filename_pregexp,
	BP_BOOL require_valid_user = BP_TRUE
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Static Request Handlers Which Requrie Valid Users %%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

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
);


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
);

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
);

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
);

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
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Static Request Handlers (builtins) %%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

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
);


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
);

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
);

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
);

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
);





// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% HTTP Request Tracking %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




// Cascading request response template substitutions.
typedef struct __BP_HTTPD_REQUEST_RESPONSE_TEMPLATE_SUBSTITUTION {

	// the template string which will be replaced
	char * replace_this;

	// replace request with this
	char * replace_with;

} BP_HTTPD_REQUEST_RESPONSE_TEMPLATE_SUBSTITUTION, *P_BP_HTTPD_REQUEST_RESPONSE_TEMPLATE_SUBSTITUTION;



// HTTPD request structure.
typedef struct __BP_HTTPD_REQUEST {


	// A unique uuid associated with this request.  This is also the
	// name of the file which is created if a request being handled
	// has posted data.  The file is stored in the temporary directory
	// specified in the request handler.
	char * uuid;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% HTTPd Details %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// httpd servicing this request (circular reference)
	P_BP_HTTPD_SESSION httpd;

	// the request handler servicing this request (mapped from a gnutls_protocol_t enum within gnutls.h)
	P_BP_HTTPD_REQUEST_HANDLER request_handler;

	// request method
	BP_HTTPD_METHOD http_method;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Websocket Markers %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This marker is set to true if this is a websocket.  Websocket requests are
	// not autodestroyed on exit.  Websockets must clean up their own requests.
	BP_BOOL is_websocket_and_request_destruction_must_be_handled_externally;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Request Handler Parameters %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// These parameters are directly set via the request handler entry setup routine.  They should
	// never be freed or directly manipulated.  They are available for read purposes, and contain the
	// data associated with this request handler entry.
	BP_HTTPD_REQUEST_HANDLER_PARAMS params;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Extra Headers %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// extra headers which are to be sent with
	// the request response.  Added dynamically during the
	// server request handler execution.
	P_BP_HASH_TABLE_REGISTRY extra_headers;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% SSL/TLS Info %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if a request is a https request
	BP_BOOL secure_client_request;

	// the clients protocol version (mapped from
	BP_HTTPD_REQUEST_TLS_VERSION secure_client_proto_version;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Host Details %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// TODO: hostname of connecting host.  We have to be careful when populating
	// this as hostnames lookups can leak data.
	char * hostname;

	// ip address of host or hosts gateway.
	char * ip_address;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Argument Registry %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// The argument registry contains all arguments parsed
	// for the request (POST/GET/COOKIE).
	P_BP_HASH_TABLE_REGISTRY arg_hreg;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Post Information %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// MHD post processor for this request
	struct MHD_PostProcessor * pp;

	// This will be true if the post processor has been started.
	BP_BOOL post_processing_started;

	// this will be set to BP_TRUE if post processing
	// has completed ok.
	BP_BOOL post_processing_completed;

	// this is set the the post encoding of a specific type (1 encoding per request)
	BP_HTTPD_REQUEST_CONTENT_TYPE post_encoding;

	// indicator if the post data is a file
	BP_BOOL post_data_is_file;

	// the full file path for an uploaded post
	char *post_uploaded_full_file_path;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% JSON Output %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// marker indicating that this is a json handler
	BP_BOOL is_json_handler;

	// if this request is going to result in json output, this structure is initialized
	P_BP_JSON json_output;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Request Session %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if this user has a valid session, this will be set
	// to a casted P_BP_HTTPD_USER_SESSION ptr.
	void * session;

	// this is set if xsrf checks have been run in a request handler (e.g. via BP_HTTPD_RUN_XSRF_CHECKS)
	BP_BOOL xsrf_checks_passed_ok;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Request Response %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// The response structure used when a response is sent back.
	struct MHD_Response *response;


	// --- response template substitutions -----

	// template substitutions are performed dynamically before a
	// response is sent.
	P_BP_HTTPD_REQUEST_RESPONSE_TEMPLATE_SUBSTITUTION response_tmpl_substitutions;

	// number of substitutions currently in the buffer
	size_t response_tmpl_substitution_n;


	// --- response buffer (this is what actually gets sent) ----

	// current prepared response buffer
	char * response_buff;

	// current response buffer size
	size_t response_buff_size;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Validation Markers %%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if any parameters are parsed with issue, this is
	// marked to BP_FALSE.  Invalid requests can be disgarded
	// by removing them from the request registry using the
	// built in functions.
	BP_BOOL request_is_valid;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Iterative Internal Markers %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This is an opaque pointer to the last P_BP_HTTPD_REQUEST_VAR
	// that was updated.  This is primarily used by the POST processor
	// callback.  To determine when one var ends and another begins.  This
	// should not be used with any reliable intent unless you absolutely
	// know what you're doing.
	void * internal_last_request_var_processed;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Extra Headers %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// extra headers are set here from requests.  Format is {{"header", val }, NULL}
	char *** extra_headers_out;


} BP_HTTPD_REQUEST, *P_BP_HTTPD_REQUEST;


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
);


// These are stored in the argument registry according to type.  Data which was posted which ended
// up with posted tmpfiles will be found in the data_file parameter as an absolute path, and must be read in from file (using
// something like BP_FileGetContents())
typedef struct __BP_HTTPD_REQUEST_VAR {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Identifiers %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// the content-type of the request var
	BP_HTTPD_REQUEST_CONTENT_TYPE content_type;

	// the type of variable represented here
	BP_HTTPD_REQUEST_VAR_TYPE type;

	// variable name
	char * name;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Quick Indicators %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// These are set during creation/assignment and are simply
	// used as quick lookup indicators for easing development.

	// this request var is stored in a file
	BP_BOOL content_stored_in_file;

	// If this is true, the var is ready for use in a
	// request handler.
	BP_BOOL ready;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Data Representations %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// data (if not in a file)
	char * data;

	// if was read as a file, this is the path to the file
	char * data_file;

	// size of the filedata stored
	size_t data_size;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Internal Iterative Data %%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// file descriptor of secure temp file
	int    file_descriptor;
	size_t file_handle_data_written;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Filter Information %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if the filters have been run, this is set
	BP_BOOL filters_have_been_run;

	// if the filter passed checks ok, set this
	BP_BOOL filter_checks_passed_ok;

	// If the filter fails, this is set to a pointer to
	// a P_BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER pointer (opaquely).
	void *  failed_filter_check;

	// This is an optional filter message you can pass back when exiting.
	char * failed_filter_message;



} BP_HTTPD_REQUEST_VAR, *P_BP_HTTPD_REQUEST_VAR;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Request Handler Parameter Filters %%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This define declares all the potential parameters a filter callback can have.  Since ALL filters
// operate using the same set of parameters, this define lets us keep track of every filters parameter
// set, allowing us to change all filters easily if need-be.
#define BP_HTTP_REQUEST_HANDLER_FILTER_PARAMETERS\
     /* P_BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER */ void  * filter_as_void,     \
		P_BP_HTTPD_REQUEST_VAR                                 request_var,\
		char                                                 * key,        \
		void                                                 * data,       \
		size_t                                                 data_size

// This is used to declare parameter filters so that all filters are provided
// data/parameters uniformly.
#define BP_HTTP_REQUEST_HANDLER_DECLARE_PARAMETER_FILTER( filter )\
		BP_BOOL filter (BP_HTTP_REQUEST_HANDLER_FILTER_PARAMETERS)

// Simple macro to include an opener for filter declaration.  This is typically found
// inside a request handler itself near the top of the handler main entry point.
#define BP_HTTPD_REQUEST_HANDLER_DECLARE_FILTERS\
		if(!req->request_handler->parameter_filters)\

// Runs basic sanity check against the filter.
#define BP_HTTPD_REQUEST_HANDLER_FILTER_PROLOGUE\
		if(!filter_as_void)                                                                           \
		{                                                                                             \
			printf("\n[!!] Critical Error.  No filter pointer passed to filter function.  Exiting."); \
			return BP_FALSE;                                                                          \
		}                                                                                             \
		if(!request_var)                                                                              \
		{                                                                                             \
			printf("\n[!!] Critical Error.  No request var passed to filter function.  Exiting.");    \
			return BP_FALSE;                                                                          \
		}                                                                                             \
		/* set filter from void */                                                                    \
		P_BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER filter = (P_BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER) filter_as_void;\
		if(!key)                                                                                      \
			BP_HTTPD_REQUEST_HANDLER_FILTER_FAILED("no_key_provided");                                \
		if(!data)                                                                                     \
			BP_HTTPD_REQUEST_HANDLER_FILTER_FAILED("no_data_provided");                               \
		if(!data_size)                                                                                \
			BP_HTTPD_REQUEST_HANDLER_FILTER_FAILED("no_data_size_provided");                          \




// if a handler failed for any arbitrary reason, this can be called to exit immediately
#define BP_HTTPD_REQUEST_HANDLER_FILTER_FAILED( optional_msg )\
		{                                                                                        \
			request_var->filters_have_been_run   = BP_TRUE;                                      \
			request_var->filter_checks_passed_ok = BP_FALSE;                                     \
			request_var->failed_filter_check     = filter;                                       \
			request_var->failed_filter_message   = optional_msg ? bpstrdup(optional_msg) : NULL; \
			return BP_FALSE;                                                                     \
		}

// use this to return when all checks are finally passed and you are ready to return from your filter
#define BP_HTTPD_REQUEST_HANDLER_FILTER_PASSED_ALL_CHECKS\
		{                                                                                        \
			request_var->filters_have_been_run   = BP_TRUE;                                      \
			request_var->filter_checks_passed_ok = BP_TRUE;                                      \
			request_var->failed_filter_check     = filter;                                       \
			request_var->failed_filter_message   = NULL;                                         \
			return BP_TRUE;                                                                      \
		}




// Adds a macro to a request using BP_HTTPdRequestHandlerParameterFilterAdd.
#define BP_HTTPD_REQUEST_HANDLER_ADD_FILTER(key, var_type, filter, filter_config)\
		BP_HTTPdRequestHandlerParameterFilterAdd                \
		(                                                       \
			req->request_handler,                               \
			var_type,                                           \
			(char *) key,                                       \
			(char *) #filter,                                   \
			(BP_HTTPdRequestHandlerParameterFilterFptr) filter, \
			filter_config                                       \
		)



// --- Utility Macros for Easing RH Filter Use -------------------

#define BP_HTTPD_RH_ADD_POST_FILTER(key, filter, filter_config)\
		BP_HTTPD_REQUEST_HANDLER_ADD_FILTER(key, BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA, filter, filter_config)


// -- fptr declarations ----------------------------


// Parameter filter callback declaration.  Filter functions are used for parameter sanity.
typedef BP_BOOL (*BP_HTTPdRequestHandlerParameterFilterFptr)( BP_HTTP_REQUEST_HANDLER_FILTER_PARAMETERS );



// -- structure defintitions -----------------------

// Filter configuration.
typedef struct __BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER_CONFIG {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Basic Parameters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// min and max values for unsigned longs
	unsigned long unsigned_min;
	unsigned long unsigned_max;

	// min and max values for signed longs
	long          signed_min;
	long          signed_max;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% User Provided Extra Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// extra data passed through
	void *extra_data;


} BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER_CONFIG, *P_BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER_CONFIG;


// Parameter filter
typedef struct __BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER {

	// var key
	char * key;

	// filter routine
	BP_HTTPdRequestHandlerParameterFilterFptr filter;

	// var type
	BP_HTTPD_REQUEST_VAR_TYPE var_type;

	// request handler
	P_BP_HTTPD_REQUEST_HANDLER request_handler;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Filter Configuration %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this is set to true if
	BP_BOOL filter_config_has_been_set;

	// optional filter configuration
	BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER_CONFIG filter_config;


} BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER, *P_BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER;



// Add parameter filter.  Filters stack.  Attempting to add two filters
// to one request handler will result in cascading filtering.
BP_ERROR_T BP_HTTPdRequestHandlerParameterFilterAdd
(
		P_BP_HTTPD_REQUEST_HANDLER                         request_handler,
		BP_HTTPD_REQUEST_VAR_TYPE                          var_type,
		char                                              *key,
		char                                              *filter_name,
		BP_HTTPdRequestHandlerParameterFilterFptr          filter,
		P_BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER_CONFIG filter_config

);

// Attempt to lookup a table associated with holding "filter_sets".  Filter sets cascade
// directly into each other and are used for filtering out parameters.  This table contains
// a sub-table "filter_sets" which can be iterated through using BP_HASH_ITER macros in order
// to list them all.
P_BP_HASH_TABLE_KEYED_ENTRY  BP_HTTPdRequestHandlerParameterFilterSetLookup
(
	P_BP_HTTPD_REQUEST_HANDLER       request_handler,
	BP_HTTPD_REQUEST_VAR_TYPE        var_type,
	char                           * key
);

// Attempt to lookup a parameter filter structure given all lookup parameters.  This is mainly used for checking if
// filters exist before adding new filters, but it can also be used for any various kind of needed lookup.
P_BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER BP_HTTPdRequestHandlerFilterLookup
(
		P_BP_HTTPD_REQUEST_HANDLER                request_handler,
		BP_HTTPD_REQUEST_VAR_TYPE                 var_type,
		char                                    * key,
		char                                    * filter_name,
		BP_HTTPdRequestHandlerParameterFilterFptr filter
);

// Run a specific check against a var.  Requires an already looked up filter and variable.
BP_BOOL BP_HTTPdRequestHandlerFilterCheckVarAgainstFilter
(
		P_BP_HTTPD_REQUEST_VAR                      var,
		P_BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER filter
);

// Attempts to run filter checks on a request.  Request filters are stored in the request handler, filter check
// output is stored in the P_BP_HTTPD_REQUEST itself.  Logically, this function should be stored with the httpd
// request methods themselves, but I made a dev-call and put this function here instead, as it's heavily reliant
// on the code here (above).
BP_ERROR_T BP_HTTPdRequestRunFilterChecks(P_BP_HTTPD_REQUEST req);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Default Parameter Filter Routines %%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// add built-in alphanumeric filter
BP_HTTP_REQUEST_HANDLER_DECLARE_PARAMETER_FILTER(BP_HTTPdRequestHandlerFilterBuiltin_alphanum);

// add built-in signed long filter
BP_HTTP_REQUEST_HANDLER_DECLARE_PARAMETER_FILTER(BP_HTTPdRequestHandlerFilterBuiltin_signed_long);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Websocket Accept Key Calculator %%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This will return a variable length base64 key.  The "client_nonce_base64" value
// is the value you get from Sec-WebSocket-Key from the client request.  This process
// is specified in the websocket RFC on page 8 (as of right 2017), and documented in
// the stackoverflow answer: http://stackoverflow.com/questions/34778321/websocket-handshake-sec-websocket-accept-header-value-is-incorrect
char * BP_HTTPdWebsocketCalculateSecAcceptKey
(
	char *client_nonce_base64
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Request Create and Destroy %%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempts to create a new request structure using the httpds request registry. (called via BP_HTTPdRequestAdd)
P_BP_HTTPD_REQUEST BP_HTTPdRequestCreateNewEmpty(P_BP_HTTPD_SESSION httpd);

// Adds a request to the request registry.
P_BP_HTTPD_REQUEST BP_HTTPdRequestAdd
(
		P_BP_HTTPD_SESSION         httpd,
		P_BP_HTTPD_REQUEST_HANDLER request_handler,
		struct MHD_Connection *    connection,
        const char *               url,
        const char *               method,
        const char *               version
);

// Attempts to destroy a request (called after a request is dispatched automatically, shouldn't be called
// directly unless you know what you're doing).  Destroys all relevant data.
BP_ERROR_T BP_HTTPdRequestDestroy(P_BP_HTTPD_SESSION httpd, P_BP_HTTPD_REQUEST request);

// Internal function used to destroy the argument registry.  Called automatically via BP_HTTPdRequestDestroy()
BP_ERROR_T BP_HTTPdRequestDestroyArgRegistry(P_BP_HTTPD_REQUEST request);



// --- Individual Request Var Table Destructors ----

// Internal function to destroy post arguments.  Destroys data and closes related file descriptors.
BP_ERROR_T BP_HTTPdRequestDestroyPostArgs(P_BP_HTTPD_REQUEST request);

// Internal function to destroy HTTP GET arguments.  Destroys data and closes related file descriptors.
BP_ERROR_T BP_HTTPdRequestDestroyGetArgs(P_BP_HTTPD_REQUEST request);

// Internal function to destroy HTTP COOKIE arguments.  Destroys data and closes related file descriptors.
BP_ERROR_T BP_HTTPdRequestDestroyCookieArgs(P_BP_HTTPD_REQUEST request);




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Request Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to lookup a request by it's uuid
P_BP_HTTPD_REQUEST BP_HTTPdRequestLookupByUUID(P_BP_HTTPD_SESSION httpd, char * uuid);

// -- request specific layout routine --

// REQUEST REGISTRY: This function is called when a new request is added. It ensures
// that the request registry has request specific tables created.
BP_ERROR_T BP_HTTPdCreateInitialRequestArgumentHashRegistryLayout(P_BP_HTTPD_REQUEST request);

// -- encoding checks ----

// Attempt to translate encoding string to a restricted encoding set (all others
// fail immediately).
BP_HTTPD_REQUEST_CONTENT_TYPE BP_HTTPdRequestGetContentType(char * content_type);


// -- request argument registry addrs ----

// Adds a variable to the registry according to type.
BP_ERROR_T BP_HTTPdRequestVarAddToArgRegistry
(
		P_BP_HTTPD_REQUEST             request,
		BP_HTTPD_REQUEST_VAR_TYPE      type,
		BP_HTTPD_REQUEST_CONTENT_TYPE  encoding,
		char *                         name,
		char *                         data,
		char *                         data_file,
		size_t                         data_size
);

// Appends data to an existing request variable.  This is primarily used
// during the post processor.
BP_ERROR_T BP_HTTPdRequestVarAppendData
(
		P_BP_HTTPD_REQUEST             request,
		P_BP_HTTPD_REQUEST_VAR         var,
		char *                         data,
		size_t                         data_size
);

// Marks a variable as ready (flag within var request structure)
BP_ERROR_T BP_HTTPdRequestVarMarkReady(P_BP_HTTPD_REQUEST_VAR request);


// Attempts to lookup an argument from the argument registry
P_BP_HTTPD_REQUEST_VAR BP_HTTPdRequestVarArgRegistryLookup
(
		P_BP_HTTPD_REQUEST             request,
		BP_HTTPD_REQUEST_VAR_TYPE      type,
		char *                         name
);

// Attempts to lookup a request variable using case insensitive search. (requires
// linear search iterating through request table, slower than non case insensitive version)
P_BP_HTTPD_REQUEST_VAR BP_HTTPdRequestVarArgRegistryLookupCaseInsensitive
(
		P_BP_HTTPD_REQUEST             request,
		BP_HTTPD_REQUEST_VAR_TYPE      type,
		char *                         name
);


// Destroy a request var members and close file descriptors if necessary.
BP_ERROR_T BP_HTTPdRequestVarDestroyMembers(P_BP_HTTPD_REQUEST_VAR request_var);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Var Type Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Simply checks the argument tables and determines whether or not a request has arguments of the
// specified type.
BP_BOOL BP_HTTPdRequestContainsArgumentType
(
	P_BP_HTTPD_REQUEST             request,
	BP_HTTPD_REQUEST_VAR_TYPE      type
);

// Request contains POST arguments. (calls: BP_HTTPdRequestContainsArgumentType)
BP_BOOL BP_HTTPdRequestContainsPOST(P_BP_HTTPD_REQUEST request);

// Request contains GET arguments. (calls: BP_HTTPdRequestContainsArgumentType)
BP_BOOL BP_HTTPdRequestContainsGET(P_BP_HTTPD_REQUEST request);

// Request contains sent headers. (calls: BP_HTTPdRequestContainsArgumentType)
BP_BOOL BP_HTTPdRequestContainsHEADER(P_BP_HTTPD_REQUEST request);

// Request contains sent cookie values. (calls: BP_HTTPdRequestContainsArgumentType)
BP_BOOL BP_HTTPdRequestContainsCOOKIE(P_BP_HTTPD_REQUEST request);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Request Exit/Cleanup %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// THIS FUNCTIONALITY IS IMPLEMENTED VIA MACROS IN BP-HTTPdHelperMacros.h.


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Request Extra Headers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Adds an extra response header to the request
BP_ERROR_T BP_HTTPdAddExtraResponseHeaderToRequest
(
		P_BP_HTTPD_REQUEST request,
		char             * response_header,
		char             * response_value
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Request Handling Quality of Life Macros %%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// --- Default Enter and Exit Macros (simple checks) ----

// entrance macro
#define BP_HTTPD_ENTER_REQUEST_HANDLER \
	if(!request)\
	return ERR_FAILURE;

// exit macro
#define BP_HTTPD_EXIT_REQUEST_HANDLER




// --- HTTP Method Conveinence Macros for Request Handlers ----

// HTTP GET: convienence macro for request handlers specifically
#define BP_HTTPD_RH_METHOD_IS_GET \
		BP_HTTPD_METHOD_IS_GET(method)

// HTTP POST: convienence macro for request handlers specifically
#define BP_HTTPD_RH_METHOD_IS_POST \
		BP_HTTPD_METHOD_IS_POST(method)

// HTTP PUT: convienence macro for request handlers specifically
#define BP_HTTPD_RH_METHOD_IS_PUT \
		BP_HTTPD_METHOD_IS_PUT(method)

// HTTP CONNECT: convienence macro for request handlers specifically
#define BP_HTTPD_RH_METHOD_IS_CONNECT \
		BP_HTTPD_METHOD_IS_CONNECT(method)

// HTTP DELETE: convienence macro for request handlers specifically
#define BP_HTTPD_RH_METHOD_IS_DELETE \
		BP_HTTPD_METHOD_IS_DELETE(method)

// HTTP CONNECT: convienence macro for request handlers specifically
#define BP_HTTPD_RH_METHOD_IS_HEAD \
		BP_HTTPD_METHOD_IS_HEAD(method)

// HTTP OPTIONS: convienence macro for request handlers specifically
#define BP_HTTPD_RH_METHOD_IS_OPTIONS \
		BP_HTTPD_METHOD_IS_OPTIONS(method)

// HTTP TRACE: convienence macro for request handlers specifically
#define BP_HTTPD_RH_METHOD_IS_TRACE \
		BP_HTTPD_METHOD_IS_TRACE(method)




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Data Extraction %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Extracts relevant GET data into the hash table.  Relevant data is determined by examining the content
// of extract_names and matching them up with MHT internal retrieval routines.
BP_ERROR_T BP_HTTPdExtractGetDataFromRequest(P_BP_HTTPD_SESSION httpd, char **extract_names);

// Extracts relevant POST data into the hash table.  Relevant data is determined by examining the content
// of extract_names and matching them up with MHT internal retrieval routines.
BP_ERROR_T BP_HTTPdExtractPostDataFromRequest(P_BP_HTTPD_SESSION httpd, char **extract_names);

// Extracts relevant COOKIE data into the hash table.  Relevant data is determined by examining the content
// of extract_names and matching them up with MHT internal retrieval routines.
BP_ERROR_T BP_HTTPdExtractCookieDataFromRequest(P_BP_HTTPD_SESSION httpd, char **extract_names);

// Attempts to determine a httpd method using available conveinence macros.
BP_HTTPD_METHOD BP_HTTPdDetermineMethod(char *method);




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Add Users %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



// user structure
typedef struct __BP_HTTPD_USER {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% User Specifiers %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// access level associated with this user
	BP_HTTPD_USER_ACCESS access;

	// type of user
	BP_HTTPD_USER_TYPE type;

	// hash type of the user
	BP_HTTPD_USER_PASSWORD_HASH_TYPE hash_type;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% User Credentials %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// name of this user
	char * username;

	// password salt for calculating this users hash
	char * password_salt;

	// password for this user
	char * password_hash;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Cookie Information %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// cookie for this user
	char * cookie;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Misc Info %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// users email address
	char * email;

	// users biograpical information
	char * bio;

	// users role
	char * role;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Session %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// P_BP_HTTPD_USER_SESSION user session if one is set.
	void * session;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Data Rate Limits %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Set to BP_TRUE when data rate limits are enabled for users.
	BP_BOOL data_rate_limits_enabled;

	// Data rate limits for user (imposed globally for the user)
	BP_HTTPD_DATA_RATE_LIMITER data_rate_limits;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Externally Managed Extra Data %%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Extra data about this user, stored in JSON.  This is primarily
	// used by custom webserver managers and allows for custom properties
	// to be associated with users.
	P_BP_JSON externally_managed_extra_json_data;


} BP_HTTPD_USER, *P_BP_HTTPD_USER;


// Adds a user to the httpd database.
BP_ERROR_T BP_HTTPdUserAdd
(
		P_BP_HTTPD_SESSION               httpd,
		BP_HTTPD_USER_ACCESS             access,
		BP_HTTPD_USER_TYPE               type,
		BP_HTTPD_USER_PASSWORD_HASH_TYPE hash_type,
		char *                           username,
		char *                           password,
		char *                           password_salt,
		char *                           email,
		char *                           bio,
		char *                           role,
		BP_BOOL                          password_has_been_prehashed = BP_FALSE,
		P_BP_JSON                        extra_data = NULL
);

// Attempts to lookup a user within the table.
P_BP_HTTPD_USER BP_HTTPdUserLookup
(
		P_BP_HTTPD_SESSION httpd,
		char * username,
		char * password,
		char * email,
		char * bio,
		char * role
);


// Attempts to generate a salted hash buffer provided a given password.  Result
// must be free'd.
char * BP_HTTPdUserCreateSaltedHash
(
		BP_HTTPD_USER_PASSWORD_HASH_TYPE hash_type,
		char * salt,
		char * pass
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% JSON User Loading %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// load users from json buffer (buffer must be reasonable/printable/non-null)
BP_ERROR_T BP_HTTPdUserLoadFromJSONBuffer
(
	P_BP_HTTPD_SESSION httpd,
	char *json_buffer
);

// load users from a json file (securely loads file and then calls BP_HTTPdUserLoadFromJSONBuffer directly)
BP_ERROR_T BP_HTTPdUserLoadFromJSONFile
(
	P_BP_HTTPD_SESSION httpd,
	char *path_to_json_file
);

// attempts to create a JSON tree containing a valid httpd user record.  (used for modifying user registry)
P_BP_JSON BP_HTTPdCreateUserJSON
(
	BP_HTTPD_USER_ACCESS             access,
	BP_HTTPD_USER_TYPE               type,
	BP_HTTPD_USER_PASSWORD_HASH_TYPE hash_type,
	char *                           username,
	char *                           password,
	char *                           password_salt,
	char *                           email,
	char *                           bio,
	char *                           role
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Sessions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// api key privileges
typedef struct __BP_HTTPD_API_KEY_PRIVS {

	// placeholder till we have more privs
	BP_BOOL tmp;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Max Uses %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// arbitrary number of uses an API key can be used for before re-authentication is required (set to zero to disable)
	size_t max_uses_till_death;


} BP_HTTPD_API_KEY_PRIVS, *P_BP_HTTPD_API_KEY_PRIVS;


// define api key
typedef struct __BP_HTTPD_API_KEY {


	// key identifier
	char * key_identifier;

	// api key privileges
	BP_HTTPD_API_KEY_PRIVS privs;

	// user session (opaque and resolves to P_BP_HTTPD_USER_SESSION)
	void * user_session;


} BP_HTTPD_API_KEY, *P_BP_HTTPD_API_KEY;




// Cookie value.
typedef struct __BP_HTTPD_USER_SESSION {


	// randomly generated session identifier string
	char * identifier;

	// the key selected for sending identifier as cookie.  Cookie
	// will be: cookie_key=identifier
	char * cookie_key;

	// type of session
	BP_HTTPD_USER_SESSION_TYPE type;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% User Association %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// the user associated with this cookie
	P_BP_HTTPD_USER user;





	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% XSRF Token Registry %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this registry is used only for xsrf token lookups
	P_BP_HASH_TABLE_REGISTRY xsrf_reg;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Session Data Registry %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Explanation:  This registry is used to allow users to persistently
	//               storage data between requests.  This can be considered
	//               as a local data cache which can be used between requests.

	// the actual lookup registry itself
	P_BP_HASH_TABLE_REGISTRY session_data_hreg;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% API Keys %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// api key associated with this session
	P_BP_HTTPD_API_KEY api_key;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Session Timers %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// the time in seconds from unix epoch that the session was issued.
	size_t issued_time;

	// the number of seconds this session is allowed to live (this is
	// updated on request unless otherwise specified)
	size_t life_in_seconds;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Cookie Sent %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this flag is set when the server sends the set-cookie: header for this user session
	BP_BOOL cookie_sent;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Extra Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this is extra json data attached to a user which can contain arbitrary data as may
	// be required by a controlling server.  (allows for user data customization)
	P_BP_JSON extra_data;


} BP_HTTPD_USER_SESSION, *P_BP_HTTPD_USER_SESSION;


// Add a session for a user.
P_BP_HTTPD_USER_SESSION BP_HTTPdSessionAdd
(
		P_BP_HTTPD_SESSION           httpd,
		BP_HTTPD_USER_SESSION_TYPE   session_type,
		P_BP_HTTPD_USER              user,
		size_t                       session_lifetime_in_seconds,
		size_t                       xsrf_token_count
);

// Destroy a user session.
BP_ERROR_T BP_HTTPdSessionDestroy
(
		P_BP_HTTPD_SESSION httpd,
		P_BP_HTTPD_USER_SESSION user_session
);

// Retrieves a valid user if a valid session id is matched.
P_BP_HTTPD_USER_SESSION BP_HTTPdSessionAuthUserFromSessionId
(
		P_BP_HTTPD_SESSION httpd,
		char * session_id
);

// Creates a session by looking at the user database and selecting
// a user.
P_BP_HTTPD_USER_SESSION BP_HTTPdSessionAuthUserFromCreds
(
		P_BP_HTTPD_SESSION httpd,
		char * user,
		char * pass
);

// Creates an anonymous session which only grants access to xsrf
// token generation.  This provides xsrf tokens which can be utilized
// with a pre-authentication environment.  Typically this is used to
// prevent xsrf on login forms while still allowing login.
P_BP_HTTPD_USER_SESSION BP_HTTPdSessionCreateAnonymousGuest
(
		P_BP_HTTPD_SESSION httpd
);

// Attempts to lookup request handler within the table.
P_BP_HTTPD_USER_SESSION BP_HTTPdSessionLookup
(
		P_BP_HTTPD_SESSION httpd,
		char *             session_id
);

// Returns BP_TRUE if the session has expired, or BP_FALSE if it's
// still valid.
BP_BOOL BP_HTTPdSessionExpired
(
		P_BP_HTTPD_USER_SESSION user_session
);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Session Persistent (cache) Data %%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// persistent data
typedef struct __BP_HTTPD_USER_SESSION_PERSISTENT_DATA {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Lookup Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// uuid of persistent type
	char * key;

	// data type identifier information
	char * data_type;

	// description
	char * data_description;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// data pointer
	void * data;

	// length of data
	size_t data_len;


} BP_HTTPD_USER_SESSION_PERSISTENT_DATA, *P_BP_HTTPD_USER_SESSION_PERSISTENT_DATA;


// Attempts to add persistent data to a session
BP_ERROR_T BP_HTTPdSessionAddPersistentData
(
	P_BP_HTTPD_USER_SESSION user_session,
	char                   *key,
	void                   *data,
	size_t                  data_len,
	char                   *data_description = NULL,
	char                   *data_type        = NULL
);

// Attempts to lookup session persistent data by UUID
P_BP_HTTPD_USER_SESSION_PERSISTENT_DATA BP_HTTPdSessionLookupDataByKey
(
	P_BP_HTTPD_USER_SESSION user_session,
	char                   *key
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Add XSRF Tokens %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

typedef struct __BP_HTTPD_SESSION_XSRF_TOKEN {

	// the actual token used (shared with the hash entry
	char * token;

	// the time the xsrf token was created
	time_t created_ts;

	// the time the xsrf token is set to expire
	time_t expires_ts;

} BP_HTTPD_SESSION_XSRF_TOKEN, *P_BP_HTTPD_SESSION_XSRF_TOKEN;


// Adds an arbitrary number of valid xsrf tokens to a user session.
BP_ERROR_T BP_HTTPdSessionXSRFTokensAdd
(
	P_BP_HTTPD_USER_SESSION user_session,
	size_t                  token_count
);

// Adds one xsrf token to a user session.
BP_ERROR_T BP_HTTPdSessionXSRFTokenAdd
(
		P_BP_HTTPD_USER_SESSION user_session
);

// Checks to see if an xsrf token matches one generated
P_BP_HTTPD_SESSION_XSRF_TOKEN BP_HTTPdSessionXSRFTokenLookup
(
		P_BP_HTTPD_USER_SESSION user_session,
		char *xsrf_token
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% API Keys %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempts to add an api key to a user session within the hash registry
BP_ERROR_T BP_HTTPdSessionAddAPIKey
(
	P_BP_HTTPD_SESSION       httpd,
	P_BP_HTTPD_USER_SESSION  user_session,
	char *                   key_identifier,
	P_BP_HTTPD_API_KEY_PRIVS privs
);

// searches a httpd for an api key (hash table search, very fast)
P_BP_HTTPD_API_KEY BP_HTTPdSessionLookupAPIKeyByIdentifier
(
	P_BP_HTTPD_SESSION httpd,
	char * key_identifier
);

// removes an api key from the global api key registry
BP_ERROR_T BP_HTTPdDestroyAPIKey
(
);

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
);


// This routine is used for outputting a binary buffer directly to the page.  This is particularly
// useful for serving binary data such as images/tarballs/etc.
BP_ERROR_T BP_HTTPdRequestAppendBinaryBuffer
(
		P_BP_HTTPD_REQUEST                                request,
		char *                                            buffer,
		size_t                                            buffer_size
);


// Send the handlers configured login form callback (sends login banner).
BP_ERROR_T BP_HTTPdRequestResponseSendLoginForm
(
		P_BP_HTTPD_REQUEST      request,
		struct MHD_Connection  *connection,
		char *                  reason_for_login_header,
		char *                  reason_for_login_body,
		char *                  reason_for_login_footer
);


// Sends a prepared response over a specified connection
BP_ERROR_T BP_HTTPdRequestResponseSendPrepared
(
		P_BP_HTTPD_REQUEST     request,
		struct MHD_Connection *connection,
		BP_HTTPD_RESPONSE_CODE http_response_code
);




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Websockets Request Handlers %%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// websocket encryption type enumerator
typedef enum BP_HTTPD_WEBSOCKET_TYPE
{
	BP_HTTPD_WEBSOCKET_TYPE_UNSET,
	BP_HTTPD_WEBSOCKET_TYPE_PLAINTEXT,
	BP_HTTPD_WEBSOCKET_TYPE_TLS,
	BP_HTTPD_WEBSOCKET_TYPE_INVALID
};

// All the different opcodes in RFC6455, 4bits total
// so it's max 0-15.
typedef enum BP_HTTPD_WEBSOCKET_FRAME_OPCODE
{
	BP_HTTPD_WEBSOCKET_FRAME_OPCODE_CONTINUATION,
	BP_HTTPD_WEBSOCKET_FRAME_OPCODE_TEXT,
	BP_HTTPD_WEBSOCKET_FRAME_OPCODE_BINARY,
	BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_1_NON_CONTROL,
	BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_2_NON_CONTROL,
	BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_3_NON_CONTROL,
	BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_4_NON_CONTROL,
	BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_5_NON_CONTROL,
	BP_HTTPD_WEBSOCKET_FRAME_OPCODE_CONNECTION_CLOSE,
	BP_HTTPD_WEBSOCKET_FRAME_OPCODE_PING,
	BP_HTTPD_WEBSOCKET_FRAME_OPCODE_PONG,
	BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_1_CONTROL,
	BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_2_CONTROL,
	BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_3_CONTROL,
	BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_4_CONTROL,
	BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_5_CONTROL,
	BP_HTTPD_WEBSOCKET_FRAME_OPCODE_INVALID
};

// These values correspond to connection close frames and values.
typedef enum BP_HTTPD_WEBSOCKET_CLOSE_FRAME_STATUS_CODE
{
	BP_HTTPD_WEBSOCKET_CLOSE_FRAME_STATUS_CODE_UNSET = 0,
	BP_HTTPD_WEBSOCKET_CLOSE_FRAME_STATUS_CODE_NORMAL_CLOSURE = 1000,
	BP_HTTPD_WEBSOCKET_CLOSE_FRAME_STATUS_CODE_GOING_AWAY = 1001,
	BP_HTTPD_WEBSOCKET_CLOSE_FRAME_STATUS_CODE_PROTOCOL_ERROR = 1002,
	BP_HTTPD_WEBSOCKET_CLOSE_FRAME_STATUS_CODE_UNACCEPTED_DATA_TYPE = 1003,
	BP_HTTPD_WEBSOCKET_CLOSE_FRAME_STATUS_CODE_RESERVED_1 = 1004,
	BP_HTTPD_WEBSOCKET_CLOSE_FRAME_STATUS_CODE_RESERVED_2 = 1005,
	BP_HTTPD_WEBSOCKET_CLOSE_FRAME_STATUS_CODE_RESERVED_3 = 1006,
	BP_HTTPD_WEBSOCKET_CLOSE_FRAME_STATUS_CODE_INCONSISTENT_DATA_FOR_FRAME_TYPE = 1007,
	BP_HTTPD_WEBSOCKET_CLOSE_FRAME_STATUS_CODE_GENERIC_POLICY_VIOLATION = 1008,
	BP_HTTPD_WEBSOCKET_CLOSE_FRAME_STATUS_CODE_MESSAGE_TOO_BIG = 1009,
	BP_HTTPD_WEBSOCKET_CLOSE_FRAME_STATUS_CODE_CLIENT_TERMINATED_DUE_TO_EXTENSION_NEGOTIATION_FAILURE = 1010,
	BP_HTTPD_WEBSOCKET_CLOSE_FRAME_STATUS_CODE_UNEXPECTED_CONDITION = 1011,
	BP_HTTPD_WEBSOCKET_CLOSE_FRAME_STATUS_CODE_RESERVED_4 = 1015
};

// Decoded recieved frame header.
typedef struct __BP_HTTPD_WEBSOCKET_DECODED_RECV_FRAME_HEADER {

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Individual Flags %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this is set if we don't have fragmented packets
	BP_BOOL fin_flag;

	// reserved flags
	BP_BOOL rsv_1_flag;
	BP_BOOL rsv_2_flag;
	BP_BOOL rsv_3_flag;

	// operational code associated with this operation
	unsigned short opcode;

	// setif we have a masking key we need to utilize to xor decryhpt our payload
	BP_BOOL mask_flag;

	// -- if this is a close frame, this is the close code provided ---

	// this is set to the close frame code if we're closing a connection
	BP_HTTPD_WEBSOCKET_CLOSE_FRAME_STATUS_CODE close_frame_code;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Payload Lengths %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// payload length (always converted to 64bit regardless of size)
	uint64_t payload_len;

	// masking key (always a 32bit value)
	unsigned char masking_key[4];

	// This is the offset into the payload from the frame start
	// in which we "should" start reading data.
	unsigned short payload_offset_from_frame_start;

	// total length of the header (same as payload_offset_from_frame_start but here for double clarity)
	unsigned short header_len;

	// total number of payload bytes read
	uint64_t payload_bytes_read;

	// this contains the number of bytes remaining for a read operation
	uint64_t payload_bytes_remaining_for_read;

	// --- this contains the last read size from the frame data ----

	// used to track decode sizes during decodes
	uint64_t last_payload_decode_data_len;

	// last decoded byte index in the overall scope of decoding (this is used for xor decoding since it's based on position)
	size_t last_decode_byte_idx;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Identification Markers %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if reserved header flags are set, we have an invalid header decode
	BP_BOOL reserved_header_flags_are_set;

	// if this is a control frame this will be set to true
	BP_BOOL is_control_frame;

	// if this is a non control frame, this will be set to true (this is redundant due to the above is_control_frame
	// member, but I like having it here for mental clarity)
	BP_BOOL is_non_control_frame;

	// This marker is set if BP_HTTPdWebsocketDecodedFrameHeader completes successfully.
	BP_BOOL decoded_ok;

	// This is set if the opcode was determined to be valid (convience marker)
	BP_BOOL opcode_is_valid;

	// This is set if we've determined an invalid or unused opcode
	BP_BOOL opcode_is_invalid_or_reserved;

	// payload finished reading marker
	BP_BOOL payload_finished_reading;

	// if the data length is too short, this long member will be set
	BP_BOOL data_len_is_too_short_to_decode_header;

	// the number of bytes read before we determine the read is too short
	size_t data_len_is_too_short_read_count;

} BP_HTTPD_WEBSOCKET_DECODED_RECV_FRAME_HEADER, *P_BP_HTTPD_WEBSOCKET_DECODED_RECV_FRAME_HEADER;


// When sending data, frame headers need to be constructed.  This structure allows
// for the easy encoding of that data.
typedef struct __BP_HTTPD_WEBSOCKET_ENCODED_SEND_FRAME_HEADER {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Individual Flags %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this is set if we don't have fragmented packets
	BP_BOOL fin_flag;

	// reserved flags
	BP_BOOL rsv_1_flag;
	BP_BOOL rsv_2_flag;
	BP_BOOL rsv_3_flag;

	// operational code associated with this operation
	BP_HTTPD_WEBSOCKET_FRAME_OPCODE opcode;

	// setif we have a masking key we need to utilize to xor decryhpt our payload
	BP_BOOL mask_flag;

	// masking key (always a 32bit value)
	unsigned char masking_key[4];

	// payload length (always converted to 64bit regardless of size)
	uint64_t payload_len;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Encoder Flags and Preserved Positions %%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This is the last encoder byte index, used for performing encoder
	// modulo xor calculus.
	size_t last_payload_encode_byte_idx;

	// -- flags ---

	// marked as true if the header has been encoded ok
	BP_BOOL header_encoded_ok;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Actual Encoded Bytestring %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// note: encoded headers will never be longer than 40 bytes, so
	// we can just set the header length hear rather than
	// working with allocators.

	// encoded header bytes
	unsigned char encoded_header[40];

	// this is the full length of the encoded header
	size_t encoded_header_full_length;


} BP_HTTPD_WEBSOCKET_ENCODED_SEND_FRAME_HEADER, *P_BP_HTTPD_WEBSOCKET_ENCODED_SEND_FRAME_HEADER;


// connection exchange data type
typedef enum BP_HTTPD_WEBSOCKET_CONNECTION_EXCHANGE_DATA_TYPE
{
	BP_HTTPD_WEBSOCKET_CONNECTION_EXCHANGE_DATA_TYPE_UNSET,
	BP_HTTPD_WEBSOCKET_CONNECTION_EXCHANGE_DATA_TYPE_JSON,
	BP_HTTPD_WEBSOCKET_CONNECTION_EXCHANGE_DATA_TYPE_BASE64_SIZE_PREFIXED_BINARY,
	BP_HTTPD_WEBSOCKET_CONNECTION_EXCHANGE_DATA_TYPE_RAW_BINARY,
	BP_HTTPD_WEBSOCKET_CONNECTION_EXCHANGE_DATA_TYPE_INVALID
};


// Websocket data policy.
typedef struct __BP_HTTPD_WEBSOCKET_CONNECTION_POLICY {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Exchange Data Type %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This is the data type we're expecting to exchange with the end connection.
	BP_HTTPD_WEBSOCKET_CONNECTION_EXCHANGE_DATA_TYPE exchange_data_type;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Size Limits %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// maximum size of a frame before we disgard or disconnect
	uint64_t max_frame_len;
	BP_BOOL  max_frame_len_has_been_exceeded;

	// connection timeout in seconds
	size_t connection_timeout_in_seconds;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Policy Flags %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// --- Frame Is Too Long Policies -----------------

	// If this is set, we will disconnect if we encounter a frame
	// that is too long.
	BP_BOOL disconnect_on_too_long_frame;

	// If this is set, we will discard a frame if it's too long.
	BP_BOOL discard_frame_on_too_long_frame;


	// --- Connection Timeout Policies ----------------

	// disgard the connection on timeout
	BP_BOOL disconnect_on_connection_timeout;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Rate Limits for Send/Recv %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// socket rate limits
	BP_HTTPD_DATA_RATE_LIMITER socket_rate_limits;



} BP_HTTPD_WEBSOCKET_CONNECTION_POLICY, *P_BP_HTTPD_WEBSOCKET_CONNECTION_POLICY;



// Connection state.
typedef enum BP_HTTPD_WEBSOCKET_CONNECTION_STATE
{
	BP_HTTPD_WEBSOCKET_STATE_UNSET,
	BP_HTTPD_WEBSOCKET_STATE_PENDING_CONNECTION,
	BP_HTTPD_WEBSOCKET_STATE_CONNECTION_ESTABLISHED,
	BP_HTTPD_WEBSOCKET_STATE_PENDING_DISCONNECTION,
	BP_HTTPD_WEBSOCKET_STATE_DISCONNECTED,
	BP_HTTPD_WEBSOCKET_STATE_INVALID
};


// --- Websocket Callback Functions Typedefs ---------------------------


// This is the main callback which will contain our frame reader event loop, not to be confused
// with the read frame callback below which is used to read an individual frame.
typedef  void * (*P_BP_WEBSOCKET_FRAME_READER_CB)(void *);

// websocket read frame callback
typedef  void (*P_BP_WEBSOCKET_READ_FRAME_CB)( /*P_BP_HTTPD_WEBSOCKET_COMPLETE_RECV_FRAME*/ void *);

// websocket send frame callback
typedef  void * (*P_BP_WEBSOCKET_SEND_FRAME_CB)(/*P_BP_HTTPD_WEBSOCKET_COMPLETE_RECV_FRAME*/ void *);

// websocket event loop start callback
typedef  void * (*P_BP_WEBSOCKET_EVENT_LOOP_START_CB)(/*P_BP_HTTPD_WEBSOCKET*/ void *);

// websocket event loop finished callback
typedef  void * (*P_BP_WEBSOCKET_EVENT_LOOP_STOP_CB)(/*P_BP_HTTPD_WEBSOCKET_COMPLETE_RECV_FRAME*/ void *, /*P_BP_HTTPD_WEBSOCKET_FRAME_DECODER_STATE*/ void *);

// websocket event loop finished callback
typedef  void * (*P_BP_WEBSOCKET_CONNECTION_ERROR_CB)(/*P_BP_HTTPD_WEBSOCKET_COMPLETE_RECV_FRAME*/ void *, /*P_BP_HTTPD_WEBSOCKET_FRAME_DECODER_STATE*/ void *);



// websocket frame decoder state (this is purposfully done without registries to increase speed
// since this is used as a decoder frame)
typedef struct __BP_HTTPD_WEBSOCKET_FRAME_DECODER_STATE {


	// hash registry allocator
	P_BP_HASH_TABLE_REGISTRY hreg;

	// this is the socket file descriptor
	int socket_file_descriptor;

	// epoll file descriptor
	int epoll_file_descriptor;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% EPOLL loop configuration %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// current event
	struct epoll_event event;

	// event stack
	struct epoll_event *events;

	// Maximum number of EPOLL events to wait on.
	size_t max_events;

	// Total number of events which have been processed.
	uint64_t total_events_processed;

	// -- epoll indicator loops --------------

	// if an epoll fd was created ok
	BP_BOOL epoll_fd_created_ok;

	// control events are added ok
	BP_BOOL epoll_ctl_added_events_ok;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Counters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// total number of recieved frames
	uint64_t recv_frame_count;

	// recieve frame counts (indexed by frame opcode enum)
	uint64_t recv_frame_counts_by_type[BP_HTTPD_WEBSOCKET_FRAME_OPCODE_INVALID+1];

	// total number of bytes recieved
	uint64_t recv_bytes_total;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Decoder Configuration Settings %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Maximum read length.
	size_t max_read_len;

	// last time read() is called, this value is set
	size_t last_read_len;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Boundary Correction Aggregators %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// used to piece read() pieces into correct frames
	P_BP_LINKL read_aggregate_tq;

	// used to piece data together into correct data parts (different than frames)
	P_BP_LINKL data_aggregate_tq;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Frame Headers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Current recieve frame header
	BP_HTTPD_WEBSOCKET_DECODED_RECV_FRAME_HEADER curr_recv_frame_header;

	// This is the correct length to decode payloads based on the amount
	// of data remaining in a buffer.
	size_t frame_decode_payload_partial_len;


	// --- decoder markers -------------

	// if the last read filled the buffer, this is marked as true
	BP_BOOL last_read_filled_buffer;

	// mark read as expecting header
	BP_BOOL expecting_header;

	// header decoded flag
	BP_BOOL frame_header_decoded_ok;

	// data decoded flag
	BP_BOOL frame_data_decoded_ok;

	// this is set when a frame is fully decoded
	BP_BOOL frame_fully_decoded;


	// --- exit markers ----------------

	// this is set when it's time to end all loops and exit thread
	BP_BOOL finished;

	// this is set when the decoder requests termination due to error
	BP_BOOL finished_due_to_error;

	// string indicating the error
	char *finshed_err_msg;

} BP_HTTPD_WEBSOCKET_FRAME_DECODER_STATE, *P_BP_HTTPD_WEBSOCKET_FRAME_DECODER_STATE;



// local websocket structure
typedef struct __BP_HTTPD_WEBSOCKET {


	// hash reg allocator
	P_BP_HASH_TABLE_REGISTRY hreg;

	// request data passed through from our caller
	P_BP_HTTPD_REQUEST request;

	// httpd to which this websocket is a part
	P_BP_HTTPD_SESSION httpd;

	// the parent request handler which this websocket is tied to
	P_BP_HTTPD_REQUEST_HANDLER parent_request_handler;

	// pthread info
	pthread_t pt;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Upgrade Members %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Upgrade response handle for MHD.  Set before the websocket
	// thread is started.
	struct MHD_UpgradeResponseHandle * mhd_urh;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Basic Components %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// socket uuid (generated on websocket creation and used as
	// a lookup from the HTTPD)
	char uuid[BP_UUID_LEN+1];

	// file descriptor
	MHD_socket sock;

	// websocket type
	BP_HTTPD_WEBSOCKET_TYPE type;

	// connection state
	BP_HTTPD_WEBSOCKET_CONNECTION_STATE state;

	// websocket accept key
	char *websocket_accept_key;

	// connection policy (rate limits and other policy information)
	BP_HTTPD_WEBSOCKET_CONNECTION_POLICY connection_policy;

	// Name of the websocket application that this websocket is tied to.
	char *websocket_app_name;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Websocket Error State Flags %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this is marked if the upgrade callback has been triggered
	BP_BOOL upgrade_cb_has_triggered;

	// upgrade callback
	BP_BOOL upgrade_cb_has_exited_ok;


	// frame reader has triggered
	BP_BOOL frame_reader_cb_has_triggered;

	// called when the frame reader exits ok
	BP_BOOL frame_reader_cb_has_exited_ok;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Maximum Number of EPOLL events %%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Maximum number of EPOLL events to wait on.
	size_t max_epoll_events;

	// Maximum read size (also the size of a stack buffer used
	// for reads)
	size_t max_read_len;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% HTTP Upgrade Response Sent %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// the upgrade response which was sent (must be destroyed with MHD_destroy_response ())
	struct MHD_Response *upgrade_response_sent;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Non-Data Frame Type Counters %%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// total number of pings sent
	uint64_t pings_sent;

	// number of pings recieved
	uint64_t pings_recieved;

	// number of pongs sent
	uint64_t pongs_sent;

	// number of pongs recieved
	uint64_t pongs_recieved;

	// number of connection close frames recieved
	uint64_t connection_close_frames_recived;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% CallBacks %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// frame reader callback (main callback from which other callbacks are activated)
	P_BP_WEBSOCKET_FRAME_READER_CB frame_reader_cb;


	// --- loop event cbs --------------

	// read callback
	P_BP_WEBSOCKET_READ_FRAME_CB      read_frame_cb;
	size_t                            read_frame_cb_activation_count;

	// send callback (called whenever send frame method is called)
	P_BP_WEBSOCKET_SEND_FRAME_CB      send_frame_cb;
	size_t                            send_frame_cb_activation_count;

	// event loop start callback
	P_BP_WEBSOCKET_EVENT_LOOP_START_CB event_loop_start_cb;
	size_t                             event_loop_start_cb_activation_count;

	// event loop stop callback
	P_BP_WEBSOCKET_EVENT_LOOP_STOP_CB event_loop_stop_cb;
	size_t                            event_loop_stop_cb_activation_count;

	// connection error callback
	P_BP_WEBSOCKET_CONNECTION_ERROR_CB connection_err_cb;
	size_t                             connection_err_cb_activation_count;


} BP_HTTPD_WEBSOCKET, *P_BP_HTTPD_WEBSOCKET;


// Complete frame structure used for decoding.
typedef struct __BP_HTTPD_WEBSOCKET_COMPLETE_RECV_FRAME {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Parent Websocket Session %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// the websocket associated with this frame
	P_BP_HTTPD_WEBSOCKET websocket;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Decoded Header %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// decoded frame header
	P_BP_HTTPD_WEBSOCKET_FRAME_DECODER_STATE decoder_state;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// data
	unsigned char *data;

	// data length
	size_t data_len;


} BP_HTTPD_WEBSOCKET_COMPLETE_RECV_FRAME, *P_BP_HTTPD_WEBSOCKET_COMPLETE_RECV_FRAME;





// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Websocket Code Clarity Macros %%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// --- security oriented macros ----------

#define BP_HTTPD_WEBSOCKET_SECURITY_CHECK_LOCAL_STACK_CALLBACKS_INTERNAL\
	if(websocket->frame_reader_cb != frame_reader_cb_stack_check)\
	{\
		decoder_state.finished = BP_TRUE;\
		decoder_state.finished_due_to_error = BP_TRUE;\
		decoder_state.finshed_err_msg = "Websocket finished due to frame_reader_cb stack value mismatch.  This is a memory corruption error and should NEVER occur.";\
		break;\
	}\
	if(websocket->read_frame_cb != read_frame_cb_stack_check)\
	{\
		decoder_state.finished = BP_TRUE;\
		decoder_state.finished_due_to_error = BP_TRUE;\
		decoder_state.finshed_err_msg = "Websocket finished due to read_frame_cb stack value mismatch.  This is a memory corruption error and should NEVER occur.";\
		break;\
	}\
	if(websocket->send_frame_cb != send_frame_cb_stack_check)\
	{\
		decoder_state.finished = BP_TRUE;\
		decoder_state.finished_due_to_error = BP_TRUE;\
		decoder_state.finshed_err_msg = "Websocket finished due to send_frame_cb stack value mismatch.  This is a memory corruption error and should NEVER occur.";\
		break;\
	}\
	if(websocket->event_loop_start_cb != event_loop_start_cb_stack_check)\
	{\
		decoder_state.finished = BP_TRUE;\
		decoder_state.finished_due_to_error = BP_TRUE;\
		decoder_state.finshed_err_msg = "Websocket finished due to event_loop_start_cb stack value mismatch.  This is a memory corruption error and should NEVER occur.";\
		break;\
	}\
	if(websocket->event_loop_stop_cb != event_loop_stop_cb_stack_check)\
	{\
		decoder_state.finished = BP_TRUE;\
		decoder_state.finished_due_to_error = BP_TRUE;\
		decoder_state.finshed_err_msg = "Websocket finished due to event_loop_stop_cb stack value mismatch.  This is a memory corruption error and should NEVER occur.";\
		break;\
	}\
	if(websocket->connection_err_cb != connection_err_cb_stack_check)\
	{\
		decoder_state.finished = BP_TRUE;\
		decoder_state.finished_due_to_error = BP_TRUE;\
		decoder_state.finshed_err_msg = "Websocket finished due to connection_err_cb stack value mismatch.  This is a memory corruption error and should NEVER occur.";\
		break;\
	}



// --- websocket callback macros -------

// This macro is used to open a websocket frame reader callback
#define BP_HTTPD_WEBSOCKET_RUN_CALLBACK_OPEN_FRAME_READER_LOGIC_INTERNAL\
	printf("\n [+] websocket upgrade callback attempting to open frame reader");\
	if(!cls)                                                                 \
	{\
		printf("\n [!!] websocket upgrade callback reports null cls pointer");\
		return NULL;                                                         \
	}\
	P_BP_HTTPD_WEBSOCKET websocket = (P_BP_HTTPD_WEBSOCKET) cls;             \
	if(!websocket)                                                           \
	{\
		printf("\n [!!] websocket upgrade callback reports null websocket structure pointer");\
		return NULL;                                                         \
	}\
	BP_HTTPD_WEBSOCKET_FRAME_DECODER_STATE decoder_state;                    \
	BP_ZERO_STACK(decoder_state);                                            \
	decoder_state.epoll_file_descriptor = epoll_create1(0);                  \
	if(decoder_state.epoll_file_descriptor == -1)                            \
	{                                                                        \
		printf("\n [!!] websocket upgrade callback reports epoll_file_descriptor is -1 (erroneous value)");\
		decoder_state.epoll_fd_created_ok = BP_FALSE;                        \
		MHD_upgrade_action (websocket->mhd_urh,  MHD_UPGRADE_ACTION_CLOSE);  \
		return NULL;                                                         \
	}                                                                        \
	decoder_state.max_events = websocket->max_epoll_events;                  \
	BP_ZERO_STACK(decoder_state.event);                                      \
	decoder_state.event.data.fd = websocket->sock;                           \
	decoder_state.event.events = EPOLLIN | EPOLLET;                          \
	int s =  epoll_ctl                                                       \
	(                                                                        \
		decoder_state.epoll_file_descriptor,                                 \
		EPOLL_CTL_ADD,                                                       \
		websocket->sock,                                                     \
		&decoder_state.event                                                 \
	);                                                                       \
	if(s == -1)                                                              \
	{                                                                        \
		printf("\n [!!] websocket upgrade callback reports epoll_ctl operation has failed");\
		decoder_state.epoll_ctl_added_events_ok = BP_FALSE;                  \
		MHD_upgrade_action (websocket->mhd_urh,  MHD_UPGRADE_ACTION_CLOSE);  \
		return NULL;                                                         \
	}                                                                        \
	decoder_state.events = (struct epoll_event *) ht_calloc(websocket->hreg, decoder_state.max_events, sizeof(epoll_event)); \
	if(!decoder_state.events)                                                \
	{                                                                        \
		MHD_upgrade_action (websocket->mhd_urh,  MHD_UPGRADE_ACTION_CLOSE);  \
		printf("\n [!!] websocket upgrade callback reports decoder_state.events is empty");\
		return NULL;                                                         \
	}                                                                        \
	decoder_state.read_aggregate_tq = BP_CreateLinkL(".", BP_TRUE);          \
	decoder_state.data_aggregate_tq = BP_CreateLinkL(".", BP_TRUE);          \
	P_BP_LINKL tmp_read_aggregate_shift_tq = (P_BP_LINKL) NULL;              \
	unsigned char *read_aggregate_shift_buff = (unsigned char *) NULL;       \
	decoder_state.expecting_header = BP_TRUE;                                \
	int last_read_count = 0;                                                 \
	int events_pending_n = 0;                                                \
	int event_iter_idx   = 0;                                                \
	size_t frame_decode_payload_partial_len = 0;                             \
	char raw_stack_read_buf[websocket->max_read_len+16];                     \
	unsigned char *aggregate_buff_read_pos = NULL;                           \
	size_t aggregate_buff_read_idx = NULL;                                   \
	size_t remaining_bytes_till_end_of_last_read = 0;                        \
	uint16_t * close_code_ptr = NULL;                                        \
	P_BP_WEBSOCKET_FRAME_READER_CB frame_reader_cb_stack_check = websocket->frame_reader_cb;\
	P_BP_WEBSOCKET_READ_FRAME_CB read_frame_cb_stack_check = websocket->read_frame_cb;                   \
	P_BP_WEBSOCKET_SEND_FRAME_CB send_frame_cb_stack_check = websocket->send_frame_cb;                   \
	P_BP_WEBSOCKET_EVENT_LOOP_START_CB event_loop_start_cb_stack_check = websocket->event_loop_start_cb; \
	P_BP_WEBSOCKET_EVENT_LOOP_STOP_CB event_loop_stop_cb_stack_check = websocket->event_loop_stop_cb;    \
	P_BP_WEBSOCKET_CONNECTION_ERROR_CB connection_err_cb_stack_check = websocket->connection_err_cb;     \
	printf("\n [+] websocket upgrade callback open frame callback executed ok!");

// Close the frame reader logic internal.
#define BP_HTTPD_WEBSOCKET_RUN_CALLBACK_CLOSE_FRAME_READER_LOGIC_INTERNAL\
	if(decoder_state.finished)                                         \
	{                                                                  \
		websocket->frame_reader_cb_has_exited_ok = BP_TRUE;            \
	}                                                                  \
	MHD_upgrade_action (websocket->mhd_urh,  MHD_UPGRADE_ACTION_CLOSE);\
	return NULL;


#define BP_HTTPD_WEBSOCKET_RUN_CALLBACK_OPEN_FRAME_READER_LOOP\
	while(1) \
	{\
		wait_for_events_on_fd:\
		BP_dbg_printf("\n [!!] Attempting to wait for events on fd");\
		BP_dbg_printf("\n");\
		BP_HTTPD_WEBSOCKET_SECURITY_CHECK_LOCAL_STACK_CALLBACKS_INTERNAL;\
		BP_dbg_printf("\n [!!] stack checks passed ok");\
		BP_dbg_printf("\n");\
		if(decoder_state.finished)\
		{\
			BP_dbg_printf("\n [!!] frame reader loop is exiting due to decoder state reporting being finished");\
			BP_dbg_printf("\n");\
			break;\
		}\
		events_pending_n = 0;\
		event_iter_idx   = 0;\
		BP_dbg_printf("\n [!!] frame reader loop reports that we should wait for event loop");\
		BP_dbg_printf("\n");\
		events_pending_n = epoll_wait(decoder_state.epoll_file_descriptor, decoder_state.events, decoder_state.max_events, -1);\
		BP_dbg_printf("\n [!!] entering event loop.");\
		BP_dbg_printf("\n");\
		for\
		(\
			event_iter_idx = 0;\
			event_iter_idx < events_pending_n;\
			event_iter_idx++\
		)\
		{\
			if(decoder_state.finished)\
				break;\
			decoder_state.finished = BP_FALSE;\
			if\
			(\
				 (decoder_state.events[event_iter_idx].events & EPOLLERR) ||\
				 (decoder_state.events[event_iter_idx].events & EPOLLHUP) ||\
				!(decoder_state.events[event_iter_idx].events & EPOLLIN)\
			)\
			{\
				decoder_state.finished = BP_TRUE;\
				decoder_state.finished_due_to_error = BP_TRUE;\
				decoder_state.finshed_err_msg = "Websocket finished due to bad epoll event state detected.";\
				continue;\
			}\
			if( websocket->sock != decoder_state.events[event_iter_idx].data.fd)\
			{\
				BP_dbg_printf("\n fd is bad: %u", decoder_state.events[event_iter_idx].data.fd);\
				BP_dbg_printf("\n");\
				continue;\
			}\
			while(1)\
			{\
				read_directly_from_fd:\
				if(decoder_state.finished)\
					break;\
				last_read_count = 0;\
				decoder_state.last_read_filled_buffer = BP_FALSE;\
				BP_ZERO_STACK(raw_stack_read_buf);\
				BP_dbg_printf("\n Attempting read?");\
				BP_dbg_printf("\n");\
				last_read_count = read(decoder_state.events[event_iter_idx].data.fd, raw_stack_read_buf, sizeof(raw_stack_read_buf)-16);\
				BP_dbg_printf("\n Last read count?: %u", last_read_count);\
				BP_dbg_printf("\n");\
				if(last_read_count == -1)\
				{\
					if(errno != EAGAIN)\
					{\
						decoder_state.finished = BP_TRUE;\
						decoder_state.finished_due_to_error = BP_TRUE;\
						decoder_state.finshed_err_msg = "Websocket finished due to last read count being -1 and errno was not EAGAIN.  Likely remote closed connection or physical disconnection.";\
						break;\
					}\
					else\
					{\
						BP_dbg_printf("\n Socket error has occurred?");\
						usleep(2);\
						goto wait_for_events_on_fd;\
					}\
				}\
				if (last_read_count == 0)\
				{\
					BP_dbg_printf("\n Read count was zero?");\
					goto wait_for_events_on_fd;\
				}\
				BP_HTTPdRateLimitAdjustRatesForWebsocket\
				(\
					websocket,\
					0,\
					last_read_count,\
					0,\
					0\
				);\
				decoder_state.recv_bytes_total += last_read_count;\
				decoder_state.last_read_len = last_read_count;\
				if(decoder_state.last_read_len == decoder_state.max_read_len)\
					decoder_state.last_read_filled_buffer = BP_TRUE;\
				unsigned char *last_read_buff = (unsigned char *) tq_calloc(decoder_state.read_aggregate_tq, last_read_count, 1, BPLN);\
				if(!last_read_buff)\
				{\
					decoder_state.finished = BP_TRUE;\
					decoder_state.finished_due_to_error = BP_TRUE;\
					decoder_state.finshed_err_msg = "Websocket finished due to tq_calloc being unable to allocate a read buffer.  We are out of memory.";\
					break;\
				}\
				memcpy(last_read_buff, &raw_stack_read_buf, last_read_count);\
				BP_LinkLFlattenRaw(decoder_state.read_aggregate_tq);\
				if(!decoder_state.read_aggregate_tq->flat_buff)\
				{\
					decoder_state.finished = BP_TRUE;\
					break;\
				}\
				if(last_read_count < decoder_state.read_aggregate_tq->flat_buff_n)\
					last_read_count = decoder_state.read_aggregate_tq->flat_buff_n;\
				if(last_read_count == websocket->max_read_len)\
					decoder_state.last_read_filled_buffer = BP_TRUE;\
				remaining_bytes_till_end_of_last_read = last_read_count;\
				aggregate_buff_read_pos = &decoder_state.read_aggregate_tq->flat_buff[aggregate_buff_read_idx];\
				if(decoder_state.expecting_header == BP_TRUE)\
					goto decode_frame_header;\
				else\
				if(decoder_state.expecting_header == BP_FALSE)\
					goto decode_frame_data;\
				else\
				{\
					decoder_state.finished = BP_TRUE;\
					decoder_state.finished_due_to_error = BP_TRUE;\
					decoder_state.finshed_err_msg = "Websocket finished due to impossible header decoding state, likely corruption detected.";\
					break;\
				}\
				decode_frame_header:\
				printf("\n decode_frame_header_activated: %u", decoder_state.expecting_header);\
				printf("\n");\
				if(decoder_state.expecting_header)\
				{\
					BP_dbg_printf("\n decode frame header 1");\
					BP_dbg_printf("\n");\
					decoder_state.recv_frame_count++;\
					decoder_state.frame_header_decoded_ok = BP_FALSE;\
					if\
					(\
						BP_HTTPdWebsocketDecodeFrameHeader\
						(\
							(unsigned char *) aggregate_buff_read_pos,\
							remaining_bytes_till_end_of_last_read,\
							(P_BP_HTTPD_WEBSOCKET_DECODED_RECV_FRAME_HEADER) &decoder_state.curr_recv_frame_header\
						) == ERR_SUCCESS\
					)\
					{\
						printf("\n decode frame header 2");\
						printf("\n");\
						decoder_state.frame_header_decoded_ok = BP_TRUE;\
					}\
					BP_dbg_printf("\n decode frame header 3");\
					BP_dbg_printf("\n");\
					if(!decoder_state.frame_header_decoded_ok)\
					{\
						BP_dbg_printf("\n decode frame header 4");\
						BP_dbg_printf("\n");\
						if(decoder_state.curr_recv_frame_header.data_len_is_too_short_to_decode_header)\
						{\
							BP_dbg_printf("\n decode frame header 5");\
							BP_dbg_printf("\n");\
							decoder_state.recv_frame_count--;\
							tmp_read_aggregate_shift_tq = BP_CreateLinkL(".", BP_TRUE);\
							read_aggregate_shift_buff = (unsigned char *) tq_calloc(tmp_read_aggregate_shift_tq, remaining_bytes_till_end_of_last_read, 1, BPLN);\
							memcpy(read_aggregate_shift_buff, aggregate_buff_read_pos, remaining_bytes_till_end_of_last_read);\
							BP_DestroyLinkL(decoder_state.read_aggregate_tq, BP_TRUE);\
							decoder_state.read_aggregate_tq = tmp_read_aggregate_shift_tq;\
							aggregate_buff_read_idx = 0;\
							aggregate_buff_read_pos = NULL;\
							BP_ZERO_STACK(decoder_state.curr_recv_frame_header);\
							BP_dbg_printf("\n decode frame header 6");\
							BP_dbg_printf("\n");\
							goto read_directly_from_fd;\
						}\
						BP_dbg_printf("\n decode frame header 7");\
						BP_dbg_printf("\n");\
						decoder_state.finished = BP_TRUE;\
						decoder_state.finished_due_to_error = BP_TRUE;\
						decoder_state.finshed_err_msg = "Websocket finished due a websocket frame header being decoded incorrectly.";\
						break;\
					}\
					BP_dbg_printf("\n decode frame header 8");\
					BP_dbg_printf("\n");\
					decoder_state.expecting_header = BP_FALSE;\
					if(decoder_state.curr_recv_frame_header.header_len == last_read_count)\
					{\
						BP_dbg_printf("\n decode frame header 9 here!");\
						BP_dbg_printf("\n");\
						if(decoder_state.curr_recv_frame_header.opcode == BP_HTTPD_WEBSOCKET_FRAME_OPCODE_PING)\
						{\
							websocket->pings_recieved++;\
							BP_HTTPdWebsocketSendPong(websocket,0);\
						}\
						if(decoder_state.curr_recv_frame_header.opcode == BP_HTTPD_WEBSOCKET_FRAME_OPCODE_PONG)\
						{\
							websocket->pongs_recieved++;\
						}\
						if(decoder_state.curr_recv_frame_header.opcode == BP_HTTPD_WEBSOCKET_FRAME_OPCODE_CONNECTION_CLOSE)\
						{\
							websocket->connection_close_frames_recived++;\
							decoder_state.finished = BP_TRUE;\
						}\
						BP_HTTPdWebsocketDisplayDecodedFrameHeader((P_BP_HTTPD_WEBSOCKET_DECODED_RECV_FRAME_HEADER)&decoder_state.curr_recv_frame_header);\
						BP_DestroyLinkL(decoder_state.read_aggregate_tq, BP_TRUE);\
						decoder_state.read_aggregate_tq = BP_CreateLinkL(".", BP_TRUE);\
						aggregate_buff_read_idx         = 0;\
						aggregate_buff_read_pos         = NULL;\
						if(websocket->connection_close_frames_recived)\
						{\
							decoder_state.finished = BP_TRUE;\
						}\
						goto read_directly_from_fd;\
					}\
					BP_dbg_printf("\n decode frame header 10");\
					printf("\n");\
					if(decoder_state.curr_recv_frame_header.opcode == BP_HTTPD_WEBSOCKET_FRAME_OPCODE_PING)\
					{\
						websocket->pings_recieved++;\
						BP_HTTPdWebsocketSendPong(websocket,0);\
					}\
					if(decoder_state.curr_recv_frame_header.opcode == BP_HTTPD_WEBSOCKET_FRAME_OPCODE_PONG)\
					{\
						websocket->pongs_recieved++;\
					}\
					if(decoder_state.curr_recv_frame_header.opcode == BP_HTTPD_WEBSOCKET_FRAME_OPCODE_CONNECTION_CLOSE)\
					{\
						websocket->connection_close_frames_recived++;\
						decoder_state.finished = BP_TRUE;\
					}\
					BP_HTTPdWebsocketDisplayDecodedFrameHeader((P_BP_HTTPD_WEBSOCKET_DECODED_RECV_FRAME_HEADER)&decoder_state.curr_recv_frame_header);\
					if(last_read_count > decoder_state.curr_recv_frame_header.payload_offset_from_frame_start)\
					{\
						BP_dbg_printf("\n decode frame header 11");\
						BP_dbg_printf("\n");\
						aggregate_buff_read_idx               +=   decoder_state.curr_recv_frame_header.payload_offset_from_frame_start;\
						aggregate_buff_read_pos                =  &decoder_state.read_aggregate_tq->flat_buff[aggregate_buff_read_idx];\
						remaining_bytes_till_end_of_last_read -=   decoder_state.curr_recv_frame_header.payload_offset_from_frame_start;\
						decoder_state.expecting_header         =   BP_FALSE;\
						goto decode_frame_data;\
					}\
					BP_dbg_printf("\n decode frame header 12");\
					BP_dbg_printf("\n");\
				}\
				decode_frame_data:\
				if(!decoder_state.expecting_header)\
				{\
					if(decoder_state.curr_recv_frame_header.payload_bytes_remaining_for_read < remaining_bytes_till_end_of_last_read)\
					{\
						frame_decode_payload_partial_len       = decoder_state.curr_recv_frame_header.payload_len - decoder_state.curr_recv_frame_header.payload_bytes_read;\
						remaining_bytes_till_end_of_last_read -= decoder_state.curr_recv_frame_header.payload_bytes_remaining_for_read;\
					}\
					else\
					if(decoder_state.curr_recv_frame_header.payload_bytes_remaining_for_read >= remaining_bytes_till_end_of_last_read)\
					{\
						frame_decode_payload_partial_len      = remaining_bytes_till_end_of_last_read;\
						remaining_bytes_till_end_of_last_read = 0;\
					}\
					if(!frame_decode_payload_partial_len && !remaining_bytes_till_end_of_last_read)\
					{\
						aggregate_buff_read_idx = 0;\
						aggregate_buff_read_pos = NULL;\
						BP_DestroyLinkL(decoder_state.read_aggregate_tq, BP_TRUE);\
						decoder_state.read_aggregate_tq = BP_CreateLinkL(".", BP_TRUE);\
						goto read_directly_from_fd;\
					}\
					if(!frame_decode_payload_partial_len && remaining_bytes_till_end_of_last_read)\
					{\
						decoder_state.expecting_header = BP_TRUE;\
						aggregate_buff_read_idx +=  decoder_state.curr_recv_frame_header.last_payload_decode_data_len;\
						aggregate_buff_read_pos =  &decoder_state.read_aggregate_tq->flat_buff[aggregate_buff_read_idx];\
						goto decode_frame_header;\
					}\
					decoder_state.frame_data_decoded_ok = BP_FALSE;\
					if\
					(\
						BP_HTTPdWebsocketDecodeFrameData\
						(\
								&decoder_state.curr_recv_frame_header,\
								(unsigned char *) aggregate_buff_read_pos,\
								frame_decode_payload_partial_len,\
								decoder_state.data_aggregate_tq\
						) == ERR_SUCCESS\
					)\
					{\
						decoder_state.frame_data_decoded_ok = BP_TRUE;\
					}\
					if(!decoder_state.frame_data_decoded_ok)\
					{\
						decoder_state.finished = BP_TRUE;\
						decoder_state.finished_due_to_error = BP_TRUE;\
						decoder_state.finshed_err_msg = "Websocket finished due do a bad decoding of a websocket frame header.";\
						break;\
					}\
					if(decoder_state.curr_recv_frame_header.payload_finished_reading)\
					{\
						decoder_state.recv_frame_counts_by_type[decoder_state.curr_recv_frame_header.opcode]++;\
						BP_LinkLFlattenRaw(decoder_state.data_aggregate_tq);\


// close the frame reader loop
#define BP_HTTPD_WEBSOCKET_RUN_CALLBACK_CLOSE_FRAME_READER_LOOP\
						if(decoder_state.finished == BP_TRUE)\
						{\
							BP_dbg_printf("\n gets here close 1");\
							BP_dbg_printf("\n");\
							break;\
						}\
						BP_dbg_printf("\n gets here close 2");\
						BP_dbg_printf("\n");\
						BP_DestroyLinkL(decoder_state.data_aggregate_tq, BP_TRUE);\
						decoder_state.data_aggregate_tq = BP_CreateLinkL(".", BP_TRUE);\
						decoder_state.expecting_header = BP_TRUE;\
						BP_dbg_printf("\n gets here close 3");\
						BP_dbg_printf("\n");\
						if(remaining_bytes_till_end_of_last_read)\
						{\
							aggregate_buff_read_idx += decoder_state.curr_recv_frame_header.last_payload_decode_data_len;\
							aggregate_buff_read_pos =  &decoder_state.read_aggregate_tq->flat_buff[aggregate_buff_read_idx];\
							BP_dbg_printf("\n gets here close 4: %u - %u - %u", remaining_bytes_till_end_of_last_read, aggregate_buff_read_idx, decoder_state.read_aggregate_tq->flat_buff_n);\
							BP_dbg_printf("\n");\
							goto decode_frame_header;\
						}\
						BP_dbg_printf("\n gets here close 5");\
						BP_dbg_printf("\n");\
						aggregate_buff_read_idx = 0;\
						aggregate_buff_read_pos = NULL;\
						BP_DestroyLinkL(decoder_state.read_aggregate_tq, BP_TRUE);\
						decoder_state.read_aggregate_tq = BP_CreateLinkL(".", BP_TRUE);\
						BP_ZERO_STACK(decoder_state.curr_recv_frame_header);\
						BP_dbg_printf("\n gets here close 6");\
						BP_dbg_printf("\n");\
						goto read_directly_from_fd;\
					}\
					else\
					{\
						BP_dbg_printf("\n gets here close 7");\
						BP_dbg_printf("\n");\
						decoder_state.expecting_header = BP_FALSE;\
						if(remaining_bytes_till_end_of_last_read)\
						{\
							BP_dbg_printf("\n gets here close 8");\
							BP_dbg_printf("\n");\
							decoder_state.finished_due_to_error = BP_TRUE;\
							decoder_state.finished = BP_TRUE;\
							decoder_state.finshed_err_msg = "Websocket finished due to the detection of bytes remaining in a buffer which should have none.";\
							break;\
						}\
						BP_dbg_printf("\n gets here close 9");\
						BP_dbg_printf("\n");\
						aggregate_buff_read_idx = 0;\
						aggregate_buff_read_pos = NULL;\
						BP_DestroyLinkL(decoder_state.read_aggregate_tq, BP_TRUE);\
						decoder_state.read_aggregate_tq = BP_CreateLinkL(".", BP_TRUE);\
						BP_dbg_printf("\n gets here close 10");\
						BP_dbg_printf("\n");\
						goto read_directly_from_fd;\
					}\
				}\
				if(decoder_state.finished)\
				{\
					BP_dbg_printf("\n gets here close 11");\
					BP_dbg_printf("\n");\
					break;\
				}\
			}\
			if(decoder_state.finished)\
			{\
				BP_dbg_printf("\n gets here close 12");\
				BP_dbg_printf("\n");\
				break;\
			}\
		}\
		if(decoder_state.finished)\
		{\
			BP_dbg_printf("\n gets here 13");\
			BP_dbg_printf("\n");\
			break;\
		}\
	}\

// declare upgrade callback
#define BP_HTTPD_DECLARE_WEBSOCKET_UPGRADE_CALLBACK(callback_func_name)\
		static void callback_func_name\
		(\
			void                             *cls,\
			struct MHD_Connection            *connection,\
			void                             *con_cls,\
			const char                       *extra_in,\
			size_t                            extra_in_size,\
			MHD_socket                        sock,\
			struct MHD_UpgradeResponseHandle *urh\
		)

// declare websocket frame reader callback
#define BP_HTTPD_DECLARE_WEBSOCKET_FRAME_READER_CALLBACK(callback_func_name)\
		void *callback_func_name (void *cls)

// this is called at the start of the basic frame reader
#define BP_HTTPD_WEBSOCKET_BASIC_FRAME_READER_THREAD_STARTUP_PROLOGUE\
	P_BP_HTTPD_WEBSOCKET websocket = (P_BP_HTTPD_WEBSOCKET) cls;              \
	if(!websocket)                                                            \
	{                                                                         \
		websocket->upgrade_cb_has_triggered = BP_TRUE;                        \
		websocket->upgrade_cb_has_exited_ok = BP_FALSE;                       \
		return;                                                               \
	}                                                                         \
	if (extra_in_size)                                                        \
	{                                                                         \
		websocket->upgrade_cb_has_triggered = BP_TRUE;                        \
		websocket->upgrade_cb_has_exited_ok = BP_FALSE;                       \
		return;                                                               \
	}                                                                         \
	websocket->mhd_urh = urh;                                                 \
	websocket->upgrade_cb_has_triggered = BP_TRUE;                            \
	websocket->sock = sock;                                                   \
	if (0 != pthread_create (&websocket->pt, NULL, websocket->frame_reader_cb, (void *) websocket))     \
		return;                                                               \
	websocket->upgrade_cb_has_exited_ok = BP_TRUE;                            \


// called when a websocket frame reader is prepared for exit
#define BP_HTTPD_WEBSOCKET_RUN_CALLBACK_CLOSE_FRAME_READER_LOGIC_INTERNAL 1


// --- Developer Note: This is the main access handler callback macro and it creates 2 functions ---------------

#define BP_HTTPD_DECLARE_WEBSOCKET_ACCESS_HANDLER_CALLBACK(callback_func_name)\
		BP_HTTPD_DECLARE_WEBSOCKET_UPGRADE_CALLBACK(callback_func_name##_upgrade_cb)\
		{\
			BP_HTTPD_WEBSOCKET_BASIC_FRAME_READER_THREAD_STARTUP_PROLOGUE;\
			return;\
		}\
		BP_HTTPD_DECLARE_ACCESS_HANDLER_CALLBACK(callback_func_name)

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Websocket Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// --- frame decoding -----------

// This routine will decode a frame header based on raw frame data.  It takes data
// from
BP_ERROR_T BP_HTTPdWebsocketDecodeFrameHeader
(
		unsigned char *                           raw_frame_data,
		size_t                                    raw_frame_data_len,
		P_BP_HTTPD_WEBSOCKET_DECODED_RECV_FRAME_HEADER empty_header_in_out
);

// display a decoded frame header
BP_ERROR_T BP_HTTPdWebsocketDisplayDecodedFrameHeader
(
	P_BP_HTTPD_WEBSOCKET_DECODED_RECV_FRAME_HEADER decoded_frame_header
);

// Decode frame data using a frame header.
BP_ERROR_T BP_HTTPdWebsocketDecodeFrameData
(
		P_BP_HTTPD_WEBSOCKET_DECODED_RECV_FRAME_HEADER frame_header,
		unsigned char                            *data,
		size_t                                    data_len,
		P_BP_LINKL                                data_aggregate_tq
);


// --- frame encoding ----------

// This will encode a send frame header based around configured flags within the structure.  The
// structure itself holds the encoded data and a flag indicating whether encode was successful.  This
// INTENTIONALLY can create bad frame headers if you pass in bad data.  This is for the purpose of
// creating headers for fuzzers which may be doing protocol testing.
BP_ERROR_T BP_HTTPdWebsocketEncodeSendFrameHeader
(
	P_BP_HTTPD_WEBSOCKET_ENCODED_SEND_FRAME_HEADER frame_header_in_out
);

// Use masking key to encode send data.  Data will be encoded IN PLACE so ensure
// to supply data you are fine with being modified (pre-copy if necessary).  The
// masking key modulo position is stored within the P_BP_HTTPD_WEBSOCKET_ENCODED_SEND_FRAME_HEADER
// structure.
BP_ERROR_T BP_HTTPdWebsocketEncodeSendData
(
	P_BP_HTTPD_WEBSOCKET_ENCODED_SEND_FRAME_HEADER frame_header_in_out,
	unsigned char                                 *data_to_encode_in_out,
	size_t                                         data_to_encode_size
);


// --- Add Websocket Handler For Processing Upgrade Requests --------------

// This will add a websocket type request handler that will allow us handle websockets as they come in.
BP_ERROR_T BP_HTTPdRequestHandlerAddWebsocket
(
		P_BP_HTTPD_SESSION                       httpd,
		char *                                   handler_path,
		BP_HTTPdRequestHandlerFptr               handler,
		P_BP_HTTPD_REQUEST_HANDLER_SETUP_CONFIGS setup_configs,
		void *                                   external_handler_copy_parameter,
		size_t                                   external_handler_copy_parameter_size_n,
		BP_HTTPdLoginFormCallback                login_form_cb
);


// --- Create Plaintext Sockets --------------

// create websocket
P_BP_HTTPD_WEBSOCKET BP_HTTPdCreateWebsocket
(
	P_BP_HTTPD_SESSION                       httpd,
	char                                    *websocket_app_name,
	BP_HTTPD_WEBSOCKET_TYPE                  socket_type,
	P_BP_HTTPD_REQUEST                       request,
	char                                    *calculated_websocket_accept_key,
	char                                    *url,
	P_BP_HTTPD_WEBSOCKET_CONNECTION_POLICY   initial_connection_policy,
	size_t                                   max_read_len,
	size_t                                   max_epoll_events,
	P_BP_WEBSOCKET_FRAME_READER_CB           frame_reader_cb,
	P_BP_WEBSOCKET_READ_FRAME_CB             read_frame_cb,
	P_BP_WEBSOCKET_SEND_FRAME_CB             send_frame_cb,
	P_BP_WEBSOCKET_EVENT_LOOP_START_CB       event_loop_start_cb,
	P_BP_WEBSOCKET_EVENT_LOOP_STOP_CB        event_loop_stop_cb,
	P_BP_WEBSOCKET_CONNECTION_ERROR_CB       connection_err_cb
);


// --- Crafting Frame Headers ----------------


// --- Recv Data on a Websocket --------------

// Note: Recieving data on a websocket is done
//       from within a macro'ized loop.  There is
//       no BP_HTTPdWebsocketRecv function as of
//       right now.  That's because the macro loop provides
//       you with a decoded data buffer that becomes
//       available for you as soon as it's ready.
//       This is done due to epoll/nonblocking.


// --- Send Data on Websocket ----------------

// Send data on a websocket using a raw preconfigured header.  This
// requires you to preconfigure a P_BP_HTTPD_WEBSOCKET_ENCODED_SEND_FRAME_HEADER
// and pass it in as a parameter.  This routine is used for raw sends and
// situational protocol fuzzing (for security testing).
BP_ERROR_T BP_HTTPdWebsocketSendRaw
(
	P_BP_HTTPD_WEBSOCKET                           websocket,
	P_BP_HTTPD_WEBSOCKET_ENCODED_SEND_FRAME_HEADER raw_frame_header,
	unsigned char                                 *send_data,
	size_t                                         send_data_size,
	size_t                                         send_chunk_max_size,
	int                                            additional_tcp_flags
);

//
// Send text data over a websocket.  Text data will be
// marked with a text opocode set within the frame/header when sent.
//
// IMPORTANT: Per the RFC, frames which are sent to the client should
// not be masked.  Only client->server frames should be masked.  For
// that reason, please set the perform_masking_before_send parameter
// correctly depending on your position in the server/client relationship.
//
BP_ERROR_T BP_HTTPdWebsocketSend
(
	P_BP_HTTPD_WEBSOCKET            websocket,
	BP_HTTPD_WEBSOCKET_FRAME_OPCODE opcode,
	char                           *send_data,
	size_t                          send_data_size,
	size_t                          send_chunk_max_size,
	int                             additional_tcp_flags,
	BP_BOOL                         perform_masking_before_send
);

// This attempts to send a file over a websocket
BP_ERROR_T BP_HTTPdWebsocketSendFromFile
(
	P_BP_HTTPD_WEBSOCKET            websocket,
	BP_HTTPD_WEBSOCKET_FRAME_OPCODE opcode,
	char                           *send_file_path,
	size_t                          file_read_and_send_chunk_max_size,
	int                             additional_tcp_flags,
	BP_BOOL                         perform_masking_before_send
);


// --- Send Special Data on Websocket -------

// Send websocket protocol connection close frame.
BP_ERROR_T BP_HTTPdWebsocketSendConnectionClose
(
		P_BP_HTTPD_WEBSOCKET websocket,
		int                  additional_tcp_flags
);

// Send websocket protocol ping.
BP_ERROR_T BP_HTTPdWebsocketSendPing
(
		P_BP_HTTPD_WEBSOCKET websocket,
		int                  additional_tcp_flags
);

// Send websocket protocol pong.
BP_ERROR_T BP_HTTPdWebsocketSendPong
(
		P_BP_HTTPD_WEBSOCKET websocket,
		int                  additional_tcp_flags
);



// --- Close Websockets ----------------------

// close a websocket connection
BP_ERROR_T BP_HTTPdWebsocketClose
(
		P_BP_HTTPD_SESSION httpd
);





// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Connection Rate Limits (routines) %%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// --- init/update timmers -----------------------

// initialize timers
BP_ERROR_T BP_HTTPdRateLimitInitTimers(P_BP_HTTPD_DATA_RATE_LIMITER rate_limits);

// update timers
BP_ERROR_T BP_HTTPdRateLimitUpdateTimers(P_BP_HTTPD_DATA_RATE_LIMITER rate_limits);


// --- rate adjustments --------------------------

// Adjust rate for user based on new add data.
BP_ERROR_T BP_HTTPdRatesLimitAdjustRateForUser
(
	P_BP_HTTPD_USER user,
	size_t send_rate_add,
	size_t recv_rate_add
);

// Adjust rate for request handler based on new add data.
BP_ERROR_T BP_HTTPdRateLimitAdjustRatesForRequestHandler
(
	P_BP_HTTPD_REQUEST_HANDLER request_handler,
	size_t send_rate_add,
	size_t recv_rate_add
);

// Adjust rate for websocket based on new add data.
BP_ERROR_T BP_HTTPdRateLimitAdjustRatesForWebsocket
(
	P_BP_HTTPD_WEBSOCKET websocket,
	size_t               send_rate_bytes_add,
	size_t               recv_rate_bytes_add,
	size_t               send_rate_frames_add,
	size_t               recv_rate_frames_add
);

// --- check rate limits -------------------

// Simple check to determine if any of the rates within the rate limit structure, has been
// exceeded.
BP_BOOL BP_HTTPdRateLimitCheckAnyRateExceedsLimit(P_BP_HTTPD_DATA_RATE_LIMITER rate_limit);

// One or more send rates has been exceeded.
BP_BOOL BP_HTTPdRateLimitCheckSendRateExceedsLimit(P_BP_HTTPD_DATA_RATE_LIMITER rate_limit);

// One or more recv rates has been exceeded.
BP_BOOL BP_HTTPdRateLimitCheckRecvExceedsLimit(P_BP_HTTPD_DATA_RATE_LIMITER rate_limit);


// --- reset rate limits ------------------

// Reset the rate limits.
BP_ERROR_T BP_HTTPdRateLimitResetLimits(P_BP_HTTPD_DATA_RATE_LIMITER rate_limits);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Display an entire httpd session (with optional constraints)
BP_ERROR_T BP_HTTPdDisplay
(
		P_BP_HTTPD_SESSION httpd,
		BP_BOOL            display_system_registry,
		BP_BOOL            display_request_registry,
		BP_BOOL            display_request_details
);

// --- System Registry Display Routines ----------

// Display users found within the system registry
BP_ERROR_T BP_HTTPdDisplayUsers(P_BP_HTTPD_SESSION httpd);

// Display Requests in the system registry
BP_ERROR_T BP_HTTPdDisplayRequests(P_BP_HTTPD_SESSION httpd);

// Display request handlers within the system registry
BP_ERROR_T BP_HTTPdDisplayRequestHandlers(P_BP_HTTPD_SESSION httpd);

// Display sessions within the system registry
BP_ERROR_T BP_HTTPdDisplaySessions(P_BP_HTTPD_SESSION httpd);

// Display log within the system registry
BP_ERROR_T BP_HTTPdDisplayLog(P_BP_HTTPD_SESSION httpd);


// --- Specific Datatype Display Routines ---

// display a user
BP_ERROR_T BP_HTTPdDisplayUser(P_BP_HTTPD_USER user, BP_BOOL redact);

// display a request handler
BP_ERROR_T BP_HTTPdDisplayRequestHandler(P_BP_HTTPD_REQUEST_HANDLER request_handler);

// display request
BP_ERROR_T BP_HTTPdDisplayRequest(P_BP_HTTPD_REQUEST request);

// display a user session structure
BP_ERROR_T BP_HTTPdDisplayUserSession(P_BP_HTTPD_USER_SESSION user_session);



// --- Request Argument Registry Display Routines ----------

// display request argument registry HEADER
BP_ERROR_T BP_HTTPdDisplayRequestHeaderArgs(P_BP_HTTPD_REQUEST request);

// display request argument registry POST
BP_ERROR_T BP_HTTPdDisplayRequestPostArgs(P_BP_HTTPD_REQUEST request);

// display request argument registry GET
BP_ERROR_T BP_HTTPdDisplayRequestGetArgs(P_BP_HTTPD_REQUEST request);

// display request argument registry COOKIE
BP_ERROR_T BP_HTTPdDisplayRequestCookieArgs(P_BP_HTTPD_REQUEST request);

// --- Display Request Argument -----------

// Display a single request variable
BP_ERROR_T BP_HTTPdDisplayRequestVar(P_BP_HTTPD_REQUEST_VAR request_var);


#endif /* INCLUDE_NETWORK_HTTPD_BP_HTTPD_H_ */

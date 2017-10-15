/***************************************************************************
 *            BP-NetworkWeb.h
 *
 *  Tue Nov 14 22:20:44 2006
 *  Copyright  2006  User
 *  Email
 */

// ------------------------------------------------------------------------------
// HTTP SESSION STRUCTURES AND FUNCTIONALITY

// This structure is used to hold a whitelist element
// containing an expression, and a weight.  The weight is used
// for the purpose of diong best-guess load balancing.

typedef struct __BP_DB_WEB_WEIGHTED_WHITELIST_ELEMENT {

	// whitelist structure (member allocation to minimize
	// cache hits.)
	char whitelist[1024];

	// weight
	size_t weight;

} BP_DB_WEB_WEIGHTED_WHITELIST_ELEMENT, *P_BP_DB_WEB_WEIGHTED_WHITELIST_ELEMENT;



// web request balancer
typedef struct __BP_DB_WEB_REQUEST_BALANCER {

	// elements (white list entries)
	P_BP_DB_WEB_WEIGHTED_WHITELIST_ELEMENT entries;

	// total entries in balancer
	size_t entry_n;

	// flag indicating a update is required
	size_t requires_update;

	// generic balancer (from utilities) used for quick
	// balancing between requests.
	P_BP_WEIGHTED_BALANCER balancer;

	// garbage collector used by this balancer
	P_BP_GARBAGE_COLLECTOR_DATA gc;

} BP_DB_WEB_REQUEST_BALANCER, *P_BP_DB_WEB_REQUEST_BALANCER;


// This structure holds an entry retrieved from the web request db.  Used in
// the block class primarily and has no implementation of set/get
// in C, but is defined here due to its nature. Data is defined
// within the structure as opposed to being dynamically allocated
// to reduce cache misses.

typedef struct __BP_WEB_RESULT_ENTRY {

	// id in the db
	size_t result_id;

	// assessment id
	size_t assessment_id;

	// type
	size_t type;

	// sha1
	char sha1[41];

	// href
	char href[1025];

	// host
	size_t host;

	// base
	size_t base;

	// path
	size_t path;

	// script
	size_t script;

	// status
	size_t status;

	// status of update resolver backend (allows locks to provide non-contention)
	size_t update_resolution_status;

	// form or anchor code
	size_t origin_type_form_or_anchor_code;

	// commit_timestamp
	char commit_timestamp[129];

	// audit start timestamp
	char audit_start_timestamp[129];

	// audit completed timestamp
	char audit_completed_timestamp[129];

	// spidered status
	size_t spidered;

	// spidered parent id
	size_t spidered_parent_result_id;

	// spidered timestamp
	char spidered_timestamp[129];

	// total hits
	size_t hits;

	// disabled flag
	size_t disabled;

	// disabled status code
	size_t disable_code;

	// Individual disable settings for path/base/script.
	// path
	size_t disabled_path;
	size_t disabled_path_code;

	// TODO: host
	size_t disabled_host;
	size_t disabled_host_code;

	// TODO: base
	size_t disabled_base;
	size_t disabled_base_code;

	// TODO: script
	size_t disabled_script;
	size_t disabled_script_code;

	// TODO: administrative disable override
	size_t administrative_disable_override;
	size_t administrative_disable_override_code;

	// flag indicating whether or not the record
	// has been already updated via either
	// Block::
	//    setLastResultSpideredStatus(size_t status_code);
	// or
	//    setLastResultStatus(size_t status_code);
	// zero for false, any other value for true
	size_t record_already_updated_spidered;
	size_t record_already_updated_audit;


} BP_WEB_RESULT_ENTRY, *P_BP_WEB_RESULT_ENTRY;


// this structure corresponds to the grief3 result_path_uris table, and
// is used to hold data relative to that table, within the context
// of the running app.
typedef struct __BP_WEB_PATH_ENTRY {

	// database id associated with this record
	size_t id;

	// sha1 hash
	char sha1[41];

	// path_href entry in table
	char path_href[1025];

	// hits entry in table
	size_t hits;

	// result id which this path was discovered
	size_t initial_result_id;

	// timestamp entry was committed
	char created_ts[257];

	// disabled flags
	size_t disabled;
	size_t disabled_code;

} BP_WEB_PATH_ENTRY, *P_BP_WEB_PATH_ENTRY;



// Holds a list of just in time PCRE expressions which
// can be matched at runtime (directly after each request)
typedef struct __BP_PCRE_JIT_MATCH_ENTRY {

	// Parameters filled in via retrieveWebJITExpressionsFromDB (in Block).  Can
	// be filled in manually, but would be a pain.
	_vdbin size_t id;
	_vdbin size_t user_id;
	_vdbin size_t group_id;
	_vdbin char regexp[BP_KILOBYTE];
	_vdbin char description[BP_KILOBYTE];
	_vdbin char created_ts[128];

	// simple counter for indicating how many times this
	// particular expression was invoked.
	_vout size_t total_number_times_invoked;

	// =======================================================

	// match information associated with JIT
	_vdbout BP_REGEX_MATCH_INFO match_info;


} BP_PCRE_JIT_MATCH_ENTRY, *P_BP_PCRE_JIT_MATCH_ENTRY;


// Structure to hold individual header settings.
typedef struct __BP_HTTP_REQUEST_HEADER {

	// flag indicating whether or not to send this header
	BP_BOOL send_header;

	// the field name (eg Accept-Charset, Cache-Control, etc)
	char * field_name;

	// a simple description of the field
	char * field_description;

	// the set value of the field
	char * value;

} BP_HTTP_REQUEST_HEADER, *P_BP_HTTP_REQUEST_HEADER;

// A static instance containing all the common request headers.  This is duplicated
// for http sessions when they're created.
extern BP_HTTP_REQUEST_HEADER BP_HTTP_COMMON_REQUEST_HEADERS[];


// Search stack for request headers.
typedef struct __BP_HTTP_REQUEST_HEADER_SEARCH_RESULTS {

	// array of pointers that match the search criteria
	P_BP_HTTP_REQUEST_HEADER * headers;

	// number of headers returned
	size_t headers_n;

} BP_HTTP_REQUEST_HEADER_SEARCH_RESULTS, *P_BP_HTTP_REQUEST_HEADER_SEARCH_RESULTS;


// This structure holds a current webpage read directly from the
// curl engine.
typedef struct _CURL_READ_WEBPAGE {


	// actual pointer to real data retrieved
	char * memPtr;
	size_t size;

	// header data (points to the beginning of memPtr)
	char *hdr_data;
	size_t hdr_data_size;

	// page data (points to page data at the end of the headers)
	char *page_data;
	size_t page_data_size;


} CURL_READ_WEBPAGE, *P_CURL_READ_WEBPAGE;



// This structure holds one parsed header value per instance.
typedef struct _BP_HTTP_PARSED_HEADER {

	// string index if found (used for easy lookup)
	char *header_index;

	// length of index header (bytes)
	size_t index_len;

	// string value if found (IMPORTANT: does not need to be free'd.  Is an index
	// into the raw header.)
	char *value;

	// length of value
	size_t value_len;

	// full header (unparsed)
	char *unparsed;
	size_t unparsed_len;

} BP_HTTP_PARSED_HEADER, *P_BP_HTTP_PARSED_HEADER;



// This structure is used to hold a parsed version of the
// raw http headers.  Parse can be wrong if invalid data is
// supplied.  Garbage in, gtfo.
typedef struct _BP_HTTP_PARSED_HEADER_INFO {

	// headers parsed (poniters to allocated structures)
	P_BP_HTTP_PARSED_HEADER headers;

	// total count of headers parsed (number of pointers in array above)
	size_t header_count;

} BP_HTTP_PARSED_HEADER_INFO, *P_BP_HTTP_PARSED_HEADER_INFO;



// This structure holds proxy credentials
typedef struct _BP_HTTP_PROXY_CREDENTIALS {

	// index into the local proxy array to which this credential
	// set corresponds.
	size_t proxyIndex;

	// username string
	char user[512];

	// proxy string
	char pass[512];

} BP_HTTP_PROXY_CREDENTIALS, P_BP_HTTP_PROXY_CREDENTIALS;



// Proxy Use Policy : These enumerations are utilized for determining
// how proxies will be utilized during requests.  If proxies are enabled
// and a valid proxy pool exists for use, the enumerations below will change
// proxy behavior.
enum BP_WEB_PROXY_POLICY
{
	BP_WEB_PROXY_POLICY_NONE,
	BP_WEB_PROXY_POLICY_FULL_RANDOMIZATION_BETWEEN_REQUESTS,
	BP_WEB_PROXY_POLICY_USE_ONE_AT_A_TIME_TILL_PROXY_FAILS_THEN_SWITCH_TO_NEXT,
	BP_WEB_PROXY_POLICY_USE_RANDOM_TILL_PROXY_FAILS_THEN_SWITCH_TO_NEXT_RANDOM
};



// enumerated proxy types
typedef enum BP_WEB_PROXY_TYPE
{
	BP_WEB_PROXY_TYPE_NONE,
	BP_WEB_PROXY_TYPE_HTTP,
	BP_WEB_PROXY_TYPE_SOCKS4,
	BP_WEB_PROXY_TYPE_SOCKS5,
	BP_WEB_PROXY_TYPE_SOCKS4A, /* no dns leaking */
	BP_SEB_PROXY_TYPE_SOCKS5_HOSTNAME
};



// structure for holding proxy sets
typedef struct _BP_WEB_PROXY_EXTENDED_INFO {

	// proxy connect string
	char connect_string[1024];

	// Run time disable flag that fails if HTTPRequest cannot validate the
	// proxy prior to use.
	size_t disabled_due_to_bad_validation;

	// validated
	size_t validated;

	// time the proxy was last validated
	timeval last_validated_time;

	// proxy type
	BP_WEB_PROXY_TYPE proxy_type;

	// local proxy credentials structure
	BP_HTTP_PROXY_CREDENTIALS creds;

} BP_WEB_PROXY_EXTENDED_INFO, *P_BP_WEB_PROXY_EXTENDED_INFO;



// proxy set structure used to hold a collection of web proxy info structures
typedef struct _BP_WEB_USABLE_PROXY_SET {

	// stack of proxy use info structures
	P_BP_WEB_PROXY_EXTENDED_INFO proxies;

	// proxy count (number of structures in the proxies stack)
	size_t proxy_cnt;

	// last proxy used index (index into proxies stack)
	size_t last_proxy_idx;

} BP_WEB_USABLE_PROXY_SET, *P_BP_WEB_USABLE_PROXY_SET;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%% Proxy DB Type Definitions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// structure definition of database proxy groups
typedef struct _BP_PROXY_DB_GROUP {

	// database id associated with this group
	size_t id;

	// user id associated with this proxy group
	size_t user_id;

	// name buffer (identifies group)
	char name[1025];

	// indicates whether or not the proxy group is enabled
	size_t enabled;

	// timestamp in which the proxy group was created
	char created_ts[256];

	// any arbitrary user notes associated with this group
	char user_notes[1024];

} BP_PROXY_DB_GROUP, *P_BP_PROXY_DB_GROUP;



// Defines an unknown proxy type (nont CURLPROXY_??? proxy type).  This value has to be
// outside of the CURLPROXY_??? enum range.  I chose 0xffffffff just on a whim.
#define BP_PROXY_TYPE_UNKNOWN_NONWORKING 0xffffffff



// structure definition for working with database proxy entries
typedef struct _BP_PROXY_DB_ENTRY {

	// database id associated with the entry
	size_t id;

	// user id associated with the entry (relevant to other applications which may aim to integrate with blockparty)
	size_t user_id;

	// group id (proxy groups)
	size_t group_id;

	// proxy type
	size_t proxy_type;

	// actual host
	char host[2048];

	// port on which to connect
	size_t port;

	// proxy connect username (if set)
	char username[2048];

	// proxy connect password (if set)
	char password[2048];

	// notes associated with this proxy
	char user_notes[2048];

	// timestamp this proxy entry was added to the database
	char created_ts[256];

	// timestamp signifying when this proxy was last validated as working (pulls data ok)
	char last_validated_ts[256];

	// whether or not the proxy has been enabled
	size_t enabled;

	// Flag indicating whether this proxy has been validated (tested) during
	// application run time.  This flag is always zero unless a proxy has been
	// validated (aka, it's not set via DB query, it's set in the application)
	size_t proxy_has_been_validated_at_runtime;

	//validations tatus code set during validation testing/updating
	size_t validation_status;


} BP_PROXY_DB_ENTRY, *P_BP_PROXY_DB_ENTRY;


// validation test entry
typedef struct _BP_PROXY_DB_VALIDATION_TEST_ENTRY {

	// db id
	size_t id;

	// user id
	size_t user_id;

	// group id
	size_t group_id;

	// url to fetch
	char url_fetch[1024];

	// post arguments
	char post_args[1024];

	// regular expression to match
	char regexp_match[1024];

	// number of times this test has been invoked (used for balancing tests)
	size_t count_invoked;

	// created timestamp
	char created[256];

} BP_PROXY_DB_VALIDATION_TEST_ENTRY, *P_BP_PROXY_DB_VALIDATION_TEST_ENTRY;


// Structure used for holding proxy-validation test group entries.
typedef struct _BP_PROXY_DB_VALIDATION_TEST_GROUP {

	// db id
	size_t id;

	// user id
	size_t user_id;

	// group name
	char group_name[1024];

	// group description
	char group_description[1024];

	// created timestamp
	char created_ts[256];

} BP_PROXY_DB_VALIDATION_TEST_GROUP, *P_BP_PROXY_DB_VALIDATION_TEST_GROUP;



// user agent selection enumeration
enum BP_USER_AGENT_POOL
{
	BP_USER_AGENT_POOL_NONE,
	BP_USER_AGENT_POOL_BROWSERS_ONLY,
	BP_USER_AGENT_POOL_ALL
};


// This is used for retrieving curl debugging data via callback.
typedef struct __BP_HTTP_DEBUG_DATA {

	// trace data
	char trace_ascii; /* 1 or 0 -- I don't know what this is all about but it's in the cURL documentation */


} BP_HTTP_DEBUG_DATA, *P_BP_HTTP_DEBUG_DATA;

// This structure holds relevant information in regards to a
// http session.
typedef struct _BP_HTTP_SESSION {


	// if this is a cloned http session, this flag is set to
	// BP_TRUE
	BP_BOOL is_clone;

	// memory manager used for handling all allocations in a http session.
	P_BP_LINKL m;

	// url associated with last request.   Only set/updated after a request is completed.
	char * lastRequestedHref;

	// local curl object
	CURL *curl;

	// enum curl code (not allocated ever)
	CURLcode res;

	// user credentials
	char *userPass;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Request Blacklists %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// request blacklist
	char ** request_regexp_blacklist;

	// request blacklist count
	size_t request_regexp_blacklist_n;

	// flag set during each request.  Value is 0 if non-blacklisted, non-zero if blacklisted.
	size_t last_request_was_blacklisted;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%% Proxy Sets (Database Integrated Version) %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// DB-ORIENTED: Proxy set enabled flag.  Toggle this to use proxies for requests.
	bool use_db_proxy_set;

	// if this is set to true, the block will skip JIT validation (best
	// when used with reliable proxies, such as tor circuits)
	bool skip_jit_validation_checks;

	// DB-ORIENTED: proxy groups that have been populated dynamically
	P_BP_PROXY_DB_GROUP db_populated_proxy_groups;
	size_t              db_populated_proxy_group_n;

	// DB-ORIENTED: This set of proxies is the primary place to find proxy data after requests
	// have been made.
	P_BP_PROXY_DB_ENTRY db_populated_proxy_sets;
	size_t              db_populated_proxy_n;

	// DB-ORIENTED: Validation test sets are used for validating that proxies are alive and working as intended.
	// Proxy groups are used to manage sets of validation tests.  These variables contain said proxy groups.
	P_BP_PROXY_DB_VALIDATION_TEST_GROUP db_populated_proxy_validation_groups;
	size_t								db_populated_proxy_validation_group_n;

	// Individual validation test sets
	P_BP_PROXY_DB_VALIDATION_TEST_ENTRY db_populated_proxy_validation_tests;
	size_t								db_populated_proxy_validation_test_n;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%% Proxy Sets (non-db integrated version) %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// IMPORTANT NOTICE: Only one proxy solution can be used.  This version is
	//                   in no way connected to a database, and can be used in a pinch.
	//                   However, it is less feature rich and more slap-dash than then
	//                   database integrated version.  It is recommended that you choose
	//                   the db integrated version where possible.

	// NON-DB: Proxy set enabled flag.  Toggle this to use proxies for requests.
	bool use_web_proxy_set;

	// NON-DB: Usable proxy set.
	P_BP_WEB_USABLE_PROXY_SET proxy_set;

	// NON-DB: Sets the current proxy config to use when making requests.
	size_t proxy_set_config_index;

	// NON-DB: This flag is used for JIT proxy checking and signifies
	// whether or not a proxy set should take place before a request.  If
	// this is true, the proxy application code in HTTPRequest function
	// will not be applied, allowing for proxy checking to take place without
	// a recursive loop occuring.
	size_t disable_proxy_check_in_http_request;

	// -- old

	// DEPRECATED FOR FUTURE DELETION: proxy information
	char **proxies;
	size_t proxyCount;

	// DEPRECATED FOR FUTURE DELETION : proxy credentials array
	BP_HTTP_PROXY_CREDENTIALS *proxyCredentials;
	size_t                     proxyCredCount;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Detailed Request Settings %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// HTTP Auth methods
	int httpAuthNonDefault; // default is CURLAUTH_ANY

	// Follow location, true false, enable the following of redirection headers
	int httpFollowLocation;
	int httpMaxFollowRedirs;

	// PUT Request
	int isHttpPut;
	char *fileName;
	int fileSize;

	// GET Request
	int isHttpGet;

	// POST Request
	int isHttpPost;
	char * httpPostData; // most likely should be URL encoded

	// port information
	int interfaceIsSet;
	char * interface;

	// netrc
	int netrcIsSet;
	char *netrcFile;

	// ssl verify peer and host (true/false)
	int sslVerifyPeer;
	int sslVerifyHost;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%% Cookie Settings %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set this to true if you have a cookie set
	int cookieIsSet;

	// flag to indicate whether we are using a cookie from a file
	int useCookieFromFile;

	// actual cookie value
	char *cookie;

	// cookie file (need to double check)
	char *cookieFromFile;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Request Headers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// These request headers (if send_enabled) will be sent along
	// with each request.  The table is auto populated on creation (via tq)
	// with the contents of BP_HTTP_COMMON_REQUEST_HEADERS.  All initial
	// header values are disabled to start with and must be enabled manually
	// if you want them set.  Additional headers can be added to this array
	// by calling BP_HTTPAddRequestHeader() directly.
	P_BP_HTTP_REQUEST_HEADER request_headers;

	// number of headers in the above array
	size_t request_headers_n;

	// localized search results (easy storage here for easy destroy later)
	P_BP_HTTP_REQUEST_HEADER_SEARCH_RESULTS request_header_search_results;

	// Prepared headers in slist.  This is used because curl takes
	// the CURLOPT_HTTPHEADER option in the form of a slist.  This
	// is destroyed iteratively before each request is called.  You shouldn't
	// mess with it unless you absolutely know what you're doing.

	struct curl_slist * request_header_chunk_list;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%% Standard Web Analysis Settings %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if set to true, the underlying library will use a random sleep
	// timer to throw off audit detection.
	size_t use_request_timing;

	// number of retries on a request before the request engine
	// stops trying to retrieve a page.
	size_t max_request_retries;

	// this is the rand()%modulo value used for sleeps.
	size_t request_time_modulo;

	// start/end times in unix epoch format
	time_t request_start_time;
	time_t request_end_time;

	// total time in seconds it took to make the request
	size_t req_time_s;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Recieved Headers From Server %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// parsed and sorted header info.  Raw header, unparsed, can be found
	// in the ->page structure directly below.
	P_BP_HTTP_PARSED_HEADER_INFO headers;
	size_t                       header_count;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% JIT PCRE REGEXP Matching %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// just in time regular expression checks
	P_BP_PCRE_JIT_MATCH_ENTRY jit_pcre_regexp_checks;

	// number of jit regexp checks to run
	size_t jit_pcre_regexp_check_n;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Connect Codes %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// the status code returned from the server (404, 302, 500, etc)
	size_t http_status_code;

	// the connect code returned by curl
	size_t http_connect_code;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Timeout Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this should be set reasonably, as the curl default is 300
	// seconds.
	size_t http_connect_timeout;

	// this should also be set reasonably, but is less drastic
	// than the above.
	size_t http_session_timeout;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%% Requested Page Information After Request %%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Current Page Information
	CURL_READ_WEBPAGE *page;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Debug Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Debug information is necessary to gather HTTP sent header
	// information, as well as typical data which has been sent.

	// debug data
	P_BP_HTTP_DEBUG_DATA dbg_data;


} BP_HTTP_SESSION, *P_BP_HTTP_SESSION;


// Static Reallocation Routine
void * BP_myrealloc(void *ptr, size_t size);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Request Callbacks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Create a callback to handle read data being retrieved (converse is BP_HTTPDebugCallback, even
// though it has debug in it's name, it's actually used for determining data before it's actually
// being sent through)
size_t BP_GetPageCallback(void *ptr, size_t size, size_t nmemb, void *data);

// This is a prototyped function (from cURL documentation) to be used for the purposes
// of collecting cURL debug information outbound.
static int BP_HTTPDebugCallback(CURL *handle, curl_infotype type, char *data, size_t size, void * userp);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Cookie Oriented Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Sets cookie
int BP_HTTPSetCookie(P_BP_HTTP_SESSION session, char *cookie);

// Sets cookie jar (holds cookies done with authentication)
int BP_HTTPSetCookieJar(P_BP_HTTP_SESSION session, char *cookie_jar_file);

// Gets a duplicated cookie string from the last set of headers recieved (for session managment)
char * BP_HTTPGetCookieFromLastHeadersRecievedSetCookie(P_BP_HTTP_SESSION session);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Username Oriented Routines %%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// NULL for blank in either value (Allocates new userPass Chunk)
int BP_HTTPSetUserPass(P_BP_HTTP_SESSION session, char *user, char *pass);

// Sets HTTP POST Arguments
int BP_HTTPSetPostVars(P_BP_HTTP_SESSION session, char *postString);

// Destroys a sessions iterative data only.  This can be safely used
// in web result stack (see: Block class) iterative loops.
BP_ERROR_T BP_HTTPDestroySessionIterativeData(P_BP_HTTP_SESSION session);

// Gets a new HTTP String (clean_session is true or false, and indicates whether
// or not to destroy iterative data and clear out curl data.
int BP_HTTPRequest(P_BP_HTTP_SESSION session, char *URL, size_t clean_session = false);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Enable Redirects %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// enable 302 redirect following in the provided curl handle
BP_ERROR_T BP_HTTPSetFollowRedirects(P_BP_HTTP_SESSION session, BP_BOOL set_val, size_t max_redirects);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Session Creation/Deletion/Invocation %%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Creates a new session structure, used by invoke session
P_BP_HTTP_SESSION BP_HTTPCreateSession();

// HTTP Global Initialization Routine
int BP_HTTPGlobalInit();

// Blockpartys HTTP Interface, starts a session and retrieves a new page, storing
// it in a local buffer.
int BP_HTTPInvokeSession(P_BP_HTTP_SESSION session);

// Clones relevant session data, including proxies, settings, and most non
// extant data.  This data is mostly used in threads so that threads can easily
// gather http session settings before invoking their own connections.
P_BP_HTTP_SESSION BP_HTTPCloneSession(P_BP_HTTP_SESSION session);

// Destroys a HTTP Session
int BP_HTTPDestroySession(P_BP_HTTP_SESSION session);

// Destroys a HTTP Session Clone.  Cloned sessions cannot be destroyed
// using BP_HTTPDestroySession, as their allocations are not all from
// the clone tailqueue, and could lead to possible double free conditions.
BP_ERROR_T BP_HTTPDestroyClonedSession(P_BP_HTTP_SESSION session);

// This does not destroy an entire session, but only frees its
// CURL pointer data.  This function is primarily used with the
// proxy subsystem in order to create new curl sessions on the fly
// without taking down the entire http session.
int BP_HTTPDestroySessionCurlOnly(P_BP_HTTP_SESSION session);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Timeout Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Set timeout
int BP_HTTPSetTimeout(P_BP_HTTP_SESSION session, size_t seconds);

// set the connect timeout
BP_ERROR_T BP_HTTPSetConnectTimeout(P_BP_HTTP_SESSION session, size_t seconds);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Utility Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Display a Page Structure.  This is not to be confused with a HTML parse
// structure.  This is just raw data that curl provides, stored in a
// usable structure.
int BP_HTTPDisplayWebpageStructure(P_CURL_READ_WEBPAGE data, size_t display_content);


// All in one display routine, attempts to print out http info.
BP_ERROR_T BP_HTTPDisplaySession(P_BP_HTTP_SESSION session);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% HTTP Request Headers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// === Apply ==

// actually apply all enabled headers which the user has set, to the cURL object
// within the session.  This is called automatically within BP_HTTPRequest()
// before any request is performed, so don't call it directly unless
// you know what you're doing.
BP_ERROR_T BP_HTTPApplyAllEnabledRequestHeadersToCurrentRequest(P_BP_HTTP_SESSION session);

// === Add/Search/Modify ==

// Adds all common request headers to the current HTTP session.
BP_ERROR_T BP_HTTPAddAllCommonRequestHeadersToSession(P_BP_HTTP_SESSION session);

// Adds a request header to the provided http session.
BP_ERROR_T BP_HTTPAddRequestHeader(P_BP_HTTP_SESSION session, char * field_name, char *field_description, char *value, BP_BOOL enabled_for_send);

// destroy all the request headers in the session (uses tail queue allocator)
BP_ERROR_T BP_HTTPDestroyRequestHeaders(P_BP_HTTP_SESSION session);

// Searches for request headers in the http session that match a specified
// criteria.  Set enabled to BP_INVALID_UNSET to omit searching the enabled field.
BP_ERROR_T BP_HTTPSearchRequestHeadersByCriteria(P_BP_HTTP_SESSION session, char **field_names, BP_BOOL enabled = BP_INVALID_UNSET);

// Destroys search results if necessary.
BP_ERROR_T BP_HTTPDestroyRequestHeadersSearchResults(P_BP_HTTP_SESSION session);

// == Update ==

// Update a request header with a new value or status.
BP_ERROR_T BP_HTTPUpdateRequestHeader(P_BP_HTTP_SESSION session, P_BP_HTTP_REQUEST_HEADER header, char *new_val, BP_BOOL new_enabled_status);

// Update all the items in current search results with the provided data.
BP_ERROR_T BP_HTTPUpdateRequestHeadersFromSearchResults(P_BP_HTTP_SESSION session, char **new_vals, size_t new_val_n, BP_BOOL * new_enabled_statuses = NULL, size_t status_n = 0);

// == Display Results ==

// Display request header search results
BP_ERROR_T BP_HTTPDisplayRequestHeaderSearchResults(P_BP_HTTP_SESSION session);

// Display the request headers for the current session.  Set second parameter to BP_TRUE, BP_FALSE to match.  Set to BP_INVALID_UNSET
// to display all (default).
BP_ERROR_T BP_HTTPDisplayRequestHeaders(P_BP_HTTP_SESSION session, BP_BOOL only_display_enabled_matches = BP_INVALID_UNSET);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Http Returned Header Manipulation/Extraction %%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Creates a pointer to a structure of header elements.  The elements are
// seperated by index if one is available.
P_BP_HTTP_PARSED_HEADER_INFO BP_HTTPParseHeaders(P_CURL_READ_WEBPAGE data);

// print out headers
BP_ERROR_T BP_HTTPDisplayHeaderData(P_BP_HTTP_PARSED_HEADER_INFO header_info);

// destroys headers
BP_ERROR_T BP_HTTPDestroyParsedHeaders(P_BP_HTTP_PARSED_HEADER_INFO header_info);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Proxy Setters/Validators/Randomizers %%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Sets a generic http proxy
int BP_HTTPSetProxy(P_BP_HTTP_SESSION session, char *proxy);

// Sets a proxy with a defined type
int BP_HTTPSetProxyType(P_BP_HTTP_SESSION session, int proxyType);

// Validates a proxy by requesting a validation URL
int BP_HTTPValidateCurrentProxy(P_BP_HTTP_SESSION session, char *validateURL, char *successStr);

// Attempts to determine (by BRUTE FORCE) what type of proxy a given connect string alludes to.
// It will return either ERR_FAILURE, or CURLPROXY_(SOCKS4/SOCKS5/HTTP).  Can use any connect string
// usable by curl.  Requires a HTTP session in order to work.  To pass a user/password, embed them in
// proxy string as follows: http://user:pass@ipaddr:port  cURL, and this library ignore the http:// prefix.
size_t BP_HTTPValidateProxyTypeByBruteForce(P_BP_HTTP_SESSION session, char *connect_string, char *validateURL, char *successStr, size_t http_timeout);

// This structure holds parameters which are passed to the threaded
// proxy validator void entry point.  This data is mutable by the threads
// and should not ever be accessed until a thread exits.  Following this
// will ensure no contention issues; disregard at your own risk.
typedef struct _BP_DB_VALIDATION_THREAD_PARMS {

	// proxy entry to validate
	P_BP_PROXY_DB_ENTRY proxy_entry;

	// timeout (in seconds) to wait for a successful proxy request
	size_t timeout;

	// proxy validation tests to use
	P_BP_PROXY_DB_VALIDATION_TEST_ENTRY test_entries;
	size_t                              test_entry_n;
	size_t                              test_attempt_before_failing_validation;

	// flag indicating whether or not to attempt to brute force the proxy type if no type is set
	size_t brute_force_proxy_type;

} BP_DB_VALIDATION_THREAD_PARMS, *P_BP_DB_VALIDATION_THREAD_PARMS;

// This version of the above validation function (HTTPValidateProxyTypeByBruteForce) is used for validating
// proxies in a threaded model.  It can be called by pthread_create or the native Block() class bindings in order
// to attempt to validate a proxy.
void * BP_HTTPValidateProxyTypeByBruteForce_thinit(/* P_BP_DB_VALIDATION_THREAD_PARMS */ void *validation_parms);

// This routine attempts to set the proxy for the session using only a proxy DB entry structure.
BP_ERROR_T BP_HTTPSetProxyViaProxyDBEntryRecord(P_BP_HTTP_SESSION session, P_BP_PROXY_DB_ENTRY record);

// Attempts to validate a proxy via a proxydb validation test record.
BP_ERROR_T BP_HTTPValidateProxyViaProxyDBTestRecord(P_BP_HTTP_SESSION session, P_BP_PROXY_DB_ENTRY proxy, P_BP_PROXY_DB_VALIDATION_TEST_ENTRY test, size_t brute_force_proxy_type);

// Display proxy record (non-class oriented version, can be called within thread)
BP_ERROR_T BP_HTTPDisplayProxyDBEntryRecord(P_BP_PROXY_DB_ENTRY record);

// Set Proxies in Session and assigns count
int BP_HTTPSetProxyArray(P_BP_HTTP_SESSION session, char **proxies);


// Generates a char ** array of proxies from a file
char ** BP_HTTPGenerateProxyListFromFile(char *filePath);

// Gets a count from the char ** generated above
size_t BP_HTTPGetProxyCount(char **proxyArray);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% PCRE Blacklist Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// add a pcre blacklist items
BP_ERROR_T BP_HTTPAddPCREBlacklistItem(P_BP_HTTP_SESSION session, char * pcre);

// validate blacklist entries
BP_ERROR_T BP_HTTPValidatePCREBlacklistAgainstURL(P_BP_HTTP_SESSION session, char *URL);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% User Agent Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Sets HTTP User Agent
int BP_HTTPUserAgent(P_BP_HTTP_SESSION session, char *agent);

// Browser agent only generator
char ** BP_GenerateBrowserAgentOnlyArray();

// Huge valid user agent array, contains crawlers and browsers alike
char ** BP_GenerateAllAgentsArray();

// Returns an array count for generating random user strings, ARRAY MUST BE
// NULL PTR TERMINATED.
size_t BP_HTTPUserAgentCount(char **userAgents);

// pulls a random user agent string from the global array and returns
// a strndup(1024)'ed buffer back.
char * BP_HTTPGetRandomUserAgentString(char **agents, size_t totalElements);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Parse Utility Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Allocates a new character buffer which takes an anchor URL which
// contains a script name or extraenous data, and removes the data
// until it finds a precursory /.

// eg. /test/something.php will be returned in a new buffer as /test/

// Fails if no previous / is found in which case NULL is returned.
char * BP_HTTPStripScript(char *url, size_t len);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%% CURL VALUE GETTRS %%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Gets a null terminated ascii string containing the ip address of the
// last connection made by curl.
char * BP_HTTPGetRemoteLastIP(P_BP_HTTP_SESSION session);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%% PREG JIT Match Tests %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// after a page has been retrieved, the memory buffers are populated.  At this point
// we can run our JIT check set.
BP_ERROR_T BP_HTTPRunJITExpressionChecks(P_BP_HTTP_SESSION session);

// displays a regexp test
BP_ERROR_T BP_DisplayJITRegexpTest(P_BP_PCRE_JIT_MATCH_ENTRY jit_test);

// destroy a set of match entries, free'ing all relevant data
BP_ERROR_T BP_DestroyJITMatchEntries(P_BP_PCRE_JIT_MATCH_ENTRY jit_test);

// TAILQ VERSION: destroy a set of match entries, free'ing all relevant data
BP_ERROR_T BP_DestroyJITMatchEntriesTQ(P_BP_PCRE_JIT_MATCH_ENTRY jit_test, P_BP_LINKL tq);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%% Database Oriented Web Request Balancer %%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// create a new web request balancer
P_BP_DB_WEB_REQUEST_BALANCER BP_CreateDBWebRequestBalancer();

// destroy a web request balancer
BP_ERROR_T BP_DestroyDBWebRequestBalancer(P_BP_DB_WEB_REQUEST_BALANCER balancer);

// display the web reuest balancer
BP_ERROR_T BP_DisplayDBWebRequestBalancer(P_BP_DB_WEB_REQUEST_BALANCER balancer, size_t display_balance_table);

// run a url through the balancer
BP_ERROR_T BP_DBWebRequestBalancerProcessTarget(P_BP_DB_WEB_REQUEST_BALANCER balancer, char * target);

// push a whitelist element onto the balancer stack
BP_ERROR_T BP_DBWebRequestBalancerPushWhitelistItem(P_BP_DB_WEB_REQUEST_BALANCER balancer, P_BP_DB_WEB_WEIGHTED_WHITELIST_ELEMENT element);

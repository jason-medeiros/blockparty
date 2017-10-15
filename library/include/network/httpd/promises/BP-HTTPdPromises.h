#ifndef __BP_HTTPD_PROMISES_H
#define __BP_HTTPD_PROMISES_H

typedef enum
{
	BP_HTTPD_PROMISE_STATE_UNSET,
	BP_HTTPD_PROMISE_STATE_UNSTARTED_PENDING,
	BP_HTTPD_PROMISE_STATE_STARTED,
	BP_HTTPD_PROMISE_STATE_FINISHED

} BP_PROMISE_STATE;

// These are the differing categeories of promise peer endpoints.
typedef enum
{
	BP_HTTPD_PROMISE_PEER_TYPE_UNSET,
	BP_HTTPD_PROMISE_PEER_TYPE_MASTER,
	BP_HTTPD_PROMISE_PEER_TYPE_PROCESSING_ENDPOINT,
	BP_HTTPD_PROMISE_PEER_TYPE_UTILITY_ENDPOINT

} BP_PROMISE_PEER_TYPE;

// When peering is setup, these peers are utilized to hold
// promise peer information.
typedef struct __BP_HTTPD_PROMISE_PEER {

	// type of peer
	BP_PROMISE_PEER_TYPE peer_type;

	// hostname of the peer
	char *promise_peer_hostname;

	// peer ip address information
	sockaddr_in promise_peer_ip_info;

	// port to connect to the promise peer server on
	unsigned short promise_peer_port;

	// peering authentication key
	char *auth_api_key;

	// indicates if the peer is a tor host or not
	BP_BOOL peer_is_tor_host;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Peer Service Offering %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if the peer service data has been updated, this is set to true
	BP_BOOL peer_service_info_retrieved_ok;

	// this is the url of the service offering (eg. https://{promise_peer_hostname_here}/promises/)
	char * peer_service_offering_api_url;


	// this is the peering version of the
	char * peer_version_reported;

	// this is last parsed json containing service offerings
	P_BP_JSON peer_service_offerings;

	// the last time the peer service offerings were last updated
	time_t peer_service_offerings_last_updated;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Peer Configuration Settings %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// If this is set to true, this peer will serve reciepts through
	// it's own public peer registry, and peer reciepts will be distributed
	// directly.  This means that the parent will only get a stub reciept
	// indicating that it can remove it's peer processor request.
	BP_BOOL peer_can_serve_reciepts;





} BP_HTTPD_PROMISE_PEER, *P_BP_HTTPD_PROMISE_PEER;



// Promise reciepts are provided as output to the client recieving them.  They
// are destroyed on pickup or they are culled based on culling parameters.
typedef struct __BP_HTTPD_PROMISE_RECEIPT {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Creation Parameters %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// the time the promise was created/added
	time_t time_created;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Creator Parameters %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Remote IP stored as a character string (for convienence)
	char *remote_ip_as_chr;

	// remote ip address information (in familiar sockaddr format)
	sockaddr_in remote_ip;


	// If the remote creator is an administrator, they bypass all
	// promises currently in line to be processed.
	BP_BOOL creator_is_admin;

	// Indicates whether the promise has been fufilled or not.
	BP_BOOL promise_has_been_fufilled;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Creator Parameters %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// event priority information
	BP_EVENT_PRIORITY event_priority;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Promise Request Data %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this is the json for the specified promise request
	char * promise_request_json;

	// length of the promise request JSON
	size_t promise_request_json_len;

	// this is set to BP_TRUE if the json was parsed ok and a
	// filter was generated.
	BP_BOOL promise_request_json_parsed_ok;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Culling Parameters %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// the time which this reciept should be culled
	time_t time_to_cull;


} BP_HTTPD_PROMISE_RECEIPT, *P_BP_HTTPD_PROMISE_RECEIPT;

typedef struct __BP_HTTPD_PROMISE {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Promise Flags %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// owner of this promise
	BP_HTTPD_USER httpd_owner;

	// this is the guid reciept for the promise
	char * promise_reciept_identifier;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Promise Flags %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// the promise has completed ok
	BP_BOOL promise_completed_ok;

	// this is set if the promise has returned data
	BP_BOOL promise_returned_data;

	// this promise was returned from a db query
	BP_BOOL promise_result_returned_from_db;

	// if this result was fetched from a remote resource, this is marked
	BP_BOOL promise_result_returned_from_remote_resource;

	// the promise result is returned from local memory
	BP_BOOL promise_result_returned_from_local_memory;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Events %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// event associated with this promise
	P_BP_EVENT event;

} BP_HTTPD_PROMISE, *P_BP_HTTPD_PROMISE;


// Promises session structure.  This is primarily used
// for looking up and routing promises to various event
// endpoints.
typedef struct __BP_HTTPD_PROMISES_SESSION {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Peer Registry %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	P_BP_HASH_TABLE_REGISTRY master_peer_registry;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Promieses Counter %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// current number of promises waiting
	size_t server_current_promises_waiting_count;

	// current number of promises which were exported to our peers
	size_t server_current_promises_exported_to_peers;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Promises Settings %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Always process admin queries first.  This prevents promise limiting
	// from allowing admin promises through.  Admin promises should always
	// override any other in the queue, executing almost immediately
	// where possible.
	BP_BOOL admin_promises_override_promise_count;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Promise Caching %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// If this is set to true, promises will be cached to disk when a limit is hit instead
	// of being simply disgarded.
	BP_BOOL cache_promises_to_disk_when_memory_limit_is_hit;

	// Due to the potential of cache promise abuse (eg users/hackers filling disk), a maximum
	// cache limit should be set.  This value should never exceed (your_available_disk_space / sizeof(BP_HTTPD_PROMISE)).
	size_t max_number_of_promises_to_cache;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Internal Registries %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// system hreg used for allocation and storing promises
	P_BP_HASH_TABLE_REGISTRY system_hreg;


} BP_HTTPD_PROMISES_SESSION, *P_BP_HTTPD_PROMISES_SESSION;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create / Destroy Promises Sessions %%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

P_BP_HTTPD_PROMISES_SESSION BP_PromisesCreateSession
(
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Promises Default Request Handler %%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This request handler is used to add promises to the table.
BP_HTTPD_DECLARE_ACCESS_HANDLER_CALLBACK(BP_PromiseDefaultRequestHandler);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Peer Reciept Request Handlers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// TODO: Build out peer-reciept handlers (peers request reciept tables between hosts for easier spread pickup)
BP_HTTPD_DECLARE_ACCESS_HANDLER_CALLBACK(BP_PromiseAbsorbRemotePromiseReciepts);



#endif

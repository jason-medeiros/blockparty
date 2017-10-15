/*
 * BP-WebChat.h
 *
 *  Created on: Jan 31, 2016
 *      Author: root
 */

#ifndef INCLUDE_WEBCHAT_BP_WEBCHAT_H_
#define INCLUDE_WEBCHAT_BP_WEBCHAT_H_


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% WebChat Sessions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This is the name of the event processor for handling webchat events.
#define WEBCHAT_EVENT_PROCESSOR_NAME "GR4_TorEventProcessor"

// event types
typedef enum
{

	BP_WEBCHAT_EVENT_UNSET,
	BP_WEBCHAT_EVENT_USER_ADD_TO_CHANNEL,
	BP_WEBCHAT_EVENT_USER_REMOVE_FROM_CHANNEL,
	BP_WEBCHAT_EVENT_USER_GET_METADATA,
	BP_WEBCHAT_EVENT_CHANNEL_ADD,
	BP_WEBCHAT_EVENT_CHANNEL_REMOVE,
	BP_WEBCHAT_EVENT_CHANNEL_ADD_MESSAGE_FROM_USER,
	BP_WEBCHAT_EVENT_SYSTEM_GET_HREG

} BP_WEBCHAT_EVENT_TYPE;

// event return codes
typedef enum
{

	BP_WEBCHAT_EVENT_RETURN_FAILURE,
	BP_WEBCHAT_EVENT_RETURN_SUCCESS

} BP_WEBCHAT_EVENT_RETURN_CODE;


// event status types
typedef enum
{

	BP_WEBCHAT_EVENT_STATUS_UNSTARTED,
	BP_WEBCHAT_EVENT_STATUS_STARTING_UP,
	BP_WEBCHAT_EVENT_STATUS_RUNNING,
	BP_WEBCHAT_EVENT_STATUS_STOPPING,
	BP_WEBCHAT_EVENT_STATUS_FINISHED

} BP_WEBCHAT_EVENT_STATUS;

// types of channel messages which can be sent
typedef enum
{

	BP_WEBCHAT_CHANNEL_MESSAGE_UNSET,
	BP_WEBCHAT_CHANNEL_MESSAGE_TO_CHANNEL,
	BP_WEBCHAT_CHANNEL_MESSAGE_TO_CHANNEL_GROUP,
	BP_WEBCHAT_CHANNEL_MESSAGE_TO_USER,
	BP_WEBCHAT_CHANNEL_MESSAGE_TO_USER_GROUP,
	BP_WEBCHAT_CHANNEL_MESSAGE_TO_ALL

} BP_WEBCHAT_CHANNEL_MESSAGE_TYPE;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Social Status %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// webchat user social status
typedef enum
{

	BP_WEBCHAT_USER_SOCIAL_STATUS_ABSOLUTELY_HATED,
	BP_WEBCHAT_USER_SOCIAL_STATUS_EXTREMELY_HATED,
	BP_WEBCHAT_USER_SOCIAL_STATUS_HATED,
	BP_WEBCHAT_USER_SOCIAL_STATUS_MOSTLY_HATED,
	BP_WEBCHAT_USER_SOCIAL_STATUS_SOMEWHAT_HATED,
	BP_WEBCHAT_USER_SOCIAL_STATUS_SOMEWHAT_LIKED,
	BP_WEBCHAT_USER_SOCIAL_STATUS_MOSTLY_LIKED,
	BP_WEBCHAT_USER_SOCIAL_STATUS_LIKED,
	BP_WEBCHAT_USER_SOCIAL_STATUS_EXTREMELY_LIKED,
	BP_WEBCHAT_USER_SOCIAL_STATUS_ABSOLUTELY_LIKED

} BP_WEBCHAT_USER_SOCIAL_STATUS;


// Webchat channel group user flags
typedef enum
{

	BP_WEBCHAT_CHANNEL_GROUP_USER_BANNED,
	BP_WEBCHAT_CHANNEL_GROUP_USER_HATED

} BP_WEBCHAT_CHANNEL_GROUP_USER_FLAGS;


// Webchat channel user flags

// Webchat channel user level
typedef enum
{
	BP_WEBCHAT_CHANNEL_USER_LEVEL_UNSET,
	BP_WEBCHAT_CHANNEL_USER_LEVEL_BANNED_USER,
	BP_WEBCHAT_CHANNEL_USER_LEVEL_ANONYMOUS_USER,
	BP_WEBCHAT_CHANNEL_USER_LEVEL_NORMAL_USER,
	BP_WEBCHAT_CHANNEL_USER_LEVEL_CHANNEL_OPER_USER,
	BP_WEBCHAT_CHANNEL_USER_LEVEL_CHANNEL_GROUP_OPER_USER

} BP_WEBCHAT_CHANNEL_USER_LEVEL;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Chat Registry Metadata %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// metadata for the /channels/ table.
typedef struct __BP_WEBCHAT_CHANNELS_METADATA {


	// total number of channels
	size_t channel_count;


} BP_WEBCHAT_CHANNELS_METADATA, *P_BP_WEBCHAT_CHANNELS_METADATA;


// metadata for the /users/ table.
typedef struct __BP_WEBCHAT_USERS_METADATA {

	// total number of users connected to this webchat
	size_t user_count;

} BP_WEBCHAT_USERS_METADATA, *P_BP_WEBCHAT_USERS_METADATA;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Webchat Users %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// webchat user
typedef struct __BP_WEBCHAT_USER {


	// authenticated httpd user
	P_BP_HTTPD_USER httpd_user;

	// username
	char *chat_user;

	// is administrator
	BP_BOOL is_admin;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Social Status Ranking %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This is the social status ranking for this particular user.
	BP_WEBCHAT_USER_SOCIAL_STATUS overall_social_status_ranking;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Creation Privileges %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// can create channel group
	BP_BOOL can_create_channel_group;

	// user can create channels
	BP_BOOL can_create_channel;

	// users can create groups
	BP_BOOL can_create_user_group;




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Total Message Statistics %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// total number of messages sent
	size_t total_messages_sent;

	// total number of messages recieved
	size_t total_messages_recieved;




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Total Message Social Counts %%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This is the ANONYMOUS number of times other users have
	// approved of a users messages.
	size_t total_user_approved_messages;

	// This is the ANONYMOUS number of times other users have
	// disapproved of a users messages.
	size_t total_user_disapproved_messages;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% User Change Privs %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this user can change it's own username
	BP_BOOL can_change_username;

	// this user can change it's own password
	BP_BOOL can_change_password;



} BP_WEBCHAT_USER, *P_BP_WEBCHAT_USER;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Channel and Channel Groups %%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Channel groups
typedef struct __BP_WEBCHAT_CHANNEL_GROUP {


	// group name
	char *group_name;

	// group owner
	P_BP_WEBCHAT_USER group_owner;


} BP_WEBCHAT_CHANNEL_GROUP, *P_BP_WEBCHAT_CHANNEL_GROUP;


// Metadata associated with a specific user within a channel.
typedef struct __BP_WEBCHAT_CHANNEL_USER_INFO {

	// Indicates if the user is a channel admin.  Only a channel admin can create or destroy a channel.
	BP_BOOL is_channel_admin;

	// Indicates if the user is a channel op.  Channel ops can invite people, remove people, ban people, or voice people.
	BP_BOOL is_channel_op;

	// indicates that this user is a channel mod
	BP_BOOL is_channel_mod;

	// indicates that this user is a channel normal user
	BP_BOOL is_channel_normal_user;

	// indicates that a user is in a room, but not visible to other users (must be promoted
	// by an admin)
	BP_BOOL is_invisible;

} BP_WEBCHAT_CHANNEL_USER, *P_BP_WEBCHAT_CHANNEL_USER;


// Channel structure held in the webchat system_hreg
typedef struct __BP_WEBCHAT_CHANNEL {


	// name of the channel
	char * channel_name;

	// channel topic
	char * channel_topic;

	// number of channel members
	size_t member_count;

	// channel is public (public channel marker
	BP_BOOL channel_is_public;


} BP_WEBCHAT_CHANNEL, *P_BP_WEBCHAT_CHANNEL;



// When a user joins a channel, this structure holds metadata
// regarding what privileges that user has within that channel.
typedef struct __BP_WEBCHAT_CHANNEL_USER_METADATA {


	// pointer to the channel in question
	P_BP_WEBCHAT_CHANNEL channel;


	// user associated with this set of privileges
	P_BP_WEBCHAT_USER user;

	// the privilege level for this user at this privilege level
	BP_WEBCHAT_CHANNEL_USER_LEVEL user_level;

	// user social status in this channel
	BP_WEBCHAT_USER_SOCIAL_STATUS user_social_status;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Channel Change Privs %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this user can change a channel topic
	BP_BOOL can_change_channel_topic;

	// this user can change a channel secret key
	BP_BOOL can_change_channel_secret_key;

	// this user can change a channel crypto algorithm
	BP_BOOL can_change_channel_crypto_algo;



}  BP_WEBCHAT_CHANNEL_USER_METADATA, *P_BP_WEBCHAT_CHANNEL_USER_METADATA;




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Webchat Messages %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Channel message structure held in the webchat system_hreg
typedef struct __BP_WEBCHAT_MESSAGE {


	// message type
	BP_WEBCHAT_CHANNEL_MESSAGE_TYPE type;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Channel or Channel Group %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// channel group if sending to a channel group
	P_BP_WEBCHAT_CHANNEL_GROUP channel_group;

	// channel that this message belongs to
	P_BP_WEBCHAT_CHANNEL channel;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Users %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// user which sent this message
	P_BP_USER from_user;

	// If this is a user-to-user message, this is the user the
	// message is from.
	P_BP_USER to_user;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Message %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// message
	char *msg;

	// message length
	size_t msg_len;


} BP_WEBCHAT_MESSAGE, *P_BP_WEBCHAT_MESSAGE;



// This structure holds information relevant to to this chat session
typedef struct __BP_WEBCHAT {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Event Manager This Session Is Hooked Into %%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// event manager
	P_BP_EVENT_MANAGER evm;

	// current running status as reported by the event manager
	BP_WEBCHAT_EVENT_STATUS run_status;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% System Registries %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// system hreg for this tor session
	P_BP_HASH_TABLE_REGISTRY system_hreg;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Session Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Webchat session name, can be looked up via GHT
	char * name;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Event Command Registry %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// command registry
	P_BP_HASH_TABLE_REGISTRY command_reg;


} BP_WEBCHAT, *P_BP_WEBCHAT;


// webchat events which are processed from worker threads
typedef struct __BP_WEBCHAT_EVENT_DATA {

	// webchat event type
	BP_WEBCHAT_EVENT_TYPE event_type;

	// webchat structure to utilize
	P_BP_WEBCHAT webchat;

	// return code for the specified event data
	BP_WEBCHAT_EVENT_RETURN_CODE return_code;

	// webchat event status
	BP_WEBCHAT_EVENT_STATUS status;

	// event return data
	void *webchat_event_return_data;

} BP_WEBCHAT_EVENT_DATA, *P_BP_WEBCHAT_EVENT_DATA;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Web Chat Create/Destroy Session %%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Create a event manager.  It will be automatically added in the ght under the provided session name.
P_BP_WEBCHAT BP_WebChatCreateSession
(
	P_BP_EVENT_MANAGER evm,
	char              *session_name
);

// destroy webchat session
BP_ERROR_T BP_WebChatDestroySession
(
	P_BP_WEBCHAT webchat
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Registry Initializers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// initialize command registry if it hasn't already been initialized
BP_ERROR_T BP_WebChatInitCommandRegistry(P_BP_WEBCHAT webchat);

// initialize basic chat
BP_ERROR_T BP_WebChatInitBasicRegistry(P_BP_WEBCHAT webchat);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Convert Request Commands to Internal Event Types %%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Convert a command to an event type so we can switch() on it easily.
BP_WEBCHAT_EVENT_TYPE BP_WebChatRequestCommandToEventType
(
	P_BP_WEBCHAT webchat,
	char        *cmd,
	size_t       cmd_len
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Event Manager Callbacks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// thread entry point for our application handler.  This must be
// added to whichever event handler you want to hook it into at runtime
// in order for it to work. (BP_EventMgrAddProcessor)
BP_EVENT_PROCESSOR_FUNC( BP_WebChatThreadCallbackEntryPoint );

// Load/register all event processors within a provided event manager.  This
// essentially primes the event recievers so that events can be dispatched.
BP_ERROR_T BP_WebChatRegisterEventProcessorsWithEVM
(
	P_BP_EVENT_MANAGER evm
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% WebChat Event System Integration %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// This will attempt to create an event in the provided event manager.
BP_ERROR_T BP_WebChatCreateEvent
(
	P_BP_EVENT_MANAGER     evm,
	P_BP_WEBCHAT           webchat,
	BP_WEBCHAT_EVENT_TYPE  event_type,
	void *                 event_data,
	size_t                 event_data_size
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Add Users from HTTPd Logins %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Adds a webchat user by using a http user session and some secondary parameters
// relating to user function.
P_BP_WEBCHAT_USER BP_WebChatAddUserFromHttpdUser
(
		P_BP_WEBCHAT                  webchat,
		P_BP_HTTPD_USER               httpd_user,
		char                         *chat_user,
		BP_BOOL                       is_admin,
		BP_WEBCHAT_USER_SOCIAL_STATUS overall_social_status_ranking,
		BP_BOOL                       can_create_channel_group,
		BP_BOOL                       can_create_channel,
		BP_BOOL                       can_create_user_group,
		size_t                        total_messages_sent,
		size_t                        total_messages_recieved,
		size_t                        total_user_approved_messages,
		size_t                        total_user_disapproved_messages,
		BP_BOOL                       can_change_username,
		BP_BOOL                       can_change_password
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Lookup Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// lookup webchat user from httpd user session
P_BP_WEBCHAT_USER BP_WebChatLookupUserFromHTTPdUser
(
		P_BP_WEBCHAT                  webchat,
		P_BP_HTTPD_USER               httpd_user
);

// lookup a webchat user from webchat username
P_BP_WEBCHAT_USER BP_WebChatLookupUserFromChatUsername
(
	P_BP_WEBCHAT  webchat,
	char         *webchat_username
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% WebChat Send Events %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// BP_WEBCHAT_EVENT_USER_ADD_TO_CHANNEL,
// BP_WEBCHAT_EVENT_USER_REMOVE_FROM_CHANNEL,
// BP_WEBCHAT_EVENT_USER_GET_METADATA
// BP_WEBCHAT_EVENT_USER_PICKUP_MESSAGES,
// BP_WEBCHAT_EVENT_CHANNEL_ADD,
// BP_WEBCHAT_EVENT_CHANNEL_REMOVE,
// BP_WEBCHAT_EVENT_CHANNEL_ADD_MESSAGE_FROM_USER


// Adds a user to a channel
BP_ERROR_T BP_WebChatUserAddToChannel
(
	P_BP_WEBCHAT       webchat,
	P_BP_HTTPD_USER    user,
	char              *channel_name
);

// Removes a user from a channel
BP_ERROR_T BP_WebChatUserRemoveFromChannel
(
	P_BP_WEBCHAT       webchat,
	P_BP_HTTPD_USER    user,
	char              *channel_name
);

// Retrieves user metadata.
BP_ERROR_T BP_WebChatUserGetMetadata
(
	P_BP_WEBCHAT       webchat,
	P_BP_HTTPD_USER    user
);



// Pickup dangling messages for all channels for a provided user.
BP_ERROR_T BP_WebChatUserPickupMessages
(
	P_BP_WEBCHAT       webchat,
	P_BP_HTTPD_USER    user
);

// BP_WEBCHAT_EVENT_CHANNEL_ADD,
BP_ERROR_T BP_WebChatChannelAdd
(
	P_BP_WEBCHAT       webchat,
	P_BP_HTTPD_USER    owner,
	char              *channel_name
);

// BP_WEBCHAT_EVENT_CHANNEL_REMOVE,
BP_ERROR_T BP_WebChatChannelRemove
(
	P_BP_WEBCHAT       webchat,
	P_BP_HTTPD_USER    owner,
	char              *channel_name
);

// BP_WEBCHAT_EVENT_CHANNEL_ADD_MESSAGE_FROM_USER
BP_ERROR_T BP_WebChatChannelAddMessageFromUser
(
	P_BP_WEBCHAT       webchat,
	char              *channel_name,
	P_BP_HTTPD_USER    user,
	char              *msg,
	size_t             msg_len
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Webchat Lookup Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to lookup a channel to see if it exists.
P_BP_WEBCHAT_CHANNEL BP_WebChatChannelLookup
(
	P_BP_WEBCHAT webchat,
	char *channel_name
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Webchat Request Processor %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// parameter macro for request processing
#define BP_WEBCHAT_REQUEST_PROCESSOR_PARAMETERS\
		P_BP_WEBCHAT              webchat,\
		P_BP_WEBCHAT_USER         webchat_user,\
		P_BP_HTTPD_REQUEST        req,\
		struct MHD_Connection *   connection,\
		P_BP_HTTPD_USER_SESSION   httpd_user_session


// processor prologue just runs basic checks
#define BP_WEBCHAT_REQUEST_PROCESSOR_PROLOGUE\
	if(!webchat)\
		return ERR_FAILURE;\
	if(!webchat_user)\
		return ERR_FAILURE;\
	if(!req)\
		return ERR_FAILURE;\
	if(!req->json_output)\
		return ERR_FAILURE;\
	if(!connection)\
		return ERR_FAILURE;\
	if(!httpd_user_session)\
		return ERR_FAILURE;

// attempts to process a web chat request by processing req parameters
BP_ERROR_T BP_WebChatProcessRequest
(
		BP_WEBCHAT_REQUEST_PROCESSOR_PARAMETERS
);

// --- Individual Request Processors --------------------------------

// Attempts to process a "user_get_metadata" request from a user.
BP_ERROR_T BP_WebChatProcessRequest_UserGetMetadata
(
		BP_WEBCHAT_REQUEST_PROCESSOR_PARAMETERS
);

// Attempts to process a "user_get_metadata" request from a user.
BP_ERROR_T BP_WebChatProcessRequest_AdminRetrieveSystemHregAsJSON
(
		BP_WEBCHAT_REQUEST_PROCESSOR_PARAMETERS
);




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Webchat Error Responses %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempt to add error to response.  Code or message both
// have to be JSON output reasonable strings (no JSON special characters)
BP_ERROR_T BP_WebChatAddErrorToResponse
(
	P_BP_HTTPD_REQUEST  req,
	char               *error_code,
	char               *error_message
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Webchat Event Pickup (from main application loop) %%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This picks up events in the main loop and processes/destroys them.
BP_ERROR_T BP_WebChatPickupEventsFromMainLoop
(
	P_BP_WEBCHAT webchat
);



#endif /* INCLUDE_WEBCHAT_BP_WEBCHAT_H_ */

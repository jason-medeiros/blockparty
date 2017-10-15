
// include main file
#include "../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Web Chat Create/Destroy Session %%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Create a event manager.  It will be automatically added in the ght under the provided session name.
P_BP_WEBCHAT BP_WebChatCreateSession
(
	P_BP_EVENT_MANAGER evm,
	char              *session_name
)
{


	// ensure we have an event manager
	if(!evm)
		return NULL;

	// ensure we have a session name
	if(!session_name)
		return NULL;


	// create new hash registry
	P_BP_HASH_TABLE_REGISTRY system_hreg = BP_CreateHashTableRegistry();
	if(!system_hreg)
		return NULL;

	// attempt to allocate space for webchat structure
	P_BP_WEBCHAT webchat = (P_BP_WEBCHAT) ht_calloc(system_hreg, sizeof(BP_WEBCHAT), 1);
	if(!webchat)
		return NULL;

	// set system hreg within the newly created object
	webchat->system_hreg = system_hreg;

	// initialize command registry
	if(!BP_WebChatInitCommandRegistry(webchat))
	{

		// destroy webchat session
		BP_WebChatDestroySession(webchat);
		return NULL;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Basic Tables to System Hreg %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set table path
	char *table_path[10];
	BP_ZERO_STACK(table_path);


	// ----

	// set users path
	table_path[0] = "users";

	// add table
	BP_HashRegAddTableByPath(webchat->system_hreg, (char **) &table_path);

	// add by httpd username reference table (for faster lookups)
	table_path[0] = "users";
	table_path[1] = "hashed_by_httpd_username";
	BP_HashRegAddTableByPath(webchat->system_hreg, (char **) &table_path);

	// add by httpd username reference table (for faster lookups)
	table_path[0] = "users";
	table_path[1] = "hashed_by_webchat_username";
	BP_HashRegAddTableByPath(webchat->system_hreg, (char **) &table_path);

	// zero out add path before next
	BP_ZERO_STACK(table_path);


	// ----

	// set classifications path
	table_path[0] = "classifications";

	// add table
	BP_HashRegAddTableByPath(webchat->system_hreg, (char **) &table_path);

	// ----

	// set system path
	table_path[0] = "system";

	// add table
	BP_HashRegAddTableByPath(webchat->system_hreg, (char **) &table_path);

	// ----

	// set channels path
	table_path[0] = "channels";

	// add table
	BP_HashRegAddTableByPath(webchat->system_hreg, (char **) &table_path);

	// ----

	// set references path
	table_path[0] = "references";

	// add table
	BP_HashRegAddTableByPath(webchat->system_hreg, (char **) &table_path);

	// return with the newly created session
	return webchat;

}

// destroy webchat session
BP_ERROR_T BP_WebChatDestroySession
(
	P_BP_WEBCHAT webchat
)
{

	// ensure we have a webchat structure
	if(!webchat)
		return ERR_FAILURE;
	if(!webchat->system_hreg)
		return ERR_FAILURE;

	// destroy the command registry if it's set
	if(webchat->command_reg)
	{
		BP_DestroyHashTableRegistry(webchat->command_reg);
		webchat->command_reg = NULL;
	}

	// destroy all allocated memory in the webchat system (this destroys ALL
	// memory dangling for this webchat)
	BP_DestroyHashTableRegistry(webchat->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Command Registry %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// initialize command registry if it hasn't already been initialized
BP_ERROR_T BP_WebChatInitCommandRegistry(P_BP_WEBCHAT webchat)
{

	// ensure we have a webchat structure
	if(!webchat)
		return ERR_FAILURE;

	// ensure we don't already have a command registry
	if(webchat->command_reg)
		return ERR_FAILURE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Setup Command Registry %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new command registry
	webchat->command_reg = BP_CreateHashTableRegistry();
	if(!webchat->command_reg)
	{
		return ERR_FAILURE;
	}

	// set table path
	char *table_path[10];
	BP_ZERO_STACK(table_path);

	// set table path
	table_path[0] = "commands";

	// add table
	BP_HashRegAddTableByPath(webchat->command_reg, (char **) &table_path);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Commands to Registry %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// add data to table by path
	BP_HashRegAddDataToTableByPath
	(
		webchat->command_reg,
		(char **) &table_path,
		"user_add_to_channel",
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_WEBCHAT_EVENT_TYPE)
	);

	// remove user from channel
	BP_HashRegAddDataToTableByPath
	(
		webchat->command_reg,
		(char **) &table_path,
		"user_remove_from_channel",
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_WEBCHAT_EVENT_TYPE)
	);

	// retrieve user metadata
	BP_HashRegAddDataToTableByPath
	(
		webchat->command_reg,
		(char **) &table_path,
		"user_get_metadata",
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_WEBCHAT_EVENT_TYPE)
	);

	// add a channel to the webchat
	BP_HashRegAddDataToTableByPath
	(
		webchat->command_reg,
		(char **) &table_path,
		"channel_add",
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_WEBCHAT_EVENT_TYPE)
	);

	// add a channel to the webchat
	BP_HashRegAddDataToTableByPath
	(
		webchat->command_reg,
		(char **) &table_path,
		"channel_remove",
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_WEBCHAT_EVENT_TYPE)
	);

	// add a channel to the webchat
	BP_HashRegAddDataToTableByPath
	(
		webchat->command_reg,
		(char **) &table_path,
		"channel_add_message_from_user",
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_WEBCHAT_EVENT_TYPE)
	);

	// get hash table registry
	BP_HashRegAddDataToTableByPath
	(
		webchat->command_reg,
		(char **) &table_path,
		"system_get_hreg",
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_WEBCHAT_EVENT_TYPE)
	);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% lookup each event and set values %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// declare entry lookup
	P_BP_HASH_TABLE_KEYED_ENTRY entry_lookup = NULL;

	// declare value reference
	BP_WEBCHAT_EVENT_TYPE * event_type = NULL;

	// set entry lookup path
	char *entry_lookup_path[10];
	BP_ZERO_STACK(entry_lookup_path);


	// ==== Add User Add To Channel Command =================

	// set the lookup path
	entry_lookup_path[0] = "commands";
	entry_lookup_path[1] = "user_add_to_channel";

	// attempt to lookup table entry by path
	entry_lookup = BP_HashRegLookupTableEntryByPath
	(
		webchat->command_reg,
		(char **) &entry_lookup_path
	);

	// ensure we have a lookup
	if(!entry_lookup)
		return ERR_FAILURE;

	// set the event type
	event_type = (BP_WEBCHAT_EVENT_TYPE *) entry_lookup->data;

	// set the event type
	*event_type = BP_WEBCHAT_EVENT_USER_ADD_TO_CHANNEL;


	// ==== Add User Add To Channel Command =================

	// set the lookup path
	entry_lookup_path[0] = "commands";
	entry_lookup_path[1] = "user_remove_from_channel";

	// attempt to lookup table entry by path
	entry_lookup = BP_HashRegLookupTableEntryByPath
	(
		webchat->command_reg,
		(char **) &entry_lookup_path
	);

	// ensure we have a lookup
	if(!entry_lookup)
		return ERR_FAILURE;

	// set the event type
	event_type = (BP_WEBCHAT_EVENT_TYPE *) entry_lookup->data;

	// set the event type
	*event_type = BP_WEBCHAT_EVENT_USER_REMOVE_FROM_CHANNEL;


	// ==== Retrieve User Metadata Command =================

	// set the lookup path
	entry_lookup_path[0] = "commands";
	entry_lookup_path[1] = "user_get_metadata";

	// attempt to lookup table entry by path
	entry_lookup = BP_HashRegLookupTableEntryByPath
	(
		webchat->command_reg,
		(char **) &entry_lookup_path
	);

	// ensure we have a lookup
	if(!entry_lookup)
		return ERR_FAILURE;

	// set the event type
	event_type = (BP_WEBCHAT_EVENT_TYPE *) entry_lookup->data;

	// set the event type
	*event_type = BP_WEBCHAT_EVENT_USER_GET_METADATA;




	// ==== Channel Add Command =================

	// set the lookup path
	entry_lookup_path[0] = "commands";
	entry_lookup_path[1] = "channel_add";

	// attempt to lookup table entry by path
	entry_lookup = BP_HashRegLookupTableEntryByPath
	(
		webchat->command_reg,
		(char **) &entry_lookup_path
	);

	// ensure we have a lookup
	if(!entry_lookup)
		return ERR_FAILURE;

	// set the event type
	event_type = (BP_WEBCHAT_EVENT_TYPE *) entry_lookup->data;

	// set the event type
	*event_type = BP_WEBCHAT_EVENT_CHANNEL_ADD;


	// ==== Channel Remove Command =================

	// set the lookup path
	entry_lookup_path[0] = "commands";
	entry_lookup_path[1] = "channel_remove";

	// attempt to lookup table entry by path
	entry_lookup = BP_HashRegLookupTableEntryByPath
	(
		webchat->command_reg,
		(char **) &entry_lookup_path
	);

	// ensure we have a lookup
	if(!entry_lookup)
		return ERR_FAILURE;

	// set the event type
	event_type = (BP_WEBCHAT_EVENT_TYPE *) entry_lookup->data;

	// set the event type
	*event_type = BP_WEBCHAT_EVENT_CHANNEL_REMOVE;



	// ==== Channel Add Message From User =================

	// set the lookup path
	entry_lookup_path[0] = "commands";
	entry_lookup_path[1] = "channel_add_message_from_user";

	// attempt to lookup table entry by path
	entry_lookup = BP_HashRegLookupTableEntryByPath
	(
		webchat->command_reg,
		(char **) &entry_lookup_path
	);

	// ensure we have a lookup
	if(!entry_lookup)
		return ERR_FAILURE;

	// set the event type
	event_type = (BP_WEBCHAT_EVENT_TYPE *) entry_lookup->data;

	// set the event type
	*event_type = BP_WEBCHAT_EVENT_CHANNEL_ADD_MESSAGE_FROM_USER;


	// === Get System Hreg as Json ================

	// set the lookup path
	entry_lookup_path[0] = "commands";
	entry_lookup_path[1] = "system_get_hreg";

	// attempt to lookup table entry by path
	entry_lookup = BP_HashRegLookupTableEntryByPath
	(
		webchat->command_reg,
		(char **) &entry_lookup_path
	);

	// ensure we have a lookup
	if(!entry_lookup)
		return ERR_FAILURE;

	// set the event type
	event_type = (BP_WEBCHAT_EVENT_TYPE *) entry_lookup->data;

	// set the event type
	*event_type = BP_WEBCHAT_EVENT_SYSTEM_GET_HREG;


	// return indicating success
	return ERR_SUCCESS;

}


// initialize basic chat
BP_ERROR_T BP_WebChatInitBasicRegistry(P_BP_WEBCHAT webchat)
{

	// ensure we have a webchat structure
	if(!webchat)
		return ERR_FAILURE;


	// set table path
	char *table_path[10];
	BP_ZERO_STACK(table_path);


	// --- add channels table ---

	// set table path to "channels" and add
	table_path[0] = "channels";

	// add table
	BP_HashRegAddTableByPath(webchat->command_reg, (char **) &table_path);

	// add channels metadata
	BP_HashRegAddDataToTableByPath
	(
		webchat->command_reg,
		(char **) &table_path,
		"metadata",
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_WEBCHAT_CHANNELS_METADATA)
	);


	// --- add users table ---

	// set table path to "users" and add
	table_path[0] = "users";

	// add table
	BP_HashRegAddTableByPath(webchat->command_reg, (char **) &table_path);




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Channels Metadata %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Convert Request Commands to Internal Event Types %%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Convert a command to an event type so we can switch() on it easily.
BP_WEBCHAT_EVENT_TYPE BP_WebChatRequestCommandToEventType
(
	P_BP_WEBCHAT webchat,
	char        *cmd,
	size_t       cmd_len
)
{


	// run basic null checks
	if(!webchat)
		return BP_WEBCHAT_EVENT_UNSET;
	if(!webchat->command_reg)
		return BP_WEBCHAT_EVENT_UNSET;
	if(!cmd)
		return BP_WEBCHAT_EVENT_UNSET;
	if(!cmd_len)
		return BP_WEBCHAT_EVENT_UNSET;
	if(!BP_StringIsReasonablePrintableString(cmd, cmd_len, BP_FALSE, BP_FALSE))
		return BP_WEBCHAT_EVENT_UNSET;


	// create entry lookup path
	char *entry_lookup_path[10];
	BP_ZERO_STACK(entry_lookup_path);

	// set the entry lookup path
	entry_lookup_path[0] = "commands";
	entry_lookup_path[1] = cmd;


	// attempt to lookup entry by path
	P_BP_HASH_TABLE_KEYED_ENTRY command_entry = BP_HashRegLookupTableEntryByPath(webchat->command_reg, (char **) &entry_lookup_path);
	if(!command_entry)
	{
		return BP_WEBCHAT_EVENT_UNSET;
	}

	// ensure we have data
	if(!command_entry->data)
	{
		return BP_WEBCHAT_EVENT_UNSET;
	}


	// declare value reference
	BP_WEBCHAT_EVENT_TYPE * event_type_ptr = NULL;
	BP_WEBCHAT_EVENT_TYPE   event_type     = BP_WEBCHAT_EVENT_UNSET;

	// set the event type from data
	event_type_ptr = (BP_WEBCHAT_EVENT_TYPE *) command_entry->data;

	// set the event type
	event_type = *event_type_ptr;

	// return the looked up event type
	return event_type;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Event Manager Callbacks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// thread entry point for our application handler.  This must be
// added to whichever event handler you want to hook it into at runtime
// in order for it to work. (BP_EventMgrAddProcessor)
BP_EVENT_PROCESSOR_FUNC( BP_WebChatThreadCallbackEntryPoint )
{

	// processor entry macro
	BP_EVENT_PROCESSOR_ENTRY;

	// set tor event data
	P_BP_WEBCHAT_EVENT_DATA webchat_event_data = (P_BP_WEBCHAT_EVENT_DATA) event->event_data;
	if(!webchat_event_data)
	{
		BP_EVENT_PROCESSOR_EXIT;
	}

	printf("\n HERE HERE THIS IS THE EVENT DATA POINTER: %p (event) - %p (webchat event)", event, webchat_event_data);

	// set webchat handle
	P_BP_WEBCHAT webchat = webchat_event_data->webchat;
	if(!webchat)
	{

		// ensure we have tor
		printf("\n [!!] Critical: webchat session has not been passed in.");
		BP_EVENT_PROCESSOR_EXIT;

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Process Start Event %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// process start event if set
	if(webchat_event_data->event_type == BP_WEBCHAT_EVENT_USER_GET_METADATA)
	{

		printf("\n Metadata processor event has been picked up ok.");

	}

	// exit event processor
	BP_EVENT_PROCESSOR_EXIT;
}

// Load/register all event processors within a provided event manager.  This
// essentially primes the event recievers so that events can be dispatched.
BP_ERROR_T BP_WebChatRegisterEventProcessorsWithEVM
(
	P_BP_EVENT_MANAGER evm
)
{

	// ensure we have an event manager
	if(!evm)
		return ERR_FAILURE;

	//attempt to lookup the webchat processor
	P_BP_EVENT_PROCESSOR webchat_processor = BP_EventMgrLookupProcessorByName(evm, WEBCHAT_EVENT_PROCESSOR_NAME);
	if(!webchat_processor)
	{

		// attempt to add the processor if it doesn't exist
		BP_EventMgrAddProcessor
		(
				evm,
				WEBCHAT_EVENT_PROCESSOR_NAME,
				"WebChat event processor.",
				BP_EVENT_PROCESSOR_COMPUTE_TARGET_TYPE_CPU_THREAD,
				BP_WebChatThreadCallbackEntryPoint
		);


	}

	// attempt to lookup again after addition attempt
	webchat_processor = BP_EventMgrLookupProcessorByName(evm, WEBCHAT_EVENT_PROCESSOR_NAME);
	if(!webchat_processor)
		return ERR_FAILURE;


	// return indicating success
	return ERR_SUCCESS;

}



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
)
{
	return ERR_SUCCESS;
}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Add Users from HTTPd Logins %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to add a chat user from a httpd user (user enters chat)
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
)
{

	// basic null checks
	if(!webchat)
		return NULL;
	if(!httpd_user)
		return NULL;
	if(!chat_user)
		return NULL;
	if(!webchat->system_hreg)
		return NULL;


	// basic reasonable checks on new chat username
	if(!BP_StringIsReasonablePrintableString(chat_user, bpstrlen(chat_user), BP_FALSE, BP_FALSE))
		return NULL;

	// basic reasonable checks on httpd username
	if(!BP_StringIsReasonablePrintableString(httpd_user->username, bpstrlen(httpd_user->username), BP_FALSE, BP_FALSE))
		return NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt Lookup Checks %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// exit if we already have a user
	if(BP_WebChatLookupUserFromHTTPdUser(webchat, httpd_user))
		return NULL;
	if(BP_WebChatLookupUserFromChatUsername(webchat, chat_user))
		return NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add New User and Specify Initial Settings %%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new webchat user
	P_BP_WEBCHAT_USER new_webchat_user = (P_BP_WEBCHAT_USER) ht_calloc(webchat->system_hreg, sizeof(BP_WEBCHAT_USER), 1);
	if(!new_webchat_user)
		return NULL;

	// set settings for new user from webchat
	new_webchat_user->httpd_user                      = httpd_user;
	new_webchat_user->chat_user                       = ht_strndup(webchat->system_hreg, chat_user, bpstrlen(chat_user));
	new_webchat_user->is_admin                        = is_admin;
	new_webchat_user->overall_social_status_ranking   = overall_social_status_ranking;
	new_webchat_user->can_create_channel_group        = can_create_channel_group;
	new_webchat_user->can_create_channel              = can_create_channel;
	new_webchat_user->can_create_user_group           = can_create_user_group;
	new_webchat_user->total_messages_sent             = total_messages_sent;
	new_webchat_user->total_messages_recieved         = total_messages_recieved;
	new_webchat_user->total_user_approved_messages    = total_user_approved_messages;
	new_webchat_user->total_user_disapproved_messages = total_user_disapproved_messages;
	new_webchat_user->can_change_username             = can_change_username;
	new_webchat_user->can_change_password             = can_change_password;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Table Fast Lookup References %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create add path
	char *table_add_path[10];
	BP_ZERO_STACK(table_add_path);

	// create search path
	char *table_search_path[10];
	BP_ZERO_STACK(table_search_path);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Hash By HTTP Username %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the table add path
	table_add_path[0] = "users";
	table_add_path[1] = "hashed_by_httpd_username";

	// add data from username
	BP_HashRegAddDataToTableByPath
	(
			webchat->system_hreg,
			(char **) &table_add_path,
			httpd_user->username,
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
			NULL,
			sizeof(new_webchat_user)
	);

	// search to see if httpd user already exists
	table_search_path[0] = "users";
	table_search_path[1] = "hashed_by_httpd_username";
	table_search_path[2] = httpd_user->username;

	// attempt to lookup entry in the table
	P_BP_HASH_TABLE_KEYED_ENTRY user_lookup = BP_HashRegLookupTableEntryByPath
	(
		webchat->system_hreg,
		(char **) &table_search_path
	);

	// set pointer reference
	P_BP_WEBCHAT_USER * user_ptr = (P_BP_WEBCHAT_USER *) user_lookup->data;

	// copy pointer into new data
	*user_ptr = new_webchat_user;

	// user lookup
	P_BP_WEBCHAT_USER httpd_user_lookup = BP_WebChatLookupUserFromHTTPdUser(webchat, httpd_user);
	if(!httpd_user_lookup)
		return NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Reference by Username %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	table_add_path[0] = "users";
	table_add_path[1] = "hashed_by_webchat_username";

	// add data from username
	BP_HashRegAddDataToTableByPath
	(
			webchat->system_hreg,
			(char **) &table_add_path,
			chat_user,
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
			NULL,
			sizeof(new_webchat_user)
	);

	// search to see if httpd user already exists
	table_search_path[0] = "users";
	table_search_path[1] = "hashed_by_webchat_username";
	table_search_path[2] = chat_user;

	// attempt to lookup entry in the table
	P_BP_HASH_TABLE_KEYED_ENTRY webchat_lookup = BP_HashRegLookupTableEntryByPath
	(
		webchat->system_hreg,
		(char **) &table_search_path
	);

	// set pointer reference
	P_BP_WEBCHAT_USER * webchat_ptr = (P_BP_WEBCHAT_USER *) webchat_lookup->data;

	// copy pointer into new data
	*webchat_ptr = new_webchat_user;

	// lookup user from chat username
	P_BP_WEBCHAT_USER webchat_user_lookup = BP_WebChatLookupUserFromChatUsername(webchat,chat_user);
	if(!webchat_user_lookup)
		return NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	printf("\n ADDING USERS HERE WOOP: %p - %p", httpd_user_lookup, webchat_user_lookup);

	// return the new webchat user
	return new_webchat_user;


}


// lookup from httpd user
P_BP_WEBCHAT_USER BP_WebChatLookupUserFromHTTPdUser
(
		P_BP_WEBCHAT                  webchat,
		P_BP_HTTPD_USER               httpd_user
)
{

	// ensure we have a webchat
	if(!webchat)
		return NULL;

	// ensure we have a httpd user
	if(!httpd_user)
		return NULL;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Search Logic %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create search path
	char *table_search_path[10];
	BP_ZERO_STACK(table_search_path);

	// search to see if httpd user already exists
	table_search_path[0] = "users";
	table_search_path[1] = "hashed_by_httpd_username";
	table_search_path[2] = httpd_user->username;

	// attempt to lookup entry in the table
	P_BP_HASH_TABLE_KEYED_ENTRY user_lookup = BP_HashRegLookupTableEntryByPath
	(
		webchat->system_hreg,
		(char **) &table_search_path
	);

	// check for user lookup
	if(!user_lookup)
		return NULL;

	// since its a pointer to poitner reference, we have to create and deref it before returning
	P_BP_WEBCHAT_USER * lookup_data = (P_BP_WEBCHAT_USER *) user_lookup->data;

	// ensure we have lookup data
	if(!lookup_data)
		return NULL;

	// return the user lookup (or null if it can't be found)
	return *lookup_data;

}

// lookup from webchat username
P_BP_WEBCHAT_USER BP_WebChatLookupUserFromChatUsername
(
	P_BP_WEBCHAT  webchat,
	char         *webchat_username
)
{

	// ensure we have a webchat
	if(!webchat)
		return NULL;

	// ensure we have a webchat user
	if(!webchat_username)
		return NULL;

	// ensure username is reasonable printable
	if(!BP_StringIsReasonablePrintableString(webchat_username, bpstrlen(webchat_username), BP_FALSE, BP_FALSE))
		return NULL;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Search Logic %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create search path
	char *table_search_path[10];
	BP_ZERO_STACK(table_search_path);

	// search to see if webchat user already exists
	table_search_path[0] = "users";
	table_search_path[1] = "hashed_by_webchat_username";
	table_search_path[2] = webchat_username;

	// attempt to lookup entry in the table
	P_BP_HASH_TABLE_KEYED_ENTRY user_lookup = BP_HashRegLookupTableEntryByPath
	(
		webchat->system_hreg,
		(char **) &table_search_path
	);

	// check for user lookup
	if(!user_lookup)
		return NULL;

	// return the user lookup (or null if it can't be found)
	return (P_BP_WEBCHAT_USER) user_lookup->data;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% WebChat Send Events %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



// Adds a user to a channel
BP_ERROR_T BP_WebChatUserAddToChannel
(
	P_BP_WEBCHAT       webchat,
	P_BP_HTTPD_USER    user,
	char              *channel_name
)
{


	// run basic null checks
	if(!webchat)
		return ERR_FAILURE;
	if(!user)
		return ERR_FAILURE;
	if(!channel_name)
		return ERR_FAILURE;






	// return indicating success
	return ERR_SUCCESS;

}

// Removes a user from a channel
BP_ERROR_T BP_WebChatUserRemoveFromChannel
(
	P_BP_WEBCHAT       webchat,
	P_BP_HTTPD_USER    user,
	char              *channel_name
)
{
	return ERR_SUCCESS;
}


// Pickup dangling messages for all channels for a provided user.
BP_ERROR_T BP_WebChatUserPickupMessages
(
	P_BP_WEBCHAT       webchat,
	P_BP_HTTPD_USER    user
)
{
	return ERR_SUCCESS;
}

// BP_WEBCHAT_EVENT_CHANNEL_ADD,
BP_ERROR_T BP_WebChatChannelAdd
(
	P_BP_WEBCHAT       webchat,
	P_BP_HTTPD_USER    owner,
	char              *channel_name
)
{


	// run basic null checks
	if(!webchat)
		return ERR_FAILURE;
	if(!owner)
		return ERR_FAILURE;
	if(!channel_name)
		return ERR_FAILURE;

	// ensure string is reasonable printable
	if(!BP_StringIsReasonablePrintableString(channel_name, bpstrlen(channel_name), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;


	// --- add channels table ---

	// create new table path
	char * table_path[10];
	BP_ZERO_STACK(table_path);

	// set table path to "channels" and add
	table_path[0] = "channels";
	table_path[1] = channel_name;


	// This is where we add a new channel.  The steps are as follows:
	//
	// 1) Check if channel already exists.  If it does, we cannot add it.  Exit.
	//
	// 2) Add channel to registry by name.
	//
	// 3) Create initial webchat channel structure.
	//
	// 4) Create initial webchat channel user structure.
	//
	// 5) Add a new table entry with channel name as key.
	//
	// 6) Add webchat channel structure to channel.
	//


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check if Channel Already Exists %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	/*
	// add table
	BP_HashRegAddTableByPath(webchat->command_reg, (char **) &table_path);

	// add channels metadata
	BP_HashRegAddDataToTableByPath
	(
		webchat->command_reg,
		(char **) &table_path,
		"metadata",
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_WEBCHAT_CHANNELS_METADATA)
	);

	*/

	// return indicating success
	return ERR_SUCCESS;

}



// BP_WEBCHAT_EVENT_CHANNEL_REMOVE,
BP_ERROR_T BP_WebChatChannelRemove
(
	P_BP_WEBCHAT       webchat,
	P_BP_HTTPD_USER    owner,
	char              *channel_name
)
{
	return ERR_SUCCESS;
}

// BP_WEBCHAT_EVENT_CHANNEL_ADD_MESSAGE_FROM_USER
BP_ERROR_T BP_WebChatChannelAddMessageFromUser
(
	P_BP_WEBCHAT       webchat,
	char              *channel_name,
	P_BP_HTTPD_USER    user,
	char              *msg,
	size_t             msg_len
)
{


	// P_BP_HTTPD_USER    owner,

	return ERR_SUCCESS;
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Webchat Lookup Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to lookup a channel to see if it exists.
P_BP_WEBCHAT_CHANNEL BP_WebChatChannelLookup
(
	P_BP_WEBCHAT webchat,
	char *channel_name
)
{


	// ensure we have a webchat
	if(!webchat)
		return NULL;

	// ensure the webchat has a system hash table registry
	if(!webchat->system_hreg)
		return NULL;



	// return NULL if we can't look things up ok
	return NULL;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Webchat Event Pickup (from main application loop) %%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This picks up events in the main loop and processes/destroys them.
BP_ERROR_T BP_WebChatPickupEventsFromMainLoop
(
	P_BP_WEBCHAT webchat
)
{
	return ERR_SUCCESS;
}

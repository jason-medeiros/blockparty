
// include main file
#include "../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Webchat Request Processor %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempts to process a web chat request by processing req parameters
BP_ERROR_T BP_WebChatProcessRequest
(
		BP_WEBCHAT_REQUEST_PROCESSOR_PARAMETERS
)
{

	// run request processor prologue
	BP_WEBCHAT_REQUEST_PROCESSOR_PROLOGUE;

	printf("\n got webchat user: %p - %s", webchat_user, webchat_user->chat_user);
	printf("\n");

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt To Process Request %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// get command variable
	P_BP_HTTPD_REQUEST_VAR command_var = BP_HTTPdRequestVarArgRegistryLookup(req, BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA, (char *) "cmd");
	if(!command_var)
	{

		// exit with no-command message if we have no command provided
		return BP_HTTPdRequestHandlerRespondJSONFromStaticText(req, ERR_FAILURE, "{\n\t\"command_msg\": \"no_command\"\n}");

	}

	// ensure we have data and size
	if(!command_var->data || !command_var->data_size)
	{
		return BP_HTTPdRequestHandlerRespondJSONFromStaticText(req, ERR_FAILURE, "{\n\t\"command_msg\": \"no_command_data\"\n}");
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Perform Parameter Filtering %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// TODO: Parameter filtering before values are used.  Will depend
	//       on design specifics.


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Resolve Event Type %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set event type
	BP_WEBCHAT_EVENT_TYPE event_type = BP_WebChatRequestCommandToEventType
	(
		webchat,
		command_var->data,
		command_var->data_size
	);

	// set bad command type
	if(!event_type)
	{
		return BP_HTTPdRequestHandlerRespondJSONFromStaticText(req, ERR_FAILURE, "{\n\t\"command_msg\": \"bad_command\"\n}");
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Declare and Attempt to Extract Incomming Parameters %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// extract vars first
	P_BP_HTTPD_REQUEST_VAR channel_var = __POST(req,  "channel");
	P_BP_HTTPD_REQUEST_VAR msg_var     = __POST(req,  "msg");
	P_BP_HTTPD_REQUEST_VAR user_var    = __POST(req,  "user");

	// variable used specifically for hregs
	P_BP_HTTPD_REQUEST_VAR hreg_var    = __POST(req,  "hreg");



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Process Event Requests By Type %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// event dispatch status when sending event
	BP_ERROR_T event_dispatch_status = ERR_FAILURE;

	// switch on the event type
	switch(event_type)
	{


		// send add user to channel event
		case BP_WEBCHAT_EVENT_USER_ADD_TO_CHANNEL:
			if(channel_var)
			if(channel_var->data)
			{

				// send event to add user to a channel
				event_dispatch_status = BP_WebChatUserAddToChannel
				(
					webchat,
					httpd_user_session->user,
					(char *) channel_var->data
				);

			}
			break;

		// send remove user from channel event
		case BP_WEBCHAT_EVENT_USER_REMOVE_FROM_CHANNEL:
			if(channel_var)
			if(channel_var->data)
			{

					// send event to add user to a channel
					event_dispatch_status = BP_WebChatUserRemoveFromChannel
					(
							webchat,
							httpd_user_session->user,
							(char *) channel_var->data
					);

			}
			break;


		// attempt to get metadata
		case BP_WEBCHAT_EVENT_USER_GET_METADATA:

			// attempt to process request
			if(!BP_WebChatProcessRequest_UserGetMetadata(webchat, webchat_user, req, connection, httpd_user_session))
			{
				return ERR_FAILURE;
			}

			// dbg: test always failure
			// return ERR_FAILURE;

			break;

		// send add channel event
		case BP_WEBCHAT_EVENT_CHANNEL_ADD:
			if(channel_var)
			if(channel_var->data)
			{

					// attempt to add chat channel
					event_dispatch_status = BP_WebChatChannelAdd
					(
							webchat,
							httpd_user_session->user,
							channel_var->data
					);

			}
			break;

		// send remove channel event
		case BP_WEBCHAT_EVENT_CHANNEL_REMOVE:
			if(channel_var)
			if(channel_var->data)
			{

					// attempt to remove web chat channel
					event_dispatch_status = BP_WebChatChannelRemove
					(
							webchat,
							httpd_user_session->user,
							channel_var->data
					);

			}
			break;

		// add a message from a user
		case BP_WEBCHAT_EVENT_CHANNEL_ADD_MESSAGE_FROM_USER:
			if(channel_var)
			if(channel_var->data)
			{

				// attempt to process request
				if(!BP_WebChatProcessRequest_UserGetMetadata(webchat, webchat_user, req, connection, httpd_user_session))
				{
					return ERR_FAILURE;
				}


			}
			break;


		// attempt to retrieve a system hash registry based on request
		case BP_WEBCHAT_EVENT_SYSTEM_GET_HREG:
			if(hreg_var)
			if(hreg_var->data)
			{

					// attempt to add message from user
					event_dispatch_status = BP_WebChatProcessRequest_AdminRetrieveSystemHregAsJSON
					(
							webchat,
							webchat_user,
							req,
							connection,
							httpd_user_session
					);

			}
			break;

		// break on default
		default:
			break;

	}


	// return indicating success
	return ERR_SUCCESS;

}


// Attempts to process a "user_get_metadata" request from a user.
BP_ERROR_T BP_WebChatProcessRequest_UserGetMetadata
(
		BP_WEBCHAT_REQUEST_PROCESSOR_PARAMETERS
)
{

	// Run request processor prologue, checks parameters (including the JSON
	// output) to ensure they exist.
	BP_WEBCHAT_REQUEST_PROCESSOR_PROLOGUE;


    // create add path
    char *user_metadata_add_path[10];
    BP_ZERO_STACK(user_metadata_add_path);

    // set initial branch path
    user_metadata_add_path[0] = "webchat_user_metadata";

    // add table if it doesn't already exist
    BP_JSONAddNodeByPath(req->json_output, (char **) &user_metadata_add_path, NULL, 0, BP_FALSE);



    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Add Chat User %%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // set the add path key
    user_metadata_add_path[0] = "webchat_user_metadata";
    user_metadata_add_path[1] = "chat_user";

    // add username directly
    BP_JSONAddNodeByPath
	(
			req->json_output,
			(char **) &user_metadata_add_path,
			webchat_user->chat_user,
			bpstrlen(webchat_user->chat_user),
			BP_FALSE
	);


    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Add Is_Admin %%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // set the add path key
    user_metadata_add_path[0] = "webchat_user_metadata";
    user_metadata_add_path[1] = "is_admin";

    // set is_admin string
    char *is_admin =  webchat_user->is_admin ? (char *) "yes" : (char *) "no";

    // add username directly
    BP_JSONAddNodeByPath
	(
			req->json_output,
			(char **) &user_metadata_add_path,
			is_admin,
			bpstrlen(is_admin),
			BP_FALSE
	);


    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Add Social Status %%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the add path key
	user_metadata_add_path[0] = "webchat_user_metadata";
	user_metadata_add_path[1] = "overall_social_status_ranking";

	// create buffer to hold string
	char social_status_ranking[256];
	BP_ZERO_STACK(social_status_ranking);

	// set the social status ranking
	snprintf
	(
			(char *) &social_status_ranking,
			sizeof(social_status_ranking)-1,
			"%u",
			webchat_user->overall_social_status_ranking
	);

	// add username directly
	BP_JSONAddNodeByPath
	(
		req->json_output,
		(char **) &user_metadata_add_path,
		social_status_ranking,
		bpstrlen(social_status_ranking),
		BP_FALSE
	);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Creation Privileges %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// --- can_create_channel_group --------------

	// set the add path key
	user_metadata_add_path[0] = "webchat_user_metadata";
	user_metadata_add_path[1] = "can_create_channel_group";

	// set is_admin string
    char *can_create_channel_group =  webchat_user->can_create_channel_group ? (char *) "yes" : (char *) "no";

	// add username directly
	BP_JSONAddNodeByPath
	(
		req->json_output,
		(char **) &user_metadata_add_path,
		can_create_channel_group,
		bpstrlen(can_create_channel_group),
		BP_FALSE
	);


	// --- can_create_channel --------------

	// set the add path key
	user_metadata_add_path[0] = "webchat_user_metadata";
	user_metadata_add_path[1] = "can_create_channel";

	// set is_admin string
    char *can_create_channel =  webchat_user->can_create_channel ? (char *) "yes" : (char *) "no";

	// add username directly
	BP_JSONAddNodeByPath
	(
		req->json_output,
		(char **) &user_metadata_add_path,
		can_create_channel,
		bpstrlen(can_create_channel),
		BP_FALSE
	);

	// --- can_create_channel --------------

	// set the add path key
	user_metadata_add_path[0] = "webchat_user_metadata";
	user_metadata_add_path[1] = "can_create_user_group";

	// set is_admin string
	char *can_create_user_group =  webchat_user->can_create_user_group ? (char *) "yes" : (char *) "no";

	// add username directly
	BP_JSONAddNodeByPath
	(
		req->json_output,
		(char **) &user_metadata_add_path,
		can_create_user_group,
		bpstrlen(can_create_user_group),
		BP_FALSE
	);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Messages Sent and Recieved %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// -- total_messages_sent -----------

	// set the add path key
	user_metadata_add_path[0] = "webchat_user_metadata";
	user_metadata_add_path[1] = "total_messages_sent";

	// create buffer to hold string
	char total_messages_sent[256];
	BP_ZERO_STACK(total_messages_sent);

	// set total_messages_sent
	snprintf
	(
			(char *) &total_messages_sent,
			sizeof(total_messages_sent)-1,
			"%u",
			webchat_user->total_messages_sent
	);

	// add total_messages_sent
	BP_JSONAddNodeByPath
	(
		req->json_output,
		(char **) &user_metadata_add_path,
		total_messages_sent,
		bpstrlen(total_messages_sent),
		BP_FALSE
	);


	// -- total_messages_recieved -----------

	// set the add path key
	user_metadata_add_path[0] = "webchat_user_metadata";
	user_metadata_add_path[1] = "total_messages_recieved";

	// create buffer to hold string
	char total_messages_recieved[256];
	BP_ZERO_STACK(total_messages_recieved);

	// set total_messages_sent
	snprintf
	(
			(char *) &total_messages_recieved,
			sizeof(total_messages_recieved)-1,
			"%u",
			webchat_user->total_messages_recieved
	);

	// add total_messages_recieved
	BP_JSONAddNodeByPath
	(
		req->json_output,
		(char **) &user_metadata_add_path,
		total_messages_recieved,
		bpstrlen(total_messages_recieved),
		BP_FALSE
	);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Total Message Social Counts %%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// -- total_user_approved_messages -----------

	// set the add path key
	user_metadata_add_path[0] = "webchat_user_metadata";
	user_metadata_add_path[1] = "total_user_approved_messages";

	// create buffer to hold string
	char total_user_approved_messages[256];
	BP_ZERO_STACK(total_user_approved_messages);

	// set total_messages_sent
	snprintf
	(
			(char *) &total_user_approved_messages,
			sizeof(total_user_approved_messages)-1,
			"%u",
			webchat_user->total_user_approved_messages
	);

	// add total_messages_sent
	BP_JSONAddNodeByPath
	(
		req->json_output,
		(char **) &user_metadata_add_path,
		total_user_approved_messages,
		bpstrlen(total_user_approved_messages),
		BP_FALSE
	);


	// -- total_user_approved_messages -----------

	// set the add path key
	user_metadata_add_path[0] = "webchat_user_metadata";
	user_metadata_add_path[1] = "total_user_disapproved_messages";

	// create buffer to hold string
	char total_user_disapproved_messages[256];
	BP_ZERO_STACK(total_user_disapproved_messages);

	// set total_messages_sent
	snprintf
	(
			(char *) &total_user_disapproved_messages,
			sizeof(total_user_disapproved_messages)-1,
			"%u",
			webchat_user->total_user_disapproved_messages
	);

	// add total_messages_sent
	BP_JSONAddNodeByPath
	(
		req->json_output,
		(char **) &user_metadata_add_path,
		total_user_disapproved_messages,
		bpstrlen(total_user_disapproved_messages),
		BP_FALSE
	);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% User Change Privs %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// --- can_change_username --------------

	// set the add path key
	user_metadata_add_path[0] = "webchat_user_metadata";
	user_metadata_add_path[1] = "can_change_username";

	// set is_admin string
	char *can_change_username =  webchat_user->can_change_username ? (char *) "yes" : (char *) "no";

	// add username directly
	BP_JSONAddNodeByPath
	(
		req->json_output,
		(char **) &user_metadata_add_path,
		can_change_username,
		bpstrlen(can_change_username),
		BP_FALSE
	);


	// --- can_change_password --------------

	// set the add path key
	user_metadata_add_path[0] = "webchat_user_metadata";
	user_metadata_add_path[1] = "can_change_password";

	// set is_admin string
	char *can_change_password =  webchat_user->can_change_password ? (char *) "yes" : (char *) "no";

	// add username directly
	BP_JSONAddNodeByPath
	(
		req->json_output,
		(char **) &user_metadata_add_path,
		can_change_password,
		bpstrlen(can_change_password),
		BP_FALSE
	);


	// return indicating success
	return ERR_SUCCESS;

}



// Attempts to retrieve a hash table registry as JSON and return it to the user.
BP_ERROR_T BP_WebChatProcessRequest_AdminRetrieveSystemHregAsJSON
(
		BP_WEBCHAT_REQUEST_PROCESSOR_PARAMETERS
)
{

	// Run request processor prologue, checks parameters (including the JSON
	// output) to ensure they exist.
	BP_WEBCHAT_REQUEST_PROCESSOR_PROLOGUE;

	// retrieve the hareg name
	P_BP_HTTPD_REQUEST_VAR hreg_name    = __POST(req,  "hreg");

	// ensure we have a name registry
	if(!hreg_name)
		return ERR_FAILURE;
	if(!hreg_name->data)
		return ERR_FAILURE;

	// ensure data is reasonable/printable
	if(!BP_StringIsReasonablePrintableString((char *) hreg_name->data, hreg_name->data_size, BP_FALSE, BP_FALSE))
		return ERR_FAILURE;


	// "global_hash_table_registry" request
	if(BP_StringsMatchAsReasonableStrings(hreg_name->data, "global_hash_table_registry"))
	{

		// attempt to append registry
		BP_JSONAppendHReg
		(
				req->json_output,
				"global_hash_table_registry",
				&global_hash_table_registry,
				BP_TRUE,
				BP_TRUE
		);

		// return after appending registry
		return ERR_SUCCESS;
	}


	// "webchat_command_registry" request
	if(BP_StringsMatchAsReasonableStrings(hreg_name->data, "webchat_command_registry"))
	{

		// attempt to append registry
		BP_JSONAppendHReg
		(
				req->json_output,
				"webchat_command_registry",
				webchat->command_reg,
				BP_TRUE,
				BP_TRUE
		);

		// return after appending registry
		return ERR_SUCCESS;
	}

	// "webchat_system_registry" request
	if(BP_StringsMatchAsReasonableStrings(hreg_name->data, "webchat_system_registry"))
	{

		// attempt to append registry
		BP_JSONAppendHReg
		(
				req->json_output,
				"webchat_system_registry",
				webchat->system_hreg,
				BP_TRUE,
				BP_TRUE
		);


		// return after appending registry
		return ERR_SUCCESS;

	}

	// ------------

	// "all registries" request
	if(BP_StringsMatchAsReasonableStrings(hreg_name->data, "all_registries"))
	{

		// attempt to append registry
		BP_JSONAppendHReg
		(
				req->json_output,
				"webchat_system_registry",
				webchat->system_hreg,
				BP_TRUE,
				BP_TRUE
		);

		// attempt to append registry
		BP_JSONAppendHReg
		(
				req->json_output,
				"webchat_command_registry",
				webchat->command_reg,
				BP_TRUE,
				BP_TRUE
		);

		// attempt to append registry
		BP_JSONAppendHReg
		(
				req->json_output,
				"global_hash_table_registry",
				&global_hash_table_registry,
				BP_TRUE,
				BP_TRUE
		);

		// return after appending registry
		return ERR_SUCCESS;

	}


	// return indicating success
	return ERR_SUCCESS;

}




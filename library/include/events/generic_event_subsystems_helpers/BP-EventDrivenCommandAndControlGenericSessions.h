#ifndef __BP_EVENT_DRIVEN_COMMAND_AND_CONTROL_GENERIC_SESSIONS_H
#define __BP_EVENT_DRIVEN_COMMAND_AND_CONTROL_GENERIC_SESSIONS_H


// This structure is used to process generic commands.
typedef struct __BP_EDCNC_GENERIC_SESSION {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Event Manager This Session Is Hooked Into %%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// event manager
	P_BP_EVENT_MANAGER evm;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% JSON Configuration Data %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// configuration information
	P_BP_JSON parsed_config;

	// initial configuration buffer
	char *initial_config_json_buff;

	// initial configuration buffer size
	size_t initial_config_json_buff_size;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Generic Settings %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if this session is interactive, we must indicate here
	BP_BOOL is_interactive_session;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Event Processor for Application %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// event processor callback to handle our data events
	void * (*event_processor)(void *event_data);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% System Registries %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// system hreg for this session
	P_BP_HASH_TABLE_REGISTRY system_hreg;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Service Run Information %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// start time
	timeval service_start_time;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Command / Control Session %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// flag indicating if the command and control session has started ok
	BP_BOOL cc_started_ok;
	BP_BOOL cc_exited_ok;

	// file descriptor set used by the cc session to manipulate the
	P_BP_FDSET tor_app;

	// command and control session for manipulating tor
	P_BP_CC_SESSION cc_session;


} BP_EDCNC_GENERIC_SESSION, *P_BP_EDCNC_GENERIC_SESSION;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create and Destroy EDCNC Sessions %%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// create event driven command and control session
P_BP_EDCNC_GENERIC_SESSION BP_EdcncCreateSession
(
	P_BP_EVENT_MANAGER evm,
	char *session_creation_json
);

// Destroy generic command and control sessions.
BP_ERROR_T BP_EdcncDestroySession(P_BP_EDCNC_GENERIC_SESSION edcnc_session);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Add Command Trees from JSON %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




#endif /* __BP_EVENT_DRIVEN_COMMAND_AND_CONTROL_GENERIC_SESSIONS_H* */

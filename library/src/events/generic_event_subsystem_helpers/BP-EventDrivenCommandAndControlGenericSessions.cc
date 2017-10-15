// include blockparty main
#include "../../include/BP-Main.h"


// %%%%%%%%%%%
// TOMORROW MORNING RETURN HERE:
// --
// You need to begin writing the generic command and control event driven interface.
// --
// Step 1: Parse Defined Json from Whiteboard
// Step 2: Determine application settings from parse.
// Step 3: Review tor code and figure out how to load application.



// Create a generic event driven CNC session based on provided json, linking it into the
// provided event manager.
P_BP_EDCNC_GENERIC_SESSION BP_EdcncCreateSession
(
	P_BP_EVENT_MANAGER evm,
	char *session_creation_json
)
{

	// basic null checks
	if(!evm)
		return NULL;
	if(!session_creation_json)
		return NULL;

	// run basic check to ensure no garbage characters exist in our buffer
	if(!BP_StringIsReasonablePrintableString(session_creation_json, bpstrlen(session_creation_json), BP_TRUE, BP_TRUE))
		return NULL;

	// create new json session to parse from
	P_BP_JSON parsed_session_creation_json = BP_JSONCreate();

	// if we don't have a parsed session creation json, exit null
	if(!parsed_session_creation_json)
		return NULL;

	/*
	// example command and control buffer for parsing
	char * basic_generic_command_and_control_initial_json_structure = ""\
	"\n  {"\
	"\n  	'type': 'app_command_and_control',   "\
	"\n  	'application_path': '',              "\
	"\n  	'envp':                              "\
	"\n  	{                                    "\
	"\n  	},                                   "\
	"\n  	'argv':                              "\
	"\n  	{                                    "\
	"\n  	},                                   "\
	"\n  	'is_interactive': 'yes',             "\
	"\n  	'application_event_status':          "\
	"\n  	{                                    "\
	"\n  		'val1': 'blah'                   "\
	"\n  	}                                    "\
	"\n  }";
	*/


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Extract Data from JSON %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// entry lookup into json table
	P_BP_HTKE config_entry_lookup = NULL;

	// perform json lookups
	P_BP_HTKE type_lookup              = BP_JSONLookupEntryByPathQuery(parsed_session_creation_json, "/type");
	P_BP_HTKE app_path_lookup          = BP_JSONLookupEntryByPathQuery(parsed_session_creation_json, "/application_path");
	P_BP_HTKE envp_lookup              = BP_JSONLookupEntryByPathQuery(parsed_session_creation_json, "/envp");
	P_BP_HTKE argv_lookup              = BP_JSONLookupEntryByPathQuery(parsed_session_creation_json, "/argv");
	P_BP_HTKE is_interactive_lookup    = BP_JSONLookupEntryByPathQuery(parsed_session_creation_json, "/is_interactive");
	P_BP_HTKE event_status_lookup      = BP_JSONLookupEntryByPathQuery(parsed_session_creation_json, "/application_event_status");




	// ensure all initial lookups were successful
	if
	(
		!type_lookup           ||
		!app_path_lookup       ||
		!envp_lookup           ||
		!argv_lookup           ||
		!is_interactive_lookup ||
		!event_status_lookup
	)
	{
		return NULL;
	}

	// run basic type checks
	if
	(
		type_lookup->type           != BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING      ||
		app_path_lookup->type       != BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING      ||
		is_interactive_lookup->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING      ||
		event_status_lookup->type   != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE ||
		envp_lookup->type           != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE ||
		argv_lookup->type           != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE
	)
	{
		return NULL;
	}

	// --- check for type marker -------------

	// ensure that the type lookup matches our marker (defined in BP-Main.h)
	if(!ht_memmatch_entry(type_lookup, BP_EDCNC_GENERIC_SESSION_MARKER, bpstrlen(BP_EDCNC_GENERIC_SESSION_MARKER)))
	{
		return NULL;
	}


	// --- run additional data checks -------

	// gather app path
	char * app_path  = (char *) app_path_lookup->data;

	// ensure path exists at provided position
	if(!BP_FileAtPathExistsAndIsExecutable(app_path))
	{
		return NULL;
	}

	// ensure app is interactive
	BP_BOOL is_interactive = BP_FALSE;

	// ensure that the type lookup matches our marker
	if(ht_memmatch_entry(is_interactive_lookup, "yes", bpstrlen("yes")))
		is_interactive = BP_TRUE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Session %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create system hreg
	P_BP_HASH_TABLE_REGISTRY system_hreg = BP_CreateHashTableRegistry();

	// ensure we have a system hreg
	if(!system_hreg)
		return NULL;

	// attempt to create session within hreg
	P_BP_EDCNC_GENERIC_SESSION edcnc_generic_session = (P_BP_EDCNC_GENERIC_SESSION) ht_calloc(system_hreg, sizeof(BP_EDCNC_GENERIC_SESSION), 1);

	// ensure we have created our sesssion
	if(!edcnc_generic_session)
	{
		BP_DestroyHashTableRegistry(system_hreg);
		return NULL;
	}

	// mark session as interactive
	edcnc_generic_session->is_interactive_session = is_interactive;




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Envp and Argv Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// check if our envp lookup is a table
	if(!ht_entry_is_table(envp_lookup))
	{
		BP_EdcncDestroySession(edcnc_generic_session);
		return NULL;
	}

	// check if our argv lookup is a table
	if(!ht_entry_is_table(argv_lookup))
	{
		BP_EdcncDestroySession(edcnc_generic_session);
		return NULL;
	}

	// -- iterate through envp and create ---

	// iterate through envp hashes
	BP_HASH_ITER_OPEN(envp_lookup, curr_envp_lookup)
	{

		// curr_envp_lookup->key;
		// curr_envp_lookup->data;
		printf("\n BLAH");

	}
	BP_HASH_ITER_CLOSE;


	// -- iterate through argv and create ---

	// iterate through envp hashes
	BP_HASH_ITER_OPEN(argv_lookup, curr_argv_lookup)
	{


		// curr_envp_lookup->key;
		// curr_envp_lookup->data;
		printf("\n BLAH");

	}
	BP_HASH_ITER_CLOSE;


	// size_t  envp_element_count = 0;
	// size_t  argv_element_count = 0;

	// return null
	return NULL;

}


// Destroy generic command and control sessions.
BP_ERROR_T BP_EdcncDestroySession(P_BP_EDCNC_GENERIC_SESSION edcnc_session)
{

	// ensure we have an edcnc session
	if(!edcnc_session)
		return ERR_FAILURE;

	// if we have a command and control session, destroy it
	if(edcnc_session->cc_session)
	{
		BP_DestroyCCSession(edcnc_session->cc_session);
		edcnc_session->cc_session = NULL;
	}

	// if we have a parsed json config, destroy it
	if(edcnc_session->parsed_config)
	{
		BP_JSONDestroy(edcnc_session->parsed_config);
		edcnc_session->parsed_config = NULL;
	}

	// destroy the system registry (last)
	if(edcnc_session->system_hreg)
	{

		// Destroy reg but do not overwrite pointer.  If you overwrite the system_hreg pointer
		// you will be writing to free'd memory since the registry allocates the structure.
		BP_DestroyHashTableRegistry(edcnc_session->system_hreg);

	}

	// return indicating success
	return ERR_SUCCESS;

}

/*
 * BP-DBMessageReporting.h
 *
 *  Created on: Oct 12, 2016
 *      Author: root
 */

#ifndef __BP_DBMESSAGE_REPORTING_AND_DISPLAY_ROUTINES_H
#define __BP_DBMESSAGE_REPORTING_AND_DISPLAY_ROUTINES_H



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% BPDB Errors/Warnings/Notices %%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// message level/weight indicators.
typedef enum BPDB_MESSAGE_LEVEL_INDICATOR
{
	BPDB_MESSAGE_LEVEL_INDICATOR_UNSET,
	BPDB_MESSAGE_LEVEL_INDICATOR_SYSTEM_FATAL_ERROR,
	BPDB_MESSAGE_LEVEL_INDICATOR_SYSTEM_ERROR,
	BPDB_MESSAGE_LEVEL_INDICATOR_SYSTEM_WARNING,
	BPDB_MESSAGE_LEVEL_INDICATOR_SYSTEM_NOTICE
};

// message categories
typedef enum BPDB_MESSAGE_CATEGORY
{
	BPDB_MESSAGE_CATEGORY_UNSET,
	BPDB_MESSAGE_CATEGORY_SYSTEM_BUSY, /* Unavailable connections/no connections/etc */
	BPDB_MESSAGE_CATEGORY_SYNGEN,      /* Syntax generator failed for some reason.  Check error message for details. */
	BPDB_MESSAGE_CATEGORY_MEMORY,      /* The problem was related to memory, or the lack there-of */
	BPDB_MESSAGE_CATEGORY_NETWORK      /* Network related issue such as failed connect/reconnect. */
};

// subsystem type distinguishes between database types
typedef enum BPDB_SUBSYSTEM_TYPE
{
	BPDB_SUBSYSTEM_TYPE_UNSET,
	BPDB_SUBSYSTEM_TYPE_APP_INTERNAL,
	BPDB_SUBSYSTEM_TYPE_MARIADB,
	BPDB_SUBSYSTEM_TYPE_SQLITE,
	BPDB_SUBSYSTEM_TYPE_MARIADB_AND_SQLITE
};

// internal message structure for
typedef struct __BPDB_INTERNAL_MESSAGE_EXTRA_INFO {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Quick Lookup Indicators %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// message error level/weight
	BPDB_MESSAGE_LEVEL_INDICATOR msg_level;

	// subsystem originator of this message
	BPDB_SUBSYSTEM_TYPE subsystem_type;

	// this is the category to which this message belongs
	BPDB_MESSAGE_CATEGORY msg_category;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Message Origin Information %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if the origination of this message is remote (aka the server, this is set to BP_TRUE)
	BP_BOOL msg_originator_is_remote;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% System Busy Flags %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// checkout failed when requested
	BP_BOOL mariadb_connection_check_out_failed;

	// set if the remote host indicates we can't run any more queries
	BP_BOOL mariadb_remote_indicates_too_many_queries;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Syntax Generator Flags %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if the query generation failed, this is marked
	BP_BOOL query_generation_failed;

	BP_BOOL is_multi_query;
	BP_BOOL is_select_query;
	BP_BOOL is_insert_query;
	BP_BOOL is_update_query;
	BP_BOOL is_replace_query;
	BP_BOOL is_call_query;

	// if the user chooses to add their own custom sql in
	// our database query system, this gets marked.
	BP_BOOL has_ad_hoc_query_components;

	// if a query is parsed from json,
	BP_BOOL json_to_sql_generation_failed;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Query Status %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this gets marked if the query fails
	BP_BOOL mariadb_connection_query_failed;


} BPDB_INTERNAL_MESSAGE_EXTRA_INFO, *P_BPDB_INTERNAL_MESSAGE_EXTRA_INFO;

// msg type
typedef enum BPDB_ADD_MSG_TYPE
{
	BPDB_ADD_MSG_TYPE_UNSET,
	BPDB_ADD_MSG_TYPE_NOTICE,
	BPDB_ADD_MSG_TYPE_WARNING,
	BPDB_ADD_MSG_TYPE_ERROR
};

// Adds a message to an arbitrary branch (this is the overlying routine used by the error/warning/notice routines
// below.
BP_ERROR_T BP_DBAddMsg
(
	P_BPDB                                bpdb,
	BPDB_ADD_MSG_TYPE                     msg_type,
	char                                 *message,
	P_BPDB_INTERNAL_MESSAGE_EXTRA_INFO    ext_info_in_out = NULL
);


// ------ All of the Err/Warn/Notice Routines Utilize BP_DBAddMsg at their core ------------------------------




// add a parse error to the parse output hash table registry (since buff position can be 0, we need to use -1 as an unset value)
BP_ERROR_T BP_DBAddErrorMsg
(
	P_BPDB                                bpdb,
	char                                 *message,
	P_BPDB_INTERNAL_MESSAGE_EXTRA_INFO    ext_info_in_out = NULL
);

// add a parse warning to the parse output hash table registry
BP_ERROR_T BP_DBAddWarningMsg
(
	P_BPDB                                bpdb,
	char *                                message,
	P_BPDB_INTERNAL_MESSAGE_EXTRA_INFO    ext_info_in_out = NULL
);

// add a parse notice to the parse output hash table registry
BP_ERROR_T BP_DBAddNoticeMsg
(
	P_BPDB                                bpdb,
	char *                                message,
	P_BPDB_INTERNAL_MESSAGE_EXTRA_INFO    ext_info_in_out = NULL
);


// -- message display routines -------------------

// display notices
BP_ERROR_T BP_DBDisplayNotices(P_BPDB bpdb);

// display warnings
BP_ERROR_T BP_DBDisplayWarnings(P_BPDB bpdb);

// display errors
BP_ERROR_T BP_DBDisplayErrors(P_BPDB bpdb);

// display all the parser messages
BP_ERROR_T BP_DBDisplayMessages(P_BPDB bpdb);



#endif

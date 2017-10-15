/*
 * BP_CommandLineToolBase.h
 *
 *  Created on: Aug 31, 2013
 *      Author: root
 */

#ifndef BP_COMMANDLINETOOLBASE_H_
#define BP_COMMANDLINETOOLBASE_H_

// type defining what type of data will be found in the db
typedef enum
{
	BP_CLT_DATA_TYPE_UNSET,
	BP_CLT_DATA_TYPE_APP_RUN_ARGS,
	BP_CLT_DATA_TYPE_APP_STDOUT,
	BP_CLT_DATA_TYPE_APP_STDERR,
	BP_CLT_DATA_TYPE_APP_BADVAL
} BP_CLT_DATA_TYPE;




// record which matches up to the `command_line_tool_data`
// column in the resultsdb.
typedef struct __BP_COMMAND_LINE_TOOL_DB_DATA {

	// id of data in the db
	size_t id;

	// matches with main application table
	size_t tool_id;

	// data type which is unique for this tool (set in binary enumerator)
	BP_CLT_DATA_TYPE tool_data_type;

	// assessment id which matches up with this tool
	size_t assessment_id;

	// base associated with this
	size_t base;

	// path associated with this clt
	size_t path;

	// script associated with this clt
	size_t script;
	size_t result;
	char data_sha1[40];
	char * data;
	size_t data_size;
	char *commit_ts;


} BP_COMMAND_LINE_TOOL_DB_DATA, *P_BP_COMMAND_LINE_TOOL_DB_DATA;

// db entry which corresponds with the `command_line_tool` column
// in the database.
typedef struct __BP_COMMAND_LINE_TOOL_DB_ENTRY {

	// id which corresponds to this tool
	size_t id;

	// name of the tool
	char * name;

	// version of the tool
	char * version;

	// description of the tool
	char * description;

	// time the record was added to the database
	char * commit_ts;

	// data entries matched to this tool
	P_BP_COMMAND_LINE_TOOL_DB_DATA data_entries;

	// number of entries in the above stack
	size_t data_entry_n;

} BP_COMMAND_LINE_TOOL_DB_ENTRY, *P_BP_COMMAND_LINE_TOOL_DB_ENTRY;


// command line tool db session
typedef struct __BP_COMMAND_LINE_TOOL_DB_SESSION {

	// memory allocator for the session
	P_BP_LINKL m;

	// structures which
	P_BP_COMMAND_LINE_TOOL_DB_ENTRY tools;
	size_t tool_n;

} BP_COMMAND_LINE_TOOL_DB_SESSION, *P_BP_COMMAND_LINE_TOOL_DB_SESSION;



// All command line tools inherit from this class.  This class
// provides a very easy to use interface for linking command line
// operational output with the database.
class BP_CommandLineToolBase : public Block
{

public:

	// memory allocator for base
	P_BP_LINKL application_m;

	// Application run monitor.  Simple interface to global
	// os profiling singleton.
	BP_CLTRunMonitor * app_mon;

	// flag indicating whether or not the initialization has failed
	size_t application_init_failed;

	// Command line tool session.  This works with the native Block
	// command and control capability to allow for fully interactive
	// application control.
	P_BP_COMMAND_LINE_TOOL_DB_SESSION application_session;

	// path to the command line tool binary
	char * application_path_to_bin;

	// Environment pointers for application (duplicated when set).  Should be
	// ideally set from app init envp, but can be set however you want.
	char ** application_envp;
	size_t  application_envp_n;

	// arguments
	char ** application_args;
	size_t  application_arg_n;

	// run id (from db, use the getNewRunIDFromDB method below)
	char * application_unique_db_run_id;

	// constructor / deconstructor
	BP_CommandLineToolBase(char * path_to_application_binary);
	~BP_CommandLineToolBase();

	// sets envp (required): Duplicates pointers.
	BP_ERROR_T SetEnvP(char ** envp, size_t envp_n);

	// destroys envp
	BP_ERROR_T DestroyLocalEnvP();

	// add a command line argument
	BP_ERROR_T AddCmdArgument(char * token, char *arg);

	// destroy the arguments
	BP_ERROR_T DestroyCmdArguments();

	// =====================================


	// Execute application.  This will populate some local data, so before
	// calling it recursively, call the cleanup routine below, or just delete
	// and recreate the object.  Performance hits should be negligable either way.
	BP_ERROR_T executeApplicationBinary
	(
			size_t poll_read_timeout,
			size_t poll_read_timeout_stderr,
			BP_BOOL display_reads,
			BP_BOOL display_writes
	);

	// Post execution cleanup routine.  Call after you've duplicated any data
	// you'd like to keep from the populated session.
	BP_ERROR_T postExecutionCleanup();


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Database Functions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// command line tool in the db
	size_t  cltdb_id;

	// name of the command line tool (as shown in the db)
	char *  cltdb_name;

	// version of the command line tool (as shown in the db);
	char *  cltdb_version;

	// description of the command line tool (as shown in the db)
	char * cltdb_description;

	// commit timestamp
	char * cltdb_commit_timestamp;



	// insert command line tool into the db
	BP_ERROR_T insertCommandLineTool(char * sensor_id, char *name, char *version, char *description);

	// insert command line tool run data
	BP_ERROR_T insertCommandLineToolData( size_t tool_data_type, size_t assessment_id, size_t nmap_host_id, size_t base_id, size_t path_id, size_t script_id, size_t result_id, char * data, size_t data_size);

	// retrieves a new unique run id from the database
	BP_ERROR_T getNewRunIDFromDB();



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %% Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// display current configuration
	BP_ERROR_T displayApplicationConfig();

	// display arguments currently configured
	BP_ERROR_T displayApplicationArgs();

	// Displays run string.
	BP_ERROR_T displayApplicationRunString();

};

#endif /* BP_COMMANDLINETOOLBASE_H_ */

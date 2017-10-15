/*
 * BP_CommandLineToolBase.c
 *
 *  Created on: Aug 30, 2013
 *      Author: root
 */

// NOTE: SEE RELATED BP_CommandLineToolBase.h FILE FOR
//       INFORMATION RELATED TO HOW TO USE THIS INTERFACE.


// include the blockparty main include
#include "../../../../include/BP-Main.h"

// application controller constructor
BP_CommandLineToolBase::BP_CommandLineToolBase(char * path_to_application_binary) : Block()
{

	// set default parameters
	this->application_unique_db_run_id = NULL;
	this->application_init_failed   = 0;
	this->application_path_to_bin   = NULL;
	this->application_args          = NULL;
	this->application_arg_n         = 0;
	this->application_envp          = NULL;
	this->application_envp_n        = 0;
	this->application_m             = NULL;

	// ensure we have a application binary
	if(!path_to_application_binary)

	{
		printf("\n [+] Error: Application controller cannot be created without a valid path to application binary.");
		this->application_init_failed = 1;
		return;

	}

	// memory allocator for this class
	size_t random_id = BP_GenRandomUnsignedLong(1, 0xffffffff);

	// create new threadsfe allocator
	this->application_m = BP_CreateLinkL("application app ctrl", BP_TRUE);

	// duplicate the path as an initial element
	this->application_path_to_bin = (char *) tq_strdup(this->application_m, path_to_application_binary, BPLN);

	// set first argument as bin path
	this->application_args    = (char **) tq_calloc(this->application_m, sizeof(char *) * (2), 1, BPLN);
	this->application_args[0] = (char *)  tq_strdup(this->application_m, this->application_path_to_bin, BPLN);
	this->application_args[1] = NULL;
	this->application_arg_n++;

	// Create instance into global run monitor.
	this->app_mon = new BP_CLTRunMonitor
	(
		this->application_arg_n,
		this->application_args,
		this->application_envp
	);

	// ==== Init Command Line Tool DB Vars ========

	// set command line tool db init values
	this->cltdb_id               = 0;
	this->cltdb_name             = 0;
	this->cltdb_version          = 0;
	this->cltdb_description      = 0;
	this->cltdb_commit_timestamp = 0;

}

// application controller deconstructor
BP_CommandLineToolBase::~BP_CommandLineToolBase()
{

	if(this->application_unique_db_run_id)
	{
		tq_free(this->application_m, this->application_unique_db_run_id);
		this->application_unique_db_run_id = NULL;
	}

	// destroy path to bin
	if(this->application_path_to_bin)
	{
		tq_free(this->application_m, this->application_path_to_bin);
		this->application_path_to_bin = NULL;
	}
	// destroy data
	this->DestroyLocalEnvP();
	this->DestroyCmdArguments();

	// check memory collector
	if(this->application_m)
	{
		if(this->application_m->total_allocations)
		{
			printf("\n [+] Error: Destroying BP_CommandLineToolBase with floating chunks.");
			BP_DisplayLinkL(this->application_m);
		}

	}

	// destroy the list
	BP_DestroyLinkL(this->application_m, BP_TRUE);

	// set the pointer to null
	this->application_m = NULL;

	// destroy the application monitor
	if(this->app_mon)
		delete this->app_mon;

}

// sets envp (required): Duplicates pointers.
BP_ERROR_T BP_CommandLineToolBase::SetEnvP(char ** envp, size_t envp_n)
{

	// ensure we have some values
	if(!envp || !envp_n)
		return ERR_FAILURE;

	// check if we have envp and destroy if so
	if(this->application_envp_n || this->application_envp)
	{
		// destroys envp and sets pointer/count to zero
		this->DestroyLocalEnvP();
	}

	// set count from input
	this->application_envp_n = envp_n;

	// allocate enough space for the buffer
	this->application_envp   = (char **) tq_realloc(this->application_m, this->application_envp, sizeof(char *) * (this->application_envp_n+1), BPLN);

	// zero out the allocated memory
	memset((void *) this->application_envp, 0x00, sizeof(char *) * (this->application_envp_n+1));

	// copy in pointers
	size_t n = 0;
	for(; n < this->application_envp_n; n++)
	{
		// copy in envp buffer (max 16k)
		this->application_envp[n] = (char *) tq_strdup(this->application_m, envp[n], BPLN);
	}

	// return indicating success
	return ERR_SUCCESS;

}

// destroys envp
BP_ERROR_T BP_CommandLineToolBase::DestroyLocalEnvP()
{

	if(!this->application_envp)
		return ERR_FAILURE;
	if(!this->application_envp_n)
		return ERR_FAILURE;

	size_t n = 0;
	for(; n < this->application_envp_n; n++)
	{
		if(this->application_envp[n])
		{
			tq_free(this->application_m, this->application_envp[n]);
		}
	}

	// free the envp itself
	tq_free(this->application_m, this->application_envp);

	// set pointer to null to eliminate any confusion
	this->application_envp = NULL;

	// reset counter as well
	this->application_envp_n = 0;

	// return indicating success
	return ERR_SUCCESS;

}

// add a command line argument
BP_ERROR_T BP_CommandLineToolBase::AddCmdArgument(char * token, char *arg)
{

	if(!this->application_path_to_bin)
	{
		printf("\n [+] Error: Application module requires a path to binary set before parameters can be added.");
		return ERR_FAILURE;
	}

	// flag indicating update status of either token or arg
	size_t token_or_arg_updated = 0;

	// increment argument count
	if(token)
	{

		// increase the argument
		this->application_arg_n++;

		// realloc on argument stack
		this->application_args = (char **) tq_realloc(this->application_m, this->application_args, sizeof(char *) * (this->application_arg_n+1), BPLN);

		// duplicate the argument (max 8k length) and truncate
		this->application_args[this->application_arg_n-1] = (char *) tq_strdup(this->application_m, token, BPLN);
		this->application_args[this->application_arg_n]   = (char *) NULL;

		// set flag
		token_or_arg_updated=1;

	}

	// check to see if we have arg pointer
	if(arg)
	{

		// increment the argument counter
		this->application_arg_n++;

		// realloc on argument stack
		this->application_args = (char **) tq_realloc(this->application_m, this->application_args, sizeof(char *) * (this->application_arg_n+1), BPLN);

		// duplicate the argument (max 8k length) and truncate
		this->application_args[this->application_arg_n-1] = (char *) tq_strdup(this->application_m, arg, BPLN);
		this->application_args[this->application_arg_n]   = (char *) NULL;

		// set flag
		token_or_arg_updated = 1;


	}

	// ensure we have updated at least one token before giving the ok
	if(!token_or_arg_updated)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// destroy the arguments
BP_ERROR_T BP_CommandLineToolBase::DestroyCmdArguments()
{

	// ensure we have arguments to free
	if(this->application_args)
	{

		// loop to destroy
		size_t n = 0;
		for(; n < this->application_arg_n; n++)
		{
			if(this->application_args[n])
				tq_free(this->application_m, this->application_args[n]);
		}

		// free the char ** itself
		tq_free(this->application_m, this->application_args);

		// reset pointers
		this->application_args  = NULL;
		this->application_arg_n = 0;

		// return indicating success
		return ERR_SUCCESS;

	}

	// return indicating success
	return ERR_FAILURE;

}


// Execute application.  This will populate some local data, so before
// calling it recursively, call the cleanup routine below, or just delete
// and recreate the object.  Performance hits should be negligable either way.
//
// You can also reimplement this function to meet the needs of the command you
// are executing so long as you satisfy basic requirements.
BP_ERROR_T BP_CommandLineToolBase::executeApplicationBinary
(
		size_t poll_read_timeout,
		size_t poll_read_timeout_stderr,
		BP_BOOL display_reads,
		BP_BOOL display_writes
)
{

	// ensure we have a path to the binary
	if(!this->application_path_to_bin)
	{
		printf("\n [!!] Error: Using Application interface requires path to binary.");
		return ERR_FAILURE;
	}

	// ensure we have arguments
	if(!this->application_args)
	{
		printf("\n [!!] Error: Application Module Requires parameters to run.");
		return ERR_FAILURE;
	}

	if(!this->application_envp)
	{
		printf("\n [!!] Error: Application Module requires an ENVP set.  You can just pass in the one from main().");
		return ERR_FAILURE;
	}

	// execute the program
	this->File_Execute(this->application_args[0], this->application_args, this->application_envp, BP_TRUE);

	// set nonblocking io on the program
	BP_FSetNonBlockingIO(this->fdSet);

	// create new cc session
	this->ccNewSession(this->fdSet, "Application Command And Control Module", BP_CC_SHELL_LINUX_EXECVE);

	// set monitoring flags
	this->cc_session->display_reads_from_app_stdout = display_reads;
	this->cc_session->display_reads_from_app_stderr = display_reads;
	this->cc_session->display_writes_to_app_stdin   = display_writes;

	// set polling timeouts
	this->cc_session->poll_timeout        = poll_read_timeout;
	this->cc_session->poll_timeout_stderr = poll_read_timeout_stderr;


	// return indicating success
	return ERR_SUCCESS;

}

BP_ERROR_T BP_CommandLineToolBase::displayApplicationArgs()
{

	// ensure we have arguments
	if(!this->application_args)
	{
		return ERR_FAILURE;
	}

	printf("\n [+] Application Parameters:");
	size_t n = 0;
	for(; n < this->application_arg_n; n++)
	{
		printf("\n [+] arg%zu: %s", n, this->application_args[n]);
	}

	printf("\n [+] As Run String: \n");

	n = 0;
	for(; n < this->application_arg_n; n++)
	{
		printf("%s ", this->application_args[n]);
	}

	printf("\n");
	printf("\n");
	// return indicating success
	return ERR_SUCCESS;

}

// Displays run string.
BP_ERROR_T BP_CommandLineToolBase::displayApplicationRunString()
{

	// ensure we have an argument count
	if(!this->application_arg_n)
		return ERR_FAILURE;
	printf("\n");
	size_t n = 0;
	for(; n < this->application_arg_n; n++)
	{
		printf("%s ", this->application_args[n]);
	}

	return ERR_SUCCESS;
}

// Run this after execution to clean up any data stored in the class.
BP_ERROR_T BP_CommandLineToolBase::postExecutionCleanup()
{

	// close file descriptors
	close(this->fdSet->stderr[0]);
	close(this->fdSet->stderr[1]);
	close(this->fdSet->stdin[0]);
	close(this->fdSet->stdin[1]);
	close(this->fdSet->stdout[0]);
	close(this->fdSet->stdout[1]);

	// run kill loop
	do
	{

		if(!this->fdSet->pid || this->fdSet->pid == (pid_t) -1 )
			break;

		// attempt to wait on any pid (frees any possible zombies)
		waitpid(-1,NULL,WNOHANG);

		// update the application snapshot
		this->app_mon->updateSnapshot();

		// search for the process
		this->app_mon->searchProcesses(NULL, NULL, this->fdSet->pid);

		// If our process doesn't exist, we can exit ok without forcible
		// kill.
		if(!this->app_mon->process_search_indexes_n)
			break;

		// At this point, we've confirmed that our application still exists, so we must
		// kill it forcibly

		// create kill buffer
		char kill_buff[128];

		// zero out the buffer
		memset(&kill_buff, 0x00, sizeof(kill_buff));

		// SAFELY create kill buffer
		sprintf((char *) &kill_buff, "kill -9 %u", this->fdSet->pid);

		printf("\n [+] Running Kill: %s", kill_buff);

		// use system command shell to kill the process
		system(kill_buff);

		// sleep before trying again (prevents kill insanity)
		sleep(1);

	} while (this->app_mon->process_search_indexes_n);

	// return indicating success
	return ERR_SUCCESS;
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %% Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// display current configuration
BP_ERROR_T BP_CommandLineToolBase::displayApplicationConfig()
{

	printf("\n [+] Displaying Application Controller: %p", this);
	printf("\n\t allocator:    %p", this->application_m);
	printf("\n\t total allocs: %u", this->application_m->total_allocations);
	printf("\n\t alloc size:   %u", this->application_m->total_size);
	printf("\n\t binpath:      %s", this->application_path_to_bin);
	printf("\n\t envp:         %p", this->application_envp);
	printf("\n\t envp_n:       %u", this->application_envp_n);
	printf("\n\t args:         %p", this->application_args);
	printf("\n\t arg_n:        %u", this->application_arg_n);

	// display allocated memory
	BP_DisplayLinkL(this->application_m);

	// return indicating success
	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Database Functions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// insert command line tool into the db
BP_ERROR_T BP_CommandLineToolBase::insertCommandLineTool(char * sensor_id, char *name, char *version, char *description)
{

	// ensure we have at least some parameters
	if(!sensor_id && !name && !version && !description)
	{
		return ERR_FAILURE;
	}

	// do length checks up front
	if(strnlen(sensor_id, BP_KILOBYTE * 8) > (BP_KILOBYTE * 4))
		return ERR_FAILURE;
	if(strnlen(name, BP_KILOBYTE * 8) > (BP_KILOBYTE * 4))
		return ERR_FAILURE;
	if(strnlen(version, BP_KILOBYTE * 8) > (BP_KILOBYTE * 4))
		return ERR_FAILURE;
	if(strnlen(description, BP_KILOBYTE * 8) > (BP_KILOBYTE * 4))
		return ERR_FAILURE;

	// create a builder
	BP_StringUtils * builder = new BP_StringUtils(NULL);

	// set information here
	char * sensor_id_s   = NULL;
	char * name_s        = NULL;
	char * version_s     = NULL;
	char * description_s = NULL;

	// set sensor id value (always alloc so we can simplify freeing below)
	if(!sensor_id)
		sensor_id_s = (char *) bpstrdup("NULL");
	else
		sensor_id_s = (char *) BP_BufferToMySQLHexString(sensor_id, bpstrlen(sensor_id));

	// set name
	if(!name)
		name_s = (char *) bpstrdup("NULL");
	else
		name_s = (char *) BP_BufferToMySQLHexString(name, bpstrlen(name));

	// set version
	if(!version)
		version_s = (char *) bpstrdup("NULL");
	else
		version_s = (char *) BP_BufferToMySQLHexString(version, bpstrlen(version));

	// set description
	if(!description)
		description_s = (char *) bpstrdup("NULL");
	else
		description_s = (char *) BP_BufferToMySQLHexString(description, bpstrlen(description));

	// start building the query
	builder->BuildString((char *) "INSERT INTO `command_line_tools` ( `tool_sha1`, `sensor_identifier_sha1`, `run_attempts`, `name`, `version`, `description` ) VALUES ");

	// create pointer to hold our sha1
	char * tool_sha1 = (char *) NULL;

	// set allocation size
	size_t alloc_size =  bpstrlen(sensor_id)+bpstrlen(name)+64;

	// allocate space for the buffer
	char * unique_buff = (char *) tq_calloc(this->application_m, alloc_size, 1, BPLN);

	// this->GenSHA1();
	snprintf(unique_buff, alloc_size-1, "%s%s", sensor_id, name);

	// set length
	size_t c = bpstrlen(unique_buff);

	// generate sha1
	this->GenSHA1(unique_buff, c);

	// create bad value if necessary
	if(!this->sha1_ascii)
		this->sha1_ascii = (char *) tq_strdup(this->application_m, "BADSHA1FROMBLOCK", BPLN);

	// build the rest of the query
	builder->BuildString(" ( '");
	builder->BuildString(this->sha1_ascii);
	builder->BuildString("' , ");
	builder->BuildString(sensor_id_s);
	builder->BuildString(" , ");
	builder->BuildString(" 1 ");
	builder->BuildString(" , ");
	builder->BuildString(name_s);
	builder->BuildString(" , ");
	builder->BuildString(version_s);
	builder->BuildString(" , ");
	builder->BuildString(description_s);
	builder->BuildString(" ) ON DUPLICATE KEY UPDATE id = LAST_INSERT_ID(id), run_attempts = run_attempts + 1");

	// destroy the sha1
	// tq_free(this->application_m, this->sha1_ascii);

	if(unique_buff)
		tq_free(this->application_m, unique_buff);

	// prepare and run insert query
	if(this->mysqlPrepareQuery(builder->build_str) != ERR_SUCCESS)
	{
		// destroy data
		delete builder;
		if(sensor_id_s)
			bpfree(sensor_id_s);
		if(name_s)
			bpfree(name_s);
		if(version_s)
			bpfree(version_s);
		if(description_s)
			bpfree(description_s);
		return ERR_FAILURE;
	}

	if(this->mysqlExecuteStatement() != ERR_SUCCESS)
	{
		// destroy data
		delete builder;
		if(sensor_id_s)
			bpfree(sensor_id_s);
		if(name_s)
			bpfree(name_s);
		if(version_s)
			bpfree(version_s);
		if(description_s)
			bpfree(description_s);
		return ERR_FAILURE;
	}

	// set the insert id
	if(this->mysqlSession->stmt)
	{



#if USE_MARIADB == 1

	// save insert id for next set of inserts
	this->cltdb_id = mysql_insert_id(this->mysqlSession->conn);

#else

	// save insert id for next set of inserts
	this->cltdb_id =this->mysqlSession->stmt->insert_id;

#endif

	}

	// destroy the statement
	if(this->mysqlDestroyStatement() != ERR_SUCCESS)
	{
		// destroy data
		delete builder;
		if(sensor_id_s)
			bpfree(sensor_id_s);
		if(name_s)
			bpfree(name_s);
		if(version_s)
			bpfree(version_s);
		if(description_s)
			bpfree(description_s);
		return ERR_FAILURE;
	}

	// destroy data
	if(sensor_id_s)
		bpfree(sensor_id_s);
	if(name_s)
		bpfree(name_s);
	if(version_s)
		bpfree(version_s);
	if(description_s)
		bpfree(description_s);


	// destroy the builder string
	delete builder;

	// return indicating success
	return ERR_SUCCESS;

}

// insert command line tool run data
BP_ERROR_T BP_CommandLineToolBase::insertCommandLineToolData
(
	size_t tool_data_type,
	size_t assessment_id,
	size_t nmap_host_id,
	size_t base_id,
	size_t path_id,
	size_t script_id,
	size_t result_id,
	char * data,
	size_t data_size
)
{

	// check that we have a resolved id
	if(!this->cltdb_id)
	{
		printf("\n [!!] Attempting to insertCommandLineToolData without having a valid tool_id to associate it with.  Exiting routine with failure.  Fix: call insertCommandLineTool() first.");
		return ERR_SUCCESS;
	}

	// cant insert data without data
	if(!data || !data_size)
		return ERR_FAILURE;

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;
	if(!this->mysqlSession->connected)
		return ERR_FAILURE;

	// create a new run id if we don't already have one
	if(!this->application_unique_db_run_id)
		this->getNewRunIDFromDB();
	if(!this->application_unique_db_run_id)
		return ERR_FAILURE;

	// create new string builder
	BP_StringUtils * builder = new BP_StringUtils(NULL);

	// frontload insert query
	builder->BuildString("INSERT INTO `command_line_tool_data` (`run_id`, `tool_id`, `tool_data_type`, `assessment_id`, `nmap_host_id`, `base`, `path`, `script`, `result`, `data_sha1`, `data`) VALUES ");

	// `run_id`,
	char * run_id_s = BP_BufferToMySQLHexString(this->application_unique_db_run_id, bpstrlen(this->application_unique_db_run_id));

	// `tool_id`,
	char * tool_id_s = BP_SizeTToMySQLBuffer(this->cltdb_id);

	// `tool_data_type`,
	char * tool_data_type_s = BP_SizeTToMySQLBuffer(tool_data_type);

	// `assessment_id`,
	char * assessment_id_s = BP_SizeTToMySQLBuffer(assessment_id);

	// `nmap_host_id`,
	char * nmap_host_id_s = BP_SizeTToMySQLBuffer(nmap_host_id);

	// `base`,
	char * base_s = BP_SizeTToMySQLBuffer(base_id);

	// `path`,
	char * path_s = BP_SizeTToMySQLBuffer(path_id);

	// `script`,
	char * script_s = BP_SizeTToMySQLBuffer(script_id);

	// `result`,
	char * result_s = BP_SizeTToMySQLBuffer(result_id);

	// `data_sha1`,
	this->GenSHA1(data, data_size);

	// create hex string
	char * data_sha1_s = BP_BufferToMySQLHexString(this->sha1_ascii, bpstrlen(this->sha1_ascii));

	// `data`
	char * data_s = BP_BufferToMySQLHexString(data, data_size);


	// build in parameters
	builder->BuildString(" ( ");
	builder->BuildString(run_id_s);
	builder->BuildString(" , ");
	builder->BuildString(tool_id_s);
	builder->BuildString(" , ");
	builder->BuildString(tool_data_type_s);
	builder->BuildString(" , ");
	builder->BuildString(assessment_id_s);
	builder->BuildString(" , ");
	builder->BuildString(nmap_host_id_s);
	builder->BuildString(" , ");
	builder->BuildString(base_s);
	builder->BuildString(" , ");
	builder->BuildString(path_s);
	builder->BuildString(" , ");
	builder->BuildString(script_s);
	builder->BuildString(" , ");
	builder->BuildString(result_s);
	builder->BuildString(" , ");
	builder->BuildString(data_sha1_s);
	builder->BuildString(" , ");
	builder->BuildString(data_s);
	builder->BuildString(" ) ");

	// set execute
	this->mysqlPrepareQuery(builder->build_str);
	this->mysqlExecuteStatement();
	this->mysqlDestroyStatement();


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %% Destroy Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create hex string
	if(data_sha1_s)
		bpfree(data_sha1_s);

	// `run_id`,
	if(run_id_s)
		bpfree(run_id_s);

	// `tool_id`,
	if(tool_id_s)
		bpfree(tool_id_s);

	// `tool_data_type`,
	if(tool_data_type_s)
		bpfree(tool_data_type_s);

	// `assessment_id`,
	if(assessment_id_s)
		bpfree(assessment_id_s);

	// `nmap_host_id`,
	if(nmap_host_id_s)
		bpfree(nmap_host_id_s);

	// `base`,
	if(base_s)
		bpfree(base_s);

	// `path`,
	if(path_s)
		bpfree(path_s);

	// `script`,
	if(script_s)
		bpfree(script_s);

	// `result`,
	if(result_s)
		bpfree(result_s);

	// destroy the builder
	delete builder;

	// return indicating success
	return ERR_SUCCESS;

}

// retrieves a sha1(UUID()) from the db.
BP_ERROR_T BP_CommandLineToolBase::getNewRunIDFromDB()
{

	// ensure mysql sessions work
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// destroy run id if it exists
	if(this->application_unique_db_run_id)
	{
		tq_free(this->application_m, this->application_unique_db_run_id);
		this->application_unique_db_run_id = NULL;
	}

	// create new string builder
	BP_StringUtils * builder = new BP_StringUtils(NULL);

	// create result buffer
	char result[64];
	memset(&result, 0x00, sizeof(result));

	// create query
	builder->BuildString("SELECT SHA1(UUID())");

	// prepare the query
	if(this->mysqlPrepareQuery(builder->build_str) != ERR_SUCCESS)
	{
		delete builder;
		return ERR_FAILURE;
	}

	// add result
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *)&result, sizeof(result)-1);

	// bind the results
	this->mysqlBindResults();

	// execute the statement
	if(this->mysqlExecuteStatement() != ERR_SUCCESS)
	{
		delete builder;
		return ERR_FAILURE;
	}

	// store the results of the query
	this->mysqlStoreResults();

	// retrieve the unique sha1
	this->mysqlFetchResult();

	// allocate space for the new run id (sizeof result)
	this->application_unique_db_run_id = (char *) tq_calloc(this->application_m, sizeof(result), 1, BPLN);

	// copy in the data
	memcpy(this->application_unique_db_run_id, result, sizeof(result));

	// destroy the statement
	this->mysqlDestroyStatement();

	// delete the builder
	delete builder;

	// return indicating success
	return ERR_SUCCESS;

}

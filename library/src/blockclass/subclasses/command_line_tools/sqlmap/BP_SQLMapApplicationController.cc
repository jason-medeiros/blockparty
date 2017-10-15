/*
 * BP_SQLMapApplicationController.c
 *
 *  Created on: Aug 30, 2013
 *      Author: root
 */

// NOTE: SEE RELATED BP_SQLMapApplicationController.h FILE FOR
//       INFORMATION RELATED TO HOW TO USE THIS INTERFACE.


// include the blockparty main include
#include "../../../../../include/BP-Main.h"

// Counter indicating how many sqlmap classes have started
size_t global_sqlmap_run_count = 0;


// contructorstructor
BP_SQLMapApplicationController::BP_SQLMapApplicationController(char * path_to_sqlmap_binary): BP_CommandLineToolBase(path_to_sqlmap_binary)
{

	// this is flagged after the app exited
	this->sqlmap_exited = 0;

	// this is the sqlmap result csv file
	this->sqlmap_result_csv_file = NULL;

	// set parsed data object to null (BP_StringUtils)
	this->sqlmap_csv_parsed_data = NULL;

	// create a new linked list
	this->sqlmap_m = BP_CreateLinkL("sqlmap application allocator", BP_TRUE);

}

// deconstructor
BP_SQLMapApplicationController::~BP_SQLMapApplicationController()
{

	// destroy the parsed data if so
	if(this->sqlmap_csv_parsed_data)
	{
		delete this->sqlmap_csv_parsed_data;
		this->sqlmap_csv_parsed_data = NULL;
	}



	// destroy the memory allocator
	BP_DestroyLinkL(this->sqlmap_m, BP_TRUE);

	// set allocator to null
	this->sqlmap_m = NULL;

}

// =====================================


// Execute sqlmap.  This will populate some local data, so before
// calling it recursively, call the cleanup routine below, or just delete
// and recreate the object.  Performance hits should be negligable either way.
BP_ERROR_T BP_SQLMapApplicationController::executeSQLMapBinary()
{

	// ensure we have a path to the binary
	if(!this->application_path_to_bin)
	{
		printf("\n [+] Error: Using Application interface requires path to binary.");
		return ERR_FAILURE;
	}

	// ensure we have arguments
	if(!this->application_args)
	{
		printf("\n [+] Error: Application Module Requires parameters to run.");
		return ERR_FAILURE;
	}

	if(!this->application_envp)
	{
		printf("\n [+] Error: Application Module requires an ENVP set.  You can just pass in the one from main().");
		return ERR_FAILURE;
	}

	// execute the program
	if(this->File_Execute(this->application_args[0], this->application_args, this->application_envp, BP_TRUE) != ERR_SUCCESS)
	{

		printf("\n [!!] Failed to execute sqlmap!");
		this->displayApplicationArgs();
		printf("\n");

		// optional breakpoint
		// __asm("int3");
		return ERR_FAILURE;

	}

	if(!this->fdSet)
	{
		printf("\n [!!] Err: No FDSET!?");
		return ERR_FAILURE;
	}

	// increase the global run count here
	global_sqlmap_run_count++;

	// set nonblocking io on the program
	BP_FSetNonBlockingIO(this->fdSet);

	// create new cc session
	this->ccNewSession(this->fdSet, "SQLMap Command And Control Module", BP_CC_SHELL_LINUX_EXECVE);

	// set monitoring flags
	this->cc_session->display_reads_from_app_stdout = BP_FALSE;
	this->cc_session->display_reads_from_app_stderr = BP_FALSE;
	this->cc_session->display_writes_to_app_stdin   = BP_FALSE;

	// set the poll timeout
	this->cc_session->poll_timeout        = 60000 * 15;
	this->cc_session->poll_timeout_stderr = 60000 * 15;

	// expect the shutdown and exit after
	this->ccAddTryExcept("start_sqlmap", 0, 0, "shutting down at", bpstrlen("shutting down at"));

	// add premature exit on errors
	this->ccAddPrematureExitPCRE("\\[ERROR\\] all tested parameters appear to be not injectable");
	this->ccAddPrematureExitPCRE("shutting down at");
	this->ccAddPrematureExitPCRE("\\[CRITICAL\\] there were no forms found at the given target URL");
	this->ccAddPrematureExitPCRE("\\[CRITICAL\\] connection exception detected");
	this->ccAddPrematureExitPCRE("\\[CRITICAL\\]");

	// add an extraction pcre test (data is pulled
	this->ccAddInformationExtractionPCRE("\\[INFO\\] you can find results of scanning in multiple targets mode inside the CSV file '.*csv'");

	// run command test
	this->ccExecuteCommand("start_sqlmap", NULL);

	// close file descriptors
	close(this->fdSet->stderr[0]);
	close(this->fdSet->stderr[1]);
	close(this->fdSet->stdin[0]);
	close(this->fdSet->stdin[1]);
	close(this->fdSet->stdout[0]);
	close(this->fdSet->stdout[1]);


	// Check to see if we need to kill the process (uses out of
	// process kill to replicate actual runtime kill which may be
	// expected by process, also prevents any undefined in-process behavior)

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

		// sleep before trying again (prevents insanity)
		sleep(1);


	} while (this->app_mon->process_search_indexes_n);

	// decrease run ocunt
	global_sqlmap_run_count--;

	// Parse the buffer matched from information extraction out to a csv file which we
	// can read from.
	if(this->cc_session->info_searches)
	if(!this->cc_session->info_searches[0].match_strs_n || !this->cc_session->info_searches[0].match_strs)
	{

		// no matches means no file exported
		return ERR_FAILURE;

	}

	/*
	this->sqlmap_result_csv_file = BP_ParseBufferToUNIXFilesystemPathCSVPCRE(this->cc_session->info_searches[0].match_strs[0], bpstrlen(this->cc_session->info_searches[0].match_strs[0]));

	// link the filename
	tq_link(this->sqlmap_m, this->sqlmap_result_csv_file, 4, BPLN);

	// attempt to partse and insert data
	this->parseSQLMapCSVFileAndInsert(this->sqlmap_result_csv_file);
	*/

	// decrease the sqlmap run count here


	// return indicating success
	return ERR_SUCCESS;

}

// inserts results into the database (requires a valid this->mysqlSession).  Data inserted
// is taken directly from sqlmap_csv_parsed_data.
BP_ERROR_T BP_SQLMapApplicationController::insertSQLMapBinaryRunResults()
{

	if(!this->sqlmap_csv_parsed_data)
	{
		return ERR_FAILURE;
	}

	// ensure we have csv data
	if(!this->sqlmap_csv_parsed_data->csv_num_rows)
		return ERR_FAILURE;

	// The sqlmap output includes a header row.  If we only have one row, we have a verified
	// empty file and can exit here.
	if(this->sqlmap_csv_parsed_data->csv_num_rows == 1)
		return ERR_FAILURE;

	// create new builder
	BP_StringUtils * builder = new BP_StringUtils(NULL);

	// base builder insert string
	builder->BuildString((char *)"INSERT IGNORE INTO `sqlmap_results` (`result_sha1`, `target_sha1`, `target_url`, `place`, `parameter`, `techniques`) VALUES ");

	// insert sqlmap data
	size_t n = 1;
	for(; n < this->sqlmap_csv_parsed_data->csv_num_rows; n++)
	{

		if(!this->sqlmap_csv_parsed_data->csv[n][0])
					continue;
		if(!this->sqlmap_csv_parsed_data->csv[n][1])
					continue;
		if(!this->sqlmap_csv_parsed_data->csv[n][2])
					continue;
		if(!this->sqlmap_csv_parsed_data->csv[n][3])
			continue;

		// build all elements into one string for a joined hash (allows fast lookup)
		BP_StringUtils * tmp = new BP_StringUtils(NULL);

		// build string
		tmp->BuildString((char *) this->sqlmap_csv_parsed_data->csv[n][0]);
		tmp->BuildString((char *) this->sqlmap_csv_parsed_data->csv[n][1]);
		tmp->BuildString((char *) this->sqlmap_csv_parsed_data->csv[n][2]);
		tmp->BuildString((char *) this->sqlmap_csv_parsed_data->csv[n][3]);

		// generate hex strings
		char * target_full_unique_string = BP_BufferToMySQLHexString(tmp->build_str, bpstrlen(tmp->build_str));
		char * target_url_insert_str     = BP_BufferToMySQLHexString(this->sqlmap_csv_parsed_data->csv[n][0], bpstrlen(this->sqlmap_csv_parsed_data->csv[n][0]));
		char * place_insert_str          = BP_BufferToMySQLHexString(this->sqlmap_csv_parsed_data->csv[n][1], bpstrlen(this->sqlmap_csv_parsed_data->csv[n][1]));
		char * parameter_insert_str      = BP_BufferToMySQLHexString(this->sqlmap_csv_parsed_data->csv[n][2], bpstrlen(this->sqlmap_csv_parsed_data->csv[n][2]));
		char * technique_url_insert_str  = BP_BufferToMySQLHexString(this->sqlmap_csv_parsed_data->csv[n][3], bpstrlen(this->sqlmap_csv_parsed_data->csv[n][3]));

		// builder no longer needed
		delete tmp;

		// build the string
		builder->BuildString((char *) " ( ");

		// `result_sha1`,
		builder->BuildString((char *) " SHA1(");
		builder->BuildString(target_full_unique_string);
		builder->BuildString("), ");

		// `target_sha1`,
		builder->BuildString((char *) " SHA1( ");
		builder->BuildString(target_url_insert_str);
		builder->BuildString("), ");

		// `target_url`,
		builder->BuildString((char *) target_url_insert_str);
		builder->BuildString(", ");

		// `place`,
		builder->BuildString(place_insert_str);
		builder->BuildString(", ");

		// `parameter`,
		builder->BuildString(parameter_insert_str);
		builder->BuildString(", ");

		// `techniques`
		builder->BuildString(technique_url_insert_str);

		// build the string
		builder->BuildString((char *) " ) ");

		// free allocated space
		bpfree(target_full_unique_string);
		bpfree(target_url_insert_str);
		bpfree(place_insert_str);
		bpfree(parameter_insert_str);
		bpfree(technique_url_insert_str);

		// only add continuation comma if we have more row data
		if((n+1) < this->sqlmap_csv_parsed_data->csv_num_rows)
			builder->BuildString((char *)",");

	}

	if(builder->build_str)
	if(builder->build_str[bpstrlen(builder->build_str)-1] == ',')
		builder->build_str[bpstrlen(builder->build_str)-1] = '\0';

	// prepare the query
	// printf("\n QUERY1: %s", builder->build_str);
	this->mysqlPrepareQuery(builder->build_str);

	// execute the statement
	this->mysqlExecuteStatement();

	// destroy the statement
	this->mysqlDestroyStatement();

	// delete the builder
	delete builder;
	delete this->sqlmap_csv_parsed_data;
	this->sqlmap_csv_parsed_data = NULL;
	// return indicating success
	return ERR_SUCCESS;

}


// Post execution cleanup routine.  Call after you've duplicated any data
// you'd like to keep from the populated session.
BP_ERROR_T BP_SQLMapApplicationController::postExecutionCleanup()
{

	// return indicating success
	return ERR_SUCCESS;

}

// This method recursively searches a directory path for all csv files and attempts to insert them.
BP_ERROR_T BP_SQLMapApplicationController::parseAllSQLMapCSVInDir(char * directory_path)
{

	// ensure we have a directory path
	if(!directory_path)
		return ERR_FAILURE;

	// create new search block
	Block * path_search = new Block();

	// recursively search for csv files
	path_search->File_FindAllType(directory_path, BP_FILETYPE_REG, 1);

	// check to see that we got contents
	if(!path_search->fileList)
	{
		delete path_search;
		return ERR_FAILURE;
	}

	// exit if we have no file list
	if(path_search->fileList)
	{

		P_BP_FILE_LIST file_list = path_search->fileList->first;

		// walk file list
		for(; file_list; file_list = file_list->next)
		{
			if(!file_list->pathToFile)
				break;

			BP_REGEX_MATCH_INFO info;
			memset(&info, 0x00, sizeof(info));

			// match the regular expression
			if(BP_MatchRegularExpressionPCRE(file_list->pathToFile, strnlen(file_list->pathToFile, 4096), "\\.csv", 0, 1024, &info, 0) == ERR_SUCCESS)
			{

				// now parse csv file
				this->parseSQLMapCSVFileAndInsert(file_list->pathToFile);

			}

			// destroy the match info
			BP_DestroyRegexpMatchInfo(&info);

			// ensure we have a next pointer
			if(!file_list->next)
				break;
		}


		return ERR_FAILURE;
	}

	// destroy the block
	delete path_search;

	// return indicating success
	return ERR_SUCCESS;
}

// Parse a sqlmap binary file and store it in the database.
BP_ERROR_T BP_SQLMapApplicationController::parseSQLMapCSVFileAndInsert(char * path_to_file)
{

	// ensure we have a file path
	if(!path_to_file)
		return ERR_FAILURE;

	if(this->sqlmap_csv_parsed_data)
	{
		delete this->sqlmap_csv_parsed_data;
		this->sqlmap_csv_parsed_data = NULL;
	}

	// display message
	// printf("\n [+] Attempting to insert data from: %s", path_to_file);

	// attempt to extract contents
	this->FileGetContents(path_to_file);

	// ensure we got contents
	if(this->got_contents)
	{

		// display message
		// printf("\n [+] Extracted CSV data from file ok.  Now to parse CSV in memory.");

		// parse out csv
		sqlmap_csv_parsed_data = new BP_StringUtils(this->got_contents);

		// parse the csv
		sqlmap_csv_parsed_data->ParseCsv(true);

		// display the parsed csv data
		printf("\n [+] Parsed CSV For SQLMap File");
		sqlmap_csv_parsed_data->DisplayParsedCsv();


	}

	// insert the run results
	this->insertSQLMapBinaryRunResults();
	tq_free(this->tq, this->got_contents);
	// return indicating success
	return ERR_SUCCESS;

}

// returns the global current sql map process count
size_t BP_SQLMapApplicationController::currentNumProcessesRunning()
{

	// return the global sqlmap run count
	return global_sqlmap_run_count;
}


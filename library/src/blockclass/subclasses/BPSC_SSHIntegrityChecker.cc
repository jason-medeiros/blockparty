/*
 * BPSC_SSHIntegrityChecker.cc
 * ------------------------------------------
 * Used to run integrity checks against remote hosts using nothing but
 * SSH.  This is very useful for environments that are sensitive, but
 * are only running SSH.
 *
 *

 --  SQL FOR HOST INFO TABLE ----

 SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";

 CREATE TABLE IF NOT EXISTS `host_info` (
 `index` int(11) NOT NULL AUTO_INCREMENT,
 `name` text NOT NULL,
 `time` text NOT NULL,
 `host` text NOT NULL,
 `dir` text NOT NULL,
 `path` text NOT NULL,
 `md5` text NOT NULL,
 PRIMARY KEY (`index`)
 ) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

 *  Created on: Apr 25, 2010
 *      Author: root
 */

// include main
#include "../../../include/BP-Main.h"

// polls a table in the database looking for jobs based on a status code
BP_ERROR_T BPSC_SSHIntegrityChecker::pollQueue(BPSC_JOB_QUEUE_STATUS status, size_t min_limit, size_t max_limit)
{

	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	 * %%%%%%%% Lookup Queue Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


	char *status_str = NULL;
	switch(status)
	{
		case BPSC_JOB_QUEUE_STATUS_NEW:
			status_str = "new";
			break;
		case BPSC_JOB_QUEUE_STATUS_FINISHED:
			status_str = "finished";
			break;
		case BPSC_JOB_QUEUE_STATUS_RUNNING:
			status_str = "running";
			break;
		case BPSC_JOB_QUEUE_STATUS_WAITING:
			status_str = "waiting";
			break;
		default:
			return ERR_FAILURE;
	}

	// base sql (limits are dynamic %u's)
	char isql[2048] = {0};
	snprintf((char *) &isql, 2046, "SELECT {id}, {config_id}, {job_status} from `host_queue` where `job_status` = ? order by id desc limit %u,%u", min_limit, max_limit);

	// create encrypted sql (must be bpfree'd)
	char * sql = this->mysqlAesPrepare((char *) &isql, NULL);
	if(!sql)
		return ERR_FAILURE;

	// prepare the encrypted mysql query
	this->mysqlPrepareQuery(sql);

	// set parameters for lookup (bind prepare)
	this->mysqlAddParameter(MYSQL_TYPE_STRING, status_str, bpstrlen(status_str));
	this->mysqlBindParameters();


	// create result reciept variables
	size_t job_id    = 0;
	size_t config_id = 0;
	char job_status[1024] = {0};

	// set result parameters
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &job_id,     sizeof(job_id));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &config_id,  sizeof(config_id));
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &job_status, sizeof(job_status)-1);

	// bind parameters
	this->mysqlBindResults();

	// execute query
	this->mysqlExecuteStatement();

	// store results locally
	this->mysqlStoreResults();


	// variables required for data retrieval
	size_t counter = 0;
	P_BPSC_JOB_QUEUE_ENTRY entries = NULL;

	// fetch the results
	while(this->mysqlFetchResult())
	{

		// increment and bprealloc the data pointer
		counter++;
		entries = (P_BPSC_JOB_QUEUE_ENTRY) bprealloc(entries, sizeof(BPSC_JOB_QUEUE_ENTRY) * counter);

		// zero out new memory
		memset((char *) &entries[counter-1], 0x00, sizeof(BPSC_JOB_QUEUE_ENTRY));

		// assign variables to memory
		entries[counter-1].job_id    = job_id;
		entries[counter-1].config_id = config_id;

		// store job status in local buffer
		snprintf((char *) &entries[counter-1].status, sizeof(entries[counter-1].status)-1, "%s", (char *) &job_status);

	}


	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	 * %%%%%%%% Lookup Host Configurations For Queue %%%%%%%%%%%%
	 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */


	// now set pointers in class/free old pointers
	switch(status)
	{

		case BPSC_JOB_QUEUE_STATUS_NEW:

			if(this->new_queue)
				bpfree(this->new_queue);
			this->new_queue       = entries;
			this->new_entry_count = counter;

			break;

		case BPSC_JOB_QUEUE_STATUS_FINISHED:

			if(this->finished_queue)
				bpfree(this->finished_queue);
			this->finished_queue       = entries;
			this->finished_entry_count = counter;

			break;

		case BPSC_JOB_QUEUE_STATUS_RUNNING:

			if(this->running_queue)
				bpfree(this->running_queue);
			this->running_queue       = entries;
			this->running_entry_count = counter;

			break;

		case BPSC_JOB_QUEUE_STATUS_WAITING:

			if(this->waiting_queue)
				bpfree(this->waiting_queue);
			this->waiting_queue       = entries;
			this->waiting_entry_count = counter;

			break;

		// shouldn't ever trigger this
		default:
			return ERR_FAILURE;


	}

	// destroy/free memory
	this->mysqlDestroyStatement();
	if(sql)
		bpfree(sql);

	// return indicating success
	return ERR_SUCCESS;

}

// lookup (populate) host configurations in queue (normally called in pollQueue directly)
BP_ERROR_T BPSC_SSHIntegrityChecker::lookupHostConfigFromQueue(BPSC_JOB_QUEUE_STATUS which_queue)
{

	// variables required for data retrieval
	size_t counter = 0;
	P_BPSC_JOB_QUEUE_ENTRY entries = NULL;

	// now set pointers in class/free old pointers
	switch(which_queue)
	{

		case BPSC_JOB_QUEUE_STATUS_NEW:
			entries = this->new_queue;
			counter = this->new_entry_count;

			break;

		case BPSC_JOB_QUEUE_STATUS_FINISHED:
			entries = this->finished_queue;
			counter = this->finished_entry_count;

			break;

		case BPSC_JOB_QUEUE_STATUS_RUNNING:
			entries = this->running_queue;
			counter = this->running_entry_count;

			break;

		case BPSC_JOB_QUEUE_STATUS_WAITING:
			entries = this->waiting_queue;
			counter = this->waiting_entry_count;

			break;

		// shouldn't ever trigger this
		default:

			return ERR_FAILURE;

	}

	// index counter
	size_t i = 0;

	// walk the queues
	for(i = 0; i < counter; i++)
	{
		// LAST WORKING HERE



	}

	// return indicating success
	return ERR_SUCCESS;

}


// saves the session
BP_ERROR_T BPSC_SSHIntegrityChecker::saveSession() {

	if (!this->mysqlSession) {
		this->AddErrorToStack("Error in BPSC_SSHIntegrityChecker::saveSession(): Requires mysql session.");
		return ERR_FAILURE;
	}

	// -------------- Lookup existing id (if set) ---------------


	// Check to see if a session exists in the db matching the name/host combo
	// set in the class.
	this->mysqlPrepareQuery("select host_sessions.index from host_sessions where name = ? and host = ?;");

	// set parameters for lookup (bind)
	this->mysqlAddParameter(MYSQL_TYPE_STRING, this->session_name, bpstrlen(this->session_name));
	this->mysqlAddParameter(MYSQL_TYPE_STRING, this->host_identifier, bpstrlen(this->host_identifier));
	this->mysqlBindParameters();

	// bind results
	size_t result_index = 0;
	this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &result_index, sizeof(result_index));
	this->mysqlBindResults();

	// execute query
	this->mysqlExecuteStatement();

	// store results locally
	this->mysqlStoreResults();

	// fetch a result
	this->mysqlFetchResult();
	this->mysqlDestroyStatement();

	// set session database id
	this->session_db_id = result_index;

	// prepare query
	if (!result_index)
		this->mysqlPrepareQuery("replace into host_sessions set name = ?, host = ?, last_check_timestamp = ?, description=?");
	else
		this->mysqlPrepareQuery("update host_sessions set name = ?, host = ?, last_check_timestamp = ?, description=? where host_sessions.index = ?");

	// get current timestamp
	char timestamp_buff[64] = { 0 };
	snprintf((char *) &timestamp_buff, sizeof(timestamp_buff) - 1, "%u", time(NULL));

	// add parameters
	this->mysqlAddParameter(MYSQL_TYPE_STRING, this->session_name, bpstrlen(this->session_name));
	this->mysqlAddParameter(MYSQL_TYPE_STRING, this->host_identifier, bpstrlen(this->host_identifier));
	this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) &timestamp_buff, bpstrlen(timestamp_buff));
	this->mysqlAddParameter(MYSQL_TYPE_STRING, this->session_description, bpstrlen(this->session_description));

	// add result index if set
	if (result_index)
		this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &result_index, sizeof(result_index));

	this->mysqlBindParameters();

	// execute query
	this->mysqlExecuteStatement();
	this->mysqlDestroyStatement();

	// call it again to get the index if this is the first db insert
	if(!result_index)
		this->saveSession();

	return ERR_SUCCESS;
}

// stackable connect strings
BP_ERROR_T BPSC_SSHIntegrityChecker::addConnectString(char * connect_str, char *pass, char *key, char *key_pass, char *prompt) {

	// create new session if one doesn't exist
	if (!this->cc_session) {

		if (!this->initial_arguments) {
			this->AddErrorToStack("Error in BPSC_SSHIntegrityChecker::addConnectString(): No initial arguments set.");
			return ERR_FAILURE;
		}

		// execute application with arguments + envp
		this->File_Execute(this->initial_arguments[0], this->initial_arguments, this->initial_envp, BP_TRUE);

		if (!this->fdSet) {
			this->AddErrorToStack("Error in BPSC_SSHIntegrityChecker::addConnectString(): Could not instantiate program (bad initial arguments).");
			return ERR_FAILURE;
		}

		// set the file descriptors to non blocking io
		BP_FSetNonBlockingIO(this->fdSet);

		// create a new session based on input
		if (this->ccNewSession(this->fdSet, "ssh session", BP_CC_SHELL_LINUX_EXECVE) != ERR_SUCCESS) {
			this->AddErrorToStack("Error in BPSC_SSHIntegrityChecker::addConnectString(): New session creation failed.");
			return ERR_FAILURE;
		}

		// set error if cannot create session session
		if (!this->cc_session) {
			this->AddErrorToStack("Error in BPSC_SSHIntegrityChecker::addConnectString(): Failed to create session.");
			return ERR_FAILURE;
		}

		// set flags for display read/write off
		this->cc_session->display_reads_from_app_stdout = BP_FALSE;
		this->cc_session->display_reads_from_app_stderr = BP_FALSE;
		this->cc_session->display_writes_to_app_stdin   = BP_FALSE;

		// set poll timeout to 5 seconds
		this->cc_session->poll_timeout = 10000;
		this->cc_session->poll_timeout_stderr = 1000;

		// set connect depth as 0
		this->connect_depth = 0;

	}

	// --------------------------------------


	// Create Proper Login Credentials :: Rather Complex Logic
	char sess_name[1024] = { 0 };
	char key_tmp[1024] = { 0 };

	// string to indicate password entry
	char *pass_tmp = "assword:";

	// set connect string name
	snprintf((char *) &sess_name, sizeof(sess_name) - 1, "ssh_connect_depth_%u", this->connect_depth);

	// set passphrase expect string
	if (key) {
		snprintf((char *) &key_tmp, sizeof(key_tmp) - 1, "Enter passphrase for key *.\\:");
	}

	// if the connect string is set (non-initial connect), setup connect parameters
	if (connect_str) {

		// if key_pass is set, setup dialog
		if (key_pass) {
			// add try except (ssh host), (expect key phrase)
			this->ccAddTryExcept((char *) &sess_name, connect_str, bpstrlen(connect_str), (char*) &key_tmp, bpstrlen(key_tmp));

			// if we have a password set along with the key password, set up for that
			// (send keypass), (expect password indicator), (send password) (expect prompt)
			if (pass) {
				this->ccAddTryExcept((char*) &sess_name, key_pass, bpstrlen(key_pass), pass_tmp, bpstrlen(pass_tmp));
				this->ccAddTryExcept((char*) &sess_name, pass, bpstrlen(pass), prompt, bpstrlen(prompt));
			} else // no password: (send keypass), (expect prompt)
			{
				this->ccAddTryExcept((char*) &sess_name, key_pass, bpstrlen(key_pass), prompt, bpstrlen(prompt));
			}
		} else // (no key is set) (no key prolog required)
		{

			// if we have a password set along with the key password, set up for that
			// (send keypass), (expect password), (send password) (expect prompt)
			if (pass) {
				// add try except (ssh host), (expect key phrase)
				this->ccAddTryExcept((char*) &sess_name, connect_str, bpstrlen(connect_str), pass_tmp, bpstrlen(pass_tmp));
				this->ccAddTryExcept((char*) &sess_name, pass, bpstrlen(pass), prompt, bpstrlen(prompt));

			} else // (send nothing), (expect prompt)
			{
				this->ccAddTryExcept((char*) &sess_name, connect_str, bpstrlen(connect_str), prompt, bpstrlen(prompt));
			}

		}

	} else // (no connect string is set: identical as above except with no connect string, aka. used for initial connect
	{

		// if key_pass is set, setup dialog
		if (key_pass) {
			// add try except (ssh host), (expect key phrase)
			this->ccAddTryExcept((char*) &sess_name, 0, 0, bpstrdup((char*) &key_tmp), bpstrlen(key_tmp));

			// if we have a password set along with the key password, set up for that
			// (send keypass), (expect password indicator), (send password) (expect prompt)
			if (pass) {
				this->ccAddTryExcept((char*) &sess_name, key_pass, bpstrlen(key_pass), pass_tmp, bpstrlen(pass_tmp));
				this->ccAddTryExcept((char*) &sess_name, pass, bpstrlen(pass), prompt, bpstrlen(prompt));
			} else // no password: (send keypass), (expect prompt)
			{
				this->ccAddTryExcept((char*) &sess_name, key_pass, bpstrlen(key_pass), prompt, bpstrlen(prompt));
			}
		} else // (no key is set) (no key prolog required)
		{

			// if we have a password set along with the key password, set up for that
			// (send keypass), (expect password), (send password) (expect prompt)
			if (pass) {

				// add try except (ssh host), (expect key phrase)
				this->ccAddTryExcept((char*) &sess_name, 0, 0, pass_tmp, bpstrlen(pass_tmp));
				this->ccAddTryExcept((char*) &sess_name, pass, bpstrlen(pass), prompt, bpstrlen(prompt));

			} else // (send nothing), (expect prompt)
			{
				this->ccAddTryExcept((char*) &sess_name, 0, 0, prompt, bpstrlen(prompt));
			}

		}

	}

	// increment connect depth counter
	this->connect_depth++;

	// return indicating success
	return ERR_SUCCESS;

}


// connect through connect strings
BP_ERROR_T BPSC_SSHIntegrityChecker::connectDB()
{

	// try mysql connect first
	if (!this->mysqlSession) {

		// connect to mysql
		if (this->MysqlConnect(this->mysql_host, this->mysql_user, this->mysql_pass, this->mysql_db, 3306) == ERR_FAILURE) {
			this->AddErrorToStack("Error in BPSC_SSHIntegrityChecker::connectSSH: Cannot connect msyql and mysql unset.");
			return ERR_FAILURE;
		}

	}

	return ERR_SUCCESS;

}

// connect through connect strings
BP_ERROR_T BPSC_SSHIntegrityChecker::connectSSH() {

	// try mysql connect first
	if (!this->mysqlSession) {

		if(this->connectDB())
			return ERR_FAILURE;

	}

	// run connects
	size_t i = 0;
	char connect_name[1024] = { 0 };

	for (i = 0; i < this->connect_depth; i++) {

		// zero out memory
		memset(&connect_name, 0x00, 1024);

		// set connect depth string (for command lookup)
		snprintf((char *) &connect_name, sizeof(connect_name) - 1, "ssh_connect_depth_%u", i);

		// execute the connect string
		if (this->ccExecuteCommand((char*) &connect_name, NULL) == ERR_FAILURE) {
			this->AddErrorToStack("Error in BPSC_SSHIntegrityChecker::connectSSH(): failed at connect connect name added below.");
			this->AddErrorToStack((char*) &connect_name);
			return ERR_FAILURE;
		}

	}

	this->current_depth++;
	return ERR_SUCCESS;

}

// check integrity of directory, store in db (requires mysql connection), prompt is for expect.
BP_ERROR_T BPSC_SSHIntegrityChecker::checkMD5(char *remote_directory_root_path, char *prompt) {

	// check mysql session first
	if (!this->mysqlSession) {
		this->AddErrorToStack("Error in BPSC_SSHIntegrityChecker::connectSSH: Cannot connect msyql and mysql unset.");
		return ERR_FAILURE;
	}

	// make sure we have a root directory path
	if (!remote_directory_root_path) {
		this->AddErrorToStack("Error in BPSC_SSHIntegrityChecker::connectSSH(): no directory path set.");
		return ERR_FAILURE;
	}

	// make sure we have a command and control session attached
	if (!this->cc_session) {
		this->AddErrorToStack("Error in BPSC_SSHIntegrityChecker::connectSSH(): no command and control session.");
		return ERR_FAILURE;
	}

	// command to run
	char dir_command[4096] = { 0 };

	// set command dir
	snprintf((char *) &dir_command, sizeof(dir_command) - 1, this->md5_command, remote_directory_root_path);

	// add try/except for md5 generation
	P_BP_CC_DECISION_TREE md5_tree = this->ccCommandExists("match_md5");

	// if the tree exists, replace arguments
	if (md5_tree) {
		md5_tree->top_branch->try_this = (char *) &dir_command;
		md5_tree->top_branch->try_buff_size = bpstrlen(dir_command);
	} else // create new
	{
		this->ccAddTryExcept("match_md5", (char *) &dir_command, bpstrlen(dir_command), prompt, bpstrlen(prompt));
	}

	// execute the command
	this->ccExecuteCommand("match_md5", NULL);

	// -- Process Output ------------

	// buffer for reading lines
	char read_line[2048] = { 0 };

	// buffer for holding temporary md5 (+null terminator)
	char md5_buff[33] = { 0 };

	// buffer for holding timestamp
	char timestamp_buff[64] = { 0 };

	// byte index
	size_t i = 0;
	size_t x = 0;
	size_t j = 0;
	size_t buff_length = bpstrlen(this->cc_session->last_matched_buffer);

	// this loop actually walks the result data and extracts the md5/filename
	// results must be in the format {md5_hash}{n spaces}{path to file}{newline}
	for (i = 0; i < buff_length; i += x) {

		// exit loop if character is a null terminator
		if (this->cc_session->last_matched_buffer[i] == 0x00)
			break;

		// read till newline (x contains bpstrlen after this)
		for (x = 0; this->cc_session->last_matched_buffer[i + x] != 0x0a && this->cc_session->last_matched_buffer[i + x] != 0x00; x++) {
		}

		// if the length of the string is > 32 we have a potential hash match
		if (x > 32) {

			// zero out buffers
			memset((char *) &read_line, 0x00, sizeof(read_line));
			memset((char *) &md5_buff, 0x00, sizeof(md5_buff));
			memset((char *) &timestamp_buff, 0x00, sizeof(timestamp_buff));

			// read md5 into md5 var
			snprintf((char *) &md5_buff, 33, "%s", (char *) &this->cc_session->last_matched_buffer[i]);

			// walk md5 buffer and check to make sure its contents are hexidecimal
			for (j = 0; j < 33; j++) {

				// check if its not hexideicmal
				if (md5_buff[j] != 'a' && md5_buff[j] != 'A' && md5_buff[j] != 'b' && md5_buff[j] != 'B' && md5_buff[j] != 'c' && md5_buff[j] != 'C'
						&& md5_buff[j] != 'd' && md5_buff[j] != 'D' && md5_buff[j] != 'e' && md5_buff[j] != 'E' && md5_buff[j] != 'f' && md5_buff[j] != 'F'
						&& md5_buff[j] != 'f' && md5_buff[j] != 'F' && md5_buff[j] != '0' && md5_buff[j] != '1' && md5_buff[j] != '2' && md5_buff[j] != '3'
						&& md5_buff[j] != '4' && md5_buff[j] != '5' && md5_buff[j] != '6' && md5_buff[j] != '7' && md5_buff[j] != '8' && md5_buff[j] != '9') {
					break;
				}
			}

			// if we've matched all characters as hexidecimal, then save the remainder of the string as well
			if (j == 32) {
				// remove intermediate space
				for (j = 0; this->cc_session->last_matched_buffer[i + 32 + j] == 0x20; j++) {
				}

				// read line into local copy
				snprintf((char *) &read_line, x - (32 + j), "%s", (char *) &this->cc_session->last_matched_buffer[i + 32 + j]);

				// store timestamp
				snprintf((char *) &timestamp_buff, sizeof(timestamp_buff) - 1, "%u", time(NULL));

				// --------- Log to DB Here -------------------------------

				// prepare a query (statement)
				this->mysqlPrepareQuery("INSERT INTO `ssh_integrity`.`host_info` (`name` ,`time` ,`host` ,`dir` ,`path` ,`md5`) VALUES (?, ?, ?, ?, ?, ?);");

				// `name` ,`time` ,`host` ,`dir` ,`path` ,`md5`
				this->mysqlAddParameter(MYSQL_TYPE_STRING, this->session_name, bpstrlen(this->session_name));
				this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) &timestamp_buff, bpstrlen((char *) &timestamp_buff));
				this->mysqlAddParameter(MYSQL_TYPE_STRING, this->host_identifier, bpstrlen(this->host_identifier));
				this->mysqlAddParameter(MYSQL_TYPE_STRING, remote_directory_root_path, bpstrlen(remote_directory_root_path));
				this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) &read_line, bpstrlen((char*) &read_line));
				this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) &md5_buff, bpstrlen((char*) &md5_buff));
				this->mysqlBindParameters();

				// execute the statement
				this->mysqlExecuteStatement();

				// destroy the statement / result / parameters
				this->mysqlDestroyStatement();

			}

		}

		// moves past the newline in the string
		x++;

	}

	return ERR_SUCCESS;
}

// check command output
BP_ERROR_T BPSC_SSHIntegrityChecker::checkCommandOutput(char *command, char *prompt) {

	// check to make sure the command exists
	if (!this->ccCommandExists(command))
		this->ccAddTryExcept(command, command, bpstrlen(command), prompt, bpstrlen(prompt));

	// execute the command
	if (this->ccExecuteCommand(command, NULL) == ERR_FAILURE)
		return ERR_FAILURE;

	// -------------- Lookup existing id (if set) ---------------

	// prepare query
	this->mysqlPrepareQuery("replace host_command_output set session_id = ?, timestamp = ?, command = ?, command_output = ?");

	// get current timestamp
	char timestamp_buff[64] = { 0 };
	snprintf((char *) &timestamp_buff, sizeof(timestamp_buff) - 1, "%u", time(NULL));

	// add parameters
	this->mysqlAddParameter(MYSQL_TYPE_LONG,   (char *) &this->session_db_id, sizeof(size_t));
	this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) &timestamp_buff, bpstrlen(timestamp_buff));
	this->mysqlAddParameter(MYSQL_TYPE_STRING, command, bpstrlen(command));
	this->mysqlAddParameter(MYSQL_TYPE_STRING, this->cc_session->last_matched_buffer, bpstrlen(this->cc_session->last_matched_buffer));

	// bind parameters
	this->mysqlBindParameters();

	// execute query
	this->mysqlExecuteStatement();
	this->mysqlDestroyStatement();

	return ERR_SUCCESS;

}

// exit out
BP_ERROR_T BPSC_SSHIntegrityChecker::finishSSH() {

	if (!this->ccCommandExists("exit"))
		this->ccAddTryExcept("exit", this->exit_string, bpstrlen(this->exit_string), NULL, 0);

	// run the exit command n-times reducing depth on each
	size_t i = 0;
	for (i = 0; i < this->current_depth; i++) {
		this->ccExecuteCommand("exit", NULL);
		this->current_depth--;
	}

	// set depth as zero
	this->connect_depth = 0;
	return ERR_SUCCESS;
}

// attempts to sudo on the current host and expect a prompt
size_t BPSC_SSHIntegrityChecker::doSudo(char *pass, char *sudo_expect_prompt)
{

	if (!pass || !sudo_expect_prompt) {
		this->AddErrorToStack("Error in BPSC_SSHIntegrityChecker::doSudo(): bad parameters to function.");
		return ERR_FAILURE;
	}

	// add try/except for sudo
	this->ccAddTryExcept("sudo", this->sudo_path, bpstrlen(this->sudo_path), ":", 1);
	this->ccAddTryExcept("sudo", pass, bpstrlen(pass), sudo_expect_prompt, bpstrlen(sudo_expect_prompt));
	this->current_depth++;

	// execute the command on the remote host
	return this->ccExecuteCommand("sudo", NULL);

}

// attempts to switch user (su) on current host and expect a prompt
size_t BPSC_SSHIntegrityChecker::doSu(char *user, char *pass, char *su_expect_prompt) {

	if (!pass || !su_expect_prompt) {
		this->AddErrorToStack("Error in BPSC_SSHIntegrityChecker::doSu(): bad parameters to function.");
		return ERR_FAILURE;
	}

	// add try/except for sudo
	this->ccAddTryExcept("su", this->su_path, bpstrlen(this->su_path), ":", 1);
	this->ccAddTryExcept("su", pass, bpstrlen(pass), su_expect_prompt, bpstrlen(su_expect_prompt));

	// execute the command on the remote host
	return this->ccExecuteCommand("su", NULL);

}


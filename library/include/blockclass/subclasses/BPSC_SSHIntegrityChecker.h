/*
 * BPSC_SSHIntegrityChecker.h
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

--
-- Dumping data for table `host_info`
--

 *
 *  Created on: Apr 25, 2010
 *      Author: Jason Medeiros\
 *
 */


#ifndef BPSC_SSHINTEGRITYCHECKER_H_
#define BPSC_SSHINTEGRITYCHECKER_H_

/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%%%%%%%%%%% Job Queue Structures %%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// job status
enum BPSC_JOB_QUEUE_STATUS {
	BPSC_JOB_QUEUE_STATUS_RUNNING,
	BPSC_JOB_QUEUE_STATUS_NEW,
	BPSC_JOB_QUEUE_STATUS_WAITING,
	BPSC_JOB_QUEUE_STATUS_FINISHED
};

// job queue structure (for retrieving jobs from the database)
typedef struct _BPSC_JOB_QUEUE_ENTRY {
	size_t job_id;
	size_t config_id;
	char   status[512];
} BPSC_JOB_QUEUE_ENTRY, *P_BPSC_JOB_QUEUE_ENTRY;


/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%%%%%%%% Host Configuration Structures %%%%%%%%%%%%%%%%%%%%
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


// Virtual Host root path configuration (pointers are malloced)
typedef struct _BPSC_HOST_CONFIG_VHOST_ROOT_PATH {

	// filesystem path
	char * filesystem_path;

	// web_request_path
	char * web_request_path;

} BPSC_HOST_CONFIG_VHOST_ROOT_PATH, *P_BPSC_HOST_CONFIG_VHOST_ROOT_PATH;


// Virtual host configuration (for virtual webservers on the host)
typedef struct _BPSC_HOST_CONFIG_VHOST {

	// id for this vhost in the database
	size_t db_id;

	// configuration id
	size_t config_id;

	// virtual host configured paths
	P_BPSC_HOST_CONFIG_VHOST_ROOT_PATH paths;

	// number of paths in the above path array
	size_t path_count;

} BPSC_HOST_CONFIG_VHOST, *P_BPSC_HOST_CONFIG_VHOST;


// Host Configuration
typedef struct _BPSC_HOST_CONFIG {

	// database identifier for this configuration
	size_t db_id;

	// user database, db identifier (used to identify which user this record belongs to)
	size_t user_id;


	// description of the host
	char *host_description;

	// host IP address
	char *host_ip_address;

	// enabled / disabled (yes/no/priority/delayed/freeuser)
	size_t enabled;


	// priority string (pay/paypriority/free/freepriority)
	char *priority;

	// pay run count
	size_t pay_run_count;
	size_t paypriority_run_count;

	// free run count
	size_t free_run_count;
	size_t free_priority_run_count;


} BPSC_HOST_CONFIG, *P_BPSC_HOST_CONFIG;


/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%%%%%%%%%%%%%%% Class Definition %%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

// class definition
class BPSC_SSHIntegrityChecker : public Block {

	 public:


	// ----- MySQL Database Parameters -------------

	// mysql host/user/pass
	char *mysql_host;
	char *mysql_user;
	char *mysql_pass;

	// database to use
	char *mysql_db;

	// table that meets the sql layout as described at the top of this file
	char *mysql_table;

	// password string that is used for mysql AES encryption
	char *mysql_aes_encrypt_key;

	// connect through connect strings
	BP_ERROR_T connectDB();

	// ----- SSH Parameters ------------------

	// since ssh is a whore, we use an expect script that wraps it.  This is the scripts path.
	char *path_to_ssh_script;

	// for initial ssh connect, ssh data is here
	char **initial_arguments;

	// initial envp
	char **initial_envp;


	// string to use for exiting one level of shell depth
	char *exit_string;


	// session name (name in db)
	char *session_name;

	// session description
	char *session_description;

	// session db identifier (set after session is saved with saveSession())
	size_t session_db_id;

	// host identifier (used for database inserts)
	char *host_identifier;




	// number of ssh connections deep current connection is
	size_t connect_depth;

	// current depth of ssh connections (ssh-to-ssh)
	size_t current_depth;

	// command to use for md5 generation
	char *md5_command;


	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	 * %%%%%%%% Job Queue/Polling Capability %%%%%%%%%%%%%%%%%%%%
	 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

	// queue structures
	P_BPSC_JOB_QUEUE_ENTRY running_queue;
	size_t running_entry_count;

	P_BPSC_JOB_QUEUE_ENTRY new_queue;
	size_t new_entry_count;

	P_BPSC_JOB_QUEUE_ENTRY waiting_queue;
	size_t waiting_entry_count;

	P_BPSC_JOB_QUEUE_ENTRY finished_queue;
	size_t finished_entry_count;

	// polls the database queue for a given type
	BP_ERROR_T pollQueue(BPSC_JOB_QUEUE_STATUS status, size_t min_limit, size_t max_limit);

	// lookup (populate) host configurations in queue (normally called in pollQueue directly)
	BP_ERROR_T lookupHostConfigFromQueue(BPSC_JOB_QUEUE_STATUS which_queue);


	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	 * %%%%%%%% Configuration Retrieval %%%%%%%%%%%%%%%%%%%%%%%%%
	 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


	// save session
	size_t session_saved;
	BP_ERROR_T saveSession();

	// stackable connect strings
	BP_ERROR_T addConnectString(char * connect_str, char *pass, char *key, char *key_pass, char *prompt);

	// connect through connect strings
	BP_ERROR_T connectSSH();

	// check integrity of directory, store in db (requires mysql connection)
	BP_ERROR_T checkMD5(char *remote_directory_root_path, char *prompt);

	// check command output
	BP_ERROR_T checkCommandOutput(char *command, char *prompt);

	// exit out
	BP_ERROR_T finishSSH();


	// ------------ Sudo/SU Interface ---------------------------

	// binary paths to sudo or su binaries for this current/depth/session
	char *sudo_path;
	char *su_path;

	// attempts to sudo on the current host and expect a prompt
	size_t doSudo(char *pass, char *sudo_expect_prompt);

	// attempts to switch user (su) on current host and expect a prompt
	size_t doSu(char *user, char *pass, char *su_expect_prompt);



 };

#endif /* BPSC_SSHINTEGRITYCHECKER_H_ */

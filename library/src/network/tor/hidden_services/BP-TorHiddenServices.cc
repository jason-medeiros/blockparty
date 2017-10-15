/*
 * BP-TorHiddenServices.cc
 *
 *  Created on: Dec 18, 2015
 *      Author: root
 */

// include main
#include "../../../../include/BP-Main.h"


// Attempts to create a hidden service on a given port provided a dir/port/bind/port set.
P_BP_TOR_HIDDEN_SERVICE BP_TorHiddenServiceCreate
(
	char         * service_dir,
	unsigned short service_port,
	char         * bind_local_addr,
	unsigned short bind_local_port,
	unsigned short tor_socks_port
)
{

	// ensure we have basic parameters
	if(!service_dir)
		return NULL;
	if(!service_port)
		return NULL;
	if(!bind_local_addr)
		return NULL;
	if(!bind_local_port)
		return NULL;


	// if the service port isn't open, we fail
	if(!BP_NetTCPPortIsOpen("127.0.0.1", bind_local_port))
	{
		printf("\n [!!] Tor service local bind port is open.");
		return NULL;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Extended Parameter Checks %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure the string is reasonable for an ext4 directory
	if(!BP_StringIsReasonableForUnixEXT4Directory(service_dir, bpstrlen(service_dir)))
		return NULL;

	// ensure the bind address is an ipv4 address
	if(!BP_StringIsIPv4Address(bind_local_addr))
		return NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Paths %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// individual path settings
	char * path_to_hostname_file_path    = NULL;
	char * path_to_private_key_file_path = NULL;
	char * path_to_torrc_file_path       = NULL;

	// create new joiner tq
	P_BP_LINKL tq = NULL;


	// --- create hostname file path -------------------

	tq = BP_CreateLinkL(".", BP_TRUE);

	// set service directory into place
	tq_strdup(tq, service_dir, BPLN);
	tq_strdup(tq, "hostname", BPLN);

	// flatten
	BP_LinkLFlattenJoinStringElements(tq);

	// set path from flat buffer
	path_to_hostname_file_path = (char *) bpstrdup((char *) tq->flat_buff);

	// destroy the linkl after joining strings
	BP_DestroyLinkL(tq, BP_TRUE);



	// --- create private key file path -------------------

	tq = BP_CreateLinkL(".", BP_TRUE);

	// set service directory into place
	tq_strdup(tq, service_dir, BPLN);
	tq_strdup(tq, "private_key", BPLN);

	// flatten
	BP_LinkLFlattenJoinStringElements(tq);

	// set private key path
	path_to_private_key_file_path = (char *) bpstrdup((char *) tq->flat_buff);

	// destroy the linkl after joining strings
	BP_DestroyLinkL(tq, BP_TRUE);



	// --- create torrc file path -------------------

	tq = BP_CreateLinkL(".", BP_TRUE);

	// set service directory into place
	tq_strdup(tq, service_dir, BPLN);
	tq_strdup(tq, "torrc", BPLN);

	// flatten
	BP_LinkLFlattenJoinStringElements(tq);

	// set torrc path
	path_to_torrc_file_path = (char *) bpstrdup((char *) tq->flat_buff);

	// destroy the linkl after joining strings
	BP_DestroyLinkL(tq, BP_TRUE);


	if
	(
		!path_to_hostname_file_path    ||
		!path_to_private_key_file_path ||
		!path_to_torrc_file_path
	)
	{
		if(path_to_hostname_file_path)
			bpfree(path_to_hostname_file_path);
		if(path_to_private_key_file_path)
			bpfree(path_to_private_key_file_path);
		if(path_to_torrc_file_path)
			bpfree(path_to_torrc_file_path);


		// return null for failure if any couldn't be created
		return NULL;

	}




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create torrc file content %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// torrc buffer
	char * torrc_buff = (char *) bpcalloc(bpstrlen(service_dir) + 2048, 1);

	// create the torrc buffer
	sprintf
	(
			torrc_buff,
			"SocksPort %u\n"                      \
			"SocksBindAddress 127.0.0.1\n"        \
			"HiddenServiceDir %s\n"               \
			"HiddenServicePort %u 127.0.0.1:%u",
			tor_socks_port,
			service_dir,
			bind_local_port,
			service_port
	);

	// set the content length
	size_t torrc_content_len = bpstrlen(torrc_buff);

	// ensure we have a torrc
	if(!torrc_buff && !torrc_content_len)
		return NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Populate Service Information %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to create new hash table registry
	P_BP_HASH_TABLE_REGISTRY system_hreg = BP_CreateHashTableRegistry();
	if(!system_hreg)
		return NULL;

	// attempt to create the hidden service
	P_BP_TOR_HIDDEN_SERVICE hidden_service = (P_BP_TOR_HIDDEN_SERVICE) ht_calloc
	(
			system_hreg,
			sizeof(BP_TOR_HIDDEN_SERVICE),
			1
	);

	// ensure we could allocate the service ok
	if(!hidden_service)
		return NULL;

	// set the system hreg
	hidden_service->system_hreg = system_hreg;

	// generate a random uuid
	char *tmp_uuid = BP_GenRandomUUID();

	// set the uuid
	hidden_service->uuid = ht_strdup(system_hreg, tmp_uuid);

	// destroy the uuid after assignment/duplication
	bpfree(tmp_uuid);

	// set the service dir and port
	hidden_service->service_dir  = ht_strdup(system_hreg, service_dir);
	hidden_service->service_port = service_port;

	// set bind local and port
	hidden_service->bind_local_addr = ht_strdup(system_hreg, bind_local_addr);
	hidden_service->bind_local_port = bind_local_port;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Torrc %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set torrc file path
	hidden_service->torrc_file_path = ht_strdup(system_hreg, path_to_torrc_file_path);

	// set content
	hidden_service->torrc_content   = ht_strdup(system_hreg, torrc_buff);
	if(torrc_content_len)
		bpfree(torrc_buff);

	// set buffer length
	hidden_service->torrc_content_len = torrc_content_len;


	// if the file exists and is readable, set flag indicating so
	if(BP_FileAtPathExistsAndIsReadable(hidden_service->torrc_file_path))
		hidden_service->torrc_file_exists = BP_TRUE;

	// always try to create the hidden service directory, even if it fails
	BP_FSDirectoryCreate(hidden_service->service_dir, 0600);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Secondary Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if the torrc exists on the filesystem, read it in and run some checks.
	char   *torrc_from_filesystem     = 0;
	size_t torrc_from_filesystem_size = 0;

	// if the directory exists, run checks
	if(BP_FSDirectoryExists(service_dir))
	{

		// if the torrc file exists, attempt to load and compare it to our current buffer
		if(hidden_service->torrc_file_exists)
		{

			// attempt to get the torrc file path
			torrc_from_filesystem_size = BP_FGetSize(hidden_service->torrc_file_path);
			if(torrc_from_filesystem_size > BP_TORRC_MAX_FILE_SIZE)
			{

				// destroy the tor service structures before returning
				BP_TorHiddenServiceDestroy(hidden_service);
				return NULL;

			}

			// attempt to load the content from the filesystem
			torrc_from_filesystem = BP_FileGetContents(hidden_service->torrc_file_path);
			if(!torrc_from_filesystem)
			{
				// destroy the tor service structures before returning
				BP_TorHiddenServiceDestroy(hidden_service);
				return NULL;
			}

			// ensure returned content is reasonable printable
			if(!BP_StringIsReasonablePrintableString(torrc_from_filesystem, bpstrlen(torrc_from_filesystem), BP_TRUE, BP_TRUE))
			{
				// destroy the tor service structures before returning
				BP_TorHiddenServiceDestroy(hidden_service);
				return NULL;
			}
		}


	}

	// if there is no torrc from the filesystem, attempt to write current as torrc
	if(!torrc_from_filesystem)
	{



		// create new torrc file
		if
		(
			!BP_FilePutContents
			(
					hidden_service->torrc_file_path,
					hidden_service->torrc_content,
					hidden_service->torrc_content_len,
					BP_FALSE
			)
		)
		{
			// destroy the tor service structures before returning
			BP_TorHiddenServiceDestroy(hidden_service);
			return NULL;
		}

		// if the file exists and is readable, set flag indicating so
		if(BP_FileAtPathExistsAndIsReadable(hidden_service->torrc_file_path))
			hidden_service->torrc_file_exists = BP_TRUE;

	}
	else
	{

		// flag for indicating if content doesn't match
		BP_BOOL torrc_content_doesnt_match = BP_FALSE;

		// check length first, then content, mark mismatch where they occur
		if(torrc_from_filesystem_size != hidden_service->torrc_content_len)
		{
			torrc_content_doesnt_match = BP_TRUE;
		}
		else if
		(
			memcmp
			(
					hidden_service->torrc_content,
					torrc_from_filesystem,
					torrc_from_filesystem_size
			) != 0
		)
		{
			torrc_content_doesnt_match = BP_TRUE;
		}

		// overwrite old content if things differ
		if(torrc_content_doesnt_match)
		{

			// create new torrc file
			if
			(
				!BP_FilePutContents
				(
						hidden_service->torrc_file_path,
						hidden_service->torrc_content,
						hidden_service->torrc_content_len,
						BP_FALSE
				)
			)
			{
				// destroy the tor service structures before returning
				BP_TorHiddenServiceDestroy(hidden_service);
				return NULL;
			}

		}

		// if the file exists and is readable, set flag indicating so
		if(BP_FileAtPathExistsAndIsReadable(hidden_service->torrc_file_path))
			hidden_service->torrc_file_exists = BP_TRUE;


	}

	// return the hidden service configuration
	return hidden_service;

}



// attempts to start a tor hidden service
BP_ERROR_T BP_TorHiddenServiceStart
(
	P_BP_TOR_HIDDEN_SERVICE tor_service
)
{

	// run basic null checks
	if(!tor_service)
		return ERR_FAILURE;
	if(!tor_service->bind_local_addr)
		return ERR_FAILURE;
	if(!tor_service->bind_local_port)
		return ERR_FAILURE;
	if(!tor_service->service_dir)
		return ERR_FAILURE;
	if(!tor_service->service_port)
		return ERR_FAILURE;
	if(!tor_service->torrc_file_exists)
		return ERR_FAILURE;

	// dbg: vebosity
	printf("\n [+] Attempting to start service!");

    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Exec Parameters and Execute Binary %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set execution path
	char * tor_exec_path = "/usr/sbin/tor";

	// set argv for application.
	char *tor_params[] =
	{
		 tor_exec_path,
		 "-f",
		 "./service/torrc",
		 NULL
	};

	// exec the file (don't set envp)
	P_BP_FDSET tor_app = BP_FExec
	(
		 tor_exec_path,
		 (char **) &tor_params,
		 NULL /* envp*/,
		 BP_TRUE
	);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% 1) Create Basic CC Session To Start TOR Service %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set cc session
	P_BP_CC_SESSION tor_cc_session = BP_CCNewSession
	(
		 tor_app,
		 "command_control_interface_ut",
		 BP_CC_SHELL_LINUX_EXECVE
	);

	// display reads and writes (values can be changed inbetween
	// command runs to disable output for certain commands, or
	// enable output for others)
	tor_cc_session->display_reads_from_app_stdout = BP_TRUE;
	tor_cc_session->display_reads_from_app_stderr = BP_TRUE;
	tor_cc_session->display_writes_to_app_stdin   = BP_FALSE;


	// Set stdout polling timeout (values are additive and can be reset
	// between dispatched commands to change the select() responsiveness
	// of the app being controlled.)
	tor_cc_session->select_child_stdout_timeout_sec  = 3;
	tor_cc_session->select_child_stdout_timeout_usec = 500;

    // declare return value
    BP_ERROR_T ret_val = ERR_FAILURE;



    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% 2) Add Auth Try/Expect Logic %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // Add some arbitrary commands to the command orchard.
    BP_CCAddCommand(tor_cc_session, "start_and_monitor_tor");

    // set the expected string to the tor bootstrap message
    char *tor_bootstrap_str = "Bootstrapped 100%: Done.";

    // Add some try/expect branches to some commands
    ret_val = BP_CCAddTryExpectToCommand
    (
            tor_cc_session,
            "start_and_monitor_tor",
            NULL,
            0,
			tor_bootstrap_str,
            bpstrlen(tor_bootstrap_str),
            BP_CC_MATCH_BUFF_POLICY_MEM_SEARCH
    );

    // attempt to detect password field
    if(!BP_CCDispatcher(tor_cc_session, "start_and_monitor_tor", NULL))
    {
            printf("\n [!!] Dispatcher failed for start_and_monitor_tor.");
            exit(0);
    }



    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Display CC Information After %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // attempt to visualize session
    BP_CCVisualizeSession(tor_cc_session, BP_TRUE, BP_TRUE);




	// return indicating success
	return ERR_SUCCESS;

}


// This attempts to create a hidden service using an already existing configuration.
BP_ERROR_T BP_TorHiddenServiceStartFromFilesystemTorrc
(
	P_BP_TOR_HIDDEN_SERVICE tor_service,
	char                  * path_to_torrc
)
{

	// ensure we have a tor service
	if(!tor_service)
		return ERR_FAILURE;

	// ensure we have a torrc
	if(!path_to_torrc && tor_service->torrc_file_path)
		return ERR_FAILURE;

	printf("\n Got torrc file path?: %s", tor_service->torrc_file_path);
	// return indicating success
	return ERR_SUCCESS;

}




// Attempts to stop a tor hidden service after its been started by stopping the tor
// application and closing all filehandles safely.
BP_ERROR_T BP_TorHiddenServiceStop(P_BP_TOR_HIDDEN_SERVICE tor_service)
{


	// return indicating success
	return ERR_SUCCESS;

}

// Attempt to destroy the tor hidden service, freeing all memory associated
// with the service.
BP_ERROR_T BP_TorHiddenServiceDestroy(P_BP_TOR_HIDDEN_SERVICE tor_service)
{

	// ensure we have a service to destroy
	if(!tor_service)
		return ERR_FAILURE;
	if(!tor_service->system_hreg)
		return ERR_FAILURE;


	// destroy the hash table registry (free'ing all memory)
	BP_DestroyHashTableRegistry(tor_service->system_hreg);


	// return indicating success
	return ERR_SUCCESS;

}

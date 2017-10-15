/*
 * BP-TorHiddenServices.h
 *
 *  Created on: Dec 18, 2015
 *      Author: root
 */

#ifndef INCLUDE_NETWORK_TOR_HIDDEN_SERVICES_BP_TORHIDDENSERVICES_H_
#define INCLUDE_NETWORK_TOR_HIDDEN_SERVICES_BP_TORHIDDENSERVICES_H_


// Set an arbitrary torrc max file size.  This is to prevent torrc's from ever getting too large.
#define BP_TORRC_MAX_FILE_SIZE 8000


// This information is set when you try to open a service.  Can also be refreshed
// directly.
typedef struct __BP_TOR_HIDDEN_SERVICE_RUNNING_INFO {

	// checks to see if
	BP_BOOL service_is_running;

	// check if the local port is open for our sercie
	BP_BOOL local_port_is_open;

	// check if tor port is open
	BP_BOOL tor_port_is_open;

	// tor configuration file exists
	BP_BOOL tor_config_file_exits;

	// pid file xists
	BP_BOOL tor_pid_file_exists;

} BP_TOR_HIDDEN_SERVICE_RUNNING_INFO, *P_BP_TOR_HIDDEN_SERVICE_RUNNING_INFO;


// tor hidden service
typedef struct __BP_TOR_HIDDEN_SERVICE {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Basic Local Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// uuid of this tor service connection
	char *uuid;

	// pid of the running tor process
	pid_t pid;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Basic Tor Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Directory tor will use to keep track of service information.  Must be valid directory.
	char *service_dir;

	// port to map service to externally
	unsigned short service_port;

	// local address to bind to
	char * bind_local_addr;

	// local port to bind to
	unsigned short bind_local_port;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%% Torrc File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// path to the torrc file
	char * torrc_file_path;

	// file exists
	BP_BOOL torrc_file_exists;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Torrc Buffer %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// content of the torrc file
	char * torrc_content;

	// length of the torrc buffer
	size_t torrc_content_len;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Running Info %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// running info flags
	BP_TOR_HIDDEN_SERVICE_RUNNING_INFO run_info;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% System Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// hash table registry used for allocations and whatnot
	P_BP_HASH_TABLE_REGISTRY system_hreg;


} BP_TOR_HIDDEN_SERVICE, *P_BP_TOR_HIDDEN_SERVICE;


// Attempts to create a hidden service on a given port provided a dir/port/bind/port set.
P_BP_TOR_HIDDEN_SERVICE BP_TorHiddenServiceCreate
(
	char         * service_dir,
	unsigned short service_port,
	char         * bind_local_addr,
	unsigned short bind_local_port,
	unsigned short tor_socks_port = 9050
);


// attempts to start a tor hidden service
BP_ERROR_T BP_TorHiddenServiceStart(P_BP_TOR_HIDDEN_SERVICE tor_service);


// This attempts to create a hidden service using an already existing configuration.
BP_ERROR_T BP_TorHiddenServiceStartFromFilesystemTorrc
(
	P_BP_TOR_HIDDEN_SERVICE tor_service,
	char                  * path_to_torrc
);

// Attempts to stop a tor hidden service after its been started by stopping the tor
// application and closing all filehandles safely.
BP_ERROR_T BP_TorHiddenServiceStop(P_BP_TOR_HIDDEN_SERVICE tor_service);

// Attempt to destroy the tor hidden service, freeing all memory associated
// with the service.
BP_ERROR_T BP_TorHiddenServiceDestroy(P_BP_TOR_HIDDEN_SERVICE tor_service);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Service Setup %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Check Service %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to refresh running info
BP_ERROR_T BP_TorHiddenServiceRefreshRunInfo(P_BP_TOR_HIDDEN_SERVICE tor_service);



#endif /* INCLUDE_NETWORK_TOR_HIDDEN_SERVICES_BP_TORHIDDENSERVICES_H_ */

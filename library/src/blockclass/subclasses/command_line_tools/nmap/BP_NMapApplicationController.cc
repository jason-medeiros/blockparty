/*
 * BP_NMapApplicationController.c
 *
 *  Created on: Aug 30, 2013
 *      Author: root
 */

// NOTE: SEE RELATED BP_NMapApplicationController.h FILE FOR
//       INFORMATION RELATED TO HOW TO USE THIS INTERFACE.


// include the blockparty main include
#include "../../../../../include/BP-Main.h"

// Counter indicating how many nmap classes have started
size_t global_nmap_run_count = 0;


// contructorstructor
BP_NMapApplicationController::BP_NMapApplicationController(char * path_to_nmap_binary): BP_CommandLineToolBase(path_to_nmap_binary)
{

	// set null
	this->nmap_xml_parser = NULL;

	// this is flagged after the app exited
	this->nmap_exited = 0;

	// set output file to null
	this->nmap_output_file = NULL;

	// create a new linked list
	this->nmap_m = BP_CreateLinkL("nmap application allocator", BP_TRUE);

	// set defaults
	this->host_subsections   = NULL;
	this->host_subsections_n = 0;

}

// deconstructor
BP_NMapApplicationController::~BP_NMapApplicationController()
{

	// destroy output file if necessary
	if(	this->nmap_output_file )
	{
		tq_free(this->nmap_m, this->nmap_output_file);
		this->nmap_output_file = NULL;
	}

	// destroy the memory allocator
	BP_DestroyLinkL(this->nmap_m, BP_TRUE);

	// set allocator pointer to null
	this->nmap_m = NULL;

}

// =====================================


// Execute nmap.  This will populate some local data, so before
// calling it recursively, call the cleanup routine below, or just delete
// and recreate the object.  Performance hits should be negligable either way.
BP_ERROR_T BP_NMapApplicationController::executeNMapBinary()
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

		printf("\n [!!] Failed to execute nmap!");
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
	global_nmap_run_count++;

	// set nonblocking io on the program
	BP_FSetNonBlockingIO(this->fdSet);

	// create new cc session
	this->ccNewSession(this->fdSet, "NMap Command And Control Module", BP_CC_SHELL_LINUX_EXECVE);

	// set monitoring flags
	this->cc_session->display_reads_from_app_stdout = BP_TRUE;
	this->cc_session->display_reads_from_app_stderr = BP_TRUE;
	this->cc_session->display_writes_to_app_stdin   = BP_TRUE;

	// set the poll timeout (25m)
	this->cc_session->poll_timeout        = 60000 * 25;
	this->cc_session->poll_timeout_stderr = 60000 * 25;

	// expect the shutdown and exit after
	this->ccAddTryExcept("start_nmap", 0, 0, "Nmap done", bpstrlen("Nmap done"));

	// add premature exit on errors
	this->ccAddPrematureExitPCRE("Nmap done");

	// run command test
	this->ccExecuteCommand("start_nmap", NULL);

	// close file descriptors
	close(this->fdSet->stderr[0]);
	close(this->fdSet->stderr[1]);
	close(this->fdSet->stdin [0]);
	close(this->fdSet->stdin [1]);
	close(this->fdSet->stdout[0]);
	close(this->fdSet->stdout[1]);


	// Check to see if we need to kill the process (uses out of
	// process kill to replicate actual runtime kill which may be
	// expected by process, also prevents any undefined in-process behavior)

	do
	{

		// ensure we have a non-zero non-error pid
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
	global_nmap_run_count--;


	// Parse the buffer matched from information extraction out to a csv file which we
	// can read from.
	if(this->cc_session->info_searches)
	if(!this->cc_session->info_searches[0].match_strs_n || !this->cc_session->info_searches[0].match_strs)
	{

		// no matches means no file exported
		return ERR_FAILURE;

	}

	// return indicating success
	return ERR_SUCCESS;

}

// Sets the output file.
BP_ERROR_T BP_NMapApplicationController::setNMapOutputXMLFile(char * nmap_xml_file)
{

	if(!nmap_xml_file)
		return ERR_FAILURE;

	// destroy nmap output if necessary
	if(this->nmap_output_file)
	{

		// destroy the file if so set
		tq_free(this->nmap_m, this->nmap_output_file);
		this->nmap_output_file = NULL;

	}

	// set the output file
	this->nmap_output_file = (char *) tq_strdup(this->nmap_m, nmap_xml_file, BPLN);


	// return indicating success
	return ERR_SUCCESS;

}

// Parses the output XML file.
BP_ERROR_T BP_NMapApplicationController::parseNMapOutputXMLFile()
{

	// ensure we have an output file to parse
	if(!this->nmap_output_file)
		return ERR_FAILURE;

	// destroy xml parser if we already have one
	if(this->nmap_xml_parser)
	{
		delete this->nmap_xml_parser;
		this->nmap_xml_parser = NULL;
	}

	// create new string builder
	BP_StringUtils * xml_parser = new BP_StringUtils(NULL);

	// attempt to parse the file
	xml_parser->xmlParseFile(this->nmap_output_file);

	// exit if the xml wasn't parsed ok
	if(!xml_parser->xml_parsed_ok)
	{

		// destroy all the xml parser data
		xml_parser->xmlDestroyAll();
		delete xml_parser;
		return ERR_FAILURE;

	}


	// populate root node element
	xml_parser->xmlGetRootNodeElement();

	// display retrieved root node
	printf("\n [+] Got root node: %p", xml_parser->xml_root_node);

	// set current node as root node (used for iterations)
	xml_parser->xml_curr_node = xml_parser->xml_root_node;

	// set current node as children (top node is basically empty; gotta move one down)
	xml_parser->xml_curr_node = xml_parser->xml_curr_node->children;

	// loop xml node elements
	while ( xml_parser->xmlGetNextNodeElement() )
	{


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Node Type Detection Logic %%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// skip non-node types
		if(xml_parser->xml_curr_node->type != 1)
			continue;

		// process nodes via switch
		switch
		(
			this->xmlGetTopLevelNodeTypeByName((char *) xml_parser->xml_curr_node->name)
		)
		{

			case BP_NMAP_XML_NODE_TYPE_UNSET:
				printf("\n [+] ERR: Node type unset: %s", xml_parser->xml_curr_node->name);
				break;

			case BP_NMAP_XML_NODE_TYPE_SCANINFO:
				printf("\n [+] SCANINFO Parse:");
				this->parseNMap_SCANINFO(xml_parser->xml_curr_node);
				break;

			case BP_NMAP_XML_NODE_TYPE_VERBOSE:
				printf("\n [+] VERBOSE Parse:");
				this->parseNMap_VERBOSE(xml_parser->xml_curr_node);
				break;

			case BP_NMAP_XML_NODE_TYPE_DEBUGGING:
				printf("\n [+] DEBUGGING Parse:");
				this->parseNMap_DEBUGGING(xml_parser->xml_curr_node);
				break;

			case BP_NMAP_XML_NODE_TYPE_HOST:
				printf("\n [+] HOST Parse:");

				// allocate space for subsections
				this->host_subsections_n++;
				this->host_subsections = (P_BP_NMAP_HOST *) tq_realloc(this->nmap_m, this->host_subsections, sizeof(P_BP_NMAP_HOST) * (this->host_subsections_n+1), BPLN);
				this->host_subsections[this->host_subsections_n-1] = this->parseNMap_HOST(xml_parser->xml_curr_node);
				break;

			case BP_NMAP_XML_NODE_TYPE_RUNSTATS:
				printf("\n [+] RUNSTATS Parse:");
				this->parseNMap_RUNSTATS(xml_parser->xml_curr_node);
				break;

			// default / do-nothing case
			default:
				break;
		}

		/*

		// display properties if we have any
		printf("\n\t [+] Curr Node Properties: ");

		// print individual properties
		if(xml_parser->xml_curr_node->properties)
		{
			xml_parser->xmlPrintNodeProperty(xml_parser->xml_curr_node->properties, 1);
		}

		// sort through children nodes
		for(; xml_parser->xml_curr_node->children; xml_parser->xml_curr_node->children = xml_parser->xml_curr_node->children->next)
		{

			// check the node type
			if(xml_parser->xml_curr_node->children->type != XML_ELEMENT_NODE)
				continue;

			// print the current child node
			printf("\n\t\t [+] Child Node:");
			xml_parser->xmlPrintNode(xml_parser->xml_curr_node->children, 2);

			for(; xml_parser->xml_curr_node->children->properties; xml_parser->xml_curr_node->children->properties = xml_parser->xml_curr_node->children->properties->next)
			{

				// display properties
				xml_parser->xmlPrintNodeProperty(xml_parser->xml_curr_node->children->properties, 3);

				// display child properties if so necessar
				if(xml_parser->xml_curr_node->children->properties->children)
				if(xml_parser->xml_curr_node->children->properties->children->type == 1)
					xml_parser->xmlPrintNode(xml_parser->xml_curr_node->children->properties->children, 4);

			}

		}
		*/
	}


	// print some truncating newlines
	printf("\n\n");

	// populate root node element
	xml_parser->xmlGetRootNodeElement();




	printf("\n [+] Parsed NMAP Output OK!");


	// return indicating success
	return ERR_SUCCESS;

}



// ==== Sub-section parsers =======



// SCANINFO section parser (parses from scan info top)
BP_ERROR_T BP_NMapApplicationController::parseNMap_SCANINFO(xmlNode * scaninfo)
{

	printf("\n Parsing Scan Info?: %p", scaninfo);

	// return indicating success
	return ERR_SUCCESS;

}

// VERBOSE section parser
BP_ERROR_T BP_NMapApplicationController::parseNMap_VERBOSE(xmlNode * verbose)
{

	printf("\n Parsing Verbose Info?: %p", verbose);

	// return indicating success
	return ERR_SUCCESS;
}

// DEBUGGING section parser
BP_ERROR_T BP_NMapApplicationController::parseNMap_DEBUGGING(xmlNode * debugging)
{

	printf("\n Parsing Debugging Info?: %p", debugging);
	// return indicating success
	return ERR_SUCCESS;
}

// HOST section parser
P_BP_NMAP_HOST BP_NMapApplicationController::parseNMap_HOST(xmlNode * host)
{

	// check for host information
	if(!host)
		return NULL;

	// WARNING: Spaghetti below.  Because of the way that libxml parses everything
	//          into a series of tree/lists, a lot of traversal logic must take place.
	//          This logic is in the form of multiple layers of loops, and is specific
	//          to expected NMAP xml file output.

	// create new host entry (returned day)
	P_BP_NMAP_HOST host_entry = (P_BP_NMAP_HOST) tq_calloc(this->nmap_m, sizeof(BP_NMAP_HOST), 1, BPLN);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%% Parse Out Properties %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set local properties which we can iterate from
	xmlAttr * properties = host->properties;

	// walk properties
	for(; properties; properties = properties->next)
	{

		// get name
		if(!properties->name)
			continue;

		// set start time here
		if(memcmp(properties->name, "starttime", bpstrlen("starttime")) == 0)
		{
			host_entry->starttime = (char *) tq_strdup(this->nmap_m, (char *) properties->children->content, BPLN);
		}

		// set end time here
		if(memcmp(properties->name, "endtime", bpstrlen("endtime")) == 0)
		{
			host_entry->endtime = (char *) tq_strdup(this->nmap_m, (char *) properties->children->content, BPLN);
		}

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%% Parse Out Children %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure we have children
	if(!host->children)
		return NULL;

	// set children pointer
	xmlNode * children = host->children;

	// walk children
	for(; children; children = children->next)
	{

		// ensure type is xml node
		if(children->type != 1)
			continue;

		// %%%%%%%%%% STATUS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// generate status structure here
		if(memcmp(children->name, "status", bpstrlen("status")) == 0)
		{

			// walk properties list
			for(; children->properties; children->properties = children->properties->next)
			{

				// check for state
				if(memcmp( children->properties->name, "state", 5) == 0)
				{

					printf("\n Processing State: %s", children->properties->children->content);

					// set state
					host_entry->status.state = (char *) tq_strdup(this->nmap_m, (char *) children->properties->children->content, BPLN);

				}

				// if we have a reason, set it here
				if(memcmp( children->properties->name, "reason", 6) == 0)
				{
					printf("\n Processing Reason: %s", children->properties->children->content);
					// set reason
					host_entry->status.reason = (char *) tq_strdup(this->nmap_m, (char *) children->properties->children->content, BPLN);

				}

			}

		}


		// %%%%%%%%%% ADDRESS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


		// generate status structure here
		if(memcmp(children->name, "address", bpstrlen("address")) == 0)
		{

			printf("\n Now parsing address entries");

			// walk properties list
			for
			(
				;
				children->properties;
				children->properties = children->properties->next
			)
			{

				// check for addrtype entry
				if(memcmp( children->properties->name, "addrtype", 8) == 0)
				{

					if(children->properties->children)
					for(;children->properties->children; children->properties->children = children->properties->children->next)
					{

						// increment the addr types count
						host_entry->addrtypes_n ++;

						// allocate space for addresses
						host_entry->addrtypes = (char **) tq_realloc(this->nmap_m, host_entry->addrtypes, sizeof(char *) * (host_entry->addrtypes_n+1), BPLN);

						// now duplicate the entry itself
						host_entry->addrtypes[host_entry->addrtypes_n-1] = tq_strdup(this->nmap_m, (char *) children->properties->children->content, BPLN);

						// truncate stack
						host_entry->addrtypes[host_entry->addrtypes_n] = NULL;

						// printf("\n got addrtype content?: %s", children->properties->children->content);
						printf("\n host_entry->addrtypes[host_entry->addrtypes_n-1]: %s", host_entry->addrtypes[host_entry->addrtypes_n-1]);

					}

					continue;

				}

				// check for addr entry
				if(memcmp( children->properties->name, "addr", 4) == 0)
				{

					printf("\n Addr processing");

					if(children->properties->children)
					for(;children->properties->children; children->properties->children = children->properties->children->next)
					{

						// increment the addr types count
						host_entry->addrs_n ++;

						// allocate space for addresses
						host_entry->addrs = (char **) tq_realloc(this->nmap_m, host_entry->addrs, sizeof(char *) * (host_entry->addrs_n+1), BPLN);

						// now duplicate the entry itself
						host_entry->addrs[host_entry->addrs_n-1] = tq_strdup(this->nmap_m, (char *) children->properties->children->content, BPLN);
						host_entry->addrs[host_entry->addrs_n]   = NULL;

						// printf("\n got addrtype content?: %s", children->properties->children->content);
						printf("\n host_entry->addrs[host_entry->addrs_n-1] : %s", host_entry->addrs[host_entry->addrs_n-1] );

					}

					// continue the loop at this point
					continue;
				}

			}

		}

		// %%%%%%%%%% HOSTNAMES %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// parse hostnames
		if(memcmp(children->name, "hostnames", bpstrlen("hostnames")) == 0)
		{

			printf("\n --");
			printf("\n Now parsing hostnames");

			// walk properties list
			for
			(
				;
				children->properties;
				children->properties = children->properties->next
			)
			{
				printf("\n h: %s", children->properties->name);
			}

			for(;children->children; children->children = children->children->next)
			{

				if(children->children->type != 1)
					continue;

				// increment hostnames entry counter
				host_entry->hostnames_n++;

				// create new hostname structure
				host_entry->hostnames = (P_BP_NMAP_HOSTNAME) tq_realloc(this->nmap_m, host_entry->hostnames, sizeof(BP_NMAP_HOSTNAME) * (host_entry->hostnames_n + 1) , BPLN);

				// walk the property list
				for(; children->children->properties; children->children->properties = children->children->properties->next)
				{

					// ensure we have a name
					if(!children->children->properties->name)
						continue;

					// insert type
					if(memcmp(children->children->properties->name, "type", 4) == 0)
					{
						host_entry->hostnames[host_entry->hostnames_n-1].type = (char *) tq_strdup(this->nmap_m, (char *) children->children->properties->children->content, BPLN);
						printf("\n host_entry->hostnames[host_entry->hostnames_n-1].type: %s", host_entry->hostnames[host_entry->hostnames_n-1].type);
					}

					// now insert name
					if(memcmp(children->children->properties->name, "name", 4) == 0)
					{

						host_entry->hostnames[host_entry->hostnames_n-1].name = (char *) tq_strdup(this->nmap_m, (char *) children->children->properties->children->content, BPLN);
						printf("\n host_entry->hostnames[host_entry->hostnames_n-1].name: %s", host_entry->hostnames[host_entry->hostnames_n-1].name);

					}

				}

			}

		}

		// %%%%%%%%%% PORTS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// parse ports
		if(memcmp(children->name, "ports", bpstrlen("ports")) == 0)
		{

			printf("\n --");
			printf("\n Now parsing PORTS");

			for(;children->children; children->children = children->children->next)
			{

				if(children->children->type != 1)
					continue;

				if(!children->children->name)
					continue;

				// check and see if we got extra ports
				if(memcmp(children->children->name, "extraports", 10) == 0)
				{

					// walk the property list
					for
					(
						;
						children->children->properties;
						children->children->properties = children->children->properties->next
					)
					{

						// check if we have state entry
						if(memcmp(children->children->properties->name, "state", 5) == 0)
						{

							// set state entry
							host_entry->extraports_state = (char *) tq_strdup(this->nmap_m, (char *) children->children->properties->children->content, BPLN);

						}

						// check if we have count entry
						if(memcmp(children->children->properties->name, "count", 5) == 0)
						{

							// set state entry
							host_entry->extraports_count = (char *) tq_strdup(this->nmap_m, (char *) children->children->children->content, BPLN);

						}

					}

				}

				// check and see if we got port information
				if(memcmp(children->children->name, "port", 4) == 0)
				{

					// increment ports entry counter
					host_entry->ports_n++;

					// create new ports structure
					host_entry->ports = (P_BP_NMAP_PORT) tq_realloc(this->nmap_m, host_entry->ports, sizeof(BP_NMAP_PORT) * (host_entry->ports_n + 1) , BPLN);

					// walk the property list
					for(; children->children->properties; children->children->properties = children->children->properties->next)
					{

						// ensure we have a name
						if(!children->children->properties->name)
							continue;

						if(memcmp(children->children->properties->name, "protocol", 8) == 0)
						{
							host_entry->ports[host_entry->ports_n-1].protocol = (char *) tq_strdup(this->nmap_m, (char *) children->children->properties->children->content, BPLN);
							printf("\n host_entry->ports[host_entry->ports_n-1].protocol: %s", host_entry->ports[host_entry->ports_n-1].protocol);
						}

						if(memcmp(children->children->properties->name, "portid", 6) == 0)
						{

							host_entry->ports[host_entry->ports_n-1].portid = (char *) tq_strdup(this->nmap_m, (char *) children->children->properties->children->content, BPLN);
							printf("\n host_entry->ports[host_entry->ports_n-1].portid: %s", host_entry->ports[host_entry->ports_n-1].portid);

						}

					}

					// now walk individual nodes (aka port stats/service etc)
					for(; children->children->children; children->children->children = children->children->children->next)
					{

						if(memcmp(children->children->children->name, "state", 5) == 0)
						{
							for(; children->children->children->properties; children->children->children->properties = children->children->children->properties->next)
							{
								if(memcmp(children->children->children->properties->name, "state", 5) == 0)
								{
									host_entry->ports[host_entry->ports_n-1].state = tq_strdup(this->nmap_m, (char *) children->children->children->properties->children->content, BPLN);
								}
							}
						}

						if(memcmp(children->children->children->name, "service", 7) == 0)
						{

							for(; children->children->children->properties; children->children->children->properties = children->children->children->properties->next)
							{
								if(memcmp(children->children->children->properties->name, "name", 4) == 0)
								{
									host_entry->ports[host_entry->ports_n-1].service = tq_strdup(this->nmap_m, (char *) children->children->children->properties->children->content, BPLN);
								}
							}

						}

					}

				} // == end port subparser ======

			}

		}

		// %%%%%%%%%% TIMES %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// parse times
		if(memcmp(children->name, "times", bpstrlen("times")) == 0)
		{

		}

	}


	// return indicating success
	return host_entry;
}

// RUNSTATS section parser
BP_ERROR_T BP_NMapApplicationController::parseNMap_RUNSTATS(xmlNode * runstats)
{
	printf("\n Parsing Runstats Info?: %p", runstats);

	// return indicating success
	return ERR_SUCCESS;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Database Insertion Methods %%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// insert host node data
BP_ERROR_T BP_NMapApplicationController::insertNMapHostRecord(P_BP_NMAP_HOST host)
{

	// ensure we have host data
	if(!host)
		return ERR_FAILURE;

	// ensure we have a session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// ensure session is connected
	if(!this->mysqlSession->connected)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%% Insert Host Entry %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Generate parameter/query vars.
	char *ipv4_addr         = "''";
	char *ipv6_addr         = "''";
	char * mac_addr         = "''";
	char * host_starttime   = BP_BufferToMySQLHexString(host->starttime,        host->starttime ? bpstrlen(host->starttime) : 0);
	if(!host_starttime)
		host_starttime = "''";

	char * host_endtime     = BP_BufferToMySQLHexString(host->endtime,          host->endtime ? bpstrlen(host->endtime) : 0);
	if(!host_endtime)
		host_endtime = "''";

	char * state            = BP_BufferToMySQLHexString(host->status.state,     host->status.state ? bpstrlen(host->status.state) : 0);
	if(!state)
		state = "''";

	char * state_reason     = BP_BufferToMySQLHexString(host->status.reason,    host->status.reason ? bpstrlen(host->status.reason) : 0);
	if(!state_reason)
		state_reason = "''";

	// State check
	char * extraports_state =  BP_BufferToMySQLHexString(host->extraports_state, host->extraports_state ? bpstrlen(host->extraports_state) : 0);
	if(!extraports_state)
		extraports_state = "''";

	// check extraports count
	char * extraports_count = BP_BufferToMySQLHexString(host->extraports_count, host->extraports_count? bpstrlen(host->extraports_count) : 0);
	if(!extraports_count)
		extraports_count = "''";

	// get addrs
	size_t n = 0;
	for(; n < host->addrtypes_n; n++)
	{

		// ensure the pointer is non-null
		if(!host->addrs[n])
			continue;

		if(memcmp((void *) host->addrtypes[n], "ipv4", 4 ) == 0)
		{
			ipv4_addr = (char *) BP_BufferToMySQLHexString(host->addrs[n], host->addrs[n] ? bpstrlen(host->addrs[n]) : 0);
		}
		if(memcmp((void *) host->addrtypes[n], "ipv6", 4 ) == 0)
		{
			ipv6_addr = (char *) BP_BufferToMySQLHexString(host->addrs[n], bpstrlen(host->addrs[n]));
		}
		if(memcmp((void *) host->addrtypes[n], "mac", 3 ) == 0)
		{
			mac_addr = (char *) BP_BufferToMySQLHexString(host->addrs[n], bpstrlen(host->addrs[n]));
		}

	}

	// destroy the statement if one is lingering
	this->mysqlDestroyStatement();

	// create builder
	BP_StringUtils * builder = new BP_StringUtils(NULL);

	// build the insert front-load

	builder->BuildString("INSERT INTO `nmap_host_info` ( ");
	builder->BuildString(" `ipv4_addr`, `ipv6_addr`, `mac_addr`, `host_starttime`, ");
	builder->BuildString(" `host_endtime`, `state`, `state_reason`, `extraports_state`, `extraports_count`) VALUES ");

	builder->BuildString(" ( ");
	builder->BuildString(ipv4_addr);
	builder->BuildString(" , ");
	builder->BuildString(ipv6_addr);
	builder->BuildString(" , ");
	builder->BuildString(mac_addr);
	builder->BuildString(" , ");
	builder->BuildString(host_starttime);
	builder->BuildString(" , ");
	builder->BuildString(host_endtime);
	builder->BuildString(" , ");
	builder->BuildString(state);
	builder->BuildString(" , ");
	builder->BuildString(state_reason);
	builder->BuildString(" , ");
	builder->BuildString(extraports_state);
	builder->BuildString(" , ");
	builder->BuildString(extraports_count);
	builder->BuildString(" )  ");

	// prepare the query
	this->mysqlPrepareQuery(builder->build_str);

	// execute
	if(this->mysqlExecuteStatement() != ERR_SUCCESS)
	{

		// ensure we can execute our statement (in the case that
		// the server goes away for whatever reason)
		return ERR_FAILURE;
	}


#if USE_MARIADB == 1


	// save insert id for next set of inserts
	size_t insert_id = mysql_insert_id(this->mysqlSession->conn);

#else

	// save insert id for next set of inserts
	size_t insert_id = this->mysqlSession->stmt->insert_id;

#endif

	// destroy the statement
	this->mysqlDestroyStatement();

	// destroy the builder (free strings)
	delete builder;

	// create host information id buffer suitable for insert use
	char *nmap_host_info_id = BP_SizeTToMySQLBuffer(insert_id);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%% Insert Ports %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	char *protocol = "''";
	char *portid = "''";

	n = 0;
	for(; n < host->ports_n; n++)
	{
		// create new builder
		builder = new BP_StringUtils(NULL);

		// generate port id string
		char * protocol_s = BP_BufferToMySQLHexString(host->ports[n].protocol, bpstrlen(host->ports[n].protocol));

		// get port number
		size_t port_num = strtoul(host->ports[n].portid, NULL, 10);

		// create insert port string
		char * insert_port_str = BP_SizeTToMySQLBuffer(port_num);


		builder->BuildString("INSERT INTO `nmap_port_info` (`nmap_host_info_id`, `protocol`, `portid`, `state`, `service`) VALUES ");
		builder->BuildString(" ( ");
		builder->BuildString(nmap_host_info_id);
		builder->BuildString(" , ");
		builder->BuildString(protocol_s);
		builder->BuildString(" , ");
		builder->BuildString(insert_port_str);
		builder->BuildString(" , ");
		builder->BuildMySQLHexFromBuff(host->ports[n].state, strnlen(host->ports[n].state, 2048));
		builder->BuildString(" , ");
		builder->BuildMySQLHexFromBuff(host->ports[n].service, strnlen(host->ports[n].service, 2048));
		builder->BuildString(" ) ");

		// prepare the query
		this->mysqlPrepareQuery(builder->build_str);

		printf("\n Got PORT INSERT: %s", builder->build_str);


		// execute
		this->mysqlExecuteStatement();
		this->mysqlDestroyStatement();

		// destroy insert port string
		bpfree(insert_port_str);
		bpfree(protocol_s);
		insert_port_str = NULL;

		delete builder;
		builder = NULL;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%% Insert Hostnames %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	n = 0;
	for(; n < host->hostnames_n; n++)
	{
		// create new builder
		builder = new BP_StringUtils(NULL);

		char * hostname      = (char *) BP_BufferToMySQLHexString(host->hostnames[n].name, bpstrlen(host->hostnames[n].name));
		char * hostname_type = (char *) BP_BufferToMySQLHexString(host->hostnames[n].type, bpstrlen(host->hostnames[n].type));

		if(!hostname)
			continue;
		if(!hostname_type)
			continue;
		builder->BuildString("INSERT INTO `nmap_hostname_info` (`nmap_host_info_id`, `name`, `type`) VALUES ");
		builder->BuildString(" ( ");
		builder->BuildString(nmap_host_info_id);
		builder->BuildString(" , ");
		builder->BuildString(hostname);
		builder->BuildString(" , ");
		builder->BuildString(hostname_type);
		builder->BuildString(" ) ");


		// prepare the query
		this->mysqlPrepareQuery(builder->build_str);

		// execute
		this->mysqlExecuteStatement();

		// destroy dangling data
		this->mysqlDestroyStatement();


		if(hostname)
			bpfree(hostname);
		if(hostname_type)
			bpfree(hostname_type);

		delete builder;
		builder = NULL;

	}


	// return indicating success
	return ERR_SUCCESS;

}
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% General Utility Methods %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// resolves an xml node string to a type which can be switched() on
BP_NMAP_XML_TOPLEVEL_NODE_TYPE BP_NMapApplicationController::xmlGetTopLevelNodeTypeByName(char * node_name)
{

	// check the node name
	if(!node_name)
		return BP_NMAP_XML_NODE_TYPE_UNSET;

	// set node name length
	size_t node_name_n = strnlen(node_name, 1024);

	// length has to be greater than four
	if(node_name_n < 4)
		return BP_NMAP_XML_NODE_TYPE_UNSET;

	// BP_NMAP_XML_NODE_TYPE_UNSET,
	char *unset      = (char *) "unset";
	if(node_name_n == 5)
	if(memcmp(unset, node_name, 5) == 0)
		return BP_NMAP_XML_NODE_TYPE_UNSET;

	// BP_NMAP_XML_NODE_TYPE_SCANINFO,
	char * scaninfo  = (char *) "scaninfo";
	if(node_name_n == 8)
	if(memcmp(scaninfo, node_name, 8) == 0)
		return BP_NMAP_XML_NODE_TYPE_SCANINFO;

	// BP_NMAP_XML_NODE_TYPE_VERBOSE,
	char * verbose   = (char *) "verbose";
	if(node_name_n == 7)
	if(memcmp(verbose, node_name, 7) == 0)
		return BP_NMAP_XML_NODE_TYPE_VERBOSE;

	// BP_NMAP_XML_NODE_TYPE_DEBUGGING,
	char * debugging = (char *) "debugging";
	if(node_name_n == 9)
	if(memcmp(debugging, node_name, 9) == 0)
		return BP_NMAP_XML_NODE_TYPE_DEBUGGING;

	// BP_NMAP_XML_NODE_TYPE_HOST,
	char * host      = (char *) "host";
	if(node_name_n == 4)
	if(memcmp(host, node_name, 4) == 0)
		return BP_NMAP_XML_NODE_TYPE_HOST;


	// BP_NMAP_XML_NODE_TYPE_RUNSTATS
	char * runstats  = (char *) "runstats";
	if(node_name_n == 8)
	if(memcmp(runstats, node_name, 8) == 0)
		return BP_NMAP_XML_NODE_TYPE_RUNSTATS;


	// return bad data
	return BP_NMAP_XML_NODE_TYPE_UNSET;

}

// returns the global current sql map process count
size_t BP_NMapApplicationController::currentNumProcessesRunning()
{

	// return the global nmap run count
	return global_nmap_run_count;

}


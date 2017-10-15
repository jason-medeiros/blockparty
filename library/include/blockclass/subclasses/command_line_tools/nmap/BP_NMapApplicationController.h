/*
 * BP_NMapApplicationController.h
 *
 *  Created on: Aug 30, 2013
 *      Author: root
 */

#ifndef BP_NMAPAPPLICATIONCONTROLLER_H_
#define BP_NMAPAPPLICATIONCONTROLLER_H_

/*

	BP_NMapApplicationController : BP_CommandLineToolBase : Block
	==================================================================
	This class inherits from command line tool base controller, and allows
	for the direct control of the nmap application.  The controller has the
	ability to report directly to the db if so set.


*/

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %% NMap XML Data Node Type Enumerators %%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



typedef enum
{
	BP_NMAP_XML_NODE_TYPE_UNSET,
	BP_NMAP_XML_NODE_TYPE_SCANINFO,
	BP_NMAP_XML_NODE_TYPE_VERBOSE,
	BP_NMAP_XML_NODE_TYPE_DEBUGGING,
	BP_NMAP_XML_NODE_TYPE_HOST,
	BP_NMAP_XML_NODE_TYPE_RUNSTATS
} BP_NMAP_XML_TOPLEVEL_NODE_TYPE;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% NMap Data Structures %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// port entry
typedef struct __BP_NMAP_PORT {
	char * protocol;
	char * portid;
	char * service;
	char * state;
} BP_NMAP_PORT, *P_BP_NMAP_PORT;

// set the hostname
typedef struct __BP_NMAP_HOSTNAME {

	// hostname
	char *name;

	// record type
	char *type;

} BP_NMAP_HOSTNAME, *P_BP_NMAP_HOSTNAME;

typedef struct __BP_NMAP_ADDRESS {
	char * addr;
	char * addrtype;
	char * vendor;
} BP_NMAP_ADDRESS, *P_BP_NMAP_ADDRESS;


// corresponds to the "status" section of the nmap xml file
typedef struct __BP_NMAP_STATUS {

	// host state (up/down)
	char * state;

	// reason for up down
	char * reason;

} BP_NMAP_STATUS, *P_BP_NMAP_STATUS;

// corresponds to the "host" section of the nmap xml file (this is
// the struct you're looking for)
typedef struct __BP_NMAP_HOST {

	// scan start time
	char *starttime;

	// scan end time
	char *endtime;

	// status structure
	BP_NMAP_STATUS status;

	// address types
	char ** addrtypes;
	size_t  addrtypes_n;

	// address types
	char ** addrs;
	size_t  addrs_n;

	// hostname information
	P_BP_NMAP_HOSTNAME hostnames;
	size_t             hostnames_n;

	// extraports information
	char *extraports_state;
	char *extraports_count;

	// port information
	P_BP_NMAP_PORT ports;
	size_t         ports_n;

} BP_NMAP_HOST, *P_BP_NMAP_HOST;

// corresponds to the "debugging" section of the nmap xml file
typedef struct __BP_NMAP_DEBUGGING {

	char * level;

} BP_NMAP_DEBUGGING, *P_BP_NMAP_DEBUGGING;

// corresponds to the "verbose" section of a nmap xml file
typedef struct __BP_NMAP_VERBOSE {

	char * level;

} BP_NMAP_VERBOSE, *P_BP_NMAP_VERBOSE;



// corresponds to the "scaninfo" section of a nmap xml file
typedef struct __BP_NMAP_SCANINFO {

	// scan type
	char * type;

	// scan protocol
	char * protocol;

	// number of services scanned
	char * numservices;

	// not sure
	char * services;


	// ==== Tree Data ======================

	// secondary information
	P_BP_NMAP_VERBOSE verbose;

	// debugging information
	P_BP_NMAP_DEBUGGING debugging;

	// host information
	P_BP_NMAP_HOST hosts;

} BP_NMAP_SCANINFO, *P_BP_NMAP_SCANINFO;




// Class Definition
class BP_NMapApplicationController : public BP_CommandLineToolBase /* inherits from Block */
{

public:

	// memory allocator for local data
	P_BP_LINKL nmap_m;


	// constructor (autocalls contructor for command line base)
	BP_NMapApplicationController(char * path_to_nmap_binary);

	// deconstructor (frees all memory)
	~BP_NMapApplicationController();

	// =====================================

	// this is flagged after the app exited
	size_t nmap_exited;

	// Execute nmap.  This will populate some local data, so before
	// calling it recursively, call the cleanup routine below, or just delete
	// and recreate the object.  Performance hits should be negligable either way.
	BP_ERROR_T executeNMapBinary();


	// Post execution cleanup routine.  Call after you've duplicated any data
	// you'd like to keep from the populated session.
	BP_ERROR_T postExecutionCleanup();


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%% NMap XML Parsing %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the nmap output file
	char * nmap_output_file;

	// new xml parser simply created for parsing the set output file
	BP_StringUtils * nmap_xml_parser;


	// Sets the output file.
	BP_ERROR_T setNMapOutputXMLFile(char * nmap_xml_file);

	// Parses the output XML file.
	BP_ERROR_T parseNMapOutputXMLFile();

	// ==== Sub-section parsers =======

	// host subsections (contain host/port/addr information)
	// This is an array of pointers to structures.
	P_BP_NMAP_HOST * host_subsections;
	size_t         host_subsections_n;


	// SCANINFO section parser (parses from scan info top)
	BP_ERROR_T parseNMap_SCANINFO(xmlNode * scaninfo);

	// VERBOSE section parser
	BP_ERROR_T parseNMap_VERBOSE(xmlNode * verbose);

	// DEBUGGING section parser
	BP_ERROR_T parseNMap_DEBUGGING(xmlNode * debugging);

	// HOST section parser
	P_BP_NMAP_HOST parseNMap_HOST(xmlNode * host);

	// RUNSTATS section parser
	BP_ERROR_T parseNMap_RUNSTATS(xmlNode * runstats);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Database Insertion Methods %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// insert host node data
	BP_ERROR_T insertNMapHostRecord(P_BP_NMAP_HOST host);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% General Utility Methods %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// resolves an xml node string to a type which can be switched() on
	BP_NMAP_XML_TOPLEVEL_NODE_TYPE xmlGetTopLevelNodeTypeByName(char * node_name);

	// returns the global current nmap process count
	size_t currentNumProcessesRunning();

};


#endif /* BP_NMAPAPPLICATIONCONTROLLER_H_ */

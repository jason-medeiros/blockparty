/*
 * BP-NmapDB.h -- Headers and structures relating to the extraction of the nmap
 * data from a mysql database.  The database includes numerous data items that are
 * redundant between structures.  They are preserved to keep convention with the db.
 *
 *
 *  Created on: Mar 14, 2010
 *      Author: Jason Medeiros
 */


#ifndef BPNMAPDB_H_
#define BPNMAPDB_H_

// port structure to use (names are taken from database fields)
typedef struct _BP_NMAPDB_PORTSTATS {
	char d[64];          // time
	char t[64];          // date
	size_t hostid;       // host id in db
	char target_ip[64];  // target ip (ascii)
	unsigned short port; // target port
	char protocol[16];   // protocol (tcp/udp/etc)
	char service[1024];  // nmap service info
	char state[16];      // (open/closed/filtered/etc)
	char fullversion[16];// full version info
	size_t scannerid;    // scanner id
	size_t runid;        // runid
	size_t rec_id;       // rec id
} BP_NMAPDB_PORTSTATS, *P_BP_NMAPDB_PORTSTATS;

// hosts structure
typedef struct _BP_NMAPDB_HOSTSTATS {
	size_t hostid;
	char ip[64];
	char user[1024];
	size_t userid;
	char d[64];
	char t[64];
	size_t open_ports;
	size_t ports_scanned;
	size_t runid;
	size_t scanner_id;
	size_t rec_id;
} BP_NMAPDB_HOSTSTATS, *P_BP_NMAPDB_HOSTSTATS;

// target structure
typedef struct _BP_NMAPDB_TARGETS {
	char host[1024];
	char d[64];
	char t[64];
	char ip[64];
	size_t ipnum;
	char mac[64];
	char macvendor[64];
	size_t flag;
	size_t hostid;
	char description[1024];
	char os_guessed[1024];
	char os_known[1024];
	char fingerprint[1024];
	size_t rec_id;
} BP_NMAPDB_TARGETS, *P_NMAPDB_TARGETS;

// scanner (user) table
typedef struct _BP_NMAPDB_SCANNERS {
	char scanner[1024]; // username
	char scanner_ip[64]; // ip scanner used
	char description[1024]; // description
	char d[64];
	char t[64];
	size_t flag;
	size_t scanner_id;
	size_t rec_id;
} BP_NMAPDB_SCANNERS, *P_BP_NMAPDB_SCANNERS;

// runlist table
typedef struct _BP_NMAPDB_RUNLIST {
	char d[64];
	char t[64];
	size_t run_type;
	char user[1024];
	size_t userid;
	char host[1024];
	char host_ip[64];
	size_t numscanned;
	size_t numup;
	size_t runid;
	size_t rec_id;
} BP_NMAPDB_RUNLIST, *P_BP_NMAPDB_RUNLIST;

// this enumerator defines the type of result in a BP_NMAPDB_RESULTS structure.
enum BP_NMAPDB_RESULT_TYPE {
	BP_NMAPDB_RESULT_RUNLIST,
	BP_NMAPDB_RESULT_SCANNERS,
	BP_NMAPDB_RESULT_TARGETS,
	BP_NMAPDB_RESULT_HOSTSTATS,
	BP_NMAPDB_RESULT_PORTSTATS
};


// generic result set container for nmap db
typedef struct _BP_NMAPDB_RESULTS {


	// number of records returned from the database
	size_t record_count;

	// opaque datatype holding one of a variety of different data structure types (normally arrays of database related
	// structures)
	void *opaque_data;

	// result type (defined above in enum)
	BP_NMAPDB_RESULT_TYPE type;

	// query used to generate results
	char *query;

} BP_NMAPDB_RESULTS, *P_BP_NMAPDB_RESULTS;


// full search results structures (populated in block class only)
typedef struct _BP_NMAPDB_SEARCH_RESULTS {

	// search results for runlist
	P_BP_NMAPDB_RESULTS runlist;

	// search result for scanners
	P_BP_NMAPDB_RESULTS scanners;

	// search results for targets
	P_BP_NMAPDB_RESULTS targets;

	// search results for hoststats
	P_BP_NMAPDB_RESULTS hoststats;

	// search results for portstats
	P_BP_NMAPDB_RESULTS portstats;

} BP_NMAPDB_SEARCH_RESULTS, *P_BP_NMAPDB_SEARCH_RESULTS;


/*
 * Using Search Functions:
 * ----------------------------------------------------
 * Each search function has a min, and max parameter.  Searching is done by
 * building a SQL query that basically matches all min/max items set in the
 * structures provided.  Min uses "<" operator, and Max uses ">" operator.
 * The like parameter matches using the "LIKE" keyword, and the equal parameter
 * matches with the "=" operator.  NULL parameters are not processed.  Like parameters
 * only work on character entries.
 */


// search the portstats database table
P_BP_NMAPDB_RESULTS BP_NDBSearchPortStats(P_BP_MYSQL_SESSION mysql_db, P_BP_NMAPDB_PORTSTATS min, P_BP_NMAPDB_PORTSTATS max, P_BP_NMAPDB_PORTSTATS like, P_BP_NMAPDB_PORTSTATS equal, size_t limitBottom, size_t limitTop);

// search the hoststats database table
P_BP_NMAPDB_RESULTS BP_NDBSearchHostStats(P_BP_MYSQL_SESSION mysql_db, P_BP_NMAPDB_HOSTSTATS min, P_BP_NMAPDB_HOSTSTATS max, P_BP_NMAPDB_HOSTSTATS like, P_BP_NMAPDB_HOSTSTATS equal, size_t limitBottom, size_t limitTop);

// search the targets database table
P_BP_NMAPDB_RESULTS BP_NDBSearchTargets(P_BP_MYSQL_SESSION mysql_db, P_NMAPDB_TARGETS min, P_NMAPDB_TARGETS max, P_NMAPDB_TARGETS like, P_NMAPDB_TARGETS equal, size_t limitBottom, size_t limitTop);

// search the scanners database table
P_BP_NMAPDB_RESULTS BP_NDBSearchScanners(P_BP_MYSQL_SESSION mysql_db, P_BP_NMAPDB_SCANNERS min, P_BP_NMAPDB_SCANNERS max, P_BP_NMAPDB_SCANNERS like, P_BP_NMAPDB_SCANNERS equal, size_t limitBottom, size_t limitTop);

// search the run list database table
P_BP_NMAPDB_RESULTS BP_NDBSearchRunlist(P_BP_MYSQL_SESSION mysql_db, P_BP_NMAPDB_RUNLIST min, P_BP_NMAPDB_RUNLIST max, P_BP_NMAPDB_RUNLIST like, P_BP_NMAPDB_RUNLIST equal, size_t limitBottom, size_t limitTop);


// ---------- Visualization Functions ----------------------------


// This function autodetects a result type and prints out the result.
int BP_NDBVisualizeResults(P_BP_NMAPDB_RESULTS result, size_t displayQuery);


// Visualizer for P_BP_NMAPDB_PORTSTATS structure
int BP_NDBVisualzePortStats(P_BP_NMAPDB_PORTSTATS data);

// Visualizer for P_BP_NMAPDB_HOSTSTATS structure
int BP_NDBVisualizeHostStats(P_BP_NMAPDB_HOSTSTATS data);

// Visualizer for P_NMAPDB_TARGETS structure
int BP_NDBVisualizeTargets(P_NMAPDB_TARGETS data);

// Visualizer for P_BP_NMAPDB_SCANNERS structure
int BP_NDBVisualizeScanners(P_BP_NMAPDB_SCANNERS data);

// Visualizer for P_BP_NMAPDB_RUNLIST structure
int BP_NDBVisualizeRunlist(P_BP_NMAPDB_RUNLIST data);


#endif /* BPNMAPDB_H_ */

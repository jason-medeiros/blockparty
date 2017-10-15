/*
 * BP-DBSqlite.h
 *
 *  Created on: Jul 30, 2015
 *      Author: root
 */

#ifndef __BP_DB_SQLITE_H_
#define __BP_DB_SQLITE_H_


// Query type enumerator.  Used for logging queries
// which are being run.
typedef enum
{

	BP_DB_SQLITE_QUERY_TYPE_UNSET,
	BP_DB_SQLITE_QUERY_TYPE_SELECT,
	BP_DB_SQLITE_QUERY_TYPE_INSERT,
	BP_DB_SQLITE_QUERY_TYPE_UPDATE,
	BP_DB_SQLITE_QUERY_TYPE_DELETE,
	BP_DB_SQLITE_QUERY_TYPE_CREATE_TABLE

} BP_DB_SQLITE_QUERY_TYPE;

typedef enum
{
	BP_DB_SQLITE_TRANSACTION_STATE_UNSET,
	BP_DB_SQLITE_TRANSACTION_STATE_OPEN,
	BP_DB_SQLITE_TRANSACTION_STATE_CLOSED
} BP_DB_SQLITE_TRANSACTION_STATE;

// different parameter types
typedef enum
{

	BP_DB_SQLITE_QUERY_PARAM_TYPE_UNSET,
	BP_DB_SQLITE_QUERY_PARAM_TYPE_NULL,
	BP_DB_SQLITE_QUERY_PARAM_TYPE_INTEGER,
	BP_DB_SQLITE_QUERY_PARAM_TYPE_REAL,
	BP_DB_SQLITE_QUERY_PARAM_TYPE_TEXT,
	BP_DB_SQLITE_QUERY_PARAM_TYPE_BLOB

} BP_DB_SQLITE_QUERY_PARAM_TYPE;




// SQLite Structure
typedef struct __BP_DB_SQLITE {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% SQLite Internal Handles %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// sqlite 3 db handle
	sqlite3 * db;

	// file associated with this db handle
	char * db_file;

	// statment
	sqlite3_stmt * stmt;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Schema Information %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// schema information about the loaded db
	P_BP_HASH_TABLE_REGISTRY schema;

	// total number of tables found within the sqlite schema.
	size_t num_tables;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Query Transaction Stack %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// NOTE: All queries are run in transactions.  Even
	//       if only running one query, it will be run
	//       in a transaction.  The routine BP_SQLiteAddQuery
	//       will add a query into the hreg below.  When the
	//       query stack is is filled with the required
	//       queries to generate state information, you can
	//       call BP_SQLiteRunTransaction()

	// this is the uuid of the last query prepared
	char * last_query_uuid;

	// the uuid of the last query result
	char * last_result_query_uuid;


	// number of parameters for the last query
	size_t last_query_param_n;

	// these are the queries which will be run
	P_BP_HASH_TABLE_REGISTRY queries;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Query Result Cache HREG %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this holds a small cache
	P_BP_HASH_TABLE_REGISTRY query_result_cache;

	// If a query has run, this will hold the last result as a pointer
	// to a table.  If running multiple queries, this will be the result of
	// the last query.  You caniterate through the table to check mulitple results,
	// or just look in this->query_result_cache manually (since this pointer
	// is just a reference into that table registry)
	P_BP_HASH_TABLE_KEYED_ENTRY last_query_results;


	// number of results fetched for last fetched query.
	size_t last_result_retrieved_n;

	// total number of records retrieved for ALL queries
	size_t last_transaction_record_count_retrieved;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Query Logging %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this is the log database
	sqlite3 * log_db;

	// file associated with the log file
	char    * log_db_file;


	// --- log settings ------

	// only log errors (default is BP_TRUE)
	BP_BOOL   only_log_errors;

	// log parameters for queries
	BP_BOOL   log_query_parameters;

	// maximum number of entries to store in the query log
	// before truncating queries.
	size_t    log_max_n;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Last Query Flags %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// The last query flags are set after a query is
	// run (or attempted to be run).  They allow for
	// diagnostic runtime checks.

	// if the last query reported any error, this is set
	BP_BOOL last_query_reported_error;

	// if the last query ran ok, this has is set
	BP_BOOL last_query_ran_ok;

	// if the last query generated results, this is set
	BP_BOOL last_query_generated_results;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% General Counters %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// --- transaction counters ----------

	// total number of transactions started
	size_t total_transactions_started;

	// total transactions exited ok
	size_t total_transactions_exited_ok;

	// total number of transactions that failed
	size_t total_transactions_exited_failed;


	// --- individual query counters -----

	// total number of queries executed
	size_t total_queries_executed_n;

	// total number of queries prepared
	size_t total_queries_prepared_n;

	// total number of queries failed
	size_t total_queries_failed_n;

	// total results retrieved
	size_t total_results_retrieved_n;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Hash Table Registry for Handle %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Hash table registry.  Holds all system allocations, and holds
	// relevant data regarding this handle.
	P_BP_HASH_TABLE_REGISTRY system_hreg;


} BP_DB_SQLITE, *P_BP_DB_SQLITE;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Open and Close Connection %%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempt to open a sqlite file.  You can optionally also set
// a query log file here.  The query log will run with default parameters
// unless set.
//
// TODO: Bring query log file parameter defaults into this header (after defined)
//
P_BP_DB_SQLITE BP_SQLiteOpen
(
	char * open_file,
	char * query_log_file
);

// close a sqlite database
BP_ERROR_T BP_SQLiteClose(P_BP_DB_SQLITE sqli);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Schema Related Utilities %%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This will hold information about each column available.
typedef struct __BP_DB_SQLITE_TABLE_COLUMN {

	// column index
	size_t cid;

	// column name
	char * name;

	// type of column
	BP_DB_SQLITE_QUERY_PARAM_TYPE type;

	// whether or not the value can be null
	BP_BOOL notnull;

	// default value / value size if set
	void * dflt_value;
	size_t dflt_value_n;

	// primary key
	BP_BOOL pk;

} BP_DB_SQLITE_TABLE_COLUMN, *P_BP_DB_SQLITE_TABLE_COLUMN;

// attempt to fetch tables
BP_ERROR_T BP_SQLiteSchemaFetchTables(P_BP_DB_SQLITE sqli);

// attempt to fetch columns for tables
BP_ERROR_T BP_SQLiteSchemaFetchColumnsForTables(P_BP_DB_SQLITE sqli);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Add Query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Add query to the current query stack.  Returns a reference
// to sqli->last_query_uuid if run ok.  If adding query fails,
// this returns null.
char * BP_SQLiteAddQuery
(
	P_BP_DB_SQLITE          sqli,
	BP_DB_SQLITE_QUERY_TYPE query_type,
	char *                  sql
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Add Parameters to Query %%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// When you call BP_SQLiteAddQueryParam, one of these structures
// is added to the query hash table and is associated with
// a query.
typedef struct __BP_SQLITE_QUERY_PARAM {

	// type type of parameter
	BP_DB_SQLITE_QUERY_PARAM_TYPE type;

	// data to be used as a parameter
	void * data;
	int    data_as_int;

	// size of parameter
	size_t data_size;

} BP_SQLITE_QUERY_PARAM, *P_BP_SQLITE_QUERY_PARAM;

// add a parameter
BP_ERROR_T BP_SQLiteAddQueryParam
(
	P_BP_DB_SQLITE                sqli,
	char *                        query_id,
	BP_DB_SQLITE_QUERY_PARAM_TYPE param_type,
	void *                        param_data,
	size_t                        param_data_size

);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Run Queries %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Run all queries that are currently in the stack. Run all within
// a transaction.
BP_ERROR_T BP_SQLiteRunQueries
(
	P_BP_DB_SQLITE sqli
);




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Clear Old Queries/Results/Both %%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// clear all queries within sqli->queries registry.
BP_ERROR_T BP_SQLiteClearQueryCache
(
	P_BP_DB_SQLITE sqli
);

// clear all results within the sqli->query_result_cache
BP_ERROR_T BP_SQLiteClearResultCache
(
	P_BP_DB_SQLITE sqli
);

// Clears both caches (just calls the above two routines)
BP_ERROR_T BP_SQLiteClearBothQueryAndResultCaches
(
	P_BP_DB_SQLITE sqli
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Results Cache %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// metadata for a result
typedef struct __BP_DB_SQLITE_RESULT_METADATA {

	// result index
	size_t                        result_idx;

	// column index
	size_t                        col_idx;

	// entry type
	BP_DB_SQLITE_QUERY_PARAM_TYPE type;

	// data size
	size_t                        data_size;

} BP_DB_SQLITE_RESULT_METADATA, *P_BP_DB_SQLITE_RESULT_METADATA;



// fetch a result for a query
BP_ERROR_T BP_SQLiteFetchResult
(
	P_BP_DB_SQLITE sqli,
	char *         query_id,
	size_t         result_idx
);

// adds a result column to a query result
BP_ERROR_T BP_SQLiteAddResultColumnEntry
(
	P_BP_DB_SQLITE                sqli,
	char *                        query_result_uuid,
	size_t                        result_idx,
	char *                        col_name,
	size_t                        col_idx,
	BP_DB_SQLITE_QUERY_PARAM_TYPE type,
	void *                        data,
	size_t                        data_size
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display SQLite Handle %%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Display sqliite output.
BP_ERROR_T BP_SQLiteDisplay
(
		P_BP_DB_SQLITE sqli
);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Queries / Results %%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Display sqlite query stack.
BP_ERROR_T BP_SQLiteDisplayQueryStack
(
		P_BP_DB_SQLITE           sqli,
		P_BP_HASH_TABLE_REGISTRY query_stack
);

// Display sqlite result stack.
BP_ERROR_T BP_SQLiteDisplayResultStack
(
		P_BP_DB_SQLITE           sqli,
		P_BP_HASH_TABLE_REGISTRY result_stack
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Schema %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Display sqlite schema.
BP_ERROR_T BP_SQLiteDisplaySchema
(
		P_BP_DB_SQLITE           sqli,
		P_BP_HASH_TABLE_REGISTRY schema
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Conversion Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This returns a bpcalloced string representation of a query parameter.  Result
// must be bpfree'd.  This is mainly used when displaying results.
char * BP_SQLiteConvertParameterToCStringForDisplay(P_BP_SQLITE_QUERY_PARAM param);

// This routine is used to convert metadata and raw data into a human readable
// c string suitable for display.  The result must be bpfreed.
char * BP_SQLiteConvertResultDataToCStringForDisplay
(
		P_BP_DB_SQLITE_RESULT_METADATA record_metadata,
		void * data
);

#endif /* __BP_DBSQLITE_H_ */

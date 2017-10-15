/*
 * BP-DB.h
 *
 *  Created on: Aug 8, 2015
 *      Author: root
 */

#ifndef __BP_DB_H_
#define __BP_DB_H_


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Results Schema %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// database results schema
#include "./schema/results/BP-DBResultsSchema.h"

// database datasets schema
#include "./schema/datasets/BP-DBDatasetsSchema.h"

// html related schema
#include "./schema/html/BP-DBHtmlSchema.h"

// intel schema
#include "./schema/intel/BP-DBIntelSchema.h"

// sensor schema
#include "./schema/sensor/BP-DBSensorSchema.h"

// auth schema
#include "./schema/auth/BP-DBAuthSchema.h"

// assessment schema
#include "./schema/assessment/BP-DBAssessmentSchema.h"

// access list schema
#include "./schema/accesslists/BP-DBAccessListsSchema.h"

// third party tools schema
#include "./schema/third_party_tools/BP-DBThirdPartyToolsSchema.h"

// report oriented schema
#include "./schema/reports/BP-DBReportsSchema.h"

// test sets schema
#include "./schema/testsets/BP-DBTestSetsSchema.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% BPDB Utility Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// create shortcut macro for entering critical sections
#define BPDB_ENTER_CRITICAL_SECTION \
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(bpdb->system_hreg)

// shortcut macro for leaving critical section
#define BPDB_LEAVE_CRITICAL_SECTION \
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(bpdb->system_hreg)

// shortcut macro for leaving critical section and
#define BPDB_LEAVE_CRITICAL_SECTION_AND_RETURN(ret_val) \
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(bpdb->system_hreg, ret_val)

// check in a mariadb connection and return
#define BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ret_val) \
		{                                                                  \
			BP_DBCheckInMariadbConnection(bpdb, mariadbi);                 \
			BPDB_LEAVE_CRITICAL_SECTION_AND_RETURN(ret_val);               \
		}

// this can be called to automatically checkout a mariadbi.
#define BPDB_CHECKOUT_MARIADB_PROLOGUE(ret_val) \
	P_BP_MYSQL_SESSION mariadbi = BP_DBCheckOutAvailableMariadbConnection(bpdb);    \
	if(!mariadbi)                                                                   \
		return ret_val;                                                             \
	if(!mariadbi->conn)                                                             \
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ret_val);          \
	if(!mariadbi->connected)                                                        \
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ret_val);          \
	if(mariadbi->server_gone_away)                                                  \
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ret_val);          \
	if(mariadbi->server_lost_connection)                                            \
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ret_val);          \
	printf("\n [+] dbg: Checked out mariadb entry %p: %s", mariadbi, mariadbi->uuid);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Syngen Utility Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// create shortcut macro for entering critical sections
#define BPDB_SYNGEN_ENTER_CRITICAL_SECTION \
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(syngen->bpdb->system_hreg)

// shortcut macro for leaving critical section
#define BPDB_SYNGEN_LEAVE_CRITICAL_SECTION \
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(syngen->bpdb->system_hreg)

// shortcut macro for leaving critical section and
#define BPDB_SYNGEN_LEAVE_CRITICAL_SECTION_AND_RETURN(ret_val) \
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(syngen->bpdb->system_hreg, ret_val)



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% BPDB MariaDB Metadata Structures %%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// mariadb bigint
typedef long long int BPDB_MARIADB_BIGINT;

// mariadb column type identifiers
typedef enum
{

	/* unset value */
	BPDB_MARIADB_COLUMN_TYPE_UNSET,

	/* begin numeric data types */
	BPDB_MARIADB_COLUMN_TYPE_TINYINT,
	BPDB_MARIADB_COLUMN_TYPE_SMALLINT,
	BPDB_MARIADB_COLUMN_TYPE_MEDIUMINT,
	BPDB_MARIADB_COLUMN_TYPE_INT,
	BPDB_MARIADB_COLUMN_TYPE_BIGINT,
	BPDB_MARIADB_COLUMN_TYPE_DECIMAL,
	BPDB_MARIADB_COLUMN_TYPE_FLOAT,
	BPDB_MARIADB_COLUMN_TYPE_DOUBLE,
	BPDB_MARIADB_COLUMN_TYPE_BIT,

	/* begin string data types */
	BPDB_MARIADB_COLUMN_TYPE_CHAR,
	BPDB_MARIADB_COLUMN_TYPE_VARCHAR,
	BPDB_MARIADB_COLUMN_TYPE_BINARY,
	BPDB_MARIADB_COLUMN_TYPE_VARBINARY,
	BPDB_MARIADB_COLUMN_TYPE_TINYBLOB,
	BPDB_MARIADB_COLUMN_TYPE_BLOB,
	BPDB_MARIADB_COLUMN_TYPE_MEDIUMBLOB,
	BPDB_MARIADB_COLUMN_TYPE_LONGBLOB,
	BPDB_MARIADB_COLUMN_TYPE_TINYTEXT,
	BPDB_MARIADB_COLUMN_TYPE_TEXT,
	BPDB_MARIADB_COLUMN_TYPE_MEDIUMTEXT,
	BPDB_MARIADB_COLUMN_TYPE_LONGTEXT,
	BPDB_MARIADB_COLUMN_TYPE_ENUM,/**/

	/* begin date and time data types */
	BPDB_MARIADB_COLUMN_TYPE_DATE,
	BPDB_MARIADB_COLUMN_TYPE_TIME,
	BPDB_MARIADB_COLUMN_TYPE_DATETIME,
	BPDB_MARIADB_COLUMN_TYPE_TIMESTAMP,
	BPDB_MARIADB_COLUMN_TYPE_YEAR,
	BPDB_MARIADB_COLUMN_TYPE_BAD_VALUE

} BPDB_MARIADB_COLUMN_TYPE;

// Table column metadata
typedef struct __BPDB_MARIADB_TABLE_COLUMN_METADATA {



	// TABLE_CATALOG
	char                    *table_catalog;
	size_t                   table_catalog_data_len;
	BPDB_MARIADB_COLUMN_TYPE table_catalog_data_type;

	// TABLE_SCHEMA
	char   *table_schema;
	size_t  table_schema_data_len;


	// TABLE_NAME
	char   *table_name;
	size_t  table_name_data_len;

	// COLUMN_NAME
	char   *column_name;
	size_t  column_name_data_len;

	// ORDINAL_POSITION
	BPDB_MARIADB_BIGINT ordinal_position;

	// COLUMN_DEFAULT (this is a longtext, but we're capping it at 1024 to keep sanity)
	char   *column_default;
	size_t  column_default_data_len;

	// IS_NULLABLE
	char   *is_nullable;
	size_t  is_nullable_data_len;

	// DATA_TYPE
	char                    *data_type;
	size_t                   data_type_data_len;
	BPDB_MARIADB_COLUMN_TYPE data_type_as_enum;

	// CHARACTER_MAXIMUM_LENGTH
	BPDB_MARIADB_BIGINT character_maximum_length;

	// CHARACTER_OCTET_LENGTH
	BPDB_MARIADB_BIGINT character_octet_length;

	// NUMERIC_PRECISION
	BPDB_MARIADB_BIGINT numeric_precision;

	// NUMERIC_SCALE
	BPDB_MARIADB_BIGINT numeric_scale;

	// DATETIME_PRECISION
	BPDB_MARIADB_BIGINT datetime_precision;

	// CHARACTER_SET_NAME
	char   *character_set_name;
	size_t  character_set_name_data_len;

	// COLLATION_NAME
	char   *collation_name;
	size_t  collation_name_data_len;

	// COLUMN_TYPE
	char                    *column_type;
	size_t                   column_type_data_len;

	// COLUMN_KEY
	char   *column_key;
	size_t  column_key_data_len;

	// EXTRA
	char   *extra;
	size_t  extra_data_len;

	// PRIVILEGES
	char   *privileges;
	size_t  privileges_data_len;

	// COLUMN_COMMENT
	char   *column_comment;
	size_t  column_comment_data_len;

} BPDB_MARIADB_TABLE_COLUMN_METADATA, *P_BPDB_MARIADB_TABLE_COLUMN_METADATA;


typedef struct __BPDB_MARIADB_TABLE_METADATA {

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% MariaDB Table Info %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Note: This information comes directly from information_schema.tables

	// table catalog information
	char   *table_catalog;
	size_t  table_catalog_data_len;

	// table schema information
	char   *table_schema;
	size_t  table_schema_data_len;

	// table name information
	char   *table_name;
	size_t  table_name_data_len;

	// table type information
	char   *table_type;
	size_t  table_type_data_len;

	// engine
	char   *engine;
	size_t  engine_data_len;

	// table version
	BPDB_MARIADB_BIGINT version;

	// table row format
	char   *row_format;
	size_t  row_format_data_len;

	// table rows
	BPDB_MARIADB_BIGINT table_rows;

	// average row length
	BPDB_MARIADB_BIGINT avg_row_length;

	// data length
	BPDB_MARIADB_BIGINT data_length;

	// max data length
	BPDB_MARIADB_BIGINT max_data_length;

	// index length
	BPDB_MARIADB_BIGINT index_length;

	// data free
	BPDB_MARIADB_BIGINT data_free;

	// auto increment
	BPDB_MARIADB_BIGINT auto_increment;

	// create time
	BPDB_MARIADB_BIGINT create_time;

	// update time
	BPDB_MARIADB_BIGINT update_time;

	// check time
	BPDB_MARIADB_BIGINT check_time;

	// table collation
	char   *table_collation;
	size_t  table_collation_data_len;

	// checksum
	size_t checksum;

	// create options
	char   *create_options;
	size_t  create_options_data_len;

	// table comment
	char   *table_comment;
	size_t  table_comment_data_len;

} BPDB_MARIADB_TABLE_METADATA, *P_BPDB_MARIADB_TABLE_METADATA;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% BPDB SQL Table Representation %%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Table column type categories.
typedef enum
{
	BPDB_SQL_TABLE_COLUMN_METADATA_TYPE_UNSET,
	BPDB_SQL_TABLE_COLUMN_METADATA_TYPE_INTEGER,
	BPDB_SQL_TABLE_COLUMN_METADATA_TYPE_BIN,
	BPDB_SQL_TABLE_COLUMN_METADATA_TYPE_TEXT
} BPDB_SQL_TABLE_COLUMN_METADATA_TYPE;


// This is the table name stored as a last entry in the a tables column set.  This name is
// reserved and cannot be used by programs utilizing the blockparty api.
#define BP_SQL_TABLE_COLUMN_METADATA_RESERVED_TABLE_NAME "bp_sql_table_column_metadata"

// Representation of a sql table column.
typedef struct __BPDB_SQL_TABLE_COLUMN_METADATA {

	// name of the column
	char * bpdb_column_name;

	// type of column being represented
	BPDB_SQL_TABLE_COLUMN_METADATA_TYPE type;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Column Indexes / Flags %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set if this column is a primary key
	BP_BOOL primary_key;

	// set if this column is indexed
	BP_BOOL indexed;

	// set if this column is unique
	BP_BOOL unique;

	// set if this column is auto increment
	BP_BOOL auto_inc;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% MariaDB Table Info %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Note: This information comes directly from information_schema.columns
	BPDB_MARIADB_TABLE_COLUMN_METADATA mariadb_column_metadata;



} BPDB_SQL_TABLE_COLUMN_METADATA, *P_BPDB_SQL_TABLE_COLUMN_METADATA;



// This is the table name stored as a last entry in the a tables column set.  This name is
// reserved and cannot be used by programs utilizing the blockparty api.
#define BP_SQL_TABLE_METADATA_RESERVED_TABLE_NAME "bp_sql_table_metadata"

// SQL table metadata.
typedef struct __BPDB_SQL_TABLE_METADATA {

	// name of the table
	char * bpdb_table_name;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Table Representations %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Checks if something exists in the persistent database.
	BP_BOOL exists_in_persistent_db;

	// Checks if something exists in the local cache database.
	BP_BOOL exists_in_localcache_db;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Table Stats %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// total number of columns in the table
	size_t column_count;


	// total number of records in the database currently
	size_t total_records_in;
	size_t total_records_in_for_persistent_db;
	size_t total_records_in_for_localcache_db;


	// total number of records written out to the persistent db
	size_t total_records_out;
	size_t total_records_out_for_persistent_db;
	size_t total_records_out_for_localcache_db;


	// total number of columns with a primary key designation
	size_t total_primary_key_cols;

	// total number of columns with indexes
	size_t total_indexed_cols;

	// total number of columns with unique designation
	size_t total_unique_cols;

	// total numbers of columns with auto_inc designation
	size_t total_auto_inc_cols;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% MariaDB Table Info %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this information is populated on connect (or when the metadata update routine is called)
	BPDB_MARIADB_TABLE_METADATA mariadb_table_metadata;



} BPDB_SQL_TABLE_METADATA, *P_BPDB_SQL_TABLE_METADATA;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% BPDB Main Structure %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Blockparty database handle.
typedef struct __BPDB {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Session Markers %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// uuid for this session
	char * uuid;

	// If the bpdb was initialized ok, this is set.  This should always be
	// checked as BP_DBOpen will always return a structure (so long as memory is available).
	// If this is set to BP_FALSE, you can check the messages registry to see why
	// the connection open failed.
	BP_BOOL init_ok;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% System Registry %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// system hash registry.  This contains all table/column/query metadata.
	P_BP_HASH_TABLE_REGISTRY system_hreg;

	// number of connection entries
	size_t connection_entry_n;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Sqlite Database Handles %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// sqlite handle
	P_BP_DB_SQLITE sqlite;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Configuration Settings %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// If his is enabled, every bpdb write will be mirrored between
	// the mysqldb and the sqlite db.  This is expensive and should
	// only be done with large transactions.
	BP_BOOL write_mirroring_enabled;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Error/Warning/Notice Messages Registry %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// messages registry
	P_BP_HASH_TABLE_REGISTRY messages_reg;

	// total number of messages in the registry
	size_t messages_reg_total_count;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% SQLGen Registries %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// When a sql generator is created, it is added to this registry by
	// uuid.  This registry can be used to track sql generators which
	// are reliant on this bpdb, as well as providing a position from
	// which we can retrieve query results for a query given just a uuid.
	// This functionality is mainly used by the JSON query subsystem.
	P_BP_HASH_TABLE_REGISTRY sqlgen_reg;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Validation Markers %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this is set to true if the connection is available
	BP_BOOL connection_is_available;


} BPDB, *P_BPDB;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Syntax Generator Type Definitions %%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

typedef enum
{
	BP_DBSYNTAX_GEN_REQUEST_TYPE_UNSET,
	BP_DBSYNTAX_GEN_REQUEST_TYPE_CREATE_TABLE,
	BP_DBSYNTAX_GEN_REQUEST_TYPE_SELECT_FROM_TABLE,
	BP_DBSYNTAX_GEN_REQUEST_TYPE_INSERT_INTO_TABLE
} BP_DBSYNTAX_GEN_REQUEST_TYPE;




typedef enum
{
	BPDB_SELECT_WHERE_VALUE_TYPE_UNSET,
	BPDB_SELECT_WHERE_VALUE_TYPE_NEGATIVE_INTEGER,
	BPDB_SELECT_WHERE_VALUE_TYPE_POSITIVE_INTEGER,
	BPDB_SELECT_WHERE_VALUE_TYPE_HEX_STRING,
	BPDB_SELECT_WHERE_VALUE_TYPE_QUOTED_LITERAL,
	BPDB_SELECT_WHERE_VALUE_TYPE_TABLE_AND_COLUMN
} BPDB_SELECT_WHERE_VALUE_TYPE;


// select where
typedef struct __BPDB_SELECT_WHERE {


	// the field name to be compared with the limiter (must be valid).  Examples
	// are like table.name, table_name.value_name etc.
	char * field_name;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Conjunctions / Limiters Errata %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// limiter (=, >=, <= etc)
	char * limiter;

	// conjunction for this value (AND/OR/etc)
	char * conjunction;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Value Information %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// value type
	BPDB_SELECT_WHERE_VALUE_TYPE value_type;

	// value
	char * value;
	size_t value_len;


} BPDB_SELECT_WHERE, *P_BPDB_SELECT_WHERE;

// enumerator
typedef enum
{
	BPDB_SELECT_JOIN_TYPE_NONE,
	BPDB_SELECT_JOIN_TYPE_RIGHT_JOIN,
	BPDB_SELECT_JOIN_TYPE_LEFT_JOIN,
	BPDB_SELECT_JOIN_TYPE_INNER_JOIN,
	BPDB_SELECT_JOIN_TYPE_CROSS_JOIN
} BPDB_SELECT_JOIN_TYPE;


// select join
typedef struct __BPDB_SELECT_JOIN {

	// the type of join to use
	BPDB_SELECT_JOIN_TYPE join_type;

	// the table to join
	char * table_name;

	// front part of the join
	char * front_join;

	// comparison
	char * comparison_operator;

	// tail part of the join
	char * tail_join;

} BPDB_SELECT_JOIN, *P_BPDB_SELECT_JOIN;


// limiter if necessary (use ex: LIMIT 0,10)
typedef struct __BPDB_SELECT_LIMITS {

	// the low limit
	size_t limit_low;

	// the high limit
	size_t limit_high;

} BPDB_SELECT_LIMITS, *P_BPDB_SELECT_LIMITS;


// order by clause
typedef struct __BPDB_SELECT_ORDER_BY {

	// field
	char * field;

	// ASC or DESC
	char * sort_designator;

} BPDB_SELECT_ORDER_BY, *P_BPDB_SELECT_ORDER_BY;


// group by clause
typedef struct __BPDB_SELECT_GROUP_BY {

	// group by field
	char * field;

} BPDB_SELECT_GROUP_BY, *P_BPDB_SELECT_GROUP_BY;


// Select generator
typedef struct __BPDB_SQLGEN_SELECT {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Query Parts %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// -- parts (NEW) --

	P_BP_HASH_TABLE_REGISTRY select_query_parts_hreg;


	// -- parts (OLD) --

	// fields to be selected
	char **                field_names;

	// table to select from
	char *                 table_name;

	// where clause
	P_BPDB_SELECT_WHERE    select_where;
	size_t                 select_where_entry_n;

	// limits to impose on the query tail
	P_BPDB_SELECT_LIMITS   limits;

	// join clauses
	P_BPDB_SELECT_JOIN     join_on;
	size_t                 join_on_entry_n;

	// order by clause
	P_BPDB_SELECT_ORDER_BY order_by;

	// group by clause
	P_BPDB_SELECT_GROUP_BY group_by;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Preliminary Lists (Allocated Internally) %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// These lists are generated by BP_DBCheckSelectInfoAndGenerateMetadataLists and are
	// utilized as preliminary

	// list of table names
	P_BP_LINKL table_name_list;

	// list of field names
	P_BP_LINKL field_name_list;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Prepared MariaDB Elements (Allocated Internally) %%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Note: Each of the parameters and results structures below
	//       are allocated with their own tailqueue allocator.  The
	//       reason they don't just use one allocator is that it's
	//       very likely that bind parameters will be destroyed between
	//       successive queries, whereas bound results can linger between
	//       queries.

	// Bound parameters for a prepared query.
	P_BP_LINKL   bind_parameters_tq_allocator;
	MYSQL_BIND * bind_parameters;
	size_t       bind_parameters_n;

	// Results which will be filled in.
	P_BP_LINKL   bind_results_tq_allocator;
	MYSQL_BIND * bind_results;
	size_t       bind_results_n;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Mariadb stmt %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// mariadb statement
	MYSQL_STMT * mariadb_stmt;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Mariadb Fetched Record Counter %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This gets iterated every time BP_DBSyntaxGenFetchResult is called successfully
	// on a record fetch.
	size_t fetched_record_n;

	// hash table registry mapping entries within this->bind_results to column
	// names in this->field_names
	P_BP_HASH_TABLE_REGISTRY fetched_record_hreg;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% JSON Registry %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This registry is used for the purposes of creating json and is not
	// to be confused with fetched_record_hreg.  While fetched_record_hreg
	// only contains MYSQL_BIND * pointers, this json hreg contains actual
	// data which would be useful to JSONAppendHReg.  This is data that
	// could be sent back to a client without issue.  This data is populated
	// when BP_DBSGFetchResult is called successfully.
	P_BP_HASH_TABLE_REGISTRY fetched_record_json_hreg;

} BPDB_SQLGEN_SELECT, *P_BPDB_SQLGEN_SELECT;



// SQL Syntax generator.
typedef struct __BPDB_SQLGEN {


	// associated bpdb handle.
	P_BPDB bpdb;

	// query uuid (used as key in pending_queries table)
	char * uuid;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Checked out Mariadb Connection for this Generator %%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this is the uuid that is associated with this sqlgen session
	P_BP_MYSQL_SESSION mariadb;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Semantic Containers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Type of this sql generation.  This value can be switch()ed to determine which
	// structure below has been propagated.
	BP_DBSYNTAX_GEN_REQUEST_TYPE type;

	// these structures hold semantic information about specific structures

	// select information if this query is a select query.
	P_BPDB_SQLGEN_SELECT select_info;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% System Hash Registry (used for allocations) %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// system hash table registry
	P_BP_HASH_TABLE_REGISTRY system_hreg;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Finalized Queries %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// generated query for sqlite
	char * sqlite_query;

	// generated query for mariadb
	char * mariadb_query;

	// If the query is finalized, this flag is set.  Finalizing is a term indicating
	// that no more query modifications can take place for this specific query.
	BP_BOOL query_generation_finalized;


} BPDB_SQLGEN, *P_BPDB_SQLGEN;





// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Open / Close BPDB Handle %%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempt to open a BPDB database from provided (already established) database
// handles.
P_BPDB BP_DBOpenFromHandles(P_BP_MYSQL_SESSION persistent_db, P_BP_DB_SQLITE local_db);

// Attempt to open a BPDB handle using standard creds.
P_BPDB BP_DBOpen
(
		char *mariadb_host,
		char *mariadb_database,
		size_t mariadb_port,
		char *mariadb_user,
		char *mariadb_pass,
		char *sqlite_db_file,
		char *sqlite_log_file,
		size_t mariadb_connection_pool_count = 1
);

// attempt to close the bpdb
BP_ERROR_T BP_DBClose(P_BPDB bpdb);

// --------- Close All Connections ------------------------------

// Attempts to close all connections in the mariadb connection pool.
BP_ERROR_T BP_DBCloseMariaDBConnectionPool(P_BPDB bpdb);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Clear Memory Used in Previous Queries %%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Destroy dangling memory from previous queries
BP_ERROR_T BP_DBDestroyMemoryFromAnyPreviousQueries(P_BPDB bpdb);

// destroy all registered sql syntax generators
BP_ERROR_T BP_DBDestroyAllRegisteredSQLSyntaxGenerators(P_BPDB bpdb);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Add Connection to Pool %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// adds a mariadb connection to the pool of available connections
BP_ERROR_T BP_DBAddMariadbConnectionToPool(P_BPDB bpdb, P_BP_MYSQL_SESSION mariadbi);

// attempts to search the mariadb connection pool and retrieve a connection by uuid
P_BP_MYSQL_SESSION BP_DBLookupMariadbConnectionByUUID(P_BPDB bpdb, char * uuid);

// Attempts to checkout/fetch a mariadb connection which is available and ready for
// interaction with a bpdb handle.
P_BP_MYSQL_SESSION BP_DBCheckOutAvailableMariadbConnection(P_BPDB bpdb);

// Attempts to checkin a mariadb connection after you're done with use.  All data
// from previous queries are cleared after checkin of a connection.
BP_ERROR_T BP_DBCheckInMariadbConnection(P_BPDB bpdb, P_BP_MYSQL_SESSION mariadbi);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Tables and Columns %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// add a table.
BP_ERROR_T BP_DBTableAdd
(
		P_BPDB                  bpdb,
		char *                  table_name,
		P_BPDB_SQL_TABLE_COLUMN_METADATA columns,
		size_t                  column_n
);

// Add a column to a table.  Adds one column only.
BP_ERROR_T BP_DBColumnAdd
(
		P_BPDB bpdb,
		char *table_name,
		char *column_name,
		BPDB_SQL_TABLE_COLUMN_METADATA_TYPE column_type,
		BP_BOOL primary_key,
		BP_BOOL indexed,
		BP_BOOL unique,
		BP_BOOL auto_inc
);

// Add multiple columns to a table at once.  Columns are copied using BP_DBColumnAdd iteratively.
BP_ERROR_T BP_DBColumnsAdd
(
		P_BPDB                  bpdb,
		char *                  table_name,
		P_BPDB_SQL_TABLE_COLUMN_METADATA columns,
		size_t                  column_n
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Schema Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// install local schema
BP_ERROR_T BP_DBSchemaInstallLocal(P_BPDB bpdb);

// simple routine to sintall a table into a buffer
BP_ERROR_T BP_DBSchemaInstallTableFromBufferLocal(P_BPDB bpdb, char * table_sql);






// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Cache Insertion Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// TODO TOMORROW MORNING
typedef enum
{

	BPDB_SELECTOR_TYPE_UNSET,
	BPDB_SELECTOR_TYPE_LOCALCACHE_DATABASE,
	BPDB_SELECTOR_TYPE_PERSISTENT_DATABASE,
	BPDB_SELECTOR_TYPE_BOTH_DATABASES

} BPDB_SELECTOR_TYPE;

// Record entries.
typedef struct __BPDB_RECORD_ENTRY {

	// type of record being inserted (must match
	// the type within the column the data is being
	// inserted into)
	BPDB_SQL_TABLE_COLUMN_METADATA_TYPE type;

	// database selector for this record
	BPDB_SELECTOR_TYPE db_selector;


	// data for this record
	void * data;

	// size of the data being stored
	size_t size;

} BPDB_RECORD_ENTRY, *P_BPDB_RECORD_ENTRY;


// Database entries.
typedef struct __BPDB_RECORD_ENTRIES {

	// Array of entries.
	P_BPDB_RECORD_ENTRY entries;

	// numbe of entries
	size_t entries_n;

} BPDB_RECORD_ENTRIES, *P_BPDB_RECORD_ENTRIES;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Query Modification Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Generates a CREATE TABLE sql query from the table specified by table_name.  Table must
// be found within the associated bpdb handle.
//
// finalizes query: yes
//
BP_ERROR_T BP_DBSyntaxGenCreateTable(P_BPDB_SQLGEN syngen, char *table_name);

// Generates a select query
BP_ERROR_T BP_DBSyntaxGenSelectFrom
(
	P_BPDB_SQLGEN        syngen,
	P_BPDB_SQLGEN_SELECT select_info
);

// Generates an insert query from the
BP_ERROR_T BP_DBSyntaxGenInsertInto(P_BPDB_SQLGEN syngen, BPDB_SELECTOR_TYPE out_db_selector, char *table_name, size_t max_records);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Flush Records Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// This attempts to flush records out to the database.
BP_ERROR_T BP_DBFlushRecordsOut(P_BPDB bpdb, BPDB_SELECTOR_TYPE out_db_selector, BP_BOOL run_in_transaction);

// Called from BP_DBFlushRecordsOut.  Will flush records to the local sqlite db if possible.
BP_ERROR_T BP_DBFlushRecordsOutToLocalCache(P_BPDB bpdb, P_BPDB_SQLGEN syngen, BP_BOOL run_in_transaction);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Lookup Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// --- table entry lookups ----

// looks up a table by a table name.
P_BP_HASH_TABLE_KEYED_ENTRY BP_DBLookupTableByName(P_BPDB bpdb, char * table_name);

// looks up the columns table for a given table name.  This is the keyed entry for the
// tables/specific_table/columns/ path.
P_BP_HASH_TABLE_KEYED_ENTRY BP_DBLookupTableColumns(P_BPDB bpdb, char *table_name);

// looks up a column given a table name and column name for lookup
P_BP_HASH_TABLE_KEYED_ENTRY BP_DBLookupTableColumnByName(P_BPDB bpdb, char *table_name, char *column_name);

// this is a utility version of BP_DBLookupTableColumnByName that takes a field in the form of table.column, and
// automatically splits it before calling BP_DBLookupTableColumnByName to attempt lookup.
P_BP_HASH_TABLE_KEYED_ENTRY BP_DBLookupTableColumnByJoinedFieldName(P_BPDB bpdb, char *joined_table_column_field);



// --- metadata lookups ----

// attempt to lookup table metadata by name.
P_BPDB_SQL_TABLE_METADATA BP_DBLookupTableMetadataFromTableName(P_BPDB bpdb, char *table_name);

// attempt to lookup table column metatadata by name
P_BPDB_SQL_TABLE_COLUMN_METADATA BP_DBLookupTableColumnMetadataFromColumnName(P_BPDB bpdb, char *table_name, char *column_name);

// attempt to lookup table column metatadata by name
P_BPDB_SQL_TABLE_COLUMN_METADATA BP_DBLookupTableColumnMetadataFromJoinedFieldName(P_BPDB bpdb, char *joined_table_column_field);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Mariadb Column Information Resolution Routines %%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// this takes in mariadb metadata and provides a numeric type
BPDB_MARIADB_COLUMN_TYPE BP_DBResolveMariadbColumnMetadataToType(P_BPDB_MARIADB_TABLE_COLUMN_METADATA mariadb_column_metadata);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Numeric Utilities for BPDB %%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempts to retrieve the number of columns for a specified table
size_t BP_DBGetColumnCountForTable
(
		P_BPDB bpdb,
		char *table_name
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Utility Conversion Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// converts string to a bpcalloced string version of the metadata type.
char * BP_DBColumnTypeToString(BPDB_SQL_TABLE_COLUMN_METADATA_TYPE type);

// Simple macro for making the code within BP_DBMariadbColumnTypeToString easier to read.  Should not be used
// outside of BP_DBMariadbColumnTypeToString.
#define BP_DB_MARIADB_COL_TYPE_STR_ALLOC(str)\
		hreg ? ht_strdup(hreg, str) : bpstrdup(str);

// Attempts to convert a maria db type to string.  This is mainly used for the creation of JSON
// record outputs.  If the hreg parameter is non-null, the allocation returned will be ht_strdup'ed
// rather than bpstrduped.  Result must be ht_free/bpfree'd accordingly.
char * BP_DBMariadbColumnTypeToString(BPDB_MARIADB_COLUMN_TYPE type, P_BP_HASH_TABLE_REGISTRY hreg = NULL);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Result Data Autoconversion %%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This function formats result information into proper output for json.  This is mainly used for autojson creation.  This
// will call one of the functions below by switching off based on type.  Each basic type has it's own function to make things
// developmentally clearer.
char * BP_DBMariadbConvertColumnResultDataToJSONData(MYSQL_BIND * col_bind_info, BPDB_MARIADB_COLUMN_TYPE type);


// --- Note: The three routines below are a composite and are called directly from BP_DBMariadbConvertColumnResultDataToJSONData ---

// Attempts to convert INTEGER column result data to JSON data.
char * BP_DBMariadbConvertIntegerColumnResultDataToJSONData(MYSQL_BIND * col_bind_info, BPDB_MARIADB_COLUMN_TYPE type);

// Attempts to convert BLOB column result data to base64 encoded output.
char * BP_DBMariadbConvertBlobColumnResultDataToJSONData(MYSQL_BIND * col_bind_info, BPDB_MARIADB_COLUMN_TYPE type);

// Attempt to convert a time type to a json string.
char * BP_DBMariadbConvertTimeColumnResultToJSONData(MYSQL_BIND * col_bind_info, BPDB_MARIADB_COLUMN_TYPE type);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// display a bpdb handle
BP_ERROR_T BP_DBDisplay
(
	P_BPDB bpdb,
	BP_BOOL display_mariadb_table_metadata  = BP_FALSE,
	BP_BOOL display_mariadb_column_metadata = BP_FALSE,
	BP_BOOL display_sqlite_table_metadata   = BP_FALSE,
	BP_BOOL display_sqlite_column_metadata = BP_FALSE
);

// display any syntax generators registered to this bpdb
BP_ERROR_T BP_DBDisplayRegisteredSyntaxGenerators(P_BPDB bpdb);

// display tables directly (only tables)
BP_ERROR_T BP_DBDisplayTablesAndColumns
(
	P_BPDB bpdb,
	BP_BOOL display_mariadb_table_metadata  = BP_FALSE,
	BP_BOOL display_mariadb_column_metadata = BP_FALSE,
	BP_BOOL display_sqlite_table_metadata   = BP_FALSE,
	BP_BOOL display_sqlite_column_metadata = BP_FALSE
);

// TODO: Display pending queries.

// Display mariadb table metadata
BP_ERROR_T BP_DBDisplayMariaDBTableMetadata
(
	P_BPDB bpdb,
	P_BPDB_MARIADB_TABLE_METADATA table_metadata,
	size_t tab_indent_n
);

// Display mariadb table column metadata
BP_ERROR_T BP_DBDisplayMariaDBTableColumnMetadata
(
	P_BPDB bpdb,
	P_BPDB_MARIADB_TABLE_COLUMN_METADATA table_column_metadata,
	size_t tab_indent_n
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% SQL Generator Lookups %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempt to lookup sql generator.
P_BPDB_SQLGEN BP_DBLookupSQLGeneratorByUUID(P_BPDB bpdb, char *uuid);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% JSON SQLGen Query Part Separators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to separate right join string elements into their parts (stored in join_data_in_out)
BP_ERROR_T BP_DBJSONSeparateRightJoinStringIntoParts
(
	P_BPDB_SQLGEN syngen,
	P_BPDB_SELECT_JOIN join_data_in_out,
	char *string_elements
);

// attempt to separate right join string elements into their parts (stored in join_data_in_out)
BP_ERROR_T BP_DBJSONSeparateLeftJoinStringIntoParts
(
	P_BPDB_SQLGEN syngen,
	P_BPDB_SELECT_JOIN join_data_in_out,
	char *string_elements
);

// attempt to separate where-clause string elements into their parts (stored in join_data_in_out)
BP_ERROR_T BP_DBJSONSeparateWhereClauseStringIntoParts
(
	P_BPDB_SQLGEN syngen,
	P_BPDB_SELECT_WHERE select_clause_in_out,
	char *string_elements
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% JSON Query Interface %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// attempt to run a stack of queries from JSON
P_BP_JSON BP_DBRunQueriesFromJSON(P_BPDB bpdb, P_BP_JSON json_query);


// -- run various sub query types ----------------

// Run select query from provided json.  Since queries can come in stacked, you need to provide a query start position.  Query run
// information is appended to the json_query structure within the "result" field.
BP_ERROR_T BP_DBRunSelectQueryFromJSON_internal(P_BPDB bpdb, P_BP_JSON json_query);

// Run an insert query from provided json.  Since queries can come in stacked, you need to provide a query start position.
BP_ERROR_T BP_DBRunInsertQueryFromJSON_internal(P_BPDB bpdb, P_BP_JSON json_query);

// Run an update query from provided json.  Since queries can come in stacked, you need to provide a query start position.
BP_ERROR_T BP_DBRunUpdateQueryFromJSON_internal(P_BPDB bpdb, P_BP_JSON json_query);







// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Fetch Results Into JSON %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// attempt to fetch next result from JSON provided a lookup path query (eg. /select/results)
BP_ERROR_T BP_DBFetchNextResultIntoJSON
(
	P_BPDB bpdb,
	char *results_path_lookup,
	P_BP_JSON completed_json_query,
	BP_BOOL destroy_results_before_stacking_result
);

// attempt to fetch all results
BP_ERROR_T BP_DBFetchAllResultsIntoJSON
(
	P_BPDB    bpdb,
	char     *results_path_lookup,
	P_BP_JSON completed_json_query
);





#endif /* __BP_DB_H */

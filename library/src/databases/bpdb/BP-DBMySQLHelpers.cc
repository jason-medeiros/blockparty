/*
 * BP-DBMySQLHelpers.h
 *
 *  Created on: Oct 12, 2015
 *      Author: root
 */

// include BP main
#include "../../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Auto Load Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Attempt to retieve all tables from the database opened with the current bpdb handle.
BP_ERROR_T BP_DBMySQLAutoLoadAllTables(P_BPDB bpdb)
{


	// negative checks
	if(!bpdb)
		return ERR_FAILURE;

	// enter critical section
	BPDB_ENTER_CRITICAL_SECTION;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Checkout Mariab Connection from Pool %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt checkout
	P_BP_MYSQL_SESSION mariadbi = BP_DBCheckOutAvailableMariadbConnection(bpdb);
	if(!mariadbi)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBMySQLAutoLoadAllTables attempted to call BP_DBCheckOutAvailableMariadbConnection but was unable to checkout a connection.");
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);
	}

	if(!mariadbi->conn)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBMySQLAutoLoadAllTables attempted to call BP_DBCheckOutAvailableMariadbConnection, and was able to checkout a connection, but that connection has a NULL connection pointer (meaning the connection is invalid or has gone away/been destroyed).");
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);
	}

	if(!mariadbi->connected)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBMySQLAutoLoadAllTables attempted to call BP_DBCheckOutAvailableMariadbConnection, and was able to checkout a connection, but the connection is not marked as connected.");
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);
	}

	if(mariadbi->server_gone_away)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBMySQLAutoLoadAllTables attempted to call BP_DBCheckOutAvailableMariadbConnection, and was able to checkout a connection, but that connection is marked as having a server which has gone-away.");
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);
	}

	if(mariadbi->server_lost_connection)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBMySQLAutoLoadAllTables attempted to call BP_DBCheckOutAvailableMariadbConnection, and was able to checkout a connection, but that connection is marked as having a server which has lost connection.");
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);
	}


	printf("\n [+] dbg: Checked out mariadb entry %p: %s", mariadbi, mariadbi->uuid);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Prepare Query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// sql query to pull all tables from an arbitrary database.
	char * table_retrieval_query = \
			"SELECT                 "\
			"TABLE_CATALOG,         "\
			"TABLE_SCHEMA,          "\
			"TABLE_NAME,            "\
			"TABLE_TYPE,            "\
			"ENGINE,                "\
			"VERSION,               "\
			"ROW_FORMAT,            "\
			"TABLE_ROWS,            "\
			"AVG_ROW_LENGTH,        "\
			"DATA_LENGTH,           "\
			"MAX_DATA_LENGTH,       "\
			"INDEX_LENGTH,          "\
			"DATA_FREE,             "\
			"AUTO_INCREMENT,        "\
			"CREATE_TIME,           "\
			"UPDATE_TIME,           "\
			"CHECK_TIME,            "\
			"TABLE_COLLATION,       "\
			"CHECKSUM,              "\
			"CREATE_OPTIONS,        "\
			"TABLE_COMMENT          "\
			"FROM                   "\
			"INFORMATION_SCHEMA.TABLES WHERE TABLE_TYPE = 'BASE TABLE' AND TABLE_SCHEMA=?";

	// attempt to prepare the query
	MYSQL_STMT * stmt = BP_MySQLPrepareQuery
	(
		mariadbi,
		table_retrieval_query
	);

	// ensure the statement was created ok
	if(!stmt)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBMySQLAutoLoadAllTables  Could not prepare SELECT statement.");
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);
	}

	// display statement prep message
	// printf("\n [+] Statement prepared OK!");



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Bind Parameter(s) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create space for parameter
	MYSQL_BIND bind_params[2];

	// zero out the bind parameters
	BP_ZERO_STACK(bind_params);

	// you can set this to whatever you want so long it's an actual db
	char * db_to_fetch_tables_from = "grief3";

	// set table name
	unsigned long int db_table_name_len = bpstrlen(db_to_fetch_tables_from);


	// set parameter values
	bind_params[0].buffer_type     = MYSQL_TYPE_STRING;
	bind_params[0].buffer          = db_to_fetch_tables_from;
	bind_params[0].is_null         = 0;
	bind_params[0].buffer_length   = bpstrlen(db_to_fetch_tables_from);


	// bind parameters
	if(BP_MySQLBindParameters(stmt, (MYSQL_BIND *) &bind_params, mariadbi) == ERR_FAILURE)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBMySQLAutoLoadAllTables failed to bind parameters to SELECT query.");
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);
	}

	// display bind success message
	// printf("\n [+] Parameters bound OK!");



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Execute Statement %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// attempt to execute the statement
	if(BP_MySQLExecuteStatement(stmt, mariadbi) == ERR_FAILURE)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBMySQLAutoLoadAllTables failed to execute SELECT query.");
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);
	}

	// display execution message
	// printf("\n [+] Statement executed ok!");


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Prepare Results For Bind %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



	// this structure will hold the table data
	BPDB_MARIADB_TABLE_METADATA mariadb_table_metadata;
	BP_ZERO_STACK(mariadb_table_metadata);

	// results
	MYSQL_BIND result[22];

	// zero out results structure
	BP_ZERO_STACK(result);

	// result index
	size_t result_idx = 0;



	// 1)  TABLE_CATALOG
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;

	// 2)  TABLE_SCHEMA
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;

	// 3)  TABLE_NAME
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;

	// 4)  TABLE_TYPE
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;

	// 5)  ENGINE
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;

	// 6)  VERSION
	result[result_idx].buffer_type   = MYSQL_TYPE_LONGLONG;
	result_idx++;

	// 7)  ROW_FORMAT
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;

	// 8)  TABLE_ROWS
	result[result_idx].buffer_type   = MYSQL_TYPE_LONGLONG;
	result_idx++;

	// 9)  AVG_ROW_LENGTH
	result[result_idx].buffer_type   = MYSQL_TYPE_LONGLONG;
	result_idx++;

	// 10) DATA_LENGTH
	result[result_idx].buffer_type   = MYSQL_TYPE_LONGLONG;
	result_idx++;

	// 11) MAX_DATA_LENGTH
	result[result_idx].buffer_type   = MYSQL_TYPE_LONGLONG;
	result_idx++;

	// 12) INDEX_LENGTH
	result[result_idx].buffer_type   = MYSQL_TYPE_LONGLONG;
	result_idx++;

	// 13) DATA_FREE
	result[result_idx].buffer_type   = MYSQL_TYPE_LONGLONG;
	result_idx++;

	// 14) AUTO_INCREMENT
	result[result_idx].buffer_type   = MYSQL_TYPE_LONGLONG;
	result_idx++;

	// 15) CREATE_TIME
	result[result_idx].buffer_type   = MYSQL_TYPE_LONGLONG;
	result_idx++;

	// 16) UPDATE_TIME
	result[result_idx].buffer_type   = MYSQL_TYPE_LONGLONG;
	result_idx++;

	// 17) CHECK_TIME
	result[result_idx].buffer_type   = MYSQL_TYPE_LONGLONG;
	result_idx++;

	// 18) TABLE_COLLATION
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;

	// 19) CHECKSUM
	result[result_idx].buffer_type   = MYSQL_TYPE_LONGLONG;
	result_idx++;

	// 20) CREATE_OPTIONS
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;

	// 21) TABLE_COMMENT
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;




	// bind the result
	if(BP_MySQLBindResult(stmt, (MYSQL_BIND *) &result, result_idx, mariadbi) == ERR_FAILURE)
	{

		// return indicating failure
		BP_DBAddErrorMsg(bpdb, "BP_DBMySQLAutoLoadAllTables binding results failed when attempting to retrieve metadata from SELECT.");
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);

	}

	// display bind message
	// printf("\n [+] Result bound to statement ok!");

	// attempt to store results (fetch from server)
	if(BP_MySQLStoreResults(stmt, mariadbi) == ERR_FAILURE)
	{

		// return indicating failure
		BP_DBAddErrorMsg(bpdb, "BP_DBMySQLAutoLoadAllTables tried to call BP_MySQLStoreResults to store results, but we failed even though we're expecting results.");
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);

	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Fetch Results from Stored Result Set %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// simple enumerator
	enum
	{
		TABLE_CATALOG,
		TABLE_SCHEMA,
		TABLE_NAME,
		TABLE_TYPE,
		ENGINE,
		VERSION,
		ROW_FORMAT,
		TABLE_ROWS,
		AVG_ROW_LENGTH,
		DATA_LENGTH,
		MAX_DATA_LENGTH,
		INDEX_LENGTH,
		DATA_FREE,
		AUTO_INCREMENT,
		CREATE_TIME,
		UPDATE_TIME,
		CHECK_TIME,
		TABLE_COLLATION,
		CHECKSUM,
		CREATE_OPTIONS,
		TABLE_COMMENT
	};


	// set table path
	char *table_path[4];

	// zero the table
	BP_ZERO_STACK(table_path);


	// fetch results and display table
	while(BP_MySQLFetchResult(stmt, mariadbi) == ERR_SUCCESS)
	{


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Add Table  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// set initial path
		table_path[0] = "tables";
		table_path[1] = (char *) result[TABLE_NAME].buffer; // (char *) &result[TABLE_NAME].buffer;
		table_path[2] = NULL;

		// add table to the db
		BP_ERROR_T ret_val = BP_HashRegAddTableByPath(bpdb->system_hreg, (char **) &table_path);
		if(!ret_val)
		{
			// create some dynamic stack space
			char tmp_str[bpstrlen((char *) result[TABLE_NAME].buffer)+512];
			BP_ZERO_STACK(tmp_str);

			// copy the buffer in
			sprintf((char *) &tmp_str, "\n BP_DBMySQLAutoLoadAllTables tried to add table %s and failed", result[TABLE_NAME].buffer);

			// add error and exit
			BP_DBAddErrorMsg(bpdb, (char *) &tmp_str);
			BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);
		}

		// zero the table
		BP_ZERO_STACK(table_path);



		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Add Metadata %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// add metadata entry for table here now that the column has been added
		P_BP_HASH_TABLE_KEYED_ENTRY table_lookup = BP_DBLookupTableByName(bpdb, (char *) result[TABLE_NAME].buffer);
		if(!table_lookup)
		{

			// create some dynamic stack space
			char tmp_str[bpstrlen((char *) result[TABLE_NAME].buffer)+512];
			BP_ZERO_STACK(tmp_str);

			// copy the buffer in
			sprintf((char *) &tmp_str, "\n BP_DBMySQLAutoLoadAllTables tried to lookup table %s and failed", result[TABLE_NAME].buffer);

			// add error and exit
			BP_DBAddErrorMsg(bpdb, (char *) &tmp_str);
			BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);
		}

		// attempt to add metadata entry to the table
		if
		(
			!BP_HashRegAddDataToTable
			(
				bpdb->system_hreg,
				table_lookup,
				BP_SQL_TABLE_METADATA_RESERVED_TABLE_NAME, /* in BP-DB.h */
				BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
				NULL,
				sizeof(BPDB_SQL_TABLE_METADATA)
			)
		)
		{

			// create some dynamic stack space
			char tmp_str[bpstrlen((char *) result[TABLE_NAME].buffer)+512];
			BP_ZERO_STACK(tmp_str);

			// copy the buffer in
			sprintf((char *) &tmp_str, "\n BP_DBMySQLAutoLoadAllTables tried to add metadata for table %s and failed", result[TABLE_NAME].buffer);

			// add error and exit
			BP_DBAddErrorMsg(bpdb, (char *) &tmp_str);
			BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);

		}

		// set lookup path
		char * lookup_path[] =
		{
			"tables",
			(char *) result[TABLE_NAME].buffer,
			BP_SQL_TABLE_METADATA_RESERVED_TABLE_NAME,
			NULL
		};

		// retrive table metadata by path
		P_BP_HASH_TABLE_KEYED_ENTRY table_metadata_lookup = BP_HashRegLookupTableEntryByPath(bpdb->system_hreg, (char **) &lookup_path);
		if(!table_metadata_lookup)
		{

			// create some dynamic stack space
			char tmp_str[bpstrlen((char *) result[TABLE_NAME].buffer)+512];
			BP_ZERO_STACK(tmp_str);

			// copy the buffer in
			sprintf((char *) &tmp_str, "\n BP_DBMySQLAutoLoadAllTables tried to lookup metadata (after addition) for table %s and failed", result[TABLE_NAME].buffer);

			// add error and exit
			BP_DBAddErrorMsg(bpdb, (char *) &tmp_str);
			BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);

		}

		// set table metadata
		P_BPDB_SQL_TABLE_METADATA table_metadata = (P_BPDB_SQL_TABLE_METADATA) table_metadata_lookup->data;
		if(!table_metadata)
		{

			// create some dynamic stack space
			char tmp_str[bpstrlen((char *) result[TABLE_NAME].buffer)+512];
			BP_ZERO_STACK(tmp_str);

			// copy the buffer in
			sprintf((char *) &tmp_str, "\n BP_DBMySQLAutoLoadAllTables tried to dereference metadata (after addition) for table %s and this resulted in a NULL pointer.  Corruption detected.", result[TABLE_NAME].buffer);

			// add error and exit
			BP_DBAddErrorMsg(bpdb, (char *) &tmp_str);
			BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);

		}


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Fill in Metadata Structure %%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// NOTE: All flags are off by default due to memset to 0x00.

		// it exists in persistent database
		table_metadata->exists_in_persistent_db = BP_TRUE;



		// === TABLE_CATALOG =====================

		// allocate space for table catalog
		table_metadata->mariadb_table_metadata.table_catalog = (char *) ht_calloc
		(
			bpdb->system_hreg,
			(size_t) result[TABLE_CATALOG].buffer_length + 1,
			1
		);

		// copy in data
		memcpy
		(
			(void *) table_metadata->mariadb_table_metadata.table_catalog,
			(void *) result[TABLE_CATALOG].buffer,
			(size_t) result[TABLE_CATALOG].buffer_length
		);

		// set length
		table_metadata->mariadb_table_metadata.table_catalog_data_len = result[TABLE_CATALOG].buffer_length;


		// === TABLE_SCHEMA =====================

		// allocate space for table catalog
		table_metadata->mariadb_table_metadata.table_schema = (char *) ht_calloc
		(
			bpdb->system_hreg,
			(size_t) result[TABLE_SCHEMA].buffer_length + 1,
			1
		);

		// copy in data
		memcpy
		(
			(void *) table_metadata->mariadb_table_metadata.table_schema,
			(void *) result[TABLE_SCHEMA].buffer,
			(size_t) result[TABLE_SCHEMA].buffer_length
		);

		// set length
		table_metadata->mariadb_table_metadata.table_schema_data_len = result[TABLE_SCHEMA].buffer_length;


		// === TABLE_NAME =======================

		// allocate space for table catalog
		table_metadata->mariadb_table_metadata.table_name = (char *) ht_calloc
		(
			bpdb->system_hreg,
			(size_t) result[TABLE_NAME].buffer_length + 1,
			1
		);

		// copy in data
		memcpy
		(
			(void *) table_metadata->mariadb_table_metadata.table_name,
			(void *) result[TABLE_NAME].buffer,
			(size_t) result[TABLE_NAME].buffer_length
		);

		// set length
		table_metadata->mariadb_table_metadata.table_name_data_len = result[TABLE_NAME].buffer_length;


		// === TABLE_TYPE =======================

		// allocate space for table catalog
		table_metadata->mariadb_table_metadata.table_type = (char *) ht_calloc
		(
			bpdb->system_hreg,
			(size_t) result[TABLE_TYPE].buffer_length + 1,
			1
		);

		// copy in data
		memcpy
		(
			(void *) table_metadata->mariadb_table_metadata.table_type,
			(void *) result[TABLE_TYPE].buffer,
			(size_t) result[TABLE_TYPE].buffer_length
		);

		// set length
		table_metadata->mariadb_table_metadata.table_type_data_len = result[TABLE_TYPE].buffer_length;


		// === ENGINE ===========================

		// allocate space for table catalog
		table_metadata->mariadb_table_metadata.engine = (char *) ht_calloc
		(
			bpdb->system_hreg,
			(size_t) result[ENGINE].buffer_length + 1,
			1
		);

		// copy in data
		memcpy
		(
			(void *) table_metadata->mariadb_table_metadata.engine,
			(void *) result[ENGINE].buffer,
			(size_t) result[ENGINE].buffer_length
		);

		// set length
		table_metadata->mariadb_table_metadata.engine_data_len = result[ENGINE].buffer_length;


		// === VERSION ==========================
		memcpy
		(
			(void *) &table_metadata->mariadb_table_metadata.version,
			(void *) result[VERSION].buffer,
			result[VERSION].buffer_length
		);


		// === ROW_FORMAT =======================

		// allocate space for table catalog
		table_metadata->mariadb_table_metadata.row_format = (char *) ht_calloc
		(
			bpdb->system_hreg,
			(size_t) result[ROW_FORMAT].buffer_length + 1,
			1
		);

		// copy in data
		memcpy
		(
			(void *) table_metadata->mariadb_table_metadata.row_format,
			(void *) result[ROW_FORMAT].buffer,
			(size_t) result[ROW_FORMAT].buffer_length
		);

		// set length
		table_metadata->mariadb_table_metadata.row_format_data_len = result[ROW_FORMAT].buffer_length;


		// === TABLE_ROWS =======================
		memcpy
		(
			(void *) &table_metadata->mariadb_table_metadata.table_rows,
			(void *) result[TABLE_ROWS].buffer,
			result[TABLE_ROWS].buffer_length
		);

		// === AVG_ROW_LENGTH ===================
		memcpy
		(
			(void *) &table_metadata->mariadb_table_metadata.avg_row_length,
			(void *) result[AVG_ROW_LENGTH].buffer,
			result[AVG_ROW_LENGTH].buffer_length
		);

		// === DATA_LENGTH ======================
		memcpy
		(
			(void *) &table_metadata->mariadb_table_metadata.data_length,
			(void *) result[DATA_LENGTH].buffer,
			result[DATA_LENGTH].buffer_length
		);

		// === MAX_DATA_LENGTH ==================
		memcpy
		(
			(void *) &table_metadata->mariadb_table_metadata.max_data_length,
			(void *) result[MAX_DATA_LENGTH].buffer,
			result[MAX_DATA_LENGTH].buffer_length
		);

		// === INDEX_LENGTH =====================
		memcpy
		(
			(void *) &table_metadata->mariadb_table_metadata.index_length,
			(void *) result[INDEX_LENGTH].buffer,
			result[INDEX_LENGTH].buffer_length
		);

		// === DATA_FREE ========================
		memcpy
		(
			(void *) &table_metadata->mariadb_table_metadata.data_free,
			(void *) result[DATA_FREE].buffer,
			result[DATA_FREE].buffer_length
		);

		// === AUTO_INCREMENT ===================
		memcpy
		(
			(void *) &table_metadata->mariadb_table_metadata.auto_increment,
			(void *) result[AUTO_INCREMENT].buffer,
			result[AUTO_INCREMENT].buffer_length
		);

		// === CREATE_TIME ======================
		memcpy
		(
			(void *) &table_metadata->mariadb_table_metadata.create_time,
			(void *) result[CREATE_TIME].buffer,
			result[CREATE_TIME].buffer_length
		);

		// === UPDATE_TIME ======================
		memcpy
		(
			(void *) &table_metadata->mariadb_table_metadata.update_time,
			(void *) result[UPDATE_TIME].buffer,
			result[UPDATE_TIME].buffer_length
		);

		// === CHECK_TIME =======================
		memcpy
		(
			(void *) &table_metadata->mariadb_table_metadata.check_time,
			(void *) result[CHECK_TIME].buffer,
			result[CHECK_TIME].buffer_length
		);

		// === TABLE_COLLATION ==================

		// allocate space for table catalog
		table_metadata->mariadb_table_metadata.table_collation = (char *) ht_calloc
		(
			bpdb->system_hreg,
			(size_t) result[TABLE_COLLATION].buffer_length + 1,
			1
		);

		// copy in data
		memcpy
		(
			(void *) table_metadata->mariadb_table_metadata.table_collation,
			(void *) result[TABLE_COLLATION].buffer,
			(size_t) result[TABLE_COLLATION].buffer_length
		);

		// set length
		table_metadata->mariadb_table_metadata.table_collation_data_len = result[TABLE_COLLATION].buffer_length;


		// === CHECKSUM =========================
		memcpy
		(
			(void *) &table_metadata->mariadb_table_metadata.checksum,
			(void *) result[CHECKSUM].buffer,
			result[CHECKSUM].buffer_length
		);

		// === CREATE_OPTIONS ===================

		// allocate space for table catalog
		table_metadata->mariadb_table_metadata.create_options = (char *) ht_calloc
		(
			bpdb->system_hreg,
			(size_t) result[CREATE_OPTIONS].buffer_length + 1,
			1
		);

		// copy in data
		memcpy
		(
			(void *) table_metadata->mariadb_table_metadata.create_options,
			(void *) result[CREATE_OPTIONS].buffer,
			(size_t) result[CREATE_OPTIONS].buffer_length
		);

		// set length
		table_metadata->mariadb_table_metadata.create_options_data_len = result[CREATE_OPTIONS].buffer_length;


		// === TABLE_COMMENT ====================

		// allocate space for table catalog
		table_metadata->mariadb_table_metadata.table_comment = (char *) ht_calloc
		(
			bpdb->system_hreg,
			(size_t) result[TABLE_COMMENT].buffer_length + 1,
			1
		);

		// copy in data
		memcpy
		(
			(void *) table_metadata->mariadb_table_metadata.table_comment,
			(void *) result[TABLE_COMMENT].buffer,
			(size_t) result[TABLE_COMMENT].buffer_length
		);

		// set length
		table_metadata->mariadb_table_metadata.table_comment_data_len = result[TABLE_COMMENT].buffer_length;


	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Now Gather Column Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set table lookup path
	char * table_lookup_path[4];

	// set tables as the lookup path
	table_lookup_path[0] = "tables";
	table_lookup_path[1] = NULL;

	// iterate through items and search
	BP_HASH_ITER_FROM_LOOKUP_PATH_OPEN(bpdb->system_hreg, table_lookup_path, table_name_entry_iter)
	{

		// table name entry iterator.
		if(table_name_entry_iter)
		{

			// dbg: Display table
			printf("\n [+] dbg: loading table %s", table_name_entry_iter->key);

			// attempt to load columns
			if(!BP_DBMySQLAutoLoadTableColumns(bpdb, table_name_entry_iter->key))
			{
				printf("\n [+] dbg: failed to load columns for table %s", table_name_entry_iter->key);
			}

		}


	} BP_HASH_ITER_FROM_LOOKUP_PATH_CLOSE;

	// return indicating success
	BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_SUCCESS);

}

// table columns
BP_ERROR_T BP_DBMySQLAutoLoadTableColumns(P_BPDB bpdb, char *table_name)
{


	// negative checks
	if(!bpdb)
		return ERR_FAILURE;
	if(!bpdb->system_hreg)
		return ERR_FAILURE;

	// enter critical section
	BPDB_ENTER_CRITICAL_SECTION;

	// run the mariadb prologue.  Return err failure if any checks fail.
	BPDB_CHECKOUT_MARIADB_PROLOGUE(ERR_FAILURE);




	// table position iterator
	enum
	{
		TABLE_CATALOG,
		TABLE_SCHEMA,
		TABLE_NAME,
		COLUMN_NAME,
		ORDINAL_POSITION,
		COLUMN_DEFAULT,
		IS_NULLABLE,
		DATA_TYPE,
		CHARACTER_MAXIMUM_LENGTH,
		CHARACTER_OCTET_LENGTH,
		NUMERIC_PRECISION,
		NUMERIC_SCALE,
		DATETIME_PRECISION,
		CHARACTER_SET_NAME,
		COLLATION_NAME,
		COLUMN_TYPE,
		COLUMN_KEY,
		EXTRA,
		PRIVILEGES,
		COLUMN_COMMENT
	};

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Prepare Query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// define column retrieval query to use
	char * column_retrieval_query = \
		" SELECT                     "\
		" 	TABLE_CATALOG,           "\
		" 	TABLE_SCHEMA,            "\
		" 	TABLE_NAME,              "\
		" 	COLUMN_NAME,             "\
		" 	ORDINAL_POSITION,        "\
		" 	COLUMN_DEFAULT,          "\
		" 	IS_NULLABLE,             "\
		" 	DATA_TYPE,               "\
		" 	CHARACTER_MAXIMUM_LENGTH,"\
		" 	CHARACTER_OCTET_LENGTH,  "\
		" 	NUMERIC_PRECISION,       "\
		" 	NUMERIC_SCALE,           "\
		" 	DATETIME_PRECISION,      "\
		" 	CHARACTER_SET_NAME,      "\
		" 	COLLATION_NAME,          "\
		" 	COLUMN_TYPE,             "\
		" 	COLUMN_KEY,              "\
		" 	EXTRA,                   "\
		" 	PRIVILEGES,              "\
		" 	COLUMN_COMMENT           "\
		" FROM information_schema.columns WHERE table_schema = ? AND table_name = ?";


	// attempt to prepare the query
	MYSQL_STMT * stmt = BP_MySQLPrepareQuery
	(
		mariadbi,
		column_retrieval_query
	);

	// ensure the statement was created ok
	if(!stmt)
	{
		printf("\n [!!] Could not prepare statement, exiting.");
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Bind Parameter(s) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create space for parameter
	MYSQL_BIND bind_params[3];

	// zero out the bind parameters
	BP_ZERO_STACK(bind_params);

	// you can set this to whatever you want so long it's an actual db
	char * db_to_fetch_tables_from = "grief3";

	// set table name
	unsigned long int db_table_name_len = bpstrlen(db_to_fetch_tables_from);


	// set parameter values
	bind_params[0].buffer_type     = MYSQL_TYPE_STRING;
	bind_params[0].buffer          = db_to_fetch_tables_from;
	bind_params[0].is_null         = 0;
	bind_params[0].buffer_length   = bpstrlen(db_to_fetch_tables_from);

	// set parameter values
	bind_params[1].buffer_type     = MYSQL_TYPE_STRING;
	bind_params[1].buffer          = table_name;
	bind_params[1].is_null         = 0;
	bind_params[1].buffer_length   = bpstrlen(table_name);


	// bind parameters
	if(BP_MySQLBindParameters(stmt, (MYSQL_BIND *) &bind_params, mariadbi) == ERR_FAILURE)
	{
		printf("\n [+] Could not bind parameters.");
		BP_MySQLCloseStatement(stmt, mariadbi);
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Execute Statement %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// attempt to execute the statement
	if(BP_MySQLExecuteStatement(stmt, mariadbi) == ERR_FAILURE)
	{
		printf("\n [+] Could not execute statement.");
		BP_MySQLCloseStatement(stmt, mariadbi);
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Prepare Results For Bind %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



	// this structure will hold the table data
	BPDB_MARIADB_TABLE_COLUMN_METADATA mariadb_column_metadata;
	BP_ZERO_STACK(mariadb_column_metadata);

	// results (must match enumerator item count + 1 null entry)
	MYSQL_BIND result[21];

	// zero out results structure
	BP_ZERO_STACK(result);


	// result index
	size_t result_idx = 0;


	// TABLE_CATALOG
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;

	// TABLE_SCHEMA
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;

	// TABLE_NAME
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;

	// COLUMN_NAME
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;

	// ORDINAL_POSITION
	result[result_idx].buffer_type   = MYSQL_TYPE_LONGLONG;
	result_idx++;

	// COLUMN_DEFAULT
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;

	// IS_NULLABLE
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;

	// DATA_TYPE
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;

	// CHARACTER_MAXIMUM_LENGTH
	result[result_idx].buffer_type   = MYSQL_TYPE_LONGLONG;
	result_idx++;

	// CHARACTER_OCTET_LENGTH
	result[result_idx].buffer_type   = MYSQL_TYPE_LONGLONG;
	result_idx++;

	// NUMERIC_PRECISION
	result[result_idx].buffer_type   = MYSQL_TYPE_LONGLONG;
	result_idx++;

	// NUMERIC_SCALE
	result[result_idx].buffer_type   = MYSQL_TYPE_LONGLONG;
	result_idx++;

	// DATETIME_PRECISION
	result[result_idx].buffer_type   = MYSQL_TYPE_LONGLONG;
	result_idx++;

	// CHARACTER_SET_NAME
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;

	// COLLATION_NAME
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;

	// COLUMN_TYPE
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;

	// COLUMN_KEY
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;

	// EXTRA
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;

	// PRIVILEGES
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;

	// COLUMN_COMMENT
	result[result_idx].buffer_type   = MYSQL_TYPE_STRING;
	result_idx++;



	// bind the result
	if(BP_MySQLBindResult(stmt, (MYSQL_BIND *) &result[0], result_idx, mariadbi) == ERR_FAILURE)
	{

		printf("\n [+] Could not bind result.");

		// return indicating failure
		BP_MySQLCloseStatement(stmt, mariadbi);
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);

	}

	// display bind message
	// printf("\n [+] Result bound to statement ok!");

	// attempt to store results (fetch from server)
	if(BP_MySQLStoreResults(stmt, mariadbi) == ERR_FAILURE)
	{

		printf("\n [+] Could not store result.");
		// return indicating failure
		BP_MySQLCloseStatement(stmt, mariadbi);
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Fetch Results from Stored Result Set %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set table path with some extra padding
	char *table_path[10];

	// zero the table
	BP_ZERO_STACK(table_path);

	// set column path
	table_path[0] = "tables";
	table_path[1] = table_name;
	table_path[2] = "columns";
	table_path[3] = NULL;

	// add columns "table" to the hreg
	BP_ERROR_T ret_val = BP_HashRegAddTableByPath(bpdb->system_hreg, (char **) &table_path);
	if(!ret_val)
	{
		BP_MySQLCloseStatement(stmt, mariadbi);
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);
	}


	// zero the table path after we add the "columns" entry
	BP_ZERO_STACK(table_path);

	// fetch results and display table
	while(BP_MySQLFetchResult(stmt, mariadbi) == ERR_SUCCESS)
	{


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Add Table  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// set initial path
		table_path[0] = "tables";
		table_path[1] = table_name;
		table_path[2] = "columns";
		table_path[3] = (char *) result[COLUMN_NAME].buffer; // (char *) &result[TABLE_NAME].buffer;
		table_path[4] = NULL;

		// add table to the db
		BP_ERROR_T ret_val = BP_HashRegAddTableByPath(bpdb->system_hreg, (char **) &table_path);
		if(!ret_val)
		{
			BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);
		}

		// zero the table
		BP_ZERO_STACK(table_path);


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Add Metadata %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


		// looks up a column given a table name and column name for lookup
		P_BP_HASH_TABLE_KEYED_ENTRY column_lookup = BP_DBLookupTableColumnByName(bpdb, table_name, (char *) result[COLUMN_NAME].buffer);
		if(!column_lookup)
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(bpdb->system_hreg, ERR_FAILURE);

		// attempt to add column metadata
		BP_HashRegAddDataToTable
		(
			bpdb->system_hreg,
			column_lookup,
			BP_SQL_TABLE_COLUMN_METADATA_RESERVED_TABLE_NAME,
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
			NULL,
			sizeof(BPDB_SQL_TABLE_COLUMN_METADATA)
		);

		// set lookup path
		char * lookup_path[] =
		{
			"tables",
			table_name,
			"columns",
			(char *) result[COLUMN_NAME].buffer,
			BP_SQL_TABLE_COLUMN_METADATA_RESERVED_TABLE_NAME,
			NULL
		};

		// retrive column metadata by path
		P_BP_HASH_TABLE_KEYED_ENTRY column_metadata_lookup = BP_HashRegLookupTableEntryByPath(bpdb->system_hreg, (char **) &lookup_path);
		if(!column_metadata_lookup)
		{
			BP_MySQLCloseStatement(stmt, mariadbi);
			BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);
		}

		// set column metadata from data element
		P_BPDB_SQL_TABLE_COLUMN_METADATA column_metadata = (P_BPDB_SQL_TABLE_COLUMN_METADATA) column_metadata_lookup->data;
		if(!column_metadata)
		{
			BP_MySQLCloseStatement(stmt, mariadbi);
			BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_FAILURE);
		}


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Fill in Metadata Structure %%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


		// === TABLE_CATALOG ==============

		// allocate space for table catalog
		column_metadata->mariadb_column_metadata.table_catalog = (char *) ht_calloc
		(
			bpdb->system_hreg,
			(size_t) result[TABLE_CATALOG].buffer_length + 1,
			1
		);

		// copy in data
		memcpy
		(
			(void *) column_metadata->mariadb_column_metadata.table_catalog,
			(void *) result[TABLE_CATALOG].buffer,
			(size_t) result[TABLE_CATALOG].buffer_length
		);

		// set length
		column_metadata->mariadb_column_metadata.table_catalog_data_len = result[TABLE_CATALOG].buffer_length;


		// === TABLE_SCHEMA ================

		// allocate space for table catalog
		column_metadata->mariadb_column_metadata.table_schema = (char *) ht_calloc
		(
			bpdb->system_hreg,
			(size_t) result[TABLE_SCHEMA].buffer_length + 1,
			1
		);

		// copy in data
		memcpy
		(
			(void *) column_metadata->mariadb_column_metadata.table_schema,
			(void *) result[TABLE_SCHEMA].buffer,
			(size_t) result[TABLE_SCHEMA].buffer_length
		);

		// set length
		column_metadata->mariadb_column_metadata.table_schema_data_len = result[TABLE_SCHEMA].buffer_length;


		// === TABLE_NAME ==================

		// allocate space for table catalog
		column_metadata->mariadb_column_metadata.table_name = (char *) ht_calloc
		(
					bpdb->system_hreg,
			(size_t) result[TABLE_NAME].buffer_length + 1,
			1
		);

		// copy in data
		memcpy
		(
			(void *) column_metadata->mariadb_column_metadata.table_name,
			(void *) result[TABLE_NAME].buffer,
			(size_t) result[TABLE_NAME].buffer_length
		);

		// set length
		column_metadata->mariadb_column_metadata.table_name_data_len = result[TABLE_NAME].buffer_length;


		// === COLUMN_NAME =================

		// allocate space for column name
		column_metadata->mariadb_column_metadata.column_name = (char *) ht_calloc
		(
			bpdb->system_hreg,
			(size_t) result[COLUMN_NAME].buffer_length + 1,
			1
		);

		// copy in data (don't allocate since this is a BIGINT col.
		memcpy
		(
			(void *) column_metadata->mariadb_column_metadata.column_name,
			(void *) result[COLUMN_NAME].buffer,
			(size_t) result[COLUMN_NAME].buffer_length
		);

		// set length
		column_metadata->mariadb_column_metadata.column_name_data_len = result[COLUMN_NAME].buffer_length;


		// === ORDINAL_POSITION ============

		// copy in oridinal position directly
		memcpy
		(
			(void *) &column_metadata->mariadb_column_metadata.ordinal_position,
			(void *) result[ORDINAL_POSITION].buffer,
			(size_t) result[ORDINAL_POSITION].buffer_length
		);


		// === COLUMN_DEFAULT ==============

		// allocate space for column_default
		column_metadata->mariadb_column_metadata.column_default = (char *) ht_calloc
		(
			bpdb->system_hreg,
			(size_t) result[COLUMN_DEFAULT].buffer_length + 1,
			1
		);

		// copy in data
		memcpy
		(
			(void *) column_metadata->mariadb_column_metadata.column_default,
			(void *) result[COLUMN_DEFAULT].buffer,
			(size_t) result[COLUMN_DEFAULT].buffer_length
		);

		// set length
		column_metadata->mariadb_column_metadata.column_default_data_len = result[COLUMN_DEFAULT].buffer_length;


		// === IS_NULLABLE =================


		// allocate space for is_nullable
		column_metadata->mariadb_column_metadata.is_nullable = (char *) ht_calloc
		(
			bpdb->system_hreg,
			(size_t) result[IS_NULLABLE].buffer_length + 1,
			1
		);

		// copy in data
		memcpy
		(
			(void *) column_metadata->mariadb_column_metadata.is_nullable,
			(void *) result[IS_NULLABLE].buffer,
			(size_t) result[IS_NULLABLE].buffer_length
		);

		// set length
		column_metadata->mariadb_column_metadata.is_nullable_data_len = result[IS_NULLABLE].buffer_length;


		// === DATA_TYPE ===================

		// allocate space for is_nullable
		column_metadata->mariadb_column_metadata.data_type = (char *) ht_calloc
		(
			bpdb->system_hreg,
			(size_t) result[DATA_TYPE].buffer_length + 1,
			1
		);

		memcpy
		(
			(void *) column_metadata->mariadb_column_metadata.data_type,
			(void *) result[DATA_TYPE].buffer,
			(size_t) result[DATA_TYPE].buffer_length
		);

		// set length
		column_metadata->mariadb_column_metadata.data_type_data_len = result[DATA_TYPE].buffer_length;

		// set datatype as enumerator (function only requires the use of the datatype member within the metadata structure
		// so it can be called here safely)
		column_metadata->mariadb_column_metadata.data_type_as_enum = BP_DBResolveMariadbColumnMetadataToType(&column_metadata->mariadb_column_metadata);

		// === CHARACTER_MAXIMUM_LENGTH ====
		memcpy
		(
			(void *) &column_metadata->mariadb_column_metadata.character_maximum_length,
			(void *) result[CHARACTER_MAXIMUM_LENGTH].buffer,
			(size_t) result[CHARACTER_MAXIMUM_LENGTH].buffer_length
		);

		// === CHARACTER_OCTET_LENGTH ======
		memcpy
		(
			(void *) &column_metadata->mariadb_column_metadata.character_octet_length,
			(void *) result[CHARACTER_OCTET_LENGTH].buffer,
			(size_t) result[CHARACTER_OCTET_LENGTH].buffer_length
		);

		// === NUMERIC_PRECISION ===========
		memcpy
		(
			(void *) &column_metadata->mariadb_column_metadata.numeric_precision,
			(void *) result[NUMERIC_PRECISION].buffer,
			(size_t) result[NUMERIC_PRECISION].buffer_length
		);

		// === NUMERIC_SCALE ===============
		memcpy
		(
			(void *) &column_metadata->mariadb_column_metadata.numeric_scale,
			(void *) result[NUMERIC_SCALE].buffer,
			(size_t) result[NUMERIC_SCALE].buffer_length
		);

		// === DATETIME_PRECISION ==========
		memcpy
		(
			(void *) &column_metadata->mariadb_column_metadata.datetime_precision,
			(void *) result[DATETIME_PRECISION].buffer,
			(size_t) result[DATETIME_PRECISION].buffer_length
		);


		// === CHARACTER_SET_NAME ==========

		// allocate space for is_nullable
		column_metadata->mariadb_column_metadata.character_set_name = (char *) ht_calloc
		(
			bpdb->system_hreg,
			(size_t) result[CHARACTER_SET_NAME].buffer_length + 1,
			1
		);

		// copy in data
		memcpy
		(
			(void *) column_metadata->mariadb_column_metadata.character_set_name,
			(void *) result[CHARACTER_SET_NAME].buffer,
			(size_t) result[CHARACTER_SET_NAME].buffer_length
		);

		// set length
		column_metadata->mariadb_column_metadata.character_set_name_data_len = result[CHARACTER_SET_NAME].buffer_length;


		// === COLLATION_NAME ==============

		// allocate space for is_nullable
		column_metadata->mariadb_column_metadata.collation_name = (char *) ht_calloc
		(
			bpdb->system_hreg,
			(size_t) result[COLLATION_NAME].buffer_length + 1,
			1
		);

		// copy in data
		memcpy
		(
			(void *) column_metadata->mariadb_column_metadata.collation_name,
			(void *) result[COLLATION_NAME].buffer,
			(size_t) result[COLLATION_NAME].buffer_length
		);

		// set length
		column_metadata->mariadb_column_metadata.collation_name_data_len = result[COLLATION_NAME].buffer_length;


		// === COLUMN_TYPE =================

		// allocate space for column_type
		column_metadata->mariadb_column_metadata.column_type = (char *) ht_calloc
		(
			bpdb->system_hreg,
			(size_t) result[COLUMN_TYPE].buffer_length + 1,
			1
		);

		// copy in data
		memcpy
		(
			(void *) column_metadata->mariadb_column_metadata.column_type,
			(void *) result[COLUMN_TYPE].buffer,
			(size_t) result[COLUMN_TYPE].buffer_length
		);

		// set length
		column_metadata->mariadb_column_metadata.column_type_data_len = result[COLUMN_TYPE].buffer_length;


		// === COLUMN_KEY ==================


		// allocate space for column_type
		column_metadata->mariadb_column_metadata.column_key = (char *) ht_calloc
		(
			bpdb->system_hreg,
			(size_t) result[COLUMN_KEY].buffer_length + 1,
			1
		);

		memcpy
		(
			(void *) column_metadata->mariadb_column_metadata.column_key,
			(void *) result[COLUMN_KEY].buffer,
			(size_t) result[COLUMN_KEY].buffer_length
		);

		// set length
		column_metadata->mariadb_column_metadata.column_key_data_len = result[COLUMN_KEY].buffer_length;


		// === EXTRA =======================

		// allocate space for extra,
		column_metadata->mariadb_column_metadata.extra = (char *) ht_calloc
		(
			bpdb->system_hreg,
			(size_t) result[EXTRA].buffer_length + 1,
			1
		);

		// copy in data
		memcpy
		(
			(void *) column_metadata->mariadb_column_metadata.extra,
			(void *) result[EXTRA].buffer,
			(size_t) result[EXTRA].buffer_length
		);

		// set length
		column_metadata->mariadb_column_metadata.extra_data_len = result[EXTRA].buffer_length;


		// === PRIVILEGES ==================

		// allocate space for extra,
		column_metadata->mariadb_column_metadata.privileges = (char *) ht_calloc
		(
			bpdb->system_hreg,
			(size_t) result[PRIVILEGES].buffer_length + 1,
			1
		);

		// copy in data
		memcpy
		(
			(void *) column_metadata->mariadb_column_metadata.privileges,
			(void *) result[PRIVILEGES].buffer,
			(size_t) result[PRIVILEGES].buffer_length
		);

		// set length
		column_metadata->mariadb_column_metadata.privileges_data_len = result[PRIVILEGES].buffer_length;


		// === COLUMN_COMMENT ==============

		// allocate space for column_comment,
		column_metadata->mariadb_column_metadata.column_comment = (char *) ht_calloc
		(
			bpdb->system_hreg,
			(size_t) result[COLUMN_COMMENT].buffer_length + 1,
			1
		);

		// copy in data
		memcpy
		(
			(void *) column_metadata->mariadb_column_metadata.column_comment,
			(void *) result[COLUMN_COMMENT].buffer,
			(size_t) result[COLUMN_COMMENT].buffer_length
		);

		// set length
		column_metadata->mariadb_column_metadata.column_comment_data_len = result[COLUMN_COMMENT].buffer_length;

	}

	// close the statement
	BP_MySQLCloseStatement(stmt, mariadbi);

	// return indicating success
	BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, ERR_SUCCESS);

}

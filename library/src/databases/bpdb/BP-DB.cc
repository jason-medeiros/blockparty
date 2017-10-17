/*
 * BP-DB.cc
 *
 *  Created on: Aug 8, 2015
 *      Author: root
 */


// include BP main
#include "../../../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%  Open or Close Syntax Generation Session %%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Attempt to open a BPDB handle using standard creds.
P_BPDB BP_DBOpen
(
		char * mariadb_host,
		char * mariadb_database,
		size_t mariadb_port,
		char * mariadb_user,
		char * mariadb_pass,
		char * sqlite_db_file,
		char * sqlite_log_file,
		size_t mariadb_connection_pool_count
)
{

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Hash Table Registry/BPDB Struct First %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Note: This routine will always try to return a P_BPDB structure
	//       even if init fails.  The reason for this is that we use
	//       the error reporting facilities of BPDB in order to report BPDB
	//       failures.  Ensure that you check the bpdb->init_ok flag before
	//       attempting to use any newly created BPDB.

	// create system registry
	P_BP_HASH_TABLE_REGISTRY system_hreg = BP_CreateHashTableRegistry();
	if(!system_hreg)
		return NULL;

	// allocate bpdb from system hreg
	P_BPDB bpdb = (P_BPDB) ht_calloc(system_hreg, sizeof(BPDB), 1);
	if(!bpdb)
		return NULL;


	// -- after allocation is confirmed, setup bpdb init state ---


	// set the sytem hreg recursively (easy cleanup/managment)
	bpdb->system_hreg = system_hreg;

	// allocate space for the uuid
	bpdb->uuid = (char *) ht_calloc(bpdb->system_hreg, BP_UUID_LEN+1, 1);

	// generate the uuid
	BP_GenRandomUUID_StoreInBuff(bpdb->uuid);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Default Table Entries %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create table path and zero out
	char * table_path[4];
	BP_ZERO_STACK(table_path);


	// add table to the db
	BP_ERROR_T ret_val = BP_HashRegAddTableByPathQuery(system_hreg, "/mariadb_connections");
	if(!ret_val)
	{

		// return null since this is a critical err
		BP_DBClose(bpdb);
		return NULL;

	}

	// add table to the db
	ret_val = BP_HashRegAddTableByPathQuery(bpdb->system_hreg, "/tables");
	if(!ret_val)
	{

		// return null since this is a critical err
		BP_DBClose(bpdb);
		return NULL;
	}

	// add table here
	ret_val = BP_HashRegAddTableByPathQuery(bpdb->system_hreg, "/pending_queries");
	if(!ret_val)
	{

		// return null since this is a critical err
		BP_DBClose(bpdb);
		return NULL;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Parameter Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// --- mariadb checks -------------

	// ensure we have a host
	if(!mariadb_host)
	{
		BP_DBAddErrorMsg(bpdb, "BPDB parameter mariadb_host was NULL.  This value must be a reasonable printable string.");
		return bpdb;
	}
	else if(!BP_StringIsReasonableForMariaDBHostNameDB(mariadb_host, bpstrlen(mariadb_host)))
	{
		BP_DBAddErrorMsg(bpdb, "BPDB parameter mariadb_host didn't pass filter checks.  Ensure your mariadb_host is a valid hostname.");
		return bpdb;
	}

	// ensure we have a port
	if(!mariadb_port)
	{
		BP_DBAddErrorMsg(bpdb, "BPDB parameter mariadb_port was set to zero.  We do not consider this a valid port.");
		return bpdb;
	}
	else if(mariadb_port >= 65534)
	{
		BP_DBAddErrorMsg(bpdb, "BPDB mariadb_port is set to > 65534.  This is disallowed.");
		return bpdb;
	}

	// ensure we have a user
	if(!mariadb_user)
	{
		BP_DBAddErrorMsg(bpdb, "BPDB parameter mariadb_user was null.  This value must be set.");
		return bpdb;
	}
	else if(!BP_StringIsReasonableForUsernameMariaDB(mariadb_user, bpstrlen(mariadb_user)))
	{
		BP_DBAddErrorMsg(bpdb, "BPDB parameter mariadb_user did not pass filter tests.  Ensure your username is normal, reasonable, ascii.");
		return bpdb;
	}

	// ensure we have a password
	if(!mariadb_pass)
	{
		BP_DBAddErrorMsg(bpdb, "BPDB parameter mariadb_pass was null.  This value must be set.  If you want an empty password use an empty string (but seriously, use a password).");
		return bpdb;
	}
	else if(!BP_StringIsReasonableForPasswordMariaDB(mariadb_pass, bpstrlen(mariadb_pass)))
	{
		BP_DBAddErrorMsg(bpdb, "BPDB parameter mariadb_pass did not pass filter tests.  Ensure your password is normal, reasonable, ascii.");
		return bpdb;
	}

	// ensure we have a mariadb pool count
	if(!mariadb_connection_pool_count)
	{
		BP_DBAddErrorMsg(bpdb, "BPDB parameter mariadb_connection_pool_count is zero.  This setting must be a non-zero value.");
		return bpdb;
	}
	else if(mariadb_connection_pool_count >= 8)
	{
		// note: do not return here since this is only a warning
		BP_DBAddWarningMsg(bpdb, "BPDB parameter mariadb_connection_pool_count is >= 8.  Setting this many connections on one database can be dangerously high.");
	}

	// --- sqlite checks ------------

	// ensure we have a database file
	if(!sqlite_db_file)
	{
		BP_DBAddErrorMsg(bpdb, "BPDB parameter sqlite_db_file is NULL.  The sqlite db is used for caching and must be set.");
		return bpdb;
	}
	else if(!BP_StringIsReasonableForDatabaseFileSQLiteDB(sqlite_db_file, bpstrlen(sqlite_db_file), BP_FALSE))
	{
		BP_DBAddErrorMsg(bpdb, "BPDB parameter sqlite_db_file did not pass filter checks.  Ensure the paramter points to a database file.");
		return bpdb;
	}

	// ensure we have a log file
	if(!sqlite_log_file)
	{
		BP_DBAddErrorMsg(bpdb, "BPDB parameter sqlite_log_file is NULL.  The sqlite_log_file holds error/notice/warning log information and must be set.");
		return bpdb;
	}
	else if(!BP_StringIsReasonableForDatabaseFileSQLiteDB(sqlite_log_file, bpstrlen(sqlite_log_file), BP_FALSE))
	{
		BP_DBAddErrorMsg(bpdb, "BPDB parameter sqlite_log_file did not pass filter checks.  Ensure the paramter points to a database file.");
		return bpdb;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Initialize Connection Pool %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add entries in hash table
	size_t connection_pool_iter = 0;

	// try to establish mariadb pool
	for
	(
		;
		connection_pool_iter < mariadb_connection_pool_count;
		connection_pool_iter++
	)
	{

		// create new mariadb session
		P_BP_MYSQL_SESSION mariadbi = BP_MySQLConnect(mariadb_host, mariadb_user, mariadb_pass, mariadb_database, mariadb_port);
		if(!mariadbi)
		{

			// create temporary error buffer
			char tmp_err[256];
			BP_ZERO_STACK(tmp_err);

			// create error
			sprintf((char *)&tmp_err, "While creating connection pools a failure occured on connection number %zu (out of %zu).  Connect attempt failed.", connection_pool_iter, mariadb_connection_pool_count);

			// add error
			BP_DBAddErrorMsg(bpdb, (char *)&tmp_err);
			return bpdb;

		}

		// adds a mariadb connection to the pool of available connections
		if(!BP_DBAddMariadbConnectionToPool(bpdb, mariadbi))
		{
			// create temporary error buffer
			char tmp_err[512];
			BP_ZERO_STACK(tmp_err);

			// create error
			sprintf((char *)&tmp_err, "While creating connection pools a failure occured on connection number %zu (out of %zu).  Failed to add created connection to our pool. (potentially out of memory?)", connection_pool_iter, mariadb_connection_pool_count);

			// add error
			BP_DBAddErrorMsg(bpdb, (char *)&tmp_err);
			return bpdb;
		}

	}

	// dbg: display critical section depth
	//printf("\n [+] BPDB Critical section depth after adding connection pools: %u", bpdb->system_hreg->critical_section_lock_depth);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Open Handles %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new sqlite session
	P_BP_DB_SQLITE sqli = BP_SQLiteOpen(sqlite_db_file, sqlite_log_file);
	if(!sqli)
	{

		// create temporary error buffer
		char tmp_err[bpstrlen(sqlite_db_file)+256];
		BP_ZERO_STACK(tmp_err);

		// create error
		sprintf((char *)&tmp_err, "Could not open our sqlite_db: %s", sqlite_db_file);

		// add error
		BP_DBAddErrorMsg(bpdb, (char *)&tmp_err);
		return bpdb;

	}

	// set sqlite handle
	bpdb->sqlite  = sqli;

	// dbg: display critical section depth after sqlite handler
	printf("\n [+] BPDB Critical section depth after opening sqlite handle: %u", bpdb->system_hreg->critical_section_lock_depth);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Autoload MySQL Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to autoload all tables from the db
	if(!BP_DBMySQLAutoLoadAllTables(bpdb))
	{
		BP_DBAddErrorMsg(bpdb, "BPDB setup failed.  Specifically, the BP_DBMySQLAutoLoadAllTables() routine failed.  This means that table metadata has failed to load for this connection.  This makes intelligent syntax generation impossible.  Ensure the user for this database connection can view schema elements for the tables/databases being used.");
		return bpdb;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Mark Init Ok and Return New BPDB %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// display critical section depth after autoloading all
	printf("\n [+] BPDB Critical section depth after autoloading all tables: %u", bpdb->system_hreg->critical_section_lock_depth);

	// mark the bpdb as having initialized ok
	bpdb->init_ok = BP_TRUE;

	// return the new database handle
	return bpdb;

}



// Attempt to close the bpdb handle.
BP_ERROR_T BP_DBClose(P_BPDB bpdb)
{

	// ensure we have a bpdb to destroy
	if(!bpdb)
		return ERR_FAILURE;

	// close our sqlite database handle
	if(!BP_SQLiteClose(bpdb->sqlite))
	{
		BP_DBAddErrorMsg(bpdb, "Attempted to call BP_SQLiteClose within BP_DBClose and failed.  SQLite handle went bad somehow.");
		return ERR_FAILURE;
	}

	// ensure mariadb connection pool closed
	if(!BP_DBCloseMariaDBConnectionPool(bpdb))
	{
		BP_DBAddErrorMsg(bpdb, "Attempted to call BP_DBCloseMariaDBConnectionPool within BP_DBClose and failed.  BP_DBCloseMariaDBConnectionPool somehow went bad (likely dropped connections/timeouts).");
		return ERR_FAILURE;
	}


	// --- Only destroy registries after closures are ok -------------

	// destroy messages registry if it exists
	BP_DestroyHashTableRegistry(bpdb->messages_reg);

	// destroy the system hreg (destroys the initial struct as well as
	// all errata)
	BP_DestroyHashTableRegistry(bpdb->system_hreg);


	// return indicating success
	return ERR_SUCCESS;

}


// --------- Close All Connections ------------------------------

// Attempts to close all connections in the mariadb connection pool.
BP_ERROR_T BP_DBCloseMariaDBConnectionPool(P_BPDB bpdb)
{

	// ensure we have a bpdb connection
	if(!bpdb)
		return ERR_FAILURE;

	// attempt to lookup the connections table via path query
	P_BP_HTKE mariadb_conn_table = BP_HashRegLookupTableByPathQuery(bpdb->system_hreg, "/mariadb_connections");
	if(!mariadb_conn_table)
	{
		BP_DBAddErrorMsg(bpdb, "Failed to lookup the mariadb_connections table within the system_hreg.");
		return ERR_FAILURE;
	}

	// close/fail indicator
	BP_BOOL one_or_more_connections_failed_to_close = BP_FALSE;

	// iterate through connections in the table and close them one by one
	BP_HASH_ITER_OPEN((P_BP_HTKE) mariadb_conn_table->data, mariadb_conn_iter_entry)
	{

		// set pointer from local data
		P_BP_MYSQL_SESSION * tmp_session_ptr = (P_BP_MYSQL_SESSION *) mariadb_conn_iter_entry->data;

		// ensure we have a session pointer
		if(!tmp_session_ptr)
			continue;
		if(!tmp_session_ptr[0])
			continue;


		// attempt to close the connection
		if(!BP_MySQLClose(tmp_session_ptr[0]))
		{
			BP_DBAddErrorMsg(bpdb, "Connection failed to close with BP_MySQLClose() within BP_DBCloseMariaDBConnectionPool().  This should not occur under normal circumstances.");
			one_or_more_connections_failed_to_close = BP_TRUE;
		}

	}
	BP_HASH_ITER_CLOSE;

	// if one or more failed to close, return indicating failure
	if(one_or_more_connections_failed_to_close)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Clear Memory Used in Previous Queries %%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Destroy dangling memory from previous queries
BP_ERROR_T BP_DBDestroyMemoryFromAnyPreviousQueries(P_BPDB bpdb)
{

	// ensure we have a bpdb ptr
	if(!bpdb)
		return ERR_FAILURE;

	// destroy all registered sql syntax generators
	BP_DBDestroyAllRegisteredSQLSyntaxGenerators(bpdb);

	// return indicating success
	return ERR_SUCCESS;

}


// destroy all registered sql syntax generators
BP_ERROR_T BP_DBDestroyAllRegisteredSQLSyntaxGenerators(P_BPDB bpdb)
{

	// ensure that we have a bpdb structure to work with
	if(!bpdb)
		return ERR_FAILURE;
	if(!bpdb->sqlgen_reg)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Iterate through SQL Generators And Destroy %%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// display the hash table registry
	BP_HashRegDisplay(bpdb->sqlgen_reg, BP_FALSE, BP_TRUE);

	// iterate through hash entries
	if(bpdb->sqlgen_reg->hive)
	{

		// iterate through table
		BP_HASH_ITER_OPEN(bpdb->sqlgen_reg->hive, sqlgen_table_entry)
		{

			// iterate through entries
			BP_HASH_ITER_OPEN((P_BP_HTKE) sqlgen_table_entry->data, sqlgen_entry)
			{

				// set current sql generator
				P_BPDB_SQLGEN curr_sqlgen = (P_BPDB_SQLGEN) sqlgen_entry->data;

				// close the current sql generator
				BP_DBSGClose(curr_sqlgen);

			}
			BP_HASH_ITER_CLOSE;

		}
		BP_HASH_ITER_CLOSE;
	}


	// destroy the sqlgen hash table registry
	BP_DestroyHashTableRegistry(bpdb->sqlgen_reg);

	// nullify the current registry
	bpdb->sqlgen_reg = NULL;

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Add Connection to Pool %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// adds a mariadb connection to the pool of available connections
BP_ERROR_T BP_DBAddMariadbConnectionToPool(P_BPDB bpdb, P_BP_MYSQL_SESSION mariadbi)
{

	// return indicating failure
	if(!bpdb)
		return ERR_FAILURE;
	if(!bpdb->system_hreg)
		return ERR_FAILURE;

	// enter critical section
	BPDB_ENTER_CRITICAL_SECTION;

	// lookup connection by uuid
	if(BP_DBLookupMariadbConnectionByUUID(bpdb, mariadbi->uuid))
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBLookupMariadbConnectionByUUID failed while attempting BP_DBAddMariadbConnectionToPool.");
		BPDB_LEAVE_CRITICAL_SECTION_AND_RETURN(ERR_FAILURE);
	}

	// display addition message
	printf("\n [+] Adding maraidb connection to connection pool: %s", mariadbi->uuid);

	// add space for entry
	if(
		!BP_HashRegAddDataToTableByPathQuery
		(
				bpdb->system_hreg,
				"/mariadb_connections",
				mariadbi->uuid,
				BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
				NULL,
				sizeof(BP_MYSQL_SESSION *)
		)
	)
	{

		BP_DBAddErrorMsg(bpdb, "BP_HashRegAddDataToTableByPathQuery failed while attempting BP_DBAddMariadbConnectionToPool.");
		BPDB_LEAVE_CRITICAL_SECTION_AND_RETURN(ERR_FAILURE);

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Lookup Entry After Creation %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set connection lookup path
	char * connection_lookup_path[] =
	{
		"mariadb_connections",
		mariadbi->uuid,
		NULL
	};

	// attempt to lookup the connection entry
	P_BP_HASH_TABLE_KEYED_ENTRY connection_entry = BP_HashRegLookupTableEntryByPath(bpdb->system_hreg, (char **) &connection_lookup_path);
	if(!connection_entry)
	{
		BP_DBAddErrorMsg(bpdb, "BP_HashRegLookupTableEntryByPath failed while attempting BP_DBAddMariadbConnectionToPool.");
		BPDB_LEAVE_CRITICAL_SECTION_AND_RETURN(ERR_FAILURE);
	}

	// copy in the new connection entry addr (POINTER ONLY)
	memcpy(connection_entry->data, &mariadbi, sizeof(BP_MYSQL_SESSION *));

	// lookup mysqli session
	P_BP_MYSQL_SESSION mysqli_session = BP_DBLookupMariadbConnectionByUUID(bpdb, mariadbi->uuid);

	// mark the session as pool available after addition
	mysqli_session->pool_available = BP_TRUE;

	// return indicating success
	BPDB_LEAVE_CRITICAL_SECTION_AND_RETURN(ERR_SUCCESS);

}

// attempts to search the mariadb connection pool and retrieve a connection by uuid
P_BP_MYSQL_SESSION BP_DBLookupMariadbConnectionByUUID(P_BPDB bpdb, char * uuid)
{

	// run basic checks
	if(!bpdb)
		return NULL;

	if(!uuid)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBLookupMariadbConnectionByUUID attempted lookup with null uuid parameter");
		return NULL;
	}

	if(!BP_StringIsReasonablePrintableString(uuid, bpstrlen(uuid), BP_FALSE, BP_FALSE))
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBLookupMariadbConnectionByUUID attempted lookup uuid which was an unreasonable string (contained bad non-ascii chars).");
		return NULL;
	}
	if(!BP_StringIsUUID(uuid))
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBLookupMariadbConnectionByUUID attempted lookup with a uuid which failed our BP_StringIsUUID filter check");
		return NULL;
	}


	// enter critical section
	BPDB_ENTER_CRITICAL_SECTION;

	// set connection lookup path
	char * connection_lookup_path[] =
	{
			"mariadb_connections",
			uuid,
			NULL
	};

	// attempt to lookup the connection entry
	P_BP_HASH_TABLE_KEYED_ENTRY connection_entry = BP_HashRegLookupTableEntryByPath(bpdb->system_hreg, (char **) &connection_lookup_path);
	if(!connection_entry)
	{
		// note: don't add error here, as this is a lookup routine and null lookups are valid
		BPDB_LEAVE_CRITICAL_SECTION_AND_RETURN(NULL);
	}

	// set session lookup
	P_BP_MYSQL_SESSION * session_lookup = (P_BP_MYSQL_SESSION *) connection_entry->data;

	// exit if there is no session to deref into
	if(!*session_lookup)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBLookupMariadbConnectionByUUID entry looked up, but mariadb session pointer was NULL after lookup.  This means our connection pool is corrupted.");
		BPDB_LEAVE_CRITICAL_SECTION_AND_RETURN(NULL);
	}

	// create mysqli reference
	P_BP_MYSQL_SESSION mysqli = *session_lookup;

	// return the reference
	BPDB_LEAVE_CRITICAL_SECTION_AND_RETURN(mysqli);

}

// Attempts to checkout/fetch a mariadb connection which is available and ready for
// interaction with a bpdb handle.
P_BP_MYSQL_SESSION BP_DBCheckOutAvailableMariadbConnection(P_BPDB bpdb)
{

	// ensure we have a bpdb structure
	if(!bpdb)
		return NULL;
	if(!bpdb->system_hreg)
		return NULL;

	// enter critical section
	BPDB_ENTER_CRITICAL_SECTION;

	// set lookup path to the mariadb connections table
	char * lookup_path[] = { "mariadb_connections", NULL };

	// open an iterator to walk all connections
	BP_HASH_ITER_FROM_LOOKUP_PATH_OPEN(bpdb->system_hreg, (char **) &lookup_path, connection_iter)
	{

		// set session lookup
		P_BP_MYSQL_SESSION * session_lookup = (P_BP_MYSQL_SESSION *) connection_iter->data;

		// exit if there is no session to deref into
		if(!*session_lookup)
		{
			// leave critical section and return
			BP_DBAddErrorMsg(bpdb, "BP_DBCheckOutAvailableMariadbConnection found NULL session lookup during search for available connection.  Potentially corrupt pool detected.");
			BPDB_LEAVE_CRITICAL_SECTION_AND_RETURN(NULL);
		}

		// create mysqli reference
		P_BP_MYSQL_SESSION mysqli = *session_lookup;

		// if the pool entry is available, return the mysqli
		if(mysqli->pool_available)
		{
			mysqli->pool_available = BP_FALSE;
			BPDB_LEAVE_CRITICAL_SECTION_AND_RETURN(mysqli);
		}
		else
		{
			// only uncomment if you need to debug the loop
			// BP_DBAddErrorMsg(bpdb, "BP_DBCheckOutAvailableMariadbConnection reports that pool is not available?");
		}

	} BP_HASH_ITER_FROM_LOOKUP_PATH_CLOSE;

	// return null if we can't find a connection which hasn't been checked out
	BPDB_LEAVE_CRITICAL_SECTION_AND_RETURN(NULL);

}

// Attempts to checkin a mariadb connection after you're done with use.  All data
// from previous queries are cleared after checkin of a connection.
BP_ERROR_T BP_DBCheckInMariadbConnection(P_BPDB bpdb, P_BP_MYSQL_SESSION mariadbi)
{

	// ensure we have a bpdb structure
	if(!bpdb)
		return ERR_FAILURE;
	if(!bpdb->system_hreg)
		return ERR_FAILURE;

	// enter critical section
	BPDB_ENTER_CRITICAL_SECTION;

	// lookup the connection in the table
	P_BP_MYSQL_SESSION lookup_connection = BP_DBLookupMariadbConnectionByUUID(bpdb, mariadbi->uuid);
	if(!lookup_connection)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBCheckInMariadbConnection tried to lookup connection for checkin via BP_DBLookupMariadbConnectionByUUID, but failed.");
		return ERR_FAILURE;
	}

	// make the pool entry available
	if(!lookup_connection->pool_available)
	{
		mariadbi->pool_available = BP_TRUE;
		BPDB_LEAVE_CRITICAL_SECTION_AND_RETURN(ERR_SUCCESS);
	}

	// return null if we can't find a connection which hasn't been checked out
	BPDB_LEAVE_CRITICAL_SECTION_AND_RETURN(ERR_FAILURE);

}

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
)
{

	// ensure we have a handle and a table name
	if(!bpdb)
		return ERR_FAILURE;
	if(!table_name)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBTableAdd tried to add a null table.");
		return ERR_FAILURE;
	}

	// ensure table name is reasonable
	if(!BP_StringIsReasonableForTableNameDB(table_name, bpstrlen(table_name)))
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBTableAdd tried to add a table_name which did not pass filter tests.");
		return ERR_FAILURE;
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Add Table %%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// lock critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(bpdb->system_hreg);

	// set table path
	char *table_path[2];

	// zero the table
	BP_ZERO_STACK(table_path);

	// set initial path
	table_path[0] = table_name;

	// add table to the db
	BP_ERROR_T ret_val = BP_HashRegAddTableByPath(bpdb->system_hreg, (char **) &table_path);
	if(!ret_val)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBTableAdd failed to add table to system_hreg.");
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(bpdb->system_hreg, ret_val);
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Table Metadata %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add metadata entry for table here now that the column has been added
	P_BP_HASH_TABLE_KEYED_ENTRY table_lookup = BP_DBLookupTableByName(bpdb, table_name);
	if(!table_lookup)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBTableAdd failed to lookup table after addition.");
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(bpdb->system_hreg, ERR_FAILURE);
	}

	// attempt to add metadata entry to the table
	BP_HashRegAddDataToTable
	(
		bpdb->system_hreg,
		table_lookup,
		BP_SQL_TABLE_METADATA_RESERVED_TABLE_NAME, /* in BP-DB.h */
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BPDB_SQL_TABLE_METADATA)
	);

	// set lookup path
	char * lookup_path[] = {table_name, BP_SQL_TABLE_METADATA_RESERVED_TABLE_NAME, NULL};

	// retrive table metadata by path
	P_BP_HASH_TABLE_KEYED_ENTRY table_metadata_lookup = BP_HashRegLookupTableEntryByPath(bpdb->system_hreg, (char **) &lookup_path);
	if(!table_metadata_lookup)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBTableAdd table_metadata_lookup failed.  Table addition likely failed.");
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(bpdb->system_hreg, ERR_FAILURE);
	}

	// set table metadata
	P_BPDB_SQL_TABLE_METADATA table_metadata = (P_BPDB_SQL_TABLE_METADATA) table_metadata_lookup->data;
	if(!table_metadata)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBTableAdd table_metadata_lookup DATA failed.");
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(bpdb->system_hreg, ERR_FAILURE);
	}

	/*
	table_metadata->exists_in_localcache_db;
	table_metadata->exists_in_persistent_db;
	table_metadata->table_name;
	table_metadata->total_record_count_at_last_update;
	table_metadata->total_records_selected;
	*/



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Columns %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add columns
	ret_val = BP_DBColumnsAdd(bpdb, table_name, columns, column_n);
	if(!ret_val)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBTableAdd column(s) addition failed.");
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(bpdb->system_hreg, ret_val);
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Generate SQLite CREATE TABLE Syntax %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Generate MySQL CREATE TABLE Syntax %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Leave Critical Section and Exit %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(bpdb->system_hreg);

	// return indicating success
	return ret_val;

}

// add a column.
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
)
{

	// run basic null checks
	if(!bpdb)
		return ERR_FAILURE;


	if(!table_name)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBColumnAdd table_name parameter was NULL");
		return ERR_FAILURE;
	}
	if(!column_name)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBColumnAdd column_name parameter was NULL");
		return ERR_FAILURE;
	}
	if(!column_type)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBColumnAdd column_type parameter was NULL");
		return ERR_FAILURE;
	}

	// ensure table name is reasonable
	if(!BP_StringIsReasonableForTableNameDB(table_name, bpstrlen(table_name)))
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBColumnAdd table_name did not pass filter tests.");
		return ERR_FAILURE;
	}

	// ensure column name is reasonable
	if(!BP_StringIsReasonableForColumnNameDB(column_name, bpstrlen(column_name)))
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBColumnAdd column_name did not pass filter tests.");
		return ERR_FAILURE;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Lookup Table %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// system hash registry
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(bpdb->system_hreg);

	// set lookup path
	char * lookup_path[10] = {0};

	// set the table path
	lookup_path[0] = table_name;

	// lookup the table
	P_BP_HASH_TABLE_KEYED_ENTRY table_entry = BP_HashRegLookupTableByPath
	(
		bpdb->system_hreg,
		(char **) &lookup_path
	);

	// ensure we have a table entry
	if(!table_entry)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBColumnAdd table_entry failed lookup.");
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(bpdb->system_hreg, ERR_FAILURE);
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Column %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set add path
	char * add_path[10] = {0};

	// set addition path
	add_path[0] = table_name;
	add_path[1] = column_name;
	add_path[2] = NULL;

	// add table by path
	BP_ERROR_T ret_val = BP_HashRegAddTableByPath(bpdb->system_hreg, (char **) &add_path);
	if(!ret_val)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBColumnAdd column_name addition failed when attempting BP_HashRegAddTableByPath.");
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(bpdb->system_hreg, ERR_FAILURE);
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Column Metadata %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add metadata
	BP_HashRegAddDataToTableByPath
	(
			bpdb->system_hreg,
			(char **) &add_path,
			"metadata",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
			NULL,
			sizeof(BPDB_SQL_TABLE_COLUMN_METADATA)
	);

	// zero out the lookup path
	BP_ZERO_STACK(lookup_path);

	// set lookup path
	lookup_path[0] = table_name;
	lookup_path[1] = column_name;
	lookup_path[2] = "metadata";
	lookup_path[3] = NULL;

	// set data entry
	P_BP_HASH_TABLE_KEYED_ENTRY data_entry = BP_HashRegLookupTableEntryByPath(bpdb->system_hreg, (char **) &lookup_path);
	if(!data_entry)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBColumnAdd data_entry addition failed when attempting BP_HashRegAddTableByPath.");
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(bpdb->system_hreg, ERR_FAILURE);
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Column Metadata Entries %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set metadata
	P_BPDB_SQL_TABLE_COLUMN_METADATA metadata = (P_BPDB_SQL_TABLE_COLUMN_METADATA) data_entry->data;

	// set column name
	metadata->bpdb_column_name = ht_strdup(bpdb->system_hreg, column_name);

	// set column type
	metadata->type = column_type;

	// set primary key
	metadata->primary_key = primary_key;

	// set indexed
	metadata->indexed = indexed;

	// set unique
	metadata->unique = unique;

	// set auto_inc
	metadata->auto_inc = auto_inc;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Column Data Table %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// zero out the lookup path
	BP_ZERO_STACK(add_path);

	// set lookup path
	add_path[0] = table_name;
	add_path[1] = column_name;
	add_path[2] = "data";
	add_path[3] = NULL;

	// add table by path
	ret_val = BP_HashRegAddTableByPath(bpdb->system_hreg, (char **) &add_path);
	if(!ret_val)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBColumnAdd failed to add data via BP_HashRegAddTableByPath");
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(bpdb->system_hreg, ERR_FAILURE);
	}

	// check to ensure data was added ok
	table_entry = BP_HashRegLookupTableByPath(bpdb->system_hreg, (char **) &add_path);
	if(!table_entry)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBColumnAdd failed to lookup data via BP_HashRegAddTableByPath");
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(bpdb->system_hreg, ERR_FAILURE);
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Exit Critical Section and Return %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(bpdb->system_hreg);


	// return indicating success
	return ERR_SUCCESS;

}

// Add multiple columns to a table at once.  Columns are copied using BP_DBColumnAdd iteratively.
BP_ERROR_T BP_DBColumnsAdd
(
		P_BPDB                           bpdb,
		char *                           table_name,
		P_BPDB_SQL_TABLE_COLUMN_METADATA columns,
		size_t                           column_n
)
{

	// basic null checks
	if(!bpdb)
		return ERR_FAILURE;
	if(!bpdb->system_hreg)
		return ERR_FAILURE;

	if(!table_name)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBColumnsAdd was called with a NULL table_name parameter.");
		return ERR_FAILURE;
	}

	if(!columns)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBColumnsAdd was called with a NULL columns parameter.");
		return ERR_FAILURE;
	}

	if(!column_n)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBColumnsAdd was called with a zero column count.");
		return ERR_FAILURE;
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Add Cols %%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(bpdb->system_hreg);

	// iterate through columns and check sanity
	size_t n = 0;
	for(; n < column_n; n++)
	{

		// if any strings fail check, exit immediately
		if(!BP_StringIsReasonableForColumnNameDB(columns[n].bpdb_column_name, bpstrlen(columns[n].bpdb_column_name)))
		{

			// create space for a temporary string
			char tmp_str[400];
			BP_ZERO_STACK(tmp_str);

			// copy in string
			sprintf(tmp_str, "BP_DBColumnsAdd attempted a BP_StringIsReasonableForColumnNameDB filter check on columns[%zu].bpdb_column_name but filter check failed.", n);

			// add error message
			BP_DBAddErrorMsg(bpdb, (char *) &tmp_str);

			// exit routine
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(bpdb->system_hreg, ERR_FAILURE);

		}

	}


	// lookup table metadata
	P_BPDB_SQL_TABLE_METADATA table_metadata = BP_DBLookupTableMetadataFromTableName(bpdb, table_name);
	if(!table_metadata)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBColumnsAdd failed table lookup via BP_DBLookupTableMetadataFromTableName ");

		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(bpdb->system_hreg, ERR_FAILURE);
	}


	// at this point column names have been sanitized, so we can go ahead and add them
	for(n = 0; n < column_n; n++)
	{


		// printf("\n Attempting to add column?!: %s - %u - %u", columns[n].column_name, n, column_n);

		// Add column.  Should never fail.
		BP_ERROR_T ret_val = BP_DBColumnAdd
		(
			bpdb,
			table_name,
			columns[n].bpdb_column_name,
			columns[n].type,
			columns[n].primary_key,
			columns[n].indexed,
			columns[n].unique,
			columns[n].auto_inc
		);

		// exit immediately if column isn't added
		if(!ret_val)
		{
			// create space for a temporary string
			char tmp_str[400];
			BP_ZERO_STACK(tmp_str);

			// copy in string
			sprintf(tmp_str, "BP_DBColumnsAdd attempted to call BP_DBColumnAdd for column %zu but failed.", n);

			// add error message
			BP_DBAddErrorMsg(bpdb, (char *) &tmp_str);
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(bpdb->system_hreg, ERR_FAILURE);
		}


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Adjust Table Metadata %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// increment column count
		table_metadata->column_count++;

		// increment designators where applicable (+1 on BP_TRUE, +0 on BP_FALSE)
		table_metadata->total_primary_key_cols += columns[n].primary_key;
		table_metadata->total_indexed_cols     += columns[n].indexed;
		table_metadata->total_unique_cols      += columns[n].unique;
		table_metadata->total_auto_inc_cols    += columns[n].auto_inc;


	}


	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(bpdb->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Schema Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// install local schema
BP_ERROR_T BP_DBSchemaInstallLocal(P_BPDB bpdb)
{

	// ensure we have a bpdb
	if(!bpdb)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(bpdb->system_hreg);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Install Results Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Create results table
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_RESULTS);

	// Install result_base_uris table.
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_RESULT_BASE_URIS);

	// Install result_path_uris table.
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_RESULT_PATH_URIS);

	// Install result_script_uris table.
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_RESULT_SCRIPT_URIS);

	// Install result_hosts table.
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_RESULT_HOSTS);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Install Dataset Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// --- proxies ---

	// install proxies table
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_PROXIES);

	// install proxy_groups table
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_PROXY_GROUPS);

	// --- wordlists ---

	// install wordlists table
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_WORDLISTS);

	// install wordlist groups table
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_WORDLIST_GROUPS);

	// --- useragents ---

	// install useragents table
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_USERAGENTS);

	// install useragent groups table
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_USERAGENT_GROUPS);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Install Intel Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// search term groups
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_SEARCH_TERM_GROUPS);


	// install jit expression groups table
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_WEB_JIT_EXPRESSION_GROUPS);

	// install jit expression matches table
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_WEB_JIT_EXPRESSION_MATCHES);

	// install jit expression tests table
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_WEB_JIT_EXPRESSION_TESTS);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Install HTML Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// install anchor(s) tables
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_HTML_ANCHOR_DATA);
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_HTML_ANCHOR_VAR_DATA);

	// install form(s) tables
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_HTML_FORM_DATA);
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_HTML_FORM_INPUT_DATA);
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_HTML_FORM_INPUT_PROPERTY_DATA);
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_HTML_FORM_PROPERTY_DATA);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Install Sensor Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// install basic sensor tables
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_SENSORS);
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_SENSOR_CONFIG_REGISTRY);
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_SENSOR_GROUPS);
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_SENSOR_HEALTH_UPDATES);
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_SENSOR_MESSAGES);
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_SENSOR_REQUESTS);
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_SENSOR_SUBSYSTEM_CONFIG);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Install Auth Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// api key(s) table
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_KEYS);
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_KEY_GROUPS);
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_KEY_PRIVS);

	// user(s) table
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_KEY_USERS);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Install Assessment Tables %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// assesment table
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_ASSESSMENTS);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Install Access Lists %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add whitelists
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_WHITELISTS);
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_WHITELIST_GROUPS);

	// add blacklists
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_BLACKLISTS);
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_BLACKLIST_GROUPS);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Install Third Party Tools Tables %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// nmap tables
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_NMAP_HOSTNAME_INFO);
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_NMAP_HOST_INFO);
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_NMAP_PORT_INFO);

	// generic tables
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_COMMAND_LINE_TOOLS);
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_COMMAND_LINE_TOOL_DATA);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Install Report(s) Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// report tables
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_REPORTS_GENERATED);
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_REPORT_TEMPLATES);
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_REPORT_TEMPLATE_SNIPPETS);
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_REPORT_TEMPLATE_SNIPPET_GROUPS);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Install Test Sets Schema %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// validation tests
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_VALIDATION_TESTS);
	BP_DBSchemaInstallTableFromBufferLocal(bpdb, BP_DB_SQLITE_CREATE_TABLE_VALIDATION_TEST_GROUPS);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Leave Critical Section and Exit %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(bpdb->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}


// simple routine to sintall a table into a buffer
BP_ERROR_T BP_DBSchemaInstallTableFromBufferLocal(P_BPDB bpdb, char * table_sql)
{


	// ensure we have a bpdb
	if(!bpdb)
		return ERR_FAILURE;
	if(!bpdb->sqlite)
		return ERR_FAILURE;
	if(!table_sql)
		return ERR_FAILURE;

	// table must be reasonable/printable
	if(!BP_StringIsReasonablePrintableString(table_sql, bpstrlen(table_sql), BP_TRUE, BP_FALSE))
		return ERR_FAILURE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Install Tables %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(bpdb->system_hreg);

	// add query
	BP_SQLiteAddQuery
	(
			bpdb->sqlite,
			BP_DB_SQLITE_QUERY_TYPE_CREATE_TABLE,
			table_sql
	);

	// run table queries
	BP_SQLiteRunQueries(bpdb->sqlite);

	// clear query and result caches
	BP_SQLiteClearBothQueryAndResultCaches(bpdb->sqlite);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Leave Critical Section and Exit %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(bpdb->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Flush Records Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This attempts to flush records out to the database.
BP_ERROR_T BP_DBFlushRecordsOut(P_BPDB bpdb, BPDB_SELECTOR_TYPE out_db_selector, BP_BOOL run_in_transaction)
{

	// run basic null checks
	if(!bpdb)
		return ERR_FAILURE;
	if(!out_db_selector)
		return ERR_FAILURE;


	// create new syntax generator
	P_BPDB_SQLGEN syngen = BP_DBSGOpen(bpdb);
	if(!syngen)
		return ERR_FAILURE;

	// verbosity (optional)
	printf("\n [!!] Attempting to flush records to selected db: %p", syngen);

	// switch/action based on database selector
	switch(out_db_selector)
	{

		case BPDB_SELECTOR_TYPE_LOCALCACHE_DATABASE:
			printf("\n [+] Flushing to local database.");
			BP_DBFlushRecordsOutToLocalCache(bpdb, syngen, run_in_transaction);
			break;

		case BPDB_SELECTOR_TYPE_PERSISTENT_DATABASE:
			break;

		case BPDB_SELECTOR_TYPE_BOTH_DATABASES:
			BP_DBFlushRecordsOutToLocalCache(bpdb, syngen, run_in_transaction);
			break;

	}

	// return indicating success
	return ERR_SUCCESS;

}


// Called from BP_DBFlushRecordsOut.  Will flush records to the local sqlite db if possible.
BP_ERROR_T BP_DBFlushRecordsOutToLocalCache(P_BPDB bpdb, P_BPDB_SQLGEN syngen, BP_BOOL run_in_transaction)
{

	// ensure we have a bpdb and a syntax generator
	if(!bpdb)
		return ERR_FAILURE;
	if(!syngen)
		return ERR_FAILURE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Run Syntax Generator %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	BP_HASH_ITER_OPEN(bpdb->system_hreg->hive, table_iter)
	{

		// set table name
		char * table_name = table_iter->key;


		printf("\n [+] Table Iter?: %s", table_iter->key);
		size_t max_records = 0;

		// lookup table metadata
		P_BPDB_SQL_TABLE_METADATA table_metadata = BP_DBLookupTableMetadataFromTableName(bpdb, table_name);
		if(!table_metadata)
			break;

		// ensure we have a total record count
		if(!table_metadata->total_records_in)
			continue;

		// Generate the insert into query for sqlite.
		BP_DBSGInsertInto
		(
			syngen,
			BPDB_SELECTOR_TYPE_LOCALCACHE_DATABASE,
			table_iter->key,
			max_records
		);


	} BP_HASH_ITER_CLOSE;

	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Lookup Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// looks up a table by a table n
P_BP_HASH_TABLE_KEYED_ENTRY BP_DBLookupTableByName(P_BPDB bpdb, char * table_name)
{

	// ensure we have a table name
	if(!table_name)
		return NULL;

	// ensure name is reasonable for table
	if(!BP_StringIsReasonableForTableNameDB(table_name, bpstrlen(table_name)))
		return NULL;

	// create table path stack
	char * table_path[10];
	BP_ZERO_STACK(table_path);

	// set table name for lookup
	table_path[0] = "tables";
	table_path[1] = table_name;
	table_path[2] = NULL;

	// lookup table
	P_BP_HASH_TABLE_KEYED_ENTRY table = (P_BP_HASH_TABLE_KEYED_ENTRY) BP_HashRegLookupTableByPath(bpdb->system_hreg, (char **) &table_path);

	// table name (or null if unfound)
	return table;

}



// looks up the columns table for a given table name.  This is the keyed entry for the
// tables/specific_table/columns/ path.
P_BP_HASH_TABLE_KEYED_ENTRY BP_DBLookupTableColumns(P_BPDB bpdb, char *table_name)
{

	// ensure we have a table name
	if(!table_name)
		return NULL;

	// ensure name is reasonable for table
	if(!BP_StringIsReasonableForTableNameDB(table_name, bpstrlen(table_name)))
		return NULL;

	// create table columns path stack
	char * path[10];
	BP_ZERO_STACK(path);

	// set lookup path
	path[0] = "tables";
	path[1] = table_name;
	path[2] = "columns";
	path[3] = NULL;

	// lookup table
	P_BP_HASH_TABLE_KEYED_ENTRY columns = (P_BP_HASH_TABLE_KEYED_ENTRY) BP_HashRegLookupTableByPath(bpdb->system_hreg, (char **) &path);

	// column, or null if unfound
	return columns;

}

// looks up a column given a table name and column name for lookup
P_BP_HASH_TABLE_KEYED_ENTRY BP_DBLookupTableColumnByName(P_BPDB bpdb, char *table_name, char *column_name)
{

	// ensure we have a table name
	if(!table_name)
		return NULL;
	if(!column_name)
		return NULL;

	// ensure name is reasonable for table
	if(!BP_StringIsReasonableForTableNameDB(table_name, bpstrlen(table_name)))
		return NULL;

	// ensure column name is reasonable
	if(!BP_StringIsReasonableForColumnNameDB(column_name, bpstrlen(column_name)))
		return NULL;

	// create table path stack
	char * table_path[10];
	BP_ZERO_STACK(table_path);

	// set table name for lookup
	table_path[0] = "tables";
	table_path[1] = table_name;
	table_path[2] = "columns";
	table_path[3] = column_name;
	table_path[4] = NULL;

	// lookup table
	P_BP_HASH_TABLE_KEYED_ENTRY column = (P_BP_HASH_TABLE_KEYED_ENTRY) BP_HashRegLookupTableByPath(bpdb->system_hreg, (char **) &table_path);

	// column, or null if unfound
	return column;

}

// this is a utility version of BP_DBLookupTableColumnByName that takes a field in the form of table.column, and
// automatically splits it before calling BP_DBLookupTableColumnByName to attempt lookup.
P_BP_HASH_TABLE_KEYED_ENTRY BP_DBLookupTableColumnByJoinedFieldName(P_BPDB bpdb, char *joined_table_column_field)
{

	// ensure we have a bpdb element
	if(!bpdb)
		return NULL;

	// check if the string is reasonable
	BP_BOOL field_is_ok = BP_StringIsReasonableForSelectFieldSpecifierDB(joined_table_column_field, bpstrlen(joined_table_column_field));

	// if the field isn't ok, exit indicating failure
	if(field_is_ok == BP_FALSE)
		return NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Separate Field from Table and Check %%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create table and field
	char ** table_and_field = BP_SplitBuffer(joined_table_column_field, ".");
	if(!table_and_field)
		return NULL;

	// ensure we have table and column
	if(table_and_field[0] && table_and_field[1])
	{

		// attempt to retrieve column data
		P_BP_HASH_TABLE_KEYED_ENTRY column_lookup = BP_DBLookupTableColumnByName(bpdb, table_and_field[0], table_and_field[1]);

		// destroy the pointer array
		BP_DestroyPointerArray((void **) table_and_field, 2);

		// destroy the table and field entry
		bpfree(table_and_field);

		// return the looked up data (or null if lookup fails)
		return column_lookup;

	}

	// destroy the pointer array
	BP_DestroyPointerArray((void **) table_and_field, 2);

	// destroy the table and field entry
	bpfree(table_and_field);

	// return indicating successNULL
	return NULL;

}

// attempt to lookup table metadata by name.
P_BPDB_SQL_TABLE_METADATA BP_DBLookupTableMetadataFromTableName(P_BPDB bpdb, char *table_name)
{

	// ensure we have a table name
	if(!table_name)
		return NULL;

	// ensure name is reasonable for table
	if(!BP_StringIsReasonableForTableNameDB(table_name, bpstrlen(table_name)))
		return NULL;


	// create table path stack
	char * table_path[10];
	BP_ZERO_STACK(table_path);

	// set table name for lookup
	table_path[0] = "tables";
	table_path[1] = table_name;
	table_path[2] = BP_SQL_TABLE_METADATA_RESERVED_TABLE_NAME;
	table_path[3] = NULL;

	// lookup table
	P_BP_HASH_TABLE_KEYED_ENTRY table_metadata_entry = (P_BP_HASH_TABLE_KEYED_ENTRY) BP_HashRegLookupTableEntryByPath(bpdb->system_hreg, (char **) &table_path);

	// return metadata if found ok
	if(table_metadata_entry)
	if(table_metadata_entry->data)
		return (P_BPDB_SQL_TABLE_METADATA) table_metadata_entry->data;

	// return NULL if lookup fails
	return NULL;

}


// attempt to lookup table column metatadata by name
P_BPDB_SQL_TABLE_COLUMN_METADATA BP_DBLookupTableColumnMetadataFromColumnName
(
		P_BPDB bpdb,
		char *table_name,
		char *column_name
)
{

	// ensure we have a table name
	if(!table_name)
		return NULL;

	// ensure name is reasonable for table
	if(!BP_StringIsReasonableForTableNameDB(table_name, bpstrlen(table_name)))
		return NULL;

	// ensure string is reasonable for the column name
	if(!BP_StringIsReasonableForColumnNameDB(column_name, bpstrlen(column_name)))
		return NULL;

	// create table path stack
	char * table_path[10];
	BP_ZERO_STACK(table_path);

	// set table name for lookup
	table_path[0] = "tables";
	table_path[1] = table_name;
	table_path[2] = "columns";
	table_path[3] = column_name;
	table_path[4] = BP_SQL_TABLE_COLUMN_METADATA_RESERVED_TABLE_NAME;
	table_path[5] = NULL;

	// lookup table
	P_BP_HASH_TABLE_KEYED_ENTRY table_column_metadata_entry = (P_BP_HASH_TABLE_KEYED_ENTRY) BP_HashRegLookupTableEntryByPath(bpdb->system_hreg, (char **) &table_path);

	// return metadata if found ok
	if(table_column_metadata_entry)
	if(table_column_metadata_entry->data)
		return (P_BPDB_SQL_TABLE_COLUMN_METADATA) table_column_metadata_entry->data;

	// return NULL if lookup fails
	return NULL;

}


// attempt to lookup table column metatadata by name
P_BPDB_SQL_TABLE_COLUMN_METADATA BP_DBLookupTableColumnMetadataFromJoinedFieldName(P_BPDB bpdb, char *joined_table_column_field)
{

	// check if the string is reasonable
	BP_BOOL field_is_ok = BP_StringIsReasonableForSelectFieldSpecifierDB(joined_table_column_field, bpstrlen(joined_table_column_field));

	// if the field isn't ok, exit indicating failure
	if(field_is_ok == BP_FALSE)
		return NULL;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Separate Field from Table and Check %%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create table and field
	char ** table_and_field = BP_SplitBuffer(joined_table_column_field, ".");
	if(!table_and_field)
		return NULL;

	// ensure we have table and column
	if(table_and_field[0] && table_and_field[1])
	{

		// attempt to retrieve column data
		P_BPDB_SQL_TABLE_COLUMN_METADATA column_metadata_lookup = BP_DBLookupTableColumnMetadataFromColumnName(bpdb, table_and_field[0], table_and_field[1]);

		// destroy the pointer array
		BP_DestroyPointerArray((void **) table_and_field, 2);

		// destroy the table and field entry
		bpfree(table_and_field);

		// return the looked up data (or null if lookup fails)
		return column_metadata_lookup;

	}


	// destroy the pointer array
	BP_DestroyPointerArray((void **) table_and_field, 2);

	// destroy the table and field entry
	bpfree(table_and_field);

	// return NULL if the lookup fails
	return NULL;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Mariadb Column Information Resolution Routines %%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// this takes in mariadb metadata and provides a numeric type
BPDB_MARIADB_COLUMN_TYPE BP_DBResolveMariadbColumnMetadataToType(P_BPDB_MARIADB_TABLE_COLUMN_METADATA mariadb_column_metadata)
{

	// ensure we have metadata to check
	if(!mariadb_column_metadata)
		return BPDB_MARIADB_COLUMN_TYPE_UNSET;


	// check to ensure that the type field is reasonable
	if(!BP_StringIsReasonableForColumnTypeFieldDB(mariadb_column_metadata->data_type, mariadb_column_metadata->data_type_data_len))
	{
		return BPDB_MARIADB_COLUMN_TYPE_UNSET;
	}


	enum
	{
		IDX_TINYINT,
		IDX_SMALLINT,
		IDX_MEDIUMINT,
		IDX_INT,
		IDX_BIGINT,
		IDX_DECIMAL,
		IDX_FLOAT,
		IDX_DOUBLE,
		IDX_BIT,
		IDX_CHAR,
		IDX_VARCHAR,
		IDX_BINARY,
		IDX_VARBINARY,
		IDX_TINYBLOB,
		IDX_BLOB,
		IDX_MEDIUMBLOB,
		IDX_LONGBLOB,
		IDX_TINYTEXT,
		IDX_TEXT,
		IDX_MEDIUMTEXT,
		IDX_LONGTEXT,
		IDX_ENUM,
		IDX_DATE,
		IDX_TIME,
		IDX_DATETIME,
		IDX_TIMESTAMP,
		IDX_YEAR,
		IDXTOP
	};


	// these are all the type prefixes found within mariadb (https://mariadb.com/kb/en/mariadb/data-types/)

	// iterator through the type prefixes
	size_t idx_iter = 0;

	// declare all prefixes as values from the above enum
	char * type_prefixes[IDXTOP+1];
	type_prefixes[IDX_TINYINT]     = "tinyint";
	type_prefixes[IDX_SMALLINT]    = "smallint";
	type_prefixes[IDX_MEDIUMINT]   = "mediumint";
	type_prefixes[IDX_INT]         = "int";
	type_prefixes[IDX_BIGINT]      = "bigint";
	type_prefixes[IDX_DECIMAL]     = "decimal";
	type_prefixes[IDX_FLOAT]       = "float";
	type_prefixes[IDX_DOUBLE]      = "double";
	type_prefixes[IDX_BIT]         = "bit";
	type_prefixes[IDX_CHAR]        = "char";
	type_prefixes[IDX_VARCHAR]     = "varchar";
	type_prefixes[IDX_BINARY]      = "binary";
	type_prefixes[IDX_VARBINARY]   = "varbinary";
	type_prefixes[IDX_TINYBLOB]    = "tinyblob";
	type_prefixes[IDX_BLOB]        = "blob";
	type_prefixes[IDX_MEDIUMBLOB]  = "mediumblob";
	type_prefixes[IDX_LONGBLOB]    = "longblob";
	type_prefixes[IDX_TINYTEXT]    = "tinytext";
	type_prefixes[IDX_TEXT]        = "text";
	type_prefixes[IDX_MEDIUMTEXT]  = "mediumtext";
	type_prefixes[IDX_LONGTEXT]    = "longtext";
	type_prefixes[IDX_ENUM]        = "enum";
	type_prefixes[IDX_DATE]        = "date";
	type_prefixes[IDX_TIME]        = "time";
	type_prefixes[IDX_DATETIME]    = "datetime";
	type_prefixes[IDX_TIMESTAMP]   = "timestamp";
	type_prefixes[IDX_YEAR]        = "year";
	type_prefixes[IDXTOP]          = NULL;

	// initialize the type iterator
	size_t type_iter = 0;

	// walk type prefixes
	for(; type_prefixes[type_iter]; type_iter++)
	{


		// initialize prefix and compare lengths
		size_t prefix_len  = 0;
		size_t compare_len = 0;

		// calculate type prefix length
		prefix_len = bpstrlen(type_prefixes[type_iter]);

		// calculate the compare length
		compare_len = bpstrlen(mariadb_column_metadata->data_type);

		// stop the loop if the prefix length is greater than the compare length
		if(prefix_len > compare_len)
			continue;

		// if the prefix doesn't match at this position, continue the loop
		if(memcmp(mariadb_column_metadata->data_type, type_prefixes[type_iter], prefix_len) != 0)
			continue;

		// switch on the type iter and return the correct type where necessary
		switch(type_iter)
		{
			case IDX_TINYINT:
				return BPDB_MARIADB_COLUMN_TYPE_TINYINT;

			case IDX_SMALLINT:
				return BPDB_MARIADB_COLUMN_TYPE_SMALLINT;

			case IDX_MEDIUMINT:
				return BPDB_MARIADB_COLUMN_TYPE_MEDIUMINT;

			case IDX_INT:
				return BPDB_MARIADB_COLUMN_TYPE_INT;


			case IDX_BIGINT:
				return BPDB_MARIADB_COLUMN_TYPE_BIGINT;

			case IDX_DECIMAL:
				return BPDB_MARIADB_COLUMN_TYPE_DECIMAL;

			case IDX_FLOAT:
				return BPDB_MARIADB_COLUMN_TYPE_FLOAT;

			case IDX_DOUBLE:
				return BPDB_MARIADB_COLUMN_TYPE_DOUBLE;

			case IDX_BIT:
				return BPDB_MARIADB_COLUMN_TYPE_BIT;

			case IDX_CHAR:
				return BPDB_MARIADB_COLUMN_TYPE_CHAR;

			case IDX_VARCHAR:
				return BPDB_MARIADB_COLUMN_TYPE_VARCHAR;

			case IDX_BINARY:
				return BPDB_MARIADB_COLUMN_TYPE_BINARY;

			case IDX_VARBINARY:
				return BPDB_MARIADB_COLUMN_TYPE_VARBINARY;

			case IDX_TINYBLOB:
				return BPDB_MARIADB_COLUMN_TYPE_TINYBLOB;

			case IDX_BLOB:
				return BPDB_MARIADB_COLUMN_TYPE_BLOB;

			case IDX_MEDIUMBLOB:
				return BPDB_MARIADB_COLUMN_TYPE_MEDIUMBLOB;

			case IDX_LONGBLOB:
				return BPDB_MARIADB_COLUMN_TYPE_LONGBLOB;

			case IDX_TINYTEXT:
				return BPDB_MARIADB_COLUMN_TYPE_TINYTEXT;

			case IDX_TEXT:
				return BPDB_MARIADB_COLUMN_TYPE_TEXT;

			case IDX_MEDIUMTEXT:
				return BPDB_MARIADB_COLUMN_TYPE_MEDIUMTEXT;

			case IDX_LONGTEXT:
				return BPDB_MARIADB_COLUMN_TYPE_LONGTEXT;

			case IDX_ENUM:
				return BPDB_MARIADB_COLUMN_TYPE_ENUM;

			case IDX_DATE:
				return BPDB_MARIADB_COLUMN_TYPE_DATE;

			case IDX_TIME:
				return BPDB_MARIADB_COLUMN_TYPE_TIME;

			case IDX_DATETIME:
				return BPDB_MARIADB_COLUMN_TYPE_DATETIME;

			case IDX_TIMESTAMP:
				return BPDB_MARIADB_COLUMN_TYPE_TIMESTAMP;

			case IDX_YEAR:
				return BPDB_MARIADB_COLUMN_TYPE_YEAR;

			default:
				break;

		}

	}

	// return unset if we couldn't resolve the value
	return BPDB_MARIADB_COLUMN_TYPE_UNSET;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Numeric Utilities for BPDB %%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempts to retrieve the number of columns for a specified table
size_t BP_DBGetColumnCountForTable
(
		P_BPDB bpdb,
		char *table_name
)
{

	// run basic checks
	if(!bpdb)
		return NULL;
	if(!table_name)
		return NULL;
	if(!BP_StringIsReasonableForTableNameDB(table_name, bpstrlen(table_name)))
		return NULL;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Attempt to lookup table %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(bpdb->system_hreg);


	// attempt to look up the columns "table" within the hash registry
	P_BP_HASH_TABLE_KEYED_ENTRY columns_table = BP_DBLookupTableColumns(bpdb, table_name);

	// return zero if we couldn't perform a lookup
	if(!columns_table)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(bpdb->system_hreg, 0);

	// return zero if the lookup has no entries
	if(!columns_table->data)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(bpdb->system_hreg, 0);


	// this is the value to be returned
	size_t column_count = 0;

	// open the iterator
	BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) columns_table->data, column_iter)
	{

		// increment the column count
		column_count++;

	} BP_HASH_ITER_CLOSE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Exit Critical Section and Return Row %%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(bpdb->system_hreg);

	// return the filled out column count
	return column_count;
}




// converts string to a bpcalloced string version of the metadata type.
char * BP_DBColumnTypeToString(BPDB_SQL_TABLE_COLUMN_METADATA_TYPE type)
{

	// gen string based on switch
	switch(type)
	{

		case BPDB_SQL_TABLE_COLUMN_METADATA_TYPE_UNSET:
			return bpstrdup("BPDB_SQL_TABLE_COLUMN_METADATA_TYPE_UNSET");
			break;

		case BPDB_SQL_TABLE_COLUMN_METADATA_TYPE_INTEGER:
			return bpstrdup("BPDB_SQL_TABLE_COLUMN_METADATA_TYPE_INTEGER");
			break;

		case BPDB_SQL_TABLE_COLUMN_METADATA_TYPE_BIN:
			return bpstrdup("BPDB_SQL_TABLE_COLUMN_METADATA_TYPE_BIN");
			break;

		case BPDB_SQL_TABLE_COLUMN_METADATA_TYPE_TEXT:
			return bpstrdup("BPDB_SQL_TABLE_COLUMN_METADATA_TYPE_TEXT");
			break;

		default:
			break;

	}

	// return null if there is no type match
	return NULL;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Utility Conversion Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to convert a maria db type to string.  This is mainly used for the creation of JSON
// record outputs.  If the bpdb parameter is non-null, the allocation returned will be ht_strdup'ed
// rather than bpstrduped.  Result must be ht_free/bpfree'd accordingly.
char * BP_DBMariadbColumnTypeToString(BPDB_MARIADB_COLUMN_TYPE type, P_BP_HASH_TABLE_REGISTRY hreg)
{

	// set the return string to null
	char *ret_str = NULL;

	// switch on the type
	switch(type)
	{

		case BPDB_MARIADB_COLUMN_TYPE_UNSET:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_UNSET");
			break;

		/* begin numeric data types */
		case BPDB_MARIADB_COLUMN_TYPE_TINYINT:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_TINYINT");
			break;
		case BPDB_MARIADB_COLUMN_TYPE_SMALLINT:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_SMALLINT");
			break;
		case BPDB_MARIADB_COLUMN_TYPE_MEDIUMINT:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_MEDIUMINT");
			break;
		case BPDB_MARIADB_COLUMN_TYPE_INT:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_INT");
			break;
		case BPDB_MARIADB_COLUMN_TYPE_BIGINT:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_BIGINT");
			break;
		case BPDB_MARIADB_COLUMN_TYPE_DECIMAL:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_DECIMAL");
			break;
		case BPDB_MARIADB_COLUMN_TYPE_FLOAT:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_FLOAT");
			break;
		case BPDB_MARIADB_COLUMN_TYPE_DOUBLE:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_DOUBLE");
			break;
		case BPDB_MARIADB_COLUMN_TYPE_BIT:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_BIT");
			break;

		/* begin string data types */
		case BPDB_MARIADB_COLUMN_TYPE_CHAR:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_CHAR");
			break;
		case BPDB_MARIADB_COLUMN_TYPE_VARCHAR:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_VARCHAR");
			break;
		case BPDB_MARIADB_COLUMN_TYPE_BINARY:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_BINARY");
			break;
		case BPDB_MARIADB_COLUMN_TYPE_VARBINARY:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_VARBINARY");
			break;
		case BPDB_MARIADB_COLUMN_TYPE_TINYBLOB:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_TINYBLOB");
			break;
		case BPDB_MARIADB_COLUMN_TYPE_BLOB:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_BLOB");
			break;
		case BPDB_MARIADB_COLUMN_TYPE_MEDIUMBLOB:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_MEDIUMBLOB");
			break;
		case BPDB_MARIADB_COLUMN_TYPE_LONGBLOB:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_LONGBLOB");
			break;
		case BPDB_MARIADB_COLUMN_TYPE_TINYTEXT:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_TINYTEXT");
			break;
		case BPDB_MARIADB_COLUMN_TYPE_TEXT:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_TEXT");
			break;
		case BPDB_MARIADB_COLUMN_TYPE_MEDIUMTEXT:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_MEDIUMTEXT");
			break;
		case BPDB_MARIADB_COLUMN_TYPE_LONGTEXT:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_LONGTEXT");
			break;
		case BPDB_MARIADB_COLUMN_TYPE_ENUM:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_ENUM");
			break;

		/* begin date and time data types */
		case BPDB_MARIADB_COLUMN_TYPE_DATE:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_DATE");
			break;

		case BPDB_MARIADB_COLUMN_TYPE_TIME:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_TIME");
			break;

		case BPDB_MARIADB_COLUMN_TYPE_DATETIME:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_DATETIME");
			break;

		case BPDB_MARIADB_COLUMN_TYPE_TIMESTAMP:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_TIMESTAMP");
			break;

		case BPDB_MARIADB_COLUMN_TYPE_YEAR:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BPDB_MARIADB_COLUMN_TYPE_YEAR");
			break;

		default:
			ret_str = BP_DB_MARIADB_COL_TYPE_STR_ALLOC("BAD_TYPE_VAL");
			break;

	}

	// return the new string
	return ret_str;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Result Data Autoconversion %%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// This function formats result information into proper output for json.  This is mainly used for autojson creation.  This
// will call one of the functions below by switching off based on type.  Each basic type has it's own function to make things
// developmentally clearer.
char * BP_DBMariadbConvertColumnResultDataToJSONData
(
	MYSQL_BIND             * col_bind_info,
	BPDB_MARIADB_COLUMN_TYPE type
)
{

	if(!col_bind_info)
		return NULL;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Buffers for Int Types %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create space for potential integer output
	char int_out_buff[256];
	BP_ZERO_STACK(int_out_buff);

	// create temporary types
	long long int           * tmp_llint_ptr  = 0;
	long long unsigned int  tmp_lluint_ptr = 0;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Buffers for Blob Types %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// arbitrary blob data
	char * blob_data = NULL;
	size_t blob_len  = 0;




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Buffers for Time Types %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Javascript time format is:
	//
	// day_of_week
	// month
	// day of month
	// year
	// HH:MM:SS
	// timezone

	// structure to hold time data
	MYSQL_TIME * time_data = NULL;

	// switch on the type
	switch(type)
	{

		case BPDB_MARIADB_COLUMN_TYPE_UNSET:
			break;

		/* begin numeric data types */
		case BPDB_MARIADB_COLUMN_TYPE_TINYINT:
		case BPDB_MARIADB_COLUMN_TYPE_SMALLINT:
		case BPDB_MARIADB_COLUMN_TYPE_MEDIUMINT:
		case BPDB_MARIADB_COLUMN_TYPE_INT:
		case BPDB_MARIADB_COLUMN_TYPE_BIGINT:
		case BPDB_MARIADB_COLUMN_TYPE_DECIMAL:
		case BPDB_MARIADB_COLUMN_TYPE_FLOAT:
		case BPDB_MARIADB_COLUMN_TYPE_DOUBLE:
		case BPDB_MARIADB_COLUMN_TYPE_BIT:
			return BP_DBMariadbConvertIntegerColumnResultDataToJSONData(col_bind_info, type);

		/* begin string data types */
		case BPDB_MARIADB_COLUMN_TYPE_CHAR:
		case BPDB_MARIADB_COLUMN_TYPE_VARCHAR:
		case BPDB_MARIADB_COLUMN_TYPE_BINARY:
		case BPDB_MARIADB_COLUMN_TYPE_VARBINARY:
		case BPDB_MARIADB_COLUMN_TYPE_TINYBLOB:
		case BPDB_MARIADB_COLUMN_TYPE_BLOB:
		case BPDB_MARIADB_COLUMN_TYPE_MEDIUMBLOB:
		case BPDB_MARIADB_COLUMN_TYPE_LONGBLOB:
		case BPDB_MARIADB_COLUMN_TYPE_TINYTEXT:
		case BPDB_MARIADB_COLUMN_TYPE_TEXT:
		case BPDB_MARIADB_COLUMN_TYPE_MEDIUMTEXT:
		case BPDB_MARIADB_COLUMN_TYPE_LONGTEXT:
			return BP_DBMariadbConvertBlobColumnResultDataToJSONData(col_bind_info, type);

		/* todo: enum data type */
		case BPDB_MARIADB_COLUMN_TYPE_ENUM:
			break;

		/* begin date and time data types */
		case BPDB_MARIADB_COLUMN_TYPE_DATE:
		case BPDB_MARIADB_COLUMN_TYPE_TIME:
		case BPDB_MARIADB_COLUMN_TYPE_DATETIME:
		case BPDB_MARIADB_COLUMN_TYPE_TIMESTAMP:
		case BPDB_MARIADB_COLUMN_TYPE_YEAR:
			return BP_DBMariadbConvertTimeColumnResultToJSONData(col_bind_info, type);

		default:
			break;

	}

	return NULL;
}

// Attempts to convert INTEGER column result data to JSON data.
char * BP_DBMariadbConvertIntegerColumnResultDataToJSONData(MYSQL_BIND * col_bind_info, BPDB_MARIADB_COLUMN_TYPE type)
{

	// ensure we have bind info
	if(!col_bind_info)
		return NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Declare Temporary Integer Ptr Types %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// short/ushort
	short  * short_ptr = NULL;
	unsigned short * ushort_ptr = NULL;

	// long/ulong
	long        * long_ptr = NULL;
	unsigned long ulong_ptr = NULL;

	// longlong/ulonglong
	long long        * longlong_ptr = NULL;
	unsigned long long ulonglong_ptr = NULL;

	// long floats for decimal types
	float       * float_ptr  = NULL;
	double      * double_ptr = NULL;

	// create new out buffer
	char int_out_buff[256] = {0};


	switch(type)
	{

		case BPDB_MARIADB_COLUMN_TYPE_UNSET:
			break;

		/* begin numeric data types */
		case BPDB_MARIADB_COLUMN_TYPE_BIT:
			short_ptr = (short *) col_bind_info->buffer;
			sprintf(int_out_buff, "%i", *short_ptr);
			break;

		case BPDB_MARIADB_COLUMN_TYPE_TINYINT:
			short_ptr = (short *) col_bind_info->buffer;
			sprintf(int_out_buff, "%i", *short_ptr);
			break;

		case BPDB_MARIADB_COLUMN_TYPE_SMALLINT:
			short_ptr = (short *) col_bind_info->buffer;
			sprintf(int_out_buff, "%i", *short_ptr);
			break;

		case BPDB_MARIADB_COLUMN_TYPE_MEDIUMINT:
			long_ptr = (long *) col_bind_info->buffer;
			sprintf(int_out_buff, "%i", *long_ptr);
			break;

		case BPDB_MARIADB_COLUMN_TYPE_INT:
			long_ptr = (long *) col_bind_info->buffer;
			sprintf(int_out_buff, "%i", *long_ptr);
			break;

		case BPDB_MARIADB_COLUMN_TYPE_BIGINT:
			longlong_ptr = (long long *) col_bind_info->buffer;
			sprintf(int_out_buff, "%i", *longlong_ptr);
			break;

		case BPDB_MARIADB_COLUMN_TYPE_DECIMAL:
			float_ptr = (float *) col_bind_info->buffer;
			sprintf(int_out_buff, "%lf", *float_ptr);
			break;

		case BPDB_MARIADB_COLUMN_TYPE_FLOAT:
			float_ptr = (float *) col_bind_info->buffer;
			sprintf(int_out_buff, "%lf", *float_ptr);
			break;

		case BPDB_MARIADB_COLUMN_TYPE_DOUBLE:
			double_ptr = (double *) col_bind_info->buffer;
			sprintf(int_out_buff, "%llf", *double_ptr);
			break;

		default:
			break;
	}

	// if we have a buffer to return, return it
	if(bpstrlen(int_out_buff))
	{
		return bpstrdup(int_out_buff);
	}

	// return NULL if we have no type match
	return NULL;

}


// Attempts to convert BLOB column result data to base64 encoded output.
char * BP_DBMariadbConvertBlobColumnResultDataToJSONData(MYSQL_BIND * col_bind_info, BPDB_MARIADB_COLUMN_TYPE type)
{

	// ensure we have bind info
	if(!col_bind_info)
		return NULL;
	if(!col_bind_info->buffer)
		return NULL;

	// return buffer
	char *ret_buff = NULL;

	// switch on type
	switch(type)
	{

		// binary blob types
		case BPDB_MARIADB_COLUMN_TYPE_CHAR:
		case BPDB_MARIADB_COLUMN_TYPE_VARCHAR:
		case BPDB_MARIADB_COLUMN_TYPE_BINARY:
		case BPDB_MARIADB_COLUMN_TYPE_VARBINARY:
		case BPDB_MARIADB_COLUMN_TYPE_TINYBLOB:
		case BPDB_MARIADB_COLUMN_TYPE_BLOB:
		case BPDB_MARIADB_COLUMN_TYPE_MEDIUMBLOB:
		case BPDB_MARIADB_COLUMN_TYPE_LONGBLOB:
		case BPDB_MARIADB_COLUMN_TYPE_TINYTEXT:
		case BPDB_MARIADB_COLUMN_TYPE_TEXT:
		case BPDB_MARIADB_COLUMN_TYPE_MEDIUMTEXT:
		case BPDB_MARIADB_COLUMN_TYPE_LONGTEXT:

			// create encode and return the buffer
			ret_buff = BP_Base64Encode((unsigned char *) col_bind_info->buffer, col_bind_info->buffer_length);
			break;

		default:
			break;

	}

	// return the buffer
	return ret_buff;

}


// Attempt to convert a time type to a json string.
char * BP_DBMariadbConvertTimeColumnResultToJSONData(MYSQL_BIND * col_bind_info, BPDB_MARIADB_COLUMN_TYPE type)
{

	// ensure we have bind info
	if(!col_bind_info)
		return NULL;
	if(!col_bind_info->buffer)
		return NULL;

	// return buffer
	char *ret_buff = NULL;

	// set time data from buffer
	MYSQL_TIME * time_data = (MYSQL_TIME *) col_bind_info->buffer;

	// time buffer
	char time_buff[512];
	BP_ZERO_STACK(time_buff);

	// switch on type
	switch(type)
	{

		/* begin date and time data types */
		case BPDB_MARIADB_COLUMN_TYPE_DATE:
		case BPDB_MARIADB_COLUMN_TYPE_TIME:
		case BPDB_MARIADB_COLUMN_TYPE_DATETIME:
		case BPDB_MARIADB_COLUMN_TYPE_TIMESTAMP:
		case BPDB_MARIADB_COLUMN_TYPE_YEAR:
			sprintf
			(
					time_buff,
					"%.04u-%.02u-%.02u T%.02u:%.02u:%.02u",
					time_data->year,
					time_data->month,
					time_data->day,
					time_data->hour,
					time_data->minute,
					time_data->second
			);
			break;
		default:
			break;
	}

	// dupe the timebuf if necessary
	if(bpstrlen(time_buff))
	{
		ret_buff = bpstrdup(time_buff);
	}

	// return the filled out buffer (or null if badval)
	return ret_buff;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% SQL Generator Lookups %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempt to lookup sql generator.
P_BPDB_SQLGEN BP_DBLookupSQLGeneratorByUUID(P_BPDB bpdb, char *uuid)
{

	// run basic checks
	if(!bpdb)
		return NULL;
	if(!uuid)
		return NULL;
	if(!BP_StringIsUUID(uuid))
		return NULL;

	// run reg checks
	if(!bpdb->sqlgen_reg)
		return NULL;
	if(!bpdb->sqlgen_reg->hive)
		return NULL;
	if(!bpdb->sqlgen_reg->total_data_entries_in_hive)
		return NULL;

	P_BP_HTKE sqlgen_entry = BP_HashRegLookupTableEntryByDataKeyIndexAndTablePathQuery(bpdb->sqlgen_reg, "/sql_generators", uuid);

	// if we have no lookup, exit with failure
	if(!sqlgen_entry)
		return NULL;

	// if there is a lookup, but no data, we've found a corruption
	if(!sqlgen_entry->data)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBLookupSQLGeneratorByUUID encountered a sqlgen_entry with a null data parameter.  This indicates a corruption was detected.");
		return NULL;
	}

	// set sqlgen data
	P_BPDB_SQLGEN sqlgen = (P_BPDB_SQLGEN) sqlgen_entry->data;

	// return the sqlgenerator pointer so that it can be used
	return sqlgen;

}

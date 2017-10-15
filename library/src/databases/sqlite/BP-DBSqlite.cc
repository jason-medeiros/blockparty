/*
 * BP-DBSqlite.cc
 *
 *  Created on: Jul 30, 2015
 *      Author: root
 */


// include main functions
#include "../include/BP-Main.h"


// attempt to open a sqlite file
P_BP_DB_SQLITE BP_SQLiteOpen
(
	char * open_file,
	char * query_log_file
)
{

	// ensure we can open the file
	if(!open_file)
		return NULL;

	// ensure string is reasonable/printable
	if(!BP_StringIsReasonablePrintableString(open_file, bpstrlen(open_file), BP_FALSE, BP_FALSE))
		return NULL;

	// query the log file
	if(query_log_file)
	if(!BP_StringIsReasonablePrintableString(query_log_file, bpstrlen(query_log_file), BP_FALSE, BP_FALSE))
		return NULL;

	// check to make sure sqlite is thread safe
	if(!sqlite3_threadsafe())
		return NULL;

	// create hash table registry
	P_BP_HASH_TABLE_REGISTRY sqli_hreg = BP_CreateHashTableRegistry();
	if(!sqli_hreg)
		return NULL;



	// create new sqli structure
	P_BP_DB_SQLITE sqli = (P_BP_DB_SQLITE) BP_HashRegAddMemoryEntry(sqli_hreg, BP_HASH_REG_MEM_ENTRY_TYPE_GENERIC_ALLOCATION, sizeof(BP_DB_SQLITE), BPLN);

	// set the system hreg
	sqli->system_hreg = sqli_hreg;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Open %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(sqli->system_hreg);

	// open the sqlite
	int rc = sqlite3_open(open_file, &sqli->db);

	// check return values
	if(rc != SQLITE_OK)
	{
		printf("\n [!!] Cannot open sqlite db %s ", open_file);

		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(sqli->system_hreg, NULL);
	}

	// set the db file
	sqli->db_file = ht_strdup(sqli->system_hreg, open_file);


	// NOTE: Do not create query result cache here.  The result cache
	//       will be created automatically if a query has results
	//       to generate.




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Open Query Log %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// open the query log file
	if(query_log_file)
	{

		// open the logger  sqlite log db
		int log_rc = sqlite3_open(query_log_file, &sqli->log_db);

		// log database file
		sqli->log_db_file = ht_strdup(sqli->system_hreg, query_log_file);

		// set defaults to log 100000 errors to disk
		sqli->only_log_errors      = BP_TRUE;
		sqli->log_max_n            = 100000;
		sqli->log_query_parameters = BP_TRUE;

		// check return values
		if(log_rc != SQLITE_OK)
		{

			// verbosity
			printf("\n [!!] Cannot open query log sqlite db %s ", query_log_file);

			// leave critical section
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(sqli->system_hreg);
			BP_SQLiteClose(sqli);
			return NULL;

		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Populate Initial Schema %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to fetch tables
	BP_SQLiteSchemaFetchTables(sqli);

	// attempt to fetch columns for tables
	BP_SQLiteSchemaFetchColumnsForTables(sqli);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Leave Critical Section and Exit %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(sqli->system_hreg);

	// return the new sqli handle
	return sqli;

}

// close a sqlite database
BP_ERROR_T BP_SQLiteClose(P_BP_DB_SQLITE sqli)
{

	if(!sqli)
		return ERR_FAILURE;

	if(sqli->db)
		sqlite3_close(sqli->db);


	// destroy result caches
	BP_SQLiteClearBothQueryAndResultCaches(sqli);

	// destroy the schema if necessary
	if(sqli->schema)
		BP_DestroyHashTableRegistry(sqli->schema);


	// log database
	if(sqli->log_db)
		sqlite3_close(sqli->log_db);


	// destroy the system hash registry last (this destroys
	// ALL memory related to this sqli handle)
	BP_DestroyHashTableRegistry(sqli->system_hreg);


	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Schema Related Utilities %%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to fetch tables
BP_ERROR_T BP_SQLiteSchemaFetchTables(P_BP_DB_SQLITE sqli)
{


	// ensure we have a sqli pointer
	if(!sqli)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Create Reg %%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(sqli->system_hreg);


	// destroy existing schema if it exists
	if(sqli->schema)
	{
		sqli->schema = NULL;
		BP_DestroyHashTableRegistry(sqli->schema);
	}

	// if we have no schema (we shouldn't), create it here
	if(!sqli->schema)
	{

		// reassign it to a new registry
		sqli->schema = BP_CreateHashTableRegistry();

	}

	// ensure we've created our schema reg
	if(!sqli->schema)
	{
		BP_SQLiteClearBothQueryAndResultCaches(sqli);
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(sqli->system_hreg, ERR_FAILURE);
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Schema Fetch SQL %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this is the fetch table sql
	char * sql =\
	"\n SELECT * FROM                             "\
	"\n    (SELECT * FROM sqlite_master UNION ALL "\
	"\n     SELECT * FROM sqlite_temp_master)     "\
	"\n WHERE type='table'                        "\
	"\n ORDER BY name"\
	"";

	// add query
	BP_SQLiteAddQuery(sqli, BP_DB_SQLITE_QUERY_TYPE_SELECT, sql);
	BP_SQLiteRunQueries(sqli);

	// ensure we have some query results
	if(!sqli->query_result_cache)
	{


		// destroy the schema if necessary
		if(sqli->schema)
		{
			BP_DestroyHashTableRegistry(sqli->schema);
			sqli->schema = NULL;
		}

		// clear before exit
		BP_SQLiteClearBothQueryAndResultCaches(sqli);
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(sqli->system_hreg, ERR_FAILURE);

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Entries %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create the table path used for accessing our schema
	char *table_path[10] = {0};
	BP_ZERO_STACK(table_path);



	// set schema from sqli pointer
	P_BP_HASH_TABLE_KEYED_ENTRY schema = sqli->schema->hive;

	// iterate through the results
	BP_HASH_ITER_OPEN(sqli->query_result_cache->hive, result_outermost)
	{

		BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) result_outermost->data, result)
		{



			// printf("\n Got Key?: %s", result->key);

			// this counter is used for iterating through the individual tables
			size_t inner_counter = 0;

			BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) result->data, result_inner)
			{

				// ensure we have a key
				if(!result_inner->key)
					continue;

				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				// %%% Add Top Level Name Entry for Table %%%%%%%%%%%%%%%%%%%
				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

				// this will hold the preserved name entry
				P_BP_HASH_TABLE_KEYED_ENTRY preserved_name_entry = NULL;

				BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) result_inner->data, result_record)
				{

					// if we've found the table name go ahead and mark it
					if(memcmp(result_record->key, "name", bpstrlen(result_record->key))==0)
					{

						// iterate through records
						BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) result_record->data, result_individual_data_entries)
						{

							// set key
							if(memcmp(result_individual_data_entries->key, "record_data", bpstrlen(result_individual_data_entries->key))==0)
							{

								// zero out the stack before setting entries
								BP_ZERO_STACK(table_path);

								// ensure we have the key
								table_path[0] = (char *) result_individual_data_entries->data;
								table_path[1] = NULL;

								// add table to path
								BP_HashRegAddTableByPath(sqli->schema, (char **) &table_path);

								// set the name entry (used for meta addition loop below)
								preserved_name_entry = result_individual_data_entries;

							}

						} BP_HASH_ITER_CLOSE;



					}

				} BP_HASH_ITER_CLOSE;

				// name entry must be set before we continue
				if(!preserved_name_entry)
					continue;


				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				// %%% Add Individual Table Meta %%%%%%%%%%%%%%%%%%%%%%%%%%%%
				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

				BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) result_inner->data, result_record)
				{


					// Zero out the stack before setting entries
					BP_ZERO_STACK(table_path);

					// Set path from data/key pair.
					table_path[0] = (char *) preserved_name_entry->data;
					table_path[1] = result_record->key;
					table_path[2] = NULL;


					// add table to path
					BP_HashRegAddTableByPath(sqli->schema, (char **) &table_path);

					// metadata
					P_BP_DB_SQLITE_RESULT_METADATA metadata = NULL;

					// add data from individual data entries within the records (data and metadata entries)
					BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) result_record->data, result_individual_data_entries)
					{

						// check for metadata
						if(bpstrlen(result_individual_data_entries->key) == bpstrlen("record_metadata"))
						if(memcmp(result_individual_data_entries->key, "record_metadata", bpstrlen("record_metadata")) == 0)
						{

							// add data to table by path by copying over the result data into our table
							BP_HashRegAddDataToTableByPath
							(
									sqli->schema,
									(char **) &table_path,
									result_individual_data_entries->key,
									BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
									result_individual_data_entries->data,
									sizeof(BP_DB_SQLITE_RESULT_METADATA)
							);

							// set metadata
							metadata = (P_BP_DB_SQLITE_RESULT_METADATA) result_individual_data_entries->data;

						}

					} BP_HASH_ITER_CLOSE;

					if(metadata)
					BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) result_record->data, result_individual_data_entries)
					{

						// check for record data
						if(bpstrlen(result_individual_data_entries->key) == bpstrlen("record_data"))
						if(memcmp(result_individual_data_entries->key, "record_data", bpstrlen("record_data")) == 0)
						{

							// add data to table by path by copying over the result data into our table
							BP_HashRegAddDataToTableByPath
							(
									sqli->schema,
									(char **) &table_path,
									result_individual_data_entries->key,
									BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
									result_individual_data_entries->data,
									metadata->data_size
							);

						}

					} BP_HASH_ITER_CLOSE;

				} BP_HASH_ITER_CLOSE;

			} BP_HASH_ITER_CLOSE;

		} BP_HASH_ITER_CLOSE;

	} BP_HASH_ITER_CLOSE;

	// BP_SQLiteDisplay(sqli);
	// BP_HashRegDisplay(sqli->query_result_cache, BP_FALSE, BP_TRUE);
	// BP_HashRegDisplay(sqli->schema, BP_FALSE, BP_TRUE);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Leave Critical Section and Exit %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// clear the queries
	BP_SQLiteClearQueryCache(sqli);

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(sqli->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}

// attempt to fetch columns for tables
BP_ERROR_T BP_SQLiteSchemaFetchColumnsForTables(P_BP_DB_SQLITE sqli)
{


	// return indicating success
	return ERR_SUCCESS;

}

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
)
{


	// ensure we have a sqli pointer
	if(!sqli)
		return NULL;

	// ensure we have a sql string
	if(!sql)
		return NULL;

	// ensure the string is reasonable/printable
	if(!BP_StringIsReasonablePrintableString(sql, bpstrlen(sql), BP_TRUE, BP_TRUE))
		return NULL;

	// return indicating failure
	if(!query_type)
		return NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Prepare %%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(sqli->system_hreg);

	// create queries table if it doesn't already exist
	if(!sqli->queries)
	{

		// create the queries table
		sqli->queries = BP_CreateHashTableRegistry();

		// set table path to /queries/
		char * init_table_path[] = {"queries", NULL};

		// add new table
		BP_HashRegAddTableByPath
		(
				sqli->queries,
				(char **) &init_table_path
		);

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add UUID Table %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// generate a random uuid
	char uuid_buff[BP_UUID_LEN+1];
	BP_ZERO_STACK(uuid_buff);

	// generate random uuid and store in local buffer
	BP_GenRandomUUID_StoreInBuff((char *) &uuid_buff);

	// set table path to /queries/
	char * query_table_path[] = {"queries", (char *) &uuid_buff, NULL};

	// add new table for the query itself
	BP_HashRegAddTableByPath
	(
			sqli->queries,
			(char **) &query_table_path
	);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Parameters Table %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add parameters table
	char * params_table_path[] = {"queries", (char *) &uuid_buff, "params", NULL};

	// add new table for the query itself
	BP_HashRegAddTableByPath
	(
			sqli->queries,
			(char **) &params_table_path
	);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add SQL Data %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set sql path
	char * sql_table_path[] = {"queries", (char *) &uuid_buff, NULL};

	// add sql to the new table (copied over)
	BP_HashRegAddDataToTableByPath
	(
			sqli->queries,
			(char **) &sql_table_path,
			"sql",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
			(void *) sql,
			bpstrlen(sql)
	);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Last UUID %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// destroy last query uuid if its already set
	if(sqli->last_query_uuid)
	{
		ht_free(sqli->queries, sqli->last_query_uuid);
	}

	// set the last query uuid here by duplicating it within our registry
	sqli->last_query_uuid = ht_strdup(sqli->queries, (char *) &uuid_buff);

	// set the last query parameter count
	sqli->last_query_param_n = 0;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Leave Critical Section and Exit %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(sqli->system_hreg);

	// return indicating success
	return sqli->last_query_uuid;
}

// Add query parameter.
BP_ERROR_T BP_SQLiteAddQueryParam
(
	P_BP_DB_SQLITE                sqli,
	char *                        query_id,
	BP_DB_SQLITE_QUERY_PARAM_TYPE param_type,
	void *                        param_data,
	size_t                        param_data_size

)
{


	// ensure we have a sqli pointer
	if(!sqli)
		return ERR_FAILURE;

	// ensure we have queries
	if(!sqli->queries)
		return ERR_FAILURE;

	// if the query is passed as null, use last set
	if(!query_id)
		query_id = sqli->last_query_uuid;

	// ensure we have a query id
	if(!query_id)
		return ERR_FAILURE;

	// ensure the query id is reasonable
	if(!BP_StringIsReasonablePrintableString(query_id, bpstrlen(query_id), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// ensure we have parameter data
	if(!param_data || !param_data_size)
		return ERR_FAILURE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Add Param %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(sqli->system_hreg);

	// set sql path
	char * sql_table_path[] = {"queries", (char *) query_id, "params", NULL};

	// create buffer to hold number
	char num_buff[128];
	BP_ZERO_STACK(num_buff);

	// last query parameter count
	sprintf(num_buff, "%u", sqli->last_query_param_n);

	// add sql to the new table (copied over)
	BP_HashRegAddDataToTableByPath
	(
			sqli->queries,
			(char **) &sql_table_path,
			(char *)  &num_buff,
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
			(void *) NULL,
			sizeof(BP_SQLITE_QUERY_PARAM)
	);

	// set the sql table entry path
	char * sql_table_entry_path[] = {"queries", (char *) query_id, "params", (char *) &num_buff, NULL};

	// attempt to lookup entry by path
	P_BP_HASH_TABLE_KEYED_ENTRY parameter_entry = BP_HashRegLookupTableEntryByPath(sqli->queries, sql_table_entry_path);

	// if the entry wasn't added, exit here
	if(!parameter_entry)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(sqli->system_hreg, ERR_FAILURE);

	// set parameter from data lookup
	P_BP_SQLITE_QUERY_PARAM param = (P_BP_SQLITE_QUERY_PARAM) parameter_entry->data;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Data and Copy %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// allocate the parameter data
	param->data = (void *) ht_calloc(sqli->queries, param_data_size+10, 1);

	// copy in parameter data
	memcpy(param->data, param_data, param_data_size);

	// set the paraeter type
	param->type = param_type;

	// increment the parameter stack
	sqli->last_query_param_n++;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Leave Critical Section and Exit %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(sqli->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Run Queries %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Run all queries that are currently in the stack. Run all within
// a transaction.
BP_ERROR_T BP_SQLiteRunQueries
(
	P_BP_DB_SQLITE sqli
)
{

	// ensure we have a sqli pointer
	if(!sqli)
		return ERR_FAILURE;

	// ensure we have a database
	if(!sqli->db)
		return ERR_FAILURE;

	// ensure we have queries
	if(!sqli->queries)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Reset Danglers %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set last query results if necessary
	sqli->last_query_results = NULL;

	// query uuid
	sqli->last_result_query_uuid = NULL;

	// total number of last results retrieved
	sqli->last_result_retrieved_n = 0;

	// set increase the record count
	sqli->last_transaction_record_count_retrieved = 0;




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Add Param %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(sqli->system_hreg);

	// general steps:
	//    begin transaction;
	//    run queries;
	//    rollback if failed;
	//    exit transaction;


	// query table path
	char * query_table_path[] = {"queries", NULL};

	// attempt to lookup
	P_BP_HASH_TABLE_KEYED_ENTRY query_table = BP_HashRegLookupTableByPath(sqli->queries, query_table_path);

	// check to ensure we have query table
	if(!query_table)
	{
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(sqli->system_hreg, ERR_FAILURE);
	}
	if(!query_table->data)
	{
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(sqli->system_hreg, ERR_FAILURE);
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Iterate Queries and Run %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// temporary iterator values used during the HASH_ITER loop below
	P_BP_HASH_TABLE_KEYED_ENTRY entry = NULL;
	P_BP_HASH_TABLE_KEYED_ENTRY tmp   = NULL;

	// inner hash table
	P_BP_HASH_TABLE_KEYED_ENTRY query_table_inner = (P_BP_HASH_TABLE_KEYED_ENTRY) query_table->data;

	// transaction open marker
	BP_BOOL transaction_opened = BP_FALSE;

	// calculate the last entry
	P_BP_HASH_TABLE_KEYED_ENTRY last_entry = NULL;
	for
	(
		last_entry = query_table_inner;
		last_entry->hh.next;
		last_entry = (P_BP_HASH_TABLE_KEYED_ENTRY) last_entry->hh.next
	){}



	// query_table
	// for(entry = last_entry; entry; entry = (P_BP_HASH_TABLE_KEYED_ENTRY) entry->hh.prev)
	HASH_ITER(hh, query_table_inner, entry, tmp)
	{

		// printf("\n Got Key?: %s", entry->key);
		// if the transaction isn't opened, open it
		if(!transaction_opened)
		{

			// open the transaction
			sqlite3_exec(sqli->db, "BEGIN", 0,0,0);

			// ensure the transaction is opened
			transaction_opened = BP_TRUE;

		}

		// ensure we have data
		if(!entry->data)
			continue;


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Lookup Sql Entry %%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// get sql from relative table
		P_BP_HASH_TABLE_KEYED_ENTRY sql_entry = BP_HashRegLookupEntryFromHashKeyViaRelativePosition
		(
				sqli->queries,
				(P_BP_HASH_TABLE_KEYED_ENTRY) entry,
				"sql"
		);

		// ensure we have sql
		if(!sql_entry)
		{
			continue;
		}

		// set sql from query entry data
		char * sql = (char *) sql_entry->data;

		// prepare sql statement
		int rc = sqlite3_prepare_v2(sqli->db, sql, bpstrlen(sql), &sqli->stmt, NULL);
		if(rc != SQLITE_OK)
		{
			printf("\n [!!] Error: Couldn't prepare sqlite query %s - %s", sql, sqlite3_errmsg(sqli->db));
			continue;
		}

		// printf("\n SQL Prepared OK!: %s", sql);


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Lookup Parameters Table %%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// set lookup path
		char * lookup_param_table_path[] =
		{
				"queries",
				entry->key,
				"params",
				NULL
		};

		// get parameters table
		P_BP_HASH_TABLE_KEYED_ENTRY parameters_table = BP_HashRegLookupTableByPath
		(
				sqli->queries,
				(char **) &lookup_param_table_path
		);


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Iterate Parameters and Add %%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// add parameters if necessary
		if(parameters_table)
		if(parameters_table->data)
		{

			// parameter position index
			size_t param_n = 0;

			// parmeter buffer (converts to char[])
			char param_n_buff[128];

			do
			{

				// zero the parameter buffer
				BP_ZERO_STACK(param_n_buff);

				// copy in numeric index
				sprintf((char *) &param_n_buff, "%u", param_n);

				// get sql from relative table
				P_BP_HASH_TABLE_KEYED_ENTRY parm_entry = BP_HashRegLookupEntryFromHashKeyViaRelativePosition
				(
						sqli->queries,
						(P_BP_HASH_TABLE_KEYED_ENTRY) parameters_table,
						(char *) &param_n_buff
				);

				// break loop if we can't look up parameter at index
				if(!parm_entry)
					break;
				if(!parm_entry->data)
					break;

				// printf("\n Got parameter?: %p - %s", parm_entry, parm_entry->key);

				// set parameter from data
				P_BP_SQLITE_QUERY_PARAM param = (P_BP_SQLITE_QUERY_PARAM) parm_entry->data;

				// get the key as an unsigned long (used in bind)
				unsigned long key_as_ul = strtoul(parm_entry->key, NULL, 10);

				// since we start at zero in the parameter stack, and since columns in
				// sqlite start at 1, we need to increment the key_as_ul variable here
				// before it gets into the switch.
				key_as_ul++;


				// space to hold parameter as integer
				int parm_as_int     = 0;

				// space to hold parameter as a double (float)
				double parm_as_double = 0;

				// printf("\n Got parameter type?: %u", param->type);

				// bind parameter based on types
				switch(param->type)
				{

					case BP_DB_SQLITE_QUERY_PARAM_TYPE_UNSET:
						break;

					case BP_DB_SQLITE_QUERY_PARAM_TYPE_NULL:
						sqlite3_bind_null(sqli->stmt, key_as_ul);
						break;

					case BP_DB_SQLITE_QUERY_PARAM_TYPE_INTEGER:
						memcpy(&parm_as_int, param->data, sizeof(parm_as_int));
						sqlite3_bind_int(sqli->stmt, key_as_ul, parm_as_int);
						break;

					case BP_DB_SQLITE_QUERY_PARAM_TYPE_REAL:
						memcpy(&parm_as_double, param->data, sizeof(parm_as_double));
						sqlite3_bind_double(sqli->stmt, key_as_ul, parm_as_double);
						break;

					case BP_DB_SQLITE_QUERY_PARAM_TYPE_TEXT:
						// printf("\n Adding text parameter?: %u - %s", key_as_ul, (char *) param->data);
						sqlite3_bind_text(sqli->stmt, key_as_ul,(char *) param->data, bpstrlen((char *) param->data), SQLITE_STATIC);
						break;

					case BP_DB_SQLITE_QUERY_PARAM_TYPE_BLOB:
						sqlite3_bind_blob(sqli->stmt, key_as_ul, param->data, param->data_size, SQLITE_STATIC);
						break;

					default:
						break;

				}

				// increment the position index
				param_n++;

			} while(1);


		}


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Gather Query Results %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// step the statement
		size_t result_idx = 0;
		while(BP_SQLiteFetchResult(sqli, entry->key, result_idx))
		{
			// printf("\n Adding result");

			// increment the index
			result_idx++;

		}

		// set query results path
		char * query_results_iter_path[] =
		{
			"results",
			entry->key,
			(char *) &result_idx,
			NULL
		};

		// lookup table after entry and set last results
		P_BP_HASH_TABLE_KEYED_ENTRY last_query_result_lookup = BP_HashRegLookupTableByPath
		(
				sqli->query_result_cache,
				(char **) &query_results_iter_path
		);


		// set last query results if necessary
		sqli->last_query_results = last_query_result_lookup;

		// query uuid
		sqli->last_result_query_uuid = entry->key;

		// total number of last results retrieved
		sqli->last_result_retrieved_n = result_idx;

		// set increase the record count
		sqli->last_transaction_record_count_retrieved += result_idx;


		// finalize (write)
		sqlite3_finalize(sqli->stmt);

	}


	// if the transaction was opened, close it here
	if(transaction_opened)
	{
		sqlite3_exec(sqli->db, "COMMIT", 0,0,0);
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Leave Critical Section and Exit %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(sqli->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Clear Old Queries/Results/Both %%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// clear all queries within sqli->queries registry.
BP_ERROR_T BP_SQLiteClearQueryCache
(
	P_BP_DB_SQLITE sqli
)
{

	// ensure we have a sqli pointer
	if(!sqli)
		return ERR_FAILURE;
	if(!sqli->queries)
		return ERR_FAILURE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Add Param %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(sqli->system_hreg);

	// destroy queries registry (frees all memory associated)
	BP_DestroyHashTableRegistry(sqli->queries);

	// nullify
	sqli->queries = NULL;

	// reset all "last" indicators
	sqli->last_query_generated_results = BP_FALSE;
	sqli->last_query_param_n           = 0;
	sqli->last_query_ran_ok            = BP_FALSE;
	sqli->last_query_reported_error    = BP_FALSE;
	sqli->last_query_uuid              = NULL;
	sqli->last_result_query_uuid       = NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Leave Critical Section and Exit %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(sqli->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}


// clear all results within the sqli->query_result_cache
BP_ERROR_T BP_SQLiteClearResultCache
(
	P_BP_DB_SQLITE sqli
)
{

	// ensure we have a sqli pointer
	if(!sqli)
		return ERR_FAILURE;
	if(!sqli->query_result_cache)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Add Param %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(sqli->system_hreg);

	// destroy the registry
	BP_DestroyHashTableRegistry(sqli->query_result_cache);

	// nullify cache after it's done
	sqli->query_result_cache = NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Leave Critical Section and Exit %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(sqli->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}


// Clears both caches (just calls the above two routines)
BP_ERROR_T BP_SQLiteClearBothQueryAndResultCaches
(
	P_BP_DB_SQLITE sqli
)
{

	// ensure we have an interface handle
	if(!sqli)
		return ERR_FAILURE;

	// clear cache (has it's own locks)
	BP_SQLiteClearResultCache(sqli);

	// clear queries (has it's own locks)
	BP_SQLiteClearQueryCache(sqli);

	// always return success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Results Cache %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// fetch a result
BP_ERROR_T BP_SQLiteFetchResult
(
	P_BP_DB_SQLITE sqli,
	char *         query_id,
	size_t         result_idx

)
{

	// ensure we have a sqli pointer
	if(!sqli)
		return ERR_FAILURE;
	if(!sqli->system_hreg)
		return ERR_FAILURE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Add Param %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(sqli->system_hreg);

	// make one step forward
	int set_result = sqlite3_step(sqli->stmt);
	if(set_result != SQLITE_ROW)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(sqli->system_hreg, ERR_FAILURE);

	// attempt to get column count
	int col_n = sqlite3_column_count(sqli->stmt);
	if(!col_n)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(sqli->system_hreg, ERR_FAILURE);


	// iterate through columns
	int iter_n = 0;
	for
	(
		;
		iter_n < col_n;
		iter_n++
	)
	{

		// set type
		BP_DB_SQLITE_QUERY_PARAM_TYPE bp_type = BP_DB_SQLITE_QUERY_PARAM_TYPE_UNSET;

		// get column type
		int col_type = sqlite3_column_type(sqli->stmt, iter_n);

		// if the data is int
		int    val_as_int = 0;

		// if the data is double
		double val_as_double = 0;

		// null pointer
		void * null_ptr   = NULL;

		// data and data size
		void * data   = NULL;
		size_t data_n = 0;

		switch(col_type)
		{

			case SQLITE_INTEGER:
				bp_type = BP_DB_SQLITE_QUERY_PARAM_TYPE_INTEGER;
				val_as_int = sqlite3_column_int(sqli->stmt, iter_n);
				data       = (void *) &val_as_int;
				data_n     = sizeof(int);
				break;

			case SQLITE_FLOAT:
				bp_type = BP_DB_SQLITE_QUERY_PARAM_TYPE_REAL;
				val_as_double = sqlite3_column_double(sqli->stmt, iter_n);
				data    = (void *) &val_as_double;
				data_n  = sqlite3_column_bytes(sqli->stmt, iter_n);
				break;

			case SQLITE_BLOB:
				bp_type = BP_DB_SQLITE_QUERY_PARAM_TYPE_BLOB;
				data    = (void *) sqlite3_column_blob(sqli->stmt, iter_n);
				data_n  = sqlite3_column_bytes(sqli->stmt, iter_n);
				break;

			case SQLITE_NULL:
				bp_type = BP_DB_SQLITE_QUERY_PARAM_TYPE_NULL;
				data   = &null_ptr;
				data_n = sizeof(null_ptr);
				break;

			case SQLITE_TEXT:
				bp_type = BP_DB_SQLITE_QUERY_PARAM_TYPE_TEXT;
				data    = (void *) sqlite3_column_blob(sqli->stmt, iter_n);
				data_n  = sqlite3_column_bytes(sqli->stmt, iter_n);
				break;

			default:
				break;

		}

		// get the column name
		char * col_name = (char *) sqlite3_column_name(sqli->stmt, iter_n);

		// add new entry
		// printf("\n Column type?: %i - %s - %s", col_type, query_id, col_name);

		// add column entry
		BP_SQLiteAddResultColumnEntry
		(
			sqli,
			query_id,
			result_idx,
			col_name,
			iter_n,
			bp_type,
			data,
			data_n
		);

	}

	// increment the last result count
	sqli->last_result_retrieved_n++;

	// set query uuid
	sqli->last_result_query_uuid = query_id;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Leave Critical Section and Exit %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(sqli->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}

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
)
{

	// ensure we have a sqli pointer
	if(!sqli)
		return ERR_FAILURE;
	if(!sqli->system_hreg)
		return ERR_FAILURE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Add Param %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(sqli->system_hreg);

	// create result cache if to doesn't exist
	if(!sqli->query_result_cache)
	{

		// create a query cache
		sqli->query_result_cache = BP_CreateHashTableRegistry();

		// set the results path
		char * results_path[] = {"results", NULL};

		// add results table
		BP_HashRegAddTableByPath(sqli->query_result_cache, (char **) &results_path);

	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt Table Lookup %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set query results path
	char * query_results_path[] = {"results", query_result_uuid, NULL};

	// attempt to add the table (fails graceful if already exists)
	BP_HashRegAddTableByPath(sqli->query_result_cache, (char **) &query_results_path);


	// attempt lookup new (or existing)
	P_BP_HASH_TABLE_KEYED_ENTRY lookup_entry = BP_HashRegLookupTableByPath(sqli->query_result_cache, (char **) &query_results_path);

	// ensure we can lookup our entry
	if(!lookup_entry)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(sqli->system_hreg, ERR_FAILURE);

	// set records ptr
	P_BP_HASH_TABLE_KEYED_ENTRY result_records = (P_BP_HASH_TABLE_KEYED_ENTRY) lookup_entry->data;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt Table Record Addition %%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// get the index from the count
	char idx_buff[100];
	sprintf((char *) &idx_buff, "%u", result_idx);

	// set query results path
	char * query_results_iter_path[] =
	{
		"results",
		query_result_uuid,
		(char *) &idx_buff,
		NULL
	};

	// attempt to add the table (fails graceful if already exists)
	BP_HashRegAddTableByPath
	(
			sqli->query_result_cache,
			(char **) &query_results_iter_path
	);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Inner Table For Record %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// note: inner record contains data and metadata.

	// set query results path
	char * query_results_inner_iter_path[] =
	{
		"results",
		query_result_uuid,
		(char *) &idx_buff,
		col_name,
		NULL
	};

	// attempt to add the table (fails graceful if already exists)
	BP_HashRegAddTableByPath
	(
			sqli->query_result_cache,
			(char **) &query_results_inner_iter_path
	);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Inner Table %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// add data to table by path
	BP_HashRegAddDataToTableByPath
	(
			sqli->query_result_cache,
			(char **) &query_results_inner_iter_path,
			"record_data",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
			data,
			data_size
	);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Metadata to Record %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add metadata to table
	BP_HashRegAddDataToTableByPath
	(
			sqli->query_result_cache,
			(char **) &query_results_inner_iter_path,
			"record_metadata",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
			NULL,
			sizeof(BP_DB_SQLITE_RESULT_METADATA)
	);

	// set query results path
	char * query_results_metadata_iter_lookup_path[] =
	{
			"results",
			query_result_uuid,
			(char *) &idx_buff,
			col_name,
			"record_metadata",
			NULL
	};

	// lookup the record metadata
	P_BP_HASH_TABLE_KEYED_ENTRY record_metadata_entry = BP_HashRegLookupTableEntryByPath(sqli->query_result_cache, (char **) &query_results_metadata_iter_lookup_path);

	// create typed reference to record metadata
	P_BP_DB_SQLITE_RESULT_METADATA record_metadata = (P_BP_DB_SQLITE_RESULT_METADATA) record_metadata_entry->data;

	// if the lookup was ok, go ahead and fill in values
	if(record_metadata)
	{
		record_metadata->result_idx = result_idx;
		record_metadata->col_idx    = col_idx;
		record_metadata->type       = type;
		record_metadata->data_size  = data_size;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Leave Critical Section and Exit %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(sqli->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Conversion Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This returns a bpcalloced string representation of a query parameter.  Result
// must be free'd.  This is mainly used when displaying results.
char * BP_SQLiteConvertParameterToCStringForDisplay(P_BP_SQLITE_QUERY_PARAM param)
{

	// ensure we have a parameter
	if(!param)
		return NULL;

	// return value
	char *ret_val = NULL;

	// create a float value (8byte ieee)
	float tmp_float = 0;

	// switch on parameter type
	switch(param->type)
	{

		case BP_DB_SQLITE_QUERY_PARAM_TYPE_UNSET:
			break;

		case BP_DB_SQLITE_QUERY_PARAM_TYPE_NULL:
			ret_val = bpstrdup("NULL");
			break;

		case BP_DB_SQLITE_QUERY_PARAM_TYPE_INTEGER:
			ret_val = (char *) bpcalloc(40, 1);
			sprintf(ret_val, "%i", param->data);
			break;

		case BP_DB_SQLITE_QUERY_PARAM_TYPE_REAL:
			memcpy(&tmp_float, param->data, sizeof(float));
			break;

		case BP_DB_SQLITE_QUERY_PARAM_TYPE_TEXT:
			ret_val = bpstrdup((char *) param->data);
			break;

		case BP_DB_SQLITE_QUERY_PARAM_TYPE_BLOB:
			ret_val = BP_BufferToHexString((char *) param->data, param->data_size);
			break;

		default:
			break;

	}

	// return null if no conversion can occur
	return ret_val;

}

// This routine is used to convert metadata and raw data into a human readable
// c string suitable for display.
char * BP_SQLiteConvertResultDataToCStringForDisplay
(
		P_BP_DB_SQLITE_RESULT_METADATA record_metadata,
		void * data
)
{

	// ensure we have metadata
	if(!record_metadata)
		return NULL;
	if(!data)
		return NULL;

	// check the retval
	char *ret_val = NULL;

	// used for data to int conversion
	int data_as_int = 0;

	// create a float value (8byte ieee)
	double tmp_double = 0;

	// switch on record type
	switch(record_metadata->type)
	{

		case BP_DB_SQLITE_QUERY_PARAM_TYPE_UNSET:
			ret_val = bpstrdup("BP_DB_SQLITE_QUERY_PARAM_TYPE_UNSET");
			break;

		case BP_DB_SQLITE_QUERY_PARAM_TYPE_NULL:
			ret_val = bpstrdup("NULL");
			break;

		case BP_DB_SQLITE_QUERY_PARAM_TYPE_INTEGER:
			ret_val = (char *) bpcalloc(40, 1);
			memcpy(&data_as_int, data, sizeof(int));
			sprintf(ret_val, "%i", data_as_int);
			break;

		case BP_DB_SQLITE_QUERY_PARAM_TYPE_REAL:
			memcpy(&tmp_double, data, sizeof(double));
			break;

		case BP_DB_SQLITE_QUERY_PARAM_TYPE_TEXT:
			ret_val = bpstrndup((char *) data, record_metadata->data_size);
			break;

		case BP_DB_SQLITE_QUERY_PARAM_TYPE_BLOB:
			ret_val = BP_BufferToHexString((char *) data, record_metadata->data_size);
			break;

		default:
			ret_val = bpstrdup("BADVAL");
			break;

	}

	// return null if no conversion can occur
	return ret_val;
}

/*
 * BP-DBSQLSGerators.cc
 *
 *  Created on: Nov 19, 2015
 *      Author: root
 */

// include BP main
#include "../../include/BP-Main.h"




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%  Open or Close Syntax Generation Session %%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// open a syntax generator session
P_BPDB_SQLGEN BP_DBSGOpen(P_BPDB bpdb)
{

	// ensure we have a bpdb to work with
	if(!bpdb)
		return NULL;
	if(!bpdb->system_hreg)
		return NULL;
	if(!bpdb->system_hreg->hive)
		return NULL;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create SQLGen Registry within BPDB if Unset %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create reg and add top table
	if(!bpdb->sqlgen_reg)
	{
		bpdb->sqlgen_reg = BP_CreateHashTableRegistry();
		BP_HashRegAddTableByPathQuery(bpdb->sqlgen_reg, "/sql_generators");
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Registry %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new hash table registry for system operations
	P_BP_HASH_TABLE_REGISTRY system_hreg = BP_CreateHashTableRegistry();
	if(!system_hreg)
		return NULL;

	// create syntax gen from system reg (calloc clears all null initializers to correct values)
	P_BPDB_SQLGEN syngen = (P_BPDB_SQLGEN) ht_calloc(system_hreg, sizeof(BPDB_SQLGEN), 1);
	if(!syngen)
	{
		BP_DestroyHashTableRegistry(system_hreg);
		return NULL;
	}

	// set system registry from newly created
	syngen->system_hreg = system_hreg;

	// set bpdb handle
	syngen->bpdb = bpdb;

	// create tmp space for uuid
	char tmp_uuid[BP_UUID_LEN+4];
	BP_ZERO_STACK(tmp_uuid);

	// generate a random uuid and store it in the buffer
	BP_GenRandomUUID_StoreInBuff((char *) &tmp_uuid);

	// duplicate uuid
	syngen->uuid = ht_strdup(syngen->system_hreg, (char *) &tmp_uuid);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Checkout Connection for Use %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt checkout
	P_BP_MYSQL_SESSION mariadbi = BP_DBCheckOutAvailableMariadbConnection(bpdb);
	if(!mariadbi)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBSGOpen attempted to call BP_DBCheckOutAvailableMariadbConnection but was unable to checkout a connection.");
		BP_DBSGClose(syngen);
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, NULL);
	}

	if(!mariadbi->conn)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBSGOpen attempted to call BP_DBCheckOutAvailableMariadbConnection, and was able to checkout a connection, but that connection has a NULL connection pointer (meaning the connection is invalid or has gone away/been destroyed).");
		BP_DBSGClose(syngen);
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, NULL);
	}

	if(!mariadbi->connected)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBSGOpen attempted to call BP_DBCheckOutAvailableMariadbConnection, and was able to checkout a connection, but the connection is not marked as connected.");
		BP_DBSGClose(syngen);
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, NULL);
	}

	if(mariadbi->server_gone_away)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBSGOpen attempted to call BP_DBCheckOutAvailableMariadbConnection, and was able to checkout a connection, but that connection is marked as having a server which has gone-away.");
		BP_DBSGClose(syngen);
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, NULL);
	}

	if(mariadbi->server_lost_connection)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBSGOpen attempted to call BP_DBCheckOutAvailableMariadbConnection, and was able to checkout a connection, but that connection is marked as having a server which has lost connection.");
		BP_DBSGClose(syngen);
		BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, NULL);
	}

	// printf("\n [+] dbg: Checked out mariadb entry %p: %s", mariadbi, mariadbi->uuid);

	// set the mariadb interface here
	syngen->mariadb = mariadbi;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Syngen to Registry %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create reg
	if(bpdb->sqlgen_reg)
	{

		// add syntax generator to table by uuid
		BP_HashRegAddDataToTableByPathQuery
		(
			bpdb->sqlgen_reg,
			"/sql_generators",
			syngen->uuid,
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_C_DATASTRUCTURE_EXTERNALLY_MANAGED,
			(void *) syngen,
			sizeof(P_BPDB_SQLGEN)
		);

		// create temporary lookup path
		char *tmp_lookup_path[] =
		{
			"sql_generators",
			syngen->uuid,
			NULL
		};

		// attempt to lookup the data
		P_BP_HTKE sqlgen_data_lookup = BP_HashRegLookupTableEntryByPath(bpdb->sqlgen_reg, (BP_HASH_TABLE_TRAVERSAL_PATH) &tmp_lookup_path);
		if(!sqlgen_data_lookup)
		{
			BP_DBAddErrorMsg(syngen->bpdb, "Failed to lookup sqlgen data pointer within the bpdb->sqlgen_reg registry.  Likely out of memory.");
			BP_DBSGClose(syngen);
			BPDB_LEAVE_CRITICAL_SECTION_CHECKIN_AND_RETURN(mariadbi, NULL);
		}

		// if we've gotten a lookup ok, things were added ok, continue to exit

	}

	// return the syntax generator
	return syngen;

}

// close the syntax generator
BP_ERROR_T BP_DBSGClose(P_BPDB_SQLGEN syngen)
{

	// ensure we have a syntax generator
	if(!syngen)
		return ERR_FAILURE;
	if(!syngen->system_hreg)
		return ERR_FAILURE;


	// enter critical section
	BPDB_SYNGEN_ENTER_CRITICAL_SECTION;

	// checkin mariadb handle if necessary
	if(syngen->mariadb)
	{

		// destroy buffer auto allocator
		if(syngen->mariadb->result_buffer_auto_allocator)
		{
			BP_DestroyLinkL(syngen->mariadb->result_buffer_auto_allocator, BP_TRUE);
			syngen->mariadb->result_buffer_auto_allocator = NULL;
		}

		// close the statement
		if(syngen->mariadb->stmt)
		{
			BP_MySQLCloseStatement(syngen->mariadb->stmt, syngen->mariadb);
			syngen->mariadb->stmt = NULL;
		}

		// checkin the mariadb connection
		BP_DBCheckInMariadbConnection(syngen->bpdb, syngen->mariadb);

	}

	// overwrite just in case
	syngen->mariadb = NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Dangling Information %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// destroy generators if necessary
	BP_DBSyntaxGenSelectInfoDestroy(syngen);

	// leave critical section
	BPDB_SYNGEN_LEAVE_CRITICAL_SECTION;

	// destroy the hash table registry
	BP_DestroyHashTableRegistry(syngen->system_hreg);


	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Query Modification Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Generates a CREATE TABLE sql query from the table specified by table_name.  Table must
// be found within the associated bpdb handle.
//
// finalizes query: yes
//
BP_ERROR_T BP_DBSGCreateTable(P_BPDB_SQLGEN syngen, char *table_name)
{
	return ERR_FAILURE; // todo
}


// Generates a select query.
BP_ERROR_T BP_DBSGSelectFrom
(
	P_BPDB_SQLGEN        syngen,
	P_BPDB_SQLGEN_SELECT select_info
)
{



	// ensure we have a syntax generator
	if(!syngen)
		return ERR_FAILURE;

	// if we don't have a database exit with failure
	if(!syngen->bpdb)
		return ERR_FAILURE;

	// ensure we have a checked out mariadb connection
	if(!syngen->mariadb)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSGSelectFrom syngen->mariadb parameter is NULL.");
		return ERR_FAILURE;
	}

	printf("\n gets here ok?!");

	// ensure we have select info to work with
	if(!syngen->select_info)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSGSelectFrom select_info parameter was NULL.");
		return ERR_FAILURE;
	}

	// ensure we have query parts
	if(!syngen->select_info->select_query_parts_hreg)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSGSelectFrom select_info->select_query_parts_hreg was null.");
		return ERR_FAILURE;
	}


	// !!!!!!!!!!!!!!! BEGIN FILLING OUT QUERY STRUCTURES FROM HREG !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!!!!!!!!!!!!!! BEGIN FILLING OUT QUERY STRUCTURES FROM HREG !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!!!!!!!!!!!!!! BEGIN FILLING OUT QUERY STRUCTURES FROM HREG !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!!!!!!!!!!!!!! BEGIN FILLING OUT QUERY STRUCTURES FROM HREG !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Retrieve Query Parts From Hreg %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// retrieve query parts
	P_BP_HTKE field_names                    = BP_HashRegLookupTableByPathQuery(syngen->select_info->select_query_parts_hreg, "/field_names");
	P_BP_HTKE primary_select_from_table_name = BP_HashRegLookupTableByPathQuery(syngen->select_info->select_query_parts_hreg, "/primary_select_from_table_name");
	P_BP_HTKE join_clauses                   = BP_HashRegLookupTableByPathQuery(syngen->select_info->select_query_parts_hreg, "/join_clauses");
	P_BP_HTKE where_clauses                  = BP_HashRegLookupTableByPathQuery(syngen->select_info->select_query_parts_hreg, "/where_clauses");
	P_BP_HTKE group_by_clauses               = BP_HashRegLookupTableByPathQuery(syngen->select_info->select_query_parts_hreg, "/group_by_clauses");
	P_BP_HTKE limit_clause                   = BP_HashRegLookupTableByPathQuery(syngen->select_info->select_query_parts_hreg, "/limit_clause");
	P_BP_HTKE order_clause                   = BP_HashRegLookupTableByPathQuery(syngen->select_info->select_query_parts_hreg, "/order_clause");

	printf("\n [+] Getting here woo2!");
	printf("\n\t field_names:                    %p", field_names);
	printf("\n\t primary_select_from_table_name: %p", primary_select_from_table_name);
	printf("\n\t where_clauses:                  %p", where_clauses);
	printf("\n\t group_by_clauses:               %p", group_by_clauses);
	printf("\n\t limit_clause:                   %p", limit_clause);
	printf("\n\t order_clause:                   %p", order_clause);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Table Name %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure we have a data pointer
	if(primary_select_from_table_name->data)
	{

		// set first keyed entry as data
		P_BP_HTKE tmp_table = (P_BP_HTKE) primary_select_from_table_name->data;

		if(tmp_table)
			syngen->select_info->table_name = (char *) tmp_table->data;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Field Info %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create select name builder inkl
	P_BP_LINKL select_fieldname_builder_linkl = BP_CreateLinkL(".", BP_TRUE);

	// create field list
	if(field_names)
	{

		if(field_names->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{

			// iterate trhough field names
			BP_HASH_ITER_OPEN((P_BP_HTKE) field_names->data, field_name)
			{

				// add to tailqueue
				tq_strdup(select_fieldname_builder_linkl, (char *) field_name->data, BPLN);

			}
			BP_HASH_ITER_CLOSE;

		}

	}

	// convert tailqueue to field names
	syngen->select_info->field_names = (char **) BP_LinkLToBPCallocPointerArray(select_fieldname_builder_linkl);

	// destroy linkl after conversion (pointers are all bpcalloced now)
	BP_DestroyLinkL(select_fieldname_builder_linkl, BP_TRUE);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Join Info (needed for building tables) %%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure values are nullified
	syngen->select_info->join_on         = NULL;
	syngen->select_info->join_on_entry_n = 0;

	// create field list
	if(join_clauses)
	{

		// ensure the type is a iterable hash table
		if(join_clauses->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{

			// iterate trhough field names
			BP_HASH_ITER_OPEN((P_BP_HTKE) join_clauses->data, join_clause)
			{

				// only process join clauses if they're tables
				if(join_clause->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
					continue;

				// reallocate space for our joins
				syngen->select_info->join_on = (P_BPDB_SELECT_JOIN) ht_realloc(syngen->system_hreg, (void *) syngen->select_info->join_on, sizeof(BPDB_SELECT_JOIN) * (syngen->select_info->join_on_entry_n + 1));

				// set the current join element
				P_BPDB_SELECT_JOIN curr_join_elem = &syngen->select_info->join_on[syngen->select_info->join_on_entry_n];

				// iterate through the join clauses
				BP_HASH_ITER_OPEN((P_BP_HTKE) join_clause->data, curr_join)
				{

					// process table name
					if(BP_StringsMatchAsReasonableStrings(curr_join->key, "table_name") == BP_TRUE)
					{
						curr_join_elem->table_name = (char *) curr_join->data;
					}

					// process front join
					if(BP_StringsMatchAsReasonableStrings(curr_join->key, "front_join") == BP_TRUE)
					{
						curr_join_elem->front_join = (char *) curr_join->data;
					}

					// process comparison operator
					if(BP_StringsMatchAsReasonableStrings(curr_join->key, "comparison_operator") == BP_TRUE)
					{
						curr_join_elem->comparison_operator = (char *) curr_join->data;
					}

					// process tail join
					if(BP_StringsMatchAsReasonableStrings(curr_join->key, "tail_join") == BP_TRUE)
					{
						curr_join_elem->tail_join = (char *) curr_join->data;
					}

					// process join type
					if(BP_StringsMatchAsReasonableStrings(curr_join->key, "type") == BP_TRUE)
					{
						size_t tmp_type = 0;
						memcpy(&tmp_type, curr_join->data, sizeof(size_t));
						curr_join_elem->join_type = (BPDB_SELECT_JOIN_TYPE) tmp_type;
					}

				}
				BP_HASH_ITER_CLOSE;

				// increment the entry count
				syngen->select_info->join_on_entry_n++;

			}
			BP_HASH_ITER_CLOSE;

		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Where Clauses %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure where classes are null
	syngen->select_info->select_where         = NULL;
	syngen->select_info->select_where_entry_n = 0;

	if(where_clauses)
	{

		// ensure the type is a iterable hash table
		if(where_clauses->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{

			// iterate through where clauses
			BP_HASH_ITER_OPEN((P_BP_HTKE) where_clauses->data, where_clause)
			{

				// only process where clauses if they're tables
				if(where_clause->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
					continue;

				// reallocate space for our where clauses
				syngen->select_info->select_where = (P_BPDB_SELECT_WHERE) ht_realloc(syngen->system_hreg, (void *) syngen->select_info->select_where, sizeof(BPDB_SELECT_WHERE) * (syngen->select_info->select_where_entry_n + 1));

				// set the current where element
				P_BPDB_SELECT_WHERE curr_where_elem = &syngen->select_info->select_where[syngen->select_info->select_where_entry_n];


				// iterate through the join clauses
				BP_HASH_ITER_OPEN((P_BP_HTKE) where_clause->data, curr_where_clause_entry)
				{

					// set conjunction where necessary (first element should have null conjunction)
					if(BP_StringsMatchAsReasonableStrings(curr_where_clause_entry->key, "conjunction") == BP_TRUE)
					{
						curr_where_elem->conjunction = (char *) curr_where_clause_entry->data;
					}

					// process table name
					if(BP_StringsMatchAsReasonableStrings(curr_where_clause_entry->key, "field_name") == BP_TRUE)
					{
						curr_where_elem->field_name = (char *) curr_where_clause_entry->data;
					}

					// process front join
					if(BP_StringsMatchAsReasonableStrings(curr_where_clause_entry->key, "limiter") == BP_TRUE)
					{
						curr_where_elem->limiter = (char *) curr_where_clause_entry->data;
					}

					// process comparison operator
					if(BP_StringsMatchAsReasonableStrings(curr_where_clause_entry->key, "value") == BP_TRUE)
					{
						curr_where_elem->value = (char *) curr_where_clause_entry->data;
					}

					// process comparison operator
					if(BP_StringsMatchAsReasonableStrings(curr_where_clause_entry->key, "value_type") == BP_TRUE)
					{

						size_t * tmp_value_type = (size_t *) curr_where_clause_entry->data;
						curr_where_elem->value_type = (BPDB_SELECT_WHERE_VALUE_TYPE) *tmp_value_type;
					}

					// process comparison operator
					if(BP_StringsMatchAsReasonableStrings(curr_where_clause_entry->key, "value_len") == BP_TRUE)
					{
						size_t * tmp_value_len = (size_t *) curr_where_clause_entry->data;
						curr_where_elem->value_len = (BPDB_SELECT_WHERE_VALUE_TYPE) *tmp_value_len;
					}


				}
				BP_HASH_ITER_CLOSE;


				// increment the entry count
				syngen->select_info->select_where_entry_n++;

			}
			BP_HASH_ITER_CLOSE;

		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Group By Clauses %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// group by
	syngen->select_info->group_by = NULL;

	// set group by clauses
	if(group_by_clauses)
	{

		// set tmp group by
		P_BP_HTKE tmp_group_by = (P_BP_HTKE) group_by_clauses->data;

		if(tmp_group_by)
		{

			// allocate space for the group_by clause
			syngen->select_info->group_by = (P_BPDB_SELECT_GROUP_BY) ht_calloc(syngen->system_hreg, sizeof(BPDB_SELECT_GROUP_BY), 1);

			// set group by clauses
			syngen->select_info->group_by->field = (char *) tmp_group_by->data;

		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// Add Order By Clause %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set order clause to null
	syngen->select_info->order_by = NULL;

	// ensure we have an order clause
	if(order_clause)
	{

		if(order_clause->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{

			// allocate space for the order by structure
			syngen->select_info->order_by = (P_BPDB_SELECT_ORDER_BY) ht_calloc(syngen->system_hreg, sizeof(BPDB_SELECT_ORDER_BY), 1);

			// iterate through clause and fill out values
			BP_HASH_ITER_OPEN( (P_BP_HTKE) order_clause->data, order_clause_entry)
			{


				if(BP_StringsMatchAsReasonableStrings(order_clause_entry->key, "field"))
				{
					syngen->select_info->order_by->field = (char *) order_clause_entry->data;
				}
				if(BP_StringsMatchAsReasonableStrings(order_clause_entry->key, "sort_designator"))
				{
					syngen->select_info->order_by->sort_designator = (char *) order_clause_entry->data;
				}

			}
			BP_HASH_ITER_CLOSE;


			// if neither part was set, destroy order_by
			if(!syngen->select_info->order_by->field || !syngen->select_info->order_by->sort_designator)
			{

				// nullify field and sort_designator
				syngen->select_info->order_by->field           = NULL;
				syngen->select_info->order_by->sort_designator = NULL;

				// destroy order by
				syngen->select_info->order_by = NULL;

			}

		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// Add Limit Clause %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set limit clause to null
	syngen->select_info->limits = NULL;

	// ensure we have an limit clause
	if(limit_clause)
	{

		// allocate space for the limits structure
		syngen->select_info->limits = (P_BPDB_SELECT_LIMITS) ht_calloc(syngen->system_hreg, sizeof(BPDB_SELECT_LIMITS), 1);

		// iterate through clause and fill out values
		BP_HASH_ITER_OPEN( (P_BP_HTKE) limit_clause->data, limit_clause_entry)
		{


			if(BP_StringsMatchAsReasonableStrings(limit_clause_entry->key, "limit_low"))
			{

				// set limit and copy in
				size_t limit_low = 0;
				memcpy(&limit_low, limit_clause_entry->data, sizeof(size_t));

				// set limit low here
				syngen->select_info->limits->limit_low = limit_low;

			}

			if(BP_StringsMatchAsReasonableStrings(limit_clause_entry->key, "limit_high"))
			{

				// set limit and copy in
				size_t limit_high = 0;
				memcpy(&limit_high, limit_clause_entry->data, sizeof(size_t));

				// set limit high here
				syngen->select_info->limits->limit_high = limit_high;

			}



		}
		BP_HASH_ITER_CLOSE;

	}


	// !!!!!!!!!!!!!!! BEGIN BUILDING QUERY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!!!!!!!!!!!!!! BEGIN BUILDING QUERY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!!!!!!!!!!!!!! BEGIN BUILDING QUERY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!!!!!!!!!!!!!! BEGIN BUILDING QUERY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!!!!!!!!!!!!!! BEGIN BUILDING QUERY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Basic Checks and Generate Metadata %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// run checks and generate table/field_name lists (stored within select info)
	if(!BP_DBCheckSelectInfoAndGenerateMetadataLists(syngen, syngen->select_info))
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSGSelectFrom tried to call BP_DBCheckSelectInfoAndGenerateMetadataLists but it failed");
		return ERR_FAILURE;
	}





	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check Metadata Against BPDB Schema %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// These two checks attempt to check all items against
	// entities within the BPDB schema.

	// run field name check
	if(!BP_DBCheckFieldNamesAgainstSchema(syngen, syngen->select_info->field_name_list))
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSGSelectFrom tried to call BP_DBCheckFieldNamesAgainstSchema to verify field names.  Verification has failed.");
		return ERR_FAILURE;
	}

	// run table name check
	if(!BP_DBCheckTableNamesAgainstSchema(syngen, syngen->select_info->table_name_list))
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSGSelectFrom tried to call BP_DBCheckTableNamesAgainstSchema to verify table names.  Verification has failed.");
		return ERR_FAILURE;
	}


	// create a linkl to hold our query
	P_BP_LINKL query_string_gen = BP_CreateLinkL("select query generator", BP_TRUE);

	// ensure the linkl was created successfully
	if(!query_string_gen)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSGSelectFrom failed to create new select query generator (tail queue linkl).  Likely out of memory.  Catestropic.");
		return ERR_FAILURE;
	}

	// add select statement to start query
	tq_strdup(query_string_gen, "SELECT", BPLN);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Process Field Names %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set select field name iter
	size_t select_field_name_iter = 0;

	// field names
	if(field_names)
	{

		if(field_names->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{

			// iterate trhough field names
			BP_HASH_ITER_OPEN((P_BP_HTKE) field_names->data, field_name)
			{

				if
				(
					BP_DBAddSelectResultToSelectInfo
					(
						syngen,
						syngen->select_info,
						(char *) field_name->data
					) != ERR_SUCCESS
				)
				{

					// destroy the query generator
					BP_DestroyLinkL(query_string_gen, BP_TRUE);

					// create error string
					char tmp_err[256];
					BP_ZERO_STACK(tmp_err);
					sprintf((char *) &tmp_err, "BP_DBSGSelectFrom failed to BP_DBAddSelectResultToSelectInfo on field name idx: %zu", select_field_name_iter);
					BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);

					// return indicating failure
					return ERR_FAILURE;

				}


				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				// %%% Add Query Part Here %%%%%%%%%%%%%%%%%%%%
				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

				// add space before field name
				tq_strdup(query_string_gen, " ", BPLN);

				// add field name
				tq_strdup(query_string_gen, (char *) field_name->data, BPLN);

				// if we have a next element, add a comma, if not, add a space separator
				if(field_name->hh.next != NULL)
					tq_strdup(query_string_gen, ",", BPLN);
				else
					tq_strdup(query_string_gen, " ", BPLN);

				// increment the iter position
				select_field_name_iter++;

			}
			BP_HASH_ITER_CLOSE;

		}

	}

	// add from table
	tq_strdup(query_string_gen, "FROM ", BPLN);
	tq_strdup(query_string_gen, syngen->select_info->table_name, BPLN);





	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Joins if Necessary (JOIN before WHERE) %%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we have joins, add them
	if(syngen->select_info->join_on)
	{


		// create join iter
		size_t join_iter = 0;

		for
		(
			;
			join_iter < syngen->select_info->join_on_entry_n;
			join_iter++
		)
		{

			// add join prefixes here
			switch(syngen->select_info->join_on[join_iter].join_type)
			{

				case BPDB_SELECT_JOIN_TYPE_RIGHT_JOIN:
					tq_strdup(query_string_gen, " RIGHT JOIN ", BPLN);
					break;

				case BPDB_SELECT_JOIN_TYPE_LEFT_JOIN:
					tq_strdup(query_string_gen, " LEFT JOIN ", BPLN);
					break;

				case BPDB_SELECT_JOIN_TYPE_INNER_JOIN:
					tq_strdup(query_string_gen, " JOIN ", BPLN);
					break;

				case BPDB_SELECT_JOIN_TYPE_CROSS_JOIN:
					tq_strdup(query_string_gen, " CROSS JOIN ", BPLN);
					break;

				default:
					break;

			}

			// check the comparison iterator
			if
			(
				!BP_StringIsReasonableForMariadbComparisonOperatorDB
				(
					syngen->select_info->join_on[join_iter].comparison_operator,
					bpstrlen(syngen->select_info->join_on[join_iter].comparison_operator)
				)
			)
			{

				// destroy all residual select info (lists/results/parameters etc) before exiting
				BP_DBSelectInfoCleanResidualData(syngen->select_info);

				// destroy the query generator
				BP_DestroyLinkL(query_string_gen, BP_TRUE);

				// create error string
				char tmp_err[256];
				BP_ZERO_STACK(tmp_err);
				sprintf((char *) &tmp_err, "BP_DBSGSelectFrom failed to BP_StringIsReasonableForMariadbComparisonOperatorDB on join idx: %zu", join_iter);
				BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);

				// return indicating failure
				return ERR_FAILURE;

			}



			// add join element
			tq_strdup(query_string_gen, syngen->select_info->join_on[join_iter].table_name, BPLN);
			tq_strdup(query_string_gen, " ON ", BPLN);
			tq_strdup(query_string_gen, syngen->select_info->join_on[join_iter].front_join, BPLN);
			tq_strdup(query_string_gen, " ", BPLN);
			tq_strdup(query_string_gen, syngen->select_info->join_on[join_iter].comparison_operator, BPLN);
			tq_strdup(query_string_gen, " ", BPLN);
			tq_strdup(query_string_gen, syngen->select_info->join_on[join_iter].tail_join, BPLN);

		}

	}





	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add WHERE clauses %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if there are where clauses, add them here
	if(syngen->select_info->select_where && syngen->select_info->select_where_entry_n)
	{

		// add preliminary where clause
		tq_strdup(query_string_gen, " WHERE ", BPLN);

		// declare the iterator
		size_t select_where_iter = 0;

		// walk selections
		for
		(
			;
			select_where_iter < syngen->select_info->select_where_entry_n;
			select_where_iter++
		)
		{

			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Run Value Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// every where iterator after the first must have a valid conjunction
			if(select_where_iter > 0)
			if(!syngen->select_info->select_where[select_where_iter].conjunction)
			{

				// destroy the query generator
				BP_DestroyLinkL(query_string_gen, BP_TRUE);

				// create error string
				char tmp_err[256];
				BP_ZERO_STACK(tmp_err);
				sprintf((char *) &tmp_err, "BP_DBSGSelectFrom failed because a where clause other than the first was listed without a conjunction (aka there was no AND, OR, etc clauses). This occured on clause index number %zu.", select_where_iter);
				BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);

				// return indicating failure
				return ERR_FAILURE;

			}


			// check the limiter
			if(syngen->select_info->select_where[select_where_iter].limiter)
			if(!BP_StringIsReasonableForMariadbComparisonOperatorDB(syngen->select_info->select_where[select_where_iter].limiter, bpstrlen(syngen->select_info->select_where[select_where_iter].limiter)))
			{

				// destroy all residual select info (lists/results/parameters etc) before exiting
				BP_DBSelectInfoCleanResidualData(syngen->select_info);

				// destroy the query generator
				BP_DestroyLinkL(query_string_gen, BP_TRUE);


				// create error string
				char tmp_err[256+bpstrlen(syngen->select_info->select_where[select_where_iter].limiter)];
				BP_ZERO_STACK(tmp_err);
				sprintf((char *) &tmp_err, "BP_DBSGSelectFrom reports that the current select limiter is invalid at index %zu (val: '%s')", select_where_iter, syngen->select_info->select_where[select_where_iter].limiter);
				BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);


				// return indicating failure
				return ERR_FAILURE;
			}

			// check the conjunction
			if(syngen->select_info->select_where[select_where_iter].conjunction)
			if(!BP_StringIsReasonableForMariadbConjunctionDB(syngen->select_info->select_where[select_where_iter].conjunction, bpstrlen(syngen->select_info->select_where[select_where_iter].conjunction)))
			{

				// destroy all residual select info (lists/results/parameters etc) before exiting
				BP_DBSelectInfoCleanResidualData(syngen->select_info);

				// destroy the query generator
				BP_DestroyLinkL(query_string_gen, BP_TRUE);

				// create error string
				char tmp_err[256];
				BP_ZERO_STACK(tmp_err);
				sprintf((char *) &tmp_err, "BP_DBSGSelectFrom failed to BP_StringIsReasonableForMariadbConjunctionDB on select_where_iter: %zu", select_where_iter);
				BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);

				// return indicating failure
				return ERR_FAILURE;

			}


			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Add Select-Where Entries After Checks Finish %%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// if there's a conjunction, add it
			if(syngen->select_info->select_where[select_where_iter].conjunction)
			{

				tq_strdup(query_string_gen, " ", BPLN);
				tq_strdup(query_string_gen, syngen->select_info->select_where[select_where_iter].conjunction, BPLN);
				tq_strdup(query_string_gen, " ", BPLN);

			}

			// add front part with field name
			tq_strdup(query_string_gen, syngen->select_info->select_where[select_where_iter].field_name, BPLN);
			tq_strdup(query_string_gen, " ", BPLN);

			// add limiter
			tq_strdup(query_string_gen, syngen->select_info->select_where[select_where_iter].limiter, BPLN);
			tq_strdup(query_string_gen, " ", BPLN);



			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Add in Values %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


			// if the value is a table field, copy it in directly (sanity of string has
			// been predetermined by check routines, copy is safe)
			if(syngen->select_info->select_where[select_where_iter].value_type == BPDB_SELECT_WHERE_VALUE_TYPE_TABLE_AND_COLUMN)
			{

				// add where value (note: value checks already done at this point)
				tq_strdup(query_string_gen, " ", BPLN);
				tq_strdup(query_string_gen, syngen->select_info->select_where[select_where_iter].value, BPLN);

			}
			else
			{

				// add where parameter
				if(!BP_DBAddSelectWhereParameterToSelectInfo(syngen, syngen->select_info, &syngen->select_info->select_where[select_where_iter]))
				{

					// destroy all residual select info (lists/results/parameters etc) before exiting
					BP_DBSelectInfoCleanResidualData(syngen->select_info);

					// destroy the query generator
					BP_DestroyLinkL(query_string_gen, BP_TRUE);

					// create error string
					char tmp_err[256];
					BP_ZERO_STACK(tmp_err);
					sprintf((char *) &tmp_err, "BP_DBSGSelectFrom failed to BP_DBAddSelectWhereParameterToSelectInfo on select_where_iter: %zu", select_where_iter);
					BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);


					// return indicating failure
					return ERR_FAILURE;

				}

				// add the prepared statement position
				tq_strdup(query_string_gen, "?", BPLN);

			}

		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Group By %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add group by if we have one
	if(syngen->select_info->group_by)
	{
		tq_strdup(query_string_gen, " GROUP BY (", BPLN);
		tq_strdup(query_string_gen, syngen->select_info->group_by->field, BPLN);
		tq_strdup(query_string_gen, ") ", BPLN);
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Order By %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set orderby fields into query
	if(syngen->select_info->order_by)
	{

		// check to ensure that sort designator is valid before adding
		if(!BP_StringIsReasonableForMariadbSortDesignatorDB(syngen->select_info->order_by->sort_designator, bpstrlen(syngen->select_info->order_by->sort_designator)))
		{

			// destroy all residual select info (lists/results/parameters etc) before exiting
			BP_DBSelectInfoCleanResidualData(syngen->select_info);

			// destroy the query generator
			BP_DestroyLinkL(query_string_gen, BP_TRUE);

			// create error string
			char tmp_err[256];
			BP_ZERO_STACK(tmp_err);
			sprintf((char *) &tmp_err, "BP_DBSGSelectFrom failed to BP_StringIsReasonableForMariadbSortDesignatorDB");
			BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);

			// return indicating failure
			return ERR_FAILURE;

		}

		// add sort designator
		tq_strdup(query_string_gen, " ORDER BY ", BPLN);
		tq_strdup(query_string_gen, syngen->select_info->order_by->field, BPLN);
		tq_strdup(query_string_gen, " ", BPLN);
		tq_strdup(query_string_gen, syngen->select_info->order_by->sort_designator, BPLN);

	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Limits %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(syngen->select_info->limits)
	{

		// low limit cannot be greater than the high limit (although it CAN be equal)
		if(syngen->select_info->limits->limit_low > syngen->select_info->limits->limit_high)
		{

			// destroy all residual select info (lists/results/parameters etc) before exiting
			BP_DBSelectInfoCleanResidualData(syngen->select_info);

			// destroy the query generator
			BP_DestroyLinkL(query_string_gen, BP_TRUE);

			// create error string
			char tmp_err[256];
			BP_ZERO_STACK(tmp_err);
			sprintf((char *) &tmp_err, "BP_DBSGSelectFrom failed because limit_low was greater than limit_high when creating LIMIT clause.");
			BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);

			// return indicating failure
			return ERR_FAILURE;

		}

		// set low limit
		char limit_low[128];
		BP_ZERO_STACK(limit_low);
		sprintf((char *) &limit_low, "%u", syngen->select_info->limits->limit_low);

		// set high limit
		char limit_high[128];
		BP_ZERO_STACK(limit_high);
		sprintf((char *) &limit_high, "%u", syngen->select_info->limits->limit_high);

		// add strdup
		tq_strdup(query_string_gen, " LIMIT ", BPLN);
		tq_strdup(query_string_gen, (char *) &limit_low,  BPLN);
		tq_strdup(query_string_gen, ",", BPLN);
		tq_strdup(query_string_gen, (char *) &limit_high, BPLN);

	}













	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Generate Final Query %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// flatten string elements
	BP_LinkLFlattenJoinStringElements(query_string_gen);

	// set final query
	char * final_select_query = (char *) query_string_gen->flat_buff;

	// display the final select query
	printf("\n Got final select?: %s", final_select_query);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Prepare Query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to prepare the query
	MYSQL_STMT * stmt = BP_MySQLPrepareQuery
	(
		syngen->mariadb,
		final_select_query
	);

	// ensure the statement was created ok
	if(!stmt)
	{

		// destroy all residual select info (lists/results/parameters etc) before exiting
		BP_DBSelectInfoCleanResidualData(syngen->select_info);

		// destroy the query generator
		BP_DestroyLinkL(query_string_gen, BP_TRUE);

		// create error string
		char tmp_err[256];
		BP_ZERO_STACK(tmp_err);
		sprintf((char *) &tmp_err, "BP_DBSGSelectFrom reports that BP_MySQLPrepareQuery failed to prepare a statement for query: '%s'.", final_select_query);
		BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);

		// return indicating failure
		return ERR_FAILURE;

	}


	// display statement prep message
	printf("\n [+] Statement prepared OK!");


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Bind Parameters If Necessary %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	printf("\n Got Bind Parameters for Query?: %zu", syngen->select_info->bind_parameters_n);

	// set bind parameters
	if(syngen->select_info->bind_parameters && syngen->select_info->bind_parameters_n)
	{

		// bind parameters
	   if(BP_MySQLBindParameters(stmt, (MYSQL_BIND *) syngen->select_info->bind_parameters, syngen->mariadb) == ERR_FAILURE)
	   {

			// destroy all residual select info (lists/results/parameters etc) before exiting
			BP_DBSelectInfoCleanResidualData(syngen->select_info);

			// destroy the query generator
			BP_DestroyLinkL(query_string_gen, BP_TRUE);

			// create error string
			char tmp_err[256];
			BP_ZERO_STACK(tmp_err);
			sprintf((char *) &tmp_err, "BP_DBSGSelectFrom reports that BP_MySQLBindParameters failed to prepare a statement.");
			BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);

			// return indicating failure
			return ERR_FAILURE;

	   }

	   printf("\n Parameters have been bound ok!?");
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Execute Statement %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to execute the statement
	if(BP_MySQLExecuteStatement(stmt, syngen->mariadb) == ERR_FAILURE)
	{

			// destroy all residual select info (lists/results/parameters etc) before exiting
			BP_DBSelectInfoCleanResidualData(syngen->select_info);

			// destroy the query generator
			BP_DestroyLinkL(query_string_gen, BP_TRUE);

			// create error string
			char tmp_err[256];
			BP_ZERO_STACK(tmp_err);
			sprintf((char *) &tmp_err, "BP_DBSGSelectFrom reports that BP_MySQLExecuteStatement failed execute statement after prepare.");
			BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);

			// return indicating failure
			return ERR_FAILURE;

	}

	// dbg: display execution status
	printf("\n [!!] Statement executed ok!!.");

	// set the syngen->select_info mariadb statement after execution is verified.
	syngen->select_info->mariadb_stmt = stmt;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Bind Results and Store If Necessary %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we have results to bind, bind them here
	if(syngen->select_info->bind_results && syngen->select_info->bind_results_n)
	{

		// bind the result
		if
		(
			BP_MySQLBindResult
			(
				stmt,
				syngen->select_info->bind_results,
				syngen->select_info->bind_results_n,
				syngen->mariadb
			) == ERR_FAILURE
		)
		{
				printf("\n [!!] Unable to bind result to the statement.  Required for fetching result.");

				// destroy all residual select info (lists/results/parameters etc) before exiting
				BP_DBSelectInfoCleanResidualData(syngen->select_info);

				// destroy the query generator
				BP_DestroyLinkL(query_string_gen, BP_TRUE);


				// create error string
				char tmp_err[256];
				BP_ZERO_STACK(tmp_err);
				sprintf((char *) &tmp_err, "BP_DBSGSelectFrom reports BP_MySQLBindResult failed when trying to bind results.");
				BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);

				// return indicating failure
				return ERR_FAILURE;

		}

		// display bind message
		printf("\n [+] Result bound to statement ok inner!");

		// attempt to store results (fetch from server)
		if(BP_MySQLStoreResults(stmt, syngen->mariadb) == ERR_FAILURE)
		{

				printf("\n [!!] Unable to store results");

				// destroy all residual select info (lists/results/parameters etc) before exiting
				BP_DBSelectInfoCleanResidualData(syngen->select_info);

				// destroy the query generator
				BP_DestroyLinkL(query_string_gen, BP_TRUE);

				// create error string
				char tmp_err[256];
				BP_ZERO_STACK(tmp_err);
				sprintf((char *) &tmp_err, "BP_DBSGSelectFrom reports BP_MySQLStoreResults failed when trying to store results.");
				BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);

				// return indicating failure
				return ERR_FAILURE;

		}

		printf("\n RESULT STORED OK!");

		// add result references here
		if(!BP_DBAddSelectResultReferences(syngen,syngen->select_info))
		{

			// destroy all residual select info (lists/results/parameters etc) before exiting
			BP_DBSelectInfoCleanResidualData(syngen->select_info);

			// destroy the query generator
			BP_DestroyLinkL(query_string_gen, BP_TRUE);

			// create error string
			char tmp_err[256];
			BP_ZERO_STACK(tmp_err);
			sprintf((char *) &tmp_err, "BP_DBSGSelectFrom reports BP_DBAddSelectResultReferences failed when trying to add references.");
			BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);


			// return indicating failure
			return ERR_FAILURE;

		}

	}

	// destroy the query generator
	BP_DestroyLinkL(query_string_gen, BP_TRUE);

	// return indicating success
	return ERR_SUCCESS;

}


// Generates a select query.
BP_ERROR_T BP_DBSGSelectFrom_OLD
(
	P_BPDB_SQLGEN        syngen,
	P_BPDB_SQLGEN_SELECT select_info
)
{

	// ensure we have a syntax generator
	if(!syngen)
		return ERR_FAILURE;

	// if we don't have a database exit with failure
	if(!syngen->bpdb)
		return ERR_FAILURE;

	// ensure we have a checked out mariadb connection
	if(!syngen->mariadb)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSGSelectFrom syngen->mariadb parameter is NULL.");
		return ERR_FAILURE;
	}

	// ensure we have select info to work with
	if(!select_info)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSGSelectFrom select_info parameter was NULL.");
		return ERR_FAILURE;
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Basic Checks and Generate Metadata %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// run checks and generate table/field_name lists (stored within select info)
	if(!BP_DBCheckSelectInfoAndGenerateMetadataLists(syngen,select_info))
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSGSelectFrom tried to call BP_DBCheckSelectInfoAndGenerateMetadataLists but it failed");
		return ERR_FAILURE;
	}





	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check Metadata Against BPDB Schema %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// These two checks attempt to check all items against
	// entities within the BPDB schema.

	// run field name check
	if(!BP_DBCheckFieldNamesAgainstSchema(syngen, select_info->field_name_list))
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSGSelectFrom tried to call BP_DBCheckFieldNamesAgainstSchema to verify field names.  Verification has failed.");
		return ERR_FAILURE;
	}

	// run table name check
	if(!BP_DBCheckTableNamesAgainstSchema(syngen, select_info->table_name_list))
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSGSelectFrom tried to call BP_DBCheckTableNamesAgainstSchema to verify table names.  Verification has failed.");
		return ERR_FAILURE;
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Preliminary State Setup %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// gather field count
	size_t select_field_n = BP_CountNullTerminatedPtrArray(select_info->field_names);

	// create a linkl
	P_BP_LINKL query_string_gen = BP_CreateLinkL("select query generator", BP_TRUE);
	if(!query_string_gen)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSGSelectFrom failed to create new select query generator (tail queue linkl).  Likely out of memory.  Catestropic.");
		return ERR_FAILURE;
	}


	// add select statement to start query
	tq_strdup(query_string_gen, "SELECT", BPLN);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Field Names and Populate Result Binds %%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new iterator for adding field names
	size_t select_field_name_iter = 0;


	// add selected field names
	for
	(
		;
		select_field_name_iter < select_field_n;
		select_field_name_iter++
	)
	{

		// ensure we have a string, if not break
		if(!select_info->field_names[select_field_name_iter])
			break;


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Add Results Here %%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// Attempts to add a result to the select info structure.  Used internally.
		if
		(
			!BP_DBAddSelectResultToSelectInfo
			(
				syngen,
				select_info,
				select_info->field_names[select_field_name_iter]
			)
		)
		{

			// destroy all residual select info (lists/results/parameters etc) before exiting
			BP_DBSelectInfoCleanResidualData(select_info);

			// destroy the query generator
			BP_DestroyLinkL(query_string_gen, BP_TRUE);

			// create error string
			char tmp_err[256];
			BP_ZERO_STACK(tmp_err);
			sprintf((char *) &tmp_err, "BP_DBSGSelectFrom failed to BP_DBAddSelectResultToSelectInfo on field name idx: %zu", select_field_name_iter);
			BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);

			// return indicating failure
			return ERR_FAILURE;

		}








		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Add Query Part Here %%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// add space before field name
		tq_strdup(query_string_gen, " ", BPLN);

		// add field name
		tq_strdup(query_string_gen, select_info->field_names[select_field_name_iter], BPLN);

		// add trailing comma if we have another item
		if(select_info->field_names[select_field_name_iter+1])
			tq_strdup(query_string_gen, ",", BPLN);
		else
			tq_strdup(query_string_gen, " ", BPLN);


	}


	// add from table
	tq_strdup(query_string_gen, "FROM ", BPLN);
	tq_strdup(query_string_gen, select_info->table_name, BPLN);




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Joins if Necessary (JOIN before WHERE) %%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we have joins, add them
	if(select_info->join_on)
	{


		// create join iter
		size_t join_iter = 0;

		for
		(
			;
			join_iter < select_info->join_on_entry_n;
			join_iter++
		)
		{

			// add join prefixes here
			switch(select_info->join_on[join_iter].join_type)
			{

				case BPDB_SELECT_JOIN_TYPE_RIGHT_JOIN:
					tq_strdup(query_string_gen, " RIGHT JOIN ", BPLN);
					break;

				case BPDB_SELECT_JOIN_TYPE_LEFT_JOIN:
					tq_strdup(query_string_gen, " LEFT JOIN ", BPLN);
					break;

				case BPDB_SELECT_JOIN_TYPE_INNER_JOIN:
					tq_strdup(query_string_gen, " JOIN ", BPLN);
					break;

				case BPDB_SELECT_JOIN_TYPE_CROSS_JOIN:
					tq_strdup(query_string_gen, " CROSS JOIN ", BPLN);
					break;

				default:
					break;

			}

			// check the comparison iterator
			if(!BP_StringIsReasonableForMariadbComparisonOperatorDB(select_info->join_on[join_iter].comparison_operator, bpstrlen(select_info->join_on[join_iter].comparison_operator)))
			{

				// destroy all residual select info (lists/results/parameters etc) before exiting
				BP_DBSelectInfoCleanResidualData(select_info);

				// destroy the query generator
				BP_DestroyLinkL(query_string_gen, BP_TRUE);

				// create error string
				char tmp_err[256];
				BP_ZERO_STACK(tmp_err);
				sprintf((char *) &tmp_err, "BP_DBSGSelectFrom failed to BP_StringIsReasonableForMariadbComparisonOperatorDB on join idx: %zu", join_iter);
				BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);

				// return indicating failure
				return ERR_FAILURE;

			}



			// add join element
			tq_strdup(query_string_gen, select_info->join_on[join_iter].table_name, BPLN);
			tq_strdup(query_string_gen, " ON ", BPLN);
			tq_strdup(query_string_gen, select_info->join_on[join_iter].front_join, BPLN);
			tq_strdup(query_string_gen, " ", BPLN);
			tq_strdup(query_string_gen, select_info->join_on[join_iter].comparison_operator, BPLN);
			tq_strdup(query_string_gen, " ", BPLN);
			tq_strdup(query_string_gen, select_info->join_on[join_iter].tail_join, BPLN);

		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add WHERE clauses %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if there are where clauses, add them here
	if(select_info->select_where && select_info->select_where_entry_n)
	{

		// add preliminary where clause
		tq_strdup(query_string_gen, " WHERE ", BPLN);

		// declare the iterator
		size_t select_where_iter = 0;

		// walk selections
		for
		(
			;
			select_where_iter < select_info->select_where_entry_n;
			select_where_iter++
		)
		{

			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Run Value Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// every where iterator after the first must have a valid conjunction
			if(select_where_iter > 0)
			if(!select_info->select_where[select_where_iter].conjunction)
			{
				// destroy the query generator
				BP_DestroyLinkL(query_string_gen, BP_TRUE);

				// create error string
				char tmp_err[256];
				BP_ZERO_STACK(tmp_err);
				sprintf((char *) &tmp_err, "BP_DBSGSelectFrom failed because a where clause other than the first was listed without a conjunction (aka there was no AND, OR, etc clauses). This occured on clause index number %zu.", select_where_iter);
				BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);

				// return indicating failure
				return ERR_FAILURE;

			}


			// check the limiter
			if(select_info->select_where[select_where_iter].limiter)
			if(!BP_StringIsReasonableForMariadbComparisonOperatorDB(select_info->select_where[select_where_iter].limiter, bpstrlen(select_info->select_where[select_where_iter].limiter)))
			{

				// destroy all residual select info (lists/results/parameters etc) before exiting
				BP_DBSelectInfoCleanResidualData(select_info);

				// destroy the query generator
				BP_DestroyLinkL(query_string_gen, BP_TRUE);


				// create error string
				char tmp_err[256+bpstrlen(select_info->select_where[select_where_iter].limiter)];
				BP_ZERO_STACK(tmp_err);
				sprintf((char *) &tmp_err, "BP_DBSGSelectFrom reports that the current select limiter is invalid at index %zu (val: '%s')", select_where_iter, select_info->select_where[select_where_iter].limiter);
				BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);


				// return indicating failure
				return ERR_FAILURE;
			}

			// check the conjunction
			if(select_info->select_where[select_where_iter].conjunction)
			if(!BP_StringIsReasonableForMariadbConjunctionDB(select_info->select_where[select_where_iter].conjunction, bpstrlen(select_info->select_where[select_where_iter].conjunction)))
			{

				// destroy all residual select info (lists/results/parameters etc) before exiting
				BP_DBSelectInfoCleanResidualData(select_info);

				// destroy the query generator
				BP_DestroyLinkL(query_string_gen, BP_TRUE);

				// create error string
				char tmp_err[256];
				BP_ZERO_STACK(tmp_err);
				sprintf((char *) &tmp_err, "BP_DBSGSelectFrom failed to BP_StringIsReasonableForMariadbConjunctionDB on select_where_iter: %zu", select_where_iter);
				BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);

				// return indicating failure
				return ERR_FAILURE;

			}


			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Add Select-Where Entries After Checks Finish %%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// if there's a conjunction, add it
			if(select_info->select_where[select_where_iter].conjunction)
			{

				tq_strdup(query_string_gen, " ", BPLN);
				tq_strdup(query_string_gen, select_info->select_where[select_where_iter].conjunction, BPLN);
				tq_strdup(query_string_gen, " ", BPLN);

			}

			// add front part with field name
			tq_strdup(query_string_gen, select_info->select_where[select_where_iter].field_name, BPLN);
			tq_strdup(query_string_gen, " ", BPLN);

			// add limiter
			tq_strdup(query_string_gen, select_info->select_where[select_where_iter].limiter, BPLN);
			tq_strdup(query_string_gen, " ", BPLN);



			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Add in Values %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


			// if the value is a table field, copy it in directly (sanity of string has
			// been predetermined by check routines, copy is safe)
			if(select_info->select_where[select_where_iter].value_type == BPDB_SELECT_WHERE_VALUE_TYPE_TABLE_AND_COLUMN)
			{

				// add where value (note: value checks already done at this point)
				tq_strdup(query_string_gen, " ", BPLN);
				tq_strdup(query_string_gen, select_info->select_where[select_where_iter].value, BPLN);

			}
			else
			{

				// add where parameter
				if(!BP_DBAddSelectWhereParameterToSelectInfo(syngen, select_info, &select_info->select_where[select_where_iter]))
				{

					// destroy all residual select info (lists/results/parameters etc) before exiting
					BP_DBSelectInfoCleanResidualData(select_info);

					// destroy the query generator
					BP_DestroyLinkL(query_string_gen, BP_TRUE);

					// create error string
					char tmp_err[256];
					BP_ZERO_STACK(tmp_err);
					sprintf((char *) &tmp_err, "BP_DBSGSelectFrom failed to BP_DBAddSelectWhereParameterToSelectInfo on select_where_iter: %zu", select_where_iter);
					BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);


					// return indicating failure
					return ERR_FAILURE;

				}

				// add the prepared statement position
				tq_strdup(query_string_gen, "?", BPLN);

			}

		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Order By %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(select_info->order_by)
	{

		// check to ensure that sort designator is valid before adding
		if(!BP_StringIsReasonableForMariadbSortDesignatorDB(select_info->order_by->sort_designator, bpstrlen(select_info->order_by->sort_designator)))
		{

			// destroy all residual select info (lists/results/parameters etc) before exiting
			BP_DBSelectInfoCleanResidualData(select_info);

			// destroy the query generator
			BP_DestroyLinkL(query_string_gen, BP_TRUE);

			// create error string
			char tmp_err[256];
			BP_ZERO_STACK(tmp_err);
			sprintf((char *) &tmp_err, "BP_DBSGSelectFrom failed to BP_StringIsReasonableForMariadbSortDesignatorDB");
			BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);

			// return indicating failure
			return ERR_FAILURE;

		}

		// add sort designator
		tq_strdup(query_string_gen, " ORDER BY ", BPLN);
		tq_strdup(query_string_gen, select_info->order_by->field, BPLN);
		tq_strdup(query_string_gen, " ", BPLN);
		tq_strdup(query_string_gen, select_info->order_by->sort_designator, BPLN);

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Limits %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(select_info->limits)
	{

		// low limit cannot be greater than the high limit (although it CAN be equal)
		if(select_info->limits->limit_low > select_info->limits->limit_high)
		{

			// destroy all residual select info (lists/results/parameters etc) before exiting
			BP_DBSelectInfoCleanResidualData(select_info);

			// destroy the query generator
			BP_DestroyLinkL(query_string_gen, BP_TRUE);

			// create error string
			char tmp_err[256];
			BP_ZERO_STACK(tmp_err);
			sprintf((char *) &tmp_err, "BP_DBSGSelectFrom failed because limit_low was greater than limit_high when creating LIMIT clause.");
			BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);

			// return indicating failure
			return ERR_FAILURE;

		}

		// set low limit
		char limit_low[128];
		BP_ZERO_STACK(limit_low);
		sprintf((char *) &limit_low, "%u", select_info->limits->limit_low);

		// set high limit
		char limit_high[128];
		BP_ZERO_STACK(limit_high);
		sprintf((char *) &limit_high, "%u", select_info->limits->limit_high);

		// add strdup
		tq_strdup(query_string_gen, " LIMIT ", BPLN);
		tq_strdup(query_string_gen, (char *) &limit_low,  BPLN);
		tq_strdup(query_string_gen, ",", BPLN);
		tq_strdup(query_string_gen, (char *) &limit_high, BPLN);

	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Generate Final Query %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// flatten string elements
	BP_LinkLFlattenJoinStringElements(query_string_gen);

	// set final query
	char * final_select_query = (char *) query_string_gen->flat_buff;

	// display the final select query
	printf("\n Got final select?: %s", final_select_query);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Prepare Query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // attempt to prepare the query
    MYSQL_STMT * stmt = BP_MySQLPrepareQuery
    (
    	syngen->mariadb,
		final_select_query
    );

    // ensure the statement was created ok
    if(!stmt)
    {

		// destroy all residual select info (lists/results/parameters etc) before exiting
		BP_DBSelectInfoCleanResidualData(select_info);

		// destroy the query generator
		BP_DestroyLinkL(query_string_gen, BP_TRUE);

		// create error string
		char tmp_err[256];
		BP_ZERO_STACK(tmp_err);
		sprintf((char *) &tmp_err, "BP_DBSGSelectFrom reports that BP_MySQLPrepareQuery failed to prepare a statement for query: '%s'.", final_select_query);
		BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);

		// return indicating failure
		return ERR_FAILURE;

    }


    // display statement prep message
    printf("\n [+] Statement prepared OK!");


    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Bind Parameters If Necessary %%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // set bind parameters
    if(select_info->bind_parameters && select_info->bind_parameters_n)
    {

    	// bind parameters
	   if(BP_MySQLBindParameters(stmt, (MYSQL_BIND *) select_info->bind_parameters, syngen->mariadb) == ERR_FAILURE)
	   {

			// destroy all residual select info (lists/results/parameters etc) before exiting
			BP_DBSelectInfoCleanResidualData(select_info);

			// destroy the query generator
			BP_DestroyLinkL(query_string_gen, BP_TRUE);

			// create error string
			char tmp_err[256];
			BP_ZERO_STACK(tmp_err);
			sprintf((char *) &tmp_err, "BP_DBSGSelectFrom reports that BP_MySQLBindParameters failed to prepare a statement.");
			BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);

			// return indicating failure
			return ERR_FAILURE;

	   }

	   printf("\n Parameters have been bound ok!?");
    }

    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Execute Statement %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // attempt to execute the statement
	if(BP_MySQLExecuteStatement(stmt, syngen->mariadb) == ERR_FAILURE)
	{

			// destroy all residual select info (lists/results/parameters etc) before exiting
			BP_DBSelectInfoCleanResidualData(select_info);

			// destroy the query generator
			BP_DestroyLinkL(query_string_gen, BP_TRUE);

			// create error string
			char tmp_err[256];
			BP_ZERO_STACK(tmp_err);
			sprintf((char *) &tmp_err, "BP_DBSGSelectFrom reports that BP_MySQLExecuteStatement failed execute statement after prepare.");
			BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);

			// return indicating failure
			return ERR_FAILURE;

	}

	// dbg: display execution status
	printf("\n [!!] Statement executed ok!!.");

	// set the select_info mariadb statement after execution is verified.
	select_info->mariadb_stmt = stmt;



    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Bind Results and Store If Necessary %%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we have results to bind, bind them here
    if(select_info->bind_results && select_info->bind_results_n)
    {

        // bind the result
        if
		(
				BP_MySQLBindResult
				(
						stmt,
						select_info->bind_results,
						select_info->bind_results_n,
						syngen->mariadb
				) == ERR_FAILURE
		)
        {
                printf("\n [!!] Unable to bind result to the statement.  Required for fetching result.");

                // destroy all residual select info (lists/results/parameters etc) before exiting
				BP_DBSelectInfoCleanResidualData(select_info);

				// destroy the query generator
				BP_DestroyLinkL(query_string_gen, BP_TRUE);


				// create error string
				char tmp_err[256];
				BP_ZERO_STACK(tmp_err);
				sprintf((char *) &tmp_err, "BP_DBSGSelectFrom reports BP_MySQLBindResult failed when trying to bind results.");
				BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);

				// return indicating failure
				return ERR_FAILURE;

        }

        // display bind message
        printf("\n [+] Result bound to statement ok inner!");

        // attempt to store results (fetch from server)
        if(BP_MySQLStoreResults(stmt, syngen->mariadb) == ERR_FAILURE)
        {

                printf("\n [!!] Unable to store results");

                // destroy all residual select info (lists/results/parameters etc) before exiting
				BP_DBSelectInfoCleanResidualData(select_info);

				// destroy the query generator
				BP_DestroyLinkL(query_string_gen, BP_TRUE);

				// create error string
				char tmp_err[256];
				BP_ZERO_STACK(tmp_err);
				sprintf((char *) &tmp_err, "BP_DBSGSelectFrom reports BP_MySQLStoreResults failed when trying to store results.");
				BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);

				// return indicating failure
				return ERR_FAILURE;

        }

        printf("\n RESULT STORED OK!");

        // add result references here
        if(!BP_DBAddSelectResultReferences(syngen,select_info))
        {

        	// destroy all residual select info (lists/results/parameters etc) before exiting
			BP_DBSelectInfoCleanResidualData(select_info);

			// destroy the query generator
			BP_DestroyLinkL(query_string_gen, BP_TRUE);

			// create error string
			char tmp_err[256];
			BP_ZERO_STACK(tmp_err);
			sprintf((char *) &tmp_err, "BP_DBSGSelectFrom reports BP_DBAddSelectResultReferences failed when trying to add references.");
			BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_err);


			// return indicating failure
			return ERR_FAILURE;

        }

    }

	// return indicating success
	return ERR_SUCCESS;

}


// Generates the sql syntax required for an insert query.
BP_ERROR_T BP_DBSGInsertInto(P_BPDB_SQLGEN syngen, BPDB_SELECTOR_TYPE out_db_selector, char *table_name, size_t max_records)
{

	if(!syngen)
		return ERR_FAILURE;
	if(!out_db_selector)
		return ERR_FAILURE;
	if(!table_name)
		return ERR_FAILURE;

	// ensure table name is reasonable
	if(!BP_StringIsReasonableForTableNameDB(table_name, bpstrlen(table_name)))
		return ERR_FAILURE;


	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Fetch Result %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Fetches a result, results are automatically destroyed on fetch so be careful
// to copy data rather than reference it directly.

BP_ERROR_T BP_DBSGFetchResult
(
	P_BPDB_SQLGEN        syngen,
	P_BPDB_SQLGEN_SELECT select_info,
	BP_BOOL              create_json_reg
)
{

	// run basic null checks
	if(!syngen)
		return ERR_FAILURE;
	if(!syngen->bpdb)
		return ERR_FAILURE;

	// ensure we have select_info
	if(!select_info)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSGFetchResult reports that it was run with a NULL select_info parameter.");
		return ERR_FAILURE;
	}

	// ensure we have a mariadb statement to process
	if(!select_info->mariadb_stmt)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSGFetchResult reports that it was run with a NULL select_info->mariadb_stmt parameter.");
		return ERR_FAILURE;
	}
	if(!select_info->bind_results)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSGFetchResult reports that it was run with a NULL bind_results parameter.");
		return ERR_FAILURE;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Retrieve Result if Possible %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// fetch result
	if(BP_MySQLFetchResult(select_info->mariadb_stmt, syngen->mariadb) == ERR_SUCCESS)
	{

		// increment the record counter on successful fetch
		select_info->fetched_record_n++;

		// create a new hash table registry if necessary
		if(!select_info->fetched_record_hreg)
		{

			// create new hash table registry
			select_info->fetched_record_hreg = BP_CreateHashTableRegistry();

		}

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Now Generate JSON Table For Values %%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// if we are creating a json reg, do it now
		if(create_json_reg)
		{

			// attempt to update the json result registry
			BP_DBSGUpdateJSONResultRegistry
			(
				syngen,
				select_info
			);

		}

		// destroy the result after fetching
		BP_MySQLFreeResult(select_info->mariadb_stmt, syngen->mariadb);

		// return indicating success
		return ERR_SUCCESS;

	}

	// destroy the result after fetching
	BP_MySQLFreeResult(select_info->mariadb_stmt, syngen->mariadb);

	// return indicating failure
	return ERR_FAILURE;

}


// Get current result value by field name.  Returned result is
// standard mysql/mariadb bind structure.
MYSQL_BIND * BP_DBSGLookupResultByFieldName
(
	P_BPDB_SQLGEN        syngen,
	P_BPDB_SQLGEN_SELECT select_info,
	char               * field_name
)
{

	// run basic checks
	if(!syngen)
		return NULL;
	if(!syngen->bpdb)
		return NULL;

	if(!select_info)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSGLookupResultByFieldName reports that it was run with a NULL select_info parameter.");
		return NULL;
	}
	if(!field_name)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSGLookupResultByFieldName reports that it was run with a NULL field_name parameter.");
		return NULL;
	}

	// lock system hash registry before modification
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(select_info->fetched_record_hreg);

	// set lookup path
	char * lookup_path[] = {field_name, "result_data_ptr", NULL};

	// retrive table metadata by path
	P_BP_HASH_TABLE_KEYED_ENTRY result_data_ptr_lookup = BP_HashRegLookupTableEntryByPath(select_info->fetched_record_hreg, (char **) &lookup_path);
	if(!result_data_ptr_lookup)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(select_info->fetched_record_hreg, NULL);


	// create result pointer array to deref from
	MYSQL_BIND ** result_ptr_arr = (MYSQL_BIND **) result_data_ptr_lookup->data;

	// set the return pointer here from deref
	MYSQL_BIND * ret_ptr = *result_ptr_arr;

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(select_info->fetched_record_hreg);

	// return the ret ptr
	return ret_ptr;

}


// Attempts to update the json result registry.
BP_ERROR_T BP_DBSGUpdateJSONResultRegistry
(
	P_BPDB_SQLGEN        syngen,
	P_BPDB_SQLGEN_SELECT select_info
)
{

	// ensure we have a syngen
	if(!syngen)
		return ERR_FAILURE;
	if(!syngen->bpdb)
		return ERR_FAILURE;

	// ensure we have select info
	if(!select_info)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSGUpdateJSONResultRegistry reports that it was run with a NULL select_info parameter.");
		return ERR_FAILURE;
	}

	// ensure we have a record registry
	if(!select_info->fetched_record_hreg)
		return ERR_FAILURE;
	if(!select_info->fetched_record_n)
		return ERR_FAILURE;

	// If a hreg already exists we have to destroy/recreate it.
	if(select_info->fetched_record_json_hreg)
	{

		// destroy the registry
		BP_DestroyHashTableRegistry(select_info->fetched_record_json_hreg);

	}



	// --- create new registry and translate record data ----------

	// create new registry
	select_info->fetched_record_json_hreg = BP_CreateHashTableRegistry();

	// column metadata iterator
	P_BPDB_SQL_TABLE_COLUMN_METADATA col_metadata_iter = NULL;

	// create some stack to hold a potential record
	char record_buff[128];
	BP_ZERO_STACK(record_buff);

	// fill in the buffer
	sprintf(record_buff, "res_idx_%u", select_info->fetched_record_n-1);


	// -- attempt to add top path --
	// create the top add path
	char *top_add_path[4];
	BP_ZERO_STACK(top_add_path);

	// set the record buff as the top path
	top_add_path[0] = (char *) &record_buff;

	// add result header
	if(!BP_HashRegAddTableByPath(select_info->fetched_record_json_hreg, (char **) &top_add_path))
	{
		BP_DestroyHashTableRegistry(select_info->fetched_record_json_hreg);
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSGUpdateJSONResultRegistry reports that it failed to add a new top path.");
		return ERR_FAILURE;
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Utility Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create lookup stack
	char *lookup_stack[10];
	BP_ZERO_STACK(lookup_stack);

	// set lookup entry to null
	P_BP_HASH_TABLE_KEYED_ENTRY data_lookup_entry = NULL;


	// iterate through the hashes
	BP_HASH_ITER_OPEN(select_info->fetched_record_hreg->hive, result_iter)
	{

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Add Top Level Path %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// set the add path from the key
		top_add_path[1] = result_iter->key;

		// add new table
		BP_HashRegAddTableByPath(select_info->fetched_record_json_hreg, (char **) &top_add_path);


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Lookup Metadata and Set Type %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// attempt to lookup column data from bpdb
		col_metadata_iter = BP_DBLookupTableColumnMetadataFromJoinedFieldName(syngen->bpdb, result_iter->key);
		if(col_metadata_iter)
		{

			// get column type from bin type (since type is sent in json)
			char * col_type = BP_DBMariadbColumnTypeToString(col_metadata_iter->mariadb_column_metadata.data_type_as_enum);

			// add data to table by path query
			BP_HashRegAddDataToTableByPath
			(
				select_info->fetched_record_json_hreg,
				top_add_path,
				"type",
				BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
				col_type,
				bpstrlen(col_type)
			);

			// destroy column type
			bpfree(col_type);

		}



		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Add Data From Bind (based on type) %%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// get column type from bin type (since type is sent in json)
		// char * col_type = BP_DBMariadbColumnTypeToString(col_metadata_iter->mariadb_column_metadata.data_type_as_enum);

		// attempt to retrieve bind info
		MYSQL_BIND * col_bind_info = BP_DBSGLookupResultByFieldName(syngen, select_info, result_iter->key);
		if(col_bind_info)
		{

			// attempt to autoconvert data to json format
			char * out_val = BP_DBMariadbConvertColumnResultDataToJSONData
			(
				col_bind_info,
				col_metadata_iter->mariadb_column_metadata.data_type_as_enum
			);

			// add data to table by path query
			BP_HashRegAddDataToTableByPath
			(
				select_info->fetched_record_json_hreg,
				top_add_path,
				"value",
				BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
				out_val,
				bpstrlen(out_val)
			);

			// switch on column type
			switch(col_metadata_iter->mariadb_column_metadata.data_type_as_enum)
			{

				// all blob types are base64, mark json as such
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

					// mark record as base64
					BP_HashRegAddDataToTableByPath
					(
						select_info->fetched_record_json_hreg,
						top_add_path,
						"is_base64",
						BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
						(char *) "true",
						bpstrlen("true")
					);
					break;

				// all other types are unmarked as base64
				default:
					break;

			}

			// destroy column type
			bpfree(out_val);

		}

	} BP_HASH_ITER_CLOSE;


	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Sanity / Check Functions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Checks a field name list and make sure elements match up
BP_ERROR_T BP_DBCheckSelectFieldNameList
(
	P_BPDB     bpdb,
	P_BP_LINKL field_name_list,
	P_BP_LINKL table_name_list
)
{

	// ensure we have a bpdb (required for error reporting)
	if(!bpdb)
		return ERR_FAILURE;

	// ensure we have a field name list
	if(!field_name_list)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBCheckSelectFieldNameList was called with a NULL field_name_list parameter.");
		return ERR_FAILURE;
	}

	// ensure there are allocations in the list
	if(!field_name_list->total_allocations)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBCheckSelectFieldNameList was called with field_name_list->total_allocations == 0.");
		return ERR_FAILURE;
	}

	// ensure we have a table name list
	if(!table_name_list)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBCheckSelectFieldNameList was called with a NULL table_name_list parameter.");
		return ERR_FAILURE;
	}

	// ensure we have allocations in the list
	if(!table_name_list->total_allocations)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBCheckSelectFieldNameList was called with table_name_list->total_allocations == 0.");
		return ERR_FAILURE;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Walk Field Name List %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to get the first item
	P_BP_LINKL_DATA field_name_iter = TAILQ_FIRST(&field_name_list->head);

	// ensure we have a first item
	if(!field_name_iter)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBCheckSelectFieldNameList attempted to find a first element within the first_name_list, and failed.");
		return ERR_FAILURE;
	}

	// index counter
	size_t field_name_list_idx = 0;

	// looks for a pointer in the item
	TAILQ_FOREACH(field_name_iter, &field_name_list->head, linkl_data)
	{


		// only work with strndup/strdup types
		if
		(
			field_name_iter->data_type != BP_MEMPROF_ALLOC_TYPE_STRDUP   &&
			field_name_iter->data_type != BP_MEMPROF_ALLOC_TYPE_STRNDUP
		)
		{
			field_name_list_idx++;
			continue;
		}


		// set field element
		char * field_elem = (char *) field_name_iter->data;



		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Run Basic Checks %%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// break loop if the field name doesn't exist (aka null ptr)
		if(!field_elem)
			break;

		// calculate field length
		size_t field_len = bpstrlen(field_elem);

		// exit with failure due to 0 length field (UNACCEPTABLE)
		if(!field_len)
		{

			// create some stack space
			char tmp_buff[512];
			BP_ZERO_STACK(tmp_buff);

			// add message indicating that we found an empty field
			sprintf((char *) &tmp_buff, "BP_DBCheckSelectFieldNameList encountered an empty field (aka empty/zero-length string) at list index position %zu", field_name_list_idx);

			// add error and exit
			BP_DBAddErrorMsg(bpdb, (char *) &tmp_buff);
			return ERR_FAILURE;
		}

		// check if the string is reasonable
		BP_BOOL field_is_ok = BP_StringIsReasonableForSelectFieldSpecifierDB(field_elem, field_len);

		// if the field isn't ok, exit indicating failure
		if(field_is_ok == BP_FALSE)
		{

			// create some stack space
			char tmp_buff[1024+bpstrlen(field_elem)];
			BP_ZERO_STACK(tmp_buff);

			// add message indicating that we found an empty field
			sprintf((char *) &tmp_buff, "BP_DBCheckSelectFieldNameList reports '%s' failed sanity check in BP_StringIsReasonableForSelectFieldSpecifierDB", field_elem);

			// add error and exit
			BP_DBAddErrorMsg(bpdb, (char *) &tmp_buff);
			return ERR_FAILURE;
		}


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Separate Field from Table and Check %%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// create table and field
		char ** table_and_field = BP_SplitBuffer(field_elem, ".");
		if(!table_and_field)
		{
			// create some stack space
			char tmp_buff[1024+bpstrlen(field_elem)];
			BP_ZERO_STACK(tmp_buff);

			// add message indicating that we found an empty field
			sprintf((char *) &tmp_buff, "BP_DBCheckSelectFieldNameList reports '%s' could not be split into two parts (missing/bad-syntax period separator)", field_elem);

			// add error and exit
			BP_DBAddErrorMsg(bpdb, (char *) &tmp_buff);
			return ERR_FAILURE;
		}

		// dbg:
		// printf("\n Searching for Table and Field:: %s - %s", table_and_field[0], table_and_field[1]);

		// ensure the table is within the list of table names
		if(!BP_LinkLSearchForString(table_name_list, table_and_field[0], bpstrlen(table_and_field[0])))
		{

			// dbg:
			// printf("\n [!!] Failed to look up table %s", table_and_field[0]);
			// BP_DisplayLinkL(table_name_list);

			// create some stack space
			char tmp_buff[1024+bpstrlen(table_and_field[0])];
			BP_ZERO_STACK(tmp_buff);

			// add message indicating that we found an empty field
			sprintf((char *) &tmp_buff, "BP_DBCheckSelectFieldNameList reports that the table '%s' could not be found in table_name_list ", table_and_field[0]);

			// add error and exit
			BP_DBAddErrorMsg(bpdb, (char *) &tmp_buff);

			// destroy the pointer array
			BP_DestroyPointerArray((void **) table_and_field, 2);

			// destroy the table and field entry
			bpfree(table_and_field);

			// return indicating failure
			return ERR_FAILURE;

		}


		// dbg: display table and field
		// printf("\n Got Table and Field?: %s - %s", table_and_field[0], table_and_field[1]);


		// destroy the pointer array
		BP_DestroyPointerArray((void **) table_and_field, 2);

		// destroy the table and field entry
		bpfree(table_and_field);

		// increment the field name list index
		field_name_list_idx++;

	}

	// return indicating success
	return ERR_SUCCESS;

}

// Checks joins to make sure they make sense from a logical perspective.
BP_ERROR_T BP_DBCheckJoinElements
(
	P_BPDB             bpdb,
	P_BPDB_SELECT_JOIN joins,
	size_t             joins_n,
	P_BP_LINKL         table_name_list
)
{


	// ensure we have a bpdb
	if(!bpdb)
		return ERR_FAILURE;

	// ensure we have joins and list
	if(!joins)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBCheckJoinElements was called with a NULL joins parameter.");
		return ERR_FAILURE;
	}
	if(!joins_n)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBCheckJoinElements was called with a zero join count.");
		return ERR_FAILURE;
	}
	if(!table_name_list)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBCheckJoinElements was called with an empty table_name_list.");
		return ERR_FAILURE;
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check Join Table Names %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// declare join iterator
	size_t join_iter = 0;

	// walk joins
	for(; join_iter < joins_n; join_iter++)
	{


		if(!BP_LinkLSearchForString(table_name_list, joins[join_iter].table_name, bpstrlen(joins[join_iter].table_name)))
		{

			char tmp_buff[1024+bpstrlen(joins[join_iter].table_name)];
			BP_ZERO_STACK(tmp_buff);

			// add message indicating that we found an empty field
			sprintf((char *) &tmp_buff, "BP_DBCheckJoinElements reports table '%s' could not be found within the table_name_list", joins[join_iter].table_name);

			// add error and exit
			BP_DBAddErrorMsg(bpdb, (char *) &tmp_buff);
			return ERR_FAILURE;
		}

	}

	// return indicating success
	return ERR_SUCCESS;

}


// Checks "where" clause elements to make sure they make sense from a logical perspective.
BP_ERROR_T BP_DBCheckSelectClauseElements
(
	P_BPDB              bpdb,
	P_BPDB_SELECT_WHERE wheres,
	size_t              wheres_n,
	P_BP_LINKL          field_name_list
)
{

	// we must have a bpdb at all times (for error reporting)
	if(!bpdb)
		return ERR_FAILURE;

	// basic checks
	if(!wheres)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBCheckSelectClauseElements was called with a NULL 'wheres' parameter.");
		return ERR_FAILURE;
	}

	if(!wheres_n)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBCheckSelectClauseElements was called with a zero where count");
		return ERR_FAILURE;
	}
	if(!field_name_list)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBCheckSelectClauseElements was called with a NULL field_name_list parameter.");
		return ERR_FAILURE;
	}

	/*
		// create some stack space
		char tmp_buff[1024+bpstrlen(table_and_field[0])];
		BP_ZERO_STACK(tmp_buff);

		// add message indicating that we found an empty field
		sprintf((char *) &tmp_buff, "BP_DBCheckSelectFieldNameList reports that the table '%s' could not be found in table_name_list ", table_and_field[0]);

		// add error and exit
		BP_DBAddErrorMsg(bpdb, (char *) &tmp_buff);
		*/


	// where iter for walking
	size_t where_iter_n = 0;

	// walk the where clauses and run checks
	for(; where_iter_n < wheres_n; where_iter_n++)
	{

		// ensure we have a field name
		if(!wheres[where_iter_n].field_name)
		{
			// create some stack space
			char tmp_buff[1024];
			BP_ZERO_STACK(tmp_buff);

			// add message indicating that we found an empty field
			sprintf((char *) &tmp_buff, "BP_DBCheckSelectClauseElements reports that the field name at index position %zu was NULL.", where_iter_n);

			// add error and exit
			BP_DBAddErrorMsg(bpdb, (char *) &tmp_buff);
			return ERR_FAILURE;
		}

		// search for the where clause field in field name list
		if(!BP_LinkLSearchForString(field_name_list, wheres[where_iter_n].field_name, bpstrlen(wheres[where_iter_n].field_name)))
			return ERR_FAILURE;

		// if the value is a table, check it here
		if(wheres[where_iter_n].value_type == BPDB_SELECT_WHERE_VALUE_TYPE_TABLE_AND_COLUMN)
		{

			// check for field
			if(!BP_LinkLSearchForString(field_name_list, wheres[where_iter_n].value, bpstrlen(wheres[where_iter_n].value)))
			{
				// create some stack space
				char tmp_buff[1024+bpstrlen(wheres[where_iter_n].value)];
				BP_ZERO_STACK(tmp_buff);

				// add message indicating that we found an empty field
				sprintf((char *) &tmp_buff, "BP_DBCheckSelectClauseElements reports that the field name '%s' could not be found in field_name_list ", wheres[where_iter_n].value);

				// add error and exit
				BP_DBAddErrorMsg(bpdb, (char *) &tmp_buff);
				return ERR_FAILURE;
			}

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// This is a "master" routine that runs all checks necessary for
// generating a select info.  It analyzes all elements in a
// BPDB_SQLGEN_SELECT item, and returns ERR_FAILURE on bad
// values.  You should only ever need to call this function
// once to verify values.

BP_ERROR_T BP_DBCheckSelectInfoAndGenerateMetadataLists
(
		P_BPDB_SQLGEN        syngen,
		P_BPDB_SQLGEN_SELECT select_info
)
{


	// ensure we have a syntax generator
	if(!syngen)
		return ERR_FAILURE;

	// if we don't have a database exit with failure
	if(!syngen->bpdb)
		return ERR_FAILURE;

	// ensure we have select info to work with
	if(!select_info)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBCheckSelectInfoAndGenerateMetadataLists was called with a NULL select_info parameter");
		return ERR_FAILURE;
	}

	// ensure we have a table name
	if(!select_info->table_name)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBCheckSelectInfoAndGenerateMetadataLists was called with a NULL select_info->table_name parameter");
		return ERR_FAILURE;
	}

	// check limits if they were passed in
	if(select_info->limits)
	{
		if(select_info->limits->limit_low > select_info->limits->limit_high)
		{
			BP_DBAddErrorMsg(syngen->bpdb, "BP_DBCheckSelectInfoAndGenerateMetadataLists was called with a limit_low > limit_high");
			return ERR_FAILURE;
		}
	}


	// ensure table name is reasonable
	if(!BP_StringIsReasonableForTableNameDB(select_info->table_name, bpstrlen(select_info->table_name)))
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBCheckSelectInfoAndGenerateMetadataLists was called with an unreasonable table name (did not pass filter check)");
		return ERR_FAILURE;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Generate List of Possible Tables for Query %%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create array of all possible table names
	P_BP_LINKL table_name_list = BP_DBGenerateTableListFromSelectInfo(select_info);
	if(!table_name_list)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBCheckSelectInfoAndGenerateMetadataLists called BP_DBGenerateTableListFromSelectInfo but it failed to generate table list.");
		return ERR_FAILURE;
	}

	// generate field name list
	P_BP_LINKL field_name_list = BP_DBGenerateFieldNameListFromSelectInfo(select_info, table_name_list);
	if(!field_name_list)
	{

		// destroy the table list
		BP_DestroyLinkL(table_name_list, BP_TRUE);
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBCheckSelectInfoAndGenerateMetadataLists called BP_DBGenerateFieldNameListFromSelectInfo but it failed to generate field list.");
		// return indicating failure
		return ERR_FAILURE;

	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Field Name Checks %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure we have feild names
	if(!select_info->field_names)
	{

		// destroy the lists
		BP_DestroyLinkL(table_name_list, BP_TRUE);
		BP_DestroyLinkL(field_name_list, BP_TRUE);

		// add error message
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBCheckSelectInfoAndGenerateMetadataLists contains unpopulated select_info->field_names");

		// return indicating failure
		return ERR_FAILURE;

	}

	// gather field count
	size_t select_field_n = BP_CountNullTerminatedPtrArray(select_info->field_names);

	// run field name check via list
	if(!BP_DBCheckSelectFieldNameList(syngen->bpdb, field_name_list, table_name_list))
	{

		// destroy the lists
		BP_DestroyLinkL(table_name_list, BP_TRUE);
		BP_DestroyLinkL(field_name_list, BP_TRUE);

		// add error message
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBCheckSelectInfoAndGenerateMetadataLists called BP_DBCheckSelectFieldNameList but it failed.");

		// return err failure
		return ERR_FAILURE;

	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Where Clause Checks %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(select_info->select_where)
	{

		if(!BP_DBCheckSelectClauseElements(syngen->bpdb, select_info->select_where, select_info->select_where_entry_n, field_name_list))
		{

			// destroy the lists
			BP_DestroyLinkL(table_name_list, BP_TRUE);
			BP_DestroyLinkL(field_name_list, BP_TRUE);

			// add error message
			BP_DBAddErrorMsg(syngen->bpdb, "BP_DBCheckSelectInfoAndGenerateMetadataLists called BP_DBCheckSelectClauseElements but it failed.");


			// return err failure
			return ERR_FAILURE;

		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Join Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(select_info->join_on && select_info->join_on_entry_n)
	{

		// run checks
		if(!BP_DBCheckJoinElements(syngen->bpdb, select_info->join_on, select_info->join_on_entry_n, table_name_list))
		{

			// destroy the lists
			BP_DestroyLinkL(table_name_list, BP_TRUE);
			BP_DestroyLinkL(field_name_list, BP_TRUE);

			// add error message
			BP_DBAddErrorMsg(syngen->bpdb, "BP_DBCheckSelectInfoAndGenerateMetadataLists called BP_DBCheckJoinElements but it failed.");

			// exit indicating failure
			return ERR_FAILURE;

		}

	}

	// set field name list and table name list before returning
	select_info->field_name_list = field_name_list;
	select_info->table_name_list = table_name_list;

	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Schema Check Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to check fieldnames against schema
BP_ERROR_T BP_DBCheckFieldNamesAgainstSchema(P_BPDB_SQLGEN syngen, P_BP_LINKL field_name_list)
{

	// basic null checks
	if(!syngen)
		return ERR_FAILURE;
	if(!syngen->bpdb)
		return ERR_FAILURE;

	if(!field_name_list)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBCheckFieldNamesAgainstSchema was called with a NULL field_name_list parameter");
		return ERR_FAILURE;
	}

	// attempt to get the first item
	P_BP_LINKL_DATA field_name_iter = TAILQ_FIRST(&field_name_list->head);

	// ensure we have a first item
	if(!field_name_iter)
		return ERR_FAILURE;


	// looks for a pointer in the item
	TAILQ_FOREACH(field_name_iter, &field_name_list->head, linkl_data)
	{


		// only work with strndup/strdup types
		if
		(
			field_name_iter->data_type != BP_MEMPROF_ALLOC_TYPE_STRDUP   &&
			field_name_iter->data_type != BP_MEMPROF_ALLOC_TYPE_STRNDUP
		)
		{
			BP_DBAddErrorMsg(syngen->bpdb, "BP_DBCheckFieldNamesAgainstSchema has a link data type which isn't a string.  Corruption detected.");
			return ERR_FAILURE;
		}


		// attempt to lookup a column based on the field name
		if(!BP_DBLookupTableColumnByJoinedFieldName(syngen->bpdb, (char *) field_name_iter->data))
		{

			// create some dynamic stack data
			char tmp_str[bpstrlen((char*) field_name_iter->data)+512];

			// add error message
			sprintf((char *) &tmp_str, "BP_DBCheckFieldNamesAgainstSchema reports that '%s' fails lookup in BP_DBLookupTableColumnByJoinedFieldName", (char *) field_name_iter->data);
			BP_DBAddErrorMsg(syngen->bpdb, (char*) &tmp_str);

			// exit routine
			return ERR_FAILURE;
		}


		// attempt to lookup column metadata based on field name (required to ensure
		// that metadata exists in the table being analyzed)
		if(!BP_DBLookupTableColumnMetadataFromJoinedFieldName(syngen->bpdb, (char *) field_name_iter->data))
		{

			// create some dynamic stack data
			char tmp_str[bpstrlen((char*) field_name_iter->data)+512];

			// add error message
			sprintf((char *) &tmp_str, "BP_DBCheckFieldNamesAgainstSchema reports that '%s' fails METADATA lookup in BP_DBLookupTableColumnMetadataFromJoinedFieldName", (char *) field_name_iter->data);
			BP_DBAddErrorMsg(syngen->bpdb, "BP_DBCheckFieldNamesAgainstSchema failed to lookup .");

			// exit routine
			return ERR_FAILURE;

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// attempt to check tablenames against schema
BP_ERROR_T BP_DBCheckTableNamesAgainstSchema(P_BPDB_SQLGEN syngen, P_BP_LINKL table_name_list)
{

	// basic null checks
	if(!syngen)
		return ERR_FAILURE;
	if(!syngen->bpdb)
		return ERR_FAILURE;
	if(!table_name_list)
		return ERR_FAILURE;

	// attempt to get the first item
	P_BP_LINKL_DATA table_name_iter = TAILQ_FIRST(&table_name_list->head);

	// ensure we have a first item
	if(!table_name_iter)
		return ERR_FAILURE;

	// looks for a pointer in the item
	TAILQ_FOREACH(table_name_iter, &table_name_list->head, linkl_data)
	{


		// only work with strndup/strdup types
		if
		(
			table_name_iter->data_type != BP_MEMPROF_ALLOC_TYPE_STRDUP   &&
			table_name_iter->data_type != BP_MEMPROF_ALLOC_TYPE_STRNDUP
		)
		{

			// return indicating failure
			return ERR_FAILURE;

		}

		// lookup the table and make sure it exists
		if(!BP_DBLookupTableByName(syngen->bpdb, (char *) table_name_iter->data))
			return ERR_FAILURE;

	}

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Parameter Addition %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Adds a select parameter to a select info query.  This shouldn't be used directly.  This is used internally
// by BP_DBSGSelectFrom to add parameters inline.
BP_ERROR_T BP_DBAddSelectWhereParameterToSelectInfo
(
	P_BPDB_SQLGEN        syngen,
	P_BPDB_SQLGEN_SELECT select_info,
	P_BPDB_SELECT_WHERE  select_where
)
{


	// run basic parameter checks
	if(!syngen)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectWhereParameterToSelectInfo provided syngen parameter was null.");
		return ERR_FAILURE;
	}
	if(!syngen->bpdb)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectWhereParameterToSelectInfo provided syngen had no bpdb member (syngen->bpdb == NULL).");
		return ERR_FAILURE;
	}

	if(!select_info)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectWhereParameterToSelectInfo was called with a NULL select_info parameter.");
		return ERR_FAILURE;
	}
	if(!select_where)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectWhereParameterToSelectInfo was called with a NULL select_where parameter.");
		return ERR_FAILURE;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Allocate Memory for Bind %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Create TQ if necessary
	if(!select_info->bind_parameters_tq_allocator)
		select_info->bind_parameters_tq_allocator = BP_CreateLinkL(".", BP_TRUE);

	// bind parameters
	select_info->bind_parameters = (MYSQL_BIND *) tq_realloc
	(
		select_info->bind_parameters_tq_allocator,
		select_info->bind_parameters,
		sizeof(MYSQL_BIND) * (select_info->bind_parameters_n + 1),
		BPLN
	);

	// ensure you have a bind parameter allocated
	if(!select_info->bind_parameters)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectWhereParameterToSelectInfo failed to allocate space for bind parameters.");
		return ERR_FAILURE;
	}


	// set the bind parameter reference (for more readable code below)
	MYSQL_BIND * new_bind_parameter = &select_info->bind_parameters[select_info->bind_parameters_n];

	// zero out the bind parameter
	memset(new_bind_parameter, 0x00, sizeof(MYSQL_BIND));

	// increment the bind parameter count
	select_info->bind_parameters_n++;




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Lookup Column Metadata %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to lookup column metadata for the field
	P_BPDB_SQL_TABLE_COLUMN_METADATA column_metadata = BP_DBLookupTableColumnMetadataFromJoinedFieldName(syngen->bpdb, select_where->field_name);
	if(!column_metadata)
	{

		// return indicating failure
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectWhereParameterToSelectInfo called BP_DBLookupTableColumnMetadataFromJoinedFieldName but it failed.");
		return ERR_FAILURE;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Bind Parameter Type from Metadata %%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	/*

	// switch on the enum type
	switch(column_metadata->mariadb_column_metadata.data_type_as_enum)
	{

		case BPDB_MARIADB_COLUMN_TYPE_TINYINT:
			new_bind_parameter->buffer_type = MYSQL_TYPE_TINY;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_SMALLINT:
			new_bind_parameter->buffer_type = MYSQL_TYPE_SHORT;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_MEDIUMINT:
			new_bind_parameter->buffer_type = MYSQL_TYPE_INT24;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_INT:
			new_bind_parameter->buffer_type = MYSQL_TYPE_LONG;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_BIGINT:
			new_bind_parameter->buffer_type = MYSQL_TYPE_LONGLONG;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_DECIMAL:
			new_bind_parameter->buffer_type = MYSQL_TYPE_DECIMAL;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_FLOAT:
			new_bind_parameter->buffer_type = MYSQL_TYPE_FLOAT;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_DOUBLE:
			new_bind_parameter->buffer_type = MYSQL_TYPE_DOUBLE;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_BIT:
			new_bind_parameter->buffer_type = MYSQL_TYPE_BIT;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_CHAR:
			new_bind_parameter->buffer_type = MYSQL_TYPE_STRING;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_VARCHAR:
			new_bind_parameter->buffer_type = MYSQL_TYPE_VAR_STRING;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_BINARY:
			new_bind_parameter->buffer_type = MYSQL_TYPE_STRING;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_VARBINARY:
			new_bind_parameter->buffer_type = MYSQL_TYPE_VAR_STRING;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_TINYBLOB:
			new_bind_parameter->buffer_type = MYSQL_TYPE_BLOB;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_BLOB:
			break;
		case BPDB_MARIADB_COLUMN_TYPE_MEDIUMBLOB:
			new_bind_parameter->buffer_type = MYSQL_TYPE_BLOB;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_LONGBLOB:
			new_bind_parameter->buffer_type = MYSQL_TYPE_BLOB;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_TINYTEXT:
			new_bind_parameter->buffer_type = MYSQL_TYPE_BLOB;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_TEXT:
			new_bind_parameter->buffer_type = MYSQL_TYPE_BLOB;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_MEDIUMTEXT:
			new_bind_parameter->buffer_type = MYSQL_TYPE_BLOB;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_LONGTEXT:
			new_bind_parameter->buffer_type = MYSQL_TYPE_BLOB;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_ENUM:
			new_bind_parameter->buffer_type = MYSQL_TYPE_ENUM;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_DATE:
			new_bind_parameter->buffer_type = MYSQL_TYPE_DATE;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_TIME:
			new_bind_parameter->buffer_type = MYSQL_TYPE_TIME;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_DATETIME:
			new_bind_parameter->buffer_type = MYSQL_TYPE_DATETIME;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_TIMESTAMP:
			new_bind_parameter->buffer_type = MYSQL_TYPE_TIMESTAMP;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_YEAR:
			new_bind_parameter->buffer_type = MYSQL_TYPE_YEAR;
			break;

		// if we failed a type match, exit with failure
		default:
			BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectWhereParameterToSelectInfo attempted to lookup metadata type, but failed.  This should never happen.");
			return ERR_FAILURE;

	}

	*/

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Perform Value Conversions %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// selected/converted values
	void * selected_value       = NULL;
	size_t selected_value_len = 0;

	// temporary long integer pointer used for conversion
	long int * tmp_long_int_ptr = NULL;



	// switch on the value type
	switch(select_where->value_type)
	{

		case BPDB_SELECT_WHERE_VALUE_TYPE_UNSET:
			break;

		/*
		// convert integers
		case BPDB_SELECT_WHERE_VALUE_TYPE_NEGATIVE_INTEGER:
			tmp_long_int_ptr = (long int *) select_where->value;
			selected_value = tmp_long_int_ptr;
			printf("\n [NEGATIVE INT] THIS IS THE FINAL VALUE AS A NEGATIVE INTEGER: %s", tmp_long_int_ptr);
			new_bind_parameter->buffer_type = MYSQL_TYPE_LONG;
			break;

		case BPDB_SELECT_WHERE_VALUE_TYPE_POSITIVE_INTEGER:
			tmp_long_int_ptr = (long int *) select_where->value;

			printf("\n [POSITIVE INT] THIS IS THE FINAL VALUE AS A POSITIVE INTEGER: %s", tmp_long_int_ptr);
			selected_value = tmp_long_int_ptr;
			new_bind_parameter->buffer_type = MYSQL_TYPE_LONG;
			break;
		*/

		// hex strings must be converted to
		case BPDB_SELECT_WHERE_VALUE_TYPE_HEX_STRING:
		case BPDB_SELECT_WHERE_VALUE_TYPE_POSITIVE_INTEGER:
		case BPDB_SELECT_WHERE_VALUE_TYPE_NEGATIVE_INTEGER:
		case BPDB_SELECT_WHERE_VALUE_TYPE_QUOTED_LITERAL:
		case BPDB_SELECT_WHERE_VALUE_TYPE_TABLE_AND_COLUMN:
			printf("\n [QUOTED STRING] THIS IS THE FINAL VALUE AS A QUOTED STRING: %s", select_where->value);
			selected_value = select_where->value;
			new_bind_parameter->buffer_type = MYSQL_TYPE_BLOB;
			break;

		// break on default
		default:
			break;

	}

	// set selected value
	if(!selected_value)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectWhereParameterToSelectInfo had value type conversion fail.");
		return ERR_FAILURE;
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Convert Parameters %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// not supported
	// case MYSQL_TYPE_ENUM:
	// case MYSQL_TYPE_BIT:
	// case MYSQL_TYPE_DECIMAL:


	int32_t tmp_converted_int32 = 0;

	// 64bit int
	int     tmp_converted_int64 = 0;

	switch(new_bind_parameter->buffer_type)
	{


		case MYSQL_TYPE_TINY:
		case MYSQL_TYPE_SHORT:
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONG:
			selected_value_len = 4;
			break;

		case MYSQL_TYPE_LONGLONG:
			selected_value_len = 8;
			break;

		case MYSQL_TYPE_FLOAT:
			selected_value_len = 8;
			break;
		case MYSQL_TYPE_DOUBLE:
			selected_value_len = 8;
			break;

		case MYSQL_TYPE_STRING:
		case MYSQL_TYPE_VAR_STRING:
		case MYSQL_TYPE_BLOB:
			selected_value_len = select_where->value_len;
			break;

		// add date fields
		case MYSQL_TYPE_DATE:
		case MYSQL_TYPE_TIME:
		case MYSQL_TYPE_DATETIME:
		case MYSQL_TYPE_TIMESTAMP:
		case MYSQL_TYPE_YEAR:
			selected_value_len = sizeof(MYSQL_TIME);
			break;

		default:
			break;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Parameter Data %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set value and value_len
	new_bind_parameter->buffer        = select_where->value;
	new_bind_parameter->buffer_length = select_where->value_len;


	printf("\n Got the Buffer Length at end of addition?: %zu", new_bind_parameter->buffer_length);
	printf("\n Got here with value %u: %s (buffer type: %u)", select_where->value_len, select_where->value, new_bind_parameter->buffer_type);
	BP_DBAddNoticeMsg(syngen->bpdb, "BPDB Syntax generator got here ok?!", NULL);

	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Result Addition %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Attempts to add a result to the select info structure.  Used internally.
BP_ERROR_T BP_DBAddSelectResultToSelectInfo
(
	P_BPDB_SQLGEN        syngen,
	P_BPDB_SQLGEN_SELECT select_info,
	char               * select_field_name
)
{

	// ensure we have a syntax generator and a bpdb handle
	if(!syngen)
		return ERR_FAILURE;
	if(!syngen->bpdb)
		return ERR_FAILURE;

	// run basic parameter checks
	if(!select_info)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectResultToSelectInfo was called with a NULL select_info parameter.");
		return ERR_FAILURE;
	}
	if(!select_field_name)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectResultToSelectInfo was called with a NULL select_field_name parameter.");
		return ERR_FAILURE;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Allocate Memory for Bind %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Create TQ if necessary
	if(!select_info->bind_results_tq_allocator)
	{
		select_info->bind_results_tq_allocator = BP_CreateLinkL(".", BP_TRUE);
		select_info->bind_results = NULL;
	}

	// bind results
	select_info->bind_results = (MYSQL_BIND *) tq_realloc
	(
		select_info->bind_results_tq_allocator,
		select_info->bind_results,
		sizeof(MYSQL_BIND) * (select_info->bind_results_n + 1),
		BPLN
	);

	// ensure you have a bind results allocated
	if(!select_info->bind_results)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectResultToSelectInfo attempted to reallocate space for new entry, but allocation failed.");
		return ERR_FAILURE;
	}


	// set the bind results reference (for more readable code below)
	MYSQL_BIND * new_bind_result = &select_info->bind_results[select_info->bind_results_n];

	// zero out the new result structure after realloc
	memset(new_bind_result, 0x00, sizeof(MYSQL_BIND));

	// increment the bind results count
	select_info->bind_results_n++;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Lookup Column Metadata %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to lookup column metadata for the field
	P_BPDB_SQL_TABLE_COLUMN_METADATA column_metadata = BP_DBLookupTableColumnMetadataFromJoinedFieldName(syngen->bpdb, select_field_name);
	if(!column_metadata)
	{

		// return indicating failure
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectResultToSelectInfo called BP_DBLookupTableColumnMetadataFromJoinedFieldName but it failed.");
		return ERR_FAILURE;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Bind Result Type from Metadata %%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// switch on the enum type
	switch(column_metadata->mariadb_column_metadata.data_type_as_enum)
	{

		case BPDB_MARIADB_COLUMN_TYPE_TINYINT:
			new_bind_result->buffer_type = MYSQL_TYPE_TINY;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_SMALLINT:
			new_bind_result->buffer_type = MYSQL_TYPE_SHORT;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_MEDIUMINT:
			new_bind_result->buffer_type = MYSQL_TYPE_INT24;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_INT:
			new_bind_result->buffer_type = MYSQL_TYPE_LONG;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_BIGINT:
			new_bind_result->buffer_type = MYSQL_TYPE_LONGLONG;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_DECIMAL:
			new_bind_result->buffer_type = MYSQL_TYPE_DECIMAL;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_FLOAT:
			new_bind_result->buffer_type = MYSQL_TYPE_FLOAT;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_DOUBLE:
			new_bind_result->buffer_type = MYSQL_TYPE_DOUBLE;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_BIT:
			new_bind_result->buffer_type = MYSQL_TYPE_BIT;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_CHAR:
			new_bind_result->buffer_type = MYSQL_TYPE_STRING;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_VARCHAR:
			new_bind_result->buffer_type = MYSQL_TYPE_VAR_STRING;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_BINARY:
			new_bind_result->buffer_type = MYSQL_TYPE_STRING;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_VARBINARY:
			new_bind_result->buffer_type = MYSQL_TYPE_VAR_STRING;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_TINYBLOB:
			new_bind_result->buffer_type = MYSQL_TYPE_BLOB;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_BLOB:
			break;
		case BPDB_MARIADB_COLUMN_TYPE_MEDIUMBLOB:
			new_bind_result->buffer_type = MYSQL_TYPE_BLOB;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_LONGBLOB:
			new_bind_result->buffer_type = MYSQL_TYPE_BLOB;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_TINYTEXT:
			new_bind_result->buffer_type = MYSQL_TYPE_BLOB;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_TEXT:
			new_bind_result->buffer_type = MYSQL_TYPE_BLOB;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_MEDIUMTEXT:
			new_bind_result->buffer_type = MYSQL_TYPE_BLOB;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_LONGTEXT:
			new_bind_result->buffer_type = MYSQL_TYPE_BLOB;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_ENUM:
			new_bind_result->buffer_type = MYSQL_TYPE_ENUM;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_DATE:
			new_bind_result->buffer_type = MYSQL_TYPE_DATE;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_TIME:
			new_bind_result->buffer_type = MYSQL_TYPE_TIME;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_DATETIME:
			new_bind_result->buffer_type = MYSQL_TYPE_DATETIME;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_TIMESTAMP:
			new_bind_result->buffer_type = MYSQL_TYPE_TIMESTAMP;
			break;
		case BPDB_MARIADB_COLUMN_TYPE_YEAR:
			new_bind_result->buffer_type = MYSQL_TYPE_YEAR;
			break;

		// if we failed a type match, exit with failure
		default:
			BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectResultToSelectInfo column type lookup failed.  This should never happen.");
			return ERR_FAILURE;

	}

	// NOTE: Do not add hash table entry here.  If you do, due to potential reallocs
	//       when adding new entries, the hash table can wind up with bad entries.

	// return indicating success
	return ERR_SUCCESS;

}


// Attempts to create a hash table within the select_info structure by associating
// potentially retrieved tables
BP_ERROR_T BP_DBAddSelectResultReferences
(
	P_BPDB_SQLGEN        syngen,
	P_BPDB_SQLGEN_SELECT select_info
)
{

	// ensure we got a syntax generator and select info
	if(!syngen)
		return ERR_FAILURE;
	if(!syngen->bpdb)
		return ERR_FAILURE;

	if(!select_info)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectResultReferences was called with a NULL select_info parameter");
		return ERR_FAILURE;
	}

	// return indicating failure if results don't exist
	if(!select_info->bind_results)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectResultReferences was called with a NULL select_info->bind_results parameter");
		return ERR_FAILURE;
	}
	if(!select_info->bind_results_n)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectResultReferences was called with a NULL select_info->bind_results_n count");
		return ERR_FAILURE;
	}


	// results iter
	size_t results_iter = 0;
	for
	(
		;
		results_iter < select_info->bind_results_n;
		results_iter++
	)
	{

		// break if we've hit our cap
		if(!select_info->field_names[results_iter])
			break;

		// add result to hash table
		if
		(
			!BP_DBAddSelectResultReferenceToResultHashTable
			(
				syngen,
				select_info,
				select_info->field_names[results_iter],
				&select_info->bind_results[results_iter]
			)
		)
		{
			BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectResultReferences attempted to call BP_DBAddSelectResultReferenceToResultHashTable but it failed.");
		}

	}

	// return indicating success
	return ERR_SUCCESS;

}


// Attempt to add a result entry to match with a result
BP_ERROR_T BP_DBAddSelectResultReferenceToResultHashTable
(
	P_BPDB_SQLGEN        syngen,
	P_BPDB_SQLGEN_SELECT select_info,
	char               * select_field_name,
	MYSQL_BIND *         result_entry
)
{

	// run basic null checks
	if(!syngen)
		return ERR_FAILURE;
	if(!syngen->bpdb)
		return ERR_FAILURE;

	if(!select_info)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectResultReferenceToResultHashTable was called with a NULL select_info parameter.");
		return ERR_FAILURE;
	}

	if(!select_field_name)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectResultReferenceToResultHashTable was called with a NULL select_field_name parameter.");
		return ERR_FAILURE;
	}

	if(!result_entry)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectResultReferenceToResultHashTable was called with a NULL result_entry parameter.");
		return ERR_FAILURE;
	}


	printf("\n [dbg] Attempting to add to hash table here ptr: %p", result_entry);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Entry to Hash Registry (for name lookups) %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure we have a hash table registry
	if(!select_info->fetched_record_hreg)
	{

		// create registry just in time
		select_info->fetched_record_hreg = BP_CreateHashTableRegistry();

	}

	// lock critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(select_info->fetched_record_hreg);

	// set table path
	char *table_path[3];
	BP_ZERO_STACK(table_path);

	// set table path
	table_path[0] = select_field_name;

	// attempt to lookup table from the previous path
	P_BP_HASH_TABLE_KEYED_ENTRY table_lookup = BP_HashRegLookupTableByPath(select_info->fetched_record_hreg, table_path);

	// if the item already exists, return indicating failure
	if(table_lookup)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectResultReferenceToResultHashTable attempted to call BP_HashRegLookupTableByPath on select_field_name but it failed.");
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(select_info->fetched_record_hreg, ERR_FAILURE);
	}


	// -- Attempt to Add Table -----------

	// add table to the db
	BP_ERROR_T ret_val = BP_HashRegAddTableByPath(select_info->fetched_record_hreg, (char **) &table_path);
	if(!ret_val)
	{

		// leave critical section here (necessary)
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectResultReferenceToResultHashTable attempted to call BP_HashRegAddTableByPath on select_field_name but it failed.");
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(select_info->fetched_record_hreg, ERR_FAILURE);

	}

	// try lookup again after addition
	table_lookup = BP_HashRegLookupTableByPath(select_info->fetched_record_hreg, table_path);

	// if the lookup fails, we have an erroneous condition
	if(!table_lookup)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectResultReferenceToResultHashTable attempted to call BP_HashRegLookupTableByPath after select_field_name addition, but it (lookup) failed.");
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(select_info->fetched_record_hreg, ERR_FAILURE);
	}


	// -- Attempt to Add Data to Table -----------

	// attempt to add metadata entry to the table
	BP_HashRegAddDataToTable
	(
		select_info->fetched_record_hreg,
		table_lookup,
		"result_data_ptr", /* in BP-DB.h */
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(MYSQL_BIND *)
	);


	// set lookup path
	char * lookup_path[] = {select_field_name, "result_data_ptr", NULL};

	// retrive table metadata by path
	P_BP_HASH_TABLE_KEYED_ENTRY result_data_ptr_lookup = BP_HashRegLookupTableEntryByPath(select_info->fetched_record_hreg, (char **) &lookup_path);
	if(!result_data_ptr_lookup)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBAddSelectResultReferenceToResultHashTable attempted to add result data, but it failed to add to hreg.");
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(select_info->fetched_record_hreg, ERR_FAILURE);
	}


	printf("\n Using result entry:!: %p", result_entry);
	// result_data_ptr_lookup->data = (void *) result_entry;
	// copy in the pointer
	memcpy(result_data_ptr_lookup->data, &result_entry, sizeof(MYSQL_BIND * ));

	printf("\n Using data after?: %p", result_data_ptr_lookup->data);


	MYSQL_BIND ** tmp_ptr = (MYSQL_BIND**) result_data_ptr_lookup->data;

	printf("\n WATPTRDISPLAY HERE: %p", *tmp_ptr);

	// = result_entry;
	// return indicating success
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(select_info->fetched_record_hreg, ERR_SUCCESS);

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Dataset Generators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Creates a searchable list of tables which represent ALL tables which could be selected
// from in the query.  This information is verified AFTER generation.  This simly generates
// a list of unverified tables.
P_BP_LINKL BP_DBGenerateTableListFromSelectInfo
(
	P_BPDB_SQLGEN_SELECT select_info
)
{

	// ensure we have select info
	if(!select_info)
		return NULL;

	// create new linkl
	P_BP_LINKL tq = BP_CreateLinkL(".", BP_TRUE);


	// NOTE: the tq_strdup routines only work on reasonable/printable strings, so sanitization
	//       occurs here very basically.  Secondary sanitization occurs during the "check" process.


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add From Table Name %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add table name if it's set
	if(select_info->table_name)
		tq_strdup_unique(tq, select_info->table_name, BPLN);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add From Group By %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add group by table if one exists
	if(select_info->group_by)
	{

		// set table offset
		char **table_offs = BP_SplitBufferBin(select_info->group_by->field, bpstrlen(select_info->group_by->field), ".", 1);
		if(table_offs)
		{
			tq_strdup_unique(tq, table_offs[0], BPLN);
			BP_DestroyPointerArrayNullTerminated((void **) table_offs);

		}
		else
		{
			tq_strdup_unique(tq, select_info->group_by->field, BPLN);
		}

	}




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add From Join Info %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// add in joined tables
	size_t n = 0;
	if(select_info->join_on_entry_n && select_info->join_on)
	{

		for(; n < select_info->join_on_entry_n; n++)
		{

			tq_strdup_unique(tq, select_info->join_on[n].table_name, BPLN);

		}

	}

	// dbg: display the linkl for debugging if desired
	//BP_DisplayLinkL(tq);
	BP_LinkLPrintStringElementsToStdout(tq);

	// return null
	return tq;

}


// Generates a searchable list of all field names which represent all tables which could be selected
P_BP_LINKL BP_DBGenerateFieldNameListFromSelectInfo
(
		P_BPDB_SQLGEN_SELECT select_info,
		P_BP_LINKL           table_name_list
)
{


	// ensure we have select info
	if(!select_info)
		return NULL;

	// ensure we have filed names
	if(!select_info->field_names)
		return NULL;

	// NOTE: Collect unsplit field names first, then submit array to BP_DBCheckSelectFieldNames
	//       further validation.


	// create new linkl
	P_BP_LINKL tq = BP_CreateLinkL(".", BP_TRUE);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Normal Field Names %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// field iterator
	size_t field_iter = 0;

	// search for field iter
	for
	(
		;
		select_info->field_names[field_iter];
		field_iter++
	)
	{

		// set the unique
		tq_strdup_unique(tq, select_info->field_names[field_iter], BPLN);

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add From Where-clause Info %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// iterate through where info and add tables
	if(select_info->select_where && select_info->select_where_entry_n)
	{

		// set where iterator
		size_t where_iter = 0;

		// walk where items
		for(; where_iter < select_info->select_where_entry_n; where_iter++)
		{

			// append field info
			tq_strdup_unique(tq, select_info->select_where[where_iter].field_name, BPLN);

			// if the value is a table name, add it here
			if(select_info->select_where[where_iter].value_type == BPDB_SELECT_WHERE_VALUE_TYPE_TABLE_AND_COLUMN)
			{

				// add value if it's a table name
				tq_strdup_unique(tq, select_info->select_where[where_iter].value, BPLN);

			}

		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Fields from Joins %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add fields from joins
	if(select_info->join_on && select_info->join_on_entry_n)
	{

		// join iter
		size_t join_iter = 0;

		// go through join entries
		for
		(
			;
			join_iter < select_info->join_on_entry_n;
			join_iter++
		)
		{

			// add joins
			tq_strdup_unique(tq, select_info->join_on[join_iter].front_join, BPLN);
			tq_strdup_unique(tq, select_info->join_on[join_iter].tail_join, BPLN);

		}

	}

	// dbg: display the linkl for debugging if desired
	// BP_DisplayLinkL(tq);
	BP_LinkLPrintStringElementsToStdout(tq);

	// return null
	return tq;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Destroy / Free Dangling Data Routines %%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This cleans out all dangling data that could possibly be generated as a result of
// the attempted or comleted prepare of a select statement.  It should be noted that
// all potentially dangling memory isn't allocated in the master hash table.  This is
// to prevent any need for contentious locking of the table itself.
BP_ERROR_T BP_DBSelectInfoCleanResidualData
(
	P_BPDB_SQLGEN_SELECT select_info,
	BP_BOOL              destroy_bind_results,
	BP_BOOL              destroy_bind_parameters,
	BP_BOOL              destroy_field_name_list,
	BP_BOOL              destroy_table_name_list
)
{

	// ensure we have select info to work with
	if(!select_info)
		return ERR_FAILURE;

	// PROTECTED FIELDS / DO NOT DESTROY:
	// fields to be selected
	//	char **                field_names;
	//
	//	// table to select from
	//	char *                 table_name;
	//
	// where clause
	//	P_BPDB_SELECT_WHERE    select_where;
	//	size_t                 select_where_entry_n;
	//
	// limits to impose on the query tail
	//	P_BPDB_SELECT_LIMITS   limits;
	//
	// join clauses
	//	P_BPDB_SELECT_JOIN     join_on;
	//	size_t                 join_on_entry_n;
	//
	//	// order by clause
	//	P_BPDB_SELECT_ORDER_BY order_by;
	//
	// group by clause
	//	P_BPDB_SELECT_GROUP_BY group_by;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Cleanup Internally Allocated Lists %%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// destroy table name list
	if(destroy_table_name_list)
	if(select_info->table_name_list)
	{
		BP_DestroyLinkL(select_info->table_name_list, BP_TRUE);
		select_info->table_name_list = NULL;
	}

	// destroy field name list
	if(destroy_field_name_list)
	if(select_info->field_name_list)
	{
		BP_DestroyLinkL(select_info->field_name_list, BP_TRUE);
		select_info->field_name_list = NULL;

	}

	// destroy the bind parameters list
	if(destroy_bind_parameters)
	if(select_info->bind_parameters_tq_allocator)
	{
		BP_DestroyLinkL(select_info->bind_parameters_tq_allocator, BP_TRUE);
		select_info->bind_parameters_tq_allocator = NULL;
		select_info->bind_parameters              = NULL;
		select_info->bind_parameters_n            = 0;
	}


	// destroy the bind results list
	if(destroy_bind_results)
	if(select_info->bind_results_tq_allocator)
	{
		BP_DestroyLinkL(select_info->bind_results_tq_allocator, BP_TRUE);
		select_info->bind_results_tq_allocator = NULL;
		select_info->bind_results              = NULL;
		select_info->bind_results_n            = 0;
	}



	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// display select info
BP_ERROR_T BP_DBDisplaySelectInfo(P_BPDB_SQLGEN_SELECT select_info)
{

	if(!select_info)
	{
		printf("\n [!!] BP_DBDisplaySelectInfo called with a null parameter.");
		return ERR_FAILURE;
	}

	// display select info
	printf("\n [+] Select Info %p", select_info);

	// display field names
	if(select_info->field_names)
	{

		printf("\n\t Field Names: ");
		size_t field_name_iter = 0;
		for(; select_info->field_names[field_name_iter]; field_name_iter++)
		{
			printf("\n\t   - %s", select_info->field_names[field_name_iter]);
		}

	}

	// display table name
	printf("\n\t table_name: %s", select_info->table_name);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Select-Where Items %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// display select wheres
	if(select_info->select_where)
	{

		size_t select_where_iter = 0;
		for
		(
			;
			select_where_iter < select_info->select_where_entry_n;
			select_where_iter++
		)
		{

			// display select info
			BP_DBDisplaySelectInfoSelectWhere(&select_info->select_where[select_where_iter], 1);

		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Limits %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(select_info->limits)
	{

		// display low and high limit
		printf("\n\t limit_low/limit_high: %u - %u", select_info->limits->limit_low, select_info->limits->limit_high);

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Join On %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(select_info->join_on)
	{

		size_t join_on_iter = 0;
		for(; join_on_iter < select_info->join_on_entry_n; join_on_iter++)
		{
			BP_DBDisplaySelectInfoSelectJoin(&select_info->join_on[join_on_iter], 1);
		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Order By %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(select_info->order_by)
	{
		printf("\n\t order by field:        %s", select_info->order_by->field);
		printf("\n\t order sort designator: %s", select_info->order_by->sort_designator);
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Group By %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(select_info->group_by)
	{
		printf("\n\t group by: todo");
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Bind Parameters %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// display bind parameters
	 BP_DBDisplaySelectInfoMariadbBindParameters(select_info);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Bind Results %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// display bind results
	BP_DBDisplaySelectInfoMariadbBindResults(select_info);

	// return indicating success
	return ERR_SUCCESS;

}


// display select where info
BP_ERROR_T BP_DBDisplaySelectInfoSelectWhere(P_BPDB_SELECT_WHERE select_where, size_t tab_depth)
{

	// ensure we have an element to display
	if(!select_where)
		return ERR_FAILURE;

	// set tab_buff init to empty string
	char * tab_buff = "";

	// create a linkl
	P_BP_LINKL tab_linkl = BP_CreateLinkL(".", BP_TRUE);

	// if tab depth is set, generate tab string
	if(tab_depth)
	{

		// add tab elements
		size_t n = 0;
		for(; n < tab_depth; n++)
		{
			tq_strdup(tab_linkl, "\t", BPLN);
		}

		// create flat buffer
		BP_LinkLFlattenJoinStringElements(tab_linkl);

		// set the tab buffer
		tab_buff = (char *) tab_linkl->flat_buff;
	}


	printf("\n %s [+] Select Where Item %p", tab_buff, select_where);
	printf("\n %s     conjunction:          %s", tab_buff, select_where->conjunction);
	printf("\n %s     field_name:           %s", tab_buff, select_where->field_name);
	printf("\n %s     limiter:              %s", tab_buff, select_where->limiter);
	printf("\n %s     value_len:            %u", tab_buff, select_where->value_len);
	printf("\n %s     value_type:           ",   tab_buff);

	// display type
	switch(tab_buff, select_where->value_type)
	{

		case BPDB_SELECT_WHERE_VALUE_TYPE_UNSET:
			printf("BPDB_SELECT_WHERE_VALUE_TYPE_UNSET");
			break;

		case BPDB_SELECT_WHERE_VALUE_TYPE_NEGATIVE_INTEGER:
			printf("BPDB_SELECT_WHERE_VALUE_TYPE_NEGATIVE_INTEGER");
			printf("\n %s     value:                %ld", tab_buff, (long int) *select_where->value);
			break;

		case BPDB_SELECT_WHERE_VALUE_TYPE_POSITIVE_INTEGER:
			printf("BPDB_SELECT_WHERE_VALUE_TYPE_POSITIVE_INTEGER");
			printf("\n %s     value:                %ld", tab_buff, (long int) *select_where->value);
			break;

		case BPDB_SELECT_WHERE_VALUE_TYPE_HEX_STRING:
			printf("BPDB_SELECT_WHERE_VALUE_TYPE_HEX_STRING");
			printf("\n %s     value:                %s", tab_buff, select_where->value);
			break;

		case BPDB_SELECT_WHERE_VALUE_TYPE_QUOTED_LITERAL:
			printf("BPDB_SELECT_WHERE_VALUE_TYPE_QUOTED_LITERAL");
			printf("\n %s     value:                %s", tab_buff, select_where->value);
			break;

		case BPDB_SELECT_WHERE_VALUE_TYPE_TABLE_AND_COLUMN:
			printf("BPDB_SELECT_WHERE_VALUE_TYPE_TABLE_AND_COLUMN");
			printf("\n %s     value:                %s", tab_buff, select_where->value);
			break;

		default:
			printf("BPDB_SELECT_WHERE_VALUE_TYPE_UNKNOWN");
			break;

	}

	// destroy the linkl
	BP_DestroyLinkL(tab_linkl, BP_TRUE);

	// return indicating success
	return ERR_SUCCESS;

}

// display select join info
BP_ERROR_T BP_DBDisplaySelectInfoSelectJoin(P_BPDB_SELECT_JOIN join_on, size_t tab_depth)
{

	// ensure we have an element to display
	if(!join_on)
		return ERR_FAILURE;

	// set tab_buff init to empty string
	char * tab_buff = "";

	// create a linkl
	P_BP_LINKL tab_linkl = BP_CreateLinkL(".", BP_TRUE);

	// if tab depth is set, generate tab string
	if(tab_depth)
	{

		// add tab elements
		size_t n = 0;
		for(; n < tab_depth; n++)
		{
			tq_strdup(tab_linkl, "\t", BPLN);
		}

		// create flat buffer
		BP_LinkLFlattenJoinStringElements(tab_linkl);

		// set the tab buffer
		tab_buff = (char *) tab_linkl->flat_buff;
	}

	// display join on item data
	printf("\n %s [+] Select Join-On Item %p", tab_buff, join_on);
	printf("\n %s     join_type:            %u", tab_buff, join_on->join_type);
	printf("\n %s     table_name:           %s", tab_buff, join_on->table_name);
	printf("\n %s     front_join:           %s", tab_buff, join_on->front_join);
	printf("\n %s     comparison_operator:  %s", tab_buff, join_on->comparison_operator);
	printf("\n %s     tail_join:            %s", tab_buff, join_on->tail_join);


	// destroy the linkl
	BP_DestroyLinkL(tab_linkl, BP_TRUE);

	// return indicating success
	return ERR_SUCCESS;

}

// display mariadb parameters for a provided select info
BP_ERROR_T BP_DBDisplaySelectInfoMariadbBindParameters(P_BPDB_SQLGEN_SELECT select_info)
{

	// ensure we have select info to work with
	if(!select_info)
		return ERR_FAILURE;

	// ensure we have bind parameters
	if(!select_info->bind_parameters)
		return ERR_FAILURE;
	if(!select_info->bind_parameters_n)
		return ERR_FAILURE;

	// display parameter header
	printf("\n\t [+] Displaying MariaDB Bind Parameters %p", select_info->bind_parameters);

	// declare the parameter iterator
	size_t parameter_iter = 0;

	// walk parameters and display
	for
	(
			;
			parameter_iter < select_info->bind_parameters_n;
			parameter_iter++
	)
	{

		// display the bind parameter
		BP_DBDisplaySelectInfoMariadbBindParameter
		(
				&select_info->bind_parameters[parameter_iter],
				2,
				parameter_iter
		);

	}


	// return indicating success
	return ERR_SUCCESS;

}



// display mariadb parameters for a provided select info
BP_ERROR_T BP_DBDisplaySelectInfoMariadbBindParameter(MYSQL_BIND * bind_parameter, size_t tab_depth, size_t parameter_index)
{

	// check if we have a bind parameter
	if(!bind_parameter)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Tab Buffer %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set tab_buff init to empty string
	char * tab_buff = "";

	// create a linkl
	P_BP_LINKL tab_linkl = BP_CreateLinkL(".", BP_TRUE);

	// if tab depth is set, generate tab string
	if(tab_depth)
	{

		// add tab elements
		size_t n = 0;
		for(; n < tab_depth; n++)
		{
			tq_strdup(tab_linkl, "\t", BPLN);
		}

		// create flat buffer
		BP_LinkLFlattenJoinStringElements(tab_linkl);

		// set the tab buffer
		tab_buff = (char *) tab_linkl->flat_buff;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Bind Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// iterator for bind parameters
	size_t bind_parameter_iter = 0;


	// bind_parameter,
	printf("\n%s [+] Bind Parameter %p (idx: %u)", tab_buff, bind_parameter, parameter_index);
	printf("\n%s     length:             %p", tab_buff, bind_parameter->length);
	if(bind_parameter->length)
		printf(" val: %u", *bind_parameter->length);

	printf("\n%s     is_null:            %p", tab_buff, bind_parameter->is_null);
	printf("\n%s     buffer:             %p", tab_buff, bind_parameter->buffer);
	printf("\n%s     error:              %p", tab_buff, bind_parameter->error);
	printf("\n%s     row_ptr:            %p", tab_buff, bind_parameter->row_ptr);
	printf("\n%s     store_param_func:   %p", tab_buff, bind_parameter->store_param_func);
	printf("\n%s     fetch_result:       %p", tab_buff, bind_parameter->fetch_result);
	printf("\n%s     skip_result:        %p", tab_buff, bind_parameter->skip_result);
	printf("\n%s     buffer_length:      %u", tab_buff, bind_parameter->buffer_length);
	printf("\n%s     offset:             %u", tab_buff, bind_parameter->offset);
	printf("\n%s     length_value:       %u", tab_buff, bind_parameter->length_value);

	// (MEMBER REMOVED IN LATER VERSIONS OF MARIADB)
	// printf("\n%s     param_number:       %u", tab_buff, bind_parameter->param_number);
	printf("\n%s     pack_length:        %u", tab_buff, bind_parameter->pack_length);

	// set buffer type string
	char * buffer_type_str = BP_MySQLTypeToString(bind_parameter->buffer_type);
	printf("\n%s     buffer_type:        %s (%u)", tab_buff, buffer_type_str, bind_parameter->buffer_type);
	if(buffer_type_str)
		bpfree(buffer_type_str);

	printf("\n%s     error_value:        %u", tab_buff, bind_parameter->error_value);
	printf("\n%s     is_unsigned:        %u", tab_buff, bind_parameter->is_unsigned);
	printf("\n%s     long_data_used:     %u", tab_buff, bind_parameter->long_data_used);
	printf("\n%s     is_null_value:      %u", tab_buff, bind_parameter->is_null_value);
	printf("\n%s     extension:          %p", tab_buff, bind_parameter->extension);



	// destroy the linkl before exit
	BP_DestroyLinkL(tab_linkl, BP_TRUE);

	// return indicating success
	return ERR_SUCCESS;

}

// display mariadb results for a provided select info
BP_ERROR_T BP_DBDisplaySelectInfoMariadbBindResults(P_BPDB_SQLGEN_SELECT select_info)
{

	// ensure we have select info to work with
	if(!select_info)
		return ERR_FAILURE;

	// ensure we have bind results
	if(!select_info->bind_results)
		return ERR_FAILURE;
	if(!select_info->bind_results_n)
		return ERR_FAILURE;

	// display result header
	printf("\n\t [+] Displaying MariaDB Bind Results %p", select_info->bind_results);

	// declare the result iterator
	size_t result_iter = 0;

	// walk results and display
	for
	(
			;
			result_iter < select_info->bind_results_n;
			result_iter++
	)
	{

		// display the bind result
		BP_DBDisplaySelectInfoMariadbBindResult
		(
				&select_info->bind_results[result_iter],
				2,
				result_iter
		);

	}

	// return indicating success
	return ERR_SUCCESS;

}


// display mariadb results for a provided select info
BP_ERROR_T BP_DBDisplaySelectInfoMariadbBindResult(MYSQL_BIND * bind_result, size_t tab_depth, size_t result_index)
{

	// check if we have a bind result
	if(!bind_result)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Tab Buffer %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set tab_buff init to empty string
	char * tab_buff = "";

	// create a linkl
	P_BP_LINKL tab_linkl = BP_CreateLinkL(".", BP_TRUE);

	// if tab depth is set, generate tab string
	if(tab_depth)
	{

		// add tab elements
		size_t n = 0;
		for(; n < tab_depth; n++)
		{
			tq_strdup(tab_linkl, "\t", BPLN);
		}

		// create flat buffer
		BP_LinkLFlattenJoinStringElements(tab_linkl);

		// set the tab buffer
		tab_buff = (char *) tab_linkl->flat_buff;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Bind Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// iterator for bind results
	size_t bind_result_iter = 0;


	// bind_result,
	printf("\n%s [+] Bind Result %p (idx: %u)", tab_buff, bind_result, result_index);
	printf("\n%s     length:             %p", tab_buff, bind_result->length);
	if(bind_result->length)
		printf(" val: %u", *bind_result->length);

	printf("\n%s     is_null:            %p", tab_buff, bind_result->is_null);
	printf("\n%s     buffer:             %p", tab_buff, bind_result->buffer);
	printf("\n%s     error:              %p", tab_buff, bind_result->error);
	printf("\n%s     row_ptr:            %p", tab_buff, bind_result->row_ptr);
	printf("\n%s     store_param_func:   %p", tab_buff, bind_result->store_param_func);
	printf("\n%s     fetch_result:       %p", tab_buff, bind_result->fetch_result);
	printf("\n%s     skip_result:        %p", tab_buff, bind_result->skip_result);
	printf("\n%s     buffer_length:      %u", tab_buff, bind_result->buffer_length);
	printf("\n%s     offset:             %u", tab_buff, bind_result->offset);
	printf("\n%s     length_value:       %u", tab_buff, bind_result->length_value);

	// (MEMBER REMOVED IN LATER VERSIONS OF MARIADB)
	// printf("\n%s     param_number:       %u", tab_buff, bind_result->param_number);

	printf("\n%s     pack_length:        %u", tab_buff, bind_result->pack_length);

	// set buffer type string
	char * buffer_type_str = BP_MySQLTypeToString(bind_result->buffer_type);
	printf("\n%s     buffer_type:        %s (%u)", tab_buff, buffer_type_str, bind_result->buffer_type);
	if(buffer_type_str)
		bpfree(buffer_type_str);


	printf("\n%s     error_value:        %u", tab_buff, bind_result->error_value);
	printf("\n%s     is_unsigned:        %u", tab_buff, bind_result->is_unsigned);
	printf("\n%s     long_data_used:     %u", tab_buff, bind_result->long_data_used);
	printf("\n%s     is_null_value:      %u", tab_buff, bind_result->is_null_value);
	printf("\n%s     extension:          %p", tab_buff, bind_result->extension);



	// destroy the linkl before exit
	BP_DestroyLinkL(tab_linkl, BP_TRUE);

	// return indicating success
	return ERR_SUCCESS;

}


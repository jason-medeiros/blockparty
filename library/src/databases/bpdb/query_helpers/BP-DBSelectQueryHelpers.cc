
// include blockparty
#include "../../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Select-Query Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Frees/destroys any dangling sql information that might be present in the
// current allocator.
BP_ERROR_T BP_DBSyntaxGenSelectInfoDestroy(P_BPDB_SQLGEN syngen)
{

	// ensure we have a syntax generator and select info
	if(!syngen)
		return ERR_FAILURE;
	if(!syngen->select_info)
		return ERR_FAILURE;


	// destroy query parts if they're set
	if(syngen->select_info->select_query_parts_hreg)
	{
		BP_DestroyHashTableRegistry(syngen->select_info->select_query_parts_hreg);
		syngen->select_info->select_query_parts_hreg = NULL;
	}

	// destroy results if they're set
	if(syngen->select_info->fetched_record_hreg)
	{

		BP_DestroyHashTableRegistry(syngen->select_info->fetched_record_hreg);
		syngen->select_info->fetched_record_hreg = NULL;
		syngen->select_info->fetched_record_n    = 0;

	}

	// destroy field name list
	if(syngen->select_info->field_name_list)
	{
		BP_DestroyLinkL(syngen->select_info->field_name_list, BP_TRUE);
	}

	// destroy field names if set
	if(syngen->select_info->field_names)
	{
		BP_DestroyPointerArrayNullTerminated((void **) syngen->select_info->field_names);
		bpfree(syngen->select_info->field_names);
		syngen->select_info->field_names = NULL;
	}

	// destroy table name list
	if(syngen->select_info->table_name_list)
	{
		BP_DestroyLinkL(syngen->select_info->table_name_list, BP_TRUE);
	}


	// destroy mysql parameter/result allocators (kept separate due to mysql binding requirements)
	if(syngen->select_info->bind_parameters_tq_allocator)
	{
		BP_DestroyLinkL(syngen->select_info->bind_parameters_tq_allocator, BP_TRUE);
		syngen->select_info->bind_parameters_tq_allocator = NULL;
	}

	// destroy any bound results
	if(syngen->select_info->bind_results_tq_allocator)
	{
		BP_DestroyLinkL(syngen->select_info->bind_results_tq_allocator, BP_TRUE);
		syngen->select_info->bind_results_tq_allocator = NULL;
	}

	// destroy the fetched record json info if it was created
	if(syngen->select_info->fetched_record_json_hreg)
	{
		BP_DestroyHashTableRegistry(syngen->select_info->fetched_record_json_hreg);
		syngen->select_info->fetched_record_json_hreg = NULL;
	}


	// destroy select info and nullify (no null check necessary since we do that when entering the function)
	ht_free(syngen->system_hreg, syngen->select_info);
	syngen->select_info = NULL;


	// return indicating success
	return ERR_SUCCESS;

}


// Initializes/Creates Select Info.
BP_ERROR_T BP_DBSyntaxGenSelectInfoInit(P_BPDB_SQLGEN syngen)
{


	// ensure we have a sql generator
	if(!syngen)
		return ERR_FAILURE;

	// destroy info if it exists before init
	BP_DBSyntaxGenSelectInfoDestroy(syngen);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Initial Dynamic Data Structures %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// allocate select info after destruction
	syngen->select_info = (P_BPDB_SQLGEN_SELECT) ht_calloc(syngen->system_hreg, sizeof(BPDB_SQLGEN_SELECT), 1);

	// zero the info
	memset(syngen->select_info, 0x00, sizeof(BPDB_SQLGEN_SELECT));

	// create necessary registries
	syngen->select_info->fetched_record_json_hreg = BP_CreateHashTableRegistry();
	syngen->select_info->select_query_parts_hreg  = BP_CreateHashTableRegistry();
	syngen->select_info->fetched_record_hreg      = BP_CreateHashTableRegistry();

	// create necessary tailqueues
	syngen->select_info->bind_results_tq_allocator    = BP_CreateLinkL("bind_results_allocator", BP_TRUE);
	syngen->select_info->bind_results_n               = 0;
	syngen->select_info->bind_results                 = NULL;
	syngen->select_info->bind_parameters_tq_allocator = BP_CreateLinkL("bind_params_allocator", BP_TRUE);
	syngen->select_info->bind_parameters_n            = 0;
	syngen->select_info->bind_parameters              = NULL;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Fill In Required Registry Tables/Paths %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to create all tables and paths
	if
	(
		!BP_HashRegAddTableByPathQuery(syngen->select_info->select_query_parts_hreg, "/field_names")      ||
		!BP_HashRegAddTableByPathQuery(syngen->select_info->select_query_parts_hreg, "/primary_select_from_table_name") ||
		!BP_HashRegAddTableByPathQuery(syngen->select_info->select_query_parts_hreg, "/where_clauses")    ||
		!BP_HashRegAddTableByPathQuery(syngen->select_info->select_query_parts_hreg, "/join_clauses")     ||
		!BP_HashRegAddTableByPathQuery(syngen->select_info->select_query_parts_hreg, "/group_by_clauses") ||
		!BP_HashRegAddTableByPathQuery(syngen->select_info->select_query_parts_hreg, "/limit_clause")     ||
		!BP_HashRegAddTableByPathQuery(syngen->select_info->select_query_parts_hreg, "/order_clause")
	)
	{
			BP_DBSyntaxGenSelectInfoDestroy(syngen);
			return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}


// -- field names --

// Attempt to add a single field name.
BP_ERROR_T BP_DBSyntaxGenSelectInfoAddFieldName(P_BPDB_SQLGEN syngen, char *field)
{

	// run basic null checks
	if(!syngen)
		return ERR_FAILURE;
	if(!field)
		return ERR_FAILURE;
	if(!syngen->select_info)
		return ERR_FAILURE;
	if(!syngen->select_info->select_query_parts_hreg)
		return ERR_FAILURE;

	// ensure field is reasonable
	if(!BP_StringIsReasonableForSelectFieldSpecifierDB(field, bpstrlen(field)))
		return ERR_FAILURE;

	// set add path
	char *add_path[] =
	{
			"field_names",
			NULL
	};

	// add the field entry, exit with failure code if it cannot be added
	if(
		!BP_HashRegAddDataToTableByPath
		(
			syngen->select_info->select_query_parts_hreg,
			(char **) &add_path,
			field,
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
			field,
			bpstrlen(field)
		)
	)
	{
		return ERR_FAILURE;
	}


	// return indicating success
	return ERR_SUCCESS;

}

// Adds a set of fields to the current select info structure.  Fields must be null ptr terminated.
BP_ERROR_T BP_DBSyntaxGenSelectInfoAddFieldNames(P_BPDB_SQLGEN syngen, char **fields)
{

	// ensure we have a syntax generator
	if(!syngen)
		return ERR_FAILURE;
	if(!syngen->bpdb)
		return ERR_FAILURE;
	if(!fields)
		return ERR_FAILURE;
	if(!fields[0])
		return ERR_FAILURE;

	// run field checks first
	size_t str_pos_iter = 0;
	for(; fields[str_pos_iter]; str_pos_iter++)
	{

		// ensure it's reasonable
		if(!BP_StringIsReasonableForSelectFieldSpecifierDB(fields[str_pos_iter], bpstrlen(fields[str_pos_iter])))
		{

			// create a temporary message buffer
			char tmp_str[512];
			BP_ZERO_STACK(tmp_str);

			// copy in message
			snprintf((char *) &tmp_str, 511, "BP_DBSyntaxGenSelectInfoAddFieldNames reports that the field specifier at position %zu within the provided field array is not a reasonable field name.  The field has not been directly added to this message because it potentially contains corrupt binary data.", str_pos_iter);

			// add error message
			BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_str, NULL);
			return ERR_FAILURE;

		}

	}

	// now attempt to add fields
	for(str_pos_iter = 0; fields[str_pos_iter]; str_pos_iter++)
	{

		// call addr and exit on failure
		if(!BP_DBSyntaxGenSelectInfoAddFieldName(syngen, fields[str_pos_iter]))
		{

			// create a temporary message buffer
			char tmp_str[512+bpstrlen(fields[str_pos_iter])];
			BP_ZERO_STACK(tmp_str);

			// copy in message
			snprintf((char *) &tmp_str, 511, "BP_DBSyntaxGenSelectInfoAddFieldNames reports that the field specifier at position %zu (%s) cannot be added (likely duplicate key error)", str_pos_iter, fields[str_pos_iter]);

			// add error message
			BP_DBAddErrorMsg(syngen->bpdb, (char *) &tmp_str, NULL);
			return ERR_FAILURE;

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}


// -- primary table selection --

// sets the primary selection table (not a joined table, must be set, must be valid)
BP_ERROR_T BP_DBSyntaxGenSelectInfoSetPrimaryTable(P_BPDB_SQLGEN syngen, char *primary_select_table)
{

	// run basic checks
	if(!syngen)
		return ERR_SUCCESS;
	if(!primary_select_table)
		return ERR_SUCCESS;
	if(!BP_StringIsReasonableForTableNameDB(primary_select_table, bpstrlen(primary_select_table)))
		return ERR_FAILURE;
	if(!bpstrlen(primary_select_table))
		return ERR_FAILURE;
	if(!syngen->select_info)
		return ERR_FAILURE;
	if(!syngen->select_info->select_query_parts_hreg)
		return ERR_FAILURE;

	// lookup table entry
	P_BP_HTKE query_parts_hreg_table = BP_HashRegLookupTableByPathQuery(syngen->select_info->select_query_parts_hreg, "/primary_select_from_table_name");
	if(!query_parts_hreg_table)
		return ERR_FAILURE;

	// clear the table if we have entries
	if(query_parts_hreg_table->data)
	{

		// iterate through entries within the table
		BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) query_parts_hreg_table->data, table_entry)
		{

			BP_HashRegDestroyEntry(syngen->select_info->select_query_parts_hreg, table_entry);

		}
		BP_HASH_ITER_CLOSE;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Entry after Clearing %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to add the value
	BP_ERROR_T ret_val = BP_HashRegAddDataToTableByPathQuery
	(
			syngen->select_info->select_query_parts_hreg,
			"/primary_select_from_table_name",
			primary_select_table,
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
			primary_select_table,
			bpstrlen(primary_select_table)
	);

	// return indicating data addition information
	return ret_val;

}


// -- where clauses --

// Adds a where clause to the select settings based on input and type.  Performs sanitization and
// corruption detection before adding.
BP_ERROR_T BP_DBSyntaxGenSelectInfoAddWhereClause
(
		P_BPDB_SQLGEN                syngen,
		BPDB_SELECT_WHERE_VALUE_TYPE type,
		char *                       conjunction,
		char *                       field_name,
		char *                       limiter,
		char *                       value,
		size_t                       value_len
)
{

	// run basic checks
	if(!syngen)
	{
		return ERR_FAILURE;
	}
	if(!syngen->bpdb)
	{
		return ERR_FAILURE;
	}

	// set select info
	if(!syngen->select_info)
		return ERR_FAILURE;
	if(!syngen->select_info->select_query_parts_hreg)
		return ERR_FAILURE;

	if(!type)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddWhereClause was called with an unset type parameter.");
		return ERR_FAILURE;
	}
	if(!field_name)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddWhereClause was called with a null field name.");
		return ERR_FAILURE;
	}
	if(!limiter)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddWhereClause was called with a null limiter parameter.");
		return ERR_FAILURE;
	}
	if(!value)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddWhereClause was called with a null value parameter.");
		return ERR_FAILURE;
	}

	// run reasonable-ness checks.
	if(!BP_StringIsReasonableForSelectFieldSpecifierDB(field_name, bpstrlen(field_name)))
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddWhereClause was called with an unreasonable field name (bad characters).");
		return ERR_FAILURE;
	}

	if(!BP_StringIsReasonableForMariadbComparisonOperatorDB(limiter, bpstrlen(limiter)))
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddWhereClause was called with an unreasonable limiter parameter (bad characters).");
		return ERR_FAILURE;
	}

	if(conjunction)
	{
		if(!BP_StringIsReasonableForMariadbConjunctionDB(conjunction, bpstrlen(conjunction)))
		{
			BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddWhereClause was called with an unreasonable conjunction parameter (bad characters).");
			return ERR_FAILURE;
		}
	}





	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Clause Table %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// count the clauses
	size_t clause_entry_count = BP_HashRegCountEntriesInTableByPathQuery
	(
		syngen->select_info->select_query_parts_hreg,
		"/where_clauses",
		BP_HASH_TABLE_COUNT_POLICY_COUNT_ONLY_TABLE_ENTRIES
	);

	// create clause position
	char clause_position[256];
	BP_ZERO_STACK(clause_position);
	sprintf((char *) &clause_position, "where_clause_%zu", clause_entry_count);

	// setup table add path
	char *table_path[] =
	{
		"where_clauses",
		(char *) &clause_position,
		NULL
	};

	// add table path
	BP_HashRegAddTableByPath
	(
		syngen->select_info->select_query_parts_hreg,
		(char **) &table_path
	);

	// attempt to lookup table
	P_BP_HTKE table_lookup = BP_HashRegLookupTableByPath
	(
		syngen->select_info->select_query_parts_hreg,
		(char **) &table_path
	);

	// exit if the table was not added ok
	if(!table_lookup)
	{
		return ERR_FAILURE;
	}

	// create lookup path
	char *lookup_path = BP_HashRegEntryToPathQuery(table_lookup);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Fields after Checks Complete %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to add conjunction
	if(conjunction)
	{

		if
		(
			!BP_HashRegAddDataToTableByPathQuery
			(
				syngen->select_info->select_query_parts_hreg,
				lookup_path,
				"conjunction",
				BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
				conjunction,
				bpstrlen(conjunction)
			)
		)
		{
			bpfree(lookup_path);
			BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddWhereClause was unable to add conjunction parameter.");
			return ERR_FAILURE;
		}

	}

	// attempt to add field_name
	if
	(
		!BP_HashRegAddDataToTableByPathQuery
		(
			syngen->select_info->select_query_parts_hreg,
			lookup_path,
			"field_name",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
			field_name,
			bpstrlen(field_name)
		)
	)
	{
		bpfree(lookup_path);
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddWhereClause was unable to add field_name parameter.");
		return ERR_FAILURE;
	}



	// attempt to add limiter
	if
	(
		!BP_HashRegAddDataToTableByPathQuery
		(
			syngen->select_info->select_query_parts_hreg,
			lookup_path,
			"limiter",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
			limiter,
			bpstrlen(limiter)
		)
	)
	{
		bpfree(lookup_path);
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddWhereClause was unable to add limiter parameter.");
		return ERR_FAILURE;
	}



	// set value type
	size_t value_type = type;

	// attempt to add value type
	if
	(
		!BP_HashRegAddDataToTableByPathQuery
		(
			syngen->select_info->select_query_parts_hreg,
			lookup_path,
			"value_type",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_SIZE_T,
			(char *) &value_type,
			sizeof(size_t)
		)
	)
	{
		bpfree(lookup_path);
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddWhereClause was unable to add value_type parameter.");
		return ERR_FAILURE;
	}


	// attempt to add value length
	if
	(
		!BP_HashRegAddDataToTableByPathQuery
		(
			syngen->select_info->select_query_parts_hreg,
			lookup_path,
			"value_len",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_SIZE_T,
			(char *) &value_len,
			sizeof(size_t)
		)
	)
	{
		bpfree(lookup_path);
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddWhereClause was unable to add value_type parameter.");
		return ERR_FAILURE;
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Parse Values Based on Type %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create buffer to hold integer conversion buffers
	char tmp_integer_buff[512];
	BP_ZERO_STACK(tmp_integer_buff);

	// this is used to dereference long integer values
	long int * tmp_long_int_ptr = NULL;

	// switch on the type
	switch(type)
	{

		// add direct values
		case BPDB_SELECT_WHERE_VALUE_TYPE_TABLE_AND_COLUMN:
		case BPDB_SELECT_WHERE_VALUE_TYPE_QUOTED_LITERAL:

			// string values must always be reasonable/printable
			if(!BP_StringIsReasonablePrintableString(value, bpstrlen(value)))
			{
				bpfree(lookup_path);
				BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddWhereClause was called with an unreasonable value parameter (bad characters in BPDB_SELECT_WHERE_VALUE_TYPE_QUOTED_LITERAL or BPDB_SELECT_WHERE_VALUE_TYPE_TABLE_AND_COLUMN type).");
				return ERR_FAILURE;
			}

			if
			(
				!BP_HashRegAddDataToTableByPathQuery
				(
					syngen->select_info->select_query_parts_hreg,
					lookup_path,
					"value",
					BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
					value,
					bpstrlen(value)
				)
			)
			{
				bpfree(lookup_path);
				BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddWhereClause was unable to add value parameter.");
				return ERR_FAILURE;
			}
			break;

		// set hex value
		case BPDB_SELECT_WHERE_VALUE_TYPE_HEX_STRING:
			if
			(
				!BP_HashRegAddDataToTableByPathQuery
				(
					syngen->select_info->select_query_parts_hreg,
					lookup_path,
					"value",
					BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
					value,
					value_len
				)
			)
			{
				bpfree(lookup_path);
				BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddWhereClause was unable to add value parameter (BPDB_SELECT_WHERE_VALUE_TYPE_HEX_STRING).");
				return ERR_FAILURE;
			}
			break;

		// add positive integer
		case BPDB_SELECT_WHERE_VALUE_TYPE_POSITIVE_INTEGER:


			tmp_long_int_ptr = (long int *) value;

			printf("\n GOT POSITIVE INTEGER TYPE?!: %li", *tmp_long_int_ptr);
			sprintf((char *) &tmp_integer_buff, "%li", *tmp_long_int_ptr);
			if
			(
				!BP_HashRegAddDataToTableByPathQuery
				(
					syngen->select_info->select_query_parts_hreg,
					lookup_path,
					"value",
					BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
					value,
					value_len
				)
			)
			{
				bpfree(lookup_path);
				BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddWhereClause was unable to add value parameter.");
				return ERR_FAILURE;
			}
			break;

		// add negative integer
		case BPDB_SELECT_WHERE_VALUE_TYPE_NEGATIVE_INTEGER:
			tmp_long_int_ptr = (long int *) value;
			sprintf((char *) &tmp_integer_buff, "%li", *tmp_long_int_ptr);
			if
			(
				!BP_HashRegAddDataToTableByPathQuery
				(
					syngen->select_info->select_query_parts_hreg,
					lookup_path,
					"value",
					BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
					value,
					value_len
				)
			)
			{
				bpfree(lookup_path);
				BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddWhereClause was unable to add value parameter.");
				return ERR_FAILURE;
			}
			break;

		default:
			BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddWhereClause was passed in a bad type.  Conversion failed.");
			break;

	}

	// destroy the lookup path
	bpfree(lookup_path);

	// return indicating success
	return ERR_SUCCESS;

}


// -- joins --

// Adds a join part to the select settings based on input and type.  Performs sanitization and
// corruption detection before adding.
BP_ERROR_T BP_DBSyntaxGenSelectInfoAddJoinPart
(
	P_BPDB_SQLGEN         syngen,
	BPDB_SELECT_JOIN_TYPE type,
	char *                table_name,
	char *                front_join,
	char *                comparison_operator,
	char *                tail_join
)
{

	// run basic checks
	if(!syngen)
	{
		return ERR_FAILURE;
	}
	if(!syngen->select_info)
		return ERR_FAILURE;
	if(!syngen->select_info->select_query_parts_hreg)
		return ERR_FAILURE;
	if(!syngen->bpdb)
	{
		return ERR_FAILURE;
	}
	if(!type)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddJoinPart was called with an unset type parameter.");
		return ERR_FAILURE;
	}
	if(!table_name)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddJoinPart was called with a null table name.");
		return ERR_FAILURE;
	}
	if(!front_join)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddJoinPart was called with a null front join.");
		return ERR_FAILURE;
	}
	if(!comparison_operator)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddJoinPart was called with a null comparison operator.");
		return ERR_FAILURE;
	}
	if(!tail_join)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddJoinPart was called with a null tail join.");
		return ERR_FAILURE;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run String Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// BPDB_SELECT_JOIN_TYPE type,
	// char *table_name,
	// char *front_join,
	// char *comparison_operator,
	// char *tail_join

	// check table name
	if(!BP_StringIsReasonableForTableNameDB(table_name, bpstrlen(table_name)))
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddJoinPart was called with an unreasonable talbe name (bad characters).");
		return ERR_FAILURE;
	}

	// check front join (should always be a field specifier)
	if(!BP_StringIsReasonableForSelectFieldSpecifierDB(front_join, bpstrlen(front_join)))
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddJoinPart was called with an unreasonable talbe name (bad characters).");
		return ERR_FAILURE;
	}

	// check front join (should always be a field specifier)
	if(!BP_StringIsReasonableForMariadbComparisonOperatorDB(comparison_operator, bpstrlen(comparison_operator)))
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddJoinPart was called with an unreasonable comparison operator (bad characters).");
		return ERR_FAILURE;
	}

	// check tail join (can be roughly any value, validated during query)
	if(!BP_StringIsReasonablePrintableString(tail_join, bpstrlen(tail_join)))
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddJoinPart was called with an unreasonable tail join (bad characters).");
		return ERR_FAILURE;
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Clause Table %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// count the clauses
	size_t clause_entry_count = BP_HashRegCountEntriesInTableByPathQuery
	(
		syngen->select_info->select_query_parts_hreg,
		"/join_clauses",
		BP_HASH_TABLE_COUNT_POLICY_COUNT_ONLY_TABLE_ENTRIES
	);

	// create clause position
	char clause_position[256];
	BP_ZERO_STACK(clause_position);
	sprintf((char *) &clause_position, "join_clause_%zu", clause_entry_count);

	// setup table add path
	char *table_path[] =
	{
		"join_clauses",
		(char *) &clause_position,
		NULL
	};

	// add table path
	BP_HashRegAddTableByPath
	(
		syngen->select_info->select_query_parts_hreg,
		(char **) &table_path
	);

	// attempt to lookup table
	P_BP_HTKE table_lookup = BP_HashRegLookupTableByPath
	(
		syngen->select_info->select_query_parts_hreg,
		(char **) &table_path
	);

	// exit if the table was not added ok
	if(!table_lookup)
	{
		return ERR_FAILURE;
	}

	// create lookup path
	char *lookup_path = BP_HashRegEntryToPathQuery(table_lookup);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Fields after Checks Complete %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// BPDB_SELECT_JOIN_TYPE type,
	// char *table_name,
	// char *front_join,
	// char *comparison_operator,
	// char *tail_join


	// attempt to add value type
	if
	(
		!BP_HashRegAddDataToTableByPathQuery
		(
			syngen->select_info->select_query_parts_hreg,
			lookup_path,
			"type",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_SIZE_T,
			(char *) &type,
			sizeof(size_t)
		)
	)
	{
		bpfree(lookup_path);
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddJoinPart was unable to add value_type parameter.");
		return ERR_FAILURE;
	}

	// attempt to add table_name
	if
	(
		!BP_HashRegAddDataToTableByPathQuery
		(
			syngen->select_info->select_query_parts_hreg,
			lookup_path,
			"table_name",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
			table_name,
			bpstrlen(table_name)
		)
	)
	{
		bpfree(lookup_path);
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddJoinPart was unable to add table_name parameter.");
		return ERR_FAILURE;
	}

	// attempt to add table_name
	if
	(
		!BP_HashRegAddDataToTableByPathQuery
		(
			syngen->select_info->select_query_parts_hreg,
			lookup_path,
			"front_join",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
			front_join,
			bpstrlen(front_join)
		)
	)
	{
		bpfree(lookup_path);
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddJoinPart was unable to add table_name parameter.");
		return ERR_FAILURE;
	}


	// attempt to add table_name
	if
	(
		!BP_HashRegAddDataToTableByPathQuery
		(
			syngen->select_info->select_query_parts_hreg,
			lookup_path,
			"comparison_operator",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
			comparison_operator,
			bpstrlen(comparison_operator)
		)
	)
	{
		bpfree(lookup_path);
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddJoinPart was unable to add table_name parameter.");
		return ERR_FAILURE;
	}

	// attempt to add tail_join
	if
	(
		!BP_HashRegAddDataToTableByPathQuery
		(
			syngen->select_info->select_query_parts_hreg,
			lookup_path,
			"tail_join",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
			tail_join,
			bpstrlen(tail_join)
		)
	)
	{
		bpfree(lookup_path);
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddJoinPart was unable to add tail_join parameter.");
		return ERR_FAILURE;
	}


	// destroy the lookup path
	bpfree(lookup_path);

	// return indicating success
	return ERR_SUCCESS;

}

// -- group by --

// this intends to add a group by clause to the selection
// todo: flush this out and get it working.
BP_ERROR_T BP_DBSyntaxGenSelectInfoAddGroupByClause
(
		P_BPDB_SQLGEN syngen,
		char         *field_name
)
{

	// run basic checks
	if(!syngen)
		return ERR_FAILURE;
	if(!field_name)
		return ERR_FAILURE;
	if(!BP_StringIsReasonableForSelectFieldSpecifierDB(field_name, bpstrlen(field_name)))
		return ERR_FAILURE;


	// attempt to add field_name
	if
	(
		!BP_HashRegAddDataToTableByPathQuery
		(
			syngen->select_info->select_query_parts_hreg,
			"/group_by_clauses",
			"field_name",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
			field_name,
			bpstrlen(field_name)
		)
	)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoAddJoinPart was unable to add field_name parameter.");
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}

// -- limits --

// Set limit-by clause.
BP_ERROR_T BP_DBSyntaxGenSelectInfoSetLimitClause
(
		P_BPDB_SQLGEN syngen,
		size_t limit_low,
		size_t limit_high
)
{

	// run basic checks
	if(!syngen)
		return ERR_FAILURE;
	if(!limit_high)
		return ERR_FAILURE;
	if(limit_low > limit_high)
		return ERR_FAILURE;
	if(!syngen->select_info)
		return ERR_FAILURE;
	if(!syngen->select_info->select_query_parts_hreg)
		return ERR_FAILURE;

	// attempt to add limit_low
	if
	(
		!BP_HashRegAddDataToTableByPathQuery
		(
			syngen->select_info->select_query_parts_hreg,
			"/limit_clause",
			"limit_low",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_SIZE_T,
			(char *) &limit_low,
			sizeof(limit_low)
		)
	)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoSetLimitClause was unable to add limit_low parameter.");
		return ERR_FAILURE;
	}

	// attempt to add limit_high
	if
	(
		!BP_HashRegAddDataToTableByPathQuery
		(
			syngen->select_info->select_query_parts_hreg,
			"/limit_clause",
			"limit_high",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_SIZE_T,
			(char *) &limit_high,
			sizeof(limit_high)
		)
	)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoSetLimitClause was unable to add limit_high parameter.");
		return ERR_FAILURE;
	}


	// return indicating success
	return ERR_SUCCESS;

}

// -- order --

BP_ERROR_T BP_DBSyntaxGenSelectInfoSetOrderByClause
(
		P_BPDB_SQLGEN syngen,
		char *field,
		char *sort_designator
)
{


	// run basic checks
	if(!syngen)
		return ERR_FAILURE;
	if(!syngen->select_info)
		return ERR_FAILURE;
	if(!syngen->select_info->select_query_parts_hreg)
		return ERR_FAILURE;

	if(!field)
		return ERR_FAILURE;
	if(!sort_designator)
		return ERR_FAILURE;
	if(!BP_StringIsReasonableForSelectFieldSpecifierDB(field, bpstrlen(field)))
		return ERR_FAILURE;
	if(!BP_StringIsReasonableForMariadbSortDesignatorDB(sort_designator, bpstrlen(sort_designator)))
		return ERR_FAILURE;

	printf("\n got field here:      %s", field);
	printf("\n got designator here: %s", sort_designator);

	// attempt to add limit_low
	if
	(
		!BP_HashRegAddDataToTableByPathQuery
		(
			syngen->select_info->select_query_parts_hreg,
			"/order_clause",
			"field",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
			field,
			bpstrlen(field)
		)
	)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoSetOrderByClause was unable to add field parameter.");
		return ERR_FAILURE;
	}

	// attempt to add sort_designator
	if
	(
		!BP_HashRegAddDataToTableByPathQuery
		(
			syngen->select_info->select_query_parts_hreg,
			"/order_clause",
			"sort_designator",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
			sort_designator,
			bpstrlen(sort_designator)
		)
	)
	{
		BP_DBAddErrorMsg(syngen->bpdb, "BP_DBSyntaxGenSelectInfoSetOrderByClause was unable to add sort_designator parameter.");
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Hash Reg to Select Info %%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// if no select info structure is passed into the select-from
BP_ERROR_T BP_DBSyntaxGenSelectInfoStructFromInternalRegistries(P_BPDB_SQLGEN syngen)
{

	// set syngen information
	if(!syngen)
		return ERR_FAILURE;
	if(!syngen->select_info)
		return ERR_FAILURE;


	// retrieve query parts
	P_BP_HTKE field_names                    = BP_HashRegLookupTableByPathQuery(syngen->select_info->select_query_parts_hreg, "/field_names");
	P_BP_HTKE primary_select_from_table_name = BP_HashRegLookupTableByPathQuery(syngen->select_info->select_query_parts_hreg, "/primary_select_from_table_name");
	P_BP_HTKE where_clauses                  = BP_HashRegLookupTableByPathQuery(syngen->select_info->select_query_parts_hreg, "/where_clauses");
	P_BP_HTKE group_by_clauses               = BP_HashRegLookupTableByPathQuery(syngen->select_info->select_query_parts_hreg, "/group_by_clauses");
	P_BP_HTKE limit_clause                   = BP_HashRegLookupTableByPathQuery(syngen->select_info->select_query_parts_hreg, "/limit_clause");
	P_BP_HTKE order_clause                   = BP_HashRegLookupTableByPathQuery(syngen->select_info->select_query_parts_hreg, "/order_clause");


	printf("\n [+] Getting here woo!");
	printf("\n\t field_names: %p", field_names);
	printf("\n\t primary_select_from_table_name: %p", primary_select_from_table_name);
	printf("\n\t where_clauses: %p", where_clauses);
	printf("\n\t group_by_clauses: %p", group_by_clauses);
	printf("\n\t limit_clause: %p", limit_clause);
	printf("\n\t order_clause: %p", limit_clause);


	// select info
	P_BPDB_SQLGEN_SELECT tmp_select_info = NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Process Field Names %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// create select name builder inkl
	P_BP_LINKL select_name_builder_linkl = BP_CreateLinkL(".", BP_TRUE);

	// field names
	if(field_names)
	{

		if(field_names->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{

			// iterate trhough field names
			BP_HASH_ITER_OPEN((P_BP_HTKE) field_names->data, field_name)
			{

				// add to tailqueue
				tq_strdup(select_name_builder_linkl, (char *) field_name->data, BPLN);

			}
			BP_HASH_ITER_CLOSE;

		}

	}

	// convert tailqueue to field names
	char ** field_names_ptr_arr = (char **) BP_LinkLToBPCallocPointerArray(select_name_builder_linkl);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Process Primary Table Name %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	/*
	BP_HASH_ITER_OPEN()
	{

	}
	BP_HASH_ITER_CLOSE;
	*/


	// return indicating success
	return ERR_SUCCESS;

}



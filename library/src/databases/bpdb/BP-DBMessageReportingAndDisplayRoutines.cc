
// include blockparty
#include "../../../include/BP-Main.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

BP_ERROR_T BP_DBDisplay
(
	P_BPDB bpdb,
	BP_BOOL display_mariadb_table_metadata,
	BP_BOOL display_mariadb_column_metadata,
	BP_BOOL display_sqlite_table_metadata,
	BP_BOOL display_sqlite_column_metadata
)
{



	// ensure we have a database handle
	if(!bpdb)
		return ERR_FAILURE;
	if(!bpdb->system_hreg)
		return ERR_FAILURE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Display %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(bpdb->system_hreg);


	printf("\n [+] Displaying BPDB handle: %p", bpdb);
	printf("\n    uuid:                    %s", bpdb->uuid);
	printf("\n    system_hreg:             %p", bpdb->system_hreg);
	// printf("\n    mariadb:                 %p", bpdb->mariadb);
	printf("\n    sqlite:                  %p", bpdb->sqlite);
	printf("\n    write_mirroring_enabled: %s", bpdb->write_mirroring_enabled ? "yes" : "no");
	printf("\n");

	// display schema information
	printf("\n\t [+] SQL Table Schema Information:");
	printf("\n");

	// display tables
	// display tables directly (only tables)
	BP_DBDisplayTablesAndColumns
	(
		bpdb,
		display_mariadb_table_metadata,
		display_mariadb_column_metadata,
		display_sqlite_table_metadata,
		display_sqlite_column_metadata
	);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Leave Critical Section and Exit %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(bpdb->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}


// display any syntax generators registered to this bpdb
BP_ERROR_T BP_DBDisplayRegisteredSyntaxGenerators(P_BPDB bpdb)
{

	// run basic null checks
	if(!bpdb)
		return ERR_FAILURE;
	if(!bpdb->sqlgen_reg)
		return ERR_FAILURE;
	if(!bpdb->sqlgen_reg->hive)
		return ERR_FAILURE;
	if(!bpdb->sqlgen_reg->total_data_entries_in_hive)
		return ERR_FAILURE;

	// displaying sqlgen registries
	printf("\n\t  [+] Displaying SQL Generators: %p", bpdb->sqlgen_reg);

	// iterate through sqlgen
	BP_HASH_ITER_OPEN((P_BP_HTKE) bpdb->sqlgen_reg->hive->data, sqlgen_table_entry)
	{
		printf("\n\t\t  sqlgen uuid: %s - %p", sqlgen_table_entry->key, sqlgen_table_entry->data);
	}
	BP_HASH_ITER_CLOSE;

	// return indicating success
	return ERR_SUCCESS;

}


// display tables directly (only tables)
BP_ERROR_T BP_DBDisplayTablesAndColumns
(
	P_BPDB bpdb,
	BP_BOOL display_mariadb_table_metadata,
	BP_BOOL display_mariadb_column_metadata,
	BP_BOOL display_sqlite_table_metadata,
	BP_BOOL display_sqlite_column_metadata
)
{


	// ensure we have a database handle
	if(!bpdb)
		return ERR_FAILURE;
	if(!bpdb->system_hreg)
		return ERR_FAILURE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Display %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(bpdb->system_hreg);


	// iterate through tables and display names
	if(bpdb->system_hreg->hive)
	BP_HASH_ITER_OPEN(bpdb->system_hreg->hive, outer_table_iter)
	{

		// only display tables entry
		if(outer_table_iter->key)
		if(strncmp(outer_table_iter->key, "tables", 6) != 0)
			continue;

		// iterate individual tables
		BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) outer_table_iter->data, table_iter)
		{

			// ensure that the entry is a table
			if(table_iter->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
				continue;

			// retrieve column count for this table
			size_t col_count_n = BP_DBGetColumnCountForTable(bpdb,table_iter->key);


			// display table top
			printf("\n\t\t [+] %s  (%u columns)", table_iter->key, col_count_n);

			// display table metadata if necessary
			if(display_mariadb_table_metadata)
			{

				// attempt to lookup table metadata by name.
				P_BPDB_SQL_TABLE_METADATA table_metadata = BP_DBLookupTableMetadataFromTableName(bpdb, table_iter->key);

				// display metadata if it can be looked up ok
				if(table_metadata)
				{

					// display mariadb table metadata here
					BP_DBDisplayMariaDBTableMetadata
					(
							bpdb,
							&table_metadata->mariadb_table_metadata,
							3
					);

				}

			}


			// looks up the columns table for a given table name.  This is the keyed entry for the
			// tables/specific_table/columns/ path.
			P_BP_HASH_TABLE_KEYED_ENTRY table_columns = BP_DBLookupTableColumns(bpdb, table_iter->key);

			// iterate through columns now
			BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) table_columns->data, col_data_iter)
			{

				// display column
				printf("\n\t\t\t\t [->] %s (column in %s)", col_data_iter->key, table_iter->key);

				// display mariadb column metadata
				if(display_mariadb_column_metadata)
				{

					// retrieve column metadata
					P_BPDB_SQL_TABLE_COLUMN_METADATA column_metadata = BP_DBLookupTableColumnMetadataFromColumnName(bpdb, table_iter->key, col_data_iter->key);

					// display column metadata
					BP_DBDisplayMariaDBTableColumnMetadata
					(
							bpdb,
							&column_metadata->mariadb_column_metadata,
							5
					);

				}


			} BP_HASH_ITER_CLOSE;

		} BP_HASH_ITER_CLOSE;

		printf("\n");

	} BP_HASH_ITER_CLOSE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Leave Critical Section and Exit %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(bpdb->system_hreg);

	// return indicating success
	return ERR_SUCCESS;
}

// Display mariadb table metadata
BP_ERROR_T BP_DBDisplayMariaDBTableMetadata
(
		P_BPDB bpdb,
		P_BPDB_MARIADB_TABLE_METADATA table_metadata,
		size_t tab_indent_n
)
{

	// ensure we have metadata values
	if(!bpdb)
		return ERR_FAILURE;
	if(!bpdb->system_hreg)
		return ERR_FAILURE;
	if(!table_metadata)
		return ERR_FAILURE;
	if(tab_indent_n > 20)
		return ERR_FAILURE;

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(bpdb->system_hreg);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Tab Indent Buffer %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create a new tailqueue
	P_BP_LINKL tq = BP_CreateLinkL(".", BP_TRUE);

	// create the tab buffer
	size_t tab_iter = 0;
	for(; tab_iter < tab_indent_n; tab_iter++)
		tq_strdup(tq, "\t", BPLN);

	// join string elements
	BP_LinkLFlattenJoinStringElements(tq);

	// store the tab buffer
	char * tab_buff = bpstrndup((char *) tq->flat_buff, tq->flat_buff_n);

	// destroy the linkl after flattening
	BP_DestroyLinkL(tq, BP_TRUE);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Metadata %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	printf("\n%s[+] MariaDB Table Metadata for %s (%p)", tab_buff, table_metadata->table_name, table_metadata);
	printf("\n%s   table_catalog:    %s",       tab_buff, table_metadata->table_catalog);
	printf("\n%s   table_schema:     %s",       tab_buff, table_metadata->table_schema);
	printf("\n%s   table_name:       %s",       tab_buff, table_metadata->table_name);
	printf("\n%s   table_type:       %s",       tab_buff, table_metadata->table_type);
	printf("\n%s   engine:           %s",       tab_buff, table_metadata->engine);
	printf("\n%s   version:          %lli",     tab_buff, table_metadata->version);
	printf("\n%s   row_format:       %s",       tab_buff, table_metadata->row_format);
	printf("\n%s   table_rows:       %lli",     tab_buff, table_metadata->table_rows);
	printf("\n%s   avg_row_length:   %lli",     tab_buff, table_metadata->avg_row_length);
	printf("\n%s   data_length:      %lli",     tab_buff, table_metadata->data_length);
	printf("\n%s   max_data_length:  %lli",     tab_buff, table_metadata->data_length);
	printf("\n%s   index_length:     %lli",     tab_buff, table_metadata->index_length);
	printf("\n%s   data_free:        %lli",     tab_buff, table_metadata->data_free);
	printf("\n%s   auto_increment:   %lli",     tab_buff, table_metadata->auto_increment);
	printf("\n%s   create_time:      %lli",     tab_buff, table_metadata->create_time);
	printf("\n%s   update_time:      %lli",     tab_buff, table_metadata->update_time);
	printf("\n%s   check_time:       %lli",     tab_buff, table_metadata->check_time);
	printf("\n%s   table_collation:  %s",       tab_buff, table_metadata->table_collation);
	printf("\n%s   check_time:       %u",       tab_buff, table_metadata->check_time);
	printf("\n%s   create_options:   %s",       tab_buff, table_metadata->create_options);
	printf("\n%s   table_comment:    %s",       tab_buff, table_metadata->table_comment);

	// destroy the tab buffer after display
	bpfree(tab_buff);



	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(bpdb->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}

// Display mariadb table column metadata
BP_ERROR_T BP_DBDisplayMariaDBTableColumnMetadata
(
		P_BPDB bpdb,
		P_BPDB_MARIADB_TABLE_COLUMN_METADATA table_column_metadata,
		size_t tab_indent_n
)
{

	// ensure we have metadata values
	if(!bpdb)
		return ERR_FAILURE;
	if(!bpdb->system_hreg)
		return ERR_FAILURE;
	if(!table_column_metadata)
		return ERR_FAILURE;
	if(tab_indent_n > 20)
		return ERR_FAILURE;

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(bpdb->system_hreg);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Tab Indent Buffer %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create a new tailqueue
	P_BP_LINKL tq = BP_CreateLinkL(".", BP_TRUE);

	// create the tab buffer
	size_t tab_iter = 0;
	for(; tab_iter < tab_indent_n; tab_iter++)
		tq_strdup(tq, "\t", BPLN);

	// join string elements
	BP_LinkLFlattenJoinStringElements(tq);

	// store the tab buffer
	char * tab_buff = bpstrndup((char *) tq->flat_buff, tq->flat_buff_n);

	// destroy the linkl after flattening
	BP_DestroyLinkL(tq, BP_TRUE);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Metadata %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	printf("\n%s[+] MariaDB Column Metadata for %s",     tab_buff, table_column_metadata->column_name);
	printf("\n%s   table_catalog:             %s",       tab_buff, table_column_metadata->table_catalog);
	printf("\n%s   table_schema:              %s",       tab_buff, table_column_metadata->table_schema);
	printf("\n%s   table_name:                %s",       tab_buff, table_column_metadata->table_name);
	printf("\n%s   column_name:               %s",       tab_buff, table_column_metadata->column_name);
	printf("\n%s   ordinal_position:          %lli",     tab_buff, table_column_metadata->ordinal_position);
	printf("\n%s   column_default:            %s",       tab_buff, table_column_metadata->column_default);
	printf("\n%s   is_nullable:               %s",       tab_buff, table_column_metadata->is_nullable);
	printf("\n%s   data_type:                 %s",       tab_buff, table_column_metadata->data_type);
	printf("\n%s   character_maximum_length:  %lli",     tab_buff, table_column_metadata->character_maximum_length);
	printf("\n%s   character_octet_length:    %lli",     tab_buff, table_column_metadata->character_octet_length);
	printf("\n%s   numeric_precision:         %lli",     tab_buff, table_column_metadata->numeric_precision);
	printf("\n%s   numeric_scale:             %lli",     tab_buff, table_column_metadata->numeric_scale);
	printf("\n%s   datetime_precision:        %lli",     tab_buff, table_column_metadata->datetime_precision);
	printf("\n%s   character_set_name:        %s",       tab_buff, table_column_metadata->character_set_name);
	printf("\n%s   collation_name:            %s",       tab_buff, table_column_metadata->collation_name);
	printf("\n%s   column_type:               %s",       tab_buff, table_column_metadata->column_type);
	printf("\n%s   column_key:                %s",       tab_buff, table_column_metadata->column_key);
	printf("\n%s   extra:                     %s",       tab_buff, table_column_metadata->extra);
	printf("\n%s   privileges:                %s",       tab_buff, table_column_metadata->privileges);
	printf("\n%s   column_comment:            %s",       tab_buff, table_column_metadata->column_comment);

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(bpdb->system_hreg);

	// return indicating success
	return ERR_SUCCESS;


}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% BPDB Errors/Warnings/Notices %%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Adds a message to an arbitrary branch (this is the overlying routine used by the error/warning/notice routines
// below.
BP_ERROR_T BP_DBAddMsg
(
	P_BPDB                                bpdb,
	BPDB_ADD_MSG_TYPE                     msg_type,
	char                                 *message,
	P_BPDB_INTERNAL_MESSAGE_EXTRA_INFO    ext_info_in_out
)
{

	// run basic checks
	if(!bpdb)
		return ERR_FAILURE;
	if(!message)
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(message, bpstrlen(message), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Conditionals %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// table lookup
	char *table_str = NULL;

	// set the table str based on the msg type
	switch(msg_type)
	{
		case BPDB_ADD_MSG_TYPE_NOTICE:
			table_str = "/notices";
			break;
		case BPDB_ADD_MSG_TYPE_WARNING:
			table_str = "/warnings";
			break;
		case BPDB_ADD_MSG_TYPE_ERROR:
			table_str = "/errors";
			break;
		default:
			break;
	}


	// set msg string
	char *msg_str   = message;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Setup Table if Necessary %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create messages registry if it doesn't exist
	if(!bpdb->messages_reg)
	{
		bpdb->messages_reg_total_count = 0;
		bpdb->messages_reg = BP_CreateHashTableRegistry();
	}


	// attempt to lookup errors table by path
	P_BP_HTKE error_table = BP_HashRegLookupTableByPathQuery(bpdb->messages_reg, table_str);
	if(!error_table)
	{

		// attempt to add the errors table
		BP_HashRegAddTableByPathQuery(bpdb->messages_reg, table_str);

		// attempt lookup again
		error_table = BP_HashRegLookupTableByPathQuery(bpdb->messages_reg, table_str);
		if(!error_table)
			return ERR_FAILURE;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Table For New Error %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create random sha1 key
	char * random_sha1_key = BP_GenRandomSHA1();

	// set the addition path
	char *add_new_table_path[] =
	{
		"errors",
		random_sha1_key,
		NULL
	};

	// inner table path with table index set
	char * inner_table_path = BP_HashRegPathArrayToStringLookup(add_new_table_path);
	if(!inner_table_path)
	{
		bpfree(random_sha1_key);
		return ERR_FAILURE;
	}

	// create new table
	if(!BP_HashRegAddTableByPathQuery(bpdb->messages_reg, inner_table_path))
	{
		bpfree(random_sha1_key);
		bpfree(inner_table_path);
		return ERR_FAILURE;
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Data To Table %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// -- add "/errors/sha1/msg" item to the registry by path as utf8 string
	if(!BP_HashRegAddDataToTableByPathQuery(bpdb->messages_reg, inner_table_path, "msg", BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING, msg_str, bpstrlen(msg_str)))
	{
		bpfree(random_sha1_key);
		bpfree(inner_table_path);
		return ERR_FAILURE;
	}

	// -- add "/errors/sha1/ext_info" item as binary blob (copied into registry)
	if(ext_info_in_out)
	{

		if(!BP_HashRegAddDataToTableByPathQuery(bpdb->messages_reg, inner_table_path, "ext_info", BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB, (char *) ext_info_in_out, sizeof(BPDB_INTERNAL_MESSAGE_EXTRA_INFO)))
		{
			bpfree(random_sha1_key);
			bpfree(inner_table_path);
			return ERR_FAILURE;
		}
	}



	// destroy random sha1 key
	bpfree(random_sha1_key);
	bpfree(inner_table_path);

	// return indicating success
	return ERR_SUCCESS;

}



// add a parse error to the parse output hash table registry (since buff position can be 0, we need to use -1 as an unset value)
BP_ERROR_T BP_DBAddErrorMsg
(
	P_BPDB                                bpdb,
	char                                 *message,
	P_BPDB_INTERNAL_MESSAGE_EXTRA_INFO    ext_info_in_out
)
{

	// return the add message result
	return BP_DBAddMsg
	(
		bpdb,
		BPDB_ADD_MSG_TYPE_ERROR,
		message,
		ext_info_in_out
	);

}

// add a parse warning to the parse output hash table registry
BP_ERROR_T BP_DBAddWarningMsg
(
	P_BPDB                                bpdb,
	char *                                message,
	P_BPDB_INTERNAL_MESSAGE_EXTRA_INFO    ext_info_in_out
)
{

	// return the add message result
	return BP_DBAddMsg
	(
		bpdb,
		BPDB_ADD_MSG_TYPE_WARNING,
		message,
		ext_info_in_out
	);


}

// add a parse notice to the parse output hash table registry
BP_ERROR_T BP_DBAddNoticeMsg
(
	P_BPDB                                bpdb,
	char *                                message,
	P_BPDB_INTERNAL_MESSAGE_EXTRA_INFO    ext_info_in_out
)
{


	// return the add message result
	return BP_DBAddMsg
	(
		bpdb,
		BPDB_ADD_MSG_TYPE_NOTICE,
		message,
		ext_info_in_out
	);

}



// -- message display routines -------------------

// display notices
BP_ERROR_T BP_DBDisplayNotices(P_BPDB bpdb)
{


	// return indicating success
	return ERR_SUCCESS;

}


// display warnings
BP_ERROR_T BP_DBDisplayWarnings(P_BPDB bpdb)
{


	// return indicating success
	return ERR_SUCCESS;
}


// display errors
BP_ERROR_T BP_DBDisplayErrors(P_BPDB bpdb)
{

	if(!bpdb)
		return ERR_FAILURE;
	if(!bpdb->messages_reg)
		return ERR_FAILURE;


	// retrieve the iter table
	P_BP_HTKE msg_iter_table = BP_HashRegLookupTableByPathQuery(bpdb->messages_reg, "/errors");
	if(!msg_iter_table)
	{
		return ERR_FAILURE;
	}
	if(!msg_iter_table->data)
	{
		return ERR_FAILURE;
	}

	// iterate through the messages and display error message
	BP_HASH_ITER_OPEN((P_BP_HTKE) msg_iter_table->data, msg_entry)
	{

		if(msg_entry->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{

			char *msg = NULL;

			// iterate through internal data
			BP_HASH_ITER_OPEN((P_BP_HTKE) msg_entry->data, msg_item)
			{
				if(ht_memmatch_key(msg_item, "msg", 3))
				{
					msg = (char *) msg_item->data;
				}
			}
			BP_HASH_ITER_CLOSE;

			printf("\n [!!] Bpdb Error: %s", msg);
		}

	}
	BP_HASH_ITER_CLOSE;

	// return indicating success
	return ERR_SUCCESS;
}


// display all the parser messages
BP_ERROR_T BP_DBDisplayMessages(P_BPDB bpdb)
{
	if(!bpdb)
		return ERR_FAILURE;

	BP_DBDisplayErrors(bpdb);
	BP_DBDisplayWarnings(bpdb);
	BP_DBDisplayNotices(bpdb);

	return ERR_SUCCESS;
}

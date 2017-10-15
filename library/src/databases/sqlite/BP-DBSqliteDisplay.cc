/*
 * BP-DBSqliteDisplay.cc
 *
 *  Created on: Aug 4, 2015
 *      Author: root
 */


// include main functions
#include "../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display SQLite Handle %%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Display sqli.
BP_ERROR_T BP_SQLiteDisplay
(
		P_BP_DB_SQLITE sqli
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

	// sqlite handle direct items
	printf("\n [+] SQLite Handle %p", sqli);
	printf("\n     db:                                      %p", sqli->db);
	printf("\n     db_file:                                 %s", sqli->db_file);
	printf("\n     stmt:                                    %p", sqli->stmt);
	printf("\n     last_query_uuid:                         %s", sqli->last_query_uuid);
	printf("\n     last_query_param_n:                      %u", sqli->last_query_param_n);
	printf("\n     queries:                                 %p", sqli->queries);
	printf("\n     query_result_cache:                      %p", sqli->query_result_cache);
	printf("\n     last_query_results:                      %p", sqli->last_query_results);
	printf("\n     last_result_query_uuid:                  %s", sqli->last_result_query_uuid);
	printf("\n     last_result_retrieved_n:                 %u", sqli->last_result_retrieved_n);
	printf("\n     last_transaction_record_count_retrieved: %u", sqli->last_transaction_record_count_retrieved);
	printf("\n     log_db:                                  %p", sqli->log_db);
	printf("\n     log_db_file:                             %s", sqli->log_db_file);
	printf("\n     only_log_errors:                         %s", sqli->log_db_file ? "yes":"no");
	printf("\n     log_query_parameters:                    %s", sqli->log_query_parameters ? "yes":"no");
	printf("\n     log_max:                                 %u", sqli->log_max_n);
	printf("\n     query_result_cache:                      %p", sqli->query_result_cache);
	printf("\n     log_db:                                  %p", sqli->log_db);
	printf("\n     log_db_file:                             %s", sqli->log_db_file);
	printf("\n     only_log_errors:                         %s", sqli->only_log_errors ? "yes" : "no");
	printf("\n     log_query_parameters:                    %s", sqli->log_query_parameters ? "yes" : "no");
	printf("\n     log_max_n:                               %u", sqli->log_max_n);
	printf("\n     last_query_reported_error:               %s", sqli->last_query_reported_error ? "yes" : "no");
	printf("\n     last_query_ran_ok:                       %s", sqli->last_query_ran_ok ? "yes" : "no");
	printf("\n     last_query_generated_results:            %s", sqli->last_query_generated_results ? "yes" : "no");
	printf("\n     total_transactions_started:              %u", sqli->total_transactions_started);
	printf("\n     total_transactions_exited_ok:            %u", sqli->total_transactions_exited_ok);
	printf("\n     total_transactions_exited_failed:        %u", sqli->total_transactions_exited_failed);
	printf("\n     total_queries_executed_n:                %u", sqli->total_queries_executed_n);
	printf("\n     total_queries_prepared_n:                %u", sqli->total_queries_prepared_n);
	printf("\n     total_queries_failed_n:                  %u", sqli->total_queries_failed_n);
	printf("\n     total_results_retrieved_n:               %u", sqli->total_results_retrieved_n);
	printf("\n     system_hreg:                             %p", sqli->system_hreg);


	printf("\n");

	// display schema if any tables are found
	BP_SQLiteDisplaySchema
	(
			sqli,
			sqli->schema
	);
	printf("\n");


	// printf("\n Displaying queries?");
	// BP_HashRegDisplay(sqli->queries, BP_FALSE, BP_TRUE );

	// display query stack if it's set
	BP_SQLiteDisplayQueryStack
	(
			sqli,
			sqli->queries
	);
	printf("\n");

	// printf("\n Displaying results?: ");
	// BP_HashRegDisplay(sqli->query_result_cache, BP_FALSE, BP_TRUE);

	// display results if there are any
	BP_SQLiteDisplayResultStack
	(
			sqli,
			sqli->query_result_cache
	);
	printf("\n");



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Leave Critical Section and Exit %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(sqli->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}


// Display sqlite query stack.
BP_ERROR_T BP_SQLiteDisplayQueryStack
(
		P_BP_DB_SQLITE           sqli,
		P_BP_HASH_TABLE_REGISTRY query_stack
)
{

	// ensure we have a sqli handle
	if(!sqli)
		return ERR_FAILURE;

	// ensure we have a query stack
	if(!query_stack)
		return ERR_FAILURE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Create Border Set for SFS %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(sqli->system_hreg);

    // create border set to use for borderization
    BP_STRING_ROW_BORDER_SET border_set;
    BP_ZERO_STACK(border_set);

    // border left and right strings
    border_set.border_left  = " | ";
    border_set.border_right = "|";

    // border top items
    border_set.border_top_enabled           = BP_TRUE;
    border_set.border_top_char              = "-";
    border_set.border_top_left_corner       = " +";
    border_set.border_top_right_corner      = "+";

    // border bottom items
    border_set.border_bottom_enabled        = BP_TRUE;
    border_set.border_bottom_char           = "-";
    border_set.border_bottom_left_corner    = " +";
    border_set.border_bottom_right_corner   = "+";


    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Add Row Groups %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // set string formatting session
    P_BP_SF_SESSION sfs = BP_CreateStringFormattingSession("query display", &border_set);

    // add row group to string formatting session (no borders)
    BP_AddRowGroupToSFSession         (sfs, "rowgroup1",  NULL);
    BP_AddRowToRowGroupSFSession      (sfs, "rowgroup1", "row1", NULL);
    BP_AddColumnGroupToRowSFSession   (sfs, "rowgroup1", "row1", "col_group1", NULL);
    BP_AddColumnToColumnGroupSFSession(sfs, "rowgroup1", "row1", "col_group1", "column1", NULL);

    // Generates a string entry from an ascii character string

    // BP_AddRowGroupToSFSession(sfs, "query_parms", NULL);

    // set inidial colors
    BP_COLOR_CODE_SET_T initial_colors;
    BP_ZERO_STACK(initial_colors);

    initial_colors.fg = BP_COLOR_CODE_FG_RED;
    initial_colors.bg = BP_COLOR_CODE_BG_BLUE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Entries to SFS Via Loops %%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// test iter loop
	if(sqli->queries)
	BP_HASH_ITER_OPEN(sqli->queries->hive, outer_entry)
	{



		// test iter loop
		BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) sqli->queries->hive->data, query_entry)
		{

			// ensure we have a query entry
			if(!query_entry)
				continue;

			// set initial colors
			initial_colors.fg = BP_COLOR_CODE_FG_GREEN;
			initial_colors.bg = BP_COLOR_CODE_BG_BLACK;

			// create buffer to store sql
			char * uuid_display_buff = (char *) bpcalloc(BP_UUID_LEN+50, 1);

			// copy in data
			snprintf(uuid_display_buff, BP_UUID_LEN+49, "Query UUID: %s", query_entry->key);


			// set uuid entry
			P_BP_SFS_STRING_ENTRY query_uuid_entry = BP_GenerateStringEntryFromCharString
			(
					sfs,
					"query_uuid",
					uuid_display_buff,
					bpstrlen(uuid_display_buff),
					120,
					&initial_colors,
					0
			);

			// add uuid column
	        BP_AddStringEntryToColumnSFSession
	        (
	                sfs,
	                "rowgroup1",
	                "row1",
	                "col_group1",
	                "column1",
					query_uuid_entry
	        );


	        // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	        // %%% Lookup and Display SQL %%%%%%%%%%%%%%%%%%%
	        // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	    	// test iter loop
	    	BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) query_entry->data, inner_entry)
	    	{

				if(!inner_entry)
					continue;

	    		// ensure we have a key to work with
	    		if(inner_entry->key)
				{

					// 1) add sql entry if found
					if(memcmp(inner_entry->key, "sql", bpstrlen(inner_entry->key)) == 0)
					{

						// set initial colors
						initial_colors.fg = BP_COLOR_CODE_FG_GREEN;
						initial_colors.bg = BP_COLOR_CODE_BG_NONE;



						// create buffer to store sql
						char * sql_display_buff = (char *) bpstrdup((char *) inner_entry->data);

						// strip out newlines if they're found
						char * sql_display_buff_newlines_removed = BP_FindRemoveBin(sql_display_buff, bpstrlen(sql_display_buff), "\n", 1, 10000);

						// ensure we have a final (replacements ok).  If not set from original display buff.
						if(sql_display_buff_newlines_removed)
						{

							bpfree(sql_display_buff);
							sql_display_buff = sql_display_buff_newlines_removed;
						}

						// strip double spaces
						do
						{

							// check for buffer
							sql_display_buff_newlines_removed = BP_FindReplace(sql_display_buff, "  ", " ", 1, 100000);

							// if we have a stripped buffer
							if(sql_display_buff_newlines_removed)
							{

								// ensure we have the final buff
								bpfree(sql_display_buff);
								sql_display_buff = NULL;

								// set final buffer
								sql_display_buff = sql_display_buff_newlines_removed;

							}
							else
								break;

						} while(1);



						// create final buffer
						char * sql_display_buff_final = (char *) bpcalloc(bpstrlen(sql_display_buff)+100, 1);



						// set the query
						sprintf(sql_display_buff_final, "Query:    %s", sql_display_buff);


						// set uuid entry
						P_BP_SFS_STRING_ENTRY sql_display_entry = BP_GenerateStringEntryFromCharString
						(
								sfs,
								"sql",
								(char *) sql_display_buff_final,
								bpstrlen((char *) sql_display_buff_final),
								bpstrlen((char *) sql_display_buff_final),
								&initial_colors,
								1
						);

						// add uuid column
						BP_AddStringEntryToColumnSFSession
						(
								sfs,
								"rowgroup1",
								"row1",
								"col_group1",
								"column1",
								sql_display_entry
						);

						// destroy original string if it wasn't overwritten
						if(sql_display_buff)
						{
							bpfree(sql_display_buff);
							sql_display_buff = NULL;
						}

						// destroy overwrite if necessary
						if(sql_display_buff_newlines_removed)
						{
							bpfree(sql_display_buff_newlines_removed);
							sql_display_buff_newlines_removed = NULL;
						}

					}

				}

	    	} BP_HASH_ITER_CLOSE;


	    	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	    	// %%% Lookup and Display Record Parameters  %%%%
	    	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// test iter loop
			BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) query_entry->data, inner_entry)
			{

				// ensure we have an entry
				if(!inner_entry)
					continue;

				// ensure we have a key
				if(inner_entry->key)
				{

					// iterate through parameters
					if(memcmp(inner_entry->key, "params", bpstrlen(inner_entry->key)) == 0)
					{

						// set parameter iterator
						size_t param_n = 0;

						// walk entries
						BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) inner_entry->data, parameter_entry)
						{

							// zero out the row name
							char row_name[128];
							BP_ZERO_STACK(row_name);

							// add the rown
							sprintf(row_name, "param_row_%u", param_n);

							//add row
							BP_AddRowToRowGroupSFSession      (sfs, "rowgroup1", row_name, NULL);
							BP_AddColumnGroupToRowSFSession   (sfs, "rowgroup1", row_name, "col_group1", NULL);
							BP_AddColumnToColumnGroupSFSession(sfs, "rowgroup1", row_name, "col_group1", "column1", NULL);

							// set initial colors
							initial_colors.fg = BP_COLOR_CODE_FG_BLUE;
							initial_colors.bg = BP_COLOR_CODE_BG_NONE;


							// set the parmaeter name from the key
							char * parm_name = parameter_entry->key;

							// retrieve/convert parameter
							char * parm_data = BP_SQLiteConvertParameterToCStringForDisplay((P_BP_SQLITE_QUERY_PARAM) parameter_entry->data);

							// allocate space of parameter
							char * final_param_buff = (char *) bpcalloc
							(
								bpstrlen( (char *) parm_data ? parm_data : "" ) + bpstrlen((char *) parm_name) + 50,
								1
							);

							// prepare final buffer
							sprintf(final_param_buff, "Parameter %s = %s", parameter_entry->key, parm_data ? parm_data : "");

							// set uuid entry
							P_BP_SFS_STRING_ENTRY sql_display_entry = BP_GenerateStringEntryFromCharString
							(
									sfs,
									"sql",
									(char *) final_param_buff,
									bpstrlen((char *) final_param_buff),
									bpstrlen((char *) final_param_buff),
									&initial_colors,
									1
							);

							// add uuid column
							BP_AddStringEntryToColumnSFSession
							(
									sfs,
									"rowgroup1",
									row_name,
									"col_group1",
									"column1",
									sql_display_entry
							);



							// increment the parameter count
							param_n++;

						} BP_HASH_ITER_CLOSE;

					}

				}

			} BP_HASH_ITER_CLOSE;


			// ensure we have a display buffer
			if(uuid_display_buff)
			{
				bpfree(uuid_display_buff);
				uuid_display_buff = NULL;
			}

		} BP_HASH_ITER_CLOSE;


	} BP_HASH_ITER_CLOSE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Gen Final and Display %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// generate final output
	BP_GenerateFinalOutputSFSession(sfs);

	// display the sfs
	BP_DisplayFinalOutputSFSession(sfs, BP_TRUE);

	// destroy the string formatting session before exiting
	BP_DestroyStringFormattingSession(sfs);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Leave Critical Section and Exit %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(sqli->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}


// Display sqlite result stack.
BP_ERROR_T BP_SQLiteDisplayResultStack
(
		P_BP_DB_SQLITE           sqli,
		P_BP_HASH_TABLE_REGISTRY result_stack
)
{


	if(!sqli)
		return ERR_FAILURE;
	if(!result_stack)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Create Border Set for SFS %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(sqli->system_hreg);

	// create border set to use for borderization
	BP_STRING_ROW_BORDER_SET border_set;
	BP_ZERO_STACK(border_set);

	// border left and right strings
	border_set.border_left  = "";
	border_set.border_right = "";

	// border top items
	border_set.border_top_enabled           = BP_TRUE;
	border_set.border_top_char              = "-";
	border_set.border_top_left_corner       = "|";
	border_set.border_top_right_corner      = "|";

	// border bottom items
	border_set.border_bottom_enabled        = BP_TRUE;
	border_set.border_bottom_char           = "-";
	border_set.border_bottom_left_corner    = "|";
	border_set.border_bottom_right_corner   = "|";


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Second Border Set for SFS (Column) %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create border set to use for borderization
	BP_STRING_ROW_BORDER_SET border_set_col;
	BP_ZERO_STACK(border_set_col);

	// border left and right strings
	border_set_col.border_left  = "|";
	border_set_col.border_right = "|";

	// border top items
	border_set_col.border_top_enabled           = BP_TRUE;
	border_set_col.border_top_char              = "-";
	border_set_col.border_top_left_corner       = "+";
	border_set_col.border_top_right_corner      = "+";

	// border bottom items
	border_set_col.border_bottom_enabled        = BP_TRUE;
	border_set_col.border_bottom_char           = "-";
	border_set_col.border_bottom_left_corner    = "+";
	border_set_col.border_bottom_right_corner   = "+";


	// set initial colors
	BP_COLOR_CODE_SET_T initial_colors;
	BP_ZERO_STACK(initial_colors);

	initial_colors.fg = BP_COLOR_CODE_FG_BLUE;
	initial_colors.bg = BP_COLOR_CODE_BG_NONE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Row Groups %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set string formatting session
	P_BP_SF_SESSION sfs = BP_CreateStringFormattingSession("query display", NULL);

	// add row group to string formatting session (no borders)
	BP_AddRowGroupToSFSession         (sfs, "rowgroup1",  NULL);

	// Don't uncomment this.  This is here for easy dev lookup:
	// ---------------------------------------------------------
	// BP_AddRowToRowGroupSFSession      (sfs, "rowgroup1", "row1", NULL);
	// BP_AddColumnGroupToRowSFSession   (sfs, "rowgroup1", "row1", "col_group1", NULL);
	// BP_AddColumnToColumnGroupSFSession(sfs, "rowgroup1", "row1", "col_group1", "column1", NULL);

	// Generates a string entry from an ascii character string
	// BP_AddRowGroupToSFSession(sfs, "query_parms", NULL);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Database Table Column Header Names %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this is set to true after the banner is displayed
	BP_BOOL banner_displayed = BP_FALSE;

	// allocate space for an index var
	char index_val[256];
	BP_ZERO_STACK(index_val);

	// this is used to  preserve the uuid of the query we're displaying
	// as we traverse through the results data structure.  It is displayed
	// at the end of the routine and should not be freed (as it is only a
	// reference).
	char * preserved_uuid_ref = NULL;

	// togle used for setting uuid
	BP_BOOL uuid_has_been_preserved = BP_FALSE;

	// Loop through first entry in order to add column header.  This
	// was moved from the main loop (below) for the purpose of making the code
	// easier to read.  It was complex enough to give me a headache, so
	// i though it would be a gentle provision to move it here for clarity.
	if(sqli->queries)
	BP_HASH_ITER_OPEN(result_stack->hive, outer_entry)
	{

		// printf("\n outer_entry: %s", outer_entry->key);
		// test iter loop
		BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) outer_entry->data, result_entry)
		{

			// ensure we have a result
			if(!result_entry)
				continue;


			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Display Banner %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


			// table path
			char *table_path[] = {"results", result_entry->key, NULL, NULL};

			// if the uuid has not been preserved, we can preserve it (will
			// always be the first key)
			if(!uuid_has_been_preserved)
			{
				preserved_uuid_ref = result_entry->key;
				uuid_has_been_preserved = BP_TRUE;
			}

			// set keyed entry
			P_BP_HASH_TABLE_KEYED_ENTRY keyed_entry = NULL;
			keyed_entry = BP_HashRegLookupTableByPath(result_stack, (char **) &table_path);

			// exit if we have nothing to display
			if(!keyed_entry)
				BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(sqli->system_hreg, ERR_FAILURE);

			// zero out the index stack
			BP_ZERO_STACK(index_val);

			// add row group (first record)
			BP_AddRowToRowGroupSFSession      (sfs, "rowgroup1", "row1", NULL);
			BP_AddColumnGroupToRowSFSession   (sfs, "rowgroup1", "row1", "col_group1", NULL);

			// flag indicating whether or not columns have been added below
			BP_BOOL columns_have_been_added = BP_FALSE;

			// display keys
			BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) keyed_entry->data, result_entry_members)
			{

				// printf("\n result_entry_members: %s", result_entry_members->key);

				if(!columns_have_been_added)
				{
					// BP_AddColumnGroupToRowSFSession   (sfs, "rowgroup1", "row1", "col_group1", NULL);
					BP_AddColumnToColumnGroupSFSession
					(
							sfs,
							"rowgroup1",
							"row1",
							"col_group1",
							result_entry_members->key,
							&border_set_col
					);
					columns_have_been_added = BP_TRUE;
				}
				// Iterate to add records
				BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) result_entry_members->data, result_entry_members_inner)
				{

					// printf("\n result_entry_members_inner: %s", result_entry_members_inner->key);

					// set top as green
					initial_colors.fg = BP_COLOR_CODE_FG_GREEN;

					// set uuid entry
					P_BP_SFS_STRING_ENTRY display_record = BP_GenerateStringEntryFromCharString
					(
							sfs,
							"entry_record",
							(char *) result_entry_members_inner->key,
							bpstrlen((char *) result_entry_members_inner->key),
							30,
							&initial_colors,
							0
					);

					// add column to column group
					BP_AddColumnToColumnGroupSFSession
					(
							sfs,
							"rowgroup1",
							"row1",
							"col_group1",
							result_entry_members_inner->key,
							&border_set_col
					);

					// add column
					BP_AddStringEntryToColumnSFSession
					(
							sfs,
							"rowgroup1",
							"row1",
							"col_group1",
							result_entry_members_inner->key,
							display_record
					);


				} BP_HASH_ITER_CLOSE;

				// display flag
				banner_displayed = BP_TRUE;

				// exit if necessary
				if(banner_displayed)
					break;


			} BP_HASH_ITER_CLOSE;

			if(banner_displayed)
				break;

		} BP_HASH_ITER_CLOSE;

		if(banner_displayed)
			break;

	} BP_HASH_ITER_CLOSE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Entries to SFS Via Loops %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This is the total row count which is used to determine how many total
	// rows are in the output. (set count to 2 since we added the headers earlier)
	size_t total_row_count = 2;

	// Loop through first entry in order to add column header.  This
	// was moved from the main loop (below) for the purpose of making the code
	// easier to read.  It was complex enough to give me a headache, so
	// i though it would be a gentle provision to move it here for clarity.
	if(sqli->queries)
	BP_HASH_ITER_OPEN(result_stack->hive, outer_entry)
	{

		// printf("\n outer_entry: %s", outer_entry->key);
		// test iter loop
		BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) outer_entry->data, result_entry)
		{

			// ensure we have a result
			if(!result_entry)
				continue;

			// set the row buff
			char row_buff[256];
			BP_ZERO_STACK(row_buff);

			// sprint in the row number
			sprintf((char *) &row_buff, "row%u", total_row_count);

			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Display Banner %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// table path
			char *table_path[] = {"results", result_entry->key, NULL, NULL};


			// set keyed entry
			P_BP_HASH_TABLE_KEYED_ENTRY keyed_entry = NULL;
			keyed_entry = BP_HashRegLookupTableByPath(result_stack, (char **) &table_path);

			// exit if we have nothing to display
			if(!keyed_entry)
				BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(sqli->system_hreg, ERR_FAILURE);

			// zero out the index stack
			BP_ZERO_STACK(index_val);

			// flag indicating whether or not columns have been added below
			BP_BOOL columns_have_been_added = BP_FALSE;

			// add row group (first record)
			BP_AddRowToRowGroupSFSession      (sfs, "rowgroup1", (char *) &row_buff, NULL);
			BP_AddColumnGroupToRowSFSession   (sfs, "rowgroup1", (char *) &row_buff, "col_group1", NULL);

			// display keys
			BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) keyed_entry->data, result_entry_members)
			{

				if(!columns_have_been_added)
				{
					// add a column to the session
					BP_AddColumnToColumnGroupSFSession
					(
							sfs,
							"rowgroup1",
							(char *) &row_buff,
							"col_group1",
							result_entry_members->key,
							&border_set_col
					);
				}

				// if the columns haven't been added, add them
				if(!columns_have_been_added)
				{
					// Iterate to add records
					BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) result_entry_members->data, result_entry_members_inner)
					{
						// add a column to the session
						BP_AddColumnToColumnGroupSFSession
						(
								sfs,
								"rowgroup1",
								(char *) &row_buff,
								"col_group1",
								result_entry_members_inner->key,
								&border_set_col
						);

					} BP_HASH_ITER_CLOSE;

					columns_have_been_added = BP_TRUE;
				}

				// Iterate to add records
				BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) result_entry_members->data, result_entry_members_inner)
				{

					// printf("\n result_entry_members_inner: %s", result_entry_members_inner->key);

					// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
					// %%% Attempt Data/Metadata Lookups %%%%%%%%%%%%%%%%%%%%%
					// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

					// result stack used for lookups
					char *result_stack[10];
					BP_ZERO_STACK(result_stack);

					result_stack[0];

					// set top as green
					initial_colors.fg = BP_COLOR_CODE_FG_GREEN;


					// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
					// %%% Gather Data adn Metadata %%%%%%%%%%%%%%%%%%%%%%%%%
					// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


					// declare data and metadata
					P_BP_DB_SQLITE_RESULT_METADATA metadata = NULL;
					void *                         data     = NULL;

					// extract metadata and data
					BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) result_entry_members_inner->data, result_entry_members_deepest)
					{

						// set metadata if matched
						if(strncmp(result_entry_members_deepest->key, "record_metadata", bpstrlen(result_entry_members_deepest->key)) == 0)
						{

							// set metadata
							metadata = (P_BP_DB_SQLITE_RESULT_METADATA) result_entry_members_deepest->data;

						}

						// set record data if matched
						if(strncmp(result_entry_members_deepest->key, "record_data", bpstrlen(result_entry_members_deepest->key)) == 0)
						{

							// set data
							data = result_entry_members_deepest->data;

						}

					} BP_HASH_ITER_CLOSE;



					// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
					// %%% Add Data to Table from Record %%%%%%%%%%%%%%%%%%%%
					// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

					// set final buffer from data
					char * final_buff = BP_SQLiteConvertResultDataToCStringForDisplay(metadata, data);


					// set uuid entry
					P_BP_SFS_STRING_ENTRY display_record = BP_GenerateStringEntryFromCharString
					(
							sfs,
							"entry_record",
							(char *) final_buff,
							bpstrlen((char *) final_buff),
							30,
							&initial_colors,
							0
					);

					// add column
					BP_AddStringEntryToColumnSFSession
					(
							sfs,
							"rowgroup1",
							(char *) &row_buff,
							"col_group1",
							result_entry_members_inner->key,
							display_record
					);

					// destroy the bufffer
					bpfree(final_buff);


				} BP_HASH_ITER_CLOSE;

				// incremtn the row count
				total_row_count++;

			} BP_HASH_ITER_CLOSE;

		} BP_HASH_ITER_CLOSE;




	} BP_HASH_ITER_CLOSE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Gen Final and Display on Loop %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// generate final output
	BP_GenerateFinalOutputSFSession(sfs);


	// verbosity
	printf("\n [+] Displaying Query Results for Query Matching UUID: %s", preserved_uuid_ref);

	// display the sfs
	BP_DisplayFinalOutputSFSession(sfs, BP_TRUE);

	// destroy the string formatting session
	BP_DestroyStringFormattingSession(sfs);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Leave Critical Section and Exit %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(sqli->system_hreg);



	// return indicating success
	return ERR_SUCCESS;
}


// Display sqlite result stack.
BP_ERROR_T BP_SQLiteDisplaySchema
(
		P_BP_DB_SQLITE           sqli,
		P_BP_HASH_TABLE_REGISTRY schema
)
{


	if(!sqli)
		return ERR_FAILURE;
	if(!schema)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Create Border Set for SFS %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(sqli->system_hreg);

	// create border set to use for borderization
	BP_STRING_ROW_BORDER_SET border_set;
	BP_ZERO_STACK(border_set);

	// border left and right strings
	border_set.border_left  = "";
	border_set.border_right = "";

	// border top items
	border_set.border_top_enabled           = BP_TRUE;
	border_set.border_top_char              = "";
	border_set.border_top_left_corner       = "|";
	border_set.border_top_right_corner      = "|";

	// border bottom items
	border_set.border_bottom_enabled        = BP_TRUE;
	border_set.border_bottom_char           = "";
	border_set.border_bottom_left_corner    = "|";
	border_set.border_bottom_right_corner   = "|";


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Create Border Set for SFS %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create border set to use for borderization
	BP_STRING_ROW_BORDER_SET border_set_col;
	BP_ZERO_STACK(border_set_col);

	// border left and right strings
	border_set_col.border_left  = "|";
	border_set_col.border_right = "|";

	// border top items
	border_set_col.border_top_enabled           = BP_TRUE;
	border_set_col.border_top_char              = "-";
	border_set_col.border_top_left_corner       = "+";
	border_set_col.border_top_right_corner      = "+";

	// border bottom items
	border_set_col.border_bottom_enabled        = BP_TRUE;
	border_set_col.border_bottom_char           = "-";
	border_set_col.border_bottom_left_corner    = "+";
	border_set_col.border_bottom_right_corner   = "+";



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Row Groups %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set string formatting session
	P_BP_SF_SESSION sfs = BP_CreateStringFormattingSession("query display", NULL);

	// add row group to string formatting session (no borders)
	BP_AddRowGroupToSFSession         (sfs, "rowgroup1",  NULL);
	BP_AddRowToRowGroupSFSession      (sfs, "rowgroup1", "row1", NULL);
	BP_AddColumnGroupToRowSFSession   (sfs, "rowgroup1", "row1", "col_group1", NULL);
	BP_AddColumnToColumnGroupSFSession(sfs, "rowgroup1", "row1", "col_group1", "column1", NULL);

	// Generates a string entry from an ascii character string
	// BP_AddRowGroupToSFSession(sfs, "query_parms", NULL);

	// set inidial colors
	BP_COLOR_CODE_SET_T initial_colors;
	BP_ZERO_STACK(initial_colors);

	initial_colors.fg = BP_COLOR_CODE_FG_BLUE;
	initial_colors.bg = BP_COLOR_CODE_BG_NONE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt Linear Table Name Display %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this is set to true after the banner is displayed
	BP_BOOL banner_displayed = BP_FALSE;

	// test iter loop
	if(sqli->queries)
	BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) schema->hive, outer_entry)
	{


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Display Outer Table Info %%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// ensure we have a banner display
		if(!banner_displayed)
		{

			printf("\n [+] Schema: Listing tables.");
			banner_displayed = BP_TRUE;
		}

		// display the table name
		printf("\n     [+] Schema Table: %s", outer_entry->key);
		printf("\n");


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Display Table Details %%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// zero out table path
		char * table_path[10];
		BP_ZERO_STACK(table_path);

		// display inner data
		printf("\n     {");
		BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) outer_entry->data, data_marker_entry)
		{


			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Lookup Metadata %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// set lookup path for metadata
			table_path[0] = outer_entry->key;
			table_path[1] = data_marker_entry->key;
			table_path[2] = "record_metadata";
			table_path[3] = NULL;

			// attempt metadata lookup
			P_BP_HASH_TABLE_KEYED_ENTRY metadata_lookup = BP_HashRegLookupTableEntryByPath(schema, (char **) &table_path);
			if(!metadata_lookup)
				continue;

			// set metadata from result data
			P_BP_DB_SQLITE_RESULT_METADATA metadata = (P_BP_DB_SQLITE_RESULT_METADATA) metadata_lookup->data;
			if(!metadata)
				continue;


			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Lookup Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// set lookup path for metadata
			table_path[0] = outer_entry->key;
			table_path[1] = data_marker_entry->key;
			table_path[2] = "record_data";
			table_path[3] = NULL;

			// attempt metadata lookup
			P_BP_HASH_TABLE_KEYED_ENTRY data_lookup = BP_HashRegLookupTableEntryByPath(schema, (char **) &table_path);
			if(!data_lookup)
				continue;
			if(!data_lookup->data)
				continue;

			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Convert Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// convert data to string using metadata
			char * data_as_string = BP_SQLiteConvertResultDataToCStringForDisplay
			(
					metadata,
					data_lookup->data
			);


			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Display Converted Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


			// display the data marker entry
			printf("\n          %s: ", data_marker_entry->key);

			// add newline before sql entry to make things easier to view
			if(bpstrlen(data_marker_entry->key) == 3)
			{
				if(memcmp(data_marker_entry->key, "sql", 3) == 0)
				{
					printf("\n");

					char * tmp_sql = BP_FindReplace(data_as_string, "\n", "\n\t\t", 3, 10000);
					if(tmp_sql)
					{
						printf("\n\t\t %s", tmp_sql);
						bpfree(tmp_sql);
					}

				}
			}
			else
			{
				// display data string
				printf("%s", data_as_string);
			}



		} BP_HASH_ITER_CLOSE;
		printf("\n     }");
		printf("\n");

	} BP_HASH_ITER_CLOSE;

	// set display banner flag to false
	banner_displayed = BP_FALSE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Gen Final and Display %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// generate final output
	BP_GenerateFinalOutputSFSession(sfs);

	// display the sfs
	BP_DisplayFinalOutputSFSession(sfs, BP_TRUE);


	/*
	// BP_DisplayStringFormattingSession(sfs)
    // create new display settings and zero out data
    BP_SFS_INFO_DISPLAY_SETTINGS display_settings;

    // simple macro to initialize all elements in the display
    // settings to true.
    BP_SFS_DISPLAY_ALL_TRUE_STACK(display_settings);

    // display the sfs
    BP_DisplayStringFormattingSession
	(
			sfs,
			&display_settings
	);

	*/
	printf("\n Got this sfs!!?!: %p", sfs);

    // destroy the string formatting session before exiting
   	BP_DestroyStringFormattingSession(sfs);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Leave Critical Section and Exit %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(sqli->system_hreg);



	// return indicating success
	return ERR_SUCCESS;
}

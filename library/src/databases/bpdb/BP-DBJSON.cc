/*
 * BP-DBJSON.cc
 *
 *  Created on: Oct 12, 2016
 *      Author: root
 */

// include blockparty
#include "../../include/BP-Main.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% JSON Query Part Separators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to separate right join string elements into their parts (stored in join_data_in_out)
BP_ERROR_T BP_DBJSONSeparateRightJoinStringIntoParts
(
	P_BPDB_SQLGEN syngen,
	P_BPDB_SELECT_JOIN join_data_in_out,
	char *string_elements
)
{

	// set syntax generator
	if(!syngen)
		return ERR_FAILURE;
	if(!syngen->bpdb)
		return ERR_FAILURE;

	// set bpdb
	P_BPDB bpdb = syngen->bpdb;

	// run basic checks
	if(!bpdb)
		return ERR_FAILURE;
	if(!join_data_in_out)
		return ERR_FAILURE;
	if(!string_elements)
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(string_elements, bpstrlen(string_elements), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	printf("\n Attempting to separate join parts here woo: %s", string_elements);

	// calculate elements length
	size_t string_element_len = bpstrlen(string_elements);

	// shortest element is 7 bytes:
	// t.i=o.i
	if(string_element_len < 7)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Basic Buffer Analysis %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // create ascii configuration
    BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION ascii_config;
    BP_ZERO_STACK(ascii_config);

    // set some flags in the structure (default is BP_FALSE aka 0)
    ascii_config.count_ascii                      = BP_TRUE;
    ascii_config.count_extended                   = BP_TRUE;
    ascii_config.count_individual_characters      = BP_TRUE;

    // attempt to generate analysis
    P_BP_BUFFER_ANALYSIS buff_analysis = BP_CreateBufferAnalysis
    (
		(unsigned char *) string_elements,
		(size_t) bpstrlen(string_elements),
		&ascii_config
    );

    // ensure we created a new buffer analysis
    if(!buff_analysis)
    	return ERR_FAILURE;

    // there should always be one equals sign
    if(buff_analysis->ascii->char_counts['='] != 1)
    {
    	BP_DestroyBufferAnalysis(buff_analysis);
    	return ERR_FAILURE;
    }

    // there should always be two periods
    if(buff_analysis->ascii->char_counts['.'] != 2)
    {
    	BP_DestroyBufferAnalysis(buff_analysis);
        return ERR_FAILURE;
    }

    // destroy the buffer analysis
    BP_DestroyBufferAnalysis(buff_analysis);
    buff_analysis = NULL;


    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Strip Spaces and Create Buffer %%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // create initial parse buffer by stripping strings
    char * parse_buff = BP_FindRemoveBin
    (
		string_elements,
		string_element_len,
		" ",
		1,
		BP_KILOBYTE
    );

    // ensure the parse buffer was created ok
    if(!parse_buff)
    	return ERR_FAILURE;


    // set elements
    char ** elements = BP_ExplodeBufferBin(parse_buff, bpstrlen(parse_buff), "=", 1, 3);
    if(!elements)
    	return ERR_FAILURE;

    // ensure that there are only two elements in the array
    if(BP_CountNullTerminatedPtrArray(elements) != 2)
    {

    	BP_DestroyPointerArrayNullTerminated((void **) elements);
    	bpfree(elements);
    	bpfree(parse_buff);
    	return ERR_FAILURE;
    }


    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Run Element Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // table separator must exist in the middle of the string, not the
    // front or back.
    if
	(
			elements[0][0] == '.' || elements[0][bpstrlen(elements[0])-1] == '.' ||
			elements[1][0] == '.' || elements[1][bpstrlen(elements[1])-1] == '.'
	)
    {
    	BP_DestroyPointerArrayNullTerminated((void **) elements);
		bpfree(elements);
		bpfree(parse_buff);
		return ERR_FAILURE;
    }

    // split the first table since we need to extract the table name
    char ** table_col_split = BP_SplitBuffer(elements[0], ".");
    if(!table_col_split)
    {
    	BP_DestroyPointerArrayNullTerminated((void **) elements);
		bpfree(elements);
		bpfree(parse_buff);
		return ERR_FAILURE;
    }

    // perform assignments
    join_data_in_out->table_name          = ht_strdup(syngen->system_hreg, table_col_split[0]);
    join_data_in_out->front_join          = ht_strdup(syngen->system_hreg, elements[0]);
    join_data_in_out->comparison_operator = ht_strdup(syngen->system_hreg, "=");
    join_data_in_out->tail_join           = ht_strdup(syngen->system_hreg, elements[1]);
    join_data_in_out->join_type           = BPDB_SELECT_JOIN_TYPE_RIGHT_JOIN;


    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Cleanup And Exit %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // destroy data
	BP_DestroyPointerArrayNullTerminated((void **) elements);
    BP_DestroyPointerArrayNullTerminated((void **) table_col_split);
	bpfree(elements);
	bpfree(table_col_split);
	bpfree(parse_buff);

	// return indicating success
	return ERR_SUCCESS;

}

// attempt to separate right join string elements into their parts (stored in join_data_in_out)
BP_ERROR_T BP_DBJSONSeparateLeftJoinStringIntoParts
(
	P_BPDB_SQLGEN syngen,
	P_BPDB_SELECT_JOIN join_data_in_out,
	char *string_elements
)
{

	// set syntax generator
	if(!syngen)
		return ERR_FAILURE;
	if(!syngen->bpdb)
		return ERR_FAILURE;

	// set bpdb
	P_BPDB bpdb = syngen->bpdb;


	// run basic checks
	if(!bpdb)
		return ERR_FAILURE;
	if(!join_data_in_out)
		return ERR_FAILURE;
	if(!string_elements)
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(string_elements, bpstrlen(string_elements), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	printf("\n Attempting to separate join parts here woo: %s", string_elements);

	// calculate elements length
	size_t string_element_len = bpstrlen(string_elements);

	// shortest element is 7 bytes:
	// t.i=o.i
	if(string_element_len < 7)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Basic Buffer Analysis %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // create ascii configuration
    BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION ascii_config;
    BP_ZERO_STACK(ascii_config);

    // set some flags in the structure (default is BP_FALSE aka 0)
    ascii_config.count_ascii                      = BP_TRUE;
    ascii_config.count_extended                   = BP_TRUE;
    ascii_config.count_individual_characters      = BP_TRUE;

    // attempt to generate analysis
    P_BP_BUFFER_ANALYSIS buff_analysis = BP_CreateBufferAnalysis
    (
		(unsigned char *) string_elements,
		(size_t) bpstrlen(string_elements),
		&ascii_config
    );

    // ensure the analysis was created ok
    if(!buff_analysis)
    	return ERR_FAILURE;

    // there should always be one equals sign
    if(buff_analysis->ascii->char_counts['='] != 1)
    {
    	BP_DestroyBufferAnalysis(buff_analysis);
    	return ERR_FAILURE;
    }

    // there should always be two periods
    if(buff_analysis->ascii->char_counts['.'] != 2)
    {
    	BP_DestroyBufferAnalysis(buff_analysis);
        return ERR_FAILURE;
    }

    // destroy the buffer analysis
    BP_DestroyBufferAnalysis(buff_analysis);
    buff_analysis = NULL;



    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Strip Spaces and Create Buffer %%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // create initial parse buffer by stripping strings
    char * parse_buff = BP_FindRemoveBin
    (
		string_elements,
		string_element_len,
		" ",
		1,
		BP_KILOBYTE
    );

    // ensure the parse buffer was created ok
    if(!parse_buff)
    	return ERR_FAILURE;

    // set elements
    char ** elements = BP_ExplodeBufferBin(parse_buff, bpstrlen(parse_buff), "=", 1, 3);
    if(!elements)
    	return ERR_FAILURE;

    // ensure that there are only two elements in the array
    if(BP_CountNullTerminatedPtrArray(elements) != 2)
    {

    	BP_DestroyPointerArrayNullTerminated((void **) elements);
    	bpfree(elements);
    	bpfree(parse_buff);
    	return ERR_FAILURE;
    }


    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Run Element Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // table separator must exist in the middle of the string, not the
    // front or back.
    if
	(
			elements[0][0] == '.' || elements[0][bpstrlen(elements[0])-1] == '.' ||
			elements[1][0] == '.' || elements[1][bpstrlen(elements[1])-1] == '.'
	)
    {
    	BP_DestroyPointerArrayNullTerminated((void **) elements);
		bpfree(elements);
		bpfree(parse_buff);
		return ERR_FAILURE;
    }

    // split the first table since we need to extract the table name
    char ** table_col_split = BP_SplitBuffer(elements[0], ".");
    if(!table_col_split)
    {
    	BP_DestroyPointerArrayNullTerminated((void **) elements);
		bpfree(elements);
		bpfree(parse_buff);
		return ERR_FAILURE;
    }

    // perform assignments
    join_data_in_out->table_name          = ht_strdup(syngen->system_hreg, table_col_split[0]);
    join_data_in_out->front_join          = ht_strdup(syngen->system_hreg, elements[0]);
    join_data_in_out->comparison_operator = ht_strdup(syngen->system_hreg, "=");
    join_data_in_out->tail_join           = ht_strdup(syngen->system_hreg, elements[1]);
    join_data_in_out->join_type           = BPDB_SELECT_JOIN_TYPE_LEFT_JOIN;


    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Cleanup And Exit %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // destroy data
	BP_DestroyPointerArrayNullTerminated((void **) elements);
    BP_DestroyPointerArrayNullTerminated((void **) table_col_split);
	bpfree(elements);
	bpfree(table_col_split);
	bpfree(parse_buff);

	// return indicating success
	return ERR_SUCCESS;

}

// attempt to separate where-clause string elements into their parts (stored in join_data_in_out)
BP_ERROR_T BP_DBJSONSeparateWhereClauseStringIntoParts
(
	P_BPDB_SQLGEN              syngen,
	P_BPDB_SELECT_WHERE select_clause_in_out,
	char *              string_elements
)
{

	// run basic checks
	if(!syngen)
		return ERR_FAILURE;
	if(!syngen->bpdb)
		return ERR_FAILURE;

	// create reference
	P_BPDB bpdb = syngen->bpdb;

	if(!select_clause_in_out)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBJSONSeparateWhereClauseStringIntoParts was called with a NULL select_clause_in_out parameter.");
		return ERR_FAILURE;
	}

	if(!string_elements)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBJSONSeparateWhereClauseStringIntoParts was called with a NULL string_elements parameter.");
		return ERR_FAILURE;
	}

	if(!BP_StringIsReasonablePrintableString(string_elements, bpstrlen(string_elements), BP_FALSE, BP_FALSE))
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBJSONSeparateWhereClauseStringIntoParts was called with unreasonable string elements string (contains bad binary data)");
		return ERR_FAILURE;
	}

	// create a temporary tq for allocating data (easy cleanup on failure)
	P_BP_LINKL tmp_tq = BP_CreateLinkL(".", BP_TRUE);
	if(!tmp_tq)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBJSONSeparateWhereClauseStringIntoParts attempted to create a tracking linkl but failed.  Likely out of memory.");
		return ERR_FAILURE;
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Step 1: Trim Whitespace from Element Ends %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to trip whitespace
	char * trimmed_elem_string = (char *) BP_StringTrimWhitespace
	(
		string_elements,
		BP_STRING_TRIM_DIRECTION_BOTH,
		BP_KILOBYTE * 100
	);

	// ensure we have a trimmed string
	if(!trimmed_elem_string)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBJSONSeparateWhereClauseStringIntoParts failed to create duplicated/trimmed string.  Likely out of memory.");
		return ERR_FAILURE;
	}
	if(!bpstrlen(trimmed_elem_string))
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBJSONSeparateWhereClauseStringIntoParts duplicated trimmed string was zero length empty string.");
		bpfree(trimmed_elem_string);
		return ERR_FAILURE;
	}

	// set trimmed length
	size_t trimmed_len = bpstrlen(trimmed_elem_string);


	// These will be filled out by the logic below are assigned via a
	// tailqueue allocator.  They are deallocated on exit.

	char *front_part_table_name  = NULL;
	char *front_part_column_name = NULL;
	char *tail_part_raw_value    = NULL;
	char *tail_part_table_name   = NULL;
	char *tail_part_column_name  = NULL;
	char *separator_token        = NULL;

	// this is set to the current parse position (do not free this)
	char *curr_parse_position = trimmed_elem_string;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Step 2: Check for a Prefixing Conjunction (AND/OR) %%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// conjunction markers
	BP_BOOL has_conjunction     = BP_FALSE;
	BP_BOOL conjunction_is_and  = BP_FALSE;
	BP_BOOL conjunction_is_or   = BP_FALSE;


	// search for OR conjunction
	if(trimmed_len >= 3)
	{
		if(memcmp(curr_parse_position, "OR ", 3) == 0)
		{
			has_conjunction     = BP_TRUE;
			conjunction_is_or   = BP_TRUE;
			curr_parse_position = &curr_parse_position[3];
		}
	}

	// search for AND conjunction
	if(trimmed_len >= 4)
	{
		if(memcmp(curr_parse_position, "AND ", 4) == 0)
		{
			has_conjunction    = BP_TRUE;
			conjunction_is_and = BP_TRUE;
			curr_parse_position = &curr_parse_position[4];
		}
	}


	// move past all whitespace
	size_t chr_pos = 0;
	if(has_conjunction)
	{

		for(; curr_parse_position[chr_pos]; chr_pos++)
		{

			switch(curr_parse_position[chr_pos])
			{

				// pass whitespace chars
				case ' ':
				case '\t':
				case '\n':
				case '\r':
					continue;
					break;

				// default
				default:
					break;

			}

			// break the loop
			break;

		}

		// set current parse position after we've moved past all whitespace
		curr_parse_position = &curr_parse_position[chr_pos];
		if(!curr_parse_position)
		{
			bpfree(trimmed_elem_string);
			BP_DBAddErrorMsg(bpdb, "BP_DBJSONSeparateWhereClauseStringIntoParts found end of string at unexpected position.");
			return ERR_FAILURE;
		}

	}




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Step 2: Attempt to Retrieve Front Part Table Name %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Front part must be: "tablename." where tablename is a valid table name

	// attempt to lookup index for first dot
	int dot_str_lookup = BP_LookupFirstCharacterOffsetInReasonableString(curr_parse_position, '.', BP_STRING_OFFSET_LOOKUP_DIRECTION_FORWARD);
	if(dot_str_lookup <= 0)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBJSONSeparateWhereClauseStringIntoParts failed to lookup tablename-column separator char '.' in where clause");

		bpfree(trimmed_elem_string);
		BP_DestroyLinkL(tmp_tq, BP_TRUE);
		return ERR_FAILURE;
	}

	// create front part table name here
	front_part_table_name = tq_strndup(tmp_tq, curr_parse_position, (size_t) dot_str_lookup, BPLN);
	if(!front_part_table_name)
	{

		BP_DBAddErrorMsg(bpdb, "BP_DBJSONSeparateWhereClauseStringIntoParts failed to duplicate table name.  Likely out of memory.");
		bpfree(trimmed_elem_string);
		BP_DestroyLinkL(tmp_tq, BP_TRUE);
		return ERR_FAILURE;
	}

	// ensure that the front part is reasonable for a table name
	if(!BP_StringIsReasonableForTableNameDB(front_part_table_name, bpstrlen(front_part_table_name)))
	{

		// create new temporary string
		char tmp_str[512+bpstrlen(front_part_table_name)];
		BP_ZERO_STACK(tmp_str);

		// add and report error
		sprintf((char *) &tmp_str, "BP_DBJSONSeparateWhereClauseStringIntoParts tried to validate table '%s' and failed", front_part_table_name);
		BP_DBAddErrorMsg(bpdb, (char *) &tmp_str);

		// destroy dangling data
		bpfree(trimmed_elem_string);
		BP_DestroyLinkL(tmp_tq, BP_TRUE);
		return ERR_FAILURE;
	}

	// attempt lookup (this verifies that the table already exists)
	if(!BP_DBLookupTableByName(bpdb, front_part_table_name))
	{

		// create new temporary string
		char tmp_str[512+bpstrlen(front_part_table_name)];
		BP_ZERO_STACK(tmp_str);

		// add and report error
		sprintf((char *) &tmp_str, "BP_DBJSONSeparateWhereClauseStringIntoParts tried to lookup table '%s' and failed", front_part_table_name);
		BP_DBAddErrorMsg(bpdb, (char *) &tmp_str);


		bpfree(trimmed_elem_string);
		BP_DestroyLinkL(tmp_tq, BP_TRUE);
		return ERR_FAILURE;
	}

	// set the current parse position
	curr_parse_position = &curr_parse_position[dot_str_lookup+1];



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Step 3: Attempt to Retrieve Front Part Column Name %%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// iterate through buffer from current position
	chr_pos = 0;
	for(; curr_parse_position[chr_pos]; chr_pos++)
	{

		// continue loop if character is upper or lowercase ascii
		if(BP_IS_LOWER_ASCII(curr_parse_position[chr_pos]))
			continue;
		if(BP_IS_UPPER_ASCII(curr_parse_position[chr_pos]))
			continue;
		if(curr_parse_position[chr_pos] == '_')
			continue;

		// break
		break;

	}

	// if we hit the end of the string without finding a non-matching character, we
	// know our query is bad.
	if(!curr_parse_position[chr_pos])
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBJSONSeparateWhereClauseStringIntoParts found end of string at unexpected position.");
		bpfree(trimmed_elem_string);
		BP_DestroyLinkL(tmp_tq, BP_TRUE);
		return ERR_FAILURE;
	}

	// set the front part of the column name here
	front_part_column_name = tq_strndup(tmp_tq, curr_parse_position, chr_pos, BPLN);
	if(!front_part_column_name)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBJSONSeparateWhereClauseStringIntoParts failed to duplicate front_part_column_name.  Likely out of memory.");
		bpfree(trimmed_elem_string);
		BP_DestroyLinkL(tmp_tq, BP_TRUE);
		return ERR_FAILURE;
	}

	// ensure the column name has a length
	if(!bpstrlen(front_part_column_name))
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBJSONSeparateWhereClauseStringIntoParts duplicated a column name, but the result was zero length string.");
		bpfree(trimmed_elem_string);
		BP_DestroyLinkL(tmp_tq, BP_TRUE);
		return ERR_FAILURE;
	}

	// perform column lookup
	if(!BP_DBLookupTableColumnByName(bpdb, front_part_table_name, front_part_column_name))
	{

		// create new temporary string
		char tmp_str[512+bpstrlen(front_part_column_name)+bpstrlen(front_part_table_name)];
		BP_ZERO_STACK(tmp_str);

		// add and report error
		sprintf((char *) &tmp_str, "BP_DBJSONSeparateWhereClauseStringIntoParts tried to lookup column '%s' within table '%s' and failed", front_part_column_name, front_part_table_name);
		BP_DBAddErrorMsg(bpdb, (char *) &tmp_str);


		bpfree(trimmed_elem_string);
		BP_DestroyLinkL(tmp_tq, BP_TRUE);
		return ERR_FAILURE;
	}

	// set the current parse position
	curr_parse_position = &curr_parse_position[chr_pos];



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Step 4: Retrieve Separating Operator %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// move past all whitespace
	chr_pos = 0;
	for(; curr_parse_position[chr_pos]; chr_pos++)
	{

		switch(curr_parse_position[chr_pos])
		{

			// pass whitespace chars
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				continue;
				break;

			// default
			default:
				break;

		}

		// break the loop
		break;

	}

	// set current parse position after we've moved past all whitespace
	curr_parse_position = &curr_parse_position[chr_pos];
	if(!curr_parse_position)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBJSONSeparateWhereClauseStringIntoParts found end of string at unexpected position.");
		return ERR_FAILURE;
	}

	// get the length of the current parse position
	size_t curr_parse_position_len = bpstrlen(curr_parse_position);


	// -- holds valid identifier information ----

	// these values are set based on what is present
	size_t  identifier_len                 = 0;
	BP_BOOL found_valid_identifier         = BP_FALSE;
	BP_BOOL identifier_contains_not_prefix = BP_FALSE;

	if(curr_parse_position_len >= 4)
	{

		if(memcmp(curr_parse_position, "NOT ", 4) == 0)
		{

			// mark as containing not prefix
			identifier_contains_not_prefix = BP_TRUE;

			// move parse position forward 4 bytes
			curr_parse_position      =  &curr_parse_position[4];
			curr_parse_position_len  -= 4;

		}

	}

	// if we have no length, the query is bad
	if(!curr_parse_position_len)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBJSONSeparateWhereClauseStringIntoParts found end of string at unexpected position.");
		bpfree(trimmed_elem_string);
		BP_DestroyLinkL(tmp_tq, BP_TRUE);
		return ERR_FAILURE;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Comparison Operator Checks %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	if(BP_StringIsReasonableForMariadbComparisonOperatorDB(curr_parse_position, 6))
	{
		found_valid_identifier = BP_TRUE;
		identifier_len = 6;
	}
	else if(BP_StringIsReasonableForMariadbComparisonOperatorDB(curr_parse_position, 5))
	{
		found_valid_identifier = BP_TRUE;
		identifier_len = 5;
	} else if(BP_StringIsReasonableForMariadbComparisonOperatorDB(curr_parse_position, 4))
	{
		found_valid_identifier = BP_TRUE;
		identifier_len = 4;
	} else if(BP_StringIsReasonableForMariadbComparisonOperatorDB(curr_parse_position, 3))
	{
		found_valid_identifier = BP_TRUE;
		identifier_len = 3;
	} else if(BP_StringIsReasonableForMariadbComparisonOperatorDB(curr_parse_position, 2))
	{
		found_valid_identifier = BP_TRUE;
		identifier_len = 2;
	} else if(BP_StringIsReasonableForMariadbComparisonOperatorDB(curr_parse_position, 1))
	{
		found_valid_identifier = BP_TRUE;
		identifier_len = 1;
	}

	// if we haven't found a valid identifier
	if(!found_valid_identifier)
	{

		// add error
		BP_DBAddErrorMsg(bpdb, "BP_DBJSONSeparateWhereClauseStringIntoParts was unable to find valid comparison operator for where clause.");
		bpfree(trimmed_elem_string);
		BP_DestroyLinkL(tmp_tq, BP_TRUE);
		return ERR_FAILURE;

	}

	// --- at this point we've found a valid identifier ---

	// create stack buffer to hold temporary identifier buffer
	char tmp_identifier_buff[64];
	BP_ZERO_STACK(tmp_identifier_buff);

	if(identifier_contains_not_prefix)
	{

		// move the parse position down 4 bytes if we have a not prefix
		curr_parse_position = &curr_parse_position[-4];

		// increment the length by four
		identifier_len += 4;

	}

	// copy in identifier
	memcpy((char *) &tmp_identifier_buff, curr_parse_position, identifier_len);

	// set the separator token
	separator_token = tq_strdup(tmp_tq, (char *) &tmp_identifier_buff, BPLN);

	// move position forward
	curr_parse_position = &curr_parse_position[identifier_len];


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Step X: Extract Tail Part Literals if Set %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// move past all whitespace
	chr_pos = 0;
	for(; curr_parse_position[chr_pos]; chr_pos++)
	{

		switch(curr_parse_position[chr_pos])
		{

			// pass whitespace chars
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				continue;
				break;

			// default
			default:
				break;

		}

		// break the loop
		break;

	}

	// set current parse position after we've moved past all whitespace
	curr_parse_position = &curr_parse_position[chr_pos];
	if(!curr_parse_position)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBJSONSeparateWhereClauseStringIntoParts found end of string at unexpected position.");
		bpfree(trimmed_elem_string);
		BP_DestroyLinkL(tmp_tq, BP_TRUE);
		return ERR_FAILURE;
	}

	// update parse position length
	curr_parse_position_len = bpstrlen(curr_parse_position);


	// Valid Types Are:
	//                negative integer: -1
	//                positive integer: 1
	//                hex_string:       0xFF00AABB
	//                quoted_literal:   'some value with only ascii chars'
	//                table_and_colum:  table_name.column_name


	// type identifiers (set through if/else logic below)
	BP_BOOL is_negative_integer     = BP_FALSE;
	BP_BOOL is_positive_integer     = BP_FALSE;
	BP_BOOL is_hex_string           = BP_FALSE;
	BP_BOOL is_hex_upper            = BP_FALSE;
	BP_BOOL is_hex_lower            = BP_FALSE;
	BP_BOOL is_quoted_literal       = BP_FALSE;
	BP_BOOL is_quoted_literal_empty = BP_FALSE;
	BP_BOOL is_table_and_column     = BP_FALSE;


	// check hex type first
	if(!tail_part_raw_value)
	if(curr_parse_position_len >= 3)
	{
		if(memcmp(curr_parse_position, "0x", 2) == 0)
		{
			if(BP_StringIsReasonableForHexLowerASCII(&curr_parse_position[2],curr_parse_position_len-2))
			{
				is_hex_string = BP_TRUE;
				is_hex_lower  = BP_TRUE;
				tail_part_raw_value = tq_strdup(tmp_tq, curr_parse_position, BPLN);
			}
			else if(BP_StringIsReasonableForHexUpperASCII(&curr_parse_position[2],curr_parse_position_len-2))
			{
				is_hex_string = BP_TRUE;
				is_hex_upper  = BP_TRUE;
				tail_part_raw_value = tq_strdup(tmp_tq, curr_parse_position, BPLN);

			}
		}
	}

	// check if it's a negative integer
	if(!tail_part_raw_value)
	if(curr_parse_position_len >= 2)
	{
		if( curr_parse_position[0] == '-' )
		{
			if(BP_StringIsReasonableForSignedIntegerASCII(curr_parse_position, curr_parse_position_len))
			{
				is_negative_integer = BP_TRUE;
				tail_part_raw_value = tq_strdup(tmp_tq, curr_parse_position, BPLN);
			}
		}
	}

	// check if it's a positive integer
	if(!tail_part_raw_value)
	if(curr_parse_position_len >= 1)
	{

		if(BP_StringIsReasonableForUnsignedIntegerASCII(curr_parse_position, curr_parse_position_len))
		{
			is_positive_integer = BP_TRUE;
			tail_part_raw_value = tq_strdup(tmp_tq, curr_parse_position, BPLN);
		}

	}

	// if it's not a numeric type calculate
	if
	(
		!is_negative_integer &&
		!is_positive_integer &&
		!is_hex_string       &&
		!is_hex_upper        &&
		!is_hex_lower		 &&
		!tail_part_raw_value
	)
	{

		if(curr_parse_position_len == 2)
		{
			if(memcmp(curr_parse_position, "''", 2) == 0)
			{
				is_quoted_literal = BP_TRUE;
				is_quoted_literal_empty = BP_TRUE;
				tail_part_raw_value = tq_strdup(tmp_tq, curr_parse_position, BPLN);
			}
		}
		else if(curr_parse_position[0] == '\'')
		{

			// unquote the string (works with single or double quotes)
			char *unquoted_literal = BP_UnquoteQuotedString(curr_parse_position, curr_parse_position_len);

			// set our value if we have an unquoted literal
			if(unquoted_literal)
			{

				// mark as quoted literal
				is_quoted_literal = BP_TRUE;

				// duplicate the string and free the literal
				tail_part_raw_value = tq_strdup(tmp_tq, unquoted_literal, BPLN);
				bpfree(unquoted_literal);

			}

		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Step X: Extract Tail Part Table Reference if Set %%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if it's not a numeric type calculate
	if
	(
		!is_negative_integer &&
		!is_positive_integer &&
		!is_hex_string       &&
		!is_hex_upper        &&
		!is_hex_lower		 &&
		!is_quoted_literal   &&
		!tail_part_raw_value
	)
	{

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Step 2: Attempt to Retrieve Front Part Table Name %%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// attempt to lookup index for first dot
		dot_str_lookup = BP_LookupFirstCharacterOffsetInReasonableString(curr_parse_position, '.', BP_STRING_OFFSET_LOOKUP_DIRECTION_FORWARD);
		if(dot_str_lookup <= 0)
		{

			// add error message
			BP_DBAddErrorMsg(bpdb, "BP_DBJSONSeparateWhereClauseStringIntoParts detected that second part of where clause was table.column type, but no table separator '.' value could be found.");

			bpfree(trimmed_elem_string);
			BP_DestroyLinkL(tmp_tq, BP_TRUE);
			return ERR_FAILURE;
		}

		// create front part table name here
		tail_part_table_name = tq_strndup(tmp_tq, curr_parse_position, (size_t) dot_str_lookup, BPLN);
		if(!tail_part_table_name)
		{

			// add error message
			BP_DBAddErrorMsg(bpdb, "BP_DBJSONSeparateWhereClauseStringIntoParts failed to duplicate the tail-part table name.  Likely out of memory. ");


			bpfree(trimmed_elem_string);
			BP_DestroyLinkL(tmp_tq, BP_TRUE);
			return ERR_FAILURE;
		}


		// ensure that the front part is reasonable for a table name
		if(!BP_StringIsReasonableForTableNameDB(tail_part_table_name, bpstrlen(tail_part_table_name)))
		{

			// create tmp str and zero it
			char tmp_str[512+bpstrlen(tail_part_table_name)];
			BP_ZERO_STACK(tmp_str);

			// add create error message
			sprintf((char *) &tmp_str, "BP_DBJSONSeparateWhereClauseStringIntoParts attempted to validate/filter table name '%s', but it failed filter checks.", tail_part_table_name);
			BP_DBAddErrorMsg(bpdb, (char *) &tmp_str);

			bpfree(trimmed_elem_string);
			BP_DestroyLinkL(tmp_tq, BP_TRUE);
			return ERR_FAILURE;

		}

		// attempt lookup (this verifies that the table already exists)
		if(!BP_DBLookupTableByName(bpdb, tail_part_table_name))
		{

			// create tmp str and zero it
			char tmp_str[512+bpstrlen(tail_part_table_name)];
			BP_ZERO_STACK(tmp_str);

			// add create error message
			sprintf((char *) &tmp_str, "BP_DBJSONSeparateWhereClauseStringIntoParts attempted to lookup table name '%s', but it failed filter checks.", tail_part_table_name);
			BP_DBAddErrorMsg(bpdb, (char *) &tmp_str);


			bpfree(trimmed_elem_string);
			BP_DestroyLinkL(tmp_tq, BP_TRUE);
			return ERR_FAILURE;
		}

		// set the current parse position
		curr_parse_position = &curr_parse_position[dot_str_lookup+1];


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Step 3: Attempt to Retrieve Front Part Column Name %%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// iterate through buffer from current position
		chr_pos = 0;
		for(; curr_parse_position[chr_pos]; chr_pos++)
		{

			// continue loop if character is upper or lowercase ascii
			if(BP_IS_LOWER_ASCII(curr_parse_position[chr_pos]))
				continue;
			if(BP_IS_UPPER_ASCII(curr_parse_position[chr_pos]))
				continue;
			if(curr_parse_position[chr_pos] == '_')
				continue;

			// break
			break;

		}

		// if the current position isn't the end of the string, we have a problem
		if(curr_parse_position[chr_pos])
		{
			BP_DBAddErrorMsg(bpdb, "BP_DBJSONSeparateWhereClauseStringIntoParts reached unexpected end of string.");
			bpfree(trimmed_elem_string);
			BP_DestroyLinkL(tmp_tq, BP_TRUE);
			return ERR_FAILURE;
		}

		// set the tail part of the column name here
		tail_part_column_name = tq_strndup(tmp_tq, curr_parse_position, chr_pos, BPLN);
		if(!tail_part_column_name)
		{
			BP_DBAddErrorMsg(bpdb, "BP_DBJSONSeparateWhereClauseStringIntoParts failed to duplicate tail-part column name.  Likely out of memory..");
			bpfree(trimmed_elem_string);
			BP_DestroyLinkL(tmp_tq, BP_TRUE);
			return ERR_FAILURE;
		}

		// ensure the column name has a length
		if(!bpstrlen(tail_part_column_name))
		{
			BP_DBAddErrorMsg(bpdb, "BP_DBJSONSeparateWhereClauseStringIntoParts duplicated tail-part column name but it was a zero length string.  Bad value.");
			bpfree(trimmed_elem_string);
			BP_DestroyLinkL(tmp_tq, BP_TRUE);
			return ERR_FAILURE;
		}

		// perform column lookup
		if(!BP_DBLookupTableColumnByName(bpdb, tail_part_table_name, tail_part_column_name))
		{

			// create tmp str and zero it
			char tmp_str[512+bpstrlen(tail_part_table_name)];
			BP_ZERO_STACK(tmp_str);

			// add create error message
			sprintf((char *) &tmp_str, "BP_DBJSONSeparateWhereClauseStringIntoParts attempted to lookup column name '%s' but failed.", tail_part_column_name);
			BP_DBAddErrorMsg(bpdb, (char *) &tmp_str);

			bpfree(trimmed_elem_string);
			BP_DestroyLinkL(tmp_tq, BP_TRUE);
			return ERR_FAILURE;
		}

		// mark it as a table and column
		is_table_and_column = BP_TRUE;

	}




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Step X: Run Final Checks and Exit %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create combined front part table.value string
	P_BP_LINKL tmp_combiner = BP_CreateLinkL(".", BP_TRUE);

	// add string elements
	tq_strdup(tmp_combiner, front_part_table_name, BPLN);
	tq_strdup(tmp_combiner, ".", BPLN);
	tq_strdup(tmp_combiner, front_part_column_name, BPLN);

	// join string elements
	BP_LinkLFlattenJoinStringElements(tmp_combiner);

	// set front combined filed
	select_clause_in_out->field_name = ht_strdup(syngen->system_hreg, (char *) tmp_combiner->flat_buff);

	// tail combined field (only set if necessary)
	char *value = NULL;

	// destroy the linkl
	BP_DestroyLinkL(tmp_combiner, BP_TRUE);


	// assign type based on flags which were set duing parsing
	if(is_negative_integer)
	{

		// set value as positive integer
		select_clause_in_out->value_type = BPDB_SELECT_WHERE_VALUE_TYPE_NEGATIVE_INTEGER;

		// set value
		select_clause_in_out->value     = (char *) ht_calloc(syngen->system_hreg, sizeof(long int), 1);

		// set tmp pointer so conversion can take place
		long int * long_integer_tmp =  (long int *) select_clause_in_out->value;

		// set pointer
		*long_integer_tmp = strtol(tail_part_raw_value, NULL, 10);

		// set the value length
		select_clause_in_out->value_len = sizeof(long int);

	}
	else if(is_positive_integer)
	{

		// set value as positive integer
		select_clause_in_out->value_type = BPDB_SELECT_WHERE_VALUE_TYPE_POSITIVE_INTEGER;

		// set value
		select_clause_in_out->value     = (char *) ht_calloc(syngen->system_hreg, sizeof(long int), 1);

		// set tmp pointer so conversion can take place
		long int * long_integer_tmp =  (long int *) select_clause_in_out->value;
		*long_integer_tmp = strtol(tail_part_raw_value, NULL, 10);

		// convert string and store value
		// BP_StringToSignedLongBase10(tail_part_raw_value, bpstrlen(tail_part_raw_value), (long int *) select_clause_in_out->value);

		// set the value length
		select_clause_in_out->value_len = sizeof(long int);

	}
	else if(is_hex_string)
	{

		// set type as hex string
		select_clause_in_out->value_type = BPDB_SELECT_WHERE_VALUE_TYPE_HEX_STRING;

		// set value by parsing the string into a binary blob (store the bin length as the value length)

		// set bin blob
		char *tmp_bin_blob = BP_StringHexToBinaryBlob(tail_part_raw_value, (size_t *) &select_clause_in_out->value_len);

		if(tmp_bin_blob)
		{

			// set value (one extra byte can be allocated here without causing issues to force null termination (no exceptions) )
			select_clause_in_out->value = (char *) ht_calloc(syngen->system_hreg, select_clause_in_out->value_len+1, 1);

			// copy in the data
			if(select_clause_in_out->value)
				memcpy(select_clause_in_out->value, tmp_bin_blob, select_clause_in_out->value_len);

			// destroy the binary blob
			bpfree(tmp_bin_blob);

		}

	}
	else if(is_quoted_literal)
	{

		// set value type as quoted literal
		select_clause_in_out->value_type = BPDB_SELECT_WHERE_VALUE_TYPE_QUOTED_LITERAL;

		// set value
		select_clause_in_out->value     = ht_strdup(syngen->system_hreg, tail_part_raw_value);
		select_clause_in_out->value_len = bpstrlen(tail_part_raw_value);

	}
	else if(is_table_and_column)
	{

		// set type
		select_clause_in_out->value_type = BPDB_SELECT_WHERE_VALUE_TYPE_TABLE_AND_COLUMN;

		// recreate joiner
		tmp_combiner = BP_CreateLinkL(".", BP_TRUE);

		// add string elements
		tq_strdup(tmp_combiner, tail_part_table_name, BPLN);
		tq_strdup(tmp_combiner, ".", BPLN);
		tq_strdup(tmp_combiner, tail_part_column_name, BPLN);

		// join string elements
		BP_LinkLFlattenJoinStringElements(tmp_combiner);

		// set front combined filed
		char *tail_combined_field = ht_strdup(syngen->system_hreg, (char *) tmp_combiner->flat_buff);

		// set value
		select_clause_in_out->value     = tail_combined_field;
		select_clause_in_out->value_len = bpstrlen(tail_combined_field);

		// destroy the linkl
		BP_DestroyLinkL(tmp_combiner, BP_TRUE);

	}
	else
	{

		// add error and exit
		BP_DBAddErrorMsg(bpdb, "BP_DBJSONSeparateWhereClauseStringIntoParts reports that tail part was unable to be parsed into valid type.  Bad where clause detected.");
		bpfree(trimmed_elem_string);
		BP_DestroyLinkL(tmp_tq, BP_TRUE);
		return ERR_FAILURE;

	}


	// set conjunction from separator token (should always be set by this point)
	select_clause_in_out->limiter = ht_strdup(syngen->system_hreg, separator_token);

	// fill in conjunction
	if(has_conjunction)
	{
		if(conjunction_is_or)
		{
			select_clause_in_out->conjunction = ht_strdup(syngen->system_hreg, "OR");
		}
		else if (conjunction_is_and)
		{
			select_clause_in_out->conjunction = ht_strdup(syngen->system_hreg, "AND");
		}
	}

	/* Optional debuggery:


	BP_DBDisplaySelectInfoSelectWhere(select_clause_in_out, 1);

	printf("\n string_elements:        %s", string_elements);
	printf("\n front_part_table_name:  %s", front_part_table_name);
	printf("\n front_part_column_name: %s", front_part_column_name);
	printf("\n tail_part_table_name:   %s", tail_part_table_name);
	printf("\n tail_part_column_name:  %s", tail_part_column_name);
	printf("\n tail_part_raw_value:    %s", tail_part_raw_value);
	printf("\n separator_token:        %s", separator_token);
	printf("\n conjunction:            %s", select_clause_in_out->conjunction);
	printf("\n parse position:         %s", curr_parse_position);
	printf("\n");

	 */

	// destroy the trimmed element string before returning
	bpfree(trimmed_elem_string);

	// destroy all after
	BP_DestroyLinkL(tmp_tq, BP_TRUE);

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% JSON Query Interface %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// attempt to run a stack of queries from JSON
P_BP_JSON BP_DBRunQueriesFromJSON(P_BPDB bpdb, P_BP_JSON json_query)
{

	// ensure that we have bpdp
	if(!bpdb)
		return NULL;

	// basic null checks
	if(!json_query)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBRunQueriesFromJSON was called with a NULL json_query parameter.  This likely means your query JSON failed to parse correctly.");
		return NULL;
	}
	if(!json_query->json_out_hreg)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBRunQueriesFromJSON was called with a NULL json_query->json_out_hreg parameter.  This likely means your query JSON failed to parse correctly.");
		return NULL;
	}

	if(!json_query->json_out_hreg->hive)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBRunQueriesFromJSON was called with a NULL json_query->json_out_hreg->hive parameter.  Corruption detected.");
		return NULL;
	}
	if(!json_query->json_out_hreg->hive->data)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBRunQueriesFromJSON was called with a NULL json_query->json_out_hreg->hive->data parameter.  This likely means your query JSON failed to parse correctly.");
		return NULL;
	}



	// query result
	P_BP_JSON query_res = NULL;

	// current query pointer
	P_BP_JSON curr_query = NULL;

	// clone lookup path (used for cloning json for queries)
	char *tmp_clone_lookup_path = NULL;

	// iterate through queries
	BP_HASH_ITER_OPEN((P_BP_HTKE) json_query->json_out_hreg->hive, query_iter)
	{

		// if we have a select query, process it
		if(ht_memmatch_key_front_part(query_iter, "select_", bpstrlen("select_")) == ERR_SUCCESS)
		{

			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Process Select Query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// create lookup path
			tmp_clone_lookup_path = BP_HashRegCreateReasonableStringPathFromKeyedEntry(json_query->json_out_hreg, query_iter);

			// clone from path query
			curr_query = BP_JSONCloneFromPathQuery
			(
				json_query,
				tmp_clone_lookup_path,
				BP_TRUE,
				400,
				400
			);

			// run select query using cloned json input
			if(!BP_DBRunSelectQueryFromJSON_internal(bpdb, curr_query))
			{
				printf("\n [!!] Select query from JSON failed.");
				BP_JSONDestroy(curr_query);
				bpfree(tmp_clone_lookup_path);
				continue;
			}

			// ---- ADD RESULTS PATH AFTER QUERY RUN ----------

			// array as a path
			char ** select_query_path_array = BP_HashRegCreatePathFromKeyedEntry
			(
				json_query->json_out_hreg,
				query_iter
			);

			// create lookup query
			char * path_lookup_query[10];
			BP_ZERO_STACK(path_lookup_query);

			// path lookup query
			path_lookup_query[0] = query_iter->key;
			path_lookup_query[1] = "results";
			path_lookup_query[2] = "sqlgen_uuid";
			path_lookup_query[3] = NULL;

			// attempt to regenerate uuid
			P_BP_HTKE sqlgen_uuid_entry = BP_JSONLookupEntryByPath(curr_query, (char **) &path_lookup_query);

			// if we have results
			if(sqlgen_uuid_entry)
			{

				// remove the "sqlgen_uuid" entry in the query stack
				path_lookup_query[2] = NULL;

				// add table
				BP_JSONAddNodeByPath(json_query, (char **) &path_lookup_query, NULL, 0, BP_FALSE);

				// restore sqlgen paramter
				path_lookup_query[2] = "sqlgen_uuid";

				// add node
				BP_JSONAddNodeByPath
				(
					json_query,
					(char **) &path_lookup_query,
					(char *) sqlgen_uuid_entry->data,
					sqlgen_uuid_entry->data_size,
					BP_FALSE
				);

			}

			// destroy the path
			BP_DestroyPointerArrayNullTerminated((void **) select_query_path_array);
			bpfree(select_query_path_array);

			// cleanup
			BP_JSONDestroy(curr_query);
			ht_free(json_query->json_out_hreg, tmp_clone_lookup_path);

		}
		else
		if(ht_memmatch_key(query_iter, "update", bpstrlen("update")))
		{

			printf("\n Got update query?");

		}
		else
		if(ht_memmatch_key(query_iter, "insert", bpstrlen("insert")))
		{

			printf("\n Got insert query?");

		}
		else
		if(ht_memmatch_key(query_iter, "call", bpstrlen("call")))
		{

			printf("\n Got call query?");

		}
		else
		{
			printf("\n Got unknown or bad query?: %s", query_iter->key);
		}



		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Process Result If We Have One %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// destroy json on loop
		// BP_JSONDestroy(curr_query);
		// curr_query = NULL;

	}
	BP_HASH_ITER_CLOSE;

	return NULL;
}


// -- run various sub query types ----------------

// Run select query from provided json.  Since queries can come in stacked, you need to provide a query start position.
BP_ERROR_T BP_DBRunSelectQueryFromJSON_internal(P_BPDB bpdb, P_BP_JSON json_query)
{


	// run basic null checks
	if(!bpdb)
		return ERR_FAILURE;

	if(!json_query)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBRunSelectQueryFromJSON_internal was called with a null json_query");
		return ERR_FAILURE;
	}
	if(!json_query->json_out_hreg)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBRunSelectQueryFromJSON_internal was called with a null json_query->json_out_hreg");
		return ERR_FAILURE;
	}
	if(!json_query->json_out_hreg->hive)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBRunSelectQueryFromJSON_internal was called with a null json_query->json_out_hreg->hive");
		return ERR_FAILURE;
	}

	// if the top element is not a select statement, exit
	if(!ht_memmatch_key_front_part(json_query->json_out_hreg->hive, "select_", bpstrlen("select_")))
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBRunSelectQueryFromJSON_internal was called with a top level key which does not match a select key lookup (key select-prefix is something other than simply 'select_')");
		return ERR_FAILURE;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Step 1: Lookup Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// selecting path
	char * selecting_path[] =
	{
		json_query->json_out_hreg->hive->key,
		"selecting",
		NULL
	};

	// from_table path
	char * from_table_path[] =
	{
		json_query->json_out_hreg->hive->key,
		"from_table",
		NULL
	};

	// right join paths
	char * right_joins_path[] =
	{
		json_query->json_out_hreg->hive->key,
		"right_joins",
		NULL
	};

	// left join paths
	char * left_joins_path[] =
	{
		json_query->json_out_hreg->hive->key,
		"left_joins",
		NULL
	};

	//where clauses paths
	char * where_clauses_path[] =
	{
		json_query->json_out_hreg->hive->key,
		"where_clauses",
		NULL
	};

	// group by clause
	char * group_by_clauses_path[] =
	{
		json_query->json_out_hreg->hive->key,
		"group_by_clauses",
		NULL
	};

	// order clauses
	char * order_clauses_path[] =
	{
		json_query->json_out_hreg->hive->key,
		"order_clauses",
		NULL
	};

	// limit clauses
	char * limit_clauses_path[] =
	{
		json_query->json_out_hreg->hive->key,
		"limit_clauses",
		NULL
	};

	// run lookups
	P_BP_HTKE selectors        = BP_JSONLookupEntryByPath(json_query, (char **) &selecting_path);
	P_BP_HTKE from_table       = BP_JSONLookupEntryByPath(json_query, (char **) &from_table_path);
	P_BP_HTKE right_joins      = BP_JSONLookupEntryByPath(json_query, (char **) &right_joins_path);
	P_BP_HTKE left_joins       = BP_JSONLookupEntryByPath(json_query, (char **) &left_joins_path);
	P_BP_HTKE where_clauses    = BP_JSONLookupEntryByPath(json_query, (char **) &where_clauses_path);
	P_BP_HTKE group_by_clauses = BP_JSONLookupEntryByPath(json_query, (char **) &group_by_clauses_path);
	P_BP_HTKE order_clauses    = BP_JSONLookupEntryByPath(json_query, (char **) &order_clauses_path);
	P_BP_HTKE limit_clauses    = BP_JSONLookupEntryByPath(json_query, (char **) &limit_clauses_path);

	// only mandatory table is selectors
	if(!selectors)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBRunSelectQueryFromJSON_internal was unable to lookup the selectors table from within /select/selecting.  All select queries must contain selectors or they are invalid.");
		return ERR_FAILURE;
	}
	if(!from_table)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBRunSelectQueryFromJSON_internal was unable to lookup the from_table field.  All select queries  ");
		return ERR_FAILURE;
	}
	// open a syntax generator session (if successful, it will be added to the bpdb)
	P_BPDB_SQLGEN syngen = BP_DBSGOpen(bpdb);

	// initialize select info
	BP_DBSyntaxGenSelectInfoInit(syngen);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Step 2: Define Query Composite Success Designators %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// these get set when parsing selectors
	BP_BOOL contains_selectors                = BP_FALSE;
	BP_BOOL contains_only_valid_selectors     = BP_FALSE;
	size_t  selector_count                    = 0;

	// these get set when parsing right joins
	BP_BOOL contains_right_joins              = BP_FALSE;
	BP_BOOL contains_only_valid_right_joins   = BP_FALSE;
	size_t right_join_count                   = 0;

	// these get set when parsing left joins
	BP_BOOL contains_left_joins               = BP_FALSE;
	BP_BOOL contains_only_valid_left_joins    = BP_FALSE;
	size_t left_join_count                    = 0;

	// these get set after parsing left and right joins
	BP_BOOL contains_joins                    = BP_FALSE;
	BP_BOOL contains_only_valid_joins         = BP_FALSE;
	size_t join_count                         = 0;

	// these get set when parsing where clauses
	BP_BOOL contains_where_clauses            = BP_FALSE;
	BP_BOOL contains_only_valid_where_clauses = BP_FALSE;
	size_t  where_clause_count                = 0;


	// -- order by and limit have no count elements since they are always singular elements ---

	// set when parsing order clauses
	BP_BOOL contains_order_by_clause          = BP_FALSE;
	BP_BOOL contains_valid_order_by_clause    = BP_FALSE;

	// set when parsing limit clauses
	BP_BOOL contains_limit_clause             = BP_FALSE;
	BP_BOOL contains_valid_limit_clause       = BP_FALSE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Step 2: Process Selectors %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this will hold the array of selectors used for query lookup
	char ** selector_array = NULL;

	// add selectors
	if(selectors)
	if(selectors->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
	{

		// create new tailqueue for creating a char ** array
		P_BP_LINKL tmp_selector_add_tq = BP_CreateLinkL(".", BP_TRUE);

		// mark valid selectors as true before iteration
		contains_only_valid_selectors = BP_TRUE;

		// iterate through all selectors and add to tq
		BP_HASH_ITER_OPEN((P_BP_HTKE) selectors->data, selector)
		{

			if(BP_DBLookupTableColumnByJoinedFieldName(bpdb, (char *) selector->key ))
			{

				// add selector
				tq_strndup(tmp_selector_add_tq, (char *) selector->key, bpstrlen(selector->key), BPLN);

				// increment the selector count
				selector_count++;

			}
			else
			{

				// mark indicator since we've found a bad entry
				contains_only_valid_selectors = BP_FALSE;

			}

		}
		BP_HASH_ITER_CLOSE;


		// create the pointer array
		selector_array = (char **) BP_LinkLToBPCallocPointerArray(tmp_selector_add_tq);

		// we must have a filled out selector array at this point
		if(!selector_array)
		{
			BP_DestroyLinkL(tmp_selector_add_tq, BP_TRUE);
			BP_DBAddErrorMsg(bpdb, "BP_DBRunSelectQueryFromJSON_internal created a null selector_array (BP_LinkLToBPCallocPointerArray(tmp_selector_add_tq) failed).");
			return ERR_FAILURE;
		}

		// ensure there are elements in our array
		if(!BP_CountNullTerminatedPtrArray(selector_array))
		{
			bpfree(selector_array);
			BP_DestroyLinkL(tmp_selector_add_tq, BP_TRUE);
			BP_DBAddErrorMsg(bpdb, "BP_DBRunSelectQueryFromJSON_internal had an empty selector_array.");
			return ERR_FAILURE;
		}

		// destroy linkl after parsing logic
		BP_DestroyLinkL(tmp_selector_add_tq, BP_TRUE);

		// add field names (must be added at the top, do not move to the bottom)
		BP_DBSyntaxGenSelectInfoAddFieldNames(syngen, selector_array);

		// destroy the pointer array
		BP_DestroyPointerArrayNullTerminated((void **) selector_array);
		bpfree(selector_array);


	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% From Table %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// select from table
	char * select_from_table = NULL;
	if(from_table)
	{

		if(from_table->data)
		{

			// set from table entries
			P_BP_HTKE from_table_entries = (P_BP_HTKE) from_table->data;

			if(from_table_entries->key)
			{

				// ensure string is reasonable/printable
				if(BP_StringIsReasonablePrintableString(from_table_entries->key, bpstrlen(from_table_entries->key), BP_FALSE, BP_FALSE))
				{

					// set select from table
					select_from_table = from_table_entries->key;

				}

			}

		}

	}

	// set select from table
	if(!select_from_table)
	{

		// ensure we have a select_from entry
		BP_DestroyPointerArrayNullTerminated((void **) selector_array);
		bpfree(selector_array);
		BP_DBAddErrorMsg(bpdb, "BP_DBRunSelectQueryFromJSON_internal was unable to find a 'select from' table.");

		// return indicating failure
		return ERR_FAILURE;

	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Step 3: Process Joins (Right/Left) If They Exist %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// joins
	P_BPDB_SELECT_JOIN joins = NULL;

	// current join stack index count
	size_t join_idx = 0;


	// --- count right and left joins ---

	if(right_joins)
	if(right_joins->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
	{

		// set flags
		contains_right_joins = BP_TRUE;
		contains_joins       = BP_TRUE;

		// count right joins
		BP_HASH_ITER_OPEN((P_BP_HTKE) right_joins->data, right_join)
		{
			join_idx++;
		}
		BP_HASH_ITER_CLOSE;

	}

	if(left_joins)
	if(left_joins->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
	{

		// set flags
		contains_left_joins  = BP_TRUE;
		contains_joins       = BP_TRUE;

		// count left joins
		BP_HASH_ITER_OPEN((P_BP_HTKE) left_joins->data, left_join)
		{
			join_idx++;
		}
		BP_HASH_ITER_CLOSE;

	}

	// if we have joins, go ahead and allocate space for them now (counting and allocating
	// in this fashion prevents a lot of realloc() calls.
	if(join_idx)
	{

		// create join stack
		joins = (P_BPDB_SELECT_JOIN) ht_calloc(syngen->system_hreg, sizeof(BPDB_SELECT_JOIN) * (join_idx + 1), 1);
		if(!joins)
		{
			bpfree(selector_array);
			return ERR_FAILURE;
		}

		// reset join index while looping a second time (for assignment)
		join_idx = 0;

		// set contains joins marker
		contains_joins = BP_TRUE;

		if(right_joins)
		if(right_joins->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{


			// set right join marker to true
			contains_only_valid_right_joins = BP_TRUE;

			// count right joins
			BP_HASH_ITER_OPEN((P_BP_HTKE) right_joins->data, right_join)
			{

				if(BP_DBJSONSeparateRightJoinStringIntoParts(syngen, &joins[join_idx], right_join->key))
				{
					join_idx++;
					right_join_count++;
					join_count++;
				}
				else
				{
					contains_only_valid_right_joins = BP_FALSE;
				}

			}
			BP_HASH_ITER_CLOSE;

		}

		if(left_joins)
		if(left_joins->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{

			// set right join marker to true
			contains_only_valid_left_joins = BP_TRUE;

			// count left joins
			BP_HASH_ITER_OPEN((P_BP_HTKE) left_joins->data, left_join)
			{
				if(BP_DBJSONSeparateLeftJoinStringIntoParts(syngen, &joins[join_idx], left_join->key))
				{
					join_idx++;
					left_join_count++;
					join_count++;
				}
				else
				{
					contains_only_valid_left_joins = BP_FALSE;
				}
			}
			BP_HASH_ITER_CLOSE;

		}

	}

	// set market initially to true and then disqualify
	contains_only_valid_joins = BP_TRUE;

	// mark valid joins
	if(left_join_count && !contains_only_valid_left_joins)
		contains_only_valid_joins = BP_FALSE;
	if(right_join_count && !contains_only_valid_right_joins)
		contains_only_valid_joins = BP_FALSE;

	// set join count
	join_count = left_join_count + right_join_count;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Step 3: Process Where Clauses Joins If They Exist %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// wheres
	P_BPDB_SELECT_WHERE wheres = NULL;

	// current wheres stack index count
	size_t wheres_idx = 0;


	// --- count where clauses ---

	if(where_clauses)
	{

		if(where_clauses->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{

			// count right joins
			BP_HASH_ITER_OPEN((P_BP_HTKE) where_clauses->data, where_clause)
			{
				wheres_idx++;
			}
			BP_HASH_ITER_CLOSE;

		}

	}

	// if we have joins, go ahead and allocate space for them now (counting and allocating
	// in this fashion prevents a lot of realloc() calls.
	if(wheres_idx)
	{

		// set marker for where clauses
		contains_where_clauses = BP_TRUE;

		// create where-clause stack
		wheres = (P_BPDB_SELECT_WHERE) ht_calloc(syngen->system_hreg, sizeof(BPDB_SELECT_WHERE) * (wheres_idx + 1), 1);
		if(!wheres)
		{
			BP_DestroyPointerArrayNullTerminated((void **) selector_array);
			bpfree(selector_array);
			return ERR_FAILURE;
		}

		// set where clause index (used for iterating)
		wheres_idx = 0;

		// set clause marker before iterating
		contains_only_valid_where_clauses = BP_TRUE;

		// if we have where clauses to process, go ahead and process them
		if(where_clauses)
		{
			if(where_clauses->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
			{

				// count right joins
				BP_HASH_ITER_OPEN((P_BP_HTKE) where_clauses->data, where_clause)
				{


					// Note: This is a fairly convoluted routine used to figure out SQL logic.
					if(BP_DBJSONSeparateWhereClauseStringIntoParts(syngen, &wheres[wheres_idx], (char *) where_clause->data) == ERR_SUCCESS)
					{
						wheres_idx++;
						where_clause_count++;
					}
					else
					{

						// create some stack space for error reporting
						char tmp_str[512+bpstrlen((char *) where_clause->data)];
						BP_ZERO_STACK(tmp_str);
						sprintf((char *) &tmp_str, "BP_DBRunSelectQueryFromJSON_internal reports that the following where clause is invalid '%s'", where_clause->data);
						BP_DBAddErrorMsg(bpdb, (char *) &tmp_str);

						// mark it as having potentially invalid where clauses
						contains_only_valid_where_clauses = BP_FALSE;

					}

				}
				BP_HASH_ITER_CLOSE;

			}
			else
			{
				printf("\n JOE ROGAN PODCAST WUT?22");
				BP_DBAddErrorMsg(bpdb, "BP_DBRunSelectQueryFromJSON_internal where_clauses had a non-table type.");
			}
		}
		else
		{
			BP_DBAddErrorMsg(bpdb, "BP_DBRunSelectQueryFromJSON_internal appears to have a non_zero where_idx but a null set of where clauses.");

		}


	}
	else
	{
		printf("\n NO WHERE CLAUSES FOUND");
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Step X: Process Group By Clauses If They Exist %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// create and zero the group_by clause
	BPDB_SELECT_GROUP_BY group_by;
	BP_ZERO_STACK(group_by);

	if(group_by_clauses)
	if(group_by_clauses->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
	{

		// this is defined as an iter, but really all we're looking for is one
		// entry.  It's done like this in case we modify the code in the future
		// to include group_by option fields.
		BP_HASH_ITER_OPEN((P_BP_HTKE) group_by_clauses->data, group_by_clause)
		{

			// set the group by field
			group_by.field = ht_strdup(syngen->system_hreg, group_by_clause->key);

			// only ever take one entry
			break;

		}
		BP_HASH_ITER_CLOSE;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Step 3: Process Order Clauses If They Exist %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// order by table and direction
	char *order_by_table     = NULL;
	char *order_by_direction = NULL;

	// ensures that the order is reasonable
	size_t order_iter_n = 0;

	// create select order query
	BPDB_SELECT_ORDER_BY order_by;
    BP_ZERO_STACK(order_by);


	// process order clauses
	if(order_clauses)
	if(order_clauses->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
	{

		// iterate through table
		BP_HASH_ITER_OPEN((P_BP_HTKE) order_clauses->data, order_clause)
		{

			if(ht_memmatch_key(order_clause, "index", bpstrlen("index")))
			{
				if(BP_DBLookupTableColumnByJoinedFieldName(bpdb, (char *) order_clause->data))
				{
					order_by_table = ht_strdup(syngen->system_hreg, (char *) order_clause->data);
				}
			}
			else if(ht_memmatch_key(order_clause, "order", bpstrlen("order")))
			{
				if(order_clause->data)
				{
					if(ht_memmatch_entry(order_clause, "ASC", bpstrlen("ASC")+1))
					{

						order_by_direction = ht_strdup(syngen->system_hreg, (char *) order_clause->data);

					} else if(ht_memmatch_entry(order_clause, "DESC", bpstrlen("DESC")+1))
					{

						order_by_direction = ht_strdup(syngen->system_hreg, (char *) order_clause->data);

					}
				}
			}
		}
		BP_HASH_ITER_CLOSE;

	}

	// if we passed all the checks, set order by direction here
	if(order_by_table && order_by_direction)
	{

		// mark as containing valid order by clauses
		contains_valid_order_by_clause = BP_TRUE;

		// set field and sort designator
		order_by.field           = order_by_table;
		order_by.sort_designator = order_by_direction;

	}

	printf("\n Got Order Index Clause: %s", (char *) order_by_table);
	printf("\n Got Order Direction:    %s", (char *) order_by_direction);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Step 3: Process Limit Clauses If They Exist %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	printf("\n [+] Limit Clauses: %p", limit_clauses);

	// limit top and bottom (set if limits are available)
	char *limit_top    = NULL;
	char *limit_bottom = NULL;

	// create limit struct
	BPDB_SELECT_LIMITS limits;
	BP_ZERO_STACK(limits);

	// check for limit clauses
	if(limit_clauses)
	if(limit_clauses->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
	{

		// set validation marker before loop
		contains_valid_limit_clause = BP_TRUE;

		// iterate through table
		BP_HASH_ITER_OPEN((P_BP_HTKE) limit_clauses->data, limit_clause)
		{

			if(ht_memmatch_key(limit_clause, "top", bpstrlen("top")))
			{

				if(BP_StringIsReasonableForUnsignedIntegerASCII((char *) limit_clause->data, bpstrlen((char *) limit_clause->data)))
				{
					BP_StringToUnsignedLongBase10((char *) limit_clause->data, bpstrlen((char *) limit_clause->data), &limits.limit_high);
				}
				else
				{
					// create some stack space for error reporting
					char tmp_str[512];
					BP_ZERO_STACK(tmp_str);
					sprintf((char *) &tmp_str, "BP_DBRunSelectQueryFromJSON_internal reports that limit_top is not an integer value.");
					BP_DBAddErrorMsg(bpdb, (char *) &tmp_str);

					contains_valid_limit_clause = BP_FALSE;
				}

			}
			else if(ht_memmatch_key(limit_clause, "bottom", bpstrlen("bottom")))
			{

				if(BP_StringIsReasonableForUnsignedIntegerASCII((char *) limit_clause->data, bpstrlen((char *) limit_clause->data)))
				{
					BP_StringToUnsignedLongBase10((char *) limit_clause->data, bpstrlen((char *) limit_clause->data), &limits.limit_low);
				}
				else
				{
					// create some stack space for error reporting
					char tmp_str[512];
					BP_ZERO_STACK(tmp_str);
					sprintf((char *) &tmp_str, "BP_DBRunSelectQueryFromJSON_internal reports that limit_bottom is not an integer value.");
					BP_DBAddErrorMsg(bpdb, (char *) &tmp_str);
					contains_valid_limit_clause = BP_FALSE;
				}

			}

		}
		BP_HASH_ITER_CLOSE;


	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check Markers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set marker to true before checks
	BP_BOOL query_appears_to_be_ok = BP_TRUE;

	// run checks
	if(contains_selectors && !contains_only_valid_selectors)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBRunSelectQueryFromJSON_internal was called with invalid selectors.");
		query_appears_to_be_ok = BP_FALSE;
	}
	else if(contains_right_joins && !contains_only_valid_right_joins)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBRunSelectQueryFromJSON_internal was called with invalid right joins.");
		query_appears_to_be_ok = BP_FALSE;
	}
	else if(contains_left_joins && !contains_only_valid_left_joins)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBRunSelectQueryFromJSON_internal was called with invalid left joins.");
		query_appears_to_be_ok = BP_FALSE;
	}
	else if(contains_joins && !contains_only_valid_joins)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBRunSelectQueryFromJSON_internal was called with invalid joins (either type).");
		query_appears_to_be_ok = BP_FALSE;
	}
	else if(contains_where_clauses && !contains_only_valid_where_clauses)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBRunSelectQueryFromJSON_internal was called with invalid where clause(s).");
		query_appears_to_be_ok = BP_FALSE;
	}
	else if(contains_order_by_clause && !contains_valid_order_by_clause)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBRunSelectQueryFromJSON_internal was called with invalid order-by clause(s).");
		query_appears_to_be_ok = BP_FALSE;
	}
	else if(contains_limit_clause && !contains_valid_limit_clause)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBRunSelectQueryFromJSON_internal was called with invalid limit clause(s).");
		query_appears_to_be_ok = BP_FALSE;
	}

	// ensure the query appears ok
	if(!query_appears_to_be_ok)
	{
		printf("\n QUERY APPEARS TO HAVE FAILED PREP STEPS?");
		BP_DBAddErrorMsg(bpdb, "BP_DBRunSelectQueryFromJSON_internal reports query has failed pre-check value analysis and the query cannot be run.");
		return ERR_FAILURE;
	}
	else
	{
		printf("\n QUERY PASSED STEPS OK");
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Setup and Run Query Now That All Checks Have Passed %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure the generator can be opened successfully
	if(!syngen)
	{
		BP_DBAddErrorMsg(bpdb, "BP_DBRunSelectQueryFromJSON_internal called BP_DBSGOpen, but no syntax-generator could be created.");
		return ERR_FAILURE;
	}

	/*
	// select information
	BPDB_SQLGEN_SELECT select_info;
	BP_ZERO_STACK     (select_info);


	// set field names
	select_info.field_names = selector_array;

	// NOTE: this will need to be changed
	select_info.table_name  = "results";

	// set where clauses
	select_info.select_where         = wheres;
	select_info.select_where_entry_n = where_clause_count;

	// set joins
	select_info.join_on         = joins;
	select_info.join_on_entry_n = join_count;

	// set orderby and limits from local stack elements
	select_info.order_by = &order_by;
	select_info.limits   = &limits;
	*/

	// set field names (do not uncomment, field names have already been added)
	// BP_DBSyntaxGenSelectInfoAddFieldNames  (syngen, selector_array);

	// set primary select_from table
	BP_DBSyntaxGenSelectInfoSetPrimaryTable(syngen, select_from_table);

	// iterate through where cluses and add
	size_t where_clause_iter = 0;
	for
	(
		;
		where_clause_iter < where_clause_count;
		where_clause_iter++
	)
	{

		// set the where clause
		P_BPDB_SELECT_WHERE curr_where = &wheres[where_clause_iter];

		// add where clause
		BP_DBSyntaxGenSelectInfoAddWhereClause
		(
			syngen,
			curr_where->value_type,
			curr_where->conjunction,
			curr_where->field_name,
			curr_where->limiter,
			curr_where->value,
			curr_where->value_len
		);

	}

	// process joins
	if(joins)
	{
		// iterate through joins
		size_t join_count_iter = 0;
		for(; join_count_iter < join_count; join_count_iter++)
		{

			// current join
			P_BPDB_SELECT_JOIN curr_join = &joins[join_count_iter];

			// add join part
			BP_DBSyntaxGenSelectInfoAddJoinPart
			(
				syngen,
				curr_join->join_type,
				curr_join->table_name,
				curr_join->front_join,
				curr_join->comparison_operator,
				curr_join->tail_join
			);

		}

	}

	// retrieve the group by clause
	BP_DBSyntaxGenSelectInfoAddGroupByClause(syngen, group_by.field);

	// limit clause
	BP_DBSyntaxGenSelectInfoSetLimitClause
	(
		syngen,
		limits.limit_low,
		limits.limit_high
	);

	// add sort designator
	BP_DBSyntaxGenSelectInfoSetOrderByClause
	(
		syngen,
		order_by.field,
		order_by.sort_designator
	);


	// return here 11/21/2016 - HERE HERE
	// return here 11/21/2016 - HERE HERE
	// return here 11/21/2016 - HERE HERE
	//
	// We just added BP_DBSyntaxGenSelectInfoAddFieldNames.  We need to add the relevant
	// "add" routines in place within all of the existing elements below.
	//
	// The most straight-forward solution might be to simply go to the end of the function and add elements as they're
	// already parsed (legacy code). Aka just iterate through them and call the BP_DBSyntaxGenSelectInfoAdd*routines.
	//
	// return here 11/21/2016 - HERE HERE
	// return here 11/21/2016 - HERE HERE
	// return here 11/21/2016 - HERE HERE



	// attempt select from generation
	if(!BP_DBSGSelectFrom(syngen, NULL))
	{

		// add error
		BP_DBAddErrorMsg(bpdb, "BP_DBRunSelectQueryFromJSON_internal called BP_DBSGSelectFrom, but the call failed.");
		return ERR_FAILURE;

	}

	// display select info
	printf("\n\n SELECT INFO HERE HERE");
	BP_DBDisplaySelectInfo(syngen->select_info);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Syngen To Results Registry By UUID %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create some space on the stack for our uuid buffer
	char uuid_buff[BP_UUID_LEN+4];
	BP_ZERO_STACK(uuid_buff);

	// attempt to generate random uuid
	BP_GenRandomUUID_StoreInBuff((char *) &uuid_buff);
	if(bpstrlen(uuid_buff) == 0)
	{
		return ERR_FAILURE;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Syntax Generator Reference to Json %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// results
	char * results_path[] =
	{
		json_query->json_out_hreg->hive->key,
		"results",
		NULL
	};

	// sqlgen path
	char * sqlgen_path[] =
	{
		json_query->json_out_hreg->hive->key,
		"results",
		"sqlgen_uuid",
		NULL
	};

	// add results
	BP_JSONAddNodeByPath(json_query, (char **) &results_path, NULL, 0, BP_FALSE);
	BP_JSONAddNodeByPath(json_query, (char **) &sqlgen_path,  syngen->uuid, bpstrlen(syngen->uuid), BP_FALSE);


	// return indicating success
	return ERR_SUCCESS;

}

// Run an insert query from provided json.  Since queries can come in stacked, you need to provide a query start position.
BP_ERROR_T BP_DBRunInsertQueryFromJSON_internal(P_BPDB bpdb, P_BP_JSON json_query)
{
	return ERR_FAILURE;
}

// Run an update query from provided json.  Since queries can come in stacked, you need to provide a query start position.
BP_ERROR_T BP_DBRunUpdateQueryFromJSON_internal(P_BPDB bpdb, P_BP_JSON json_query)
{
	return ERR_FAILURE;
}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Fetch Results Into JSON %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to fetch next result from JSON provided a lookup path query (eg. /select/results)
BP_ERROR_T BP_DBFetchNextResultIntoJSON
(
	P_BPDB    bpdb,
	char *    results_path_lookup,
	P_BP_JSON completed_json_query,
	BP_BOOL   destroy_results_before_stacking_result
)
{


	// run basic sanity checks
	if(!bpdb)
		return ERR_FAILURE;
	if(!bpdb->sqlgen_reg)
		return ERR_FAILURE;
	if(!bpdb->sqlgen_reg->hive)
		return ERR_FAILURE;
	if(!bpdb->sqlgen_reg->total_data_entries_in_hive)
		return ERR_FAILURE;
	if(!results_path_lookup)
		return ERR_FAILURE;
	if(!completed_json_query)
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(results_path_lookup, bpstrlen(results_path_lookup), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// clone from path query
	P_BP_JSON curr_query = completed_json_query;

	// fail if we have no current query
	if(!curr_query)
	{
		return ERR_FAILURE;
	}

	// convert string lookup to path array
	char ** results_path = BP_HashRegStringLookupToPathArray(results_path_lookup, bpstrlen(results_path_lookup));
	if(!results_path)
		return ERR_FAILURE;

	// ensure we have path elements
	if(!results_path[0])
	{
		BP_DestroyPointerArrayNullTerminated((void **) results_path);
		bpfree(results_path);
		return ERR_FAILURE;
	}
	if(!results_path[1])
	{
		BP_DestroyPointerArrayNullTerminated((void **) results_path);
		bpfree(results_path);
		return ERR_FAILURE;
	}

	// set path lookup
	char *path_lookup[] =
	{
		results_path[0],
		"results",
		"sqlgen_uuid",
		NULL
	};

	// attempt to lookup the sqlgen uuid for this query
	P_BP_HTKE uuid_lookup = BP_JSONLookupEntryByPath(curr_query, (char **) &path_lookup);

	// exit if we have no uuid
	if(!uuid_lookup)
	{
		BP_DestroyPointerArrayNullTerminated((void **) results_path);
		bpfree(results_path);
		return ERR_FAILURE;
	}

	// now attempt to lookup sqlgen by uuid
	P_BPDB_SQLGEN sqlgen_lookup = BP_DBLookupSQLGeneratorByUUID(bpdb, (char *) uuid_lookup->data);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Fetch Records %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// fetch result
	if(!BP_DBSGFetchResult(sqlgen_lookup, sqlgen_lookup->select_info, BP_TRUE))
	{
		BP_DestroyPointerArrayNullTerminated((void **) results_path);
		bpfree(results_path);
		return ERR_FAILURE;
	}

	// display the select info
	// BP_DBDisplaySelectInfo(sqlgen_lookup->select_info);

	// generate random uuid index
	char * tmp_idx = BP_GenRandomUUID();

	// iterate through the bind results
	size_t bind_result_iter = 0;
	for(; bind_result_iter < sqlgen_lookup->select_info->bind_results_n; bind_result_iter++)
	{

		// set current result from result stack
		MYSQL_BIND * curr_result = &sqlgen_lookup->select_info->bind_results[bind_result_iter];

		// create conversion buff
		char numeric_conversion_buff[256];
		BP_ZERO_STACK(numeric_conversion_buff);

		// create timestamp conversion buff
		char timestamp_conversion_buff[512];
		BP_ZERO_STACK(timestamp_conversion_buff);

		// set temporary long pointer
		long * tmp_long_ptr = NULL;

		// set path buffer
		char *tmp_path[20];
		BP_ZERO_STACK(tmp_path);

		// set the query add path
		tmp_path[0] = results_path[0];
		tmp_path[1] = "results";
		tmp_path[2] = tmp_idx;
		tmp_path[3] = sqlgen_lookup->select_info->field_names[bind_result_iter];
		tmp_path[4] = NULL;

		// set temporary time
		MYSQL_TIME * tmp_time = NULL;

		// temporary hex buffers for blob types
		char *tmp_hex_blob_buff = NULL;
		char *tmp_hex_blob_buff_final = NULL;

		// gather supported data types
		switch(curr_result->buffer_type)
		{


			// unprocessed types
			case MYSQL_TYPE_DECIMAL:
				break;
			case MYSQL_TYPE_TINY:
				break;
			case MYSQL_TYPE_NEWDECIMAL:
				break;
			case MYSQL_TYPE_ENUM:
				break;
			case MYSQL_TYPE_SET:
				break;
			case MYSQL_TYPE_BIT:
				break;


			// numeric types
			case MYSQL_TYPE_SHORT:
				sprintf((char *) &numeric_conversion_buff, "%hi", curr_result->buffer);
				BP_JSONAddNodeByPath(curr_query, tmp_path, numeric_conversion_buff, bpstrlen(numeric_conversion_buff), BP_FALSE);
				break;

			case MYSQL_TYPE_INT24:
			case MYSQL_TYPE_LONG:
				BP_ZERO_STACK(numeric_conversion_buff);
				tmp_long_ptr = (long *) curr_result->buffer;
				sprintf((char *) &numeric_conversion_buff, "%i", (long) *tmp_long_ptr);
				BP_JSONAddNodeByPath(curr_query, tmp_path, numeric_conversion_buff, bpstrlen(numeric_conversion_buff), BP_FALSE);
				break;

			case MYSQL_TYPE_LONGLONG:
				sprintf((char *) &numeric_conversion_buff, "%lli", curr_result->buffer);
				BP_JSONAddNodeByPath(curr_query, tmp_path, numeric_conversion_buff, bpstrlen(numeric_conversion_buff), BP_FALSE);
				break;

			case MYSQL_TYPE_FLOAT:
				sprintf((char *) &numeric_conversion_buff, "%f", curr_result->buffer);
				BP_JSONAddNodeByPath(curr_query, tmp_path, numeric_conversion_buff, bpstrlen(numeric_conversion_buff), BP_FALSE);
				break;

			case MYSQL_TYPE_DOUBLE:
				sprintf((char *) &numeric_conversion_buff, "%f", curr_result->buffer);
				BP_JSONAddNodeByPath(curr_query, tmp_path, numeric_conversion_buff, bpstrlen(numeric_conversion_buff), BP_FALSE);
				break;

			case MYSQL_TYPE_NULL:
				BP_JSONAddNodeByPath(curr_query, tmp_path, (char *) "NULL", bpstrlen("NULL"), BP_FALSE);
				break;


			// process timestamps
			case MYSQL_TYPE_DATETIME:
			case MYSQL_TYPE_YEAR:
			case MYSQL_TYPE_NEWDATE:
			case MYSQL_TYPE_DATE:
			case MYSQL_TYPE_TIME:
			case MYSQL_TYPE_TIMESTAMP:

				// CURRENT_TIMESTAMP() EXAMPLE: "2016-11-21 17:33:15"

				// set timestamp
				tmp_time = (MYSQL_TIME *) curr_result->buffer;

				// create timestamp
				sprintf
				(
					(char *) &timestamp_conversion_buff,
					"%.02u-%.02u-%.02u %.02u:%.02u:%.02u",
					tmp_time->year,
					tmp_time->month,
					tmp_time->day,
					tmp_time->hour,
					tmp_time->minute,
					tmp_time->second
				);
				BP_JSONAddNodeByPath(curr_query, tmp_path, (char *) timestamp_conversion_buff, bpstrlen(timestamp_conversion_buff), BP_FALSE);
				break;





			// process blob fields (returned as hexidecimal)
			case MYSQL_TYPE_TINY_BLOB:
			case MYSQL_TYPE_MEDIUM_BLOB:
			case MYSQL_TYPE_LONG_BLOB:
			case MYSQL_TYPE_BLOB:

				// create hex blob
				tmp_hex_blob_buff = BP_BufferToMySQLHexString((char *) curr_result->buffer, curr_result->buffer_length, NULL);
				if(tmp_hex_blob_buff)
				{

					// add node by path
					BP_JSONAddNodeByPath(curr_query, tmp_path, (char *) tmp_hex_blob_buff_final, bpstrlen(tmp_hex_blob_buff_final), BP_FALSE);

					// set temp hex blob
					if(tmp_hex_blob_buff)
						bpfree(tmp_hex_blob_buff);

					// set buff
					tmp_hex_blob_buff = NULL;

				}
				break;

			// unhandled type
			case MYSQL_TYPE_GEOMETRY:
				break;


			// process string types
			case MYSQL_TYPE_VARCHAR:
			case MYSQL_TYPE_VAR_STRING:
			case MYSQL_TYPE_STRING:
				BP_JSONAddNodeByPath(curr_query, tmp_path, (char *) curr_result->buffer, curr_result->buffer_length, BP_FALSE);
				break;

			// default (do nothing)
			default:
				break;

		}

	}

	// destroy results path
	if(results_path)
	{
		BP_DestroyPointerArrayNullTerminated((void **) results_path);
		bpfree(results_path);
	}

	// destroy the temporarily retrieved index
	bpfree(tmp_idx);

	// return indicating success
	return ERR_SUCCESS;

}

// attempt to fetch all results
BP_ERROR_T BP_DBFetchAllResultsIntoJSON
(
	P_BPDB bpdb,
	char *    results_path_lookup,
	P_BP_JSON completed_json_query
)
{

	// attempt to fetch next result from JSON provided a lookup path query (eg. /select/results)
	while
	(
		BP_DBFetchNextResultIntoJSON
		(
			bpdb,
			results_path_lookup,
			completed_json_query,
			BP_FALSE
		) == ERR_SUCCESS
	){}

	// return indicating success
	return ERR_SUCCESS;

}


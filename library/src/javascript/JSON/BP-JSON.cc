/*
 * BP-JSON.cc
 *
 *  Created on: Dec 7, 2015
 *      Author: root
 */

// include blockparty main
#include "../../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% JSON Create / Destroy %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Create new json object
P_BP_JSON BP_JSONCreate()
{

	// create new system hreg
	P_BP_HASH_TABLE_REGISTRY system_hreg = BP_CreateHashTableRegistry();
	if(!system_hreg)
		return NULL;

	// allocate new buffer
	P_BP_JSON json = (P_BP_JSON) ht_calloc(system_hreg, sizeof(BP_JSON), 1);

	// set system hash table registry
	json->system_hreg = system_hreg;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Initial TQ Prep %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create a new thread safe linkl for prepare
	if(!json->json_out_prepare_tq)
		json->json_out_prepare_tq = BP_CreateLinkL(".", BP_TRUE);

	// dupe in the opening parentheis to start (json is built this way)
	tq_strdup(json->json_out_prepare_tq, "{", BPLN);

	// return the new json (or null if alloc fails)
	return json;

}

// Create new json object from a parsable buffer.  Must check parse state for parse information after calling.  Returns
// a json handle whether parse fails or not.
P_BP_JSON BP_JSONCreateFromBuffer(char *buffer, size_t buff_len, size_t max_depth_for_token_allocation, size_t max_tokens_total)
{

	// allocate new buffer
	P_BP_JSON json = (P_BP_JSON) BP_JSONCreate();
	if(!json)
		return NULL;

	// attempt to parse from buffer
	BP_JSONParseFromBuffer(json, buffer, buff_len, max_depth_for_token_allocation, max_tokens_total);

	// return the new json whether parse fails or not (need to check parse info to determine if parse was ok)
	return json;

}


// Attempts to create JSON from a file.  Reads entire file and attempts to parse into JSON.
P_BP_JSON BP_JSONCreateFromFile(char *filename, size_t max_depth_for_token_allocation, size_t max_tokens_total)
{

	// ensure we have a filename
	if(!filename)
		return NULL;

	// create extra info
	BP_FILE_LOAD_EXTRA_INFO extra_info;
	BP_ZERO_STACK(extra_info);

	// retrieve file contents
	char *buffer = BP_FileGetContents(filename, &extra_info);
	if(!buffer)
	{
		return NULL;
	}


	// allocate new buffer
	P_BP_JSON json = (P_BP_JSON) BP_JSONCreate();
	if(!json)
	{
		bpfree(buffer);
		return NULL;
	}

	// attempt to parse from buffer
	if(!BP_JSONParseFromBuffer(json, buffer, extra_info.file_size, max_depth_for_token_allocation, max_tokens_total))
	{
		bpfree(buffer);
		BP_JSONDestroy(json);
		return NULL;
	}

	// destroy the parse buffer before returning
	bpfree(buffer);

	// return the new json structure if parsed OK!
	return json;

}



// attempt to destroy a json object
BP_ERROR_T BP_JSONDestroy(P_BP_JSON json)
{

	if(!json)
		return ERR_FAILURE;
	if(!json->system_hreg)
		return ERR_FAILURE;

	// destroy parse state if it exists
	if(json->parse_state)
	{
		BP_JSONDestroyParseState(json->parse_state);
		json->parse_state = NULL;
	}

	// destroy json in registry if it exists
	if(json->json_in_hreg)
	{
		BP_DestroyHashTableRegistry(json->json_in_hreg);
		json->json_in_hreg = NULL;
	}

	// destroy json out registry if it exists
	if(json->json_out_hreg)
	{
		BP_DestroyHashTableRegistry(json->json_out_hreg);
		json->json_out_hreg = NULL;
	}

	// destroy the prepare linkl if it's set
	if(json->json_out_prepare_tq)
	{
		BP_DestroyLinkL(json->json_out_prepare_tq, BP_TRUE);
		json->json_out_prepare_tq = NULL;
	}

	P_BP_HASH_TABLE_REGISTRY system_hreg = json->system_hreg;

	// destroy the reg
	// ht_free(system_hreg, json);


	// destroy system registry (destroys all other allocations including main structure)
	BP_DestroyHashTableRegistry(system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}


// destroys output buffers (dangling data)
BP_ERROR_T BP_JSONDestroyOutputBuffers(P_BP_JSON json)
{

	if(!json)
		return ERR_FAILURE;
	if(!json->json_out)
		return ERR_FAILURE;

	// destroy the data
	ht_free(json->system_hreg, json->json_out);
	json->json_out = NULL;
	json->json_out_len = 0;

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Add Nodes %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// add node by path
BP_ERROR_T BP_JSONAddNodeByPath
(
		P_BP_JSON json,
		char    **path,
		char    * data,
		size_t    data_len,
		BP_BOOL   base64_encode_data
)
{

	// path is required
	if(!path)
		return ERR_FAILURE;

	// DO NOT: ensure we have data, do not uncomment.  Null entries are used to create tables.
	// if(data)
	// if(!data_len)
	//	  return ERR_FAILURE;

	// ensure we have an output hash registry
	if(!json->json_out_hreg)
	{

		// if there is no output registry, create it
		json->json_out_hreg = BP_CreateHashTableRegistry();

	}

	// get the number of items
	size_t path_idx_n = BP_CountNullTerminatedPtrArray(path);

	// create paths
	char * paths[path_idx_n];
	BP_ZERO_STACK(paths);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% First Attempt to Lookup Chunk (verify it doesn't exist) %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = BP_HashRegLookupTableEntryByPath(json->json_out_hreg, path);
	if(tmp_lookup)
	{
		printf("\n [+] Attempting to add JSON entry by path, but an entry with that path already exists. JSON path listed below.");
		printf("\n");
		BP_DisplayStringArray("-", 1, 1, path);
		return ERR_FAILURE;
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Now Add Entry %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// path index iter
	size_t path_idx_iter = 0;
	for
	(
		;
		path_idx_iter < path_idx_n;
		path_idx_iter++
	)
	{

		// if we're at the last element we add a new data element and exit
		if(!path[path_idx_iter+1])
		{

			// if we don't have data, add a table, if we do have data, add data
			if(!data)
			{

				BP_HashRegAddTableByPath(json->json_out_hreg, path);
				break;

			}
			else
			{

				// set write length; depends on whether or not we base64
				size_t write_len = 0;

				// this will contain element data after it's either encoded or decoded
				char * element_data = NULL;
				size_t element_size = 0;


				// encode the data if set
				if(base64_encode_data)
				{

					// attempt to encode passed in data
					element_data = BP_Base64Encode((unsigned char *) data, data_len);
					if(!element_data)
						return ERR_FAILURE;

					// set element size
					element_size = bpstrlen(element_data)+1;

				}
				else
				{
					// attempt to encode passed in data
					element_data = data;
					if(!element_data)
						return ERR_FAILURE;

					// set element size
					element_size = data_len;

				}

				// ensure we have element data
				if(!element_data)
					return ERR_FAILURE;

				// add data to tree
				BP_ERROR_T add_status = BP_HashRegAddDataToTableByPath
				(
						json->json_out_hreg,
						paths,
						path[path_idx_iter],
						BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
						NULL,
						element_size+1
				);

				// if we failed to add, exit
				if(add_status != ERR_SUCCESS)
					return ERR_FAILURE;


				// set lookup path on stack
				char * lookup_path[path_idx_n+2];
				BP_ZERO_STACK(lookup_path);

				// copy in paths directly
				memcpy((char **) &lookup_path, paths, sizeof(paths));

				// set the lookup path
				lookup_path[path_idx_iter] = path[path_idx_iter];

				// set the entry lookup
				P_BP_HASH_TABLE_KEYED_ENTRY entry_lookup = BP_HashRegLookupTableEntryByPath(json->json_out_hreg, (char **) &lookup_path);
				if(entry_lookup)
				{

					// copy in the base64 data
					memcpy(entry_lookup->data, element_data, bpstrlen(element_data));

					// mark data as base64 since we are using base64 data
					if(base64_encode_data)
						entry_lookup->is_base64 = BP_TRUE;

					// destroy element data if it was base64
					if(element_data != data)
					{
						bpfree(element_data);
					}

					// return indicating success
					return ERR_SUCCESS;

				}

				// return indicating failure
				return ERR_FAILURE;

			}

		}

		// set paths
		paths[path_idx_iter] = path[path_idx_iter];



		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Attempt Lookup as Table %%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// attempt to lookup index by path
		if(!BP_HashRegLookupTableByPath(json->json_out_hreg, (char **) &paths))
		{

			// printf("\n Adding table?!: %s", paths[path_idx_iter]);
			BP_HashRegAddTableByPath(json->json_out_hreg, (char **) &paths);

		}

	}

	// BP_HashRegDisplay(json->json_out_hreg, BP_FALSE, BP_TRUE);

	// return indicating success
	return ERR_SUCCESS;

}


// Add node by path (path "query" is in the form: /index1/whatever_index2/something_three/target data.
BP_ERROR_T BP_JSONAddNodeByPathQuery
(
	P_BP_JSON json,
	char    * lookup_query,
	char    * data,
	size_t    data_len,
	BP_BOOL   base64_encode_data
)
{

	// run basic null checks
	if(!json)
		return ERR_FAILURE;
	if(!lookup_query)
		return ERR_FAILURE;

	// ensure lookup query is reasonable/printable string
	if(!BP_StringIsReasonablePrintableString(lookup_query, bpstrlen(lookup_query), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// create path array from lookup query
	char ** path_lookup_array = BP_HashRegStringLookupToPathArray(lookup_query, bpstrlen(lookup_query));

	// optional debug message
	// printf("\n [dbg] path_lookup_array and first element ptr: %p - %p", path_lookup_array, path_lookup_array[0]);

	// check to ensure we've created a lookup array
	if(!path_lookup_array)
		return ERR_FAILURE;



	// ensure we have a first element
	if(!path_lookup_array[0])
	{
		bpfree(path_lookup_array);
		return ERR_FAILURE;
	}

	// now that path is calculated, add it if possible
	BP_ERROR_T add_ret_code = BP_JSONAddNodeByPath
	(
		json,
		path_lookup_array,
		data,
		data_len,
		base64_encode_data
	);

	// destroy the pointer array
	BP_DestroyPointerArrayNullTerminated((void **) path_lookup_array);

	// free the pointer itself
	bpfree(path_lookup_array);

	// return indicating success or failure
	return add_ret_code;

}

// Add table by path.
BP_ERROR_T BP_JSONAddTableByPath
(
		P_BP_JSON json,
		char    **path,
		BP_BOOL   table_is_anonymous
)
{

	// basic null checks
	if(!json)
		return ERR_FAILURE;
	if(!path)
		return ERR_FAILURE;

	// return the path addition result
	return BP_JSONAddNodeByPath(json, path, NULL, 0, BP_FALSE);


}

// Add table by path query. (frontend for BP_JSONAddNodeByPathQuery with null data parameters)
BP_ERROR_T BP_JSONAddTableByPathQuery
(
		P_BP_JSON json,
		char    * lookup_query,
		BP_BOOL   table_is_anonymous
)
{

	// basic null checks
	if(!json)
		return ERR_FAILURE;
	if(!lookup_query)
		return ERR_FAILURE;

	// return the path addition result
	return BP_JSONAddNodeByPathQuery(json, lookup_query, NULL, 0, BP_FALSE);


}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Destroy Nodes and Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// --- remove nodes -------------------

// Destroy node by path
BP_ERROR_T BP_JSONRemoveNodeByPath
(
		P_BP_JSON json,
		char    **path
)
{

	// ensure we have a json pointer and a path
	if(!json)
		return ERR_FAILURE;
	if(!path)
		return ERR_FAILURE;

	printf("\n Getting here, attempting to call BP_HashRegDestroyEntryByPath");

	// destroy entry by path
	return BP_HashRegDestroyEntryByPath
	(
		json->json_out_hreg,
		path
	);

}

// Add node by path (lookup query is in the form: /index1/whatever_index2/something_three/target data.
BP_ERROR_T BP_JSONRemoveNodeByPathQuery
(
		P_BP_JSON json,
		char    * lookup_query
)
{

	// ensure lookup query is reasonable/printable string
	if(!BP_StringIsReasonablePrintableString(lookup_query, bpstrlen(lookup_query), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// create path array from lookup query
	char ** path_lookup_array = BP_HashRegStringLookupToPathArray(lookup_query, bpstrlen(lookup_query));

	// check to ensure we've created a lookup array
	if(!path_lookup_array)
		return ERR_FAILURE;

	// return the node by path
	return BP_JSONRemoveNodeByPath
	(
			json,
			path_lookup_array
	);

}


// --- remove tables -------

// Add table by path query.
BP_ERROR_T BP_JSONRemoveTableByPath
(
		P_BP_JSON json,
		char    **path
)
{

	// ensure we have json and a path
	if(!json)
		return ERR_FAILURE;
	if(!path)
		return ERR_FAILURE;

	// destroy table by path
	return BP_HashRegDestroyTableByPath(json->json_out_hreg, path);

}

// Add table by path query.
BP_ERROR_T BP_JSONRemoveTableByPathQuery
(
		P_BP_JSON json,
		char    * lookup_query
)
{

	// ensure lookup query is reasonable/printable string
	if(!BP_StringIsReasonablePrintableString(lookup_query, bpstrlen(lookup_query), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// create path array from lookup query
	char ** path_lookup_array = BP_HashRegStringLookupToPathArray(lookup_query, bpstrlen(lookup_query));

	// check to ensure we've created a lookup array
	if(!path_lookup_array)
		return ERR_FAILURE;

	// attempt to remove the table by path
	BP_ERROR_T remove_status = BP_JSONRemoveTableByPath
	(
			json,
			path_lookup_array
	);

	// destroy lookup array
	BP_DestroyPointerArrayNullTerminated((void **) path_lookup_array);
	bpfree(path_lookup_array);

	// return the status
	return remove_status;


}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Replace Nodes and Tables (remove first and re-add) %%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Replace node by path
BP_ERROR_T BP_JSONReplaceNodeByPath
(
		P_BP_JSON json,
		char    **path,
		char    * data,
		size_t    data_len,
		BP_BOOL   base64_encode_data
)
{

	// ensure we have json and a path
	if(!json)
		return ERR_FAILURE;
	if(!path)
		return ERR_FAILURE;


	// remove node by path first (auto-fails if node doesn't exist)
	BP_JSONRemoveNodeByPath(json, path);

	// now add by path
	return BP_JSONAddNodeByPath(json, path, data, data_len, base64_encode_data);

}

// Replace node by path (lookup query is in the form: /index1/whatever_index2/something_three/target data.
BP_ERROR_T BP_JSONReplaceNodeByPathQuery
(
		P_BP_JSON json,
		char    * lookup_query,
		char    * data,
		size_t    data_len,
		BP_BOOL   base64_encode_data
)
{

	// basic null checks
	if(!json)
		return ERR_FAILURE;
	if(!lookup_query)
		return ERR_FAILURE;


	// attemt to remove table first (auto fails if it doesn't exist)
	BP_JSONRemoveNodeByPathQuery
	(
			json,
			lookup_query
	);

	// add node by path query
	return BP_JSONAddNodeByPathQuery(json, lookup_query, data, data_len, base64_encode_data);

}


// Replace table by path.
BP_ERROR_T BP_JSONReplaceTableByPath
(
		P_BP_JSON json,
		char    **path,
		BP_BOOL   table_is_anonymous
)
{


	// ensure we have json and a path
	if(!json)
		return ERR_FAILURE;
	if(!path)
		return ERR_FAILURE;

	// remove node by path first (auto-fails if node doesn't exist)
	BP_JSONRemoveTableByPath(json, path);

	// now add by path
	return BP_JSONAddTableByPath(json, path, table_is_anonymous);

}


// Replace table by path query.
BP_ERROR_T BP_JSONReplaceTableByPathQuery
(
		P_BP_JSON json,
		char    * lookup_query,
		BP_BOOL   table_is_anonymous
)
{

	// basic null checks
	if(!json)
		return ERR_FAILURE;
	if(!lookup_query)
		return ERR_FAILURE;

	// remove node by path first (auto-fails if node doesn't exist)
	BP_JSONRemoveTableByPathQuery(json, lookup_query);

	// now add by path
	return BP_JSONAddTableByPathQuery(json, lookup_query, table_is_anonymous);

}




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Parse JSON From Buffer %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Basic json string compare featured in the simple json example, but updated to work
// with additional checks.
BP_ERROR_T BP_JSONStringEq(const char *json, jsmntok_t *tok, const char *s)
{

	// run basic checks
	if(!json)
		return ERR_FAILURE;
	if(!tok)
		return ERR_FAILURE;
	if(!s)
		return ERR_FAILURE;


	// run checks
	if(tok->type       == JSMN_STRING)
	if((int) bpstrlen(s) == tok->end)
	if(tok->start)
	if(strncmp(json + tok->start, s, tok->end - tok->start) == 0)
	{

		// string must be reasonable printable string or we exit with failure
		if(!BP_StringIsReasonablePrintableString(( char *) json + tok->start, tok->end - tok->start, BP_FALSE, BP_FALSE))
			return ERR_FAILURE;

		// return indicating success
		return ERR_SUCCESS;

	}

	// return indicating failure
	return ERR_FAILURE;

}


// Returns a parse state object to be used by the parser.  Errors/warnings/notices are
// tracked in the parent json structure.
P_BP_JSON_PARSE_STATE BP_JSONCreateInitialParseState
(
		P_BP_JSON json,
		char  *buffer,
		size_t buffer_size,
		size_t max_depth_for_token_allocation,
		size_t max_tokens_total
)
{

	// json structure check
	if(!json)
		return NULL;

	// ensure we have buffer and buffer size
	if(!buffer)
	{
		BP_JSONParseAddErrorMsg(json, NULL, "Initial parse state creation failed because passed in buffer was null.");
		return NULL;
	}

	if(!buffer_size)
	{
		BP_JSONParseAddErrorMsg(json, NULL, "Initial parse state creation failed because passed in buffer_size was zero.");
		return NULL;
	}

	if(!BP_StringIsReasonablePrintableString(buffer, buffer_size, BP_TRUE, BP_TRUE))
	{
		BP_JSONParseAddErrorMsg(json, NULL, "Initial parse state creation failed because passed in buffer was unreasonable/non-printable.");
		return NULL;
	}

	// ensure max depth and max tokens have been passed ok
	if(!max_depth_for_token_allocation)
	{
		BP_JSONParseAddErrorMsg(json, NULL, "Initial parse state creation failed because max_depth_for_token_allocation was zero");
		return NULL;
	}

	if(!max_tokens_total)
	{
		BP_JSONParseAddErrorMsg(json, NULL, "Initial parse state creation failed because max_tokens_total was zero");
		return NULL;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Parse State %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new tail queue allocator
	P_BP_LINKL tq = BP_CreateLinkL(".", BP_TRUE);
	if(!tq)
	{
		BP_JSONParseAddErrorMsg(json, NULL, "Initial parse state creation failed because we were unable to create a internal tail queue allocator.  This is a critical internal memory error and could possibly be an indicator that the system is out of memory.  Administrators have been notified.");
		return NULL;
	}

	// create new parse state
	P_BP_JSON_PARSE_STATE new_state = (P_BP_JSON_PARSE_STATE) tq_calloc(tq, sizeof(BP_JSON_PARSE_STATE), 1, BPLN);
	if(!new_state)
	{
		BP_JSONParseAddErrorMsg(json, NULL, "Initial parse state creation failed because we were unable to allocate memory for the state.  Critical error.");

		BP_DestroyLinkL(tq, BP_TRUE);
		return NULL;
	}

	// set allocator internally
	new_state->tq = tq;


	// ---- set buffer ---------

	// duplicate buffer to parse from (prevents ptr shenanigans which could possibly occur during any threading)
	new_state->buffer = (char *) tq_strndup(new_state->tq, buffer, buffer_size, BPLN);

	// set buffer size
	new_state->buffer_size = buffer_size;


	// ---- create token extractions --------------

	// allocate space for jsmn tokens
	new_state->jsmn_tokens = (jsmntok_t *) tq_calloc
	(
		new_state->tq,
		sizeof(jsmntok_t) * (max_tokens_total + 1),
		1,
		BPLN
	);

	// ensure we have created our tokens
	if(!new_state->jsmn_tokens)
	{
		BP_JSONParseAddErrorMsg(json, NULL, "Initial parse state creation failed because we were unable to allocate memory for the state tokens.  Critical error.");
		BP_DestroyLinkL(tq, BP_TRUE);
		return NULL;
	}

	// set the tokens allocated count
	new_state->jsmn_tokens_allocated_n = max_tokens_total;


	// ---- create space for token element array ----

	// allocate space for jsmn tokens
	new_state->token_element_stack = (P_BP_JSON_PARSE_TOKEN_ELEMENT) tq_calloc
	(
		new_state->tq,
		sizeof(BP_JSON_PARSE_TOKEN_ELEMENT) * (max_tokens_total + 1),
		1,
		BPLN
	);

	// ensure we have created our tokens
	if(!new_state->token_element_stack)
	{
		BP_DestroyLinkL(tq, BP_TRUE);
		return NULL;
	}

	// set the item stack
	new_state->token_element_stack_items_allocated_n = max_tokens_total;




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Init and Create Space Allocations for Parse %%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// initialize the parser
	jsmn_init(json, &new_state->p);

	// attempt json parse and get parse token count as integer
	int parse_token_cnt = jsmn_parse
	(
		json,
		&new_state->p,
		new_state->buffer,
		new_state->buffer_size,
		new_state->jsmn_tokens,
		new_state->jsmn_tokens_allocated_n
	);

	// handle error val (less than zero is parse error)
	if(parse_token_cnt < 0)
	{

		switch(parse_token_cnt)
		{

			case JSMN_ERROR_INVAL:
				BP_JSONParseAddErrorMsg(json, NULL, "Parser failed reporting JSMN_ERROR_INVAL.  This means there is an error in your JSON which prevented it from being parsed as valid JSON.  Please attempt to check your json for bad values (such as via jslint.com).");
				break;

			case JSMN_ERROR_NOMEM:
				BP_JSONParseAddErrorMsg(json, NULL, "Parser failed reporting JSMN_ERROR_NOMEM.  If this message gets added it will be a miracle because we're already out of memory somehow.  Will attempt to notify admins.");
				break;

			case JSMN_ERROR_PART:
				BP_JSONParseAddErrorMsg(json, NULL, "Parser failed reporting JSMN_ERROR_PART.  This means that JSON was parsing ok, but JSON data was unexpectedly cut short.");
				break;

			default:
				BP_JSONParseAddErrorMsg(json, NULL, "Parser failed reporting an unknown error type.  This is undefined behanvior.");
				break;

		}

		// printf("\n [!!] JSON parse failed due to bad parse.");
		// destroy jsmn tokens before exit

		BP_DestroyLinkL(tq, BP_TRUE);
		return NULL;

	}

	// Handle zero error.  Zero indicates that parse was ok but object was empty.
	if(parse_token_cnt == 0)
	{

		// printf("\n [!!] JSON parse failed due to zero token count.");
		// destroy jsmn_tokens before exit
		BP_JSONParseAddErrorMsg(json, NULL, "Parser failed reporting that JSON parsed ok, but somehow has a zero token count.");
		BP_DestroyLinkL(tq, BP_TRUE);
		return NULL;

	}

	/*
	// check first token type
	if(new_state->jsmn_tokens[0].type != JSMN_OBJECT)
	{
		// printf("\n [!!] JSON parse failed due to zero token count.");
		// destroy jsmn_tokens before exit
		BP_JSONParseAddErrorMsg(json, NULL, "Parser failed reporting that JSON parsed ok, but the first token was somehow not an object.  Enusre that your json is wrapped in exterior {} characters.");
		BP_DestroyLinkL(tq, BP_TRUE);
		return NULL;

	}
	*/

	// convert from int to size_t and assign here (previous negative value check ensure's value is within signed range)
	new_state->parsed_token_count = (size_t) parse_token_cnt;

	// set parsed_tokens_remaining item (same as count/decremented as parse occurs)
	new_state->parsed_tokens_remaining = new_state->parsed_token_count;


	// Due to calloc()/all other elements are set to zero/null automatically.

	// return the filled out state
	return new_state;

}


// destroy the parse state
BP_ERROR_T BP_JSONDestroyParseState(P_BP_JSON_PARSE_STATE parse_state)
{

	// ensure we have a parse state
	if(!parse_state)
		return ERR_FAILURE;

	// ensure we have a tailqueue allocator
	if(!parse_state->tq)
		return ERR_FAILURE;

	// attempt to destroy the parse state by destroying the linkl allocator
	if(!BP_DestroyLinkL(parse_state->tq, BP_TRUE))
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;
}


// --- Advances the Parse State -------------------

BP_ERROR_T BP_JSONDisplayParseState(P_BP_JSON_PARSE_STATE parse_state)
{

	// ensure we have a parse state
	if(!parse_state)
		return ERR_FAILURE;

	// create tab buff
	char * tab_buff = BP_CreateTabBuffer(parse_state->current_traversal_depth, NULL);

	// set the use buff logically
	char *used_tab_buff = tab_buff ? tab_buff : (char *) "";

	// display the parse state
	printf("\n%s [+] JSON Parse State: %p", used_tab_buff, parse_state);
	printf("\n%s    jsmn_parser p:                         %p",  used_tab_buff, &parse_state->p);
	printf("\n%s    jsmn_tokens:                           %p",  used_tab_buff, parse_state->jsmn_tokens);
	printf("\n%s    jsmn_tokens_allocated_n:               %zu", used_tab_buff, parse_state->jsmn_tokens_allocated_n);
	printf("\n%s    parsed_token_count:                    %zu", used_tab_buff, parse_state->parsed_token_count);
	printf("\n%s    parsed_tokens_remaining:               %zu", used_tab_buff, parse_state->parsed_tokens_remaining);
	printf("\n%s    buffer:                                %p",  used_tab_buff, parse_state->buffer);
	printf("\n%s    buffer_size:                           %zu", used_tab_buff, parse_state->buffer_size);
	printf("\n%s    max_depth_for_token_allocation:        %zu", used_tab_buff, parse_state->max_depth_for_token_allocation);
	printf("\n%s    current_traversal_depth:               %zu", used_tab_buff, parse_state->current_traversal_depth);
	printf("\n%s    token_element_stack:                   %p",  used_tab_buff, parse_state->token_element_stack);
	printf("\n%s    token_element_stack_items_allocated_n: %zu",  used_tab_buff, parse_state->token_element_stack_items_allocated_n);
	printf("\n%s    current_token_element_index:           %zu", used_tab_buff, parse_state->current_token_element_index);

	// this is set to BP_TRUE if during parsing, any anonymous arrays
	// were detected.
	printf("\n%s    parse_encountered_anonymous_arrays:                      %s", used_tab_buff, parse_state->parse_encountered_anonymous_arrays ? "BP_TRUE" : "BP_FALSE");

	// this is set to BP_TRUE if during parsing, any anonymous objects
	// were detected.
	printf("\n%s    parse_encountered_anonymous_objects:                     %s", used_tab_buff, parse_state->parse_encountered_anonymous_objects ? "BP_TRUE" : "BP_FALSE");

	// combined flag for both above for simplicity
	printf("\n%s    parse_encountered_anonymous_arrays_or_objects:           %s", used_tab_buff, parse_state->parse_encountered_anonymous_arrays_or_objects ? "BP_TRUE" : "BP_FALSE");

	// this is set to BP_TRUE if during parsing, any primitives are encountered
	// without any keys.
	printf("\n%s    parse_encountered_primitive_without_key:                 %s", used_tab_buff, parse_state->parse_encountered_primitive_without_key ? "BP_TRUE" : "BP_FALSE");

	// this is set to BP_TRUE if during parsing, any strings are encountered
	// without any keys.
	printf("\n%s    parse_encountered_string_without_key:                    %s", used_tab_buff, parse_state->parse_encountered_string_without_key ? "BP_TRUE" : "BP_FALSE");

	// this is set to bp true if a string or primitive without a key is encountered
	printf("\n%s    parse_encountered_string_or_privitive_without_key:       %s", used_tab_buff, parse_state->parse_encountered_string_or_privitive_without_key ? "BP_TRUE" : "BP_FALSE");

	// --- Duplicate Check Flags ----------------------------

	// this is set if the parse encountered duplicate keys in the same array
	printf("\n%s    parse_encountered_duplicate_keys_in_the_same_array:      %s", used_tab_buff, parse_state->parse_encountered_duplicate_keys_in_the_same_array ? "BP_TRUE" : "BP_FALSE");

	// this is set if the parse encountered duplicate keys in the same object
	printf("\n%s    parse_encountered_duplicate_keys_in_the_same_object:     %s", used_tab_buff, parse_state->parse_encountered_duplicate_keys_in_the_same_object ? "BP_TRUE" : "BP_FALSE");

	// this is set if either of the above are true
	printf("\n%s    parse_encountered_duplicate_keys_in_array_or_object      %s", used_tab_buff, parse_state->parse_encountered_duplicate_keys_in_array_or_object ? "BP_TRUE" : "BP_FALSE");


	printf("\n%s    last_token:                            %p",  used_tab_buff, parse_state->last_token);
	printf("\n%s    last_token_parent:                     %p" , used_tab_buff, parse_state->last_token_parent);
	printf("\n%s    tq:                                    %p",  used_tab_buff, parse_state->tq);




	// destroy the tab buff if necessary
	if(tab_buff)
		bpfree(tab_buff);

	// return indicating success
	return ERR_SUCCESS;
}

// display the current parse state in terms of path queries
BP_ERROR_T BP_JSONDisplayParseStatePathsQueriesOnly(P_BP_JSON_PARSE_STATE parse_state)
{

	// run basic null checks
	if(!parse_state)
		return ERR_FAILURE;
	if(!parse_state->token_element_stack)
		return ERR_FAILURE;
	if(!parse_state->token_element_stack_items_allocated_n)
		return ERR_FAILURE;

	// set element iter to zero
	size_t element_iter = 0;

	// set current element
	P_BP_JSON_PARSE_TOKEN_ELEMENT curr_element = NULL;

	// top level is always going to be the current parsed element index
	for(; element_iter < parse_state->current_token_element_index; element_iter++)
	{

		// set current element
		curr_element = &parse_state->token_element_stack[element_iter];

		// display path query to current
		printf("\n %s", curr_element->path_query_to_current);

	}

	// return indicating success
	return ERR_SUCCESS;


}

// display a parse element
BP_ERROR_T BP_JSONDisplayParseElement(P_BP_JSON_PARSE_STATE parse_state, P_BP_JSON_PARSE_TOKEN_ELEMENT parse_element, BP_BOOL only_display_relevant)
{

	// check for parse element
	if(!parse_element)
		return ERR_FAILURE;

	// ensure we have a parse state to work with
	if(!parse_state)
		return ERR_FAILURE;

	// check for token element
	printf("\n [+] JSON Parse Token Element: %p", parse_element);

	if(only_display_relevant && parse_element->token_init_ok)
	{

		// BP_BOOL token_init_ok;
		printf("\n\t token_init_ok: %s", parse_element->token_init_ok ? "BP_TRUE" : "BP_FALSE");

	}

	// BP_BOOL is_object;
	if(only_display_relevant && parse_element->is_object)
	{

		printf("\n\t is_object: %s", parse_element->is_object ? "BP_TRUE" : "BP_FALSE");

	}

	// BP_BOOL is_array;
	if(only_display_relevant && parse_element->is_array)
	{

		printf("\n\t is_array: %s", parse_element->is_array ? "BP_TRUE" : "BP_FALSE");

	}

	// BP_BOOL is_string;
	if(only_display_relevant && parse_element->is_string)
	{

		printf("\n\t is_string: %s", parse_element->is_string ? "BP_TRUE" : "BP_FALSE");

	}

	// BP_BOOL is_primitive;
	if(only_display_relevant && parse_element->is_primitive)
	{

		printf("\n\t is_primitive: %s", parse_element->is_primitive ? "BP_TRUE" : "BP_FALSE");

	}

	// BP_BOOL has_parent;
	if(only_display_relevant && parse_element->has_parent)
	{

		printf("\n\t has_parent: %s", parse_element->has_parent ? "BP_TRUE" : "BP_FALSE");

	}

	// BP_BOOL parent_is_anonymous;
	if(only_display_relevant && parse_element->parent_is_anonymous)
	{

		printf("\n\t parent_is_anonymous: %s", parse_element->parent_is_anonymous ? "BP_TRUE" : "BP_FALSE");

	}

	// BP_BOOL current_token_label_is_anonymous;
	if(only_display_relevant && parse_element->current_token_label_is_anonymous)
	{

		printf("\n\t current_token_label_is_anonymous: %s", parse_element->current_token_label_is_anonymous ? "BP_TRUE" : "BP_FALSE");

	}

	// BP_BOOL parent_is_object;
	if(only_display_relevant && parse_element->parent_is_object)
	{

		printf("\n\t parent_is_object: %s", parse_element->parent_is_object ? "BP_TRUE" : "BP_FALSE");

	}

	// BP_BOOL parent_is_array;
	if(only_display_relevant && parse_element->parent_is_array)
	{

		printf("\n\t parent_is_array: %s", parse_element->parent_is_array ? "BP_TRUE" : "BP_FALSE");

	}

	// BP_BOOL parent_is_string;
	if(only_display_relevant && parse_element->parent_is_string)
	{

		printf("\n\t parent_is_string: %s", parse_element->parent_is_string ? "BP_TRUE" : "BP_FALSE");

	}

	// BP_BOOL parent_is_primitive;
	if(only_display_relevant && parse_element->parent_is_primitive)
	{

		printf("\n\t parent_is_primitive: %s", parse_element->parent_is_primitive ? "BP_TRUE" : "BP_FALSE");

	}

	// BP_BOOL has_children;
	if(only_display_relevant && parse_element->has_children)
	{

		printf("\n\t has_children: %s", parse_element->has_children ? "BP_TRUE" : "BP_FALSE");

	}

	// size_t child_count;
	if(only_display_relevant && parse_element->child_count)
	{

		printf("\n\t child_count: %zu", parse_element->child_count);

	}

	// BP_BOOL child_is_string_only;
	if(only_display_relevant && parse_element->child_is_string_only)
	{

		printf("\n\t child_is_string_only: %s", parse_element->child_is_string_only ? "BP_TRUE" : "BP_FALSE");

	}

	// char *child_as_string
	if(only_display_relevant && parse_element->child_as_string)
	{

		printf("\n\t child_as_string: %s", parse_element->child_as_string);

	}

	// BP_BOOL child_is_primitive_only;
	if(only_display_relevant && parse_element->child_is_primitive_only)
	{

		printf("\n\t child_is_primitive_only: %s", parse_element->child_is_primitive_only ? "BP_TRUE" : "BP_FALSE");

	}

	// char *child_as_primitive
	if(only_display_relevant && parse_element->child_as_primitive)
	{

		printf("\n\t child_as_primitive: %s", parse_element->child_as_primitive);

	}

	// jsmntok_t *child_tokens_start;
	if(only_display_relevant && parse_element->child_tokens_start)
	{

		printf("\n\t child_tokens_start: %p", parse_element->child_tokens_start);

	}

	// char * parent_string_val;
	if(only_display_relevant && parse_element->parent_string_val)
	{

		printf("\n\t parent_string_val: %s", parse_element->parent_string_val);

	}

	// char * parent_primitive_val;
	if(only_display_relevant && parse_element->parent_primitive_val)
	{

		printf("\n\t parent_primitive_val: %s", parse_element->parent_primitive_val);

	}

	// char * current_string_val;
	if(only_display_relevant && parse_element->current_string_val)
	{

		printf("\n\t current_string_val: %s", parse_element->current_string_val);

	}

	// char * current_primitive_val;
	if(only_display_relevant && parse_element->current_primitive_val)
	{

		printf("\n\t current_primitive_val: %s", parse_element->current_primitive_val);

	}

	// jsmntok_t *parent_token;
	if(only_display_relevant && parse_element->parent_token)
	{

		printf("\n\t parent_token: %p", parse_element->parent_token);

	}

	// jsmntok_t *current_token;
	if(only_display_relevant && parse_element->current_token)
	{

		printf("\n\t current_token: %p", parse_element->current_token);

	}

	// display parent token element
	if(only_display_relevant && parse_element->parent_token_element)
	{

		printf("\n\t parent_token_element: %p", parse_element->parent_token_element);

	}

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% JSON Parser Error/Warn/Notice Gen Routines %%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// add a parse error to the parse output hash table registry
BP_ERROR_T BP_JSONParseAddErrorMsg(P_BP_JSON json, P_BP_JSON_PARSE_TOKEN_ELEMENT token_element, char *message, int buff_position, char bad_char)
{

	// run basic checks
	if(!json)
		return ERR_FAILURE;
	if(!message)
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(message, bpstrlen(message), BP_FALSE, BP_FALSE))
			return ERR_FAILURE;


	// table string for lookup
	char * table_str = "/errors";

	// msg string for add
	char * msg_str = message;



	// create registry jit if necessary
	if(!json->parse_messages_reg)
		json->parse_messages_reg = BP_CreateHashTableRegistry();



	// attempt to lookup errors table by path
	P_BP_HTKE error_table = BP_HashRegLookupTableByPathQuery(json->parse_messages_reg, table_str);
	if(!error_table)
	{

		// attempt to add the errors table
		BP_HashRegAddTableByPathQuery(json->parse_messages_reg, table_str);

		// attempt lookup again
		error_table = BP_HashRegLookupTableByPathQuery(json->parse_messages_reg, table_str);
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
		return ERR_FAILURE;

	// create new table
	if(!BP_HashRegAddTableByPathQuery(json->parse_messages_reg, inner_table_path))
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Data To Table %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// -- add /errors/sha1/msg

	// add data to table path by query
	if(!BP_HashRegAddDataToTableByPathQuery(json->parse_messages_reg, inner_table_path, "msg", BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING, msg_str, bpstrlen(msg_str)))
	{
		bpfree(random_sha1_key);
		return ERR_FAILURE;
	}

	// -- add /errors/sha1/buff_pos

	// allocate space on the stack to hold our position
	char position_buff[64];
	BP_ZERO_STACK(position_buff);

	// copy in the buffer position
	sprintf((char *)&position_buff, "%i", buff_position);

	// add data to table path by query
	if(!BP_HashRegAddDataToTableByPathQuery(json->parse_messages_reg, inner_table_path, "buff_pos", BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING, &position_buff, bpstrlen(position_buff)))
	{
		bpfree(random_sha1_key);
		return ERR_FAILURE;
	}

	// -- add /errors/sha1/bad_char

	// add data to table path by query
	if(!BP_HashRegAddDataToTableByPathQuery(json->parse_messages_reg, inner_table_path, "bad_char", BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING, &bad_char, 1))
	{
		bpfree(random_sha1_key);
		return ERR_FAILURE;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Dangling Data and Exit %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// destroy the sha1 key
	bpfree(random_sha1_key);

	// --- attach

	// return indicating success
	return ERR_SUCCESS;

}

// Add a parse warning to the parse output hash table registry
BP_ERROR_T BP_JSONParseAddWarningMsg(P_BP_JSON json, P_BP_JSON_PARSE_TOKEN_ELEMENT token_element, char *message, int buff_position, char bad_char)
{

	// run basic null checks
	if(!json)
		return ERR_FAILURE;
	if(!message)
		return ERR_FAILURE;

	// table string for lookup
	char * table_str = "/warnings";

	// msg string for add
	char * msg_str = message;

	// ensure string is reasonable/printable
	if(!BP_StringIsReasonablePrintableString(msg_str, bpstrlen(msg_str), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// create registry jit if necessary
	if(!json->parse_messages_reg)
		json->parse_messages_reg = BP_CreateHashTableRegistry();



	// attempt to lookup warnings table by path
	P_BP_HTKE warning_table = BP_HashRegLookupTableByPathQuery(json->parse_messages_reg, table_str);
	if(!warning_table)
	{

		// attempt to add the warnings table
		BP_HashRegAddTableByPathQuery(json->parse_messages_reg, table_str);

		// attempt lookup again
		warning_table = BP_HashRegLookupTableByPathQuery(json->parse_messages_reg, table_str);
		if(!warning_table)
			return ERR_FAILURE;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Table For New Warning %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create random sha1 key
	char * random_sha1_key = BP_GenRandomSHA1();

	// set the addition path
	char *add_new_table_path[] =
	{
		"warnings",
		random_sha1_key,
		NULL
	};

	// inner table path with table index set
	char * inner_table_path = BP_HashRegPathArrayToStringLookup(add_new_table_path);
	if(!inner_table_path)
		return ERR_FAILURE;

	// create new table
	if(!BP_HashRegAddTableByPathQuery(json->parse_messages_reg, inner_table_path))
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Data To Table %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// -- add /warnings/sha1/msg

	// add data to table path by query
	if(!BP_HashRegAddDataToTableByPathQuery(json->parse_messages_reg, inner_table_path, "msg", BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING, msg_str, bpstrlen(msg_str)))
	{
		bpfree(random_sha1_key);
		return ERR_FAILURE;
	}

	// -- add /warnings/sha1/buff_pos

	// allocate space on the stack to hold our position
	char position_buff[64];
	BP_ZERO_STACK(position_buff);

	// copy in the buffer position
	sprintf((char *)&position_buff, "%i", buff_position);

	// add data to table path by query
	if(!BP_HashRegAddDataToTableByPathQuery(json->parse_messages_reg, inner_table_path, "buff_pos", BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING, &position_buff, bpstrlen(position_buff)))
	{
		bpfree(random_sha1_key);
		return ERR_FAILURE;
	}

	// -- add /warnings/sha1/bad_char

	// add data to table path by query
	if(!BP_HashRegAddDataToTableByPathQuery(json->parse_messages_reg, inner_table_path, "bad_char", BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING, &bad_char, 1))
	{
		bpfree(random_sha1_key);
		return ERR_FAILURE;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Dangling Data and Exit %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// destroy the sha1 key
	bpfree(random_sha1_key);

	// --- attach

	// return indicating success
	return ERR_SUCCESS;

}

// add a parse notice to the parse output hash table registry
BP_ERROR_T BP_JSONParseAddNoticeMsg(P_BP_JSON json, P_BP_JSON_PARSE_TOKEN_ELEMENT token_element, char *message, int buff_position, char bad_char)
{

	// run basic null checks
	if(!json)
		return ERR_FAILURE;
	if(!message)
		return ERR_FAILURE;

	// table string for lookup
	char * table_str = "/notices";

	// msg string for add
	char * msg_str = message;

	// ensure string is reasonable/printable
	if(!BP_StringIsReasonablePrintableString(msg_str, bpstrlen(msg_str), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// create registry jit if necessary
	if(!json->parse_messages_reg)
		json->parse_messages_reg = BP_CreateHashTableRegistry();



	// attempt to lookup notices table by path
	P_BP_HTKE notice_table = BP_HashRegLookupTableByPathQuery(json->parse_messages_reg, table_str);
	if(!notice_table)
	{

		// attempt to add the notices table
		BP_HashRegAddTableByPathQuery(json->parse_messages_reg, table_str);

		// attempt lookup again
		notice_table = BP_HashRegLookupTableByPathQuery(json->parse_messages_reg, table_str);
		if(!notice_table)
			return ERR_FAILURE;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Table For New Notice %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create random sha1 key
	char * random_sha1_key = BP_GenRandomSHA1();

	// set the addition path
	char *add_new_table_path[] =
	{
		"notices",
		random_sha1_key,
		NULL
	};

	// inner table path with table index set
	char * inner_table_path = BP_HashRegPathArrayToStringLookup(add_new_table_path);
	if(!inner_table_path)
		return ERR_FAILURE;

	// create new table
	if(!BP_HashRegAddTableByPathQuery(json->parse_messages_reg, inner_table_path))
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Data To Table %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// -- add /notices/sha1/msg

	// add data to table path by query
	if(!BP_HashRegAddDataToTableByPathQuery(json->parse_messages_reg, inner_table_path, "msg", BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING, msg_str, bpstrlen(msg_str)))
	{
		bpfree(random_sha1_key);
		return ERR_FAILURE;
	}

	// -- add /notices/sha1/buff_pos

	// allocate space on the stack to hold our position
	char position_buff[64];
	BP_ZERO_STACK(position_buff);

	// copy in the buffer position
	sprintf((char *)&position_buff, "%i", buff_position);

	// add data to table path by query
	if(!BP_HashRegAddDataToTableByPathQuery(json->parse_messages_reg, inner_table_path, "buff_pos", BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING, &position_buff, bpstrlen(position_buff)))
	{
		bpfree(random_sha1_key);
		return ERR_FAILURE;
	}

	// -- add /notices/sha1/bad_char

	// add data to table path by query
	if(!BP_HashRegAddDataToTableByPathQuery(json->parse_messages_reg, inner_table_path, "bad_char", BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING, &bad_char, 1))
	{
		bpfree(random_sha1_key);
		return ERR_FAILURE;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Dangling Data and Exit %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// destroy the sha1 key
	bpfree(random_sha1_key);

	// --- attach

	// return indicating success
	return ERR_SUCCESS;

}


// -- message display routines -------------------


// display notices
BP_ERROR_T BP_JSONDisplayParseNotices(P_BP_JSON json)
{

	// ensure we have a json structure
	if(!json)
		return ERR_FAILURE;

	// ensure we have a parse messages reg and hive
	if(!json->parse_messages_reg)
		return ERR_FAILURE;
	if(!json->parse_messages_reg->hive)
		return ERR_FAILURE;

	// attempt to lookup notice table
	P_BP_HTKE notice_table_lookup   = BP_HashRegLookupTableByPathQuery(json->parse_messages_reg, "/notices");

	// ensure we can lookup notices table
	if(!notice_table_lookup)
		return ERR_FAILURE;

	P_BP_HTKE notice_table_children =(P_BP_HTKE) notice_table_lookup->data;

	// iterate through the notices
	BP_HASH_ITER_OPEN(notice_table_children, notice_iter_current)
	{

		// continue the loop if the current type is not a keyed table entry
		if(notice_iter_current->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
			continue;

		// set lookup path
		char *lookup_path[] =
		{
				"notices",
				notice_iter_current->key,
				NULL
		};

		// create the table lookup path
		char * table_path = BP_HashRegPathArrayToStringLookup(lookup_path);
		if(!table_path)
			continue;

		// retrieve the msg entry from the table
		P_BP_HTKE msg_entry = BP_HashRegLookupTableEntryByDataKeyIndexAndTablePathQuery(json->parse_messages_reg, table_path, "msg");

		// retrieve the buffer position entry from the table
		P_BP_HTKE buff_pos_entry = BP_HashRegLookupTableEntryByDataKeyIndexAndTablePathQuery(json->parse_messages_reg, table_path, "buff_pos");

		// retrieve the bad character entry
		P_BP_HTKE bad_char_entry = BP_HashRegLookupTableEntryByDataKeyIndexAndTablePathQuery(json->parse_messages_reg, table_path, "bad_char");

		// destroy table path on loop
		bpfree(table_path);


		// signed long to hold our buffer position
		long buffer_position = 0;

		// attempt conversion
		BP_StringToSignedLongBase10((char *) buff_pos_entry->data, buff_pos_entry->data_size, &buffer_position);


		// if buffer position is -1, it means that json parsing failed but there was no
		// bad character or buffer position to examine (a general notice)
		if(buffer_position == -1)
		{
			printf("\n [!!] JSON General Parse Notice: %s", (char *) msg_entry->data);
		}
		else
		{

			// If it's not -1, we parse out a specific notice here

			// if we have a position and bad char, perform buffer to type conversions and display output message
			if(buff_pos_entry       && bad_char_entry)
			if(buff_pos_entry->data && bad_char_entry->data)
			{

				// create typed pointer reference
				char *tmp_data = (char *) bad_char_entry->data;

				// set bad char
				char bad_char = tmp_data[0];


				// display message after data conversions take place
				printf("\n [!!] (pos:%i|chr:0x%.02x) JSON Parse Notice: %s", buffer_position, bad_char, (char *) msg_entry->data);

			}

		}

	}
	BP_HASH_ITER_CLOSE;

	// return indicating success
	return ERR_SUCCESS;

}

// display warnings
BP_ERROR_T BP_JSONDisplayParseWarnings(P_BP_JSON json)
{
	// ensure we have a json structure
	if(!json)
		return ERR_FAILURE;

	// ensure we have a parse messages reg and hive
	if(!json->parse_messages_reg)
		return ERR_FAILURE;
	if(!json->parse_messages_reg->hive)
		return ERR_FAILURE;

	// attempt to lookup warning table
	P_BP_HTKE warning_table_lookup   = BP_HashRegLookupTableByPathQuery(json->parse_messages_reg, "/warnings");

	// ensure we can lookup warnings table
	if(!warning_table_lookup)
		return ERR_FAILURE;

	P_BP_HTKE warning_table_children =(P_BP_HTKE) warning_table_lookup->data;

	// iterate through the warnings
	BP_HASH_ITER_OPEN(warning_table_children, warning_iter_current)
	{

		// continue the loop if the current type is not a keyed table entry
		if(warning_iter_current->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
			continue;

		// set lookup path
		char *lookup_path[] =
		{
				"warnings",
				warning_iter_current->key,
				NULL
		};

		// create the table lookup path
		char * table_path = BP_HashRegPathArrayToStringLookup(lookup_path);
		if(!table_path)
			continue;

		// retrieve the msg entry from the table
		P_BP_HTKE msg_entry = BP_HashRegLookupTableEntryByDataKeyIndexAndTablePathQuery(json->parse_messages_reg, table_path, "msg");

		// retrieve the buffer position entry from the table
		P_BP_HTKE buff_pos_entry = BP_HashRegLookupTableEntryByDataKeyIndexAndTablePathQuery(json->parse_messages_reg, table_path, "buff_pos");

		// retrieve the bad character entry
		P_BP_HTKE bad_char_entry = BP_HashRegLookupTableEntryByDataKeyIndexAndTablePathQuery(json->parse_messages_reg, table_path, "bad_char");

		// destroy table path on loop
		bpfree(table_path);


		// signed long to hold our buffer position
		long buffer_position = 0;

		// attempt conversion
		BP_StringToSignedLongBase10((char *) buff_pos_entry->data, buff_pos_entry->data_size, &buffer_position);


		// if buffer position is -1, it means that json parsing failed but there was no
		// bad character or buffer position to examine (a general warning)
		if(buffer_position == -1)
		{
			printf("\n [!!] JSON General Parse Warning: %s", (char *) msg_entry->data);
		}
		else
		{

			// If it's not -1, we parse out a specific warning here

			// if we have a position and bad char, perform buffer to type conversions and display output message
			if(buff_pos_entry       && bad_char_entry)
			if(buff_pos_entry->data && bad_char_entry->data)
			{

				// create typed pointer reference
				char *tmp_data = (char *) bad_char_entry->data;

				// set bad char
				char bad_char = tmp_data[0];


				// display message after data conversions take place
				printf("\n [!!] (pos:%i|chr:0x%.02x) JSON Parse Warning: %s", buffer_position, bad_char, (char *) msg_entry->data);

			}

		}

	}
	BP_HASH_ITER_CLOSE;

	// return indicating success
	return ERR_SUCCESS;

}


// display errors
BP_ERROR_T BP_JSONDisplayParseErrors(P_BP_JSON json)
{

	// ensure we have a json structure
	if(!json)
		return ERR_FAILURE;

	// ensure we have a parse messages reg and hive
	if(!json->parse_messages_reg)
		return ERR_FAILURE;
	if(!json->parse_messages_reg->hive)
		return ERR_FAILURE;

	// attempt to lookup error table
	P_BP_HTKE error_table_lookup   = BP_HashRegLookupTableByPathQuery(json->parse_messages_reg, "/errors");

	// ensure we can lookup errors table
	if(!error_table_lookup)
		return ERR_FAILURE;

	P_BP_HTKE error_table_children =(P_BP_HTKE) error_table_lookup->data;

	// iterate through the errors
	BP_HASH_ITER_OPEN(error_table_children, error_iter_current)
	{

		// continue the loop if the current type is not a keyed table entry
		if(error_iter_current->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
			continue;

		// set lookup path
		char *lookup_path[] =
		{
				"errors",
				error_iter_current->key,
				NULL
		};

		// create the table lookup path
		char * table_path = BP_HashRegPathArrayToStringLookup(lookup_path);
		if(!table_path)
			continue;

		// retrieve the msg entry from the table
		P_BP_HTKE msg_entry = BP_HashRegLookupTableEntryByDataKeyIndexAndTablePathQuery(json->parse_messages_reg, table_path, "msg");

		// retrieve the buffer position entry from the table
		P_BP_HTKE buff_pos_entry = BP_HashRegLookupTableEntryByDataKeyIndexAndTablePathQuery(json->parse_messages_reg, table_path, "buff_pos");

		// retrieve the bad character entry
		P_BP_HTKE bad_char_entry = BP_HashRegLookupTableEntryByDataKeyIndexAndTablePathQuery(json->parse_messages_reg, table_path, "bad_char");

		// destroy table path on loop
		bpfree(table_path);


		// signed long to hold our buffer position
		long buffer_position = 0;

		// attempt conversion
		BP_StringToSignedLongBase10((char *) buff_pos_entry->data, buff_pos_entry->data_size, &buffer_position);


		// if buffer position is -1, it means that json parsing failed but there was no
		// bad character or buffer position to examine (a general error)
		if(buffer_position == -1)
		{
			printf("\n [!!] JSON General Parse Error: %s", (char *) msg_entry->data);
		}
		else
		{

			// If it's not -1, we parse out a specific error here

			// if we have a position and bad char, perform buffer to type conversions and display output message
			if(buff_pos_entry       && bad_char_entry)
			if(buff_pos_entry->data && bad_char_entry->data)
			{

				// create typed pointer reference
				char *tmp_data = (char *) bad_char_entry->data;

				// set bad char
				char bad_char = tmp_data[0];


				// display message after data conversions take place
				printf("\n [!!] (pos:%i|chr:0x%.02x) JSON Parse Error: %s", buffer_position, bad_char, (char *) msg_entry->data);

			}

		}

	}
	BP_HASH_ITER_CLOSE;

	// return indicating success
	return ERR_SUCCESS;

}



// display all the parser messages
BP_ERROR_T BP_JSONDisplayParseMessages(P_BP_JSON json)
{

	// ensure we have a json structure
	if(!json)
		return ERR_FAILURE;

	// ensure we have a parse messages reg and hive
	if(!json->parse_messages_reg)
		return ERR_FAILURE;
	if(!json->parse_messages_reg->hive)
		return ERR_FAILURE;

	// attempt to lookup error table
	P_BP_HTKE error_table_lookup   = BP_HashRegLookupTableByPathQuery(json->parse_messages_reg, "/errors");

	// attempt to lookup warning table
	P_BP_HTKE warning_table_lookup = BP_HashRegLookupTableByPathQuery(json->parse_messages_reg, "/warnings");

	// attempt to lookup notice table
	P_BP_HTKE notice_table_lookup  = BP_HashRegLookupTableByPathQuery(json->parse_messages_reg, "/notices");

	// display errors if set
	if(error_table_lookup)
	{
		BP_JSONDisplayParseErrors(json);
	}

	// display warnings if set
	if(warning_table_lookup)
	{
		BP_JSONDisplayParseWarnings(json);
	}

	// display notices if set
	if(notice_table_lookup)
	{
		BP_JSONDisplayParseNotices(json);
	}

	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Token Data Extraction Routines %%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Attempts to get a string value from a JSMN_STRING type token. Parse state is utilized due to
// it containing a copy of the buffer being parsed.  Returned value must be "reasonable/printable" or
// it will be destroyed after extraction and no value will be returned.  Returned value is bpcalloced() and
// must be bpfreed().  Null is returned on failure.
char * BP_JSONGetStringValueFromJSMNToken(P_BP_JSON_PARSE_STATE parse_state, jsmntok_t *token)
{

	// ensure we have a parse state
	if(!parse_state)
		return NULL;

	// ensure we have a token
	if(!token)
		return NULL;

	// type must be NULL
	if(token->type != JSMN_STRING)
		return NULL;

	// ensure we have a buffer and a buffer size
	if(!parse_state->buffer)
		return NULL;
	if(!parse_state->buffer_size)
		return NULL;

	// ensure the buffer being passed in is reasonable/printable (since it's being used for extraction)
	if(!BP_StringIsReasonablePrintableString(parse_state->buffer, parse_state->buffer_size, BP_TRUE, BP_TRUE))
		return NULL;

	// neither start nor end can be -1
	if(token->start == -1)
		return NULL;
	if(token->end == -1)
		return NULL;



	// check to ensure that the start isn't past the end of our buffer
	if(token->start >= parse_state->buffer_size)
		return NULL;

	// ensure that the end isn't past the end of our buffer
	if(token->end >= parse_state->buffer_size)
		return NULL;


	// buffer length
	size_t token_buff_len = 0;

	// calculate the size of the buffer
	int size_calc = token->end - token->start;

	// ensure the size calculation is reasonable
	if(size_calc <= 0)
		return NULL;

	// convert to size_t after negative check
	token_buff_len = (size_t) size_calc;

	// token size isn't set in string types
	// if(token->size)

	// go ahead and copy out values now using strndup()
	char *token_value_as_string = bpstrndup((char *) &parse_state->buffer[token->start], token_buff_len);

	// exit if duplication failed for whatever reason
	if(!token_value_as_string)
		return NULL;

	// return indicating success
	return token_value_as_string;

}



// Attempts to retrieve a string value represenation of a primitive (exactly as BP_JSONGetStringValueFromJSMNToken(), with different type check)
char * BP_JSONGetPrimitiveValueAsStringValueFromJSMNToken(P_BP_JSON_PARSE_STATE parse_state, jsmntok_t *token)
{

	// ensure we have a parse state
	if(!parse_state)
		return NULL;

	// ensure we have a token
	if(!token)
		return NULL;

	// type must be JSMN_PRIMITIVE
	if(token->type != JSMN_PRIMITIVE)
		return NULL;

	// ensure we have a buffer and a buffer size
	if(!parse_state->buffer)
		return NULL;
	if(!parse_state->buffer_size)
		return NULL;

	// ensure the buffer being passed in is reasonable/printable (since it's being used for extraction)
	if(!BP_StringIsReasonablePrintableString(parse_state->buffer, parse_state->buffer_size, BP_TRUE, BP_TRUE))
		return NULL;

	// neither start nor end can be -1
	if(token->start == -1)
		return NULL;
	if(token->end == -1)
		return NULL;

	// printf("\n GETS THIS FAR IN CHECKS?");


	// check to ensure that the start isn't past the end of our buffer
	if(token->start >= parse_state->buffer_size)
		return NULL;

	// ensure that the end isn't past the end of our buffer
	if(token->end >= parse_state->buffer_size)
		return NULL;


	// buffer length
	size_t token_buff_len = 0;

	// calculate the size of the buffer
	int size_calc = token->end - token->start;

	// ensure the size calculation is reasonable
	if(size_calc <= 0)
		return NULL;

	// convert to size_t after negative check
	token_buff_len = (size_t) size_calc;

	// token size isn't set in string types
	// if(token->size)

	// go ahead and copy out values now using strndup()
	char *token_value_as_string = bpstrndup((char *) &parse_state->buffer[token->start], token_buff_len);

	// exit if duplication failed for whatever reason
	if(!token_value_as_string)
		return NULL;

	// return indicating success
	return token_value_as_string;

}


// Fills the path strings within the provided token element.
BP_ERROR_T BP_JSONGeneratePathBufferRecursivelyFromToken(P_BP_JSON_PARSE_STATE parse_state, P_BP_JSON_PARSE_TOKEN_ELEMENT token_element)
{

	// basic null checks
	if(!parse_state)
		return ERR_FAILURE;
	if(!token_element)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Step 1: Generate Full Path to Element %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new talqueue allocator for generating linkl
	P_BP_LINKL path_builder_tq = BP_CreateLinkL(".", BP_TRUE);

	// if we have a parent token, immediately recurse
	if(token_element->parent_token_element)
	{

		// recurse and attempt to generate token
		if(!BP_JSONGeneratePathBufferRecursivelyFromToken_r(parse_state, (P_BP_JSON_PARSE_TOKEN_ELEMENT) token_element->parent_token_element, path_builder_tq))
		{
			BP_DestroyLinkL(path_builder_tq, BP_TRUE);
			return ERR_FAILURE;
		}

	}

	// --- Add Path Values -----------------------

	// if the current item is anonymous, use the anonymous parent string here
	if(token_element->current_token_label_is_anonymous)
	{
		tq_strdup(path_builder_tq, "/", BPLN);
		tq_strdup(path_builder_tq, token_element->current_string_val, BPLN);
	}
	else if(token_element->current_string_val)
	{
		tq_strdup(path_builder_tq, "/", BPLN);
		tq_strdup(path_builder_tq, token_element->current_string_val, BPLN);
	}
	else if(token_element->current_primitive_val)
	{
		tq_strdup(path_builder_tq, "/", BPLN);
		tq_strdup(path_builder_tq, token_element->current_primitive_val, BPLN);
	}

	// flatten the linkl to create single string
	BP_LinkLFlattenJoinStringElements(path_builder_tq);

	// destroy current info if set
	if(token_element->path_query_to_current)
		tq_free(parse_state->tq, token_element->path_query_to_current);

	// duplicate the path to the current item as our path to current element
	token_element->path_query_to_current = (char *) tq_strdup(parse_state->tq, (char *) path_builder_tq->flat_buff, BPLN);

	// destroy the linkl before exiting
	BP_DestroyLinkL(path_builder_tq, BP_TRUE);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Step 2: Generate Full Path to Parent %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// only continue if we have a parent token element to work with (if not we're at top position)
	if(!token_element->parent_token_element)
		return ERR_SUCCESS;

	// create new linkl for parsing parent
	path_builder_tq = BP_CreateLinkL(".", BP_TRUE);

	// cast parent element from token (void *)
	P_BP_JSON_PARSE_TOKEN_ELEMENT parent_element = (P_BP_JSON_PARSE_TOKEN_ELEMENT) token_element->parent_token_element;

	// if we have a parent token, immediately recurse
	if(parent_element->parent_token_element)
	{

		// recurse and attempt to generate token
		if(!BP_JSONGeneratePathBufferRecursivelyFromToken_r(parse_state, (P_BP_JSON_PARSE_TOKEN_ELEMENT) parent_element->parent_token_element, path_builder_tq))
		{
			BP_DestroyLinkL(path_builder_tq, BP_TRUE);
			return ERR_FAILURE;
		}

	}

	// --- Add Path Values -----------------------

	// if the current item is anonymous, use the anonymous parent string here
	if(parent_element->current_token_label_is_anonymous)
	{
		tq_strdup(path_builder_tq, "/", BPLN);
		tq_strdup(path_builder_tq, parent_element->current_string_val, BPLN);
	}
	else if(parent_element->current_string_val)
	{
		tq_strdup(path_builder_tq, "/", BPLN);
		tq_strdup(path_builder_tq, parent_element->current_string_val, BPLN);
	}
	else if(parent_element->current_primitive_val)
	{
		tq_strdup(path_builder_tq, "/", BPLN);
		tq_strdup(path_builder_tq, parent_element->current_primitive_val, BPLN);
	}

	// --- Flatten and Set Values ---------------------------------

	// flatten the linkl to create single string
	BP_LinkLFlattenJoinStringElements(path_builder_tq);

	// destroy parent info if set
	if(token_element->path_query_to_parent)
		tq_free(parse_state->tq, token_element->path_query_to_parent);

	// duplicate the path to the parent item as our path to current element
	token_element->path_query_to_parent = (char *) tq_strdup(parse_state->tq, (char *) path_builder_tq->flat_buff, BPLN);

	// destroy the linkl before exiting
	BP_DestroyLinkL(path_builder_tq, BP_TRUE);


	// return indicating success
	return ERR_SUCCESS;

}


// adds to the specified path
BP_ERROR_T BP_JSONGeneratePathBufferRecursivelyFromToken_r(P_BP_JSON_PARSE_STATE parse_state, P_BP_JSON_PARSE_TOKEN_ELEMENT token_element, P_BP_LINKL path_builder_tq)
{

	// check for our parse state
	if(!parse_state)
		return ERR_FAILURE;

	// ensure we have our token
	if(!token_element)
		return ERR_FAILURE;

	// ensure we have our path in/out pointer
	if(!path_builder_tq)
		return ERR_FAILURE;

	// if we have a parent token, immediately recurse
	if(token_element->parent_token_element)
	{

		// recurse and attempt to generate token
		if(!BP_JSONGeneratePathBufferRecursivelyFromToken_r(parse_state, (P_BP_JSON_PARSE_TOKEN_ELEMENT) token_element->parent_token_element, path_builder_tq))
		{
			BP_DestroyLinkL(path_builder_tq, BP_TRUE);
			return ERR_FAILURE;
		}

	}

	// if the current item is anonymous, use the anonymous parent string here
	if(token_element->current_token_label_is_anonymous)
	{
		tq_strdup(path_builder_tq, "/", BPLN);
		tq_strdup(path_builder_tq, token_element->current_string_val, BPLN);
	}
	else if(token_element->current_string_val)
	{
		tq_strdup(path_builder_tq, "/", BPLN);
		tq_strdup(path_builder_tq, token_element->current_string_val, BPLN);
	}
	else if(token_element->current_primitive_val)
	{
		tq_strdup(path_builder_tq, "/", BPLN);
		tq_strdup(path_builder_tq, token_element->current_primitive_val, BPLN);
	}


	// return indicating success
	return ERR_SUCCESS;

}

// push parse state from current state
BP_ERROR_T BP_JSONPushAdvanceParseState(P_BP_JSON_PARSE_STATE parse_state)
{

	// run basic null checks
	if(!parse_state)
		return ERR_FAILURE;
	if(!parse_state->jsmn_tokens)
		return ERR_FAILURE;
	if(!parse_state->jsmn_tokens_allocated_n)
		return ERR_FAILURE;
	if(!parse_state->parsed_token_count)
		return ERR_FAILURE;

	// ensure we have tokens remaining
	if(!parse_state->parsed_tokens_remaining)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Get Parent Information for Token %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// printf("\n Advancing State");

	// set current parse element as reference
	P_BP_JSON_PARSE_TOKEN_ELEMENT curr_token_parse_element = (P_BP_JSON_PARSE_TOKEN_ELEMENT) &parse_state->token_element_stack[parse_state->current_token_element_index];

	// ensure token is zeroed before use
	memset(curr_token_parse_element, 0x00, sizeof(BP_JSON_PARSE_TOKEN_ELEMENT));

	// set current token
	jsmntok_t *current_token = &parse_state->jsmn_tokens[parse_state->current_token_element_index];
	if(!current_token)
		return ERR_FAILURE;


	// retrieve parent token
	jsmntok_t *parent_token = NULL;

	// if the parent token is a string type, this will hold its value
	char *parent_token_str_val = NULL;

	// if the parent token is a primitive type, this will hold its value
	char *parent_token_primitive_val = NULL;


	// set parent information
	if(parse_state->current_token_element_index != 0 && (current_token->parent >= 0))
	{


		// set parent reference from decrementing index
		parent_token = &parse_state->jsmn_tokens[current_token->parent];

		// set last token parent reference
		parse_state->last_token_parent = parent_token;

		// set parent pointer here (due to cascading a child should never have a token that does not belong to them)
		curr_token_parse_element->parent_token_element = (void *) &parse_state->token_element_stack[current_token->parent];

		// internal switch on parent type
		switch(parent_token->type)
		{

			case JSMN_OBJECT:

				// mark parent token type as object (no data extraction)
				curr_token_parse_element->parent_is_object = BP_TRUE;

				// printf("\n\t [dbg] PARENT IS_OBJECT");
				break;

			case JSMN_ARRAY:

				// mark parent token type as array (no data extraction
				curr_token_parse_element->parent_is_array = BP_TRUE;

				// printf("\n\t [dbg] PARENT IS ARRAY");
				break;

			case JSMN_STRING:

				// mark parent token type as string
				curr_token_parse_element->parent_is_string = BP_TRUE;

				// extract data
				parent_token_str_val = BP_JSONGetStringValueFromJSMNToken(parse_state, parent_token);

				// printf("\n\t [dbg] PARENT STRING VALUE: %s", parent_token_str_val);
				break;

			case JSMN_PRIMITIVE:

				// mark token type as primitive
				curr_token_parse_element->parent_is_primitive = BP_TRUE;

				// extract data
				parent_token_primitive_val = BP_JSONGetPrimitiveValueAsStringValueFromJSMNToken(parse_state, parent_token);

				// printf("\n\t [dbg] PARENT IS PRIMITIVE: %s", parent_token_str_val);
				break;

			default:
				break;

		}


	}

	// this will contain a string value if the object was a string
	char *str_val       = NULL;

	// this will also contain a string value if the object was a primitive
	char *primitive_val = NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Detect Child Element Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// set child tokens
	jsmntok_t *child_tokens = NULL;

	// set child token iterator
	size_t child_token_iter = 0;


	// the "size" element is only set if we have children, and it is always set
	// to the total child element count to follow.
	if(current_token->size > 0)
	{

		// set children info
		curr_token_parse_element->has_children = BP_TRUE;

		// set child count (ensure to cast)
		curr_token_parse_element->child_count = (size_t) current_token->size;

		// set the start position as +1
		curr_token_parse_element->child_tokens_start = &parse_state->jsmn_tokens[parse_state->current_token_element_index+1];

		// set local child tokens reference
		child_tokens = curr_token_parse_element->child_tokens_start;

		// if the child is a string only, mark it
		if(curr_token_parse_element->child_tokens_start)
		{
			if(curr_token_parse_element->child_count == 1 && child_tokens[0].type == JSMN_STRING)
			{

				curr_token_parse_element->child_is_string_only = BP_TRUE;

				// extract the child as a string
				char *tmp_child_as_str = BP_JSONGetStringValueFromJSMNToken(parse_state, &child_tokens[0]);
				if( tmp_child_as_str )
				{

					// set child as string
					curr_token_parse_element->child_as_string = (char *) tq_strdup(parse_state->tq, tmp_child_as_str, BPLN);

					// destroy primitive
					bpfree(tmp_child_as_str);

				}
				else
				{

					// set empty string if necessary
					curr_token_parse_element->child_as_string = (char *) tq_strdup(parse_state->tq, "", BPLN);

				}


			}
		}

		// if the child is a primitive only, mark it
		if(curr_token_parse_element->child_tokens_start)
		{
			if(curr_token_parse_element->child_count == 1 && child_tokens[0].type == JSMN_PRIMITIVE)
			{

				// mark child as primitive
				curr_token_parse_element->child_is_primitive_only = BP_TRUE;

				// gather string
				char *tmp_child_as_primitive =  BP_JSONGetPrimitiveValueAsStringValueFromJSMNToken(parse_state, &child_tokens[0]);

				// set child as primitive
				curr_token_parse_element->child_as_primitive = (char *) tq_strdup(parse_state->tq, tmp_child_as_primitive, BPLN);

				// destroy primitive
				bpfree(tmp_child_as_primitive);

			}
		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Detect/Set Information about Current Object %%%%%%%%%%%%%%%%%%%%%%%%%%

	// switch on the token type
	switch(current_token->type)
	{

		case JSMN_OBJECT:
			curr_token_parse_element->is_object = BP_TRUE;
			// printf("\n\t [dbg] CURRENT IS JSMN OBJECT");
			break;

		case JSMN_ARRAY:
			curr_token_parse_element->is_array = BP_TRUE;
			// printf("\n\t [dbg] CURRENT IS JSMN ARRAY");
			break;

		case JSMN_STRING:
			curr_token_parse_element->is_string = BP_TRUE;
			str_val = BP_JSONGetStringValueFromJSMNToken(parse_state, current_token);
			// printf("\n\t [dbg] CURRENT_IS_STRING: %s", str_val);
			break;

		case JSMN_PRIMITIVE:
			curr_token_parse_element->is_primitive = BP_TRUE;
			primitive_val = BP_JSONGetPrimitiveValueAsStringValueFromJSMNToken(parse_state, current_token);
			// printf("\n\t [dbg] CURRENT IS PRIMITIVE: %s", parent_token_str_val);
			break;

		default:
			break;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Extra Token Data Locally %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// set parent token
	curr_token_parse_element->parent_token = parent_token;

	// set current token
	curr_token_parse_element->current_token = current_token;

	// if we have a string value, set it here
	if(str_val)
	{
		curr_token_parse_element->current_string_val = tq_strdup(parse_state->tq, str_val, BPLN);
	}

	// if we have a primitive value, set it here
	if(primitive_val)
	{
		curr_token_parse_element->current_primitive_val = tq_strdup(parse_state->tq, primitive_val, BPLN);
	}

	// check if we have a parent token
	if(parent_token)
	{

		// mark this token as having a parent
		curr_token_parse_element->has_parent = BP_TRUE;

		// if the parent token has a string value, it's not anonymous
		if(parent_token_str_val)
		{

			// mark anon as false
			curr_token_parse_element->parent_is_anonymous = BP_FALSE;

			// set string value
			curr_token_parse_element->parent_string_val = (char *) tq_strdup(parse_state->tq, parent_token_str_val, BPLN);

			// set parent is string
			if(parent_token->type == JSMN_STRING)
				curr_token_parse_element->parent_is_string = BP_TRUE;


		}
		else if(parent_token_primitive_val)
		{

			// mark anon as false
			curr_token_parse_element->parent_is_anonymous = BP_FALSE;

			// set string value
			curr_token_parse_element->parent_primitive_val = (char *) tq_strdup(parse_state->tq, parent_token_primitive_val, BPLN);

			// set parent is string
			if(parent_token->type == JSMN_PRIMITIVE)
				curr_token_parse_element->parent_is_primitive = BP_TRUE;

		}
		else
		{

			// mark anon as false
			if(curr_token_parse_element->parent_token_element)
			{

				// set tmp parent reference to perform check
				P_BP_JSON_PARSE_TOKEN_ELEMENT tmp_parent = (P_BP_JSON_PARSE_TOKEN_ELEMENT) curr_token_parse_element->parent_token_element;

				// mark parent as anonymous if it's anonymous
				if(tmp_parent->current_token_label_is_anonymous)
					curr_token_parse_element->parent_is_anonymous = BP_TRUE;

			}

		}

	}



	// if we have no string value, and no primitive value, mark token as anonymous and assign string value
	// to random guid.
	if(!str_val && !primitive_val)
	{

		// run checks for anonymous objects
		if(parent_token)
		{

			// if the parent is an array or an object, we are looking at an anonymous json object
			if(parent_token->type == JSMN_ARRAY || parent_token->type == JSMN_OBJECT)
			{

				// create some buffer on the stack to allow for uuid to generate
				char uuid_tmp[BP_UUID_LEN+30];

				// zero out the memory
				BP_ZERO_STACK(uuid_tmp);

				// generate random uuid
				BP_GenRandomUUID_StoreInBuff((char *) &uuid_tmp);

				// copy in tail of string
				memcpy(&uuid_tmp[BP_UUID_LEN], "_anonymous_json_object", bpstrlen("_anonymous_json_object"));


				// set anonymous label indicator
				curr_token_parse_element->current_token_label_is_anonymous = BP_TRUE;

				// set string value
				curr_token_parse_element->current_string_val = (char *) tq_strdup(parse_state->tq, (char *) &uuid_tmp, BPLN);

			}

		}
		else
		{

			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% NO PARENT/TOP LEVEL OBJECT ASSIGNMENT %%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// create some buffer on the stack to allow for uuid to generate
			char uuid_tmp[BP_UUID_LEN+30];

			// zero out the memory
			BP_ZERO_STACK(uuid_tmp);

			// generate random uuid
			BP_GenRandomUUID_StoreInBuff((char *) &uuid_tmp);

			// copy in tail of string
			memcpy(&uuid_tmp[BP_UUID_LEN], "_anonymous_json_object", bpstrlen("_anonymous_json_object"));


			// set anonymous label indicator
			curr_token_parse_element->current_token_label_is_anonymous = BP_TRUE;

			// set string value
			curr_token_parse_element->current_string_val = (char *) tq_strdup(parse_state->tq, (char *) &uuid_tmp, BPLN);

		}

	}


	// set parent token from reference
	P_BP_JSON_PARSE_TOKEN_ELEMENT parent_token_element = NULL;
	if(curr_token_parse_element->parent_token_element)
	{

		// set parent token element
		parent_token_element = (P_BP_JSON_PARSE_TOKEN_ELEMENT) curr_token_parse_element->parent_token_element;

	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %% Generate Path Buffer for Token Element %%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to generate the path buffer for the specific token
	BP_JSONGeneratePathBufferRecursivelyFromToken(parse_state, curr_token_parse_element);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Global Parse Flags Based on Parse State %%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	if(parent_token_element)
	{

		// if the parent is an array or an object, check for anonymous children
		if(parent_token_element->is_array || parent_token_element->is_object)
		{

			// if the current is an array or an object
			if(curr_token_parse_element->is_array || curr_token_parse_element->is_object)
			{

				// if the current element is anonymous
				if(curr_token_parse_element->current_token_label_is_anonymous == BP_TRUE)
				{

					// if current parse element is array, we can mark anon array flag, if it's
					// an object, we can mark it an anon object.
					if(curr_token_parse_element->is_array == BP_TRUE)
					{
						parse_state->parse_encountered_anonymous_arrays = BP_TRUE;
					}
					else if(curr_token_parse_element->is_object == BP_TRUE)
					{
						parse_state->parse_encountered_anonymous_objects = BP_TRUE;
					}

					// mark flag indicating we've encountered anon arrays or objects
					parse_state->parse_encountered_anonymous_arrays_or_objects = BP_TRUE;

				}

			}

		}

	}


	// check to see if both current and parent are anonymous
	if(parent_token_element)
	{
		if(parent_token_element->current_token_label_is_anonymous)
		{


			// if the parent is anonymous and the element has no children, we have an
			// anonymous string or primitive.
			switch(curr_token_parse_element->current_token->type)
			{
				case JSMN_STRING:
					if(!curr_token_parse_element->child_tokens_start)
						parse_state->parse_encountered_string_without_key = BP_TRUE;
					break;

				case JSMN_PRIMITIVE:
					if(!curr_token_parse_element->child_tokens_start)
						parse_state->parse_encountered_primitive_without_key = BP_TRUE;
					break;

				default:
					break;

			}


			// set double marked token here
			if(parse_state->parse_encountered_primitive_without_key || parse_state->parse_encountered_string_without_key)
				parse_state->parse_encountered_string_or_privitive_without_key = BP_TRUE;

		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Clean Potentially Dangling Tokens %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// clean current value extractions
	if(str_val)
		bpfree(str_val);
	if(primitive_val)
		bpfree(primitive_val);

	// clean parent value extractions
	if(parent_token_str_val)
		bpfree(parent_token_str_val);
	if(parent_token_primitive_val)
		bpfree(parent_token_primitive_val);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Optional Dbg/Display %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// display parse element
	// BP_JSONDisplayParseElement(parse_state, curr_token_parse_element, BP_TRUE);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Move Iterators As Necessary %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set current token reference
	parse_state->last_token = current_token;

	// increment the element index depth
	parse_state->current_token_element_index++;

	// decrement the remaining count
	parse_state->parsed_tokens_remaining--;

	// ensure the token was initialized ok
	curr_token_parse_element->token_init_ok = BP_TRUE;


	// return indicating success
	return ERR_SUCCESS;

}






// Attempt to parse JSON from buffer, storing it in the "in" registry of the P_BP_JSON session.
BP_ERROR_T BP_JSONParseFromBuffer
(
		P_BP_JSON json,
		char * buffer,
		size_t buffer_size,
		size_t max_depth_for_token_allocation,
		size_t max_tokens_total
)
{

	// ensure we have a json object
	if(!json)
		return ERR_FAILURE;

	// destroy dangling parse messages if any are set
	if(json->parse_messages_reg)
	{
		BP_DestroyHashTableRegistry(json->parse_messages_reg);
		json->parse_messages_reg = NULL;
	}

	// ensure we have a buffer
	if(!buffer)
	{
		BP_JSONParseAddErrorMsg(json, NULL, "Provided JSON buffer was NULL");
		return ERR_FAILURE;
	}

	// ensure we have a buffer size
	if(!buffer_size)
	{
		BP_JSONParseAddErrorMsg(json, NULL, "Provided JSON buffer had a zero length");
		return ERR_FAILURE;
	}

	// ensure we have a max depth set
	if(!max_depth_for_token_allocation)
	{
		BP_JSONParseAddErrorMsg(json, NULL, "No max depth for token allocation provided to JSON parsing routine.  This is a developer error either in a module or the core system.");
		return ERR_FAILURE;
	}

	// ensure we have a max alloc set
	if(!max_tokens_total)
	{
		BP_JSONParseAddErrorMsg(json, NULL, "No max total tokens have been provided to the JSON parsing routine.  This means there is no cue to trigger allocation of memory.  This is a developer error either in a module or the core system.");
		return ERR_FAILURE;
	}

	// ensure string is reasonable printable string
	if(!BP_StringIsReasonablePrintableString(buffer, buffer_size, BP_TRUE, BP_TRUE))
	{
		BP_JSONParseAddErrorMsg(json, NULL, "Provided JSON buffer was not reasonable/printable.  It contains non-printable/unreasonable characters.  Please use ASCII only.");
		return ERR_FAILURE;
	}

	// dbg message/optional
	// printf("\n [dbg] JSON Parser Passes Initial Checks");

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Perform Cleanup in Case of Misuse %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// destroy existing parse state if one already exists
	if(json->parse_state)
	{
		BP_JSONDestroyParseState(json->parse_state);
		json->parse_state = NULL;
	}

	// destroy json output buffer upon parse attempt
	if(json->json_out)
	{
		ht_free(json->system_hreg, json->json_out);
		json->json_out = NULL;
	}

	// destroy the output hreg
	if(json->json_out_hreg)
	{
		BP_DestroyHashTableRegistry(json->json_out_hreg);
		json->json_out_hreg = NULL;
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Step 1: Parse via libjsmn %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create parser state information structure
	P_BP_JSON_PARSE_STATE parse_state = BP_JSONCreateInitialParseState
	(
			json,
			buffer,
			buffer_size,
			max_depth_for_token_allocation,
			max_tokens_total
	);

	// ensure we could create our initial parse state
	if(!parse_state)
	{
		BP_JSONParseAddErrorMsg(json, NULL, "Creating initial JSON parse state failed.");
		return ERR_FAILURE;
	}

	// you can set this to any nonzero number and debug the loop below if
	// necessary.  Set it to -1 to disable this functionality.
	int debug_exit_after_preset_iter_count = -1;

	// display the current parse state
	// printf("\n [+] Displaying Parse State at Start of Advance Loop");
	// BP_JSONDisplayParseState(parse_state);

	// Advancing the parse state adds information for all tokens, walking
	// each token and creating required state/path information.
	while(BP_JSONPushAdvanceParseState(parse_state) == ERR_SUCCESS)
	{

		// run debug loop exit checks
		if(debug_exit_after_preset_iter_count != -1)
		{



			// break the loop if we've hit our trigger
			if(debug_exit_after_preset_iter_count == 0)
				break;

			// decrement the iterator count
			debug_exit_after_preset_iter_count--;
		}

	}

	// display parse state path query list
	// printf("\n [+] Full path query list.");
	// BP_JSONDisplayParseStatePathsQueriesOnly(parse_state);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Step 2: Translate Parsed Token Elements into HREG %%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if the parse state indicates we have no tokens, the parse was bad
	if(!parse_state->token_element_stack)
	{
		BP_JSONDestroyParseState(parse_state);
		return ERR_FAILURE;
	}

	// if we have no elements allocated, exit
	if(!parse_state->token_element_stack_items_allocated_n)
	{
		BP_JSONDestroyParseState(parse_state);
		return ERR_FAILURE;
	}

	// set element iter to zero
	size_t element_iter = 0;

	// set current element
	P_BP_JSON_PARSE_TOKEN_ELEMENT curr_element = NULL;

	// set parent element
	P_BP_JSON_PARSE_TOKEN_ELEMENT parent_element = NULL;



	// top level is always going to be the current parsed element index
	for(; element_iter < parse_state->current_token_element_index; element_iter++)
	{

		// set current JSON token element
		curr_element = &parse_state->token_element_stack[element_iter];

		// ensure we have a pointer
		if(!curr_element)
			break;

		// get parent element
		parent_element = (P_BP_JSON_PARSE_TOKEN_ELEMENT) curr_element->parent_token_element;


		// display path query to current
		// printf("\n [PARENT] ADD LOOP HERE: %s", curr_element->path_query_to_parent);
		// printf("\n [CHILD]  ADD LOOP HERE: %s", curr_element->path_query_to_current);

		// This will hold our hreg entry.  The registry entries external data pointer will be
		// used to link the current_hreg_entry (later retrieved via lookup), with a parse element.
		P_BP_HASH_TABLE_KEYED_ENTRY curr_hreg_entry = NULL;


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Run Duplicate Key Check %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// if we have a path, check to ensure it doesn't already exist
		if(curr_element->path_query_to_current)
		{

			// check to see if the object already exists
			if(curr_element->current_token->type == JSMN_OBJECT || curr_element->current_token->type == JSMN_ARRAY )
			{
				curr_hreg_entry = BP_HashRegLookupTableByPathQuery(json->json_out_hreg, curr_element->path_query_to_current);
			}
			else
				curr_hreg_entry = BP_HashRegLookupTableEntryByPathQuery(json->json_out_hreg, curr_element->path_query_to_current);

			// if the hreg exists, check it's parents and set markers
			if(curr_hreg_entry)
			{

				if(parent_element)
				{
					if(parent_element->current_token)
					{
						if(parent_element->current_token->type == JSMN_OBJECT)
							parse_state->parse_encountered_duplicate_keys_in_the_same_object = BP_TRUE;
						if(parent_element->current_token->type == JSMN_ARRAY)
							parse_state->parse_encountered_duplicate_keys_in_the_same_array = BP_TRUE;
					}
				}

				// set duplicate keys marker
				parse_state->parse_encountered_duplicate_keys_in_array_or_object = BP_TRUE;

			}

		}


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Attempt Additions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// check the current token type
		switch(curr_element->current_token->type)
		{

			// add table if it's an object
			case JSMN_OBJECT:

				BP_JSONAddTableByPathQuery(json, curr_element->path_query_to_current);
				curr_hreg_entry = BP_HashRegLookupTableByPathQuery(json->json_out_hreg, curr_element->path_query_to_current);
				break;

			// add array if it's an array
			case JSMN_ARRAY:
				BP_JSONAddTableByPathQuery(json, curr_element->path_query_to_current);
				curr_hreg_entry = BP_HashRegLookupTableByPathQuery(json->json_out_hreg, curr_element->path_query_to_current);
				break;

			// jsmn string
			case JSMN_STRING:

				// BP_JSONAddNodeByPathQuery(json, curr_element->path_query_to_current, curr_element->);
				if(curr_element->child_as_string && curr_element->child_is_string_only)
				{

					// printf("\n Attempting to add child as string only?: %s - %s", curr_element->child_as_string, curr_element->path_query_to_current);
					// if the child is a string, just add the token to the current table
					BP_JSONAddNodeByPathQuery(json, curr_element->path_query_to_current, curr_element->child_as_string, bpstrlen(curr_element->child_as_string), BP_FALSE);
					curr_hreg_entry = BP_HashRegLookupTableEntryByPathQuery(json->json_out_hreg, curr_element->path_query_to_current);

					// printf("\n String hreg entry1?: %p - %s", curr_hreg_entry, curr_element->child_as_string);
				}
				else if(curr_element->child_as_primitive && curr_element->child_is_primitive_only)
				{

					// printf("\n Attempting to add child as primitive only?: %s - %s", curr_element->child_as_primitive, curr_element->path_query_to_current);
					// if the child is a string, just add the token to the current table
					BP_JSONAddNodeByPathQuery(json, curr_element->path_query_to_current, curr_element->child_as_primitive, bpstrlen(curr_element->child_as_primitive), BP_FALSE);
					curr_hreg_entry = BP_HashRegLookupTableEntryByPathQuery(json->json_out_hreg, curr_element->path_query_to_current);
					// printf("\n String hreg entry2?: %p", curr_hreg_entry);

				}
				else if(!curr_element->child_tokens_start && !curr_element->parent_is_string && !curr_element->parent_is_primitive)
				{

					// add self reference if there are no children to this comment
					// printf("\n TRYING TO ADD BY PARENT HERE(STR): %s", curr_element->path_query_to_current);
					BP_JSONAddNodeByPathQuery(json, curr_element->path_query_to_current, curr_element->current_string_val, bpstrlen(curr_element->current_string_val), BP_FALSE);
					curr_hreg_entry = BP_HashRegLookupTableEntryByPathQuery(json->json_out_hreg, curr_element->path_query_to_current);

					printf("\n String hreg entry3?: %p - %s", curr_hreg_entry, curr_element->current_string_val);
					printf("\n Current Path: %s", curr_element->path_query_to_current);

				}

				break;

			case JSMN_PRIMITIVE:

				// BP_JSONAddNodeByPathQuery(json, curr_element->path_query_to_current, curr_element->);
				if(curr_element->child_as_primitive  && curr_element->child_is_primitive_only)
				{

					// if the child is a string, just add the token to the current table
					BP_JSONAddNodeByPathQuery(json, curr_element->path_query_to_current, curr_element->child_as_primitive, bpstrlen(curr_element->child_as_primitive), BP_FALSE);
					curr_hreg_entry = BP_HashRegLookupTableEntryByPathQuery(json->json_out_hreg, curr_element->path_query_to_current);

					// printf("\n Primitive hreg entry1?: %p", curr_hreg_entry);
				}
				else if(curr_element->child_as_string && curr_element->child_is_string_only)
				{

					// if the child is a string, just add the token to the current table
					BP_JSONAddNodeByPathQuery(json, curr_element->path_query_to_current, curr_element->child_as_string, bpstrlen(curr_element->child_as_string), BP_FALSE);
					curr_hreg_entry = BP_HashRegLookupTableEntryByPathQuery(json->json_out_hreg, curr_element->path_query_to_current);

					// printf("\n Primitive hreg entry2?: %p", curr_hreg_entry);
				}
				else if(!curr_element->child_tokens_start && !curr_element->parent_is_string && !curr_element->parent_is_primitive)
				{

					// add self reference if there are no children to this comment
					// printf("\n TRYING TO ADD BY PARENT HERE: %s", curr_element->path_query_to_current);
					BP_JSONAddNodeByPathQuery(json, curr_element->path_query_to_current, curr_element->current_primitive_val, bpstrlen(curr_element->current_primitive_val), BP_FALSE);
					curr_hreg_entry = BP_HashRegLookupTableEntryByPathQuery(json->json_out_hreg, curr_element->path_query_to_current);

					// printf("\n Primitive hreg entry3?: %p", curr_hreg_entry);
				}
				break;

			default:
				break;
		}


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Examine Current Node and Add Data Element Reference %%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// if we were able to look up our new entry in the tree, perform necessary
		// data associations
		if(curr_hreg_entry)
		{

			// printf("\n [dbg] Setting metadata ok!");
			// set the current element (pointer into array) as the element metadata
			curr_hreg_entry->external_entry_metadata = (void *) curr_element;

		}

	}

	// set the parse state within the json object here
	json->parse_state = parse_state;

	// return indicating success
	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Parse From File Contents %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Reads a file and attempts to parse it as json.
BP_ERROR_T BP_JSONParseFromFile
(
		P_BP_JSON json,
		char *path_to_file,
		size_t max_depth_for_token_allocation,
		size_t max_tokens_total
)
{


	if(!json)
		return ERR_FAILURE;
	if(!path_to_file)
		return ERR_FAILURE;
	if(!BP_StringIsReasonableForUnixEXT4Filename(path_to_file, bpstrlen(path_to_file), BP_TRUE))
		return ERR_FAILURE;

	// create load info and zero it
	BP_FILE_LOAD_EXTRA_INFO load_info;
	BP_ZERO_STACK(load_info);

	// retrieve contents
	char *buff_contents = BP_FileGetContents(path_to_file, &load_info);

	// ensure we could read the contents
	if(!buff_contents)
		return ERR_FAILURE;

	// attempt to parse JSON from the provided buffer
	if(!BP_JSONParseFromBuffer(json, buff_contents, load_info.file_size, 40, 500))
	{
		bpfree(buff_contents);
		return ERR_FAILURE;
	}

	// destroy the buffer contents
	bpfree(buff_contents);

	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create Hreg Append Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to parse output hreg and create json.  If the from_hreg variable is set, the
// json generator use that registry instead of the internal builtin registry.
BP_ERROR_T BP_JSONAppendHReg
(
		P_BP_JSON                 json,
		char                    * hreg_json_name,
		P_BP_HASH_TABLE_REGISTRY  hreg,
		BP_BOOL                   include_data,
		BP_BOOL                   base64_encode_bin_data
)
{


	// ensure we have output and a registry to create from
	if(!json)
		return ERR_FAILURE;

	// ensure we have a hash registry to append
	if(!hreg)
		return ERR_FAILURE;

	// check json id name
	if(!hreg_json_name)
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(hreg_json_name, bpstrlen(hreg_json_name), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// ensure we have a hive and that our type is a table
	if(!hreg->hive)
		return ERR_FAILURE;
	if(hreg->hive->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Top Level Node Table %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create node add path
	char *node_add_path[10];
	BP_ZERO_STACK(node_add_path);

	// set node add path
	node_add_path[0] = hreg_json_name;

	// attempt to add to json
	BP_JSONAddNodeByPath
	(
			json,
			(char **) &node_add_path,
			NULL,
			0,
			BP_FALSE
	);

	// run append from add path; this creates a recursing object
	// starting at our initial add path above, cascading into
	// the hive until all entries are appended.
	BP_JSONAppendHReg_r
	(
			json,
			hreg->hive,
			(char **) &node_add_path,
			include_data,
			base64_encode_bin_data
	);


	// increase the number of registries appended
	json->number_of_registries_appended++;

	// return indicating success
	return ERR_SUCCESS;

}

// Attempts to append a hreg to json at a path
BP_ERROR_T BP_JSONAppendHRegByTablePath
(
		P_BP_JSON                 json,
		P_BP_HASH_TABLE_REGISTRY  hreg,
		char                     *new_appended_table_name,
		char                    **initial_append_path,
		BP_BOOL                   include_data,
		BP_BOOL                   base64_encode_bin_data
)
{

	// run basic checks
	if(!json)
		return ERR_FAILURE;
	if(!hreg)
		return ERR_FAILURE;
	if(!initial_append_path)
		return ERR_FAILURE;
	if(!json)
		return ERR_FAILURE;
	if(!hreg)
		return ERR_FAILURE;
	if(!hreg->hive)
		return ERR_FAILURE;
	if(!hreg->hive->key)
		return ERR_FAILURE;
	if(!initial_append_path)
		return ERR_FAILURE;
	if(!new_appended_table_name)
		return ERR_FAILURE;
	if(!BP_StringIsReasonableForJSONPropertyNameString(new_appended_table_name, bpstrlen(new_appended_table_name)))
		return ERR_FAILURE;


	// -- ensure initial path exists ---

	// attempt to lookup the entry within our json (ensure it doesn't exist)
	P_BP_HASH_TABLE_KEYED_ENTRY hreg_entry = BP_JSONLookupEntryByPath(json, initial_append_path);
	if(!hreg_entry)
	{
		printf("\n [!!] JSON parent path entry not found.");
		return ERR_FAILURE;
	}

	// --- ensure combined path does NOT exist -------

	// key operation
	char *tmp_add_path[] =
	{
		new_appended_table_name,
		NULL
	};
	printf("\n Appending HREG NAME?: %s", new_appended_table_name);

	// attempt to combine string arrays
	char **combined_path = BP_CombineStringArray(initial_append_path, (char **) &tmp_add_path);
	if(!combined_path)
		return ERR_FAILURE;

	// ensure combined path entry doesn't already exist
	if(BP_JSONLookupEntryByPath(json, combined_path))
	{
		printf("\n [!!] JSON parent path entry not found.");
		BP_DestroyPointerArrayNullTerminated((void **) combined_path);
		bpfree(combined_path);
		return ERR_FAILURE;
	}

	// lookup parent table entry
	char ** parent_table_entry = BP_JSONEntryToPath(hreg_entry);
	if(!parent_table_entry)
	{
		BP_DestroyPointerArrayNullTerminated((void **) combined_path);
		bpfree(combined_path);
		return ERR_FAILURE;
	}

	// attempt to lookup the entry within our json (ensure it does exist after creation)
	hreg_entry = BP_JSONLookupEntryByPath(json, initial_append_path);

	// attempt to add registry
	BP_ERROR_T append_ret = BP_JSONAppendHReg_r
	(
		json,
		(P_BP_HTKE) hreg->hive,
		(char **) combined_path,
		include_data,
		base64_encode_bin_data
	);

	// ensure we have successfully appended our json
	if(!append_ret)
	{
		BP_DestroyPointerArrayNullTerminated((void **) combined_path);
		bpfree(combined_path);
		return ERR_FAILURE;
	}

	// destroy allocated memory before exit
	BP_DestroyPointerArrayNullTerminated((void **) combined_path);
	bpfree(combined_path);

	BP_DestroyPointerArrayNullTerminated((void **) parent_table_entry);
	bpfree(parent_table_entry);

	// return indicating success
	return ERR_SUCCESS;

}

// Attempts to append a hreg to json at a path, using /path/query/lookup.
BP_ERROR_T BP_JSONAppendHRegByTablePathQuery
(
		P_BP_JSON                 json,
		P_BP_HASH_TABLE_REGISTRY  hreg,
		char                     *new_appended_table_name,
		char                     *initial_append_path_query,
		BP_BOOL                   include_data,
		BP_BOOL                   base64_encode_bin_data
)
{

	// since this function only proxies off to
	if(!initial_append_path_query)
		return ERR_FAILURE;

	// attempt to lookup the entry within our json (ensure it doesn't exist)
	P_BP_HASH_TABLE_KEYED_ENTRY hreg_entry = BP_JSONLookupEntryByPathQuery(json, initial_append_path_query);
	if(!hreg_entry)
	{
		printf("\n [!!] JSON parent path entry not found.");
		return ERR_FAILURE;
	}

	// create path
	char **add_path = BP_HashRegEntryToPath(hreg_entry);

	// return the state of the hreg append attempt
	BP_ERROR_T ret_val = BP_JSONAppendHRegByTablePath
	(
			json,
			hreg,
			new_appended_table_name,
			add_path,
			include_data,
			base64_encode_bin_data
	);

	// ensure we have an add path
	if(add_path)
	{
		BP_DestroyPointerArrayNullTerminated((void **) add_path);
		bpfree(add_path);
	}

	// return indicating success
	return ret_val;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% JSON Append Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to parse output hreg and create json.  If the from_hreg variable is set, the
// json generator use that registry instead of the internal builtin registry.
BP_ERROR_T BP_JSONAppend
(
		P_BP_JSON  json,
		char      *hreg_json_name,
		P_BP_JSON  json_appending,
		BP_BOOL    include_data,
		BP_BOOL    base64_encode_bin_data
)
{

	// run basic checks
	if(!json_appending)
		return ERR_FAILURE;
	if(!json_appending->json_out_hreg)
		return ERR_FAILURE;
	if(!json_appending->json_out_hreg->total_tables_in_hive)
		return ERR_FAILURE;
	if(!json_appending->json_out_hreg->total_data_entries_in_hive)
		return ERR_FAILURE;

	// run addition
	return BP_JSONAppendHReg
	(
			json,
			hreg_json_name,
			json_appending->json_out_hreg,
			include_data,
			base64_encode_bin_data
	);

}


// Attempts to append a hreg to json at a path
BP_ERROR_T BP_JSONAppendByTablePath
(
		P_BP_JSON  json,
		P_BP_JSON  json_appending,
		char      *new_appended_table_name,
		char     **initial_append_path,
		BP_BOOL    include_data,
		BP_BOOL    base64_encode_bin_data
)
{

	// run basic checks
	if(!json_appending)
		return ERR_FAILURE;
	if(!json_appending->json_out_hreg)
		return ERR_FAILURE;
	if(!json_appending->json_out_hreg->total_tables_in_hive)
		return ERR_FAILURE;

	// note: can have missing data entries
	// if(!json_appending->json_out_hreg->total_data_entries_in_hive)
	// 	return ERR_FAILURE;


	// run addition
	return BP_JSONAppendHRegByTablePath
	(
		json,
		json_appending->json_out_hreg,
		new_appended_table_name,
		(char **) initial_append_path,
		include_data,
		base64_encode_bin_data
	);

}

// Attempts to append a hreg to json at a path, using /path/query/lookup.
BP_ERROR_T BP_JSONAppendByTablePathQuery
(
		P_BP_JSON  json,
		P_BP_JSON  json_appending,
		char      *new_appended_table_name,
		char      *initial_append_path_query,
		BP_BOOL    include_data,
		BP_BOOL    base64_encode_bin_data
)
{

	// run basic checks
	if(!json_appending)
		return ERR_FAILURE;
	if(!json_appending->json_out_hreg)
		return ERR_FAILURE;
	if(!json_appending->json_out_hreg->total_tables_in_hive)
		return ERR_FAILURE;
	if(!json_appending->json_out_hreg->total_data_entries_in_hive)
		return ERR_FAILURE;

	// run addition
	return BP_JSONAppendHRegByTablePathQuery
	(
		json,
		json_appending->json_out_hreg,
		new_appended_table_name,
		initial_append_path_query,
		include_data,
		base64_encode_bin_data
	);

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% JSON Append From Buffer Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// append from json parsable buffer
BP_ERROR_T BP_JSONAppendFromBuffer
(
		P_BP_JSON  json,
		char      *hreg_json_name,
		char      *json_buffer,
		size_t     max_depth_for_token_allocation,
		size_t     max_tokens_total,
		BP_BOOL    include_data,
		BP_BOOL    base64_encode_bin_data
)
{

	// json buffer
	if(!json_buffer)
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(json_buffer, bpstrlen(json_buffer), BP_TRUE, BP_TRUE))
		return ERR_FAILURE;

	// load json from buffer
	P_BP_JSON arbitrary_json = BP_JSONCreateFromBuffer
	(
		json_buffer,
		bpstrlen(json_buffer),
		max_depth_for_token_allocation,
		max_tokens_total
	);

	// ensure json was created ok
	if(!arbitrary_json)
		return ERR_FAILURE;

	// run addition
	BP_ERROR_T ret_val = BP_JSONAppend
	(
			json,
			hreg_json_name,
			arbitrary_json,
			include_data,
			base64_encode_bin_data
	);

	// destroy arbitrary json
	BP_JSONDestroy(arbitrary_json);

	// return the value
	return ret_val;

}


// Append from buffer using table path.
BP_ERROR_T BP_JSONAppendFromBufferByTablePath
(
		P_BP_JSON  json,
		char      *json_buffer,
		size_t     max_depth_for_token_allocation,
		size_t     max_tokens_total,
		char      *new_appended_table_name,
		char     **initial_append_path,
		BP_BOOL    include_data,
		BP_BOOL    base64_encode_bin_data
)
{

	// json buffer
	if(!json_buffer)
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(json_buffer, bpstrlen(json_buffer), BP_TRUE, BP_TRUE))
		return ERR_FAILURE;

	// load json from buffer
	P_BP_JSON arbitrary_json = BP_JSONCreateFromBuffer
	(
		json_buffer,
		bpstrlen(json_buffer),
		max_depth_for_token_allocation,
		max_tokens_total
	);

	// ensure json was created ok
	if(!arbitrary_json)
		return ERR_FAILURE;

	// run addition
	BP_ERROR_T ret_val =  BP_JSONAppendByTablePath
	(
		json,
		arbitrary_json,
		new_appended_table_name,
		initial_append_path,
		include_data,
		base64_encode_bin_data
	);

	// destroy arbitrary json
	BP_JSONDestroy(arbitrary_json);

	// return the value
	return ret_val;

}


// Append from buffer using table path query.
BP_ERROR_T BP_JSONAppendFromBufferByTablePathQuery
(
		P_BP_JSON  json,
		char      *json_buffer,
		size_t     max_depth_for_token_allocation,
		size_t     max_tokens_total,
		char      *new_appended_table_name,
		char      *initial_append_path_query,
		BP_BOOL    include_data,
		BP_BOOL    base64_encode_bin_data
)
{

	// json buffer
	if(!json_buffer)
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(json_buffer, bpstrlen(json_buffer), BP_TRUE, BP_TRUE))
		return ERR_FAILURE;

	// load json from buffer
	P_BP_JSON arbitrary_json = BP_JSONCreateFromBuffer
	(
		json_buffer,
		bpstrlen(json_buffer),
		max_depth_for_token_allocation,
		max_tokens_total
	);

	// ensure json was created ok
	if(!arbitrary_json)
		return ERR_FAILURE;

	// apptend table by path query
	BP_ERROR_T ret_val =  BP_JSONAppendByTablePathQuery
	(
		json,
		arbitrary_json,
		new_appended_table_name,
		initial_append_path_query,
		include_data,
		base64_encode_bin_data
	);


	// destroy arbitrary json
	BP_JSONDestroy(arbitrary_json);

	// return the value
	return ret_val;

}





// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Recursing Append Routine %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Append hash registry recurse.
BP_ERROR_T BP_JSONAppendHReg_r
(
	P_BP_JSON                   json,
	P_BP_HASH_TABLE_KEYED_ENTRY hreg_entry,
	char                      **table_add_start_path,
	BP_BOOL                     include_data,
	BP_BOOL                     base64_encode_bin_data
)
{

	// run basic null checks
	if(!json)
		return ERR_FAILURE;
	if(!hreg_entry)
		return ERR_FAILURE;
	if(!table_add_start_path)
		return ERR_FAILURE;
	if(!table_add_start_path[0])
		return ERR_FAILURE;

	// count add path
	size_t table_depth = BP_CountNullTerminatedPtrArray((char **) table_add_start_path);

	// create new table add path, and zero it out
	char *table_add_path[table_depth+2];
	BP_ZERO_STACK(table_add_path);

	// copy in table path
	memcpy(&table_add_path, table_add_start_path, sizeof(char*) * table_depth);

	// if the entry is an integer/size_t type it will be converted
	size_t entry_as_size_t = 0;
	int    entry_as_int    = 0;


	// iterate through entries
	BP_HASH_ITER_OPEN(hreg_entry, hreg_entry_iter)
	{

		// switch on type
		switch(hreg_entry_iter->type)
		{

			// attempt to add entry
			case BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE:

				// set the path as the key
				table_add_path[table_depth] = hreg_entry_iter->key;

				// attempt to add to json
				BP_JSONAddNodeByPath
				(
						json,
						(char **) &table_add_path,
						NULL,
						0,
						BP_FALSE
				);

				// attempt to recurse here
				BP_JSONAppendHReg_r
				(
						json,
						(P_BP_HASH_TABLE_KEYED_ENTRY) hreg_entry_iter->data,
						table_add_path,
						include_data,
						base64_encode_bin_data
				);

				break;

			// add binary blob
			case BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB:

				// set the path as the key
				table_add_path[table_depth] = hreg_entry_iter->key;

				// attempt to add to json
				BP_JSONAddNodeByPath
				(
						json,
						(char **) &table_add_path,
						include_data ? (char *) hreg_entry_iter->data : NULL,
						include_data ? hreg_entry_iter->data_size : 0,
						base64_encode_bin_data
				);
				break;

			case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_JSON:
			case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING:


				// set the path as the key
				table_add_path[table_depth] = hreg_entry_iter->key;

				// attempt to add to json
				BP_JSONAddNodeByPath
				(
						json,
						(char **) &table_add_path,
						include_data ? (char *) hreg_entry_iter->data : NULL,
						include_data ? hreg_entry_iter->data_size : 0,
						base64_encode_bin_data
				);

				break;


			// unsigned integer type
			case BP_HASH_TABLE_KEYED_ENTRY_TYPE_SIGNED_INT:
				printf("\n Dbvalue was signed int");
				break;

			default:
				printf("\n Dbvalue was unknown");
				break;

		}


	} BP_HASH_ITER_CLOSE;

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Lookup Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// lookup a json table entry by path query
P_BP_HASH_TABLE_KEYED_ENTRY BP_JSONLookupEntryByPath(P_BP_JSON json, char **path)
{

	// run basic null checks
	if(!json)
		return NULL;
	if(!json->json_out_hreg)
		return NULL;
	if(!path)
		return NULL;

	// attempt to look up an entry
	P_BP_HASH_TABLE_KEYED_ENTRY looked_up_entry = BP_HashRegLookupTableEntryByPath
	(
		json->json_out_hreg,
		path
	);

	// if the entry failed, the entry might be a table, attempt to resolve it as one
	if(!looked_up_entry)
	{

		// resolve by table now
		looked_up_entry = BP_HashRegLookupTableByPath
		(
			json->json_out_hreg,
			path
		);

	}

	// return the looked up entry (or null if lookup fails)
	return looked_up_entry;

}


// lookup a json table entry by path query
P_BP_HASH_TABLE_KEYED_ENTRY BP_JSONLookupEntryByPathQuery(P_BP_JSON json, char *entry_path_lookup_query)
{

	// run basic null checks
	if(!json)
		return NULL;
	if(!json->json_out_hreg)
		return NULL;
	if(!entry_path_lookup_query)
		return NULL;

	// attempt to look up an entry
	P_BP_HASH_TABLE_KEYED_ENTRY looked_up_entry = BP_HashRegLookupTableEntryByPathQuery
	(
		json->json_out_hreg,
		entry_path_lookup_query
	);

	// if the entry failed, the entry might be a table, attempt to resolve it as one
	if(!looked_up_entry)
	{

		// resolve by table now
		looked_up_entry = BP_HashRegLookupTableByPathQuery
		(
			json->json_out_hreg,
			entry_path_lookup_query
		);

	}

	// return the looked up entry (or null if lookup fails)
	return looked_up_entry;

}

// lookup entry by table index and table path query
P_BP_HASH_TABLE_KEYED_ENTRY BP_JSONLookupEntryByTableIndexAndTablePathQuery(P_BP_JSON json, char *table_lookup_query, char *table_data_index)
{

	// run basic null checks
	if(!json)
		return NULL;
	if(!json->json_out_hreg)
		return NULL;
	if(!table_lookup_query)
		return NULL;
	if(!table_data_index)
		return NULL;

	// attempt to lookup entry in the hash registry
	P_BP_HASH_TABLE_KEYED_ENTRY looked_up_entry = BP_HashRegLookupTableEntryByDataKeyIndexAndTablePathQuery
	(
		json->json_out_hreg,
		table_lookup_query,
		table_data_index
	);

	// lookup the entry
	if(looked_up_entry)
		return looked_up_entry;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Lookup Table (requires path) %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// determine path array
	char ** path_array = BP_HashRegStringLookupToPathArray(table_lookup_query, bpstrlen(table_lookup_query));
	if(!path_array)
		return NULL;

	// retrieve entry count
	size_t entry_count = BP_CountNullTerminatedPtrArray(path_array);
	if(!entry_count)
		return NULL;

	// zero out the extended path
	char *extended_path[entry_count+10];
	BP_ZERO_STACK(extended_path);

	// iterate through path array and build query
	size_t iter_n = 0;
	for(; iter_n < entry_count; iter_n++)
	{
		extended_path[iter_n] = path_array[iter_n];
	}

	// set index and truncate
	extended_path[iter_n]   = table_data_index;
	extended_path[iter_n+1] = NULL;

	// set the looked up entry from table if we couldn't find an entry
	looked_up_entry = BP_HashRegLookupTableByPath(json->json_out_hreg, (char **) &extended_path);
	if(!looked_up_entry)
		return NULL;

	// return the looked up entry (or null if lookup fails)
	return looked_up_entry;

}


// Attempts to lookup an entry from a table path and entry index (calculated with BP_HASH_ITER_OPEN loop).  If the
// path is set to "/", it will start at the top hive entry of json->json_out.
P_BP_HASH_TABLE_KEYED_ENTRY BP_JSONLookupEntryByTablePathQueryAndEntryIndex(P_BP_JSON json, char *table_lookup_query, size_t entry_index)
{

	if(!json)
		return NULL;
	if(!json->json_out_hreg)
		return NULL;
	if(!json->json_out_hreg->hive)
		return NULL;
	if(!table_lookup_query)
		return NULL;
	if(!BP_StringIsReasonablePrintableString(table_lookup_query, bpstrlen(table_lookup_query), BP_FALSE, BP_FALSE))
		return NULL;

	// set found table to null
	P_BP_HTKE table_found = (P_BP_HTKE) NULL;

	// if our table lookup query is "/", set lookup position as our top table
	if(bpstrlen(table_lookup_query) == 1)
	{
		if(table_lookup_query[0] == '/')
		{
			table_found = (P_BP_HTKE) json->json_out_hreg->hive;
		}
	}
	else
	{

		// if we have a path query, attempt lookup
		table_found = BP_JSONLookupEntryByPathQuery(json, table_lookup_query);

	}

	// exit if we cannot lookup the table
	if(!table_found)
		return NULL;

	// ensure
	if(table_found->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		return NULL;

	// iterating counter
	size_t iter = 0;

	// the found entry (if found)
	P_BP_HTKE entry_found = NULL;

	// iterate through entries and attempt lookups
	BP_HASH_ITER_OPEN(table_found, tmp_json_entry)
	{

		// if we've found our index, assign the found entry and break the loop
		if(iter == entry_index)
		{
			entry_found = tmp_json_entry;
			break;
		}


		// increment the iterator on loop
		iter++;
	}
	BP_HASH_ITER_CLOSE;


	// return the found entry
	return entry_found;

}


// Attempts to lookup the top table entry for the json parsed info
P_BP_HASH_TABLE_KEYED_ENTRY BP_JSONLookupEntryTopTable(P_BP_JSON json)
{

	// run basic null data checks
	if(!json)
		return NULL;
	if(!json->json_out_hreg)
		return NULL;
	if(!json->json_out_hreg->hive)
		return NULL;

	// ensure type is correct
	if(json->json_out_hreg->hive->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		return NULL;

	// return the toplevel table
	return json->json_out_hreg->hive;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Specialized Lookup Routines (lookup-as-type) %%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// access an entry as a string (do not free result)
char * BP_JSONAccessEntryAsReasonableString(P_BP_HASH_TABLE_KEYED_ENTRY entry, BP_BOOL allow_newlines, BP_BOOL allow_carriage_returns)
{

	// ensure we have an entry
	if(!entry)
		return NULL;
	if(!entry->data)
		return NULL;
	if(!entry->data_size)
		return NULL;

	// string must be reasonable
	if(!BP_StringIsReasonablePrintableString( (char *) entry->data, entry->data_size-1, allow_newlines, allow_carriage_returns))
	{
		return NULL;
	}

	// return the data pointer
	return (char *) entry->data;

}



// access an entry as a unsigned long (conversion status is stored in convert_status_in_out)
unsigned long BP_JSONAccessEntryAsUnsignedLong(P_BP_HASH_TABLE_KEYED_ENTRY entry, BP_BOOL * convert_status_in_out)
{

	// ensure we have an entry
	if(!entry)
		return NULL;
	if(!entry->data)
		return NULL;
	if(!entry->data_size)
		return NULL;

	if(!BP_StringIsReasonableForUnsignedIntegerASCII((char *) entry->data, entry->data_size - 1))
	{

		// set the conversion status
		if(convert_status_in_out)
			*convert_status_in_out = BP_FALSE;

		// return zero for failure
		return 0;

	}

	// set errno before conversion call
	errno = 0;
	unsigned long ret_val = strtoul((char *) entry->data, NULL, 10);

	// run errno checks to ensure conversion occured ok
	if(errno == EINVAL || errno == ERANGE)
	{

		// set convert status
		if(convert_status_in_out)
			*convert_status_in_out = BP_FALSE;

		// exit
		return 0;

	}


	// set convert status to success!
	if(convert_status_in_out)
		*convert_status_in_out = BP_TRUE;

	// return the ret val
	return ret_val;

}

// access an entry as long
long BP_JSONAccessEntryAsLong(P_BP_HASH_TABLE_KEYED_ENTRY entry, BP_BOOL * convert_status_in_out)
{


	// ensure we have an entry
	if(!entry)
		return NULL;
	if(!entry->data)
		return NULL;
	if(!entry->data_size)
		return NULL;

	if(!BP_StringIsReasonableForSignedIntegerASCII((char *) entry->data, entry->data_size - 1))
	{

		// set the conversion status
		if(convert_status_in_out)
			*convert_status_in_out = BP_FALSE;

		// return zero for failure
		return 0;

	}

	// set errno before conversion call
	errno = 0;
	long ret_val = strtol((char *) entry->data, NULL, 10);

	// run errno checks to ensure conversion occured ok
	if(errno == EINVAL || errno == ERANGE)
	{

		// set convert status
		if(convert_status_in_out)
			*convert_status_in_out = BP_FALSE;

		// exit
		return 0;

	}


	// set convert status to success!
	if(convert_status_in_out)
		*convert_status_in_out = BP_TRUE;

	// return the ret val
	return ret_val;


}


// --- long long conversions (double length) ---



// access an entry as a unsigned long long (conversion status is stored in convert_status_in_out)
unsigned long long BP_JSONAccessEntryAsUnsignedLongLong(P_BP_HASH_TABLE_KEYED_ENTRY entry, BP_BOOL * convert_status_in_out)
{

	// ensure we have an entry
	if(!entry)
		return NULL;
	if(!entry->data)
		return NULL;
	if(!entry->data_size)
		return NULL;

	if(!BP_StringIsReasonableForUnsignedIntegerASCII((char *) entry->data, entry->data_size - 1))
	{

		// set the conversion status
		if(convert_status_in_out)
			*convert_status_in_out = BP_FALSE;

		// return zero for failure
		return 0;

	}

	// set errno before conversion call
	errno = 0;
	unsigned long long ret_val = strtoull((char *) entry->data, NULL, 10);

	// run errno checks to ensure conversion occured ok
	if(errno == EINVAL || errno == ERANGE)
	{

		// set convert status
		if(convert_status_in_out)
			*convert_status_in_out = BP_FALSE;

		// exit
		return 0;

	}


	// set convert status to success!
	if(convert_status_in_out)
		*convert_status_in_out = BP_TRUE;

	// return the ret val
	return ret_val;

}

// access an entry as long long
long long BP_JSONAccessEntryAsLongLong(P_BP_HASH_TABLE_KEYED_ENTRY entry, BP_BOOL * convert_status_in_out)
{


	// ensure we have an entry
	if(!entry)
		return NULL;
	if(!entry->data)
		return NULL;
	if(!entry->data_size)
		return NULL;

	if(!BP_StringIsReasonableForSignedIntegerASCII((char *) entry->data, entry->data_size - 1))
	{

		// set the conversion status
		if(convert_status_in_out)
			*convert_status_in_out = BP_FALSE;

		// return zero for failure
		return 0;

	}

	// set errno before conversion call
	errno = 0;
	long long ret_val = strtoll((char *) entry->data, NULL, 10);

	// run errno checks to ensure conversion occured ok
	if(errno == EINVAL || errno == ERANGE)
	{

		// set convert status
		if(convert_status_in_out)
			*convert_status_in_out = BP_FALSE;

		// exit
		return 0;

	}


	// set convert status to success!
	if(convert_status_in_out)
		*convert_status_in_out = BP_TRUE;

	// return the ret val
	return ret_val;


}

// --- long quad conversions (quad length) ---


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Keyed Entry To Path Reverse Lookups %%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This is a simple frontend to P_HashRegEntryToPathQuery
char * BP_JSONEntryToPathQuery(P_BP_HASH_TABLE_KEYED_ENTRY entry)
{

	// ensure we have an entry
	if(!entry)
		return NULL;

	// run conversion
	return BP_HashRegEntryToPathQuery(entry);

}

// Attemmpts to convert a hash entry to path.  Result is bp_calloced and must be free'd
// with BP_DestroyPtrArray() and bpfree()
BP_HASH_TABLE_TRAVERSAL_PATH BP_JSONEntryToPath(P_BP_HASH_TABLE_KEYED_ENTRY entry)
{

	// ensure we have an entry
	if(!entry)
		return NULL;

	// run conversion
	return BP_HashRegEntryToPath(entry);

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create JSON Buffer By Crawling Registry %%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// create output from a json
BP_ERROR_T BP_JSONCreateOutputFromPathQuery
(
	P_BP_JSON json,
	char *    lookup_path_query,
	BP_BOOL   base64_encode_bin_data
)
{

	// ensure we have a json structure
	if(!json)
		return ERR_FAILURE;

	// ensure we have a registry to work with
	if(!json->json_out_hreg)
		return ERR_FAILURE;

	// ensure we have hive entries
	if(!json->json_out_hreg->hive)
		return ERR_FAILURE;
	if(!json->json_out_hreg->hive->data)
		return ERR_FAILURE;

	// run table lookup first
	if(!BP_StringIsReasonablePrintableString(lookup_path_query, bpstrlen(lookup_path_query), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// run table lookup
	P_BP_HTKE table_lookup = BP_JSONLookupEntryByPathQuery(json, lookup_path_query);
	if(!table_lookup)
		return ERR_FAILURE;
	if(table_lookup->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		return ERR_FAILURE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Dangling Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure we don't already have a tailqueue to work with
	if(json->json_out_prepare_tq)
	{

		// destroy the linkl if it already exists
		BP_DestroyLinkL(json->json_out_prepare_tq, BP_TRUE);
		json->json_out_prepare_tq = NULL;

	}

	// destroy old json if it exists
	if(json->json_out)
	{

		// destroy old out buffer if it already exists
		ht_free(json->system_hreg, json->json_out);

		// set parameter to null
		json->json_out     = NULL;
		json->json_out_len = 0;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Create JSON Output %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create linkl
	json->json_out_prepare_tq = BP_CreateLinkL("json_output_queue", BP_TRUE);

	// create the output (skip path iter so that we only recurse through a single table)
	BP_ERROR_T creation_ret_val = BP_JSONCreateOutput_r
	(
			json,
			(P_BP_HTKE) table_lookup,
			0,
			json->json_out_prepare_tq,
			base64_encode_bin_data,
			BP_TRUE
	);

	// return indicating success
	return creation_ret_val;

}


// create output from a json
BP_ERROR_T BP_JSONCreateOutput
(
	P_BP_JSON json,
	BP_BOOL   base64_encode_bin_data,
	BP_BOOL   encapsulate_result
)
{


	// ensure we have a json structure
	if(!json)
		return ERR_FAILURE;

	// ensure we have a registry to work with
	if(!json->json_out_hreg)
		return ERR_FAILURE;

	// ensure we have hive entries
	if(!json->json_out_hreg->hive)
		return ERR_FAILURE;
	// if(!json->json_out_hreg->hive->data)
	// 	return ERR_FAILURE;

	// ensure we don't already have a tailqueue to work with
	if(json->json_out_prepare_tq)
	{

		// destroy the linkl if it already exists
		BP_DestroyLinkL(json->json_out_prepare_tq, BP_TRUE);
		json->json_out_prepare_tq = NULL;

	}

	// destroy old json if it exists
	if(json->json_out)
	{

		// destroy old out buffer if it already exists
		ht_free(json->system_hreg, json->json_out);

		// set parameter to null
		json->json_out     = NULL;
		json->json_out_len = 0;

	}

	// create linkl
	json->json_out_prepare_tq = BP_CreateLinkL("json_output_queue", BP_TRUE);

	// set tab depth
	size_t initial_tab_depth = 0;

	// open the table
	if(encapsulate_result == BP_TRUE)
	{
		BP_JSONCreateOutput_AddTableOpen(json->json_out_prepare_tq, 0);
		initial_tab_depth = 1;
	}

	// create the output
	BP_JSONCreateOutput_r
	(
			json,
			json->json_out_hreg->hive,
			initial_tab_depth,
			json->json_out_prepare_tq,
			base64_encode_bin_data
	);

	// close table
	if(encapsulate_result == BP_TRUE)
		BP_JSONCreateOutput_AddTableClose(json->json_out_prepare_tq, 0);

	// return indicating success
	return ERR_SUCCESS;

}


// re-entrant create-output routine (called directly from BP_JSONCreateOutput*)
BP_ERROR_T BP_JSONCreateOutput_r
(
		P_BP_JSON                   json,
		P_BP_HASH_TABLE_KEYED_ENTRY table_r,
		size_t                      tab_depth,
		P_BP_LINKL           	    out_json_tq,
		BP_BOOL                     base64_encode_bin_data,
		BP_BOOL                     skip_outermost_table_iter
)
{

	// run basic null checks
	if(!json)
		return ERR_FAILURE;
	if(!table_r)
		return ERR_FAILURE;
	if(!out_json_tq)
		return ERR_FAILURE;

	// set parse metadata here if it's available (it will be null in all cases except when available)
	P_BP_JSON_PARSE_TOKEN_ELEMENT element_parse_metadata = (P_BP_JSON_PARSE_TOKEN_ELEMENT) table_r->external_entry_metadata;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Process Table Entries (don't skip outermost) %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we are not skipping the outermost table
	if(!skip_outermost_table_iter)
	{

		// start iteration on the table
		BP_HASH_ITER_OPEN((P_BP_HTKE) table_r, table_r_item_iter)
		{

			// if the type is a table, recurse, if not, simply display entry
			if(table_r_item_iter->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
			{

				// add key val
				BP_JSONCreateOutput_AddKeyVal(json->json_out_prepare_tq, tab_depth, table_r_item_iter, base64_encode_bin_data);

				// open the table
				BP_JSONCreateOutput_AddTableOpen(json->json_out_prepare_tq, tab_depth);

				// recurse if we have a hash table type
				BP_JSONCreateOutput_r
				(
					json,
					(P_BP_HTKE) table_r_item_iter->data,
					tab_depth+1,
					out_json_tq,
					base64_encode_bin_data
				);

				// close the table
				BP_JSONCreateOutput_AddTableClose(json->json_out_prepare_tq, tab_depth);

				// add comma only if we have a next element
				if(table_r_item_iter->hh.next)
					tq_strdup(json->json_out_prepare_tq, ",", BPLN);

			}
			else
			{

				// attempt to add key value here
				BP_JSONCreateOutput_AddKeyVal(json->json_out_prepare_tq, tab_depth, table_r_item_iter, base64_encode_bin_data);

				// add comma only if we have a next element
				if(table_r_item_iter->hh.next)
					tq_strdup(json->json_out_prepare_tq, ",", BPLN);

			}

		}
		BP_HASH_ITER_CLOSE;

	}
	else
	{

		// if the type is a table, recurse, if not, simply display entry
		if(table_r->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{

			// add key val
			BP_JSONCreateOutput_AddKeyVal(json->json_out_prepare_tq, tab_depth, table_r, base64_encode_bin_data);

			// open the table
			BP_JSONCreateOutput_AddTableOpen(json->json_out_prepare_tq, tab_depth);

			// recurse if we have a hash table type
			BP_JSONCreateOutput_r
			(
				json,
				(P_BP_HTKE) table_r->data,
				tab_depth+1,
				out_json_tq,
				base64_encode_bin_data
			);

			// close the table
			BP_JSONCreateOutput_AddTableClose(json->json_out_prepare_tq, tab_depth);


		}
		else
		{

			// attempt to add key value here
			BP_JSONCreateOutput_AddKeyVal(json->json_out_prepare_tq, tab_depth, table_r, base64_encode_bin_data);

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// add a keyed value
BP_ERROR_T BP_JSONCreateOutput_AddKeyVal
(
	P_BP_LINKL out_json_tq,
	size_t tab_depth,
	P_BP_HTKE item,
	BP_BOOL base64_encode_bin_data
)
{

	// ensure we have an item
	if(!item)
		return ERR_FAILURE;

	// ensure we have a key (all entries must have keys)
	if(!item->key)
		return ERR_FAILURE;

	// extrapolate metadata if set
	P_BP_JSON_PARSE_TOKEN_ELEMENT element_parse_metadata = (P_BP_JSON_PARSE_TOKEN_ELEMENT) item->external_entry_metadata;



	// this will contain a quoted key if necessary
	char *quoted_key = NULL;

	// this will contain the quoted buffer
	char *quoted_buffer = NULL;



	if(element_parse_metadata)
	{

		if(!element_parse_metadata->current_token_label_is_anonymous)
		{

			// create double quoted key
			quoted_key = BP_DoubleQuoteString(item->key);

			// add new line (only if it's not the first allocation/this prevents a newline from appearing before
			// the first entry)
			if(out_json_tq->total_allocations != 0)
				tq_strdup(out_json_tq, "\n", BPLN);

			// add tabs
			tq_tabs  (out_json_tq, tab_depth, BPLN);

			tq_strdup(out_json_tq, quoted_key, BPLN);

			// add colon if it's a hash table
			if(item->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
				tq_strdup(out_json_tq, ":", BPLN);

		}

	}
	else
	{

		// create double quoted key
		quoted_key = BP_DoubleQuoteString(item->key);

		// Note: Default no-metadata-present behavior is to just display keys as quoted values

		// add new line (only if it's not the first allocation/this prevents a newline from appearing before
		// the first entry)
		if(out_json_tq->total_allocations != 0)
			tq_strdup(out_json_tq, "\n", BPLN);

		// add tabs
		tq_tabs  (out_json_tq, tab_depth, BPLN);

		// add quoted key
		tq_strdup(out_json_tq, quoted_key, BPLN);

		// add colon if it's a hash table
		if(item->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
			tq_strdup(out_json_tq, ":", BPLN);

	}


	// if the entry and the value aren't the same, we need to add a colon
	if(ht_entry_key_and_value_are_the_same(item))
	{
		bpfree(quoted_key);
		return ERR_SUCCESS;
	}


	// base64 encoded data
	char *base64_data = NULL;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Type Specific Output %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// temporary conversion buffer
	char *tmp_conv_buffer = NULL;

	// switch on the type
	switch(item->type)
	{

		// if it's a binary blob, mark it
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB:

			// encode the base64 data
			if(base64_encode_bin_data)
			{
				base64_data = BP_Base64Encode((unsigned char *) item->data, item->data_size);
			}

			// quote the string
			quoted_buffer = BP_DoubleQuoteString(base64_data);

			// add buffer
			tq_strdup(out_json_tq, ":", BPLN);
			tq_strdup(out_json_tq, " ", BPLN);
			tq_strdup(out_json_tq, quoted_buffer, BPLN);

			break;

		// if it's a string type, add
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_JSON:

			if(element_parse_metadata)
			{
				if(element_parse_metadata->child_is_string_only)
				{
					// quote the string
					quoted_buffer = BP_DoubleQuoteString((char *) item->data);

					// add buffer as quoted
					tq_strdup(out_json_tq, ":", BPLN);
					tq_strdup(out_json_tq, " ", BPLN);
					tq_strdup(out_json_tq, quoted_buffer, BPLN);
				}
				else if(element_parse_metadata->child_is_primitive_only)
				{
					// add buffer as direct if it's a primitive
					tq_strdup(out_json_tq, ":", BPLN);
					tq_strdup(out_json_tq, " ", BPLN);
					tq_strdup(out_json_tq, (char *) item->data, BPLN);
				}
				else /* empty string case */
				{

					// add buffer as direct if it's a primitive
					tq_strdup(out_json_tq, ":", BPLN);
					tq_strdup(out_json_tq, " ", BPLN);
					tq_strdup(out_json_tq, "\"\"", BPLN);

				}
			}
			else
			{
				// quote the string
				quoted_buffer = BP_DoubleQuoteString((char *) item->data);

				// add buffer as quoted
				tq_strdup(out_json_tq, ":", BPLN);
				tq_strdup(out_json_tq, " ", BPLN);
				tq_strdup(out_json_tq, quoted_buffer, BPLN);
			}
			break;

		default:
			break;

	}

	// cleanup
	if(quoted_key)
		bpfree(quoted_key);
	if(quoted_buffer)
		bpfree(quoted_buffer);
	if(tmp_conv_buffer)
		bpfree(tmp_conv_buffer);

	// return indicating success
	return ERR_SUCCESS;

}

// open and close tables
BP_ERROR_T BP_JSONCreateOutput_AddTableOpen(P_BP_LINKL out_json_tq, size_t tab_depth)
{

	// ensure we have json
	if(!out_json_tq)
		return ERR_FAILURE;

	// add newline only if we have previous allocations
	if(out_json_tq->total_allocations)
		tq_strdup(out_json_tq, "\n", BPLN);

	// add tabs
	tq_tabs  (out_json_tq, tab_depth, BPLN);

	// add opening parenthesis
	tq_strdup(out_json_tq, "{", BPLN);

	// return indicating success
	return ERR_SUCCESS;

}

// close table
BP_ERROR_T BP_JSONCreateOutput_AddTableClose(P_BP_LINKL out_json_tq, size_t tab_depth)
{

	// ensure we have json
	if(!out_json_tq)
		return ERR_FAILURE;

	// open table
	tq_strdup(out_json_tq, "\n", BPLN);
	tq_tabs  (out_json_tq, tab_depth, BPLN);
	tq_strdup(out_json_tq, "}", BPLN);

	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Finalize Output %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// generate a json buffer from local tq (destroys old if already set)
BP_ERROR_T BP_JSONGenerateJSONBufferFromLocalTQ(P_BP_JSON json)
{

	// if we don't have a json object, exit failure
	if(!json)
		return ERR_FAILURE;

	// ensure we have a prepare tq (should be created via JSONCreateOutput)
	if(!json->json_out_prepare_tq)
		return ERR_FAILURE;


	// flatten string elements
	BP_LinkLFlattenJoinStringElements(json->json_out_prepare_tq);

	// ensure we have a flat buffer
	if(!json->json_out_prepare_tq->flat_buff)
		return ERR_FAILURE;

	// calculate length from buff
	size_t buff_len = bpstrlen((char *) json->json_out_prepare_tq->flat_buff);
	if(!buff_len)
		return ERR_FAILURE;

	// ensure the buffer is reasonable
	if(!BP_StringIsReasonablePrintableString((char *) json->json_out_prepare_tq->flat_buff, buff_len, BP_TRUE, BP_TRUE))
		return ERR_FAILURE;


	// clear old if necessary
	if(json->json_out)
	{

		// destroy old json if it exists
		ht_free(json->system_hreg, json->json_out);

		// set parameter to null
		json->json_out = NULL;
		json->json_out_len = 0;

	}

	// attempt to duplicate the prepared buffer
	json->json_out = ht_strndup
	(
		json->system_hreg,
		(char *) json->json_out_prepare_tq->flat_buff,
		buff_len
	);

	// ensure we could create space for our output buffer
	if(!json->json_out)
		return ERR_FAILURE;

	// set the output length
	json->json_out_len = bpstrlen((char *) json->json_out);

	// return indicating success
	return ERR_SUCCESS;
}


// finalizes json output buffer
BP_ERROR_T BP_JSONFinalizeOutput(P_BP_JSON json, BP_BOOL encapsulate_result)
{

	// if we don't have a json object, exit failure
	if(!json)
		return ERR_FAILURE;

	// destroy output buffers if they exist
	BP_JSONDestroyOutputBuffers(json);


	// attempt to create json output
	if(!BP_JSONCreateOutput(json, BP_FALSE, encapsulate_result))
	{

		// return indicating failure if we can't create output
		return ERR_FAILURE;

	}

	// ensure we can generate buffer from tq
	if(!BP_JSONGenerateJSONBufferFromLocalTQ(json))
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Display JSON information.
BP_ERROR_T BP_JSONDisplay(P_BP_JSON json)
{

	// ensure we have a json structure to work with
	if(!json)
		return ERR_FAILURE;

	// display structrue
	printf("\n [+] JSON %p", json);
	printf("\n   system_hreg:   %p", json->system_hreg);
	printf("\n   json_in_hreg:  %p", json->json_in_hreg);
	printf("\n   json_out_hreg: %p", json->json_out_hreg);
	printf("\n   json_out_len:  %u", json->json_out_len);
	printf("\n   json_out:      %s: ", json->json_out ? "(output starts on next line)" : "NULL");

	// display json if it's set
	if(json->json_out)
	{
		printf("\n%s", json->json_out);
	}

	// display all the parser messages
	BP_JSONDisplayParseMessages(json);


}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% JSON Cloning %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to clone json from a path query
P_BP_JSON BP_JSONCloneFromPathQuery
(
	P_BP_JSON  json_to_clone_from,
	char     * lookup_query,
	BP_BOOL    base64_encode_bin_data,
	size_t max_depth_for_token_allocation,
	size_t max_tokens_total
)
{

	// basic null checks
	if(!json_to_clone_from)
		return NULL;
	if(!lookup_query)
		return NULL;


	// attempt to create output from path query
	if(!BP_JSONCreateOutputFromPathQuery(json_to_clone_from, lookup_query, base64_encode_bin_data))
		return NULL;

	// create json buffer from local TQ (do not use finalize output since we're creating json from a path lookup)
	if(!BP_JSONGenerateJSONBufferFromLocalTQ(json_to_clone_from))
		return NULL;

	// ensure we could create our json
	if(!json_to_clone_from->json_out)
		return NULL;


	// create clone from generated JSON output (reparse)
	P_BP_JSON clone_json = BP_JSONCreateFromBuffer
	(
		json_to_clone_from->json_out,
		bpstrlen(json_to_clone_from->json_out),
		max_depth_for_token_allocation,
		max_tokens_total
	);

	if(!clone_json)
		return NULL;
	if(!clone_json->parse_state)
		return NULL;

	// if we have output set, destroy it before exit
	if(json_to_clone_from->json_out)
	{
		ht_free(json_to_clone_from->system_hreg, json_to_clone_from->json_out);
		json_to_clone_from->json_out = NULL;
	}

	// return the cloned json
	return clone_json;

}


/*
 * BP_JSONUtils.cc
 *
 *  Created on: May 30, 2017
 *      Author: root
 */


// include main includes
#include "../../../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create and Destroy %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// class constructor
BP_JSONUtils::BP_JSONUtils()
{

	// nullify class pointers
	this->json = NULL;

	// maximum depth
	this->max_depth = 40;

	// maximum tokens total
	this->max_tokens_total = 4096;

	// this should be set to a proxydb entry if you have one (or a custom one if you want)
	this->web_proxy = NULL;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create JSON Structure %%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new json
	this->json = BP_JSONCreate();
	if(!this->json)
	{
		printf("\n [!!] Error: Failed to create new JSON session.");
		return;
	}

	// mark initialization flag
	this->initialized = BP_TRUE;

}

// destroy json
BP_JSONUtils::~BP_JSONUtils()
{


	// destroy json if it exists
	if(this->json)
	{
		BP_JSONDestroy(this->json);
		this->json = NULL;
	}


}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Cleanup Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// clean all dangling data
BP_ERROR_T BP_JSONUtils::clean()
{

	// immediately return with success if we have nothing to clean
	if(!this->json)
		return ERR_SUCCESS;

	// destroy json
	BP_JSONDestroy(this->json);

	// nullify json
	this->json = NULL;

	// ensure we can create our json
	this->json = BP_JSONCreate();
	if(!this->json)
	{
		this->AddErr("Cleaned JSON, but after cleaning was unable to recreate JSON session.  This is likely due to being out of memory.");
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}


// finalize json (generates json buffers from registry structures)
BP_ERROR_T BP_JSONUtils::finalize(BP_BOOL encapsulate_result)
{

	// ensure we have json
	if(!this->json)
		return ERR_FAILURE;


	// finalize
	BP_JSONFinalizeOutput(this->json, encapsulate_result);

	// ensure we've created output
	if(!this->json->json_out)
		return ERR_FAILURE;
	if(!this->json->json_out_len)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Local JSON Generators (loads from local sources) %%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// attempt to load from buffer
BP_ERROR_T BP_JSONUtils::loadFromBuffer(char *buffer, size_t buff_len)
{

	// run basic checks
	if(!this->json)
		return ERR_FAILURE;
	if(!buffer)
		return ERR_FAILURE;
	if(!buff_len)
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(buffer, buff_len, BP_TRUE, BP_TRUE))
		return ERR_FAILURE;

	// clean out any dangling data
	if(!this->clean())
		return ERR_FAILURE;

	// attempt to parse
	BP_ERROR_T parse_ret = BP_JSONParseFromBuffer(this->json, buffer, buff_len, this->max_depth, this->max_tokens_total);
	if(!parse_ret)
	{
		this->AddErr("Failed to parse from buffer.");
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}

// attempt to load from file
BP_ERROR_T BP_JSONUtils::loadFromFile(char *file_path)
{

	// ensure we have a json session set
	if(!this->json)
		return ERR_FAILURE;
	if(!BP_StringIsReasonableForUnixEXT4Filename(file_path, bpstrlen(file_path), BP_TRUE))
		return ERR_FAILURE;

	// clean out any dangling data
	if(!this->clean())
		return ERR_FAILURE;

	// attempt parse
	BP_ERROR_T parse_ret = BP_JSONParseFromFile(this->json, file_path, this->max_depth, this->max_tokens_total);
	if(!parse_ret)
	{
		this->AddErr("Failed to parse from file.");
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}

// load from existing json
BP_ERROR_T BP_JSONUtils::loadFromExistingJSON(P_BP_JSON json_to_clone_from)
{

	// ensure we have a json session set
	if(!this->json)
		return ERR_FAILURE;
	if(!json_to_clone_from)
		return ERR_FAILURE;

	// clean out any dangling data
	if(!this->clean())
		return ERR_FAILURE;

	// attempt finalization
	BP_JSONFinalizeOutput(json_to_clone_from, BP_FALSE);
	if(!json_to_clone_from->json_out)
	{
		this->AddErr("Failed to load from existing JSON.  Json was empty set.");
		return ERR_FAILURE;
	}

	// destroy local json
	BP_JSONDestroy(this->json);

	// create from buiffer
	this->json = BP_JSONCreateFromBuffer(json_to_clone_from->json_out, json_to_clone_from->json_out_len, this->max_depth, this->max_tokens_total);
	if(!this->json)
	{
		this->AddErr("Failed to create json from cloned buffer.  This is highly unusual and should never occur at this point due to previous checks being done.  Investigate this.");
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}


// load json from existing hash registry
BP_ERROR_T BP_JSONUtils::loadFromExistingHReg
(
	P_BP_HASH_TABLE_REGISTRY hreg,
	BP_BOOL   include_data,
	BP_BOOL   base64_encode_bin_data
)
{

	// run basic checks
	if(!this->json)
		return ERR_FAILURE;
	if(!hreg)
		return ERR_FAILURE;
	if(!hreg->hive)
		return ERR_FAILURE;

	// clean out any dangling data
	if(!this->clean())
		return ERR_FAILURE;

	// create top path table
	char *tmp_top_path[] =
	{
			hreg->hive->key,
			NULL
	};

	// add the top table
	BP_JSONAddTableByPath(this->json, (char **) &tmp_top_path, BP_FALSE);

	// append the rest of the registry
	if(hreg->hive->data)
	{

		// append children if we have any
		BP_JSONAppendHReg_r(this->json, (P_BP_HTKE) hreg->hive->data, (char **) &tmp_top_path, include_data, base64_encode_bin_data);

	}


	// return indicating success
	return ERR_SUCCESS;

}


// load json from existing hash registry table (requires table to be present)
BP_ERROR_T BP_JSONUtils::loadFromExistingHRegTable
(
	P_BP_HASH_TABLE_REGISTRY hreg,
	P_BP_HTKE                table,
	BP_BOOL                  include_data,
	BP_BOOL                  base64_encode_bin_data
)
{

	// run basic checks
	if(!this->json)
		return ERR_FAILURE;
	if(!hreg)
		return ERR_FAILURE;
	if(!hreg->hive)
		return ERR_FAILURE;
	if(!table)
		return ERR_FAILURE;
	if(!table->key)
		return ERR_FAILURE;
	if(table->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		return ERR_FAILURE;

	// clean out any dangling data
	if(!this->clean())
		return ERR_FAILURE;

	// create top path table
	char *tmp_top_path[] =
	{
			table->key,
			NULL
	};

	// add the top table
	BP_JSONAddTableByPath(this->json, (char **) &tmp_top_path, BP_FALSE);

	// append the rest of the registry
	if(table->data)
	{

		// append children if we have any
		BP_JSONAppendHReg_r(this->json, (P_BP_HTKE) table->data, (char **) &tmp_top_path, include_data, base64_encode_bin_data);

	}


	// return indicating success
	return ERR_SUCCESS;

}



// load json from existing hash registry table path (requires table to be present)
BP_ERROR_T BP_JSONUtils::loadFromExistingHRegTableByPath
(
	P_BP_HASH_TABLE_REGISTRY hreg,
	char                     **path,
	BP_BOOL                  include_data,
	BP_BOOL                  base64_encode_bin_data
)
{

	// run basic checks
	if(!this->json)
		return ERR_FAILURE;
	if(!hreg)
		return ERR_FAILURE;
	if(!hreg->hive)
		return ERR_FAILURE;
	if(!path)
		return ERR_FAILURE;
	if(!path[0])
		return ERR_FAILURE;

	// clean out any dangling data
	if(!this->clean())
		return ERR_FAILURE;

	// lookup table by path
	P_BP_HTKE table_lookup = BP_HashRegLookupTableByPath(hreg, path);
	if(!table_lookup)
		return ERR_FAILURE;

	// attempt to load from table
	return this->loadFromExistingHRegTable
	(
		hreg,
		table_lookup,
		include_data,
		base64_encode_bin_data
	);

}


// load from existing json utils class
BP_ERROR_T BP_JSONUtils::loadFromExistingJSONUtils(BP_JSONUtils *json_utils)
{

	// run basic checks
	if(!json_utils)
		return ERR_FAILURE;
	if(!json_utils->json)
		return ERR_FAILURE;

	// load from json within utils
	return this->loadFromExistingJSON(json_utils->json);

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Remote JSON Generators (loads from remote sources) %%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to load from a HTTP request
BP_ERROR_T BP_JSONUtils::loadFromWeb(char *url)
{

	// ensure we have a json session set
	if(!this->json)
		return ERR_FAILURE;


	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Operate By Path %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// add a table using a path
BP_ERROR_T BP_JSONUtils::addTableByPath(char **path, BP_BOOL table_is_anonymous)
{

	// run basic checks
	if(!this->json)
		return ERR_FAILURE;
	if(!path)
		return ERR_FAILURE;
	if(!path[0])
		return ERR_FAILURE;

	// attempt add
	BP_ERROR_T ret_val = BP_JSONAddTableByPath(this->json, path, table_is_anonymous);
	if(!ret_val)
	{
		this->AddErr("Failed to add table by path.");
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}

// remove a table by path
BP_ERROR_T BP_JSONUtils::removeTableByPath(char **path)
{

	// ensure we have a json session set
	if(!this->json)
		return ERR_FAILURE;

	// attempt to remove table by path
	BP_ERROR_T ret_val = BP_JSONRemoveTableByPath(this->json, path);
	if(!ret_val)
	{
		this->AddErr("Failed to remove table by path.");
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}


// add a node by path
BP_ERROR_T BP_JSONUtils::addNodeByPath(char **path, char *data, size_t data_len, BP_BOOL base64_encode_data)
{

	// ensure we have a json session set
	if(!this->json)
		return ERR_FAILURE;

	printf("\nAdding node at this path: %s\n", data);
	BP_DisplayStringArray(" -- ", 1, 1, path);

	// attempt to add node by path
	BP_ERROR_T ret_val = BP_JSONAddNodeByPath(this->json, path, data, data_len, base64_encode_data);
	if(!ret_val)
	{
		this->AddErr("Failed to add node by path.");
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}



// remove node by path
BP_ERROR_T BP_JSONUtils::removeNodeByPath(char **path)
{

	// ensure we have a json session set
	if(!this->json)
		return ERR_FAILURE;

	// attempt to remove node by path
	BP_ERROR_T ret_val = BP_JSONRemoveNodeByPath(this->json, path);
	if(!ret_val)
	{
		this->AddErr("Failed to remove node by path.");
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}


// replace node by path
BP_ERROR_T BP_JSONUtils::replaceNodeByPath(char **path, char *data, size_t data_len, BP_BOOL base64_encode_data)
{

	// ensure we have a json session set
	if(!this->json)
		return ERR_FAILURE;

	// attempt to replace node by path
	BP_ERROR_T ret_val = BP_JSONReplaceNodeByPath(this->json, path, data, data_len, base64_encode_data);
	if(!ret_val)
	{
		this->AddErr("Failed to replace node by path.");
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}


// replace table by path
BP_ERROR_T BP_JSONUtils::replaceTableByPath(char **path, BP_BOOL table_is_anonymous)
{

	// ensure we have a json session set
	if(!this->json)
		return ERR_FAILURE;

	// attempt to replace table by path
	BP_ERROR_T ret_val = BP_JSONReplaceTableByPath(this->json, path, table_is_anonymous);
	if(!ret_val)
	{
		this->AddErr("Failed to replace table by path.");
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}


// append hreg by path
BP_ERROR_T BP_JSONUtils::appendHRegByPath
(
		char                      **path,
		char                       *new_appended_table_name,
		P_BP_HASH_TABLE_REGISTRY    hreg,
		BP_BOOL                     include_data,
		BP_BOOL                     base64_encode_bin_data
)
{

	// ensure we have a json session set
	if(!this->json)
		return ERR_FAILURE;

	// attempt to append hreg by path
	BP_ERROR_T ret_val = BP_JSONAppendHRegByTablePath(this->json, hreg, new_appended_table_name, path, include_data, base64_encode_bin_data);
	if(!ret_val)
	{
		this->AddErr("Failed to append hreg by path.");
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}


// append json by path
BP_ERROR_T BP_JSONUtils::appendJSONByPath
(
		char       **path,
		char        *new_appended_json_table_name,
		P_BP_JSON   json_appending,
		BP_BOOL     include_data,
		BP_BOOL     base64_encode_bin_data
)
{

	// ensure we have a json session set
	if(!this->json)
		return ERR_FAILURE;

	// attempt to append json by path
	BP_ERROR_T ret_val = BP_JSONAppendByTablePath(this->json, json_appending, new_appended_json_table_name, path, include_data, base64_encode_bin_data);
	if(!ret_val)
	{
		this->AddErr("Failed to append json by path.");
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}


// --- add-node utilities based on type assumptions ---------

// add a string node by path (auto length checks, auto reasonable check)
BP_ERROR_T BP_JSONUtils::addStringNodeByPath(char **path, char *reasonable_string)
{

	// run basic checks
	if(!path)
		return ERR_FAILURE;
	if(!reasonable_string)
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(reasonable_string, bpstrlen(reasonable_string), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// add the node by path
	return this->addNodeByPath(path, reasonable_string, bpstrlen(reasonable_string), BP_FALSE);

}

// add a file node by path (length is dependent on file length, file must fit in memory, file will be base64 encoded)
BP_ERROR_T BP_JSONUtils::addFileNodeByPath(char **path, char *file_path)
{

	// run basic checks
	if(!path)
		return ERR_FAILURE;
	if(!file_path)
		return ERR_FAILURE;
	if(!BP_StringIsReasonableForUnixEXT4Filename(file_path, bpstrlen(file_path), BP_TRUE))
		return ERR_FAILURE;

	// create and zero out extra info
	BP_FILE_LOAD_EXTRA_INFO extra_info;
	BP_ZERO_STACK(extra_info);

	// attempt to load file contents
	char * file_buff = BP_FileGetContents(file_path, (P_BP_FILE_LOAD_EXTRA_INFO) &extra_info);
	if(!file_buff)
		return ERR_FAILURE;
	if(!extra_info.file_size)
	{
		bpfree(file_buff);
		return ERR_FAILURE;
	}

	// add the node
	BP_ERROR_T ret_val = this->addNodeByPath(path, file_buff, extra_info.file_size, BP_TRUE);

	// destroy the file buffer
	bpfree(file_buff);

	// return indicating success
	return ret_val;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Operate By Path Query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// add a table using a path query
BP_ERROR_T BP_JSONUtils::addTableByPathQuery(char *path_query, BP_BOOL table_is_anonymous)
{

	// ensure we have a json session set
	if(!this->json)
		return ERR_FAILURE;

	// create path
	char ** path = BP_HashRegStringLookupToPathArray(path_query, bpstrlen(path_query));
	if(!path)
		return ERR_FAILURE;


	// perform operation
	BP_ERROR_T ret_val = this->addTableByPath(path, table_is_anonymous);

	// destroy the path if necessary
	if(path)
	{
		BP_DestroyPointerArrayNullTerminated((void **) path);
		bpfree(path);
	}

	// return based on operation
	return ret_val;

}

// remove a table by path query
BP_ERROR_T BP_JSONUtils::removeTableByPathQuery(char *path_query)
{

	// ensure we have a json session set
	if(!this->json)
		return ERR_FAILURE;

	// create path
	char ** path = BP_HashRegStringLookupToPathArray(path_query, bpstrlen(path_query));
	if(!path)
		return ERR_FAILURE;

	// remove table by path
	BP_ERROR_T ret_val = this->removeTableByPath(path);

	// destroy the path if necessary
	if(path)
	{
		BP_DestroyPointerArrayNullTerminated((void **) path);
		bpfree(path);
	}

	// return based on operation
	return ret_val;

}

// add a node by path query
BP_ERROR_T BP_JSONUtils::addNodeByPathQuery(char *path_query, char *data, size_t data_len, BP_BOOL base64_encode_data)
{

	// ensure we have a json session set
	if(!this->json)
		return ERR_FAILURE;

	// create path
	char ** path = BP_HashRegStringLookupToPathArray(path_query, bpstrlen(path_query));
	if(!path)
		return ERR_FAILURE;

	// attempt to add node by path
	BP_ERROR_T ret_val = this->addNodeByPath(path, data, data_len, base64_encode_data);

	// destroy the path if necessary
	if(path)
	{
		BP_DestroyPointerArrayNullTerminated((void **) path);
		bpfree(path);
	}

	// return based on operation
	return ret_val;

}






// remove node by path query
BP_ERROR_T BP_JSONUtils::removeNodeByPathQuery(char *path_query)
{

	// ensure we have a json session set
	if(!this->json)
		return ERR_FAILURE;

	// create path
	char ** path = BP_HashRegStringLookupToPathArray(path_query, bpstrlen(path_query));
	if(!path)
		return ERR_FAILURE;


	// attempt to remove node by path
	BP_ERROR_T ret_val = this->removeNodeByPath(path);

	// destroy the path if necessary
	if(path)
	{
		BP_DestroyPointerArrayNullTerminated((void **) path);
		bpfree(path);
	}

	// return based on operation
	return ret_val;

}

// replace node by path query
BP_ERROR_T BP_JSONUtils::replaceNodeByPathQuery(char *path_query, char *data, size_t data_len, BP_BOOL base64_encode_data)
{

	// ensure we have a json session set
	if(!this->json)
		return ERR_FAILURE;

	// create path
	char ** path = BP_HashRegStringLookupToPathArray(path_query, bpstrlen(path_query));
	if(!path)
		return ERR_FAILURE;

	// perform replacement
	BP_ERROR_T ret_val = this->replaceNodeByPath(path, data, data_len, base64_encode_data);


	// destroy the path if necessary
	if(path)
	{
		BP_DestroyPointerArrayNullTerminated((void **) path);
		bpfree(path);
	}

	// return based on operation
	return ret_val;

}

// replace table by path query
BP_ERROR_T BP_JSONUtils::replaceTableByPathQuery(char *path_query, BP_BOOL table_is_anonymous)
{

	// ensure we have a json session set
	if(!this->json)
		return ERR_FAILURE;

	// create path
	char ** path = BP_HashRegStringLookupToPathArray(path_query, bpstrlen(path_query));
	if(!path)
		return ERR_FAILURE;

	// perform replacement
	BP_ERROR_T ret_val = this->replaceTableByPath(path, table_is_anonymous);

	// destroy the path if necessary
	if(path)
	{
		BP_DestroyPointerArrayNullTerminated((void **) path);
		bpfree(path);
	}

	// return based on operation
	return ret_val;

}

// append hreg by path query
BP_ERROR_T BP_JSONUtils::appendHRegByPathQuery
(
		char                       *path_query,
		char                       *new_appended_table_name,
		P_BP_HASH_TABLE_REGISTRY    hreg,
		BP_BOOL                     include_data,
		BP_BOOL                     base64_encode_bin_data
)
{

	// ensure we have a json session set
	if(!this->json)
		return ERR_FAILURE;

	// create path
	char ** path = BP_HashRegStringLookupToPathArray(path_query, bpstrlen(path_query));
	if(!path)
		return ERR_FAILURE;

	// append hash table registry by path
	BP_ERROR_T ret_val = this->appendHRegByPath
	(
			path,
			new_appended_table_name,
			hreg,
			include_data,
			base64_encode_bin_data
	);

	// destroy the path if necessary
	if(path)
	{
		BP_DestroyPointerArrayNullTerminated((void **) path);
		bpfree(path);
	}

	// return based on operation
	return ret_val;

}

// append json by path query
BP_ERROR_T BP_JSONUtils::appendJSONByPathQuery
(
		char        *path_query,
		char        *new_appended_json_table_name,
		P_BP_JSON   json_appending,
		BP_BOOL     include_data,
		BP_BOOL     base64_encode_bin_data
)
{

	// ensure we have a json session set
	if(!this->json)
		return ERR_FAILURE;

	// create path
	char ** path = BP_HashRegStringLookupToPathArray(path_query, bpstrlen(path_query));
	if(!path)
		return ERR_FAILURE;

	// append json by path
	BP_ERROR_T ret_val = this->appendJSONByPath
	(
			path,
			new_appended_json_table_name,
			json_appending,
			include_data,
			base64_encode_bin_data
	);

	// destroy the path if necessary
	if(path)
	{
		BP_DestroyPointerArrayNullTerminated((void **) path);
		bpfree(path);
	}

	// return based on operation
	return ret_val;

}


// --- add-node utilities based on type assumptions ---------

// add a string node by path query (auto length checks, auto reasonable check)
BP_ERROR_T BP_JSONUtils::addStringNodeByPathQuery(char *path_query, char *reasonable_string)
{

	// run basic checks
	if(!path_query)
		return ERR_FAILURE;
	if(!reasonable_string)
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(reasonable_string, bpstrlen(reasonable_string), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// add the node by path query
	BP_ERROR_T ret_val = this->addNodeByPathQuery(path_query, reasonable_string, bpstrlen(reasonable_string), BP_FALSE);

	// return the result of the operation
	return ret_val;
}

// add a file node by path query (length is dependent on file length, file must fit in memory, file will be base64 encoded)
BP_ERROR_T BP_JSONUtils::addFileNodeByPathQuery(char *path_query, char *file_path)
{

	// run basic checks
	if(!path_query)
		return ERR_FAILURE;
	if(!file_path)
		return ERR_FAILURE;

	// create path
	char ** path = BP_HashRegStringLookupToPathArray(path_query, bpstrlen(path_query));
	if(!path)
		return ERR_FAILURE;

	// add file node by path
	BP_ERROR_T ret_val = this->addFileNodeByPath(path, file_path);

	// destroy the path if necessary
	if(path)
	{
		BP_DestroyPointerArrayNullTerminated((void **) path);
		bpfree(path);
	}

	// return the result of the operation
	return ret_val;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Lookup Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// --- by path -----------

// lookup a table by path (returned item is type checked to ensure it's a table)
P_BP_HTKE BP_JSONUtils::lookupTableByPath(char **path)
{

	// run basic checks
	if(!this->json)
		return NULL;
	if(!path)
		return NULL;
	if(!path[0])
		return NULL;

	// attempt to lookup the entry
	P_BP_HTKE found_entry = BP_JSONLookupEntryByPath(this->json, path);
	if(!found_entry)
		return NULL;

	// run type check
	if(found_entry->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		return NULL;

	// return the entry if it's found
	return found_entry;

}

// lookup a node by path (returned item is type checked to ensure it's not a table)
P_BP_HTKE BP_JSONUtils::lookupNodeByPath(char **path)
{


	// run basic checks
	if(!this->json)
		return NULL;
	if(!path)
		return NULL;
	if(!path[0])
		return NULL;

	// attempt to lookup the entry
	P_BP_HTKE found_entry = BP_JSONLookupEntryByPath(this->json, path);
	if(!found_entry)
		return NULL;

	// run type check
	if(found_entry->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		return NULL;

	// return the entry if it's found
	return found_entry;

}

// --- by path query ----

// lookup a table by path query (returned item is type checked to ensure it's a table)
P_BP_HTKE BP_JSONUtils::lookupTableByPathQuery(char *path_query)
{

	// ensure we have a json session set
	if(!this->json)
		return NULL;

	// create path
	char ** path = BP_HashRegStringLookupToPathArray(path_query, bpstrlen(path_query));
	if(!path)
		return NULL;

	// lookup table
	P_BP_HTKE ret_val = this->lookupTableByPath(path);

	// destroy the path if necessary
	if(path)
	{
		BP_DestroyPointerArrayNullTerminated((void **) path);
		bpfree(path);
	}

	// return based on operation
	return ret_val;

}

// lookup a node by path query (returned item is type checked to ensure it's not a table)
P_BP_HTKE BP_JSONUtils::lookupNodeByPathQuery(char *path_query)
{


	// ensure we have a json session set
	if(!this->json)
		return NULL;

	// create path
	char ** path = BP_HashRegStringLookupToPathArray(path_query, bpstrlen(path_query));
	if(!path)
		return NULL;

	// lookup node
	P_BP_HTKE ret_val = this->lookupNodeByPath(path);

	// destroy the path if necessary
	if(path)
	{
		BP_DestroyPointerArrayNullTerminated((void **) path);
		bpfree(path);
	}

	// return based on operation
	return ret_val;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Data Access Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to access a text node by a given path (does not check if string is reasonable)
char * BP_JSONUtils::accessUTF8TextEntryByPath(char **path)
{

	// ensure
	if(!path)
		return NULL;

	// attempt to lookup the entry
	P_BP_HTKE entry_lookup = this->lookupNodeByPath(path);

	// return the data if we have a type match
	switch(entry_lookup->type)
	{

		// if the type lookup is a utf-8 string, return it immediately
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_JSON:
			if(entry_lookup->data)
				return (char *) entry_lookup->data;
			break;

		default:
			printf("\n ENTRY TYPE MISMATCH?: %zu", entry_lookup->type);
			break;

	}

	// return null if we were unable to lookup the item
	return NULL;
}


// path query version of the UTF8 text access method.
char *BP_JSONUtils::accessUTF8TextEntryByPathQuery(char *path_query)
{
	// create path
	char ** path = BP_HashRegStringLookupToPathArray(path_query, bpstrlen(path_query));
	if(!path)
		return NULL;

	// return the utf8 text entry
	return this->accessUTF8TextEntryByPath(path);

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display JSON %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// display json output
BP_ERROR_T BP_JSONUtils::display()
{

	// ensure we have a json session
	if(!this->json)
	{
		printf("\n [+] Cannot display json because json session pointer is null.");
		return ERR_FAILURE;
	}

	// display the json
	BP_JSONDisplay(this->json);

	// display errors and warnings if any are set
	if(this->error_stack)
		this->DisplayErrors();
	if(this->warning_stack)
		this->DisplayWarnings();

	// return indicating success
	return ERR_SUCCESS;

}


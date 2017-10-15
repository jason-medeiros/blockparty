/*
 * BP_JSONUtils.h
 *
 *  Created on: May 30, 2017
 *      Author: root
 */

#ifndef INCLUDE_BLOCKCLASS_SUBCLASSES_BP_JSONUTILS_H_
#define INCLUDE_BLOCKCLASS_SUBCLASSES_BP_JSONUTILS_H_


class BP_JSONUtils : public BP_BaseError
{

	public:

	// json C session used by this class.
	P_BP_JSON json;

	// marked when the class is initialized ok
	BP_BOOL initialized;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create and Destroy %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create json utils
	BP_JSONUtils();

	// destroy the utils
	~BP_JSONUtils();


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Cleanup Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// clean all dangling data
	BP_ERROR_T clean();

	// finalize json (generates json buffers from registry structures)
	BP_ERROR_T finalize(BP_BOOL encapsulate_result = BP_FALSE);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Local JSON Generators (loads from local sources) %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// maximum depth
	size_t max_depth;

	// maximum tokens total
	size_t max_tokens_total;

	// attempt to load empty json (new json)
	BP_ERROR_T loadEmpty();

	// attempt to load from buffer
	BP_ERROR_T loadFromBuffer(char *buffer, size_t buff_len);

	// attempt to load from file
	BP_ERROR_T loadFromFile(char *file_path);

	// load from existing json session
	BP_ERROR_T loadFromExistingJSON(P_BP_JSON json_to_clone_from);

	// load json from existing hash registry
	BP_ERROR_T loadFromExistingHReg
	(
		P_BP_HASH_TABLE_REGISTRY hreg,
		BP_BOOL                  include_data           = BP_TRUE,
		BP_BOOL                  base64_encode_bin_data = BP_TRUE
	);

	// load json from existing hash registry table (requires table to be present)
	BP_ERROR_T loadFromExistingHRegTable
	(
		P_BP_HASH_TABLE_REGISTRY hreg,
		P_BP_HTKE                table,
		BP_BOOL                  include_data           = BP_TRUE,
		BP_BOOL                  base64_encode_bin_data = BP_TRUE
	);

	// load json from existing hash registry table path (requires table to be present)
	BP_ERROR_T loadFromExistingHRegTableByPath
	(
		P_BP_HASH_TABLE_REGISTRY hreg,
		char                     **path,
		BP_BOOL                  include_data           = BP_TRUE,
		BP_BOOL                  base64_encode_bin_data = BP_TRUE
	);

	// load from existing json utils class
	BP_ERROR_T loadFromExistingJSONUtils(BP_JSONUtils *json_utils);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Remote JSON Generators (loads from remote sources) %%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we have a proxy to use, it will be set before
	P_BP_PROXY_DB_ENTRY web_proxy;

	// attempt to load from a HTTP request
	BP_ERROR_T loadFromWeb(char *url);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Operate By Path %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add a table using a path
	BP_ERROR_T addTableByPath(char **path, BP_BOOL table_is_anonymous = BP_FALSE);

	// remove a table by path
	BP_ERROR_T removeTableByPath(char **path);

	// add a node by path
	BP_ERROR_T addNodeByPath(char **path, char *data, size_t data_len, BP_BOOL base64_encode_data = BP_FALSE);

	// remove node by path
	BP_ERROR_T removeNodeByPath(char **path);

	// replace node by path
	BP_ERROR_T replaceNodeByPath(char **path, char *data, size_t data_len, BP_BOOL base64_encode_data = BP_FALSE);

	// replace table by path
	BP_ERROR_T replaceTableByPath(char **path, BP_BOOL table_is_anonymous = BP_FALSE);

	// append hreg by path
	BP_ERROR_T appendHRegByPath
	(
			char                      **path,
			char                       *new_appended_table_name,
			P_BP_HASH_TABLE_REGISTRY    hreg,
			BP_BOOL                     include_data,
			BP_BOOL                     base64_encode_bin_data = BP_TRUE
	);

	// append json by path
	BP_ERROR_T appendJSONByPath
	(
			char       **path,
			char        *new_appended_json_table_name,
			P_BP_JSON   json_appending,
			BP_BOOL     include_data,
			BP_BOOL     base64_encode_bin_data = BP_TRUE
	);

	// --- add-node utilities based on type assumptions ---------

	// add a string node by path (auto length checks, auto reasonable check)
	BP_ERROR_T addStringNodeByPath(char **path, char *reasonable_string);

	// add a file node by path (length is dependent on file length, file must fit in memory, file will be base64 encoded)
	BP_ERROR_T addFileNodeByPath(char **path, char *file_path);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Operate By Path Query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add a table using a path query
	BP_ERROR_T addTableByPathQuery(char *path_query, BP_BOOL table_is_anonymous = BP_FALSE);

	// remove a table by path query
	BP_ERROR_T removeTableByPathQuery(char *path_query);

	// add a node by path query
	BP_ERROR_T addNodeByPathQuery(char *path_query, char *data, size_t data_len, BP_BOOL base64_encode_data = BP_FALSE);

	// remove node by path query
	BP_ERROR_T removeNodeByPathQuery(char *path_query);

	// replace node by path query
	BP_ERROR_T replaceNodeByPathQuery(char *path_query, char *data, size_t data_len, BP_BOOL base64_encode_data = BP_FALSE);

	// replace table by path query
	BP_ERROR_T replaceTableByPathQuery(char *path_query, BP_BOOL table_is_anonymous = BP_FALSE);

	// append hreg by path query
	BP_ERROR_T appendHRegByPathQuery
	(
			char                       *path_query,
			char                       *new_appended_table_name,
			P_BP_HASH_TABLE_REGISTRY    hreg,
			BP_BOOL                     include_data,
			BP_BOOL                     base64_encode_bin_data = BP_TRUE
	);

	// append json by path query
	BP_ERROR_T appendJSONByPathQuery
	(
			char        *path_query,
			char        *new_appended_json_table_name,
			P_BP_JSON   json_appending,
			BP_BOOL     include_data,
			BP_BOOL     base64_encode_bin_data = BP_TRUE
	);


	// --- add-node utilities based on type assumptions ---------

	// add a string node by path query (auto length checks, auto reasonable check)
	BP_ERROR_T addStringNodeByPathQuery(char *path_query, char *reasonable_string);

	// add a file node by path query (length is dependent on file length, file must fit in memory, file will be base64 encoded)
	BP_ERROR_T addFileNodeByPathQuery(char *path_query, char *file_path);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Lookup Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// --- by path -----------

	// lookup a table by path (returned item is type checked to ensure it's a table)
	P_BP_HTKE lookupTableByPath(char **path);

	// lookup a node by path (returned item is type checked to ensure it's not a table)
	P_BP_HTKE lookupNodeByPath(char **path);

	// --- by path query ----

	// lookup a table by path query (returned item is type checked to ensure it's a table)
	P_BP_HTKE lookupTableByPathQuery(char *path_query);

	// lookup a node by path query (returned item is type checked to ensure it's not a table)
	P_BP_HTKE lookupNodeByPathQuery(char *path_query);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Data Access Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to access a text node by a given path (does not check if string is reasonable)
	char * accessUTF8TextEntryByPath(char **path);

	// path query version of the UTF8 text access method.
	char * accessUTF8TextEntryByPathQuery(char *path_query);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display JSON %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// display json output
	BP_ERROR_T display();

};




#endif /* INCLUDE_BLOCKCLASS_SUBCLASSES_BP_JSONUTILS_H_ */

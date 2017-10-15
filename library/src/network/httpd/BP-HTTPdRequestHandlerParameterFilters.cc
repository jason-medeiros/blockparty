/*
 * BP-HTTPdRequestHandlerParameterFilters.cc
 *
 *  Created on: Dec 13, 2015
 *      Author: root
 */

// include main
#include "../../../include/BP-Main.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Request Handler Parameter Filters %%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Add parameter filter.  Filters stack.  Attempting to add two filters
// to one request handler will result in cascading filtering.
BP_ERROR_T BP_HTTPdRequestHandlerParameterFilterAdd
(
	P_BP_HTTPD_REQUEST_HANDLER                         request_handler,
	BP_HTTPD_REQUEST_VAR_TYPE                          var_type,
	char                                              *key,
	char                                              *filter_name,
	BP_HTTPdRequestHandlerParameterFilterFptr          filter,
	P_BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER_CONFIG filter_config
)
{

	// Run basic checks
	if(!request_handler)
		return ERR_FAILURE;
	if(!var_type)
		return ERR_FAILURE;
	if(!key)
		return ERR_FAILURE;
	if(!filter)
		return ERR_FAILURE;

	// note: filter_config is optional.


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Registry if Necessary %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// create space for registry path
	char *reg_path[10];
	BP_ZERO_STACK(reg_path);

	// path index is zero
	size_t path_idx = 0;

	// set parameter filter
	reg_path[path_idx] = "filters";
	path_idx++;

	// only create filters if they don't exist.
	if(!request_handler->parameter_filters)
	{

		// create hash table registry
		request_handler->parameter_filters = BP_CreateHashTableRegistry();
		if(!request_handler->parameter_filters)
			return ERR_FAILURE;

		// add filters table
		BP_HashRegAddTableByPath(request_handler->parameter_filters, (char **) &reg_path);

		// add cookie table
		reg_path[path_idx] = "BP_HTTPD_REQUEST_VAR_TYPE_COOKIE";
		BP_HashRegAddTableByPath(request_handler->parameter_filters, (char **) &reg_path);

		// add http get table
		reg_path[path_idx] = "BP_HTTPD_REQUEST_VAR_TYPE_GET";
		BP_HashRegAddTableByPath(request_handler->parameter_filters, (char **) &reg_path);

		// add http header table
		reg_path[path_idx] = "BP_HTTPD_REQUEST_VAR_TYPE_HEADER";
		BP_HashRegAddTableByPath(request_handler->parameter_filters, (char **) &reg_path);

		// add post data table
		reg_path[path_idx] = "BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA";
		BP_HashRegAddTableByPath(request_handler->parameter_filters, (char **) &reg_path);

		// add post tmpfile table
		reg_path[path_idx] = "BP_HTTPD_REQUEST_VAR_TYPE_POST_TMPFILE";
		BP_HashRegAddTableByPath(request_handler->parameter_filters, (char **) &reg_path);


	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt Filter Addition %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This is a type indicator string set in the switch below
	char * type_indicator_str = NULL;

	// create add path on the stack
	char *add_path[10];
	BP_ZERO_STACK(add_path);

	// path index
	size_t add_path_idx = 0;

	// set initial path to filters
	add_path[add_path_idx] = "filters";
	add_path_idx++;


	// switch on the var type
	switch(var_type)
	{


		// post data type
		case BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA:
			type_indicator_str = "BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA";
			add_path[path_idx] = "BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA";
			add_path_idx++;
			break;

		// set tmpfile
		case BP_HTTPD_REQUEST_VAR_TYPE_POST_TMPFILE:
			type_indicator_str = "BP_HTTPD_REQUEST_VAR_TYPE_POST_TMPFILE";
			add_path[path_idx] = "BP_HTTPD_REQUEST_VAR_TYPE_POST_TMPFILE";
			add_path_idx++;
			break;

		// set get
		case BP_HTTPD_REQUEST_VAR_TYPE_GET:
			type_indicator_str = "BP_HTTPD_REQUEST_VAR_TYPE_GET";
			add_path[path_idx] = "BP_HTTPD_REQUEST_VAR_TYPE_GET";
			add_path_idx++;
			break;

		// set cookie
		case BP_HTTPD_REQUEST_VAR_TYPE_COOKIE:
			type_indicator_str = "BP_HTTPD_REQUEST_VAR_TYPE_COOKIE";
			add_path[path_idx] = "BP_HTTPD_REQUEST_VAR_TYPE_COOKIE";
			add_path_idx++;
			break;

		// set header path
		case BP_HTTPD_REQUEST_VAR_TYPE_HEADER:
			type_indicator_str = "BP_HTTPD_REQUEST_VAR_TYPE_HEADER";
			add_path[path_idx] = "BP_HTTPD_REQUEST_VAR_TYPE_HEADER";
			add_path_idx++;
			break;

		// default case is to break
		default:
			break;

	}

	// ensure we have a type indicator string (set in switch above)
	if(!type_indicator_str)
		return ERR_FAILURE;

	// set the final path lookup from key parameter
	add_path[add_path_idx] = key;
	add_path_idx++;

	// add table by path
	BP_HashRegAddTableByPath
	(
		request_handler->parameter_filters,
		(char **) &add_path
	);

	// filter set for this filter
	add_path[add_path_idx] = "filter_set";

	// add table by path
	if(!BP_HashRegAddTableByPath(request_handler->parameter_filters,(char **) &add_path))
	{

		// return indicating failure if we couldn't add the filter set table ok
		return ERR_FAILURE;

	}




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Filter Entry %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set lookup path
	char *filter_set_add_path[] = {"filters", type_indicator_str, key, "filter_set", NULL};

	// set filter add value
	BP_ERROR_T filter_add_ret_val = ERR_FAILURE;

	// attempt to add data to table
	filter_add_ret_val = BP_HashRegAddDataToTableByPath
	(
			request_handler->parameter_filters,
			(char **) &filter_set_add_path,
			filter_name,
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
			NULL,
			sizeof(BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER)
	);

	// ensure we have added the filter ok
	if(!filter_add_ret_val)
	{
		return ERR_FAILURE;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Lookup %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set lookup path
	char *filter_set_lookup_path[] = {"filters", type_indicator_str, key, "filter_set", filter_name, NULL};

	// attempt to lookup the new filter
	P_BP_HASH_TABLE_KEYED_ENTRY new_filter_lookup = BP_HashRegLookupTableEntryByPath
	(
		request_handler->parameter_filters,
		(char **) &filter_set_lookup_path
	);

	// ensure we have a new filter
	if(!new_filter_lookup)
		return ERR_FAILURE;
	if(!new_filter_lookup->data)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add New Entry Info %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// reference new filter
	P_BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER new_filter = (P_BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER) new_filter_lookup->data;

	// set the new filter key
	new_filter->key = filter_name;

	// set the filter
	new_filter->filter = filter;

	// set the var type
	new_filter->var_type = var_type;

	// set the request handler
	new_filter->request_handler = request_handler;

	// if we have a filter config, copy it over
	if(filter_config)
	{

		// copy in config
		memcpy
		(
			&new_filter->filter_config,
			filter_config,
			sizeof(BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER_CONFIG)
		);

		// mark set config value to true after copy completes
		new_filter->filter_config_has_been_set = BP_TRUE;

	}

	// return indicating success after all data set
	return ERR_SUCCESS;

}


// Attempt to lookup a parameter filter set child-table.
P_BP_HASH_TABLE_KEYED_ENTRY BP_HTTPdRequestHandlerParameterFilterSetLookup
(
	P_BP_HTTPD_REQUEST_HANDLER                request_handler,
	BP_HTTPD_REQUEST_VAR_TYPE                 var_type,
	char                                    * key
)
{

	// ensure we have a request handler
	if(!request_handler)
		return NULL;
	if(!var_type)
		return NULL;
	if(!key)
		return NULL;

	// ensure key is reasonable printable
	if(!BP_StringIsReasonablePrintableString(key, bpstrlen(key), BP_FALSE, BP_FALSE))
		return NULL;


	// create lookup path on the stack
	char *lookup_path[10];
	BP_ZERO_STACK(lookup_path);

	// path index
	size_t path_idx = 0;

	// set initial path to filters
	lookup_path[path_idx] = "filters";
	path_idx++;


	// switch on the var type
	switch(var_type)
	{

		// post tmpfile type
		case BP_HTTPD_REQUEST_VAR_TYPE_POST_TMPFILE:
			lookup_path[path_idx] = "BP_HTTPD_REQUEST_VAR_TYPE_POST_TMPFILE";
			path_idx++;
			break;

		// post data type
		case BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA:
			lookup_path[path_idx] = "BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA";
			path_idx++;
			break;

		// set get
		case BP_HTTPD_REQUEST_VAR_TYPE_GET:
			lookup_path[path_idx] = "BP_HTTPD_REQUEST_VAR_TYPE_GET";
			path_idx++;
			break;

		// set cookie
		case BP_HTTPD_REQUEST_VAR_TYPE_COOKIE:
			lookup_path[path_idx] = "BP_HTTPD_REQUEST_VAR_TYPE_COOKIE";
			path_idx++;
			break;

		// set header path
		case BP_HTTPD_REQUEST_VAR_TYPE_HEADER:
			lookup_path[path_idx] = "BP_HTTPD_REQUEST_VAR_TYPE_HEADER";
			path_idx++;
			break;

		// default case is to break
		default:
			break;

	}

	// set the id path from key parameter
	lookup_path[path_idx] = key;
	path_idx++;

	// set the filterset parameter to cap it off
	lookup_path[path_idx] = "filter_set";

	// attempt to lookup the table by path
	P_BP_HASH_TABLE_KEYED_ENTRY filter_table = BP_HashRegLookupTableByPath(request_handler->parameter_filters, (char **) &lookup_path);
	if(!filter_table)
		return NULL;


	// return
	return (P_BP_HASH_TABLE_KEYED_ENTRY) filter_table->data;

}


// Attempt to lookup a parameter filter structure given all lookup parameters.  This is mainly used for checking if
// filters exist before adding new filters, but it can also be used for any various kind of needed lookup.
P_BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER BP_HTTPdRequestHandlerFilterLookup
(
	P_BP_HTTPD_REQUEST_HANDLER                request_handler,
	BP_HTTPD_REQUEST_VAR_TYPE                 var_type,
	char                                    * key,
	char                                    * filter_name,
	BP_HTTPdRequestHandlerParameterFilterFptr filter
)
{

	// basic null checks
	if(!request_handler)
		return NULL;
	if(!var_type)
		return NULL;
	if(!key)
		return NULL;
	if(!filter)
		return NULL;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt Lookup %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set lookup path
	char *filter_set_lookup[] = {"filters", key, "fitler_set", "filter_name", NULL};

	// run filter check
	P_BP_HASH_TABLE_KEYED_ENTRY filter_lookup = BP_HashRegLookupTableEntryByPath(request_handler->parameter_filters, (char **) &filter_set_lookup);
	if(!filter)
		return NULL;

	// set filter
	P_BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER filter_data = (P_BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER) filter_lookup->data;

	// return the lookup (or null if it's not found)
	return filter_data;

}

// Run a specific check against a var.  Requires an already looked up filter and variable.
BP_BOOL BP_HTTPdRequestHandlerFilterCheckVarAgainstFilter
(
		P_BP_HTTPD_REQUEST_VAR                      var,
		P_BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER filter
)
{


	// run basic filter checks
	if(!filter)
		return BP_FALSE;
	if(!filter->key)
		return BP_FALSE;
	if(!filter->filter)
		return BP_FALSE;

	// run basic var checks (don't run data checks yet; data checks are run after type is
	// actualized)
	if(!var)
		return BP_FALSE;
	if(!var->name)
		return BP_FALSE;
	if(!BP_StringIsReasonablePrintableString(var->name, bpstrlen(var->name), BP_FALSE, BP_FALSE))
		return BP_FALSE;
	if(!var->type)
		return BP_FALSE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check Var Data Against Fitler %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Note: The following could probably be implemented in an if/or/case but instead was implemented
	//       in long-form if clauses so that each type could be modified in the future with custom
	//       behaviors per type.  This results in a small code duplication, but it's fine as a price
	//       to keep var behaviors conceptually/organizationally contained.

	// post data
	if(var->type == BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA)
	{

		// ensure we have data to filter
		if(!var->data)
			return BP_FALSE;
		if(!var->data_size)
			return BP_FALSE;


		// run filter
		if(!filter->filter((void *) filter, var, var->name, var->data, var->data_size))
		{

			// return false if the filter fails
			return BP_FALSE;

		}

	}

	// http get data
	if(var->type == BP_HTTPD_REQUEST_VAR_TYPE_GET)
	{


		// ensure we have data to filter
		if(!var->data)
			return BP_FALSE;
		if(!var->data_size)
			return BP_FALSE;


		// run filter
		if(!filter->filter((void *) filter, var, var->name, var->data, var->data_size))
		{

			// return false if the filter fails
			return BP_FALSE;

		}

	}

	// cookie data
	if(var->type == BP_HTTPD_REQUEST_VAR_TYPE_COOKIE)
	{

		// ensure we have data to filter
		if(!var->data)
			return BP_FALSE;
		if(!var->data_size)
			return BP_FALSE;


		// run filter
		if(!filter->filter((void *) filter, var, var->name, var->data, var->data_size))
		{

			// return false if the filter fails
			return BP_FALSE;

		}

	}

	// header data
	if(var->type == BP_HTTPD_REQUEST_VAR_TYPE_HEADER)
	{

		// ensure we have data to filter
		if(!var->data)
			return BP_FALSE;
		if(!var->data_size)
			return BP_FALSE;


		// run filter
		if(!filter->filter((void *) filter, var, var->name, var->data, var->data_size))
		{

			// return false if the filter fails
			return BP_FALSE;

		}
	}


	// --- todo ----

	// tmpfile type
	if(var->type == BP_HTTPD_REQUEST_VAR_TYPE_POST_TMPFILE)
	{
		// todo: implement post tmpfile filter checks (must be done with iterative
		//       filters due to requests not being stored in memory)
	}

	// return indicating success if all passes check OK
	return BP_TRUE;

}

// Attempts to run filter checks on a request.  Request filters are stored in the request handler, filter check
// output is stored in the P_BP_HTTPD_REQUEST itself.  Logically, this function should be stored with the httpd
// request methods themselves, but I made a dev-call and put this function here instead, as it's heavily reliant
// on the code here (above).
BP_ERROR_T BP_HTTPdRequestRunFilterChecks(P_BP_HTTPD_REQUEST req)
{

	// run basic checks
	if(!req)
		return ERR_FAILURE;
	if(!req->request_handler)
		return ERR_FAILURE;
	if(!req->request_handler->parameter_filters)
		return ERR_FAILURE;

	// Note:  This routine tries to be as efficient as possible by only running checks against
	//        posted parameters.


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run HTTP Post Filters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// only run post checks if we have posted arguments
	if(BP_HTTPdRequestContainsPOST(req))
	{


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Lookup HTTP Post Data Table %%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// set the posted data lookup path
		char * post_table_lookup_path[] = {"POST", NULL};

		// post table
		P_BP_HASH_TABLE_KEYED_ENTRY post_table = NULL;

		// attempt to looup the post table
		post_table = BP_HashRegLookupTableByPath(req->arg_hreg, (char **) &post_table_lookup_path);
		if(!post_table->data)
			return ERR_FAILURE;

		// set post table from data pointer so we can iterate
		post_table = (P_BP_HASH_TABLE_KEYED_ENTRY) post_table->data;



		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Iterate HTTP Post Data Table %%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// req->arg_hreg
		P_BP_HTTPD_REQUEST_VAR curr_var = NULL;

		// close the hash iterator
		BP_HASH_ITER_OPEN(post_table, post_table_iter)
		{

			// set the current value on loop
			curr_var = (P_BP_HTTPD_REQUEST_VAR) post_table_iter->data;
			if(!curr_var)
				break;

			// attempt to pull a filter set
			P_BP_HASH_TABLE_KEYED_ENTRY post_filter_set = BP_HTTPdRequestHandlerParameterFilterSetLookup
			(
				req->request_handler,
				BP_HTTPD_REQUEST_VAR_TYPE_POST_DATA,
				curr_var->name
			);

			// ensure we have a filter set
			if(!post_filter_set)
				break;

			// iterate though hashes
			BP_HASH_ITER_OPEN(post_filter_set, post_filter_entry)
			{

				// break if we have no entries
				if(!post_filter_entry)
					break;
				printf("\n WOOP: FILTER KEY -> %s", post_filter_entry->key);

				if
				(
					!BP_HTTPdRequestHandlerFilterCheckVarAgainstFilter
					(
						(P_BP_HTTPD_REQUEST_VAR)                      curr_var,
						(P_BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER) post_filter_entry->data
					)
				)
				{

					// if the filter check fails, exit immediately (don't run other checks, just fail immediately)
					return ERR_FAILURE;
				}



			} BP_HASH_ITER_CLOSE;


		} BP_HASH_ITER_CLOSE;

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run HTTP GET Filters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(BP_HTTPdRequestContainsGET(req))
	{

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Lookup HTTP GET Data Table %%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// set the http get data lookup path
		char * get_table_lookup_path[] = {"GET", NULL};

		// get table
		P_BP_HASH_TABLE_KEYED_ENTRY get_table = NULL;

		// attempt to looup the get table
		get_table = BP_HashRegLookupTableByPath(req->arg_hreg, (char **) &get_table_lookup_path);
		if(!get_table->data)
			return ERR_FAILURE;

		// set get table from data pointer so we can iterate
		get_table = (P_BP_HASH_TABLE_KEYED_ENTRY) get_table->data;



		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Iterate HTTP GET Data Table %%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// req->arg_hreg
		P_BP_HTTPD_REQUEST_VAR curr_var = NULL;

		// close the hash iterator
		BP_HASH_ITER_OPEN(get_table, get_table_iter)
		{

			// set the current value on loop
			curr_var = (P_BP_HTTPD_REQUEST_VAR) get_table_iter->data;
			if(!curr_var)
				break;

			// attempt to pull a filter set
			P_BP_HASH_TABLE_KEYED_ENTRY get_filter_set = BP_HTTPdRequestHandlerParameterFilterSetLookup
			(
				req->request_handler,
				BP_HTTPD_REQUEST_VAR_TYPE_GET,
				curr_var->name
			);

			// ensure we have a filter set
			if(!get_filter_set)
				break;

			// iterate though hashes
			BP_HASH_ITER_OPEN(get_filter_set, get_filter_entry)
			{

				// break if we have no entries
				if(!get_filter_entry)
					break;
				printf("\n GET FILTER KEY -> %s", get_filter_entry->key);

				if
				(
					!BP_HTTPdRequestHandlerFilterCheckVarAgainstFilter
					(
						(P_BP_HTTPD_REQUEST_VAR)                      curr_var,
						(P_BP_HTTPD_REQUEST_HANDLER_PARAMETER_FILTER) get_filter_entry->data
					)
				)
				{

					// if the filter check fails, exit immediately (don't run other checks, just fail immediately)
					return ERR_FAILURE;

				}



			} BP_HASH_ITER_CLOSE;


		} BP_HASH_ITER_CLOSE;

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run HTTP HEADER Filters %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(BP_HTTPdRequestContainsHEADER(req))
	{

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run HTTP COOKIE Filters %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(BP_HTTPdRequestContainsCOOKIE(req))
	{

	}

	// display the argument hreg at this point
	BP_HashRegDisplay(req->arg_hreg, BP_FALSE, BP_TRUE);

	/*
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Iterate Through Filters and Apply %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set lookup path to the filters table
	char *filter_set_lookup[] = {"filters", NULL};

	// iterate through filters set
	BP_HASH_ITER_FROM_LOOKUP_PATH_OPEN
	(
		req->request_handler->parameter_filters,
		(char **) &filter_set_lookup,
		filter_iter_entry
	)
	{

		// iterate through filter entries
		if(filter_iter_entry->data)
		{

			P_BP_HASH_TABLE_KEYED_ENTRY filter_set_table = (P_BP_HASH_TABLE_KEYED_ENTRY) filter_iter_entry->data;

		}

	} BP_HASH_ITER_FROM_LOOKUP_PATH_CLOSE;
	*/

	// return indicating success
	return ERR_SUCCESS;

}




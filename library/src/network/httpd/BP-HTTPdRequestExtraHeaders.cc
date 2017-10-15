/*
 * BP-HTTPdRequestExtraHeaders.cc
 *
 *  Created on: Dec 2, 2015
 *      Author: root
 */

#include "../../../include/BP-Main.h"

// Adds an extra response header to the request
BP_ERROR_T BP_HTTPdAddExtraResponseHeaderToRequest
(
		P_BP_HTTPD_REQUEST request,
		char             * response_header,
		char             * response_value
)
{

	// ensure we have basic parameters
	if(!request)
		return ERR_FAILURE;
	if(!response_header)
		return ERR_FAILURE;
	if(!response_value)
		return ERR_FAILURE;

	// ensure response header is reasonable/printable
	if(!BP_StringIsReasonablePrintableString(response_header, bpstrlen(response_header), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// ensure response value is reasonable printable
	if(!BP_StringIsReasonablePrintableString(response_value, bpstrlen(response_value), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;


	// after checks complete, calculate lengths
	size_t response_header_len = bpstrlen(response_header);
	size_t response_val_len    = bpstrlen(response_value);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Setup Initial Registry if Necessary %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create the hash table registry if necessary
	if(!request->extra_headers)
	{

		// create hash table registry
		request->extra_headers = BP_CreateHashTableRegistry();

		// set the headers path
		char * headers_table_path[] =
		{
			"headers",
			NULL
		};

		// attempt to add header to table
		BP_HashRegAddTableByPath(request->extra_headers, (char **) &headers_table_path);


	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Lookup Response (already-exists check)%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// set data lookup path
	char * pre_data_lookup_path[] =
	{
		"headers",
		response_header,
		NULL
	};

	// Attempt to get the value data.  Fail if it's already found.
	P_BP_HASH_TABLE_KEYED_ENTRY pre_header_value_lookup = BP_HashRegLookupTableEntryByPath(request->extra_headers, (char **) &pre_data_lookup_path);
	if(pre_header_value_lookup)
		return ERR_FAILURE;





	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Lookup Data and Fill %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set data lookup path
	char * data_add_lookup_path[] =
	{
		"headers",
		NULL
	};

	// attempt to add data here
	BP_HashRegAddDataToTableByPath
	(
			request->extra_headers,
			(char **) &data_add_lookup_path,
			response_header,
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
			NULL,
			response_val_len+4
	);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt 2nd Lookup to Ensure Process Completed %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to get the value data
	P_BP_HASH_TABLE_KEYED_ENTRY header_value_lookup = BP_HashRegLookupTableEntryByPath(request->extra_headers, (char **) &pre_data_lookup_path);
	if(!header_value_lookup)
		return ERR_FAILURE;

	// copy in the response data
	memcpy(header_value_lookup->data, response_value, response_val_len);

	// dbg: message in case you want to print-debug
	// printf("\n [+] HERE HERE: ADDED EXTRA REQUEST HEADER OK!");

	// return indicating success
	return ERR_SUCCESS;

}


/*
 * BP-JSON.h
 *
 *  Created on: Dec 7, 2015
 *      Author: root
 */

#ifndef INCLUDE_JAVASCRIPT_JSON_BP_JSON_H_
#define INCLUDE_JAVASCRIPT_JSON_BP_JSON_H_


// parse token element
typedef struct __BP_JSON_PARSE_TOKEN_ELEMENT {


	// this is set to BP_TRUE once the token has been initialized via forward parsing
	BP_BOOL token_init_ok;

	// -- type references for convenience/code clarity)

	// simple flag indicating that this is an object
	BP_BOOL is_object;

	// toggle indicating that this is an array
	BP_BOOL is_array;

	// set if this is a string
	BP_BOOL is_string;

	// set if this is a primitive
	BP_BOOL is_primitive;


	// -- Parent Information -----

	// indicator whether or not this token element has a parent
	BP_BOOL has_parent;

	// checks if parent is anonymous
	BP_BOOL parent_is_anonymous;

	// if it's anonymous this indicates whether or not labels have been applied
	BP_BOOL current_token_label_is_anonymous;


	// -- type references for convenience/code clarity)

	// simple flag indicating that this is an object
	BP_BOOL parent_is_object;

	// toggle indicating that this is an array
	BP_BOOL parent_is_array;

	// set if this is a string
	BP_BOOL parent_is_string;

	// set if this is a primitive
	BP_BOOL parent_is_primitive;


	// ---- Child Elements ------------------------------------------------------

	// check to see if we have children
	BP_BOOL has_children;

	// total count of immediate children directly below (does not count recursing children)
	size_t child_count;

	// if the child is just a string, this will be set to BP_TRUE
	BP_BOOL child_is_string_only;

	// if the child is a string, it's held here
	char *child_as_string;

	// if the child is only a primitive, this will be set to BP_TRUE
	BP_BOOL child_is_primitive_only;

	// child as a primitive
	char * child_as_primitive;

	// the start of the child tokens array within our local reference (only set if we have children).  Remember
	// that child parent lookups are into the global parse_state->token_
	jsmntok_t *child_tokens_start;


	// ---- Data Values (set by appropriate type/unset values are null) ---------

	// String value of the parent, only set if we have a parent.  If there
	// is no parent, this will be set to a random uuid value.
	char * parent_string_val;

	// if the parent is a primitive, this will be its primitive val
	char * parent_primitive_val;

	// current string value
	char * current_string_val;

	// current primitive value
	char * current_primitive_val;


	// --- path calculations --------------

	// path query to the current item
	char *path_query_to_current;

	// path query to the parent item
	char *path_query_to_parent;


	// --- token references ---------------

	// parent token if one is set
	jsmntok_t *parent_token;

	// current token
	jsmntok_t *current_token;


	// --- LinkL Pointer ----

	// if this object has a parent, this is set to a reference
	void *parent_token_element;

} BP_JSON_PARSE_TOKEN_ELEMENT, *P_BP_JSON_PARSE_TOKEN_ELEMENT;


// Parser state associated with JSON parsing.  Used to recurse through
// json buffer parses.  This is used only when parsing out json from
// a text representation.
typedef struct __BP_JSON_PARSE_STATE {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Basic Parser Info %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// jsmn parser
	jsmn_parser p;

	// tokens which were parsed out
	jsmntok_t * jsmn_tokens;

	// maximum number of tokens
	size_t jsmn_tokens_allocated_n;

	// parsed token count total
	size_t parsed_token_count;

	// tokens remaining to be parsed
	size_t parsed_tokens_remaining;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Internal Parser Information %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// buffer which is being parsed
	char *buffer;

	// size of the buffer being parsed
	size_t buffer_size;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Token Counts and Limits %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// maximum depth for current token allocation
	size_t max_depth_for_token_allocation;

	// the current depth of traversal
	size_t current_traversal_depth;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Token Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This stack is wound/unwound as a complete path stack.  It is only
	// valid while the current element is being accessed.  Recursion grows/shrinks
	// this stack, so be careful when accessing it.
	P_BP_JSON_PARSE_TOKEN_ELEMENT token_element_stack;

	// maximum number of tokens
	size_t token_element_stack_items_allocated_n;

	// current token index
	size_t current_token_element_index;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Parse General Indicators/Sanity Flags %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this is set to BP_TRUE if during parsing, any anonymous arrays
	// were detected.
	BP_BOOL parse_encountered_anonymous_arrays;

	// this is set to BP_TRUE if during parsing, any anonymous objects
	// were detected.
	BP_BOOL parse_encountered_anonymous_objects;

	// combined flag for both above for simplicity
	BP_BOOL parse_encountered_anonymous_arrays_or_objects;

	// this is set to BP_TRUE if during parsing, any primitives are encountered
	// without any keys.
	BP_BOOL parse_encountered_primitive_without_key;

	// this is set to BP_TRUE if during parsing, any strings are encountered
	// without any keys.
	BP_BOOL parse_encountered_string_without_key;

	// this is set to bp true if a string or primitive without a key is encountered
	BP_BOOL parse_encountered_string_or_privitive_without_key;

	// --- Duplicate Check Flags ----------------------------

	// this is set if the parse encountered duplicate keys in the same array
	BP_BOOL parse_encountered_duplicate_keys_in_the_same_array;

	// this is set if the parse encountered duplicate keys in the same object
	BP_BOOL parse_encountered_duplicate_keys_in_the_same_object;

	// this is set if either of the above are true
	BP_BOOL parse_encountered_duplicate_keys_in_array_or_object;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% State Pointers To Tokens %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// last token that passed through BP_JSONPushAdvanceParseState.
	jsmntok_t *last_token;

	// if the last token had a parent, a pointer is available here
	jsmntok_t *last_token_parent;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% State TQ Allocator for Easy Cleanup %%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// linkl associated with this parse
	P_BP_LINKL tq;

} BP_JSON_PARSE_STATE, *P_BP_JSON_PARSE_STATE;


// Main JSON functionality.
typedef struct __BP_JSON {

	// system hash table registry
	P_BP_HASH_TABLE_REGISTRY system_hreg;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Parse State (if JSON is parsed %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// state information relating to json.
	P_BP_JSON_PARSE_STATE parse_state;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Input and Output Registries %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// If this session is used to parse json, this registry will contain
	// the parsed json data.
	P_BP_HASH_TABLE_REGISTRY json_in_hreg;

	// If this data is being used to generate output json, this registry will contain
	// the prepared json data.
	P_BP_HASH_TABLE_REGISTRY json_out_hreg;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Parse Errors %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// When JSON is being parsed, this registry is populated with /errors, /warnings, /notice tables, each containing information
	// about the last parse.  Each parse attempt will destroy and re-create this table, so be sure to extract
	// data you want before attempting a second parse.
	P_BP_HASH_TABLE_REGISTRY parse_messages_reg;

	// total number of parse messages
	size_t total_parse_messages;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Output Indicators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// total number of registries appended to this json
	size_t number_of_registries_appended;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Prepared Output JSON %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this is populated by BP_JSONCreateOutput
	char * json_out;
	size_t json_out_len;

	// this is the tailqueue allocator used to prepare the json
	// output before it is finalized.
	P_BP_LINKL json_out_prepare_tq;


} BP_JSON, *P_BP_JSON;

// define strict jsmn
#define JSMN_STRICT

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% JSON Create / Destroy %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Create new json object
P_BP_JSON BP_JSONCreate();

// Create new json object from a parsable buffer.  Must check parse state for parse information after calling.  Returns
// a json handle whether parse fails or not.
P_BP_JSON BP_JSONCreateFromBuffer
(
	char  *buffer,
	size_t buff_len,
	size_t max_depth_for_token_allocation,
	size_t max_tokens_total
);

// Attempts to create JSON from a file.  Reads entire file and attempts to parse into JSON.
P_BP_JSON BP_JSONCreateFromFile(char *filename, size_t max_depth_for_token_allocation, size_t max_tokens_total);

// attempt to destroy a json object
BP_ERROR_T BP_JSONDestroy(P_BP_JSON json);

// destroys output buffers (dangling data)
BP_ERROR_T BP_JSONDestroyOutputBuffers(P_BP_JSON json);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Libjsmn Parser Interface %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/**
 * Parse JSON string and fill tokens.  This routine has been modified in
 * order to provide error output information and increase readability.
 *
 * Note: This must be defined here and not in B-LibJSMN.h simply because the BP_JSON struct is
 *       not defined at the time of BP-LiBJSMN.h loading.  Because we use that structure to
 *       track error/warning/notice information, the function definition had to be moved here.
 */
int jsmn_parse
(
	P_BP_JSON    json,
	jsmn_parser *parser,
	const char * js,
	size_t       len,
	jsmntok_t *  tokens,
	unsigned int num_tokens
);

/**
 * Create JSON parser over an array of tokens
 */
void jsmn_init
(
	P_BP_JSON    json,
	jsmn_parser *parser
);




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Parse JSON From Buffer %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Basic json string compare featured in the simple json example, but updated to work
// with additional checks.
BP_ERROR_T BP_JSONStringEq(const char *json, jsmntok_t *tok, const char *s);


// --- create and destroy parse state ----------

// Returns a parse state object to be used by the parser.  Errors/warnings/notices are
// tracked in the parent json structure.
P_BP_JSON_PARSE_STATE BP_JSONCreateInitialParseState
(
	P_BP_JSON json,
	char  *buffer,
	size_t buffer_size,
	size_t max_depth_for_token_allocation,
	size_t max_tokens_total
);

// destroy the parse state
BP_ERROR_T BP_JSONDestroyParseState(P_BP_JSON_PARSE_STATE parse_state);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Parse State Display Routines %%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// display the current parse state
BP_ERROR_T BP_JSONDisplayParseState(P_BP_JSON_PARSE_STATE parse_state);

// display the current parse state in terms of path queries
BP_ERROR_T BP_JSONDisplayParseStatePathsQueriesOnly(P_BP_JSON_PARSE_STATE parse_state);

// display a parse element
BP_ERROR_T BP_JSONDisplayParseElement(P_BP_JSON_PARSE_STATE parse_state, P_BP_JSON_PARSE_TOKEN_ELEMENT parse_element, BP_BOOL only_display_relevant = BP_TRUE);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% JSON Parser Error/Warn/Notice Gen Routines %%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// add a parse error to the parse output hash table registry (since buff position can be 0, we need to use -1 as an unset value)
BP_ERROR_T BP_JSONParseAddErrorMsg(P_BP_JSON json, P_BP_JSON_PARSE_TOKEN_ELEMENT token_element, char *message, int buff_position = -1, char bad_char = 0x00);

// add a parse warning to the parse output hash table registry
BP_ERROR_T BP_JSONParseAddWarningMsg(P_BP_JSON json, P_BP_JSON_PARSE_TOKEN_ELEMENT token_element, char *message, int buff_position = -1, char bad_char = 0x00);

// add a parse notice to the parse output hash table registry
BP_ERROR_T BP_JSONParseAddNoticeMsg(P_BP_JSON json, P_BP_JSON_PARSE_TOKEN_ELEMENT token_element, char *message, int buff_position = -1, char bad_char = 0x00);


// -- message display routines -------------------

// display notices
BP_ERROR_T BP_JSONDisplayParseNotices(P_BP_JSON json);

// display warnings
BP_ERROR_T BP_JSONDisplayParseWarnings(P_BP_JSON json);

// display errors
BP_ERROR_T BP_JSONDisplayParseErrors(P_BP_JSON json);

// display all the parser messages
BP_ERROR_T BP_JSONDisplayParseMessages(P_BP_JSON json);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Token Data Extraction Routines %%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to get a string value from a JSMN_STRING type token. Parse state is utilized due to
// it containing a copy of the buffer being parsed.  Returned value must be "reasonable/printable" or
// it will be destroyed after extraction and no value will be returned.  Returned value is bpcalloced() and
// must be bpfreed().
char * BP_JSONGetStringValueFromJSMNToken(P_BP_JSON_PARSE_STATE parse_state, jsmntok_t *token);


// Attempts to retrieve a string value represenation of a primitive in a manner similar to BP_JSONGetStringValueFromJSMNToken()
char * BP_JSONGetPrimitiveValueAsStringValueFromJSMNToken(P_BP_JSON_PARSE_STATE parse_state, jsmntok_t *token);


// generates a bprealloced array path which can be used for adding into our json hreg.  The path output will be stored in the token itself.
BP_ERROR_T BP_JSONGeneratePathBufferRecursivelyFromToken(P_BP_JSON_PARSE_STATE parse_state, P_BP_JSON_PARSE_TOKEN_ELEMENT token_element);

// recurses parents to build path
BP_ERROR_T BP_JSONGeneratePathBufferRecursivelyFromToken_r(P_BP_JSON_PARSE_STATE parse_state, P_BP_JSON_PARSE_TOKEN_ELEMENT token_element, P_BP_LINKL path_builder_tq);


// -- push and pop parse state ---

// push parse state from current state (this is primarily used within BP_JSONParseFromBuffer().
BP_ERROR_T BP_JSONPushAdvanceParseState(P_BP_JSON_PARSE_STATE parse_state);




// Attempt to parse JSON from a provided buffer.  Buffer must be reasonable/printable string.  Buffer
// can contain \n or \r and parse will complete ok.  The max_depth_for_token_allocation is used to
// heap allocate a buffer.  If it's too large, it can cause the application to run out of memory, so
// be careful when setting that parameter.  Depth relates to how forward a search can run.  Max tokens
// indicates how many tokens can possibly be returned.  A returned parse state will be stored in the
// json session and will contain data relative to what's contained in json_out.  This includes parse
// sanity indicators which can be used to return potential problems to end users.
BP_ERROR_T BP_JSONParseFromBuffer
(
	P_BP_JSON json,
	char *buffer,
	size_t buffer_size,
	size_t max_depth_for_token_allocation,
	size_t max_tokens_total
);


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
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Add Nodes and Tables (fresh add/no replace) %%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Add node by path
BP_ERROR_T BP_JSONAddNodeByPath
(
		P_BP_JSON json,
		char    **path,
		char    * data,
		size_t    data_len,
		BP_BOOL   base64_encode_data = BP_TRUE
);

// Add node by path (lookup query is in the form: /index1/whatever_index2/something_three/target data.
BP_ERROR_T BP_JSONAddNodeByPathQuery
(
		P_BP_JSON json,
		char    * lookup_query,
		char    * data,
		size_t    data_len,
		BP_BOOL   base64_encode_data = BP_TRUE
);

// Add table by path.
BP_ERROR_T BP_JSONAddTableByPath
(
		P_BP_JSON json,
		char    **path,
		BP_BOOL   table_is_anonymous
);

// Add table by path query.
BP_ERROR_T BP_JSONAddTableByPathQuery
(
		P_BP_JSON json,
		char    * lookup_query,
		BP_BOOL   table_is_anonymous = BP_FALSE
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Remove Nodes and Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Remove a node by path.
BP_ERROR_T BP_JSONRemoveNodeByPath
(
		P_BP_JSON json,
		char    **path
);

// Remove a node by path query.
BP_ERROR_T BP_JSONRemoveNodeByPathQuery
(
		P_BP_JSON json,
		char    * lookup_query
);

// Remove a table by path.
BP_ERROR_T BP_JSONRemoveTableByPath
(
		P_BP_JSON json,
		char    **path
);

// Remove a table by path query.
BP_ERROR_T BP_JSONRemoveTableByPathQuery
(
		P_BP_JSON json,
		char    * lookup_query
);


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
		BP_BOOL   base64_encode_data = BP_TRUE
);

// Replace node by path (lookup query is in the form: /index1/whatever_index2/something_three/target data.
BP_ERROR_T BP_JSONReplaceNodeByPathQuery
(
		P_BP_JSON json,
		char    * lookup_query,
		char    * data,
		size_t    data_len,
		BP_BOOL   base64_encode_data = BP_TRUE
);

// Replace table by path. (Delete/recreate)
BP_ERROR_T BP_JSONReplaceTableByPath
(
		P_BP_JSON json,
		char    **path,
		BP_BOOL   table_is_anonymous
);

// Replace table by path query.  (Delete/recreate)
BP_ERROR_T BP_JSONReplaceTableByPathQuery
(
		P_BP_JSON json,
		char    * lookup_query,
		BP_BOOL   table_is_anonymous = BP_FALSE
);





// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Hash Registry Append Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to parse output hreg and create json.  If the from_hreg variable is set, the
// json generator use that registry instead of the internal builtin registry.
BP_ERROR_T BP_JSONAppendHReg
(
		P_BP_JSON                 json,
		char                    * hreg_json_name,
		P_BP_HASH_TABLE_REGISTRY  hreg,
		BP_BOOL                   include_data,
		BP_BOOL                   base64_encode_bin_data = BP_TRUE
);


// Attempts to append a hreg to json at a path
BP_ERROR_T BP_JSONAppendHRegByTablePath
(
		P_BP_JSON                 json,
		P_BP_HASH_TABLE_REGISTRY  hreg,
		char                     *new_appended_table_name,
		char                    **initial_append_path,
		BP_BOOL                   include_data,
		BP_BOOL                   base64_encode_bin_data = BP_TRUE
);

// Attempts to append a hreg to json at a path, using /path/query/lookup.
BP_ERROR_T BP_JSONAppendHRegByTablePathQuery
(
		P_BP_JSON                 json,
		P_BP_HASH_TABLE_REGISTRY  hreg,
		char                     *new_appended_table_name,
		char                     *initial_append_path_query,
		BP_BOOL                   include_data,
		BP_BOOL                   base64_encode_bin_data = BP_TRUE
);


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
		BP_BOOL    base64_encode_bin_data = BP_TRUE
);


// Attempts to append a hreg to json at a path
BP_ERROR_T BP_JSONAppendByTablePath
(
		P_BP_JSON  json,
		P_BP_JSON  json_appending,
		char      *new_appended_table_name,
		char     **initial_append_path,
		BP_BOOL    include_data,
		BP_BOOL    base64_encode_bin_data = BP_TRUE
);

// Attempts to append a hreg to json at a path, using /path/query/lookup.
BP_ERROR_T BP_JSONAppendByTablePathQuery
(
		P_BP_JSON  json,
		P_BP_JSON  json_appending,
		char      *new_appended_table_name,
		char      *initial_append_path_query,
		BP_BOOL    include_data,
		BP_BOOL    base64_encode_bin_data = BP_TRUE
);

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
		BP_BOOL    base64_encode_bin_data = BP_TRUE
);


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
		BP_BOOL    base64_encode_bin_data = BP_TRUE
);

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
		BP_BOOL    base64_encode_bin_data = BP_TRUE
);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Recursing Append Routine %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// As of the time of this comment, BP_JSONAppendHReg_r is used by the following:
//
//
// BP_JSONAppendHReg
// BP_JSONAppendHRegByTablePath
// BP_JSONAppendHRegByTablePathQuery
// BP_JSONAppend
// BP_JSONAppendByTablePath
// BP_JSONAppendByTablePathQuery


// -- recursing append routing ---

// Append hash registry recurse.  Called directly from BP_JSONAppendHReg
BP_ERROR_T BP_JSONAppendHReg_r
(
		P_BP_JSON json,
		P_BP_HASH_TABLE_KEYED_ENTRY hreg_entry,
		char                      **table_add_start_path,
		BP_BOOL                   include_data,
		BP_BOOL                   base64_encode_bin_data
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Lookup Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// lookup a json table entry by path pointer set
P_BP_HASH_TABLE_KEYED_ENTRY BP_JSONLookupEntryByPath(P_BP_JSON json, char **path);

// return here and add lookups after you add them to the hashtable.c
// BP_ERROR_T BP_JSONLookup
P_BP_HASH_TABLE_KEYED_ENTRY BP_JSONLookupEntryByPathQuery(P_BP_JSON json, char *entry_path_lookup_query);

// lookup entry by table index and table path query
P_BP_HASH_TABLE_KEYED_ENTRY BP_JSONLookupEntryByTableIndexAndTablePathQuery(P_BP_JSON json, char *table_lookup_query, char *table_data_index);

// Attempts to lookup an entry from a table path and entry index (calculated with BP_HASH_ITER_OPEN loop).  If the
// path is set to "/", it will start at the top hive entry of json->json_out.
P_BP_HASH_TABLE_KEYED_ENTRY BP_JSONLookupEntryByTablePathQueryAndEntryIndex(P_BP_JSON json, char *table_lookup_query, size_t entry_index);

// Attempts to lookup the top table entry for the json parsed info
P_BP_HASH_TABLE_KEYED_ENTRY BP_JSONLookupEntryTopTable(P_BP_JSON json);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Specialized Lookup Routines (lookup-as-type) %%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// access an entry as a string (do not free result)
char * BP_JSONAccessEntryAsReasonableString(P_BP_HASH_TABLE_KEYED_ENTRY entry, BP_BOOL allow_newlines, BP_BOOL allow_carriage_returns);

// --- long conversions --------

// access an entry as a unsigned long (conversion status is stored in convert_status_in_out)
unsigned long BP_JSONAccessEntryAsUnsignedLong(P_BP_HASH_TABLE_KEYED_ENTRY entry, BP_BOOL * convert_status_in_out);

// access an entry as long
long BP_JSONAccessEntryAsLong(P_BP_HASH_TABLE_KEYED_ENTRY entry, BP_BOOL * convert_status_in_out);


// --- long long conversions (double length) ---

// access an entry as a unsigned long long (conversion status is stored in convert_status_in_out)
unsigned long long BP_JSONAccessEntryAsUnsignedLongLong(P_BP_HASH_TABLE_KEYED_ENTRY entry, BP_BOOL * convert_status_in_out);

// access an entry as long long
long long BP_JSONAccessEntryAsLongLong(P_BP_HASH_TABLE_KEYED_ENTRY entry, BP_BOOL * convert_status_in_out);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Keyed Entry To Path Reverse Lookups %%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This is a simple frontend to P_HashRegEntryToPathQuery
char * BP_JSONEntryToPathQuery(P_BP_HASH_TABLE_KEYED_ENTRY entry);

// Attemmpts to convert a hash entry to path.  Result is bp_calloced and must be free'd
// with BP_DestroyPtrArray() and bpfree()
BP_HASH_TABLE_TRAVERSAL_PATH BP_JSONEntryToPath(P_BP_HASH_TABLE_KEYED_ENTRY entry);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create JSON Buffer By Crawling Registry %%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// create output from a json
BP_ERROR_T BP_JSONCreateOutput
(
		P_BP_JSON json,
		BP_BOOL   base64_encode_bin_data = BP_TRUE,
		BP_BOOL   encapsulate_result = BP_TRUE
);


// create output from a json
BP_ERROR_T BP_JSONCreateOutputFromPathQuery
(
	P_BP_JSON json,
	char *    lookup_path_query,
	BP_BOOL   base64_encode_bin_data
);

// -- recursing output generator -- do not call directly --

// re-entrant create-output routine (called directly from BP_JSONCreateOutput)
BP_ERROR_T BP_JSONCreateOutput_r
(
		P_BP_JSON                   json,
		P_BP_HASH_TABLE_KEYED_ENTRY table_r,
		size_t                      tab_depth,
		P_BP_LINKL           	    out_json_tq,
		BP_BOOL                     base64_encode_bin_data = BP_TRUE,
		BP_BOOL                     skip_outermost_table_iter = BP_FALSE
);


// -- output item tailqueue items ---

// add a keyed value
BP_ERROR_T BP_JSONCreateOutput_AddKeyVal
(
	P_BP_LINKL out_json_tq,
	size_t tab_depth,
	P_BP_HTKE item,
	BP_BOOL base64_encode_bin_data = BP_TRUE
);

// open and close tables
BP_ERROR_T BP_JSONCreateOutput_AddTableOpen(P_BP_LINKL out_json_tq, size_t tab_depth);
BP_ERROR_T BP_JSONCreateOutput_AddTableClose(P_BP_LINKL out_json_tq, size_t tab_depth);




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Finalize Output %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// generate a json buffer from local tq (destroys old if already set)
BP_ERROR_T BP_JSONGenerateJSONBufferFromLocalTQ(P_BP_JSON json);

// Finalizes json output buffer.  Encapsulating the result will ensure the json is nested with encompassing curly brackets.
BP_ERROR_T BP_JSONFinalizeOutput(P_BP_JSON json, BP_BOOL encapsulate_result = BP_TRUE);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Display JSON information.
BP_ERROR_T BP_JSONDisplay(P_BP_JSON json);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% JSON Cloning %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to clone json from a path query
P_BP_JSON BP_JSONCloneFromPathQuery
(
		P_BP_JSON  json_to_clone_from,
		char     * lookup_query,
		BP_BOOL    base64_encode_bin_data,
		size_t     max_depth_for_token_allocation,
		size_t     max_tokens_total
);


#endif /* INCLUDE_JAVASCRIPT_JSON_BP_JSON_H_ */

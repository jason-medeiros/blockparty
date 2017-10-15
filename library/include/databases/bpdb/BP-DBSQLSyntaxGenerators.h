/*
 * BP-DBSQLSGerators.h
 *
 *  Created on: Nov 19, 2015
 *      Author: root
 */

#ifndef INCLUDE_DATABASES_BPDB_BP_DBSQLSYNTAXGENERATORS_H_
#define INCLUDE_DATABASES_BPDB_BP_DBSQLSYNTAXGENERATORS_H_



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%  Open or Close Syntax Generation Session %%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// open a syntax generator session
P_BPDB_SQLGEN BP_DBSGOpen(P_BPDB bpdb);

// close the syntax generator
BP_ERROR_T BP_DBSGClose(P_BPDB_SQLGEN syngen);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Query Modification Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Generates a CREATE TABLE sql query from the table specified by table_name.  Table must
// be found within the associated bpdb handle.
//
// finalizes query: yes
//
BP_ERROR_T BP_DBSGCreateTable(P_BPDB_SQLGEN syngen, char *table_name);

// Generates a select query
BP_ERROR_T BP_DBSGSelectFrom
(
	P_BPDB_SQLGEN        syngen,
	P_BPDB_SQLGEN_SELECT select_info
);

// Generates an insert query from the
BP_ERROR_T BP_DBSGInsertInto(P_BPDB_SQLGEN syngen, BPDB_SELECTOR_TYPE out_db_selector, char *table_name, size_t max_records);


// dispatch a select query
BP_ERROR_T BP_DBSGDispatchSelect(P_BPDB_SQLGEN syngen, P_BPDB_SQLGEN_SELECT select_info);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Fetch Result %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Fetches a result, results are automatically destroyed on fetch so be careful
// to copy data rather than reference it directly.  The create_json_reg parameter
// is used to create a registry of usable data which can be plugged into the JSON
// subsystem.
BP_ERROR_T BP_DBSGFetchResult
(
	P_BPDB_SQLGEN        syngen,
	P_BPDB_SQLGEN_SELECT select_info,
	BP_BOOL              create_json_reg = BP_TRUE
);

// Get current result value by field name.  Returned result is
// standard mysql/mariadb bind structure.
MYSQL_BIND * BP_DBSGLookupResultByFieldName
(
	P_BPDB_SQLGEN        syngen,
	P_BPDB_SQLGEN_SELECT select_info,
	char               * field_name
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% JSON Registry %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Adds a json result value.  DO NOT USE THIS OUTSIDE OF BP_DBSGUpdateJSONResultRegistry.  This
// is a utility macro and is intended only to make code within BP_DBSGUpdateJSONResultRegistry easier
// to read/understand.
#define BPDB_ADD_JSON_RES_VAL(path, name, value, value_len) \
	BP_HashRegAddDataToTableByPath                             \
	(                                                          \
			select_info->fetched_record_json_hreg,             \
			(char **) path,                                              \
			name,                                              \
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,        \
			NULL,                                              \
			value_len                                          \
	);                                                         \
	memcpy(&lookup_stack, &path, sizeof(path));                \
	lookup_stack[2] = name;                                    \
	data_lookup_entry = BP_HashRegLookupTableEntryByPath       \
	(                                                          \
		select_info->fetched_record_json_hreg,                 \
		(char **) &lookup_stack                                \
	);                            \
	if(!data_lookup_entry)\
		break;\
	memcpy(data_lookup_entry->data, value, value_len);	\
	BP_ZERO_STACK(lookup_stack);



// Attempts to update the json result registry.
BP_ERROR_T BP_DBSGUpdateJSONResultRegistry
(
	P_BPDB_SQLGEN        syngen,
	P_BPDB_SQLGEN_SELECT select_info
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Sanity / Check Functions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// set maximum select fields count here (it's arbitrary but used when checking
// field counts to determine if a developer is being ridiculous)
#define BPDB_MAX_SELECT_FIELDS_COUNT 256

// -- begin edits ---

// Checks a field name list and make sure elements match up
BP_ERROR_T BP_DBCheckSelectFieldNameList
(
	P_BPDB     bpdb,
	P_BP_LINKL field_name_list,
	P_BP_LINKL table_name_list
);

// Checks joins to make sure they make sense from a logical perspective.
BP_ERROR_T BP_DBCheckJoinElements
(
	P_BPDB     bpdb,
	P_BPDB_SELECT_JOIN joins,
	size_t joins_n,
	P_BP_LINKL table_name_list
);

// Checks "where" clause elements to make sure they make sense from a logical perspective.
BP_ERROR_T BP_DBCheckSelectClauseElements
(
	P_BPDB              bpdb,
	P_BPDB_SELECT_WHERE wheres,
	size_t              wheres_n,
	P_BP_LINKL          field_name_list
);

// -- end edits 10/13 ---


// This is a "master" routine that runs all checks necessary for
// generating a select info.  It analyzes all elements in a
// BPDB_SQLGEN_SELECT item, and returns ERR_FAILURE on bad
// values.  You should only ever need to call this function
// once to verify values.

BP_ERROR_T BP_DBCheckSelectInfoAndGenerateMetadataLists
(
		P_BPDB_SQLGEN        syngen,
		P_BPDB_SQLGEN_SELECT select_info
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Schema Check Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to check fieldnames against schema
BP_ERROR_T BP_DBCheckFieldNamesAgainstSchema(P_BPDB_SQLGEN syngen, P_BP_LINKL field_name_list);

// attempt to check tablenames against schema
BP_ERROR_T BP_DBCheckTableNamesAgainstSchema(P_BPDB_SQLGEN syngen, P_BP_LINKL table_name_list);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Parameter Addition %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Adds a select parameter to a select info query.  This shouldn't be used directly.  This is used internally
// by BP_DBSGSelectFrom to add parameters inline.
BP_ERROR_T BP_DBAddSelectWhereParameterToSelectInfo
(
	P_BPDB_SQLGEN        syngen,
	P_BPDB_SQLGEN_SELECT select_info,
	P_BPDB_SELECT_WHERE  select_where
);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Result Addition %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to add a result to the select info structure.  Used internally.
BP_ERROR_T BP_DBAddSelectResultToSelectInfo
(
	P_BPDB_SQLGEN        syngen,
	P_BPDB_SQLGEN_SELECT select_info,
	char * select_field_name
);


// Attempts to create a hash table within the select_info structure by associating
// potentially retrieved tables
BP_ERROR_T BP_DBAddSelectResultReferences
(
	P_BPDB_SQLGEN        syngen,
	P_BPDB_SQLGEN_SELECT select_info
);


// Attempt to add a result entry to match with a result
BP_ERROR_T BP_DBAddSelectResultReferenceToResultHashTable
(
	P_BPDB_SQLGEN        syngen,
	P_BPDB_SQLGEN_SELECT select_info,
	char               * select_field_name,
	MYSQL_BIND *         result_entry
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Dataset Generators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Creates a searchable list of tables which represent ALL tables which could be selected
// from in the query.
P_BP_LINKL BP_DBGenerateTableListFromSelectInfo(P_BPDB_SQLGEN_SELECT select_info);

// Generates a searchable list of all field names which represent all tables which could be selected
P_BP_LINKL BP_DBGenerateFieldNameListFromSelectInfo
(
		P_BPDB_SQLGEN_SELECT select_info,
		P_BP_LINKL           table_name_list
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Destroy / Free Dangling Data Routines %%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This cleans out all dangling data that could possibly be generated as a result of
// the attempted or comleted prepare of a select statement.  It should be noted that
// all potentially dangling memory isn't allocated in the master hash table.  This is
// to prevent any need for contentious locking of the table itself.
BP_ERROR_T BP_DBSelectInfoCleanResidualData
(
		P_BPDB_SQLGEN_SELECT select_info,
		BP_BOOL              destroy_bind_results    = BP_TRUE,
		BP_BOOL              destroy_bind_parameters = BP_TRUE,
		BP_BOOL              destroy_field_name_list = BP_TRUE,
		BP_BOOL              destroy_table_name_list = BP_TRUE
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// display select info
BP_ERROR_T BP_DBDisplaySelectInfo(P_BPDB_SQLGEN_SELECT select_info);

// display select where info
BP_ERROR_T BP_DBDisplaySelectInfoSelectWhere(P_BPDB_SELECT_WHERE select_where, size_t tab_depth);

// display select join info
BP_ERROR_T BP_DBDisplaySelectInfoSelectJoin(P_BPDB_SELECT_JOIN join_on, size_t tab_depth);

// display mariadb parameters for a provided select info
BP_ERROR_T BP_DBDisplaySelectInfoMariadbBindParameters(P_BPDB_SQLGEN_SELECT select_info);

// display mariadb parameters for a provided select info
BP_ERROR_T BP_DBDisplaySelectInfoMariadbBindParameter(MYSQL_BIND * bind_parameter, size_t tab_depth,  size_t parameter_index = (size_t) -1);

// display mariadb results for a provided select info
BP_ERROR_T BP_DBDisplaySelectInfoMariadbBindResults(P_BPDB_SQLGEN_SELECT select_info);

// display mariadb results for a provided select info
BP_ERROR_T BP_DBDisplaySelectInfoMariadbBindResult(MYSQL_BIND * bind_result, size_t tab_depth, size_t result_index);

#endif /* INCLUDE_DATABASES_BPDB_BP_DBSQLSYNTAXGENERATORS_H_ */

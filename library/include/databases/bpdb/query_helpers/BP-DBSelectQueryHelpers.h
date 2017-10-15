#ifndef __BP_DBSELECTQUERYHELPERS_H
#define __BP_DBSELECTQUERYHELPERS_H


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Select-Query Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Initializes/Creates Select Info. (creates relevant structures)
BP_ERROR_T BP_DBSyntaxGenSelectInfoInit(P_BPDB_SQLGEN syngen);

// Frees/destroys any dangling sql information that might be present in the
// current allocator.
BP_ERROR_T BP_DBSyntaxGenSelectInfoDestroy(P_BPDB_SQLGEN syngen);

// -- field names --

// Attempt to add a field name.
BP_ERROR_T BP_DBSyntaxGenSelectInfoAddFieldName(P_BPDB_SQLGEN syngen, char *field);

// adds a set of fields to the current select info structure
BP_ERROR_T BP_DBSyntaxGenSelectInfoAddFieldNames(P_BPDB_SQLGEN syngen, char **fields);

// -- primary table selection --

// sets the primary selection table (not a joined table, must be set, must be valid)
BP_ERROR_T BP_DBSyntaxGenSelectInfoSetPrimaryTable(P_BPDB_SQLGEN syngen, char *primary_select_table);


// -- where clauses --

// Adds a where clause to the select settings based on input and type.  Performs sanitization and
// corruption detection before adding.
BP_ERROR_T BP_DBSyntaxGenSelectInfoAddWhereClause
(
	P_BPDB_SQLGEN                syngen,
	BPDB_SELECT_WHERE_VALUE_TYPE type,
	char *                       conjunction,
	char *                       field_name,
	char *                       limiter,
	char *                       value,
	size_t                       value_len
);


// -- joins --

// Adds a join part to the select settings based on input and type.  Performs sanitization and
// corruption detection before adding.
BP_ERROR_T BP_DBSyntaxGenSelectInfoAddJoinPart
(
		P_BPDB_SQLGEN syngen,
		BPDB_SELECT_JOIN_TYPE type,
		char *table_name,
		char *front_join,
		char *comparison_operator,
		char *tail_join
);


// -- limits --

// Set limit-by clause.
BP_ERROR_T BP_DBSyntaxGenSelectInfoSetLimitClause
(
		P_BPDB_SQLGEN syngen,
		size_t limit_low,
		size_t limit_high
);

// -- order --

BP_ERROR_T BP_DBSyntaxGenSelectInfoSetOrderByClause
(
		P_BPDB_SQLGEN syngen,
		char *field,
		char *sort_designator
);

// -- group by --

// this intends to add a group by clause to the selection
// todo: flush this out and get it working.
BP_ERROR_T BP_DBSyntaxGenSelectInfoAddGroupByClause
(
		P_BPDB_SQLGEN syngen,
		char *field_name
);


// if no select info structure is passed into the select-from
BP_ERROR_T BP_DBSyntaxGenSelectInfoStructFromInternalRegistries(P_BPDB_SQLGEN syngen);



#endif /* BP_DBSELECTQUERYHELPERS_H */

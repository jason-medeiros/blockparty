/*
 * BP-DBIntelSchema.h
 *
 *  Created on: Aug 9, 2015
 *      Author: root
 */

#ifndef __BP_DB_INTEL_SCHEMA_H
#define __BP_DB_INTEL_SCHEMA_H


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Search Engines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#define BP_DB_SQLITE_CREATE_TABLE_SEARCH_TERM_GROUPS\
	"\n CREATE TABLE IF NOT EXISTS search_term_groups "\
	"\n ("\
	"\n    id                              INTEGER PRIMARY KEY, "\
	"\n    user_id                         INTEGER,  "\
	"\n    group_name                      TEXT,     "\
	"\n    group_description               TEXT,     "\
	"\n    created_ts                      INTEGER   "\
	"\n );"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% JIT Matches %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// web_jit_expression_groups
#define BP_DB_SQLITE_CREATE_TABLE_WEB_JIT_EXPRESSION_GROUPS\
	"\n CREATE TABLE IF NOT EXISTS web_jit_expression_groups "\
	"\n ("\
	"\n    id                              INTEGER PRIMARY KEY, "\
	"\n    user_id                         INTEGER,  "\
	"\n    group_name                      TEXT,     "\
	"\n    group_description               TEXT,     "\
	"\n    created_ts                      INTEGER   "\
	"\n );"

// web_jit_expression_matches
#define BP_DB_SQLITE_CREATE_TABLE_WEB_JIT_EXPRESSION_MATCHES\
	"\n CREATE TABLE IF NOT EXISTS web_jit_expression_matches"\
	"\n ("\
	"\n    id                              INTEGER PRIMARY KEY, "\
	"\n    group_id                        INTEGER,  "\
	"\n    test_id                         INTEGER,  "\
	"\n    result_id                       INTEGER,  "\
	"\n    sha1                            TEXT,     "\
	"\n    matched                         TEXT,     "\
	"\n    insert_ts                       INTEGER   "\
	"\n );"\

// web_jit_expression_tests
#define BP_DB_SQLITE_CREATE_TABLE_WEB_JIT_EXPRESSION_TESTS\
	"\n CREATE TABLE IF NOT EXISTS web_jit_expression_tests"\
	"\n ("\
	"\n    id                              INTEGER PRIMARY KEY, "\
	"\n    user_id                         INTEGER,  "\
	"\n    group_id                        INTEGER,  "\
	"\n    expression                      TEXT,     "\
	"\n    description                     TEXT,     "\
	"\n    enabled                         INTEGER,  "\
	"\n    created_ts                      INTEGER   "\
	"\n );"\






#endif /* INCLUDE_DATABASES_BPDB_SCHEMA_INTEL_BP_DBINTELSCHEMA_H_ */

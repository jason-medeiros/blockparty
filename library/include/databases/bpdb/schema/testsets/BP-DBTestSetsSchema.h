/*
 * BP-DBTestSetsSchema.h
 *
 *  Created on: Aug 10, 2015
 *      Author: root
 */

#ifndef __BP_DBTEST_SETS_SCHEMA_H
#define __BP_DBTEST_SETS_SCHEMA_H


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Test Sets Schema %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#define BP_DB_SQLITE_CREATE_TABLE_VALIDATION_TESTS\
	"\n CREATE TABLE IF NOT EXISTS validation_tests "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    user_id                          INTEGER,  "\
	"\n    group_id                         INTEGER,  "\
	"\n    url_fetch                        TEXT,     "\
	"\n    post_args                        TEXT,     "\
	"\n    regexp_match                     TEXT,     "\
	"\n    count_invoked                    INTEGER,  "\
	"\n    created_ts                       INTEGER   "\
	"\n );"


#define BP_DB_SQLITE_CREATE_TABLE_VALIDATION_TEST_GROUPS\
	"\n CREATE TABLE IF NOT EXISTS validation_test_groups "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    user_id                          INTEGER,  "\
	"\n    group_name                       TEXT,     "\
	"\n    group_description                TEXT,     "\
	"\n    created_ts                       INTEGER   "\
	"\n );"


#endif /* __BP_DBTEST_SETS_SCHEMA_H */

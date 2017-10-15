/*
 * BP-DBSecuritySchema.h
 *
 *  Created on: Aug 9, 2015
 *      Author: root
 */

#ifndef __BP_DBSECURITY_H
#define __BP_DBSECURITY_H


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% XSS Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#define BP_DB_SQLITE_CREATE_TABLE_XSS\
	"\n CREATE TABLE IF NOT EXISTS xss "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    result_id                        INTEGER,  "\
	"\n    http_get_input                   TEXT,     "\
	"\n    http_post_input                  TEXT,     "\
	"\n    commit_ts                        INTEGER   "\
	"\n );"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% SQLi Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#define BP_DB_SQLITE_CREATE_TABLE_SQL_MESSAGES\
	"\n CREATE TABLE IF NOT EXISTS sql_messages "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    result_id                        INTEGER,  "\
	"\n    http_get_input                   TEXT,     "\
	"\n    http_post_input                  TEXT,     "\
	"\n    msql_test_matches                TEXT,     "\
	"\n    mysql_test_matches               TEXT,     "\
	"\n    oracle_test_matches              TEXT,     "\
	"\n    odbc_test_matches                TEXT,     "\
	"\n    postgres_test_matches            TEXT,     "\
	"\n    engine_matches                   TEXT,     "\
	"\n    commit_ts                        INTEGER   "\
	"\n );"

#define BP_DB_SQLITE_CREATE_TABLE_SQLI\
	"\n CREATE TABLE IF NOT EXISTS sqli "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    result_id                        INTEGER,  "\
	"\n    request_log_id                   INTEGER,  "\
	"\n    request_time_to_complete         INTEGER,  "\
	"\n    http_get                         TEXT,     "\
	"\n    http_post                        TEXT,     "\
	"\n    http_cookie                      TEXT,     "\
	"\n    commit_ts                        INTEGER   "\
	"\n );"



#endif /* __BP_DBSECURITY_H */

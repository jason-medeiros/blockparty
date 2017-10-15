#ifndef __BP_DBRESULTS_SCHEMA_H
#define __BP_DBRESULTS_SCHEMA_H

// SQLite to create results table.
#define BP_DB_SQLITE_CREATE_TABLE_RESULTS\
	"\n CREATE TABLE IF NOT EXISTS results "\
	"\n ("\
	"\n    id                              INTEGER PRIMARY KEY, "\
	"\n    assessment_id                   INTEGER, "\
	"\n    type                            INTEGER, "\
	"\n    sha1                            TEXT UNIQUE, "\
	"\n    href                            TEXT, "\
	"\n    host                            INTEGER, "\
	"\n    base                            INTEGER, "\
	"\n    path                            INTEGER, "\
	"\n    script                          INTEGER, "\
	"\n    status                          INTEGER, "\
	"\n    update_resolution_status        INTEGER, "\
	"\n    origin_type_form_or_anchor_code INTEGER, "\
	"\n    commit_timestamp                INTEGER, "\
	"\n    audit_start_timestamp           INTEGER, "\
	"\n    audit_completed_timestamp       INTEGER, "\
	"\n    spidered                        INTEGER, "\
	"\n    spider_parent_result_id         INTEGER, "\
	"\n    spidered_timestamp              INTEGER, "\
	"\n    hits                            INTEGER, "\
	"\n    disabled                        INTEGER, "\
	"\n    disabled_code                   INTEGER, "\
	"\n    disabled_path                   INTEGER, "\
	"\n    disabled_path_code              INTEGER, "\
	"\n    disabled_host                   INTEGER, "\
	"\n    disabled_host_code              INTEGER, "\
	"\n    disabled_base                   INTEGER, "\
	"\n    disabled_base_code              INTEGER, "\
	"\n    disabled_script                 INTEGER, "\
	"\n    disabled_script_code            INTEGER  "\
	"\n );"\

// result base urls
#define BP_DB_SQLITE_CREATE_TABLE_RESULT_BASE_URIS\
	"\n CREATE TABLE IF NOT EXISTS result_base_uris "\
	"\n ("\
	"\n    id                              INTEGER PRIMARY KEY, "\
	"\n    sha1                            TEXT UNIQUE, "\
	"\n    base_href                       TEXT, "\
	"\n    hits                            INTEGER, "\
	"\n    initial_result_id               INTEGER, "\
	"\n    created_ts                      INTEGER, "\
	"\n    disabled                        INTEGER, "\
	"\n    disabled_code                   INTEGER, "\
	"\n    brute_status                    INTEGER "\
	"\n );"\

// result path urls
#define BP_DB_SQLITE_CREATE_TABLE_RESULT_PATH_URIS\
	"\n CREATE TABLE IF NOT EXISTS result_path_uris "\
	"\n ("\
	"\n    id                              INTEGER PRIMARY KEY, "\
	"\n    sha1                            TEXT UNIQUE, "\
	"\n    path_href                       TEXT, "\
	"\n    hits                            INTEGER, "\
	"\n    initial_result_id               INTEGER, "\
	"\n    created_ts                      INTEGER, "\
	"\n    disabled                        INTEGER, "\
	"\n    disabled_code                   INTEGER, "\
	"\n    brute_status                    INTEGER "\
	"\n );"\

// result script urls
#define BP_DB_SQLITE_CREATE_TABLE_RESULT_SCRIPT_URIS\
	"\n CREATE TABLE IF NOT EXISTS result_script_uris "\
	"\n ("\
	"\n    id                              INTEGER PRIMARY KEY, "\
	"\n    sha1                            TEXT UNIQUE, "\
	"\n    script_href                     TEXT, "\
	"\n    hits                            INTEGER, "\
	"\n    initial_result_id               INTEGER, "\
	"\n    created_ts                      INTEGER, "\
	"\n    disabled                        INTEGER, "\
	"\n    disabled_code                   INTEGER "\
	"\n );"\

// result hosts table.
#define BP_DB_SQLITE_CREATE_TABLE_RESULT_HOSTS\
	"\n CREATE TABLE IF NOT EXISTS result_hosts "\
	"\n ("\
	"\n    id                              INTEGER PRIMARY KEY, "\
	"\n    sha1                            TEXT UNIQUE, "\
	"\n    hostname                        TEXT, "\
	"\n    hits                            INTEGER, "\
	"\n    initial_result_id               INTEGER, "\
	"\n    created_ts                      INTEGER, "\
	"\n    disabled                        INTEGER, "\
	"\n    disabled_code                   INTEGER, "\
	"\n    portscan_status                 INTEGER "\
	"\n );"\




#endif


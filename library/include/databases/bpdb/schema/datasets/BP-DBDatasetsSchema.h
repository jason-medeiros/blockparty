/*
 * BP-DBDatasetsSchema.h
 *
 *  Created on: Aug 8, 2015
 *      Author: root
 */

#ifndef __BP_DBDATASETS_SCHEMA_H
#define __BP_DBDATASETS_SCHEMA_H


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Proxies %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// proxies table
#define BP_DB_SQLITE_CREATE_TABLE_PROXIES\
	"\n CREATE TABLE IF NOT EXISTS proxies "\
	"\n ("\
	"\n    id                              INTEGER PRIMARY KEY, "\
	"\n    user_id                         INTEGER, "\
	"\n    group_id                        INTEGER, "\
	"\n    proxy_type                      INTEGER, "\
	"\n    host                            TEXT, "\
	"\n    port                            TEXT, "\
	"\n    username                        TEXT, "\
	"\n    password                        TEXT, "\
	"\n    user_notes                      TEXT, "\
	"\n    created_ts                      INTEGER, "\
	"\n    last_validated_ts               INTEGER, "\
	"\n    enabled                         INTEGER, "\
	"\n    validation_status               INTEGER "\
	"\n );"\


// proxy_groups table
#define BP_DB_SQLITE_CREATE_TABLE_PROXY_GROUPS\
	"\n CREATE TABLE IF NOT EXISTS proxy_groups "\
	"\n ("\
	"\n    id                              INTEGER PRIMARY KEY, "\
	"\n    user_id                         INTEGER, "\
	"\n    group_id                        INTEGER, "\
	"\n    enabled                         INTEGER, "\
	"\n    created_ts                      INTEGER, "\
	"\n    group_description               TEXT     "\
	"\n );"\



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Wordlists %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// proxies table
#define BP_DB_SQLITE_CREATE_TABLE_WORDLISTS\
	"\n CREATE TABLE IF NOT EXISTS wordlists "\
	"\n ("\
	"\n    id                              INTEGER PRIMARY KEY, "\
	"\n    user_id                         INTEGER, "\
	"\n    group_id                        INTEGER, "\
	"\n    unique_lookup_sha1              TEXT, "\
	"\n    value                           TEXT, "\
	"\n    sha1                            TEXT, "\
	"\n    md5                             TEXT, "\
	"\n    created_ts                      INTEGER "\
	"\n );"\


// wordlist_groups table
#define BP_DB_SQLITE_CREATE_TABLE_WORDLIST_GROUPS\
	"\n CREATE TABLE IF NOT EXISTS wordlist_groups "\
	"\n ("\
	"\n    id                              INTEGER PRIMARY KEY, "\
	"\n    user_id                         INTEGER, "\
	"\n    group_name                      TEXT,     "\
	"\n    group_description               TEXT,     "\
	"\n    created_ts                      INTEGER "\
	"\n );"\


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Useragents %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// useragents table
#define BP_DB_SQLITE_CREATE_TABLE_USERAGENTS\
	"\n CREATE TABLE IF NOT EXISTS useragents "\
	"\n ("\
	"\n    id                              INTEGER PRIMARY KEY, "\
	"\n    user_id                         INTEGER, "\
	"\n    group_id                        INTEGER, "\
	"\n    type                            INTEGER, "\
	"\n    value                           TEXT, "\
	"\n    created_ts                      INTEGER "\
	"\n );"\

// useragent_groups table
#define BP_DB_SQLITE_CREATE_TABLE_USERAGENT_GROUPS\
	"\n CREATE TABLE IF NOT EXISTS useragent_groups "\
	"\n ("\
	"\n    id                              INTEGER PRIMARY KEY, "\
	"\n    user_id                         INTEGER,  "\
	"\n    group_name                      TEXT,     "\
	"\n    group_description               TEXT,     "\
	"\n    created_ts                      INTEGER   "\
	"\n );"\



#endif /* __BP_DBDATASETS_SCHEMA_H */

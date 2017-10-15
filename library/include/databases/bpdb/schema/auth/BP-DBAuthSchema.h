/*
 * BP-DBAuthSchema.h
 *
 *  Created on: Aug 9, 2015
 *      Author: root
 */

#ifndef __BP_DBAUTH_SCHEMA
#define __BP_DBAUTH_SCHEMA


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% API Key Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#define BP_DB_SQLITE_CREATE_TABLE_KEYS\
	"\n CREATE TABLE IF NOT EXISTS keys "\
	"\n ("\
	"\n    id                               INTEGER, "\
	"\n    user_id                          INTEGER, "\
	"\n    group_id                         INTEGER, "\
	"\n    key                              TEXT,    "\
	"\n    created_ts                       INTEGER, "\
	"\n    last_used_ts                     INTEGER  "\
	"\n );"


#define BP_DB_SQLITE_CREATE_TABLE_KEY_GROUPS\
	"\n CREATE TABLE IF NOT EXISTS key_groups "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    user_id                          INTEGER,  "\
	"\n    group_name                       TEXT,     "\
	"\n    group_description                TEXT,     "\
	"\n    created_ts                       INTEGER   "\
	"\n );"


#define BP_DB_SQLITE_CREATE_TABLE_KEY_PRIVS\
	"\n CREATE TABLE IF NOT EXISTS key_privs "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    user_id                          INTEGER,  "\
	"\n    key_id                           INTEGER,  "\
	"\n    req_action                       TEXT,     "\
	"\n    allow                            INTEGER   "\
	"\n );"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% User Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#define BP_DB_SQLITE_CREATE_TABLE_KEY_USERS\
	"\n CREATE TABLE IF NOT EXISTS users "\
	"\n ("\
	"\n    id                               INTEGER, "\
	"\n    first_name                       TEXT,    "\
	"\n    middle_name                      TEXT,    "\
	"\n    last_name                        TEXT,    "\
	"\n    email                            TEXT,    "\
	"\n    username                         TEXT,    "\
	"\n    password                         TEXT,    "\
	"\n    salt                             TEXT,    "\
	"\n    created_ts                       INTEGER, "\
	"\n    role                             TEXT     "\
	"\n );"



#endif /* __BP_DBAUTH_SCHEMA */

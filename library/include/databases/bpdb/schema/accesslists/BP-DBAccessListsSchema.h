/*
 * BP-DBAccessListsSchema.h
 *
 *  Created on: Aug 9, 2015
 *      Author: root
 */

#ifndef __BP_DBACCESS_LISTS_H
#define __BP_DBACCESS_LISTS_H


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Whitelists %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#define BP_DB_SQLITE_CREATE_TABLE_WHITELISTS\
	"\n CREATE TABLE IF NOT EXISTS whitelists "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    user_id                          INTEGER,  "\
	"\n    group_id                         INTEGER,  "\
	"\n    type                             INTEGER,  "\
	"\n    value                            TEXT,     "\
	"\n    created_ts                       INTEGER   "\
	"\n );"

#define BP_DB_SQLITE_CREATE_TABLE_WHITELIST_GROUPS\
	"\n CREATE TABLE IF NOT EXISTS whitelist_groups "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    user_id                          INTEGER,  "\
	"\n    group_name                       TEXT,     "\
	"\n    group_description                TEXT,     "\
	"\n    created_ts                       INTEGER   "\
	"\n );"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Blacklists %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#define BP_DB_SQLITE_CREATE_TABLE_BLACKLISTS\
	"\n CREATE TABLE IF NOT EXISTS blacklists "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    user_id                          INTEGER,  "\
	"\n    group_id                         INTEGER,  "\
	"\n    type                             INTEGER,  "\
	"\n    value                            TEXT,     "\
	"\n    created_ts                       INTEGER   "\
	"\n );"

#define BP_DB_SQLITE_CREATE_TABLE_BLACKLIST_GROUPS\
	"\n CREATE TABLE IF NOT EXISTS blacklist_groups "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    user_id                          INTEGER,  "\
	"\n    group_name                       TEXT,     "\
	"\n    group_description                TEXT,     "\
	"\n    created_ts                       INTEGER   "\
	"\n );"


#endif /* __BP_DBACCESS_LISTS_H */

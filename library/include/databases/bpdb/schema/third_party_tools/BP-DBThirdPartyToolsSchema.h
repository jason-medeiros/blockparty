/*
 * BP-DBThirdPartyToolsSchema.h
 *
 *  Created on: Aug 10, 2015
 *      Author: root
 */

#ifndef __BP_DBTHIRD_PARTY_TOOLS_H
#define __BP_DBTHIRD_PARTY_TOOLS_H

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% NMAP Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#define BP_DB_SQLITE_CREATE_TABLE_NMAP_HOSTNAME_INFO\
	"\n CREATE TABLE IF NOT EXISTS nmap_hostname_info "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    result_base_id                   INTEGER,  "\
	"\n    result_host_info_id              INTEGER,  "\
	"\n    name                             TEXT,     "\
	"\n    type                             TEXT,     "\
	"\n    commit_ts                        INTEGER   "\
	"\n );"

#define BP_DB_SQLITE_CREATE_TABLE_NMAP_HOST_INFO\
	"\n CREATE TABLE IF NOT EXISTS nmap_host_info "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    result_base_id                   INTEGER,  "\
	"\n    ipv4_addr                        TEXT,     "\
	"\n    ipv6_addr                        TEXT,     "\
	"\n    mac_addr                         TEXT,     "\
	"\n    host_starttime                   TEXT,     "\
	"\n    host_endtime                     TEXT,     "\
	"\n    state                            TEXT,     "\
	"\n    state_reason                     TEXT,     "\
	"\n    extraports_state                 TEXT,     "\
	"\n    extraports_count                 TEXT,     "\
	"\n    commit_ts                        INTEGER   "\
	"\n );"

#define BP_DB_SQLITE_CREATE_TABLE_NMAP_PORT_INFO\
	"\n CREATE TABLE IF NOT EXISTS nmap_port_info "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    result_base_id                   INTEGER,  "\
	"\n    nmap_host_info_id                INTEGER,  "\
	"\n    protocol                         TEXT,     "\
	"\n    portid                           INTEGER,  "\
	"\n    commit_ts                        INTEGER   "\
	"\n );"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Geneirc Tool Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#define BP_DB_SQLITE_CREATE_TABLE_COMMAND_LINE_TOOLS\
	"\n CREATE TABLE IF NOT EXISTS command_line_tools "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    tool_sha1                        TEXT,     "\
	"\n    sensor_identifier_sha1           TEXT,     "\
	"\n    run_attempts                     INTEGER   "\
	"\n    name                             TEXT,     "\
	"\n    version                          TEXT,     "\
	"\n    description                      TEXT,     "\
	"\n    commit_ts                        INTEGER   "\
	"\n );"


#define BP_DB_SQLITE_CREATE_TABLE_COMMAND_LINE_TOOL_DATA\
	"\n CREATE TABLE IF NOT EXISTS command_line_tool_data "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    run_id                           INTEGER,  "\
	"\n    tool_id                          INTEGER,  "\
	"\n    tool_data_type                   INTEGER,  "\
	"\n    assessment_id                    INTEGER,  "\
	"\n    nmap_host_id                     INTEGER,  "\
	"\n    base                             INTEGER,  "\
	"\n    path                             INTEGER,  "\
	"\n    script                           INTEGER,  "\
	"\n    result                           INTEGER,  "\
	"\n    data_sha1                        TEXT,     "\
	"\n    data                             TEXT,     "\
	"\n    commit_ts                        INTEGER   "\
	"\n );"


#endif /* __BP_DBTHIRD_PARTY_TOOLS_H */

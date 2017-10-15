/*
 * BP-DBReportsSchema.h
 *
 *  Created on: Aug 10, 2015
 *      Author: root
 */

#ifndef __BP_DBREPORTS_SCHEMA_H
#define __BP_DBREPORTS_SCHEMA_H

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Reports Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#define BP_DB_SQLITE_CREATE_TABLE_REPORTS_GENERATED\
	"\n CREATE TABLE IF NOT EXISTS reports_generated "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    user_id                          INTEGER,  "\
	"\n    sha1                             TEXT,     "\
	"\n    template_id                      INTEGER,  "\
	"\n    report_body                      TEXT,     "\
	"\n    commit_ts                        INTEGER   "\
	"\n );"


#define BP_DB_SQLITE_CREATE_TABLE_REPORT_TEMPLATES\
	"\n CREATE TABLE IF NOT EXISTS report_templates "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    user_id                          INTEGER,  "\
	"\n    sha1                             TEXT,     "\
	"\n    report_template_identifier       TEXT,     "\
	"\n    report_template_body             TEXT,     "\
	"\n    commit_ts                        INTEGER,  "\
	"\n    last_edited_ts                   INTEGER   "\
	"\n );"


#define BP_DB_SQLITE_CREATE_TABLE_REPORT_TEMPLATE_SNIPPETS\
	"\n CREATE TABLE IF NOT EXISTS report_template_snippets "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    user_id                          INTEGER,  "\
	"\n    sha1                             TEXT,     "\
	"\n    snippet_identifier               TEXT,     "\
	"\n    snippet_body                     TEXT,     "\
	"\n    snippet_notes                    TEXT,     "\
	"\n    commit_ts                        INTEGER   "\
	"\n );"


#define BP_DB_SQLITE_CREATE_TABLE_REPORT_TEMPLATE_SNIPPET_GROUPS\
	"\n CREATE TABLE IF NOT EXISTS report_template_snippet_groups "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    user_id                          INTEGER,  "\
	"\n    group_name                       TEXT,     "\
	"\n    group_description                TEXT,     "\
	"\n    created_ts                       INTEGER   "\
	"\n );"


#endif /* __BP_DBREPORTS_SCHEMA_H */

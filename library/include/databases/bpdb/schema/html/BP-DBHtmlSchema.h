/*
 * BP-DBHtmlSchema.h
 *
 *  Created on: Aug 9, 2015
 *      Author: root
 */

#ifndef __BP_DBHTML_SCHEMA
#define __BP_DBHTML_SCHEMA


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% HTML Anchors %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#define BP_DB_SQLITE_CREATE_TABLE_HTML_ANCHOR_DATA\
	"\n CREATE TABLE IF NOT EXISTS html_anchor_data "\
	"\n ("\
	"\n    id                         INTEGER,  "\
	"\n    result_id                  INTEGER,  "\
	"\n    rel_href                   TEXT,     "\
	"\n    script_name                TEXT,     "\
	"\n    commit_ts                  INTEGER   "\
	"\n );"\

#define BP_DB_SQLITE_CREATE_TABLE_HTML_ANCHOR_VAR_DATA\
	"\n CREATE TABLE IF NOT EXISTS html_anchor_data "\
	"\n ("\
	"\n    id                         INTEGER,  "\
	"\n    result_id                  INTEGER,  "\
	"\n    anchor_id                  INTEGER,  "\
	"\n    name                       TEXT,     "\
	"\n    value                      TEXT,     "\
	"\n    commit_ts                  INTEGER   "\
	"\n );"\



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% HTML Forms %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#define BP_DB_SQLITE_CREATE_TABLE_HTML_FORM_DATA\
	"\n CREATE TABLE IF NOT EXISTS html_form_data "\
	"\n ("\
	"\n    id                         INTEGER,  "\
	"\n    result_id                  INTEGER,  "\
	"\n    form_method                TEXT,     "\
	"\n    form_action                TEXT,     "\
	"\n    commit_ts                  INTEGER   "\
	"\n );"\

#define BP_DB_SQLITE_CREATE_TABLE_HTML_FORM_INPUT_DATA\
	"\n CREATE TABLE IF NOT EXISTS html_form_input_data "\
	"\n ("\
	"\n    id                         INTEGER,  "\
	"\n    result_id                  INTEGER,  "\
	"\n    form_id                    INTEGER,  "\
	"\n    commit_ts                  INTEGER   "\
	"\n );"\

#define BP_DB_SQLITE_CREATE_TABLE_HTML_FORM_INPUT_PROPERTY_DATA\
	"\n CREATE TABLE IF NOT EXISTS html_form_input_property_data "\
	"\n ("\
	"\n    id                         INTEGER,  "\
	"\n    result_id                  INTEGER,  "\
	"\n    form_id                    INTEGER,  "\
	"\n    form_input_id              INTEGER,  "\
	"\n    property_name              TEXT,     "\
	"\n    property_value             TEXT,     "\
	"\n    commit_ts                  INTEGER   "\
	"\n );"\

#define BP_DB_SQLITE_CREATE_TABLE_HTML_FORM_PROPERTY_DATA\
	"\n CREATE TABLE IF NOT EXISTS html_form_property_data "\
	"\n ("\
	"\n    id                         INTEGER,  "\
	"\n    result_id                  INTEGER,  "\
	"\n    form_id                    INTEGER,  "\
	"\n    property_name              TEXT,     "\
	"\n    property_value             TEXT,     "\
	"\n    commit_ts                  INTEGER   "\
	"\n );"\


#endif /* __BP_DBHTML_SCHEMA */

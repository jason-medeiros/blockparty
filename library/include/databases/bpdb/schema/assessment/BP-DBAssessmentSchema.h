/*
 * BP-DBAssessmentSchema.h
 *
 *  Created on: Aug 9, 2015
 *      Author: root
 */

#ifndef __BP_DBASSESSMENT_SCHEMA
#define __BP_DBASSESSMENT_SCHEMA


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Assessment Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#define BP_DB_SQLITE_CREATE_TABLE_ASSESSMENTS\
	"\n CREATE TABLE IF NOT EXISTS assessments "\
	"\n ("\
	"\n    id                               INTEGER,  "\
	"\n    user_id                          INTEGER,  "\
	"\n    name                             TEXT,     "\
	"\n    commit_ts                        INTEGER   "\
	"\n    enabled                          INTEGER   "\
	"\n );"

#endif /* __BP_DBASSESSMENT_SCHEMA */

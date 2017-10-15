/*
 * BP-DBStringFilters.h
 *
 *  Created on: Oct 12, 2016
 *      Author: root
 */

#ifndef __BP_DBSTRINGFILTERS_H_
#define __BP_DBSTRINGFILTERS_H_



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% String Utilities for BPDB %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// -- NOTE: Do not prefix anything to StringIsReasonable routines (eg BP_DBStringIsReasonable) as they're used quite
//          often throughout the entire library.  Leaving the StringIsReasonable prefix allows the indexer the ability to
//          quick-lookup all routines.  Basically it just makes development easier.



// -- MariaDB Connector Value Checks

// checks if a host name is reasonable for a mariadb host connect string (must be host or ip address)
BP_BOOL BP_StringIsReasonableForMariaDBHostNameDB(char *buffer, size_t len);

// checks if a database name is reasonable
BP_BOOL BP_StringIsReasonableForDatabaseMariaDB(char *buffer, size_t len);

// checks if a username is reasonable
BP_BOOL BP_StringIsReasonableForUsernameMariaDB(char *buffer, size_t len);

// checks if a host name is reasonable for a mariadb connect string (must be host or ip address
BP_BOOL BP_StringIsReasonableForPasswordMariaDB(char *buffer, size_t len);


// -- SQLite Connector Value Checks

// checks to see if a sqlite file is reasonable
BP_BOOL BP_StringIsReasonableForDatabaseFileSQLiteDB(char *buffer, size_t len, BP_BOOL attempt_to_stat_file);

// checks to see if log file is reasonable
BP_BOOL BP_StringIsReasonableForDatabaseLogFileSQLiteDB(char *buffer, size_t len, BP_BOOL attempt_to_stat_file);


// -- MariaDB/SQLite Shared Table and Column Validators ------------------

// checks if a string is syntactically reasonable for what's expected of a table name.
BP_BOOL BP_StringIsReasonableForTableNameDB(char * buffer, size_t len);

// checks if a string is syntactically reasonable for what's expected of a column name.
BP_BOOL BP_StringIsReasonableForColumnNameDB(char * buffer, size_t len);

// checks if a string is syntactically reasonable for what's expected of a database name.
BP_BOOL BP_StringIsReasonableForDatabaseNameDB(char * buffer, size_t len);

// checks to see if a string is reasonable for a field specifier (should be: tablename.fieldname)
BP_BOOL BP_StringIsReasonableForSelectFieldSpecifierDB(char * buffer, size_t len);

// checks to see if a string is reasonable for a column type field
BP_BOOL BP_StringIsReasonableForColumnTypeFieldDB(char *buffer, size_t len);

// checks to see if a string is reasonable for a mariadb comparison operator
BP_BOOL BP_StringIsReasonableForMariadbComparisonOperatorDB(char *buffer, size_t len);

// checks to see if a string is reasonable for a mariadb conjunction
BP_BOOL BP_StringIsReasonableForMariadbConjunctionDB(char *buffer, size_t len);

// checks to see if a string is reasonable for a mariadb sort designator
BP_BOOL BP_StringIsReasonableForMariadbSortDesignatorDB(char *buffer, size_t len);




#endif

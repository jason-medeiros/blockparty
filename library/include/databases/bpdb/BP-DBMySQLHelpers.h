/*
 * BP-DBMySQL.h
 *
 *  Created on: Oct 12, 2015
 *      Author: root
 */

#ifndef INCLUDE_DATABASES_BPDB_BP_DBMYSQLHELPERS_H_
#define INCLUDE_DATABASES_BPDB_BP_DBMYSQLHELPERS_H_


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Auto Load Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempt to retieve all tables from the database opened with the current bpdb handle.
BP_ERROR_T BP_DBMySQLAutoLoadAllTables(P_BPDB bpdb);

// table columns
BP_ERROR_T BP_DBMySQLAutoLoadTableColumns(P_BPDB bpdb, char *table_name);


#endif /* INCLUDE_DATABASES_BPDB_BP_DBMYSQLHELPERS_H_ */

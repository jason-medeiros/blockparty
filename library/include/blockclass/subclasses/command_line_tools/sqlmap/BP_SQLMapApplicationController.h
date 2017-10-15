/*
 * BP_SQLMapApplicationController.h
 *
 *  Created on: Aug 30, 2013
 *      Author: root
 */

#ifndef BP_SQLMAPAPPLICATIONCONTROLLER_H_
#define BP_SQLMAPAPPLICATIONCONTROLLER_H_

/*

	BP_SQLMapApplicationController : BP_CommandLineToolBase : Block
	==================================================================
	This class inherits from command line tool base controller, and allows
	for the direct control of the sqlmap application.  The controller has the
	ability to report directly to the db if so set.


*/


class BP_SQLMapApplicationController : public BP_CommandLineToolBase /* inherits from Block */
{

public:

	// memory allocator for local data
	P_BP_LINKL sqlmap_m;


	// constructor (autocalls contructor for command line base)
	BP_SQLMapApplicationController(char * path_to_sqlmap_binary);

	// deconstructor (frees all memory)
	~BP_SQLMapApplicationController();

	// =====================================

	// this is flagged after the app exited
	size_t sqlmap_exited;

	// this is the sqlmap result csv file
	char * sqlmap_result_csv_file;

	// this is set directly after the sqlmap application exits.  If the sqlmap_exited
	// flag is set, this will either be null or contain valid data.
	BP_StringUtils * sqlmap_csv_parsed_data;

	// Execute sqlmap.  This will populate some local data, so before
	// calling it recursively, call the cleanup routine below, or just delete
	// and recreate the object.  Performance hits should be negligable either way.
	BP_ERROR_T executeSQLMapBinary();

	// inserts results into the database (requires a valid this->mysqlSession).  Data inserted
	// is taken directly from sqlmap_csv_parsed_data.
	BP_ERROR_T insertSQLMapBinaryRunResults();

	// Post execution cleanup routine.  Call after you've duplicated any data
	// you'd like to keep from the populated session.
	BP_ERROR_T postExecutionCleanup();

	// Parse csv file name from output.  The sqlmap
	// binary always writes the path of the csv file generated
	// out to the filesystem.  This parses an [INFO] string to a
	// usable path.
	BP_ERROR_T parseCSVFileFromString(char * info_str);

	// Parse a sqlmap binary file and store it in the database.
	BP_ERROR_T parseSQLMapCSVFileAndInsert(char * path_to_file);

	// recursively searches a directory for CSV data and creates
	// relevant inserts.
	BP_ERROR_T parseAllSQLMapCSVInDir(char * directory_path);

	// returns the global current sql map process count
	size_t currentNumProcessesRunning();

};


#endif /* BP_SQLMAPAPPLICATIONCONTROLLER_H_ */

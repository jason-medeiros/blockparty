/*
 * BP_StringFormattingSession.h
 *
 *  Created on: Aug 4, 2014
 *      Author: root
 */

#ifndef BP_STRINGFORMATTINGSESSION_H_
#define BP_STRINGFORMATTINGSESSION_H_


// String formatting session class.  Used for output.
class BP_StringFormattingSession : public BP_BaseError
{


public:


	//
	// Session associated with this class (C).  This session contains
	// a tailqueue allocator, which is used for all allocations in this
	// class.  It's the reason this class has no tq member.
	//
	// This session contains all the relevant structure data, such as
	// row groups, rows, column groups, columns, etc, as well as the
	// linked form of the session itself.
	//
	P_BP_SF_SESSION sfs;

	// last search results
	P_BP_SFS_SEARCH_RESULTS sfs_search_results;

	// session display settings (not to be confused with output settings).
    BP_SFS_INFO_DISPLAY_SETTINGS display_settings;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Current Indexes %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// NEVER MANIPULATE THESE DIRECTLY, USE THE CHANGE INDEX ROUTINES
	// BELOW.

	// index for the current row group
	char * row_group_idx;

	// index for the current row within the row group
	char * row_idx;

	// index for the column group within the row index
	char * column_group_idx;

	// index for the column within the column group
	char * column_idx;

	// string entry index swithin the column
	char * string_entry_idx;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Current Entries %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// NEVER MANIPULATE THESE DIRECTLY, USE THE CHANGE INDEX ROUTINES
	// BELOW.

	// the row group associated with the row group index
	P_BP_SFS_STRING_ROW_GROUP       row_group;

	// the row associated with the row index
	P_BP_SFS_STRING_ROW             row;

	// the column group associated with the column group index
	P_BP_SFS_STRING_COLUMN_GROUP    column_group;

	// the column associated with the column index
	P_BP_SFS_STRING_COLUMN          column;

	// the string entry associated with the string entry index
	P_BP_SFS_STRING_ENTRY           string_entry;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Methods to Change Indexes %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// change the row group and update all entries
	BP_ERROR_T changeRowGroupIdx   (char *row_group_idx, BP_BOOL add_if_doesnt_exist = BP_FALSE);

	// change the row index and update all entries
	BP_ERROR_T changeRowIdx        (char *row_idx, BP_BOOL add_if_doesnt_exist = BP_FALSE);

	// update the column group index and update all entries
	BP_ERROR_T changeColGroupIdx   (char *col_group_idx, BP_BOOL add_if_doesnt_exist = BP_FALSE);

	// update the column index and update all entries
	BP_ERROR_T changeColIdx        (char *col_idx, BP_BOOL add_if_doesnt_exist = BP_FALSE);

	// update the string entry index and update all entries
	BP_ERROR_T changeStringEntryIdx
	(
			char *str_entry_idx,
			BP_BOOL add_if_doesnt_exist = BP_FALSE,
			char *string_data           = NULL,
			size_t length               = 0,
			size_t display_width        = 0,
			P_BP_COLOR_CODE_SET_T initial_colors = NULL,
			BP_BOOL is_newline_delimited = BP_FALSE,
			size_t pad_lines_below       = 0,
			size_t pad_lines_above       = 0,
			P_BP_STRING_ROW_BORDER_SET border_set = NULL
	);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Methods %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add routines are just convinence / logical wrappers for the change routines.  They
	// just simply enable the add_if_doesnt_exist parameters.  This is mostly done for ease
	// of development/increased code readability.

	// add a row group
	BP_ERROR_T addRowGroup(char *row_group_idx = NULL);

	// add a row
	BP_ERROR_T addRow(char *row_idx = NULL);

	// add a column group
	BP_ERROR_T addColumnGroup(char *col_group_idx = NULL);

	// add a column
	BP_ERROR_T addColumn(char *column_idx = NULL);

	// add a string entry
	BP_ERROR_T addStringEntry
	(
		char *string_entry_idx,
		char *string_data,
		size_t length,
		size_t display_width,
		P_BP_COLOR_CODE_SET_T initial_colors,
		BP_BOOL is_newline_delimited = BP_FALSE,
		size_t pad_lines_below = 0,
		size_t pad_lines_above = 0,
		P_BP_STRING_ROW_BORDER_SET border_set = NULL
	);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Update/Move/Reset Methods %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// moves to a specified placement within the sfs session
	BP_ERROR_T move
	(
		char * row_group_idx,
		char * row_idx       = NULL,
		char * col_group_idx = NULL,
		char * col_idx       = NULL,
		char * str_entry_idx = NULL
	);

	// updates all data based on the current indexes
	BP_ERROR_T update();

	// resets all data (frees) stored in indexes and nullifies entries (doesn't free)
	BP_ERROR_T reset
	(
		BP_BOOL destroy_row_group_idx = BP_TRUE,
		BP_BOOL destroy_row_idx       = BP_TRUE,
		BP_BOOL destroy_col_group_idx = BP_TRUE,
		BP_BOOL destroy_col_idx       = BP_TRUE,
		BP_BOOL destroy_str_entry_idx = BP_TRUE
	);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Final Output %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// call this to take the entrie structure of the document and prepare it, applying
	// colors, borders and whatnot, and generating the final output data for each element.
	BP_ERROR_T prepareFinalOutput();


	// if the final output was generated ok, we can display it here
	BP_ERROR_T displayFinalOutput(BP_BOOL display_colorized = BP_TRUE);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Utility Methods %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// simple routine to display sfs data within the class
	BP_ERROR_T displaySFSClassData();

	// display the sfs session structure (this is not an output routine, this is
	// a utility routine for visualizing the sfs session structure)
	BP_ERROR_T displaySFSSession();

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Class Constructor / Deconstructor %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// class constructor
	BP_StringFormattingSession(char * session_name);

	// class deconstructor
	~BP_StringFormattingSession();

};


#endif /* BP_STRINGFORMATTINGSESSION_H_ */

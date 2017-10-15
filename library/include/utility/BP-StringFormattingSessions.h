/*
 * BP-StringFormattingSessions.h
 *
 *  Created on: Aug 4, 2014
 *      Author: root
 */

#ifndef BP_STRINGFORMATTINGSESSIONS_H_
#define BP_STRINGFORMATTINGSESSIONS_H_


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% String Formatting Sessions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// String formatting sessions allow a developer to pack strings and columns
// in an intuitive and simple way.  It works by first creating a session to
// manage and allocate memory.  The session stores row groups.  Row groups
// store rows.  Rows store column groups.  Column groups store colums.  Columns
// store string entries, and string entries contain display data such as
// lengths and vty color preferences for rows.


// Forward declare structs so they can be referenced in cascading form
typedef struct __BP_SFS_STRING_COLUMN_GROUP;
typedef struct __BP_SFS_STRING_COLUMN;
typedef struct __BP_SFS_STRING_ROW;
typedef struct __BP_SFS_STRING_ROW_GROUP;
typedef struct __BP_SFS_STRING_COLUMN;
typedef struct __BP_SF_SESSION;


// abstract used for holding character information
typedef struct __BP_SFS_CHAR_ENTRY {

	// character
	char character;

	// color display codes for character
	BP_COLOR_CODE_SET_T color_info;

} BP_SFS_CHAR_ENTRY, *P_BP_SFS_CHAR_ENTRY;



// abstract used for holding individual string info
typedef struct __BP_SFS_STRING_ENTRY {


	// string entry identifier
	char * string_entry_identifier;

	// string entry identifier length
	size_t string_entry_identifier_len;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% String Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// string data
	P_BP_SFS_CHAR_ENTRY string;

	// length of the string created
	size_t string_length;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Custom Display Parameters %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Display max width (when text is displayed, if this item
	// is non-zero, it becomes the max column width)
	size_t display_max_width;

	// these lines are added to the generated columnization
	size_t pad_lines_above;
	size_t pad_lines_below;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% String Flags %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Indicator whether or not the string is marked for display
	BP_BOOL marked_display_disabled;

	// If this is set to true, the before being ouput to the user
	// the display width will be auto calculated by breaking the
	// string into newlines, and selecting the widest column.
	BP_BOOL auto_calculate_max_width_by_newlines;

	// If this is set to true, the buffer will be stripped of newlines
	// before display.
	BP_BOOL strip_newlines;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Border Set %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// border set for row group
	BP_STRING_ROW_BORDER_SET border_set;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Converted Display Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// final columnized data
	P_BP_GENERIC_STRING_COLUMNIZATION final_columnization;
	P_BP_GENERIC_STRING_COLUMNIZATION final_columnization_colorized;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Lookup Indicators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this entry belongs to this column
	__BP_SFS_STRING_COLUMN * column;

	// this entry belongs to this formatting session
	__BP_SF_SESSION * session;

} BP_SFS_STRING_ENTRY, *P_BP_SFS_STRING_ENTRY;



// simple structure to represent a column of data
typedef struct __BP_SFS_STRING_COLUMN {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Column Indicators %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// column name as defined by the developer
	char *column_identifier;
	size_t column_identifier_len;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Column Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// the actual data
	P_BP_SFS_STRING_ENTRY *col_data;

	// number of entries
	size_t entries;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Column Extraneous Data %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// width of the columns (array of size_t matching with col_data)
	size_t * widths;

	// widest column
	size_t widest_column;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Border Separators %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// border on the right will be padded with this string
	char * border_left;

	// border on the right will be padded with this string
	char * border_right;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Output Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// final columnized data
	P_BP_GENERIC_STRING_COLUMNIZATION final_columnization;
	P_BP_GENERIC_STRING_COLUMNIZATION final_columnization_colorized;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Border Set %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// border set for row group
	BP_STRING_ROW_BORDER_SET border_set;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Lookup Indicators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this entry belongs to this column column group
	struct __BP_SFS_STRING_COLUMN_GROUP * column_group;

	// this entry belongs to this formatting session
	struct __BP_SF_SESSION * session;


} BP_SFS_STRING_COLUMN, *P_BP_SFS_STRING_COLUMN;




// simple structure to represent a grouping of columns, which
// is used primarily for displaying the individualo columns.
typedef struct __BP_SFS_STRING_COLUMN_GROUP {

	char * column_group_identifier;
	size_t column_group_identifier_len;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Col Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// columns associated with this group
	P_BP_SFS_STRING_COLUMN * columns;

	// number of columns in the column array
	size_t columns_n;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Settings %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// pad the mismatched colum lines or not
	BP_BOOL pad_column_mismatch_lines;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Border Set %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// border set for row group
	BP_STRING_ROW_BORDER_SET border_set;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Output Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// final columnized data
	P_BP_GENERIC_STRING_COLUMNIZATION final_columnization;
	P_BP_GENERIC_STRING_COLUMNIZATION final_columnization_colorized;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Lookup Indicators %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this entry belongs to this row
	struct __BP_SFS_STRING_ROW * row;

	// this entry belongs to this formatting session
	struct __BP_SF_SESSION * session;

} BP_SFS_STRING_COLUMN_GROUP, *P_BP_SFS_STRING_COLUMN_GROUP;




// string row data
typedef struct __BP_SFS_STRING_ROW {

	// string identifier for this row
	char *row_identifier;
	size_t row_identifier_len;

	// columns sets
	P_BP_SFS_STRING_COLUMN_GROUP * col_groups;

	// number of column sets stored here
	size_t                     col_group_n;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Output Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// final columnized data
	P_BP_GENERIC_STRING_COLUMNIZATION final_columnization;
	P_BP_GENERIC_STRING_COLUMNIZATION final_columnization_colorized;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Border Set for This Group %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// border set for row group
	BP_STRING_ROW_BORDER_SET border_set;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Lookup Indicators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this entry belongs to this row group
	struct __BP_SFS_STRING_ROW_GROUP * row_group;

	// this entry belongs to this formatting session
	struct __BP_SF_SESSION * session;



} BP_SFS_STRING_ROW, *P_BP_SFS_STRING_ROW;



// Allows the managing of row data
typedef struct __BP_SFS_STRING_ROW_GROUP {


	// string representing a name/identifier for this row group
	char *row_group_identifier;
	size_t row_group_identifier_len;

	// actual row sets
	P_BP_SFS_STRING_ROW * rows;

	//  row count
	size_t            row_n;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Output Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// final columnized data
	P_BP_GENERIC_STRING_COLUMNIZATION final_columnization;
	P_BP_GENERIC_STRING_COLUMNIZATION final_columnization_colorized;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Border Set for This Group %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// border set for row group
	BP_STRING_ROW_BORDER_SET border_set;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Lookup Indicators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this entry belongs to this formatting session
	struct __BP_SF_SESSION * session;


} BP_SFS_STRING_ROW_GROUP, *P_BP_SFS_STRING_ROW_GROUP;


// Main string formatting session structure.
typedef struct __BP_SF_SESSION {


	// tailqueue allocator used for this session
	P_BP_LINKL tq;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Row Group Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// row groups data (each row contains a column group, each column group, a number of
	// columns.
	P_BP_SFS_STRING_ROW_GROUP * row_groups;

	// number of row groups
	size_t row_group_n;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Session Identifiers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// session name (if desired)
	char * session_name;

	// length of the above buffer, in bytes (primarily used for search speed)
	size_t session_name_len;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Border Set for This Group %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// border set for row group
	BP_STRING_ROW_BORDER_SET border_set;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Finalized Output For User Display %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// These are populated by calls to BP_GenerateFinalOutputSFSession(sfs).

	// final columnized data
	P_BP_GENERIC_STRING_COLUMNIZATION final_columnization;
	P_BP_GENERIC_STRING_COLUMNIZATION final_columnization_colorized;

	// this flag is set to BP_TRUE if the session was prepared into
	// a suitable output buffer.
	BP_BOOL output_prepared_ok;


} BP_SF_SESSION, *P_BP_SF_SESSION;



// Individual matching search result entries.  The
// entries which are set are dependent on search
// parameters found within a search query routine.
typedef struct __BP_SFS_SEARCH_RESULT {

	// session that this entry belongs to
	P_BP_SF_SESSION                    session;

	// row group which matched search
	P_BP_SFS_STRING_ROW_GROUP          row_group;

	// row which matched search
	P_BP_SFS_STRING_ROW                row;

	// column group which matched search
	P_BP_SFS_STRING_COLUMN_GROUP       column_group;

	// column which matched search
	P_BP_SFS_STRING_COLUMN             column;

	// string entry which matched search
	P_BP_SFS_STRING_ENTRY              string_entry;

} BP_SFS_SEARCH_RESULT, *P_BP_SFS_SEARCH_RESULT;



// Holds a number of search results.
typedef struct __BP_SFS_SEARCH_RESULTS {

	// search result entries
	P_BP_SFS_SEARCH_RESULT results;

	// number of results
	size_t                 result_n;

	// session associated with this search result
	P_BP_SF_SESSION session;

} BP_SFS_SEARCH_RESULTS, *P_BP_SFS_SEARCH_RESULTS;


// Search query which can be passed to the sfs search mechanisms/routines.
typedef struct __BP_SFS_SEARCH_QUERY {

	// === Search Options =====================================

	// At least one search option must be set, or no search
	// will be performed.

	// the row group to search (if NULL all are searched)
	char *          row_group_id;

	// the row to search (if NULL, all are searched)
	char * row_id;

	// column group id (if NULL, all are searched)
	char * column_group_id;

	// column group id (if NULL, all are searched)
	char * column_id;

	// string to search (if NULL, all are searched)
	char * string_entry_id;


} BP_SFS_SEARCH_QUERY, *P_BP_SFS_SEARCH_QUERY;


// structure to hold display settings for session information
typedef struct __BP_SFS_INFO_DISPLAY_SETTINGS {

	BP_BOOL display_session_header;
	BP_BOOL display_row_groups;
	BP_BOOL display_rows;
	BP_BOOL display_column_groups;
	BP_BOOL display_columns;
	BP_BOOL display_string_entries;
	BP_BOOL display_individual_characters;
	BP_BOOL display_vty_color;

	// This is a private member which is used when displaying data.
	// don't modify this unless you want display to go all wackadoo.
	size_t current_display_depth;

} BP_SFS_INFO_DISPLAY_SETTINGS, *P_BP_SFS_INFO_DISPLAY_SETTINGS;

// simple macro to set all display flags to true
#define BP_SFS_DISPLAY_ALL_TRUE_STACK(sfsd) \
						BP_ZERO_STACK(sfsd);\
						sfsd.display_session_header = BP_TRUE;\
						sfsd.display_row_groups = BP_TRUE;\
						sfsd.display_rows=BP_TRUE;\
						sfsd.display_column_groups=BP_TRUE;\
						sfsd.display_columns=BP_TRUE;\
						sfsd.display_string_entries=BP_TRUE;\
						sfsd.display_individual_characters=BP_TRUE;\
						sfsd.display_vty_color=BP_TRUE;

// simple macro to create a stack buffer full of tabs, used for
// dynamically indenting display entries
#define BP_SFS_CREATE_TAB_DEPTH_BUFFER char tab_depth[255];\
										BP_ZERO_STACK(tab_depth);\
										if(display_settings->current_display_depth < 254)\
										memset(tab_depth, '\t', display_settings->current_display_depth);

// creates a string column group
P_BP_SFS_STRING_COLUMN BP_CreateStringColumnGroup(size_t pad_column_mismatch_lines);

// Creates a column structure from a character ** array.  Because this
// creates a complex data type, a tail queue allocator is required for
// managing the creation of the data.
P_BP_SFS_STRING_COLUMN BP_CreateStringColumnFromStringArrayTQ(char ** string_array, size_t entries, P_BP_COLOR_CODE_SET_T color_info, P_BP_LINKL tq);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Colorizations for Entries %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Repaint an existing string entry with a provided (NULL TERMINATED) paint set array.  This array can be
// generated by using the BP_GenerateAlternatingColorStack() routine found in BP-StringUtilities.cc (near the bottom.)
// Just use one color entry as an argument to that routine to generate solid colors.
BP_ERROR_T BP_RepaintExistingStringEntry(P_BP_SFS_STRING_ENTRY entry, P_BP_COLOR_CODE_SET_T * paint_set);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Session Row Add-ers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to add a row group to a string formatting session
BP_ERROR_T BP_AddRowGroupToSFSession
(
	P_BP_SF_SESSION sfs,
	char * new_row_group_identifier,
	P_BP_STRING_ROW_BORDER_SET border_set = NULL
);

// attempt to add a row group to a string formatting session
BP_ERROR_T BP_AddRowToRowGroupSFSession
(
	P_BP_SF_SESSION sfs,
	char * row_group_identifier,
	char * new_row_identifier,
	P_BP_STRING_ROW_BORDER_SET border_set = NULL
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Session Column Add-ers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// attempt to add a column to a row group
BP_ERROR_T BP_AddColumnGroupToRowSFSession
(
	P_BP_SF_SESSION sfs,
	char *row_group_identifier,
	char *row_identifier,
	char *new_column_group_identifier,
	P_BP_STRING_ROW_BORDER_SET border_set = NULL
);

// attempt to add a column to a row group
BP_ERROR_T BP_AddColumnToColumnGroupSFSession
(
	P_BP_SF_SESSION sfs,
	char *row_group_identifier,
	char *row_identifier,
	char *column_group_identifier,
	char *new_column_identifier,
	P_BP_STRING_ROW_BORDER_SET border_set = NULL
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Session String Entry Add-ers %%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Add string entry to a column using raw character string data.  This
// routine performs an auto-conversion to a P_BP_SFS_STRING_ENTRY.  This
// is more of a utility routine than anything else, as it just frontends
// into BP_AddStringEntryToColumn (found below).
BP_ERROR_T BP_AddStringEntryToColumnUsingASCIIDataSFSession
(
	P_BP_SF_SESSION sfs,
	char *row_group_identifier,
	char *row_identifier,
	char *column_group_identifier,
	char *column_identifier,
	char *new_string_identifier,
	char *new_string_data,
	size_t new_string_len,
	size_t new_display_width,
	P_BP_COLOR_CODE_SET_T new_initial_colors,
	P_BP_STRING_ROW_BORDER_SET border_set
);

// add a string entry to a column using a string entry pointer.  This is not a
// copy oriented routine.  This routine simply stores the string entry pointer
// within the column.  Again, the string_entry and related data is NOT duplicated,
// only a reference is stored.
BP_ERROR_T BP_AddStringEntryToColumnSFSession
(
	P_BP_SF_SESSION sfs,
	char * row_group_identifier,
	char * row_identifier,
	char * column_group_identifier,
	char * column_identifier,
	P_BP_SFS_STRING_ENTRY string_entry,
	P_BP_STRING_ROW_BORDER_SET border_set = NULL
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% String Entry Generators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Generates a string entry from an ascii character string
P_BP_SFS_STRING_ENTRY BP_GenerateStringEntryFromCharString
(
		P_BP_SF_SESSION sfs,
		char * string_entry_identifier,
		char * string,
		size_t string_len,
		size_t display_width,
		P_BP_COLOR_CODE_SET_T initial_colors,
		size_t pad_lines_below = 0,
		size_t pad_lines_above = 0,
		P_BP_STRING_ROW_BORDER_SET border_set = NULL
);

// Generates a character * string from a string entry.  Essentially
// strips colors and whatnot.  Uses the tailqueue in the sfs session
// to perform the conversion, so it must be free'd with tq_free(sfs->tq, char_string_goes_here);
char * BP_GenerateCharStringFromStringEntry
(
		P_BP_SF_SESSION sfs,
		P_BP_SFS_STRING_ENTRY string_entry
);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Search Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// search string formatting session
P_BP_SFS_SEARCH_RESULTS BP_SearchStringFormattingSession
(
	P_BP_SF_SESSION session,
	P_BP_SFS_SEARCH_QUERY query
);

// Destroy search results
BP_ERROR_T BP_DestroySearchResultsSFSession(P_BP_SF_SESSION session, P_BP_SFS_SEARCH_RESULTS sfs_search_results);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Search Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Display RESULTS. Calls BP_DisplaySFSSearchResult on each result in the results
// structure.
BP_ERROR_T BP_DisplaySFSSearchResults(P_BP_SFS_SEARCH_RESULTS results);

// Display single result entry.
BP_ERROR_T BP_DisplaySFSSearchResult(P_BP_SFS_SEARCH_RESULT result);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Standard Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// NOTE:  Display routines are not to be confused with output
//        routines.  Output routines are used for displaying
//        formatted text, whereas display routines are used for
//        displaying relative session structures (program data).


// Display a string entry
BP_ERROR_T BP_DisplaySFSStringEntry
(
		P_BP_SFS_STRING_ENTRY string_entry,
		P_BP_SFS_INFO_DISPLAY_SETTINGS display_settings
);

// Display a column
BP_ERROR_T BP_DisplaySFSColumn
(
		P_BP_SFS_STRING_COLUMN column,
		P_BP_SFS_INFO_DISPLAY_SETTINGS display_settings

);

// display a column group
BP_ERROR_T BP_DisplaySFSColumnGroup
(
		P_BP_SFS_STRING_COLUMN_GROUP column_group,
		P_BP_SFS_INFO_DISPLAY_SETTINGS display_settings
);

// Display a row
BP_ERROR_T BP_DisplaySFSRow
(
		P_BP_SFS_STRING_ROW row,
		P_BP_SFS_INFO_DISPLAY_SETTINGS display_settings
);

// display a row group
BP_ERROR_T BP_DisplaySFSRowGroup
(
	P_BP_SFS_STRING_ROW_GROUP row_group,
	P_BP_SFS_INFO_DISPLAY_SETTINGS display_settings
);

// display a string formatting session
BP_ERROR_T BP_DisplayStringFormattingSession
(
		P_BP_SF_SESSION session,
		P_BP_SFS_INFO_DISPLAY_SETTINGS display_settings
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Entry Painting Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%






// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Final Output %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// This routine will display final output generated and display it to the user via
// stdout.
BP_ERROR_T BP_DisplayFinalOutputSFSession(P_BP_SF_SESSION session, BP_BOOL display_colored);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Formatted Output Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// These routines are used for displaying a formatted session to
// the end-user.  These routines combine all selected elements
// of a formatted display session, and allow display of data.  For
// all intents and purposes, the only routine you should ever have to
// call is BP_GenerateFinalOutputSFSession.

// Generate the final output.  This output is stored in the session itself
// as:
//
// session->(char **)final_output_rows;
// session->(size_t)final_output_row_n;
//
// You should only ever have to call this routine when trying to generate
// finalized output.
BP_ERROR_T BP_GenerateFinalOutputSFSession(P_BP_SF_SESSION session);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// ===== Other Output Utility Routines (mainly used by BP_GenerateFinalOutputSFSession above ======
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/*
 *
 * The following routines are used to generate final formatted data.  They must be called in sequence
 * in order for meaningful data to be created.
 *
 * - Columns require string data.
 * - Column groups require column data.
 * - Rows require column group data.
 * - Row groups require row data.
 *
 * All of these routines are called in the correct order by BP_GenerateFinalOutputSession(session);
 *
 */

// Call this first.
BP_ERROR_T BP_ColumnizeAllStringEntriesSFSession(P_BP_SF_SESSION session);

// Call this second.
BP_ERROR_T BP_ColumnizeAllColumnEntriesSFSession(P_BP_SF_SESSION session);

// Call this third.
BP_ERROR_T BP_ColumnizeAllColumnGroupEntriesSFSession(P_BP_SF_SESSION session);

// Call this fourth.
BP_ERROR_T BP_ColumnizeAllRowEntriesSFSession(P_BP_SF_SESSION session);

// Call this fifth.
BP_ERROR_T BP_ColumnizeAllRowGroupEntriesSFSession(P_BP_SF_SESSION session);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Clean up routines for columization routines %%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Simply destroys the data within the non_colorized_final_ouptut and colorized_final_output data
// stored in each individual string entry.  This is mainly a utility routine and probably shouldn't
// be used directly.
BP_ERROR_T BP_DestroyAllStringEntryColumnizedDataSFSession(P_BP_SF_SESSION session);






// Simple routine to display a string entry.  Mostly used for just displaying a simple
// colorized buffer without regard to any column / sfs session data.  Usually done for
// debugging purposes.
BP_ERROR_T BP_OutputStringEntryAsIsWithoutColumnization(P_BP_SFS_STRING_ENTRY entry);

// Generate an output data from string entry.  The data is stored directly within
// the string entry itself.
BP_ERROR_T BP_GenerateOutputStringArrayFromStringEntrySFSession
(
	P_BP_SF_SESSION sfs,
	P_BP_SFS_STRING_ENTRY entry,
	BP_BOOL enable_colorization
);

// Formatted row.  If output_only is set to BP_TRUE, no formatted
// buffer is returned.  If it is set to BP_TRUE, the formatted value is returned
// as a pointer.  Erroneous conditions are returned as a null value.  The upside
// to outputting directly is that the buffer formatted won't be stored in memory
// except for one row at a time.
char ** BP_FormatRowForOutputSFSSession
(
	P_BP_SF_SESSION session,
	P_BP_SFS_STRING_ROW row,
	BP_BOOL output_only
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Utility Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// counts the number of string entries in the row
size_t BP_CountNumberOfStringEntriesInRowSFSession(P_BP_SFS_STRING_ROW row);

// Get the max width for a row.  This is primarily utilized to generate lines
// for the output buffer.
size_t BP_GetRowMaxWidthOfAllStringEntriesSFSession(P_BP_SFS_STRING_ROW row);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Session Constructor / Desconstructor %%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Creates a new string formatting session
P_BP_SF_SESSION BP_CreateStringFormattingSession(char * session_name, P_BP_STRING_ROW_BORDER_SET border_set = NULL);

// destroy the string formatting session
BP_ERROR_T BP_DestroyStringFormattingSession(P_BP_SF_SESSION formatting_session);



#endif /* BP_STRINGFORMATTINGSESSIONS_H_ */

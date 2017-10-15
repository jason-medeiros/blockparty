/*
 * BP-BufferAnalysisASCII.h
 *
 *  Created on: Jul 24, 2014
 *      Author: root
 */

#ifndef BP_BUFFERANALYSISASCII_H_
#define BP_BUFFERANALYSISASCII_H_




// This structure holds configuration flags for the search engine.  Only enabled
// features are processed.
typedef struct __BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION {

	// if this flag is set to true, the number of ascii characters in
	// the currently analyzed buffer will be counted.
	BP_BOOL count_ascii;

	// if this flag is set to true, the number of extended ascii characters
	// in the buffer will be counted.
	BP_BOOL count_extended;

	// counts the number of individual characters within the buffer (will count
	// every A every B every C every, ..., every 0xff).  Counts are stored within
	// a
	BP_BOOL count_individual_characters;

} BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION, *P_BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION;



// This structure holds the number of certain types of characters (defined in
// BP-ASCIICharts.h) encountered during buffer analysis.  The entries in this structure
// can be matched to the similar entries in the chart BP_ASCII_CHAR_FLAGS structures.
typedef struct __BP_BUFFER_ANALYSIS_ASCII_FLAG_COUNTS {

	// number of contol characters encountered
	size_t control_n;

	// number of alpha lower characters encountered
	size_t alphalower_n;

	// number of alpha caps characters encountered
	size_t alphacaps_n;

	// number of 0-9 number characters encountered
	size_t number_n;

	// number of printable characters encountered
	size_t printable_n;

	// number of non-printable characters encountered
	size_t nonprintable_n;

	// number of extended characters encountered
	size_t extended_n;

	// number of math characters encountered
	size_t math_n;

	// number of superscript characters encountered
	size_t superscript_n;

	// number of currency symbols encountered
	size_t currency_n;

	// number of extened ascii fraction symbols encountered
	size_t fraction_n;

	// number of extended ascii acute latin symbols encountered
	size_t acute_n;

	// number of extended ascii grave latin symbols encountered
	size_t grave_n;

	// number of extended ascii diaeresis latin symbols encountered
	size_t diaeresis_n;

	// number of slash (extened or non-extended) characters encountered
	size_t slash_n;

	// number of extended ascii circumflex latin symbols encountered
	size_t circumflex_n;

	// number of extended ascii caron latin symbols encountered
	size_t caron_n;

	// number of extended ascii macron latin symbols encountered
	size_t macron_n;

	// number of extended ascii punctuation latin symbols encountered
	size_t punctuation_n;

	// number of symbols encountered (all symbols)
	size_t symbol_n;

	// number of extended latin symbols encountered
	size_t latin_n;

	// number of extended ascii latin nonstandard caps symbols encountered
	size_t latin_nonstandard_caps_n;

	// number of extended ascii latin nonstandard lowercase symbols encountered
	size_t latin_nonstandard_lower_n;

	// number of quotation (lower or non-lower) characters encountered
	size_t quotation_n;

	// number of whitespace characters encountered
	size_t whitespace_n;

	// number of undefined characters encountered
	size_t undefined_n;

} BP_BUFFER_ANALYSIS_ASCII_FLAG_COUNTS, *P_BP_BUFFER_ANALYSIS_ASCII_FLAG_COUNTS;


// this structure holds a series of flags associated with buffer analysis, and is
// used primarily by the BP_BUFFER_ANALYSIS_ASCII_STRINGS structure, to indicate
// the type of string we've encountered.
typedef struct __BP_BUFFER_ANALYSIS_ASCII_STRING_FLAGS {

	// NOTE:  These can be inaccurate.  For example, a unix file path
	//        could be a relative path for a url.  This matching should
	//        be taken with an grain of salt.

	// if it's a unix file path, mark here
	BP_BOOL is_unix_file_path;

	// if it's a url, mark here
	BP_BOOL is_url;

	// if the url is a strict url with matching TLD
	BP_BOOL is_strict_url_matching_with_tld;

	// if it's a host URL, mark here
	BP_BOOL is_host_url;

	// if it's a base URL, mark here
	BP_BOOL is_base_url;

	// if it's a script url, mark here
	BP_BOOL is_script_url;

	// if it's a path url, mark here
	BP_BOOL is_path_url;

	// if it's a unix dynamic library, list here
	BP_BOOL is_dynamic_so;

	// if it exists on the filesystem as a direct path
	BP_BOOL exists_as_file_on_filesystem;

	// checks to see if the file is readable (only set if exists_as_file_on_filesystem is ok)
	BP_BOOL exists_as_file_on_filesystem_and_is_readable;

	// checks to see if the file is writable (only set if exists_as_file_on_filesystem is ok)
	BP_BOOL exists_as_file_on_filesystem_and_is_writable;


} BP_BUFFER_ANALYSIS_ASCII_STRING_FLAGS, *P_BP_BUFFER_ANALYSIS_ASCII_STRING_FLAGS;


// Enumerator of search flag values.  Used for matching flags in BP_BUFFER_ANALYSIS_ASCII_STRING_FLAGS
// to specific values created by the secondary analysis routine (BP_RunSecondaryStringAnalysisOnExtractedStrings).
typedef enum BP_ASCII_STRING_SEARCH_FLAG_VALUES {
	BP_ASCII_STRING_SEARCH_NONE,
	BP_ASCII_STRING_SEARCH_NEGATIVE_MATCH,
	BP_ASCII_STRING_SEARCH_POSITIVE_MATCH,
	BP_ASCII_STRING_SEARCH_PARTIAL_POSITIVE_MATCH,
	BP_ASCII_STRING_SEARCH_PARTIAL_NEGATIVE_MATCH,
	BP_ASCII_STRING_SEARCH_ALL_SEARCH_FLAG_CONDITIONS_MET,           /* This is the most important flag, as it indicates that all the flags were matched during search */
	BP_ASCII_STRING_SEARCH_ALL_SEARCH_FLAG_CONDITIONS_MET_NEGATIVELY /* Second most important flag, indicates that of all the flags passed in, all were matched negatively */
};



// This structure exists within the BP_BUFFER_ANALYSIS_ASCII structure and
// is used to hold strings which have been extracted from a buffer.
typedef struct __BP_BUFFER_ANALYSIS_ASCII_STRINGS {


	// This indicates whether or not the string has been marked as restricted or not.  Restricted
	// strings are omitted from display routines.
	BP_BOOL marked_as_restricted;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% String Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// the position in the analyzed buffer (aka offset)
	size_t position_in_analyzed_buffer;

	// length of string (in bytes)
	size_t string_length;

	// the address of the string in the buffer (not a duplicated string)
	unsigned char * address_of_string_in_buffer;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% General ASCII Flag Restrictions %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// These flags are set automatically when BP_ExtractASCIIStringsFromAnalyzedBuffer is
	// called.  They are the value of the passed in parameters match_flags, and match_flags_how.
	// The values are COPIED IN to the values below, and are not references.

	// ==== Positive Match Flags =================

	// flag indicating whether or not the match flags are enabled
	BP_BOOL match_flags_enabled;

	// actual flags used to generate this value
	BP_ASCII_CHAR_FLAGS match_flags;

	// the enumerated type match provided by the user, POSITIVE/NEGITIVE/PARTIAL/ALL SETTINGS
	BP_ASCII_CHARACTER_SEARCH_FLAG_VALUES match_flags_how;

	// ==== Negative Match Flags =================

	// flag indicating whether or not negative match flags are enabled
	BP_BOOL negative_match_flags_enabled;

	// negative match flags
	BP_ASCII_CHAR_FLAGS negative_match_flags;

	// the enumerate type match provided by the user for negative matching
	BP_ASCII_CHARACTER_SEARCH_FLAG_VALUES negative_match_flags_how;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Character Restrictions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// If the user wants to restrict around a literal string of characters
	// that can be done with these values.  These values are set via the
	// BP_ExtractASCIIStringsFromAnalyzedBuffer routine, and are passed in
	// as a value to that function.  These values are duplicated and stored
	// here as a tq allocated buffer.  Do not free them directly, the analysis
	// destroy routine does it automatically.

	// actual string of restricted characters
	unsigned char * restricted_characters;

	// number of restricted characters in the buffer
	size_t restricted_characters_n;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% String Terminator Sequence %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// String terminators are used for determining when a string is terminated within
	// an analyzed buffer.

	// this is the terminator used for terminating the string.
	unsigned char *string_terminator;

	// length of string terminator
	size_t string_terminator_length;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% String Flags/Secondary Analysis %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// determines if the string is quoted or not
	BP_STRING_QUOTE_CHECK_RESULTS string_quote_info;


	// flag indicating whether or not the analysis was completed
	BP_BOOL string_flag_analysis_completed;

	// flags associated with this string
	BP_BUFFER_ANALYSIS_ASCII_STRING_FLAGS string_flags;




} BP_BUFFER_ANALYSIS_ASCII_STRINGS, *P_BP_BUFFER_ANALYSIS_ASCII_STRINGS;




// ascii buffer analysis
typedef struct __BP_BUFFER_ANALYSIS_ASCII {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% ASCII Character Map (for lookups) %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Character map (destroyed when analysis is destroyed).  This is used
	// for generating accurate flag counts.
	P_BP_ASCII_CHARACTER_MAP char_map;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%% Generic Character Count Flags %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// contains counts of various flags.
	BP_BUFFER_ANALYSIS_ASCII_FLAG_COUNTS ascii_char_flag_counts;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Character Encounter Counts %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// For every byte, there exists an index in this table.  Each index
	// contains a count.  To use this, index it as follows.
	// char_counts['A']; char_counts['B'] etc.  For nonstandard ascii, you can
	// supply char_counts[0xfe]; etc.
	size_t char_counts[256];


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% String Data (generated if user requested) %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// strings extracted
	P_BP_BUFFER_ANALYSIS_ASCII_STRINGS strings;

	// number of ascii strings extracted
	size_t string_n;


} BP_BUFFER_ANALYSIS_ASCII, *P_BP_BUFFER_ANALYSIS_ASCII;


#endif /* BP_BUFFERANALYSISASCII_H_ */

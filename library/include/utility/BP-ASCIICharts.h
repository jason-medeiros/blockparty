/*
 * BP-ASCIICharts.h
 *
 *  Created on: Jul 14, 2014
 *      Author: root
 */

#ifndef BP_ASCIICHARTS_H_
#define BP_ASCIICHARTS_H_


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Basic ASCII Character Charts %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// flags associated with the current character (not a bitfield to prevent endian
// portability issues)
typedef struct __BP_ASCII_CHAR_FLAGS {
	unsigned char is_control;                   // finished tagging (by hand)
	unsigned char is_alphalower;   				// finished tagging (by hand)
	unsigned char is_alphacaps;    				// finished tagging (by hand)
	unsigned char is_number;       				// finished tagging (by hand)
	unsigned char is_printable;    				// finished tagging (by hand)
	unsigned char is_nonprintable; 				// finished tagging (by hand)
	unsigned char is_extended;     				// finished tagging (by hand)
	unsigned char is_math;         				// finished tagging (by hand)
	unsigned char is_superscript;  				// finished tagging (by hand)
	unsigned char is_currency;     				// finished tagging (by hand)
	unsigned char is_fraction;     				// finished tagging (by hand)
	unsigned char is_acute;        				// finished tagging (by hand)
	unsigned char is_grave;        				// finished tagging (by hand)
	unsigned char is_diaeresis;    				// finished tagging (by hand)
	unsigned char is_slash;        				// finished tagging (by hand)
	unsigned char is_circumflex;   				// finished tagging (by hand)
	unsigned char is_caron;        				// finished tagging (by hand)
	unsigned char is_macron;       				// finished tagging (by hand)
	unsigned char is_punctuation;  				// finished tagging (by hand)
	unsigned char is_symbol;                    // finished tagging (by hand)
	unsigned char is_latin;        				// finished tagging (by hand)
	unsigned char is_latin_nonstandard_caps;    // finished tagging (by hand)
	unsigned char is_latin_nonstandard_lower;   // finished tagging (by hand)
	unsigned char is_quotation;                 // finished tagging (by hand)
	unsigned char is_whitespace;                // finished tagging (by hand)
	unsigned char is_undefined;
} BP_ASCII_CHAR_FLAGS, *P_BP_ASCII_CHAR_FLAGS;



// this structure holds the basic ascii chart
typedef struct __BP_ASCII_CHART {

	// hexidecimal representation of ascii character
	unsigned char ascii_char;

	// abbreviation of the symbol, as a string
	char * symbol_abbreviation;

	// description of the symbol (from available ascii chart references found)
	char * symbol_description;

	// associated html number (aka &#134; for html dagger)
	char * html_numeric_code;

	// associated html name for character (aka &euro; for euro sign)
	char * html_name;

	// structure to hold flags associated with this character
	BP_ASCII_CHAR_FLAGS flags;

	// flag indicating whether or not the character is restricted from being included
	// in various operations (such as search operations)
	BP_BOOL is_restricted;


} BP_ASCII_CHART, *P_BP_ASCII_CHART, *P_BP_ASCII_CHART_ENTRY, **P_BP_ASCII_CHART_ENTRIES;



// This structure is used for handling ascii types
typedef struct __BP_ASCII_CHARACTER_MAP {


	// tail queue allocator used for all allocations
	P_BP_LINKL tq;

	// ascii chart
	P_BP_ASCII_CHART ascii_chart;

	// number of entries in the chart
	size_t ascii_chart_entry_n;


} BP_ASCII_CHARACTER_MAP, *P_BP_ASCII_CHARACTER_MAP;


// this structure holds ascii character search results
typedef struct __BP_ASCII_CHARACTER_MAP_SEARCH_RESULTS {

	// tailqueue allocator used for stacking results
	P_BP_LINKL tq;

	// this is a pointer stack of results, the individual
	// results are simply references to a character map entry.
	// This is allocated as an array of pointers, but only the
	// chart entries structure should every be free'd.  Never
	// free the individual results, as that job is done by
	// the character map deconstructor.
	P_BP_ASCII_CHART_ENTRIES results;

	// number of results which were returned as part of a search.
	size_t result_n;

} BP_ASCII_CHARACTER_MAP_SEARCH_RESULTS, *P_BP_ASCII_CHARACTER_MAP_SEARCH_RESULTS;


// Returns one of four values found in the BP_ASCII_CHARACTER_SEARCH_FLAG_VALUES enumerator (defined in this
// header file) based on a compare of two flag sets.  This code is inlined in more complicated fashion
// within the actual search routine.  This is here simply as a general utility which can be used
// for more customized purposes.
typedef enum BP_ASCII_CHARACTER_SEARCH_FLAG_VALUES {
	BP_ASCII_CHAR_SEARCH_NONE,
	BP_ASCII_CHAR_SEARCH_NEGATIVE_MATCH,
	BP_ASCII_CHAR_SEARCH_POSITIVE_MATCH,
	BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH,
	BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH,
	BP_ASCII_CHAR_SEARCH_ALL_SEARCH_FLAG_CONDITIONS_MET,  /* This is the most important flag, as it indicates that all the flags were matched during search */
	BP_ASCII_CHAR_SEARCH_ALL_SEARCH_FLAG_CONDITIONS_MET_NEGATIVELY /* Second most important flag, indicates that of all the flags passed in, all were matched negatively */
};

// simple macro used in the chart addition routines below for moving to the next chart position
#define ASCII_CHART_MOVE_NEXT chart = &chart[1];

// simple macro utilized to cleanly allocate strings within a character maps tail queue.
#define ASCII_CHART_ADDSTR(x) (char *) tq_memdup(char_map->tq, (char *) x, strlen(x), BPLN);


// Return the number of flags which are set (BP_TRUE) within a BP_ASCII_CHAR_FLAGS structure.  Primarily used for
// matching flags.
size_t BP_CountNumberASCIICharFlagsSet(P_BP_ASCII_CHAR_FLAGS flags);

// Returns a BP_FLAGS_T value set with any combination of BP_ASCII_CHARACTER_SEARCH_FLAG_VALUES enumerated
// values.
BP_FLAGS_T BP_CompareASCIICharFlags(P_BP_ASCII_CHAR_FLAGS flags1, P_BP_ASCII_CHAR_FLAGS flags2);



// generates an ascii character map
P_BP_ASCII_CHARACTER_MAP BP_CreateASCIICharacterMap();

// destroys an ascii character map (frees all memory, including the P_BP_ASCII_CHARACTER_MAP structure)
BP_ERROR_T BP_DestroyASCIICharacterMap(P_BP_ASCII_CHARACTER_MAP char_map);

// fills a character map with a standard/extended ascii chart
BP_ERROR_T BP_AddASCIIChartToCharacterMap(P_BP_ASCII_CHARACTER_MAP char_map);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Chart Search Routines (fast/additive index lookups) %%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Returns a chart entry based on the search character provided, and the assorted flags.  Will return
// null if a character does not match with the provided flags given.  To omit flag checks and to just
// run a fast lookup, provide the flags parameter as a NULL pointer value.  This returns a maximum of
// one char.
P_BP_ASCII_CHART_ENTRY BP_LookupASCIIChar(P_BP_ASCII_CHARACTER_MAP char_map, unsigned char character, P_BP_ASCII_CHAR_FLAGS flags);

// Search the ascii table for entries matching the provided flags.  The result returned from this
// function must be destroyed via BP_DestroyASCIISearchResults.
P_BP_ASCII_CHARACTER_MAP_SEARCH_RESULTS BP_CreateSearchMapFromASCIITable(P_BP_ASCII_CHARACTER_MAP char_map, P_BP_ASCII_CHAR_FLAGS flags, BP_ASCII_CHARACTER_SEARCH_FLAG_VALUES search_flags);

// This simply adds a specified char to a search result stack.  It is used primarily
// by BP_SearchASCIITable() to stack up matches.
BP_ERROR_T BP_AddCharacterReferenceToSearchResults(P_BP_ASCII_CHARACTER_MAP_SEARCH_RESULTS results, P_BP_ASCII_CHART_ENTRY char_entry);

// Destroys the results.  After calling this on a valid results structure, all memory including
// the results structure itself will be freed.
BP_ERROR_T BP_DestroyASCIISearchResults(P_BP_ASCII_CHARACTER_MAP_SEARCH_RESULTS results);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Chart Character Restriction Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This will mark the restricted flag in each BP_ASCII_CHART entry (aka character
// that matches a byte in the restricted chars string).  The restricted flag is matched
// in a variety of analysis functions.  For example, when extracting strings using
// the buffer analysis routines in BP-BufferAnalysis.h, restricted characters are
BP_ERROR_T BP_RestrictASCIICharsInMap(P_BP_ASCII_CHARACTER_MAP char_map, unsigned char *restricted_chars, size_t restricted_char_string_n);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Chart Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// displays one singular chart entry
BP_ERROR_T BP_DisplayASCIIChartEntry(P_BP_ASCII_CHART_ENTRY chart_entry);

// Displays entries in the character map provided.  If a flags parameter is passed in, only the entries
// that match the specific flag configuration are displayed.
BP_ERROR_T BP_DisplayASCIICharacterMap(P_BP_ASCII_CHARACTER_MAP char_map, P_BP_ASCII_CHAR_FLAGS flags);

// Displays ascii character map search results.  If a flags parameter is passed in, only the entries
// that match the specific flag configuration are displayed.
BP_ERROR_T BP_DisplayASCIICharacterMapSearchResults(P_BP_ASCII_CHARACTER_MAP_SEARCH_RESULTS search_results, P_BP_ASCII_CHAR_FLAGS flags);

// Simple routine designed to display the settings within a flags bitfield.  The display routine
// will display which flags have been set.  This is not to be confused with the flags structure
// used within the ascii character map.  This works as a BITFIELD.
BP_ERROR_T BP_DisplayASCIICompareMatchFlags(BP_FLAGS_T match_flags);



#endif /* BP_ASCIICHARTS_H_ */

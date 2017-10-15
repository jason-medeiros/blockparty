/*
 * BP-BufferAnalysis.h
 *
 *  Created on: Jul 24, 2014
 *      Author: root
 */

#ifndef BP_BUFFERANALYSIS_H_
#define BP_BUFFERANALYSIS_H_

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% About Buffer Analysis %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/*

 Since this portion of the code is farily convoluted and outside simple
 description, I'll try to define the portioning and intent of this code.

 Essentially, this code provides a simple interface to examine an unknown buffer
 for certain traits which will allow us to gain meaningful information from
 the buffer, using only programatic inferrance.

 While the code itself is very complex, the interface is extremely simple to use
 requiring only the creation of a managment structure, some configuraiton flags
 and a call to a routine.  Using the configuration flag structure, you can enable
 or disable certain analysis subsections.  You can also simply enable all options
 to do a thorough analysis.



 Steps of Analysis:

 Each of the following analysis steps rely on depth oriented routines, so they are not
 each called directly in succession, whereas they need to be individually called.  This is
 to prevent additional unnecessary or unwanted processing from occuring.

	1) Basic analysis of buffer (character counts/total char types/etc)
		- very fast
		- easy way to determine if a buffer has unwanted or restricted characters

	2) String Extracting/Marking (deliniates portions of the buffer to submit to more advanced forms of analysis)
		- extracts strings from an existing buffer
		- strings are deliniated by user supplied string terminator
		- strings can be restricted to specific character sets/types/flag configurations\

	3) Basic String analysis (determines if strings match certain known types, urls, paths, special values)
		- utilizes regular expressions to determine if a string matches a known pattern

	4) Advanced Buffer Analysis
		- utilizes data from a unix magic (/etc/magic or /usr/share/magic) file to mark the entire buffer
		  as a file type (if it exists)

	5) Advanced String Analysis (magic file matching, custom string analysis routines)
		- utilizes data from a unix magic (/etc/magic or /usr/share/magic) file to mark strings
		  as file types.

 Known Uses:

	1) Parse out file portions and apply magic analysis, or determine if magic analysis fails.  This
	   can be useful for determining if a segment is holding a known data type, which can be parsed.

	2) Restrict a buffer to certain character sets, such as by way of validating data.  For example, you can
	   use a flag match to ensure that a buffer does not contain extended characters, or other erroneous data types.

	3) Match a buffer to ensure it's a certain type, such as a unix file path, or a windows file path.

	4) Determine a file type without using the unix file command.



 Author:  Jason A. Medeiros

 */

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Main Buffer Analysis Structure Definitions %%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



// This structure holds all buffer analysis data
typedef struct __BP_BUFFER_ANALYSIS {

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Memory Allocator for Analysis Routines %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// All buffer analysis routines which cascade into this structure
	// rely on this memory allocator.  This allocator is destroyed along
	// with this structure when BP_DestroyBufferAnalysis is called.

	P_BP_LINKL tq;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Pointer and Size of Buffer Analyzed %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// buffer to analyze (not duplicated, this is just a pointer, do not free
	// this in the destructor, free it outside if necessary)
	unsigned char * buffer;

	// bytes in the buffer being analyzed
	size_t          buffer_n;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Individual Cascading Analsysis Structures %%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// contains ASCII buffer analysis if so configured
	P_BP_BUFFER_ANALYSIS_ASCII ascii;


} BP_BUFFER_ANALYSIS, *P_BP_BUFFER_ANALYSIS;


/*
 *
 * Main Buffer Analysis Routine:
 * ==================================================
 * Pass in NULL values to disable any analysis configuration subsystem.  The
 * output structure needs to be freed with BP_DestroyBufferAnalysis.
 *
 */

P_BP_BUFFER_ANALYSIS BP_CreateBufferAnalysis
(
	unsigned char *buffer,
	size_t         buffer_n,
	P_BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION ascii_config
);


// destroys any memory associated with a buffer analysis
BP_ERROR_T BP_DestroyBufferAnalysis(P_BP_BUFFER_ANALYSIS analysis);

// displays a buffer analysis structure and all provided sub-structures
BP_ERROR_T BP_DisplayBufferAnalysis(P_BP_BUFFER_ANALYSIS analysis);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Flag Count / Analysis %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Returns a BP_FLAGS_T value set with any combination of BP_ASCII_CHARACTER_SEARCH_FLAG_VALUES enumerated
// values.
BP_FLAGS_T BP_CompareASCIIStringFlags(P_BP_BUFFER_ANALYSIS_ASCII_STRING_FLAGS flags_compare_this, P_BP_BUFFER_ANALYSIS_ASCII_STRING_FLAGS flags_compare_with_this);

// Return the number of flags which are set (BP_TRUE) within a P_BP_BUFFER_ANALYSIS_ASCII_STRING_FLAGS structure.  Primarily used for
// matching flags.
size_t BP_CountNumberASCIIStringFlagsSet(P_BP_BUFFER_ANALYSIS_ASCII_STRING_FLAGS flags);

// displays what flags are set in the match flags
BP_ERROR_T BP_DisplayASCIIStringMatchFlags(BP_FLAGS_T match_flags);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Mark Restricted Strings %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Marks the strings.marked_as_restricted marker with the value of mark_restricted_status based
// on the provided flags, and the match how setting.  If you set the flags_match value to null, it will
// match all entries.
BP_ERROR_T BP_MarkRestrictedOnExtractedStrings
(
	P_BP_BUFFER_ANALYSIS                    buff_analysis,
	BP_BOOL                                 mark_restricted_status,
	P_BP_BUFFER_ANALYSIS_ASCII_STRING_FLAGS flags_match,
	BP_ASCII_STRING_SEARCH_FLAG_VALUES      flags_match_how
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% ASCII Buffer Analysis %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// creates an ascii buffer analysis based around the configuration provided.  The analysis which is generated is
// stored as a pointer within the P_BP_BUFFER_ANALYSIS pointer provided as first function parameter.
BP_ERROR_T BP_CreateBufferAnalysisASCII(P_BP_BUFFER_ANALYSIS buff_analysis, P_BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION analysis_config);


// Calculates ASCII strings which are found in the buffer, and stores them within the buffer analysis.  The
// extracted strings are stored within buff_analysis->ascii->strings structure stack, and the total number
// of strings is stored in analysis->ascii->strings_n.  You can navigate them by iterating through the stack
// indexes till you reatch strings_n-1 top.  This routine will automatically stack new results into the strings
// stack, so it can be called recursively for a variety of different line break sequences.  An example line break
// sequence could be like the following (null character break):
//
// unsigned char * line_break = "\x00";
// size_t          sequence_len = 1;
//
BP_ERROR_T BP_ExtractASCIIStringsFromAnalyzedBuffer
(
		P_BP_BUFFER_ANALYSIS                  buff_analysis,
		P_BP_ASCII_CHAR_FLAGS                 match_flags,
		BP_ASCII_CHARACTER_SEARCH_FLAG_VALUES match_flags_how,
		P_BP_ASCII_CHAR_FLAGS                 negative_match_flags,
		BP_ASCII_CHARACTER_SEARCH_FLAG_VALUES negative_match_flags_how,
		unsigned char *                       line_break_sequence,
		size_t                                line_break_sequence_n,
		unsigned char *                       restricted_chars,
		size_t                                restricted_chars_n,
		size_t                                minimum_string_length
);


// adds a string to a ascii portion of buffer analyis.  This is a utility routine used by BP_ExtractASCIIStringsFromAnalyzedBuffer
// and probably shouldn't be used directly, although it can be, depending on the current predicament.  The memory allocated here
// is automatically freed() when BP_DestroyBufferAnalysis is called (via tail queue destruction)
BP_ERROR_T BP_AddASCIIStringToBufferAnalysis
(
	P_BP_BUFFER_ANALYSIS                  buff_analysis,
	size_t                                position_in_analyzed_buffer,
	size_t                                string_len,
	unsigned char *                       address_of_string_in_buffer,
	P_BP_ASCII_CHAR_FLAGS                 match_flags,
	BP_ASCII_CHARACTER_SEARCH_FLAG_VALUES match_flags_how,
	P_BP_ASCII_CHAR_FLAGS                 neative_match_flags,
	BP_ASCII_CHARACTER_SEARCH_FLAG_VALUES negative_match_flags_how
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Secondary Analysis Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// The secondary analysis routines provided here are used for the purpose of determining
// certain string characteristics, and are computationally expensive.  They use regular
// expressions to determine data types found.  This secondary analysis should not be used
// unless you know that the buffer sizes will be reasonable, or you could find yourself
// within an exhaustive memory condition.

// run secondary analysis on extracted strings
BP_ERROR_T BP_RunSecondaryStringAnalysisOnExtractedStrings(P_BP_BUFFER_ANALYSIS buff_analysis);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// display the buffer analysis ascii portion. (Provide null flags_match parameter to match all)
BP_ERROR_T BP_DisplayBufferAnalysisASCII
(
	P_BP_BUFFER_ANALYSIS buff_analysis
);

// display an ascii string structure (found within buff analysis) (Provide null flags_match parameter to match all)
BP_ERROR_T BP_DisplayBufferAnalysisASCIIString
(
	P_BP_BUFFER_ANALYSIS_ASCII_STRINGS      buff_string,
	BP_BOOL                                 display_string_data
);

#endif /* BP_BUFFERANALYSIS_H_ */

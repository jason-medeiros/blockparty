/*
 * BP-BufferAnalysisASCII.cc
 *
 *  Created on: Jul 24, 2014
 *      Author: root
 */



// include main functions
#include "../../../include/BP-Main.h"

// creates an ascii buffer analysis based around the configuration provided.  The analysis which is generated is
// stored as a pointer within the P_BP_BUFFER_ANALYSIS pointer provided as first function parameter.
BP_ERROR_T BP_CreateBufferAnalysisASCII(P_BP_BUFFER_ANALYSIS buff_analysis, P_BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION analysis_config)
{

	// ensure we have valid pointers, if not return null
	if(!buff_analysis || !analysis_config)
		return ERR_FAILURE;

	// ensure we have a tail queue allocator to work with
	if(!buff_analysis->tq)
		return ERR_FAILURE;

	// create pointer within the buffer analysis structure, using the analysis structure
	// tailqueue allocator.
	buff_analysis->ascii = (P_BP_BUFFER_ANALYSIS_ASCII) tq_calloc
	(
		buff_analysis->tq,
		sizeof(BP_BUFFER_ANALYSIS_ASCII),
		1,
		BPLN
	);

	// ensure we created the ascii structure ok, if not return
	if(!buff_analysis->ascii)
		return ERR_FAILURE;

	// localized reference for code clarity purposes
	P_BP_BUFFER_ANALYSIS_ASCII ascii_analysis = buff_analysis->ascii;

	// now create a character map which we can use for analysis purposes
	ascii_analysis->char_map = BP_CreateASCIICharacterMap();

	// if we couldn't create the map, exit here
	if(!ascii_analysis->char_map)
		return ERR_FAILURE;


	// current byte index (for readability)
	P_BP_ASCII_CHART current_char_index = NULL;

	// reference to current character
	unsigned char curr_character = 0;

	// now walk each position in the buffer provided within the analysis
	// structure, and update ascii analysis appropriately
	size_t n = 0;
	for(; n < buff_analysis->buffer_n; n++)
	{

		// set character reference
		curr_character = buff_analysis->buffer[n];

		// current character index
		current_char_index = &ascii_analysis->char_map->ascii_chart[curr_character];

		// increment the character counter at the current index
		ascii_analysis->char_counts[curr_character]++;

		// debugging print statement:
		// printf("\n Current Char: %.02x - %.02x ", current_char_index->ascii_char, buff_analysis->buffer[n]);

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Now Update Flag Counts %%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// Since all the flags in the current character index are either 1 or 0, we can
		// simply add all values directly to the analysis flag count structure.  Simple
		// as could be.

		ascii_analysis->ascii_char_flag_counts.control_n                 += current_char_index->flags.is_control;

		ascii_analysis->ascii_char_flag_counts.alphalower_n              += current_char_index->flags.is_alphalower;

		ascii_analysis->ascii_char_flag_counts.alphacaps_n               += current_char_index->flags.is_alphacaps;

		ascii_analysis->ascii_char_flag_counts.number_n                  += current_char_index->flags.is_number;

		ascii_analysis->ascii_char_flag_counts.printable_n               += current_char_index->flags.is_printable;

		ascii_analysis->ascii_char_flag_counts.nonprintable_n            += current_char_index->flags.is_nonprintable;

		ascii_analysis->ascii_char_flag_counts.extended_n                += current_char_index->flags.is_extended;

		ascii_analysis->ascii_char_flag_counts.math_n                    += current_char_index->flags.is_math;

		ascii_analysis->ascii_char_flag_counts.superscript_n             += current_char_index->flags.is_superscript;

		ascii_analysis->ascii_char_flag_counts.currency_n                += current_char_index->flags.is_currency;

		ascii_analysis->ascii_char_flag_counts.fraction_n                += current_char_index->flags.is_fraction;

		ascii_analysis->ascii_char_flag_counts.acute_n                   += current_char_index->flags.is_acute;

		ascii_analysis->ascii_char_flag_counts.grave_n                   += current_char_index->flags.is_grave;

		ascii_analysis->ascii_char_flag_counts.diaeresis_n               += current_char_index->flags.is_diaeresis;

		ascii_analysis->ascii_char_flag_counts.slash_n                   += current_char_index->flags.is_slash;

		ascii_analysis->ascii_char_flag_counts.circumflex_n              += current_char_index->flags.is_circumflex;

		ascii_analysis->ascii_char_flag_counts.caron_n                   += current_char_index->flags.is_caron;

		ascii_analysis->ascii_char_flag_counts.macron_n                  += current_char_index->flags.is_macron;

		ascii_analysis->ascii_char_flag_counts.punctuation_n             += current_char_index->flags.is_punctuation;

		ascii_analysis->ascii_char_flag_counts.symbol_n                  += current_char_index->flags.is_symbol;

		ascii_analysis->ascii_char_flag_counts.latin_n                   += current_char_index->flags.is_latin;

		ascii_analysis->ascii_char_flag_counts.latin_nonstandard_caps_n  += current_char_index->flags.is_latin_nonstandard_caps;

		ascii_analysis->ascii_char_flag_counts.latin_nonstandard_lower_n += current_char_index->flags.is_latin_nonstandard_lower;

		ascii_analysis->ascii_char_flag_counts.quotation_n               += current_char_index->flags.is_quotation;

		ascii_analysis->ascii_char_flag_counts.whitespace_n              += current_char_index->flags.is_whitespace;

		ascii_analysis->ascii_char_flag_counts.undefined_n               += current_char_index->flags.is_undefined;

	}



	// return indicating success (analysis created successfully)
	return ERR_SUCCESS;

}

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
)
{

	// ensure we have a valid buffer analysis routine
	if(!buff_analysis)
		return ERR_FAILURE;
	if(!buff_analysis->ascii)
		return ERR_FAILURE;

	// if we have positive match flags ensure we have a setting as to
	// how to match them.
	if(match_flags)
	if(!match_flags_how)
		return ERR_FAILURE;

	// if we have negative match flags, ensure we have a setting
	// as to how to match them as well
	if(negative_match_flags)
	if(!negative_match_flags_how)
		return ERR_FAILURE;

	// create iterator for use as index in various loops below
	size_t n = 0;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Mark Restricted Sets if Necessary %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// the character map we'll be using must be created here (it is destroyed
	// before exit)
	P_BP_ASCII_CHARACTER_MAP char_map = BP_CreateASCIICharacterMap();
	if(!char_map)
		return ERR_FAILURE;

	// check to see if the user has indicated that they wish to check for
	// literal restricted characters.  This is done so that we don't have to
	// iterate through the string of restricted characters at each iteration, and
	// instead just lookup restricted characters within the array as additive
	// index operations (faster).
	if(restricted_chars && restricted_chars_n)
	{

		// mark restricted characters
		for(n = 0; n < restricted_chars_n; n++)
		{
			char_map->ascii_chart[restricted_chars[n]].is_restricted = BP_TRUE;
		}

	}


	// Compare flags (since the BP_CompareASCIICharFlags routine can set multiple
	// bits within flags_t, we have to ensure that thing are checked only for the
	// user provided bits.
	BP_FLAGS_T cmp_flags_pos = 0;
	BP_FLAGS_T cmp_flags_neg = 0;

	// how this works is that we walk each character in the ascii character
	// map/chart and mark flags that do not match those flags found within the
	// users provided match set.
	for(n = 0; n <= 0xff; n++)
	{

		// reset compare flags
		cmp_flags_pos = BP_CompareASCIICharFlags(match_flags, &char_map->ascii_chart[n].flags);
		cmp_flags_neg = BP_CompareASCIICharFlags(negative_match_flags, &char_map->ascii_chart[n].flags);

		if(BP_FLAG_IS_SET(cmp_flags_pos, match_flags_how) && BP_FLAG_IS_SET(cmp_flags_neg, negative_match_flags_how))
		{
			// If flags are set ok, the character is unrestricted.
			//
		}
		else
		{

			// if the flags aren't set ok, restrict this f-er
			char_map->ascii_chart[n].is_restricted = BP_TRUE;
			// printf("\n Marking: %s", char_map->ascii_chart[n].symbol_description);

		}

	}




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% String Extraction Flags %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// these flags assist in the main extract loop by marking whether or
	// not certain conditions have been met.
	BP_BOOL line_break_hit      = BP_FALSE;
	BP_BOOL restricted_char_hit = BP_FALSE;
	BP_BOOL end_of_buffer_hit   = BP_FALSE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Now Walk Individual Characters and Match Appropriately %%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// current character being analyzed
	unsigned char current_char = 0x00;

	// current start read position is the buffers first character
	unsigned char * current_read_start_position = buff_analysis->buffer;

	// this is the current read position
	size_t current_read_length = 0;


	// iterate through string
	for(n = 0; n < buff_analysis->buffer_n; n++)
	{

		// set current character from index
		current_char = buff_analysis->buffer[n];

		// check for bad read position first, always skip restricted characters
		// if the current read length is 0 bytes.
		if(!current_read_length)
		if(char_map->ascii_chart[current_char].is_restricted == BP_TRUE)
			continue;


		// The remainder of the buffer cannot be less than the length
		// of the user provided line break sequence.
		if(line_break_sequence && line_break_sequence_n)
		{

			// ensure that we have enough space left for a successful compare (prevents
			// out of bounds compares)
			if(( buff_analysis->buffer_n - n) < line_break_sequence_n)
				break;

			// compare the line break sequence
			if
			(
				memcmp(
						(void *)&buff_analysis->buffer[n],
						(void *) line_break_sequence,
						line_break_sequence_n
				) == 0
			)
			{
				line_break_hit = BP_TRUE;
			}
		}

		// if the character is restricted, mark it as such
		if(char_map->ascii_chart[current_char].is_restricted)
		{
				restricted_char_hit = BP_TRUE;
		}
		else
		{
				// increment the read length and continue the loop
				current_read_length++;
		}


		// store the string if the line break was hit
		if(line_break_hit == BP_TRUE || restricted_char_hit == BP_TRUE)
		{

			// if the string is of some particular defined length, add it to the buffer
			// analysis (works on reference, not duplication, to minimize space)
			if(current_read_length >= minimum_string_length)
			{

				// set the read start position
				current_read_start_position = &buff_analysis->buffer[n-current_read_length];

				// attempt to add string to analysis
				BP_AddASCIIStringToBufferAnalysis
				(
					buff_analysis,
					n-current_read_length,                        /* n is the current read position as iterated */
					current_read_length,
					current_read_start_position,
					match_flags,
					match_flags_how,
					negative_match_flags,
					negative_match_flags_how
				);


				// now reset the current read length and continue the loop
				current_read_length = 0;
				line_break_hit      = BP_FALSE;
				restricted_char_hit = BP_FALSE;
				continue;

			}
			else
			{

				current_read_length = 0;
				current_read_start_position = &buff_analysis->buffer[n];
				line_break_hit      = BP_FALSE;
				restricted_char_hit = BP_FALSE;
				continue;

			}

		}



	}

	// destroy the character map before exit
	if(char_map)
		BP_DestroyASCIICharacterMap(char_map);

	// return indicating success
	return ERR_SUCCESS;

}


// adds a string to a ascii portion of buffer analyis.  This is a utility routine used by BP_ExtractASCIIStringsFromAnalyzedBuffer
// and probably shouldn't be used directly, although it can be, depending on the current predicament.  The strings added here
// are automatically freed() when BP_DestroyBufferAnalysis is called.
BP_ERROR_T BP_AddASCIIStringToBufferAnalysis
(
		P_BP_BUFFER_ANALYSIS                  buff_analysis,
		size_t                                position_in_analyzed_buffer,
		size_t                                string_len,
		unsigned char *                       address_of_string_in_buffer,
		P_BP_ASCII_CHAR_FLAGS                 match_flags,
		BP_ASCII_CHARACTER_SEARCH_FLAG_VALUES match_flags_how,
		P_BP_ASCII_CHAR_FLAGS                 negative_match_flags,
		BP_ASCII_CHARACTER_SEARCH_FLAG_VALUES negative_match_flags_how
)
{

	// ensure we've created an analysis
	if(!buff_analysis)
		return ERR_FAILURE;

	// ensure we have a tail queue allocator
	if(!buff_analysis->tq)
		return ERR_FAILURE;

	// ensure analysis structure has an ascii substructure4
	if(!buff_analysis->ascii)
		return ERR_FAILURE;

	// ensure we have a buffer
	if(!buff_analysis->buffer)
		return ERR_FAILURE;

	// ensure we have a length
	if(!buff_analysis->buffer_n)
		return ERR_FAILURE;

	// attempt to allocate data
	buff_analysis->ascii->strings = (P_BP_BUFFER_ANALYSIS_ASCII_STRINGS) tq_realloc
	(
			buff_analysis->tq,
			buff_analysis->ascii->strings,
			sizeof(BP_BUFFER_ANALYSIS_ASCII_STRINGS) * (buff_analysis->ascii->string_n+1),
			BPLN
	);

	// return failure if we could not allocate memory for strings
	if(!buff_analysis->ascii->strings)
		return ERR_FAILURE;

	// increase the string count here
	buff_analysis->ascii->string_n++;

	// create a reference for readability purposes
	P_BP_BUFFER_ANALYSIS_ASCII_STRINGS new_string = &buff_analysis->ascii->strings[buff_analysis->ascii->string_n-1];


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %% Set Members Here %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the position in the analyzed buffer
	new_string->position_in_analyzed_buffer = position_in_analyzed_buffer;

	// set the string length
	new_string->string_length = string_len;

	// set the address of the string in the buffer
	new_string->address_of_string_in_buffer = address_of_string_in_buffer;

	// set match flags
	if(match_flags)
	{

		// set match flags enabled here
		new_string->match_flags_enabled = BP_TRUE;

		// copy in the positive match flags
		memcpy(&new_string->match_flags, match_flags, sizeof(BP_ASCII_CHAR_FLAGS));

		// set the match how
		new_string->match_flags_how = match_flags_how;

	}


	// check to see if we were provided with negative match flags
	if(negative_match_flags)
	{

		// set match flags enabled here
		new_string->match_flags_enabled = BP_TRUE;

		// copy in negative flags
		memcpy(&new_string->negative_match_flags, negative_match_flags, sizeof(BP_ASCII_CHAR_FLAGS));

		// set the negative "match how"
		new_string->negative_match_flags_how = negative_match_flags_how;
	}

	// return indicating success
	return ERR_SUCCESS;


}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Secondary Analysis Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// The secondary analysis routines provided here are used for the purpose of determining
// certain string characteristics, and are computationally expensive.  They use regular
// expressions to determine data types found.  This secondary analysis should not be used
// unless you know that the buffer sizes will be reasonable, or you could find yourself
// within an exhaustive memory condition.

// run secondary analysis on extracted strings
BP_ERROR_T BP_RunSecondaryStringAnalysisOnExtractedStrings(P_BP_BUFFER_ANALYSIS buff_analysis)
{

	// check to ensure we have an analysis structure
	if(!buff_analysis)
		return ERR_FAILURE;

	// ensure we have an ascii analysis structure
	if(!buff_analysis->ascii)
		return ERR_FAILURE;

	// ensure we have strings in the ascii analysis structure
	if(!buff_analysis->ascii->string_n)
		return ERR_FAILURE;

	// walk each individual string and mark applicable strings
	size_t n = 0;

	// reference to the current string
	P_BP_BUFFER_ANALYSIS_ASCII_STRINGS curr_string = NULL;

	// string used for checking regex constants, allocated dynamically
	// and automatically freed on loop.
	char * string_iter = NULL;

	// walk every entry in the strings array, run analysis on each
	for(; n < buff_analysis->ascii->string_n; n++)
	{

		// Set the current string reference for easier
		// readability.
		curr_string = &buff_analysis->ascii->strings[n];

		// check if the buffer is quoted
		curr_string->string_quote_info = BP_CheckIfBufferIsQuoted
		(
			(char *) curr_string->address_of_string_in_buffer,
			curr_string->string_length
		);

		// attempt to allocate space here
		string_iter = (char *) bpcalloc(curr_string->string_length+10, 1);
		if(!string_iter)
			break;

		// copy in the string (extra space ensures buffer is null terminated)
		memcpy(string_iter, curr_string->address_of_string_in_buffer, curr_string->string_length);

		curr_string->string_flags.is_unix_file_path               = (BP_BOOL) BP_StringIsPathURL  (string_iter);
		curr_string->string_flags.is_url                          = (BP_BOOL) BP_StringIsURL      (string_iter, BP_FALSE);
		curr_string->string_flags.is_strict_url_matching_with_tld = (BP_BOOL) BP_StringIsURL(string_iter, BP_TRUE);
		curr_string->string_flags.is_host_url                     = (BP_BOOL) BP_StringIsHostURL  (string_iter);
		curr_string->string_flags.is_base_url                     = (BP_BOOL) BP_StringIsBaseURL  (string_iter);
		curr_string->string_flags.is_script_url                   = (BP_BOOL) BP_StringIsScriptURL(string_iter);
		curr_string->string_flags.is_path_url                     = (BP_BOOL) BP_StringIsPathURL  (string_iter);


		// check for .so
		if(curr_string->string_length >= 3)
		if(memcmp(&curr_string->address_of_string_in_buffer[curr_string->string_length-4], ".so", 3) == 0)
			curr_string->string_flags.is_dynamic_so = BP_TRUE;


		// now check if it's a likely file or not
		if(BP_FileAtPathExists(string_iter) == BP_TRUE)
		{

			// now check if the file exists
			curr_string->string_flags.exists_as_file_on_filesystem = (BP_BOOL) BP_TRUE;

			// if it exists, check if its read/writeable
			if(curr_string->string_flags.exists_as_file_on_filesystem)
			{
				curr_string->string_flags.exists_as_file_on_filesystem_and_is_readable = (BP_BOOL) BP_FileAtPathExistsAndIsReadable(string_iter);
				curr_string->string_flags.exists_as_file_on_filesystem_and_is_writable = (BP_BOOL) BP_FileAtPathExistsAndIsWritable(string_iter);
			}

		}


		// destroy the iter on loop
		bpfree(string_iter);
		string_iter = NULL;

	}

	return ERR_SUCCESS;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Flag Matching Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Returns a BP_FLAGS_T value set with any combination of BP_ASCII_CHARACTER_SEARCH_FLAG_VALUES enumerated
// values.
BP_FLAGS_T BP_CompareASCIIStringFlags(P_BP_BUFFER_ANALYSIS_ASCII_STRING_FLAGS flags_compare_this, P_BP_BUFFER_ANALYSIS_ASCII_STRING_FLAGS flags_compare_with_this)
{

	// ensure we have two sets of flags to compare
	if(!flags_compare_this || !flags_compare_with_this)
		return BP_ASCII_STRING_SEARCH_NONE;

	// count for matches (used to determine if we have an inbound search perfect partial match)
	size_t compare_flag_count = BP_CountNumberASCIIStringFlagsSet(flags_compare_this);

	// match flag count
	size_t match_flag_count = 0;

	// match negative count
	size_t match_flag_negative_count = 0;


	// declare and initialize flags (this is a bitfield flag-set which
	// is the eventual return value for this routine)
	BP_FLAGS_T match_flags = 0;


	// set based on pure positive/negative match
	if(memcmp(flags_compare_this, flags_compare_with_this, sizeof(BP_BUFFER_ANALYSIS_ASCII_STRING_FLAGS)) == 0)
	{

		// set indicator showing we have a perfect match
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_POSITIVE_MATCH);

		// since we have a perfect match, we know all flag values were set ok
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_ALL_SEARCH_FLAG_CONDITIONS_MET);

		// return the flags
		return match_flags;

	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_NEGATIVE_MATCH);
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% is matched as unix file path %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(flags_compare_this->is_unix_file_path == 1)
	if(flags_compare_this->is_unix_file_path == flags_compare_with_this->is_unix_file_path)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% is matched as generic URL %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(flags_compare_this->is_url == 1)
	if(flags_compare_this->is_url == flags_compare_with_this->is_url)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% is matched is_host_url %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(flags_compare_this->is_host_url == 1)
	if(flags_compare_this->is_host_url == flags_compare_with_this->is_host_url)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% is matched is_base_url %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(flags_compare_this->is_base_url == 1)
	if(flags_compare_this->is_base_url == flags_compare_with_this->is_base_url)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% is matched is_script_url %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(flags_compare_this->is_script_url == 1)
	if(flags_compare_this->is_script_url == flags_compare_with_this->is_script_url)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% is matched is_path_url %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(flags_compare_this->is_path_url == 1)
	if(flags_compare_this->is_path_url == flags_compare_with_this->is_path_url)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% is matched is_path_url %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(flags_compare_this->is_dynamic_so == 1)
	if(flags_compare_this->is_dynamic_so == flags_compare_with_this->is_dynamic_so)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% is matched exists_as_file_on_filesystem %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(flags_compare_this->exists_as_file_on_filesystem == 1)
	if(flags_compare_this->exists_as_file_on_filesystem == flags_compare_with_this->exists_as_file_on_filesystem)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% is matched exists_as_file_on_filesystem_and_is_readable %%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(flags_compare_this->exists_as_file_on_filesystem_and_is_readable == 1)
	if(flags_compare_this->exists_as_file_on_filesystem_and_is_readable == flags_compare_with_this->exists_as_file_on_filesystem_and_is_readable)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% is matched exists_as_file_on_filesystem_and_is_writable %%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(flags_compare_this->exists_as_file_on_filesystem_and_is_writable == 1)
	if(flags_compare_this->exists_as_file_on_filesystem_and_is_writable == flags_compare_with_this->exists_as_file_on_filesystem_and_is_writable)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}



	// if we have matched the correct number of flags, we can safely consider
	// all conditions met.
	if(compare_flag_count == match_flag_count)
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_ALL_SEARCH_FLAG_CONDITIONS_MET);

	if(compare_flag_count == match_flag_negative_count)
		BP_SET_FLAG(match_flags, BP_ASCII_STRING_SEARCH_ALL_SEARCH_FLAG_CONDITIONS_MET_NEGATIVELY);



	// return the matched flags
	return match_flags;

}


// Return the number of flags which are set (BP_TRUE) within a P_BP_BUFFER_ANALYSIS_ASCII_STRING_FLAGS structure.  Primarily used for
// matching flags.
size_t BP_CountNumberASCIIStringFlagsSet(P_BP_BUFFER_ANALYSIS_ASCII_STRING_FLAGS flags)
{

	// ensure we have a valid pointer
	if(!flags)
		return ERR_FAILURE;

	// flag counts
	size_t flag_count = 0;

	// set flags as appropriate
	if(flags->is_unix_file_path)
		flag_count++;
	if(flags->is_url)
		flag_count++;
	if(flags->is_host_url)
		flag_count++;
	if(flags->is_base_url)
		flag_count++;
	if(flags->is_path_url)
		flag_count++;
	if(flags->is_dynamic_so)
		flag_count++;
	if(flags->exists_as_file_on_filesystem)
		flag_count++;
	if(flags->exists_as_file_on_filesystem_and_is_readable)
		flag_count++;
	if(flags->exists_as_file_on_filesystem_and_is_writable)
		flag_count++;

	// return the total flag count
	return flag_count;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Mark Restricted Strings %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Marks the strings.marked_as_restricted marker with the value of mark_restricted_status based
// on the provided flags, and the match how setting.
BP_ERROR_T BP_MarkRestrictedOnExtractedStrings
(
	P_BP_BUFFER_ANALYSIS                    buff_analysis,
	BP_BOOL                                 mark_restricted_status,
	P_BP_BUFFER_ANALYSIS_ASCII_STRING_FLAGS flags_match,
	BP_ASCII_STRING_SEARCH_FLAG_VALUES      flags_match_how
)
{

	// ensure we have an analysis buffer and an extracted strings count
	if(!buff_analysis)
		return ERR_FAILURE;
	if(!buff_analysis->ascii)
		return ERR_FAILURE;
	if(!buff_analysis->ascii->strings || !buff_analysis->ascii->string_n)
		return ERR_FAILURE;

	// match flags set, set dynamically below
	BP_FLAGS_T match_flags = 0;

	size_t n = 0;
	for(; n < buff_analysis->ascii->string_n; n++)
	{

		// if we're marking all, just go ahead and do it
		if(!flags_match)
		{
			buff_analysis->ascii->strings[n].marked_as_restricted = mark_restricted_status;
			continue;
		}

		// compute the match flags
		match_flags = BP_CompareASCIIStringFlags(flags_match, &buff_analysis->ascii->strings[n].string_flags);

		// BP_DisplayASCIIStringMatchFlags(match_flags);

		// mark restricted here
		if(BP_FLAG_IS_SET(match_flags, flags_match_how))
		{
			buff_analysis->ascii->strings[n].marked_as_restricted = mark_restricted_status;
		}

		// reset flags
		match_flags = 0;

	}

	// return indicating success
	return ERR_SUCCESS;

}

// displays what flags are set in the match flags
BP_ERROR_T BP_DisplayASCIIStringMatchFlags(BP_FLAGS_T match_flags)
{

	// ensure we have at least one flag set
	if(!match_flags)
	{
		printf("\n\t [+] No flags set.");
		return ERR_FAILURE;
	}


	printf("\n\t [+] ASCII String Match Flags:");
	printf("\n\t\t negative match:                %s", BP_FLAG_IS_SET(match_flags, BP_ASCII_STRING_SEARCH_NEGATIVE_MATCH) ? "yes" : "no");
	printf("\n\t\t positive match:                %s", BP_FLAG_IS_SET(match_flags, BP_ASCII_STRING_SEARCH_POSITIVE_MATCH) ? "yes" : "no");
	printf("\n\t\t partial positive match:        %s", BP_FLAG_IS_SET(match_flags, BP_ASCII_STRING_SEARCH_PARTIAL_POSITIVE_MATCH) ? "yes" : "no");
	printf("\n\t\t partial negative match:        %s", BP_FLAG_IS_SET(match_flags, BP_ASCII_STRING_SEARCH_PARTIAL_NEGATIVE_MATCH) ? "yes" : "no");
	printf("\n\t\t all search conditions met:     %s", BP_FLAG_IS_SET(match_flags, BP_ASCII_STRING_SEARCH_ALL_SEARCH_FLAG_CONDITIONS_MET) ? "yes" : "no");
	printf("\n\t\t all search conditions met neg: %s", BP_FLAG_IS_SET(match_flags, BP_ASCII_STRING_SEARCH_ALL_SEARCH_FLAG_CONDITIONS_MET_NEGATIVELY) ? "yes" : "no");

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// display the buffer analysis ascii portion.
BP_ERROR_T BP_DisplayBufferAnalysisASCII(P_BP_BUFFER_ANALYSIS buff_analysis)
{

	// ensure we have an analysis pointer
	if(!buff_analysis)
		return ERR_FAILURE;

	// ensure we have an ascii analysis pointer
	if(!buff_analysis->ascii)
		return ERR_FAILURE;

	// character map
	printf("\n\t [-] ASCII Analysis Structure (%p)", buff_analysis->ascii);
	printf("\n");
	printf("\n\t\t char_map: %p", buff_analysis->ascii->char_map);
	printf("\n");

	// create a reference
	P_BP_BUFFER_ANALYSIS_ASCII_FLAG_COUNTS flag_counts = &buff_analysis->ascii->ascii_char_flag_counts;

	// create and zero out an empty flags structure (used for
	// comparisson)
	BP_BUFFER_ANALYSIS_ASCII_FLAG_COUNTS empty_flags;
	BP_ZERO_STACK(empty_flags);

	// if we have flags set, display them
	if(memcmp(&empty_flags, flag_counts, sizeof(BP_BUFFER_ANALYSIS_ASCII_FLAG_COUNTS)) != 0)
	{

		// display flag count
		printf("\n\t\t [-] Flag Counts: ");


		if(flag_counts->control_n)
		{
			printf("\n\t\t    control_n: ................%u", flag_counts->control_n);
		}

		if(flag_counts->alphalower_n)
		{
			printf("\n\t\t    alphalower_n: .............%u", flag_counts->alphalower_n);
		}

		if(flag_counts->alphacaps_n)
		{
			printf("\n\t\t    alphacaps_n: ..............%u", flag_counts->alphacaps_n);
		}

		if(flag_counts->number_n)
		{
			printf("\n\t\t    number_n: .................%u", flag_counts->number_n);
		}

		if(flag_counts->printable_n)
		{
			printf("\n\t\t    printable_n: ..............%u", flag_counts->printable_n);
		}

		if(flag_counts->nonprintable_n)
		{
			printf("\n\t\t    nonprintable_n: ...........%u", flag_counts->nonprintable_n);
		}

		if(flag_counts->extended_n)
		{
			printf("\n\t\t    extended_n: ...............%u", flag_counts->extended_n);
		}

		if(flag_counts->math_n)
		{
			printf("\n\t\t    math_n: ...................%u", flag_counts->math_n);
		}

		if(flag_counts->superscript_n)
		{
			printf("\n\t\t    superscript_n: ............%u", flag_counts->superscript_n);
		}

		if(flag_counts->currency_n)
		{
			printf("\n\t\t    currency_n: ...............%u", flag_counts->currency_n);
		}

		if(flag_counts->fraction_n)
		{
			printf("\n\t\t    fraction_n: ...............%u", flag_counts->fraction_n);
		}

		if(flag_counts->acute_n)
		{
			printf("\n\t\t    acute_n: ..................%u", flag_counts->acute_n);
		}

		if(flag_counts->grave_n)
		{
			printf("\n\t\t    grave_n: ..................%u", flag_counts->grave_n);
		}

		if(flag_counts->diaeresis_n)
		{
			printf("\n\t\t    diaeresis_n: ..............%u", flag_counts->diaeresis_n);
		}

		if(flag_counts->slash_n)
		{
			printf("\n\t\t    slash_n: ..................%u", flag_counts->slash_n);
		}

		if(flag_counts->circumflex_n)
		{
			printf("\n\t\t    circumflex_n: .............%u", flag_counts->circumflex_n);
		}

		if(flag_counts->caron_n)
		{
			printf("\n\t\t    caron_n: ..................%u", flag_counts->caron_n);
		}

		if(flag_counts->macron_n)
		{
			printf("\n\t\t    macron_n: .................%u", flag_counts->macron_n);
		}

		if(flag_counts->punctuation_n)
		{
			printf("\n\t\t    punctuation_n: ............%u", flag_counts->punctuation_n);
		}

		if(flag_counts->symbol_n)
		{
			printf("\n\t\t    symbol_n: .................%u", flag_counts->symbol_n);
		}

		if(flag_counts->latin_n)
		{
			printf("\n\t\t    latin_n: ..................%u", flag_counts->latin_n);
		}

		if(flag_counts->latin_nonstandard_caps_n)
		{
			printf("\n\t\t    latin_nonstandard_caps_n:  %u", flag_counts->latin_nonstandard_caps_n);
		}

		if(flag_counts->latin_nonstandard_lower_n)
		{
			printf("\n\t\t    latin_nonstandard_lower_n: %u", flag_counts->latin_nonstandard_lower_n);
		}

		if(flag_counts->quotation_n)
		{
			printf("\n\t\t    quotation_n: ..............%u", flag_counts->quotation_n);
		}

		if(flag_counts->whitespace_n)
		{
			printf("\n\t\t    whitespace_n: .............%u", flag_counts->whitespace_n);
		}

		if(flag_counts->undefined_n)
		{
			printf("\n\t\t    undefined_n: ..............%u", flag_counts->undefined_n);
		}

		// add trailing newline
		printf("\n");

	}


	// display character count header
	printf("\n\t\t [-] Char Counts:");

	// iterate through individual characters
	size_t n = 0;
	for(; n <= 0xff; n++)
	{

		// only display characters with existing counts
		if(!buff_analysis->ascii->char_counts[n])
			continue;

		// display the character
		printf("\n\t\t     char: 0x%.02x - %u - %s", n, buff_analysis->ascii->char_counts[n], buff_analysis->ascii->char_map->ascii_chart[n].symbol_description);

	}
	printf("\n");

	// now iterate through strings
	n = 0;
	for(; n < buff_analysis->ascii->string_n; n++)
	{

		// don't display restricted strings
		if(buff_analysis->ascii->strings[n].marked_as_restricted == BP_TRUE)
			continue;

		printf("\n\t [x] String Index in String Stack: %zu", n);
		BP_DisplayBufferAnalysisASCIIString(&buff_analysis->ascii->strings[n], BP_TRUE);

	}


	// return indicating success
	return ERR_SUCCESS;

}

// display an ascii string structure (found within buff analysis)
BP_ERROR_T BP_DisplayBufferAnalysisASCIIString(P_BP_BUFFER_ANALYSIS_ASCII_STRINGS buff_string, BP_BOOL display_string_data)
{

	if(!buff_string)
		return ERR_FAILURE;

	// return immediately if it's restricted
	if(buff_string->marked_as_restricted == BP_TRUE)
		return ERR_FAILURE;

	printf("\n\t [+] Analyzed String (%p)", buff_string);
	printf("\n\t     position in analyzed buffer:  %zu", buff_string->position_in_analyzed_buffer);
	printf("\n\t     string length:                %zu", buff_string->string_length);
	printf("\n\t     address of string in buffer:  %p",  buff_string->address_of_string_in_buffer);
	printf("\n\t     match_flags_enabled:          %s",  buff_string->match_flags_enabled ? "yes" : "no");

	// printf("\n\t     match_flags:                  %p",  buff_string->match_flags);
	// printf("\n\t     match_flags_how:              %zu", buff_string->match_flags_how);

	if(buff_string->restricted_characters)
	{
		printf("\n\t     restricted_characters:        %p",  buff_string->restricted_characters);
		printf("\n\t     restricted_characters_n:      %u",  buff_string->restricted_characters_n);
	}

	if(buff_string->string_terminator)
	{
		printf("\n\t     string_terminator:            %p",  buff_string->string_terminator);
		printf("\n\t     string_terminator_length:     %u",  buff_string->string_terminator_length);
	}

	// create empty flags for compare
	BP_BUFFER_ANALYSIS_ASCII_STRING_FLAGS empty_flags;
	BP_ZERO_STACK(empty_flags);

	// if we have flags, go ahead and display flags
	if(memcmp(&empty_flags, &buff_string->string_flags, sizeof(empty_flags)) != 0)
	{

		if(buff_string->string_flags.is_unix_file_path)
			printf("\n\t     flags.is_unix_file_path");

		if(buff_string->string_flags.is_url)
			printf("\n\t     flags.is_url");

		if(buff_string->string_flags.is_host_url)
			printf("\n\t     flags.is_host_url");

		if(buff_string->string_flags.is_base_url)
			printf("\n\t     flags.is_base_url");

		if(buff_string->string_flags.is_script_url)
			printf("\n\t     flags.is_script_url");

		if(buff_string->string_flags.is_path_url)
			printf("\n\t     flags.is_path_url");

		if(buff_string->string_flags.is_dynamic_so)
			printf("\n\t     flags.is_dynamic_so");

		if(buff_string->string_flags.exists_as_file_on_filesystem)
			printf("\n\t     flags.exists_as_file_on_filesystem");

		if(buff_string->string_flags.exists_as_file_on_filesystem_and_is_readable)
			printf("\n\t     flags.exists_as_file_on_filesystem_and_is_readable");

		if(buff_string->string_flags.exists_as_file_on_filesystem_and_is_writable)
			printf("\n\t     flags.exists_as_file_on_filesystem_and_is_writable");

	}

	// display string data if necessary
	if(display_string_data == BP_TRUE)
	{

		// allocate temporary space
		char *display_string = (char *) bpcalloc(buff_string->string_length+5, 1);

		// copy in data
		memcpy(display_string, buff_string->address_of_string_in_buffer, buff_string->string_length);

		// actually display string]
		printf("\n\t     STRING DATA: %s", display_string);

		// destroy temporary space
		bpfree(display_string);

	}

	// display trailing newline
	printf("\n");

	// return indicating success
	return ERR_SUCCESS;

}

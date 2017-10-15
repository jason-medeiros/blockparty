/*
 * BP-StringUtilities.cc
 *
 *  Created on: Mar 16, 2010
 *      Author: Jason Medeiros
 */




// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! NOTICE REGARDING COMPLEX STRING FORMATTING / GENERATION !!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// IMPORTANT:  This file contains a large number of utilities for working
//             with raw strings.  HOWEVER, if you're trying to generate
//             data suitable for terminal output, this is not where you
//             want to be looking.
//
//             Instead, look at the code within utilities/BP-StringFormattingSessions.(h/cc)
//             for a more useful set of routines.  A unit test for working
//             with these sessions is available in:
//
//              applications/examples/string_formatting_sessions_ut.cc
//
//             The unit test code contains examples for working with C and C++
//             both.
//
//
//             Session Features:
//             ------------------------------------------------------
//                   - terminal colors (per character)
//                   - rows/row groups
//                   - columns/column groups
//                   - easy memory allocation/cleanup (tailqueue allocation)
//                   - word wrapping
//                   - definitive column wrapping
//                   - terminal width auto-detection for wrapping
//                   - element searching (rows/columns/groups/strings)
//                   - and more
//
//             Additionally, a C++ class is available for working
//             with string formatting sessions directly.  This class
//             is available in:
//
//                   include/blockclass/subclasses/BP-StringFormattingSession.(cc/h)
//
//
//
//         Don't be stupid, use it.
//


// ###################################################################
// ## POSIX (gnulibc) REGEXP Routines ################################
// ###################################################################


// include main functions
#include "../../include/BP-Main.h"


// Attempt to match regular expression against haystack of haystack_size bytes using
// POSIX regular expression in regular_expression.  The regexp_flags variable can be set
// to alter the behavior of the evaluation.  Possible flags shown below.

//	 These are the bit flags that you can pass to the function below:
//		 REG_EXTENDED
//   		 Treat the pattern as an extended regular expression, rather than as a basic regular expression.
//		 REG_ICASE
//    		Ignore case when matching letters.
//		 REG_NOSUB
//    		Don't bother storing the contents of the matches-ptr array.
// 		 REG_NEWLINE
//    		Treat a newline in string as dividing string into multiple lines, so that $ can match before the newline and ^ can match after. Also, don't permit . to match a newline, and don't permit [^...] to match a newline.

BP_ERROR_T BP_MatchRegularExpression
(
		_vin    char *haystack,
		_vin    size_t haystack_size,
		_vin    char *regular_expression,
		_vin    int regexp_flags,
		_vin    size_t max_match_count,
		_vinout P_BP_REGEX_MATCH_INFO match_info
)
{

	// check parameters
	if(!haystack || !haystack_size || !regular_expression)
		return ERR_FAILURE;


	// create space for compiled regex
	regex_t compiled_regex = {0};

	// compile the regex, return null on failure
	if(regcomp (&compiled_regex, regular_expression, regexp_flags))
		return ERR_FAILURE;

	// create space for match (always add space for final element)
	regmatch_t tmp_match[max_match_count+1];

	// zero out memory
	memset(&tmp_match, 0x00, sizeof(tmp_match));


	// run regular expression against string, return null if no match
	if( !regexec (&compiled_regex, haystack, max_match_count, (regmatch_t *) &tmp_match, regexp_flags) )
	{

		// create null test (matches end with -1 in params)
		regmatch_t end_test;
		memset(&end_test, 0x00, sizeof(end_test));

		end_test.rm_eo = -1;
		end_test.rm_so = -1;


		// match array indexer
		size_t i = 0;

		// create match info if necessary
		if(match_info)
			match_info->matches = NULL;

		if(match_info)
		for(
				i = 0;
				memcmp(&tmp_match[i], &end_test, sizeof(regmatch_t)) != 0 && i < max_match_count;
				i++
		)
		{

			// bprealloc space for matches on loop
			match_info->matches = (regmatch_t *) bprealloc(match_info->matches, sizeof(regmatch_t) * (i+1) );

			// copy structure into the newly allocated space
			memcpy(&match_info->matches[i], &tmp_match[i], sizeof(regmatch_t));

			// increment the number of structures (starting at 1 not 0)
			match_info->num_matched = i+1;

		}


		// bpfree regular expression
		regfree(&compiled_regex);

		// return indicating success
		return ERR_SUCCESS;

	}

	// bpfree regular expression
	regfree(&compiled_regex);

	// exit indicating failure
	return ERR_FAILURE;

}

// displays match info for string
BP_ERROR_T BP_DisplayRegexMatchInfo(P_BP_REGEX_MATCH_INFO match_info)
{

	if(!match_info)
		return ERR_FAILURE;

	if(match_info->num_matched == 0)
		return ERR_FAILURE;

	// walk arrays and display match content
	printf("\n [+] Regular Expression Match for %p: Indexes relative to start of string.", match_info);

	size_t i = 0;
	for(i = 0; i < match_info->num_matched; i++)
	{
		printf("\n\t -> Match #%u:", i);
		printf("\n\t      Start Byte Index: %u", match_info->matches[i].rm_so);
		printf("\n\t      End Byte Index:   %u", match_info->matches[i].rm_eo);

	}

	// return indicating success
	return ERR_SUCCESS;
}

// Regexp find and replace all occurances of replace_this in in_buff.  Returns new pointer with replacements made.
// This function does not modify in_buff directly.  Returns null if no replacements are made.
char * BP_RegFindReplace(char *in_buff, char *replace_this, char *replace_with, size_t replace_size, size_t max_replacements, size_t regexp_flags)
{

	// temporary match info structure
    P_BP_REGEX_MATCH_INFO match_info = (P_BP_REGEX_MATCH_INFO) bpcalloc(sizeof(BP_REGEX_MATCH_INFO), 1);

    // attempt to make a match
    BP_ERROR_T match = BP_MatchRegularExpression(in_buff, bpstrlen(in_buff), replace_this, regexp_flags, max_replacements, match_info );

    // destroy match info immediately
    BP_DestroyRegexpMatchInfo(match_info);
	bpfree(match_info);
	match_info = NULL;

    // no match made
    if(match == ERR_FAILURE)
    {
    	return NULL;
    }


    // buffer which will contain replacments, returned to user.
    char *out_buff = NULL;

    // set temp pointer to current position
    char *tmp = in_buff;

    // for monitoring string length
    size_t string_length = 0;

    // replacement count
    size_t replacement_count = 0;

    // used for storage of the remainder of the string
    char * str_remainder = NULL;

    // recreate match info
    match_info = (P_BP_REGEX_MATCH_INFO) bpcalloc(sizeof(BP_REGEX_MATCH_INFO), 1);

    // as long as we match regular expressions, make replacements
    while(BP_MatchRegularExpression(tmp, bpstrlen(tmp), replace_this, regexp_flags, max_replacements, match_info ) == ERR_SUCCESS)
    {

    	// increase string length by start offset + replacement size + 1
    	string_length += match_info->matches[0].rm_so + replace_size;

    	// bpreallocate buffer on loop
    	out_buff = (char *) bprealloc
    				(
						out_buff,
						string_length + 100
					);


    	// this is the write start offset used for writing into the existing outbuff
    	size_t start_offs = string_length - (match_info->matches[0].rm_so + replace_size);

    	// zero out new memory (start of new data)
    	memset(&out_buff[start_offs], 0x00, (match_info->matches[0].rm_so + replace_size + 100));

    	// reproduce string in buffer
    	strncat(out_buff, tmp, match_info->matches[0].rm_so);

    	// only replace if NON null, if its null omit the replacement but still record the deletion
    	if(replace_with)
    		strncat(out_buff, replace_with, replace_size );

    	// replacement count
    	replacement_count++;

    	/*
    	printf("\n Write Start Offset: %u", start_offs);
    	printf("\n Got outbuff after alloc: %s - %u", out_buff, replace_size);
    	printf("\n Replacing with: %s", replace_with);
    	printf("\n-");
		*/

    	// if oom, return null
    	if(!out_buff)
    	{
    		BP_DestroyRegexpMatchInfo(match_info);
    		bpfree(match_info);
			return NULL;
    	}


    	// matches end of string (moves match forward)
    	tmp = &tmp[match_info->matches[0].rm_eo];

    	// set remainder in the case we're at the loop end
    	str_remainder = tmp;

    	// destroy match info on loop
    	BP_DestroyRegexpMatchInfo(match_info);
    	bpfree(match_info);
    	match_info = (P_BP_REGEX_MATCH_INFO) bpcalloc(sizeof(BP_REGEX_MATCH_INFO), 1);

    }

    // copy the remainder of the data into the outbuff
    if(out_buff)
    {

    	// get the string remainder
    	size_t remainder_len = bpstrlen(str_remainder);

    	// bpreallocate output buffer
    	out_buff = (char *) bprealloc(out_buff, bpstrlen(out_buff) + remainder_len + 1);
    	memset(&out_buff[bpstrlen(out_buff)], 0x00, remainder_len+1);

    	// concatenate remainder
    	strncat(out_buff, str_remainder, remainder_len);

    }

    // destroy match information if found
    if(match_info)
    {
    	BP_DestroyRegexpMatchInfo(match_info);
    	bpfree(match_info);
    }

    // return the new pointer
    return out_buff;

}

// Standard find and replace.  Returns a newly allocated string which
// must be bpfreed.
char * BP_FindReplace(char *in_buff, char *replace_this, char *replace_with, size_t replace_size, size_t max_replacements)
{

	// ensure we have valid pointers
	if(!in_buff || !replace_this || !replace_with)
		return NULL;

	// ensure that pointers point to strings
	if(!bpstrlen(in_buff))
		return NULL;
	 if(!bpstrlen(replace_this))
		 return NULL;
	 if(!bpstrlen(replace_with))
		 return NULL;

	// if the string to be replaced in is shorter than match check
	// exit.
	if(bpstrlen(in_buff) < bpstrlen(replace_this))
		return NULL;

	// first calculate the number of matches in the string
	size_t has_matches = 0;

	// set the read index and write index
	size_t read_idx  = 0;
	size_t write_idx = 0;

	// previous read index (used for calculating lengths)
	size_t prev_read_index  = 0;
	size_t prev_write_index = 0;

	// calculate length
	size_t in_len           = bpstrlen(in_buff);
	size_t replace_this_len = bpstrlen(replace_this);

	// character used for byte by byte checking
	unsigned char test_char = 0x00;

	// buffer which will be reallocated and rewritten
	char *write_buff = NULL;

	// walk index items and calculate lengths
	do
	{

		// simple binary check to see if we have encountered the first char of our string
		if((unsigned char) in_buff[read_idx] == (unsigned char) replace_this[0])
		{

			// if we found the first char, compare memory.
			if(memcmp(&in_buff[read_idx], replace_this, replace_this_len) == 0)
			{



				// display the current write/read indexes
				// printf("\n Found Match At (%c - %c) : %s",in_buff[read_idx], replace_this[0],  &in_buff[read_idx]);
				// printf("\n write: %u, read: %u", write_idx, read_idx);

				// move the write index up
				write_idx+=replace_size;


				// printf("\n write after: %u", write_idx);

				// the write_idx variable currently points to the end of the read
				write_buff = (char *) bprealloc(write_buff, write_idx+2);



				// zero out new memory
				memset(&write_buff[write_idx-(replace_size)], 0x00, replace_size+1);


				// copy in data here
				memcpy(&write_buff[prev_write_index], &in_buff[prev_read_index], read_idx-prev_read_index);


				//
				// since the buffer was reallocated, we can copy in data
				// from the base read index (aka prev_read_index) to the
				// write buffers base (aka.  start of new alloc'ed mem)
				//
				memcpy
				(
						&write_buff[write_idx-(replace_size)],
						replace_with,
						replace_size
				);

				// move the read index up
				read_idx+=replace_this_len;

				// set the previous read and write indexes
				prev_read_index  = read_idx;
				prev_write_index = write_idx;

			}
			else
			{

				// read and write indexes
				read_idx++;
				write_idx++;

			}

		}
		else
		{
			// increment read and write indexes
			read_idx++;
			write_idx++;
		}

	} while(read_idx < in_len);

	if(prev_read_index)
	if(bpstrlen(&in_buff[prev_read_index]))
	{

		// calculate size for the remainder
		size_t final_realloc = bpstrlen(&in_buff[prev_read_index]);

		// perform final reallocation
		write_buff = (char *) bprealloc(write_buff, prev_write_index+final_realloc+10);

		// initialize the memory
		memset(&write_buff[prev_write_index], 0x00, final_realloc+1);

		// copy in the remainder of the string
		strncat(&write_buff[prev_write_index-1], &in_buff[prev_read_index], final_realloc);

	}

	// return the generated buffer
	return write_buff;

}


// This will attempt to find and remove patterns in a string.  Binary safe so long as
// provided lengths are given.  This is primarily to allow FindReplace the ability to
// remove strings and replace them with null strings.
char * BP_FindRemoveBin
(
		char * in_buff,
		size_t in_buff_size,
		char * remove_this,
		size_t remove_this_size,
		size_t max_removals
)
{

	// ensure we have valid pointers
	if(!in_buff)
		return NULL;
	if(!in_buff_size)
		return NULL;

	// ensurein_buff_size
	 if(!remove_this)
		 return NULL;
	 if(!remove_this_size)
		 return NULL;

	// if the string to be replaced in is shorter than match check
	// exit.
	if(in_buff_size < remove_this_size)
		return NULL;

	// first calculate the number of matches in the string
	size_t has_matches = 0;

	// set the read index and write index
	size_t read_idx  = 0;
	size_t write_idx = 0;

	// previous read index (used for calculating lengths)
	size_t prev_read_index  = 0;
	size_t prev_write_index = 0;

	// calculate length
	size_t in_len           = bpstrlen(in_buff);
	size_t replace_this_len = remove_this_size;

	// character used for byte by byte checking
	unsigned char test_char = 0x00;

	// buffer which will be reallocated and rewritten
	char *write_buff = NULL;

	// walk index items and calculate lengths
	do
	{

		// simple binary check to see if we have encountered the first char of our string
		if((unsigned char) in_buff[read_idx] == (unsigned char) remove_this[0])
		{

			// if we found the first char, compare memory.
			if(memcmp(&in_buff[read_idx], remove_this, remove_this_size) == 0)
			{



				// display the current write/read indexes
				// printf("\n Found Match At (%c - %c) : %s",in_buff[read_idx], remove_this[0],  &in_buff[read_idx]);
				// printf("\n write: %u, read: %u", write_idx, read_idx);

				// move the write index down
				//write_idx -= remove_this_size;

				// the write_idx variable currently points to the end of the read
				write_buff = (char *) bprealloc(write_buff, write_idx+1);

				// copy in data here
				memcpy(&write_buff[prev_write_index], &in_buff[prev_read_index], read_idx-prev_read_index);

				// printf("\n Writebuff?: %s - %s - %s", write_buff, (char *) &in_buff[prev_read_index], (char *) &in_buff[prev_read_index]);

				// move the read index up
				read_idx += remove_this_size;

				// set the previous read and write indexes
				prev_read_index  = read_idx;
				prev_write_index = write_idx;

			}
			else
			{

				// increment the read position
				read_idx++;
				write_idx++;

			}

		}
		else
		{
			// increment the read and write position
			read_idx++;
			write_idx++;
		}

	} while(read_idx < in_len);

	if(prev_read_index)
	if(bpstrlen(&in_buff[prev_read_index]))
	{

		// calculate size for the remainder
		size_t final_realloc = bpstrlen(&in_buff[prev_read_index]);

		// perform final reallocation
		write_buff = (char *) bprealloc(write_buff, prev_write_index+final_realloc+10);

		// initialize the memory
		memset(&write_buff[prev_write_index], 0x00, final_realloc+1);

		// copy in the remainder of the string
		strncat(&write_buff[prev_write_index-1], &in_buff[prev_read_index], final_realloc);

	}

	// return the generated buffer
	return write_buff;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Very Basic/Fast String Conformity Validator %%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//
// NOTICE: This content validator does not use expressions, it is used for
//         quickly (loop) determining basic content of a string to provide
//         a mechanism to run literal data validation without much hassle.
//

// Enumerator providing easily switchable types for determining string
// contents.
/*
typedef enum BP_STRING_CONFORMITY {
	BP_STRING_CONFORMITY_UNDETERMINED,
	BP_STRING_CONFORMS_AS_NULL_WITH_ZERO_LENGTH,
	BP_STRING_CONFORMS_AS_EMPTY_STRING,
	BP_STRING_CONFORMS_AS_ASCII_ALPHA_ONLY,
	BP_STRING_CONFORMS_AS_ASCII_ALPHANUM_ONLY,
	BP_STRING_CONFORMS_AS_ASCII_ALPHANUM_SPACES_ONLY,
	BP_STRING_CONFORMS_AS_ASCII_WITH_NO_CONTROL_CHARACTERS,
	BP_STRING_CONFORMS_AS_ASCII_WITH_CONTROL_CHARACTERS,
	BP_STRING_CONTAINS_NON_ASCII_BINARY_DATA
};
*/

// takes a string and runs a sequence of checks to determine whether the
// the string conforms to a basic sequence mapping.
BP_STRING_CONFORMITY_FLAGS_T BP_StringConformsAs
(
	char *string,
	size_t len,
	char *exception_chars,
	size_t exception_chars_n
)
{

	// set flags
	BP_STRING_CONFORMITY_FLAGS_T c_flags = 0;

	// if we have no string and a length, we can't determine
	// anything (essentially erroneous condition)
	if(!string && len)
	{

		// bad value
		BP_SET_FLAG(c_flags, BP_STRING_CONFORMITY_BADVAL);

		// return the flags
		return c_flags;

	}

	// check for null value
	if(!string && !len)
	{

			// bad value
			BP_SET_FLAG(c_flags, BP_STRING_CONFORMITY_BADVAL);

			// return the flags
			return c_flags;

	}

	// check for empty string
	if(string && !len)
	{


		if(string[0] != '\0')
		{

			// bad value if the string isn't immediately null terminated (prevent potential corruptions down the line)
			BP_SET_FLAG(c_flags, BP_STRING_CONFORMITY_BADVAL);

		}
		else
		{

			// if the string is NULL, terminated set the flag here and immediately return (no more checks are required)
			BP_SET_FLAG(c_flags, BP_STRING_CONFORMS_AS_EMPTY_STRING);

		}

		// return the flags
		return c_flags;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Minimal Buffer Analysis %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create ascii configuration
	BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION ascii_config;

	// zero out the structure (automagic macro)
	BP_ZERO_STACK(ascii_config);

	// set all counters to true
	ascii_config.count_ascii                      = BP_TRUE;
	ascii_config.count_extended                   = BP_TRUE;
	ascii_config.count_individual_characters      = BP_TRUE;

	// attempt to generate buffer analysis
	P_BP_BUFFER_ANALYSIS buff_analysis = BP_CreateBufferAnalysis
	(
			(unsigned char *) string,
			(size_t) len,
			&ascii_config
	);

	// ensure we h ave a buffer analysis
	if(!buff_analysis)
	{
		BP_SET_FLAG(c_flags, BP_STRING_CONFORMITY_BADVAL);
		return c_flags;
	}

	// calculate the exception character length if it exists
	size_t exception_char_len = 0;
	if(exception_chars && exception_chars_n)
	{

		// run exception length calculation
		size_t exception_iter = 0;
		for(; exception_iter < exception_chars_n; exception_iter++)
		{
			exception_char_len += buff_analysis->ascii->char_counts[exception_chars[exception_iter]];
		}

	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Fast Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Explanation: Rather than use loops to create checks, our buffer analysis
	//              subsystem simply filters characters by count.  So, using this
	//              methodology, we're able to simply add character counts together
	//              to check them against the data length.  If the len == character count
	//              we know ONLY those characters exist in the buffer.  Simple as that.

	// get the digit length (number of characters in all digit places)

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Numeric Fast Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Fastcheck: Numeric digit only
	if((
			buff_analysis->ascii->char_counts['0'] +
			buff_analysis->ascii->char_counts['1'] +
			buff_analysis->ascii->char_counts['2'] +
			buff_analysis->ascii->char_counts['3'] +
			buff_analysis->ascii->char_counts['4'] +
			buff_analysis->ascii->char_counts['5'] +
			buff_analysis->ascii->char_counts['6'] +
			buff_analysis->ascii->char_counts['7'] +
			buff_analysis->ascii->char_counts['8'] +
			buff_analysis->ascii->char_counts['9']
	) + exception_char_len == len)
	{

		BP_SET_FLAG(c_flags, BP_STRING_CONFORMS_AS_NUMERIC_ONLY);
		BP_DestroyBufferAnalysis(buff_analysis);
		return c_flags;
	}





	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Alphabetic Fast Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// Fastcheck: Alphabetic lowercase only.
	if((

			buff_analysis->ascii->char_counts['a'] +
			buff_analysis->ascii->char_counts['b'] +
			buff_analysis->ascii->char_counts['c'] +
			buff_analysis->ascii->char_counts['d'] +
			buff_analysis->ascii->char_counts['e'] +
			buff_analysis->ascii->char_counts['f'] +
			buff_analysis->ascii->char_counts['g'] +
			buff_analysis->ascii->char_counts['h'] +
			buff_analysis->ascii->char_counts['i'] +
			buff_analysis->ascii->char_counts['j'] +
			buff_analysis->ascii->char_counts['k'] +
			buff_analysis->ascii->char_counts['l'] +
			buff_analysis->ascii->char_counts['m'] +
			buff_analysis->ascii->char_counts['n'] +
			buff_analysis->ascii->char_counts['o'] +
			buff_analysis->ascii->char_counts['p'] +
			buff_analysis->ascii->char_counts['q'] +
			buff_analysis->ascii->char_counts['r'] +
			buff_analysis->ascii->char_counts['s'] +
			buff_analysis->ascii->char_counts['t'] +
			buff_analysis->ascii->char_counts['u'] +
			buff_analysis->ascii->char_counts['v'] +
			buff_analysis->ascii->char_counts['w'] +
			buff_analysis->ascii->char_counts['x'] +
			buff_analysis->ascii->char_counts['y'] +
			buff_analysis->ascii->char_counts['z']
	) + exception_char_len  == len)
	{

		BP_SET_FLAG(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHA);
		BP_SET_FLAG(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHA_LOWER_ONLY);
		BP_DestroyBufferAnalysis(buff_analysis);
		return c_flags;

	}


	// Fastcheck: Alphabetic uppercase only.
	if((

			buff_analysis->ascii->char_counts['A'] +
			buff_analysis->ascii->char_counts['B'] +
			buff_analysis->ascii->char_counts['C'] +
			buff_analysis->ascii->char_counts['D'] +
			buff_analysis->ascii->char_counts['E'] +
			buff_analysis->ascii->char_counts['F'] +
			buff_analysis->ascii->char_counts['G'] +
			buff_analysis->ascii->char_counts['H'] +
			buff_analysis->ascii->char_counts['I'] +
			buff_analysis->ascii->char_counts['J'] +
			buff_analysis->ascii->char_counts['K'] +
			buff_analysis->ascii->char_counts['L'] +
			buff_analysis->ascii->char_counts['M'] +
			buff_analysis->ascii->char_counts['N'] +
			buff_analysis->ascii->char_counts['O'] +
			buff_analysis->ascii->char_counts['P'] +
			buff_analysis->ascii->char_counts['Q'] +
			buff_analysis->ascii->char_counts['R'] +
			buff_analysis->ascii->char_counts['S'] +
			buff_analysis->ascii->char_counts['T'] +
			buff_analysis->ascii->char_counts['U'] +
			buff_analysis->ascii->char_counts['V'] +
			buff_analysis->ascii->char_counts['W'] +
			buff_analysis->ascii->char_counts['X'] +
			buff_analysis->ascii->char_counts['Y'] +
			buff_analysis->ascii->char_counts['Z']

	) + exception_char_len  == len)
	{

		BP_SET_FLAG(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHA);
		BP_SET_FLAG(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHA_UPPER_ONLY);
		BP_DestroyBufferAnalysis(buff_analysis);
		return c_flags;

	}



	// Fastcheck: Alphabetic Mixed Only
	if((

			buff_analysis->ascii->char_counts['A'] +
			buff_analysis->ascii->char_counts['B'] +
			buff_analysis->ascii->char_counts['C'] +
			buff_analysis->ascii->char_counts['D'] +
			buff_analysis->ascii->char_counts['E'] +
			buff_analysis->ascii->char_counts['F'] +
			buff_analysis->ascii->char_counts['G'] +
			buff_analysis->ascii->char_counts['H'] +
			buff_analysis->ascii->char_counts['I'] +
			buff_analysis->ascii->char_counts['J'] +
			buff_analysis->ascii->char_counts['K'] +
			buff_analysis->ascii->char_counts['L'] +
			buff_analysis->ascii->char_counts['M'] +
			buff_analysis->ascii->char_counts['N'] +
			buff_analysis->ascii->char_counts['O'] +
			buff_analysis->ascii->char_counts['P'] +
			buff_analysis->ascii->char_counts['Q'] +
			buff_analysis->ascii->char_counts['R'] +
			buff_analysis->ascii->char_counts['S'] +
			buff_analysis->ascii->char_counts['T'] +
			buff_analysis->ascii->char_counts['U'] +
			buff_analysis->ascii->char_counts['V'] +
			buff_analysis->ascii->char_counts['W'] +
			buff_analysis->ascii->char_counts['X'] +
			buff_analysis->ascii->char_counts['Y'] +
			buff_analysis->ascii->char_counts['Z'] +
			buff_analysis->ascii->char_counts['a'] +
			buff_analysis->ascii->char_counts['b'] +
			buff_analysis->ascii->char_counts['c'] +
			buff_analysis->ascii->char_counts['d'] +
			buff_analysis->ascii->char_counts['e'] +
			buff_analysis->ascii->char_counts['f'] +
			buff_analysis->ascii->char_counts['g'] +
			buff_analysis->ascii->char_counts['h'] +
			buff_analysis->ascii->char_counts['i'] +
			buff_analysis->ascii->char_counts['j'] +
			buff_analysis->ascii->char_counts['k'] +
			buff_analysis->ascii->char_counts['l'] +
			buff_analysis->ascii->char_counts['m'] +
			buff_analysis->ascii->char_counts['n'] +
			buff_analysis->ascii->char_counts['o'] +
			buff_analysis->ascii->char_counts['p'] +
			buff_analysis->ascii->char_counts['q'] +
			buff_analysis->ascii->char_counts['r'] +
			buff_analysis->ascii->char_counts['s'] +
			buff_analysis->ascii->char_counts['t'] +
			buff_analysis->ascii->char_counts['u'] +
			buff_analysis->ascii->char_counts['v'] +
			buff_analysis->ascii->char_counts['w'] +
			buff_analysis->ascii->char_counts['x'] +
			buff_analysis->ascii->char_counts['y'] +
			buff_analysis->ascii->char_counts['z']
	) + exception_char_len  == len)
	{

		BP_SET_FLAG(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHA);
		BP_SET_FLAG(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHA_MIXED_CASE);
		BP_DestroyBufferAnalysis(buff_analysis);
		return c_flags;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Hexidecimal Fast Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Fastcheck: Hexidecimal lowercase digits only
	if((
			buff_analysis->ascii->char_counts['0'] +
			buff_analysis->ascii->char_counts['1'] +
			buff_analysis->ascii->char_counts['2'] +
			buff_analysis->ascii->char_counts['3'] +
			buff_analysis->ascii->char_counts['4'] +
			buff_analysis->ascii->char_counts['5'] +
			buff_analysis->ascii->char_counts['6'] +
			buff_analysis->ascii->char_counts['7'] +
			buff_analysis->ascii->char_counts['8'] +
			buff_analysis->ascii->char_counts['9'] +
			buff_analysis->ascii->char_counts['a'] +
			buff_analysis->ascii->char_counts['b'] +
			buff_analysis->ascii->char_counts['c'] +
			buff_analysis->ascii->char_counts['d'] +
			buff_analysis->ascii->char_counts['e'] +
			buff_analysis->ascii->char_counts['f']
	) + exception_char_len  == len)
	{

		BP_SET_FLAG(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHANUM);
		BP_SET_FLAG(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHANUM_LOWER_ONLY);
		BP_SET_FLAG(c_flags, BP_STRING_CONFORMS_AS_HEXIDECIMAL);
		BP_SET_FLAG(c_flags, BP_STRING_CONFORMS_AS_HEXIDECIMAL_LOWER_ONLY);
		BP_DestroyBufferAnalysis(buff_analysis);
		return c_flags;

	}

	// Fastcheck: Hexidecimal uppercase digits only
	if((
			buff_analysis->ascii->char_counts['0'] +
			buff_analysis->ascii->char_counts['1'] +
			buff_analysis->ascii->char_counts['2'] +
			buff_analysis->ascii->char_counts['3'] +
			buff_analysis->ascii->char_counts['4'] +
			buff_analysis->ascii->char_counts['5'] +
			buff_analysis->ascii->char_counts['6'] +
			buff_analysis->ascii->char_counts['7'] +
			buff_analysis->ascii->char_counts['8'] +
			buff_analysis->ascii->char_counts['9'] +
			buff_analysis->ascii->char_counts['A'] +
			buff_analysis->ascii->char_counts['B'] +
			buff_analysis->ascii->char_counts['C'] +
			buff_analysis->ascii->char_counts['D'] +
			buff_analysis->ascii->char_counts['E'] +
			buff_analysis->ascii->char_counts['F']
	) + exception_char_len  == len)
	{

		BP_SET_FLAG(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHANUM);
		BP_SET_FLAG(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHANUM_UPPER_ONLY);
		BP_SET_FLAG(c_flags, BP_STRING_CONFORMS_AS_HEXIDECIMAL);
		BP_SET_FLAG(c_flags, BP_STRING_CONFORMS_AS_HEXIDECIMAL_UPPER_ONLY);
		BP_DestroyBufferAnalysis(buff_analysis);
		return c_flags;

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Alphanumeric Fast Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Fastcheck: Alphanumeric lowercase only.
	if((
			buff_analysis->ascii->char_counts['0'] +
			buff_analysis->ascii->char_counts['1'] +
			buff_analysis->ascii->char_counts['2'] +
			buff_analysis->ascii->char_counts['3'] +
			buff_analysis->ascii->char_counts['4'] +
			buff_analysis->ascii->char_counts['5'] +
			buff_analysis->ascii->char_counts['6'] +
			buff_analysis->ascii->char_counts['7'] +
			buff_analysis->ascii->char_counts['8'] +
			buff_analysis->ascii->char_counts['9'] +
			buff_analysis->ascii->char_counts['a'] +
			buff_analysis->ascii->char_counts['b'] +
			buff_analysis->ascii->char_counts['c'] +
			buff_analysis->ascii->char_counts['d'] +
			buff_analysis->ascii->char_counts['e'] +
			buff_analysis->ascii->char_counts['f'] +
			buff_analysis->ascii->char_counts['g'] +
			buff_analysis->ascii->char_counts['h'] +
			buff_analysis->ascii->char_counts['i'] +
			buff_analysis->ascii->char_counts['j'] +
			buff_analysis->ascii->char_counts['k'] +
			buff_analysis->ascii->char_counts['l'] +
			buff_analysis->ascii->char_counts['m'] +
			buff_analysis->ascii->char_counts['n'] +
			buff_analysis->ascii->char_counts['o'] +
			buff_analysis->ascii->char_counts['p'] +
			buff_analysis->ascii->char_counts['q'] +
			buff_analysis->ascii->char_counts['r'] +
			buff_analysis->ascii->char_counts['s'] +
			buff_analysis->ascii->char_counts['t'] +
			buff_analysis->ascii->char_counts['u'] +
			buff_analysis->ascii->char_counts['v'] +
			buff_analysis->ascii->char_counts['w'] +
			buff_analysis->ascii->char_counts['x'] +
			buff_analysis->ascii->char_counts['y'] +
			buff_analysis->ascii->char_counts['z']
	) + exception_char_len  == len)
	{

		BP_SET_FLAG(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHANUM);
		BP_SET_FLAG(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHANUM_LOWER_ONLY);
		BP_DestroyBufferAnalysis(buff_analysis);
		return c_flags;

	}


	// Fastcheck: Alphanumeric uppercase only.
	if((
			buff_analysis->ascii->char_counts['0'] +
			buff_analysis->ascii->char_counts['1'] +
			buff_analysis->ascii->char_counts['2'] +
			buff_analysis->ascii->char_counts['3'] +
			buff_analysis->ascii->char_counts['4'] +
			buff_analysis->ascii->char_counts['5'] +
			buff_analysis->ascii->char_counts['6'] +
			buff_analysis->ascii->char_counts['7'] +
			buff_analysis->ascii->char_counts['8'] +
			buff_analysis->ascii->char_counts['9'] +
			buff_analysis->ascii->char_counts['A'] +
			buff_analysis->ascii->char_counts['B'] +
			buff_analysis->ascii->char_counts['C'] +
			buff_analysis->ascii->char_counts['D'] +
			buff_analysis->ascii->char_counts['E'] +
			buff_analysis->ascii->char_counts['F'] +
			buff_analysis->ascii->char_counts['G'] +
			buff_analysis->ascii->char_counts['H'] +
			buff_analysis->ascii->char_counts['I'] +
			buff_analysis->ascii->char_counts['J'] +
			buff_analysis->ascii->char_counts['K'] +
			buff_analysis->ascii->char_counts['L'] +
			buff_analysis->ascii->char_counts['M'] +
			buff_analysis->ascii->char_counts['N'] +
			buff_analysis->ascii->char_counts['O'] +
			buff_analysis->ascii->char_counts['P'] +
			buff_analysis->ascii->char_counts['Q'] +
			buff_analysis->ascii->char_counts['R'] +
			buff_analysis->ascii->char_counts['S'] +
			buff_analysis->ascii->char_counts['T'] +
			buff_analysis->ascii->char_counts['U'] +
			buff_analysis->ascii->char_counts['V'] +
			buff_analysis->ascii->char_counts['W'] +
			buff_analysis->ascii->char_counts['X'] +
			buff_analysis->ascii->char_counts['Y'] +
			buff_analysis->ascii->char_counts['Z']

	) + exception_char_len  == len)
	{

		BP_SET_FLAG(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHANUM);
		BP_SET_FLAG(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHANUM_UPPER_ONLY);
		BP_DestroyBufferAnalysis(buff_analysis);
		return c_flags;

	}



	// Fastcheck: Alphanumeric Mixed Only
	if((
			buff_analysis->ascii->char_counts['0'] +
			buff_analysis->ascii->char_counts['1'] +
			buff_analysis->ascii->char_counts['2'] +
			buff_analysis->ascii->char_counts['3'] +
			buff_analysis->ascii->char_counts['4'] +
			buff_analysis->ascii->char_counts['5'] +
			buff_analysis->ascii->char_counts['6'] +
			buff_analysis->ascii->char_counts['7'] +
			buff_analysis->ascii->char_counts['8'] +
			buff_analysis->ascii->char_counts['9'] +
			buff_analysis->ascii->char_counts['A'] +
			buff_analysis->ascii->char_counts['B'] +
			buff_analysis->ascii->char_counts['C'] +
			buff_analysis->ascii->char_counts['D'] +
			buff_analysis->ascii->char_counts['E'] +
			buff_analysis->ascii->char_counts['F'] +
			buff_analysis->ascii->char_counts['G'] +
			buff_analysis->ascii->char_counts['H'] +
			buff_analysis->ascii->char_counts['I'] +
			buff_analysis->ascii->char_counts['J'] +
			buff_analysis->ascii->char_counts['K'] +
			buff_analysis->ascii->char_counts['L'] +
			buff_analysis->ascii->char_counts['M'] +
			buff_analysis->ascii->char_counts['N'] +
			buff_analysis->ascii->char_counts['O'] +
			buff_analysis->ascii->char_counts['P'] +
			buff_analysis->ascii->char_counts['Q'] +
			buff_analysis->ascii->char_counts['R'] +
			buff_analysis->ascii->char_counts['S'] +
			buff_analysis->ascii->char_counts['T'] +
			buff_analysis->ascii->char_counts['U'] +
			buff_analysis->ascii->char_counts['V'] +
			buff_analysis->ascii->char_counts['W'] +
			buff_analysis->ascii->char_counts['X'] +
			buff_analysis->ascii->char_counts['Y'] +
			buff_analysis->ascii->char_counts['Z'] +
			buff_analysis->ascii->char_counts['a'] +
			buff_analysis->ascii->char_counts['b'] +
			buff_analysis->ascii->char_counts['c'] +
			buff_analysis->ascii->char_counts['d'] +
			buff_analysis->ascii->char_counts['e'] +
			buff_analysis->ascii->char_counts['f'] +
			buff_analysis->ascii->char_counts['g'] +
			buff_analysis->ascii->char_counts['h'] +
			buff_analysis->ascii->char_counts['i'] +
			buff_analysis->ascii->char_counts['j'] +
			buff_analysis->ascii->char_counts['k'] +
			buff_analysis->ascii->char_counts['l'] +
			buff_analysis->ascii->char_counts['m'] +
			buff_analysis->ascii->char_counts['n'] +
			buff_analysis->ascii->char_counts['o'] +
			buff_analysis->ascii->char_counts['p'] +
			buff_analysis->ascii->char_counts['q'] +
			buff_analysis->ascii->char_counts['r'] +
			buff_analysis->ascii->char_counts['s'] +
			buff_analysis->ascii->char_counts['t'] +
			buff_analysis->ascii->char_counts['u'] +
			buff_analysis->ascii->char_counts['v'] +
			buff_analysis->ascii->char_counts['w'] +
			buff_analysis->ascii->char_counts['x'] +
			buff_analysis->ascii->char_counts['y'] +
			buff_analysis->ascii->char_counts['z']
	) + exception_char_len  == len)
	{

		BP_SET_FLAG(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHANUM);
		BP_SET_FLAG(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHANUM_MIXED_CASE);
		BP_DestroyBufferAnalysis(buff_analysis);
		return c_flags;

	}


	// destroy the buffer analysis
	BP_DestroyBufferAnalysis(buff_analysis);

	// return the flags by default (val is zero if we get here)
	return c_flags;

}


// displays string conformity flags
BP_ERROR_T BP_DisplayStringConformityFlags(BP_STRING_CONFORMITY_FLAGS_T c_flags)
{

	// create new linkl
	P_BP_LINKL tq = BP_CreateLinkL(".", BP_TRUE);

	// check empty string only
	if(BP_FLAG_IS_SET(c_flags, BP_STRING_CONFORMS_AS_EMPTY_STRING))
		tq_strdup(tq, "\n\tBP_STRING_CONFORMS_AS_EMPTY_STRING", BPLN);

	// check numeric only
	if(BP_FLAG_IS_SET(c_flags, BP_STRING_CONFORMS_AS_NUMERIC_ONLY))
		tq_strdup(tq, "\n\tBP_STRING_CONFORMS_AS_NUMERIC_ONLY", BPLN);

	// check hexidecimal only
	if(BP_FLAG_IS_SET(c_flags, BP_STRING_CONFORMS_AS_HEXIDECIMAL))
		tq_strdup(tq, "\n\tBP_STRING_CONFORMS_AS_HEXIDECIMAL", BPLN);

	// check hexidecimal lower only
	if(BP_FLAG_IS_SET(c_flags, BP_STRING_CONFORMS_AS_HEXIDECIMAL_LOWER_ONLY))
		tq_strdup(tq, "\n\tBP_STRING_CONFORMS_AS_HEXIDECIMAL_LOWER_ONLY", BPLN);

	// check hexidecimal upper only
	if(BP_FLAG_IS_SET(c_flags, BP_STRING_CONFORMS_AS_HEXIDECIMAL_UPPER_ONLY))
		tq_strdup(tq, "\n\tBP_STRING_CONFORMS_AS_HEXIDECIMAL_UPPER_ONLY", BPLN);

	// check alpha
	if(BP_FLAG_IS_SET(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHA))
		tq_strdup(tq, "\n\tBP_STRING_CONFORMS_AS_ASCII_ALPHA", BPLN);

	// check alphalower
	if(BP_FLAG_IS_SET(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHA_LOWER_ONLY))
		tq_strdup(tq, "\n\tBP_STRING_CONFORMS_AS_ASCII_ALPHA_LOWER_ONLY", BPLN);

	// check alphaupper
	if(BP_FLAG_IS_SET(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHA_UPPER_ONLY))
		tq_strdup(tq, "\n\tBP_STRING_CONFORMS_AS_ASCII_ALPHA_UPPER_ONLY", BPLN);

	// check alphamixed
	if(BP_FLAG_IS_SET(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHA_MIXED_CASE))
		tq_strdup(tq, "\n\tBP_STRING_CONFORMS_AS_ASCII_ALPHA_MIXED_CASE", BPLN);

	// check alphanum
	if(BP_FLAG_IS_SET(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHANUM))
		tq_strdup(tq, "\n\tBP_STRING_CONFORMS_AS_ASCII_ALPHANUM", BPLN);

	// check alphanum lower
	if(BP_FLAG_IS_SET(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHANUM_LOWER_ONLY))
		tq_strdup(tq, "\n\tBP_STRING_CONFORMS_AS_ASCII_ALPHANUM_LOWER_ONLY", BPLN);

	// check alphanum upper
	if(BP_FLAG_IS_SET(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHANUM_UPPER_ONLY))
		tq_strdup(tq, "\n\tBP_STRING_CONFORMS_AS_ASCII_ALPHANUM_UPPER_ONLY", BPLN);

	// check alphanum mixed
	if(BP_FLAG_IS_SET(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHANUM_MIXED_CASE))
		tq_strdup(tq, "\n\tBP_STRING_CONFORMS_AS_ASCII_ALPHANUM_MIXED_CASE", BPLN);

	// flatten all elements into a joined buffer
	BP_LinkLFlattenJoinStringElements(tq);

	// string conformity flags
	printf("\n [+] String Conformity Flags (%lu): ", c_flags);
	printf("%s", (char *) tq->flat_buff);

	// destroy the linkl
	BP_DestroyLinkL(tq, BP_TRUE);

	// return indicating success
	return ERR_SUCCESS;

}



// ###################################################################
// ## PCRE Routines ##################################################
// ###################################################################

BP_ERROR_T BP_MatchRegularExpressionPCRE
(
	_vin char *  haystack,
	_vin size_t  haystack_size,
	_vin char *  regular_expression,
	_vin int     regexp_flags,
	_vin size_t  max_match_count,
	_vinout P_BP_REGEX_MATCH_INFO match_info,
	_vin size_t  save_matches
)
{

	// cascading parameter checks
	if(!haystack)
		return ERR_FAILURE;
	if(!haystack_size)
		return ERR_FAILURE;
	if(!regular_expression)
		return ERR_FAILURE;
	if(!strnlen(regular_expression, 1))
		return ERR_FAILURE;
	if(!max_match_count)
		return ERR_FAILURE;


	// set match info structure
	BP_REGEX_MATCH_INFO match_info_tmp;
	BP_ZERO_STACK(match_info_tmp);

	// if no match info structure is passed in, use
	// a temporary dummy one in its place.
	if(!match_info)
	{

		// set pointer as stack temporary info struct
		match_info = (P_BP_REGEX_MATCH_INFO) &match_info_tmp;

	}


	// error and error offset values
	const char *error = NULL;
	int   erroffset   = 0;

	// regular expression
	pcre *re          = NULL;

	// return value of executed expression (length or code)
	int   rc          = 0;

	// iterator
	int   i           = 0;

	// stored return vectors
	int   ovector[max_match_count+100];

	// zero the ovector stack
	BP_ZERO_STACK(ovector);



	// pcre_compile below sometimes performs some invalid writes
	// this attempts to fix that by providing extra truncating data.
	// compile regular expression
	re = pcre_compile
	(
			regular_expression,
			regexp_flags,
	    	&error,
	    	&erroffset,
	    	0
	);

	// set a flag indicating that matches are stored on the heap
	// in the case that it's needed.
	if(save_matches)
	{
		match_info->pcre_string_matches_saved = true;
	}

	// check to ensure that the regular expression compiled
	if (!re)
	{
		// bpfree(regular_expression_extra);
		printf("\n [!] Unable to compile perl compatible regular expression (%s)", regular_expression);
		return ERR_FAILURE;
	}



	// sub string
	char * psubStrMatchStr  = NULL;

	// set offset and length
	unsigned int offset = 0;
	unsigned int len    = haystack_size;

	// set the match count
	size_t match_count = 0;

	// set defaults
	match_info->preg_matches 	 = NULL;
	match_info->pcre_enabled 	 = 0;
	match_info->pcre_num_matched = 0;

	// set distinction in structure to show this is a pcre match
	match_info->pcre_enabled = 1;

	do
	{


		// break loop if our offset gets too high
		if(offset >= haystack_size)
			break;

		// find all matches
		rc = pcre_exec
		(
				re,
				0,
				haystack,
				haystack_size,
				offset,
				0,
				ovector,
				max_match_count*sizeof(int)
		);

		// ensure we have a match
		if(rc == PCRE_ERROR_NOMATCH)
			break;

		// switch on bad returns
		if(rc < 0)
		switch(rc)
		{

			case PCRE_ERROR_NULL:
				pcre_free(re);
				return ERR_FAILURE;
				break;

			case PCRE_ERROR_BADOPTION:
				pcre_free(re);
				return ERR_FAILURE;
				break;

			case PCRE_ERROR_BADMAGIC:
				pcre_free(re);
				return ERR_FAILURE;
				break;

			case PCRE_ERROR_UNKNOWN_NODE:
				pcre_free(re);
				return ERR_FAILURE;
				break;

			case PCRE_ERROR_NOMEMORY:
				pcre_free(re);
				return ERR_FAILURE;
				break;

			default:
				pcre_free(re);
				return ERR_FAILURE;
				break;

		}

		// if rc reports what we've matched too many, set the max rc to
		// the max match count.
		if(!rc)
		{
			rc = 1;
		}

		// iterate all matches
		if(rc)
		for(int i = 0; i < rc; ++i)
		{

			// break if there is no rc
			if(!rc)
				break;

			// break the loop if we're too far
			if(match_count >= max_match_count)
				break;

			// increment match counter
			match_count++;

			// get the substring
			pcre_get_substring
			(
					haystack,
					ovector,
					rc,
					i,
					(const char **) &(psubStrMatchStr)
			);

			// display substring (debug optional)
			// printf("\n Substring[%i] - %i: %s", rc,  i, psubStrMatchStr);


			// if the user indicates the interest to save matches, this is where it occurs
			if(save_matches)
			{

				// allocate regular expression match structure
				match_info->preg_matches = (p_pregmatch_t) bprealloc(match_info->preg_matches, sizeof(pregmatch_t) * match_count);

				// zero out the new structure
				memset(&match_info->preg_matches[match_count-1], 0x00, sizeof(pregmatch_t));

				// set vectors
				match_info->preg_matches[match_count-1].match_len = ovector[2*i+1] - ovector[2*i];

				// duplicate string
				/*
				printf("\n MATCHED REGEXP: %s", regular_expression);
				printf("\n ATTEMPTING STRING DUP IN REGEXP?: %s", psubStrMatchStr);
				*/
				match_info->preg_matches[match_count-1].matched_string = (char *) bpstrndup(psubStrMatchStr, bpstrlen(psubStrMatchStr));

				//match_info->preg_matches[match_count-1].match_index;
				match_info->preg_matches[match_count-1].match_index = ovector[0];
			}

			// increase the match count
			match_info->pcre_num_matched++;

			// destroy the match string
			if(psubStrMatchStr)
			{
				pcre_free_substring(psubStrMatchStr);
				psubStrMatchStr = NULL;
			}

		}

		// destroy the match string
		if(psubStrMatchStr)
		{
			pcre_free_substring(psubStrMatchStr);
			psubStrMatchStr = NULL;
		}

		// break the loop if we're too far
		if(match_count >= max_match_count)
			break;

		// set match count
		if(!match_count)
		{

			pcre_free(re);
			//bpfree(regular_expression_extra);
			return ERR_FAILURE;
		}


		// move offset and advance
		offset = ovector[1];

	} while(1);

	// destroy the re if necessary
	pcre_free(re);

	//bpfree(regular_expression_extra);



	// if we have matched items, return indicating so
	if(match_count)
	{

		// printf("\n RETURNING WITH POSITIVE MATCH COUNT");
		return ERR_SUCCESS;
	}

	// no match
	return ERR_FAILURE;
}

// perl compatible regular expression match info display routine.
//BP_DisplayRegexMatchInfoPCRE
BP_ERROR_T BP_DisplayRegexMatchInfoPCRE(P_BP_REGEX_MATCH_INFO match_info)
{


	if(!match_info)
		return ERR_FAILURE;

	if(!match_info->pcre_enabled)
		return ERR_FAILURE;

	if(!match_info->pcre_num_matched)
		return ERR_FAILURE;

	// walk arrays and display match content
	printf("\n [+] PCRE Match for %p: Indexes relative to start of string.", match_info);

	size_t i = 0;
	for(i = 0; i < match_info->pcre_num_matched; i++)
	{
		printf("\n\t -> Match #%u:", i);
		printf("\n\t      Start Byte Index: %u", match_info->preg_matches[i].match_index);
		printf("\n\t      Match Length:     %u", match_info->preg_matches[i].match_len);
		if(match_info->preg_matches[i].matched_string)
			printf("\n\t      Match Length:     %s", match_info->preg_matches[i].matched_string);

	}

	// return indicating no match
	return ERR_FAILURE;

}

// perl compatible regular expression find and replace routine.
char * BP_RegFindReplacePCRE
(
		char * in_buff,
		size_t buff_len,
		char * replace_this,
		char * replace_with,
		size_t replace_size,
		size_t max_replacements,
		size_t regexp_flags
)
{

	if(!in_buff)
		return NULL;
	if(!buff_len)
		return NULL;
	if(!replace_this)
		return NULL;
	if(!replace_with)
		return NULL;
	if(!replace_size)
		return NULL;
	if(!max_replacements)
		return NULL;

	// create new match info structure
	BP_REGEX_MATCH_INFO match_info;
	memset(&match_info, 0x00, sizeof(match_info));

	// attempt to match a regular expression (must save matches)
	size_t matched = BP_MatchRegularExpressionPCRE
	(
			in_buff,
			buff_len,
			replace_this,
			regexp_flags,
			max_replacements,
			&match_info,
			true
	);

	// printf("\n Getting past initial checks: %u", matched);

	// if no matches are to be performed, exit with indication
	if(!matched)
		return NULL;

	// ensure the matches have been set
	if(!match_info.preg_matches)
		return NULL;

	// ensure we have a match count
	if(!match_info.pcre_num_matched)
		return NULL;


	// this string will be allocated and retrofitted
	char * out_str = NULL;

	// check num matched
	size_t n = 0;
	size_t buff_len_sub = 0;
	size_t buff_len_add = 0;

	for(; n < match_info.pcre_num_matched; n++)
	{

		// subtract the match length
		buff_len_sub += match_info.preg_matches[n].match_len;

		// add the replace size
		buff_len_add += replace_size+4;
	}

	// printf("\n totals?: %u - %u", match_info.pcre_num_matched, max_replacements);

	// set buffer length for bpreallocation
	size_t buff_new_len = 0;
	buff_new_len =  buff_len - buff_len_sub;
	buff_new_len += buff_len_add + 1;
	buff_new_len += buff_len_sub;

	// allocate out buffer
	out_str = (char * ) bpcalloc(buff_new_len, 1);

	// now create the new buffer
	size_t buff_write_index = 0;
	size_t buff_read_index  = 0;
	size_t buff_read_len    = 0;


	// outbuffer write position
	size_t outbuff_write_position = 0;
	size_t inbuff_read_position   = 0;


	// loop for recalculating/allocating substituion buffer
	for
	(
		n = 0;
		n < match_info.pcre_num_matched;
		n++
	)
	{

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Check 1: We've Matched at Zero Index %%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// if we've matched at the 0 position in the out buff, we
		// simply copy directly.
		if(!match_info.preg_matches[n].match_index)
		{

			// copy out data
			memcpy((void *) &out_str[outbuff_write_position], replace_with, replace_size);

			// move write position up past the replacement size
			outbuff_write_position += replace_size;
			inbuff_read_position   += match_info.preg_matches[n].match_len;

			// this is used to calculate copy distance
			size_t copy_distance_between_replace_and_next_index = 0;

			// if we have a next match, calculate distance
			if((n+1) < match_info.pcre_num_matched)
			{


				// run distance calculation
				copy_distance_between_replace_and_next_index = (match_info.preg_matches[n+1].match_index - match_info.preg_matches[n].match_index) - match_info.preg_matches[n+1].match_len;

				// copy replacement data
				if(copy_distance_between_replace_and_next_index)
				{

					// copy data if necessary
					memcpy((void *) &out_str[outbuff_write_position], &in_buff[inbuff_read_position], copy_distance_between_replace_and_next_index);

					// move up the read position
					inbuff_read_position   += copy_distance_between_replace_and_next_index;
					outbuff_write_position += copy_distance_between_replace_and_next_index;

				}

			}

			// continue here since all data was copied
			continue;

		}

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Check 2: Copy From Found Indexes %%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// if we have a match index, we can copy from position
		if(match_info.preg_matches[n].match_index)
		{

			// copy in buffer front where necessary
			if(!outbuff_write_position)
			{

				// copy data if necessary
				memcpy((void *) &out_str[outbuff_write_position], &in_buff[inbuff_read_position], match_info.preg_matches[n].match_index);

				// move up the read position
				inbuff_read_position   += match_info.preg_matches[n].match_index;
				outbuff_write_position += match_info.preg_matches[n].match_index;

			}


			// copy out data
			memcpy((void *) &out_str[outbuff_write_position], replace_with, replace_size);

			// move write position up past the replacement size
			outbuff_write_position += replace_size;
			inbuff_read_position   += match_info.preg_matches[n].match_len;

			// this is used to calculate copy distance
			size_t copy_distance_between_replace_and_next_index = 0;

			// if we have a next match, calculate distance
			if((n+1) < match_info.pcre_num_matched)
			{


				// run distance calculation
				copy_distance_between_replace_and_next_index = (match_info.preg_matches[n+1].match_index - match_info.preg_matches[n].match_index) - match_info.preg_matches[n+1].match_len;

				// copy replacement data
				if(copy_distance_between_replace_and_next_index)
				{

					// copy data if necessary
					memcpy((void *) &out_str[outbuff_write_position], &in_buff[inbuff_read_position], copy_distance_between_replace_and_next_index);

					// move up the read position
					inbuff_read_position   += copy_distance_between_replace_and_next_index;
					outbuff_write_position += copy_distance_between_replace_and_next_index;

				}

			}

			// continue here since all data was copied
			continue;

		}

	}


	// copy in the tail of the buffer
	if(bpstrlen(&in_buff[inbuff_read_position]))
	{

		// copy data if necessary
		memcpy((void *) &out_str[outbuff_write_position], &in_buff[inbuff_read_position], (buff_len - inbuff_read_position));

	}

	// printf("\n Outstr: %s", out_str);

	// destroy the match info
	BP_DestroyRegexpMatchInfo(&match_info);


	// return the newly allocated string
	return out_str;

}


// destroys all member data within the match structure
BP_ERROR_T BP_DestroyRegexpMatchInfo(_vdestroy P_BP_REGEX_MATCH_INFO match_info)
{


	size_t n = 0;
	if(match_info->pcre_string_matches_saved)
	if(match_info->preg_matches)
	for(; n < match_info->pcre_num_matched; n++)
	{

		// destroy matched string if it's set
		if(match_info->preg_matches[n].matched_string)
			bpfree(match_info->preg_matches[n].matched_string);


		// set matched string to null
		match_info->preg_matches[n].matched_string = NULL;

	}

	// destroy matches (pcre)
	if(match_info->preg_matches)
		bpfree(match_info->preg_matches);

	// destroy matches (posix)
	if(match_info->matches)
		bpfree(match_info->matches);

	// return indicating success
	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% URL Decoding %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// used in the url decoder below
char BP_FromHex(char ch)
{
  return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

// decodes a url
char *BP_URLDecode(char *str)
{

	// ensure we have a string to decode
	if(!str)
		return NULL;

	// ensure string is reasonable before decoding
	if(!BP_StringIsReasonablePrintableString(str, bpstrlen(str), BP_FALSE, BP_FALSE))
		return NULL;

	char *pstr = str, *buf = (char *) malloc(bpstrlen(str) + 1), *pbuf = buf;

	while (*pstr)
	{

		if (*pstr == '%')
		{

		  if (pstr[1] && pstr[2])
		  {
			*pbuf++ = BP_FromHex(pstr[1]) << 4 | BP_FromHex(pstr[2]);
			pstr += 2;
		  }

		}
		else if (*pstr == '+')
		{
		  *pbuf++ = ' ';
		}
		else
		{
		  *pbuf++ = *pstr;
		}
		pstr++;
	}


	// terminate the buffer
	*pbuf = '\0';
	return buf;

}


// ###################################################################
// ## Blacklist Routines #############################################
// ###################################################################

// USES PCRE: This function takes a blacklist, an item, and an item count and checks
// to see that the item exists as a regexp in the blacklist.  If so it returns
// the position (first, second, third) of the index which matched. If there
// is no match, it returns 0.  Note: The 0 index, is the first position.
int BP_StringMatchesBlacklistREGEXP(char ** blacklist, size_t blacklist_cnt, char *item)
{

	// run initial checks
	if(!blacklist || !blacklist_cnt || !item)
		return ERR_FAILURE;

	// loop index for blacklist iteration loop
	size_t i = 0;

	// walk every entry in the blacklist array, and compare it with the anchor href
	for(i = 0; i < blacklist_cnt; i++)
	{

		// check for empty/no length entries
		if(!blacklist[i])
			break;
		if(!strnlen(blacklist[i], 1))
			continue;

		// store match information
		BP_REGEX_MATCH_INFO match_info = {0};

		//	printf("\n Trying to match: %s in \n this: %s", item, &blacklist[i][0]);

		// match regular expression
		BP_ERROR_T match_code = BP_MatchRegularExpressionPCRE
				(
						item,
						bpstrlen(item),
						(char *)&blacklist[i][0],
						0,
						1,
						(P_BP_REGEX_MATCH_INFO) &match_info,
						true
				);

		// destroy match info where necessary
		BP_DestroyRegexpMatchInfo(&match_info);

		// check for a successful match and return success (item is blacklisted)
		if(match_code == ERR_SUCCESS)
			return i+1;

	}

	// return indicating that the item isn't in the blacklist
	return ERR_FAILURE;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% SHA1 Generation From Buffer or Buffer Stack %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Generates a SHA1 has provided given input.  Returned string must be bpfreed.
char * BP_GenSHA1(char *in, size_t in_size)
{

	if(!in || !in_size)
		return NULL;

	// now create ascii buffer
	char *sha1_ascii = (char *) bpcalloc(41, 1);

	// 20 byte outbuffer
	unsigned char obuf[20];

	// create hash
	SHA1((unsigned char *) in, in_size, (unsigned char *) &obuf);

	// create ascii hash from binary
	size_t i = 0;
	for(; i < 20; i++)
	{
		snprintf((char *) &sha1_ascii[bpstrlen(sha1_ascii)], 4, "%.02x", obuf[i]);
	}

	// return the ascii
	return sha1_ascii;

}

// Generates a SHA1 provided given input of a null ptr terminated string array.  Returned string must be bpfreed.  This function
// is obviously not length safe, and should be used with caution.  Max string length for this
// function is 2048.
char * BP_GenSHA1FromStringArray(char **in)
{

	if(!in)
		return NULL;

	// loop iter
	size_t j = 0;

	// length counter
	size_t x = 0;

	// bprealloced loop string, used for mdfs
	char *loop_str     = NULL;
	char *md5_item_str = NULL;

	// walk the strings
	for(; in[j]; j++)
	{

		// make sure the string has a length
		if(!bpstrlen(in[j]))
			continue;

		// generate the md5 for an item
		md5_item_str = (char *) BP_GenSHA1(in[j], strnlen(in[j], 2048));
		if(!md5_item_str)
			return NULL;

		// increase length of string
		loop_str = (char *) bprealloc(loop_str, (j*40) + 41);

		// zero out new position
		memset((char *) &loop_str[j*40], 0x00, 41);

		// copy in the buffer
		memcpy((char *) &loop_str[j*40], md5_item_str, 40);

		// bpfree string on loop
		bpfree(md5_item_str);
		md5_item_str = NULL;

	}

	if(!loop_str)
		return NULL;

	// sha1 the final string
	char *ret_str = BP_GenSHA1(loop_str, strnlen(loop_str, 4096));

	// destroy the aggregate sum chunk
	bpfree(loop_str);

	// return the newly allocated md5
	return ret_str;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% SHA256 Generation From Buffer or Buffer Stack %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

unsigned int k[64] = {
   0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
   0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
   0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
   0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
   0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
   0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
   0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
   0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

// NOTE: Used from http://bradconte.com/sha256_c (the author specifies no license or terms
//       of restriction for this code.

void sha256_transform(BP_SHA256_CTX *ctx, unsigned char data[])
{
   unsigned int a,b,c,d,e,f,g,h,i,j,t1,t2,m[64];

   for (i=0,j=0; i < 16; ++i, j += 4)
      m[i] = (data[j] << 24) | (data[j+1] << 16) | (data[j+2] << 8) | (data[j+3]);
   for ( ; i < 64; ++i)
      m[i] = SIG1(m[i-2]) + m[i-7] + SIG0(m[i-15]) + m[i-16];

   a = ctx->state[0];
   b = ctx->state[1];
   c = ctx->state[2];
   d = ctx->state[3];
   e = ctx->state[4];
   f = ctx->state[5];
   g = ctx->state[6];
   h = ctx->state[7];

   for (i = 0; i < 64; ++i) {
      t1 = h + EP1(e) + CH(e,f,g) + k[i] + m[i];
      t2 = EP0(a) + MAJ(a,b,c);
      h = g;
      g = f;
      f = e;
      e = d + t1;
      d = c;
      c = b;
      b = a;
      a = t1 + t2;
   }

   ctx->state[0] += a;
   ctx->state[1] += b;
   ctx->state[2] += c;
   ctx->state[3] += d;
   ctx->state[4] += e;
   ctx->state[5] += f;
   ctx->state[6] += g;
   ctx->state[7] += h;
}

void sha256_init(BP_SHA256_CTX *ctx)
{
   ctx->datalen = 0;
   ctx->bitlen[0] = 0;
   ctx->bitlen[1] = 0;
   ctx->state[0] = 0x6a09e667;
   ctx->state[1] = 0xbb67ae85;
   ctx->state[2] = 0x3c6ef372;
   ctx->state[3] = 0xa54ff53a;
   ctx->state[4] = 0x510e527f;
   ctx->state[5] = 0x9b05688c;
   ctx->state[6] = 0x1f83d9ab;
   ctx->state[7] = 0x5be0cd19;
}

void sha256_update(BP_SHA256_CTX *ctx, unsigned char data[], unsigned int len)
{
   unsigned int t,i;

   for (i=0; i < len; ++i) {
      ctx->data[ctx->datalen] = data[i];
      ctx->datalen++;
      if (ctx->datalen == 64) {
         sha256_transform(ctx,ctx->data);
         DBL_INT_ADD(ctx->bitlen[0],ctx->bitlen[1],512);
         ctx->datalen = 0;
      }
   }
}

void sha256_final(BP_SHA256_CTX *ctx, unsigned char hash[])
{
   unsigned int i;

   i = ctx->datalen;

   // Pad whatever data is left in the buffer.
   if (ctx->datalen < 56) {
      ctx->data[i++] = 0x80;
      while (i < 56)
         ctx->data[i++] = 0x00;
   }
   else {
      ctx->data[i++] = 0x80;
      while (i < 64)
         ctx->data[i++] = 0x00;
      sha256_transform(ctx,ctx->data);
      memset(ctx->data,0,56);
   }

   // Append to the padding the total message's length in bits and transform.
   DBL_INT_ADD(ctx->bitlen[0],ctx->bitlen[1],ctx->datalen * 8);
   ctx->data[63] = ctx->bitlen[0];
   ctx->data[62] = ctx->bitlen[0] >> 8;
   ctx->data[61] = ctx->bitlen[0] >> 16;
   ctx->data[60] = ctx->bitlen[0] >> 24;
   ctx->data[59] = ctx->bitlen[1];
   ctx->data[58] = ctx->bitlen[1] >> 8;
   ctx->data[57] = ctx->bitlen[1] >> 16;
   ctx->data[56] = ctx->bitlen[1] >> 24;
   sha256_transform(ctx,ctx->data);

   // Since this implementation uses little endian byte ordering and SHA uses big endian,
   // reverse all the bytes when copying the final state to the output hash.
   for (i=0; i < 4; ++i) {
      hash[i]    = (ctx->state[0] >> (24-i*8)) & 0x000000ff;
      hash[i+4]  = (ctx->state[1] >> (24-i*8)) & 0x000000ff;
      hash[i+8]  = (ctx->state[2] >> (24-i*8)) & 0x000000ff;
      hash[i+12] = (ctx->state[3] >> (24-i*8)) & 0x000000ff;
      hash[i+16] = (ctx->state[4] >> (24-i*8)) & 0x000000ff;
      hash[i+20] = (ctx->state[5] >> (24-i*8)) & 0x000000ff;
      hash[i+24] = (ctx->state[6] >> (24-i*8)) & 0x000000ff;
      hash[i+28] = (ctx->state[7] >> (24-i*8)) & 0x000000ff;
   }
}



// Attempts to generate a sha256 hash.
char * BP_GenSHA256(char *in, size_t in_size)
{

	// ensure we have a buffer and a size
	if(!in)
		return NULL;
	if(!in_size)
		return NULL;

	// context to use
	BP_SHA256_CTX ctx;

	// space for the hash
	unsigned char hash[32] = {0};

	// init the context
	sha256_init(&ctx);

	// calculate the hash
	sha256_update(&ctx, (unsigned char *) in, in_size);

	// generate the final hash
	sha256_final(&ctx,hash);


	// create the hash output before returning
	char * txt_sha256 = BP_BufferToHexString((char *) &hash, 32);

	// return the duplicated hash
	return txt_sha256;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% MD5 Generation From Buffer or Buffer Stack %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Simple routine to generate a MD5 Hash
char * BP_GenMD5(char *in, size_t in_size)
{

	// ensure we have data to work with
	if(!in && in_size)
		return NULL;

	// allocate space for md5
	char * md5 = (char *) bpcalloc(MD5_DIGEST_LENGTH+1, 1);

	// calculate the md5 sum
	MD5((unsigned char *) in, in_size, (unsigned char *) md5);

	// md5 as text
	char *txt_md5 = BP_BufferToHexString(md5, MD5_DIGEST_LENGTH);

	// destroy the md5
	bpfree(md5);

	// return the filled out md5 sum
	return txt_md5;

}



// Destroys an array of pointers, similar to that which is
// used in the fmtTable used by the test generator.
size_t BP_DestroyPointerArray(void ** ptrs, size_t ptrCount) {

	if (!ptrs)
		return ERR_FAILURE;

	size_t x = 0;
	for (x = 0; x < ptrCount; x++)
	{

		// only free non-null pointers
		if(ptrs[x])
			bpfree(ptrs[x]);

		// overwrite pointer with null
		ptrs[x] = NULL;

	}

	return ERR_SUCCESS;
}

// Destroys an array of pointers but relies on a trailing
// null terminating null pointer to determine when it should
// stop destroying.
BP_ERROR_T BP_DestroyPointerArrayNullTerminated(void ** ptrs)
{

	// check to ensure that we have a pointer
	if(!ptrs)
		return ERR_FAILURE;

	// destroy the individual elements
	size_t n = 0;
	for(; ptrs[n]; n++)
	{
		bpfree(ptrs[n]);
		ptrs[n] = NULL;
	}

	// return indicating success
	return ERR_SUCCESS;

}


// Destroys an array of pointers, TAILQ version..
size_t BP_DestroyPointerArrayTAILQ(P_BP_LINKL tq, void ** ptrs, size_t ptrCount) {

	if (!ptrs)
		return ERR_FAILURE;

	size_t x = 0;
	for (x = 0; x < ptrCount; x++) {
		tq_free(tq, ptrs[x]);
		ptrs[x] = NULL;
	}

	return ERR_SUCCESS;
}


// creates a string containing pseudorandom unsigned char data.
unsigned char * BP_GenRandomUnsignedCharString(size_t len)
{

	// run length check
	if(!len)
		return NULL;

	// allocate space for the ascii string
	unsigned char *random_str = (unsigned char *) bpcalloc(len+1, 1);

	// check to ensure data was allocated
	if(!random_str)
		return NULL;

	// generate the random string
	size_t n = 0;
	unsigned char curr_char = 0x00;

	// seed random with loose seed (not secure data)
	BP_SeedPNRG();

	// walk and create random string
	for(; n < len; n++)
	{

		// set the character after its validated
		random_str[n] = (unsigned char) rand();

	}

	// return the random string data
	return random_str;

}



// creates a string containing pseudorandom (not cryptographically secure) ASCII data
char * BP_GenRandomASCIICharString(size_t len, BP_GEN_RANDOM_ASCII_STRING_TYPE type, P_BP_LINKL tq)
{

	// run length check
	if(!len)
		return NULL;

	// allocate space for the ascii string
	char *random_str = NULL;
	if(!tq)
		random_str = (char *) bpcalloc(len+1, 1);
	else
		random_str = (char *) tq_calloc(tq, len+1, 1, BPLN);

	// check to ensure data was allocated
	if(!random_str)
		return NULL;

	// generate the random string
	size_t n = 0;
	char curr_char = 0x00;

	// seed random with loose seed (not secure data)
	BP_SeedPNRG();
	// srand((unsigned int) NEW_RANDOM_ID);

	for(; n < len; n++)
	{

		switch(type)
		{

			// no filter, generate any printable ascii chars
			case BP_GEN_RANDOM_ASCII_STRING_TYPE_UNSET:
				// generate a char.
				curr_char = rand() % 94 + 33;
				random_str[n] = curr_char;
				break;

			// generate only alphabetic strings caps or lower chars
			case BP_GEN_RANDOM_ASCII_STRING_TYPE_ALPHA_CAPS_OR_LOWER:

				// simple loop to generate a random that fits correctly
				do
				{
					// generate a char.
					curr_char = rand();

				} while( !(curr_char >= 'A' && curr_char <= 'Z') && !(curr_char >= 'a' && curr_char <= 'z' ) );

				// set the character after its validated
				random_str[n] = curr_char;
				break;

			// caps-only filter for alphanueric generation
			case BP_GEN_RANDOM_ASCII_STRING_TYPE_ALPHA_CAPS_ONLY:

				// simple loop to generate a random that fits correctly
				do
				{
					// generate a char.
					curr_char = rand();

				} while( !(curr_char >= 'A' && curr_char <= 'Z'));

				// set the character after its validated
				random_str[n] = curr_char;
				break;

			// filter for generating alphanum caps or lower data
			case BP_GEN_RANDOM_ASCII_STRING_TYPE_ALPHA_NUMERIC_CAPS_OR_LOWER:

				// simple loop to generate a random that fits correctly
				do
				{
					// generate a char.
					curr_char = rand();

				} while( !(curr_char >= 'A' && curr_char <= 'Z') && !(curr_char >= 'a' && curr_char <= 'z' ) && !(curr_char >= '0' && curr_char <= '9'));

				// set the character after its validated
				random_str[n] = curr_char;
				break;

			// filter for generating alphanumeric caps only data
			case BP_GEN_RANDOM_ASCII_STRING_TYPE_ALPHA_NUMERIC_CAPS_ONLY:

				// simple loop to generate a random that fits correctly
				do
				{
					// generate a char.
					curr_char = rand();

				} while( !(curr_char >= 'A' && curr_char <= 'Z') && !(curr_char >= '0' && curr_char <= '9'));

				// set the character after its validated
				random_str[n] = curr_char;
				break;

			// filter to generate only numbers only string data
			case BP_GEN_RANDOM_ASCII_STRING_TYPE_NUMERIC_ONLY:

				// simple loop to generate a random that fits correctly
				do
				{
					// generate a char.
					curr_char = rand();

				} while( !(curr_char >= '0' && curr_char <= '9'));

				// set the character after its validated
				random_str[n] = curr_char;

				break;

			// filter to only generate random printable ascii symbols >!@# etc
			case BP_GEN_RANDOM_ASCII_STRING_TYPE_SYMBOLS_ONLY:

				// simple loop to generate a random that fits correctly
				do
				{
					// generate a char.
					curr_char = rand() % 94 + 33;
				} while( (curr_char >= 'A' && curr_char <= 'Z') || (curr_char >= 'a' && curr_char <= 'z' ) || (curr_char >= '0' && curr_char <= '9'));


				// set the character after its validated
				random_str[n] = curr_char;

				break;

			// print error message and exit on unrecognized type
			default:
				printf("[!] Error, unable to generate random string due to bad type variable.");
				if(!tq)
					bpfree(random_str);
				else
					tq_free(tq, random_str);

				return NULL;
				break;

		}

	}

	// return the random string
	return random_str;

}

// randomizes a pointer array from an existing array
char ** BP_RandomizePointerArray(char **ptrs, size_t num_ptrs)
{

	// return null on failure
	if(!num_ptrs)
		return NULL;

	// create space for new pointers
	char ** new_ptrs = (char **) bpcalloc(sizeof(char *) *  (num_ptrs+10), 1);
	size_t * matches = (size_t *) bpcalloc(sizeof(size_t) * (num_ptrs+10), 1);

	// set current guess to zero
	size_t curr_guess = 0;

	// Seed srandom with some random data (better entropy required here I think).  Don't
	// use the method as a primary random generator, as it's slow.  It's fine for seeding
	// though.
	BP_SeedPNRG();
	// srandom(BP_GenRandomUnsignedLong(1, 0xffffffff));

	size_t i = 0;

	// this is the replacement index used for writing into
	// the new array.
	size_t j = 0;



	size_t num_already_matched = 0;
	size_t matched_in_loop = 0;
	size_t zero_position_added = 0;
	for(;;)
	{


		// check to ensure that our katch count isn't already hit
		if(num_already_matched == num_ptrs)
			break;

		/*
		 * The curr_guess below is a guessed array index.  It must fall within the range
		 * of the index range of ptrs (passed in argument).
		 */

		// create a different random number
		curr_guess = rand() % (num_ptrs);


		/* Use these statements to test limits.
		if(curr_guess == 1023)
			printf("\n Got 1023?");
		if(curr_guess == 0)
			printf("\n Got 0");
		if(curr_guess == 1024)
			printf("\n Got 1024");
		*/
		// printf("\n Got num?: %u", curr_guess);


		// The guess cannot escape the bounds of the array.
		// Since it's an index, subtract comparison by one to
		// compensate for displacement.
		if(curr_guess >= (num_ptrs))
			continue;



		// The following loop resolves the current guess down
		// to an indicator of whether the guess (aka index) exists
		// in the list of already stored guesses.  This prevents
		// duplicate entries from coming into existence.  The search
		// has to walk the entire list (including null entries) so that
		// this routine can function on arbitrary pointer buffers.
		size_t x = 0;
		matched_in_loop = 0;
		for(; x <= num_ptrs; x++ )
		{

			// this fixes a bug that caused the zero position
			// to fail to add (due to the calloc assigning all
			// positions to zero, and the comparison being zero);
			if(curr_guess == 0 && !zero_position_added)
			{
				matched_in_loop = 0;
				zero_position_added = 1;
				break;
			}

			if(curr_guess == matches[x])
			{
				matched_in_loop=1;
			}
		}

		// continue the loop if we already matched an entry
		if(matched_in_loop)
			continue;


		// ===================================================

		// increase number matched
		num_already_matched++;

		// save guess in matches array so its not reused
		matches[j] = curr_guess;

		// set prt in new position
		new_ptrs[curr_guess] = ptrs[j];


		if(ptrs[j] == NULL)
		{
			printf("\n Argument at location: %u is null", j);
		}
		// printf("\n Making Assignment from %u: %p", j, ptrs[j]);
		// move index up
		j++;

		// break loop if we fetched the correct number of ptr elements
		if(j == (num_ptrs-1))
			break;

	}

	// return new array
	return new_ptrs;

}

// randomizes a pointer array from an existing array
char ** BP_RandomizePointerArrayTQ(char **ptrs, size_t num_ptrs, P_BP_LINKL m)
{

	// return null on failure
	if(!num_ptrs)
		return NULL;

	// create space for new pointers
	char ** new_ptrs = (char **)  tq_calloc(m, sizeof(char *) *  (num_ptrs+10), 1, BPLN);
	size_t * matches = (size_t *) tq_calloc(m, sizeof(size_t) * (num_ptrs+10), 1, BPLN);

	// set current guess to zero
	size_t curr_guess = 0;

	// Seed srandom with some random data (better entropy required here I think).  Don't
	// use the method as a primary random generator, as it's slow.  It's fine for seeding
	// though.
	BP_SeedPNRG();
	// srandom(BP_GenRandomUnsignedLong(1, 0xffffffff));

	size_t i = 0;

	// this is the replacement index used for writing into
	// the new array.
	size_t j = 0;



	size_t num_already_matched = 0;
	size_t matched_in_loop = 0;
	size_t zero_position_added = 0;
	for(;;)
	{


		// check to ensure that our katch count isn't already hit
		if(num_already_matched == num_ptrs)
			break;

		/*
		 * The curr_guess below is a guessed array index.  It must fall within the range
		 * of the index range of ptrs (passed in argument).
		 */

		// create a different random number
		curr_guess = rand() % (num_ptrs);


		/* Use these statements to test limits.
		if(curr_guess == 1023)
			printf("\n Got 1023?");
		if(curr_guess == 0)
			printf("\n Got 0");
		if(curr_guess == 1024)
			printf("\n Got 1024");
		*/
		// printf("\n Got num?: %u", curr_guess);


		// The guess cannot escape the bounds of the array.
		// Since it's an index, subtract comparison by one to
		// compensate for displacement.
		if(curr_guess >= (num_ptrs))
			continue;



		// The following loop resolves the current guess down
		// to an indicator of whether the guess (aka index) exists
		// in the list of already stored guesses.  This prevents
		// duplicate entries from coming into existence.  The search
		// has to walk the entire list (including null entries) so that
		// this routine can function on arbitrary pointer buffers.
		size_t x = 0;
		matched_in_loop = 0;
		for(; x <= num_ptrs; x++ )
		{

			// this fixes a bug that caused the zero position
			// to fail to add (due to the calloc assigning all
			// positions to zero, and the comparison being zero);
			if(curr_guess == 0 && !zero_position_added)
			{
				matched_in_loop = 0;
				zero_position_added = 1;
				break;
			}

			if(curr_guess == matches[x])
			{
				matched_in_loop=1;
			}
		}

		// continue the loop if we already matched an entry
		if(matched_in_loop)
			continue;


		// ===================================================

		// increase number matched
		num_already_matched++;

		// save guess in matches array so its not reused
		matches[j] = curr_guess;

		// set prt in new position
		new_ptrs[curr_guess] = ptrs[j];


		if(ptrs[j] == NULL)
		{
			printf("\n Argument at location: %u is null", j);
		}
		// printf("\n Making Assignment from %u: %p", j, ptrs[j]);
		// move index up
		j++;

		// break loop if we fetched the correct number of ptr elements
		if(j == (num_ptrs-1))
			break;

	}

	// return new array
	return new_ptrs;

}


// Explodes a buffer provided and returns an allocated array of allocated pointers (must free entire array
// or call BP_DestroyPointerArray on the array itself).  This routine is not binary safe.
char ** BP_ExplodeBuffer(char *buffer, char *delim_string)
{

	// ensure we have a buffer and a delim
	if(!buffer)
		return NULL;
	if(!delim_string)
		return NULL;

	if(!BP_StringIsReasonablePrintableString(buffer, bpstrlen(buffer), BP_TRUE, BP_TRUE))
		return NULL;

	if(!BP_StringIsReasonablePrintableString(delim_string, bpstrlen(delim_string), BP_TRUE, BP_TRUE))
		return NULL;

	// attempt to explode a buffer (large natural limit)
	return BP_ExplodeBufferBin(buffer, bpstrlen(buffer), delim_string, bpstrlen(delim_string), BP_KILOBYTE * BP_KILOBYTE);

}

// Explodes a buffer provided and returns an allocated array of allocated pointers (must free entire array
// or call BP_DestroyPointerArray on the array itself).  The maximum elements parameter determines exactly
// how many extractions will be done.  Setting this parameter to 0 allows for infinite extractions.
// This routine is binary safe.
char ** BP_ExplodeBufferBin(char *buffer, size_t buffer_len, char *delim_string, size_t delim_len, size_t max_elements)
{

	// Ensure all parameters are set before continuing. All values must
	// be non-zero.
	if(!buffer || !buffer_len || !delim_string || !delim_len)
		return NULL;


	// get deliminator count
	size_t delim_count = BP_CountTokensInBuffer(buffer, buffer_len, delim_string, delim_len);

	// if there were no tokens found, return with a zero pointer
	if(!delim_count)
	{
		return NULL;
	}

	// allocate zeroed space for the array based on the token count
	char ** split_array = (char **) bpcalloc
	(
		(sizeof(char *) * (delim_count + 1))+(sizeof(char*) * 5),
		1
	);

	// Set the start read position for buffer extraction
	char * start_position = buffer;

	// Set the end read position for buffer extraction as null.  It will be set upon
	// extraction determination in loop logic below.
	char * end_position = NULL;

	// this is used to count the number of extractions made from a buffer
	size_t total_extractions = 0;

	// flag indicating whether or not we're running our first read (aka from the start of the buffer)
	size_t is_first_read = BP_TRUE;

	// read length
	size_t read_len = 0;

	// Iterate through the tokens
	size_t n = 0;
	for(; n < buffer_len; n++)
	{


		// If the buffer length remaining is less than the token length
		// then there can be no more comparison matching.
		if((buffer_len - n) < delim_len)
		{
			break;
		}

		// since we have a proper sized buffer to match from, we can
		// run the compare here.
		if(memcmp(&buffer[n], delim_string, delim_len) == 0)
		{

			// set the end read position
			end_position = &buffer[n];

			// determine the read length by iterating between start and end (BIG/LITTLE ENDIAN safe)
			for(read_len = 0; &start_position[read_len] != end_position; read_len++){}

			// create space for the new buffer (this includes a single byte for 0x00
			// in the case that it's an empty string)
			split_array[total_extractions] = (char *) bpcalloc(read_len+1, 1);

			// if the read length is greater than zero, we must copy in the string
			if(read_len > 0)
			{

				// copy in the string
				memcpy(split_array[total_extractions], start_position, read_len);

			}

			// reset the start position
			start_position = (char *) &buffer[n+delim_len];

			// determine if we have a empty end delmited value (aka: value,value,value,)
			if(start_position[0] == (char) 0x00)
			{

				// create space for empty string
				split_array[total_extractions+1] = (char *) bpcalloc(1, 1);
				break;

			}

			// move past the token (minus one because the loop increments
			// the value)
			n += ( delim_len - 1 );

			// increment the number of instances discovered
			total_extractions++;


			// ensure that the fread flag is now set to false
			is_first_read = BP_FALSE;

			// check if we've hit the max number of extractions
			if(max_elements)
			if(total_extractions == max_elements)
				break;

		}

	}

	// add remainder if necessary
	if(start_position[0] != (char) 0x00)
	{


		// at this point we're at the last extraction element and may have to append a remainder
		// string in the last array place.
		for(read_len = 0; start_position[read_len]; read_len++){}

		// allocate space for the remainder string
		split_array[total_extractions] = (char *) bpcalloc(read_len+1, 1);

		// if we have a remainder, copy it in here
		if(read_len)
		{

			// copy in the string
			memcpy(split_array[total_extractions], start_position, read_len);

		}

	}

	// return the array
	return split_array;

}

// Splits a string along the first instance of the provided token, returns two allocated
// strings, within a char ** or NULL if there was no split performed (aka character
// does not exist).  Both the char ** and the individual char * within the array must
// be freed.  You can also call BP_DestroyPointerArray to destroy all allocated memory
// quickly and efficiently without having to write your own logic.  This routine is not binary
// safe.
char ** BP_SplitBuffer(char *buffer, char *delim_string)
{

	if(!buffer)
		return NULL;
	if(!delim_string)
		return NULL;
	if(!BP_StringIsReasonablePrintableString(buffer, bpstrlen(buffer), BP_TRUE, BP_TRUE))
		return NULL;
	if(!BP_StringIsReasonablePrintableString(delim_string, bpstrlen(delim_string), BP_TRUE, BP_TRUE))
		return NULL;

	// split using binary split version
	return BP_SplitBufferBin(buffer, bpstrlen(buffer), delim_string, bpstrlen(delim_string));

}


// Splits a string along the first instance of the provided token, returns two allocated
// strings, within a char ** or NULL if there was no split performed (aka character
// does not exist).  Both the char ** and the individual char * within the array must
// be freed.  You can also call BP_DestroyPointerArray to destroy all allocated memory
// quickly and efficiently without having to write your own logic.  This routine is binary
// safe.
char ** BP_SplitBufferBin(char *buffer, size_t buffer_len, char *delim_string, size_t delim_len)
{


	// Ensure all parameters are set before continuing. All values must
	// be non-zero.
	if(!buffer || !buffer_len || !delim_string || !delim_len)
		return NULL;


	// get deliminator count
	size_t delim_count = BP_CountTokensInBuffer(buffer, buffer_len, delim_string, delim_len);

	// this will contain an array of 2 pointers trailing with a NULL ptr
	// terminating the array if the split was successful.
	char ** split_array = (char **) NULL;

	// if there were no tokens found, return with a zero pointer
	if(!delim_count)
		return NULL;

	// front part of the string
	char *front_part = (char *) NULL;
	char *back_part  = (char *) NULL;

	// stores the length of the back part in bytes
	size_t back_part_len = 0;

	// walk the buffer, find the delimiter, and return the strings
	size_t n = 0;
	for(; n < buffer_len; n++)
	{

		// If the buffer length remaining is less than the token length
		// then there can be no more comparison matching.
		if((buffer_len - n) < delim_len)
		{
			break;
		}

		// since we have a proper sized buffer to match from, we can
		// run the compare here.
		if(memcmp(&buffer[n], delim_string, delim_len) == 0)
		{

			// allocate space for the front part
			front_part = (char *) bpcalloc(n+1, 1);

			// calculate the length of the back part
			back_part_len = buffer_len - (n+delim_len);

			// set the end part here
			back_part = (char *) bpcalloc(back_part_len+1, 1);

			// if we have a read length from start, we don't have an empty string
			// so we can now copy in data.
			if(n != 0)
			{

				// copy in the data
				memcpy(front_part, buffer, n);

			}

			// copy in the back part if not an empty string
			if(back_part_len)
			{

				// copy in the data
				memcpy(back_part, &buffer[n+delim_len], back_part_len);

			}

			// break the loop
			if(front_part || back_part)
				break;

		}

	}

	// check if we have a failed assignment on either side (shouldn't ever happen)
	if(!back_part || !front_part)
	{

		if(back_part)
			bpfree(back_part);
		if(front_part)
			bpfree(front_part);

		// return indicating failure via NULL
		return NULL;

	}


	// allocate and zero space to store the strings
	split_array = (char **) bpcalloc(sizeof(char *) * 3, 1);

	// ensure the array was allocated ok
	if(!split_array)
		return NULL;

	// set the front part
	split_array[0] = front_part;

	// set the back part
	split_array[1] = back_part;


	// return the split data
	return split_array;

}

// Counts the number of pointers in a null pointer terminated pointer array.  Set
// the max_count parameter to zero to allow for uncapped matches.
size_t BP_CountNumberOfPointersInPointerArray(void ** ptrs, size_t max_count)
{

	// ensure that we have a pointer
	if(!ptrs)
		return 0;

	// return zero if the first pointer is null
	if(!ptrs[0])
		return 0;

	// set pointer count
	size_t pointer_count = 0;

	// walk the pointers
	size_t n = 0;
	for(; ptrs[pointer_count]; pointer_count++)
	{

		// break the loop if we hit the top match count (if set)
		if(max_count)
		if(pointer_count == max_count)
			break;

	}

	// return the number of pointers found within the array
	return pointer_count;

}

// returns a heap allocated set of size_t entries corresponding to each individual string in
// the ptrs array.  This is primarily used for linking in existing string arrays to a tailqueue
// allocator.
size_t * BP_CountStringLengthsInStringPointerArrayAndReturnValues(char ** ptrs, size_t max_count, size_t max_line_len)
{

	// ensure parameters are set
	if(!ptrs || !max_count)
		return NULL;

	// pointer which is returned
	size_t * ret_ptrs = (size_t *) NULL;

	size_t n = 0;
	for(; ptrs[n]; n++)
	{

		// reallocate space
		ret_ptrs = (size_t *) bprealloc(ret_ptrs, sizeof(size_t *) * (n+1));

		// ensure we've reallocated ok
		if(!ret_ptrs)
			return NULL;

		// retrieve string length
		ret_ptrs[n] = strnlen(ptrs[n], max_line_len);

	}

	// return the pointer array
	return ret_ptrs;

}





// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% String Column-ization (splits strings into arrays of columns) %%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Returns a stack of strings split along a column_width boundary.  The array is NULL pointer
// terminated.  This can be freed with the BP_DestroyPointerArray routine.  If the generation
// of string data fails for whatever reason, this routine returns NULL.
P_BP_GENERIC_STRING_COLUMNIZATION BP_SplitStringIntoEqualWidthColumns
(
		char * str_to_split,    /* the string you wish to split into columns */
		size_t str_length,      /* the length of the string provided */
		size_t column_width,    /* the total width you'd like the columns to be*/
		BP_BOOL hyphenate,      /* if you want words to be hyphenated along truncated lines, set to BP_TRUE, else BP_FALSE*/
		size_t empty_pad_lines,  /* you can specify if you want to pad the buffer with empty column_width lines here.  If not, set to zero. */
		P_BP_COLOR_CODE_SET_T * colors, /* if this is passed in, each color will be applied to the non-padded (re: string) buffer */
		P_BP_LINKL tq           /* if you want to use a tailqueue allocator, you can set one to use here */
)
{

	// ensure we have a string, and a length
	if(!str_to_split || !str_length)
		return NULL;

	// ensure we have a column width
	if(!column_width)
		return NULL;


	// this will hold the columns which are being returned to the user
	char ** columns = (char **) NULL;

	// calculate the total number of allocations
	size_t total_string_allocations = (str_length / column_width) + empty_pad_lines;

	// add extra pad line if we have a remainder
	if(str_length % column_width)
		total_string_allocations++;

	// allocate the base columns array (+1 for null terminator)
	columns = (char **) bpcalloc
	(
		(total_string_allocations + 1) * sizeof(char *),
		1
	);

	// ensure array was created
	if(!columns)
		return NULL;

	// create space for normalization data
	size_t * space_normalization_data = (size_t *) bpcalloc(sizeof(size_t) * (total_string_allocations+1), 1);
	if(!space_normalization_data)
	{
		bpfree(columns);
		return NULL;
	}

	// now create space for all the columns
	size_t n = 0;
	for(; n < total_string_allocations; n++)
	{

		// allocate space here
		columns[n] = (char *) bpcalloc(column_width+2, 1);

		// if the space wasn't allocated ok, destroy what was allocated
		// and return null.
		if(!columns[n])
		{
			BP_DestroyPointerArray((void **) columns, n);
			bpfree(columns);
			return NULL;
		}

		// memset(columns[n], 0x00, column_width);

	}



	// now that all space has been allocated, we can walk through the string
	// and create the columnized buffer.
	size_t curr_position = 0;
	size_t curr_buffer_idx = 0;
	n=0;

	// This holds an array of indicators, showing which lines have been hyphenated.  It's
	// reallocated/allocated every time a line is hypenated, and will contain the line number (not
	// the index number) of the line which was hyphenated.  This means you have to subtract 1 from
	// the hyphenated indicator in order to get the correct index for comparison.
	size_t * hyphenated_lines = NULL;
	size_t   current_hyphenated_line_count = 0;


	// longest width and colorized data
	size_t longest_width           = 0;
	size_t longest_width_colorized = 0;

	for(; n < str_length; n++)
	{

		// check width constraints and move to next index if necessary.  Also
		// hyphenate if necessary.
		if(curr_position == column_width)
		{

			// check to see if hyphenation is required.  The dual checks are to
			// verify if truncation is needed.
			if(str_to_split[n-1] != (char) 0x20)
			{

				if(str_to_split[n] != (char) 0x20)
				if(hyphenate == BP_TRUE)
				{

					// increment the number of hyphenated lines
					current_hyphenated_line_count++;

					// reallocate space for the hyphenated lines
					hyphenated_lines = (size_t *) bprealloc(hyphenated_lines, sizeof(size_t) * (current_hyphenated_line_count+1));

					// set the index here
					hyphenated_lines[current_hyphenated_line_count-1]   = curr_buffer_idx+1;
					// printf("\n Line: %u is hyphenated", curr_buffer_idx);

					// terminate
					hyphenated_lines[current_hyphenated_line_count] = 0;

					// finally, perform hyphenation
					columns[curr_buffer_idx][curr_position] = '-';

				}

			}

			// reset the buffer write position
			curr_position = 0;

			// move to the next buffer index in the char ** array
			curr_buffer_idx++;

		}

		// assign the buffer here
		columns[curr_buffer_idx][curr_position] = str_to_split[n];

		// increment the inner length check
		curr_position++;

	}

	// iterator used to add space padding to lines as necessary
	size_t m = 0;


	size_t curr_width = 0;
	size_t add_spaces = 0;
	for(n = 0; n < total_string_allocations; n++)
	{


		// set current width
		curr_width = bpstrlen(columns[n]);

		// number of spaces to add
		add_spaces = (column_width+1) - curr_width;

		// add normalization data
		space_normalization_data[n] = add_spaces;

		// printf("\n Got Add Line Count: %u", space_normalization_data[n]);
		// printf("[%s]   - %u \n",columns[n], space_normalization_data[n]);

	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Begin Colorization Logic %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



	// this flag is set of normalization (space padding) was done previously
	BP_BOOL normalized_via_color_encoding = BP_FALSE;

	// this array stores the difference between colorized and non-colorized lines
	size_t * color_normalization_data = NULL;

	// these are used in the case that a color pointer is unprovided (aka a byte
	// is uncolored)
	BP_COLOR_CODE_SET_T no_colors;
	BP_ZERO_STACK(no_colors);

	// ensure we have colors and columns
	if(colors && columns)
	if(columns[0] && colors[0])
	{



		// first calculate columns, colors, and widest column
		size_t   color_count       = 0;
		size_t   column_count      = 0;
		size_t   widest_column     = 0;
		size_t * old_lengths       = NULL;
		size_t * colorized_lengths = NULL;

		// count columns (null ptr terminated array)
		for(n = 0; columns[n]; n++)
			column_count++;

		// count colors (null ptr terminated array)
		for(n = 0; colors[n]; n++)
			color_count++;


		// allocate space for old lengths
		old_lengths = (size_t *) bpcalloc(sizeof(size_t) * (column_count + 1), 1);
		if(!old_lengths)
			return NULL;

		// allocate space for colorized lengths
		colorized_lengths = (size_t *) bpcalloc(sizeof(size_t) * (column_count +1), 1);
		if(!colorized_lengths)
		{
			bpfree(old_lengths);
			return NULL;
		}

		// length iter to prevent redundant bpstrlen() calculations below (saves cpu)
		size_t length_iter = 0;

		// calculate length data
		for(n = 0; columns[n]; n++)
		{

			// calculate the length
			length_iter = bpstrlen(columns[n]);

			if(length_iter > widest_column)
				widest_column = length_iter;

			// set the length iter
			old_lengths[n] = length_iter;

		}

		// this index is used for creating color slices
		size_t j = 0;

		// this is the hyphenation calculation index (used to determine
		// if a line was hyphenated)
		size_t i = 0;

		// create colorized column array
		char ** colorized_columns = NULL;

		// this represents the current index into the color array, which is used
		// for correctly slicing the array for the colorization routine.
		size_t current_color_index = 0;

		// since we got this far, we can allocate the color normalization data
		color_normalization_data = (size_t *) bpcalloc(sizeof(size_t) * (column_count+1), 1);

		// ensure we have columns to process
		if(column_count)
		{

			// allocate zeroed memory to store the new colors
			colorized_columns = (char **) bpcalloc(sizeof(char *) * (column_count+1), 1);

			// set indexed colors to null
			P_BP_COLOR_CODE_SET_T * indexed_colors = NULL;

			// walk each column, and generate colorization data
			for(n = 0; n < column_count; n++)
			{

				if(!columns[n])
				{
					if(indexed_colors)
					{
						bpfree(indexed_colors);
						indexed_colors = NULL;
					}
					continue;
				}

				// since the colorization routine demands only a slice of the colors provided
				// go ahead and create that slice.
				indexed_colors = (P_BP_COLOR_CODE_SET_T*) bpcalloc(sizeof(P_BP_COLOR_CODE_SET_T *) * (old_lengths[n]+1), 1);

				// index the slice with the values provided
				for(j = 0; j < old_lengths[n]; j++)
				{

					if(!colors[current_color_index])
					{

						// set generic no colors
						indexed_colors[j] = &no_colors;

					}
					else
					{

						// set provided colors
						indexed_colors[j] = colors[current_color_index];

					}
					current_color_index++;

				}

				// now, check if the hyphenation occurred on this line, if so, we have to
				// decrement the color index (to account for the hyphenation byte within the color
				// array)
				if(hyphenated_lines)
				for(i = 0; hyphenated_lines[i]; i++)
				{
					if(hyphenated_lines[i] == (n+1))
					{
						// printf("\n Found a Hyphenated Line: %s", columns[n]);

						// decrement color index with regard to the hyphen
						current_color_index--;
					}
				}

				// printf("\n Got Current: %s - %u", columns[n], old_lengths[n]);



				// set column length
				colorized_columns[n] = BP_ColorizeString(columns[n], old_lengths[n], indexed_colors);



				// if there was no column data, set column as a duplicate string
				if(!colorized_columns[n])
				{
					if(indexed_colors)
					{
						bpfree(indexed_colors);
						indexed_colors = NULL;
					}
					continue;
				}

				// now set normalization data
				color_normalization_data[n] = bpstrlen(colorized_columns[n]) - bpstrlen(columns[n]);


				// set the colorized lengths
				colorized_lengths[n] = bpstrlen(colorized_columns[n]);

				// destroy the indexed colors
				bpfree(indexed_colors);
				indexed_colors = NULL;

			}

			// destroy colors if they exist
			if(indexed_colors)
			{
				bpfree(indexed_colors);
				indexed_colors = NULL;
			}

		}

		// holds the line distance calculations
		size_t line_difference = 0;
		size_t new_length      = 0;

		// new column with padding
		char * new_column_with_padding = NULL;



		// walk columns
		for(n = 0; colorized_columns[n]; n++)
		{

			// calculate the line difference
			line_difference = colorized_lengths[n] - old_lengths[n];

			if(old_lengths[n] > longest_width)
				longest_width = old_lengths[n];


			// allocate new space
			new_column_with_padding = (char *) bpcalloc(line_difference+widest_column+10, 1);
			if(!new_column_with_padding)
			{

				// destroy the pointer array
				BP_DestroyPointerArrayNullTerminated((void **) colorized_columns);

				// destroy the colorized columns
				bpfree(colorized_columns);

				// exit the loop
				break;

			}

			// space-ify the entire column
			memset(new_column_with_padding, 0x20, line_difference+widest_column);

			// set space normalization data
			space_normalization_data[n] = (line_difference+widest_column)-bpstrlen(colorized_columns[n]);
			normalized_via_color_encoding = BP_TRUE;

			// copy in the colorized data
			memcpy(new_column_with_padding,colorized_columns[n], bpstrlen(colorized_columns[n]));

			// destroy the old colorized column
			bpfree(colorized_columns[n]);

			// destroy the old non-colorized column (so we can sub our new colorized
			// buffer in its place)
			bpfree(columns[n]);

			// set the new columns value
			columns[n] = new_column_with_padding;

			// set longest width colorized here
			if(bpstrlen(columns[n]) > longest_width_colorized)
				longest_width_colorized = bpstrlen(columns[n]);

			// printf("\n New Length?: %u - %s", bpstrlen(columns[n]), columns[n]);

		}

		// now recalculate
		// printf("\n Got Widest Column: %u", widest_column);


		if(old_lengths)
			bpfree(old_lengths);
		if(colorized_lengths)
			bpfree(colorized_lengths);
		if(colorized_columns)
			bpfree(colorized_columns);

	}


	// create return structure
	P_BP_GENERIC_STRING_COLUMNIZATION ret_val = (P_BP_GENERIC_STRING_COLUMNIZATION) bpcalloc(sizeof(BP_GENERIC_STRING_COLUMNIZATION), 1);
	ret_val->columns                  = columns;
	ret_val->space_normalization_data = space_normalization_data;
	ret_val->column_count             = total_string_allocations;
	ret_val->color_normalization_data = color_normalization_data;

	size_t idx = 0;
	// normalize uncolored buffers here if necessary
	if(normalized_via_color_encoding == BP_TRUE && empty_pad_lines)
	{

		idx = total_string_allocations - empty_pad_lines;
		for(n = 0; n < empty_pad_lines; n++)
		{

			if(ret_val->columns[idx])
				bpfree(ret_val->columns[idx]);

			ret_val->columns[idx] = (char *) bpcalloc(column_width+10, 1);

			// add extra space if we've hyphenated a line (to account for the hypen added)
			if(hyphenated_lines)
				memset(ret_val->columns[idx], 0x20, column_width+1);
			else
				memset(ret_val->columns[idx], 0x20, column_width);

			// increase the index if necessary
			idx++;

		}

		for(n = 0; ret_val->columns[n]; n++)
		{
			// set longest width here
			if(bpstrlen(ret_val->columns[n]) > longest_width_colorized)
				longest_width_colorized = bpstrlen(ret_val->columns[n]);
		}

	}


	// normalize non-colorized data here
	size_t old_len = 0;
	if(normalized_via_color_encoding == BP_FALSE)
	{

		for(n = 0; n < total_string_allocations; n++)
		{

			old_len = bpstrlen(ret_val->columns[n]);
			for(m = 0; m < ret_val->space_normalization_data[n]; m++ )
				strcat(ret_val->columns[n], " ");

			// set longest width here
			if(bpstrlen(ret_val->columns[n]) > longest_width)
				longest_width = bpstrlen(ret_val->columns[n]);

		}

	}

	// set longest width here
	ret_val->longest_width           = longest_width;
	ret_val->longest_width_colorized = longest_width_colorized;

	// display alloc if desired
	/*
	for(n = 0; n < total_string_allocations; n++)
	{
		printf("\n [%s] %p FINAL -- %u - %u - %u", ret_val->columns[n], ret_val->columns[n], empty_pad_lines, ret_val->space_normalization_data[n], bpstrlen(ret_val->columns[n]));
	}
	*/

	if(hyphenated_lines)
		bpfree(hyphenated_lines);

	// if a tailqueue was passed in, return it here as a tq allocated element
	if(tq) return BP_StoreColumnizationInTailQ(ret_val, tq, BP_TRUE);

	// return the columns
	return ret_val;

}


// Returns a stack of equal width columns split along a character boundary.  Aka
// this will take (for example) an ASCII art file, newline delimited, and measure
// each line.  It will then create a set of columns, with the longest line being
// the column width, and copy every line into the newly created columns.  If a
// stack of color set pointers is passed in here, each character will get painted
// with the resultant colors.  However, doing this means that the bytewidth of
// each column will not be equidistant.  They will be

P_BP_GENERIC_STRING_COLUMNIZATION BP_SplitStringIntoEqualWidthColumnsByDeliniation
(
	char * str_to_split,
	size_t str_len,
	unsigned char deliniation_char,
	size_t min_length_of_columns, /* this parameter can be set to a non-zero value to ensure columns are at a min length */
	P_BP_COLOR_CODE_SET_T * colors,
	P_BP_LINKL tq                 /* if you want to use a tailqueue allocator, you can set one to use here */
)
{

	// ensure we have a string, and a length
	if(!str_to_split || !str_len)
		return NULL;

	// deliniation char cant be NULL
	if(!deliniation_char)
		return NULL;

	// return var
	char ** columns = (char **) NULL;

	// retrieve the number of lines in the buffer
	size_t total_lines = BP_GetNumberOfLinesInBuffer(str_to_split, str_len, deliniation_char);

	// set the longest line here
	size_t longest_line = BP_GetLongestLineInBuffer(str_to_split, str_len, deliniation_char);

	// ensure we have total lines
	if(!total_lines || !longest_line)
	{
		return NULL;
	}

	// attempt to allocate the columns
	columns = (char **) bpcalloc(sizeof(char *) * (total_lines + 1), 1);

	// ensure we have columns
	if(!columns)
		return NULL;

	// create space for normalization data
	size_t * space_normalization_data = (size_t *) bpcalloc(sizeof(size_t) * (total_lines+1), 1);
	if(!space_normalization_data)
	{
		bpfree(columns);
		return NULL;
	}



	// This is the index used by the colorization algorithm below.  Defined here
	// for clarity but it doesn't get used till you get into the colorization logic
	// below.
	size_t j = 0;

	// final length
	size_t final_length = 0;

	// set the official final line length here
	if(min_length_of_columns > longest_line)
		final_length = min_length_of_columns;
	else
		final_length = longest_line;

	// allocate space
	size_t n = 0;
	for(; n < total_lines; n++)
	{


		columns[n] = (char *) bpcalloc(final_length + 1, 1);

		// if the allocation failed, destroy and exit
		if(!columns[n])
		{
			BP_DestroyPointerArray((void **) columns, n);
			bpfree(columns);
			return NULL;
		}

		memset(columns[n], 0x20, final_length);

	}

	// create separator as stack data
	char separator[2];
	BP_ZERO_STACK(separator);

	// set the deliniator
	separator[0] = deliniation_char;

	// now explode the data along deliniation, and copy data into columns
	char ** exploded = BP_ExplodeBufferBin(str_to_split, str_len, (char*) &separator, 1, BP_GIGABYTE);

	// destroy existing columns and return null if we couldn't allocate ok
	if(!exploded)
	{
		BP_DestroyPointerArray((void **) columns, n);
		bpfree(columns);
		return NULL;
	}

	// copy length
	size_t copy_len = 0;

	// current number of padding at end of the line
	size_t curr_pad = 0;

	// longest width and colorized data
	size_t longest_width           = 0;
	size_t longest_width_colorized = 0;

	// this holds color normalization data for the individual lines
	size_t * color_normalization_data = NULL;

	// walk the exploded data and copy in data
	if(!colors)
	{

		for(n = 0; exploded[n]; n++)
		{

			// gather copy length (always ok)
			copy_len = bpstrlen(exploded[n]);

			// copy in data, overwriting spaces where necessary
			memcpy(columns[n], exploded[n], copy_len);

			// space_normalization_data
			curr_pad = final_length - copy_len;

			// set space normalization here
			space_normalization_data[n] = curr_pad;

			if(bpstrlen(columns[n]) > longest_width)
				longest_width = bpstrlen(columns[n]);

			// printf("\n Normalization: %u", space_normalization_data[n]);
		}


	}
	else
	{

		// calculate longest non-cololorized with before
		// we start colorizing anything.
		for(n = 0; exploded[n]; n++)
		{
			if(bpstrlen(columns[n]) > longest_width)
				longest_width = bpstrlen(columns[n]);
		}


		/* BEGIN COLORIZTION LOGIC
		 *
		 * DEVELOPER NOTE::DEVELOPER NOTE::DEVELOPER NOTE::DEVELOPER NOTE::DEVELOPER NOTE
		 *
		 *     Colorization is fairly convoluted as it requires multi-byte color codes
		 *     embedded into a string for the colors to register.  For this purpose, we
		 *     must do a significant amount of recolorization here.  The reason we cannot
		 *     just one-to-one is because when breaking up columns, there is a possibility
		 *     of breaking a line along a boundary which is in the middle of a color code.  If
		 *     this occurs, the color code isn't recognized by the terminal and just becomes
		 *     invalid 0x1b (color indicator) output.  This code looks horrible, but it's
		 *     absolutely necessary for accomplishing what's desired.
		 *
		 * DEVELOPER NOTE::DEVELOPER NOTE::DEVELOPER NOTE::DEVELOPER NOTE::DEVELOPER NOTE
		 *
		 */

		// Holds color differential (normalization) data used for storing
		// the differences between colorized and uncolorized data (stored in
		// the returned structure).  Use N as our count since it's calculated
		// directly above.
		color_normalization_data = (size_t *) bpcalloc(sizeof(size_t) * n+1, 1);

		// Holds the colorized string which is re-indexed into the exploded array.
		// When this is re-indexed, the previous index is destroyed (freed), and this pointer
		// takes its spot.
		char *colorized_string = NULL;

		// Line length is used to index into the parameters passed in, and select correct
		// indexes for colorization.
		size_t line_len = 0;

		// this pointer will hold the old value of exploded[n], and gets free'd
		// on loop.
		char * old_pointer = NULL;

		// Color indexes.  These represent the depth into the colors (routine parameter)
		// array we're currently at, as well as the top of the index.
		size_t current_color_idx = 0;
		size_t current_colors_selected = 0;
		size_t max_color_idx     = 0;
		size_t max_colors_selected = 0;

		// the max color index will be the length of the string, as
		// each entry in the string should have a color associated with it.
		max_color_idx       = str_len;

		// the maximum colors selected will be the total number of indexes
		// minus the exploded line count.  This is because we are not colorizing
		// new line entries. (we skip over colorizing them so that only text blocks
		// are colorized and not entire lines)
		max_colors_selected = str_len - total_lines;

		// color stack, created on loop to feature references to the
		// actual colors (routine parameter values)
		P_BP_COLOR_CODE_SET_T * color_stack = NULL;

		// Colorized exploded array.  This is absolutely necessary because
		// we use it to calculate the difference (in bytes) between the colorized
		// and non-colorized exploded elements.  The difference allows us
		// to calculate a correct uniform set of returnable entries.
		char **colorized_exploded = (char **) bpcalloc(sizeof(char *) * (total_lines+1), 1);


		// Walk each line in the exploded[] buffer, process each entry
		// into a colorized entry, and destroy the old pointer.
		for(n = 0; n < total_lines; n++)
		{

			// ensure we have a pointer
			if(!exploded[n])
				break;

			// calculate line length
			line_len = bpstrlen(exploded[n]);
			if(line_len)
			{

				// allocate a null terminated color stack to pass to BP_ColorizeString
				// (necessary for colorization to take place)
				color_stack = (P_BP_COLOR_CODE_SET_T *) bpcalloc(sizeof(P_BP_COLOR_CODE_SET_T *) * (line_len + 1), 1);

				// prepare color_stack from indexed colors
				for(j = 0; j < line_len; j++)
				{
					color_stack[j] = colors[current_color_idx];
					current_color_idx++;
				}

				// colorize the string
				colorized_string = BP_ColorizeString(exploded[n], line_len, color_stack, BP_TRUE);

				// plumb in the new line with the new colorized string
				colorized_exploded[n] = colorized_string;

				// set normalization data
				color_normalization_data[n] = bpstrlen(colorized_exploded[n]) - bpstrlen(columns[n]);


			}
			else
			{
				colorized_exploded[n] = bpstrdup(exploded[n]);
			}

			// skip over the color index if we have a next line
			if(exploded[n+1])
				current_color_idx++;


			// destroy color stack on loop
			if(color_stack)
			{
				bpfree(color_stack);
				color_stack = NULL;
			}

		}

		// now that the exploded array has been colorized at this point, we can
		// recalculate the line lengths necessary to store the extra bytes, and
		// store them as necessary.

		size_t column_line_len       = 0;
		size_t exploded_line_len     = 0;
		size_t colorized_line_len    = 0;
		size_t new_column_line_len   = 0;
		size_t total_bytes_added_by_colorization = 0;
		size_t remainder_for_padding = 0;

		// walk all lines, calculate, and reallocate space
		for(n = 0; n < total_lines; n++)
		{

			// all three values must be set
			if(!columns[n] || !exploded[n] || !colorized_exploded[n])
				break;

			column_line_len    = bpstrlen(columns[n]);
			exploded_line_len  = bpstrlen(exploded[n]);
			colorized_line_len = bpstrlen(colorized_exploded[n]);

			// this should always be the case, since colorization always
			// adds data to the line.
			if(colorized_line_len > exploded_line_len)
			{

				// calculate the total bytes added (difference)
				total_bytes_added_by_colorization = colorized_line_len - exploded_line_len;

				// set the new column length
				new_column_line_len = final_length+total_bytes_added_by_colorization;

				// remainder bytes for padding
				remainder_for_padding = final_length - exploded_line_len;

				// printf("\n Rem: %u - %u - %u - %u", remainder_for_padding, exploded_line_len,  final_length, new_column_line_len);

				// realloc
				columns[n] = (char *) bprealloc(columns[n], new_column_line_len+1);

				// break the loop if we can't reallocate memory for whatever reason (catestrophy)
				if(!columns[n])
					break;

				// set the entire buffer to space characters (0x20)
				memset(columns[n], 0x20, new_column_line_len);

				// null terminate the new allocation (don't +1 or you'll hit a bad index)
				columns[n][new_column_line_len] = 0x00;

				// finally copy in colorized line
				memcpy(columns[n], colorized_exploded[n], bpstrlen(colorized_exploded[n]));

				if(bpstrlen(columns[n]) > longest_width_colorized)
					longest_width_colorized = bpstrlen(columns[n]);

				// printf("\n New Column?: [%s]", columns[n]);

				// calculate normalization here
				space_normalization_data[n] = final_length - exploded_line_len;

				if(color_normalization_data)
				if(color_normalization_data[n])
					color_normalization_data[n] += space_normalization_data[n];

			}


			// color_normalization_data[n] = color_normalization_data[n] - space_normalization_data[n];

			// reset lengths on loop
			colorized_line_len                = 0;
			exploded_line_len                 = 0;
			new_column_line_len               = 0;
			total_bytes_added_by_colorization = 0;


		}

		// destroy colorized data if necessary
		if(colorized_exploded)
		{
			BP_DestroyPointerArray((void **) colorized_exploded, total_lines);
			bpfree(colorized_exploded);
		}

	}

	// destroy the exploded data
	if(exploded)
	{
		BP_DestroyPointerArray((void **) exploded, total_lines);
		bpfree(exploded);
	}



	// create return structure
	P_BP_GENERIC_STRING_COLUMNIZATION ret_val = (P_BP_GENERIC_STRING_COLUMNIZATION) bpcalloc(sizeof(BP_GENERIC_STRING_COLUMNIZATION), 1);
	ret_val->columns                  = columns;
	ret_val->space_normalization_data = space_normalization_data;
	ret_val->color_normalization_data = color_normalization_data;
	ret_val->column_count             = total_lines;
	ret_val->longest_width            = longest_width;
	ret_val->longest_width_colorized  = longest_width_colorized;


	// if a tailqueue was passed in, return it here as a tq allocated element
	if(tq) return BP_StoreColumnizationInTailQ(ret_val, tq, BP_TRUE);

	// return the filled out columns
	return ret_val;

}


// Merge a stack of pointers to columnizations into one columnization structure.  This
// routine will merge them vertically (one on top of the other).
P_BP_GENERIC_STRING_COLUMNIZATION BP_MergeGenericColumnizationsVertically(P_BP_GENERIC_STRING_COLUMNIZATION * columnizations, size_t columnization_count)
{

	// ensure we have columns to count
	if(!columnizations || !columnization_count)
		return NULL;

	// values to precalculate
	size_t longest_non_colorized = 0;
	size_t longest_colorized = 0;
	size_t total_lines_vertical = 0;

	// declare iterator
	size_t n = 0;
	for(; n < columnization_count; n++)
	{

		// all columnizations must be set, if one isn't, go ahead and
		// exit here as we've recieved erroneous data from the stupid
		// programmer.
		if(!columnizations[n])
			return NULL;

		// increase column count
		total_lines_vertical+=columnizations[n]->column_count;

		// set longest non-colorized
		if(columnizations[n]->longest_width > longest_non_colorized)
			longest_non_colorized = columnizations[n]->longest_width;

		// set longest colorized
		if(columnizations[n]->longest_width_colorized > longest_colorized)
			longest_colorized = columnizations[n]->longest_width_colorized;

	}

	/*
	printf("\n Got Total Vertical Lines?:  %u", total_lines_vertical);
	printf("\n Got Longest Colorized:      %u", longest_colorized);
	printf("\n Got Longest Non-Colorized:  %u", longest_non_colorized);
	*/

	// ensure we can create the columinzation
	P_BP_GENERIC_STRING_COLUMNIZATION ret_val = (P_BP_GENERIC_STRING_COLUMNIZATION) bpcalloc(sizeof(BP_GENERIC_STRING_COLUMNIZATION), 1);
	if(!ret_val)
		return NULL;

	// set widths from precalculations
	ret_val->column_count            = total_lines_vertical;
	ret_val->longest_width_colorized = longest_colorized;
	ret_val->longest_width           = longest_non_colorized;

	// allocate space for the columns
	ret_val->columns = (char **) bpcalloc(sizeof(char *) * (ret_val->column_count+1), 1);
	if(!ret_val->columns)
	{
		bpfree(ret_val);
		return NULL;
	}

	// allocate space for space normalization data
	ret_val->space_normalization_data = (size_t *) bpcalloc(sizeof(size_t) * (ret_val->column_count+1), 1);
	if(!ret_val->space_normalization_data)
	{
		BP_DestroyGenericColumnization(ret_val, BP_TRUE);
		return NULL;
	}

	// allocate space for color normalization data
	ret_val->color_normalization_data = (size_t *) bpcalloc(sizeof(size_t) * (ret_val->column_count+1), 1);
	if(!ret_val->color_normalization_data)
	{
		BP_DestroyGenericColumnization(ret_val, BP_TRUE);
		return NULL;
	}


	// declare iterator for line indexs
	size_t ret_line_idx = 0;

	// Declare iterator for column line indexes.  This is used
	// to iterate through each of the columnization routine
	// paramaters -> columns[] values.
	size_t col_line_idx = 0;

	// create output columnization from stacked data
	for(n = 0; n < columnization_count; n++)
	{

		// for each index, create new items
		for
		(
			col_line_idx = 0;
			col_line_idx < columnizations[n]->column_count;
			col_line_idx++
		)
		{


			// only process if the line is longer than the output
			if(longest_non_colorized > columnizations[n]->longest_width)
			{

				// if the row is colorized, specifiy that here
				if(columnizations[n]->color_normalization_data)
				{

					// reallocate space including normalization data
					ret_val->columns[ret_line_idx] = (char *) bpcalloc(longest_non_colorized + columnizations[n]->color_normalization_data[col_line_idx]+columnizations[n]->space_normalization_data[col_line_idx]+1, 1);

					// set padding
					memset(ret_val->columns[ret_line_idx], 0x20, longest_non_colorized + columnizations[n]->color_normalization_data[col_line_idx]);

					if((bpstrlen(ret_val->columns[ret_line_idx]) - columnizations[n]->color_normalization_data[col_line_idx]) < longest_non_colorized)
					{
						// printf("\n FOUND CANDIDATE?!");
						// +columnizations[n]->space_normalization_data[col_line_idx]
					}

					// copy in data
					memcpy(ret_val->columns[ret_line_idx], columnizations[n]->columns[col_line_idx], bpstrlen(columnizations[n]->columns[col_line_idx]));

					// set space normalization data
					if(columnizations[n]->space_normalization_data)
						ret_val->space_normalization_data[ret_line_idx] = 0; // columnizations[n]->space_normalization_data[col_line_idx];

					// set color normalization data
					ret_val->color_normalization_data[ret_line_idx] = columnizations[n]->color_normalization_data[col_line_idx];

				}
				else
				{

					// allocate storage space
					ret_val->columns[ret_line_idx] = (char *) bpcalloc(longest_non_colorized+1, 1);

					// set padding to appropriate length
					memset(ret_val->columns[ret_line_idx], 0x20, longest_non_colorized);

					// copy in data
					memcpy(ret_val->columns[ret_line_idx], columnizations[n]->columns[col_line_idx], bpstrlen(columnizations[n]->columns[col_line_idx]));

					// set space normalization data
					if(columnizations[n]->space_normalization_data)
						ret_val->space_normalization_data[ret_line_idx] = 0; // columnizations[n]->space_normalization_data[col_line_idx];

				}


			}
			else
			{

				// if the row is colorized, specifiy that here
				if(columnizations[n]->color_normalization_data)
				{

					// printf("\n Alloc: %u", longest_non_colorized );

					// reallocate space including normalization data
					ret_val->columns[ret_line_idx] = (char *) bpcalloc(longest_non_colorized + columnizations[n]->color_normalization_data[col_line_idx]+columnizations[n]->space_normalization_data[col_line_idx]+1, 1);

					// set padding
					memset(ret_val->columns[ret_line_idx], 0x20, longest_non_colorized + columnizations[n]->color_normalization_data[col_line_idx]);

					// copy in data
					memcpy(ret_val->columns[ret_line_idx], columnizations[n]->columns[col_line_idx], bpstrlen(columnizations[n]->columns[col_line_idx]));

					// set space normalization data
					if(columnizations[n]->space_normalization_data)
						ret_val->space_normalization_data[ret_line_idx] = 0; // columnizations[n]->space_normalization_data[col_line_idx];

					// set color normalization data
					ret_val->color_normalization_data[ret_line_idx] = columnizations[n]->color_normalization_data[col_line_idx];

				}
				else
				{

					ret_val->columns[ret_line_idx] = (char *) bpcalloc(longest_non_colorized+1, 1);

					// set padding to appropriate length
					memset(ret_val->columns[ret_line_idx], 0x20, longest_non_colorized);

					// printf("\n MOO???: %u - %u", longest_non_colorized, bpstrlen(columnizations[n]->columns[col_line_idx]));
					// copy in data
					memcpy(ret_val->columns[ret_line_idx], columnizations[n]->columns[col_line_idx], bpstrlen(columnizations[n]->columns[col_line_idx]));

					// set space normalization data
					if(columnizations[n]->space_normalization_data)
						ret_val->space_normalization_data[ret_line_idx] = 0; // columnizations[n]->space_normalization_data[col_line_idx];


				}


			}

			// increment the return line index
			ret_line_idx++;

		}

	}


	// return the filled out pointer
	return ret_val;

}

// Merge a stack of pointers to columnizations into one columnization structure.  This
// routine will merge them vertically (one on top of the other).
P_BP_GENERIC_STRING_COLUMNIZATION BP_MergeGenericColumnizationsHorizontally(P_BP_GENERIC_STRING_COLUMNIZATION * columnizations, size_t columnization_count)
{


	// ensure we have columns to count
	if(!columnizations || !columnization_count)
		return NULL;

	// the maximum widest column
	size_t widest_column           = 0;
	size_t widest_column_colorized = 0;
	size_t tallest_column          = 0;

	// declare iterator
	size_t n = 0;
	size_t j = 0;
	for(; n < columnization_count; n++)
	{

		widest_column           += columnizations[n]->longest_width;
		widest_column_colorized += columnizations[n]->longest_width_colorized;

		if(columnizations[n]->column_count > tallest_column)
			tallest_column = columnizations[n]->column_count;

	}

	// this holds the calculated line length
	size_t calculated_line_length = 0;

	// create space to hold recalculate normalization data
	size_t * new_color_normalization_data = (size_t *) bpcalloc(sizeof(size_t) * (tallest_column+1), 1);
	size_t * new_space_normalization_data = (size_t *) bpcalloc(sizeof(size_t) * (tallest_column+1), 1);

	// create space for new column data
	char ** new_column_data = (char **) bpcalloc(sizeof(char *) * (tallest_column+1), 1);


	size_t col_iter_idx = 0;
	for(n = 0; n < tallest_column; n++)
	{

		// reset before running line calculation
		calculated_line_length = 0;

		P_BP_LINKL tq = BP_CreateLinkL(".", BP_FALSE);


		// run line calculation
		for( j = 0; j < columnization_count; j++)
		{

			if(col_iter_idx < columnizations[j]->column_count)
			{
				if(columnizations[j]->columns[col_iter_idx])
				{
					tq_strdup(tq, columnizations[j]->columns[col_iter_idx], BPLN);
				}

				// set new color normalization data if available
				if(columnizations[j]->color_normalization_data)
					new_color_normalization_data[col_iter_idx] += columnizations[j]->color_normalization_data[col_iter_idx];
				if(columnizations[j]->space_normalization_data)
					new_space_normalization_data[col_iter_idx] += columnizations[j]->space_normalization_data[col_iter_idx];

			}
			else
			{

				// create temporary buffer
				char *tmp_buff = (char *) bpcalloc(columnizations[j]->longest_width+1, 1);

				// set spaces
				memset(tmp_buff, 0x20, columnizations[j]->longest_width);

				// add empty line to the tailqueue
				tq_strdup(tq, tmp_buff, BPLN);

				// destroy the temporary buffer
				bpfree(tmp_buff);

			}

		}

		// flatten out the elements above
		BP_LinkLFlattenJoinStringElements(tq);

		// duplicate string in new column data
		new_column_data[col_iter_idx] = (char *) bpstrdup((char *) tq->flat_buff);

		// destroy the linkl at this point
		BP_DestroyLinkL(tq, BP_TRUE);

		col_iter_idx++;

	}

	P_BP_GENERIC_STRING_COLUMNIZATION ret_val = (P_BP_GENERIC_STRING_COLUMNIZATION) bpcalloc(sizeof(BP_GENERIC_STRING_COLUMNIZATION), 1);
	ret_val->color_normalization_data = new_color_normalization_data;
	ret_val->space_normalization_data = new_space_normalization_data;
	ret_val->columns                  = new_column_data;
	ret_val->column_count             = tallest_column;
	ret_val->longest_width            = widest_column;
	ret_val->longest_width_colorized  = widest_column_colorized;



	// return the filled out pointer
	return ret_val;

}


// Display a generic columnization.
BP_ERROR_T BP_DisplayGenericColumnization(P_BP_GENERIC_STRING_COLUMNIZATION columnization, BP_BOOL display_only_column_data)
{

	if(!columnization)
		return ERR_FAILURE;

	if(display_only_column_data == BP_FALSE)
	{
		printf("\n [+] Columnization Data (%p)", columnization);
		printf("\n");
		printf("\n     columns:                  %p", columnization->columns);
		printf("\n     column_count:             %u", columnization->column_count);
		printf("\n     space_normalization_data: %p", columnization->space_normalization_data);
		printf("\n     color_normalization_data: %p", columnization->color_normalization_data);
		printf("\n     longest_width:            %u", columnization->longest_width);
		printf("\n     longest_width_colorized:  %u", columnization->longest_width_colorized);

		if(columnization->columns && columnization->column_count)
		{

			printf("\n   ");
			printf("\n     [+] Column Data: str (len - rlen -  c_norm - s_norm)");
			printf("\n");

			// display columns
			size_t n = 0;
			for(; n < columnization->column_count; n++)
			{

				printf
				(
					"\n        %s  (len: %u - rlen: %u, - %u - %u)",
					columnization->columns[n],
					bpstrlen(columnization->columns[n]),
					bpstrlen(columnization->columns[n]) - (columnization->color_normalization_data ? columnization->color_normalization_data[n] : 0),
					columnization->color_normalization_data ? columnization->color_normalization_data[n] : 0,
					columnization->space_normalization_data ? columnization->space_normalization_data[n] : 0
				);

			}

		}

		printf("\n\n");
	}
	else
	{

		// display columns
		size_t n = 0;
		for(; n < columnization->column_count; n++)
		{
			printf("\n%s",	columnization->columns[n]);
		}

	}

	// return indicating success
	return ERR_SUCCESS;

}


// Destroys a generic columnization.  Set the second parameter to BP_TRUE if you want to also
// destroy the data which was passed in (aka the structure itself).  You'd not want to enable
// this if you were using a local stack structure (not heap).
BP_ERROR_T BP_DestroyGenericColumnization(P_BP_GENERIC_STRING_COLUMNIZATION columnization, BP_BOOL destroy_struct, P_BP_LINKL tq)
{


	// ensure we have a pointer
	if(!columnization)
		return ERR_FAILURE;


	if(!tq)
	{
		// destroy color normalization data
		if(columnization->color_normalization_data)
		{
			bpfree(columnization->color_normalization_data);
		}

		// destroy space normalization data
		if(columnization->space_normalization_data)
		{
			bpfree(columnization->space_normalization_data);
		}

		// destroy column data
		size_t n = 0;
		if(columnization->columns)
		{
			for(; n < columnization->column_count; n++)
				bpfree(columnization->columns[n]);

			bpfree(columnization->columns);
			columnization->columns = NULL;
		}


		// if we're destroying it, go ahead and do it, if not
		// just zero it out.
		if(destroy_struct == BP_TRUE)
		{
			bpfree(columnization);
		}
		else
		{
			memset(columnization, 0x00, sizeof(BP_GENERIC_STRING_COLUMNIZATION));
		}
	}
	else
	{

		// destroy color normalization data
		if(columnization->color_normalization_data)
		{
			tq_free(tq, columnization->color_normalization_data);
		}

		// destroy space normalization data
		if(columnization->space_normalization_data)
		{
			tq_free(tq, columnization->space_normalization_data);
		}

		// destroy column data
		size_t n = 0;
		if(columnization->columns)
		{

			for(; n < columnization->column_count; n++)
				tq_free(tq, columnization->columns[n]);

			tq_free(tq, columnization->columns);
			columnization->columns = NULL;

		}


		// if we're destroying it, go ahead and do it, if not
		// just zero it out.
		if(destroy_struct == BP_TRUE)
		{
			tq_free(tq, columnization);
		}
		else
		{
			memset(columnization, 0x00, sizeof(BP_GENERIC_STRING_COLUMNIZATION));
		}

	}
	// return indicating success
	return ERR_SUCCESS;

}

// Since columnizations typically don't work within the greater mindest of tailqueue allocators, this
// routine allows you to duplicate a columnization and store it in a tailqueue.
P_BP_GENERIC_STRING_COLUMNIZATION BP_StoreColumnizationInTailQ(P_BP_GENERIC_STRING_COLUMNIZATION columnization, P_BP_LINKL tq, BP_BOOL destroy_copied_columnization)
{

	// ensure we have a columnization and a tail queue allocator
	if(!columnization || !tq)
		return NULL;

	// create space to store here
	P_BP_GENERIC_STRING_COLUMNIZATION ret_val = (P_BP_GENERIC_STRING_COLUMNIZATION) tq_calloc(tq, sizeof(BP_GENERIC_STRING_COLUMNIZATION), 1, BPLN);

	// allocate color normalization data
	if(columnization->color_normalization_data)
	{

		// allocate space for normalization data
		ret_val->color_normalization_data = (size_t *) tq_calloc(tq, sizeof(size_t) * (columnization->column_count+1), 1, BPLN);

		// copy in color normalization data
		memcpy(ret_val->color_normalization_data, columnization->color_normalization_data, sizeof(size_t) * columnization->column_count);

	}

	// allocate space normalization data
	if(columnization->space_normalization_data)
	{

		// allocate space for normalization data
		ret_val->space_normalization_data = (size_t *) tq_calloc(tq, sizeof(size_t) * (columnization->column_count+1), 1, BPLN);

		// copy in normalization data
		memcpy(ret_val->space_normalization_data, columnization->space_normalization_data, sizeof(size_t) * columnization->column_count);

	}

	// copy in columns if set
	if(columnization->columns)
	{

		// allocate space for columns
		ret_val->columns                  = (char **)  tq_calloc(tq, sizeof(char *) * (columnization->column_count+1), 1, BPLN);

		// copy in column data by duplicating strings
		size_t n = 0;
		for(; n < columnization->column_count; n++)
			ret_val->columns[n] = (char *) tq_strdup(tq, columnization->columns[n], BPLN);

	}

	// set static members
	ret_val->column_count             = columnization->column_count;
	ret_val->longest_width            = columnization->longest_width;
	ret_val->longest_width_colorized  = columnization->longest_width_colorized;

	// set tailqueue allocator here
	ret_val->tq = tq;

	// destroy columnization here
	BP_DestroyGenericColumnization(columnization, BP_TRUE, columnization->tq);

	// copy data into normalization arrays
	return ret_val;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Buffer Borderization %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Clones a border set on the heap.  If a tq is set, it will be cloned within that allocator.
P_BP_STRING_ROW_BORDER_SET BP_CloneBorderSet(P_BP_STRING_ROW_BORDER_SET border_set_to_clone, P_BP_LINKL tq)
{

	// ensure we have a border set to work with
	if(!border_set_to_clone)
		return NULL;

	P_BP_STRING_ROW_BORDER_SET ret_border_set = NULL;

	// allocate via tailqueue if one is provided, if not, allocate normally
	if(tq)
	{

		// create the border set using tail queue allocator
		ret_border_set = (P_BP_STRING_ROW_BORDER_SET) tq_calloc(tq, sizeof(BP_STRING_ROW_BORDER_SET), 1, BPLN);

		// set tailqueue here
		ret_border_set->tq = tq;

		// set flag here indicating allocation via tail queue
		ret_border_set->allocated_via_tailqueue = BP_TRUE;

		// allocate individual elements using tail queue allocator
		ret_border_set->border_left                = tq_strdup(tq, border_set_to_clone->border_left, BPLN);
		ret_border_set->border_right               = tq_strdup(tq, border_set_to_clone->border_right, BPLN);
		ret_border_set->border_top_char            = tq_strdup(tq, border_set_to_clone->border_top_char, BPLN);
		ret_border_set->border_top_left_corner     = tq_strdup(tq, border_set_to_clone->border_top_left_corner, BPLN);
		ret_border_set->border_top_right_corner    = tq_strdup(tq, border_set_to_clone->border_top_right_corner, BPLN);
		ret_border_set->border_bottom_char         = tq_strdup(tq, border_set_to_clone->border_bottom_char, BPLN);
		ret_border_set->border_bottom_left_corner  = tq_strdup(tq, border_set_to_clone->border_bottom_left_corner, BPLN);
		ret_border_set->border_bottom_right_corner = tq_strdup(tq, border_set_to_clone->border_bottom_right_corner, BPLN);

	}
	else
	{

		// create the border set using standard allocator
		ret_border_set = (P_BP_STRING_ROW_BORDER_SET) tq_calloc(tq, sizeof(BP_STRING_ROW_BORDER_SET), 1, BPLN);

		// allocate individual elements using standard allocator
		ret_border_set->border_left                = bpstrdup(border_set_to_clone->border_left);
		ret_border_set->border_right               = bpstrdup(border_set_to_clone->border_right);
		ret_border_set->border_top_char            = bpstrdup(border_set_to_clone->border_top_char);
		ret_border_set->border_top_left_corner     = bpstrdup(border_set_to_clone->border_top_left_corner);
		ret_border_set->border_top_right_corner    = bpstrdup(border_set_to_clone->border_top_right_corner);
		ret_border_set->border_bottom_char         = bpstrdup(border_set_to_clone->border_bottom_char);
		ret_border_set->border_bottom_left_corner  = bpstrdup(border_set_to_clone->border_bottom_left_corner);
		ret_border_set->border_bottom_right_corner = bpstrdup(border_set_to_clone->border_bottom_right_corner);

	}


	// set integer/size_t/bool values (literal, doesn't depend on heap allocations)
	ret_border_set->border_bottom_right_corner_len_if_colorized = border_set_to_clone->border_bottom_right_corner_len_if_colorized;
	ret_border_set->border_bottom_left_corner_len_if_colorized  = border_set_to_clone->border_bottom_left_corner_len_if_colorized;
	ret_border_set->border_bottom_char_len_if_colorized         = border_set_to_clone->border_bottom_char_len_if_colorized;
	ret_border_set->border_bottom_enabled                       = border_set_to_clone->border_bottom_enabled;
	ret_border_set->border_top_right_corner_len_if_colorized    = border_set_to_clone->border_top_right_corner_len_if_colorized;
	ret_border_set->border_top_left_corner_len_if_colorized     = border_set_to_clone->border_top_left_corner_len_if_colorized;
	ret_border_set->border_top_char_len_if_colorized            = border_set_to_clone->border_top_char_len_if_colorized;
	ret_border_set->border_top_enabled                          = border_set_to_clone->border_top_enabled;
	ret_border_set->border_right_display_len_if_colorized       = border_set_to_clone->border_right_display_len_if_colorized;
	ret_border_set->border_left_display_len_if_colorized        = border_set_to_clone->border_left_display_len_if_colorized;

	// return with the border set
	return ret_border_set;

}


// Add a border to a buffer.  Returns a newly allocated string columnization structure (aka
// you must destroy the old one)
P_BP_GENERIC_STRING_COLUMNIZATION BP_BorderizeStringColumnization(P_BP_GENERIC_STRING_COLUMNIZATION string_columnization, P_BP_STRING_ROW_BORDER_SET border_set)
{


	if(!string_columnization)
		return NULL;
	if(!border_set)
		return NULL;
	if(!string_columnization->columns)
		return NULL;


	// create columnization return data
	P_BP_GENERIC_STRING_COLUMNIZATION ret_val = (P_BP_GENERIC_STRING_COLUMNIZATION) bpcalloc(sizeof(BP_GENERIC_STRING_COLUMNIZATION), 1);

	// set the initial column count here
	ret_val->column_count = string_columnization->column_count;

	// this is used for indexing data
	size_t current_str_idx = 0;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Border Counters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// border left len
	size_t left_border_len  = 0;
	size_t right_border_len = 0;

	// top corner lengths
	size_t top_left_corner_len  = 0;
	size_t top_right_corner_len = 0;

	// bottom corner lengths
	size_t bottom_left_corner_len  = 0;
	size_t bottom_right_corner_len = 0;

	// calculate top and right & left items
	if(border_set->border_top_left_corner)
		top_left_corner_len  = bpstrlen(border_set->border_top_left_corner);
	if(border_set->border_top_right_corner)
		top_right_corner_len = bpstrlen(border_set->border_top_right_corner);

	// calculate bottom right & left items
	if(border_set->border_bottom_left_corner)
		bottom_left_corner_len  = bpstrlen(border_set->border_bottom_left_corner);
	if(border_set->border_bottom_right_corner)
		bottom_right_corner_len = bpstrlen(border_set->border_bottom_right_corner);

	// calculate left and right border lengths
	if(border_set->border_left)
		left_border_len = bpstrlen(border_set->border_left);
	if(border_set->border_right)
		right_border_len = bpstrlen(border_set->border_right);

/*
	if((top_left_corner_len + top_right_corner_len) >  string_columnization->longest_width)
	{

		printf("\n Failing her?: %u", string_columnization->longest_width);
		return NULL;
	}
*/
	// line iterator
	size_t n = 0;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Allocate Storage Spaces %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// add storage space for top and bottom if necessary
	if(border_set->border_top_enabled)
		ret_val->column_count++;
	if(border_set->border_bottom_enabled)
		ret_val->column_count++;

	// create space for columns
	ret_val->columns = (char **) bpcalloc(sizeof(char *) * (ret_val->column_count+1), 1);

	// allocate space for color normalization data
	ret_val->color_normalization_data = (size_t *) bpcalloc(sizeof(size_t) * (ret_val->column_count+1), 1);

	// allocate space for space normalization data
	ret_val->space_normalization_data = (size_t *) bpcalloc(sizeof(size_t) * (ret_val->column_count+1), 1);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Top Border if Necessary %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(border_set->border_top_enabled)
	if(border_set->border_top_right_corner || border_set->border_top_left_corner)
	{

		// allocator for top tailqueue
		P_BP_LINKL top_tq = BP_CreateLinkL(".", BP_FALSE);

		// add left corner
		if(border_set->border_top_left_corner)
			tq_strdup(top_tq, border_set->border_top_left_corner, BPLN);

		// add character spaces
		for(n = 0; n < string_columnization->longest_width - (top_left_corner_len + top_right_corner_len) + (right_border_len + left_border_len); n++)
		{
			tq_strndup(top_tq, border_set->border_top_char, 1, BPLN);
		}

		// add right corner
		if(border_set->border_top_right_corner)
			tq_strdup(top_tq, border_set->border_top_right_corner, BPLN);

		// flatten the join
		BP_LinkLFlattenJoinStringElements(top_tq);

		// since we're not doing any color normalization, set that here
		ret_val->color_normalization_data[current_str_idx] = 0;

		// since we're not doing any space normalization, set that here
		ret_val->space_normalization_data[current_str_idx] = 0;

		// set column data
		ret_val->columns[current_str_idx] = (char *) bpstrndup((char *) top_tq->flat_buff, top_tq->flat_buff_n);

		// increment the current string index
		current_str_idx++;

		// destroy the tailqueue
		BP_DestroyLinkL(top_tq, BP_TRUE);

	}


	// Now add columns
	for(n = 0; n < string_columnization->column_count; n++)
	{

		// create flattener
		P_BP_LINKL tq = BP_CreateLinkL(".", BP_FALSE);

		// add left border if set
		if(border_set->border_left)
			tq_strdup(tq, border_set->border_left, BPLN);

		// now add string
		tq_strdup(tq, string_columnization->columns[n], BPLN);

		// add right border if set
		if(border_set->border_right)
			tq_strdup(tq, border_set->border_right, BPLN);

		// flatten the elements
		BP_LinkLFlattenJoinStringElements(tq);

		// set column data
		ret_val->columns[current_str_idx] = (char *) bpstrndup((char *) tq->flat_buff, tq->flat_buff_n);

		// set color normalization data if we have a valid array to set from
		if(string_columnization->color_normalization_data)
		{
			ret_val->color_normalization_data[current_str_idx] = string_columnization->color_normalization_data[n];

			// set the longest width if necessary
			if(bpstrlen(ret_val->columns[current_str_idx]) > ret_val->longest_width_colorized)
				ret_val->longest_width_colorized = bpstrlen(ret_val->columns[current_str_idx]);



		}

		// set space normalization data if we have a valid array to set from
		if(string_columnization->space_normalization_data)
			ret_val->space_normalization_data[current_str_idx] = string_columnization->space_normalization_data[n];

		// set longest width here
		if
		(
			(bpstrlen(ret_val->columns[current_str_idx]) - ret_val->color_normalization_data[current_str_idx]) > ret_val->longest_width
		)
		{
			ret_val->longest_width = bpstrlen(ret_val->columns[current_str_idx]) - ret_val->color_normalization_data[current_str_idx];
		}

		// increment the string index
		current_str_idx++;

		// destroy the flattener
		BP_DestroyLinkL(tq, BP_TRUE);

	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Bottom Border if Necessary %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(border_set->border_bottom_enabled)
	if(border_set->border_bottom_right_corner || border_set->border_bottom_left_corner)
	{

		// allocator for bottom tailqueue
		P_BP_LINKL bottom_tq = BP_CreateLinkL(".", BP_FALSE);

		// add left corner
		if(border_set->border_bottom_left_corner)
			tq_strdup(bottom_tq, border_set->border_bottom_left_corner, BPLN);

		// add character spaces
		for(n = 0; n < string_columnization->longest_width - (bottom_left_corner_len + bottom_right_corner_len) + (right_border_len + left_border_len); n++)
		{
			tq_strndup(bottom_tq, border_set->border_bottom_char, 1, BPLN);
		}

		// add right corner
		if(border_set->border_bottom_right_corner)
			tq_strdup(bottom_tq, border_set->border_bottom_right_corner, BPLN);

		// flatten the join
		BP_LinkLFlattenJoinStringElements(bottom_tq);

		// set column data
		ret_val->columns[current_str_idx] = (char *) bpstrndup((char *) bottom_tq->flat_buff, bottom_tq->flat_buff_n);

		// the longest (uncolorized) width will always be the bottom line length
		ret_val->longest_width = bpstrlen(ret_val->columns[current_str_idx]);

		// increment the current string index
		current_str_idx++;

		// destroy the tailqueue
		BP_DestroyLinkL(bottom_tq, BP_TRUE);



	}

	// printf("\n %%%%%%%%% DIE %%%%%%%%%%%");
	// return the data here
	return ret_val;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Line Oriented Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// returns the number of lines in a buffer deliniated by the supplied delinator.
size_t BP_GetNumberOfLinesInBuffer(char *buff, size_t buff_len, unsigned char deliniator)
{

	if(!buff | !buff_len || !deliniator)
		return 0;

	// create separator as stack data
	char separator[2];
	BP_ZERO_STACK(separator);

	// set index to the supplied deliniator
	separator[0] = (unsigned char) deliniator;

	// try to explode the number of lines in the buffer
	char ** exploded = BP_ExplodeBufferBin(buff, buff_len, (char*) &separator, 1, BP_GIGABYTE);

	// ensure we've exploded
	if(!exploded)
	{
		return 0;
	}

	// count number of exploded lines
	size_t n = 0;
	for(; exploded[n]; n++)	{}

	// destroy the allocated data
	BP_DestroyPointerArray((void **) exploded, n);

	// destroy the pointer container
	bpfree(exploded);

	// return the count
	return n;


}

// returns the longest line in a buffer deliniated by the supplied deliniator
size_t BP_GetLongestLineInBuffer(char *buff, size_t buff_len, unsigned char deliniator)
{

	// ensure we have parameters
	if(!buff | !buff_len || !deliniator)
		return 0;

	// create separator as stack data
	char separator[2];
	BP_ZERO_STACK(separator);

	// set index to the supplied deliniator
	separator[0] = (unsigned char) deliniator;

	// try to explode the number of lines in the buffer
	char ** exploded = BP_ExplodeBufferBin(buff, buff_len, (char*) &separator, 1, BP_GIGABYTE);

	// ensure we've exploded
	if(!exploded)
		return 0;

	// iterative line counters
	size_t last_line    = 0;
	size_t longest_line = 0;

	// count number of exploded lines
	size_t n = 0;
	for(; exploded[n]; n++)
	{
		last_line = bpstrlen(exploded[n]);
		if(last_line > longest_line)
			longest_line = last_line;
	}

	// destroy the allocated data
	BP_DestroyPointerArray((void **) exploded, n);

	// destroy the pointer container
	bpfree(exploded);

	// return the count
	return longest_line;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Character Position Finder %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



// Returns the index within the provided buffer of the first instance of
// character.  This is used for finding offsets into buffers.  Returns -1
// if an offset cannot be found (or error)
int BP_LookupFirstCharacterOffsetInBuffer
(
	char * buffer,
	size_t buffer_len,
	char   lookup_char,
	BP_STRING_OFFSET_LOOKUP_DIRECTION lookup_direction
)
{

	// run basic checks (do not check lookup_char because any char is valid)
	if(!buffer)
		return -1;
	if(!buffer_len)
		return -1;

	// ensure a direction marker is set
	if
	(
		lookup_direction != BP_STRING_OFFSET_LOOKUP_DIRECTION_FORWARD &&
		lookup_direction != BP_STRING_OFFSET_LOOKUP_DIRECTION_REVERSE
	)
	{
		return -1;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Forward Lookup %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(lookup_direction == BP_STRING_OFFSET_LOOKUP_DIRECTION_FORWARD)
	{
		size_t n = 0;
		for(; n < buffer_len; n++)
		{
			if(buffer[n] == lookup_char)
				return n;
		}

		// return error if we couldn't find a lookup
		return -1;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Reverse Lookup %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(lookup_direction == BP_STRING_OFFSET_LOOKUP_DIRECTION_REVERSE)
	{
		// set index position
		size_t n = buffer_len-1;

		// iterate and run check
		for(; n >= 0; n--)
		{
			if(buffer[n] == lookup_char)
				return n;
		}

		// return error if we couldn't find a lookup
		return -1;

	}

	// return -1 if no checks matched (should never get here)
	return -1;

}


// Same as above but requires a reasonable string as input.
int BP_LookupFirstCharacterOffsetInReasonableString
(
	char * reasonable_string,
	char   lookup_char,
	BP_STRING_OFFSET_LOOKUP_DIRECTION lookup_direction
)
{

	// run reasonable checks
	if(!reasonable_string)
		return -1;
	if(!BP_StringIsReasonablePrintableString(reasonable_string, bpstrlen(reasonable_string), BP_FALSE, BP_FALSE))
		return -1;

	// set len
	size_t len = bpstrlen(reasonable_string);

	// attempt lookup
	return BP_LookupFirstCharacterOffsetInBuffer
	(
		reasonable_string,
		len,
		lookup_char,
		lookup_direction
	);

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% String Quote Testing / Removal %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Tests to see if the provided string is a quoted string.
BP_STRING_QUOTE_CHECK_RESULTS BP_CheckIfBufferIsQuoted(char * buffer, size_t buffer_len)
{


	// check buffer or buffer length
	if(!buffer || !buffer_len)
		return BP_STRING_IS_UNDEFINED_OR_EMPTY;



	// holds the number of single or double quotes found within the string
	size_t number_of_single_quotes = 0;
	size_t number_of_double_quotes = 0;

	// check double quotes first
	if(buffer[0] == '"')
		number_of_double_quotes++;
	if(buffer[buffer_len-1] == '"')
		number_of_double_quotes++;

	// check single quotes next
	if(buffer[0] == '\'')
		number_of_single_quotes++;
	if(buffer[buffer_len-1] == '\'')
		number_of_single_quotes++;


	// check to see if the string is not quoted
	if
	(
			(number_of_single_quotes == 0) &&
			(number_of_double_quotes == 0)
	)
	{

		return BP_STRING_IS_NOT_QUOTED;

	}

	// check for mixed quote error
	if
	(
		(number_of_single_quotes == 1) &&
		(number_of_double_quotes == 1)
	)
	{
		return BP_STRING_IS_MIXED_QUOTED_ERROR;
	}


	// check for double quote next
	if
	(
			(number_of_single_quotes == 0) &&
			(number_of_double_quotes == 2)
	)
	{
		return BP_STRING_IS_DOUBLE_QUOTED;
	}

	// check for single quote next
	if
	(
			(number_of_single_quotes == 2) &&
			(number_of_double_quotes == 0)
	)
	{
		return BP_STRING_IS_SINGLE_QUOTED;
	}


	// check for mismatch now
	if
	(
		(number_of_single_quotes == 1 && number_of_double_quotes == 0) ||
		(number_of_double_quotes == 1 && number_of_single_quotes == 0)
	)
	{
		return BP_STRING_IS_MISMATCHED_QUOTED;
	}


	// return undefined result if we haven't found any other match
	return BP_STRING_RESULT_IS_UNDEFINED;

}


// Strips quotes from a string.  This relies on BP_CheckIfBufferIsQuoted to determine
// quote types and strips from there.  The return value is NULL in the case that the
// string has quotes which were unable to be removed, or a newly allocated string
// containing the unquoted string.  The return value must be freed.  It is recommended
// that you call the check routine before utilizing this routine for thoroughness reasons.
char * BP_UnquoteQuotedString(char *buffer, size_t buffer_len)
{

	// ensure we have a buffer and a length associated
	if(!buffer || !buffer_len)
		return NULL;

	// add quote check
	BP_STRING_QUOTE_CHECK_RESULTS quote_check = BP_CheckIfBufferIsQuoted(buffer, buffer_len);

	// ensure that the string is quoted in some direct way
	if
	(
		(quote_check != BP_STRING_IS_DOUBLE_QUOTED) &&
		(quote_check != BP_STRING_IS_SINGLE_QUOTED)
	)
	{

		// if it's not quoted, return NULL
		return NULL;

	}


	// allocate space for the new string
	char * ret_str = (char *) bpcalloc(buffer_len, 1);

	// ensure we can allocate the string
	if(!ret_str)
		return NULL;

	// set read position as +1 (for quote removal)
	char * read_pos = (char *) &buffer[1];

	// copy in data
	memcpy(ret_str, read_pos, buffer_len-2);

	// return null by default
	return ret_str;

}

// The BP_CheckIfBufferIsQuoted() routine returns a check result.  This result can be
// translated into a string representation by supplying that check result to this routine.
char * BP_QuoteCheckResultToStringValue(BP_STRING_QUOTE_CHECK_RESULTS quote_check_results)
{

	// switch on the check results
	switch(quote_check_results)
	{


		case BP_STRING_IS_UNDEFINED_OR_EMPTY:
			return strdup("BP_STRING_IS_UNDEFINED_OR_EMPTY");

		case BP_STRING_IS_NOT_QUOTED:
			return strdup("BP_STRING_IS_NOT_QUOTED");

		case BP_STRING_IS_DOUBLE_QUOTED:
			return strdup("BP_STRING_IS_DOUBLE_QUOTED");

		case BP_STRING_IS_SINGLE_QUOTED:
			return strdup("BP_STRING_IS_SINGLE_QUOTED");

		case BP_STRING_IS_MISMATCHED_QUOTED:
			return strdup("BP_STRING_IS_MISMATCHED_QUOTED");

		case BP_STRING_IS_MIXED_QUOTED_ERROR:
			return strdup("BP_STRING_IS_MIXED_QUOTED_ERROR");

		case BP_STRING_RESULT_IS_UNDEFINED:
			return strdup("BP_STRING_RESULT_IS_UNDEFINED");

		// break on default (return NULL)
		default:
			break;

	}

	// return NULL on failure
	return NULL;

}


// --- Quote escaping tests -----------------------------

// Note: Escaped quotes are escaped by the '\' character.

// Returns BP_TRUE if the buffer contains an unescaped single quote.  Set char_pos_inout to a size_t
// to retrieve the position in the buffer where the unquoted character was found.
BP_BOOL BP_BufferContainsUnescapedSingleQuote(char *buffer, size_t buff_size, size_t *char_pos_inout)
{

	if(!buffer)
		return BP_FALSE;
	if(!buff_size)
		return BP_FALSE;

	// walk through the buffer and check for single quotes
	size_t n = 0;
	for(; n < buff_size; n++)
	{

		// if the index at the current position
		if(buffer[n] == '\'')
		{

		}

	}

	// return indicating that the buffer contains a unescaped single quote
	return BP_TRUE;

}

// Returns BP_TRUE if the buffer contains an unescaped double quote.  Set char_pos_inout to a size_t
// to retrieve the position in the buffer where the unquoted character was found.
BP_BOOL BP_BufferContainsUnescapedDoubleQuote(char *buffer, size_t buff_size, size_t *char_pos_inout)
{

	// return indicating success
	return BP_TRUE;

}




// Counts the number of instances a given token is found within a buffer.  This function
// is binary safe.  The primary purpose of this routine is for determining the size of
// storage arrays to generate when exploding or splitting strings/buffers.
size_t BP_CountTokensInBuffer(char *buffer, size_t buffer_len, char *token, size_t token_len)
{

	// Ensure that the passed in parameters are set.  They must all be set or the routine
	// is being used improperly.
	if(!buffer || !buffer_len || !token || !token_len)
		return 0;

	// number of instances
	size_t num_instances = 0;

	// iterate through the characters in a string and run compares to determine the token count
	size_t n = 0;
	for(; n < buffer_len; n++)
	{

		// If the buffer length remaining is less than the token length
		// then there can be no more comparison matching.
		if((buffer_len - n) < token_len)
			break;

		// since we have a proper sized buffer to match from, we can
		// run the compare here.
		if(memcmp(&buffer[n], token, token_len) == 0)
		{

			// increment the number of instances discovered
			num_instances++;

			// move past the token (minus one because the loop increments
			// the value)
			n += ( token_len - 1 );

		}

	}

	// return the number of counted instances
	return num_instances;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Simple Display Routines for Arbitrary Types %%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Display an array of strings.  This is primarily a utility routine to assist in development
// related tasks, such as verifying exploded data content.
BP_ERROR_T BP_DisplayStringArray(char * prefix, size_t new_lines_between_item, size_t tabs_before_item, char **array)
{

	// ensure we have an array to work with
	if(!array)
		return ERR_FAILURE;

	// walk items and display
	size_t n = 0;
	size_t j = 0;
	size_t i = 0;
	for(; array[n]; n++)
	{

		// add tabs first
		for(i = 0; i < tabs_before_item; i++)
			printf("\t");

		// display the prefix
		if(prefix)
			printf("%s", prefix);

		// display the item
		printf("%s", array[n]);

		// display set newlines
		for(j = 0; j < new_lines_between_item; j++)
			printf("\n");

	}

	// return indicating success
	return ERR_SUCCESS;

}

// Creates a column structure from a character ** array
P_BP_SFS_STRING_COLUMN BP_CreateStringColumnFromStringArrayTQ(char ** string_array, size_t entries, P_BP_COLOR_CODE_SET_T color_info, P_BP_LINKL tq)
{

	// ensure we have string array and count
	if(!string_array || !entries || !tq)
		return NULL;

	/*

	// allocate collumn structure
	P_BP_STRING_COLUMN column = (P_BP_STRING_COLUMN) tq_calloc(tq, sizeof(BP_STRING_COLUMN), 1, BPLN);
	if(!column)
		return NULL;

	// allocate space for entries
	column->col_data = (char **) tq_calloc(tq, sizeof(char *) * (entries + 1), 1, BPLN);
	if(!column->col_data)
	{
		tq_free(tq, column);
		return NULL;
	}

	// allocate space for widths
	column->widths = (size_t *) tq_calloc(tq, sizeof(size_t ) * (entries+1), 1, BPLN);
	if(!column->widths)
	{
		tq_free(tq, column->col_data);
		tq_free(tq, column);
		return NULL;
	}

	// longest entry checking
	size_t longest_entry_len = 0;
	size_t current_entry_len = 0;

	// iterate through entries and duplicate each
	size_t n = 0;
	for(; n < entries; n++)
	{

		// ensure we have an array placement
		if(!string_array[n])
			continue;

		// calculate current entry length
		current_entry_len = bpstrlen(string_array[n]);

		// check entry lengths
		if(current_entry_len > longest_entry_len)
			longest_entry_len = current_entry_len;


		// store entry width
		column->widths[n]   = current_entry_len;

		// try to duplicate the column
		column->col_data[n] = tq_strdup(tq, string_array[n], BPLN);



		// ensure we could create the string ok, if not, destroy allocated
		// memory and exit.
		if(!column->col_data[n])
		{

			BP_DestroyPointerArrayTAILQ(tq, (void **) column->col_data, n);
			tq_free(tq, column->widths);
			tq_free(tq, column);
			return NULL;

		}

	}

	// set entries to the real number of entries created
	column->entries = n;

	// set column width from widest column
	column->widest_column = longest_entry_len;


	// set color info if necessary
	if(color_info)
	{
		memcpy(&column->color_info, color_info, sizeof(BP_COLOR_CODE_SET_T));
	}

		return column;
	*/

	// return column
	return NULL;

}





// display a string column group
BP_ERROR_T BP_DisplayStringColumnGroup(P_BP_SFS_STRING_COLUMN_GROUP column_group, BP_BOOL pad_column_mismatch_lines)
{


	// declare iterators
	size_t n = 0;
	size_t j = 0;

	// indicator showing that we have no more lines to display
	BP_BOOL finished_displaying_lines = BP_FALSE;

	// string index
	size_t string_index = 0;


	// Step 1:


/*
				BP_printfVTColor
				(
					"%s",
					columns[j].color_info.fg,
					columns[j].color_info.bg,
					columns[j].col_data[string_index]
				);
*/

	return ERR_SUCCESS;


}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// end simple display routines
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Various Coversion Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// convert size_t to buffer
char * BP_SizeTToBuffer(size_t val)
{

	// create string
	char * val_buff = (char *) bpcalloc(32, 1);
	if(!val_buff)
		return NULL;

	// embed the value in the string
	snprintf(val_buff, 31, "%zu", val);

	// result must be bpfree'd
	return val_buff;

}


// convert size_t to buffer
char * BP_SizeTToBufferHexBuffer(size_t val)
{

	// create string
	char * val_buff = (char *) bpcalloc((sizeof(size_t)*2)+3, 1);
	if(!val_buff)
		return NULL;

	// embed the value in the string
	snprintf(val_buff, (sizeof(size_t)*2)+2, "0x%x", val);

	// result must be bpfree'd
	return val_buff;

}

// convert size_t to buffer
char * BP_SizeTToMySQLBuffer(size_t val, P_BP_LINKL tq)
{

	// create string

	char * val_buff = NULL;
	if(!tq)
		val_buff = (char *) bpcalloc(32, 1);
	else
	val_buff = (char *) tq_calloc(tq, 32, 1, BPLN);
	if(!val_buff)
		return NULL;

	// embed the value in the string
	snprintf(val_buff, 31, "0x%x", val);

	// result must be bpfree'd
	return val_buff;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Standard Type to Ascii Conversions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// generate a uint64 value
char * BP_UInt64ToASCII(uint64_t val, BP_NUMERIC_STRING_CONVERSIONS conversion)
{

	// create space to hold the buffer
	char *out_buff = NULL;

	// set random selector
	size_t rand_selection = 0;

	// switch on the conversion
	switch(conversion)
	{

		// standard %llu encoding (returns a normal number)
		case BP_NUMERIC_STRING_CONVERSIONS_STANDARD:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(uint64_t) * 4)+1, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(uint64_t) * 4), "%llu", val);
			break;

		// hex encoding
		case BP_NUMERIC_STRING_CONVERSIONS_HEX:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(uint64_t) * 2)+1, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(uint64_t) * 2), "%llx", val);
			break;

		case BP_NUMERIC_STRING_CONVERSIONS_HEX_WITH_0x:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(uint64_t) * 4)+3, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(uint64_t) * 4)+2, "0x%llx", val);
			break;

		// this generates a 123 or a 0xfff type string from the provided number.
		case BP_NUMERIC_STRING_CONVERSIONS_RANDOM_STANDARD_OR_0xHEX_CONVERSION:

			// make random selection based on pnrg
			rand_selection = BP_GenRandomUnsignedLong(1, 2);
			switch(rand_selection)
			{
				case 1:
					return BP_UInt64ToASCII(val, BP_NUMERIC_STRING_CONVERSIONS_STANDARD);
					break;
				case 2:
					return BP_UInt64ToASCII(val, BP_NUMERIC_STRING_CONVERSIONS_HEX_WITH_0x);
					break;
				default:
					break;
			}

			break;

		default:
			return NULL;
			break;

	}

	// return the generated buffer
	return out_buff;

}

char * BP_UInt32ToASCII(uint32_t val, BP_NUMERIC_STRING_CONVERSIONS conversion)
{

	// buffer which will be returned
	char *out_buff = NULL;

	// set random selector
	size_t rand_selection = 0;

	// switch on the conversion
	switch(conversion)
	{

		// standard %llu encoding (returns a normal number)
		case BP_NUMERIC_STRING_CONVERSIONS_STANDARD:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(uint32_t) * 4)+1, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(uint32_t) * 4), "%u", val);
			break;

		// hex encoding
		case BP_NUMERIC_STRING_CONVERSIONS_HEX:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(uint32_t) * 2)+1, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(uint32_t) * 2), "%x", val);
			break;

		case BP_NUMERIC_STRING_CONVERSIONS_HEX_WITH_0x:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(uint32_t) * 4)+3, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(uint32_t) * 4)+2, "0x%x", val);
			break;

		// this generates a 123 or a 0xfff type string from the provided number.
		case BP_NUMERIC_STRING_CONVERSIONS_RANDOM_STANDARD_OR_0xHEX_CONVERSION:

			// make random selection based on pnrg
			rand_selection = BP_GenRandomUnsignedLong(1, 2);
			switch(rand_selection)
			{
				case 1:
					return BP_UInt32ToASCII(val, BP_NUMERIC_STRING_CONVERSIONS_STANDARD);
					break;
				case 2:
					return BP_UInt32ToASCII(val, BP_NUMERIC_STRING_CONVERSIONS_HEX_WITH_0x);
					break;
				default:
					break;
			}

			break;
		default:
			return NULL;
			break;

	}

	// return the generated buffer
	return out_buff;
}

char * BP_UInt16ToASCII(uint16_t val, BP_NUMERIC_STRING_CONVERSIONS conversion)
{

	// create space to hold the buffer
	char *out_buff = NULL;

	// set random selector
	size_t rand_selection = 0;

	// switch on the conversion
	switch(conversion)
	{

		// standard %llu encoding (returns a normal number)
		case BP_NUMERIC_STRING_CONVERSIONS_STANDARD:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(uint16_t) * 4)+1, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(uint16_t) * 4), "%u", val);
			break;

		// hex encoding
		case BP_NUMERIC_STRING_CONVERSIONS_HEX:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(uint16_t) * 2)+1, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(uint16_t) * 2), "%x", val);
			break;

		case BP_NUMERIC_STRING_CONVERSIONS_HEX_WITH_0x:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(uint16_t) * 4)+3, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(uint16_t) * 4)+2, "0x%x", val);
			break;


		// this generates a 123 or a 0xfff type string from the provided number.
		case BP_NUMERIC_STRING_CONVERSIONS_RANDOM_STANDARD_OR_0xHEX_CONVERSION:

			// make random selection based on pnrg
			rand_selection = BP_GenRandomUnsignedLong(1, 2);
			switch(rand_selection)
			{
				case 1:
					return BP_UInt16ToASCII(val, BP_NUMERIC_STRING_CONVERSIONS_STANDARD);
					break;
				case 2:
					return BP_UInt16ToASCII(val, BP_NUMERIC_STRING_CONVERSIONS_HEX_WITH_0x);
					break;
				default:
					break;
			}

			break;

		default:
			return NULL;
			break;

	}

	// return the generated buffer
	return out_buff;

}


char * BP_UInt8ToASCII(uint8_t  val,  BP_NUMERIC_STRING_CONVERSIONS conversion)
{
	// create space to hold the buffer
	char *out_buff = NULL;

	// set random selector
	size_t rand_selection = 0;

	// switch on the conversion
	switch(conversion)
	{

		// standard %llu encoding (returns a normal number)
		case BP_NUMERIC_STRING_CONVERSIONS_STANDARD:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(uint8_t) * 4)+1, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(uint8_t) * 4), "%u", val);
			break;

		// hex encoding
		case BP_NUMERIC_STRING_CONVERSIONS_HEX:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(uint8_t) * 2)+1, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(uint8_t) * 2), "%x", val);
			break;

		case BP_NUMERIC_STRING_CONVERSIONS_HEX_WITH_0x:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(uint8_t) * 4)+3, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(uint8_t) * 4)+2, "0x%.02x", val);
			break;

		// this generates a 123 or a 0xfff type string from the provided number.
		case BP_NUMERIC_STRING_CONVERSIONS_RANDOM_STANDARD_OR_0xHEX_CONVERSION:

			// make random selection based on pnrg
			rand_selection = BP_GenRandomUnsignedLong(1, 2);
			switch(rand_selection)
			{
				case 1:
					return BP_UInt8ToASCII(val, BP_NUMERIC_STRING_CONVERSIONS_STANDARD);
					break;
				case 2:
					return BP_UInt8ToASCII(val, BP_NUMERIC_STRING_CONVERSIONS_HEX_WITH_0x);
					break;
				default:
					break;
			}

			break;
		default:
			return NULL;
			break;

	}

	// return the generated buffer
	return out_buff;

}


// generate a int64 value
char * BP_Int64ToASCII(int64_t val, BP_NUMERIC_STRING_CONVERSIONS conversion)
{

	// create space to hold the buffer
	char *out_buff = NULL;

	// set random selector
	size_t rand_selection = 0;


	// switch on the conversion
	switch(conversion)
	{

		// standard %llu encoding (returns a normal number)
		case BP_NUMERIC_STRING_CONVERSIONS_STANDARD:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(int64_t) * 4)+2, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(int64_t) * 4), "%lli", val);
			break;

		// hex encoding
		case BP_NUMERIC_STRING_CONVERSIONS_HEX:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(int64_t) * 2)+1, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(int64_t) * 2), "%llx", val);
			break;

		case BP_NUMERIC_STRING_CONVERSIONS_HEX_WITH_0x:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(int64_t) * 4)+3, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(int64_t) * 4)+2, "0x%llx", val);
			break;

		// this generates a 123 or a 0xfff type string from the provided number.
		case BP_NUMERIC_STRING_CONVERSIONS_RANDOM_STANDARD_OR_0xHEX_CONVERSION:

			// make random selection based on pnrg
			rand_selection = BP_GenRandomUnsignedLong(1, 2);
			switch(rand_selection)
			{
				case 1:
					return BP_Int64ToASCII(val, BP_NUMERIC_STRING_CONVERSIONS_STANDARD);
					break;
				case 2:
					return BP_Int64ToASCII(val, BP_NUMERIC_STRING_CONVERSIONS_HEX_WITH_0x);
					break;
				default:
					break;
			}

			break;

		default:
			return NULL;
			break;

	}

	// return the generated buffer
	return out_buff;

}

char * BP_Int32ToASCII(int32_t val, BP_NUMERIC_STRING_CONVERSIONS conversion)
{

	// buffer which will be returned
	char *out_buff = NULL;

	// set random selector
	size_t rand_selection = 0;

	// switch on the conversion
	switch(conversion)
	{

		// standard %llu encoding (returns a normal number)
		case BP_NUMERIC_STRING_CONVERSIONS_STANDARD:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(int32_t) * 4)+2, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(int32_t) * 4), "%i", val);
			break;

		// hex encoding
		case BP_NUMERIC_STRING_CONVERSIONS_HEX:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(int32_t) * 2)+1, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(int32_t) * 2), "%x", val);
			break;

		case BP_NUMERIC_STRING_CONVERSIONS_HEX_WITH_0x:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(int32_t) * 4)+3, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(int32_t) * 4)+2, "0x%x", val);
			break;

		// this generates a 123 or a 0xfff type string from the provided number.
		case BP_NUMERIC_STRING_CONVERSIONS_RANDOM_STANDARD_OR_0xHEX_CONVERSION:

			// make random selection based on pnrg
			rand_selection = BP_GenRandomUnsignedLong(1, 2);
			switch(rand_selection)
			{
				case 1:
					return BP_Int32ToASCII(val, BP_NUMERIC_STRING_CONVERSIONS_STANDARD);
					break;
				case 2:
					return BP_Int32ToASCII(val, BP_NUMERIC_STRING_CONVERSIONS_HEX_WITH_0x);
					break;
				default:
					break;
			}

			break;

		default:
			return NULL;
			break;

	}

	// return the generated buffer
	return out_buff;
}

char * BP_Int16ToASCII(int16_t val, BP_NUMERIC_STRING_CONVERSIONS conversion)
{

	// create space to hold the buffer
	char *out_buff = NULL;

	// set random selector
	size_t rand_selection = 0;

	// switch on the conversion
	switch(conversion)
	{

		// standard %llu encoding (returns a normal number)
		case BP_NUMERIC_STRING_CONVERSIONS_STANDARD:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(int16_t) * 4)+2, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(int16_t) * 4), "%i", val);
			break;

		// hex encoding
		case BP_NUMERIC_STRING_CONVERSIONS_HEX:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(int16_t) * 2)+1, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(int16_t) * 2), "%x", val);
			break;

		case BP_NUMERIC_STRING_CONVERSIONS_HEX_WITH_0x:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(int16_t) * 4)+3, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(int16_t) * 4)+2, "0x%.04x", (unsigned short) val);
			break;

		// this generates a 123 or a 0xfff type string from the provided number.
		case BP_NUMERIC_STRING_CONVERSIONS_RANDOM_STANDARD_OR_0xHEX_CONVERSION:

			// make random selection based on pnrg
			rand_selection = BP_GenRandomUnsignedLong(1, 2);
			switch(rand_selection)
			{
				case 1:
					return BP_Int16ToASCII(val, BP_NUMERIC_STRING_CONVERSIONS_STANDARD);
					break;
				case 2:
					return BP_Int16ToASCII(val, BP_NUMERIC_STRING_CONVERSIONS_HEX_WITH_0x);
					break;
				default:
					break;
			}

			break;
		default:
			return NULL;
			break;

	}

	// return the generated buffer
	return out_buff;

}


char * BP_Int8ToASCII(int8_t  val,  BP_NUMERIC_STRING_CONVERSIONS conversion)
{
	// create space to hold the buffer
	char *out_buff = NULL;

	// set random selector
	size_t rand_selection = 0;

	// switch on the conversion
	switch(conversion)
	{

		// standard %llu encoding (returns a normal number)
		case BP_NUMERIC_STRING_CONVERSIONS_STANDARD:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(int8_t) * 4)+2, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(int8_t) * 4), "%i", val);
			break;

		// hex encoding
		case BP_NUMERIC_STRING_CONVERSIONS_HEX:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(int8_t) * 2)+1, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(int8_t) * 2), "%.02x", (unsigned char) val);
			break;

		case BP_NUMERIC_STRING_CONVERSIONS_HEX_WITH_0x:

			// allocate the buffer
			out_buff = (char *) bpcalloc((sizeof(int8_t) * 4)+3, 1);

			// copy in the buffer
			snprintf(out_buff, (sizeof(int8_t) * 4)+2, "0x%.02x", (unsigned char) val);
			break;

		// this generates a 123 or a 0xfff type string from the provided number.
		case BP_NUMERIC_STRING_CONVERSIONS_RANDOM_STANDARD_OR_0xHEX_CONVERSION:

			// make random selection based on pnrg
			rand_selection = BP_GenRandomUnsignedLong(1, 2);
			switch(rand_selection)
			{
				case 1:
					return BP_Int8ToASCII(val, BP_NUMERIC_STRING_CONVERSIONS_STANDARD);
					break;
				case 2:
					return BP_Int8ToASCII(val, BP_NUMERIC_STRING_CONVERSIONS_HEX_WITH_0x);
					break;
				default:
					break;
			}

			break;
		default:
			return NULL;
			break;

	}

	// return the generated buffer
	return out_buff;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Print Bits in Bytes %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to print bits in a byte
BP_ERROR_T BP_PrintBitsInByte(unsigned char in_byte)
{

	// display bits
	BP_CHECK_BIT_IS_SET(in_byte, 7) ? printf("1") : printf("0");
	BP_CHECK_BIT_IS_SET(in_byte, 6) ? printf("1") : printf("0");
	BP_CHECK_BIT_IS_SET(in_byte, 5) ? printf("1") : printf("0");
	BP_CHECK_BIT_IS_SET(in_byte, 4) ? printf("1") : printf("0");
	BP_CHECK_BIT_IS_SET(in_byte, 3) ? printf("1") : printf("0");
	BP_CHECK_BIT_IS_SET(in_byte, 2) ? printf("1") : printf("0");
	BP_CHECK_BIT_IS_SET(in_byte, 1) ? printf("1") : printf("0");
	BP_CHECK_BIT_IS_SET(in_byte, 0) ? printf("1") : printf("0");

	// return indicating success
	return ERR_SUCCESS;

}

// attept to print bits in a series of bytes (space delimited)
BP_ERROR_T BP_PrintBitsInBytes(unsigned char *in_bytes, size_t byte_count)
{

	// ensure we have a pointer and a count
	if(!in_bytes)
		return ERR_FAILURE;
	if(!byte_count)
		return ERR_FAILURE;

	// iterate through bytes and print
	size_t n = 0;
	for(; n < byte_count; n++)
	{

		// print the bits
		BP_PrintBitsInByte(in_bytes[n]);

		// add space if necessary
		if((n+1) != byte_count)
			printf(" ");

	}

	// return indicating success
	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Simple String to Number Conversion Utilties %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// returns true if the string contains only
BP_BOOL BP_StringContainsOnlyNumericASCIIDigits
(
	char  *str,
	size_t len
)
{



	// ensure we have a directory and a length
	if(!str || !len)
		return BP_FALSE;

	// attempt to verify that the string is reasonable first
	if(!BP_StringIsReasonablePrintableString(str, len, BP_FALSE, BP_FALSE))
		return BP_FALSE;



	// create ascii configuration
	BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION ascii_config;

	// zero out the structure (automagic macro)
	BP_ZERO_STACK(ascii_config);

	// set some flags in the structure to count ascii (we already counted invalid with our initial check)
	ascii_config.count_ascii                      = BP_TRUE;

	// attempt to generate analysis
	P_BP_BUFFER_ANALYSIS buff_analysis = BP_CreateBufferAnalysis
	(
			(unsigned char *) str,
			(size_t) len,
			&ascii_config
	);

	// ensure we h ave a buffer analysis
	if(!buff_analysis)
		return BP_FALSE;

	// get the digit length (number of characters in all digit places)
	size_t digit_len = \
			buff_analysis->ascii->char_counts['0'] +
			buff_analysis->ascii->char_counts['1'] +
			buff_analysis->ascii->char_counts['2'] +
			buff_analysis->ascii->char_counts['3'] +
			buff_analysis->ascii->char_counts['4'] +
			buff_analysis->ascii->char_counts['5'] +
			buff_analysis->ascii->char_counts['6'] +
			buff_analysis->ascii->char_counts['7'] +
			buff_analysis->ascii->char_counts['8'] +
			buff_analysis->ascii->char_counts['9'];


	// destroy the buffer analysis
	BP_DestroyBufferAnalysis(buff_analysis);

	// if the calculated digit length is mismatched, we have a bad buffer, exit failing here
	if(digit_len != len)
		return BP_FALSE;

	// return indicating a successful match
	return BP_TRUE;

}

// Converts a string to a signed long (base10).  Stores result in store_value.
BP_ERROR_T BP_StringToSignedLongBase10(char * str, size_t len, long * store_value)
{

	// run basic checks
	if(!str || !len)
		return ERR_FAILURE;
	if(!store_value)
		return ERR_FAILURE;

	// if there is no where to put a result, exit failure
	if(!store_value)
		return ERR_FAILURE;

	// Run digit check on string. (offset check by one byte if it's a negative number)
	if(str[0] == '-')
	{
		// check that string contains only numeric ascii digits (offset)
		if(!BP_StringContainsOnlyNumericASCIIDigits(&str[1], len-1))
			return ERR_FAILURE;
	}
	else
	{
		// check that string contains only numeric ascii digits
		if(!BP_StringContainsOnlyNumericASCIIDigits(str, len))
			return ERR_FAILURE;
	}

	// check to ensure that the string doesn't start with zero
	if(str[0] == '0')
	   return ERR_FAILURE;

	// reset errno before strtol to catch ERANGE
	errno = 0;

	// set store value from pointer reference
	long tmp_val = strtol(str, NULL, 10);
	if(errno == ERANGE || errno == EINVAL)
		return ERR_FAILURE;

	// set the storage var from the tmp value if conversion was successful
	*store_value = tmp_val;

	// return indicating success
	return ERR_SUCCESS;

}

// Converts a string to a unsigned long (base10).  Stores result in value_length.
BP_ERROR_T BP_StringToUnsignedLongBase10(char * str, size_t len, unsigned long * store_value)
{

	// run basic checks
	if(!str || !len)
		return ERR_FAILURE;
	if(!store_value)
		return ERR_FAILURE;

	// if there is no where to put a result, exit failure
	if(!store_value)
		return ERR_FAILURE;

	// check that string contains only numeric ascii digits
	if(!BP_StringContainsOnlyNumericASCIIDigits(str, len))
		return ERR_FAILURE;


	// Check to ensure that the string doesn't start with zero.  This check
	// is only valid for strings longer than 1 byte, since 0 itself is
	// a valid value.
	if(str[0] == '0' && (bpstrlen(str) > 1))
	   return ERR_FAILURE;

	// set tmp storage
	unsigned long tmp_val = 0;


	// attempt to convert
	errno = 0;
	tmp_val = strtoul(str, NULL, 10);

	// return if errno has been set
	if(errno)
		return ERR_FAILURE;

	// set the storage var from the tmp value if conversion was successful
	*store_value = tmp_val;

	// return indicating success
	return ERR_SUCCESS;

}

// This will convert a string such as 0xCAFI EBABE in ascii, to a binary string.  The length
// of the new bpcalloced buffer (returned pointer) will be stored in out_blob_length if provided.
char * BP_StringHexToBinaryBlob(char *ascii_hex_string_to_convert, size_t *out_blob_length)
{

	// run basic checks
	if(!ascii_hex_string_to_convert)
		return NULL;
	if(!BP_StringIsReasonablePrintableString(ascii_hex_string_to_convert, bpstrlen(ascii_hex_string_to_convert), BP_FALSE, BP_FALSE))
		return NULL;

	// calculate string length
	size_t in_str_len = bpstrlen(ascii_hex_string_to_convert);

	// string pointer to use
	char *in_str_ptr = NULL;

	// skip 0x prefix if it's set
	if(in_str_len >= 2)
	{

		if(memcmp("0x", ascii_hex_string_to_convert, 2) == 0)
		{
			in_str_ptr = &ascii_hex_string_to_convert[2];
			in_str_len -= 2;
		}
		else
		{
			in_str_ptr = ascii_hex_string_to_convert;
		}

	}
	else
	{
		in_str_ptr = ascii_hex_string_to_convert;
	}

	// attempt conversion to lowercase hexidecimal
	char *converted_lower_hex = BP_StringToLowerCase(in_str_ptr, in_str_len);
	if(!converted_lower_hex)
		return NULL;


	// calculate the new length
	size_t converted_lower_hex_len = bpstrlen(converted_lower_hex);

	// if the string is not reasonable for lower case hex
	if(!BP_StringIsReasonableForHexLowerASCII(converted_lower_hex, converted_lower_hex_len))
	{
		bpfree(converted_lower_hex);
		return NULL;
	}

	// calculate if we have a remainder
	size_t remainder_modulo                = converted_lower_hex_len % 2;
	size_t byte_len_not_counting_remainder = converted_lower_hex_len / 2;

	// create out buffer
	char *final_out_buff = (char *) bpcalloc(byte_len_not_counting_remainder+remainder_modulo+1, 1);

	// set current character
	unsigned long curr_char = 0;

	// create string position iter
	size_t str_pos_iter = 0;

	// index for the output string
	size_t out_str_idx = 0;

	// iterate through the string and switch on the character
	for(; str_pos_iter < converted_lower_hex_len; str_pos_iter+=2)
	{

		// set current pointer index
		unsigned char *curr_ptr_idx = (unsigned char *) &converted_lower_hex[str_pos_iter];

		// create and zero a temporary hex buffer
		unsigned char tmp_hex_buff[3];
		BP_ZERO_STACK(tmp_hex_buff);


		// set temporary hex buffer
		tmp_hex_buff[0] = (unsigned char) curr_ptr_idx[0];
		tmp_hex_buff[1] = (unsigned char) curr_ptr_idx[1];
		tmp_hex_buff[2] = (unsigned char) 0x00;

		// set the current character
		curr_char = strtoul((char *) &tmp_hex_buff, NULL, 16);

		// set the current character
		final_out_buff[out_str_idx] = (unsigned char) curr_char;
		out_str_idx++;

	}

	// destroy the converted lower hex
	bpfree(converted_lower_hex);

	// set the blob length if provided
	if(out_blob_length)
		*out_blob_length = out_str_idx;

	// return the final converted buffer
	return final_out_buff;

}


// checks if a string is ascii hex only
BP_BOOL BP_StringIsAsciiHex(char *str_value)
{

	// ensure string is reasonable/printable
	if(!BP_StringIsReasonablePrintableString(str_value, bpstrlen(str_value), BP_FALSE, BP_FALSE))
		return BP_FALSE;

	// string value
	size_t str_len = bpstrlen(str_value);



	// return indicating success
	return BP_TRUE;
}

// ==== Regular Expression URL Parsing ===============


// parses a string to a url
char * BP_ParseStringToURLPCRE(char *long_form_url, BP_BOOL require_valid_tld_match)
{


	if(!long_form_url)
		return NULL;

	// create zeroed out inout structure
	BP_REGEX_MATCH_INFO match_info = {0};

	// get matches
	size_t matched = 0;

	// check if the user specified whether or not to do forced tld matching
	if(require_valid_tld_match)
		matched = BP_MatchRegularExpressionPCRE(long_form_url, bpstrlen(long_form_url), GENERIC_MATCH_URL_FROM_START_PCRE,           PCRE_CASELESS, 1024, &match_info, true);
	else
		matched = BP_MatchRegularExpressionPCRE(long_form_url, bpstrlen(long_form_url), GENERIC_MATCH_URL_FROM_START_NON_STRICT_TLD, PCRE_CASELESS, 1024, &match_info, true);


	// set the return string default to null
	char *ret_string = NULL;

	// set matched
	if(matched == ERR_SUCCESS)
	{

		// check for the matches
		size_t n = 0;
		for(; n < match_info.pcre_num_matched; n++)
		{

			// display matched string
			ret_string = bpstrdup(match_info.preg_matches->matched_string);
			break;

		}

	}

	// destroy the match info
	BP_DestroyRegexpMatchInfo(&match_info);

	// return null for failure
	return ret_string;

}

// parses a url to a base using pcre
char * BP_ParseURLToHostPCRE(char *long_form_url, BP_BOOL require_dot_in_path_for_match)
{

	// parse to base first
	char * base = BP_ParseURLToBasePCRE(long_form_url);
	if(!base)
		return NULL;

	// path matching regexp
	char * regexp = NULL;

	if(require_dot_in_path_for_match)
		regexp = GENERIC_MATCH_HOST_FROM_BASE_REQUIRE_DOT;
	else
		regexp = GENERIC_MATCH_HOST_FROM_BASE_NO_DOT_REQUIRED;

	// create zeroed out inout structure
	BP_REGEX_MATCH_INFO match_info = {0};

	// get matches
	size_t matched = BP_MatchRegularExpressionPCRE(long_form_url, bpstrlen(long_form_url), regexp, PCRE_CASELESS, 1024, &match_info, true);

	// set the return string default to null
	char *ret_string = NULL;

	// set matched
	if(matched == ERR_SUCCESS)
	{

		if(require_dot_in_path_for_match)
		{
			// check for the matches
			size_t n = 0;
			for(; n < match_info.pcre_num_matched; n++)
			{

				// display matched string
				ret_string = bpstrdup(match_info.preg_matches->matched_string);
				break;

			}
		}
		else
		{

			// if no dot is required, it will be the second match
			// as the first match will be the http/ftp/whatever string
			// due to extremely loose matching constraints.

			// printf("\n Got Matched String?: %s", match_info.preg_matches[1].matched_string);

			// if we don't have at least two matches, we have a bad parse
			if(match_info.pcre_num_matched >= 2)
			{
				ret_string = bpstrdup(match_info.preg_matches[1].matched_string);


			}




		}

	}
	else
	{
		// HOST MATCH FAILED
	}

	// destroy base
	bpfree(base);

	// destroy the match info
	BP_DestroyRegexpMatchInfo(&match_info);

	// return null for failure
	return ret_string;

}


// parses a url to a base using pcre
char * BP_ParseURLToBasePCRE(char *long_form_url, BP_BOOL require_valid_tld)
{

	if(!long_form_url)
		return NULL;
	if(!BP_StringIsReasonablePrintableString(long_form_url, bpstrlen(long_form_url), BP_FALSE, BP_FALSE))
		return NULL;


	// old path regexp
	// char * regexp = "[hH][tT][tT][pP][sS]?://(?:[\\w-]+\\.)(?:\\.?[\\w]{2,})+";;

	// new regexp
	char * regexp = NULL;

	// run valid tld match
	if(require_valid_tld)
		regexp = GENERIC_MATCH_BASE_FROM_START_REQUIRE_TLD;
	else
		regexp = GENERIC_MATCH_BASE_FROM_START_TLD_NOT_REQUIRED;


	// create zeroed out inout structure
	BP_REGEX_MATCH_INFO match_info = {0};

	// get matches (ignore case)
	size_t matched = BP_MatchRegularExpressionPCRE(long_form_url, bpstrlen(long_form_url), regexp, PCRE_CASELESS, 1024, &match_info, true);

	// set the return string default to null
	char *ret_string = NULL;

	// set matched
	if(matched == ERR_SUCCESS)
	{

		//printf("\n BASE MATCHED?!!!");
		// check for the matches
		size_t n = 0;
		for(; n < match_info.pcre_num_matched; n++)
		{

			//printf("\n BASE MATCHED?!!!: %s", match_info.preg_matches->matched_string);

			// display matched string
			ret_string = bpstrdup(match_info.preg_matches->matched_string);
			break;

		}

	}

	// destroy the match info
	BP_DestroyRegexpMatchInfo(&match_info);

	// return null for failure
	return ret_string;

}

// parses a url to a path using pcre
char * BP_ParseURLToPathPCRE(char *long_form_url, BP_BOOL require_valid_tld)
{

	// ensure we have a parameter
	if(!long_form_url)
		return NULL;
	if(!BP_StringIsReasonablePrintableString(long_form_url, bpstrlen(long_form_url), BP_FALSE, BP_FALSE))
		return NULL;

	// path matching regexp
	// char * regexp = "[hH][tT][tT][pP][sS]?://(?:[\\w-]+\\.)(?:\\.?[\\w-]{2,})+(?:[\\/~\\w\\-0-9]+)[\\/]";

	// new regexp matching iana standards domains
	// char * regexp = "[a-zA-Z]{3,}\\:\\/\\/(?:[\\w-]+\\.)+(?:construction|photography|enterprises|contractors|technology|equipment|directory|ventures|plumbing|lighting|holdings|graphics|diamonds|clothing|singles|kitchen|gallery|voyage|travel|tattoo|museum|estate|camera|today|tips|sexy|post|name|mobi|menu|land|jobs|info|guru|coop|bike|asia|arpa|aero|xxx|uno|tel|pro|org|net|mil|int|gov|edu|com|cat|biz|zw|zm|za|yt|ye|ws|wf|vu|vn|vi|vg|ve|vc|va|uz|uy|us|uk|ug|ua|tz|tw|tv|tt|tr|tp|to|tn|tm|tl|tk|tj|th|tg|tf|td|tc|sz|sy|sx|sv|su|st|sr|so|sn|sm|sl|sk|sj|si|sh|sg|se|sd|sc|sb|sa|rw|ru|rs|ro|re|qa|py|pw|pt|ps|pr|pn|pm|pl|pk|ph|pg|pf|pe|pa|om|nz|nu|nr|np|no|nl|ni|ng|nf|ne|nc|na|mz|my|mx|mw|mv|mu|mt|ms|mr|mq|mp|mo|mn|mm|ml|mk|mh|mg|me|md|mc|ma|ly|lv|lu|lt|ls|lr|lk|li|lc|lb|la|kz|ky|kw|kr|kp|kn|km|ki|kh|kg|ke|jp|jo|jm|je|it|is|ir|iq|io|in|im|il|ie|id|hu|ht|hr|hn|hm|hk|gy|gw|gu|gt|gs|gr|gq|gp|gn|gm|gl|gi|gh|gg|gf|ge|gd|gb|ga|fr|fo|fm|fk|fj|fi|eu|et|es|er|eg|ee|ec|dz|do|dm|dk|dj|de|cz|cy|cx|cw|cv|cu|cr|co|cn|cm|cl|ck|ci|ch|cg|cf|cd|cc|ca|bz|by|bw|bv|bt|bs|br|bo|bn|bm|bj|bi|bh|bg|bf|be|bd|bb|ba|az|ax|aw|au|at|as|ar|aq|ao|an|am|al|ai|ag|af|ae|ad|ac)+ [\\/]";
	// char * regexp =  "[a-zA-Z]{3,}\\:\\/\\/(?:[a-zA-Z0-9-@\\:]+\\.|[a-zA-Z0-9-@\\:]+\\.{1,})+(?:construction|photography|enterprises|contractors|technology|equipment|directory|ventures|plumbing|lighting|holdings|graphics|diamonds|clothing|singles|kitchen|gallery|voyage|travel|tattoo|museum|estate|camera|today|tips|sexy|post|name|mobi|menu|land|jobs|info|guru|coop|bike|asia|arpa|aero|xxx|uno|tel|pro|org|net|mil|int|gov|edu|com|cat|biz|zw|zm|za|yt|ye|ws|wf|vu|vn|vi|vg|ve|vc|va|uz|uy|us|uk|ug|ua|tz|tw|tv|tt|tr|tp|to|tn|tm|tl|tk|tj|th|tg|tf|td|tc|sz|sy|sx|sv|su|st|sr|so|sn|sm|sl|sk|sj|si|sh|sg|se|sd|sc|sb|sa|rw|ru|rs|ro|re|qa|py|pw|pt|ps|pr|pn|pm|pl|pk|ph|pg|pf|pe|pa|om|nz|nu|nr|np|no|nl|ni|ng|nf|ne|nc|na|mz|my|mx|mw|mv|mu|mt|ms|mr|mq|mp|mo|mn|mm|ml|mk|mh|mg|me|md|mc|ma|ly|lv|lu|lt|ls|lr|lk|li|lc|lb|la|kz|ky|kw|kr|kp|kn|km|ki|kh|kg|ke|jp|jo|jm|je|it|is|ir|iq|io|in|im|il|ie|id|hu|ht|hr|hn|hm|hk|gy|gw|gu|gt|gs|gr|gq|gp|gn|gm|gl|gi|gh|gg|gf|ge|gd|gb|ga|fr|fo|fm|fk|fj|fi|eu|et|es|er|eg|ee|ec|dz|do|dm|dk|dj|de|cz|cy|cx|cw|cv|cu|cr|co|cn|cm|cl|ck|ci|ch|cg|cf|cd|cc|ca|bz|by|bw|bv|bt|bs|br|bo|bn|bm|bj|bi|bh|bg|bf|be|bd|bb|ba|az|ax|aw|au|at|as|ar|aq|ao|an|am|al|ai|ag|af|ae|ad|ac)(?:[:0-9]?)+(?:\\/[\\/~\\w\\-0-9]+\\/|\\/)";
	// "[a-zA-Z0-9\-]{1,}\\:\\/\\/[\\!\\:\\@\\.a-zA-Z0-9\\-]{1,}(?:[\\.a-zA-Z0-9\\-]{1,})(?:\\.construction|\\.photography|\\.enterprises|\\.contractors|\\.technology|\\.equipment|\\.directory|\\.ventures|\\.plumbing|\\.lighting|\\.holdings|\\.graphics|\\.diamonds|\\.clothing|\\.singles|\\.kitchen|\\.gallery|\\.voyage|\\.travel|\\.tattoo|\\.museum|\\.estate|\\.camera|\\.today|\\.tips|\\.sexy|\\.post|\\.name|\\.mobi|\\.menu|\\.land|\\.jobs|\\.info|\\.guru|\\.coop|\\.bike|\\.asia|\\.arpa|\\.aero|\\.xxx|\\.uno|\\.tel|\\.pro|\\.org|\\.net|\\.mil|\\.int|\\.gov|\\.edu|\\.com|\\.cat|\\.biz|\\.zw|\\.zm|\\.za|\\.yt|\\.ye|\\.ws|\\.wf|\\.vu|\\.vn|\\.vi|\\.vg|\\.ve|\\.vc|\\.va|\\.uz|\\.uy|\\.us|\\.uk|\\.ug|\\.ua|\\.tz|\\.tw|\\.tv|\\.tt|\\.tr|\\.tp|\\.to|\\.tn|\\.tm|\\.tl|\\.tk|\\.tj|\\.th|\\.tg|\\.tf|\\.td|\\.tc|\\.sz|\\.sy|\\.sx|\\.sv|\\.su|\\.st|\\.sr|\\.so|\\.sn|\\.sm|\\.sl|\\.sk|\\.sj|\\.si|\\.sh|\\.sg|\\.se|\\.sd|\\.sc|\\.sb|\\.sa|\\.rw|\\.ru|\\.rs|\\.ro|\\.re|\\.qa|\\.py|\\.pw|\\.pt|\\.ps|\\.pr|\\.pn|\\.pm|\\.pl|\\.pk|\\.ph|\\.pg|\\.pf|\\.pe|\\.pa|\\.om|\\.nz|\\.nu|\\.nr|\\.np|\\.no|\\.nl|\\.ni|\\.ng|\\.nf|\\.ne|\\.nc|\\.na|\\.mz|\\.my|\\.mx|\\.mw|\\.mv|\\.mu|\\.mt|\\.ms|\\.mr|\\.mq|\\.mp|\\.mo|\\.mn|\\.mm|\\.ml|\\.mk|\\.mh|\\.mg|\\.me|\\.md|\\.mc|\\.ma|\\.ly|\\.lv|\\.lu|\\.lt|\\.ls|\\.lr|\\.lk|\\.li|\\.lc|\\.lb|\\.la|\\.kz|\\.ky|\\.kw|\\.kr|\\.kp|\\.kn|\\.km|\\.ki|\\.kh|\\.kg|\\.ke|\\.jp|\\.jo|\\.jm|\\.je|\\.it|\\.is|\\.ir|\\.iq|\\.io|\\.in|\\.im|\\.il|\\.ie|\\.id|\\.hu|\\.ht|\\.hr|\\.hn|\\.hm|\\.hk|\\.gy|\\.gw|\\.gu|\\.gt|\\.gs|\\.gr|\\.gq|\\.gp|\\.gn|\\.gm|\\.gl|\\.gi|\\.gh|\\.gg|\\.gf|\\.ge|\\.gd|\\.gb|\\.ga|\\.fr|\\.fo|\\.fm|\\.fk|\\.fj|\\.fi|\\.eu|\\.et|\\.es|\\.er|\\.eg|\\.ee|\\.ec|\\.dz|\\.do|\\.dm|\\.dk|\\.dj|\\.de|\\.cz|\\.cy|\\.cx|\\.cw|\\.cv|\\.cu|\\.cr|\\.co|\\.cn|\\.cm|\\.cl|\\.ck|\\.ci|\\.ch|\\.cg|\\.cf|\\.cd|\\.cc|\\.ca|\\.bz|\\.by|\\.bw|\\.bv|\\.bt|\\.bs|\\.br|\\.bo|\\.bn|\\.bm|\\.bj|\\.bi|\\.bh|\\.bg|\\.bf|\\.be|\\.bd|\\.bb|\\.ba|\\.az|\\.ax|\\.aw|\\.au|\\.at|\\.as|\\.ar|\\.aq|\\.ao|\\.an|\\.am|\\.al|\\.ai|\\.ag|\\.af|\\.ae|\\.ad|\\.ac){1,}(?:[\\:\\/0-9]{0,})\\/(?:[0-9a-zA-Z\\-\\~]{1,}[\\/])";

	char * regexp = NULL;
	if(require_valid_tld)
		regexp = GENERIC_MATCH_PATH_FROM_START_REQUIRE_TLD;
	else
		regexp = GENERIC_MATCH_PATH_FROM_START_TLD_NOT_REQUIRED;


	//regexp = "(?:([a-zA-Z0-9\-]{1,10}))(?:([:][\\/][\\/]{1,1}))";
	regexp = "[a-zA-Z]{1,}\\:\\/\\/[\\!\\:\\@\\.a-zA-Z0-9\\-]{1,}(?:[\\.a-zA-Z0-9\\-]{1,})(?:[\\:\\/0-9]{0,})\\/(?:([\\.\\_\\/\\~0-9a-zA-Z]){0,})\\/";

	// create zeroed out inout structure
	BP_REGEX_MATCH_INFO match_info;
	BP_ZERO_STACK(match_info);

	int regexp_flags    = 0;
	regexp_flags           |= PCRE_MULTILINE;
	regexp_flags           |= PCRE_DOTALL;


	// get matches
	size_t matched = BP_MatchRegularExpressionPCRE(long_form_url, bpstrlen(long_form_url), regexp, regexp_flags, 3, (P_BP_REGEX_MATCH_INFO) &match_info, true);

	// set the return string default to null
	char *ret_string = NULL;

	// set matched
	if(matched == ERR_SUCCESS)
	{

		// check for the matches
		size_t n = 0;
		for(; n < match_info.pcre_num_matched; n++)
		{

			// allocate space for data
			char *ret = (char *) bpcalloc(match_info.preg_matches[n].match_len+16, 1);

			// copy data in
			memcpy(ret, match_info.preg_matches[n].matched_string, match_info.preg_matches[n].match_len);
			ret_string = ret;
			break;

		}

	}
	else
	{
		// printf("\n Path not matched: %s - %s", regexp, long_form_url);

	}

	// destroy the match info
	BP_DestroyRegexpMatchInfo(&match_info);

	// if we can't parse it to a path, try to parse it to a base
	if(!ret_string)
	{
		ret_string = BP_ParseURLToBasePCRE(long_form_url, require_valid_tld);
	}

	// return null for failure
	return ret_string;

}

// parses a url to a script using pcre
char * BP_ParseURLToScriptPCRE(char *long_form_url, BP_BOOL require_valid_tld)
{

	// ensure we have a url to parse
	if(!long_form_url)
		return NULL;

	// path matching regexp
	// char * regexp = "[a-zA-Z]{3,}\\:\\/\\/(?:[\\w-]+\\.)+(?:construction|photography|enterprises|contractors|technology|equipment|directory|ventures|plumbing|lighting|holdings|graphics|diamonds|clothing|singles|kitchen|gallery|voyage|travel|tattoo|museum|estate|camera|today|tips|sexy|post|name|mobi|menu|land|jobs|info|guru|coop|bike|asia|arpa|aero|xxx|uno|tel|pro|org|net|mil|int|gov|edu|com|cat|biz|zw|zm|za|yt|ye|ws|wf|vu|vn|vi|vg|ve|vc|va|uz|uy|us|uk|ug|ua|tz|tw|tv|tt|tr|tp|to|tn|tm|tl|tk|tj|th|tg|tf|td|tc|sz|sy|sx|sv|su|st|sr|so|sn|sm|sl|sk|sj|si|sh|sg|se|sd|sc|sb|sa|rw|ru|rs|ro|re|qa|py|pw|pt|ps|pr|pn|pm|pl|pk|ph|pg|pf|pe|pa|om|nz|nu|nr|np|no|nl|ni|ng|nf|ne|nc|na|mz|my|mx|mw|mv|mu|mt|ms|mr|mq|mp|mo|mn|mm|ml|mk|mh|mg|me|md|mc|ma|ly|lv|lu|lt|ls|lr|lk|li|lc|lb|la|kz|ky|kw|kr|kp|kn|km|ki|kh|kg|ke|jp|jo|jm|je|it|is|ir|iq|io|in|im|il|ie|id|hu|ht|hr|hn|hm|hk|gy|gw|gu|gt|gs|gr|gq|gp|gn|gm|gl|gi|gh|gg|gf|ge|gd|gb|ga|fr|fo|fm|fk|fj|fi|eu|et|es|er|eg|ee|ec|dz|do|dm|dk|dj|de|cz|cy|cx|cw|cv|cu|cr|co|cn|cm|cl|ck|ci|ch|cg|cf|cd|cc|ca|bz|by|bw|bv|bt|bs|br|bo|bn|bm|bj|bi|bh|bg|bf|be|bd|bb|ba|az|ax|aw|au|at|as|ar|aq|ao|an|am|al|ai|ag|af|ae|ad|ac)+";

	// new regexp (awesome regexp)
	// char * regexp = "[a-zA-Z]{3,}\\:\\/\\/(?:[a-zA-Z0-9-@\\:]+\\.|[a-zA-Z0-9-@\\:]+\\.{1,})+(?:construction|photography|enterprises|contractors|technology|equipment|directory|ventures|plumbing|lighting|holdings|graphics|diamonds|clothing|singles|kitchen|gallery|voyage|travel|tattoo|museum|estate|camera|today|tips|sexy|post|name|mobi|menu|land|jobs|info|guru|coop|bike|asia|arpa|aero|xxx|uno|tel|pro|org|net|mil|int|gov|edu|com|cat|biz|zw|zm|za|yt|ye|ws|wf|vu|vn|vi|vg|ve|vc|va|uz|uy|us|uk|ug|ua|tz|tw|tv|tt|tr|tp|to|tn|tm|tl|tk|tj|th|tg|tf|td|tc|sz|sy|sx|sv|su|st|sr|so|sn|sm|sl|sk|sj|si|sh|sg|se|sd|sc|sb|sa|rw|ru|rs|ro|re|qa|py|pw|pt|ps|pr|pn|pm|pl|pk|ph|pg|pf|pe|pa|om|nz|nu|nr|np|no|nl|ni|ng|nf|ne|nc|na|mz|my|mx|mw|mv|mu|mt|ms|mr|mq|mp|mo|mn|mm|ml|mk|mh|mg|me|md|mc|ma|ly|lv|lu|lt|ls|lr|lk|li|lc|lb|la|kz|ky|kw|kr|kp|kn|km|ki|kh|kg|ke|jp|jo|jm|je|it|is|ir|iq|io|in|im|il|ie|id|hu|ht|hr|hn|hm|hk|gy|gw|gu|gt|gs|gr|gq|gp|gn|gm|gl|gi|gh|gg|gf|ge|gd|gb|ga|fr|fo|fm|fk|fj|fi|eu|et|es|er|eg|ee|ec|dz|do|dm|dk|dj|de|cz|cy|cx|cw|cv|cu|cr|co|cn|cm|cl|ck|ci|ch|cg|cf|cd|cc|ca|bz|by|bw|bv|bt|bs|br|bo|bn|bm|bj|bi|bh|bg|bf|be|bd|bb|ba|az|ax|aw|au|at|as|ar|aq|ao|an|am|al|ai|ag|af|ae|ad|ac)(?:[:0-9]?)+";
	char * regexp = NULL;
	if(require_valid_tld)
		regexp = GENERIC_MATCH_SCRIPT_FROM_URL_REQUIRE_TLD;
	else
		regexp = GENERIC_MATCH_SCRIPT_FROM_URL_TLD_NOT_REQUIRED;

	// printf("\n Using script regex?: %s", regexp);

	// create zeroed out inout structure
	BP_REGEX_MATCH_INFO match_info = {0};

	// get matches
	size_t matched = BP_MatchRegularExpressionPCRE(long_form_url, bpstrlen(long_form_url), regexp, 0, 1, &match_info, true);

	// set the return string default to null
	char *ret_string = NULL;

	// set matched
	if(matched == ERR_SUCCESS)
	{

		// check for the matches
		size_t n = 0;
		for(; n < match_info.pcre_num_matched; n++)
		{

			// display matched string
			ret_string = bpstrdup(match_info.preg_matches->matched_string);
			break;

		}

	}

	// destroy the match info
	BP_DestroyRegexpMatchInfo(&match_info);


	if(ret_string)
	{

		char * tmp_path = BP_ParseURLToPathPCRE(long_form_url);
		if(tmp_path)
		{
			// check path len
			size_t path_len = bpstrlen(tmp_path);

			// check ret length
			size_t ret_len = bpstrlen(ret_string);

			// if the ret length is less than the path, we have
			// a mismatch, so just return the path.
			if(path_len > ret_len)
			{
				// printf("\n Overwriting: %s - %s", ret_string, tmp_path);
				bpfree(ret_string);

				// set the tmp path
				ret_string = tmp_path;

			}
			else
			{
				bpfree(tmp_path);
			}



		}

	}


	// return null for failure
	return ret_string;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Regular Expression String Type Matching %%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// string conforms to a basic URL
BP_ERROR_T BP_StringIsURL(char *str_value, BP_BOOL require_valid_tld_match)
{

	// ensure we have a string value
	if(!str_value)
		return ERR_FAILURE;

	// ensure the value has length
	if(!strnlen(str_value, 1))
		return ERR_FAILURE;

	// determine string length
	size_t len = bpstrlen(str_value);

	// attempt to parse the value to a url
	char * parsed_to_url = BP_ParseStringToURLPCRE(str_value, require_valid_tld_match);

	// printf("\n Parsed to url?: %s - %s", str_value, parsed_to_url);

	// attempt to parse to url
	if(!parsed_to_url)
		return ERR_FAILURE;

	// if the parsed length is not matched to length, we know we
	// have unmatched data.
	if(bpstrlen(parsed_to_url) != len)
	{
		free(parsed_to_url);
		return ERR_FAILURE;
	}

	// run a memory comparisson to ensure values match
	if(memcmp(parsed_to_url, str_value, len) != 0)
	{
		free(parsed_to_url);
		return ERR_FAILURE;
	}

	// destroy the parsed string
	free(parsed_to_url);

	// return indicating success
	return ERR_SUCCESS;

}

// string conforms to a host semantic
BP_ERROR_T BP_StringIsHostURL(char *str_value)
{


	// ensure we have a string value
	if(!str_value)
		return ERR_FAILURE;

	// ensure the value has length
	if(!strnlen(str_value, 1))
		return ERR_FAILURE;

	// determine string length
	size_t len = bpstrlen(str_value);

	// attempt to parse the value to a url
	char * parsed = BP_ParseURLToHostPCRE(str_value);

	// ensure that value was parsed
	if(!parsed)
		return ERR_FAILURE;

	// if the parsed length is not matched to length, we know we
	// have unmatched data.
	if(bpstrlen(parsed) != len)
	{
		free(parsed);
		return ERR_FAILURE;
	}

	// if memcmp fails, exit with failure status
	if(memcmp(parsed, str_value, len) != 0)
	{
		free(parsed);
		return ERR_FAILURE;
	}

	// destroy parsed string
	free(parsed);

	// return indicating success
	return ERR_SUCCESS;

}

// string conforms to a base semantic
BP_ERROR_T BP_StringIsBaseURL(char *str_value)
{

	// ensure we have a string value
	if(!str_value)
		return ERR_FAILURE;

	// ensure the value has length
	if(!strnlen(str_value, 1))
		return ERR_FAILURE;

	// determine string length
	size_t len = bpstrlen(str_value);

	// attempt to parse the value to a base
	char * parsed = BP_ParseURLToBasePCRE(str_value);

	// ensure that value was parsed
	if(!parsed)
		return ERR_FAILURE;

	// if the parsed length is not matched to length, we know we
	// have unmatched data.
	if(bpstrlen(parsed) != len)
	{
		free(parsed);
		return ERR_FAILURE;
	}

	// if memcmp fails, exit with failure status
	if(memcmp(parsed, str_value, len) != 0)
	{
		free(parsed);
		return ERR_FAILURE;
	}

	// destroy parsed string
	free(parsed);

	// return indicating success
	return ERR_SUCCESS;

	// return indicating success
	return ERR_SUCCESS;

}

// string conforms to a script semantic
BP_ERROR_T BP_StringIsScriptURL(char * str_value)
{

	// ensure we have a string value
	if(!str_value)
		return ERR_FAILURE;

	// ensure the value has length
	if(!strnlen(str_value, 1))
		return ERR_FAILURE;

	// determine string length
	size_t len = bpstrlen(str_value);

	// attempt to parse the value to a script
	char * parsed = BP_ParseURLToScriptPCRE(str_value);

	// ensure that value was parsed
	if(!parsed)
		return ERR_FAILURE;

	// if the parsed length is not matched to length, we know we
	// have unmatched data.
	if(bpstrlen(parsed) != len)
	{
		free(parsed);
		return ERR_FAILURE;
	}

	// if memcmp fails, exit with failure status
	if(memcmp(parsed, str_value, len) != 0)
	{
		free(parsed);
		return ERR_FAILURE;
	}

	// destroy parsed string
	free(parsed);

	// return indicating success
	return ERR_SUCCESS;

}
// string conforms to a path semantic
BP_ERROR_T BP_StringIsPathURL(char * str_value)
{

	// ensure we have a string value
	if(!str_value)
		return ERR_FAILURE;

	// ensure the value has length
	if(!strnlen(str_value, 1))
		return ERR_FAILURE;

	// determine string length
	size_t len = bpstrlen(str_value);

	// attempt to parse the value to a script
	char * parsed = BP_ParseURLToPathPCRE(str_value);

	// ensure that value was parsed
	if(!parsed)
		return ERR_FAILURE;

	// if the parsed length is not matched to length, we know we
	// have unmatched data.
	if(bpstrlen(parsed) != len)
	{
		free(parsed);
		return ERR_FAILURE;
	}

	// if memcmp fails, exit with failure status
	if(memcmp(parsed, str_value, len) != 0)
	{
		free(parsed);
		return ERR_FAILURE;
	}

	// destroy parsed string
	free(parsed);

	// return indicating success
	return ERR_SUCCESS;

}

// checks if a string is an IPv4 address (doesn't use preg)
BP_ERROR_T BP_StringIsIPv4Address(char * str_value)
{

	// ensure we have a value
	if(!str_value)
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(str_value, bpstrlen(str_value), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Analyze Buffer %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	 // create ascii configuration
	BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION ascii_config;

	// zero out the structure (automagic macro)
	BP_ZERO_STACK(ascii_config);

	// set some flags in the structure (default is BP_FALSE aka 0)
	ascii_config.count_ascii                      = BP_TRUE;
	ascii_config.count_extended                   = BP_TRUE;
	ascii_config.count_individual_characters      = BP_TRUE;

	// attempt to generate analysis
	P_BP_BUFFER_ANALYSIS buff_analysis = BP_CreateBufferAnalysis
	(
			(unsigned char *) str_value,
			bpstrlen(str_value),
			&ascii_config
	);

	// ensure we h ave a buffer analysis
	if(!buff_analysis)
	{
			BP_DestroyBufferAnalysis(buff_analysis);
			return ERR_FAILURE;
	}

	// must have three periods in it or we've failed sanity
	if(buff_analysis->ascii->char_counts['.'] != 3)
	{
		BP_DestroyBufferAnalysis(buff_analysis);
		return ERR_FAILURE;
	}

	// check if string is just numbers and periods
	BP_STRING_CONFORMITY_FLAGS_T c_flags = BP_StringConformsAs
	(
			str_value,
			bpstrlen(str_value),
			".",
			1
	);

	// ensure we have conformity flags
	if(!c_flags)
	{
		BP_DestroyBufferAnalysis(buff_analysis);
		return ERR_FAILURE;
	}

	// ensure it's numeric only except for the dots
	if(!BP_FLAG_IS_SET(c_flags, BP_STRING_CONFORMS_AS_NUMERIC_ONLY))
	{
		BP_DestroyBufferAnalysis(buff_analysis);
		return ERR_FAILURE;
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Specific Analysis %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ip addresses can't be longer than 15 chars long
	size_t max_ip_len = 15;

	// calculate the string length
	size_t string_len = bpstrlen(str_value);

	// if the string length is greater than the max for an ip address, fail
	if(string_len > max_ip_len)
	{
		BP_DestroyBufferAnalysis(buff_analysis);
		return ERR_FAILURE;
	}

	// digit length
	size_t digit_len = 0;

	// current read position
	char *curr_read_pos = str_value;

	// octet val
	unsigned long octet_val = 0;

	// octet count
	size_t octet_counter = 0;

	// temporary buffer used for octet conversion
	char tmp_buff[16];
	BP_ZERO_STACK(tmp_buff);


	// iterate through string and check values
	size_t str_iter = 0;
	do
	{


		// set digit length/octet val to zero on loop
		digit_len = 0;
		octet_val = 0;

		// zero out the octet buffer on loop
		BP_ZERO_STACK(tmp_buff);



		// first char of a read should always be a number
		if(!isdigit(curr_read_pos[0]))
		{
			BP_DestroyBufferAnalysis(buff_analysis);
			return ERR_FAILURE;
		}

		// calculate length of first octet
		if(isdigit(curr_read_pos[digit_len]))
			digit_len++;
		if(isdigit(curr_read_pos[digit_len]))
			digit_len++;
		if(isdigit(curr_read_pos[digit_len]))
			digit_len++;

		// if the fourth item isn't a digit, we have a bad value and can exit here
		if(isdigit(curr_read_pos[digit_len]))
		{
			BP_DestroyBufferAnalysis(buff_analysis);
			return ERR_FAILURE;
		}


		// copy data into the buffer
		memcpy((char *) &tmp_buff, curr_read_pos, digit_len);

		// attempt to convert value to base 10
		if(!BP_StringToUnsignedLongBase10(tmp_buff, digit_len, &octet_val))
		{
			BP_DestroyBufferAnalysis(buff_analysis);
			return ERR_FAILURE;
		}

		// if the octet value is ever > 255, return failure
		if(octet_val > 255)
		{
			BP_DestroyBufferAnalysis(buff_analysis);
			return ERR_FAILURE;
		}

		// set the current read position reference on loop
		curr_read_pos = (char *) &curr_read_pos[digit_len+1];

		// increment the octet counter on loop
		octet_counter++;

	} while(octet_counter < 4);

	// destroy buffer analysis before exit
	BP_DestroyBufferAnalysis(buff_analysis);

	// return indicating success
	return ERR_SUCCESS;

}


// uses regular expressions to determine if a string is a valid email address.
BP_ERROR_T BP_StringIsValidEmailAddress(char *str_value)
{

	// ensure we have a string value
	if(!str_value)
		return ERR_FAILURE;

	// ensure value is reasonable printable string
	if(!BP_StringIsReasonablePrintableString(str_value, bpstrlen(str_value)))
		return ERR_FAILURE;

	// calculate length after check
	char len = bpstrlen(str_value);
	if(len < 6)
		return ERR_FAILURE;

	// rfc 5322 states that the whole address can never be > 254
	if(len > 254)
		return ERR_FAILURE;


	// must start with and end with alphanumeric values
	if(!isalnum(str_value[len-1]))
		return ERR_FAILURE;
	if(!isalnum(str_value[0]))
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Buffer Analysis %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create ascii configuration
	BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION ascii_config;

	// zero out the structure (automagic macro)
	BP_ZERO_STACK(ascii_config);

	// set all counters to true
	ascii_config.count_ascii                      = BP_TRUE;
	ascii_config.count_extended                   = BP_TRUE;
	ascii_config.count_individual_characters      = BP_TRUE;


	// attempt to generate buffer analysis
	P_BP_BUFFER_ANALYSIS buff_analysis = BP_CreateBufferAnalysis
	(
			(unsigned char *) str_value,
			(size_t) bpstrlen(str_value),
			&ascii_config
	);

	// if there is no buff analysis, exit now
	if(!buff_analysis)
	{

		// force exit if analysis failed
		return ERR_FAILURE;

	}

	// check to ensure that there is at least one period
	if(!buff_analysis->ascii->char_counts['.'])
	{
		BP_DestroyBufferAnalysis(buff_analysis);
		return ERR_FAILURE;
	}

	// Ensure period count is reasonable.  IP's can have 3, subdomains can add more, top domain can add
	// another few.  10 is reasonable here.
	if(buff_analysis->ascii->char_counts['.'] >= 10)
	{
		BP_DestroyBufferAnalysis(buff_analysis);
		return ERR_FAILURE;
	}

	// Ensure that buffer only contains (and always contains) one @ sign.
	if(buff_analysis->ascii->char_counts['@'] != 1)
	{
		BP_DestroyBufferAnalysis(buff_analysis);
		return ERR_FAILURE;
	}

	// destroy the buffer analysis
	BP_DestroyBufferAnalysis(buff_analysis);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Local Part/Domain Part Checks %%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ex: local-part@domain-part
	// Per RFC 5322/RFC 5321 local part can be 64 bytes long, and domain part can be 255.
	// That said, it also says that an email can never be longer than 254 bytes long.

	// explode the buffers
	char ** front_and_domain_parts = BP_ExplodeBufferBin(str_value, len, "@", 1, 2);
	if(!front_and_domain_parts)
		return ERR_FAILURE;

	// ensure we have 2 entries (should always be the case)
	if(BP_CountNullTerminatedPtrArray(front_and_domain_parts) != 2)
	{
		BP_DestroyPointerArrayNullTerminated((void **) front_and_domain_parts);
		return ERR_FAILURE;
	}

	// run RFC length checks
	if(bpstrlen(front_and_domain_parts[0]) > 64)
		return ERR_FAILURE;
	if(bpstrlen(front_and_domain_parts[1]) > 254)
		return ERR_FAILURE;

	// destroy front and domain parts
	BP_DestroyPointerArrayNullTerminated((void **) front_and_domain_parts);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Regexp Check %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// loose url grabbing regexp (can result in false positives, but not very often, treat
	// results from this regexp with regard to that taint effect)
 	char * regexp = "[\\w-]+(?:\\.[\\w-]+)*@(?:[\\w-]+\\.)+[a-zA-Z]{2,7}";

	// create zeroed out inout structure
	BP_REGEX_MATCH_INFO match_info = {0};

	// get matches
	size_t matched = BP_MatchRegularExpressionPCRE
	(
		str_value,
		len,
		regexp,
		0,
		1024,
		&match_info,
		true
	);

	// if we have a match we can attempt to insert the email
	if(matched == ERR_SUCCESS)
	{

		// clear out match info
		BP_DestroyRegexpMatchInfo(&match_info);

		// return indicating success
		return ERR_SUCCESS;

	}

	// clear out match info
	BP_DestroyRegexpMatchInfo(&match_info);

	// return indicating failure
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Specific Parsing %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



// Attempts to parse out a csv file from a buffer.  This is primarily
// used with the sqlmap subsystem which generates a csv file directly.
char * BP_ParseBufferToUNIXFilesystemPathCSVPCRE(char *buffer, size_t buffer_size)
{

	// path matching regexp
	char * regexp = "(?:\\/\.?[\\w-]{4,})+(?:[\\/~\\w\\-0-9]+)\\.csv";

	// create zeroed out inout structure
	BP_REGEX_MATCH_INFO match_info = {0};

	// get matches
	size_t matched = BP_MatchRegularExpressionPCRE(buffer, buffer_size, regexp, 0, 1024, &match_info, true);

	// set the return string default to null
	char *ret_string = NULL;

	// set matched
	if(matched == ERR_SUCCESS)
	{

		// check for the matches
		size_t n = 0;
		for(; n < match_info.pcre_num_matched; n++)
		{

			// display matched string
			ret_string = bpstrdup(match_info.preg_matches->matched_string);
			break;

		}

	}

	// destroy the match info
	BP_DestroyRegexpMatchInfo(&match_info);

	// return null for failure
	return ret_string;

}


// checks a stack of strings to see if the string is already
// in the stack.
BP_ERROR_T BP_StringExistsInPointerArray(char ** strings_to_search, size_t stack_size, char *string)
{

	if(!strings_to_search)
		return ERR_FAILURE;
	if(!stack_size)
		return ERR_FAILURE;
	if(!string)
		return ERR_FAILURE;

	size_t n = 0;
	for(; n < stack_size; n++)
	{

		// ensure we have strings to search
		if(!strings_to_search[n])
			return ERR_FAILURE;

		// check string length
		size_t search_len = strnlen(strings_to_search[n], 4096);
		size_t string_len = strnlen(string, 4096);

		// check string length
		if(search_len != string_len)
			return ERR_FAILURE;

		// if the strings match, return success
		if(memcmp(string, strings_to_search, search_len) == 0)
			return ERR_SUCCESS;

	}

	// return indicating success
	return ERR_SUCCESS;
}

char * BP_SingleQuoteString(char *string)
{
	if(!string)
		return NULL;

	// calculate the length
	size_t len = bpstrlen(string);
	if(!len)
	{
		return bpstrdup("''");
	}

	// add some extra buffer space
	len+=16;

	// allocate space for out string
	char * out_str = (char *) bpcalloc(len, 1);

	// quot the string
	snprintf(out_str, len-1, "'%s'", string);

	// return failure
	return out_str;
}

char * BP_DoubleQuoteString(char *string)
{

	if(!string)
		return NULL;

	// calculate the length
	size_t len = bpstrlen(string);
	if(!len)
	{
		return bpstrdup("\"\"");
	}

	// add some extra buffer space
	len+=16;

	// allocate space for out string
	char * out_str = (char *) bpcalloc(len, 1);

	// quot the string
	snprintf(out_str, len-1, "\"%s\"", string);

	// return failure
	return out_str;

}

// simple routine to duplicate memory
void *memdup(const void *src, size_t n) {

	if (!src)
		return NULL;
	if (n <= 0)
		return NULL;

	void *dst = NULL;
	dst = bpcalloc(n, 1);
	if(dst)
	{
		memcpy(dst, src, n);
	}
	else
	{
		printf("\n");
		printf("\n [!!] Error: Allocation failed during memory duplication.  Exiting with trap.");
		printf("\n");
		__asm("int3");
	}
	return dst;
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% XOR "Encryption" %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// XOR "Encrypts" a provided value.  Value can contain binary data.  Since
// xored values do not change length during "encrypting" data.
unsigned char * BP_XOREncrypt(unsigned char *val, size_t val_length, unsigned char * key, size_t key_length)
{

	// ensure we have non-zero pointers and size references
	if(!val)
		return NULL;
	if(!val_length)
		return NULL;
	if(!key)
		return NULL;
	if(!key_length)
		return NULL;

	// attempt to allocate space for buffer
	unsigned char *out_val = (unsigned char *) bpcalloc(val_length, 1);
	if(!out_val)
		return NULL;

	// copy in data
	memcpy(out_val, val, val_length);

	// index counter for key
	size_t val_count = 0;

	//Loop through each byte of file until EOF
	size_t outer_n = 0;
	for(; outer_n < val_length; outer_n++)
	{

		//XOR the data and write it to a file
		out_val[outer_n] = out_val[outer_n] ^ key[val_count];

		// keep track of value count
		val_count++;
		if(val_count == val_length)
			val_count = 0;

	}

	// return bin value on success
	return out_val;

}

// XOR Decryption routine (it's just a logical placeholder for
// XOREncrypt)
unsigned char * BP_XORDecrypt(unsigned char *val, size_t val_length, unsigned char * key, size_t key_length)
{

	// return null on failure
	return BP_XOREncrypt(val, val_length, key, key_length);

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Base 64 Encoding / Decoding %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// encodes a buffer with base64 (must be freed)
char * BP_Base64Encode(unsigned char *src, size_t len)
{

	// set the standard character lookup table
	unsigned char base64_table[64];
	memcpy(&base64_table, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/", bpstrlen("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"));

	// check for valid data
	if(!src)
		return NULL;
	if(!len)
		return NULL;

	// -- begin

	unsigned char *out = NULL;
	unsigned char *pos = NULL;
	unsigned char *end = NULL;
	unsigned char *in  = NULL;

	size_t olen = 0;
	int line_len = 0;

	olen = len * 4 / 3 + 4; /* 3-byte blocks to 4-byte */
	olen += olen / 72; /* line feeds */
	olen++; /* nul termination */

	// create the return / output buffer
	out = (unsigned char *) bpcalloc(olen, 1);
	if (out == NULL)
		return NULL;

	// set a pointer to the end of the data
	end = src + len;

	// set pointer to the beginning of the data
	in = src;

	// set position as the beginning of the string
	pos = out;

	// set the current line length to zero
	line_len = 0;

	// loop from the beginning to the end of the string
	while ( (end - in) >= 3)
	{

		// this is the meat of the base64 operations
		*pos++ = base64_table[in[0] >> 2];
		*pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
		*pos++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
		*pos++ = base64_table[in[2]   & 0x3f];

		in       += 3;
		line_len += 4;
		/*
		if (line_len >= 72) {
			*pos++ = '\n';
			line_len = 0;
		}
		*/

	}

	if (end - in)
	{
		*pos++ = base64_table[in[0] >> 2];
		if (end - in == 1)
		{
			*pos++ = base64_table[(in[0] & 0x03) << 4];
			*pos++ = '=';
		}
		else
		{
			*pos++ = base64_table[(  (in[0] & 0x03) << 4) |  (in[1] >> 4 )  ];
			*pos++ = base64_table[  (in[1] & 0x0f) <<   2  ];
		}

		*pos++ = '=';

	}

	// null truncate the output string (since pos is a pointer to outs end)
	*pos = '\0';

	// return pointer to the data
	return (char *) out;

}

// decodes a buffer which has been encoded with base64
P_BP_BASE64_DECODED_DATA BP_Base64Decode(char *src, size_t len)
{


	if(!src)
		return NULL;
	if(!len)
		return NULL;

	// set the standard character lookup table
	unsigned char base64_table[64];
	memcpy(&base64_table, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/", bpstrlen("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"));

	unsigned char dtable[256] = {0};
	unsigned char *out = NULL;
	unsigned char *pos = NULL;
	unsigned char in[4] = {0};
	unsigned char block[4] = {0};
	unsigned char tmp = 0x00;

	// set counters
	size_t i     = 0;
	size_t count = 0;
	size_t olen  = 0;

	// set decode table to 0x80 in all places
	memset(dtable, 0x80, 256);

	// set decode table
	for (i = 0; i < sizeof(base64_table); i++)
	{
		dtable[base64_table[i]] = i;
	}

	dtable['='] = 0;

	count = 0;
	for (i = 0; i < len; i++)
	{
		if (dtable[src[i]] != 0x80)
			count++;
	}

	if (count % 4)
		return NULL;

	olen = count / 4 * 3;
	pos = out = (unsigned char *) bpcalloc(count, 1);
	if (out == NULL)
		return NULL;

	count = 0;
	for (i = 0; i < len; i++)
	{
		tmp = dtable[src[i]];
		if (tmp == 0x80)
				continue;

		in[count] = src[i];
		block[count] = tmp;
		count++;
		if (count == 4)
		{
				*pos++ = (block[0] << 2) | (block[1] >> 4);
				*pos++ = (block[1] << 4) | (block[2] >> 2);
				*pos++ = (block[2] << 6) | block[3];
				count = 0;
		}
	}

	if (pos > out)
	{
		if (in[2] == '=')
			pos -= 2;
		else if (in[3] == '=')
			pos--;
	}

	// create decoded data structure
	P_BP_BASE64_DECODED_DATA decoded = (P_BP_BASE64_DECODED_DATA) bpcalloc(sizeof(BP_BASE64_DECODED_DATA), 1);

	// set buffer and position in the new structure
	decoded->buff        = out;
	decoded->buff_length = pos - out;

	// return the usable structure
	return decoded;

}

// destroys the data including structure (call this every time you need
// to destroy the result from the above decode function)
BP_ERROR_T BP_Base64DestroyDecodedData(P_BP_BASE64_DECODED_DATA data)
{
	if(data)
	{

		// destroy buffer if necessary
		if(data->buff)
			bpfree(data->buff);

		// now free structure itself
		bpfree(data);

		// return indicating success
		return ERR_SUCCESS;

	}

	// return indicating failure in the case we have no data
	return ERR_FAILURE;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% String Array Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Pushes an array onto a heap allocated array of char*s.
BP_ERROR_T BP_ArrayPushDupString(char * value, char *** stack, size_t *stack_n)
{

	// ensure we have a pointer to the stack
	if(!stack_n)
		return ERR_FAILURE;

	// calculate the new stack size based on provided inputs
	size_t new_stack_size = (*stack_n+1) * sizeof(char *);

	// create new stack
	stack[0] = (char **) bprealloc(*stack, new_stack_size);

	// set the value in the gadget stack
	stack[0][*stack_n] = bpstrdup(value);

	// increment the stack counter
	*stack_n = *stack_n + 1;

	// return indicating success
	return ERR_SUCCESS;

}

// TAIL QUEUE VERSION
// Pushes an array onto a heap allocated array of char*s.
// TAIL QUEUE VERSION
BP_ERROR_T BP_ArrayPushDupStringTQ(P_BP_LINKL tq, char * value, char *** stack, size_t *stack_n)
{

	// ensure we have a pointer to the stack
	if(!stack_n)
		return ERR_FAILURE;

	// calculate the new stack size based on provided inputs
	size_t new_stack_size = (*stack_n+2) * sizeof(char *);

	// create new stack
	stack[0] = (char **) tq_realloc(tq, *stack, new_stack_size, BPLN);

	// set the value in the gadget stack
	stack[0][*stack_n] = tq_strdup(tq, value, BPLN);

	// null terminate the array
	stack[0][*stack_n+1] = NULL;

	// increment the stack counter
	*stack_n = *stack_n + 1;

	// return indicating success
	return ERR_SUCCESS;

}


// HASH REGISTRY VERSION
// Pushes an array onto a heap allocated array of char*s.
// HASH REGISTRY VERSION
BP_ERROR_T BP_ArrayPushDupStringHREG(P_BP_HASH_TABLE_REGISTRY hreg, char * value, char *** stack, size_t *stack_n)
{

	// ensure we have a registry
	if(!hreg)
		return ERR_FAILURE;

	// ensure we have a pointer to the stack
	if(!stack_n)
		return ERR_FAILURE;

	// we need a pointer to a char **.
	if(!stack)
		return ERR_FAILURE;

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(hreg);

	// calculate the new stack size based on provided inputs
	size_t new_stack_size = (*stack_n+2) * sizeof(char *);

	// create new stack
	stack[0] = (char **) ht_realloc(hreg, *stack, new_stack_size);

	// set the value in the stack
	stack[0][*stack_n] = ht_strdup(hreg, value);

	// null terminate the array
	stack[0][*stack_n+1] = NULL;

	// increment the stack counter
	*stack_n = *stack_n + 1;

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(hreg);

	// return indicating success
	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% String/Mem Joining Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// NOTE: These routines are very simple and just use a tail queue allocator
//       to join up strings/memory by allocating them in the tq, and then flattening
//       the tq.  The final string must be bpfree()d or tq_free'd if you
//       link the returned result into your own tailqueue.

// Simply joins two strings and returns a heap buffer for you to use.  This routine
// is not binary safe, and should only be used on strings which you'd feel safe supplying
// to strdup.
char * BP_JoinStrings(char *string_front, char *string_tail, P_BP_LINKL tq_in)
{

	// create string front or tail if they don't already exist
	if(!string_front)
	{
		string_front = "";
	}

	// create tail
	if(!string_tail)
	{
		string_tail = "";
	}


	// ensure we have parameters
	if(!string_front || !string_tail)
		return NULL;

	// create linkl or exit if we cant
	P_BP_LINKL tq = BP_CreateLinkL(".", BP_FALSE);
	if(!tq)
		return NULL;

	// run length calculations first
	size_t front_len = bpstrlen(string_front);
	size_t tail_len   = bpstrlen(string_tail);


	// Just return a a tail if there's no front part
	if(!front_len && tail_len)
	{

		BP_DestroyLinkL(tq, BP_TRUE);
		if(tq_in)
			return tq_strdup(tq_in, string_tail, BPLN);
		else
			return bpstrdup(string_tail);
	}

	if(front_len && !tail_len)
	{

		BP_DestroyLinkL(tq, BP_TRUE);
		if(tq_in)
			return tq_strdup(tq_in, string_front, BPLN);
		else
			return bpstrdup(string_tail);

	}

	// dupe the front
	if(!tq_strdup(tq, string_front, BPLN))
	{
		BP_DestroyLinkL(tq, BP_TRUE);
		return NULL;
	}

	// dupe the tail
	if(!tq_strdup(tq, string_tail, BPLN))
	{
		BP_DestroyLinkL(tq, BP_TRUE);
		return NULL;
	}

	// flatten the tq
	if(BP_LinkLFlattenJoinStringElements(tq) != ERR_SUCCESS)
	{
		BP_DestroyLinkL(tq, BP_TRUE);
		return NULL;
	}

	// this is the buffer which gets returned
	char *final_buffer = NULL;

	// copy the final buffer
	if(tq->flat_buff)
	{
		final_buffer = bpstrdup((char *) tq->flat_buff);
	}

	// destroy the final tailqueue
	BP_DestroyLinkL(tq, BP_TRUE);

	// check if we have an output tailueue
	if(tq_in)
	{
		// link the final buffer if we have a tailqueue to link it in
		tq_link(tq_in, final_buffer, front_len + tail_len + 1, BPLN);
	}

	// return the final buffer
	return final_buffer;

}


// joins two pieces of memory in a newly allocated buffer.
void * BP_JoinMem(void *mem_front, size_t front_len, void *mem_tail, size_t tail_len, P_BP_LINKL tq_in)
{

	// ensure all join parts are available
	if(!mem_front || !mem_tail || !front_len || !tail_len)
		return NULL;

	// create linkl or exit if we cant
	P_BP_LINKL tq = BP_CreateLinkL(".", BP_FALSE);
	if(!tq)
		return NULL;

	// attempt to allocate pointers
	void *front_part = tq_calloc(tq, front_len, 1, BPLN);
	void *tail_part  = tq_calloc(tq, tail_len, 1, BPLN);

	// ensure both were created ok
	if(!front_part || !tail_part)
	{
		BP_DestroyLinkL(tq, BP_TRUE);
		return NULL;
	}

	// now copy in data
	memcpy(front_part, mem_front, front_len);
	memcpy(tail_part,  mem_tail,  tail_len);

	// flatten the tq
	if(BP_LinkLFlattenRaw(tq) != ERR_SUCCESS)
	{
		BP_DestroyLinkL(tq, BP_TRUE);
		return NULL;
	}

	// this is the final buffer which gets returned
	void * final_buffer = NULL;

	// set final buffer if we've flattened things ok
	if(tq->flat_buff)
	{

		// allocate space for the final buffer
		final_buffer = bpcalloc(front_len+tail_len, 1);

		// if it couldn't be created, exit here
		if(!final_buffer)
		{
			BP_DestroyLinkL(tq, BP_TRUE);
			return NULL;
		}

		// finally copy in the joined data
		memcpy(final_buffer, tq->flat_buff, front_len+tail_len);

	}

	// destroy the linkl and return with the final generated buffer
	BP_DestroyLinkL(tq, BP_TRUE);

	// check if we have an output tailueue
	if(tq_in)
	{

		// link the final buffer if we have a tailqueue to link it in
		tq_link(tq_in, final_buffer, front_len + tail_len, BPLN);

	}

	// return the pointer
	return final_buffer;

}

// joins all elements in string_array by calling BP_JoinStrings on each element consecutively.
char * BP_JoinStringArray(char ** string_array, size_t entry_count, P_BP_LINKL tq_in)
{

	// ensure we have a string array and an entry count
	if(!string_array || !entry_count)
		return NULL;

	// minimum elements is 2 (front and tail)
	if(entry_count < 2)
		return NULL;

	// return joined string
	if(entry_count == 2)
		return BP_JoinStrings(string_array[0], string_array[1], tq_in);

	// final buffer for returning to the caller
	char * final_buffer = NULL;

	// set initial buffer
	char * last_buffer = NULL;

	// notes if we're in the first buffer
	BP_BOOL is_first_iter = BP_TRUE;

	// walk each entry and join things up
	size_t n = 0;
	for(; n < entry_count; n++)
	{

		// the first iteration requires a few different steps
		if(is_first_iter == BP_TRUE)
		{

			// attempt to join the buffer
			final_buffer = BP_JoinStrings(string_array[0], string_array[1], tq_in);

			// ensure we could create the buffer
			if(!final_buffer)
				return NULL;

			// store the last buffer as the final buffer, so it can be free'd on loop
			last_buffer = final_buffer;

			// set flag indicating we're no longer on the first iteration, and continue
			// the loop.
			is_first_iter = BP_FALSE;
			n++;
			continue;

		}


		// try to create the final buffer
		final_buffer = BP_JoinStrings(last_buffer, string_array[n], tq_in);

		// free last_buffer in tailqueue if set, if not, normal free
		if(tq_in)
		{
			tq_free(tq_in, last_buffer);
			last_buffer = final_buffer;
		}
		else
		{
			bpfree(last_buffer);
			last_buffer = final_buffer;
		}


	}

	// return the final buffer
	return final_buffer;

}


// This routine joins an array of buffers (simple structure containing a pointer and a size), and stores the resulting
// joined buffer in the out_buff parameter.  It's done this way so we don't have to allocate the out_buff
// for the user, and they can just supply a stack pointer etc.  If this routine returns ERR_SUCCESS, it means
// that the data and len members of out_buff have been set to the joined data, and can then be used.  The
// data member is allocated on the heap directly if no tq_in is provided.  If one is provided, it is allocated
// via that tailqueue.
BP_ERROR_T BP_JoinMemArray(P_BP_BUFFER_T mem_array, size_t entry_count, P_BP_BUFFER_T out_buff, P_BP_LINKL tq_in)
{


	// ensure we have a string array and an entry count
	if(!mem_array || !entry_count || !out_buff)
		return ERR_FAILURE;

	// minimum elements is 2 (front and tail)
	if(entry_count < 2)
		return ERR_FAILURE;

	// final buffer for returning to the caller
	BP_BUFFER_T final_buffer;
	BP_ZERO_STACK(final_buffer);

	// set initial buffer
	BP_BUFFER_T last_buffer;
	BP_ZERO_STACK(last_buffer);

	// notes if we're in the first buffer
	BP_BOOL is_first_iter = BP_TRUE;

	// walk each entry and join things up
	size_t n = 0;
	for(; n < entry_count; n++)
	{

		// ensure we have data to store
		if(!mem_array[n].data || !mem_array[n].len)
			continue;

		// the first iteration requires a few different steps
		if(is_first_iter == BP_TRUE)
		{


			// attempt to join memory
			final_buffer.data = BP_JoinMem
			(
				mem_array[0].data, mem_array[0].len,
				mem_array[1].data, mem_array[1].len,
				tq_in
			);

			// set length
			final_buffer.len = mem_array[0].len + mem_array[1].len;

			// ensure we could create the buffer
			if(!final_buffer.data || !final_buffer.len)
				return ERR_FAILURE;

			// store the last buffer as the final buffer, so it can be free'd on loop
			last_buffer.data = final_buffer.data;
			last_buffer.len  = final_buffer.len;

			// set flag indicating we're no longer on the first iteration, and continue
			// the loop.
			is_first_iter = BP_FALSE;
			n++;
			continue;

		}


		// join the element into our buffer
		final_buffer.data = BP_JoinMem
		(
			last_buffer.data,  last_buffer.len,
			mem_array[n].data, mem_array[n].len,
			tq_in
		);

		// increase the join length by the element length
		final_buffer.len += mem_array[n].len;


		// free last_buffer in tailqueue if set, if not, normal free
		if(tq_in)
		{
			// free and reset length (tailqueue)
			tq_free(tq_in, last_buffer.data);
			last_buffer.len = 0;
		}
		else
		{
			// free and reset length (natural free)
			bpfree(last_buffer.data);
			last_buffer.len = 0;
		}

		// set data and length
		last_buffer.data = final_buffer.data;
		last_buffer.len  = final_buffer.len;

	}

	// set output members
	out_buff->data = final_buffer.data;
	out_buff->len  = final_buffer.len;

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% String/Mem Combining Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to combine a string array (result is bp_calloced and must be destroyed)
char ** BP_CombineStringArray(char ** array_head, char **array_tail)
{

	// this array is our out
	char **out_array = NULL;


	// --- add head ----

	// create temporary linkl
	P_BP_LINKL tmp_linkl = BP_CreateLinkL(".", BP_FALSE);
	if(!tmp_linkl)
		return NULL;

	if(array_head)
	{

		// array count
		size_t array_count = BP_CountNullTerminatedPtrArray(array_head);
		if(array_count)
		{

			// iterate through array
			size_t iter = 0;
			for(; iter < array_count; iter++)
			{

				// duplicate string
				tq_strdup(tmp_linkl, array_head[iter], BPLN);

			}

		}

	}

	// --- add tail ----

	if(array_tail)
	{

		// array count
		size_t array_count = BP_CountNullTerminatedPtrArray(array_tail);
		if(array_count)
		{

			// iterate through array
			size_t iter = 0;
			for(; iter < array_count; iter++)
			{

				// duplicate string
				tq_strdup(tmp_linkl, array_tail[iter], BPLN);

			}

		}
	}

	// create ptr array
	out_array = (char **) BP_LinkLToBPCallocPointerArray(tmp_linkl);

	// destroy the linkl
	BP_DestroyLinkL(tmp_linkl, BP_TRUE);

	// return the output array
	return out_array;

}



// Generates a hexidecimal string representation of a buffer provided.  Uses
// the supplied tail queue allocator if one is passed in.  If the passed tail
// queue allocator is null, this function uses standard allocation routines to generate
// the buffer, which implies they must be freed directly after use.
char * BP_GenHexStringFromBuffer(unsigned char * buff, size_t size, P_BP_LINKL tq)
{

	// check to ensure we have a buffer and a buffer size
	if(!buff || !size)
		return NULL;

	// since we're expanding the buffer from one byte per char to
	// two bytes per hex char, we can just multiply size to get
	// the correct buffer size.
	size_t buff_size = (size * 2) + 1;

	// now allocate the out buffer
	char *out_buff = (char *) NULL;
	if(tq)
		out_buff = (char *) tq_calloc(tq, buff_size, 1, BPLN);
	else
		out_buff = (char *) bpcalloc(buff_size, 1);

	// now fill the buffer with hex data
	size_t n = 0;
	for(; n < size; n++)
	{
		sprintf(&out_buff[n*2], "%.02x", buff[n]);
	}

	if(out_buff && tq)
	{

		// set data type via manual means since we're creating a string
		P_BP_LINKL_DATA linkl_data = BP_BackwardLookupDataItemFromPointer(tq, out_buff);

		// set the data type here
		linkl_data->data_type = BP_MEMPROF_ALLOC_TYPE_USER_DEFINED_ASCII;

	}

	// return the allocated buffer
	return out_buff;

}

// same as the non-escaped version, except this version escapes the string as if it were
// \xNN encoded (aka \x00\x01\x02)
char * BP_GenEscapedHexStringFromBuffer(unsigned char * buff, size_t size, P_BP_LINKL tq)
{

	// check to ensure we have a buffer and a buffer size
	if(!buff || !size)
		return NULL;

	// since we're expanding the buffer from one byte per char to
	// four bytes per hex char, we can just multiply size to get
	// the correct buffer size.
	size_t buff_size = (size * 4) + 1;

	// now allocate the out buffer
	char *out_buff = (char *) NULL;
	if(tq)
		out_buff = (char *) tq_calloc(tq, buff_size, 1, BPLN);
	else
		out_buff = (char *) bpcalloc(buff_size, 1);

	// now fill the buffer with escaped hex data
	size_t n = 0;
	size_t j = 0;
	for(; n < size; n++)
	{
		sprintf(&out_buff[n*4], "\\x%.02x", buff[n]);

	}

	if(out_buff && tq)
	{

		// set data type via manual means since we're creating a string
		P_BP_LINKL_DATA linkl_data = BP_BackwardLookupDataItemFromPointer(tq, out_buff);

		// set the data type here
		linkl_data->data_type = BP_MEMPROF_ALLOC_TYPE_USER_DEFINED_ASCII;

	}

	// return the allocated buffer
	return out_buff;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% String Case Converstion %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// converts a string to lower case (returns bp allocated memory, must be bpfreed)
char * BP_StringToLowerCase(char * buff, size_t buff_len)
{

	// ensure it's a reasonable/printable string
	if(!BP_StringIsReasonablePrintableString(buff, buff_len, BP_TRUE, BP_TRUE))
		return NULL;

	// set string iterator
	size_t string_iter = 0;

	// allocate space for the returning string
	char * ret_string = (char *) bpcalloc(buff_len+1, 1);
	if(!ret_string)
		return NULL;


	// convert to lowercase
	for(; string_iter < buff_len; string_iter++)
	{

		// check the byte to see if it's alphanumeric before conversion
		if(isalpha(buff[string_iter]))
			ret_string[string_iter] = tolower(buff[string_iter]);
		else
			ret_string[string_iter] = buff[string_iter];

	}

	// return the new string
	return ret_string;

}

// converts a string to upper case (returns bp allocated memory, must be bpfreed)
char * BP_StringToUpperCase(char * buff, size_t buff_len)
{

	// ensure it's a reasonable/printable string
	if(!BP_StringIsReasonablePrintableString(buff, buff_len, BP_TRUE, BP_TRUE))
		return NULL;

	// set string iterator
	size_t string_iter = 0;

	// allocate space for the returning string
	char * ret_string = (char *) bpcalloc(buff_len+1, 1);
	if(!ret_string)
		return NULL;


	// convert to uppercase
	for(; string_iter < buff_len; string_iter++)
	{

		// check the byte to see if it's alphanumeric before conversion
		if(isalpha(buff[string_iter]))
			ret_string[string_iter] = toupper(buff[string_iter]);
		else
			ret_string[string_iter] = buff[string_iter];

	}

	// return the new string
	return ret_string;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Unique ID Generation %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Generates a random uuid.  This is purely random, not based on the GUID/UUID standard
// which relies on various pieces of system data.  The reason for this is that the UUID
// standard can leak information in UUIDs, so this aims to avoid that data leak.
char * BP_GenRandomUUID()
{

	// create buffer first for uuid
	char *random_uuid = (char *) bpcalloc(37, 1);

	// generate 32 bytes of data
	unsigned char *random_data = BP_GenRandomUnsignedCharString(16);

	sprintf
	(
			random_uuid, "%.02x%.02x%.02x%.02x-%.02x%.02x-%.02x%.02x-%.02x%.02x-%.02x%.02x%.02x%.02x%.02x%.02x",
			random_data[0],
			random_data[1],
			random_data[2],
			random_data[3],
			random_data[4],
			random_data[5],
			random_data[6],
			random_data[7],
			random_data[8],
			random_data[9],
			random_data[10],
			random_data[11],
			random_data[12],
			random_data[13],
			random_data[14],
			random_data[15]
	);

	// destroy the random data
	bpfree(random_data);

	// return the random uuid value
	return random_uuid;

}

// Buffer must be at least 40 bytes long
BP_ERROR_T BP_GenRandomUUID_StoreInBuff(char *buff)
{

	if(!buff)
		return ERR_FAILURE;

	// attempt to create a new uuid
	char * new_uuid = BP_GenRandomUUID();
	if(!new_uuid)
		return ERR_FAILURE;

	// copy in data
	memcpy(buff, new_uuid, bpstrlen(new_uuid));

	// destroy the uuid
	bpfree(new_uuid);

	// return indicating success
	return ERR_SUCCESS;

}

// Simple check to ensure a string matches a UUID pattern.
BP_ERROR_T BP_StringIsUUID(char * str)
{

	// ensure we have a string
	if(!str)
		return ERR_FAILURE;

	// ensure the string is 36 bytes long
	if(strnlen(str, 40) != 36)
		return ERR_FAILURE;

	// ensure string is reasonable/printable
	if(!BP_StringIsReasonablePrintableString(str, 36, BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// set parts from indexable positions
	char *first_part  = &str[0];
	char *second_part = &str[9];
	char *third_part  = &str[14];
	char *fourth_part = &str[19];
	char *fifth_part  = &str[24];

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Checks on First Part %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// check first part first (fast branch math via macros)
	if
	(
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(first_part[0]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(first_part[1]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(first_part[2]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(first_part[3]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(first_part[4]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(first_part[5]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(first_part[6]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(first_part[7])
	)
	{
		return ERR_FAILURE;
	}

	// check first part joiner
	if(first_part[8] != '-')
		return ERR_FAILURE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Checks on Second Part %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// check second part first (fast branch math via macros)
	if
	(
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(second_part[0]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(second_part[1]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(second_part[2]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(second_part[3])
	)
	{
		return ERR_FAILURE;
	}

	// check second part joiner
	if(second_part[4] != '-')
		return ERR_FAILURE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Checks on Third Part %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// check third part first (fast branch math via macros)
	if
	(
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(third_part[0]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(third_part[1]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(third_part[2]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(third_part[3])
	)
	{
		return ERR_FAILURE;
	}

	// check third part joiner
	if(third_part[4] != '-')
		return ERR_FAILURE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Checks on Fourth Part %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// check third part fourth (fast branch math via macros)
	if
	(
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(fourth_part[0]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(fourth_part[1]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(fourth_part[2]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(fourth_part[3])
	)
	{
		return ERR_FAILURE;
	}

	// check fourth part joiner
	if(fourth_part[4] != '-')
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Checks on Fifth Part %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// check third part fifth (fast branch math via macros)
	if
	(
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(fifth_part[0]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(fifth_part[1]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(fifth_part[2]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(fifth_part[3]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(fifth_part[4]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(fifth_part[5]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(fifth_part[6]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(fifth_part[7]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(fifth_part[8]) ||
		!BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(fifth_part[9])
	)
	{
		return ERR_FAILURE;
	}

	// return indicating we have a uuid match
	return ERR_SUCCESS;

}


// Generates a random SHA1() hash in ascii (40bytes random as ascii)
char * BP_GenRandomSHA1()
{

	// create buffer first for uuid
	char *random_sha1 = (char *) bpcalloc(41, 1);

	// generate 40 bytes of random data
	unsigned char *random_data = BP_GenRandomUnsignedCharString(20);

	sprintf
	(
			random_sha1, "%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x",
			random_data[0],
			random_data[1],
			random_data[2],
			random_data[3],
			random_data[4],
			random_data[5],
			random_data[6],
			random_data[7],
			random_data[8],
			random_data[9],
			random_data[10],
			random_data[11],
			random_data[12],
			random_data[13],
			random_data[14],
			random_data[15],
			random_data[16],
			random_data[17],
			random_data[18],
			random_data[19]
	);

	// destroy the random data
	bpfree(random_data);

	// return the random uuid value
	return random_sha1;

}


// Generates a random SHA1() hash in ascii (40bytes random as ascii)
char * BP_GenRandomMD5()
{

	// create buffer first for uuid
	char *random_md5 = (char *) bpcalloc(33, 1);

	// generate 40 bytes of random data
	unsigned char *random_data = BP_GenRandomUnsignedCharString(16);

	sprintf
	(
			random_md5, "%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x",
			random_data[0],
			random_data[1],
			random_data[2],
			random_data[3],
			random_data[4],
			random_data[5],
			random_data[6],
			random_data[7],
			random_data[8],
			random_data[9],
			random_data[10],
			random_data[11],
			random_data[12],
			random_data[13],
			random_data[14],
			random_data[15]
	);

	// destroy the random data
	bpfree(random_data);

	// return the random uuid value
	return random_md5;

}




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% PHP Gadget String Encoders %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// PHP string encoders encode binary buffers as strings of php code.  The reason for this
// is obfuscating or otherwise modifying a string of bytes to meet some encoder criteria.
//

// Encodes a single character to a char string.  Non printables are hex encoded as escaped hex.
char * BP_GenUCharAsPHPEscapedASCII(unsigned char val, P_BP_LINKL tq)
{

	// DONT CHECK VALUES, AS EVERY BYTE FROM 0x00 to 0xff ARE VALID

	// define output buffer
	char *out_buff = NULL;

	// check if the value is printable
	if(isprint(val))
	{

		// allocate using tailqueue if we have one, if not use normal calloc
		if(tq)
			out_buff = (char *) tq_calloc(tq, 5, 1, BPLN);
		else
			out_buff = (char *) bpcalloc(5, 1);

		// create the buffer
		sprintf(out_buff,"'%c'", (char) val);

	}
	else
	{

		// allocate using tailqueue if we have one, if not use normal calloc
		if(tq)
			out_buff = (char *) tq_calloc(tq, 7, 1, BPLN);
		else
			out_buff = (char *) bpcalloc(7, 1);


		// create the buffer
		sprintf(out_buff, "\"\\x%.02x\"", (unsigned char) val);

	}

	// If the out buffer was generated ok, and the tail queue is set,
	// change the type of the assigned chunk to user defined ascii.  This
	// allows the List flattener to work on this chunk (if not it'd be considered
	// a binary calloc chunk)
	if(out_buff && tq)
	{

		// set data type via manual means since we're creating a string
		P_BP_LINKL_DATA linkl_data = BP_BackwardLookupDataItemFromPointer(tq, out_buff);

		// set the data type here
		linkl_data->data_type = BP_MEMPROF_ALLOC_TYPE_USER_DEFINED_ASCII;

	}

	// return the filled out buffer
	return out_buff;

}

// Encodes a single character as ord('n')
char * BP_GenUCharAsPHPOrd(unsigned char val, P_BP_LINKL tq)
{

	// DONT CHECK VALUES, AS EVERY BYTE FROM 0x00 to 0xff ARE VALID

	// define output buffer
	char *out_buff = NULL;

	// check if the value is printable
	if(isprint(val))
	{

		// allocate using tailqueue if we have one, if not use normal calloc
		if(tq)
			out_buff = (char *) tq_calloc(tq, 11, 1, BPLN);
		else
			out_buff = (char *) bpcalloc(11, 1);

		// create the buffer
		sprintf(out_buff,"ord('%c')", (char) val);

	}
	else
	{

		// allocate using tailqueue if we have one, if not use normal calloc
		if(tq)
			out_buff = (char *) tq_calloc(tq, 13, 1, BPLN);
		else
			out_buff = (char *) bpcalloc(13, 1);


		// create the buffer
		sprintf(out_buff, "ord(\"\\x%.02x\")", (unsigned char) val);

	}

	// If the out buffer was generated ok, and the tail queue is set,
	// change the type of the assigned chunk to user defined ascii.  This
	// allows the List flattener to work on this chunk (if not it'd be considered
	// a binary calloc chunk)
	if(out_buff && tq)
	{

		// set data type via manual means since we're creating a string
		P_BP_LINKL_DATA linkl_data = BP_BackwardLookupDataItemFromPointer(tq, out_buff);

		// set the data type here
		linkl_data->data_type = BP_MEMPROF_ALLOC_TYPE_USER_DEFINED_ASCII;

	}

	// return the filled out buffer
	return out_buff;

}

// Encodes a single character as chr(dec)
char * BP_GenUCharAsPHPChr(unsigned char val, P_BP_LINKL tq)
{
	// DONT CHECK VALUES, AS EVERY BYTE FROM 0x00 to 0xff ARE VALID

	// define output buffer
	char *out_buff = NULL;

	// allocate using tailqueue if we have one, if not use normal calloc
	if(tq)
		out_buff = (char *) tq_calloc(tq, 16, 1, BPLN);
	else
		out_buff = (char *) bpcalloc(16, 1);


	// create the buffer
	sprintf(out_buff, "chr(%u)", (unsigned char) val);


	// If the out buffer was generated ok, and the tail queue is set,
	// change the type of the assigned chunk to user defined ascii.  This
	// allows the List flattener to work on this chunk (if not it'd be considered
	// a binary calloc chunk)
	if(out_buff && tq)
	{

		// set data type via manual means since we're creating a string
		P_BP_LINKL_DATA linkl_data = BP_BackwardLookupDataItemFromPointer(tq, out_buff);

		// set the data type here
		linkl_data->data_type = BP_MEMPROF_ALLOC_TYPE_USER_DEFINED_ASCII;

	}

	// return the filled out buffer
	return out_buff;

}


// Encodes a single character as php hex
char * BP_GenUCharAsPHPHex(unsigned char val, P_BP_LINKL tq)
{

	// DONT CHECK VALUES, AS EVERY BYTE FROM 0x00 to 0xff ARE VALID

	// define output buffer
	char *out_buff = NULL;

	// allocate using tailqueue if we have one, if not use normal calloc
	if(tq)
		out_buff = (char *) tq_calloc(tq, 5, 1, BPLN);
	else
		out_buff = (char *) bpcalloc(5, 1);


	// create the buffer
	sprintf(out_buff, "0x%.02x", (unsigned char) val);


	// If the out buffer was generated ok, and the tail queue is set,
	// change the type of the assigned chunk to user defined ascii.  This
	// allows the List flattener to work on this chunk (if not it'd be considered
	// a binary calloc chunk)
	if(out_buff && tq)
	{

		// set data type via manual means since we're creating a string
		P_BP_LINKL_DATA linkl_data = BP_BackwardLookupDataItemFromPointer(tq, out_buff);

		// set the data type here
		linkl_data->data_type = BP_MEMPROF_ALLOC_TYPE_USER_DEFINED_ASCII;

	}

	// return the filled out buffer
	return out_buff;

}

// Encodes a single character as a decimal number
char * BP_GenUCharAsPHPDecimal(unsigned char val, P_BP_LINKL tq)
{
	// DONT CHECK VALUES, AS EVERY BYTE FROM 0x00 to 0xff ARE VALID

		// define output buffer
		char *out_buff = NULL;

		// allocate using tailqueue if we have one, if not use normal calloc
		if(tq)
			out_buff = (char *) tq_calloc(tq, 5, 1, BPLN);
		else
			out_buff = (char *) bpcalloc(5, 1);


		// create the buffer
		sprintf(out_buff, "%u", (unsigned char) val);


		// If the out buffer was generated ok, and the tail queue is set,
		// change the type of the assigned chunk to user defined ascii.  This
		// allows the List flattener to work on this chunk (if not it'd be considered
		// a binary calloc chunk)
		if(out_buff && tq)
		{

			// set data type via manual means since we're creating a string
			P_BP_LINKL_DATA linkl_data = BP_BackwardLookupDataItemFromPointer(tq, out_buff);

			// set the data type here
			linkl_data->data_type = BP_MEMPROF_ALLOC_TYPE_USER_DEFINED_ASCII;

		}

		// return the filled out buffer
		return out_buff;

}

// Encodes a single character to specified selection
char * BP_GenPHPGadgetEncodeUChar(unsigned char val, BP_PHP_GADGET_BUFFER_ENCODE_SELECTOR selector, P_BP_LINKL tq)
{

	// ensure we have a valid value or selector
	if(!selector)
		return NULL;

	// set the generated buffer to NULL
	char * gen_buff = NULL;
	char * fmt_buff = NULL;

	// random selector used for random cases below
	size_t random_selector = 0;

	// generate value depending on provided data
	switch(selector)
	{

		// generates ascii buffer in the 'c' 'd' 'e' format
		case BP_PHP_GADGET_BUFFER_ENCODE_ASCII_ONLY:
			return BP_GenUCharAsPHPHex(val, tq);
			break;

		// generates ascii buffer in the chr(decimal) format
		case BP_PHP_GADGET_BUFFER_ENCODE_CHR_ONLY:
			return BP_GenUCharAsPHPChr(val, tq);
			break;

		// generates ascii buffer as 0xff format.
		case BP_PHP_GADGET_BUFFER_ENCODE_HEX_ONLY:
			return BP_GenUCharAsPHPHex(val, tq);
			break;

		// generates an ascii buffer as a normal integer value
		case BP_PHP_GADGET_BUFFER_ENCODE_DECIMAL_ONLY:
			return BP_GenUCharAsPHPDecimal(val, tq);
			break;

		// generates an ascii buffer which is randomly selected as either
		// ascii, hex, or decimal.
		case BP_PHP_GADGET_BUFFER_ENCODE_RANDOM_ASCII_HEX_DECIMAL:
			random_selector = BP_GenRandomUnsignedLong(1,3);
			switch(random_selector)
			{
				case 1:
					return BP_GenUCharAsPHPEscapedASCII(val, tq);
					break;
				case 2:
					return BP_GenUCharAsPHPHex(val, tq);
					break;
				case 3:
					return BP_GenUCharAsPHPDecimal(val, tq);
					break;
				default:
					break;
			}
			break;

		// generates and ascii buffer which is randomly selected between hex
		// and decimal.
		case BP_PHP_GADGET_BUFFER_ENCODE_RANDOM_HEX_DECIMAL:
			random_selector = BP_GenRandomUnsignedLong(1,2);
			switch(random_selector)
			{
				case 1:
					return BP_GenUCharAsPHPHex(val, tq);
					break;
				case 2:
					return BP_GenUCharAsPHPDecimal(val, tq);
					break;
				default:
					break;
			}
			break;

		// generates an ascii buffer which is either an ascii char, or
		// a decimal value.
		case BP_PHP_GADGET_BUFFER_ENCODE_RANDOM_ASCII_DECIMAL:
			random_selector = BP_GenRandomUnsignedLong(1,2);
			switch(random_selector)
			{
				case 1:
					return BP_GenUCharAsPHPEscapedASCII(val, tq);
					break;
				case 2:
					return BP_GenUCharAsPHPDecimal(val, tq);
					break;
				default:
					break;
			}
			break;

			// generates an ascii buffer which is either an ascii char, or
			// a chr(decimal) value
			case BP_PHP_GADGET_BUFFER_ENCODE_RANDOM_ASCII_CHR:
				random_selector = BP_GenRandomUnsignedLong(1,2);
				switch(random_selector)
				{
					case 1:
						return BP_GenUCharAsPHPEscapedASCII(val, tq);
						break;
					case 2:
						return BP_GenUCharAsPHPChr(val, tq);
						break;
					default:
						break;
				}
				break;

		// generates an ascii buffer which is either an ascii char, or
		// an 0xFF hex encoded value.
		case BP_PHP_GADGET_BUFFER_ENCODE_RANDOM_ASCII_HEX:
			random_selector = BP_GenRandomUnsignedLong(1,2);
			switch(random_selector)
			{
				case 1:
					return BP_GenUCharAsPHPEscapedASCII(val, tq);
					break;
				case 2:
					return BP_GenUCharAsPHPHex(val, tq);
					break;
				default:
					break;
			}
			break;

		case BP_PHP_GADGET_BUFFER_ENCODE_RANDOM_HEX_DECIMAL_ORD:
			random_selector = BP_GenRandomUnsignedLong(1,3);
			switch(random_selector)
			{
				case 1:
					return BP_GenUCharAsPHPDecimal(val, tq);
					break;
				case 2:
					return BP_GenUCharAsPHPHex(val, tq);
					break;
				case 3:
					return BP_GenUCharAsPHPOrd(val, tq);
				default:
					break;
			}
			break;
		case BP_PHP_GADGET_BUFFER_ENCODE_RANDOM_HEX_ORD:
			random_selector = BP_GenRandomUnsignedLong(1,2);
			switch(random_selector)
			{
				case 1:
					return BP_GenUCharAsPHPHex(val, tq);
					break;
				case 2:
					return BP_GenUCharAsPHPOrd(val, tq);
				default:
					break;
			}
			break;
		case BP_PHP_GADGET_BUFFER_ENCODE_RANDOM_DECIMAL_ORD:
			random_selector = BP_GenRandomUnsignedLong(1,2);
			switch(random_selector)
			{
				case 1:
					return BP_GenUCharAsPHPDecimal(val, tq);
					break;
				case 2:
					return BP_GenUCharAsPHPOrd(val, tq);
				default:
					break;
			}
			break;

		// default case, return NULL
		default:
			return NULL;

	}

	// should never get here, values should be returned in the
	// above switch/case.
	return NULL;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Number->Code Encoders (translate strings to code) %%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


char * BP_GenPHPGadgetEncodeNumberAsHex(size_t val, P_BP_LINKL tq)
{

	// first attempt to convert the buffer
	char *val_out = BP_GenHexStringFromBuffer((unsigned char *) &val, sizeof(size_t), tq);
	if(!val_out)
		return NULL;

	// pointer used for returning tq data
	char *ret_val = NULL;

	// if we have a specified tail queue, use it, if not, just
	// return the chunk immediately (no free).
	if(tq)
	{
		// add the chunk to the list
		char *ret_val = tq_strdup(tq, val_out, BPLN);

		// free the old value
		bpfree(val_out);
	}
	else
		return val_out;

	// return the tq value if set
	return ret_val;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Buffer->Code Encoders (translate strings to code) %%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Encodes a buffer as pack("c*", 0x41, 65,'A') encoded string
char * BP_GenPHPGadgetEncodeBufferAsPack(unsigned char * buff, size_t buff_size, BP_PHP_GADGET_BUFFER_ENCODE_SELECTOR encode_selector, P_BP_LINKL tq)
{

	// ensure we have a buffer and a buffer size
	if(!buff || !buff_size)
		return NULL;

	// set tail queue allocator here
	P_BP_LINKL tq_in = BP_CreateLinkL("intq", BP_FALSE);

	// out buffer
	char *out_buff = NULL;

	// add the front part of the string
	tq_strdup(tq_in, "pack('c*',", BPLN);

	size_t n = 0;
	for(; n < buff_size; n++)
	{

		// encode the chars
		BP_GenPHPGadgetEncodeUChar(buff[n], encode_selector, tq_in);

		// add chunk as comma
		if((n+1) < buff_size)
			tq_strdup(tq_in, ",", BPLN);

	}

	// add trailing parenthesis
	tq_strdup(tq_in, ")", BPLN);

	// flatten the tailqueue to create string
	BP_LinkLFlattenJoinStringElements(tq_in);

	// duplicate the generated flat buffer on the passed tq
	// and store it as the return buffer.
	if(tq)
		out_buff = tq_strdup(tq, (char *) tq_in->flat_buff, BPLN);
	else
		out_buff = bpstrdup((char *) tq_in->flat_buff);


	// destroy the inner linkl
	BP_DestroyLinkL(tq_in, BP_TRUE);

	// return the packed buffer
	return out_buff;

}


// encodes a buffer by using an imploding array of chr(dec)
char * BP_GenPHPGadgetEncodeBufferAsImplodingArray(unsigned char * buff, size_t buff_size, P_BP_LINKL tq)
{

	// ensure we have a buffer and a buffer size
	if(!buff || !buff_size)
		return NULL;

	// set tail queue allocator here
	P_BP_LINKL tq_in = BP_CreateLinkL("intq", BP_FALSE);

	// out buffer
	char *out_buff = NULL;

	// add the front part of the string
	tq_strdup(tq_in, "implode(array(", BPLN);

	size_t n = 0;
	for(; n < buff_size; n++)
	{

		// encode the chars
		BP_GenPHPGadgetEncodeUChar(buff[n], BP_PHP_GADGET_BUFFER_ENCODE_RANDOM_ASCII_CHR, tq_in);

		// add chunk as comma
		if((n+1) < buff_size)
			tq_strdup(tq_in, ",", BPLN);

	}

	// add trailing parenthesis
	tq_strdup(tq_in, "))", BPLN);

	// flatten the tailqueue to create string
	BP_LinkLFlattenJoinStringElements(tq_in);

	// duplicate the generated flat buffer on the passed tq
	// and store it as the return buffer.
	if(tq)
		out_buff = tq_strdup(tq, (char *) tq_in->flat_buff, BPLN);
	else
		out_buff = bpstrdup((char *) tq_in->flat_buff);

	// destroy the inner linkl
	BP_DestroyLinkL(tq_in, BP_TRUE);

	// return the packed buffer
	return out_buff;

}

// creates a set of chr(val).'A' values, randomly selected.
char * BP_GenPHPGadgetEncodeBufferAsDotJoinedChars(unsigned char * buff, size_t buff_size, P_BP_LINKL tq)
{

	// ensure we have a buffer and a buffer size
	if(!buff || !buff_size)
		return NULL;

	// set tail queue allocator here
	P_BP_LINKL tq_in = BP_CreateLinkL("intq", BP_FALSE);

	// out buffer
	char *out_buff = NULL;

	size_t n = 0;
	for(; n < buff_size; n++)
	{

		// encode the chars
		BP_GenPHPGadgetEncodeUChar(buff[n], BP_PHP_GADGET_BUFFER_ENCODE_RANDOM_ASCII_CHR, tq_in);

		// add chunk as comma
		if((n+1) < buff_size)
		{
			tq_strdup(tq_in, ".", BPLN);
		}

	}

	// flatten the tailqueue to create string
	BP_LinkLFlattenJoinStringElements(tq_in);

	// duplicate the generated flat buffer on the passed tq
	// and store it as the return buffer.
	if(tq)
		out_buff = tq_strdup(tq, (char *) tq_in->flat_buff, BPLN);
	else
		out_buff = bpstrdup((char *) tq_in->flat_buff);

	// destroy the inner linkl
	BP_DestroyLinkL(tq_in, BP_TRUE);

	// return the packed buffer
	return out_buff;

}


// encodes a buffer by using an imploding array of chr(dec)
char * BP_GenPHPGadgetEncodeBufferAsImplodingSScanf(unsigned char * buff, size_t buff_size, P_BP_LINKL tq)
{

	// ensure we have a buffer and a buffer size
	if(!buff || !buff_size)
		return NULL;

	// set tail queue allocator here
	P_BP_LINKL tq_in = BP_CreateLinkL("intq", BP_FALSE);

	// out buffer
	char *out_buff = NULL;

	// add the front part of the string
	tq_strdup(tq_in, "implode(sscanf(", BPLN);


	// creates a set of chr(val).'A' values, randomly selected.
	BP_GenPHPGadgetEncodeBufferAsDotJoinedChars(buff, buff_size, tq_in);

	// add trailing parenthesis
	tq_strdup(tq_in, ", \"%[^\\n]\"))", BPLN);

	// flatten the tailqueue to create string
	BP_LinkLFlattenJoinStringElements(tq_in);

	// duplicate the generated flat buffer on the passed tq
	// and store it as the return buffer.
	if(tq)
		out_buff = tq_strdup(tq, (char *) tq_in->flat_buff, BPLN);
	else
		out_buff = bpstrdup((char *) tq_in->flat_buff);

	// destroy the inner linkl
	BP_DestroyLinkL(tq_in, BP_TRUE);

	// return the packed buffer
	return out_buff;

}


// generates a sprintf statement with hex encoding
char * BP_GenPHPGadgetEncodeBufferAsSprintf(unsigned char * buff, size_t buff_size, P_BP_LINKL tq)
{

	// ensure we have a buffer and a buffer size
	if(!buff || !buff_size)
		return NULL;

	// set tail queue allocator here
	P_BP_LINKL tq_in = BP_CreateLinkL("intq", BP_FALSE);

	// out buffer
	char *out_buff = NULL;

	// add the front part of the string
	tq_strdup(tq_in, "sprintf(\"%s\", ", BPLN);

	// creates a set of chr(val).'A' values, randomly selected.
	BP_GenPHPGadgetEncodeBufferAsDotJoinedChars(buff, buff_size, tq_in);

	// add trailing parenthesis
	tq_strdup(tq_in, ")", BPLN);

	// flatten the tailqueue to create string
	BP_LinkLFlattenJoinStringElements(tq_in);

	// duplicate the generated flat buffer on the passed tq
	// and store it as the return buffer.
	if(tq)
		out_buff = tq_strdup(tq, (char *) tq_in->flat_buff, BPLN);
	else
		out_buff = bpstrdup((char *) tq_in->flat_buff);

	// destroy the inner linkl
	BP_DestroyLinkL(tq_in, BP_TRUE);

	// return the packed buffer
	return out_buff;

}


// generates a base64_decode("base64_string") statement
char * BP_GenPHPGadgetEncodeBufferAsBase64Decode(unsigned char * buff, size_t buff_size, P_BP_LINKL tq)
{

	// ensure we have a buffer and a buffer size
	if(!buff || !buff_size)
		return NULL;

	// set tail queue allocator here
	P_BP_LINKL tq_in = BP_CreateLinkL("intq", BP_FALSE);

	// out buffer
	char *out_buff = NULL;

	// add the front part of the string
	tq_strdup(tq_in, "base64_decode(\"", BPLN);

	// encode the buffer
	char * base64_str = BP_Base64Encode(buff, buff_size);

	// add the base64 string
	tq_strdup(tq_in, base64_str, BPLN);

	// destroy the base64 string (not the duplicate in the tail queue, but the orig alloc)
	bpfree(base64_str);

	// add trailing parenthesis
	tq_strdup(tq_in, "\")", BPLN);

	// flatten the tailqueue to create string
	BP_LinkLFlattenJoinStringElements(tq_in);

	// duplicate the generated flat buffer on the passed tq
	// and store it as the return buffer.
	if(tq)
		out_buff = tq_strdup(tq, (char *) tq_in->flat_buff, BPLN);
	else
		out_buff = bpstrdup((char *) tq_in->flat_buff);

	// destroy the inner linkl
	BP_DestroyLinkL(tq_in, BP_TRUE);

	// return the packed buffer
	return out_buff;
}

// Selects a random buffer encoder from all of the available php encoders, and encodes
// the string using that.
char * BP_GenPHPGadgetEncodeBufferAsRandomEncode(unsigned char * buff, size_t buff_size, P_BP_LINKL tq)
{

	if(!buff || !buff_size)
		return NULL;

	// generate random
	size_t selector = BP_GenRandomUnsignedLong(1,6);

	switch(selector)
	{
		// generates a set of pack('c*', 0xval, dec) values
		case 1:
			return BP_GenPHPGadgetEncodeBufferAsPack(buff, buff_size, BP_PHP_GADGET_BUFFER_ENCODE_RANDOM_HEX_DECIMAL, tq);
			break;

		// generates a set of implode(array(chr(dec),chr(dec2)) values
		case 2:
			return BP_GenPHPGadgetEncodeBufferAsImplodingArray(buff, buff_size, tq);
			break;

		// generates a imploding scanf with hex encoding
		case 3:
			return BP_GenPHPGadgetEncodeBufferAsImplodingSScanf(buff, buff_size, tq);
			break;

		// generates a sprintf statement with hex encoding
		case 4:
			return BP_GenPHPGadgetEncodeBufferAsSprintf(buff, buff_size, tq);
			break;

		// generates a randomly selected buff
		case 5:
			return BP_GenPHPGadgetEncodeBufferAsDotJoinedChars(buff, buff_size, tq);
			break;

		// generates a base64_decode("base64_string") statement
		case 6:
			return BP_GenPHPGadgetEncodeBufferAsBase64Decode(buff, buff_size, tq);
			break;


		default:
			break;
	}
	return NULL;
}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Format Wrappers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

const char * BP_COLOR_CODE_PREFIX = "\x1b[";
const char * BP_COLOR_CODE_AFFIX  = "m";
const char * BP_COLOR_CODE_SEPARATOR = ";";

// foreground colors
const char * BP_FOREGROUND_COLOR_CODES[] =
{
	NULL,
	"0",
	"31",
	"32",
	"33",
	"34",
	"35",
	"36",
	"37"
};

// background colors
const char * BP_BACKGROUND_COLOR_CODES[] =
{
	NULL,
	"40",
	"41",
	"42",
	"43",
	"44",
	"45",
	"46",
	"47"
};

// Simple wrapper for printf
BP_ERROR_T BP_printf(const char * fmt, ...)
{

	// exit here if no format is supplied
	if(!fmt)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}


// returns an array full of alternating pointers to the color code sets found in codes.
P_BP_COLOR_CODE_SET_T * BP_GenerateAlternatingColorStack(P_BP_COLOR_CODE_SET_T codes, size_t code_count, size_t num_to_generate_as_alterating, P_BP_LINKL tq)
{

	// ensure we have codes and a code count
	if(!codes || !code_count || !num_to_generate_as_alterating)
	{
		return NULL;
	}

	P_BP_COLOR_CODE_SET_T * ret_codes = NULL;

	if(tq)
		ret_codes = (P_BP_COLOR_CODE_SET_T *) tq_calloc(tq, sizeof(P_BP_COLOR_CODE_SET_T) * (num_to_generate_as_alterating+1), 1, BPLN);
	else
		ret_codes = (P_BP_COLOR_CODE_SET_T *) bpcalloc(sizeof(P_BP_COLOR_CODE_SET_T) * (num_to_generate_as_alterating+1), 1);

	// ensure we allocated the data ok
	if(!ret_codes)
		return NULL;

	// this is the iterator that moves between items in the code set which
	// was passed into this routine.
	size_t code_iter = 0;

	// walk the total number
	size_t n = 0;
	for(; n < num_to_generate_as_alterating; n++, code_iter++)
	{

		// reset the iterator if we hit our top
		if(code_iter == code_count)
			code_iter = 0;

		// set the ret codes
		ret_codes[n] = &codes[code_iter];

	}

	// return the codes
	return ret_codes;

}



// Generates a heap buffer storing the color prefix data for a color set.  Result
// must be freed using bpfree (or tq_free if linked into a linkl)
char * BP_GenerateTerminalColorPrefixFromColorSet(P_BP_COLOR_CODE_SET_T colors)
{

	// ensure we have a valid pointer with fg/bg data.
	if(!colors)
		return NULL;
	if(!colors->bg && !colors->fg)
		return NULL;

	// create return buffer
	char * prefix = (char *)  bpcalloc(255,1);
	if(!prefix)
		return NULL;

	// flag indicating whether or not the sequence has been truncated
	BP_BOOL color_truncated = BP_FALSE;

	// add prefix if we have a foreground/background color
	if(colors->fg || colors->bg)
	{

		// add prefix
		strcat(prefix, BP_COLOR_CODE_PREFIX);

		// add foreground color if necessary
		if(colors->fg)
		{
			strcat(prefix, BP_FOREGROUND_COLOR_CODES[colors->fg] );
			if(!colors->bg)
				strcat(prefix, "m");
		}

		if(colors->bg)
		{

			if(!colors->fg)
			{
				strcat(prefix, BP_FOREGROUND_COLOR_CODES[BP_COLOR_CODE_BG_WHITE]);
				strcat(prefix, ";");
			}
			else
			{
				strcat(prefix, ";");
			}
			strcat(prefix, BP_BACKGROUND_COLOR_CODES[colors->bg]);
			strcat(prefix, "m");
		}


	}

	// declare prefix and format length
	size_t prefix_len = 0;

	// calculate prefix and fmt
	prefix_len = bpstrlen(prefix);

	// trim off the remaining bytes (as 255 bytes is likely too long)
	prefix = (char *) bprealloc((void *) prefix, prefix_len+1);
	if(!prefix)
		return NULL;

	// ensure the realloc space is null terminated
	prefix[prefix_len] = (char) 0x00;

	// return the buffer
	return prefix;

}

// Colorizes a string based around the colors passed in.  Each character must have a
// matching colors pointer entry.  This means that the string length must equal the number
// of pointers available.  Don't mess with this routine, it's perfect.
char * BP_ColorizeString(char * string, size_t string_len, P_BP_COLOR_CODE_SET_T * colors, BP_BOOL terminate_with_color_reset)
{

	if(!string || !string_len || !colors)
		return NULL;

	// string to be returned
	char * out_str = NULL;

	// LinkL used for pushing entries.
	P_BP_LINKL tq = BP_CreateLinkL(".", BP_FALSE);

	// this is generated once per loop
	char *color_prefix = NULL;

	// this stores the last generated color prefix, it's compared
	// with the current color prefix to determine if we need to re-colorize.
	char *last_color_prefix = NULL;

	// string length must match colors
	size_t n = 0;
	for(; n < string_len; n++)
	{

		// generate color prefix


		// color must be set
		if(colors[n])
		{
			color_prefix = BP_GenerateTerminalColorPrefixFromColorSet(colors[n]);
			if(color_prefix)
			{
				// link the color in
				tq_strdup(tq, color_prefix, BPLN);
			}
		}

		// link in the character
		tq_strndup(tq, &string[n], 1, BPLN);

		// destroy the color prefix on loop
		if(color_prefix)
		{

			bpfree(color_prefix);
			color_prefix = NULL;

			// reset terminal data if set
			if(terminate_with_color_reset == BP_TRUE)
				tq_strdup(tq, BP_COLOR_CODE_RESET_DEFAULT_COLORS, BPLN);

		}

	}



	// flatten the tq (creates the final colorized string)
	BP_LinkLFlattenJoinStringElements(tq);

	// set the buffer to return as the new flat buffer
	out_str = (char *) tq->flat_buff;

	// generate the return buffer here on the normal heap, aka
	// copy it from the tq.
	if(out_str)
		out_str = bpstrdup(out_str);

	// destroy the linkl
	BP_DestroyLinkL(tq, BP_TRUE);

	// return the string
	return out_str;

}

// If blockparty is in a debug configuration, this will print out debug info,
// if the library is not in a debug configuration, this will print nothing.
BP_ERROR_T BP_dbg_printf(const char * fmt, ...)
{

#if BP_LIBRARY_DEBUG_CONFIGURATION_ENABLED == 1

	// variable argument list
	va_list args;
	va_start(args, fmt);

	// restore the normal colors
	vfprintf(stdout, fmt, args);

	// end vargs
	va_end(args);

#endif

	// return indicating success
	return ERR_SUCCESS;

}


// Wrapper for printf using VT colors
BP_ERROR_T BP_printfVTColor(const char * fmt,  BP_FOREGROUND_COLOR_CODE_T fgc, BP_BACKGROUND_COLOR_CODE_T bgc, ...)
{

	// exit here if no format is supplied
	if(!fmt)
		return ERR_FAILURE;

	// reset terminal color before print
	BP_RESET_TERMINAL_COLOR;

	// variable argument list
	va_list args;
	va_start(args, fmt);

	// create and zero out the stack prefix
	char prefix[255];
	BP_ZERO_STACK(prefix);

	// flag indicating whether or not the sequence has been truncated
	BP_BOOL color_truncated = BP_FALSE;

	// add prefix if we have a foreground/background color
	if(fgc || bgc)
	{

		// add prefix
		strcat(prefix, BP_COLOR_CODE_PREFIX);

		// add foreground color if necessary
		if(fgc)
		{
			strcat(prefix, BP_FOREGROUND_COLOR_CODES[fgc] );
			if(!bgc)
				strcat(prefix, "m");
		}

		if(bgc)
		{

			if(!fgc)
			{
				strcat(prefix, BP_FOREGROUND_COLOR_CODES[BP_COLOR_CODE_BG_WHITE]);
				strcat(prefix, ";");
			}
			else
			{
				strcat(prefix, ";");
			}
			strcat(prefix, BP_BACKGROUND_COLOR_CODES[bgc]);
			strcat(prefix, "m");
		}


	}

	// declare prefix and format length
	size_t prefix_len = 0;
	size_t fmt_len    = 0;

	// calculate prefix and fmt
	prefix_len = bpstrlen(prefix);
	fmt_len    = bpstrlen(fmt);


	// allocate memory
	char *out_fmt = (char *) bpcalloc(prefix_len+fmt_len+1, 1);

	// copy in the data
	if(prefix_len)
		strcat(out_fmt, prefix);
	if(fmt_len)
		strcat(out_fmt, fmt);

	// restore the normal colors
	vfprintf(stdout, out_fmt, args);

	// end vargs
	va_end(args);

	// now reset terminal colors
	BP_RESET_TERMINAL_COLOR;


	// destroy the buffer before exit
	if(out_fmt)
		bpfree(out_fmt);

	// return indicating success
	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% String Trimming %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempt to trim whitepace off the ends of a string (uses BP_StringTrim
// under the hood)
char * BP_StringTrimWhitespace
(
		char * str_to_trim,
		BP_STRING_TRIM_DIRECTION trim_direction,
		size_t  max_trim
)
{

	// NOTE: no basic checks since BP_StringTrim does
	// sanitization.


	// attempt to trim string
	return BP_StringTrim
	(
			str_to_trim,
			trim_direction,
			BP_TRUE,
			BP_TRUE,
			BP_TRUE,
			BP_TRUE,
			'\0',
			max_trim
	);

}

// This routine will attempt to trim a REASONABLE
// string based around parameters.  Max trim is the
// maximum trim depth this will allow.
char * BP_StringTrim
(
		char * str_to_trim,
		BP_STRING_TRIM_DIRECTION trim_direction,
		BP_BOOL trim_newline,
		BP_BOOL trim_tab,
		BP_BOOL trim_space,
		BP_BOOL trim_carriage_return,
		char    trim_arbitrary_char,
		size_t  max_trim
)
{


	// trim front
	if(!str_to_trim)
		return NULL;
	if(!trim_direction)
		return NULL;

	// attempt to calculate trim length
	size_t str_to_trim_len = bpstrlen(str_to_trim);

	// string must be deemed reasonable before any processing occurs
	if(!BP_StringIsReasonablePrintableString(str_to_trim, bpstrlen(str_to_trim), BP_TRUE, BP_TRUE))
		return NULL;



	// string counter
	size_t n = 0;

	// new string pointer
	char * new_str = NULL;

	// size placeholder for new string
	size_t new_str_len = 0;

	// final string which will be returned (trimmed string)
	char * final_string = NULL;

	// tracks the total number of times a character was trimmed
	size_t total_trims = 0;


	// trim right if user specified (simple nullify)
	if(trim_direction == BP_STRING_TRIM_DIRECTION_RIGHT || trim_direction == BP_STRING_TRIM_DIRECTION_BOTH)
	{

		// LOOP NOTE: n indexes must be -1 in the right-end loop due to bpstrlen returning a +1 index.

		// allocate new string
		new_str     = bpstrdup(str_to_trim);
		new_str_len = bpstrlen(new_str);

		// nullify trailing whitespace
		for(n = new_str_len; n; n--)
		{

			// stop loop if we hit our cap on total trims
			if(max_trim <= total_trims)
				break;

			// run newline check
			if(trim_newline)
			if(new_str[n-1] == '\n')
			{
				new_str[n-1] = '\0';
				total_trims++;
				continue;
			}

			// run carriage return check
			if(trim_carriage_return)
			if(new_str[n-1] == '\r')
			{
				new_str[n-1] = '\0';
				total_trims++;
				continue;
			}

			// run space check
			if(trim_space)
			if(new_str[n-1] == ' ')
			{
				new_str[n-1] = '\0';
				total_trims++;
				continue;
			}

			// run tab check
			if(trim_tab)
			if(new_str[n-1] == '\t')
			{
				new_str[n-1] = '\0';
				total_trims++;
				continue;
			}

			if(trim_arbitrary_char)
			if(new_str[n-1] == trim_arbitrary_char)
			{
				new_str[n-1] = '\0';
				total_trims++;
				continue;
			}

			// if no characters match, we can break here
			break;

		}

	}

	// debug verbosity
	//if(new_str)
	//	printf("\n [!!] Got after end replace???: \"%s\"", new_str);


	// trim left if user specified
	if(trim_direction == BP_STRING_TRIM_DIRECTION_LEFT || trim_direction == BP_STRING_TRIM_DIRECTION_BOTH)
	{

		// allocate new string if necessary
		if(!new_str)
		{
			new_str     = bpstrdup(str_to_trim);
			new_str_len = str_to_trim_len;
		}
		else
		{

			// recalculate length just in case we
			// nullified any tail characters above.
			new_str_len = bpstrlen(new_str);

		}

		// nullify trailing whitespace
		for(n = 0; n < new_str_len; n++)
		{

			// stop loop if we hit our cap on total trims
			if(max_trim <= total_trims)
				break;

			// run newline check
			if(trim_newline)
			if(new_str[n] == '\n')
			{
				total_trims++;
				continue;
			}

			// run carriage return check
			if(trim_carriage_return)
			if(new_str[n] == '\r')
			{
				total_trims++;
				continue;
			}

			// run space check
			if(trim_space)
			if(new_str[n] == ' ')
			{
				total_trims++;
				continue;
			}

			// run tab check
			if(trim_tab)
			if(new_str[n] == '\t')
			{
				total_trims++;
				continue;
			}

			if(trim_arbitrary_char)
			if(new_str[n] == trim_arbitrary_char)
			{
				total_trims++;
				continue;
			}

			// if no characters match, we've reached
			// the end and can break.
			break;

		}

		// set final string
		final_string = bpstrdup((char *) &new_str[n]);

	}

	// if we haven't created any new strings, no replacements
	// took place.
	if(!new_str)
		return NULL;
	if(!bpstrlen(new_str))
	{
		bpfree(new_str);
		return NULL;
	}

	// dup the final string from newstring if only
	// the tailpart was fixed up.
	if(!final_string)
		final_string = bpstrdup(new_str);

	// free the new string
	bpfree(new_str);

	// return the final prepared string (trimmed)
	return final_string;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Safer String Length Drop in Replacements %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// attempt to calculate string length (generic drop in replacement for bpstrlen() but stops on any binary characters)
size_t bpstrlen_real(const char *str, BPLN_PARMS)
{


	// ensure we have a string to analyze and a length
	if(!str)
		return 0;
	if(!str[0])
		return 0;

	// iterate through string
	size_t char_iter = 0;
	for(; str[char_iter]; char_iter++)
	{

		// if it's not alphanumeric continue
		if(isalnum(str[char_iter]))
			continue;

		// ensure the character isn't null before doing any tests
		if(!str[char_iter])
			break;

		// switch on allowed chars (long code but fastest pathing)
		switch(str[char_iter])
		{

			// all valid characters pass through ok
			case ' ':
			case '\t':
			case '!':
			case '@':
			case '#':
			case '$':
			case '%':
			case '^':
			case '&':
			case '*':
			case '(':
			case ')':
			case '_':
			case '+':
			case '=':
			case '-':
			case '~':
			case '`':
			case '<':
			case '>':
			case ',':
			case '.':
			case '?':
			case '/':
			case '\\':
			case '"':
			case '\'':
			case ':':
			case ';':
			case '{':
			case '[':
			case ']':
			case '}':
			case '|':
			case '\r':
			case '\n':

				continue;

			// 0x1b is terminal color escape.
			case '\x1b':
				continue;

			// null terminator
			case 0x00:
				break;

			// return zero if the string has a bad char in it somehow (also display stack output)
			default:

				printf("\n [!!] string found with binary data while using bpstrlen_real (should never occur): We are continuing, but please look at this later. ");
				printf("\n - offending char: 0x%.02x at position %u", (unsigned char)  str[char_iter], char_iter);
				printf("\n - func: %s", func);
				printf("\n - line: %u", line_number);
				printf("\n - file: %s", file_name);
				printf("\n");
				continue;

		}

	}

	// return total number of good characters
	return char_iter;

}

// attempt to calculate string length (generic drop in replacement for bpstrlen() but stops on any binary characters)
size_t bpstrnlen_real(const char *str, size_t max_len, BPLN_PARMS)
{

	// ensure we have a string to analyze and a length
	if(!str)
		return 0;
	if(!str[0])
		return 0;

	// iterate through string
	size_t char_iter = 0;
	for(; str[char_iter]; char_iter++)
	{

		// break if we hit our max_len
		if(char_iter >= max_len)
			break;

		// ensure the character isn't null before doing any tests
		if(!str[char_iter])
			break;

		// if it's not alphanumeric continue
		if(isalnum(str[char_iter]))
			continue;

		// switch on allowed chars (long code but fastest pathing)
		switch(str[char_iter])
		{

			// all valid characters pass through ok
			case ' ':
			case '\t':
			case '!':
			case '@':
			case '#':
			case '$':
			case '%':
			case '^':
			case '&':
			case '*':
			case '(':
			case ')':
			case '_':
			case '+':
			case '=':
			case '-':
			case '~':
			case '`':
			case '<':
			case '>':
			case ',':
			case '.':
			case '?':
			case '/':
			case '\\':
			case '"':
			case '\'':
			case ':':
			case ';':
			case '{':
			case '[':
			case ']':
			case '}':
			case '|':
			case '\r':
			case '\n':
				continue;

			// 0x1b is terminal color escape.
			case '\x1b':
				continue;

			// null terminator
			case 0x00:
				break;

			// return zero if the string has a bad char in it somehow (also display stack output)
			default:

				printf("\n [!!] string found with binary data while using bpstrnlen_real (should never occur): ");
				printf("\n - offending char: 0x%.02x at position %u", (unsigned char) str[char_iter], char_iter);
				printf("\n - func: %s", func);
				printf("\n - line: %u", line_number);
				printf("\n - file: %s", file_name);
				printf("\n");
				continue;

		}

	}

	// return total number of good characters
	return char_iter;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Quick Analysis Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Reasonable, is a term I defined.  This means that the string contains only printable characters, and optionally, newlines.  I use this
// routine to do quick checks on user input strings, so that I can make determinations on whether or not the strings should be usable.  This
// relies on the buffer analysis capability of the library to work.  Again, this is a quick use routine that I personally use, and isn't
// something forced upon you.  You can use the buffer analysis capability directly if you prefer.
BP_BOOL BP_StringIsReasonablePrintableString
(
		char *  string_to_analyze,
		size_t  len,
		BP_BOOL allow_newlines,
		BP_BOOL allow_carriage_returns
)
{

	// ensure we have a string to analyze and a length
	if(!string_to_analyze || !len)
		return BP_FALSE;
	if(!bpstrlen(string_to_analyze))
		return BP_FALSE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% New Refactored Analyzer (no-heap usage/fast as possible) %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// iterate through string
	size_t char_iter = 0;
	for(; char_iter < len; char_iter++)
	{

		// ensure position is not null
		if(!string_to_analyze[char_iter])
		{
			// __asm("int3");
			// printf("\n [+] The bad char is: at index %u --> %.02x", char_iter, string_to_analyze[char_iter]);
			return BP_FALSE;
		}

		// if it's not alphanumeric continue
		if(isalnum(string_to_analyze[char_iter]))
			continue;

		// !@#$%^&*()_+=-~`<>,.?/"':;{[]}\|

		// switch on allowed chars (long but fastest pathing)
		switch(string_to_analyze[char_iter])
		{
			case ' ':
			case '\t':
			case '!':
			case '@':
			case '#':
			case '$':
			case '%':
			case '^':
			case '&':
			case '*':
			case '(':
			case ')':
			case '_':
			case '+':
			case '=':
			case '-':
			case '~':
			case '`':
			case '<':
			case '>':
			case ',':
			case '.':
			case '?':
			case '/':
			case '\\':
			case '"':
			case '\'':
			case ':':
			case ';':
			case '{':
			case '[':
			case ']':
			case '}':
			case '|':
				continue;

			// process newlines
			case '\n':
				if(allow_newlines)
					continue;
				else
				{
					// printf("\n [+] The bad char is: %.02x", string_to_analyze[char_iter]);
					return BP_FALSE;
				}

			// process carriage returns
			case '\r':
				if(allow_carriage_returns)
					continue;
				else
				{
					// printf("\n [+] The bad char is: %.02x", string_to_analyze[char_iter]);
					return BP_FALSE;
				}
				break;

			// return false if we don't have any matches
			default:
				//printf("\n [+] The bad char is: %.02x", string_to_analyze[char_iter]);
				return BP_FALSE;

		}

	}

	// return indicating success
	return BP_TRUE;

}



// This attempts to determine if a string is reasonable with regard to a string which may become a unix filesystem path.  This routine
// will ensure that the string is a reasonable/printable string (using BP_StringIsReasonablePrintableString) but will run secondary
// character checks to make sure a user isn't sending in malformed path data.  The attempt_to_stat_file parameter will attempt to actually
// touch the file on the filesystem if it's enabled (disabled by default)
BP_BOOL BP_StringIsReasonableForUnixEXT4Directory
(
		char * directory_path_to_analyze,
		size_t len,
		BP_BOOL attempt_to_stat_directory
)
{

	// ensure we have a directory and a length
	if(!directory_path_to_analyze || !len)
		return BP_FALSE;


	// attempt to verify that the string is reasonable first
	if(!BP_StringIsReasonablePrintableString(directory_path_to_analyze, len, BP_FALSE, BP_FALSE))
		return BP_FALSE;

	// create ascii configuration
	BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION ascii_config;

	// zero out the structure (automagic macro)
	BP_ZERO_STACK(ascii_config);

	// set some flags in the structure to count ascii (we already counted invalid with our initial check)
	ascii_config.count_ascii                      = BP_TRUE;

	// attempt to generate analysis
	P_BP_BUFFER_ANALYSIS buff_analysis = BP_CreateBufferAnalysis
	(
			(unsigned char *) directory_path_to_analyze,
			(size_t) len,
			&ascii_config
	);

	// ensure we h ave a buffer analysis
	if(!buff_analysis)
		return BP_FALSE;


	// ensure buffer doesn't have any special chars which could
	// play havoc with later parsing.
	if
	(
			buff_analysis->ascii->char_counts['\\'] ||
			buff_analysis->ascii->char_counts['`' ] ||
			buff_analysis->ascii->char_counts['|' ] ||
			buff_analysis->ascii->char_counts['}' ] ||
			buff_analysis->ascii->char_counts['{' ] ||
			buff_analysis->ascii->char_counts[')' ] ||
			buff_analysis->ascii->char_counts['(' ] ||
			buff_analysis->ascii->char_counts['*' ] ||
			buff_analysis->ascii->char_counts['>' ] ||
			buff_analysis->ascii->char_counts['<' ] ||
			buff_analysis->ascii->char_counts[';' ] ||
			buff_analysis->ascii->char_counts[':' ] ||
			buff_analysis->ascii->char_counts[',' ] ||
			buff_analysis->ascii->char_counts['^' ] ||
			buff_analysis->ascii->char_counts['%' ] ||
			buff_analysis->ascii->char_counts['#' ] ||
			buff_analysis->ascii->char_counts['@' ] ||
			buff_analysis->ascii->char_counts['!' ] ||
			buff_analysis->ascii->char_counts['=' ] ||
			buff_analysis->ascii->char_counts['+' ] ||
			buff_analysis->ascii->char_counts['?' ] ||
			buff_analysis->ascii->char_counts['"' ] ||
			buff_analysis->ascii->char_counts['\''] ||
			buff_analysis->ascii->char_counts['$' ] ||
			buff_analysis->ascii->char_counts['&' ]
	)
	{

		// destroy the buffer analysis
		BP_DestroyBufferAnalysis(buff_analysis);

		// return indicating the path is bad
		return BP_FALSE;

	}

	// destroy the buffer analysis
	BP_DestroyBufferAnalysis(buff_analysis);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Stat if Desired %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(attempt_to_stat_directory)
	{

		// create structure to hold stat
		struct stat dir_stat_info;
		BP_ZERO_STACK(dir_stat_info);

		// lstat returns err on non-zero, so return false here if we fail a stat
		// for arbitrary reasons.
		if(stat(directory_path_to_analyze, &dir_stat_info))
			return BP_FALSE;

		// ensure that the destination is a directory
		if(!S_ISDIR(dir_stat_info.st_mode))
			return BP_FALSE;

	}

	// return indicating that the path is suitable
	return BP_TRUE;
}

// This attempts to determine if a string is reasonable with regard to a string which may become a unix filesystem path.  This routine
// will ensure that the string is a reasonable/printable string (using BP_StringIsReasonablePrintableString) but will run secondary
// character checks to make sure a user isn't sending in malformed path data.  The attempt_to_stat_file parameter will attempt to actually
// touch the file on the filesystem if it's enabled (disabled by default)
BP_BOOL BP_StringIsReasonableForUnixEXT4Filename
(
		char * file_path_to_analyze,
		size_t len,
		BP_BOOL attempt_to_stat_file
)
{

	// ensure we have a directory and a length
	if(!file_path_to_analyze || !len)
		return BP_FALSE;



	// attempt to verify that the string is reasonable first
	if(!BP_StringIsReasonablePrintableString(file_path_to_analyze, len, BP_FALSE, BP_FALSE))
		return BP_FALSE;

	// create ascii configuration
	BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION ascii_config;

	// zero out the structure (automagic macro)
	BP_ZERO_STACK(ascii_config);

	// set some flags in the structure to count ascii (we already counted invalid with our initial check)
	ascii_config.count_ascii                      = BP_TRUE;

	// attempt to generate analysis
	P_BP_BUFFER_ANALYSIS buff_analysis = BP_CreateBufferAnalysis
	(
			(unsigned char *) file_path_to_analyze,
			(size_t) len,
			&ascii_config
	);

	// ensure we h ave a buffer analysis
	if(!buff_analysis)
		return BP_FALSE;


	// ensure buffer doesn't have any special chars which could
	// play havoc with later parsing.
	if
	(
			buff_analysis->ascii->char_counts['\\'] ||
			buff_analysis->ascii->char_counts['`' ] ||
			buff_analysis->ascii->char_counts['|' ] ||
			buff_analysis->ascii->char_counts['}' ] ||
			buff_analysis->ascii->char_counts['{' ] ||
			buff_analysis->ascii->char_counts[')' ] ||
			buff_analysis->ascii->char_counts['(' ] ||
			buff_analysis->ascii->char_counts['*' ] ||
			buff_analysis->ascii->char_counts['>' ] ||
			buff_analysis->ascii->char_counts['<' ] ||
			buff_analysis->ascii->char_counts[';' ] ||
			buff_analysis->ascii->char_counts[':' ] ||
			buff_analysis->ascii->char_counts[',' ] ||
			buff_analysis->ascii->char_counts['^' ] ||
			buff_analysis->ascii->char_counts['%' ] ||
			buff_analysis->ascii->char_counts['#' ] ||
			buff_analysis->ascii->char_counts['@' ] ||
			buff_analysis->ascii->char_counts['!' ] ||
			buff_analysis->ascii->char_counts['=' ] ||
			buff_analysis->ascii->char_counts['+' ] ||
			buff_analysis->ascii->char_counts['?' ] ||
			buff_analysis->ascii->char_counts['"' ] ||
			buff_analysis->ascii->char_counts['\''] ||
			buff_analysis->ascii->char_counts['$' ] ||
			buff_analysis->ascii->char_counts['&' ]
	)
	{

		// destroy the buffer analysis
		BP_DestroyBufferAnalysis(buff_analysis);

		// return indicating the path is bad
		return BP_FALSE;

	}

	// destroy the buffer analysis
	BP_DestroyBufferAnalysis(buff_analysis);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Stat if Desired %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(attempt_to_stat_file)
	{

		// create structure to hold stat
		struct stat file_stat_info;
		BP_ZERO_STACK(file_stat_info);

		// lstat returns err on non-zero, so return false here if we fail a stat
		// for arbitrary reasons.
		if(stat(file_path_to_analyze, &file_stat_info))
			return BP_FALSE;

		// ensure that the destination is a file
		if(!S_ISREG(file_stat_info.st_mode))
			return BP_FALSE;


	}

	// return indicating that the path is suitable
	return BP_TRUE;
}

// checks to ensure that a string is reasonable for a json property name
BP_BOOL BP_StringIsReasonableForJSONPropertyNameString
(
		char * json_propname_string_to_analyze,
		size_t json_propname_string_len
)
{

	// attempt to verify that the string is reasonable first
	if(!BP_StringIsReasonablePrintableString(json_propname_string_to_analyze, json_propname_string_len, BP_FALSE, BP_FALSE))
		return BP_FALSE;

	// ensure the first position is alphanumeric
	if(!isalnum(json_propname_string_to_analyze[0]))
		return BP_FALSE;

	// ensure the last position is alphanumeric
	if(!isalnum(json_propname_string_to_analyze[bpstrlen(json_propname_string_to_analyze)-1]))
		return BP_FALSE;

	// create ascii configuration
	BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION ascii_config;
	BP_ZERO_STACK(ascii_config);

	// set some flags in the structure to count ascii (we already counted invalid with our initial check)
	ascii_config.count_ascii                      = BP_TRUE;

	// attempt to generate analysis
	P_BP_BUFFER_ANALYSIS buff_analysis = BP_CreateBufferAnalysis
	(
			(unsigned char *) json_propname_string_to_analyze,
			(size_t)          json_propname_string_len,
			&ascii_config
	);

	// ensure we h ave a buffer analysis
	if(!buff_analysis)
		return BP_FALSE;


	// ensure buffer doesn't have any special chars which could
	// play havoc with later parsing.
	if
	(
			buff_analysis->ascii->char_counts['\\'] ||
			buff_analysis->ascii->char_counts['`' ] ||
			buff_analysis->ascii->char_counts['"' ] ||
			buff_analysis->ascii->char_counts['\'']

	)
	{

		// destroy the buffer analysis
		BP_DestroyBufferAnalysis(buff_analysis);

		// return indicating the path is bad
		return BP_FALSE;

	}

	// destroy the buffer analysis
	BP_DestroyBufferAnalysis(buff_analysis);

	// return indicating success
	return BP_TRUE;

}


// This attempts to determine if a string is reasonable with regard to JSON string output.
BP_BOOL BP_StringIsReasonableForJSONOutputString
(
	char * json_output_string_to_analyze,
	size_t json_output_string_len
)
{

	// attempt to verify that the string is reasonable first
	if(!BP_StringIsReasonablePrintableString(json_output_string_to_analyze, json_output_string_len, BP_FALSE, BP_FALSE))
		return BP_FALSE;

	// create ascii configuration
	BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION ascii_config;
	BP_ZERO_STACK(ascii_config);

	// set some flags in the structure to count ascii (we already counted invalid with our initial check)
	ascii_config.count_ascii                      = BP_TRUE;

	// attempt to generate analysis
	P_BP_BUFFER_ANALYSIS buff_analysis = BP_CreateBufferAnalysis
	(
			(unsigned char *) json_output_string_to_analyze,
			(size_t)          json_output_string_len,
			&ascii_config
	);

	// ensure we h ave a buffer analysis
	if(!buff_analysis)
		return BP_FALSE;


	// ensure buffer doesn't have any special chars which could
	// play havoc with later parsing.
	if
	(
			buff_analysis->ascii->char_counts['\\'] ||
			buff_analysis->ascii->char_counts['`' ] ||
			buff_analysis->ascii->char_counts['"' ] ||
			buff_analysis->ascii->char_counts['\'']

	)
	{

		// destroy the buffer analysis
		BP_DestroyBufferAnalysis(buff_analysis);

		// return indicating the path is bad
		return BP_FALSE;

	}

	// destroy the buffer analysis
	BP_DestroyBufferAnalysis(buff_analysis);

	// return indicating success
	return BP_TRUE;

}

// This attempts to determine if a string is reasonable for an unsigned integer value.
BP_BOOL BP_StringIsReasonableForUnsignedIntegerASCII
(
		char * string_to_analyze,
		size_t string_len
)
{

	// run basic checks
	if(!string_to_analyze)
		return BP_FALSE;
	if(!string_len)
		return BP_FALSE;

	// walk string and look for bad characters
	size_t n = 0;
	for(; n < string_len; n++)
	{
		if(!BP_IS_DIGIT_ASCII(string_to_analyze[n]))
			return BP_FALSE;
	}

	// if all characters are ascii digits return true
	return BP_TRUE;

}

// This attempts to determine if a string is reasonable for a signed integer value.
BP_BOOL BP_StringIsReasonableForSignedIntegerASCII
(
		char * string_to_analyze,
		size_t string_len
)
{

	// run basic checks
	if(!string_to_analyze)
		return BP_FALSE;
	if(!string_len)
		return BP_FALSE;

	// run checks based on positions
	if(string_to_analyze[0] == '-')
	{
		return BP_StringIsReasonableForUnsignedIntegerASCII
		(
			(char *) &string_to_analyze[1],
			string_len-1
		);
	}
	else
	{
		return BP_StringIsReasonableForUnsignedIntegerASCII
		(
			(char *) &string_to_analyze[0],
			string_len-1
		);
	}

	// should never get here
	return BP_FALSE;

}

// This attempts to determine if a string is reasonable for an unsigned integer value.
BP_BOOL BP_StringIsReasonableForHexUpperASCII
(
		char * string_to_analyze,
		size_t string_len
)
{

	// run basic checks
	if(!string_to_analyze)
		return BP_FALSE;
	if(!string_len)
		return BP_FALSE;

	// walk string and look for bad characters
	size_t n = 0;
	for(; n < string_len; n++)
	{
		if(!BP_IS_UPPER_HEX_ASCII(string_to_analyze[n]))
			return BP_FALSE;
	}

	// if all characters are ascii hex return true
	return BP_TRUE;

}



// This attempts to see if a string only contains hexidecimal characters.
BP_BOOL BP_StringIsReasonableForHexLowerASCII
(
		char * string_to_analyze,
		size_t string_len
)
{

	// run basic checks
	if(!string_to_analyze)
		return BP_FALSE;
	if(!string_len)
		return BP_FALSE;

	// walk string and look for bad characters
	size_t n = 0;
	for(; n < string_len; n++)
	{
		if(!BP_IS_LOWER_HEX_ASCII(string_to_analyze[n]))
			return BP_FALSE;
	}

	// if all characters are ascii hex return true
	return BP_TRUE;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Tab Buffer Generation %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to create a tab buffer at tab_depth length.  If store_in_linkl is used, the buffer
// will be created via the linkl.  If not, it will be created using bpcalloc, and will thus need
// to be free'd.
char * BP_CreateTabBuffer(size_t tab_depth, P_BP_LINKL store_in_linkl)
{

	// tab depth must be non-zero
	if(tab_depth == 0)
		return NULL;

	// set return buff
	char * ret_buff = (char *) NULL;

	// create tab buff
	char tab_buff[tab_depth+1];
	BP_ZERO_STACK(tab_buff);

	// fill in tabs
	memset((char *) &tab_buff, '\t', tab_depth);

	// store in the linked list
	if(store_in_linkl)
	{

		// duplicate in linkl if set
		ret_buff = tq_strdup(store_in_linkl, (char *) &tab_buff, BPLN);
		return ret_buff;

	}

	// set the return buffer
	ret_buff = (char *) bpstrdup((char *) &tab_buff);
	return ret_buff;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Null Terminated String Compares/Matching %%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Checks if two strings are the same.  Strings must be reasonable/printable or this routine will return false.  This
// is a slightly safer version of strcmp.  Strings which are not null terminated can still result in access violations.
BP_BOOL BP_StringsMatchAsReasonableStrings(char *str1, char *str2)
{

	// ensure pointers are not
	if(!str1)
		return BP_FALSE;
	if(!str2)
		return BP_FALSE;

	// get basic length checks (can potentially fault on unterminated strings)
	size_t str1_len = bpstrlen(str1);
	size_t str2_len = bpstrlen(str2);

	// lengths must match
	if(str1_len != str2_len)
		return BP_FALSE;

	// ensure both strings are reasonable/printable
	if(!BP_StringIsReasonablePrintableString(str1, str1_len, BP_TRUE, BP_TRUE))
		return BP_FALSE;
	if(!BP_StringIsReasonablePrintableString(str2, str2_len, BP_TRUE, BP_TRUE))
		return BP_FALSE;

	// ensure that both strings match
	if(memcmp(str1, str2, str1_len) != 0)
		return BP_FALSE;

	// return indicating that the strings match
	return BP_TRUE;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Password Reading / Masking from Input %%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to read a password or other sensitive information using a mask char.  To
// disable mask characters getting output at all, you can supply a null byte.  Returned
// value is bpcalloced buffer and must be bpfreed().
char * BP_GetPasswordFromTerminal
(
	FILE * read_from,
	size_t max_read_len,
	char char_to_mask_output_with
)
{

	// run basic null checks
	if(!read_from)
		return NULL;
	if(!max_read_len)
		return NULL;


	// termios preserving structures
	struct termios preserved_term_settings;
	struct termios modified_term_settings;

	// zero out termios
	BP_ZERO_STACK(preserved_term_settings);
	BP_ZERO_STACK(modified_term_settings);

	// preserve the old terminal information
	if (tcgetattr (0, &preserved_term_settings))
	{
		printf("\n [!!] tcgetattr failed in BP_GetPasswordFromTerminal.  Exiting prematurely.");
		return NULL;
	}

	// duplicate
	memcpy (&modified_term_settings, &preserved_term_settings, sizeof(struct termios));

	// disable keyboard echo
	modified_term_settings.c_lflag &= ~(ICANON | ECHO);

	// set basic flags
	modified_term_settings.c_cc[VTIME] = 0;
	modified_term_settings.c_cc[VMIN] = 1;

	// set the terminal information using the revised terminal settings
	if (tcsetattr (0, TCSANOW, &modified_term_settings))
	{
		printf("\n [!!] tcsetattr failed in BP_GetPasswordFromTerminal.  Exiting prematurely.");
		return NULL;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Start Terminal Read Loop %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create var for reading characters one at a time
	char read_char = 0x00;

	// current read length
	size_t curr_read_len = 0;

	// buffer to return to caller
	char *ret_buff = NULL;

	// iterate through characters till we hit our cap
	while(1)
	{

		// attempt to read a character from stdin
		read_char = fgetc(read_from);

		// ensure we haven't hit the end of file char
		if(read_char == EOF)
			break;

		// increment the read length
		curr_read_len++;

		// break if the character is a newline or carriage return
		if(read_char == '\n')
			break;
		if(read_char == '\r')
			break;

		// reallocate the buffer
		ret_buff = (char *) bprealloc(ret_buff, curr_read_len+1);

		// set the read character
		ret_buff[curr_read_len-1] = read_char;

		// ensure string is reasonable/printable
		if(!BP_StringIsReasonablePrintableString(ret_buff, bpstrlen(ret_buff), BP_FALSE, BP_FALSE))
		{

			// set the terminal information using the revised terminal settings
			tcsetattr (0, TCSANOW, &preserved_term_settings);
			bpfree(ret_buff);
			// printf("\n [!!] BP_GetPasswordFromTerminal somehow read input from fgetc which translated into an unreasonable string.  Likely tampering detected. Please ensure your input string for this routine is reasonable.");
			return NULL;
		}

		// write mask character to stdout if it's non-null
		if(char_to_mask_output_with)
			fputc (char_to_mask_output_with, stdout);

		// break if we hit our max read length
		if(curr_read_len >= max_read_len)
		{
			break;
		}

	}

	// always reset terminal settings
	tcsetattr (0, TCSANOW, &preserved_term_settings);

	// simply return null if the user only enters a newline
	if(!ret_buff)
	{
		return NULL;
	}

	// ensure string is reasonable/printable at final check point
	if(!BP_StringIsReasonablePrintableString(ret_buff, bpstrlen(ret_buff), BP_FALSE, BP_FALSE))
	{
		bpfree(ret_buff);
		// printf("\n [!!] BP_GetPasswordFromTerminal somehow read input from fgetc which translated into an unreasonable string.  Likely tampering detected. Please ensure your input string for this routine is reasonable.");
		return NULL;
	}

	// return the string
	return ret_buff;

}

// clear a linux terminal using xterm sequence.
BP_ERROR_T BP_ClearLinuxTerminalOutput()
{

	// use x-term clear screen sequence to clear screen
	printf("\033[2J");

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Array Copy %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// copy a string array.  Result must be destroyed using BP_DestroyPointerArrayNullTerminated(**)
char ** BP_StringArrayCreateCopyOnHeap(char ** null_terminated_array_of_reasonable_strings)
{

	// run basic checks
	if(!null_terminated_array_of_reasonable_strings)
		return NULL;
	if(!null_terminated_array_of_reasonable_strings[0])
		return NULL;


	// ensure all strings are reasonable before continuing
	size_t n = 0;
	for(; null_terminated_array_of_reasonable_strings[n]; n++)
	{

		// ensure string is reasonable
		if(!BP_StringIsReasonablePrintableString(null_terminated_array_of_reasonable_strings[n], bpstrlen(null_terminated_array_of_reasonable_strings[n]), BP_TRUE, BP_TRUE))
			return NULL;

	}

	// create linkl
	P_BP_LINKL linkl = BP_CreateLinkL(".", BP_FALSE);

	// perform assignment
	for(n = 0; null_terminated_array_of_reasonable_strings[n]; n++)
	{

		// link the string in the tq
		tq_strdup(linkl, null_terminated_array_of_reasonable_strings[n], BPLN);

	}

	// convert to pointer array
	char ** new_array = (char **) BP_LinkLToBPCallocPointerArray(linkl);

	// destroy the linkl
	BP_DestroyLinkL(linkl, BP_TRUE);

	// return the new array
	return new_array;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Password Policy Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Returns BP_TRUE if the provided password passes the provided policy checks.  Password must be
// a reasonable-printable or check will not run.  Policy check information is returned in the
// provided json_in_out.
BP_BOOL BP_PasswordPassesPolicyCheck(P_BP_JSON policy_in_out, char *password)
{

	// ensure you have a password and a policy
	if(!password)
		return BP_FALSE;
	if(!BP_StringIsReasonablePrintableString(password, bpstrlen(password), BP_FALSE, BP_FALSE))
		return BP_FALSE;
	if(!policy_in_out)
		return BP_FALSE;

	// set password length
	size_t password_len = bpstrlen(password);

	// temporary string used when adding error messages (never allocated, only static strings)
	char *tmp_err = NULL;

	// marker indicating whether or not any given violation has occured
	BP_BOOL policy_violation_has_occurred = BP_FALSE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Violations Table %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// remove policy violations table if it already exists
	BP_JSONRemoveTableByPathQuery(policy_in_out, "/password_policy/policy_violations");

	// add policy_violations table
	if(!BP_JSONAddTableByPathQuery(policy_in_out, "/password_policy/policy_violations", BP_FALSE))
		return BP_FALSE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Pull Information From JSON %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// pull keys from JSON
	P_BP_HTKE min_length_htke                = BP_JSONLookupEntryByPathQuery(policy_in_out, "/password_policy/min_length");
	P_BP_HTKE max_length_htke                = BP_JSONLookupEntryByPathQuery(policy_in_out, "/password_policy/max_length");
	P_BP_HTKE special_chars_set_htke         = BP_JSONLookupEntryByPathQuery(policy_in_out, "/password_policy/special_chars_set");
	P_BP_HTKE disallowed_chars_set_htke      = BP_JSONLookupEntryByPathQuery(policy_in_out, "/password_policy/disallowed_chars_set");
	P_BP_HTKE min_special_chars_htke         = BP_JSONLookupEntryByPathQuery(policy_in_out, "/password_policy/min_special_chars");
	P_BP_HTKE max_special_chars_htke         = BP_JSONLookupEntryByPathQuery(policy_in_out, "/password_policy/max_special_chars");
	P_BP_HTKE min_char_diversity_htke        = BP_JSONLookupEntryByPathQuery(policy_in_out, "/password_policy/min_char_diversity");
	P_BP_HTKE check_known_bad_passwords_htke = BP_JSONLookupEntryByPathQuery(policy_in_out, "/password_policy/check_known_bad_passwords");
	P_BP_HTKE known_bad_password_lists_htke  = BP_JSONLookupEntryByPathQuery(policy_in_out, "/password_policy/known_bad_password_lists");





	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Length Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



	// get min and max
	long min_length = BP_JSONAccessEntryAsUnsignedLong(min_length_htke, NULL);
	long max_length = BP_JSONAccessEntryAsUnsignedLong(max_length_htke, NULL);

	// run min length checks
	if(password_len < min_length)
	{

		// set err message
		tmp_err = "Password did not meet minimum length requirements.";

		// add min length notice
		BP_JSONAddNodeByPathQuery
		(
			policy_in_out,
			"/password_policy/policy_violations/min_length_not_met",
			tmp_err,
			bpstrlen(tmp_err),
			BP_FALSE
		);

		// set policy violation marker
		policy_violation_has_occurred = BP_TRUE;

	}

	// run max length checks
	if(password_len > max_length)
	{

		// set err message
		tmp_err = "Password exceeded maximum length allowed.";

		// add min length notice
		BP_JSONAddNodeByPathQuery
		(
			policy_in_out,
			"/password_policy/policy_violations/max_length_exceeded",
			tmp_err,
			bpstrlen(tmp_err),
			BP_FALSE
		);

		// set policy violation marker
		policy_violation_has_occurred = BP_TRUE;

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Buffer Analysis %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create ascii configuration
	BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION ascii_config;

	// zero out the structure (automagic macro)
	BP_ZERO_STACK(ascii_config);

	// set all counters to true
	ascii_config.count_ascii                      = BP_TRUE;
	ascii_config.count_extended                   = BP_TRUE;
	ascii_config.count_individual_characters      = BP_TRUE;


	// attempt to generate buffer analysis
	P_BP_BUFFER_ANALYSIS buff_analysis = BP_CreateBufferAnalysis
	(
			(unsigned char *) password,
			(size_t) bpstrlen(password),
			&ascii_config
	);

	// if there is no buff analysis, exit now
	if(!buff_analysis)
	{

		// set err message
		tmp_err = "Could not create buffer analysis.  Likely out of memory.";

		// add min length notice
		BP_JSONAddNodeByPathQuery
		(
			policy_in_out,
			"/password_policy/policy_violations/buffer_analysis_failed",
			tmp_err,
			bpstrlen(tmp_err),
			BP_FALSE
		);

		// force exit if analysis failed
		return BP_FALSE;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Disallowed Char Checks %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// retrieve disallowed chars set
	char * disallowed_chars_set = BP_JSONAccessEntryAsReasonableString(disallowed_chars_set_htke, BP_FALSE, BP_FALSE);

	// set the disallowed characer iterator
	size_t disallowed_char_iter = 0;

	// char counter
	size_t total_disallowed_chars = 0;

	// process if we have a char set
	if(disallowed_chars_set)
	{

		// iterate through disallowed chars
		for(; disallowed_chars_set[disallowed_char_iter]; disallowed_char_iter++)
		{

			// set curr char
			char curr_char = disallowed_chars_set[disallowed_char_iter];

			// increment char count by count info
			total_disallowed_chars += buff_analysis->ascii->char_counts[curr_char];

		}

		// if we have disallowed characters, mark it
		if(total_disallowed_chars)
		{

			// set err message
			tmp_err = "Password contained disallowed/illegal characters.";

			// add min length notice
			BP_JSONAddNodeByPathQuery
			(
				policy_in_out,
				"/password_policy/policy_violations/illegal_characters_detected",
				tmp_err,
				bpstrlen(tmp_err),
				BP_FALSE
			);

			// set policy violation marker
			policy_violation_has_occurred = BP_TRUE;

		}

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Special Chars Check %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// retrieve special chars
	char *special_chars = BP_JSONAccessEntryAsReasonableString(special_chars_set_htke, BP_FALSE, BP_FALSE);

	// set min and max
	unsigned long min_special_chars = BP_JSONAccessEntryAsUnsignedLong(min_special_chars_htke, NULL);
	unsigned long max_special_chars = BP_JSONAccessEntryAsUnsignedLong(max_special_chars_htke, NULL);


	// special char iter
	size_t special_char_iter = 0;

	// this holds the actual special character count
	size_t total_special_char_count = 0;


	// special characters
	if(!special_chars)
	{

		// set err message
		tmp_err = "No special chars found, skipping check";

		// add min length notice
		BP_JSONAddNodeByPathQuery
		(
			policy_in_out,
			"/password_policy/policy_violations/special_char_check_skipped",
			tmp_err,
			bpstrlen(tmp_err),
			BP_FALSE
		);

	}
	else
	{

		// iterate through special chars
		for(; special_chars[special_char_iter]; special_char_iter++)
		{

			// set curr char
			char curr_char = special_chars[special_char_iter];

			// increment char count by count info
			total_special_char_count += buff_analysis->ascii->char_counts[curr_char];

		}

		// run minimum special char check
		if(total_special_char_count < min_special_chars)
		{

			// set err message
			char err_buff[512];
			BP_ZERO_STACK(err_buff);

			// add message
			sprintf((char *) &err_buff, "Special character minimum limit not met (%zu chars out of %zu set)", total_special_char_count, min_special_chars);

			// add min length notice
			BP_JSONAddNodeByPathQuery
			(
				policy_in_out,
				"/password_policy/policy_violations/special_char_count_not_met",
				(char *) &err_buff,
				bpstrlen((char *) &err_buff),
				BP_FALSE
			);

			// set policy violation marker
			policy_violation_has_occurred = BP_TRUE;

		}

		// run maximum special char check
		if(total_special_char_count > max_special_chars)
		{

			// set err message
			char err_buff[512];
			BP_ZERO_STACK(err_buff);

			// add message
			sprintf((char *) &err_buff, "Special character maximum count exceeded (%zu chars set > %zu max chars)", total_special_char_count, min_special_chars);

			// add min length notice
			BP_JSONAddNodeByPathQuery
			(
				policy_in_out,
				"/password_policy/policy_violations/special_char_count_max_exceeded",
				(char *) &err_buff,
				bpstrlen((char *) &err_buff),
				BP_FALSE
			);

			// set policy violation marker
			policy_violation_has_occurred = BP_TRUE;

		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Character Diversity Checks %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// retrieve minimum character diversity
	unsigned long min_char_diversity = BP_JSONAccessEntryAsUnsignedLong(min_char_diversity_htke, NULL);

	// character diversity
	size_t char_diversity_actual = 0;

	// set temp character iterator
	unsigned char tmp_char = (unsigned char) 0x00;

	// iterate through special chars
	for(; tmp_char <= 0xff; tmp_char++)
	{

		// work through character diversity
		if(buff_analysis->ascii->char_counts[tmp_char])
			char_diversity_actual++;

		// break loop if char is too high
		if(tmp_char == 0xff)
			break;

	}


	// check diversity
	if(char_diversity_actual < min_char_diversity)
	{

		// set err message
		tmp_err = "Character diversity minimum not met.";

		// add min length notice
		BP_JSONAddNodeByPathQuery
		(
			policy_in_out,
			"/password_policy/policy_violations/special_char_check_skipped",
			tmp_err,
			bpstrlen(tmp_err),
			BP_FALSE
		);

		// set policy violation marker
		policy_violation_has_occurred = BP_TRUE;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Dictionary Checks %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	if(known_bad_password_lists_htke)
	{

		// iterate through password lists
		BP_HASH_ITER_OPEN((P_BP_HTKE) known_bad_password_lists_htke->data, pass_file)
		{

			// ensure we have data
			if(pass_file->data)
			{

				// create filename reference
				char *tmp_filename = (char *) pass_file->data;

				// ensure file is in place and reasonable
				if(!BP_StringIsReasonableForUnixEXT4Filename(tmp_filename, bpstrlen(tmp_filename), BP_TRUE))
				{

					// set err message
					char err_buff[512+bpstrlen(tmp_filename)];
					BP_ZERO_STACK(err_buff);

					// add message
					sprintf((char *) &err_buff, "The password check file '%s' was not found", tmp_filename);


					// add min length notice
					BP_JSONAddNodeByPathQuery
					(
						policy_in_out,
						"/password_policy/policy_violations/password_list_file_not_found",
						(char *) &err_buff,
						bpstrlen((char *) &err_buff),
						BP_FALSE
					);

					// set policy violation marker
					policy_violation_has_occurred = BP_TRUE;

					// break the loop
					break;

				}


				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				// %%% Run Linear File Check Here %%%%%%%%%%%%%
				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

				if(BP_PasswordExistsInFile(tmp_filename, password, NULL))
				{

					// set err message
					char err_buff[512+bpstrlen(tmp_filename)];
					BP_ZERO_STACK(err_buff);

					// add message
					sprintf((char *) &err_buff, "The password was found within the file '%s'", tmp_filename);

					// add min length notice
					BP_JSONAddNodeByPathQuery
					(
						policy_in_out,
						"/password_policy/policy_violations/password_found_in_existing_list",
						(char *) &err_buff,
						bpstrlen((char *) &err_buff),
						BP_FALSE
					);

					// set policy violation marker
					policy_violation_has_occurred = BP_TRUE;

					// break the loop
					break;
				}

			}

		}
		BP_HASH_ITER_CLOSE;

	}

	// destroy buffer analysis on exit
	if(buff_analysis)
		BP_DestroyBufferAnalysis(buff_analysis);

	// set policy violation marker
	if(policy_violation_has_occurred)
		return BP_FALSE;

	// retrun indicating that the password passed checks
	return BP_TRUE;

}


// Attempts to open the pass file and read it line by line, checking password.
BP_BOOL BP_PasswordExistsInFile(char *pass_file, char *password, size_t * line_number_in_out)
{

	// pass file and password
	if(!pass_file)
		return BP_FALSE;
	if(!password)
		return BP_FALSE;

	// ensure password is reasonable printable
	if(!BP_StringIsReasonablePrintableString(password, bpstrlen(password), BP_FALSE, BP_FALSE))
		return BP_FALSE;

	// ensure pass file exists
	if(!BP_StringIsReasonableForUnixEXT4Filename(pass_file, bpstrlen(pass_file), BP_TRUE))
		return BP_FALSE;

	// open file
	FILE * tmp_file = fopen( pass_file, "r");
	if(!tmp_file)
		return BP_FALSE;

	// create a 1k buffer for reading line by line
	char tmp_line_buff[BP_KILOBYTE];
	BP_ZERO_STACK(tmp_line_buff);

	// holds line number on iteration
	size_t line_number = 0;

	// iterate through lines in file (max 1k line length)
	while
	(
		fgets((char *) &tmp_line_buff, sizeof(tmp_line_buff)-10, tmp_file)
	)
	{

		// truncate
		if(tmp_line_buff[bpstrlen(tmp_line_buff)-1] == '\n')
			tmp_line_buff[bpstrlen(tmp_line_buff)-1] = 0x00;

		// Attempt a match.
		if(BP_StringsMatchAsReasonableStrings(tmp_line_buff, password))
		{

			// cloase the tmp file
			fclose(tmp_file);

			// mark the line number if specified
			if(line_number_in_out)
				*line_number_in_out = line_number;

			// exit indicating a match
			return BP_TRUE;

		}


		// increment the line number/index
		line_number++;

		// calculate length of string
		size_t tmp_bpstrlen = bpstrlen(tmp_line_buff);
		if(!tmp_bpstrlen)
			continue;

		// zero only the length of the string (saves cpu cycles over zeroing the entire buff)
		memset(tmp_line_buff, 0x00, tmp_bpstrlen);


	}



	// close the file
	fclose(tmp_file);

	// exit indicating false
	return BP_FALSE;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Check Password Against Cleartext %%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Returns true if the salted password matches the plaintext provided
BP_BOOL BP_PasswordMatchesSaltedPlaintext(char *plaintext_password, char *hash_algo, char *hash, char *salt)
{

	// ensure all parts are non-null
	if(!plaintext_password)
		return BP_FALSE;
	if(!hash_algo)
		return BP_FALSE;
	if(!hash)
		return BP_FALSE;
	if(!salt)
		return BP_FALSE;

	// run reasonable string checks on all parts
	if(!BP_StringIsReasonablePrintableString(plaintext_password, bpstrlen(plaintext_password), BP_FALSE, BP_FALSE))
		return BP_FALSE;
	if(!BP_StringIsReasonablePrintableString(hash_algo, bpstrlen(hash_algo), BP_FALSE, BP_FALSE))
		return BP_FALSE;
	if(!BP_StringIsReasonablePrintableString(hash,      bpstrlen(hash),      BP_FALSE, BP_FALSE))
		return BP_FALSE;
	if(!BP_StringIsReasonablePrintableString(salt,      bpstrlen(salt),      BP_FALSE, BP_FALSE))
		return BP_FALSE;


	// set combined plaintext and salt
	char * combined_plaintext_and_salt = BP_JoinStrings(salt, plaintext_password, NULL);
	if(!combined_plaintext_and_salt)
		return BP_FALSE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Hash %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// hashed value
	char *hashed_value = NULL;

	// create hashes based on the hash algorithm
	if(BP_StringsMatchAsReasonableStrings(hash_algo, "sha1"))
	{
		hashed_value = BP_GenSHA1(combined_plaintext_and_salt, bpstrlen(combined_plaintext_and_salt));
	}
	else if(BP_StringsMatchAsReasonableStrings(hash_algo, "sha256"))
	{
		hashed_value = BP_GenSHA256(combined_plaintext_and_salt, bpstrlen(combined_plaintext_and_salt));
	}
	else
	{

		// zero out the plaintext/salt
		memset(combined_plaintext_and_salt, 0x00, bpstrlen(combined_plaintext_and_salt));

		// destroy the combined plaintext and salt
		bpfree(combined_plaintext_and_salt);
		bpfree(hashed_value);

		// return indicating that a match was made
		return BP_FALSE;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check Hash %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	printf("\n HASHED VALUE: %s", hashed_value);
	printf("\n HASH: %s", hash);
	printf("\n PASSCOMBINED: %s", combined_plaintext_and_salt);

	if(BP_StringsMatchAsReasonableStrings(hashed_value, hash))
	{

		// zero out the plaintext/salt
		memset(combined_plaintext_and_salt, 0x00, bpstrlen(combined_plaintext_and_salt));

		// destroy the combined plaintext and salt
		bpfree(combined_plaintext_and_salt);
		bpfree(hashed_value);

		// return indicating that a match was made
		return BP_TRUE;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Cleanup %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// zero out the plaintext/salt
	memset(combined_plaintext_and_salt, 0x00, bpstrlen(combined_plaintext_and_salt));

	// destroy the combined plaintext and salt
	bpfree(combined_plaintext_and_salt);
	bpfree(hashed_value);

	// return indicating no match
	return BP_FALSE;

}

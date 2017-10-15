/*
 * BP-DBStringFilters.cc
 *
 *  Created on: Oct 12, 2016
 *      Author: root
 */


// include blockparty
#include "../../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% String Utilities for BPDB %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// checks if a host name is reasonable for a mariadb host connect string (must be host or ip address)
BP_BOOL BP_StringIsReasonableForMariaDBHostNameDB(char *buffer, size_t len)
{


	if(!buffer || !len)
		return BP_FALSE;


	// attempt to verify that the string is reasonable first
	if(!BP_StringIsReasonablePrintableString(buffer, len, BP_FALSE, BP_FALSE))
		return BP_FALSE;

	// check buffer
	if(strlen(buffer) != len)
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
			(unsigned char *) buffer,
			(size_t) len,
			&ascii_config
	);

	// ensure we h ave a buffer analysis
	if(!buff_analysis)
		return BP_FALSE;


	// if the filter count matches total count, filter passes check (simple+fast check)
	size_t buff_filter_ctr = 0;

	// -- digits --
	buff_filter_ctr += buff_analysis->ascii->char_counts['0'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['1'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['2'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['3'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['4'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['5'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['6'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['7'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['8'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['9'];

	// -- lower case --
	buff_filter_ctr += buff_analysis->ascii->char_counts['a'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['b'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['c'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['d'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['e'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['f'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['g'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['h'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['i'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['j'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['k'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['l'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['m'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['n'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['o'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['p'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['q'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['r'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['s'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['t'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['u'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['v'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['w'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['x'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['y'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['z'];

	// -- upper case -------
	buff_filter_ctr += buff_analysis->ascii->char_counts['A'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['B'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['C'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['D'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['E'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['F'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['G'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['H'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['I'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['J'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['K'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['L'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['M'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['N'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['O'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['P'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['Q'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['R'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['S'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['T'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['U'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['V'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['W'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['X'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['Y'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['Z'];

	// -- special chars ----
	buff_filter_ctr += buff_analysis->ascii->char_counts['.'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['-'];

	// if the filter counter of all the allowed characters does not
	// equal our total length, we have encountered a bad string and
	// must exit.
	if(buff_filter_ctr != len)
	{
		BP_DestroyBufferAnalysis(buff_analysis);
		return BP_FALSE;
	}


	// destroy the buffer analysis
	BP_DestroyBufferAnalysis(buff_analysis);


	// --- run pattern checks on allowed characters
	//
	// note: '--' is allowed in domain names but '---' is not

	// there can never be a ..
	if(len >= 2)
	if(strstr(buffer, ".."))
		return BP_FALSE;

	// there can never be a ---
	if(len >= 3)
	if(strstr(buffer, "---"))
		return BP_FALSE;

	// there can never be a '-.' or a '.-'
	if(len >= 2)
	{
		if(strstr(buffer, "-."))
			return BP_FALSE;
		if(strstr(buffer, ".-"))
			return BP_FALSE;
	}

	// if the buffer ends with a period, it's not a good buffer
	if(buffer[len-1] == '.')
		return BP_FALSE;

	// return indicating that the path is suitable
	return BP_TRUE;

}


// checks if a host name is reasonable for a mariadb host connect string (must be host or ip address)
BP_BOOL BP_StringIsReasonableForDatabaseMariaDB(char *buffer, size_t len)
{


	if(!buffer || !len)
		return BP_FALSE;


	// attempt to verify that the string is reasonable first
	if(!BP_StringIsReasonablePrintableString(buffer, len, BP_FALSE, BP_FALSE))
		return BP_FALSE;

	// check buffer
	if(strlen(buffer) != len)
		return BP_FALSE;

	// max database name length is 64 (from mysql.com section on identifiers)
	if(len > 64)
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
			(unsigned char *) buffer,
			(size_t) len,
			&ascii_config
	);

	// ensure we h ave a buffer analysis
	if(!buff_analysis)
		return BP_FALSE;


	// if the filter count matches total count, filter passes check (simple+fast check)
	size_t buff_filter_ctr = 0;

	// -- digits --
	buff_filter_ctr += buff_analysis->ascii->char_counts['0'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['1'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['2'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['3'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['4'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['5'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['6'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['7'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['8'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['9'];

	// -- lower case --
	buff_filter_ctr += buff_analysis->ascii->char_counts['a'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['b'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['c'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['d'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['e'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['f'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['g'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['h'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['i'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['j'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['k'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['l'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['m'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['n'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['o'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['p'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['q'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['r'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['s'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['t'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['u'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['v'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['w'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['x'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['y'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['z'];

	// -- upper case -------
	buff_filter_ctr += buff_analysis->ascii->char_counts['A'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['B'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['C'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['D'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['E'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['F'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['G'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['H'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['I'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['J'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['K'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['L'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['M'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['N'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['O'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['P'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['Q'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['R'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['S'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['T'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['U'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['V'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['W'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['X'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['Y'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['Z'];

	// -- special chars ---- (underscore and dollarsign are allowed chars)
	buff_filter_ctr += buff_analysis->ascii->char_counts['_'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['$'];

	// if the filter counter of all the allowed characters does not
	// equal our total length, we have encountered a bad string and
	// must exit.
	if(buff_filter_ctr != len)
	{
		BP_DestroyBufferAnalysis(buff_analysis);
		return BP_FALSE;
	}


	// destroy the buffer analysis
	BP_DestroyBufferAnalysis(buff_analysis);

	// return indicating that the path is suitable
	return BP_TRUE;

}


// checks if a host name is reasonable for a mariadb host connect string (must be host or ip address)
BP_BOOL BP_StringIsReasonableForUsernameMariaDB(char *buffer, size_t len)
{


	if(!buffer || !len)
		return BP_FALSE;


	// attempt to verify that the string is reasonable first
	if(!BP_StringIsReasonablePrintableString(buffer, len, BP_FALSE, BP_FALSE))
		return BP_FALSE;

	// check buffer
	if(strlen(buffer) != len)
		return BP_FALSE;

	// max database name length is 64 (from mysql.com section on identifiers)
	if(len > 64)
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
			(unsigned char *) buffer,
			(size_t) len,
			&ascii_config
	);

	// ensure we h ave a buffer analysis
	if(!buff_analysis)
		return BP_FALSE;


	// if the filter count matches total count, filter passes check (simple+fast check)
	size_t buff_filter_ctr = 0;

	// -- digits --
	buff_filter_ctr += buff_analysis->ascii->char_counts['0'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['1'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['2'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['3'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['4'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['5'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['6'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['7'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['8'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['9'];

	// -- lower case --
	buff_filter_ctr += buff_analysis->ascii->char_counts['a'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['b'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['c'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['d'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['e'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['f'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['g'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['h'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['i'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['j'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['k'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['l'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['m'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['n'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['o'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['p'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['q'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['r'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['s'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['t'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['u'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['v'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['w'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['x'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['y'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['z'];

	// -- upper case -------
	buff_filter_ctr += buff_analysis->ascii->char_counts['A'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['B'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['C'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['D'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['E'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['F'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['G'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['H'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['I'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['J'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['K'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['L'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['M'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['N'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['O'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['P'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['Q'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['R'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['S'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['T'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['U'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['V'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['W'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['X'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['Y'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['Z'];

	// -- special chars ---- (underscore and dollarsign are allowed chars)
	buff_filter_ctr += buff_analysis->ascii->char_counts['_'];
	buff_filter_ctr += buff_analysis->ascii->char_counts['$'];

	// if the filter counter of all the allowed characters does not
	// equal our total length, we have encountered a bad string and
	// must exit.
	if(buff_filter_ctr != len)
	{
		BP_DestroyBufferAnalysis(buff_analysis);
		return BP_FALSE;
	}


	// destroy the buffer analysis
	BP_DestroyBufferAnalysis(buff_analysis);

	// return indicating that the path is suitable
	return BP_TRUE;

}


// checks if a host name is reasonable for a mariadb host connect string (must be host or ip address)
BP_BOOL BP_StringIsReasonableForPasswordMariaDB(char *buffer, size_t len)
{

	// ensure we have a buffer and length
	if(!buffer || !len)
		return BP_FALSE;

	// password can be any reasonable-printable without newlines
	if(!BP_StringIsReasonablePrintableString(buffer, len, BP_FALSE, BP_FALSE))
		return BP_FALSE;

	// return indicating that the path is suitable
	return BP_TRUE;

}

// -- SQLite Connector Value Checks

// checks to see if a sqlite file is reasonable
BP_BOOL BP_StringIsReasonableForDatabaseFileSQLiteDB(char *buffer, size_t len, BP_BOOL attempt_to_stat_file)
{

	if(BP_StringIsReasonableForUnixEXT4Filename(buffer, len, attempt_to_stat_file))
	{
		return BP_TRUE;
	}

	// return indicating false if the file isn't reasonable
	return BP_FALSE;
}

// checks to see if log file is reasonable
BP_BOOL BP_StringIsReasonableForDatabaseLogFileSQLiteDB(char *buffer, size_t len, BP_BOOL attempt_to_stat_file)
{

	if(BP_StringIsReasonableForUnixEXT4Filename(buffer, len, attempt_to_stat_file))
	{
		return BP_TRUE;
	}

	// return indicating false if the file isn't reasonable
	return BP_FALSE;

}


// -- MariaDB/SQLite Shared Table and Column Validators ------------------


// checks if a string is syntactically reasonable for what's expected of a table name.
BP_BOOL BP_StringIsReasonableForTableNameDB(char * buffer, size_t len)
{

	// ensure we have a directory and a length
	if(!buffer || !len)
		return BP_FALSE;


	// attempt to verify that the string is reasonable first
	if(!BP_StringIsReasonablePrintableString(buffer, len, BP_FALSE, BP_FALSE))
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
			(unsigned char *) buffer,
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
			buff_analysis->ascii->char_counts[' ']  ||
			buff_analysis->ascii->char_counts['\\'] ||
			buff_analysis->ascii->char_counts['`' ] ||
			buff_analysis->ascii->char_counts['|' ] ||
			buff_analysis->ascii->char_counts['}' ] ||
			buff_analysis->ascii->char_counts['{' ] ||
			buff_analysis->ascii->char_counts[']' ] ||
			buff_analysis->ascii->char_counts['[' ] ||
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


	// return indicating that the path is suitable
	return BP_TRUE;


}

// checks if a string is syntactically reasonable for what's expected of a column name.
BP_BOOL BP_StringIsReasonableForColumnNameDB(char * buffer, size_t len)
{

	// ensure we have a directory and a length
	if(!buffer || !len)
		return BP_FALSE;


	// attempt to verify that the string is reasonable first
	if(!BP_StringIsReasonablePrintableString(buffer, len, BP_FALSE, BP_FALSE))
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
			(unsigned char *) buffer,
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
			buff_analysis->ascii->char_counts[' ']  ||
			buff_analysis->ascii->char_counts['\\'] ||
			buff_analysis->ascii->char_counts['`' ] ||
			buff_analysis->ascii->char_counts['|' ] ||
			buff_analysis->ascii->char_counts['}' ] ||
			buff_analysis->ascii->char_counts['{' ] ||
			buff_analysis->ascii->char_counts[']' ] ||
			buff_analysis->ascii->char_counts['[' ] ||
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


	// return indicating that the path is suitable
	return BP_TRUE;
}

// checks if a string is syntactically reasonable for what's expected of a database name.
BP_BOOL BP_StringIsReasonableForDatabaseNameDB(char * buffer, size_t len)
{

	// ensure we have a directory and a length
	if(!buffer || !len)
		return BP_FALSE;


	// attempt to verify that the string is reasonable first
	if(!BP_StringIsReasonablePrintableString(buffer, len, BP_FALSE, BP_FALSE))
		return BP_FALSE;

	// create ascii configuration
	BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION ascii_config;

	// zero out the structure (automagic macro)
	BP_ZERO_STACK(ascii_config);

	// set some flags in the structure to count ascii (we already counted invalid with our initial check)
	ascii_config.count_ascii = BP_TRUE;

	// attempt to generate analysis
	P_BP_BUFFER_ANALYSIS buff_analysis = BP_CreateBufferAnalysis
	(
			(unsigned char *) buffer,
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
			buff_analysis->ascii->char_counts[' ']  ||
			buff_analysis->ascii->char_counts['\\'] ||
			buff_analysis->ascii->char_counts['`' ] ||
			buff_analysis->ascii->char_counts['|' ] ||
			buff_analysis->ascii->char_counts['}' ] ||
			buff_analysis->ascii->char_counts['{' ] ||
			buff_analysis->ascii->char_counts[']' ] ||
			buff_analysis->ascii->char_counts['[' ] ||
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


	// return indicating that the path is suitable
	return BP_TRUE;

}

// checks to see if a string is reasonable for a field specifier (should be: tablename.fieldname)
BP_BOOL BP_StringIsReasonableForSelectFieldSpecifierDB(char * buffer, size_t len)
{

	// ensure we have a directory and a length
	if(!buffer || !len)
		return BP_FALSE;

	// if it's a star selector we can return immediately without
	// running verifications.
	if(len == 1)
	if(buffer[0] == '*')
		return BP_TRUE;



	// attempt to verify that the string is reasonable first
	if(!BP_StringIsReasonablePrintableString(buffer, len, BP_FALSE, BP_FALSE))
		return BP_FALSE;

	// create ascii configuration
	BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION ascii_config;

	// zero out the structure (automagic macro)
	BP_ZERO_STACK(ascii_config);

	// set some flags in the structure to count ascii (we already counted invalid with our initial check)
	ascii_config.count_ascii = BP_TRUE;

	// attempt to generate analysis
	P_BP_BUFFER_ANALYSIS buff_analysis = BP_CreateBufferAnalysis
	(
			(unsigned char *) buffer,
			(size_t) len,
			&ascii_config
	);

	// ensure we h ave a buffer analysis
	if(!buff_analysis)
		return BP_FALSE;


	// ensure we only have one '.' count
	if(buff_analysis->ascii->char_counts['.'] != 1)
	{

		// destroy the buffer analysis before exiting
		BP_DestroyBufferAnalysis(buff_analysis);

		// return indicating mismatch
		return BP_FALSE;

	}

	// string cannot start with a peiriod
	if(buffer[0] == '.')
	{

		// destroy the buffer analysis before exiting
		BP_DestroyBufferAnalysis(buff_analysis);

		// return indicating mismatch
		return BP_FALSE;

	}

	// string cannot end with a peiriod
	if(buffer[len-1] == '.')
	{

		// destroy the buffer analysis before exiting
		BP_DestroyBufferAnalysis(buff_analysis);

		// return indicating mismatch
		return BP_FALSE;

	}

	// ensure buffer doesn't have any special chars which could
	// play havoc with later parsing.
	if
	(
			buff_analysis->ascii->char_counts[' ']  ||
			buff_analysis->ascii->char_counts['\\'] ||
			buff_analysis->ascii->char_counts['`' ] ||
			buff_analysis->ascii->char_counts['|' ] ||
			buff_analysis->ascii->char_counts['}' ] ||
			buff_analysis->ascii->char_counts['{' ] ||
			buff_analysis->ascii->char_counts[']' ] ||
			buff_analysis->ascii->char_counts['[' ] ||
			buff_analysis->ascii->char_counts[')' ] ||
			buff_analysis->ascii->char_counts['(' ] ||
			buff_analysis->ascii->char_counts['>' ] ||
			buff_analysis->ascii->char_counts['<' ] ||
			buff_analysis->ascii->char_counts[';' ] ||
			buff_analysis->ascii->char_counts[':' ] ||
			buff_analysis->ascii->char_counts[',' ] ||
			buff_analysis->ascii->char_counts['*' ] || /* note, star selectors ARE forbidden at the time of this check */
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


	// return indicating that the path is suitable
	return BP_TRUE;

}


// checks to see if a string is reasonable for a column type field
BP_BOOL BP_StringIsReasonableForColumnTypeFieldDB(char *buffer, size_t len)
{


	// ensure we have a directory and a length
	if(!buffer || !len)
		return BP_FALSE;

	// if it's a star selector we can return immediately without
	// running verifications.
	if(len == 1)
	if(buffer[0] == '*')
		return BP_TRUE;



	// attempt to verify that the string is reasonable first
	if(!BP_StringIsReasonablePrintableString(buffer, len, BP_FALSE, BP_FALSE))
		return BP_FALSE;

	// create ascii configuration
	BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION ascii_config;

	// zero out the structure (automagic macro)
	BP_ZERO_STACK(ascii_config);

	// set some flags in the structure to count ascii (we already counted invalid with our initial check)
	ascii_config.count_ascii = BP_TRUE;

	// attempt to generate analysis
	P_BP_BUFFER_ANALYSIS buff_analysis = BP_CreateBufferAnalysis
	(
			(unsigned char *) buffer,
			(size_t) len,
			&ascii_config
	);

	// ensure we have a buffer analysis
	if(!buff_analysis)
		return BP_FALSE;

	// ensure buffer doesn't have any special chars which could
	// play havoc with later parsing.
	if
	(
			buff_analysis->ascii->char_counts[' ']  ||
			buff_analysis->ascii->char_counts['\\'] ||
			buff_analysis->ascii->char_counts['.']  ||
			buff_analysis->ascii->char_counts['`' ] ||
			buff_analysis->ascii->char_counts['|' ] ||
			buff_analysis->ascii->char_counts['}' ] ||
			buff_analysis->ascii->char_counts['{' ] ||
			buff_analysis->ascii->char_counts[']' ] ||
			buff_analysis->ascii->char_counts['[' ] ||
			buff_analysis->ascii->char_counts['>' ] ||
			buff_analysis->ascii->char_counts['<' ] ||
			buff_analysis->ascii->char_counts[';' ] ||
			buff_analysis->ascii->char_counts[':' ] ||
			buff_analysis->ascii->char_counts[',' ] ||
			buff_analysis->ascii->char_counts['*' ] ||
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


	// return indicating that the path is suitable
	return BP_TRUE;

}



// checks to see if a string is reasonable for a mariadb comparison operator
BP_BOOL BP_StringIsReasonableForMariadbComparisonOperatorDB(char *buffer, size_t len)
{

	// ensure we have a directory and a length
	if(!buffer || !len)
		return BP_FALSE;

	// attempt to verify that the string is reasonable first
	if(!BP_StringIsReasonablePrintableString(buffer, len, BP_FALSE, BP_FALSE))
		return BP_FALSE;

	// statically defined set of strings
	char * internal_bpdb_comparison_operator_string_array[]
	{
			"!=",
			"<",
			"<=",
			"<=>",
			"=",
			">",
			">=",
			"!",
			"&&",
			"XOR",
			"||",
			NULL
	};


	// create list of valid comparison operators
	P_BP_LINKL valid_comparison_operators = BP_CreateLinkL(".", BP_TRUE);

	// declare string iterator
	size_t str_iter = 0;

	// walk str iter and create linkl
	for(; internal_bpdb_comparison_operator_string_array[str_iter]; str_iter++)
	{

		// add operator
		tq_strdup(valid_comparison_operators, internal_bpdb_comparison_operator_string_array[str_iter], BPLN);

	}

	// duplicate tmp_buff before compare
	char *tmp_buff = bpstrndup(buffer, len);

	// if we can't find the buffer, destroy linkl and exit
	if(!BP_LinkLSearchForString(valid_comparison_operators, tmp_buff, len))
	{
		bpfree(tmp_buff);
		BP_DestroyLinkL(valid_comparison_operators, BP_TRUE);
		return BP_FALSE;
	}


	// destroy the linkl before exiting success
	bpfree(tmp_buff);
	BP_DestroyLinkL(valid_comparison_operators, BP_TRUE);

	// return indicating that the operator has been found
	return BP_TRUE;

}

// checks to see if a string is reasonable for a mariadb conjunction
BP_BOOL BP_StringIsReasonableForMariadbConjunctionDB(char *buffer, size_t len)
{


	// ensure we have a directory and a length
	if(!buffer || !len)
		return BP_FALSE;

	// attempt to verify that the string is reasonable first
	if(!BP_StringIsReasonablePrintableString(buffer, len, BP_FALSE, BP_FALSE))
		return BP_FALSE;

	// attempt to convert buffer
	char * buff_uppercase = BP_StringToUpperCase(buffer, len);
	if(!buff_uppercase)
		return BP_FALSE;

	// statically defined set of strings
	char * internal_bpdb_conjunction_string_array[]
	{
			"AND",
			"OR",
			"NOT",
			"<=>",
			"=",
			">",
			">=",
			"!",
			"&&",
			"XOR",
			"||",
			NULL
	};


	// create list of valid comparison operators
	P_BP_LINKL valid_conjunctions = BP_CreateLinkL(".", BP_TRUE);

	// declare string iterator
	size_t str_iter = 0;

	// walk str iter and create linkl
	for(; internal_bpdb_conjunction_string_array[str_iter]; str_iter++)
	{

		// add operator
		tq_strdup(valid_conjunctions, internal_bpdb_conjunction_string_array[str_iter], BPLN);

	}

	// if we can't find the buffer, destroy linkl and exit
	if(!BP_LinkLSearchForString(valid_conjunctions, buff_uppercase, len))
	{
		BP_DestroyLinkL(valid_conjunctions, BP_TRUE);
		bpfree(buff_uppercase);
		return BP_FALSE;
	}

	// destroy the uppercase buffer
	bpfree(buff_uppercase);

	// destroy the linkl before exiting success
	BP_DestroyLinkL(valid_conjunctions, BP_TRUE);

	// return indicating that the operator has been found
	return BP_TRUE;

}

// checks to see if a string is reasonable for a mariadb sort designator
BP_BOOL BP_StringIsReasonableForMariadbSortDesignatorDB(char *buffer, size_t len)
{


	// ensure we have a directory and a length
	if(!buffer || !len)
		return BP_FALSE;

	// attempt to verify that the string is reasonable first
	if(!BP_StringIsReasonablePrintableString(buffer, len, BP_FALSE, BP_FALSE))
		return BP_FALSE;

	// attempt to convert buffer
	char * buff_uppercase = BP_StringToUpperCase(buffer, len);
	if(!buff_uppercase)
		return BP_FALSE;

	// statically defined set of strings
	char * internal_bpdb_sort_designator_string_array[]
	{
			"ASC",
			"DESC",
			NULL
	};


	// create list of valid designators
	P_BP_LINKL valid_sort_designators = BP_CreateLinkL(".", BP_TRUE);

	// declare string iterator
	size_t str_iter = 0;

	// walk str iter and create linkl
	for(; internal_bpdb_sort_designator_string_array[str_iter]; str_iter++)
	{

		// add operator
		tq_strdup(valid_sort_designators, internal_bpdb_sort_designator_string_array[str_iter], BPLN);

	}

	// if we can't find the buffer, destroy linkl and exit
	if(!BP_LinkLSearchForString(valid_sort_designators, buff_uppercase, len))
	{
		BP_DestroyLinkL(valid_sort_designators, BP_TRUE);
		bpfree(buff_uppercase);
		return BP_FALSE;
	}

	// destroy the uppercase buffer
	bpfree(buff_uppercase);

	// destroy the linkl before exiting success
	BP_DestroyLinkL(valid_sort_designators, BP_TRUE);

	// return indicating that the operator has been found
	return BP_TRUE;

}



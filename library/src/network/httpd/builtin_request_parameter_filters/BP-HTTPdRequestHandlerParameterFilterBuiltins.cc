/*
 * BP-HTTPdRequestHandlerParameterFilterBuiltins.cc
 *
 *  Created on: Dec 14, 2015
 *      Author: root
 */


// include main
#include "../../../../include/BP-Main.h"

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Default Parameter Filter Routines %%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

BP_HTTP_REQUEST_HANDLER_DECLARE_PARAMETER_FILTER(BP_HTTPdRequestHandlerFilterBuiltin_alphanum)
{

	// run filter prologue
	BP_HTTPD_REQUEST_HANDLER_FILTER_PROLOGUE;

    // check string conformity
	BP_STRING_CONFORMITY_FLAGS_T c_flags = BP_StringConformsAs
    (
            (char *) data,
            data_size
    );

	// if flag checks failed, exit immediately
	if(!c_flags)
		BP_HTTPD_REQUEST_HANDLER_FILTER_FAILED("string_not_alphanumeric");

	// check if the string is alphanumeric
	if(!BP_FLAG_IS_SET(c_flags, BP_STRING_CONFORMS_AS_ASCII_ALPHANUM))
	{
		BP_HTTPD_REQUEST_HANDLER_FILTER_FAILED("string_not_alphanumeric");
	}

	// exit after all checks passed
	BP_HTTPD_REQUEST_HANDLER_FILTER_PASSED_ALL_CHECKS;

}


// ensures that a request filter parameter is within the valid ranges of a signed long value.
BP_HTTP_REQUEST_HANDLER_DECLARE_PARAMETER_FILTER(BP_HTTPdRequestHandlerFilterBuiltin_signed_long)
{

	// run filter prologue
	BP_HTTPD_REQUEST_HANDLER_FILTER_PROLOGUE;

	// run basic checks


	// this is the value as it is converted
	unsigned long conversion_val = 0;

	// attempt to convert data (automatically checks buffer sanity by using
	// buffer analysis subsystem)
	BP_ERROR_T conversion_status = BP_StringToUnsignedLongBase10
	(
		(char *) data,
		data_size,
		(unsigned long *) &conversion_val
	);

	// filter fails if string is unable to be converted to a signed long base10 value
	if(!conversion_status)
		BP_HTTPD_REQUEST_HANDLER_FILTER_FAILED("conversion_to_base10_long_signed_failed");

	// if a filter configuration has been set
	if(filter->filter_config_has_been_set)
	{

		printf("\n Configuration Set OK!!!");

	}

	// exit after all checks passed
	BP_HTTPD_REQUEST_HANDLER_FILTER_PASSED_ALL_CHECKS;

}

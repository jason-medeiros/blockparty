/*
 * BP-JSSystemBuiltins.cc
 *
 *  Created on: May 17, 2015
 *      Author: root
 */


// blockparty main include
#include "../../../../include/BP-Main.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% System Function Definition Table %%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Functions that are available within the system context.
JSFunctionSpec BP_JS_GLOBAL_SYSTEM_CONTEXT_FUNCTION_TABLE[] =
{
    JS_FS("BP_JSDebugPrintStdout",   BP_JSDebugPrintStdout,   1, 0),
    JS_FS_END
};

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Default System Functions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// === System Context Functions ============================

// simply takes a string argument and prints it to stdout
static bool BP_JSDebugPrintStdout(JSContext *cx, unsigned argc, jsval *vp)
{

	// get arguments
	JS::CallArgs args = CallArgsFromVp(argc, vp);

	// if we have the correct number of arguments
	if(argc != 1)
		return false;

	// set the first argument
	JSString * arg_str = args[0].toString();

	// encode the string
	char * encoded_arg_str = JS_EncodeString(cx, arg_str);


	/*
	// Optional Buffer analysis: Uncomment to enable

	// create ascii configuration
	BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION ascii_config;

	// zero out the structure (automagic macro)
	BP_ZERO_STACK(ascii_config);

	// set some flags in the structure (default is BP_FALSE aka 0)
	ascii_config.count_ascii                      = BP_TRUE;
	ascii_config.count_extended                   = BP_TRUE;
	ascii_config.count_individual_characters      = BP_TRUE;

	// create analysis of buffer
	P_BP_BUFFER_ANALYSIS buff_analysis = BP_CreateBufferAnalysis
	(
			(unsigned char *) encoded_arg_str,
			bpstrlen(encoded_arg_str),
			&ascii_config
	);

	// buffer analysis
	BP_DisplayBufferAnalysisASCII(buff_analysis);
	*/

	// check if the buffer is reasonable
	BP_BOOL str_reasonable = BP_StringIsReasonablePrintableString(encoded_arg_str, bpstrlen(encoded_arg_str)-1, BP_TRUE, BP_TRUE);

	// return failure if the print string is not reasonable
	if(str_reasonable == BP_FALSE)
	{

		// free the encoded argument string
		JS_free(cx, encoded_arg_str);

		// exit immediately because the string is unreasonable
		return false;

	}

	// print out the encoded string to the console
	printf("%s", encoded_arg_str);

	// free the string
	JS_free(cx, encoded_arg_str);


	// return indicating true
	return true;

}

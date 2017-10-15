/*
 * BP_CommandLineOptionParser.h
 *
 * Command line option parser for blockparty.  Handles options
 * in the form of -xx yy or -xx.
 *
 * Document Author: Jason A. Medeiros
 */


#ifndef BP_COMMANDLINEOPTIONPARSER_H_
#define BP_COMMANDLINEOPTIONPARSER_H_

// forward class declaration
class BP_CommandLineOptionParser;

// forward parser structure declaration
typedef struct _BP_COMMAND_LINE_PARSER_TOKENS BP_COMMAND_LINE_PARSER_TOKENS, *P_BP_COMMAND_LINE_PARSER_TOKENS;


// Argument type enumerator for determining how to manage
// arguments as provided to the command line parser.  Set within
// the BP_COMMAND_LINE_PARSER_TOKENS structure.
typedef enum BP_COMMAND_LINE_ARGUMENT_TYPE
{
	/* Simple Data Types */
	BP_COMMAND_LINE_ARGUMENT_UNSET,
	BP_COMMAND_LINE_ARGUMENT_STAND_ALONE_TOGGLE, /* aka no argument required, eg -b -c -d */
	BP_COMMAND_LINE_ARGUMENT_CHAR_STRING,        /* aka string, eg -b "value here" */
	BP_COMMAND_LINE_ARGUMENT_SIZE_T,             /* aka value, eg  -b 1234 */
	BP_COMMAND_LINE_ARGUMENT_SIGNED_INT,         /* same as above but with possible negative value, eg -b "-1234" */
	/* Complex Data Types Start Here */
	BP_COMMAND_LINE_ARGUMENT_URL                 /* aka any url, eg -b "http://www.thing.com/blah/what/script.php?1=1&2=2" */
};



//! Typedef for command line parser tokens. The options included here must match the optarg string passed in.
typedef struct _BP_COMMAND_LINE_PARSER_TOKENS {



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Values Used for Parsing %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// a single character representing the individual token
	char * token;

	// length of the token in bytes
	size_t token_length;

	// flag indicating if the token is long form (aka --token=value)
	bool token_is_long_form;

	// argument required or not (true for use, false for no)
	bool token_required;

	// type of the value which will be parsed
	BP_COMMAND_LINE_ARGUMENT_TYPE value_type;

	// description of token
	char * token_description;

	// example of token usage
	char * token_usage_example;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Buffer Analysis %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this is a buffer analysis created when the token is parsed (see BP-BufferAnalysis.h for
	// information regarding what is analyzed.)
	P_BP_BUFFER_ANALYSIS value_buff_analysis;

	// analysis configuration
	BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION ascii_analysis_configuration;

	// unparsed value
	unsigned char * unparsed_value;

	// size of unparsed value
	size_t          unparsed_value_n;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Informational Flags %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if the value has been found to have embedded backticks (aka `) this
	// flag is enabled.
	bool value_contains_backticks;

	// value contains non-printable characters
	bool value_contains_nonprintables;

	// value contains embedded single quotes
	bool value_contains_embedded_single_quotes;

	// value contains embedded double quotes
	bool value_contains_embedded_double_quotes;

	// the parsed value contains only asci 0-9
	bool value_contains_a_positive_number;

	// the parsed value contains a prefix of '-' with only numbers following
	bool value_contains_a_negative_number;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Default Values as Types %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// default value as string
	char *default_value_as_string;

	// default value as size_t
	size_t default_value_as_size_t;

	// default value as int
	int default_value_as_int;

	// default value as toggle (BP_TRUE/BP_FALSE)
	BP_BOOL default_value_as_toggle;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Max/Minimum Values for Argument %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// maximum length if the argument is a string type
	size_t max_length_for_string;

	// minimum length for string if the argument is string type
	size_t min_length_for_string;

	// maximum value as a size_t (must be positive or zero)
	size_t max_value_as_size_t;

	// minimum value as a size_t (must be positive or zero)
	size_t min_value_as_size_t;

	// maximum value if the argument is an integer type (can be negative)
	int max_value_as_integer;

	// minimum value if the argument is an integer type (can be negative)
	int min_value_as_integer;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Applicable PREG Filters %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Perl compatible regular expressions to match on
	// string types.  If the string does not match after
	// iterating through this filter stack, no processing
	// will take place.
	char **preg_filters;

	// number of filters in the stack
	size_t preg_filter_n;

	// number of filters which were successfully matched (should match up
	// with preg_filter_n if all checks passed ok)
	size_t preg_runtime_match_count;

	// this is a BP_TRUE/BP_FALSE flag indicating whether or not the filter
	// checks were successfully run.
	BP_ERROR_T preg_filters_all_checked_ok;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Callback to use if argument set %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// flag indicating whether or not to use the callback defined below)
	size_t argument_callback_enabled;

	// If the following callback is set, the following happens in this order:
	//
	// - All arguments are parsed.
	//
	// - Checks are made to ensure that the tokens involved have passed all min/max/filter
	//  requirements.
	//
	// - Callbacks are invoked in linear order starting with argumens which appear
	//   first in the parser stack.
	//
	// - When all callbacks complete, parser exits.
	//
	// Callbacks should not be used for starting program functionality, and instead should only
	// be used for manipulating set arguments before application start (token validation, etc)
	//
	void * (*argument_activation_callback)(BP_CommandLineOptionParser * parser, P_BP_COMMAND_LINE_PARSER_TOKENS token);

	// when a callback is invoked, this value is set as a return value.  It is opaque but defined
	// as a void pointer.  To convert values, just typecast the return value to your desired type.
	void * callback_return_value;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Values After Parse is Performed %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// flag indicating if the value has been set ok.  This flag is set before the callback
	// activates, meaning that the callback (if used) can override this setting.
	bool value_has_been_set_ok;

	// result of quote checking stored here.
	BP_STRING_QUOTE_CHECK_RESULTS value_parse_quote_check;

	// Position of this value/option as pertains to the argv.  With long
	// form options this should always be the same.  With short form
	// options, the option should preceed the value.
	size_t argv_position_of_value;
	size_t argv_position_of_option;

	// Total number of times this toggle (set) exists within the command line
	// parameters.  This should always be 1 if set or 0 if unset.  If the value is any
	// other value, the user has provided duplicate values.
	size_t option_encountered_in_argv_count;

	// the value which is parsed
	char *parsed_value_as_string;

	// parsed value as size_t
	size_t parsed_value_as_size_t;

	// parsed value as integer
	int parsed_value_as_int;

	// parsed value as toggle
	BP_BOOL parsed_value_as_toggle;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Messages Associated with Token %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// warning message
	char **token_warning_msgs;

	// count of all the warnings encountered
	size_t token_warning_n;

	// error messages
	char **token_error_msg;

	// count of all the errors encounterd
	size_t token_error_n;


} BP_COMMAND_LINE_PARSER_TOKENS, *P_BP_COMMAND_LINE_PARSER_TOKENS;




//! Parses out command line arguments.
class BP_CommandLineOptionParser : public BP_BaseError
{


public:

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Class Local Arguments %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// tailqueue allocator (handles all memory allocation for this class)
	P_BP_LINKL tq;

	// program argc, set via parsing, exists as a duplicate
	size_t argc;

	// program argv, set via parsing, exists as a duplicate
	char **argv;

	// program argc, set via parsing, counted directly
	size_t envpc;

	// program envp, set via parsing, exists as a duplicate
	char **envp;


	// stack of tokens
	P_BP_COMMAND_LINE_PARSER_TOKENS tokens;

	// the number of parsed arguments / tokens in the above structure stack.
	size_t tokens_n;

	// strings indicating whether or not a parse attempt resulted in warnings or errors
	bool parsing_tokens_resulted_in_err;
	bool parsing_tokens_resulted_in_warn;

	// flag indicating if a parsed token was encountered more than once during parsing
	bool parsing_tokens_resulted_in_duplicate_set;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Constructor / Deconstructor %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// class constructor
	BP_CommandLineOptionParser();

	// class deconstructor
	~BP_CommandLineOptionParser();


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% General Data Type Option Addition Methods %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Adds an option to the parser (generic, add any type.  This is used when constructing
	// the more type-specific option addition methods below.)
	BP_ERROR_T addOption
	(
		char *token,
		bool token_is_long_form,
		size_t token_required,
		BP_COMMAND_LINE_ARGUMENT_TYPE value_type,
		char *token_description,
		char *token_usage_example,
		void * default_value,
		size_t max_length_for_string,
		size_t min_length_for_string,
		size_t max_value_as_size_t,
		size_t min_value_as_size_t,
		int max_value_as_integer,
		int min_value_as_integer,
		void * (*argument_activation_callback)(BP_CommandLineOptionParser * parser, P_BP_COMMAND_LINE_PARSER_TOKENS token) = NULL,
		char ** preg_custom_filters = NULL
	);

	// Add Stand alone toggle option. (aka: -a -b -c -d -e)
	BP_ERROR_T addToggleOption
	(
			char * token,
			size_t token_required,
			char *token_description,
			char *token_usage_example,
			size_t default_value,
			void * (*argument_activation_callback)(BP_CommandLineOptionParser * parser, P_BP_COMMAND_LINE_PARSER_TOKENS token) = NULL,
			char ** preg_custom_filters = NULL
	);

	// Add string option (aka: -a "string value" -b "other string value")
	BP_ERROR_T addStringOption
	(
			char * token,
			size_t token_required,
			char *token_description,
			char *token_usage_example,
			char * default_value,
			size_t max_len,
			size_t min_len,
			void * (*argument_activation_callback)(BP_CommandLineOptionParser * parser, P_BP_COMMAND_LINE_PARSER_TOKENS token) = NULL,
			char ** preg_custom_filters = NULL
	);

	// Add a size_t option (it's the sizeof(size_t) type.  This type
	// of option cannot be negative.
	BP_ERROR_T addSizeTOption
	(
			char * token,
			size_t token_required,
			char *token_description,
			char *token_usage_example,
			size_t default_value,
			size_t max_size,
			size_t min_size,
			void * (*argument_activation_callback)(BP_CommandLineOptionParser * parser, P_BP_COMMAND_LINE_PARSER_TOKENS token) = NULL,
			char ** preg_custom_filters = NULL
	);

	// Adds a signed integer option (can be negative to signed int max on platform)
	BP_ERROR_T addSignedIntegerOption
	(
			char * token,
			size_t token_required,
			char *token_description,
			char *token_usage_example,
			int default_value,
			int max_size,
			int min_size,
			void * (*argument_activation_callback)(BP_CommandLineOptionParser * parser, P_BP_COMMAND_LINE_PARSER_TOKENS token) = NULL,
			char ** preg_custom_filters = NULL
	);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Advanced Data Type Option Addition Methods %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// Add url option (aka: -a "http://something/blah/what?what=boo&boo=urns::etc"
	BP_ERROR_T addUrlOption
	(
			char * token,
			size_t token_required,
			char *token_description,
			char *token_usage_example,
			char * default_value,
			size_t max_len,
			size_t min_len,
			void * (*argument_activation_callback)(BP_CommandLineOptionParser * parser, P_BP_COMMAND_LINE_PARSER_TOKENS token) = NULL,
			char ** preg_custom_filters = NULL
	);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Parsing Methods %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This is the main option parser method.  It relies on the type switch found within the
	// token structure (passed in value) to route the option and corresponding argument to the
	// correct secondary parsing option.  Since this method relies on argc/argv, it directly
	// manipulates the token object to store the relevant positional occurance of the correct argument
	// by mapping it within the token structure itself.  If you use the parseTypeN() methods, this
	// becomes impossible, as they do not parse from argv, and simply parse values instead.  This
	// was done for convienence, but should be noted as a behavior for sake of understanding.  This
	// routine works on any option type, long form (aka --a=b) or short form (aka -a b), and as
	// such is a bit convoluted in nature.  It is recommended to read source/comments slowly
	// and carefully to ensure thorough understanding.
	BP_ERROR_T parseOptions(int argc, char **argv, char ** envp = NULL, size_t envpn = 128);

	// parse stand alone toggle option (no value required)
	BP_ERROR_T parseStandAloneToggleOption(char *option, P_BP_COMMAND_LINE_PARSER_TOKENS token);

	// parse a character string option
	BP_ERROR_T parseStringOption(char *option, char *value,  size_t value_len, P_BP_COMMAND_LINE_PARSER_TOKENS token);

	// parse a sizet option
	BP_ERROR_T parseSizeTOption(char *option, char *value, size_t value_len, P_BP_COMMAND_LINE_PARSER_TOKENS token);

	// parse a signed integer option
	BP_ERROR_T parseSignedIntegerOption(char *option, char *value, size_t value_len, P_BP_COMMAND_LINE_PARSER_TOKENS token);

	// parse a URL option
	BP_ERROR_T parseUrlOption(char *option, char *value,  size_t value_len, P_BP_COMMAND_LINE_PARSER_TOKENS token);

	// Generic option parser, parses all types of options
	BP_ERROR_T parseOption(int argc, char **argv, P_BP_COMMAND_LINE_PARSER_TOKENS token);


	// post parse checks (checks tokens to ensure that values that must be set, have been set and
	// performs basic consistency checks).  This function simply runs the individual token check
	// method against all tokens in the class local token stack.
	BP_ERROR_T runPostParseTokenChecks();

	// Runs a check on a token and determines if the token was set correctly or
	// if it has become erroneous for some reason.  Results of examination are
	// stored as strings which can be read in human readable format.
	BP_ERROR_T runTokenCheck(P_BP_COMMAND_LINE_PARSER_TOKENS token);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Utility Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// option lookup (attempts to resolve a token to a value, automatically works with either short
	// or long options(eg: -a --a=b)
	P_BP_COMMAND_LINE_PARSER_TOKENS optionLookup(char * option_value);

	// default token value as string
	char *token_default_value_as_string;

	// utility routine that will retrieve the default value of the parser, and store it within the local class
	// token_defalt_value_as_string member value.  If the value is already set in the class,
	// it is automatically freed, nullified, and overwritten.  This is simply for convienence.
	BP_ERROR_T getDefaultValueAsCharString(char * option_value);

	// current token value as string (set below)
	char * token_current_value_as_string;

	// utility routine that will retrieve the current set value of the selected token, and store it within the local class
	// token_current_value_as_string member value.  If the value is already set in the class,
	// it is automatically freed, nullified, and overwritten.  This is simply for convienence.
	BP_ERROR_T getCurrentValueAsCharString(char * option_value);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Token Value Gettrs %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// flag indicating whether or not any of the methods below failed.  Can
	// be examined directly after one of the methods below is invoked to see
	// if the value was retrieved ok (BP_TRUE) or if it failed (BP_FALSE).
	BP_BOOL retrieval_ok;


	// retrieves a true or false based on the provided token
	BP_BOOL retrieveStandAloneToggleValueByOption(char *option_value);

	// retrieves a size_t value based on the provided token.
	size_t retrieveSizeTValueByOption(char *option_value);

	// retrieves a signed integer value based on the provided token.
	int retrieveSignedIntegerValueByOption(char *option_value);



	// retrieves a url token based on the provided token.
	char * retrieveUrlValueByOption(char *option_value, BP_BOOL duplicate_value_on_return = BP_FALSE);

	// retrieves a string value based on the provided token
	char * retrieveStringValueByOption(char *option_value, BP_BOOL duplicate_value_on_return = BP_FALSE);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Runs Buffer Analysis Against Token %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Runs a buffer analysis (as found in BP-BufferAnalysis.h) against a provided buffer.  This allows
	// the ability to generate statistics about any given string argument.  This is run before
	// any argument parsing occurs, and the result is stored within the tokens value_buff_analysis member
	// pointer.
	BP_ERROR_T runBufferAnalysisAgainstTokenValue(unsigned char * buffer, size_t buff_len, P_BP_COMMAND_LINE_PARSER_TOKENS token);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Warnings / Errors For Individual Tokens %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add an error to the individual token (must be ascii/null terminated)
	BP_ERROR_T addErrorToToken(P_BP_COMMAND_LINE_PARSER_TOKENS token, char *error_msg);

	// add a warning to the individual token (must be ascii/null terminated)
	BP_ERROR_T addWarningToToken(P_BP_COMMAND_LINE_PARSER_TOKENS token, char *warning_msg);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Visualization / Display Methods %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// display the options as they exist
	BP_ERROR_T displayOptions();

	// display the options parse as JSON (can be used for visualizing things like iptables rules)
	BP_ERROR_T displayOptionsAsJSON(BP_BOOL display_options_set_from_command_line_only, char *extra_data);

	// display a single option
	BP_ERROR_T displayOption(P_BP_COMMAND_LINE_PARSER_TOKENS option);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Clone Routine (For Duplicating Between Threaded Target Selectors %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Cloning creates a rough duplicate of this object, useful for passing onto threads.  This is
	// useful for omitting concurrency problems which might arise due to sharing threaded data.  This
	// prevents the need to lock data between threads, which is a major boon for concurrent optimization.

	// returns a new command line option parser with duplicated values.
	BP_CommandLineOptionParser * clone();



};


#endif /* BP_COMMANDLINEOPTIONPARSER_H_ */

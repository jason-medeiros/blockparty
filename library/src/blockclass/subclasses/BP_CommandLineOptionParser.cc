#include "../../../include/BP-Main.h"

// class constructor
BP_CommandLineOptionParser::BP_CommandLineOptionParser()
{

	// create a new tailqueue allocator for use in this class
	this->tq = BP_CreateLinkL("option parser tq", BP_FALSE);

	// set tokens to null
	this->tokens = NULL;

	// set token count to zero
	this->tokens_n = 0;

	// set argc to zero and argv to null to prevent any initial confusion
	this->argc = 0;
	this->argv = NULL;

	// set envpc to zero and envp to null
	this->envpc = 0;
	this->envp  = NULL;

	// set parse error indicators to false
	this->parsing_tokens_resulted_in_err  = false;
	this->parsing_tokens_resulted_in_warn = false;

	// set duplicate flag to zero
	this->parsing_tokens_resulted_in_duplicate_set = false;

	// utility members
	this->token_default_value_as_string = NULL;
	this->token_current_value_as_string = NULL;

	// set the retrieval status as BP_FALSE as init.
	this->retrieval_ok = BP_FALSE;

}

// class deconstructor
BP_CommandLineOptionParser::~BP_CommandLineOptionParser()
{

	// since buffer analysis structures are allocated with their own
	// tailqueue allocator, we have to iterate and destroy them here.

	if(this->tokens)
	{
		size_t n = 0;
		for(; n < this->tokens_n; n++)
		{

			// if the token has an analysis structure, destroy it
			if(this->tokens[n].value_buff_analysis)
			{
				BP_DestroyBufferAnalysis(this->tokens[n].value_buff_analysis);
				this->tokens[n].value_buff_analysis = NULL;
			}
		}
	}


	// destroy the allocated data if set
	if(this->tq)
	{

		// destroy the linkl
		BP_DestroyLinkL(this->tq, BP_TRUE);

		// nullify the tail queue
		this->tq = NULL;

	}

}

// Add Stand alone toggle option.
BP_ERROR_T BP_CommandLineOptionParser::addToggleOption
(
		char * token,
		size_t token_required,
		char *token_description,
		char *token_usage_example,
		size_t default_value,
		void * (*argument_activation_callback)(BP_CommandLineOptionParser * parser, P_BP_COMMAND_LINE_PARSER_TOKENS token),
		char ** preg_custom_filters
)
{

	// ensure we have a token
	if(!token)
		return ERR_FAILURE;

	// calculate the token length
	size_t token_len = strnlen(token, 100);

	// determine if the token is long form
	bool token_is_long_form = false;
	if(token_len >= 3)
	if
	(
		(token[0] == '-') &&
		(token[1] == '-')
	)
	{
		token_is_long_form = true;
	}

	// add a generic option
	return this->addOption
	(
			token,
			token_is_long_form,
			token_required,
			BP_COMMAND_LINE_ARGUMENT_STAND_ALONE_TOGGLE,
			token_description,
			token_usage_example,
			(void *) default_value,
			0,
			0,
			0,
			0,
			0,
			0,
			argument_activation_callback,
			preg_custom_filters
	);

}


// Add Stand alone toggle option.
BP_ERROR_T BP_CommandLineOptionParser::addStringOption
(
		char * token,
		size_t token_required,
		char *token_description,
		char *token_usage_example,
		char * default_value,
		size_t max_len,
		size_t min_len,
		void * (*argument_activation_callback)(BP_CommandLineOptionParser * parser, P_BP_COMMAND_LINE_PARSER_TOKENS token),
		char ** preg_custom_filters
)
{

	// ensure we have a token
	if(!token)
		return ERR_FAILURE;

	// calculate the token length
	size_t token_len = strnlen(token, 100);

	// determine if the token is long form
	bool token_is_long_form = false;
	if(token_len >= 3)
	if
	(
		(token[0] == '-') &&
		(token[1] == '-')
	)
	{
		token_is_long_form = true;
	}

	// add a generic option
	return this->addOption
	(
			token,
			token_is_long_form,
			token_required,
			BP_COMMAND_LINE_ARGUMENT_CHAR_STRING,
			token_description,
			token_usage_example,
			(void *) default_value,
			max_len,
			min_len,
			0,
			0,
			0,
			0,
			argument_activation_callback,
			preg_custom_filters
	);

}

// Add a size_t option (it's the sizeof(size_t) type.  This type
// of option cannot be negative.
BP_ERROR_T BP_CommandLineOptionParser::addSizeTOption
(
		char * token,
		size_t token_required,
		char *token_description,
		char *token_usage_example,
		size_t default_value,
		size_t max_size,
		size_t min_size,
		void * (*argument_activation_callback)(BP_CommandLineOptionParser * parser, P_BP_COMMAND_LINE_PARSER_TOKENS token),
		char ** preg_custom_filters
)
{

	// ensure we have a token
	if(!token)
		return ERR_FAILURE;

	// calculate the token length
	size_t token_len = strnlen(token, 100);

	// determine if the token is long form
	bool token_is_long_form = false;
	if(token_len >= 3)
	if
	(
		(token[0] == '-') &&
		(token[1] == '-')
	)
	{
		token_is_long_form = true;
	}

	// add a generic option
	return this->addOption
	(
			token,
			token_is_long_form,
			token_required,
			BP_COMMAND_LINE_ARGUMENT_SIZE_T,
			token_description,
			token_usage_example,
			(void *) default_value,
			0,
			0,
			max_size,
			min_size,
			0,
			0,
			argument_activation_callback,
			preg_custom_filters
	);

}


// Adds a signed integer option (can be negative to signed int max on platform)
BP_ERROR_T BP_CommandLineOptionParser::addSignedIntegerOption
(
		char * token,
		size_t token_required,
		char *token_description,
		char *token_usage_example,
		int default_value,
		int max_size,
		int min_size,
		void * (*argument_activation_callback)(BP_CommandLineOptionParser * parser, P_BP_COMMAND_LINE_PARSER_TOKENS token),
		char ** preg_custom_filters
)
{

	// ensure we have a token
	if(!token)
		return ERR_FAILURE;

	// calculate the token length
	size_t token_len = strnlen(token, 100);

	// determine if the token is long form
	bool token_is_long_form = false;
	if(token_len >= 3)
	if
	(
		(token[0] == '-') &&
		(token[1] == '-')
	)
	{
		token_is_long_form = true;
	}

	// add a generic option
	return this->addOption
	(
			token,
			token_is_long_form,
			token_required,
			BP_COMMAND_LINE_ARGUMENT_SIGNED_INT,
			token_description,
			token_usage_example,
			(void *) default_value,
			0,
			0,
			0,
			0,
			max_size,
			min_size,
			argument_activation_callback,
			preg_custom_filters
	);

}



// Add url option (aka: -a "http://something/blah/what?what=boo&boo=urns::etc"
BP_ERROR_T BP_CommandLineOptionParser::addUrlOption
(
		char * token,
		size_t token_required,
		char *token_description,
		char *token_usage_example,
		char * default_value,
		size_t max_len,
		size_t min_len,
		void * (*argument_activation_callback)(BP_CommandLineOptionParser * parser, P_BP_COMMAND_LINE_PARSER_TOKENS token),
		char ** preg_custom_filters
)
{

	// ensure we have a token
	if(!token)
		return ERR_FAILURE;

	// calculate the token length
	size_t token_len = strnlen(token, 100);

	// determine if the token is long form
	bool token_is_long_form = false;
	if(token_len >= 3)
	if
	(
		(token[0] == '-') &&
		(token[1] == '-')
	)
	{
		token_is_long_form = true;
	}

	// add a generic option
	return this->addOption
	(
			token,
			token_is_long_form,
			token_is_long_form,
			BP_COMMAND_LINE_ARGUMENT_URL,
			token_description,
			token_usage_example,
			(void *) default_value,
			max_len,
			min_len,
			0,
			0,
			0,
			0,
			argument_activation_callback,
			preg_custom_filters
	);

}



// adds a command line option to the parser (this is the fallback routine used by all the
// specific argument addition methods and implements all options available.  It's obtuse
// but only as a mechanism to provide functionality to the more specific argument addition
// methods)
BP_ERROR_T BP_CommandLineOptionParser::addOption
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
	void * (*argument_activation_callback)(BP_CommandLineOptionParser * parser, P_BP_COMMAND_LINE_PARSER_TOKENS token),
	char ** preg_custom_filters
)
{

	// ensure a tailqueue allocator is set for us to work with
	if(!this->tq)
		return ERR_FAILURE;

	// ensure we have a token to add
	if(!token)
		return ERR_FAILURE;

	// increment the argument count
	this->tokens_n++;

	// allocate tokens
	this->tokens = (P_BP_COMMAND_LINE_PARSER_TOKENS) tq_realloc(this->tq, this->tokens, sizeof(BP_COMMAND_LINE_PARSER_TOKENS) * this->tokens_n, BPLN);

	// if the token was unabled to be created, we have to exit as we're likely out of memory and about to experience a critical
	// application failure)
	if(!this->tokens)
		return ERR_FAILURE;


	// zero out the allocated structure
	memset(&this->tokens[this->tokens_n-1], 0x00, sizeof(BP_COMMAND_LINE_PARSER_TOKENS));

	// set a reference for readability/use
	P_BP_COMMAND_LINE_PARSER_TOKENS new_token = (P_BP_COMMAND_LINE_PARSER_TOKENS) &this->tokens[this->tokens_n-1];


	// set token
	new_token->token = (char *) tq_strndup(this->tq, token, 4096, BPLN);

	// set the token length
	new_token->token_length = bpstrlen(new_token->token);

	// set long form indicator
	new_token->token_is_long_form = token_is_long_form;

	// set token requirement
	new_token->token_required = token_required;

	// set the type of value expected for the argument
	new_token->value_type = value_type;

	// set the token description
	if(token_description)
		new_token->token_description = (char *) tq_strdup(this->tq, token_description, BPLN);

	// set the token usage
	if(token_usage_example)
		new_token->token_usage_example = (char *) tq_strdup(this->tq, token_usage_example, BPLN);


	// set the default value here
	switch(value_type)
	{

		// if it's unset, do nothing
		case BP_COMMAND_LINE_ARGUMENT_UNSET:
			break;

		// default value as toggle
		case BP_COMMAND_LINE_ARGUMENT_STAND_ALONE_TOGGLE:
			new_token->default_value_as_toggle = default_value ? BP_TRUE : BP_FALSE;
			break;

		// if the value is a string, process string values here
		case BP_COMMAND_LINE_ARGUMENT_CHAR_STRING:

			// check for default value and add if necessary
			if(default_value)
				new_token->default_value_as_string = (char *) tq_strdup(this->tq, (char *) default_value, BPLN);

			// now fill in max/min length for string
			new_token->max_length_for_string = max_length_for_string;
			new_token->min_length_for_string = min_length_for_string;
			break;

		// if the value is a size_t type, process here
		case BP_COMMAND_LINE_ARGUMENT_SIZE_T:

			// set the default value for size_t type here
			new_token->default_value_as_size_t = (size_t) default_value;

			// set the minimum and maximum values for size_t types
			new_token->max_value_as_size_t = max_value_as_size_t;
			new_token->min_value_as_size_t = min_value_as_size_t;
			break;

		// if the value is a signed integer type, process here
		case BP_COMMAND_LINE_ARGUMENT_SIGNED_INT:

			// copy in signed integer directly
			memcpy(&new_token->default_value_as_int, &default_value, sizeof(new_token->default_value_as_int));

			// set the minimum and maximum values for signed integers
			new_token->max_value_as_integer = max_value_as_integer;
			new_token->min_value_as_integer = min_value_as_integer;
			break;

		// if the argument is a url type, process here
		case BP_COMMAND_LINE_ARGUMENT_URL:

			// attempt to validate value as a url
			if
			(
					BP_StringIsURL((char *) default_value, BP_TRUE) == ERR_SUCCESS ||
					BP_StringIsURL((char *) default_value, BP_FALSE) == ERR_SUCCESS
			)
			{
				new_token->default_value_as_string = (char *) tq_strdup(this->tq, (char *) default_value, BPLN);
			}

			// since urls are essentially string types, we use string values here
			new_token->max_length_for_string = max_length_for_string;
			new_token->min_length_for_string = min_length_for_string;
			break;

		// default switch exit if no type can be matched
		default:
			break;

	}

	// if we have a callback to activate for this token, set it here
	if(argument_activation_callback)
	{
		new_token->argument_callback_enabled = BP_TRUE;
		new_token->argument_activation_callback = argument_activation_callback;
	}

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Parsing Methods %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// parse all available options
BP_ERROR_T BP_CommandLineOptionParser::parseOptions(int argc, char **argv, char **envp, size_t envpn)
{


	// check to see that we have argc or argv
	if(!argc)
	{
		printf("\n [+] Command line parser reports no argument count was passed in.  This should never occur.");
		return ERR_FAILURE;
	}

	if(!argv)
	{
		printf("\n [+] Command line parser reports no argument vector was passed in.  This should never occur.");
		return ERR_FAILURE;
	}


	// iterator
	size_t n = 0;

	// check to see if the user passed in an envp pointer
	if(envp)
	{

		// declare string reasonable or not
		BP_BOOL envp_string_is_reasonable = BP_FALSE;

		// envp index
		size_t envp_index_actual = 0;

		// count the number of entries
		size_t alloc_ptr_cnt = BP_CountNullTerminatedPtrArray(envp);

		// only allocate if envp has been set
		if(alloc_ptr_cnt)
		{

			// allocate space for envp (can potentially be a larger value than the actual pointer count, this is
			// not a problem)
			this->envp = (char **) tq_calloc(this->tq, sizeof(char *) * (alloc_ptr_cnt+1), 1, BPLN);

			// now copy in data
			for(n=0; envp[n]; n++)
			{

				// ensure envp string is reasonable
				if(!BP_StringIsReasonablePrintableString(envp[n], bpstrlen(envp[n]), BP_FALSE, BP_FALSE))
				{

					// add envp message
					char tmp_warn_buff[1024];
					BP_ZERO_STACK(tmp_warn_buff);
					snprintf((char *) &tmp_warn_buff, 1000, "When parsing ENVP, the entry at index %zu was found to be unreasonable (contained non-ascii/newlines/carriage returns).  This is not an error, but is only noted if in the case a module developer needs access to a bad ENVP entry.  Disregard this message if you're not a module developer.", n);

					// add the error
					this->AddWarn((char *) &tmp_warn_buff);
					continue;

				}


				// store the envp pointer
				this->envp[envp_index_actual] = tq_strdup(this->tq, envp[n], BPLN);

				// increment the actual index
				envp_index_actual++;

			}

			// set the envpc counter
			this->envpc = envp_index_actual;

		}

	}



	// store localized versions of argc/argv (prevents user manipulation)
	this->argv = (char **) tq_calloc(this->tq, sizeof(char *) * (argc+1), 1, BPLN);
	if(!argv)
	{
		printf("\n [+] command line option parser failed to allocate memory for argv.");
		return ERR_FAILURE;
	}

	// walk all arg count and dup again
	for(n = 0; n < argc; n++)
	{
		this->argv[n] = tq_strdup(this->tq, argv[n], BPLN);
	}

	// ensure we have at least one element
	if(!this->argv[0])
	{
		printf("\n [+] Command line parser reports that this->argv[0] is unset.  This is a critical error as argv[0] should contain at least the application name.");
		return ERR_FAILURE;
	}

	// set argc here
	this->argc = argc;

	// current token to lookup
	P_BP_COMMAND_LINE_PARSER_TOKENS current_token = NULL;

	// walk the argument counter
	for(n=0; n < argc; n++)
	{

		// lookup token
		current_token = this->optionLookup(this->argv[n]);

		// if the token wasn't found, continue the loop and attempt to look
		// for another token.
		if(!current_token)
			continue;

		// switch on the token type (all these switches invoke the same base method
		// but the seperation allows us to customize pre/post initialization behaviors
		// if we so choose to at a later point)
		switch(current_token->value_type)
		{

			case BP_COMMAND_LINE_ARGUMENT_UNSET:
				break;

			// perform toggle if necessary
			case BP_COMMAND_LINE_ARGUMENT_STAND_ALONE_TOGGLE:

				// attempt to parse the stand alone toggle option
				this->parseOption(this->argc, this->argv, current_token);
				break;

			case BP_COMMAND_LINE_ARGUMENT_CHAR_STRING:

				// attempt to parse the character string option
				this->parseOption(this->argc, this->argv, current_token);
				break;

			case BP_COMMAND_LINE_ARGUMENT_SIZE_T:

				// attempt to parse the option as a size_t type
				this->parseOption(this->argc, this->argv, current_token);
				break;

			case BP_COMMAND_LINE_ARGUMENT_SIGNED_INT:

				// attempt to parse the option as a signed integer type
				this->parseOption(this->argc, this->argv, current_token);
				break;

			case BP_COMMAND_LINE_ARGUMENT_URL:

				// attempt to parse out the token
				this->parseOption(this->argc, this->argv, current_token);
				break;

			default:
				break;

		}

	}

	// run the post parse checks
	return this->runPostParseTokenChecks();

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Parse Dispatcher %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

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
BP_ERROR_T BP_CommandLineOptionParser::parseOption(int argc, char **argv, P_BP_COMMAND_LINE_PARSER_TOKENS token)
{

	// return indicating failure in the case that one of the necessary values haven't been provided
	if(!argc || !argv || !token || (argc < 0))
		return ERR_FAILURE;

	// don't duplicate efforts
	if(token->value_has_been_set_ok == true)
		return ERR_SUCCESS;

	// token must contain a parsable token value
	if(!token->token)
		return ERR_FAILURE;



	// used for determining initial check via length of values
	size_t pos_iter_len = 0;

	// lenth of value associated with the iter
	size_t pos_value_len = 0;

	// length of the token associated with the iter (correct for both long and short
	// term options)
	size_t pos_token_len = 0;

	// used for determining whether or not the curent argv position
	// being examined is a long form value or not.
	size_t current_item_is_long_form = BP_FALSE;

	// stores a token which was looked up via the lookup method
	// below.
	P_BP_COMMAND_LINE_PARSER_TOKENS token_lookup = (P_BP_COMMAND_LINE_PARSER_TOKENS) NULL;

	// this variable holds the split from a long form token.  Example would
	// be something like: --long-form=blah gets cut into --long-form and blah
	// and stored within this array.
	char ** long_form_token_split = (char **) NULL;

	// integer value used for conversions
	int integer_val = 0;

	// final processed string
	char *final_string = (char *) NULL;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% References Used for Secondary Extraction %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// contains the token extracted (do not free; only a reference)
	char * token_parsed = NULL;

	// contains the value extracted (do not free; only a reference)
	char * value_parsed = NULL;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Main Extraction Loop Logic %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// NOTE: While it APPEARS that this is processing all of the available options
	//       this is not the case.  This loop processes exactly ONE token, no more.

	// Find token and assign.
	int n = 0;
	for(; n < argc; n++)
	{

		// ensure there is an argument in position
		if(!argv[n])
			break;

		// if the argument isn't reasonable/printable text, continue the loop
		if(!BP_StringIsReasonablePrintableString(argv[n], bpstrlen(argv[n]), BP_FALSE, BP_FALSE))
		{
			printf("\n [!!] Passed in command line option %i:%s contained non-ascii binary data (option or input skipped)", n, argv[n]);
			continue;
		}

		// calculate the iter length
		pos_iter_len = (size_t) strnlen(argv[n], 65535);


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Process Long Form Options %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// Processing long form options takes additional work compared to short form options.  This
		// is for a few reasons.
		//
		// 	1) Long form options are one string, as opposed to two flag/value strings.
		//  2) Long form options can be quote encased within a single string.

		if(token->token_is_long_form)
		{

			// long form items must be greater than or equal to 3 bytes long (aka --a=, --a).  Never
			// process shorter values.
			if(pos_iter_len < 3)
				continue;

			// attempt to split if the type is not a toggle
			if(token->value_type != BP_COMMAND_LINE_ARGUMENT_STAND_ALONE_TOGGLE)
			{

				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				// %%% Stop Erroneous Extraction Here %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

				// split the token along first encountered '=' boundary
				long_form_token_split = BP_SplitBufferBin(argv[n], pos_iter_len, "=", 1);

				// Ensure the long form token split was split ok, If not, destroy and
				// continue things.
				if
				(
					BP_CountNumberOfPointersInPointerArray((void **) long_form_token_split, 10) != 2
				)
				{

					// destroy data if necessary
					if(long_form_token_split)
					{

						// destroy the null terminated pointer array elements
						BP_DestroyPointerArrayNullTerminated((void **) long_form_token_split);

						// destroy the actual allocated pointer stack
						bpfree(long_form_token_split);

						// nullify token split here
						long_form_token_split = NULL;

					}

					// if we have a next parameter, process it as the input for the current flag
					if(argv[n+1])
					{

						// if we aren't able to split our token into two pieces dividing by the = character
						// then we need to check to see if we can use argv[n+1] instead (eg. --flag "value")

						// allocate space for our split token buffer
						long_form_token_split = (char **) bpcalloc(sizeof(char *)*3, 1);

						// set split as bp duplicated elements
						long_form_token_split[0] = bpstrdup(argv[n]);
						long_form_token_split[1] = bpstrdup(argv[n+1]);

					}
					else
					{

						// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						// !!! continue the loop at this point as no additional processing needs to be done. !!
						// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						continue;

					}

				}

			}
			else /* if the option is a stand alone toggle, process it here */
			{

				// parse the stand alone option here
				this->parseStandAloneToggleOption(argv[n], token);

				// argv position and value position should be the same since
				// this is a long form option.
				token->argv_position_of_option = n;
				token->argv_position_of_value  = n;

				// continue the loop here since we've parsed out the option
				continue;

			}



			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %% Parse Non-Toggle Options %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


			// now we know that we have a set of tokens to process, check for quoted strings
			final_string = (char *) NULL;

			// calculate the value length at the current position
			pos_value_len = strnlen(long_form_token_split[1], 65535);

			// run quote check
			BP_STRING_QUOTE_CHECK_RESULTS quote_check = BP_CheckIfBufferIsQuoted
			(
				long_form_token_split[1],
				pos_value_len
			);

			// store quote check
			token->value_parse_quote_check = quote_check;

			// Switch on the quote check, and unquote if necessary.  Handle
			// erroneous conditions here as well.
			switch(quote_check)
			{

				// check of string is quoted
				case BP_STRING_IS_DOUBLE_QUOTED:
				case BP_STRING_IS_SINGLE_QUOTED:

					// attempt unquote here (works with double or single quotes)
					final_string = BP_UnquoteQuotedString
					(
							long_form_token_split[1],
							strnlen(long_form_token_split[1], 65535)
					);

					// if the string was unquoted successfully, destroy the
					// previous set and reset here.
					if(final_string)
					{
						bpfree(long_form_token_split[1]);
						long_form_token_split[1] = final_string;
					}
					break;


				// mismatched quote error
				case BP_STRING_IS_MISMATCHED_QUOTED:
					break;

				// mixed quote error
				case BP_STRING_IS_MIXED_QUOTED_ERROR:
					break;


				// if the string is some variation of
				default:
					break;

			}



			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Token/Value Parsed and Unquoted Now; Assign and Process %%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// set values from the split
			token_parsed = long_form_token_split[0];
			value_parsed = long_form_token_split[1];

			// now that we have a token set with front part and end
			// part, we can attempt to look up the token
			pos_token_len = strnlen(token_parsed, 65535);

			// if we stripped quotes, reset the value length here
			pos_value_len = strnlen(value_parsed, 65535);

			// set default integer value
			integer_val = 0;


			// first do a simple length check, then a mem comparison
			if(token->token_length == pos_token_len)
			if(memcmp(token->token, token_parsed, pos_token_len) == 0)
			{

				// at this point we've matched the token and can run a value assignment
				switch(token->value_type)
				{

					// unset value / do nothing
					case BP_COMMAND_LINE_ARGUMENT_UNSET:
						break;

					// parse character string if necessary
					case BP_COMMAND_LINE_ARGUMENT_CHAR_STRING:

						// parse the string option here
						this->parseStringOption(token_parsed, value_parsed, pos_value_len, token);
						token->argv_position_of_option = n;
						token->argv_position_of_value  = n;
						break;

					// parse size_t if necessary
					case BP_COMMAND_LINE_ARGUMENT_SIZE_T:

						// parse the size_t option
						this->parseSizeTOption(token_parsed, value_parsed, pos_value_len, token);
						token->argv_position_of_option = n;
						token->argv_position_of_value  = n;
						break;

					// parse signed integer if necessary
					case BP_COMMAND_LINE_ARGUMENT_SIGNED_INT:

						// parse the signed int
						this->parseSignedIntegerOption(token_parsed, value_parsed, pos_value_len, token);
						token->argv_position_of_option = n;
						token->argv_position_of_value  = n;
						break;

					// parse command line argument if necessary
					case BP_COMMAND_LINE_ARGUMENT_URL:

						// parse a url option
						this->parseUrlOption(token_parsed, value_parsed, pos_value_len, token);
						token->argv_position_of_option = n;
						token->argv_position_of_value  = n;
						break;

					// default case, do nothing
					default:
						break;

				}

			}


			// destroy the null terminated pointer array elements
			if(long_form_token_split)
			{
				BP_DestroyPointerArrayNullTerminated((void **) long_form_token_split);
				bpfree(long_form_token_split);
			}

			// nullify
			long_form_token_split = NULL;



		}
		else
		{

			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Process Short Form Options %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


			// short form values must be 2 bytes long (aka -a -b -c)
			if(pos_iter_len != token->token_length)
				continue;


			// only porcess if we have a token match
			if(memcmp(token->token, argv[n], token->token_length) != 0)
				continue;


			// always reset positional value length on loop
			pos_value_len = 0;

			// if the token is not a stand alone value, we must have another
			// entry in argv[n+1] for a stored value.
			if(token->value_type != BP_COMMAND_LINE_ARGUMENT_STAND_ALONE_TOGGLE)
			{

				if(!((n+1) < argc))
					continue;
				if(!argv[n+1])
					continue;

				// assign initial value here
				value_parsed = argv[n+1];

				// get value length
				pos_value_len = strnlen(argv[n+1], 65535);

				// ensure we have an adequte value length
				if(!pos_value_len)
					continue;

				// attempt to unquote the string (returns NULL if the value is unquoted)
				final_string = BP_UnquoteQuotedString(value_parsed, pos_value_len);

				// ensure we have a final value duplicated (must be duplicated as it's always freed)
				if(!final_string)
					final_string = bpstrndup(value_parsed, pos_value_len);
				else
					pos_value_len = strnlen(final_string, 65535);

			}



			// stand alone toggles can be processed as a simple switch
			switch(token->value_type)
			{

				case BP_COMMAND_LINE_ARGUMENT_STAND_ALONE_TOGGLE:
					this->parseStandAloneToggleOption(argv[n], token);
					token->argv_position_of_option = n;
					token->argv_position_of_value  = 0;
					break;

				case BP_COMMAND_LINE_ARGUMENT_CHAR_STRING:
					this->parseStringOption(argv[n], final_string, pos_value_len, token);
					token->argv_position_of_option = n;
					token->argv_position_of_value  = n+1;
					n++;
					break;

				case BP_COMMAND_LINE_ARGUMENT_SIZE_T:
					this->parseSizeTOption(argv[n], final_string, pos_value_len, token);
					token->argv_position_of_option = n;
					token->argv_position_of_value  = n+1;
					n++;
					break;

				case BP_COMMAND_LINE_ARGUMENT_SIGNED_INT:
					this->parseSignedIntegerOption(argv[n], final_string, pos_value_len, token);
					token->argv_position_of_option = n;
					token->argv_position_of_value  = n+1;
					n++;
					break;

				case BP_COMMAND_LINE_ARGUMENT_URL:
					this->parseUrlOption(argv[n], final_string, pos_value_len, token);
					token->argv_position_of_option = n;
					token->argv_position_of_value  = n+1;
					n++;
					break;

				// default case, do nothing
				default:
					break;

			}

			// destroy the value here if necessary
			if(final_string)
			{
				bpfree(final_string);
				final_string = NULL;
			}


		}

		// zero out the lengths on loop
		pos_value_len=0;
		pos_iter_len =0;

		// !!! Loop Continues Here !!!


	}


	// return indicating success
	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Individual Parse Option Methods %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// parse a character string option
BP_ERROR_T BP_CommandLineOptionParser::parseStandAloneToggleOption(char *option, P_BP_COMMAND_LINE_PARSER_TOKENS token)
{


	// return indicating failure
	if(!option || !token)
		return ERR_FAILURE;

	// ensure that the token has not already been set (prevents duplicates)
	if(token->value_has_been_set_ok)
		return ERR_FAILURE;

	// now we know that we have a set of tokens to process, check for quoted strin7gs
	char *final_string = (char *) NULL;

	// calculate the token length
	size_t token_len = strnlen(option, 65535);

	// ensure that the token supplied matches length
	if(token_len != token->token_length)
		return ERR_FAILURE;

	// ensure that the two buffers compare identically
	if(memcmp(token->token, option, token_len) != 0)
		return ERR_FAILURE;

	// apply toggle here if necessary
	if(token->default_value_as_toggle == BP_FALSE)
		token->parsed_value_as_toggle = BP_TRUE;
	else
		token->parsed_value_as_toggle = BP_FALSE;

	// set the value parse flag here
	token->value_has_been_set_ok = true;


	// return indicating success
	return ERR_SUCCESS;

}

// parse a character string option
BP_ERROR_T BP_CommandLineOptionParser::parseStringOption(char *option, char *value,  size_t value_len, P_BP_COMMAND_LINE_PARSER_TOKENS token)
{

	// return indicating failure
	if(!option || !value || !token)
		return ERR_FAILURE;

	// ensure that the token has not already been set (prevents duplicates)
	if(token->value_has_been_set_ok)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Buffer Analysis First %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to run analysis
	if(this->runBufferAnalysisAgainstTokenValue((unsigned char *) value, value_len, token) != ERR_SUCCESS)
		return ERR_FAILURE;

	// string values cannot contain non-printables
	if(token->value_contains_nonprintables)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Now Extract and Store Options %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// run length checks here
	if(value_len < token->min_length_for_string || value_len > token->max_length_for_string)
		return ERR_FAILURE;

	// set the value here
	token->parsed_value_as_string = (char *) tq_strndup(this->tq, value, value_len, BPLN);

	// increment the number of times it was found in argv
	token->option_encountered_in_argv_count++;

	// set flag indicating that things were set ok
	token->value_has_been_set_ok = true;

	// return indicating success
	return ERR_SUCCESS;

}

// parse a sizet option
BP_ERROR_T BP_CommandLineOptionParser::parseSizeTOption(char *option, char *value, size_t value_len, P_BP_COMMAND_LINE_PARSER_TOKENS token)
{


	// return indicating failure
	if(!option || !value || !token)
		return ERR_FAILURE;

	// ensure that the token has not already been set (prevents duplicates)
	if(token->value_has_been_set_ok)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Buffer Analysis  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to run analysis
	if(this->runBufferAnalysisAgainstTokenValue((unsigned char *) value, value_len, token) != ERR_SUCCESS)
		return ERR_FAILURE;

	// the value must be a positive number
	if(!token->value_contains_a_positive_number)
		return ERR_FAILURE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Now Extract and Store Options %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set size_t value here from provided string (error is zero)
	size_t size_t_val = strtoul(value, NULL, 10);

	// check min and max values
	if
	(
		(size_t_val < token->min_value_as_size_t) ||
		(size_t_val > token->max_value_as_size_t)
	)
	{

		// return indicating failure
		return ERR_FAILURE;

	}


	// set zero value here
	if(size_t_val == 0)
	if(strnlen(value, 10) == 1)
	if(value[0] == '0')
	{

		// set the value here
		token->parsed_value_as_size_t = 0;

		// increment the number of times it was found in argv
		token->option_encountered_in_argv_count++;

		// set flag indicating that things were set ok
		token->value_has_been_set_ok = true;

	}

	// set non-zero value here
	if(size_t_val)
	{

		// set the value here
		token->parsed_value_as_size_t = size_t_val;
		token->option_encountered_in_argv_count++;

		// set flag indicating that things were set ok
		token->value_has_been_set_ok = true;

		// return here if value has been set
		return ERR_SUCCESS;

	}

	// return indicating failure if no value can be set
	return ERR_FAILURE;

}

// parse a signed integer option
BP_ERROR_T BP_CommandLineOptionParser::parseSignedIntegerOption(char *option, char *value, size_t value_len, P_BP_COMMAND_LINE_PARSER_TOKENS token)
{

	// return indicating failure
	if(!option || !value || !token)
		return ERR_FAILURE;

	// ensure that the token has not already been set (prevents duplicates)
	if(token->value_has_been_set_ok)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Buffer Analysis  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to run analysis
	if(this->runBufferAnalysisAgainstTokenValue((unsigned char *) value, value_len, token) != ERR_SUCCESS)
		return ERR_FAILURE;

	// the value must be a positive number
	if(token->value_contains_a_positive_number || token->value_contains_a_negative_number)
	{} else return ERR_FAILURE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Now Extract and Store Options %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set default integer value
	int integer_val = integer_val = strtol(value, NULL, 10);

	// check min and max values
	if
	(
		(integer_val < token->min_value_as_integer) ||
		(integer_val > token->max_value_as_integer)
	)
		return ERR_FAILURE;

	// if a value is too large for ATOI to process, the routine
	// will return a value of zero.  However, considering that zero
	// is a valid option, we run this check to perform zero assignment
	// if necessary.
	if(integer_val == 0)
	if(value_len   == 1)
	if(value[0]    == '0')
	{

		// set the value here
		token->parsed_value_as_int = 0;

		// increment the number of times it was found in argv
		token->option_encountered_in_argv_count++;

		// set flag indicating that things were set ok
		token->value_has_been_set_ok = true;

		// exit as option has been set
		return ERR_SUCCESS;

	}


	// if the integer value is any other value, we can set it here
	if(integer_val)
	{

		// set the value here
		token->parsed_value_as_int = integer_val;
		token->option_encountered_in_argv_count++;

		// set flag indicating that things were set ok
		token->value_has_been_set_ok = true;

		// return here if value has been set
		return ERR_SUCCESS;

	}

	// return indicating failure if no value can be set
	return ERR_FAILURE;

}

// parse a URL option
BP_ERROR_T BP_CommandLineOptionParser::parseUrlOption(char *option, char *value, size_t value_len, P_BP_COMMAND_LINE_PARSER_TOKENS token)
{

	// return indicating failure
	if(!option || !value || !token)
		return ERR_FAILURE;

	// ensure that the token has not already been set (prevents duplicates)
	if(token->value_has_been_set_ok)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Buffer Analysis First %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to run analysis
	if(this->runBufferAnalysisAgainstTokenValue((unsigned char *) value, value_len, token) != ERR_SUCCESS)
		return ERR_FAILURE;

	// string values cannot contain non-printables
	if(token->value_contains_nonprintables)
		return ERR_FAILURE;

	// check if the string is a url
	if
	(
			(BP_StringIsURL(value, BP_TRUE)  != ERR_SUCCESS) &&
			(BP_StringIsURL(value, BP_FALSE) != ERR_SUCCESS)
	)
	{
		return ERR_FAILURE;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Now Extract and Store Options %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// run length checks here
	if(value_len < token->min_length_for_string || value_len > token->max_length_for_string)
		return ERR_FAILURE;

	// set the value here
	token->parsed_value_as_string = (char *) tq_strndup(this->tq, value, value_len, BPLN);

	// increment the number of times it was found in argv
	token->option_encountered_in_argv_count++;

	// set flag indicating that things were set ok
	token->value_has_been_set_ok = true;


	// return indicating success
	return ERR_SUCCESS;

}




// post parse checks (checks tokens to ensure that values that must be set, have been set and
// performs basic consistency checks)
BP_ERROR_T BP_CommandLineOptionParser::runPostParseTokenChecks()
{


	// check token stack
	if(!this->tokens)
	{
		this->AddErr("Token Pointer is NULL.");
		return ERR_FAILURE;
	}

	// check token count
	if(!this->tokens_n)
	{
		this->AddErr("Token count is zero.");
		return ERR_FAILURE;
	}

	// current token for examination
	P_BP_COMMAND_LINE_PARSER_TOKENS current_token = NULL;

	// walk all the tokens and run checks
	size_t n = 0;
	for(; n < this->tokens_n; n++)
	{

		// set current token for code simplification
		current_token = &this->tokens[n];

		// run the token check
		if(this->runTokenCheck(current_token) != ERR_SUCCESS)
		{

			// create temporary buffer
			char tmp_buff[1024];
			BP_ZERO_STACK(tmp_buff);

			// add err
			snprintf((char *) &tmp_buff, 1000, "Command line option parser token check failed on token index %zu (%s).  Please ensure you've set your parameters correctly.", n, current_token->token);
			this->AddErr((char *) &tmp_buff);
			return ERR_FAILURE;
		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// Runs a check on a token and determines if the token was set correctly or
// if it has become erroneous for some reason.  Results of examination are
// stored as strings which can be read in human readable format.
BP_ERROR_T BP_CommandLineOptionParser::runTokenCheck(P_BP_COMMAND_LINE_PARSER_TOKENS token)
{

	// ensure we have a token
	if(!token)
		return ERR_FAILURE;

	// check if the token is required.  If it is required, it must be set ok
	if(token->token_required && !token->value_has_been_set_ok)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Utility Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// option lookup (attempts to resolve a token to a value, automatically works with either short
// or long options(eg: -a --a=b)
P_BP_COMMAND_LINE_PARSER_TOKENS BP_CommandLineOptionParser::optionLookup(char * option_value)
{

	// check to ensure we have an option value
	if(!option_value)
		return NULL;

	// ensure we have a token count
	if(!this->tokens_n)
		return NULL;

	// ensure we have tokens to work with
	if(!this->tokens)
		return NULL;

	// calculate option length
	size_t option_length = (size_t) strnlen(option_value, 65535);

	// ensure we have at minimum length of two (aka -a)
	if(option_length < 2)
		return NULL;

	// iterator used for searching
	size_t n = 0;

	// used to hold a split buffer if necessary
	char ** split_buffer = NULL;

	// if else to handle long form or short form options
	if(option_length == 2)
	{

		// look for short-form options here
		for(; n < this->tokens_n; n++)
		{

			// check for long form, skip if so
			if(!this->tokens[n].token_is_long_form)
			{

				// ensure that the string length matches the option (prevents undercutting)
				if(bpstrlen(this->tokens[n].token) == option_length)
				{

					// if the option matches the token, return the structure as a pointer
					if(memcmp(this->tokens[n].token, option_value, option_length) == 0)
						return &this->tokens[n];

				}

			}

		}

	}
	else
	{

		// look for long form options here
		for(; n < this->tokens_n; n++)
		{

			// check for long form, process if so
			if(this->tokens[n].token_is_long_form)
			{


				// split the buffer if possible
				char ** split_buffer = BP_SplitBufferBin(option_value, option_length, "=", 1);


				// add this to the below if statement if something broke
				// if(this->tokens[n].value_type == BP_COMMAND_LINE_ARGUMENT_STAND_ALONE_TOGGLE)

				// if it's a stand alone toggle, there is no split buffer available, so we
				// can immediately run cascading checks and return the lookup if available
				if(!split_buffer)
				if(memcmp(this->tokens[n].token, option_value, option_length) == 0)
					return &this->tokens[n];

				// if the buffer cannot be split along delimited lines, return NULL
				// to indicate that the attempt failed.
				if(!split_buffer)
				{



				}
				else
				{

					// printf("\n SHEEEEET: %s - %s", option_value, this->tokens[n].token);

					// ensure that the string length matches the option (prevents undercutting)
					if(bpstrlen(this->tokens[n].token) == strnlen(split_buffer[0], 65535))
					{

						// if the option matches the token, return the structure as a pointer
						if(memcmp(this->tokens[n].token, split_buffer[0], strnlen(split_buffer[0], 65535)) == 0)
						{


							// destroy split buffer entries
							if(split_buffer[0])
								bpfree(split_buffer[0]);
							if(split_buffer[1])
								bpfree(split_buffer[1]);

							// destroy the split buffer
							bpfree(split_buffer);

							// if the token was matched successfully, return here
							return &this->tokens[n];

						}

					}

				}

				// destroy split buffer entries if necessary
				if(split_buffer)
				{
					if(split_buffer[0])
						bpfree(split_buffer[0]);
					if(split_buffer[1])
						bpfree(split_buffer[1]);

					// destroy the split buffer
					bpfree(split_buffer);

					// nullify on loop
					split_buffer = NULL;

				}


			}

			// continue loop

		}

	}


	// destroy split buffer entries if necessary
	if(split_buffer)
	{
		if(split_buffer[0])
			bpfree(split_buffer[0]);
		if(split_buffer[1])
			bpfree(split_buffer[1]);

		// destroy the split buffer
		bpfree(split_buffer);

	}


	// if no match is found, return NULL
	return NULL;

}


// utility routine that will retrieve the default value of the parser, and store it within the local class
// token_defalt_value_as_string member value.  If the value is already set in the class,
// it is automatically freed, nullified, and overwritten.  This is simply for convienence.
BP_ERROR_T BP_CommandLineOptionParser::getDefaultValueAsCharString(char * option_value)
{


	// ensure we have an option value
	if(!option_value)
		return ERR_FAILURE;

	// option for local lookup
	P_BP_COMMAND_LINE_PARSER_TOKENS token_lookup = NULL;

	// run option lookup
	token_lookup = this->optionLookup(option_value);
	if(!token_lookup)
		return ERR_FAILURE;

	// destroy value if it's set
	if(this->token_default_value_as_string)
	{
		tq_free(this->tq, this->token_default_value_as_string);
		this->token_default_value_as_string = NULL;
	}

	char tmp_buff[BP_KILOBYTE/2];
	BP_ZERO_STACK(tmp_buff);

	// create buffer by checking type and creating appropriate buffer
	switch(token_lookup->value_type)
	{

		case BP_COMMAND_LINE_ARGUMENT_UNSET:
			return ERR_FAILURE;

		case BP_COMMAND_LINE_ARGUMENT_STAND_ALONE_TOGGLE:
			this->token_default_value_as_string = tq_strdup(this->tq, token_lookup->default_value_as_toggle ? (char *) "On" :(char *)  "Off", BPLN);
			return ERR_SUCCESS;

		case BP_COMMAND_LINE_ARGUMENT_CHAR_STRING:
			this->token_default_value_as_string = tq_strdup(this->tq, token_lookup->default_value_as_string ?  token_lookup->default_value_as_string : (char *) "Unset String Value", BPLN);
			return ERR_SUCCESS;

		case BP_COMMAND_LINE_ARGUMENT_SIZE_T:
			sprintf(tmp_buff, "%zu", token_lookup->default_value_as_size_t);
			this->token_default_value_as_string = tq_strdup(this->tq, tmp_buff, BPLN);
			return ERR_SUCCESS;

		case BP_COMMAND_LINE_ARGUMENT_SIGNED_INT:
			sprintf(tmp_buff, "%i", token_lookup->default_value_as_int);
			this->token_default_value_as_string = tq_strdup(this->tq, tmp_buff, BPLN);
			return ERR_SUCCESS;

		case BP_COMMAND_LINE_ARGUMENT_URL:
			this->token_default_value_as_string = tq_strdup(this->tq, token_lookup->default_value_as_string ? token_lookup->default_value_as_string : (char *) "Unset" , BPLN);
			return ERR_SUCCESS;

	}

	// return indicating success (should technically never get here)
	return ERR_SUCCESS;

}



// utility routine that will retrieve the current set value of the selected token, and store it within the local class
// token_current_value_as_string member value.  If the value is already set in the class,
// it is automatically freed, nullified, and overwritten.  This is simply for convienence.
BP_ERROR_T BP_CommandLineOptionParser::getCurrentValueAsCharString(char * option_value)
{

	// ensure we have an option value
	if(!option_value)
		return ERR_FAILURE;

	// option for local lookup
	P_BP_COMMAND_LINE_PARSER_TOKENS token_lookup = NULL;

	// run option lookup
	token_lookup = this->optionLookup(option_value);
	if(!token_lookup)
		return ERR_FAILURE;

	// retrieving current values requires a successful parse
	if(!token_lookup->value_has_been_set_ok)
		return ERR_FAILURE;


	// destroy value if it's set
	if(this->token_current_value_as_string)
	{
		tq_free(this->tq, this->token_current_value_as_string);
		this->token_current_value_as_string = NULL;
	}

	// temporary buffer used for building int/size_t values
	char tmp_buff[BP_KILOBYTE/2];
	BP_ZERO_STACK(tmp_buff);

	// create buffer by checking type and creating appropriate buffer
	switch(token_lookup->value_type)
	{

		case BP_COMMAND_LINE_ARGUMENT_UNSET:
			return ERR_FAILURE;

		case BP_COMMAND_LINE_ARGUMENT_STAND_ALONE_TOGGLE:
			this->token_current_value_as_string = tq_strdup(this->tq, token_lookup->parsed_value_as_toggle ? (char *) "On" :(char *)  "Off", BPLN);
			return ERR_SUCCESS;

		case BP_COMMAND_LINE_ARGUMENT_CHAR_STRING:
			this->token_current_value_as_string = tq_strdup(this->tq, (char *) token_lookup->parsed_value_as_string ? token_lookup->parsed_value_as_string : (char *) "Unset", BPLN);
			return ERR_SUCCESS;

		case BP_COMMAND_LINE_ARGUMENT_SIZE_T:
			sprintf(tmp_buff, "%zu", token_lookup->parsed_value_as_size_t);
			this->token_current_value_as_string = tq_strdup(this->tq, tmp_buff, BPLN);
			return ERR_SUCCESS;

		case BP_COMMAND_LINE_ARGUMENT_SIGNED_INT:
			sprintf(tmp_buff, "%i", token_lookup->parsed_value_as_int);
			this->token_current_value_as_string = tq_strdup(this->tq, tmp_buff, BPLN);
			return ERR_SUCCESS;

		case BP_COMMAND_LINE_ARGUMENT_URL:
			this->token_current_value_as_string = tq_strdup(this->tq, token_lookup->parsed_value_as_string ?(char *)  "Unset" : token_lookup->parsed_value_as_string, BPLN);
			return ERR_SUCCESS;

	}

	// return indicating success (should technically never get here)
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Token Value Gettrs %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Retrieves selected (user selected) values, either default value, or
// the set value, depending on token type.

// ================ By Option =======================

// retrieves a true or false based on the provided token
BP_BOOL BP_CommandLineOptionParser::retrieveStandAloneToggleValueByOption(char *option_value)
{

	// ensure we have an option value
	if(!option_value)
	{
		this->retrieval_ok = BP_FALSE;
		return BP_FALSE;
	}

	// attempt to lookup the token
	P_BP_COMMAND_LINE_PARSER_TOKENS token = this->optionLookup(option_value);
	if(!token)
	{
		this->retrieval_ok = BP_FALSE;
		return BP_FALSE;
	}

	if(token->value_has_been_set_ok)
	{
		this->retrieval_ok = BP_TRUE;
		return token->parsed_value_as_toggle;
	}
	else
	{
		this->retrieval_ok = BP_TRUE;
		return token->default_value_as_toggle;
	}


}

// retrieves a string value based on the provided token
char * BP_CommandLineOptionParser::retrieveStringValueByOption(char *option_value, BP_BOOL duplicate_value_on_return)
{

	// ensure we have an option value
	if(!option_value)
	{
		this->retrieval_ok = BP_FALSE;
		return NULL;
	}

	// attempt to lookup the token
	P_BP_COMMAND_LINE_PARSER_TOKENS token = this->optionLookup(option_value);
	if(!token)
	{
		this->retrieval_ok = BP_FALSE;
		return NULL;
	}

	if(token->value_has_been_set_ok)
	{

		// set retrieval indicator as OK
		this->retrieval_ok = BP_TRUE;

		// duplicate on return if specified
		if(duplicate_value_on_return)
		{
			if(!token->parsed_value_as_string)
				return NULL;

			return strdup(token->parsed_value_as_string);
		}
		else
			return token->parsed_value_as_string;


	}
	else
	{

		// set retrieval indicator as OK
		this->retrieval_ok = BP_TRUE;

		// duplicate on return if specified
		if(duplicate_value_on_return)
		{

			if(!token->default_value_as_string)
				return NULL;

			return strdup(token->default_value_as_string);

		}
		else
			return token->default_value_as_string;

	}

}

// retrieves a size_t value based on the provided token.
size_t BP_CommandLineOptionParser::retrieveSizeTValueByOption(char *option_value)
{

	// ensure we have an option value
	if(!option_value)
	{
		this->retrieval_ok = BP_FALSE;
		return 0;
	}

	// attempt to lookup the token
	P_BP_COMMAND_LINE_PARSER_TOKENS token = this->optionLookup(option_value);
	if(!token)
	{
		this->retrieval_ok = BP_FALSE;
		return 0;
	}

	if(token->value_has_been_set_ok)
	{
		this->retrieval_ok = BP_TRUE;
		return token->parsed_value_as_size_t;
	}
	else
	{
		this->retrieval_ok = BP_TRUE;
		return token->default_value_as_size_t;
	}


}

// retrieves a signed integer value based on the provided token.
int BP_CommandLineOptionParser::retrieveSignedIntegerValueByOption(char *option_value)
{

	// ensure we have an option value
	if(!option_value)
	{
		this->retrieval_ok = BP_FALSE;
		return 0;
	}

	// attempt to lookup the token
	P_BP_COMMAND_LINE_PARSER_TOKENS token = this->optionLookup(option_value);
	if(!token)
	{
		this->retrieval_ok = BP_FALSE;
		return 0;
	}

	if(token->value_has_been_set_ok)
	{
		this->retrieval_ok = BP_TRUE;
		return token->parsed_value_as_int;
	}
	else
	{
		this->retrieval_ok = BP_TRUE;
		return token->default_value_as_int;
	}

}

// retrieves a url token based on the provided token.
char * BP_CommandLineOptionParser::retrieveUrlValueByOption(char *option_value, BP_BOOL duplicate_value_on_return)
{

	// ensure we have an option value
	if(!option_value)
	{
		this->retrieval_ok = BP_FALSE;
		return NULL;
	}

	// attempt to lookup the token
	P_BP_COMMAND_LINE_PARSER_TOKENS token = this->optionLookup(option_value);
	if(!token)
	{
		this->retrieval_ok = BP_FALSE;
		return NULL;
	}

	// generate value or dupe if necessary
	if(token->value_has_been_set_ok)
	{

		// set retrieval indicator as OK
		this->retrieval_ok = BP_TRUE;

		// duplicate on return if specified
		if(duplicate_value_on_return)
		{
			if(!token->parsed_value_as_string)
				return NULL;

			return strdup(token->parsed_value_as_string);
		}
		else
			return token->parsed_value_as_string;


	}
	else
	{

		// set retrieval indicator as OK
		this->retrieval_ok = BP_TRUE;

		// duplicate on return if specified
		if(duplicate_value_on_return)
		{

			if(!token->default_value_as_string)
				return NULL;

			return strdup(token->default_value_as_string);

		}
		else
			return token->default_value_as_string;


	}

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Runs Buffer Analysis Against Token %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Runs a buffer analysis (as found in BP-BufferAnalysis.h) against a provided buffer.  This allows
// the ability to generate statistics about any given string argument.  This is run before
// any argument parsing occurs, and the result is stored within the tokens value_buff_analysis member
// pointer.
BP_ERROR_T BP_CommandLineOptionParser::runBufferAnalysisAgainstTokenValue(unsigned char * buffer, size_t buff_len, P_BP_COMMAND_LINE_PARSER_TOKENS token)
{

	// ensure we have a buffer to analyze, a length to check, and a token to attribute
	// the analysis to
	if(!buffer || !buff_len || !token)
		return ERR_FAILURE;

	// if we already have an analysis for this token, destroy it first and
	// reset reference indicators
	if(token->value_buff_analysis)
	{

		// destroy first
		BP_DestroyBufferAnalysis(token->value_buff_analysis);

		// nullify analysis
		token->value_buff_analysis = NULL;

		// reset indicators
		token->value_contains_backticks              = false;
		token->value_contains_nonprintables          = false;
		token->value_contains_embedded_single_quotes = false;
		token->value_contains_embedded_double_quotes = false;
		token->value_contains_a_positive_number      = false;
		token->value_contains_a_negative_number      = false;

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Buffer Analysis %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set/reset analysis configurations
	token->ascii_analysis_configuration.count_ascii                 = BP_TRUE;
	token->ascii_analysis_configuration.count_extended              = BP_TRUE;
	token->ascii_analysis_configuration.count_individual_characters = BP_TRUE;

	// create the buffer analysis (counts characters, etc)
	token->value_buff_analysis = BP_CreateBufferAnalysis
	(
		(unsigned char *) buffer,
		(size_t)          buff_len,
		&token->ascii_analysis_configuration
	);

	// we need a buffer analysis to continue
	if(!token->value_buff_analysis)
		return ERR_FAILURE;

	// check for backticks
	if(token->value_buff_analysis->ascii->char_counts['`'])
		token->value_contains_backticks = true;

	// check for embedded single quotes
	if(token->value_buff_analysis->ascii->char_counts['\''])
		token->value_contains_embedded_single_quotes = true;

	// check for embedded double quotes
	if(token->value_buff_analysis->ascii->char_counts['"'])
		token->value_contains_embedded_double_quotes = true;

	// check for nonprintables/extended/control
	if
	(
		token->value_buff_analysis->ascii->ascii_char_flag_counts.extended_n     ||
		token->value_buff_analysis->ascii->ascii_char_flag_counts.nonprintable_n ||
		token->value_buff_analysis->ascii->ascii_char_flag_counts.control_n
	)
	{
		token->value_contains_nonprintables = true;
	}


	// check if the buffer contains only numeric digits
	if(token->value_buff_analysis->ascii->ascii_char_flag_counts.number_n == strnlen((char *) buffer, buff_len))
		token->value_contains_a_positive_number = true;

	// check if it's a negative number
	if(buffer[0] == '-')
	if(token->value_buff_analysis->ascii->ascii_char_flag_counts.number_n == (strnlen((char *) buffer, buff_len)-1))
		token->value_contains_a_negative_number = true;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Basic String Extraction %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // add some forced positive flags
    BP_ASCII_CHAR_FLAGS positive_flags;
    BP_ZERO_STACK(positive_flags);

    // add some forced negative flags
    BP_ASCII_CHAR_FLAGS negative_flags;
    BP_ZERO_STACK(negative_flags);

    // match must be printable character
    positive_flags.is_printable = BP_TRUE;

    // match cannot be extended ascii
    negative_flags.is_extended     = BP_TRUE;
    negative_flags.is_control      = BP_TRUE;
    negative_flags.is_nonprintable = BP_TRUE;

    // Extract all strings from the buffer only using positive
    // and negative match flags.
    BP_ExtractASCIIStringsFromAnalyzedBuffer
    (
    		token->value_buff_analysis,
            &positive_flags,
            BP_ASCII_CHAR_SEARCH_ALL_SEARCH_FLAG_CONDITIONS_MET,
            &negative_flags,
            BP_ASCII_CHAR_SEARCH_ALL_SEARCH_FLAG_CONDITIONS_MET_NEGATIVELY,
            NULL,
            0,
            NULL,
            0,
            8
    );

    // run secondary analysis (tests if things match up with known types, check if
    // files exist, etc.
    BP_RunSecondaryStringAnalysisOnExtractedStrings(token->value_buff_analysis);

	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Warnings / Errors For Individual Tokens %%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// add an error to the individual token (must be ascii/null terminated)
BP_ERROR_T BP_CommandLineOptionParser::addErrorToToken(P_BP_COMMAND_LINE_PARSER_TOKENS token, char *error_msg)
{

	// ensure we have a token, error message, and a tail queue allocator to use
	// for storage.
	if(!token || !error_msg || !this->tq)
		return ERR_FAILURE;


	// add the error message using the tail queue string stacking duplicator
	return BP_ArrayPushDupStringTQ(this->tq, error_msg, &token->token_error_msg, &token->token_error_n);

}

// add a warning to the individual token (must be ascii/null terminated)
BP_ERROR_T BP_CommandLineOptionParser::addWarningToToken(P_BP_COMMAND_LINE_PARSER_TOKENS token, char *warning_msg)
{

	// ensure we have a token, warning message, and tail queue allocator
	// to use for storage.
	if(!token || !warning_msg || !this->tq)
		return ERR_FAILURE;

	// add the error message using the tail queue string stacking duplicator
	return BP_ArrayPushDupStringTQ(this->tq, warning_msg, &token->token_warning_msgs, &token->token_warning_n);

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Visualization / Display Methods %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// display the options as they exist
BP_ERROR_T BP_CommandLineOptionParser::displayOptions()
{

	// check to see if tokens are set
	if(!this->tokens)
		return ERR_FAILURE;
	if(!this->tokens_n)
		return ERR_FAILURE;

	// display simple message
	printf("\n\t [+] Displaying Options in Stack (%p)", this->tokens);

	// walk individual tokens and display argument data
	size_t n = 0;
	for(; n < this->tokens_n; n++)
	{

		// add newline between option
		printf("\n");

		// display the option
		this->displayOption(&this->tokens[n]);

	}

	// display trailing newlines
	printf("\n\n");

	// return indicating success
	return ERR_SUCCESS;

}


// display the options as they exist
BP_ERROR_T BP_CommandLineOptionParser::displayOptionsAsJSON(BP_BOOL display_options_set_from_command_line_only = BP_TRUE, char *extra_data = NULL)
{

	// check to see if tokens are set
	if(!this->tokens)
		return ERR_FAILURE;
	if(!this->tokens_n)
		return ERR_FAILURE;


	// if extra data was passed in, insure it's readable/printable
	if(extra_data)
	if(!BP_StringIsReasonablePrintableString(extra_data, bpstrlen(extra_data)))
		return ERR_FAILURE;

	// ensure the extra data doesn't contain any single quotes
	if(strstr(extra_data, "\'"))
		return ERR_FAILURE;


	// toggle for appending commas
	bool append_comma_cycle = false;

	// walk individual tokens and display argument data
	size_t n = 0;
	for(; n < this->tokens_n; n++)
	{

		// if we are only displaying options set from command line, check to
		// ensure value is set before displaying
		if(display_options_set_from_command_line_only == BP_TRUE)
		{

			// continue loop if value is unset
			if(!this->tokens[n].value_has_been_set_ok)
				continue;

		}

		if(append_comma_cycle == false)
		{


			// display opening bracket but only if we have tokens
			printf("{", this->tokens);

			// add extra data to json output here (but only if we have arguments)
			if(extra_data)
			{
				printf("\n\t\"extra_data\": '%s',", extra_data);
			}

			// set the cycle indicator to true
			append_comma_cycle = true;

		}
		else
		{
			if((n+1) < this->tokens_n) printf(",");
		}

		// display correct types
		switch(this->tokens[n].value_type)
		{

			case BP_COMMAND_LINE_ARGUMENT_STAND_ALONE_TOGGLE:
				printf("\n\t\"%s\": \"true\"", this->tokens[n].token);
				break;

			case BP_COMMAND_LINE_ARGUMENT_CHAR_STRING:
				printf("\n\t\"%s\": \"%s\"", this->tokens[n].token, tokens[n].parsed_value_as_string);
				break;

			case BP_COMMAND_LINE_ARGUMENT_SIZE_T:
				printf("\n\t\"%s\": \"%zu\"", this->tokens[n].token, tokens[n].parsed_value_as_size_t);
				break;

			case BP_COMMAND_LINE_ARGUMENT_SIGNED_INT:
				printf("\n\t\"%s\": \"%i\"", this->tokens[n].token, tokens[n].parsed_value_as_int);
				break;

			case BP_COMMAND_LINE_ARGUMENT_URL:
				printf("\n\t\"%s\": \"%s\"", this->tokens[n].token, tokens[n].parsed_value_as_string);
				break;

			default:
				break;

		}


		// display the option
		// this->displayOption(&this->tokens[n]);

	}

	// close if we have the end of our comma cycle
	if(append_comma_cycle)
	{
		// display closing brace and newline
		printf("\n}");
		printf("\n");
	}

	// return indicating success
	return ERR_SUCCESS;

}


// display a single option
BP_ERROR_T BP_CommandLineOptionParser::displayOption(P_BP_COMMAND_LINE_PARSER_TOKENS option)
{

	// ensure we have an option pointer
	if(!option)
		return ERR_FAILURE;

	// display option header
	printf("\n\t\t [+] Option Token: %s", option->token);
	printf("\n\t\t     token_is_long_form:           %s", option->token_is_long_form ? "yes" : "no");
	printf("\n\t\t     token_required:               %s", option->token_required ? "yes" : "no");
	printf("\n\t\t     buffer analysis:              %p", option->value_buff_analysis);

	// value type string
	char *value_type_string = (char *) NULL;

	// switch to generate string
	switch(option->value_type)
	{

		case BP_COMMAND_LINE_ARGUMENT_UNSET:
			value_type_string="BP_COMMAND_LINE_ARGUMENT_UNSET";
			printf("\n\t\t     value_type:                   %s", value_type_string);
			break;

		case BP_COMMAND_LINE_ARGUMENT_STAND_ALONE_TOGGLE:
			value_type_string="BP_COMMAND_LINE_ARGUMENT_STAND_ALONE_TOGGLE";
			printf("\n\t\t     value_type:                   %s", value_type_string);
			printf("\n\t\t     default_value_as_toggle:      %s", option->default_value_as_toggle ? "BP_TRUE" : "BP_FALSE");
			printf("\n\t\t     value_has_been_set_ok:        %s", option->value_has_been_set_ok ? "yes" : "no");
			printf("\n\t\t     parsed_value_as_toggle:       %u", option->parsed_value_as_toggle);
			break;

		case BP_COMMAND_LINE_ARGUMENT_CHAR_STRING:
			value_type_string="BP_COMMAND_LINE_ARGUMENT_CHAR_STRING";
			printf("\n\t\t     value_type:                   %s", value_type_string);
			printf("\n\t\t     default_value_as_string:      %s", option->default_value_as_string);
			printf("\n\t\t     max_length_for_string:        %u", option->max_length_for_string);
			printf("\n\t\t     min_length_for_string:        %u", option->min_length_for_string);
			printf("\n\t\t     value_has_been_set_ok:        %s", option->value_has_been_set_ok ? "yes" : "no");
			printf("\n\t\t     parsed_value_as_string:       %s", option->parsed_value_as_string);
			break;

		case BP_COMMAND_LINE_ARGUMENT_SIZE_T:
			value_type_string="BP_COMMAND_LINE_ARGUMENT_SIZE_T";
			printf("\n\t\t     value_type:                   %s", value_type_string);
			printf("\n\t\t     default_value_as_size_t:      %u", option->default_value_as_size_t);
			printf("\n\t\t     max_value_as_size_t:          %u", option->max_value_as_size_t);
			printf("\n\t\t     min_value_as_size_t:          %u", option->min_value_as_size_t);
			printf("\n\t\t     value_has_been_set_ok:        %s", option->value_has_been_set_ok ? "yes" : "no");
			printf("\n\t\t     parsed_value_as_size_t:       %u", option->parsed_value_as_size_t);
			break;

		case BP_COMMAND_LINE_ARGUMENT_SIGNED_INT:
			value_type_string="BP_COMMAND_LINE_ARGUMENT_SIGNED_INT";
			printf("\n\t\t     value_type:                   %s", value_type_string);
			printf("\n\t\t     default_value_as_int:         %i", option->default_value_as_int);
			printf("\n\t\t     max_value_as_integer:         %i", option->max_value_as_integer);
			printf("\n\t\t     min_value_as_integer:         %i", option->min_value_as_integer);
			printf("\n\t\t     value_has_been_set_ok:        %s", option->value_has_been_set_ok ? "yes" : "no");
			printf("\n\t\t     parsed_value_as_int:          %i", option->parsed_value_as_int);
			break;

		case BP_COMMAND_LINE_ARGUMENT_URL:
			value_type_string="BP_COMMAND_LINE_ARGUMENT_URL";
			printf("\n\t\t     value_type:                   %s", value_type_string);
			printf("\n\t\t     value_has_been_set_ok:        %s", option->value_has_been_set_ok ? "yes" : "no");
			printf("\n\t\t     default_value_as_string:      %s", option->default_value_as_string);
			printf("\n\t\t     max_length_for_string:        %u", option->max_length_for_string);
			printf("\n\t\t     min_length_for_string:        %u", option->min_length_for_string);
			printf("\n\t\t     value_has_been_set_ok:        %s", option->value_has_been_set_ok ? "yes" : "no");
			printf("\n\t\t     parsed_value_as_string:       %s", option->parsed_value_as_string);
			break;

		default:
			value_type_string="BAD TYPE";
			printf("\n\t\t     value_type:                   %s", value_type_string);
			break;

	}

	// argument quote status
	if(option->value_parse_quote_check)
	{

		// generate and display the quote string
		char *quote_str_val = (char *) BP_QuoteCheckResultToStringValue(option->value_parse_quote_check);

		// display/destroy the quote string value here if necessary
		if(quote_str_val)
		{

			// display
			printf("\n\t\t     value_parse_quote_check:      %s", quote_str_val);

			// subsequent destruction
			bpfree(quote_str_val);

		}


	}


	// argument callback settings
	printf("\n\t\t     argument_callback_enabled:    %s", option->argument_callback_enabled ? "yes": "no");
	printf("\n\t\t     argument_activation_callback: %p", option->argument_activation_callback);
	printf("\n\t\t     callback_return_value:        %u", option->callback_return_value);

	// display description
	if(option->token_description)
		printf("\n\t\t     token_description:            %s", option->token_description);

	// display usage
	if(option->token_usage_example)
		printf("\n\t\t     token_usage_example:          %s", option->token_usage_example);


	// display perl compatible regular expressions here if set
	if(option->preg_filters)
	{

		// display filters
		printf("\n\t\t      [+] PREG Filters: ");

	}


	// display errors if necessary
	if(option->token_error_msg)
	{

		// display error banner
		printf("\n\t\t      [+] Errors: ");

		// display errors themselves
		BP_DisplayStringArray(" (err):  ", 1, 3, option->token_error_msg);

	}

	// display warnings if necessary
	if(option->token_warning_msgs)
	{
		// display error banner
		printf("\n\t\t      [+] Warnings: ");

		// display errors themselves
		BP_DisplayStringArray(" (warn): ", 1, 3, option->token_error_msg);

	}


	// return indicating success
	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Clone Routine (For Duplicating Between Threaded Target Selectors %%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Cloning creates a rough duplicate of this object, useful for passing onto threads.  This is
// useful for omitting concurrency problems which might arise due to sharing threaded data.  This
// prevents the need to lock data between threads, which is a major boon for concurrent optimization.

// returns a new command line option parser with duplicated values.
BP_CommandLineOptionParser * BP_CommandLineOptionParser::clone()
{

	// create a new command line option parser
	BP_CommandLineOptionParser * ret_clone = new BP_CommandLineOptionParser();
	if(!ret_clone)
		return NULL;

	// set the token count
	ret_clone->tokens_n = this->tokens_n;

	// clone the tokens overall structure
	ret_clone->tokens = (P_BP_COMMAND_LINE_PARSER_TOKENS) tq_calloc(ret_clone->tq, sizeof(BP_COMMAND_LINE_PARSER_TOKENS) * ret_clone->tokens_n, 1, BPLN);

	// create pointers for doing to->from copying
	P_BP_COMMAND_LINE_PARSER_TOKENS clone_token_to   = NULL;
	P_BP_COMMAND_LINE_PARSER_TOKENS clone_token_from = NULL;

	// now copy in basic token data.  The rest of the token information will
	// be filled in by calling ->parseOptions() on the local data.
	size_t n = 0;
	for(; n < ret_clone->tokens_n; n++)
	{

		// set the clone to/from values
		clone_token_from = &this->tokens[n];
		clone_token_to   = &ret_clone->tokens[n];

		// order will always be:
		// clone_token_to <- clone_token_from

		// set token string
		clone_token_to->token = tq_strdup(ret_clone->tq, clone_token_from->token, BPLN);

		// set token length
		clone_token_to->token_length = clone_token_from->token_length;

		// set long form flag
		clone_token_to->token_is_long_form = clone_token_from->token_is_long_form;

		// set required flag
		clone_token_to->token_required = clone_token_from->token_required;

		// set value type
		clone_token_to->value_type = clone_token_from->value_type;

		// set token description
		clone_token_to->token_description = tq_strdup(ret_clone->tq, clone_token_from->token_description, BPLN);

		// set token usage example
		clone_token_to->token_usage_example = tq_strdup(ret_clone->tq, clone_token_from->token_usage_example, BPLN);

		// set default value as toggle
		clone_token_to->default_value_as_toggle = clone_token_from->default_value_as_toggle;

		// set default value as string
		clone_token_to->default_value_as_string = tq_strdup(ret_clone->tq, clone_token_from->default_value_as_string, BPLN);

		// set max length for string
		clone_token_to->max_length_for_string = clone_token_from->max_length_for_string;

		// set min length for string
		clone_token_to->min_length_for_string = clone_token_from->min_length_for_string;

		// set default value as size_t
		clone_token_to->default_value_as_size_t = clone_token_from->default_value_as_size_t;

		// set max value as size_t
		clone_token_to->max_value_as_size_t = clone_token_from->max_value_as_size_t;

		// set min value as size_t
		clone_token_to->min_value_as_size_t = clone_token_from->min_value_as_size_t;


	}

	// parse the options at this point
	ret_clone->parseOptions(this->argc, this->argv, this->envp, this->envpc);

	// return the cloned option parser
	return ret_clone;

}

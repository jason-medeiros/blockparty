#include "../../../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Constructor / Deconstructor %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// class constructor
BP_ApplicationHelpGenerator::BP_ApplicationHelpGenerator(BP_CommandLineOptionParser * clo_parser)
{

	// create a new tailqueue allocator for use in this class
	this->tq = BP_CreateLinkL("application help tq", BP_FALSE);

	// store the command line option parser here if necessary
	if(clo_parser)
		this->clop = clo_parser;
	else
		this->clop = NULL;

	// retrieve terminal information
	this->terminal_info = BP_CreateCurrentTerminalInformationLinux();

	// set groups to null
	this->groups  = NULL;
	this->group_n = 0;

	// nullify name and description
	this->app_name        = NULL;
	this->app_description = NULL;

	// create new string formatting session
	this->sfsc = new BP_StringFormattingSession("app help");


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Build Information %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this is the default build time and date, set to the time date
	// of the blockparty library.  You can override this in your own
	// application if desired.

	this->app_build_time = __TIME__;
	this->app_build_date = __DATE__;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Initialize Usage Examples %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set usage examples
	this->usage_examples  = NULL;

	// set example counts
	this->usage_example_n = 0;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Initialize Border Sets %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	this->app_info_border_set                  = NULL;
	this->aa_company_name_big_art_border_set   = NULL;
	this->aa_company_name_small_art_border_set = NULL;
	this->aa_header_art_border_set             = NULL;
	this->aa_body_art_border_set               = NULL;
	this->aa_footer_art_border_set             = NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Initialize All Default Colors %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	BP_ZERO_STACK(this->aa_company_name_big_art_colors);
	BP_ZERO_STACK(this->aa_company_name_small_art_colors);
	BP_ZERO_STACK(this->aa_header_art_colors);
	BP_ZERO_STACK(this->aa_body_art_colors);
	BP_ZERO_STACK(this->aa_footer_art_colors);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Initialize Art Stacks %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// initialize company name big art
	this->aa_company_name_big_art = NULL;
	this->aa_company_name_big_art_n = 0;
	this->aa_company_name_big_art_randomize = BP_FALSE;

	// initialize company name small art
	this->aa_company_name_small_art = NULL;
	this->aa_company_name_small_art_n = 0;
	this->aa_company_name_small_art_randomize = BP_FALSE;

	// initialize header art data
	this->aa_header_art = NULL;
	this->aa_header_art_n = 0;
	this->aa_header_art_randomize = BP_FALSE;

	// initialize body art data
	this->aa_body_art = NULL;
	this->aa_body_art_n = 0;
	this->aa_body_art_randomize = BP_FALSE;

	// initialize footer art data
	this->aa_footer_art = NULL;
	this->aa_footer_art_n = 0;
	this->aa_footer_art_randomize = BP_FALSE;

	// zero out the version info
	BP_ZERO_STACK(this->app_version);

	// init the lock
	sem_init(&this->locked, 1, 1);

}

// class deconstructor
BP_ApplicationHelpGenerator::~BP_ApplicationHelpGenerator()
{

	// destroy terminal info if necessary
	if(this->terminal_info)
		BP_DestroyTerminalInformationLinux(this->terminal_info);

	// destroy the string formatting session if it exists
	if(this->sfsc)
		delete this->sfsc;

	// destroy the clop if set
	if(this->clop)
		delete this->clop;

	// destroy the allocated data if set
	if(this->tq)
	{

		// destroy the linkl
		BP_DestroyLinkL(this->tq, BP_TRUE);

		// nullify the tail queue
		this->tq = NULL;

	}

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Thread Locking %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// lock
void BP_ApplicationHelpGenerator::lock()
{
	sem_wait(&this->locked);
}

// unlock
void BP_ApplicationHelpGenerator::unlock()
{
	sem_post(&this->locked);
}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% ASCII Art Entries %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


BP_ERROR_T BP_ApplicationHelpGenerator::addCompanyNameArt(char * art, BP_BOOL large_or_small)
{

	if(!art)
		return ERR_FAILURE;

	if(large_or_small == BP_TRUE)
	{

		// allocate space for art entry + new terminator
		this->aa_company_name_big_art = (char **) tq_realloc(this->tq, this->aa_company_name_big_art, sizeof(char *) * (this->aa_company_name_big_art_n+2), BPLN);

		// ensure we have art set, if not return null
		if(!this->aa_company_name_big_art)
			return ERR_FAILURE;

		// set the art
		this->aa_company_name_big_art[this->aa_company_name_big_art_n] = tq_strdup(this->tq, art, BPLN);

		// terminate the array
		this->aa_company_name_big_art[this->aa_company_name_big_art_n+1] = NULL;

		// increment element count
		this->aa_company_name_big_art_n++;


	}
	else
	{

		// allocate space for art entry + new terminator
		this->aa_company_name_small_art = (char **) tq_realloc(this->tq, this->aa_company_name_small_art, sizeof(char *) * (this->aa_company_name_small_art_n+2), BPLN);

		// ensure we have art set, if not return null
		if(!this->aa_company_name_small_art)
				return ERR_FAILURE;

		// set the art
		this->aa_company_name_small_art[this->aa_company_name_small_art_n] = tq_strdup(this->tq, art, BPLN);

		// terminate the array
		this->aa_company_name_small_art[this->aa_company_name_small_art_n+1] = NULL;

		// increment element count
		this->aa_company_name_small_art_n++;


	}


	// return indicating success
	return ERR_SUCCESS;
}

BP_ERROR_T BP_ApplicationHelpGenerator::addHeaderArt(char * art)
{

	// ensure we have an art pointer
	if(!art)
		return ERR_FAILURE;

    // allocate space for art entry + new terminator
    this->aa_header_art = (char **) tq_realloc(this->tq, this->aa_header_art, sizeof(char *) * (this->aa_header_art_n+2), BPLN);

    // ensure we have art set, if not return null
    if(!this->aa_header_art)
    	return ERR_FAILURE;

    // set the art
    this->aa_header_art[this->aa_header_art_n] = tq_strdup(this->tq, art, BPLN);

    // terminate the array
    this->aa_header_art[this->aa_header_art_n+1] = NULL;

    // increment element count
    this->aa_header_art_n++;

	// return indicating success
	return ERR_SUCCESS;

}

BP_ERROR_T BP_ApplicationHelpGenerator::addBodyArt(char * art)
{

	if(!art)
		return ERR_FAILURE;

    // allocate space for art entry + new terminator
    this->aa_body_art = (char **) tq_realloc(this->tq, this->aa_body_art, sizeof(char *) * (this->aa_body_art_n+2), BPLN);

    // ensure we have art set, if not return null
    if(!this->aa_body_art)
    	return ERR_FAILURE;

    // set the art
    this->aa_body_art[this->aa_body_art_n] = tq_strdup(this->tq, art, BPLN);

    // terminate the array
    this->aa_body_art[this->aa_body_art_n+1] = NULL;

    // increment element count
    this->aa_body_art_n++;

	// return indicating success
	return ERR_SUCCESS;

}

BP_ERROR_T BP_ApplicationHelpGenerator::addFooterArt(char * art)
{

	if(!art)
		return ERR_FAILURE;

    // allocate space for art entry + new terminator
    this->aa_footer_art = (char **) tq_realloc(this->tq, this->aa_footer_art, sizeof(char *) * (this->aa_footer_art_n+2), BPLN);

    // ensure we have art set, if not return null
    if(!this->aa_footer_art)
    	return ERR_FAILURE;

    // set the art
    this->aa_footer_art[this->aa_footer_art_n] = tq_strdup(this->tq, art, BPLN);

    // terminate the array
    this->aa_footer_art[this->aa_footer_art_n+1] = NULL;

    // increment element count
    this->aa_footer_art_n++;

	// return indicating success
	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Generated Groups/Entries/Etc %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// add an option group
BP_ERROR_T BP_ApplicationHelpGenerator::addOptionGroup
(
	char * group_name,
	char * group_description,
	P_BP_STRING_ROW_BORDER_SET border_set
)
{

	// ensure we have a group name and description
	if(!group_name || !group_description)
		return ERR_FAILURE;

	// reallocate space for group
	this->groups = (P_BP_AHG_OPTION_GROUP) tq_realloc
	(
		this->tq,
		this->groups,
		sizeof(BP_AHG_OPTION_GROUP) * (this->group_n+1),
		BPLN
	);

	// zero out the new memory
	memset(&this->groups[this->group_n], 0x00, sizeof(BP_AHG_OPTION_GROUP));

	// set group name and description
	this->groups[this->group_n].group_name        = tq_strdup(this->tq, group_name,        BPLN);
	this->groups[this->group_n].group_description = tq_strdup(this->tq, group_description, BPLN);

	// set border-set if one was passed in
	if(border_set)
		this->groups[this->group_n].border_set = border_set;

	// increment the group count
	this->group_n++;

	// return indicating success
	return ERR_SUCCESS;

}



// Add an entry to the option group via the local command line parser.  Works
// by looking up the token in the command line parser class, and assigns it
// to the specified group.
BP_ERROR_T BP_ApplicationHelpGenerator::addOptionEntryToOptionGroup
(
		char * group_name,
		char * new_entry_name,
		char * new_entry_description,
		char * token,
		P_BP_STRING_ROW_BORDER_SET border_set
)
{

	// ensure that we have command line option parser set
	if(!this->clop)
		return ERR_FAILURE;

	// ensure parameters are set
	if(!group_name || !new_entry_name || !new_entry_description || !token)
		return ERR_FAILURE;


	// set the group to work with
	P_BP_AHG_OPTION_GROUP group = this->lookupOptionGroupByName(group_name);
	if(!group)
		return ERR_FAILURE;

	// attempt to lookup the token
	P_BP_COMMAND_LINE_PARSER_TOKENS token_ptr = this->clop->optionLookup(token);
	if(!token_ptr)
	{
		return ERR_FAILURE;
	}

	// set entries here
	group->entries = (P_BP_AHG_OPTION_ENTRY) tq_realloc
	(
			this->tq,
			group->entries,
			sizeof(BP_AHG_OPTION_ENTRY) * (group->entry_n+1),
			BPLN
	);

	// ensure memory was allocated
	if(!group->entries)
		return ERR_FAILURE;

	// zero out memory
	memset(&group->entries[group->entry_n], 0x00, sizeof(BP_AHG_OPTION_ENTRY));

	// set name and description
	group->entries[group->entry_n].name        = tq_strdup(this->tq, new_entry_name,        BPLN);
	group->entries[group->entry_n].description = tq_strdup(this->tq, new_entry_description, BPLN);

	// set token
	group->entries[group->entry_n].token = token_ptr;

	// set border set if one was passed in
	if(border_set)
		group->entries[group->entry_n].border_set = border_set;

	// increase entry count
	group->entry_n++;

	// return indicating success
	return ERR_SUCCESS;

}

// Add a dependency to a option entry (within an option group)
BP_ERROR_T BP_ApplicationHelpGenerator::addOptionDependencyToOptionEntry
(
	char * group_name,
	char * entry_name,
	char * dependency_group_name,
	char * dependency_name,
	BP_AHG_ENTRY_DEPENDENCY_VALUES depends_how
)
{

	// Check if the dependency already exists.
	if(this->checkIfDependenciesExist(group_name,entry_name, dependency_group_name, dependency_name, depends_how) == ERR_SUCCESS)
		return ERR_FAILURE;

	// ensure we have parameters
	if(!group_name || !entry_name)
		return ERR_FAILURE;

	// ensure we have groups to search
	if(!this->groups || !this->group_n)
		return ERR_FAILURE;

	// attempt to look up the group
	P_BP_AHG_OPTION_GROUP group = this->lookupOptionGroupByName(group_name);
	if(!group)
		return ERR_FAILURE;

	// attempt to look up the group
	P_BP_AHG_OPTION_GROUP dep_group = this->lookupOptionGroupByName(dependency_group_name);
	if(!dep_group)
		return ERR_FAILURE;

	// lookup entry
	P_BP_AHG_OPTION_ENTRY entry     = this->lookupEntryInOptionGroup(group , entry_name);
	if(!entry)
		return ERR_FAILURE;

	// lookup dep entry
	P_BP_AHG_OPTION_ENTRY dep_entry = this->lookupEntryInOptionGroup(dep_group , dependency_name);
	if(!dep_entry)
		return ERR_FAILURE;


	// At this point we've gotten all entries and dependency values.
	entry->dependencies = (P_BP_AHG_ENTRY_DEPENDENCY) tq_realloc
	(
		this->tq,
		entry->dependencies,
		sizeof(BP_AHG_ENTRY_DEPENDENCY) * (entry->dependency_n + 1),
		BPLN
	);

	// ensure we could reallocate data
	if(!entry->dependencies)
		return ERR_FAILURE;

	// zero out entry
	memset(&entry->dependencies[entry->dependency_n], 0x00, sizeof(BP_AHG_ENTRY_DEPENDENCY));

	// set dependency info
	entry->dependencies[entry->dependency_n].depends_how = depends_how;
	entry->dependencies[entry->dependency_n].entry       = dep_entry;
	entry->dependencies[entry->dependency_n].group       = dep_group;

	// increment the dependency count for the entry
	entry->dependency_n++;

	// return indicating success
	return ERR_SUCCESS;

}

// simply checks to see that dependencies exist or not
BP_ERROR_T BP_ApplicationHelpGenerator::checkIfDependenciesExist
(
	char * group_name,
	char * entry_name,
	char * dependency_group_name,
	char * dependency_name,
	BP_AHG_ENTRY_DEPENDENCY_VALUES depends_how
)
{

	// ensure we have parameters
	if(!group_name || !entry_name)
		return ERR_FAILURE;

	// ensure we have groups to search
	if(!this->groups || !this->group_n)
		return ERR_FAILURE;

	// attempt to look up the group
	P_BP_AHG_OPTION_GROUP group = this->lookupOptionGroupByName(group_name);
	if(!group)
		return ERR_FAILURE;

	// attempt to look up the group
	P_BP_AHG_OPTION_GROUP dep_group = this->lookupOptionGroupByName(dependency_group_name);
	if(!dep_group)
		return ERR_FAILURE;

	// lookup entry
	P_BP_AHG_OPTION_ENTRY entry     = this->lookupEntryInOptionGroup(group , entry_name);
	if(!entry)
		return ERR_FAILURE;

	// ensure we have dependencies to search through
	if(!entry->dependency_n || !entry->dependencies)
		return ERR_FAILURE;

	// lookup dep entry to make sure it also exists
	P_BP_AHG_OPTION_ENTRY dep_entry = this->lookupEntryInOptionGroup(dep_group , dependency_name);
	if(!dep_entry)
		return ERR_FAILURE;

	// pointer for holding the current dependency
	P_BP_AHG_ENTRY_DEPENDENCY curr_dep = NULL;

	// now ensure that the dependency exists in the entry
	size_t n = 0;
	for(; n < entry->dependency_n; n++)
	{

		// set the current dependency
		curr_dep = &entry->dependencies[n];

		// The dependency has to be a pointer EXACT match.  If
		// it matches, return indicating that we have a dependency match.
		if(curr_dep->entry == dep_entry)
		{
			if(depends_how != BP_AHG_ENTRY_DEPENDENCY_NONE)
			if(depends_how != curr_dep->depends_how)
				continue;

			// return indicating success if all check are ok
			return ERR_SUCCESS;

		}

	}

	// return indicating failure if we have no match
	return ERR_FAILURE;

}



// attempts to lookup an entry
P_BP_AHG_OPTION_ENTRY BP_ApplicationHelpGenerator::lookupEntryInOptionGroup(P_BP_AHG_OPTION_GROUP group , char *entry_name)
{

	// check parameters first
	if(!group || !entry_name)
		return NULL;

	// ensure we have groups to search
	if(!this->groups || !this->group_n)
		return NULL;

	// ensure we have entries to search
	if(!group->entries || !group->entry_n)
		return NULL;

	// current entry
	P_BP_AHG_OPTION_ENTRY current_entry = NULL;

	// set search len
	size_t search_len = bpstrlen(entry_name);

	// search group entries
	size_t n = 0;
	for(; n < group->entry_n; n++)
	{

		// set current entry from indexed value
		current_entry = &group->entries[n];

		// calculate the current entry length
		size_t entry_len = bpstrlen(current_entry->name);

		// lengths have to match
		if(entry_len != search_len)
			continue;

		// see if we have a match, return the match if so
		if(memcmp(current_entry->name, entry_name, search_len) == 0)
			return current_entry;

	}

	// return null on failure
	return NULL;

}


// Lookup an option group by name.
P_BP_AHG_OPTION_GROUP BP_ApplicationHelpGenerator::lookupOptionGroupByName(char * group_name)
{

	// ensure we have a group name
	if(!group_name)
		return NULL;

	// ensure we have groups to search
	if(!this->groups || !this->group_n)
		return NULL;

	// group name
	size_t group_name_len = bpstrlen(group_name);

	// group search length
	size_t group_search_len = 0;

	size_t n = 0;
	for(; n < this->group_n; n++)
	{

		// calculate group name len
		group_search_len = bpstrlen(this->groups[n].group_name);

		// ensure that the search length matches the name len
		if(group_search_len != group_name_len)
			continue;

		// compare name and return the found group if match
		if(memcmp(this->groups[n].group_name, group_name, group_search_len) == 0)
			return &this->groups[n];

	}

	// exit here if we can't find any group to lookup
	return NULL;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Last Minute Parameter Checks (Verifies Vars Are Set OK) %%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Walks the entries in the local command line option parser (this->clop) and verifies
// that all required arguments have been set, if not, it calculates which arguments
// arent set correctly, and why.
BP_ERROR_T BP_ApplicationHelpGenerator::checkThatAllRequiredParametersAreSet()
{

	// if we have no command line option parser, we can't verify
	// anything, so exit with failure code.
	if(!this->clop)
		return ERR_FAILURE;

	// if we have no tokens, we can assumen that the developer has set
	// none, so exit with success code here.
	if(!this->clop->tokens)
		return ERR_SUCCESS;



	// the command line option parser has the ability to check for mandatory
	// entries; run that check here.
	if(this->clop->runPostParseTokenChecks() != ERR_SUCCESS)
	{
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}

// This walks the options and checks to see that all dependencies are met ok.
BP_ERROR_T BP_ApplicationHelpGenerator::checkThatAllMandatoryDependenciesAreMet()
{

	// Ensure we have groups and group count.  If they're not set, we can
	// return indicating success, as we can't have mandatory dependencies
	// if we simply have no dependencies at all.
	if(!this->groups)
		return ERR_SUCCESS;
	if(!this->group_n)
		return ERR_SUCCESS;

	// current entry and current group
	P_BP_AHG_OPTION_ENTRY current_entry = NULL;
	P_BP_AHG_OPTION_GROUP current_group = NULL;

	// walk group count
	size_t n = 0;
	for(; n < this->group_n; n++)
	{

		// set current group
		current_group = &this->groups[n];

		// walk individual entries and check dependencies
		size_t j = 0;
		for(j = 0; j < current_group->entry_n; j++)
		{

			// set the current entry
			current_entry = &current_group->entries[j];

			// if our current entry is required, and it hasn't been set ok, we have encountered
			// bad options (aka values which were required haven't been set by the user)
			if(!current_entry->token->value_has_been_set_ok && current_entry->token->token_required)
			{
				// printf("\n Failing here?!: %s", current_entry->token->token);
				return ERR_FAILURE;
			}

			// check deps
			size_t i = 0;
			for(i = 0; i < current_entry->dependency_n; i++)
			{

				// now check individual mandatory dependencies
				if(current_entry->dependencies[i].depends_how == BP_AHG_ENTRY_DEPENDENCY_MANDATORY)
				if(current_entry->dependencies[i].entry)
				if(!current_entry->dependencies[i].entry->token->value_has_been_set_ok)
					return ERR_FAILURE;



			}


		}


	}


	// return indicating success
	return ERR_SUCCESS;

}

// This method combines the checkThatAllRequiredParametersAreSet, checkThatAllMandatoryDependenciesAreMet,
// and the option_parser->optionLookup("--help") methods to see if the application is prepared and ready
// to run.
BP_ERROR_T BP_ApplicationHelpGenerator::finalApplicationErrorCheck()
{

	// ensure we have a command line option parser
	if(!this->clop)
		return ERR_FAILURE;

	// ensure that all required parameters have been set ok
	if(this->checkThatAllRequiredParametersAreSet() != ERR_SUCCESS)
		return ERR_FAILURE;

	// within all the set options, ensure that dependencies are set
	if(this->checkThatAllMandatoryDependenciesAreMet() != ERR_SUCCESS)
		return ERR_FAILURE;

	// check to see that the --help option hasn't been set (if it's available
	// for the current program)
	if(this->clop->optionLookup("--help"))
	if(this->clop->optionLookup("--help")->value_has_been_set_ok)
		return ERR_FAILURE;;

	// return indicating success (after all the checks have been passed)
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Usage Examples %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// adds a usage example to the local class
BP_ERROR_T BP_ApplicationHelpGenerator::addUsageExample(char *name, char * description, char *usage_string)
{

	// ensure we have parameters
	if(!name || !description || !usage_string)
		return ERR_FAILURE;

	// attempt to allocate memory
	this->usage_examples = (P_BP_AHG_USAGE_EXAMPLE) tq_realloc(this->tq, this->usage_examples, sizeof(BP_AHG_USAGE_EXAMPLE) * (this->usage_example_n+1), BPLN);

	// ensure the memory was allocated ok
	if(!this->usage_examples)
		return ERR_FAILURE;

	// zero out memory
	memset(&this->usage_examples[this->usage_example_n], 0x00, sizeof(BP_AHG_USAGE_EXAMPLE));

	// set values
	this->usage_examples[this->usage_example_n].name         = tq_strdup(this->tq, name, BPLN);
	this->usage_examples[this->usage_example_n].description  = tq_strdup(this->tq, description, BPLN);
	this->usage_examples[this->usage_example_n].usage_string = tq_strdup(this->tq, usage_string, BPLN);

	// increment count
	this->usage_example_n++;

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Settrs: Set Various Information Regarding the Application %%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// set basic information
BP_ERROR_T BP_ApplicationHelpGenerator::setBasicInformation(char *app_name, char *app_description, char * app_header_art, P_BP_AHG_APP_VERSION_INFO version_info)
{

	// set the app name if desired
	if(app_name)
	{

		// destroy the app name if necessary
		if(this->app_name)
			tq_free(this->tq, this->app_name);

		// set the app name here if desired
		this->app_name = tq_strndup(this->tq, app_name, BP_KILOBYTE, BPLN);

	}

	// set the description if desired
	if(app_description)
	{

		// destroy the description if necessary
		if(this->app_description)
			tq_free(this->tq, this->app_description);

		// set the app description if desired
		this->app_description = tq_strndup(this->tq, app_description, BP_KILOBYTE, BPLN);

	}

	// set version information if desired
	if(version_info)
	{

		// set build indicator
		this->app_version.build_indicator = version_info->build_indicator;

		// set major version
		this->app_version.major_version   = version_info->major_version;

		// set minor version
		this->app_version.minor_version   = version_info->minor_version;

	}

	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Final Generator (Builds Everything) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This routine calls all the individual build routines.
BP_ERROR_T BP_ApplicationHelpGenerator::buildApplicationHelp()
{

	// build the application header (if set)
	this->buildApplicationHeader();

	// builld the name and desc table
	this->buildApplicationNameAndDescriptionTable();

	// build the application version table
	this->buildApplicationVersionTable();

	// build application group output (if set)
	this->buildApplicationGroupEntryOptions();

	// build usage examples (if set)
	this->buildApplicationUsageExamples();

	// build application footer (if set)
	this->buildApplicationFooter();


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Build The Error Table if Deps are Unset  %%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// generate the error table
	if(this->clop->tokens_n && (this->clop->argc > 1))
	{

		// ensure mandator deps are met
		if(this->checkThatAllMandatoryDependenciesAreMet() != ERR_SUCCESS)
		{

			// build the application error tables
			this->buildApplicationErrorTables();

		}

	}


	// sfsc->displaySFSSession();

	// return indicating success
	return ERR_SUCCESS;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Individual Build Generators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Build the application header.
BP_ERROR_T BP_ApplicationHelpGenerator::buildApplicationHeader()
{

	if(!this->sfsc)
		return ERR_FAILURE;

	// company art selections (assigned in the picking logic below)
	char * company_art_selected_big   = NULL;
	char * company_art_selected_small = NULL;

	// header body and footer art selections
	char * header_art_selected = NULL;
	char * body_art_selected   = NULL;


	// random index used for assigning elements
	size_t random_idx = 0;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Element Picking Logic %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Pick the company name art (big)
	if(this->aa_company_name_big_art && this->aa_company_name_big_art_n)
	{
		if(this->aa_company_name_big_art_randomize)
		{
			random_idx = BP_GenRandomUnsignedLong(0, this->aa_company_name_big_art_n-1);
			company_art_selected_big = this->aa_company_name_big_art[random_idx];
		}
		else
		{
			company_art_selected_big = this->aa_company_name_big_art[0];
		}
	}

	// pick the small company name art to use
    if(this->aa_company_name_small_art && this->aa_company_name_small_art_n)
    {
		if(this->aa_company_name_small_art_randomize)
		{
				random_idx = BP_GenRandomUnsignedLong(0, this->aa_company_name_small_art_n-1);
				company_art_selected_small = this->aa_company_name_small_art[random_idx];
		}
		else
		{
				company_art_selected_small = this->aa_company_name_small_art[0];
		}
    }

    // Pick the header art to use
    if(this->aa_header_art && this->aa_header_art_n)
    {
		if(this->aa_header_art_randomize)
		{
				random_idx = BP_GenRandomUnsignedLong(0, this->aa_header_art_n-1);
				header_art_selected = this->aa_header_art[random_idx];
		}
		else
		{
				header_art_selected = this->aa_header_art[0];
		}
    }

    // Pick the body art to use
    if(this->aa_body_art && this->aa_body_art_n)
    {
		if(this->aa_body_art_randomize)
		{
				random_idx = BP_GenRandomUnsignedLong(0, this->aa_body_art_n-1);
				body_art_selected = this->aa_body_art[random_idx];
		}
		else
		{
				body_art_selected = this->aa_body_art[0];
		}
    }


    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Build Header Using Selected Elements %%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if(company_art_selected_big)
	{

	    sfsc->addRowGroup   ("company_art_selected_big");
	    sfsc->addRow        ("row1");
	    sfsc->addColumnGroup("colgroup1");
	    sfsc->addColumn     ("column1");
	    sfsc->addStringEntry
	    (
	    		"company_art_selected_big",
	    		company_art_selected_big,
	    		bpstrlen(company_art_selected_big),
	    		0,
	    		&this->aa_company_name_big_art_colors,
	    		BP_TRUE,
	    		0,
	    		0,
	    		this->aa_company_name_big_art_border_set
	    );

	}

	if(company_art_selected_small)
	{

	    sfsc->addRowGroup   ("company_art_selected_small");
	    sfsc->addRow        ("row1");
	    sfsc->addColumnGroup("colgroup1");
	    sfsc->addColumn     ("column1");
	    sfsc->addStringEntry
	    (
	    		"company_art_selected_small",
	    		company_art_selected_small,
	    		bpstrlen(company_art_selected_small),
	    		0,
	    		&this->aa_company_name_small_art_colors,
	    		BP_TRUE,
	    		0,
	    		0,
	    		this->aa_company_name_small_art_border_set
	    );

	}


	if(header_art_selected)
	{
	    sfsc->addRowGroup   ("header_art_selected");
	    sfsc->addRow        ("row1");
	    sfsc->addColumnGroup("colgroup1");
	    sfsc->addColumn     ("column1");
	    sfsc->addStringEntry
	    (
	    		"header_art_selected",
	    		header_art_selected,
	    		bpstrlen(header_art_selected),
	    		0,
	    		&this->aa_header_art_colors,
	    		BP_TRUE,
	    		0,
	    		0,
	    		this->aa_header_art_border_set
	    );
	}

	if(body_art_selected)
	{

		sfsc->addRowGroup   ("body_art_selected");
		sfsc->addRow        ("row1");
		sfsc->addColumnGroup("colgroup1");
		sfsc->addColumn     ("column1");
		sfsc->addStringEntry
		(
				"body_art_selected",
				body_art_selected,
				bpstrlen(body_art_selected),
				0,
				&this->aa_body_art_colors,
				BP_TRUE,
				0,
				0,
				this->aa_body_art_border_set
		);

	}






	// return indicating success
	return ERR_SUCCESS;

}


// Builds the build version table.
BP_ERROR_T BP_ApplicationHelpGenerator::buildApplicationVersionTable()
{

	if(!this->sfsc)
		return ERR_FAILURE;

	// create version string and zero it out
	char version_string[BP_KILOBYTE];
	BP_ZERO_STACK(version_string);

	// set the app version string
	sprintf(version_string, "%zu.%zu.%zu", this->app_version.major_version, this->app_version.minor_version, this->app_version.build_indicator);

	// create room for blockparty version string
	char blockparty_version_string[BP_KILOBYTE];
	BP_ZERO_STACK(blockparty_version_string);

	// set the blockparty build indicator
	sprintf(blockparty_version_string, "%zu.%zu.%zu", BP_LIBRARY_MAJOR_VERSION, BP_LIBRARY_MINOR_VERSION, BP_LIBRARY_BUILD_ID);

	// create a string stack we can join up easily
	char *str_stack_to_join[8];
	BP_ZERO_STACK(str_stack_to_join);

	// fill in the blanks
	size_t idx=0;
	str_stack_to_join[idx] = (char *) "App Version:        "; idx++;
	str_stack_to_join[idx] = (char *)  version_string; idx++;
	str_stack_to_join[idx] = "\n"; idx++;
	str_stack_to_join[idx] = (char *) "Blockparty Version: "; idx++;
	str_stack_to_join[idx] = (char *) blockparty_version_string; idx++;



	// join the string array into a single string
	char * joined_string = BP_JoinStringArray(str_stack_to_join, idx);
	if(!joined_string)
	{
		return ERR_FAILURE;
	}


	BP_COLOR_CODE_SET_T tmp_version_table_colors;
	tmp_version_table_colors.fg = BP_COLOR_CODE_FG_CYAN;
	tmp_version_table_colors.bg = BP_COLOR_CODE_BG_NONE;
	sfsc->addRowGroup();
	sfsc->addRow        ();
	sfsc->addColumnGroup();
	sfsc->addColumn     ();
	sfsc->addStringEntry
	(
			"version table",
			joined_string,
			bpstrlen(joined_string),
			0,
			&tmp_version_table_colors,
			BP_TRUE,
			1,
			0,
			&BP_STSBS_border_set_onlypadding_no_bottom_border_1
	);
	sfsc->addColumn     ();
	tmp_version_table_colors.fg = BP_COLOR_CODE_FG_GREEN;
	tmp_version_table_colors.bg = BP_COLOR_CODE_BG_NONE;
	sfsc->addStringEntry
	(
			"app build date spacer",
			"Built On:",
			bpstrlen("Built On:"),
			bpstrlen("Built On:"),
			&tmp_version_table_colors,
			BP_FALSE,
			0,
			0,
			&BP_STSBS_border_set_onlypadding_no_bottom_border_1
	);


	sfsc->addColumn     ();
	tmp_version_table_colors.fg = BP_COLOR_CODE_FG_NONE;
	tmp_version_table_colors.bg = BP_COLOR_CODE_BG_NONE;
	sfsc->addStringEntry
	(
			"app build date",
			this->app_build_date,
			bpstrlen(this->app_build_date),
			bpstrlen(this->app_build_date),
			&tmp_version_table_colors,
			BP_FALSE,
			1,
			0,
			&BP_STSBS_border_set_onlypadding_1
	);

	sfsc->addColumn();
	sfsc->addStringEntry
	(
			"app build time",
			this->app_build_time,
			bpstrlen(this->app_build_time),
			bpstrlen(this->app_build_time),
			&tmp_version_table_colors,
			BP_FALSE,
			1,
			0,
			&BP_STSBS_border_set_onlypadding_1
	);

	// destroy the joined string
	bpfree(joined_string);

	// return indicating success
	return ERR_SUCCESS;

}


// Build app name/description table.
BP_ERROR_T  BP_ApplicationHelpGenerator::buildApplicationNameAndDescriptionTable()
{


	if(!this->sfsc)
		return ERR_FAILURE;


	// create a string stack we can join up easily
	char *str_stack_to_join[12];
	BP_ZERO_STACK(str_stack_to_join);



	BP_COLOR_CODE_SET_T tmp_version_table_colors;
	tmp_version_table_colors.fg = BP_COLOR_CODE_FG_RED;
	tmp_version_table_colors.bg = BP_COLOR_CODE_BG_BLACK;

	sfsc->addRowGroup();
	sfsc->addRow        ();
	sfsc->addColumnGroup();
	sfsc->addColumn     ();
	sfsc->addStringEntry
	(
			"app name",
			this->app_name,
			bpstrlen(this->app_name),
			bpstrlen(this->app_name),
			&tmp_version_table_colors,
			BP_FALSE,
			0,
			0,
			&BP_STSBS_border_set_onlypadding_no_bottom_border_1
	);




	tmp_version_table_colors.fg = BP_COLOR_CODE_FG_NONE;
	tmp_version_table_colors.bg = BP_COLOR_CODE_BG_NONE;

	// add description as a new row
	sfsc->addRowGroup   ();
	sfsc->addRow        ();
	sfsc->addColumnGroup();
	sfsc->addColumn     ();
	sfsc->addStringEntry
	(
			"version table",
			this->app_description,
			bpstrlen(this->app_description),
			100,
			&tmp_version_table_colors,
			BP_FALSE,
			1,
			0,
			&BP_STSBS_border_set_onlypadding_no_bottom_border_3

	);


	// return indicating success
	return ERR_SUCCESS;

}


// Builds the application group entry options (cascades)
BP_ERROR_T BP_ApplicationHelpGenerator::buildApplicationGroupEntryOptions()
{

	// ensure we have a sfsc to work with
	if(!this->sfsc)
		return ERR_FAILURE;

	// ensure that we have groups and a group count
	if(!this->groups || ! this->group_n)
		return ERR_FAILURE;

	// current group and entry
	P_BP_AHG_OPTION_GROUP curr_group = NULL;
	P_BP_AHG_OPTION_ENTRY curr_entry = NULL;


	// walk groups and generate options
	size_t n = 0;
	for(; n < this->group_n; n++)
	{

		// set the current group
		curr_group = &this->groups[n];


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Add Padding Here %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


		// use empty (random uuid) values
		sfsc->addRowGroup   ();
		sfsc->addRow        ();
		sfsc->addColumnGroup();
		sfsc->addColumn     ();
		sfsc->addStringEntry("empty padding","     ",5,	5,NULL,	BP_FALSE,1,	0,NULL);



		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Add Row Group Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		BP_COLOR_CODE_SET_T tmp_group_name_colors;
		tmp_group_name_colors.fg = BP_COLOR_CODE_FG_GREEN;
		tmp_group_name_colors.bg = BP_COLOR_CODE_BG_BLACK;

		// use empty (random uuid) values
		sfsc->addRowGroup   ();
		sfsc->addRow        ();
		sfsc->addColumnGroup();


		sfsc->addColumn     ();
		sfsc->addStringEntry
		(
			"curr_group_name",
			curr_group->group_name,
			bpstrlen(curr_group->group_name),
			bpstrlen(curr_group->group_name),
			&tmp_group_name_colors,
			BP_FALSE,
			0,
			0,
			curr_group->border_set
		);

		BP_COLOR_CODE_SET_T tmp_group_desc_colors;
		tmp_group_desc_colors.fg = BP_COLOR_CODE_FG_NONE;
		tmp_group_desc_colors.bg = BP_COLOR_CODE_BG_BLACK;

		sfsc->addColumn     ();
		sfsc->addStringEntry
		(
				"curr_group",
				curr_group->group_description,
				bpstrlen(curr_group->group_description),
				bpstrlen(curr_group->group_description),
				&tmp_group_desc_colors,
				BP_FALSE,
				0,
				0,
				curr_group->border_set
		);


		size_t longest_token = 0;
		size_t j = 0;
		for(j = 0; j < curr_group->entry_n; j++)
		{

			if(bpstrlen(curr_group->entries[j].token->token) > longest_token)
				longest_token = bpstrlen(curr_group->entries[j].token->token);

		}

		// now add entries
		for(j = 0; j < curr_group->entry_n; j++)
		{

			curr_entry = &curr_group->entries[j];

			BP_COLOR_CODE_SET_T tmp_token_name_colors;
			tmp_token_name_colors.fg = BP_COLOR_CODE_FG_NONE;
			tmp_token_name_colors.bg = BP_COLOR_CODE_BG_NONE;

			// if the token is required, we set colors on it
			if(curr_entry->token->token_required == BP_TRUE)
			{
				if(curr_entry->token->value_has_been_set_ok)
					tmp_token_name_colors.fg = BP_COLOR_CODE_FG_GREEN;
				else
					tmp_token_name_colors.fg = BP_COLOR_CODE_FG_RED;
			}

			sfsc->addRowGroup   ();
			sfsc->addRow        ();
			sfsc->addColumnGroup();

			sfsc->addColumn     ("token column");
			sfsc->addStringEntry
			(
					curr_entry->name,
					curr_entry->token->token,
					bpstrlen(curr_entry->token->token),
					longest_token,
					&tmp_token_name_colors,
					BP_FALSE,
					1,
					0,
					&BP_STSBS_border_set_onlypadding_3
			);

			sfsc->addColumn("description_column");

			sfsc->addStringEntry
			(
					curr_entry->token->token_description,
					curr_entry->token->token_description,
					bpstrlen(curr_entry->token->token_description),
					80,
					NULL,
					BP_FALSE,
					0,
					0,
					&BP_STSBS_border_set_onlypadding_no_bottom_border_4
			);

			char *tmp_joiner = NULL;
			// fill in the default value if there is on
			if(curr_entry->token->value_has_been_set_ok != true)
			{
				if(this->clop->getDefaultValueAsCharString(curr_entry->token->token) == ERR_SUCCESS)
				if(this->clop->token_default_value_as_string)
				{

					// add a column
					// sfsc->addColumn();

					// create a temporary joiner
					tmp_joiner = BP_JoinStrings("Default: ", this->clop->token_default_value_as_string);

					// set default color to yellow
					BP_COLOR_CODE_SET_T tmp_default_value_colors;
					tmp_default_value_colors.fg = BP_COLOR_CODE_FG_YELLOW;
					tmp_default_value_colors.bg = BP_COLOR_CODE_BG_NONE;


					// add it as a value
					sfsc->addStringEntry
					(
							tmp_joiner,
							tmp_joiner,
							bpstrlen(tmp_joiner),
							bpstrlen(tmp_joiner),
							&tmp_default_value_colors,
							BP_FALSE,
							1,
							0,
							&BP_STSBS_border_set_onlypadding_no_bottom_border_4
					);

					// destroy the joiner here (it's not tailqueue allocated.
					bpfree(tmp_joiner);

				}

			}
			else
			{

				if(this->clop->getCurrentValueAsCharString(curr_entry->token->token) == ERR_SUCCESS)
				if(this->clop->token_current_value_as_string)
				{

					// add a column
					// sfsc->addColumn();

					// create a temporary joiner
					tmp_joiner = BP_JoinStrings("Set Val: ", this->clop->token_current_value_as_string);

					// set default color to yellow
					BP_COLOR_CODE_SET_T tmp_current_value_colors;
					tmp_current_value_colors.fg = BP_COLOR_CODE_FG_GREEN;
					tmp_current_value_colors.bg = BP_COLOR_CODE_BG_NONE;


					// add it as a value
					sfsc->addStringEntry
					(
							tmp_joiner,
							tmp_joiner,
							bpstrlen(tmp_joiner),
							bpstrlen(tmp_joiner),
							&tmp_current_value_colors,
							BP_FALSE,
							1,
							0,
							&BP_STSBS_border_set_onlypadding_no_bottom_border_4
					);

					// destroy the joiner here (it's not tailqueue allocated.
					bpfree(tmp_joiner);

				}
			}


			if(curr_entry->dependencies)
			{

				sfsc->addColumn();

				char *deps_str = "Mandatory Depends:";
				sfsc->addStringEntry
				(
						"deps_str",
						deps_str,
						bpstrlen(deps_str),
						bpstrlen(deps_str),
						NULL,
						BP_FALSE,
						0,
						0,
						&BP_STSBS_border_set_onlypadding_no_bottom_border_1
				);



				BP_COLOR_CODE_SET_T tmp_colors;
				tmp_colors.fg = BP_COLOR_CODE_FG_NONE;
				tmp_colors.bg = BP_COLOR_CODE_BG_NONE;


				// Generate Mandatory Dependency
				P_BP_LINKL tmp_tq = BP_CreateLinkL(".", BP_FALSE);
				size_t l = 0;
				for(l = 0; l < curr_entry->dependency_n; l++)
				{

					tmp_colors.fg = BP_COLOR_CODE_FG_NONE;
					tmp_colors.bg = BP_COLOR_CODE_BG_NONE;

					if(curr_entry->dependencies[l].depends_how != BP_AHG_ENTRY_DEPENDENCY_MANDATORY)
						continue;



					// dependency check
					if(curr_entry->dependencies[l].entry->token->value_has_been_set_ok == BP_TRUE)
						tmp_colors.fg = BP_COLOR_CODE_FG_GREEN;
					else
						tmp_colors.fg = BP_COLOR_CODE_FG_RED;

					//sfsc->addColumn();
					sfsc->addStringEntry
					(
						"deps",
						curr_entry->dependencies[l].entry->token->token,
						bpstrlen(curr_entry->dependencies[l].entry->token->token),
						bpstrlen(curr_entry->dependencies[l].entry->token->token),
						&tmp_colors,
						BP_FALSE,
						0,
						0,
						&BP_STSBS_border_set_onlypadding_1
					);

				}




			}

		}


	}

	// sfsc->DisplayErrors();
	// sfsc->displaySFSSession();
	// return indicating success
	return ERR_SUCCESS;

}


// Builds the application usage examples.
BP_ERROR_T BP_ApplicationHelpGenerator::buildApplicationUsageExamples()
{

	// ensure we have a sfsc to work with
	if(!this->sfsc)
		return ERR_FAILURE;

	// ensure that we have groups and a group count
	if(!this->usage_examples || ! this->usage_example_n)
		return ERR_FAILURE;

	// current group and entry
	P_BP_AHG_USAGE_EXAMPLE curr_usage_example = NULL;


	// add a padding row before the usage examples to distinguish them
	sfsc->addRowGroup   ();
	sfsc->addRow        ();
	sfsc->addColumnGroup();
	sfsc->addColumn     ();
	sfsc->addStringEntry("empty padding","     ",5,	5,NULL,	BP_FALSE,3,	0,NULL);



	// walk groups and generate options
	size_t n = 0;
	for(; n < this->usage_example_n; n++)
	{

		// set the current group
		curr_usage_example = &this->usage_examples[n];

		// use empty (random uuid) values
		sfsc->addRowGroup   ();
		sfsc->addRow        ();
		sfsc->addColumnGroup();
		sfsc->addColumn     ();


		BP_COLOR_CODE_SET_T tmp_usage_colors;
		tmp_usage_colors.fg = BP_COLOR_CODE_FG_YELLOW;
		tmp_usage_colors.bg = BP_COLOR_CODE_BG_BLACK;

		sfsc->addStringEntry
		(
			"curr_group",
			curr_usage_example->name,
			bpstrlen(curr_usage_example->name),
			60,
			&tmp_usage_colors,
			BP_FALSE,
			0,
			0,
			&BP_STSBS_border_set_underline_1
		);


		sfsc->addStringEntry
		(
			"curr_description",
			curr_usage_example->description,
			bpstrlen(curr_usage_example->description),
			80,
			NULL,
			BP_FALSE,
			0,
			0,
			&BP_STSBS_border_set_onlypadding_5
		);

		// create joiner array to join argv[0] with the usage
		// example.
		char *joiner[4];
		char *joined = NULL;
		BP_ZERO_STACK(joiner);

		// set the join elements
		joiner[0] = this->clop->argv[0];
		joiner[1] = " ";
		joiner[2] = curr_usage_example->usage_string;

		// join the strings
		joined = BP_JoinStringArray(joiner, 3, NULL);

		BP_COLOR_CODE_SET_T tmp_usage_string_colors;
		tmp_usage_string_colors.fg = BP_COLOR_CODE_FG_GREEN;
		tmp_usage_string_colors.bg = BP_COLOR_CODE_BG_NONE;

		sfsc->addStringEntry
		(
				"curr_usage_string",
				joined,
				bpstrlen(joined),
				100,
				&tmp_usage_string_colors,
				BP_FALSE,
				0,
				0,
				&BP_STSBS_border_set_onlypadding_5
		);

		// add a padding row before the usage examples to distinguish them
		sfsc->addRowGroup   ();
		sfsc->addRow        ();
		sfsc->addColumnGroup();
		sfsc->addColumn     ();
		sfsc->addStringEntry("empty padding","     ",5,	5,NULL,	BP_FALSE,1,	0,NULL);


		// if the join was ok, free it
		if(joined)
			bpfree(joined);

	}


	// return indicating success
	return ERR_SUCCESS;

}


BP_ERROR_T BP_ApplicationHelpGenerator::buildApplicationErrorTables()
{

	// check to see that we have a sfsc set
	if(!this->sfsc)
		return ERR_FAILURE;

	// ensure we have some groups
	if(!this->groups)
		return ERR_FAILURE;

	// add a new rowgroup
	this->sfsc->addRowGroup();
	this->sfsc->addRow();
	this->sfsc->addColumnGroup();
	this->sfsc->addColumn();

	char * err_table_title = "Runtime Errors Occured:";


	BP_COLOR_CODE_SET_T tmp_error_table_title_colors;
	tmp_error_table_title_colors.fg = BP_COLOR_CODE_FG_RED;
	tmp_error_table_title_colors.bg = BP_COLOR_CODE_BG_BLACK;

	sfsc->addStringEntry
	(
			"err table title",
			err_table_title,
			bpstrlen(err_table_title),
			100,
			&tmp_error_table_title_colors,
			BP_FALSE,
			0,
			0,
			&BP_STSBS_border_set_onlypadding_1
	);


	// current group and entry
	P_BP_AHG_OPTION_GROUP current_group = NULL;
	P_BP_AHG_OPTION_ENTRY current_entry = NULL;

	// walk groups
	size_t n = 0;
	for(; n < this->group_n; n++)
	{

		// set current group
		current_group = &this->groups[n];




		// walk group entries
		size_t j = 0;
		for(j=0; j < current_group->entry_n;j++)
		{

			// set current entry
			current_entry = &current_group->entries[j];

			// check tokens
			if(!current_entry->token->value_has_been_set_ok && current_entry->token->token_required)
			{

				tmp_error_table_title_colors.fg = BP_COLOR_CODE_FG_RED;
				tmp_error_table_title_colors.bg = BP_COLOR_CODE_BG_NONE;

				this->sfsc->addRowGroup();
				this->sfsc->addRow();
				this->sfsc->addColumnGroup();
				this->sfsc->addColumn();
				sfsc->addStringEntry
				(
						"row name err",
						current_entry->token->token,
						bpstrlen(current_entry->token->token),
						bpstrlen(current_entry->token->token),
						&tmp_error_table_title_colors,
						BP_FALSE,
						0,
						0,
						&BP_STSBS_border_set_onlypadding_5
				);


				char * err_msg = " is required but has not been set.";
				this->sfsc->addColumn();
				sfsc->addStringEntry
				(
						"row name err msg",
						err_msg,
						bpstrlen(err_msg),
						bpstrlen(err_msg),
						&tmp_error_table_title_colors,
						BP_FALSE,
						0,
						0,
						&BP_STSBS_border_set_onlypadding_1
				);


			}




		}



	}


	// return indicating success
	return ERR_SUCCESS;

}

// Build the application header.
BP_ERROR_T BP_ApplicationHelpGenerator::buildApplicationFooter()
{

	// ensure we have a sfsc to work with
	if(!this->sfsc)
		return ERR_FAILURE;

	// random index used for assigning elements
	size_t random_idx = 0;

	// art to use
	char * footer_art_selected = NULL;

    // Pick the footer art to use
    if(this->aa_footer_art && this->aa_footer_art_n)
    {

		if(this->aa_footer_art_randomize)
		{
				random_idx = BP_GenRandomUnsignedLong(0, this->aa_footer_art_n-1);
				footer_art_selected = this->aa_footer_art[random_idx];
		}
		else
		{
				footer_art_selected = this->aa_footer_art[0];
		}

    }

    if(footer_art_selected)
	{
		sfsc->addRowGroup   ("footer_art_selected");
		sfsc->addRow        ("row1");
		sfsc->addColumnGroup("colgroup1");
		sfsc->addColumn     ("column1");
		sfsc->addStringEntry
		(
				"footer_art_selected",
				footer_art_selected,
				bpstrlen(footer_art_selected),
				0,
				&this->aa_footer_art_colors,
				BP_TRUE,
				0,
				0,
				this->aa_footer_art_border_set
		);
	}

	// return indicating success
	return ERR_SUCCESS;

}


// Build Version Box

// Build Date / Time Box

// Build App Author Box




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// display the application help
BP_ERROR_T BP_ApplicationHelpGenerator::displayApplicationHelp()
{


	if(!this->sfsc)
		return ERR_FAILURE;

	// prepare the final output
    sfsc->prepareFinalOutput();

    // display the final output
    sfsc->displayFinalOutput();

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Utility Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// displays the localized class data as it appears
BP_ERROR_T BP_ApplicationHelpGenerator::displayClassLocalData()
{

	printf("\n [+] Application Help Generator (%p)", this);
	printf("\n     tq:      %p", this->tq);
	printf("\n     app built on:        %s - %s", this->app_build_time, this->app_build_date);
	printf("\n     blockparty built on: %s - %s", BP_LIBRARY_COMPILED_BUILD_TIME, BP_LIBRARY_COMPILED_BUILD_DATE);
	printf("\n     blockparty version:  %u.%u.%u", BP_LIBRARY_MAJOR_VERSION, BP_LIBRARY_MINOR_VERSION, BP_LIBRARY_BUILD_ID);
	printf("\n     groups:              %p", this->groups);
	printf("\n     group_n:             %u", this->group_n);
	printf("\n");

	// current group
	P_BP_AHG_OPTION_GROUP current_group = NULL;

	// current entry
	P_BP_AHG_OPTION_ENTRY current_entry = NULL;

	// walk all groups
	size_t n =0;
	for(; n < this->group_n; n++)
	{

		// set current group
		current_group = &this->groups[n];

		// display header information
		printf("\n\t [-] Group - %s - (%p)", current_group->group_name, current_group);
		printf("\n\t     group_description: %s", current_group->group_description);
		printf("\n\t     entries:  %p", current_group->entries);
		printf("\n\t     entry_n:  %u", current_group->entry_n);

		// if entries exist, display them
		if(current_group->entry_n && current_group->entries)
		{

			printf("\n");

			// walk group entries
			size_t j = 0;
			for(; j < current_group->entry_n; j++)
			{

				// set current entry
				current_entry = &current_group->entries[j];
				printf("\n\t\t [-] Entry (%p)", current_entry);
				printf("\n\t\t     name:         %s", current_entry->name);
				printf("\n\t\t     description:  %s", current_entry->description);
				printf("\n\t\t     token:        %p - (%s)", current_entry->token, current_entry->token ? current_entry->token->token : "unset");
				printf("\n\t\t     dependencies: %p", current_entry->dependencies);
				printf("\n\t\t     dependency_n: %u", current_entry->dependency_n);
				printf("\n");

				size_t x = 0;
				for(; x < current_entry->dependency_n; x++)
				{

					char * dep_how = NULL;
					switch(current_entry->dependencies[x].depends_how)
					{
						case BP_AHG_ENTRY_DEPENDENCY_NONE:
							dep_how = "BP_AHG_ENTRY_DEPENDENCY_NONE";
							break;
						case BP_AHG_ENTRY_DEPENDENCY_MANDATORY:
							dep_how = "BP_AHG_ENTRY_DEPENDENCY_MANDATORY";
							break;
						case BP_AHG_ENTRY_DEPENDENCY_COMPLIMENTARY:
							dep_how = "BP_AHG_ENTRY_DEPENDENCY_COMPLIMENTARY";
							break;
						default:
							dep_how = "BAD VALUE";
							break;
					}

					printf
					(
						"\n\t\t\t    [-] dep: %p - %s - %s",
						current_entry->dependencies[x].entry,
						current_entry->dependencies[x].entry->name,
						dep_how
					);

				}

				printf("\n");


			}

		}

		// add newline
		printf("\n");

	}

	// display company name big art here
	if(this->aa_company_name_big_art)
	{
		printf("\n");
		printf("\n [+] Company Name Ascii Art (big): ");
		printf("\n      aa_company_name_big_art:   %p", this->aa_company_name_big_art);
		printf("\n      aa_company_name_big_art_n: %u", this->aa_company_name_big_art_n);
		printf("\n      randomized:                %s", this->aa_company_name_big_art_randomize ? "yes" : "no");
		for(n = 0; n < this->aa_company_name_big_art_n; n++)
		{
			printf("\n\t [-] Entry: %p", this->aa_company_name_big_art[n]);
		}

	}

	// display company name small art here
	if(this->aa_company_name_small_art)
	{

		printf("\n");
		printf("\n [+] Company Name Ascii Art (small): ");
		printf("\n      aa_company_name_small_art:   %p", this->aa_company_name_small_art);
		printf("\n      aa_company_name_small_art_n: %u", this->aa_company_name_small_art_n);
		printf("\n      randomized:                  %s", this->aa_company_name_small_art_randomize ? "yes" : "no");

		for(n = 0; n < this->aa_company_name_small_art_n; n++)
		{
			printf("\n\t [-] Entry: %p", this->aa_company_name_small_art[n]);
		}

	}

    // display header art
    if(this->aa_header_art)
    {

		printf("\n");
		printf("\n [+] Header Ascii Art (small): ");
		printf("\n      aa_header_art:   %p", this->aa_header_art);
		printf("\n      aa_header_art_n: %u", this->aa_header_art_n);
		printf("\n      randomized:                  %s", this->aa_header_art_randomize ? "yes" : "no");

		for(n = 0; n < this->aa_header_art_n; n++)
		{
				printf("\n\t [-] Entry: %p", this->aa_header_art[n]);
		}

    }

    // display body art here
    if(this->aa_body_art)
    {

		printf("\n");
		printf("\n [+] Body Ascii Art (small): ");
		printf("\n      aa_body_art:   %p", this->aa_body_art);
		printf("\n      aa_body_art_n: %u", this->aa_body_art_n);
		printf("\n      randomized:                  %s", this->aa_body_art_randomize ? "yes" : "no");

		for(n = 0; n < this->aa_body_art_n; n++)
		{
				printf("\n\t [-] Entry: %p", this->aa_body_art[n]);
		}

    }

    // display footer art here
    if(this->aa_footer_art)
    {

		printf("\n");
		printf("\n [+] Footer Ascii Art (small): ");
		printf("\n      aa_footer_art:   %p", this->aa_footer_art);
		printf("\n      aa_footer_art_n: %u", this->aa_footer_art_n);
		printf("\n      randomized:                  %s", this->aa_footer_art_randomize ? "yes" : "no");

		for(n = 0; n < this->aa_footer_art_n; n++)
		{
				printf("\n\t [-] Entry: %p", this->aa_footer_art[n]);
		}

    }


    if(this->usage_examples && this->usage_example_n)
    {
    	printf("\n");
    	printf("\n [+] Usage Examples (%p)", this->usage_examples);
    	printf("\n     usage_example_n:  %u", this->usage_example_n);
    	for(n = 0; n < this->usage_example_n; n++)
    	{
    		printf("\n");
    		printf("\n\t [-] Usage Example Entry:");
    		printf("\n\t     name:         %s", this->usage_examples[n].name);
    		printf("\n\t     description:  %s", this->usage_examples[n].description);
    		printf("\n\t     usage string: %s", this->usage_examples[n].usage_string);

    	}

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


// clones the current help generator, including the command line option parser
// that it works with.
BP_ApplicationHelpGenerator * BP_ApplicationHelpGenerator::clone()
{

	// a command line option parser is required for us to create a clone
	if(!this->clop)
		return NULL;

	// clone command line option parser first
	BP_CommandLineOptionParser * new_clop = this->clop->clone();
	if(!new_clop)
		return NULL;

	// attempt to create a new clone
	BP_ApplicationHelpGenerator * ret_clone = new BP_ApplicationHelpGenerator(new_clop);
	if(!ret_clone)
	{
		delete new_clop;
		return NULL;
	}

	// set foreground and background colors
	ret_clone->argument_required_colors.bg = this->argument_required_colors.bg;
	ret_clone->argument_required_colors.fg = this->argument_required_colors.fg;

	// clone the app_info_border_set
	ret_clone->app_info_border_set = BP_CloneBorderSet(this->app_info_border_set, ret_clone->tq);

	// now set the app info
	ret_clone->app_version.build_indicator = this->app_version.build_indicator;
	ret_clone->app_version.major_version   = this->app_version.build_indicator;
	ret_clone->app_version.minor_version   = this->app_version.build_indicator;

	// clone the application name
	if(this->app_name)
		ret_clone->app_name = tq_strdup(ret_clone->tq, this->app_name, BPLN);

	// clone the application description
	if(this->app_description)
		ret_clone->app_description = tq_strdup(ret_clone->tq, this->app_description, BPLN);

	// clone the application build date
	if(this->app_build_date)
		ret_clone->app_build_date = tq_strdup(ret_clone->tq, this->app_build_date, BPLN);

	// clone the build time
	if(this->app_build_time)
		ret_clone->app_build_time = tq_strdup(ret_clone->tq, this->app_build_time, BPLN);

	// set the general purpose linear iterator
	size_t n = 0;

	// duplicate big company art
	if(this->aa_company_name_big_art)
	{

		// set the number of entries
		ret_clone->aa_company_name_big_art_n         = this->aa_company_name_big_art_n;

		// calloc the entries
		ret_clone->aa_company_name_big_art           = (char **) tq_calloc(ret_clone->tq, sizeof(char*) * this->aa_company_name_big_art_n, 1, BPLN);

		// walk the art entries
		for(n = 0; n < ret_clone->aa_company_name_big_art_n; n++)
		{

			// copy in the entry
			ret_clone->aa_company_name_big_art[n] = tq_strdup(ret_clone->tq, this->aa_company_name_big_art[n], BPLN);

		}

		// copy over the randomization setting
		ret_clone->aa_company_name_big_art_randomize = this->aa_company_name_big_art_randomize;

		// clone the colors
		ret_clone->aa_company_name_big_art_colors.fg = this->aa_company_name_big_art_colors.fg;
		ret_clone->aa_company_name_big_art_colors.bg = this->aa_company_name_big_art_colors.bg;

		// clone the border set
		ret_clone->aa_company_name_big_art_border_set = BP_CloneBorderSet(this->aa_company_name_big_art_border_set, ret_clone->tq);

	}


	// duplicate small company art
	if(this->aa_company_name_small_art)
	{

		// set the number of entries
		ret_clone->aa_company_name_small_art_n         = this->aa_company_name_small_art_n;

		// calloc the entries
		ret_clone->aa_company_name_small_art           = (char **) tq_calloc(ret_clone->tq, sizeof(char*) * this->aa_company_name_small_art_n, 1, BPLN);

		// walk the art entries
		for(n = 0; n < ret_clone->aa_company_name_small_art_n; n++)
		{

			// copy in the entry
			ret_clone->aa_company_name_small_art[n] = tq_strdup(ret_clone->tq, this->aa_company_name_small_art[n], BPLN);

		}

		// copy over the randomization setting
		ret_clone->aa_company_name_small_art_randomize = this->aa_company_name_small_art_randomize;

		// clone the colors
		ret_clone->aa_company_name_small_art_colors.fg = this->aa_company_name_small_art_colors.fg;
		ret_clone->aa_company_name_small_art_colors.bg = this->aa_company_name_small_art_colors.bg;

		// clone the border set
		ret_clone->aa_company_name_small_art_border_set = BP_CloneBorderSet(this->aa_company_name_small_art_border_set, ret_clone->tq);

	}


	// duplicate header
	if(this->aa_header_art)
	{

		// set the number of entries
		ret_clone->aa_header_art_n         = this->aa_header_art_n;

		// calloc the entries
		ret_clone->aa_header_art           = (char **) tq_calloc(ret_clone->tq, sizeof(char*) * this->aa_header_art_n, 1, BPLN);

		// walk the art entries
		for(n = 0; n < ret_clone->aa_header_art_n; n++)
		{

			// copy in the entry
			ret_clone->aa_header_art[n] = tq_strdup(ret_clone->tq, this->aa_header_art[n], BPLN);

		}

		// copy over the randomization setting
		ret_clone->aa_header_art_randomize = this->aa_header_art_randomize;

		// clone the colors
		ret_clone->aa_header_art_colors.fg = this->aa_header_art_colors.fg;
		ret_clone->aa_header_art_colors.bg = this->aa_header_art_colors.bg;

		// clone the border set
		ret_clone->aa_header_art_border_set = BP_CloneBorderSet(this->aa_header_art_border_set, ret_clone->tq);

	}


	// duplicate body art
	if(this->aa_body_art)
	{

		// set the number of entries
		ret_clone->aa_body_art_n         = this->aa_body_art_n;

		// calloc the entries
		ret_clone->aa_body_art           = (char **) tq_calloc(ret_clone->tq, sizeof(char*) * this->aa_body_art_n, 1, BPLN);

		// walk the art entries
		for(n = 0; n < ret_clone->aa_body_art_n; n++)
		{

			// copy in the entry
			ret_clone->aa_body_art[n] = tq_strdup(ret_clone->tq, this->aa_body_art[n], BPLN);

		}

		// copy over the randomization setting
		ret_clone->aa_body_art_randomize = this->aa_body_art_randomize;

		// clone the colors
		ret_clone->aa_body_art_colors.fg = this->aa_body_art_colors.fg;
		ret_clone->aa_body_art_colors.bg = this->aa_body_art_colors.bg;

		// clone the border set
		ret_clone->aa_body_art_border_set = BP_CloneBorderSet(this->aa_body_art_border_set, ret_clone->tq);

	}

	// duplicate footer art
	if(this->aa_footer_art)
	{

		// set the number of entries
		ret_clone->aa_footer_art_n         = this->aa_footer_art_n;

		// calloc the entries
		ret_clone->aa_footer_art           = (char **) tq_calloc(ret_clone->tq, sizeof(char*) * this->aa_footer_art_n, 1, BPLN);

		// walk the art entries
		for(n = 0; n < ret_clone->aa_footer_art_n; n++)
		{

			// copy in the entry
			ret_clone->aa_footer_art[n] = tq_strdup(ret_clone->tq, this->aa_footer_art[n], BPLN);

		}

		// copy over the randomization setting
		ret_clone->aa_footer_art_randomize = this->aa_footer_art_randomize;

		// clone the colors
		ret_clone->aa_footer_art_colors.fg = this->aa_footer_art_colors.fg;
		ret_clone->aa_footer_art_colors.bg = this->aa_footer_art_colors.bg;

		// clone the border set
		ret_clone->aa_footer_art_border_set = BP_CloneBorderSet(this->aa_footer_art_border_set, ret_clone->tq);

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Art is done, Now Build Groups %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// allocate space for groups
	ret_clone->groups = (P_BP_AHG_OPTION_GROUP) tq_calloc(ret_clone->tq, sizeof(BP_AHG_OPTION_GROUP) * this->group_n, 1, BPLN);

	// set group count
	ret_clone->group_n = this->group_n;

	// set current group as null (will be assigned below)
	P_BP_AHG_OPTION_GROUP current_group = NULL;
	P_BP_AHG_OPTION_GROUP ret_group     = NULL;

	// secondary iterator
	size_t j = 0;



	// now clone each group by walking them one by one
	for(n = 0; n < ret_clone->group_n; n++)
	{

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Set Iterators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// set the ret group
		ret_group = &ret_clone->groups[n];

		// set the current group
		current_group = &this->groups[n];


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Clone Individual Items %%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


		// clone the group name
		ret_group->group_name = tq_strdup(ret_clone->tq, current_group->group_name, BPLN);

		// clone the group name colors
		ret_group->group_name_colors.fg = current_group->group_name_colors.fg;
		ret_group->group_name_colors.bg = current_group->group_name_colors.bg;

		// clone the border set
		ret_group->border_set = BP_CloneBorderSet(current_group->border_set, ret_clone->tq);

		// clone the group description
		ret_group->group_description = tq_strdup(ret_clone->tq, current_group->group_description, BPLN);

		// clone the group description colors
		ret_group->group_description_colors.bg = current_group->group_description_colors.bg;
		ret_group->group_description_colors.fg = current_group->group_description_colors.fg;

		// clone individual entries
		ret_group->entries = (P_BP_AHG_OPTION_ENTRY) tq_calloc(ret_clone->tq, sizeof(BP_AHG_OPTION_ENTRY) * current_group->entry_n, 1, BPLN);
		ret_group->entry_n = current_group->entry_n;


		// now fill in the entries
		for(j=0; j < ret_group->entry_n; j++)
		{

			// copy name
			ret_group->entries[j].name = tq_strdup(ret_clone->tq, current_group->entries[j].name, BPLN);

			// copy description
			ret_group->entries[j].description = tq_strdup(ret_clone->tq, current_group->entries[j].description, BPLN);

			// copy border set
			ret_group->entries[j].border_set = BP_CloneBorderSet(current_group->entries[j].border_set, ret_clone->tq);

			// don't set token here, set it below in the next loop

			// set the dependency count (it's fine to set it here, even though dep structures
			// actually aren't allocated till further below)
			ret_group->entries[j].dependency_n = current_group->entries[j].dependency_n;

		}

	}


	// Now set tokens (requires entries to be set first)
	// now clone each group by walking them one by one
	for(n = 0; n < ret_clone->group_n; n++)
	{

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Set Iterators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// set the ret group
		ret_group = &ret_clone->groups[n];

		// set the current group
		current_group = &this->groups[n];


		// now fill in the tokens
		for(j=0; j < ret_group->entry_n; j++)
		{

			// copy token by looking it up locally
			ret_group->entries[j].token = new_clop->optionLookup(current_group->entries[j].token->token);

		}

	}


	// the current entry
	P_BP_AHG_OPTION_ENTRY current_entry = NULL;

	// the clone entry
	P_BP_AHG_OPTION_ENTRY clone_entry   = NULL;

	// dependency iterator
	size_t x = 0;

	// Now Fill in dependencies
	for(n = 0; n < ret_clone->group_n; n++)
	{


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Set Iterators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// set the ret group
		ret_group = &ret_clone->groups[n];

		// set the current group
		current_group = &this->groups[n];


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Check For Dependency in Current Group %%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// walk individual entries, find dependencies
		for(j = 0; j < current_group->entry_n; j++)
		{

			// set the current entry
			current_entry = &current_group->entries[j];
			clone_entry   = &ret_group->entries[j];

			// dont allocate space unless we need to
			if(!current_entry->dependency_n && !clone_entry->dependency_n)
				continue;

			// allocate space for dependencies
			clone_entry->dependencies = (P_BP_AHG_ENTRY_DEPENDENCY) tq_calloc(ret_clone->tq, sizeof(BP_AHG_ENTRY_DEPENDENCY) * clone_entry->dependency_n, 1, BPLN);

			// set the lookup group and the lookup entry
			P_BP_AHG_OPTION_GROUP lookup_group = NULL;
			P_BP_AHG_OPTION_ENTRY lookup_entry = NULL;

			// walk individual dependencies
			for(; x < clone_entry->dependency_n; x++)
			{

				// ensure that we have an entry and group match
				if(!current_entry->dependencies[x].entry || !current_entry->dependencies[x].group)
					continue;


				// attempt to lookup the group
				lookup_group = ret_clone->lookupOptionGroupByName(current_entry->dependencies[x].group->group_name);
				if(!lookup_group)
					continue;

				// attempt to lookup the entry
				lookup_entry = ret_clone->lookupEntryInOptionGroup(lookup_group, current_entry->dependencies[x].entry->name);
				if(!lookup_entry)
					continue;

				// set the dependency mechanism
				clone_entry->dependencies[x].depends_how = current_entry->dependencies[x].depends_how;
				clone_entry->dependencies[x].entry = lookup_entry;
				clone_entry->dependencies[x].group = lookup_group;

			}

		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Last Step: Clone Usage Examples %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	ret_clone->usage_example_n = this->usage_example_n;
	ret_clone->usage_examples = (P_BP_AHG_USAGE_EXAMPLE) tq_calloc(ret_clone->tq, sizeof(BP_AHG_USAGE_EXAMPLE) * this->usage_example_n, 1, BPLN);

	// walk individual usage examples and fill in the details
	for(n = 0; n < this->usage_example_n; n++)
	{

		// copy name
		ret_clone->usage_examples[n].name = tq_strdup(ret_clone->tq, this->usage_examples[n].name, BPLN);

		// copy description
		ret_clone->usage_examples[n].description = tq_strdup(ret_clone->tq, this->usage_examples[n].description, BPLN);

		// copy usage string
		ret_clone->usage_examples[n].usage_string = tq_strdup(ret_clone->tq, this->usage_examples[n].usage_string, BPLN);

	}

	// And finally, we're done with cloning. :)

	// return the clone
	return ret_clone;

}



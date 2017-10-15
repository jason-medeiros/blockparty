/*
 * BP_ApplicationHelpGenerator.h
 *
 *  Created on: Aug 1, 2014
 *      Author: root
 */

#ifndef BP_APPLICATIONHELPGENERATOR_H_
#define BP_APPLICATIONHELPGENERATOR_H_

// Enumerator holding default values associated with help generation.  These
// can be overridden by setting the option_xxx values found within the classes
// public members.
enum {
	BP_APPLICATION_HELP_GENERATOR_DEFAULT_TOKEN_COLUMN_WIDTH = 20,
	BP_APPLICATION_HELP_GENERATOR_DEFAULT_TOKEN_INDENT_WIDTH = 5,
	BP_APPLICATION_HELP_GENERATOR_DEFAULT_VALUE_COLUMN_WIDTH = 60
};


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Application Version %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// structure to hold application version information
typedef struct __BP_AHG_APP_VERSION_INFO {

	// minor version of this application
	size_t minor_version;

	// major version of this application
	size_t major_version;

	// build indicator
	size_t build_indicator;

} BP_AHG_APP_VERSION_INFO, *P_BP_AHG_APP_VERSION_INFO;




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Option Entries %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// entries can depend on one another
typedef enum BP_AHG_ENTRY_DEPENDENCY_VALUES {
	BP_AHG_ENTRY_DEPENDENCY_NONE,
	BP_AHG_ENTRY_DEPENDENCY_MANDATORY,
	BP_AHG_ENTRY_DEPENDENCY_COMPLIMENTARY,
	BP_AHG_ENTRY_DEPENDENCY_CONFLICTING
};


// Forward declare option entry (fully defined below, forward declared so
// it can be referenced within the dependency struct)
typedef struct __BP_AHG_OPTION_ENTRY;

// Forward declare the option group so we can reference it in the dep structure.
typedef struct __BP_AHG_OPTION_GROUP;



// entry dependencies
typedef struct __BP_AHG_ENTRY_DEPENDENCY {

	// corresponds to one of the values in the enumerator, displaying
	// how entries correspond to one another.
	BP_AHG_ENTRY_DEPENDENCY_VALUES depends_how;

	// a reference to the group which the entry exists
	__BP_AHG_OPTION_GROUP * group;

	// a reference to the entry which is depended on
	__BP_AHG_OPTION_ENTRY * entry;

} BP_AHG_ENTRY_DEPENDENCY, *P_BP_AHG_ENTRY_DEPENDENCY;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Usage Examples %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Usage examples are stored in this structure after being assigned via
// class methods below.
typedef struct __BP_AHG_USAGE_EXAMPLE {

	// name of usage example
	char * name;

	// application description
	char * description;

	// application usage string
	char * usage_string;

} BP_AHG_USAGE_EXAMPLE, *P_BP_AHG_USAGE_EXAMPLE;



// Application help generator option entry.
typedef struct __BP_AHG_OPTION_ENTRY {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Name and Description %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// name of the entry
	char * name;

	// description of the entry
	char * description;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Border Set %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// border set to use for this group
	P_BP_STRING_ROW_BORDER_SET border_set;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Token %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// token from the command line token parser associated with this entry
	P_BP_COMMAND_LINE_PARSER_TOKENS token;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Dependencies %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// dependencies
	P_BP_AHG_ENTRY_DEPENDENCY dependencies;

	// number of dependencies available
	size_t                    dependency_n;


} BP_AHG_OPTION_ENTRY, *P_BP_AHG_OPTION_ENTRY;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Group Options %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// AHG Entries are stored in groups.  Groups are options which
// relate to each other in some way.  A group can have multiple
// entries.

// Option groups.  Used to associate groups of options together.
typedef struct __BP_AHG_OPTION_GROUP {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Option Group Name %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// group name
	char * group_name;

	// color codes for name
	BP_COLOR_CODE_SET_T group_name_colors;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Border Set %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// border set to use for this group
	P_BP_STRING_ROW_BORDER_SET border_set;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Option Group Description %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// description of the group
	char * group_description;

	// color codes for name
	BP_COLOR_CODE_SET_T group_description_colors;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Group Entry Items %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// entries within this group
	P_BP_AHG_OPTION_ENTRY entries;

	// entry count
	size_t entry_n;

} BP_AHG_OPTION_GROUP, *P_BP_AHG_OPTION_GROUP;



//! Generates a readable user help output.
class BP_ApplicationHelpGenerator : public BP_BaseError
{


public:


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Class Local Arguments %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// tailqueue allocator (handles all memory allocation for this class)
	P_BP_LINKL tq;

	// information regarding the terminal
	P_BP_LINUX_TERMINAL_INFO terminal_info;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Thread Locking %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// If using the AHG in a threaded environment, this should be used
	// before performing any operations to prevent contentions.

	// semaphore to lock this ahg
	sem_t locked;

	// lock
	void lock();

	// unlock
	void unlock();



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Color Codes %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if an argument is required (set in the options)
	BP_COLOR_CODE_SET_T argument_required_colors;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Command Line Option Parser %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// command line option parser associated with this help generator
	BP_CommandLineOptionParser * clop;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% String Formatting Session %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this is the string formatting session used by the class to generate
	// user output.
	BP_StringFormattingSession * sfsc;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Basic Application Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	P_BP_STRING_ROW_BORDER_SET app_info_border_set;

	// version of this application
	BP_AHG_APP_VERSION_INFO app_version;

	// application name
	char * app_name;

	// application description
	char * app_description;

	// these are set directly from compiler defines
	char * app_build_date;

	// same as above
	char * app_build_time;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% ASCII Art Entries %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// company name information
	char **                    aa_company_name_big_art;
	size_t                     aa_company_name_big_art_n;
	BP_BOOL                    aa_company_name_big_art_randomize;
	BP_COLOR_CODE_SET_T        aa_company_name_big_art_colors;
	P_BP_STRING_ROW_BORDER_SET aa_company_name_big_art_border_set;

	char ** aa_company_name_small_art;
	size_t  aa_company_name_small_art_n;
	BP_BOOL aa_company_name_small_art_randomize;
	BP_COLOR_CODE_SET_T        aa_company_name_small_art_colors;
	P_BP_STRING_ROW_BORDER_SET aa_company_name_small_art_border_set;

	// adds company name art to the builtin array (BP_TRUE if large, BP_FALSE if small)
	BP_ERROR_T addCompanyNameArt(char * art, BP_BOOL large_or_small);

	// Header art data
	char ** aa_header_art;
	size_t  aa_header_art_n;
	BP_BOOL aa_header_art_randomize;
	BP_COLOR_CODE_SET_T        aa_header_art_colors;
	P_BP_STRING_ROW_BORDER_SET aa_header_art_border_set;

	// adds header art to the builtin array
	BP_ERROR_T addHeaderArt(char * art);

	// Body art data
	char ** aa_body_art;
	size_t  aa_body_art_n;
	BP_BOOL aa_body_art_randomize;
	BP_COLOR_CODE_SET_T        aa_body_art_colors;
	P_BP_STRING_ROW_BORDER_SET aa_body_art_border_set;

	// adds body art to the builtin array
	BP_ERROR_T addBodyArt(char * art);

	// footer art data
	char ** aa_footer_art;
	size_t  aa_footer_art_n;
	BP_BOOL aa_footer_art_randomize;
	BP_COLOR_CODE_SET_T        aa_footer_art_colors;
	P_BP_STRING_ROW_BORDER_SET aa_footer_art_border_set;

	// adds footer art to the builtin array
	BP_ERROR_T addFooterArt(char * art);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Generated Groups/Entries/Etc %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// groups
	P_BP_AHG_OPTION_GROUP groups;

	// number of groups here
	size_t group_n;

	// add an option group
	BP_ERROR_T addOptionGroup
	(
		char * group_name,
		char * group_description,
		P_BP_STRING_ROW_BORDER_SET border_set = NULL
	);

	// Add an entry to the option group via the local command line parser.  Works
	// by looking up the token in the command line parser class, and assigns it
	// to the specified group.
	BP_ERROR_T addOptionEntryToOptionGroup
	(
		char * group_name,
		char * new_entry_name,
		char * new_entry_description,
		char * token,
		P_BP_STRING_ROW_BORDER_SET border_set = NULL
	);

	// Add a dependency to a option entry (within an option group)
	BP_ERROR_T addOptionDependencyToOptionEntry
	(
		char * group_name,
		char * entry_name,
		char * dependency_group_name,
		char * dependency_name,
		BP_AHG_ENTRY_DEPENDENCY_VALUES depends_how
	);

	// Checks to see if dependencies exist.  All options except depends_how are mandatory.  Returns
	// ERR_SUCCESS if dep exists, ERR_FAILURE if it does not.
	BP_ERROR_T checkIfDependenciesExist
	(
		char * group_name,
		char * entry_name,
		char * dependency_group_name,
		char * dependency_name,
		BP_AHG_ENTRY_DEPENDENCY_VALUES depends_how = BP_AHG_ENTRY_DEPENDENCY_NONE
	);


	// === Lookup Routines =============

	// Similar to the above checkIfDependenciesExist method above, this method returns
	// a pointer to the dependency.
	BP_ERROR_T lookupDependenciesIfTheyExist
	(
		char * group_name,
		char * entry_name,
		char * dependency_group_name,
		char * dependency_name,
		BP_AHG_ENTRY_DEPENDENCY_VALUES depends_how = BP_AHG_ENTRY_DEPENDENCY_NONE
	);

	// Lookup an option group by name.
	P_BP_AHG_OPTION_GROUP lookupOptionGroupByName(char * group_name);

	// attempts to lookup an entry
	P_BP_AHG_OPTION_ENTRY lookupEntryInOptionGroup(P_BP_AHG_OPTION_GROUP group , char *entry_name);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Last Minute Parameter Checks (Verifies Vars Are Set OK) %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Walks the entries in the local command line option parser (this->clop) and verifies
	// that all required arguments have been set, if not, it calculates which arguments
	// arent set correctly, and why.
	BP_ERROR_T checkThatAllRequiredParametersAreSet();

	// This walks the options and checks to see that all dependencies are met ok.
	BP_ERROR_T checkThatAllMandatoryDependenciesAreMet();

	// Check that no conflicting dependencies are set.
	BP_ERROR_T checkThatThereAreNoConflictingDependenciesSet();


	// This method combines the checkThatAllRequiredParametersAreSet, checkThatAllMandatoryDependenciesAreMet,
	// and the option_parser->optionLookup("--help") methods to see if the application is prepared and ready
	// to run.
	BP_ERROR_T finalApplicationErrorCheck();


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Usage Examples %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// usage examples
	P_BP_AHG_USAGE_EXAMPLE usage_examples;

	// number of examples stored
	size_t             usage_example_n;

	// adds a usage example to the local class
	BP_ERROR_T addUsageExample(char *name, char * description, char *usage_string);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Constructor / Deconstructor %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// class constructor
	BP_ApplicationHelpGenerator(BP_CommandLineOptionParser * clo_parser);

	// class deconstructor
	~BP_ApplicationHelpGenerator();


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Settrs: Set Various Information Regarding the Application %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set basic information
	BP_ERROR_T setBasicInformation(char *app_name, char *app_description, char * app_header_art, P_BP_AHG_APP_VERSION_INFO version_info);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Final Generator (Builds Everything) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// calls all the build generators below in the correct order (generates
	// final help output)
	BP_ERROR_T buildApplicationHelp();


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Individual Build Generators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Build the application header
	BP_ERROR_T buildApplicationHeader();

	// Build Version information
	BP_ERROR_T buildApplicationVersionTable();

	// Build app name/description table.
	BP_ERROR_T buildApplicationNameAndDescriptionTable();

	// Builds the footer
	BP_ERROR_T buildApplicationFooter();

	// Builds the application group entry options (cascades)
	BP_ERROR_T buildApplicationGroupEntryOptions();

	// Builds the application usage examples.
	BP_ERROR_T buildApplicationUsageExamples();

	// Builds the runtime error table generator.
	BP_ERROR_T buildApplicationErrorTables();

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// display the application help
	BP_ERROR_T displayApplicationHelp();



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Utility Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// displays the localized class data as it appears
	BP_ERROR_T displayClassLocalData();


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Clone Routine (For Duplicating Between Threaded Target Selectors %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Cloning creates a rough duplicate of this object, useful for passing onto threads.  This is
	// useful for omitting concurrency problems which might arise due to sharing threaded data.  This
	// prevents the need to lock data between threads, which is a major boon for concurrent optimization.


	// clones the current help generator, including the command line option parser
	// that it works with.
	BP_ApplicationHelpGenerator * clone();




};



#endif /* BP_APPLICATIONHELPGENERATOR_H_ */

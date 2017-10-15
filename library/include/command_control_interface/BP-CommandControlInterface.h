
#ifndef BP_COMMAND_CONTROL_INTERFACE_H
#define BP_COMMAND_CONTROL_INTERFACE_H

// ---- Useful Defines --------

// the encoded metasploit underlined "msf >" prompt with control characters. Used as a success code when
// working with metasploit (move to different place later).
 #define METASPLOIT_PROMPT "\x6D\x73\x66\x1B\\[\x30\x6D\x20\x1B\\[\x30\x6D\x3E"

/*
 * Blockparty Command Control Interface:
 * ------------------------------------------------------------
 * The blockparty command control interface is used to control command line
 * processes by way of console interaction.  Blockparty already supports advanced
 * process control mechanisms, so this is relatively simple to implement.
 *
 * How it works:
 * ---------------------------------------------------
 *
 * 1) Each process configured has a set of decision trees/branches associated with it.
 *
 * 2) Each decision tree branch has a pair of input/output regular expressions.
 *
 * 3) When expressions in a branch match, a behavior based command string is generated.
 *
 * 4) Command string is executed through the process, and the result is matched
 *    and parsed accordingly.
 *
 * 5) Decision branches are dynamic and can reconfigure themselves by matches.
 *
 */

// Used when performing find and replace substitutions in "try" strings.
#define BP_CC_MAX_SUBSTITUTIONS 1024

// Prints debug strings for command and control interface if > 0
#define BP_CC_DEBUG 1


// This enum defines the default branch behaviors invoked
// when a process fails.
typedef enum BP_CC_DEFAULT_FAIL_BEHAVIOR
{
	BP_CC_FAIL_FOLOW_NO,
	BP_CC_FAIL_FOLLOW_RESET,
	BP_CC_FAIL_FOLLOW_EXIT,
	BP_CC_FAIL_FOLLOW_CUSTOM_RESET,
	BP_CC_FAIL_FOLLOW_CUSTOM_EXIT
};

// Enumerator for shell types
typedef enum BP_CC_SHELL_INTERFACE
{

	/* badval */
	BP_CC_SHELL_UNSET,

	/* use this when controlling execve processes */
	BP_CC_SHELL_LINUX_EXECVE,

	/* these aren't implemented yet: 2015 */
	BP_CC_SHELL_LINUX_BASH,
	BP_CC_SHELL_LINUX_SH,
	BP_CC_SHELL_WINDOWS_GENERIC

};

// Enumerator for buffer matching policies.
typedef enum BP_CC_MATCH_BUFF_POLICY
{

	/* badval */
	BP_CC_MATCH_BUFF_POLICY_UNSET,

	/* this uses memmem to search memory */
	BP_CC_MATCH_BUFF_POLICY_MEM_SEARCH,

	/* this uses memcmp to check buffer output exactly 1 to 1 */
	BP_CC_MATCH_BUFF_POLICY_MEM_EXACT,

	/* this uses a perl compatible regular expression to search the buffer */
	BP_CC_MATCH_BUFF_POLICY_PCRE_SEARCH

};


// This structure holds the argc/argv/envp of a program.
typedef struct __BP_ARGC_ARGV_ENVP {

	// arg count
	int argc;

	// arg vector
	char ** argv;

	// environment pointers
	char ** envp;

} BP_ARGC_ARGV_ENVP, *P_BP_ARGC_ARGV_ENVP;


// This structure is the main command controller decision
// branch structure.  Each major function of a process control
// application uses a decision tree similar to this.  In its
// purest essence, you can think of this being something
// very similar to expect control statements.
typedef struct _BP_CC_DECISION_BRANCH {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Try and Expect Buffers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this is the buffer that is to be sent out to the process, this buffer
	// is considered to be binary (always) and must have a proper size
	// argument set.
	char  *try_this;
	size_t try_buff_size;

	// this is the data to expect back from the application.  The value
	// is gnu regular expression compatiable.  Set to NULL/0 to expect nothing
	// and always evaluate to MATCHED.
	char  *expect_this;
	size_t expect_buff_size; // size of expect_this

	// Match policy of this branch.  Either PCRE or memory matching
	// based.
	BP_CC_MATCH_BUFF_POLICY expect_match_policy;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Matched Text %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this holds the text that was matched against.
	char               *matched_text;
	size_t              matched_text_size;

	// regular expression match info
	BP_REGEX_MATCH_INFO match_info;



	// if there isn't a "no" decision branch set, this flag indicates the
	// default behavior.(0 = reset branch, 1 = exit process branch)
	BP_CC_DEFAULT_FAIL_BEHAVIOR default_fail_behavior;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Tree + Possible Branches %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this points to the decision tree that this branch belongs to
	struct _BP_CC_DECISION_TREE * tree;

	// -- branches --

	// this is dynamically set by the decision tree
	struct _BP_CC_DECISION_BRANCH * prev;

	// this branch is executed if the expected response matches
	struct _BP_CC_DECISION_BRANCH * yes;

	// this branch is executed if the expected response does not match
	struct _BP_CC_DECISION_BRANCH * no;

	// this branch is executed as a custom reset for this branch
	// if the application fails.
	struct _BP_CC_DECISION_BRANCH * custom_reset;

	// custom exit branch set if the application fails.
	struct _BP_CC_DECISION_BRANCH * custom_exit;


} BP_CC_DECISION_BRANCH, *P_BP_CC_DECISION_BRANCH;


// Premature exits.  Used for detecting (via text search) if an application
// has exited prematurely for whatever reason.
typedef struct _BP_CC_EXIT_SEARCHES {

	// regular expression to test for
	char * expression;

	// regular expression matching
	size_t expression_matched;

	// string that was matched
	char * match_str;

	// match
	size_t match_len;

} BP_CC_EXIT_SEARCHES, *P_BP_CC_EXIT_SEARCHES;



// Set of PCRE expressions which can be run against application
// output.  Expressions which have matched are stored as strings
// in the structure below.
typedef struct _BP_CC_INFORMATION_SEARCHES {

	// PCRE regular expression to test for
	char * expression;

	// regular expression matching
	size_t expression_matched;

	// strings that were matched, in order of appearance
	char ** match_strs;

	// number of matches in the above string array
	size_t match_strs_n;

	// match
	size_t match_len;

} BP_CC_INFORMATION_SEARCHES, *P_BP_CC_INFORMATION_SEARCHES;


// Decision trees are used to hold specific execution branches.
typedef struct _BP_CC_DECISION_TREE {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Basic Tree Configurations %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// command used to invoke this decision tree
	char *command;

	// arguments passed into the tree (must be null ptr terminated).  Arguments
	// are replaced in the decision branches with substitution like {argument_0}, {argument_1}, {argument_x}.
	char **arguments;

	// last success string found.
	char  *last_matched_buffer;
	size_t last_matched_buffer_size;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Static Tree Branches %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// top branch of the tree
	P_BP_CC_DECISION_BRANCH top_branch;


} BP_CC_DECISION_TREE, *P_BP_CC_DECISION_TREE;


// This is the main command and control structure used for handling the
// manipulation of subprocesses by way of their outputs.
typedef struct _BP_CC_SESSION {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% System Allocator And Registry Information %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// tailqueue memory allocator
	P_BP_LINKL m;

	// system hash table registry
	P_BP_HASH_TABLE_REGISTRY system_hreg;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Basic Session Information/Config %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// string used to identify this session (just for info really)
	char *session_name;

	// shell interface to use (mainly for semantics)
	BP_CC_SHELL_INTERFACE shell_type;

	// last command run status (typically ERR_FAILURE/ERR_SUCCESS indicating branch behavior)
	BP_ERROR_T run_status_code;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Runtime Display Configuration %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// note: these are mainly for debug purposes so you can trace application
	//       interactions while you write your applications.

	// display reads from stdout (displayed in green)
	BP_BOOL display_reads_from_app_stdout;

	// display reads from stderr (displayed in red)
	BP_BOOL display_reads_from_app_stderr;

	// display writes to stdin (displayed in blue)
	BP_BOOL display_writes_to_app_stdin;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Runtime Display Configuration %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if this is set to true, all output will be colorized
	BP_BOOL colorize_display_output;

	// stdout colorset
	BP_COLOR_CODE_SET_T app_stdout_colors;

	// stderr color set
	BP_COLOR_CODE_SET_T app_stderr_colors;

	// stdin color set
	BP_COLOR_CODE_SET_T app_stdin_colors;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Internal Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// file descriptors used for this session (stdio/stdout/stderr)
	P_BP_FDSET fds;

	// buffers containing all stdout
	char  *stdout_buff;
	size_t stdout_buff_byte_n;

	// buffer containing all stderr
	char  *stderr_buff;
	size_t stderr_buff_byte_n;




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Last Matched Buffers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// name of last command processed
	char *last_command;

	// last matched buffer for last command
	char  *last_matched_buffer;
	size_t last_matched_buffer_n;

	// last error buffer for command
	char  *last_error_buffer;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Polling Configurations %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// select() timeout seconds (added with usec)
	size_t select_child_stdout_timeout_sec;

	// select() timeout microseconds (added with sec)
	size_t select_child_stdout_timeout_usec;


	// --- old -----------------

	// for reads, this is the default timeout in miliseconds
	size_t poll_timeout;

	// poll timeout for stderr
	size_t poll_timeout_stderr;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %% Search Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// command and control premature exit searches
	P_BP_CC_EXIT_SEARCHES exit_searches;
	size_t                exit_search_n;

	// flag indicating whether or not we've exited
	size_t exited_prematurely;

	// searches
	P_BP_CC_INFORMATION_SEARCHES info_searches;
	size_t info_search_n;

	// incremented every time we have an information match
	size_t total_info_extractions;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Decision Trees %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This is the array of commands which can possibly be run.  Each
	// command and control session has its own series of commands.  Each
	// command is processed as a decision tree.  Array is null ptr terminated.
	// This pointer is null if there are no commands associated.
	P_BP_CC_DECISION_TREE * commands;
	size_t                  commands_n;

	// --- special branches ----------

	// default exit branch for session
	P_BP_CC_DECISION_BRANCH exit;

	// default reset branch for session
	P_BP_CC_DECISION_BRANCH reset;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Logic Relative Branches %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This is the last expect match branch that was hit after running BP_CCDispatcher.
	P_BP_CC_DECISION_BRANCH last_expect_match_success_branch;

	// This is the last failed branch of an expect command.
	P_BP_CC_DECISION_BRANCH last_expect_match_failed_branch;


} BP_CC_SESSION, *P_BP_CC_SESSION;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create / Destroy CC Sessions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Create a new command control session.
P_BP_CC_SESSION BP_CCNewSession
(
		P_BP_FDSET fds,
		char *session_name,
		BP_CC_SHELL_INTERFACE shell_interface
);

// Destroys a session, freeing all associated memory, including
// the session structure itself.
BP_ERROR_T BP_DestroyCCSession(P_BP_CC_SESSION session);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Adding Try/Expect (User Interface) %%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Note:  Use these routines when developing your applications.  Steps involved are:
//   1) Create session.
//   2) Create command.
//   3) Add try/expect branches to command.
//   4) Run dispatcher.

// Adds a command.
BP_ERROR_T BP_CCAddCommand(P_BP_CC_SESSION session, char *command_name);

// Adds a try/expect to a command.
BP_ERROR_T BP_CCAddTryExpectToCommand
(
		P_BP_CC_SESSION         session,
		char                   *command_name,
		char                   *try_buff,
		size_t                  try_buff_size,
		char                   *expect_buff,
		size_t                  expect_buff_size,
		BP_CC_MATCH_BUFF_POLICY expect_match_policy = BP_CC_MATCH_BUFF_POLICY_MEM_SEARCH
);

// Adds an expect to command.
BP_ERROR_T BP_CCAddExpectToCommand
(
	P_BP_CC_SESSION session,
	char           *command_name,
	char           *expect_buff,
	size_t          expect_buff_size
);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Lookup Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempt to retrieve a command tree by name.  Returns NULL on bad parameters/bad lookup.
P_BP_CC_DECISION_TREE BP_CCGetCommandByName
(
	P_BP_CC_SESSION session,
	char           *command_name
);







// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Decision Trees (For Blockparty Internal Use) %%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Note: Don't use these routines when writing your applications.  They are subject
//       to change.  Use the

// creates a new decision tree.
P_BP_CC_DECISION_TREE BP_CCCreateNewDecisionTree
(
	P_BP_CC_SESSION         session,
	char                   *command_identifier,
	P_BP_CC_DECISION_BRANCH top_branch
);

// creates a new branch in a decision branch in an existing tree
P_BP_CC_DECISION_BRANCH BP_CCCreateNewDecisionBranch
(
		P_BP_CC_SESSION             session,
		P_BP_CC_DECISION_TREE       tree_root,
		P_BP_CC_DECISION_BRANCH     prev,
		char                      * try_this,
		size_t                      try_size,
		char *                      expect_this,
		size_t                      expect_size,
		BP_CC_MATCH_BUFF_POLICY     expect_match_policy,
		BP_CC_DEFAULT_FAIL_BEHAVIOR fail_behavior,
		P_BP_CC_DECISION_BRANCH     yes,
		P_BP_CC_DECISION_BRANCH     no,
		P_BP_CC_DECISION_BRANCH     custom_reset,
		P_BP_CC_DECISION_BRANCH     custom_exit
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%% Dispatchers/Processors %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Command dispatcher.  This will automatically dispatch based on fork or forkpty() fdsets.
BP_ERROR_T BP_CCDispatcher
(
		P_BP_CC_SESSION session,
		char           *command_name,
		char           **arguments
);

// Create dispatcher from pty.  This is only to be used when ptys are active for
// forked
BP_ERROR_T BP_CCDispatcherFromPty
(
		P_BP_CC_SESSION session,
		char           *command_name,
		char           **arguments
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Command Tree Processors (workhorses) %%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Command processor (actually invokes command, typically called from BP_CCDispatcher)
BP_ERROR_T BP_CCProcessCommandTree(P_BP_CC_SESSION session, P_BP_CC_DECISION_BRANCH execute_branch);

// process a branch in the command tree.  Processing is relative to buffers present (or soon to be present
// in the session.  This is the pseudoterminal version of BP_CCProcessCommandTree.
BP_ERROR_T BP_CCProcessCommandTreePty(P_BP_CC_SESSION session, P_BP_CC_DECISION_BRANCH execute_branch);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%% Substitutions/Data Manipulations %%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Performs substitutions in a command string based around the {arg_#} format.
// Arg replacements start at 0.
char * BP_CCPerformSubstitutions(P_BP_CC_SESSION session, char * command_str, char ** substitutions);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%% Display/Visualiation %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// visualizes the current state of a command and control session
BP_ERROR_T BP_CCVisualizeSession(P_BP_CC_SESSION session, size_t show_tree, size_t show_branches);

// prints the session structure
BP_ERROR_T BP_CCPrintSession(P_BP_CC_SESSION session, size_t display_branches);

// prints the tree structure.
BP_ERROR_T BP_CCPrintTree(P_BP_CC_DECISION_TREE tree, size_t display_branches);

// recursive branch display function
BP_ERROR_T BP_CCPrintBranches(P_BP_CC_DECISION_BRANCH branch);

// display search information as it is found in a session
BP_ERROR_T BP_CCPrintInformationExtractions(P_BP_CC_INFORMATION_SEARCHES searches);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%% Premature Exit %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Add a premature exit search to the session.  This will cause
// the application to search the application output for the provided
// regexp, and exit in the case it's encountered.  You can add as many
// expressions as you want.  This application uses PCRE (new) not POSIX (old)
// regular expressions.
BP_ERROR_T BP_CCAddPrematureExitTestString(P_BP_CC_SESSION session, char * expression_pcre);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%% Information Extraction %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Adds an expression which will be used to match command output to specific data.
BP_ERROR_T BP_CCAddInformationExtractionRegexpString(P_BP_CC_SESSION session, char * expression_pcre);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%% CC Data Destruction %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Destroys information searches
BP_ERROR_T BP_DestroyCCInformationSearches(P_BP_CC_SESSION session);

// Destroy Exit Searches
BP_ERROR_T BP_DestroyCCExitSearches(P_BP_CC_SESSION session);

// Destroys a specific decision tree branch.
// Destroys a decision tree as well as all
// data attached.
BP_ERROR_T BP_DestroyCCDecisionBranch(P_BP_CC_SESSION session, P_BP_CC_DECISION_BRANCH branch);

// Destroys a decision tree as well as all
// data attached.
BP_ERROR_T BP_DestroyCCDecisionTree(P_BP_CC_SESSION session, P_BP_CC_DECISION_TREE tree);





#endif // BP_COMMAND_CONTROL_INTERFACE_H

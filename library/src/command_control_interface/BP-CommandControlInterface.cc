/*
 * BP-CommandControlInterface.cc
 *
 *  Created on: Mar 16, 2010
 *      Author: jsaon emedirso
 */

// include blockparty main
#include "../../include/BP-Main.h"

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create / Destroy CC Sessions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Create a new empty command control session.
P_BP_CC_SESSION BP_CCNewSession(P_BP_FDSET fds, char *session_name, BP_CC_SHELL_INTERFACE shell_interface)
{

	// ensure we have file descriptors
	if (!fds)
	{
		printf("\n [!!] Error: Attempted to create Command/Control session without P_FD_SET (file descriptors)");
		return NULL;
	}

	// ensure we have a session name
	if(!session_name)
	{
		printf("\n [!!] Error: Attempted to create a new Command/Control session without a session name.");
		return NULL;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create New Session and Init Defaults %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// allocate space for new session structure
	P_BP_CC_SESSION session = (P_BP_CC_SESSION) bpcalloc(sizeof(BP_CC_SESSION), 1);
	if (!session)
		return NULL;

	// create a new random id
	size_t random_id = BP_GenRandomUnsignedLong(1, 0xffffffff);

	// create new allocator
	session->m = (P_BP_LINKL) BP_CreateLinkL("cc session allocator", BP_TRUE);

	// set session name (max length 512)
	session->session_name = tq_strndup(session->m, session_name, 512, BPLN);

	// set shell interface
	session->shell_type = shell_interface;

	// set file descriptor set
	session->fds = fds;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Default Colorization/Display Parameters %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// colorize output by default
	session->colorize_display_output = BP_TRUE;

	// set stdout colors
	session->app_stdout_colors.bg = BP_COLOR_CODE_BG_BLUE;
	session->app_stdout_colors.fg = BP_COLOR_CODE_FG_WHITE;

	// set stderr colors
	session->app_stderr_colors.bg = BP_COLOR_CODE_BG_RED;
	session->app_stderr_colors.fg = BP_COLOR_CODE_FG_WHITE;

	// set stdin colors
	session->app_stdin_colors.bg = BP_COLOR_CODE_BG_GREEN;
	session->app_stdin_colors.fg = BP_COLOR_CODE_FG_WHITE;



	// return filled out session
	return session;

}


// Destroys a session, freeing all associated memory, including
// the session structure itself.
BP_ERROR_T BP_DestroyCCSession(P_BP_CC_SESSION session)
{

	if(!session)
		return ERR_FAILURE;

	// destroy session name if so set
	if(session->session_name)
	{
		tq_free(session->m, session->session_name);
		session->session_name = NULL;
	}


	// destroy information searches
	BP_DestroyCCInformationSearches(session);

	// destroy exit searches
	BP_DestroyCCExitSearches(session);

	// destroy the entire tree
	// BP_DestroyCCDecisionTree(session, session->);
	if(session->commands)
	{
		size_t n = 0;
		for(; session->commands[n]; n++)
		{
			BP_DestroyCCDecisionTree(session, session->commands[n]);
			session->commands[n] = NULL;
		}

	}

	if(session->m)
	{
		if(session->m->total_allocations)
		{
			printf("\n [+] Programmer Notice : Destroying CC session with chunks left (chunks freed automatically here).");
			BP_DisplayLinkL(session->m);
		}
	}

	// destroy allocated chunks
	BP_DestroyLinkL(session->m, BP_TRUE);

	// set memory allocator to null
	session->m = NULL;

	// free the session
	bpfree(session);

	// return indicating success
	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Adding Try/Expect (User Interface) %%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Note:  Use these routines when developing your applications.  Steps involved are:
//   1) Create session.
//   2) Create command.
//   3) Add try/expect branches to command.
//   4) Run dispatcher.

// Adds a command.
BP_ERROR_T BP_CCAddCommand(P_BP_CC_SESSION session, char *command_name)
{

	// run basic checks
	if(!session)
		return ERR_FAILURE;
	if(!session->m)
		return ERR_FAILURE;



	// attempt to lookup the command by name
	if(BP_CCGetCommandByName(session, command_name))
		return ERR_FAILURE;

	// ensure name is reasonable/printable
	if(!BP_StringIsReasonablePrintableString(command_name, bpstrlen(command_name), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;



	// bprealloc command structure to size
	session->commands = (P_BP_CC_DECISION_TREE *) tq_realloc
	(
		session->m,
		session->commands,
		(
			sizeof(P_BP_CC_DECISION_TREE) * (session->commands_n+1)
		)+sizeof(P_BP_CC_DECISION_TREE),
		BPLN
	);


	// set first command as current command
	session->commands[session->commands_n] = BP_CCCreateNewDecisionTree
	(
		session,
		command_name,
		NULL
	);

	// set null terminator
	session->commands[session->commands_n + 1] = NULL;

	// increment the command count
	session->commands_n++;

	// return indicating success
	return ERR_SUCCESS;

}

// Adds a try/expect to a command.
BP_ERROR_T BP_CCAddTryExpectToCommand
(
	P_BP_CC_SESSION         session,
	char                   *command_name,
	char                   *try_buff,
	size_t                  try_buff_size,
	char                   *expect_buff,
	size_t                  expect_buff_size,
	BP_CC_MATCH_BUFF_POLICY expect_match_policy
)
{


	// devnote: trybuff is optional.  It's the buff that gets sent first before expecting
	//          something back.  Often times, especially when starting programs, you're not
	//          expected to send something before getting something back.  In those cases,
	//          just

	if(!session)
		return ERR_FAILURE;
	if(!command_name)
		return ERR_FAILURE;


	// attempt to lookup command tree
	P_BP_CC_DECISION_TREE cc_tree = BP_CCGetCommandByName(session, command_name);
	if(!cc_tree)
		return ERR_FAILURE;

	// set tmp_branch as top branch
	P_BP_CC_DECISION_BRANCH lookup_branch = cc_tree->top_branch;

	// new branch created
	P_BP_CC_DECISION_BRANCH new_branch = NULL;

	// walk to end of "yes" branches if set
	if (lookup_branch)
	{
		for (; lookup_branch->yes; lookup_branch = lookup_branch->yes) {};
	}

	// if there is no lookup branch, add new top branch
	if(!lookup_branch)
	{

		// set top branch since we have no lookup
		new_branch = BP_CCCreateNewDecisionBranch
		(
			session,
			cc_tree,
			NULL, /* do not set add branch */
			try_buff,
			try_buff_size,
			expect_buff,
			expect_buff_size,
			expect_match_policy,
			BP_CC_FAIL_FOLLOW_RESET,
			NULL,
			NULL,
			NULL,
			NULL
		);

		// set top branch since it was unset previously
		cc_tree->top_branch = new_branch;

	}
	else
	{

		// create new decision branch at end point location since lookup was ok
		new_branch = BP_CCCreateNewDecisionBranch
		(
			session,
			cc_tree,
			lookup_branch, /* this parameter is the main difference between the above call */
			try_buff,
			try_buff_size,
			expect_buff,
			expect_buff_size,
			expect_match_policy,
			BP_CC_FAIL_FOLLOW_RESET,
			NULL,
			NULL,
			NULL,
			NULL
		);

		// set the yes branch to the new branch
		lookup_branch->yes = new_branch;

	}

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Lookup Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempt to retrieve a command tree by name.  Returns NULL on bad parameters/bad lookup.
P_BP_CC_DECISION_TREE BP_CCGetCommandByName
(
	P_BP_CC_SESSION session,
	char           *command_name
)
{

	// ensure we have a session and a command name
	if(!session)
		return NULL;
	if(!session->commands)
		return NULL;
	if(!command_name)
		return NULL;

	// ensure command is reasonable printable
	if(!BP_StringIsReasonablePrintableString(command_name, bpstrlen(command_name), BP_FALSE, BP_FALSE))
		return NULL;

	// iterate through commands and find our command
	size_t n = 0;
	for
	(
		;
		n < session->commands_n;
		n++
	)
	{

		// if we have a command name match, return the command tree pointer
		if(memcmp(session->commands[n]->command, command_name, bpstrlen(session->commands[n]->command)) == 0)
			return session->commands[n];

	}

	// return indicating that we failed our command lookup
	return NULL;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Decision Trees (For Internal Blockparty Use) %%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Note: Don't use these routines when writing your applications.  They are subject
//       to change.

// Creates a new decision tree complete with exit/reset branch.
P_BP_CC_DECISION_TREE BP_CCCreateNewDecisionTree
(
	P_BP_CC_SESSION         session,
	char                   *command_identifier,
	P_BP_CC_DECISION_BRANCH top_branch
)
{

	if (!command_identifier)
		return NULL;

	// create empty new tree
	P_BP_CC_DECISION_TREE new_tree = (P_BP_CC_DECISION_TREE) tq_calloc(session->m, sizeof(BP_CC_DECISION_TREE), 1, BPLN);
	if (!new_tree)
		return NULL;

	// copy in command name
	new_tree->command = tq_strndup(session->m, command_identifier, 1024, BPLN);

	// set top branch if available
	new_tree->top_branch = top_branch;

	// return new tree structure
	return new_tree;

}

// Creates a new branch in a decision branch in an existing tree
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
)
{

	// allocate space for decision branch
	P_BP_CC_DECISION_BRANCH new_branch = (P_BP_CC_DECISION_BRANCH) tq_calloc(session->m, sizeof(BP_CC_DECISION_BRANCH), 1, BPLN);
	if (!new_branch)
		return NULL;

	// set tree root
	new_branch->tree = tree_root;

	// if we have a "try" buffer, set it here
	if(try_this)
	{
		new_branch->try_this      = tq_strndup(session->m, try_this, try_size, BPLN);
		new_branch->try_buff_size = try_size;
	}

	// expect this (regexp safe)
	if(expect_this)
	{
		new_branch->expect_this         = tq_strndup(session->m, expect_this, expect_size, BPLN);
		new_branch->expect_buff_size    = expect_size;
		new_branch->expect_match_policy = expect_match_policy;
	}

	// set fail behavior
	new_branch->default_fail_behavior = fail_behavior;

	// set yes no branches
	new_branch->yes = yes;
	new_branch->no  = no;

	// set custom reset/exit
	new_branch->custom_reset = custom_reset;
	new_branch->custom_exit  = custom_exit;

	// set previous pointer if provided.
	new_branch->prev = prev;

	// return the new branch
	return new_branch;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%% Dispatchers/Processors %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



// This function attempts to execute the cc command identified by command_name, with the 
// substitution arguments found in **arguments.
BP_ERROR_T BP_CCDispatcher
(
		P_BP_CC_SESSION session,
		char           *command_name,
		char           **arguments
)
{


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Basic Dispatcher Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure we have a non-null session
	if (!session)
		return ERR_FAILURE;

	// ensure we have a command name
	if(!command_name)
		return ERR_FAILURE;

	// cannot work without a set of file descriptors
	if (!session->fds)
		return ERR_FAILURE;

	// must have commands
	if (!session->commands)
		return ERR_FAILURE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Forkpty Version %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we have forpty_master set, we're working with a pty session
	if(session->fds->session_is_forkpty)
	{

		// run pty version of the CC.  This is implemented in BP-CommandAndControlInterfacePty.cc
		return BP_CCDispatcherFromPty(session, command_name, arguments);

	}





	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Standard Fork Version %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// attempt to lookup command tree before dispatching
	P_BP_CC_DECISION_TREE command_tree = BP_CCGetCommandByName(session,	command_name);
	if(!command_tree)
		return ERR_FAILURE;

	// After the command is identified, process.  Return result of processing.
	if(!BP_CCProcessCommandTree(session, command_tree->top_branch))
	{
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}

// process a branch in the command tree.  Processing is relative to buffers present (or soon to be present
// in the session.
BP_ERROR_T BP_CCProcessCommandTree(P_BP_CC_SESSION session, P_BP_CC_DECISION_BRANCH execute_branch)
{

	// ensure we have a session and execute_branch
	if (!session)
		return ERR_FAILURE;
	if (!execute_branch)
		return ERR_FAILURE;

	// Polling Fd Structures (used mostly under non-blocking scenarios
	struct pollfd stdin_poll  = {0};
	struct pollfd stdout_poll = {0};
	struct pollfd stderr_poll = {0};

	// set stdin polling settings (stdin is not polled but is here in case i'd like to poll in the future)
	stdin_poll.fd     = session->fds->stdin[1];
	stdin_poll.events = POLLOUT | POLLWRBAND;

	// set stdout polling settings
	stdout_poll.fd     = session->fds->stdout[0];
	stdout_poll.events = POLLIN;

	// set stderr polling settings
	stderr_poll.fd     = session->fds->stderr[0];
	stderr_poll.events = POLLIN;

	// polling result
	int poll_result = 0;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Send "Try" Buffer If Set %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// run the try command
	if (execute_branch->try_this)
	{

		// perform substitutions
		if(session->display_writes_to_app_stdin)
		{

			BP_printfVTColor("%s", BP_COLOR_CODE_FG_RED, BP_COLOR_CODE_BG_GREEN, execute_branch->try_this);


		}


		// write to file descriptors
		if(BP_FDWrite(session->fds->stdin[1], execute_branch->try_this, execute_branch->try_buff_size) == -1)
			return ERR_FAILURE;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Expect Some Data Back %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// rolling buffer data size
	size_t rolling_read_buffer_data_size = 512;

	// data write index
	size_t data_write_index = 0;

	// declare stdout/stderr read sizes
	size_t stdout_read_size = 0;
	size_t stderr_read_size = 0;

	// set standard err index
	size_t std_err_index = 0;

	// size check iter
	size_t check_iter = 0;

	// test the expect
	if (execute_branch->expect_this)
	{


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% First Loop: Read From STDOUT %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


		// reset iterators on loop
		data_write_index = 0;
		stdout_read_size = 0;

		// main expect loop
		do
		{


			// set size at loop entry from the write index
			execute_branch->matched_text_size = data_write_index;


			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Attempt Poll on Stdout %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// poll for some data based on the specified poll timeout
			poll_result = poll(&stdout_poll, 1, session->poll_timeout);

			// exit the loop if we have no data to read
			if(poll_result == 0)
			{
				break;
			}

			// exit if we have a poll read error
			if(poll_result == -1)
			{
				return ERR_FAILURE;
			}


			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Iter Position %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// read in inter
			if(check_iter == 4)
			if(execute_branch->matched_text_size)
			{


				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				// %%% Rolling Data Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%
				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

				// set check iterator to zero
				check_iter = 0;

				// set rolling read indexing.  Prevents having to match
				// against anything but the current rolling data.
				size_t current_read_index = 0;
				if(data_write_index >= rolling_read_buffer_data_size)
				{
					// set the current read index
					current_read_index = data_write_index - rolling_read_buffer_data_size;
				}


				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				// %%% Information Extraction Checks %%%%%%%%%%%%%%%%%%
				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

				// now check for info search strings
				size_t info_search_iter = 0;
				for
				(
						;
						info_search_iter < session->info_search_n;
						info_search_iter++
				)
				{

					// create some match check data
					BP_REGEX_MATCH_INFO info_info;
					BP_ZERO_STACK(info_info);

					// attempt information search against current text match buffer
					if
					(
							BP_MatchRegularExpressionPCRE
							(
									(char *)        &execute_branch->matched_text[current_read_index],
									bpstrlen((char *) &execute_branch->matched_text[current_read_index]),
									session->info_searches[info_search_iter].expression,
									0,
									1024,
									&info_info,
									true
							) == ERR_SUCCESS
					)
					{


						// set the match string
						char * match_str = tq_strdup(session->m, info_info.preg_matches[0].matched_string, BPLN);

						// checks a stack of strings to see if the string is already
						// in the stack.
						if
						(
								BP_StringExistsInPointerArray
								(
										session->info_searches[info_search_iter].match_strs,
										session->info_searches[info_search_iter].match_strs_n,
										match_str
								) == ERR_SUCCESS
						)
						{

							// destroy match string if necessary
							tq_free(session->m, match_str);

						}
						else
						{

							// set match strings
							session->info_searches[info_search_iter].match_strs_n++;
							session->info_searches[info_search_iter].match_strs = (char **) tq_realloc
							(
									session->m,
									session->info_searches[info_search_iter].match_strs,
									sizeof(char *) * (session->info_searches[info_search_iter].match_strs_n+1),
									BPLN
							);

							// set match string here
							session->info_searches[info_search_iter].match_strs[session->info_searches[info_search_iter].match_strs_n-1] = match_str;
							session->info_searches[info_search_iter].match_strs[session->info_searches[info_search_iter].match_strs_n]   = NULL;

							// check premature exit
							printf
							(
								"\n [+] CC Information Extraction At: (%u)(%s)",
								session->info_searches[info_search_iter].match_strs_n - 1,
								session->info_searches[info_search_iter].match_strs[session->info_searches[info_search_iter].match_strs_n - 1]
							);

							// set premature exit flag
							session->total_info_extractions++;

						}

						// destroy the regexp match info
						BP_DestroyRegexpMatchInfo(&info_info);

					}

				}

				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				// %%% Premature Exit Checks %%%%%%%%%%%%%%%%%%%%%%%%%%
				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

				// now check for premature exit strings
				size_t exit_search_iter = 0;
				for
				(
					;
					exit_search_iter < session->exit_search_n;
					exit_search_iter++
				)
				{

					// create some match check data
					BP_REGEX_MATCH_INFO exit_info;
					BP_ZERO_STACK(exit_info);

					// attempt the premature exit match
					if
					(
						BP_MatchRegularExpressionPCRE
						(
							(char *) &execute_branch->matched_text[current_read_index],
							bpstrlen((char *) &execute_branch->matched_text[current_read_index]),
							session->exit_searches[exit_search_iter].expression,
							0,
							1024,
							&exit_info,
							true
						) == ERR_SUCCESS
					)
					{

						// check premature exit
						printf("\n [+] CC Premature Exit At: (%s)(%s)", exit_info.preg_matches[0].matched_string, execute_branch->matched_text);

						// destroy the regexp match info
						BP_DestroyRegexpMatchInfo(&exit_info);

						// set premature exit flag
						session->exited_prematurely=1;
						break;
					}

					// destroy info
					BP_DestroyRegexpMatchInfo(&exit_info);

					// break loop
					if(session->exited_prematurely)
						break;

				}


				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				// %%% Decision Tree Match Checks %%%%%%%%%%%%%%%%%%%%%
				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

				// zero out the match information structure
				memset(&execute_branch->match_info, 0x00, sizeof(BP_REGEX_MATCH_INFO));

				// now that a buffer has been read, attempt to match
				if (BP_MatchRegularExpression(execute_branch->matched_text, execute_branch->matched_text_size, execute_branch->expect_this, REG_EXTENDED, 0, NULL) == ERR_SUCCESS)
				{
					break;
				}

				// break loop
				if(session->exited_prematurely)
					break;

			}


			// break loop if we've hit a premature exit search (set in the quick exit loop above)
			if(session->exited_prematurely)
				break;

			/*
			 *
			 * CPU Optimization For Later Implementation:
			 * ==========================================
			 * Using a rolling buffer/flush allocation system for reads
			 * to prevent overzealous allocations.
			 *
			 */
			execute_branch->matched_text = (char *) tq_realloc
			(
					session->m,
					execute_branch->matched_text,
					data_write_index + 2,
					BPLN
			);


			// set stdout buff
			session->stdout_buff = execute_branch->matched_text;

			// increment the byte count
			session->stdout_buff_byte_n++;


			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Read From Stdout After All Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// note:  Checks are run in a cascading fashion BEFORE reads from
			//        file descriptors.  This is to allow for a do-while pattern
			//        of implementation.


			// read data at selected position
			stdout_read_size = BP_FDRead(session->fds->stdout[0], &execute_branch->matched_text[data_write_index], 1);

			// truncate read position
			execute_branch->matched_text[data_write_index+1] = '\0';

			// display the read data if so specified
			if(session->display_reads_from_app_stdout)
				BP_printfVTColor("%s", BP_COLOR_CODE_FG_BLACK, BP_COLOR_CODE_BG_BLUE, &execute_branch->matched_text[data_write_index]);

			// iterate total
			data_write_index += stdout_read_size;

			// iterate check iterator linear
			check_iter++;

		// only loop if FDRead can read data appropriately.
		} while ((stdout_read_size != 0) && (stdout_read_size != -1));

	}
	else // !execute_branch->expect_this
	{


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% If We Expect Nothing, Read STDOUT Blindly Till Polling Fails  %%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// If there is no expect_this branch to follow, we can just keep
		// reading data till the polling stops.  This is usually done for
		// applications you don't care about output for.  Like for example
		// say you wanted to run "ls -la", this would have no expectation,
		// you'd only want the program output.




		// declare temporary read buffer
		char *stdout_tmp_arbitrary_read = NULL;

		// reset write index
		data_write_index = 0;

		// current stdout read size
		stdout_read_size=0;


		do
		{

			// poll on STDOUT
			poll_result = poll(&stdout_poll, 1, session->poll_timeout);

			if(poll_result == 0)
			{
				break;
			}

			// return indicating failure
			if(poll_result == -1)
			{
				return ERR_FAILURE;
			}

			// bprealloc the matched text buffer with some arbitrary extra padding at the end
			stdout_tmp_arbitrary_read = (char *) tq_realloc
			(
				session->m,
				stdout_tmp_arbitrary_read,
				data_write_index + 2,
				BPLN
			);

			// read data at selected position from stdout
			stdout_read_size               = BP_FDRead(session->fds->stdout[0], &stdout_tmp_arbitrary_read[data_write_index], 1);
			stdout_tmp_arbitrary_read[data_write_index+1] = '\0';

			// if we've set to display reads, display output here
			if(session->display_reads_from_app_stdout)
				BP_printfVTColor("%s", BP_COLOR_CODE_FG_BLACK, BP_COLOR_CODE_BG_BLUE, &stdout_tmp_arbitrary_read[data_write_index]);

			// iterate total
			data_write_index += stdout_read_size;


		} while (poll_result != 0 && poll_result != -1);

		// destroy the temporary read after done
		if(stdout_tmp_arbitrary_read)
			tq_free(session->m, stdout_tmp_arbitrary_read);

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Poll Stderr After Stdout %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// reset data write index
	data_write_index = 0;

	// reset read size
	stderr_read_size = 0;

	// declare temporary read
	char *tmp_stderr_read = NULL;

	// If the session hasn't exited prematurely, poll.
	if(!session->exited_prematurely)
	{

		do
		{

			// poll on the fd
			poll_result = poll(&stderr_poll, 1, session->poll_timeout_stderr);

			// exit immediately if we can't poll stdout
			if(poll_result == 0)
			{
				break;
			}

			// return indicating failure if stderr simply isn't readable
			if(poll_result == -1)
			{
				return ERR_FAILURE;
			}


			// bprealloc the matched text buffer to +1
			tmp_stderr_read = (char *) tq_realloc
			(
				session->m,
				tmp_stderr_read,
				data_write_index + 2,
				BPLN
			);

			// read data at selected position
			stderr_read_size = BP_FDRead(session->fds->stderr[0], &tmp_stderr_read[data_write_index], 1);
			tmp_stderr_read[data_write_index+1] = '\0';

			// if we're displaying reads, display the stderr buff
			if(session->display_reads_from_app_stderr)
				BP_printfVTColor("%s", BP_COLOR_CODE_FG_BLACK, BP_COLOR_CODE_BG_RED, &tmp_stderr_read[data_write_index]);

			// iterate total
			data_write_index += stderr_read_size;


		} while (poll_result != 0 && poll_result != -1);

	}


	// if we have a temporary read buffer, destroy it after polling
	if(tmp_stderr_read)
		tq_free(session->m, tmp_stderr_read);


	// Return success if the command processed without any
	// errors.
	if(session->last_matched_buffer)
	{

		tq_free(session->m, session->last_matched_buffer);
		session->last_matched_buffer = NULL;

	}

	// set the buffer as the new pointer
	session->last_matched_buffer = execute_branch->matched_text;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Recurse Decision Tree %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// run the yes execute tree if there is one
	if(execute_branch->yes)
		BP_CCProcessCommandTree(session, execute_branch->yes);



	// return indicating success
	return ERR_SUCCESS;

}

// Performs substitutions in a command string based around the {arg_#} format.
// Arg replacements start at 0.
char * BP_CCPerformSubstitutions(P_BP_CC_SESSION session, char * command_str, char ** substitutions)
{

	// check incomming parameters
	if (!command_str || !substitutions)
		return NULL;

	// if there are no substitutions, return duplicated string
	if (!*substitutions)
		return tq_strdup(session->m, command_str, BPLN);

	// duplicate command string
	char *in_buff = tq_strdup(session->m, command_str, BPLN);

	// static stack substitution buffer
	char substitute_this[128] = { 0 };

	// general purpose index used for indexing arrays
	size_t i = 0;

	// used for dealing with newly allocated ptrs below
	char *tmp_substitutions = NULL;

	// Walk individual substitutions, and perform substitutions.
	for
	(
		i = 0;
		substitutions[i];
		i++
	)
	{

		// zero out the substitution string
		memset(&substitute_this, 0x00, sizeof(substitute_this));

		// create argument substitution buffer
		snprintf((char *) &substitute_this, sizeof(substitute_this) - 1, "{arg_%u}", i);

		// run find and replace against the buffer
		tmp_substitutions = BP_RegFindReplace(in_buff, substitute_this, (char *) &substitutions[i], bpstrlen(substitutions[i]), BP_CC_MAX_SUBSTITUTIONS, 0);

		// find and replace returns null if
		if (tmp_substitutions)
		{
			tq_free(session->m, in_buff);
			in_buff = tmp_substitutions;
		}

	}

	// return string with all substitutions made
	return in_buff;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%% C&C Structure Visualization Routines %%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

BP_ERROR_T BP_CCVisualizeSession(P_BP_CC_SESSION session, size_t show_tree, size_t show_branches) {

	if (!session)
		return ERR_FAILURE;

	// display the session structure
	BP_CCPrintSession(session, show_branches);

	return ERR_SUCCESS;
}

// prints the session structure
BP_ERROR_T BP_CCPrintSession(P_BP_CC_SESSION session, size_t display_branches) {

	// check to make sure we have a session
	if (!session)
		return ERR_FAILURE;

	// command and control session
	printf("\n [+] Displaying Command & Control Session: %p", session);

	// display file descriptors if set
	if (session->fds)
	{

		// display file descriptor sets
		printf("\n\t File Descriptors Used in This CC Session: %p", session->fds);
		printf("\n\t\t stdin [0]: %u", session->fds->stdin[0]);
		printf("\n\t\t stdin [1]: %u", session->fds->stdin[1]);
		printf("\n\t\t stdout[0]: %u", session->fds->stdout[0]);
		printf("\n\t\t stdout[1]: %u", session->fds->stdout[1]);
		printf("\n\t\t stderr[0]: %u", session->fds->stderr[0]);
		printf("\n\t\t stderr[1]: %u", session->fds->stderr[1]);

	}
	else
	{

		printf("\n\t File Descriptors Used in This CC Session: No file descriptors set.");

	}

	// display session name
	if (session->session_name)
		printf("\n\t session name: %s", session->session_name);
	else
		printf("\n\t session name: None set.");

	// display last command processed
	if (session->last_command)
		printf("\n\t last command: %s", session->last_command);
	else
		printf("\n\t last command: Empty.");

	// display last run status code
	switch (session->run_status_code) {
	case ERR_FAILURE:
		printf("\n\t last run status code: ERR_FAILURE");
		break;

	case ERR_SUCCESS:
		printf("\n\t last run status code: ERR_SUCCESS");
		break;

	default:
		printf("\n\t last run status code: UNDEFINED CODE!!");
		break;

	}

	// display the last matched buffer
	if (session->last_matched_buffer)
		printf("\n\t last matched buffer:  %s", session->last_matched_buffer);
	else
		printf("\n\t last matched buffer:  BP_CC_NONE_MATCHED");

	// display the set shell type
	printf("\n\t ->session shell set as: ");

	switch (session->shell_type)
	{

		case BP_CC_SHELL_LINUX_EXECVE:
			printf("BP_CC_SHELL_LINUX_EXECVE");
			break;

		case BP_CC_SHELL_LINUX_BASH:
			printf("BP_CC_SHELL_LINUX_BASH");
			break;

		case BP_CC_SHELL_WINDOWS_GENERIC:
			printf("BP_CC_SHELL_WINDOWS_GENERIC");
			break;

		default:
			printf("UNDEFINED SHELL TYPE");
			break;

	}


	if(session->commands)
	{

		// count total number for commands found
		size_t i = 0;
		for (i = 0; session->commands[i]; i++){}


		// display command pointer and command count
		printf("\n\t Command Set: %p:", session->commands);
		printf("\n\t Command Count: %u", i);

		// if the number of records is > 0 display output
		if (i) {

			// display individual commands
			printf("\n\t Commands:");
			for (i = 0; session->commands[i]; i++) {

				BP_CCPrintTree(session->commands[i], display_branches);
			}
		}

	}
	else
	{
		printf("\n\t Commands: Empty.");
	}


	// display exit/reset branch pointers
	printf("\n\t ->exit branch:  %p", session->exit);
	printf("\n\t ->reset branch: %p", session->reset);


	// display premature exits
	printf("\n\t ->premature exits:      %p", session->exit_searches);
	printf("\n\t ->premature exit Count: %u", session->exit_search_n);


	// display information matches
	printf("\n [+] Displaying Information Extractions:");
	BP_CCPrintInformationExtractions(session->info_searches);

	// return success
	return ERR_SUCCESS;

}

// display search information as it is found in a session
BP_ERROR_T BP_CCPrintInformationExtractions(P_BP_CC_INFORMATION_SEARCHES searches)
{

	if(!searches)
		return ERR_FAILURE;

	// display information matches
	printf("\n\t [+] info matches: ");
	printf("\n\t\t expression:         %s", searches->expression);
	printf("\n\t\t expression_matched: %u", searches->expression_matched);
	printf("\n\t\t match count:        %u", searches->match_strs_n);

	size_t n = 0;
	for(; n < searches->match_strs_n; n++)
	{
		printf("\n\t\t match_str: %s", searches->match_strs[n]);
	}

	// return indicating success
	return ERR_SUCCESS;
}

// prints the tree structure.  This function is recursive based on tree pointers.
BP_ERROR_T BP_CCPrintTree(P_BP_CC_DECISION_TREE tree, size_t display_branches) {

	if (!tree)
		return ERR_FAILURE;

	// print the decision tree pointer
	printf("\n\t\t + Command Decision Tree %p: \"%s\" ", tree, tree->command);

	// index variable
	size_t i = 0;

	// process arguments
	if (tree->arguments) {

		// count arguments
		for (i = 0; tree->arguments[i]; i++) {
		}

		// display arguments
		for (i = 0; tree->arguments[i]; i++) {

			// display argument
			printf("\n\t\t\t {argument_%u} = %s", i, tree->arguments[i]);

		}

	}

	// display the last matched buffer if set
	if (tree->last_matched_buffer && tree->last_matched_buffer_size) {
		printf("\n\t\t\t ->last matched buffer:      %s", tree->last_matched_buffer);
		printf("\n\t\t\t ->last matched buffer size: %u", tree->last_matched_buffer_size);
	}

	printf("\n\t\t\t -> Top Branch Pointer: %p", tree->top_branch);

	// display the branches if set
	if (display_branches)
		BP_CCPrintBranches(tree->top_branch);

	// return success
	return ERR_SUCCESS;

}

// recursive function that prints out all possible tree branches (starting with yes)
BP_ERROR_T BP_CCPrintBranches(P_BP_CC_DECISION_BRANCH branch) {

	if (!branch)
		return ERR_FAILURE;

	printf("\n\t\t\t [+] Branch %p", branch);

	if (branch->try_this) {
		printf("\n\t\t\t -> try this:      %s", branch->try_this);
		printf("\n\t\t\t -> try data size: %u", branch->try_buff_size);
	}

	if (branch->expect_this) {
		printf("\n\t\t\t -> expect this:      %s", branch->expect_this);
		printf("\n\t\t\t -> expect data size: %u", branch->expect_buff_size);
	}

	printf("\n\t\t\t -> default fail behavior: ");
	switch (branch->default_fail_behavior) {

	case BP_CC_FAIL_FOLLOW_CUSTOM_EXIT:
		printf("BP_CC_FAIL_FOLLOW_CUSTOM_EXIT");
		break;

	case BP_CC_FAIL_FOLLOW_CUSTOM_RESET:
		printf("BP_CC_FAIL_FOLLOW_CUSTOM_RESET");
		break;

	case BP_CC_FAIL_FOLLOW_EXIT:
		printf("BP_CC_FAIL_FOLLOW_EXIT");
		break;

	case BP_CC_FAIL_FOLLOW_RESET:
		printf("BP_CC_FAIL_FOLLOW_RESET");
		break;

	case BP_CC_FAIL_FOLOW_NO:
		printf("BP_CC_FAIL_FOLOW_NO");
		break;

	default:
		break;
	}

	printf("\n\t\t\t -> branch belongs to tree:  %p", branch->tree);
	printf("\n\t\t\t -> previous branch in tree: %p", branch->prev);

	printf("\n\t\t\t -> custom reset: %p", branch->custom_reset);
	printf("\n\t\t\t -> custom exit:  %p", branch->custom_exit);
	printf("\n");

	// display yes branch if set
	if (branch->yes) {
		BP_CCPrintBranches(branch->yes);
	}

	// display no branch if set
	if (branch->no) {
		BP_CCPrintBranches(branch->no);
	}

	// return success
	return ERR_SUCCESS;
}

// Add a premature exit search to the session.  This will cause
// the application to search the application output for the provided
// regexp, and exit in the case it's encountered.  You can add as many
// expressions as you want.  This application uses PCRE (new) not POSIX
// regular expressions.
BP_ERROR_T BP_CCAddPrematureExitTestString(P_BP_CC_SESSION session, char * expression_pcre)
{

	// check parameters
	if(!session)
		return ERR_FAILURE;
	if(!expression_pcre)
		return ERR_FAILURE;


	// increment exit search
	session->exit_search_n++;

	// allocate space for searches
	session->exit_searches = (P_BP_CC_EXIT_SEARCHES) tq_realloc
	(
		session->m,
		session->exit_searches,
		sizeof(BP_CC_EXIT_SEARCHES)*(session->exit_search_n+1),
		BPLN
	);


	// zero out new structures
	memset
	(
			(void *)&session->exit_searches[session->exit_search_n-1],
			0x00,
			sizeof(BP_CC_EXIT_SEARCHES)*2
	);


	// set the expression in the structure
	session->exit_searches[session->exit_search_n-1].expression = (char *) tq_strdup
	(
		session->m,
		expression_pcre,
		BPLN
	);

	// return indicating success
	return ERR_SUCCESS;

}

// Adds an expression which will be used to match command output to specific data.
BP_ERROR_T BP_CCAddInformationExtractionRegexpString(P_BP_CC_SESSION session, char * expression_pcre)
{

	if(!session)
		return ERR_FAILURE;
	if(!expression_pcre)
		return ERR_FAILURE;

	// increment info search
	session->info_search_n++;

	// allocate space for searches
	session->info_searches = (P_BP_CC_INFORMATION_SEARCHES) tq_realloc(session->m, session->info_searches, sizeof(BP_CC_INFORMATION_SEARCHES)*(session->info_search_n+1), BPLN);

	// zero out new structures
	memset((void *)&session->info_searches[session->info_search_n-1], 0x00, sizeof(BP_CC_INFORMATION_SEARCHES)*2);

	// set the expression in the structure
	session->info_searches[session->info_search_n-1].expression = (char *) tq_strdup(session->m, expression_pcre, BPLN);

	// return indicating success
	return ERR_SUCCESS;


}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%% CC Data Destruction %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Destroys information searches
BP_ERROR_T BP_DestroyCCInformationSearches(P_BP_CC_SESSION session)
{

	// ensure there is a session passed in (contains allocator)
	if(!session)
		return ERR_FAILURE;

	// ensure we have a set of chunks to free/unlink
	if(!session->info_searches)
		return ERR_FAILURE;

	// walk the array and destroy data where necessary
	size_t n = 0;
	for(; n < session->info_search_n; n++)
	{

		// destroy expression
		if(session->info_searches[n].expression)
		{

			// free the data
			tq_free(session->m, session->info_searches[n].expression);

			// set data members
			session->info_searches[n].expression         = NULL;
			session->info_searches[n].expression_matched = 0;

		}

		// destroy match str if necessary
		if(session->info_searches[n].match_strs)
		{

			size_t j = 0;
			for(; j < session->info_searches[n].match_strs_n; j++)
			{

				// free the match str if necessary
				tq_free(session->m, session->info_searches[n].match_strs[j]);

			}

			// set values
			session->info_searches[n].match_strs   = NULL;
			session->info_searches[n].match_strs_n = 0;
			session->info_searches[n].match_len    = 0;

		}

	}

	// destroy the structure itself
	if(session->info_searches)
	{
		tq_free(session->m, session->info_searches);
		session->info_searches = NULL;
	}

	// return indicating success
	return ERR_SUCCESS;

}

// Destroy Exit Searches
BP_ERROR_T BP_DestroyCCExitSearches(P_BP_CC_SESSION session)
{

	// ensure there is a session passed in (contains allocator)
	if(!session)
		return ERR_FAILURE;

	// ensure we have a set of chunks to free/unlink
	if(!session->exit_searches)
		return ERR_FAILURE;

	// walk the array and destroy data where necessary
	size_t n = 0;
	for(; n < session->exit_search_n; n++)
	{

		// destroy expression
		if(session->exit_searches[n].expression)
		{

			// free the data
			tq_free(session->m, session->exit_searches[n].expression);

			// set data members
			session->exit_searches[n].expression         = NULL;
			session->exit_searches[n].expression_matched = 0;

		}

		// destroy match str if necessary
		if(session->exit_searches[n].match_str)
		{

			// free the match str if necessary
			tq_free(session->m, session->exit_searches[n].match_str);

			// set values
			session->exit_searches[n].match_str = NULL;
			session->exit_searches[n].match_len = 0;

		}

	}

	// destroy the structure itself
	if(session->exit_searches)
	{
		tq_free(session->m, session->exit_searches);
		session->exit_searches = NULL;
	}

	// return indicating success
	return ERR_SUCCESS;


}

// Destroys a specific decision tree branch.
// Destroys a decision tree as well as all
// data attached.
BP_ERROR_T BP_DestroyCCDecisionBranch(P_BP_CC_SESSION session, P_BP_CC_DECISION_BRANCH branch)
{

	if(!session)
		return ERR_FAILURE;
	if(!branch)
		return ERR_FAILURE;

	if(branch)
	{
		if(branch->custom_exit)
			BP_DestroyCCDecisionBranch(session, branch->custom_exit);
		if(branch->custom_reset)
			BP_DestroyCCDecisionBranch(session, branch->custom_reset);
		if(branch->no)
			BP_DestroyCCDecisionBranch(session, branch->no);
		if(branch->yes)
			BP_DestroyCCDecisionBranch(session, branch->yes);

	}

	// return indicating success
	return ERR_SUCCESS;
}

// Destroys a decision tree as well as all
// data attached.
BP_ERROR_T BP_DestroyCCDecisionTree(P_BP_CC_SESSION session, P_BP_CC_DECISION_TREE tree)
{

	// check tree
	if(!tree)
		return ERR_FAILURE;

	// destroy the command string if necessary
	if(tree->command)
	{
		tq_free(session->m, tree->command);
		tree->command = NULL;
	}
	if(tree->last_matched_buffer)
	{
		tq_free(session->m, tree->last_matched_buffer);
		tree->last_matched_buffer = NULL;
		tree->last_matched_buffer_size = 0;
	}

	if(tree->arguments)
	{
		// TREE ARGUMENTS ARE NOT DUPLICATED AND SHOULD
		// NOT BE FREE'D
	}

	// check for the top branch, if it exists, destroy the whole tree
	if(tree->top_branch)
	{
		BP_DestroyCCDecisionBranch(session, tree->top_branch);
		tq_free(session->m, tree);

	}

	// return indicating success
	return ERR_SUCCESS;

}



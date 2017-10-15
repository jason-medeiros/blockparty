/*
 * BP-CommandControlInterfacePty.cc
 *
 *  Created on: Dec 22, 2015
 *      Author: root
 */

// include blockparty main
#include "../../include/BP-Main.h"

// This function attempts to execute a command identified by command_name.  It differs from
// CCDispatcher in that this routine is designed to work with descriptors which come from
// forkpty().  Using forkpty() is slower than simple fork(), but it allows us the ability to
// control applications which require a terminal to be present.
BP_ERROR_T BP_CCDispatcherFromPty
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

	// if we don't have a master, return failure
	if(!session->fds->session_is_forkpty)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Standard Ptyfork Version %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// attempt to lookup command tree before dispatching
	P_BP_CC_DECISION_TREE command_tree = BP_CCGetCommandByName(session,	command_name);
	if(!command_tree)
		return ERR_FAILURE;


	// After the command is identified, process.  Return result of processing.
	if(!BP_CCProcessCommandTreePty(session, command_tree->top_branch))
	{
		return ERR_FAILURE;
	}

    // return indicating success
    return ERR_SUCCESS;

}


// process a branch in the command tree.  Processing is relative to buffers present (or soon to be present
// in the session.
BP_ERROR_T BP_CCProcessCommandTreePty(P_BP_CC_SESSION session, P_BP_CC_DECISION_BRANCH execute_branch)
{

	// ensure we have a session and execute_branch
	if (!session)
		return ERR_FAILURE;
	if (!execute_branch)
		return ERR_FAILURE;


	// read fd set
	fd_set rfd;
	BP_ZERO_STACK(rfd);

	// write fdset
	fd_set wfd;
	BP_ZERO_STACK(wfd);

	// error fdset
	fd_set efd;
	BP_ZERO_STACK(efd);

	// timeval for polling
	timeval tv;
	BP_ZERO_STACK(tv);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run "Try" By Writing To Process STDIN %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



	// run the try command
	if (execute_branch->try_this)
	{

		FD_ZERO(&rfd);
		FD_ZERO(&wfd);
		FD_ZERO(&efd);

		// set fds before write
		FD_SET(session->fds->forkpty_master, &rfd);

		// perform substitutions
		if(session->display_writes_to_app_stdin)
		{
			BP_printfVTColor("%s", BP_COLOR_CODE_FG_RED, BP_COLOR_CODE_BG_GREEN, execute_branch->try_this);
		}

		// write try command to fd
		if(BP_FDWrite(session->fds->forkpty_master, execute_branch->try_this, execute_branch->try_buff_size) == -1)
		{
			return ERR_FAILURE;
		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run "Expect" By Reading Child Process STDOUT %%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this is set to true if the application read failed
	BP_BOOL expect_failed_due_to_bad_app_read = BP_FALSE;

	// buffer read from app
	char  *read_from_app_buff = NULL;
	size_t read_from_app_buff_n = 0;

	// if we're expecting data, attempt to read data
	if(execute_branch->expect_this)
	{

		printf("\n I MIGHT VOMIT");
		// data write index
		size_t data_write_index = 0;

		// declare stdout/stderr read sizes
		int stdout_read_size = 0;
		int stderr_read_size = 0;

		// set standard err index
		size_t std_err_index = 0;

		// size check iter
		size_t check_iter = 0;

		// create stack read buffer (1kb)
		char read_char_buff [BP_KILOBYTE+1];

		// main read loop for expect
		for(;;)
		{

			// zero the read and write buffers on loop
			if(stdout_read_size)
				BP_ZERO_STACK(read_char_buff);

			// zero all the file descriptor sets
			FD_ZERO(&rfd);
			FD_ZERO(&wfd);
			FD_ZERO(&efd);

			// set master
			FD_SET(session->fds->forkpty_master, &rfd);

			// note: this makes it keyboard interactive
			// FD_SET(STDIN_FILENO, &rfd);

			// set polling timeouts
			tv.tv_sec  = session->select_child_stdout_timeout_sec;
			tv.tv_usec = session->select_child_stdout_timeout_usec;

			// select on fds
			int select_val = select
			(
					session->fds->forkpty_master + 1,
					&rfd,
					&wfd,
					&efd,
					&tv
			);

			// if we can no longer select, break the loop
			if(select_val == -1)
				break;

			// if we have no fds to read from, break loop
			if(select_val == 0)
				break;

			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Read From Application %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// Check for data to receive; the received
			// data includes also the data previously sent
			// on the same master descriptor (line 90).
			if (FD_ISSET(session->fds->forkpty_master, &rfd))
			{

				// attempt to read a kilobyte
				stdout_read_size = read(session->fds->forkpty_master, &read_char_buff[0], BP_KILOBYTE);

				// printf("\n Got read size?: %i", stdout_read_size);
				if ( stdout_read_size != -1)
				{

					// perform reallocation based on current size + read size
					read_from_app_buff = (char *) bprealloc
					(
							read_from_app_buff,
							read_from_app_buff_n + stdout_read_size + 1
					);

					// copy in read data at read position
					memcpy(&read_from_app_buff[read_from_app_buff_n], &read_char_buff, stdout_read_size);

					// increment the read size by the size of the data read from stdout
					read_from_app_buff_n += stdout_read_size;


				}
				else
				{

					/*
					// since we get a bad read, check if the process exited, if the process exited
					// it's not actually a bad read (bug fixed on 07/2016)

					// if the pid is not running (checked here with kill), and we got a -1 on read, something
					// has gone wrong and we can no longer trust our results.
					if(kill(session->fds->pid, 0) == 0)
					{
						printf("\n Fudge you, you fudge: %i - %s", session->fds->pid, read_from_app_buff);
						expect_failed_due_to_bad_app_read = BP_TRUE;
					}
					*/
					// set expect failure
					break;

				}

			}

		}


	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Expect Match Checks  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Create expect checks indicator.  This is initialized to true, and only
	// set to false if one or more checks in the cascading if/else below fails.
	BP_BOOL expect_checks_passed_ok = BP_TRUE;


	// run the "expect" branch
	if(execute_branch->expect_this)
	{

		// display the read buffer
		if(read_from_app_buff)
		{

			// display reads if set
			if(session->display_reads_from_app_stdout)
			{
				BP_printfVTColor("\n%s", BP_COLOR_CODE_FG_WHITE, BP_COLOR_CODE_BG_BLUE, read_from_app_buff);
				BP_RESET_TERMINAL_COLOR;
			}

			// If we're expecting some data back from the application, go ahead and run expect checks.
			if(execute_branch->expect_this)
			{

				/* run memmem to search buffer */
				if(execute_branch->expect_match_policy == BP_CC_MATCH_BUFF_POLICY_MEM_SEARCH)
				{

					// run length check disqualifier first
					if(expect_checks_passed_ok)
					if(read_from_app_buff_n < execute_branch->expect_buff_size)
						expect_checks_passed_ok = BP_FALSE;

					// run search disqualifier next
					if(expect_checks_passed_ok)
					if(!memmem(read_from_app_buff, read_from_app_buff_n, execute_branch->expect_this, execute_branch->expect_buff_size))
						expect_checks_passed_ok = BP_FALSE;

				}
				else if(execute_branch->expect_match_policy == BP_CC_MATCH_BUFF_POLICY_MEM_EXACT)
				{

					/* run memcmp to exact-match buffer */

					// run length check disqualifier first
					if(expect_checks_passed_ok)
					if(read_from_app_buff_n != execute_branch->expect_buff_size)
						expect_checks_passed_ok = BP_FALSE;

					// run memcmp disqualifier next
					if(expect_checks_passed_ok)
					if(memcmp(read_from_app_buff, execute_branch->expect_this, read_from_app_buff_n) != 0)
						expect_checks_passed_ok = BP_FALSE;


				}
				else
				/* run PCRE to search buffer */
				if(execute_branch->expect_match_policy == BP_CC_MATCH_BUFF_POLICY_PCRE_SEARCH)
				{

					// zero out our match info before use
					BP_ZERO_STACK(execute_branch->match_info);

					// attempt to match pcre on our buffer
					if(expect_checks_passed_ok)
					if
					(
						BP_MatchRegularExpressionPCRE
						(
							(char *) read_from_app_buff,
							read_from_app_buff_n,
							execute_branch->expect_this,
							0,
							1024,
							&execute_branch->match_info,
							BP_TRUE
						) != ERR_SUCCESS
					)
					{

						// set expect checks flag to false/fail
						expect_checks_passed_ok = BP_FALSE;

						// destroy the regexp match info if we've failed match
						BP_DestroyRegexpMatchInfo(&execute_branch->match_info);


					}

				}

				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				// %%% After Match Checks Complete/Store Buffers %%%%
				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

				// ensure checks passed ok before setting buffers
				if(expect_checks_passed_ok)
				{

					// destroy matched buffer if necessary before re-assigning
					if(execute_branch->matched_text)
						tq_free(session->m, execute_branch->matched_text);

					// set buffers directly (since they've been tq allocated
					execute_branch->matched_text      = read_from_app_buff;
					execute_branch->matched_text_size = read_from_app_buff_n;

					// set the last success branch
					session->last_expect_match_success_branch = execute_branch;

				}

			}

		}

		// if the expect checks failed, exit here
		if(!expect_checks_passed_ok)
		{

			// Destroy the application read buffer if we haven't attached it to
			// an execution branch.
			if(read_from_app_buff)
				tq_free(session->m, read_from_app_buff);

			// display check failed message
			printf("\n [!!] Expect check failed for: %s", execute_branch->expect_this);

			// set last failed branch
			session->last_expect_match_failed_branch = execute_branch;

			// return indicating failure
			return ERR_FAILURE;

		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Recurse Decision Tree %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// run the yes execute tree if there is one
	if(expect_failed_due_to_bad_app_read)
	{

		// set last failed branch before exiting
		session->last_expect_match_failed_branch = execute_branch;
		return ERR_FAILURE;

	}

	// Branch to yes if we've passed our checks.  Branch to no if we failed our checks.
	if(execute_branch->yes && expect_checks_passed_ok)
	{
		return BP_CCProcessCommandTreePty(session, execute_branch->yes);
	}
	else if(execute_branch->no && !expect_checks_passed_ok)
	{
		return BP_CCProcessCommandTreePty(session, execute_branch->no);
	}

	// return ERR_SUCCESS after recursion bottoms.
	return ERR_SUCCESS;

}

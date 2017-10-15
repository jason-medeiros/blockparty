#include "../../include/BP-Main.h"

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Open Connection / Close Connection %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// MySQL Connect Bindings
P_BP_MYSQL_SESSION BP_MySQLConnect(char *mysqlHost, char *mysqlUser, char *mysqlPass, char *mysqlDB, size_t port)
{

	// ensure mysql host was provided
	if(!mysqlHost)
		return NULL;

	// ensure mysql user was provided
	if(!mysqlUser)
		return NULL;

	// ensure mysql password was provided
	if(!mysqlPass)
		return NULL;

	// enusre db was provided
	if(!mysqlDB)
		return NULL;

	// ensure port was provided
	if(!port)
		return NULL;


	// allocate new mysql session structure
	P_BP_MYSQL_SESSION mysqlSession = (P_BP_MYSQL_SESSION) bpcalloc(sizeof(BP_MYSQL_SESSION), 1);
	if(!mysqlSession)
		return NULL;

	// create new system hreg (currently used mostly for critical section locking, added sept/2015). Don't
	// enter critical section here, as the structure is not in danger of contention until this function returns
	// a valid pointer.
	mysqlSession->system_hreg = BP_CreateHashTableRegistry();

	// destroy the session and return null if we couldn't create our system hreg (oom)
	if(!mysqlSession->system_hreg)
	{
		BP_MySQLClose(mysqlSession);
		return NULL;
	}

	// allocate space for new uuid
	mysqlSession->uuid = (char *) ht_calloc(mysqlSession->system_hreg, BP_UUID_LEN+1, 1);

	// create and store new uuid
	BP_GenRandomUUID_StoreInBuff(mysqlSession->uuid);


	// set the default behavior of this session to display errors immediately on generation
	mysqlSession->display_errors_immediately = BP_TRUE;

	// enable lock timeout transaction retries as default (safe behavior, don't disable unless
	// really necessary)
	mysqlSession->restart_lock_timeout_transactions = BP_TRUE;

	// create a new tailqueue allocator.
	mysqlSession->tq = BP_CreateLinkL("mysql tq", BP_FALSE);
	if(!mysqlSession->tq)
	{

		// destroy the mysql session here if we couldn't allocate
		// the tq.
		BP_MySQLClose(mysqlSession);

		// return indicating failure
		return NULL;

	}

	// initialize semaphore for connection thread safety
	sem_init(&mysqlSession->db_lock, 1, 1);

	// create a conn object from mysql_init
	mysqlSession->conn = mysql_init(NULL);

	// FLAG FOR SSL CAN BE SET HERE OR SET BEFORE USING mysql_ssl_set()
	mysqlSession->conn = mysql_real_connect
	(
		mysqlSession->conn,
		mysqlHost,
		mysqlUser,
		mysqlPass,
		mysqlDB,
		0,
		0,
		CLIENT_IGNORE_SIGPIPE | CLIENT_MULTI_STATEMENTS
	);

	// ensure we have a connection
	if(!mysqlSession->conn)
	{

		// set last connection as failed
		mysqlSession->last_connection_failed = BP_TRUE;

		// add error
		BP_MySQLAddErr(mysqlSession, BP_MYSQL_ERROR_CONNECT, 0, "Connection to database failed.  Check credentials/host/port.", BPLN);

		// return the session anyway (as it contains relevant data)
		return mysqlSession;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Connection Completed / Set Structure Values %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set true as connected
	mysqlSession->connected = true;
	mysqlSession->host = (char *) tq_strdup(mysqlSession->tq, mysqlHost, BPLN);
	mysqlSession->user = (char *) tq_strdup(mysqlSession->tq, mysqlUser, BPLN);
	mysqlSession->pass = (char *) tq_strdup(mysqlSession->tq, mysqlPass, BPLN);
	mysqlSession->db   = (char *) tq_strdup(mysqlSession->tq, mysqlDB,   BPLN);
	mysqlSession->port = port;

	// set autocommit
	// mysql_autocommit(mysqlSession->conn, true);

	// return the new session
	return mysqlSession;

}

// destroys structures and closes connection (Must be heap pointer)
BP_ERROR_T BP_MySQLClose(P_BP_MYSQL_SESSION mysqlSession)
{

	// ensure we have a session
	if(!mysqlSession)
		return ERR_FAILURE;

	// ensure we have a hreg to lock
	if(!mysqlSession->system_hreg)
	{
		#if DEBUG_MYSQL_OUTPUT == BP_TRUE
		printf("\n [DB-DBG] Tried to close a handle that didn't have a valid system hash registry (used for locking)");
		return ERR_FAILURE;
		#endif
	}

	// enter critical section (thread safety)
	BP_MYSQL_ENTER_CRITICAL_SECTION;

	// ensure connected status
	if(!mysqlSession->connected || mysqlSession->server_gone_away || mysqlSession->server_lost_connection)
	{
		#if DEBUG_MYSQL_OUTPUT == BP_TRUE
		printf("\n [DB-DBG] Server is not connected.  Cannot use this routine without valid connection.");
		#endif

		// enter critical section (thread safety)
		BP_MYSQL_LEAVE_CRITICAL_SECTION;

		// return indicating failure
		return ERR_FAILURE;

	}

	// if we have a connection
	if(mysqlSession->conn)
	{

		#if DEBUG_MYSQL_OUTPUT == BP_TRUE
		printf("\n [DB-DBG] Closing actual MySQL connection: %p, %p", mysqlSession->conn, mysqlSession);
		#endif


		// optional debug breakpoint
		// asm("int3");

		// attempt to close the connection
		mysql_close(mysqlSession->conn);

		// exit the thread (fixes a memory leak, this is required)
		mysql_thread_end();

	}

	// destroy semaphore before exit
	sem_destroy(&mysqlSession->db_lock);

	// destroy the linkl
	BP_DestroyLinkL(mysqlSession->tq, BP_TRUE);

	// leave critical section (thread safety)
	BP_MYSQL_LEAVE_CRITICAL_SECTION;

	// destroy the hash table registry
	BP_DestroyHashTableRegistry(mysqlSession->system_hreg);

	// free the mysql session
	bpfree(mysqlSession);

	// return indicating success
	return ERR_SUCCESS;

}


// ============== Thread Safety / Locks ======================================

// lock to ensure no other items can use the session (IF LOCKED, UNLOCKE REQUIRED)
BP_ERROR_T BP_MySQLSemLock  (P_BP_MYSQL_SESSION mysqlSession)
{

	// ensure we have a session pointer
	if(!mysqlSession)
		return ERR_FAILURE;

	// sem_wait returns non-zero on failure
	if(sem_wait(&mysqlSession->db_lock))
		return ERR_FAILURE;

	// if our wait succeeds, return success
	return ERR_SUCCESS;

}


// unlocks a session
BP_ERROR_T BP_MySQLSemUnlock(P_BP_MYSQL_SESSION mysqlSession)
{

	// ensure we have a session pointer
	if(!mysqlSession)
		return ERR_FAILURE;

	// sem_wait returns non-zero on failure
	if(sem_post(&mysqlSession->db_lock))
		return ERR_FAILURE;

	// if our wait succeeds, return success
	return ERR_SUCCESS;

}


// Use With null terminated strings only, results are accessed through struct MYSQL pointer
int BP_MySQLQuery(P_BP_MYSQL_SESSION mysqlSession, char * str)
{

	// check for session, exit with static value if not
	if(!mysqlSession)
	{
		return BP_MYSQL_QUERY_ERROR_CODE_NO_SESSION;
	}

	// enter critical section (thread safety)
	BP_MYSQL_ENTER_CRITICAL_SECTION;

	// ensure connected status
	if(!mysqlSession->connected || mysqlSession->server_gone_away || mysqlSession->server_lost_connection)
	{

		// display notice (nota debug notice, this is a legit notice)
		printf("\n [!!] Server is not connected.  Cannot use this routine without valid connection.");

		// leave critical section (thread safety)
		BP_MYSQL_LEAVE_CRITICAL_SECTION;

		// code is not connected
		return BP_MYSQL_QUERY_ERROR_CODE_NOT_CONNECTED;

	}

	// run the query and gather the return value
	int retVal =  mysql_query(mysqlSession->conn, str);

	// leave critical section before returning
	BP_MYSQL_LEAVE_CRITICAL_SECTION;

	// return from function
	return retVal;

}


// Binary safe version, string must mach length in length , results are accessed through struct MYSQL pointer
int BP_MySQLRealQuery(P_BP_MYSQL_SESSION mysqlSession, char * str, unsigned long length)
{

	// check for session, exit with static value if not
	if(!mysqlSession)
	{
		return BP_MYSQL_QUERY_ERROR_CODE_NO_SESSION;
	}

	// enter critical section (thread safety)
	BP_MYSQL_ENTER_CRITICAL_SECTION;

	// ensure connected status
	if(!mysqlSession->connected || mysqlSession->server_gone_away || mysqlSession->server_lost_connection)
	{

		// add error to stack
		BP_MySQLAddErr(mysqlSession, BP_MYSQL_ERROR_PREPARE, 0, "Server is not connected.  Cannot use this routine without valid connection.", BPLN);

		// leave critical section before returning
		BP_MYSQL_LEAVE_CRITICAL_SECTION;
		return BP_MYSQL_QUERY_ERROR_CODE_NOT_CONNECTED;

	}

	// run the query
	int retVal =  mysql_real_query(mysqlSession->conn, str, length);

	// leave critical section before returning
	BP_MYSQL_LEAVE_CRITICAL_SECTION;

	// return the value returned via the query
	return retVal;
}


// runs a stack of queries and removes results, parsing them into the correct
// storage syntax.  YOU NEED TO USE THIS FUNCTION IF IT"S A MULTI QUERY, OR ELSE.
BP_ERROR_T BP_MySQLQueryMulti(P_BP_MYSQL_SESSION mysqlSession, char *query)
{

	// set mystl session
	if(!mysqlSession)
		return ERR_FAILURE;

	// enter critical section before returning
	BP_MYSQL_ENTER_CRITICAL_SECTION;

	// ensure we have a connection pointer
	if(!mysqlSession->conn)
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// ensure we're actually connected
	if(!mysqlSession->connected)
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// ensure we have a query
	if(!query)
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// ensure the query is reasonable printable string
	if(!BP_StringIsReasonablePrintableString(query, bpstrlen(query), BP_TRUE, BP_TRUE))
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// prepare raw
	BP_MySQLQuery
	(
		mysqlSession,
		(char *) query
	);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Required Result Cleanup %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// query status
	int status = 0;

	// process required multiquery cleanup
	do
	{

		/* did current statement return data? */
		MYSQL_RES * result = mysql_store_result(mysqlSession->conn);
		if (result)
		{
			mysql_free_result(result);
		}
		else
		{

			if (mysql_field_count(mysqlSession->conn) == 0)
			{
			}
			else
			{
				break;
			}

		}

		/* more results? -1 = no, >0 = error, 0 = yes (keep looping) */
		if ((status = mysql_next_result(mysqlSession->conn)) > 0)
		{
			// do nothing
		}

	} while (status == 0);

	// return indicating success
	BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}


// Returns stored results after a query, NULL on failure
P_BP_MYSQL_RESULT BP_MySQLStoreResult(P_BP_MYSQL_SESSION mysqlSession)
{

	// check for session, exit with static value if not
	if(!mysqlSession)
		return NULL;
	if(!mysqlSession->system_hreg)
		return NULL;

	// enter critical section before returning
	BP_MYSQL_ENTER_CRITICAL_SECTION;

	// ensure connected status
	if(!mysqlSession->connected || mysqlSession->server_gone_away || mysqlSession->server_lost_connection)
	{

		// add error
		BP_MySQLAddErr(mysqlSession, BP_MYSQL_ERROR_PREPARE, 0, "Tried to store results with no valid connection.", BPLN);

		// return null
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	}

	// create new result set and zero memory
	P_BP_MYSQL_RESULT resultSet = (P_BP_MYSQL_RESULT) tq_calloc(mysqlSession->tq, sizeof(BP_MYSQL_RESULT), 1, BPLN);

	// store results from the mysql connection
	resultSet->set = mysql_store_result(mysqlSession->conn);

	// check for result set
	if(resultSet->set == NULL)
	{

		// destroy and return null if unset
		tq_free(mysqlSession->tq, resultSet);
		BP_MySQLAddErr(mysqlSession, BP_MYSQL_ERROR_PREPARE, 0, "Couldn't store result.", BPLN);

		// return null
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	}

	// set the number of fields in the result
	resultSet->numFields = mysql_num_fields(resultSet->set);

	// return the result set
	BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(resultSet);

}

// Returns a result set from a query, MYSQL_ROW is a typedefed
// 2dimensional character array.  Number of fields returned is stored
// in result earlier.
MYSQL_ROW BP_MySQLFetchRow(P_BP_MYSQL_RESULT result, P_BP_MYSQL_SESSION mysqlSession)
{

	// ensure we have a mysql session
	if(!mysqlSession)
		return NULL;
	if(!mysqlSession->system_hreg)
		return NULL;

	// enter critical section before returning
	BP_MYSQL_ENTER_CRITICAL_SECTION;

	// ensure we have a result
	if(!result)
	{
		BP_MySQLAddErr(mysqlSession, BP_MYSQL_ERROR_PREPARE, 0, "Failed to fetch row because result was null.", BPLN);

		// return null
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	}

	// create new row item
	MYSQL_ROW row = (char **) NULL;

	// fetch row
	row = mysql_fetch_row(result->set);
	if(!row)
	{
		BP_MySQLAddErr(mysqlSession, BP_MYSQL_ERROR_PREPARE, 0, "Failed to fetch row.", BPLN);
	}

	// return the fetched row (or mysql error value if set)
	BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(row);

}

// returns the length array corresponding to the MYSQL_ROW returned from FetchRow
unsigned long * BP_MySQLFetchLengths(P_BP_MYSQL_RESULT result, P_BP_MYSQL_SESSION mysqlSession)
{

	if(!mysqlSession)
		return NULL;
	if(!mysqlSession->system_hreg)
		return NULL;

	// enter critical section before returning
	BP_MYSQL_ENTER_CRITICAL_SECTION;

	// ensure we have a result
	if(!result)
	{
		BP_MySQLAddErr(mysqlSession, BP_MYSQL_ERROR_PREPARE, 0, "Attempted to fetch lengths, but there was no result to fetch from.", BPLN);
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);
	}

	// fetch lengths
	unsigned long * fetch_lengths = mysql_fetch_lengths(result->set);

	// ensure we have fetched some lengths
	if(!fetch_lengths)
	{
		BP_MySQLAddErr(mysqlSession, BP_MYSQL_ERROR_PREPARE, 0, "Failed to fetch lengths.", BPLN);
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);
	}

	// fetch the lengths
	BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fetch_lengths);

}

// Create New Database: max name length: 1024 (possible injection if misused,
// I REPEAT, IT IS POSSIBLE TO INJECT INTO THE dbName VARIABLE.  Don't just
// arbitrarily give access to this function without first properly sanitizng
// user input.  This function is deliberately written this way).
BP_ERROR_T BP_MySQLCreateDB(P_BP_MYSQL_SESSION mysqlSession, char *dbName)
{

	// check for session, exit with static value if not
	if(!mysqlSession)
		return ERR_FAILURE;
	if(!mysqlSession->system_hreg)
		return ERR_FAILURE;

	// enter critical section
	BP_MYSQL_ENTER_CRITICAL_SECTION;

	// ensure connected status
	if(!mysqlSession->connected || mysqlSession->server_gone_away || mysqlSession->server_lost_connection)
	{
		#if DEBUG_MYSQL_OUTPUT == BP_TRUE
		printf("\n [!!] Server is not connected.  Cannot use this routine without valid connection.");
		#endif

		// leave critical section
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}

	// if we have no dbname set, return failure
	if(!dbName)
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// ensure dbname is reasonable printable string
	if(!BP_StringIsReasonablePrintableString(dbName, bpstrlen(dbName), BP_FALSE, BP_FALSE))
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);


	// get the db name len up to 1KB
	size_t dbNameLen = strnlen(dbName, 1024);

	// lots of space left over just in case of some terrible unforseen event
	char *createQuery = (char *) tq_calloc(mysqlSession->tq, 2048, 1, BPLN);

	// create stack of arrays
	char *create_db_string[10];

	// zero out stack
	BP_ZERO_STACK(create_db_string);

	// create db string stack
	create_db_string[0] = "CREATE DATABASE `";
	create_db_string[1] = dbName;
	create_db_string[2] = "`;";

	// join the strings into a heap buffer
	char * final_buff = BP_JoinStringArray((char **)&create_db_string, 3);

	// run query
	int result = BP_MySQLQuery(mysqlSession, final_buff);

	// return result indicator
	if(result != 0)
	{
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// return indicating success if the return was zero
	BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}


// Drops a Database.  This function is vulnerable to SQL injection
// in the dbName parameter.  This is by design.  Be sure to sanitize
// user inputbefore using this function, or trust user input at your
// own peril.
//
//
int BP_MySQLDropDB (P_BP_MYSQL_SESSION mysqlSession, char *dbName)
{

	// check for session, exit with static value if not
	if(!mysqlSession)
		return BP_MYSQL_QUERY_ERROR_CODE_NO_SESSION;
	if(!mysqlSession->system_hreg)
		return BP_MYSQL_QUERY_ERROR_CODE_NO_SESSION;

	// enter critical section
	BP_MYSQL_ENTER_CRITICAL_SECTION;

	// ensure connected status
	if(!mysqlSession->connected || mysqlSession->server_gone_away || mysqlSession->server_lost_connection)
	{
		printf("\n [!!] Server is not connected in BP_MySQLDropDB.  Cannot use this routine without valid connection.");
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(BP_MYSQL_QUERY_ERROR_CODE_NOT_CONNECTED);
	}


	// check that we have a database name
	if(dbName)
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	if(!BP_StringIsReasonablePrintableString(dbName, bpstrlen(dbName), BP_FALSE, BP_FALSE))
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);


	// procure the length of the database name
	size_t dbNameLen = strnlen(dbName, 1024);

	// allocate query buffer with lots of space left over
	char *destroyQuery = (char *) bpcalloc(2048, 1);

	// add query prefix
	strncat(destroyQuery, "DROP DATABASE `", 100);

	// add dbname (can be full length because of extra buffer allocation
	// above.
	strncat(destroyQuery, dbName, dbNameLen);

	// terminate query buffer
	strcat(destroyQuery, "`;");

	// run drop query
	int result = BP_MySQLQuery(mysqlSession, destroyQuery);

	// return the mysql result
	BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(result);

}


// === Error Stacks ===================================================

// Adds error information to an existing mysql session.  Provide a valid
// mysql session, and relevant parameters, and you can set dynamic errors
// to your mysql session.
BP_ERROR_T BP_MySQLAddErr
(
	P_BP_MYSQL_SESSION mysqlSession,
	BP_MYSQL_ERROR_T err_type,
	size_t err_number,
	char * error_string,
	BPLN_PARMS
)
{

	// ensure we have a session
	if(!mysqlSession)
		return ERR_FAILURE;
	if(!mysqlSession->system_hreg)
		return ERR_FAILURE;

	// enter critical section
	BP_MYSQL_ENTER_CRITICAL_SECTION;

	// ensure we have a tail queue allocator
	if(!mysqlSession->tq)
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// reallocate space for the errors
	mysqlSession->errors = (P_BP_MYSQL_ERROR) tq_realloc
	(
		mysqlSession->tq,
		mysqlSession->errors,
		sizeof(BP_MYSQL_ERROR) * (mysqlSession->error_n+1),
		BPLN
	);

	// ensure we could we could create errors
	if(!mysqlSession->errors)
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// zero out the new structure
	memset(&mysqlSession->errors[mysqlSession->error_n], 0x00, sizeof(BP_MYSQL_ERROR));


	// === now set items ============================

	// create a reference to increase readability
	P_BP_MYSQL_ERROR err = &mysqlSession->errors[mysqlSession->error_n];

	// set error information
	err->err_type    = err_type;
	err->err_number  = err_number;
	err->err_string  = tq_strdup(mysqlSession->tq, error_string, BPLN);
	err->file_name   = tq_strdup(mysqlSession->tq, file_name,    BPLN);
	err->func        = tq_strdup(mysqlSession->tq, func,         BPLN);
	err->line_number = line_number;
	err->thread_id   = syscall(SYS_gettid);
	err->query       = tq_strdup(mysqlSession->tq, mysqlSession->last_sql_prepared, BPLN);

	// display the error immediately if set
	if(mysqlSession->display_errors_immediately)
		BP_MySQLDisplayError(err);


	// increase the error count
	mysqlSession->error_n++;

	// return indicating success
	BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}


// Display mysql errors.
BP_ERROR_T BP_MySQLDisplayErrors(P_BP_MYSQL_SESSION mysqlSession, BP_MYSQL_ERROR_T display_error_type)
{


	// Ensure we have valid parameters
	if(!mysqlSession)
		return ERR_FAILURE;
	if(!mysqlSession->error_n)
		return ERR_FAILURE;
	if(!mysqlSession->errors)
		return ERR_FAILURE;
	if(!mysqlSession->system_hreg)
		return ERR_FAILURE;

	// enter critical section
	BP_MYSQL_ENTER_CRITICAL_SECTION;

	// walk all errors and display
	size_t n = 0;
	for(; n < mysqlSession->error_n; n++)
	{

		// display selected error type, or all for default
		switch(display_error_type)
		{
			case BP_MYSQL_ERROR_NONE:
				BP_MySQLDisplayError(&mysqlSession->errors[n]);
				break;
			case BP_MYSQL_ERROR_SYNTAX:
				BP_MySQLDisplayError(&mysqlSession->errors[n]);
				break;
			case BP_MYSQL_ERROR_CONNECT:
				BP_MySQLDisplayError(&mysqlSession->errors[n]);
				break;
			case BP_MYSQL_ERROR_PREPARE:
				BP_MySQLDisplayError(&mysqlSession->errors[n]);
				break;
			case BP_MYSQL_ERROR_DESTROY:
				BP_MySQLDisplayError(&mysqlSession->errors[n]);
				break;
			case BP_MYSQL_ERROR_CLOSE:
				BP_MySQLDisplayError(&mysqlSession->errors[n]);
				break;
			default:
				BP_MySQLDisplayError(&mysqlSession->errors[n]);
				break;

		}

	}

	// return indicating success
	BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}


// Display a single mysql error.
BP_ERROR_T BP_MySQLDisplayError(P_BP_MYSQL_ERROR error_data)
{

	// ensure we have error data
	if(!error_data)
		return ERR_FAILURE;

	// error tmp set below
	char * err_type_tmp = (char *) NULL;

	switch(error_data->err_type)
	{
		case BP_MYSQL_ERROR_NONE:
			err_type_tmp = (char *) "BP_MYSQL_ERROR_NONE";
			break;
		case BP_MYSQL_ERROR_SYNTAX:
			err_type_tmp = (char *) "BP_MYSQL_ERROR_SYNTAX";
			break;
		case BP_MYSQL_ERROR_CONNECT:
			err_type_tmp = (char *) "BP_MYSQL_ERROR_CONNECT";
			break;
		case BP_MYSQL_ERROR_PREPARE:
			err_type_tmp = (char *) "BP_MYSQL_ERROR_PREPARE";
			break;
		case BP_MYSQL_ERROR_DESTROY:
			err_type_tmp = (char *) "BP_MYSQL_ERROR_DESTROY";
			break;
		case BP_MYSQL_ERROR_CLOSE:
			err_type_tmp = (char *) "BP_MYSQL_ERROR_CLOSE";
			break;
		default:
			err_type_tmp = (char *) "ERR BAD TYPE";
			break;
	}

	// display the error data
	printf
	(
		"\n"
		"\t[MYSQL-ERR] (tid: %zu) \n"\
		"\t   err_type:       %s   \n"\
		"\t   err number:     %u   \n"\
		"\t   err_string:     %s   \n"\
		"\t   file_name:      %s   \n"\
		"\t   line_number:    %zu  \n"\
		"\t   function:       %s   \n"\
		"\t   prepared query: %s   \n",
		error_data->thread_id,
		err_type_tmp,
		error_data->err_number,
		error_data->err_string,
		error_data->file_name,
		error_data->line_number,
		error_data->func,
		error_data->query
	);

	// display the thread id
	// printf("\n\t [+] Stack Trace For MySQL Error: (tid: %zu)\n", error_data->thread_id);

	// display debug backtrace
	// BP_LinuxDebugBacktraceSimple(2);

	// return indicating success
	return ERR_SUCCESS;

}


// ---------------- Prepared Statements (2010) --------------


// Stub function: returns last auto_increment id from an insert/update
my_ulonglong BP_MySQLLastInsertId(MYSQL_STMT*stmt)
{
	return mysql_stmt_insert_id(stmt);
}

// prepare mysql query
MYSQL_STMT * BP_MySQLPrepareQuery(P_BP_MYSQL_SESSION mysqlSession, char *sql)
{

	// ensure we have session
	if(!mysqlSession)
		return NULL;
	if(!mysqlSession->system_hreg)
		return NULL;

	// enter critical section
	BP_MYSQL_ENTER_CRITICAL_SECTION;

	// ensure the database is connected
	if
	(
		!mysqlSession->conn                 ||
		!mysqlSession->connected            ||
		mysqlSession->server_gone_away      ||
		mysqlSession->server_lost_connection
	)
	{

		// display message
		BP_MySQLAddErr(mysqlSession, BP_MYSQL_ERROR_PREPARE, 0, "Attempted to prepare query in BP_MySQLPrepareQuery, but the server was not connected.", BPLN);

		// return indicating success
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	}


	// destroy mysql statement if it already exists
	if(mysqlSession->stmt)
	{

		// close the statement
		// mysql_stmt_close(mysqlSession->stmt);

		// nullify the reference
		mysqlSession->stmt = NULL;

	}

	// declare statement
	MYSQL_STMT  *stmt = NULL;

	// ensure that the last sql prepared is destroyed between
	// iterations of prepares.
	if(mysqlSession->last_sql_prepared)
	{

		// destroy last sql prepared
		tq_free(mysqlSession->tq, mysqlSession->last_sql_prepared);

		// nullify pointer
		mysqlSession->last_sql_prepared = NULL;

	}

	// set the last sql
	mysqlSession->last_sql_prepared = tq_strdup(mysqlSession->tq, sql, BPLN);

	// initialize prepared statement
	stmt = mysql_stmt_init(mysqlSession->conn);

	// ensure we have a statement
	if(!stmt)
	{
		BP_MySQLAddErr(mysqlSession, BP_MYSQL_ERROR_PREPARE, 0, "Failed to init mysql statement.", BPLN);
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);
	}

	// prepare sql
	if( mysql_stmt_prepare(stmt, sql, bpstrlen(sql)) )
	{

		// retrieve mysql stmt error
		char * stmt_err = (char *) mysql_stmt_error(stmt);

		// store the error here (Syntax error/Lost Connection/Actual SQL error message)
		BP_MySQLAddErr(mysqlSession, BP_MYSQL_ERROR_PREPARE, 0, stmt_err, BPLN);

		// throw trap signal nd exit
		mysqlSession->stmt = NULL;

		// close the statement
		mysql_stmt_close(stmt);

		// return indicating null
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	}

	// set statement in mysql session
	mysqlSession->stmt = stmt;

	// return the statement as a pointer
	BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(stmt);

}

// bind parameters to a mysql stmt
BP_ERROR_T BP_MySQLBindParameters(MYSQL_STMT *stmt, MYSQL_BIND *params, P_BP_MYSQL_SESSION mysqlSession)
{

	if(!mysqlSession)
		return ERR_FAILURE;
	if(!mysqlSession->system_hreg)
		return ERR_FAILURE;

	// enter critical section
	BP_MYSQL_ENTER_CRITICAL_SECTION;


	// ensure we have a statement
	if(!stmt)
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// ensure we have parameters
	if(!params)
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// bind parameters
	if (mysql_stmt_bind_param(stmt, params))
	{

		// retrieve mysql stmt error
		char * stmt_err = (char *) mysql_stmt_error(stmt);

		// store the error here (Syntax error/Lost Connection/Actual SQL error message)
		BP_MySQLAddErr(mysqlSession, BP_MYSQL_ERROR_BIND, 0, stmt_err, BPLN);

		// exit if we can't bind things
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}

	// return indicating success
	BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}


// execute prepared statement
BP_ERROR_T BP_MySQLExecuteStatement(MYSQL_STMT * stmt, P_BP_MYSQL_SESSION mysqlSession)
{

	// ensure we have mysql sessions
	if(!mysqlSession)
		return ERR_FAILURE;
	if(!mysqlSession->system_hreg)
		return ERR_FAILURE;

	// enter critical section
	BP_MYSQL_ENTER_CRITICAL_SECTION;

	// ensure we have a statement to work with
	if(!stmt)
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// declare stmt err
	char * stmt_err = NULL;

	// set the error check length
	size_t err_check_len = bpstrlen(MYSQL_LOCK_WAIT_TIMEOUT_EXCEEDED_MSG);

	// transaction restart count
	size_t transaction_restart_count = 0;

	// error message
	char err_msg[1024];
	BP_ZERO_STACK(err_msg);

// restart transaction label (jumps up here if transaction fails)
restart_transaction:

	int ret = mysql_stmt_execute(stmt);
	if(ret)
	{

		// retrieve mysql stmt error
		char * stmt_err = (char *) mysql_stmt_error(stmt);

		// display statement error if necessary
#if DEBUG_MYSQL_OUTPUT == BP_TRUE
		printf("\n [!!] MySQL Prepared Statement Failed to Execute: %s", stmt_err);
#endif

		// re-run if necessary
		if(stmt_err)
		{

			// if we've timed out, and we're set to restart transactions, try here
			if(bpstrlen(stmt_err) == err_check_len)
			if(memcmp(stmt_err, MYSQL_LOCK_WAIT_TIMEOUT_EXCEEDED_MSG, err_check_len) == 0)
			if(mysqlSession->restart_lock_timeout_transactions)
			{

				// only consider a transaction error if we've failed the same transaction
				// more than 3 times.
				if(transaction_restart_count >= 3)
				{

					BP_ZERO_STACK(err_msg);
					sprintf(err_msg, "Lock timeout.  Restarting transaction again.  This transaction loop has run %zu times.", transaction_restart_count);
					BP_MySQLAddErr(mysqlSession, BP_MYSQL_ERROR_EXECUTE, 0, err_msg, BPLN);

				}

				// increment the transaction restart count
				transaction_restart_count++;

				// restart the transaction
				goto restart_transaction;

			}

		}

		// store the error here (Syntax error/Lost Connection/Actual SQL error message)
		BP_MySQLAddErr(mysqlSession, BP_MYSQL_ERROR_EXECUTE, 0, stmt_err, BPLN);

		// return err failure
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}

	// return indicating success
	BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}


// bind results to a statment
BP_ERROR_T BP_MySQLBindResult(MYSQL_STMT *stmt, MYSQL_BIND * results, size_t result_column_count, P_BP_MYSQL_SESSION mysqlSession)
{

	// ensure we have mysql sessions
	if(!mysqlSession)
		return ERR_FAILURE;
	if(!mysqlSession->system_hreg)
		return ERR_FAILURE;

	// enter critical section
	BP_MYSQL_ENTER_CRITICAL_SECTION;

	// nullify results before testing
	mysqlSession->bind_results_external_ptr = NULL;

	// destroy the buffer allocator if not already set
	if(mysqlSession->result_buffer_auto_allocator)
	{
		BP_DestroyLinkL(mysqlSession->result_buffer_auto_allocator, BP_TRUE);
		mysqlSession->result_buffer_auto_allocator = NULL;
	}

	// check to ensure we have a statement
	if(!stmt)
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// ensure we have results
	if(!results)
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Session Result Pointers %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Note: Pointers/count need to be allocated here due to
	//       the linear progression of logic below.  Do not
	//       move this.


	// set external pointer
	mysqlSession->bind_results_external_ptr = results;

	// set the result column count
	mysqlSession->bind_results_external_column_count = result_column_count;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Up Lengths Where Necessary %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// iterator to walk fetched fields
	size_t result_field_fetch_iter = 0;

	// walk the results
	for
	(
		;
		result_field_fetch_iter < mysqlSession->bind_results_external_column_count;
		result_field_fetch_iter++
	)
	{


		// if we don't have a tailqueue allocator to work with, allocate it JIT
		if(!mysqlSession->result_buffer_auto_allocator)
			mysqlSession->result_buffer_auto_allocator = BP_CreateLinkL(".", BP_TRUE);

		// note:  Don't allocate buffer here.  Only allocate buffer after fetch (can alloc
		//        and then grab column data directly).


		// allocate space for holding the is_null parameter
		mysqlSession->bind_results_external_ptr[result_field_fetch_iter].is_null = (my_bool *) tq_calloc
		(
				mysqlSession->result_buffer_auto_allocator,
				sizeof(size_t),
				1,
				BPLN
		);

		// allocate space for holding the length parameter (not to be confused with buffer_length)
		mysqlSession->bind_results_external_ptr[result_field_fetch_iter].length = (size_t *) tq_calloc
		(
				mysqlSession->result_buffer_auto_allocator,
				sizeof(size_t),
				1,
				BPLN
		);

		/*
		// allocate space to hold buffer length
		mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer_length = (long unsigned int *) tq_calloc
		(
				mysqlSession->result_buffer_auto_allocator,
				sizeof(long unsigned int),
				1,
				BPLN
		);
		*/

		// allocate space for holding the error parameter
		mysqlSession->bind_results_external_ptr[result_field_fetch_iter].error = (my_bool *) tq_calloc
		(
				mysqlSession->result_buffer_auto_allocator,
				sizeof(size_t),
				1,
				BPLN
		);


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Allocate Static Buffers if Types Match %%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// this has to be done for mysql types which don't match string or blob types.  The
		// table below shows the length (in bytes) of each type as per the mysql documentation
		// on mysql.com (http://dev.mysql.com/doc/refman/5.7/en/mysql-stmt-fetch.html)

		/*
		MYSQL_TYPE_TINY	1
		MYSQL_TYPE_SHORT	2
		MYSQL_TYPE_LONG	4
		MYSQL_TYPE_LONGLONG	8
		MYSQL_TYPE_FLOAT	4
		MYSQL_TYPE_DOUBLE	8
		MYSQL_TYPE_TIME	sizeof(MYSQL_TIME)
		MYSQL_TYPE_DATE	sizeof(MYSQL_TIME)
		MYSQL_TYPE_DATETIME	sizeof(MYSQL_TIME)
		MYSQL_TYPE_STRING	data length
		MYSQL_TYPE_BLOB	data_length
		*/

		/* from mysql_com.h:
		MYSQL_TYPE_DECIMAL,   // done
		MYSQL_TYPE_TINY,      // done
		MYSQL_TYPE_SHORT,     // done
		MYSQL_TYPE_LONG,      // done
		MYSQL_TYPE_FLOAT,     // done
		MYSQL_TYPE_DOUBLE,    // done
		MYSQL_TYPE_NULL,
		MYSQL_TYPE_TIMESTAMP, // done
		MYSQL_TYPE_LONGLONG,  // done
		MYSQL_TYPE_INT24,	  // int
		MYSQL_TYPE_DATE,      // done
		MYSQL_TYPE_TIME,      // done
		MYSQL_TYPE_DATETIME,  // done
		MYSQL_TYPE_YEAR,      // done
		MYSQL_TYPE_NEWDATE,   // done
		MYSQL_TYPE_BIT,       // done
		MYSQL_TYPE_NEWDECIMAL // done
		 */


		// Switch on the type and allocate where necessary.  Note that some extra (4) bytes are added
		// as padding, because I'm paranoid.  That's the whole reason.  Paranoia.
		switch(mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer_type)
		{


			// int24
			case MYSQL_TYPE_INT24:
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer = tq_calloc
				(
					mysqlSession->result_buffer_auto_allocator,
					sizeof(int),
					1,
					BPLN
				);

				// set buffer length
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer_length = sizeof(int);
				break;

			// set decimal/newdecimal type
			case MYSQL_TYPE_DECIMAL:
			case MYSQL_TYPE_NEWDECIMAL:
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer = tq_calloc
				(
					mysqlSession->result_buffer_auto_allocator,
					sizeof(double),
					1,
					BPLN
				);

				// set buffer length
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer_length = sizeof(double);
				break;

			// 1 byte (+4)
			case MYSQL_TYPE_TINY:
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer = tq_calloc
				(
					mysqlSession->result_buffer_auto_allocator,
					5,
					1,
					BPLN
				);
				// set buffer length
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer_length = 1;
				break;

			// bit type
			case MYSQL_TYPE_BIT:
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer = tq_calloc
				(
					mysqlSession->result_buffer_auto_allocator,
					5,
					1,
					BPLN
				);
				// set buffer length
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer_length = 1;
				break;


			// 2 bytes (+4)
			case MYSQL_TYPE_SHORT:
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer = tq_calloc
				(
					mysqlSession->result_buffer_auto_allocator,
					6,
					1,
					BPLN
				);
				// set buffer length
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer_length = 2;
				break;

			// 4 bytes (+4)
			case MYSQL_TYPE_LONG:
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer = tq_calloc
				(
					mysqlSession->result_buffer_auto_allocator,
					8,
					1,
					BPLN
				);
				// set buffer length
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer_length = 4;
				break;

			// 8 bytes (+4)
			case MYSQL_TYPE_LONGLONG:
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer = tq_calloc
				(
					mysqlSession->result_buffer_auto_allocator,
					12,
					1,
					BPLN
				);

				// set buffer length
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer_length = 8;
				break;

			// 4 bytes (+4)
			case MYSQL_TYPE_FLOAT:
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer = tq_calloc
				(
					mysqlSession->result_buffer_auto_allocator,
					8,
					1,
					BPLN
				);

				// set buffer length
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer_length = 4;
				break;

			// 8 bytes (+4)
			case MYSQL_TYPE_DOUBLE:
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer = tq_calloc
				(
					mysqlSession->result_buffer_auto_allocator,
					12,
					1,
					BPLN
				);

				// set buffer length
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer_length = 8;
				break;

			// sizeof(MYSQL_TYPE) bytes (+4)
			case MYSQL_TYPE_TIME:
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer = tq_calloc
				(
					mysqlSession->result_buffer_auto_allocator,
					sizeof(MYSQL_TIME)+4,
					1,
					BPLN
				);

				// set buffer length
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer_length = sizeof(MYSQL_TIME);
				break;

			// sizeof(MYSQL_TYPE) bytes (+4)
			case MYSQL_TYPE_DATE:
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer = (
					mysqlSession->result_buffer_auto_allocator,
					sizeof(MYSQL_TIME)+4,
					1,
					BPLN
				);

				// set buffer length
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer_length = sizeof(MYSQL_TIME);
				break;

			// newdata type
			case MYSQL_TYPE_NEWDATE:
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer = (
					mysqlSession->result_buffer_auto_allocator,
					sizeof(MYSQL_TIME)+4,
					1,
					BPLN
				);

				// set buffer length
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer_length = sizeof(MYSQL_TIME);
				break;

			// year type
			case MYSQL_TYPE_YEAR:
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer = (
					mysqlSession->result_buffer_auto_allocator,
					sizeof(MYSQL_TIME)+4,
					1,
					BPLN
				);

				// set buffer length
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer_length = sizeof(MYSQL_TIME);
				break;

			// sizeof(MYSQL_TYPE) bytes (+4)
			case MYSQL_TYPE_DATETIME:
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer = (
					mysqlSession->result_buffer_auto_allocator,
					sizeof(MYSQL_TIME)+4,
					1,
					BPLN
				);

				// set buffer length
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer_length = sizeof(MYSQL_TIME);
				break;


			// sizeof(MYSQL_TYPE) bytes (+4)
			case MYSQL_TYPE_TIMESTAMP:
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer = (
					mysqlSession->result_buffer_auto_allocator,
					sizeof(MYSQL_TIME)+4,
					1,
					BPLN
				);

				// set buffer length
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer_length = sizeof(MYSQL_TIME);
				break;

			// DO NOTHING HERE.  These are allocated dynamically later.
			case MYSQL_TYPE_STRING:
				break;
			case MYSQL_TYPE_BLOB:
				break;
			default:
				break;

		}


	}


	// attempt to bind results
	if(mysql_stmt_bind_result(stmt, results))
	{

		// retrieve mysql stmt error
		char * stmt_err = (char *) mysql_stmt_error(stmt);

		// store the error here (Syntax error/Lost Connection/Actual SQL error message)
		BP_MySQLAddErr(mysqlSession, BP_MYSQL_ERROR_BIND, 0, stmt_err, BPLN);

		// return indicating failure
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}



	// return indicating success
	BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}


// Store results (optional, retrieve results from db before fetching)
BP_ERROR_T BP_MySQLStoreResults(MYSQL_STMT * stmt, P_BP_MYSQL_SESSION mysqlSession)
{


	// ensure we have mysql sessions
	if(!mysqlSession)
		return ERR_FAILURE;
	if(!mysqlSession->system_hreg)
		return ERR_FAILURE;

	// enter critical section
	BP_MYSQL_ENTER_CRITICAL_SECTION;


	// ensure we have a statement
	if(!stmt)
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// attempt to store result
	if(mysql_stmt_store_result(stmt))
	{

		// retrieve mysql stmt error
		char * stmt_err = (char *) mysql_stmt_error(stmt);

		// store the error here (Syntax error/Lost Connection/Actual SQL error message)
		BP_MySQLAddErr(mysqlSession, BP_MYSQL_ERROR_STORE, 0, stmt_err, BPLN);

		// return indicating failure
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}

	// return indicating success
	BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// Fetch result from a valid statement
BP_ERROR_T BP_MySQLFetchResult(MYSQL_STMT * stmt, P_BP_MYSQL_SESSION mysqlSession)
{

	// ensure we have mysql sessions
	if(!mysqlSession)
		return ERR_FAILURE;
	if(!mysqlSession->system_hreg)
		return ERR_FAILURE;

	// enter critical section
	BP_MYSQL_ENTER_CRITICAL_SECTION;

	// ensure we have a statement
	if(!stmt)
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);


	// printf("\n Fetching?");
	// printf("\n");

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Fetch Record %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// fetch result
	int fetch_val = mysql_stmt_fetch(stmt);

	// exit immediately if the fetch fails
	if
	(
		(fetch_val == 1) ||
		(fetch_val == MYSQL_NO_DATA)
	)
	{
		// return indicating failure
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// check to ensure that the column count and the field count match up
	if(mysqlSession->bind_results_external_column_count != stmt->field_count)
	{

		// dbg: optional display of bad column count/field count.
		printf("\n [DBG] mysqlSession->bind_results_external_column_count (%u) != stmt->field_count (%u)", mysqlSession->bind_results_external_column_count, stmt->field_count);

		// add error if necessary
		BP_MySQLAddErr
		(
				mysqlSession,
				BP_MYSQL_ERROR_FETCH,
				0,
				"Attempted to fetch record but the statement shows a field count (aka. column count) "\
				"mismatch with the number of results in the mysqlSession->bind_results_external_column_count "\
				"field.  This is a developer error.  Check your result count matches the number of results "\
				"you're passing in.",
				BPLN
		);



		// exit with error status if we have a bad count
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}


	// ensure we fetched data ok
	if
	(
		(fetch_val != 1) &&
		(fetch_val != MYSQL_NO_DATA)
	)
	{


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Auto Column Allocation (Added 2015) %%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// iterator to walk fetched fields
		size_t result_field_fetch_iter = 0;

		// walk the results
		for
		(
			result_field_fetch_iter = 0;
			result_field_fetch_iter < stmt->field_count;
			result_field_fetch_iter++
		)
		{


			/* 2016 type values (all)
			MYSQL_TYPE_DECIMAL,
			MYSQL_TYPE_TINY,
			MYSQL_TYPE_SHORT,
			MYSQL_TYPE_LONG,
			MYSQL_TYPE_FLOAT,
			MYSQL_TYPE_DOUBLE,
			MYSQL_TYPE_NULL,
			MYSQL_TYPE_TIMESTAMP,
			MYSQL_TYPE_LONGLONG,
			MYSQL_TYPE_INT24,
			MYSQL_TYPE_DATE,
			MYSQL_TYPE_TIME,
			MYSQL_TYPE_DATETIME,
			MYSQL_TYPE_YEAR,
			MYSQL_TYPE_NEWDATE,
			MYSQL_TYPE_VARCHAR,
			MYSQL_TYPE_BIT,
			MYSQL_TYPE_NEWDECIMAL=246,
			MYSQL_TYPE_ENUM=247,
			MYSQL_TYPE_SET=248,
			MYSQL_TYPE_TINY_BLOB=249,
			MYSQL_TYPE_MEDIUM_BLOB=250,
			MYSQL_TYPE_LONG_BLOB=251,
			MYSQL_TYPE_BLOB=252,
			MYSQL_TYPE_VAR_STRING=253,
			MYSQL_TYPE_STRING=254,
			MYSQL_TYPE_GEOMETRY=255
			*/

			/* blob types only for reference
			MYSQL_TYPE_TINY_BLOB=249,
			MYSQL_TYPE_MEDIUM_BLOB=250,
			MYSQL_TYPE_LONG_BLOB=251,
			MYSQL_TYPE_BLOB=252,
			MYSQL_TYPE_VAR_STRING=253,
			 MYSQL_TYPE_STRING=254,
			 */

			// set buffer type for exclusionary if
			enum enum_field_types buffer_type = mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer_type;

			// don't allocate anything if it's not a blob or a string type.  These types are the only can posess
			// dynamic lengths.  The other buffer types are preallocated at result binding since they're static
			// buffer lengths.
			if
			(
				buffer_type != MYSQL_TYPE_DECIMAL &&
				buffer_type != MYSQL_TYPE_TINY &&
				buffer_type != MYSQL_TYPE_SHORT &&
				buffer_type != MYSQL_TYPE_LONG &&
				buffer_type != MYSQL_TYPE_FLOAT &&
				buffer_type != MYSQL_TYPE_DOUBLE &&
				buffer_type != MYSQL_TYPE_NULL &&
				buffer_type != MYSQL_TYPE_TIMESTAMP &&
				buffer_type != MYSQL_TYPE_LONGLONG &&
				buffer_type != MYSQL_TYPE_INT24 &&
				buffer_type != MYSQL_TYPE_DATE &&
				buffer_type != MYSQL_TYPE_TIME &&
				buffer_type != MYSQL_TYPE_DATETIME &&
				buffer_type != MYSQL_TYPE_YEAR &&
				buffer_type != MYSQL_TYPE_NEWDATE &&
				buffer_type != MYSQL_TYPE_VARCHAR &&
				buffer_type != MYSQL_TYPE_BIT &&
				buffer_type != MYSQL_TYPE_NEWDECIMAL &&
				buffer_type != MYSQL_TYPE_ENUM &&
				buffer_type != MYSQL_TYPE_SET &&
				buffer_type != MYSQL_TYPE_TINY_BLOB &&
				buffer_type != MYSQL_TYPE_MEDIUM_BLOB &&
				buffer_type != MYSQL_TYPE_LONG_BLOB &&
				buffer_type != MYSQL_TYPE_BLOB &&
				buffer_type != MYSQL_TYPE_VAR_STRING &&
				buffer_type != MYSQL_TYPE_STRING &&
				buffer_type != MYSQL_TYPE_GEOMETRY

			)
			{
				printf("\n [+] Error: Unsupported MySQL/MariaDB type fetched/skipped during result retrieval.");
				printf("\n            Unsupported buffer_type id: %u", mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer_type);
				continue;
			}


			// run dynamic size creation on-fetch for blob types
			if
			(
				buffer_type == MYSQL_TYPE_TINY_BLOB   ||
				buffer_type == MYSQL_TYPE_MEDIUM_BLOB ||
				buffer_type == MYSQL_TYPE_LONG_BLOB   ||
				buffer_type == MYSQL_TYPE_BLOB        ||
				buffer_type == MYSQL_TYPE_VAR_STRING  ||
				buffer_type == MYSQL_TYPE_STRING
			)
			{


				// free buffer on each loop before re-allocating.  Prevents run-away allocations
				// which could clog things.  Only destroy the other member allocations AFTER results
				// have finished pulling, as they're all just static size_t's etc.
				if(mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer)
				{


					// destroy old buffer if necessary
					tq_free
					(
						mysqlSession->result_buffer_auto_allocator,
						mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer
					);

					// reset parameters
					mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer = NULL;
					mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer_length = 0;


				}

				// finally allocate space for the buffer (plus 4 bytes extra just for termination purposes)
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer = (char *) tq_calloc
				(
						mysqlSession->result_buffer_auto_allocator,
						*mysqlSession->bind_results_external_ptr[result_field_fetch_iter].length + 4,
						1,
						BPLN
				);

				// set buffer length from length parameter
				mysqlSession->bind_results_external_ptr[result_field_fetch_iter].buffer_length = *mysqlSession->bind_results_external_ptr[result_field_fetch_iter].length;

			}


			// attempt to fetch the column
			int fetch_column_result = mysql_stmt_fetch_column
			(
					stmt,
					&mysqlSession->bind_results_external_ptr[result_field_fetch_iter],
					result_field_fetch_iter,
					0
			);

			// exit if our fetch fails
			if(fetch_column_result != 0)
			{
				fetch_val = 1;
				goto exit_err;
			}

		}


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Exit Here If We've Retrieved/Processed Records OK %%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


		if(result_field_fetch_iter)
		{

			// debug: optionally print out results after retrieval to see record addition linearlly
			// printf("\n [+] DEBUG: Displaying mysqlSession->bind_results_external_ptr MySQL results after fetch");
			// BP_MySQLPrintBindArray(mysqlSession->bind_results_external_ptr, 1);

		}

		// return indicating success after allocating/fetching all columns
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);



	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% At This Point We Have No More Records. %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	exit_err:
	// Destroy allocator (free head ptr as well)
	BP_DestroyLinkL(mysqlSession->result_buffer_auto_allocator, BP_TRUE);

	// reset allocator
	mysqlSession->result_buffer_auto_allocator = NULL;

	// overwrite the bind results external pointer
	mysqlSession->bind_results_external_ptr = NULL;


	// according to the MySQL manuals, 1 is indicative of
	// an error.
	if(fetch_val == 1)
	{

		// retrieve mysql stmt error
		char * stmt_err = (char *) mysql_stmt_error(stmt);

		// store the error here (Syntax error/Lost Connection/Actual SQL error message)
		BP_MySQLAddErr(mysqlSession, BP_MYSQL_ERROR_FETCH, 0, stmt_err, BPLN);

	}

	// return indicating failure
	BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

}

// Destroy result
BP_ERROR_T BP_MySQLFreeResult(MYSQL_STMT *stmt, P_BP_MYSQL_SESSION mysqlSession)
{

	// ensure we have mysql sessions
	if(!mysqlSession)
		return ERR_FAILURE;
	if(!mysqlSession->system_hreg)
		return ERR_FAILURE;

	// enter critical section
	BP_MYSQL_ENTER_CRITICAL_SECTION;

	// check if we have a statement
	if(!stmt)
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// prepare the meta result
	MYSQL_RES * prepare_meta_result = NULL;

	// prepare meta result
	prepare_meta_result = mysql_stmt_result_metadata(stmt);
	if(!prepare_meta_result)
	{

		// retrieve mysql stmt error
		char * stmt_err = (char *) mysql_stmt_error(stmt);

		// store the error here (Syntax error/Lost Connection/Actual SQL error message)
		BP_MySQLAddErr(mysqlSession, BP_MYSQL_ERROR_DESTROY, 0, stmt_err, BPLN);

		// return indicating failure
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}

	// free result data
	mysql_free_result(prepare_meta_result);

	// return indicating success
	BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// close mysql statement
BP_ERROR_T BP_MySQLCloseStatement(MYSQL_STMT *stmt, P_BP_MYSQL_SESSION mysqlSession)
{

	// ensure we have mysql sessions
	if(!mysqlSession)
		return ERR_FAILURE;
	if(!mysqlSession->system_hreg)
		return ERR_FAILURE;

	// enter critical section
	BP_MYSQL_ENTER_CRITICAL_SECTION;

	// ensure we have a statement
	if(!stmt)
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// attempt to close the statement
	if(mysql_stmt_close(stmt))
	{

		// retrieve mysql stmt error
		char * stmt_err = (char *) mysql_stmt_error(stmt);

		// store the error here (Syntax error/Lost Connection/Actual SQL error message)
		BP_MySQLAddErr(mysqlSession, BP_MYSQL_ERROR_PREPARE, 0, stmt_err, BPLN);

		// return indicating failure
		BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}

	// return indicating success
	BP_MYSQL_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Utility Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This routine attempts to convert types found within the enum_field_types
// enumerator found within mysql_com.h, into actual strings.  This allows
// them to be easily displayed via routines such as BP_MySQLPrintBindArray above.
// The returned string must be bpfreed when you're done with it.
char * BP_MySQLTypeToString(size_t mysql_type_enum_value)
{

	// Switch on the enum type immediately.  Value can be zero.
	switch(mysql_type_enum_value)
	{

		case MYSQL_TYPE_DECIMAL:
			return bpstrdup("MYSQL_TYPE_DECIMAL");

		case MYSQL_TYPE_TINY:
			return bpstrdup("MYSQL_TYPE_TINY");

		case MYSQL_TYPE_SHORT:
			return bpstrdup("MYSQL_TYPE_SHORT");

		case MYSQL_TYPE_LONG:
			return bpstrdup("MYSQL_TYPE_LONG");

		case MYSQL_TYPE_FLOAT:
			return bpstrdup("MYSQL_TYPE_FLOAT");

		case MYSQL_TYPE_DOUBLE:
			return bpstrdup("MYSQL_TYPE_DOUBLE");

		case MYSQL_TYPE_NULL:
			return bpstrdup("MYSQL_TYPE_NULL");

		case MYSQL_TYPE_TIMESTAMP:
			return bpstrdup("MYSQL_TYPE_TIMESTAMP");

		case MYSQL_TYPE_LONGLONG:
			return bpstrdup("MYSQL_TYPE_LONGLONG");

		case MYSQL_TYPE_INT24:
			return bpstrdup("MYSQL_TYPE_INT24");

		case MYSQL_TYPE_DATE:
			return bpstrdup("MYSQL_TYPE_DATE");

		case MYSQL_TYPE_TIME:
			return bpstrdup("MYSQL_TYPE_TIME");

		case MYSQL_TYPE_DATETIME:
			return bpstrdup("MYSQL_TYPE_DATETIME");

		case MYSQL_TYPE_YEAR:
			return bpstrdup("MYSQL_TYPE_YEAR");

		case MYSQL_TYPE_NEWDATE:
			return bpstrdup("MYSQL_TYPE_NEWDATE");

		case MYSQL_TYPE_VARCHAR:
			return bpstrdup("MYSQL_TYPE_VARCHAR");

		case MYSQL_TYPE_BIT:
			return bpstrdup("MYSQL_TYPE_BIT");

		case MYSQL_TYPE_NEWDECIMAL:
			return bpstrdup("MYSQL_TYPE_NEWDECIMAL");

		case MYSQL_TYPE_ENUM:
			return bpstrdup("MYSQL_TYPE_ENUM");

		case MYSQL_TYPE_SET:
			return bpstrdup("MYSQL_TYPE_SET");

		case MYSQL_TYPE_TINY_BLOB:
			return bpstrdup("MYSQL_TYPE_TINY_BLOB");

		case MYSQL_TYPE_MEDIUM_BLOB:
			return bpstrdup("MYSQL_TYPE_MEDIUM_BLOB");

		case MYSQL_TYPE_LONG_BLOB:
			return bpstrdup("MYSQL_TYPE_LONG_BLOB");

		case MYSQL_TYPE_BLOB:
			return bpstrdup("MYSQL_TYPE_BLOB");

		case MYSQL_TYPE_VAR_STRING:
			return bpstrdup("MYSQL_TYPE_VAR_STRING");

		case MYSQL_TYPE_STRING:
			return bpstrdup("MYSQL_TYPE_STRING");

		case MYSQL_TYPE_GEOMETRY:
			return bpstrdup("MYSQL_TYPE_GEOMETRY");

		default:
			return bpstrdup("TYPE_VALUE_IS_BAD_ERROR");
	}

	return NULL;
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// display bind parameters
BP_ERROR_T BP_MySQLPrintBindArray(MYSQL_BIND *bind_arr, size_t element_count)
{

	// ensure we have a bind array
	if(!bind_arr)
		return ERR_FAILURE;

	size_t i = 0;
	printf("\n\t [+] MySQL_BIND stack %p : %u deep", bind_arr, element_count);
	for(i = 0; i < element_count; i++)
	{

		printf("\n\t\t [-] Element %i", i);

		// output length pointer
		printf("\n\t\t\t length: %p", bind_arr[i].length);

		// Pointer to null indicator
		printf("\n\t\t\t is_null: %p", bind_arr[i].is_null);

		// buffer to get/put data
		printf("\n\t\t\t buffer: %p", bind_arr[i].buffer);

		// set this if you want to track data truncations happened during fetch
		printf("\n\t\t\t error: %p", bind_arr[i].error);

		// buffer type
		char * buff_type_as_str = BP_MySQLTypeToString(bind_arr[i].buffer_type);
		printf("\n\t\t\t buffer_type: %s (numeric val: %u)", buff_type_as_str, bind_arr[i].buffer_type);
		if(buff_type_as_str)
			bpfree(buff_type_as_str);

		// output buffer length, must be set when fetching str/binary
		printf("\n\t\t\t buffer_length: %u", bind_arr[i].buffer_length );

		// for the current data position
		printf("\n\t\t\t row_ptr: %p",bind_arr[i].row_ptr );

		// offset position for char/binary fetch
		printf("\n\t\t\t offset: %u", bind_arr[i].offset);

		// Used if length is 0
		printf("\n\t\t\t length_value: %u", bind_arr[i].length_value);

		// (MEMBER REMOVED IN LATER VERSIONS OF MARIADB) For null count and error messages
		// printf("\n\t\t\t param_number: %u", bind_arr[i].param_number);

		// Internal length for packed data
		printf("\n\t\t\t pack_length: %u", bind_arr[i].pack_length);

		// used if error is 0
		printf("\n\t\t\t error_value: %u", bind_arr[i].error_value);

		// set if integer type is unsigned
		printf("\n\t\t\t is_unsigned: %u", bind_arr[i].is_unsigned);

		// If used with mysql_send_long_data
		printf("\n\t\t\t Length: %u",bind_arr[i].long_data_used);

		// Used if is_null is 0
		printf("\n\t\t\t is_null_value:    %u", bind_arr[i].is_null_value);
		printf("\n\t\t\t store_param_func: %p",bind_arr[i].store_param_func );
		printf("\n\t\t\t fetch_result:     %p", bind_arr[i].fetch_result );
		printf("\n\t\t\t skip_result:      %p",bind_arr[i].skip_result);

	}

	// return indicating success
	return ERR_SUCCESS;
}


// display a mysql session
void BP_MySQLDisplaySession(P_BP_MYSQL_SESSION mysqlSession)
{

	// ensure we have a session
	if(!mysqlSession)
		return;

	printf("\n [+] Displaying MySQL/MariaDB handle %p", mysqlSession);
	printf("\n     host:      %s", mysqlSession->host);
	printf("\n     db:        %s", mysqlSession->db);
	printf("\n     user:      %s", mysqlSession->user);
	printf("\n     pass:      %s", mysqlSession->pass);
	printf("\n     port:      %u", mysqlSession->port);
	printf("\n     uuid:                                %s", mysqlSession->uuid);
	printf("\n     system_hreg:                         %p", mysqlSession->system_hreg);
	printf("\n     pool_available:                      %s", mysqlSession->pool_available ? "true" : "false");
	printf("\n     tq:                                  %p", mysqlSession->tq);
	printf("\n     db_lock:                             %u", mysqlSession->db_lock);
	printf("\n     conn:                                %p", mysqlSession->conn);
	printf("\n     last_connection_failed:              %s", mysqlSession->last_connection_failed ? "true" : "false");
	printf("\n     server_gone_away:                    %u", mysqlSession->server_gone_away);
	printf("\n     server_lost_connection:              %u", mysqlSession->server_lost_connection);
	printf("\n     active:                              %u", mysqlSession->active);
	printf("\n     connected:                           %u", mysqlSession->connected);
	printf("\n     restart_lock_timeout_transactions:   %s", mysqlSession->restart_lock_timeout_transactions ? "true" : "false");
	printf("\n     called_from_pthread:                 %s", mysqlSession->called_from_pthread ? "true" : "false");
	printf("\n     thread_id:                           %u", mysqlSession->thread_id);
	printf("\n     last_query_resulted_in_deadlock:     %s", mysqlSession->last_query_resulted_in_deadlock ? "true" : "false");
	printf("\n     last_query_resulted_in_syntax_error: %s", mysqlSession->last_query_resulted_in_syntax_error ? "true" : "false");
	printf("\n     display_errors_immediately:          %s", mysqlSession->display_errors_immediately ? "true" : "false");
	printf("\n     errors:                              %p", mysqlSession->errors);
	printf("\n     error_n:                             %p", mysqlSession->error_n);
	printf("\n     multi_results:                       %p", mysqlSession->multi_results);
	printf("\n     stmt:                                %p", mysqlSession->stmt);
	printf("\n     last_sql_prepared:                   %p", mysqlSession->last_sql_prepared);
	printf("\n     bind_parameters_external_ptr:        %p", mysqlSession->bind_parameters_external_ptr);
	printf("\n     bind_results_external_column_count:  %u", mysqlSession->bind_results_external_column_count);
	printf("\n     result_buffer_auto_allocator:        %u", mysqlSession->result_buffer_auto_allocator);


	if(mysqlSession->errors)
	{
		// display the error countrs
		printf("\n\t [+] MySQL/MariaDB Errors For %p (%u errors total)", mysqlSession, mysqlSession->error_n);

		// display errors one by one by type
		BP_MySQLDisplayErrors(mysqlSession, BP_MYSQL_ERROR_NONE);
		BP_MySQLDisplayErrors(mysqlSession, BP_MYSQL_ERROR_SYNTAX);
		BP_MySQLDisplayErrors(mysqlSession, BP_MYSQL_ERROR_CONNECT);
		BP_MySQLDisplayErrors(mysqlSession, BP_MYSQL_ERROR_PREPARE);
		BP_MySQLDisplayErrors(mysqlSession, BP_MYSQL_ERROR_FETCH);
		BP_MySQLDisplayErrors(mysqlSession, BP_MYSQL_ERROR_BIND);
		BP_MySQLDisplayErrors(mysqlSession, BP_MYSQL_ERROR_EXECUTE);
		BP_MySQLDisplayErrors(mysqlSession, BP_MYSQL_ERROR_STORE);
		BP_MySQLDisplayErrors(mysqlSession, BP_MYSQL_ERROR_DESTROY);
		BP_MySQLDisplayErrors(mysqlSession, BP_MYSQL_ERROR_CLOSE);

	}

	// exit routine
	return;
}



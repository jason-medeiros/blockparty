/*
 * Block Class:
 * =====================================================================
 * The block class provides an easy to use interface to the blockparty library.
 * While the library can be used directly without using even one block class
 * instance, it is recommended that you use the block class for your application
 * development.  The block class is simple to use, and contains bindings for most
 * of the blockparty library capabilities.
 *
 *
 * Document Author:  Jason A. Medeiros
 * Document Created: 500BC
 *
 */



// include main blockparty include file
#include "../../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %% !!! BEGIN CONSTRUCTOR/DECONSTRUCTORS !!! %%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Block Constructor Functions
Block::Block(int type)
{

	// create a new system hash table registry (used for locking and misc allocations)
	this->system_hreg = BP_CreateHashTableRegistry();

	// create a new tail queue memory allocator
	this->tq = BP_CreateLinkL("block", BP_TRUE);



	// set yahoo blacklist
	this->custom_yahoo_blacklist = NULL;


	// Initialize all pointers to null
	this->fdSet       = NULL;
	this->fileList    = NULL;
	this->searchList  = NULL;
	this->net_handle  = NULL;
	this->bufferList  = NULL;
	this->file        = NULL;

#if USE_BLOCKPARTY_DEBUGGING == 1

	#if BLOCKPARTY_DEBUGGING_TARGET_64BIT == 1

	#endif // #if BLOCKPARTY_DEBUGGING_TARGET_64BIT == 1


	#if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1
	// set ascii to null
	this->sha1_ascii = NULL;
	this->dbgHandle = NULL;
	this->triggerFile = NULL;
	this->exploitAnalysis = NULL;
	this->instructionTable = NULL;
	this->instructions = NULL;
	this->syscallLookup = NULL;
	#endif // #if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1

#endif // USE_BLOCKPARTY_DEBUGGING == 1

	this->mysqlResults = NULL;
	this->mysqlSession = NULL;
	this->httpSession = NULL;
	this->htmlExtraction = NULL;
	this->webAssessment = NULL;
	this->google_session = NULL;
	this->ddg_session = NULL;
	this->custom_google_blacklist = NULL;
	this->custom_ddg_blacklist = NULL;

	// errors and warnings
	this->errorCount = 0;
	this->errorStrings = NULL;
	this->warningCount = 0;
	this->warningStrings = NULL;
	this->got_contents_size = 0;
	this->got_contents = 0;
	this->unparsedHtml = NULL;
	this->unparsedHtmlLen = 0;

	// set initial web results
	memset((char *) &this->web_result_href, 0x00, 4096);
	this->web_result_id = 0;

	// enable extended data tracking in the databse
	this->enable_form_data_logging_during_extraction   = 0;
	this->enable_anchor_data_logging_during_extraction = 0;

	// set all session variables to null
	this->bing_session   = NULL;
	this->yahoo_session  = NULL;
	this->google_session = NULL;

	// initialize proxy db members
	this->db_populated_proxy_group_n            = 0;
	this->db_populated_proxy_groups             = NULL;
	this->db_populated_proxy_n                  = 0;
	this->db_populated_proxy_sets               = NULL;
	this->db_populated_proxy_validation_group_n = 0;
	this->db_populated_proxy_validation_groups  = NULL;
	this->db_populated_proxy_validation_test_n  = 0;
	this->db_populated_proxy_validation_tests   = NULL;


	this->google_retry_on_too_small_size = false;
	this->google_retry_on_bad_connect = false;

	// web request jit testing initialization
	this->web_request_jit_match_set = NULL;
	this->web_request_jit_match_n = 0;

	// set the web result stack data
	this->web_result_stack_n = 0;
	this->web_result_stack_data_allocator = NULL;
	this->web_result_stack   = NULL;

	// current/next index of the web result stored in the web_result_href pointer.
	this->web_result_stack_current_index = 0;
	this->web_result_stack_next_index    = 0;

	// flag indicating whether or not the stack needs to be reset
	this->web_result_stack_all_popped = 0;

	// set lookups to disabled by default
	this->disable_result_lookups_due_to_duplicate_key_contention_fix = 1;

	// set balancer to null by default
	this->web_balancer = NULL;

	// set default flags to true
	this->base_insertion_is_handled_by_backend_db_event = 1;
	this->path_insertion_is_handled_by_backend_db_event = 1;

	// create new string builder for delayed insert sql
	this->jit_delayed_insert_sql = NULL;

	// create new string utils
	this->result_stack_builder = NULL;

	// result stacking (on by default, every 100 records)
	this->enable_result_insert_stacking = true;
	this->result_stacking_flush_rate = 100;
	this->result_stack_n = 0;

	this->mysql_stmt = NULL;


	// set clause generation
	this->results_in_id_clause = NULL;
	this->id_n = 0;
	this->results_in_assessment_id_clause = NULL;
	this->assessment_id_n = 0;
	this->results_in_base_clause = NULL;
	this->base_n = 0;
	this->results_in_path_clause = NULL;
	this->path_n = 0;
	this->results_in_script_clause = NULL;
	this->script_n = 0;

	this->htmlExtraction = NULL;

	// message logging disabled by default
	this->msg_logging_enabled = 0;

	// set web result hits (aka how many time's we've seen a link) thresholds
	this->web_result_disable_script_threshold = 0;
	this->web_result_disable_path_threshold   = 0;
	this->web_result_disable_base_threshold   = 0;

	// these parameters are thresholds for the result_*_uris tables in the database
	// and reflect the `spidered_count` column in a >= match semantic.
	this->web_result_disable_script_spidered_threshold = 0;
	this->web_result_disable_path_spidered_threshold   = 0;
	this->web_result_disable_base_spidered_threshold   = 0;

	// set audited thresholds
	this->web_result_disable_script_audited_threshold = 0;
	this->web_result_disable_path_audited_threshold   = 0;
	this->web_result_disable_base_audited_threshold   = 0;

	// set web result lookup parameters
	this->web_result_lookup_parms = NULL;

	// init dmss
	this->disconnectme_session = NULL;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Declare/Initialize Thread Members %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#ifdef BP_BLOCKCLASS_INCLUDE_USE_OLD_PTHREAD_METHODS

	this->threads = NULL;

	// zero the thread info buffer
	memset(&this->thread_info, 0x00, sizeof(this->thread_info));

	// init the thread semaphore
	sem_init(&this->thread_info.lock, 0, 1);
	this->thread_info.total_running_threads = 0;

#endif


#ifdef BP_BLOCKCLASS_INCLUDE_USE_NEW_PTHREAD_METHODS

	// create new thread manager
	this->thread_manager = BP_CreateThreadManager("block thread manager", "thread manager");


#endif

#ifdef BP_BLOCKCLASS_INCLUDE_USER_DB

	// nullify user info pointer
	this->user_info = NULL;

#endif

	// set this to immediately display errors
	this->display_errors_immediately = BP_TRUE;

	// set this to immediately display warnings
	this->display_warnings_immediately = BP_FALSE;

	// init dupe lookups
	this->disable_result_lookups_due_to_duplicate_key_contention_fix = 0;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Command Injection Assessment %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// command injection assessment
	this->cmd_injection_assessment = NULL;

}

// duplicate constructor for default value
Block::Block()
{

	// create a new system hash table registry (used for locking and misc allocations)
	this->system_hreg = BP_CreateHashTableRegistry();

	// Create a new tail queue memory allocator for
	// this block.
	this->tq = BP_CreateLinkL("block", BP_TRUE);

	// Initialize all pointers to null
	this->sha1_ascii  = NULL;
	this->fdSet       = NULL;
	this->fileList    = NULL;
	this->searchList  = NULL;
	this->net_handle  = NULL;
	this->bufferList  = NULL;
	this->file        = NULL;

#if USE_BLOCKPARTY_DEBUGGING == 1

#if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1
	this->dbgHandle = NULL;

	this->triggerFile = NULL;

	this->exploitAnalysis = NULL;

	this->instructionTable = NULL;

	this->instructions = NULL;

	this->syscallLookup = NULL;
#endif

#endif // USE_BLOCKPARTY_DEBUGGING == 1

	this->mysqlResults = NULL;
	this->mysqlSession = NULL;

	this->httpSession = NULL;
	this->htmlExtraction = NULL;

	this->webAssessment = NULL;
	this->ddg_session = NULL;
	this->google_session = NULL;

	// initialize blacklists
	this->custom_google_blacklist = NULL;
	this->custom_yahoo_blacklist = NULL;
	this->custom_ddg_blacklist = NULL;

	// errors and warnings
	this->errorCount = 0;
	this->errorStrings = NULL;
	this->warningCount = 0;
	this->warningStrings = NULL;

#ifdef BP_BLOCKCLASS_INCLUDE_MYSQL_DB_BINDINGS

	// statment
	this->mysql_stmt = NULL;

	// results structure used with fetch
	this->mysql_results = NULL;

	// current number of results in results array
	this->mysql_results_count = 0;

	// paramters structure used with prepare
	this->mysql_parameters = NULL;

	// current number of parameters in parameters array
	this->mysql_parameter_count = 0;

	// database encryption aes key
	this->mysql_aes_key = NULL;

#ifdef BP_BLOCKCLASS_INCLUDE_NMAP_SQL_DB_BINDINGS

	// set sessions = null
	this->cc_session = NULL;

	// set result pointer to null
	this->nmapdb_results = NULL;

	// portstat search items
	memset(&this->portstats_min_s, 0x00, sizeof(this->portstats_min_s));
	memset(&this->portstats_max_s, 0x00, sizeof(this->portstats_max_s));
	memset(&this->portstats_like_s, 0x00, sizeof(this->portstats_like_s));
	memset(&this->portstats_equal_s, 0x00, sizeof(this->portstats_equal_s));

	// hoststats search items
	memset(&this->hoststats_min_s, 0x00, sizeof(this->portstats_min_s));
	memset(&this->hoststats_max_s, 0x00, sizeof(this->portstats_max_s));
	memset(&this->hoststats_like_s, 0x00, sizeof(this->portstats_like_s));
	memset(&this->hoststats_equal_s, 0x00, sizeof(this->portstats_equal_s));

	// target search items
	memset(&this->targets_min_s, 0x00, sizeof(this->portstats_min_s));
	memset(&this->targets_max_s, 0x00, sizeof(this->portstats_max_s));
	memset(&this->targets_like_s, 0x00, sizeof(this->portstats_like_s));
	memset(&this->targets_equal_s, 0x00, sizeof(this->portstats_equal_s));

	// scanner search items
	memset(&this->scanners_min_s, 0x00, sizeof(this->portstats_min_s));
	memset(&this->scanners_max_s, 0x00, sizeof(this->portstats_max_s));
	memset(&this->scanners_like_s, 0x00, sizeof(this->portstats_like_s));
	memset(&this->scanners_equal_s, 0x00, sizeof(this->portstats_equal_s));

	// reset runlist search items
	memset(&this->runlist_min_s, 0x00, sizeof(this->portstats_min_s));
	memset(&this->runlist_max_s, 0x00, sizeof(this->portstats_max_s));
	memset(&this->runlist_like_s, 0x00, sizeof(this->portstats_like_s));
	memset(&this->runlist_equal_s, 0x00, sizeof(this->portstats_equal_s));

#endif
#endif

	this->got_contents_size = 0;
	this->got_contents = 0;

	// create buffer and drop it in the garbage collector
	char buff[256] = {0};
	snprintf(buff, 128, "Garbage Collector for Block(): %p", this );

	// create the garbage collector
	this->gc = BP_CreateNewGarbageCollector(buff, 1);

	this->unparsedHtml = NULL;
	this->unparsedHtmlLen = 0;

	memset((char *) &this->web_result_href, 0x00, 4096);
	this->web_result_id = 0;

	// enable extended data tracking in the databse
	this->enable_form_data_logging_during_extraction   = 0;
	this->enable_anchor_data_logging_during_extraction = 0;
	this->bing_session                   = NULL;
	this->yahoo_session                  = NULL;
	this->google_session                 = NULL;
	this->google_retry_on_too_small_size = false;


	// initialize proxy db members
	this->db_populated_proxy_group_n = 0;
	this->db_populated_proxy_groups  = NULL;
	this->db_populated_proxy_n       = 0;
	this->db_populated_proxy_sets    = NULL;
	this->db_populated_proxy_validation_group_n = 0;
	this->db_populated_proxy_validation_groups  = NULL;
	this->db_populated_proxy_validation_test_n  = 0;
	this->db_populated_proxy_validation_tests   = NULL;

	google_retry_on_bad_connect = false;

	// JIT matching
	this->web_request_jit_match_set = NULL;
	this->web_request_jit_match_n = 0;

	// set the web result stack data
	this->web_result_stack_n = 0;
	this->web_result_stack_data_allocator = NULL;
	this->web_result_stack   = NULL;

	// current/next index of the web result stored in the web_result_href pointer.
	this->web_result_stack_current_index = 0;
	this->web_result_stack_next_index    = 0;

	// flag indicating whether or not the stack needs to be reset
	this->web_result_stack_all_popped = 0;

	// set lookups to disabled by default
	this->disable_result_lookups_due_to_duplicate_key_contention_fix = 0;

	// set balancer to null by default
	this->web_balancer = NULL;

	// set default flags to true
	this->base_insertion_is_handled_by_backend_db_event = 1;
	this->path_insertion_is_handled_by_backend_db_event = 1;

	// create new string builder for delayed insert sql
	this->jit_delayed_insert_sql = NULL;

	// result stacking (on by default, every 100 records)
	this->enable_result_insert_stacking = true;
	this->result_stacking_flush_rate = 100;
	this->result_stack_n = 0;

	// initialize stack builder ptr
	this->result_stack_builder = NULL;

	// set clause generation
	this->results_in_id_clause = NULL;
	this->id_n = 0;
	this->results_in_assessment_id_clause = NULL;
	this->assessment_id_n = 0;
	this->results_in_base_clause = NULL;
	this->base_n = 0;
	this->results_in_path_clause = NULL;
	this->path_n = 0;
	this->results_in_script_clause = NULL;
	this->script_n = 0;
	this->htmlExtraction = NULL;

	// message logging disabled by default
	this->msg_logging_enabled = 0;

	// set web result thresholds
	this->web_result_disable_script_threshold = 0;
	this->web_result_disable_path_threshold   = 0;
	this->web_result_disable_base_threshold   = 0;

	// set web result lookup parameters
	this->web_result_lookup_parms = NULL;

	// init dmss
	this->disconnectme_session = NULL;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Declare/Initialize Thread Members %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#ifdef BP_BLOCKCLASS_INCLUDE_USE_OLD_PTHREAD_METHODS

	// set threads to null
	this->threads = NULL;

	// init the thread semaphore
	sem_init(&this->thread_info.lock, 0, 1);
	this->thread_info.total_running_threads = 0;


#endif


#ifdef BP_BLOCKCLASS_INCLUDE_USE_NEW_PTHREAD_METHODS

	// create new thread manager
	this->thread_manager = BP_CreateThreadManager("block thread manager", "thread manager");

#endif

#ifdef BP_BLOCKCLASS_INCLUDE_USER_DB

	// nullify user info pointer
	this->user_info = NULL;

#endif

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Command Injection Assessment %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// command injection assessment
	this->cmd_injection_assessment = NULL;

}

// Default deconstructor (no memory should escape this).  All
// of the methods called below do initial null checks to ensure
// that the parameters they're being called on, are set.
Block::~Block()
{

	// enter the critical section
	BP_BLOCK_ENTER_CRITICAL_SECTION;

	// ensure we destroy the sha1 data if necessary
	if(this->sha1_ascii)
	{
		tq_free(this->tq, this->sha1_ascii);
		this->sha1_ascii = NULL;
	}

#ifdef BP_BLOCKCLASS_INCLUDE_USER_DB

	if(this->user_info)
	{
		this->destroyAuthUserData();
	}

#endif // BP_BLOCKCLASS_INCLUDE_USER_DB



#ifdef BP_BLOCKCLASS_INCLUDE_BING_SCRAPING_INTERFACE
	this->destroyBingSession();
#endif // BP_BLOCKCLASS_INCLUDE_BING_SCRAPING_INTERFACE


#ifdef BP_BLOCKCLASS_INCLUDE_GOOGLE_SCRAPING_INTERFACE
	this->destroyGoogleSession();
#endif // BP_BLOCKCLASS_INCLUDE_GOOGLE_SCRAPING_INTERFACE


#ifdef BP_BLOCKCLASS_INCLUDE_YAHOO_SCRAPING_INTERFACE
	this->destroyYahooSession();
#endif // BP_BLOCKCLASS_INCLUDE_YAHOO_SCRAPING_INTERFACE



	// destroy buffer chain (if set)
	this->DestroyChain();

	// destroy local errors and warnings (if any exist)
	this->DestroyErrorsAndWarnings();

	// destroy usable proxy set if they exist
	this->DestroyUsableProxySet();

	// destroly dynamic web result stack where clauses
	this->destroyWebResultStackWhereClauses();

	// check web assessment (only destroy extraction or assessment, as assessment
	// destroy method auto destroys the extraction)
	if(this->webAssessment)
	{

		// destroy the web assessment
		this->DestroyWebAssessment(false, true, true);
		this->webAssessment = NULL;
		this->htmlExtraction = NULL;
	}

	if(this->htmlExtraction)
	{
		this->HtmlDestroyExtraction();
		this->htmlExtraction = NULL;
	}

	if(this->mysqlSession)
	{
		this->mysqlDestroyStatement();
		this->MysqlClose();
		this->mysqlSession = NULL;
	}


	this->destroyWebJITExpressionData();
	this->SockDestroy();
	this->File_DestroyFileList();
	this->File_DestroySearchList();


	this->destroyProxyDBRelatedData();
	this->destroyWebResultStack();



	if(this->unparsedHtml)
	{
		tq_free(this->tq, this->unparsedHtml);
		this->unparsedHtml = NULL;
		this->unparsedHtmlLen = 0;
	}

	if(this->result_stack_builder)
	{
		delete this->result_stack_builder;
		this->result_stack_builder = NULL;
	}

	// destroy network handle objects
	if (this->net_handle)
	{
		BP_DestroyNetworkHandle(this->net_handle);
		net_handle = NULL;
	}

	// this is where you could bpfree non-free'd pointers
	if(this->gc)
	{
		BP_DestroyGarbageCollector(this->gc);
		this->gc = NULL;
	}

	// destroy web result data
	this->destroyWebResultStack();

	// destroy jit expression data if necessary
	this->destroyWebJITExpressionData();

	// destroy proxy db related data if necessary
	this->destroyProxyDBRelatedData();

	// close http session if necessary (destroys all data)
	this->HttpCloseDestroySession();

	// destroy command and control session if set
	if(this->cc_session)
	{
		BP_DestroyCCSession(this->cc_session);
		this->cc_session = NULL;
	}

	// destroy the fd set where necessary
	if(this->fdSet)
	{
		tq_free(this->tq, this->fdSet);
		this->fdSet = NULL;
	}

	// destroy got contents if set
	if(this->got_contents)
	{
		tq_free(this->tq, this->got_contents);
		this->got_contents = NULL;
	}

	// only continue if we have a valid allocator
	if(!this->tq)
		return;

	if(this->tq->total_allocations != 0)
	{

		// display debug information if so set
		if(BLOCK_CLASS_DEBUG_DECONSTRUCTOR_MEMORY_MESSAGES)
		{
			printf("\n [+] Error: Destroying block which still currently has allocations.");
			printf("\n [+] Information Below: ");
			BP_DisplayLinkL(this->tq);
		}

	}


#ifdef BP_BLOCKCLASS_INCLUDE_USE_OLD_PTHREAD_METHODS

	// destroy thread info lock
	sem_destroy(&this->thread_info.lock);

	// destroy thread items if they're set
	this->DestroyAllThreads();


#endif


#ifdef BP_BLOCKCLASS_INCLUDE_USE_NEW_PTHREAD_METHODS

	// destroy the thread manager if set
	if(this->thread_manager)
	{

		// rejoin all threads
		BP_RejoinAllThreads(this->thread_manager);

		// create new thread manager
		BP_DestroyThreadManager(this->thread_manager);

		// set the thread manager to null
		this->thread_manager = NULL;

	}


#endif

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Command Injection Assessment %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we have a command injection assessment, destroy it here
	if(this->cmd_injection_assessment)
	{

		// destroy the command injection session here
		this->DestroyCommandInjectionSession();

		// command injection assessment
		this->cmd_injection_assessment = NULL;

	}

	// init dupe lookups
	this->disable_result_lookups_due_to_duplicate_key_contention_fix = 0;

	// destroy all allocated memory
	BP_DestroyLinkL(this->tq, BP_TRUE);

	// set tq to null
	this->tq = NULL;

	// leave critical section before destroying system hreg
	BP_BLOCK_LEAVE_CRITICAL_SECTION;

	// destroy the hash table registry
	BP_DestroyHashTableRegistry(this->system_hreg);

}

// If the block object is not allocated with "new" the constructor
// routine isn't run to initialize local variables, this function allows
// for those local variables to be initialized through routine instead
// in order to make things much easier then manual initializations.
// THIS IS SEVERELY DEPRECATED, DO NOT USE THIS
// THIS IS SEVERELY DEPRECATED, DO NOT USE THIS
// THIS IS SEVERELY DEPRECATED, DO NOT USE THIS
// THIS IS SEVERELY DEPRECATED, DO NOT USE THIS
// THIS IS SEVERELY DEPRECATED, DO NOT USE THIS
// THIS IS SEVERELY DEPRECATED, DO NOT USE THIS
int Block::InitLocals()
{

	// Initialize all pointers to null
	fdSet      = NULL;
	fileList   = NULL;
	searchList = NULL;
	net_handle  = NULL;


#if USE_BLOCKPARTY_DEBUGGING == 1



	#if BLOCKPARTY_DEBUGGING_TARGET_64BIT == 1



	#endif // #if BLOCKPARTY_DEBUGGING_TARGET_64BIT == 1


	#if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1
	dbgHandle        = NULL;
	triggerFile      = NULL;
	exploitAnalysis  = NULL;
	instructionTable = NULL;
	instructions     = NULL;
	syscallLookup    = NULL;
	#endif // #if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1

#endif // USE_BLOCKPARTY_DEBUGGING == 1

	mysqlResults = NULL;
	mysqlSession = NULL;
	httpSession = NULL;
	htmlExtraction = NULL;
	webAssessment = NULL;

	// errors and warnings
	errorCount = 0;
	errorStrings = NULL;
	warningCount = 0;
	warningStrings = NULL;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %% !!! END CONSTRUCTOR/DECONSTRUCTORS !!! %%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// -------------------UTILITY FUNCTIONS-------------------------//

// Prints out the blocks internal bufferList according to type
BP_ERROR_T Block::PrintList()
{

	// ensure we have a bufferlist
	if(!this->bufferList)
		return ERR_FAILURE;

	// display all the bufferlist data
	// DataListPrintAll(this->bufferList->first);

	// return indicating success
	return ERR_SUCCESS;

}

// attempt to destroy the buffer list
int Block::DestroyChain()
{


	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% ERROR  FUNCTIONS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// add an element to error array
int Block::AddErrorToStack(char *errorMsg)
{

	// enter critical section
	BP_BLOCK_ENTER_CRITICAL_SECTION;

	if(!this->msg_logging_enabled)
		return ERR_FAILURE;

	// display errors immediately
	if(this->display_errors_immediately)
		printf("\nBlock Error: %s", errorMsg);

	// add error if parameter is null
	if (errorMsg == NULL)
		this->AddErrorToStack((char *) "Parameter passed to Block::AddErrorToStack() was NULL.");

	// increment the error count
	this->errorCount++;

	// bprealloc the buffer to the proper size
	this->errorStrings = (char **) tq_realloc(this->tq, this->errorStrings, sizeof(char *) * this->errorCount, BPLN);

	// set the index
	this->errorStrings[this->errorCount - 1] = tq_strndup(this->tq, errorMsg, BP_CLASS_MAX_ERROR_STR_LEN, BPLN);

	// leave the critical section
	BP_BLOCK_LEAVE_CRITICAL_SECTION;

	// return indicating success
	return ERR_SUCCESS;

}

// add a warning to the error stack
int Block::AddWarningToStack(char *warningMsg)
{

	// enter critical section
	BP_BLOCK_ENTER_CRITICAL_SECTION;

	if(!this->msg_logging_enabled)
		BP_BLOCK_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// display errors immediately
	if(this->display_warnings_immediately)
		printf("\nBlock Warning: %s", warningMsg);

	// add error if parameter is null
	if (warningMsg == NULL)
		this->AddErrorToStack((char *) "Parameter passed to Block::AddWarningToStack() was NULL.");

	// increment the warning count
	this->warningCount++;

	// bprealloc the buffer to the proper size
	this->warningStrings = (char **) tq_realloc(this->tq, this->warningStrings, sizeof(char *) * this->warningCount, BPLN);

	// set the index
	this->warningStrings[this->warningCount - 1] = tq_strndup(this->tq, warningMsg, BP_CLASS_MAX_WARNING_STR_LEN, BPLN);

	// leave critical section
	BP_BLOCK_LEAVE_CRITICAL_SECTION;

	// return indicating success
	return ERR_SUCCESS;

}

// Print errors to stdout
int Block::DisplayErrors()
{

	// enter critical section
	BP_BLOCK_ENTER_CRITICAL_SECTION;

	// check error count first
	if (this->errorCount == 0)
		BP_BLOCK_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// display error stack
	int i = 0;
	for (i = 0; i < this->errorCount; i++)
	{
		printf("\nError (%i) : %s", i, this->errorStrings[i]);
	}

	// return indicating success
	BP_BLOCK_LEAVE_CRITICAL_SECTION;
	return ERR_SUCCESS;

}

// Print warnings to stdout
int Block::DisplayWarnings()
{

	// enter critical section
	BP_BLOCK_ENTER_CRITICAL_SECTION;

	// check error count first
	if (this->warningCount == 0)
		BP_BLOCK_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// display error stack
	int i = 0;
	for
	(
		i = 0;
		i < this->warningCount;
		i++
	)
	{
		printf("\nWarning (%i) : %s", i, this->warningStrings[i]);
	}

	BP_BLOCK_LEAVE_CRITICAL_SECTION;
	return ERR_SUCCESS;

}

// destroys errors
int Block::DestroyErrors()
{

	// enter critical section
	BP_BLOCK_ENTER_CRITICAL_SECTION;

	// check to see if we have errors
	if(this->errorCount)
	{

		// walk all errors and destroy data
		size_t i = 0;
		for(; i < this->errorCount; i++)
			tq_free(this->tq, this->errorStrings[i]);

		// destroy the error strings array
		tq_free(this->tq, this->errorStrings);

		// set pointer to null
		this->errorStrings = NULL;

		// reset error count
		this->errorCount = 0;

	}

	// leave critical section and exit function
	BP_BLOCK_LEAVE_CRITICAL_SECTION
	return ERR_SUCCESS;

}

// destroys warnings
int Block::DestroyWarnings()
{

	// enter critical section
	BP_BLOCK_ENTER_CRITICAL_SECTION;

	// ensure we have a set warning count
	if(this->warningCount)
	{

		// walk warnings and free
		size_t i = 0;
		for(; i < this->warningCount; i++)
			tq_free(this->tq, this->warningStrings[i]);

		// destroy the holder array
		tq_free(this->tq, this->warningStrings);

		// set pointer to null
		this->warningStrings = NULL;

		// reset warnning count to zero
		this->warningCount = 0;

	}

	// return indicating success
	BP_BLOCK_LEAVE_CRITICAL_SECTION
	return ERR_SUCCESS;

}

// calls destroy errors and destroy warnings successively.
int Block::DestroyErrorsAndWarnings()
{
	BP_BLOCK_ENTER_CRITICAL_SECTION
	this->DestroyWarnings();
	this->DestroyErrors();
	BP_BLOCK_LEAVE_CRITICAL_SECTION
}


// Uses fork() to set a process exit timeout
BP_ERROR_T Block::setProcessExecutionTimeout(size_t timeout_secs)
{

	// DEV NOTE: Don't use CRITICAL SECTIONS IN THIS METHOD

	// set mask
	sigset_t mask;
	sigset_t orig_mask;
	struct timespec timeout;
	pid_t pid;

	sigemptyset (&mask);
	sigaddset (&mask, SIGCHLD);

	// set the process mask
	if (sigprocmask(SIG_BLOCK, &mask, &orig_mask) < 0)
	{
		perror ("sigprocmask");
		return ERR_SUCCESS;
	}

	// fork here
	pid = fork ();

	// exit here if we're the child
	if(pid == 0)
		return ERR_SUCCESS;

	// set timeout in seconds
	timeout.tv_sec = timeout_secs;
	timeout.tv_nsec = 0;

	// perform wait loop
	do
	{

		if (sigtimedwait(&mask, NULL, &timeout) < 0)
		{

			if (errno == EINTR)
			{
				/* Interrupted by a signal other than SIGCHLD. */
				continue;
			}
			else if (errno == EAGAIN)
			{
				printf ("Timeout, killing child\n");
				kill (pid, SIGKILL);
				return ERR_FAILURE;
			}
			else
			{
				perror ("sigtimedwait");
				return ERR_SUCCESS;
			}

		}

		break;

	} while (1);

	// wait on the pid
	if (waitpid(pid, NULL, 0) < 0)
	{
		perror ("waitpid");
		return ERR_FAILURE;
	}

	// return indicating failure
	return ERR_FAILURE;

}


// Tests to see if a regulare expression item is matched in a null
// pointer terminated array of strings.  Null pointers are not searched.
// This function returns 0 on error, and a string pointer position on
// success.  Notice: 0 index in ptr array is the first position (aka 1).
size_t Block::matchRegexpInBlacklist(char ** blacklist, char *item)
{

	// ensure we have items
	if(!blacklist || !item)
		return 0;
	if(!blacklist[0])
		return 0;

	// get the length of the blacklist
	size_t blacklist_len = this->countNullTerminatedPtrArray(blacklist);

	// run the blacklist testing algorithm
	return BP_StringMatchesBlacklistREGEXP(blacklist, blacklist_len, item);

}

// Counts pointers in a null terminated pointer array and returns in size_t.
size_t Block::countNullTerminatedPtrArray(char **count_array)
{

	// ensure we have an array to count
	if(!count_array)
		return 0;

	// ensure we have at least one position
	if(!count_array[0])
		return 0;

	// count the items
	size_t n = 0;
	for(; count_array[n]; n++){}

	// return indicating success
	return n;

}

// Generates a SHA1 has provided given input.
bool Block::GenSHA1(char *in, size_t in_size)
{

	// ensure we have an in pointer and a size
	if(!in || !in_size)
		return ERR_FAILURE;

	// ensure we destroy the sha1 data if necessary
	if(this->sha1_ascii)
	{
		tq_free(this->tq, this->sha1_ascii);
		this->sha1_ascii = NULL;
	}

	// now create ascii buffer
	this->sha1_ascii = (char *) tq_calloc(this->tq, 41, 1, BPLN);
	unsigned char obuf[20];

	// create hash
	SHA1((unsigned char *) in, in_size, (unsigned char *) &obuf);

	// create ascii hash from binary
	size_t i = 0;
	for(; i < 20; i++)
	{
		snprintf((char *) &this->sha1_ascii[bpstrlen(this->sha1_ascii)], 4, "%.02x", obuf[i]);
	}

	// return indicating success
	return ERR_SUCCESS;

}

// relies on FileGetContents(), generates an ascii sha1 from a file
bool Block::GenSHA1FromFile(char *filename)
{

	// ensure we have a filename
	if(!filename)
		return ERR_FAILURE;

	// ensure the filename exists and is readable
	if(BP_FileAtPathExistsAndIsReadable(filename) != ERR_SUCCESS)
		return ERR_FAILURE;

	// retrieve contents
	Block * get_contents = new Block();

	// attempt to get contents
	if(get_contents->FileGetContents(filename) == ERR_FAILURE)
	{

		// destroy the block before returning
		delete get_contents;

		// return indicating failure
		return ERR_FAILURE;

	}

	// create sha value using local tailqueue allocator
	if(!this->GenSHA1(get_contents->got_contents, get_contents->got_contents_size))
	{
		delete get_contents;
		return ERR_FAILURE;
	}

	// destroy the block used for gathering file data
	delete get_contents;

	// return the content of sha1 routine
	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%% SOCKET FUNCTIONS %%%%%%%%%%%%%%%%%%%%%%%%%%

#ifdef BP_BLOCKCLASS_INCLUDE_NETWORK_BINDINGS

// connects to a host on port using protocol specified
int Block::SockConnect(char * destAddr, short destPort, int proto)
{

	// ensure we have parameter values
	if(!destAddr)
		return ERR_FAILURE;
	if(!destPort)
		return ERR_FAILURE;

	// First see if we have an existing net handle, if we do, close and
	// destroy resultant data.
	if (this->net_handle)
	{

		// close the socket
		this->SockClose();

		// destroy the socket
		this->SockDestroy();

		// set pointer to NULL
		this->net_handle = NULL;

	}

	// create a new network socket
	this->net_handle = BP_NetCreateSocket(destAddr, destPort, proto, 0, NULL);

	// ensure it was created OK
	if (!this->net_handle)
	{
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}

// listens for incomming connections using specified protocol
int Block::SockListen(char * srcAddr, short srcPort, int proto)
{

	// First see if we have an existing net handle, if we do, close and
	// destroy resultant data.
	if (this->net_handle)
	{

		// close the socket
		this->SockClose();

		// destroy the socket
		this->SockDestroy();

		// set pointer to NULL
		this->net_handle = NULL;

	}

	// create new listener socket
	this->net_handle = BP_NetCreateSocket(srcAddr, srcPort, proto, BP_NETWORK_LISTEN_SOCKET, NULL);

	// ensure that we created the entwork handle
	if (!this->net_handle)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// accepts and assigns the current blocks net_handle to a new accepted P_NETWORK
// structure that was created by validating a listening P_NETWORK
// established socket structure argument.
int Block::SockAccept(P_BP_NETWORK_HANDLE acceptHandle)
{

	// ensure we have a network handle
	if (net_handle != NULL)
	{
		SockClose();
		SockDestroy();
	}

	// accept on the handle, and reassign
	this->net_handle = BP_NetAccept(acceptHandle);

	// ensure we have a network handle
	if (!this->net_handle)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// accept on a socket
int Block::SockAccept()
{

	// ensure we have a network handle
	if(!this->net_handle)
		return ERR_FAILURE;

	// accept on the local net_handle object
	P_BP_NETWORK_HANDLE acceptSocket = BP_NetAccept(net_handle);

	// ensure the socket was created
	if (acceptSocket == NULL)
		return ERR_FAILURE;

	// destroy the old net_handle object
	SockClose();

	// reassign the socket
	net_handle = acceptSocket;

	// return indicating success
	return ERR_SUCCESS;

}

// close a socket connection
int Block::SockClose()
{

	// ensure we have a network handle
	if (!this->net_handle)
		return ERR_FAILURE;

	// shutdown the socket
	shutdown(net_handle->sock, 2);

	// close the socket
	if (close(net_handle->sock) != 0)
	{
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}

// destroy the current network handle
int Block::SockDestroy()
{

	// ensure we have a network handle to destroy
	if(!this->net_handle)
		return ERR_FAILURE;

	// destroy the network handle
	BP_DestroyNetworkHandle(this->net_handle);

	// set local pointer to null
	this->net_handle = NULL;

	// return indicating success
	return ERR_SUCCESS;

}

// this is the primary function around which this fuzzer lib is
// based.  It will play a list of buffers, including having the
// ability to recieve data within a block.  This is the key difference
// between SendBlock and Play.  Play will actually recieve buffers as
// well.
int Block::SockPlay()
{

	/*
	P_BP_DATA_LIST first = NULL;

	// buffer used to flush the remainder of data being read
	// from the pipe.
	char readBuff[1000];
	memset(readBuff, 0x00, 1000);

	if (bufferList == NULL) {
		return ERR_FAILURE;
	}

	first = bufferList->first;
	for (bufferList = bufferList->first; bufferList != NULL; bufferList = bufferList->next) {

		// always read first
		if (bufferList->dataType == BP_DATA_ENTRY_TYPE_READ) {

			if (NetRead(net_handle, bufferList->readData, bufferList->dataSize) == -1) {
				return ERR_FAILURE;
			}

		} else if (bufferList->dataType != BP_DATA_ENTRY_TYPE_READ) {

			if (bufferList->binData == NULL) {
				if (NetSend(net_handle, bufferList->data, bufferList->dataSize) == -1) {
					return ERR_FAILURE;
				}
			} else {
				// send binary data if found
				if (NetSend(net_handle, bufferList->binData, bufferList->dataSize) == -1) {
					return ERR_FAILURE;
				}
			}

		}

	}

	bufferList = first;
	*/

	return ERR_SUCCESS;
}

int Block::SockSendBlock()
{

	/*
	P_BP_DATA_LIST first = bufferList->first;

	for (bufferList = bufferList->first; bufferList != NULL; bufferList = bufferList->next) {
		// do not send buffer (aka skip it) if its a RECV type
		// key difference from play here
		if (bufferList->dataType != BP_DATA_ENTRY_TYPE_READ) {
			if (NetSend(net_handle, bufferList->data, bufferList->dataSize) == -1) {
				break;
			}
		}

	}

	bufferList = first;
	*/

	return ERR_SUCCESS;
}

// send data over a socket
int Block::SockSend(char *data, int size)
{

	// ensure we have a network handle
	if(!this->net_handle)
		return ERR_FAILURE;

	// attempt to send the data
	if (BP_NetSend(this->net_handle, data, size) == -1)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// Recieve data over a connected socket.
int Block::SockRecv(char *data, int size)
{

	// ensure we have a network handle
	if(!this->net_handle)
		return ERR_FAILURE;

	// attempt to read data
	if (BP_NetRecv(this->net_handle, data, size) == -1)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

#endif // BP_BLOCKCLASS_INCLUDE_NETWORK_BINDINGS


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%% SSL FUNCTIONS %%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#ifdef BP_BLOCKCLASS_INCLUDE_NETWORKSSL_BINDINGS

// loads all needed SSL components
BP_ERROR_T Block::SSLInit()
{

	BP_ERROR_T err = ERR_FAILURE;

	err = BP_SSLLibInit();
	if (err != ERR_SUCCESS)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// connects on existing net_handle using sslMethod
BP_ERROR_T Block::SSLConnect(SSL_METHOD *sslMethod)
{

	int err = 0;

	// set chosen context
	err = BP_SSLSetContext(net_handle, sslMethod);
	if (err != ERR_SUCCESS)
		return ERR_FAILURE;

	// attempt ssl connection
	err = BP_SSLConnect(net_handle);
	if (err != ERR_SUCCESS)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// same as SockSendBlock but over a SSL oriented connection instead
BP_ERROR_T Block::SSLSendBlock()
{

	/*

	// ensure we have a buffer list
	if(!this->bufferList)
		return ERR_FAILURE;

	// ensure we have a first pointer
	if(!this->bufferList->first)
		return ERR_FAILURE;

	// create simple retaining pointer
	P_BP_DATA_LIST first = this->bufferList->first;

	// walk buffer list and send out data
	for
	(
			bufferList = bufferList->first;
			bufferList != NULL;
			bufferList = bufferList->next
	)
	{

		// do not send buffer (aka skip it) if its a RECV type
		// key difference from play here
		if (bufferList->dataType != BP_DATA_ENTRY_TYPE_READ)
		{

			// send the data
			if (SSLSend(bufferList->data, bufferList->dataSize) == -1)
			{
				break;
			}

		}

	}

	// reset pointer
	bufferList = first;
	*/

	// return indicating success
	return ERR_SUCCESS;

}

// plays a SSL block, reads included
BP_ERROR_T Block::SSLPlay()
{

	/*
	// ensure we have a buffer list
	if(!this->bufferList)
		return ERR_FAILURE;

	// ensure we have a first pointer
	if(!this->bufferList->first)
		return ERR_FAILURE;

	// create simple retaining pointer
	P_DATA_LIST first = this->bufferList->first;

	// walk the list
	for
	(
			bufferList = bufferList->first;
			bufferList != NULL;
			bufferList = bufferList->next
	)
	{
		// do not send buffer (aka skip it) if its a RECV type
		// key difference from play here
		if (bufferList->dataType != BP_DATA_ENTRY_TYPE_READ)
		{

			if (bufferList->dataType != BP_DATA_ENTRY_TYPE_BINARY)
			{

				if (SSLSend(bufferList->readData, bufferList->dataSize) == -1)
				{
					return ERR_FAILURE;
				}

			} else
			{

				if (SSLSend(bufferList->binData, bufferList->dataSize) == -1)
				{
					return ERR_FAILURE;
				}

			}

		} else if (bufferList->dataType == BP_DATA_ENTRY_TYPE_READ)
		{

			if (SSLRecv(bufferList->data, bufferList->dataSize) == -1)
			{
				return ERR_FAILURE;
			}
		}
	}

	// reset bufferlist pointer
	bufferList = first;
	 */

	// return indicating success
	return ERR_SUCCESS;

}

// sends data out of a ssl enabled socket
int Block::SSLSend(char *data, int size)
{

	// check to see if we have parameters
	if(!data || !size)
		return ERR_FAILURE;

	// write data
	int length = BP_SSLWrite(net_handle, (BYTE *) data, size);

	// return the written data length
	return length;

}

// recieves data on a ssl enabled socket
int Block::SSLRecv(char *data, int size)
{

	// ensure we have data or size
	if(!data || !size)
		return ERR_FAILURE;

	// read the data here
	int length = BP_SSLRead(net_handle, (BYTE *) data, size);

	// return the read size
	return length;

}

#endif // BP_BLOCKCLASS_INCLUDE_NETWORKSSL_BINDINGS

// -------------------OPERATORS--------------------------------//
/*
// equals operator
Block& Block::operator =(char *incommingBuffer)
{


	bufferList = NULL;
	// bufferList = DataListFromString(incommingBuffer);
	return *this;

}

Block& Block::operator =(Block incommingAppendingBlock)
{

	if (bufferList != NULL)
	{
		// DestroyDataList(bufferList->first);
	}

	// DestroyDataList(bufferList);
	// bufferList = DataListDup(incommingAppendingBlock.bufferList);

	return *this;

}

// plus operator
Block& Block::operator +(char *incommingBuffer)
{

	if (bufferList == NULL)
	{
		// bufferList = DataListFromString(incommingBuffer);
	}
	else
	{
		// DataListAppendList(bufferList, DataListFromString(incommingBuffer));
	}

	return *this;

}

Block& Block::operator +(Block incommingAppendingBlock)
{

	if (bufferList == NULL)
	{
		// bufferList = DataListDup(incommingAppendingBlock.bufferList);
	}
	else
	{
		// DataListAppendList(bufferList, DataListDup(incommingAppendingBlock.bufferList));
	}

	return *this;

}

// plus equals operator
Block& Block::operator+=(char *incommingBuffer)
{

	if (bufferList == NULL)
	{
		// bufferList = DataListFromString(incommingBuffer);
	}
	else
	{
		// DataListAppendList(bufferList->first, DataListFromString(incommingBuffer));
	}

	return *this;
}

// overridden += operator
Block& Block::operator+=(Block incommingAppendingBlock)
{

	if (bufferList == NULL)
	{
		// bufferList = DataListDup(incommingAppendingBlock.bufferList);
	}
	else
	{
		// DataListAppendList(bufferList, DataListDup(incommingAppendingBlock.bufferList));
	}
	return *this;
}

// adds a read block to our list, added in with operator group due to similarities
// in operational nature.
int Block::AddReadBlock(int size, bool destroyExisting)
{

	// this should be size
	return ERR_SUCCESS;
}

// Adds an arbitrary binary block (new-standard, depreciating old blocks)
int Block::AddBinaryBlock(unsigned char * buff, size_t len, bool destroyExisting)
{



	return ERR_SUCCESS;

}

// returns a pointer to a read block structure that can then be used to
// view or compare the buffer contents from the read block at the position
// specified.  Positions start from the 0'th occurance in the block, so AccessReadBlock(0);
// would effectively give you the contents of the first read buffer.

P_BP_DATA_LIST Block::AccessReadPos(int blockPos)
{

	return NULL;

}
*/

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%% FUZZING FUNCTIONS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#ifdef BP_BLOCKCLASS_INCLUDE_FUZZING_BINDINGS

// Creates a block of random data, if destroyExisting list is not 0 or NULL
// it will append the data to the list
Block& Block::GenRandomData(int size, int destroyExistingList)
{



	return *this;

}

// Goes through the buffer list and regenerates all blocks
// containing random data, with new random data.  If randomizeSize
// is set to true, it will create random ammounts of random data.
// in the list instead of generating the previous size allocated
// to the list random portions.

/*
 int Block::RegenRandomData(bool randomizeSize){


 return ERR_SUCCESS;
 }
 */

// generates a buffer containing format string characters of , specifier type
// (aka: "x for %x" or "c for %c".  If the size is not divisible by two it is
// rounded up.  The destroyExistingList parameter is used exclusively to
// destroy the list already contained in the block, destroying all contained data.
Block& Block::GenFormatData(int size, char specifier, int destroyExistingList)
{


	return *this;
}

// Generates a buffer containing a sequence of characters (such as "../")
// that are seqLen bytes long, read from the seq pointer, iterated over
// count number of times. The destroyExistingList parameter is used exclusively to
// destroy the list already contained in the block, destroying all contained data.
Block& Block::GenSequenceData(int count, char *seq, int seqLen, int destroyExistingList)
{


	return *this;
}

#endif // BP_BLOCKCLASS_INCLUDE_FUZZING_BINDINGS


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%% FILE BINDINGS %%%%%%%%%%%%%%%%%%%%%%%%%%

#ifdef BP_BLOCKCLASS_INCLUDE_FILE_BINDINGS

// executes the given file at path provided that the
// user has privileges to execute the file. If the
// returnFds flag is set to true, it will return all
// standard file streams (stdin/out/err) in the classes
// fdSet struct.
int Block::File_Execute(char *filePath, char **params, char **envp, BP_BOOL returnFds)
{

	// destroy the fd set if we already have one
	if(this->fdSet)
	{
		tq_free(this->tq, this->fdSet);
		this->fdSet = NULL;
	}

	// exec the file
	this->fdSet = BP_FExec(filePath, params, envp, returnFds);

	// link the file descriptor set into our tail queue
	tq_link(this->tq, this->fdSet, 4, BPLN);

	// check if we have fds
	if (returnFds == true)
	if (fdSet == NULL)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// searches the filesystem from specified rootDirPath for all
// the types that match the type specified.
int Block::File_FindAllType(char *rootDirPath, BP_FILETYPE type, bool recursive)
{

	// ensure we have a root directory path
	if(!rootDirPath)
		return ERR_FAILURE;

	// check to see if we have a file list and destroy
	// if necessary.
	if (this->fileList)
	{
		BP_DestroyFileList(this->fileList);
		this->fileList = NULL;
	}

	// create file list
	this->fileList = BP_FSFindAllType(rootDirPath, type, recursive, NULL);

	// return indicating success
	return ERR_SUCCESS;

}

// searches the existing list for conditions, stores result in searchList
// note: will destroy previous contents of the searchList element.
int Block::File_ListSearch(char *name, bool execute, bool write, bool read, bool elf, uid_t uid, gid_t gid)
{

	// search the list
	if (this->searchList != NULL)
	{
		BP_DestroyFileList(this->searchList);
		this->searchList = NULL;
	}

	// ensure we have a file list
	if (this->fileList == NULL)
		return ERR_FAILURE;

	// attempt to search all files
	this->searchList = BP_FSSearchAll(fileList, name, execute, write, read, elf, uid, gid);

	// return indicating success
	return ERR_SUCCESS;

}

// prints out all files in the fileList
int Block::File_PrintFileList()
{

	// ensure we have a file list
	if(!this->fileList)
		return ERR_FAILURE;

	// attempt to print the file list
	BP_FileListPrintAll(fileList);

	// return indicating success
	return ERR_SUCCESS;

}

// prints out all files in the searchList
int Block::File_PrintSearchList()
{

	// ensure we have a search list
	if(!this->searchList)
		return ERR_FAILURE;

	// attempt to print the file list
	BP_FileListPrintAll(this->searchList);

	// return indicating success
	return ERR_SUCCESS;

}

// destroys the objects file list
int Block::File_DestroyFileList()
{

	// ensure we have a file list
	if(!this->fileList)
		return ERR_FAILURE;

	// initialize error
	int err = 0;

	// attempt to destroy the file list
	err = BP_DestroyFileList(fileList->first);

	// set the file list to null
	fileList = NULL;

	// return error code
	return err;

}

// destroys the objects searchList
int Block::File_DestroySearchList()
{

	int err = 0;
	if (this->searchList)
	{

		// destroy the file list
		err = BP_DestroyFileList(searchList->first);

		// nullify the pointer
		this->searchList = NULL;

		// return error code
		return err;

	}

	// return indicating failure by default
	return ERR_FAILURE;

}

// destroys both lists: err condition does not account for failures
int Block::File_DestroyLists()
{

	if (searchList != NULL)
	{
		BP_DestroyFileList(searchList->first);
		searchList = NULL;
	}

	if (fileList != NULL)
	{
		BP_DestroyFileList(fileList->first);
		fileList = NULL;
	}

	// return indicating success
	return ERR_SUCCESS;

}

// ---------------------------------------
// functions for manipulating file descriptors:

// Note: These are used to operate on resources
// local to the class.

// Sends the entire bufferList in the object to the
// specified file descriptor.
int Block::FD_SendBlock(int fd)
{

	/*

	// set first list position
	P_BP_DATA_LIST first = bufferList->first;

	// walk buffer list
	for
	(
			bufferList = bufferList->first;
			bufferList != NULL;
			bufferList = bufferList->next
	)
	{

		// do not send buffer (aka skip it) if its a RECV type
		// key difference from play here
		if (bufferList->dataType != BP_DATA_ENTRY_TYPE_READ)
		{

			if (FD_Write(fd, bufferList->readData, bufferList->dataSize) == -1)
			{
				break;
			}

		}

	}

	bufferList = first;

	*/

	return ERR_SUCCESS;

}

// plays a block over specified read/write file descriptors reads included
int Block::FD_Play(int fdRead, int fdWrite)
{


	// return indicating success
	return ERR_SUCCESS;

}

// opens a file descriptor, and returns a stream FILE object
// use FClose to close the descriptor not FDClose
int Block::FD_Open(int fd, char *mode)
{

	// check mode
	if(!mode)
		return ERR_FAILURE;

	// attempt to open the file descriptors
	this->file = fdopen(fd, mode);
	if (this->file == NULL)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// closes an open file descriptor (not a FILE)
int Block::FD_Close(int fd)
{

	// attempt to close the file descriptor
	if (BP_FDClose(fd) != ERR_SUCCESS)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// Writes to a given file descriptor
int Block::FD_Write(int fd, char *buff, int size)
{

	// ensure we have a buffer and size parameter
	if(!buff || !size)
		return ERR_FAILURE;

	// attempt to read from file descriptor
	int bytesWritten = BP_FDWrite(fd, buff, size);

	// return the number of bytes written
	return bytesWritten;

}

// Reads from a given file descriptor
int Block::FD_Read(int fd, char *buff, int size)
{

	// ensure we have a buffer and size parameter
	if(!buff || !size)
		return ERR_FAILURE;

	// attempt to read from the fd
	int bytesRead = BP_FDRead(fd, buff, size);

	// return the number of bytes read
	return bytesRead;

}

// ---------------------------------------
// functions for manipulating file streams

// Opens a file on the filesystem with accessString permissions (aka. "r", "r+", "w").  This
// routine uses the internal block file pointer
int Block::File_Open(char *rootDirPath, char *accessString)
{

	// ensure we have valid pointers
	if(!rootDirPath || !accessString)
		return ERR_FAILURE;

	// if we already have a file open in this block, close it first
	if(this->file)
		this->File_Close();

	// attempt to open the file
	this->file = fopen(rootDirPath, accessString);

	// check to see if the file was opened ok
	if (!file)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// Open a file pointer.  This does not store the file pointer in the
// local class.  Use the File_Open function if you want to use
// the internal file pointer.
FILE * Block::File_OpenFptr(char *rootDirPath, char *accessString)
{

	// check parameters
	if(!rootDirPath || !accessString)
		return NULL;

	// attempt to open the file with given access
	FILE *outFile = fopen(rootDirPath, accessString);

	// ensure we have an outfile
	if (outFile == NULL)
		return NULL;

	// return the opened file (not stored in class)
	return outFile;

}

// closes a file stream
int Block::File_Close(FILE * inFile)
{

	// ensure we have an infile
	if (!inFile)
		return ERR_FAILURE;

	if (BP_FClose(inFile) == ERR_FAILURE)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// close the current block file
int Block::File_Close()
{

	// ensure we have a file pointer
	if (!this->file)
		return ERR_FAILURE;

	// close the file
	if (BP_FClose(this->file) == ERR_FAILURE)
		return ERR_FAILURE;

	// set file pointer to null
	this->file = NULL;

	// return indicating success
	return ERR_SUCCESS;

}

// writes across a file stream
int Block::File_Write(FILE * inFile, char *buff, int size)
{

	// ensure we have valid pointers/size
	if (!inFile || !buff || !size)
		return ERR_FAILURE;

	// attempt to write content
	int bytesWritten = BP_FWrite(inFile, buff, size);

	// return the number of bytes written
	return bytesWritten;

}

// write to currently open file
int Block::File_Write(char *buff, int size)
{

	// ensure we have a local file and parameters
	if(!this->file || !buff || !size)
		return ERR_FAILURE;

	// attempt to write the bytes
	int bytesWritten = BP_FWrite(file, buff, size);

	// return the length of bytes written
	return bytesWritten;

}

// reads from a file stream
int Block::File_Read(FILE *inFile, char *buff, int size)
{

	// check parameters
	if (!inFile || !buff || !size)
		return ERR_FAILURE;

	// attempt to read the bytes
	int bytesRead = BP_FRead(inFile, buff, size);

	// return the number of bytes read
	return bytesRead;

}

// read from local file
int Block::File_Read(char *buff, int size)
{

	// check parameters and local file pointer
	if(!this->file || !buff || !size)
		return ERR_FAILURE;

	// attempt to read from local file
	int bytesRead = BP_FRead(this->file, buff, size);

	// return the number of bytes read
	return bytesRead;

}

//! retrieves file contents of a given file.
int Block::FileGetContents(char *file)
{

	// ensure we have a file
	if(!file)
		return ERR_FAILURE;

	// check to see if the file exists and is readable
	if(!BP_FileAtPathExistsAndIsReadable(file))
	{
		this->AddErrorToStack((char *) "Error. Tried to open file as readable.  Open failed.  Check file exists and is readable");
		return ERR_FAILURE;
	}

	// open file
	if(this->File_Open(file, (char *) "r") != ERR_SUCCESS)
	{
		this->AddWarningToStack((char *) "Error.  Cannot open file.");
		return ERR_FAILURE;
	}

	// create space for writer byte
	char read_byte = (char) 0x00;
	this->got_contents = NULL;
	this->got_contents_size = 0;

	// read the file byte by byte and allocate/assign (lazy)
	while(this->File_Read(&read_byte, 1))
	{

		// increment got contents size
		this->got_contents_size++;

		// realloc the buffer
		this->got_contents      = (char *) tq_realloc(this->tq, this->got_contents, this->got_contents_size+1, BPLN);

		// copy in read byte
		this->got_contents[this->got_contents_size-1] = read_byte;

		// terminate
		this->got_contents[this->got_contents_size] = 0x00;

	}

	// close the file
	this->File_Close();

	// return indicating success
	return ERR_SUCCESS;

}

#endif // BP_BLOCKCLASS_INCLUDE_FILE_BINDINGS

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%% POSIX THREADS %%%%%%%%%%%%%%%%%%%%%%%%%

#ifdef BP_BLOCKCLASS_INCLUDE_PTHREAD_BINDINGS


#ifdef BP_BLOCKCLASS_INCLUDE_USE_OLD_PTHREAD_METHODS

// creates a new thread and appends it to the threads list
int Block::CreateNewThread(void *( startRoutine)(void*), void *arg, int argLen, char * description)
{

	if(!startRoutine)
		return ERR_FAILURE;

	// create new thread list element
	P_THREAD_LIST newThread = NULL;

	// create the thread
	newThread = CreateThread(startRoutine, arg, argLen, description, &this->thread_info);

	// check to see if the thread was created or not
	if (newThread == NULL)
	{
		this->AddErrorToStack((char*) "Error in CreateNewThread: Failed to create thread.");
		return ERR_FAILURE;
	}

	// add it to the list
	if (threads != NULL)
		AppendThreadToList(threads, newThread);
	else
		threads = newThread;

	// return indicating success
	return ERR_SUCCESS;

}

// Gets a thread by the function pointer it was called with.
// If multiple threads have the same function pointer, the first
// is returned.  Returns -1 if a thread cannot be found matching the
// pointer.
#ifdef __GNUC__
int Block::GetThreadByStartRoutine(void * (*startRoutine)(void*))
{

	int i = 0;
	if (threads == NULL)
		return -1;

	P_THREAD_LIST threadFirst = threads->first;

	for (threads = threads->first; threads != NULL; threads = threads->next) {
		if (threads->startRoutine == startRoutine) {
			// return the thread number
			threads = threadFirst;
			return i;
		}
		i++;
	}

	// could not locate a suitable
	threads = threadFirst;
	return -1;

}
#endif

// Gets a thread number from the list by comparing strings to
// the list of thread descriptions.
int Block::GetThreadByDescription(char *desc, int cmpLen)
{

	int i = 0;
	if (threads == NULL)
		return -1;

	P_THREAD_LIST threadFirst = threads->first;

	for
	(
		threads = threads->first;
		threads != NULL;
		threads = threads->next
	)
	{
		if (threads->threadDescription != NULL)
		{

			if (memcmp(threads->threadDescription, desc, cmpLen) == 0)
			{
				// return the thread number
				threads = threadFirst;
				return i;
			}

		}

		i++;

	}

	// could not locate a suitable match
	threads = threadFirst;
	return -1;

}

// Gets the actual thread ptr by description.
P_THREAD_LIST Block::GetThreadPtrByDescription(char *desc, int cmpLen)
{

	//ensure we have threads
	if (this->threads == NULL)
	{

		this->AddErrorToStack((char*) "Error in GetThreadPtrByDescription: No threads to poll from.");
		return NULL;
	}

	// set thread pointer
	P_THREAD_LIST threadFirst = this->threads->first;

	// walk threads and find correct thread
	int i = 0;
	for (; threadFirst != NULL; threadFirst = threadFirst->next)
	{

		// ensure there is a description set on the thread
		if (threadFirst->threadDescription)
		if (memcmp(threadFirst->threadDescription, desc, cmpLen) == 0)
		{

			// return the thread number
			return threadFirst;

		}

	}

	// return null for failure in the case that the thread cannot be located
	return NULL;

}

// Redispatches a thread (re-runs) using the thread structure provided.
BP_ERROR_T Block::redispatchThread(P_THREAD_LIST thread)
{

	// ensure we have a thread
	if(!thread)
	{
		this->AddErrorToStack((char*) "Error in RedispatchThread: NULL thread parameter.");
		return ERR_FAILURE;
	}

	// redispatch it
	return RedispatchThread(thread);

}

// destroys the thread at the position threadNumber
int Block::DestroyThread(int threadNumber)
{

	int i = 0;
	P_THREAD_LIST threadFirst = NULL;

	if (threadNumber < 0 || threads == NULL)
		return ERR_FAILURE;

	// save the first pointer
	threadFirst = threads->first;

	for (threads = threads->first; threads != NULL; threads = threads->next)
	{

		if (i == threadNumber)
			break;

		i++;

	}

	if (threads == NULL)
	{
		threads = threadFirst;
		return ERR_FAILURE;
	}

	// destroy the individual thread element
	FreeThreadElement(threads);

	// return indicating success
	return ERR_SUCCESS;
}

// iterates through the threads list and attempts to destroy
// each thread.
int Block::DestroyAllThreads()
{

	if (this->threads == NULL)
		return ERR_FAILURE;

	// destroy the threads list
	FreeThreadList(this->threads->first);

	// reset thread pointer
	this->threads = NULL;

	// return indicating success
	return ERR_SUCCESS;

}

// prints out the thread list
void Block::PrintThreads() {

	// ensure we have a pointer to go from
	if (threads == NULL)
		return;

	// display the thread list
	PrintThreadList(threads);

}


// ====== Threaded Auto-Processor ==========================

// This processor routine will create thread_n (count) threads, with startRoutine as an entry point.  The threads
// will take arguments directly from the thread_params pointer array.  New threads will be created as threads exit
// untill each element of thread_params has been completed.  After this routine has exited, you can check the individual
// parameter structures for information regarding their arbitrary input/output/statuses.
BP_ERROR_T Block::threadedDataProcessor(size_t thread_n, void *(startRoutine)(void*), void ** thread_params, size_t param_n)
{

	// do some quick parameter validation
	if(!thread_params)
	{
		this->AddErrorToStack("Error: Tried to run threaded data processor with no parameters set.");
		return ERR_FAILURE;
	}
	if(!startRoutine)
	{
		this->AddErrorToStack("Error: Tried to run the threaded data processor with no start routine set.");
		return ERR_FAILURE;
	}
	if(!thread_n)
	{
		this->AddErrorToStack("Error: Tried to run the threaded data processor without an arbitrary thread count (count is zero)");
		return ERR_FAILURE;
	}

	// Check to see if we have an already existing threads configuration.  If we do, exit immediately.  In order
	// to use this routine an empty thread configuration state must exist.
	if(this->threads)
	{
		this->AddErrorToStack("Error: This block object seems to already have a this->threads structure.  Try destroying the existing thread configuration before dispatching this routine");
		return ERR_FAILURE;
	}


	// check to make sure we have parameters
	if(!param_n)
	{
		printf("\n [+] Error using threaded data dispatcher.  You have a zero parameter count.");
		return ERR_FAILURE;
	}

	// should never try to start more threads than the parameters we already have
	if(thread_n > param_n)
		thread_n = param_n;

	// start initial threads to process front of data
	size_t n = 0;

	// holds name buffer (zeroed on loop)
	char name[128];


	// create thread for each specified
	for(n = 0; n < thread_n; n++)
	{

		// zero out name
		memset(&name, 0x00, sizeof(name));

		// add auto thread label
		sprintf((char *)&name, "auto-thread %u", n);

		// create new thread using managed threads
		this->CreateNewThread(startRoutine, thread_params[n], sizeof(void *), name);

	}


	// flag used to exit the thread processing loop below
	size_t finished_processing = 0;

	// used to iterate thread items without destroying this->threads pointer.
	P_THREAD_LIST thread_inner_iter = NULL;

	// data index used for redispatching individual threads as they exit
	size_t data_index = 0;

	// set index as the top current running thread
	data_index = n;

	// set the initial running status (indicates whether or not we still have threads running)
	BP_ERROR_T thread_running_status = ERR_FAILURE;

	do
	{

		// flag to indicate if flags are still running
		size_t threads_are_still_running = 0;

		// current running thread count
		size_t thread_cnt = 0;

		if(!this->threads)
		{
			printf("\n [ERR] No threads?");
			break;
		}


		// walk the thread list starting from first position
		for(
				thread_inner_iter = this->threads->first;
				thread_inner_iter;
				thread_inner_iter = thread_inner_iter->next
			)
		{


			// do null checks
			while(!thread_params[data_index])
			{

				if(data_index >= param_n)
				{
					finished_processing = 1;
					break;
				}
				data_index++;

			}

			// exit if the processing is finished
			if(finished_processing)
				break;

			// attempt to join the thread if possible (unnecessary, already done in redispatcher)
			// pthread_join(thread_inner_iter->threadId, NULL);

			// if a thread has exited, attempt to (logically) redispatch the thread with
			// a new data parameter from our parameter stack.

			if(thread_inner_iter->threadState == BP_THREAD_STATUS_EXITED)
			{

				// display redispatch message
				printf("\n Redispatching thread!!:%u, %p, %u",thread_inner_iter->threadId, thread_inner_iter->sharedData, data_index);
				printf("\n Current running threads?: %u", thread_inner_iter->thread_mgmt_data->total_running_threads);
				printf("\n");

				// reset shared data pointer from parameter index
				thread_inner_iter->sharedData = thread_params[data_index];

				// redispatch the thread
				this->redispatchThread(thread_inner_iter);

				// increment the data index so that the next dispatch uses a new parameter set
				data_index++;

			}

			// check if threads are still runnin
			if(this->thread_info.total_running_threads)
			{

				// set flag
				threads_are_still_running = 1;

			}

			// Exit the loop if the index has matched our parameter count.  No new thread should be created as the
			// data has all been processed.
			if(data_index == param_n)
			{
				finished_processing = 1;
				break;
			}

		}


		// can't ever exit if threads are still running
		if(threads_are_still_running)
		{

			usleep(40);

			// check to see if we have any process exits in the case that
			// we're using this dispatcher with a sub process.
			int wait_status = 0;
			// waitpid((pid_t) -1, &wait_status, WNOHANG);
			waitpid((pid_t) -1, &wait_status, 0);
			continue;

		}

		// exit in the case of reaching the final data index
		if(data_index == (param_n))
			finished_processing = 1;

	} while(!finished_processing);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Potential Zombie Cleanups %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set status to zero
	int status = 0;

	// call waitpid to clean up any zombies that might exist
	waitpid((pid_t) -1, &status, WNOHANG);

	// wait for all threads to exit
	while(this->thread_info.total_running_threads)
	{
		usleep(10);
	}

	// printf("\n exiting! %u", this->thread_info.total_running_threads);
	if(this->thread_info.total_running_threads)
	{
		printf("\n Exiting with running threads?! %u", this->thread_info.total_running_threads);
	}

	// rejoin all the exited threads (frees memory)
	RejoinAllThreads(this->threads);

	// return indicating success
	return ERR_SUCCESS;

}


// Rejoins all threads in a thread list.
BP_ERROR_T Block::rejoinThreads()
{

	// check for threads list
	if(!this->threads)
		return ERR_FAILURE;

	// attempt to rejoin threads here
	return RejoinAllThreads(this->threads);

}

#endif

#ifdef BP_BLOCKCLASS_INCLUDE_USE_NEW_PTHREAD_METHODS


// ====== Threaded Auto-Processor ==========================

// This processor routine will create thread_n (count) threads, with startRoutine as an entry point.  The threads
// will take arguments directly from the thread_params pointer array.  New threads will be created as threads exit
// untill each element of thread_params has been completed.  After this routine has exited, you can check the individual
// parameter structures for information regarding their arbitrary input/output/statuses.
BP_ERROR_T Block::threadedDataProcessor(size_t thread_n, void *(startRoutine)(void*), void ** thread_params, size_t param_n)
{

	// the class should already have a thread manager, it's created
	// by the class constructor.  If no manager is set, exit immediately.
	if(!this->thread_manager)
		return ERR_FAILURE;



	// destroy the thread manager
	BP_DestroyThreadManager(this->thread_manager);

	// create a new thread manager
	this->thread_manager = BP_CreateThreadManager("block tdp", "block tdp");

	// ensure we can create the new thread manager
	if(!this->thread_manager)
		return ERR_FAILURE;

	// ensure we have thread parameters
	if(!thread_params)
	{
		this->AddErrorToStack("Error: Tried to run threaded data processor with no parameters set.");
		return ERR_FAILURE;
	}

	// ensure we have a start routine
	if(!startRoutine)
	{
		this->AddErrorToStack("Error: Tried to run the threaded data processor with no start routine set.");
		return ERR_FAILURE;
	}

	// ensure we have a thread count
	if(!thread_n)
	{
		this->AddErrorToStack("Error: Tried to run the threaded data processor without an arbitrary thread count (count is zero)");
		return ERR_FAILURE;
	}

	// ensure we have a prameter count
	if(!param_n)
	{
		printf("\n [+] Error using threaded data dispatcher.  You have a zero parameter count.");
		return ERR_FAILURE;
	}

	// should never try to start more threads than the parameters we already have
	if(thread_n > param_n)
		thread_n = param_n;

	// general purpose iterator
	size_t n = 0;

	// create param lens
	size_t * param_lens = (size_t *) tq_calloc
	(
			this->tq,
			sizeof(void *) * (param_n+10),
			1,
			BPLN
	);

	// set parameter lengths (seems useless, perhaps i had grand designs)
	for(n = 0; n < param_n; n++)
	{
		param_lens[n] = sizeof(void *);
	}

	// error code
	BP_ERROR_T err_code = ERR_SUCCESS;

	// create the desired number of threads (don't start them initially)
	for(n = 0; n < thread_n; n++)
	{

		// create threads (don't start them immediately)
		err_code = BP_CreateThread
		(
			this->thread_manager,
			startRoutine,
			NULL,
			0,
			BP_FALSE
		);

		if(err_code != ERR_SUCCESS)
		{
			printf("\n Failed to create thread");
		}

	}

	// asm("int3");
	// start the threaded data processor (runs all, rejoins all, quick and easy, no contentions)
	BP_ThreadedDataProcessor(this->thread_manager, thread_params, param_lens, param_n);

	// destroy the parameters
	tq_free(this->tq, param_lens);

	// return indicating success
	return ERR_SUCCESS;

}



// create a new thread
BP_ERROR_T Block::createNewThread
(
	void *   (start_routine)(void*),
	void *   arg,
	size_t   arg_len,
	BP_BOOL  start_thread_upon_creation,
	char *   name,
	char *   description
)
{

	// ensure we have a thread manager
	if(!this->thread_manager)
		return ERR_FAILURE;

	// attempt to create the thread
	return BP_CreateThread
	(
		this->thread_manager,
		start_routine,
		arg,
		arg_len,
		start_thread_upon_creation,
		name,
		description
	);


}


// rejoin all the threads in the manager
BP_ERROR_T Block::rejoinAllThreads()
{

	// ensure we have a thread manager
	if(!thread_manager)
		return ERR_FAILURE;

	// attempt to rejoin all the threads
	BP_RejoinAllThreads(this->thread_manager);

	// return indicating success
	return ERR_SUCCESS;

}


#endif


#endif // BP_BLOCKCLASS_INCLUDE_PTHREAD_BINDINGS
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%% DEBUGGER BINDINGS %%%%%%%%%%%%%%%%%%%%%%%%%%

// Functions to access platform debugging
// libraries (ptrace/win32)

#ifdef BP_BLOCKCLASS_INCLUDE_DEBUGGER_BINDINGS

/*
 *
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%% 64Bit Debugging Interface %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 *
 */

#if BLOCKPARTY_DEBUGGING_TARGET_64BIT == 1



#endif // #if BLOCKPARTY_DEBUGGING_TARGET_64BIT == 1


/*
 *
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%% 32Bit Debugging Interface %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 *
 */


#if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1

// Attaches to a pid, destroys the handle if not already destroyed
// and detaches if not already detached.
int Block::DbgStartPid(int pid)
{

	// detach if not detached
	if (dbgHandle != NULL)
	{

		if (dbgHandle->attached > 0)
		{
			DebugDetach(dbgHandle);
		}
	}

	// destroy if not destroyed
	if (dbgHandle != NULL)
	{
		DestroyDebugHandle(dbgHandle);
		dbgHandle = NULL;
	}

	// Now start pid
	dbgHandle = DebugAttachToPid(pid);
	if (dbgHandle == NULL)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// Starts a new process with argv/envp pointers
int Block::DbgStartCreateProc(char * path, char **args, char **environment)
{

	// detach if not detached
	if (dbgHandle != NULL)
	{
		if (dbgHandle->attached > 0)
		{
			DebugDetach(dbgHandle);
		}
	}

	// destroy if not destroyed
	if (dbgHandle != NULL)
	{
		DestroyDebugHandle(dbgHandle);
		dbgHandle = NULL;
	}

	// now create process
	dbgHandle = DebugCreateProcessAttach(path, args, environment);

	if (dbgHandle == NULL)
		return ERR_FAILURE;

	if (dbgHandle->debugPid == -1)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// Function responsible for continuing debugging:
// On each loop all comparisons in our debug handle are
// evaluated.  Returns true if a condition evaluated.
int Block::DbgContinue()
{

	if (dbgHandle == NULL)
		return ERR_DBG_HANDLE_NULL;

	switch (DebugContinue(dbgHandle))
	{

		case ERR_DBG_PROCESS_EXITED:
			return ERR_DBG_PROCESS_EXITED;

		case ERR_DBG_PROCESS_SIGNALED:
			return ERR_DBG_PROCESS_SIGNALED;

		case ERR_DBG_PROCESS_STOPPED:
			return ERR_DBG_PROCESS_STOPPED;

		default:
			break;

	}

	// return indicating failure
	return ERR_FAILURE;

}

// Steps the debugger loop one step, no conditions
// are evaluated during this step.
int Block::DbgStep()
{

	if (dbgHandle == NULL)
		return ERR_FAILURE;

	return DebugStep(dbgHandle);

}

// Function to detach from debugging session
// this also destroys the debug handle, resetting
// it to null.
int Block::DbgDetachDestroy()
{

	if (dbgHandle == NULL)
		return ERR_FAILURE;

	if (DebugDetach(dbgHandle) == ERR_FAILURE)
		return ERR_FAILURE;

	if (DestroyDebugHandle(dbgHandle) == ERR_FAILURE)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// only detaches
int Block::DbgDetach()
{

	if (dbgHandle == NULL)
		return ERR_FAILURE;

	if (DebugDetach(dbgHandle) == ERR_FAILURE)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// returns ERR_SUCCESS if the breakpoint is installed
// and ERR_FAILURE if it is not.
int Block::DbgBreakpointIsSet(int bpIdentifier)
{

	if (dbgHandle == NULL)
		return ERR_DBG_HANDLE_NULL;

	P_DEBUG_BREAKPOINT_LIST bpElement = NULL;

	if (BreakpointIsSet(dbgHandle, bpIdentifier) == ERR_SUCCESS)
	{

		bpElement = GetBreakpointByNumber(dbgHandle, bpIdentifier);

		if (bpElement != NULL)
		{
			// verify in memory that the breakpoint is set by comparing it with int3
			if (DebugCompareMemory(dbgHandle, bpElement->bpAddr, (void *) "\xCC", 1) == ERR_SUCCESS)
			{

				// return indicating success
				return ERR_SUCCESS;
			}

		}
		else
		{

			// return indicating failure
			return ERR_FAILURE;

		}

	}

	// return indicating failure
	return ERR_FAILURE;

}

// returns the address of a breakpoint given a bpIdentifier (bpNumber)
void * Block::DbgBreakpointGetAddr(int bpIdentifier)
{

	// ensure we have a debug handle
	if(!dbgHandle)
		return NULL;

	// declare bp element
	P_DEBUG_BREAKPOINT_LIST bpElement = NULL;

	// check if bp is set
	if (BreakpointIsSet(dbgHandle, bpIdentifier) == ERR_SUCCESS)
	{

		bpElement = GetBreakpointByNumber(dbgHandle, bpIdentifier);
		if (bpElement != NULL)
			return bpElement->bpAddr;

	}

	return NULL;
}


// only destroys
int Block::DbgDestroy()
{

	// ensure we have a debug handle
	if(!dbgHandle)
		return ERR_FAILURE;

	// destroy the debug handle
	if (DestroyDebugHandle(dbgHandle) == ERR_FAILURE)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// --------------------------
// Functions to set conditions
// Note: Calling [the same] Set condition twice in a row
// creates a new condition in the list, eg. it does not
// overwrite an existing condition.

// Note: isGt  and isLt representa match condition
// if the register value is greater then or less then
// the specified eax.  Be careful not to double set
// gt/lt combinations.

// eax
int Block::SetEaxCondition(int eax, bool isGt, bool isLt)
{

	if (dbgHandle == NULL)
		return ERR_FAILURE;

	// first see if the conditions stucture is set
	// create a new one if not, and return
	if (dbgHandle->conditions == NULL)
	{

		AddConditionElement(dbgHandle, 0, 0, // signal
				1, eax, isGt, isLt, // eax, set condition
				0, 0, 0, 0, // ebx
				0, 0, 0, 0, // ecx
				0, 0, 0, 0, // edx
				0, 0, 0, 0, // esp
				0, 0, 0, 0, // ebp
				0, 0, 0, 0, // eip
				0, 0, 0, 0, // esi
				0, NULL, NULL, 0);

		return ERR_SUCCESS;

	}

	// if a condition does exist, be sure that it hasn't
	// been set yet, if it has, create and append a new
	// condition to the list.
	if (dbgHandle->conditions != NULL)
	{

		// get the last element of the list first
		for
		(
				dbgHandle->conditions = dbgHandle->conditions->first;
				dbgHandle->conditions->next != NULL;
				dbgHandle->conditions = dbgHandle->conditions->next
		) {}

		// conditions should now be set to the last element.
		// Evaluate the last conditions setability, if it is
		// already set, then create and append a new list.

		if (dbgHandle->conditions->eaxSet > 0)
		{

			AddConditionElement
			(
				dbgHandle, 0, 0, // signal
				1, eax, isGt, isLt, // eax, set condition
				0, 0, 0, 0, // ebx
				0, 0, 0, 0, // ecx
				0, 0, 0, 0, // edx
				0, 0, 0, 0, // esp
				0, 0, 0, 0, // ebp
				0, 0, 0, 0, // eip
				0, 0, 0, 0, // esi
				0, NULL, NULL, 0
			);

			// return indicating success
			return ERR_SUCCESS;

		}
		else
		{

			// if the condition is settable, set it in the
			// existing condition list element (saves space)
			dbgHandle->conditions->eaxSet = 1;
			dbgHandle->conditions->eax    = eax;
			dbgHandle->conditions->eaxGt  = isGt;
			dbgHandle->conditions->eaxLt  = isLt;

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// ebx
int Block::SetEbxCondition(int ebx, bool isGt, bool isLt)
{

	// ensure we have a debug handle
	if (dbgHandle == NULL)
		return ERR_FAILURE;

	// first see if the conditions stucture is set
	// create a new one if not, and return
	if (dbgHandle->conditions == NULL)
	{

		AddConditionElement
		(
				dbgHandle, 0, 0, // signal
				0, 0, 0, 0, // eax
				1, ebx, isGt, isLt, // ebx, set condition
				0, 0, 0, 0, // ecx
				0, 0, 0, 0, // edx
				0, 0, 0, 0, // esp
				0, 0, 0, 0, // ebp
				0, 0, 0, 0, // eip
				0, 0, 0, 0, // esi
				0, NULL, NULL, 0
		);

		// return indicating success
		return ERR_SUCCESS;

	}

	// if a condition does exist, be sure that it hasn't
	// been set yet, if it has, create and append a new
	// condition to the list.
	if (dbgHandle->conditions != NULL)
	{

		// get the last element of the list first
		for
		(
				dbgHandle->conditions = dbgHandle->conditions->first;
				dbgHandle->conditions->next != NULL;
				dbgHandle->conditions = dbgHandle->conditions->next
		) {}

		// conditions should now be set to the last element.
		// Evaluate the last conditions setability, if it is
		// already set, then create and append a new list.

		if (dbgHandle->conditions->ebxSet > 0)
		{

			AddConditionElement
			(
					dbgHandle, 0, 0, // signal
					0, 0, 0, 0, // eax
					1, ebx, isGt, isLt, // ebx, set condition
					0, 0, 0, 0, // ecx
					0, 0, 0, 0, // edx
					0, 0, 0, 0, // esp
					0, 0, 0, 0, // ebp
					0, 0, 0, 0, // eip
					0, 0, 0, 0, // esi
					0, NULL, NULL, 0
			);

			// return indicating success
			return ERR_SUCCESS;

		}
		else
		{

			// if the condition is settable, set it in the
			// existing condition list element (saves space)
			dbgHandle->conditions->ebxSet = 1;
			dbgHandle->conditions->ebx = ebx;
			dbgHandle->conditions->ebxGt = isGt;
			dbgHandle->conditions->ebxLt = isLt;

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// ecx
int Block::SetEcxCondition(int ecx, bool isGt, bool isLt)
{

	// ensure we have a debug handle
	if(!dbgHandle)
		return ERR_FAILURE;

	// first see if the conditions stucture is set
	// create a new one if not, and return
	if (dbgHandle->conditions == NULL)
	{

		AddConditionElement
		(
				dbgHandle, 0, 0, // signal
				0, 0, 0, 0, // eax
				0, 0, 0, 0, // ebx
				1, ecx, isGt, isLt, // ecx, set condition
				0, 0, 0, 0, // edx
				0, 0, 0, 0, // esp
				0, 0, 0, 0, // ebp
				0, 0, 0, 0, // eip
				0, 0, 0, 0, // esi
				0, NULL, NULL, 0);

		// return indicating success
		return ERR_SUCCESS;

	}

	// if a condition does exist, be sure that it hasn't
	// been set yet, if it has, create and append a new
	// condition to the list.
	if (dbgHandle->conditions != NULL)
	{

		// get the last element of the list first
		for
		(
				dbgHandle->conditions = dbgHandle->conditions->first;
				dbgHandle->conditions->next != NULL;
				dbgHandle->conditions = dbgHandle->conditions->next
		) {}

		// conditions should now be set to the last element.
		// Evaluate the last conditions setability, if it is
		// already set, then create and append a new list.

		if (dbgHandle->conditions->ecxSet > 0) {
			AddConditionElement(dbgHandle, 0, 0, // signal
					0, 0, 0, 0, // eax
					0, 0, 0, 0, // ebx
					1, ecx, isGt, isLt, // ecx, set condition
					0, 0, 0, 0, // edx
					0, 0, 0, 0, // esp
					0, 0, 0, 0, // ebp
					0, 0, 0, 0, // eip
					0, 0, 0, 0, // esi
					0, NULL, NULL, 0);

			// return indicating success
			return ERR_SUCCESS;

		}
		else
		{

			// if the condition is settable, set it in the
			// existing condition list element (saves space)
			dbgHandle->conditions->ecxSet = 1;
			dbgHandle->conditions->ecx = ecx;
			dbgHandle->conditions->ecxGt = isGt;
			dbgHandle->conditions->ecxLt = isLt;

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// edx
int Block::SetEdxCondition(int edx, bool isGt, bool isLt)
{

	// ensure we have a debug handle
	if(!dbgHandle)
		return ERR_FAILURE;

	// first see if the conditions stucture is set
	// create a new one if not, and return
	if (dbgHandle->conditions == NULL)
	{
		AddConditionElement
		(
			dbgHandle, 0, 0, // signal
			0, 0, 0, 0, // eax
			0, 0, 0, 0, // ebx
			0, 0, 0, 0, // ecx
			1, edx, isGt, isLt, // edx, set condition
			0, 0, 0, 0, // esp
			0, 0, 0, 0, // ebp
			0, 0, 0, 0, // eip
			0, 0, 0, 0, // esi
			0, NULL, NULL, 0
		);

		// return indicating success
		return ERR_SUCCESS;

	}

	// if a condition does exist, be sure that it hasn't
	// been set yet, if it has, create and append a new
	// condition to the list.
	if (dbgHandle->conditions != NULL)
	{

		// get the last element of the list first
		for
		(
				dbgHandle->conditions = dbgHandle->conditions->first;
				dbgHandle->conditions->next != NULL;
				dbgHandle->conditions = dbgHandle->conditions->next
		){}

		// conditions should now be set to the last element.
		// Evaluate the last conditions setability, if it is
		// already set, then create and append a new list.

		if (dbgHandle->conditions->edxSet > 0)
		{

			AddConditionElement
			(
				dbgHandle, 0, 0, // signal
				0, 0, 0, 0, // eax
				0, 0, 0, 0, // ebx
				0, 0, 0, 0, // ecx
				1, edx, isGt, isLt, // edx, set condition
				0, 0, 0, 0, // esp
				0, 0, 0, 0, // ebp
				0, 0, 0, 0, // eip
				0, 0, 0, 0, // esi
				0, NULL, NULL, 0
			);

			// return indicating success
			return ERR_SUCCESS;

		}
		else
		{

			// if the condition is settable, set it in the
			// existing condition list element (saves space)
			dbgHandle->conditions->edxSet = 1;
			dbgHandle->conditions->edx = edx;
			dbgHandle->conditions->edxGt = isGt;
			dbgHandle->conditions->edxLt = isLt;
		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// ebp
int Block::SetEbpCondition(int ebp, bool isGt, bool isLt)
{

	if(!dbgHandle)
		return ERR_FAILURE;

	// first see if the conditions stucture is set
	// create a new one if not, and return
	if (dbgHandle->conditions == NULL)
	{

		AddConditionElement
		(
			dbgHandle, 0, 0, // signal
			0, 0, 0, 0, // eax
			0, 0, 0, 0, // ebx
			0, 0, 0, 0, // ecx
			0, 0, 0, 0, // edx
			0, 0, 0, 0, // esp
			1, ebp, isGt, isLt, // ebp, set condition
			0, 0, 0, 0, // eip
			0, 0, 0, 0, // esi
			0, NULL, NULL, 0
		);

		// return indicating success
		return ERR_SUCCESS;
	}

	// if a condition does exist, be sure that it hasn't
	// been set yet, if it has, create and append a new
	// condition to the list.
	if (dbgHandle->conditions != NULL)
	{

		// get the last element of the list first
		for
		(
				dbgHandle->conditions = dbgHandle->conditions->first;
				dbgHandle->conditions->next != NULL;
				dbgHandle->conditions = dbgHandle->conditions->next
		) {}

		// conditions should now be set to the last element.
		// Evaluate the last conditions setability, if it is
		// already set, then create and append a new list.

		if (dbgHandle->conditions->ebpSet > 0)
		{

			AddConditionElement
			(
				dbgHandle, 0, 0, // signal
				0, 0, 0, 0, // eax
				0, 0, 0, 0, // ebx
				0, 0, 0, 0, // ecx
				0, 0, 0, 0, // edx
				0, 0, 0, 0, // esp
				1, ebp, isGt, isLt, // ebp, set condition
				0, 0, 0, 0, // eip
				0, 0, 0, 0, // esi
				0, NULL, NULL, 0
			);

			// return indicating success
			return ERR_SUCCESS;

		}
		else
		{

			// if the condition is settable, set it in the
			// existing condition list element (saves space)
			dbgHandle->conditions->ebpSet = 1;
			dbgHandle->conditions->ebp = ebp;
			dbgHandle->conditions->ebpGt = isGt;
			dbgHandle->conditions->ebpLt = isLt;

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// esp
int Block::SetEspCondition(int esp, bool isGt, bool isLt)
{

	// ensure we have a debug handle
	if(!dbgHandle)
		return ERR_FAILURE;

	// first see if the conditions stucture is set
	// create a new one if not, and return
	if (dbgHandle->conditions == NULL)
	{

		AddConditionElement
		(
			dbgHandle, 0, 0, // signal
			0, 0, 0, 0, // eax
			0, 0, 0, 0, // ebx
			0, 0, 0, 0, // ecx
			0, 0, 0, 0, // edx
			1, esp, isGt, isLt, // esp, set condition
			0, 0, 0, 0, // ebp
			0, 0, 0, 0, // eip
			0, 0, 0, 0, // esi
			0, NULL, NULL, 0
		);

		// return indicating success
		return ERR_SUCCESS;

	}

	// if a condition does exist, be sure that it hasn't
	// been set yet, if it has, create and append a new
	// condition to the list.
	if (dbgHandle->conditions != NULL)
	{

		// get the last element of the list first
		for
		(
			dbgHandle->conditions = dbgHandle->conditions->first;
			dbgHandle->conditions->next != NULL;
			dbgHandle->conditions = dbgHandle->conditions->next
		) {}

		// conditions should now be set to the last element.
		// Evaluate the last conditions setability, if it is
		// already set, then create and append a new list.

		if (dbgHandle->conditions->espSet > 0)
		{

			AddConditionElement
			(
				dbgHandle, 0, 0, // signal
				0, 0, 0, 0, // eax
				0, 0, 0, 0, // ebx
				0, 0, 0, 0, // ecx
				0, 0, 0, 0, // edx
				1, esp, isGt, isLt, // esp, set condition
				0, 0, 0, 0, // ebp
				0, 0, 0, 0, // eip
				0, 0, 0, 0, // esi
				0, NULL, NULL, 0
			);

			// return indicating success
			return ERR_SUCCESS;

		}
		else
		{

			// if the condition is settable, set it in the
			// existing condition list element (saves space)
			dbgHandle->conditions->espSet = 1;
			dbgHandle->conditions->ecx = esp;
			dbgHandle->conditions->ecxGt = isGt;
			dbgHandle->conditions->ecxLt = isLt;

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// esi
int Block::SetEsiCondition(int esi, bool isGt, bool isLt)
{

	// ensure we have a debug handle
	if (dbgHandle)
		return ERR_FAILURE;

	// first see if the conditions stucture is set
	// create a new one if not, and return
	if (dbgHandle->conditions == NULL)
	{
		AddConditionElement
		(
				dbgHandle, 0, 0, // signal
				0, 0, 0, 0, // eax
				0, 0, 0, 0, // ebx
				0, 0, 0, 0, // ecx
				0, 0, 0, 0, // edx
				0, 0, 0, 0, // esp
				0, 0, 0, 0, // ebp
				0, 0, 0, 0, // eip
				1, esi, isGt, isLt, // esi, set condition
				0, NULL, NULL, 0
		);

		// return indicating success
		return ERR_SUCCESS;

	}

	// if a condition does exist, be sure that it hasn't
	// been set yet, if it has, create and append a new
	// condition to the list.
	if (dbgHandle->conditions != NULL)
	{

		// get the last element of the list first
		for
		(
			dbgHandle->conditions = dbgHandle->conditions->first;
			dbgHandle->conditions->next != NULL;
			dbgHandle->conditions = dbgHandle->conditions->next
		) {}

		// conditions should now be set to the last element.
		// Evaluate the last conditions setability, if it is
		// already set, then create and append a new list.

		if (dbgHandle->conditions->espSet > 0)
		{

			AddConditionElement
			(
				dbgHandle, 0, 0, // signal
				0, 0, 0, 0, // eax
				0, 0, 0, 0, // ebx
				0, 0, 0, 0, // ecx
				0, 0, 0, 0, // edx
				0, 0, 0, 0, // esp
				0, 0, 0, 0, // ebp
				0, 0, 0, 0, // eip
				1, esi, isGt, isLt, // esi, set condition
				0, NULL, NULL, 0
			);

			// return indicating success
			return ERR_SUCCESS;

		}
		else
		{

			// if the condition is settable, set it in the
			// existing condition list element (saves space)
			dbgHandle->conditions->esiSet = 1;
			dbgHandle->conditions->esi = esi;
			dbgHandle->conditions->esiGt = isGt;
			dbgHandle->conditions->esiLt = isLt;

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// eip
int Block::SetEipCondition(int eip, bool isGt, bool isLt)
{

	// ensure we have a db handle
	if (!dbgHandle)
		return ERR_FAILURE;

	// first see if the conditions stucture is set
	// create a new one if not, and return
	if (dbgHandle->conditions == NULL)
	{

		AddConditionElement
		(
			dbgHandle, 0, 0, // signal
			0, 0, 0, 0, // eax
			0, 0, 0, 0, // ebx
			0, 0, 0, 0, // ecx
			0, 0, 0, 0, // edx
			0, 0, 0, 0, // esp
			0, 0, 0, 0, // ebp
			1, eip, isGt, isLt, // eip, set condition
			0, 0, 0, 0, // esi
			0, NULL, NULL, 0
		);

		// return indicating success
		return ERR_SUCCESS;

	}

	// if a condition does exist, be sure that it hasn't
	// been set yet, if it has, create and append a new
	// condition to the list.
	if (dbgHandle->conditions != NULL)
	{

		// get the last element of the list first
		for
		(
				dbgHandle->conditions = dbgHandle->conditions->first;
				dbgHandle->conditions->next != NULL;
				dbgHandle->conditions = dbgHandle->conditions->next
		) {}

		// conditions should now be set to the last element.
		// Evaluate the last conditions setability, if it is
		// already set, then create and append a new list.

		if (dbgHandle->conditions->eipSet > 0)
		{
			AddConditionElement
			(
					dbgHandle, 0, 0, // signal
					0, 0, 0, 0, // eax
					0, 0, 0, 0, // ebx
					0, 0, 0, 0, // ecx
					0, 0, 0, 0, // edx
					0, 0, 0, 0, // esp
					0, 0, 0, 0, // ebp
					1, eip, isGt, isLt, // eip, set condition
					0, 0, 0, 0, // esi
					0, NULL, NULL, 0
			);

			// return indicating success
			return ERR_SUCCESS;

		}
		else
		{

			// if the condition is settable, set it in the
			// existing condition list element (saves space)
			dbgHandle->conditions->eipSet = 1;
			dbgHandle->conditions->eip = eip;
			dbgHandle->conditions->eipGt = isGt;
			dbgHandle->conditions->eipLt = isLt;

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// memory comparison
// on loop compares whats in cmpBuff to the process memory
// at procAddr, for cmpLen bytes.
int Block::SetMemCondition(void *procAddr, void *cmpBuff, int cmpLen)
{

	// check debug handle
	if (dbgHandle == NULL)
		return ERR_FAILURE;

	// first see if the conditions stucture is set
	// create a new one if not, and return
	if (dbgHandle->conditions == NULL)
	{

		AddConditionElement
		(
				dbgHandle, 0, 0, // signal
				0, 0, 0, 0, // eax
				0, 0, 0, 0, // ebx
				0, 0, 0, 0, // ecx
				0, 0, 0, 0, // edx
				0, 0, 0, 0, // esp, set condition
				0, 0, 0, 0, // ebp
				0, 0, 0, 0, // eip
				0, 0, 0, 0, // esi
				1, procAddr, (unsigned char *) cmpBuff, cmpLen
		);

		// return indicating success
		return ERR_SUCCESS;

	}

	// if a condition does exist, be sure that it hasn't
	// been set yet, if it has, create and append a new
	// condition to the list.
	if (dbgHandle->conditions != NULL)
	{

		// get the last element of the list first
		for
		(
				dbgHandle->conditions = dbgHandle->conditions->first;
				dbgHandle->conditions->next != NULL;
				dbgHandle->conditions = dbgHandle->conditions->next
		) {}

		// conditions should now be set to the last element.
		// Evaluate the last conditions setability, if it is
		// already set, then create and append a new list.

		if (dbgHandle->conditions->espSet > 0)
		{

			AddConditionElement
			(
					dbgHandle, 0, 0, // signal
					0, 0, 0, 0, // eax
					0, 0, 0, 0, // ebx
					0, 0, 0, 0, // ecx
					0, 0, 0, 0, // edx
					0, 0, 0, 0, // esp, set condition
					0, 0, 0, 0, // ebp
					0, 0, 0, 0, // eip
					0, 0, 0, 0, // esi
					1, procAddr, (unsigned char *) cmpBuff, cmpLen
			);

			// return indicating success
			return ERR_SUCCESS;

		}
		else
		{

			// if the condition is settable, set it in the
			// existing condition list element (saves space)
			dbgHandle->conditions->cmpSet = 1;
			dbgHandle->conditions->cmpBuff = (unsigned char *) cmpBuff;
			dbgHandle->conditions->cmpAddr = procAddr;
			dbgHandle->conditions->cmpLen = cmpLen;
		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// destroys all conditions in the debugHandle
int Block::DestroyConditions()
{

	// ensure we have a debug handle
	if(!dbgHandle)
		return ERR_FAILURE;

	// init ret value
	int retCondition = 0;

	// if we have conditions, attempt to destroy them
	if (dbgHandle->conditions)
	{

		retCondition = DestroyConditionList(dbgHandle->conditions);
		dbgHandle->conditions = NULL;

	}

	// return value of destroy operation
	return retCondition;

}

// Debug Utility Functions
// gets the registers from the process, storing them
// locally in the dbgHandle
int Block::DbgGetRegs()
{

	// ensure we have a debug handle
	if(!dbgHandle)
		return ERR_FAILURE;

	// get the registers
	return DebugGetRegisters(dbgHandle);

}

// Prints the local instruction list
int Block::DbgPrintInstructions()
{

	// display instructions
	X86_32_PrintInstructionsINTEL(instructions);

	// return indicating success
	return ERR_SUCCESS;

}

// Returns a character buffer equivilent to the instructions
// printed above.
char * Block::DbgGenerateInstructionBuffer()
{
	return X86_32_GenerateInstructionsBufferINTEL(instructions);
}

// same as above, but uses a specified buffer instead
char * Block::DbgGenerateInstructionBuffer(P_X86_32_DECODED_INSTRUCTION_LIST instList)
{
	return X86_32_GenerateInstructionsBufferINTEL(instList);
}

// Retrieves the number of instructions in the debugged instruction
// list.
size_t Block::DbgGetInstructionCount()
{
	return X86_32_GetInstructionCount(instructions->first);
}

// Prints out an arbitrary instruction list
int Block::DbgPrintInstructions(P_X86_32_DECODED_INSTRUCTION_LIST instList)
{

	if (!instList)
		return ERR_FAILURE;

	return X86_32_PrintInstructionsINTEL(instList);
}

// Refreshes or creates a new x86 instruction table within the
// class.  This is necessary for disassembling any instructions
// using the class disassembler.  This does not need to be called
// if debugging a process and having called DbgRefresh.  However
// DbgRefresh does not generate a table if not debugging.  Rule
// of thumb: not-debugging, use this function.
int Block::DbgRefreshInstructionTable()
{

	if (!instructionTable)
		instructionTable = X86_32_GetX86InstructionTable();

	// return indicating success
	return ERR_SUCCESS;

}

// Decodes all instructions in buff buffer till length.
// Local Version: Allocated to local class pointer
int Block::DbgDecodeInstructions(void *buff, size_t length, size_t index)
{

	// bpfree if already allocated
	if (instructions != NULL) {
		X86_32_DestroyInstructionList(instructions);
		instructions = NULL;
	}

	// decode instructions at position specified
	instructions = X86_32_DecodeInstructions(instructionTable, (unsigned char *) buff, length, (void *) index);

	// return indicating success
	return ERR_SUCCESS;

}

// Non-Local Version: Allocated to pointer, This function
// is not responsible for bpfreeing the instructions list if previously
// set.
P_X86_32_DECODED_INSTRUCTION_LIST Block::DbgDecodeInstructionsNL(void *buff, size_t length, size_t index)
{

	P_X86_32_DECODED_INSTRUCTION_LIST instList;
	instList = X86_32_DecodeInstructions(instructionTable, (unsigned char *) buff, length, (void *) index);

	return instList;
}

// Utilizes the existing memory map and attempts to decode
// instructions from a given address.

// Local Version: Allocated to local class pointer
int Block::DbgDecodeInstructionsMem(void *addr, size_t length)
{

	// read arbitrary data from memory
	unsigned char * memRead = DbgReadMemory(addr, length);
	if (memRead == NULL)
		return ERR_FAILURE;

	// Decode the instructions and store in local struct
	if (DbgDecodeInstructions((void *) memRead, length, (size_t ) addr) == ERR_SUCCESS)
	{

		if (memRead != NULL)
			tq_free(this->tq, memRead);

		// return indicating success
		return ERR_SUCCESS;

	}

	// return indicating failure
	return ERR_FAILURE;

}

// Non-Local Version: Allocated to pointer
P_X86_32_DECODED_INSTRUCTION_LIST Block::DbgDecodeInstructionsMemNL(void *addr, size_t length)
{

	// read arbitrary data from memory
	unsigned char * memRead = DbgReadMemory(addr, length);
	if (memRead == NULL)
		return ERR_FAILURE;

	// Decode the instructions and store in local struct
	P_X86_32_DECODED_INSTRUCTION_LIST instList = DbgDecodeInstructionsNL((void *) memRead, length, (size_t ) addr);
	if (instList)
	{

		if(memRead)
			tq_free(this->tq, memRead);

		// return the instruction list
		return instList;

	}

	// return null for failure
	return NULL;

}

// retrieves memory, returns a bpmalloced buffer of readLen
// buffer must be bpfree'd if retrieved
unsigned char * Block::DbgReadMemory(void *processAddr, int readLen)
{

	if(!dbgHandle)
		return NULL;
	if(dbgHandle->attached <= 0)
		return NULL;

	// attempt to read memory
	return DebugReadMemory(dbgHandle, processAddr, readLen);

}

// prints out printLen bytes of memory from the process starting
// at void *processAddr 0x.08 style
int Block::DbgPrintMem(void *processAddr, int printLen)
{

	if (processAddr == NULL)
		return ERR_FAILURE;

	if (printLen == 0)
		return ERR_FAILURE;

	printf("\n Trying to read from %p", processAddr);
	unsigned char * memRead = DbgReadMemory(processAddr, printLen);
	if (memRead == NULL)
		return ERR_FAILURE;

	printf("\n memRead returned :%p\n", memRead);

	int i = 0;
	int x = 0;
	size_t tempMem = 0;

	for (printLen = printLen / 4; printLen > 0; printLen--)
	{

		if (i == 6)
		{
			printf("\n");
			i = 0;
		}

		memcpy(&tempMem, &memRead[x], 4);
		printf("0x%.08x ", tempMem);

		x += 4;
		i++;
		tempMem = 0;

	}

	// return indicating success
	return ERR_SUCCESS;

}

// prints out the stack ranges of the current stack
int Block::DbgPrintStack()
{

	if (!dbgHandle)
		return ERR_FAILURE;

	if (DebugPrintStack(dbgHandle) != ERR_SUCCESS)
	{

		// return indicating failure
		return ERR_FAILURE;

	}

	// return indicating success
	return ERR_SUCCESS;

}

// prints out the heap for the current heap
int Block::DbgPrintHeap()
{

	// ensure we have a debug handle
	if(!dbgHandle)
		return ERR_FAILURE;

	if (DebugPrintHeap(dbgHandle) != ERR_SUCCESS)
	{

		// return indicating failure
		return ERR_FAILURE;

	}

	// return indicating success
	return ERR_SUCCESS;

}

// prints out registers for the current process
int Block::DbgPrintRegisters()
{

	if (dbgHandle == NULL)
		return ERR_FAILURE;

	printf("\n Registers:__________________ ");
	printf("\n\t EIP: 0x%.08x", dbgHandle->eip);
	printf("\n\t EBP: 0x%.08x", dbgHandle->ebp);
	printf("\n\t ESP: 0x%.08x", dbgHandle->esp);
	printf("\n");
	printf("\n\t EAX: 0x%.08x", dbgHandle->eax);
	printf("\n\t EBX: 0x%.08x", dbgHandle->ebx);
	printf("\n\t ECX: 0x%.08x", dbgHandle->ecx);
	printf("\n\t EDX: 0x%.08x", dbgHandle->edx);
	printf("\n\t ESI: 0x%.08x", dbgHandle->esi);
	printf("\n -------------------------- ");

	// return indicating success
	return ERR_SUCCESS;

}

// compares memory, return ERR_SUCCESS on true, ERR_FAILURE
// on false.
int Block::DbgCmpMem(void *procAddr, void *cmpBuff, int cmpLen)
{

	// ensure we have a debug handle
	if (dbgHandle)
		return ERR_FAILURE;

	// attempt memory comparison
	return DebugCompareMemory(dbgHandle, procAddr, (unsigned char *) cmpBuff, cmpLen);

}

// sets a breakpoint in a running process
int Block::DbgAddBreakpoint(void * addr, int bpIdentifier)
{

	// ensure we have a debug handle
	if(!dbgHandle)
		return ERR_FAILURE;

	// add breakpoint
	if (CreateAddBreakpoint(dbgHandle, addr, bpIdentifier) == ERR_FAILURE)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// updates the various different debugger relevant
// data: (memory maps, registers, stack, heap etc)
int Block::DbgRefreshData()
{

	// ensure we have a debug handle
	if (!dbgHandle)
		return ERR_DBG_CANNOT_REFRESH_DATA;

	// bpfree existing memory structures
	if (dbgHandle->memory != NULL)
	{

		// bpfree memory list
		dbgHandle->memory = NULL;

	}

	if (dbgHandle->stack)
	{

		// bpfree stack list
		dbgHandle->stack = NULL;

	}

	if (dbgHandle->heap != NULL)
	{

		// bpfree heap list
		dbgHandle->heap = NULL;

	}

	// reset all registers to zero
	dbgHandle->eax = 0;
	dbgHandle->ebx = 0;
	dbgHandle->ecx = 0;
	dbgHandle->edx = 0;
	dbgHandle->esp = 0;
	dbgHandle->ebp = 0;
	dbgHandle->esi = 0;
	dbgHandle->eip = 0;
	dbgHandle->edi = 0;

	// reset segment registers
	dbgHandle->__cs = 0;
	dbgHandle->cs = 0;

	dbgHandle->__ds = 0;
	dbgHandle->ds = 0;

	dbgHandle->__es = 0;
	dbgHandle->es = 0;

	dbgHandle->__fs = 0;
	dbgHandle->fs = 0;

	dbgHandle->__gs = 0;
	dbgHandle->gs = 0;

	dbgHandle->__ss = 0;
	dbgHandle->gs = 0;

	// registers first
	if (DebugGetRegisters(dbgHandle) != ERR_SUCCESS)
	{
		printf("\n Failed to retrieve new registers.");
	}

	// then memory map
	if (GetMemMap(dbgHandle) != ERR_SUCCESS)
	{
		printf("\n Failed to retrieve memory mapping.");
	}
	else
	{

		if (DebugGetHeapFromMapping(dbgHandle) != ERR_SUCCESS)
		{

		}
		if (DebugGetStackFromMapping(dbgHandle) != ERR_SUCCESS)
		{

		}

	}

	if (instructionTable == NULL)
	{
		instructionTable = X86_32_GetX86InstructionTable();
	}

	if (syscallLookup == NULL)
	{
		syscallLookup = DebugGetLinuxSyscallLookupTable();
	}

	size_t elfReadSize = 0;
	unsigned char *elfInstReadBuff = NULL;

	// Then relevant elf headers from memory
	if (dbgHandle->memory != NULL)
	{

		elf = ELF32_ExtractHeaderListAll_Mem(dbgHandle);
		if (elf == NULL)
			printf("\n ERROR: Elf header not retrieved from dbgHandle. ");

		// walk elf list and decode instructions from pages
		if (elf != NULL)
		{
			if (elf->first != NULL)
			{

				for (elf = elf->first; elf != NULL; elf = elf->next)
				{

					if (elf->header != NULL)
					{

						elfReadSize = (size_t ) elf->header->memMap->highAddr - (size_t ) elf->header->memMap->lowAddr;

						// Decode instructions
						// elfInstReadBuff = DbgReadMemory((void *) elf->header->memMap->highAddr, elfReadSize);

						// printf("\nelf->header->memMap->lowAddr: %p, buff: %p, readlength: %i", elf->header->memMap->lowAddr, elfInstReadBuff, elfReadSize);
						// elf->header->decodedInstructions = (void *) X86_32_DecodeInstructions(instructionTable, (unsigned char *) elfInstReadBuff, elfReadSize, (void *) elf->header->memMap->lowAddr);

						// Conditional exit point, resets elf pointer to first.
						// on break.
						if (elf->next == NULL)
						{
							elf = elf->first;
							break;
						}

					}
				}
			}
		}
	}

	// return indicating success
	return ERR_SUCCESS;

}

// gets the memory map associated with a pointer in a running process
P_MEMORY_MAP Block::DbgPointerToMemMap(size_t checkPtr)
{

	if (!dbgHandle)
		return (P_MEMORY_MAP) NULL;

	if (!dbgHandle->memory)
		return (P_MEMORY_MAP) NULL;

	if (!dbgHandle->memory->first)
		return (P_MEMORY_MAP) NULL;

	if (checkPtr == 0)
		return (P_MEMORY_MAP) NULL;

	P_MEMORY_MAP memory = dbgHandle->memory;

	for
	(
		memory = memory->first;
		memory != NULL;
		memory = memory->next)
	{

		// default range check
		if ((size_t ) checkPtr <= (size_t ) memory->highAddr && checkPtr >= (size_t ) memory->lowAddr)
		{

			// return if we found a suitable result
			return (P_MEMORY_MAP) memory;

		}

	}

	// return null ptr on failure
	return (P_MEMORY_MAP) NULL;

}

// creates a valid security analysis given a valid debugging session
// which can pinpoint security holes in an application.
int Block::DbgExploitAnalysis(char * payload, int payloadLength)
{

	// ensure we have a debug handle
	if(!dbgHandle)
		return ERR_DBG_HANDLE_NULL;

	// refresh our memory areas
	if (DbgRefreshData() != ERR_SUCCESS)
		return ERR_DBG_CANNOT_REFRESH_DATA;

	// see if our exploit analysis structure is already filled in
	/*
	 if(exploitAnalysis != NULL){
	 exploitAnalysis->next = DebugCreateExploitAnalysis(dbgHandle, payload, payloadLength);
	 if(exploitAnalysis->next == NULL)
	 return ERR_DBG_CANNOT_CREATE_EXPLOIT_ANALYSIS;

	 exploitAnalysis->next->prev = exploitAnalysis;
	 exploitAnalysis->next->first = exploitAnalysis->first;
	 exploitAnalysis = exploitAnalysis->next;

	 } else {

	 exploitAnalysis = DebugCreateExploitAnalysis(dbgHandle, payload, payloadLength);
	 }

	 */

	// return indicating success
	return ERR_SUCCESS;

}

// Prints out the stack analysis
void Block::DbgPrintStackAnalysis()
{
	DebugPrintStackAnalysis(exploitAnalysis);
}

// Prints out the heap analysis
void Block::DbgPrintHeapAnalysis()
{
	DebugPrintHeapAnalysis(exploitAnalysis);
}

// Prints out the register analysis
void Block::DbgPrintRegisterAnalysis()
{
	DebugPrintHeapAnalysis(exploitAnalysis);
}

// prints out all analysis
void Block::DbgPrintAllAnalysis()
{

}

// sets the exploit file to use, reads and parses
// out any blockparty tags in the file.
int Block::DbgSetTriggerFile(char * triggerPath)
{

	triggerFile = NULL;

	// open the trigger file for read/write
	triggerFile = File_OpenFptr(triggerPath, (char *) "r");

	// return indicating success
	return ERR_SUCCESS;

}

// Writes a payload, into a PoC file at a position.
// The specifics of how the buffer are written are language dependent.
// The language being written with is determined via language tag
// in the <exploitbuff> structure
int Block::DbgWriteTriggerBuff(char *triggerPath, char * payload, int payloadLength, int triggerId)
{

	if (triggerPath == NULL)
		return ERR_TRIGGER_FILE_PASSED_AS_NULL;

	if (payload == NULL)
		return ERR_PAYLOAD_FILE_PASSED_AS_NULL;

	if (payloadLength == 0)
		return ERR_PAYLOAD_LENGTH_ZERO;

	if (triggerId == 0)
		return ERR_TRIGGER_ID_ZERO;

	DbgSetTriggerFile(triggerPath);
	if (triggerFile == NULL)
	{
		printf("\n Cannot set trigger file.");
		triggerFile = NULL;
		return ERR_DBG_TRIGGER_FILE_NOT_SET;
	}

	// file pointer used to write our payload.bin file
	FILE * exploitFile = NULL;

	// used to count buffer positions
	int i = 0;

	// max read size of 256k for source files, can be easily
	// increased by define in LinuxDebug.h
	char * triggerBuffer = (char *) tq_malloc(this->tq, MAX_TRIGGER_FILE_SIZE, BPLN);
	if (triggerBuffer == NULL)
	{
		printf("\n Cannot allocate memory.");
		return ERR_DBG_CANNOT_ALLOCATE_TRIGGER_FILE_MEMORY;
	}

	char * newFileBuffer = (char *) tq_malloc(this->tq, MAX_TRIGGER_FILE_SIZE, BPLN);
	if (newFileBuffer == NULL)
	{
		printf("\n Cannot allocate trigger file memory.");
		return ERR_DBG_CANNOT_ALLOCATE_TRIGGER_FILE_MEMORY;
	}

	memset(newFileBuffer, 0x00, MAX_TRIGGER_FILE_SIZE);
	memset(triggerBuffer, 0x00, MAX_TRIGGER_FILE_SIZE);
	File_Read(triggerFile, triggerBuffer, MAX_TRIGGER_FILE_SIZE - 1);

	// builder stack variable to create exploit id
	char exploitIdStr[4000];
	memset(exploitIdStr, 0x00, 4000);

	memset(exploitIdStr, 0x00, 4000);
	strncat(exploitIdStr, (char *) "<exploitbuff id=", 3999);

	snprintf(&exploitIdStr[bpstrlen(exploitIdStr)], 4, (char *) "%i", triggerId);
	char * filePos = strstr(triggerBuffer, exploitIdStr);
	if (filePos == NULL)
		return ERR_EXPLOIT_TAG_NOT_FOUND;

	char * endFileBuffPos = strstr(filePos, (char *) "</exploitbuff>");
	if (endFileBuffPos == NULL)
		return ERR_EXPLOIT_TAG_NOT_FOUND;

	// positions for our language variable
	char * languagePos = strstr(filePos, (char *) "language=");
	if (languagePos == NULL)
		return ERR_LANGUAGE_TAG_NOT_FOUND;

	// position for our name variable
	char * namePos = strstr(filePos, (char *) "name=");
	if (namePos == NULL)
		return ERR_NAME_TAG_NOT_FOUND;

	// now determine where we are in our buffer
	if
	(
		filePos        != NULL &&
		endFileBuffPos != NULL &&
		namePos        != NULL &&
		languagePos    != NULL
	)
	{

		// skip to new line or null, whichever comes first
		for
		(
				i = 0;
				filePos[i] != '\x00' &&
				filePos[i] != '\n'   &&
				filePos[i] != '\r';
				i++
		)
		{}

		filePos = &filePos[i];
		filePos[0] = 0x00;

		// finding true end of the buffer replacement space, aka at the
		// newline delimiter.
		while (endFileBuffPos[0] != '\n' && endFileBuffPos[0] != '\r')
		{

			endFileBuffPos = (char *) ((size_t ) endFileBuffPos - 1);

			if ((size_t ) endFileBuffPos < (size_t ) filePos)
			{
				return ERR_DBG_CANNOT_LOCATE_WRITE_DELIMITER;
			}

		}

		// create a buffer for our buffer name
		char * nameBuff = (char *) tq_malloc(this->tq, 255, BPLN);
		memset(nameBuff, 0x00, 255);
		memcpy(nameBuff, namePos, 255);

		// walk the buffer looking for spaces (additional parsing
		// required for special characters)
		for
		(
				i = 0;
				i <= 255 && nameBuff[i] != '\t' && nameBuff[i] != ' ' && nameBuff[i] != '>' && nameBuff[i] != '>';
				i++
		) {}


		// null terminate the buffer location in memory
		if (i < 255)
		{
			nameBuff[i] = 0x00;
			nameBuff = &nameBuff[bpstrlen("name=")];
		}

		// create a buffer for our buffer name
		char * languageBuff = (char *) tq_malloc(this->tq, 255, BPLN);
		memset(languageBuff, 0x00, 255);
		memcpy(languageBuff, languagePos, 255);

		// walk the buffer looking for spaces (additional parsing
		// required for special characters)
		for
		(
				i = 0;
				i <= 255 && languageBuff[i] != '\t' && languageBuff[i] != ' ' && languageBuff[i] != '>' && languageBuff[i] != '>';
				i++
		) {}


		// null terminate the buffer location in memory
		if (i < 255)
		{
			languageBuff[i] = 0x00;
			languageBuff = &languageBuff[bpstrlen("language=")];
		}

		char * idBuff = (char *) tq_malloc(this->tq, 100, BPLN);
		memset(idBuff, 0x00, 100);

		snprintf(idBuff, 99, (char *) "%i", triggerId);

		// now perform language comparisons
		if (memcmp(languageBuff, (char *) "C", 1) == 0)
		{

			// Generate C Style Accessor
			strncat(newFileBuffer, triggerBuffer, MAX_TRIGGER_FILE_SIZE - 1);
			strncat(newFileBuffer, (char *) "\n", 1);
			strncat(newFileBuffer, ExploitCreateBuffer_C(nameBuff, idBuff, payloadLength), MAX_TRIGGER_FILE_SIZE - 1000);
			strncat(newFileBuffer, endFileBuffPos, MAX_TRIGGER_FILE_SIZE - 1000);

		}
		else if(memcmp(languageBuff, (char *) "Perl", 4) == 0)
		{

			// Generate Perl Style Accessor
			strncat(newFileBuffer, triggerBuffer, MAX_TRIGGER_FILE_SIZE - 1);
			strncat(newFileBuffer, (char *) "\n", 1);
			strncat(newFileBuffer, ExploitCreateBuffer_Perl(nameBuff, idBuff, payloadLength), MAX_TRIGGER_FILE_SIZE - 1000);
			strncat(newFileBuffer, endFileBuffPos, MAX_TRIGGER_FILE_SIZE - 1000);

		}
		else if (memcmp(languageBuff, (char *) "Python", 6) == 0)
		{

			// Generate Python Style Accessor
			strncat(newFileBuffer, triggerBuffer, MAX_TRIGGER_FILE_SIZE - 1);
			strncat(newFileBuffer, (char *) "\n", 1);
			strncat(newFileBuffer, ExploitCreateBuffer_Python(nameBuff, idBuff, payloadLength), MAX_TRIGGER_FILE_SIZE - 1000);
			strncat(newFileBuffer, endFileBuffPos, MAX_TRIGGER_FILE_SIZE - 1000);

		}
		else if (memcmp(languageBuff, (char *) "Ruby", 4) == 0)
		{

			// Generate Ruby Style Accessor
			strncat(newFileBuffer, triggerBuffer, MAX_TRIGGER_FILE_SIZE - 1);
			strncat(newFileBuffer, (char *) "\n", 1);
			strncat(newFileBuffer, ExploitCreateBuffer_Ruby(nameBuff, idBuff, payloadLength), MAX_TRIGGER_FILE_SIZE - 1000);
			strncat(newFileBuffer, endFileBuffPos, MAX_TRIGGER_FILE_SIZE - 1000);

		} else
		{
			return ERR_LANGUAGE_NOT_SPECIFIED;
		}

		// set our payload path buffers
		char payloadPath[1000];
		memset(payloadPath, 0x00, 1000);
		strncat(payloadPath, (char *) "./payload", 9);
		strncat(payloadPath, idBuff, 255);
		strncat(payloadPath, (char *) ".bin", 4);

		// Write the payload to file
		exploitFile = File_OpenFptr(payloadPath, (char *) "w");
		if (exploitFile != NULL)
		{
			File_Write(exploitFile, payload, payloadLength);
			File_Close(exploitFile);
		}
		else
		{
			return ERR_CANNOT_WRITE_PAYLOAD_FILE;
		}

		exploitFile = NULL;

		// Rewrite our modified full buffer
		File_Close(triggerFile);
		triggerFile = File_OpenFptr(triggerPath, (char *) "w");

		if (triggerFile != NULL)
		{
			File_Write(triggerFile, newFileBuffer, bpstrlen(newFileBuffer));
			File_Close(triggerFile);
		}
		else
		{
			return ERR_CANNOT_WRITE_TRIGGER_FILE;
		}

	}
	else
	{
		return ERR_DBG_EXPLOIT_DELIMITER_MALFORMED;
	}

	// now we can compare variable data
	return ERR_SUCCESS;

}

// removes a breakpoint
int Block::DbgRemoveBreakpoint(int bpIdentifier)
{

	if (DestroyBreakpoint(dbgHandle, bpIdentifier) == ERR_FAILURE)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// removes all breakpoints
int Block::DbgRemoveAllBreakpoints()
{

	if (DestroyBreakpointList(dbgHandle) == ERR_FAILURE)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// prints out all breakpoints
int Block::DbgPrintBreakpoints()
{

	if (PrintBreakpointList(dbgHandle) == ERR_FAILURE)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// prints out all conditions and flags there in
// as well as flag status.  If there is no list,
// nothing prints.
void Block::PrintConditions()
{

	if (dbgHandle == NULL)
		return;

	if (dbgHandle->conditions != NULL)
		PrintConditionList(dbgHandle->conditions->first);

}

#endif // 32bit

#endif // BP_BLOCKCLASS_INCLUDE_DEBUGGER_BINDINGS


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%% MYSQL ROUTINES %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


#ifdef BP_BLOCKCLASS_INCLUDE_MYSQL_DB_BINDINGS

// Connect to a mysql database.
BP_ERROR_T Block::MysqlConnect(char *mysqlHost, char *mysqlUser, char *mysqlPass, char *mysqlDB, size_t port)
{

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// destroy existing session if one exists
	if(this->mysqlSession)
	{

		// destroy a statement if it exists
		this->mysqlDestroyStatement();

		// close the session
		BP_MySQLClose(this->mysqlSession);

		// set the class handle to null
		this->mysqlSession = NULL;

	}

	// try to connect here
	this->mysqlSession = BP_MySQLConnect(mysqlHost, mysqlUser, mysqlPass, mysqlDB, port);

	// ensure we have created a session, if not, exit
	if(!this->mysqlSession)
	{
		BP_MySQLAddErr(this->mysqlSession, BP_MYSQL_ERROR_CONNECT, 0, "Block class could not connect to database.", BPLN);
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// leave the critical section
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION;


	// return indicating success
	return ERR_SUCCESS;

}

// closes an established mysql connection (local class or a pointer)
BP_ERROR_T Block::MysqlClose(P_BP_MYSQL_SESSION mysqlSession)
{

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// var for return value
	BP_ERROR_T ret_val = ERR_FAILURE;

	// ensure we have a session to close
	if(mysqlSession)
	{

		// close the session
		ret_val = BP_MySQLClose(mysqlSession);

		// set handle to null
		this->mysqlSession = NULL;

		// return the close return value
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION;
		return ret_val;

	}

	// return the out value
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION;

	// attempt close (destorys session and autolocks the critical section)
	ret_val = BP_MySQLClose(this->mysqlSession);

	// set mysql session to null
	this->mysqlSession = NULL;

	// return the provided value
	return ret_val;

}

// attempts to reconnect where necessary
int Block::MySQLReconnect()
{
	if(!this->mysqlSession)
		return ERR_FAILURE;

	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	if(!this->mysqlSession->pass)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	if(!this->mysqlSession->user)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	if(!this->mysqlSession->host)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	if(!this->mysqlSession->db)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	if(!this->mysqlSession->port)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// display reconnect message
	printf("\n [!!] Attempting reconnect (sleeping small interval first (1-3 seconds)");

	// perform actual sleep
	sleep(BP_GenRandomUnsignedLong(1,3));

	// attempt to connect
	int ret = this->MysqlConnect(this->mysqlSession->host, this->mysqlSession->user, this->mysqlSession->pass, this->mysqlSession->db, 3306);

	// display reconn message
	if(ret != 0)
		printf("\n [+] Reconnect attempt failed - returned code: %i", ret);
	else
		printf("\n [+] Reconnected ok!");

	// return with the code provided
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ret);

}

// null terminated string querys only
int Block::MysqlQuery(char *query)
{

	// return distinct error if we have no session
	if(!this->mysqlSession)
		return BP_MYSQL_QUERY_ERROR_CODE_NO_SESSION;

	// enter critical section
	BP_MYSQL_ENTER_CRITICAL_SECTION;

	// attempt reconnect if necessary
	while(!this->mysqlSession->connected)
	{
		BP_MySQLAddErr(this->mysqlSession, BP_MYSQL_ERROR_CONNECT, 0, "Block class tried to run query, but the session was not connected.", BPLN);
		this->MySQLReconnect();
	}

	// attempt to run the query
	int ret_val = BP_MySQLQuery(mysqlSession, query);

	// leave critical section and return value of query
	if(!ret_val)
	{
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);
	}

	// exit failure
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

}

// Real query can contain binary data, specify query length appropriately
// or face the consequences.
int Block::MysqlRealQuery(char *query, size_t len)
{

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return BP_MYSQL_QUERY_ERROR_CODE_NO_SESSION;

	// attempt reconnect if necessary
	while(!this->mysqlSession->connected)
	{
		BP_MySQLAddErr(this->mysqlSession, BP_MYSQL_ERROR_CONNECT, 0, "Block is not connected to the database.", BPLN);
		this->MySQLReconnect();
	}

	// attempt to run the query
	int ret_val = BP_MySQLRealQuery(mysqlSession, query, len);

	// leave critical section and return value of query
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ret_val);

}

// retrieves results from the last query
BP_ERROR_T Block::MysqlGetResults()
{

	// ensure we have a session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// store results
	this->mysqlResults = BP_MySQLStoreResult(mysqlSession);

	if (!this->mysqlResults)
	{
		BP_MySQLAddErr
		(
			this->mysqlSession,
			BP_MYSQL_ERROR_CONNECT,
			0,
			"Block could not get results in block->MysqlGetResults.",
			BPLN
		);

		// return indicating failure
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// Returns a row structure from the results
MYSQL_ROW Block::MysqlGetRow()
{

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return NULL;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// attempt to fetch row
	MYSQL_ROW ret_val = BP_MySQLFetchRow(mysqlResults, this->mysqlSession);

	// leave critical section and return value
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION;
	return ret_val;

}

// returns the length array corresponding to the MYSQL_RESULT returned from MysqlGetResults
unsigned long * Block::MysqlFetchLengths()
{

	// ensure we have a session
	if(!this->mysqlSession)
		return NULL;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// attempt to fetch lengths
	unsigned long * ret_val = BP_MySQLFetchLengths(mysqlResults, this->mysqlSession);

	// leave the critical section
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION;
	return ret_val;

}

// Create New Database: Non-depreciated CREATE DATABASE style
int Block::MysqlCreateDb(char *dbName)
{

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// attempt to create db
	BP_ERROR_T ret_val = BP_MySQLCreateDB(mysqlSession, dbName);

	// leave critical section
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION;

	// return the create status val
	return ret_val;

}

// ------------- Database Semaphore Locking ---------

// Use the following to lock the mysql session before doing any
// threaded db work.
BP_ERROR_T Block::mysqlSemLock()
{

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// NOTE: Don't enter critical section.

	// return the lock value
	return BP_MySQLSemLock(this->mysqlSession);

}

// unlock to stop blocking this semaphore.  Call only after a successful lock
// or you can run into contention problems.
BP_ERROR_T Block::mysqlSemUnlock()
{
	// ensure we have a mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// NOTE: Don't enter critical section.

	// return the unlock value
	return BP_MySQLSemUnlock(this->mysqlSession);

}

// ------------------ Prepared Statements ---------------------------

// AES encrypt/decrypt for prepared statements.  Any ? in the string
// is replaced with AES_ENCRYPT(?, 'key_here').   Anything surrounded by
// {value_name} is replaced with AES_DECRYPT(value_name, 'key_here').  Key must be
// alphanumeric.
char * Block::mysqlAesPrepare(char *sql, char *key)
{

	// if a statement already exists, destroy and add warning
	if (!sql)
	{
		this->AddErrorToStack((char*) "Error in Block::mysqlAesPrepare(): No SQL argument passed.");
		return NULL;
	}

	// set key from class if its null
	if (!key)
		key = this->mysql_aes_key;

	// if key is still null, no key is found
	if (!key)
	{
		this->AddErrorToStack((char*) "Error in Block::mysqlAesPrepare(): No encryption key passed or set in class.");
		return NULL;
	}

	// make sure the key is a correct length
	if (bpstrlen(key) > 1024)
	{
		this->AddErrorToStack((char*) "Error in Block::mysqlAesPrepare(): Encryption key cannot be longer then 1024 bytes.");
		return NULL;
	}

	// string indexes
	size_t i = 0;
	size_t x = 0;

	// replacement index
	size_t replacement_len = 0;

	// set length as string length
	size_t len = bpstrlen(sql);

	// encrypt sql
	char * encrypt_sql = NULL;

	// create a stack buffer for holding key statement
	char key_buff[2048] = { 0 };
	char name_buff[256] = { 0 };

	// walk string and perform replacements using bprealloc
	for (; i < len; i++)
	{

		// zero out the member
		memset(&key_buff, 0x00, 2048);
		memset(&name_buff, 0x00, 128);

		// check for null byte
		if (sql[i] == 0x00) {
			// return the encrypted string if we've hit the end of the string
			return encrypt_sql;
		}

		// if its a question mark, encrypt it (database inbound data)
		if (sql[i] == '?')
		{
			printf("\n Replacing ?");
			// set key
			snprintf((char *) &key_buff, 2040, (char *) "AES_ENCRYPT(?, '%s')", key);

			// set string index
			x = bpstrlen(encrypt_sql);

			// bpreallocate encrypted sql string
			encrypt_sql = (char *) tq_realloc(this->tq, (void *) encrypt_sql, x + bpstrlen(key_buff) + 1, BPLN);

			// zero out end of the buffer
			memset(&encrypt_sql[x], 0x00, bpstrlen(key_buff) + 1);

			// copy in new data
			memcpy(&encrypt_sql[x], key_buff, bpstrlen(key_buff));

		} // if its an open bracket, decrypt it (database outbound to client data)
		else if (sql[i] == '{')
		{

			printf("\n Replacing {}");
			// calculate length till closing bracket (or null)
			for (x = 0; sql[i + x] != '}' && sql[i + x] != 0x00; x++) {
			}

			// length check
			if (x > 255) {
				this->AddErrorToStack((char*) "Error in Block::mysqlAesPrepare(): Value within {} > 255 bytes.");
				return NULL;
			}

			// copy memory into key buffer
			memcpy(&name_buff, &sql[i + 1], x - 1);

			// increment x
			i += x;

			// set decryption buffer
			snprintf((char *) &key_buff, 2040, (char *) "AES_DECRYPT(%s, '%s')", (char *) &name_buff, key);

			// set string index
			x = bpstrlen(encrypt_sql);

			// bpreallocate encrypted sql string
			encrypt_sql = (char *) tq_realloc(this->tq, (void *) encrypt_sql, x + bpstrlen(key_buff) + 1, BPLN);

			// zero out end of the buffer
			memset(&encrypt_sql[x], 0x00, bpstrlen(key_buff) + 1);

			// copy in new data
			memcpy(&encrypt_sql[x], key_buff, bpstrlen(key_buff));

		} // if neither encrypt or decrypt, just copy in the character to the end of the new string
		else
		{

			// set string index
			if (encrypt_sql)
				x = bpstrlen(encrypt_sql);
			else
				x = 0;

			// return new string
			// bpreallocate encrypted sql string
			encrypt_sql = (char *) tq_realloc(this->tq, (void *) encrypt_sql, x + 2, BPLN);

			// zero out end of the buffer
			memset(&encrypt_sql[x], 0x00, 2);

			// set ne character in the string
			encrypt_sql[x] = sql[i];

		}

	}

	// technically should'nt ever get here, should return in the null clause in the loop above
	return encrypt_sql;

}

// prepare mysql query ( ? parameters)
BP_ERROR_T Block::mysqlPrepareQuery(char * sql)
{

	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// require valid sql
	if(!sql)
	{
		this->AddErrorToStack((char *) "\n [!!] Attempting to prepare NULL sql.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// attempt reconnect if necessary
	while(!this->mysqlSession->connected)
	{
		printf("\n MySQL not connected in %s, %u", __FILE__, __LINE__);
		// __asm("int3");
		this->MySQLReconnect();
	}

	// printf("\nwatwat1");
	if(this->mysqlSession->server_gone_away)
	{
		printf("\n [!!] SERVER HAS GONE AWAY");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}
	// printf("\nwatwat2");
	if(this->mysqlSession->server_lost_connection)
	{
		printf("\n [!!] SERVER HAS LOST CONNECTION");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}


	// if a statement already exists, destroy and add warning
	if (this->mysql_stmt)
	{
		this->AddWarningToStack((char *) "Warning in Block::mysqlPrepareQuery(): Statement already exists, destroying before continuing.");
		this->mysqlDestroyStatement();
		this->mysql_stmt = NULL;
	}


	// prepare the query
	this->mysql_stmt = BP_MySQLPrepareQuery(this->mysqlSession, sql);

	// check if statement was prepared properly
	if (!this->mysql_stmt) {
		this->AddErrorToStack((char*) "Error in Block::mysqlPrepareQuery(): Failed to prepare query.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// set parameter count
	this->mysql_known_parameter_count = mysql_stmt_param_count(this->mysql_stmt);

	// exit critical section and return
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// adds a parameter to the parameters stack
BP_ERROR_T Block::mysqlAddParameter(enum_field_types col_type, char *buffer, size_t buffer_length)
{

	// ensure we have a session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enterr critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	if (this->mysql_parameter_count >= this->mysql_known_parameter_count)
	{
		this->AddErrorToStack((char*) "Error in Block::mysqlAddParameter(): The parameter stack for this statement is already full");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// allocate space for new parameter
	this->mysql_parameters = (MYSQL_BIND *) tq_realloc(this->tq, this->mysql_parameters, sizeof(MYSQL_BIND) * (this->mysql_parameter_count + 1), BPLN);

	// zero out element data
	memset(&this->mysql_parameters[this->mysql_parameter_count], 0x00, sizeof(MYSQL_BIND));

	// set arguments
	this->mysql_parameters[this->mysql_parameter_count].buffer_type = col_type;

	// only set parameter if it's not a null type
	if(MYSQL_TYPE_NULL != col_type )
	{
		this->mysql_parameters[this->mysql_parameter_count].buffer = buffer;
		this->mysql_parameters[this->mysql_parameter_count].buffer_length = buffer_length;
	}

	// increment parameter counter
	this->mysql_parameter_count++;

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// binds parameters to a statement
BP_ERROR_T Block::mysqlBindParameters()
{

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// attempt reconnect if necessary
	if(!this->mysqlSession->connected)
	{

		printf("\n MySQL not connected in %s, %u", __FILE__, __LINE__);
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}

	if ( this->mysql_known_parameter_count != this->mysql_parameter_count )
	{
		this->AddErrorToStack((char*) "Error in Block::mysqlBindParameters(): Wrong number of parameters set, cannot bind.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	if (BP_MySQLBindParameters(this->mysql_stmt, this->mysql_parameters, this->mysqlSession) == ERR_FAILURE)
	{
		this->AddErrorToStack((char*) "Error in Block::mysqlBindParameters(): Parameters failed to bind.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// adds a  result to the result stack (used for fetching data, one result per column)
BP_ERROR_T Block::mysqlAddResult(enum_field_types col_type, char *buffer, size_t buffer_length)
{

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// allocate space for new result
	this->mysql_results = (MYSQL_BIND *) tq_realloc(this->tq, this->mysql_results, sizeof(MYSQL_BIND) * (this->mysql_results_count + 1), BPLN);

	// zero out element data
	memset(&this->mysql_results[this->mysql_results_count], 0x00, sizeof(MYSQL_BIND));

	// set arguments
	this->mysql_results[this->mysql_results_count].buffer_type   = col_type;
	// this->mysql_results[this->mysql_results_count].buffer        = buffer;
	// this->mysql_results[this->mysql_results_count].buffer_length = buffer_length;
	// this->mysql_results[this->mysql_results_count].is_null       = (my_bool *)       tq_calloc(this->tq, sizeof(my_bool), 1, BPLN);
	// this->mysql_results[this->mysql_results_count].length        = (unsigned long *) tq_calloc(this->tq, sizeof(unsigned long ), 1, BPLN);
	// this->mysql_results[this->mysql_results_count].error         = (my_bool *)       tq_calloc(this->tq, sizeof(my_bool), 1, BPLN);

	// increment parameter counter
	this->mysql_results_count++;

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// binds result configuration to a statement
BP_ERROR_T Block::mysqlBindResults()
{

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;



	// attempt reconnect if necessary
	if(!this->mysqlSession->connected)
	{
		printf("\n MySQL not connected in %s, %u", __FILE__, __LINE__);
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	if (!this->mysql_stmt)
	{
		this->AddErrorToStack((char*) "Error in Block::mysqlBindResults(): Statement unset.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	if (BP_MySQLBindResult(this->mysql_stmt, this->mysql_results, this->mysql_results_count, this->mysqlSession) != ERR_SUCCESS)
	{
		this->AddErrorToStack((char*) "Error in Block::mysqlBindResults(): Bind failed.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// store results (optional, copies data locally before fetch is called)
BP_ERROR_T Block::mysqlStoreResults()
{

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// attempt reconnect if necessary
	if(!this->mysqlSession->connected)
	{
		printf("\n MySQL not connected in %s, %u", __FILE__, __LINE__);
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// ensure we have a prepared statement
	if (!this->mysql_stmt)
	{
		this->AddErrorToStack((char*) "Error in Block::mysqlStoreResults(): Statement unset.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// attempt to store results
	if (BP_MySQLStoreResults(this->mysql_stmt, this->mysqlSession) == ERR_FAILURE)
	{
		this->AddErrorToStack((char*) "Error in Block::mysqlStoreResults(): Failed to store local result.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);
}

// fetch result
BP_ERROR_T Block::mysqlFetchResult()
{

	// ensure we have a session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;


	// attempt reconnect if necessary
	if(!this->mysqlSession->connected)
	{
		printf("\n MySQL not connected in %s, %u", __FILE__, __LINE__);
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// ensure we have a mysql statement
	if(!this->mysql_stmt)
	{
		this->AddErrorToStack((char*) "Error in Block::mysqlFetchResult(): Empty statement.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// attempt to fetch results
	if(BP_MySQLFetchResult(this->mysql_stmt, this->mysqlSession) == ERR_FAILURE)
	{
		this->AddWarningToStack((char *) "Error in Block::mysqlFetchResult(): Fetch failed/no results.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// destroy statement (free)
BP_ERROR_T Block::mysqlDestroyStatement()
{

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// store any dangling results
	if(this->mysql_stmt)
	{

		//this->mysqlStoreResults();
	}

	// attempt reconnect if necessary
	if(!this->mysqlSession->connected)
	{
		printf("\n MySQL not connected in %s, %u", __FILE__, __LINE__);
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// destroy the last prepared if it's set
	if(this->mysqlSession->last_sql_prepared)
	{
		tq_free(this->mysqlSession->tq, this->mysqlSession->last_sql_prepared);
		this->mysqlSession->last_sql_prepared = NULL;
	}

	// destroy allocated parameters if they exist
	if (this->mysql_parameters)
	{
		tq_free(this->tq, this->mysql_parameters);
		this->mysql_parameters = NULL;
		this->mysql_parameter_count = 0;
	}

	if (!this->mysql_stmt)
	{
		this->AddErrorToStack((char*) "Error in Block::mysqlDestroyStatement(): Empty statement.");
	}


	// close any statement
	if (this->mysql_stmt  || this->mysqlSession->stmt )
	if(BP_MySQLCloseStatement(this->mysql_stmt, this->mysqlSession) != ERR_SUCCESS)
	{
		this->mysql_stmt = NULL;
		this->mysqlSession->stmt = NULL;

	}



	// reset statement to null
	this->mysql_stmt = NULL;

	// destroy allocated results if they exist
	if (this->mysql_results)
	{

		// bpfree individual results first
		size_t x = 0;
		for(; x < this->mysql_results_count; x++)
		{
			if(this->mysql_results[x].is_null)
			{
				tq_free(this->tq, this->mysql_results[x].is_null);
				this->mysql_results[x].is_null = NULL;
			}
			if(this->mysql_results[x].length)
			{
				tq_free(this->tq, this->mysql_results[x].length);
				this->mysql_results[x].length = NULL;
			}
			if(this->mysql_results[x].error)
			{
				tq_free(this->tq, this->mysql_results[x].error);
				this->mysql_results[x].error = NULL;
			}

		}

		// now bpfree entire structure
		tq_free(this->tq, this->mysql_results);

		// nullify
		this->mysql_results       = NULL;
		this->mysql_results_count = 0;

	}

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);
}

// executes a prepared statement
BP_ERROR_T Block::mysqlExecuteStatement()
{

	// ensure we have a session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// attempt reconnect if necessary
	if(!this->mysqlSession->connected)
	{

		// add error
		this->AddErrorToStack((char*) "Error in Block::mysqlExecuteStatement(): Not connected.");

		// if we're not connected, exit immediately
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}


	if (!this->mysql_stmt)
	{
		this->AddErrorToStack((char*) "Error in Block::mysqlExecuteStatement(): Empty statement.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	if(this->mysql_known_parameter_count != this->mysql_parameter_count)
	{
		printf("\n [!!] Error: mysql parameter count mismatch (expected: %u , actual: %u", this->mysql_known_parameter_count, this->mysql_parameter_count);
		printf("\n [!!] Execution Failed on Query: %s", this->mysqlSession->last_sql_prepared);
		printf("\n [!!] Displaying Parameter Stack: ");
		this->mysqlPrintParamArray();
		this->mysqlDestroyStatement();
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}


	size_t err_n = 0;
	while (BP_MySQLExecuteStatement(this->mysql_stmt, this->mysqlSession) == ERR_FAILURE)
	{

		/*
		printf("\n [!!] Error: mysql parameter failed to execute. ");
		printf("\n [!!] Execution Failed on Query: %s", this->mysqlSession->last_sql_prepared);
		printf("\n [!!] Displaying Parameter Stack: ");
		this->mysqlPrintParamArray();
		*/

		this->mysqlDestroyStatement();
		this->mysql_stmt = NULL;
		this->AddErrorToStack((char*) "Error in Block::mysqlExecuteStatement(): Execute statement failed.");

		// return indicating failure
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}

	// leave critical section and return
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// utility functions for displaying
BP_ERROR_T Block::mysqlPrintResultArray()
{

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// ensure we have results available
	if (!this->mysql_results)
	{
		this->AddErrorToStack((char*) "Error in Block::mysqlPrintResultArray(): Array is empty.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// attempt to print the bind array
	BP_MySQLPrintBindArray(this->mysql_results, this->mysql_results_count);

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// attempt to print the parameters
BP_ERROR_T Block::mysqlPrintParamArray()
{

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// ensure we have mysql parameters
	if (!this->mysql_parameters)
	{
		this->AddErrorToStack((char*) "Error in Block::mysqlPrintParamArray(): Array is empty.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// attempt to print the bind array parameters
	BP_MySQLPrintBindArray(this->mysql_parameters, this->mysql_parameter_count);

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

#endif // BP_BLOCKCLASS_INCLUDE_MYSQL_DB_BINDINGS
#ifdef BP_BLOCKCLASS_INCLUDE_HTML_BINDINGS

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%% HTML  ATTACK ROUTINES %%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// This function initializes the http subcomponents in the class
// and starts a new http session.
int Block::HttpInit()
{

	// create new http session
	httpSession = BP_HTTPCreateSession();

	// link in the http session here
	lastInjEncode = NULL;
	userAgent     = NULL;
	BP_HTTPGlobalInit();

	// return indicating success
	return ERR_SUCCESS;

}


// Threaded cURL requests cannot use signals.  It causes incorrect longjmp's and a host
// of other mess.  Use this to fix that.
BP_ERROR_T Block::HttpDisableCurlSignalsForThreadedApps() {

	if(!this->httpSession)
		return ERR_FAILURE;

	if(!this->httpSession->curl)
		return ERR_FAILURE;

	// enable the option
	curl_easy_setopt(this->httpSession->curl, CURLOPT_NOSIGNAL, 1);

	// return indicating success
	return ERR_SUCCESS;

}

// Sets the user agent
int Block::HttpSetUserAgent(char *agent)
{
	BP_HTTPUserAgent(httpSession, agent);
	return ERR_SUCCESS;
}

// Sets a random user agent
int Block::HttpSetRandomUserAgent()
{
	// set user agent strings (allocated, must be destroyed)
	char ** browserUserAgentStrings = BP_GenerateBrowserAgentOnlyArray();

	// pick user agent string
	size_t browser_index = BP_GenRandomUnsignedLong(0, BP_HTTPUserAgentCount(browserUserAgentStrings) - 1);

	// allocate user agent string from global browser strings array (found in library)
	char *ua = (char *) tq_strndup(this->tq, browserUserAgentStrings[browser_index], bpstrlen(browserUserAgentStrings[browser_index]), BPLN);

	// set user agent
	this->HttpSetUserAgent(ua);

	// destroy the pointer array now that we no longer need it
	BP_DestroyPointerArray((void **) browserUserAgentStrings, BP_HTTPUserAgentCount(browserUserAgentStrings));

	// bpfree the user agent string now
	tq_free(this->tq, ua);

	// also bpfree the entire agent string ptr container buffer
	bpfree(browserUserAgentStrings);

	// return indicating success
	return ERR_SUCCESS;

}

// Sets username and password for site requests
int Block::HttpSetUserPass(char *user, char *pass)
{
	BP_HTTPSetUserPass(httpSession, user, pass);
	return ERR_SUCCESS;
}

// Sets post data
int Block::HttpSetPostData(char *postStr)
{
	BP_HTTPSetPostVars(httpSession, postStr);
	return ERR_SUCCESS;
}

// Sets cookie
int Block::HttpSetCookie(char *cookie)
{
	BP_HTTPSetCookie(httpSession, cookie);
	return ERR_SUCCESS;
}

// Sets cookie jar
int Block::HttpSetCookieJar(char *cookie_file)
{
	return BP_HTTPSetCookieJar(httpSession, cookie_file);
}

// Makes a request against a provided URL, CAN BE HTTPS :)
int Block::HttpMakeRequest(char *url, bool log_to_mysql)
{

	// JIT: create a new http session if one doesn't exist
	// already.
	if(!this->httpSession)
		this->HttpInit();

	// set ret value
	size_t ret_val = ERR_FAILURE;

	// use local request data here
	if(!url)
	{
		ret_val = BP_HTTPRequest(this->httpSession, (char *) &this->web_result_href);
	}
	else
	{
		ret_val = BP_HTTPRequest(httpSession, url);
	}

	// log the request to
	if(log_to_mysql == true)
	{

		// enter critical section
		BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

		// ensure we have a connected session
		if(this->mysqlSession)
		if(this->mysqlSession->connected)
		{

			// attempt to log the request
			this->mysqlSemLock();
			this->HttpLogLastRequestToMySQL();
			this->mysqlSemUnlock();

		}

		// leave critical section
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION;

	}

	// return the value returned from BP_HTTPRequest
	return ret_val;

}

// Logs a the last Http request to mysql (request must be made with this->HttpMakeRequest(...);
int Block::HttpLogLastRequestToMySQL()
{

	// run initial value checks
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	if(!this->mysqlSession->connected)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	if(!this->httpSession)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	if(!this->httpSession->page)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	if(!this->httpSession->page->hdr_data)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	if(!this->httpSession->page->hdr_data_size)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	if(!this->tq)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// allocate sha1 stack
	char ** sha1_gen_stack = (char **) tq_calloc(this->tq, sizeof(char*) * 4, 1, BPLN);

	// set empty (non-null strings)
	sha1_gen_stack[0] = "";
	sha1_gen_stack[1] = "";
	sha1_gen_stack[2] = "";

	// set the sha1 generator stack
	if(this->httpSession->lastRequestedHref)
		sha1_gen_stack[0] = this->httpSession->lastRequestedHref;
	if(this->httpSession->httpPostData)
		sha1_gen_stack[1] = this->httpSession->httpPostData;
	if(this->httpSession->cookie)
		sha1_gen_stack[2] = this->httpSession->cookie;

	// generate sha1 from the stack
	char * calculated_hash = BP_GenSHA1FromStringArray(sha1_gen_stack);
	if(sha1_gen_stack)
		tq_free(this->tq, sha1_gen_stack);

	// define insert parameters as strings
	char *result_id         = NULL;
	char *http_status_code  = NULL;
	char *http_connect_code = NULL;
	char *http_get_string   = NULL;
	char *http_post_string  = NULL;
	char *http_cookie       = NULL;
	char *http_headers      = NULL;


	// run jit href check
	BP_StringUtils * query_builder = new BP_StringUtils(NULL);

	// build the string
	query_builder->BuildString((char *) "INSERT INTO `http_request_log` (`request_unique_sha1`, `result_id`, `http_status_code`, `http_connect_code`, `http_get_string`, `http_post_string`, `http_cookie`, `http_headers`) VALUES (");


	// set result as the last web_result_id
	result_id = (char *) BP_SizeTToMySQLBuffer(this->web_result_id);

	// http status code
	http_status_code  = (char *) BP_SizeTToMySQLBuffer(this->httpSession->http_status_code);

	// http connect code
	http_connect_code = (char *) BP_SizeTToMySQLBuffer(this->httpSession->http_connect_code);

	// http get string
	http_get_string   = (char *) BP_BufferToMySQLHexString(this->httpSession->lastRequestedHref, this->httpSession->lastRequestedHref ? bpstrlen(this->httpSession->lastRequestedHref) : 0);

	// http post string
	http_post_string  = (char *) BP_BufferToMySQLHexString(this->httpSession->httpPostData, this->httpSession->httpPostData ? bpstrlen(this->httpSession->httpPostData) : 0);

	// set http post string
	http_cookie  = (char *) BP_BufferToMySQLHexString(this->httpSession->cookie, this->httpSession->cookie ? bpstrlen(this->httpSession->cookie): 0);

	// convert http headers
	http_headers      = (char *) BP_BufferToMySQLHexString(this->httpSession->page->hdr_data, this->httpSession->page->hdr_data_size);



	// append result id buffer
	query_builder->BuildString((char *) "\"");
	query_builder->BuildString((char *) calculated_hash);
	query_builder->BuildString((char *) "\"");
	query_builder->BuildString((char *) " , ");
	query_builder->BuildString((char *) result_id);
	query_builder->BuildString((char *) " , ");
	query_builder->BuildString((char *) http_status_code);
	query_builder->BuildString((char *) " , ");
	query_builder->BuildString((char *) http_connect_code);
	query_builder->BuildString((char *) " , ");
	query_builder->BuildString((char *) http_get_string);
	query_builder->BuildString((char *) " , ");
	query_builder->BuildString((char *) http_post_string);
	query_builder->BuildString((char *) " , ");
	query_builder->BuildString((char *) http_cookie);
	query_builder->BuildString((char *) " , ");
	query_builder->BuildString((char *) http_headers);


	// close query set
	query_builder->BuildString((char *) " ) ");

	// prepare the query
	if(this->mysqlPrepareQuery(query_builder->build_str) == ERR_FAILURE)
	{

		if(calculated_hash)
			bpfree(calculated_hash);
		if(result_id)
			bpfree(result_id);
		if(http_status_code)
			bpfree(http_status_code);
		if(http_connect_code)
			bpfree(http_connect_code);
		if(http_get_string)
			bpfree(http_get_string);
		if(http_post_string)
			bpfree(http_post_string);
		if(http_cookie)
			bpfree(http_cookie);
		if(http_headers)
			bpfree(http_headers);

		// nullify
		calculated_hash = NULL;
		result_id = NULL;
		http_status_code = NULL;
		http_connect_code = NULL;
		http_get_string = NULL;
		http_post_string = NULL;
		http_cookie = NULL;
		http_headers = NULL;

		// destroy statement (free memory)
		this->mysqlDestroyStatement();

		// exit
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}

	// execute the query
	if(this->mysqlExecuteStatement() == ERR_FAILURE)
	{

		if(calculated_hash)
			bpfree(calculated_hash);
		if(result_id)
			bpfree(result_id);
		if(http_status_code)
			bpfree(http_status_code);
		if(http_connect_code)
			bpfree(http_connect_code);
		if(http_get_string)
			bpfree(http_get_string);
		if(http_post_string)
			bpfree(http_post_string);
		if(http_cookie)
			bpfree(http_cookie);
		if(http_headers)
			bpfree(http_headers);

		// nullify
		calculated_hash = NULL;
		result_id = NULL;
		http_status_code = NULL;
		http_connect_code = NULL;
		http_get_string = NULL;
		http_post_string = NULL;
		http_cookie = NULL;
		http_headers = NULL;

		// destroy statement (free memory)
		this->mysqlDestroyStatement();

		// destroy the query builder
		delete query_builder;
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}


	// destroy the query builder
	delete query_builder;

	// destroy statement (free memory)
	this->mysqlDestroyStatement();

	// destroy generated data before return
	if(calculated_hash)
		bpfree(calculated_hash);
	if(result_id)
		bpfree(result_id);
	if(http_status_code)
		bpfree(http_status_code);
	if(http_connect_code)
		bpfree(http_connect_code);
	if(http_get_string)
		bpfree(http_get_string);
	if(http_post_string)
		bpfree(http_post_string);
	if(http_cookie)
		bpfree(http_cookie);
	if(http_headers)
		bpfree(http_headers);

	calculated_hash = NULL;
	result_id = NULL;
	http_status_code = NULL;
	http_connect_code = NULL;
	http_get_string = NULL;
	http_post_string = NULL;
	http_cookie = NULL;
	http_headers = NULL;


	#if USE_MARIADB == 1

		// save insert id for next set of inserts
		size_t ret_id = mysql_insert_id(this->mysqlSession->conn);
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ret_id);

	#else

		// return indicating success
		size_t ret_id = this->mysqlSession->stmt->insert_id;
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ret_id);

	#endif

	// never gets here
	return ERR_SUCCESS;

}

// Destroy Sessions
int Block::HttpCloseDestroySession()
{

	// ensure that the session is set
	if(!this->httpSession)
		return ERR_FAILURE;

	// destroy the session
	size_t ret = BP_HTTPDestroySession(this->httpSession);

	// set pointer
	this->httpSession = NULL;

	// return the code
	return ret;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%% CURL VALUE GETTRS %%%%%%%%%%%%%%%%%%%%%%%%%

// Gets a null terminated ascii string containing the ip address of the
// last connection made by curl.
char * Block::HttpGetRemoteIp()
{
	return BP_HTTPGetRemoteLastIP(this->httpSession);
}


// ---------------- USEFUL INJECTION GENERATORS ----------------------


// Takes the input string and converts it to stringFromCharCode
// style.
char * Block::EncXSSInjectionStringEncode(char *str, int len)
{
	lastInjEncode = BP_XSSInjectionStringEncode(str, len);
	return lastInjEncode;
}

// Encodes as UTF8
char * Block::EncUTF8StringEncode(char *str, int len)
{
	lastInjEncode = BP_UTF8StringEncode(str, len);
	return lastInjEncode;
}

// MSQL styled CHAR() encoding ( MICROSOFT )
char * Block::EncMSQLCharFuncInjectionStringEncode(char *str, int len)
{
	lastInjEncode = BP_MSQLCharFuncInjectionStringEncode(str, len);
	return lastInjEncode;
}

// MySQL styled CHAR() encoding (MYSQL)
char * Block::EncMYSQLCharFuncInjectionStringEncode(char *str, int len)
{
	lastInjEncode = BP_MYSQLCharFuncInjectionStringEncode(str, len);
	return lastInjEncode;
}

// *C injection string encoder
char * Block::EncCInjectionStringEncode(char *str, int len)
{
	lastInjEncode = BP_CInjectionStringEncode(str, len);
	return lastInjEncode;
}

// *changes it to a php suitable injection string
char * Block::EncPHPInjectionStringEncode(char *str, int len)
{
	lastInjEncode = BP_PHPInjectionStringEncode(str, len);
	return lastInjEncode;
}

// *INLINE VERSION* Function will take a C string and convert it to a ASM
char * Block::EncInlineASMStringEncode(char *str, int len)
{
	lastInjEncode = BP_InlineASMStringEncode(str, len);
	return lastInjEncode;
}

// *NON-INLINE VERSION* Function will take a C string and convert it to ASM
char * Block::EncASMStringEncode(char *str, int len)
{
	lastInjEncode = BP_ASMStringEncode(str, len);
	return lastInjEncode;
}

// ---------------- HTML EXTRACTION ROUTINES -------------------------


// Provide a buffer, and it will extract a full HTML structure
P_BP_HTML_FULL_EXTRACTION Block::HtmlParse(char *url, bool saveBuffInClass)
{


	// http session is required for a parse
	if (!this->httpSession)
		return NULL;

	// if there is no url, set url as last requested href
	if(!url)
		url = this->httpSession->lastRequestedHref;

	// if we still have no url, return false
	if(!url)
		return NULL;

	if(this->htmlExtraction)
	{
		this->HtmlDestroyExtraction();
		this->htmlExtraction = NULL;
	}

	// create html extraction (null last pointer creates its own garbage collector)
	this->htmlExtraction = HTMLDecodeBuffer
	(
			httpSession->page->page_data,
			httpSession->page->page_data_size,
			url,
			NULL,
			NULL,
			HTML_PARSE_NOERROR,
			NULL,
			this->httpSession
	);

	// if the user has specified that they want to save the buffer,
	// duplicate it here and set the size.
	if (saveBuffInClass)
	{

		// destroy unparsed data first
		if(this->unparsedHtml)
		{
			tq_free(this->tq, this->unparsedHtml);
			this->unparsedHtml = NULL;
			this->unparsedHtmlLen = 0;
		}

		// now set it
		this->unparsedHtml    = tq_strndup(this->tq, httpSession->page->page_data, httpSession->page->page_data_size, BPLN);
		this->unparsedHtmlLen = httpSession->page->page_data_size;

	}

	// return the fully parsed html
	return htmlExtraction;

}

// Prints to stdout a full extraction
void Block::HtmlPrintExtraction()
{
	HTMLPrintFullExtraction(htmlExtraction, this->html_display_blacklisted);
}

// Destroys the current extraction
int Block::HtmlDestroyExtraction()
{

	if(!this->htmlExtraction)
		return ERR_FAILURE;

	// if we saved unparsed html in the class, destroy it first
	if (this->unparsedHtml)
	{

		// destroy unparsed html
		tq_free(this->tq, this->unparsedHtml);
		this->unparsedHtml = NULL;
		this->unparsedHtmlLen = 0;

	}

	// destroy the extraction now
	size_t ret = HTMLDestroyExtraction(this->htmlExtraction);
	this->htmlExtraction = NULL;

	// return indicating success;
	return ret;

}

#endif // BP_BLOCKCLASS_INCLUDE_HTML_BINDINGS

#ifdef BP_BLOCKCLASS_INCLUDE_PROXY_DB_BINDINGS

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%% Proxy DB Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




// add group
BP_ERROR_T addProxyDBGroup
(
		char *group_name
)
{

	if(!group_name)
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(group_name, bpstrlen(group_name), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// add proxy
BP_ERROR_T Block::addProxyDBEntry
(
		size_t group_id,
		size_t proxy_type,
		char * host,
		size_t port,
		char * username,
		char * password,
		char * user_notes,
		size_t enabled
)
{

	// this method requires a valid mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// exit if the mysqli object is unset
	if(!this->mysqlSession->conn)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// destroy statement if one exists
	this->mysqlDestroyStatement();

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%% Build Query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new string builder
	BP_StringUtils * query_builder = new BP_StringUtils((char *) "");

	// select query to run
	char *base_query = (char *) ""\
			"INSERT INTO proxies "\
			" (                              "\
			"     `id`,                      "\
			"     `user_id`,                 "\
			"     `group_id`,                "\
			"     `proxy_type`,              "\
			"     `host`,                    "\
			"     `port`,                    "\
			"     `username`,                "\
			"     `password`,                "\
			"     `user_notes`,              "\
			"     `created_ts`,              "\
			"     `last_validated_ts`,       "\
			"     `enabled`                  "\
			")                               "\
			" VALUES                         "\
			"(                               "\
			"     NULL,                      "\
			"     1,                         "\
			"     ?,                         "\
			"     ?,                         "\
			"     ?,                         "\
			"     ?,                         "/* port */\
			"     ?,                         "/* username */\
			"     ?,                         "\
			"     ?,                         "\
			"     NOW(),                     "\
			"     NULL,                         "\
			"     ? "\
			")";

	// prepare the query
	if(this->mysqlPrepareQuery(base_query) != ERR_SUCCESS)
	{
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%% Add Parameters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// set group id
	this->mysqlAddParameter(MYSQL_TYPE_LONG,   (char *) &group_id,  sizeof(group_id));

	// set proxy type
	this->mysqlAddParameter(MYSQL_TYPE_LONG,   (char *) &proxy_type,  sizeof(proxy_type));

	// set host
	if(host)
		this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *)  host, strnlen(host, 1024)-1);
	else
		this->mysqlAddParameter(MYSQL_TYPE_NULL, (char *)  NULL, 0);

	// set port
	this->mysqlAddParameter(MYSQL_TYPE_LONG,   (char *) &port,  sizeof(port));

	// set password
	if(username)
		this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *)  username, strnlen(username, 1024));
	else
		this->mysqlAddParameter(MYSQL_TYPE_NULL, (char *)  NULL, 0);

	// set password
	if(password)
		this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *)  password, strnlen(password, 1024));
	else
		this->mysqlAddParameter(MYSQL_TYPE_NULL, (char *)  NULL, 0);

	// set user notes
	if(user_notes)
		this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *)  user_notes, strnlen(user_notes, 1024));
	else
		this->mysqlAddParameter(MYSQL_TYPE_NULL, (char *)  NULL, 0);

	// set enabled
	this->mysqlAddParameter(MYSQL_TYPE_LONG,   (char *) &enabled,  sizeof(enabled));


	// bind parameters
	this->mysqlBindParameters();

	// execute statement
	this->mysqlExecuteStatement();

	// store result
	this->mysqlStoreResults();

	// fetch the result into the result_index var
	// this->mysqlFetchResult();

	/* COMMENT OUT START
END COMMMENT*/

	// leak close possibly
	this->mysqlDestroyStatement();

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}


// This attempts to parse any number of entries and sort them out into
// insertable records in the database.
BP_ERROR_T Block::addProxyDBEntriesFromGenericFormatProxyBuff
(
		char *file_containing_proxies_to_insert
)
{

	// this method requires a valid mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;


	if(!file_containing_proxies_to_insert)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	if(!BP_StringIsReasonableForUnixEXT4Directory(file_containing_proxies_to_insert, BP_TRUE))
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// attempt to retrieve file content
	char * file_content = BP_FileGetContents(file_containing_proxies_to_insert);
	if(!file_content)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// explode the file
	char ** exploded = BP_ExplodeBufferBin(file_content, bpstrlen(file_content), "\n", 1, 500000);

	// ensure file was exploded ok
	if(!exploded)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	size_t n = 0;
	for(; exploded[n]; n++)
	{

		// check string to make sure it's reasonable
		if(!BP_StringIsReasonablePrintableString(exploded[n], bpstrlen(exploded[n]), BP_FALSE, BP_FALSE))
			continue;

		// split along colon
		char ** split_buff = BP_SplitBuffer(exploded[n], ":");
		if(!split_buff)
			continue;

		// count split array indexes
		size_t arr_n = BP_CountNullTerminatedPtrArray(split_buff);
		if(arr_n != 2)
		{

			// destroy the split buffer
			BP_DestroyPointerArrayNullTerminated((void **) split_buff);
			continue;

		}

		// set host name and port
		char * host_name  = split_buff[0];
		char * port_ascii = split_buff[1];
		size_t port = strtoul(port_ascii, NULL, 10);

		// add proxy entry
		this->addProxyDBEntry
		(
				2,
				0,
				host_name,
				port,
				NULL,
				NULL,
				NULL,
				1
		);


		// destroy the split buffer
		BP_DestroyPointerArrayNullTerminated((void **) split_buff);

		// printf("\n LINE: %s", exploded[n]);
	}

	if(exploded)
		BP_DestroyPointerArrayNullTerminated((void **) exploded);


	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// the following methods are used for interfacing with the generic blockparty
// proxy database.

// retrieves proxy group sets via criteria (zero / empty values for unset/wildcard search terms)
BP_ERROR_T Block::getProxyDBGroupSetByCriteria
(
		size_t id,
		size_t user_id,
		char *group_name,
		size_t bottom_limit,
		size_t top_limit
)
{

	// this method requires a valid mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// exit if the mysqli object is unset
	if(!this->mysqlSession->conn)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%% Build Query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new string builder
	BP_StringUtils * query_builder = new BP_StringUtils((char *) "");

	// select query to run
	char *base_query = (char *) "SELECT "\
						" `id`, `user_id`, `group_name`, `enabled`, `created_ts`, `group_description` "\
						"FROM `proxy_groups` WHERE 1=1 ";

	// add the base query
	query_builder->BuildString(base_query);

	// dynamically add initial query constraints
	if(id)
		query_builder->BuildString( (char *) " AND `id` = ?");
	if(user_id)
		query_builder->BuildString( (char *) " AND `user_id` = ?");
	if(group_name)
		query_builder->BuildString( (char *) " AND `group_name` = ?");

	// create simple limit string
	char limit_str[256] = {0};

	// populate the limit string (transfer lengths precalculated)
	sprintf((char *) &limit_str, (char *) " LIMIT %u, %u ", bottom_limit, top_limit);

	// append the limit string
	query_builder->BuildString(limit_str);

	// prepare the query
	if(this->mysqlPrepareQuery(query_builder->build_str) != ERR_SUCCESS)
	{
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%% Add Parameters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// dynamically add initial query constraints
	if(id)
		this->mysqlAddParameter(MYSQL_TYPE_LONG,   (char *) &id,  sizeof(id));
	if(user_id)
		this->mysqlAddParameter(MYSQL_TYPE_LONG,   (char *) &user_id,  sizeof(user_id));
	if(group_name)
		this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) group_name, strnlen(group_name, 1024)-1);

	// bind the query parameters before executing statement
	this->mysqlBindParameters();

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%% Add Results %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// allocate new proxy result value
	BP_PROXY_DB_GROUP proxy_result = {0};
	memset((void *)&proxy_result, 0x00, sizeof(proxy_result));

	// `id`
	this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &proxy_result.id, sizeof(proxy_result.id));

	// `user_id`
	this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &proxy_result.user_id, sizeof(proxy_result.user_id));

	// `name`
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &proxy_result.name, sizeof(proxy_result.name)-1);

	// `enabled`
	this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &proxy_result.enabled, sizeof(proxy_result.enabled));

	// `created_ts`
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &proxy_result.created_ts, sizeof(proxy_result.created_ts)-1);

	// `user_notes`
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &proxy_result.user_notes, sizeof(proxy_result.user_notes)-1);

	// now bind results
	this->mysqlBindResults();


	// %%%%%%% Execute the SELECT query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// execute the statement
	if(this->mysqlExecuteStatement() != ERR_SUCCESS)
	{
		printf("\n First execute select statement failed.  Exiting getProxyDBGroupSetByCriteria.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// store the results of the query
	this->mysqlStoreResults();


	// destroy structure if necessary before proceeding
	if(this->db_populated_proxy_groups)
		tq_free(this->tq, this->db_populated_proxy_groups);

	// reset class proxy sets before continuing
	this->db_populated_proxy_groups   = NULL;
	this->db_populated_proxy_group_n  = 0;


	// fetch an individual record
	while(this->mysqlFetchResult() == ERR_SUCCESS)
	{

		// display the fetched record
		// this->displayProxyDBEntryRecord((P_BP_PROXY_DB_ENTRY) &proxy_result);

		// increment the populated proxy array count
		this->db_populated_proxy_group_n++;

		// allocate space for the new set
		this->db_populated_proxy_groups = (P_BP_PROXY_DB_GROUP) tq_realloc(this->tq, (void *) this->db_populated_proxy_groups, sizeof(BP_PROXY_DB_GROUP) * this->db_populated_proxy_group_n, BPLN);

		// copy the retrieved data into class array
		memcpy((void *) &this->db_populated_proxy_groups[this->db_populated_proxy_group_n-1], (void *) &proxy_result, sizeof(proxy_result));

		// zero out the result structure before loop (new record)
		memset((void *)&proxy_result, 0x00, sizeof(proxy_result));

	}

	// destroy statement (free memory)
	this->mysqlDestroyStatement();

	// destroy the query builder
	delete query_builder;

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// generates a proxy set via criteria
BP_ERROR_T Block::getProxyDBSetByCriteria
(
		size_t id,
		size_t user_id,
		size_t group_id,
		size_t proxy_type,
		char *host,
		char *username,
		char *password,
		size_t port,
		char *user_notes,
		size_t validation_status,
		char *created_ts_ge,
		char *created_ts_le,
		size_t bottom_limit,
		size_t top_limit
)
{


	// exit if the mysqli object is unset
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%% Build Query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new string builder
	BP_StringUtils * query_builder = new BP_StringUtils("");

	// select query to run
	char *base_query =    "SELECT "\
						" `id`, `user_id`, `group_id`, `proxy_type`, `host`, `port`, "\
						"`username`, `password`, `user_notes`, `created_ts`, `last_validated_ts`, `enabled`, `validation_status` "\
						"FROM `proxies` WHERE 1=1 ";


	// add the base query
	query_builder->BuildString(base_query);

	// dynamically add initial query constraints
	if(id)
		query_builder->BuildString( (char *) " AND `id` = ?");
	if(user_id)
		query_builder->BuildString( (char *) " AND `user_id` = ?");
	if(group_id)
		query_builder->BuildString( (char *) " AND `group_id` = ?");
	if(proxy_type)
		query_builder->BuildString( (char *) " AND `proxy_type` = ?");
	if(host)
		query_builder->BuildString( (char *) " AND `host` = ?");
	if(username)
		query_builder->BuildString( (char *) " AND `username` = ?");
	if(password)
		query_builder->BuildString( (char *) " AND `password` = ?");
	if(port)
		query_builder->BuildString( (char *) " AND `port` = ?");
	if(user_notes)
		query_builder->BuildString( (char *) " AND `user_notes` = ?");
	if(created_ts_ge)
		query_builder->BuildString( (char *) " AND `created_ts` >= ?");
	if(created_ts_le)
		query_builder->BuildString( (char *) " AND `created_ts` <= ?");

	// no if statement here because zero is a valid status
	if(validation_status != (size_t) -1)
		query_builder->BuildString( (char *) " AND `validation_status` = ?");

	// create simple limit string
	char limit_str[256] = {0};

	// populate the limit string
	sprintf((char *) &limit_str, " LIMIT %u, %u ", bottom_limit, top_limit);

	// append the limit string
	query_builder->BuildString(limit_str);

	// prepare the query
	if(this->mysqlPrepareQuery(query_builder->build_str) != ERR_SUCCESS)
	{
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// printf("\n Proxy Query: %s\n\n", query_builder->build_str);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%% Add Parameters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// dynamically add initial query constraints
	if(id)
		this->mysqlAddParameter(MYSQL_TYPE_LONG,   (char *) &id, sizeof(id));
	if(user_id)
		this->mysqlAddParameter(MYSQL_TYPE_LONG,   (char *) &user_id, sizeof(user_id));
	if(group_id)
		this->mysqlAddParameter(MYSQL_TYPE_LONG,   (char *) &group_id, sizeof(group_id));
	if(proxy_type)
		this->mysqlAddParameter(MYSQL_TYPE_LONG,   (char *) &proxy_type, sizeof(proxy_type));
	if(host)
		this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) host,     strnlen(host, 1024)-1);
	if(username)
		this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) username, strnlen(username, 1024)-1);
	if(password)
		this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) password, strnlen(password, 1024)-1);
	if(port)
		this->mysqlAddParameter(MYSQL_TYPE_LONG,   (char *) &port, sizeof(port));
	if(user_notes)
		this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) user_notes, strnlen(user_notes, 1024)-1);
	if(created_ts_ge)
		this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) created_ts_ge, strnlen(created_ts_ge, 1024)-1);
	if(created_ts_le)
		this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) created_ts_le, strnlen(created_ts_le, 1024)-1);

	// no if statmenet, always include validation status
	if(validation_status != (size_t) -1)
		this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &validation_status, sizeof(validation_status));

	// bind the query parameters before executing statement
	this->mysqlBindParameters();

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%% Add Results %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// allocate new proxy result value
	BP_PROXY_DB_ENTRY proxy_result = {0};
	memset((void *)&proxy_result, 0x00, sizeof(proxy_result));

	// `id`
	this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &proxy_result.id, sizeof(proxy_result.id));

	// `user_id`
	this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &proxy_result.user_id, sizeof(proxy_result.user_id));

	// `group_id`
	this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &proxy_result.group_id, sizeof(proxy_result.group_id));

	// `proxy_type`
	this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &proxy_result.proxy_type, sizeof(proxy_result.proxy_type));

	// `host`
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &proxy_result.host, sizeof(proxy_result.host)-1);

	// `port`
	this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &proxy_result.port, sizeof(proxy_result.port));

	// `username`
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &proxy_result.username, sizeof(proxy_result.username)-1);

	// `password`
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &proxy_result.password, sizeof(proxy_result.password)-1);

	// `user_notes
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &proxy_result.user_notes, sizeof(proxy_result.user_notes)-1);

	// `created_ts`
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &proxy_result.created_ts, sizeof(proxy_result.created_ts)-1);

	// `last_validated_ts`
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &proxy_result.last_validated_ts, sizeof(proxy_result.last_validated_ts)-1);

	// `enabled`
	this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &proxy_result.enabled, sizeof(proxy_result.enabled));

	// validation_status
	this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &proxy_result.validation_status, sizeof(proxy_result.validation_status));

	// now bind results
	this->mysqlBindResults();


	// %%%%%%% Execute the SELECT query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// execute the statement
	if(this->mysqlExecuteStatement() != ERR_SUCCESS)
	{
		printf("\n First execute select statement failed.  Exiting unit test.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// store the results of the query
	this->mysqlStoreResults();

	// destroy structure if necessary before proceeding
	if(this->db_populated_proxy_sets)
		tq_free(this->tq, this->db_populated_proxy_sets);

	// reset class proxy sets before continuing
	this->db_populated_proxy_sets = NULL;
	this->db_populated_proxy_n    = 0;


	// fetch an individual record
	while(this->mysqlFetchResult() == ERR_SUCCESS)
	{

		// display the fetched record
		// this->displayProxyDBEntryRecord((P_BP_PROXY_DB_ENTRY) &proxy_result);

		// increment the populated proxy array count
		this->db_populated_proxy_n++;

		// allocate space for the new set
		this->db_populated_proxy_sets = (P_BP_PROXY_DB_ENTRY) tq_realloc(this->tq, (void *) this->db_populated_proxy_sets, sizeof(BP_PROXY_DB_ENTRY) * this->db_populated_proxy_n, BPLN);

		// column iterator
		size_t col_iter = 0;

		// " `id`, "
		memcpy(&this->db_populated_proxy_sets[this->db_populated_proxy_n-1].id, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// "`user_id`, "
		memcpy(&this->db_populated_proxy_sets[this->db_populated_proxy_n-1].user_id, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// "`group_id`, "
		memcpy(&this->db_populated_proxy_sets[this->db_populated_proxy_n-1].group_id, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// "`proxy_type`, "
		memcpy(&this->db_populated_proxy_sets[this->db_populated_proxy_n-1].proxy_type, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// "`host`, "
		memcpy(&this->db_populated_proxy_sets[this->db_populated_proxy_n-1].host, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// "`port`, "
		memcpy(&this->db_populated_proxy_sets[this->db_populated_proxy_n-1].port, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// "`username`, "
		memcpy(&this->db_populated_proxy_sets[this->db_populated_proxy_n-1].username, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// "`password`, "
		memcpy(&this->db_populated_proxy_sets[this->db_populated_proxy_n-1].password, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// "`user_notes`, "
		memcpy(&this->db_populated_proxy_sets[this->db_populated_proxy_n-1].user_notes, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// "`created_ts`, "
		memcpy(&this->db_populated_proxy_sets[this->db_populated_proxy_n-1].created_ts, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// "`last_validated_ts`, "
		memcpy(&this->db_populated_proxy_sets[this->db_populated_proxy_n-1].last_validated_ts, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// "`enabled`, "
		memcpy(&this->db_populated_proxy_sets[this->db_populated_proxy_n-1].enabled, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// "`validation_status`
		memcpy(&this->db_populated_proxy_sets[this->db_populated_proxy_n-1].validation_status, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;


		// zero out the result structure before loop (new record)
		memset((void *)&proxy_result, 0x00, sizeof(proxy_result));

	}

	// destroy statement (free memory)
	this->mysqlDestroyStatement();

	// destroy the query builder
	delete query_builder;

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// pulls proxy validation groups by provided criteria.  null criteria is an empty (blanket) search.
BP_ERROR_T Block::getProxyDBValidationGroupSetByCriteria(size_t id, size_t user_id, char *group_name, char *group_description, char *created_ts, size_t bottom_limit, size_t top_limit)
{


	// exit if the mysqli object is unset
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%% Build Query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new string builder
	BP_StringUtils * query_builder = new BP_StringUtils("");

	// select query to run
	char *base_query =    "SELECT "\
						" `id`, `user_id`, `group_name`, `group_description`, `created_ts` "\
						"FROM `validation_test_groups` WHERE 1=1 ";


	// add the base query
	query_builder->BuildString(base_query);

	// dynamically add initial query constraints
	if(id)
		query_builder->BuildString( (char *) " AND `id` = ?");
	if(user_id)
		query_builder->BuildString( (char *) " AND `user_id` = ?");
	if(group_name)
		query_builder->BuildString( (char *) " AND `group_name` = ?");
	if(group_description)
		query_builder->BuildString( (char *) " AND `group_description` = ?");
	if(created_ts)
		query_builder->BuildString( (char *) " AND `created_ts` = ?");
	// create simple limit string
	char limit_str[256] = {0};

	// populate the limit string
	sprintf((char *) &limit_str, " LIMIT %u, %u ", bottom_limit, top_limit);

	// append the limit string
	query_builder->BuildString(limit_str);

	// prepare the query
	if(this->mysqlPrepareQuery(query_builder->build_str) != ERR_SUCCESS)
	{
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%% Add Parameters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// dynamically add initial query constraints
	if(id)
		this->mysqlAddParameter(MYSQL_TYPE_LONG,     (char *) &id, sizeof(id));
	if(user_id)
		this->mysqlAddParameter(MYSQL_TYPE_LONG,     (char *) &user_id, sizeof(user_id));
	if(group_name)
		this->mysqlAddParameter(MYSQL_TYPE_STRING,   (char *) group_name, strnlen(group_name, 1024)-1);
	if(group_description)
		this->mysqlAddParameter(MYSQL_TYPE_STRING,   (char *) group_description, strnlen(group_description, 1024)-1 );
	if(created_ts)
		this->mysqlAddParameter(MYSQL_TYPE_STRING,   (char *) created_ts, strnlen(created_ts, 1024)-1);

	// bind the query parameters before executing statement
	this->mysqlBindParameters();

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%% Add Results %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// allocate new proxy result value
	BP_PROXY_DB_VALIDATION_TEST_GROUP proxy_result = {0};
	memset((void *)&proxy_result, 0x00, sizeof(proxy_result));

	// `id`
	this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &proxy_result.id, sizeof(proxy_result.id));

	// `user_id`
	this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &proxy_result.user_id, sizeof(proxy_result.user_id));

	// `group_name`
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &proxy_result.group_name, sizeof(proxy_result.group_name));

	// `group_description`
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &proxy_result.group_description, sizeof(proxy_result.group_description));

	// `created_ts`
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &proxy_result.created_ts, sizeof(proxy_result.created_ts));

	// now bind results
	this->mysqlBindResults();


	// %%%%%%% Execute the SELECT query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// execute the statement
	if(this->mysqlExecuteStatement() != ERR_SUCCESS)
	{
		printf("\n First execute select statement failed.  Exiting unit test.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// store the results of the query
	this->mysqlStoreResults();

	// destroy structure if necessary before proceeding
	if(this->db_populated_proxy_validation_groups)
		tq_free(this->tq, this->db_populated_proxy_validation_groups);

	// reset class proxy sets before continuing
	this->db_populated_proxy_validation_groups     = NULL;
	this->db_populated_proxy_validation_group_n    = 0;


	// fetch an individual record
	while(this->mysqlFetchResult() == ERR_SUCCESS)
	{

		// increment the populated proxy array count
		this->db_populated_proxy_validation_group_n++;

		// allocate space for the new set
		this->db_populated_proxy_validation_groups = (P_BP_PROXY_DB_VALIDATION_TEST_GROUP) tq_realloc(this->tq, (void *) this->db_populated_proxy_validation_groups, sizeof(BP_PROXY_DB_VALIDATION_TEST_GROUP) * this->db_populated_proxy_validation_group_n, BPLN);


		// column iterator
		size_t col_iter = 0;

		// `id`,
		memcpy(&this->db_populated_proxy_validation_groups[this->db_populated_proxy_validation_group_n-1].id, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// `user_id`,
		memcpy(&this->db_populated_proxy_validation_groups[this->db_populated_proxy_validation_group_n-1].user_id, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// `group_name`,
		memcpy(&this->db_populated_proxy_validation_groups[this->db_populated_proxy_validation_group_n-1].group_name, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// `group_description`,
		memcpy(&this->db_populated_proxy_validation_groups[this->db_populated_proxy_validation_group_n-1].group_description, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// `created_ts`
		memcpy(&this->db_populated_proxy_validation_groups[this->db_populated_proxy_validation_group_n-1].created_ts, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// zero out the result structure before loop (new record)
		memset((void *)&proxy_result, 0x00, sizeof(proxy_result));

	}

	// destroy statement (free memory)
	this->mysqlDestroyStatement();

	// destroy the query builder
	delete query_builder;

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// pulls proxy validation tests by provided criteria.  null criteria is an empty (blanket search)
BP_ERROR_T Block::getProxyDBValidationTestSetsByCriteria(size_t id, size_t user_id, size_t group_id, char *url_fetch, char *post_args, char *regexp_match, size_t count_invoked, char *created, size_t bottom_limit, size_t top_limit)
{

	// exit if the mysqli object is unset
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%% Build Query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new string builder
	BP_StringUtils * query_builder = new BP_StringUtils("");

	// select query to run
	char *base_query =    "SELECT "\
						" `id`, `user_id`, `group_id`, `url_fetch`, `post_args`, `regexp_match`, `count_invoked`, `created` "\
						"FROM `validation_tests` WHERE 1=1 ";


	// add the base query
	query_builder->BuildString(base_query);

	// dynamically add initial query constraints
	if(id)
		query_builder->BuildString( (char *) " AND `id` = ?");
	if(user_id)
		query_builder->BuildString( (char *) " AND `user_id` = ? ");
	if(group_id)
		query_builder->BuildString( (char *) " AND `group_id` = ? ");
	if(url_fetch)
		query_builder->BuildString( (char *) " AND `url_fetch` = ? ");
	if(post_args)
		query_builder->BuildString( (char *) " AND `post_args` = ? ");
	if(regexp_match)
		query_builder->BuildString( (char *) " AND `regexp_match` = ? ");
	if(count_invoked)
		query_builder->BuildString( (char *) " AND `count_invoked` = ? ");
	if(created)
		query_builder->BuildString( (char *) " AND `created` = ? ");

	// create simple limit string
	char limit_str[256] = {0};

	// populate the limit string
	sprintf((char *) &limit_str, " LIMIT %u, %u ", bottom_limit, top_limit);

	// append the limit string
	query_builder->BuildString(limit_str);

	// prepare the query
	if(this->mysqlPrepareQuery(query_builder->build_str) != ERR_SUCCESS)
	{
		delete query_builder;
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%% Add Parameters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// dynamically add initial query constraints
	if(id)
		this->mysqlAddParameter(MYSQL_TYPE_LONG,     (char *) &id, sizeof(id));
	if(user_id)
		this->mysqlAddParameter(MYSQL_TYPE_LONG,     (char *) &user_id, sizeof(user_id));
	if(group_id)
		this->mysqlAddParameter(MYSQL_TYPE_LONG,   (char *) &group_id, sizeof(group_id));
	if(url_fetch)
		this->mysqlAddParameter(MYSQL_TYPE_STRING,   (char *) url_fetch, strnlen(url_fetch, 1024)-1);
	if(post_args)
		this->mysqlAddParameter(MYSQL_TYPE_STRING,   (char *) post_args, strnlen(post_args, 1024)-1);
	if(regexp_match)
		this->mysqlAddParameter(MYSQL_TYPE_STRING,   (char *) regexp_match, strnlen(regexp_match, 1024)-1);
	if(count_invoked)
		this->mysqlAddParameter(MYSQL_TYPE_LONG,   (char *) &count_invoked, sizeof(count_invoked));
	if(created)
		this->mysqlAddParameter(MYSQL_TYPE_STRING,   (char *) created, strnlen(created, 1024)-1);

	// bind the query parameters before executing statement
	this->mysqlBindParameters();

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%% Add Results %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// allocate new proxy result value
	BP_PROXY_DB_VALIDATION_TEST_ENTRY proxy_result = {0};
	memset((void *)&proxy_result, 0x00, sizeof(proxy_result));

	// `id`
	this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &proxy_result.id, sizeof(proxy_result.id));

	// `user_id`
	this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &proxy_result.user_id, sizeof(proxy_result.user_id));

	// `group_id`
	this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &proxy_result.group_id, sizeof(proxy_result.group_id));

	// `url_fetch`
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &proxy_result.url_fetch, sizeof(proxy_result.url_fetch));

	// `post_args`
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &proxy_result.post_args, sizeof(proxy_result.post_args));

	// `regexp_match`
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &proxy_result.regexp_match, sizeof(proxy_result.regexp_match));

	// `count_invoked`
	this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &proxy_result.count_invoked, sizeof(proxy_result.count_invoked));

	// `created`
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &proxy_result.created, sizeof(proxy_result.created));

	// now bind results
	this->mysqlBindResults();


	// %%%%%%% Execute the SELECT query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// execute the statement
	if(this->mysqlExecuteStatement() != ERR_SUCCESS)
	{
		printf("\n First execute select statement failed.  Exiting unit test.");
		delete query_builder;
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// store the results of the query
	this->mysqlStoreResults();

	// destroy structure if necessary before proceeding
	if(this->db_populated_proxy_validation_tests)
		tq_free(this->tq, this->db_populated_proxy_validation_tests);

	// reset class proxy sets before continuing
	this->db_populated_proxy_validation_tests     = NULL;
	this->db_populated_proxy_validation_test_n    = 0;


	// fetch an individual record
	while(this->mysqlFetchResult() == ERR_SUCCESS)
	{

		// increment the populated proxy array count
		this->db_populated_proxy_validation_test_n++;

		// allocate space for the new set
		this->db_populated_proxy_validation_tests = (P_BP_PROXY_DB_VALIDATION_TEST_ENTRY) tq_realloc(this->tq, (void *) this->db_populated_proxy_validation_tests, sizeof(BP_PROXY_DB_VALIDATION_TEST_ENTRY) * this->db_populated_proxy_validation_test_n, BPLN);


		// column iterator
		size_t col_iter = 0;


		// `id`,
		memcpy(&this->db_populated_proxy_validation_tests[this->db_populated_proxy_validation_test_n-1].id, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// `user_id`,
		memcpy(&this->db_populated_proxy_validation_tests[this->db_populated_proxy_validation_test_n-1].user_id, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// `group_id`,
		memcpy(&this->db_populated_proxy_validation_tests[this->db_populated_proxy_validation_test_n-1].group_id, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// `url_fetch`,
		memcpy(&this->db_populated_proxy_validation_tests[this->db_populated_proxy_validation_test_n-1].url_fetch, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// `post_args`,
		memcpy(&this->db_populated_proxy_validation_tests[this->db_populated_proxy_validation_test_n-1].post_args, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// `regexp_match`,
		memcpy(&this->db_populated_proxy_validation_tests[this->db_populated_proxy_validation_test_n-1].regexp_match, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// `count_invoked`,
		memcpy(&this->db_populated_proxy_validation_tests[this->db_populated_proxy_validation_test_n-1].count_invoked, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// `created`
		memcpy(&this->db_populated_proxy_validation_tests[this->db_populated_proxy_validation_test_n-1].created, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;


		// zero out the result structure before loop (new record)
		memset((void *)&proxy_result, 0x00, sizeof(proxy_result));

	}

	// destroy statement (free memory)
	this->mysqlDestroyStatement();

	// destroy the query builder
	delete query_builder;

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// ======== Update(s) Proxy DB Entries ====================================================



// Update a proxy db entry in the database.  Requires a valid proxy db entry item.
BP_ERROR_T Block::updateProxyDBEntryInDB(P_BP_PROXY_DB_ENTRY record_to_update)
{

	// ensure we have a record to update
	if(!record_to_update)
	{
		this->AddErrorToStack("Attempted to update a null proxy db entry.");
		return ERR_FAILURE;
	}

	// exit if the mysqli object is unset
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%% Build Query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new string builder
	BP_StringUtils * query_builder = new BP_StringUtils("");

	// select query to run
	char *base_query =    "UPDATE `proxies` SET `id` = `id` ";

	// add the base query
	query_builder->BuildString(base_query);

	// add user id as a query constraint if its set
	if(record_to_update->user_id)
	{
		query_builder->BuildString( (char *) ", `user_id` = ? ");
	}

	// add group id as query constraint if its set
	if(record_to_update->group_id)
	{
		query_builder->BuildString( (char *) ", `group_id` = ? ");
	}

	// add proxy type as query constraint if set
	if(record_to_update->proxy_type)
	{
		query_builder->BuildString( (char *) ", `proxy_type` = ? ");
	}

	// add host as query constraint if set
	if(strnlen(record_to_update->host, 2047))
	{
		query_builder->BuildString( (char *) ", `host` = ? ");
	}

	// add port as a query constraint if its set
	if(record_to_update->port)
	{
		query_builder->BuildString( (char *) ", `port` = ? ");
	}

	// add username as a query constraint if its set
	if(strnlen(record_to_update->username, 2047))
	{
		query_builder->BuildString( (char *) ", `username` = ? ");
	}

	// add password as a query constraint if its set
	if(strnlen(record_to_update->password, 2047))
	{
		query_builder->BuildString( (char *) ", `password` = ? ");
	}

	// add user notes as a query constraint if its set
	if(strnlen(record_to_update->user_notes, 2047))
	{
		query_builder->BuildString( (char *) ", `user_notes` = ? ");
	}

	// add created timestamp as a query constraint if set
	if(strnlen(record_to_update->created_ts, 255))
	{
		query_builder->BuildString( (char *) ", `created_ts` = ? ");
	}

	// set the last validated ts as constraint
	if(strnlen(record_to_update->last_validated_ts, 255))
	{
		query_builder->BuildString( (char *) ", `last_validated_ts` = NOW() ");
	}

	// set the enabled/disabled status as a constraint
	if(record_to_update->enabled)
	{
		query_builder->BuildString( (char *) ", `enabled` = ? ");
	}

	// always add validation status (should be 0 or 1)
	query_builder->BuildString( (char *) ", `validation_status` = ? ");

	// always add the id limiter
	query_builder->BuildString( (char *) " WHERE `id` = ? ");


	// always add limit string (only one update at a time)
	query_builder->BuildString( (char *) " LIMIT 1");

	// prepare the query
	if(this->mysqlPrepareQuery(query_builder->build_str) != ERR_SUCCESS)
	{
		delete query_builder;
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%% Add Parameters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// add user id as a query constraint if its set
	if(record_to_update->user_id)
	{
		this->mysqlAddParameter(MYSQL_TYPE_LONG,     (char *) &record_to_update->user_id, sizeof(record_to_update->user_id));
	}

	// add group id as query constraint if its set
	if(record_to_update->group_id)
	{
		this->mysqlAddParameter(MYSQL_TYPE_LONG,     (char *) &record_to_update->group_id, sizeof(record_to_update->group_id));
	}

	// add proxy type as query constraint if set
	if(record_to_update->proxy_type)
	{
		this->mysqlAddParameter(MYSQL_TYPE_LONG,     (char *) &record_to_update->proxy_type, sizeof(record_to_update->proxy_type));
	}

	// add host as query constraint if set
	if(strnlen(record_to_update->host, 2047))
	{
		this->mysqlAddParameter(MYSQL_TYPE_STRING,     (char *) &record_to_update->host, sizeof(record_to_update->host)-1);
	}

	// add port as a query constraint if its set
	if(record_to_update->port)
	{
		this->mysqlAddParameter(MYSQL_TYPE_LONG,     (char *) &record_to_update->port, sizeof(record_to_update->port));
	}

	// add username as a query constraint if its set
	if(strnlen(record_to_update->username, 2047))
	{
		this->mysqlAddParameter(MYSQL_TYPE_STRING,     (char *) &record_to_update->username, sizeof(record_to_update->username)-1);
	}

	// add password as a query constraint if its set
	if(strnlen(record_to_update->password, 2047))
	{
		this->mysqlAddParameter(MYSQL_TYPE_STRING,     (char *) &record_to_update->password, sizeof(record_to_update->password)-1);
	}

	// add user notes as a query constraint if its set
	if(strnlen(record_to_update->user_notes, 2047))
	{
		this->mysqlAddParameter(MYSQL_TYPE_STRING,     (char *) &record_to_update->user_notes, sizeof(record_to_update->user_notes)-1);
	}

	// add created timestamp as a query constraint if set
	if(strnlen(record_to_update->created_ts, 255))
	{
		this->mysqlAddParameter(MYSQL_TYPE_STRING,     (char *) &record_to_update->created_ts, sizeof(record_to_update->created_ts)-1);
	}

	// set the last validated ts as constraint
	if(strnlen(record_to_update->last_validated_ts, 255))
	{
		// this is set via NOW()
		// this->mysqlAddParameter(MYSQL_TYPE_STRING,     (char *) &record_to_update->last_validated_ts, sizeof(record_to_update->last_validated_ts)-1);
	}

	// set the enabled/disabled status as a constraint
	if(record_to_update->enabled)
	{
		this->mysqlAddParameter(MYSQL_TYPE_LONG,     (char *) &record_to_update->enabled, sizeof(record_to_update->enabled));
	}

	// always add validation data
	this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &record_to_update->proxy_has_been_validated_at_runtime, sizeof(record_to_update->proxy_has_been_validated_at_runtime));

	// add specific id limitation
	this->mysqlAddParameter(MYSQL_TYPE_LONG,     (char *) &record_to_update->id, sizeof(record_to_update->id));

	// bind the query parameters before executing statement
	this->mysqlBindParameters();

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%% Add Results %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// -- no results will be generated for this routine ----

	// %%%%%%% Execute the SELECT query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// execute the statement
	if(this->mysqlExecuteStatement() != ERR_SUCCESS)
	{
		this->AddErrorToStack((char *) "Unable to execute proxy db entry update query.");
		delete query_builder;
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// destroy statement (free memory)
	this->mysqlDestroyStatement();

	// destroy the query builder
	delete query_builder;

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// Updates all the proxy db entries set in the class.
BP_ERROR_T Block::updateProxyDBEntriesInDB(bool only_update_runtime_validated)
{

	// exit if the mysqli object is unset
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// simple pointer/count check
	if(!this->db_populated_proxy_n || !this->db_populated_proxy_sets)
	{
		this->AddErrorToStack("The proxy db entries in this class are unset/NULL/zero.  Ensure you have a proxy set before calling this routine in the future.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// loop through the test sets and record elements
	size_t n = 0;
	for(; n < this->db_populated_proxy_n; n++)
	{

		// continue the loop if the developer has specified to only update runtime validated proxies
		if(only_update_runtime_validated)
		if(!this->db_populated_proxy_sets[n].proxy_has_been_validated_at_runtime)
		{
			continue;
		}

		// only update proxies with non-zero ids
		if(!this->db_populated_proxy_sets[n].id)
			continue;

		// update the proxy id
		printf("\n Attempting to update proxy id %u", this->db_populated_proxy_sets[n].id);

		// attempt to update proxy
		this->updateProxyDBEntryInDB((P_BP_PROXY_DB_ENTRY) &this->db_populated_proxy_sets[n]);

	}

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}



// ======== Attempts to Validate ProxyDB Records ==========================================

// Attempt to validate a proxydb entry (requires valid httpSession set in class to work).
BP_ERROR_T validateProxyDBEntry(P_BP_PROXY_DB_ENTRY entry, P_BP_PROXY_DB_VALIDATION_TEST_ENTRY validation_test, bool brute_force_proxy_type = true)
{

	// return indicating success
	return ERR_SUCCESS;

}

// Attempt to validate all proxies in the current proxy set.  This action will modify data items within
// the current proxy sets to reflect validation status (aka it will toggle enabled/disabled fields).
BP_ERROR_T Block::validateProxyDBEntries(size_t tests_to_try_before_failure_n, bool brute_force_proxy_type, bool use_threads, size_t thread_n, size_t timeout_in_seconds)
{


	// exit if the mysqli object is unset
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// ensure we have a number of tests to test with
	if(!tests_to_try_before_failure_n)
		return ERR_FAILURE;

	// allocate space for parameters
	void ** params = (void **) tq_calloc(this->tq, (this->db_populated_proxy_n+1) * (sizeof(void*)), 1, BPLN);

	// loop through proxies and set pointers in parameter stack
	size_t n = 0;
	for(; n < this->db_populated_proxy_n; n++)
	{

		// allocate parameter structure (this is tq_free(this->tq, )d from inside the thread.  No need to worry about cleanup of
		// this particular structure.  This tq_free(this->tq, ) scheme isn't specific to the thread processor method below, but rather simply specific
		// to this routine.
		P_BP_DB_VALIDATION_THREAD_PARMS thread_parm_item = (P_BP_DB_VALIDATION_THREAD_PARMS) tq_calloc(this->tq, sizeof(BP_DB_VALIDATION_THREAD_PARMS), 1, BPLN);

		// set proxy entry to test (since this routine is blocking with pthread_join(), it's fine to leave mutable
		// data here (i think, whiskey, mrrrrggh).
		thread_parm_item->proxy_entry = (P_BP_PROXY_DB_ENTRY) &this->db_populated_proxy_sets[n];

		// set test entries
		thread_parm_item->test_entries = (P_BP_PROXY_DB_VALIDATION_TEST_ENTRY) this->db_populated_proxy_validation_tests;
		thread_parm_item->test_entry_n = this->db_populated_proxy_validation_test_n;

		// set the max number of tests to try before failure
		thread_parm_item->test_attempt_before_failing_validation = tests_to_try_before_failure_n;

		// set flag inidicating whether or not to brute force a proxy type
		thread_parm_item->brute_force_proxy_type = brute_force_proxy_type;

		// set timeout (in seconds)
		thread_parm_item->timeout = timeout_in_seconds;

		// --

		// set parameter in dispatcher item stack (this data will be processed iteratively in a threaded environment)
		params[n] = (void *) thread_parm_item;

	}

	// run the threaded data processor to evaluate the proxies individually
	this->threadedDataProcessor(thread_n, BP_HTTPValidateProxyTypeByBruteForce_thinit, params, n);

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}


// ======== Attach Proxies to Http Session (required before autoproxying can be done) =====

BP_ERROR_T Block::attachProxyDBRecordsToHttpSession(P_BP_HTTP_SESSION session)
{

	// exit if the mysqli object is unset
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// select either a provided session, or the current session in the class
	P_BP_HTTP_SESSION use_session = NULL;
	if(session)
		use_session = session;
	else
		use_session = this->httpSession;

	// ensure at least one session is set
	if(!use_session)
	{
		this->AddErrorToStack((char*) "Unable to attach proxy db records to http session.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}


	// // set indicator to true (will use proxies on request)
	use_session->use_db_proxy_set = true;

	// set proxy information in session
	use_session->db_populated_proxy_groups             = this->db_populated_proxy_groups;
	use_session->db_populated_proxy_group_n            = this->db_populated_proxy_group_n;
	use_session->db_populated_proxy_sets               = this->db_populated_proxy_sets;
	use_session->db_populated_proxy_n                  = this->db_populated_proxy_n;
	use_session->db_populated_proxy_validation_groups  = this->db_populated_proxy_validation_groups;
	use_session->db_populated_proxy_validation_group_n = this->db_populated_proxy_validation_group_n;
	use_session->db_populated_proxy_validation_tests   = this->db_populated_proxy_validation_tests;
	use_session->db_populated_proxy_validation_test_n  = this->db_populated_proxy_validation_test_n;

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// attach regexp tests to http session
BP_ERROR_T Block::attachJITRegexpTestRecordsToHttpSession()
{

	if(!this->web_request_jit_match_set)
		return ERR_FAILURE;
	if(!this->web_request_jit_match_n)
		return ERR_FAILURE;
	if(!this->httpSession)
		return ERR_FAILURE;

	// set in http session (memory is duplicated) and set check directly
	this->httpSession->jit_pcre_regexp_checks  = (P_BP_PCRE_JIT_MATCH_ENTRY) memdup((void *) this->web_request_jit_match_set, sizeof(BP_PCRE_JIT_MATCH_ENTRY) * this->web_request_jit_match_n);
	this->httpSession->jit_pcre_regexp_check_n = this->web_request_jit_match_n;

	// return indicating success
	return ERR_SUCCESS;

}


// =======  Destroy Data ==========================

// destroys proxy related data
BP_ERROR_T Block::destroyProxyDBRelatedData()
{

	if(this->db_populated_proxy_groups)
	{
		tq_free(this->tq, this->db_populated_proxy_groups);
		this->db_populated_proxy_groups = NULL;
		this->db_populated_proxy_group_n = 0;
	}
	if(this->db_populated_proxy_sets)
	{
		tq_free(this->tq, this->db_populated_proxy_sets);
		this->db_populated_proxy_sets = NULL;
		this->db_populated_proxy_n = 0;
	}

	if(this->db_populated_proxy_validation_groups)
	{
		tq_free(this->tq, this->db_populated_proxy_validation_groups);
		this->db_populated_proxy_validation_groups = NULL;
		this->db_populated_proxy_validation_group_n = 0;
	}

	if(this->db_populated_proxy_validation_tests)
	{
		tq_free(this->tq, this->db_populated_proxy_validation_tests);
		this->db_populated_proxy_validation_tests = NULL;
		this->db_populated_proxy_validation_test_n = 0;
	}

	// return indicating success
	return ERR_SUCCESS;

}

// display all jit matches
BP_ERROR_T Block::displayAllJITMatches()
{

	// ensure we have some matches to display
	if(!this->web_request_jit_match_n)
		return ERR_FAILURE;

	size_t n = 0;
	for(; n < this->web_request_jit_match_n; n++)
	{

		// display the test
		BP_DisplayJITRegexpTest(&this->web_request_jit_match_set[n]);

	}

	// return indicating success
	return ERR_SUCCESS;
}

// ================


// display current proxy records stored in class
BP_ERROR_T Block::displayProxyDBEntryRecords(bool only_display_runtime_validated)
{

	// check to see that a value is set
	if(!this->db_populated_proxy_n)
		return ERR_FAILURE;

	// display individual records
	size_t n = 0;
	for(; n < this->db_populated_proxy_n; n++)
	{
		this->displayProxyDBEntryRecord((P_BP_PROXY_DB_ENTRY) &this->db_populated_proxy_sets[n], only_display_runtime_validated);
	}

	// return indicating success
	return ERR_SUCCESS;

}

// display proxy record
BP_ERROR_T Block::displayProxyDBEntryRecord(P_BP_PROXY_DB_ENTRY record, bool only_display_runtime_validated)
{

	// ensure we have a record pointer
	if(!record)
		return ERR_FAILURE;

	// check to see if it has been runtime validated
	if(only_display_runtime_validated)
	if(!record->proxy_has_been_validated_at_runtime)
		return ERR_FAILURE;

	// display record
	printf("\n\t [+] DB Proxy Entry %p", record);
	printf("\n\t\t id:                %u", record->id);
	printf("\n\t\t user_id:           %u", record->user_id);
	printf("\n\t\t group_id:          %u", record->group_id);
	printf("\n\t\t proxy_type:        %u", record->proxy_type);
	printf("\n\t\t host:              %s", record->host);
	printf("\n\t\t port:              %u", record->port);
	printf("\n\t\t username:          %s", record->username);
	printf("\n\t\t password:          %s", record->password);
	printf("\n\t\t user_notes:        %s", record->user_notes);
	printf("\n\t\t created_ts:        %s", record->created_ts);
	printf("\n\t\t last_validated_ts: %s", record->last_validated_ts);
	printf("\n\t\t runtime validated: %s", record->proxy_has_been_validated_at_runtime ? "yes" : "no");
	printf("\n\t\t marked as enabled: %s", record->enabled ? "yes" : "no");
	printf("\n\t\t validation status (from db): %u", record->validation_status);

	printf("\n");

	// exit indicating success
	return ERR_SUCCESS;

}

// display proxy group record
BP_ERROR_T Block::displayProxyDBGroupRecord(P_BP_PROXY_DB_GROUP record)
{

	if(!record)
		return ERR_FAILURE;

	// display record
	printf("\n\t [+] DB Proxy Group %p",   record);
	printf("\n\t\t id:                %u", record->id);
	printf("\n\t\t user_id:           %u", record->user_id);
	printf("\n\t\t name:              %s", record->name);
	printf("\n\t\t enabled:           %u", record->enabled);
	printf("\n\t\t created_ts:        %s", record->created_ts);
	printf("\n\t\t user_notes:        %s", record->user_notes);

	// exit indicating success
	return ERR_SUCCESS;

}


// display current proxy group records stored in class
BP_ERROR_T Block::displayProxyDBGroupRecords()
{

	// check to see that a value is set
	if(!this->db_populated_proxy_groups)
		return ERR_FAILURE;

	// display individual records
	size_t n = 0;
	for(; n < this->db_populated_proxy_group_n; n++)
		this->displayProxyDBGroupRecord((P_BP_PROXY_DB_GROUP) &this->db_populated_proxy_groups[n]);

	// return indicating success
	return ERR_SUCCESS;

}


// display proxy validation group record
BP_ERROR_T Block::displayProxyDBValidationGroupRecord(P_BP_PROXY_DB_VALIDATION_TEST_GROUP record)
{

	if(!record)
		return ERR_FAILURE;

	// display record
	printf("\n\t [+] DB Proxy Validation Group %p",   record);
	printf("\n\t\t id:                  %u", record->id);
	printf("\n\t\t user_id:             %u", record->user_id);
	printf("\n\t\t group name:          %s", record->group_name);
	printf("\n\t\t group_descriptions:  %s", record->group_description);
	printf("\n\t\t created_ts:          %s", record->created_ts);

	// exit indicating success
	return ERR_SUCCESS;

}


// display current proxy group records stored in class
BP_ERROR_T Block::displayProxyDBValidationGroupRecords()
{

	// check to see that a value is set
	if(!this->db_populated_proxy_validation_groups)
		return ERR_FAILURE;

	// display individual records
	size_t n = 0;
	for(; n < this->db_populated_proxy_validation_group_n; n++)
		this->displayProxyDBValidationGroupRecord((P_BP_PROXY_DB_VALIDATION_TEST_GROUP) &this->db_populated_proxy_validation_groups[n]);

	// return indicating success
	return ERR_SUCCESS;

}

// display a proxy validation test entry
BP_ERROR_T Block::displayProxyDBValidationTestEntryRecord(P_BP_PROXY_DB_VALIDATION_TEST_ENTRY record)
{

	if(!record)
		return ERR_FAILURE;

	// display record
	printf("\n\t [+] DB Proxy Validation Test Entry %p",   record);
	printf("\n\t\t id:                  %u", record->id);
	printf("\n\t\t user_id:             %u", record->user_id);
	printf("\n\t\t group id:            %u", record->group_id);
	printf("\n\t\t url_fetch:           %s", record->url_fetch);
	printf("\n\t\t post_args:           %s", record->post_args);
	printf("\n\t\t regexp_match:        %s", record->regexp_match);
	printf("\n\t\t count_invoked:       %u", record->count_invoked);
	printf("\n\t\t created:             %s", record->created);

	// exit indicating success
	return ERR_SUCCESS;

}

// display current proxy group records stored in class
BP_ERROR_T Block::displayProxyDBValidationTestEntryRecords()
{

	// check to see that a value is set
	if(!this->db_populated_proxy_validation_tests)
		return ERR_FAILURE;

	// display individual records
	size_t n = 0;
	for(; n < this->db_populated_proxy_validation_test_n; n++)
		this->displayProxyDBValidationTestEntryRecord((P_BP_PROXY_DB_VALIDATION_TEST_ENTRY) &this->db_populated_proxy_validation_tests[n]);

	// return indicating success
	return ERR_SUCCESS;

}



#endif // BP_BLOCKCLASS_INCLUDE_PROXY_DB_BINDINGS

#ifdef BP_BLOCKCLASS_INCLUDE_WEB_ASSESSMENT_BINDINGS

// Initializes the web assessment structure with default values
int Block::CreateWebAssessment(char *targetURL, bool siteLock)
{

	// Null checks
	if (!targetURL || !this->httpSession || !this->htmlExtraction)
	{
		printf("\n [!!] Error, unable to create web assessment. (%p, %p, %p)", targetURL, this->httpSession, this->htmlExtraction);
		return ERR_FAILURE;
	}

	// NOTE: The mysql connection is passed in via this->httpSession, which contains a pointer to this
	//       classes mysql handle.

	// Create a new assessment with SQL tables (large)
	webAssessment = BP_CreateWebAssessment(targetURL, this->httpSession, this->htmlExtraction, true, true, siteLock);
	if (!webAssessment)
	{
		return ERR_FAILURE;
	}

	// Set arbitrary default possibility values (number of checks will not exceed this)
	webAssessment->maxAnchorPossibilities = 65535;
	webAssessment->maxFormPossibilities   = 65535;

	// return success
	return ERR_SUCCESS;

}

// Initializes the web assessment structure with default values
int Block::CreateWebAssessment(char *targetURL, P_BP_HTTP_SESSION httpSession, P_BP_HTML_FULL_EXTRACTION htmlExtraction, bool siteLock)
{

	// Null checks
	if (!targetURL || !httpSession || !htmlExtraction)
		return ERR_FAILURE;

	// Create a new assessment with SQL tables (large)
	webAssessment = BP_CreateWebAssessment(targetURL, httpSession, htmlExtraction, true, true, siteLock);
	if (!webAssessment)
		return ERR_FAILURE;

	// set the webassessment pointer
	this->webAssessment = webAssessment;

	// Set arbitrary default possibility values (number of checks will not exceed this)
	webAssessment->maxAnchorPossibilities = 65535;
	webAssessment->maxFormPossibilities   = 65535;

	// return success
	return ERR_SUCCESS;
}

// Runs the web assessment  (should always use local links only unless
// you want to go to jail.)  Results are stored in the assessment structure
// upon completion of the assessment.  Assessments can take a long time
// based upon the speed of connections, and the responsiveness of the host.
// Timers should be used on threads running assessments that can terminate
// and destroy thread data upon timer limits.  Also, QT for president 2008.
int Block::RunWebAssessment()
{

	// ensure we have a web assessment
	if (!this->webAssessment)
		return ERR_FAILURE;

	// run web assessment and return
	BP_RunWebAssessment(this->webAssessment);


	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Command Line Injection Testing %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// run command injection session
BP_ERROR_T Block::CreateCommandInjectionSession()
{

	// ensure we have a html extraction
	if(!this->htmlExtraction)
	{
		this->AddErrorToStack("Error:  Cannot create command injection session without a valid HTML extraction.");
		return ERR_FAILURE;
	}

	// ensure we have a web assessment
	if(!this->webAssessment)
	{
		this->AddErrorToStack("Error:  Cannot create command injection session without a valid web assessment.");
		return ERR_FAILURE;
	}

	// if we already have an injection session, destroy it first
	if(this->cmd_injection_assessment)
		this->DestroyCommandInjectionSession();

	// attempt to create the new command injection session
	this->cmd_injection_assessment = BP_CommandInjectionGenerateNewSessionFromWebAssessmentAndHTMLExtraction
	(
		this->webAssessment,
		this->htmlExtraction
	);


	// return indicating success
	return ERR_SUCCESS;

}


// Attempt to save results to the database
BP_ERROR_T Block::SaveCommandInjectionResultsToDB()
{

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// ensure we have an assessment
	if(!this->cmd_injection_assessment)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// Must have injection points to log, if not just return a
	// success code, as we have nothing to insert.
	if(!this->cmd_injection_assessment->total_command_injection_points_discovered)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);


	// walk tests looking for results
	size_t n = 0;
	if(!this->cmd_injection_assessment->request_n)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	P_BP_WEB_COMMAND_INJECTION_HTTP_REQUEST curr_request = NULL;

	// walk each request
	for(; n < this->cmd_injection_assessment->request_n; n++)
	{
		curr_request = this->cmd_injection_assessment->all_requests[n];


		if(curr_request->is_file_verification_vuln && curr_request->is_file_verification_vuln)
		{

			char * file_verification_insert_query = NULL;
			P_BP_LINKL tmp_tq = BP_CreateLinkL("tmp", BP_FALSE);

			// set the result id if one is available
			char * result_id   = BP_BufferToMySQLHexString((char *) &this->web_result_id, sizeof(this->web_result_id), tmp_tq);

			// set result type for 1 for file verification
			char * result_type =   "1";
			char * injectable_parameter = NULL;

			if(curr_request->test_ref->is_anchor_test)
			{
				injectable_parameter = BP_BufferToMySQLHexString(curr_request->test_ref->anchor_var->name, bpstrlen(curr_request->test_ref->anchor_var->name), tmp_tq);
			}
			else if(curr_request->test_ref->is_form_test)
			{

				// lookup the input
				P_BP_HTML_INPUT_PROPERTY_LIST input_name = HTMLGetInputPropertyByNameI(curr_request->test_ref->form_input->properties, "name");

				// create parameter
				injectable_parameter = BP_BufferToMySQLHexString(input_name->name, bpstrlen(input_name->name), tmp_tq);

			}

			char * test_type      = "0x1";

			char * test_front     = NULL;
			if(curr_request->file_verification_test_ref->front_part)
				test_front = BP_BufferToMySQLHexString(curr_request->file_verification_test_ref->front_part, bpstrlen(curr_request->file_verification_test_ref->front_part), tmp_tq);
			else
				test_front = "NULL";

			char * back_part      = NULL;
			if(curr_request->file_verification_test_ref->back_part)
				 back_part = BP_BufferToMySQLHexString(curr_request->file_verification_test_ref->back_part, bpstrlen(curr_request->file_verification_test_ref->back_part), tmp_tq);
			else
				back_part = "NULL";


			char * request_get    = NULL;
			if(curr_request->get_str)
				request_get = BP_BufferToMySQLHexString(curr_request->get_str, bpstrlen(curr_request->get_str), tmp_tq);
			else
				request_get = "NULL";

			char * request_post   = NULL;
			if(curr_request->post_str)
				request_post = BP_BufferToMySQLHexString(curr_request->post_str, bpstrlen(curr_request->post_str), tmp_tq);
			else
				request_post = "NULL";

			char * request_cookie = NULL;
			if(curr_request->cookie_str)
				request_cookie = BP_BufferToMySQLHexString(curr_request->cookie_str, bpstrlen(curr_request->cookie_str), tmp_tq);
			else
				request_cookie = "NULL";

			char * request_header = NULL;
			if(curr_request->header_str)
				request_header = BP_BufferToMySQLHexString(curr_request->header_str, bpstrlen(curr_request->header_str), tmp_tq);
			else
				request_header = "NULL";

			// create joiner stack to hold query
			char * joiner_stack[60];
			BP_ZERO_STACK(joiner_stack);


			size_t joiner_idx = 0;
			joiner_stack[joiner_idx] = "INSERT INTO `cmd_injection_points` (`result_id` ,`result_type` ,`injectable_parameter` ,`test_type` ,`test_front` ,	`test_back` ,`verification_get` ,`verification_post` ,`verification_cookie` ,`verification_header` ) VALUES ";
			joiner_idx++;
			joiner_stack[joiner_idx] = "(";
			joiner_idx++;
			joiner_stack[joiner_idx] = result_id;
			joiner_idx++;
			joiner_stack[joiner_idx] = ",";
			joiner_idx++;
			joiner_stack[joiner_idx] = result_type;
			joiner_idx++;
			joiner_stack[joiner_idx] = ",";
			joiner_idx++;
			joiner_stack[joiner_idx] = injectable_parameter;
			joiner_idx++;
			joiner_stack[joiner_idx] = ",";
			joiner_idx++;
			joiner_stack[joiner_idx] = test_type;
			joiner_idx++;
			joiner_stack[joiner_idx] = ",";
			joiner_idx++;
			joiner_stack[joiner_idx] = test_front;
			joiner_idx++;
			joiner_stack[joiner_idx] = ",";
			joiner_idx++;
			joiner_stack[joiner_idx] = back_part;
			joiner_idx++;
			joiner_stack[joiner_idx] = ",";
			joiner_idx++;
			joiner_stack[joiner_idx] = request_get;
			joiner_idx++;
			joiner_stack[joiner_idx] = ",";
			joiner_idx++;
			joiner_stack[joiner_idx] = request_post;
			joiner_idx++;
			joiner_stack[joiner_idx] = ",";
			joiner_idx++;
			joiner_stack[joiner_idx] = request_cookie;
			joiner_idx++;
			joiner_stack[joiner_idx] = ",";
			joiner_idx++;
			joiner_stack[joiner_idx] = request_header;
			joiner_idx++;
			joiner_stack[joiner_idx] = ");";
			joiner_idx++;

			// generate the query
			char * joined_query = BP_JoinStringArray((char ** )&joiner_stack, joiner_idx, tmp_tq);


			this->mysqlPrepareQuery(joined_query);
			this->mysqlExecuteStatement();
			this->mysqlDestroyStatement();

			printf("\n QUERY: %s", joined_query);

			// testroy the tmp
			BP_DestroyLinkL(tmp_tq, BP_TRUE);

		}

		if(curr_request->is_pattern_reflection_vuln && curr_request->is_pattern_reflection_vuln)
		{

			char * file_verification_insert_query = NULL;
			P_BP_LINKL tmp_tq = BP_CreateLinkL("tmp", BP_FALSE);

			// set the result id if one is available
			char * result_id   = BP_BufferToMySQLHexString((char *) &this->web_result_id, sizeof(this->web_result_id), tmp_tq);

			// set result type for 1 for file verification
			char * result_type =   "1";
			char * injectable_parameter = NULL;

			if(curr_request->test_ref->is_anchor_test)
			{
				injectable_parameter = BP_BufferToMySQLHexString(curr_request->test_ref->anchor_var->name, bpstrlen(curr_request->test_ref->anchor_var->name), tmp_tq);
			}
			else if(curr_request->test_ref->is_form_test)
			{

				// lookup the input
				P_BP_HTML_INPUT_PROPERTY_LIST input_name = HTMLGetInputPropertyByNameI(curr_request->test_ref->form_input->properties, "name");

				// create parameter
				injectable_parameter = BP_BufferToMySQLHexString(input_name->name, bpstrlen(input_name->name), tmp_tq);

			}

			char * test_type      = "0x2";

			char * test_front     = NULL;
			if(curr_request->pattern_reflection_test_ref->front_part)
				test_front = BP_BufferToMySQLHexString(curr_request->pattern_reflection_test_ref->front_part, bpstrlen(curr_request->pattern_reflection_test_ref->front_part), tmp_tq);
			else
				test_front = "NULL";

			char * back_part      = NULL;
			if(curr_request->pattern_reflection_test_ref->back_part)
				 back_part = BP_BufferToMySQLHexString(curr_request->pattern_reflection_test_ref->back_part, bpstrlen(curr_request->pattern_reflection_test_ref->back_part), tmp_tq);
			else
				back_part = "NULL";


			char * request_get    = NULL;
			if(curr_request->get_str)
				request_get = BP_BufferToMySQLHexString(curr_request->get_str, bpstrlen(curr_request->get_str), tmp_tq);
			else
				request_get = "NULL";

			char * request_post   = NULL;
			if(curr_request->post_str)
				request_post = BP_BufferToMySQLHexString(curr_request->post_str, bpstrlen(curr_request->post_str), tmp_tq);
			else
				request_post = "NULL";

			char * request_cookie = NULL;
			if(curr_request->cookie_str)
				request_cookie = BP_BufferToMySQLHexString(curr_request->cookie_str, bpstrlen(curr_request->cookie_str), tmp_tq);
			else
				request_cookie = "NULL";

			char * request_header = NULL;
			if(curr_request->header_str)
				request_header = BP_BufferToMySQLHexString(curr_request->header_str, bpstrlen(curr_request->header_str), tmp_tq);
			else
				request_header = "NULL";

			// create joiner stack to hold query
			char * joiner_stack[60];
			BP_ZERO_STACK(joiner_stack);


			size_t joiner_idx = 0;
			joiner_stack[joiner_idx] = "INSERT INTO `cmd_injection_points` (`result_id` ,`result_type` ,`injectable_parameter` ,`test_type` ,`test_front` ,	`test_back` ,`verification_get` ,`verification_post` ,`verification_cookie` ,`verification_header` ) VALUES ";
			joiner_idx++;
			joiner_stack[joiner_idx] = "(";
			joiner_idx++;
			joiner_stack[joiner_idx] = result_id;
			joiner_idx++;
			joiner_stack[joiner_idx] = ",";
			joiner_idx++;
			joiner_stack[joiner_idx] = result_type;
			joiner_idx++;
			joiner_stack[joiner_idx] = ",";
			joiner_idx++;
			joiner_stack[joiner_idx] = injectable_parameter;
			joiner_idx++;
			joiner_stack[joiner_idx] = ",";
			joiner_idx++;
			joiner_stack[joiner_idx] = test_type;
			joiner_idx++;
			joiner_stack[joiner_idx] = ",";
			joiner_idx++;
			joiner_stack[joiner_idx] = test_front;
			joiner_idx++;
			joiner_stack[joiner_idx] = ",";
			joiner_idx++;
			joiner_stack[joiner_idx] = back_part;
			joiner_idx++;
			joiner_stack[joiner_idx] = ",";
			joiner_idx++;
			joiner_stack[joiner_idx] = request_get;
			joiner_idx++;
			joiner_stack[joiner_idx] = ",";
			joiner_idx++;
			joiner_stack[joiner_idx] = request_post;
			joiner_idx++;
			joiner_stack[joiner_idx] = ",";
			joiner_idx++;
			joiner_stack[joiner_idx] = request_cookie;
			joiner_idx++;
			joiner_stack[joiner_idx] = ",";
			joiner_idx++;
			joiner_stack[joiner_idx] = request_header;
			joiner_idx++;
			joiner_stack[joiner_idx] = ");";
			joiner_idx++;

			// generate the query
			char * joined_query = BP_JoinStringArray((char ** )&joiner_stack, joiner_idx, tmp_tq);


			this->mysqlPrepareQuery(joined_query);
			this->mysqlExecuteStatement();
			this->mysqlDestroyStatement();

			// printf("\n QUERY: %s", joined_query);

			// testroy the tmp
			BP_DestroyLinkL(tmp_tq, BP_TRUE);


		}

	}

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// runs the command injection session
BP_ERROR_T Block::RunCommandInjectionSession(size_t thread_count)
{

	// ensure we have a command injection assessment
	if(!this->cmd_injection_assessment)
		return ERR_FAILURE;

	BP_CommandInjectionRunTests(this->cmd_injection_assessment, thread_count);

	// return indicating success
	return ERR_SUCCESS;

}

// destroy the command injection session
BP_ERROR_T Block::DestroyCommandInjectionSession()
{

	// ensure we have a command injection assessment
	if(!this->cmd_injection_assessment)
		return ERR_FAILURE;

	// Destroy the command line injection session
	BP_CommandInjectionDestroySession(this->cmd_injection_assessment);

	// return indicating success
	return ERR_SUCCESS;

}

// Attempts to display the command injection session
BP_ERROR_T Block::DisplayCommandInjectionSession
(
	BP_BOOL display_cmd_injection_test_set,
	BP_BOOL display_anchor_tests,
	BP_BOOL display_form_tests,
	BP_BOOL display_test_elements
)
{

	if(!this->cmd_injection_assessment)
		return ERR_FAILURE;

	// display the command injeciton session
	BP_CommandInjectionDisplaySession(this->cmd_injection_assessment);

	// return indicating success
	return ERR_SUCCESS;

}

// starts a control shell
BP_ERROR_T Block::ControlInteractiveCommandInjectionSession()
{

	// starts a new shell
	BP_CommandInjectionShellInteractive
	(
			this->cmd_injection_assessment
	);

	return ERR_SUCCESS;
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Web Assessment DB Interface (result db) %%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// This function will attempt to save the web assessment stored in the classes local pointer
// structure.
int Block::SaveWebAssessment(size_t result_id)
{

	// check to ensure we have a valid web assessment
	if (!this->webAssessment)
	{
		return ERR_FAILURE;
	}

	// check to ensure we have a valid mysql session
	if (!this->mysqlSession)
	{
		return ERR_FAILURE;
	}

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	/*
	// create new web db class iterface
	BPSC_WebAssessmentDbInterface * audit_dbi = new BPSC_WebAssessmentDbInterface(this);

	// set assessment in class
	audit_dbi->assessment = this->webAssessment;

	// run the assessment storage routine
	audit_dbi->storeAssessment();

	// return indicating success
	return ERR_SUCCESS;
	*/

	// create new garbage collector for this method
	P_BP_GARBAGE_COLLECTOR_DATA gc = (P_BP_GARBAGE_COLLECTOR_DATA) BP_CreateNewGarbageCollector("Save assessment", 1);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%% Old Code Below %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// return indicating success
	// return ERR_SUCCESS;


	// empty marker
	const char *empty = (char *) "empty";
	char * form_action_target = NULL;


	// Save sql injections if they exist
	if (this->webAssessment->sqlResults)
	{
		this->WebAssessmentSaveSQLErrorMessageData();
	}

	// Save xss conditions if they exist
	if (this->webAssessment->xssResults)
	{

		// store xss results in local pointer for iterations
		P_BP_WEB_XSS_TEST_RESULT xssresults = this->webAssessment->xssResults;
		for (
			xssresults = xssresults->first;
			xssresults;
			xssresults = xssresults->next
			)
		{

			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%% Check to make sure record doesn't already exist %%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


			// sql injection prepare
			if (this->mysqlPrepareQuery((char *) "SELECT id FROM xss WHERE result_index = ? and http_get_input = ? and http_post_input =  ?") == ERR_FAILURE) {
				printf("\n Cannot prepare query");
				exit(0);
			}

			// - result_index
			this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &result_id, sizeof(result_id));

			// - http_get_input
			if (xssresults->anchorActualTest)
				this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) xssresults->anchorActualTest, bpstrlen(xssresults->anchorActualTest));
			else
				this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) empty, bpstrlen(empty));

			// - http_post_input
			if (xssresults->formActualTest)
				this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) xssresults->formActualTest, bpstrlen(xssresults->formActualTest));
			else
				this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) empty, bpstrlen(empty));

			// bind parameters
			this->mysqlBindParameters();

			// add result
			size_t result_index = 0;

			// bind result parameter
			this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &result_index, sizeof(result_index));

			// bind the results
			this->mysqlBindResults();

			// execute statement
			if (this->mysqlExecuteStatement() != ERR_SUCCESS) {
				// printf("\n Cannot execute statement");
				exit(0);
			}

			// store result
			this->mysqlStoreResults();

			// fetch the result into the result_index var
			this->mysqlFetchResult();

			// ADDED DURING LEAK CHECK?: destroy the statement at this point
			this->mysqlDestroyStatement();

			// continue the loop
			if (result_index)
				continue;

			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%% Insert SQL Injection (Validated non-duplicate) %%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


			// db place holder
			char *empty = (char *) "empty";

			// sql injection prepare
			this->mysqlPrepareQuery((char *) "INSERT INTO xss ( result_index, http_get_input, http_post_input) VALUES ( ?, ?, ? );");

			// - result_index
			this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &result_id, sizeof(result_id));

			// - http_get_input
			if (xssresults->anchorActualTest)
				this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) xssresults->anchorActualTest, bpstrlen(xssresults->anchorActualTest));
			else
			{

				// set href from form action in the case that its from a form
				P_BP_HTML_FORM_PROPERTY_LIST proplist = BP_GetVulnerableFormActionPropertyXSS(xssresults);
				if(!proplist)
					this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) empty, bpstrlen(empty));
				else
					this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) proplist->content, proplist->cLen);

			}

			// - http_post_input
			if (xssresults->formActualTest)
				this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) xssresults->formActualTest, bpstrlen(xssresults->formActualTest));
			else
				this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) empty, bpstrlen(empty));

			// bind parameters
			this->mysqlBindParameters();

			// execute statement
			this->mysqlExecuteStatement();

			// store result
			this->mysqlStoreResults();

			// fetch the result into the result_index var
			this->mysqlFetchResult();

			/* COMMENT OUT START
END COMMMENT*/

			// leak close possibly
			this->mysqlDestroyStatement();


		}

	}


	// now insert blind sql injections
	size_t j = 0;
	if(this->webAssessment->bsqli_results)
	for(; j < this->webAssessment->bsqli_result_count; j++)
	{


		// prepare the selection verification query
		this->mysqlPrepareQuery((char *) "SELECT count(*) FROM `sqli` WHERE `result_id` = ? AND `request_log_id` = ? AND  `http_get` = ? AND `http_post` = ? AND `http_cookie` = ?");

		// `result_id`
		this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &this->webAssessment->bsqli_results[j].db_result_id, sizeof(size_t));

		// `request_log_id`
		this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &this->webAssessment->bsqli_results[j].db_request_log_id, sizeof(size_t));

		// `http_get`
		if(this->webAssessment->bsqli_results[j].http_get)
			this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) this->webAssessment->bsqli_results[j].http_get, bpstrlen(this->webAssessment->bsqli_results[j].http_get));
		else
			this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) empty, bpstrlen(empty));

		// `http_post`
		if(this->webAssessment->bsqli_results[j].http_post)
			this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) this->webAssessment->bsqli_results[j].http_post, bpstrlen(this->webAssessment->bsqli_results[j].http_post));
		else
			this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) empty, bpstrlen(empty));

		// `http_cookie`
		if(this->webAssessment->bsqli_results[j].http_cookie)
			this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) this->webAssessment->bsqli_results[j].http_cookie, bpstrlen(this->webAssessment->bsqli_results[j].http_cookie));
		else
			this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) empty, bpstrlen(empty));


		// add result
		size_t result_cnt = 0;

		// bind result parameter
		this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &result_cnt, sizeof(result_cnt));



		// bind the results
		this->mysqlBindResults();

		// bind parameters
		this->mysqlBindParameters();

		// execute statement
		this->mysqlExecuteStatement();

		// fetch the result
		this->mysqlFetchResult();

		// ensure we have a result count
		if(result_cnt)
		{

			// destroy the statement and continue loop if record is matched
			this->mysqlDestroyStatement();
			continue;

		}

		// ==================================================
		// next insert the query if it doesn't already exist

		// db place holder
		char *empty = (char *) "empty";

		// sql injection prepare
		this->mysqlPrepareQuery((char *) "INSERT INTO `sqli` (`id`, `result_id`, `request_log_id`, `request_time_to_complete`, `http_get`, `http_post`, `http_cookie`) VALUES (NULL, ?, ?, ?, ?, ?, ?);");


		// `result_id`
		this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &this->webAssessment->bsqli_results[j].db_result_id, sizeof(size_t));

		// `request_log_id`
		this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &this->webAssessment->bsqli_results[j].db_request_log_id, sizeof(size_t));

		// `request time to complete``
		this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &this->webAssessment->bsqli_results[j].actual_request_time, sizeof(size_t));

		// `http_get`
		if(this->webAssessment->bsqli_results[j].http_get)
			this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) this->webAssessment->bsqli_results[j].http_get, bpstrlen(this->webAssessment->bsqli_results[j].http_get));
		else
			this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) empty, bpstrlen(empty));

		// `http_post`
		if(this->webAssessment->bsqli_results[j].http_post)
			this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) this->webAssessment->bsqli_results[j].http_post, bpstrlen(this->webAssessment->bsqli_results[j].http_post));
		else
			this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) empty, bpstrlen(empty));

		// `http_cookie`
		if(this->webAssessment->bsqli_results[j].http_cookie)
			this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) this->webAssessment->bsqli_results[j].http_cookie, bpstrlen(this->webAssessment->bsqli_results[j].http_cookie));
		else
			this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) empty, bpstrlen(empty));

		// bind parameters
		this->mysqlBindParameters();

		// execute statement
		this->mysqlExecuteStatement();

	}

	// insert all application engine matches
	if(this->webAssessment->appErrorResults)
	{

		// save the application engine data
		this->WebAssessmentSaveAppEngineData();

	}

	// bpfree all the chunks on loop
	BP_GarbageCollectorFreeAllChunks(gc);
	BP_DestroyGarbageCollector(gc);
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}


// Saves the SQL error message data to the database.
BP_ERROR_T Block::WebAssessmentSaveSQLErrorMessageData()
{

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// check pointers before entering save logic
	if(!this->webAssessment)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	if(!this->webAssessment->sqlResults)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// set iterative result pointer
	P_BP_WEB_SQLINJ_TEST_RESULT sql_result = this->webAssessment->sqlResults->first;

	// set index iterator
	size_t index_iterator = 0;

	// walk the sql results
	for(; sql_result; sql_result = sql_result->next)
	{


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% MySQL Match Insert %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		for
		(
				index_iterator = 0;
				index_iterator < sql_result->mysqlTestMatchesMade;
				index_iterator++
		)
		{

			this->WebAssessmentAddEngineAnalysisDataEntry
			(
					sql_result->request_data.result_id,
					sql_result->request_data.request_id,
					sql_result->request_data.http_status_code,
					(char *) &sql_result->request_data.http_get,
					(char *) &sql_result->request_data.http_post,
					(char *) &sql_result->request_data.http_cookie,
					(char *) &sql_result->request_data.http_header,
					(char *) &sql_result->request_data.suspected_get_var,
					(char *) &sql_result->request_data.suspected_post_var,
					(char *) &sql_result->request_data.suspected_cookie_var,
					(char *) &sql_result->request_data.suspected_header_var,
					BP_WEB_ANALYSIS_ENGINES_MYSQL,
					BP_WEB_ANALYSIS_CONDITION_SQL_MSG,
					(char *) this->webAssessment->mysqlErrCodes[sql_result->mysqlTestMatchIndexes[index_iterator]].mysqlErrStr,
					(char *) this->webAssessment->mysqlErrCodes[sql_result->mysqlTestMatchIndexes[index_iterator]].mysqlErrStr,
					sql_result->mysqlTestMatchIndexes[index_iterator]
			);

		}


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Microsoft SQL Match Insert %%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		for
		(
				index_iterator = 0;
				index_iterator < sql_result->msqlTestMatchesMade;
				index_iterator++
		)
		{

			this->WebAssessmentAddEngineAnalysisDataEntry
			(
					sql_result->request_data.result_id,
					sql_result->request_data.request_id,
					sql_result->request_data.http_status_code,
					(char *) &sql_result->request_data.http_get,
					(char *) &sql_result->request_data.http_post,
					(char *) &sql_result->request_data.http_cookie,
					(char *) &sql_result->request_data.http_header,
					(char *) &sql_result->request_data.suspected_get_var,
					(char *) &sql_result->request_data.suspected_post_var,
					(char *) &sql_result->request_data.suspected_cookie_var,
					(char *) &sql_result->request_data.suspected_header_var,
					BP_WEB_ANALYSIS_ENGINES_MSQL,
					BP_WEB_ANALYSIS_CONDITION_SQL_MSG,
					(char *) this->webAssessment->msqlErrCodes[sql_result->msqlTestMatchIndexes[index_iterator]].description,
					(char *) this->webAssessment->msqlErrCodes[sql_result->msqlTestMatchIndexes[index_iterator]].description,
					sql_result->msqlTestMatchIndexes[index_iterator]
			);

		}

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Oracle SQL Match Insert %%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		for
		(
				index_iterator = 0;
				index_iterator < sql_result->oracleTestMatchesMade;
				index_iterator++
		)
		{

			this->WebAssessmentAddEngineAnalysisDataEntry
			(
					sql_result->request_data.result_id,
					sql_result->request_data.request_id,
					sql_result->request_data.http_status_code,
					(char *) &sql_result->request_data.http_get,
					(char *) &sql_result->request_data.http_post,
					(char *) &sql_result->request_data.http_cookie,
					(char *) &sql_result->request_data.http_header,
					(char *) &sql_result->request_data.suspected_get_var,
					(char *) &sql_result->request_data.suspected_post_var,
					(char *) &sql_result->request_data.suspected_cookie_var,
					(char *) &sql_result->request_data.suspected_header_var,
					BP_WEB_ANALYSIS_ENGINES_ORACLE,
					BP_WEB_ANALYSIS_CONDITION_SQL_MSG,
					(char *) this->webAssessment->oracleErrCodes[sql_result->oracleTestMatchIndexes[index_iterator]].description,
					(char *) this->webAssessment->oracleErrCodes[sql_result->oracleTestMatchIndexes[index_iterator]].description,
					sql_result->oracleTestMatchIndexes[index_iterator]
			);

		}


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% ODBC SQL Match Insert %%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		for
		(
				index_iterator = 0;
				index_iterator < sql_result->odbcTestMatchesMade;
				index_iterator++
		)
		{

			this->WebAssessmentAddEngineAnalysisDataEntry
			(
					sql_result->request_data.result_id,
					sql_result->request_data.request_id,
					sql_result->request_data.http_status_code,
					(char *) &sql_result->request_data.http_get,
					(char *) &sql_result->request_data.http_post,
					(char *) &sql_result->request_data.http_cookie,
					(char *) &sql_result->request_data.http_header,
					(char *) &sql_result->request_data.suspected_get_var,
					(char *) &sql_result->request_data.suspected_post_var,
					(char *) &sql_result->request_data.suspected_cookie_var,
					(char *) &sql_result->request_data.suspected_header_var,
					BP_WEB_ANALYSIS_ENGINES_ODBC,
					BP_WEB_ANALYSIS_CONDITION_SQL_MSG,
					(char *) this->webAssessment->odbcErrCodes[sql_result->odbcTestMatchIndexes[index_iterator]].description,
					(char *) this->webAssessment->odbcErrCodes[sql_result->odbcTestMatchIndexes[index_iterator]].description,
					sql_result->odbcTestMatchIndexes[index_iterator]
			);

		}

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Postgres SQL Match Insert %%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		for
		(
				index_iterator = 0;
				index_iterator < sql_result->postgresTestMatchesMade;
				index_iterator++
		)
		{

			this->WebAssessmentAddEngineAnalysisDataEntry
			(
					sql_result->request_data.result_id,
					sql_result->request_data.request_id,
					sql_result->request_data.http_status_code,
					(char *) &sql_result->request_data.http_get,
					(char *) &sql_result->request_data.http_post,
					(char *) &sql_result->request_data.http_cookie,
					(char *) &sql_result->request_data.http_header,
					(char *) &sql_result->request_data.suspected_get_var,
					(char *) &sql_result->request_data.suspected_post_var,
					(char *) &sql_result->request_data.suspected_cookie_var,
					(char *) &sql_result->request_data.suspected_header_var,
					BP_WEB_ANALYSIS_ENGINES_POSTGRES,
					BP_WEB_ANALYSIS_CONDITION_SQL_MSG,
					(char *) this->webAssessment->postgresErrCodes[sql_result->postgresTestMatchIndexes[index_iterator]].postgresErrStr,
					(char *) this->webAssessment->postgresErrCodes[sql_result->postgresTestMatchIndexes[index_iterator]].postgresErrStr,
					sql_result->postgresTestMatchIndexes[index_iterator]
			);

		}

	}

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// Saves the XSS data to the database
BP_ERROR_T Block::WebAssessmentSaveXSSData()
{

	// return indicating success
	return ERR_SUCCESS;

}

// saves app engine data to the database
BP_ERROR_T Block::WebAssessmentSaveAppEngineData()
{

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;


	// insert all application engine matches
	if(this->webAssessment->appErrorResults)
	{

		// this index iterator is used to move between the different arrays
		size_t index_iterator = 0;

		P_BP_WEB_APPSERVER_TEST_RESULT app_result = this->webAssessment->appErrorResults->first;
		for
		(
				;
				app_result;
				app_result = app_result->next
		)
		{

			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% PHP Match Insert %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			for
			(
					index_iterator = 0;
					index_iterator < app_result->phpTestMatchesMade;
					index_iterator++
			)
			{

				this->WebAssessmentAddEngineAnalysisDataEntry
				(
						app_result->request_data.result_id,
						app_result->request_data.request_id,
						app_result->request_data.http_status_code,
						(char *) &app_result->request_data.http_get,
						(char *) &app_result->request_data.http_post,
						(char *) &app_result->request_data.http_cookie,
						(char *) &app_result->request_data.http_header,
						(char *) &app_result->request_data.suspected_get_var,
						(char *) &app_result->request_data.suspected_post_var,
						(char *) &app_result->request_data.suspected_cookie_var,
						(char *) &app_result->request_data.suspected_header_var,
						BP_WEB_ANALYSIS_ENGINES_PHP,
						BP_WEB_ANALYSIS_CONDITION_APP_ENGINE_MSG,
						(char *) this->webAssessment->phpErrCodes[app_result->phpTestMatchIndexes[index_iterator]].phpErrStr,
						(char *) this->webAssessment->phpErrCodes[app_result->phpTestMatchIndexes[index_iterator]].phpErrStr,
						app_result->phpTestMatchIndexes[index_iterator]
				);

			}

			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% ASP Match Insert %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			for
			(
					index_iterator = 0;
					index_iterator < app_result->aspTestMatchesMade;
					index_iterator++
			)
			{

				// add the entry
				this->WebAssessmentAddEngineAnalysisDataEntry
				(
						app_result->request_data.result_id,
						app_result->request_data.request_id,
						app_result->request_data.http_status_code,
						(char *) &app_result->request_data.http_get,
						(char *) &app_result->request_data.http_post,
						(char *) &app_result->request_data.http_cookie,
						(char *) &app_result->request_data.http_header,
						(char *) &app_result->request_data.suspected_get_var,
						(char *) &app_result->request_data.suspected_post_var,
						(char *) &app_result->request_data.suspected_cookie_var,
						(char *) &app_result->request_data.suspected_header_var,
						BP_WEB_ANALYSIS_ENGINES_ASP,
						BP_WEB_ANALYSIS_CONDITION_APP_ENGINE_MSG,
						(char *) this->webAssessment->aspErrCodes[app_result->aspTestMatchIndexes[index_iterator]].description,
						(char *) this->webAssessment->aspErrCodes[app_result->aspTestMatchIndexes[index_iterator]].description,
						app_result->aspTestMatchIndexes[index_iterator]
				);

			}

			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% JSP Match Insert %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			for
			(
					index_iterator = 0;
					index_iterator < app_result->jspTestMatchesMade;
					index_iterator++
			)
			{

				// add the entry
				this->WebAssessmentAddEngineAnalysisDataEntry
				(
						app_result->request_data.result_id,
						app_result->request_data.request_id,
						app_result->request_data.http_status_code,
						(char *) &app_result->request_data.http_get,
						(char *) &app_result->request_data.http_post,
						(char *) &app_result->request_data.http_cookie,
						(char *) &app_result->request_data.http_header,
						(char *) &app_result->request_data.suspected_get_var,
						(char *) &app_result->request_data.suspected_post_var,
						(char *) &app_result->request_data.suspected_cookie_var,
						(char *) &app_result->request_data.suspected_header_var,
						BP_WEB_ANALYSIS_ENGINES_JSP,
						BP_WEB_ANALYSIS_CONDITION_APP_ENGINE_MSG,
						(char *) this->webAssessment->jspErrCodes[app_result->jspTestMatchIndexes[index_iterator]].description,
						(char *) this->webAssessment->jspErrCodes[app_result->jspTestMatchIndexes[index_iterator]].description,
						app_result->jspTestMatchIndexes[index_iterator]
				);

			}

			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Cold Fusion Match Insert %%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			for
			(
					index_iterator = 0;
					index_iterator < app_result->cfmTestMatchesMade;
					index_iterator++
			)
			{

				// add the entry
				this->WebAssessmentAddEngineAnalysisDataEntry
				(
						app_result->request_data.result_id,
						app_result->request_data.request_id,
						app_result->request_data.http_status_code,
						(char *) &app_result->request_data.http_get,
						(char *) &app_result->request_data.http_post,
						(char *) &app_result->request_data.http_cookie,
						(char *) &app_result->request_data.http_header,
						(char *) &app_result->request_data.suspected_get_var,
						(char *) &app_result->request_data.suspected_post_var,
						(char *) &app_result->request_data.suspected_cookie_var,
						(char *) &app_result->request_data.suspected_header_var,
						BP_WEB_ANALYSIS_ENGINES_CFM,
						BP_WEB_ANALYSIS_CONDITION_APP_ENGINE_MSG,
						(char *) this->webAssessment->cfmErrCodes[app_result->cfmTestMatchIndexes[index_iterator]].description,
						(char *) this->webAssessment->cfmErrCodes[app_result->cfmTestMatchIndexes[index_iterator]].description,
						app_result->cfmTestMatchIndexes[index_iterator]
				);

			}

			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Dot Net Match Insert %%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			for
			(
					index_iterator = 0;
					index_iterator < app_result->dnetTestMatchesMade;
					index_iterator++
			)
			{

				// add the entry
				this->WebAssessmentAddEngineAnalysisDataEntry
				(
						app_result->request_data.result_id,
						app_result->request_data.request_id,
						app_result->request_data.http_status_code,
						(char *) &app_result->request_data.http_get,
						(char *) &app_result->request_data.http_post,
						(char *) &app_result->request_data.http_cookie,
						(char *) &app_result->request_data.http_header,
						(char *) &app_result->request_data.suspected_get_var,
						(char *) &app_result->request_data.suspected_post_var,
						(char *) &app_result->request_data.suspected_cookie_var,
						(char *) &app_result->request_data.suspected_header_var,
						BP_WEB_ANALYSIS_ENGINES_DNET,
						BP_WEB_ANALYSIS_CONDITION_APP_ENGINE_MSG,
						(char *) this->webAssessment->dnetErrCodes[app_result->dnetTestMatchIndexes[index_iterator]].exceptionStr,
						(char *) this->webAssessment->dnetErrCodes[app_result->dnetTestMatchIndexes[index_iterator]].exceptionStr,
						app_result->dnetTestMatchIndexes[index_iterator]
				);

			}

		}

	}

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}


// Adds an entry to the engine_analysis_data table.
BP_ERROR_T Block::WebAssessmentAddEngineAnalysisDataEntry
(
	size_t                      result_id,
	size_t                      request_id,
	size_t                      http_status_code,
	char *                      http_get,
	char *                      http_post,
	char *                      http_cookie,
	char *                      http_header,
	char *                      suspected_get_var,
	char *                      suspected_post_var,
	char *                      suspected_cookie_var,
	char *                      suspected_header_var,
	BP_WEB_ANALYSIS_ENGINE_T    detection_engine,
	BP_WEB_ANALYSIS_CONDITION_T condition,
	char *                      detection_string_test,
	char *                      detection_string_found,
	size_t                      detection_table_index
)
{

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;



	// create a temporary allocator for tracking heap chunk
	P_BP_LINKL tmp_tq = BP_CreateLinkL("tmp", BP_FALSE);

	// convert all parameters into escaped mysql hex strings, store them
	// all in the tmp tailqueue for easy cleanup.
	char * result_id_s              = BP_SizeTToMySQLBuffer    (result_id,                       tmp_tq);
	char * request_id_s             = BP_SizeTToMySQLBuffer    (request_id,                      tmp_tq);
	char * http_status_code_s       = BP_SizeTToMySQLBuffer    (http_status_code,                tmp_tq);
	char * http_get_s               = BP_BufferToMySQLHexString((char *) http_get,               bpstrlen(http_get),               tmp_tq);
	char * http_post_s              = BP_BufferToMySQLHexString((char *) http_post,              bpstrlen(http_post),              tmp_tq);
	char * http_cookie_s            = BP_BufferToMySQLHexString((char *) http_cookie,            bpstrlen(http_cookie),              tmp_tq);
	char * http_header_s            = BP_BufferToMySQLHexString((char *) http_header,            bpstrlen(http_header),            tmp_tq);
	char * suspected_get_var_s      = BP_BufferToMySQLHexString((char *) suspected_get_var,      bpstrlen(suspected_get_var),      tmp_tq);
	char * suspected_post_var_s     = BP_BufferToMySQLHexString((char *) suspected_post_var,     bpstrlen(suspected_post_var),     tmp_tq);
	char * suspected_cookie_var_s   = BP_BufferToMySQLHexString((char *) suspected_cookie_var,   bpstrlen(suspected_cookie_var),   tmp_tq);
	char * suspected_header_var_s   = BP_BufferToMySQLHexString((char *) suspected_header_var,   bpstrlen(suspected_header_var),   tmp_tq);
	char * detection_engine_s       = BP_SizeTToMySQLBuffer    (detection_engine,                 tmp_tq);
	char * detection_string_test_s  = BP_BufferToMySQLHexString((char *) detection_string_test,  bpstrlen(detection_string_test),  tmp_tq);
	char * detection_string_found_s = BP_BufferToMySQLHexString((char *) detection_string_found, bpstrlen(detection_string_found), tmp_tq);
	char * detection_table_index_s  = BP_SizeTToMySQLBuffer    (detection_table_index,           tmp_tq);
	char * condition_s              = BP_SizeTToMySQLBuffer    (condition,                       tmp_tq);


	// create a joinable array
	char * add_indexes[80];
	BP_ZERO_STACK(add_indexes);

	// create iterator for moving index positions
	size_t x = 0;

	add_indexes[x] = "INSERT INTO `engine_analysis_data` " \
			         "( "                                  \
			         "  `result_id`, "\
			         "  `request_id`, "\
			         "  `http_status_code`, " \
			         "  `http_get`, "\
			         "  `http_post`, "\
			         "  `http_cookie`, "\
			         "  `http_header`, "
			         "  `suspected_get_var`, "
			         "  `suspected_post_var`, " \
			         "  `suspected_cookie_var`, "\
			         "  `suspected_header_var`, "\
			         "  `detection_engine`, "\
			         "  `detection_string_test`, "\
			         "  `detection_string_found`, "\
			         "  `detection_table_index`, "\
			         "  `condition_type`, "\
			         "  `commit_ts`) "\
			         "  VALUES ( ";
	// move past front par
	x++;

	add_indexes[x] = result_id_s;               x++;
	add_indexes[x] = ",";                       x++;
	add_indexes[x] = request_id_s;              x++;
	add_indexes[x] = ",";                       x++;
	add_indexes[x] = http_status_code_s;        x++;
	add_indexes[x] = ",";                       x++;
	add_indexes[x] = http_get_s;                x++;
	add_indexes[x] = ",";                       x++;
	add_indexes[x] = http_post_s;               x++;
	add_indexes[x] = ",";                       x++;
	add_indexes[x] = http_cookie_s;             x++;
	add_indexes[x] = ",";                       x++;
	add_indexes[x] = http_header_s;             x++;
	add_indexes[x] = ",";                       x++;
	add_indexes[x] = suspected_get_var_s;       x++;
	add_indexes[x] = ",";                       x++;
	add_indexes[x] = suspected_post_var_s;      x++;
	add_indexes[x] = ",";                       x++;
	add_indexes[x] = suspected_cookie_var_s;    x++;
	add_indexes[x] = ",";                       x++;
	add_indexes[x] = suspected_header_var_s;    x++;
	add_indexes[x] = ",";                       x++;
	add_indexes[x] = detection_engine_s;        x++;
	add_indexes[x] = ",";                       x++;
	add_indexes[x] = detection_string_test_s;   x++;
	add_indexes[x] = ",";                       x++;
	add_indexes[x] = detection_string_found_s;  x++;
	add_indexes[x] = ",";                       x++;
	add_indexes[x] = detection_table_index_s;   x++;
	add_indexes[x] = ",";                       x++;
	add_indexes[x] = condition_s;               x++;
	add_indexes[x] = ",";                       x++;
	add_indexes[x] = "NOW()";                   x++;
	add_indexes[x] = "); ";                     x++;

	// create the query
	char * query = BP_JoinStringArray((char **) &add_indexes, x, tmp_tq);

	// printf("\n Getting here?: %s - %s", query, detection_string_test);
	// prepare query
	if(this->mysqlPrepareQuery(query) != ERR_SUCCESS)
	{
		BP_DestroyLinkL(tmp_tq, BP_TRUE);
		return ERR_FAILURE;
	}


	// execute query
	if(this->mysqlExecuteStatement() != ERR_SUCCESS)
	{
		this->mysqlDestroyStatement();
		BP_DestroyLinkL(tmp_tq, BP_TRUE);
		return ERR_FAILURE;
	}

	// destroy the statement if we got this far
	this->mysqlDestroyStatement();

	// destroy all registered memory
	BP_DestroyLinkL(tmp_tq, BP_TRUE);

	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%% Set Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Set XSS Checks Only
int Block::SetXSSChecks(P_BP_WEB_XSS_TEST xssChecks)
{

	if (!xssChecks)
		return ERR_FAILURE;

	if(!this->webAssessment)
		return ERR_FAILURE;

	// count the tests (always null structure terminated)
	size_t n = 0;
	for(; xssChecks[n].testFront; n++){}

	// set the assessment pointer
	webAssessment->xssChecks      = xssChecks;
	webAssessment->xssChecksCount = n;

	// return indicating success
	return ERR_SUCCESS;

}

// Set SQL Checks Only
int Block::SetSQLINJChecks(P_BP_WEB_SQLINJ_TEST sqlChecks)
{

	if (!sqlChecks)
		return ERR_FAILURE;

	if(!this->webAssessment)
		return ERR_FAILURE;

	if (!this->webAssessment)
	{
		this->AddErrorToStack((char*) "Error in SetSQLINJChecks: No web assessment exists.");
		return ERR_FAILURE;
	}

	// array is always terminated with a null pointer, so
	// count array members here.
	size_t n = 0;
	for(; sqlChecks[n].testFront; n++){}

	// set the sql check pointer
	webAssessment->sqlChecks     = sqlChecks;
	webAssessment->sqlCheckCount = n;

	return ERR_SUCCESS;
}

// Loads SQL injection checks from a newline delimited file.
int Block::LoadSQLINJChecksFromFile(char *sqlinj_file)
{

	// check that we have a web assessment
	if(!this->webAssessment)
		return ERR_FAILURE;

	// destroy sqli checks if they already exist
	if(this->webAssessment->sqlChecks)
	{

		BP_DestroySQLTests(this->webAssessment->sqlChecks, this->webAssessment->sqlCheckCount);
		this->webAssessment->sqlChecks = NULL;
		this->webAssessment->sqlCheckCount = 0;

	}

	// retrieve file contents
	Block * file_data = new Block();

	// retrieve file contents
	file_data->FileGetContents(sqlinj_file);

	// check to make sure we got content
	if(!file_data->got_contents_size)
	{
		this->AddErrorToStack((char*) "Error in LoadSQLINJChecksFromFile(): No data in file. (NULL)");
		return ERR_FAILURE;
	}
	if(!strnlen(file_data->got_contents, 2048))
	{
		this->AddErrorToStack((char*) "Error in LoadSQLINJChecksFromFile(): No data in file.");
		return ERR_FAILURE;
	}

	// str utils class for exploding data
	BP_StringUtils * exploder = new BP_StringUtils(file_data->got_contents);

	// explode along newlines
	exploder->Explode("\n");

	// allocate space for elements
	P_BP_WEB_SQLINJ_TEST elements = (P_BP_WEB_SQLINJ_TEST) bpcalloc(sizeof(BP_WEB_SQLINJ_TEST) * (exploder->exploded_count+1) , 1);

	// walk the exploded item list and add checks here
	size_t i = 0;
	for(; i < exploder->exploded_count-1; i++)
	{

		// static test front
		elements[i].testFront = strndup(exploder->exploded[i], 2048);

		// static test end (space, can't be null)
		elements[i].testTail = strdup(" ");

		// set the type to sqlinjection
		elements[i].type = BP_WEB_EXPLOIT_TYPE_SQLINJ;

		// set level to unsafe
		elements[i].attackLevel = BP_SQLINJ_ATTACK_UNSAFE;

		// change policy to NO POLICY
		elements[i].policy = BP_SQLINJ_NONE;

	}

	if(this->webAssessment)
	{
		this->webAssessment->sqlChecks = elements;
		this->webAssessment->sqlCheckCount = i;
	}


	// close file
	if(file_data->file)
		file_data->File_Close();

	// destroy file data object
	delete file_data;

	// destroy the exploded data
	delete exploder;

	// return indicating success
	return ERR_SUCCESS;
}

// Loads XSS checks from a newline delimited file.
int Block::LoadXSSChecksFromFile(char *xss_file)
{

	if(!this->webAssessment)
		return ERR_FAILURE;

	// destroy xss checks if they already exist
	if(this->webAssessment->xssChecks)
	{

		// printf("\n Destroying here?: %p, %u", this->webAssessment->xssChecks, this->webAssessment->xssChecksCount);
		BP_DestroyXSSTests(this->webAssessment->xssChecks, this->webAssessment->xssChecksCount);
		this->webAssessment->xssChecks = NULL;

	}

	// retrieve file contents
	Block * file_data = new Block();

	// retrieve file contents
	file_data->FileGetContents(xss_file);

	// check to make sure we got content
	if(!file_data->got_contents_size)
	{
		this->AddErrorToStack((char*) "Error in LoadXSSChecksFromFile(): No data in file. (NULL)");
		return ERR_FAILURE;
	}
	if(!strnlen(file_data->got_contents, 2048))
	{
		this->AddErrorToStack((char*) "Error in LoadXSSChecksFromFile(): No data in file.");
		return ERR_FAILURE;
	}

	// str utils class for exploding data
	BP_StringUtils * exploder = new BP_StringUtils(file_data->got_contents);

	// explode along newlines
	exploder->Explode("\n");

	// allocate space for elements
	P_BP_WEB_XSS_TEST elements = (P_BP_WEB_XSS_TEST) bpcalloc(sizeof(BP_WEB_XSS_TEST) * (exploder->exploded_count+1) , 1);

	// walk the exploded item list and add checks here
	size_t i = 0;
	for(; i < exploder->exploded_count-1; i++)
	{

		// static test front
		elements[i].testFront = bpstrndup(exploder->exploded[i], 2048);

		// static test end (space, can't be null)
		elements[i].testTail = bpstrdup(" ");

		// set the type to sqlinjection
		elements[i].type = BP_WEB_EXPLOIT_TYPE_XSS;

		// set level to unsafe
		elements[i].attackLevel = BP_XSS_ATTACK_UNSAFE;

		// change policy to NO POLICY
		elements[i].policy = BP_XSS_NONE;

	}

	if(this->webAssessment)
	{
		this->webAssessment->xssChecks      = elements;
		this->webAssessment->xssChecksCount = i;
	}


	// close file
	if(file_data->file)
		file_data->File_Close();

	// destroy file data object
	delete file_data;

	// destroy exploder object
	delete exploder;

	// return indicating success
	return ERR_SUCCESS;

}

// Utility: Display all web assessment check lists. (sqlinj/xss/etc)
int Block::DisplayWebAssessementChecks()
{

	if(!this->webAssessment)
	{
		printf("\n [!!] Cannot dislplay empty web assessment check data (no checks!).");
		return ERR_FAILURE;
	}

	// set initial poiters
	P_BP_WEB_XSS_TEST    xss_tests = this->webAssessment->xssChecks;
	P_BP_WEB_SQLINJ_TEST sqli_tests = this->webAssessment->sqlChecks;

	// dummy test for identifing the end of lists
	BP_WEB_XSS_TEST    dummy_xss_test  = {0};
	BP_WEB_SQLINJ_TEST dummy_sqli_test = {0};

	size_t i = 0;
	if(xss_tests)
	{

		// display xss tests
		for
		(
			i = 0;
			memcmp(&dummy_xss_test, &xss_tests[i], sizeof(dummy_xss_test)) != 0;
			i++
		)
		{
				printf("\n [+] Xss Test: %p", &xss_tests[i]);
				printf("\n\t front:      %s", xss_tests[i].testFront);
				printf("\n\t tail:       %s", xss_tests[i].testTail);
				printf("\n\t type:       %u", xss_tests[i].type);
				printf("\n\t policy:     %u", xss_tests[i].policy);
				printf("\n\t attack lvl: %u", xss_tests[i].attackLevel);

		}

	}


	i = 0;
	if(sqli_tests)
	{

		// display xss tests
		for
		(
			i = 0;
			memcmp(&dummy_sqli_test, &sqli_tests[i], sizeof(dummy_sqli_test)) != 0;
			i++
		)
		{
				printf("\n [+] Sqli Test: %p", &sqli_tests[i]);
				printf("\n\t front:      %s", sqli_tests[i].testFront);
				printf("\n\t tail:       %s", sqli_tests[i].testTail);
				printf("\n\t type:       %u", sqli_tests[i].type);
				printf("\n\t policy:     %u", sqli_tests[i].policy);
				printf("\n\t attack lvl: %u", sqli_tests[i].attackLevel);

		}

	}

	return ERR_SUCCESS;
}

// Set All Checks (for simplicity)
int Block::SetAllWebAssessmentChecksDefault(bool useDummy)
{

	// ensure a webassessment exists
	if (!this->webAssessment)
	{
		this->AddErrorToStack((char*) "Error in SetAllWebAssessmentChecksDefault: No web assessment exists.");
		return ERR_FAILURE;
	}

	// set real tests if not dummy
	if (!useDummy)
	{

		if(this->webAssessment->sqlChecks)
		{
			BP_DestroySQLTests(this->webAssessment->sqlChecks, this->webAssessment->sqlCheckCount);
			this->webAssessment->sqlChecks     = NULL;
			this->webAssessment->sqlCheckCount = 0;
		}

		if(this->webAssessment->xssChecks)
		{
			BP_DestroyXSSTests(this->webAssessment->xssChecks, this->webAssessment->xssChecksCount);
			this->webAssessment->xssChecks     = NULL;
			this->webAssessment->xssChecksCount = 0;
		}

		// set the non-dummy checks for SQL
		if (this->SetSQLINJChecks(BP_SQLGenerateRandomArray()) != ERR_SUCCESS)
			return ERR_FAILURE;

		// set the non-dummy checks for XSS
		if (this->SetXSSChecks(BP_XSSGenerateRandomArray()) != ERR_SUCCESS)
			return ERR_FAILURE;

		// If not real tests, then dummy tests
	}
	else
	{

		if(this->webAssessment->sqlChecks)
		{
			BP_DestroySQLTests(this->webAssessment->sqlChecks, this->webAssessment->sqlCheckCount);
			this->webAssessment->sqlChecks     = NULL;
			this->webAssessment->sqlCheckCount = 0;
		}

		if(this->webAssessment->xssChecks)
		{
			BP_DestroyXSSTests(this->webAssessment->xssChecks, this->webAssessment->xssChecksCount);
			this->webAssessment->xssChecks     = NULL;
			this->webAssessment->xssChecksCount = 0;
		}

		// set the non-dummy checks for SQL
		if (this->SetSQLINJChecks(BP_SQLGenerateRandomArray()) != ERR_SUCCESS)
			return ERR_FAILURE;

		// set the non-dummy checks for XSS
		if (this->SetXSSChecks(BP_XSSGenerateRandomArray()) != ERR_SUCCESS)
			return ERR_FAILURE;

	}

	// return indicating success
	return ERR_SUCCESS;

}

// Prints the local web assessment
int Block::PrintWebAssessment() {

	if (!this->webAssessment)
	{
		printf("\n [!!] Cannot print web assessment (most likely because of a bad link.)");
		return ERR_FAILURE;
	}

	BP_PrintAssessments(this->webAssessment);

	return ERR_SUCCESS;
}

// Destroys the local web assessment
int Block::DestroyWebAssessment(bool destroySession, bool destroyExtraction, bool destroyResults) {


	// ensure we have a web assessment
	if(!this->webAssessment)
		return ERR_FAILURE;


	// if we saved unparsed html in the class, destroy it first
	if (this->unparsedHtml)
	{
		// destroy unparsed html
		tq_free(this->tq, this->unparsedHtml);
		this->unparsedHtml = NULL;
		this->unparsedHtmlLen = 0;
	}


	// destroy assessment

	size_t retVal = 0;

	// destroy checks if they've been created
	if(this->webAssessment->sqlChecks)
	{
		BP_DestroySQLTests(this->webAssessment->sqlChecks, this->webAssessment->sqlCheckCount);
		this->webAssessment->sqlChecks     = NULL;
		this->webAssessment->sqlCheckCount = 0;
	}

	// destroy xss if set
	if(this->webAssessment->xssChecks)
	{
		BP_DestroyXSSTests(this->webAssessment->xssChecks, this->webAssessment->xssChecksCount);
		this->webAssessment->xssChecks     = NULL;
		this->webAssessment->xssChecksCount = 0;
	}

	// destroy the assessment
	retVal = BP_DestroyAssessment(this->webAssessment, destroySession, destroyExtraction, destroyResults);

	// set internal pointer to null
	this->webAssessment = NULL;

	if(destroyExtraction == true)
		this->htmlExtraction = NULL;


	// return ret value
	return retVal;

}

// generates an extended info list from file
size_t Block::GenerateUsableProxySetFromFile(char *filename, bool randomize_order)
{

	// check for http session structure
	if(!this->httpSession)
	{
		this->AddErrorToStack((char*) "Cannot generate proxy set.  NULL httpSession.");
		return ERR_FAILURE;
	}

	// return null if there is no filename
	if(!filename)
	{

		// exit failure
		this->AddErrorToStack((char*) "No file name provided (NULL).");
		return ERR_FAILURE;

	}

	// retrieve file contents of proxy file
	Block * fblock = new Block();
	fblock->FileGetContents(filename);

	// check to make sure we got content
	if(!fblock->got_contents)
	{

		// exit failure
		this->AddErrorToStack((char*) "File was empty.");
		return ERR_FAILURE;

	}

	// now explode file content along the newline barrier
	BP_StringUtils * str_utils = new BP_StringUtils(fblock->got_contents);

	// explode data here
	str_utils->Explode("\n");

	// run check to make sure we have exploded data
	if(!str_utils->exploded)
	{
		this->AddErrorToStack((char*) "Could not explode proxy file data.");
		return ERR_FAILURE;
	}


	if(randomize_order)
	{

		// create string utils object to randomize order
		BP_StringUtils * randomizer = new BP_StringUtils("nostring");

		// randomize the pointer array
		char ** randomized_array = randomizer->RandomizePointerArray(str_utils->exploded, str_utils->exploded_count-1);

		// destroy the exploded array
		tq_free(this->tq, str_utils->exploded);

		// set the exploded data to the new randomized array (no need to change length)
		str_utils->exploded = randomized_array;

	}
	/**/

	// CURRENT: Usable proxy set.
	if(!this->httpSession->proxy_set)
	{

		// generate nulled proxy set structure
		this->httpSession->proxy_set = (P_BP_WEB_USABLE_PROXY_SET) tq_calloc(this->tq, sizeof(BP_WEB_USABLE_PROXY_SET), 1, BPLN);

	}
	else
	{

		// destroy proxy list if it exists and allocate new data
		this->DestroyUsableProxySet();

		// generate nulled proxy set structure
		this->httpSession->proxy_set = (P_BP_WEB_USABLE_PROXY_SET) tq_calloc(this->tq, sizeof(BP_WEB_USABLE_PROXY_SET), 1, BPLN);

	}

	// initialize loop vars
	size_t n = 0;
	size_t proxy_config_n = 0;

	// proxy stack item
	P_BP_WEB_PROXY_EXTENDED_INFO proxy_stack = NULL;

	// now walk proxy lines and parse CSV
	for(; n < str_utils->exploded_count-1; n++)
	{

		// create csv parser
		BP_StringUtils * csv_parser = new BP_StringUtils(str_utils->exploded[n]);

		// parse csv
		csv_parser->ParseCsv(false);

		// number of csv rows must be 2 (command, pass)
		if(csv_parser->csv_num_rows != 1)
		{

			delete csv_parser;
			csv_parser = NULL;
			continue;
		}

		if(strnlen(csv_parser->csv[0][0], 5) < 2)
		{
			delete csv_parser;
			csv_parser = NULL;
			continue;
		}

		// bprealloc proxy stack to hold new element
		proxy_stack = (P_BP_WEB_PROXY_EXTENDED_INFO) tq_realloc(this->tq, proxy_stack, sizeof(BP_WEB_PROXY_EXTENDED_INFO) * (n+1), BPLN);

		// zero out proxy stack element
		memset(&proxy_stack[n], 0x00, sizeof(BP_WEB_PROXY_EXTENDED_INFO));

		// set connect string
		snprintf(proxy_stack[n].connect_string, 500, "%s", csv_parser->csv[0][0]);

		// check if proxy is http
		if(memcmp(csv_parser->csv[0][1], "http", 4) == 0)
			proxy_stack[n].proxy_type = BP_WEB_PROXY_TYPE_HTTP;

		// check if proxy is socks4 type
		if(memcmp(csv_parser->csv[0][1], "socks4", 5) == 0)
			proxy_stack[n].proxy_type = BP_WEB_PROXY_TYPE_SOCKS4;

		// check if proxy is socks5 type
		if(memcmp(csv_parser->csv[0][1], "socks5", 5) == 0)
			proxy_stack[n].proxy_type = BP_WEB_PROXY_TYPE_SOCKS5;


		// now set username and password
		snprintf(proxy_stack[n].creds.user, 500, "%s", csv_parser->csv[0][2]);
		snprintf(proxy_stack[n].creds.pass, 500, "%s", csv_parser->csv[0][3]);

		// delete the parser object on loop
		delete csv_parser;
		csv_parser = NULL;

	}

	// set proxy stack in proxy set
	this->httpSession->proxy_set->proxies = proxy_stack;

	// proxy count
	this->httpSession->proxy_set->proxy_cnt = n;

	// set the use proxy set to true
	this->httpSession->use_web_proxy_set = true;

	// return success
	return ERR_SUCCESS;

}

// CURRENT: Destroy the classes current proxy set data.
size_t Block::DestroyUsableProxySet()
{

	if(!this->httpSession)
		return ERR_FAILURE;

	// destroys the proxy set
	if(this->httpSession->proxy_set)
	{

		// destroy inner proxies pointer if it exists (should only need one bpfree due to static char[] buffs)
		if(this->httpSession->proxy_set->proxies)
		{

			tq_free(this->tq, this->httpSession->proxy_set->proxies);
			this->httpSession->proxy_set->proxies = NULL;

		}

		// bpfree the proxy set
		tq_free(this->tq, this->httpSession->proxy_set);

	}

	// set proxy set to null
	this->httpSession->proxy_set = NULL;
	return ERR_SUCCESS;
}

// CURRENT: Display usable proxy set.
size_t Block::DisplayUsableProxySet()
{

	// check for http session first
	if(!this->httpSession)
	{
		printf("\n [+] Http session is null which prevents proxy data from being printed.");
		return ERR_FAILURE;
	}

	if(!this->httpSession->proxy_set)
	{
		printf("\n [+] Proxy set is NULL and cannot be printed.");
		return ERR_FAILURE;
	}

	// display proxy info
	printf("\n [+] Proxy Set: %p", this->httpSession->proxy_set);
	printf("\n       proxy count:      %u", this->httpSession->proxy_set->proxy_cnt);
	printf("\n       last proxy index: %u", this->httpSession->proxy_set->last_proxy_idx);

	// store proxy count
	size_t proxy_set_count = this->httpSession->proxy_set->proxy_cnt;
	size_t n = 0;
	for(; n < proxy_set_count; n++)
	{

		if(!this->httpSession->proxy_set->proxies)
		{
			printf("\n Error.  Null proxies set. ");
			continue;
		}
		// ensure the proxy elements exist


		char *proxy_type = NULL;
		switch(this->httpSession->proxy_set->proxies[n].proxy_type)
		{
			case BP_WEB_PROXY_TYPE_HTTP:
				proxy_type = (char *) "http";
				break;
			case BP_WEB_PROXY_TYPE_SOCKS4:
				proxy_type = (char *) "socks4";
				break;
			case BP_WEB_PROXY_TYPE_SOCKS5:
				proxy_type = (char *) "socks5";
				break;
			default:
				proxy_type = (char *) "unset or undefined type.";
				break;

		}

		printf("\n\t [+] Proxy Info Entry: %p", &this->httpSession->proxy_set->proxies[n]);
		printf("\n\t   connect string: %s", this->httpSession->proxy_set->proxies[n].connect_string);
		printf("\n\t   validated:      %s", this->httpSession->proxy_set->proxies[n].validated ? "yes" : "no");
		printf("\n\t   proxy type:     %s", proxy_type);
		printf("\n\t   creds:          %s | %s", this->httpSession->proxy_set->proxies[n].creds.user, this->httpSession->proxy_set->proxies[n].creds.pass);

	}

	// return indicating success
	return ERR_SUCCESS;

}

// Function to retrieve List of proxies from file (newline delimited)
char ** Block::GenerateProxyListFromFile(char *filename)
{

	if (!filename)
	{
		this->AddErrorToStack((char*) "Error in Block::GenerateProxyListFromFile(): No filename provided.");
		return NULL;
	}

	// return a populated proxy list
	char **proxies = BP_HTTPGenerateProxyListFromFile(filename);

	if (!proxies) {
		this->AddErrorToStack((char*) "Error in Block::GenerateProxyListsFromFile(): No ip's found generated from file specified.");
		return NULL;
	}

	// return proxy list if set
	return proxies;
}

// CURRENT: Validate proxy configuration.
size_t Block::ValidateProxyConfig(P_BP_WEB_PROXY_EXTENDED_INFO cfg)
{

	if (!this->httpSession)
	{
		this->AddErrorToStack((char*) "Error in Block::ValidateHttpProxy(): This function requires a valid httpSession.");
		return ERR_FAILURE;
	}

	if (!this->proxyHttpTestSite || !this->proxyHttpTestExpected)
	{
		this->AddErrorToStack((char*) "Error in Block::ValidateHttpProxy(): no test conditions provided");
		return ERR_FAILURE;
	}

	// set proxy timeout
	BP_HTTPSetTimeout(this->httpSession, this->proxyHttpTestTimeout);

	// set proxy type as type
	BP_HTTPSetProxyType(this->httpSession, cfg->proxy_type);

	// set proxy as ip_addr
	BP_HTTPSetProxy(this->httpSession, cfg->connect_string);


	// try to validate current proxy, return ERR_SUCCESS/ERR_FAILURE
	return BP_HTTPValidateCurrentProxy(this->httpSession, this->proxyHttpTestSite, this->proxyHttpTestExpected);

}

// Validates the proxy list, populating the validated proxy arrays.  Will attempt
// to validate proxies according to type.
size_t Block::ValidateProxies(bool vhttp, bool vconnect, size_t proxy_type)
{

	if (!this->httpSession)
	{
		this->AddErrorToStack((char*) "Error in Block::ValidateProxies(): This function requires a valid httpSession.");
		return ERR_FAILURE;
	}

	if (!this->proxies)
	{
		this->AddErrorToStack((char*) "Error in Block::ValidateProxies(): No proxies are set");
		return ERR_FAILURE;
	}

	// index variables used for looping through ptr arrays
	size_t i = 0;
	size_t x = 0;

	// validate http proxies first if set
	if (vhttp)
	{

		// destroy old http proxy list if specified
		if (this->validatedHttpProxies)
		{
			tq_free(this->tq, this->validatedHttpProxies);
			this->validatedHttpProxies = NULL;
		}

		// now validate proxies one by one
		for (i = 0, x = 0; this->proxies[i]; i++)
		{

			// if proxy is validated, add it to list
			if
			(
					this->ValidateHttpProxy(this->proxies[i], CURLPROXY_HTTP)   == ERR_SUCCESS ||
					this->ValidateHttpProxy(this->proxies[i], CURLPROXY_SOCKS4)	== ERR_SUCCESS ||
					this->ValidateHttpProxy(this->proxies[i], CURLPROXY_SOCKS5) == ERR_SUCCESS
			)
			{

				// bprealloc validated proxies array with space for additional member
				this->validatedHttpProxies = (char **) tq_realloc(this->tq, this->validatedHttpProxies, sizeof(char *) * (x + 2), BPLN);

				// set pointer in structure
				this->validatedHttpProxies[x] = this->proxies[i];

				// increment array index
				x++;

			}

		}

		// null terminate array if set
		if (this->validatedHttpProxies)
			this->validatedHttpProxies[x] = NULL;

	}

	// validate connect proxies now
	if (vconnect)
	{

	}

	// return indicating success
	return ERR_SUCCESS;

}

// Validates one proxy (http)
size_t Block::ValidateHttpProxy(char *ip_addr, size_t curl_proxy_type)
{

	if (!this->httpSession)
	{
		this->AddErrorToStack((char*) "Error in Block::ValidateHttpProxy(): This function requires a valid httpSession.");
		return ERR_FAILURE;
	}

	if
	(
		!this->proxyHttpTestSite ||
		!this->proxyHttpTestExpected
	)
	{
		this->AddErrorToStack((char*) "Error in Block::ValidateHttpProxy(): no test conditions provided");
		return ERR_FAILURE;
	}

	// set proxy timeout
	BP_HTTPSetTimeout(this->httpSession, this->proxyHttpTestTimeout);

	// set proxy type as type
	BP_HTTPSetProxyType(this->httpSession, curl_proxy_type);

	// set proxy as ip_addr
	BP_HTTPSetProxy(this->httpSession, ip_addr);

	// try to validate current proxy, return ERR_SUCCESS/ERR_FAILURE
	return BP_HTTPValidateCurrentProxy(this->httpSession, this->proxyHttpTestSite, this->proxyHttpTestExpected);

}

//TODO: Validates one proxy (connect)
size_t Block::ValidateConnectProxy(char *ip_addr)
{

	// ensure we have a http session
	if (!this->httpSession)
	{
		this->AddErrorToStack((char*) "Error in Block::ValidateConnectProxies(): This function requires a valid httpSession.");
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}

#endif // BP_BLOCKCLASS_INCLUDE_WEB_ASSESSMENT_BINDINGS
#ifdef BP_BLOCKCLASS_INCLUDE_MYSQL_DB_BINDINGS
#ifdef BP_BLOCKCLASS_INCLUDE_NMAP_SQL_DB_BINDINGS

// reset search parameters
void Block::resetNmapDbSearchParams()
{

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// portstat search items
	memset(&this->portstats_min_s, 0x00, sizeof(this->portstats_min_s));
	memset(&this->portstats_max_s, 0x00, sizeof(this->portstats_max_s));
	memset(&this->portstats_like_s, 0x00, sizeof(this->portstats_like_s));
	memset(&this->portstats_equal_s, 0x00, sizeof(this->portstats_equal_s));

	// hoststats search items
	memset(&this->hoststats_min_s, 0x00, sizeof(this->portstats_min_s));
	memset(&this->hoststats_max_s, 0x00, sizeof(this->portstats_max_s));
	memset(&this->hoststats_like_s, 0x00, sizeof(this->portstats_like_s));
	memset(&this->hoststats_equal_s, 0x00, sizeof(this->portstats_equal_s));

	// target search items
	memset(&this->targets_min_s, 0x00, sizeof(this->portstats_min_s));
	memset(&this->targets_max_s, 0x00, sizeof(this->portstats_max_s));
	memset(&this->targets_like_s, 0x00, sizeof(this->portstats_like_s));
	memset(&this->targets_equal_s, 0x00, sizeof(this->portstats_equal_s));

	// scanner search items
	memset(&this->scanners_min_s, 0x00, sizeof(this->portstats_min_s));
	memset(&this->scanners_max_s, 0x00, sizeof(this->portstats_max_s));
	memset(&this->scanners_like_s, 0x00, sizeof(this->portstats_like_s));
	memset(&this->scanners_equal_s, 0x00, sizeof(this->portstats_equal_s));

	// reset runlist search items
	memset(&this->runlist_min_s,   0x00, sizeof(this->portstats_min_s));
	memset(&this->runlist_max_s,   0x00, sizeof(this->portstats_max_s));
	memset(&this->runlist_like_s,  0x00, sizeof(this->portstats_like_s));
	memset(&this->runlist_equal_s, 0x00, sizeof(this->portstats_equal_s));

	// leave critical section
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION;

}

// generate queries and search the database
size_t Block::searchNmapDb()
{

	// check mysql session
	if (!this->mysqlSession)
	{
		this->AddErrorToStack((char*) "Error in Block::searchNmapDb(): No MySQL connection for this block.");
		return ERR_FAILURE;
	}

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// create empty compare structures
	BP_NMAPDB_PORTSTATS portstats_empty = { 0 };
	BP_NMAPDB_HOSTSTATS hoststats_empty = { 0 };
	BP_NMAPDB_TARGETS targets_empty     = { 0 };
	BP_NMAPDB_SCANNERS scanners_empty   = { 0 };
	BP_NMAPDB_RUNLIST runlist_empty     = { 0 };

	// check portstats to see if the user specified they want to search through them
	P_BP_NMAPDB_RESULTS portstats_results = NULL;

	if
	(
			memcmp(&this->portstats_min_s,   &portstats_empty, sizeof(portstats_empty)) ||
			memcmp(&this->portstats_max_s,   &portstats_empty, sizeof(portstats_empty)) ||
			memcmp(&this->portstats_like_s,  &portstats_empty, sizeof(portstats_empty)) ||
			memcmp(&this->portstats_equal_s, &portstats_empty, sizeof(portstats_empty))
	)
	{

		// search port data around our settings
		portstats_results = BP_NDBSearchPortStats
		(
			this->mysqlSession,
			&this->portstats_min_s,
			&this->portstats_max_s,
			&this->portstats_like_s,
			&this->portstats_equal_s,
			this->limitBottom,
			this->limitTop
		);

	}

	// check hoststats to see if the user specified they want to search through them
	P_BP_NMAPDB_RESULTS hoststats_results = NULL;

	if
	(
			memcmp(&this->hoststats_min_s, &hoststats_empty, sizeof(hoststats_empty))  ||
			memcmp(&this->hoststats_max_s, &hoststats_empty, sizeof(hoststats_empty))  ||
			memcmp(&this->hoststats_like_s, &hoststats_empty, sizeof(hoststats_empty)) ||
			memcmp(&this->hoststats_equal_s, &hoststats_empty, sizeof(hoststats_empty))
	)
	{

		// search port data around our settings
		hoststats_results = BP_NDBSearchHostStats
		(
				this->mysqlSession,
				&this->hoststats_min_s,
				&this->hoststats_max_s,
				&this->hoststats_like_s,
				&this->hoststats_equal_s,
				this->limitBottom,
				this->limitTop
		);

	}

	// check targets to see if the user specified they want to search through them
	P_BP_NMAPDB_RESULTS targets_results = NULL;

	if
	(
			memcmp(&this->targets_min_s,   &targets_empty, sizeof(targets_empty)) ||
			memcmp(&this->targets_max_s,   &targets_empty, sizeof(targets_empty)) ||
			memcmp(&this->targets_like_s,  &targets_empty, sizeof(targets_empty)) ||
			memcmp(&this->targets_equal_s, &targets_empty, sizeof(targets_empty))
	)
	{

		// search targets around our settings
		targets_results = BP_NDBSearchTargets
		(
				this->mysqlSession,
				&this->targets_min_s,
				&this->targets_max_s,
				&this->targets_like_s,
				&this->targets_equal_s,
				this->limitBottom,
				this->limitTop
		);

	}

	// check scanners to see if the user specified they want to search through them
	P_BP_NMAPDB_RESULTS scanners_results = NULL;

	if
	(
			memcmp(&this->scanners_min_s, &scanners_empty, sizeof(scanners_empty)) ||
			memcmp(&this->scanners_max_s, &scanners_empty, sizeof(scanners_empty)) ||
			memcmp(&this->scanners_like_s, &scanners_empty, sizeof(scanners_empty)) ||
			memcmp(&this->scanners_equal_s, &scanners_empty, sizeof(scanners_empty))
	)
	{

		// search scanners around our settings
		hoststats_results = BP_NDBSearchScanners
		(
				this->mysqlSession,
				&this->scanners_min_s,
				&this->scanners_max_s,
				&this->scanners_like_s,
				&this->scanners_equal_s,
				this->limitBottom,
				this->limitTop
		);

	}

	// check runlist to see if the user specified they want to search through them
	P_BP_NMAPDB_RESULTS runlist_results = NULL;

	if
	(
			memcmp(&this->runlist_min_s, &runlist_empty, sizeof(runlist_empty)) ||
			memcmp(&this->runlist_max_s, &runlist_empty, sizeof(runlist_empty)) ||
			memcmp(&this->runlist_like_s, &runlist_empty, sizeof(runlist_empty)) ||
			memcmp(&this->runlist_equal_s, &runlist_empty, sizeof(runlist_empty))
	)
	{

		// search port data around our settings
		runlist_results = BP_NDBSearchRunlist
		(
				this->mysqlSession,
				&this->runlist_min_s,
				&this->runlist_max_s,
				&this->runlist_like_s,
				&this->runlist_equal_s,
				this->limitBottom,
				this->limitTop
		);

	}

	// ---------------------------------------------------------------------------

	// allocate dbresults structure if non-allocated, reuse if it is allocated
	if (!this->nmapdb_results)
	{

		if
		(
				portstats_results ||
				hoststats_results ||
				targets_results   ||
				scanners_results  ||
				runlist_results
		)
		{
			this->nmapdb_results = (P_BP_NMAPDB_SEARCH_RESULTS) tq_calloc(this->tq, sizeof(BP_NMAPDB_SEARCH_RESULTS), 1, BPLN);
		}

	}

	// set database pointers in structures
	if (this->nmapdb_results)
	{
		this->nmapdb_results->runlist   = runlist_results;
		this->nmapdb_results->scanners  = scanners_results;
		this->nmapdb_results->targets   = targets_results;
		this->nmapdb_results->hoststats = hoststats_results;
		this->nmapdb_results->portstats = portstats_results;
	}
	else
	{
		this->AddWarningToStack((char *) "Warning in Block::searchNmapDb(): Search returned 0 results.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// displays results of search in a human readable format
size_t Block::displayNmapDBSearchResults(bool displayQuery)
{

	if (!this->nmapdb_results)
	{
		this->AddErrorToStack((char*) "Error in Block::displayNmapDBSearchResults(): NULL results/Empty Set.");
		return ERR_FAILURE;
	}

	// display runlist info
	BP_NDBVisualizeResults(this->nmapdb_results->runlist, displayQuery);

	// display scanners info
	BP_NDBVisualizeResults(this->nmapdb_results->scanners, displayQuery);

	// display targets info
	BP_NDBVisualizeResults(this->nmapdb_results->targets, displayQuery);

	// display hoststats info
	BP_NDBVisualizeResults(this->nmapdb_results->hoststats, displayQuery);

	// display porstats info
	BP_NDBVisualizeResults(this->nmapdb_results->portstats, displayQuery);

	return ERR_SUCCESS;
}

// retrieves a null pointer terminated list of ips from
// a given result set.  IPs correlate to the results ip
// member data.  The ip strings are duplicated from result
// data, so they do need to be tq_free(this->tq, )'d individually.  If
// the append_ports flag is set to true, if a port exists in the
// datastructure, the port will be appended to the string like
// 127.0.0.1:8080 format.
char ** Block::retrieveIPsFromNmapDBResult(P_BP_NMAPDB_RESULTS result, bool append_ports)
{

	// ensure we have a result
	if (!result)
	{
		this->AddErrorToStack((char*) "Error in Block::retrieveIPsFromNmapDBResult(): NULL result parameter passed.  Invalid parameter.");
		return NULL;
	}

	// create array for returning data
	char ** ips = (char **) NULL;

	// index counter
	size_t i = 0;

	// temporary pointers for resolving opaque data
	BP_NMAPDB_RUNLIST * tmp_runlist = NULL;
	BP_NMAPDB_SCANNERS * tmp_scanners = NULL;
	BP_NMAPDB_TARGETS * tmp_targets = NULL;
	BP_NMAPDB_HOSTSTATS * tmp_hoststats = NULL;
	BP_NMAPDB_PORTSTATS * tmp_portstats = NULL;

	// This switch decides what ip fields from which structures the ip data will
	// be pulled.  Each case is nearly identical with the exception of the member data
	// being used.
	switch (result->type)
	{

		case BP_NMAPDB_RESULT_RUNLIST:

			// resolve opaque data
			tmp_runlist = (P_BP_NMAPDB_RUNLIST) result->opaque_data;

			if (result->record_count > 0)
				for (i = 0; i < result->record_count; i++) {

					// bprealloc data
					ips = (char **) tq_realloc(this->tq, ips, sizeof(char *) * (i + 1), BPLN);

					// allocate string
					ips[i] = tq_strdup(this->tq, tmp_runlist[i].host_ip, BPLN);

				}


			break;

		case BP_NMAPDB_RESULT_SCANNERS:

			// resolve opaque data
			tmp_scanners = (P_BP_NMAPDB_SCANNERS) result->opaque_data;

			if (result->record_count > 0)
				for (i = 0; i < result->record_count; i++) {

					// bprealloc data
					ips = (char **) tq_realloc(this->tq, ips, sizeof(char *) * (i + 1), BPLN);

					// set string
					ips[i] = tq_strdup(this->tq, tmp_scanners[i].scanner_ip, BPLN);

				}


			break;

		case BP_NMAPDB_RESULT_TARGETS:

			// resolve opaque data
			tmp_targets = (P_NMAPDB_TARGETS) result->opaque_data;

			if (result->record_count > 0)
				for (i = 0; i < result->record_count; i++) {

					// bprealloc data
					ips = (char **) tq_realloc(this->tq, ips, sizeof(char *) * (i + 1), BPLN);

					// set string
					ips[i] = tq_strdup(this->tq, tmp_targets[i].ip, BPLN);

				}

			break;

		case BP_NMAPDB_RESULT_HOSTSTATS:

			// resolve opaque data
			tmp_hoststats = (P_BP_NMAPDB_HOSTSTATS) result->opaque_data;

			if (result->record_count > 0)
				for (i = 0; i < result->record_count; i++) {

					// bprealloc data
					ips = (char **) tq_realloc(this->tq, ips, sizeof(char *) * (i + 1), BPLN);

					// set string
					ips[i] = tq_strdup(this->tq, tmp_hoststats[i].ip, BPLN);

				}

			break;

		case BP_NMAPDB_RESULT_PORTSTATS:

			// resolve opaque data
			tmp_portstats = (P_BP_NMAPDB_PORTSTATS) result->opaque_data;

			if (result->record_count > 0)
			{
				for (i = 0; i < result->record_count - 1; i++) {

					// bprealloc data
					ips = (char **) tq_realloc(this->tq, ips, sizeof(char *) * (i + 2), BPLN);

					// allocate string
					ips[i] = (char *) tq_calloc(this->tq, sizeof(tmp_portstats[i].target_ip) + 40, 1, BPLN);

					// create string
					snprintf(ips[i], sizeof(tmp_portstats[i].target_ip) - 1, "%s", tmp_portstats[i].target_ip);

					char port_str[64] = { 0 };
					sprintf(port_str, ":%u", tmp_portstats[i].port);

					// append port to the end of the string if specified
					if (append_ports)
						strncat(ips[i], port_str, 10);

				}

			}

			// terminate ip array
			if (ips)
				ips[i + 1] = NULL;

			break;

		default:
			break;

	}

	// return ips, or null if there are none
	if(ips)
		ips[i] = NULL;

	return ips;

}

#endif // BP_BLOCKCLASS_INCLUDE_NMAP_SQL_DB_BINDINGS
#endif // BP_BLOCKCLASS_INCLUDE_MYSQL_DB_BINDINGS
#ifdef BP_BLOCKCLASS_INCLUDE_COMMAND_CONTROL_INTERFACE

/*
 * Blockparty "Command and Control" Interface:
 * -----------------------------------------------
 * Used to supply command and control data to a pair of filedescriptors.  What this
 * means is that the filedescriptors are mapped either from an open process or
 * an open socket etc to read and write positions in a FD_SET structure. From there
 * you can supply data automatically to the mapped filedescriptors by a manner of
 * configured yes/no tests.
 */

size_t Block::ccNewSession(P_BP_FDSET fds, char * session_name, BP_CC_SHELL_INTERFACE shell_interface)
{

	if (!fds)
	{
		this->AddErrorToStack((char*) "Error in Block::ccNewSession(): No filedescriptor set provided.");
		return ERR_FAILURE;
	}

	if (!session_name) {
		this->AddErrorToStack((char*) "Error in Block::ccNewSession(): No session name provided.");
		return ERR_FAILURE;
	}

	// create new session
	this->cc_session = BP_CCNewSession(fds, session_name, shell_interface);

	// ensure session was created ok
	if (!this->cc_session)
	{
		this->AddErrorToStack((char*) "Error in Block::ccNewSession(): Could not create new session.");
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}

// displays an existing session data
size_t Block::ccDisplaySession() {

	if (!this->cc_session) {
		this->AddErrorToStack((char*) "Error in Block::ccDisplaySession(): Empty session.");
		return ERR_FAILURE;
	}

	if (BP_CCVisualizeSession(this->cc_session, true, true) != ERR_SUCCESS) {
		this->AddErrorToStack((char*) "Error in Block::ccDisplaySession: Could not display session (corrupt). ");
		return ERR_FAILURE;
	}

	return ERR_SUCCESS;
}

// Adds a command to the session (deprecated: use ccAddTryExcept() instead where possible)
size_t Block::ccAddCommand(char *command_name) {

	if (!this->cc_session)
	{
		this->AddErrorToStack((char*) "Error in Block::ccAddCommand(): Empty session.");
		return ERR_FAILURE;
	}

	if (!command_name)
	{
		this->AddErrorToStack((char*) "Error in Block::ccAddCommand(): Empty command name.");
		return ERR_FAILURE;
	}

	if (!this->cc_session->commands)
	{

		// two elements on first assignment
		this->cc_session->commands = (P_BP_CC_DECISION_TREE *) tq_calloc(this->tq, sizeof(P_BP_CC_DECISION_TREE) * 2, 1, BPLN);
		this->cc_session->commands[0] = BP_CCCreateNewDecisionTree(this->cc_session, command_name, NULL);
	}
	else
	{

		// get number of commands
		size_t i;
		for (i = 1; cc_session->commands[i - 1]; i++)
		{}

		// increment +2 i a final time for new element + null ptr
		i += 2;

		// allocate space
		this->cc_session->commands = (P_BP_CC_DECISION_TREE *) tq_realloc(this->tq, this->cc_session->commands, sizeof(P_BP_CC_DECISION_TREE) * i, BPLN);

		// -2 for proper indexing
		this->cc_session->commands[i - 2] = BP_CCCreateNewDecisionTree(this->cc_session, command_name, NULL);

	}

	return ERR_SUCCESS;
}

// see if a command exists (return if exists)
P_BP_CC_DECISION_TREE Block::ccCommandExists(char *command_name)
{

	// walk commands and try to find command name in commands
	size_t i = 0;
	for
	(
			i = 0;
			this->cc_session->commands[i];
			i++
	)
	{

		// check to see if command matches command string in command array (match command)
		if
		(
				strncmp
				(
						this->cc_session->commands[i]->command,
						command_name,
						bpstrlen(command_name)
				) == 0
		)
		{
			return (P_BP_CC_DECISION_TREE) this->cc_session->commands[i];
		}

	}

	// return null if unfound
	return NULL;

}

// adds a try/except clause to the end of a branch (posix regexp on except)
size_t Block::ccAddTryExcept
(
	char                   *command_name,
	char                   *try_this,
	size_t                  try_length,
	char                   *expect_this,
	size_t                  expect_length,
	BP_CC_MATCH_BUFF_POLICY expect_match_policy
)
{

	// ensure we have a cc session
	if(!this->cc_session)
	{

		// attempt to add error to block error stack
		this->AddErrorToStack((char*) "Error in Block::ccAddTryExcept(): Empty session.");
		return ERR_FAILURE;

	}

	// ensure we have a command name
	if(!command_name)
	{
		this->AddErrorToStack((char*) "Error in Block::ccAddTryExcept(): Empty command_name.");
		return ERR_FAILURE;
	}

	// Attempt to add try/except to command.
	if
	(
		!BP_CCAddTryExpectToCommand
		(
			this->cc_session,
			command_name,
			try_this,
			try_length,
			expect_this,
			expect_length,
			expect_match_policy
		)
	)
	{

		// return indicating that the try/expect combo cannot be added
		return ERR_FAILURE;

	}

	// return indicating success
	return ERR_SUCCESS;

}

// execute a command with a given argument substitution stack
size_t Block::ccExecuteCommand(char *command_name, char ** arg_stack)
{

	// ensure we have a session
	if(!this->cc_session)
	{
		this->AddErrorToStack((char*) "Error in Block::ccExecuteCommand(): Empty session.");
		return ERR_FAILURE;
	}

	if(!command_name)
	{
		this->AddErrorToStack((char*) "Error in Block::ccExecuteCommand(): Empty command_name.");
		return ERR_FAILURE;
	}

	// dispatch command
	if(BP_CCDispatcher(this->cc_session, command_name, arg_stack) == ERR_FAILURE)
	{
		this->AddErrorToStack((char*) "Error in Block::ccExecuteCommand(): Command dispatcher failed.");
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}

// Adds a premature exit pcre to the command.  This allows us to monitor output for certain strings
// and exit upon instance.
BP_ERROR_T Block::ccAddPrematureExitPCRE(char *exit_expression)
{

	// requires a valid cc session
	if(!this->cc_session)
		return ERR_FAILURE;

	// check for an exit expression
	if(!exit_expression)
	{
		return ERR_FAILURE;
	}

	// add a premature exit string
	if(!BP_CCAddPrematureExitTestString(this->cc_session, exit_expression) == ERR_FAILURE)
	{
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}



// Adds an expression which will be used to match command output to specific data.
BP_ERROR_T Block::ccAddInformationExtractionPCRE(char *info_expression)
{

	// requires a valid cc session
	if(!this->cc_session)
		return ERR_FAILURE;

	// check for an info expression
	if(!info_expression)
	{
		return ERR_FAILURE;
	}

	// add a premature exit string
	if(!BP_CCAddInformationExtractionRegexpString(this->cc_session, info_expression) == ERR_FAILURE)
	{
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;
}

#endif // BP_BLOCKCLASS_INCLUDE_COMMAND_CONTROL_INTERFACE




#ifdef BP_BLOCKCLASS_INCLUDE_USER_DB

// Returns err success if a user was able to be retrieved via lookup.  User
// information is at that point stored in this->user_info.
BP_ERROR_T Block::authUserInDB(char * username, char *password)
{

	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	if(!username)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	if(!password)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// destroy the user info if it's already set
	if(this->user_info)
	{

		// destroy auth user data
		this->destroyAuthUserData();

	}


	BP_StringUtils * builder = new BP_StringUtils(NULL);

	// add the front part of the string
	builder->BuildString
	(
		"SELECT `id`, `first_name`, `last_name`, `email`, `username`, `created`, `role` FROM `users` where `username` = "
	);

	// add username
	builder->BuildMySQLHexFromBuff(username, bpstrlen(username));

	builder->BuildString(" and `password` = BINARY SHA1(concat((select `salt` from `users` where `username` = ");

	// add username again
	builder->BuildMySQLHexFromBuff(username, bpstrlen(username));

	// build subquery
	builder->BuildString("), ");

	// add password finally
	builder->BuildMySQLHexFromBuff(password, bpstrlen(password));

	// finish the query building
	builder->BuildString(")) LIMIT 0,1");


	// attempt to prepare query
	if(this->mysqlPrepareQuery(builder->build_str) != ERR_SUCCESS)
	{

		delete builder;
		this->AddErrorToStack("SQL prepare failed when attempting result retrieval.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}


	// `id`, `first_name`, `last_name`, `email`, `username`, `created`, `role`
	size_t id = 0;
	char   first_name_res[512] = {0};
	char   last_name_res [512] = {0};
	char   email_res     [512] = {0};
	char   username_res  [512] = {0};
	char   created_res   [512] = {0};
	char   role_res      [512] = {0};

	// add parameters
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &id,             sizeof(id));
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &first_name_res, sizeof(first_name_res));
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &last_name_res,  sizeof(last_name_res));
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &email_res,      sizeof(email_res));
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &username_res,   sizeof(username_res));
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &created_res,    sizeof(created_res));
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &role_res,       sizeof(role_res));

	// bind the results
	this->mysqlBindResults();

	// execute the prepared statement
	if(this->mysqlExecuteStatement() != ERR_SUCCESS)
	{

		// delete the builder
		delete builder;

		// destroy the statement (this is required)
		this->mysqlDestroyStatement();

		// exit indicating failure
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}

	// store the results
	this->mysqlStoreResults();

	// fetch one result (max one result)
	this->mysqlFetchResult();


	BP_MySQLDisplaySession(this->mysqlSession);

	// pointer id
	size_t * ptr_id = (size_t *) mysqlSession->bind_results_external_ptr[0].buffer;

	// if the poitner id is set we know to copy in data
	if(*ptr_id)
	{


		// allocate space for user info
		this->user_info = (P_BP_USER) tq_calloc(this->tq, sizeof(BP_USER), 1, BPLN);

		// create column iterator for copying data into result
		size_t col_iter = 0;

		// `id`,
		memcpy(&this->user_info->user_id, mysqlSession->bind_results_external_ptr[col_iter].buffer, sizeof(this->user_info->user_id));
		col_iter++;

		// `first_name`,
		this->user_info->first_name = tq_strndup(this->tq, (char *) mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length, BPLN);
		col_iter++;

		// `last_name`,
		this->user_info->last_name = tq_strndup(this->tq, (char *) mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length, BPLN);
		col_iter++;

		// `email`,
		this->user_info->email = tq_strndup(this->tq, (char *) mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length, BPLN);
		col_iter++;

		// `username`,
		this->user_info->username = tq_strndup(this->tq, (char *) mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length, BPLN);
		col_iter++;

		// `created`,
		this->user_info->created = tq_strndup(this->tq, (char *) mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length, BPLN);
		col_iter++;

		// `role`
		this->user_info->role = tq_strndup(this->tq, (char *) mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length, BPLN);
		col_iter++;


		// set authenticated to true
		this->user_info->authenticated = BP_TRUE;

	}
	else
	{
		printf("\n ID FAILED TO RETRIEVE OK?!");
		printf("\n [+] AUTH USER HERE2?: %s", first_name_res);
	}

	// destroy the statement
	this->mysqlDestroyStatement();

	// delete the query builder before exit
	delete builder;

	// return indicating failure if we can't match a user
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

}

// Destroys the authenticated user data stored within this->user_info.  Pointer
// is nullified after it's destroyed.
BP_ERROR_T Block::destroyAuthUserData()
{

	// destroy the user info if it's already set
	if(this->user_info)
	{

		if(this->user_info->first_name)
			tq_free(this->tq, this->user_info->first_name);

		if(this->user_info->last_name)
			tq_free(this->tq, this->user_info->last_name);

		if(this->user_info->email)
			tq_free(this->tq, this->user_info->email);

		if(this->user_info->username)
			tq_free(this->tq, this->user_info->username);

		if(this->user_info->created)
			tq_free(this->tq, this->user_info->created);

		if(this->user_info->role)
			tq_free(this->tq, this->user_info->role);

		// destroy the user info
		tq_free(this->tq, this->user_info);

		// nullify the pointer
		this->user_info = NULL;

	}

	// destroy the statement
	this->mysqlDestroyStatement();

	// return indicating success
	return ERR_SUCCESS;

}

// display user info
BP_ERROR_T Block::displayAuthUserData()
{

	// ensure we have a user info parameter
	if(!this->user_info)
		return ERR_FAILURE;

	printf("\n [+] UserDB Info:");
	printf("\n     username:   %s", this->user_info->username);
	printf("\n     first_name: %s", this->user_info->first_name);
	printf("\n     last_name:  %s", this->user_info->last_name);
	printf("\n     email:      %s", this->user_info->email);
	printf("\n     created:    %s", this->user_info->created);
	printf("\n     role:       %s", this->user_info->role);

	// return indicating success
	return ERR_SUCCESS;

}


#endif // BP_BLOCKCLASS_INCLUDE_USER_DB




#ifdef BP_BLOCKCLASS_RESULT_DB

// displays the block configuration as is currently
// displayed.  This is high level information which does
// not include the data of substructures.  It's recommended
// to use the related methods of those substructures to display
// that relevant data.
BP_ERROR_T Block::displayBlockWebConfiguration()
{

	// display elements
	printf("\n [+] Block Web Configuration for %p", this);
	printf("\n\t current_result_id:   %u", this->web_result_id);
	printf("\n\t current_result_href: %s", (char *) &this->web_result_href);
	printf("\n\t web_result_stack:    %p", this->web_result_stack);
	printf("\n\t web_result_stack_n:  %u", this->web_result_stack_n);
	printf("\n\t enable_form_data_logging_during_extraction:   %u", this->enable_anchor_data_logging_during_extraction ? 1:0);
	printf("\n\t enable_anchor_data_logging_during_extraction: %u", this->enable_anchor_data_logging_during_extraction ? 1:0);
	printf("\n\t web_request_jit_match_set: %p", this->web_request_jit_match_set);
	printf("\n\t web_request_jit_match_n:   %u", this->web_request_jit_match_n);
	printf("\n\t web_enable_jit_match_sets: %u", this->web_enable_jit_match_sets ? 1 : 0);
	printf("\n");

	// return indicating success
	return ERR_SUCCESS;

}


// ============ Multi Result Retrieval Routines ===============


// destroys the web result stack (if it exists)
BP_ERROR_T Block::destroyWebResultStack()
{

	// destroy the pointer if it exists
	if(this->web_result_stack)
	{

		// destroy the result stack
		BP_DestroyLinkL(this->web_result_stack_data_allocator, BP_TRUE);
		this->web_result_stack = NULL;
	}

	// set popped result id as zero
	this->web_result_id = 0;

	// reset pointer and count
	this->web_result_stack = NULL;
	this->web_result_stack_n = 0;
	this->web_result_stack_all_popped = 0;
	this->web_result_stack_current_index = 0;

	// rest local href/id etc
	memset(&this->web_result_href, 0x00, sizeof(this->web_result_href));

	// return indicating success
	// never-fails.
	return ERR_SUCCESS;

}

// push an element on the web result stack
BP_ERROR_T Block::popWebResultWebStack()
{

	// ensure we have a result stack
	if(!this->web_result_stack_n)
		return ERR_FAILURE;

	// if we've hit the top, stop popping, destroy data, and return
	if(this->web_result_stack_current_index == (this->web_result_stack_n -1))
	{
		printf("\n GOT?: %u", this->web_result_id );
		return ERR_FAILURE;
	}

	// increment the current index
	this->web_result_stack_current_index++;

	// zero the buffer before copy
	memset((char *) &this->web_result_href, 0x00, sizeof(this->web_result_href));

	printf("\n Current Index?: %u", this->web_result_stack_current_index);

	// copy in string
	strncat((char *) &this->web_result_href, this->web_result_stack[this->web_result_stack_current_index].href, sizeof(this->web_result_href)-1);

	// set the result id
	this->web_result_id = this->web_result_stack[this->web_result_stack_current_index].result_id;

	// return indicating success
	return ERR_SUCCESS;

}

//  push an element on the web result stack
BP_ERROR_T Block::pushWebResultWebStack( _vin_stack P_BP_WEB_RESULT_ENTRY entry)
{


	// return indicating success
	return ERR_SUCCESS;
}

// check whether or not a record should be disabled.  As a note
// it's important to know that this is called directly from popWebResultWebStack
// automatically, this is to streamline the process of getting non-disabled
// entries.
BP_ERROR_T Block::checkWebResultEntryShouldBeDisabled(P_BP_WEB_RESULT_ENTRY entry)
{

	// ensure we have a msyql session to work with
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// ensure threshold values are set
	if(!this->web_result_disable_script_threshold)
	{
		printf("\n [!!] Error: Disable checks require a this->web_result_disable_script_threshold.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}
	if(!this->web_result_disable_path_threshold)
	{
		printf("\n [!!] Error: Disable checks require a this->web_result_disable_path_threshold.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}
	if(!this->web_result_disable_base_threshold)
	{
		printf("\n [!!] Error: Disable checks require a this->web_result_disable_base_threshold.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// ensure we have an entry
	if(!entry)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// require a set result id
	if(!entry->result_id)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// require a base
	if(!entry->base)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// require a base
	if(!entry->path)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// require a script
	if(!entry->script)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);


	// convert parameters to strings for query embedding
	char * base_hit_threshold_s   = (char *) BP_SizeTToMySQLBuffer(web_result_disable_base_threshold);
	char * path_hit_threshold_s   = (char *) BP_SizeTToMySQLBuffer(web_result_disable_path_threshold);
	char * script_hit_threshold_s = (char *) BP_SizeTToMySQLBuffer(web_result_disable_script_threshold);

	// link up the converted values for chunk loging
	tq_link(this->tq, base_hit_threshold_s, 4, BPLN);
	tq_link(this->tq, path_hit_threshold_s, 4, BPLN);
	tq_link(this->tq, script_hit_threshold_s, 4, BPLN);

	// now set result/base/path/script
	char * result_id_s   = BP_SizeTToMySQLBuffer(entry->result_id);
	char * base_id_s     = BP_SizeTToMySQLBuffer(entry->base);
	char * path_id_s     = BP_SizeTToMySQLBuffer(entry->path);
	char * script_id_s   = BP_SizeTToMySQLBuffer(entry->script);

	// link in items
	tq_link(this->tq, result_id_s, 4, BPLN);
	tq_link(this->tq, base_id_s, 4, BPLN);
	tq_link(this->tq, path_id_s, 4, BPLN);
	tq_link(this->tq, script_id_s, 4, BPLN);


	// create a new builder
	if(entry->path)
	{

		// create new builder
		BP_StringUtils * builder = new BP_StringUtils(NULL);

		// build update path query
		builder->BuildString((char *) "UPDATE `results` ");
		builder->BuildString((char *) " JOIN `result_path_uris` ON `result_path_uris`.`id` = `results`.`path` ");
		builder->BuildString((char *) " SET `results`.`disabled` = 1 ");
		builder->BuildString((char *) " WHERE `results`.`id` = ");
		builder->BuildString((char *) result_id_s);
		builder->BuildString((char *) " AND `results`.`path` = ");
		builder->BuildString((char *) path_id_s);
		builder->BuildString((char *) " AND `result_path_uris`.`hits` >= ");
		builder->BuildString((char *) path_hit_threshold_s);

		printf("\n Disable Path Query: %s", builder->build_str);

		// prepare update query
		this->mysqlPrepareQuery(builder->build_str);

		// execute statement
		this->mysqlExecuteStatement();

		// destroy statement
		this->mysqlDestroyStatement();

		delete builder;

	}

	// build update base query
	if(entry->base)
	{

		// create new builder
		BP_StringUtils * builder = new BP_StringUtils(NULL);

		// build update base query
		builder->BuildString((char *) "UPDATE `results` ");
		builder->BuildString((char *) " JOIN `result_base_uris` ON `result_base_uris`.`id` = `results`.`base` ");
		builder->BuildString((char *) " SET `results`.`disabled` = 1 ");
		builder->BuildString((char *) " WHERE `results`.`id` = ");
		builder->BuildString((char *) result_id_s);
		builder->BuildString((char *) " AND `results`.`base` = ");
		builder->BuildString((char *) base_id_s);
		builder->BuildString((char *) " AND `result_base_uris`.`hits` >= ");
		builder->BuildString((char *) base_hit_threshold_s);

		// build and execute
		this->mysqlPrepareQuery(builder->build_str);
		this->mysqlExecuteStatement();
		this->mysqlDestroyStatement();
		delete builder;

	}

	// build update script query
	if(entry->script)
	{
		// create new builder
		BP_StringUtils * builder = new BP_StringUtils(NULL);

		// build update script query
		builder->BuildString((char *) "UPDATE `results` ");
		builder->BuildString((char *) " JOIN `result_script_uris` ON `result_script_uris`.`id` = `results`.`script` ");
		builder->BuildString((char *) " SET `results`.`disabled` = 1 ");
		builder->BuildString((char *) " WHERE `results`.`id` = ");
		builder->BuildString((char *) result_id_s);
		builder->BuildString((char *) " AND `results`.`script` = ");
		builder->BuildString((char *) script_id_s);
		builder->BuildString((char *) " AND `result_script_uris`.`hits` >= ");
		builder->BuildString((char *) script_hit_threshold_s);

		// prepare query
		this->mysqlPrepareQuery(builder->build_str);

		// execute statement
		this->mysqlExecuteStatement();

		// destroy statement
		this->mysqlDestroyStatement();

		// destroy builder
		delete builder;

	}


	// free data
	if(result_id_s)
		tq_free(this->tq, result_id_s);
	if(base_id_s)
		tq_free(this->tq, base_id_s);
	if(path_id_s)
		tq_free(this->tq, path_id_s);
	if(script_id_s)
		tq_free(this->tq, script_id_s);
	if(base_hit_threshold_s)
		tq_free(this->tq, base_hit_threshold_s);
	if(path_hit_threshold_s)
		tq_free(this->tq, path_hit_threshold_s);
	if(script_hit_threshold_s)
		tq_free(this->tq, script_hit_threshold_s);

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// create the default webstack parameters
BP_ERROR_T Block::createDefaultWebResultStackParams()
{

	// destroyt the lookup parameters if they already exist
	if(this->web_result_lookup_parms)
		this->destroyWebResultLookupParameters();

	// create lookup parameters
	this->web_result_lookup_parms = (P_BP_WEB_RESULT_STACK_LOOKUP_PARMS) tq_calloc(this->tq, sizeof(BP_WEB_RESULT_STACK_LOOKUP_PARMS), 1, BPLN);

	// ensure we got it
	if(!this->web_result_lookup_parms)
		return ERR_FAILURE;

	// set default values
	this->web_result_lookup_parms->lock_for_audit               = -1;
	this->web_result_lookup_parms->lock_for_spidering           = -1;
	this->web_result_lookup_parms->lock_for_resolution_update   = -1;
	this->web_result_lookup_parms->id                           = -1;
	this->web_result_lookup_parms->assessment_id                = -1;
	this->web_result_lookup_parms->type                         = -1;
	this->web_result_lookup_parms->sha1                         = NULL;
	this->web_result_lookup_parms->href_literal                 = NULL;
	this->web_result_lookup_parms->href_like                    = NULL;
	this->web_result_lookup_parms->href_regexp                  = NULL;
	this->web_result_lookup_parms->host                         = -1;
	this->web_result_lookup_parms->host_distinct                = 0;
	this->web_result_lookup_parms->base                         = -1;
	this->web_result_lookup_parms->base_distinct                = 0;
	this->web_result_lookup_parms->path                         = -1;
	this->web_result_lookup_parms->path_distinct                = 0;
	this->web_result_lookup_parms->script                       = -1;
	this->web_result_lookup_parms->script_distinct              = 0;
	this->web_result_lookup_parms->result_is_base               = BP_FALSE;
	this->web_result_lookup_parms->result_is_path               = BP_FALSE;
	this->web_result_lookup_parms->result_is_script             = BP_FALSE;
	this->web_result_lookup_parms->status                       = -1;
	this->web_result_lookup_parms->update_resolution_status     = -1;
	this->web_result_lookup_parms->origin_type_form_or_anchor_code = -1;
	this->web_result_lookup_parms->commit_ts_ge                 = NULL;
	this->web_result_lookup_parms->commit_ts_le                 = NULL;
	this->web_result_lookup_parms->audit_start_ts_ge            = NULL;
	this->web_result_lookup_parms->audit_start_ts_le            = NULL;
	this->web_result_lookup_parms->audit_completed_ts_ge        = NULL;
	this->web_result_lookup_parms->audit_completed_ts_le        = NULL;
	this->web_result_lookup_parms->spidered                     = -1;
	this->web_result_lookup_parms->spidered_parent_result_id    = -1;
	this->web_result_lookup_parms->spidered_ts_ge               = NULL;
	this->web_result_lookup_parms->spidered_ts_le               = NULL;
	this->web_result_lookup_parms->hits                         = -1;
	this->web_result_lookup_parms->hits_ge                      = -1;
	this->web_result_lookup_parms->hits_le                      = -1;
	this->web_result_lookup_parms->disabled                     = -1;
	this->web_result_lookup_parms->disable_code                 = -1;
	this->web_result_lookup_parms->disabled_host                = -1;
	this->web_result_lookup_parms->disabled_host_code           = -1;
	this->web_result_lookup_parms->disabled_path                = -1;
	this->web_result_lookup_parms->disabled_path_code           = -1;
	this->web_result_lookup_parms->disabled_base                = -1;
	this->web_result_lookup_parms->disabled_base_code           = -1;
	this->web_result_lookup_parms->disabled_script              = -1;
	this->web_result_lookup_parms->disabled_script_code         = -1;
	this->web_result_lookup_parms->limit_bottom                 = 0;
	this->web_result_lookup_parms->limit_top                    = 500;
	this->web_result_lookup_parms->randomize_select_ids         = 0;
	this->web_result_lookup_parms->join_injection_sql           = NULL;
	this->web_result_lookup_parms->where_injection_sql          = NULL;

	// return indicating success
	return ERR_SUCCESS;

}

// update the web result stack with new values.  Results are pulled
// by looking into this->web_result_lookup_parms and running lookups
// from there.
BP_ERROR_T Block::freshenWebResultStackFromParams()
{

	// ensure we have a msyql session to work with
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;


	// ensure we have result lookup parameters
	if(!this->web_result_lookup_parms)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// destroy data upfront
	this->destroyWebResultStack();
	this->destroyWebResultStackWhereClauses();

	// fetch the result stack (aka freshen)
	BP_ERROR_T ret_val = this->fetchWebResultStackFromDB
	(
		this->web_result_lookup_parms->lock_for_audit,
		this->web_result_lookup_parms->lock_for_spidering,
		this->web_result_lookup_parms->lock_for_resolution_update,
		this->web_result_lookup_parms->id,
		this->web_result_lookup_parms->assessment_id,
		this->web_result_lookup_parms->type,
		this->web_result_lookup_parms->sha1,
		this->web_result_lookup_parms->href_literal,
		this->web_result_lookup_parms->href_like,
		this->web_result_lookup_parms->href_regexp,
		this->web_result_lookup_parms->host,
		this->web_result_lookup_parms->host_distinct,
		this->web_result_lookup_parms->base,
		this->web_result_lookup_parms->base_distinct,
		this->web_result_lookup_parms->path,
		this->web_result_lookup_parms->path_distinct,
		this->web_result_lookup_parms->script,
		this->web_result_lookup_parms->script_distinct,
		this->web_result_lookup_parms->result_is_host,
		this->web_result_lookup_parms->result_is_base,
		this->web_result_lookup_parms->result_is_path,
		this->web_result_lookup_parms->result_is_script,
		this->web_result_lookup_parms->status,
		this->web_result_lookup_parms->update_resolution_status,
		this->web_result_lookup_parms->origin_type_form_or_anchor_code,
		this->web_result_lookup_parms->commit_ts_ge,
		this->web_result_lookup_parms->commit_ts_le,
		this->web_result_lookup_parms->audit_start_ts_ge,
		this->web_result_lookup_parms->audit_start_ts_le,
		this->web_result_lookup_parms->audit_completed_ts_ge,
		this->web_result_lookup_parms->audit_completed_ts_le,
		this->web_result_lookup_parms->spidered,
		this->web_result_lookup_parms->spidered_parent_result_id,
		this->web_result_lookup_parms->spidered_ts_ge,
		this->web_result_lookup_parms->spidered_ts_le,
		this->web_result_lookup_parms->hits,
		this->web_result_lookup_parms->hits_ge,
		this->web_result_lookup_parms->hits_le,
		this->web_result_lookup_parms->disabled,
		this->web_result_lookup_parms->disable_code,
		this->web_result_lookup_parms->disabled_path,
		this->web_result_lookup_parms->disabled_path_code,
		this->web_result_lookup_parms->disabled_host,
		this->web_result_lookup_parms->disabled_host_code,
		this->web_result_lookup_parms->disabled_base,
		this->web_result_lookup_parms->disabled_base_code,
		this->web_result_lookup_parms->disabled_script,
		this->web_result_lookup_parms->disabled_script_code,
		this->web_result_lookup_parms->limit_bottom,
		this->web_result_lookup_parms->limit_top,
		this->web_result_lookup_parms->randomize_select_ids,
		this->web_result_lookup_parms->join_injection_sql,
		this->web_result_lookup_parms->where_injection_sql
	);

	// return from function
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ret_val);

}

// destroy lookup parameters
BP_ERROR_T Block::destroyWebResultLookupParameters()
{

	P_BP_WEB_RESULT_STACK_LOOKUP_PARMS l = NULL;

	if(this->web_result_lookup_parms)
	{

		// set lookup
		l = this->web_result_lookup_parms;

		if(l->join_injection_sql)
			tq_free(this->tq, l->join_injection_sql);

		if(l->where_injection_sql)
			tq_free(this->tq, l->where_injection_sql);

		if(l->spidered_ts_ge)
			tq_free(this->tq, l->spidered_ts_ge);

		if(l->spidered_ts_le)
			tq_free(this->tq, l->spidered_ts_le);

		if(l->commit_ts_ge)
			tq_free(this->tq, l->commit_ts_ge);

		if(l->commit_ts_le)
			tq_free(this->tq, l->commit_ts_le);

		if(l->audit_start_ts_ge)
			tq_free(this->tq, l->audit_start_ts_ge);

		if(l->audit_start_ts_le)
			tq_free(this->tq, l->audit_start_ts_le);

		if(l->audit_completed_ts_ge)
			tq_free(this->tq, l->audit_completed_ts_ge);

		if(l->audit_completed_ts_le)
			tq_free(this->tq, l->audit_completed_ts_le);

		if(l->sha1)
			tq_free(this->tq, l->sha1);

		if(l->href_literal)
			tq_free(this->tq, l->href_literal);

		if(l->href_like)
			tq_free(this->tq, l->href_like);

		if(l->href_regexp)
			tq_free(this->tq, l->href_regexp);

		// destroy the lookup parameters
		tq_free(this->tq, this->web_result_lookup_parms);

		// set lookup parameters to null
		this->web_result_lookup_parms = NULL;

	}

	// return indicating success
	return ERR_SUCCESS;

}


// attempts to create the local stored lookups
BP_ERROR_T Block::createWebResultStoredLookups
(
		_vin size_t lock_for_audit,
		_vin size_t lock_for_spidering,
		_vin size_t lock_for_resolution_update,
		_vdbin size_t id,                        /* id = conditional */
		_vdbin size_t assessment_id,             /* assessment_id = conditional */
		_vdbin size_t type,                      /* type = conditional */
		_vdbin char * sha1,                      /* sha1 = str_conditional */
		_vdbin char * href_literal,              /* literal href match (can be null) */
		_vdbin char * href_like,                 /* LIKE href match condition (can be null) */
		_vdbin char * href_regexp,               /* REGEXP href match condition (can be null) */
		_vdbin size_t host,              		 /* associated host id */
		_vdbin size_t host_distinct,             /* wraps select host in a DISTINCT() clause */
		_vdbin size_t base,              		 /* associated base id */
		_vdbin size_t base_distinct,             /* wraps select base in a DISTINCT() clause */
		_vdbin size_t path,             		 /* associated path id */
		_vdbin size_t path_distinct,             /* wraps select base in a DISTINCT() clause */
		_vdbin size_t script,            		 /* associated script id */
		_vdbin size_t script_distinct,           /* wraps select base in a DISTINCT() clause */
		_vdbin size_t result_is_host,            /* joins on the result_hosts table, returning a host */
		_vdbin size_t result_is_base,            /* joins on the result_base_uris table, returning a base instead of a the value in the results table */
		_vdbin size_t result_is_path,            /* joins on the path table, returning a path instead of a the value in the results table */
		_vdbin size_t result_is_script,          /* joins on the script table, returning a script instead of a the value in the results table */
		_vdbin size_t status,                    /* general audit status condition */
		_vdbin size_t update_resolution_status,  /* when a record is being resolved, this flag locks the record */
		_vdbin size_t origin_type_form_or_anchor_code, /* hard code from blockparty indicating what type of anchor/form or whatever combination this was built from */
		_vdbin char * commit_ts_ge,              /* commit timestame greater equal */
		_vdbin char * commit_ts_le,              /* commit_timestamp less than equal to */
		_vdbin char * audit_start_ts_ge,         /* audit start greater equal */
		_vdbin char * audit_start_ts_le,         /* audit start less than equal */
		_vdbin char * audit_completed_ts_ge,     /* audit completed greater equal */
		_vdbin char * audit_completed_ts_le,     /* audit completed less than equal */
		_vdbin size_t spidered,                  /* spidered status */
		_vdbin size_t spidered_parent_result_id, /* parent id of entry */
		_vdbin char * spidered_ts_ge,            /* spidered ts greater equal */
		_vdbin char * spidered_ts_le ,           /* spidered ts less equal */
		_vdbin size_t hits,                      /* literal hit count (-1 to disable)*/
		_vdbin size_t hits_ge,                   /* hits greater than or equal this value */
		_vdbin size_t hits_le,                   /* hits less than or equal this value */
		_vdbin size_t disabled,                  /* disabled match */
		_vdbin size_t disable_code,              /* code indicating why the record was disabled */
		_vdbin size_t disabled_path,             /* disabled path match */
		_vdbin size_t disabled_path_code,        /* disabled path code match */
		_vdbin size_t disabled_host,             /* disabled host match */
		_vdbin size_t disabled_host_code,        /* disabled_host_code */
		_vdbin size_t disabled_base,             /* disabled base match */
		_vdbin size_t disabled_base_code,        /* disabled_base_code */
		_vdbin size_t disabled_script,           /* disabled script match */
		_vdbin size_t disabled_script_code,      /* disabled script code */
		_vdbin size_t limit_bottom,              /* limit bottom, top */
		_vdbin size_t limit_top,                 /* limit bottom, top */
		_vin   size_t randomize_select_ids,      /* set to non-zero to return random ids */
		_vdbin char * join_injection_sql,        /* this is direct SQL suitable for the JOIN position (dangerous) */
		_vdbin char * where_injection_sql        /* direct sql in the WHERE position.  be careful */
)
{

	// destroy parameters if already set
	this->destroyWebResultLookupParameters();

	// if the parameters haven't been destroyed, we have an error condition
	if(this->web_result_lookup_parms)
		return ERR_FAILURE;

	// allocate space
	this->web_result_lookup_parms = (P_BP_WEB_RESULT_STACK_LOOKUP_PARMS) tq_calloc(this->tq, sizeof(BP_WEB_RESULT_STACK_LOOKUP_PARMS), 1, BPLN);

	// ensure we have created memory ok
	if(!this->web_result_lookup_parms)
		return ERR_FAILURE;


	// ======= Begin Fill In All Parameters ========================

	this->web_result_lookup_parms->lock_for_spidering         = lock_for_spidering;
	this->web_result_lookup_parms->lock_for_resolution_update = lock_for_resolution_update;
	this->web_result_lookup_parms->id                         = id;
	this->web_result_lookup_parms->assessment_id              = assessment_id;
	this->web_result_lookup_parms->type                       = type;
	this->web_result_lookup_parms->sha1                       = ( char *) tq_strdup(this->tq, sha1, BPLN);
	this->web_result_lookup_parms->href_literal               = ( char *) tq_strdup(this->tq, href_literal, BPLN);
	this->web_result_lookup_parms->href_like                  = ( char *) tq_strdup(this->tq, href_like, BPLN);
	this->web_result_lookup_parms->href_regexp                = ( char *) tq_strdup(this->tq, href_regexp, BPLN);
	this->web_result_lookup_parms->base                       = host;
	this->web_result_lookup_parms->base_distinct              = host_distinct;
	this->web_result_lookup_parms->base                       = base;
	this->web_result_lookup_parms->base_distinct              = base_distinct;
	this->web_result_lookup_parms->path                       = path;
	this->web_result_lookup_parms->path_distinct              = path_distinct;
	this->web_result_lookup_parms->script                     = script;
	this->web_result_lookup_parms->script_distinct            = script_distinct;
	this->web_result_lookup_parms->result_is_host             = result_is_host;
	this->web_result_lookup_parms->result_is_base             = result_is_base;
	this->web_result_lookup_parms->result_is_path             = result_is_path;
	this->web_result_lookup_parms->result_is_script           = result_is_script;
	this->web_result_lookup_parms->status                     = status;
	this->web_result_lookup_parms->update_resolution_status   = update_resolution_status;
	this->web_result_lookup_parms->origin_type_form_or_anchor_code = origin_type_form_or_anchor_code;
	this->web_result_lookup_parms->commit_ts_ge               = ( char *) tq_strdup(this->tq, commit_ts_ge, BPLN);
	this->web_result_lookup_parms->commit_ts_le               = ( char *) tq_strdup(this->tq, commit_ts_le, BPLN);
	this->web_result_lookup_parms->audit_start_ts_ge          = ( char *) tq_strdup(this->tq, audit_start_ts_ge, BPLN);
	this->web_result_lookup_parms->audit_start_ts_le          = ( char *) tq_strdup(this->tq, audit_start_ts_le, BPLN);
	this->web_result_lookup_parms->audit_completed_ts_ge      = ( char *) tq_strdup(this->tq, audit_completed_ts_ge, BPLN);
	this->web_result_lookup_parms->audit_completed_ts_le      = ( char *) tq_strdup(this->tq, audit_completed_ts_le, BPLN);
	this->web_result_lookup_parms->spidered                   = spidered;
	this->web_result_lookup_parms->spidered_parent_result_id = spidered_parent_result_id;
	this->web_result_lookup_parms->spidered_ts_ge            = ( char *) tq_strdup(this->tq, spidered_ts_ge, BPLN);
	this->web_result_lookup_parms->spidered_ts_le            = ( char *) tq_strdup(this->tq, spidered_ts_le, BPLN);
	this->web_result_lookup_parms->hits                      = hits;
	this->web_result_lookup_parms->hits_ge                   = hits_ge;
	this->web_result_lookup_parms->hits_le                   = hits_le;
	this->web_result_lookup_parms->disabled                  = disabled;
	this->web_result_lookup_parms->disable_code              = disable_code;
	this->web_result_lookup_parms->disabled_path             = disabled_path;
	this->web_result_lookup_parms->disabled_path_code        = disabled_path_code;
	this->web_result_lookup_parms->disabled_base             = disabled_host;
	this->web_result_lookup_parms->disabled_base_code        = disabled_host_code;
	this->web_result_lookup_parms->disabled_base             = disabled_base;
	this->web_result_lookup_parms->disabled_base_code        = disabled_base_code;
	this->web_result_lookup_parms->disabled_script           = disabled_script;
	this->web_result_lookup_parms->disabled_script_code      = disabled_script_code;
	this->web_result_lookup_parms->limit_bottom              = limit_bottom;
	this->web_result_lookup_parms->limit_top                 = limit_top;
	this->web_result_lookup_parms->randomize_select_ids      = randomize_select_ids;
	this->web_result_lookup_parms->join_injection_sql        = ( char *) tq_strdup(this->tq, join_injection_sql, BPLN);
	this->web_result_lookup_parms->where_injection_sql       = ( char *) tq_strdup(this->tq, where_injection_sql, BPLN);


	// ======= End Parameter Fill ==================================

	// return indicating success
	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %% Main Web Result Fetch routine %%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// pulls web results from db based on parameters
BP_ERROR_T Block::fetchWebResultStackFromDB
(
	_vin size_t lock_for_audit,
	_vin size_t lock_for_spidering,
	_vin size_t lock_for_resolution_update,
	_vdbin size_t id,                        /* id = conditional */
	_vdbin size_t assessment_id,             /* assessment_id = conditional */
	_vdbin size_t type,                      /* type = conditional */
	_vdbin char * sha1,                      /* sha1 = str_conditional */
	_vdbin char * href_literal,              /* literal href match (can be null) */
	_vdbin char * href_like,                 /* LIKE href match condition (can be null) */
	_vdbin char * href_regexp,               /* REGEXP href match condition (can be null) */
	_vdbin size_t host,              		 /* associated host id */
	_vdbin size_t host_distinct,             /* wraps select host in a DISTINCT() clause */
	_vdbin size_t base,              		 /* associated base id */
	_vdbin size_t base_distinct,             /* wraps select base in a DISTINCT() clause */
	_vdbin size_t path,             		 /* associated path id */
	_vdbin size_t path_distinct,             /* wraps select base in a DISTINCT() clause */
	_vdbin size_t script,            		 /* associated script id */
	_vdbin size_t script_distinct,           /* wraps select base in a DISTINCT() clause */
	_vdbin size_t result_is_host,            /* joins on the result_hosts table, returning a host */
	_vdbin size_t result_is_base,            /* joins on the result_base_uris table, returning a base instead of a the value in the results table */
	_vdbin size_t result_is_path,            /* joins on the path table, returning a path instead of a the value in the results table */
	_vdbin size_t result_is_script,          /* joins on the script table, returning a script instead of a the value in the results table */
	_vdbin size_t status,                    /* general audit status condition */
	_vdbin size_t update_resolution_status,  /* when a record is being resolved, this flag locks the record */
	_vdbin size_t origin_type_form_or_anchor_code, /* hard code from blockparty indicating what type of anchor/form or whatever combination this was built from */
	_vdbin char * commit_ts_ge,              /* commit timestame greater equal */
	_vdbin char * commit_ts_le,              /* commit_timestamp less than equal to */
	_vdbin char * audit_start_ts_ge,         /* audit start greater equal */
	_vdbin char * audit_start_ts_le,         /* audit start less than equal */
	_vdbin char * audit_completed_ts_ge,     /* audit completed greater equal */
	_vdbin char * audit_completed_ts_le,     /* audit completed less than equal */
	_vdbin size_t spidered,                  /* spidered status */
	_vdbin size_t spidered_parent_result_id, /* parent id of entry */
	_vdbin char * spidered_ts_ge,            /* spidered ts greater equal */
	_vdbin char * spidered_ts_le ,           /* spidered ts less equal */
	_vdbin size_t hits,                      /* literal hit count (-1 to disable)*/
	_vdbin size_t hits_ge,                   /* hits greater than or equal this value */
	_vdbin size_t hits_le,                   /* hits less than or equal this value */
	_vdbin size_t disabled,                  /* disabled match */
	_vdbin size_t disable_code,              /* code indicating why the record was disabled */
	_vdbin size_t disabled_path,             /* disabled path match */
	_vdbin size_t disabled_path_code,        /* disabled path code match */
	_vdbin size_t disabled_host,             /* disabled host match */
	_vdbin size_t disabled_host_code,        /* disabled_host_code */
	_vdbin size_t disabled_base,             /* disabled base match */
	_vdbin size_t disabled_base_code,        /* disabled_base_code */
	_vdbin size_t disabled_script,           /* disabled script match */
	_vdbin size_t disabled_script_code,      /* disabled script code */
	_vdbin size_t limit_bottom,              /* limit bottom, top */
	_vdbin size_t limit_top,                 /* limit bottom, top */
	_vin   size_t randomize_select_ids,      /* set to non-zero to return random ids */
	_vdbin char * join_injection_sql,        /* this is direct SQL suitable for the JOIN position (dangerous) */
	_vdbin char * where_injection_sql        /* direct sql in the WHERE position.  be careful */
)
{

	// this routine requires a valid mysql connection
	if(!this->mysqlSession)
	{
		this->AddErrorToStack("Web result retrieval requires valid mysql connection.");
		return ERR_FAILURE;
	}

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// destroy existing results if already set for whatever reason
	this->destroyWebResultStack();

	// destroy statement (free memory etc if needed)
	this->mysqlDestroyStatement();


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Save Parameters for Repeat Queries %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	this->createWebResultStoredLookups
	(
			lock_for_audit,
			lock_for_spidering,
			lock_for_resolution_update,
			id,                        /* id = conditional */
			assessment_id,             /* assessment_id = conditional */
			type,                      /* type = conditional */
			sha1,                      /* sha1 = str_conditional */
			href_literal,              /* literal href match (can be null) */
			href_like,                 /* LIKE href match condition (can be null) */
			href_regexp,               /* REGEXP href match condition (can be null) */
			host,              		 /* associated host id */
			host_distinct,             /* wraps select host in a DISTINCT() clause */
			base,              		 /* associated base id */
			base_distinct,             /* wraps select base in a DISTINCT() clause */
			path,             		 /* associated path id */
			path_distinct,             /* wraps select base in a DISTINCT() clause */
			script,            		 /* associated script id */
			script_distinct,           /* wraps select base in a DISTINCT() clause */
			result_is_host,            /* joins on the result_hosts table, returning a host */
			result_is_base,            /* joins on the result_base_uris table, returning a base instead of a the value in the results table */
			result_is_path,            /* joins on the path table, returning a path instead of a the value in the results table */
			result_is_script,          /* joins on the script table, returning a script instead of a the value in the results table */
			status,                    /* general audit status condition */
			update_resolution_status,  /* when a record is being resolved, this flag locks the record */
			origin_type_form_or_anchor_code, /* hard code from blockparty indicating what type of anchor/form or whatever combination this was built from */
			commit_ts_ge,              /* commit timestame greater equal */
			commit_ts_le,              /* commit_timestamp less than equal to */
			audit_start_ts_ge,         /* audit start greater equal */
			audit_start_ts_le,         /* audit start less than equal */
			audit_completed_ts_ge,     /* audit completed greater equal */
			audit_completed_ts_le,     /* audit completed less than equal */
			spidered,                  /* spidered status */
			spidered_parent_result_id, /* parent id of entry */
			spidered_ts_ge,            /* spidered ts greater equal */
			spidered_ts_le ,           /* spidered ts less equal */
			hits,                      /* literal hit count (-1 to disable)*/
			hits_ge,                   /* hits greater than or equal this value */
			hits_le,                   /* hits less than or equal this value */
			disabled,                  /* disabled match */
			disable_code,              /* code indicating why the record was disabled */
			disabled_path,             /* disabled path match */
			disabled_path_code,        /* disabled path code match */
			disabled_host,             /* disabled host match */
			disabled_host_code,        /* disabled_host_code */
			disabled_base,             /* disabled base match */
			disabled_base_code,        /* disabled_base_code */
			disabled_script,           /* disabled script match */
			disabled_script_code,      /* disabled script code */
			limit_bottom,              /* limit bottom, top */
			limit_top,                 /* limit bottom, top */
			randomize_select_ids,      /* set to non-zero to return random ids */
			join_injection_sql,        /* this is direct SQL suitable for the JOIN position (dangerous) */
			where_injection_sql        /* direct sql in the WHERE position.  be careful */
	);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create / Run Queries %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	/*
	// create transaction if we're the
	if((lock_for_audit == -1) || (lock_for_spidering == -1) || (lock_for_resolution_update == -1) )
	{
		this->mysqlPrepareQuery("START TRANSACTION;");
		this->mysqlExecuteStatement();
		this->mysqlDestroyStatement();
	}
	*/

	// create string builder for the purpose of dynamic
	// query generation.
	BP_StringUtils * query_builder = new BP_StringUtils(NULL);

	// exit if we cannot create the builder
	if(!query_builder)
		return ERR_FAILURE;

	// reset the stack count on pull
	this->web_result_stack_n = 0;



	// ==== Build Query ========

	// add query front part
	query_builder->BuildString
	(   (char *)
		" SELECT " \
			"`results`.`id`, "\
			"`results`.`assessment_id`, "\
			"`results`.`type`, "\
			"`results`.`sha1`, ");

	// if we're doing a result_is_base/path/script, we need to perform a replacement here
	if(result_is_host)
	{

		// specify href from host
		query_builder->BuildString((char *) "`result_hosts`.`hostname`, ");

	}
	else if(result_is_base)
	{

			// specify href from base
			query_builder->BuildString((char *) "`result_base_uris`.`base_href`, ");

	}
	else if (result_is_path)
	{

		// specify href from path
		query_builder->BuildString((char *) "`result_path_uris`.`path_href`, ");

	}
	else if(result_is_script)
	{

		// specify href from script
		query_builder->BuildString((char *) "`result_script_uris`.`script_href`, ");

	}
	else
	{
		// default, return href
		query_builder->BuildString((char *) "`results`.`href`, ");
	}


	// add the base id clause
	query_builder->BuildString((char *)	"`results`.`host`, ");

	// add the base id clause
	query_builder->BuildString((char *)	"`results`.`base`, ");

	// ad path id clause
	query_builder->BuildString((char *)	"`results`.`path`, ");

	// add script id clause
	query_builder->BuildString((char *)	"`results`.`script`, ");

	// build remainder of the query
	query_builder->BuildString((char *) "`results`.`status`, "\
			"`results`.`update_resolution_status`, "\
			"`results`.`origin_type_form_or_anchor_code`, "\
			"`results`.`commit_timestamp`, "\
			"`results`.`audit_start_timestamp`, "\
			"`results`.`audit_completed_timestamp`, "\
			"`results`.`spidered`, "\
			"`results`.`spider_parent_result_id`, "\
			"`results`.`spidered_timestamp`, "\
			"`results`.`hits`, "\
			"`results`.`disabled`, "\
			"`results`.`disable_code`, "\
			"`results`.`disabled_path`, "\
			"`results`.`disabled_path_code`, "\
			"`results`.`disabled_host`, "\
			"`results`.`disabled_host_code`, "\
			"`results`.`disabled_base`, "\
			"`results`.`disabled_base_code`, "\
			"`results`.`disabled_script`, "\
			"`results`.`disabled_script_code` "\
		" FROM `results` "
	);

	// add join string where necessary (BE CAREFUL WITH WHAT DATA GETS IN HERE)
	if(join_injection_sql)
	{
		query_builder->BuildString((char *) join_injection_sql);
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create base/path/script Table Joins %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we're doing a result_is_base/path/script, we need to perform a replacement here
	if (result_is_host)
	{

		// specify href from base
		query_builder->BuildString((char *) " JOIN `result_hosts` ON `result_hosts`.`id` = `results`.`host` ");

	}
	else if(result_is_base)
	{

		// specify href from base
		query_builder->BuildString((char *) " JOIN `result_base_uris` ON `result_base_uris`.`id` = `results`.`base` ");

	}
	else if (result_is_path)
	{

		// specify href from path
		query_builder->BuildString((char *) " JOIN `result_path_uris` ON `result_path_uris`.`id` = `results`.`path` ");

	}
	else if(result_is_script)
	{

		// specify href from base
		query_builder->BuildString((char *) " JOIN `result_script_uris` ON `result_script_uris`.`script_id` = `results`.`script` ");

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%% OPEN SUB SELECT CLAUSE %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// MODIFIED 2015 to remove nested select (SINCE ITS AN OUTRAGEOUS COST WHEN
	// YOUR DB IS > 5M RECORDS)

	/*
	// add where 1=1 clause (all records)
	query_builder->BuildString((char *) " WHERE `results`.`id` IN (SELECT `results`.`id` FROM `results` WHERE 1=1 ");

	// add direct sql if so specified (dangerous)
	if(where_injection_sql)
	{
		query_builder->BuildString((char *) where_injection_sql);
	}
	*/


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Force Indexes if Necessary for Group By %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Constraints %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add basic where clause
	query_builder->BuildString((char *) "WHERE ");

	// determines if we should use a prefix AND to the builder
	BP_BOOL add_and = BP_FALSE;

	// id constraint
	if(id != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`id` = ");
		query_builder->BuildMySQLHexFromSizeT(id);
	}

	// assessment id constraint
	if(assessment_id != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`assessment_id` = ");
		query_builder->BuildMySQLHexFromSizeT(assessment_id);
	}

	// record type constraint
	if(type != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`type` = ");
		query_builder->BuildMySQLHexFromSizeT(type);
	}

	// sha1 constraint
	if(sha1 != NULL)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`sha1` =  ");
		query_builder->BuildMySQLHexFromBuff(sha1, bpstrlen(sha1));
	}

	// href literal constraint
	if(href_literal != NULL)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`href` = ");
		query_builder->BuildMySQLHexFromBuff(href_literal, bpstrlen(href_literal));
	}

	// href like constraint
	if(href_like  != NULL)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`href` LIKE ");
		query_builder->BuildMySQLHexFromBuff(href_like, bpstrlen(href_like));

	}

	// href regexp constraint
	if(href_regexp != NULL)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`href` REGEXP ");
		query_builder->BuildMySQLHexFromBuff(href_regexp, bpstrlen(href_regexp));

	}

	// base literal constraint
	if(host != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`host` = ");
		query_builder->BuildMySQLHexFromSizeT(host);

	}

	// base literal constraint
	if(base != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`base` = ");
		query_builder->BuildMySQLHexFromSizeT(base);

	}

	// path literal constraint
	if(path != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`path` = ");
		query_builder->BuildMySQLHexFromSizeT(path);

	}

	// script constraint
	if(script != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`script` = ");
		query_builder->BuildMySQLHexFromSizeT(script);

	}

	// status constraint
	if(status != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`status` = ");
		query_builder->BuildMySQLHexFromSizeT(status);
	}

	// status constraint
	if(update_resolution_status != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`update_resolution_status` = ");
		query_builder->BuildMySQLHexFromSizeT(update_resolution_status);

	}

	// status constraint
	if(origin_type_form_or_anchor_code != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`origin_type_form_or_anchor_code` = ");
		query_builder->BuildMySQLHexFromSizeT(origin_type_form_or_anchor_code);

	}

	// commit timestamp >=
	if(commit_ts_ge != NULL)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`commit_timestamp` >= ");
		query_builder->BuildMySQLHexFromBuff(commit_ts_ge, bpstrlen(commit_ts_ge));

	}

	// commit timestamp <=
	if(commit_ts_le != NULL)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`commit_timestamp` <= ");
		query_builder->BuildMySQLHexFromBuff(commit_ts_le, bpstrlen(commit_ts_le));
	}

	// audit start timestamp >=
	if(audit_start_ts_ge != NULL)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`audit_start_timestamp` >= ");
		query_builder->BuildMySQLHexFromBuff(audit_start_ts_ge, bpstrlen(audit_start_ts_ge));
	}

	// audit start timestamp <=
	if(audit_start_ts_le != NULL)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`audit_start_timestamp` <= ");
		query_builder->BuildMySQLHexFromBuff(audit_start_ts_le, bpstrlen(audit_start_ts_le));

	}

	// audit completed timestamp >=
	if(audit_completed_ts_ge != NULL)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`audit_completed_timestamp` >= ");
		query_builder->BuildMySQLHexFromBuff(audit_completed_ts_ge, bpstrlen(audit_completed_ts_ge));

	}

	// audit completed timestamp <=
	if(audit_completed_ts_le != NULL)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`audit_completed_timestamp` <= ");
		query_builder->BuildMySQLHexFromBuff(audit_completed_ts_le, bpstrlen(audit_completed_ts_le));

	}

	// spidered flag
	if(spidered != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`spidered` = ");
		query_builder->BuildMySQLHexFromSizeT(spidered);

	}

	// spidered parent result id
	if(spidered_parent_result_id != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`spider_parent_result_id` = ");
		query_builder->BuildMySQLHexFromSizeT(spidered_parent_result_id);

	}

	// spidered timestamp >=
	if(spidered_ts_ge != NULL)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`spidered_timestamp` >= ");
		query_builder->BuildMySQLHexFromBuff(spidered_ts_ge, bpstrlen(spidered_ts_ge));

	}

	// spidered timestamp <=
	if(spidered_ts_le != NULL)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`spidered_timestamp` >= ");
		query_builder->BuildMySQLHexFromBuff(spidered_ts_le, bpstrlen(spidered_ts_le));

	}

	// add hits constraint set
	if(hits != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`hits` = ");
		query_builder->BuildMySQLHexFromSizeT(hits);
	}

	if(hits_ge != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`hits` >= ");
		query_builder->BuildMySQLHexFromSizeT(hits_ge);
	}

	if(hits_le != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`hits` <= ");
		query_builder->BuildMySQLHexFromSizeT(hits_le);
	}

	// disabled sets
	if(disabled != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`disabled` = ");
		query_builder->BuildMySQLHexFromSizeT(disabled);
	}

	if(disable_code != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`disable_code` = ");
		query_builder->BuildMySQLHexFromSizeT(disable_code);

	}

	// disable path
	if(disabled_path != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`disabled_path` = ");
		query_builder->BuildMySQLHexFromSizeT(disabled_path);
	}

	// disabled path code
	if(disabled_path_code != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");


		query_builder->BuildString(" `results`.`disabled_path_code` = ");
		query_builder->BuildMySQLHexFromSizeT(disabled_path_code);
	}

	// disable host constraints
	if(disabled_host != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`disabled_host` = ");
		query_builder->BuildMySQLHexFromSizeT(disabled_host);
	}


	if(disabled_host_code != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`disabled_host_code` = ");
		query_builder->BuildMySQLHexFromSizeT(disabled_host_code);
	}

	// disable base constraints
	if(disabled_base != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`disabled_base` = ");
		query_builder->BuildMySQLHexFromSizeT(disabled_base);

	}


	if(disabled_base_code != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`disabled_base_code` = ");
		query_builder->BuildMySQLHexFromSizeT(disabled_base_code);
	}

	// disable script constraints
	if(disabled_script != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`disabled_script` =  ");
		query_builder->BuildMySQLHexFromSizeT(disabled_script);
	}

	if(disabled_script_code != -1)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		query_builder->BuildString(" `results`.`disabled_script_code` = ");
		query_builder->BuildMySQLHexFromSizeT(disabled_script_code);
	}

	// add randomization if necessary (fast despite the large number of queries)
	if(randomize_select_ids)
	{

		// set add flag if not needed, and build if it is
		if(!add_and)
			add_and = BP_TRUE;
		else
			query_builder->BuildString(" AND ");

		// stack buffer for converting
		char out_buff[128] = {0};

		// add query front part
		query_builder->BuildString("  `results`.`id` >= ");

		// get random base (DO NOT LOOP THIS AND CREATE AN 'IN () '.  We tried that
		// and it was too slow.  Don't do it over).  Random base is fastest as it's
		// a subselction rather than a total division.
		size_t random_base_index = this->retrieveRandomResultDBIndexFastFind(-1, -1);

		// set base
		sprintf(out_buff, "%zu", random_base_index);

		// append base
		query_builder->BuildString((char *) &out_buff);

		/*
		// increase the base by the top of the limit
		random_base_index += limit_top;
		memset((void *)&out_buff, 0x00, sizeof(out_buff));
		sprintf(out_buff, "%zu", random_base_index);

		// now lock in the results
		query_builder->BuildString((char *) " AND `results`.`id` <= ");
		query_builder->BuildString((char *) &out_buff);
		*/
	}

	// NOTE: removed subselect 2015, don't uncomment.
	// terminate the whole subselect here
	// query_builder->BuildString(") ");

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% JOIN On Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%% GROUPBY/DISTINCT %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// IMPORTANT INFORMATION ABOUT GROUP BY:
	// Group by will cause queries to become VERY slow if used with an
	// order by clause.  Cannot use both.  Has to be one or the other.

	// flag indicating if group by was set
	size_t group_by_set = 0;

	// check base
	if(host_distinct)
	{
		query_builder->BuildString((char *)	" GROUP BY (`results`.`host`) ");
		group_by_set=1;
	}

	// check base
	if(base_distinct  && !group_by_set)
	{
		query_builder->BuildString((char *)	" GROUP BY (`results`.`base`) ");
		group_by_set=1;
	}

	// check path
	if(path_distinct && !group_by_set)
	{
		query_builder->BuildString((char *)	" GROUP BY (`results`.`path`) ");
	}

	// check script
	if(script_distinct && !group_by_set)
	{
		query_builder->BuildString((char *)	" GROUP BY (`results`.`script`) ");
	}




	// order string
	char order_str[256];
	BP_ZERO_STACK(order_str);

	// only add order by if group clause isn't set
	if(!group_by_set)
	{

		// oadd order by clause
		sprintf((char *) &order_str, " ORDER BY `results`.`id` ASC ");

		// add limit constraint (default 500 records)
		query_builder->BuildString((char *) &order_str);

	}

	/// always add limit clause
	char limit_str[256];
	BP_ZERO_STACK(limit_str);
	sprintf((char *) &limit_str, " LIMIT %u, %u", limit_bottom, limit_top);

	// add limit clause
	query_builder->BuildString((char *) &limit_str);







	// add the "FOR UPADATE" locking clause to the query if the
	// lock parameters are any value but -1.  This prevents
	// database contention and should be used wherever possible
	// when writing to shared data sets (such as those found in
	// the results table)
	if((lock_for_audit != -1) || (lock_for_spidering != -1) || (lock_for_resolution_update != -1))
	{
		query_builder->BuildString(" FOR UPDATE; ");
		// query_builder->BuildString(" LOCK IN SHARE MODE; ");
	}

	// UNCOMMENT THIS TO SEE SELECT QUERY
	printf("\n SELECT QUERY For Debug: %s", query_builder->build_str);
	// printf("\n ");

	// ===== Now Prepare and add parameters ========

	// prepare the query
	if(this->mysqlPrepareQuery(query_builder->build_str) != ERR_SUCCESS)
	{

		delete query_builder;
		this->AddErrorToStack("SQL prepare failed when attempting result retrieval.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%% Add Results %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// allocate new proxy result value
	BP_WEB_RESULT_ENTRY result = {0};
	memset((void *)&result, 0x00, sizeof(result));

	// add result entries
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.result_id,                   sizeof(result.result_id));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.assessment_id,               sizeof(result.assessment_id));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.type,                        sizeof(result.type));
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &result.sha1,                        sizeof(result.sha1));
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &result.href,                        sizeof(result.href));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.host,                 	      sizeof(result.host));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.base,                 	      sizeof(result.base));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.path,                	      sizeof(result.path));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.script,                	  sizeof(result.script));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.status,                      sizeof(result.status));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.update_resolution_status,    sizeof(result.update_resolution_status));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.origin_type_form_or_anchor_code, sizeof(result.origin_type_form_or_anchor_code));
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &result.commit_timestamp,            sizeof(result.commit_timestamp));
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &result.audit_start_timestamp,       sizeof(result.audit_start_timestamp));
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &result.audit_completed_timestamp,   sizeof(result.audit_completed_timestamp));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.spidered,                    sizeof(result.spidered));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.spidered_parent_result_id,   sizeof(result.spidered_parent_result_id));
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &result.spidered_timestamp,          sizeof(result.spidered_timestamp));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.hits,                        sizeof(result.hits));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.disabled,                    sizeof(result.disabled));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.disable_code,                sizeof(result.disable_code));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.disabled_path,               sizeof(result.disabled_path));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.disabled_path_code,          sizeof(result.disabled_path_code));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.disabled_host,               sizeof(result.disabled_host));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.disabled_host_code,          sizeof(result.disabled_host_code));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.disabled_base,               sizeof(result.disabled_base));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.disabled_base_code,          sizeof(result.disabled_base_code));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.disabled_script,             sizeof(result.disabled_script));
	this->mysqlAddResult(MYSQL_TYPE_LONG,   (char *) &result.disabled_script_code,        sizeof(result.disabled_script_code));

	// now bind results
	this->mysqlBindResults();

	// %%%%%%% Execute the SELECT query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// execute the statement
	if(this->mysqlExecuteStatement() != ERR_SUCCESS)
	{
		delete query_builder;
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// store the results of the query
	this->mysqlStoreResults();

	// reset stack count
	this->web_result_stack_n = 0;
	if(this->web_result_stack)
	{
		this->destroyWebResultStack();
	}

	// ensure web result count is before pulling new records
	this->web_result_stack_n = 0;

	// create new linkl for holding result data
	this->web_result_stack_data_allocator = BP_CreateLinkL(".", BP_TRUE);

	// fetch an individual record
	while(this->mysqlFetchResult() == ERR_SUCCESS)
	{

		// increment the array count
		this->web_result_stack_n++;

		// allocate space for the new set
		this->web_result_stack = (P_BP_WEB_RESULT_ENTRY) tq_realloc
		(
				this->web_result_stack_data_allocator,
				(void *) this->web_result_stack, sizeof(BP_WEB_RESULT_ENTRY) * (this->web_result_stack_n+1),
				BPLN
		);


		printf("\n CURRENT WEBSTACK INDEX: %u", this->web_result_stack_n-1);
		printf("\n");

		// 2015 CHANGE: CANNOT SIMPLY COPY IN RESULTS VERBATIM DUE TO DYNAMIC ALLOCATOR

		// copy the retrieved data into class array
		// memcpy((void *) &this->web_result_stack[this->web_result_stack_n-1], (void *) &result, sizeof(result));

		size_t col_iter = 0;


		// add result id
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].result_id, mysqlSession->bind_results_external_ptr[col_iter].buffer, sizeof(this->web_result_stack[this->web_result_stack_n-1].result_id));
		col_iter++;

		// add assessment id
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].assessment_id, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add type
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].type, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add sha1
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].sha1, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add href
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].href, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add host
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].host, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add base
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].base, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add path
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].path, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add script
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].script, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add status
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].status, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add update resolution status
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].update_resolution_status, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add origin type form or anchor
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].origin_type_form_or_anchor_code, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add commit timestamp
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].commit_timestamp, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add audit start timestamp
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].audit_start_timestamp, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add audit completed timestamp
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].audit_completed_timestamp, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add spidered timestamp
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].spidered, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add spidered parent result id
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].spidered_parent_result_id, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add spidered timestamp
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].spidered_timestamp, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add hits
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].hits, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add disabled
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].disabled, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add disabled code
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].disable_code, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add disabled path
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].disabled_path, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add disabled path code
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].disabled_path_code, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add disabled host
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].disabled_host, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add disabled host code
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].disabled_host_code, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add disabled base
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].disabled_base, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add disabled base code
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].disabled_base_code, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add disabled script
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].disabled_script, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// add disabled script code
		memcpy(&this->web_result_stack[this->web_result_stack_n-1].disabled_script_code, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		/**/
		// zero out the result structure before loop (new record)
		memset((void *)&result, 0x00, sizeof(result));

	}

	// display webstack result count
	printf("\n [+] Webstack Result Count: %u", this->web_result_stack_n);

	// destroy the query builder
	delete query_builder;


	// exit here if we have no locks to update in
	if((lock_for_audit == -1) && (lock_for_spidering == -1) && (lock_for_resolution_update == -1))
	{

		// return indicating success
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%% Now Update If necessary %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new query builder for update query.  Technically this
	// code could've been done in the result addition logic above, but
	// it was causing the code to look obfuscated, which is undesireable.
	query_builder = new BP_StringUtils(NULL);

	// small stack buffer to hold formatted text before
	// it is built.
	char index_add_str[128];

	// add the first part of the update query
	query_builder->BuildString((char *) " UPDATE `results` SET ");

	size_t first_val_add = 0;

	// add lock for audit
	if(lock_for_audit != -1)
	{

		// set status
		query_builder->BuildString(" `status`= ");
		query_builder->BuildMySQLHexFromSizeT(lock_for_audit);

	}


	// add lock for spidering
	if(lock_for_spidering != (size_t) -1)
	{

		if(first_val_add)
		{
			query_builder->BuildString((char *) ",");
			first_val_add = 1;
		}

		query_builder->BuildString(" `spidered`= ");
		query_builder->BuildMySQLHexFromSizeT(lock_for_spidering);

	}

	// add lock for resolution
	if(lock_for_resolution_update != (size_t) -1)
	{

		if(first_val_add)
		{
			query_builder->BuildString((char *) ",");
			first_val_add = 1;
		}

		query_builder->BuildString(" `update_resolution_status`= ");
		query_builder->BuildMySQLHexFromSizeT(lock_for_resolution_update);

	}


	// add the constraint to the set
	query_builder->BuildString(" WHERE `id` IN ( ");

	// iterate through each retrieved element and create string
	size_t n = 0;
	for(; n < this->web_result_stack_n; n++)
	{


		// build the string using the result id from array
		query_builder->BuildMySQLHexFromSizeT(this->web_result_stack[n].result_id);

		// add comma where necessary
		if((n+1) < this->web_result_stack_n)
			query_builder->BuildString( (char *) ",");

	}

	// complete the "IN" set
	query_builder->BuildString((char *) ");");


	// display update query
	printf("\n UPDATE QUERY HERE: %s", query_builder->build_str);


	// prepare the query
	if(this->web_result_stack_n)
	if(this->mysqlPrepareQuery(query_builder->build_str) != ERR_SUCCESS)
	{
		delete query_builder;
		this->AddErrorToStack("SQL prepare failed when attempting result update locking.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// execute update statement
	if(this->web_result_stack_n)
	if(this->mysqlExecuteStatement() != ERR_SUCCESS)
	{
		delete query_builder;
		this->AddErrorToStack("SQL execute failed when attempting result update locking.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	/*
	// create transaction if we're the
	if((lock_for_audit == -1) || (lock_for_spidering == -1) || (lock_for_resolution_update == -1) )
	{
		this->mysqlPrepareQuery("COMMIT;");
		this->mysqlExecuteStatement();
		this->mysqlDestroyStatement();
	}
	*/

	// destroy the query builder where necessary
	if(query_builder)
		delete query_builder;

	// destroy statement before exit
	this->mysqlDestroyStatement();

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// this will disable web result stack entries which have a configuration that matches the configured thresholds
// (query is very fast due to multiple indexing).
BP_ERROR_T Block::disableWebResultStackRecords(size_t base_hit_threshold, size_t path_hit_threshold, size_t script_hit_threshold)
{

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter db critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// only work when we have a result stack to  process
	if(!this->web_result_stack_n)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);


	// base hit threshold
	char * base_hit_threshold_s   = BP_SizeTToMySQLBuffer(base_hit_threshold);

	// path hi treshold
	char * path_hit_threshold_s   = BP_SizeTToMySQLBuffer(path_hit_threshold);

	// script hit threshold
	char * script_hit_threshold_s = BP_SizeTToMySQLBuffer(script_hit_threshold);



	// create where clauses
	this->webResultStackWhereClauses();

	// create a new builder
	BP_StringUtils * builder = NULL;


	if(this->path_n)
	{

		// create new builder
		builder = new BP_StringUtils(NULL);

		// build update path query
		builder->BuildString((char *) "UPDATE `results` SET `disabled` = 1 ");
		builder->BuildString((char *) " JOIN `result_path_uris` ON ");
		builder->BuildString((char *) this->results_in_path_clause);
		builder->BuildString((char *) " WHERE ");
		builder->BuildString((char *) this->results_in_id_clause);
		builder->BuildString((char *) " AND `result_path_uris`.`hits` >= ");
		builder->BuildString((char *) path_hit_threshold_s);

		// build and execute
		this->mysqlPrepareQuery(builder->build_str);
		this->mysqlExecuteStatement();
		this->mysqlDestroyStatement();
		delete builder;

	}


	// build update base query
	if(this->base_n)
	{
		builder = new BP_StringUtils(NULL);
		builder->BuildString((char *) "UPDATE `results` SET `disabled` = 1, `disabled_base` = 1 JOIN `result_base_uris` ON ");
		builder->BuildString((char *) this->results_in_base_clause);
		builder->BuildString((char *) " WHERE ");
		builder->BuildString((char *) this->results_in_id_clause);
		builder->BuildString((char *) " AND `result_base_uris`.`hits` >= ");
		builder->BuildString((char *) base_hit_threshold_s);

		// build and execute
		this->mysqlPrepareQuery(builder->build_str);
		this->mysqlExecuteStatement();
		this->mysqlDestroyStatement();
		delete builder;

	}

	// build update script query
	if(this->script_n)
	{
		builder = new BP_StringUtils(NULL);
		builder->BuildString((char *) "UPDATE `results` SET `disabled` = 1, `disabled_script` = 1 JOIN `result_script_uris` ON ");
		builder->BuildString((char *) this->results_in_script_clause);
		builder->BuildString((char *) " WHERE ");
		builder->BuildString((char *) this->results_in_id_clause);
		builder->BuildString((char *) " AND `result_script_uris`.`hits` >= ");
		builder->BuildString((char *) script_hit_threshold_s);

		// build and execute
		this->mysqlPrepareQuery(builder->build_str);
		this->mysqlExecuteStatement();
		this->mysqlDestroyStatement();
		delete builder;

	}

	// bpfree allocated variables.  Don't use
	// tq free as it is not used for variable allocation.
	if(base_hit_threshold_s)
		bpfree(base_hit_threshold_s);
	if(path_hit_threshold_s)
		bpfree(path_hit_threshold_s);
	if(script_hit_threshold_s)
		bpfree(script_hit_threshold_s);

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// Fetch a path stack from the db.
BP_ERROR_T Block::fetchPathStack
(
	_vin size_t lock_for_brute_force,
	_vin char * sha1,
	_vin char * path_href,
	_vin char * path_href_like,
	_vin char * path_href_regexp,
	_vin size_t  hits,
	_vin size_t  initial_result_id,
	_vin char *  created_ts,
	_vin size_t  disabled,
	_vin size_t  disabled_code,
	_vin size_t brute_status
)
{

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter db critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Retrieve Records %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create a new query builder
	BP_StringUtils * query_builder = new BP_StringUtils(NULL);

	// set the basic select query
	query_builder->BuildString("SELECT `id`, `sha1`, `path_href`, `hits`, `initial_result_id`, `created_ts`, `disabled`, `disabled_code`, `brute_status` FROM `result_path_uris` ");


	// test to see if we'll need a where clause
	if(
			sha1 ||
			path_href ||
			path_href_like ||
			path_href_regexp ||
			hits != -1 ||
			initial_result_id != -1 ||
			created_ts ||
			disabled != -1 ||
			disabled_code != -1 ||
			brute_status != -1
	)
	{
		query_builder->BuildString(" WHERE ");
	}

	size_t add_and = 0;
	if(sha1)
	{

		// add and if necessary
		if(add_and)
		{
			query_builder->BuildString(" AND ");
		}

		query_builder->BuildString(" `sha1` = ");
		query_builder->BuildMySQLHexFromBuff(sha1, bpstrlen(sha1));

		if(!add_and)
			add_and++;
	}

	if(path_href)
	{

		// add and if necessary
		if(add_and)
		{
			query_builder->BuildString(" AND ");
		}

		query_builder->BuildString(" `sha1` = ");
		query_builder->BuildMySQLHexFromBuff(sha1, bpstrlen(sha1));

		if(!add_and)
			add_and++;

	}

	if(path_href_like)
	{

		// add and if necessary
		if(add_and)
		{
			query_builder->BuildString(" AND ");
		}


		query_builder->BuildString(" `sha1` = ");
		query_builder->BuildMySQLHexFromBuff(sha1, bpstrlen(sha1));

		if(!add_and)
			add_and++;

	}

	if(path_href_regexp)
	{

		// add and if necessary
		if(add_and)
		{
			query_builder->BuildString(" AND ");
		}

		query_builder->BuildString(" `path_href` REGEXP ");
		query_builder->BuildMySQLHexFromBuff(sha1, bpstrlen(sha1));

		if(!add_and)
			add_and++;

	}

	if(hits != -1){}
	if(initial_result_id != -1){}
	if(created_ts){}
	if(disabled != -1){}
	if(disabled_code != -1){}
	if(brute_status != -1){}


	// delete the query builder
	delete query_builder;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% If we locked, update here %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}


// pulls a result index into a size_t, and does it quickly.  Works
// off results.id.
size_t Block::retrieveRandomResultDBIndexFastFind(_vdbin size_t spidered_status, _vdbin size_t audit_status)
{

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter db critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// clear any dangling data if necessary
	this->mysqlDestroyStatement();

	// db id to return
	size_t db_id = 0;

	// create new string builder
	BP_StringUtils * query_builder = new BP_StringUtils(NULL);

	// set builder string
	query_builder->BuildString((char *) "SELECT (SELECT ROUND(RAND() * (SELECT MAX(id) FROM results)) AS id) FROM results ");

	// small buffer for update checks
	char buff[128] = {0};

	// add where clause if we have one to set
	if( (spidered_status != -1) || (audit_status != -1) )
	{
		query_builder->BuildString((char *) " WHERE 1=1 ");
	}

	// check spidered status
	if(spidered_status != -1)
	{
		snprintf((char *) &buff, sizeof(buff)-1, " AND results.spidered = %u ", spidered_status);
		query_builder->BuildString((char *) &buff);
	}

	// check audit status
	if(audit_status != -1)
	{
		snprintf((char *) &buff, sizeof(buff)-1, " AND results.status = %u ", audit_status);
		query_builder->BuildString((char *) &buff);
	}

	// limit the query
	query_builder->BuildString((char *) " LIMIT 1; ");

	// set the query
	if(this->mysqlPrepareQuery(query_builder->build_str) != ERR_SUCCESS)
	{
		printf("\n Failed to prepare: %s", query_builder->build_str);
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// add db id as result
	this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &db_id, sizeof(db_id));
	this->mysqlBindResults();

	// execute the query
	this->mysqlExecuteStatement();

	// fetch the result
	this->mysqlFetchResult();

	// destroy the query string
	delete query_builder;

	// return database index
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(db_id);

}

// displays the web result stack in order
BP_ERROR_T Block::displayWebResultStack()
{

	if(!this->web_result_stack_n)
		return ERR_FAILURE;
	if(!this->web_result_stack)
		return ERR_FAILURE;

	// entry pointer used for iteration below
	P_BP_WEB_RESULT_ENTRY entry = NULL;

	// simple loop for iteration purposes
	size_t n = 0;
	for(; n < this->web_result_stack_n; n++)
	{

		// set entry to current index for manipulation
		entry = (P_BP_WEB_RESULT_ENTRY) &this->web_result_stack[n];

		// display entry information
		printf("\n [+] Web Result Entry: %u  ", n);
		printf("\n\t result_id:       %u", entry->result_id);
		printf("\n\t assessment_id:   %u", entry->assessment_id);
		printf("\n\t type:            %u", entry->type);
		printf("\n\t sha1:            %s", entry->sha1);
		printf("\n\t href:            %s", entry->href);
		printf("\n\t base:                      %zu", entry->base);
		printf("\n\t path:                      %zu", entry->path);
		printf("\n\t script:                    %zu", entry->script);
		printf("\n\t stat                       %zu", entry->status);
		printf("\n\t update_resolution_status:  %zu", entry->update_resolution_status);
		printf("\n\t origin_type:               %zu", entry->origin_type_form_or_anchor_code);
		printf("\n\t commit_ts:                 %s", entry->commit_timestamp);
		printf("\n\t audit_start_ts:            %s", entry->audit_start_timestamp);
		printf("\n\t audit_completed_ts:        %s", entry->audit_completed_timestamp);
		printf("\n\t spidered:                  %zu", entry->spidered);
		printf("\n\t spidered_parent_result_id: %zu", entry->spidered_parent_result_id);
		printf("\n\t spidered_ts:               %s", entry->spidered_timestamp);
		printf("\n\t disabled:                  %zu", entry->disabled);
		printf("\n\t disabled_code:             %zu", entry->disable_code);
		printf("\n\t disabled_path:             %zu", entry->disabled_path);
		printf("\n\t disabled_path_code:        %zu", entry->disabled_path_code);
		printf("\n\t disabled_base:             %zu", entry->disabled_base);
		printf("\n\t disabled_base_code:        %zu", entry->disabled_base_code);
		printf("\n\t disabled_script:           %zu", entry->disabled_script);
		printf("\n\t disabled_script_code:      %zu", entry->disabled_script_code);
		printf("\n");

	}


	// return indicating success
	return ERR_SUCCESS;

}

// === Simple Result Stack Pruning (to avoid group-by clauses and save db resources) =====

// Examines an existing web result stack and filters results down to distinct hosts.  This is similar to
// a distinct clause that groups on the result_hosts table.
BP_ERROR_T Block::filterWebResultStackDownToDistinctHosts()
{

	if(!this->web_result_stack_n)
			return ERR_FAILURE;
	if(!this->web_result_stack)
		return ERR_FAILURE;

	// create a hash table registry for looking up hosts
	P_BP_HASH_TABLE_REGISTRY compare_hash_reg = BP_CreateHashTableRegistry();

	// add the compare hash reg
	BP_HashRegAddTableByPathQuery(compare_hash_reg, "/hosts");

	// entry pointer used for iteration below
	P_BP_WEB_RESULT_ENTRY entry = NULL;

	// create and zero the host id buff
	char host_id_buff[128];
	BP_ZERO_STACK(host_id_buff);

	// simple loop for iteration purposes
	size_t n = 0;
	for(; n < this->web_result_stack_n; n++)
	{

		// set entry to current index for manipulation
		entry = (P_BP_WEB_RESULT_ENTRY) &this->web_result_stack[n];

		// create and save host id buff
		BP_ZERO_STACK(host_id_buff);
		snprintf(host_id_buff, sizeof(host_id_buff)-1, "%zu", entry->host);

		// check to see if the entry already exists
		P_BP_HTKE host_lookup = BP_HashRegLookupTableEntryByPath
		(
			compare_hash_reg,
			BP_PATH_OPEN
				"hosts",
				host_id_buff
			BP_PATH_CLOSE
		);

		// disable the host if we found our host
		if(host_lookup)
		{
			entry->disabled = 1;
			continue;
		}


		// add data entry
		BP_HashRegAddDataToTableByPath
		(
			compare_hash_reg,
			BP_PATH_OPEN
				"hosts"
			BP_PATH_CLOSE,
			host_id_buff,
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
			(void *) "found",
			5
		);

	}

	// destroy the hash table registry
	BP_DestroyHashTableRegistry(compare_hash_reg);

	// return indicating success
	return ERR_SUCCESS;

}

// Examines an existing web result stack and filters results down to distinct bases.  This is similar to
// a distinct clause that groups on the result_bases table.
BP_ERROR_T Block::filterWebResultStackDownToDistinctBases()
{

	if(!this->web_result_stack_n)
			return ERR_FAILURE;
	if(!this->web_result_stack)
		return ERR_FAILURE;

	// create a hash table registry for looking up bases
	P_BP_HASH_TABLE_REGISTRY compare_hash_reg = BP_CreateHashTableRegistry();

	// add the compare hash reg
	BP_HashRegAddTableByPathQuery(compare_hash_reg, "/bases");

	// entry pointer used for iteration below
	P_BP_WEB_RESULT_ENTRY entry = NULL;

	// create and zero the base id buff
	char base_id_buff[128];
	BP_ZERO_STACK(base_id_buff);

	// simple loop for iteration purposes
	size_t n = 0;
	for(; n < this->web_result_stack_n; n++)
	{

		// set entry to current index for manipulation
		entry = (P_BP_WEB_RESULT_ENTRY) &this->web_result_stack[n];

		// create and save base id buff
		BP_ZERO_STACK(base_id_buff);
		snprintf(base_id_buff, sizeof(base_id_buff)-1, "%zu", entry->base);

		// check to see if the entry already exists
		P_BP_HTKE base_lookup = BP_HashRegLookupTableEntryByPath
		(
			compare_hash_reg,
			BP_PATH_OPEN
				"bases",
				base_id_buff
			BP_PATH_CLOSE
		);

		// disable the base if we found our base
		if(base_lookup)
		{
			entry->disabled = 1;
			continue;
		}


		// add data entry
		BP_HashRegAddDataToTableByPath
		(
			compare_hash_reg,
			BP_PATH_OPEN
				"bases"
			BP_PATH_CLOSE,
			base_id_buff,
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
			(void *) "found",
			5
		);

	}

	// destroy the hash table registry
	BP_DestroyHashTableRegistry(compare_hash_reg);

	// return indicating success
	return ERR_SUCCESS;

}

// Examines an existing web result stack and filters results down to distinct paths.  This is similar to
// a distinct clause that groups on the result_paths table.
BP_ERROR_T Block::filterWebResultStackDownToDistinctPaths()
{

	if(!this->web_result_stack_n)
			return ERR_FAILURE;
	if(!this->web_result_stack)
		return ERR_FAILURE;

	// create a hash table registry for looking up paths
	P_BP_HASH_TABLE_REGISTRY compare_hash_reg = BP_CreateHashTableRegistry();

	// add the compare hash reg
	BP_HashRegAddTableByPathQuery(compare_hash_reg, "/paths");

	// entry pointer used for iteration below
	P_BP_WEB_RESULT_ENTRY entry = NULL;

	// create and zero the path id buff
	char path_id_buff[128];
	BP_ZERO_STACK(path_id_buff);

	// simple loop for iteration purposes
	size_t n = 0;
	for(; n < this->web_result_stack_n; n++)
	{

		// set entry to current index for manipulation
		entry = (P_BP_WEB_RESULT_ENTRY) &this->web_result_stack[n];

		// create and save path id buff
		BP_ZERO_STACK(path_id_buff);
		snprintf(path_id_buff, sizeof(path_id_buff)-1, "%zu", entry->path);

		// check to see if the entry already exists
		P_BP_HTKE path_lookup = BP_HashRegLookupTableEntryByPath
		(
			compare_hash_reg,
			BP_PATH_OPEN
				"paths",
				path_id_buff
			BP_PATH_CLOSE
		);

		// disable the path if we found our path
		if(path_lookup)
		{
			entry->disabled = 1;
			continue;
		}


		// add data entry
		BP_HashRegAddDataToTableByPath
		(
			compare_hash_reg,
			BP_PATH_OPEN
				"paths"
			BP_PATH_CLOSE,
			path_id_buff,
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
			(void *) "found",
			5
		);

	}

	// destroy the hash table registry
	BP_DestroyHashTableRegistry(compare_hash_reg);

	// return indicating success
	return ERR_SUCCESS;

}

// Examines an existing web result stack and filters results down to distinct scripts.  This is similar to
// a distinct clause that groups on the result_scripts table.
BP_ERROR_T Block::filterWebResultStackDownToDistinctScripts()
{

	if(!this->web_result_stack_n)
			return ERR_FAILURE;
	if(!this->web_result_stack)
		return ERR_FAILURE;

	// create a hash table registry for looking up scripts
	P_BP_HASH_TABLE_REGISTRY compare_hash_reg = BP_CreateHashTableRegistry();

	// add the compare hash reg
	BP_HashRegAddTableByPathQuery(compare_hash_reg, "/scripts");

	// entry pointer used for iteration below
	P_BP_WEB_RESULT_ENTRY entry = NULL;

	// create and zero the script id buff
	char script_id_buff[128];
	BP_ZERO_STACK(script_id_buff);

	// simple loop for iteration purposes
	size_t n = 0;
	for(; n < this->web_result_stack_n; n++)
	{

		// set entry to current index for manipulation
		entry = (P_BP_WEB_RESULT_ENTRY) &this->web_result_stack[n];

		// create and save script id buff
		BP_ZERO_STACK(script_id_buff);
		snprintf(script_id_buff, sizeof(script_id_buff)-1, "%zu", entry->script);

		// check to see if the entry already exists
		P_BP_HTKE script_lookup = BP_HashRegLookupTableEntryByPath
		(
			compare_hash_reg,
			BP_PATH_OPEN
				"scripts",
				script_id_buff
			BP_PATH_CLOSE
		);

		// disable the script if we found our script
		if(script_lookup)
		{
			entry->disabled = 1;
			continue;
		}


		// add data entry
		BP_HashRegAddDataToTableByPath
		(
			compare_hash_reg,
			BP_PATH_OPEN
				"scripts"
			BP_PATH_CLOSE,
			script_id_buff,
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
			(void *) "found",
			5
		);

	}

	// destroy the hash table registry
	BP_DestroyHashTableRegistry(compare_hash_reg);

	// return indicating success
	return ERR_SUCCESS;

}


// === Simple Data Generators for Result Stack ======

// Generates constraints suitable for use in queries (where clause)
BP_ERROR_T Block::webResultStackWhereClauses()
{

	if(!this->web_result_stack_n)
		return ERR_FAILURE;

	// destroy the where clauses where necessary
	this->destroyWebResultStackWhereClauses();

	// create string builder
	BP_StringUtils * id_builder            = new BP_StringUtils(NULL);
	BP_StringUtils * assessment_id_builder = new BP_StringUtils(NULL);
	BP_StringUtils * base_builder          = new BP_StringUtils(NULL);
	BP_StringUtils * path_builder          = new BP_StringUtils(NULL);
	BP_StringUtils * script_builder        = new BP_StringUtils(NULL);

	// enable unique string duplication
	id_builder->duplicate_unique_strings            = true;
	assessment_id_builder->duplicate_unique_strings = true;
	base_builder->duplicate_unique_strings          = true;
	path_builder->duplicate_unique_strings          = true;
	script_builder->duplicate_unique_strings        = true;

	// conversion buffers
	char id[128];
	char assessment_id[128];
	char base[128];
	char path[128];
	char script[128];

	// walk result stack
	size_t n = 0;
	for(; n < this->web_result_stack_n; n++)
	{

		// zero out the id string
		memset((void *)&id, 0x00, sizeof(id));
		memset((void *)&assessment_id, 0x00, sizeof(assessment_id));
		memset((void *)&base, 0x00, sizeof(base));
		memset((void *)&path, 0x00, sizeof(path));
		memset((void *)&script, 0x00, sizeof(script));

		// embed the result id in the string
		snprintf((char *) &id, sizeof(id)-1, "%zu", this->web_result_stack[n].result_id);
		snprintf((char *) &assessment_id, sizeof(assessment_id)-1, "%zu", this->web_result_stack[n].assessment_id);
		snprintf((char *) &base, sizeof(base)-1, "%zu", this->web_result_stack[n].base);
		snprintf((char *) &path, sizeof(path)-1, "%zu", this->web_result_stack[n].path);
		snprintf((char *) &script, sizeof(script)-1, "%zu", this->web_result_stack[n].script);

		// add unique strings
		if(this->web_result_stack[n].result_id)
			id_builder->AddUniqueString((char *) &id);
		if(this->web_result_stack[n].assessment_id)
			assessment_id_builder->AddUniqueString((char *) &assessment_id);
		if(this->web_result_stack[n].base)
			base_builder->AddUniqueString((char *) &base);
		if(this->web_result_stack[n].path)
			path_builder->AddUniqueString((char *) &path);
		if(this->web_result_stack[n].script)
			script_builder->AddUniqueString((char *) &script);

	}


	// build unquoted csv
	if(id_builder->unique_str_cnt)
	{
		id_builder->BuildString(" `results`.`id` in ( ");
		id_builder->buildUniqueStringStackAsCSV(BP_STRINGUTILS_BUILD_UNQUOTED_CSV);
		id_builder->BuildString(" ) ");
		this->results_in_id_clause = (char *) tq_strdup(this->tq, id_builder->build_str, BPLN);
		this->id_n = id_builder->unique_str_cnt;
		delete id_builder;
	}

	// assessment_id_builder unquoted csv
	if(assessment_id_builder->unique_str_cnt)
	{
		assessment_id_builder->BuildString(" `results`.`assessment_id` in ( ");
		assessment_id_builder->buildUniqueStringStackAsCSV(BP_STRINGUTILS_BUILD_UNQUOTED_CSV);
		assessment_id_builder->BuildString(" ) ");
		this->results_in_assessment_id_clause = (char *) tq_strdup(this->tq, assessment_id_builder->build_str, BPLN);
		this->assessment_id_n = assessment_id_builder->unique_str_cnt;
		delete assessment_id_builder;
	}

	// base unquoted csv
	if(base_builder->unique_str_cnt)
	{
		base_builder->BuildString(" `results`.`base` in ( ");
		base_builder->buildUniqueStringStackAsCSV(BP_STRINGUTILS_BUILD_UNQUOTED_CSV);
		base_builder->BuildString(" ) ");
		this->results_in_base_clause = (char *) tq_strdup(this->tq, base_builder->build_str, BPLN);
		this->base_n = base_builder->unique_str_cnt;
		delete base_builder;
	}

	// path unquoted csv
	if(path_builder->unique_str_cnt)
	{
		path_builder->BuildString(" `results`.`path` in ( ");
		path_builder->buildUniqueStringStackAsCSV(BP_STRINGUTILS_BUILD_UNQUOTED_CSV);
		path_builder->BuildString(" ) ");
		this->results_in_path_clause = (char *) tq_strdup(this->tq, path_builder->build_str, BPLN);
		this->path_n = path_builder->unique_str_cnt;
		delete path_builder;
	}

	// script unquoted csv
	if(script_builder->unique_str_cnt)
	{
		script_builder->BuildString(" `results`.`script` in ( ");
		script_builder->buildUniqueStringStackAsCSV(BP_STRINGUTILS_BUILD_UNQUOTED_CSV);
		script_builder->BuildString(" ) ");
		this->results_in_script_clause = (char *) tq_strdup(this->tq, script_builder->build_str, BPLN);
		this->script_n = script_builder->unique_str_cnt;
		delete script_builder;
	}

	// return indicating success
	return ERR_SUCCESS;

}

// Destroy the where clauses if they exist
BP_ERROR_T Block::destroyWebResultStackWhereClauses()
{

	// `results`.`id` IN ( d,a,t,a )
	if(this->results_in_id_clause)
	{
		tq_free(this->tq, this->results_in_id_clause);
		this->results_in_id_clause = NULL;
		this->id_n = 0;
	}


	// `results`.`assessment_id` IN ( d,a,t,a )
	if(this->results_in_assessment_id_clause)
	{
		tq_free(this->tq, this->results_in_assessment_id_clause);
		this->results_in_assessment_id_clause = NULL;
		this->assessment_id_n = 0;
	}

	// `results`.`base` IN ( d,a,t,a )
	if(this->results_in_base_clause)
	{
		tq_free(this->tq, this->results_in_base_clause);
		this->results_in_base_clause = NULL;
		this->base_n = 0;
	}


	// `results`.`path` IN (d,a,t,a)
	if(this->results_in_path_clause)
	{
		tq_free(this->tq, this->results_in_path_clause);
		this->results_in_path_clause = NULL;
		this->path_n = 0;
	}

	// `results`.`script` IN (d,a,t,a)
	if(this->results_in_script_clause)
	{
		tq_free(this->tq, this->results_in_script_clause);
		this->results_in_script_clause = NULL;
		this->script_n = 0;
	}

	// return indicating success
	return ERR_SUCCESS;

}

// update status flags to any value (except -1).  -1 is used as an avoidance update.
BP_ERROR_T Block::updateStatusFlagsForStack(_vdbin size_t lock_for_audit, _vdbin size_t lock_for_spidering, _vdbin size_t lock_for_update_resolution)
{

	// cant update anything if the stack is empty
	if(!this->web_result_stack_n)
		return ERR_FAILURE;

	/// ensure we have a valid mysql session
	if(!this->mysqlSession)
	{
		this->AddErrorToStack((char *) "Update status flags method requires DB connection whereas none is set.");
		return ERR_FAILURE;
	}

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// destroy statement before exit
	this->mysqlDestroyStatement();

	// create string builder for the purpose of dynamic
	// query generation.
	BP_StringUtils * query_builder = new BP_StringUtils(NULL);

	// exit if we cannot create the builder
	if(!query_builder)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// small stack buffer to hold formatted text before
	// it is built.
	char index_add_str[128];

	// add the first part of the update query
	query_builder->BuildString((char *) "UPDATE `results` SET ");

	size_t is_first = 1;

	// add lock for audit
	if(lock_for_audit != (size_t) -1)
	{
		if(is_first)
		{
			query_builder->BuildString((char *) ",");
		}

		memset(&index_add_str, 0x00, 128);
		sprintf((char *) &index_add_str, " `status`=%u", lock_for_audit);
		query_builder->BuildString((char *) &index_add_str);

		// no longer the first so set the flag
		is_first = 0;
	}

	// add lock for spidering
	if(lock_for_spidering != (size_t) -1)
	{
		if(!is_first)
		{
			query_builder->BuildString((char *) ",");
		}

		memset(&index_add_str, 0x00, 128);
		sprintf((char *) &index_add_str, " `spidered`=%u", lock_for_spidering);
		query_builder->BuildString((char *) &index_add_str);

		// no longer the first so set the flag
		is_first = 0;
	}

	// add lock for spidering
	if(lock_for_update_resolution != (size_t) -1)
	{
		if(!is_first)
		{
			query_builder->BuildString((char *) ",");
		}

		memset(&index_add_str, 0x00, 128);
		sprintf((char *) &index_add_str, " `update_resolution_status`=%u", lock_for_update_resolution);
		query_builder->BuildString((char *) &index_add_str);

		// no longer the first so set the flag
		is_first = 0;

	}

	// add the constraint to the set
	query_builder->BuildString(" WHERE `id` IN ( 0 ");


	// flag indicating whether or not the query
	// has any elements to update .   Prevents empty IN () sets.
	size_t query_contains_elements = 0;

	// iterate through each retrieved element and create string
	size_t n = 0;
	for(; n < this->web_result_stack_n; n++)
	{

		// only update records which have not already been updated individually
		if(this->web_result_stack[n].record_already_updated_audit || this->web_result_stack[n].record_already_updated_spidered)
			continue;

		// set flag if we have elements set
		query_contains_elements=1;

		// zero out string on loop
		memset(&index_add_str, 0x00, 128);

		// embed the result id
		snprintf((char *) &index_add_str, sizeof(index_add_str)-1, ", %zu", this->web_result_stack[n].result_id);

		// build the string using the result id from array
		query_builder->BuildString((char *) &index_add_str);


	}

	// complete the "IN" set
	query_builder->BuildString((char *) ");");

	// don't update unless we have a reason to update
	if(!query_contains_elements)
	{
		delete query_builder;
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// printf("\n UPDATE UNLOCK: %s", query_builder->build_str);

	// prepare the query
	if(this->mysqlPrepareQuery(query_builder->build_str) != ERR_SUCCESS)
	{
		this->AddErrorToStack("SQL prepare failed when attempting result update locking.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// execute update statement
	if(this->mysqlExecuteStatement() != ERR_SUCCESS)
	{
		this->AddErrorToStack("SQL execute failed when attempting result update locking.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// destroy statement before exit
	this->mysqlDestroyStatement();

	// destroy query builder
	delete query_builder;

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}


// ============ Single Result Retrieval Routines ==============

// Retrieve a random target from the db
BP_ERROR_T Block::retrieveResultFromDb(char *where_clause)
{

	// require valid mysql session
	if (!this->mysqlSession)
	{
		this->AddErrorToStack((char*) "Error in retrieveResultFromDb (NULL Mysql session");
		return ERR_FAILURE;
	}

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// result index
	size_t result_index = 0;
	char * result_href = (char *) tq_calloc(this->tq, 2048, 1, BPLN);

	char * base_query = (char *) "SELECT id, href FROM results where %s and status=0 ORDER BY RAND() LIMIT 1";

	// set blank where clause if its null
	if (!where_clause)
		where_clause = (char *) " ";

	size_t query_size = 0;

	// append where clause length
	query_size += bpstrlen(where_clause);
	// append base query length
	query_size += bpstrlen(base_query);

	// allocate query size
	char *query = (char *) tq_calloc(this->tq, query_size, 1, BPLN);
	snprintf(query, query_size - 1, base_query, where_clause);

	// prepare the query
	if (this->mysqlPrepareQuery(query) == ERR_FAILURE) {
		this->AddErrorToStack((char*) "Error in retrieveResultFromDb: Cannot prepare query.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// add index
	if (this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &result_index, sizeof(result_index)) == ERR_FAILURE)
		printf("\n Cannot add result");

	// add href buffer
	if (this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) result_href, 2047) == ERR_FAILURE)
		printf("\n Cannot add result");

	// run lookup query on url to make sure it doesn't already exist
	if (this->mysqlBindResults() == ERR_FAILURE)
		printf("\n Cannot bind results");

	// execute the statement
	if (this->mysqlExecuteStatement() == ERR_FAILURE)
	{
		// printf("\n Cannot execute statement.");
	}

	// store results
	this->mysqlStoreResults();

	// retch a result (max one result for this query)
	this->mysqlFetchResult();

	// destroy statement
	this->mysqlDestroyStatement();

	// store result index/href
	this->web_result_id = result_index;
	snprintf((char *) &this->web_result_href, sizeof(this->web_result_href)-1, "%s", result_href);

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}


// Retrieve a result from the database using a like query constraint.  This is unfiltered
// sql, so be careful for untrusted input.  This facility is provided under trusted grounds,
// so you should take that into consideration before implementing its use under dynamic conditions.
BP_ERROR_T Block::retrieveResultFromDbHrefLike(char *like, bool unspidered, bool unaudited, size_t retrieve_order)
{

	// require valid mysql session
	if (!this->mysqlSession)
	{
		this->AddErrorToStack((char*) "Error in retrieveResultFromDb (NULL Mysql session");
		return ERR_FAILURE;
	}

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// result index
	size_t result_index = 0;
	char * result_href = (char *) tq_calloc(this->tq, 2048, 1, BPLN);

	// set base query (prepared statement)
	const char * base_query      = (char *) "SELECT id, href FROM results where href LIKE ?";
	const char * status_clause   = (char *) " and status=0 ";
	const char * spidered_clause = (char *) " and spidered=0 ";
	const char * clause_end      = (char *) "ORDER BY RAND() LIMIT 1";

	// dynamic (non-adhoc) sql generation
	char builder_query[512] = {0};
	snprintf(builder_query, 510, "%s%s%s%s",base_query,	unaudited ? status_clause : " ",	unspidered ? spidered_clause : " ",	clause_end);

	// append base query length
	size_t query_size = bpstrlen(builder_query);


	// prepare the query
	if (this->mysqlPrepareQuery(builder_query) == ERR_FAILURE)
	{
		this->AddErrorToStack((char*) "Error in retrieveResultFromDb: Cannot prepare query.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// add href like parameter
	this->mysqlAddParameter(MYSQL_TYPE_STRING, like, bpstrlen(like));
	this->mysqlBindParameters();

	// add index result
	if (this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &result_index, sizeof(result_index)) == ERR_FAILURE)
		printf("\n Cannot add result");

	// add href buffer result
	if (this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) result_href, 2047) == ERR_FAILURE)
		this->AddErrorToStack((char*) "\n Cannot add result");

	// run lookup query on url to make sure it doesn't already exist
	if (this->mysqlBindResults() == ERR_FAILURE)
		this->AddErrorToStack((char*) "Cannot bind results");

	// execute the statement
	if (this->mysqlExecuteStatement() == ERR_FAILURE)
	{
		this->AddErrorToStack("Error: retrieveResultFromDbHrefLike failed to execute mysql statement.");
	}

	// store results
	this->mysqlStoreResults();

	// retch a result (max one result for this query)
	this->mysqlFetchResult();

	// destroy statement
	this->mysqlDestroyStatement();

	// NOTABLE DATA STORAGE: store result index/href identifier
	this->web_result_id   = result_index;

	// NOTABLE DATA STORAGE: store result href data
	snprintf((char *) &this->web_result_href, sizeof(this->web_result_href)-1, "%s", result_href);

	// destroy tmp variable on exit
	if(result_href)
	{
		tq_free(this->tq, result_href);
	}

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// used for setting results directly from result entries
BP_ERROR_T Block::setResultFromWebResultEntry(P_BP_WEB_RESULT_ENTRY web_result_entry)
{

	// ensure we have an entry
	if(!web_result_entry)
	{
		return ERR_FAILURE;
	}

	// set the result id
	this->web_result_id = web_result_entry->result_id;

	// zero out the memory
	memset(&this->web_result_href, 0x00, sizeof(this->web_result_href));

	// copy in the href data
	snprintf((char *) &this->web_result_href, sizeof(this->web_result_href)-1, "%s", web_result_entry->href);

	// return indicating success
	return ERR_SUCCESS;

}



// Sets the update resolution flag of the last record.
BP_ERROR_T Block::setLastResultUpdateResolutionStatus(size_t status_code)
{

	// check for result
	if (!this->web_result_href || !this->web_result_id || !this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;


	// set local variables (for readability)
	size_t id   = this->web_result_id;

	// update query
	char * query = (char *) "UPDATE `results` SET `update_resolution_status` = ? WHERE `results`.`id` =?;";

	// prepare query
	this->mysqlDestroyStatement();
	this->mysqlPrepareQuery(query);

	// add parameters
	this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &status_code, sizeof(status_code));

	// add result id
	this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &id, sizeof(id));

	// bind parameters
	this->mysqlBindParameters();

	// execute query
	this->mysqlExecuteStatement();

	// === Now Manipulate Result Stack if Necessary (prevents dual updates with stack updates)

	if(this->web_result_stack_n)
	{
		this->web_result_stack[this->web_result_stack_current_index].record_already_updated_spidered = 1;
	}

	// clear data where necessary
	this->mysqlDestroyStatement();

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// Sets the spidered flag of the last retrieved record
BP_ERROR_T Block::setLastResultSpideredStatus(size_t status_code)
{

	// check for result
	if (!this->web_result_href || !this->web_result_id || !this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// set local variables (for readability)
	size_t id   = this->web_result_id;

	// update query
	char * query = (char *) "UPDATE `results` SET `spidered` = ?, `spidered_timestamp` = NOW() WHERE `results`.`id` =?;";

	// prepare query
	this->mysqlDestroyStatement();
	this->mysqlPrepareQuery(query);

	// add parameters
	this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &status_code, sizeof(status_code));

	// add result id
	this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &id, sizeof(id));

	// bind parameters
	this->mysqlBindParameters();

	// execute query
	this->mysqlExecuteStatement();

	// === Now Manipulate Result Stack if Necessary (prevents dual updates with stack updates)

	if(this->web_result_stack_n)
	{
		this->web_result_stack[this->web_result_stack_current_index].record_already_updated_spidered = 1;
	}

	// clear data where necessary
	this->mysqlDestroyStatement();

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// Sets the status of the last retrieved record
BP_ERROR_T Block::setLastResultStatus(size_t status_code)
{

	// check for result
	if (!this->web_result_href || !this->web_result_id || !this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// set local variables (for readability)
	char * href = (char *) &this->web_result_href;
	size_t id = this->web_result_id;

	// update query
	char * query = (char *) "UPDATE `results` SET `status` = ? WHERE `results`.`id` =?;";

	// prepare query
	this->mysqlPrepareQuery(query);

	// add parameters
	this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &status_code, sizeof(status_code));

	// add result id
	this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &id, sizeof(id));

	// bind parameters
	this->mysqlBindParameters();

	// execute query
	this->mysqlExecuteStatement();

	// destroy the statement
	this->mysqlDestroyStatement();

	// === Now Manipulate Result Stack if Necessary (prevents dual updates with stack updates)

	if(this->web_result_stack_n)
	{
		this->web_result_stack[this->web_result_stack_current_index].record_already_updated_audit = 1;
	}

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Single Result Update Methods %%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Insert a base_href into the db (linked to result in base field)
size_t Block::insertBaseHref(char * base_href, size_t result_id)
{

	// check to ensure session exists
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;


	// href argument is required
	if(!base_href)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// ensure base is reasonable printable
	if(!BP_StringIsReasonablePrintableString(base_href, bpstrlen(base_href), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// calculate href len
	size_t base_href_len = bpstrlen(base_href);

	// ensure we have a base href
	if(!base_href_len)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// ensure the reference is reasonable
	if(base_href_len < 6)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// skip any relative links that could've possibly
	// gotten past any previous sanity checking
	if(base_href[0] == '.')
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	if(base_href[0] == '/')
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	if(base_href[0] == '#')
		return ERR_FAILURE;

	// create hex buffer
	char * hex_buff = BP_BufferToHexString(base_href,strnlen(base_href, 1024));
	if(!hex_buff)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// run jit href check
	BP_StringUtils * query_builder = new BP_StringUtils(NULL);

	// build the string
	query_builder->BuildString((char *) "INSERT INTO `result_base_uris` (`sha1`, `base_href`, `initial_result_id`) VALUES (");

	// add buffer
	query_builder->BuildString((char *) "sha1(0x");
	query_builder->BuildString((char *) hex_buff);
	query_builder->BuildString((char *) "), ");
	query_builder->BuildString((char *) "0x");
	query_builder->BuildString((char *) hex_buff);

	// add last result id (or zero if new/unreferenced)
	char result_id_buff[64] = {0};
	sprintf((char *) &result_id_buff, ", %u", result_id);

	// append result id buffer
	query_builder->BuildString((char *) &result_id_buff);

	// close query set
	query_builder->BuildString((char *) " ) ");

	// add duplicate query constraint
	query_builder->BuildString(" ON DUPLICATE KEY UPDATE hits = hits+1;");


	// prepare the query
	if(this->mysqlPrepareQuery(query_builder->build_str) == ERR_FAILURE)
	{
		if(hex_buff)
			bpfree(hex_buff);

		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// execute the query
	if(this->mysqlExecuteStatement() == ERR_FAILURE)
	{
		if(hex_buff)
			bpfree(hex_buff);

		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}

	// destroy hex buffer here
	if(hex_buff)
	{
		bpfree(hex_buff);
		hex_buff=NULL;
	}

	// store result
	this->mysqlStoreResults();

	// destroy statement
	this->mysqlDestroyStatement();

	// fetch the result
	// this->mysqlFetchResult();


	// destroy the query builder
	delete query_builder;


#if USE_MARIADB == 1

	// check for insert or update id
	my_ulonglong insert_or_update_id = mysql_insert_id(this->mysqlSession->conn);

	// if the record was updated instead of updated, the id must be looked up
	if(!insert_or_update_id)
		insert_or_update_id = this->lookupBaseHrefId(base_href);

	// add catch for undefined behavior
	if(!insert_or_update_id)
	{
		printf("\n [!!] Error: Record was not inserted, updated, or exited in defined pattern.  Undefined behavior.");
		// asm("int3");
	}

	// return the new insert id (should always be nonzero)
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION;
	return insert_or_update_id;

#else

	// check for insert or update id
	my_ulonglong insert_or_update_id = this->mysqlSession->stmt->insert_id;

	// if the record was updated instead of updated, the id must be looked up
	if(!insert_or_update_id)
		insert_or_update_id = this->lookupBaseHrefId(base_href);

	// add catch for undefined behavior
	if(!insert_or_update_id)
	{
		printf("\n [!!] Error: Record was not inserted, updated, or exited in defined pattern.  Undefined behavior.");
		// asm("int3");
	}

	// return the new insert id (should always be nonzero)
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION;
	return insert_or_update_id;

#endif

}


// Insert a host into the db (linked to result in base field)
size_t Block::insertBaseHost(char * host_href, size_t result_id)
{

	// check to ensure session exists
	if(!this->mysqlSession)
		return 0;

	// enter critical sections
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// href argument is required
	if(!host_href)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);

	// ensure base is reasonable printable
	if(!BP_StringIsReasonablePrintableString(host_href, bpstrlen(host_href), BP_FALSE, BP_FALSE))
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// create hex buffer
	char * hex_buff = BP_BufferToHexString(host_href,strnlen(host_href, 1024));
	if(!hex_buff)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);

	// run jit href check
	BP_StringUtils * query_builder = new BP_StringUtils(NULL);

	// build the string
	query_builder->BuildString((char *) "INSERT INTO `result_hosts` (`sha1`, `hostname`, `initial_result_id`) VALUES (");

	// add buffer
	query_builder->BuildString((char *) "sha1(0x");
	query_builder->BuildString((char *) hex_buff);
	query_builder->BuildString((char *) "), ");
	query_builder->BuildString((char *) "0x");
	query_builder->BuildString((char *) hex_buff);

	// add last result id (or zero if new/unreferenced)
	char result_id_buff[64] = {0};
	sprintf((char *) &result_id_buff, ", %u", result_id);

	// append result id buffer
	query_builder->BuildString((char *) &result_id_buff);

	// close query set
	query_builder->BuildString((char *) " ) ");

	// add duplicate query constraint
	query_builder->BuildString(" ON DUPLICATE KEY UPDATE hits = hits+1;");


	// prepare the query
	if(this->mysqlPrepareQuery(query_builder->build_str) == ERR_FAILURE)
	{
		if(hex_buff)
		{
			bpfree(hex_buff);
			hex_buff = NULL;
		}


		// exit routine
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);

	}

	// execute the query
	if(this->mysqlExecuteStatement() == ERR_FAILURE)
	{
		if(hex_buff)
		{
			bpfree(hex_buff);
			hex_buff = NULL;
		}

		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);
	}

	// destroy hex buffer here
	if(hex_buff)
	{
		bpfree(hex_buff);
		hex_buff=NULL;
	}

	// store result
	this->mysqlStoreResults();
	this->mysqlDestroyStatement();

	// fetch the result
	// this->mysqlFetchResult();


	// destroy the query builder
	delete query_builder;


#if USE_MARIADB == 1


	// check for insert or update id
	my_ulonglong insert_or_update_id = mysql_insert_id(this->mysqlSession->conn);

	// if the record was updated instead of updated, the id must be looked up
	if(!insert_or_update_id)
		insert_or_update_id = this->lookupBaseHostId(host_href);

	// add catch for undefined behavior
	if(!insert_or_update_id)
	{
		printf("\n [!!] Error: Record was not inserted, updated, or exited in defined pattern.  Undefined behavior.");
		// asm("int3");
	}

	// return the new insert id (should always be nonzero)
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(insert_or_update_id);


#else

	// check for insert or update id
	my_ulonglong insert_or_update_id = this->mysqlSession->stmt->insert_id;

	// if the record was updated instead of updated, the id must be looked up
	if(!insert_or_update_id)
		insert_or_update_id = this->lookupBaseHostId(host_href);

	// add catch for undefined behavior
	if(!insert_or_update_id)
	{
		printf("\n [!!] Error: Record was not inserted, updated, or exited in defined pattern.  Undefined behavior.");
		// asm("int3");
	}

	// return the new insert id (should always be nonzero)
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(insert_or_update_id);

#endif

}


// Insert a base_path into the db (linked to result in base field)
size_t Block::insertBasePath(char * base_path, size_t result_id)
{

	// check to ensure session exists
	if(!this->mysqlSession)
		return 0;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// href argument is required
	if(!base_path)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);

	// ensure base is reasonable printable
	if(!BP_StringIsReasonablePrintableString(base_path, bpstrlen(base_path), BP_FALSE, BP_FALSE))
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);


	// create hex buffer
	char * hex_buff = BP_BufferToHexString(base_path,strnlen(base_path, 1024));
	if(!hex_buff)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);

	// run jit href check
	BP_StringUtils * query_builder = new BP_StringUtils(NULL);

		// build the string
	query_builder->BuildString((char *) "INSERT INTO `result_path_uris` (`sha1`, `path_href`, `initial_result_id`) VALUES (");

	// add buffer
	query_builder->BuildString((char *) "sha1(0x");
	query_builder->BuildString((char *) hex_buff);
	query_builder->BuildString((char *) "), ");
	query_builder->BuildString((char *) "0x");
	query_builder->BuildString((char *) hex_buff);

	// add last result id (or zero if new/unreferenced)
	char result_id_buff[64] = {0};
	sprintf((char *) &result_id_buff, ", %u", result_id);

	// append result id buffer
	query_builder->BuildString((char *) &result_id_buff);

	// close query set
	query_builder->BuildString((char *) " ) ");

	// add duplicate query constraint
	query_builder->BuildString(" ON DUPLICATE KEY UPDATE  hits = hits+1;");

	// prepare the query
	if(this->mysqlPrepareQuery(query_builder->build_str) == ERR_FAILURE)
	{
		if(hex_buff)
			bpfree(hex_buff);

		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);

	}

	// execute the query
	if(this->mysqlExecuteStatement() == ERR_FAILURE)
	{
		if(hex_buff)
			bpfree(hex_buff);

		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);

	}

	// destroy hex buffer here
	if(hex_buff)
	{
		bpfree(hex_buff);
		hex_buff=NULL;
	}

	// store result
	this->mysqlStoreResults();

	// destroy statement
	this->mysqlDestroyStatement();

	// fetch the result
	// this->mysqlFetchResult();


	// destroy the query builder
	delete query_builder;


#if USE_MARIADB == 1

	// check for insert or update id
	my_ulonglong insert_or_update_id = mysql_insert_id(this->mysqlSession->conn);

	// if the record was updated instead of updated, the id must be looked up
	if(!insert_or_update_id)
		insert_or_update_id = this->lookupBasePathId(base_path);

	// add catch for undefined behavior
	if(!insert_or_update_id)
	{
		printf("\n [!!] Error: Record was not inserted, updated, or exited in defined pattern.  Undefined behavior.");
		// asm("int3");
	}

	// return the new insert id (should always be nonzero)
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(insert_or_update_id);


#else

	// check for insert or update id
	my_ulonglong insert_or_update_id = this->mysqlSession->stmt->insert_id;

	// if the record was updated instead of updated, the id must be looked up
	if(!insert_or_update_id)
		insert_or_update_id = this->lookupBasePathId(base_path);

	// add catch for undefined behavior
	if(!insert_or_update_id)
	{
		printf("\n [!!] Error: Record was not inserted, updated, or exited in defined pattern.  Undefined behavior.");
		// asm("int3");
	}

	// return the new insert id (should always be nonzero)
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(insert_or_update_id);

#endif

}


// Insert a base script into the db (linked to result in base field)
size_t Block::insertBaseScript(char * base_script, size_t result_id)
{

	// check to ensure session exists
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// href argument is required
	if(!base_script)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// ensure base is reasonable printable
	if(!BP_StringIsReasonablePrintableString(base_script, bpstrlen(base_script), BP_FALSE, BP_FALSE))
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// create hex buffer
	char * hex_buff = BP_BufferToHexString(base_script,strnlen(base_script, 1024));
	if(!hex_buff)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Build Query %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// run jit href check
	BP_StringUtils * query_builder = new BP_StringUtils(NULL);

		// build the string
	query_builder->BuildString((char *) "INSERT INTO `result_script_uris` (`sha1`, `script_href`, `initial_result_id`) VALUES (");

	// add buffer
	query_builder->BuildString((char *) "sha1(0x");
	query_builder->BuildString((char *) hex_buff);
	query_builder->BuildString((char *) "), ");
	query_builder->BuildString((char *) "0x");
	query_builder->BuildString((char *) hex_buff);

	// add last result id (or zero if new/unreferenced)
	char result_id_buff[64] = {0};
	sprintf((char *) &result_id_buff, ", %u", result_id);

	// append result id buffer
	query_builder->BuildString((char *) &result_id_buff);

	// close query set
	query_builder->BuildString((char *) " ) ");

	// add duplicate query constraint
	query_builder->BuildString(" ON DUPLICATE KEY UPDATE  hits = hits+1;");


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Prepare and Execute Query %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// prepare the query
	if(this->mysqlPrepareQuery(query_builder->build_str) == ERR_FAILURE)
	{
		if(hex_buff)
			bpfree(hex_buff);

		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);
	}

	// execute the query
	if(this->mysqlExecuteStatement() == ERR_FAILURE)
	{
		if(hex_buff)
			bpfree(hex_buff);

		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);
	}

	// destroy hex buffer here
	if(hex_buff)
	{
		bpfree(hex_buff);
		hex_buff=NULL;
	}

	// store result
	this->mysqlStoreResults();

	// destroy statement
	this->mysqlDestroyStatement();

	// fetch the result
	// this->mysqlFetchResult();

	// destroy the query builder
	delete query_builder;


#if USE_MARIADB == 1

	// check for insert or update id
	my_ulonglong insert_or_update_id = mysql_insert_id(this->mysqlSession->conn);

	// if the record was updated instead of updated, the id must be looked up
	if(!insert_or_update_id)
		insert_or_update_id = this->lookupBaseScriptId(base_script);

	// add catch for undefined behavior
	if(!insert_or_update_id)
	{
		printf("\n [!!] Error: Record was not inserted, updated, or exited in defined pattern.  Undefined behavior.");
		// asm("int3");
	}

	// return the new insert id (should always be nonzero)
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(insert_or_update_id);

#else


	// check for insert or update id
	my_ulonglong insert_or_update_id = this->mysqlSession->stmt->insert_id;

	// if the record was updated instead of updated, the id must be looked up
	if(!insert_or_update_id)
		insert_or_update_id = this->lookupBaseScriptId(base_script);

	// add catch for undefined behavior
	if(!insert_or_update_id)
	{
		printf("\n [!!] Error: Record was not inserted, updated, or exited in defined pattern.  Undefined behavior.");
		// asm("int3");
	}

	// return the new insert id (should always be nonzero)
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(insert_or_update_id);

#endif


}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Single Result ID Lookup Routines %%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// lookup result id
size_t Block::lookupResultId(char *result_href)
{

	if(!this->mysqlSession)
		return 0;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;


	if(!this->mysqlSession->conn)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);
	if(!result_href)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);
	if(!BP_StringIsReasonablePrintableString(result_href, bpstrlen(result_href), BP_FALSE, BP_FALSE))
		return 0;


	// create a lookup tq
	P_BP_LINKL lookup_tq = BP_CreateLinkL(".", BP_TRUE);

	// create host parameter ass encoded hex (sanity)
	char *result_parm = BP_BufferToMySQLHexString(result_href, bpstrlen(result_href));

	// create lookup query with embedded query
	tq_strdup(lookup_tq, "SELECT id FROM `results` WHERE href = ", BPLN);
	tq_strdup(lookup_tq, result_parm, BPLN);

	// destroy result parameter after use
	bpfree(result_parm);

	// flatten out the query
	BP_LinkLFlattenJoinStringElements(lookup_tq);

	// result index for storage data
	size_t result_index = 0;


	// prepare the query
	this->mysqlPrepareQuery((char *) lookup_tq->flat_buff);

	// add index
	if (this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &result_index, sizeof(result_index)) == ERR_FAILURE)
		this->AddErrorToStack((char*) "Cannot add result");

	// bind the parameters
	this->mysqlBindParameters();

	// run lookup query on url to make sure it doesn't already exist
	if (this->mysqlBindResults() == ERR_FAILURE)
		this->AddErrorToStack((char*) "Cannot bind results");

	// execute the statement
	if (this->mysqlExecuteStatement() == ERR_FAILURE)
	{
		this->AddErrorToStack((char *) "Failed to execute statement.");
		BP_DestroyLinkL(lookup_tq, BP_TRUE);
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// store results
	this->mysqlStoreResults();

	// fetch a result (max one result for this query)
	this->mysqlFetchResult();

	// destroy the statement
	this->mysqlDestroyStatement();

	// destroy link before exit
	BP_DestroyLinkL(lookup_tq, BP_TRUE);

	// return the result index if it was filled
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(result_index);

}

// lookup host id
size_t Block::lookupBaseHostId(char *host)
{

	if(!this->mysqlSession)
		return 0;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	if(!this->mysqlSession->conn)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);

	if(!host)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);

	if(!BP_StringIsReasonablePrintableString(host, bpstrlen(host), BP_FALSE, BP_FALSE))
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);


	// create a lookup tq
	P_BP_LINKL lookup_tq = BP_CreateLinkL(".", BP_TRUE);

	// create host parameter ass encoded hex
	char *host_parm = BP_BufferToMySQLHexString(host, bpstrlen(host));

	// create lookup query with embedded query
	tq_strdup(lookup_tq, "SELECT id FROM `result_hosts` WHERE hostname = ", BPLN);
	tq_strdup(lookup_tq, host_parm, BPLN);

	// destroy host parameter
	bpfree(host_parm);

	// flatten out the query
	BP_LinkLFlattenJoinStringElements(lookup_tq);

	// result index for storage data
	size_t result_index = 0;


	// prepare the query
	this->mysqlPrepareQuery((char *) lookup_tq->flat_buff);

	// add index
	if (this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &result_index, sizeof(result_index)) == ERR_FAILURE)
		this->AddErrorToStack((char*) "Cannot add result");

	// bind the parameters
	this->mysqlBindParameters();

	// run lookup query on url to make sure it doesn't already exist
	if (this->mysqlBindResults() == ERR_FAILURE)
		this->AddErrorToStack((char*) "Cannot bind results");

	// execute the statement
	if (this->mysqlExecuteStatement() == ERR_FAILURE)
	{
		this->AddErrorToStack((char *) "Failed to execute statement.");
		BP_DestroyLinkL(lookup_tq, BP_TRUE);
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// store results
	this->mysqlStoreResults();

	// fetch a result (max one result for this query)
	this->mysqlFetchResult();

	// destroy the statement
	this->mysqlDestroyStatement();

	// destroy link before exit
	BP_DestroyLinkL(lookup_tq, BP_TRUE);

	// return the result index if it was filled
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(result_index);

}

// lookup base id
size_t Block::lookupBaseHrefId(char *base)
{

	if(!this->mysqlSession)
		return 0;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	if(!this->mysqlSession->conn)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);

	if(!base)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);

	if(!BP_StringIsReasonablePrintableString(base, bpstrlen(base), BP_FALSE, BP_FALSE))
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);


	// create a lookup tq
	P_BP_LINKL lookup_tq = BP_CreateLinkL(".", BP_TRUE);

	// create host parameter ass encoded hex
	char *base_parm = BP_BufferToMySQLHexString(base, bpstrlen(base));

	// create lookup query with embedded query
	tq_strdup(lookup_tq, "SELECT id FROM `result_base_uris` WHERE base_href = ", BPLN);
	tq_strdup(lookup_tq, base_parm, BPLN);

	// destroy base parameter
	bpfree(base_parm);

	// flatten out the query
	BP_LinkLFlattenJoinStringElements(lookup_tq);

	// result index for storage data
	size_t result_index = 0;


	// prepare the query
	this->mysqlPrepareQuery((char *) lookup_tq->flat_buff);

	// add index
	if (this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &result_index, sizeof(result_index)) == ERR_FAILURE)
		this->AddErrorToStack((char*) "Cannot add result");

	// bind the parameters
	this->mysqlBindParameters();

	// run lookup query on url to make sure it doesn't already exist
	if (this->mysqlBindResults() == ERR_FAILURE)
		this->AddErrorToStack((char*) "Cannot bind results");

	// execute the statement
	if (this->mysqlExecuteStatement() == ERR_FAILURE)
	{
		this->AddErrorToStack((char *) "Failed to execute statement.");
		BP_DestroyLinkL(lookup_tq, BP_TRUE);
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// store results
	this->mysqlStoreResults();

	// fetch a result (max one result for this query)
	this->mysqlFetchResult();

	// destroy the statement
	this->mysqlDestroyStatement();

	// destroy link before exit
	BP_DestroyLinkL(lookup_tq, BP_TRUE);

	// return the result index if it was filled
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(result_index);

}

// lookup path id
size_t Block::lookupBasePathId(char *path)
{
	if(!this->mysqlSession)
		return 0;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	if(!this->mysqlSession->conn)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);
	if(!path)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);
	if(!BP_StringIsReasonablePrintableString(path, bpstrlen(path), BP_FALSE, BP_FALSE))
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);


	// create a lookup tq
	P_BP_LINKL lookup_tq = BP_CreateLinkL(".", BP_TRUE);

	// create host parameter ass encoded hex
	char *path_parm = BP_BufferToMySQLHexString(path, bpstrlen(path));

	// create lookup query with embedded query
	tq_strdup(lookup_tq, "SELECT id FROM `result_path_uris` WHERE path_href = ", BPLN);
	tq_strdup(lookup_tq, path_parm, BPLN);

	// destroy path parameter
	bpfree(path_parm);

	// flatten out the query
	BP_LinkLFlattenJoinStringElements(lookup_tq);

	// result index for storage data
	size_t result_index = 0;


	// prepare the query
	this->mysqlPrepareQuery((char *) lookup_tq->flat_buff);

	// add index
	if (this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &result_index, sizeof(result_index)) == ERR_FAILURE)
		this->AddErrorToStack((char*) "Cannot add result");

	// bind the parameters
	this->mysqlBindParameters();

	// run lookup query on url to make sure it doesn't already exist
	if (this->mysqlBindResults() == ERR_FAILURE)
		this->AddErrorToStack((char*) "Cannot bind results");

	// execute the statement
	if (this->mysqlExecuteStatement() == ERR_FAILURE)
	{
		this->AddErrorToStack((char *) "Failed to execute statement.");
		BP_DestroyLinkL(lookup_tq, BP_TRUE);
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// store results
	this->mysqlStoreResults();

	// fetch a result (max one result for this query)
	this->mysqlFetchResult();

	// destroy the statement
	this->mysqlDestroyStatement();

	// destroy link before exit
	BP_DestroyLinkL(lookup_tq, BP_TRUE);

	// return the result index if it was filled
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(result_index);

}


// lookup script ids
size_t Block::lookupBaseScriptId(char *script)
{

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return 0;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	if(!this->mysqlSession->conn)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);
	if(!script)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);
	if(!BP_StringIsReasonablePrintableString(script, bpstrlen(script), BP_FALSE, BP_FALSE))
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(0);


	// create a lookup tq
	P_BP_LINKL lookup_tq = BP_CreateLinkL(".", BP_TRUE);

	// create host parameter ass encoded hex
	char *script_parm = BP_BufferToMySQLHexString(script, bpstrlen(script));

	// create lookup query with embedded query
	tq_strdup(lookup_tq, "SELECT id FROM `result_script_uris` WHERE script_href = ", BPLN);
	tq_strdup(lookup_tq, script_parm, BPLN);

	// destroy script parameter
	bpfree(script_parm);

	// flatten out the query
	BP_LinkLFlattenJoinStringElements(lookup_tq);

	// result index for storage data
	size_t result_index = 0;


	// prepare the query
	this->mysqlPrepareQuery((char *) lookup_tq->flat_buff);

	// add index
	if (this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &result_index, sizeof(result_index)) == ERR_FAILURE)
		this->AddErrorToStack((char*) "Cannot add result");

	// bind the parameters
	this->mysqlBindParameters();

	// run lookup query on url to make sure it doesn't already exist
	if (this->mysqlBindResults() == ERR_FAILURE)
		this->AddErrorToStack((char*) "Cannot bind results");

	// execute the statement
	if (this->mysqlExecuteStatement() == ERR_FAILURE)
	{
		this->AddErrorToStack((char *) "Failed to execute statement.");
		BP_DestroyLinkL(lookup_tq, BP_TRUE);
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// store results
	this->mysqlStoreResults();

	// fetch a result (max one result for this query)
	this->mysqlFetchResult();

	// destroy the statement
	this->mysqlDestroyStatement();

	// destroy link before exit
	BP_DestroyLinkL(lookup_tq, BP_TRUE);

	// return the result index if it was filled
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(result_index);
}


// add whitelist
BP_ERROR_T Block::addWebRequestBalance(char * whitelist, size_t weight)
{

	// check to ensure we have a whitelist
	if(!whitelist)
		return ERR_FAILURE;

	// create new balancer if necessary
	if(!this->web_balancer)
	{
		this->web_balancer = BP_CreateDBWebRequestBalancer();
	}

	// create local item and populate with data
	BP_DB_WEB_WEIGHTED_WHITELIST_ELEMENT item = {0};
	memset(&item, 0x00, sizeof(item));

	// set item data
	item.weight = weight;
	strncat((char *)&item.whitelist, whitelist, sizeof(item.whitelist)-1);

	// push the new item on the stack and return with that code
	return BP_DBWebRequestBalancerPushWhitelistItem(this->web_balancer, &item);

}


// Test balancer with href.  If the request would be unbalanced based
// on the whitelist configuration + requests already made, this will return
// ERR_FAILURE, whereas if it is within balancing constraints it will return
// ERR_SUCCESS.
BP_ERROR_T Block::checkWebHrefForBalance(char *href)
{

	// default is success
	if(!this->web_balancer)
		return ERR_SUCCESS;

	// check for href
	if(!href)
		return ERR_FAILURE;

	// return the result of a process attempt
	return BP_DBWebRequestBalancerProcessTarget(this->web_balancer, href);

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Spider Cloning %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// !! Deprecated Notice:  This capacity was decided to be left intact
//    and preserved for potential future use, but in it's current state
//    it is purposefully unimplemented.  This capacity is instead provided
//    by using the thread/splintering model directly.  : End Notice

// Because sometimes (most of the time) we'll want to do spidering in a
// threaded fashion, we can use the methods below to clone spider
// configurations from existing blocks.  The data which is cloned is actually
// allocated as it would be if the Block had allocated them iself.  This
// should allow for a delete to destroy data as per-usual operation.

BP_ERROR_T Block::cloneSpideringDataIntoBlock(Block * block_to_clone_from)
{

	// ensure we have a block
	if(!block_to_clone_from)
		return ERR_FAILURE;

	// These Items Must Be Cloned:
	/*


	// proxy groups that have been populated dynamically
	P_BP_PROXY_DB_GROUP db_populated_proxy_groups;
	size_t              db_populated_proxy_group_n;

	 // This set of proxies is the primary place to find proxy data after requests
	// have been made.
	P_BP_PROXY_DB_ENTRY db_populated_proxy_sets;
	size_t              db_populated_proxy_n;

	P_BP_PROXY_DB_VALIDATION_TEST_GROUP db_populated_proxy_validation_groups;
	size_t								db_populated_proxy_validation_group_n;

	 // Individual validation test sets
	P_BP_PROXY_DB_VALIDATION_TEST_ENTRY db_populated_proxy_validation_tests;
	size_t								db_populated_proxy_validation_test_n;

	 web_balancer
	  // stack of web result data retrieved from db (one pointer, one
	// allocation, only needs to ever be bpfree'd once.  Destroy savely
	// by calling the destroy method below.
	P_BP_WEB_RESULT_ENTRY web_result_stack;

	// number of entries in the web result stack (cleared/updated
	// on pop/destroy)
	size_t web_result_stack_n;

	// current/next index of the web result stored in the web_result_href pointer.
	size_t web_result_stack_current_index;
	size_t web_result_stack_next_index;

	// flag indicating whether or not the stack needs to be reset
	size_t web_result_stack_all_popped;

	// this is filled with a href (url) for the last retrieved/popped record.
	char web_result_href[4096];

	// this is filled with the db index for the last retrieved record
	// and is required for update/insert.
	size_t web_result_id;
	 */

	// These Items Must Not Be Cloned:

	return ERR_SUCCESS;
}

// !! End Deprecated ======

// flushes insert request
BP_ERROR_T Block::flushInsertRequests()
{

	// requires a valid result stack builder
	if(!this->result_stack_builder)
		return ERR_FAILURE;

	// ensure we have items to flush
	if(!this->result_stack_builder->build_str)
		return ERR_FAILURE;

	// ensure we have a valid mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;


	// get string length
	size_t stack_len = bpstrlen(this->result_stack_builder->build_str);

	// null terminate over the trailing comma
	if(this->result_stack_builder->build_str[stack_len-1] == ',')
		this->result_stack_builder->build_str[stack_len-1] = 0x00;

	// Add a literal query constraint to allow the incrementing of individual
	// result hit counts.  Hit counts are indicators which illuminate
	// how many times a specific result has been encountered.
	this->result_stack_builder->BuildString((char *) " ON DUPLICATE KEY UPDATE  results.hits = results.hits+1 ");

	printf("\n [!!] flushing insert requests: %s", this->result_stack_builder->build_str);

	// prepare and execute the query
	this->mysqlPrepareQuery(this->result_stack_builder->build_str);
	this->mysqlExecuteStatement();
	this->mysqlStoreResults();
	this->mysqlDestroyStatement();

	// reset stack builder count
	this->result_stack_n = 0;

	// destroy/nullify stack builder on flush (starts process over)
	delete this->result_stack_builder;
	this->result_stack_builder = NULL;

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// stack new insert result (fast ad-hoc stacking using hex embedding, should be audited)
BP_ERROR_T Block::stackInsertRequest(size_t type, char *href, size_t status_code, size_t parent_id)
{

	// create new stack builder if necessary
	if(!this->result_stack_builder)
	{

		// create new string builder
		this->result_stack_builder = new BP_StringUtils(NULL);

		// set default behavior to duplicate the provided input string and
		// store the allocation as a pointer.
		this->result_stack_builder->duplicate_unique_strings = true;

	}

	// only add unique strings to the current stack (reduces insert overhead
	// further)

	if(this->result_stack_builder->AddUniqueString(href) != true)
		return ERR_FAILURE;


	// add result stack
	this->result_stack_n++;

	// display stacking message
	// printf("\n [+] Stacking unique element %zu", this->result_stack_n);

	// if the stack is empty, build on the front part
	if(!this->result_stack_builder->build_str)
		this->result_stack_builder->BuildString("INSERT INTO `results`( `type`, `href`, `sha1`, `base`, `path`,  `status`, `commit_timestamp`, `audit_start_timestamp`, `audit_completed_timestamp`, `spider_parent_result_id`) VALUES ");

	// set buffers from quick conversions
	char *href_buff        = BP_BufferToHexString(href, bpstrlen(href));
	char *type_buff        = BP_SizeTToBuffer(type);
	char *status_code_buff = BP_SizeTToBuffer(status_code);
	char *parent_id_buff   = BP_SizeTToBuffer(parent_id);

	this->result_stack_builder->BuildString("( ");
	this->result_stack_builder->BuildString(type_buff);
	this->result_stack_builder->BuildString(",0x");
	this->result_stack_builder->BuildString(href_buff);
	this->result_stack_builder->BuildString(",SHA1(0x");
	this->result_stack_builder->BuildString(href_buff);
	this->result_stack_builder->BuildString("),");
	this->result_stack_builder->BuildString("0,0,");
	this->result_stack_builder->BuildString(status_code_buff);
	this->result_stack_builder->BuildString(",NOW(),0,0,");
	this->result_stack_builder->BuildString(parent_id_buff);
	this->result_stack_builder->BuildString("),");

	// destroy builder variables
	bpfree(href_buff);
	bpfree(type_buff);
	bpfree(status_code_buff);
	bpfree(parent_id_buff);


	// check to see how many requests we have stacked
	if(this->result_stack_n == this->result_stacking_flush_rate)
	{

		// flush the requests if we've hit threshold
		// (this destroys all stacking related data and null resets variables)
		this->flushInsertRequests();

	}

	// return indicating success
	return ERR_SUCCESS;

}

// Inserts a new audit target into the db.  This routine automatically does insert stacking
// if configured to do so via class member settings (aka this->enable_result_insert_stacking etc).  Stacking
// is done to conserve database resources when doing inserts.  IN laymans terms, stacking essentially enables
// batch database record insertion.
size_t Block::insertNewResult(size_t type, char *href, size_t status_code, size_t parent_id)
{



	// ensure we have a non-null session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;



	// check parameters
	if (!href)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	if(!BP_StringIsReasonablePrintableString(href, bpstrlen(href), BP_FALSE, BP_FALSE))
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// run basic initial checks
	if(href[0] == '/')
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	if(href[0] == '.')
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	if(href[0] == '#')
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt To Parse Base/Path/Script/Host %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure the result can be parsed out in all
	// metrics, if not, consider it invalid
	char *script = BP_ParseURLToScriptPCRE(href);
	char *path   = BP_ParseURLToPathPCRE(script);
	char *base   = BP_ParseURLToBasePCRE(path);
	char *host   = BP_ParseURLToHostPCRE(base);

	// push results into caches
	if(!script || !path || !base || !host)
	{

		// destroy any which may have been parsed out
		if(script)
			bpfree(script);
		if(path)
			bpfree(path);
		if(base)
			bpfree(base);
		if(host)
			bpfree(host);

		// return indicating failure
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}
	printf("\n [!!] Inserting new result1: %s", href);
	// do insert stacking where necessary
	if(this->enable_result_insert_stacking == true)
	{

		// attempt to stack the insert request
		if(!this->stackInsertRequest(type, href, status_code, parent_id))
		{
			BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
		}

		// destroy all allocated memory
		if(script)
			bpfree(script);
		if(path)
			bpfree(path);
		if(base)
			bpfree(base);
		if(host)
			bpfree(host);

		// return indicating success
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

	}

	printf("\n [!!] Inserting new result2: %s", href);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Do Immediate Insert if We're Not Stacking %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// check to make sure it doesn't exist first (prevent ovewrites)
	if (this->checkResultExists(href, status_code) == ERR_SUCCESS)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// stack the request and flush immediately
	printf("\n [!!] Inserting new result3: %s", href);
	this->stackInsertRequest(type, href, status_code, parent_id);

	// flush immediately
	printf("\n [!!] Inserting new result4: %s", href);
	this->flushInsertRequests();

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// Check if a target exists (ERR_SUCCESS on exists)
size_t Block::checkResultExists(char *href, size_t status_code)
{

	// ensure we have a mysql handle
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// If the sha1 field in the db is set to use a duplicate key entry
	// this flag should be set to true, as it will prevent a lot of
	// db load.
	if(this->disable_result_lookups_due_to_duplicate_key_contention_fix)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	if(!href)
	{
		this->AddErrorToStack((char*) "Empty href passed to checkResultExists().");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// result index
	size_t result_index = 0;

	// set base query
	char * base_query = (char *) NULL;

	// if we're not searching for status code also, limit the query
	if(status_code == (size_t) -1)
	{
		base_query = "SELECT id FROM results where href = ? LIMIT 1";
	}
	else
	{
		base_query = "SELECT id FROM results where href = ? and status = ? LIMIT 1";
	}

	// set query size
	size_t query_size = 0;

	// prepare the query
	if (this->mysqlPrepareQuery(base_query) == ERR_FAILURE)
		this->AddErrorToStack((char*) "Cannot prepare query");

	// add href
	this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) href, bpstrlen(href));

	// add status_code
	if(status_code != (size_t) -1)
	{
		printf("\n Adding parameter here?");
		this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &status_code, sizeof(status_code));
	}

	// add index
	if (this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &result_index, sizeof(result_index)) == ERR_FAILURE)
		this->AddErrorToStack((char*) "Cannot add result");

	// bind the parameters
	this->mysqlBindParameters();

	// run lookup query on url to make sure it doesn't already exist
	if (this->mysqlBindResults() == ERR_FAILURE)
		this->AddErrorToStack((char*) "Cannot bind results");

	// execute the statement
	if (this->mysqlExecuteStatement() == ERR_FAILURE)
	{
		this->AddErrorToStack((char *) "Failed to execute statement.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// store results
	this->mysqlStoreResults();

	// fetch a result (max one result for this query)
	this->mysqlFetchResult();

	// if we have a record count, check
	if(this->mysql_results_count > 0)
	if(this->mysql_results[0].buffer)
	{

		// set from buffer
		size_t *record_id = (size_t *) this->mysql_results[0].buffer;

		// set the record id
		result_index = *record_id;
	}

	// destroy statement
	this->mysqlDestroyStatement();


	// return success if the record exists
	if (result_index)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

	// return failure in the case that it does not exist
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

}


// add auto result generation from page parses that come from the assessment DB
#ifdef BP_BLOCKCLASS_INCLUDE_WEB_ASSESSMENT_BINDINGS


// Inserts anchor data into the db.
size_t Block::insertAnchor(P_BP_HTML_ANCHOR_LIST anchor, size_t result_id)
{

	// ensure we have a mysql handle
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// insert an anchor
	if(!anchor)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// query to insert anchor into db (vals are added below)
	char * anchor_query = (char *) "INSERT INTO `anchor_data` (`id` ,`result_id`,`rel_href` ,`script_name`)VALUES (NULL, ?, ?, ?	);";

	// prepare the query
	this->mysqlPrepareQuery(anchor_query);

	// insert anchor base data first
	this->mysqlAddParameter(MYSQL_TYPE_LONG,   (char *) &result_id,        sizeof(result_id));
	this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) anchor->href,      bpstrlen(anchor->href));
	this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) "empty",           bpstrlen("empty"));

	// bind the parameters
	this->mysqlBindParameters();

	// execute the query
	if(this->mysqlExecuteStatement() != ERR_SUCCESS)
	{
		printf("\n ERR: Could not run anchor insert statement");
	}

	// set the last anchor id
	size_t 					  last_anchor_id   = this->mysqlSession->conn->insert_id;
	P_BP_HTML_ANCHOR_VAR_LIST anchor_vars = anchor->vars;


	// anchor variable insert query
	char * var_query    = (char *) "INSERT INTO `anchor_var_data` (`id` ,`result_id` ,`anchor_id` ,`name` ,`value`)VALUES ('', ?, ?, ?, ?);";


	if(anchor_vars)
	for
	(
		anchor_vars = anchor_vars->first;
		anchor_vars;
		anchor_vars = anchor_vars->next
	)
	{

		// prepare the query
		this->mysqlPrepareQuery(var_query);

		// generate tmp name or empty string if necessary
		char * anchor_var_name_tmp = anchor_vars->name;
		if(!anchor_var_name_tmp)
			anchor_var_name_tmp = "NULL";

		// calculate name length
		size_t anchor_var_name_len_tmp = bpstrlen(anchor_var_name_tmp);

		// generate tmp content or empty string if necessary
		char * anchor_var_content_tmp = anchor_vars->content;
		if(!anchor_var_content_tmp)
			anchor_var_content_tmp = "NULL";

		// calculate name length
		size_t anchor_var_content_len_tmp = bpstrlen(anchor_var_content_tmp);


		// insert anchor base data first
		this->mysqlAddParameter(MYSQL_TYPE_LONG,   (char *) &result_id,               sizeof(result_id));
		this->mysqlAddParameter(MYSQL_TYPE_LONG,   (char *) &last_anchor_id,          sizeof(last_anchor_id));
		this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) anchor_var_name_tmp,      bpstrlen(anchor_var_name_tmp));
		this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) anchor_var_content_tmp,   bpstrlen(anchor_var_content_tmp));


		// bind the parameters
		this->mysqlBindParameters();

		// execute the query
		if(this->mysqlExecuteStatement() != ERR_SUCCESS)
		{
			printf("\n ERR: Could not run anchor insert statement");
		}

	}

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}




// Inserts form data into the db.
size_t Block::insertForm(P_BP_HTML_FORM_LIST form, size_t result_id)
{

	// ensure we have a mysql handle
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// insert an anchor
	if(!form)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% SQL Queries %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// query to insert anchor into db (vals are added below)
	char * form_data_query = (char *) "INSERT INTO `form_data` (	`id` ,`result_id` ,`form_method` ,`form_action`	)VALUES (NULL , ?, ?, ?);";

	// query to insert form property data
	char * form_property_data_query = (char *) "INSERT INTO `form_property_data` (`id`, `result_id`, `form_id`, `property_name`, `property_value`) VALUES (NULL, ?, ?, ?, ?);";

	// query to insert input data
	char * form_input_data_query = (char *) "";

	// query to insert input property data
	char * form_input_property_data_query = (char *) "";


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% End SQL %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



	// set form property list iter
	P_BP_HTML_FORM_PROPERTY_LIST form_prop_list = form->properties;

	// exit if we have no property list
	if(!form_prop_list)
		return ERR_FAILURE;

	// action/form string
	char *action_string = (char *) "no action in form properties.";
	char *method_string = (char *) "no method set in form properties.";

	// walk the property list and find action/method
	for
	(
			form_prop_list = form_prop_list->first;
			form_prop_list;
			form_prop_list = form_prop_list->next
	)
	{

		// continue if the namelength is less than 6 (action and method are both six char strings)
		if(form_prop_list->nLen < 6)
			continue;

		if(BP_strncasecmp(form_prop_list->name, (char *) "action", 6) == 0)
		{
			action_string = form_prop_list->content;
		}
		if(BP_strncasecmp(form_prop_list->name, (char *) "method", 6) == 0)
		{
			method_string = form_prop_list->content;
		}
	}

	// prepare the query
	if(this->mysqlPrepareQuery(form_data_query) == ERR_FAILURE)
	{
		this->AddErrorToStack((char*) "Cannot prepare query.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// add parameters
	this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &result_id, sizeof(result_id));
	this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) method_string, bpstrlen(method_string));
	this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) action_string, bpstrlen(action_string));

	// bind parameters
	this->mysqlBindParameters();

	// execute query
	if(this->mysqlExecuteStatement() == ERR_FAILURE)
	{
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// save the insert id for use further down
	size_t form_insert_id = this->mysqlSession->conn->insert_id;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Insert Form Properties %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	P_BP_HTML_FORM_PROPERTY_LIST properties = form->properties;
	if(form->properties)
	{
		for
		(
			properties = properties->first;
			properties;
			properties = properties->next
		)
		{


			// prepare the query
			if(this->mysqlPrepareQuery(form_property_data_query) == ERR_FAILURE)
			{
				this->AddErrorToStack((char*) "Cannot prepare query form property data query.");
				BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
			}

			char *name_str = (char *) "empty";
			char *val_str  = (char *) "empty";

			if(properties->content)
				val_str = properties->content;
			if(properties->name)
				name_str = properties->name;

			// add parameters
			this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &result_id, sizeof(result_id));
			this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &form_insert_id, sizeof(form_insert_id));
			this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) name_str, bpstrlen(name_str));
			this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) val_str,  bpstrlen(val_str));

			// bind parameters
			this->mysqlBindParameters();

			// execute query
			if(this->mysqlExecuteStatement() == ERR_FAILURE)
			{
				BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
			}

			// save the insert id for use further down
			size_t anchor_prop_insert_id = this->mysqlSession->conn->insert_id;

		}

	}

	// %%%%%%%%%%%%%%%%%% Insert Form Inputs %%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// %%%%%%%%%%%%%%%%%% Insert Form Input Properties %%%%%%%%%%%%%%%%%%

/*
	// prepare the query
	this->mysqlPrepareQuery(anchor_query);

	// insert anchor base data first
	this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &result_id,            sizeof(result_id));
	this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) anchors->href,         bpstrlen(anchors->href));
	this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) "empty",         bpstrlen("empty"));

	// bind the parameters
	this->mysqlBindParameters();

	// execute the query
	if(this->mysqlExecuteStatement() != ERR_SUCCESS)
	{
		printf("\n ERR: Could not run anchor insert statement");
	}

	// set the last anchor id
	size_t last_anchor_id = this->mysqlSession->conn->insert_id;
	P_BP_HTML_ANCHOR_VAR_LIST anchor_vars = anchors->vars;

	if(anchor_vars)
	for
	(
		anchor_vars = anchor_vars->first;
		anchor_vars;
		anchor_vars = anchor_vars->next
	)
	{

		// prepare the query
		this->mysqlPrepareQuery(var_query);

		// insert anchor base data first
		this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &result_id,              sizeof(result_id));
		this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &last_anchor_id,         sizeof(last_anchor_id));
		this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) anchor_vars->name,     strnlen(anchor_vars->name, 2048));
		this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) anchor_vars->content,  strnlen(anchor_vars->content, 2048));

		// bind the parameters
		this->mysqlBindParameters();

		// execute the query
		if(this->mysqlExecuteStatement() != ERR_SUCCESS)
		{
			printf("\n ERR: Could not run anchor insert statement");
		}

	}
	*/

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}


// If this block class has an existing parsed http session
// attached to it in its local class pointer, that structure
// will be parsed out in order to generate new (unique) targets
// automatically.  IMPORTANT: THIS ESSENTIALLY CREATES A SPIDER
// WHEN COUPLED WITH THE ASSESSMENT AUDITOR.
//
// Note Regarding Database Record Insertion:
//    Depending on how your block is configured, this function can
//    either perform immediate insertion, or delayed insertion.  The
//    insertion changes happen within the blocks insert routines, and
//    are transparent within the method below.  If you are experiencing
//    strage database behavior with regards to when inserts occur, check
//    your block configuration and ensure that delayed insert settings are
//    configured correctly.
//
BP_ERROR_T Block::extractNewResultsFromHttpParse()
{

	// return indicating failure if we have no provided extraction
	if(!this->htmlExtraction)
		return ERR_FAILURE;

	// check to see if we dont have any anchors or forms to pull results from
	if(!this->htmlExtraction->anchors && !this->htmlExtraction->forms)
		return ERR_FAILURE;

	// NOTE: Don't use critical sections directly here as this function can take a long time
	//       to run, and it's better practice to let the member routines used in this method
	//       to do locking on a more granular level (they all lock)

	// create walker lists for both anchors and forms
	P_BP_HTML_ANCHOR_LIST anchors = this->htmlExtraction->anchors;
	P_BP_HTML_FORM_LIST   forms   = this->htmlExtraction->forms;

	// extract targets from anchors
	if(anchors)
	for(anchors = anchors->first; anchors; anchors = anchors->next)
	{

		// only walk list if href exists
		if(!anchors->href)
			continue;


		// check to make sure the record doesn't exist with any assessment
		// status at all. (check 0,1,2).  If it doesn't exist in the target
		// database, create a new record (type 4 == anchor auto).
		if
		(
			( this->checkResultExists(anchors->href, 0) != ERR_SUCCESS ) &&
			( this->checkResultExists(anchors->href, 1) != ERR_SUCCESS ) &&
			( this->checkResultExists(anchors->href, 2) != ERR_SUCCESS )
		)
		{

			// if the protocol is http/https, its a direct link, whereas if its
			// BP_ANCHOR_PROTO_DEFAULT, its a relative link.
			if(anchors->proto == BP_ANCHOR_PROTO_HTTP || anchors->proto == BP_ANCHOR_PROTO_HTTPS)
			{



				// insert the new result with status code 4 (anchor auto)
				this->insertNewResult(4, anchors->full_url, 0, this->web_result_id);

				// get the last insert id
				size_t last_result_id = this->mysqlSession->conn->insert_id;

				// actually insert the anchor here
				if(this->enable_anchor_data_logging_during_extraction)
				{
					// printf("\n BP_ANCHOR_PROTO_HTTP(s):", anchors->href);

					this->insertAnchor(anchors, last_result_id);

				}

				// create temporary target buffer
				char target_buff[2048] = {0};

				// add the data to a buffer before saving it in errors
				snprintf(target_buff, 2040, "\n [+] Adding Direct Target: %s", anchors->href);

				// log the data
				this->AddErrorToStack(target_buff);

			}
			else if(anchors->proto == BP_ANCHOR_PROTO_BASE_RELATIVE_LINK)
			{

				// calculate required size
				size_t build_buffer_size = strnlen(this->httpSession->lastRequestedHref, (8 * BP_KILOBYTE)) + strnlen(anchors->href, (8 * BP_KILOBYTE))+128;

				// create buffer to handle the relative link builder
				char *rebuilt_relative_link = (char *) calloc(1, build_buffer_size);
				if(!rebuilt_relative_link)
				{
					this->AddErrorToStack((char *) "\n [+] Could not allocate memory.  Likely OOM.");
					exit(0);
				}

				// base builder string
				char *tmp_builder = NULL;

				// Relative or direct path, chosen based on the anchor first char.  A
				// '/' char indicates a base path.
				tmp_builder = BP_ParseURLToBasePCRE(this->httpSession->lastRequestedHref);

				// null builder check (badval)
				if(!tmp_builder)
				{
					printf("\n [+] Error: Bad Relative Link: %s", this->httpSession->lastRequestedHref);
					free(rebuilt_relative_link);
					continue;
				}

				// no length check (badval)
				if(!bpstrlen(tmp_builder))
				{
					free(tmp_builder);
					tmp_builder=NULL;
					tmp_builder = (char *) strdup("http://grief3badparsedrelativebadlen/");
				}

				// truncation check
				if(tmp_builder[bpstrlen(tmp_builder)-1] != '/')
				{

					// calculate write offset
					size_t write_offs = (size_t) bpstrlen(tmp_builder);

					// reallocate the data
					tmp_builder = (char *) bprealloc(tmp_builder, bpstrlen(tmp_builder)+2);

					// set the truncation chars
					tmp_builder[write_offs]   = '/';
					tmp_builder[write_offs+1] = (char) 0x00;

				}

				//printf("\n RELATIVE_LINK: %s", this->httpSession->lastRequestedHref);
				//printf("\n BP_ANCHOR_PROTO_BASE_RELATIVE_LINK tmp_builder: %s", tmp_builder);
				//printf("\n BP_ANCHOR_PROTO_BASE_RELATIVE_LINK rebubuild:   %s", rebuilt_relative_link);

				// tack on the reduced path
				if(!rebuilt_relative_link)
				{
					printf("\n [+] Error: Failed to create rebuilt relative link for anchor: %s", anchors->href);
				}
				else
				{
					// safely copy in the buffer
					strncat(rebuilt_relative_link, tmp_builder, build_buffer_size-1);
				}

				// printf("\n Got rebuilt?: %s", rebuilt_relative_link);
				if(tmp_builder)
				{
					free(tmp_builder);
					tmp_builder = NULL;
				}

				// create built buffer (strcat OK!, non-vuln)
				if(rebuilt_relative_link)
				{

					if(anchors->href[0] == '/')
						strncat(rebuilt_relative_link, &anchors->href[1], build_buffer_size-1);
					else
						strncat(rebuilt_relative_link, anchors->href, build_buffer_size-1);

				}

				if(rebuilt_relative_link)
				if(
					( this->checkResultExists(rebuilt_relative_link,  0) != ERR_SUCCESS ) &&
					( this->checkResultExists(rebuilt_relative_link,  1) != ERR_SUCCESS ) &&
					( this->checkResultExists(rebuilt_relative_link,  2) != ERR_SUCCESS )
				)
				{

				//	printf("\n BP_ANCHOR_PROTO_BASE_RELATIVE_LINK: %s", rebuilt_relative_link);
					// insert new buffer into the database
					this->insertNewResult(4, rebuilt_relative_link, 0, this->web_result_id);

					// get the last insert id
					size_t last_result_id = this->mysqlSession->conn->insert_id;

					// insert anchor
					if(this->enable_anchor_data_logging_during_extraction)
						this->insertAnchor(anchors, last_result_id);

					// display addition message
					// this->AddErrorToStack((char*) "Adding Relative Target: %s ", rebuilt_relative_link);

				}
				else
				{
					// this->AddErrorToStack((char*) "\n Omitting (already exists): %s", rebuilt_relative_link);
				}

				// destroy rebuild if necessary
				if(rebuilt_relative_link)
				{
					free(rebuilt_relative_link);
					rebuilt_relative_link = NULL;
				}
			}
			else if(anchors->proto == BP_ANCHOR_PROTO_RELATIVE_LINK)
			{

				char * rebuilt_relative_link = BP_CreateDirectUrlFromRel(this->httpSession->lastRequestedHref, anchors->href);

				if(
					( this->checkResultExists(rebuilt_relative_link,  0) != ERR_SUCCESS ) &&
					( this->checkResultExists(rebuilt_relative_link,  1) != ERR_SUCCESS ) &&
					( this->checkResultExists(rebuilt_relative_link,  2) != ERR_SUCCESS )
				)
				{

					// insert new buffer into the database
					this->insertNewResult(4, rebuilt_relative_link, 0, this->web_result_id);

					// printf("\n BP_ANCHOR_PROTO_RELATIVE_LINK: %s", rebuilt_relative_link);


					// get the last insert id
					size_t last_result_id = this->mysqlSession->conn->insert_id;
					if(this->enable_anchor_data_logging_during_extraction)
						this->insertAnchor(anchors, last_result_id);

				}
				else
				{
					// this->AddErrorToStack((char*) "\n Omitting (already exists): %s", rebuilt_relative_link);
				}

				// destroy rebuilt relative link
				if(rebuilt_relative_link)
				{
					free(rebuilt_relative_link);
					rebuilt_relative_link = NULL;
				}

			}
			else if(anchors->proto == BP_ANCHOR_PROTO_RELATIVE_LINK)
			{


				char * rebuilt_relative_link = BP_CreateDirectUrlFromRel(this->httpSession->lastRequestedHref, anchors->href);
				if(
					( this->checkResultExists(rebuilt_relative_link,  0) != ERR_SUCCESS ) &&
					( this->checkResultExists(rebuilt_relative_link,  1) != ERR_SUCCESS ) &&
					( this->checkResultExists(rebuilt_relative_link,  2) != ERR_SUCCESS )
				)
				{

					// printf("\n BP_ANCHOR_PROTO_RELATIVE_LINK: %s", rebuilt_relative_link);
					// insert new buffer into the database
					this->insertNewResult(4, rebuilt_relative_link, 0, this->web_result_id);

					// get the last insert id
					size_t last_result_id = this->mysqlSession->conn->insert_id;
					if(this->enable_anchor_data_logging_during_extraction)
						this->insertAnchor(anchors, last_result_id);

				}
				else
				{
					// this->AddErrorToStack((char*) "\n Omitting (already exists): %s", rebuilt_relative_link);
				}

				// destroy rebuilt relative link
				if(rebuilt_relative_link)
				{
					free(rebuilt_relative_link);
					rebuilt_relative_link = NULL;
				}

			}
			else
			{
				// printf("\n Getting here?WAT!: %u", anchors->proto);
			}

		}
		else
		{
			// this->AddErrorToStack((char*) "\n Omitting (already exists): %s", anchors->href);
		}

	}

	// extract targets from forms
	if(forms)
	for(forms = forms->first; forms; forms = forms->next)
	{

		// property list iter
		P_BP_HTML_FORM_PROPERTY_LIST properties = NULL;

		// temporary href used during looping
		char *href = NULL;

		// iterate through properties and find the target if it exists
		if(forms->properties)
		for
		(
				properties = forms->properties->first;
				properties;
				properties = properties->next
		)
		{

			// ensure the property is the correct length
			if(properties->nLen != bpstrlen("action"))
				continue;

			// check (case insensitive) for action parameter
			if(strncasecmp("action", properties->name, properties->nLen) != 0)
				continue;

			// set content as href
			href = properties->content;

		}

		// only walk list if href exists
		if(!href)
			continue;

		// check to make sure the record doesn't exist with any assessment
		// status at all. (check 0,1,2).  If it doesn't exist in the target
		// database, create a new record (type 4 == anchor auto).
		if
		(
			( this->checkResultExists(href, 0) != ERR_SUCCESS ) &&
			( this->checkResultExists(href, 1) != ERR_SUCCESS ) &&
			( this->checkResultExists(href, 2) != ERR_SUCCESS )
		)
		{

			// get an "anchor" protocol from the target (URI)
			BP_ANCHOR_PROTO proto = HTMLGetAnchorProtocol(href, bpstrlen(href));

			// if the protocol is http/https, its a direct link, whereas if its
			// BP_ANCHOR_PROTO_DEFAULT, its a relative link.
			if(proto == BP_ANCHOR_PROTO_HTTP || proto == BP_ANCHOR_PROTO_HTTPS)
			{

				printf(" [!!] Adding form target: %s", href);

				// insert the new result with status code 5 (form auto)
				this->insertNewResult(5, href, 0, this->web_result_id);

				// save last result id after insert and use it for form insertion
				size_t last_result_id = this->mysqlSession->conn->insert_id;
				if(this->enable_form_data_logging_during_extraction)
				{
					this->insertForm(forms, last_result_id);
				}


			}
			else if(proto == BP_ANCHOR_PROTO_DEFAULT)
			{

				if(href)
				if(bpstrlen(href))
				{
					// calculate required size
					size_t build_buffer_size = bpstrlen(this->httpSession->lastRequestedHref) + bpstrlen(href)+128;

					// create buffer to handle the relative link builder
					char *rebuilt_relative_link = (char *) calloc( 1, build_buffer_size);

					// base builder string
					char *tmp_builder = NULL;

					// Relative or direct path, chosen based on the anchor first char.  A
					// '/' char indicates a base path.  A pound can also indicate that.
					if(href[0] == '/' || href[0] == '#')
						tmp_builder = BP_ParseURLToBasePCRE(this->httpSession->lastRequestedHref);
					else
						tmp_builder = BP_ParseURLToPathPCRE(this->httpSession->lastRequestedHref);

					if(!tmp_builder)
						tmp_builder = (char *) strdup("http://grief3badparsedrelativeanchor/");
					if(!bpstrlen(tmp_builder))
						tmp_builder = (char *) strdup("http://grief3badparsedrelativebadlenanchor/");

					// tack on the reduced pathprint
					strncat(rebuilt_relative_link, tmp_builder, bpstrlen(this->httpSession->lastRequestedHref));

					// check to ensure that the path is terminated with a '/', if not add one
					if(rebuilt_relative_link[bpstrlen(rebuilt_relative_link)-1] != '/')
						strcat(rebuilt_relative_link, "/");

					// create built buffer
					if(href[0] == '/')
						strncat(rebuilt_relative_link, &href[1], bpstrlen(href));
					else
						strncat(rebuilt_relative_link, href, bpstrlen(href));


					if(
						( this->checkResultExists(rebuilt_relative_link,  0) != ERR_SUCCESS ) &&
						( this->checkResultExists(rebuilt_relative_link,  1) != ERR_SUCCESS ) &&
						( this->checkResultExists(rebuilt_relative_link,  2) != ERR_SUCCESS )
					)
					{
						printf("\n BP_ANCHOR_PROTO_DEFAULT: %s", rebuilt_relative_link);
						// insert new buffer into the database (5 = form auto)
						this->insertNewResult(5, rebuilt_relative_link, 0, this->web_result_id);

						// save last result id after insert and use it for form insertion
						size_t last_result_id = this->mysqlSession->conn->insert_id;
						if(this->enable_form_data_logging_during_extraction)
						{
							this->insertForm(forms, last_result_id);
						}

						// display addition message
						// this->AddErrorToStack((char*) "\n [+] Adding Relative Target: %s ", rebuilt_relative_link);

					}
					else
					{
						// this->AddErrorToStack((char*) "\n [+] Omitting (form target already exists): %s", rebuilt_relative_link);
					}

					if(rebuilt_relative_link)
					{
						free(rebuilt_relative_link);
						rebuilt_relative_link = NULL;
					}

					if(tmp_builder)
					{
						// this free has to be a non-tq version due to where its allocated
						/**/free(tmp_builder);
						tmp_builder=NULL;
					}
				}

			}

		}
		else
		{
			// this->AddErrorToStack((char*) "\n [+] Omitting (form target already exists): %s", href);
		}

	}

	return ERR_SUCCESS;
}


// Extracts results from preg url matching regular expression (uses retrieved page membuffer as search).  This
// routine will use this->httpSession data if no buffer is passed in)
BP_ERROR_T Block::extractNewResultsFromPREGSearch
(
		char * buff_search,
		size_t buff_len
)
{

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// if we don't have a buffer to search, we try to set buffer from
	// http session.
	if(!buff_search && !buff_len)
	{

		// run session checks
		if(!this->httpSession)
			return ERR_FAILURE;
		if(!this->httpSession->page)
			return ERR_FAILURE;
		if(!this->httpSession->page->page_data)
			return ERR_FAILURE;
		if(!this->httpSession->page->page_data_size)
			return ERR_FAILURE;

		// set the search from input if not provided already
		buff_search = this->httpSession->page->page_data;
		buff_len    = this->httpSession->page->page_data_size;

	}

	// loose url grabbing regexp
	char * regexp = "[a-zA-Z0-9\-]{1,}\\:\\/\\/[\\!\\:\\@\\.a-zA-Z0-9\\-]{1,}(?:[\\.a-zA-Z0-9\\-]{1,})(?:\\.construction|\\.photography|\\.enterprises|\\.contractors|\\.technology|\\.equipment|\\.directory|\\.ventures|\\.plumbing|\\.lighting|\\.holdings|\\.graphics|\\.diamonds|\\.clothing|\\.singles|\\.kitchen|\\.gallery|\\.voyage|\\.travel|\\.tattoo|\\.museum|\\.estate|\\.camera|\\.today|\\.tips|\\.sexy|\\.post|\\.name|\\.mobi|\\.menu|\\.land|\\.jobs|\\.info|\\.guru|\\.coop|\\.bike|\\.asia|\\.arpa|\\.aero|\\.xxx|\\.uno|\\.tel|\\.pro|\\.org|\\.net|\\.mil|\\.int|\\.gov|\\.edu|\\.com|\\.cat|\\.biz|\\.zw|\\.zm|\\.za|\\.yt|\\.ye|\\.ws|\\.wf|\\.vu|\\.vn|\\.vi|\\.vg|\\.ve|\\.vc|\\.va|\\.uz|\\.uy|\\.us|\\.uk|\\.ug|\\.ua|\\.tz|\\.tw|\\.tv|\\.tt|\\.tr|\\.tp|\\.to|\\.tn|\\.tm|\\.tl|\\.tk|\\.tj|\\.th|\\.tg|\\.tf|\\.td|\\.tc|\\.sz|\\.sy|\\.sx|\\.sv|\\.su|\\.st|\\.sr|\\.so|\\.sn|\\.sm|\\.sl|\\.sk|\\.sj|\\.si|\\.sh|\\.sg|\\.se|\\.sd|\\.sc|\\.sb|\\.sa|\\.rw|\\.ru|\\.rs|\\.ro|\\.re|\\.qa|\\.py|\\.pw|\\.pt|\\.ps|\\.pr|\\.pn|\\.pm|\\.pl|\\.pk|\\.ph|\\.pg|\\.pf|\\.pe|\\.pa|\\.om|\\.nz|\\.nu|\\.nr|\\.np|\\.no|\\.nl|\\.ni|\\.ng|\\.nf|\\.ne|\\.nc|\\.na|\\.mz|\\.my|\\.mx|\\.mw|\\.mv|\\.mu|\\.mt|\\.ms|\\.mr|\\.mq|\\.mp|\\.mo|\\.mn|\\.mm|\\.ml|\\.mk|\\.mh|\\.mg|\\.me|\\.md|\\.mc|\\.ma|\\.ly|\\.lv|\\.lu|\\.lt|\\.ls|\\.lr|\\.lk|\\.li|\\.lc|\\.lb|\\.la|\\.kz|\\.ky|\\.kw|\\.kr|\\.kp|\\.kn|\\.km|\\.ki|\\.kh|\\.kg|\\.ke|\\.jp|\\.jo|\\.jm|\\.je|\\.it|\\.is|\\.ir|\\.iq|\\.io|\\.in|\\.im|\\.il|\\.ie|\\.id|\\.hu|\\.ht|\\.hr|\\.hn|\\.hm|\\.hk|\\.gy|\\.gw|\\.gu|\\.gt|\\.gs|\\.gr|\\.gq|\\.gp|\\.gn|\\.gm|\\.gl|\\.gi|\\.gh|\\.gg|\\.gf|\\.ge|\\.gd|\\.gb|\\.ga|\\.fr|\\.fo|\\.fm|\\.fk|\\.fj|\\.fi|\\.eu|\\.et|\\.es|\\.er|\\.eg|\\.ee|\\.ec|\\.dz|\\.do|\\.dm|\\.dk|\\.dj|\\.de|\\.cz|\\.cy|\\.cx|\\.cw|\\.cv|\\.cu|\\.cr|\\.co|\\.cn|\\.cm|\\.cl|\\.ck|\\.ci|\\.ch|\\.cg|\\.cf|\\.cd|\\.cc|\\.ca|\\.bz|\\.by|\\.bw|\\.bv|\\.bt|\\.bs|\\.br|\\.bo|\\.bn|\\.bm|\\.bj|\\.bi|\\.bh|\\.bg|\\.bf|\\.be|\\.bd|\\.bb|\\.ba|\\.az|\\.ax|\\.aw|\\.au|\\.at|\\.as|\\.ar|\\.aq|\\.ao|\\.an|\\.am|\\.al|\\.ai|\\.ag|\\.af|\\.ae|\\.ad|\\.ac){1,}(?:[\\:\\/0-9]{0,})\\/(?:[0-9a-zA-Z\\-\\~]{1,}[\\/]){0,}[a-zA-Z0-9\\.\\:\\-\\?\\&\\(\\)\\=\\*\\%%\\$\\#\\@\\!\\,]{0,}";

	// create zeroed out inout structure
	BP_REGEX_MATCH_INFO match_info = {0};

	// get matches
	size_t matched = BP_MatchRegularExpressionPCRE
	(
			buff_search,
			buff_len,
			regexp,
			0,
			1024,
			&match_info,
			true
	);


	// check to see if we matched ok
	if(matched == ERR_SUCCESS)
	{

		// iterate through matches
		size_t n = 0;
		for
		(
				;
				n < match_info.pcre_num_matched;
				n++
		)
		{

			// run basic checks
			if(match_info.preg_matches[n].matched_string[bpstrlen(match_info.preg_matches[n].matched_string)-1] == '"')
				match_info.preg_matches[n].matched_string[bpstrlen(match_info.preg_matches[n].matched_string)-1] = '/';

			// only insert if the result doesn't exist
			if(
				( this->checkResultExists(match_info.preg_matches[n].matched_string,  0) != ERR_SUCCESS ) &&
				( this->checkResultExists(match_info.preg_matches[n].matched_string,  1) != ERR_SUCCESS ) &&
				( this->checkResultExists(match_info.preg_matches[n].matched_string,  2) != ERR_SUCCESS )
			)
			{

				// check for insertion
				char *tmp = match_info.preg_matches[n].matched_string;

				// Think this was causing a bug where items would insert with a missing trucation
				// if(!isalpha(tmp[bpstrlen(tmp)-1]))
				// 	tmp[bpstrlen(tmp)-1] = (char ) 0x00;


				// display message if desired
				printf("\n Attempting to insert: %s", match_info.preg_matches[n].matched_string);

				// insert the new result
				this->insertNewResult(10, match_info.preg_matches[n].matched_string, 0, this->web_result_id);

			}

		}

	}

	// clear out match info
	BP_DestroyRegexpMatchInfo(&match_info);

	// return indicating success
	return ERR_SUCCESS;

}

// attempt to extract emails from memory buffer.This
// routine will use this->httpSession data if no buffer is passed in)
BP_ERROR_T Block::extractEmailsFromPREGSearch
(
		char * buff_search,
		size_t buff_len
)
{

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// if we don't have a buffer to search, we try to set buffer from
	// http session.
	if(!buff_search && !buff_len)
	{

		// run session checks
		if(!this->httpSession)
			return ERR_FAILURE;
		if(!this->httpSession->page)
			return ERR_FAILURE;
		if(!this->httpSession->page->page_data)
			return ERR_FAILURE;
		if(!this->httpSession->page->page_data_size)
			return ERR_FAILURE;


		// set the search from input if not provided already
		buff_search = this->httpSession->page->page_data;
		buff_len    = this->httpSession->page->page_data_size;

	}


	// loose url grabbing regexp (can result in false positives, but not very often, treat
	// results from this regexp with regard to that taint effect)
 	char * regexp = "[\\w-]+(?:\\.[\\w-]+)*@(?:[\\w-]+\\.)+[a-zA-Z]{2,7}";

	// create zeroed out inout structure
	BP_REGEX_MATCH_INFO match_info = {0};

	// get matches
	size_t matched = BP_MatchRegularExpressionPCRE
	(
		buff_search,
		buff_len,
		regexp,
		0,
		1024,
		&match_info,
		true
	);

	// if we have a match we can attempt to insert the email
	if(matched == ERR_SUCCESS)
	{

		// walk matches and attempt to insert
		size_t n = 0;
		for
		(
			;
			n < match_info.pcre_num_matched;
			n++
		)
		{

			// calculate length of matched string
			size_t len = bpstrlen(match_info.preg_matches[n].matched_string);

			// ensure string is reasonable printable string
			if(!BP_StringIsReasonablePrintableString(match_info.preg_matches[n].matched_string, len, BP_FALSE, BP_FALSE))
				continue;

			// basic check
			if(!isalpha(match_info.preg_matches[n].matched_string[len-1]))
				match_info.preg_matches[n].matched_string[len-1] = '\0';

			// insert email
			this->insertDiscoveredEmail(match_info.preg_matches[n].matched_string);

		}

	}

	// clear out match info
	BP_DestroyRegexpMatchInfo(&match_info);

	// return indicating success
	return ERR_SUCCESS;

}

// Insert an email into the discovered emails list.
size_t Block::insertDiscoveredEmail(char *email_address)
{

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// check if we have an email address
	if(!email_address)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	if(!this->mysqlSession)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);


	// update query
	char * query = (char *) "INSERT INTO `discovered_emails`(`result_id`, `sha1`, `email`) VALUES (?,  SHA1(?), ? )";


	if (this->mysqlPrepareQuery((char *) query ) == ERR_FAILURE)
	{

		this->AddErrorToStack((char*) "\n Cannot prepare query");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}

	// prepare query
	this->mysqlPrepareQuery(query);

	// add result type
	this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &this->web_result_id, sizeof(this->web_result_id));

	// add href
	this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) email_address, bpstrlen(email_address));

	// add sha1
	this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) email_address, bpstrlen(email_address));

	// bind parameters
	this->mysqlBindParameters();

	// execute query
	this->mysqlExecuteStatement();

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// if a jit check results in a match, you can use this to store the match in the db
// Uses member data for propagation.
BP_ERROR_T Block::insertWebJITExpressionMatches()
{

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;


	if(!this->web_request_jit_match_n)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	size_t n = 0;
	for(; n < this->web_request_jit_match_n; n++)
	{

		// walk through each pcre match
		size_t j = 0;
		for(; j < this->web_request_jit_match_set[n].match_info.pcre_num_matched; j++)
		{

			// set insert values
			size_t group_id  = this->web_request_jit_match_set[n].group_id;
			size_t test_id   = this->web_request_jit_match_set[n].id;
			size_t result_id = this->web_result_id;
			char * match_val = this->web_request_jit_match_set[n].match_info.preg_matches[j].matched_string;
			size_t match_len = this->web_request_jit_match_set[n].match_info.preg_matches[j].match_len;

			// insert the expression match
			this->insertWebJITExpressionMatch(group_id, test_id, result_id, match_val, match_len);

		}

	}

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}


// if a jit check results in a match, you can use this to store the match in the db
BP_ERROR_T Block::insertWebJITExpressionMatch(size_t group_id, size_t test_id, size_t result_id, char * match_val, size_t match_len)
{

	// ensure we have a match value
	if(!match_val)
		return ERR_FAILURE;

	// ensure we have a mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// update query
	char * query = (char *) "INSERT INTO `web_jit_expression_matches`(`group_id`, `test_id`, `result_id`, `sha1`, `matched`) VALUES (?, ?,  ?, SHA1(?), ? )";

	// prepare the query
	if (this->mysqlPrepareQuery((char *) query ) == ERR_FAILURE)
	{

		this->AddErrorToStack((char*) "\n Cannot prepare query");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}

	// prepare query
	this->mysqlPrepareQuery(query);

	// add result type
	this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &group_id, sizeof(group_id));
	this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &test_id, sizeof(test_id));
	this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &this->web_result_id, sizeof(this->web_result_id));

	// add match
	this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) match_val, match_len);

	// add sha1
	this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) match_val, match_len);

	// bind parameters
	this->mysqlBindParameters();

	// execute query
	this->mysqlExecuteStatement();

	// destroy the statement after the query runs
	this->mysqlDestroyStatement();

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// Appends insert data to a delayed insert string.  Allows for multiple
// inserts to be done very quickly.
BP_ERROR_T Block::insertWebJITExpressionMatchDelayed(size_t group_id, size_t test_id, size_t result_id, char * match_val, size_t match_len)
{

	// create a new builder
	BP_StringUtils * builder = new BP_StringUtils(NULL);

	// set the initial string if necessary
	if(this->jit_delayed_insert_sql)
		builder->BuildString(this->jit_delayed_insert_sql);

	// destroy the data at this point
	if(this->jit_delayed_insert_sql)
		tq_free(this->tq, this->jit_delayed_insert_sql);

	// nullify the delayed insert sql
	this->jit_delayed_insert_sql = NULL;

	// set ids
	char group_id_varchar[64]  = {0};
	char test_id_varchar[64]   = {0};
	char result_id_varchar[64] = {0};
	char match_len_varchar[64] = {0};

	// embed data
	sprintf((char *) &group_id_varchar,  "%u", group_id);
	sprintf((char *) &test_id_varchar,   "%u", test_id);
	sprintf((char *) &result_id_varchar, "%u", result_id);
	sprintf((char *) &match_len_varchar, "%u", match_len);

	// convert match to hex for insert protection (prevents sqli)
	char * match = BP_BufferToHexString(match_val, match_len);

	// add a set entry
	builder->BuildString("( ");
	builder->BuildString((char *) &group_id_varchar);
	builder->BuildString(",");
	builder->BuildString((char *) &test_id_varchar);
	builder->BuildString(",");
	builder->BuildString((char *) &result_id_varchar);
	builder->BuildString(",sha1(0x");
	builder->BuildString((char *) match);
	builder->BuildString((char *) ")");
	builder->BuildString(",0x");
	builder->BuildString((char *) match);
	builder->BuildString("),");


	// duplicate builder as delayed insert set
	this->jit_delayed_insert_sql = tq_strdup(this->tq, builder->build_str, BPLN);

	// destroy the match
	tq_free(this->tq, match);
	delete builder;

	// return indicating success
	return ERR_SUCCESS;

}

// write delayed jit entries to the database
BP_ERROR_T Block::flushWebJITExpressionMatchDelayedSQL()
{

	// ensure we have a session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// ensure we have a sql value set
	if(!this->jit_delayed_insert_sql)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// create a new builder to finish the quey
	BP_StringUtils * builder = new BP_StringUtils(NULL);

	// remove trailing comma if necessary
	if(this->jit_delayed_insert_sql[bpstrlen(this->jit_delayed_insert_sql)-1] == ',')
		this->jit_delayed_insert_sql[bpstrlen(this->jit_delayed_insert_sql)-1] = (char) 0x00;

	// build in the values
	builder->BuildString((char *) "INSERT INTO `web_jit_expression_matches` (group_id, test_id, result_id, sha1, matched) VALUES ");
	builder->BuildString(this->jit_delayed_insert_sql);

	// do the deed
	this->mysqlDestroyStatement();
	this->mysqlPrepareQuery(builder->build_str);
	this->mysqlExecuteStatement();
	this->mysqlDestroyStatement();

	// destroy data
	delete builder;

	// free item
	tq_free(this->tq, this->jit_delayed_insert_sql);
	this->jit_delayed_insert_sql = NULL;

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}


// extract match data from JIT match set
BP_ERROR_T Block::extractDataFromJITMatches()
{

	// NOTE: Don't use critical sections here.  The insert routines do it automatically on a more granular level.

	if(!this->httpSession)
		return ERR_FAILURE;
	if(!this->httpSession->page)
		return ERR_FAILURE;
	if(!this->httpSession->page->page_data)
		return ERR_FAILURE;
	if(!this->httpSession->page->page_data_size)
		return ERR_FAILURE;

	char * regexp = NULL;
	for
	(
			size_t x = 0;
			x < this->web_request_jit_match_n;
			x++
	)
	{

		// set regular expression
		regexp = (char *) &this->web_request_jit_match_set[x].regexp;

		// create zeroed out inout structure
		BP_REGEX_MATCH_INFO match_info = {0};

		// get matches
		// printf("\n Using regex: %s", regexp);
		size_t matched = BP_MatchRegularExpressionPCRE
		(
				this->httpSession->page->page_data,
				this->httpSession->page->page_data_size,
				regexp,
				PCRE_CASELESS | PCRE_MULTILINE,
				256,
				(P_BP_REGEX_MATCH_INFO )&match_info,
				true
		);

		if(matched == ERR_SUCCESS)
		{
			size_t n = 0;
			for(; n < match_info.pcre_num_matched; n++)
			{

				// ensure we have a matched string
				if(!match_info.preg_matches[n].matched_string)
					continue;

				// get match len
				size_t len = match_info.preg_matches[n].match_len;

				// check len and null terminate if necessary
				if(!isalpha(match_info.preg_matches[n].matched_string[len-1]))
					match_info.preg_matches[n].matched_string[len-1] = '\0';

				// insert match here
				this->insertWebJITExpressionMatchDelayed
				(
						this->web_request_jit_match_set[x].group_id,
						this->web_request_jit_match_set[x].id,
						this->web_result_id,
						match_info.preg_matches[n].matched_string,
						match_info.preg_matches[n].match_len
				);

			}

		}

		// clear out match info
		BP_DestroyRegexpMatchInfo(&match_info);

	}

	// flush db writes now
	this->flushWebJITExpressionMatchDelayedSQL();

	// return indicating success
	return ERR_SUCCESS;

}


// retrieve jit expressions from database (if expressions already exist, they are destroyed
// first.  This is functionally retrieve + update on loop.  Set elements to zero values to
// omit them from search.  All zero values for ALL tests.
BP_ERROR_T Block::retrieveWebJITExpressionsFromDB(size_t db_id, size_t group_id, size_t user_id, size_t bottom_limit, size_t top_limit)
{

	// this method requires a valid mysql session
	if(!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// exit if the mysqli object is unset
	if(!this->mysqlSession->conn)
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%% Build Query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new string builder
	BP_StringUtils * query_builder = new BP_StringUtils((char *) "");

	// select query to run
	char *base_query = (char *) "SELECT "\
							" `id`, `user_id`, `group_id`, `expression`, `description`, `created_ts` "\
							"FROM `web_jit_expression_tests` WHERE 1=1 ";

	// add the base query
	query_builder->BuildString(base_query);

	// dynamically add initial query constraints
	if(db_id)
		query_builder->BuildString( (char *) " AND `id` = ?");
	if(user_id)
		query_builder->BuildString( (char *) " AND `user_id` = ?");
	if(group_id)
		query_builder->BuildString( (char *) " AND `group_id` = ?");

	// create simple limit string
	char limit_str[256] = {0};

	// populate the limit string (transfer lengths precalculated)
	sprintf((char *) &limit_str, (char *) " LIMIT %u, %u ", bottom_limit, top_limit);

	// append the limit string
	if(bottom_limit != top_limit)
		query_builder->BuildString(limit_str);

	// prepare the query
	if(this->mysqlPrepareQuery(query_builder->build_str) != ERR_SUCCESS)
	{
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%% Add Parameters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// dynamically add initial query constraints
	if(db_id)
		this->mysqlAddParameter(MYSQL_TYPE_LONG,   (char *) &db_id,  sizeof(db_id));
	if(user_id)
		this->mysqlAddParameter(MYSQL_TYPE_LONG,   (char *) &user_id,  sizeof(user_id));
	if(group_id)
		this->mysqlAddParameter(MYSQL_TYPE_LONG,   (char *) &group_id,  sizeof(group_id));


	// bind the query parameters before executing statement
	this->mysqlBindParameters();

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%% Add Results %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// allocate new proxy result value
	BP_PCRE_JIT_MATCH_ENTRY result = {0};
	memset((void *)&result, 0x00, sizeof(result));

	// `id`,
	this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &result.id, sizeof(result.id));
	// `user_id`,
	this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &result.user_id, sizeof(result.user_id));
	// `group_id`,
	this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &result.group_id, sizeof(result.group_id));
	// `expression`,
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &result.regexp, sizeof(result.regexp)-1);
	// `description`,
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &result.description, sizeof(result.description)-1);
	// `created_ts`
	this->mysqlAddResult(MYSQL_TYPE_STRING, (char *) &result.created_ts, sizeof(result.created_ts)-1);

	// now bind results
	this->mysqlBindResults();


	// %%%%%%% Execute the SELECT query %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// execute the statement
	if(this->mysqlExecuteStatement() != ERR_SUCCESS)
	{
		printf("\n First execute select statement failed.  Exiting unit test.");
		BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// store the results of the query
	this->mysqlStoreResults();


	// destroy structure if necessary before proceeding
	this->destroyWebJITExpressionData();
	// (this->web_request_jit_match_set, this->tq);


	// reset class proxy sets before continuing
	this->web_request_jit_match_set   = NULL;
	this->web_request_jit_match_n     = 0;


	// fetch an individual record
	while(this->mysqlFetchResult() == ERR_SUCCESS)
	{

		// increment the populated proxy array count
		this->web_request_jit_match_n++;

		// allocate space for the new set
		this->web_request_jit_match_set = (P_BP_PCRE_JIT_MATCH_ENTRY) tq_realloc(this->tq, (void *) this->web_request_jit_match_set, sizeof(BP_PCRE_JIT_MATCH_ENTRY) * (this->web_request_jit_match_n+1), BPLN);

		// column iterator
		size_t col_iter = 0;



		// " `id`,
		memcpy(&this->web_request_jit_match_set[this->web_request_jit_match_n-1].id, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// `user_id`,
		memcpy(&this->web_request_jit_match_set[this->web_request_jit_match_n-1].user_id, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// `group_id`,
		memcpy(&this->web_request_jit_match_set[this->web_request_jit_match_n-1].group_id, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// `expression`,
		memcpy(&this->web_request_jit_match_set[this->web_request_jit_match_n-1].regexp, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// `description`,
		memcpy(&this->web_request_jit_match_set[this->web_request_jit_match_n-1].description, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;

		// `created_ts`
		memcpy(&this->web_request_jit_match_set[this->web_request_jit_match_n-1].created_ts, mysqlSession->bind_results_external_ptr[col_iter].buffer, mysqlSession->bind_results_external_ptr[col_iter].buffer_length);
		col_iter++;


	}

	// destroy statement (free memory)
	this->mysqlDestroyStatement();

	// destroy the query builder
	delete query_builder;

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// displays the current set of jit expressions
BP_ERROR_T Block::displayWebJITExpressions()
{

	//check match iters
	if(!this->web_request_jit_match_n)
		return ERR_FAILURE;
	if(!this->web_request_jit_match_set)
		return ERR_FAILURE;

	// display tests
	size_t n = 0;
	for(; n < this->web_request_jit_match_n; n++)
		BP_DisplayJITRegexpTest(&this->web_request_jit_match_set[n]);

	// return indicating success (records printed)
	return ERR_SUCCESS;

}


// destroy all jit expression data related to this class
BP_ERROR_T Block::destroyWebJITExpressionData()
{

	if(this->jit_delayed_insert_sql)
	{
		tq_free(this->tq, this->jit_delayed_insert_sql);
		this->jit_delayed_insert_sql = NULL;
	}

	if(this->web_request_jit_match_set)
	{
		tq_free(this->tq, this->web_request_jit_match_set);
		this->web_request_jit_match_set = NULL;
	}

	// set the web jit match count to zero
	this->web_request_jit_match_n = 0;

	// return indicating success
	return ERR_SUCCESS;

}

#endif


#ifdef BP_BLOCKCLASS_INCLUDE_GOOGLE_SCRAPING_INTERFACE

/*
 * Blockparty Google Scraping Interface:
 * ------------------------------------------------
 * Allows for the extraction and storage of google search results.  This
 * requires a valid HTTP session to work.
 *
 */

BP_ERROR_T Block::runGoogleSearch(char *query)
{

	// ensure we have a query
	if(!query)
		return ERR_FAILURE;

	// set http session from local class session
	P_BP_HTTP_SESSION http_session = this->httpSession;
	if (!http_session)
		return ERR_FAILURE;

	// open/setup query interface
	if (!this->google_session)
		this->google_session = BP_OpenNewGoogleSearch(query, http_session);

	// set the custom blacklist in the class
	if (this->custom_google_blacklist)
	{

		// set the list pointer in the google scraper structure
		this->google_session->custom_blacklist = this->custom_google_blacklist;

		// loop to get list entry count
		size_t i = 0;
		for (; this->custom_google_blacklist[i]; i++) {}

		// set custom balcklist count
		this->google_session->custom_blacklist_count = i;

	}

	// set the retry flag here
	this->google_session->google_retry_on_too_small_size = this->google_retry_on_too_small_size;

	// set the connect string to 0
	this->google_session->google_retry_on_bad_connect = 0;

	// set reconnect flag from class member
	if(this->google_retry_on_bad_connect)
		this->google_session->google_retry_on_bad_connect = this->google_retry_on_bad_connect;

	// set follow location to 1 (follows 302)
	curl_easy_setopt(this->google_session->http_session->curl, CURLOPT_FOLLOWLOCATION, 1);

	// retrieve query results
	while
	(
		BP_RetrieveGoogleResult(this->google_session, this->secs_between_google_request) != ERR_FAILURE
	)
	{

		// add warning to the stack
		this->AddWarningToStack((char *) "\n [+] Retrieving result");

	}

	if (this->google_session->captcha_activated)
	{
		this->AddErrorToStack((char*) "\n Fatal: Google Search Captcha Activated");
		printf("\n [!!] Fatal: Google Search Captcha Activated");
		exit(0);
	}

	// return indicating success
	return ERR_SUCCESS;
}

// save class this->google_session results to database (requires above table installed on current database)
BP_ERROR_T Block::saveGoogleResults()
{

	if (!this->google_session)
		return ERR_FAILURE;
	// require valid mysql session
	if (!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;


	// select query for testing
	char * select_query = (char *) "SELECT `id` from `results` WHERE `href` = ? LIMIT 0, 1";

	// mysql insert query
	char * insert_query = (char *) "INSERT INTO `results` (  `type` , `sha1`, `href` , `status` , `commit_timestamp`, `audit_start_timestamp`, `audit_completed_timestamp` ) VALUES (  ?, SHA1(?), ?, ?, NOW(), '0000-00-00 00:00:00', '0000-00-00 00:00:00')";

	// loop index
	size_t i = 0;
	for (i; i < this->google_session->url_count; i++) {

		if (!this->google_session->extracted_urls[i])
			continue;

		/*
		 this->mysqlSession->bind = NULL;
		 this->mysqlSession->stmt = NULL;
		 this->mysql_parameters   = NULL;
		 this->mysql_parameter_count = 0;
		 this->mysql_results        = NULL;
		 this->mysql_results_count  = 0;
		 */

		// index is used to store the result from the database
		size_t result_index = 0;

		// prepare the query
		if (this->mysqlPrepareQuery((char *) "SELECT `id` from `results` WHERE `href` = ? LIMIT 0, 1 ") == ERR_FAILURE)
		{
			this->AddErrorToStack((char*) "\n Cannot prepare query");
			BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
		}

		printf("\n [+] inserting_result:  %s", this->google_session->extracted_urls[i]);
		this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) this->google_session->extracted_urls[i], bpstrlen(this->google_session->extracted_urls[i]));

		// bind the parameters
		this->mysqlBindParameters();

		// add a result
		if (this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &result_index, sizeof(result_index)) == ERR_FAILURE)
		{
			this->AddErrorToStack((char*) "\n Cannot add result");
			BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
		}

		// run lookup query on url to make sure it doesn't already exist
		if (this->mysqlBindResults() == ERR_FAILURE)
			this->AddErrorToStack((char*) "\n Cannot bind results");

		// execute the statement
		if (this->mysqlExecuteStatement() == ERR_FAILURE)
		{
			// printf("\n Cannot execute statement.");
		}

		// store results
		this->mysqlStoreResults();

		// retch a result (max one result for this query)
		this->mysqlFetchResult();

		// destroy statement
		this->mysqlDestroyStatement();

		// if the record is already in the database, continue the loop
		if (result_index > 0)
			continue;

		// break loop if the url is null
		if (!this->google_session->extracted_urls[i])
			break;

		// ------------ Insert New Record If There Is No Match ------------------

		// Zero out parameters
		/*
		 this->mysqlSession->bind = NULL;
		 this->mysqlSession->stmt = NULL;
		 this->mysql_parameters = NULL;
		 this->mysql_parameter_count = 0;
		 */

		// target type = 1 : GOOGLE (used for target identification)
		size_t type = BP_DB_TARGET_RESULT_TYPE_GOOGLE;

		// target analysis status (0 for no analysis, this value is changed by the analysis engine)
		size_t status_zero = 0;

		// prepare the insert query
		if (this->mysqlPrepareQuery(insert_query) == ERR_FAILURE)
			this->DisplayErrors();

		// add the type parameter
		if (this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &type, sizeof(type)) == ERR_FAILURE)
			this->DisplayErrors();

		// add the url ( for sha1)
		this->mysqlAddParameter(MYSQL_TYPE_STRING, this->google_session->extracted_urls[i], bpstrlen(this->google_session->extracted_urls[i]));

		// add the url
		this->mysqlAddParameter(MYSQL_TYPE_STRING, this->google_session->extracted_urls[i], bpstrlen(this->google_session->extracted_urls[i]));

		// add the status code
		this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &status_zero, sizeof(status_zero));

		// bind parameters for query
		this->mysqlBindParameters();

		// execute query
		this->mysqlExecuteStatement();

	}

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// destroy google scraper results
BP_ERROR_T Block::destroyGoogleSession()
{

	// destroy the google session (destroys all memory allocated)
	BP_DestroyGoogleSession(this->google_session);

	// set local pointer to null
	this->google_session = NULL;

	// return indicating success
	return ERR_SUCCESS;

}

#endif // BP_BLOCKCLASS_INCLUDE_GOOGLE_SCRAPING_INTERFACE



#ifdef BP_BLOCKCLASS_INCLUDE_DDG_SCRAPING_INTERFACE

/*
 * Blockparty Ddg Scraping Interface:
 * ------------------------------------------------
 * Allows for the extraction and storage of ddg search results.  This
 * requires a valid HTTP session to work.
 *
 */

BP_ERROR_T Block::runDdgSearch(char *query)
{

	// ensure we have a query
	if(!query)
		return ERR_FAILURE;

	// set http session from local class session
	P_BP_HTTP_SESSION http_session = this->httpSession;
	if (!http_session)
		return ERR_FAILURE;

	// open/setup query interface
	if (!this->ddg_session)
		this->ddg_session = BP_OpenNewDdgSearch(query, http_session);

	// set the custom blacklist in the class
	if (this->custom_ddg_blacklist)
	{

		// set the list pointer in the ddg scraper structure
		this->ddg_session->custom_blacklist = this->custom_ddg_blacklist;

		// loop to get list entry count
		size_t i = 0;
		for (; this->custom_ddg_blacklist[i]; i++) {}

		// set custom balcklist count
		this->ddg_session->custom_blacklist_count = i;

	}

	// set the retry flag here
	this->ddg_session->ddg_retry_on_too_small_size = this->ddg_retry_on_too_small_size;

	// set the connect string to 0
	this->ddg_session->ddg_retry_on_bad_connect = 0;

	// set reconnect flag from class member
	if(this->ddg_retry_on_bad_connect)
		this->ddg_session->ddg_retry_on_bad_connect = this->ddg_retry_on_bad_connect;

	// set follow location to 1 (follows 302)
	curl_easy_setopt(this->ddg_session->http_session->curl, CURLOPT_FOLLOWLOCATION, 1);

	// retrieve query results
	while
	(
		BP_RetrieveDdgResult(this->ddg_session, this->secs_between_ddg_request) != ERR_FAILURE
	)
	{

		// add warning to the stack
		this->AddWarningToStack((char *) "\n [+] Retrieving result");

	}

	if (this->ddg_session->captcha_activated)
	{
		this->AddErrorToStack((char*) "\n Fatal: Ddg Search Captcha Activated");
		printf("\n [!!] Fatal: Ddg Search Captcha Activated");
		exit(0);
	}

	// return indicating success
	return ERR_SUCCESS;
}

// save class this->ddg_session results to database (requires above table installed on current database)
BP_ERROR_T Block::saveDdgResults()
{

	if (!this->ddg_session)
		return ERR_FAILURE;
	// require valid mysql session
	if (!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;


	// select query for testing
	char * select_query = (char *) "SELECT `id` from `results` WHERE `href` = ? LIMIT 0, 1";

	// mysql insert query
	char * insert_query = (char *) "INSERT INTO `results` (  `type` , `sha1`, `href` , `status` , `commit_timestamp`, `audit_start_timestamp`, `audit_completed_timestamp` ) VALUES (  ?, SHA1(?), ?, ?, NOW(), '0000-00-00 00:00:00', '0000-00-00 00:00:00')";

	// loop index
	size_t i = 0;
	for (i; i < this->ddg_session->url_count; i++)
	{

		if (!this->ddg_session->extracted_urls[i])
			continue;

		/*
		 this->mysqlSession->bind = NULL;
		 this->mysqlSession->stmt = NULL;
		 this->mysql_parameters   = NULL;
		 this->mysql_parameter_count = 0;
		 this->mysql_results        = NULL;
		 this->mysql_results_count  = 0;
		 */

		// index is used to store the result from the database
		size_t result_index = 0;

		// prepare the query
		if (this->mysqlPrepareQuery((char *) "SELECT `id` from `results` WHERE `href` = ? LIMIT 0, 1 ") == ERR_FAILURE)
		{
			this->AddErrorToStack((char*) "\n Cannot prepare query");
			BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
		}

		printf("\n [+] inserting_result:  %s", this->ddg_session->extracted_urls[i]);
		this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) this->ddg_session->extracted_urls[i], bpstrlen(this->ddg_session->extracted_urls[i]));

		// bind the parameters
		this->mysqlBindParameters();

		// add a result
		if (this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &result_index, sizeof(result_index)) == ERR_FAILURE)
		{
			this->AddErrorToStack((char*) "\n Cannot add result");
			BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
		}

		// run lookup query on url to make sure it doesn't already exist
		if (this->mysqlBindResults() == ERR_FAILURE)
			this->AddErrorToStack((char*) "\n Cannot bind results");

		// execute the statement
		if (this->mysqlExecuteStatement() == ERR_FAILURE)
		{
			// printf("\n Cannot execute statement.");
		}

		// store results
		this->mysqlStoreResults();

		// retch a result (max one result for this query)
		this->mysqlFetchResult();

		// destroy statement
		this->mysqlDestroyStatement();

		// if the record is already in the database, continue the loop
		if (result_index > 0)
			continue;

		// break loop if the url is null
		if (!this->ddg_session->extracted_urls[i])
			break;

		// ------------ Insert New Record If There Is No Match ------------------

		// Zero out parameters
		/*
		 this->mysqlSession->bind = NULL;
		 this->mysqlSession->stmt = NULL;
		 this->mysql_parameters = NULL;
		 this->mysql_parameter_count = 0;
		 */

		// target type = BP_DB_TARGET_RESULT_TYPE_DUCKDUCKGO : DDG (used for target identification)
		size_t type = BP_DB_TARGET_RESULT_TYPE_DUCKDUCKGO;

		// target analysis status (0 for no analysis, this value is changed by the analysis engine)
		size_t status_zero = 0;

		// prepare the insert query
		if (this->mysqlPrepareQuery(insert_query) == ERR_FAILURE)
			this->DisplayErrors();

		// add the type parameter
		if (this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &type, sizeof(type)) == ERR_FAILURE)
			this->DisplayErrors();

		// add the url ( for sha1)
		this->mysqlAddParameter(MYSQL_TYPE_STRING, this->ddg_session->extracted_urls[i], bpstrlen(this->ddg_session->extracted_urls[i]));

		// add the url
		this->mysqlAddParameter(MYSQL_TYPE_STRING, this->ddg_session->extracted_urls[i], bpstrlen(this->ddg_session->extracted_urls[i]));

		// add the status code
		this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &status_zero, sizeof(status_zero));

		// bind parameters for query
		this->mysqlBindParameters();

		// execute query
		this->mysqlExecuteStatement();

	}

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// destroy ddg scraper results
BP_ERROR_T Block::destroyDdgSession()
{

	// destroy the ddg session (destroys all memory allocated)
	BP_DestroyDdgSession(this->ddg_session);

	// set local pointer to null
	this->ddg_session = NULL;

	// return indicating success
	return ERR_SUCCESS;

}

#endif // BP_BLOCKCLASS_INCLUDE_DDG_SCRAPING_INTERFACE


#ifdef BP_BLOCKCLASS_INCLUDE_BING_SCRAPING_INTERFACE

/*
 * Blockparty Bing Scraping Interface:
 * ------------------------------------------------
 * Allows for the extraction and storage of bing search results.  This
 * requires a valid HTTP session to work.
 *
 */

BP_ERROR_T Block::runBingSearch(char *query)
{

	P_BP_HTTP_SESSION http_session = this->httpSession;
	if (!http_session)
		return ERR_FAILURE;

	// open/setup query interface
	if (!this->bing_session)
		this->bing_session = BP_OpenNewBingSearch(query, http_session);

	// set the custom blacklist in the class
	if (this->custom_bing_blacklist)
	{

		// set the list pointer in the bing scraper structure
		this->bing_session->custom_blacklist = this->custom_bing_blacklist;

		// loop to get list entry count
		size_t i = 0;
		for (; this->custom_bing_blacklist[i]; i++) {
		}

		// set custom balcklist count
		this->bing_session->custom_blacklist_count = i;

	}

	// retrieve query results
	while
	(
		BP_RetrieveBingResult(this->bing_session, this->secs_between_bing_request) != ERR_FAILURE
	)
	{
		this->AddWarningToStack((char *) "\n [+] Retrieving result");
	}

	// debug message if necessary
	// printf("\n Gets past retrievebingresult?");

	if (this->bing_session->captcha_activated)
	{
		this->AddErrorToStack((char*) "\n Fatal: Bing Search Captcha Activated");
		printf("\n [!!] Fatal: Bing Search Captcha Activated");
		exit(0);
	}

	// return indicating success
	return ERR_SUCCESS;
}

// save class this->bing_session results to database (requires above table installed on current database)
BP_ERROR_T Block::saveBingResults() {


	// require bing session
	if (!this->bing_session)
		return ERR_FAILURE;

	// require valid mysql session
	if (!this->mysqlSession)
		return ERR_FAILURE;

	// enter critical section
	BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION;

	// select query for testing
	char * select_query = (char *) "SELECT `id` from `results` WHERE `href` = ? LIMIT 0, 1";

	// mysql insert query
	char * insert_query = (char *) "INSERT INTO `results` (  `type` , `sha1`,`href` , `status` , `commit_timestamp`, `audit_start_timestamp`, `audit_completed_timestamp` ) VALUES (  ?, SHA1(?), ?, ?, NOW(), '0000-00-00 00:00:00', '0000-00-00 00:00:00' )";

	// loop index
	size_t i = 0;
	for (i; i < this->bing_session->url_count; i++) {

		if (!this->bing_session->extracted_urls[i])
			continue;

		// index is used to store the result from the database
		size_t result_index = 0;

		// prepare the query
		if (this->mysqlPrepareQuery((char *) "SELECT `id` from `results` WHERE `href` = ? LIMIT 0, 1 ") == ERR_FAILURE)
		{
			this->AddErrorToStack((char*) "\n Cannot prepare query");
			BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
		}

		printf("\n [+] %s", this->bing_session->extracted_urls[i]);
		this->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) this->bing_session->extracted_urls[i], bpstrlen(this->bing_session->extracted_urls[i]));

		// bind the parameters
		this->mysqlBindParameters();

		// add a result
		if (this->mysqlAddResult(MYSQL_TYPE_LONG, (char *) &result_index, sizeof(result_index)) == ERR_FAILURE)
		{
			this->AddErrorToStack((char*) "\n Cannot add result");
			BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
		}

		// run lookup query on url to make sure it doesn't already exist
		if (this->mysqlBindResults() == ERR_FAILURE)
			this->AddErrorToStack((char*) "\n Cannot bind results");

		// execute the statement
		if (this->mysqlExecuteStatement() == ERR_FAILURE)
		{
			// printf("\n Cannot execute statement.");
		}

		// store results
		this->mysqlStoreResults();

		// retch a result (max one result for this query)
		this->mysqlFetchResult();

		// destroy statement
		this->mysqlDestroyStatement();

		// if the record is already in the database, continue the loop
		if (result_index > 0)
			continue;

		// break loop if the url is null
		if (!this->bing_session->extracted_urls[i])
			break;

		// ------------ Insert New Record If There Is No Match ------------------

		// Zero out parameters
		/*
		 this->mysqlSession->bind = NULL;
		 this->mysqlSession->stmt = NULL;
		 this->mysql_parameters = NULL;
		 this->mysql_parameter_count = 0;
		 */

		// target type BP_DB_TARGET_RESULT_TYPE_BING (used for target identification)
		size_t type = BP_DB_TARGET_RESULT_TYPE_BING;

		// target analysis status (0 for no analysis, this value is changed by the analysis engine)
		size_t status_zero = 0;

		// prepare the insert query
		if (this->mysqlPrepareQuery(insert_query) == ERR_FAILURE)
			this->DisplayErrors();

		// add the type parameter
		if (this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &type, sizeof(type)) == ERR_FAILURE)
			this->DisplayErrors();

		// add the url
		this->mysqlAddParameter(MYSQL_TYPE_STRING, this->bing_session->extracted_urls[i], bpstrlen(this->bing_session->extracted_urls[i]));


		// add the url
		this->mysqlAddParameter(MYSQL_TYPE_STRING, this->bing_session->extracted_urls[i], bpstrlen(this->bing_session->extracted_urls[i]));

		// add the status code
		this->mysqlAddParameter(MYSQL_TYPE_LONG, (char *) &status_zero, sizeof(status_zero));

		// bind parameters for query
		this->mysqlBindParameters();

		// execute query
		this->mysqlExecuteStatement();

	}

	// return indicating success
	BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// destroy bing scraper results
BP_ERROR_T Block::destroyBingSession()
{

	// destroy the bing session (destroys all memory allocated)
	BP_DestroyBingSession(this->bing_session);

	// set local pointer to null
	this->bing_session = NULL;

	// return indicating success
	return ERR_SUCCESS;

}

#endif // BP_BLOCKCLASS_INCLUDE_BING_SCRAPING_INTERFACE



#ifdef BP_BLOCKCLASS_INCLUDE_YAHOO_SCRAPING_INTERFACE

/*
 * Blockparty Yahoo Scraping Interface:
 * ------------------------------------------------
 * Allows for the extraction and storage of yahoo search results.  This
 * requires a valid HTTP session to work.
 */

BP_ERROR_T Block::runYahooSearch(char *query)
{

	P_BP_HTTP_SESSION http_session = this->httpSession;
	if (!http_session)
		return ERR_FAILURE;

	// open/setup query interface
	if (!this->yahoo_session)
		this->yahoo_session = BP_OpenNewYahooSearch(query, http_session);

	// set the custom blacklist in the class
	if (this->custom_yahoo_blacklist)
	{

		// set the list pointer in the yahoo scraper structure
		this->yahoo_session->custom_blacklist = this->custom_yahoo_blacklist;

		// loop to get list entry count
		size_t i = 0;
		for (; this->custom_yahoo_blacklist[i]; i++) {
		}

		// set custom balcklist count
		this->yahoo_session->custom_blacklist_count = i;

	}

	// retrieve query results
	while
	(
		BP_RetrieveYahooResult(this->yahoo_session, this->secs_between_yahoo_request) != ERR_FAILURE
	)
	{

		this->AddWarningToStack((char *) "\n [+] Retrieving yahoo result");
		sleep(BP_GenRandomUnsignedLong(10,15));
	}

	if (this->yahoo_session->captcha_activated)
	{
		this->AddErrorToStack((char*) "\n Fatal: Yahoo Search Captcha Activated");
		printf("\n [!!] Fatal: Yahoo Search Captcha Activated");
		exit(0);
	}

	// return indicating success
	return ERR_SUCCESS;
}

// save class this->yahoo_session results to database (requires above table installed on current database)
BP_ERROR_T Block::saveYahooResults()
{

	if (!this->yahoo_session)
		return ERR_FAILURE;

	// require valid mysql session
	if (!this->mysqlSession)
		return ERR_FAILURE;

	// Note:  Don't enter critical section.  Insert routine has it's own locks.

	// loop index
	size_t i = 0;
	for (i; i < this->yahoo_session->url_count; i++)
	{

		// ensure we have a valid extracted parameter
		if (!this->yahoo_session->extracted_urls[i])
			continue;

		// insert new result
		this->insertNewResult(BP_DB_TARGET_RESULT_TYPE_YAHOO, this->yahoo_session->extracted_urls[i], 0, 0);

	}


	// return indicating success
	return ERR_SUCCESS;

}

// destroy yahoo scraper results
BP_ERROR_T Block::destroyYahooSession()
{

	// destroy the yahoo session (destroys all memory allocated)
	BP_DestroyYahooSession(this->yahoo_session);

	// set local pointer to null
	this->yahoo_session = NULL;

	// return indicating success
	return ERR_SUCCESS;

}

#endif // BP_BLOCKCLASS_INCLUDE_YAHOO_SCRAPING_INTERFACE


#ifdef BP_BLOCKCLASS_INCLUDE_DISCONNECTME_SCRAPING_INTERFACE

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Disconnect Me Search Options %%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// attempt to run disconnect me search
BP_ERROR_T Block::runDisconnectMeSearch(char *query, BP_DISCONNECT_ME_SEARCH_TARGET search_engine_target)
{

	// ensure we have a query and it's reasonable
	if(!query)
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(query, bpstrlen(query), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// ensure we have a http session to work with
	if(!this->httpSession)
		return ERR_FAILURE;

	// ensure we have a session
	if(!this->disconnectme_session)
	{

		// attempt to create session JIT
		this->disconnectme_session = BP_DisconnectMeSearchOpen(this->httpSession);
		if(!this->disconnectme_session)
			return ERR_FAILURE;

	}

	// set search term utf8 string from query buffer
	char * search_term_utf8 = BP_UTF8StringEncode(query, bpstrlen(query));

	// run the search
	BP_DisconnectMeSearch(this->disconnectme_session, search_term_utf8, search_engine_target, 60);

	// destroy search term
	bpfree(search_term_utf8);

	// return indicating success
	return ERR_SUCCESS;

}

BP_ERROR_T Block::saveDisconnectMeSearchResults()
{

	// ensure we have a session
	if(!this->disconnectme_session)
		return ERR_FAILURE;

	// require valid mysql session
	if (!this->mysqlSession)
		return ERR_FAILURE;

	// set lookup path
	char * lookup_path[10];
	BP_ZERO_STACK(lookup_path);

	// set results lookup path
	lookup_path[0] = "search_results";
	lookup_path[1] = NULL;

	// attempt to lookup results table
	P_BP_HASH_TABLE_KEYED_ENTRY results_table = BP_HashRegLookupTableByPath(this->disconnectme_session->system_hreg, (char **) &lookup_path);
	if(!results_table)
		return ERR_FAILURE;
	if(!results_table->data)
		return ERR_FAILURE;

	// iterate through results
	BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) results_table->data, result_iter)
	{

		// display record being insert
		printf("\n [+] Inserting Record: %s", result_iter->key);

		// insert new result
		this->insertNewResult(BP_DB_TARGET_RESULT_TYPE_GOOGLE, result_iter->key, 0, 0);

	} BP_HASH_ITER_CLOSE;


	// return indicating success
	return ERR_SUCCESS;

}

// destroy the disconnect me search session
BP_ERROR_T Block::destroyDisconnectMeSearch()
{

	// ensure we have a session
	if(!this->disconnectme_session)
		return ERR_FAILURE;

	// close the sesson
	BP_DisconnectMeSearchClose(this->disconnectme_session);

	// set pointer to null
	this->disconnectme_session = NULL;

	// return indicating success
	return ERR_SUCCESS;

}

#endif

#endif // BP_BLOCKCLASS_RESULT_DB


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%% Begin Garbage Collection Subsystem %%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// display the garbage collector brief data
size_t Block::GcDisplayBrief()
{

	// run garbage collection checks
	if(!this->gc)
	{
		this->AddErrorToStack((char*) "Error. No garbage collector to print.");
		return ERR_FAILURE;
	}

	// display brief data after allocations
	BP_GarbageCollectorDisplayBrief(this->gc);

	// return indicating success
	return ERR_SUCCESS;

}

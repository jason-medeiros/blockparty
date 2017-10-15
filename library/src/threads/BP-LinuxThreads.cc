/***************************************************************************
 *            BP-LinuxThreads.cc
 *
 *  Thu Apr 19 20:02:56 2007
 *  Copyright  2007  User
 *  Email
 ****************************************************************************/
#include "../../include/BP-Main.h"

#ifdef __GNUC__


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Blockparty Managed Thread Implementation %%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Create a new thread manager.
P_BP_THREAD_MANAGER BP_CreateThreadManager(char *name, char *description)
{

	// ensure we have a name and description
	if(!name || !description)
		return NULL;

	P_BP_LINKL manager_tq = BP_CreateLinkL("thread_manager_linkl", BP_TRUE);
	if(!manager_tq)
		return NULL;

	// create manager
	P_BP_THREAD_MANAGER thread_manager = (P_BP_THREAD_MANAGER) tq_calloc(manager_tq, sizeof(BP_THREAD_MANAGER), 1, BPLN);

	// create system registry for enabling critical sections (hash table feature)
	thread_manager->system_hreg = BP_CreateHashTableRegistry();


	// initialize the managment semaphore
	sem_init(&thread_manager->lock, 1, 1);


	// set new manager tailqueue
	thread_manager->tq = manager_tq;

	// dupliate the name
	thread_manager->name = tq_strdup(thread_manager->tq, name, BPLN);

	// duplicate the description
	thread_manager->description = tq_strdup(thread_manager->tq, description, BPLN);

	// after the manager is created, set the status to idl
	thread_manager->status = BP_THREAD_MANAGER_STATUS_IDLE;

	// return the new manager
	return thread_manager;

}

// Destroy the thread manager
BP_ERROR_T BP_DestroyThreadManager(P_BP_THREAD_MANAGER thread_manager)
{

	// cant destroy a thread manager if threads are
	// still running.
	if(thread_manager->threads_running_n)
		return ERR_FAILURE;

	// set the manager tailqueue in a reference, so we can
	// destroy it after destroying the manager itself.
	P_BP_LINKL manager_tq = thread_manager->tq;

	size_t n = 0;
	for(; n < thread_manager->threads_n; n++)
	{

		// destroy individual threads
		if(!thread_manager->threads[n])
			continue;
		if(!thread_manager->threads[n]->tq)
			continue;

		// destroy the thread linkls
		BP_DestroyLinkL(thread_manager->threads[n]->tq, BP_TRUE);

	}


	if(thread_manager->threads)
	{
		tq_free(manager_tq, thread_manager->threads);
	}


	// destroy registry
	if(thread_manager->system_hreg)
		BP_DestroyHashTableRegistry(thread_manager->system_hreg);

	// nullify ptr
	thread_manager->system_hreg = NULL;

	// finally destroy the linkl
	BP_DestroyLinkL(manager_tq, BP_TRUE);



	// return indicating success
	return ERR_SUCCESS;

}

// Display the thread manager
BP_ERROR_T BP_DisplayThreadManager(P_BP_THREAD_MANAGER thread_manager, BP_BOOL display_individual_threads)
{

	if(!thread_manager)
		return ERR_FAILURE;

	// enter critical section
	BP_THREAD_MANAGER_ENTER_CRITICAL_SECTION;
	BP_THREAD_MANAGER_ENTER_CRITICAL_SECTION;

	printf("\n [+] Thread Manager (%p)", thread_manager);
	printf("\n      name:        %s", thread_manager->name);
	printf("\n      description: %s", thread_manager->description);
	printf("\n      tq:          %p", thread_manager->tq);
	printf("\n      lock:        %u", thread_manager->lock);

	switch(thread_manager->status)
	{

		case BP_THREAD_MANAGER_STATUS_UNDEFINED:
			printf("\n      status:      BP_THREAD_MANAGER_STATUS_UNDEFINED ");
			break;
		case BP_THREAD_MANAGER_STATUS_MODIFYING_THREADS:
			printf("\n      status:      BP_THREAD_MANAGER_STATUS_MODIFYING_THREADS ");
			break;
		case BP_THREAD_MANAGER_STATUS_MODIFYING_SELF:
			printf("\n      status:      BP_THREAD_MANAGER_STATUS_MODIFYING_SELF ");
			break;
		case BP_THREAD_MANAGER_STATUS_IDLE:
			printf("\n      status:      BP_THREAD_MANAGER_STATUS_IDLE ");
			break;
		case BP_THREAD_MANAGER_STATUS_DESTROYING_SELF:
			printf("\n      status:      BP_THREAD_MANAGER_STATUS_DESTROYING_SELF ");
			break;
		default:
			printf("\n      status:      UNKNOWN_BAD_VALUE ");
			break;

	}

	// display thread counters here
	printf("\n");
	printf("\n      [-] Thread Counters:");

	// display the total dispatch count
	printf("\n           threads_dispatched_total_n:            %u", thread_manager->threads_dispatched_total_n);
	printf("\n           threads_dispatched_total_n (overflow): %u", thread_manager->threads_dispatched_total_n_overflow_count);
	printf("\n           threads_reclaimed_total_n:             %u", thread_manager->threads_reclaimed_total_n);
	printf("\n           threads_reclaimed_total_n: (overflow)  %u", thread_manager->threads_reclaimed_total_n_overflow_count);
	printf("\n           threads_running_n:                     %u", thread_manager->threads_running_n);

	// general purpose iterator for iterating threads
	size_t n = 0;

	// if the dev has selected to display individual threads, do so here
	if(display_individual_threads == BP_TRUE)
	if(thread_manager->threads_n)
	{

		printf("\n");
		printf("\n\t [-] Threads (%p) ", thread_manager->threads);
		printf("\n\t     threads_n:   %u", thread_manager->threads_n);
		printf("\n");

		// walk threads and display
		for(n = 0; n < thread_manager->threads_n; n++)
		{
			if(thread_manager->threads[n])
			{

				// display the managed thread
				BP_DisplayManagedThread(thread_manager->threads[n]);

			}

		}

	}

	// display trailing newline
	printf("\n");

	// leave critical section
	BP_THREAD_MANAGER_LEAVE_CRITICAL_SECTION


	// return indicating success
	return ERR_SUCCESS;

}

// Display the managed thread.
BP_ERROR_T BP_DisplayManagedThread(P_BP_MANAGED_THREAD managed_thread)
{

	// ensure we have a managed thread
	if(!managed_thread)
		return ERR_FAILURE;

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(managed_thread->thread_manager->system_hreg);



	printf("\n\t      [+] Thread: %p - %s", managed_thread, managed_thread->name);

	// set status
	printf("\n\t       description:           %s", managed_thread->description);

	switch(managed_thread->current_status)
	{
		case BP_THREAD_STATUS_UNDEFINED:
			printf("\n\t       status:                BP_THREAD_STATUS_UNDEFINED");
			break;

		case BP_THREAD_STATUS_UNSTARTED:
			printf("\n\t       status:                BP_THREAD_STATUS_UNSTARTED");
			break;

		case BP_THREAD_STATUS_RUNNING:
			printf("\n\t       status:                BP_THREAD_STATUS_RUNNING");
			break;

		case BP_THREAD_STATUS_LOCKED_FOR_READ:
			printf("\n\t       status:                BP_THREAD_STATUS_LOCKED_FOR_READ");
			break;

		case BP_THREAD_STATUS_LOCKED_FOR_REDISPATCH:
			printf("\n\t       status:                BP_THREAD_STATUS_LOCKED_FOR_REDISPATCH");
			break;


		case BP_THREAD_STATUS_EXITED:
			printf("\n\t       status:                BP_THREAD_STATUS_EXITED");
			break;

		case BP_THREAD_STATUS_BEING_MODIFIED_CURRENTLY_DATA_IS_UNRELIABLE:
			printf("\n\t       status:                BP_THREAD_STATUS_BEING_MODIFIED_CURRENTLY_DATA_IS_UNRELIABLE");
			break;

		case BP_THREAD_STATUS_REJOINED:
			printf("\n\t       status:                BP_THREAD_STATUS_REJOINED");
			break;

		default:
			printf("\n\t       status:                BAD_VALUE");
			break;

	}

	printf("\n\t       thread_id:             %zu", managed_thread->thread_id);
	printf("\n\t       lock:                  %zu", managed_thread->lock);
	printf("\n\t       thread_manager:        %p",  managed_thread->thread_manager);
	printf("\n\t       start_routine:         %p",  managed_thread->start_routine);
	printf("\n\t       thread_parameter:      %p", managed_thread->thread_parameter);
	printf("\n\t       thread_parameter_size: %zu", managed_thread->thread_parameter_size);


	// display trailing newline
	printf("\n");

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(managed_thread->thread_manager->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}


// Create a new thread entry in the current thread manager, initialize it's
// members, and start it.  If no name or description is passed in (aka null),
// they will be randomly generated as random UUID values.
BP_ERROR_T BP_CreateThread
(
		P_BP_THREAD_MANAGER thread_manager,
		void *   (start_routine)(void*),
		void *   arg,
		size_t   arg_len,
		BP_BOOL  start_thread_upon_creation,
		char *   name,
		char *   description
)
{

	// ensure we have a new thread manager
	if(!thread_manager)
		return ERR_FAILURE;

	// enter critical section
	BP_THREAD_MANAGER_ENTER_CRITICAL_SECTION;

	// attempt to allocate space for the managed thread
	P_BP_MANAGED_THREAD managed_thread = (P_BP_MANAGED_THREAD) tq_calloc(thread_manager->tq, sizeof(BP_MANAGED_THREAD), 1, BPLN);
	if(!managed_thread)
	{
		BP_THREAD_MANAGER_LEAVE_CRITICAL_SECTION;
		return ERR_FAILURE;
	}

	// initialize the threads lock (makes it usable as a global semaphore)
	sem_init(&managed_thread->lock, 1, 1);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Fill In Thread Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// name and descriptions
	char *name_final = NULL;
	char *desc_final = NULL;

	// Names and descriptions can be null.  If they are null, they are
	// set as randomly generated uuids.
	char name_buff[60];
	BP_ZERO_STACK(name_buff);
	char desc_buff[60];
	BP_ZERO_STACK(desc_buff);

	// set name, or random uuid if none was provided
	if(!name)
	{
		BP_GenRandomUUID_StoreInBuff(name_buff);
		name_final = name_buff;
	}
	else
		name_final = name;

	// set description, or random uuid if none was provided
	if(!description)
	{
		BP_GenRandomUUID_StoreInBuff(desc_buff);
		desc_final = desc_buff;
	}
	else
		desc_final = description;

	// create a new thread safe tail queue allocator
	managed_thread->tq = BP_CreateLinkL("managed_thread_allocator", BP_TRUE);

	// set the current thread status to unstarted
	managed_thread->current_status = BP_THREAD_STATUS_UNSTARTED;

	// set start routine
	managed_thread->start_routine = start_routine;

	// set parameter
	managed_thread->thread_parameter      = arg;
	managed_thread->thread_parameter_size = arg_len;

	// set description
	managed_thread->name        = tq_strdup(managed_thread->tq, name_final, BPLN);
	managed_thread->description = tq_strdup(managed_thread->tq, desc_final, BPLN);

	// set thread manager
	managed_thread->thread_manager = thread_manager;

	// set reference to system hash registry (for critical sections)
	managed_thread->system_hreg_ptr = thread_manager->system_hreg;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Store Managed Thread in Manager %%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// allocate new space for threads
	thread_manager->threads = (P_BP_MANAGED_THREAD *) tq_realloc(thread_manager->tq, thread_manager->threads, sizeof(P_BP_MANAGED_THREAD) * (thread_manager->threads_n+2), BPLN);

	// set the managed thread in place
	thread_manager->threads[thread_manager->threads_n]   = managed_thread;
	thread_manager->threads[thread_manager->threads_n+1] = NULL;

	// increment the thread count
	thread_manager->threads_n++;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Start Thread if Desired %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(start_thread_upon_creation == BP_TRUE)
	{
		BP_StartThread(thread_manager, managed_thread);
	}


	// leave critical section
	BP_THREAD_MANAGER_LEAVE_CRITICAL_SECTION



	// return indicating success
	return ERR_SUCCESS;

}

// Start threads which aren't already currently running.
BP_ERROR_T BP_StartAllUnstartedThreads(P_BP_THREAD_MANAGER thread_manager)
{

	// ensure we have a thread manager
	if(!thread_manager)
		return ERR_FAILURE;


	// managed thread
	P_BP_MANAGED_THREAD managed_thread = NULL;

	size_t n = 0;
	for(; n < thread_manager->threads_n; n++)
	{

		// set the managed thread
		managed_thread = thread_manager->threads[n];

		if(managed_thread->current_status == BP_THREAD_STATUS_UNSTARTED)
		{

			BP_StartThread(thread_manager, managed_thread);

		}

	}


	// return indicating success
	return ERR_SUCCESS;

}

// Start a thread (thread must exist within the thread manager for it to start, cannot be some random
// thread you created)
BP_ERROR_T BP_StartThread(P_BP_THREAD_MANAGER thread_manager, P_BP_MANAGED_THREAD managed_thread)
{

	// ensure we have a thread manager
	if(!thread_manager)
		return ERR_FAILURE;

	// ensure we have a thread to start
	if(!managed_thread)
		return ERR_FAILURE;



	// create thread (pass thread list as a pointer)
	if(pthread_create(&managed_thread->thread_id, NULL, managed_thread->start_routine, managed_thread) != 0)
	{

		return ERR_FAILURE;
	}



	// enter critical section
	BP_THREAD_MANAGER_ENTER_CRITICAL_SECTION;

	// set the status as running
	managed_thread->current_status = BP_THREAD_STATUS_RUNNING;

	// increment the number of running threads
	thread_manager->threads_running_n++;

	// set overflow (in the case that we've encountered an overflow)
	if((thread_manager->threads_dispatched_total_n+1) == 0)
		thread_manager->threads_dispatched_total_n_overflow_count++;

	// increment the total dispatch count
	thread_manager->threads_dispatched_total_n++;

	// leave critical section after all modifications
	BP_THREAD_MANAGER_LEAVE_CRITICAL_SECTION

	// return indicating success
	return ERR_SUCCESS;

}

// Looks for a thread candidate (either exited, or not started)
P_BP_MANAGED_THREAD BP_FindThreadDispatchCandidate
(
		P_BP_THREAD_MANAGER thread_manager,
		BP_BOOL lock_for_redispatch
)
{

	// ensure we have a thread manager
	if(!thread_manager)
		return NULL;

	// enter critical section
	BP_THREAD_MANAGER_ENTER_CRITICAL_SECTION;

	// thread candidate for redispatch
	P_BP_MANAGED_THREAD thread_candidate = NULL;

	// managed thread (for use with macros)
	P_BP_MANAGED_THREAD managed_thread = NULL;

	size_t n = 0;
	for(; n < thread_manager->threads_n; n++)
	{

		// set managed thread
		managed_thread = thread_manager->threads[n];

		// check current status to see if it falls within the specified
		// values.
		if
		(
				managed_thread->current_status == BP_THREAD_STATUS_UNSTARTED ||
				managed_thread->current_status == BP_THREAD_STATUS_EXITED
		)
		{

			// set the thread candidate
			thread_candidate = managed_thread;

			// lock for redispatch
			if(thread_candidate->current_status != BP_THREAD_STATUS_UNSTARTED)
			if(lock_for_redispatch)
				thread_candidate->current_status = BP_THREAD_STATUS_LOCKED_FOR_REDISPATCH;

		}

		if(thread_candidate)
			break;

	}



	// enter critical section
	BP_THREAD_MANAGER_LEAVE_CRITICAL_SECTION

	// return the thread candidate (or null if it's unset)
	return thread_candidate;

}

// Attempts to find an exited thread, and redispatch it.
BP_ERROR_T BP_RedispatchThread
(
	P_BP_THREAD_MANAGER thread_manager,
	void *   arg,
	size_t   arg_len,
	size_t   arg_idx
)
{

	// ensure we have a thread manager
	if(!thread_manager)
		return ERR_FAILURE;

	// managed thread to lookup
	P_BP_MANAGED_THREAD managed_thread = NULL;

	do
	{

		BP_THREAD_MANAGER_ENTER_CRITICAL_SECTION;

		// attempt to find a thread dispatch candidate
		managed_thread = BP_FindThreadDispatchCandidate(thread_manager);

		// if(managed_thread)
		// printf("\n Managed Thread: %p", managed_thread);

		BP_THREAD_MANAGER_LEAVE_CRITICAL_SECTION;

		// sleep for a bit so we don't just spin like crazy
		usleep(100);
	}
	while(!managed_thread);


	// enter critical section
	BP_THREAD_MANAGER_ENTER_CRITICAL_SECTION;

	// check status and attempt to rejoin the thread
	if
	(
			managed_thread->current_status == BP_THREAD_STATUS_LOCKED_FOR_REDISPATCH ||
			managed_thread->current_status == BP_THREAD_STATUS_EXITED
	)
	{


		// try to rejoin the candidate before dispatching.  Lock the manager, but
		// don't lock the individual thread (as it's already locked via this function)
		BP_RejoinThread(thread_manager, managed_thread, BP_TRUE, BP_TRUE);


	}

	// leave critical section
	BP_THREAD_MANAGER_LEAVE_CRITICAL_SECTION

	// enter critical section
	BP_THREAD_MANAGER_ENTER_CRITICAL_SECTION;


	// set thread parameter and parameter size
	managed_thread->thread_parameter      = arg;
	managed_thread->thread_parameter_size = arg_len;
	managed_thread->thread_parameter_idx  = arg_idx;

	// attempt to start the thread
	BP_StartThread(thread_manager, managed_thread);


	// leave critical section
	BP_THREAD_MANAGER_LEAVE_CRITICAL_SECTION;




	// return indicating success
	return ERR_SUCCESS;

}

// This routine will submit the provided arguments
// to the threads currently in the thread manager, processing
// data in each thread sequentially.  Ideally, the processed data
// should be an opaque type capable of storing the result of the
// processing within itself (struct->ret_value).
BP_ERROR_T BP_ThreadedDataProcessor
(
	P_BP_THREAD_MANAGER thread_manager,
	void **   args,     /* argument stack */
	size_t *  arg_lens, /* array of argument lengths */
	size_t    arg_n     /* number of arguments total */
)
{

	// ensure we have a threaded manager
	if(!thread_manager)
		return ERR_FAILURE;

	// redispatch all items in the argument stack
	size_t n = 0;
	for
	(
		n = 0;
		n < arg_n;
		n++
	)
	{

		// DONT WRAP IN CRITICAL SECTION

		// attempt to redispatch the threads
		BP_RedispatchThread
		(
			thread_manager,
			args[n],
			arg_lens[n],
			n
		);


	}

	// DON"T WRAP IN CRITICAL SECTION: attempt to rejoin all the threads
	BP_RejoinAllThreads(thread_manager);

	// return indicating success
	return ERR_SUCCESS;

}


// Attempt to rejoin a single thread
BP_ERROR_T BP_RejoinThread
(
	P_BP_THREAD_MANAGER thread_manager,
	P_BP_MANAGED_THREAD managed_thread,
	BP_BOOL lock_thread_manager,
	BP_BOOL lock_managed_thread
)
{

	// ensure we have a thread manager
	if(!thread_manager)
		return ERR_FAILURE;

	// ensure we have a managed thread
	if(!managed_thread)
		return ERR_FAILURE;



	// attempt to join the threads
	switch(pthread_join(managed_thread->thread_id, NULL))
	{

		case EINVAL:
			printf("\n pthread_join: EINVAL");
			break;

		case ESRCH:

			// enter critical section
			BP_THREAD_MANAGER_ENTER_CRITICAL_SECTION;

			// if we got a thread id, it possibly can exit before we get a chance to
			// examine it.  This forces the removal of a thread that has a valid id
			// but cannot be found (this fixes a sinister bug)
			if(managed_thread->thread_id)
			{


				// decrement the number of threads running
				thread_manager->threads_running_n--;

				// increment the overflow count
				if( (thread_manager->threads_reclaimed_total_n+1) == 0)
					thread_manager->threads_reclaimed_total_n_overflow_count++;

				// increment the number of reclaimed threads
				thread_manager->threads_reclaimed_total_n++;


				// set the thread as exited
				managed_thread->current_status = BP_THREAD_STATUS_REJOINED;

				// set thread id
				managed_thread->thread_id = 0;
			}

			// leave critical section
			BP_THREAD_MANAGER_LEAVE_CRITICAL_SECTION

			break;
		case EDEADLK:
			printf("\n pthread_join: EDEADLK");
			break;

		default:

			// enter critical section
			BP_THREAD_MANAGER_ENTER_CRITICAL_SECTION;

			// decrement the number of threads running
			thread_manager->threads_running_n--;

			// increment the overflow count
			if( (thread_manager->threads_reclaimed_total_n+1) == 0)
				thread_manager->threads_reclaimed_total_n_overflow_count++;

			// increment the number of reclaimed threads
			thread_manager->threads_reclaimed_total_n++;


			// set the thread as exited
			managed_thread->current_status = BP_THREAD_STATUS_REJOINED;

			// set thread id
			managed_thread->thread_id = 0;

			// leave critical section
			BP_THREAD_MANAGER_LEAVE_CRITICAL_SECTION

			// break
			break;

	}



	// return indicating success
	return ERR_SUCCESS;

}

// Attempt to rejoin all the threads
BP_ERROR_T BP_RejoinAllThreads(P_BP_THREAD_MANAGER thread_manager, BP_BOOL only_try_to_reclaim_dont_force)
{

	// ensure we have a thread manager
	if(!thread_manager)
		return ERR_FAILURE;

	// ensure we have threads to check
	if(!thread_manager->threads || !thread_manager->threads_n)
		return ERR_FAILURE;

	// ensure that we have some running threads
	if(!thread_manager->threads_running_n)
		return ERR_FAILURE;

	// declare pointer for managed thread
	P_BP_MANAGED_THREAD managed_thread = NULL;


	// NOTE: DO NOT PUT ENTIRE LOOP IN A CRITICAL SECTION.  IT WILL
	//       CAUSE A DEADLOCK.
	size_t n = 0;
	do
	{



		for(n=0; n < thread_manager->threads_n; n++)
		{


			// enter critical section
			BP_THREAD_MANAGER_ENTER_CRITICAL_SECTION;

			// set managed thread
			managed_thread = thread_manager->threads[n];

			// leave critical section
			BP_THREAD_MANAGER_LEAVE_CRITICAL_SECTION


			// only try to rejoin unjoined threads
			if(managed_thread->current_status == BP_THREAD_STATUS_REJOINED)
			{
				continue;
			}


			// attempt to rejoin the thread (lock manager and thread during rejoin)
			BP_RejoinThread(thread_manager, managed_thread, BP_TRUE, BP_FALSE);

		}


	} while(thread_manager->threads_running_n && only_try_to_reclaim_dont_force);

	// thread couldn't join
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Old/Bad Thread Mechanisms %%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// creates a thread and returns it in a list structure
// arg is the data pointer being passed to the thread, argLen is
// recorded locally in the P_THREAD_LIST returned to be
// referencable.
//
//
// IMPORTANT IMPORTANT READ THIS STUPID, THAT MEANS YOU JASON
//
// The destination "void * function(void *arg)" has an passed argument type of P_THREAD_LIST not
// the argument type that you submitted.  You have to cast the argument (upon reciveing it
// in the void function) to P_THREAD_LIST or your code will be all fudged up.
//
P_THREAD_LIST CreateThread(void *(startRoutine)(void*), void *arg, int argLen, char * description, P_BP_THREAD_MGMT thread_mgmt_data)
{
	
	// ensure we have a start routine
	if(startRoutine == NULL)
	{
		return NULL;
	}
	
	// create a new thread list element
	P_THREAD_LIST threadList = (P_THREAD_LIST) bpcalloc(sizeof(THREAD_LIST), 1);
		
	// set list pointers
	threadList->first = threadList;
	threadList->prev = NULL;
	threadList->next = NULL;
	
	// save start routine pointer for identification
	threadList->startRoutine = startRoutine;
	
	// save pointer to thread argument data along with size
	threadList->sharedData     = arg;
	threadList->sharedDataSize = argLen;
	
	// initialize thread lock
	threadList->thread_lock = PTHREAD_MUTEX_INITIALIZER;

	// set thread managment data
	threadList->thread_mgmt_data = thread_mgmt_data;

	// initialize set thread id
	threadList->threadId = 0;

	// copy in description string
	if(description != NULL)
	{

		// allocate space for the description
		threadList->threadDescription = (char *) bpcalloc(strnlen(description, 1024)+10, 1);

		// copy in the description
		memcpy(threadList->threadDescription, description, strnlen(description, 1024));

	}

	// create thread (pass thread list as a pointer)
	if(pthread_create(&threadList->threadId, NULL, startRoutine, threadList) != 0)
	{
			
		// bpfree bpmalloc'ed member pointers (cleanup)
		if(threadList->threadDescription != NULL)
			bpfree(threadList->threadDescription);
		
		// bpfree whole structure (cleanup)
		bpfree(threadList);

		// exit routine
		return NULL;

	}
	
	// Return with the newly populated thread list.  Lock the mutex inside
	// this thread list element before trying to access members within as
	// it may cause a race condition otherwise.
	return threadList;

}

// DESIGNED TO BE CALLED FROM WITHIN THREAD: This function is used to set a status
// code to a thread list element safely within a threaded environment.  Call this before
// a thread has exited to notify the blockparty (or your own custom) thread manager of
// a thread exiting.
BP_ERROR_T SetThreadStatusCode(P_THREAD_LIST thread_elem, BP_THREAD_STATUS status_code)
{

	// ensure that the thread element is non-null
	if(!thread_elem)
		return ERR_FAILURE;

	// try to lock the mutex
	while(LockMutex((pthread_mutex_t *) &thread_elem->thread_lock) != ERR_SUCCESS){};

	// data is now locked, set status appropriately.
	thread_elem->threadState = status_code;

	// unlock the mutex
	while(UnlockMutex((pthread_mutex_t *) &thread_elem->thread_lock) != ERR_SUCCESS){};

	// return indicating success
	return ERR_SUCCESS;

}

// DESIGNED TO BE CALLED FROM THREAD MANAGER: This function can be used in a manager/dispatcher
// to safely check the status of a thread.  The threads are responsible for setting their own statuses
// upon exit/termination.
BP_THREAD_STATUS CheckThreadStatusCode(P_THREAD_LIST thread_elem)
{

	// ensure that the thread element is non-null
	if(!thread_elem)
		return BP_THREAD_STATUS_UNDEFINED;

	// try to lock the mutex
	while(LockMutex((pthread_mutex_t *) &thread_elem->thread_lock) != ERR_SUCCESS){};

	// set status from thread state
	BP_THREAD_STATUS status = thread_elem->threadState;

	// unlock the mutex
	while(UnlockMutex((pthread_mutex_t *) &thread_elem->thread_lock) != ERR_SUCCESS){};

	// return the retrieved status code
	return status;

}

// Attempts to join all threads.  Needs to be called after dispatching any
// number of threads.  It prevents contention.
BP_ERROR_T RejoinAllThreads(P_THREAD_LIST thread_list)
{

	// ensure we have a thread list to work with
	if(!thread_list)
		return ERR_FAILURE;

	for
	(
		thread_list = thread_list->first;
		thread_list;
		thread_list = thread_list->next
	)
	{

		// printf("\n Attempting to join thread id: %u", thread_list->threadId);
		if(thread_list->threadId)
		{

			switch(pthread_join(thread_list->threadId, NULL))
			{

				case EINVAL:
					printf("\n pthread_join: EINVAL");
					break;

				case ESRCH:
					printf("\n pthread_join: ESRCH");
					break;

				case EDEADLK:
					printf("\n pthread_join: EDEADLK");
					break;

				default:
					// printf("\n thread %u joined OK!", thread_list->threadId);
					// pthread_detach(thread_redispatch->threadId);
					break;

			}

		}

	}

	// thread couldn't join
	return ERR_SUCCESS;

}

// redispatches a thread given a thread structure
BP_ERROR_T RedispatchThread(P_THREAD_LIST thread_redispatch)
{

	// ensure we have a thread structure
	if(thread_redispatch == NULL)
	{
		return ERR_FAILURE;
	}

	// you can only redispatch exited threads
	if(thread_redispatch->threadState != BP_THREAD_STATUS_EXITED)
		return ERR_FAILURE;

	// join the thread to bpfree resources
	if(thread_redispatch->threadId)
	switch(pthread_join(thread_redispatch->threadId, NULL))
	{
		case EINVAL:
			printf("\n pthread_join: EINVAL");
			break;
		case ESRCH:
			printf("\n pthread_join: ESRCH");
			break;
		case EDEADLK:
			printf("\n pthread_join: EDEADLK");
			break;
		default:
			// printf("\n thread %u joined OK!", thread_redispatch->threadId);
			// pthread_detach(thread_redispatch->threadId);
			break;
	}

	// create thread
	switch
	(
		pthread_create
		(
				&thread_redispatch->threadId,
				NULL,
				thread_redispatch->startRoutine,
				thread_redispatch
		)
	)
	{
		case EAGAIN:

			printf("\n pthread_create EAGAIN");
			return ERR_FAILURE;
		case EINVAL:
			printf("\n pthread_create: EINVAL");
			return ERR_FAILURE;
		case EPERM:
			printf("\n pthread_create: EPERM");
			return ERR_FAILURE;
		default:
			//
			break;
	}

	// printf("\n [%u] thread created OK!", thread_redispatch->threadId);


	// return indicating success (thread has launched!)
	return ERR_SUCCESS;

}



// Appends the thread list element "appendList" to the thread
// bearer element inList, attaching at the last element.  This
// function modifies the list values in appendList.  Copy them
// before calling this function if you intend to keep original values.
int AppendThreadToList(P_THREAD_LIST inList, P_THREAD_LIST appendList)
{


	if(inList == NULL)
		return ERR_FAILURE;
	
	if(appendList == NULL)
		return ERR_FAILURE;
	
	P_THREAD_LIST appendFirst = appendList->first;
	
	// get to the end of the list
	for(inList = inList->first; inList->next != NULL; inList = inList->next){}
	
	// now set all the append list first values to reflect inLists first pointer
	for(appendList = appendList->first; \
		appendList->next != NULL;\
		appendList = appendList->next){
			
		appendList->first = inList->first;	
	}
	
	if(appendList->next == NULL){
		if(appendList->first != inList->first)
			appendList->first = inList->first;
	}
	
	// now after all the pointers have been set, link up the chain
	inList->next = appendFirst;
	appendFirst->prev = inList;
	
	return ERR_SUCCESS;
}

// Cancels a thread, effectively stopping thread execution
int CancelThread(P_THREAD_LIST threadHandle)
{

	if(threadHandle == NULL)
		return ERR_FAILURE;
	
	threadHandle->threadState = BP_THREAD_STATUS_EXITED;
	
	if( pthread_cancel(threadHandle->threadId) != 0){
		return ERR_FAILURE;
	}
	
	return ERR_SUCCESS;
}

// Cancels all threads in a list, terminating them all
int CancelAllThreads(P_THREAD_LIST threadHandle)
{

	if(threadHandle == NULL)
		return ERR_FAILURE;
	
	// ignores failure/success codes
	for(threadHandle = threadHandle->first;
		threadHandle != NULL;
		threadHandle = threadHandle->next){
				CancelThread(threadHandle);
	}
	
	return ERR_SUCCESS;
}

// Creates a new mutex (fast mutex, not recursive)
pthread_mutex_t * CreateNewMutex()
{

	// allocate space for new mutex
	pthread_mutex_t * newMutex = (pthread_mutex_t *) bpmalloc(sizeof(pthread_mutex_t));
	
	// initialize mutex
	if(pthread_mutex_init(newMutex, NULL) != 0){
		bpfree(newMutex);
		return NULL;
	}
	
	// return the new mutex
	return newMutex;	

}

// Try Locks a fast mutex
BP_ERROR_T TryLockMutex(pthread_mutex_t * mutex)
{

	// check to ensure the mutex exists
	if(!mutex)
		return ERR_FAILURE;

	// attempt to try lock (zero is success, non-zero is failure)
	if(pthread_mutex_trylock(mutex))
		return ERR_FAILURE;

	// default return condition is success
	return ERR_SUCCESS;

}

// Locks a fast mutex
int LockMutex(pthread_mutex_t * mutex)
{

	// ensure the pointer isn't null
	if(mutex == NULL)
		return ERR_FAILURE;
	
	// attempt to lock the mutex
	switch(pthread_mutex_lock(mutex))
	{

		// check for invalid mutex
		case EINVAL:
			printf("\n[!!] The mutex which is being locked is invalid.  Perhaps initialize it with PTHREAD_MUTEX_INITIALIZER first.");
			return ERR_FAILURE;

		// check for max recursion mutex
		case EAGAIN:
			printf("\n[!!] The maximum number of recursive locks on this mutex has been exceeded.");
			return ERR_FAILURE;

		// mutex locked A-OK
		case 0:
			return ERR_SUCCESS;

		default:
			printf("\n[!!] This mutex cannot be locked due an error that is not EINVAL or EAGAIN.  SNAFU.");
			return ERR_FAILURE;

	}

	
	// return indicating success
	return ERR_FAILURE;

}

// Unlocks a fast mutex
int UnlockMutex(pthread_mutex_t * mutex)
{

	// ensure we have a mutex
	if(!mutex)
		return ERR_FAILURE;

	if(pthread_mutex_unlock(mutex) != 0)
		return ERR_FAILURE;
	
	return ERR_SUCCESS;
}

// Destroys a fast mutex
int DestroyMutex(pthread_mutex_t * mutex, BP_BOOL free_memory_from_heap)
{

	// ensure we have a mutex to work with
	if(!mutex)
		return ERR_FAILURE;
	
	// destroy the mutex
	pthread_mutex_destroy(mutex);

	// free the mutex
	if(free_memory_from_heap == BP_TRUE)
		bpfree(mutex);
	
	// return indicating success
	return ERR_SUCCESS;

}

// prints all the elements in the thread list
void PrintThreadList(P_THREAD_LIST inList)
{

	if(inList == NULL)
		return;
	
	for(inList = inList->first; inList != NULL; inList = inList->next){
		PrintThreadElement(inList);
	}
	
}

// prints one element from a thread chain
void PrintThreadElement(P_THREAD_LIST inList){
	
	if(inList == NULL)
		return;
	
	// print out data elements in readable form
	printf("\n [+] Thread List: %p - (%s) ", inList, inList->threadDescription);
	printf("\n\t list->first: %p", inList->first);
	printf("\n\t list->prev : %p", inList->prev);
	printf("\n\t list->next : %p", inList->next);
	
	printf("\n\t\t Start Function Pointer: %p", (void *) inList->startRoutine);
	printf("\n\t\t Shared Data Pointer   : %p", inList->sharedData);
	printf("\n\t\t Shared Data Size      : %i", inList->sharedDataSize);
	printf("\n\t\t Thread ID             : %u", inList->threadId);
	
}

// destroys one thread list element.  Automatically
// patches list if prev or next exist.  Does not destroy allocated
// shared data.
int FreeThreadElement(P_THREAD_LIST inList){
	
	if(inList == NULL)
		return ERR_FAILURE;
	
	// initialize prev/next ptrs for list manipulation
	P_THREAD_LIST prev = NULL;
	P_THREAD_LIST next = NULL;
	
	// check prev/next pointers
	if(inList->prev != NULL)
		prev = inList->prev;
	if(inList->next != NULL)
		next = inList->next;
	
	// relink the chain before bpfreeing
	if(prev != NULL)
		prev->next = next;
	
	// bpfree objects
	//if(inList->sharedData != NULL)
	//	bpfree(inList->sharedData);
	if(inList->threadDescription != NULL)
		bpfree(inList->threadDescription); 

	memset(inList, 0x00, sizeof(THREAD_LIST));
	bpfree(inList);

	return ERR_SUCCESS;
}

// destroys all the threads in a list
int FreeThreadList(P_THREAD_LIST inList)
{

	if(inList == NULL)
		return ERR_FAILURE;
	P_THREAD_LIST next = NULL;
	for(inList = inList->first; inList != NULL; inList = next){

		if(!inList)
			break;

		next = inList->next;

		// FreeThreadElement(inList);
		if(inList->threadDescription)
			bpfree(inList->threadDescription);

		bpfree(inList);

	}
	
	return ERR_SUCCESS;
}

// returns ERR_SUCCESS if the thread is running, ERR_FAILURE if the thread has exited.  Also marks
// the thread list element to reflect a RUNNING/EXITED message via the structures state member.
BP_ERROR_T CheckIfThreadIsRunning(P_THREAD_LIST thread_elem)
{

	// don't try to check up on ourselves
	if(!thread_elem)
	{
		printf("\n [!!] Cannot monitor NULL thread element.  Cannot determine run status of non-existant thread.");
		return ERR_FAILURE;
	}

	// cannot check a zero thread
	if(thread_elem->threadId == 0)
		return ERR_FAILURE;

	// Check if the thread cannot be found, if so -- it has exited.
	if(pthread_kill(thread_elem->threadId, 0) == ESRCH)
	{
		thread_elem->threadState = BP_THREAD_STATUS_EXITED;
		thread_elem->threadId = 0;
		return ERR_FAILURE;
	}

	// set running flag
	thread_elem->threadState = BP_THREAD_STATUS_RUNNING;

	// return success to indicate that the thread is still running
	return ERR_SUCCESS;

}

// this version of the above function checks/marks the status of all of the threads.  If ANY thread
// is running, this function returns ERR_SUCCESS.  This function returns ERR_FAILURE if some threads
// are still running.  Check the lists->threadState variable to see which threads are exited etc.  This
// function does modify the elements status variable.
BP_ERROR_T CheckAllThreadsAndUpdateStatus(P_THREAD_LIST threads)
{

	// check to see if we have a pointer to a list
	if(!threads)
	{
		printf("\n [!!] Unable to check status of NULL threads list.");
		return ERR_FAILURE;
	}

	// this is a flag which is incremented in the case that a thread has been discovered as running
	size_t thread_is_running = 0;

	// iterate through the threads
	for(threads = threads->first; threads; threads = threads->next)
	{

		// check for null (corruption marker)
		if(!threads)
		{
			printf("\n [!!] a thread element of NULL value has been detected.  Bad loop logic, possible corruption.");
			return ERR_FAILURE;
		}

		// if the thread is running, mark it
		if(CheckThreadStatusCode(threads) == BP_THREAD_STATUS_RUNNING)
			thread_is_running = 1;


	}

	// return indicating success
	if(thread_is_running)
		return ERR_SUCCESS;

	// if no threads are running, return indicating failure
	return ERR_FAILURE;

}


#endif /* __GNU__C */

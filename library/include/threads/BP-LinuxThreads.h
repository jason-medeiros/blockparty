/***************************************************************************
 *            BP-LinuxThreads.h
 *
 *  Thu Apr 19 19:20:53 2007
 *  Copyright  2007  User
 *  Email
 ****************************************************************************/
 
#ifndef BP_LINUX_THREADS_H
#define BP_LINUX_THREADS_H

//normal posix pthread C function pointer
typedef void* (*BP_PTHREAD_FN_PTR) (void*);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Blockparty Managed Thread Implementation %%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



// enter critical section for target selector
#define BP_THREAD_MANAGER_ENTER_CRITICAL_SECTION\
		{\
			P_BP_HASH_TABLE_REGISTRY hreg = thread_manager->system_hreg;                                                \
			BP_HASH_TABLE_ENTER_CRITICAL_SECTION(hreg);                                                                 \
		}\

// leave critical section for target selector
#define BP_THREAD_MANAGER_LEAVE_CRITICAL_SECTION\
		{\
			P_BP_HASH_TABLE_REGISTRY hreg = thread_manager->system_hreg;                                                \
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(hreg);                                                                 \
		}

// Thread status enumerator.  A thread is going to be occupying one of these
// three states.
typedef enum BP_THREAD_STATUS
{
	BP_THREAD_STATUS_UNDEFINED,
	BP_THREAD_STATUS_UNSTARTED,
	BP_THREAD_STATUS_RUNNING,
	BP_THREAD_STATUS_LOCKED_FOR_READ,
	BP_THREAD_STATUS_LOCKED_FOR_REDISPATCH,
	BP_THREAD_STATUS_EXITED,
	BP_THREAD_STATUS_BEING_MODIFIED_CURRENTLY_DATA_IS_UNRELIABLE,
	BP_THREAD_STATUS_REJOINED
};

// Thread manager status enumerator.
typedef enum BP_THREAD_MANAGER_STATUS
{
	BP_THREAD_MANAGER_STATUS_UNDEFINED,
	BP_THREAD_MANAGER_STATUS_MODIFYING_THREADS,
	BP_THREAD_MANAGER_STATUS_MODIFYING_SELF,
	BP_THREAD_MANAGER_STATUS_IDLE,
	BP_THREAD_MANAGER_STATUS_DESTROYING_SELF
};

// simple lock / unlock expanding macros
#define BP_THREAD_MANAGER_LOCK    sem_wait(&thread_manager->lock)
#define BP_THREAD_MANAGER_UNLOCK  sem_post(&thread_manager->lock)

// simple lock / unlock macros for managed thread
#define BP_MANAGED_THREAD_LOCK   sem_wait(&managed_thread->lock)
#define BP_MANAGED_THREAD_UNLOCK sem_post(&managed_thread->lock)


// forward decalre thread manager structure so it can be
// self-referenced.
typedef struct __BP_THREAD_MANAGER;


// This structure is used for holding managed threads.  Each thread
// created by the thread creation mechanisms below utilizes one of these
// structures.
typedef struct __BP_MANAGED_THREAD {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Thread General Items %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Thread safe tailqueue allocator used
	// within the thread.  Destroyed automatically
	// when a thread exits.
	P_BP_LINKL tq;

	// semaphore to lock the thread
	sem_t lock;

	// reference to the thread manager which this thread belongs
	__BP_THREAD_MANAGER * thread_manager;

	// actual thread id
	pthread_t thread_id;

	// current status of the managed thread
	BP_THREAD_STATUS current_status;

	// this reference is set on thread creation, and is used as a lock
	// which can be accessed directly from the thread without having to
	// parse through the thread_manager to find.  (prevents race conditions).
	// DO NOT DESTROY IN THREAD.
	P_BP_HASH_TABLE_REGISTRY system_hreg_ptr;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Start Routine and Parameter %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// threads start routine
	void * (*start_routine)(void*);

	// thread parameters
	void * thread_parameter;     // parameter block

	// thread parameter size
	size_t thread_parameter_size;

	// index of the parameter within the passed in index array
	size_t thread_parameter_idx;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Utility Information %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// name of the thread
	char *name;

	// description (human terms) of what the threads purpose is
	char *description; // string denoting information about this thread


} BP_MANAGED_THREAD, *P_BP_MANAGED_THREAD;


// Blockparty thread manager.
typedef struct __BP_THREAD_MANAGER {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Descriptive Items %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// name of the thread manager
	char * name;

	// description of the thread manager
	char * description;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% General Items %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// tailqueue allocator for
	P_BP_LINKL tq;

	// thread operation semaphore
	sem_t lock;

	// current status of the manager
	BP_THREAD_MANAGER_STATUS status;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Critical Sections %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// system hash registry (added in 2015, used for critical sections)
	P_BP_HASH_TABLE_REGISTRY system_hreg;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Counters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// total number of threads which were dispatched using this
	// manager.
	size_t threads_dispatched_total_n;

	// if we've created so many threads that we've overflowed the size_t,
	// this will contain the overflow count.
	size_t threads_dispatched_total_n_overflow_count;

	// when a thread exits, it is reclaimed.  This is the count of
	// reclaimed threads (total).
	size_t threads_reclaimed_total_n;

	// if we've reclaimed enough threads to overflow the above, store
	// the number of overflows here.
	size_t threads_reclaimed_total_n_overflow_count;


	// current number of running threads.
	size_t threads_running_n;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Individual Threads %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// threads engaged
	P_BP_MANAGED_THREAD * threads;

	// threads in the above stack
	size_t threads_n;


} BP_THREAD_MANAGER, *P_BP_THREAD_MANAGER;


// Create a new thread manager.
P_BP_THREAD_MANAGER BP_CreateThreadManager(char *name, char *description);

// Destroy the thread manager (
BP_ERROR_T BP_DestroyThreadManager(P_BP_THREAD_MANAGER thread_manager);

// Display the thread manager
BP_ERROR_T BP_DisplayThreadManager(P_BP_THREAD_MANAGER thread_manager, BP_BOOL display_individual_threads = BP_TRUE);

// Display the managed thread.
BP_ERROR_T BP_DisplayManagedThread(P_BP_MANAGED_THREAD managed_thread);




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
		char *   name = NULL,
		char *   description = NULL
);

// Start threads which aren't already currently running.
BP_ERROR_T BP_StartAllUnstartedThreads(P_BP_THREAD_MANAGER thread_manager);

// Start a thread (thread must exist within the thread manager for it to start, cannot be some random
// thread you created)
BP_ERROR_T BP_StartThread(P_BP_THREAD_MANAGER thread_manager, P_BP_MANAGED_THREAD managed_thread);

// Attempt to rejoin all the threads
BP_ERROR_T BP_RejoinAllThreads(P_BP_THREAD_MANAGER thread_manager, BP_BOOL only_try_to_reclaim_dont_force = BP_FALSE);

// Attempt to rejoin a single thread
BP_ERROR_T BP_RejoinThread
(
		P_BP_THREAD_MANAGER thread_manager,
		P_BP_MANAGED_THREAD managed_thread,
		BP_BOOL lock_thread_manager = BP_TRUE,
		BP_BOOL lock_managed_thread = BP_TRUE
);

// Looks for a thread candidate (either exited, or not started)
P_BP_MANAGED_THREAD BP_FindThreadDispatchCandidate(P_BP_THREAD_MANAGER thread_manager, BP_BOOL lock_for_redispatch = BP_TRUE);

// Attempts to find an exited thread, and redispatch it.
BP_ERROR_T BP_RedispatchThread
(
	P_BP_THREAD_MANAGER thread_manager,
	void *   arg,
	size_t   arg_len,
	size_t   arg_idx
);

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
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Old/Bad Thread Mechanisms %%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


 // This thread implementation is farily minimal for
 // transparency of use.  It is used to simply create
 // detached threads on the system.  All threads created
 // are stored in the below THREAD_LIST structure for 
 // reference.
 
 // Implementation Note: Due to problems that could arise
 // during thread cancelation (memory leaks and such); there
 // is no cancelation support in the library.  This is simply to 
 // prevent misuse.
 
// thread managment structure
typedef struct __BP_THREAD_MGMT {

	sem_t lock;
	size_t total_running_threads;

} BP_THREAD_MGMT, *P_BP_THREAD_MGMT;

// linked list that holds all threads utilized in a process
typedef struct _THREAD_LIST {

	// lock used before this structure can be read from/written to.
	pthread_mutex_t thread_lock;

	// actual thread id
	pthread_t threadId;

	// threads start routine
	void * (*startRoutine)(void*);
	
	// thread parameters
	void * sharedData;     // parameter block

	// thread parameter size
	int    sharedDataSize; // size of data passed
	
	// description (human terms) of what the threads purpose is
	char *threadDescription; // string denoting information about this thread
	
	// current thread state
	BP_THREAD_STATUS threadState;

	// pointer to remote managment data
	P_BP_THREAD_MGMT thread_mgmt_data;
	
	// list pointers
	struct _THREAD_LIST * first;
	struct _THREAD_LIST * next;
	struct _THREAD_LIST * prev;
		
} THREAD_LIST, *P_THREAD_LIST;


// creates a thread and returns it in a list structure
P_THREAD_LIST CreateThread(void *(start_routine)(void*), void *arg, int argLen, char *description, P_BP_THREAD_MGMT thread_mgmt_data);

// Attempts to join all threads.  Needs to be called after dispatching any
// number of threads.  It prevents contention.
BP_ERROR_T RejoinAllThreads(P_THREAD_LIST thread_list);


// redispatches a thread given a thread structure (basically re-runs a thread)
BP_ERROR_T RedispatchThread(P_THREAD_LIST thread_redispatch);

// returns ERR_SUCCESS if the thread is running, ERR_FAILURE if the thread has exited.  Also marks
// the thread list element to reflect a RUNNING/EXITED message via the structures state member.
BP_ERROR_T CheckIfThreadIsRunning(P_THREAD_LIST thread_elem);

// this version of the above function checks/marks the status of all of the threads.  If ANY thread
// is running, this function returns ERR_SUCCESS.  This function returns ERR_FAILURE if some threads
// are still running.  Check the lists->threadState variable to see which threads are exited etc.  This
// function does modify the elements status variable.
BP_ERROR_T CheckAllThreadsAndUpdateStatus(P_THREAD_LIST threads);

// adds a new thread element to a thread list
int AppendThreadToList(P_THREAD_LIST inList, P_THREAD_LIST appendList);

// Creates a new mutex (fast mutex, not recursive)
// and initializes it with the thread structure.
pthread_mutex_t * CreateNewMutex();
	
// Locks a fast mutex
int LockMutex(pthread_mutex_t * mutex);

// Try Locks a fast mutex
BP_ERROR_T TryLockMutex(pthread_mutex_t * mutex);

// DESIGNED TO BE CALLED FROM WITHIN THREAD: This function is used to set a status
// code to a thread list element safely within a threaded environment.  Call this before
// a thread has exited or you may find yourself unable to determine if a thread has exited or not.
// This is because pthread_kill does not work on exited threads (causes sigsegv).+
BP_ERROR_T SetThreadStatusCode(P_THREAD_LIST thread_elem, BP_THREAD_STATUS status_code);

// DESIGNED TO BE CALLED FROM THREAD MANAGER: This function can be used in a manager/dispatcher
// to safely check the status of a thread.  The threads are responsible for setting their own statuses
// upon exit/termination.
BP_THREAD_STATUS CheckThreadStatusCode(P_THREAD_LIST thread_elem);

// Destroys a fast mutex
int DestroyMutex(pthread_mutex_t * mutex, BP_BOOL free_memory_from_heap = BP_FALSE);

// Unlocks a fast mutex
int UnlockMutex(pthread_mutex_t * mutex);

// prints one element from a thread chain
void PrintThreadElement(P_THREAD_LIST inList);

// prints all the elements in the thread list
void PrintThreadList(P_THREAD_LIST inList);

// destroys one thread list element.  Automatically
// patches list if prev or next exist.
int FreeThreadElement(P_THREAD_LIST inList);

// destroys all the threads in a list
int FreeThreadList(P_THREAD_LIST inLIst);


#endif // BP_LINUX_THREADS_H


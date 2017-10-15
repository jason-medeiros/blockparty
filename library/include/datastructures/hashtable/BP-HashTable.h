/*
 * BP_HashTable.h
 */

#ifndef __BP_HASHTABLE_H
#define __BP_HASHTABLE_H

/*
 *
 * IMPORTANT :: IMPORTANT :: IMPORTANT :: IMPORTANT :: IMPORTANT
 *
 *  The ht_* allocator code uses memory allocators directly.  It does not use
 *  bp* versions of calloc/malloc/realloc etc.  This is because
 *  the bp versions of the functions actually use these hash tables
 *  in order to track memory with the global memory profiler.  Whenever
 *  you use bpcalloc, etc, a record is automatically added to the
 *  global memory profiler hash table if it's enabled.  Doing this
 *  allows us to track all memory allocated by the application, in
 *  total.  The drawback being that we can't circularly use bp
 *  routines within this hash table implementation directly.
 *
 *  This code uses nothing but pure C, specifically so that it can be
 *  snipped and used in pure C environmnents that don't have C++ maps.
 *
 * IMPORTANT :: IMPORTANT :: IMPORTANT :: IMPORTANT :: IMPORTANT
 *
 */


// set max table depth to 1024 (arbitrary but reasonable)
#define BP_HASH_REG_MAX_TABLE_DEPTH 4096


// Traversal path (think of directory paths on a filesystem) which is used to
// access nested registry tables.  Since tables can contain children, a traversal
// scheme must exist to navigate logical paths in a hash table registry.   The format
// is simply a char ** null terminated with a null pointer.
//
typedef char ** BP_HASH_TABLE_TRAVERSAL_PATH;


// enumeration of data types that can be stored in a named
// keypair entry.
typedef enum BP_HASH_TABLE_KEYED_ENTRY_TYPE
{
	BP_HASH_TABLE_KEYED_ENTRY_TYPE_UNSET,
	BP_HASH_TABLE_KEYED_ENTRY_TYPE_POINTER_EXTERNALLY_MANAGED, /* this type is unmanaged, meaning no free occurs to the data involved. */
	BP_HASH_TABLE_KEYED_ENTRY_TYPE_C_DATASTRUCTURE_EXTERNALLY_MANAGED,
	BP_HASH_TABLE_KEYED_ENTRY_TYPE_CPP_CLASS_POINTER_EXTERNALLY_MANAGED, // END EXTERNAL MANAGED / BEGIN MANAGED TYPES
	BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE,
	BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
	BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
	BP_HASH_TABLE_KEYED_ENTRY_TYPE_UNICODE16_STRING,
	BP_HASH_TABLE_KEYED_ENTRY_TYPE_UNICODE32_STRING,
	BP_HASH_TABLE_KEYED_ENTRY_TYPE_SIZE_T,
	BP_HASH_TABLE_KEYED_ENTRY_TYPE_SIGNED_INT,
	BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_JSON,
	BP_HASH_TABLE_KEYED_ENTRY_TYPE_MESSAGE_TO_WORKER,
	BP_HASH_TABLE_KEYED_ENTRY_TYPE_MESSAGE_FROM_WORKER,
	BP_HASH_TABLE_KEYED_ENTRY_TYPE_EVENT_UNPROCESSED,
	BP_HASH_TABLE_KEYED_ENTRY_TYPE_EVENT_PROCESSED
};



// Basic hash table structure corresponding to keyed data.  Used for most tables
// in the hash registry.  (mem table is an exception)
typedef struct __BP_HASH_TABLE_KEYED_ENTRY {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Locks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// lock for modifying/reading entry.
	sem_t lock;

	// time the write lock was acquired
	clock_t lock_acquired_time;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Entry Type Information %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// type of keyed entry
	BP_HASH_TABLE_KEYED_ENTRY_TYPE type;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Relative Pointers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This is a parent pointer.  It can be cast directly to
	// P_BP_HASH_TABLE_KEYED_ENTRY for later reads.
	void * parent;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Key and Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// uuid associated with this table entry
	char *key;

	// data associated with this table entry
	void * data;
	size_t data_size;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Optional Markers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// -- data encoding markers ---

	// developers can set this if they are assigning base64 data.
	BP_BOOL is_base64;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Externally Managed Data (not attached to hreg) %
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Each entry can have external data assigned to it.
	// The data which is assigned here is not internally managed
	// by the hash table, and when the hash registry is destroyed
	// these pointers are not manipulated in any way.
	void *external_entry_metadata;





	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% UT_Hash Structure %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// hash handle associated
	UT_hash_handle hh;


} BP_HASH_TABLE_KEYED_ENTRY, *P_BP_HASH_TABLE_KEYED_ENTRY;

// synonymn for P_BP_HASH_TABLE_KEYED_ENTRY so it can be shortened
#define P_BP_HTKE P_BP_HASH_TABLE_KEYED_ENTRY

// Types which will be found within the memory allocation table within the hash
// table registry.  Can be appended as program/types are extended.  These types
// are simply used to identify what types of allocations are still in the table.
typedef enum BP_HASH_REG_MEM_ENTRY_TYPE
{
	BP_HASH_REG_MEM_ENTRY_TYPE_UNSET,
	BP_HASH_REG_MEM_ENTRY_TYPE_GENERIC_ALLOCATION,
	BP_HASH_REG_MEM_ENTRY_TYPE_KEYED_ENTRY_IN_HIVE
};

// simple enumerator which describes the type of allocation associated
// with an allocation.
typedef enum BP_HASH_TABLE_ALLOCATOR {
	BP_HASH_TABLE_ALLOCATOR_NONE,
	BP_HASH_TABLE_ALLOCATOR_MALLOC,
	BP_HASH_TABLE_ALLOCATOR_CALLOC,
	BP_HASH_TABLE_ALLOCATOR_REALLOC,
	BP_HASH_TABLE_ALLOCATOR_STRDUP,
	BP_HASH_TABLE_ALLOCATOR_STRNDUP,
	BP_HASH_TABLE_ALLOCATOR_MMAP
};



// hash table structure corresponding to a generic void pointer
typedef struct __BP_HASH_TABLE_MEM_PTR_ENTRY {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Entry Type %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// type of allocation
	BP_HASH_REG_MEM_ENTRY_TYPE entry_type;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Key and Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ptr associated with this table entry
	void * key;

	// data associated with this table entry
	void * data;
	size_t data_size;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Allocation Info %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// allocator type
	BP_HASH_TABLE_ALLOCATOR alloc_type;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Allocation Profiling Information %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// file name in which the allocation occurs
	char * file_name;

	// line number where the allocation took place
	size_t line_number;

	// function name
	char * func;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Chunk Alloc Backtrace %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// backtrace
	char **alloc_backtrace;

	// backtrace symbols
	char ** alloc_backtrace_symbols;

	// total depth of the trace
	size_t alloc_backtrace_depth;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% UT_Hash Structure %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// hash handle associated
	UT_hash_handle hh;


} BP_HASH_TABLE_MEM_PTR_ENTRY, *P_BP_HASH_TABLE_MEM_PTR_ENTRY;




// Hash table structure, holds entries and contains a thread
// lock.
typedef struct __BP_HASH_TABLE_MEM {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Table Locks %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// write lock for hash table
	sem_t write_lock;

	// time the write lock was acquired
	clock_t write_lock_acquired_time;

	// read lock for hash table
	sem_t read_lock;

	// time the read lock was aquired
	sem_t read_lock_acquired_time;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Table Permissions %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// table permissions
	P_BP_HASH_TABLE_KEYED_ENTRY permissions;
	size_t                      permissions_n;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Table Entries %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// entries in the uuid table
	P_BP_HASH_TABLE_MEM_PTR_ENTRY entries;
	size_t                        entries_n;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Size Counters %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Note: The values below are adjusted during locked operations, and should
	//       be safe to read as long as you've acquired a lock.

	// total size of all heap allocations registered to this
	// particular registry.
	size_t managed_heap_alloc_total_size;

	// total size of all managed data
	size_t managed_data_size;

	// This is the total size of all BP_HASH_TABLE_MEM_PTR_ENTRY structures
	// contained below.
	size_t managment_structures_total_size;


} BP_HASH_TABLE_MEM, *P_BP_HASH_TABLE_MEM;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Hash Table Registry %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// The hash table registry handles multiple hash tables
// which can be used.  A registry can be used to limit
// lookup scopes between datasets.

typedef struct __BP_HASH_TABLE_REGISTRY {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Locks and Timing %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// lock for hash table
	sem_t lock;

	// time the lock was acquired
	clock_t lock_acquired_time;

	// this is used as a table lock that exists ABOVE the
	// lock member in this structure.  This allows us to lock the
	// entire registry in a routine that may have to acquire the
	// normal lock member several times.
	sem_t critical_section_lock;

	// When a thread acquires a critical section lock, it will be unlocked
	// throughout all gatechecks during critical section logic until the
	// critical section is unlocked.
	pthread_t critical_section_current_thread_id;

	// this is an internal counter for critical sections which prevents
	// a registry from exiting a critical section prematurely.
	size_t critical_section_lock_depth;

	// time which the critical section lock was acquired
	clock_t critical_section_lock_acquired_time;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Memory Associated with this Registry %%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// table associated with hash table related allocations
	P_BP_HASH_TABLE_MEM memory_table;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Registry Hive %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this hash table simply acts as a lookup for indexing
	// into the tables below.
	P_BP_HASH_TABLE_KEYED_ENTRY hive;

	// NOTE: the reason these stat records are stored here is because
	//       unlike the memory_table, the hive doesn't point to a
	//       managment structure.  It only points to cascading keyed
	//       entry pointers.

	// statistical count information: tables
	size_t total_tables_in_hive;

	// statistical count information: data entries
	size_t total_data_entries_in_hive;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Flags Associated with Registries %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if this is the global hash table registry
	BP_BOOL is_global;


} BP_HASH_TABLE_REGISTRY, *P_BP_HASH_TABLE_REGISTRY;




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Semaphore Utility Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// initializes (creates) a new semaphore
#define BP_HASH_TABLE_INIT_SEM(sem_to_lock) sem_init(&sem_to_lock, 0, 1)

// locks a semaphore
#define BP_HASH_TABLE_LOCK_SEM(sem_to_lock) sem_wait(&sem_to_lock) ? ERR_FAILURE : ERR_SUCCESS

// tries to lock a semaphore (non-blocking)
#define BP_HASH_TABLE_TRYLOCK_SEM(sem_to_lock) sem_trywait(&sem_to_lock) ? ERR_FAILURE : ERR_SUCCESS

// unlocks a semaphore
#define BP_HASH_TABLE_UNLOCK_SEM(sem_to_unlock) sem_post(&sem_to_unlock) ? ERR_FAILURE : ERR_SUCCESS

// destroys a semaphore
#define BP_HASH_TABLE_DESTROY_SEM(sem_to_destroy) sem_destroy(&sem_to_destroy)

// Returns zero if it's unlocked (or error), return 1 if it's locked.
#define BP_HASH_TABLE_SEM_IS_LOCKED(sem_to_check, tmp_val) sem_getvalue(&sem_to_check, &tmp_val) ? 0 : tmp_val




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Global Registry %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// global hash table registry
extern BP_HASH_TABLE_REGISTRY global_hash_table_registry;

// set the global hash init value to some arbitrary static value
extern size_t global_hash_table_init_ok_val;
extern size_t global_hash_table_init_ok;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Macros for Global Registry %%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// simple macro that externs the required global values
#define BP_GLOBAL_HASH_TABLE_REGISTRY \
	extern BP_HASH_TABLE_REGISTRY global_hash_table_registry;\
	extern size_t global_hash_table_init_ok_val;             \
	extern size_t global_hash_table_init_ok;


// Initializes the global hash table.  This should only ever be
// called onece per application run time.
#define BP_GLOBAL_HASH_TABLE_INIT \
	memset(&global_hash_table_registry, 0x00, sizeof(BP_HASH_TABLE_REGISTRY));\
	global_hash_table_init_ok = 0x12ab34cd56ef; \
	global_hash_table_init_ok_val = global_hash_table_init_ok;\
	global_hash_table_registry.is_global = BP_TRUE;\
	BP_HASH_TABLE_INIT_SEM(global_hash_table_registry.lock);\
	BP_HASH_TABLE_INIT_SEM(global_hash_table_registry.critical_section_lock);\


// simple macro to check global table has init'd ok
#define BP_GLOBAL_HASH_TABLE_INIT_OK \
	(global_hash_table_init_ok_val == global_hash_table_init_ok) && (global_hash_table_init_ok != 0)


// If the hreg parameter for any function is null, we override it with the
// global hash table.
#define BP_GLOBAL_HASH_TABLE_OVERRIDE(exit_value) \
	size_t global_hash_table_registry_override_failed = BP_FALSE;\
	if(!hreg)\
	{\
		if(BP_GLOBAL_HASH_TABLE_INIT_OK)\
		{\
			hreg = &global_hash_table_registry;\
		}\
		else\
			return exit_value;\
	}

// destroys all entries in the global hash table
#define BP_GLOBAL_HASH_TABLE_PURGE

// Closes the global hash table.  Table will not close if it contains entries.
#define BP_GLOBAL_HASH_TABLE_CLOSE\
                                                                   \
		/* destroy global hash registry */                         \
		BP_DestroyHashTableRegistry(NULL);                         \
		                                                           \
		/* destroy memory table  pointer*/ 					       \
		global_hash_table_init_ok_val = 0;                         \
		if(global_hash_table_registry.memory_table)                \
		{                                                          \
			free(global_hash_table_registry.memory_table);         \
			global_hash_table_registry.memory_table = NULL;        \
		}                                                          \


// this is a function masquerading as a macro.  It's too much work to implement it as a pure macro.
BP_BOOL BP_HASH_TABLE_IS_LOCKED(P_BP_HASH_TABLE_REGISTRY hreg);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Critical Section Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// enter critical section on a hash registry
#define BP_HASH_TABLE_ENTER_CRITICAL_SECTION(hreg)                                                                             \
	{                                                                                                                          \
                                                                                                                               \
        /* retrieve pthread id */                                                                                              \
        pthread_t critical_section_current_thread_id = pthread_self();                                                         \
	                                                                                                                           \
        /* set registry from parameter */                                                                                      \
        P_BP_HASH_TABLE_REGISTRY __builtin_tmp_local = (P_BP_HASH_TABLE_REGISTRY) hreg;                                        \
                                                                                                                               \
        /* if registry is null, set it as the GHT (default behavior) */                                                        \
        if(!__builtin_tmp_local)                                                                                               \
	           __builtin_tmp_local = BP_HashRegGetAddrGlobalRegistrySingleton();                                               \
	                                                                                                                           \
																															   \
		if(__builtin_tmp_local)                                                                                                \
		{                                                                                                                      \
			                                                                                                                   \
                                                                                                                               \
            if(critical_section_current_thread_id != __builtin_tmp_local->critical_section_current_thread_id)                  \
		    {                                                                                                                  \
				/* lock the semaphore */                                                                                       \
				BP_HASH_TABLE_LOCK_SEM(__builtin_tmp_local->critical_section_lock);                                            \
																															   \
				/* set lock thread id as current thread as we now have acquired the lock */                                    \
				__builtin_tmp_local->critical_section_current_thread_id = critical_section_current_thread_id;                  \
																															   \
				/* increment lock depth */                                                                                     \
				__builtin_tmp_local->critical_section_lock_depth++;                                                            \
																															   \
			}                                                                                                                  \
			else                                                                                                               \
			{                                                                                                                  \
				/* increase the lock depth without actually relocking since it's the same thread */                            \
				__builtin_tmp_local->critical_section_lock_depth++;                                                            \
				                                                                                                               \
			}                                                                                                                  \
                                                                                                                               \
		}                                                                                                                      \
                                                                                                                               \
	}


// leave a critical section
#define BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(hreg)                                                                             \
	{                                                                                                                          \
	                                                                                                                           \
	    /* retrieve pthread id */                                                                                              \
	    pthread_t critical_section_current_thread_id = pthread_self();                                                         \
	                                                                                                                           \
	    /* set registry from parameter */                                                                                      \
	    P_BP_HASH_TABLE_REGISTRY __builtin_tmp_local = (P_BP_HASH_TABLE_REGISTRY) hreg;                                        \
	                                                                                                                           \
		 /* if registry is null, set it as the GHT (default behavior) */                                                       \
	    if(!__builtin_tmp_local)                                                                                               \
		     __builtin_tmp_local = BP_HashRegGetAddrGlobalRegistrySingleton();                                                 \
                                                                                                                               \
		/* if we have a registry to work with, go ahead and leave the critical section */                                      \
		if(__builtin_tmp_local)                                                                                                \
		{                                                                                                                      \
		                                                                                                                       \
																															   \
			/* if the pthread id matches the current thread id, it's safe to go ahead with the unlock sequence */       	   \
			if(critical_section_current_thread_id == __builtin_tmp_local->critical_section_current_thread_id)                  \
			{                                                                                                                  \
																															   \
				/* decrement lock depth if necessary */                  												       \
				if(__builtin_tmp_local->critical_section_lock_depth)											               \
				{                                                                                                              \
					__builtin_tmp_local->critical_section_lock_depth--;                                                        \
				}                                                                                                              \
																															   \
				/* If we are at base lock depth, perform the actual unlock*/                                                   \
				if(!__builtin_tmp_local->critical_section_lock_depth)											               \
				{                                                                                                              \
					__builtin_tmp_local->critical_section_current_thread_id = 0;                                               \
					BP_HASH_TABLE_UNLOCK_SEM(__builtin_tmp_local->critical_section_lock);                                      \
				}                                                                                                              \
																															   \
			}                                                                                                                  \
																															   \
		}                                                                                                                      \
     }



//! Simple macro to enter a GHT critical section (allows you to operate on the ght without worrying about
//! overwriting things.
#define BP_GHT_ENTER_CRITICAL_SECTION \
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION((P_BP_HASH_TABLE_REGISTRY) NULL)

//! Simple macro to leave a GHT critical section.
#define BP_GHT_LEAVE_CRITICAL_SECTION \
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION((P_BP_HASH_TABLE_REGISTRY) NULL)

//! Leave the hash table critical and return from the function.
#define BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(hreg, ret_val)    \
	{                                                                               \
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(hreg);                                 \
		return ret_val;                                                             \
	}

//! Macro for leaving a function which is currently holding a critical section.
#define BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ret_val) \
	{\
		BP_GHT_LEAVE_CRITICAL_SECTION;\
		return ret_val;\
	}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Path Definition Open/Close for Easy Path Creation %%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// example usage:
/*
json_cpp->addTableByPath
(
        BP_PATH_OPEN
                "test_top",
                "test_mid",
                "test_low"
        BP_PATH_CLOSE
);
*/

// -- path open for parameters to functions only ------

// open the path by creating a dynamic vector.
#define BP_PATH_OPEN\
		std::vector<char *>{

// close vector by null terminiating and then accessing data directly
#define BP_PATH_CLOSE\
		, NULL}.data()


// -- path open but copy also ---

// attempt to create a copy of a path on the heap
#define BP_PATH_OPEN_COPY_HEAP\
		BP_StringArrayCreateCopyOnHeap\
		(\
			BP_PATH_OPEN

// close the path
#define BP_PATH_CLOSE_COPY_HEAP\
			BP_PATH_CLOSE\
		)


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Ptr Array Definition for Easy Array Creation %%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// example usage:
/*

void **stack_of_void_ptrs = \
	BP_PTR_ARRAY_OPEN(void *)
		void_ptr_1,
		void_ptr_2,
		void_ptr_3
	BP_PTR_ARRAY_CLOSE;

 */


// Simple way to declare arrays of pointer types that are
// typically submitted as parameters to routines.
#define BP_PTR_ARRAY_OPEN(ptr_type)\
	std::vector<ptr_type>{

// Close the ptr array.
#define BP_PTR_ARRAY_CLOSE\
	, NULL}.data()


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Modified Iterators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// this iterator is an easier to understand version of UTHash'
// HASH_ITER, except it doesn't require declartive iteration pointers.
// Pretty sure this isn't C99 compatible.
//
//
// Usage ex: (taken from sqlite bindings)
//
//
//
// test iter loop
// if(sqli->queries)
// BP_HASH_ITER_OPEN(sqli->queries->hive, outer_entry)
// {
//
//		// test iter loop
// 		BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) sqli->queries->hive->data, query_entry)
// 		{
//
// 			printf("\n Got Inner Entry HERE?: %p - %s", query_entry, query_entry->key);
// 			if(!query_entry)
// 				continue;
//
//	 		// test iter loop
// 			BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) query_entry->data, inner_entry)
// 			{
// 				printf("\n Table HERE?: %p - %s", inner_entry, inner_entry->key);
//
//	 		} BP_HASH_ITER_CLOSE;
//
//	 } BP_HASH_ITER_CLOSE;
//
// } BP_HASH_ITER_CLOSE;
//
//
//
#define BP_HASH_ITER_OPEN(table, entry_name)\
		if(table)\
		{\
			/* tmp/out values for display lookups */  \
			P_BP_HASH_TABLE_KEYED_ENTRY entry_name        = NULL; \
			P_BP_HASH_TABLE_KEYED_ENTRY entry_name##tmp   = NULL; \
			HASH_ITER(hh, table, entry_name, entry_name##tmp)\
			{\

// this closes BP_HASH_ITER_OPEN and BP_HASH_ITER_FROM_PATH.
#define BP_HASH_ITER_CLOSE\
			}\
		}\


// This works the same as BP_HASH_ITER_OPEN, except in that it uses a char ** lookup path to first
// lookup the table.
#define BP_HASH_ITER_FROM_LOOKUP_PATH_OPEN(hreg, table_lookup_path, entry_name)\
		if(table_lookup_path[0])                                                                        \
		{                                                                                               \
			/* attempt table lookup */                                                                  \
			P_BP_HASH_TABLE_KEYED_ENTRY table = BP_HashRegLookupTableByPath(hreg, table_lookup_path);   \
			if(table)                                                                                   \
			if(table->data)                                                                             \
			{                                                                                           \
                                                                                                        \
				/* tmp/out values for display lookups */                                                \
				P_BP_HASH_TABLE_KEYED_ENTRY entry_name        = NULL;                                   \
				P_BP_HASH_TABLE_KEYED_ENTRY entry_name##tmp   = NULL;                                   \
				P_BP_HASH_TABLE_KEYED_ENTRY table_inner = (P_BP_HASH_TABLE_KEYED_ENTRY) table->data;    \
				                                                                                        \
				HASH_ITER(hh, table_inner, entry_name, entry_name##tmp)                                 \
				{                                                                                       \

// close the lookup iter loop
#define BP_HASH_ITER_FROM_LOOKUP_PATH_CLOSE\
				}                                                                                       \
			}                                                                                           \
		}                                                                                               \



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create / Destroy Hash Table Registries %%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// create a new hash table
P_BP_HASH_TABLE_REGISTRY BP_CreateHashTableRegistry();

// Destroys a hash table registry, removing all associated allocations.
BP_ERROR_T BP_DestroyHashTableRegistry(P_BP_HASH_TABLE_REGISTRY hreg);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Global Registry Gettr/Settrs %%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// simple routine to get the address of the global registry singleton (prevents
// having to extern structures)
P_BP_HASH_TABLE_REGISTRY BP_HashRegGetAddrGlobalRegistrySingleton();

// checks global flag to ensure that init was ok
BP_BOOL BP_HashRegGlobalRegistryInitOk();


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Path Display Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// simply displays a key path to standard out
BP_ERROR_T BP_HashRegDisplayKeyPath
(
	BP_HASH_TABLE_TRAVERSAL_PATH table_path
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Keyed Entry To Path Reverse Lookups %%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempts to get a path query from a keyed entry by performing a reverse lookup
// on the entry.  Result is bp_calloced and must be free'd with bpfree().
char * BP_HashRegEntryToPathQuery(P_BP_HASH_TABLE_KEYED_ENTRY entry);

// Attemmpts to convert a hash entry to path.  Result is bp_calloced and must be free'd
// with BP_DestroyPtrArray() and bpfree()
BP_HASH_TABLE_TRAVERSAL_PATH BP_HashRegEntryToPath(P_BP_HASH_TABLE_KEYED_ENTRY entry);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Manipulate Tables Within Registries %%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// ==============================================================
// DONT CALL THIS, THIS IS AN INTERNAL ROUTINE THAT SHOULN'T BE
// USED UNLESS YOU KNOW WHAT YOU'RE DOING.
// ==============================================================
// Add a table to the registry by linking member data into place.  This has
// specific defined behavior which can be used to create custom linking
// patterns.  By custom I mean mangled.  As in if you use this function
// you're likely to mangle everything to hell because you don't know what
// you're doing.  I recommend you use BP_HashRegAddTableByPath instead.
BP_ERROR_T BP_HashRegAddTable
(
	P_BP_HASH_TABLE_REGISTRY    hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY table_to_append_to,
	P_BP_HASH_TABLE_KEYED_ENTRY new_table_entry
);

// Add a table to the registry by a provided path.
BP_ERROR_T BP_HashRegAddTableByPath
(
	P_BP_HASH_TABLE_REGISTRY     hreg,
	BP_HASH_TABLE_TRAVERSAL_PATH table_path
);

// Add a table to the registry by a provided path.
BP_ERROR_T BP_HashRegAddTableByPathQuery
(
	P_BP_HASH_TABLE_REGISTRY hreg,
	char *lookup_query
);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Entry/Table Lookup Routines %%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Will search a traversal path for a table.  Returns NULL if the table cannot be found. Type
// must be BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE.
P_BP_HASH_TABLE_KEYED_ENTRY BP_HashRegLookupTableByPath
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	BP_HASH_TABLE_TRAVERSAL_PATH  table_path
);

// Will attempt to lookup a hash table entry with a char * representation
// of a hash lookup (eg. "/table_1/table_2".  This is mostly for convienence
// as this funciton simply parses the lookup query and calls BP_HashRegLookupTableByPath.
P_BP_HASH_TABLE_KEYED_ENTRY BP_HashRegLookupTableByPathQuery
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	char *                        table_path_lookup_query
);

// Will search a traversal path for a table.  Returns NULL if the table cannot be found.  Type must not
// be a table.
P_BP_HASH_TABLE_KEYED_ENTRY BP_HashRegLookupTableEntryByPath
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	BP_HASH_TABLE_TRAVERSAL_PATH  entry_path
);

// Will search a traversal path and return an ENTRY based on a path query.
P_BP_HASH_TABLE_KEYED_ENTRY BP_HashRegLookupTableEntryByPathQuery
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	char *                        entry_path_lookup_query
);

// --- relative lookups by path/path_query -----------

// Will search a traversal path and return an ENTRY based on a path query.
P_BP_HASH_TABLE_KEYED_ENTRY BP_HashRegLookupTableEntryByDataKeyIndexAndTablePath
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	BP_HASH_TABLE_TRAVERSAL_PATH  table_path,
	char *                        data_index_key
);

// Will search a traversal path and return an ENTRY based on a path query.
P_BP_HASH_TABLE_KEYED_ENTRY BP_HashRegLookupTableEntryByDataKeyIndexAndTablePathQuery
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	char *                        table_path_lookup_query,
	char *                        data_index_key
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Relative Entry/Table Lookups %%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// These routines utilize a P_BP_HASH_TABLE_KEYED_ENTRY table as a
// first parameter rather than a hreg.  This allows for localized
// table lookups for hashes.  The second parameter is a key to
// use for the lookup.
P_BP_HASH_TABLE_KEYED_ENTRY BP_HashRegLookupEntryFromHashKeyViaRelativePosition
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY relative_position,
	char *key
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create Paths from Entries %%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to create a reasonable string from a keyed entry
// by crawling backwards and joining keys.
char * BP_HashRegCreateReasonableStringPathFromKeyedEntry
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY relative_position
);

// create path from keyed entry
char ** BP_HashRegCreatePathFromKeyedEntry
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY relative_position
);




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Checkout/Checkin Routines %%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// ==============================================================
// DONT CALL THIS, THIS IS AN INTERNAL ROUTINE THAT SHOULN'T BE
// USED UNLESS YOU KNOW WHAT YOU'RE DOING.
// ==============================================================
// Attempts to checkout a data entry.  This is different than checking out a table in that
// a data entry has no possibility of having children to recurse into.
BP_ERROR_T BP_HashRegCheckOutData
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY   data,
	BP_BOOL                       block_till_checkout_established
);

// Attempts to checkout a hash table at a provided path.  The third parameter
// determines whether or not the routine will attempt to block until locking
// the table (checking it out) completes.
BP_ERROR_T BP_HashRegCheckOutTable
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY   table,
	BP_BOOL                       block_till_checkout_established
);

// Attempts to checkout a hash table at a provided path.  The third parameter
// determines whether or not the routine will attempt to block until locking
// the table (checking it out) completes.
BP_ERROR_T BP_HashRegCheckOutTableByPath
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	BP_HASH_TABLE_TRAVERSAL_PATH  table_path,
	BP_BOOL                       block_till_checkout_established
);

// Attempts to check in a singular data entry.
BP_ERROR_T BP_HashRegCheckInData
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY   data
);

// Attempts to check in a table which has already been checked out.
BP_ERROR_T BP_HashRegCheckInTable
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY   table
);

// Attempts to check in a table which has already been checked out, by path.
BP_ERROR_T BP_HashRegCheckInTableByPath
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	BP_HASH_TABLE_TRAVERSAL_PATH  table_path
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Destroy Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// ==============================================================
// DONT CALL THIS, THIS IS AN INTERNAL ROUTINE THAT SHOULN'T BE
// USED UNLESS YOU KNOW WHAT YOU'RE DOING.
// ==============================================================
// This destroys an individual table object.  This phyically undoes the link within
// the table structure.  This is called by BP_HashRegDestroyTable_r and should not
// be called directly.
BP_ERROR_T BP_HashRegDestroyKeyedEntryTable
(
	P_BP_HASH_TABLE_REGISTRY    hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY entry
);

// ==============================================================
// DONT CALL THIS, THIS IS AN INTERNAL ROUTINE THAT SHOULN'T BE
// USED UNLESS YOU KNOW WHAT YOU'RE DOING.
// ==============================================================
// This destroys an individual entry and attempts to remove it from a table.  This should NEVER be called
// unless the record is locked safely first.  This is called by BP_HashRegDestroyTable_r and
// should not be called directly.
BP_ERROR_T BP_HashRegDestroyKeyedEntryData
(
	P_BP_HASH_TABLE_REGISTRY    hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY entry
);

// ==============================================================
// DONT CALL THIS, THIS IS AN INTERNAL ROUTINE THAT SHOULN'T BE
// USED UNLESS YOU KNOW WHAT YOU'RE DOING.
// ==============================================================
// This is the recursing version of the destroy table routine.  This is necessary
// because we have to assure we can checkout a full directory before deleting the
// directory.  The BP_HashRegDestroyTable routine performs the initial table checkout,
// and this routine recurses into the table and unlocks/deletes members
// of the table.  This routine should never be called directly unless
// first ensuring that the table they're calling this with is checked out
// beforehand.
BP_ERROR_T BP_HashRegDestroyTable_r
(
	P_BP_HASH_TABLE_REGISTRY hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY table
);

// ==============================================================
// SERIOUSLY, DON'T CALL BP_HashRegDestroyKeyedEntryTable,
// BP_HashRegDestroyKeyedEntryData, or  BP_HashRegDestroyTable_r
// DIRECTLY.  DONT DO IT.  JUST DONT.
// ==============================================================

// Call these routines below instead.  They perform checkouts which
// maintains sanity within the hash registry.

// Attempts to destroy the table at the element pointer, recursing
// into the path and destroying all data recursively inward till
// finally destroying itself.  This requires that everything in the
// hive is unlocked, or the request will fail with ERR_FAILURE. This
// is primarily called via BP_HashRegDestroyTableByPath.
BP_ERROR_T BP_HashRegDestroyTable
(
	P_BP_HASH_TABLE_REGISTRY hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY table
);


// Attempts to checkout a registry and seek a table within it.  Within that
// table, it will try to remove the provided entry parameter.  The table the
// entry belongs to is present within the entry itself as a backward lookup,
// so no table parameter needs to be supplied.
BP_ERROR_T BP_HashRegDestroyEntry
(
	P_BP_HASH_TABLE_REGISTRY    hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY entry_within_table_to_destroy
);


// --- Destroy By Path ---------------------------

// Attempts to destroy the table at the current path, recursing
// into the path and destroying all data recursively inward till
// finally destroying itself.  This requires that everything in the
// hive is unlocked, or the request will fail with ERR_FAILURE.
BP_ERROR_T BP_HashRegDestroyTableByPath
(
	P_BP_HASH_TABLE_REGISTRY hreg,
	BP_HASH_TABLE_TRAVERSAL_PATH table_path
);


// Attempts to destroy a table entry by a provided path.
BP_ERROR_T BP_HashRegDestroyEntryByPath
(
	P_BP_HASH_TABLE_REGISTRY    hreg,
	BP_HASH_TABLE_TRAVERSAL_PATH table_path
);


// --- Destroy Entire Hive ----------------------

// Attempts to destroy an entire hive table by table.
BP_ERROR_T BP_HashRegDestroyHive
(
	P_BP_HASH_TABLE_REGISTRY hreg
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Convert To/From HREG String Paths %%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Examples of String Paths:
//
// "/some_key/some_key/some_key/item"
// "/key1/key2/key3/item1"
// "/key_root"
//
// NOTE: These routines are only to be used on items which
//       do not have the special character "/" in them. Strings
//       must be reasonable/printable and not contain any line delimiting
//       chars.  Failure to abide by this may provide erroneous parse data.
//       Path lookups must not end with a trailing "/".  They must target
//       a specific node destination.

// This turns a char ** into a path lookup string.  The resulting
// string is bpcalloced and must be bpfreed.
char * BP_HashRegPathArrayToStringLookup(char **path);

// Tests to see if a hash registry index passes filter tests
BP_ERROR_T BP_HashRegPathArrayIndexFilterTest(char *index_str, size_t index_str_len);


// This turns a char * "/lookup/path/index" styled string into a
// char ** array which is usable for path lookups.  Result is not managed
// memory (must be freed with BP_DestroyPointerArrayNullTerminated())
char ** BP_HashRegStringLookupToPathArray(char *lookup, size_t lookup_len);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Add Data To Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// IMPORTANT: Be sure to investigate BP_HASH_TABLE_KEYED_ENTRY_TYPE before
//            attempting to modify the behavior of BP_HashRegAddDataToTable, as
//            data is added as either managed or unmanaged.  Managed or unmanaged
//			  types are enumerated within  BP_HASH_TABLE_KEYED_ENTRY_TYPE.


// link/add data to a table directly
BP_ERROR_T BP_HashRegAddDataToTable
(
		P_BP_HASH_TABLE_REGISTRY hreg,
		P_BP_HASH_TABLE_KEYED_ENTRY table,
		char * key,
		BP_HASH_TABLE_KEYED_ENTRY_TYPE data_type,
		void * data,
		size_t data_size
);

// link/add data to table by path
BP_ERROR_T BP_HashRegAddDataToTableByPath
(
		P_BP_HASH_TABLE_REGISTRY hreg,
		BP_HASH_TABLE_TRAVERSAL_PATH table_path,
		char * key,
		BP_HASH_TABLE_KEYED_ENTRY_TYPE data_type,
		void * data,
		size_t data_size
);


// This function allows you to add data to a table by using a lookup query in the form of
// "/path/whatever/index_add_point_will_be_here".
BP_ERROR_T BP_HashRegAddDataToTableByPathQuery
(
		P_BP_HASH_TABLE_REGISTRY hreg,
		char *lookup_query,
		char * key,
		BP_HASH_TABLE_KEYED_ENTRY_TYPE data_type,
		void * data,
		size_t data_size
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Display memory allocations.
BP_ERROR_T BP_HashRegDisplayMemoryAllocations(P_BP_HASH_TABLE_REGISTRY hreg);

// display memory tables
BP_ERROR_T BP_HashRegDisplayMemory
(
		P_BP_HASH_TABLE_REGISTRY hreg,
		BP_BOOL                  display_entries
);

// Display a hive.
BP_ERROR_T BP_HashRegDisplayHive
(
	P_BP_HASH_TABLE_REGISTRY hreg,
	BP_BOOL                  display_header,
	BP_BOOL                  display_entries
);

// There are two real differing types in the hash registry/tables.  The first type
// is a table.  A table can contain either other tables, or data entries.  This routine
// displays data entries, not tables.
BP_ERROR_T BP_HashRegDisplayDataEntry
(
		P_BP_HASH_TABLE_REGISTRY    hreg,
		P_BP_HASH_TABLE_KEYED_ENTRY data_entry,
		size_t                      display_tab_depth
);


// display single table (optionally set tab depth)
BP_ERROR_T BP_HashRegDisplayTable
(
	P_BP_HASH_TABLE_REGISTRY    hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY table_entry,
	size_t                      display_tab_depth,
	BP_BOOL                     recurse,
	BP_BOOL                     display_entries
);

// display the hash tables stored in this hash registry
BP_ERROR_T BP_HashRegDisplayTables
(
	P_BP_HASH_TABLE_REGISTRY hreg,
	BP_BOOL                  recurse,
	BP_BOOL                  display_entries
);

// display the table entries
BP_ERROR_T BP_HashRegDisplayTableEntries
(
		P_BP_HASH_TABLE_REGISTRY hreg,
		char * table_name
);

// this will display the
BP_ERROR_T BP_HashRegDisplayPath
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	BP_HASH_TABLE_TRAVERSAL_PATH  table_path,
	BP_BOOL                       display_table_entries,
	BP_BOOL                       display_extended_entry_info,
	size_t                        max_recurse_level
);

// simple catch all routine for displaying all information stored within a hash registry
BP_ERROR_T BP_HashRegDisplay
(
		P_BP_HASH_TABLE_REGISTRY hreg,
		BP_BOOL                  display_mem_entries,
		BP_BOOL                  display_hive_entries
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Manipulate Memory Entries %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/*
 *
 * Memory Entries are different than standard table entries stored in the hash
 * table registry in that they are stored within their own table in the registry,
 * outside of the table_lookups hive.  These routines below do not modify the hive
 * at all directly.  While it's common for entries in the hive to have their memory
 * allocated via the memory table, the memory table does not keep key references to
 * the hive data.  Instead it keeps pointer references for quick lookup of pointer
 * values.  It's a bit confusing, but it's an important distinction to understand.
 *
 * ~J
 */


// add a key to a registry
void * BP_HashRegAddMemoryEntry
(
		P_BP_HASH_TABLE_REGISTRY      hreg,
		BP_HASH_REG_MEM_ENTRY_TYPE    entry_type,
		size_t                        initial_data_size,
		BPLN_PARMS
);

// resize an item within the has
void * BP_HashRegResizeMemoryEntry
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	void *                   	  old_data_pointer,
	size_t                   	  new_data_size,
	BPLN_PARMS
);

// lookup an entry based on a provided address
P_BP_HASH_TABLE_MEM_PTR_ENTRY BP_HashRegMemPtrFind
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	void *addr
);

// Attempts to delete all the memory entries in a registry.  This should
// be only used during the destruction of the entire hash table registry;
// failing to heed this warning and attempting to use it yourself can
// very easily lead to a corruption, as hive entries correlate to memory
// entries, and if a hive entry is missing a memory entry, it causes a
// bad reference.  READ THIS WHOLE TEXT BLOCK.
BP_ERROR_T BP_HashRegDeleteAllMemoryEntries
(
	P_BP_HASH_TABLE_REGISTRY      hreg
);

// delete a memory entry
BP_ERROR_T BP_HashRegDeleteMemoryEntry
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	P_BP_HASH_TABLE_MEM_PTR_ENTRY entry
);

// delete a memory entry
BP_ERROR_T BP_HashRegDeleteMemoryEntryByDataPtr
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	void *                        data
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Validation Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// checks if a memory pointer is registered as having been allocated by
// our registry.  Only chunks which are currently still registered in
// this registry will be validated as true.
BP_BOOL BP_HashRegMemPtrValid(P_BP_HASH_TABLE_REGISTRY hreg, void *ptr);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Entry Count Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Note: Count routines do not recurse.

// count policies
typedef enum BP_HASH_TABLE_COUNT_POLICY
{
	BP_HASH_TABLE_COUNT_POLICY_UNSET,
	BP_HASH_TABLE_COUNT_POLICY_COUNT_ONLY_DATA_ENTRIES,
	BP_HASH_TABLE_COUNT_POLICY_COUNT_ONLY_TABLE_ENTRIES,
	BP_HASH_TABLE_COUNT_POLICY_COUNT_ALL
};

// count from table pointer
size_t BP_HashRegCountEntriesInTable
(
	P_BP_HASH_TABLE_KEYED_ENTRY table,
	BP_HASH_TABLE_COUNT_POLICY  count_policy = BP_HASH_TABLE_COUNT_POLICY_COUNT_ALL
);

// count by path lookup
size_t BP_HashRegCountEntriesInTableByPath
(
	P_BP_HASH_TABLE_REGISTRY   hreg,
	char **                    path,
	BP_HASH_TABLE_COUNT_POLICY count_policy = BP_HASH_TABLE_COUNT_POLICY_COUNT_ALL
);

// count by path query (does not recurse)
size_t BP_HashRegCountEntriesInTableByPathQuery
(
	P_BP_HASH_TABLE_REGISTRY   hreg,
	char *                     lookup_query,
	BP_HASH_TABLE_COUNT_POLICY count_policy = BP_HASH_TABLE_COUNT_POLICY_COUNT_ALL
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Hash Table Generic Allocator Routines %%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


/*
 * These routines are utility routines to provide the same level of
 * functionality found in the tailqueue allocator, such that allocations
 * become VERY simple to implement so long as you're working with a valid
 * hash table registry.  Most of these routines are simply frontend macros
 * for the various hash table routines found above.
 *
 * NOTE: Like hashtable other functions, calling with a null hreg activates
 *       allocation via GHT.
 *
 */

// duplicates a (REASONABLE) string and stores it in the hash table
char * ht_strdup_real
(
		P_BP_HASH_TABLE_REGISTRY hreg,
		char *dup,
		BPLN_PARMS
);

// duplicates a string and stores it in the hash table
char * ht_strndup_real
(
		P_BP_HASH_TABLE_REGISTRY hreg,
		char * dup,
		size_t len,
		BPLN_PARMS
);

void * ht_realloc_real
(
		P_BP_HASH_TABLE_REGISTRY hreg,
		void * addr,
		size_t new_size,
		BPLN_PARMS
);

void * ht_malloc_real
(
		P_BP_HASH_TABLE_REGISTRY hreg,
		size_t size,
		BPLN_PARMS
);

void * ht_calloc_real
(
		P_BP_HASH_TABLE_REGISTRY hreg,
		size_t size,
		size_t size_n,
		BPLN_PARMS
);

// Destroys memory allocated in the hash table (this routine routines success/failure)
BP_ERROR_T ht_free_real
(
		P_BP_HASH_TABLE_REGISTRY hreg,
		void *ptr,
		BPLN_PARMS
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Value Basic Type Test Routines %%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// hash table entry is a blob type (can be used for ht_*cmp routines)
BP_ERROR_T ht_entry_is_blob(P_BP_HASH_TABLE_KEYED_ENTRY entry);

// The entry is a named hash table containing zero or more sub entries which can be
// iterated through
BP_ERROR_T ht_entry_is_table(P_BP_HASH_TABLE_KEYED_ENTRY entry);

// Checks if the entry is a "special" type such as a class pointer.
BP_ERROR_T ht_entry_is_special_type(P_BP_HASH_TABLE_KEYED_ENTRY entry);

// Checks if an entry is an event type.
BP_ERROR_T ht_entry_is_event_type(P_BP_HASH_TABLE_KEYED_ENTRY entry);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Value Compare Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// --- value compares for entry data ---------------

// memory compare hash table keyed entry (binary only compare).  Returns ERR_SUCCESS if
// the buffers match (are exactly the same).  Returns ERR_FAILURE if entries differ at all.
BP_ERROR_T ht_memmatch_entry
(
		P_BP_HASH_TABLE_KEYED_ENTRY entry,
		char  *buff,
		size_t buff_size
);

// memory compare hash table keyed entry (checks that string is reasonable printable)
BP_ERROR_T ht_strncmp_entry
(
		P_BP_HASH_TABLE_KEYED_ENTRY entry,
		char  *buff,
		size_t buff_size
);

// --- value compares for keys ---------------


// memory compare hash table keyed entry (binary only compare).  Returns ERR_SUCCESS if
// the keys match (are exactly the same).  Returns ERR_FAILURE if keys differ at all.
BP_ERROR_T ht_memmatch_key
(
		P_BP_HASH_TABLE_KEYED_ENTRY entry,
		char  *buff,
		size_t buff_size
);

// memory compare hash table keyed entry (binary only compare/front part match only).  Returns ERR_SUCCESS if
// the keys match (are matched within their front portions)
BP_ERROR_T ht_memmatch_key_front_part
(
		P_BP_HASH_TABLE_KEYED_ENTRY entry,
		char  *buff,
		size_t buff_size
);

// string compare hash table keyed entry (binary only compare).  Returns ERR_SUCCESS if
// the keys match (are exactly the same).  Returns ERR_FAILURE if keys differ at all.
BP_ERROR_T ht_strncmp_key
(
		P_BP_HASH_TABLE_KEYED_ENTRY entry,
		char  *buff,
		size_t buff_size
);

// Checks to see if a key and value are the same.
BP_ERROR_T ht_entry_key_and_value_are_the_same
(
		P_BP_HASH_TABLE_KEYED_ENTRY entry
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Hash Table Allocator Macros (interface to real) %%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// macros with added line parameters

// destroys memory allocated via the ht alloc functions
#define ht_free(hreg, ptr)                           ht_free_real(hreg, ptr,  BPLN)

// strdup macro for hash table (with parameter tracking)
#define ht_strdup(hreg, dup)                         ht_strdup_real(hreg, dup, BPLN)
#define ht_strndup(hreg, dup, len)                   ht_strndup_real(hreg, dup, len, BPLN)

// allocators
#define ht_malloc(hreg, size)                        ht_malloc_real(hreg, size, BPLN)
#define ht_calloc(hreg, size, clear_elem_size)       ht_calloc_real(hreg, size, clear_elem_size, BPLN)
#define ht_realloc(hreg, realloc_ptr, realloc_size)  ht_realloc_real(hreg, realloc_ptr, realloc_size, BPLN)



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Global Hash Table Allocator Macros (interface to real) %%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// GHT: destroys memory allocated via the ht alloc functions
#define ght_free(ptr)                          ht_free   (NULL, ptr)

// GHT: strdup macro for hash table (with parameter tracking)
#define ght_strdup(str)                        ht_strdup (NULL, dup)
#define ght_strndup(str, len)                  ht_strndup(NULL, dup, len)

// GHT: allocators
#define ght_malloc(size)                       ht_malloc (NULL, size)
#define ght_calloc(size, clear_elem_size)      ht_calloc (NULL, size, clear_elem_size)
#define ght_realloc(realloc_ptr, realloc_size) ht_realloc(NULL, realloc_ptr, realloc_size)





#endif /* __BP_HASHTABLE_H */

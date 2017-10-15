/*
 * BP_LibraryMemoryProfiling.cc
 *
 *  Created on: Aug 26, 2013
 *      Author: root
 */


#include "../../include/BP-Main.h"

//
// Developer Notice:  The code below is fairly complex, but if read carefully should
// be fairly simple to understand if provided some context.  The contextual summary of the code
// found below is simply described as an overlay to the basic libc memory allocators.  This
// provides blockparty with some capacity to do runtime checks/allocation thresholding during allocation
// to prevent undefined behavior.
//
// Developer Notice:  A secondary layer to memory allocation is also provided within blockparty, and should
// be used in lieu of using the allocator code below.  You can find this capacity
// within the BP_LinkedList.cc file, which provides direct access to allocators
// managed directly via a libc (optimised) tail queue.  While a bit less granular, the tail
// queue allocators are designed to be more developer friendly.  Additionally, blockparty
// also includes a hash table allocator, which has larger entry sizes but O(1) lookup speed.
//



#if BP_USE_HASH_TABLE_PROFILER


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Global Memory Profiler Hash Table Registry %%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// global hash table registry
BP_HASH_TABLE_REGISTRY global_mprof_hash_table_registry;

// set the global hash init value to some arbitrary static value
size_t global_mprof_hash_table_init_ok_val;
size_t global_mprof_hash_table_init_ok;

// If this value is non-zero the global memory profiler will create
// backtraces for every allocaction.  This should only be enabled
// if you're debugging leaks.
BP_BOOL global_mprof_create_backtrace_on_alloc;

// global memory profiler lock
sem_t global_mprof_lock;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Memory Profiler Routines (Called Internally from bp*allocs/frees) %
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Mark an allocation if it exists.
BP_ERROR_T BP_MemProfilerMarkChunkByAddr(void * chunk, char * mark)
{



	// return indicating success
	return ERR_SUCCESS;

}

// This must be called once per application run.  It initializes
// semaphores and insures global data is in the right configuration
// before any allocations are performed.  Not calling this function
// can result in undefined behavior.
BP_ERROR_T BP_InitMemProfilerSubsystem
(
	size_t log_chunk_min_size,
	size_t diag_bpstrdup,
	size_t diag_bpstrndup,
	size_t diag_bprealloc,
	size_t diag_bpmalloc,
	size_t diag_bpcalloc,
	size_t diag_bpfree
)
{

	// only init if no init has already occured
	if(!BP_GLOBAL_MEMPROF_HASH_TABLE_INIT_OK)
			return ERR_FAILURE;


	// initialize the hash table
	BP_GLOBAL_MEMPROF_HASH_TABLE_INIT;


	// return indicating success
	return ERR_SUCCESS;

}

// disables the memory profiler subsystem and resets/frees memory.
BP_ERROR_T BP_ShutdownMemProfilerSubsystem()
{

	// only shutdown if profiler is up
	if(BP_GLOBAL_MEMPROF_HASH_TABLE_INIT_OK)
		return ERR_FAILURE;

	// destroy the hash table registry
	BP_DestroyHashTableRegistry(&global_mprof_hash_table_registry);

	// override the profiler init setting
	global_mprof_hash_table_init_ok_val = 0;

	// return indicating success
	return ERR_SUCCESS;

}



// removes a chunk from update
BP_ERROR_T BP_MemProfilerRemoveChunk(void * addr)
{

	// only shutdown if profiler is up
	if(BP_GLOBAL_MEMPROF_HASH_TABLE_INIT_OK)
		return ERR_FAILURE;

	// lock the table modification semaphore
	BP_HASH_TABLE_LOCK_SEM(global_mprof_lock);

	// delete a memory entry
	BP_ERROR_T deleted_ok = BP_HashRegDeleteMemoryEntryByDataPtr
	(
		&global_mprof_hash_table_registry,
		addr
	);

	// if it wasn't deleted ok, return indicating failure
	if(!deleted_ok)
	{
		return ERR_FAILURE;
	}

	// unlock the table modification semaphore
	BP_HASH_TABLE_UNLOCK_SEM(global_mprof_lock);


	// return indicating success
	return ERR_SUCCESS;

}

// checks to see if the chunk is in the stack
P_BP_HASH_TABLE_MEM_PTR_ENTRY BP_MemProfilerFindChunk(void *addr)
{

	// only shutdown if profiler is up
	if(BP_GLOBAL_MEMPROF_HASH_TABLE_INIT_OK)
		return NULL;

	// return found pointer (or null)
	return BP_HashRegMemPtrFind(&global_mprof_hash_table_registry,(void *) addr );

}

// displays the memprofiler
BP_ERROR_T BP_DisplayMemProfiler()
{

	// only shutdown if profiler is up
	if(!BP_GLOBAL_MEMPROF_HASH_TABLE_INIT_OK)
	{
		printf("\n[!!] Attempted to display memory profiler but global memory profiler hasn't been initialized yet.  Initialize memory profiler before utilizing this routine.");
		return ERR_FAILURE;
	}

	// display the memory profiler hash registry
	BP_HashRegDisplay(&global_mprof_hash_table_registry, BP_TRUE, BP_FALSE);

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Blockparty Custom Allocators (Hash Table version) %%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// managed strdup
char * bpstrdup_real(char *dup, BPLN_PARMS)
{



	// ensure we're not trying to duplicate a null pointer
	if(!dup)
		return NULL;

	// If the memory profiler isn't active, immediately return
	// the duplicated string.
	if(!BP_GLOBAL_MEMPROF_HASH_TABLE_INIT_OK)
		return strdup(dup);

	// lock the table modification semaphore
	BP_HASH_TABLE_LOCK_SEM(global_mprof_lock);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Managed Allocation %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// calculate the length of the string to duplicate
	size_t len = bpstrlen(dup);

	// add a memory entry ("" is 1 byte long just for null terminator)
	char * dup_string = (char *) BP_HashRegAddMemoryEntry
	(
			&global_mprof_hash_table_registry,
			BP_HASH_REG_MEM_ENTRY_TYPE_GENERIC_ALLOCATION,
			len+1,
			file_name,
			line_number,
			func
	);

	// now attempt to lookup the pointer
	P_BP_HASH_TABLE_MEM_PTR_ENTRY dup_string_hash_entry = BP_HashRegMemPtrFind
	(
			&global_mprof_hash_table_registry,
			(void *) dup_string
	);

	// if we cant lookup the hash entry, something catestrophic has occured
	if(!dup_string_hash_entry)
	{

		// exit immediately
		BP_LIBRARY_CRITICAL_DEBUG_EXIT("Unable to lookup block after allocation.  This should never happen.  Library emitting debug trap.");

	}

	// since we have the entry, set the type
	dup_string_hash_entry->alloc_type = BP_HASH_TABLE_ALLOCATOR_STRDUP;

	// copy in the string
	memcpy(dup_string, dup, len);

	// unlock the table modification semaphore
	BP_HASH_TABLE_UNLOCK_SEM(global_mprof_lock);


	// return the duplicated string
	return dup_string;

}

// managed strndup
char * bpstrndup_real(char *dup, size_t n, BPLN_PARMS)
{


	// ensure we're not trying to duplicate a null pointer
	if(!dup)
		return NULL;
	if(!n)
		return NULL;

	// If the memory profiler isn't active, immediately return
	// the duplicated string.
	if(!BP_GLOBAL_MEMPROF_HASH_TABLE_INIT_OK)
		return strndup(dup, n);

	// lock the table modification semaphore
	BP_HASH_TABLE_LOCK_SEM(global_mprof_lock);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Managed Allocation %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add a memory entry
	char * dup_string = (char *) BP_HashRegAddMemoryEntry
	(
			&global_mprof_hash_table_registry,
			BP_HASH_REG_MEM_ENTRY_TYPE_GENERIC_ALLOCATION,
			n+1,
			file_name,
			line_number,
			func
	);

	// now attempt to lookup the pointer
	P_BP_HASH_TABLE_MEM_PTR_ENTRY dup_string_hash_entry = BP_HashRegMemPtrFind
	(
			&global_mprof_hash_table_registry,
			(void *) dup_string
	);

	// if we cant lookup the hash entry, something catestrophic has occured
	if(!dup_string_hash_entry)
	{

		// exit immediately
		BP_LIBRARY_CRITICAL_DEBUG_EXIT("Unable to lookup block after allocation.  This should never happen.  Library emitting debug trap.");
		return NULL;

	}

	// since we have the entry, set the type
	dup_string_hash_entry->alloc_type = BP_HASH_TABLE_ALLOCATOR_STRNDUP;

	// copy in the string (MUST BE n, CANNOT BE ANYTHING ELSE)
	memcpy(dup_string, dup, n);


	// unlock the table modification semaphore
	BP_HASH_TABLE_UNLOCK_SEM(global_mprof_lock);

	// return the duplicated string
	return dup_string;

}

// managed realloc
void * bprealloc_real(void * addr, size_t size, BPLN_PARMS)
{

	// If the memory profiler isn't active, immediately return
	// the realloc
	if(!BP_GLOBAL_MEMPROF_HASH_TABLE_INIT_OK)
		return realloc(addr, size);


	// unlock the table modification semaphore
	BP_HASH_TABLE_LOCK_SEM(global_mprof_lock);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Managed Allocation %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// pointer to hold future mem hash entries
	P_BP_HASH_TABLE_MEM_PTR_ENTRY dup_mem_hash_entry = NULL;

	// address of the new allocation to be returned
	void * new_addr = NULL;

	// if it's a new allocation, create a new entry here
	if(!addr)
	{

		// add a new memory entry since the behavior of realloc mandates
		// that we create a new entry on a null lookup.
		new_addr = (void *) BP_HashRegAddMemoryEntry
		(
				&global_mprof_hash_table_registry,
				BP_HASH_REG_MEM_ENTRY_TYPE_GENERIC_ALLOCATION,
				size+1,
				file_name,
				line_number,
				func
		);

		// now attempt to lookup the pointer
		dup_mem_hash_entry = BP_HashRegMemPtrFind
		(
				&global_mprof_hash_table_registry,
				(void *) new_addr
		);

		// if we cant lookup the hash entry, something catestrophic has occured
		if(!dup_mem_hash_entry)
		{

			// exit immediately
			BP_LIBRARY_CRITICAL_DEBUG_EXIT("Unable to lookup block during realloc (initial alloc part).  This should never happen.  Library emitting debug trap.");

		}

		// since we have the entry, set the type
		dup_mem_hash_entry->alloc_type = BP_HASH_TABLE_ALLOCATOR_REALLOC;

	}
	else  // find and resize part of realloc()
	{

		// now attempt to lookup the pointer
		dup_mem_hash_entry = BP_HashRegMemPtrFind
		(
				&global_mprof_hash_table_registry,
				(void *) addr
		);


		// if we cant lookup the hash entry, something catestrophic has occured
		if(!dup_mem_hash_entry)
		{

			// exit immediately
			BP_LIBRARY_CRITICAL_DEBUG_EXIT("Unable to lookup block during realloc (memory resizing part).  This should never happen.  Library emitting debug trap.");

		}

		// create new addr
		new_addr = BP_HashRegResizeMemoryEntry
		(
			&global_mprof_hash_table_registry,
			addr,
			size,
			file_name,
			line_number,
			func
		);

		// -- lookup a second time to override type and also verify hash table integrity --

		// now attempt to lookup the pointer
		dup_mem_hash_entry = BP_HashRegMemPtrFind
		(
				&global_mprof_hash_table_registry,
				(void *) new_addr
		);


		// if we cant lookup the hash entry, something catestrophic has occured
		if(!dup_mem_hash_entry)
		{

			// exit immediately
			BP_LIBRARY_CRITICAL_DEBUG_EXIT("Unable to lookup block during realloc (post creation lookup part).  This should never happen.  Library emitting debug trap.");

		}

		// since we have the entry, set the type
		dup_mem_hash_entry->alloc_type = BP_HASH_TABLE_ALLOCATOR_REALLOC;

	}


	// unlock the table modification semaphore
	BP_HASH_TABLE_UNLOCK_SEM(global_mprof_lock);

	// return the resized memory
	return new_addr;

}

// managed malloc (this is here mostly as a stub in case people want to use this library
// in their own code.  This code actually uses calloc in the backend, so you can be guaranteed
// that all allocations returned are nullified).
void * bpmalloc_real(size_t size, BPLN_PARMS)
{

	// If the memory profiler isn't active, immediately return
	// the allocated memory.
	if(!BP_GLOBAL_MEMPROF_HASH_TABLE_INIT_OK)
		return malloc(size);


	// lock the table modification semaphore
	BP_HASH_TABLE_LOCK_SEM(global_mprof_lock);

	// add a memory entry
	void * new_mem = (void *) BP_HashRegAddMemoryEntry
	(
			&global_mprof_hash_table_registry,
			BP_HASH_REG_MEM_ENTRY_TYPE_GENERIC_ALLOCATION,
			size,
			file_name,
			line_number,
			func
	);


	// unlock the table modification semaphore
	BP_HASH_TABLE_UNLOCK_SEM(global_mprof_lock);

	// return the new memory
	return new_mem;

}

// managed calloc
void * bpcalloc_real(size_t size, size_t size_n, BPLN_PARMS)
{

	// If the memory profiler isn't active, immediately return
	// the allocated memory.
	if(!BP_GLOBAL_MEMPROF_HASH_TABLE_INIT_OK)
	{
		return calloc(size, size_n);
	}

	// lock the table modification semaphore
	BP_HASH_TABLE_LOCK_SEM(global_mprof_lock);

	// add a memory entry
	void * new_mem = (void *) BP_HashRegAddMemoryEntry
	(
			&global_mprof_hash_table_registry,
			BP_HASH_REG_MEM_ENTRY_TYPE_GENERIC_ALLOCATION,
			(size * size_n),
			file_name,
			line_number,
			func
	);

	// unlock the table modification semaphore
	BP_HASH_TABLE_UNLOCK_SEM(global_mprof_lock);

	// return the new memory
	return new_mem;
}

// destroy managed memory
void   bpfree_real(void * addr, BPLN_PARMS)
{

	// display null free check here
	if(!addr)
	{

		// dbg msg
		printf("\n [!!] Error: Attempting to free a null chunk.  This should never happen.  Displaying debug backtrace.");
		printf("\n - in file_name:   %s ", file_name);
		printf("\n - on line_number: %u ", line_number);
		printf("\n - in func:        %s", func);
		printf("\n");
		// create and display backtrace
		P_BP_LINUX_DEBUG_BACKTRACE debug_backtrace = BP_LinuxDebugCreateBacktrace(32);
		BP_LinuxDebugDisplayBacktrace(debug_backtrace, 1, 0, 32, BP_TRUE, BP_TRUE);
		printf("\n");

		// attempt to destroy after debug
		BP_LinuxDebugDestroyBacktrace(debug_backtrace);

		// exit
		return;

	}

	// If the memory profiler isn't active, immediately return
	// the allocated memory.
	if(!BP_GLOBAL_MEMPROF_HASH_TABLE_INIT_OK)
	{
		// free normally if not null
		free(addr);
		return;
	}

	// lock the table modification semaphore
	BP_HASH_TABLE_LOCK_SEM(global_mprof_lock);

	// delete a memory entry
	BP_ERROR_T deleted_ok = BP_HashRegDeleteMemoryEntryByDataPtr
	(
		&global_mprof_hash_table_registry,
		addr
	);

	if(!deleted_ok)
	{

		printf("\n [!!] Error: Attempted to delete addr %p from the global hreg but the operation failed.  Displaying trace.", addr);
		printf("\n - in file_name:   %s ", file_name);
		printf("\n - on line_number: %u ", line_number);
		printf("\n - in func:        %s", func);
		printf("\n");

		// create and display backtrace
		// P_BP_LINUX_DEBUG_BACKTRACE debug_backtrace = BP_LinuxDebugCreateBacktrace(32);
		// BP_LinuxDebugDisplayBacktrace(debug_backtrace, 1, 0, 32, BP_TRUE, BP_TRUE);
		// printf("\n");

		// attempt to destroy after debug
		// BP_LinuxDebugDestroyBacktrace(debug_backtrace);

	}

	// unlock the table modification semaphore
	BP_HASH_TABLE_UNLOCK_SEM(global_mprof_lock);

}

#else

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Old / Non-Hash Table Memory Profiler %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// global profiler set
sem_t global_memory_profiler_semaphore;

// flag indicating whether or not the profiler is enabled
size_t global_memory_profiler_enabled = 0;

// global memory profiler pool
P_BP_MEMPROF_ENTRY global_memprof_pool = NULL;

// global size parameter for logged chunks (chunks are only logged
// when their logging
size_t global_size_for_chunk_logging = 1024;

// number of entries in the profiler pool
size_t global_memprof_pool_n = 0;

// index to the last entry added in the pool
size_t global_memprof_last_index_added = 0;

// size of the global_memprf_pool_n object.
size_t global_memprofiler_pool_memory_usage = 0;

// these flags are set via the init routine in-parameters
// and are used to display a variety of debug messages.  Profiler
// debugging must be enabled (in BP-Main.h) in order for any
// messages at all to be displayed.
size_t global_memprofiler_diag_bpstrdup  = 0;
size_t global_memprofiler_diag_bpstrndup = 0;
size_t global_memprofiler_diag_bprealloc = 0;
size_t global_memprofiler_diag_bpmalloc  = 0;
size_t global_memprofiler_diag_bpcalloc  = 0;
size_t global_memprofiler_diag_bpfree    = 0;

// Simple OOM null allocation check for the app.  Exits if we have
// any 0x00 allocs.
size_t global_memprofiler_exit_application_on_null_allocations = 1;



// checks to see if the chunk is in the stack
P_BP_MEMPROF_ENTRY BP_MemProfilerFindChunk(void *addr)
{


	size_t n = 0;
	for(; n < global_memprof_pool_n; n++)
	{

		// get the entry at the specified location
		if(global_memprof_pool[n].mem_ptr == addr)
			return (P_BP_MEMPROF_ENTRY) &global_memprof_pool[n];

	}

	// return indicating failure
	return NULL;

}

// adds a chunk to update
BP_ERROR_T BP_MemProfilerAddUpdateProfiledChunk(void * addr, size_t new_size, BP_MEMPROF_ALLOC_TYPE type)
{

	// ensure address and new size
	if(!addr || !new_size)
		return ERR_FAILURE;

	// entry
	P_BP_MEMPROF_ENTRY entry = BP_MemProfilerFindChunk((void *) addr);
	if(!entry)
	{

		// increment the pool stack count
		global_memprof_pool_n++;
		global_memprof_pool = (P_BP_MEMPROF_ENTRY) realloc(global_memprof_pool, global_memprof_pool_n * sizeof(BP_MEMPROF_ENTRY));

		// zero out the entry
		memset((void *) &global_memprof_pool[global_memprof_pool_n-1], 0x00, sizeof(BP_MEMPROF_ENTRY));

		// set the pointer in the structure
		global_memprof_pool[global_memprof_pool_n-1].mem_ptr = addr;

		// set the init alloc size if the current size is zero
		if(global_memprof_pool[global_memprof_pool_n-1].curr_alloc_size == 0)
			global_memprof_pool[global_memprof_pool_n-1].init_alloc_size = global_memprof_pool[global_memprof_pool_n-1].curr_alloc_size;

		// set the current alloc size
		global_memprof_pool[global_memprof_pool_n-1].curr_alloc_size = new_size;
		global_memprof_pool[global_memprof_pool_n-1].total_allocations = 1;

	}
	else
	{
		printf("\n Logging existing chunk");
		// set the current alloc size
		entry->curr_alloc_size = new_size;
		entry->total_allocations++;

	}

	// return indicating success
	return ERR_SUCCESS;

}

// removes a chunk if it's been bpfree'd
// removes a chunk from update
BP_ERROR_T BP_MemProfilerRemoveChunk(void * addr)
{

#if DEBUG_MEMORY_PROFILER == 1
	printf("\n [!!] Attempting to remove monitored chunk: %p", addr);
#endif

	// cant remove what isn't there
	if(!addr)
		return ERR_FAILURE;

	// set the new entries
	P_BP_MEMPROF_ENTRY new_entries = (P_BP_MEMPROF_ENTRY) bpcalloc(1, sizeof(BP_MEMPROF_ENTRY) * (global_memprof_pool_n - 1));
	if(!new_entries)
		return ERR_FAILURE;


#if DEBUG_MEMORY_PROFILER == 1
	printf("\n [!!] Attempting to find chunk in monitored chunk list: %p", addr);
#endif

	// get chunk index
	size_t n = 0;
	for(; n < global_memprof_pool_n; n++)
	{

		if(global_memprof_pool[n].mem_ptr == addr)
		{

			#if DEBUG_MEMORY_PROFILER == 1
				printf("\n [!!] Found our chunk %p at entry index %u (struct addr: %p)", addr, n, (void *) &global_memprof_pool[n]);
			#endif

			// set offset
			size_t entries_offset   = n;
			size_t following_offset = n+1;
			size_t remaining_entries = (global_memprof_pool_n - n - 1);

			// copy in the first half of the stack
			memcpy((void *)new_entries, (void *) global_memprof_pool, sizeof(BP_MEMPROF_ENTRY) * n);

			// copy in the remaining entries
			memcpy((void *)&new_entries[n], (void *) &global_memprof_pool[n+1], sizeof(BP_MEMPROF_ENTRY) * remaining_entries);

			// set pool to the new stack
			global_memprof_pool = new_entries;

			#if DEBUG_MEMORY_PROFILER == 1
				printf("\n [!!] Chunk %p has been removed from the list.", addr);
			#endif

			// break the loop
			break;

		}

	}

	// decrement element count in the pool
	global_memprof_pool_n--;

	// return indicating success
	return ERR_SUCCESS;

}



// ====================================================
// === Allocators =====================================
// ====================================================

// string duplication
char * bpstrdup(char *dup)
{

	if(!dup)
		return ERR_FAILURE;

	if(global_memory_profiler_enabled == 0xcafebabe)
	{
		// lock global semaphor
		sem_wait(&global_memory_profiler_semaphore);
	}

	// get string length (cap out at 100mb)
	size_t string_len = strnlen(dup, 1024 * 1024 * 100);

	if(string_len >= MAXIMUM_VALUE_FOR_ANY_ALLOCATION)
	{
		printf("\n [+] Attempted to allocate more than %u (%u) bytes.  Error.", MAXIMUM_VALUE_FOR_ANY_ALLOCATION, string_len);
		if(global_memprofiler_exit_application_on_null_allocations == 1)
		{
			__asm("int3");
			exit(0);
		}
		return NULL;
	}

	// duplicate the string
	char * result = (char *) strdup(dup);

	if(!result)
	if(global_memprofiler_exit_application_on_null_allocations == 1)
	{
		printf("\n [ERROR] bpstrdup(%p): APPLICATION RAN OUT OF MEMORY WHILE TRYING TO ALLOCATE SPACE.", dup);
		__asm("int3");
		exit(0);
	}

	if(string_len >= global_size_for_chunk_logging)
	if(global_memprofiler_diag_bprealloc)
	{
		#if DEBUG_MEMORY_PROFILER == 1
			printf("\n [!!] strdup for (%p) yielded ptr %p (with calculated size: %u) ", dup, result, string_len);
		#endif
	}


	if(global_memory_profiler_enabled == 0xcafebabe)
	{

		// add the chunk to the profiler if size check passes
		if(string_len >= global_size_for_chunk_logging)
			BP_MemProfilerAddUpdateProfiledChunk(result, string_len, BP_MEMPROF_ALLOC_TYPE_STRDUP);

		// unlock global semaphore
		sem_post(&global_memory_profiler_semaphore);

	}

	return result;

}

// length restrained duplication
char * bpstrndup(char *dup, size_t n)
{

	if(n >= MAXIMUM_VALUE_FOR_ANY_ALLOCATION)
	{
		printf("\n [+] Attempted to allocate more than %u (%n) bytes.  Error.", MAXIMUM_VALUE_FOR_ANY_ALLOCATION, n);
		if(global_memprofiler_exit_application_on_null_allocations == 1)
		{
			__asm("int3");
			exit(0);
		}
		return NULL;
	}

	if(global_memory_profiler_enabled == 0xcafebabe)
	{
		// lock global semaphor
		sem_wait(&global_memory_profiler_semaphore);
	}

	char * result = (char *) strndup(dup, n);


	if(!result)
	if(global_memprofiler_exit_application_on_null_allocations == 1)
	{
		printf("\n [ERROR] bpstrndup(%p, %u): APPLICATION RAN OUT OF MEMORY WHILE TRYING TO ALLOCATE SPACE.", dup, n);
		exit(0);
	}

	if(n >= global_size_for_chunk_logging)
	if(global_memprofiler_diag_bpstrndup)
	{
		#if DEBUG_MEMORY_PROFILER == 1
			printf("\n [!!] strndup for (%p, %u) yielded ptr %p", dup, n, result);
		#endif
	}

	if(global_memory_profiler_enabled == 0xcafebabe)
	{

		// add the chunk to the profiler if size check passes
		if(n >= global_size_for_chunk_logging)
			BP_MemProfilerAddUpdateProfiledChunk(result, n, BP_MEMPROF_ALLOC_TYPE_STRNDUP);

		// unlock global semaphore
		sem_post(&global_memory_profiler_semaphore);
	}

	return result;

}

// replacement for realloc
void * bprealloc(void * addr, size_t size)
{

	if(size >= MAXIMUM_VALUE_FOR_ANY_ALLOCATION)
	{
		printf("\n [+] Attempted to allocate more than %u (%u_ bytes.  Error.", MAXIMUM_VALUE_FOR_ANY_ALLOCATION, size);
		if(global_memprofiler_exit_application_on_null_allocations == 1)
		{
			__asm("int3");
			exit(0);
		}
		return NULL;
	}


	if(global_memory_profiler_enabled == 0xcafebabe)
	{
		// lock global semaphor
		sem_wait(&global_memory_profiler_semaphore);

	}

	// allocate the result
	void * result = realloc(addr, size);


	if(!result)
	if(global_memprofiler_exit_application_on_null_allocations == 1)
	{
		printf("\n [ERROR] bprealloc(%p, %u): APPLICATION RAN OUT OF MEMORY WHILE TRYING TO ALLOCATE SPACE.", addr, size);
		exit(0);
	}


	if(size >= global_size_for_chunk_logging)
	if(global_memprofiler_diag_bprealloc)
	{
		#if DEBUG_MEMORY_PROFILER == 1
			printf("\n [!!] realloc for (%p, %u) yielded ptr %p", addr, size, result);
		#endif
	}

	if(!result)
	{
		printf("\n [!!] realloc failed for (%p, %u) yielded ptr %p", addr, size, result);
		exit(0);
	}

	if(global_memory_profiler_enabled == 0xcafebabe)
	{
		// add the chunk to the profiler if size check passes
		if(size >= global_size_for_chunk_logging)
			BP_MemProfilerAddUpdateProfiledChunk(result, size, BP_MEMPROF_ALLOC_TYPE_REALLOC);

		// unlock global semaphore
		sem_post(&global_memory_profiler_semaphore);

	}

	// return the allocated data
	return result;

}

// replacement for malloc
void * bpmalloc(size_t size)
{

	if(size >= MAXIMUM_VALUE_FOR_ANY_ALLOCATION)
	{
		printf("\n [+] Attempted to allocate more than %u (%u) bytes.  Error.", MAXIMUM_VALUE_FOR_ANY_ALLOCATION, size);
		if(global_memprofiler_exit_application_on_null_allocations == 1)
		{
			__asm("int3");
			exit(0);
		}
		return NULL;
	}

	if(global_memory_profiler_enabled == 0xcafebabe)
	{
		// lock global semaphor
		sem_wait(&global_memory_profiler_semaphore);
	}

	// allocate the result
	void * result = malloc(size);

	if(!result)
	if(global_memprofiler_exit_application_on_null_allocations == 1)
	{
		printf("\n [ERROR] bpmalloc(%u): APPLICATION RAN OUT OF MEMORY WHILE TRYING TO ALLOCATE SPACE.", size);
		exit(0);
	}


	if(size >= global_size_for_chunk_logging)
	if(global_memprofiler_diag_bpmalloc)
	{
		#if DEBUG_MEMORY_PROFILER == 1
			printf("\n [!!] malloc for (%u) yielded ptr %p", size, result);
		#endif
	}

	if(!result)
	{
		printf("\n [!!] Error: malloc failed.  Application likely ran out of memory.");
		exit(0);
	}

	if(global_memory_profiler_enabled == 0xcafebabe)
	{

		// add the chunk to the profiler if size check passes
		if(size >= global_size_for_chunk_logging)
			BP_MemProfilerAddUpdateProfiledChunk(result, size, BP_MEMPROF_ALLOC_TYPE_MALLOC);

		// unlock global semaphore
		sem_post(&global_memory_profiler_semaphore);

	}

	// return the allocated data
	return result;

}

// replacement for bpcalloc
void * bpcalloc(size_t size, size_t size_n)
{

	if((size * size_n) >= MAXIMUM_VALUE_FOR_ANY_ALLOCATION)
	{
		printf("\n [+] Attempted to allocate more than %u (%u) bytes.  Error.", MAXIMUM_VALUE_FOR_ANY_ALLOCATION, size_n);
		if(global_memprofiler_exit_application_on_null_allocations == 1)
		{
			__asm("int3");
			exit(0);
		}
		return NULL;
	}

	if(global_memory_profiler_enabled == 0xcafebabe)
	{
		// lock global semaphor
		sem_wait(&global_memory_profiler_semaphore);
	}

	// allocate the result
	void * result = calloc(size, size_n);

	if(!result)
	if(global_memprofiler_exit_application_on_null_allocations == 1)
	{
		printf("\n [ERROR] bpcalloc(%u, %u): APPLICATION RAN OUT OF MEMORY WHILE TRYING TO ALLOCATE SPACE.", size, size_n);
		exit(0);
	}


	if(size >= global_size_for_chunk_logging || size_n >= global_size_for_chunk_logging)
	if(global_memprofiler_diag_bpcalloc)
	{
		#if DEBUG_MEMORY_PROFILER == 1
			printf("\n [!!] calloc for (%u,%u) yielded ptr %p", size, size_n, result);
		#endif
	}

	if(global_memory_profiler_enabled == 0xcafebabe)
	{

		// add the chunk to the profiler if size check passes
		if(size >= global_size_for_chunk_logging)
			BP_MemProfilerAddUpdateProfiledChunk(result, size, BP_MEMPROF_ALLOC_TYPE_CALLOC);

		// unlock global semaphore
		sem_post(&global_memory_profiler_semaphore);
	}

	// return the allocated data
	return result;

}

// replacement for bpcalloc
void bpfree(void * addr)
{

	if(global_memory_profiler_enabled == 0xcafebabe)
	{
		// lock global semaphor
		sem_wait(&global_memory_profiler_semaphore);
	}

	if(global_memprofiler_diag_bpfree)
	{
		#if DEBUG_MEMORY_PROFILER == 1
			printf("\n [!!] Attempting to free %p", addr);
		#endif
	}

	// run check to ensure that the chunk is not being double free'd.  If it is being double
	// freed, throw a sigtrap so that we might debug it.
	if(global_memory_profiler_enabled == 0xcafebabe)
	if(!BP_MemProfilerFindChunk((void *) addr))
	{
		printf("\n BPMEMPROF: Attempting to free chunk that doesn't appear to be in the profiled chunk list.  Stopping application/launching debugger.");
		printf("\n\n");
		__asm("int3");
		return;
	}

	// bpfree the result
	free(addr);

	if(global_memory_profiler_enabled == 0xcafebabe)
	{

		// remove the chunk from the profiler
		BP_MemProfilerRemoveChunk(addr);

		// unlock global semaphore
		sem_post(&global_memory_profiler_semaphore);

	}

}

// This must be called once per application run.  It initializes
// semaphores and insures global data is in the right configuration
// before any allocations are performed.  Not calling this function
// can result in undefined behavior.
BP_ERROR_T BP_InitMemProfilerSubsystem
(
	size_t log_chunk_min_size,
	size_t diag_bpstrdup,
	size_t diag_bpstrndup,
	size_t diag_bprealloc,
	size_t diag_bpmalloc,
	size_t diag_bpcalloc,
	size_t diag_bpfree
)
{




#if DEBUG_MEMORY_PROFILER == 1
	printf("\n [!!] Attempting to initialize main memory profiler.");
#endif

	// initialize the memory profile semaphore
	sem_init(&global_memory_profiler_semaphore, 0, 1);

#if DEBUG_MEMORY_PROFILER == 1
	printf("\n [!!] Initialized global semaphor at %p, now waiting for lock.", &global_memory_profiler_semaphore);
#endif

	// lock global semaphor
	sem_wait(&global_memory_profiler_semaphore);


#if DEBUG_MEMORY_PROFILER == 1
	printf("\n [!!] Locked global memory profile semaphore.");
#endif

	// set global diagnostic flags from parameters
	global_memprofiler_diag_bpstrdup  = diag_bpstrdup;
	global_memprofiler_diag_bpstrndup = diag_bpstrndup;
	global_memprofiler_diag_bprealloc = diag_bprealloc;
	global_memprofiler_diag_bpmalloc  = diag_bpmalloc;
	global_memprofiler_diag_bpcalloc  = diag_bpcalloc;
	global_memprofiler_diag_bpfree    = diag_bpfree;

	// set the global profiler pool to null
	global_memprof_pool                    = NULL;
	global_memprof_pool_n                  = 0;
	global_memprof_last_index_added        = 0;
	global_memprofiler_pool_memory_usage   = 0;
	global_size_for_chunk_logging          = log_chunk_min_size;
	global_memory_profiler_enabled         = 0xcafebabe;


#if DEBUG_MEMORY_PROFILER == 1
	printf("\n [!!] Memory profiler has been loaded OK.  Now unlocking global lock.");
#endif

	// unlock global semaphore
	sem_post(&global_memory_profiler_semaphore);

#if DEBUG_MEMORY_PROFILER == 1
	printf("\n [!!] Global memory profiler lock has been unlocked.  Exiting profiler init.");
#endif

	// return indicating success
	return ERR_SUCCESS;

}

// disables the memory profiler subsystem and resets/frees memory.
BP_ERROR_T BP_ShutdownMemProfilerSubsystem()
{

#if DEBUG_MEMORY_PROFILER == 1
	printf("\n [!!] Preparing to disable memory profiler..");
#endif

	// lock global semaphor
	sem_wait(&global_memory_profiler_semaphore);

	if(global_memory_profiler_enabled != 0xcafebabe)
		return ERR_FAILURE;

	// set flag indicating capability is disabled
	global_memory_profiler_enabled = 0;
	if(global_memprof_pool_n)
	for(size_t j = 0; j < global_memprof_pool_n; j++)
	{

		// free marks if necessary
		if(global_memprof_pool[j].mark)
			free(global_memprof_pool[j].mark);

	}

	if(global_memprof_pool)
		free(global_memprof_pool);

	// reset all the global vars
	global_memprof_pool                    = NULL;
	global_memprof_pool_n                  = 0;
	global_memprof_last_index_added        = 0;
	global_memprofiler_pool_memory_usage   = 0;
	global_size_for_chunk_logging          = 0;

	// unlock global semaphore
	sem_post(&global_memory_profiler_semaphore);

#if DEBUG_MEMORY_PROFILER == 1
	printf("\n [!!] Memory profiler has been disabled.");
#endif

	// return indicating success
	return ERR_SUCCESS;

}
// Mark an allocation if it exists.
BP_ERROR_T BP_MemProfilerMarkChunkByAddr(void * chunk, char * mark)
{

	// ensure we have some parameter validation
	if(!chunk || !mark)
		return ERR_FAILURE;

#if DEBUG_MEMORY_PROFILER == 1
	printf("\n [!!] Attempting to mark chunk: %p with %s", chunk, mark);
#endif

	// look up the entry
	P_BP_MEMPROF_ENTRY entry = BP_MemProfilerFindChunk(chunk);
	if(!entry)
		return ERR_FAILURE;

	// Dont mark chunks with long strings, stupid.  You'll just exhaust
	// your memory.
	size_t mark_len = bpstrlen(mark);
	if(mark_len >= 4096)
		return ERR_FAILURE;

	// set the mark here after length validation
	entry->mark = strdup(mark);

#if DEBUG_MEMORY_PROFILER == 1
	printf("\n [!!] Marked chunk %p OK!", chunk);
#endif

	// return indicating success
	return ERR_SUCCESS;

}

#endif






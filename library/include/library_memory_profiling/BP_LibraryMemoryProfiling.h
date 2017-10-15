/*
 * BP_LibraryMemoryProfiling.h
 *
 *  Created on: Aug 26, 2013
 *      Author: root
 */

#ifndef BP_LIBRARYMEMORYPROFILING_H_
#define BP_LIBRARYMEMORYPROFILING_H_


// memory profiler allocation type
typedef enum {
	BP_MEMPROF_ALLOC_TYPE_UNSET,
	BP_MEMPROF_ALLOC_TYPE_MALLOC,
	BP_MEMPROF_ALLOC_TYPE_CALLOC,
	BP_MEMPROF_ALLOC_TYPE_STRDUP,
	BP_MEMPROF_ALLOC_TYPE_STRNDUP,
	BP_MEMPROF_ALLOC_TYPE_USER_DEFINED_ASCII,
	BP_MEMPROF_ALLOC_TYPE_REALLOC,
	BP_MEMPROF_ALLOC_TYPE_OPEN_FILEPOINTER,
	BP_MEMPROF_ALLOC_TYPE_FILE_DATA
} BP_MEMPROF_ALLOC_TYPE;


// This should be enabled at all times, the alternative is vestigial and to be removed.
#define BP_USE_HASH_TABLE_PROFILER 1

// if we're not using the hash table profiler, use the old profiler.
#if !BP_USE_HASH_TABLE_PROFILER

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Old / Non-Hash Table Memory Profiler %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// memory profiling
typedef struct __BP_MEMPROF_ENTRY {

	// pointer to memory
	void * mem_ptr;

	// allocation type
	BP_MEMPROF_ALLOC_TYPE type;

	// current allocation size
	size_t curr_alloc_size;

	// first initial size (could differ in a realloc)
	size_t init_alloc_size;

	// total number of times this has been allocated
	size_t total_allocations;

	// allocation mark (set manually for pointer monitoring)
	char * mark;

} BP_MEMPROF_ENTRY, *P_BP_MEMPROF_ENTRY;


// global profiler set
extern sem_t global_memory_profiler_semaphore;

// flag indicating whether or not the profiler is enabled
extern size_t global_memory_profiler_enabled;

// global memory profiler pool
extern P_BP_MEMPROF_ENTRY global_memprof_pool;

// global size parameter for logged chunks (chunks are only logged
// when their logging
extern size_t global_size_for_chunk_logging;

// number of entries in the profiler pool
extern size_t global_memprof_pool_n;

// index to the last entry added in the pool
extern size_t global_memprof_last_index_added;

#else // BP_USE_HASH_TABLE_PROFILER



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Global Memory Profiler Registry %%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// global hash table registry
extern BP_HASH_TABLE_REGISTRY global_mprof_hash_table_registry;

// set the global hash init value to some arbitrary static value
extern size_t global_mprof_hash_table_init_ok_val;
extern size_t global_mprof_hash_table_init_ok;

// If this value is non-zero the global memory profiler will create
// backtraces for every allocaction.  This should only be enabled
// if you're debugging leaks.
extern BP_BOOL global_mprof_create_backtrace_on_alloc;

// global memory profiler lock
extern sem_t global_mprof_lock;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Macros for Memory Profiler Registry %%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// simple macro that externs the required global memory profiler values
#define BP_GLOBAL_MEMPROF_HASH_TABLE_REGISTRY \
	extern BP_HASH_TABLE_REGISTRY global_mprof_hash_table_registry;\
	extern size_t global_mprof_hash_table_init_ok_val;             \
	extern size_t global_mprof_hash_table_init_ok;\
	extern sem_t global_mprof_lock;

// Initializes the global memory profiler hash table.
#define BP_GLOBAL_MEMPROF_HASH_TABLE_INIT \
	global_mprof_create_backtrace_on_alloc     = BP_FALSE;\
	memset(&global_mprof_hash_table_registry, 0x00, sizeof(BP_HASH_TABLE_REGISTRY));\
	global_mprof_hash_table_init_ok            = 0xFeedf00d; \
	global_mprof_hash_table_init_ok_val        = global_mprof_hash_table_init_ok;\
	global_mprof_hash_table_registry.is_global = BP_TRUE;\
	BP_HASH_TABLE_INIT_SEM(global_mprof_hash_table_registry.lock);\
	BP_HASH_TABLE_INIT_SEM(global_mprof_lock);\


// simple macro to check global memory profiler table has init'd ok
#define BP_GLOBAL_MEMPROF_HASH_TABLE_INIT_OK \
	(global_mprof_hash_table_init_ok_val == global_mprof_hash_table_init_ok) && (global_mprof_hash_table_init_ok != 0)

// Closes the global hash table.  Table will not close if it contains entries.
#define BP_GLOBAL_MEMPROF_HASH_TABLE_CLOSE \
		BP_DestroyHashTableRegistry(&global_mprof_hash_table_registry); \
		global_mprof_hash_table_init_ok_val = 0;\
		if(global_mprof_hash_table_registry.memory_table)\
		{\
			free(global_mprof_hash_table_registry.memory_table);   \
			global_mprof_hash_table_registry.memory_table = NULL;  \
		}\






// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Memory Profiler Utility Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This simple macro allows passive use with deployment by providing the ability to
// check if a pointer is valid and managed by the global allocator.  If it is not
// managed/invalid, it will return false, but only in the case that the profiler is enabled.
// if the profiler is not enabled, this macro will always return BP_TRUE which allows passive
// enabling/disabling of the profling system as a whole.
#define BP_MANAGED_PTR_VALID(ptr) \
		BP_GLOBAL_MEMPROF_HASH_TABLE_INIT_OK ? ( BP_HashRegMemPtrFind(&global_mprof_hash_table_registry,(void *) ptr ))?BP_TRUE:BP_FALSE : BP_TRUE




#endif



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Memory Profiler Routines (Called Internally from bp*allocs/frees) %
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Mark an allocation if it exists.
BP_ERROR_T BP_MemProfilerMarkChunkByAddr(void * chunk, char * mark);

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
);

// disables the memory profiler subsystem and resets/frees memory.
BP_ERROR_T BP_ShutdownMemProfilerSubsystem();

// adds a chunk to update
BP_ERROR_T BP_MemProfilerAddUpdateProfiledChunk(void * addr, size_t new_size, BP_MEMPROF_ALLOC_TYPE type);

// removes a chunk from update
BP_ERROR_T BP_MemProfilerRemoveChunk(void * addr);

// checks to see if the chunk is in the stack
P_BP_HASH_TABLE_MEM_PTR_ENTRY BP_MemProfilerFindChunk(void *addr);

// displays the memprofiler
BP_ERROR_T BP_DisplayMemProfiler();


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Blockparty Custom Allocators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// These allocators are used throughout the blockparty library, and are rigged
// to automatically report to the memory profiler when chunks are allocated
// or destroyed.  These are the actual workhorse routines for the allocator
// and are widely implemented (in the form of macros) throughout the library.

// blockparty string duplicators
char * bpstrdup_real(char *dup, BPLN_PARMS);
char * bpstrndup_real(char *dup, size_t n, BPLN_PARMS);

// blockparty reallocator
void * bprealloc_real(void * addr, size_t size, BPLN_PARMS);

// standard no-clear allocator (actually does clear)
void * bpmalloc_real(size_t size, BPLN_PARMS);

// standard clear allocator
void * bpcalloc_real(size_t size, size_t size_n, BPLN_PARMS);

// destroy managed memory
void   bpfree_real(void * addr, BPLN_PARMS);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Blockparty Allocator Macros (embed line /func info %%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// The BP allocators are macros, so that the code expansion an include line information passthrough
// from where-ever the allocation occurs, without having to specify that pass through during
// every allocation.  It's a convienence feature basically, you can use the real
// functions if you want but I'd say use the macros if at all possible.

#define bpstrdup(dup)          bpstrdup_real(dup, BPLN)
#define bpstrndup(dup, n)      bpstrndup_real(dup, n, BPLN)
#define bprealloc(addr, size)  bprealloc_real(addr, size, BPLN)
#define bpmalloc(size)         bpmalloc_real (size, BPLN)
#define bpcalloc(size, size_n) bpcalloc_real (size, size_n, BPLN)
#define bpfree(addr)           bpfree_real(addr, BPLN)

#endif /* BP_LIBRARYMEMORYPROFILING_H_ */

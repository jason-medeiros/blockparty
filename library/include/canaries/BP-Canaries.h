/*
 * BP-Canaries.h:
 *
 * The canaries subsystem exists as a globally scoped subsystem utilizing hash tables
 * to build canary lookups.  This implementation uses mmap instead of malloc to keep
 * the hash tables away from the heap.  Access members are kept in global memory so as
 * to keep them out of the heap as well.  All canary related code is implemented as a macro
 * to force inlining of the bytecode in all use cases.  This not only speeds it up, it also
 * makes tampering more difficult, as canary checks are all implemented as direct 1 to 1
 * code.
 *
 *  Created on: Dec 9, 2014
 *      Author: root
 */

#ifndef BP_CANARIES_H_
#define BP_CANARIES_H_

// canary is a basic size_t
typedef size_t canary_t;

// canary entry
typedef struct __BP_CANARY_ENTRY {

	// key
	void * key;

	// entry
	canary_t canary;

	// address of the canary
	void * addr;

	// flag indicating that the canary is in use
	BP_BOOL in_use;

	// add mmap hash table entry
	UT_hash_handle_mmap hh;

} BP_CANARY_ENTRY, *P_BP_CANARY_ENTRY;


// this is mapped directly to memory via mmap
typedef struct __BP_CANARY_ENTRY_TABLE {

	// canary table lock
	sem_t lock;

	// number of entries in the table
	size_t entries_n;

	// stack of entries
	P_BP_CANARY_ENTRY entries;

} BP_CANARY_ENTRY_TABLE, *P_BP_CANARY_ENTRY_TABLE;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Canary Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// global canary entry table
extern BP_CANARY_ENTRY_TABLE global_canary_entry_table;

// initialization ok values
extern size_t global_canary_entry_table_init_ok_val;
extern size_t global_canary_entry_table_init_ok;

// temporary entry used for lookups
extern P_BP_CANARY_ENTRY canary_tmp_entry;

// tmp lookup address for canaries
extern void * canary_tmp_lookup_addr;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Canary Semaphore Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// initializes (creates) a new semaphore
#define BP_CANARY_TABLE_INIT_SEM(sem_to_lock) sem_init(&sem_to_lock, 0, 1);

// locks a semaphore
#define BP_CANARY_TABLE_LOCK_SEM(sem_to_lock) sem_wait(&sem_to_lock);

// unlocks a semaphore
#define BP_CANARY_TABLE_UNLOCK_SEM(sem_to_unlock) sem_post(&sem_to_unlock);

// destroys a semaphore
#define BP_CANARY_TABLE_DESTROY_SEM(sem_to_destroy) sem_destroy(&sem_to_destroy);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Canary Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// includes the canary externalities required for the canary subsystem to work
#define BP_USE_CANARIES                                                    \
		extern BP_CANARY_ENTRY_TABLE global_canary_entry_table;            \
		extern size_t global_canary_entry_table_init_ok_val;               \
		extern size_t global_canary_entry_table_init_ok;

// initialize the canary subsystems
#define BP_CANARIES_INIT \
		if(global_canary_entry_table_init_ok_val != global_canary_entry_table_init_ok)                    \
		{                                                                                                 \
			memset(&global_canary_entry_table, 0x00, sizeof(global_canary_entry_table));                  \
			BP_CANARY_TABLE_INIT_SEM(global_canary_entry_table.lock);                                     \
			global_canary_entry_table_init_ok = global_canary_entry_table_init_ok_val;                    \
		}

// shutdown the canaries subsystem
#define BP_CANARIES_SHUTDOWN \
		memset(&global_canary_entry_table, 0x00, sizeof(global_canary_entry_table));\
		global_canary_entry_table_init_ok = 0;

// add a canary to the global registry
#define BP_CANARY_ADD(canary_addr)                                                                         \
		BP_CANARY_TABLE_LOCK_SEM(global_canary_entry_table.lock);                                          \
		canary_tmp_entry = (P_BP_CANARY_ENTRY) mmap_malloc(sizeof(BP_CANARY_ENTRY));                       \
		canary_tmp_entry->key = canary_addr;                                                               \
		canary_tmp_entry->canary = NEW_RANDOM_ID;                                                          \
		*canary_addr = (void *) canary_tmp_entry->canary;                                                  \
		HASH_MMAP_ADD_PTR(global_canary_entry_table.entries, key, canary_tmp_entry);                       \
		global_canary_entry_table.entries_n++;                                                             \
		BP_CANARY_TABLE_UNLOCK_SEM(global_canary_entry_table.lock);


// delete canary from the global registry
#define BP_CANARY_DEL(canary_addr)                                                                        \
		BP_CANARY_TABLE_LOCK_SEM(global_canary_entry_table.lock);                                         \
		canary_tmp_entry = NULL;                                                                          \
		canary_tmp_lookup_addr = canary_addr;                                                             \
		HASH_MMAP_FIND_PTR(global_canary_entry_table.entries,  &canary_tmp_lookup_addr, canary_tmp_entry);\
		if(canary_tmp_entry)                                                                              \
		{                                                                                                 \
			HASH_MMAP_DEL(global_canary_entry_table.entries, canary_tmp_entry);                           \
			global_canary_entry_table.entries_n--;                                                        \
			printf("\n deleted ok.");                                                                     \
		}                                                                                                 \
		else                                                                                              \
		{                                                                                                 \
			printf("\n lookup failed.");                                                                  \
		}                                                                                                 \
		BP_CANARY_TABLE_UNLOCK_SEM(global_canary_entry_table.lock);

// check if canaries match
#define BP_CANARY_CHECK(canary_addr, status_addr_out)                                                     \
		BP_CANARY_TABLE_LOCK_SEM(global_canary_entry_table.lock);                                         \
		canary_tmp_lookup_addr = canary_addr;                                                             \
		HASH_MMAP_FIND_PTR(global_canary_entry_table.entries, &canary_tmp_lookup_addr,canary_tmp_entry);  \
		*status_addr_out = BP_FALSE;                                                                      \
		if(!canary_tmp_entry)                                                                             \
		{                                                                                                 \
			*status_addr_out = BP_FALSE;                                                                  \
		}                                                                                                 \
		else                                                                                              \
		{                                                                                                 \
			if(canary_tmp_entry->canary == (canary_t)*canary_addr)                                        \
				*status_addr_out = BP_TRUE;                                                               \
			else                                                                                          \
				*status_addr_out = BP_FALSE;                                                              \
		}                                                                                                 \
		BP_CANARY_TABLE_UNLOCK_SEM(global_canary_entry_table.lock);


#endif /* BP_CANARIES_H_ */

/*
 * BP-MMapSlabAllocator.h
 *
 *  Created on: Dec 10, 2014
 *      Author: root
 */

#ifndef BP_MMAPSLABALLOCATOR_H_
#define BP_MMAPSLABALLOCATOR_H_

/*

A slab allocator is useful for allocating memory objects outside of the heap.  This
is useful for several reasons, one of which being simply not having data stored
in adjacent heap chunks.  Another is to prevent cache misses between allocated entries,
as you can have stacks of structures adjacent to one another, all being reused.  They
are best used for generating large amounts of static external arrays, such as those
required by hash buckets.  Slab allocation should not be used in lieu of malloc for
general tasks, but instead only used when performance demands dictate it.


// Diagram 1: Slab allocator visual description.
mmap
{

	 [H][E][A][D][E][R]         [\/]
	 [0] [F][R][A][G][I][D][X]   ||   S
	 [1] [F][R][A][G][I][D][X]   ||   t
	 [2] [F][R][A][G][I][D][X]   ||   a
	 [3] [F][R][A][G][I][D][X]   ||   t
	 [4] [F][R][A][G][I][D][X]  _||_  i
	 [5] [F][R][A][G][I][D][X]  \  /  c
	 [n] [F][R][A][G][I][D][X]   \/
	 [0] [E][N][T][R][Y]  [\/]
	 [1] [E][N][T][R][Y]   ||         S
	 [2] [E][N][T][R][Y]   ||         t
	 [3] [E][N][T][R][Y]   ||         a
	 [4] [E][N][T][R][Y]  _||_        t
	 [5] [E][N][T][R][Y]  \  /        i
	 [n] [E][N][T][R][Y]   \/         c

}

*/


// Slab entry info
typedef enum BP_MMAP_SLAB_ENTRY_INFO
{

	BP_MMAP_SLAB_ENTRY_INFO_NONE,             /* 0: Generic none                                                                   */
	BP_MMAP_SLAB_ENTRY_INFO_NEVER_ALLOCATED,  /* 1: If the chunk has never been allocated using the allocator, only set after init */
	BP_MMAP_SLAB_ENTRY_INFO_AVAILABLE,        /* 2: The chunk is available for allocation                                          */
	BP_MMAP_SLAB_ENTRY_INFO_IN_USE,           /* 3: The chunk is in use and not available for allocation.                          */
	BP_MMAP_SLAB_ENTRY_INFO_IS_GROUP_TOP,     /* 4: The chunk is the top of a group of chunks.                                     */
	BP_MMAP_SLAB_ENTRY_INFO_NEXT_IS_GROUPED,  /* 5: The next chunk is grouped (array) with the next chunk down.                    */
	BP_MMAP_SLAB_ENTRY_INFO_PREV_IS_GROUPED,  /* 6: The previous chunk is grouped with this chunk (array)                          */
	BP_MMAP_SLAB_ENTRY_INFO_CHUNK_MOVED,      /* 7: This chunk was moved by the defragmentation system. (debug indicator)          */
	BP_MMAP_SLAB_ENTRY_INFO_BOTTOM            /* 8: If this flag is set, this is the bottom of the map.  There is no entry which   */
	                                          /*    follows, there is only this header.  A following read will fail.               */
};


// create simple flags typedef
typedef BP_FLAGS_T BP_MMAP_SLAB_ENTRY_FLAGS;


// slab entry header (all slab entries must contain one)
typedef struct __BP_MMAP_SLAB_ENTRY_HDR {

	// flags associated with the entry
	BP_MMAP_SLAB_ENTRY_FLAGS flags;

	// identififer associated with the current entry

}BP_MMAP_SLAB_ENTRY_HDR, *P_BP_MMAP_SLAB_ENTRY_HDR;


// This structure exists as a static structure within the mmap, directly
// following the header, and is used for providing a lookup table after
// defragmentation occurs so that application which may be using old pointers
// can update their pointers from this index set.
typedef struct __BP_MMAP_SLAB_FRAGMENTATION_IDX {

	// old chunk location (group top) before fragmentation
	P_BP_MMAP_SLAB_ENTRY_HDR old_ptr;

	// new chunk location (moved group top to this location during defrag)
	P_BP_MMAP_SLAB_ENTRY_HDR new_ptr;

} BP_MMAP_SLAB_FRAGMENTATION_IDX, *P_BP_MMAP_SLAB_FRAGMENTATION_IDX;



// Slab allocator header, contains pertainent information regarding
// a managed slab and lives at the very top of the memory mapped region
// associated with slab allocator.
typedef struct __BP_MMAP_SLAB_ALLOCATOR {


	// slab allocator thread lock
	sem_t lock;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Counters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// size of each entry
	size_t entry_size;

	// number of allocated entries
	size_t entry_count;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Entries %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// top entry in the slab
	P_BP_MMAP_SLAB_ENTRY_HDR top_entry;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Map Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// the total size of the map
	size_t map_size;

	// map start address
	void * map_start;

	// map end address
	void * map_end;

	// remainder of map left over (padding for page size)
	size_t map_boundary_padding;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Fragmentation %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// start of the fragmentation index
	P_BP_MMAP_SLAB_FRAGMENTATION_IDX frag_index;

	// number of entries available in the fragmentation index
	size_t frag_entries;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// number of entries available in the allocator
	size_t entries_available;

	// number of entries used in the allocator
	size_t entries_used;

	// number of entries newly allocated
	size_t entries_still_newly_allocated;

	// pointer largest number of continuous blocks available as of last
	// informational update.
	void *largest_contiguous_blocks_available;

	// the largest number of contiguous blocks available
	size_t largest_contiguous_blocks_available_n;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Alloc Group Info %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// number of groups allocated
	size_t group_alloc_n;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Macro Move Members (DO NOT USE DIRECTLY) %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// the last index modified
	size_t last_index_modified;

	// last index which was accessed
	size_t last_index_accessed;

	// current write index
	size_t current_write_index;

	// current read index
	size_t current_read_index;

	// current entry accessed (shouldn't be used directly as this moves during operations)
	P_BP_MMAP_SLAB_ENTRY_HDR curr_entry;


} BP_MMAP_SLAB_ALLOCATOR, *P_BP_MMAP_SLAB_ALLOCATOR;



// Defragmentation information (generated locally by the routine BP_SlabAllocDefragAndUpdateFragIdx
// and destroyed before exit).  This is the only structure which is stored on the heap, and it's only
// done so temporarily unless specified by the developer to be returned (via optional parameter _out parameter)
typedef struct __BP_MMAP_SLAB_ALLOCATOR_DEFRAG_INFO {


	// this will contain a refrence to the TOP entry in a slab allocation
	P_BP_MMAP_SLAB_ENTRY_HDR group_alloc_top_ptr;

	// this is the number of chunks (including the header chunk) available
	// in the allocated group.
	size_t group_alloc_chunks_n;

	// during analysis this will be set to the number of available entries following
	// the end of a group (free space)
	size_t trailing_available_alloc_n;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Move Defragment Info %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This holds the new address of the ptr after
	P_BP_MMAP_SLAB_ENTRY_HDR group_alloc_top_ptr_moved;

	// This points to the end of the table, where the next coalesce
	// should occur.
	P_BP_MMAP_SLAB_ENTRY_HDR next_group_write_start_ptr;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Members Pertaining to First Entry %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set to true if this is the first group entry
	BP_BOOL is_first_group_entry;

	// this is set to the number of entries that precede the top entry
	size_t if_first_how_many_empty_before;


} BP_MMAP_SLAB_ALLOCATOR_DEFRAG_INFO, *P_BP_MMAP_SLAB_DEFRAG_INFO;




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Utility Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to get the top entry
#define BP_SLAB_ALLOCATOR_GET_TOP_ENTRY(allocator, top_entry_out)                                      \
	*top_entry_out = (P_BP_MMAP_SLAB_ENTRY_HDR)                                                        \
	(size_t) allocator                 +                                 /* start at the allocator */  \
    sizeof(BP_MMAP_SLAB_ALLOCATOR)     +                                 /* move past hdr */           \
	(sizeof(BP_MMAP_SLAB_FRAGMENTATION_IDX) * allocator->entry_count);   /* move past frag idx's*/


// Get reference to fragmentation index
#define BP_SLAB_ALLOCATOR_GET_FRAGMENTATION_IDX(allocator, frag_idx_out)                               \
		*frag_idx_out = (P_BP_MMAP_SLAB_FRAGMENTATION_IDX)                                             \
		((size_t) allocator + sizeof(BP_MMAP_SLAB_ALLOCATOR));


// attempt to get the next entry in an allocator
#define BP_SLAB_ALLOCATOR_GET_NEXT_ENTRY(allocator, entry_in, entry_out)                              \
		entry_out = NULL;                                                                             \
		if(allocator)                                                                                 \
		if(entry_in)                                                                                  \
		{                                                                                             \
			if(BP_FLAG_IS_SET(entry_in, BP_MMAP_SLAB_ENTRY_INFO_BOTTOM))                              \
			{                                                                                         \
				entry_out = NULL;                                                                     \
			}                                                                                         \
			else                                                                                      \
			{                                                                                         \
				entry_out = (P_BP_MMAP_SLAB_ENTRY_HDR)                                                \
				(                                                                                     \
					(size_t) entry_in + allocator->entry_size + sizeof(BP_MMAP_SLAB_ENTRY_HDR)        \
				);                                                                                    \
			}                                                                                         \
		}



// loop through all entries
#define BP_SLAB_ALLOCATOR_ITER_LOOP_ALL(allocator)                                                                                              \
		for                                                                                                                                     \
		(                                                                                                                                       \
			allocator->curr_entry = allocator->top_entry,                                                                      /* for */        \
			allocator->last_index_accessed = 0;                                                                                                 \
			!(BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_BOTTOM)) &&                                 /* condition */  \
			(allocator->curr_entry = (P_BP_MMAP_SLAB_ENTRY_HDR) &allocator->top_entry[allocator->last_index_accessed]) &&                       \
			allocator->last_index_accessed < allocator->entry_count;                                                                            \
			allocator->last_index_accessed++                                                                                   /* advance */    \
		)

// loop through all entries starting from an entry position
#define BP_SLAB_ALLOCATOR_ITER_LOOP(allocator, start_entry)                                                                                \
		for                                                                                                                                \
		(                                                                                                                                  \
			allocator->curr_entry = start_entry,                                                                           /* for */       \
			allocator->last_index_accessed = BP_SlabAllocGetEntryIDXFromEntryPtr(allocator, start_entry) ;                                 \
			!(BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_BOTTOM))   &&                           /* condition */ \
			allocator->last_index_accessed != (size_t) -1                                     &&                                           \
			(allocator->curr_entry = (P_BP_MMAP_SLAB_ENTRY_HDR) &allocator->top_entry[allocator->last_index_accessed]) &&                  \
			allocator->last_index_accessed < allocator->entry_count;                                                       /* advance */   \
			allocator->last_index_accessed++                                                                                               \
		)

// validate that the pointer is within boundaries
#define BP_SLAB_ALLOCATOR_PTR_VALID_FOR_MAP(allocator, ptr)\
		allocator->map_start < ptr || allocator->map_end > ptr



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create and Destroy Slab Allocators %%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// creates a new slab allocator
P_BP_MMAP_SLAB_ALLOCATOR BP_CreateMMapSlabAllocator(size_t entry_size, size_t entries_available);

// destroy the slab allocator
BP_ERROR_T BP_DestroyMMapSlabAllocator(P_BP_MMAP_SLAB_ALLOCATOR allocator);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Slab Semaphore Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// initializes (creates) a new semaphore
#define BP_SLAB_TABLE_INIT_SEM(sem_to_lock) sem_init(&sem_to_lock, 0, 1);

// locks a semaphore
#define BP_SLAB_TABLE_LOCK_SEM(sem_to_lock) sem_wait(&sem_to_lock);

// unlocks a semaphore
#define BP_SLAB_TABLE_UNLOCK_SEM(sem_to_unlock) sem_post(&sem_to_unlock);

// destroys a semaphore
#define BP_SLAB_TABLE_DESTROY_SEM(sem_to_destroy) sem_destroy(&sem_to_destroy);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Allocators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to allocate a set of entries.
P_BP_MMAP_SLAB_ENTRY_HDR BP_SlabAlloc(P_BP_MMAP_SLAB_ALLOCATOR allocator, size_t num_entries);

// Attempts to free a slab entry.
BP_ERROR_T BP_SlabFree(P_BP_MMAP_SLAB_ALLOCATOR allocator, P_BP_MMAP_SLAB_ENTRY_HDR entry_ptr);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Utility Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// updates the information in an allocator
BP_ERROR_T BP_SlabAllocUpdateInfo(P_BP_MMAP_SLAB_ALLOCATOR allocator, BP_BOOL skip_locks = BP_FALSE);

// returns the number of adjacent entries
size_t BP_SlabAllocAvailableAdjacentEntries(P_BP_MMAP_SLAB_ALLOCATOR allocator, P_BP_MMAP_SLAB_ENTRY_HDR start_entry);

// attempt to checkout entries
BP_ERROR_T BP_SlabAllocCheckoutEntries(P_BP_MMAP_SLAB_ALLOCATOR allocator, P_BP_MMAP_SLAB_ENTRY_HDR start_entry, size_t entry_n);

// Search the allocated pointers and try to find the correct index for the referenced lookup_entry pointer.  This function
// returns (size_t) -1 on failed lookup.
size_t BP_SlabAllocGetEntryIDXFromEntryPtr(P_BP_MMAP_SLAB_ALLOCATOR allocator, P_BP_MMAP_SLAB_ENTRY_HDR lookup_entry);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Defragmentation Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// this routine attempts to defragment the slab allocator and coalesce individual
// chunks.  The defrag_info_out parameter can be passed in as a local pointer value,
// which will be filled in if provided.  If it is provided for use, the responsibility
// will fall on the user to call free(defrag_info_out) to release the chunk.  If
// a NULL value is provided for defrag_info_out, the memory will be free'd automatically
// within the routine.  This mechanism is simply to provide heuristic possibilities
// for analysis on the DEFRAG INFO structure.
BP_ERROR_T BP_SlabAllocDefragAndUpdateFragIdx(P_BP_MMAP_SLAB_ALLOCATOR allocator, P_BP_MMAP_SLAB_DEFRAG_INFO * defrag_info_out = NULL);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// displays a slab allocator
BP_ERROR_T BP_SlabAllocatorDisplay(P_BP_MMAP_SLAB_ALLOCATOR allocator, BP_BOOL display_chunks);

// Displays mmap slab defrag information produced by BP_SlabAllocDefragAndUpdateFragIdx().  Primarily used for
// debugging but can be used to arbitrarily visualize fragmentation data.
BP_ERROR_T BP_SlabAllocatorDisplayFragInfo(P_BP_MMAP_SLAB_DEFRAG_INFO defrag_info);

#endif /* BP_MMAPSLABALLOCATOR_H_ */

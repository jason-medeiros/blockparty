/*
 * BP-MMapSlabAllocator.cc
 *
 *  Created on: Dec 10, 2014
 *      Author: root
 */

// blockparty main include file
#include "../include/BP-Main.h"


// creates a new slab allocator
P_BP_MMAP_SLAB_ALLOCATOR BP_CreateMMapSlabAllocator(size_t entry_size, size_t entries_available)
{

	// ensure we have an entry
	if(!entry_size)
		return NULL;

	// calculate total allocation size
	size_t total_alloc_size =
			sizeof( BP_MMAP_SLAB_ALLOCATOR )                               +  /* slab allocator header              */
			( sizeof(BP_MMAP_SLAB_FRAGMENTATION_IDX) * entries_available ) +  /* fragmentation table                */
			( entry_size * entries_available )                             +  /* actual entries in the table        */
			sizeof(BP_MMAP_SLAB_ENTRY_HDR);                                   /* the trailing "bottom" header entry */

	// ensure that the table isn't too big
	if( total_alloc_size > MAXIMUM_VALUE_FOR_ANY_ALLOCATION)
		return NULL;

	// increase allocation size to page boundary
	for(; total_alloc_size % PAGE_SIZE; total_alloc_size++){}


	// About Chosen Mapping Flags:
	// =============================================================
	// MAP_PRIVATE:   Private map for process.
	//
	// MAP_ANONYMOUS: Map without a file, just anonymous memory.
	//
	// MAP_LOCKED:    Allocate using locked memory.  Prevents paging this memory.  Ensures that
	//                access happen as fast as literally possible.

	// create new allocator structure
	P_BP_MMAP_SLAB_ALLOCATOR allocator = (P_BP_MMAP_SLAB_ALLOCATOR) mmap
	(
		NULL,
		total_alloc_size,
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS | MAP_LOCKED,
		-1,
		0
	);

	// ensure we could create the page
	if(allocator == (void *) -1)
		return NULL;

	// zero out the memory
	memset(allocator, 0x00, total_alloc_size);

	// initialize the semaphore
	BP_SLAB_TABLE_INIT_SEM(allocator->lock);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Map Information after MMAP %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the map start
	allocator->map_start = (void *) allocator;

	// set the map end
	allocator->map_end = (void *) ((size_t) allocator->map_start + total_alloc_size);

	// set the total map size
	allocator->map_size = total_alloc_size;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Entry Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// number of entries available
	allocator->entries_available = entries_available;

	// set entry size
	allocator->entry_size = entry_size;

	// entry count
	allocator->entry_count = entries_available;

	// set entries availble to match count
	allocator->entries_available = entries_available;


	// set the top entry
	BP_SLAB_ALLOCATOR_GET_TOP_ENTRY(allocator, &allocator->top_entry);

	// set the index ptr
	BP_SLAB_ALLOCATOR_GET_FRAGMENTATION_IDX (allocator, &allocator->frag_index);

	// void * top_entry = (P_BP_MMAP_SLAB_ENTRY_HDR) allocator + sizeof(BP_MMAP_SLAB_ALLOCATOR) + (sizeof(BP_MMAP_SLAB_FRAGMENTATION_IDX) * allocator->entry_count);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Initial Contiguous Settings to ALL Entries %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the largest contiguous as the default maximum
	allocator->largest_contiguous_blocks_available   = allocator->top_entry;
	allocator->largest_contiguous_blocks_available_n = entries_available;

	// init our header to zero
	P_BP_MMAP_SLAB_ENTRY_HDR slab_entry_hdr_addr = NULL;


	// Init all blocks (don't use macros here, as we want to init all blocks, whereas
	// the iter macros stop when they encounter the bottom)
	size_t n = 0;
	for
	(
		;
		n < allocator->entry_count;
		n++
	)
	{

		// get the address of the new slab header
		slab_entry_hdr_addr = (P_BP_MMAP_SLAB_ENTRY_HDR)(size_t) &allocator->top_entry[n];

		// ensure we have an address
		if(!slab_entry_hdr_addr)
			break;

		if(!(BP_SLAB_ALLOCATOR_PTR_VALID_FOR_MAP(allocator, slab_entry_hdr_addr)))
		{
			printf("\n Invalid address detected at start of chunk.");
			break;
		}

		if(!(BP_SLAB_ALLOCATOR_PTR_VALID_FOR_MAP(allocator, slab_entry_hdr_addr+entry_size)))
		{
			printf("\n Invalid address detected at end of chunk.  Likely sigsegv.");
			break;
		}


		// set all to never-allocated status
		BP_SET_FLAG(slab_entry_hdr_addr->flags, BP_MMAP_SLAB_ENTRY_INFO_NEVER_ALLOCATED);

		// set available flag
		BP_SET_FLAG(slab_entry_hdr_addr->flags, BP_MMAP_SLAB_ENTRY_INFO_AVAILABLE);

	}


	// set the bottom entry
	if(n)
	{

		// get the address of the new slab header
		slab_entry_hdr_addr = (P_BP_MMAP_SLAB_ENTRY_HDR)(size_t) &allocator->top_entry[n];

		// set the flag
		BP_SET_FLAG(slab_entry_hdr_addr->flags, BP_MMAP_SLAB_ENTRY_INFO_BOTTOM);

	}


	// run update before returning
	BP_SlabAllocUpdateInfo(allocator);

	// return the allocator
	return allocator;

}


// destroy the slab allocator
BP_ERROR_T BP_DestroyMMapSlabAllocator(P_BP_MMAP_SLAB_ALLOCATOR allocator)
{

	// ensure we have an allocator
	if(!allocator)
		return ERR_FAILURE;

	// unmap the allocator
	if(munmap(allocator, allocator->map_size) == -1)
		return ERR_FAILURE;

	// return indiating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Allocators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempts to allocate a set of slabs.
P_BP_MMAP_SLAB_ENTRY_HDR BP_SlabAlloc(P_BP_MMAP_SLAB_ALLOCATOR allocator, size_t num_entries)
{

	// ensure we have an allocator
	if(!allocator)
		return NULL;

	// ensure we have a number of entries to allocate
	if(!num_entries)
		return NULL;

	// lock the allocator before display
	BP_SLAB_TABLE_LOCK_SEM(allocator->lock);

	// update allocation info
	BP_SlabAllocUpdateInfo(allocator, BP_TRUE);

	// ensure we have space for allocations
	if(num_entries > allocator->largest_contiguous_blocks_available_n)
	{
		BP_SLAB_TABLE_UNLOCK_SEM(allocator->lock);
		return NULL;
	}

	// set tmp entry
	P_BP_MMAP_SLAB_ENTRY_HDR tmp_entry = NULL;

	// the entry to be returned (aka start entry)
	P_BP_MMAP_SLAB_ENTRY_HDR ret_entry = NULL;

	// loop iterator
	size_t available_adjacent_n = 0;

	// walk through the allocator and find contiguous set of slabs
	BP_SLAB_ALLOCATOR_ITER_LOOP_ALL(allocator)
	{

		// if the current slab has never been allocated, indicate that here
		if
		(
			BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_AVAILABLE)
		)
		{

			// count the number of adjacent items
			available_adjacent_n = BP_SlabAllocAvailableAdjacentEntries
			(
					allocator,
					allocator->curr_entry
			);

			// available adjacent number of entries
			if(available_adjacent_n >= num_entries)
			{

				printf("\n [+] Preparing for allocation (%u - %u)", available_adjacent_n, num_entries);


				// set the return entry
				ret_entry = allocator->curr_entry;

				// attempt to check out the chunks
				BP_SlabAllocCheckoutEntries
				(
					allocator,
					allocator->curr_entry,
					num_entries
				);

				// reset the current entry pointer
				allocator->curr_entry = ret_entry;

				// break after allocation
				break;

			}

		}

		// reset the adjacent item count
		available_adjacent_n = 0;

	}

	// unlock allocator before exit
	BP_SLAB_TABLE_UNLOCK_SEM(allocator->lock);

	// update allocation info before exit
	BP_SlabAllocUpdateInfo(allocator, BP_TRUE);

	// return the allocation
	return ret_entry;

}



// Attempts to free a slab entry.
BP_ERROR_T BP_SlabFree(P_BP_MMAP_SLAB_ALLOCATOR allocator, P_BP_MMAP_SLAB_ENTRY_HDR entry_ptr)
{

	// ensure we have an allocator
	if(!allocator)
		return ERR_FAILURE;
	if(!entry_ptr)
		return ERR_FAILURE;

	// lock the allocator before use
	BP_SLAB_TABLE_LOCK_SEM(allocator->lock);

	// First check to make sure that the entry pointer
	// is the top of a group.
	if(!BP_FLAG_IS_SET(entry_ptr->flags, BP_MMAP_SLAB_ENTRY_INFO_IS_GROUP_TOP))
	{

		// unlock the allocator
		BP_SLAB_TABLE_UNLOCK_SEM(allocator->lock);

		// return indicating failure
		return ERR_FAILURE;

	}


	// update allocation info
	BP_SlabAllocUpdateInfo(allocator, BP_TRUE);

	// set next to false
	BP_BOOL has_next = BP_FALSE;


	//
	P_BP_MMAP_SLAB_ENTRY_HDR last_hdr = NULL;

	// walk the entries and free (aka unmark/mark) entries
	BP_SLAB_ALLOCATOR_ITER_LOOP(allocator, entry_ptr)
	{

		// check flags are set
		if(BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_NEXT_IS_GROUPED))
		{

			// printf("\n Chunk has next?");
			has_next = BP_TRUE;

		}

		if(!last_hdr)
		{
			last_hdr = allocator->curr_entry;
		}
		else
		{

			if(last_hdr == allocator->curr_entry)
				printf("\n DUPLICATE CHUNKS DETECTED DURING FREE, ERROR");

		}

		// zero out flags first
		allocator->curr_entry->flags = BP_MMAP_SLAB_ENTRY_INFO_NONE;

		// after the chunk is updated set the flag indicating that the chunk is available for use
		BP_SET_FLAG(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_AVAILABLE);

		// break if there is no next
		if(!has_next)
			break;

		// overide flag
		has_next = BP_FALSE;

		last_hdr = allocator->curr_entry;

	}


	// unlock allocator before exit
	BP_SLAB_TABLE_UNLOCK_SEM(allocator->lock);

	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Utility Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// updates the information in an allocator
BP_ERROR_T BP_SlabAllocUpdateInfo(P_BP_MMAP_SLAB_ALLOCATOR allocator, BP_BOOL skip_locks)
{

	// ensure we have an allocator
	if(!allocator)
		return ERR_FAILURE;

	// lock the allocator before display
	if(skip_locks == BP_FALSE)
		BP_SLAB_TABLE_LOCK_SEM(allocator->lock);

	// ensure we have an entry count, if not, exit
	if(!allocator->entry_count)
	{

		if(skip_locks == BP_FALSE)
			BP_SLAB_TABLE_UNLOCK_SEM(allocator->lock);

		// return indicating failure
		return ERR_FAILURE;

	}

	// local variables used for calculations
	size_t entries_available                     = 0;
	size_t entries_used                          = 0;
	size_t entries_still_newly_allocated         = 0;

	// pointer to the largest set of available blocks
	P_BP_MMAP_SLAB_ENTRY_HDR largest_contiguous_blocks_available    = NULL;
	size_t largest_contiguous_blocks_available_n                    = 0;

	// prev entry data
	P_BP_MMAP_SLAB_ENTRY_HDR prev_available_slab_entry                  = NULL;
	size_t                   prev_available_slab_entry_adjacent_entry_n = 0;

	// last entry data
	P_BP_MMAP_SLAB_ENTRY_HDR last_available_slab_entry                  = NULL;
	size_t                   last_available_slab_entry_adjacent_entry_n = 0;

	// number of group entries
	size_t group_top_entries_n = 0;


	// Iterate through all items in the allocator and run checks on each to determine
	// the status of our allocator.
	BP_SLAB_ALLOCATOR_ITER_LOOP_ALL(allocator)
	{

		// count the number of group top entries
		if(BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_IS_GROUP_TOP))
			group_top_entries_n++;

		// count the number of entries used
		if(BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_IN_USE))
			entries_used++;

		// if the current slab has never been allocated, indicate that here
		if(BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_NEVER_ALLOCATED))
			entries_still_newly_allocated++;

		// check if flag is available
		if(BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_AVAILABLE))
		{

			// if the last available slab entry isn't set, set it
			if(!last_available_slab_entry)
			{

				last_available_slab_entry = allocator->curr_entry;
				last_available_slab_entry_adjacent_entry_n = 1;

			}
			else
			{

				// increase the available adjacent count
				last_available_slab_entry_adjacent_entry_n++;

			}

			entries_available++;

		}
		else
		{
			prev_available_slab_entry = last_available_slab_entry;
			prev_available_slab_entry_adjacent_entry_n = last_available_slab_entry_adjacent_entry_n;
		}

		// if the previous largest has less entries than the current set of available entries, then
		// set the current set of entries as the largest.
		if(prev_available_slab_entry_adjacent_entry_n < last_available_slab_entry_adjacent_entry_n)
		{

			// set count
			largest_contiguous_blocks_available_n = last_available_slab_entry_adjacent_entry_n;

			// set entry
			largest_contiguous_blocks_available = last_available_slab_entry;

		}

	}

	// set updated information in the allocator
	allocator->entries_available                     = entries_available;
	allocator->entries_used                          = entries_used;
	allocator->entries_still_newly_allocated         = entries_still_newly_allocated;
	allocator->largest_contiguous_blocks_available   = largest_contiguous_blocks_available;
	allocator->largest_contiguous_blocks_available_n = largest_contiguous_blocks_available_n;
	allocator->group_alloc_n                         = group_top_entries_n;

	// unlock allocator before exit
	if(skip_locks == BP_FALSE)
		BP_SLAB_TABLE_UNLOCK_SEM(allocator->lock);

	// return indicating success
	return ERR_SUCCESS;

}

// returns the number of adjacent entries
size_t BP_SlabAllocAvailableAdjacentEntries(P_BP_MMAP_SLAB_ALLOCATOR allocator, P_BP_MMAP_SLAB_ENTRY_HDR start_entry)
{

	if(!allocator)
		return 0;
	if(!start_entry)
		return 0;

	// preserve the old current entry as the loop iter modifies it and
	// we may be calling this from a routine that uses it.
	P_BP_MMAP_SLAB_ENTRY_HDR old_current = allocator->curr_entry;

	// loop iterator
	size_t available_n = 0;

	// loop entries
	BP_SLAB_ALLOCATOR_ITER_LOOP(allocator, start_entry)
	{

		if(BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_AVAILABLE))
		{
			available_n++;
		}
		else
		{
			break;
		}

	}

	// restore the old
	allocator->curr_entry = old_current;

	// return the count of adjacent available entries
	return available_n;

}


// attempt to checkout entries.  This should only ever be called via BP_SlabAlloc.  SlabAlloc
// locks the allocator by itself, which prevents the need for this routine to lock anything.  That
// said, don't use this routine unsafely w/o locks if you implement it in your own code.
BP_ERROR_T BP_SlabAllocCheckoutEntries
(
	P_BP_MMAP_SLAB_ALLOCATOR allocator,
	P_BP_MMAP_SLAB_ENTRY_HDR start_entry,
	size_t                   entry_n
)
{


	// ensure we have an allocator
	if(!allocator)
		return ERR_FAILURE;
	if(!start_entry)
		return ERR_FAILURE;
	if(!entry_n)
		return ERR_FAILURE;

	// set if this is the first entry
	BP_BOOL first_entry = BP_FALSE;

	// number of chunks marked for checkout
	size_t total_marked_n = 0;

	// iterate from the start entry
	BP_SLAB_ALLOCATOR_ITER_LOOP(allocator, start_entry)
	{

		// the allocator entry must be available for checkout
		if(!BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_AVAILABLE))
			return ERR_FAILURE;

		// remove any lingering flags
		allocator->curr_entry->flags = BP_MMAP_SLAB_ENTRY_INFO_NONE;

		// mark the entry in use
		BP_SET_FLAG(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_IN_USE);

		// mark the first entry as top if it's the top entry
		if(first_entry == BP_FALSE)
		{

			// set the flag
			first_entry = BP_TRUE;

			// since this is the first entry, mark as group top
			BP_SET_FLAG(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_IS_GROUP_TOP);

			 // set next group flag if necessary
			if(entry_n > 1)
			{
				BP_SET_FLAG(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_NEXT_IS_GROUPED);
			}

		}
		else
		{

			// mark entry as in use
			BP_SET_FLAG(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_IN_USE);

			// mark previous as grouped
			BP_SET_FLAG(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_PREV_IS_GROUPED);

			// exit if we hit our cap
			if((total_marked_n+1) < entry_n)
			{
				BP_SET_FLAG(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_NEXT_IS_GROUPED);
			}


		}


		// increment the total number of marked items
		total_marked_n++;

		// exit if we hit our cap
		if(total_marked_n == entry_n)
			break;

	}

	// return indicating success
	return ERR_SUCCESS;

}

// Search the allocated pointers and try to find the correct index for the referenced lookup_entry pointer.
size_t BP_SlabAllocGetEntryIDXFromEntryPtr(P_BP_MMAP_SLAB_ALLOCATOR allocator, P_BP_MMAP_SLAB_ENTRY_HDR lookup_entry)
{

	// ensure we have an allocator and a lookup
	if(!allocator)
		return (size_t) -1;
	if(!lookup_entry)
		return (size_t) -1;

	// set the iterator
	size_t n = 0;

	BP_SLAB_ALLOCATOR_ITER_LOOP_ALL(allocator)
	{

		// return the index if we found it
		if(allocator->curr_entry == lookup_entry)
			return n;

		// increase the index counter and continue the loop if not
		n++;

	}

	// return erroneously if unable to lookup data
	return (size_t) -1;

}




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Defragmentation Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// this routine attempts to defragment the slab allocator and coalesce individual
// chunks
BP_ERROR_T BP_SlabAllocDefragAndUpdateFragIdx
(
		P_BP_MMAP_SLAB_ALLOCATOR allocator,
		P_BP_MMAP_SLAB_DEFRAG_INFO * defrag_info_out
)
{

	// ensure we have an allocator
	if(!allocator)
		return ERR_FAILURE;

	// there is nothing to coalesce if there are no available entries
	if(!allocator->entries_available)
		return ERR_FAILURE;

	// there is nothing to coalese if there are no entries
	if(!allocator->entry_count)
		return ERR_FAILURE;

	// there is nothing to coalesce if there are no used entries
	if(!allocator->entries_used)
		return ERR_FAILURE;

	// update allocation info
	BP_SlabAllocUpdateInfo(allocator, BP_TRUE);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Allocate Defrag Info Structures on Heap %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Allocate space with a trailing entry.  The trailing entry is set for iterative purposes
	// as a trunctating element.
	P_BP_MMAP_SLAB_DEFRAG_INFO defrag_info_entries = (P_BP_MMAP_SLAB_DEFRAG_INFO) calloc
	(
			sizeof(BP_MMAP_SLAB_ALLOCATOR_DEFRAG_INFO) * (allocator->group_alloc_n + 1),
			1
	);

	// ensure we have entries allocated
	if(!defrag_info_entries)
		return ERR_FAILURE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Loop 1: Calculate Move Positions %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this holds the number of entries which were passed till we found
	// a group.
	size_t entries_till_first_group_hdr_n = 0;

	// iterative counter
	size_t entry_iter_n       = 0;
	size_t group_top_iter_n   = 0;
	size_t group_entry_iter_n = 0;


	// Indicator that a group top was found
	BP_BOOL group_top_was_found = BP_FALSE;

	// Indicator for whether or not the next entry in the list is grouped with the
	// current group top header.
	BP_BOOL next_is_grouped = BP_FALSE;

	// flag to indicat whether or not entries are trailing free allocs
	BP_BOOL curr_is_trailing_free_space = BP_FALSE;

	// number of empty chunks found before first group entry
	size_t empty_chunks_before_first_group_n = 0;



	// Loop through all entries, updating the allocator with relevant data on each loop count.
	BP_SLAB_ALLOCATOR_ITER_LOOP_ALL(allocator)
	{


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Keep Track of Empties %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// ensure we have a group top
		if(!group_top_was_found)
		{
			if(BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_AVAILABLE))
			{
				empty_chunks_before_first_group_n++;
				continue;
			}
		}


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Perform Counts %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


		if(BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_IS_GROUP_TOP))
		{

			// increment group top iter on detection of group top iteratoro
			group_top_iter_n++;

			// set the defrag top pointer
			defrag_info_entries[group_top_iter_n-1].group_alloc_top_ptr = allocator->curr_entry;

			// if the next is part of a group, indicate so here
			if(BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_NEXT_IS_GROUPED))
				next_is_grouped = BP_TRUE;

			// display group top message
			printf("\n [+] - %u - Group Top %p", entry_iter_n, defrag_info_entries[group_top_iter_n-1].group_alloc_top_ptr);

			// increase the allocated chunk count
			defrag_info_entries[group_top_iter_n-1].group_alloc_chunks_n++;

			// ensure you have a group top
			if(!group_top_was_found)
			{

				// mark as first
				defrag_info_entries[group_top_iter_n-1].is_first_group_entry = BP_TRUE;

				// set the empty chunks entry
				defrag_info_entries[group_top_iter_n-1].if_first_how_many_empty_before = empty_chunks_before_first_group_n;

				// set the group top indicator toggle
				group_top_was_found=BP_TRUE;

			}

		}
		else
		{


			if(BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_NEXT_IS_GROUPED))
			{


				// verbosity
				printf("\n - %u - Advancing group iter!", entry_iter_n);

				// increase the allocated chunk count
				defrag_info_entries[group_top_iter_n-1].group_alloc_chunks_n++;

			}
			else if(BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_PREV_IS_GROUPED))
			{

				// verbosity
				printf("\n - %u - Encountered final group entry!", entry_iter_n);

				// increase the allocated chunk count
				defrag_info_entries[group_top_iter_n-1].group_alloc_chunks_n++;
			}
			else
			{

				if(group_top_was_found == BP_TRUE)
					defrag_info_entries[group_top_iter_n-1].trailing_available_alloc_n++;

				// verbosity
				printf("\n - %u - Advancing available alloc iter.", entry_iter_n);


				// remove the presumption that the next entry is grouped
				next_is_grouped = BP_FALSE;

			}

		}


		// if a group top was not found, go ahead and update here
		if(!group_top_was_found)
			entries_till_first_group_hdr_n++;

		// increment the iterator
		entry_iter_n++;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Now Coalesce Freespace Based On Gathered Info %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// declare iterator for tracking info entry indexes
	size_t n = 0;

	// this is the iterator used for marking cleared chunks as available
	size_t j = 0;

	// this is used to determine the subtractive distance between moved chunks, and
	// allows
	size_t adjusted_upward_move_value = 0;

	// this after the initial move (first top entry) this will be set to the
	// value of the next write position.
	P_BP_MMAP_SLAB_ENTRY_HDR next_copy_location = NULL;

	// Enter loop only if a group top was found.  Loop through all entries and push entries up
	// through existing freespace, keeping track of new freespace created during these pushes, so
	// as to provide a record for iteratively pushing distances dynamically.
	if(group_top_was_found && allocator->group_alloc_n)
	for
	(
		;
		defrag_info_entries[n].group_alloc_top_ptr && (n < allocator->group_alloc_n);
		n++
	)
	{

		// if this is the FIRST group allocated in the slab, we need to
		// examine the struct.if_first_how_many_empty_before member and
		// push (aka move) the items up.
		if(defrag_info_entries[n].is_first_group_entry)
		{

			// only move data if it's actually being moved
			if( allocator->top_entry != defrag_info_entries[n].group_alloc_top_ptr)
			{

				// verbosity
				printf("\n [+] First chunk is moving.");

				// copy in information
				memcpy
				(
					(void *) allocator->top_entry,
					defrag_info_entries[n].group_alloc_top_ptr,
					allocator->entry_size * defrag_info_entries[n].group_alloc_chunks_n
				);

				// mark all cleared chunks as available
				for(j=0; j < defrag_info_entries[n].group_alloc_chunks_n; j++)
				{
					defrag_info_entries[n].group_alloc_top_ptr[j].flags = 0;
					BP_SET_FLAG(defrag_info_entries[n].group_alloc_top_ptr[j].flags, BP_MMAP_SLAB_ENTRY_INFO_AVAILABLE);
					BP_SET_FLAG(defrag_info_entries[n].group_alloc_top_ptr[j].flags, BP_MMAP_SLAB_ENTRY_INFO_CHUNK_MOVED);
				}

			}
			else
			{
				// verbosity
				printf("\n [+] First chunk move not moving, only updating the copy location.");
			}

			// set the next copy location as the bottom of the moved chunks
			next_copy_location = &allocator->top_entry
			[
				 defrag_info_entries[n].group_alloc_chunks_n
			];

			// save the next copy location
			defrag_info_entries[n].next_group_write_start_ptr = next_copy_location;

		}
		else
		{


			if(next_copy_location == defrag_info_entries[n].group_alloc_top_ptr)
			{

				// verbosity
				printf("\n [+] Secondary chunk move not moving, only updating the copy location.");

				// set the next copy location
				next_copy_location = &defrag_info_entries[n].group_alloc_top_ptr
				[
					 defrag_info_entries[n].group_alloc_chunks_n
				];

				// save the next copy location
				defrag_info_entries[n].next_group_write_start_ptr = next_copy_location;

			}
			else
			{

				// verbosity
				printf("\n [+] Secondary chunk is moving to new location");

				// set the movement indicator
				defrag_info_entries[n].group_alloc_top_ptr_moved = next_copy_location;

				// copy in information
				memcpy
				(
					(void *) next_copy_location,
					defrag_info_entries[n].group_alloc_top_ptr,
					allocator->entry_size * defrag_info_entries[n].group_alloc_chunks_n
				);

				// mark all cleared chunks as available
				for(j=0; j < defrag_info_entries[n].group_alloc_chunks_n; j++)
				{
					defrag_info_entries[n].group_alloc_top_ptr[j].flags = 0;
					BP_SET_FLAG(defrag_info_entries[n].group_alloc_top_ptr[j].flags, BP_MMAP_SLAB_ENTRY_INFO_AVAILABLE);
					BP_SET_FLAG(defrag_info_entries[n].group_alloc_top_ptr[j].flags, BP_MMAP_SLAB_ENTRY_INFO_CHUNK_MOVED);
				}


				// set the next copy location
				next_copy_location = &defrag_info_entries[n].group_alloc_top_ptr
				[
					 defrag_info_entries[n].group_alloc_chunks_n
				];

				// save the next copy location
				defrag_info_entries[n].next_group_write_start_ptr = next_copy_location;

			}

		}

	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Exit Routine %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure that the group top was found
	if(!group_top_was_found)
		return ERR_FAILURE;

	// If the dev has requested info out, provide it, if not just destroy the
	// data before exiting.
	if(!defrag_info_out)
		free(defrag_info_entries);
	else
		*defrag_info_out = defrag_info_entries;

	// return indicating success
	return ERR_SUCCESS;

}





// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// displays a slab allocator
BP_ERROR_T BP_SlabAllocatorDisplay(P_BP_MMAP_SLAB_ALLOCATOR allocator, BP_BOOL display_chunks)
{

	// ensure we have an allocator
	if(!allocator)
		return ERR_FAILURE;

	// update the information
	BP_SlabAllocUpdateInfo(allocator);

	// lock the allocator before display
	BP_SLAB_TABLE_LOCK_SEM(allocator->lock);

	// display slab allocator header info
	printf("\n [+] Slab Allocator %p", allocator);
	printf("\n\t lock:                                  %p",       &allocator->lock);
	printf("\n\t entry_size:                            %u bytes", allocator->entry_size);
	printf("\n\t entry_count:                           %u",       allocator->entry_count);
	printf("\n\t map_size:                              %u bytes (%u x PAGE_SIZE)", allocator->map_size, allocator->map_size / PAGE_SIZE);
	printf("\n\t top_entry:                             %p",       allocator->top_entry);
	printf("\n\t frag_index:                            %p",       allocator->frag_index);
	printf("\n\t entries_available:                     %u",       allocator->entries_available);
	printf("\n\t entries_used:                          %u",       allocator->entries_used);
	printf("\n\t entries_still_newly_allocated:         %u",       allocator->entries_still_newly_allocated);
	printf("\n\t largest_contiguous_blocks_available:   %p",       allocator->largest_contiguous_blocks_available);
	printf("\n\t largest_contiguous_blocks_available_n: %u",       allocator->largest_contiguous_blocks_available_n);
	printf("\n\t group_alloc_n:                         %u",       allocator->group_alloc_n);
	printf("\n\t last_index_modified:                   %u",       allocator->last_index_modified);
	printf("\n");



	// used for iterating group index
	size_t group_idx_n = 0;

	// display chunks if desired
	if(display_chunks)
	{

		// set top entry
		P_BP_MMAP_SLAB_ENTRY_HDR tmp_entry = allocator->top_entry;

		// iterator
		size_t n = 0;

		// walk entry and display items
		BP_SLAB_ALLOCATOR_ITER_LOOP_ALL(allocator)
		{

			// display entry
			if(BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_IS_GROUP_TOP))
			{
				group_idx_n = 0;
				printf("\n\t\t [TOP] Slab Entry - %u - %p (group idx: %u)", n, allocator->curr_entry, group_idx_n);
				group_idx_n++;
			}
			else
			{

				if
				(
					BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_NEXT_IS_GROUPED) ||
					BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_PREV_IS_GROUPED)
				)
				{
					printf("\n\t\t [-] Slab Entry - %u - %p (group idx: %u)", n, allocator->curr_entry, group_idx_n);
					group_idx_n++;
				}
				else
				{
					printf("\n\t\t [-] Slab Entry - %u - %p", n, allocator->curr_entry);
				}


			}

			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Display Individual Flags %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if(BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_NEVER_ALLOCATED))
				printf("\n\t\t\t - BP_MMAP_SLAB_ENTRY_INFO_NEVER_ALLOCATED");

			if(BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_AVAILABLE))
				printf("\n\t\t\t - BP_MMAP_SLAB_ENTRY_INFO_AVAILABLE");

			if(BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_CHUNK_MOVED))
				printf("\n\t\t\t - BP_MMAP_SLAB_ENTRY_INFO_CHUNK_MOVED");

			if(BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_IN_USE))
				printf("\n\t\t\t - BP_MMAP_SLAB_ENTRY_INFO_IN_USE");

			if(BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_IS_GROUP_TOP))
				printf("\n\t\t\t - BP_MMAP_SLAB_ENTRY_INFO_IS_GROUP_TOP");

			if(BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_NEXT_IS_GROUPED))
				printf("\n\t\t\t - BP_MMAP_SLAB_ENTRY_INFO_NEXT_IS_GROUPED");

			if(BP_FLAG_IS_SET(allocator->curr_entry->flags, BP_MMAP_SLAB_ENTRY_INFO_PREV_IS_GROUPED))
				printf("\n\t\t\t - BP_MMAP_SLAB_ENTRY_INFO_PREV_IS_GROUPED");

			// add newline
			printf("\n");

			// increment display index
			n++;

		}


	}

	// unlock the allocator after display
	BP_SLAB_TABLE_UNLOCK_SEM(allocator->lock);

	// return indicating success
	return ERR_SUCCESS;

}


// Displays mmap slab defrag information produced by BP_SlabAllocDefragAndUpdateFragIdx().  Primarily used for
// debugging but can be used to arbitrarily visualize fragmentation data.
BP_ERROR_T BP_SlabAllocatorDisplayFragInfo(P_BP_MMAP_SLAB_DEFRAG_INFO defrag_info)
{


	// ensure we have a defrag info structure
	if(!defrag_info)
		return ERR_FAILURE;

	// iterator
	size_t n = 0;
	for
	(
		;
		defrag_info[n].group_alloc_top_ptr;
		n++
	)
	{

		// display reference
		printf("\n [+] Defrag Group Info: %p", &defrag_info[n]);

		// display first information if set
		if(defrag_info[n].is_first_group_entry == BP_TRUE)
		{

			printf("\n\t is_first_group_entry:           BP_TRUE");
			printf("\n\t if_first_how_many_empty_before: %u", defrag_info[n].if_first_how_many_empty_before);

		}

		printf("\n\t group_alloc_top_ptr:            %p", defrag_info[n].group_alloc_top_ptr);
		printf("\n\t group_alloc_top_ptr_moved:      %p", defrag_info[n].group_alloc_top_ptr_moved);
		printf("\n\t group_alloc_chunks_n:           %u", defrag_info[n].group_alloc_chunks_n);
		printf("\n\t trailing_available_alloc_n:     %u", defrag_info[n].trailing_available_alloc_n);
		printf("\n\t next_group_write_start_ptr:     %p", defrag_info[n].next_group_write_start_ptr);


	}

	// display trailing newline
	printf("\n");

	// return indicating success
	return ERR_SUCCESS;

}

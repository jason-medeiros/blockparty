/*
 * BP_MemoryGarbageCollector.cc
 *
 *  Created on: Dec 1, 2011
 *      Author: root
 */

#include "../../include/BP-Main.h"


// Creates a new and ready to use garbage collector structure.  Use the returned
// structure as input for garbage collection routines below.
P_BP_GARBAGE_COLLECTOR_DATA BP_CreateNewGarbageCollector(char * description, size_t chunk_monitoring_enabled)
{
	// allocate new collector structure
	P_BP_GARBAGE_COLLECTOR_DATA new_collector = (P_BP_GARBAGE_COLLECTOR_DATA) bpcalloc(sizeof(BP_GARBAGE_COLLECTOR_DATA), 1);

	// set description if set
	if(description)
	{

		// copy in description up to limit
		snprintf(new_collector->collector_description, 256, "%s", description);

	}

	// if set for monitoring, set the toggle
	if(chunk_monitoring_enabled)
	{
		new_collector->monitor_chunks=1;
	}

	// return the new collector
	return new_collector;

}

// bpreallocate pointer
void * BP_realloc(P_BP_GARBAGE_COLLECTOR_DATA gc, char *msg,  void * pointer, size_t alloc_size)
{

	// allocate new pointer
	void * new_ptr = (void *) bprealloc((void *) pointer, alloc_size);

	// try to search for the pointer in the collector data set
	P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK chunk = BP_GarbageCollectorLookupPtrPosition(gc, pointer);

	// if we can't find the chunk, log it
	if(!chunk)
	{

		// log the garbage collector chunk (add list item)
		BP_GarbageCollectorLogChunk(gc, NULL, new_ptr, alloc_size, BP_GARBAGE_COLLECTOR_DATA_ALLOC_REALLOC);

	}
	else // if we can find the chunk, update chunk values
	{

		chunk->chunk_ptr  = new_ptr;
		chunk->chunk_size = alloc_size;

	}


	// return the new pointer
	return new_ptr;

}

// memory allocation
void * BP_malloc(P_BP_GARBAGE_COLLECTOR_DATA gc, char *msg, size_t alloc_size)
{

	// allocate new pointer
	void * new_ptr = (void *) bpmalloc(alloc_size);

	// log the garbage collector chunk (add list item)
	BP_GarbageCollectorLogChunk(gc, NULL, new_ptr, alloc_size, BP_GARBAGE_COLLECTOR_DATA_ALLOC_REALLOC);

	// return new pointer
	return new_ptr;

}

// blockparty garbage collection version of clear alloc
void * BP_calloc(P_BP_GARBAGE_COLLECTOR_DATA gc, char *msg, size_t alloc_size, size_t clr_size)
{

	// allocate new pointer
	void * new_ptr = (void *) bpcalloc(alloc_size, clr_size);

	// log the garbage collector chunk (add list item)
	BP_GarbageCollectorLogChunk(gc, NULL, new_ptr, alloc_size, BP_GARBAGE_COLLECTOR_DATA_ALLOC_REALLOC);

	// return the new chunk
	return new_ptr;

}

// garbage collection friendly version of bpbpstrdup
char * BP_strdup(P_BP_GARBAGE_COLLECTOR_DATA gc, char *msg, char *str)
{

	// allocate new pointer
	char * new_ptr = (char *) bpstrdup(str);

	// log the garbage collector chunk (add list item)
	BP_GarbageCollectorLogChunk(gc, NULL, new_ptr, bpstrlen(new_ptr), BP_GARBAGE_COLLECTOR_DATA_ALLOC_REALLOC);

	// return pointer
	return new_ptr;

}

// garbage collection friendly version of bpbpstrdup
char * BP_strndup(P_BP_GARBAGE_COLLECTOR_DATA gc, char *msg, char *str, size_t str_len)
{

	// allocate new pointer
	char * new_ptr = (char *) bpstrndup(str, str_len);

	// log the garbage collector chunk (add list item)
	BP_GarbageCollectorLogChunk(gc, NULL, new_ptr, str_len, BP_GARBAGE_COLLECTOR_DATA_ALLOC_STRNDUP);

	// return the new pointer
	return new_ptr;

}

// block party garbage collector bpfree
size_t BP_free(P_BP_GARBAGE_COLLECTOR_DATA gc, char *msg, void *ptr)
{

	// chunk poisition for lookup
	size_t chunk_position = 0;

	// first lookup chunk
	P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK lookup_chunk = BP_GarbageCollectorLookupPtrPosition(gc, ptr);

	// exit if there is no chunk to bpfree
	if(!lookup_chunk)
	{
		return ERR_FAILURE;
	}
	else
	{

		// unlink the chunk from the list
		if(BP_GarbageCollectorUnlinkHeapChunk(gc, lookup_chunk) == ERR_FAILURE)
		{
			printf("\n Could not unlink chunk: %p", lookup_chunk);
			BP_GargbageCollectorPrintChunk(lookup_chunk);

		}
		else
		{


			// printf("\n Heap unlinked OK!!!");

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// unlinks an element from the list
size_t BP_GarbageCollectorUnlinkHeapChunk(P_BP_GARBAGE_COLLECTOR_DATA gc, P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK chunk)
{

	// requires a garbage collection structure
	if(!gc)
	{
		printf("\n Err. No gc.");
		return ERR_FAILURE;
	}
	if(!chunk)
	{
		printf("\n Err. Got null chunk.");
		return ERR_FAILURE;
	}
	if(!gc->chunks)
	{
		printf("\n Error got null chunks in gc..");
		return ERR_FAILURE;
	}

	// set chunks pointer
	P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK chunks_list = gc->chunks->first;
	if(!chunks_list)
	{
		printf("\n Err. No first pointer.");
		return ERR_FAILURE;
	}

	// walk chunks list and verify our chunk exists in the list
	for(; chunks_list; chunks_list = chunks_list->next)
	{

		// if we found the chunk in the list, unlink it
		if(chunks_list == chunk )
		{

			// knowledge values for conditional branching
			size_t is_first = 0;
			size_t has_prev = 0;
			size_t has_next = 0;
			size_t is_middle_chunk  = 0;
			size_t is_last_chunk = 0;
			size_t chunk_has_been_freed = 0;

			// check if its a middle chunk
			if( (chunk != chunk->first) && (chunk->next != NULL) && (chunk->prev != NULL) )
				is_middle_chunk = 1;

			// check if its the last chunk
			if(chunk == BP_GarbageCollectorGetLastChunk(gc))
				is_last_chunk = 1;

			// set flag for first
			if(chunk == chunk->first)
				is_first = 1;
			if(chunk->next != NULL)
				has_next = 1;
			if(chunk->prev != NULL)
				has_prev = 1;

			// if its the first element, but has a next pointer, rework the list
			if(is_first && has_next)
			{

				// printf("\n Unlinking first w/ next");
				chunk = chunk->next;

				// destroy data pointer
				if(chunk->prev->chunk_ptr)
					bpfree(chunk->prev->chunk_ptr);

				// nullify after destruction
				chunk->prev->chunk_ptr = NULL;

				// destroy chunk itself
				bpfree(chunk->prev);
				chunk->prev  = NULL;
				chunk->first = chunk;

				// set chunks to new first element
				gc->chunks   = chunk;

				// reset all the first list pointers
				BP_GarbageCollectorResetAllFirstListPtrs(gc->chunks, chunk);

				// final reset to first element
				gc->chunks   = chunk;
				gc->total_chunks_freed_n++;

				// chunk has been unlinked
				return ERR_SUCCESS;

			}

			// if its the first, and has no next
			if(is_first && !has_next)
			{
				// increament the total number of chunks bpfreed
				// printf("\n Unlinking first w/o next");
				if(chunk->chunk_ptr)
					bpfree(chunk->chunk_ptr);

				chunk->chunk_ptr = NULL;

				if(chunk)
					bpfree(chunk);

				gc->chunks   = NULL;
				gc->total_chunks_freed_n++;
				return ERR_SUCCESS;
			}

			// if the chunk is a middle chunk, handle it here
			if(is_middle_chunk && !is_first && !is_last_chunk)
			{
				// printf("\n Unlinking middle chunk.");

				// link up chunk
				chunk->next->prev = chunk->prev;
				chunk->prev->next = chunk->next;

				// destroy chunk
				bpfree(chunk->chunk_ptr);
				bpfree(chunk);

				// return success
				gc->total_chunks_freed_n++;
				return ERR_SUCCESS;
			}

			// if its the last chunk and has a previous
			if(is_last_chunk && has_prev)
			{

				// printf("\n Unlinking last chunk w/ previous.");
				chunk->prev->next = NULL;
				bpfree(chunk->chunk_ptr);
				bpfree(chunk);
				gc->total_chunks_freed_n++;
				return ERR_SUCCESS;
			}

			// if it has no previous
			if(is_last_chunk && !has_prev)
			{
				// printf("\n Unlinking last chunk w/o previous. SINGLE.");
				bpfree(chunk->chunk_ptr);
				bpfree(chunk);
				gc->chunks = NULL;
				gc->total_chunks_freed_n++;
				return ERR_SUCCESS;
			}

			printf("\n\n FATAL ERROR: Cant bpfree this (stuck/corrupted)");
			BP_GargbageCollectorPrintChunk(chunk);
			printf("\n\n");
			asm("int3");

		}

	}

	// return indicating success
	return ERR_FAILURE;

}

// reset all first pointers in a list
size_t BP_GarbageCollectorResetAllFirstListPtrs(P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK chunk_list, P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK new_first)
{

	if(!chunk_list)
		return ERR_FAILURE;

	// reset chunk list first pointer
	chunk_list = chunk_list->first;
	for(; chunk_list; chunk_list = chunk_list->next)
	{

		// set new first pointer in list
		chunk_list->first = new_first;

	}

	// return indicating success
	return ERR_SUCCESS;

}

// destroys all unfreed chunks used by the garbage collector
size_t BP_GarbageCollectorFreeAllChunks(P_BP_GARBAGE_COLLECTOR_DATA gc)
{

	// ensure we have a valid incomming value
	if(!gc)
		return ERR_FAILURE;

	// printf("\n Freeing remainder of the chunks here.");

	// check for chunks pointer
	if(!gc->chunks)
		return ERR_FAILURE;

	// printf("\n --Function entry gc->chunks->first()---");
	// BP_GargbageCollectorPrintChunk(gc->chunks->first);
	// if we have a chunks pointer
	P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK chunks = gc->chunks->first;

	// check to see that we have a chunks pointer
	if(chunks)
	for(; chunks; chunks = gc->chunks->first)
	{

		if(!chunks)
			break;

		// bpfree the chunk pointer
		if(BP_free(gc, NULL, chunks->chunk_ptr) == ERR_FAILURE)
			break;

		// reset chunks pointer
		if(!gc->chunks)
		{
			break;
		}
	}

	// forcibly set chunk value
	gc->chunks = NULL;

	// printf("\n Exiting chunk destroyer loop");
	// return
	return ERR_SUCCESS;

}

// ======== Chunk Logger ===========================

void BP_GarbageCollectorLogChunk(P_BP_GARBAGE_COLLECTOR_DATA gc, char *msg, void *ptr, size_t alloc_size, BP_GARBAGE_COLLECTOR_DATA_ALLOC_TYPE type)
{

	// requires a garbage collector structure and pointer
	if(!gc | !ptr)
		return;

	// if no chunks, allocate fresh chunks list
	if(!gc->chunks)
	{
		gc->chunks = BP_GarbageCollectorCreateChunk(ptr, alloc_size, NULL, NULL, NULL);
		gc->chunks->type = type;
		gc->chunk_count++;
	}
	else
	{

		// isolate the last chunk
		P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK last_chunk = BP_GarbageCollectorGetLastChunk(gc);
		// printf("\n Got last chunk?: %p", last_chunk);

		// create the new chunk element
		P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK tmp_chunk = BP_GarbageCollectorCreateChunk(ptr, alloc_size, last_chunk->first, last_chunk, (P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK) NULL);
		tmp_chunk->type = type;

		// set the chunk in the list
		last_chunk->next = tmp_chunk;


		// increment the chunk count
		gc->chunk_count++;

	}

	// exit
	return;

}

// creates a new heap chunk list element
P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK BP_GarbageCollectorCreateChunk(void *ptr, size_t alloc_size, P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK first, P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK prev,P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK next)
{

	// allocate new space for chunk
	P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK chunk = (P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK) bpcalloc(sizeof(BP_GARBAGE_COLLECTOR_ALLOC_CHUNK), 1);

	// set pointer and size in tracker
	chunk->chunk_ptr = ptr;
	chunk->chunk_size = alloc_size;

	// set list pointers
	chunk->first = first;
	if(first == NULL)
		chunk->first = chunk;

	chunk->prev  = prev;
	chunk->next  = next;

	// return new chunk
	return chunk;

}

// gets the last chunk in the garbage collectors list
P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK BP_GarbageCollectorGetLastChunk(P_BP_GARBAGE_COLLECTOR_DATA gc)
{

	// return null if there is no garbage collection
	if(!gc)
		return NULL;
	if(!gc->chunks)
		return NULL;


	// use first pointer as a walker
	P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK chunks = gc->chunks->first;

	// walk the list elements
	for(; chunks; chunks = chunks->next)
	{
		if(!chunks->next)
			break;
	}


	// return the last pointer
	return chunks;
}


// returns the heap chunk index, searched by pointer
P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK BP_GarbageCollectorLookupPtrPosition(P_BP_GARBAGE_COLLECTOR_DATA gc, void *ptr)
{

	if(!gc)
		return NULL;
	if(!gc->chunks)
		return NULL;

	// linear search chunk stack looking for pointer
	size_t i = 0;

	// save first pointer
	P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK chunks = gc->chunks->first;


	// run linear search
	for(chunks = chunks->first; chunks; chunks = chunks->next)
	{
		// return the pointer in the case of a match
		if(chunks->chunk_ptr == ptr)
			return chunks;
	}

	// return indicating failure
	return NULL;

}

// ======= Display Routines =======================

// displays brief data
void BP_GarbageCollectorDisplayBrief(P_BP_GARBAGE_COLLECTOR_DATA gc)
{
	if(!gc)
	{
		printf("\n [!!] cannot display NULL garbage collector");
		return;
	}

	printf("\n [+] Garbage Collector: %p", gc);
	printf("\n    Collector Description: %s", gc->collector_description);
	printf("\n    Total Allocations:          %u", gc->total_allocated_chunks_n);
	printf("\n    Total Freed Allocs:         %u", gc->total_chunks_freed_n);
	printf("\n    Number of Monitored Chunks: %u", gc->chunk_count);
	printf("\n    Pointer to Chunks:          %p", gc->chunks);
}

// prints a garbage collector list
size_t BP_GarbageCollectorPrintChunkList(P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK chunks)
{

	if(!chunks)
	{
		printf("\n NULL chunks list.");
		return ERR_FAILURE;
	}
	if(!chunks->first)
	{
		printf("\n No first chunks pointer");
		return ERR_FAILURE;
	}

	P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK chunk = chunks->first;


	// walk each chunk and print out elements
	for(chunk = chunk->first; chunk; chunk = chunk->next)
	{
		// display the chunk
		BP_GargbageCollectorPrintChunk(chunk);

	}
	return ERR_SUCCESS;
}

// displays a chunk in the chunk list
void BP_GargbageCollectorPrintChunk(P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK chunk)
{

	printf("\n [+] GC Chunk : %p", chunk);
	printf("\n  ptr:  %p", chunk->chunk_ptr);
	printf("\n  size: %u", chunk->chunk_size);
	printf("\n     list first: %p", chunk->first);
	printf("\n     list prev:  %p", chunk->prev);
	printf("\n     list next:  %p", chunk->next);

	return;
}

// displays extended garbage collector data
void BP_GarbageCollectorDisplayExtended(P_BP_GARBAGE_COLLECTOR_DATA gc)
{

}
// displays all chunks, in order of appearance
void BP_GarbageCollectorDisplayChunksAll(P_BP_GARBAGE_COLLECTOR_DATA gc, size_t unfreed_only)
{
	if(!gc)
	{
		printf("\n Error: Cannot display chunks from empty garbage (NULL) collector.");
		return;
	}

}

// displays individual chunks by type
void BP_GarbageCollectorDisplayChunksByType(P_BP_GARBAGE_COLLECTOR_DATA gc, size_t unfreed_only, BP_GARBAGE_COLLECTOR_DATA_ALLOC_TYPE chunk_type_to_display)
{
	if(!gc)
	{
		printf("\n [+] Error: Cannot display chunks from empty garbage (NULL) collector.");
		return;
	}
	if(!gc->chunks)
	{
		printf("\n [+] Error: attempting to display an empty gc->chunks object.");
		return;
	}

	// walk chunks and display only chunks of specified type
	P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK chunks = gc->chunks->first;

	// walk / display chunks
	for(chunks = chunks->first; chunks; chunks = chunks->next)
	{

		// display individual chunk
		BP_GarbageCollectorDisplayChunk(chunks);

	}

}

// displays a single chunk entry
void BP_GarbageCollectorDisplayChunk(P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK chunk)
{
	if(!chunk)
	{
		printf("\n [!!] Cannot display null chunk.");
		return;
	}

	printf("\n [+] Chunk: %p", chunk);
	printf("\n   pointer: %p", chunk->chunk_ptr);
	printf("\n   bpfreed:   %s", chunk->chunk_has_been_freed ? "yes" : "no");
	printf("\n   dbg msg: %s", chunk->dbg_message);
	switch(chunk->type)
	{
		case BP_GARBAGE_COLLECTOR_DATA_ALLOC_UNKNOWN:
			printf("\n   type: unset type");
			break;
		case BP_GARBAGE_COLLECTOR_DATA_ALLOC_CALLOC:
			printf("\n   type: bpcalloc");
			break;
		case BP_GARBAGE_COLLECTOR_DATA_ALLOC_MALLOC:
			printf("\n   type: bpmalloc");
			break;
		case BP_GARBAGE_COLLECTOR_DATA_ALLOC_REALLOC:
			printf("\n   type: bprealloc");
			break;
		case BP_GARBAGE_COLLECTOR_DATA_ALLOC_STRDUP:
			printf("\n   type: bpbpstrdup");
			break;
		case BP_GARBAGE_COLLECTOR_DATA_ALLOC_STRNDUP:
			printf("\n   type: bpstrndup");
			break;
		default:
			printf("\n   type: NO TYPE MATCHED, ERROR!");
			break;
	}

}


// ====== Destructor ==============================

// Destroys a garbage collector structure (does not bpfree individual chunk pointers but
// does destroy chunk structures.  Make sure chunks are bpfreed before this is call or they
// could go floating.
size_t BP_DestroyGarbageCollector(P_BP_GARBAGE_COLLECTOR_DATA gc)
{

	// check for garbage collector
	if(!gc)
	{
		return ERR_FAILURE;
	}

	// destroy bpreallocated chunks list
	size_t i = 0;
	if(gc->chunks)
	for
	(
			gc->chunks = gc->chunks->first;
			gc->chunks;
	)
	{
		if(gc->chunks->next)
		{
			gc->chunks = gc->chunks->next;
			bpfree(gc->chunks->prev);
			gc->chunks->prev = NULL;
		}
		else
		{
			bpfree(gc->chunks);
			gc->chunks = NULL;
			break;
		}
	}

	// now destroy the full structure
	bpfree(gc);

	// return indicating success
	return ERR_SUCCESS;

}


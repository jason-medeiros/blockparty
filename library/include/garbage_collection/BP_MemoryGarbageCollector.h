/*
 * BP_MemoryGarbageCollector.h
 *
 *  Created on: Dec 1, 2011
 *      Author: root
 */

#ifndef BP_MEMORYGARBAGECOLLECTOR_H_
#define BP_MEMORYGARBAGECOLLECTOR_H_

// enumerator to describe types for garbage collector
typedef enum BP_GARBAGE_COLLECTOR_DATA_ALLOC_TYPE {
	BP_GARBAGE_COLLECTOR_DATA_ALLOC_UNKNOWN,
	BP_GARBAGE_COLLECTOR_DATA_ALLOC_CALLOC,
	BP_GARBAGE_COLLECTOR_DATA_ALLOC_MALLOC,
	BP_GARBAGE_COLLECTOR_DATA_ALLOC_REALLOC,
	BP_GARBAGE_COLLECTOR_DATA_ALLOC_STRDUP,
	BP_GARBAGE_COLLECTOR_DATA_ALLOC_STRNDUP
};

// Garbage collector tracking structure for individual chunks.
typedef struct _BP_GARBAGE_COLLECTOR_ALLOC_CHUNK {

	// pointer to allocated memory
	void *chunk_ptr;

	// current size of chunk
	size_t chunk_size;

	// allocation type malloc/calloc/realloc/etc
	BP_GARBAGE_COLLECTOR_DATA_ALLOC_TYPE type;

	// flag indicating if this chunk has been freed
	size_t chunk_has_been_freed;

	// debug message regarding chunk
	char dbg_message[32];

	// list pointers
	struct _BP_GARBAGE_COLLECTOR_ALLOC_CHUNK * first;
	struct _BP_GARBAGE_COLLECTOR_ALLOC_CHUNK * prev;
	struct _BP_GARBAGE_COLLECTOR_ALLOC_CHUNK * next;

} BP_GARBAGE_COLLECTOR_ALLOC_CHUNK, *P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK;

// garbage collector chunk managment structure
typedef struct _BP_GARBAGE_COLLECTOR_DATA {

	// description of the current collector data set
	char collector_description[512];

	// flag to indicate whether or not to log the allocation and destruction
	// of chunks.  Set to zero to disable, set > 0 to enable.
	size_t monitor_chunks;

	// linked list of pointers chunks which were allocated (in order) (changed from realloc as lists are faster in this case)
	P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK  chunks;
	size_t chunk_count;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Statistics %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// total number of chunks allocated
	size_t total_allocated_chunks_n;

	// total number of chunks freed
	size_t total_chunks_freed_n;

} BP_GARBAGE_COLLECTOR_DATA, *P_BP_GARBAGE_COLLECTOR_DATA;

// Creates a new and ready to use garbage collector structure.  Use the returned
// structure as input for garbage collection routines below.
P_BP_GARBAGE_COLLECTOR_DATA BP_CreateNewGarbageCollector(char * description, size_t chunk_monitoring_enabled);

// garbage collection friendly realloc.
void * BP_realloc(P_BP_GARBAGE_COLLECTOR_DATA gc, char *msg, void * pointer, size_t alloc_size);

// garbage collection friendly version of calloc.
void * BP_calloc(P_BP_GARBAGE_COLLECTOR_DATA gc, char *msg, size_t alloc_size, size_t clr_size);

// garbage collection friendly version of malloc.
void * BP_malloc(P_BP_GARBAGE_COLLECTOR_DATA gc, char *msg, size_t alloc_size);

// garbage collection friendly version of strdup
char * BP_strdup(P_BP_GARBAGE_COLLECTOR_DATA gc, char *msg, char *str);

// garbage collection friendly version of strdup
char * BP_strndup(P_BP_GARBAGE_COLLECTOR_DATA gc, char *msg, char *str, size_t str_len);


// block party garbage collector free
size_t BP_free(P_BP_GARBAGE_COLLECTOR_DATA gc, char *msg, void *ptr);

// ======== Chunk Logger ===========================

// log a heap chunk
// void BP_GarbageCollectorLogChunk(P_BP_GARBAGE_COLLECTOR_DATA gc, char *msg, void *ptr, size_t alloc_size);
void BP_GarbageCollectorLogChunk(P_BP_GARBAGE_COLLECTOR_DATA gc, char *msg, void *ptr, size_t alloc_size,  BP_GARBAGE_COLLECTOR_DATA_ALLOC_TYPE type);

// ======= Display Routines =======================

// destroys all unfreed chunks used by the garbage collector
size_t BP_GarbageCollectorFreeAllChunks(P_BP_GARBAGE_COLLECTOR_DATA gc);

// displays brief data
void BP_GarbageCollectorDisplayBrief(P_BP_GARBAGE_COLLECTOR_DATA gc);

// displays extended garbage collector data
void BP_GarbageCollectorDisplayExtended(P_BP_GARBAGE_COLLECTOR_DATA gc);

// displays individual chunks by type
void BP_GarbageCollectorDisplayChunksByType(P_BP_GARBAGE_COLLECTOR_DATA gc, size_t unfreed_only, BP_GARBAGE_COLLECTOR_DATA_ALLOC_TYPE chunk_type_to_display);

// displays all chunks, in order of appearance
void BP_GarbageCollectorDisplayChunksAll(P_BP_GARBAGE_COLLECTOR_DATA gc, size_t unfreed_only);

// displays a single chunk entry
void BP_GarbageCollectorDisplayChunk(P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK chunk);


//======== Lookup Routines ========================

// returns the heap chunk index, searched by pointer.  Returns 0 for error.  1 or above, for index.  index[0] is position one.
P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK BP_GarbageCollectorLookupPtrPosition(P_BP_GARBAGE_COLLECTOR_DATA gc, void *ptr);

// ====== Destructor ==============================

// Destroys a garbage collector structure (does not free individual chunk pointers but
// does destroy chunk structures.  Make sure chunks are freed before this is call or they
// could go floating.
size_t BP_DestroyGarbageCollector(P_BP_GARBAGE_COLLECTOR_DATA gc);

// creates a new heap chunk list element
P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK BP_GarbageCollectorCreateChunk(void *ptr, size_t alloc_size, P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK first, P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK prev,P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK next);

// gets the last chunk in the garbage collectors list
P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK BP_GarbageCollectorGetLastChunk(P_BP_GARBAGE_COLLECTOR_DATA gc);

// unlinks an element from the list
size_t BP_GarbageCollectorUnlinkHeapChunk(P_BP_GARBAGE_COLLECTOR_DATA gc, P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK chunk);

// reset all first pointers in a list
size_t BP_GarbageCollectorResetAllFirstListPtrs(P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK chunk_list, P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK new_first);

// prints a garbage collector list
size_t BP_GarbageCollectorPrintChunkList(P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK chunks);

// displays a chunk in the chunk list
void BP_GargbageCollectorPrintChunk(P_BP_GARBAGE_COLLECTOR_ALLOC_CHUNK chunk);

#endif /* BP_MEMORYGARBAGECOLLECTOR_H_ */

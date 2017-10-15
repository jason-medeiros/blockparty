/*
 * BP_LinkedList.h
 *
 *  Created on: Aug 27, 2013
 *      Author: root
 */

#ifndef __BP_LINKED_LIST_H
#define __BP_LINKED_LIST_H



// include linux queue functionality
#include <sys/queue.h>



// linked list entries (TAILQ_ENTRY)
typedef struct __BP_LINKL_DATA {


	// data which was allocated
	void *data;

	// the size of the data allocated
	size_t data_size;

	// the type of data allocated
	BP_MEMPROF_ALLOC_TYPE data_type;

	// debugging information (should be empty in production builds)
	char * file;
	size_t line_number;
	char * function;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% LinkL List Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add tail entry
	TAILQ_ENTRY(__BP_LINKL_DATA) linkl_data;


} BP_LINKL_DATA, *P_BP_LINKL_DATA;


// head entry (TAILQ_HEAD)
typedef struct __BP_LINKL {

	// this semaphore will be locked before any list
	// operations.
	sem_t thread_safe_semaphore;
	size_t thread_safe_enabled;

	// description of initialized list
	char description[256];

	// random uuid assigned to this linkl
	char *uuid;

	// total data size in list elements
	size_t total_size;

	// total number of allocations
	size_t total_allocations;

	// ==== Free Monitoring (for double free detection) =============

	// these are filled in dynamically
	void ** freed_ptrs;
	size_t  freed_ptrs_n;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% LinkL Flattening %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// flag indicating whether or not the linkl has been flattened
	// out.
	size_t linkl_has_been_flattened;

	// this is the flattened buffer
	unsigned char * flat_buff;

	// size of the flattened buffer
	size_t flat_buff_n;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% LinkL List Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set linked list head data
	TAILQ_HEAD(,__BP_LINKL_DATA) head;


} BP_LINKL, *P_BP_LINKL;


// Tailqueue registry structure, used for holding registered tail queue allocators.  This
// can be useful for storing multiple registered tailqueues within a larger body of code.
typedef struct __BP_LINKL_REGISTRY {

	// semaphore used when threads attempt to modify the global registry
	sem_t lock;

	// a stack of pointers containing all globally registered linkls
	P_BP_LINKL * registered_linkls;

	// number of registered linkls
	size_t registered_linkl_n;

} BP_LINKL_REGISTRY, *P_BP_LINKL_REGISTRY;


// creates a new linked list head object
P_BP_LINKL BP_CreateLinkL(char *description, BP_BOOL thread_safe);

// displays head data only
BP_ERROR_T BP_DisplayLinkLHeadDataOnly(P_BP_LINKL head);

// displays the current tail queue
BP_ERROR_T BP_DisplayLinkL(P_BP_LINKL head);

// displays a single linked list item
BP_ERROR_T BP_DisplayLinkLItem(P_BP_LINKL_DATA item);

// adds data to the front of the list (push front)
BP_ERROR_T BP_AddDataToLinkLHead(P_BP_LINKL head, void * data, size_t data_size, BP_MEMPROF_ALLOC_TYPE type, BPLN_PARMS);

// adds data to the end of a list (push tail)
BP_ERROR_T BP_AddDataToLinkLTail(P_BP_LINKL head, void * data, size_t data_size,  BP_MEMPROF_ALLOC_TYPE type, BPLN_PARMS);

// adds data to the end of a list (push tail)
BP_ERROR_T BP_AddAfterLinkL(P_BP_LINKL head, P_BP_LINKL_DATA insert_after_this, void * data, size_t data_size,  BP_MEMPROF_ALLOC_TYPE type, BPLN_PARMS);

// adds data to the end of a list (push tail)
BP_ERROR_T BP_AddBeforeLinkL(P_BP_LINKL head, P_BP_LINKL_DATA insert_before_this, void * data, size_t data_size,  BP_MEMPROF_ALLOC_TYPE type, BPLN_PARMS);

// destroys a linked list item
BP_ERROR_T BP_DestroyLinkLItem(P_BP_LINKL head, P_BP_LINKL_DATA item);

// destroys an entire linked list
BP_ERROR_T BP_DestroyLinkL(P_BP_LINKL head, BP_BOOL destroy_head);

// attemts to forward lookup a pointer
P_BP_LINKL_DATA BP_ForwardLookupDataItemFromPointer(P_BP_LINKL head, void *ptr);

// attempts to backward lookup
P_BP_LINKL_DATA BP_BackwardLookupDataItemFromPointer(P_BP_LINKL head, void *ptr);


// ==== string searching ========================

// attempts to search all elements for a string
char * BP_LinkLSearchForString(P_BP_LINKL head, char *search_string, size_t search_string_len);


// ==== list flateners ==========================

// This will take each element in the array, create a buffer of appropriate length
// and store each element within the newly created buffer.
BP_ERROR_T BP_LinkLFlattenRaw(P_BP_LINKL head);

// this will join string elements
BP_ERROR_T BP_LinkLFlattenJoinStringElements(P_BP_LINKL head);

// This will destroy flattened data generated by the linkl flattener.
BP_ERROR_T BP_LinkLDestroyFlatStringData(P_BP_LINKL head);


// ==== list to array utilities =================

// This routine takes a linkl and copies all data within into an external
// pointer array.  The elements in the array are the linear progression of
// data within the list.
void ** BP_LinkLToBPCallocPointerArray(P_BP_LINKL head);


// ==== print utilities =========================

// Attempts to print all string elements out to standard out, mostly for
// debugging/development testing.
BP_ERROR_T BP_LinkLPrintStringElementsToStdout(P_BP_LINKL head);




// ==== allocator Functions =====================

// allocator / destructor functions
char * tq_strdup (P_BP_LINKL head, char *dup,                   BPLN_PARMS);
void * tq_memdup (P_BP_LINKL head, void *data,  size_t size,    BPLN_PARMS);
char * tq_strndup(P_BP_LINKL head, char *dup,   size_t n,       BPLN_PARMS);
void * tq_realloc(P_BP_LINKL head, void * addr, size_t size,    BPLN_PARMS);
void * tq_malloc (P_BP_LINKL head, size_t size,                 BPLN_PARMS);
void * tq_calloc (P_BP_LINKL head, size_t size, size_t size_n,  BPLN_PARMS);
void   tq_free   (P_BP_LINKL head, void * addr);

// Adds tabs to a string element in the linked list.  Typically used when generating
// large JSON buffers.
char * tq_tabs   (P_BP_LINKL head, size_t tab_count, BPLN_PARMS);

// unique version of string allocators
char * tq_strdup_unique (P_BP_LINKL head, char *dup,                   BPLN_PARMS);
char * tq_strndup_unique(P_BP_LINKL head, char *dup, size_t n,         BPLN_PARMS);


// links an existing chunk into a tailqueue (allows arbitrarily adding items to queue)
BP_ERROR_T tq_link(P_BP_LINKL head, void * addr, size_t size, BPLN_PARMS);

// links an existing chunk into a tailqueue (does a length check, max 100mb length)
BP_ERROR_T tq_link_str(P_BP_LINKL head, char * addr, BPLN_PARMS);

// links a pointer array to a tailqueue
BP_ERROR_T tq_link_ptr_array(P_BP_LINKL head, void ** ptrs, size_t ptr_n, size_t *lengths, BP_BOOL link_ptr_container, BPLN_PARMS);


#endif /* __BP_LINKED_LIST_H */



/*
 * BP_LinkedList.cc
 *
 *  Created on: Aug 27, 2013
 *      Author: root
 */

#include "../../include/BP-Main.h"

// creates a new linked list head object
P_BP_LINKL BP_CreateLinkL(char *description, BP_BOOL thread_safe)
{

	// allocate the new list item
	P_BP_LINKL list = (P_BP_LINKL) bpcalloc(sizeof(BP_LINKL), 1);
	if(!list)
		return NULL;

	if(thread_safe)
	{

		// set thread safe enabling here
		list->thread_safe_enabled = true;

		// initialize semaphore
		sem_init(&list->thread_safe_semaphore, 0, 1);

	}

	// reconfigure semaphore
	if(list->thread_safe_enabled)
		sem_wait(&list->thread_safe_semaphore);

	// initialize the queue
	TAILQ_INIT(&list->head);

	// copy in the description
	if(description)
		strncpy((char *) &list->description, description, sizeof(list->description)-1);

	// assign the uuid as a new random UUID
	list->uuid = NEW_RANDOM_UUID;

	// reconfigure semaphore
	if(list->thread_safe_enabled)
		sem_post(&list->thread_safe_semaphore);

	// return the filled out list
	return list;

}

// adds data to the front of the list (push front)
BP_ERROR_T BP_AddDataToLinkLHead(P_BP_LINKL head, void * data, size_t data_size,  BP_MEMPROF_ALLOC_TYPE type, BPLN_PARMS)
{

	if(!head)
		return ERR_FAILURE;
	if(!data)
		return ERR_FAILURE;
	if(!data_size)
		return ERR_FAILURE;

	// create new data
	P_BP_LINKL_DATA new_data = (P_BP_LINKL_DATA) bpcalloc(sizeof(BP_LINKL_DATA), 1);
	if(!new_data)
		return ERR_FAILURE;

	new_data->data      = data;
	new_data->data_size = data_size;
	new_data->data_type = type;

	// add debugging info if set
	if(file_name)
		new_data->file = (char *) bpstrdup(file_name);
	if(line_number)
		new_data->line_number = line_number;
	if(func)
		new_data->function = (char *) bpstrdup(func);

	// reconfigure semaphore
	if(head->thread_safe_enabled)
		sem_wait(&head->thread_safe_semaphore);

	// insert a new item into the queue
	TAILQ_INSERT_HEAD(&head->head, new_data, linkl_data);

	// increment the data size
	head->total_size += data_size;
	head->total_allocations++;

	// reconfigure semaphore
	if(head->thread_safe_enabled)
		sem_post(&head->thread_safe_semaphore);

	// return indicating success
	return ERR_SUCCESS;

}

// adds data to the end of a list (push tail)
BP_ERROR_T BP_AddDataToLinkLTail(P_BP_LINKL head, void * data, size_t data_size,  BP_MEMPROF_ALLOC_TYPE type, BPLN_PARMS)
{

	if(!head)
		return ERR_FAILURE;
	if(!data)
		return ERR_FAILURE;
	if(!data_size)
		return ERR_FAILURE;

	// create new data
	P_BP_LINKL_DATA new_data = (P_BP_LINKL_DATA) bpcalloc(sizeof(BP_LINKL_DATA), 1);
	if(!new_data)
		return ERR_FAILURE;

	new_data->data      = data;
	new_data->data_size = data_size;
	new_data->data_type = type;

	// add debugging info if set
	if(file_name)
		new_data->file = (char *) bpstrdup(file_name);
	if(line_number)
		new_data->line_number = line_number;
	if(func)
		new_data->function = (char *) bpstrdup(func);

	// reconfigure semaphore
	if(head->thread_safe_enabled)
		sem_wait(&head->thread_safe_semaphore);

	// insert a new item into the queue
	TAILQ_INSERT_TAIL(&head->head, new_data, linkl_data);

	// increment the data size
	head->total_size += data_size;
	head->total_allocations++;

	// reconfigure semaphore
	if(head->thread_safe_enabled)
		sem_post(&head->thread_safe_semaphore);

	// return indicating success
	return ERR_SUCCESS;

}


// adds data to the end of a list (push tail)
BP_ERROR_T BP_AddAfterLinkL(P_BP_LINKL head, P_BP_LINKL_DATA insert_after_this, void * data, size_t data_size,  BP_MEMPROF_ALLOC_TYPE type, BPLN_PARMS)
{

	if(!head)
		return ERR_FAILURE;
	if(!insert_after_this)
		return ERR_FAILURE;
	if(!data)
		return ERR_FAILURE;
	if(!data_size)
		return ERR_FAILURE;

	// create new data
	P_BP_LINKL_DATA new_data = (P_BP_LINKL_DATA) bpcalloc(sizeof(BP_LINKL_DATA), 1);
	if(!new_data)
		return ERR_FAILURE;

	new_data->data      = data;
	new_data->data_size = data_size;
	new_data->data_type = type;

	// add debugging info if set
	if(file_name)
		new_data->file = (char *) bpstrdup(file_name);
	if(line_number)
		new_data->line_number = line_number;
	if(func)
		new_data->function = (char *) bpstrdup(func);

	// reconfigure semaphore
	if(head->thread_safe_enabled)
		sem_wait(&head->thread_safe_semaphore);

	// insert a new item into the queue
	TAILQ_INSERT_AFTER(&head->head, insert_after_this, new_data, linkl_data);

	// increment the data size
	head->total_size += data_size;
	head->total_allocations++;

	// reconfigure semaphore
	if(head->thread_safe_enabled)
		sem_post(&head->thread_safe_semaphore);

	// return indicating success
	return ERR_SUCCESS;

}

// adds data to the end of a list (push tail)
BP_ERROR_T BP_AddBeforeLinkL(P_BP_LINKL head, P_BP_LINKL_DATA insert_before_this, void * data, size_t data_size,  BP_MEMPROF_ALLOC_TYPE type, BPLN_PARMS)
{

	if(!head)
		return ERR_FAILURE;
	if(!insert_before_this)
		return ERR_FAILURE;
	if(!data)
		return ERR_FAILURE;
	if(!data_size)
		return ERR_FAILURE;



	// create new data
	P_BP_LINKL_DATA new_data = (P_BP_LINKL_DATA) bpcalloc(sizeof(BP_LINKL_DATA), 1);
	if(!new_data)
		return ERR_FAILURE;

	new_data->data      = data;
	new_data->data_size = data_size;
	new_data->data_type = type;

	// add debugging info if set
	if(line_number)
		new_data->line_number = line_number;
	if(func)
		new_data->function = (char *) bpstrdup(func);

	// reconfigure semaphore
	if(head->thread_safe_enabled)
		sem_wait(&head->thread_safe_semaphore);

	// insert a new item into the queue
	TAILQ_INSERT_BEFORE(insert_before_this, new_data, linkl_data);

	// increment the data size
	head->total_size += data_size;
	head->total_allocations++;

	// reconfigure semaphore
	if(head->thread_safe_enabled)
		sem_post(&head->thread_safe_semaphore);

	// return indicating success
	return ERR_SUCCESS;

}

// destroys a linked list item
BP_ERROR_T BP_DestroyLinkLItem(P_BP_LINKL head, P_BP_LINKL_DATA item)
{

	if(!head)
		return ERR_FAILURE;
	if(!item)
		return ERR_FAILURE;

	// reconfig semaphore
	if(head->thread_safe_enabled)
		sem_wait(&head->thread_safe_semaphore);

	// remove item from the list
	TAILQ_REMOVE(&head->head, item, linkl_data);

	// subtract the total size
	head->total_size = head->total_size - item->data_size;
	head->total_allocations--;

	if(item->data)
	{
		bpfree(item->data);
		item->data = NULL;
	}

	if(item->file)
	{
		bpfree(item->file);
		item->file = NULL;
	}

	if(item->function)
	{
		bpfree(item->function);
		item->function = NULL;
	}

	// destroy the actual item
	bpfree(item);
	item = NULL;

	// reconfig semaphore
	if(head->thread_safe_enabled)
		sem_post(&head->thread_safe_semaphore);

	// return indicating success
	return ERR_SUCCESS;

}


// destroys an entire linked list
BP_ERROR_T BP_DestroyLinkL(P_BP_LINKL head, BP_BOOL destroy_head)
{

	// ensure we have a structure
	if(!head)
		return ERR_FAILURE;

	// reconfig semaphore
	if(head->thread_safe_enabled)
		sem_wait(&head->thread_safe_semaphore);

	// destroy flat buffer data if set
	if(head->flat_buff)
	{
		BP_LinkLDestroyFlatStringData(head);
		head->flat_buff   = NULL;
		head->flat_buff_n = 0;
	}

	// fast method recommended by documentation (faster than using TAILQ_REMOVE)
	P_BP_LINKL_DATA data = NULL;
	while((data = TAILQ_FIRST(&head->head)))
	{

		// remove the item
		TAILQ_REMOVE(&head->head, data, linkl_data);

		// destroy data if set
		if(data->data)
		{
			bpfree(data->data);
			data->data = NULL;
		}

		// destroy function if set
		if(data->function)
		{
			bpfree(data->function);
			data->function = NULL;
		}
		if(data->file)
		{
			bpfree(data->file);
			data->file = NULL;
		}

		// destroy the data
		bpfree(data);
		data = NULL;

	}

	// reconfig semaphore
	if(head->thread_safe_enabled)
		sem_post(&head->thread_safe_semaphore);

	// ensure we destroy the lingering UUID
	if(head->uuid)
	{
		bpfree(head->uuid);
		head->uuid = NULL;
	}

	// free structure at his point
	if(destroy_head)
		bpfree(head);

	// return indicating success
	return ERR_SUCCESS;

}

// displays head data only
BP_ERROR_T BP_DisplayLinkLHeadDataOnly(P_BP_LINKL head)
{

	// check head
	if(!head)
		return ERR_FAILURE;

	printf("\n [+] LinkL TailQ: %p", head);
	printf("\n\t description:            %s", head->description);
	printf("\n\t thread_safe_enabled:    %u", head->thread_safe_enabled);
	printf("\n\t thread_safe_semaphore:  %u", head->thread_safe_semaphore);
	printf("\n\t uuid:                   %s", head->uuid);
	printf("\n\t total_size:             %u", head->total_size);
	printf("\n\t total_allocations:      %u", head->total_allocations);
	printf("\n\t &head:                  %p", (void*) &head->head);
	printf("\n");

	// return indicating success
	return ERR_SUCCESS;

}

// displays the current tail queue
BP_ERROR_T BP_DisplayLinkL(P_BP_LINKL head)
{

	// check head
	if(!head)
		return ERR_FAILURE;

	// attempt to get the first item
	P_BP_LINKL_DATA first_item = TAILQ_FIRST(&head->head);

	// ensure we have a first item
	if(!first_item)
		return ERR_FAILURE;

	printf("\n [+] LinkL TailQ: %p", head);
	printf("\n\t description:            %s", head->description);
	printf("\n\t thread_safe_enabled:    %u", head->thread_safe_enabled);
	printf("\n\t thread_safe_semaphore:  %u", head->thread_safe_semaphore);
	printf("\n\t uuid:                   %s", head->uuid);
	printf("\n\t total_size:             %u", head->total_size);
	printf("\n\t total_allocations:      %u", head->total_allocations);
	printf("\n\t &head:                  %p", (void*) &head->head);
	printf("\n");

	// looks for a pointer in the item
	TAILQ_FOREACH(first_item, &head->head, linkl_data)
	{

		BP_DisplayLinkLItem(first_item);

	}

	// return indicating success
	return ERR_SUCCESS;

}

// displays a single linked list item
BP_ERROR_T BP_DisplayLinkLItem(P_BP_LINKL_DATA item)
{

	// ensure we have an item
	if(!item)
		return ERR_FAILURE;

	printf("\n\t [+] List Item: %p", item);
	// display source debug information if necessary
	if(item->file)
		printf("\n\t\t allocated at: (%s:%u) : %s", item->file, item->line_number, item->function);

	printf("\n\t\t data:        %p", item->data);
	printf("\n\t\t data_size:   %u", item->data_size);
	printf("\n\t\t data_type:   ");

	switch(item->data_type)
	{
		case BP_MEMPROF_ALLOC_TYPE_UNSET:
			printf("BP_MEMPROF_ALLOC_TYPE_UNSET");
			break;
		case BP_MEMPROF_ALLOC_TYPE_MALLOC:
			printf("BP_MEMPROF_ALLOC_TYPE_MALLOC");
			break;
		case BP_MEMPROF_ALLOC_TYPE_CALLOC:
			printf("BP_MEMPROF_ALLOC_TYPE_CALLOC");
			break;
		case BP_MEMPROF_ALLOC_TYPE_STRDUP:
			printf("BP_MEMPROF_ALLOC_TYPE_STRDUP");
			break;
		case BP_MEMPROF_ALLOC_TYPE_STRNDUP:
			printf("BP_MEMPROF_ALLOC_TYPE_STRNDUP");
			break;
		case BP_MEMPROF_ALLOC_TYPE_REALLOC:
			printf("BP_MEMPROF_ALLOC_TYPE_REALLOC");
			break;
		case BP_MEMPROF_ALLOC_TYPE_USER_DEFINED_ASCII:
			printf("BP_MEMPROF_ALLOC_TYPE_USER_DEFINED_ASCII");
			break;
		default:
			printf("UNKNOWN TYPE");
			break;

	}

	// add trailing newline
	printf("\n");

	// return indicating success
	return ERR_SUCCESS;

}

// attemts to forward lookup a pointer
P_BP_LINKL_DATA BP_ForwardLookupDataItemFromPointer(P_BP_LINKL head, void *ptr)
{

	if(!ptr)
		return NULL;

	// attempt to get the first item
	P_BP_LINKL_DATA first_item = TAILQ_FIRST(&head->head);

	// ensure we have a first item
	if(!first_item)
		return NULL;

	// looks for a pointer in the item
	TAILQ_FOREACH(first_item, &head->head, linkl_data)
	{
		if(first_item->data == ptr)
			return first_item;
	}

	// return no ptr
	return NULL;

}

// attempts to backward lookup
P_BP_LINKL_DATA BP_BackwardLookupDataItemFromPointer(P_BP_LINKL head, void *ptr  )
{

	if(!ptr)
		return NULL;

	// set item
	P_BP_LINKL_DATA item = TAILQ_FIRST(&head->head);

	// looks for a pointer in the item
	TAILQ_FOREACH(item, &head->head, linkl_data) {
		if(item->data == ptr)
			return item;
	}

	// return no ptr
	return NULL;

}


// ==== string searching ========================

// attempts to search all elements for a string
char * BP_LinkLSearchForString(P_BP_LINKL head, char *search_string, size_t search_string_len)
{

	// run checks
	if(!head)
		return NULL;
	if(!search_string)
		return NULL;
	if(!search_string_len)
		return NULL;

	// attempt to get the first item
	P_BP_LINKL_DATA first_item = TAILQ_FIRST(&head->head);

	// ensure we have a first item
	if(!first_item)
		return NULL;

	// looks for a pointer in the item
	TAILQ_FOREACH(first_item, &head->head, linkl_data)
	{

		// only work with strndup/strdup types
		if
		(
			first_item->data_type != BP_MEMPROF_ALLOC_TYPE_STRDUP   &&
			first_item->data_type != BP_MEMPROF_ALLOC_TYPE_STRNDUP
		)
		{
			continue;
		}

		// ensure lengths match
		if(bpstrlen(search_string) != bpstrlen((char *) first_item->data))
			continue;

		// if the strings match, return the data
		if(memcmp((void *) first_item->data, (void *) search_string, search_string_len) == 0)
			return (char *) first_item->data;

	}

	// return null if we can't find the string
	return NULL;

}

// ==== list flateners ==========================

// This will take each element in the array, create a buffer of appropriate length
// and store each element within the newly created buffer essentially creating a
// flat unsigned char array representing the entire list.
BP_ERROR_T BP_LinkLFlattenRaw(P_BP_LINKL head)
{

	// ensure we have a head
	if(!head)
		return ERR_FAILURE;

	// destroy flattened data if it already exists
	if(head->flat_buff)
	{
		bpfree(head->flat_buff);
		head->flat_buff_n = 0;
		head->flat_buff   = NULL;
	}

	// ensure the list has a total size
	if(!head->total_size)
		return ERR_FAILURE;

	// attempt to get the first item
	P_BP_LINKL_DATA first_item = TAILQ_FIRST(&head->head);

	// ensure we have a first item
	if(!first_item)
		return ERR_FAILURE;

	// current position, used for writing data to the string
	size_t current_pos = 0;

	// inner iterator for data
	size_t n = 0;

	// allocate space for the buffer (+1 for mandatory null termination)
	head->flat_buff = (unsigned char *) bpcalloc(head->total_size+1, 1);
	head->flat_buff_n = head->total_size;

	// since void has no type, we need a temporary data pointer
	unsigned char * data = NULL;

	// looks for a pointer in the item
	TAILQ_FOREACH(first_item, &head->head, linkl_data)
	{

		// set temporary pointer
		data = (unsigned char *) first_item->data;

		// now write the buffer into position
		for(n=0; n < first_item->data_size; n++)
		{
			head->flat_buff[current_pos] = (unsigned char) data[n];
			current_pos++;
 		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// This flattens the the linkl string elements into a
// joined single string.  It removes the null terminators
// from existing strings when joining.  If you want to
// flatten raw data, use the raw version.
BP_ERROR_T BP_LinkLFlattenJoinStringElements(P_BP_LINKL head)
{

	// ensure we have a head
	if(!head)
		return ERR_FAILURE;

	// destroy flattened data if it already exists
	if(head->flat_buff)
	{
		bpfree(head->flat_buff);
		head->flat_buff_n = 0;
		head->flat_buff   = NULL;
	}

	// ensure the list has a total size
	if(!head->total_size)
		return ERR_FAILURE;

	// attempt to get the first item
	P_BP_LINKL_DATA first_item = TAILQ_FIRST(&head->head);

	// ensure we have a first item
	if(!first_item)
		return ERR_FAILURE;

	// current position, used for writing data to the string
	size_t current_pos = 0;

	// inner iterator for data
	size_t n = 0;

	// allocate space for the buffer (+1 for mandatory null termination)
	head->flat_buff   = (unsigned char *) bpcalloc(head->total_size+1, 1);
	head->flat_buff_n = head->total_size;

	// since void has no type, we need a temporary data pointer
	unsigned char * data = NULL;

	// looks for a pointer in the item
	TAILQ_FOREACH(first_item, &head->head, linkl_data)
	{

		// ensure we have a string chunk
		if(
			(first_item->data_type == BP_MEMPROF_ALLOC_TYPE_STRDUP)  ||
			(first_item->data_type == BP_MEMPROF_ALLOC_TYPE_STRNDUP) ||
			(first_item->data_type == BP_MEMPROF_ALLOC_TYPE_USER_DEFINED_ASCII)
		)
		{

			// set temporary pointer
			data = (unsigned char *) first_item->data;

			// now write the buffer into position
			for(n=0; n < (strnlen((char *) first_item->data, first_item->data_size)); n++)
			{

				head->flat_buff[current_pos] = (unsigned char) data[n];
				current_pos++;

	 		}

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// This will destroy flattened data generated by the linkl flattener.
BP_ERROR_T BP_LinkLDestroyFlatStringData(P_BP_LINKL head)
{

	// check to make sure we have a head pointer
	if(!head)
		return ERR_FAILURE;

	// destroy flattened data if it already exists
	if(head->flat_buff)
	{

		// free the buffer
		bpfree(head->flat_buff);

		// reset values
		head->flat_buff_n = 0;
		head->flat_buff = NULL;

	}

	// return indicating success
	return ERR_SUCCESS;

}



// ==== list to array utilities =================

// This routine takes a linkl and copies all data within into an external
// pointer array.  The elements in the array are the linear progression of
// data within the list.
void ** BP_LinkLToBPCallocPointerArray(P_BP_LINKL head)
{

	// run checks
	if(!head)
		return NULL;

	// attempt to get the first item
	P_BP_LINKL_DATA first_item = TAILQ_FIRST(&head->head);

	// ensure we have a first item
	if(!first_item)
		return NULL;

	// ensure we have a total allocation count
	if(!head->total_allocations)
		return NULL;

	// create array (total allocs + 1)
	void ** ret_array = (void **) bpcalloc(sizeof(void *) * (head->total_allocations + 1), 1);

	// index iterator
	size_t index_iter = 0;

	// looks for a pointer in the item
	TAILQ_FOREACH(first_item, &head->head, linkl_data)
	{


		// allocate space for the memory copy
		ret_array[index_iter] = (void *) bpcalloc(first_item->data_size+1, 1);

		// Ensure we have a ret array index created, if not fail-hard and destroy all external
		// memory.
		if(!ret_array[index_iter])
		{

			// destroy the pointer array
			BP_DestroyPointerArrayNullTerminated(ret_array);

			// destroy the returned array after elements are freed
			bpfree(ret_array);

			// return null
			return NULL;

		}

		// copy in the data
		memcpy(ret_array[index_iter], first_item->data, first_item->data_size);

		// increment the index iterator
		index_iter++;

	}

	// return null if we can't find the string
	return ret_array;

}


// ==== print utilities =========================

// Attempts to print all string elements out to standard out, mostly for
// debugging/development testing.
BP_ERROR_T BP_LinkLPrintStringElementsToStdout(P_BP_LINKL head)
{


	// ensure we have a head
	if(!head)
		return ERR_FAILURE;

	// ensure the list has a total size
	if(!head->total_size)
		return ERR_FAILURE;

	// attempt to get the first item
	P_BP_LINKL_DATA first_item = TAILQ_FIRST(&head->head);

	// ensure we have a first item
	if(!first_item)
		return ERR_FAILURE;


	// string index used to display
	size_t string_index = 0;

	// looks for a pointer in the item
	TAILQ_FOREACH(first_item, &head->head, linkl_data)
	{

		// display header if we don't have a string index
		if(string_index == 0)
			printf("\n [+] Displaying String Elements for List %p", head);



		// only work with strndup/strdup types
		if
		(
			first_item->data_type != BP_MEMPROF_ALLOC_TYPE_STRDUP   &&
			first_item->data_type != BP_MEMPROF_ALLOC_TYPE_STRNDUP
		)
		{

			// increase the string index
			string_index++;
			continue;

		}

		// display string index
		printf("\n\t [%u] - %s", string_index, (char *) first_item->data);

		// increase the string index
		string_index++;

	}


	// return indicating success
	return ERR_SUCCESS;

}




// ==== allocator Functions =====================


char * tq_strdup (P_BP_LINKL head, char * dupe, BPLN_PARMS)
{

	// ensure we have a head entry to work with
	if(!head)
		return NULL;

	// allocate chunk space
	char * chunk = bpstrdup( dupe );
	if(!chunk)
		return NULL;


	// get the dupe length
	size_t dup_len = bpstrlen((char *) chunk);

	// attempt to add the chunk and return directly
	if(BP_AddDataToLinkLTail(head, chunk, dup_len+1 , BP_MEMPROF_ALLOC_TYPE_STRDUP, file_name, line_number, func) != ERR_SUCCESS)
	{
		bpfree(chunk);
		chunk = NULL;
		return NULL;
	}

	// return the chunk
	return chunk;
}

char * tq_strndup(P_BP_LINKL head, char *dup, size_t n,  BPLN_PARMS)
{

	// ensure we have a head entry to work with
	if(!head)
		return NULL;

	// allocate chunk space
	char * chunk = bpstrndup( dup, n );
	if(!chunk)
		return NULL;

	// attempt to add the chunk and return directly
	if(BP_AddDataToLinkLTail(head, chunk, n+1 , BP_MEMPROF_ALLOC_TYPE_STRNDUP, file_name, line_number, func) != ERR_SUCCESS)
	{
		bpfree(chunk);
		chunk = NULL;
		return NULL;
	}

	// return the chunk
	return chunk;

}

// unique version of strdup (only will dupe a string if it doesn't already
// exist in the linkl.
char * tq_strdup_unique (P_BP_LINKL head, char *dup, BPLN_PARMS)
{

	// ensure we have a head and a dup
	if(!head)
		return NULL;

	// ensure we have a string to duplicate
	if(!dup)
		return NULL;

	// calculate duplicate length
	size_t dup_len = bpstrlen(dup);

	// attempt to lookup the string
	char * search_str = BP_LinkLSearchForString(head, dup, dup_len);

	// if we found the string in the list, return null
	if(search_str)
		return NULL;

	// run actual strdup now
	return tq_strdup(head, dup, BPLN);

}

// unique version of strndup (only will dupe a string if it doesn't already
// exist in the linkl.
char * tq_strndup_unique(P_BP_LINKL head, char *dup, size_t n, BPLN_PARMS)
{

	// ensure we have a head and a dup
	if(!head)
		return NULL;

	// ensure we have a string to duplicate
	if(!dup)
		return NULL;

	// ensure we have a length for the string we're duplicating
	if(!n)
		return NULL;

	// attempt to lookup the string
	char * search_str = BP_LinkLSearchForString(head, dup, n);

	// if we found the string in the list, return null
	if(search_str)
		return NULL;

	// run actual strndup now
	return tq_strndup(head, dup, n, BPLN);

}

// reallocate data
void * tq_realloc(P_BP_LINKL head, void * addr, size_t size,  BPLN_PARMS)
{

	// ensure we have a head entry to work with
	if(!head)
		return NULL;

	// chunk to be returned
	void * chunk = NULL;

	// lookup item if necessary
	P_BP_LINKL_DATA item = (P_BP_LINKL_DATA) BP_ForwardLookupDataItemFromPointer(head, addr);

	// if the item already exists (from previous allocation)
	if(item)
	{

		// subtrack previous size from total
		head->total_size = head->total_size - item->data_size;

		// realloc the actual chunk value
		item->data      = bprealloc( item->data, size );
		item->data_size = size;

		// recalculate the total size
		head->total_size = head->total_size + item->data_size;

		// ensure we have allocated data
		if(!item->data)
		{
			BP_DestroyLinkLItem(head, item);
			return NULL;
		}

		// return the new data
		return item->data;

	}
	else
	{
		// allocate chunk space
		chunk = bprealloc( addr, size );

	}

	if(!chunk)
		return NULL;

	// attempt to add the chunk to the list if it wasn't already found
	if(BP_AddDataToLinkLTail(head, chunk, size , BP_MEMPROF_ALLOC_TYPE_REALLOC, file_name, line_number, func) != ERR_SUCCESS)
	{
		bpfree(chunk);
		chunk = NULL;
		return NULL;
	}

	// return the chunk
	return chunk;
}

void * tq_malloc (P_BP_LINKL head, size_t size,  BPLN_PARMS)
{
	// ensure we have a head entry to work with
	if(!head)
		return NULL;

	// allocate chunk space
	void * chunk = bpmalloc( size );
	if(!chunk)
		return NULL;;

	// attempt to add the chunk and return directly
	if(BP_AddDataToLinkLTail(head, chunk, size , BP_MEMPROF_ALLOC_TYPE_MALLOC, file_name, line_number, func) != ERR_SUCCESS)
	{
		bpfree(chunk);
		chunk = NULL;
		return NULL;
	}

	// return the chunk
	return chunk;

}

void * tq_calloc (P_BP_LINKL head, size_t size, size_t size_n,  BPLN_PARMS)
{

	// ensure we have a head entry to work with
	if(!head)
		return NULL;

	// allocate chunk space
	void * chunk = bpcalloc( 1, size);
	if(!chunk)
		return NULL;


	// attempt to add the chunk and return directly
	if(BP_AddDataToLinkLTail(head, chunk, size , BP_MEMPROF_ALLOC_TYPE_CALLOC, file_name, line_number, func) != ERR_SUCCESS)
	{
		bpfree(chunk);
		chunk = NULL;
		return NULL;
	}

	// return the chunk
	return chunk;

}

// Duplicate a chunk of memory into our linkl (adds memory to the end of the list)
void * tq_memdup (P_BP_LINKL head, void *data,  size_t size,    BPLN_PARMS)
{

	// run basic checks
	if(!head)
		return NULL;
	if(!data)
		return NULL;
	if(!size)
		return NULL;


	// allocate chunk space
	void * chunk = bpcalloc( 1, size);
	if(!chunk)
		return NULL;

	// copy data into chunk
	memcpy(chunk, data, size);

	// attempt to add the chunk and return directly
	if(BP_AddDataToLinkLTail(head, chunk, size , BP_MEMPROF_ALLOC_TYPE_CALLOC, file_name, line_number, func) != ERR_SUCCESS)
	{
		bpfree(chunk);
		chunk = NULL;
		return NULL;
	}

	// return the chunk
	return chunk;

}

// free's chunks and deletes link elements.  Can be used with
// chunks that may not be in the list (makes implementations easier)
void   tq_free   (P_BP_LINKL head, void * addr)
{

	if(!addr)
		return;
	if(!head)
	{
		printf("\n [+] Error: Cannot free a list element without an accompanying head.");
		return;
	}


	// look for item (returns null if it doesn't exist)
	P_BP_LINKL_DATA item = (P_BP_LINKL_DATA) BP_ForwardLookupDataItemFromPointer(head, addr);

	// destroy the individual link item
	BP_DestroyLinkLItem(head, item);

}

// Adds tabs to a string element in the linked list.  Typically used when generating
// large JSON buffers.
char * tq_tabs   (P_BP_LINKL head, size_t tab_count, BPLN_PARMS)
{

	// if we don't have a head, return null immediately
	if(!head)
		return NULL;

	// ceate tab buffer
	return BP_CreateTabBuffer(tab_count, head);

}

// links an existing chunk into a tailqueue
BP_ERROR_T tq_link(P_BP_LINKL head, void * addr, size_t size, BPLN_PARMS)
{

	// ensure we have a head entry to work with
	if(!head)
		return ERR_SUCCESS;

	if(!addr || !size)
		return ERR_FAILURE;

	// attempt to add the chunk and return directly
	if(BP_AddDataToLinkLTail(head, addr, size , BP_MEMPROF_ALLOC_TYPE_MALLOC, file_name, line_number, func) != ERR_SUCCESS)
	{
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}

// links an existing chunk into a tailqueue (does a length check, max 100mb length)
BP_ERROR_T tq_link_str(P_BP_LINKL head, char * addr,BPLN_PARMS)
{

	// ensure we have a head entry to work with
	if(!head)
		return ERR_FAILURE;

	if(!addr)
		return ERR_FAILURE;

	// set max lookup for strnlen(might fault anyway)
	size_t max_len = 100 * BP_MEGABYTE;

	// get length
	size_t length = strnlen(addr, max_len);

	// don't link strings > 100mb
	if(length >= max_len)
		return ERR_FAILURE;



	// attempt to add the chunk and return directly
	if(BP_AddDataToLinkLTail(head, addr, length , BP_MEMPROF_ALLOC_TYPE_STRDUP, file_name, line_number, func) != ERR_SUCCESS)
	{
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}


// links a pointer array to a tailqueue
BP_ERROR_T tq_link_ptr_array(P_BP_LINKL head, void ** ptrs, size_t ptr_n, size_t *lengths, BP_BOOL link_ptr_container, BPLN_PARMS)
{

	// ensure we have a head entry to work with
	if(!head)
		return ERR_FAILURE;

	if(!ptrs || !ptr_n || !lengths)
		return ERR_FAILURE;


	// walk pointers and link
	size_t n = 0;
	for(; n < ptr_n; n++)
	{

		// link the item
		// tq_link(head, ptrs[n], lengths[n], file_name, line_number, func);

		if(BP_AddDataToLinkLTail(head, (void *) ptrs[n], 100, BP_MEMPROF_ALLOC_TYPE_MALLOC, file_name, line_number, func) != ERR_SUCCESS)
		{
			printf("\n Failed to link.");
		}


	}

	if(BP_AddDataToLinkLTail(head, (void *) ptrs, ptr_n*sizeof(size_t) , BP_MEMPROF_ALLOC_TYPE_MALLOC, file_name, line_number, func) != ERR_SUCCESS)
	{
		printf("\n Failed to link.");
	}


	// return indicating success
	return ERR_SUCCESS;

}


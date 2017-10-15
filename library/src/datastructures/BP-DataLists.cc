#include "../../include/BP-Main.h"

/*
 *
 * Data Lists:
 * =============================================================
 * Data lists use the blockparty tail queue linked-list implementation
 * for the purpose of providing a singular mechanism for creating stack
 * of linked and dynamic buffers.  These buffers are used typically for
 * sending dynamic content over some play mechanism.  Play mechanisms
 * essentially play a data list, allowing for recieving and sending data
 * dynamically based upon the data list type elements.
 *
 */


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Data List Operation Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// creates a new data list entry
P_BP_DATA_LIST BP_CreateDataList(char *description)
{

	// set data list
	P_BP_DATA_LIST data_list = NULL;

	// allocate space for the list (zero memory)
	data_list = (P_BP_DATA_LIST) bpcalloc(sizeof(BP_DATA_LIST), 1);

	// ensure we created a data list
	if(!data_list)
		return NULL;

	// create our linkl for storing chunks
	data_list->tq = (P_BP_LINKL) BP_CreateLinkL("datalist", BP_TRUE);

	// check to see that we have created a tail queue
	if(!data_list->tq)
		return NULL;

	// duplicate the description here
	if(description)
		data_list->desc = tq_strndup(data_list->tq, description, strnlen(description, 512), BPLN);

	// create new uuid for the list
	char *tmp = (char *) BP_GenRandomUUID();

	// set the uuid in the data list
	data_list->uuid = (char *) tq_strndup(data_list->tq, tmp, strnlen(tmp, 512), BPLN);

	// destroy the string
	bpfree(tmp);

	// return the new data list
	return data_list;

}

// destroy the data list
BP_ERROR_T BP_DestroyDataList(P_BP_DATA_LIST data_list)
{

	// ensure we have a data list
	if(!data_list)
		return ERR_FAILURE;

	// destroy tail queue if we have one
	if(data_list->tq)
	{

		// destroy all chunks in the linkl
		BP_DestroyLinkL(data_list->tq, BP_TRUE);

		// nullify pointer
		data_list->tq = NULL;

	}

	// destroy the data list
	bpfree(data_list);

	// return indicating success
	return ERR_SUCCESS;

}

// Add a new element at the end of the list
BP_ERROR_T BP_DataListAddEntry
(
	P_BP_DATA_LIST     data_list,
	char               *description,
	unsigned char *    data,
	size_t             data_size,
	BP_DATA_ENTRY_TYPE data_type,
	void            (* custom_prepare)(void *, void *),
	void              *custom_prepare_param,
	void            (* custom_read)(void *, void *),
	void             * custom_read_param,
	void            (* custom_write)(void *, void *),
	void             * custom_write_param,
	void            (* custom_post)(void *, void *),
	void             * custom_post_param

)
{

	// ensure we have a datalist and a data type
	if(!data_list || !data_type)
		return ERR_FAILURE;

	// increment entries count
	data_list->entries_n++;

	// reallocate space for entries
	data_list->entries = (P_BP_DATA_LIST_ENTRY) tq_realloc(data_list->tq, data_list->entries, sizeof(BP_DATA_LIST_ENTRY) * data_list->entries_n, BPLN);

	// create space for item
	if(!data_list->entries)
	{

		// return indicating failure if we cannot allocate space for the chunk
		return ERR_FAILURE;

	}

	// zero out the structure data
	memset(&data_list->entries[data_list->entries_n-1], 0x00, sizeof(BP_DATA_LIST_ENTRY));

	// create temporary pointer
	P_BP_DATA_LIST_ENTRY tmp = (P_BP_DATA_LIST_ENTRY) &data_list->entries[data_list->entries_n-1];

	// set custom prepare callback
	if(custom_prepare)
	{
		tmp->custom_prepare = custom_prepare;
		tmp->custom_prepare_params = custom_prepare_param;
	}

	// set custom read
	if(custom_read)
	{
		tmp->custom_read = custom_read;
		tmp->custom_read_params = custom_read_param;
	}

	// set custom write
	if(custom_write)
	{
		tmp->custom_write = custom_write;
		tmp->custom_write_params = custom_write_param;
	}

	// set custom post
	if(custom_post)
	{
		tmp->custom_post = custom_post;
		tmp->custom_post_params = custom_post_param;
	}


	// reallocate space for data
	tmp->data = (unsigned char *) tq_realloc(data_list->tq, tmp->data, data_size, BPLN);

	// copy in data if necessary (read chunks do not need to perform this step)
	if(data)
	{
		memcpy(tmp->data, data, data_size);
	}
	else // zero out memory if not assigning
	{
		memset(tmp->data, 0x00, data_size);
	}

	// set data size
	tmp->size = data_size;

	// set description
	if(description)
		tmp->description = (char *) tq_strndup(data_list->tq, description, strnlen(description, 512), BPLN);

	// switch on the data type
	switch(data_type)
	{

		// signed 8 bit integer
		case BP_DATA_ENTRY_TYPE_INT8:
			tmp->data_type = BP_DATA_ENTRY_TYPE_INT8;
			break;

		// signed 16 bit integer
		case BP_DATA_ENTRY_TYPE_INT16:
			tmp->data_type = BP_DATA_ENTRY_TYPE_INT16;
			break;

		// signed 32 bit integer
		case BP_DATA_ENTRY_TYPE_INT32:
			tmp->data_type = BP_DATA_ENTRY_TYPE_INT32;
			break;

		// signed 64 bit integer
		case BP_DATA_ENTRY_TYPE_INT64:
			tmp->data_type = BP_DATA_ENTRY_TYPE_INT64;
			break;

		// unsigned 8 bit integer
		case BP_DATA_ENTRY_TYPE_UINT8:
			tmp->data_type = BP_DATA_ENTRY_TYPE_UINT8;
			break;

		// unsigned 16bit integer
		case BP_DATA_ENTRY_TYPE_UINT16:
			tmp->data_type = BP_DATA_ENTRY_TYPE_UINT16;
			break;

		// unsigned 32bit integer
		case BP_DATA_ENTRY_TYPE_UINT32:
			tmp->data_type = BP_DATA_ENTRY_TYPE_UINT32;
			break;

		// unsigned 64bit integer
		case BP_DATA_ENTRY_TYPE_UINT64:
			tmp->data_type = BP_DATA_ENTRY_TYPE_UINT64;
			break;

		// arbitrary binary entry
		case BP_DATA_ENTRY_TYPE_BINARY:
			tmp->data_type = BP_DATA_ENTRY_TYPE_BINARY;
			break;

		// ascii entry
		case BP_DATA_ENTRY_TYPE_ASCII:
			tmp->data_type = BP_DATA_ENTRY_TYPE_ASCII;
			break;

		// format specifiers
		case BP_DATA_ENTRY_TYPE_FORMAT:
			tmp->data_type = BP_DATA_ENTRY_TYPE_FORMAT;
			break;

		// random data
		case BP_DATA_ENTRY_TYPE_RANDOM:
			tmp->data_type = BP_DATA_ENTRY_TYPE_RANDOM;
			break;

		// arbitrary sequence data
		case BP_DATA_ENTRY_TYPE_SEQUENCE:
			tmp->data_type = BP_DATA_ENTRY_TYPE_SEQUENCE;
			break;

		// read entry
		case BP_DATA_ENTRY_TYPE_READ:
			tmp->data_type = BP_DATA_ENTRY_TYPE_READ;
			break;

		// default is a bad/unset type
		default:
			tmp->data_type = BP_DATA_ENTRY_TYPE_UNSET;
			break;

	}

	// set activation flags
	tmp->activation_enabled   = BP_TRUE;

	// set disabled flag if necessary
	if(data_list->prevent_activations == BP_TRUE)
		tmp->activation_administratively_prevented = BP_TRUE;

	// return indicating success
	return ERR_SUCCESS;

}

// activates a data list entry by calling relevant callbacks
// within the datalist.
BP_ERROR_T BP_DataListActivateEntry(P_BP_DATA_LIST data_list, P_BP_DATA_LIST_ENTRY entry)
{

	// check to ensure we have a data list
	if(!data_list)
		return ERR_FAILURE;

	// check to ensure we have an entry pointer
	if(!entry)
		return ERR_FAILURE;

	// check to see if the data list heads preventative activation flag is set, if so
	// exit.
	if(data_list->prevent_activations == BP_TRUE)
	{

		// mark as administratively prevented
		entry->activation_administratively_prevented = BP_TRUE;

		// return indicating failure
		return ERR_FAILURE;

	}

	// Next check if the individual entries activation flag is enabled or not.  If not
	// exit.
	if(entry->activation_enabled != BP_TRUE)
	{

		// mark as administratively prevented
		entry->activation_administratively_prevented = BP_TRUE;

		// return indicating failure
		return ERR_FAILURE;

	}


	// if custom prepare is set, run it
	if(entry->custom_prepare)
	{
		entry->custom_prepare(entry, entry->custom_prepare_params);
	}

	// activate read callback if type is read
	if(entry->data_type == BP_DATA_ENTRY_TYPE_READ)
	{
		if(entry->custom_read)
		{
			entry->custom_read(entry, entry->custom_read_params);
		}
	}

	// activate write callback if type is read
	if(
			(entry->data_type != BP_DATA_ENTRY_TYPE_READ) &&
			(entry->data_type != BP_DATA_ENTRY_TYPE_UNSET)
	)
	{
		if(entry->custom_write)
		{
			entry->custom_write(entry, entry->custom_write_params);
		}
	}

	// run custom post function if set
	if(entry->custom_post)
	{
		entry->custom_post(entry, entry->custom_post_params);
	}

	// return indicating success
	return ERR_SUCCESS;

}


// activates the next entry in the data list
BP_ERROR_T BP_DataListActivateNextEntry(P_BP_DATA_LIST data_list)
{

	// ensure we have a data list
	if(!data_list)
		return ERR_FAILURE;

	// ensure we have entries

	if(!data_list->entries_n)
		return ERR_FAILURE;

	// ensure we have not hit the top
	if(data_list->last_entry_activated >= data_list->entries_n)
	{
		return ERR_FAILURE;
	}

	// increment the entry position
	data_list->last_entry_activated++;

	// activate the entry
	BP_DataListActivateEntry(data_list, &data_list->entries[data_list->last_entry_activated-1]);

	// return indicating success
	return ERR_SUCCESS;

}

// activates all entries in a list, starting with the first
BP_ERROR_T BP_DataListActivateAllEntries(P_BP_DATA_LIST data_list)
{

	// ensure we have a data list item
	if(!data_list)
		return ERR_FAILURE;

	// ensure we have some entries in the list
	if(!data_list->entries_n)
		return ERR_FAILURE;

	// reset last entry activation
	data_list->last_entry_activated = 0;

	// activate all entries in the list
	while(BP_DataListActivateNextEntry(data_list) == ERR_SUCCESS)
	{

	}

	// return indicating success
	return ERR_SUCCESS;

}

// Displays the current data list
BP_ERROR_T BP_DisplayDataList(P_BP_DATA_LIST data_list)
{

	// ensure we have a datalist pointer
	if(!data_list)
		return ERR_FAILURE;

	// Display Head Information First:
	printf("\n\t [+] Displaying Data List %p", data_list);
	printf("\n");
	printf("\n\t\t tq:   %p", data_list->tq);
	printf("\n\t\t desc: %s", data_list->desc);
	printf("\n\t\t uuid: %s", data_list->uuid);

	// now display individual elements
	size_t n = 0;
	if(data_list->entries_n)
	{

		// add a new line
		printf("\n");

		// walk entries and display
		for(; n < data_list->entries_n; n++)
		{

			// display entry
			BP_DisplayDataListEntry(&data_list->entries[n]);
			printf("\n");

		}

	}

	// add trailing newlines
	printf("\n\n");

	// return indicating failure
	return ERR_FAILURE;

}


// displays a single datalist entry
BP_ERROR_T BP_DisplayDataListEntry(P_BP_DATA_LIST_ENTRY entry)
{

	// ensure we have an entry
	if(!entry)
		return ERR_FAILURE;

	// attempt to resolve the type string
	char *type_str = BP_DataListGetTypeString(entry->data_type);

	// display entry
	printf("\n\t\t [+] Data List Entry (%p)", entry);
	printf("\n\t\t\t description:      %s", entry->description ? entry->description : "(unset)");
	printf("\n\t\t\t data pointer:     %p", entry->data);
	printf("\n\t\t\t data size:        %u", entry->size);
	printf("\n\t\t\t data type:        %s", type_str);
	printf("\n\t\t\t prepare routine:  %p", entry->custom_prepare);
	printf("\n\t\t\t prepare params:   %p", entry->custom_prepare_params);
	printf("\n\t\t\t read routine:     %p", entry->custom_read);
	printf("\n\t\t\t read params:      %p", entry->custom_read_params);
	printf("\n\t\t\t write routine:    %p", entry->custom_write);
	printf("\n\t\t\t write params:     %p", entry->custom_write_params);
	printf("\n\t\t\t post routine:     %p", entry->custom_post);
	printf("\n\t\t\t post params:      %p", entry->custom_post_params);
	printf("\n\t\t\t last read size:   %u", entry->last_read_size);
	printf("\n\t\t\t last write size:  %u", entry->last_write_size);



	// free string if set
	if(type_str)
		bpfree(type_str);

	// return indicating success
	return ERR_SUCCESS;

}

// This converts an entry type to a heap allocated string.  The
// returned string must be free()d.
char *  BP_DataListGetTypeString(BP_DATA_ENTRY_TYPE entry_type)
{

	// switch on the entry type
	switch(entry_type)
	{


		// signed 8 bit integer
		case BP_DATA_ENTRY_TYPE_INT8:
			return strdup("BP_DATA_ENTRY_TYPE_INT8");
			break;

		// signed 16 bit integer
		case BP_DATA_ENTRY_TYPE_INT16:
			return strdup("BP_DATA_ENTRY_TYPE_INT16");
			break;

		// signed 32 bit integer
		case BP_DATA_ENTRY_TYPE_INT32:
			return strdup("BP_DATA_ENTRY_TYPE_INT32");
			break;

		// signed 64 bit integer
		case BP_DATA_ENTRY_TYPE_INT64:
			return strdup("BP_DATA_ENTRY_TYPE_INT64");
			break;

		// unsigned 8 bit integer
		case BP_DATA_ENTRY_TYPE_UINT8:
			return strdup("BP_DATA_ENTRY_TYPE_UINT8");
			break;

		// unsigned 16bit integer
		case BP_DATA_ENTRY_TYPE_UINT16:
			return bpstrdup("BP_DATA_ENTRY_TYPE_UINT16");
			break;

		// unsigned 32bit integer
		case BP_DATA_ENTRY_TYPE_UINT32:
			return bpstrdup("BP_DATA_ENTRY_TYPE_UINT32");
			break;

		// unsigned 64bit integer
		case BP_DATA_ENTRY_TYPE_UINT64:
			return bpstrdup("BP_DATA_ENTRY_TYPE_UINT64");
			break;

		// arbitrary binary entry
		case BP_DATA_ENTRY_TYPE_BINARY:
			return bpstrdup("BP_DATA_ENTRY_TYPE_BINARY");
			break;

		// ascii entry
		case BP_DATA_ENTRY_TYPE_ASCII:
			return bpstrdup("BP_DATA_ENTRY_TYPE_ASCII");
			break;

		// format specifiers
		case BP_DATA_ENTRY_TYPE_FORMAT:
			return bpstrdup("BP_DATA_ENTRY_TYPE_FORMAT");
			break;

		// random data
		case BP_DATA_ENTRY_TYPE_RANDOM:
			return bpstrdup("BP_DATA_ENTRY_TYPE_RANDOM");
			break;

		// arbitrary sequence data
		case BP_DATA_ENTRY_TYPE_SEQUENCE:
			return bpstrdup("BP_DATA_ENTRY_TYPE_SEQUENCE");
			break;

		// read entry
		case BP_DATA_ENTRY_TYPE_READ:
			return bpstrdup("BP_DATA_ENTRY_TYPE_READ");
			break;

		// if the value is unset, state it here
		case BP_DATA_ENTRY_TYPE_UNSET:
			return bpstrdup("BP_DATA_ENTRY_TYPE_UNSET");
			break;

		default:
			return bpstrdup("Uknown/Bad entry type");
			break;
	}

	// should never get here
	return NULL;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Old Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/*
// Creates a new datalist and sets appropriate pointers if specified
// type defaults to BP_DATA_LIST_TYPE_ASCII if given a NULL parameter.
P_DATA_LIST NewDataList(P_DATA_LIST first, P_DATA_LIST next, P_DATA_LIST prev, int type)
{
	
	// allocate and zero out new list
	P_DATA_LIST newList = (P_DATA_LIST) bpcalloc(sizeof(DATA_LIST), 1);

	// return failure if we cannot create a new list element
	if(!newList)
		return NULL;

	// first set the specified datatype, default is ascii
	if(type != 0x00)
	{
		newList->dataType = type;
	}
	else
	{
		newList->dataType = BP_DATA_LIST_TYPE_ASCII;
	}
	
	// set the first element to the first argument if its not null
	// if not, set it to the newly allocated list pointer.
	if(first != NULL)
	{
		newList->first = first;
	}
	else
	{
		newList->first = newList;
	}
		
	// set the next element to the next argument if its not null
	// if it is null, set it to null to specify no next argument
	if(next != NULL)
	{
		newList->next = next;
	}
	else
	{
		newList->next = (P_DATA_LIST) NULL;
	}
	
	// set the prev element to the prev argument if its not null
	// if it is null, set it to null to specify no previous argument
	if(prev != NULL)
	{
		newList->prev = prev;
	}
	else
	{
		newList->prev = NULL;
	}

	// return the newly linked/created list
	return newList;

}





// Starts at the first pointer and destroys a list by bpfree'ing it
int DestroyDataList(P_DATA_LIST inList)
{
	
	if(inList == NULL)
		return LIST_ERROR;

	P_DATA_LIST nextList;
	
	// note: this function is perfect, dont mess with it
	for(inList = inList->first; inList != NULL;)
	{

		if(inList->next != NULL)
		{

			nextList = inList->next;

			if(inList->readData != NULL)
				bpfree(inList->readData);
			
			bpfree(inList);
			inList = nextList;

		}
		else
		{

			if(inList->readData != NULL)
				bpfree(inList->readData);
			
			bpfree(inList);
			break;

		}

	}
		
	return ERR_SUCCESS;
}




// -------------------------- SETATTRS --------------------------------------//

// changes the first, next, or prev (or all 3) pointers in inList to the values
// specified.
int DataListConfig(P_DATA_LIST inList, P_DATA_LIST first, P_DATA_LIST next, P_DATA_LIST prev, int type)
{
	
	// first set the specified datatype, default is ascii
	if(type != (int) 0x00){
		inList->dataType = type;
	}
	
	// set the first element to the first argument if its not null
	if(first != NULL){
		inList->first = first;
	} 
		
	// set the next element to the next argument if its not null
	if(next != NULL){
		inList->next = next;
	}
	
	// set the prev element to the prev argument if its not null
	if(prev != NULL){
		inList->prev = prev;
	}
		
	return 0;
}




// ------------------ GENERAL UTILITY FUNCTIONS ---------------------------//

// this takes a list and resets all first pointers to the value of argument
// "first", it then traverses the list from its original origin until it finds
// the new first element, bpfreeing all structures previous to that first element.
// trouble may arise on the account that first is not found, will test later.
int DataListSetAllFirst(P_DATA_LIST inList, P_DATA_LIST first)
{

	P_DATA_LIST start = inList->first;
	P_DATA_LIST placeHolder;
	
	// set all firsts to the new first pointer
	for
	(
			inList = inList->first;
			inList != NULL;
			inList = inList->next
	)
	{
		DataListConfig(inList, first, NULL, NULL, 0);
	}
	
	// start at the previous beginning of the list
	// bpfree any elements until you find your first
	for
	(
		;
		start != first;
		start = start->next
	)
	{
		placeHolder = start;
		start = start->next;
		bpfree(placeHolder);
	}
	
	return 0;
}




// Appends one list to another by resetting all first elements after join
int DataListAppendList(P_DATA_LIST inList, P_DATA_LIST appendList)
{

	inList = DataListGetLastElement(inList);
	
	// set the last element's next pointer to the first in our appendList
	inList->next = appendList->first;
	
	// now reset all pointers to the proper first pointer
	DataListSetAllFirst(inList, inList->first);
	
	// attempt to reset previous pointers by directional
	for
	(
			inList = inList->first;
			inList != NULL;
			inList = inList->next
	)
	{

		if(inList == inList->first)
			inList->prev = NULL;
		
		if(inList->next != NULL)
		{
			inList->next->prev = inList;
		}

	}

	return 0;
}


// returns a pointer to the last pointer element in a DATA_LIST
P_DATA_LIST DataListGetLastElement(P_DATA_LIST inList)
{

	for
	(
		inList = inList->first;
		inList->next != NULL;
		inList = inList->next
	)
	{
	}

	return inList;

}

// ----------------------------- PRINT FUNCTIONS -------------------------//

// Prints Out All Elements In A Data List Starting at inList Argument
int DataListPrintAll(P_DATA_LIST inList)
{

	int listNbr;
	
	for
	(
			listNbr = 0;
			inList != NULL;
			inList = inList->next
	)
	{

		printf("\n List Element Number: %i", listNbr);
		DataListPrintElement(inList);
		listNbr++;

	}
	
	return 0;

}

// Prints One Element Of A Data List (Pointers Only)
int DataListPrintElement(P_DATA_LIST inList)
{

	printf("\n--------------------------------\n");
	printf("\n\t List Element: %p", inList);
	printf("\n\t\t Data Size:    %i", inList->dataSize);
	
	switch(inList->dataType){
		case BP_DATA_LIST_TYPE_INTEGER:
			printf("\n\t\t Data is of type: INTEGER");
			printf("\n\t\t Data Pointer: %p", &inList->data);
			break;
		case BP_DATA_LIST_TYPE_ASCII:
			printf("\n\t\t Data is of type: BP_DATA_LIST_TYPE_ASCII");
			printf("\n\t\t Data Pointer: %p", &inList->data);
			break;
		case BP_DATA_LIST_TYPE_RANDOM:
			printf("\n\t\t Data is of type: RANDOM");
			printf("\n\t\t Data Pointer: %p", &inList->data);
			break;
		case BP_DATA_LIST_TYPE_FORMAT:
			printf("\n\t\t Data is of type: BP_DATA_LIST_TYPE_FORMAT");
			printf("\n\t\t Data Pointer: %p", &inList->data);
			break;
		case BP_DATA_LIST_TYPE_READ:
			printf("\n\t\t Data is of type: BP_DATA_LIST_TYPE_READ");
			printf("\n\t\t Data Pointer: %p", inList->readData);
			printf("\n\t\t Buffer Content:");
			PrintBuffer(inList->readData, inList->dataSize);
			break;
		case BP_DATA_LIST_TYPE_SEQUENCE:
			printf("\n\t\t Data is of type: BP_DATA_LIST_TYPE_SEQUENCE");
			printf("\n\t\t Data Pointer: %p", &inList->data);
			break;
		case BP_DATA_LIST_TYPE_BINARY:
			printf("\n\t\t Data is of type: BP_DATA_LIST_TYPE_BINARY");
			printf("\n\t\t Binary Data Pointer: %p", &inList->binData);
			printf("\n\t\tBuffer Content:\n");
			PrintBuffer(inList->binData, inList->dataSize);
			break;
			
		default:
			printf("\n\t\t Data is of type: UNSUPPORTED DATA TYPE");
			break;
	}
	
	printf("\n\t\t Ammount of data in list element data buffer: %i", inList->dataSize);
	

	printf("\n -- End Buffer Content\n");
	printf("\n\t\t\t List First Pointer: %p", inList->first);
	printf("\n\t\t\t List Next  Pointer: %p", inList->next);
	printf("\n\t\t\t List Prev  Pointer: %p", inList->prev);	
	printf("\n");

	return 0;

}


// ----------------------- DATA LIST FROM * FUNCTIONS --------------------------------//

// creates a new DATA_LIST struct from a string value
P_DATA_LIST DataListFromString(char *string)
{

	// create new outlist
	P_DATA_LIST outList = (P_DATA_LIST) NULL;
	
	// set size
	int size = strnlen(string, LARGEST_SIGNED_INTEGER-1);

	// set byte position
	int bytePlace = 0;

	// set chunk count
	int chunks = 0;
	
	if(size > MAX_DATA_BLOCK_SIZE)
		chunks = size / MAX_DATA_BLOCK_SIZE;
	
	//now you need to chunk up data and store them in the list
	if(chunks > 0){
		
		// bytePlace is used to know where we were during
		// copying.  So every loop it increments by the MAX_DATA_BLOCK_SIZE
		// and copys from there into a new block.
		for(bytePlace = 0; chunks != -2; chunks--){
			
			if(outList == NULL){
				outList = NewDataList(NULL, NULL, NULL, BP_DATA_LIST_TYPE_ASCII);
			} else {
				outList->next = NewDataList(outList->first, NULL, outList, BP_DATA_LIST_TYPE_ASCII);
				outList = outList->next;
			}
			
			// primary bytePlace usage
			if(strnlen(&string[bytePlace], LARGEST_SIGNED_INTEGER) > MAX_DATA_BLOCK_SIZE){
				memcpy(&outList->data, &string[bytePlace], MAX_DATA_BLOCK_SIZE);
				outList->dataSize = MAX_DATA_BLOCK_SIZE;
			} else {
				
				strncpy(outList->data, &string[bytePlace], MAX_DATA_BLOCK_SIZE);
				outList->dataSize = strnlen(&string[bytePlace], MAX_DATA_BLOCK_SIZE);
				break;
			}
			bytePlace += MAX_DATA_BLOCK_SIZE;
			
		}
		
	} else {
		// if there is only one chunk, allocate it simply with a strncpy
		outList = NewDataList(NULL, NULL, NULL, BP_DATA_LIST_TYPE_ASCII);
		strncpy(outList->data, string, MAX_DATA_BLOCK_SIZE);
		outList->dataSize = strnlen(string, MAX_DATA_BLOCK_SIZE);
	}
	
	return outList->first;
}



// Creates a new DATA_LIST from a existing list and returns it
P_DATA_LIST DataListDup(P_DATA_LIST inList)
{

	P_DATA_LIST newList = NULL;
	
	for(inList = inList->first; inList != NULL; inList = inList->next)
	{

		if(newList == NULL)
		{
			newList = NewDataList(NULL, NULL, NULL, inList->dataType);
			memcpy(newList->data, inList->data, MAX_DATA_BLOCK_SIZE);
			newList->dataSize = inList->dataSize;
		}
		else
		{
			newList->next = NewDataList(newList->first, NULL, newList, inList->dataType);
			newList = newList->next;
			memcpy(newList->data, inList->data, MAX_DATA_BLOCK_SIZE);
			newList->dataSize = inList->dataSize;
		}

	}
		
	return newList->first;
}

// function to destroy a read block (needs to be fixed)
int DestroyReadBlock(P_READBLOCK readBlock)
{

	if(readBlock == NULL)
		return ERR_FAILURE;
	
	if(readBlock->data == NULL){
		bpfree(readBlock);
		return ERR_FAILURE;
		
	} else
		bpfree(readBlock->data);
	bpfree(readBlock);
	
	return ERR_SUCCESS;
}
*/

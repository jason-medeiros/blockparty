
// arbitrary list error
#define LIST_ERROR -1

// Data entry type
typedef size_t BP_DATA_ENTRY_TYPE;


// this enum describes all
// data types supported by the
// DATA_LIST structure.
typedef enum {

	BP_DATA_ENTRY_TYPE_UNSET,
	BP_DATA_ENTRY_TYPE_INT8,
	BP_DATA_ENTRY_TYPE_INT16,
	BP_DATA_ENTRY_TYPE_INT32,
	BP_DATA_ENTRY_TYPE_INT64,
	BP_DATA_ENTRY_TYPE_UINT8,
	BP_DATA_ENTRY_TYPE_UINT16,
	BP_DATA_ENTRY_TYPE_UINT32,
	BP_DATA_ENTRY_TYPE_UINT64,
	BP_DATA_ENTRY_TYPE_BINARY,
	BP_DATA_ENTRY_TYPE_ASCII,
	BP_DATA_ENTRY_TYPE_FORMAT,
	BP_DATA_ENTRY_TYPE_RANDOM,
	BP_DATA_ENTRY_TYPE_SEQUENCE,
	BP_DATA_ENTRY_TYPE_READ

} BP_DATA_ENTRY_TYPE_ENUM;

// chunk/entry information
typedef struct __BP_DATA_LIST_ENTRY {

	// Description of entry
	char *description;

	// Data here
	unsigned char *data;

	// Size of current data
	size_t size;

	// Data type associated with this entry
	BP_DATA_ENTRY_TYPE data_type;

	// P_BP_DATA_LIST which this item belongs to
	void * data_list;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Activation Flags %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// this flag can be set to either BP_TRUE or BP_FALSE and determines
	// whether or not this entry is enabled for activation.  If it is not
	// enabled, this entry is skipped over to the next.
	uint8_t activation_enabled;

	// This flag is set if this chunk was the last chunk encountered before
	// a data lists execution chain is administratively disabled.
	uint8_t activation_administratively_prevented;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Custom Dispatching %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// parameters for the prepare routine
	void *custom_prepare_params;

	// parameters for the read routine
	void *custom_read_params;

	// parameters for the write routine
	void *custom_write_params;

	// custom post parameters
	void *custom_post_params;

	// This routine can be set as a custom prepare function, which will automatically
	// prepare this element. Function parameters should always be in the form of :
	//
	// 	custom_function(P_BP_DATA_LIST_ENTRY, opaque pointer to your type);
	//
	void (* custom_prepare)(void *, void *);

	// This routine can be set to a custom write routine
	void (* custom_write)(void *, void *);

	// This routine can be set to a custom read routine
	void (* custom_read)(void *, void *);

	// This routine is run after either the read or
	// write has finished.
	void (* custom_post)(void *, void *);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Activation Statistics %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the last read size in bytes (set by callback)
	size_t last_read_size;

	// set the last write size (set by callback)
	size_t last_write_size;

	// Set to either ERR_SUCCESS or ERR_FAILURE determined by if the last
	// prepare routine execution was successful.
	BP_BOOL last_prepare_successful;

	// Set to either ERR_SUCCESS or ERR_FAILURE determined by if the last
	// write routine execution was successful.
	BP_BOOL last_write_successful;

	// Set to either ERR_SUCCESS or ERR_FAILURE determined by if the last
	// read routine execution was successful.
	BP_BOOL last_read_successful;

	// Set to either ERR_SUCCESS or ERR_FAILURE determined by if the last
	// post routine execution was successful.
	BP_BOOL last_post_successful;

} BP_DATA_LIST_ENTRY, *P_BP_DATA_LIST_ENTRY;



// structure used for holding arbitrary data lists.  This list
// can be acted on and "Played" through a variety of different
// mechanisms (socket/filedescriptor/fifo/etc).
typedef struct __BP_DATA_LIST {
	

	// tail queue associated with this structure
	P_BP_LINKL tq;

	// description of this data list
	char *desc;

	// uuid for this data list (generated at
	// run time automatically)
	char *uuid;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Activation Flags %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// this flag is either BP_TRUE or BP_FALSE and is set by callbacks
	// to prevent further activations.
	uint8_t prevent_activations;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Individual Entries %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// entries in the data list
	P_BP_DATA_LIST_ENTRY entries;
	
	// number of entries in the list
	size_t entries_n;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Activation References %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// last entry activated
	size_t last_entry_activated;

} BP_DATA_LIST,  *P_BP_DATA_LIST;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Data List Operation %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// creates a new data list entry
P_BP_DATA_LIST BP_CreateDataList(char * description);

// destroy the data list
BP_ERROR_T BP_DestroyDataList(P_BP_DATA_LIST data_list);

// Add a new element at the end of the list
BP_ERROR_T BP_DataListAddEntry
(
	P_BP_DATA_LIST     data_list,
	char *description,
	unsigned char *    data,
	size_t             data_size,
	BP_DATA_ENTRY_TYPE data_type,
	void (* custom_prepare)(void *, void *),
	void * custom_prepare_param,
	void (* custom_read)(void *, void *),
	void * custom_read_param,
	void (* custom_write)(void *, void *),
	void * custom_write_param,
	void (* custom_post)(void *, void *),
	void * custom_post_param
);

// Displays the current data list
BP_ERROR_T BP_DisplayDataList(P_BP_DATA_LIST data_list);

// displays a single datalist entry
BP_ERROR_T BP_DisplayDataListEntry(P_BP_DATA_LIST_ENTRY entry);

// activates a data list entry by calling relevant callbacks
// within the datalist.
BP_ERROR_T BP_DataListActivateEntry(P_BP_DATA_LIST data_list, P_BP_DATA_LIST_ENTRY entry);

// activates the next entry in the data list
BP_ERROR_T BP_DataListActivateNextEntry(P_BP_DATA_LIST data_list);

// activates all entries in a list, starting with the first
BP_ERROR_T BP_DataListActivateAllEntries(P_BP_DATA_LIST data_list);

// This converts an entry type to a heap allocated string.  The
// returned string must be free()d.
char *  BP_DataListGetTypeString(BP_DATA_ENTRY_TYPE entry_type);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Old Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/*

// creates a new data list
P_DATA_LIST NewDataList(P_DATA_LIST first, P_DATA_LIST next, P_DATA_LIST prev, int type);

// destroys a data list starting at first element
int DestroyDataList(P_DATA_LIST inList);

// reconfigures inList with the given parameters, NULL values are left unchanged
int DataListConfig(P_DATA_LIST inList, P_DATA_LIST first, P_DATA_LIST next, P_DATA_LIST prev, int type);

// sets all pointers in a list to the value of first
int DataListSetAllFirst(P_DATA_LIST inList, P_DATA_LIST first);

// appends two datalists
int DataListAppendList(P_DATA_LIST inList, P_DATA_LIST appendList);

// gets the last pointer in a datalist
P_DATA_LIST DataListGetLastElement(P_DATA_LIST inList);

// prints out all elements in a list starting at inList (does not change first pointer)
int DataListPrintAll(P_DATA_LIST inList);

// prints out one element in a list
int DataListPrintElement(P_DATA_LIST inList);

// creates a datalist from a string
P_DATA_LIST DataListFromString(char *string);

// duplicates a datalist returning a new datalist with the same data/int values
P_DATA_LIST DataListDup(P_DATA_LIST inList);

// destroys a read block
int DestroyReadBlock(P_READBLOCK readBlock);


*/

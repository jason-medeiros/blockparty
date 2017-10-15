

#define LIST_ERROR -1

// this enum describes all
// data types supported by the
// DATA_LIST structure.
enum {  UNSUPPORTED,
		INTEGER,
		ASCII,
		FORMAT,
		RANDOM,
		SEQUENCE,
		READ,
		BINARY,
};


// this structure is used for handling data
// inside our Block class.  Never send more
// then MAX_DATA_BLOCK_SIZE bytes at a time 
// out of any given list element.
#define LARGEST_SIGNED_INTEGER 2147483647
#define MAX_DATA_BLOCK_SIZE 200

typedef struct _DATA_LIST {
	char data[MAX_DATA_BLOCK_SIZE]; // soon to be depreciated
	char *readData;
	char *binData;
	int dataType;
	int dataSize;
	
	int isExploiting;
	
	struct _DATA_LIST * first;
	struct _DATA_LIST * next;
	struct _DATA_LIST * prev;
} DATA_LIST, *P_DATA_LIST;

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

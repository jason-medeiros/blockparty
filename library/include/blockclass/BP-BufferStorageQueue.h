/***************************************************************************
 *            BP-BufferStorageQueue.h
 *
 *  Fri Nov 17 21:51:12 2006
 *  Copyright  2006  User
 *  Email
 ****************************************************************************/

#define QUEUE_ERROR -1

// buffer list queue, used for storage of buffer lists
typedef struct _BP_BUFFER_QUEUE {
	P_DATA_LIST bufferList;
	pthread_mutex_t lock;
	
	struct _BP_BUFFER_QUEUE * first;
	struct _BP_BUFFER_QUEUE * next;
	struct _BP_BUFFER_QUEUE * prev;
} BP_BUFFER_QUEUE, *P_BP_BUFFER_QUEUE;

// debug queue initializer
P_BP_BUFFER_QUEUE BufferQueueInit();

// push a new BUFFER_QUEUE struct onto the bufferList queue (copies datalist, not referenced)
int BufferQueuePush(P_BP_BUFFER_QUEUE queue, P_DATA_LIST debugInfo);

// get the next bufferList from the buffer queue 
P_DATA_LIST BufferQueueGet(P_BP_BUFFER_QUEUE queue);

// pops queue element off the fifo queue structures head by free()'ing it
// and returns the next queue element
P_BP_BUFFER_QUEUE BufferQueuePop(P_BP_BUFFER_QUEUE queue);

// Prints an entire queue (using BufferQueuePrintElement)
void BufferQueuePrintAll(P_BP_BUFFER_QUEUE queue);

// Prints a single element in a buffer queue 
void BufferQueuePrintElement(P_BP_BUFFER_QUEUE element);

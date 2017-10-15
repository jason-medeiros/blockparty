#include "../../include/BP-Main.h"

// Copies a datalist, inList, and pushes its first pointer into our
// buffer queue.
int BufferQueuePush(P_BP_BUFFER_QUEUE queue, P_BP_DATA_LIST inList)
{
	

	return ERR_SUCCESS;
}


// gets the next debugInfo structure from the list and bpfree's the 
// queue element, setting the queue pointer to the next element
P_BP_DATA_LIST BufferQueueGet(P_BP_BUFFER_QUEUE queue){
	

	
	// error if we get here
	return NULL;
}


// Creates a new queue
P_BP_BUFFER_QUEUE BufferQueueInit()
{
	
	// allocate new queue
	P_BP_BUFFER_QUEUE queue = NULL;
	
	// return the queue
	return queue;
}


// Destroys a buffer queue and all data in it 
int BufferQueueDestroy(P_BP_BUFFER_QUEUE queue)
{

	// return indicating success
	return ERR_SUCCESS;
	
}

// Destroys a single buffer queue element 
P_BP_BUFFER_QUEUE BufferQueueDestroyElement(P_BP_BUFFER_QUEUE queue)
{


	// return the filled out queue
	return queue;

}

// sets buffer queues list configuration parameters individually
// set them to NULL 
int BufferQueueConfig
(
	P_BP_BUFFER_QUEUE queue,
	P_BP_BUFFER_QUEUE first,
	P_BP_BUFFER_QUEUE next, P_BP_BUFFER_QUEUE prev
)
{

		
	// return indicating success
	return ERR_SUCCESS;
								
}

// starting from queue, set all elements in a list to the specified pointers
// (this function is primarily used for resetting all first pointers when a list
// is popped or modified in some way.  Extra functionality for bonus points.)
int BufferQueueConfigAll
(
		P_BP_BUFFER_QUEUE queue,
		P_BP_BUFFER_QUEUE first,
		P_BP_BUFFER_QUEUE next,
		P_BP_BUFFER_QUEUE prev
)
{

	return ERR_FAILURE;

}

// Function will bpfree the last end of a queue, and return the next element
// in the queue, this is unlike the destroy series of functions in that this
// function does not operate on the datalist.
P_BP_BUFFER_QUEUE BufferQueuePop(P_BP_BUFFER_QUEUE queue)
{
	
	
	
	// if its null, return null
	return NULL;

}

// prints out a buffer queue element
void BufferQueuePrintElement(P_BP_BUFFER_QUEUE element)
{


}

// prints out all elements in a buffer queue
void BufferQueuePrintAll(P_BP_BUFFER_QUEUE queue)
{


}

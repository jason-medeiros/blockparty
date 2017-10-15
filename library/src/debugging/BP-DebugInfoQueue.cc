

#include "../../include/BP-Main.h"

#if USE_BLOCKPARTY_DEBUGGING == 1

/*
 *
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%% 64Bit Debugging Interface %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 *
 */

#if BLOCKPARTY_DEBUGGING_TARGET_64BIT == 1



#endif // #if BLOCKPARTY_DEBUGGING_TARGET_64BIT == 1


/*
 *
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%% 32Bit Debugging Interface %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 *
 */


#if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1

// Pushes a new element into the queue
int DebugQueuePush(P_BP_DEBUG_QUEUE queue, P_RDEBUG_INFO debugInfo){
	
	P_BP_DEBUG_QUEUE next;
	
	if(queue->debugInfo == NULL){
		queue->debugInfo = debugInfo;
		return 0;
	} else {
		next = DebugQueueInit();
		next->first = queue->first;
		next->prev  = queue;
		next->next  = NULL;
		next->debugInfo = debugInfo;
		
		queue->next = next;
	}
	return 0;
}

// gets the next debugInfo structure from the list
P_RDEBUG_INFO DebugQueueGet(P_BP_DEBUG_QUEUE queue){
	P_RDEBUG_INFO debugInfo;
	
	if(queue->next == NULL){
		return queue->debugInfo;
	} else {
		queue = queue->next;
		debugInfo = queue->prev->debugInfo;
		bpfree(queue->prev);
		queue->prev = NULL;
		return debugInfo;
	}
	
	return 0;
}

// Creates a new queue
P_BP_DEBUG_QUEUE DebugQueueInit(){
	
	P_BP_DEBUG_QUEUE queue = (P_BP_DEBUG_QUEUE) bpmalloc(sizeof(BP_DEBUG_QUEUE));
	queue->debugInfo = NULL;
	
	queue->next = NULL;
	queue->prev = NULL;
	queue->first = queue;
	
	return queue;
}

#endif

#endif // USE_BLOCKPARTY_DEBUGGING == 1

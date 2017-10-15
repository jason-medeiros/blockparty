/*
 * Blockparty Debugging Queue:
 * ----------------------------------------------
 * Used as a blockparty data queue structure/backend.
 *
 */

#ifndef BP_DEBUG_INFO_QUEUE_H
#define BP_DEBUG_INFO_QUEUE_H

#if USE_BLOCKPARTY_DEBUGGING == 1

// debug info queue structure used by the TAP listener
typedef struct _BP_DEBUG_QUEUE {
	P_RDEBUG_INFO debugInfo;
	pthread_mutex_t lock;
	
	struct _BP_DEBUG_QUEUE * first;
	struct _BP_DEBUG_QUEUE * next;
	struct _BP_DEBUG_QUEUE * prev;
		
} BP_DEBUG_QUEUE, *P_BP_DEBUG_QUEUE;

// debug queue initializer
P_BP_DEBUG_QUEUE DebugQueueInit();

// push a new debugInfo struct onto the debugInfo queue
int DebugQueuePush(P_BP_DEBUG_QUEUE queue, P_RDEBUG_INFO debugInfo);

// get the next element from the debugInfo queue and free queue element
// debugInfo is not free'd by this function, only the queue element.
P_RDEBUG_INFO DebugQueueGet(P_BP_DEBUG_QUEUE queue);

#endif // USE_BLOCKPARTY_DEBUGGING == 1

#endif BP_DEBUG_INFO_QUEUE_H

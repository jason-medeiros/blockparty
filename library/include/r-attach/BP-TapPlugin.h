#ifdef BP_USE_REMOTE_ATTACH_PLUGIN
#ifndef BP_TAP_PLUGIN_H
#define BP_TAP_PLUGIN_H

#if USE_BLOCKPARTY_DEBUGGING == 1

// this is the remote debug structure that will be passed over the network
// from our tap.  This is only used if you implement the Tap functionality

typedef struct _TAP_PARAMETERS {
	P_BP_DEBUG_QUEUE queue;
	short port;
} TAP_PARAMETERS, *P_TAP_PARAMETERS;


// starts the threaded listener
int StartListener(short port, P_BP_DEBUG_QUEUE queue);

// used as a pthread interface to Tap
void * ThreadListener(void * params);

// function to actually start the listener from inside the thread
int TapListener(short port, P_BP_DEBUG_QUEUE queue);

// prints out register debug information
void PrintDebugInfo(P_RDEBUG_INFO debugInfo);

#endif // USE_BLOCKPARTY_DEBUGGING == 1

#endif // BP_TAP_PLUGIN_H
#endif // BP_USE_REMOTE_ATTACH_PLUGIN

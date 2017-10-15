/***************************************************************************
 *            BP-TapPlugin.c
 *
 *  Thu Nov 16 14:05:56 2006
 *  Copyright  2006  User
 *  Email
 ****************************************************************************/

#include "../../include/BP-Main.h"


#ifdef BP_USE_REMOTE_ATTACH_PLUGIN
#include <linux/user.h>

// Starts a listner port for communication with the tap daemon
int StartListener(short port, P_BP_DEBUG_QUEUE queue){
	pthread_t thread;
	pthread_attr_t threadAttr;
	P_TAP_PARAMETERS inParams = (P_TAP_PARAMETERS) bpmalloc(sizeof(TAP_PARAMETERS));

	inParams->port = port;
	inParams->queue = queue;

	pthread_create(&thread, &threadAttr, ThreadListener, inParams);

	return 0;
}

// Void prototype to tap listener to make it usable with pthreads
void * ThreadListener(void * params){
	P_TAP_PARAMETERS inParams = (P_TAP_PARAMETERS) params;
	TapListener(inParams->port, inParams->queue);
	return 0;
}

// Creates a listener port to communicate with a tap interface
int TapListener(short port, P_BP_DEBUG_QUEUE queue){

	P_NETWORK listenNet = (P_NETWORK) bpmalloc(sizeof(NETWORK));
	int optionFlag = 0;
	size_t structLength = sizeof(struct sockaddr);

	P_RDEBUG_INFO debugInfo = (P_RDEBUG_INFO) bpmalloc(sizeof(RDEBUG_INFO));


	listenNet->sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(listenNet->sock < 0)
		return -1;

	if(setsockopt(listenNet->sock, IPPROTO_TCP, TCP_NODELAY, (char *) &optionFlag, sizeof(optionFlag)) == -1)
		return -1;

	// set up listening socket
	listenNet->source.sin_addr.s_addr = INADDR_ANY;
	listenNet->source.sin_port        = htons(port);

	// fill in destination struct
	listenNet->dest.sin_addr.s_addr   = INADDR_ANY;
	listenNet->dest.sin_port          = 0;

	// bind
	if(bind(listenNet->sock, (struct sockaddr *) &listenNet->source, sizeof(struct sockaddr)) == -1)
		return 0;

	// listen with a default backlog of 5
	if(listen(listenNet->sock, 5) != 0)
		return 0;

	// it is not required to create a new socket as only one socket will
	// be communicating between the fuzzer and the tap
	listenNet->sock = accept(listenNet->sock, (struct sockaddr *) &listenNet->source, \
								&structLength);

	// now begin reading network structures
	while(1){
		if(NetRead(listenNet, (char *) &debugInfo->registers, sizeof(struct user_regs_struct)) == 0)
			return 0;

		if(NetRead(listenNet, (char *) &debugInfo->signal,    sizeof(int)) == 0)
			return 0;

		if(NetRead(listenNet, (char *) &debugInfo->stackSize, sizeof(int)) == 0)
			return 0;

		debugInfo->stackData = (char *) bpmalloc(debugInfo->stackSize);

			return 0;
		if(NetRead(listenNet, debugInfo->stackData, debugInfo->stackSize) == 0)

		debugInfo->signalDescription = (char *) bpmalloc(400);
		if(NetRead(listenNet, debugInfo->signalDescription, 399) == 0)
			return 0;


		if(queue == NULL){
			PrintDebugInfo(debugInfo);
		} else {
			DebugQueuePush(queue, debugInfo);
		}
	}

	return 0;
}


void PrintDebugInfo(P_RDEBUG_INFO debugInfo){

	printf("\n Struct Registers:");
	printf("\n\t eax: %p", (void *) debugInfo->registers.eax);
	printf("\n\t ebx: %p", (void *) debugInfo->registers.ebx);
	printf("\n\t ecx: %p", (void *) debugInfo->registers.ecx);
	printf("\n\t edx: %p", (void *) debugInfo->registers.edx);
	printf("\n\t ebp: %p", (void *) debugInfo->registers.ebp);
	printf("\n\t esp: %p", (void *) debugInfo->registers.esp);
	printf("\n\t EIP: %p", (void *) debugInfo->registers.eip);


	printf("\n\n\t\t Signal: %i",   debugInfo->signal);
	printf("\n\t\t Stack Data: %s", debugInfo->stackData);
	printf("\n\t\t Stack Size: %i", debugInfo->stackSize);
	printf("\n\t\t Signal Description: %s", debugInfo->signalDescription);
	printf("\n\n");
}
#endif //  BP_USE_REMOTE_ATTACH_PLUGIN

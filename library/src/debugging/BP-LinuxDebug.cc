
/***************************************************************************
 *            BP-LinuxDebug.cc
 *
 *  Fri Apr 20 18:12:36 2007
 *  Copyright  2007  User
 *  Email
 ****************************************************************************/
#include "../../include/BP-Main.h"

#ifdef __GNUC__
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

// ----------------------- FUNCTIONS TO UTILIZE DEBUGGING HANDLES -----------------------

// Parses a Character String (length) bytes long into a multidimensional
// character array.  Arg str is tokenized and returned. STRING IS NOT DUPLICATED.

char ** DebugParseArguments(char * argStr, int length){



	if(argStr == NULL)
		return NULL;

	if(length == 0)
		return NULL;


	// if the arguments contain sequential spaces, return null
	if(strstr(argStr, "  ") != NULL){
		return NULL;
	}


	char **ap, *argv[100], *inputstring = argStr;
	for(ap = argv; (*ap = strsep(&inputstring, " \t")) != NULL;){

	  if (**ap != '\0'){
	     if (++ap >= &argv[10])
	          break;
	  }
	}

	return argv;

}


// Attaches to a specified pid, stops the process and returns
// a debug handle.  Process must be continued to debug
// process.
P_DEBUG_HANDLE DebugAttachToPid(int pid){
	// cannot trace itself
	if(pid == 0)
		return NULL;

	P_DEBUG_HANDLE dbgHandle = (P_DEBUG_HANDLE) bpmalloc(sizeof(DEBUG_HANDLE));
		memset(dbgHandle, 0x00, sizeof(DEBUG_HANDLE));

	// set values
	dbgHandle->debugPid = pid;
	errno = 0;

	ptrace(PT_ATTACH, pid, NULL,NULL);
	dbgHandle->attached = 0;

	dbgHandle->attached = 1;

	return dbgHandle;
}

// Creates a process given a filesystem path, file arguments,
// specifying no passing values.  Process is stopped on creation
// and must be continued to for process to start.
P_DEBUG_HANDLE DebugCreateProcessAttach(char * path, char **args, char **environment){
	int parentOrChild  = 0;

	P_DEBUG_HANDLE dbgHandle = (P_DEBUG_HANDLE) bpmalloc(sizeof(DEBUG_HANDLE));
		memset(dbgHandle, 0x00, sizeof(DEBUG_HANDLE));


	parentOrChild = fork();
	if(parentOrChild == 0){
		ptrace(PT_TRACE_ME, 0, 0, 0);
		execv(path, args);
		exit(0);
	}

	// set values
	dbgHandle->debugPid = parentOrChild;
	dbgHandle->attached = 1;

	// attaching is unnecessary after a traceMe
	return dbgHandle;
}

// Continues a debug loop given a debug handle, breaking
// and stopping on a signal.  Upon signaling, the process
// has its registers retrieved and stored in its native
// debug handle, along with the signal that occured.
int DebugContinue(P_DEBUG_HANDLE dbgHandle){

	int status    = 0;
	int ptraceRet = 0;
	int retPid    = 0;

	// return null if the process is not attached/traced
	if(dbgHandle == NULL)
		return ERR_DBG_HANDLE_NULL;

	if(dbgHandle->attached < 1)
		return ERR_DBG_NOT_ATTACHED;

	// continues a ptrace of an application
	ptraceRet = ptrace(PTRACE_CONT, dbgHandle->debugPid, 0, 0);
	if(ptraceRet == -1){

		return ERR_DBG_CANNOT_CONTINUE_DEBUGGER;
	}

	// waits until the debugged application emits a signal
	retPid = waitpid(dbgHandle->debugPid, &status, 0);
	if(retPid != dbgHandle->debugPid)
		return ERR_DBG_WAIT_RETURNS_UNMATCHED_PID;

	// About: If an application is ptraced,
	// it will stop on the condition that a
	// signal occurs.  This is any signal that
	// the process emits.

	// if the process was stopped, store the signal
	if(WIFSTOPPED(status) > 0){
		dbgHandle->signal = WSTOPSIG(status);
		dbgHandle->sigDesc = L_ReturnStringFromSignal(dbgHandle);
		// since the process is stopped, it is now safe
		// to retrieve registers from the process.
		DebugGetRegisters(dbgHandle);

		return ERR_DBG_PROCESS_SIGNALED;
	} else {

		if(WIFEXITED(status) > 0){
			// if the process has exited normally
			// set the attached status to false.
			dbgHandle->attached = 0;
			return ERR_DBG_PROCESS_EXITED;
		}

		if(WIFSIGNALED(status) > 0){
			return ERR_DBG_PROCESS_SIGNALED;
		}
	}

	return ERR_FAILURE;
}

// Steps the debugger one step and returns
// conditions are not evaluated during stepping
int DebugStep(P_DEBUG_HANDLE dbgHandle){
	if(dbgHandle == NULL)
		return ERR_DBG_HANDLE_NULL;

	int ptraceReturn = ptrace(PT_STEP, dbgHandle->debugPid, NULL, NULL);
	if(ptraceReturn == ESRCH || ptraceReturn == EINVAL || \
	   ptraceReturn == EPERM || ptraceReturn == EBUSY){
	   return ERR_DBG_PTRACE_RETURNS_ERR;
	}

	return ERR_SUCCESS;
}


// Creates a stepping loop for `instructionCount` number of instructions
// evaluating the program state after each step.  This is very
// processor/resource intensive, but is very thorough. This
// function deals exclusively with the debug handles condition list,
// evaluating each condition per step.  The loop will exit immediately
// if there are no conditions to be evaluated.
//
// If a condition is raised, a pointer to it will be stored in the
// debug handles lastConditionRaised element (pointer, not a copy).
//
// Pass negative one into instructionCount to cause an infinate evaluation
// loop.
//
// Condition greater than less than parameters must be greater then
// zero to be evaluated by this function.

int DebugStepConditionLoop(P_DEBUG_HANDLE dbgHandle, int instructionCount){
	if(dbgHandle == NULL)
		return ERR_DBG_HANDLE_NULL;

	if(dbgHandle->conditions == NULL)
		return ERR_DBG_CONDITIONS_NULL;

	P_DEBUG_CONDITION_LIST first = dbgHandle->conditions->first;

	// main stepper loop
	for(int i = 0; i < instructionCount; i++){

		// step one step
		DebugStep(dbgHandle);

		// get new registers
		DebugGetRegisters(dbgHandle);

		// bpfree/refresh memory map
		if(dbgHandle->memory != NULL)
			DestroyMemMap(dbgHandle);
		GetMemMap(dbgHandle);

		// main condition evaluation loop, executed once per stepping
		for(dbgHandle->conditions = dbgHandle->conditions->first; \
			dbgHandle->conditions != NULL; \
			dbgHandle->conditions = dbgHandle->conditions->next){


			// -----------------------------------------------------
			// evaluate eax conditions
			if(dbgHandle->conditions->eaxSet > 0){

				// exact matching
				if(dbgHandle->eax == (size_t) dbgHandle->conditions->eax)
					dbgHandle->conditions->eaxMatch = 1;

				// Greater then matching
				if(dbgHandle->conditions->eaxGt != 0)
				if((size_t) dbgHandle->conditions->eaxGt < dbgHandle->eax){
					dbgHandle->conditions->eaxGtMatch = 1;
					dbgHandle->conditions->eaxMatch = 1;
				}

				// less then matching
				if(dbgHandle->conditions->eaxLt > 0)
				if((size_t) dbgHandle->conditions->eaxLt > dbgHandle->eax){
					dbgHandle->conditions->eaxLtMatch = 1;
					dbgHandle->conditions->eaxMatch = 1;
				}
				if(dbgHandle->conditions->eaxMatch > 0)
					dbgHandle->conditions->conditionReached = 1;
			}

			// -----------------------------------------------------
			// evaluate ebx conditions
			if(dbgHandle->conditions->ebxSet > 0){

				// exact matching
				if(dbgHandle->ebx == (size_t) dbgHandle->conditions->ebx)
					dbgHandle->conditions->ebxMatch = 1;

				// Greater then matching
				if(dbgHandle->conditions->ebxGt != 0)
				if((size_t) dbgHandle->conditions->ebxGt < dbgHandle->ebx){
					dbgHandle->conditions->ebxGtMatch = 1;
					dbgHandle->conditions->ebxMatch = 1;
				}

				// less then matching
				if(dbgHandle->conditions->ebxLt > 0)
				if((size_t) dbgHandle->conditions->ebxLt > dbgHandle->ebx){
					dbgHandle->conditions->ebxLtMatch = 1;
					dbgHandle->conditions->ebxMatch = 1;
				}

				if(dbgHandle->conditions->ebxMatch > 0)
					dbgHandle->conditions->conditionReached = 1;

			}

			// -----------------------------------------------------
			// evaluate ecx conditions
			if(dbgHandle->conditions->ecxSet > 0){

				// exact matching
				if(dbgHandle->ecx == (size_t) dbgHandle->conditions->ecx)
					dbgHandle->conditions->ecxMatch = 1;

				// Greater then matching
				if(dbgHandle->conditions->ecxGt != 0)
				if((size_t) dbgHandle->conditions->ecxGt < dbgHandle->ecx){
					dbgHandle->conditions->ecxGtMatch = 1;
					dbgHandle->conditions->ecxMatch = 1;
				}

				// less then matching
				if(dbgHandle->conditions->ecxLt > 0)
				if((size_t) dbgHandle->conditions->ecxLt > dbgHandle->ecx){
					dbgHandle->conditions->ecxLtMatch = 1;
					dbgHandle->conditions->ecxMatch = 1;
				}

				if(dbgHandle->conditions->ecxMatch > 0)
					dbgHandle->conditions->conditionReached = 1;

			}

			// -----------------------------------------------------
			// evaluate edx conditions
			if(dbgHandle->conditions->edxSet > 0){

				// exact matching
				if(dbgHandle->edx == (size_t) dbgHandle->conditions->edx)
					dbgHandle->conditions->edxMatch = 1;

				// Greater then matching
				if(dbgHandle->conditions->edxGt != 0)
				if((size_t) dbgHandle->conditions->edxGt < dbgHandle->edx){
					dbgHandle->conditions->edxGtMatch = 1;
					dbgHandle->conditions->edxMatch = 1;
				}

				// less then matching
				if(dbgHandle->conditions->edxLt > 0)
				if((size_t) dbgHandle->conditions->edxLt > dbgHandle->edx){
					dbgHandle->conditions->edxLtMatch = 1;
					dbgHandle->conditions->edxMatch = 1;
				}

				if(dbgHandle->conditions->edxMatch > 0)
					dbgHandle->conditions->conditionReached = 1;
			}

			// -----------------------------------------------------
			// evaluate esp conditions
			if(dbgHandle->conditions->espSet > 0){

				// exact matching
				if(dbgHandle->esp == (size_t) dbgHandle->conditions->esp)
					dbgHandle->conditions->espMatch = 1;

				// Greater then matching
				if(dbgHandle->conditions->espGt != 0)
				if((size_t) dbgHandle->conditions->espGt < dbgHandle->esp){
					dbgHandle->conditions->espGtMatch = 1;
					dbgHandle->conditions->espMatch = 1;
				}

				// less then matching
				if(dbgHandle->conditions->espLt > 0)
				if((size_t) dbgHandle->conditions->espLt > dbgHandle->esp){
					dbgHandle->conditions->espLtMatch = 1;
					dbgHandle->conditions->espMatch = 1;
				}

				if(dbgHandle->conditions->espMatch > 0)
					dbgHandle->conditions->conditionReached = 1;

			}

			// -----------------------------------------------------
			// evaluate ebp conditions
			if(dbgHandle->conditions->ebpSet > 0){

				// exact matching
				if(dbgHandle->ebp == (size_t) dbgHandle->conditions->ebp)
					dbgHandle->conditions->ebpMatch = 1;

				// Greater then matching
				if(dbgHandle->conditions->ebpGt != 0)
				if((size_t) dbgHandle->conditions->ebpGt < dbgHandle->ebp){
					dbgHandle->conditions->ebpGtMatch = 1;
					dbgHandle->conditions->ebpMatch = 1;
				}

				// less then matching
				if(dbgHandle->conditions->ebpLt > 0)
				if((size_t) dbgHandle->conditions->ebpLt > dbgHandle->ebp){
					dbgHandle->conditions->ebpLtMatch = 1;
					dbgHandle->conditions->ebpMatch = 1;
				}

				if(dbgHandle->conditions->ebpMatch > 0)
					dbgHandle->conditions->conditionReached = 1;
			}

			// -----------------------------------------------------
			// evaluate eip conditions
			if(dbgHandle->conditions->eipSet > 0){

				// exact matching
				if(dbgHandle->eip == (size_t) dbgHandle->conditions->eip)
					dbgHandle->conditions->eipMatch = 1;

				// Greater then matching
				if(dbgHandle->conditions->eipGt != 0)
				if((size_t) dbgHandle->conditions->eipGt < dbgHandle->eip){
					dbgHandle->conditions->eipGtMatch = 1;
					dbgHandle->conditions->eipMatch = 1;
				}

				// less then matching
				if(dbgHandle->conditions->eipLt > 0)
				if((size_t) dbgHandle->conditions->eipLt > dbgHandle->eip){
					dbgHandle->conditions->eipLtMatch = 1;
					dbgHandle->conditions->eipMatch = 1;
				}

				if(dbgHandle->conditions->eipMatch > 0)
					dbgHandle->conditions->conditionReached = 1;
			}

			// -----------------------------------------------------
			// evaluate esi conditions
			if(dbgHandle->conditions->esiSet > 0){

				// exact matching
				if(dbgHandle->esi == (size_t) dbgHandle->conditions->esi)
					dbgHandle->conditions->esiMatch = 1;

				// Greater then matching
				if(dbgHandle->conditions->esiGt != 0)
				if((size_t)  dbgHandle->conditions->esiGt < dbgHandle->esi){
					dbgHandle->conditions->esiGtMatch = 1;
					dbgHandle->conditions->esiMatch = 1;
				}

				// less then matching
				if(dbgHandle->conditions->esiLt > 0)
				if((size_t) dbgHandle->conditions->esiLt > dbgHandle->esi){
					dbgHandle->conditions->esiLtMatch = 1;
					dbgHandle->conditions->esiMatch = 1;
				}

				if(dbgHandle->conditions->esiMatch > 0)
					dbgHandle->conditions->conditionReached = 1;
			}
			// -----------------------------------------------------
			if(dbgHandle->conditions->cmpSet > 0){

				// verify that the pointer is valid
				if(IsPointerValidForProcess(dbgHandle, dbgHandle->conditions->cmpAddr) == ERR_SUCCESS){
					if(DebugCompareMemory(dbgHandle, \
										  dbgHandle->conditions->cmpAddr, \
										  dbgHandle->conditions->cmpBuff, \
										  dbgHandle->conditions->cmpLen) == ERR_SUCCESS){
						dbgHandle->conditions->cmpMatch = 1;
					}
				}


				if(dbgHandle->conditions->cmpMatch > 0)
						dbgHandle->conditions->conditionReached = 1;

			}

			// -----------------------------------------------------

			// after all evaluations conclude for this element, evaluate
			// if the condition has been reached, if it has, store the condition
			// pointer in the lastConditionRaised element, reset the condition,
			// and break the loop.
			if(dbgHandle->conditions->conditionReached > 0){
				dbgHandle->lastConditionRaised = dbgHandle->conditions;
				dbgHandle->conditions = first;
				return ERR_SUCCESS;
			}

		}

	}

	// reset our first pointer on loop exit
	dbgHandle->conditions = first;
	return ERR_FAILURE;
}


// Detach from a debugged processes initial thread
// does not look at sub structures.
int DebugDetach(P_DEBUG_HANDLE dbgHandle){
	if(dbgHandle == NULL)
		return ERR_FAILURE;

	dbgHandle->traceReturn = ptrace(PT_DETACH, dbgHandle->debugPid, NULL,NULL);
	dbgHandle->attached = 0;

	return ERR_SUCCESS;
}

// Destroys a condition list element
int DestroyConditionElement(P_DEBUG_CONDITION_LIST element){
	if(element == NULL)
		return ERR_DBG_CONDITIONS_NULL;

	bpfree(element);
	return ERR_SUCCESS;
}

// Destroys an entire condition list
int DestroyConditionList(P_DEBUG_CONDITION_LIST dbgList){
	if(dbgList == NULL)
		return ERR_DBG_HANDLE_NULL;

	P_DEBUG_CONDITION_LIST isNext = NULL;

	for(dbgList = dbgList->first;
		dbgList != NULL;
		dbgList = isNext){

			if(dbgList == NULL){
				break;
			} else {
				isNext = dbgList->next;
				// DestroyConditionElement(dbgList);
				if(isNext == NULL)
					break;
			}
	}
	return ERR_SUCCESS;
}

// Destroys a debug handle, condition list included
int DestroyDebugHandle(P_DEBUG_HANDLE dbgHandle){
	if(dbgHandle == NULL)
		return ERR_FAILURE;

	if(dbgHandle->conditions != NULL)
		DestroyConditionList(dbgHandle->conditions);

	if(dbgHandle->sigDesc != NULL)
		bpfree(dbgHandle->sigDesc);

	bpfree(dbgHandle);
	return ERR_SUCCESS;
}

// Tests if a breakpoint has been installed
int BreakpointIsSet(P_DEBUG_HANDLE dbgHandle, int bpNumber){

	if(dbgHandle == NULL)
		return ERR_DBG_HANDLE_NULL;

	P_DEBUG_BREAKPOINT_LIST bpElement = GetBreakpointByNumber(dbgHandle, bpNumber);

	if(bpElement == NULL)
		return ERR_FAILURE;

	if(bpElement->installed > 0)
		return ERR_SUCCESS;

	return ERR_FAILURE;
}

// Linux specific function to return a signal number from
// a debugging handle.
char * L_ReturnStringFromSignal(P_DEBUG_HANDLE dbgHandle){
	char * retString = NULL;

	// returns negative one on dbgHandle being null
	if(dbgHandle == NULL)
		return NULL;

	// bpmalloc a string buffer to hold the signal information
	retString = (char *) bpmalloc(1024);
	memset(retString, 0x00, 1024);

	// large switch case to determine the signal
	// and handle it appropriately
	switch(dbgHandle->signal){
			case SIGTRAP:
				snprintf(retString, 1023, "PID %i : SIGTRAP", dbgHandle->signal);
				return retString;
			case SIGSEGV:
				snprintf(retString, 1023, "PID: %i : SIGSEGV", dbgHandle->signal);
				return retString;
			case SIGHUP:
				snprintf(retString, 1023, "PID: %i : SIGHUP", dbgHandle->signal);
				return retString;
			case SIGINT:
				snprintf(retString, 1023, "PID: %i : SIGINT", dbgHandle->signal);
				return retString;
			case SIGQUIT:
				snprintf(retString, 1023, "PID: %i : SIGQUIT", dbgHandle->signal);
				return retString;
			case SIGILL:
				snprintf(retString, 1023, "PID: %i : SIGILL", dbgHandle->signal);
				return retString;
			case SIGABRT:
				snprintf(retString, 1023, "PID: %i : SIGABRT", dbgHandle->signal);
				return retString;
			case SIGFPE:
				snprintf(retString, 1023, "PID: %i : SIGFPE", dbgHandle->signal);
				return retString;
			case SIGKILL:
				snprintf(retString, 1023, "PID: %i : SIGKILL", dbgHandle->signal);
				return retString;
			case SIGBUS:
				snprintf(retString, 1023, "PID: %i : SIGBUS", dbgHandle->signal);
				return retString;
			case SIGSYS:
				snprintf(retString, 1023, "PID: %i : SIGSYS", dbgHandle->signal);
				return retString;
			case SIGPIPE:
				snprintf(retString, 1023, "PID: %i : SIGPIPE", dbgHandle->signal);
				return retString;
			case SIGALRM:
				snprintf(retString, 1023, "PID: %i : SIGALRM", dbgHandle->signal);
				return retString;
			case SIGTERM:
				snprintf(retString, 1023, "PID: %i : SIGTERM", dbgHandle->signal);
				return retString;
			case SIGURG:
				snprintf(retString, 1023, "PID: %i : SIGURG", dbgHandle->signal);
				return retString;
			case SIGSTOP:
				snprintf(retString, 1023, "PID: %i : SIGSTOP", dbgHandle->signal);
				return retString;
			case SIGTSTP:
				snprintf(retString, 1023, "PID: %i : SIGTSTP", dbgHandle->signal);
				return retString;
			case SIGXCPU:
				snprintf(retString, 1023, "PID: %i : SIGXCPU", dbgHandle->signal);
				return retString;
			case SIGXFSZ:
				snprintf(retString, 1023, "PID: %i : SIGXFSZ", dbgHandle->signal);
				return retString;
			case SIGVTALRM:
				snprintf(retString, 1023, "PID: %i : SIGVTALRM", dbgHandle->signal);
				return retString;
			case SIGPROF:
				snprintf(retString, 1023, "PID: %i : SIGPROF", dbgHandle->signal);
				return retString;

			default:
				bpfree(retString);
				break;
	}

	return NULL;
}


// Gets current registers from the process and
// sets them in the dbgHandle
int DebugGetRegisters(P_DEBUG_HANDLE dbgHandle){

	int error_code = 0;
	if(dbgHandle == NULL)
		return ERR_FAILURE;


	struct user_regs_struct *registers = (struct user_regs_struct *) bpmalloc( sizeof(struct user_regs_struct));
		memset(registers, 0x00, sizeof(user_regs_struct));

	error_code = ptrace(PTRACE_GETREGS, dbgHandle->debugPid, 0, registers);
	if(error_code == -1){

		printf("\n ERROR RETRIEVING REGISTERS");



		bpfree(registers);
		return ERR_FAILURE;
	}


#if __WORDSIZE != 64

	// assign registers in debug handle
	dbgHandle->eax = registers->eax;
	dbgHandle->ebx = registers->ebx;
	dbgHandle->ecx = registers->ecx;
	dbgHandle->edx = registers->edx;
	dbgHandle->esp = registers->esp;
	dbgHandle->ebp = registers->ebp;
	dbgHandle->esi = registers->esi;
	dbgHandle->eip = registers->eip;
	dbgHandle->edi = registers->edi;

	// store eflags
	dbgHandle->eflags = registers->eflags;

	// store segment registers (unsigned shorts)
	dbgHandle->cs = registers->xcs;
	dbgHandle->__cs = registers->xcs;

	dbgHandle->ds = registers->xds;
	dbgHandle->__ds = registers->xds;

	dbgHandle->es = registers->xes;
		dbgHandle->__es = registers->xes;

	dbgHandle->fs = registers->xfs;
		dbgHandle->__fs = registers->xfs;

	dbgHandle->gs = registers->xgs;
		dbgHandle->__gs = registers->xgs;

	dbgHandle->ss = registers->xss;
		dbgHandle->ss = registers->xss;

	dbgHandle->orig_eax = registers->orig_eax;

#endif

// Store 64bit registers if wordsize is 64bit
#if __WORDSIZE == 64

	dbgHandle->r15 = registers->r15;
	dbgHandle->r14 = registers->r14;
	dbgHandle->r13 = registers->r13;
	dbgHandle->r12 = registers->r12;
	dbgHandle->rbp = registers->rbp;
	dbgHandle->rbx = registers->rbx;
	dbgHandle->r11 = registers->r11;
	dbgHandle->r10 = registers->r10;
	dbgHandle->r9 = registers->r9;
	dbgHandle->r8 = registers->r8;
	dbgHandle->rax = registers->rax;
	dbgHandle->rcx = registers->rcx;
	dbgHandle->rdx = registers->rdx;
	dbgHandle->rsi = registers->rsi;
	dbgHandle->rdi = registers->rdi;
	dbgHandle->orig_rax = registers->orig_rax;
	dbgHandle->rip = registers->rip;
	dbgHandle->cs = registers->cs;
	dbgHandle->eflags = registers->eflags;
	dbgHandle->rsp = registers->rsp;
	dbgHandle->ss = registers->ss;
	dbgHandle->fs_base = registers->fs_base;
	dbgHandle->gs_base = registers->gs_base;
	dbgHandle->ds = registers->ds;
	dbgHandle->es = registers->es;
	dbgHandle->fs = registers->fs;
	dbgHandle->gs = registers->gs;

#endif


	if(registers != NULL)
		bpfree(registers);

	return ERR_SUCCESS;
}

// Compares process memory, evaluating to either true (1) or false (0) if
// memory matches.  Be careful to avoid comparing invalid ranges. ProcAddress + 1
// the whole way.
int DebugCompareMemory(P_DEBUG_HANDLE dbgHandle, void *processAddr,\
		void *cmpDataBuff, int cmpLen){

		// quick null checks
		if(dbgHandle == NULL)
			return ERR_FAILURE;

		if(cmpDataBuff == NULL)
			return ERR_FAILURE;

		if(processAddr == NULL)
			return ERR_FAILURE;

		if(cmpLen == 0)
			return ERR_FAILURE;


		int data;

		// this holds the retrieved data buffer
		unsigned char * retrievedBuff = (unsigned char *) bpmalloc(cmpLen);
			memset(retrievedBuff, 0x00, cmpLen);

		size_t addr;

		// peek data only reads in 4 byte increments
		// so make sure to know your remainder and quotient
		// of a 4 byte divisior.
		int remainder = cmpLen % 4;
		int quotient  = cmpLen / 4;

		// keeps track of where we are in our buffer during copying
		int bufferPos = 0;


		while(quotient > 0){
			data = ptrace(PTRACE_PEEKDATA, dbgHandle->debugPid,  processAddr, 0);

			// copy in the read data
				memcpy(&retrievedBuff[bufferPos], (void *) &data, 4);

			// read 4 bytes successfully, subtract from the quotient and
			// increment the buffer position.
				quotient--;
				bufferPos += 4;

			// increment the pointer to read 4 more bytes
				addr = (int) processAddr;
				addr += 4;
				processAddr = (void *) addr;
		}

		// access the proper remainder
		if(remainder > 0){

			data = ptrace(PTRACE_PEEKDATA, dbgHandle->debugPid,  processAddr, 0);

			memcpy(&retrievedBuff[bufferPos],(void *) &data, remainder);
		}

		if(cmpDataBuff == NULL || retrievedBuff == NULL || cmpLen == 0){
			return ERR_FAILURE;
		}

		// now perform the actual compare
		if(memcmp(cmpDataBuff, retrievedBuff, cmpLen) == 0){
			bpfree(retrievedBuff);
			return ERR_SUCCESS;
		} else {
			bpfree(retrievedBuff);
			return ERR_FAILURE;
		}

	bpfree(retrievedBuff);
	return ERR_FAILURE;
}


// Reads the specified number of bytes from the process, starting at
// processAddr, reading untill readLen
unsigned char * DebugReadMemory(P_DEBUG_HANDLE dbgHandle, void *processAddr, int readLen){

	if(dbgHandle == NULL)
		return NULL;

	if(dbgHandle->attached <= 0)
		return NULL;

	int data = 0;

	// this holds the retrieved data buffer
	unsigned char * retrievedBuff = (unsigned char *) bpmalloc(readLen+100);
		memset(retrievedBuff, 0x00, readLen+100);

	size_t addr;

	// peek data only reads in 4 byte increments
	// so make sure to know your remainder and quotient
	// of a 4 byte divisior.
	int remainder = readLen % 4;
	int quotient  = readLen / 4;

	// keeps track of where we are in our buffer during copying
	int bufferPos = 0;

	while(quotient > 0){
		data = ptrace(PTRACE_PEEKDATA, dbgHandle->debugPid,  processAddr, 0);

		// copy in the read data
			memcpy(&retrievedBuff[bufferPos], (void *) &data, 4);

		// read 4 bytes successfully, subtract from the quotient and
		// increment the buffer position.
			quotient--;
			bufferPos += 4;

		// increment the pointer to read 4 more bytes
			addr = (int) processAddr;
			addr += 4;
			processAddr = (void *) addr;

	}


	// this may copy some (3 max) extra bytes, but shouldn't affect anything
	// because the compare is done against a user supplied buffer
	// of a user supplied length.

	if(remainder > 0){

		// only one read is required because our remainder will never
		// be greater then 4.
		data = ptrace(PTRACE_PEEKDATA, dbgHandle->debugPid,  processAddr, 0);
		memcpy(&retrievedBuff[bufferPos],(void *) &data, remainder);
	}

	return retrievedBuff;
}



// Reads an allocated string at max readLen length from process memory at processAddr.
char * DebugReadString(P_DEBUG_HANDLE dbgHandle, void *processAddr, int readLen){

	if(dbgHandle == NULL)
		return NULL;

	if(dbgHandle->attached <= 0)
		return NULL;

	if(readLen == 0)
		return NULL;

	// this holds the retrieved data buffer
	char * retrievedBuff = (char *) bpmalloc(readLen+100);
		memset(retrievedBuff, 0x00, readLen+100);

	size_t processAddrUint = (size_t) processAddr;
	unsigned char * newChar = NULL;

	// full string loop
	for(;;){

		// loop and read one byte till we get full string
		newChar = DebugReadMemory(dbgHandle, processAddr, 1);
		if(newChar == NULL)
			break;

		// if character is null, return
		if(strnlen((char *) newChar, 1) == 0){
			break;
		} else {
			strncat(retrievedBuff, (char *) newChar, 1);
			bpfree(newChar);
		}

		// break if we hit a max length
		if(strnlen(retrievedBuff, readLen) == readLen)
			break;

		// subtract result as int and store ptr
		processAddrUint++;
		processAddr = (void *) processAddrUint;

	}

	return retrievedBuff;
}


// Sets the values for one condition block, appending the condition
// to the debug handle paramter.

// Conditions are checked in sequence, one after the other
// in a stepped debugging loop (slow but required).

// the Lt and Gt variables shown after each register parameter
// are for less than/greater than comparisons.  If they
// are set to true, the match will be returned as successful
// if the register matches a gt/lt size_teger comparison.
// A condition must have either only Lt or Gt or none set.

// The cmp parameters are used to compare memory of running processes
// to a specified buffer.  cmpLen is the length to compare.

P_DEBUG_CONDITION_LIST AddConditionElement( P_DEBUG_HANDLE dbgHandle, \
	int signalSet, int signalCmp, \
	int eaxSet, int eax, int eaxGt, int eaxLt, \
	int ebxSet, int ebx, int ebxGt, int ebxLt, \
	int ecxSet, int ecx, int ecxGt,	int ecxLt, \
	int edxSet, int edx, int edxGt, int edxLt, \
	int espSet, int esp, int espGt, int espLt, \
	int ebpSet, int ebp, int ebpGt,	int ebpLt, \
	int eipSet, int eip, int eipGt,	int eipLt, \
	int esiSet, int esi, int esiGt,	int esiLt, \
	int cmpSet, void *cmpAddr, unsigned char *cmpBuff, int cmpLen){


	int i = 0;

	if(dbgHandle == NULL)
		return NULL;

	P_DEBUG_CONDITION_LIST newList = NULL;

	// if there are no existing conditions, set conditions
	// pointer to new bpmalloc, otherwise append to list.
	if(dbgHandle->conditions == NULL){

		dbgHandle->conditions = (P_DEBUG_CONDITION_LIST) bpmalloc(sizeof(DEBUG_CONDITION_LIST));
		memset(dbgHandle->conditions, 0x00, sizeof(DEBUG_CONDITION_LIST));
		dbgHandle->conditions->first = dbgHandle->conditions;
			dbgHandle->conditions->prev = NULL;
			dbgHandle->conditions->next = NULL;

		newList = dbgHandle->conditions->first;

	} else {

		newList = (P_DEBUG_CONDITION_LIST) bpmalloc(sizeof(DEBUG_CONDITION_LIST));
		memset(newList, 0x00, sizeof(DEBUG_CONDITION_LIST));
		newList->first = newList;
			newList->prev = NULL;
			newList->next = NULL;

		AppendConditionList(dbgHandle->conditions, newList);
	}

	// conditionId is always the number of the condition
	// in the list, starting at 0.
	for(dbgHandle->conditions = dbgHandle->conditions->first; \
		dbgHandle->conditions->next != NULL; \
		dbgHandle->conditions = dbgHandle->conditions->next){
			i++;
	}

	// dbgHandle->conditions should now be set to the appropriate element (last).
	// operations can now be performed.

	// set condition identifier
	dbgHandle->conditions->conditionId = i;

	// set eax conditions
	if(eaxSet != 0){
		dbgHandle->conditions->eaxSet = eaxSet;
		dbgHandle->conditions->eax    = eax;
		dbgHandle->conditions->eaxGt  = eaxGt;
		dbgHandle->conditions->eaxLt  = eaxLt;
	}

	// set ebx conditions
	if(ebxSet != 0){
		dbgHandle->conditions->ebxSet = ebxSet;
		dbgHandle->conditions->ebx = ebx;
		dbgHandle->conditions->ebxGt = ebxGt;
		dbgHandle->conditions->ebxLt = ebxLt;
	}

	// set ecx conditions
	if(ecxSet != 0){
		dbgHandle->conditions->ecxSet = ecxSet;
		dbgHandle->conditions->ecx = ecx;
		dbgHandle->conditions->ecxGt = ecxGt;
		dbgHandle->conditions->ecxLt = ecxLt;
	}

	// set edx conditions
	if(edxSet != 0){
		dbgHandle->conditions->edxSet = edxSet;
		dbgHandle->conditions->edx = edx;
		dbgHandle->conditions->edxGt = edxGt;
		dbgHandle->conditions->edxLt = edxLt;
	}

	// set esp conditions
	if(espSet != 0){
		dbgHandle->conditions->espSet = espSet;
		dbgHandle->conditions->esp = esp;
		dbgHandle->conditions->espGt = espGt;
		dbgHandle->conditions->espLt = espLt;
	}

	// set ebp conditions
	if(ebpSet != 0){
		dbgHandle->conditions->ebpSet = ebpSet;
		dbgHandle->conditions->ebp = ebp;
		dbgHandle->conditions->ebpGt = ebpGt;
		dbgHandle->conditions->ebpLt = ebpLt;
	}

	// set eip conditions
	if(eipSet != 0){
		dbgHandle->conditions->eipSet = eipSet;
		dbgHandle->conditions->eip = eip;
		dbgHandle->conditions->eipGt = eipGt;
		dbgHandle->conditions->eipLt = eipLt;
	}

	// set esi conditions
	if(esiSet != 0){
		dbgHandle->conditions->esiSet = esiSet;
		dbgHandle->conditions->esi   = esi;
		dbgHandle->conditions->esiGt = esiGt;
		dbgHandle->conditions->esiLt = esiLt;
	}

	// sets compare conditions
	if(cmpSet != 0){
		dbgHandle->conditions->cmpSet = cmpSet;
		dbgHandle->conditions->cmpAddr = cmpAddr;
		dbgHandle->conditions->cmpBuff = cmpBuff;
		dbgHandle->conditions->cmpLen  = cmpLen;
	}

	return newList;
}

// Appends a condition list to an already existing condition list.
// This will modify appendList changing its pointers to reflect those
// in the inList.  Be aware of this modification when writing conditions.
int AppendConditionList(P_DEBUG_CONDITION_LIST inList, \
						P_DEBUG_CONDITION_LIST appendList){

	if(inList == NULL)
		return ERR_FAILURE;

	if(appendList == NULL)
		return ERR_FAILURE;

	// Effectively move to the end of the inList
	for(inList = inList->first; inList->next != NULL; inList = inList->next){}
	appendList = appendList->first;

	// match up pointers
	inList->next = appendList;
	appendList->prev = inList;

	// now set all the append list first pointers
	for(; appendList != NULL; appendList = appendList->next){
		appendList->first = inList->first;
	}

	// list should now be set correctly
	return ERR_SUCCESS;
}

// Prints out one element
void PrintConditionElement(P_DEBUG_CONDITION_LIST dbgList){
	if(dbgList == NULL)
		return;

	printf("\n\n [+] Condition List -  %p", dbgList);
	printf("\n\t Condition List->first: %p", dbgList->first);
	printf("\n\t Condition List->prev:  %p", dbgList->prev);
	printf("\n\t Condition List->next:  %p", dbgList->next);

	printf("\n\t\t conditionId: %i", dbgList->conditionId);

	if(dbgList->conditionReached == 0){
		printf("\n\t\t conditionReached: NO");
	} else {
		printf("\n\t\t conditionReached: YES");
	}


	printf("\n");

	// print out eax register information if present
	if(dbgList->eaxSet > 0){
		printf("\n\t\t [Condition Present on eax: YES]");
		printf("\n\t\t  [+] Watch Value : eax == %p", (void *) dbgList->eax);

		printf("\n\t\t  [+] Test For val > eax: ");
		if(dbgList->eaxGt > 0)
			printf(" YES");
		else
			printf(" NO");

		printf("\n\t\t  [+] Test for val < eax: ");

		if(dbgList->eaxLt > 0)
			printf(" YES");
		else
			printf(" NO");

		if(dbgList->eaxMatch > 0)
			printf("\n\t\t  [+] Condition Matched:   YES");
		else
			printf("\n\t\t  [+] Condition Matched:   NO");

			printf("\n\t\t  ----------------------------");

	} else
		printf("\n\t\t [Condition Present on eax: NO]");

	// print out ebx register information if present
	if(dbgList->ebxSet > 0){
		printf("\n\t\t [Condition Present on ebx: YES]");
		printf("\n\t\t  [+] Watch Value : ebx == %p", (void *) dbgList->ebx);

		printf("\n\t\t  [+] Test For val > ebx: ");
		if(dbgList->ebxGt > 0)
			printf(" YES");
		else
			printf(" NO");

		printf("\n\t\t  [+] Test for val < ebx: ");

		if(dbgList->ebxLt > 0)
			printf(" YES");
		else
			printf(" NO");

		if(dbgList->ebxMatch > 0)
			printf("\n\t\t  [+] Condition Matched:   YES");
		else
			printf("\n\t\t  [+] Condition Matched:   NO");

			printf("\n\t\t  ----------------------------");

	} else
		printf("\n\t\t [Condition Present on ebx: NO]");

	// print out ecx register information if present
	if(dbgList->ecxSet > 0){
		printf("\n\t\t [Condition Present on ecx: YES]");
		printf("\n\t\t  [+] Watch Value : ecx == %p", (void *) dbgList->ecx);

		printf("\n\t\t  [+] Test For val > ecx: ");
		if(dbgList->ecxGt > 0)
			printf(" YES");
		else
			printf(" NO");

		printf("\n\t\t  [+] Test for val < ecx: ");

		if(dbgList->ecxLt > 0)
			printf(" YES");
		else
			printf(" NO");

		if(dbgList->ecxMatch > 0)
			printf("\n\t\t  [+] Condition Matched:   YES");
		else
			printf("\n\t\t  [+] Condition Matched:   NO");

			printf("\n\t\t  ----------------------------");

	} else
		printf("\n\t\t [Condition Present on ecx: NO]");

	// print out edx register information if present
	if(dbgList->edxSet > 0){
		printf("\n\t\t [Condition Present on edx: YES]");
		printf("\n\t\t  [+] Watch Value : edx == %p", (void *) dbgList->edx);

		printf("\n\t\t  [+] Test For val > edx: ");
		if(dbgList->edxGt > 0)
			printf(" YES");
		else
			printf(" NO");

		printf("\n\t\t  [+] Test for val < edx: ");

		if(dbgList->edxLt > 0)
			printf(" YES");
		else
			printf(" NO");

		if(dbgList->edxMatch > 0)
			printf("\n\t\t  [+] Condition Matched:   YES");
		else
			printf("\n\t\t  [+] Condition Matched:   NO");

			printf("\n\t\t  ----------------------------");

	} else
		printf("\n\t\t [Condition Present on edx: NO]");

	// print out esp register information if present
	if(dbgList->espSet > 0){
		printf("\n\t\t [Condition Present on esp: YES]");
		printf("\n\t\t  [+] Watch Value : esp == %p", (void *) dbgList->esp);

		printf("\n\t\t  [+] Test For val > esp: ");
		if(dbgList->espGt > 0)
			printf(" YES");
		else
			printf(" NO");

		printf("\n\t\t  [+] Test for val < esp: ");

		if(dbgList->espLt > 0)
			printf(" YES");
		else
			printf(" NO");

		if(dbgList->espMatch > 0)
			printf("\n\t\t  [+] Condition Matched:   YES");
		else
			printf("\n\t\t  [+] Condition Matched:   NO");

			printf("\n\t\t  ----------------------------");

	} else
		printf("\n\t\t [Condition Present on esp: NO]");

	// print out ebp register information if present
	if(dbgList->ebpSet > 0){
		printf("\n\t\t [Condition Present on ebp: YES]");
		printf("\n\t\t  [+] Watch Value : ebp == %p", (void *) dbgList->ebp);

		printf("\n\t\t  [+] Test For val > ebp: ");
		if(dbgList->ebpGt > 0)
			printf(" YES");
		else
			printf(" NO");

		printf("\n\t\t  [+] Test for val < ebp: ");

		if(dbgList->ebpLt > 0)
			printf(" YES");
		else
			printf(" NO");

		if(dbgList->ebpMatch > 0)
			printf("\n\t\t  [+] Condition Matched:   YES");
		else
			printf("\n\t\t  [+] Condition Matched:   NO");

			printf("\n\t\t  ----------------------------");

	} else
		printf("\n\t\t [Condition Present on ebp: NO]");

	// print out eip register information if present
	if(dbgList->eipSet > 0){
		printf("\n\t\t [Condition Present on eip: YES]");
		printf("\n\t\t  [+] Watch Value : eip == %p", (void *) dbgList->eip);

		printf("\n\t\t  [+] Test For val > eip: ");
		if(dbgList->eipGt > 0)
			printf(" YES");
		else
			printf(" NO");

		printf("\n\t\t  [+] Test for val < eip: ");

		if(dbgList->eipLt > 0)
			printf(" YES");
		else
			printf(" NO");

		if(dbgList->eipMatch > 0)
			printf("\n\t\t  [+] Condition Matched:   YES");
		else
			printf("\n\t\t  [+] Condition Matched:   NO");

			printf("\n\t\t  ----------------------------");

	} else
		printf("\n\t\t [Condition Present on eip: NO]");

	// print out esi register information if present
	if(dbgList->esiSet > 0){
		printf("\n\t\t [Condition Present on esi: YES]");
		printf("\n\t\t  [+] Watch Value : esi == %p", (void *) dbgList->esi);

		printf("\n\t\t  [+] Test For val > esi: ");
		if(dbgList->esiGt > 0)
			printf(" YES");
		else
			printf(" NO");

		printf("\n\t\t  [+] Test for val < esi: ");

		if(dbgList->esiLt > 0)
			printf(" YES");
		else
			printf(" NO");

		if(dbgList->esiMatch > 0)
			printf("\n\t\t  [+] Condition Matched:   YES");
		else
			printf("\n\t\t  [+] Condition Matched:   NO");

			printf("\n\t\t  ----------------------------");

	} else
		printf("\n\t\t [Condition Present on esi: NO]");

	// print out mem compare information
	if(dbgList->cmpSet > 0){
		printf("\n\t\t [Compare Memory Enabled: YES]");
		printf("\n\t\t  [+] Watch Address: cmp == %p", (void *) dbgList->cmpAddr);
		printf("\n\t\t  [+] Match Buffer: %p", dbgList->cmpBuff);
		printf("\n\t\t  [+] Compare Length: %i", dbgList->cmpLen);
		printf("\n\t\t  [+] Compare Matched: ");
		if(dbgList->cmpMatch > 0)
			printf("YES");
		else
			printf("NO");

		printf("\n\t\t  ----------------------------");

	} else
		printf("\n\t\t [Compare Memory Enabled: NO]");

}

// Prints out an entire condition list
int PrintConditionList(P_DEBUG_CONDITION_LIST dbgList){

	if(dbgList == NULL)
		return ERR_FAILURE;
	// loop to print elements one by one
	for(dbgList = dbgList->first; dbgList != NULL; dbgList = dbgList->next){
		PrintConditionElement(dbgList);
	}

	return ERR_SUCCESS;
}


// This creates a 0xCC style breakpoint and adds it
// to the running process.  The breakpoint structure/list
// is added in the dbgList.
int CreateAddBreakpoint(P_DEBUG_HANDLE dbgHandle, void * addr, int bpNumber){

	if(dbgHandle == NULL)
		return ERR_FAILURE;

	if(dbgHandle->attached < 1)
		return ERR_FAILURE;

	// holds last list pointer, explained later
	P_DEBUG_BREAKPOINT_LIST lastList = NULL;

	// create new breakpoint list
	P_DEBUG_BREAKPOINT_LIST newBreakpoint = NULL;

	// return if breakpoint exists
	if(GetBreakpointByNumber(dbgHandle, bpNumber) != NULL){
		return ERR_FAILURE;
	}

	// get the last breakpoint list address, used further down
	if(dbgHandle->breakpoints != NULL)
	for(dbgHandle->breakpoints = dbgHandle->breakpoints->first; \
		dbgHandle->breakpoints != NULL; \
		dbgHandle->breakpoints = dbgHandle->breakpoints->next){

		// save the last element before it becomes null
		if(dbgHandle->breakpoints->next == NULL)
			lastList = dbgHandle->breakpoints;

	}

	// create new breakpoint
	newBreakpoint = (P_DEBUG_BREAKPOINT_LIST) bpmalloc(sizeof(DEBUG_BREAKPOINT_LIST));
		memset(newBreakpoint, 0x00, sizeof(DEBUG_BREAKPOINT_LIST));

	// first read in 4 bytes worth of instructions from break
	// address.
	newBreakpoint->bpAddr = addr;
	newBreakpoint->bpInstruction = (void *) ptrace(PTRACE_PEEKDATA,dbgHandle->debugPid, addr, 0);

	// set the breakpoint number
	newBreakpoint->bpNumber = bpNumber;

	// now set breakpoint; if ptrace doesn't return negitive one
	// set installed flag
	if(ptrace(PTRACE_POKEDATA, dbgHandle->debugPid, addr, 0xCCCCCCCC) != -1)
		newBreakpoint->installed++;

	// now set the list elements

	if(lastList != NULL){

		newBreakpoint->first = lastList->first;
		newBreakpoint->prev = lastList;
		newBreakpoint->next = NULL;
		lastList->next = newBreakpoint;
		dbgHandle->breakpoints = lastList->first;


	} else {

		dbgHandle->breakpoints = newBreakpoint;
		dbgHandle->breakpoints->first = dbgHandle->breakpoints;
		dbgHandle->breakpoints->prev  = NULL;
		dbgHandle->breakpoints->next  = NULL;
	}

	return ERR_SUCCESS;
}


// Destroys a breakpoint by number, relinking list appropriately
// and repatching the process memory
int DestroyBreakpoint(P_DEBUG_HANDLE dbgHandle, int bpNumber){

	if(dbgHandle == NULL)
		return ERR_FAILURE;

	if(dbgHandle->attached < 1)
		return ERR_FAILURE;

	P_DEBUG_BREAKPOINT_LIST destroyList = GetBreakpointByNumber(dbgHandle, bpNumber);
	P_DEBUG_BREAKPOINT_LIST newFirst = NULL;

	if(destroyList != NULL){

		// replace instruction
		ptrace(PTRACE_POKEDATA, dbgHandle->debugPid, destroyList->bpAddr, destroyList->bpInstruction);

		// our list previous to the one to be destroyed isn't null
		if(destroyList->prev != NULL){
			destroyList->prev->next = destroyList->next;
		} else {

			// if previous is null, This indicates that we're removing the
			// first element. All first pointers now must be reset, as the will
			// be changing.
			if(destroyList->next != NULL){
				newFirst = destroyList->next;
				newFirst->first = newFirst;

				// skip to the end of the list resetting all pointers along the
				// way.
				for(;newFirst != NULL; newFirst = newFirst->next){
					newFirst->first = destroyList->next;
				}
			}


		}

		if(destroyList->next != NULL){

			if(destroyList->prev != NULL)
			if(destroyList->prev->prev == NULL)
				destroyList->prev = NULL;

			destroyList->next->prev = destroyList->prev;
		}

		// finished patching things up, return success
		bpfree(destroyList);
		return ERR_SUCCESS;
	}

	return ERR_FAILURE;
}

// Destroys a breakpoint list, destroying all breakpoints
int DestroyBreakpointList(P_DEBUG_HANDLE dbgHandle){

	if(dbgHandle == NULL)
		return ERR_FAILURE;

	if(dbgHandle->breakpoints == NULL)
		return ERR_FAILURE;

	P_DEBUG_BREAKPOINT_LIST bpFirst = dbgHandle->breakpoints->first;

	for(dbgHandle->breakpoints = dbgHandle->breakpoints->first;
		dbgHandle->breakpoints != NULL;
		dbgHandle->breakpoints = dbgHandle->breakpoints->next){

		if(PrintBreakpointElement(dbgHandle->breakpoints) == ERR_FAILURE)
			return ERR_FAILURE;
	}

	dbgHandle->breakpoints = bpFirst;

	return ERR_SUCCESS;
}

// This function prints out all breakpoints set in a dbgHandle
int PrintBreakpointElement(P_DEBUG_BREAKPOINT_LIST bpList){

	if(bpList == NULL)
		return ERR_FAILURE;

	printf("\n bpList Element: %p", bpList);
	printf("\n\t breapoint number: %i", bpList->bpNumber);
	printf("\n\t bpList->bpAddr: %p", bpList->bpAddr);
	printf("\n\t bpList->bpInstalled: ");
		if(bpList->installed > 0){
			printf(" YES");
		} else {
			printf(" NO");
		}

	printf("\n\t\tbpList->first: %p", bpList->first);
	printf("\n\t\tbpList->prev : %p", bpList->prev);
	printf("\n\t\tbpList->next : %p", bpList->next);

	return ERR_SUCCESS;
}


// prints out the breakpoint list given a debug
// handle.
int PrintBreakpointList(P_DEBUG_HANDLE dbgHandle){
	if(dbgHandle == NULL){
		printf("\n dbgHandle null.");
		return ERR_FAILURE;
	}

	if(dbgHandle->breakpoints == NULL){
		printf("\n breakpoints null.");
		return ERR_FAILURE;
	}

	P_DEBUG_BREAKPOINT_LIST bpFirst = dbgHandle->breakpoints->first;

	for(dbgHandle->breakpoints = dbgHandle->breakpoints->first; \
		dbgHandle->breakpoints != NULL; \
		dbgHandle->breakpoints = dbgHandle->breakpoints->next){
			if(PrintBreakpointElement(dbgHandle->breakpoints) == ERR_FAILURE){
				printf("\n dbgHandle->breakpoints: unable to print.");
				return ERR_FAILURE;
			}
	}

	printf("\n Should succede.");
	dbgHandle->breakpoints = bpFirst;

	return ERR_SUCCESS;
}


// This function retreives a breakpoint list item by number.
// If the list is empty, or non-existant, null is returned
P_DEBUG_BREAKPOINT_LIST GetBreakpointByNumber(P_DEBUG_HANDLE dbgHandle, int bpNumber){
	P_DEBUG_BREAKPOINT_LIST firstBp = NULL;
	if(dbgHandle == NULL)
		return ERR_FAILURE;

	if(dbgHandle->breakpoints == NULL)
		return ERR_FAILURE;

	if(dbgHandle->breakpoints != NULL){
		firstBp = dbgHandle->breakpoints->first;
	}

	for(dbgHandle->breakpoints = dbgHandle->breakpoints->first; \
		dbgHandle->breakpoints != NULL;	\
		dbgHandle->breakpoints = dbgHandle->breakpoints->next){

			if(dbgHandle->breakpoints == NULL)
				break;

			// if the numbers match, return the pointer
			if(dbgHandle->breakpoints->bpNumber == bpNumber)
				return dbgHandle->breakpoints;
	}

	dbgHandle->breakpoints = firstBp;

	return NULL;
}


// Linux specific function to determine if a signal is
// fatal.
// ret: greater than 0 == fatal; -1 == NULL; 0 == non-fatal
int L_SignalIsFatal(P_DEBUG_HANDLE dbgHandle){

	if(dbgHandle == NULL)
		return ERR_DBG_HANDLE_NULL;

	switch(dbgHandle->signal){
			case SIGSEGV:
				return 1;
			case SIGHUP:
				return 1;
			case SIGINT:
				return 1;
			case SIGQUIT:
				return 1;
			case SIGILL:
				return 1;
			case SIGABRT:
				return 1;
			case SIGFPE:
				return 1;
			case SIGKILL:
				return 1;
			case SIGBUS:
				return 1;
			case SIGSYS:
				return 1;
			case SIGPIPE:
				return 1;
			case SIGALRM:
				return 1;
			case SIGTERM:
				return 1;
			case SIGURG:
				return 1;
			case SIGXCPU:
				return 1;
			case SIGXFSZ:
				return 1;
			case SIGVTALRM:
				return 1;
			case SIGPROF:
				return 1;
			default:
				break;
	}

	return 0;
}

// Searches through valid memory ranges for a pattern
// (can be memory intensive).  Requires valid memory
// mapping. (This is extremly useful for heap exploits)
// Returns NULL on match failure.  Pattern cannot be longer
// then the valid memory range that is being compared against.
//
// The start pointer is used to indicate where the
// search should start/resume.  Makes it easier to successively
// search memory for string occurances.
void * DebugSearchValidMemory(P_DEBUG_HANDLE dbgHandle, char * pattern, int len, void * start){

	if(dbgHandle == NULL)
		return NULL;

	if(dbgHandle->attached < 1)
		return NULL;

	if(dbgHandle->memory == NULL)
		return NULL;

	// this is used simply to count the address
	// so that comparissons will never go past
	// the end of the range.
	size_t addrCounter = 0;

	// destroy the existing memory map to get a
	// up to date map.
	DestroyMemMap(dbgHandle);

	// get a brand new memory map
	GetMemMap(dbgHandle);

	if(dbgHandle->memory == NULL)
		return NULL;

	// copy the pointer for operations
	P_MEMORY_MAP mapList = dbgHandle->memory;

	for(mapList = mapList->first; \
		mapList != NULL; \
		mapList = mapList->next){
			printf("\n [+] Searching Map(%s):\n\t %p - %p\n", mapList->path, mapList->lowAddr, mapList->highAddr);
			// byte by byte comparison of data
			for(addrCounter = (size_t) mapList->lowAddr; \
				addrCounter <= (size_t) mapList->highAddr + len; \
				addrCounter += 1){

				// if the memory at this range matches up, report back with the pointer
				if(DebugCompareMemory(dbgHandle, (void *) addrCounter, pattern, len) == ERR_SUCCESS){
					return (void *) addrCounter;
				}
			}



	}
	// Memory map has been retrieved successfully here, so continue
	// analysis.

	return NULL;

}


// Prints out the sizes of each memory range in the dbgHandle (primarily
// for analysis and diagnostics).
void DebugPrintMemoryMapSizes(P_DEBUG_HANDLE dbgHandle){


	// destroy the existing memory map to get a
	// up to date map.
	DestroyMemMap(dbgHandle);

	// get a brand new memory map
	GetMemMap(dbgHandle);

	if(dbgHandle->memory == NULL)
		return;

	// copy the pointer for operations
	P_MEMORY_MAP mapList = dbgHandle->memory;

	for(mapList = mapList->first; \
		mapList != NULL; \
		mapList = mapList->next){

		printf("\n [Maps Size]: %s\n\t%p - %p == %u (size)\n", mapList->path, mapList->lowAddr, mapList->highAddr, ( (size_t) mapList->highAddr - (size_t) mapList->lowAddr) );

	}

}


// attempts to determine if a pointer is a valid
// heap pointer by testing it against valid heap
// ranges retrieved. If it is true, it returns the
// the pointer to the low range of the heap involved.
// If the pointer is not found in any  heap, return NULL
void * DebugPointerToHeapLow(P_DEBUG_HANDLE dbgHandle, void * testPtr){
	if(dbgHandle == NULL)
		return NULL;

	if(dbgHandle->attached < 1)
		return NULL;

	if(dbgHandle->memory == NULL)
		return NULL;

	size_t uintPtr = (size_t) testPtr;

	// store the first ptr for restore
	P_MEMORY_MAP first = dbgHandle->memory->first;

	// destroy the existing memory map to get a
	// up to date map.
	DestroyMemMap(dbgHandle);

	// get a brand new memory map
	GetMemMap(dbgHandle);

	if(dbgHandle->memory == NULL)
		return NULL;

	for(dbgHandle->memory = dbgHandle->memory->first; \
		dbgHandle->memory != NULL; \
		dbgHandle->memory = dbgHandle->memory->next){
		if(uintPtr >= (size_t) dbgHandle->memory->lowAddr && \
			uintPtr <= (size_t) dbgHandle->memory->highAddr && \
			(memcmp(dbgHandle->memory->path, (void *)"[heap]", 6) == 0) ){
				return (void *) dbgHandle->memory->lowAddr;
		}
	}

	dbgHandle->memory = first;
	return NULL;
}

// determines if a pointer is valid for the memory ranges existant
// in the process. ERR_SUCCESS for true
int DebugPointerIsValid(P_DEBUG_HANDLE dbgHandle, void * testPtr){
	if(dbgHandle == NULL)
		return ERR_DBG_HANDLE_NULL;

	if(dbgHandle->attached < 1)
		return ERR_DBG_NOT_ATTACHED;

	if(dbgHandle->memory == NULL)
		return ERR_MEMMAP_NULL;

	GetMemMap(dbgHandle);
	DebugGetStackFromMapping(dbgHandle);
	DebugGetHeapFromMapping(dbgHandle);

	// store the first ptr for restore
	P_MEMORY_MAP first = dbgHandle->memory->first;


	// size_teger pointer
	size_t uintPtr = (size_t) testPtr;

	// destroy the existing memory map to get a
	// up to date map.
	DestroyMemMap(dbgHandle);

	// get a brand new memory map
	GetMemMap(dbgHandle);

	if(dbgHandle->memory == NULL)
		return ERR_MEMMAP_NULL;


	for(dbgHandle->memory = dbgHandle->memory->first; \
		dbgHandle->memory != NULL; \
		dbgHandle->memory = dbgHandle->memory->next){
		if(uintPtr >= (size_t) dbgHandle->memory->lowAddr && \
			uintPtr <= (size_t) dbgHandle->memory->highAddr ){
				dbgHandle->memory = first;
				return ERR_SUCCESS;
		}
	}

	dbgHandle->memory = first;
	return ERR_FAILURE;
}


// prints a heap element
int DebugPrintHeapChunkElement(P_HEAP_CHUNK heapElement){

	if(heapElement == NULL)
		return ERR_HEAP_ELEMENT_NULL;

	// heap element
	printf("\nHeap Element: %p", heapElement);
		printf("\n\t Prev Size: %i", heapElement->prevSize);
		printf("\n\t Size(with flags):      %i (0x%.08x)", heapElement->size, heapElement->size);
		printf("\n\t Size of buffer (mem): %i (0x%.08x)", heapElement->realSize, heapElement->realSize);
		printf("\n\t Heap Chunk Base: %p", heapElement->base);

	// is corrupted?
	printf("\n\t\t Heap Element Corrupted: ");
		if(heapElement->isCorrupted > 0)
			printf("YES");
		else
			printf("NO");

	// is first?
	printf("\n\t\t Heap Element is Last: ");
		if(heapElement->isLast > 0)
			printf("YES");
		else
			printf("NO");

	// is likely corrupted by an overflow?
	printf("\n\t\t Heap Size Likely Overflowed: ");
		if(heapElement->likelyOverflow > 0)
			printf("YES");
		else
			printf("NO");

	printf("\n");
	printf("\n\t\t\t heapElement->first: %p", heapElement->first);
	printf("\n\t\t\t heapElement->prev : %p", heapElement->prev);
	printf("\n\t\t\t heapElement->next : %p", heapElement->next);

	return ERR_SUCCESS;
}


// prints a heap list
int DebugPrintHeapChunkList(P_HEAP_CHUNK heapElement){
	printf("\n");
	int i = 0;
	for(heapElement = heapElement->first; \
		heapElement != NULL; \
		heapElement = heapElement->next){

		printf("\n+------------------+");
		printf("\nHeap Chunk #%i:  %p|", i, heapElement);
		printf("\n+-----------------------------+");
		DebugPrintHeapChunkElement(heapElement);
		i++;
	}
	return ERR_SUCCESS;
}

// prints out all heap lists in the debug handle
int DebugPrintHeap(P_DEBUG_HANDLE dbgHandle){

	if(dbgHandle == NULL){
		return ERR_DBG_HANDLE_NULL;
	}

	if(dbgHandle->heap == NULL){
		return ERR_MEM_NO_HEAP_PRESENT;
	}

	P_PROCESS_HEAP first = dbgHandle->heap->first;
	int i = 0;
	// now walk through the list printing elements
	for(dbgHandle->heap = dbgHandle->heap->first;
		dbgHandle->heap != NULL;
		dbgHandle->heap = dbgHandle->heap->next){
			printf("\n+------------------------------+");
			printf("\n| Found Heap #%i: %p", i, dbgHandle->heap);
			printf("\n|  Base: 0x%.08x", (size_t) dbgHandle->heap->base);
			printf("\n|  Top : 0x%.08x", (size_t) dbgHandle->heap->top);
			printf("\n|  Appears Exploitable By Size: ");
			if(dbgHandle->heap->sizeExploitable > 0)
				printf("YES");
			else
				printf("NO");

			printf("\n+-----------------------------+");

			DebugPrintHeapChunkList(dbgHandle->heap->heapChunks);
			i++;
	}

	dbgHandle->heap = first;

	return ERR_SUCCESS;
}

int DebugPrintHeapElement(P_PROCESS_HEAP heapElement){

	if(heapElement == NULL)
		return ERR_HEAP_LIST_NULL;

	printf("\n Heap Range Element: %p", heapElement);
	printf("\n\t Heap Chunks Pointer: %p", heapElement->heapChunks);
	printf("\n\t Heap Base Pointer: %p", heapElement->base);
	printf("\n\t Heap Top  Pointer: %p", heapElement->top);

	printf("\n\t\t heapRange->first: %p", heapElement->first);
	printf("\n\t\t heapRange->prev : %p",heapElement->prev);
	printf("\n\t\t heapRange->next : %p", heapElement->next);


	return ERR_SUCCESS;
}


// Attempts to retrieve heap from memory given a valid MEMORY_MAP
// stored in the dbgHandle
int DebugGetHeapFromMapping(P_DEBUG_HANDLE dbgHandle){
	if(dbgHandle == NULL)
		return ERR_DBG_HANDLE_NULL;

	// bpfree existant memory map if exists
	if(dbgHandle->memory != NULL){
		// bpfree memory list
	}

	// bpfree existant heap if any heap exists
	// from previous calls
	if(dbgHandle->heap != NULL){
		// bpfree heap here
		dbgHandle->heap = NULL;
	}

	// process must be attached
	if(dbgHandle->attached <= 0)
		return ERR_DBG_NOT_ATTACHED;

	// get a new memory map to verify
	if(GetMemMap(dbgHandle) != ERR_SUCCESS)
		dbgHandle->memory = NULL;

	// verify our mappings	were retrieved
	if(dbgHandle->memory == NULL)
		return ERR_MEMMAP_NULL;



	// store the first pointer
	P_MEMORY_MAP first = dbgHandle->memory;

	// stores the previous pointer
	P_PROCESS_HEAP prev = NULL;
	P_PROCESS_HEAP firstHeap = NULL;

	// used to store data gathered
	// from a process heap
	void *data = NULL;

	// used for heap chunk list creation farther down the logic tree.
	size_t chunkBase = 0;
	size_t chunkTop  = 0;

	// main loop to retrieve heap
	for(dbgHandle->memory = dbgHandle->memory->first;
		dbgHandle->memory != NULL;
		dbgHandle->memory = dbgHandle->memory->next){

		if(dbgHandle->memory->path != NULL){

			// if this if branch executes, we have verified that a heap exists
			// and will need to be allocated.
			if(memcmp(dbgHandle->memory->path, "[heap]", 6) == 0){

				// get first heap element
				if(dbgHandle->heap == NULL){


					// create a new heap structure to identify the first
					// heap.
					dbgHandle->heap = (P_PROCESS_HEAP) bpmalloc(sizeof(PROCESS_HEAP));
						memset(dbgHandle->heap, 0x00, sizeof(PROCESS_HEAP));

					// save the first pointer; used later
					firstHeap = dbgHandle->heap;

					dbgHandle->heap->first = firstHeap;
					dbgHandle->heap->prev = NULL;
					dbgHandle->heap->next = NULL;

					// assign values to the chunk pointer, only setting the base and top
					// ptrs.
					dbgHandle->heap->base = dbgHandle->memory->lowAddr;
					dbgHandle->heap->top  = dbgHandle->memory->highAddr;

					// save previous pointer
					prev = dbgHandle->heap;

				} else {

					// if its not the first heap, and the heap element is
					// not null, make a new heap element, and move the heap
					// list forward. (should be null ptr waiting on assignment)
					dbgHandle->heap = dbgHandle->heap->next;

					// create a new heap structure to identify the first
					// heap.
					dbgHandle->heap = (P_PROCESS_HEAP) bpmalloc(sizeof(PROCESS_HEAP));
						memset(dbgHandle->heap, 0x00, sizeof(PROCESS_HEAP));


					// set relevant pointers
					dbgHandle->heap->prev = prev;
					dbgHandle->heap->first = prev->first;
					dbgHandle->heap->next = NULL;

					// set base and top ranges
					dbgHandle->heap->base = dbgHandle->memory->lowAddr;
					dbgHandle->heap->top  = dbgHandle->memory->highAddr;

					// save previous pointer
					dbgHandle->heap->prev = dbgHandle->heap;

				}

			}
		}

	}



	// restore the heap list
	dbgHandle->memory = first;
	dbgHandle->heap = firstHeap;

	if(dbgHandle->heap == NULL)
		return ERR_MEM_NO_HEAP_PRESENT;

	// now using the retrieved heap, we can create a real heap list
	// utilizing the documented GNU heap structure, this can tell us
	// if any heap chunks in the list are corrupted, and in some cases
	// how.

	if(dbgHandle->heap->first->base   != NULL && \
	    dbgHandle->heap->first->top != NULL && \
		dbgHandle->heap->first->base < dbgHandle->heap->first->top){


			// --------------- FROM MALLOC SOURCES: HEAP CHUNKS
			/*
			    Free chunks are stored in circular doubly-linked lists, and look like this:

    		chunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		            |             Size of previous chunk                            |
        		    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    		`head:' |             Size of chunk, in bytes                         |P|  first chunk in heap is verifyable (size)
      			mem-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		            |             Forward pointer to next chunk in list             |
            		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            		|             Back pointer to previous chunk in list            |
            		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            		|             Unused space (may be 0 bytes long)                .
            		.                                                               .
            		.                                                               |
		nextchunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    		`foot:' |             Size of chunk, in bytes                           |  this could be overwritten, and needs to have
		            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  conditions and checks to verify no overwrite
																					   has occured.

								Checks?: Overflow checks
									is equal to 0x41414141
									is equal to 0x%x%x%x%x
									is equal to 0x%n%n%n%n
									is equal to "../."

								Checks?: Corruption Checks
									is less then minimum heap
									is greater then maximum heap
									is an already existant heap address that has been read ( less then advanced chunkBase)
									if chunkBase + newSize is greater then the existing heap remainder ( corrupt )



			struct bpmalloc_chunk {

  				INTERNAL_SIZE_T      prev_size;  // Size of previous chunk (if bpfree).
 				INTERNAL_SIZE_T      size;       // Size in bytes, including overhead.

  				struct bpmalloc_chunk* fd;         // double links -- used only if bpfree.
  				struct bpmalloc_chunk* bk;
			};


	*/

	// -------------------------------------------------------------------------
	// BEGIN HEAP CHUNKING ROUTINE TO DETERMINE HEAP LIST STRUCTURE/CORRUPTIONS
	// -------------------------------------------------------------------------

		// now we will create our first heap chunk - never corrupt :)
		dbgHandle->heap->heapChunks = (P_HEAP_CHUNK) bpmalloc(sizeof(HEAP_CHUNK));
			memset(dbgHandle->heap->heapChunks, 0x00, sizeof(HEAP_CHUNK));

		// set pointers
		dbgHandle->heap->heapChunks->first = dbgHandle->heap->heapChunks;
			dbgHandle->heap->heapChunks->prev = NULL;
			dbgHandle->heap->heapChunks->next = NULL;

		// set assignments
		chunkBase = (size_t) dbgHandle->heap->base;
		chunkTop  = (size_t) dbgHandle->heap->top;

		// set the first base pointer
		dbgHandle->heap->heapChunks->base = (void *) (chunkBase + 8);

		// now get the first prevsize, (should be zero)
		data = (void *) DebugReadMemory(dbgHandle, (void *) chunkBase, 4);
			memcpy(&dbgHandle->heap->heapChunks->prevSize, data, 4);
			if(data != NULL)
				bpfree(data);

		// now get the first chunk size
		data = (void *) DebugReadMemory(dbgHandle, (void *) (chunkBase + 4), 4);
		memcpy(&dbgHandle->heap->heapChunks->size, data, 4);
		if(data != NULL)
			bpfree(data);

		dbgHandle->heap->heapChunks->realSize = GetHeapSizeFromChunkValue(dbgHandle->heap->heapChunks->size);


		// heap determination to see if heap is large enough to exploit reliably
		if( ((size_t) dbgHandle->heap->top - (size_t) dbgHandle->heap->base) > ( (1024 * 1024) + 50) ){
			dbgHandle->heap->sizeExploitable = 1;
		}

		// temporary variable to walk through heap, may segfault if it cannot
		// find the next size

		int newSize = 0;

		int isFirst = 0;

		// the isLast flag is set if either a corruption is found in the
		// heap chunking, or the heap chunk list is done (0 byte size)
		while(dbgHandle->heap->heapChunks->isLast < 1){


			newSize = 0;
			if(isFirst == 0){
				isFirst++;
				chunkBase += dbgHandle->heap->heapChunks->realSize;
			} else {
				chunkBase += dbgHandle->heap->heapChunks->realSize + 8;
			}

			// create a new element to modify
			dbgHandle->heap->heapChunks->next = (P_HEAP_CHUNK) bpmalloc(sizeof(HEAP_CHUNK));
				memset(dbgHandle->heap->heapChunks->next, 0x00, sizeof(HEAP_CHUNK));


			// set new element pointers
				dbgHandle->heap->heapChunks->next->prev = dbgHandle->heap->heapChunks;
				dbgHandle->heap->heapChunks->next->first = dbgHandle->heap->heapChunks->first;
				dbgHandle->heap->heapChunks->next->next = NULL;
				dbgHandle->heap->heapChunks = dbgHandle->heap->heapChunks->next;

			// this is the base of the chunk, where the size is held
			dbgHandle->heap->heapChunks->base = (void *)( chunkBase + dbgHandle->heap->heapChunks->realSize + 16);

			// get next size
			data = (void *) DebugReadMemory(dbgHandle, (void *) ( (size_t) dbgHandle->heap->heapChunks->base - 4), 4);
			memcpy(&newSize, data, 4);

			if(data != NULL)
				bpfree(data);


			// assign the new size value
			dbgHandle->heap->heapChunks->size = newSize;
			dbgHandle->heap->heapChunks->realSize = GetHeapSizeFromChunkValue(dbgHandle->heap->heapChunks->size);



			// Corruption Check:
			//	is equal to 0x41414141
			if(newSize == 0x41414141){
				dbgHandle->heap->heapChunks->isLast = 1;
				dbgHandle->heap->heapChunks->likelyOverflow = 1;
				dbgHandle->heap->heapChunks->isCorrupted = 1;
			}

			// Corruption Check:
			//  Quick Cmp Check on %x%x to see if equal.
			if(newSize == 0x25782578){
				dbgHandle->heap->heapChunks->isLast = 1;
				dbgHandle->heap->heapChunks->likelyOverflow = 1;
				dbgHandle->heap->heapChunks->isCorrupted = 1;

			}

			// Corruption Check:
			//	if the next size value plus the chunk base are greater then the top of the heap
			//  a corruption has occured.
			if((newSize + chunkBase) >  (size_t) dbgHandle->heap->top) {
				dbgHandle->heap->heapChunks->isLast = 1;
				dbgHandle->heap->heapChunks->likelyOverflow = 1;
				dbgHandle->heap->heapChunks->isCorrupted = 1;
			}

			//	Corruption Check:
			//  The new size is equal to zero, it is not a valid check, potential corruption
			if(newSize == 0){
				dbgHandle->heap->heapChunks->isLast = 1;
				dbgHandle->heap->heapChunks->isCorrupted = 1;
			}

			// exit if chunk has been read in entirety
			if( chunkBase >= (size_t) dbgHandle->heap->top ){
				break;
			}

			// exit if the chunk is marked as last (corrupted or otherwise)
			if(dbgHandle->heap->heapChunks->isLast == 1){
				break;
			}

		}

	}

	// proper heap size realignment
	chunkBase+= dbgHandle->heap->heapChunks->prev->realSize+8;
	dbgHandle->heap->heapChunks->base = (void *)chunkBase;


	dbgHandle->memory = first;
	return ERR_SUCCESS;
}

// attempts to retrieve process stack from memory given a valid MEMORY_MAP
// stored in the dbgHandle.  Retrieved stack from process is stored in the
// dbgHandle it was called from. (Code It)
int DebugGetStackFromMapping(P_DEBUG_HANDLE dbgHandle){
	if(dbgHandle == NULL)
		return ERR_DBG_HANDLE_NULL;

	if(dbgHandle->memory == NULL)
		return ERR_MEMMAP_NULL;

	P_MEMORY_MAP tmpFirst = dbgHandle->memory->first;



	for(dbgHandle->memory = dbgHandle->memory->first;
		dbgHandle->memory != NULL;
		dbgHandle->memory = dbgHandle->memory->next){

		if(dbgHandle->memory->path != NULL){

			// if this if branch executes, we have verified that a heap exists
			// and will need to be allocated.
			if(memcmp(dbgHandle->memory->path, "[stack]", 7) == 0){

					if(dbgHandle->stack != NULL){

						dbgHandle->stack->next = (P_PROCESS_STACK) bpmalloc(sizeof(PROCESS_STACK));
						memset(dbgHandle->stack->next, 0x00, sizeof(PROCESS_STACK));


							dbgHandle->stack->next->first = dbgHandle->stack->first;
							dbgHandle->stack->next->prev = dbgHandle->stack;
							dbgHandle->stack->next->next = NULL;

						// advance the pointer
						dbgHandle->stack = dbgHandle->stack->next;

					} else {

						dbgHandle->stack = (P_PROCESS_STACK) bpmalloc(sizeof(PROCESS_STACK));
						memset(dbgHandle->stack, 0x00, sizeof(PROCESS_STACK));
						dbgHandle->stack->first = dbgHandle->stack;

					}


					dbgHandle->stack->top = dbgHandle->memory->highAddr;
					dbgHandle->stack->base = dbgHandle->memory->lowAddr;


			}
		}
	}

	// restore memory pointer
	dbgHandle->memory = tmpFirst;

	// null pointer fix
	if(dbgHandle->stack != NULL)
		dbgHandle->stack = dbgHandle->stack->first;

	return ERR_SUCCESS;
}


// prints out the stacks in the process along with bases/heaps
int DebugPrintStack(P_DEBUG_HANDLE dbgHandle){
	if(dbgHandle->stack == NULL)
		return ERR_MEM_NO_STACK_PRESENT;

	if(dbgHandle->stack->first == NULL)
		return ERR_MEM_NO_STACK_PRESENT;

	int i = 0;
	P_PROCESS_STACK tmpFirst = dbgHandle->stack->first;
	for(dbgHandle->stack = dbgHandle->stack->first;
		dbgHandle->stack != NULL;
		dbgHandle->stack = dbgHandle->stack->next){


			printf("\n+---------------+");
			printf("\n| Process Stack #%i: %p", i, dbgHandle->stack);
			printf("\n+-----------------------------+");

			DebugPrintStackElement(dbgHandle->stack);
			i++;
	}

	// restore the stack
	dbgHandle->stack = tmpFirst;

	return ERR_SUCCESS;
}

// prints out one stack element in a list
int DebugPrintStackElement(P_PROCESS_STACK procStack){
	if(procStack == NULL)
		return ERR_MEM_NO_STACK_PRESENT;

	printf("\n Process Stack: %p", procStack);
	printf("\n\t Stack Top: %p", procStack->top);
	printf("\n\t Stack Base: %p", procStack->base);

	printf("\n\n\t Likely Overflowed: ");
	if(procStack->likelyOverflowed > 0)
			printf("YES");
	else
			printf("NO");

	printf("\n\t Appears Corrupted: ");
	if(procStack->isCorrupted > 0)
			printf("YES");
	else
			printf("NO");

	printf("\n\t\t first: %p", 	procStack->first);
	printf("\n\t\t prev : %p", 	procStack->prev);
	printf("\n\t\t next : %p", 	procStack->next);

	return ERR_SUCCESS;
}

// Searches the process stacks for a pattern of "len" bytes, returns
// valid memory pointer or null on unfound.  Requires valid memory
// pointer.  The base parameter, if not null, is used as an offset
// to start search from, allowing easier loop creation of entire
// stack searching.
void * DebugSearchStack(P_DEBUG_HANDLE dbgHandle, char * pattern, int len, void * base){

	if(dbgHandle == NULL)
		return NULL;

	if(dbgHandle->memory == NULL)
		return NULL;

	if(dbgHandle->stack == NULL)
		return NULL;

	// if we use a base pointer, make sure its valid to some
	// portion of memory.
	if(base != NULL)
	if(DebugPointerIsValid(dbgHandle, base) != ERR_SUCCESS)
		return NULL;

	// keep track of heap ranges
	size_t stackBase   = 0;
	size_t stackTop    = 0;
	size_t returnBase = 0;

	// record our first stack pointer
	P_PROCESS_STACK stackFirst = dbgHandle->stack->first;

	// now search stack
	for(dbgHandle->stack = dbgHandle->stack->first;
		dbgHandle->stack != NULL;
		dbgHandle->stack = dbgHandle->stack->next){

		// This determines which base to use while searching the
		// stack.
		if(base == NULL){
			stackBase = (size_t) dbgHandle->stack->base;

		} else {
			stackBase = (size_t) base;

			// small catch in the case that the users provided
			// base is lower then the actual heaps base
			if(stackBase <= (size_t) dbgHandle->stack->base){
				stackBase = (size_t) dbgHandle->stack->base;
			}

		}

		stackTop  = (size_t) dbgHandle->stack->top;

		// walk from low addresses to high addresses
		// comparing memory. (cpu intensive)
		while(stackBase < (stackTop-len) ){

			// safe check to make sure we dont access
			// outside of valid memory.
			if( (stackBase + len) > stackTop){
				return NULL;
			}



			if(DebugCompareMemory(dbgHandle, (void *)stackBase, pattern, len) == ERR_SUCCESS){

				if(returnBase == 0){
					returnBase = stackBase;
				}

				dbgHandle->stack = stackFirst;
				return (void *) returnBase;
			}

			// move the stack pointer up by one
			stackBase += 1;
		}

	}

	return NULL;
}


// Searches the process heaps for a pattern of "len" bytes, returns
// valid memory pointer or null on unfound.  Requires valid memory
// pointer.  The base parameter, if not null, is used as an offset
// to start search from, allowing easier loop creation of entire
// stack searching.
void * DebugSearchHeap(P_DEBUG_HANDLE dbgHandle, char * pattern, int len, void * base){

	if(dbgHandle == NULL)
		return NULL;

	if(dbgHandle->memory == NULL)
		return NULL;

	if(dbgHandle->heap == NULL)
		return NULL;

	// if we use a base pointer, make sure its valid to some
	// portion of memory.
	if(base != NULL)
	if(DebugPointerIsValid(dbgHandle, base) != ERR_SUCCESS)
		return NULL;

	// keep track of heap ranges
	size_t heapBase   = 0;
	size_t heapTop    = 0;
	size_t returnBase = 0;

	// record our first stack pointer
	P_PROCESS_HEAP heapFirst = dbgHandle->heap->first;

	// now search stack
	for(dbgHandle->heap = dbgHandle->heap->first;
		dbgHandle->heap != NULL;
		dbgHandle->heap = dbgHandle->heap->next){

		// This determines which base to use while searching the
		// stack.
		if(base == NULL){
			heapBase = (size_t) dbgHandle->heap->base;

		} else {
			heapBase = (size_t) base;

			// small catch in the case that the users provided
			// base is lower then the actual heaps base
			if(heapBase <= (size_t) dbgHandle->heap->base){
				heapBase = (size_t) dbgHandle->heap->base;
			}

		}

		heapTop  = (size_t) dbgHandle->heap->	top;

		// walk from low addresses to high addresses
		// comparing memory.
		while(heapBase < (heapTop-len) ){

			if(DebugCompareMemory(dbgHandle, (void *) heapBase, pattern, len) == ERR_SUCCESS){
				if(returnBase == 0){
					returnBase = heapBase;
				}

				dbgHandle->heap = heapFirst;
				return (void *) returnBase;
			}

			heapBase += 1;
		}

	}

	return NULL;
}


// determines if a heap has been overflowed: (Returns Heap Pointer)
//   Returns a pointer to the affected overflowed heap if found.
//	 If none are found to be overflowed, will return NULL.
P_PROCESS_HEAP DebugIsHeapOverflowed_GetHeap(P_DEBUG_HANDLE dbgHandle){

	if(dbgHandle == NULL)
		return NULL;

	if(dbgHandle->memory == NULL)
		return NULL;

	if(dbgHandle->heap == NULL)
		return NULL;

	if(dbgHandle->heap->heapChunks == NULL)
		return NULL;

	P_PROCESS_HEAP heapFirst      = dbgHandle->heap->first;
	P_HEAP_CHUNK   heapFirstChunk = dbgHandle->heap->heapChunks->first;

	P_PROCESS_HEAP overflowedHeap = NULL;

	// walk each heap
	for(dbgHandle->heap = dbgHandle->heap->first;
		dbgHandle->heap != NULL;
		dbgHandle->heap = dbgHandle->heap->next){

			// walk this heap looking for overflowed chunks
			for(dbgHandle->heap->heapChunks = dbgHandle->heap->heapChunks->first;
				dbgHandle->heap->heapChunks != NULL;
				dbgHandle->heap->heapChunks = dbgHandle->heap->heapChunks->next){

				// if we get here, the heap has been overflowed
				if(dbgHandle->heap->heapChunks->likelyOverflow > 0){
					overflowedHeap              = dbgHandle->heap;

					// handle reassignments
					dbgHandle->heap             = heapFirst;
					dbgHandle->heap->heapChunks = heapFirstChunk;

					return overflowedHeap;

				}

			}

	}

	// handle reassignments
	dbgHandle->heap = heapFirst;
	dbgHandle->heap->heapChunks = heapFirstChunk;

	return NULL;
}


//  OVERFLOW DETECTION - 0 DAY DETECTOR/GENERATORS

// determines if a heap has been overflowed: (returns chunk pointer)
//   Returns a pointer to the affected overflowed heap if found.
//	 If none are found to be overflowed, will return NULL.
P_HEAP_CHUNK DebugIsHeapOverflowed_GetChunk(P_DEBUG_HANDLE dbgHandle){

	if(dbgHandle == NULL)
		return NULL;

	if(dbgHandle->memory == NULL)
		return NULL;

	if(dbgHandle->heap == NULL)
		return NULL;

	if(dbgHandle->heap->heapChunks == NULL)
		return NULL;

	P_PROCESS_HEAP heapFirst      = dbgHandle->heap->first;
	P_HEAP_CHUNK   heapFirstChunk = dbgHandle->heap->heapChunks->first;

	P_HEAP_CHUNK overflowedHeap = NULL;

	// walk each heap
	for(dbgHandle->heap = dbgHandle->heap->first;
		dbgHandle->heap != NULL;
		dbgHandle->heap = dbgHandle->heap->next){

			// walk this heap looking for overflowed chunks
			for(dbgHandle->heap->heapChunks = dbgHandle->heap->heapChunks->first;
				dbgHandle->heap->heapChunks != NULL;
				dbgHandle->heap->heapChunks = dbgHandle->heap->heapChunks->next){

				// if we get here, the heap has been overflowed
				if(dbgHandle->heap->heapChunks->likelyOverflow > 0){
					overflowedHeap              = dbgHandle->heap->heapChunks;

					// handle reassignments
					dbgHandle->heap             = heapFirst;
					dbgHandle->heap->heapChunks = heapFirstChunk;

					return overflowedHeap;

				}

			}

	}

	// handle reassignments
	dbgHandle->heap = heapFirst;
	dbgHandle->heap->heapChunks = heapFirstChunk;

	return NULL;
}


// Gets the chunk belonging to a specific pointer occuring in the
// processes heap.
P_HEAP_CHUNK DebugHeapPointerGetChunk(P_DEBUG_HANDLE dbgHandle, void * heapPtr){
	if(dbgHandle == NULL)
		return NULL;

	if(dbgHandle->heap == NULL)
		return NULL;

	if(dbgHandle->heap->heapChunks == NULL)
		return NULL;



	// store firsts for restorative use before return
	P_HEAP_CHUNK heapChunkFirst = dbgHandle->heap->heapChunks->first;
	P_PROCESS_HEAP heapFirst = dbgHandle->heap;
	P_HEAP_CHUNK foundInHeap = NULL;

	// walk our existing program heaps isolating the heap heap chunk that contains our pointer
	for(dbgHandle->heap = dbgHandle->heap->first;
		dbgHandle->heap != NULL;
		dbgHandle->heap = dbgHandle->heap->next){



		// see if the heap pointer is in the range of our existing heap
		if( (size_t) dbgHandle->heap->top > (size_t) heapPtr &&
			(size_t) dbgHandle->heap->base < (size_t) heapPtr){

				// if its not greater then our top, or less than our bottom
				// its in this heap.
				for(dbgHandle->heap->heapChunks = dbgHandle->heap->heapChunks->first;
					dbgHandle->heap->heapChunks != NULL;
					dbgHandle->heap->heapChunks = dbgHandle->heap->heapChunks->next){



					// now determine heap
					if( (size_t) heapPtr  >= (size_t) dbgHandle->heap->heapChunks->base  &&
						(size_t) heapPtr  <= (size_t) dbgHandle->heap->heapChunks->base + (unsigned int) dbgHandle->heap->heapChunks->realSize){
						foundInHeap = dbgHandle->heap->heapChunks;

						// restore pointers
						dbgHandle->heap->heapChunks = heapChunkFirst;
						dbgHandle->heap = heapFirst;

						// return the valid heap chunk
						return foundInHeap;

					}


				}

		}

	}

	// restore pointers if the pointer was not found in any heaps
	dbgHandle->heap = heapFirst;
	dbgHandle->heap->heapChunks = heapChunkFirst;


	return NULL;
}

// Gets the heap associated with a certain heap pointer specified
P_PROCESS_HEAP DebugHeapPointerGetHeap(P_DEBUG_HANDLE dbgHandle, void * heapPtr){
	if(dbgHandle == NULL)
		return NULL;

	if(dbgHandle->heap == NULL)
		return NULL;

	if(dbgHandle->heap->heapChunks == NULL)
		return NULL;

	// store firsts for restorative use before return
	P_PROCESS_HEAP heapFirst = dbgHandle->heap;
	P_PROCESS_HEAP foundInHeap = NULL;

	// walk our existing program heaps isolating the heap heap chunk that contains our pointer
	for(dbgHandle->heap = dbgHandle->heap->first;
		dbgHandle->heap != NULL;
		dbgHandle->heap = dbgHandle->heap->next){

		// see if the heap pointer is in the range of our existing heap
		if( (size_t) dbgHandle->heap->top >= (size_t) heapPtr ||
			(size_t) dbgHandle->heap->base <= (size_t) heapPtr){


					foundInHeap = dbgHandle->heap;
					// restore pointer
					dbgHandle->heap = heapFirst;
					return foundInHeap;
		}

	}

	// restore pointer if the pointer was not found in any heap
	dbgHandle->heap = heapFirst;
	return NULL;
}



// --------------- SYSTEM CALL TRACING FUNCTIONS -----------





// continues a stepping loop (return policy returnAtCall, or returnAfterCall
P_BP_LINUX_SYSCALL_RESULT DebugSyscallTrace(P_BP_LINUX_SYSCALL_LOOKUP lookupTable, P_BP_LINUX_SYSCALL_RESULT storeResult, P_DEBUG_HANDLE dbgHandle, size_t returnPolicy){

	if(dbgHandle == NULL)
		return NULL;

	if(lookupTable == NULL)
		return NULL;

	// signal variables
	int status    = 0;
	int ptraceRet = 0;
	int retPid    = 0;

	// store syscall arguments on entry
	P_BP_LINUX_SYSCALL_RESULT retHandle = storeResult;
	P_BP_LINUX_SYSCALL lookupHandle = NULL;


	int ptraceReturn = 0;

	// -----------------------------------------------------------------
	if( (ptraceReturn != -1) &&
		(returnPolicy != SYS_RET_CONTINUE)
		){

		ptraceReturn = ptrace(PT_SYSCALL, dbgHandle->debugPid, NULL, NULL);

		// waits until the debugged application emits a signal
		retPid = waitpid(dbgHandle->debugPid, &status, 0);
		if(retPid != dbgHandle->debugPid)
			return NULL;

		// retrieve registers for (orig_eax)
		DebugGetRegisters(dbgHandle);

		// set memory pointer if null
		if(retHandle == NULL){
			retHandle = (P_BP_LINUX_SYSCALL_RESULT) bpmalloc(sizeof(BP_LINUX_SYSCALL_RESULT));
				memset(retHandle, 0x00, sizeof(BP_LINUX_SYSCALL_RESULT));
		} else {
			retHandle = storeResult;
		}


		// syscall will be eax at this point, look up number in table
		lookupHandle = DebugLookupSyscallByNumber(lookupTable, dbgHandle->orig_eax);

		// continue unknown systrace w/o recording
		if(lookupHandle == NULL){
			goto continue_systrace;
			return NULL;
		}

		// --------------------- SET STRUCTURE DATA --------------------------------
		// store the syscall number
		retHandle->number = dbgHandle->orig_eax;

		// set default ebx data from lookup table
		retHandle->ebxPtrDataType = lookupHandle->ebxPtrDataType;
		retHandle->ebxPtrDataSize = lookupHandle->ebxPtrDataSize;
		retHandle->ebxIsPtr = lookupHandle->ebxIsPtr;

		// set default ecx data from lookup table
		retHandle->ecxPtrDataType = lookupHandle->ecxPtrDataType;
		retHandle->ecxPtrDataSize = lookupHandle->ecxPtrDataSize;
		retHandle->ecxIsPtr = lookupHandle->ecxIsPtr;

		// set default edx data from lookup table
		retHandle->edxPtrDataType = lookupHandle->edxPtrDataType;
		retHandle->edxPtrDataSize = lookupHandle->edxPtrDataSize;
		retHandle->edxIsPtr = lookupHandle->edxIsPtr;

		// set default esi data from lookup table
		retHandle->esiPtrDataType = lookupHandle->esiPtrDataType;
		retHandle->esiPtrDataSize = lookupHandle->esiPtrDataSize;
		retHandle->esiIsPtr = lookupHandle->esiIsPtr;

		// set default edi data from lookup table
		retHandle->ediPtrDataType = lookupHandle->ediPtrDataType;
		retHandle->ediPtrDataSize = lookupHandle->ediPtrDataSize;
		retHandle->ediIsPtr = lookupHandle->ediIsPtr;

		// record parameters if used ---------------------------------

		// ebx PARMS IF USED
		if(lookupHandle->ebxUse == true){

			// set the ebx value for ret
			retHandle->ebx = dbgHandle->ebx;

			// if its a pointer
			if(lookupHandle->ebxIsPtr){

				switch(retHandle->ebxPtrDataType){

					case PARAM_NULL_TERM_CHAR_PTR:

						// read string from memory
						/*
						retHandle->ebxStoredData = (unsigned char *) DebugReadString(dbgHandle, (void *) retHandle->ebx, 2048);
						retHandle->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->ebxStoredData != NULL)
							retHandle->ebxPtrDataSize = bpstrlen((char *) retHandle->ebxStoredData);
							*/
						break;


					case PARAM_CHAR_PTR_ECX_SIZE:
						retHandle->ebxStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->ebx, retHandle->ecx );
						retHandle->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->ebxStoredData != NULL)
							retHandle->ebxPtrDataSize = bpstrlen((char *) retHandle->ecx);

						break;
					case PARAM_CHAR_PTR_EDX_SIZE:
						retHandle->ebxStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->ebx, retHandle->edx );
						retHandle->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->ebxStoredData != NULL)
							retHandle->ebxPtrDataSize = retHandle->edx;

						break;
					case PARAM_CHAR_PTR_ESI_SIZE:
						retHandle->ebxStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->ebx, retHandle->esi );
						retHandle->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->ebxStoredData != NULL)
							retHandle->ebxPtrDataSize = retHandle->esi;

						break;
					case PARAM_CHAR_PTR_EDI_SIZE:
						retHandle->ebxStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->ebx, retHandle->edi);
						retHandle->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->ebxStoredData != NULL)
							retHandle->ebxPtrDataSize = retHandle->edi;

						break;

					case PARAM_BIN_DATA:
						// read binary data from memory
						retHandle->ebxStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->ebx, lookupHandle->ebxPtrDataSize);
						retHandle->ebxPtrDataType = PARAM_BIN_DATA;
						retHandle->ebxPtrDataSize = lookupHandle->ebxPtrDataSize;

						break;

					default:
						break;

				}

			}

		}

		// -------------- ECX PARAMETER RECORDING ---------------------------------------
		// ecx PARMS IF USED
		if(lookupHandle->ecxUse == true){

			// set the ecx value for ret
			retHandle->ecx = dbgHandle->ecx;

			// if its a pointer
			if(lookupHandle->ecxIsPtr){

				switch(retHandle->ecxPtrDataType){

					case PARAM_NULL_TERM_CHAR_PTR:
						/*
						// read string from memory
						retHandle->ecxStoredData = (unsigned char *) DebugReadString(dbgHandle, (void *) retHandle->ecx, 2048);
						retHandle->ecxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->ecxStoredData != NULL)
							retHandle->ecxPtrDataSize = bpstrlen((char *) retHandle->ecxStoredData);
								*/
						break;


					case PARAM_CHAR_PTR_ECX_SIZE:
						retHandle->ecxStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->ecx, retHandle->ecx );
						retHandle->ecxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->ecxStoredData != NULL)
							retHandle->ecxPtrDataSize = bpstrlen((char *) retHandle->ecx);

						break;
					case PARAM_CHAR_PTR_EDX_SIZE:
						retHandle->ecxStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->ecx, retHandle->edx );
						retHandle->ecxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->ecxStoredData != NULL)
							retHandle->ecxPtrDataSize = retHandle->edx;

						break;
					case PARAM_CHAR_PTR_ESI_SIZE:
						retHandle->ecxStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->ecx, retHandle->esi );
						retHandle->ecxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->ecxStoredData != NULL)
							retHandle->ecxPtrDataSize = retHandle->esi;

						break;
					case PARAM_CHAR_PTR_EDI_SIZE:
						retHandle->ecxStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->ecx, retHandle->edi);
						retHandle->ecxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->ecxStoredData != NULL)
							retHandle->ecxPtrDataSize = retHandle->edi;

						break;

					case PARAM_BIN_DATA:
						// read binary data from memory
						retHandle->ecxStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->ecx, lookupHandle->ecxPtrDataSize);
						retHandle->ecxPtrDataType = PARAM_BIN_DATA;
						retHandle->ecxPtrDataSize = lookupHandle->ecxPtrDataSize;

						break;

					default:
						break;

				}

			}

		}

		// edx PARMS IF USED
		if(lookupHandle->edxUse == true){

			// set the edx value for ret
			retHandle->edx = dbgHandle->edx;

			// if its a pointer
			if(lookupHandle->edxIsPtr){

				switch(retHandle->edxPtrDataType){

					case PARAM_NULL_TERM_CHAR_PTR:
						/*
						// read string from memory
						retHandle->edxStoredData = (unsigned char *) DebugReadString(dbgHandle, (void *) retHandle->edx, 2048);
						retHandle->edxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->edxStoredData != NULL)
							retHandle->edxPtrDataSize = bpstrlen((char *) retHandle->edxStoredData);
							*/
						break;


					case PARAM_CHAR_PTR_ECX_SIZE:
						retHandle->edxStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->edx, retHandle->ecx );
						retHandle->edxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->edxStoredData != NULL)
							retHandle->edxPtrDataSize = bpstrlen((char *) retHandle->ecx);

						break;
					case PARAM_CHAR_PTR_EDX_SIZE:
						retHandle->edxStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->edx, retHandle->edx );
						retHandle->edxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->edxStoredData != NULL)
							retHandle->edxPtrDataSize = retHandle->edx;

						break;
					case PARAM_CHAR_PTR_ESI_SIZE:
						retHandle->edxStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->edx, retHandle->esi );
						retHandle->edxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->edxStoredData != NULL)
							retHandle->edxPtrDataSize = retHandle->esi;

						break;
					case PARAM_CHAR_PTR_EDI_SIZE:
						retHandle->edxStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->edx, retHandle->edi);
						retHandle->edxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->edxStoredData != NULL)
							retHandle->edxPtrDataSize = retHandle->edi;

						break;

					case PARAM_BIN_DATA:
						// read binary data from memory
						retHandle->edxStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->edx, lookupHandle->edxPtrDataSize);
						retHandle->edxPtrDataType = PARAM_BIN_DATA;
						retHandle->edxPtrDataSize = lookupHandle->edxPtrDataSize;

						break;

					default:
						break;

				}

			}

		}

		// esi PARMS IF USED
		if(lookupHandle->esiUse == true){

			// set the esi value for ret
			retHandle->esi = dbgHandle->esi;

			// if its a pointer
			if(lookupHandle->esiIsPtr){

				switch(retHandle->esiPtrDataType){

					case PARAM_NULL_TERM_CHAR_PTR:
						/*
						// read string from memory
						retHandle->esiStoredData = (unsigned char *) DebugReadString(dbgHandle, (void *) retHandle->esi, 2048);
						retHandle->esiPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->esiStoredData != NULL)
							retHandle->esiPtrDataSize = bpstrlen((char *) retHandle->esiStoredData);
								*/
						break;


					case PARAM_CHAR_PTR_ECX_SIZE:
						retHandle->esiStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->esi, retHandle->ecx );
						retHandle->esiPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->esiStoredData != NULL)
							retHandle->esiPtrDataSize = bpstrlen((char *) retHandle->ecx);

						break;
					case PARAM_CHAR_PTR_EDX_SIZE:
						retHandle->esiStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->esi, retHandle->edx );
						retHandle->esiPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->esiStoredData != NULL)
							retHandle->esiPtrDataSize = retHandle->edx;

						break;
					case PARAM_CHAR_PTR_ESI_SIZE:
						retHandle->esiStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->esi, retHandle->esi );
						retHandle->esiPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->esiStoredData != NULL)
							retHandle->esiPtrDataSize = retHandle->esi;

						break;
					case PARAM_CHAR_PTR_EDI_SIZE:
						retHandle->esiStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->esi, retHandle->edi);
						retHandle->esiPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->esiStoredData != NULL)
							retHandle->esiPtrDataSize = retHandle->edi;

						break;

					case PARAM_BIN_DATA:
						// read binary data from memory
						retHandle->esiStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->esi, lookupHandle->esiPtrDataSize);
						retHandle->esiPtrDataType = PARAM_BIN_DATA;
						retHandle->esiPtrDataSize = lookupHandle->esiPtrDataSize;

						break;

					default:
						break;

				}

			}

		}

		// edi PARMS IF USED
		if(lookupHandle->ediUse == true){

			// set the edi value for ret
			retHandle->edi = dbgHandle->edi;

			// if its a pointer
			if(lookupHandle->ediIsPtr){

				switch(retHandle->ediPtrDataType){

					case PARAM_NULL_TERM_CHAR_PTR:
						/*
						// read string from memory
						retHandle->ediStoredData = (unsigned char *) DebugReadString(dbgHandle, (void *) retHandle->edi, 2048);
						retHandle->ediPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->ediStoredData != NULL)
							retHandle->ediPtrDataSize = bpstrlen((char *) retHandle->ediStoredData);
								*/
						break;


					case PARAM_CHAR_PTR_ECX_SIZE:
						retHandle->ediStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->edi, retHandle->ecx );
						retHandle->ediPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->ediStoredData != NULL)
							retHandle->ediPtrDataSize = bpstrlen((char *) retHandle->ecx);

						break;
					case PARAM_CHAR_PTR_EDX_SIZE:
						retHandle->ediStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->edi, retHandle->edx );
						retHandle->ediPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->ediStoredData != NULL)
							retHandle->ediPtrDataSize = retHandle->edx;

						break;
					case PARAM_CHAR_PTR_ESI_SIZE:
						retHandle->ediStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->edi, retHandle->esi );
						retHandle->ediPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->ediStoredData != NULL)
							retHandle->ediPtrDataSize = retHandle->esi;

						break;
					case PARAM_CHAR_PTR_EDI_SIZE:
						retHandle->ediStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->edi, retHandle->edi);
						retHandle->ediPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
						if(retHandle->ediStoredData != NULL)
							retHandle->ediPtrDataSize = retHandle->edi;

						break;

					case PARAM_BIN_DATA:
						// read binary data from memory
						retHandle->ediStoredData = (unsigned char *) DebugReadMemory(dbgHandle, (void *) retHandle->edi, lookupHandle->ediPtrDataSize);
						retHandle->ediPtrDataType = PARAM_BIN_DATA;
						retHandle->ediPtrDataSize = lookupHandle->ediPtrDataSize;

						break;

					default:
						break;

				}

			}

		}


		// -------------- SYS_RET_BEFORE TYPE RETURN/EXIT -------------------------------

		// return handle early if policy is specified
		if(returnPolicy == SYS_RET_BEFORE)
			return retHandle;

	}


	// -----------------------------------------------------------------
	// continue call and record return value (eax)
	continue_systrace:
	if( (returnPolicy == SYS_RET_CONTINUE) ||
		(returnPolicy == SYS_RET_AFTER) ) {

		ptraceReturn = ptrace(PT_SYSCALL, dbgHandle->debugPid, NULL, NULL);

		// waits until the debugged application emits a signal
		retPid = waitpid(dbgHandle->debugPid, &status, 0);
		if(retPid != dbgHandle->debugPid)
				return NULL;
			// return ERR_DBG_WAIT_RETURNS_UNMATCHED_PID;

		// About: If an application is ptraced,
		// it will stop on the condition that a
		// signal occurs.  This is any signal that
		// the process emits.

		// if the process was stopped, store the signal
		if(WIFSTOPPED(status) > 0){
			dbgHandle->signal = WSTOPSIG(status);
			dbgHandle->sigDesc = L_ReturnStringFromSignal(dbgHandle);
			// since the process is stopped, it is now safe
			// to retrieve registers from the process.


			// eax will now equal the syscall number
			DebugGetRegisters(dbgHandle);

			if(retHandle != NULL)
				retHandle->returnVal = dbgHandle->eax;

			return retHandle;
			// return ERR_DBG_PROCESS_SIGNALED;
		} else {

			if(WIFEXITED(status) > 0){
				// if the process has exited normally
				// set the attached status to false.
				dbgHandle->attached = 0;
				return NULL;
				// return ERR_DBG_PROCESS_EXITED;
			}

			if(WIFSIGNALED(status) > 0){
				return NULL;
				// return ERR_DBG_PROCESS_SIGNALED;
			}
		}


	}


	return retHandle;
}

// destroys a syscall result
size_t DebugDestroySyscallResult(P_BP_LINUX_SYSCALL_RESULT result){
	if(result == NULL)
		return ERR_FAILURE;

	if(result->ebxStoredData != NULL)
		bpfree(result->ebxStoredData);

	if(result->ecxStoredData != NULL)
			bpfree(result->ecxStoredData);

	if(result->edxStoredData != NULL)
			bpfree(result->edxStoredData);

	if(result->esiStoredData != NULL)
			bpfree(result->esiStoredData);

	if(result->ediStoredData != NULL)
			bpfree(result->ediStoredData);



	// destroy overlapping structure
	bpfree(result);

	return ERR_SUCCESS;

}

// gets a syscall from a lookup table based on number
P_BP_LINUX_SYSCALL DebugLookupSyscallByNumber(P_BP_LINUX_SYSCALL_LOOKUP table, size_t number){

	// number verification checks
	if(table == NULL)
		return NULL;
	if(number > table->syscallCount)
		return NULL;
	if(number == 0)
		return NULL;

	// n-1 is the correct position of our syscall
	return table->syscalls[number-1];
}


// Determines if a datatype is a pointer type
bool DebugSyscallDatatypeIsPointer(size_t type){
	switch(type){
		case PARAM_NULL_TERM_CHAR_PTR:
			return true;
		case PARAM_CHAR_PTR_EBX_SIZE:
			return true;
		case PARAM_CHAR_PTR_ECX_SIZE:
			return true;
		case PARAM_CHAR_PTR_EDX_SIZE:
			return true;
		case PARAM_CHAR_PTR_ESI_SIZE:
			return true;
		case PARAM_CHAR_PTR_EDI_SIZE:
			return true;
		case PARAM_BIN_DATA:
			return true;
	}


		return false;
}


// Determines if a datatype is a pointer type
bool DebugSyscallDatatypeIsCounter(size_t type){
	switch(type){
		case PARAM_REG_ONLY:
			return true;
		case PARAM_FD:
			return true;
		case PARAM_COUNTER:
			return true;
		case PARAM_SIZE_EBX:
			return true;
		case PARAM_SIZE_ECX:
			return true;
		case PARAM_SIZE_EDX:
			return true;
		case PARAM_SIZE_ESI:
			return true;
		case PARAM_SIZE_EDI:
			return true;
	}

	return false;

}

// returns a linux system call table containing all LINUX syscalls

// LAST FUNCTION IN DBG FILE
P_BP_LINUX_SYSCALL_LOOKUP DebugGetLinuxSyscallLookupTable(){

	// allocate lookup structure
	P_BP_LINUX_SYSCALL_LOOKUP sysLookup = (P_BP_LINUX_SYSCALL_LOOKUP) bpmalloc(sizeof(BP_LINUX_SYSCALL_LOOKUP));
		memset(sysLookup, 0x00, sizeof(BP_LINUX_SYSCALL_LOOKUP));

	// linux
	sysLookup->platform = LINUX_IA32_SYSCALLS;

	// total count from 1
	sysLookup->syscallCount = 190;

	sysLookup->syscalls = (P_BP_LINUX_SYSCALL *) bpmalloc(sizeof(size_t) * sysLookup->syscallCount+10);
		memset(sysLookup->syscalls, 0x00, sizeof(size_t) * sysLookup->syscallCount+10);

	// printf("\n SYSCALL ALLOCATION COUNT: %i", sizeof(BP_LINUX_SYSCALL) * sysLookup->syscallCount+10);

	// IMPORTANT, LOOKUP REFERENCES WILL ALWAYS BE syscall number minus 1
	int sn = -1;


	// sys_exit (1) -------------------------------------------
	sysLookup->syscalls[sn+1] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	    memset(sysLookup->syscalls[sn+1], 0x00, sizeof(BP_LINUX_SYSCALL));
	sysLookup->syscalls[sn+1]->name = "sys_exit";
	sysLookup->syscalls[sn+1]->source = "kernel/exit.c";
	sysLookup->syscalls[sn+1]->number = 1;
	sysLookup->syscalls[sn+1]->returnSize = 4;
	    // EBX
	    sysLookup->syscalls[sn+1]->ebxDataSize = sizeof(size_t);
	    sysLookup->syscalls[sn+1]->ebxUse = true;
	    sysLookup->syscalls[sn+1]->ebxIsPtr = false;
	    sysLookup->syscalls[sn+1]->ebxPtrDataType = PARAM_BIN_DATA;
	    sysLookup->syscalls[sn+1]->ebxPtrDataSize = 1024;

	    // sys_fork (2) -------------------------------------------
	    sysLookup->syscalls[sn+2] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+2], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+2]->name = "sys_fork";
	    sysLookup->syscalls[sn+2]->source = "arch/i386/kernel/process.c";
	    sysLookup->syscalls[sn+2]->number = 2;
	    sysLookup->syscalls[sn+2]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+2]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+2]->ebxUse = true;
	        sysLookup->syscalls[sn+2]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+2]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+2]->ebxPtrDataSize = 1024;

	    // sys_read (3) -------------------------------------------
	    sysLookup->syscalls[sn+3] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+3], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+3]->name = "sys_read";
	    sysLookup->syscalls[sn+3]->source = "fs/read_write.c";
	    sysLookup->syscalls[sn+3]->number = 3;
	    sysLookup->syscalls[sn+3]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+3]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+3]->ebxUse = true;
	        sysLookup->syscalls[sn+3]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+3]->ebxPtrDataType = PARAM_FD;
	        sysLookup->syscalls[sn+3]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+3]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+3]->ecxUse = true;
	        sysLookup->syscalls[sn+3]->ecxIsPtr = true;
	        sysLookup->syscalls[sn+3]->ecxPtrDataType = PARAM_CHAR_PTR_EDX_SIZE;
	        sysLookup->syscalls[sn+3]->ecxPtrDataSize = 0;
	        // EDX
	        sysLookup->syscalls[sn+3]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+3]->edxUse = true;
	        sysLookup->syscalls[sn+3]->edxIsPtr = false;
	        sysLookup->syscalls[sn+3]->edxPtrDataType = PARAM_COUNTER;
	        sysLookup->syscalls[sn+3]->edxPtrDataSize = 1024;

	    // sys_write (4) -------------------------------------------
	    sysLookup->syscalls[sn+4] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+4], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+4]->name = "sys_write";
	    sysLookup->syscalls[sn+4]->source = "fs/read_write.c";
	    sysLookup->syscalls[sn+4]->number = 4;
	    sysLookup->syscalls[sn+4]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+4]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+4]->ebxUse = true;
	        sysLookup->syscalls[sn+4]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+4]->ebxPtrDataType = PARAM_FD;
	        sysLookup->syscalls[sn+4]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+4]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+4]->ecxUse = true;
	        sysLookup->syscalls[sn+4]->ecxIsPtr = true;
	        sysLookup->syscalls[sn+4]->ecxPtrDataType = PARAM_CHAR_PTR_EDX_SIZE;
	        sysLookup->syscalls[sn+4]->ecxPtrDataSize = 0;
	        // EDX
	        sysLookup->syscalls[sn+4]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+4]->edxUse = true;
	        sysLookup->syscalls[sn+4]->edxIsPtr = false;
	        sysLookup->syscalls[sn+4]->edxPtrDataType = PARAM_COUNTER;
	        sysLookup->syscalls[sn+4]->edxPtrDataSize = 1024;

	    // sys_open (5) -------------------------------------------
	    sysLookup->syscalls[sn+5] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+5], 0x00, sizeof(BP_LINUX_SYSCALL));

	    sysLookup->syscalls[sn+5]->name = "sys_open";
	    sysLookup->syscalls[sn+5]->source = "fs/open.c";
	    sysLookup->syscalls[sn+5]->number = 5;
	    sysLookup->syscalls[sn+5]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+5]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+5]->ebxUse = true;
	        sysLookup->syscalls[sn+5]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+5]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+5]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+5]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+5]->ecxUse = true;
	        sysLookup->syscalls[sn+5]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+5]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+5]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+5]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+5]->edxUse = true;
	        sysLookup->syscalls[sn+5]->edxIsPtr = false;
	        sysLookup->syscalls[sn+5]->edxPtrDataType = PARAM_REG_ONLY;
	        sysLookup->syscalls[sn+5]->edxPtrDataSize = 1024;

	    // sys_close (6) -------------------------------------------
	    sysLookup->syscalls[sn+6] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+6], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+6]->name = "sys_close";
	    sysLookup->syscalls[sn+6]->source = "fs/open.c";
	    sysLookup->syscalls[sn+6]->number = 6;
	    sysLookup->syscalls[sn+6]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+6]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+6]->ebxUse = true;
	        sysLookup->syscalls[sn+6]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+6]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+6]->ebxPtrDataSize = 1024;

	    // sys_waitpid (7) -------------------------------------------
	    sysLookup->syscalls[sn+7] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+7], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+7]->name = "sys_waitpid";
	    sysLookup->syscalls[sn+7]->source = "kernel/exit.c";
	    sysLookup->syscalls[sn+7]->number = 7;
	    sysLookup->syscalls[sn+7]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+7]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+7]->ebxUse = true;
	        sysLookup->syscalls[sn+7]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+7]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+7]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+7]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+7]->ecxUse = true;
	        sysLookup->syscalls[sn+7]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+7]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+7]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+7]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+7]->edxUse = true;
	        sysLookup->syscalls[sn+7]->edxIsPtr = false;
	        sysLookup->syscalls[sn+7]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+7]->edxPtrDataSize = 1024;

	    // sys_creat (8) -------------------------------------------
	    sysLookup->syscalls[sn+8] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+8], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+8]->name = "sys_creat";
	    sysLookup->syscalls[sn+8]->source = "fs/open.c";
	    sysLookup->syscalls[sn+8]->number = 8;
	    sysLookup->syscalls[sn+8]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+8]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+8]->ebxUse = true;
	        sysLookup->syscalls[sn+8]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+8]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+8]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+8]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+8]->ecxUse = true;
	        sysLookup->syscalls[sn+8]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+8]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+8]->ecxPtrDataSize = 1024;

	    // sys_link (9) -------------------------------------------
	    sysLookup->syscalls[sn+9] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+9], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+9]->name = "sys_link";
	    sysLookup->syscalls[sn+9]->source = "fs/namei.c";
	    sysLookup->syscalls[sn+9]->number = 9;
	    sysLookup->syscalls[sn+9]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+9]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+9]->ebxUse = true;
	        sysLookup->syscalls[sn+9]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+9]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+9]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+9]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+9]->ecxUse = true;
	        sysLookup->syscalls[sn+9]->ecxIsPtr = true;
	        sysLookup->syscalls[sn+9]->ecxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+9]->ecxPtrDataSize = 0;

	    // sys_unlink (10) -------------------------------------------
	    sysLookup->syscalls[sn+10] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+10], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+10]->name = "sys_unlink";
	    sysLookup->syscalls[sn+10]->source = "fs/namei.c";
	    sysLookup->syscalls[sn+10]->number = 10;
	    sysLookup->syscalls[sn+10]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+10]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+10]->ebxUse = true;
	        sysLookup->syscalls[sn+10]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+10]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+10]->ebxPtrDataSize = 0;

	    // sys_execve (11) -------------------------------------------
	    sysLookup->syscalls[sn+11] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+11], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+11]->name = "sys_execve";
	    sysLookup->syscalls[sn+11]->source = "arch/i386/kernel/process.c";
	    sysLookup->syscalls[sn+11]->number = 11;
	    sysLookup->syscalls[sn+11]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+11]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+11]->ebxUse = true;
	        sysLookup->syscalls[sn+11]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+11]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+11]->ebxPtrDataSize = 1024;

	    // sys_chdir (12) -------------------------------------------
	    sysLookup->syscalls[sn+12] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+12], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+12]->name = "sys_chdir";
	    sysLookup->syscalls[sn+12]->source = "fs/open.c";
	    sysLookup->syscalls[sn+12]->number = 12;
	    sysLookup->syscalls[sn+12]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+12]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+12]->ebxUse = true;
	        sysLookup->syscalls[sn+12]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+12]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+12]->ebxPtrDataSize = 0;

	    // sys_time (13) -------------------------------------------
	    sysLookup->syscalls[sn+13] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+13], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+13]->name = "sys_time";
	    sysLookup->syscalls[sn+13]->source = "kernel/time.c";
	    sysLookup->syscalls[sn+13]->number = 13;
	    sysLookup->syscalls[sn+13]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+13]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+13]->ebxUse = true;
	        sysLookup->syscalls[sn+13]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+13]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+13]->ebxPtrDataSize = 1024;

	    // sys_mknod (14) -------------------------------------------
	    sysLookup->syscalls[sn+14] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+14], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+14]->name = "sys_mknod";
	    sysLookup->syscalls[sn+14]->source = "fs/namei.c";
	    sysLookup->syscalls[sn+14]->number = 14;
	    sysLookup->syscalls[sn+14]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+14]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+14]->ebxUse = true;
	        sysLookup->syscalls[sn+14]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+14]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+14]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+14]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+14]->ecxUse = true;
	        sysLookup->syscalls[sn+14]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+14]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+14]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+14]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+14]->edxUse = true;
	        sysLookup->syscalls[sn+14]->edxIsPtr = false;
	        sysLookup->syscalls[sn+14]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+14]->edxPtrDataSize = 1024;

	    // sys_chmod (15) -------------------------------------------
	    sysLookup->syscalls[sn+15] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+15], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+15]->name = "sys_chmod";
	    sysLookup->syscalls[sn+15]->source = "fs/open.c";
	    sysLookup->syscalls[sn+15]->number = 15;
	    sysLookup->syscalls[sn+15]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+15]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+15]->ebxUse = true;
	        sysLookup->syscalls[sn+15]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+15]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+15]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+15]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+15]->ecxUse = true;
	        sysLookup->syscalls[sn+15]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+15]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+15]->ecxPtrDataSize = 1024;

	    // sys_lchown (16) -------------------------------------------
	    sysLookup->syscalls[sn+16] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+16], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+16]->name = "sys_lchown";
	    sysLookup->syscalls[sn+16]->source = "fs/open.c";
	    sysLookup->syscalls[sn+16]->number = 16;
	    sysLookup->syscalls[sn+16]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+16]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+16]->ebxUse = true;
	        sysLookup->syscalls[sn+16]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+16]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+16]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+16]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+16]->ecxUse = true;
	        sysLookup->syscalls[sn+16]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+16]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+16]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+16]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+16]->edxUse = true;
	        sysLookup->syscalls[sn+16]->edxIsPtr = false;
	        sysLookup->syscalls[sn+16]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+16]->edxPtrDataSize = 1024;

	    // sys_stat (18) -------------------------------------------
	    sysLookup->syscalls[sn+18] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+18], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+18]->name = "sys_stat";
	    sysLookup->syscalls[sn+18]->source = "fs/stat.c";
	    sysLookup->syscalls[sn+18]->number = 18;
	    sysLookup->syscalls[sn+18]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+18]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+18]->ebxUse = true;
	        sysLookup->syscalls[sn+18]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+18]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+18]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+18]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+18]->ecxUse = true;
	        sysLookup->syscalls[sn+18]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+18]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+18]->ecxPtrDataSize = 1024;

	    // sys_lseek (19) -------------------------------------------
	    sysLookup->syscalls[sn+19] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+19], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+19]->name = "sys_lseek";
	    sysLookup->syscalls[sn+19]->source = "fs/read_write.c";
	    sysLookup->syscalls[sn+19]->number = 19;
	    sysLookup->syscalls[sn+19]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+19]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+19]->ebxUse = true;
	        sysLookup->syscalls[sn+19]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+19]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+19]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+19]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+19]->ecxUse = true;
	        sysLookup->syscalls[sn+19]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+19]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+19]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+19]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+19]->edxUse = true;
	        sysLookup->syscalls[sn+19]->edxIsPtr = false;
	        sysLookup->syscalls[sn+19]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+19]->edxPtrDataSize = 1024;

	    // sys_getpid (20) -------------------------------------------
	    sysLookup->syscalls[sn+20] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+20], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+20]->name = "sys_getpid";
	    sysLookup->syscalls[sn+20]->source = "kernel/sched.c";
	    sysLookup->syscalls[sn+20]->number = 20;
	    sysLookup->syscalls[sn+20]->returnSize = 4;

	    // sys_mount (21) -------------------------------------------
	    sysLookup->syscalls[sn+21] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+21], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+21]->name = "sys_mount";
	    sysLookup->syscalls[sn+21]->source = "fs/super.c";
	    sysLookup->syscalls[sn+21]->number = 21;
	    sysLookup->syscalls[sn+21]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+21]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+21]->ebxUse = true;
	        sysLookup->syscalls[sn+21]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+21]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+21]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+21]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+21]->ecxUse = true;
	        sysLookup->syscalls[sn+21]->ecxIsPtr = true;
	        sysLookup->syscalls[sn+21]->ecxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+21]->ecxPtrDataSize = 0;
	        // EDX
	        sysLookup->syscalls[sn+21]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+21]->edxUse = true;
	        sysLookup->syscalls[sn+21]->edxIsPtr = true;
	        sysLookup->syscalls[sn+21]->edxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+21]->edxPtrDataSize = 0;

	    // sys_oldumount (22) -------------------------------------------
	    sysLookup->syscalls[sn+22] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+22], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+22]->name = "sys_oldumount";
	    sysLookup->syscalls[sn+22]->source = "fs/super.c";
	    sysLookup->syscalls[sn+22]->number = 22;
	    sysLookup->syscalls[sn+22]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+22]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+22]->ebxUse = true;
	        sysLookup->syscalls[sn+22]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+22]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+22]->ebxPtrDataSize = 0;

	    // sys_setuid (23) -------------------------------------------
	    sysLookup->syscalls[sn+23] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+23], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+23]->name = "sys_setuid";
	    sysLookup->syscalls[sn+23]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+23]->number = 23;
	    sysLookup->syscalls[sn+23]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+23]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+23]->ebxUse = true;
	        sysLookup->syscalls[sn+23]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+23]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+23]->ebxPtrDataSize = 1024;

	    // sys_getuid (24) -------------------------------------------
	    sysLookup->syscalls[sn+24] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+24], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+24]->name = "sys_getuid";
	    sysLookup->syscalls[sn+24]->source = "kernel/sched.c";
	    sysLookup->syscalls[sn+24]->number = 24;
	    sysLookup->syscalls[sn+24]->returnSize = 4;

	    // sys_stime (25) -------------------------------------------
	    sysLookup->syscalls[sn+25] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+25], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+25]->name = "sys_stime";
	    sysLookup->syscalls[sn+25]->source = "kernel/time.c";
	    sysLookup->syscalls[sn+25]->number = 25;
	    sysLookup->syscalls[sn+25]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+25]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+25]->ebxUse = true;
	        sysLookup->syscalls[sn+25]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+25]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+25]->ebxPtrDataSize = 1024;

	    // sys_ptrace (26) -------------------------------------------
	    sysLookup->syscalls[sn+26] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+26], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+26]->name = "sys_ptrace";
	    sysLookup->syscalls[sn+26]->source = "arch/i386/kernel/ptrace.c";
	    sysLookup->syscalls[sn+26]->number = 26;
	    sysLookup->syscalls[sn+26]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+26]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+26]->ebxUse = true;
	        sysLookup->syscalls[sn+26]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+26]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+26]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+26]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+26]->ecxUse = true;
	        sysLookup->syscalls[sn+26]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+26]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+26]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+26]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+26]->edxUse = true;
	        sysLookup->syscalls[sn+26]->edxIsPtr = false;
	        sysLookup->syscalls[sn+26]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+26]->edxPtrDataSize = 1024;
	        // ESI
	        sysLookup->syscalls[sn+26]->esiDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+26]->esiUse = true;
	        sysLookup->syscalls[sn+26]->esiIsPtr = false;
	        sysLookup->syscalls[sn+26]->esiPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+26]->esiPtrDataSize = 1024;

	    // sys_alarm (27) -------------------------------------------
	    sysLookup->syscalls[sn+27] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+27], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+27]->name = "sys_alarm";
	    sysLookup->syscalls[sn+27]->source = "kernel/sched.c";
	    sysLookup->syscalls[sn+27]->number = 27;
	    sysLookup->syscalls[sn+27]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+27]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+27]->ebxUse = true;
	        sysLookup->syscalls[sn+27]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+27]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+27]->ebxPtrDataSize = 1024;

	    // sys_fstat (28) -------------------------------------------
	    sysLookup->syscalls[sn+28] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+28], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+28]->name = "sys_fstat";
	    sysLookup->syscalls[sn+28]->source = "fs/stat.c";
	    sysLookup->syscalls[sn+28]->number = 28;
	    sysLookup->syscalls[sn+28]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+28]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+28]->ebxUse = true;
	        sysLookup->syscalls[sn+28]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+28]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+28]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+28]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+28]->ecxUse = true;
	        sysLookup->syscalls[sn+28]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+28]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+28]->ecxPtrDataSize = 1024;

	    // sys_pause (29) -------------------------------------------
	    sysLookup->syscalls[sn+29] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+29], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+29]->name = "sys_pause";
	    sysLookup->syscalls[sn+29]->source = "arch/i386/kernel/sys_i386.c";
	    sysLookup->syscalls[sn+29]->number = 29;
	    sysLookup->syscalls[sn+29]->returnSize = 4;

	    // sys_utime (30) -------------------------------------------
	    sysLookup->syscalls[sn+30] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+30], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+30]->name = "sys_utime";
	    sysLookup->syscalls[sn+30]->source = "fs/open.c";
	    sysLookup->syscalls[sn+30]->number = 30;
	    sysLookup->syscalls[sn+30]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+30]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+30]->ebxUse = true;
	        sysLookup->syscalls[sn+30]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+30]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+30]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+30]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+30]->ecxUse = true;
	        sysLookup->syscalls[sn+30]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+30]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+30]->ecxPtrDataSize = 1024;

	    // sys_access (33) -------------------------------------------
	    sysLookup->syscalls[sn+33] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+33], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+33]->name = "sys_access";
	    sysLookup->syscalls[sn+33]->source = "fs/open.c";
	    sysLookup->syscalls[sn+33]->number = 33;
	    sysLookup->syscalls[sn+33]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+33]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+33]->ebxUse = true;
	        sysLookup->syscalls[sn+33]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+33]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+33]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+33]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+33]->ecxUse = true;
	        sysLookup->syscalls[sn+33]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+33]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+33]->ecxPtrDataSize = 1024;

	    // sys_nice (34) -------------------------------------------
	    sysLookup->syscalls[sn+34] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+34], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+34]->name = "sys_nice";
	    sysLookup->syscalls[sn+34]->source = "kernel/sched.c";
	    sysLookup->syscalls[sn+34]->number = 34;
	    sysLookup->syscalls[sn+34]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+34]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+34]->ebxUse = true;
	        sysLookup->syscalls[sn+34]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+34]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+34]->ebxPtrDataSize = 1024;

	    // sys_sync (36) -------------------------------------------
	    sysLookup->syscalls[sn+36] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+36], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+36]->name = "sys_sync";
	    sysLookup->syscalls[sn+36]->source = "fs/buffer.c";
	    sysLookup->syscalls[sn+36]->number = 36;
	    sysLookup->syscalls[sn+36]->returnSize = 4;

	    // sys_kill (37) -------------------------------------------
	    sysLookup->syscalls[sn+37] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+37], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+37]->name = "sys_kill";
	    sysLookup->syscalls[sn+37]->source = "kernel/signal.c";
	    sysLookup->syscalls[sn+37]->number = 37;
	    sysLookup->syscalls[sn+37]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+37]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+37]->ebxUse = true;
	        sysLookup->syscalls[sn+37]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+37]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+37]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+37]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+37]->ecxUse = true;
	        sysLookup->syscalls[sn+37]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+37]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+37]->ecxPtrDataSize = 1024;

	    // sys_rename (38) -------------------------------------------
	    sysLookup->syscalls[sn+38] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+38], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+38]->name = "sys_rename";
	    sysLookup->syscalls[sn+38]->source = "fs/namei.c";
	    sysLookup->syscalls[sn+38]->number = 38;
	    sysLookup->syscalls[sn+38]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+38]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+38]->ebxUse = true;
	        sysLookup->syscalls[sn+38]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+38]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+38]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+38]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+38]->ecxUse = true;
	        sysLookup->syscalls[sn+38]->ecxIsPtr = true;
	        sysLookup->syscalls[sn+38]->ecxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+38]->ecxPtrDataSize = 0;

	    // sys_mkdir (39) -------------------------------------------
	    sysLookup->syscalls[sn+39] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+39], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+39]->name = "sys_mkdir";
	    sysLookup->syscalls[sn+39]->source = "fs/namei.c";
	    sysLookup->syscalls[sn+39]->number = 39;
	    sysLookup->syscalls[sn+39]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+39]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+39]->ebxUse = true;
	        sysLookup->syscalls[sn+39]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+39]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+39]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+39]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+39]->ecxUse = true;
	        sysLookup->syscalls[sn+39]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+39]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+39]->ecxPtrDataSize = 1024;

	    // sys_rmdir (40) -------------------------------------------
	    sysLookup->syscalls[sn+40] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+40], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+40]->name = "sys_rmdir";
	    sysLookup->syscalls[sn+40]->source = "fs/namei.c";
	    sysLookup->syscalls[sn+40]->number = 40;
	    sysLookup->syscalls[sn+40]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+40]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+40]->ebxUse = true;
	        sysLookup->syscalls[sn+40]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+40]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+40]->ebxPtrDataSize = 0;

	    // sys_dup (41) -------------------------------------------
	    sysLookup->syscalls[sn+41] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+41], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+41]->name = "sys_dup";
	    sysLookup->syscalls[sn+41]->source = "fs/fcntl.c";
	    sysLookup->syscalls[sn+41]->number = 41;
	    sysLookup->syscalls[sn+41]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+41]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+41]->ebxUse = true;
	        sysLookup->syscalls[sn+41]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+41]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+41]->ebxPtrDataSize = 1024;

	    // sys_pipe (42) -------------------------------------------
	    sysLookup->syscalls[sn+42] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+42], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+42]->name = "sys_pipe";
	    sysLookup->syscalls[sn+42]->source = "arch/i386/kernel/sys_i386.c";
	    sysLookup->syscalls[sn+42]->number = 42;
	    sysLookup->syscalls[sn+42]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+42]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+42]->ebxUse = true;
	        sysLookup->syscalls[sn+42]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+42]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+42]->ebxPtrDataSize = 1024;

	    // sys_times (43) -------------------------------------------
	    sysLookup->syscalls[sn+43] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+43], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+43]->name = "sys_times";
	    sysLookup->syscalls[sn+43]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+43]->number = 43;
	    sysLookup->syscalls[sn+43]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+43]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+43]->ebxUse = true;
	        sysLookup->syscalls[sn+43]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+43]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+43]->ebxPtrDataSize = 1024;

	    // sys_brk (45) -------------------------------------------
	    sysLookup->syscalls[sn+45] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+45], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+45]->name = "sys_brk";
	    sysLookup->syscalls[sn+45]->source = "mm/mmap.c";
	    sysLookup->syscalls[sn+45]->number = 45;
	    sysLookup->syscalls[sn+45]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+45]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+45]->ebxUse = true;
	        sysLookup->syscalls[sn+45]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+45]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+45]->ebxPtrDataSize = 1024;

	    // sys_setgid (46) -------------------------------------------
	    sysLookup->syscalls[sn+46] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+46], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+46]->name = "sys_setgid";
	    sysLookup->syscalls[sn+46]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+46]->number = 46;
	    sysLookup->syscalls[sn+46]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+46]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+46]->ebxUse = true;
	        sysLookup->syscalls[sn+46]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+46]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+46]->ebxPtrDataSize = 1024;

	    // sys_getgid (47) -------------------------------------------
	    sysLookup->syscalls[sn+47] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+47], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+47]->name = "sys_getgid";
	    sysLookup->syscalls[sn+47]->source = "kernel/sched.c";
	    sysLookup->syscalls[sn+47]->number = 47;
	    sysLookup->syscalls[sn+47]->returnSize = 4;

	    // sys_signal (48) -------------------------------------------
	    sysLookup->syscalls[sn+48] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+48], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+48]->name = "sys_signal";
	    sysLookup->syscalls[sn+48]->source = "kernel/signal.c";
	    sysLookup->syscalls[sn+48]->number = 48;
	    sysLookup->syscalls[sn+48]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+48]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+48]->ebxUse = true;
	        sysLookup->syscalls[sn+48]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+48]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+48]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+48]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+48]->ecxUse = true;
	        sysLookup->syscalls[sn+48]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+48]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+48]->ecxPtrDataSize = 1024;

	    // sys_geteuid (49) -------------------------------------------
	    sysLookup->syscalls[sn+49] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+49], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+49]->name = "sys_geteuid";
	    sysLookup->syscalls[sn+49]->source = "kernel/sched.c";
	    sysLookup->syscalls[sn+49]->number = 49;
	    sysLookup->syscalls[sn+49]->returnSize = 4;

	    // sys_getegid (50) -------------------------------------------
	    sysLookup->syscalls[sn+50] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+50], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+50]->name = "sys_getegid";
	    sysLookup->syscalls[sn+50]->source = "kernel/sched.c";
	    sysLookup->syscalls[sn+50]->number = 50;
	    sysLookup->syscalls[sn+50]->returnSize = 4;

	    // sys_acct (51) -------------------------------------------
	    sysLookup->syscalls[sn+51] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+51], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+51]->name = "sys_acct";
	    sysLookup->syscalls[sn+51]->source = "kernel/acct.c";
	    sysLookup->syscalls[sn+51]->number = 51;
	    sysLookup->syscalls[sn+51]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+51]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+51]->ebxUse = true;
	        sysLookup->syscalls[sn+51]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+51]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+51]->ebxPtrDataSize = 0;

	    // sys_umount (52) -------------------------------------------
	    sysLookup->syscalls[sn+52] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+52], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+52]->name = "sys_umount";
	    sysLookup->syscalls[sn+52]->source = "fs/super.c";
	    sysLookup->syscalls[sn+52]->number = 52;
	    sysLookup->syscalls[sn+52]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+52]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+52]->ebxUse = true;
	        sysLookup->syscalls[sn+52]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+52]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+52]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+52]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+52]->ecxUse = true;
	        sysLookup->syscalls[sn+52]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+52]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+52]->ecxPtrDataSize = 1024;

	    // sys_ioctl (54) -------------------------------------------
	    sysLookup->syscalls[sn+54] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+54], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+54]->name = "sys_ioctl";
	    sysLookup->syscalls[sn+54]->source = "fs/ioctl.c";
	    sysLookup->syscalls[sn+54]->number = 54;
	    sysLookup->syscalls[sn+54]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+54]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+54]->ebxUse = true;
	        sysLookup->syscalls[sn+54]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+54]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+54]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+54]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+54]->ecxUse = true;
	        sysLookup->syscalls[sn+54]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+54]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+54]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+54]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+54]->edxUse = true;
	        sysLookup->syscalls[sn+54]->edxIsPtr = false;
	        sysLookup->syscalls[sn+54]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+54]->edxPtrDataSize = 1024;

	    // sys_fcntl (55) -------------------------------------------
	    sysLookup->syscalls[sn+55] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+55], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+55]->name = "sys_fcntl";
	    sysLookup->syscalls[sn+55]->source = "fs/fcntl.c";
	    sysLookup->syscalls[sn+55]->number = 55;
	    sysLookup->syscalls[sn+55]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+55]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+55]->ebxUse = true;
	        sysLookup->syscalls[sn+55]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+55]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+55]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+55]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+55]->ecxUse = true;
	        sysLookup->syscalls[sn+55]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+55]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+55]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+55]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+55]->edxUse = true;
	        sysLookup->syscalls[sn+55]->edxIsPtr = false;
	        sysLookup->syscalls[sn+55]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+55]->edxPtrDataSize = 1024;

	    // sys_setpgid (57) -------------------------------------------
	    sysLookup->syscalls[sn+57] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+57], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+57]->name = "sys_setpgid";
	    sysLookup->syscalls[sn+57]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+57]->number = 57;
	    sysLookup->syscalls[sn+57]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+57]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+57]->ebxUse = true;
	        sysLookup->syscalls[sn+57]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+57]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+57]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+57]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+57]->ecxUse = true;
	        sysLookup->syscalls[sn+57]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+57]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+57]->ecxPtrDataSize = 1024;

	    // sys_olduname (59) -------------------------------------------
	    sysLookup->syscalls[sn+59] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+59], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+59]->name = "sys_olduname";
	    sysLookup->syscalls[sn+59]->source = "arch/i386/kernel/sys_i386.c";
	    sysLookup->syscalls[sn+59]->number = 59;
	    sysLookup->syscalls[sn+59]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+59]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+59]->ebxUse = true;
	        sysLookup->syscalls[sn+59]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+59]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+59]->ebxPtrDataSize = 1024;

	    // sys_umask (60) -------------------------------------------
	    sysLookup->syscalls[sn+60] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+60], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+60]->name = "sys_umask";
	    sysLookup->syscalls[sn+60]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+60]->number = 60;
	    sysLookup->syscalls[sn+60]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+60]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+60]->ebxUse = true;
	        sysLookup->syscalls[sn+60]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+60]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+60]->ebxPtrDataSize = 1024;

	    // sys_chroot (61) -------------------------------------------
	    sysLookup->syscalls[sn+61] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+61], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+61]->name = "sys_chroot";
	    sysLookup->syscalls[sn+61]->source = "fs/open.c";
	    sysLookup->syscalls[sn+61]->number = 61;
	    sysLookup->syscalls[sn+61]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+61]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+61]->ebxUse = true;
	        sysLookup->syscalls[sn+61]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+61]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+61]->ebxPtrDataSize = 0;

	    // sys_ustat (62) -------------------------------------------
	    sysLookup->syscalls[sn+62] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+62], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+62]->name = "sys_ustat";
	    sysLookup->syscalls[sn+62]->source = "fs/super.c";
	    sysLookup->syscalls[sn+62]->number = 62;
	    sysLookup->syscalls[sn+62]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+62]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+62]->ebxUse = true;
	        sysLookup->syscalls[sn+62]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+62]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+62]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+62]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+62]->ecxUse = true;
	        sysLookup->syscalls[sn+62]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+62]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+62]->ecxPtrDataSize = 1024;

	    // sys_dup2 (63) -------------------------------------------
	    sysLookup->syscalls[sn+63] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+63], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+63]->name = "sys_dup2";
	    sysLookup->syscalls[sn+63]->source = "fs/fcntl.c";
	    sysLookup->syscalls[sn+63]->number = 63;
	    sysLookup->syscalls[sn+63]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+63]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+63]->ebxUse = true;
	        sysLookup->syscalls[sn+63]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+63]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+63]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+63]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+63]->ecxUse = true;
	        sysLookup->syscalls[sn+63]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+63]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+63]->ecxPtrDataSize = 1024;

	    // sys_getppid (64) -------------------------------------------
	    sysLookup->syscalls[sn+64] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+64], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+64]->name = "sys_getppid";
	    sysLookup->syscalls[sn+64]->source = "kernel/sched.c";
	    sysLookup->syscalls[sn+64]->number = 64;
	    sysLookup->syscalls[sn+64]->returnSize = 4;

	    // sys_getpgrp (65) -------------------------------------------
	    sysLookup->syscalls[sn+65] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+65], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+65]->name = "sys_getpgrp";
	    sysLookup->syscalls[sn+65]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+65]->number = 65;
	    sysLookup->syscalls[sn+65]->returnSize = 4;

	    // sys_setsid (66) -------------------------------------------
	    sysLookup->syscalls[sn+66] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+66], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+66]->name = "sys_setsid";
	    sysLookup->syscalls[sn+66]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+66]->number = 66;
	    sysLookup->syscalls[sn+66]->returnSize = 4;

	    // sys_sigaction (67) -------------------------------------------
	    sysLookup->syscalls[sn+67] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+67], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+67]->name = "sys_sigaction";
	    sysLookup->syscalls[sn+67]->source = "arch/i386/kernel/signal.c";
	    sysLookup->syscalls[sn+67]->number = 67;
	    sysLookup->syscalls[sn+67]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+67]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+67]->ebxUse = true;
	        sysLookup->syscalls[sn+67]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+67]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+67]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+67]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+67]->ecxUse = true;
	        sysLookup->syscalls[sn+67]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+67]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+67]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+67]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+67]->edxUse = true;
	        sysLookup->syscalls[sn+67]->edxIsPtr = false;
	        sysLookup->syscalls[sn+67]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+67]->edxPtrDataSize = 1024;

	    // sys_sgetmask (68) -------------------------------------------
	    sysLookup->syscalls[sn+68] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+68], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+68]->name = "sys_sgetmask";
	    sysLookup->syscalls[sn+68]->source = "kernel/signal.c";
	    sysLookup->syscalls[sn+68]->number = 68;
	    sysLookup->syscalls[sn+68]->returnSize = 4;

	    // sys_ssetmask (69) -------------------------------------------
	    sysLookup->syscalls[sn+69] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+69], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+69]->name = "sys_ssetmask";
	    sysLookup->syscalls[sn+69]->source = "kernel/signal.c";
	    sysLookup->syscalls[sn+69]->number = 69;
	    sysLookup->syscalls[sn+69]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+69]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+69]->ebxUse = true;
	        sysLookup->syscalls[sn+69]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+69]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+69]->ebxPtrDataSize = 1024;

	    // sys_setreuid (70) -------------------------------------------
	    sysLookup->syscalls[sn+70] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+70], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+70]->name = "sys_setreuid";
	    sysLookup->syscalls[sn+70]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+70]->number = 70;
	    sysLookup->syscalls[sn+70]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+70]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+70]->ebxUse = true;
	        sysLookup->syscalls[sn+70]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+70]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+70]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+70]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+70]->ecxUse = true;
	        sysLookup->syscalls[sn+70]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+70]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+70]->ecxPtrDataSize = 1024;

	    // sys_setregid (71) -------------------------------------------
	    sysLookup->syscalls[sn+71] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+71], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+71]->name = "sys_setregid";
	    sysLookup->syscalls[sn+71]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+71]->number = 71;
	    sysLookup->syscalls[sn+71]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+71]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+71]->ebxUse = true;
	        sysLookup->syscalls[sn+71]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+71]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+71]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+71]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+71]->ecxUse = true;
	        sysLookup->syscalls[sn+71]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+71]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+71]->ecxPtrDataSize = 1024;

	    // sys_sigsuspend (72) -------------------------------------------
	    sysLookup->syscalls[sn+72] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+72], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+72]->name = "sys_sigsuspend";
	    sysLookup->syscalls[sn+72]->source = "arch/i386/kernel/signal.c";
	    sysLookup->syscalls[sn+72]->number = 72;
	    sysLookup->syscalls[sn+72]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+72]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+72]->ebxUse = true;
	        sysLookup->syscalls[sn+72]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+72]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+72]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+72]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+72]->ecxUse = true;
	        sysLookup->syscalls[sn+72]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+72]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+72]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+72]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+72]->edxUse = true;
	        sysLookup->syscalls[sn+72]->edxIsPtr = false;
	        sysLookup->syscalls[sn+72]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+72]->edxPtrDataSize = 1024;

	    // sys_sigpending (73) -------------------------------------------
	    sysLookup->syscalls[sn+73] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+73], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+73]->name = "sys_sigpending";
	    sysLookup->syscalls[sn+73]->source = "kernel/signal.c";
	    sysLookup->syscalls[sn+73]->number = 73;
	    sysLookup->syscalls[sn+73]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+73]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+73]->ebxUse = true;
	        sysLookup->syscalls[sn+73]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+73]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+73]->ebxPtrDataSize = 1024;

	    // sys_sethostname (74) -------------------------------------------
	    sysLookup->syscalls[sn+74] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+74], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+74]->name = "sys_sethostname";
	    sysLookup->syscalls[sn+74]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+74]->number = 74;
	    sysLookup->syscalls[sn+74]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+74]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+74]->ebxUse = true;
	        sysLookup->syscalls[sn+74]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+74]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+74]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+74]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+74]->ecxUse = true;
	        sysLookup->syscalls[sn+74]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+74]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+74]->ecxPtrDataSize = 1024;

	    // sys_setrlimit (75) -------------------------------------------
	    sysLookup->syscalls[sn+75] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+75], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+75]->name = "sys_setrlimit";
	    sysLookup->syscalls[sn+75]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+75]->number = 75;
	    sysLookup->syscalls[sn+75]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+75]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+75]->ebxUse = true;
	        sysLookup->syscalls[sn+75]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+75]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+75]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+75]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+75]->ecxUse = true;
	        sysLookup->syscalls[sn+75]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+75]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+75]->ecxPtrDataSize = 1024;

	    // sys_getrlimit (76) -------------------------------------------
	    sysLookup->syscalls[sn+76] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+76], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+76]->name = "sys_getrlimit";
	    sysLookup->syscalls[sn+76]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+76]->number = 76;
	    sysLookup->syscalls[sn+76]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+76]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+76]->ebxUse = true;
	        sysLookup->syscalls[sn+76]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+76]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+76]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+76]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+76]->ecxUse = true;
	        sysLookup->syscalls[sn+76]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+76]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+76]->ecxPtrDataSize = 1024;

	    // sys_getrusage (77) -------------------------------------------
	    sysLookup->syscalls[sn+77] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+77], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+77]->name = "sys_getrusage";
	    sysLookup->syscalls[sn+77]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+77]->number = 77;
	    sysLookup->syscalls[sn+77]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+77]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+77]->ebxUse = true;
	        sysLookup->syscalls[sn+77]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+77]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+77]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+77]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+77]->ecxUse = true;
	        sysLookup->syscalls[sn+77]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+77]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+77]->ecxPtrDataSize = 1024;

	    // sys_gettimeofday (78) -------------------------------------------
	    sysLookup->syscalls[sn+78] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+78], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+78]->name = "sys_gettimeofday";
	    sysLookup->syscalls[sn+78]->source = "kernel/time.c";
	    sysLookup->syscalls[sn+78]->number = 78;
	    sysLookup->syscalls[sn+78]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+78]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+78]->ebxUse = true;
	        sysLookup->syscalls[sn+78]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+78]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+78]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+78]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+78]->ecxUse = true;
	        sysLookup->syscalls[sn+78]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+78]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+78]->ecxPtrDataSize = 1024;

	    // sys_settimeofday (79) -------------------------------------------
	    sysLookup->syscalls[sn+79] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+79], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+79]->name = "sys_settimeofday";
	    sysLookup->syscalls[sn+79]->source = "kernel/time.c";
	    sysLookup->syscalls[sn+79]->number = 79;
	    sysLookup->syscalls[sn+79]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+79]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+79]->ebxUse = true;
	        sysLookup->syscalls[sn+79]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+79]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+79]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+79]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+79]->ecxUse = true;
	        sysLookup->syscalls[sn+79]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+79]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+79]->ecxPtrDataSize = 1024;

	    // sys_getgroups (80) -------------------------------------------
	    sysLookup->syscalls[sn+80] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+80], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+80]->name = "sys_getgroups";
	    sysLookup->syscalls[sn+80]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+80]->number = 80;
	    sysLookup->syscalls[sn+80]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+80]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+80]->ebxUse = true;
	        sysLookup->syscalls[sn+80]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+80]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+80]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+80]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+80]->ecxUse = true;
	        sysLookup->syscalls[sn+80]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+80]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+80]->ecxPtrDataSize = 1024;

	    // sys_setgroups (81) -------------------------------------------
	    sysLookup->syscalls[sn+81] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+81], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+81]->name = "sys_setgroups";
	    sysLookup->syscalls[sn+81]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+81]->number = 81;
	    sysLookup->syscalls[sn+81]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+81]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+81]->ebxUse = true;
	        sysLookup->syscalls[sn+81]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+81]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+81]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+81]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+81]->ecxUse = true;
	        sysLookup->syscalls[sn+81]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+81]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+81]->ecxPtrDataSize = 1024;

	    // old_select (82) -------------------------------------------
	    sysLookup->syscalls[sn+82] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+82], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+82]->name = "old_select";
	    sysLookup->syscalls[sn+82]->source = "arch/i386/kernel/sys_i386.c";
	    sysLookup->syscalls[sn+82]->number = 82;
	    sysLookup->syscalls[sn+82]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+82]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+82]->ebxUse = true;
	        sysLookup->syscalls[sn+82]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+82]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+82]->ebxPtrDataSize = 1024;

	    // sys_symlink (83) -------------------------------------------
	    sysLookup->syscalls[sn+83] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+83], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+83]->name = "sys_symlink";
	    sysLookup->syscalls[sn+83]->source = "fs/namei.c";
	    sysLookup->syscalls[sn+83]->number = 83;
	    sysLookup->syscalls[sn+83]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+83]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+83]->ebxUse = true;
	        sysLookup->syscalls[sn+83]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+83]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+83]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+83]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+83]->ecxUse = true;
	        sysLookup->syscalls[sn+83]->ecxIsPtr = true;
	        sysLookup->syscalls[sn+83]->ecxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+83]->ecxPtrDataSize = 0;

	    // sys_lstat (84) -------------------------------------------
	    sysLookup->syscalls[sn+84] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+84], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+84]->name = "sys_lstat";
	    sysLookup->syscalls[sn+84]->source = "fs/stat.c";
	    sysLookup->syscalls[sn+84]->number = 84;
	    sysLookup->syscalls[sn+84]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+84]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+84]->ebxUse = true;
	        sysLookup->syscalls[sn+84]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+84]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+84]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+84]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+84]->ecxUse = true;
	        sysLookup->syscalls[sn+84]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+84]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+84]->ecxPtrDataSize = 1024;

	    // sys_readlink (85) -------------------------------------------
	    sysLookup->syscalls[sn+85] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+85], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+85]->name = "sys_readlink";
	    sysLookup->syscalls[sn+85]->source = "fs/stat.c";
	    sysLookup->syscalls[sn+85]->number = 85;
	    sysLookup->syscalls[sn+85]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+85]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+85]->ebxUse = true;
	        sysLookup->syscalls[sn+85]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+85]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+85]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+85]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+85]->ecxUse = true;
	        sysLookup->syscalls[sn+85]->ecxIsPtr = true;
	        sysLookup->syscalls[sn+85]->ecxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+85]->ecxPtrDataSize = 0;
	        // EDX
	        sysLookup->syscalls[sn+85]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+85]->edxUse = true;
	        sysLookup->syscalls[sn+85]->edxIsPtr = false;
	        sysLookup->syscalls[sn+85]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+85]->edxPtrDataSize = 1024;

	    // sys_uselib (86) -------------------------------------------
	    sysLookup->syscalls[sn+86] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+86], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+86]->name = "sys_uselib";
	    sysLookup->syscalls[sn+86]->source = "fs/exec.c";
	    sysLookup->syscalls[sn+86]->number = 86;
	    sysLookup->syscalls[sn+86]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+86]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+86]->ebxUse = true;
	        sysLookup->syscalls[sn+86]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+86]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+86]->ebxPtrDataSize = 0;

	    // sys_swapon (87) -------------------------------------------
	    sysLookup->syscalls[sn+87] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+87], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+87]->name = "sys_swapon";
	    sysLookup->syscalls[sn+87]->source = "mm/swapfile.c";
	    sysLookup->syscalls[sn+87]->number = 87;
	    sysLookup->syscalls[sn+87]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+87]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+87]->ebxUse = true;
	        sysLookup->syscalls[sn+87]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+87]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+87]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+87]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+87]->ecxUse = true;
	        sysLookup->syscalls[sn+87]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+87]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+87]->ecxPtrDataSize = 1024;

	    // sys_reboot (88) -------------------------------------------
	    sysLookup->syscalls[sn+88] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+88], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+88]->name = "sys_reboot";
	    sysLookup->syscalls[sn+88]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+88]->number = 88;
	    sysLookup->syscalls[sn+88]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+88]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+88]->ebxUse = true;
	        sysLookup->syscalls[sn+88]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+88]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+88]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+88]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+88]->ecxUse = true;
	        sysLookup->syscalls[sn+88]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+88]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+88]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+88]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+88]->edxUse = true;
	        sysLookup->syscalls[sn+88]->edxIsPtr = false;
	        sysLookup->syscalls[sn+88]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+88]->edxPtrDataSize = 1024;
	        // ESI
	        sysLookup->syscalls[sn+88]->esiDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+88]->esiUse = true;
	        sysLookup->syscalls[sn+88]->esiIsPtr = false;
	        sysLookup->syscalls[sn+88]->esiPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+88]->esiPtrDataSize = 1024;

	    // old_readdir (89) -------------------------------------------
	    sysLookup->syscalls[sn+89] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+89], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+89]->name = "old_readdir";
	    sysLookup->syscalls[sn+89]->source = "fs/readdir.c";
	    sysLookup->syscalls[sn+89]->number = 89;
	    sysLookup->syscalls[sn+89]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+89]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+89]->ebxUse = true;
	        sysLookup->syscalls[sn+89]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+89]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+89]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+89]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+89]->ecxUse = true;
	        sysLookup->syscalls[sn+89]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+89]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+89]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+89]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+89]->edxUse = true;
	        sysLookup->syscalls[sn+89]->edxIsPtr = false;
	        sysLookup->syscalls[sn+89]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+89]->edxPtrDataSize = 1024;

	    // old_mmap (90) -------------------------------------------
	    sysLookup->syscalls[sn+90] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+90], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+90]->name = "old_mmap";
	    sysLookup->syscalls[sn+90]->source = "arch/i386/kernel/sys_i386.c";
	    sysLookup->syscalls[sn+90]->number = 90;
	    sysLookup->syscalls[sn+90]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+90]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+90]->ebxUse = true;
	        sysLookup->syscalls[sn+90]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+90]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+90]->ebxPtrDataSize = 1024;

	    // sys_munmap (91) -------------------------------------------
	    sysLookup->syscalls[sn+91] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+91], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+91]->name = "sys_munmap";
	    sysLookup->syscalls[sn+91]->source = "mm/mmap.c";
	    sysLookup->syscalls[sn+91]->number = 91;
	    sysLookup->syscalls[sn+91]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+91]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+91]->ebxUse = true;
	        sysLookup->syscalls[sn+91]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+91]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+91]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+91]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+91]->ecxUse = true;
	        sysLookup->syscalls[sn+91]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+91]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+91]->ecxPtrDataSize = 1024;

	    // sys_truncate (92) -------------------------------------------
	    sysLookup->syscalls[sn+92] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+92], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+92]->name = "sys_truncate";
	    sysLookup->syscalls[sn+92]->source = "fs/open.c";
	    sysLookup->syscalls[sn+92]->number = 92;
	    sysLookup->syscalls[sn+92]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+92]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+92]->ebxUse = true;
	        sysLookup->syscalls[sn+92]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+92]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+92]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+92]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+92]->ecxUse = true;
	        sysLookup->syscalls[sn+92]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+92]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+92]->ecxPtrDataSize = 1024;

	    // sys_ftruncate (93) -------------------------------------------
	    sysLookup->syscalls[sn+93] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+93], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+93]->name = "sys_ftruncate";
	    sysLookup->syscalls[sn+93]->source = "fs/open.c";
	    sysLookup->syscalls[sn+93]->number = 93;
	    sysLookup->syscalls[sn+93]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+93]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+93]->ebxUse = true;
	        sysLookup->syscalls[sn+93]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+93]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+93]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+93]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+93]->ecxUse = true;
	        sysLookup->syscalls[sn+93]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+93]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+93]->ecxPtrDataSize = 1024;

	    // sys_fchmod (94) -------------------------------------------
	    sysLookup->syscalls[sn+94] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+94], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+94]->name = "sys_fchmod";
	    sysLookup->syscalls[sn+94]->source = "fs/open.c";
	    sysLookup->syscalls[sn+94]->number = 94;
	    sysLookup->syscalls[sn+94]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+94]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+94]->ebxUse = true;
	        sysLookup->syscalls[sn+94]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+94]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+94]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+94]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+94]->ecxUse = true;
	        sysLookup->syscalls[sn+94]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+94]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+94]->ecxPtrDataSize = 1024;

	    // sys_fchown (95) -------------------------------------------
	    sysLookup->syscalls[sn+95] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+95], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+95]->name = "sys_fchown";
	    sysLookup->syscalls[sn+95]->source = "fs/open.c";
	    sysLookup->syscalls[sn+95]->number = 95;
	    sysLookup->syscalls[sn+95]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+95]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+95]->ebxUse = true;
	        sysLookup->syscalls[sn+95]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+95]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+95]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+95]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+95]->ecxUse = true;
	        sysLookup->syscalls[sn+95]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+95]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+95]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+95]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+95]->edxUse = true;
	        sysLookup->syscalls[sn+95]->edxIsPtr = false;
	        sysLookup->syscalls[sn+95]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+95]->edxPtrDataSize = 1024;

	    // sys_getpriority (96) -------------------------------------------
	    sysLookup->syscalls[sn+96] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+96], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+96]->name = "sys_getpriority";
	    sysLookup->syscalls[sn+96]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+96]->number = 96;
	    sysLookup->syscalls[sn+96]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+96]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+96]->ebxUse = true;
	        sysLookup->syscalls[sn+96]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+96]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+96]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+96]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+96]->ecxUse = true;
	        sysLookup->syscalls[sn+96]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+96]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+96]->ecxPtrDataSize = 1024;

	    // sys_setpriority (97) -------------------------------------------
	    sysLookup->syscalls[sn+97] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+97], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+97]->name = "sys_setpriority";
	    sysLookup->syscalls[sn+97]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+97]->number = 97;
	    sysLookup->syscalls[sn+97]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+97]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+97]->ebxUse = true;
	        sysLookup->syscalls[sn+97]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+97]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+97]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+97]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+97]->ecxUse = true;
	        sysLookup->syscalls[sn+97]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+97]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+97]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+97]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+97]->edxUse = true;
	        sysLookup->syscalls[sn+97]->edxIsPtr = false;
	        sysLookup->syscalls[sn+97]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+97]->edxPtrDataSize = 1024;

	    // sys_statfs (99) -------------------------------------------
	    sysLookup->syscalls[sn+99] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+99], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+99]->name = "sys_statfs";
	    sysLookup->syscalls[sn+99]->source = "fs/open.c";
	    sysLookup->syscalls[sn+99]->number = 99;
	    sysLookup->syscalls[sn+99]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+99]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+99]->ebxUse = true;
	        sysLookup->syscalls[sn+99]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+99]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+99]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+99]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+99]->ecxUse = true;
	        sysLookup->syscalls[sn+99]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+99]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+99]->ecxPtrDataSize = 1024;

	    // sys_fstatfs (100) -------------------------------------------
	    sysLookup->syscalls[sn+100] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+100], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+100]->name = "sys_fstatfs";
	    sysLookup->syscalls[sn+100]->source = "fs/open.c";
	    sysLookup->syscalls[sn+100]->number = 100;
	    sysLookup->syscalls[sn+100]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+100]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+100]->ebxUse = true;
	        sysLookup->syscalls[sn+100]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+100]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+100]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+100]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+100]->ecxUse = true;
	        sysLookup->syscalls[sn+100]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+100]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+100]->ecxPtrDataSize = 1024;

	    // sys_ioperm (101) -------------------------------------------
	    sysLookup->syscalls[sn+101] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+101], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+101]->name = "sys_ioperm";
	    sysLookup->syscalls[sn+101]->source = "arch/i386/kernel/ioport.c";
	    sysLookup->syscalls[sn+101]->number = 101;
	    sysLookup->syscalls[sn+101]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+101]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+101]->ebxUse = true;
	        sysLookup->syscalls[sn+101]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+101]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+101]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+101]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+101]->ecxUse = true;
	        sysLookup->syscalls[sn+101]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+101]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+101]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+101]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+101]->edxUse = true;
	        sysLookup->syscalls[sn+101]->edxIsPtr = false;
	        sysLookup->syscalls[sn+101]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+101]->edxPtrDataSize = 1024;

	    // sys_socketcall (102) -------------------------------------------
	    sysLookup->syscalls[sn+102] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+102], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+102]->name = "sys_socketcall";
	    sysLookup->syscalls[sn+102]->source = "net/socket.c";
	    sysLookup->syscalls[sn+102]->number = 102;
	    sysLookup->syscalls[sn+102]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+102]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+102]->ebxUse = true;
	        sysLookup->syscalls[sn+102]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+102]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+102]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+102]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+102]->ecxUse = true;
	        sysLookup->syscalls[sn+102]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+102]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+102]->ecxPtrDataSize = 1024;

	    // sys_syslog (103) -------------------------------------------
	    sysLookup->syscalls[sn+103] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+103], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+103]->name = "sys_syslog";
	    sysLookup->syscalls[sn+103]->source = "kernel/printk.c";
	    sysLookup->syscalls[sn+103]->number = 103;
	    sysLookup->syscalls[sn+103]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+103]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+103]->ebxUse = true;
	        sysLookup->syscalls[sn+103]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+103]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+103]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+103]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+103]->ecxUse = true;
	        sysLookup->syscalls[sn+103]->ecxIsPtr = true;
	        sysLookup->syscalls[sn+103]->ecxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+103]->ecxPtrDataSize = 0;
	        // EDX
	        sysLookup->syscalls[sn+103]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+103]->edxUse = true;
	        sysLookup->syscalls[sn+103]->edxIsPtr = false;
	        sysLookup->syscalls[sn+103]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+103]->edxPtrDataSize = 1024;

	    // sys_setitimer (104) -------------------------------------------
	    sysLookup->syscalls[sn+104] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+104], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+104]->name = "sys_setitimer";
	    sysLookup->syscalls[sn+104]->source = "kernel/itimer.c";
	    sysLookup->syscalls[sn+104]->number = 104;
	    sysLookup->syscalls[sn+104]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+104]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+104]->ebxUse = true;
	        sysLookup->syscalls[sn+104]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+104]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+104]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+104]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+104]->ecxUse = true;
	        sysLookup->syscalls[sn+104]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+104]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+104]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+104]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+104]->edxUse = true;
	        sysLookup->syscalls[sn+104]->edxIsPtr = false;
	        sysLookup->syscalls[sn+104]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+104]->edxPtrDataSize = 1024;

	    // sys_getitimer (105) -------------------------------------------
	    sysLookup->syscalls[sn+105] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+105], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+105]->name = "sys_getitimer";
	    sysLookup->syscalls[sn+105]->source = "kernel/itimer.c";
	    sysLookup->syscalls[sn+105]->number = 105;
	    sysLookup->syscalls[sn+105]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+105]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+105]->ebxUse = true;
	        sysLookup->syscalls[sn+105]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+105]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+105]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+105]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+105]->ecxUse = true;
	        sysLookup->syscalls[sn+105]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+105]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+105]->ecxPtrDataSize = 1024;

	    // sys_newstat (106) -------------------------------------------
	    sysLookup->syscalls[sn+106] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+106], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+106]->name = "sys_newstat";
	    sysLookup->syscalls[sn+106]->source = "fs/stat.c";
	    sysLookup->syscalls[sn+106]->number = 106;
	    sysLookup->syscalls[sn+106]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+106]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+106]->ebxUse = true;
	        sysLookup->syscalls[sn+106]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+106]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+106]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+106]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+106]->ecxUse = true;
	        sysLookup->syscalls[sn+106]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+106]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+106]->ecxPtrDataSize = 1024;

	    // sys_newlstat (107) -------------------------------------------
	    sysLookup->syscalls[sn+107] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+107], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+107]->name = "sys_newlstat";
	    sysLookup->syscalls[sn+107]->source = "fs/stat.c";
	    sysLookup->syscalls[sn+107]->number = 107;
	    sysLookup->syscalls[sn+107]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+107]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+107]->ebxUse = true;
	        sysLookup->syscalls[sn+107]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+107]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+107]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+107]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+107]->ecxUse = true;
	        sysLookup->syscalls[sn+107]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+107]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+107]->ecxPtrDataSize = 1024;

	    // sys_newfstat (108) -------------------------------------------
	    sysLookup->syscalls[sn+108] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+108], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+108]->name = "sys_newfstat";
	    sysLookup->syscalls[sn+108]->source = "fs/stat.c";
	    sysLookup->syscalls[sn+108]->number = 108;
	    sysLookup->syscalls[sn+108]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+108]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+108]->ebxUse = true;
	        sysLookup->syscalls[sn+108]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+108]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+108]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+108]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+108]->ecxUse = true;
	        sysLookup->syscalls[sn+108]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+108]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+108]->ecxPtrDataSize = 1024;

	    // sys_uname (109) -------------------------------------------
	    sysLookup->syscalls[sn+109] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+109], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+109]->name = "sys_uname";
	    sysLookup->syscalls[sn+109]->source = "arch/i386/kernel/sys_i386.c";
	    sysLookup->syscalls[sn+109]->number = 109;
	    sysLookup->syscalls[sn+109]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+109]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+109]->ebxUse = true;
	        sysLookup->syscalls[sn+109]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+109]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+109]->ebxPtrDataSize = 1024;

	    // sys_iopl (110) -------------------------------------------
	    sysLookup->syscalls[sn+110] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+110], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+110]->name = "sys_iopl";
	    sysLookup->syscalls[sn+110]->source = "arch/i386/kernel/ioport.c";
	    sysLookup->syscalls[sn+110]->number = 110;
	    sysLookup->syscalls[sn+110]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+110]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+110]->ebxUse = true;
	        sysLookup->syscalls[sn+110]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+110]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+110]->ebxPtrDataSize = 1024;

	    // sys_vhangup (111) -------------------------------------------
	    sysLookup->syscalls[sn+111] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+111], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+111]->name = "sys_vhangup";
	    sysLookup->syscalls[sn+111]->source = "fs/open.c";
	    sysLookup->syscalls[sn+111]->number = 111;
	    sysLookup->syscalls[sn+111]->returnSize = 4;

	    // sys_idle (112) -------------------------------------------
	    sysLookup->syscalls[sn+112] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+112], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+112]->name = "sys_idle";
	    sysLookup->syscalls[sn+112]->source = "arch/i386/kernel/process.c";
	    sysLookup->syscalls[sn+112]->number = 112;
	    sysLookup->syscalls[sn+112]->returnSize = 4;

	    // sys_vm86old (113) -------------------------------------------
	    sysLookup->syscalls[sn+113] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+113], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+113]->name = "sys_vm86old";
	    sysLookup->syscalls[sn+113]->source = "arch/i386/kernel/vm86.c";
	    sysLookup->syscalls[sn+113]->number = 113;
	    sysLookup->syscalls[sn+113]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+113]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+113]->ebxUse = true;
	        sysLookup->syscalls[sn+113]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+113]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+113]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+113]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+113]->ecxUse = true;
	        sysLookup->syscalls[sn+113]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+113]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+113]->ecxPtrDataSize = 1024;

	    // sys_wait4 (114) -------------------------------------------
	    sysLookup->syscalls[sn+114] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+114], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+114]->name = "sys_wait4";
	    sysLookup->syscalls[sn+114]->source = "kernel/exit.c";
	    sysLookup->syscalls[sn+114]->number = 114;
	    sysLookup->syscalls[sn+114]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+114]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+114]->ebxUse = true;
	        sysLookup->syscalls[sn+114]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+114]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+114]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+114]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+114]->ecxUse = true;
	        sysLookup->syscalls[sn+114]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+114]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+114]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+114]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+114]->edxUse = true;
	        sysLookup->syscalls[sn+114]->edxIsPtr = false;
	        sysLookup->syscalls[sn+114]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+114]->edxPtrDataSize = 1024;
	        // ESI
	        sysLookup->syscalls[sn+114]->esiDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+114]->esiUse = true;
	        sysLookup->syscalls[sn+114]->esiIsPtr = false;
	        sysLookup->syscalls[sn+114]->esiPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+114]->esiPtrDataSize = 1024;
	        // EDI
	        sysLookup->syscalls[sn+114]->ediDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+114]->ediUse = true;
	        sysLookup->syscalls[sn+114]->ediIsPtr = false;
	        sysLookup->syscalls[sn+114]->ediPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+114]->ediPtrDataSize = 1024;

	    // sys_swapoff (115) -------------------------------------------
	    sysLookup->syscalls[sn+115] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+115], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+115]->name = "sys_swapoff";
	    sysLookup->syscalls[sn+115]->source = "mm/swapfile.c";
	    sysLookup->syscalls[sn+115]->number = 115;
	    sysLookup->syscalls[sn+115]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+115]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+115]->ebxUse = true;
	        sysLookup->syscalls[sn+115]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+115]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+115]->ebxPtrDataSize = 0;

	    // sys_sysinfo (116) -------------------------------------------
	    sysLookup->syscalls[sn+116] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+116], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+116]->name = "sys_sysinfo";
	    sysLookup->syscalls[sn+116]->source = "kernel/info.c";
	    sysLookup->syscalls[sn+116]->number = 116;
	    sysLookup->syscalls[sn+116]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+116]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+116]->ebxUse = true;
	        sysLookup->syscalls[sn+116]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+116]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+116]->ebxPtrDataSize = 1024;

	    // sys_ipc (117) -------------------------------------------
	    sysLookup->syscalls[sn+117] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+117], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+117]->name = "sys_ipc";
	    sysLookup->syscalls[sn+117]->source = "(Note)";
	    sysLookup->syscalls[sn+117]->number = 117;
	    sysLookup->syscalls[sn+117]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+117]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+117]->ebxUse = true;
	        sysLookup->syscalls[sn+117]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+117]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+117]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+117]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+117]->ecxUse = true;
	        sysLookup->syscalls[sn+117]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+117]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+117]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+117]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+117]->edxUse = true;
	        sysLookup->syscalls[sn+117]->edxIsPtr = false;
	        sysLookup->syscalls[sn+117]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+117]->edxPtrDataSize = 1024;
	        // ESI
	        sysLookup->syscalls[sn+117]->esiDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+117]->esiUse = true;
	        sysLookup->syscalls[sn+117]->esiIsPtr = false;
	        sysLookup->syscalls[sn+117]->esiPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+117]->esiPtrDataSize = 1024;
	        // EDI
	        sysLookup->syscalls[sn+117]->ediDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+117]->ediUse = true;
	        sysLookup->syscalls[sn+117]->ediIsPtr = false;
	        sysLookup->syscalls[sn+117]->ediPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+117]->ediPtrDataSize = 1024;

	    // sys_fsync (118) -------------------------------------------
	    sysLookup->syscalls[sn+118] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+118], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+118]->name = "sys_fsync";
	    sysLookup->syscalls[sn+118]->source = "fs/buffer.c";
	    sysLookup->syscalls[sn+118]->number = 118;
	    sysLookup->syscalls[sn+118]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+118]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+118]->ebxUse = true;
	        sysLookup->syscalls[sn+118]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+118]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+118]->ebxPtrDataSize = 1024;

	    // sys_sigreturn (119) -------------------------------------------
	    sysLookup->syscalls[sn+119] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+119], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+119]->name = "sys_sigreturn";
	    sysLookup->syscalls[sn+119]->source = "arch/i386/kernel/signal.c";
	    sysLookup->syscalls[sn+119]->number = 119;
	    sysLookup->syscalls[sn+119]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+119]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+119]->ebxUse = true;
	        sysLookup->syscalls[sn+119]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+119]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+119]->ebxPtrDataSize = 1024;

	    // sys_clone (120) -------------------------------------------
	    sysLookup->syscalls[sn+120] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+120], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+120]->name = "sys_clone";
	    sysLookup->syscalls[sn+120]->source = "arch/i386/kernel/process.c";
	    sysLookup->syscalls[sn+120]->number = 120;
	    sysLookup->syscalls[sn+120]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+120]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+120]->ebxUse = true;
	        sysLookup->syscalls[sn+120]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+120]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+120]->ebxPtrDataSize = 1024;

	    // sys_setdomainname (121) -------------------------------------------
	    sysLookup->syscalls[sn+121] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+121], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+121]->name = "sys_setdomainname";
	    sysLookup->syscalls[sn+121]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+121]->number = 121;
	    sysLookup->syscalls[sn+121]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+121]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+121]->ebxUse = true;
	        sysLookup->syscalls[sn+121]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+121]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+121]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+121]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+121]->ecxUse = true;
	        sysLookup->syscalls[sn+121]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+121]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+121]->ecxPtrDataSize = 1024;

	    // sys_newuname (122) -------------------------------------------
	    sysLookup->syscalls[sn+122] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+122], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+122]->name = "sys_newuname";
	    sysLookup->syscalls[sn+122]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+122]->number = 122;
	    sysLookup->syscalls[sn+122]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+122]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+122]->ebxUse = true;
	        sysLookup->syscalls[sn+122]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+122]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+122]->ebxPtrDataSize = 1024;

	    // sys_modify_ldt (123) -------------------------------------------
	    sysLookup->syscalls[sn+123] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+123], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+123]->name = "sys_modify_ldt";
	    sysLookup->syscalls[sn+123]->source = "arch/i386/kernel/ldt.c";
	    sysLookup->syscalls[sn+123]->number = 123;
	    sysLookup->syscalls[sn+123]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+123]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+123]->ebxUse = true;
	        sysLookup->syscalls[sn+123]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+123]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+123]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+123]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+123]->ecxUse = true;
	        sysLookup->syscalls[sn+123]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+123]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+123]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+123]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+123]->edxUse = true;
	        sysLookup->syscalls[sn+123]->edxIsPtr = false;
	        sysLookup->syscalls[sn+123]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+123]->edxPtrDataSize = 1024;

	    // sys_adjtimex (124) -------------------------------------------
	    sysLookup->syscalls[sn+124] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+124], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+124]->name = "sys_adjtimex";
	    sysLookup->syscalls[sn+124]->source = "kernel/time.c";
	    sysLookup->syscalls[sn+124]->number = 124;
	    sysLookup->syscalls[sn+124]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+124]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+124]->ebxUse = true;
	        sysLookup->syscalls[sn+124]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+124]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+124]->ebxPtrDataSize = 1024;

	    // sys_mprotect (125) -------------------------------------------
	    sysLookup->syscalls[sn+125] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+125], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+125]->name = "sys_mprotect";
	    sysLookup->syscalls[sn+125]->source = "mm/mprotect.c";
	    sysLookup->syscalls[sn+125]->number = 125;
	    sysLookup->syscalls[sn+125]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+125]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+125]->ebxUse = true;
	        sysLookup->syscalls[sn+125]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+125]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+125]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+125]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+125]->ecxUse = true;
	        sysLookup->syscalls[sn+125]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+125]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+125]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+125]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+125]->edxUse = true;
	        sysLookup->syscalls[sn+125]->edxIsPtr = false;
	        sysLookup->syscalls[sn+125]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+125]->edxPtrDataSize = 1024;

	    // sys_sigprocmask (126) -------------------------------------------
	    sysLookup->syscalls[sn+126] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+126], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+126]->name = "sys_sigprocmask";
	    sysLookup->syscalls[sn+126]->source = "kernel/signal.c";
	    sysLookup->syscalls[sn+126]->number = 126;
	    sysLookup->syscalls[sn+126]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+126]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+126]->ebxUse = true;
	        sysLookup->syscalls[sn+126]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+126]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+126]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+126]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+126]->ecxUse = true;
	        sysLookup->syscalls[sn+126]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+126]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+126]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+126]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+126]->edxUse = true;
	        sysLookup->syscalls[sn+126]->edxIsPtr = false;
	        sysLookup->syscalls[sn+126]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+126]->edxPtrDataSize = 1024;

	    // sys_create_module (127) -------------------------------------------
	    sysLookup->syscalls[sn+127] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+127], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+127]->name = "sys_create_module";
	    sysLookup->syscalls[sn+127]->source = "kernel/module.c";
	    sysLookup->syscalls[sn+127]->number = 127;
	    sysLookup->syscalls[sn+127]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+127]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+127]->ebxUse = true;
	        sysLookup->syscalls[sn+127]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+127]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+127]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+127]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+127]->ecxUse = true;
	        sysLookup->syscalls[sn+127]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+127]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+127]->ecxPtrDataSize = 1024;

	    // sys_init_module (128) -------------------------------------------
	    sysLookup->syscalls[sn+128] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+128], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+128]->name = "sys_init_module";
	    sysLookup->syscalls[sn+128]->source = "kernel/module.c";
	    sysLookup->syscalls[sn+128]->number = 128;
	    sysLookup->syscalls[sn+128]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+128]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+128]->ebxUse = true;
	        sysLookup->syscalls[sn+128]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+128]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+128]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+128]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+128]->ecxUse = true;
	        sysLookup->syscalls[sn+128]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+128]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+128]->ecxPtrDataSize = 1024;

	    // sys_delete_module (129) -------------------------------------------
	    sysLookup->syscalls[sn+129] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+129], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+129]->name = "sys_delete_module";
	    sysLookup->syscalls[sn+129]->source = "kernel/module.c";
	    sysLookup->syscalls[sn+129]->number = 129;
	    sysLookup->syscalls[sn+129]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+129]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+129]->ebxUse = true;
	        sysLookup->syscalls[sn+129]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+129]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+129]->ebxPtrDataSize = 0;

	    // sys_get_kernel_syms (130) -------------------------------------------
	    sysLookup->syscalls[sn+130] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+130], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+130]->name = "sys_get_kernel_syms";
	    sysLookup->syscalls[sn+130]->source = "kernel/module.c";
	    sysLookup->syscalls[sn+130]->number = 130;
	    sysLookup->syscalls[sn+130]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+130]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+130]->ebxUse = true;
	        sysLookup->syscalls[sn+130]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+130]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+130]->ebxPtrDataSize = 1024;

	    // sys_quotactl (131) -------------------------------------------
	    sysLookup->syscalls[sn+131] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+131], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+131]->name = "sys_quotactl";
	    sysLookup->syscalls[sn+131]->source = "fs/dquot.c";
	    sysLookup->syscalls[sn+131]->number = 131;
	    sysLookup->syscalls[sn+131]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+131]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+131]->ebxUse = true;
	        sysLookup->syscalls[sn+131]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+131]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+131]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+131]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+131]->ecxUse = true;
	        sysLookup->syscalls[sn+131]->ecxIsPtr = true;
	        sysLookup->syscalls[sn+131]->ecxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+131]->ecxPtrDataSize = 0;
	        // EDX
	        sysLookup->syscalls[sn+131]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+131]->edxUse = true;
	        sysLookup->syscalls[sn+131]->edxIsPtr = false;
	        sysLookup->syscalls[sn+131]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+131]->edxPtrDataSize = 1024;
	        // ESI
	        sysLookup->syscalls[sn+131]->esiDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+131]->esiUse = true;
	        sysLookup->syscalls[sn+131]->esiIsPtr = false;
	        sysLookup->syscalls[sn+131]->esiPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+131]->esiPtrDataSize = 1024;

	    // sys_getpgid (132) -------------------------------------------
	    sysLookup->syscalls[sn+132] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+132], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+132]->name = "sys_getpgid";
	    sysLookup->syscalls[sn+132]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+132]->number = 132;
	    sysLookup->syscalls[sn+132]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+132]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+132]->ebxUse = true;
	        sysLookup->syscalls[sn+132]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+132]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+132]->ebxPtrDataSize = 1024;

	    // sys_fchdir (133) -------------------------------------------
	    sysLookup->syscalls[sn+133] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+133], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+133]->name = "sys_fchdir";
	    sysLookup->syscalls[sn+133]->source = "fs/open.c";
	    sysLookup->syscalls[sn+133]->number = 133;
	    sysLookup->syscalls[sn+133]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+133]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+133]->ebxUse = true;
	        sysLookup->syscalls[sn+133]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+133]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+133]->ebxPtrDataSize = 1024;

	    // sys_bdflush (134) -------------------------------------------
	    sysLookup->syscalls[sn+134] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+134], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+134]->name = "sys_bdflush";
	    sysLookup->syscalls[sn+134]->source = "fs/buffer.c";
	    sysLookup->syscalls[sn+134]->number = 134;
	    sysLookup->syscalls[sn+134]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+134]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+134]->ebxUse = true;
	        sysLookup->syscalls[sn+134]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+134]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+134]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+134]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+134]->ecxUse = true;
	        sysLookup->syscalls[sn+134]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+134]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+134]->ecxPtrDataSize = 1024;

	    // sys_sysfs (135) -------------------------------------------
	    sysLookup->syscalls[sn+135] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+135], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+135]->name = "sys_sysfs";
	    sysLookup->syscalls[sn+135]->source = "fs/super.c";
	    sysLookup->syscalls[sn+135]->number = 135;
	    sysLookup->syscalls[sn+135]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+135]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+135]->ebxUse = true;
	        sysLookup->syscalls[sn+135]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+135]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+135]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+135]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+135]->ecxUse = true;
	        sysLookup->syscalls[sn+135]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+135]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+135]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+135]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+135]->edxUse = true;
	        sysLookup->syscalls[sn+135]->edxIsPtr = false;
	        sysLookup->syscalls[sn+135]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+135]->edxPtrDataSize = 1024;

	    // sys_personality (136) -------------------------------------------
	    sysLookup->syscalls[sn+136] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+136], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+136]->name = "sys_personality";
	    sysLookup->syscalls[sn+136]->source = "kernel/exec_domain.c";
	    sysLookup->syscalls[sn+136]->number = 136;
	    sysLookup->syscalls[sn+136]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+136]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+136]->ebxUse = true;
	        sysLookup->syscalls[sn+136]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+136]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+136]->ebxPtrDataSize = 1024;

	    // sys_setfsuid (138) -------------------------------------------
	    sysLookup->syscalls[sn+138] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+138], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+138]->name = "sys_setfsuid";
	    sysLookup->syscalls[sn+138]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+138]->number = 138;
	    sysLookup->syscalls[sn+138]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+138]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+138]->ebxUse = true;
	        sysLookup->syscalls[sn+138]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+138]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+138]->ebxPtrDataSize = 1024;

	    // sys_setfsgid (139) -------------------------------------------
	    sysLookup->syscalls[sn+139] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+139], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+139]->name = "sys_setfsgid";
	    sysLookup->syscalls[sn+139]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+139]->number = 139;
	    sysLookup->syscalls[sn+139]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+139]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+139]->ebxUse = true;
	        sysLookup->syscalls[sn+139]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+139]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+139]->ebxPtrDataSize = 1024;

	    // sys_llseek (140) -------------------------------------------
	    sysLookup->syscalls[sn+140] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+140], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+140]->name = "sys_llseek";
	    sysLookup->syscalls[sn+140]->source = "fs/read_write.c";
	    sysLookup->syscalls[sn+140]->number = 140;
	    sysLookup->syscalls[sn+140]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+140]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+140]->ebxUse = true;
	        sysLookup->syscalls[sn+140]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+140]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+140]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+140]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+140]->ecxUse = true;
	        sysLookup->syscalls[sn+140]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+140]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+140]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+140]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+140]->edxUse = true;
	        sysLookup->syscalls[sn+140]->edxIsPtr = false;
	        sysLookup->syscalls[sn+140]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+140]->edxPtrDataSize = 1024;
	        // ESI
	        sysLookup->syscalls[sn+140]->esiDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+140]->esiUse = true;
	        sysLookup->syscalls[sn+140]->esiIsPtr = false;
	        sysLookup->syscalls[sn+140]->esiPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+140]->esiPtrDataSize = 1024;
	        // EDI
	        sysLookup->syscalls[sn+140]->ediDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+140]->ediUse = true;
	        sysLookup->syscalls[sn+140]->ediIsPtr = false;
	        sysLookup->syscalls[sn+140]->ediPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+140]->ediPtrDataSize = 1024;

	    // sys_getdents (141) -------------------------------------------
	    sysLookup->syscalls[sn+141] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+141], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+141]->name = "sys_getdents";
	    sysLookup->syscalls[sn+141]->source = "fs/readdir.c";
	    sysLookup->syscalls[sn+141]->number = 141;
	    sysLookup->syscalls[sn+141]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+141]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+141]->ebxUse = true;
	        sysLookup->syscalls[sn+141]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+141]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+141]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+141]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+141]->ecxUse = true;
	        sysLookup->syscalls[sn+141]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+141]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+141]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+141]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+141]->edxUse = true;
	        sysLookup->syscalls[sn+141]->edxIsPtr = false;
	        sysLookup->syscalls[sn+141]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+141]->edxPtrDataSize = 1024;

	    // sys_select (142) -------------------------------------------
	    sysLookup->syscalls[sn+142] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+142], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+142]->name = "sys_select";
	    sysLookup->syscalls[sn+142]->source = "fs/select.c";
	    sysLookup->syscalls[sn+142]->number = 142;
	    sysLookup->syscalls[sn+142]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+142]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+142]->ebxUse = true;
	        sysLookup->syscalls[sn+142]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+142]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+142]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+142]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+142]->ecxUse = true;
	        sysLookup->syscalls[sn+142]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+142]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+142]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+142]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+142]->edxUse = true;
	        sysLookup->syscalls[sn+142]->edxIsPtr = false;
	        sysLookup->syscalls[sn+142]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+142]->edxPtrDataSize = 1024;
	        // ESI
	        sysLookup->syscalls[sn+142]->esiDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+142]->esiUse = true;
	        sysLookup->syscalls[sn+142]->esiIsPtr = false;
	        sysLookup->syscalls[sn+142]->esiPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+142]->esiPtrDataSize = 1024;
	        // EDI
	        sysLookup->syscalls[sn+142]->ediDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+142]->ediUse = true;
	        sysLookup->syscalls[sn+142]->ediIsPtr = false;
	        sysLookup->syscalls[sn+142]->ediPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+142]->ediPtrDataSize = 1024;

	    // sys_flock (143) -------------------------------------------
	    sysLookup->syscalls[sn+143] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+143], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+143]->name = "sys_flock";
	    sysLookup->syscalls[sn+143]->source = "fs/locks.c";
	    sysLookup->syscalls[sn+143]->number = 143;
	    sysLookup->syscalls[sn+143]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+143]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+143]->ebxUse = true;
	        sysLookup->syscalls[sn+143]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+143]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+143]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+143]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+143]->ecxUse = true;
	        sysLookup->syscalls[sn+143]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+143]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+143]->ecxPtrDataSize = 1024;

	    // sys_msync (144) -------------------------------------------
	    sysLookup->syscalls[sn+144] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+144], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+144]->name = "sys_msync";
	    sysLookup->syscalls[sn+144]->source = "mm/filemap.c";
	    sysLookup->syscalls[sn+144]->number = 144;
	    sysLookup->syscalls[sn+144]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+144]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+144]->ebxUse = true;
	        sysLookup->syscalls[sn+144]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+144]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+144]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+144]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+144]->ecxUse = true;
	        sysLookup->syscalls[sn+144]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+144]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+144]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+144]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+144]->edxUse = true;
	        sysLookup->syscalls[sn+144]->edxIsPtr = false;
	        sysLookup->syscalls[sn+144]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+144]->edxPtrDataSize = 1024;

	    // sys_readv (145) -------------------------------------------
	    sysLookup->syscalls[sn+145] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+145], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+145]->name = "sys_readv";
	    sysLookup->syscalls[sn+145]->source = "fs/read_write.c";
	    sysLookup->syscalls[sn+145]->number = 145;
	    sysLookup->syscalls[sn+145]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+145]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+145]->ebxUse = true;
	        sysLookup->syscalls[sn+145]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+145]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+145]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+145]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+145]->ecxUse = true;
	        sysLookup->syscalls[sn+145]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+145]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+145]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+145]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+145]->edxUse = true;
	        sysLookup->syscalls[sn+145]->edxIsPtr = false;
	        sysLookup->syscalls[sn+145]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+145]->edxPtrDataSize = 1024;

	    // sys_writev (146) -------------------------------------------
	    sysLookup->syscalls[sn+146] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+146], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+146]->name = "sys_writev";
	    sysLookup->syscalls[sn+146]->source = "fs/read_write.c";
	    sysLookup->syscalls[sn+146]->number = 146;
	    sysLookup->syscalls[sn+146]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+146]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+146]->ebxUse = true;
	        sysLookup->syscalls[sn+146]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+146]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+146]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+146]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+146]->ecxUse = true;
	        sysLookup->syscalls[sn+146]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+146]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+146]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+146]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+146]->edxUse = true;
	        sysLookup->syscalls[sn+146]->edxIsPtr = false;
	        sysLookup->syscalls[sn+146]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+146]->edxPtrDataSize = 1024;

	    // sys_getsid (147) -------------------------------------------
	    sysLookup->syscalls[sn+147] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+147], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+147]->name = "sys_getsid";
	    sysLookup->syscalls[sn+147]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+147]->number = 147;
	    sysLookup->syscalls[sn+147]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+147]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+147]->ebxUse = true;
	        sysLookup->syscalls[sn+147]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+147]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+147]->ebxPtrDataSize = 1024;

	    // sys_fdatasync (148) -------------------------------------------
	    sysLookup->syscalls[sn+148] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+148], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+148]->name = "sys_fdatasync";
	    sysLookup->syscalls[sn+148]->source = "fs/buffer.c";
	    sysLookup->syscalls[sn+148]->number = 148;
	    sysLookup->syscalls[sn+148]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+148]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+148]->ebxUse = true;
	        sysLookup->syscalls[sn+148]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+148]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+148]->ebxPtrDataSize = 1024;

	    // sys_sysctl (149) -------------------------------------------
	    sysLookup->syscalls[sn+149] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+149], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+149]->name = "sys_sysctl";
	    sysLookup->syscalls[sn+149]->source = "kernel/sysctl.c";
	    sysLookup->syscalls[sn+149]->number = 149;
	    sysLookup->syscalls[sn+149]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+149]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+149]->ebxUse = true;
	        sysLookup->syscalls[sn+149]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+149]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+149]->ebxPtrDataSize = 1024;

	    // sys_mlock (150) -------------------------------------------
	    sysLookup->syscalls[sn+150] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+150], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+150]->name = "sys_mlock";
	    sysLookup->syscalls[sn+150]->source = "mm/mlock.c";
	    sysLookup->syscalls[sn+150]->number = 150;
	    sysLookup->syscalls[sn+150]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+150]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+150]->ebxUse = true;
	        sysLookup->syscalls[sn+150]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+150]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+150]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+150]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+150]->ecxUse = true;
	        sysLookup->syscalls[sn+150]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+150]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+150]->ecxPtrDataSize = 1024;

	    // sys_munlock (151) -------------------------------------------
	    sysLookup->syscalls[sn+151] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+151], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+151]->name = "sys_munlock";
	    sysLookup->syscalls[sn+151]->source = "mm/mlock.c";
	    sysLookup->syscalls[sn+151]->number = 151;
	    sysLookup->syscalls[sn+151]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+151]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+151]->ebxUse = true;
	        sysLookup->syscalls[sn+151]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+151]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+151]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+151]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+151]->ecxUse = true;
	        sysLookup->syscalls[sn+151]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+151]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+151]->ecxPtrDataSize = 1024;

	    // sys_mlockall (152) -------------------------------------------
	    sysLookup->syscalls[sn+152] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+152], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+152]->name = "sys_mlockall";
	    sysLookup->syscalls[sn+152]->source = "mm/mlock.c";
	    sysLookup->syscalls[sn+152]->number = 152;
	    sysLookup->syscalls[sn+152]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+152]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+152]->ebxUse = true;
	        sysLookup->syscalls[sn+152]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+152]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+152]->ebxPtrDataSize = 1024;

	    // sys_munlockall (153) -------------------------------------------
	    sysLookup->syscalls[sn+153] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+153], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+153]->name = "sys_munlockall";
	    sysLookup->syscalls[sn+153]->source = "mm/mlock.c";
	    sysLookup->syscalls[sn+153]->number = 153;
	    sysLookup->syscalls[sn+153]->returnSize = 4;

	    // sys_sched_setparam (154) -------------------------------------------
	    sysLookup->syscalls[sn+154] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+154], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+154]->name = "sys_sched_setparam";
	    sysLookup->syscalls[sn+154]->source = "kernel/sched.c";
	    sysLookup->syscalls[sn+154]->number = 154;
	    sysLookup->syscalls[sn+154]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+154]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+154]->ebxUse = true;
	        sysLookup->syscalls[sn+154]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+154]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+154]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+154]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+154]->ecxUse = true;
	        sysLookup->syscalls[sn+154]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+154]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+154]->ecxPtrDataSize = 1024;

	    // sys_sched_getparam (155) -------------------------------------------
	    sysLookup->syscalls[sn+155] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+155], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+155]->name = "sys_sched_getparam";
	    sysLookup->syscalls[sn+155]->source = "kernel/sched.c";
	    sysLookup->syscalls[sn+155]->number = 155;
	    sysLookup->syscalls[sn+155]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+155]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+155]->ebxUse = true;
	        sysLookup->syscalls[sn+155]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+155]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+155]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+155]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+155]->ecxUse = true;
	        sysLookup->syscalls[sn+155]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+155]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+155]->ecxPtrDataSize = 1024;

	    // sys_sched_setscheduler (156) -------------------------------------------
	    sysLookup->syscalls[sn+156] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+156], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+156]->name = "sys_sched_setscheduler";
	    sysLookup->syscalls[sn+156]->source = "kernel/sched.c";
	    sysLookup->syscalls[sn+156]->number = 156;
	    sysLookup->syscalls[sn+156]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+156]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+156]->ebxUse = true;
	        sysLookup->syscalls[sn+156]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+156]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+156]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+156]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+156]->ecxUse = true;
	        sysLookup->syscalls[sn+156]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+156]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+156]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+156]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+156]->edxUse = true;
	        sysLookup->syscalls[sn+156]->edxIsPtr = false;
	        sysLookup->syscalls[sn+156]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+156]->edxPtrDataSize = 1024;

	    // sys_sched_getscheduler (157) -------------------------------------------
	    sysLookup->syscalls[sn+157] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+157], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+157]->name = "sys_sched_getscheduler";
	    sysLookup->syscalls[sn+157]->source = "kernel/sched.c";
	    sysLookup->syscalls[sn+157]->number = 157;
	    sysLookup->syscalls[sn+157]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+157]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+157]->ebxUse = true;
	        sysLookup->syscalls[sn+157]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+157]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+157]->ebxPtrDataSize = 1024;

	    // sys_sched_yield (158) -------------------------------------------
	    sysLookup->syscalls[sn+158] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+158], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+158]->name = "sys_sched_yield";
	    sysLookup->syscalls[sn+158]->source = "kernel/sched.c";
	    sysLookup->syscalls[sn+158]->number = 158;
	    sysLookup->syscalls[sn+158]->returnSize = 4;

	    // sys_sched_get_priority_max (159) -------------------------------------------
	    sysLookup->syscalls[sn+159] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+159], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+159]->name = "sys_sched_get_priority_max";
	    sysLookup->syscalls[sn+159]->source = "kernel/sched.c";
	    sysLookup->syscalls[sn+159]->number = 159;
	    sysLookup->syscalls[sn+159]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+159]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+159]->ebxUse = true;
	        sysLookup->syscalls[sn+159]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+159]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+159]->ebxPtrDataSize = 1024;

	    // sys_sched_get_priority_min (160) -------------------------------------------
	    sysLookup->syscalls[sn+160] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+160], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+160]->name = "sys_sched_get_priority_min";
	    sysLookup->syscalls[sn+160]->source = "kernel/sched.c";
	    sysLookup->syscalls[sn+160]->number = 160;
	    sysLookup->syscalls[sn+160]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+160]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+160]->ebxUse = true;
	        sysLookup->syscalls[sn+160]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+160]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+160]->ebxPtrDataSize = 1024;

	    // sys_sched_rr_get_interval (161) -------------------------------------------
	    sysLookup->syscalls[sn+161] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+161], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+161]->name = "sys_sched_rr_get_interval";
	    sysLookup->syscalls[sn+161]->source = "kernel/sched.c";
	    sysLookup->syscalls[sn+161]->number = 161;
	    sysLookup->syscalls[sn+161]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+161]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+161]->ebxUse = true;
	        sysLookup->syscalls[sn+161]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+161]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+161]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+161]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+161]->ecxUse = true;
	        sysLookup->syscalls[sn+161]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+161]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+161]->ecxPtrDataSize = 1024;

	    // sys_nanosleep (162) -------------------------------------------
	    sysLookup->syscalls[sn+162] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+162], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+162]->name = "sys_nanosleep";
	    sysLookup->syscalls[sn+162]->source = "kernel/sched.c";
	    sysLookup->syscalls[sn+162]->number = 162;
	    sysLookup->syscalls[sn+162]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+162]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+162]->ebxUse = true;
	        sysLookup->syscalls[sn+162]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+162]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+162]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+162]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+162]->ecxUse = true;
	        sysLookup->syscalls[sn+162]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+162]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+162]->ecxPtrDataSize = 1024;

	    // sys_mremap (163) -------------------------------------------
	    sysLookup->syscalls[sn+163] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+163], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+163]->name = "sys_mremap";
	    sysLookup->syscalls[sn+163]->source = "mm/mremap.c";
	    sysLookup->syscalls[sn+163]->number = 163;
	    sysLookup->syscalls[sn+163]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+163]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+163]->ebxUse = true;
	        sysLookup->syscalls[sn+163]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+163]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+163]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+163]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+163]->ecxUse = true;
	        sysLookup->syscalls[sn+163]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+163]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+163]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+163]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+163]->edxUse = true;
	        sysLookup->syscalls[sn+163]->edxIsPtr = false;
	        sysLookup->syscalls[sn+163]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+163]->edxPtrDataSize = 1024;
	        // ESI
	        sysLookup->syscalls[sn+163]->esiDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+163]->esiUse = true;
	        sysLookup->syscalls[sn+163]->esiIsPtr = false;
	        sysLookup->syscalls[sn+163]->esiPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+163]->esiPtrDataSize = 1024;

	    // sys_setresuid (164) -------------------------------------------
	    sysLookup->syscalls[sn+164] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+164], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+164]->name = "sys_setresuid";
	    sysLookup->syscalls[sn+164]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+164]->number = 164;
	    sysLookup->syscalls[sn+164]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+164]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+164]->ebxUse = true;
	        sysLookup->syscalls[sn+164]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+164]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+164]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+164]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+164]->ecxUse = true;
	        sysLookup->syscalls[sn+164]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+164]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+164]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+164]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+164]->edxUse = true;
	        sysLookup->syscalls[sn+164]->edxIsPtr = false;
	        sysLookup->syscalls[sn+164]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+164]->edxPtrDataSize = 1024;

	    // sys_getresuid (165) -------------------------------------------
	    sysLookup->syscalls[sn+165] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+165], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+165]->name = "sys_getresuid";
	    sysLookup->syscalls[sn+165]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+165]->number = 165;
	    sysLookup->syscalls[sn+165]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+165]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+165]->ebxUse = true;
	        sysLookup->syscalls[sn+165]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+165]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+165]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+165]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+165]->ecxUse = true;
	        sysLookup->syscalls[sn+165]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+165]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+165]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+165]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+165]->edxUse = true;
	        sysLookup->syscalls[sn+165]->edxIsPtr = false;
	        sysLookup->syscalls[sn+165]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+165]->edxPtrDataSize = 1024;

	    // sys_vm86 (166) -------------------------------------------
	    sysLookup->syscalls[sn+166] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+166], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+166]->name = "sys_vm86";
	    sysLookup->syscalls[sn+166]->source = "arch/i386/kernel/vm86.c";
	    sysLookup->syscalls[sn+166]->number = 166;
	    sysLookup->syscalls[sn+166]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+166]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+166]->ebxUse = true;
	        sysLookup->syscalls[sn+166]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+166]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+166]->ebxPtrDataSize = 1024;

	    // sys_query_module (167) -------------------------------------------
	    sysLookup->syscalls[sn+167] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+167], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+167]->name = "sys_query_module";
	    sysLookup->syscalls[sn+167]->source = "kernel/module.c";
	    sysLookup->syscalls[sn+167]->number = 167;
	    sysLookup->syscalls[sn+167]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+167]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+167]->ebxUse = true;
	        sysLookup->syscalls[sn+167]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+167]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+167]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+167]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+167]->ecxUse = true;
	        sysLookup->syscalls[sn+167]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+167]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+167]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+167]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+167]->edxUse = true;
	        sysLookup->syscalls[sn+167]->edxIsPtr = true;
	        sysLookup->syscalls[sn+167]->edxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+167]->edxPtrDataSize = 0;
	        // ESI
	        sysLookup->syscalls[sn+167]->esiDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+167]->esiUse = true;
	        sysLookup->syscalls[sn+167]->esiIsPtr = false;
	        sysLookup->syscalls[sn+167]->esiPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+167]->esiPtrDataSize = 1024;
	        // EDI
	        sysLookup->syscalls[sn+167]->ediDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+167]->ediUse = true;
	        sysLookup->syscalls[sn+167]->ediIsPtr = false;
	        sysLookup->syscalls[sn+167]->ediPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+167]->ediPtrDataSize = 1024;

	    // sys_poll (168) -------------------------------------------
	    sysLookup->syscalls[sn+168] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+168], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+168]->name = "sys_poll";
	    sysLookup->syscalls[sn+168]->source = "fs/select.c";
	    sysLookup->syscalls[sn+168]->number = 168;
	    sysLookup->syscalls[sn+168]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+168]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+168]->ebxUse = true;
	        sysLookup->syscalls[sn+168]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+168]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+168]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+168]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+168]->ecxUse = true;
	        sysLookup->syscalls[sn+168]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+168]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+168]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+168]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+168]->edxUse = true;
	        sysLookup->syscalls[sn+168]->edxIsPtr = false;
	        sysLookup->syscalls[sn+168]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+168]->edxPtrDataSize = 1024;

	    // sys_nfsservctl (169) -------------------------------------------
	    sysLookup->syscalls[sn+169] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+169], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+169]->name = "sys_nfsservctl";
	    sysLookup->syscalls[sn+169]->source = "fs/filesystems.c";
	    sysLookup->syscalls[sn+169]->number = 169;
	    sysLookup->syscalls[sn+169]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+169]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+169]->ebxUse = true;
	        sysLookup->syscalls[sn+169]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+169]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+169]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+169]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+169]->ecxUse = true;
	        sysLookup->syscalls[sn+169]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+169]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+169]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+169]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+169]->edxUse = true;
	        sysLookup->syscalls[sn+169]->edxIsPtr = false;
	        sysLookup->syscalls[sn+169]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+169]->edxPtrDataSize = 1024;

	    // sys_setresgid (170) -------------------------------------------
	    sysLookup->syscalls[sn+170] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+170], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+170]->name = "sys_setresgid";
	    sysLookup->syscalls[sn+170]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+170]->number = 170;
	    sysLookup->syscalls[sn+170]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+170]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+170]->ebxUse = true;
	        sysLookup->syscalls[sn+170]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+170]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+170]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+170]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+170]->ecxUse = true;
	        sysLookup->syscalls[sn+170]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+170]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+170]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+170]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+170]->edxUse = true;
	        sysLookup->syscalls[sn+170]->edxIsPtr = false;
	        sysLookup->syscalls[sn+170]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+170]->edxPtrDataSize = 1024;

	    // sys_getresgid (171) -------------------------------------------
	    sysLookup->syscalls[sn+171] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+171], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+171]->name = "sys_getresgid";
	    sysLookup->syscalls[sn+171]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+171]->number = 171;
	    sysLookup->syscalls[sn+171]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+171]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+171]->ebxUse = true;
	        sysLookup->syscalls[sn+171]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+171]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+171]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+171]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+171]->ecxUse = true;
	        sysLookup->syscalls[sn+171]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+171]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+171]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+171]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+171]->edxUse = true;
	        sysLookup->syscalls[sn+171]->edxIsPtr = false;
	        sysLookup->syscalls[sn+171]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+171]->edxPtrDataSize = 1024;

	    // sys_prctl (172) -------------------------------------------
	    sysLookup->syscalls[sn+172] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+172], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+172]->name = "sys_prctl";
	    sysLookup->syscalls[sn+172]->source = "kernel/sys.c";
	    sysLookup->syscalls[sn+172]->number = 172;
	    sysLookup->syscalls[sn+172]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+172]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+172]->ebxUse = true;
	        sysLookup->syscalls[sn+172]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+172]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+172]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+172]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+172]->ecxUse = true;
	        sysLookup->syscalls[sn+172]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+172]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+172]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+172]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+172]->edxUse = true;
	        sysLookup->syscalls[sn+172]->edxIsPtr = false;
	        sysLookup->syscalls[sn+172]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+172]->edxPtrDataSize = 1024;
	        // ESI
	        sysLookup->syscalls[sn+172]->esiDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+172]->esiUse = true;
	        sysLookup->syscalls[sn+172]->esiIsPtr = false;
	        sysLookup->syscalls[sn+172]->esiPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+172]->esiPtrDataSize = 1024;
	        // EDI
	        sysLookup->syscalls[sn+172]->ediDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+172]->ediUse = true;
	        sysLookup->syscalls[sn+172]->ediIsPtr = false;
	        sysLookup->syscalls[sn+172]->ediPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+172]->ediPtrDataSize = 1024;

	    // sys_rt_sigreturn (173) -------------------------------------------
	    sysLookup->syscalls[sn+173] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+173], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+173]->name = "sys_rt_sigreturn";
	    sysLookup->syscalls[sn+173]->source = "arch/i386/kernel/signal.c";
	    sysLookup->syscalls[sn+173]->number = 173;
	    sysLookup->syscalls[sn+173]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+173]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+173]->ebxUse = true;
	        sysLookup->syscalls[sn+173]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+173]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+173]->ebxPtrDataSize = 1024;

	    // sys_rt_sigaction (174) -------------------------------------------
	    sysLookup->syscalls[sn+174] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+174], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+174]->name = "sys_rt_sigaction";
	    sysLookup->syscalls[sn+174]->source = "kernel/signal.c";
	    sysLookup->syscalls[sn+174]->number = 174;
	    sysLookup->syscalls[sn+174]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+174]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+174]->ebxUse = true;
	        sysLookup->syscalls[sn+174]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+174]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+174]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+174]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+174]->ecxUse = true;
	        sysLookup->syscalls[sn+174]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+174]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+174]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+174]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+174]->edxUse = true;
	        sysLookup->syscalls[sn+174]->edxIsPtr = false;
	        sysLookup->syscalls[sn+174]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+174]->edxPtrDataSize = 1024;
	        // ESI
	        sysLookup->syscalls[sn+174]->esiDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+174]->esiUse = true;
	        sysLookup->syscalls[sn+174]->esiIsPtr = false;
	        sysLookup->syscalls[sn+174]->esiPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+174]->esiPtrDataSize = 1024;

	    // sys_rt_sigprocmask (175) -------------------------------------------
	    sysLookup->syscalls[sn+175] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+175], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+175]->name = "sys_rt_sigprocmask";
	    sysLookup->syscalls[sn+175]->source = "kernel/signal.c";
	    sysLookup->syscalls[sn+175]->number = 175;
	    sysLookup->syscalls[sn+175]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+175]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+175]->ebxUse = true;
	        sysLookup->syscalls[sn+175]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+175]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+175]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+175]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+175]->ecxUse = true;
	        sysLookup->syscalls[sn+175]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+175]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+175]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+175]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+175]->edxUse = true;
	        sysLookup->syscalls[sn+175]->edxIsPtr = false;
	        sysLookup->syscalls[sn+175]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+175]->edxPtrDataSize = 1024;
	        // ESI
	        sysLookup->syscalls[sn+175]->esiDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+175]->esiUse = true;
	        sysLookup->syscalls[sn+175]->esiIsPtr = false;
	        sysLookup->syscalls[sn+175]->esiPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+175]->esiPtrDataSize = 1024;

	    // sys_rt_sigpending (176) -------------------------------------------
	    sysLookup->syscalls[sn+176] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+176], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+176]->name = "sys_rt_sigpending";
	    sysLookup->syscalls[sn+176]->source = "kernel/signal.c";
	    sysLookup->syscalls[sn+176]->number = 176;
	    sysLookup->syscalls[sn+176]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+176]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+176]->ebxUse = true;
	        sysLookup->syscalls[sn+176]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+176]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+176]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+176]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+176]->ecxUse = true;
	        sysLookup->syscalls[sn+176]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+176]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+176]->ecxPtrDataSize = 1024;

	    // sys_rt_sigtimedwait (177) -------------------------------------------
	    sysLookup->syscalls[sn+177] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+177], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+177]->name = "sys_rt_sigtimedwait";
	    sysLookup->syscalls[sn+177]->source = "kernel/signal.c";
	    sysLookup->syscalls[sn+177]->number = 177;
	    sysLookup->syscalls[sn+177]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+177]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+177]->ebxUse = true;
	        sysLookup->syscalls[sn+177]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+177]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+177]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+177]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+177]->ecxUse = true;
	        sysLookup->syscalls[sn+177]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+177]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+177]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+177]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+177]->edxUse = true;
	        sysLookup->syscalls[sn+177]->edxIsPtr = false;
	        sysLookup->syscalls[sn+177]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+177]->edxPtrDataSize = 1024;
	        // ESI
	        sysLookup->syscalls[sn+177]->esiDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+177]->esiUse = true;
	        sysLookup->syscalls[sn+177]->esiIsPtr = false;
	        sysLookup->syscalls[sn+177]->esiPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+177]->esiPtrDataSize = 1024;

	    // sys_rt_sigqueueinfo (178) -------------------------------------------
	    sysLookup->syscalls[sn+178] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+178], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+178]->name = "sys_rt_sigqueueinfo";
	    sysLookup->syscalls[sn+178]->source = "kernel/signal.c";
	    sysLookup->syscalls[sn+178]->number = 178;
	    sysLookup->syscalls[sn+178]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+178]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+178]->ebxUse = true;
	        sysLookup->syscalls[sn+178]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+178]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+178]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+178]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+178]->ecxUse = true;
	        sysLookup->syscalls[sn+178]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+178]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+178]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+178]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+178]->edxUse = true;
	        sysLookup->syscalls[sn+178]->edxIsPtr = false;
	        sysLookup->syscalls[sn+178]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+178]->edxPtrDataSize = 1024;

	    // sys_rt_sigsuspend (179) -------------------------------------------
	    sysLookup->syscalls[sn+179] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+179], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+179]->name = "sys_rt_sigsuspend";
	    sysLookup->syscalls[sn+179]->source = "arch/i386/kernel/signal.c";
	    sysLookup->syscalls[sn+179]->number = 179;
	    sysLookup->syscalls[sn+179]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+179]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+179]->ebxUse = true;
	        sysLookup->syscalls[sn+179]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+179]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+179]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+179]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+179]->ecxUse = true;
	        sysLookup->syscalls[sn+179]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+179]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+179]->ecxPtrDataSize = 1024;

	    // sys_pread (180) -------------------------------------------
	    sysLookup->syscalls[sn+180] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+180], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+180]->name = "sys_pread";
	    sysLookup->syscalls[sn+180]->source = "fs/read_write.c";
	    sysLookup->syscalls[sn+180]->number = 180;
	    sysLookup->syscalls[sn+180]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+180]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+180]->ebxUse = true;
	        sysLookup->syscalls[sn+180]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+180]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+180]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+180]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+180]->ecxUse = true;
	        sysLookup->syscalls[sn+180]->ecxIsPtr = true;
	        sysLookup->syscalls[sn+180]->ecxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+180]->ecxPtrDataSize = 0;
	        // EDX
	        sysLookup->syscalls[sn+180]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+180]->edxUse = true;
	        sysLookup->syscalls[sn+180]->edxIsPtr = false;
	        sysLookup->syscalls[sn+180]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+180]->edxPtrDataSize = 1024;
	        // ESI
	        sysLookup->syscalls[sn+180]->esiDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+180]->esiUse = true;
	        sysLookup->syscalls[sn+180]->esiIsPtr = false;
	        sysLookup->syscalls[sn+180]->esiPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+180]->esiPtrDataSize = 1024;

	    // sys_pwrite (181) -------------------------------------------
	    sysLookup->syscalls[sn+181] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+181], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+181]->name = "sys_pwrite";
	    sysLookup->syscalls[sn+181]->source = "fs/read_write.c";
	    sysLookup->syscalls[sn+181]->number = 181;
	    sysLookup->syscalls[sn+181]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+181]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+181]->ebxUse = true;
	        sysLookup->syscalls[sn+181]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+181]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+181]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+181]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+181]->ecxUse = true;
	        sysLookup->syscalls[sn+181]->ecxIsPtr = true;
	        sysLookup->syscalls[sn+181]->ecxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+181]->ecxPtrDataSize = 0;
	        // EDX
	        sysLookup->syscalls[sn+181]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+181]->edxUse = true;
	        sysLookup->syscalls[sn+181]->edxIsPtr = false;
	        sysLookup->syscalls[sn+181]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+181]->edxPtrDataSize = 1024;
	        // ESI
	        sysLookup->syscalls[sn+181]->esiDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+181]->esiUse = true;
	        sysLookup->syscalls[sn+181]->esiIsPtr = false;
	        sysLookup->syscalls[sn+181]->esiPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+181]->esiPtrDataSize = 1024;

	    // sys_chown (182) -------------------------------------------
	    sysLookup->syscalls[sn+182] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+182], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+182]->name = "sys_chown";
	    sysLookup->syscalls[sn+182]->source = "fs/open.c";
	    sysLookup->syscalls[sn+182]->number = 182;
	    sysLookup->syscalls[sn+182]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+182]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+182]->ebxUse = true;
	        sysLookup->syscalls[sn+182]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+182]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+182]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+182]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+182]->ecxUse = true;
	        sysLookup->syscalls[sn+182]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+182]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+182]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+182]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+182]->edxUse = true;
	        sysLookup->syscalls[sn+182]->edxIsPtr = false;
	        sysLookup->syscalls[sn+182]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+182]->edxPtrDataSize = 1024;

	    // sys_getcwd (183) -------------------------------------------
	    sysLookup->syscalls[sn+183] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+183], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+183]->name = "sys_getcwd";
	    sysLookup->syscalls[sn+183]->source = "fs/dcache.c";
	    sysLookup->syscalls[sn+183]->number = 183;
	    sysLookup->syscalls[sn+183]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+183]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+183]->ebxUse = true;
	        sysLookup->syscalls[sn+183]->ebxIsPtr = true;
	        sysLookup->syscalls[sn+183]->ebxPtrDataType = PARAM_NULL_TERM_CHAR_PTR;
	        sysLookup->syscalls[sn+183]->ebxPtrDataSize = 0;
	        // ECX
	        sysLookup->syscalls[sn+183]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+183]->ecxUse = true;
	        sysLookup->syscalls[sn+183]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+183]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+183]->ecxPtrDataSize = 1024;

	    // sys_capget (184) -------------------------------------------
	    sysLookup->syscalls[sn+184] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+184], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+184]->name = "sys_capget";
	    sysLookup->syscalls[sn+184]->source = "kernel/capability.c";
	    sysLookup->syscalls[sn+184]->number = 184;
	    sysLookup->syscalls[sn+184]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+184]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+184]->ebxUse = true;
	        sysLookup->syscalls[sn+184]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+184]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+184]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+184]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+184]->ecxUse = true;
	        sysLookup->syscalls[sn+184]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+184]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+184]->ecxPtrDataSize = 1024;

	    // sys_capset (185) -------------------------------------------
	    sysLookup->syscalls[sn+185] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+185], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+185]->name = "sys_capset";
	    sysLookup->syscalls[sn+185]->source = "kernel/capability.c";
	    sysLookup->syscalls[sn+185]->number = 185;
	    sysLookup->syscalls[sn+185]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+185]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+185]->ebxUse = true;
	        sysLookup->syscalls[sn+185]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+185]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+185]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+185]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+185]->ecxUse = true;
	        sysLookup->syscalls[sn+185]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+185]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+185]->ecxPtrDataSize = 1024;

	    // sys_sigaltstack (186) -------------------------------------------
	    sysLookup->syscalls[sn+186] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+186], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+186]->name = "sys_sigaltstack";
	    sysLookup->syscalls[sn+186]->source = "arch/i386/kernel/signal.c";
	    sysLookup->syscalls[sn+186]->number = 186;
	    sysLookup->syscalls[sn+186]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+186]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+186]->ebxUse = true;
	        sysLookup->syscalls[sn+186]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+186]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+186]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+186]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+186]->ecxUse = true;
	        sysLookup->syscalls[sn+186]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+186]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+186]->ecxPtrDataSize = 1024;

	    // sys_sendfile (187) -------------------------------------------
	    sysLookup->syscalls[sn+187] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+187], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+187]->name = "sys_sendfile";
	    sysLookup->syscalls[sn+187]->source = "mm/filemap.c";
	    sysLookup->syscalls[sn+187]->number = 187;
	    sysLookup->syscalls[sn+187]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+187]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+187]->ebxUse = true;
	        sysLookup->syscalls[sn+187]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+187]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+187]->ebxPtrDataSize = 1024;
	        // ECX
	        sysLookup->syscalls[sn+187]->ecxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+187]->ecxUse = true;
	        sysLookup->syscalls[sn+187]->ecxIsPtr = false;
	        sysLookup->syscalls[sn+187]->ecxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+187]->ecxPtrDataSize = 1024;
	        // EDX
	        sysLookup->syscalls[sn+187]->edxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+187]->edxUse = true;
	        sysLookup->syscalls[sn+187]->edxIsPtr = false;
	        sysLookup->syscalls[sn+187]->edxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+187]->edxPtrDataSize = 1024;
	        // ESI
	        sysLookup->syscalls[sn+187]->esiDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+187]->esiUse = true;
	        sysLookup->syscalls[sn+187]->esiIsPtr = false;
	        sysLookup->syscalls[sn+187]->esiPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+187]->esiPtrDataSize = 1024;

	    // sys_vfork (190) -------------------------------------------
	    sysLookup->syscalls[sn+190] = (P_BP_LINUX_SYSCALL) bpmalloc(sizeof(BP_LINUX_SYSCALL));
	        memset(sysLookup->syscalls[sn+190], 0x00, sizeof(BP_LINUX_SYSCALL));
	    sysLookup->syscalls[sn+190]->name = "sys_vfork";
	    sysLookup->syscalls[sn+190]->source = "arch/i386/kernel/process.c";
	    sysLookup->syscalls[sn+190]->number = 190;
	    sysLookup->syscalls[sn+190]->returnSize = 4;
	        // EBX
	        sysLookup->syscalls[sn+190]->ebxDataSize = sizeof(size_t);
	        sysLookup->syscalls[sn+190]->ebxUse = true;
	        sysLookup->syscalls[sn+190]->ebxIsPtr = false;
	        sysLookup->syscalls[sn+190]->ebxPtrDataType = PARAM_BIN_DATA;
	        sysLookup->syscalls[sn+190]->ebxPtrDataSize = 1024;

	return sysLookup;
}

#endif // 32bit

#endif // USE_BLOCKPARTY_DEBUGGING == 1

#endif

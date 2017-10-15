/***************************************************************************
 *            BP-LinuxDebug.h
 *
 *  Thu Apr 19 18:02:43 2007
 *  Copyright  2007  User
 *  Email
 ****************************************************************************/

#ifndef BP_LINUX_DEBUG_H
#define BP_LINUX_DEBUG_H


#if USE_BLOCKPARTY_DEBUGGING == 1
#ifdef __GNUC__


#if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1


// ----------------------- FUNCTIONS TO UTILIZE DEBUGGING HANDLES -----------------------

// Parses a Character String (length) bytes long into a multidimensional
// character array.  Arg str is tokenized and returned. STRING IS NOT DUPLICATED.

char ** DebugParseArguments(char * argStr, int length);

// all creation functions return NULL on failure.

// Attaches to a specified pid, stops the process and returns
// a debug handle.  Process must be continued to debug
// process.
P_DEBUG_HANDLE DebugAttachToPid(int pid);

// Creates a process given a filesystem path, file arguments,
// and an environment.  args/environment may be null if
// specifying no passing values.  Process is stopped on creation
// and must be continued to for process to start.
P_DEBUG_HANDLE DebugCreateProcessAttach(char * path, char **args, char **environment);

// Returns an error if any condition is or a signal
// occurs.  Null if the signal is unimportant, or if the process
// exits.
int DebugContinue(P_DEBUG_HANDLE dbgHandle);

// Steps the debugger one step and returns
int DebugStep(P_DEBUG_HANDLE dbgHandle);

// Creates a stepping loop for `instructionCount` number of instructions
// evaluating the program state after each step.  This is very
// processor/resource intensive, but is very thorough. This
// function deals exclusively with the debug handles condition list,
// evaluating each condition per step.
//
// If a condition is raised, a pointer to it will be stored in the
// debug handles lastConditionRaised element (pointer, not a copy).
//
// Pass negative one into instructionCount to cause an infinate evaluation
// loop.
int DebugStepConditionLoop(P_DEBUG_HANDLE dbgHandle, int instructionCount);

// Detach from a debugged process
int DebugDetach(P_DEBUG_HANDLE dbgHandle);

// Gets current registers from the process and
// sets them in the dbgHandle
int DebugGetRegisters(P_DEBUG_HANDLE dbgHandle);

// Reads the specified number of bytes from the process, starting at
// processAddr, reading untill readLen.  Newly allocated buffer is returned
unsigned char * DebugReadMemory(P_DEBUG_HANDLE dbgHandle, void *processAddr, int readLen);

// Reads an allocated string at max readLen length from process memory at processAddr.
char * DebugReadString(P_DEBUG_HANDLE dbgHandle, void *processAddr, int readLen);



// Destroys a debug handle (needs to be updated)
int DestroyDebugHandle(P_DEBUG_HANDLE dbgHandle);


// Determines if a datatype is a pointer type
bool DebugSyscallDatatypeIsCounter(size_t type);

// Determines if a datatype is a pointer type
bool DebugSyscallDatatypeIsPointer(size_t type);

// destroys a syscall result
size_t DebugDestroySyscallResult(P_BP_LINUX_SYSCALL_RESULT result);

// ----------------- CONDITION FUNCTIONS --------------------------


// Sets the values for one condition block.
// Conditions are checked in sequence, one after the other
// in the main debugging loop.

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
	int cmpSet, void *cmpAddr, unsigned char *cmpBuff, int cmpLen);


// Appends a condition list to an already existing condition list.
// This will modify appendList changing its pointers to reflect those
// in the inList.  Be aware of this modification when writing conditions.
int AppendConditionList(P_DEBUG_CONDITION_LIST inList, P_DEBUG_CONDITION_LIST appendList);

// Destroys a condition list element associated with a dbgHandle
// given a conditionId.
int DestroyConditionElement(P_DEBUG_HANDLE dbgHandle, int conditionId);

// Destroys an entire condition list
int DestroyConditionList(P_DEBUG_CONDITION_LIST dbgList);

// Prints out one element
void PrintConditionElement(P_DEBUG_CONDITION_LIST dbgList);

// Prints out an entire condition list
int PrintConditionList(P_DEBUG_CONDITION_LIST dbgList);



// --------------- BREAKPOINT FUNCTIONS -----------------------------

// This creates a 0xCC style breakpoint and adds it
// to the running process.  The breakpoint structure/list
// is added in the dbgList.
int CreateAddBreakpoint(P_DEBUG_HANDLE dbgHandle, void * addr, int bpNumber);

// Destroys a breakpoint by number, relinking list appropriately, insructions
// are readded
int DestroyBreakpoint(P_DEBUG_HANDLE dbgHandle, int bpNumber);

// Destroys a breakpoint list, destroying all breakpoints, instructions
// are readded.
int DestroyBreakpointList(P_DEBUG_HANDLE dbgHandle);

// This function retreives a breakpoint list item by number.
// If the list is empty, or non-existant, null is returned
P_DEBUG_BREAKPOINT_LIST GetBreakpointByNumber(P_DEBUG_HANDLE dbgHandle, int bpNumber);

// This function prints out one breakpoint set in dbgHandle
// referenced by bpNumber
int PrintBreakpointElement(P_DEBUG_BREAKPOINT_LIST bpList);

// This function prints out all breakpoints set in a dbgHandle
int PrintBreakpointList(P_DEBUG_HANDLE dbgHandle);

// Returns true if the breakpoint specified by bpNumber
// has been set successfully.
int BreakpointIsSet(P_DEBUG_HANDLE dbgHandle, int bpNumber);


// ---------------- MEMORY / HEAP / STACK FUNCTIONS ----------------

// attempts to retrieve process stack from memory given a valid MEMORY_MAP
// stored in the dbgHandle.  Retrieved stack from process is stored in the
// dbgHandle it was called from. (Code It)
int DebugGetStackFromMapping(P_DEBUG_HANDLE dbgHandle);

// prints out the stacks in the process along with bases/heaps
int DebugPrintStack(P_DEBUG_HANDLE dbgHandle);

// prints out one stack element in a list
int DebugPrintStackElement(P_PROCESS_STACK procStack);

// attempts to retrieve heaps from memory given a valid MEMORY_MAP
// stored in the dbgHandle, retrieved heap list is stored in the
// dbgHandle.  If failure, dbgHandle->heaps will be NULL
int DebugGetHeapFromMapping(P_DEBUG_HANDLE dbgHandle);

// this is used to print out a heap element, without
// printing out its underlying heap list elements
int DebugPrintHeapMapping(P_DEBUG_HANDLE dbgHandle);

// Chunks are all stored in a respective heap list, stored
// inside the debug handle.

// attempts to create a linked list of the entire heap
// structure, starting at a heap base.  This represents
// the entire GNUC heap structure using the prev_size, size
// comparisons.
P_HEAP_CHUNK DebugGetHeapChunks(P_DEBUG_HANDLE dbgHandle, void *heapBase);

// prints a heap element
int DebugPrintHeapChunk(P_HEAP_CHUNK heapElement);

// prints a heap list
int DebugPrintHeapChunkList(P_HEAP_CHUNK heapElemement);

// prints out all the heaps in a list
int DebugPrintHeap(P_DEBUG_HANDLE dbgHandle);

/// prints out one heap element
int DebugPrintHeapElement(P_PROCESS_HEAP heapElement);

// Compares process memory, evaluating to either true (1) or false (0) if
// memory matches.  Be careful to avoid comparing invalid ranges.
int DebugCompareMemory(P_DEBUG_HANDLE dbgHandle, void *processAddr,\
						void *cmpDataBuff, int cmpLen);

// Searches the process stacks for a pattern of "len" bytes, returns
// valid memory pointer or null on unfound.  Requires valid memory
// pointer.  The start parameter, if not null, is used as an offset
// to start search from.
void * DebugSearchStack(P_DEBUG_HANDLE dbgHandle, char * pattern, int len, void * base);

// Searches the process heaps for a pattern of "len" bytes, returns
// valid memory pointer or null on unfound.  Requires valid memory
// pointer.  The base parameter, if not null, is used as an offset
// to start search from, allowing easier loop creation of entire
// stack searching.  Searches from bottom to top (heap searching)
void * DebugSearchHeap(P_DEBUG_HANDLE dbgHandle, char * pattern, int len, void * base);

// Searches through valid memory ranges for a pattern (can be memory intensive)
// .  Requires valid memory pointer.
void * DebugSearchValidMemory(P_DEBUG_HANDLE dbgHandle, char * pattern, int len, void * start);

// Prints out the sizes of each memory range in the dbgHandle (primarily
// for analysis and diagnostics).
void DebugPrintMemoryMapSizes(P_DEBUG_HANDLE dbgHandle);


// Gets the chunk belonging to a specific pointer occuring in the
// processes heap.
P_HEAP_CHUNK DebugHeapPointerGetChunk(P_DEBUG_HANDLE dbgHandle, void * heapPtr);

// Gets the heap associated with a certain heap pointer specified
P_PROCESS_HEAP DebugHeapPointerGetHeap(P_DEBUG_HANDLE dbgHandle, void * heapPtr);

// attempts to determine if a pointer is a valid
// heap pointer by testing it against valid heap
// ranges retrieved. ERR_SUCCESS for true
int DebugPointerIsHeap(P_DEBUG_HANDLE dbgHandle, void * testPtr);

// determines if a pointer is valid for the memory ranges existant
// in the process. ERR_SUCCESS for true
int DebugPointerIsValid(P_DEBUG_HANDLE dbgHandle, void * testPtr);

// gets the memory map associated with a pointer in a running process
P_MEMORY_MAP DebugPointerToMemMap(P_DEBUG_HANDLE handle, size_t checkPtr);

// gets the memory map associated with a pointer in a running process
P_MEMORY_MAP DebugPointerToMemMap(P_DEBUG_HANDLE handle, size_t checkPtr);


// --------------- SYSTEM CALL TRACING FUNCTIONS -----------

// returns a linux system call table containing all LINUX syscalls
P_BP_LINUX_SYSCALL_LOOKUP DebugGetLinuxSyscallLookupTable();

// gets a syscall from a lookup table based on number
P_BP_LINUX_SYSCALL DebugLookupSyscallByNumber(P_BP_LINUX_SYSCALL_LOOKUP table, size_t number);

// continues a stepping loop (return policys are below)
// SYS_RET_BEFORE,
// SYS_RET_AFTER,
// SYS_RET_CONTINUE
P_BP_LINUX_SYSCALL_RESULT DebugSyscallTrace(P_BP_LINUX_SYSCALL_LOOKUP lookupTable, P_BP_LINUX_SYSCALL_RESULT storeResult, P_DEBUG_HANDLE dbgHandle, size_t returnPolicy);


// --------------- DEBUG RELATED UTILITY FUNCTIONS -------------

// Linux specific function to return a signal number,
// given a filled siginfo_t structure.
char * L_ReturnStringFromSignal(P_DEBUG_HANDLE dbgHandle);

// Linux specific function to determine if a signal is
// fatal.
int L_SignalIsFatal(P_DEBUG_HANDLE dbgHandle);


#endif

#endif // #if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1

#endif // USE_BLOCKPARTY_DEBUGGING == 1

#endif // #ifndef BP_LINUX_DEBUG_H

#ifndef BP_LINUX_DEBUG_STRUCTURES_H
#define BP_LINUX_DEBUG_STRUCTURES_H

#if USE_BLOCKPARTY_DEBUGGING == 1

#if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1


#define MAX_TRIGGER_FILE_SIZE 256000

#include "blockparty/BP-Main.h"

#ifdef __GNUC__

/* 
	Key Structures:
	Do not uncomment, here for developer reference and ease of use.

	Debugger Conditions List:
 		Holds a list of conditions to be evaluated, very similar to a watchpoints in debugger theory.
		Full definition below.

 		typedef struct _DEBUG_CONDITION_LIST, DEBUG_CONDITION_LIST, *P_DEBUG_CONDITION_LIST;

	Debugger Breakpoint List: 
 		Holds a list of breakpoints set in the application.  Full definition below.

 		typedef struct _DEBUG_BREAKPOINT_LIST, DEBUG_BREAKPOINT_LIST, *P_DEBUG_BREAKPOINT_LIST;

	Memory Map List:
		Holds a list of every valid portion of memory, accurate to the last lookup.  Does not hold
		contents, rather it holds valid ranges for memory to be located/accessed/compared from.

 		typedef struct _MEMORY_MAP, MEMORY_MAP, *P_MEMORY_MAP;
	
	Process Heap:
		Holds a list of ranges of the entire processes heap space.  It performs dechunking for linux ia32 implementations
		of doug lea's malloc chunk structures.  The last element is typically the end of the heap, or the
		last corrupted chunk.  It walks the stack using (size).  It also stores information on whether
		or not house of the mind styled exploitation is possible.

 		typedef struct _PROCESS_HEAP, PROCESS_HEAP, P_PROCESS_HEAP heap;

	Process Stack:
		Holds a list of the entire processes stack ranges, updatable while program is stopped.
		
		typedef struct _PROCESS_STACK, PROCESS_STACK, P_PROCESS STACK stack;

*/
		
// ------------------- LINUX IA32 SYSCALL TABLE -----------------------------------

// platform enum
typedef enum {
	LINUX_IA32_SYSCALLS,
	LINUX_IA64_SYSCALLS,
	SOLARIS_IA32_SYSCALLS,
	SOLARIS_IA64_SYSCALLS
} BP_SYSCALL_PLATFORM;

// pointer type
typedef enum {
	PARAM_NULL_TERM_CHAR_PTR,
	PARAM_CHAR_PTR_EBX_SIZE, 
	PARAM_CHAR_PTR_ECX_SIZE, 
	PARAM_CHAR_PTR_EDX_SIZE, 
	PARAM_CHAR_PTR_ESI_SIZE, 
	PARAM_CHAR_PTR_EDI_SIZE, 
	PARAM_BIN_DATA,
	PARAM_REG_ONLY, 
	PARAM_FD,  // file descriptor
	PARAM_COUNTER, 
	PARAM_SIZE_EBX,
	PARAM_SIZE_ECX,
	PARAM_SIZE_EDX,
	PARAM_SIZE_ESI,
	PARAM_SIZE_EDI
} BP_SYSCALL_PARAM_PTR_TYPE;


// trace return policy
// BEFORE (before call executes), AFTER (after call finishes), CONTINUE (finish call from before)
typedef enum {
	SYS_RET_BEFORE,
	SYS_RET_AFTER,
	SYS_RET_CONTINUE
} BP_SYSTRACE_RETURN_POLICY;

// Retrieved SYSCALL result Struct (info about individual syscalls)
typedef struct _P_BP_LINUX_SYSCALL_RESULT {



	// syscall number, aka EAX
	int number;
	
	// size of return data
	size_t returnVal;
	
	// argument count
	size_t syscallArgCount;
	
	// Syscall EBX Hijack
	size_t ebx;
	size_t ebxIsPtr;
		unsigned char * ebxStoredData;
		size_t ebxPtrDataType;
		size_t ebxPtrDataSize;

	// Syscall ECX Hijack
	size_t ecx;
	size_t ecxIsPtr;
		unsigned char * ecxStoredData;
		size_t ecxPtrDataType;
		size_t ecxPtrDataSize;

	// Syscall EDX Hijack
	size_t edx;
	size_t edxIsPtr;
		unsigned char * edxStoredData;
		size_t edxPtrDataType;
		size_t edxPtrDataSize;

	// Syscall ESI Hijack
	size_t esi;
	size_t esiIsPtr;
		unsigned char * esiStoredData;
		size_t esiPtrDataType;
		size_t esiPtrDataSize;

	// Syscall EDI Hijack
	size_t edi;
	size_t ediIsPtr;
		unsigned char * ediStoredData;
		size_t ediPtrDataType;
		size_t ediPtrDataSize;
						
	
} BP_LINUX_SYSCALL_RESULT, *P_BP_LINUX_SYSCALL_RESULT;

// SYSCALL Struct (info about individual syscalls)
typedef struct _P_BP_LINUX_SYSCALL {

	// syscall number, aka EAX
	int number;
	
	// syscall name
	char *name;
	
	// name of source header file
	char *source;
	
	// size of return data
	size_t returnSize;
	
	// Syscall EBX Relation and Return Size
	bool ebxUse;
		size_t ebxDataSize;
		size_t ebxIsPtr;
			size_t ebxPtrDataType;
			size_t ebxPtrDataSize;
		
	// Syscall ECX Relation and Return Size
	bool ecxUse;
		size_t ecxDataSize;
		size_t ecxIsPtr;
			size_t ecxPtrDataType;
			size_t ecxPtrDataSize;
	// Syscall EDX Relation and Return Size		
	bool edxUse;
		size_t edxDataSize;
		size_t edxIsPtr;
			size_t edxPtrDataType;
			size_t edxPtrDataSize;
			
	// Syscall ESI Relation and Return Size
	bool esiUse;
		size_t esiDataSize;
		size_t esiIsPtr;
			size_t esiPtrDataType;
			size_t esiPtrDataSize;
			
	// Syscall EDI Relation and Return Size
	bool ediUse;
		size_t ediDataSize;
		size_t ediIsPtr;
			size_t ediPtrDataType;
			size_t ediPtrDataSize;
	
} BP_LINUX_SYSCALL, *P_BP_LINUX_SYSCALL;


// SYSCALL Lookup Table Struct
typedef struct _P_BP_SYSCALL_LOOLKUP_TABLE {
	
	// represents the platform the syscall lookup table is built for
	size_t platform;
	
	size_t syscallCount;
	
	// dynamically allocated syscall table set ARRAY
	P_BP_LINUX_SYSCALL * syscalls;
	
} BP_LINUX_SYSCALL_LOOKUP, *P_BP_LINUX_SYSCALL_LOOKUP;


// -------------------- LINUX IA32 MEMORY MAPPINGS -----------------------------

// This is the list which holds the memory mapped elements
// from the proc filesystem.  Sweetness.

typedef struct _MEMORY_MAP {
	void * highAddr;
	void * lowAddr;
	
	// memory flags, 0 off , greater than 0 on 
	int read;
	int write;
	int priv;
	int shared;
	int execute;
	
	// ELF indicator / header if found
	int elf;
	void * elfHeader;
	
	int pe;
	
	// section description
	char * path;
	
		struct _MEMORY_MAP * first;
		struct _MEMORY_MAP * prev;
		struct _MEMORY_MAP * next;
			
} MEMORY_MAP, *P_MEMORY_MAP;


// ----------------- LINUX IA32 DEBUG CONDITIONS (WATCHPOINTS) ------------------------

// this list structure is used to pass
// user specified conditions to a debugging loop.
// The conditions are all applied sequentially on every
// execution loop.  
typedef struct _DEBUG_CONDITION_LIST {
	// identifies the condition by a number
	int conditionId;
	
	// identifies whether or not the condition
	// has yet been reached.
	int conditionReached;
	
		
	int eaxSet;
	int eax; 
		int eaxMatch;
		int eaxGt;   // Greater than (if true, evauluate; same for all conditions)
			int eaxGtMatch;
		int eaxLt;  // Less than
			int eaxLtMatch;
	
	int ebxSet;
	int ebx;
		int ebxMatch;
		int ebxGt; 
			int ebxGtMatch;
		int ebxLt; 
			int ebxLtMatch;
	
	int ecxSet;
	int ecx;
		int ecxMatch;
		int ecxGt; 
			int ecxGtMatch;
		int ecxLt; 
			int ecxLtMatch;
			
	int edxSet;
	int edx;
		int edxMatch;
		int edxGt; 
			int edxGtMatch;
		int edxLt; 
			int edxLtMatch;
	
	int espSet;
	int esp;
		int espMatch;
		int espGt; 
			int espGtMatch;
		int espLt;
			int espLtMatch;
	
	int ebpSet;
	int ebp;
		int ebpMatch;
		int ebpGt;
			int ebpGtMatch;
		int ebpLt;
			int ebpLtMatch;
	
	int eipSet;
	int eip;
		int eipMatch;
		int eipGt;
			int eipGtMatch;
		int eipLt;
			int eipLtMatch;
	
	int esiSet;
	int esi;
		int esiMatch;
		int esiGt;
			int esiGtMatch;
		int esiLt;
			int esiLtMatch;
	
	int ediSet;
	int edi;
		int ediMatch;
		int ediGt;
			int ediGtMatch;
		int ediLt;
			int ediLtMatch;		
			
			
	// used to compare bytes at cmpBuff
	// with bytes at cmpAddr, at cmpLen length.
	// sets cmpMatch if buffers match. (Warning:
	// make sure not to try to compare invalid
	// segments as this could cause a segmentation
	// fault.)
	int cmpSet;
	void *cmpAddr;
		unsigned char *cmpBuff;
		char *procFsPathString;
		int cmpLen;
		int cmpMatch;
			
	struct _DEBUG_CONDITION_LIST * first;
	struct _DEBUG_CONDITION_LIST * prev;
	struct _DEBUG_CONDITION_LIST * next;
		
} DEBUG_CONDITION_LIST, *P_DEBUG_CONDITION_LIST;



// -------------------- LINUX IA32 BREAKPOINT STRUCTURE -----------------------------

// this linked list holds created breakpoints
// that are to be evaluated.  bpInst is the 4 byte value
// corresponding to the first 4 bytes of where 
// bpAddr was set. bpAddr holds the address of the
// breakpoint.  bpNumber is a set value for the breakpoint
// in the list, and installed reflects the status of the
// breakpoint. 
typedef struct _DEBUG_BREAKPOINT_LIST {
	void * bpAddr;
	void * bpInstruction;
	int bpNumber;
	int installed;
	
	// this flag ties a breakpoint to a condition evaluation.
	// When the breakpoint generates the sigtrap and it is found
	// that the breakpoint correlated with this element is to be
	// restored, this flag is evaluated for true/false (1/0) status.
	// If it is true, the breakpoint is patched, and a single step loop
	// is evaluated.  Instruction count must be specified, may be set
	// to -1 for infinate loop.
	int testConditionsOnBreak;
	int testConditionInstructionCount;
	
		struct _DEBUG_BREAKPOINT_LIST * first;
		struct _DEBUG_BREAKPOINT_LIST * prev;
		struct _DEBUG_BREAKPOINT_LIST * next;
} DEBUG_BREAKPOINT_LIST, *P_DEBUG_BREAKPOINT_LIST;


// -------------------- LINUX IA32 HEAP STRUCTURES -----------------------------

// all heap calculations are linux specific, different
// architectures will require different calculations.

// This list will hold heap chunks from which will be
// evaluated in succession.  It is VERY useful when
// determining heap corruption.  If heap size is only
// eight bytes long, and contains valid heap pointers
// then it is very likely that it is the end of the list,
// which contains the flink/blink pointers
 typedef struct _HEAP_CHUNK {
 	size_t prevSize;
 	size_t size;     // the size word unedited (aka the real value)
	size_t realSize; // the total size of the heap size, allocated (double word aligned)
 	void * data;
 	
 	// holds a pointer to the base where this heap chunk
	// starts
 	void * base;
	 
	 
 	// marks if this element of the heap
 	// layout appears corrupted
 	int isCorrupted;
 	
 	// marks if this element of the heap
 	// appears to be subject to a likely
 	// overflow.
 	int likelyOverflow;
 	
 	// this is set by comparing 4 byte pointers
 	// in a list, size and prevSize pointers
 	// then comparing pointers in the data section
 	// of the heap list.  If the two first pointers
 	// in the heap list are valid to that heap, we know
 	// to some degree of reliability that this is our last
 	// pointer.
 	int isLast;
 	
 	// these are null if isLast is false
 	void * forwardLink;
 	void * backLink;
 	
 	struct _HEAP_CHUNK * first;
 	struct _HEAP_CHUNK * prev;
 	struct _HEAP_CHUNK * next;
 	
 } HEAP_CHUNK, *P_HEAP_CHUNK;
    
    
// Generic list to hold heap elements (in case of multiple heaps)
typedef struct _PROCESS_HEAP {
	
	// holds a list referencing all heap chunks
	// in the list.  It is possible to identify
	// corrupt heap chunks via this list.
	P_HEAP_CHUNK heapChunks;
	
	// holds a pointer to the base of the heap
	void * base;
	
	// holds a pointer to the top of the heap 
 	void * top;
 	
 	// determines if its large enough to exploit, 0 off 1 on
 	int sizeExploitable;
 	
 	// this is flagged during analysis, verifying if the
 	// heap is large enough to be exploited using modern
 	// heap exploitation.
 	int verifiedExploitable;
 	
	struct _PROCESS_HEAP * first;
	struct _PROCESS_HEAP * prev;
	struct _PROCESS_HEAP * next;
		
} PROCESS_HEAP,*P_PROCESS_HEAP;


// ----------------- LINUX IA32 STACK STRUCTURES ------------------------

// this structure contains details on all linux ia32 stack structures running
// on a linux host. 

typedef struct _PROCESS_STACK {
	void * top;
	void * base;
	
	int likelyOverflowed;
	int isCorrupted;
	
	struct _PROCESS_STACK * first;
	struct _PROCESS_STACK * prev;
	struct _PROCESS_STACK * next;
} PROCESS_STACK, *P_PROCESS_STACK;


// ----------------- LINUX IA32 MASTER DEBUGGER HANDLE ------------------------

// Ptrace specific debug handles
typedef struct _DEBUG_HANDLE {
	
	// binary path of executable (if available)
	char *executable;
	bool isFromExe;
	
	// argument vector of executable (if available)
	char **argv;
	
	// pid of the process currently being debugged
	int debugPid;
	
	// is the debugger attached, 0 false 1 true
	int attached;
	
	// last signal the process provided
	int signal;
	
	// string describing the last signal occuring
	char *sigDesc;
	
	// value that ptrace returns when attempting to attach.  Used
	// for error determinations.
	int traceReturn;
	
	// ------------ MEMORY MAPPINGS/HEAP/STACK -------------
	// holds a valid memory map of the process
	// being debugged.  Must be retrieved before
	// usable.
	P_MEMORY_MAP memory;
	
	// updatable list of heap structures
	// very useful for detecting heap corruptions.
	P_PROCESS_HEAP heap;
	
	// updatable list of stack structures
	P_PROCESS_STACK stack;
		
	// --------BREAKPOINTS/CONDITIONS/WATCHPOINTS---------
	// list of conditions to apply to a stepping debugger
	// loop.
	P_DEBUG_CONDITION_LIST conditions;
	
	// if a condition has been activated via a stepping loop
	// this will hold a pointer (not a copy, direct pointer)
	// to the exact condition which was last raised.
	P_DEBUG_CONDITION_LIST lastConditionRaised;
	
	// breakpoint list, holds all breakpoints
	P_DEBUG_BREAKPOINT_LIST breakpoints;
	
	// holds address of last active breakpoint
	P_DEBUG_BREAKPOINT_LIST lastActiveBreakpoint;
	
	// -------- LAST REGISTER VALUES --------------
	// stores each register for ease of access after
	// each step if in singlestepping mode, or on signal
	// occurance in continue mode.
		size_t eax;
		size_t ebx;
		size_t ecx;
		size_t edx;
		size_t ebp;
		size_t esp;
		size_t esi;
		size_t edi;
		size_t eip;
				
		// store eflags
		unsigned short eflags;
		
		// original eax (for syscalls)
		size_t orig_eax;
		
		// store segment registers (unsigned shorts)
		unsigned short cs;
		unsigned short __cs;
			
		unsigned short ds;
		unsigned short __ds;
			
		unsigned short es;
		unsigned short __es;
				
		unsigned short fs;
		unsigned short __fs;
				
		unsigned short gs;
		unsigned short __gs;
				
		unsigned short ss;
		unsigned short __ss;
		
		// registers for 64bit user_regs_struct
#if __WORDSIZE == 64

		unsigned long r15;
		unsigned long r14;
		unsigned long r13;
		unsigned long r12;
		unsigned long rbp;
		unsigned long rbx;
		unsigned long r11;
		unsigned long r10;
		unsigned long r9;
		unsigned long r8;
		unsigned long rax;
		unsigned long rcx;
		unsigned long rdx;
		unsigned long rsi;
		unsigned long rdi;
		unsigned long orig_rax;
		unsigned long rip;
		unsigned long cs;
		unsigned long eflags;
		unsigned long rsp;
		unsigned long ss;
		unsigned long fs_base;
		unsigned long gs_base;
		unsigned long ds;
		unsigned long es;
		unsigned long fs;
		unsigned long gs;

#endif


} DEBUG_HANDLE, *P_DEBUG_HANDLE;

#endif

#endif // #if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1

#endif // USE_BLOCKPARTY_DEBUGGING == 1

#endif // BP_LINUX_DEBUG_STRUCTURES_H

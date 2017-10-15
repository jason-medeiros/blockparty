/*
 *
 * Blockparty Main Class Header File:
 *
 * Undefine defines in BP-Main.h in order to exclude components from
 * compilation.
 *
 * NOT FOR PUBLIC RELASE::ANY VIOLATION IS SUBJECT TO LEGAL PENALTY
 *
 */
#ifndef BP_CLASS_H
#define BP_CLASS_H


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Block Class Defines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Set the max warning/error string lengths (64K)
#define BP_CLASS_MAX_WARNING_STR_LEN BP_KILOBYTE * 64
#define BP_CLASS_MAX_ERROR_STR_LEN   BP_KILOBYTE * 64


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Block Class Enumerators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// simple enumerator for role definitions
typedef enum BP_USER_ROLE
{
	BP_USER_ROLE_NONE,
	BP_USER_ROLE_GUEST,
	BP_USER_ROLE_USER,
	BP_USER_ROLE_ADMIN
};

// when a user is authenticated, it's stored in this structure
typedef struct __BP_USER {

	char * first_name;
	char * last_name;
	char * email;
	char * username;
	char * created;
	char * role;

	// flag indicating whether or not this user is
	// authenticated.
	BP_BOOL authenticated;

	// user id associated
	size_t user_id;

} BP_USER, *P_BP_USER;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Block Critical Section Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// enter critical section
#define BP_BLOCK_ENTER_CRITICAL_SECTION\
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(this->system_hreg);

// leave critical section
#define BP_BLOCK_LEAVE_CRITICAL_SECTION\
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(this->system_hreg);

// simple utility to leave critical section and return from function
#define BP_BLOCK_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(val)\
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(this->system_hreg, val);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Block MySQL Handle Specific Critical Section Macros %%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// enter critical section
#define BP_BLOCK_MARIADB_ENTER_CRITICAL_SECTION\
{                                                                   \
	if(this->mysqlSession)                                          \
		BP_HASH_TABLE_ENTER_CRITICAL_SECTION(this->system_hreg);    \
}

// leave critical section
#define BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION\
{                                                                              \
	if(this->mysqlSession)                                                     \
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(this->mysqlSession->system_hreg); \
}

// simple utility to leave critical section and return from function
#define BP_BLOCK_MARIADB_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(val)\
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(this->mysqlSession->system_hreg, val);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Block Class (main blockparty class) %%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Main Block Class Used With BlockParty
class Block
{

	// Public Functions/Data
public:


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% System Hash Table Registry %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// system hreg associated with the class
	P_BP_HASH_TABLE_REGISTRY system_hreg;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %% List Allocator (fast) %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this member is allocated during the object constructor and
	// can be used in order to allocate memory safely in a list
	// fashion.
	P_BP_LINKL tq;

	// -----------------------------BUFFER LIST STRUCTURES ---------------------//

	// classes native buffer list
	P_BP_DATA_LIST bufferList;

	// destroys the native buffer list
	int DestroyChain();

	// --------------------------CONSTRUCTOR FUNCTIONS ------------------//

	// Global Block Constructor
	// creates a block of TYPE
	Block(int type);

	// creates a block with no type
	Block();

	// If the block object is not allocated with "new" the constructor
	// routine isn't run to initialize local variables, this function allows
	// for those local variables to be initialized through routine instead
	// in order to make things much easier then manual initializations.
	int InitLocals();

	// Block class deconstructor
	~Block();

	// -----------------------------OPERATORS ---------------------------//
	// when using the char * prototypes, make sure that your
	// buffers are null terminated.
	/*
	Block& operator+=(Block incommingAppendingBlock);
	Block& operator+=(char *incommingBuffer);

	Block& operator +(Block incommingAppendingBlock);
	Block& operator +(char *incommingBuffer);

	Block& operator =(Block incommingAppendingBlock);
	Block& operator =(char *incommingBuffer);

	// how to add a read buffer to a block, similar to an
	// operator
	int AddReadBlock(int size, bool destroyList);

	// Adds a binary, data list element with BINARY type.
	// the buffer is stored in the binData as a malloc'ed pointer
	// instead of using the normal data static array.
	int AddBinaryBlock(unsigned char * buff, size_t len, bool destroyExisting);

	// Accesses the P_BP_DATA_LIST read buffer in
	// the read position specified. First read would be
	// the zero'th position.  Second read would be 1st, etc.
	P_BP_DATA_LIST AccessReadPos(int blockPos);
	*/
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Error Logging %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// toggle these to BP_TRUE or BP_FALSE in order to display warnings
	// or errors immediately.  Default for warnings is off, default for errors is on.
	BP_BOOL display_errors_immediately;
	BP_BOOL display_warnings_immediately;

	// Disabled by default.  Enable to allow logging of error/warning strings.  If you enable this
	// be sure to flush the error stacks periodically or you risk creating a leak.
	size_t msg_logging_enabled;

	// number of errors encountered
	size_t errorCount;

	// array of null ptr terminated error strings
	char ** errorStrings;

	// number of warnings encountered
	size_t warningCount;

	// array of null ptr terminated warning strings
	char ** warningStrings;

	// add an element to error array
	int AddErrorToStack(char *errorMsg);

	// add a warning to the error stack
	int AddWarningToStack(char *warningMsg);

	// Print errors to stdout
	int DisplayErrors();

	// Print warnings to stdout
	int DisplayWarnings();

	// destroys errors
	int DestroyErrors();

	// destroys warnings
	int DestroyWarnings();

	// calls destroy errors and destroy warnings successively.
	int DestroyErrorsAndWarnings();

	// ----------------------------UTILITY-----------------------------//

	// Uses fork() to set a process exit timeout
	BP_ERROR_T setProcessExecutionTimeout(size_t timeout);

	// Tests to see if a regulare expression item is matched in a null
	// pointer terminated array of strings.
	// This function returns 0 on error, and a string pointer position on
	// success.  Notice: 0 index in ptr array is the first position (aka 1).
	size_t matchRegexpInBlacklist(char ** blacklist, char *item);

	// Counts pointers in a null terminated pointer array and returns in size_t.
	size_t countNullTerminatedPtrArray(char **count_array);

	// ----------------------------NETWORK-----------------------------//

#ifdef BP_BLOCKCLASS_INCLUDE_NETWORK_BINDINGS

	// this is the network handle native to a Block
	P_BP_NETWORK_HANDLE net_handle;

	// Play a list of buffers, including recieve buffers contained within a
	// buffer list.
	int SockPlay();

	// connects on a remote socket given the functional parameters
	// proto is IPPROTO_TCP or IPPROTO_UDP
	int SockConnect(char * destAddr, short destPort, int proto);

	// listens on a local socket given the functional parameters
	// proto is IPPROTO_TCP or IPPROTO_UDP
	int SockListen(char * srcAddr, short srcPort, int proto);

	// accepts and assigns the current blocks net_handle to a new accepted P_BP_NETWORK_HANDLE
	// structure that was created by validating a listening P_BP_NETWORK_HANDLE
	// established socket structure argument.
	int SockAccept(P_BP_NETWORK_HANDLE acceptHandle);

	// prototype, same as above but automatically destroys the listening
	// local sock structure, replacing it with the new accepted socket
	// structure.
	int SockAccept();

	// closes and frees a socket object
	int SockClose();

	// sends the contents of the block object data list
	int SockSendBlock();

	// destroys (frees) the local block net_handle tree
	int SockDestroy();

	// sends the specified ammount of data from the objects
	// net_handle.
	int SockSend(char *data, int size);

	// recieves the specified ammount of data from the objects
	// net_handle.
	int SockRecv(char *data, int size);

#endif // BP_BLOCKCLASS_INCLUDE_NETWORK_BINDINGS
	// ----------------------------SSL---------------------------------//

#ifdef BP_BLOCKCLASS_INCLUDE_NETWORKSSL_BINDINGS

	// must be called before using any ssl functionality
	BP_ERROR_T SSLInit();

	// connects using the specified ssl method
	// applicable methods are:
	// SSLv2_method()
	// SSLv3_method()
	// TLSv1_method()
	BP_ERROR_T SSLConnect(SSL_METHOD *sslMethod);

	// plays the contents of the block object data list,
	// reads included
	BP_ERROR_T SSLPlay();

	// sends the contents of the block object data list
	BP_ERROR_T SSLSendBlock();

	// sends a buffer out the ssl oriented socket
	int SSLSend(char *data, int size);

	// recieves size data from the ssl oriented socket
	int SSLRecv(char *data, int size);

	// [Important:] SockClose also closes and frees existing
	// SSL connections



#endif // BP_BLOCKCLASS_INCLUDE_NETWORK_BINDINGS

	// ----------------------------DATA GENERATORS---------------------//

#ifdef BP_BLOCKCLASS_INCLUDE_FUZZING_BINDINGS

	// The destroyExistingList parameter will destroy (aka free) the
	// existing buffer list if set to DESTROY.  If set to KEEP or any
	// other value, it will simply create the data and append it to the
	// list.

	// generates size number of bytes worth of random data and
	// stores them in the native bufferList
	Block& GenRandomData(int size, int destroyExistingList);

	// regenerates all blocks of data considered random
	// int RegenRandomData(bool randomizeSize);

	// generates size number of bytes worth of format specifiers and
	// stores them in the native bufferList
	Block& GenFormatData(int size, char specifier, int destroyExistingList);

	// generates count number of byte sequences which are each seqLen
	// long, and stores them in the native bufferList
	Block& GenSequenceData(int count, char *seq, int seqLen, int destroyExistingList);

#endif // BP_BLOCKCLASS_INCLUDE_FUZZING_BINDINGS
	// ---------------------------- FILES ----------------------------//

#ifdef BP_BLOCKCLASS_INCLUDE_FILE_BINDINGS

	// ---------------------------------------
	// functions for searching the filesystem

	// most recent searched filesystem list
	P_BP_FILE_LIST fileList;
	P_BP_FILE_LIST searchList;

	// most recent executed file fileDescriptor sets
	P_BP_FDSET fdSet;

	// Executes the given file at path provided that the
	// user has privileges to execute the file. If the
	// returnFds flag is set to true, it will return all
	// standard file streams (stdin/out/err) in the classes
	// fdSet struct.
	int File_Execute(char *filePath, char **params, char **envp, BP_BOOL returnFds);

	// searches the filesystem from specified rootDirPath for all
	// the types that match the type specified.
	int File_FindAllType(char *rootDirPath, BP_FILETYPE type, bool recursive);

	// searches the existing list for conditions, stores result in searchList
	int File_ListSearch(char *name, bool execute, bool write, bool read, bool elf, uid_t uid, gid_t gid);

	// prints out all files in the fileList
	int File_PrintFileList();

	// prints out all files in the searchList
	int File_PrintSearchList();

	// destroys the objects file list
	int File_DestroyFileList();

	// destroys the objects searchList
	int File_DestroySearchList();

	// destroys both lists
	int File_DestroyLists();

	// ---------------------------------------
	// functions for manipulating file descriptors:

	// Note: These are not used to operate on resources
	// local to the class.

	// Plays the entire buffer list, reads included
	int FD_Play(int fdRead, int fdWrite);

	// Sends the entire bufferList in the object to the
	// specified file descriptor.
	int FD_SendBlock(int fd);

	// opens a file descriptor, and returns a stream FILE object
	// use FClose to close the descriptor not FDClose
	int FD_Open(int fd, char *mode);

	// closes an open file descriptor (not a FILE)
	int FD_Close(int fd);

	// Writes to a given file descriptor
	int FD_Write(int fd, char *buff, int size);

	// Reads from a given file descriptor
	int FD_Read(int fd, char *buff, int size);

	// ---------------------------------------
	// functions for manipulating file streams
	FILE *file;

	// if FileGetContents is called, this contains the contents.
	char *got_contents;
	size_t got_contents_size;

	// opens a file on the filesystem with accessString permissions
	// (aka. "r", "r+", "w")
	int File_Open(char *rootDirPath, char *accessString);
	FILE * File_OpenFptr(char *rootDirPath, char *accessString);

	// closes a file stream
	int File_Close(FILE * inFile);
	int File_Close(); // Uses default class FILE pointer

	// writes across a file stream
	int File_Write(FILE * inFile, char *buff, int size);
	int File_Write(char *buff, int size); // Uses default class FILE pointer

	// reads from a file stream
	int File_Read(FILE *file, char *buff, int size);
	int File_Read(char *buff, int size); // Uses default class FILE pointer

	// lazy func to get the contents of a file
	int FileGetContents(char *file);



#endif // BP_BLOCKCLASS_INCLUDE_FILE_BINDINGS


	// ----------------------------THREADS---------------------------------//

#ifdef BP_BLOCKCLASS_INCLUDE_PTHREAD_BINDINGS

#ifdef BP_BLOCKCLASS_INCLUDE_USE_OLD_PTHREAD_METHODS

	// static thread info, contains a semaphore
	BP_THREAD_MGMT thread_info;

	// thread data
	P_THREAD_LIST threads;

	// creates a new thread and appends it to the threads list
	int CreateNewThread(void *(startRoutine)(void*), void *arg, int argLen, char * description);

	// Gets a thread by the function pointer it was called with.
	// If multiple threads have the same function pointer, the first
	// is returned.
#ifdef __GNUC__
	int GetThreadByStartRoutine( void * (*startRoutine)(void*) );
#endif

	// Gets a thread number from the list by comparing strings to
	// the list of thread descriptions.  cmpLen is the number of bytes
	// that will be compared to the desc string, starting at the
	// actual thread description.
	int GetThreadByDescription(char *desc, int cmpLen);

	// Gets the actual thread ptr by description.
	P_THREAD_LIST GetThreadPtrByDescription(char *desc, int cmpLen);

	// Redispatches a thread (re-runs) using the thread structure provided.
	BP_ERROR_T redispatchThread(P_THREAD_LIST thread);


	// It is important to know that "destroying" threads does
	// not actually cancel/kill the thread, but instead only
	// destroys the local threads list.  This will free the argument
	// data passed to the thread, but not free any resources related
	// to the thread besides that.  When the thread exits normally
	// their resources will be reprovisioned to the OS and the thread
	// will be usable again.

	// destroys the thread at the position threadNumber
	int DestroyThread(int threadNumber);

	// iterates through the threads list and attempts to destroy
	// each thread structure.  Does not destroy thread sharedData.
	int DestroyAllThreads();

	// prints out all active threads in the threads list
	void PrintThreads();

	// ====== Threaded Auto-Processor ==========================

	// This processor routine will create thread_n (count) threads, with startRoutine as an entry point.  The threads
	// will take arguments directly from the thread_params pointer array.  New threads will be created as threads exit
	// untill each element of thread_params has been completed.  After this routine has exited, you can check the individual
	// parameter structures for information regarding their arbitrary input/output/statuses.
	BP_ERROR_T threadedDataProcessor(size_t thread_n, void *(startRoutine)(void*), void ** thread_params, size_t param_n);

	// Rejoins all threads in the local threads list
	BP_ERROR_T rejoinThreads();

#endif


#ifdef BP_BLOCKCLASS_INCLUDE_USE_NEW_PTHREAD_METHODS

	// thread manager
	P_BP_THREAD_MANAGER thread_manager;

	// ====== Threaded Auto-Processor ==========================

	// This processor routine will create thread_n (count) threads, with startRoutine as an entry point.  The threads
	// will take arguments directly from the thread_params pointer array.  New threads will be created as threads exit
	// untill each element of thread_params has been completed.  After this routine has exited, you can check the individual
	// parameter structures for information regarding their arbitrary input/output/statuses.
	BP_ERROR_T threadedDataProcessor(size_t thread_n, void *(startRoutine)(void*), void ** thread_params, size_t param_n);

	// create a new thread
	BP_ERROR_T createNewThread
	(
		void *   (start_routine)(void*),
		void *   arg,
		size_t   arg_len,
		BP_BOOL  start_thread_upon_creation,
		char *   name = NULL,
		char *   description = NULL
	);

	// rejoin all the threads in the manager
	BP_ERROR_T rejoinAllThreads();


#endif


#endif // BP_BLOCKCLASS_INCLUDE_PTHREAD_BINDINGS
	// ---------------------------- DEBUGGING -------------------------------//

#ifdef BP_BLOCKCLASS_INCLUDE_DEBUGGER_BINDINGS

#if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1


	// Debugging interface
	P_DEBUG_HANDLE dbgHandle;

	// List of all memory segments found to be elf
	P_ELF_32BIT_LOOKUP_TABLE_LIST elf;

	// Exploit analysis list
	P_EXPLOIT_ANALYSIS_LIST exploitAnalysis;

	// Fully populatable instruction table
	// can be filled initially manually, or calling refresh
	P_X86_32_INSTRUCTION instructionTable;

	// Usable local instruction list that
	// can be used to retrieve instructions from
	// the disassembly functions below.
	P_X86_32_DECODED_INSTRUCTION_LIST instructions;

	// syscall lookup table
	P_BP_LINUX_SYSCALL_LOOKUP syscallLookup;

	// Refreshes or creates a new x86 instruction table within the
	// class.  This is necessary for disassembling any instructions
	// using the class disassembler.  This does not need to be called
	// if debugging a process and having called DbgRefresh.  However
	// DbgRefresh does not generate a table if not debugging.  Rule
	// of thumb: not-debugging, use this function.
	int DbgRefreshInstructionTable();

	// Decodes all instructions in buff buffer till length.
	// Local Version: Allocated to local class pointer
	int DbgDecodeInstructions(void *buff, size_t length, size_t index);

	// Non-Local Version: Allocated to pointer
	P_X86_32_DECODED_INSTRUCTION_LIST DbgDecodeInstructionsNL(void *buff, size_t length, size_t index);

	// Utilizes the existing memory map and attempts to decode
	// instructions from a given address.

	// Local Version: Allocated to local class pointer
	int DbgDecodeInstructionsMem(void *addr, size_t length);

	// Non-Local Version: Allocated to pointer
	P_X86_32_DECODED_INSTRUCTION_LIST DbgDecodeInstructionsMemNL(void *addr, size_t length);

	// Destroy the local instruction list
	int DbgDestroyInstructions();

	// Destroy arbitrary instruction list
	int DbgDestroyInstructions(P_X86_32_DECODED_INSTRUCTION_LIST destroyList);

	// Prints the local instruction list stdout
	int DbgPrintInstructions();

	// Generates a buffer suitable for printing containing instructions
	// within the instruction list.
	char * DbgGenerateInstructionBuffer();

	// same as above, but uses a specified buffer instead
	char * DbgGenerateInstructionBuffer(P_X86_32_DECODED_INSTRUCTION_LIST instList);

	// Prints an arbitrary instruction list to stdout
	int DbgPrintInstructions(P_X86_32_DECODED_INSTRUCTION_LIST instList);

	// Retrieves the number of instructions in the debugged instruction
	// list.
	size_t DbgGetInstructionCount();

	// Write instruction list search functions next

	// file that will contain our payloads.  File
	// must be delimited with <exploitbuff> tags with
	// proper identifiers.
	FILE * triggerFile;

	// --------------------------------------
	// Functions to access platform debugging
	// libraries (ptrace/win32)

	// Attaches to a pid
	int DbgStartPid(int pid);

	// Starts a new process with argv/envp pointers
	int DbgStartCreateProc(char * path, char **args, char **environment);

	// Function responsible for continuing debugging:
	// On each loop all comparisons in our debug handle are
	// evaluated.  Returns true if a condition evaluated.
	int DbgContinue();

	// Steps the debugger loop one step, no conditions
	// are evaluated during this step.
	int DbgStep();

	// Function to detach from debugging session
	// this also destroys the debug handle, resetting
	// it to null.
	int DbgDetachDestroy();

	// only detaches
	int DbgDetach();

	// only destroys
	int DbgDestroy();

	// sets a breakpoint
	int DbgAddBreakpoint(void * addr, int bpIdentifier);

	// removes a breakpoint
	int DbgRemoveBreakpoint(int bpIdentifier);

	// removes all breakpoints
	int DbgRemoveAllBreakpoints();

	// prints out all breakpoints
	int DbgPrintBreakpoints();

	// returns ERR_SUCCESS if the breakpoint is installed
	// and ERR_FAILUE if it is not
	int DbgBreakpointIsSet(int bpIdentifier);

	// returns the address of the breakpoint if set, null if not
	void * DbgBreakpointGetAddr(int bpIdentifier);

	// --------------------------
	// Functions to set conditions
	// Note: Calling [the same] Set condition twice in a row
	// creates a new condition in the list, eg. it does not
	// overwrite an existing condition.

	// eax
	int SetEaxCondition(int eax, bool isGt, bool isLt);
	// ebx
	int SetEbxCondition(int ebx, bool isGt, bool isLt);
	// ecx
	int SetEcxCondition(int ecx, bool isGt, bool isLt);
	// edx
	int SetEdxCondition(int edx, bool isGt, bool isLt);
	// ebp
	int SetEbpCondition(int ebp, bool isGt, bool isLt);
	// esp
	int SetEspCondition(int esp, bool isGt, bool isLt);
	// esi
	int SetEsiCondition(int esi, bool isGt, bool isLt);
	// eip
	int SetEipCondition(int eip, bool isGt, bool isLt);

	// memory comparison
	// on loop compares whats in cmpBuff to the process memory
	// at procAddr, for cmpLen bytes. CPU INTENSIVE.
	int SetMemCondition(void *procAddr, void *cmpBuff, int cmpLen);

	// destroys all conditions in the debugHandle
	int DestroyConditions();

	// prints out all conditions and flags there in
	// as well as flag status.
	void PrintConditions();

	// Debug Heap Functions


	// Debug Utility Functions
	// gets the registers from the process, storing them
	// locally in the dbgHandle
	int DbgGetRegs();

	// compares memory, return ERR_SUCCESS on true, ERR_FAILURE
	// on false.
	int DbgCmpMem(void *procAddr, void *cmpBuff, int cmpLen);

	// retrieves memory, returns a malloced buffer of readLen
	// buffer must be free'd if retrieved
	unsigned char * DbgReadMemory(void *processAddr, int readLen);

	// prints out printLen bytes of memory from the process starting
	// at void *processAddr 0x.08 style
	int DbgPrintMem(void *processAddr, int printLen);

	// prints out the stack ranges of the current stack
	int DbgPrintStack();

	// prints out the heap for the current heap
	int DbgPrintHeap();

	// prints out registers for the current process
	int DbgPrintRegisters();

	// updates the various different debugger relevant
	// data: (memory maps, registers, stack, heap etc)
	int DbgRefreshData();

	// creates a valid security analysis given a valid debugging session
	// which can pinpoint security holes in an application.  The data from
	// the analysis is stored in directly in the classes exploit analysis structure.
	// Calling this function successively allows you to create a linked
	// list of exploit analysis conditions.
	int DbgExploitAnalysis(char * payload, int payloadLength);

	// needs to be implemented
	int DbgDestroyExploitAnalysis();

	// sets the exploit file to use, reads and parses
	// out any blockparty tags in the file.
	int DbgSetTriggerFile(char * triggerPath);

	// Writes a payload, into a PoC file at a position.
	// The specifics of how the buffer are written are language dependent.
	// The language being written with is determined via language tag
	// in the <exploitbuff> structure
	int DbgWriteTriggerBuff(char *triggerPath, char * payload, int payloadLength, int triggerId);

	// returns the number of the element in the linked list
	// that has the highest percentage match to our exploit
	// condition.
	// Requires: Valid exploit analysis
	int DbgExploitGetMostLikely();

	// Prints out the stack analysis
	void DbgPrintStackAnalysis();

	// Prints out the heap analysis
	void DbgPrintHeapAnalysis();

	// Prints out the register analysis
	void DbgPrintRegisterAnalysis();

	// prints out all analysis
	void DbgPrintAllAnalysis();

	// Gets a proper memory map from a debug handle
	P_MEMORY_MAP DbgPointerToMemMap(size_t checkPtr);
#endif

#endif // BP_BLOCKCLASS_INCLUDE_DEBUGGER_BINDINGS
	// ---------------------------- UTILITY -------------------------------//

	// prints out the classes native bufferList
	BP_ERROR_T PrintList();

	// ---------------------------- MYSQL DATABASE ------------------------------

#ifdef BP_BLOCKCLASS_INCLUDE_MYSQL_DB_BINDINGS

	// local structure members
	P_BP_MYSQL_RESULT  mysqlResults;
	P_BP_MYSQL_SESSION mysqlSession;

	// connect to mysql database
	BP_ERROR_T MysqlConnect(char *mysqlHost, char *mysqlUser, char *mysqlPass, char *mysqlDB, size_t port);

	// attempts to reconnect where necessary
	// NOTE: THIS MUST RETURN int AS THEY RETURN INFORMATION FROM mysql_query
	int MySQLReconnect();

	// closes an established mysql connection
	BP_ERROR_T MysqlClose(P_BP_MYSQL_SESSION mysqlSession = NULL);

	// --- Query Methods ----

	// null terminated string querys only
	// NOTE: THIS MUST RETURN int AS THEY RETURN INFORMATION FROM mysql_query
	int MysqlQuery(char *query);

	// Real query can contain binary data, specify query length appropriately
	// or face the consequences.
	// NOTE: THIS MUST RETURN int AS THEY RETURN INFORMATION FROM mysql_query
	int MysqlRealQuery(char *query, size_t len);


	// --- "Get" methods ------

	// retrieves results from the last query
	BP_ERROR_T MysqlGetResults();

	// Returns a row structure from the results
	MYSQL_ROW MysqlGetRow();

	// returns the length array corresponding to the MYSQL_RESULT returned from MysqlGetResults
	// terminated with negative val.
	unsigned long * MysqlFetchLengths();

	// Create New Database: Non-depreciated CREATE DATABASE style
	int MysqlCreateDb(char *dbName);

	// ------------- Database Semaphore Locking ---------

	// Use the following to lock the mysql session before doing any
	// threaded db work.
	BP_ERROR_T mysqlSemLock();

	// unlock to stop blocking this semaphore.  Call only after a successful lock
	// or you can run into contention problems.
	BP_ERROR_T mysqlSemUnlock();

	// ------------- Prepared Statements ----------------

	// statment
	MYSQL_STMT * mysql_stmt;

	// results structure used with fetch
	MYSQL_BIND * mysql_results;

	// current number of results in results array
	size_t mysql_results_count;

	// paramters structure used with prepare
	MYSQL_BIND * mysql_parameters;

	// current number of parameters in parameters array
	size_t mysql_parameter_count;

	// parameter count from mysqlPrepareQuery() or 0 if unpopulated
	size_t mysql_known_parameter_count;

	// mysql aes db key for use when using database aes preperation
	char * mysql_aes_key;

	// AES encrypt/decrypt for prepared statements.  Any ? in the string
	// is replaced with AES_ENCRYPT(?, 'key_here').   Anything surrounded by
	// {value_name} is replaced with AES_DECRYPT(value_name, 'key_here').  Key must be
	// alphanumeric.  Returned value is allocated and must be freed after use.
	char * mysqlAesPrepare(char *sql, char *key);

	// prepare mysql query ( ? parameters)
	BP_ERROR_T mysqlPrepareQuery(char * sql);

	// adds a parameter to the parameters stack
	BP_ERROR_T mysqlAddParameter(enum_field_types col_type, char *buffer, size_t buffer_length);

	// binds parameters to a statement
	BP_ERROR_T mysqlBindParameters();

	// adds a  result to the result stack (used for fetching data, one result per column)
	BP_ERROR_T mysqlAddResult(enum_field_types col_type, char *buffer, size_t buffer_length);

	// executes a prepared statement
	BP_ERROR_T mysqlExecuteStatement();

	// binds result configuration to a statement
	BP_ERROR_T mysqlBindResults();

	// store results (optional, copies data locally before fetch is called)
	BP_ERROR_T mysqlStoreResults();

	// fetch result
	BP_ERROR_T mysqlFetchResult();

	// destroy statement (free)
	BP_ERROR_T mysqlDestroyStatement();

	// utility functions for displaying
	BP_ERROR_T mysqlPrintResultArray();
	BP_ERROR_T mysqlPrintParamArray();

#endif // BP_BLOCKCLASS_INLUDE_MYSQL_DB_BINDINGS

#ifdef BP_BLOCKCLASS_INCLUDE_HTML_BINDINGS

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%% HTTP/HTML ASSESSMENT ROUTINES %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	P_BP_HTTP_SESSION httpSession;

	// Browser agent; must be set to a null terminated string
	char * userAgent;

	// Set this to true if you wnat to override default print behavior and display
	// blacklisted entries when the form prints out (aka enable to print everything)
	bool html_display_blacklisted;

	// This function initializes the http subcomponents in the class
	// and starts a new http session.
	int HttpInit();

	// Threaded cURL requests cannot use signals.  It causes incorrect longjmp's and a host
	// of other mess.  Use this to fix that.
	BP_ERROR_T HttpDisableCurlSignalsForThreadedApps();

	// Sets the user agent
	int HttpSetUserAgent(char *agent);

	// Sets a random user agent from the built in table.
	int HttpSetRandomUserAgent();

	// Sets username and password for site requests
	int HttpSetUserPass(char *user, char *pass);

	// Sets cookie
	int HttpSetCookie(char *cookie);

	// Sets cookie jar
	int HttpSetCookieJar(char *cookie_file);

	// Sets post data
	int HttpSetPostData(char *postStr);

	// Makes a request against a provided URL, CAN BE HTTPS :)
	int HttpMakeRequest(char *url, bool log_to_mysql = true);

	// Logs a the last Http request to mysql (request must be made with this->HttpMakeRequest(...);
	int HttpLogLastRequestToMySQL();

	// Destroy Sessions
	int HttpCloseDestroySession();

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%% CURL VALUE GETTRS %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Gets a null terminated ascii string containing the ip address of the
	// last connection made by curl.
	char * HttpGetRemoteIp();

	// ---------------- USEFUL INJECTION GENERATORS ----------------------

	// All string encodings below will store results here as well
	// as return this same pointer as a result.
	char *lastInjEncode;

	// Takes the input string and converts it to stringFromCharCode
	// style.
	char * EncXSSInjectionStringEncode(char *str, int len);

	// Encodes as UTF8
	char * EncUTF8StringEncode(char *str, int len);

	// MSQL styled CHAR() encoding
	char * EncMSQLCharFuncInjectionStringEncode(char *str, int len);

	// MySQL styled CHAR() encoding
	char * EncMYSQLCharFuncInjectionStringEncode(char *str, int len);

	// *C injection string encoder
	char * EncCInjectionStringEncode(char *str, int len);

	// *changes it to a php suitable injection string
	char * EncPHPInjectionStringEncode(char *str, int len);

	// *INLINE VERSION* Function will take a C string and convert it to a ASM
	char * EncInlineASMStringEncode(char *str, int len);

	// *NON-INLINE VERSION* Function will take a C string and convert it to ASM
	char * EncASMStringEncode(char *str, int len);

	// ---------------- HTML EXTRACTION ROUTINES -------------------------

	// class html structure
	P_BP_HTML_FULL_EXTRACTION htmlExtraction;

	// saved html buffer (if supplied to HtmlParse, the buffer will be stored
	// here). STRNDUP'D, not the same buffer here, can be manipulated.
	char * unparsedHtml;
	size_t unparsedHtmlLen;

	// Provide a buffer, and it will extract a full HTML structure,
	// stores result in class-global htmlExtraction pointer as well
	// as returning it as a parameter.
	P_BP_HTML_FULL_EXTRACTION HtmlParse
	(
			char *url            = NULL,
			bool saveBuffInClass = true
	);

	// Prints to stdout a full extraction
	void HtmlPrintExtraction();

	// Destroys the current extraction
	int HtmlDestroyExtraction();

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%% HTML/PROXY ROUTINES %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ------------- Proxy Routines-------------------------------


	// CURRENT: Sets the current proxy config to use when making requests.
	size_t proxy_set_config_index;

	// CURRENT: Test site to use for validation (http://site)
	char *proxyHttpTestSite;

	// CURRENT: Expected string for validation (string in site body to match against "<title>site" etc)
	char *proxyHttpTestExpected;

	// CURRENT: Proxy timeout to use for testing
	size_t proxyHttpTestTimeout;



	// CURRENT: Function to retrieve a list of proxies seperated by type.  If
	// data exists in the current proxy set, it is destroyed and nulled before reuse.
	size_t GenerateUsableProxySetFromFile(char *filename, bool randomize_order);

	// CURRENT: Destroy the classes current proxy set data.
	size_t DestroyUsableProxySet();

	// CURRENT: Display usable proxy set.
	size_t DisplayUsableProxySet();

	// CURRENT: Validates the proxies in a proxy set to verify they are working.  Single threaded.
	size_t ValidateProxyConfig(P_BP_WEB_PROXY_EXTENDED_INFO cfg);


	// === DEPRECATED ===

	// DEPRECATED: list of proxies (must be null ptr terminated)
	char ** proxies;

	// DEPRECATED: list of validated proxies (for http) (pointers are from the **proxies array above,
	// do not treat as seperate pointers. NULL ptr terminated.)
	char ** validatedHttpProxies;

	// DEPRECATED: list of validated proxies that allow connect. (NULL ptr terminated)
	char ** validatedConnectProxies;

	// DEPRECATED: Function to retrieve List of proxies from file (newline delimited)
	char ** GenerateProxyListFromFile(char *filename);

	// DEPRECATED: Validates the proxy list, populating the validated proxy arrays.  Set flags for
	// specific checks (connect/http).  Use proxy sets instead if possible.
	size_t ValidateProxies(bool vhttp, bool vconnect, size_t proxy_type);

	// DEPRECATED: Validates one proxy (http)
	size_t ValidateHttpProxy(char *ip_addr, size_t curl_proxy_type);

	// DEPRECATED: Validates one proxy (connect)
	size_t ValidateConnectProxy(char *ip_addr);

#endif // BP_BLOCKCLASS_INCLUDE_HTML_BINDINGS

#ifdef BP_BLOCKCLASS_INCLUDE_PROXY_DB_BINDINGS

	// ========== Custom Proxy DB Interface =====================

	// This simple interface allows the building of custom applications that utilize the grief/jciaf
	// proxy table format.  The proxy table is defined as shown below.   A valid this->mysqli object must
	// be connected to a valid server/database containing that defined table in order for any of the functions
	// below to be useful.

	/*
		CREATE TABLE IF NOT EXISTS `proxies` (
		  `id` int(11) NOT NULL AUTO_INCREMENT,
		  `user_id` int(11) NOT NULL,
		  `group_id` int(11) NOT NULL,
		  `proxy_type` int(11) NOT NULL,
		  `host` varchar(256) NOT NULL,
		  `port` varchar(5) NOT NULL,
		  `username` text NOT NULL,
		  `password` varchar(256) NOT NULL,
		  `user_notes` varchar(1024) NOT NULL,
		  `created_ts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
		  `last_validated_ts` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
		  `enabled` int(11) NOT NULL DEFAULT '1',
		  PRIMARY KEY (`id`)
		) ENGINE=InnoDB  DEFAULT CHARSET=latin1 ;

		CREATE TABLE IF NOT EXISTS `proxy_groups` (
  	  	  	  `id` int(11) NOT NULL AUTO_INCREMENT,
  	  	      `user_id` int(11) NOT NULL,
  	  	  	  `name` varchar(32) NOT NULL,
  	  	  	  `enabled` int(11) NOT NULL,
              `created_ts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  	  	  	  `user_notes` varchar(1024) NOT NULL,
  	  	  	  PRIMARY KEY (`id`)
		) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=7 ;

	 */

	// proxy groups that have been populated dynamically
	P_BP_PROXY_DB_GROUP db_populated_proxy_groups;
	size_t              db_populated_proxy_group_n;


	// --- DB Proxy Add Routines ----

	// if a group was added
	size_t db_last_added_group;

	// add group
	BP_ERROR_T addProxyDBGroup
	(
			char *group_name
	);

	// add proxy
	BP_ERROR_T addProxyDBEntry
	(
			size_t group_id,
			size_t proxy_type,
			char * host,
			size_t port,
			char * username,
			char * password,
			char * user_notes,
			size_t enabled
	);


	// This attempts to parse any number of entries and sort them out into
	// insertable records in the database.
	BP_ERROR_T addProxyDBEntriesFromGenericFormatProxyBuff
	(
			char *file_containing_proxies_to_insert
	);


	// --- DB Proxy Get Routines ----

	// generates a proxy group set via search criteria (zero/null for empty search values)
	BP_ERROR_T getProxyDBGroupSetByCriteria
	(
			size_t id,
			size_t user_id,
			char *group_name,
			size_t bottom_limit = 0,
			size_t top_limit = 1024
	);

	// This set of proxies is the primary place to find proxy data after requests
	// have been made.
	P_BP_PROXY_DB_ENTRY db_populated_proxy_sets;
	size_t              db_populated_proxy_n;

	// generates a proxy set via criteria
	BP_ERROR_T getProxyDBSetByCriteria
	(
			size_t id,
			size_t user_id,
			size_t group_id,
			size_t proxy_type,
			char *host,
			char *username,
			char *password,
			size_t port,
			char *user_notes,
			size_t validation_status,
			char *created_ts_ge,
			char *created_ts_le,
			size_t bottom_limit = 0,
			size_t top_limit = 1024
	);

	// Validation test sets are used for validating that proxies are alive and working as intended.
	// Proxy groups are used to manage sets of validation tests.  These variables contain said proxy groups.
	P_BP_PROXY_DB_VALIDATION_TEST_GROUP db_populated_proxy_validation_groups;
	size_t								db_populated_proxy_validation_group_n;

	// pulls proxy validation groups by provided criteria.  null criteria is an empty (blanket) search.
	BP_ERROR_T getProxyDBValidationGroupSetByCriteria
	(
			size_t id,
			size_t user_id,
			char *group_name,
			char *group_description,
			char *created_ts,
			size_t bottom_limit = 0,
			size_t top_limit = 1024
	);


	// Individual validation test sets
	P_BP_PROXY_DB_VALIDATION_TEST_ENTRY db_populated_proxy_validation_tests;
	size_t								db_populated_proxy_validation_test_n;

	// pulls proxy validation tests by provided criteria.  null criteria is an empty (blanket search)
	BP_ERROR_T getProxyDBValidationTestSetsByCriteria
	(
			size_t id,
			size_t user_id,
			size_t group_id,
			char *url_fetch,
			char *post_args,
			char *regexp_match,
			size_t count_invoked,
			char *created,
			size_t bottom_limit = 0,
			size_t top_limit = 1024
	);


	// ====== Update(s) Proxy DB Entries ==========================

	// Update a proxy db entry in the database.  Requires a valid proxy db entry item.
	BP_ERROR_T updateProxyDBEntryInDB(P_BP_PROXY_DB_ENTRY record_to_update);

	// Updates all the proxy db entries set in the class.
	BP_ERROR_T updateProxyDBEntriesInDB(bool only_update_runtime_validated = false);


	// ======== Attempts to Validate ProxyDB Records ==========================================

	// Attempt to validate a proxydb entry (requires valid httpSession set in class to work).
	BP_ERROR_T validateProxyDBEntry(P_BP_PROXY_DB_ENTRY entry, P_BP_PROXY_DB_VALIDATION_TEST_ENTRY validation_test, bool brute_force_proxy_type = true);

	// Attempt to validate all proxies in the current proxy set.  This action will modify data items within
	// the current proxy sets to reflect validation status (aka it will toggle enabled/disabled fields).  After
	// this method completes, it's ok to save/update the records in the db.
	BP_ERROR_T validateProxyDBEntries(size_t tests_to_try_before_failure_n, bool brute_force_proxy_type, bool use_threads = false, size_t thread_n = 1, size_t timeout_in_seconds = 5);

	// ======== Attach Proxies to Http Session (required before autoproxying can be done) =====

	// use NULL as a parameter to attach the records to the current http session
	BP_ERROR_T attachProxyDBRecordsToHttpSession(P_BP_HTTP_SESSION session);

	// =======  Destroy Data ==========================

	// destroys proxy related data
	BP_ERROR_T destroyProxyDBRelatedData();

	// ======== Display Routines ===================

	// display proxy record
	BP_ERROR_T displayProxyDBEntryRecord(P_BP_PROXY_DB_ENTRY record, bool only_display_runtime_validated = false);

	// display current proxy records stored in class
	BP_ERROR_T displayProxyDBEntryRecords(bool only_display_runtime_validated = false);

	// display proxy group record
	BP_ERROR_T displayProxyDBGroupRecord(P_BP_PROXY_DB_GROUP record);

	// display current proxy group records stored in class
	BP_ERROR_T displayProxyDBGroupRecords();

	// display a proxy validation group record
	BP_ERROR_T displayProxyDBValidationGroupRecord(P_BP_PROXY_DB_VALIDATION_TEST_GROUP record);

	// display current proxy group records stored in class
	BP_ERROR_T displayProxyDBValidationGroupRecords();

	// display a proxy validation test entry
	BP_ERROR_T displayProxyDBValidationTestEntryRecord(P_BP_PROXY_DB_VALIDATION_TEST_ENTRY record);

	// display current proxy validation tests stored in class.
	BP_ERROR_T displayProxyDBValidationTestEntryRecords();


#endif // BP_BLOCKCLASS_INCLUDE_PROXY_DB_BINDINGS


#ifdef BP_BLOCKCLASS_INCLUDE_WEB_ASSESSMENT_BINDINGS

	// Web assessment structure used for running appropriate
	// assessments.
	P_BP_WEB_ASSESSMENT webAssessment;


	// Initializes the web assessment structure with default values
	int CreateWebAssessment(char *targetURL, bool siteLock);

	// Creates a web assessment with session/html pointers from the outside
	int CreateWebAssessment(char *targetURL, P_BP_HTTP_SESSION httpSession, P_BP_HTML_FULL_EXTRACTION htmlExtraction, bool siteLock);

	// Destroys the current webassessment
	int DestroyWebAssessment(bool destroySession, bool destroyExtraction, bool destroyResults);

	// Runs the web assessment (should always use local links only unless
	// you want to go to jail.)
	int RunWebAssessment();



	// --- Command Injection Tests -------------------

	// Command line injection assessment (added 2014)
	P_BP_WEB_COMMAND_INJECTION_TEST_SESSION cmd_injection_assessment;

	// run command injection session
	BP_ERROR_T CreateCommandInjectionSession();

	// runs the command injection session
	BP_ERROR_T RunCommandInjectionSession(size_t thread_count);

	// destroy the command injection session
	BP_ERROR_T DestroyCommandInjectionSession();

	// saves the command injection results to the db (requires a
	// valid database connection).
	BP_ERROR_T SaveCommandInjectionResultsToDB();

	// Attempts to display the command injection session
	BP_ERROR_T DisplayCommandInjectionSession
	(
		BP_BOOL display_cmd_injection_test_set = BP_TRUE,
		BP_BOOL display_anchor_tests           = BP_TRUE,
		BP_BOOL display_form_tests             = BP_TRUE,
		BP_BOOL display_test_elements          = BP_TRUE
	);

	// starts a control shell for the provided injection session
	BP_ERROR_T ControlInteractiveCommandInjectionSession();



	// --- Log Assessment Results in Database --------

	// Saves the webassessment to the database.  See below for required create table syntax.
	int SaveWebAssessment(size_t result_id);


	// Saves the SQL error message data to the database.
	BP_ERROR_T WebAssessmentSaveSQLErrorMessageData();

	// Saves the XSS data to the database
	BP_ERROR_T WebAssessmentSaveXSSData();

	// saves app engine data to the database
	BP_ERROR_T WebAssessmentSaveAppEngineData();


	// Adds an entry to the engine_analysis_data table.
	BP_ERROR_T WebAssessmentAddEngineAnalysisDataEntry
	(
		size_t                      result_id,
		size_t                      request_id,
		size_t                      http_status_code,
		char *                      http_get,
		char *                      http_post,
		char *                      http_cookie,
		char *                      http_header,
		char *                      suspected_get_var,
		char *                      suspected_post_var,
		char *                      suspected_cookie_var,
		char *                      suspected_header_var,
		BP_WEB_ANALYSIS_ENGINE_T    detection_engine,
		BP_WEB_ANALYSIS_CONDITION_T condition,
		char *                      detection_string_test,
		char *                      detection_string_found,
		size_t                      detection_table_index
	);


	// --------- Assessment Initializers -------------

	// Set All Checks (for simplicity): set useDummy flag to true to use dummy tests
	int SetAllWebAssessmentChecksDefault(bool useDummy);

	// Set SQL Checks Only
	int SetSQLINJChecks(P_BP_WEB_SQLINJ_TEST sqlChecks);

	// Load SQLi checks from a file.
	int LoadSQLINJChecksFromFile(char *sqlinj_file);

	// Load XSS checks from a file.
	int LoadXSSChecksFromFile(char *xss_file);

	// Utility: Display all web assessment check lists. (sqlinj/xss/etc)
	int DisplayWebAssessementChecks();

	// Set XSS Checks Only
	int SetXSSChecks(P_BP_WEB_XSS_TEST xssChecks);

	// --------- Web Assessment Utilities -----------------

	// Prints the local web assessment
	int PrintWebAssessment();

#endif // BP_BLOCKCLASS_INCLUDE_WEB_ASSESSMENT_BINDINGS

#ifdef BP_BLOCKCLASS_INCLUDE_PCAP_GENERATION_BINDINGS

	// %%%%%%%%%%%%% PCAP Bindings %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// the filter expression used with pcap
	char *pcapFilterExpression;

	// the device to listen on

#endif // BP_BLOCKCLASS_INCLUDE_PCAP_GENERATION_BINDINGS

#ifdef BP_BLOCKCLASS_INCLUDE_MYSQL_DB_BINDINGS
#ifdef BP_BLOCKCLASS_INCLUDE_NMAP_SQL_DB_BINDINGS

	// %%%%%%%%%%%%%% NMAP DB Bindings %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// All functions below require class to have an active mysql connection.

	// portstat search items
	BP_NMAPDB_PORTSTATS portstats_min_s;
	BP_NMAPDB_PORTSTATS portstats_max_s;
	BP_NMAPDB_PORTSTATS portstats_like_s;
	BP_NMAPDB_PORTSTATS portstats_equal_s;

	// hoststats search items
	BP_NMAPDB_HOSTSTATS hoststats_min_s;
	BP_NMAPDB_HOSTSTATS hoststats_max_s;
	BP_NMAPDB_HOSTSTATS hoststats_like_s;
	BP_NMAPDB_HOSTSTATS hoststats_equal_s;

	// target search items
	BP_NMAPDB_TARGETS targets_min_s;
	BP_NMAPDB_TARGETS targets_max_s;
	BP_NMAPDB_TARGETS targets_like_s;
	BP_NMAPDB_TARGETS targets_equal_s;

	// scanner search items
	BP_NMAPDB_SCANNERS scanners_min_s;
	BP_NMAPDB_SCANNERS scanners_max_s;
	BP_NMAPDB_SCANNERS scanners_like_s;
	BP_NMAPDB_SCANNERS scanners_equal_s;

	// runlist search items
	BP_NMAPDB_RUNLIST runlist_min_s;
	BP_NMAPDB_RUNLIST runlist_max_s;
	BP_NMAPDB_RUNLIST runlist_like_s;
	BP_NMAPDB_RUNLIST runlist_equal_s;

	// top of the limit clause used to generate results
	size_t limitTop;

	// bottom of the limit clause used to generate results
	size_t limitBottom;

	// results of last query
	P_BP_NMAPDB_SEARCH_RESULTS nmapdb_results;

	// reset search parameters
	void resetNmapDbSearchParams();

	// uses the parameters above to generate queries and search the database
	size_t searchNmapDb();

	// displays results of search in a human readable format
	size_t displayNmapDBSearchResults(bool displayQuery);

	// retrieves a null pointer terminated list of ips from
	// a given result set.  IPs correlate to the results ip
	// member data.
	char ** retrieveIPsFromNmapDBResult(P_BP_NMAPDB_RESULTS result, bool append_ports);

#endif // BP_BLOCKCLASS_INCLUDE_NMAP_SQL_DB_BINDINGS
#endif // BP_BLOCKCLASS_INCLUDE_MYSQL_DB_BINDINGS

#ifdef BP_BLOCKCLASS_INCLUDE_COMMAND_CONTROL_INTERFACE

	/*
	 * Blockparty "Command and Control" Interface:
	 * -----------------------------------------------
	 * Used to supply command and control data to a pair of filedescriptors.  What this
	 * means is that the filedescriptors are mapped either from an open process or
	 * an open socket etc to read and write positions in a FD_SET structure. From there
	 * you can supply data automatically to the mapped filedescriptors by a manner of
	 * configured yes/no tests.
	 */

	// holds class session data for cc session
	P_BP_CC_SESSION cc_session;

	// creates a new session and sets it in the cc_session class variable
	size_t ccNewSession(P_BP_FDSET fds, char * session_name, BP_CC_SHELL_INTERFACE shell_interface);

	// displays an existing session data
	size_t ccDisplaySession();

	// adds a command to the session
	size_t ccAddCommand(char *command_name);

	// adds a try/except clause to the end of a branch (posix regexp on except)
	size_t ccAddTryExcept
	(
			char                   *command_name,
			char                   *try_this,
			size_t                  try_length,
			char                   *expect_this,
			size_t                  expect_length,
			BP_CC_MATCH_BUFF_POLICY expect_match_policy = BP_CC_MATCH_BUFF_POLICY_MEM_SEARCH
	);

	// execute a command with a given argument substitution stack
	size_t ccExecuteCommand(char *command_name, char ** arg_stack);

	// Adds a premature exit pcre to the command.  This allows us to monitor output for certain strings
	// and exit upon instance.
	BP_ERROR_T ccAddPrematureExitPCRE(char *exit_expression);

	// Adds an expression which will be used to match command output to specific data.
	BP_ERROR_T ccAddInformationExtractionPCRE(char *info_expression);

	// see if a command exists (return if exists)
	P_BP_CC_DECISION_TREE ccCommandExists(char *command_name);

#endif // BP_BLOCKCLASS_INCLUDE_COMMAND_CONTROL_INTERFACE

#ifdef BP_BLOCKCLASS_INCLUDE_GENERAL_UTILITIES


	// Generates a SHA1 has provided given input. Bin data is 20bytes non-terminated.
	char *sha1_ascii;
	bool GenSHA1(char *in, size_t in_size);

	// relies on FileGetContents(), generates an ascii sha1 from a file
	bool GenSHA1FromFile(char *filename);

#endif // BP_BLOCKCLASS_INCLUDE_GENERAL_UTILITIES

#ifdef BP_BLOCKCLASS_INCLUDE_USER_DB

	/*
	 * Blockparty User Database Methods:
	 *
	 * The methods below can be used to quickly authenticate, or otherwise
	 * modify users within the user table of blockparty/grief.
	 *
	 * Obviously, to use these methods, a valid mysqlSession is required.
	 *
	 */

	// holds user info if one was able to be looked up (null if unset)
	P_BP_USER user_info;

	// Returns err success if a user was able to be retrieved via lookup.  User
	// information is at that point stored in this->user_info.
	BP_ERROR_T authUserInDB(char * username, char *password);

	// Destroys the authenticated user data stored within this->user_info.  Pointer
	// is nullified after it's destroyed.
	BP_ERROR_T destroyAuthUserData();

	// display user info (if available)
	BP_ERROR_T displayAuthUserData();

#endif // BP_BLOCKCLASS_INCLUDE_USER_DB

#ifdef BP_BLOCKCLASS_RESULT_DB

	/*
	 *
	 * About web results:
	 * Web results are pulled from the `results` table within the blockparty/grief
	 * database.  Below, there are two sets of methods.  One set of method pulls singular
	 * entries from the db, and sets them in the local vars:
	 *
	 * 		this->web_result_href;
	 * 		this->web_result_id;
	 *
	 * 	The second set of methods pulls data in a multi-data
	 * 	way, allowing for multiple records to be pulled at
	 * 	once which greatly reduces load on the database.  The
	 * 	methods:
	 *
	 * 		this->webResultStackPush()
	 * 			and
	 * 		this->webResultStackPop()
	 *
	 * 	can be used to manipulate the class members such that the
	 * 	data can be iteratively moved in the following fashion:
	 *
	 * 	this->web_result_href = popped stack top entry
	 * 	this->web_result_id   = popped stack top db id
	 *
	 * 	Each set of routines comes complete with their own insert
	 * 	routines, which should be used where applicable.  Do note that
	 * 	the multi-data version is MUCH MUCH BETTER performance wise
	 * 	in regards to the db, but is LESS RESPONSIVE due to less database
	 * 	checks being made.  You have to find your own balance
	 * 	between the both of these in order to create a working
	 * 	operational configuration.
	 *
	 */

	// this is filled with a href (url) for the last retrieved/popped record.
	char web_result_href[4096];

	// this is filled with the db index for the last retrieved record
	// and is required for update/insert.
	size_t web_result_id;

	// if this flag is set to a non-zero value, the
	// checkResultExists function is forced to always
	// return ERR_SUCCESS.  The reason for using this flag
	// is in the case your db has a duplicate key check aka sha1(val)
	// which prevents database insert contention issues.  This is
	// highly recommended to be set if you're using result
	// stacks.  If you're using our schema, this value should
	// always be enabled.
	size_t disable_result_lookups_due_to_duplicate_key_contention_fix;


	// use this method to display web information tailored to your liking.
	BP_ERROR_T displayBlockWebConfiguration();

	// ============ Multi Result Retrieval Routines ===============


	// stack of web result data retrieved from db (one pointer, one
	// allocation, only needs to ever be free'd once.  Destroy savely
	// by calling the destroy method below.
	P_BP_WEB_RESULT_ENTRY web_result_stack;

	// number of entries in the web result stack (cleared/updated
	// on pop/destroy)
	size_t web_result_stack_n;

	// this was added in 2016 to add tracked dynamic data due to the new auto-size result buffering in mysql in blockparty.
	P_BP_LINKL web_result_stack_data_allocator;

	// current/next index of the web result stored in the web_result_href pointer.
	size_t web_result_stack_current_index;
	size_t web_result_stack_next_index;

	// flag indicating whether or not the stack needs to be reset
	size_t web_result_stack_all_popped;

	// parameters used when popping results.  Set after the first fetch.
	P_BP_WEB_RESULT_STACK_LOOKUP_PARMS web_result_lookup_parms;


	// DONT USE THIS TO FETCH:
	// Attempts to create the local stored lookups (the function parameters here will always
	// match the parameters for fetchWebResultStackFromDB, as it is called from within fetchWebResultStackFromDB
	// directly.
	BP_ERROR_T createWebResultStoredLookups
	(

		_vin size_t lock_for_audit,
		_vin size_t lock_for_spidering,
		_vin size_t lock_for_resolution_update,
		_vdbin size_t id,                        /* id = conditional */
		_vdbin size_t assessment_id,             /* assessment_id = conditional */
		_vdbin size_t type,                      /* type = conditional */
		_vdbin char * sha1,                      /* sha1 = str_conditional */
		_vdbin char * href_literal,              /* literal href match (can be null) */
		_vdbin char * href_like,                 /* LIKE href match condition (can be null) */
		_vdbin char * href_regexp,               /* REGEXP href match condition (can be null) */
		_vdbin size_t host,              		 /* associated host id */
		_vdbin size_t host_distinct,             /* wraps select host in a DISTINCT() clause */
		_vdbin size_t base,              		 /* associated base id */
		_vdbin size_t base_distinct,             /* wraps select base in a DISTINCT() clause */
		_vdbin size_t path,             		 /* associated path id */
		_vdbin size_t path_distinct,             /* wraps select base in a DISTINCT() clause */
		_vdbin size_t script,            		 /* associated script id */
		_vdbin size_t script_distinct,           /* wraps select base in a DISTINCT() clause */
		_vdbin size_t result_is_host,            /* joins on the result_hosts table, returning a host */
		_vdbin size_t result_is_base,            /* joins on the result_base_uris table, returning a base instead of a the value in the results table */
		_vdbin size_t result_is_path,            /* joins on the path table, returning a path instead of a the value in the results table */
		_vdbin size_t result_is_script,          /* joins on the script table, returning a script instead of a the value in the results table */
		_vdbin size_t status,                    /* general audit status condition */
		_vdbin size_t update_resolution_status,  /* when a record is being resolved, this flag locks the record */
		_vdbin size_t origin_type_form_or_anchor_code, /* hard code from blockparty indicating what type of anchor/form or whatever combination this was built from */
		_vdbin char * commit_ts_ge,              /* commit timestame greater equal */
		_vdbin char * commit_ts_le,              /* commit_timestamp less than equal to */
		_vdbin char * audit_start_ts_ge,         /* audit start greater equal */
		_vdbin char * audit_start_ts_le,         /* audit start less than equal */
		_vdbin char * audit_completed_ts_ge,     /* audit completed greater equal */
		_vdbin char * audit_completed_ts_le,     /* audit completed less than equal */
		_vdbin size_t spidered,                  /* spidered status */
		_vdbin size_t spidered_parent_result_id, /* parent id of entry */
		_vdbin char * spidered_ts_ge,            /* spidered ts greater equal */
		_vdbin char * spidered_ts_le ,           /* spidered ts less equal */
		_vdbin size_t hits,                      /* literal hit count (-1 to disable)*/
		_vdbin size_t hits_ge,                   /* hits greater than or equal this value */
		_vdbin size_t hits_le,                   /* hits less than or equal this value */
		_vdbin size_t disabled,                  /* disabled match */
		_vdbin size_t disable_code,              /* code indicating why the record was disabled */
		_vdbin size_t disabled_path,             /* disabled path match */
		_vdbin size_t disabled_path_code,        /* disabled path code match */
		_vdbin size_t disabled_host,             /* disabled host match */
		_vdbin size_t disabled_host_code,        /* disabled_host_code */
		_vdbin size_t disabled_base,             /* disabled base match */
		_vdbin size_t disabled_base_code,        /* disabled_base_code */
		_vdbin size_t disabled_script,           /* disabled script match */
		_vdbin size_t disabled_script_code,      /* disabled script code */
		_vdbin size_t limit_bottom,              /* limit bottom, top */
		_vdbin size_t limit_top,                 /* limit bottom, top */
		_vin   size_t randomize_select_ids,      /* set to non-zero to return random ids */
		_vdbin char * join_injection_sql,        /* this is direct SQL suitable for the JOIN position (dangerous) */
		_vdbin char * where_injection_sql        /* direct sql in the WHERE position.  be careful */

	);

	// destroy the web result lookup parameters
	BP_ERROR_T destroyWebResultLookupParameters();

	// destroys the web result stack (if it exists)
	BP_ERROR_T destroyWebResultStack();

	// pop an element on the web result stack  (uses index)
	BP_ERROR_T popWebResultWebStack();

	//  TODO: push an element on the web result stack
	BP_ERROR_T pushWebResultWebStack( P_BP_WEB_RESULT_ENTRY entry);

	// check whether or not a record should be disabled.  As a note
	// it's important to know that this is called directly from popWebResultWebStack
	// automatically, this is to streamline the process of getting non-disabled
	// entries.   These were not put in their own structure so as to make it
	// more obvious (for developers who are not me) as to how result JIT disabling
	// is done.  (Delayed result disabling is done in the libgrief result cache.
	// This is the JIT implementation of this code.  It's important to have a JIT version
	// because the resolver could potentially have faulty intelligence if it falls behind
	// in resolution scope for whatever reason.)

	// these parameters are thresholds for the result_*_uris tables in the database
	// and reflect the `hits` column in a >= match semantic.
	size_t web_result_disable_script_threshold;
	size_t web_result_disable_path_threshold;
	size_t web_result_disable_base_threshold;

	// these parameters are thresholds for the result_*_uris tables in the database
	// and reflect the `spidered_count` column in a >= match semantic.
	size_t web_result_disable_script_spidered_threshold;
	size_t web_result_disable_path_spidered_threshold;
	size_t web_result_disable_base_spidered_threshold;

	// these parameters are thresholds for the result_*_uris tables in the database
	// and reflect the `audited_count` column in a >= match semantic.
	size_t web_result_disable_script_audited_threshold;
	size_t web_result_disable_path_audited_threshold;
	size_t web_result_disable_base_audited_threshold;

	// run the actual check
	BP_ERROR_T checkWebResultEntryShouldBeDisabled(P_BP_WEB_RESULT_ENTRY entry);

	// create the default webstack parameters
	BP_ERROR_T createDefaultWebResultStackParams();

	// update the web result stack with new values.  Results are pulled
	// by looking into this->web_result_lookup_parms and running lookups
	// from there.
	BP_ERROR_T freshenWebResultStackFromParams();

	// pulls web results from db based on parameters and
	// stores them in the results stack above.
	BP_ERROR_T fetchWebResultStackFromDB
	(
			_vin size_t lock_for_audit,
			_vin size_t lock_for_spidering,
			_vin size_t lock_for_resolution_update,
			_vdbin size_t id,                        /* id = conditional */
			_vdbin size_t assessment_id,             /* assessment_id = conditional */
			_vdbin size_t type,                      /* type = conditional */
			_vdbin char * sha1,                      /* sha1 = str_conditional */
			_vdbin char * href_literal,              /* literal href match (can be null) */
			_vdbin char * href_like,                 /* LIKE href match condition (can be null) */
			_vdbin char * href_regexp,               /* REGEXP href match condition (can be null) */
			_vdbin size_t host,              		 /* associated host id */
			_vdbin size_t host_distinct,             /* wraps select host in a DISTINCT() clause */
			_vdbin size_t base,              		 /* associated base id */
			_vdbin size_t base_distinct,             /* wraps select base in a DISTINCT() clause */
			_vdbin size_t path,             		 /* associated path id */
			_vdbin size_t path_distinct,             /* wraps select base in a DISTINCT() clause */
			_vdbin size_t script,            		 /* associated script id */
			_vdbin size_t script_distinct,           /* wraps select base in a DISTINCT() clause */
			_vdbin size_t result_is_host,            /* joins on the result_hosts table, returning a host */
			_vdbin size_t result_is_base,            /* joins on the result_base_uris table, returning a base instead of a the value in the results table */
			_vdbin size_t result_is_path,            /* joins on the path table, returning a path instead of a the value in the results table */
			_vdbin size_t result_is_script,          /* joins on the script table, returning a script instead of a the value in the results table */
			_vdbin size_t status,                    /* general audit status condition */
			_vdbin size_t update_resolution_status,  /* when a record is being resolved, this flag locks the record */
			_vdbin size_t origin_type_form_or_anchor_code, /* hard code from blockparty indicating what type of anchor/form or whatever combination this was built from */
			_vdbin char * commit_ts_ge,              /* commit timestame greater equal */
			_vdbin char * commit_ts_le,              /* commit_timestamp less than equal to */
			_vdbin char * audit_start_ts_ge,         /* audit start greater equal */
			_vdbin char * audit_start_ts_le,         /* audit start less than equal */
			_vdbin char * audit_completed_ts_ge,     /* audit completed greater equal */
			_vdbin char * audit_completed_ts_le,     /* audit completed less than equal */
			_vdbin size_t spidered,                  /* spidered status */
			_vdbin size_t spidered_parent_result_id, /* parent id of entry */
			_vdbin char * spidered_ts_ge,            /* spidered ts greater equal */
			_vdbin char * spidered_ts_le ,           /* spidered ts less equal */
			_vdbin size_t hits,                      /* literal hit count (-1 to disable)*/
			_vdbin size_t hits_ge,                   /* hits greater than or equal this value */
			_vdbin size_t hits_le,                   /* hits less than or equal this value */
			_vdbin size_t disabled,                  /* disabled match */
			_vdbin size_t disable_code,              /* code indicating why the record was disabled */
			_vdbin size_t disabled_path,             /* disabled path match */
			_vdbin size_t disabled_path_code,        /* disabled path code match */
			_vdbin size_t disabled_host,             /* disabled host match */
			_vdbin size_t disabled_host_code,        /* disabled_host_code */
			_vdbin size_t disabled_base,             /* disabled base match */
			_vdbin size_t disabled_base_code,        /* disabled_base_code */
			_vdbin size_t disabled_script,           /* disabled script match */
			_vdbin size_t disabled_script_code,      /* disabled script code */
			_vdbin size_t limit_bottom,              /* limit bottom, top */
			_vdbin size_t limit_top,                 /* limit bottom, top */
			_vin   size_t randomize_select_ids,      /* set to non-zero to return random ids */
			_vdbin char * join_injection_sql,        /* this is direct SQL suitable for the JOIN position (dangerous) */
			_vdbin char * where_injection_sql        /* direct sql in the WHERE position.  be careful */
	);


	// this will disable web result stack entries which have a configuration that matches the configured thresholds
	// (query is very fast due to multiple indexing).
	BP_ERROR_T disableWebResultStackRecords(size_t base_hit_threshold, size_t path_hit_threshold, size_t script_hit_threshold);

	// update status flags for entire stack (int) -1 for unset
	BP_ERROR_T updateStatusFlagsForStack(_vdbin size_t status  = -1, _vdbin size_t spidered =  -1, _vdbin size_t lock_for_update_resolution = -1);

	// displays the web result stack in order
	BP_ERROR_T displayWebResultStack();


	// === Simple Result Stack Pruning (to avoid group-by clauses and save db resources) =====

	// Examines an existing web result stack and filters results down to distinct hosts.  This is similar to
	// a distinct clause that groups on the result_hosts table.
	BP_ERROR_T filterWebResultStackDownToDistinctHosts();

	// Examines an existing web result stack and filters results down to distinct hosts.  This is similar to
	// a distinct clause that groups on the result_bases table.
	BP_ERROR_T filterWebResultStackDownToDistinctBases();

	// Examines an existing web result stack and filters results down to distinct paths.  This is similar to
	// a distinct clause that groups on the result_paths table.
	BP_ERROR_T filterWebResultStackDownToDistinctPaths();

	// Examines an existing web result stack and filters results down to distinct scripts.  This is similar to
	// a distinct clause that groups on the result_scripts table.
	BP_ERROR_T filterWebResultStackDownToDistinctScripts();



	// === Simple Data Generators for Result Stack ======


	// `results`.`id` IN ( d,a,t,a )
	char * results_in_id_clause;
	size_t id_n;

	// `results`.`assessment_id` IN ( d,a,t,a )
	char * results_in_assessment_id_clause;
	size_t assessment_id_n;

	// `results`.`host` IN ( d,a,t,a )
	char * results_in_host_clause;
	size_t host_n;

	// `results`.`base` IN ( d,a,t,a )
	char * results_in_base_clause;
	size_t base_n;

	// `results`.`path` IN (d,a,t,a)
	char * results_in_path_clause;
	size_t path_n;

	// `results`.`script` IN (d,a,t,a)
	char * results_in_script_clause;
	size_t script_n;


	// Generates constraints suitable for use in queries (where clause)
	BP_ERROR_T webResultStackWhereClauses();

	// Destroy the where clauses if they exist
	BP_ERROR_T destroyWebResultStackWhereClauses();



	// ============ Path Retrieval Functionality ==================

	// Web paths are used for attempting to verify positions, and extend
	// recon to a variety of levels.  Most notably, brute forcing.

	// holds web paths retrieved (corresponds to the result_path_uris table in the grief3 db)
	P_BP_WEB_PATH_ENTRY web_path_stack;
	size_t              web_path_stack_n;

	// Fetch a path stack from the db.
	BP_ERROR_T fetchPathStack
	(
			_vin size_t lock_for_brute_force,
			_vin char * sha1,
			_vin char * path_href,
			_vin char * path_href_like,
			_vin char * path_href_regexp,
			_vin size_t hits,
			_vin size_t initial_result_id,
			_vin char * created_ts,
			_vin size_t disabled,
			_vin size_t disabled_code,
			_vin size_t brute_status
	);

	// ============ Single Result Retrieval Routines ==============

	// pulls a result index into a size_t, and does it quickly.  Works
	// off results.id.
	size_t retrieveRandomResultDBIndexFastFind(_vdbin size_t spidered_status = -1, _vdbin size_t audit_status = -1);

	// Retrieve a random target from the db (where clause is an INJECTION, be careful with
	// using it, its here to be powerful, not abused).
	BP_ERROR_T retrieveResultFromDb(char *where_clause);

	// used for setting results directly from result entries
	BP_ERROR_T setResultFromWebResultEntry(P_BP_WEB_RESULT_ENTRY web_result_entry);

	// Retrieve a random target from the DB, with href limitation only (NO INJECTION,
	// prepared statement!)  Use this if you do not trust input.  Query is href LIKE ?.
	// retrieve order = 0 for sort by id, 1 for sort rand()
	BP_ERROR_T retrieveResultFromDbHrefLike
	(
			char *like,
			bool unspidered,
			bool unaudited,
			size_t retrieve_order = 0
	);

	// Sets the update resolution flag of the last record. (used by the database resolving engine)
	BP_ERROR_T setLastResultUpdateResolutionStatus(size_t status_code);

	// Sets the spidered flag of the last retrieved record
	BP_ERROR_T setLastResultSpideredStatus(size_t status_code);

	// Sets the status of the last retrieved record
	BP_ERROR_T setLastResultStatus(size_t status_code);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Single Result Insert Methods %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// these flags are default set to true, and prevent the sensor
	// from inserting base or path information.  The reason for this
	// is that both the path and base are inferrable from the full href,
	// and to save on insert requests/request size we can simply have those values parsed
	// and filled out within the mysql db event scheduling process itself.  Only
	// enable these flags if you have the required stored procedures/events installed
	// aka sp_findMissingBasesAndPathsForResults();
	size_t base_insertion_is_handled_by_backend_db_event;
	size_t path_insertion_is_handled_by_backend_db_event;

	// Inserts a new audit target into the db (automatically checks for href)
	size_t insertNewResult(size_t type, char *href, size_t status_code, size_t parent_id);

	// Inserts anchor data into the db.
	size_t insertAnchor(P_BP_HTML_ANCHOR_LIST anchor, size_t result_id);

	// Inserts form data into the db.
	size_t insertForm(P_BP_HTML_FORM_LIST form, size_t result_id);

	// Check if a target exists (ERR_SUCCESS on exists)
	size_t checkResultExists(char *href, size_t status_code = (size_t)-1);

	// Insert an email into the discovered emails list.
	size_t insertDiscoveredEmail(char *email_address);

	// Insert a host into the db (linked to result in base field)
	size_t insertBaseHost(char * host_href, size_t result_id=0);

	// Insert a base_href into the db (linked to result in base field), requires
	// duplicate key index on sha1 field.
	size_t insertBaseHref(char * base_href, size_t result_id=0);

	// Insert a base_path into the db (linked to result in base field)
	size_t insertBasePath(char * base_path, size_t result_id=0);

	// Insert a base script into the db (linked to result in script field)
	size_t insertBaseScript(char *base_script, size_t result_id=0);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Single Result ID Lookup Routines %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// lookup result id
	size_t lookupResultId(char *result_href);

	// lookup host id
	size_t lookupBaseHostId(char *host);

	// lookup base id
	size_t lookupBaseHrefId(char *base);

	// lookup path id
	size_t lookupBasePathId(char *path);

	// lookup script ids
	size_t lookupBaseScriptId(char *script);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%% Insert Stacking %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Because the database is so large, it makes sense to
	// actually run inserts in batches rather than individually.
	// This is because when you have millions of records, you
	// will be spending most of your time doing lookups rather
	// than actually inserting data.
	//
	// How Does It Work:
	// =======================
	// It modifies the behavior of insertNewResult() so that
	// it writes to a stacked insert rather than inserts immediately.
	// Set the insert flush rate to the number of inserts you want to
	// stack before flushing.  Flushing is done either automatically
	// via a check in insertNewResult() or at developer discretion by calling
	// flushInsertRequests().
	//
	// The flag: enable_result_insert_stacking must be true for this functionality
	// to be enabled.


	// set this to true to enable stacking
	bool enable_result_insert_stacking;

	// flush rate to autoflush on
	size_t result_stacking_flush_rate;

	// stack of insert items (allocated dynamically)
	BP_StringUtils * result_stack_builder;

	// current number of items stacked in the dynamic query string
	size_t result_stack_n;

	// adds a stack item
	BP_ERROR_T stackInsertRequest(size_t type, char *href, size_t status_code, size_t parent_id);

	// flushes insert request
	BP_ERROR_T flushInsertRequests();



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%% Balanced Request Whitelisting %%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this is set in the case that we're using balancing
	P_BP_DB_WEB_REQUEST_BALANCER web_balancer;

	// add whitelist
	BP_ERROR_T addWebRequestBalance(char * whitelist, size_t weight);

	// test balancer with href.  If the request would be unbalanced based
	// on the whitelist configuration + requests already made, this will return
	// ERR_FAILURE, whereas if it is within balancing constraints it will return
	// ERR_SUCCESS.
	BP_ERROR_T checkWebHrefForBalance(char *href);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Spider Threading %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Because sometimes (most of the time) we'll want to do spidering in a
	// threaded fashion, we can use the methods below to clone spider
	// configurations from existing blocks.  The data which is cloned is actually
	// allocated as it would be if the Block had allocated them iself.  This
	// should allow for a delete to destroy data as per-usual operation.

	// clone spidering data into new block
	BP_ERROR_T cloneSpideringDataIntoBlock(Block * block_to_clone_from);

	// make request as spider
	// add to update stack

// add auto result generation from page parses that come from the assessment DB
#ifdef BP_BLOCKCLASS_INCLUDE_WEB_ASSESSMENT_BINDINGS

	// If this block class has an existing web assessment
	// attached to it in its local class pointer, that structure
	// will be parsed out in order to generate new (unique) targets
	// automatically.  IMPORTANT: THIS ESSENTIALLY CREATES A SPIDER
	// WHEN COUPLED WITH THE ASSESSMENT AUDITOR.

	// enable extended data tracking in the databse
	bool enable_form_data_logging_during_extraction;
	bool enable_anchor_data_logging_during_extraction;

	// spider/extracts data from html parse.
	BP_ERROR_T extractNewResultsFromHttpParse();

	// extracts results from preg url matching regular expression (uses retrieved page membuffer as search).  This
	// routine will use this->httpSession data if no buffer is passed in)
	BP_ERROR_T extractNewResultsFromPREGSearch(char *buff_search = 0, size_t buff_len = 0);

	// attempt to extract emails from memory buffer.This
	// routine will use this->httpSession data if no buffer is passed in)
	BP_ERROR_T extractEmailsFromPREGSearch(char *buff_search = 0, size_t buff_len = 0);


	// ===== Custom JIT Matching =================

	/* This allows JIT match sets to be applied to
	 * blockparty http sessions.  Will automatically
	 * run match sets after each extraction and proceed
	 * to automatically store them in the database.
	 */

	// just in time match set used (records must be pulled from
	// database or set manually; database is recommended)
	// this is an array when populated.
	P_BP_PCRE_JIT_MATCH_ENTRY web_request_jit_match_set;

	// count of the JIT matche tests in the above match array
	size_t web_request_jit_match_n;

	// retrieve jit expressions from database (if expressions already exist, they are destroyed
	// first.  This is functionally retrieve + update on loop.  Set elements to zero values to
	// omit them from search.  All zero values for ALL tests.
	BP_ERROR_T retrieveWebJITExpressionsFromDB(size_t db_id, size_t group_id, size_t user_id, size_t bottom_limit, size_t top_limit);

	// DONOTUSE::BROKEN::OLD attach regexp tests to http session (gathered from retrieveWebJITExpressionsFromDB above)
	BP_ERROR_T attachJITRegexpTestRecordsToHttpSession();

	// DONOTUSE::BROKEN::OLD if a jit check results in a match, you can use this to store the match in the db
	// Uses member data for propagation.
	BP_ERROR_T insertWebJITExpressionMatches();

	// This is called JIT at extractDataFromJITMatches below. if a jit
	// check results in a match, you can use this to store the match
	// in the db
	BP_ERROR_T insertWebJITExpressionMatch(size_t group_id, size_t test_id, size_t result_id, char * match_val, size_t match_len);

	// This is called JIT at extractDataFromJITMatches below. if a jit
	// check results in a match, you can use this to store the match
	// in the db
	char * jit_delayed_insert_sql;
	BP_ERROR_T insertWebJITExpressionMatchDelayed(size_t group_id, size_t test_id, size_t result_id, char * match_val, size_t match_len);

	// write delayed jit entries to the database
	BP_ERROR_T flushWebJITExpressionMatchDelayedSQL();

	// extract match data from JIT match set
	BP_ERROR_T extractDataFromJITMatches();

	// display all jit matches
	BP_ERROR_T displayAllJITMatches();

	// displays the current set of jit expressions
	BP_ERROR_T displayWebJITExpressions();

	// destroy all jit expression data related to this class
	BP_ERROR_T destroyWebJITExpressionData();

	// toggle to enable or disable match sets
	bool web_enable_jit_match_sets;


#endif


#ifdef BP_BLOCKCLASS_INCLUDE_GOOGLE_SCRAPING_INTERFACE

	/*
	 * Blockparty Google Scraping Interface:
	 * ------------------------------------------------
	 * Allows for the extraction and storage of google search results.  This
	 * requires a valid HTTP session to work.
	 *
	 */

	// flag to indicate behavior of google subsystem
	bool google_retry_on_too_small_size;

	// number of seconds between google page requests (used to stifle captcha)
	size_t secs_between_google_request;

	// set retry flag
	bool google_retry_on_bad_connect;

	// class google session (contains results and data)
	P_BP_GOOGLESCRAPE_SESSION google_session;

	// custom blacklist (null POINTER array terminated)
	char ** custom_google_blacklist;

	// runs a google search using the classes built in web engine
	BP_ERROR_T runGoogleSearch(char *query);

	// commit search results to database (requires valid mysql connection)
	/* Create Table Syntax: search results
			CREATE TABLE `results` (
 	 	 	 	 `index` int(11) NOT NULL AUTO_INCREMENT,
 	 	 	 	 `type` int(11) NOT NULL,
 	 	 	 	 `href` text NOT NULL,
 	 	 	 	 `status` int(11) NOT NULL,
 	 	 	 	 `commit_timestamp` datetime NOT NULL,
 	 	 	 	 PRIMARY KEY (`index`)
			) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=latin1
	*/

	// save class this->google_session results to database (requires above table installed on current database)
	BP_ERROR_T saveGoogleResults();

	// destroy google scraper session
	BP_ERROR_T destroyGoogleSession();

#endif//  BP_BLOCKCLASS_INCLUDE_GOOGLE_SCRAPING_INTERFACE

#ifdef BP_BLOCKCLASS_INCLUDE_DDG_SCRAPING_INTERFACE

	/*
	 * Blockparty Duck Duck Go - Onion Scraping Interface:
	 * ------------------------------------------------
	 * Allows for the extraction and storage of ddg search results.  This
	 * requires a valid HTTP session to work.
	 *
	 */

	// flag to indicate behavior of ddg subsystem
	bool ddg_retry_on_too_small_size;

	// number of seconds between ddg page requests (used to stifle captcha)
	size_t secs_between_ddg_request;

	// set retry flag
	bool ddg_retry_on_bad_connect;

	// class ddg session (contains results and data)
	P_BP_DDGSCRAPE_SESSION ddg_session;

	// custom blacklist (null POINTER array terminated)
	char ** custom_ddg_blacklist;

	// runs a ddg search using the classes built in web engine
	BP_ERROR_T runDdgSearch(char *query);

	// commit search results to database (requires valid mysql connection)
	/* Create Table Syntax: search results
			CREATE TABLE `results` (
 	 	 	 	 `index` int(11) NOT NULL AUTO_INCREMENT,
 	 	 	 	 `type` int(11) NOT NULL,
 	 	 	 	 `href` text NOT NULL,
 	 	 	 	 `status` int(11) NOT NULL,
 	 	 	 	 `commit_timestamp` datetime NOT NULL,
 	 	 	 	 PRIMARY KEY (`index`)
			) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=latin1
	*/

	// save class this->ddg_session results to database (requires above table installed on current database)
	BP_ERROR_T saveDdgResults();

	// destroy ddg scraper session
	BP_ERROR_T destroyDdgSession();

#endif//  BP_BLOCKCLASS_INCLUDE_DDG_SCRAPING_INTERFACE


#ifdef BP_BLOCKCLASS_INCLUDE_BING_SCRAPING_INTERFACE

	/*
	 * Blockparty Bing Scraping Interface:
	 * ------------------------------------------------
	 * Allows for the extraction and storage of bing search results.  This
	 * requires a valid HTTP session to work.
	 *
	 */

	// number of seconds between bing page requests (used to stifle captcha)
	size_t secs_between_bing_request;

	// class bing session (contains results and data)
	P_BP_BINGSCRAPE_SESSION bing_session;

	// custom blacklist (null POINTER array terminated)
	char ** custom_bing_blacklist;

	// runs a bing search using the classes built in web engine
	BP_ERROR_T runBingSearch(char *query);

	// commit search results to database (requires valid mysql connection)
	/* Create Table Syntax: search results
			CREATE TABLE `results` (
 	 	 	 	 `index` int(11) NOT NULL AUTO_INCREMENT,
 	 	 	 	 `type` int(11) NOT NULL,
 	 	 	 	 `href` text NOT NULL,
 	 	 	 	 `status` int(11) NOT NULL,
 	 	 	 	 `commit_timestamp` datetime NOT NULL,
 	 	 	 	 PRIMARY KEY (`index`)
			) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=latin1
	*/

	// save class this->bing_session results to database (requires above table installed on current database)
	BP_ERROR_T saveBingResults();

	// destroy bing scraper session
	BP_ERROR_T destroyBingSession();

#endif//  BP_BLOCKCLASS_INCLUDE_BING_SCRAPING_INTERFACE

#ifdef BP_BLOCKCLASS_INCLUDE_YAHOO_SCRAPING_INTERFACE

	/*
	 * Blockparty Yahoo Scraping Interface:
	 * ------------------------------------------------
	 * Allows for the extraction and storage of yahoo search results.  This
	 * requires a valid HTTP session to work.
	 *
	 */

	// number of seconds between yahoo page requests (used to stifle captcha)
	size_t secs_between_yahoo_request;

	// class yahoo session (contains results and data)
	P_BP_YAHOOSCRAPE_SESSION yahoo_session;

	// custom blacklist (null POINTER array terminated)
	char ** custom_yahoo_blacklist;

	// runs a yahoo search using the classes built in web engine
	BP_ERROR_T runYahooSearch(char *query);

	// save class this->yahoo_session results to database (requires above table installed on current database)
	BP_ERROR_T saveYahooResults();

	// destroy yahoo scraper session
	BP_ERROR_T destroyYahooSession();

#endif//  BP_BLOCKCLASS_INCLUDE_YAHOO_SCRAPING_INTERFACE


#ifdef BP_BLOCKCLASS_INCLUDE_DISCONNECTME_SCRAPING_INTERFACE

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Disconnect Me Search Options %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// disconnect me search/scrape session
	P_BP_DISCONNECTME_SCRAPE_SESSION disconnectme_session;



	// attempt to run disconnect me search against selected target
	BP_ERROR_T runDisconnectMeSearch(char *query, BP_DISCONNECT_ME_SEARCH_TARGET search_engine_target);

	// save the search results
	BP_ERROR_T saveDisconnectMeSearchResults();

	// destroy the disconnect me search session
	BP_ERROR_T destroyDisconnectMeSearch();

#endif


#endif // BP_BLOCKCLASS_RESULT_DB


	// BLOCK CLASS GARBAGE COLLECTION SUBSYSTEM (BUILT IN)
	P_BP_GARBAGE_COLLECTOR_DATA gc;

	// displays garbage collector brief for this class
	size_t GcDisplayBrief();

};

#endif // BP_CLASS_H

/*
 * Blockparty Main C/C++ Include File:
 * --------------------------------------------
 * Include this file whenever developing with Blockparty.
 *
 *  Document Author:  Jason A. Medeiros
 *  Document Created: 357 B.C.
 *
 */

#ifndef BP_MAIN_H
#define BP_MAIN_H
#define _GNU_SOURCE

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Non-Blockparty Headers / Third Party / Stdlib %%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Necessary standard header includes
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <ctype.h>
#include <cstddef>
#include <sys/uio.h>
#include <sys/ioctl.h> 
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/syscall.h>
#include <sys/ptrace.h>
#include <sys/prctl.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stropts.h>
#include <poll.h>
#include <malloc.h>
#include <execinfo.h>

// #include <asm/ptrace.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/user.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>

// dynamic libraries
#include <dlfcn.h>

// network headers
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <search.h>

// pseudoterminal
#include <pty.h>

// termios
#include <termios.h>

// posix / perlc compatible regular expression functions
#include <regex.h>
#include <pcre.h>

// ssl headers
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/sha.h>
#include <openssl/md5.h>

// Gnutls headers (need both ssl libraries).  This is mainly
// for gaining access to the gnutls headers needed for libmicrohttpd.
#include <gnutls/gnutls.h>

// libstatgrab headers
#include <statgrab.h>

// libxml html parser
#include <libxml/HTMLparser.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

// sqlite database include
#include <sqlite3.h>

// cURL Library Header (must be at least v7.20 to build)
#include <curl/curl.h>

// PCAP Headers
#include <pcap.h>

// include libmailutil headers (for email)
#include <mailutils/mailutils.h>

// include uchar types
#include <uchar.h>

// stdarg, for printf wrappers
#include <stdarg.h>

// include memory managment header
#include <sys/mman.h>

// include epoll header
#include <sys/epoll.h>


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% libmicrohttpd %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// include microhttpd
#include <microhttpd.h>

// include microhttpd sockets header
// #include <mhd_sockets.h>

// include unix password (for getting usernames and ids)
#include <pwd.h>

// include group (for getting groups and ids)
#include <grp.h>

// include file tree walking capability
#include <ftw.h>
#include <fts.h>



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Mozilla Spidermonkey JS Engine %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// include javascript jsapi (for spidermonkey)
#include <jsapi.h>



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Blockparty Build Versions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// These change between release.
#define BP_LIBRARY_MAJOR_VERSION 0
#define BP_LIBRARY_MINOR_VERSION 10
#define BP_LIBRARY_BUILD_ID      2602



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Build Date and Time For Library %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#ifdef __TIME__
#define BP_LIBRARY_COMPILED_BUILD_TIME __TIME__
#endif

#ifdef __DATE__
#define BP_LIBRARY_COMPILED_BUILD_DATE __DATE__
#endif

#ifndef BP_LIBRARY_COMPILED_BUILD_TIME
#define BP_LIBRARY_COMPILED_BUILD_TIME "Compiled Build Time Unset"
#endif

#ifndef BP_LIBRARY_COMPILED_BUILD_DATE
#define BP_LIBRARY_COMPILED_BUILD_DATE "Compiled Build Date Unset"
#endif


// If this is set to true, the library is set to debug configuration,
// which enables (among other things) BP_dbg_printf to display output.
#ifndef BP_LIBRARY_DEBUG_CONFIGURATION_ENABLED
#define BP_LIBRARY_DEBUG_CONFIGURATION_ENABLED 1
#endif


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Basic Memory Behavior %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Blockparty has 3 major interfaces where allocations take place.  These
// flags potentially affect all of them.  These flags affect how the
// library behaves when platform allocators fail.

// This will cause int3 (software breakpoint) to be triggered when any
// allocation fails (typically when out of memory)
#define BP_DEBUG_BREAKPOINT_ON_ALLOC_FAIL 1


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Simple Generic Type Definitions (non-standard) %%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// simple type definition for a single byte
#define BP_BYTE_T unsigned char;


// Simple generic buffer structure for holding buffers.  This is
// not used very often in the library, but instead is provided
// as a convience routine
typedef struct __BP_BUFFER_T {

	// data in this buffer
	void * data;

	// length of this data
	size_t len;

} BP_BUFFER_T, *P_BP_BUFFER_T;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%% Size Definitions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// simple true and false enumeration typedef.  This is
// not a true boolean typedef, as it includes an invalid
// code which can be used for marking boolean conditions
// as simply unset/invalid in the context of boolean
// comparisons.
typedef enum BP_BOOL {
	BP_FALSE,        /* 0 */
	BP_TRUE,         /* 1 */
	BP_INVALID_UNSET /* 2 */
};

// simple data sizes
#define BP_BYTE     1
#define BP_KILOBYTE 1024
#define BP_MEGABYTE 1048576
#define BP_GIGABYTE 1073741824

// top limit for any alloc (1gb)
#define MAXIMUM_VALUE_FOR_ANY_ALLOCATION 1073741824

// set max limits
#if __x86_64__

	#define BP_64BIT_UMAX 18446744073709551615
	#define BP_32BIT_UMAX 4294967295
	#define BP_32BIT_SMAX 2147483647

	// you can set the value for an allocation here
	#undef MAXIMUM_VALUE_FOR_ANY_ALLOCATION
	#define MAXIMUM_VALUE_FOR_ANY_ALLOCATION 1073741824

#else

	#define BP_64BIT_UMAX "18446744073709551615"
	#define BP_64BIT_UMAX_IS_STR 1
	#define BP_32BIT_UMAX 4294967295
	#define BP_32BIT_SMAX 2147483647

	// you can set the value for an allocation here
	#define MAXIMUM_VALUE_FOR_ANY_ALLOCATION (BP_GIGABYTE * 1)

#endif

// byte typedef
typedef unsigned char BYTE;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Simple Char Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#define BP_IS_UPPER_ASCII(ch) \
	((unsigned char) ch >= 65 && (unsigned char) ch <= 90) ? ERR_SUCCESS : ERR_FAILURE

#define BP_IS_LOWER_ASCII(ch) \
	((unsigned char) ch >= 97 && (unsigned char) ch <= 122) ? ERR_SUCCESS : ERR_FAILURE

#define BP_IS_DIGIT_ASCII(ch) \
	((unsigned char) ch >= 48 && (unsigned char) ch <= 57) ? ERR_SUCCESS : ERR_FAILURE

#define BP_IS_LOWER_HEX_ASCII(ch) \
	(                                                              \
		((unsigned char) ch >= 48 && (unsigned char) ch <= 57 ) || \
		((unsigned char) ch >= 97 && (unsigned char) ch <= 102)    \
	)                                                              \
		? ERR_SUCCESS : ERR_FAILURE

#define BP_IS_UPPER_HEX_ASCII(ch) \
	(                                                              \
		((unsigned char) ch >= 48 && (unsigned char) ch <= 57 ) || \
		((unsigned char) ch >= 65 && (unsigned char) ch <= 70)    \
	)                                                              \
		? ERR_SUCCESS : ERR_FAILURE

// if it's a digit, a lowercase ascii or upper ascii
#define BP_IS_VALID_ASCII_CHAR_OR_DIGIT_UPPERLOWER(ch) \
	(BP_IS_UPPER_ASCII(ch) || BP_IS_LOWER_ASCII(ch) || BP_IS_DIGIT_ASCII(ch) ) ? ERR_SUCCESS : ERR_FAILURE

// // if it's a digit or lowercase ascii
#define BP_IS_VALID_ASCII_CHAR_OR_DIGIT_LOWER(ch) \
		( BP_IS_LOWER_ASCII(ch) || BP_IS_DIGIT_ASCII(ch) ) ? ERR_SUCCESS : ERR_FAILURE


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% UUID Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// length of a uuid including dashes
#define BP_UUID_LEN 36



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Memory Clearing Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// zeros out any arbitrary value
#define BP_ZERO(ptr, size) ptr ? memset((void *) ptr, 0x00, (size_t) size) : 0

// zeros out any arbitrary stack structure (does not require size, as size is calculated
// via sizeof)
#define BP_ZERO_STACK(stack_value) memset((void *) &stack_value, 0x00, sizeof(stack_value))



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Terminal Color Codes %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Enumerated terminal foreground and background colors.  Macros for
// generating random colors can be found below the utility/*.h includes
// in this file (BP-Main.h).

typedef enum BP_FOREGROUND_COLOR_CODE_T {
	BP_COLOR_CODE_FG_NONE,
	BP_COLOR_CODE_FG_BLACK,
	BP_COLOR_CODE_FG_RED,
	BP_COLOR_CODE_FG_GREEN,
	BP_COLOR_CODE_FG_YELLOW,
	BP_COLOR_CODE_FG_BLUE,
	BP_COLOR_CODE_FG_MAGENTA,
	BP_COLOR_CODE_FG_CYAN,
	BP_COLOR_CODE_FG_WHITE
};

typedef enum BP_BACKGROUND_COLOR_CODE_T {
	BP_COLOR_CODE_BG_NONE,
	BP_COLOR_CODE_BG_BLACK,
	BP_COLOR_CODE_BG_RED,
	BP_COLOR_CODE_BG_GREEN,
	BP_COLOR_CODE_BG_BROWN,
	BP_COLOR_CODE_BG_BLUE,
	BP_COLOR_CODE_BG_MAGENTA,
	BP_COLOR_CODE_BG_CYAN,
	BP_COLOR_CODE_BG_WHITE
};

// color code structure, used for passing foregrounds and background to
// various routines.
typedef struct __BP_COLOR_CODE_SET_T {

	// foreground color
	BP_FOREGROUND_COLOR_CODE_T fg;

	// background color
	BP_BACKGROUND_COLOR_CODE_T bg;

} BP_COLOR_CODE_SET_T, *P_BP_COLOR_CODE_SET_T;


// string which resets terminal colors if displayed to the terminal
#define BP_COLOR_CODE_RESET_DEFAULT_COLORS "\x1b[0m"

// simple macro for printf to reset colors
#define BP_RESET_TERMINAL_COLOR printf("\x1b[0m")


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Benchmarking Macros (very fast/rudimentary) %%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// simple
typedef struct __BP_BENCHMARK_LOCAL {
	timespec ts_start;
	timespec ts_stop;
	timespec ts_diff;
} BP_BENCHMARK_LOCAL, *P_BP_BENCHMARK_LOCAL;

// initialize the locals
#define BP_BENCHMARK_INIT_LOCALS BP_BENCHMARK_LOCAL bp_benchmark_locals; memset(&bp_benchmark_locals, 0x00, sizeof(bp_benchmark_locals));

// start benchmark using a local var (local stackframe)
#define BP_BENCHMARK_LOCAL_START clock_gettime(CLOCK_REALTIME, &bp_benchmark_locals.ts_start)

// stop a benchmark using a local stackframe
#define BP_BENCHMARK_LOCAL_STOP  clock_gettime(CLOCK_REALTIME, &bp_benchmark_locals.ts_stop)

// calculate the elapsed time
#define BP_BENCHMARK_TIME_CALCULATE_ELAPSED \
	if\
	(\
			( bp_benchmark_locals.ts_stop.tv_nsec - bp_benchmark_locals.ts_start.tv_nsec ) < 0\
	)\
	{\
		bp_benchmark_locals.ts_diff.tv_sec  = bp_benchmark_locals.ts_stop.tv_sec - bp_benchmark_locals.ts_start.tv_sec - 1;\
		bp_benchmark_locals.ts_diff.tv_nsec = 1000000000                         + bp_benchmark_locals.ts_stop.tv_nsec - bp_benchmark_locals.ts_start.tv_nsec;\
	}\
	else\
	{\
		bp_benchmark_locals.ts_diff.tv_sec  = bp_benchmark_locals.ts_stop.tv_sec  - bp_benchmark_locals.ts_start.tv_sec;\
		bp_benchmark_locals.ts_diff.tv_nsec = bp_benchmark_locals.ts_stop.tv_nsec - bp_benchmark_locals.ts_start.tv_nsec;\
	}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Flag Position Definitions for Generic UINT32 Flags %%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// ABOUT: The following definitions and macros are used for working with
//        flag configurations that require less than 32 flag places.  The settrs
//        simply switches on or off bits within a uint32_t value, and a gettr
//        macro is provided for easily determining the setting of those individual
//        flags.  This code is VERY fast/memory efficient and should be used whenever
//        possible for dealing with option flags.


// typedef for working with 32bit flags
typedef uint32_t BP_FLAGS_T;

// Enumerator containing all speicific flag
// positions possible within a 32bit value.
typedef enum BP_UINT32_FLAG_POSITION
{
	BP_UINT32_FLAG_POSITION_1  = 1,
	BP_UINT32_FLAG_POSITION_2  = 2,
	BP_UINT32_FLAG_POSITION_3  = 4,
	BP_UINT32_FLAG_POSITION_4  = 8,
	BP_UINT32_FLAG_POSITION_5  = 16,
	BP_UINT32_FLAG_POSITION_6  = 32,
	BP_UINT32_FLAG_POSITION_7  = 64,
	BP_UINT32_FLAG_POSITION_8  = 128,
	BP_UINT32_FLAG_POSITION_9  = 256,
	BP_UINT32_FLAG_POSITION_10 = 512,
	BP_UINT32_FLAG_POSITION_11 = 1024,
	BP_UINT32_FLAG_POSITION_12 = 2048,
	BP_UINT32_FLAG_POSITION_13 = 4096,
	BP_UINT32_FLAG_POSITION_14 = 8192,
	BP_UINT32_FLAG_POSITION_15 = 16384,
	BP_UINT32_FLAG_POSITION_16 = 32768,
	BP_UINT32_FLAG_POSITION_17 = 65536,
	BP_UINT32_FLAG_POSITION_18 = 131072,
	BP_UINT32_FLAG_POSITION_19 = 262144,
	BP_UINT32_FLAG_POSITION_20 = 524288,
	BP_UINT32_FLAG_POSITION_21 = 1048576,
	BP_UINT32_FLAG_POSITION_22 = 2097152,
	BP_UINT32_FLAG_POSITION_23 = 4194304,
	BP_UINT32_FLAG_POSITION_24 = 8388608,
	BP_UINT32_FLAG_POSITION_25 = 16777216,
	BP_UINT32_FLAG_POSITION_26 = 33554432,
	BP_UINT32_FLAG_POSITION_27 = 67108864,
	BP_UINT32_FLAG_POSITION_28 = 134217728,
	BP_UINT32_FLAG_POSITION_29 = 268435456,
	BP_UINT32_FLAG_POSITION_30 = 536870912,
	BP_UINT32_FLAG_POSITION_31 = 1073741824,
	BP_UINT32_FLAG_POSITION_32 = 2147483648
};

// Simple macro to set a value at a position.
#define BP_SET_FLAG(value, flag_position) value |= 1 << (flag_position-1)

// Simple macro to unset a value at a position
#define BP_UNSET_FLAG(value, flag_position) value &= ~(1 << (flag_position-1))

// Simple macro to xor toggle a value at a position
#define BP_TOGGLE_FLAG(value, flag_position) value ^= 1 << (flag_position-1)

// this returns 1 or 0 depending on if the flag is on (1) or off (0)
#define BP_FLAG_IS_SET(value, flag_position) value & (1 << (flag_position-1))

// this unsets all flags (zeros value)
#define BP_UNSET_ALL_FLAGS(value) value = 0

/*
 Usage Example: Inlined here for quick reference, a compilable example is available
                within the library unit tests.

	// declare a set of flags
	BP_FLAGS_T flag_value = 0;

	// attempt to set the value at position 32
	BP_UINT32_SET_FLAG(flag_value, 32);

	// attempt to set a flag at position 1
	BP_UINT32_SET_FLAG(flag_value, 1);

	// unset the flag here if desired
	// BP_UINT32_UNSET_FLAG(flag_value, 32);

	// display the integer represenation of the flag
	printf("\n Flag Value: %u", flag_value);

	// display the flags at position 1 and 32
	printf("\n Flag is Set at Position 32: %s", BP_UINT32_FLAG_IS_SET(flag_value, 32) ? "yes" : "no");
	printf("\n Flag is Set at Position 1:  %s", BP_UINT32_FLAG_IS_SET(flag_value, 1)  ? "yes" : "no");

	// toggle the flag using macro
	BP_UINT32_TOGGLE_FLAG(flag_value, 1);

	// now display that flag again
	printf("\n Flag Toggled on Position 1:   %s", BP_UINT32_FLAG_IS_SET(flag_value, 1)  ? "yes" : "no");
	printf("\n Flag Toggled off Position 1:  %s", BP_UINT32_FLAG_IS_SET(flag_value, 1)  ? "no" : "yes");

 */

// simple check to see if a particular bit is set
#define BP_CHECK_BIT_IS_SET(test_var, position) ((test_var) & (1<<(position)))

// set a bit within a position
#define BP_SET_BIT(test_var, position) test_var |= 1 << position


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%% Time Definitions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// one minute
#define BP_MINUTE 60

// one hour
#define BP_HOUR   3600

// one day
#define BP_DAY    86400

// approximately one year, in seconds (doesn't account for leap years etc)
#define BP_YEAR_APPROX 31536000


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%% empty defines for readability %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// empty defines used for function syntax.  This is used to increase
// code readability.  They are non-functional except as labels.

#define _vout      /* variable goes in with empty data, and comes out populated */
#define _vin       /* variable goes in and is unmodified */
#define _vinout    /* variable goes in with data, comes out with more data */
#define _vin_stack /* variable comes in and is stacked in some stack */
#define _vdestroy  /* variable will be destroyed */
#define _vdbin     /* variable is to be populated via database */
#define _vdbout    /* variable is to be populated into the database */
#define _vdbinout  /* variable comes in to the db, and is modified only to be rewritten to db on condition (eg. loop condition) */


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% File Line/Number Ez Gen: Defines / Expanding Macros  %%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// set this to allow embedding of file/line/func output.
#define USE_LINE_POS_FILE_OUTPUT 1

//  this can be added to any function requiring (size_t) line_number, (char *) function name.
#if USE_LINE_POS_FILE_OUTPUT == 0

	// this defines the symbol BPLN, which expands to file.cc:line_no:function name
	#define BPLN (char *) __FILE__, (size_t) __LINE__, (char *) __FUNCTION__

#else


	// you can use this to supply null values to the BPLN macro.  This would be set
	// in a non-debug build.
	#define BPLN (char *) "File Omitted", (size_t) 0, (char *) "Function Omitted"


#endif

// Macro for adding line parameters to any function declaration
// eg:
// 		void * whatever(void * whatever_parm, BPLN_PARMS);
//
#define BPLN_PARMS char * file_name, size_t line_number, char * func

// Macro for passing arguments through a function (mainly for macro-ized allocators such as ht_strdup/ht_calloc/etc)
#define BPLN_PARMS_PASSTHROUGH file_name, line_number, func



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%% Old-Dated Defines/ Typedefs %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



// Simple define used for determining whether or not
// to destroy an existing list.  Extremly dated, should be moved.
#define DESTROY 1

// Define used for network/ssl connections.  Dated, and should
// be moved in the future going forward.
#define NULL_HANDLE_ERROR 1

// Very old structure used with remote debugging.  Slated for removal.
typedef struct _RDEBUG_INFO {
	struct user_regs_struct registers;
	int signal;
	char * stackData;
	int stackSize;
	char * signalDescription;
} RDEBUG_INFO, *P_RDEBUG_INFO;



// Used to read data out of a buffer chain.
// typically contains a replica of the buffer
// data, and thus should be destroyed after
// being accessed.
typedef struct _READBLOCK {
	unsigned char * data;
	int position;
	int size;
} READBLOCK, *P_READBLOCK;




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%% Error Managment %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Error Managment Functions
#include "errors/BP-Errors.h"

// Simple critical exit macro (uses debug trap)
#define BP_LIBRARY_CRITICAL_DEBUG_EXIT(reason_ascii_str) \
			printf("\n [!!] BLOCKPARY LIBRARY CRITICAL ERROR: %s", reason_ascii_str); \
			printf("\n\n");                                                           \
			asm("int3");




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% UTHash Implementation %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// include header for hash table manipulation (heap based)
#include "./datastructures/hashtable/uthash.h"

// include header for hash table manipulation (mmap based)
#include "./datastructures/hashtable/uthash_mmap.h"




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Hash Table Data Structure Special Section %%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Hash tables are included above other data structures in the file
// inclusion list simply because they are used as a high performance
// lookup mechanism, used even in the memory profiler itself.

#include "./datastructures/hashtable/BP-HashTable.h"

// global hash table externalizations
#define BP_GLOBAL_HASH_REGISTRIES      \
		BP_GLOBAL_HASH_TABLE_REGISTRY; \
		BP_GLOBAL_MEMPROF_HASH_TABLE_REGISTRY;

// initializes global hash registries
#define BP_GLOBAL_HASH_REGISTRIES_INIT \
		BP_GLOBAL_HASH_TABLE_INIT;     \
		BP_GLOBAL_MEMPROF_HASH_TABLE_INIT;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Program Initiation Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%There ar

// will hold debug information structures in the future
#define BLOCKPARTY_DEBUG_UNIT_TEST

// entry point macro, includes blockparty and the global hash registries.
#define BLOCKPARTY_MAIN_GLOBALS \
	BP_GLOBAL_HASH_REGISTRIES;

// unit test entry point
#define BLOCKPARTY_UNIT_TEST_ENTRY_POINT \
		BLOCKPARTY_MAIN_GLOBALS;\
		BLOCKPARTY_DEBUG_UNIT_TEST;\
		int main(int argc, char **argv, char **envp)

// main application entry pointPro
#define BLOCKPARTY_APP_ENTRY_POINT\
		BLOCKPARTY_MAIN_GLOBALS;\
		int main(int argc, char **argv, char **envp)


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Linux Debugging Backtrace Capability %%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Useful utility for generating in-place backtraces (including symbols) for
// the purposes of debugging applications.  For best traces, compile your
// application with -rdynamic.
#include "./debugging/BP-LinuxBacktrace.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Blockparty Semaphore Managment %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// simple wrapper for semaphore routines, designed to allow for easy
// use of semaphores between threads.  This implementation simply contains
// debug options for semaphores which will list which threads are locking,
// how, and why.  This is particularly useful for debugging the blockparty
// event system / message pump.

#include "./semaphores/BP-Semaphores.h"




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% MMap Allocator %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// The mmap allocator is primarily used with hash table allocators, such
// as the one used in the blockparty canary implementation.  The reason
// to use external maps is to provide segreation and boundary protection
// to critical blockparty structures.
#include "mmap/BP-MMapAllocator.h"

// A slab allocator to go along with the normal mmap allocator.  Uses
// mmap to create a slab of items which allocations can come from.  It
// is highly recommended that this be used if you need mmaped arrays
// available.
#include "mmap/BP-MMapSlabAllocator.h"




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Seeding Random %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// NOTICE: Seeding random only needs to be done once per application
//         run.  It is for this reason that the random seeding is done
//         via a global singleton.  If you cann any random generation
//         function, it will automatically call the seed generation
//         init routine, which will seed from a system random source.
//         There is no reason to call the PNRG init routines directly.

// contains the seed generator
#include "utility/BP-SeedPNRG.h"






// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Canaries %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Canaries are values used to determine if invalid memory accesses
// have occured.  They are stored in their own mmap() range not adjacent
// to the heap.  This allows them to be used as a memory protection
// mechanism in the case of overwrites.

// include main file for canaries
#include "./canaries/BP-Canaries.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%% Linux Memory Profiler %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/*
 *
 * The following will enable the hooking of various glibc memory related
 * routines (malloc, realloc, free, etc).  Edit the values you find in
 * BP_LibraryMemoryProfiler.h to suit your profiling needs.
 *
 * Use the functions below in order to enable monitoring:
 * char * bpstrdup(char *dup);
 * char * bpstrndup(char *dup, size_t n);
 * void * bprealloc(void * addr, size_t size);
 * void * bpmalloc(size_t size);
 * void * bpcalloc(size_t size, size_t size_n);
 * void   bpfree(void * addr);
 *
 * The non bp* versions of the functions are still active, but are
 * not logged.
 *
 */

// enables robust debug message output from the memory profiler.
// THIS IS VERY SLOW AND SHOULD ONLY BE USED WHEN DEBUGGING BUGS
// WHICH MAY EXIST IN THE PROFILER ITSELF (hence the name).
#define DEBUG_MEMORY_PROFILER 1

// include the memory profiler headers
#include "./library_memory_profiling/BP_LibraryMemoryProfiling.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%% General Datatype Implementations %%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// !!! HASH TABLES ARE DECLARED ABOVE IN THEIR OWN SECTION !!!

// linked list
#include "./datastructures/linkedlist/BP_LinkedList.h"

// datalist structure (simple linked list, not a tailqueue list like the
// one found in BP_LinkedList.h).  This is the legacy linked list, do not
// use it going forward.  Use the tailqueue version.
#include "datastructures/linkedlist/BP-DataLists.h"

// buffer storage queue data structure
#include "datastructures/BufferQueue/BP-BufferStorageQueue.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Include JSON Parsing and Generation Routines %%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// JSON is a low level datatype in blockparty, just above linked lists.  It is used
// throughout the library as an abstraction layer for complex data types.  It was chosen
// as an abstraction language due to the large number of reader libraries available, as well
// as the ability to send JSON directly to the browser as a native data type.

// include libjsmn (MIT licensed/included directly in blockparty/thank you Serge Zaitsev).
// Be sure to include JSMN_PARENT_LINKS, which enables list traversal in the JSON objects.
#define JSMN_PARENT_LINKS
#include "javascript/JSON/BP-LibJSMN.h"

// include json parser/creator (included here so that JSON is accessible as a datatype
// throughout the entire library)
#include "javascript/JSON/BP-JSON.h"





// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Threads %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// include linux threads
#include "threads/BP-LinuxThreads.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Static Data Sets (ascii art/blurbs/borders/etc) %%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// These need to come after the definition of the datastructures, as
// they depend on them.

#include "static_data_sets/BP-StaticAsciiArt.h"
#include "static_data_sets/BP-StaticBorderSets.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Basic Buffer/Debug Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// debugging information queue
#include "debugging/BP-DebugInfoQueue.h"

// fuzzing utilities
#include "fuzzing/BP-FuzzUtilities.h"

// very simple buffer related utilities (legacy, do not use.  Use string
// utils instead)
#include "blockclass/BP-BufferUtilities.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Terminal Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#include "terminal/BP-LinuxTerminal.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Memory Garbage Collection %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// garbage collector
#include "garbage_collection/BP_MemoryGarbageCollector.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% ASCII Charts / Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// include the ascii chart functionality
#include "utility/BP-ASCIICharts.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% User Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// linux user utilities
#include "utility/BP-LinuxUserUtilities.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Operating System Profiling %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// includes the functions which allow us to do things like get processes, os information,
// etc.
#include "os_profiling/BP_LibstatgrabHelpers.h"
#include "os_profiling/BP_OSProfiling.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Generic Weight/Balancer (fast math) %%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// balancer
#include "utility/BP-GenericFastWeightedBalancer.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%% Filesystem Interaction  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Filesystem Interaction Functions (these are basic functions, if you
// need to handle a filesystem, there is a hash registry for that purpose
// below in the Fileystem Registry Section)
#include "filesystem/BP-Filesystem.h"




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%% String Manipulation Utilities %%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// General Purpose String Utilities (or ports)
#include "utility/BP-StringUtilities.h"

// General Purpose Template Processing
#include "utility/BP-GeneralPurposeTemplateSubstitution.h"

// General Purpose Number Utilities (or ports)
#include "utility/BP-NumberUtilities.h"

// General Purpose Time Utilities
#include "utility/BP-TimeUtilities.h"

// General Purpose File Utiliites
#include "utility/BP-FileGeneralUtilities.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Specific String Filters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// include database string filters
#include "databases/bpdb/BP-DBStringFilters.h"

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Filesystem Registry %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Filesystem registry (improved hash registry interface to the filesystem)
#include "filesystem/BP-FilesystemRegistry.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Random Color Macros (needs to be defined after utilities %%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// These simple macros let you generate random colors based on the result
// of a call to BP_GenRandomUnsignedLong (found in numbers utilities above)

// generate any color
#define BP_RANDOM_FG_COLOR   (BP_FOREGROUND_COLOR_CODE_T) BP_GenRandomUnsignedLong((size_t) BP_COLOR_CODE_FG_NONE, (size_t) BP_COLOR_CODE_FG_WHITE)
#define BP_RANDOM_BG_COLOR   (BP_BACKGROUND_COLOR_CODE_T) BP_GenRandomUnsignedLong((size_t) BP_COLOR_CODE_BG_NONE, (size_t) BP_COLOR_CODE_BG_WHITE)

// same as above, but no white
#define BP_RANDOM_FG_COLOR_NOT_WHITE (BP_FOREGROUND_COLOR_CODE_T) BP_GenRandomUnsignedLong((size_t) BP_COLOR_CODE_FG_NONE, (size_t) BP_COLOR_CODE_FG_CYAN)
#define BP_RANDOM_BG_COLOR_NOT_WHITE (BP_BACKGROUND_COLOR_CODE_T) BP_GenRandomUnsignedLong((size_t) BP_COLOR_CODE_BG_NONE, (size_t) BP_COLOR_CODE_BG_CYAN)

// same as above, but no black
#define BP_RANDOM_FG_COLOR_NOT_BLACK (BP_FOREGROUND_COLOR_CODE_T) BP_GenRandomUnsignedLong((size_t) BP_COLOR_CODE_FG_RED, (size_t) BP_COLOR_CODE_FG_WHITE)
#define BP_RANDOM_BG_COLOR_NOT_BLACK (BP_BACKGROUND_COLOR_CODE_T) BP_GenRandomUnsignedLong((size_t) BP_COLOR_CODE_BG_RED, (size_t) BP_COLOR_CODE_BG_WHITE)



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Color Generation Utilities (needs to be here) %%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#include "utility/BP-ColorGenerationUtilities.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%% Random ID Generation Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Simple utility macro which will cleanly return a new random size_t between 1 and 4bill'ish.  This
// feeds from /dev/urandom, so keep that in mind.
#define NEW_RANDOM_ID BP_GenRandomUnsignedLong(1,0xffffffff)

// Generates a new random UUID.  The memory created here must be bpfree()'d.
#define NEW_RANDOM_UUID BP_GenRandomUUID()



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%% Syslogging Functionality %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// main syslogging include
#include "utility/BP-Syslogging.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%% Functions Designed for Signal Autoconfiguration %%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// The functions found within this section of code are mainly used for
// configuring application response to signals.
//
//  e.g. auto-gdb on sigsegv

#include "utility/BP-LinuxSignalAutoconfigurations.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% String Formatting Sessions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// String formatting sessions are used to build:
//
// 		session->row group->row->column_group->column->string entry
//
// cascading display information.  This is suitable for generating arbitrary
// display output for terminal output.  It is used by all complex data
// output routines in the blockparty library.  While it is housed in
// the utility directory, this code can be considered a subsystem within
// itself.

// include the formatting session main header
#include "utility/BP-StringFormattingSessions.h"




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%% Sockets/SSL Interfaces %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Network
#include "network/BP-Network.h"
#include "network/BP-NetworkSSL.h"

// Raw Network Proxy Implementations (SOCKS5/SOCKS4)
#include "network/Proxies/BP-NetworkProxies.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% SQLite Bindings %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// include the sqlite database bindings
#include "databases/sqlite/BP-DBSqlite.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%% MySQL Bindings %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#define USE_MARIADB 1


// if we want to use mariadb, we have to set mysql here
#if USE_MARIADB == 1

	// This is the mariadb drop in replacement for the old mysql client library.  It's typically
	// located in /usr/local/mariadbclient/include/ by default, and should be defined within
	// the eclipse include/build settings.
	#include <mysql.h>

#else

	// this is the oracle mysql connector, use never
	#include <mysql/mysql.h>`


#endif

#include "databases/mysql/BP-DBMySQL.h"
#include "databases/mysql/BP-DBMySQLConnectionPooling.h"




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Blockparty Result DB %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// include the db structures and types
#include "databases/BP-ResultDBStructuresAndTypes.h"

// Include the blockparty database builtins.  This utilizes sqlite and
// mysql to create a seamless interface between both.  This happens by
// way of builtin blockparty hash tables.
#include "databases/bpdb/BP-DB.h"

// Include the bpdb helpers (auto-load schemas, etc)
#include "databases/bpdb/BP-DBMySQLHelpers.h"


// -- bpdb syntax generators + query helpers ---

// include syntax SQL syntax generator helpers
#include "databases/bpdb/BP-DBSQLSyntaxGenerators.h"

// query helpers for specific query types (seperated into files for easier readability/understanding/logical separation)
#include "databases/bpdb/query_helpers/BP-DBInsertQueryHelpers.h"
#include "databases/bpdb/query_helpers/BP-DBSelectQueryHelpers.h"
#include "databases/bpdb/query_helpers/BP-DBUpdateQueryHelpers.h"
#include "databases/bpdb/query_helpers/BP-DBReplaceQueryHelpers.h"
#include "databases/bpdb/query_helpers/BP-DBStoredProcedureQueryHelpers.h"



// -- error/warning/notice message oriented routines --

// routines used for adding messages and displaying structures
#include "databases/bpdb/BP-DBMessageReportingAndDisplayRoutines.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%% NMAP SQL DB Interface %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// nmap database includes
#include "network/nmapdb/BP-NmapDB.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%% Web Request Interface %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// web headers for curl/etc
#include "network/web/BP-NetworkWeb.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%% HTML Decoding %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#include "decoders/HTML/BP-HTMLDecoder.h"




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%% Packet Capture %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// PCAP routines
#include "network/PCAP/BP-PacketCapture.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%% Email Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// DEPRECATED: Old Email Routines
#include "network/email/BP-Email.h"

// New cURL based SMTP routines.
#include "network/email/BP-EmailSMTP.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%% Search Engine Scrapers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Google Search/Scraper (Dated, use disconnect me scraper)
#include "network/searchengines/BP-GoogleScraper.h"

// Bing Search/Scraper (Dated, use disconnect me scraper)
#include "network/searchengines/BP-BingScraper.h"

// Yahoo Search/Scraper (Dated, use disconnect me scraper)
#include "network/searchengines/BP-YahooScraper.h"

// include the disconnect me scraper
#include "network/searchengines/BP-DisconnectMeScraper.h"

// include the duckduckgo scraper
#include "network/searchengines/BP-DuckDuckGoScraper.h"






// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Httpd Server %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This provides the mechanism to allow for http clients to communicate
// with the daemon in the form of REST/json requests.  The blockparty
// library contains sets of verbs which can be used to accept and broker
// requests.  Each httpd server maintains it's own session information
// and it's own hash table registry.  Each http server created is also
// registered directly with the global hash table.


// include the header for the httpd server (based on libmicrohttpd)
#include "network/httpd/BP-HTTPd.h"

// Include httpd helper macros (used mostly for building your own applications.  Simple stuff
// like auth macros, etc.)
#include "network/httpd/BP-HTTPdHelperMacros.h"

// include request handler helpers
#include "network/httpd/BP-HTTPdRequestHandlerHelpers.h"






// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%% Debugging Flag %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Currently only 32-bit debugging is supported.  Use the
// define below to enable/disable debugging build support
// as necessary.

#define USE_BLOCKPARTY_DEBUGGING 1

#if __WORDSIZE == 64
#define BLOCKPARTY_DEBUGGING_TARGET_64BIT 1
#else
#define BLOCKPARTY_DEBUGGING_TARGET_32BIT 1
#endif


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% General Exploit Utils / Gadget Generators %%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// High level gadget factory.  Allows the custom crafting of self-encoding/decoding
// payloads for compromised targets.
#include "exploitation/Utilities/BP-Gadgets.h"

// gadget generation for PHP
#include "exploitation/Utilities/PHP/GadgetGenerators/BP-PHPGadgetGenerators.h"




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%% WEB EXPLOITATION %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Web Assessment Engine Hardcoded Limits (depth triggers)
#define BP_MAX_EMBEDDED_ANCHOR_TEST_LENGTH 8096
#define BP_MAX_EMBEDDED_FORM_TEST_LENGTH   8096

// Exploit test enumeration
enum BP_WEB_EXPLOIT_TEST_TYPE {
	BP_WEB_EXPLOIT_TYPE_NONE,
	BP_WEB_EXPLOIT_TYPE_XSS,
	BP_WEB_EXPLOIT_TYPE_SQLINJ,
	BP_WEB_EXPLOIT_TYPE_LDAPINJ,
	BP_WEB_EXPLOIT_TYPE_PHPINJ,
	BP_WEB_EXPLOIT_TYPE_ASPINJ,
	BP_WEB_EXPLOIT_TYPE_JSPINJ
};


// Utilities for working with web exploitation
#include "exploitation/WebExploitation/BP-WebExploitationUtils.h"

// String Matching Routines for Web Result Analysis
#include "exploitation/WebExploitation/BP-WebExploitationMatching.h"

// HTTP Error Code include
#include "exploitation/WebExploitation/BP-WebExploitationHttpErrCodes.h"

// SQL DB Error Code Tables/Generators
#include "exploitation/WebExploitation/SQLErrCodes/BP-WebExploitMysqlErrCodes.h"
#include "exploitation/WebExploitation/SQLErrCodes/BP-WebExploitMsqlErrCodes.h"
#include "exploitation/WebExploitation/SQLErrCodes/BP-WebExploitPostgresErrCodes.h"
#include "exploitation/WebExploitation/SQLErrCodes/BP-WebExploitODBCErrCodes.h"
#include "exploitation/WebExploitation/SQLErrCodes/BP-WebExploitOracleErrCodes.h"

// Misc Error Codes
// NONE YET

// Application Engine/Server Error Codes
#include "exploitation/WebExploitation/AppEngineErrCodes/BP-WebExploitPHPErrCodes.h"
#include "exploitation/WebExploitation/AppEngineErrCodes/BP-WebExploitASPErrCodes.h" // pending
#include "exploitation/WebExploitation/AppEngineErrCodes/BP-WebExploitJSPErrCodes.h" // pending
#include "exploitation/WebExploitation/AppEngineErrCodes/BP-WebExploitCFMErrCodes.h" // pending
#include "exploitation/WebExploitation/AppEngineErrCodes/BP-WebExploitDotNETErrCodes.h" // pending

// Lookup functionality
#include "exploitation/WebExploitation/BP-WebExploitationTableLookups.h"

// Application Server Result Data
#include "exploitation/WebExploitation/BP-WebExploitationAppServers.h"

// Web Exploitation Main Includes for XSS/SQL
#include "exploitation/WebExploitation/BP-WebExploitationXss.h"
#include "exploitation/WebExploitation/BP-WebExploitationSql.h"

// Blind SQL Injection Capabilities
#include "exploitation/WebExploitation/BlindSQLiDetection/BP-WebExploitationBlindSQLiDetection.h"

// Web Exploitation Assessment Includes
#include "exploitation/WebExploitation/BP-WebExploitation.h"

// Web exploitation exploit request sets.
#include "exploitation/WebExploitation/BP-WebExploitationExploitRequestSets.h"

// Analysis simplification routines
#include "exploitation/WebExploitation/BP-WebExploitTestResultAnalysis.h"


// Command Execution Capabilities
#include "exploitation/WebExploitation/CommandExecution/BP-WebExploitationCommandExecution.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Blockparty Debugging %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// include the remote attach plugin if specified
// #define BP_USE_REMOTE_ATTACH_PLUGIN
#ifdef BP_USE_REMOTE_ATTACH_PLUGIN
	#include "r-attach/BP-TapPlugin.h"
#endif


#if USE_BLOCKPARTY_DEBUGGING == 1

// DEBUG STRUCTUES, MUST BE DEFINED HERE BECAUSE OF ELF
// RUNTIME INTERDEPENDENCIES WITH P_MEM_MAP STRUCTURE
// Note that the elf pointer in the memory map is a void pointer
// and not typedefed to an elf mapping.  You must typedef this yourself
// when using it.
#include "debugging/BP-LinuxDebugStructures.h"

// Elf Extractions
#include "ELF/ELF32/BP-ELFExtraction.h"
#include "ELF/ELF32/BP-ELFSections.h"

// debugging and memory
#include "debugging/BP-LinuxDebug.h"
#include "memory/BP-LinuxMemMapping.h"

// exploitation routines
#include "exploitation/BP-LinuxExploitation.h"

// intel x86 decoder (custom)
#include "decoders/x86ASM/BP-x86InstTable.h"
#include "decoders/x86ASM/BP-x86InstDecode.h"


#endif // USE_BLOCKPARTY_DEBUGGING == 1



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Command and Control Interface (for processes) %%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// process command and control interface (used for controlling processes)
#include "command_control_interface/BP-CommandControlInterface.h"





// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%% Modular Exploitation Includes %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// main include for the modular exploitation subsystem
#include "exploitation/ModularExploitation/BP-ModularExploitation.h"

// include tests if necessary
#define MOD_EXPLOITATION_INCLUDE_UNIT_TESTS 1
#if MOD_EXPLOITATION_INCLUDE_UNIT_TESTS == 1

// include basic (mostly empty) module unit test
#include "exploitation/ModularExploitation/BuiltInExploitModules/UnitTestExploitModule/BP-ExploitModule_ut.h"


#endif

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Compromise Sessions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/*
 *
 * A compromise session is a general session used for interfacing with a
 * variety of compromised endpoints.  Typically, an endpoint will contain
 * a stage two listener, either established directly or via connect back,
 * which can be used to garner target information about the remote host.
 *
 */

// main include for the modular exploitation subsystem
#include "exploitation/Compromise/BP-Compromise.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Generic Buffer Analysis %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This is the last include in the main BP include, this is because analysis can utilzie
// all portions of the library to generate it's heuristics.

// main include for ascii related buffer analysis
#include "buffer_analysis/ASCII/BP-BufferAnalysisASCII.h"

// Main include for ALL buffer analysis capability.  This contains generic buffer
// analysis routines.
#include "buffer_analysis/BP-BufferAnalysis.h"




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%% BLOCKPARTY BLOCKCLASS INCLUSIONS %%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// includes relevant mysql components into class
#define   BP_BLOCKCLASS_INCLUDE_MYSQL_DB_BINDINGS
// #undef BP_BLOCKCLASS_INCLUDE_MYSQL_DB_BINDINGS


// includes disassembler bindings into class
#if USE_BLOCKPARTY_DEBUGGING == 1
#define   BP_BLOCKCLASS_INCLUDE_DEBUGGER_BINDINGS
// #undef BP_BLOCKCLASS_INCLUDE_DEBUGGER_BINDINGS
#endif // USE_BLOCKPARTY_DEBUGGING == 1


// includes fuzzer bindings into class
#define   BP_BLOCKCLASS_INCLUDE_FUZZING_BINDINGS
// #undef BP_BLOCKCLASS_INCLUDE_FUZZING_BINDINGS

// includes network bindings into class
#define   BP_BLOCKCLASS_INCLUDE_NETWORK_BINDINGS
// #undef BP_BLOCKCLASS_INCLUDE_NETWORK_BINDINGS

// includes network/ssl bindings into class
#define   BP_BLOCKCLASS_INCLUDE_NETWORKSSL_BINDINGS
// #undef BP_BLOCKCLASS_INCLUDE_NETWORKSSL_BINDINGS

// includes http greifing bindings into class
#define   BP_BLOCKCLASS_INCLUDE_HTTPGREIF_BINDINGS
// #undef BP_BLOCKCLASS_INCLUDE_HTTPGREIF_BINDINGS

// includes file search/manipulation bindings into class
#define   BP_BLOCKCLASS_INCLUDE_FILE_BINDINGS
// #undef BP_BLOCKCLASS_INCLUDE_FILE_BINDINGS

// includes pthread bindings into class
#define   BP_BLOCKCLASS_INCLUDE_PTHREAD_BINDINGS
// #undef BP_BLOCKCLASS_INCLUDE_PTHREAD_BINDINGS

// use old pthread methods (should never use these unless under special circumstances)
#define BP_BLOCKCLASS_INCLUDE_USE_OLD_PTHREAD_METHODS
#undef  BP_BLOCKCLASS_INCLUDE_USE_OLD_PTHREAD_METHODS

// general utilities
#define BP_BLOCKCLASS_INCLUDE_GENERAL_UTILITIES
// #undef BP_BLOCKCLASS_INCLUDE_GENERAL_UTILITIES

// use new pthread methods (should always use this unless supporting extreme legacy code, the old
// code has some systematic problems with bad reads)
#define BP_BLOCKCLASS_INCLUDE_USE_NEW_PTHREAD_METHODS
// #undef BP_BLOCKCLASS_INCLUDE_USE_NEW_PTHREAD_METHODS

// includes HTML bindings (libxml/curl)
#define BP_BLOCKCLASS_INCLUDE_HTML_BINDINGS
// #undef BP_BLOCKCLASS_INCLUDE_HTML_EXTRACTION_BINDINGS


// include Web Assessment (libxml/curl and dependent on html bindings above)
#define BP_BLOCKCLASS_INCLUDE_WEB_ASSESSMENT_BINDINGS
// #undef BP_BLOCKCLASS_INCLUDE_WEB_ASSESSMENT_BINDINGS

// include PCAP bindings for file capture, generation, replay (libpcap)
#define BP_BLOCKCLASS_INCLUDE_PCAP_GENERATION_BINDINGS
// #undef BP_BLOCKCLASS_INCLUDE_PCAP_GENERATION_BINDINGS

// include NMAP_SQL_DB bindings for extracting data from a mysql database (requires mysql bindings enabled)
#define BP_BLOCKCLASS_INCLUDE_NMAP_SQL_DB_BINDINGS
// #undef BP_BLOCKCLASS_INCLUDE_NMAP_SQL_DB_BINDINGS

// include NMAP_SQL_DB bindings for extracting data from a mysql database (requires mysql bindings enabled)
#define BP_BLOCKCLASS_INCLUDE_COMMAND_CONTROL_INTERFACE
// #undef BP_BLOCKCLASS_INCLUDE_COMMAND_CONTROL_INTERFACE

// --------- User DB Interface -----------------------

// include the user DB interface
#define BP_BLOCKCLASS_INCLUDE_USER_DB
// #undef BP_BLOCKCLASS_INCLUDE_USER_DB

// --------- Results DB Interface --------------------

// include this to include the result db capability
#define BP_BLOCKCLASS_RESULT_DB
// #undef BP_BLOCKCLASS_RESULT_DB

// includes web proxy DB bindings (very useful integration into a proxy db table, multipurpose)
#define BP_BLOCKCLASS_INCLUDE_PROXY_DB_BINDINGS
// #undef BP_BLOCKCLASS_INCLUDE_PROXY_DB_BINDINGS

#define BP_BLOCKCLASS_INCLUDE_GOOGLE_SCRAPING_INTERFACE
// #undef BP_BLOCKCLASS_INCLUDE_GOOGLE_SCRAPING_INTERFACE

#define BP_BLOCKCLASS_INCLUDE_BING_SCRAPING_INTERFACE
// #undef BP_BLOCKCLASS_INCLUDE_BING_SCRAPING_INTERFACE

#define BP_BLOCKCLASS_INCLUDE_YAHOO_SCRAPING_INTERFACE
// #undef BP_BLOCKClASS_INCLUDE_YAHOO_SCRAPING_INTERFACE

#define BP_BLOCKCLASS_INCLUDE_DISCONNECTME_SCRAPING_INTERFACE
// #undef BP_BLOCKClASS_INCLUDE_DISCONNECTME_SCRAPING_INTERFACE

#define BP_BLOCKCLASS_INCLUDE_DDG_SCRAPING_INTERFACE
// #undef BP_BLOCKCLASS_INCLUDE_DDG_SCRAPING_INTERFACE



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%% Main Block Class Header (Block()) %%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Set this to 1 in order to see memory which is being auto-free'd by
// the garbage collector on deconstruction.
#define BLOCK_CLASS_DEBUG_DECONSTRUCTOR_MEMORY_MESSAGES 1

// basic error handling class, practically all classes inherit error/warning
// managment features from this class.
#include "blockclass/subclasses/BP_BaseError.h"

// simple class used for manipulating strings.  This class is simply
// an easy to use front end for most of the routines found within the
// C BP-StringUtilities.(cc/h) files.
#include "blockclass/subclasses/BP_StringUtils.h"

// Include the json parser utility class.  This class is used for easily manipulating JSON data.
#include "blockclass/subclasses/BP_JSONUtils.h"

// More complicated string class, used for handling string formatting sessions.
// This class serves as a frontend for the routines found within the
// BP-StringFormattingSessions.(cc/h) files.  This should be used for all
// complicated/application specific terminal output, as it supports
// multiple features, including colorization, etc.
#include "blockclass/subclasses/BP_StringFormattingSession.h"


// Main Blockparty block class
#include "blockclass/BP-BlockClass.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%% BlockParty Utility Subclasses %%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// utility class for parsing configuration files
#include "blockclass/subclasses/BP_ConfigParser.h"

// utility class for parsing command line options
#include "blockclass/subclasses/BP_CommandLineOptionParser.h"

// utility for creating quick / messy forking (shouldn't use this, it's outdated.  Use the CLT
// interface instead.
#include "blockclass/subclasses/BP_ForkDispatcher.h"

// Utility for creating threads. (also shouldn't use this, use the Block threaded dispatcher)
#include "blockclass/subclasses/BP_ThreadDispatcher.h"

// Utility class for interacting with ssh sessions
#include "blockclass/subclasses/BPSC_SSHIntegrityChecker.h"

// DB interface for result DB (OLD, VERY OLD)
#include "blockclass/subclasses/BPSC_WebAssessmentDbInterface.h"

// Utility class for spawning multiple mysql connections.
#include "blockclass/subclasses/BP_MySQLMultiThreadConnectionPool.h"

// Don't use this.
#include "blockclass/subclasses/BP_ThreadedWebResultStackSpider.h"

// web form submitter
#include "blockclass/subclasses/BP_WebFormSubmitter.h"


// utility class for auto-generating application help messages
// (relies on the BP_ConfigParser class)
#include "blockclass/subclasses/BP_ApplicationHelpGenerator.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Specific Application Controllers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// The following classes are used to control various specific applications.  These applications
// can be directly used to modifiy the runtime behavior of the various tools supported.  This
// can be dangerous in terms of local security if done poorly, so I've taken great
// care to try to thwart the inherent dangers, but it's up to you to validate that
// command line injections are not possible.

// Note: For instructions on how to use these application controllers, see the top of
//       each included file below.


// Application run monitor.  Used to make sure processes don't run out of control.  Because
// of the nature of interfacing with processes, sometimes we can't be certain about how applications
// will behave.  With this run monitor, we can use OS profiling information to make determination
// regarding the current number of running processes, as well as the current state of VM/CPU/Load etc.
// This information can be critical in terms of keeping sensor health within optimal ranges.
//
// A instance of this singleton is present in every BP_CommandLineTool object created.
#include "blockclass/subclasses/command_line_tools/BP_CLTRunMonitor.h"

// Command line tool base class (used for interacting with command line tools)
#include "blockclass/subclasses/command_line_tools/BP_CommandLineToolBase.h"

// SQLMap Application Controller
#include "blockclass/subclasses/command_line_tools/sqlmap/BP_SQLMapApplicationController.h"

// NMap Application Controller
#include "blockclass/subclasses/command_line_tools/nmap/BP_NMapApplicationController.h"




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Basic Events %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Libevent includes (package libevent-dev)
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

// event handling functions
#include "events/BP-Events.h"

// These are helper macros for developers to use when defining new event driven subsystems
// (use these to implement your subsystems for the sake of uniformity)
#include "events/BP-EventDrivenSubsystemHelperMacros.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Tor Interface (utilizes event system) %%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// include tor header
#include "network/tor/BP-Tor.h"

// include hidden services main
#include "network/tor/hidden_services/BP-TorHiddenServices.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% NMap Application Controller (utilizes event system) %%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// include nmap header
#include "network/nmap/BP-NMAP.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Web Chat %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// include webchat
#include "webchat/BP-WebChat.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Javascript (via Mozilla Spidermonkey) %%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// include admin builtin functions
#include "javascript/builtin/admin/BP-JSAdminBuiltins.h"

// include system builtin functions
#include "javascript/builtin/system/BP-JSSystemBuiltins.h"

// include user builtin functions
#include "javascript/builtin/user/BP-JSUserBuiltins.h"

// include document object model builtin functions
#include "javascript/builtin/dom/BP-JSDomBuiltins.h"


// Include the javascript binding (builtin functions must be included
// before this is.)
#include "javascript/BP-Javascript.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Main Init Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// simple macro for performing init
#define BP_GLOBAL_MAIN_INIT \
		/* initialize global registries. */                                     \
		BP_GLOBAL_HASH_REGISTRIES_INIT;                                         \
		/* initialize global spidermonkey context */                            \
		JS_Init();                                                              \
		/* initialize global curl context. */                                   \
		curl_global_init(CURL_GLOBAL_ALL);

// simple macro for performing exit. Destroys all memory used
// by blockparty before exiting.  For that reason this shouldn't
// be used by unit tests.  In unit tests we need to see which memory
// is escaping so we can destroy it.  For this reason we have the
// BP_GLOBAL_MAIN_UNIT_TEST_EXIT macro instead.
#define BP_GLOBAL_MAIN_EXIT \
		BP_GLOBAL_HASH_TABLE_CLOSE;\
		JS_ShutDown();\
		BP_GLOBAL_MEMPROF_HASH_TABLE_CLOSE;\
		mysql_library_end();\
		curl_global_cleanup();

// --- Unit Test Versions ----------------------


// Simple macro to exit main without destroying the memprofiler
// table.  The reason this is important is that if the memprofiler
// is destroyed, all associated memory is ALSO destroyed, meaning that
// we can't trace leaked memory in unit tests.
#define BP_GLOBAL_MAIN_UNIT_TEST_EXIT\
		BP_GLOBAL_HASH_TABLE_CLOSE;\
		JS_ShutDown();\
		global_mprof_hash_table_init_ok_val = 0;\
		if(global_mprof_hash_table_registry.memory_table)\
		{\
			free(global_mprof_hash_table_registry.memory_table);   \
			global_mprof_hash_table_registry.memory_table = NULL;  \
		}\
		mysql_library_end();\
		curl_global_cleanup();


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Generic Event Subsystems %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// this is the generic session marker which will be used by the edcnc session generator
#define BP_EDCNC_GENERIC_SESSION_MARKER "edcnc_generic_session"

// generic event subsystem handler for command and control systems
#include "events/generic_event_subsystems_helpers/BP-EventDrivenCommandAndControlGenericSessions.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Ansible Subsystem %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// include the ansible subsystem for easy remote system management
#include "ansible/BP-Ansible.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Httpd Server Promises System %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Promises header should be included very last, as promises are an interface
// abstraction layer for all blockparty components.
#include "network/httpd/promises/BP-HTTPdPromises.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Blockparty Binary (.so) Module Loader Utilities %%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Include the dynamic modules loader header file
#include "dynamic_modules/BP-DynamicModules.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Display an entire httpd session
BP_ERROR_T BP_HTTPdDisplay(P_BP_HTTPD_SESSION httpd);


// END MAIN BLOCKPARTY INCLUDE FILE
#endif // BP_MAIN_H

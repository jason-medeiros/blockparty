

#ifndef BP_ERRORS_H
#define BP_ERRORS_H


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Error Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% General Error Codes %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// The codes below are the integral status indicators for the blockparty
// library.  They remain in use for most of the blockparty C library, and
// in specific places within the C++ library.

// Generic ERROR/SUCCESS Codes (widely implemented throughout the library.  If an application returns
// error, you can often retrieve information about the error within the global error log )
typedef enum BP_ERROR_T
{
	ERR_FAILURE = 0,
	ERR_SUCCESS = 1
};


// Invalid code (minimially implemented in edge cases)
#define ERR_INVALID                         2

// set enabled and disabled (same as success/failure)
#define BP_ENABLED  ERR_SUCCESS
#define BP_DISABLED ERR_FAILURE


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Old/Antiquated Error Codes %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// These codes are older and were originally going to be used for defining
// system behavior but fell out of use for maintainence reasons.  They are still
// implemented in some VERY OLD parts of the library.  This is not to say
// that old parts are bad parts, so take it with a grain of salt but be
// aware that going forward most things return either success/failure or
// a pointer to a newly created thing.  Things that require enumerated
// type returns (aka more than one code) have appropriate enums described
// where possible.


// General Errors 2 - 150
#define ERR_FAILED_ASSIGNMENT 				10
#define ERR_FAILED_CREATE_GLOBAL_TREE 		11
#define ERR_CANNOT_OPEN_FILE                12
#define ERR_INVALID_CHARACTERS				13
#define ERR_STRNDUP_FAILED					14
#define ERR_FAILED_MALLOC                   15

// Buffer Stoage Queue Error Conditions:  151 - 200
#define ERR_QUEUE_NULL	               		151
#define ERR_QUEUE_CONFIGURATION_FAILED 		152

// Debug Error Conditions:  201 - 449
#define ERR_DBG_PROCESS_EXITED				201
#define ERR_DBG_PROCESS_SIGNALED 			202
#define ERR_DBG_PROCESS_STOPPED 			203
#define ERR_DBG_HANDLE_NULL					204
#define ERR_DBG_BREAKPOINTS_NULL			205
#define ERR_DBG_CONDITIONS_NULL				206
#define ERR_DBG_NOT_ATTACHED				207
#define ERR_DBG_CANNOT_CONTINUE_DEBUGGER	208
#define ERR_DBG_WAIT_RETURNS_UNMATCHED_PID  209
#define ERR_DBG_PTRACE_RETURNS_ERR          210
#define ERR_MEMMAP_NULL						211
#define ERR_MEMMAP_CANNOT_ADVANCE_STRING    212
#define ERR_HEAP_ELEMENT_NULL               213
#define ERR_HEAP_LIST_NULL                  214
#define ERR_MEM_NO_HEAP_PRESENT				215
#define ERR_MEM_NO_STACK_PRESENT			216
#define ERR_DBG_CANNOT_REFRESH_DATA			217
#define ERR_DBG_CANNOT_CREATE_EXPLOIT_ANALYSIS 218
#define ERR_DBG_TRIGGER_FILE_NOT_SET		219
#define ERR_DBG_CANNOT_ALLOCATE_TRIGGER_FILE_MEMORY 220
#define ERR_DBG_CANNOT_LOCATE_WRITE_DELIMITER 	221
#define ERR_DBG_EXPLOIT_DELIMITER_MALFORMED 	222
#define ERR_LANGUAGE_NOT_SPECIFIED 				223
#define ERR_CANNOT_WRITE_PAYLOAD_FILE			224
#define ERR_CANNOT_WRITE_TRIGGER_FILE			225
#define ERR_TRIGGER_FILE_PASSED_AS_NULL			226
#define ERR_PAYLOAD_FILE_PASSED_AS_NULL			227
#define ERR_PAYLOAD_LENGTH_ZERO                 228
#define ERR_TRIGGER_ID_ZERO						229
#define ERR_EXPLOIT_TAG_NOT_FOUND				230
#define ERR_LANGUAGE_TAG_NOT_FOUND				231
#define ERR_NAME_TAG_NOT_FOUND					232


// Network Errors 450 - 600
#define ERR_NETWORK_NULL_DEST               450
#define ERR_NETWORK_NEGATIVE_SOCKET			451
#define ERR_SENT_LESS_THEN_SIZE				452
#define ERR_READ_FAILED						453
#define ERR_FAILED_CREATE_SOCK_TREE         454
#define ERR_FAIL_SSL_LIB_INIT               455
#define ERR_FAIL_SSL_SET_METHOD             456
#define ERR_FAIL_SSL_SET_CONTEXT			457
#define ERR_FAIL_SSL_SET_SSL	            458
#define ERR_FAIL_SSL_CONTEXT_VERIFICATION   459
#define ERR_FAIL_SSL_SET_FD                 460
#define ERR_FAIL_SSL_CONNECT				461
#define ERR_FAIL_SSL_READ_ERROR				462
#define ERR_FAIL_SSL_ACCEPT					463
#define ERR_FAIL_SSL_RSAKEY_NULL			464
#define ERR_FAIL_SSL_SET_PRIVATE_KEY		465
#define ERR_FAIL_SSL_SET_PUBLIC_KEY			466
#define ERR_FAIL_SSL_USE_CERTIFICATE_FILE   467
#define ERR_FAIL_SSL_USE_PRIVATEKEY_FILE    468
#define ERR_FAIL_SSL_CHECK_PRIVATE_KEY      469
#define ERR_FAIL_SSL_PASSWORD_NULL          470
#define ERR_FAIL_SSL_NULL                   471
#define ERR_FAIL_SSL_WRITE_ERROR			472
#define ERR_FAIL_SSL_SHUTDOWN               473
#define ERR_FAIL_SSL_PASS_TOO_LONG   		474
#define ERR_FAIL_SSL_SEED_FILE_TOO_LONG 	475
#define ERR_FAIL_SSL_KEY_FILE_TOO_LONG 		476
#define ERR_FAIL_SSL_INPUT_TOO_LONG         477

#endif // BP_ERRORS_H

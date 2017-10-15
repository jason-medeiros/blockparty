/*
 * BP-Semaphores.h
 *
 *  Created on: Apr 17, 2015
 *      Author: root
 */

#ifndef INCLUDE_SEMAPHORES_BP_SEMAPHORES_H_
#define INCLUDE_SEMAPHORES_BP_SEMAPHORES_H_


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Data Structures and Defined Values %%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// set this to true or false to debug semaphore locks
#define BP_DEBUG_SEMAPHORE_LOCKS BP_TRUE

// The state of the semaphore.
typedef enum BP_SEMAPHORE_STATE
{
	BP_SEMAPHORE_STATE_UNSET,
	BP_SEMAPHORE_STATE_LOCKED,
	BP_SEMAPHORE_STATE_UNLOCKED,
	BP_SEMAPHORE_STATE_MULTIPLE_LOCKS_WAITING
};


// Semaphore locking information
typedef struct __BP_SEMAPHORE_LOCK_INFO {

	// pthread thread id
	pthread_t pthread_id;

	// thread id (via gettid() syscall)2
	pid_t thread_id;

} BP_SEMAPHORE_LOCK_INFO, *P_BP_SEMAPHORE_LOCK_INFO;


// Semaphore tracking information
typedef struct __BP_SEMAPHORE_INFO {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Semaphore Info %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// flag whether or not the semaphore has been initialized
	BP_BOOL sem_initialized;

	// semaphore
	sem_t sem;

	// state of the semaphore
	BP_SEMAPHORE_STATE state;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Lookup Information / Key %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// key used for lookups
	char uuid_key[50];


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Line/File/Func Info %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// line this was created (these are not duplicated, they are just pointers
	// to the actual static references)
	size_t line;
	char * func;
	char * file;


} BP_SEMAPHORE_INFO, *P_BP_SEMAPHORE_INFO;




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Quality of Life Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Macro to validate the we have a correct working environment within the
// global hash registry.
#define BP_SEMAPHORE_VALIDATE_WORKING_ENVIRONMENT(ret) \
	if(BP_SemaphoreEnsureWorkingGlobalHashTableEnvirionment() == ERR_FAILURE) \
	{\
		printf("\n [!!] Error: Could not validate semaphore working environment within global hash table.  Critical error.");\
		return ret;\
	}




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Macro->Func Wrappers to Track BPLN Values Automatically %%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// --- SEMAPHORE INIT ----------

// initialize a semaphore (wrapper with parameters supplied)
#define BP_SemaphoreInit(pshared, value) \
		BP_SemaphoreInit_real(pshared, value, BPLN)

// initialize a semaphore, but use default values
#define BP_SemaphoreInitDefault() \
		BP_SemaphoreInit_real(1, 1, BPLN)

// --- SEMAPHORE DESTROY ------

// NOT Implemented (function does not need BPLN)

// --- SEMAPHORE WAIT ---------

#define BP_SemaphoreWait(sem) \
		BP_SemaphoreWait_real(sem, BPLN)

#define BP_SemaphoreTryWait(sem) \
		BP_SemaphoreTryWait_real(sem, BPLN)

// --- SEMAPHORE POST ----------

#define BP_SemaphorePost(sem) \
		BP_SemaphorePost_real(sem, BPLN)

// --- SEMAPHORE GET VALUE -----

#define BP_SemGetValue(sem) \
		BP_SemGetValue_real(sem, BPLN)


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Global Hash Registry Related Routines %%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This is called to ensure that the /system/semaphores table has
// been setup correctly.  It simply searches for an existing semaphore
// table and if it doesn't exist, adds it to /system/.
BP_ERROR_T BP_SemaphoreEnsureWorkingGlobalHashTableEnvirionment();


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Semaphore Init and Destroy %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// semaphore init
P_BP_SEMAPHORE_INFO BP_SemaphoreInit_real(int pshared, unsigned int value, BPLN_PARMS);

// semaphore destroy
BP_ERROR_T BP_SemaphoreDestroy(P_BP_SEMAPHORE_INFO sem);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Wait and Post (READ: Un/Locking) %%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// NOTE ABOUT LOCKING.  The locks GHT table is dynamically created
// when calling the routines below.  The wait routine will add a lock
// entry.  The post routine will remove it.  There is not BP_SemaphoreLock
// as it's all taken care of by wait and post.

// semaphore wait
BP_ERROR_T BP_SemaphoreWait_real(P_BP_SEMAPHORE_INFO sem, BPLN_PARMS);

// semaphore trywait
BP_ERROR_T BP_SemaphoreTryWait_real(P_BP_SEMAPHORE_INFO sem, BPLN_PARMS);

// semaphore post
BP_ERROR_T BP_SemaphorePost_real(P_BP_SEMAPHORE_INFO sem, BPLN_PARMS);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Retrieve Semaphore Value %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to retrive the value of a semaphore
int BP_SemGetValue_real(P_BP_SEMAPHORE_INFO sem, BPLN_PARMS);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Lookup Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// searches the GHT looking for a semaphore matching the given unique uuid
P_BP_SEMAPHORE_INFO BP_SemaphoreLookupByUUID(char *uuid);

// Attempts to retrieve a keyed table entry (hash table entry) for the
// provided semaphore.
P_BP_HASH_TABLE_KEYED_ENTRY BP_SemaphoreGetKeyedTableEntry(P_BP_SEMAPHORE_INFO sem);

// Attempts to retrieve a semaphore lock keyed table entry (hash table entry) for the
// provided semaphore.  This returns the first semaphore only, and is used primarily
// by BP_SemaphorePost_real.
P_BP_HASH_TABLE_KEYED_ENTRY BP_SemaphoreLocksGetFirstLock(P_BP_SEMAPHORE_INFO sem);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Existence Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Check to see if a semaphore exists in GHT
BP_ERROR_T BP_SemaphoreExistsInGHT(P_BP_SEMAPHORE_INFO sem);

// Simple check to see if there is a lock table for the given semaphore (lock
// table holds a cascading set of locks demonstrating which threads are waiting)
BP_ERROR_T BP_SemaphoreLockTableExistsInGHT(P_BP_SEMAPHORE_INFO sem);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Semaphore Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Will look into /system/semaphores and display all semaphores.
BP_ERROR_T BP_DisplaySemaphores(BP_BOOL display_lock_order);

// DO NOT USE DIRECTLY WITHOUT UNDERSTANDING WHAT YOU'RE DOING:
// Will attempt to display a valid semaphore table within the
// semaphore table.
BP_ERROR_T BP_DisplaySemaphoreTable(P_BP_HASH_TABLE_KEYED_ENTRY semaphore_table);

// Will attempt to display a valid semaphore entry in the table.
BP_ERROR_T BP_DisplaySemaphore(P_BP_SEMAPHORE_INFO sem);

// Will attempt to display a semaphore by looking it up by UUID.
BP_ERROR_T BP_DisplaySemaphoreByUUID(char *uuid, BP_BOOL display_lock_order);




#endif /* INCLUDE_SEMAPHORES_BP_SEMAPHORES_H_ */

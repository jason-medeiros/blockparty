/*
 * BP_Semaphores.cc
 *
 *  Created on: Apr 17, 2015
 *      Author: root
 */


#include "../../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Global Hash Registry Related Routines %%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This is called to ensure that the /system/semaphores table has
// been setup correctly.  It simply searches for an existing semaphore
// table and if it doesn't exist, adds it to /system/.
BP_ERROR_T BP_SemaphoreEnsureWorkingGlobalHashTableEnvirionment()
{

	// ensure the hash table has been initialized tok
	if(!(BP_GLOBAL_HASH_TABLE_INIT_OK))
	{
		printf("\n [!!] Error: Attempted to create a semaphore even though the global hash tables were not initialized.  Please call BP_GLOBAL_HASH_TABLE_INIT before attempting to use blockparty semaphores.");
		return ERR_FAILURE;
	}

	// enter ght critical section
	BP_GHT_ENTER_CRITICAL_SECTION;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Hash Table Lookups %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Simple flag to indicate via logic below whether or not the /system/semaphores table
	// exists already.
	size_t semaphore_table_exists = 0;

    // create a table path on the stack for ease of use
	char * table_path_actual[10] = {0};

	// reference to the local stack
	BP_HASH_TABLE_TRAVERSAL_PATH table_path = (char **) &table_path_actual;

	// set path to /system/semaphores
	table_path[0] = "system";
	table_path[1] = "semaphores";
	table_path[2] = NULL;

	// attempt lookup for /system/semaphores
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);

	// display lookup results
	if(tmp_lookup)
		semaphore_table_exists = BP_TRUE;
	else
		semaphore_table_exists = BP_FALSE;

	// if the table exists, immediately  return OK
	if(semaphore_table_exists == BP_TRUE)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Tables If Necessary %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// First check if the system path exists.  If not, create it.
	table_path[0] = "system";
	table_path[1] = NULL;

	// lookup system
	tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);

	// if it's not there, create it
	if(!tmp_lookup)
	{

		// add the table
		BP_HashRegAddTableByPath(NULL, table_path);

		// ensure it exists
		tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);

		// ensure we can lookup the table, and exit if we
		// failed for some reason.
		if(!tmp_lookup)
			BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}

	// now check for the semaphore path
	table_path[0] = "system";
	table_path[1] = "semaphores";
	table_path[2] = NULL;

	// add the table
	BP_HashRegAddTableByPath(NULL, table_path);

	// ensure it exists
	tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);

	// ensure we can lookup the table, and exit if we
	// failed for some reason.
	if(!tmp_lookup)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// return indicating success
	BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Semaphore Init and Destroy %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// semaphore init
P_BP_SEMAPHORE_INFO BP_SemaphoreInit_real(int pshared, unsigned int value, BPLN_PARMS)
{


	// validate that there is a working environment
	BP_SEMAPHORE_VALIDATE_WORKING_ENVIRONMENT(NULL);

	// enter critical section
	BP_GHT_ENTER_CRITICAL_SECTION;

	// create new uuid buffer
	char new_uuid[50];

	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// loop till we find a valid uuid (should realistically only be one loop cycle)
	for(;;)
	{

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Create New UUID For Semaphore Table %%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// zero out buff
		memset(&new_uuid, 0x00, 50);

		// create a new uuid
		BP_GenRandomUUID_StoreInBuff((char *) new_uuid);

		// now check for the semaphore path
		table_path[0] = "system";
		table_path[1] = "semaphores";
		table_path[2] = new_uuid;
		table_path[3] = NULL;

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Verify Semaphore Doesn't Exist %%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// check for tmp_lookup, continue loop if it doesn't exist
		tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);
		if(tmp_lookup)
			continue;

		// add the table
		BP_HashRegAddTableByPath(NULL, table_path);

		// check for tmp lookup again after addition
		tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);

		// exit loop if we added things ok, if not return failure immediately
		if(tmp_lookup)
			break;
		else
		{
			printf("\n [!!] Error: Could not create semaphore table using a unique uuid.  This is critical.");
			BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);
		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create New Semaphore Table Using UUID %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to add data to the table
	BP_ERROR_T err_status = BP_HashRegAddDataToTableByPath
	(
		NULL,
		table_path,
		"semaphore",
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_SEMAPHORE_INFO)
	);

	// set path to /system/semaphores/uuid/semaphore
	table_path[0] = "system";
	table_path[1] = "semaphores";
	table_path[2] = new_uuid;
	table_path[3] = "semaphore";
	table_path[4] = NULL;

	// declare the lookup entry
	P_BP_HASH_TABLE_KEYED_ENTRY lookup_entry = NULL;

	// attempt to get entry
    lookup_entry = BP_HashRegLookupTableEntryByPath(NULL,table_path);

    // ensure we can lookup the new entry
    if(!lookup_entry)
    {
    	printf("\n [!!] Error: Could not lookup the semaphore entry using the provided path.  This is critical and should never occur.  Possible dangling semaphore table exists.");
    	BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);
    }

    // set semaphore from data pointer
    P_BP_SEMAPHORE_INFO semaphore = (P_BP_SEMAPHORE_INFO) lookup_entry->data;

    // ensure we have data
    if(!semaphore)
    {
    	printf("\n [!!] Error: Created semaphore entry, but the data pointer is empty.  This indicates a bad allocation and should never occur.");
    	BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);
    }


    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Fill In Semaphore Data From Hash Table Entry %%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // initialize the semaphore
    sem_init(&semaphore->sem, pshared, value);

    // set unlock
    semaphore->state = BP_SEMAPHORE_STATE_UNLOCKED;

    // copy in the uuid
    memcpy(semaphore->uuid_key, new_uuid, bpstrlen(new_uuid));

    // set the semaphore to initialized
    semaphore->sem_initialized = BP_TRUE;

    // set bpln parameters for tracking purposes (don't duplicate/don't free)
    semaphore->line = line_number;
    semaphore->func = func;
    semaphore->file = file_name;


	// return indicating success
    BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(semaphore);


}

// semaphore destroy
BP_ERROR_T BP_SemaphoreDestroy(P_BP_SEMAPHORE_INFO sem)
{


	// validate that there is a working environment
	BP_SEMAPHORE_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	// ensure we have a semaphore
	if(!sem)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Retrieve Semaphore GHT Entry %%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// now attempt to retrieve the semaphore table created in the GHT associated with this
	// managed semaphore structure.
	P_BP_HASH_TABLE_KEYED_ENTRY semaphore_entry = BP_SemaphoreGetKeyedTableEntry(sem);

	// ensure we have a semaphore entry
	if(!semaphore_entry)
		return ERR_FAILURE;

	// enter critical section
	BP_GHT_ENTER_CRITICAL_SECTION;

	// ensure the type is a named hash table
	if(semaphore_entry->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// leave critical section
	BP_GHT_LEAVE_CRITICAL_SECTION;

	// ensure the semaphore has nothing waiting on it
	P_BP_HASH_TABLE_KEYED_ENTRY lock = BP_SemaphoreLocksGetFirstLock(sem);

	// enter critical section
	BP_GHT_ENTER_CRITICAL_SECTION;

	// there should be no locks, so go ahead and check for locks
	if(lock)
	{
		printf("\n [!!] Error: Attempting to destroy a semaphore which has locks waiting.");
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% All Checks Finished, Commence Destruction %%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// only destroy initialized semaphores
	if(sem->sem_initialized == BP_TRUE)
	{

		// destroy the semaphore first
		sem_destroy(&sem->sem);

	}

	// display the semaphore entry (debug, don't uncomment)
	// printf("\n Sem entry?: %s", semaphore_entry->key);

	// attempt to destroy the table
	BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(BP_HashRegDestroyTable(NULL, semaphore_entry));

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Wait and Post %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// semaphore wait
BP_ERROR_T BP_SemaphoreWait_real(P_BP_SEMAPHORE_INFO sem, BPLN_PARMS)
{

	// validate that there is a working environment
	BP_SEMAPHORE_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	// enter critical section
	BP_GHT_ENTER_CRITICAL_SECTION;


	// ensure we have a semaphore info structure
	if(!sem)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// ensure the uuid has been set
	if(!strnlen(sem->uuid_key, 1))
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% First Check for Semaphore and Locks Table %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// create table path and zero it
	char *table_path[10];
	BP_ZERO_STACK(table_path);

	// ---- Check for Semaphore in GHT ----------------

	// leave critical section
	BP_GHT_LEAVE_CRITICAL_SECTION;


	// check to see if the semaphore exists
	BP_ERROR_T err_status = BP_SemaphoreExistsInGHT(sem);

	// enter critical section
	BP_GHT_ENTER_CRITICAL_SECTION;


	// if it doesn't exist, exit immediately
	if(err_status == ERR_FAILURE)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);


	// ---- Check for Semaphore Lock Table in GHT ----

	// leave critical section
	BP_GHT_LEAVE_CRITICAL_SECTION;

	// since we're locking, check for existence of the locks table
	err_status = BP_SemaphoreLockTableExistsInGHT(sem);

	// enter critical section
	BP_GHT_ENTER_CRITICAL_SECTION;

	// create the locks table if necessary
	if(err_status == ERR_FAILURE)
	{

		// add locks table here
		table_path[0] = "system";
		table_path[1] = "semaphores";
		table_path[2] = sem->uuid_key;
		table_path[3] = "locks";
		table_path[4] = NULL;

		// add the table
		BP_HashRegAddTableByPath(NULL, table_path);

		// enter critical section
		BP_GHT_LEAVE_CRITICAL_SECTION;

		// check for lock table after addition
		err_status = BP_SemaphoreLockTableExistsInGHT(sem);

		// enter critical section
		BP_GHT_ENTER_CRITICAL_SECTION;

		// Ensure that the table exists.  Exit if not, as it should've been
		// created with the above table add operation.
		if(err_status == ERR_FAILURE)
			BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Lock Entry to Table %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add locks table here
	table_path[0] = "system";
	table_path[1] = "semaphores";
	table_path[2] = sem->uuid_key;
	table_path[3] = "locks";
	table_path[4] = NULL;

	// create new uuid
	char new_uuid[50];
	BP_ZERO_STACK(new_uuid);

	// generate a new random uuid
	BP_GenRandomUUID_StoreInBuff(new_uuid);

	// attempt to add data to the table
	err_status = BP_HashRegAddDataToTableByPath
	(
		NULL,
		table_path,
		new_uuid,
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_SEMAPHORE_LOCK_INFO)
	);

	// set path to /system/semaphores/uuid/semaphore
	table_path[0] = "system";
	table_path[1] = "semaphores";
	table_path[2] = sem->uuid_key;
	table_path[3] = "locks";
	table_path[4] = new_uuid;
	table_path[5] = NULL;

	// declare the lookup entry
	P_BP_HASH_TABLE_KEYED_ENTRY lookup_entry = NULL;

	// attempt to get entry
	lookup_entry = BP_HashRegLookupTableEntryByPath(NULL,table_path);

	// ensure we can lookup the new entry
	if(!lookup_entry)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// set lock pointer
	P_BP_SEMAPHORE_LOCK_INFO lock = (P_BP_SEMAPHORE_LOCK_INFO) lookup_entry->data;

	// ensure we have the lock
	if(!lock)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// attempt to retrive the thread id
	lock->pthread_id = pthread_self();


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Finally Perform Semaphore Wait %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave the critical section before the wait
	BP_GHT_LEAVE_CRITICAL_SECTION;


	// switch the state to locked before waiting
	sem->state = BP_SEMAPHORE_STATE_LOCKED;

	// wait on semaphore
	sem_wait(&sem->sem);

	// return indicating success
	return ERR_SUCCESS;

}

// semaphore trywait
BP_ERROR_T BP_SemaphoreTryWait_real(P_BP_SEMAPHORE_INFO sem, BPLN_PARMS)
{

	// validate that there is a working environment
	BP_SEMAPHORE_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	// ensure we have a semaphore info structure
	if(!sem)
		return ERR_FAILURE;

	// ensure the uuid has been set
	if(!strnlen(sem->uuid_key, 1))
		return ERR_FAILURE;

	// enter critical section
	BP_GHT_ENTER_CRITICAL_SECTION;

	// Wait on semaphore immediately to prevent possibility
	// of a race condition.
	if(sem_trywait(&sem->sem) != 0)
	{

		// return immediately if we can't wait for the lock
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% First Check for Semaphore and Locks Table %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// create table path and zero it
	char *table_path[10];
	BP_ZERO_STACK(table_path);

	// check to see if the semaphore exists
	BP_ERROR_T err_status = BP_SemaphoreExistsInGHT(sem);

	// if it doesn't exist, exit immediately
	if(err_status == ERR_FAILURE)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// since we're locking, check for existence of the locks table
	err_status = BP_SemaphoreLockTableExistsInGHT(sem);

	// create the locks table if necessary
	if(err_status == ERR_FAILURE)
	{

		// add locks table here
		table_path[0] = "system";
		table_path[1] = "semaphores";
		table_path[2] = sem->uuid_key;
		table_path[3] = "locks";
		table_path[4] = NULL;

		// add the table
		BP_HashRegAddTableByPath(NULL, table_path);

		// check for lock table after addition
		err_status = BP_SemaphoreLockTableExistsInGHT(sem);

		// Ensure that the table exists.  Exit if not, as it should've been
		// created with the above table add operation.
		if(err_status == ERR_FAILURE)
			BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Lock Entry to Table %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add locks table here
	table_path[0] = "system";
	table_path[1] = "semaphores";
	table_path[2] = sem->uuid_key;
	table_path[3] = "locks";
	table_path[4] = NULL;

	// create new uuid
	char new_uuid[50];
	BP_ZERO_STACK(new_uuid);

	// generate a new random uuid
	BP_GenRandomUUID_StoreInBuff(new_uuid);

	// attempt to add data to the table
	err_status = BP_HashRegAddDataToTableByPath
	(
		NULL,
		table_path,
		new_uuid,
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_SEMAPHORE_LOCK_INFO)
	);

	// set path to /system/semaphores/uuid/semaphore
	table_path[0] = "system";
	table_path[1] = "semaphores";
	table_path[2] = sem->uuid_key;
	table_path[3] = "locks";
	table_path[4] = new_uuid;
	table_path[5] = NULL;

	// declare the lookup entry
	P_BP_HASH_TABLE_KEYED_ENTRY lookup_entry = NULL;

	// attempt to get entry
	lookup_entry = BP_HashRegLookupTableEntryByPath(NULL,table_path);

	// ensure we can lookup the new entry
	if(!lookup_entry)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// set lock pointer
	P_BP_SEMAPHORE_LOCK_INFO lock = (P_BP_SEMAPHORE_LOCK_INFO) lookup_entry->data;

	// ensure we have the lock
	if(!lock)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);


	// attempt to retrive the thread id
	lock->pthread_id = pthread_self();

	// switch the state to locked before exiting
	sem->state = BP_SEMAPHORE_STATE_LOCKED;

	// return indicating success
	BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// semaphore post
BP_ERROR_T BP_SemaphorePost_real(P_BP_SEMAPHORE_INFO sem, BPLN_PARMS)
{

	// validate that there is a working environment
	BP_SEMAPHORE_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	// ensure we have a semaphore to work with
	if(!sem)
		return ERR_FAILURE;

	// -- NOTE: Don't enter crit section at top, enter below.

	// ensure we have an entry
	P_BP_HASH_TABLE_KEYED_ENTRY sem_entry = BP_SemaphoreGetKeyedTableEntry(sem);

	// ensure we have an entry
	if(!sem_entry)
		return ERR_FAILURE;

	// attempt to check for a top lock entry
	P_BP_HASH_TABLE_KEYED_ENTRY lock_entry = BP_SemaphoreLocksGetFirstLock(sem);

	// ensure we have a lock entry
	if(!lock_entry)
		return ERR_FAILURE;



	// enter critical section at correct position after lookups
	BP_GHT_ENTER_CRITICAL_SECTION;

	// attempt to destroy the entry
	BP_ERROR_T destroy_status = BP_HashRegDestroyEntry( NULL, lock_entry );

	// exit immediately if we cannot destroy the lock entry
	if(destroy_status == ERR_FAILURE)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// now commit to sem_post
	int ret_val = sem_post(&sem->sem);

	// ensure it's not an invalid semaphore
	if(ret_val == EINVAL)
	{
		printf("\n [!!] Error: Attempting to sem_post on an invalid semaphore (or one that hasn't been initialized with sem_init).  Critical failure.");
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// ensure we aren't overflowing the value
	if(ret_val == EOVERFLOW)
	{
		printf("\n [!!] Error: Attempting to sem_post on a valid semaphore, however this post would cause an integer overflow.  Critical failure.");
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// set state to unlock
	sem->state = BP_SEMAPHORE_STATE_UNLOCKED;

	// return indicating success
	BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Retrieve Semaphore Value %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to retrive the value of a semaphore
int BP_SemGetValue_real(P_BP_SEMAPHORE_INFO sem, BPLN_PARMS)
{

	// validate that there is a working environment
	BP_SEMAPHORE_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);


	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Lookup Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// searches the GHT looking for a semaphore matching the given unique uuid
P_BP_SEMAPHORE_INFO BP_SemaphoreLookupByUUID(char *uuid)
{

	// validate that there is a working environment
	BP_SEMAPHORE_VALIDATE_WORKING_ENVIRONMENT(NULL);



	// return null on failure
	return NULL;

}

// semaphore exists
BP_ERROR_T BP_SemaphoreExistsInGHT(P_BP_SEMAPHORE_INFO sem)
{

	// validate that there is a working environment
	BP_SEMAPHORE_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	// ensure we have a semaphore info structure
	if(!sem)
		return ERR_FAILURE;



	// enter critical section
	BP_GHT_ENTER_CRITICAL_SECTION;

	// ensure the uuid has been set
	if(!strnlen(sem->uuid_key, 1))
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// leave critical section
	BP_GHT_LEAVE_CRITICAL_SECTION;

	// get keyed table entry
	P_BP_HASH_TABLE_KEYED_ENTRY err_status = BP_SemaphoreGetKeyedTableEntry(sem);

	// enter critical section
	BP_GHT_ENTER_CRITICAL_SECTION;

	// ensure we can retrieve an entry (aka it exists)
	if(!err_status)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// leave the critical section
	BP_GHT_LEAVE_CRITICAL_SECTION;

	// return indicating success
	return ERR_SUCCESS;

}


// Attempts to retrieve a keyed table entry (hash table entry) for the
// provided semaphore.
P_BP_HASH_TABLE_KEYED_ENTRY BP_SemaphoreGetKeyedTableEntry(P_BP_SEMAPHORE_INFO sem)
{

	// validate that there is a working environment
	BP_SEMAPHORE_VALIDATE_WORKING_ENVIRONMENT(NULL);

	// ensure we have a semaphore info structure
	if(!sem)
		return NULL;

	// ensure the uuid has been set
	if(!strnlen(sem->uuid_key, 1))
		return NULL;

	// enter critical section
	BP_GHT_ENTER_CRITICAL_SECTION;




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% First Check for Semaphore %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// create table path and zero it
	char *table_path[10];
	BP_ZERO_STACK(table_path);

	// now check for the semaphore path
	table_path[0] = "system";
	table_path[1] = "semaphores";
	table_path[2] = (char *)&sem->uuid_key;
	table_path[3] = NULL;

	// check for tmp_lookup, continue loop if it doesn't exist
	tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);
	if(tmp_lookup)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(tmp_lookup);

	// return NULL on failure to lookup
	BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

}

// Attempts to retrieve a semaphore lock keyed table entry (hash table entry) for the
// provided semaphore.  This returns the first semaphore only, and is used primarily
// by BP_SemaphorePost_real.
P_BP_HASH_TABLE_KEYED_ENTRY BP_SemaphoreLocksGetFirstLock(P_BP_SEMAPHORE_INFO sem)
{

	// ensure we have a semaphore info structure
	if(!sem)
		return NULL;

	// ensure the uuid has been set
	if(!strnlen(sem->uuid_key, 1))
		return NULL;

	// enter critical section
	BP_GHT_ENTER_CRITICAL_SECTION;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt Table Lookup %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// create table path and zero it
	char *table_path[10];
	BP_ZERO_STACK(table_path);

	// now check for the semaphore path
	table_path[0] = "system";
	table_path[1] = "semaphores";
	table_path[2] = sem->uuid_key;
	table_path[3] = "locks";
	table_path[4] = NULL;

	// check for tmp_lookup, continue loop if it doesn't exist
	tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);
	if(!tmp_lookup)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	// advance the tmp lookup pointer to the data position
	// so we can run our lookup in the iter loop below.
	tmp_lookup = (P_BP_HASH_TABLE_KEYED_ENTRY) tmp_lookup->data;
	if(!tmp_lookup)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	// temporary iterator values used during the HASH_ITER loop below
	P_BP_HASH_TABLE_KEYED_ENTRY entry       = NULL;
	P_BP_HASH_TABLE_KEYED_ENTRY tmp         = NULL;

	// iterate through all hashes and display
	HASH_ITER(hh, tmp_lookup, entry, tmp)
	{

		// break on first iter (aka first record)
		break;

	}

	// return the lookup
	BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(entry);

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Existence Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Simple check to see if there is a lock table for the given semaphore (lock
// table holds a cascading set of locks demonstrating which threads are waiting)
BP_ERROR_T BP_SemaphoreLockTableExistsInGHT(P_BP_SEMAPHORE_INFO sem)
{

	// validate that there is a working environment
	BP_SEMAPHORE_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	// ensure we have a semaphore info structure
	if(!sem)
		return ERR_FAILURE;

	// ensure the uuid has been set
	if(!strnlen(sem->uuid_key, 1))
		return ERR_FAILURE;


	// enter critical section
	BP_GHT_ENTER_CRITICAL_SECTION;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt Table Lookup %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// create table path and zero it
	char *table_path[10];
	BP_ZERO_STACK(table_path);

	// now check for the semaphore path
	table_path[0] = "system";
	table_path[1] = "semaphores";
	table_path[2] = sem->uuid_key;
	table_path[3] = "locks";
	table_path[4] = NULL;

	// check for tmp_lookup, continue loop if it doesn't exist
	tmp_lookup = BP_HashRegLookupTableByPath(NULL, table_path);
	if(!tmp_lookup)
		BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// return indicating success
	BP_GHT_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_SUCCESS);

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Semaphore Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Will look into /system/semaphores and display all semaphores.
BP_ERROR_T BP_DisplaySemaphores(BP_BOOL display_lock_order)
{

	// validate that there is a working environment
	BP_SEMAPHORE_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Lookup Semaphores Table %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create table path
	char * table_path[] = {"system", "semaphores", NULL};

	// attempt to lookup table
	P_BP_HASH_TABLE_KEYED_ENTRY semaphores_table = BP_HashRegLookupTableByPath
	(
		NULL,
		(BP_HASH_TABLE_TRAVERSAL_PATH) &table_path
	);

	// ensure the semaphores table exists
	if(!semaphores_table)
	{
		printf("\n [!!] Error: Could not lookup global semaphores table when attempting to display semaphores.  This is critical.");
		return ERR_FAILURE;
	}

	// ensure there's a data member, if not we have nothing to display
	if(!semaphores_table->data)
		return ERR_FAILURE;

	// advance table past the actual semaphores table
	semaphores_table = (P_BP_HASH_TABLE_KEYED_ENTRY) semaphores_table->data;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Now Display Entries %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	printf("\n [+] Displaying all GHT Semaphores found in /system/semaphores:");

	// temporary iterator values used during the HASH_ITER loop below
	P_BP_HASH_TABLE_KEYED_ENTRY entry     = NULL;
	P_BP_HASH_TABLE_KEYED_ENTRY tmp       = NULL;
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_table = NULL;



	// iterate entries
	HASH_ITER(hh, semaphores_table, entry, tmp)
	{

		// set temp table
		tmp_table = (P_BP_HASH_TABLE_KEYED_ENTRY) entry->data;

		// It's either a data entry or a hash table.  Display either based on
		// parameter preference.
		if(entry->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{

			printf("\n Got Semaphore Entry!!");

			// display the data entry
			/*
			BP_HashRegDisplayDataEntry
			(
					hreg,
					entry,
					display_tab_depth
			);
			*/

			// continue the loop
			continue;

		}
		else
		{

			// printf("\n Got Semaphore TABLE!!");

			// display the semaphore table
			BP_DisplaySemaphoreTable(entry);

			// recurse here
			// BP_HashRegDisplayTable(NULL, entry, BP_FALSE, BP_FALSE, BP_FALSE);

		}


	}

	// iterate
	printf("\n");

	// return indicating success
	return ERR_SUCCESS;

}


// DO NOT USE DIRECTLY WITHOUT UNDERSTANDING WHAT YOU'RE DOING:
// Will attempt to display a valid semaphore table within the
// semaphore table.
BP_ERROR_T BP_DisplaySemaphoreTable(P_BP_HASH_TABLE_KEYED_ENTRY semaphore_table)
{

	// validate that there is a working environment
	BP_SEMAPHORE_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	// ensure we have a table
	if(!semaphore_table)
		return ERR_FAILURE;

	// ensure the type is set correctly
	if(semaphore_table->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		return ERR_FAILURE;

	// ensure we have data
	if(!semaphore_table->data)
		return ERR_FAILURE;

	// ensure the key is a uuid
	if(BP_StringIsUUID(semaphore_table->key) == ERR_FAILURE)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Now Lookup Semaphore from Table %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// printf("\n key: %s - %u", semaphore_table->key, bpstrlen(semaphore_table->key));

	// lookup the semaphore (should always exist)
	P_BP_HASH_TABLE_KEYED_ENTRY sem_entry = BP_HashRegLookupEntryFromHashKeyViaRelativePosition(NULL, semaphore_table, "semaphore");

	// ensure we have a semaphore
	if(!sem_entry)
		return ERR_FAILURE;

	// now that we have a semaphore, we can display it
	P_BP_SEMAPHORE_INFO sem = (P_BP_SEMAPHORE_INFO) sem_entry->data;

	// display the semaphore info
	BP_DisplaySemaphore(sem);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check for Locks and Display %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// lookup the locks table (if it exists)
	P_BP_HASH_TABLE_KEYED_ENTRY locks_table = BP_HashRegLookupEntryFromHashKeyViaRelativePosition(NULL, semaphore_table, "locks");

	// if we have locks, go ahead and display them
	if(locks_table)
	{

		// ensure we have data
		if(!locks_table->data)
			return ERR_FAILURE;

		// move the locks table to it's data table (where the locks are placed)
		locks_table = (P_BP_HASH_TABLE_KEYED_ENTRY) locks_table->data;

		// temporary iterator values used during the HASH_ITER loop below
		P_BP_HASH_TABLE_KEYED_ENTRY entry     = NULL;
		P_BP_HASH_TABLE_KEYED_ENTRY tmp       = NULL;
		P_BP_HASH_TABLE_KEYED_ENTRY tmp_table = NULL;

		// set current lock info
		P_BP_SEMAPHORE_LOCK_INFO curr_lock_info = NULL;


		// iterate lock entries
		HASH_ITER(hh, locks_table, entry, tmp)
		{

			// current lock info
			curr_lock_info = (P_BP_SEMAPHORE_LOCK_INFO) entry->data;

			// ensure we have a lock
			if(!curr_lock_info)
			{
				// printf("\n [!!] Error: Lock entry exists, but it's nullified.");
				break;
			}

			// display the lock
			printf("\n\t      >>  Lock -- %s", entry->key);
			printf("\n\t             pthread_id: %u", curr_lock_info->pthread_id);
			printf("\n\t             thread_id:  %u", curr_lock_info->thread_id);


		}

	}


	// return indicating success
	return ERR_SUCCESS;

}

// Will attempt to display a valid semaphore entry in the table.
BP_ERROR_T BP_DisplaySemaphore(P_BP_SEMAPHORE_INFO sem)
{

	// validate that there is a working environment
	BP_SEMAPHORE_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	// ensure we have a semaphore
	if(!sem)
		return ERR_FAILURE;

	// display the uuid key
	printf("\n\t [+] Sem: %s", sem->uuid_key);

	// display the state
	printf("\n\t\t state:         ");
	switch(sem->state)
	{
		case BP_SEMAPHORE_STATE_LOCKED:
			printf("BP_SEMAPHORE_STATE_LOCKED");
			break;
		case BP_SEMAPHORE_STATE_MULTIPLE_LOCKS_WAITING:
			printf("BP_SEMAPHORE_STATE_MULTIPLE_LOCKS_WAITING");
			break;
		case BP_SEMAPHORE_STATE_UNLOCKED:
			printf("BP_SEMAPHORE_STATE_UNLOCKED");
			break;
		case BP_SEMAPHORE_STATE_UNSET:
			printf("BP_SEMAPHORE_STATE_UNSET");
			break;
		default:
			printf("INVALID ENTRY BADDATA");
			break;
	}

	// create local storage
	int value_store = 0;

	// retrieve the value from the current semaphore
	sem_getvalue(&sem->sem, &value_store);

	// display the current value
	printf("\n\t\t current value: %i", value_store);

	// display the declaration point
	printf("\n\t\t location:      %s:%u - %s", sem->file, sem->line, sem->func);

	// return indicating success
	return ERR_SUCCESS;

}

// Will attempt to display a semaphore by looking it up by UUID.
BP_ERROR_T BP_DisplaySemaphoreByUUID(char *uuid, BP_BOOL display_lock_order)
{

	// validate that there is a working environment
	BP_SEMAPHORE_VALIDATE_WORKING_ENVIRONMENT(ERR_FAILURE);

	// return indicating success
	return ERR_SUCCESS;

}

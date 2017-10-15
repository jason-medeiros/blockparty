#include "../../include/BP-Main.h"

/*
 *
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%%% Blockparty MySQL Connection Pooling %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 *
 * This is the C implementation of the blockparty connection pooling capability.  This code is
 * implemented in both C and C++ for the purpose of making this code available at different base
 * levels of the library functionality.  This code is available to all library portions above
 * base OS / memory profiling, and should be used where-ever possible except in non-blockparty
 * implementations.  If you are writing an application using blockparty, you should use the C++
 * implementation of connection pooling.  If you are making modifications to core components of
 * blockparty itself, it's recommended you use this code, as not only is it faster, it is more
 * basal with regard to blockparty itself.
 *
 */


// This is the connection pool constructor.  How it works, is you provide it a set of
// credentials, and it tries to create a specified number of connections.  The connections
// are then made available by petitioning the connection pool structure with the acquisition
// routines below.
P_BP_MYSQL_CONN_POOL BP_CreateMySQLConnectionPool
(
		size_t number_of_pool_entries_to_create,
		char * host,
		char * user,
		char * pass,
		char * db,
		size_t port,
		size_t delay_between_connects_in_secs
)
{

	// pool entry count is required for connect
	if(!number_of_pool_entries_to_create)
		return NULL;

	// host is required for connects
	if(!host)
		return NULL;

	// user is required for connects
	if(!user)
		return NULL;

	// pass is required for connects
	if(!pass)
		return NULL;

	// set default port to 3306 if unset
	if(!port)
		port = 3306;

	// set default connect interval to 2 seconds if unset
	if(!delay_between_connects_in_secs)
		delay_between_connects_in_secs = 2;

	// create space for the new pool structure
	P_BP_MYSQL_CONN_POOL pool = (P_BP_MYSQL_CONN_POOL) bpcalloc(sizeof(BP_MYSQL_CONN_POOL), 1);
	if(!pool)
		return NULL;

	// create a new tail queue linked list allocator
	pool->tq = BP_CreateLinkL("BP MySQL C Connection Pool", BP_TRUE);

	// initialize the pool locking semaphore to prevent thread conflicts
	sem_init(&pool->lock, 1, 1);

	// create space for each individual session, zero out memory.  Allocate
	// data within the current tail queue allocator.
	pool->entries = (P_BP_MYSQL_CONNECTION_POOL_ENTRY) tq_calloc(pool->tq, sizeof(BP_MYSQL_CONNECTION_POOL_ENTRY)*number_of_pool_entries_to_create, 1, BPLN);

	// clear everything out if we have no entries allocated
	if(!pool->entries)
	{

		// destroy the pool as it exists and return if we cannot create the connection pool
		BP_DestroyMySQLConnectionPool(pool);
		return NULL;

	}

	// create pools
	size_t n = 0;
	for(; n < number_of_pool_entries_to_create; n++)
	{

		// attempt to connect
		P_BP_MYSQL_SESSION session = BP_MySQLConnect(host, user, pass, db, port);

		// ensure we were able to create a session
		if(!session)
		{

			// destroy the pool as it exists and return if we cannot create the connection pool
			BP_DestroyMySQLConnectionPool(pool);
			return NULL;

		}


		// increment the number of total entries
		pool->entries_total++;

		// set the session within the indexed entry
		pool->entries[n].conn = session;

		// create a new random id for the pool entry
		size_t pool_entry_id = NEW_RANDOM_ID;

		// walk pool entries and ensure we got a real-deal unique id
		// for the pool entry.
		for(size_t x = 0; x < n; x++)
		{

			// ensure id doesn't already exist in the pool, if it does
			// reset the id with a new random value, reset iterator, and restart
			// loop.
			if(pool_entry_id == pool->entries[n].pool_entry_id)
			{

				// spawn a new id in
				pool_entry_id = NEW_RANDOM_ID;

				// reset iterator and continue the loop
				x = 0;
				continue;

			}

		}

		// set a new random id
		pool->entries[n].pool_entry_id = pool_entry_id;

		// initialize the new session lock
		sem_init(&pool->entries[n].lock, 1, 1);

		// sleep delay before next connect
		sleep(delay_between_connects_in_secs);

	}

	// return the connection pool
	return pool;

}

// attempts to destroy a mysql connection pool
BP_ERROR_T BP_DestroyMySQLConnectionPool(P_BP_MYSQL_CONN_POOL pool)
{

	// ensure we have a pool
	if(!pool)
		return ERR_FAILURE;

	// check to see if we have pool entries
	if(pool->entries)
	{

		// walk each entry and destroy session
		size_t n = 0;
		for(; n < pool->entries_total; n++)
		{

			// destroy the locks for each entry
			sem_destroy(&pool->entries[n].lock);

			// close each session
			BP_MySQLClose(pool->entries[n].conn);

		}

	}

	// destroy the entire tail queue (frees all dangling chunks)
	BP_DestroyLinkL(pool->tq, BP_TRUE);

	// destroy the semaphore
	sem_destroy(&pool->lock);


	// free the pool structure itself
	bpfree(pool);

	// return indicating success
	return ERR_SUCCESS;

}

// Attempts to acquire a free entry in the mysql connection pool and lock it for our use.  Will
// return NULL on failure.
P_BP_MYSQL_CONNECTION_POOL_ENTRY BP_MySQLConnectionPoolAcquireAndLock(P_BP_MYSQL_CONN_POOL pool)
{

	// check to see if we have pointer
	if(!pool)
	{
		return NULL;
	}

	// walk each entry and issue a sem_trywait till we get an available connection
	size_t n = 0;

	// Loop forever till we get a connection, but only loop through
	// connections found within the connection pool.  Infinite loop
	// is caused by reset logic at bottom of loop.
	for(n = 0; n < pool->entries_total; n++)
	{

		// try to wait
		int wait_status = sem_trywait(&pool->entries[n].lock);
		if(wait_status != 0)
		{

			// reset loop iter if we havent found a good entry
			if(pool->entries_total-1 == n)
				n = 0;

			// continue the loop
			continue;

		}

		// set the in use flag to true
		pool->entries[n].in_use = BP_TRUE;

		// increment the number of entries currently in use
		pool->entries_in_use++;

		// unlock pool
		sem_post(&pool->lock);

		// return the newly locked entry
		return &pool->entries[n];

	}

	// return no entry if one could'nt be found
	return NULL;

}

// Releases a MySQL connection pool entry, and marks it available for another thread to use.
BP_ERROR_T BP_MySQLConnectionPoolMakeAvailableAndUnlock
(
		P_BP_MYSQL_CONN_POOL pool,
		P_BP_MYSQL_CONNECTION_POOL_ENTRY entry
)
{

	// ensure we have a pointer to our pool
	if(!pool)
		return ERR_FAILURE;

	// check entry
	if(!entry)
		return ERR_FAILURE;

	// set the in use flag to false
	entry->in_use = BP_FALSE;

	// decrease the number of entries currently in use
	pool->entries_in_use--;

	// unlock entry (doesn't need a prior wait, as it's already waited on)
	sem_post(&entry->lock);

	// return indicating success
	return ERR_SUCCESS;

}

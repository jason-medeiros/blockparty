/*
 * BP_MySQLMultiThreadConnectionPool.cc
 *
 *  Created on: May 11, 2013
 *      Author: root
 */



// include main includes
#include "../../../include/BP-Main.h"

// class constructor (automatically sets up individual connections)
BP_MySQLMultiThreadConnectionPool::BP_MySQLMultiThreadConnectionPool(char *host, char *user, char *pass, char *name, size_t port, size_t number_of_connections_to_init)
{


	// null initialize connection pool data
	this->conns  = NULL;
	this->conn_n = 0;

	// set initialization status to uninitialized
	this->init_status = false;

	// initialize the garbage collector
	this->gc = BP_CreateNewGarbageCollector("MySQLi Connection Pool Garbage Collector", 1);

	// exit if we cannot create garbage collector (required)
	if(!this->gc)
		return;


	// calculate connection structure array size
	size_t conn_elems_size = number_of_connections_to_init * sizeof(BP_MYSQL_CONN_POOL_ENTRY);

	// allocate space for elements
	this->conns = (P_BP_MYSQL_CONN_POOL_ENTRY) BP_calloc(this->gc, "connection pool entries", conn_elems_size, 1);

	// if we can't allocate connections, exit
	if(!this->conns)
	{
		BP_DestroyGarbageCollector(this->gc);
		return;
	}

	// set connection size
	this->conn_n = number_of_connections_to_init;


	// now create individual connections
	size_t n = 0;
	for(; n < this->conn_n; n++)
	{

		// retry the connect
		connect_try:

		// initialize new block object
		this->conns[n].mysqli = new Block();

		// connect to database
		if(this->conns[n].mysqli->MysqlConnect(host, user, pass, name, port) == ERR_FAILURE)
		{

			// delete the object
			delete this->conns[n].mysqli;

			// nullify
			this->conns[n].mysqli = NULL;

			printf("\n [!!] Connect failed during pool creation.  Retrying in a few seconds.");
			sleep(BP_GenRandomUnsignedLong(1,5));

			// go to the connect label
			goto connect_try;

		}
		// create new thread mutex
		this->conns[n].mutex = CreateNewMutex();

	}

	// create mutex for object itself
	this->mutex = CreateNewMutex();

	// set initialization status to true
	this->init_status = true;

}

// class deconstructor (automatically cleans up class)
BP_MySQLMultiThreadConnectionPool::~BP_MySQLMultiThreadConnectionPool()
{

	// walk all connections
	size_t n = 0;
	for(; n < this->conn_n; n++)
	{

		// try to unlock the connection mutex
		TryLockMutex(this->conns[n].mutex);
		UnlockMutex(this->conns[n].mutex);
		DestroyMutex(this->conns[n].mutex, BP_TRUE);

		// destroy the connection block
		if(this->conns[n].mysqli)
			delete this->conns[n].mysqli;

	}

	// unlock the class mutex itself
	TryLockMutex(this->mutex);
	UnlockMutex(this->mutex);
	DestroyMutex(this->mutex, BP_TRUE);

	// destroy allocated chunks
	if(this->gc)
	{
		BP_GarbageCollectorFreeAllChunks(this->gc);
		BP_DestroyGarbageCollector(this->gc);
		this->gc = NULL;
	}

}

// release connection
BP_ERROR_T BP_MySQLMultiThreadConnectionPool::releaseConnection(P_BP_MYSQL_CONN_POOL_ENTRY conn_to_release)
{

	// ensure we have a non-null value
	if(!conn_to_release)
		return ERR_FAILURE;

	// attempt to unlock mutex
	while(UnlockMutex(conn_to_release->mutex) == ERR_FAILURE){}

	// return indicating success
	return ERR_SUCCESS;

}

P_BP_MYSQL_CONN_POOL_ENTRY BP_MySQLMultiThreadConnectionPool::fetchAvailableConnection()
{


	// entry
	P_BP_MYSQL_CONN_POOL_ENTRY entry = (P_BP_MYSQL_CONN_POOL_ENTRY) NULL;

	// walk objects and attempt to find one that isn't locked
	size_t n = 0;
	for(; n < this->conn_n; n++)
	{

		// return the connection object if mutex is lockable
		if(TryLockMutex(this->conns[n].mutex) == ERR_SUCCESS)
			return (P_BP_MYSQL_CONN_POOL_ENTRY) &this->conns[n];

		// continue search if we cannot lock the mutex

	}

	// return null if no connection is available
	return NULL;

}

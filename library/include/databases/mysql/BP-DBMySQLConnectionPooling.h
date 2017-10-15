#ifndef __BP_DB_MYSQL_CONNECTION_POOLING_H
#define __BP_DB_MYSQL_CONNECTION_POOLING_H

// Connection pool status
typedef enum BP_MYSQL_CONN_POOL_STATUS {
	P_BP_MYSQL_CONN_POOL_STATUS_UNSET,
	P_BP_MYSQL_CONN_POOL_STATUS_ENABLED,
	P_BP_MYSQL_CONN_POOL_STATUS_DISABLED
};

// connection pool entry
typedef struct __P_BP_MYSQL_CONNECTION_POOL_ENTRY {

	// quick flag indicating whether or not this
	// pool entry is currently in use.  0 for free, non-zero for in use.
	size_t in_use;

	// pool entry id
	size_t pool_entry_id;

	// mysql connection in pool
	P_BP_MYSQL_SESSION conn;

	// semaphore / thread safe lock
	sem_t lock;

} BP_MYSQL_CONNECTION_POOL_ENTRY, *P_BP_MYSQL_CONNECTION_POOL_ENTRY;

// main connection pool structure
typedef struct __P_BP_MYSQL_CONN_POOL {

	// semaphre / lock for connection pool
	sem_t lock;

	// tail queue memory allocator
	P_BP_LINKL tq;

	// set to unset/enabled/disabled.  Quick way to stop operations within this conn pool.
	BP_MYSQL_CONN_POOL_STATUS status;

	// entries within the mysql connection pool
	P_BP_MYSQL_CONNECTION_POOL_ENTRY entries;

	// total number of entries in the entries stack
	size_t entries_total;

	// total number of entries in use
	size_t entries_in_use;

} BP_MYSQL_CONN_POOL, *P_BP_MYSQL_CONN_POOL;


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
);

// attempts to destroy a mysql connection pool
BP_ERROR_T BP_DestroyMySQLConnectionPool(P_BP_MYSQL_CONN_POOL pool);

// Attempts to acquire a free entry in the mysql connection pool and lock it for our use.  Will
// return NULL on failure.
P_BP_MYSQL_CONNECTION_POOL_ENTRY BP_MySQLConnectionPoolAcquireAndLock(P_BP_MYSQL_CONN_POOL pool);

// Releases a MySQL connection pool entry, and marks it available for another thread to use.
BP_ERROR_T BP_MySQLConnectionPoolMakeAvailableAndUnlock
(
		P_BP_MYSQL_CONN_POOL pool,
		P_BP_MYSQL_CONNECTION_POOL_ENTRY entry
);


#endif /* __BP_DB_MYSQL_CONNECTION_POOLING_H */

/*
 * BP_MySQLMultiThreadConnectionPool.h
 *
 *  Created on: May 11, 2013
 *      Author: root
 */

#ifndef BP_MYSQLMULTITHREADCONNECTIONPOOL_H_
#define BP_MYSQLMULTITHREADCONNECTIONPOOL_H_

// structure used to maintain mysql connection pool
typedef struct __BP_MYSQL_CONN_POOL_ENTRY {

	// individual mysqli entry
	Block *mysqli;

	// mutex lock to prevent contention
	pthread_mutex_t * mutex;

} BP_MYSQL_CONN_POOL_ENTRY, *P_BP_MYSQL_CONN_POOL_ENTRY;


class BP_MySQLMultiThreadConnectionPool : BP_BaseError
{

	// connection pool
	P_BP_MYSQL_CONN_POOL_ENTRY conns;

	// number of connections established
	size_t conn_n;

	// garbage collector
	P_BP_GARBAGE_COLLECTOR_DATA gc;


public:

	// After the class constructor is run, this will tell you whether or not the connections
	// established were successful or not.
	bool init_status;

	// object mutex (only one thread can operate on the object at a time)
	pthread_mutex_t * mutex;

	// class constructor (automatically sets up individual connections)
	BP_MySQLMultiThreadConnectionPool(char *host, char *user, char *pass, char *name, size_t port, size_t number_of_connections_to_init);

	// class deconstructor (automatically cleans up class)
	~BP_MySQLMultiThreadConnectionPool();

	// fetch available connection (returns NULL on no connection available, at which point you should wait)
	P_BP_MYSQL_CONN_POOL_ENTRY fetchAvailableConnection();

	// release connection so that the connection can be used by a different thread
	BP_ERROR_T releaseConnection(P_BP_MYSQL_CONN_POOL_ENTRY conn_to_release);

};



#endif /* BP_MYSQLMULTITHREADCONNECTIONPOOL_H_ */

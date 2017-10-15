/*
 * BP-Canaries.cc
 *
 *  Created on: Dec 9, 2014
 *      Author: root
 */

// include the main blockparty include
#include "../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Canary Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// global canary entry table
BP_CANARY_ENTRY_TABLE global_canary_entry_table;

// initialization ok values
size_t global_canary_entry_table_init_ok_val = 0x34cd56ef;
size_t global_canary_entry_table_init_ok;

// temporary entry used for lookups
P_BP_CANARY_ENTRY canary_tmp_entry;

// tmp lookup address for canaries
void * canary_tmp_lookup_addr;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Allocators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// pointer to the canaries map
void *canaries_map;

// flag set if we have init the canaries map
size_t canary_map_init_ok_val = 0xcafebabe;
size_t canary_map_init_ok;

// canary lock
sem_t canary_map_lock;

// attempt to create the global map
BP_ERROR_T BP_CanariesInitGlobalMap(size_t max_canaries)
{

	// exit immediately if we're already initialized ok
	if(canary_map_init_ok_val == canary_map_init_ok)
	{
		return ERR_FAILURE;
	}

	// ensure we have a canary creation count
	if(!max_canaries)
		return ERR_FAILURE;

	// open dev zero
	int fd = open("/dev/zero", O_RDWR);

	// calculate the number of canaries to create
	size_t canaries_size = max_canaries * (sizeof(BP_CANARY_ENTRY));

	// round out the edges
	while(canaries_size % 4096)
		canaries_size++;

	// map in the data
	canaries_map = mmap
	(
		0,
		max_canaries * (sizeof(BP_CANARY_ENTRY)),
		PROT_READ|PROT_WRITE,
		MAP_PRIVATE,
		fd,
		0
	);

	// close the file descriptor
	close(fd);

	// ensure we have a canaries map
	if(!canaries_map)
		return ERR_FAILURE;

	// zero out canary memory
	memset(canaries_map, 0x00, max_canaries * (sizeof(BP_CANARY_ENTRY)));

	// init the canary lock
	BP_CANARY_TABLE_INIT_SEM(canary_map_lock);

	// return indicating success
	return ERR_SUCCESS;

}


// destroy the global map
BP_ERROR_T BP_CanariesDestroyGlobalMap()
{

	// ensure we are initialized
	if(canary_map_init_ok_val != canary_map_init_ok)
		return ERR_FAILURE;

	// destroy the canary lock
	BP_CANARY_TABLE_DESTROY_SEM(canary_map_lock);

}

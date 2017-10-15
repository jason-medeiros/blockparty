/*
 * BP_HashTable.cc
 *
 *  Created on: Aug 27, 2013
 *      Author: root
 */


// include main include file
#include "../../include/BP-Main.h"


// this is a function masquerading as a macro.  It's too much work to implement it as a pure macro.
BP_BOOL BP_HASH_TABLE_IS_LOCKED(P_BP_HASH_TABLE_REGISTRY hreg)
{

		// check to see if we have a registry
		if(!hreg)
			return BP_FALSE;

		// init lock status
		int lock_status = 0;

		// printf("\n Lock depth?!: %u", hreg->critical_section_lock_depth);

		// attempt to retrieve semaphore
		if(sem_getvalue(&hreg->critical_section_lock, &lock_status))
		{
			return BP_FALSE;
		}

		// cleave critical section
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(hreg);

		// if it's locked,
		if(!lock_status)
			return BP_TRUE;

		// if the table isn't locked, exit failure
		return BP_FALSE;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Global Registry %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// global hash table registry
BP_HASH_TABLE_REGISTRY global_hash_table_registry;

// set the global hash init value to some arbitrary static value
size_t global_hash_table_init_ok_val;
size_t global_hash_table_init_ok;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create / Destroy Hash Table Registries %%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Creates a new hash table registry.
P_BP_HASH_TABLE_REGISTRY BP_CreateHashTableRegistry()
{

	// allocate space for hash table registry
	P_BP_HASH_TABLE_REGISTRY hreg = (P_BP_HASH_TABLE_REGISTRY) calloc
	(
		sizeof(BP_HASH_TABLE_REGISTRY),
		1
	);

	// iterator for exiting
	size_t calloc_iter = 0;

	// set 10 second allocation wait time
	size_t calloc_iter_max = 10;

	// attempt reallocation
	while(!hreg)
	{

		// sleep 1 second before trying next allocation
		sleep(1);

		// attempt allocation
		hreg = (P_BP_HASH_TABLE_REGISTRY) calloc
		(
			sizeof(BP_HASH_TABLE_REGISTRY),
			1
		);

		// break the loop if the allocation succeeded
		if(hreg)
			break;

		// increment the iterator
		calloc_iter++;

		// break if we hit our allocation cap
		if(calloc_iter == calloc_iter_max)
			break;

	}

	// trigger breakpoint on fail if necessary
	if(!hreg)
	{
		if(BP_DEBUG_BREAKPOINT_ON_ALLOC_FAIL)
		{
			printf("\n [!!] BP_CreateHashTableRegistry failed to calloc() after waiting 10 seconds.  This is unreasonable behavior and indicates that your system is unable to handle the thoroughput being thrown at it.");
			asm("int3");
		}
		return NULL;
	}

	// initialize the hash table semaphore
	BP_HASH_TABLE_INIT_SEM(hreg->lock);

	// initialize the critical section lock
	BP_HASH_TABLE_INIT_SEM(hreg->critical_section_lock);

	// return indicating success
	return hreg;

}

// Destroys a hash table registry, removing all associated allocations.
BP_ERROR_T BP_DestroyHashTableRegistry(P_BP_HASH_TABLE_REGISTRY hreg)
{


	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// flag indicating whether or not this is the singleton global hash table
	BP_BOOL is_ght = BP_FALSE;

	// if the hreg is marked specifically as global, indicate it here.
	if(hreg->is_global == BP_TRUE)
		is_ght = BP_TRUE;

	// attempt to acquire the hive semaphore
	BP_HASH_TABLE_LOCK_SEM(hreg->lock);

	// destroy hive if set
	if(hreg->hive)
	{

		// attempt to destroy the registry hive first
		BP_HashRegDestroyHive(hreg);

		// NOTE: Since the hive is a recursing set of structures, you don't
		//       need to destroy the hreg->hive pointer.

	}

	// destroy memory table if set
	if(hreg->memory_table)
	{

		// purge any dangling memory entries
		BP_HashRegDeleteAllMemoryEntries(hreg);

		// destroy the memory table
		free(hreg->memory_table);
		hreg->memory_table = NULL;

	}
	// attempt to unlock
	BP_HASH_TABLE_UNLOCK_SEM(hreg->lock);

	// attempt to lock the table
	BP_HASH_TABLE_DESTROY_SEM(hreg->lock);

	// if it's not the GHT we can free it
	if(!is_ght)
	{
		free(hreg);
		hreg = NULL;
	}

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Global Registry Gettr/Settrs %%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// simple routine to get the address of the global registry singleton (prevents
// having to extern structures)
P_BP_HASH_TABLE_REGISTRY BP_HashRegGetAddrGlobalRegistrySingleton()
{

	// only return the table if it's been initialized ok
	if(BP_GLOBAL_HASH_TABLE_INIT_OK)
		return &global_hash_table_registry;

	// return the address of the registry
	return NULL;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Path Display Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// simply displays a key path to standard out
BP_ERROR_T BP_HashRegDisplayKeyPath
(
	BP_HASH_TABLE_TRAVERSAL_PATH table_path
)
{

	// ensure we have a path
	if(!table_path)
		return ERR_FAILURE;
	if(!table_path[0])
		return ERR_FAILURE;

	// set path index
	size_t path_idx = 0;

	// walk path items and display
	for
	(
		;
		(path_idx < BP_HASH_REG_MAX_TABLE_DEPTH);
		path_idx++
	)
	{

		// exit immediately if necessary
		if(!table_path[path_idx])
			break;

		// display the path item
		printf(" > %s", table_path[path_idx]);

	}

	// return indicating success
	return ERR_SUCCESS;

}




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Keyed Entry To Path Reverse Lookups %%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempts to get a path query from a keyed entry by performing a reverse lookup
// on the entry.
char * BP_HashRegEntryToPathQuery(P_BP_HASH_TABLE_KEYED_ENTRY entry)
{


	// ensure we have an entry
	if(!entry)
		return NULL;

	// attempt to lookup the path directly
	BP_HASH_TABLE_TRAVERSAL_PATH lookup_path = BP_HashRegEntryToPath(entry);

	// ensure we could lookup our table
	if(!lookup_path)
		return NULL;

	// path query to return
	char * path_query = BP_HashRegPathArrayToStringLookup((char **) lookup_path);

	// ensure we have a path query
	if(!path_query)
	{
		BP_DestroyPointerArrayNullTerminated((void **) lookup_path);
		bpfree(lookup_path);
		return NULL;
	}

	// destroy the lookup path
	BP_DestroyPointerArrayNullTerminated((void **) lookup_path);
	bpfree(lookup_path);

	// return the path query
	return path_query;

}

// attemmpts to convert a hash entry to path
BP_HASH_TABLE_TRAVERSAL_PATH BP_HashRegEntryToPath(P_BP_HASH_TABLE_KEYED_ENTRY entry)
{

	// ensure we have an entry and a key
	if(!entry)
		return NULL;
	if(!entry->key)
		return NULL;

	// ensure the index passes filter checks
	if(!BP_HashRegPathArrayIndexFilterTest(entry->key, bpstrlen(entry->key)))
		return NULL;

	// reverse entry iterator
	P_BP_HTKE reverse_iter_entry = NULL;

	// parent index count (used for dynamic stack allocation)
	int parent_idx_n = 0;

	// check to see that we have a parent
	if(entry->parent)
	{

		// fast loop to reverse-traverse through the list
		for
		(
			reverse_iter_entry = entry;
			reverse_iter_entry;
			reverse_iter_entry = (P_BP_HTKE) reverse_iter_entry->parent
		)
		{
			parent_idx_n++;
			if(!reverse_iter_entry->parent)
				break;
		}

	}
	else
	{

		// if we are the top entry (aka no-parent
		reverse_iter_entry = entry;
		parent_idx_n++;

	}


	// run count and pointer checks
	if(!parent_idx_n)
		return NULL;
	if(!reverse_iter_entry)
		return NULL;

	// allocate space for table (returned from routine, add extra entry for null ptr)
	char ** table_path = (char **) bpcalloc
	(
		sizeof(char *) * (parent_idx_n + 1),
		1
	);

	// -- if the index count is 1, we can just add the key and return quick

	if(parent_idx_n == 1)
	{

		// set the key
		table_path[0] = bpstrdup(entry->key);
		table_path[1] = NULL;

		// return the filled out path
		return table_path;
	}

	// -----

	// if the index count is greater than 1, we have to reverse lookup
	// again (cannot forward lookup due to the potential for multiple
	// lookup paths).

	// fast loop to reverse-traverse through the list
	for
	(
		reverse_iter_entry = entry;
		reverse_iter_entry;
		reverse_iter_entry = (P_BP_HTKE) reverse_iter_entry->parent
	)
	{

		// subtract from the parent index on loop
		parent_idx_n--;

		// never go negative
		if(parent_idx_n < 0)
			break;

		// check table path
		table_path[parent_idx_n] = bpstrdup(reverse_iter_entry->key);

	}

	// return the filled out table path
	return table_path;

}




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Manipulate Tables Within Registries %%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Add a table to the registry by a provided path.
BP_ERROR_T BP_HashRegAddTable
(
	P_BP_HASH_TABLE_REGISTRY hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY table_to_append_to,
	P_BP_HASH_TABLE_KEYED_ENTRY new_table_entry
)
{

	// handle the key
	// printf("\n WatWa12: %p - %p", new_table_entry, table_to_append_to);

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// if there is no append table,
	//if(!table_to_append_to)
	//	table_to_append_to = hreg->hive;



	// ensure we have a new table entry
	if(!new_table_entry)
		return ERR_FAILURE;

	// ensure the addition is a key type
	if(!new_table_entry->key)
		return ERR_FAILURE;




	// if there is no table to append to, we're using the hive (and it's empty), so go ahead and
	// use the direct hive pointer here.
	if(!table_to_append_to)
	{

		// add to the main hive if there is nothing to append to
		HASH_ADD_KEYPTR
		(
			hh,
			hreg->hive,
			new_table_entry->key,
			bpstrlen(new_table_entry->key),
			new_table_entry
		);

		// Note: No data reset needs to occur because we're using hreg->hive directly here.

	}
	else
	{

		// set the data pointer so data can be appended
		P_BP_HASH_TABLE_KEYED_ENTRY table_to_append_to_data = (P_BP_HASH_TABLE_KEYED_ENTRY) table_to_append_to->data;

		// add to the table
		HASH_ADD_KEYPTR
		(
			hh,
			table_to_append_to_data,
			new_table_entry->key,
			bpstrlen(new_table_entry->key),
			new_table_entry
		);

		// reset data pointer to reflect current situation
		table_to_append_to->data = (void *) table_to_append_to_data;

	}

	// increase the total number of tables in the hive
	hreg->total_tables_in_hive++;

	// return indicating success
	return ERR_SUCCESS;

}

// Add a table to the registry by a provided path.
BP_ERROR_T BP_HashRegAddTableByPath
(
	P_BP_HASH_TABLE_REGISTRY     hreg,
	BP_HASH_TABLE_TRAVERSAL_PATH table_path
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// ensure we have a key to add
	if(!table_path)
		return ERR_FAILURE;
	if(!table_path[0])
		return ERR_FAILURE;

	// count the number of entries in the table
	size_t path_entry_count = BP_CountNullTerminatedPtrArray(table_path);

	// need at least one element
	if(!path_entry_count)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Perform Preliminary Lookup (prevents overwrites) %%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to lookup the table (check if it already exists)
	P_BP_HASH_TABLE_KEYED_ENTRY table_lookup = BP_HashRegLookupTableByPath(	hreg, table_path );

	// if we found the table, exit immediately as we cannot add something that
	// already exists.
	if(table_lookup)
	{

		// printf("\n Path Here1?: %s - ", table_lookup->key);
		// BP_HashRegDisplayKeyPath(table_path);

		// prevent duplicate tables
		return ERR_FAILURE;

	}

	// printf("\n Sheeeeeet: %u ", path_entry_count);

	// Now lookup table parent (for forward-backward linking).  Linking in the
	// table parent allows for very easy record removal/deletion.

	// used to save a position in the path which is nullified during check below.
	char * saved_path_position = NULL;

	// the actual table parent
	P_BP_HASH_TABLE_KEYED_ENTRY table_parent = NULL;

	// only set parent if we have a path count.
	if(path_entry_count)
	{

		// only attempt to lookup parent if a parent could possibly exist.
		if(path_entry_count-1)
		{

			// preserve path
			saved_path_position = table_path[path_entry_count-1];

			// overwrite last path position
			table_path[path_entry_count-1] = NULL;

			// attempt to lookup parent at this point
			table_parent = BP_HashRegLookupTableByPath(	hreg, table_path );

			// printf("\n Null Failure At: ");
			// BP_HashRegDisplayKeyPath(table_path);

			// restore saved path position (before potential exit)
			table_path[path_entry_count-1] = saved_path_position;


			// a table parent is required for proper linking (path indicative), if no parent
			// is set, return immediately.
			if(!table_parent)
				return ERR_FAILURE;



		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Entry %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// allocate new entry
	P_BP_HASH_TABLE_KEYED_ENTRY new_entry = (P_BP_HASH_TABLE_KEYED_ENTRY) BP_HashRegAddMemoryEntry
	(
		hreg,
		BP_HASH_REG_MEM_ENTRY_TYPE_KEYED_ENTRY_IN_HIVE,
		sizeof(BP_HASH_TABLE_KEYED_ENTRY),
		BPLN
	);

	// initialize the semaphore
	BP_HASH_TABLE_INIT_SEM(new_entry->lock);

	// set the key here -- local allocation (DO NOT CHANGE
	// FROM STRDUP to BPSTRDUP)
	new_entry->key = strdup(table_path[path_entry_count-1]);

	// add new table to the entries after it has been resized
	new_entry->data =  NULL;

	// set the entry type as a hash table
	new_entry->type = BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE;

	// set the parent pointer
	new_entry->parent = (void *) table_parent;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Perform Table Addition %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create var to hold return status
	BP_ERROR_T ret_status = ERR_FAILURE;

	// call the actual add routine
	ret_status = BP_HashRegAddTable
	(
		hreg,
		table_parent,
		new_entry
	);

	// printf("\n Gets Here: %s - ", ret_status == ERR_FAILURE ? "FAIL" : "SUCCESS");
	// BP_HashRegDisplayKeyPath(table_path);
	// printf("\n");

	// return indicating success
	return ERR_SUCCESS;

}


// Add a table to the registry by a provided path.
BP_ERROR_T BP_HashRegAddTableByPathQuery
(
	P_BP_HASH_TABLE_REGISTRY hreg,
	char *lookup_query
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	if(!lookup_query)
		return ERR_FAILURE;

	// validate that string is reasonable/printable
	if(!BP_StringIsReasonablePrintableString(lookup_query, bpstrlen(lookup_query), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// Create path array from lookup query.  This function will fail
	// if the lookup query cannot be parsed (bad characters etc)
	char ** path_lookup_array = BP_HashRegStringLookupToPathArray(lookup_query, bpstrlen(lookup_query));

	// if we can't convert the path, exit with failure code
	if(!path_lookup_array)
		return ERR_FAILURE;
	if(!path_lookup_array[0])
	{
		bpfree(path_lookup_array);
		return ERR_FAILURE;
	}

	// display the path lookup array
	printf("\n Path Lookup: '%s'\n", lookup_query);
	BP_DisplayStringArray(" path_lookup_arr[]: ", 1, 1, path_lookup_array);

	// Add a table to the registry by a provided path.
	BP_ERROR_T ret_code = BP_HashRegAddTableByPath
	(
		hreg,
		path_lookup_array
	);

	// destroy the pointer array before exiting
	BP_DestroyPointerArrayNullTerminated((void **) path_lookup_array);

	// destroy the array structure itself
	bpfree(path_lookup_array);

	// return with the addition return code
	return ret_code;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Entry/Table Lookup Routines %%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Will search a traversal path for a table.  Returns NULL if the table cannot be found. Type
// must be BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE.
P_BP_HASH_TABLE_KEYED_ENTRY BP_HashRegLookupTableByPath
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	BP_HASH_TABLE_TRAVERSAL_PATH  table_path
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(NULL);

	// ensure we have a search path
	if(!table_path)
		return NULL;
	if(!table_path[0])
		return NULL;

	// count the number of entries in the table
	size_t path_entry_count = BP_CountNullTerminatedPtrArray(table_path);
	if(!path_entry_count)
	{
		return NULL;
	}

	// local storage point for lookup
	P_BP_HASH_TABLE_KEYED_ENTRY found_table = NULL;

	// iterator for the path entry positions
	size_t path_entry_iter = 0;


	// The current lookup position within the lookup loop below.  Used for discovering
	// nested tables.
	P_BP_HASH_TABLE_KEYED_ENTRY lookup_position = hreg->hive;
	if(!lookup_position)
	{
		return NULL;
	}

	// attempt to lookup the table by path
	for
	(
		;
		path_entry_iter < path_entry_count && lookup_position;
		path_entry_iter++
	)
	{

		// ensure we have our hash table values or exit immediately (hh is a static
		// structure and will always exist so long as lookup_position) is a structure
		// itself.
		if(!lookup_position->hh.tbl)
		{
			return NULL;
		}

		// reset the found table on loop
		found_table = NULL;

		// attempt to lookup the table
		HASH_FIND
		(
			hh,
			lookup_position,
			table_path[path_entry_iter],
			bpstrlen(table_path[path_entry_iter]),
			found_table
		);

		// if our lookup fails we can immediately exit the lookup
		if(!found_table)
			return NULL;


		// the type of the entry must be table.  If it's not, it's likely an abstract
		// type entry rather than a hash table entry.  Imagine it being like the record found
		// was a file in a directory, rather than a directory itself.
		if(found_table->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
			return NULL;

		// if the entry count is only 1 deep, we can just return the table itself
		if(path_entry_count == 1)
			return found_table;


		// exit if we're in the correct position and we have a table entry
		if(path_entry_iter == (path_entry_count-1))
		{

			// printf("\n At lookup position?!");
			// printf("\n Lookup Table?: %s - %p: ", found_table->key, found_table->data);
			// BP_HashRegDisplayKeyPath(table_path);

			// return the found table here
			return found_table;

		}

		// advance the lookup position
		lookup_position = (P_BP_HASH_TABLE_KEYED_ENTRY) found_table->data;

		// advance the lookup position
		if(!lookup_position)
			break;

	}


	// return the found table if it was found at the correct
	// depth.
	if(path_entry_iter == (path_entry_count-1))
		return found_table;

	// if no table was found at the correct depth, go ahead and exit
	return NULL;

}


// Will attempt to lookup a hash table entry with a char * representation
// of a hash lookup (eg. "/table_1/table_2".  This is mostly for convienence
// as this funciton simply parses the lookup query and calls BP_HashRegLookupTableByPath.
P_BP_HASH_TABLE_KEYED_ENTRY BP_HashRegLookupTableByPathQuery
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	char *                        table_path_lookup_query
)
{

	// do not do hreg check here, it is done via macro in the function which is called below

	// ensure we have a lookup query set
	if(!table_path_lookup_query)
		return NULL;

	// ensure lookup query is reasonable/printable string
	if(!BP_StringIsReasonablePrintableString(table_path_lookup_query, bpstrlen(table_path_lookup_query), BP_FALSE, BP_FALSE))
		return NULL;

	// create path array from lookup query
	char ** path_lookup_array = BP_HashRegStringLookupToPathArray(table_path_lookup_query, bpstrlen(table_path_lookup_query));

	// check to ensure we've created a lookup array
	if(!path_lookup_array)
		return NULL;

	// ensure we have a first element
	if(!path_lookup_array[0])
	{
		bpfree(path_lookup_array);
		return NULL;
	}

	// gather lookup query
	P_BP_HASH_TABLE_KEYED_ENTRY lookup_entry = BP_HashRegLookupTableByPath(hreg, path_lookup_array);
	if(!lookup_entry)
	{
		BP_DestroyPointerArrayNullTerminated((void **) path_lookup_array);
		bpfree(path_lookup_array);
		return NULL;
	}

	// destroy the path lookup array entries
	BP_DestroyPointerArrayNullTerminated((void **) path_lookup_array);

	// destroy array structure
	bpfree(path_lookup_array);

	// return the lookup entry
	return lookup_entry;

}

// Will search a traversal path for a table.  Returns NULL if the table cannot be found.  Type must not
// be a table.
P_BP_HASH_TABLE_KEYED_ENTRY BP_HashRegLookupTableEntryByPath
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	BP_HASH_TABLE_TRAVERSAL_PATH  entry_path
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(NULL);

	// ensure we have a search path
	if(!entry_path)
		return NULL;
	if(!entry_path[0])
		return NULL;

	// count the number of entries in the table
	size_t path_entry_count = BP_CountNullTerminatedPtrArray(entry_path);
	if(!path_entry_count)
	{
		return NULL;
	}

	// local storage point for lookup (not just table, contains final entry as well)
	P_BP_HASH_TABLE_KEYED_ENTRY found_table = NULL;

	// iterator for the path entry positions
	size_t path_entry_iter = 0;


	// The current lookup position within the lookup loop below.  Used for discovering
	// nested tables.
	P_BP_HASH_TABLE_KEYED_ENTRY lookup_position = hreg->hive;
	if(!lookup_position)
	{
		return NULL;
	}

	// attempt to lookup the table by path
	for
	(
		;
		path_entry_iter < path_entry_count && lookup_position;
		path_entry_iter++
	)
	{


		// ensure we have our hash table values or exit immediately (hh is a static
		// structure and will always exist so long as lookup_position) is a structure
		// itself.
		if(!lookup_position->hh.tbl)
			return NULL;

		// reset the found table on loop
		found_table = NULL;

		// attempt to lookup the table
		HASH_FIND
		(
			hh,
			lookup_position,
			entry_path[path_entry_iter],
			bpstrlen(entry_path[path_entry_iter]),
			found_table
		);

		// if our lookup fails we can immediately exit the lookup
		if(!found_table)
			return NULL;

		// if the entry count is only 1 deep, we can just return the table itself
		if(path_entry_iter == (path_entry_count-1))
		{
			if(found_table->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
				return found_table;
			else
				return NULL;
		}



		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Advance Table %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// the type of the entry must be table.  If it's not, it's likely an abstract
		// type entry rather than a hash table entry.  Imagine it being like the record found
		// was a file in a directory, rather than a directory itself.
		if(found_table->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
			return NULL;

		// advance the lookup position
		lookup_position = (P_BP_HASH_TABLE_KEYED_ENTRY) found_table->data;

		// advance the lookup position
		if(!lookup_position)
			break;



	}

	// return the found table if it was found at the correct
	// depth.
	if(path_entry_iter == (path_entry_count-1))
		return found_table;

	// if no table was found at the correct depth, go ahead and exit
	return NULL;

}

// Will search a traversal path and return an ENTRY based on a path query.
P_BP_HASH_TABLE_KEYED_ENTRY BP_HashRegLookupTableEntryByPathQuery
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	char *                        entry_path_lookup_query
)
{

	// do not do hreg check here, it is done via macro in the function which is called below

	// ensure we have a lookup query
	if(!entry_path_lookup_query)
		return NULL;

	// ensure lookup query is reasonable/printable string
	if(!BP_StringIsReasonablePrintableString(entry_path_lookup_query, bpstrlen(entry_path_lookup_query), BP_FALSE, BP_FALSE))
		return NULL;

	// create path array from lookup query
	char ** path_lookup_array = BP_HashRegStringLookupToPathArray(entry_path_lookup_query, bpstrlen(entry_path_lookup_query));

	// check to ensure we've created a lookup array
	if(!path_lookup_array)
		return NULL;

	// ensure we have a first element
	if(!path_lookup_array[0])
	{
		bpfree(path_lookup_array);
		return NULL;
	}

	// gather lookup query
	P_BP_HASH_TABLE_KEYED_ENTRY lookup_entry = BP_HashRegLookupTableEntryByPath(hreg, path_lookup_array);
	if(!lookup_entry)
	{
		BP_DestroyPointerArrayNullTerminated((void **) path_lookup_array);
		bpfree(path_lookup_array);
		return NULL;
	}

	// destroy the path lookup array entries
	BP_DestroyPointerArrayNullTerminated((void **) path_lookup_array);

	// destroy array structure
	bpfree(path_lookup_array);

	// return the lookup entry
	return lookup_entry;

}


// --- relative lookups by path/path_query -----------



// Will search a traversal path and return an ENTRY based on a path query.
P_BP_HASH_TABLE_KEYED_ENTRY BP_HashRegLookupTableEntryByDataKeyIndexAndTablePath
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	BP_HASH_TABLE_TRAVERSAL_PATH  table_path,
	char *                        data_index_key
)
{

	// basic null checks
	if(!table_path)
		return NULL;
	if(!data_index_key)
		return NULL;

	// ensure we have a first element
	if(!table_path[0])
	{
		return NULL;
	}


	// -- calculate size and allocate memory -----

	// count the pointer array
	size_t ptr_array_size = BP_CountNullTerminatedPtrArray(table_path);

	// allocate space for the lookup (table size + extra ptr + null)
	char ** tmp_path_lookup_array = (char **) bpcalloc
	(
		sizeof(char *) * (ptr_array_size+2),
		1
	);

	// exit if memory allocation failed
	if(!tmp_path_lookup_array)
		return NULL;


	// -- copy in strings for lookup ----------

	// copy in elements
	size_t table_idx = 0;
	for(; table_path[table_idx]; table_idx++)
	{
		tmp_path_lookup_array[table_idx] = bpstrdup(table_path[table_idx]);
	}

	// append data index and add key
	tmp_path_lookup_array[ptr_array_size]   = bpstrdup(data_index_key);
	tmp_path_lookup_array[ptr_array_size+1] = NULL;


	// -- attempt lookup -------------------

	// gather lookup query
	P_BP_HASH_TABLE_KEYED_ENTRY lookup_entry = BP_HashRegLookupTableEntryByPath(hreg, tmp_path_lookup_array);
	if(!lookup_entry)
	{
		BP_DestroyPointerArrayNullTerminated((void **) tmp_path_lookup_array);
		bpfree(tmp_path_lookup_array);
		return NULL;
	}

	// -- destroy allocated data and exit

	// destroy the path lookup array entries
	BP_DestroyPointerArrayNullTerminated((void **) tmp_path_lookup_array);

	// destroy array structure
	bpfree(tmp_path_lookup_array);

	// return the looked up entry
	return lookup_entry;

}


// Will search a traversal path and return an ENTRY based on a path query.
P_BP_HASH_TABLE_KEYED_ENTRY BP_HashRegLookupTableEntryByDataKeyIndexAndTablePathQuery
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	char *                        table_path_lookup_query,
	char *                        data_index_key
)
{

	// basic null checks
	if(!table_path_lookup_query)
		return NULL;
	if(!data_index_key)
		return NULL;

	// run basic sanity checks
	if(!BP_StringIsReasonablePrintableString(table_path_lookup_query, bpstrlen(table_path_lookup_query), BP_FALSE, BP_FALSE))
		return NULL;
	if(!BP_StringIsReasonablePrintableString(data_index_key, bpstrlen(data_index_key), BP_FALSE, BP_FALSE))
		return NULL;


	// create path array from lookup query
	char ** path_lookup_array = BP_HashRegStringLookupToPathArray(table_path_lookup_query, bpstrlen(table_path_lookup_query));

	// check to ensure we've created a lookup array
	if(!path_lookup_array)
		return NULL;

	// ensure we have a first element
	if(!path_lookup_array[0])
	{
		bpfree(path_lookup_array);
		return NULL;
	}

	// count the pointer array
	size_t ptr_array_size = BP_CountNullTerminatedPtrArray(path_lookup_array);

	// since it's bpcalloced, it can be bprealloced here without issue to add an extra parameter
	path_lookup_array = (char **) bprealloc
	(
			path_lookup_array,
			sizeof(char *) * (ptr_array_size+2)
	);

	// append data index and add key
	path_lookup_array[ptr_array_size]   = bpstrdup(data_index_key);
	path_lookup_array[ptr_array_size+1] = NULL;

	// gather lookup query
	P_BP_HASH_TABLE_KEYED_ENTRY lookup_entry = BP_HashRegLookupTableEntryByPath(hreg, path_lookup_array);
	if(!lookup_entry)
	{
		BP_DestroyPointerArrayNullTerminated((void **) path_lookup_array);
		bpfree(path_lookup_array);
		return NULL;
	}

	// destroy the path lookup array entries
	BP_DestroyPointerArrayNullTerminated((void **) path_lookup_array);

	// destroy array structure
	bpfree(path_lookup_array);

	// return the looked up entry
	return lookup_entry;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Relative Entry/Table Lookups %%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// These routines utilize a P_BP_HASH_TABLE_KEYED_ENTRY table as a
// first parameter rather than a hreg.  This allows for localized
// table lookups for hashes.  The second parameter is a key to
// use for the lookup.
P_BP_HASH_TABLE_KEYED_ENTRY BP_HashRegLookupEntryFromHashKeyViaRelativePosition
(
		P_BP_HASH_TABLE_REGISTRY      hreg,
		P_BP_HASH_TABLE_KEYED_ENTRY relative_position,
		char *key
)
{


	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(NULL);

	// ensure we have a relative position
	if(!relative_position)
		return NULL;

	// ensure the position is a table
	if(relative_position->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		return NULL;

	// ensure we have data to search through
	if(!relative_position->data)
		return NULL;

	// ensure we have a key
	if(!key)
		return NULL;

	// ensure string is reasonable/printable ascii
	if(BP_StringIsReasonablePrintableString(key, bpstrlen(key), BP_FALSE, BP_FALSE) == ERR_FAILURE)
		return NULL;

	// set entries
	P_BP_HASH_TABLE_KEYED_ENTRY entries = (P_BP_HASH_TABLE_KEYED_ENTRY) relative_position->data;

	// stores the found lookup
	P_BP_HASH_TABLE_KEYED_ENTRY found_entry = NULL;

	// attempt to lookup the table
	HASH_FIND
	(
		hh,
		entries,
		key,
		bpstrlen(key),
		found_entry
	);


	// Run final validation checks.
	if(found_entry)
	{

		// first ensure that the entry itself is valid
		if(!BP_HashRegMemPtrValid(hreg, found_entry))
		{
			printf("\n Exiting because found entry was bad?");
			return NULL;
		}

		// now, if there is data, make sure the data is valid
		if(found_entry->data)
		{

			if(!BP_HashRegMemPtrValid(hreg, found_entry->data))
			{
				printf("\n Exiting because found DATA was bad?: %p", found_entry->data);
				return NULL;
			}

		}

	}

	// if we have an entry, return it immediately
	if(found_entry)
		return found_entry;

	// return NULL if can't be found
	return NULL;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Create Paths from Entries %%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to create a reasonable string from a keyed entry
// by crawling backwards and joining keys.
char * BP_HashRegCreateReasonableStringPathFromKeyedEntry
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY   relative_position
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(NULL);

	if(!hreg)
		return NULL;
	if(!relative_position)
		return NULL;
	if(!relative_position->key)
		return NULL;

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(hreg);


	// this is used for preserving a pointer on loop (for tracking memory for destroy)
	char * joined_str_loop_preserve = NULL;

	// join string array
	char * join_str_arr[10] = {0};
	size_t join_str_arr_n   = 0;

	// joined string is the actual composite string
	char * joined_str = NULL;

	// holds the top path when it's found
	P_BP_HASH_TABLE_KEYED_ENTRY path_top = NULL;

	// attempt to retrieve the top entry
	P_BP_HASH_TABLE_KEYED_ENTRY relative_position_iter = relative_position;
	for
	(
			;
			relative_position_iter;
			relative_position_iter = (P_BP_HASH_TABLE_KEYED_ENTRY) relative_position_iter->parent
	)
	{

		// if we don't have a joined string, allocate from dup
		if(!joined_str)
		{

			// set from start position key if necessary
			joined_str = BP_JoinStrings("/", relative_position_iter->key);

		}
		else
		{

			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Join String %%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// reset string array
			join_str_arr_n = 0;

			// append slash
			join_str_arr[join_str_arr_n] = "/";
			join_str_arr_n++;

			// add key
			join_str_arr[join_str_arr_n] = relative_position_iter->key;
			join_str_arr_n++;

			// prefix with the joining string
			join_str_arr[join_str_arr_n] = joined_str;
			join_str_arr_n++;

			// terminate the array
			join_str_arr[join_str_arr_n] = NULL;

			// join string array
			joined_str_loop_preserve = BP_JoinStringArray(join_str_arr, join_str_arr_n, NULL);

			// destroy old memory
			if(joined_str)
				bpfree(joined_str);

			// ensure we have preserved a pointer
			if(joined_str_loop_preserve)
			{

				// update pointer
				joined_str = joined_str_loop_preserve;

			}

		}


		// -- end if else ------------

		// ensure we have a relative position
		if(!relative_position_iter->parent)
		{
			break;
		}

	} // loop-end

	// destroy preserve if necessary
	if(joined_str_loop_preserve)
	if(joined_str_loop_preserve != joined_str)
		bpfree(joined_str_loop_preserve);


	// set the return string (needs to be allocated on the hash table)
	char * ret_str = NULL;
	if(joined_str)
	{

		// duplicate the path within the registry
		ret_str = ht_strdup(hreg, joined_str);


		// destroy joined string
		bpfree(joined_str);
		joined_str = NULL;

	}

	// printf("\n");
	// printf("\n Path Iter Joined Hash Table?: %p:%s", ret_str, ret_str);
	// printf("\n Path Iter Joined normal?:     %s", joined_str);
	// printf("\n");


	// leave the critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(hreg);

	// return the joined string
	return ret_str;

}


// create path from keyed entry
char ** BP_HashRegCreatePathFromKeyedEntry
(
	P_BP_HASH_TABLE_REGISTRY    hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY relative_position
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(NULL);

	if(!hreg)
		return NULL;
	if(!relative_position)
		return NULL;
	if(!relative_position->key)
		return NULL;

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(hreg);

	// create the initial path
	char *str_path = BP_HashRegCreateReasonableStringPathFromKeyedEntry
	(
		hreg,
		relative_position
	);

	// exit if path could not be created
	if(!str_path)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(hreg, NULL);

	// create path array
	char ** ret_path_array = BP_HashRegStringLookupToPathArray(str_path, bpstrlen(str_path));

	// destroy the string path
	ht_free(hreg, str_path);

	// leave the critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(hreg, ret_path_array);

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Checkout/Checkin Routines %%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to checkout a data entry.  This is different than checking out a table in that
// a data entry has no possibility of having children to recurse into.
BP_ERROR_T BP_HashRegCheckOutData
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY   data,
	BP_BOOL                       block_till_checkout_established
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// ensure we have data
	if(!data)
		return ERR_FAILURE;


	// block until the checkout has been established
	if(block_till_checkout_established == BP_TRUE)
	{

		// blocking semaphore lock (critical section)
		BP_HASH_TABLE_LOCK_SEM(data->lock);

	}
	else
	{

		// check that semaphore is locked
		if
		(
			BP_HASH_TABLE_TRYLOCK_SEM(data->lock) == ERR_SUCCESS
		)
		{

			// return indicating success
			return ERR_SUCCESS;

		}
		else
		{

			// return indicating failure
			return ERR_FAILURE;

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}


// Attempts to checkout a hash table at a provided path.  The third parameter
// determines whether or not the routine will attempt to block until locking
// the table (checking it out) completes.
BP_ERROR_T BP_HashRegCheckOutTable
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY   table,
	BP_BOOL                       block_till_checkout_established
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// return indicating failure
	if(!table)
		return ERR_FAILURE;

	// ensure the table provided is the correct type
	if(table->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		return ERR_FAILURE;

	// tmp value required by lock check macro
	int tmp_val = 0;

	// Indicator to describe if the parent table is locked before
	// recursing into children.
	BP_BOOL parent_locked_ok = BP_FALSE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Lock Table %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// block until the checkout has been established
	if(block_till_checkout_established == BP_TRUE)
	{

		// blocking semaphore lock (critical section)
		BP_HASH_TABLE_LOCK_SEM(table->lock);

		// set the lock flag to true
		parent_locked_ok = BP_TRUE;

	}
	else
	{

		// check that semaphore is locked
		if
		(
			BP_HASH_TABLE_TRYLOCK_SEM(table->lock) == ERR_SUCCESS
		)
		{
			parent_locked_ok = BP_TRUE;
		}
		else
		{

			// return indicating failure
			return ERR_FAILURE;

		}

	}

	// NOTE: Should only ever get to this position if the table is locked.

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Exit Immediately if No Data %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if there is no data, check lock status and return
	// accordingly.
	if(!table->data)
	{

		// ensure the parent is locked ok
		if(parent_locked_ok == BP_TRUE)
			return ERR_SUCCESS;
		else
			return ERR_FAILURE;

	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Cascading Pointer %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// cascading hash table
	P_BP_HASH_TABLE_KEYED_ENTRY entry_table   = (P_BP_HASH_TABLE_KEYED_ENTRY) table->data;




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Recursively Lock Children %%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// temporary iterator values used during the HASH_ITER loop below
	P_BP_HASH_TABLE_KEYED_ENTRY entry       = NULL;
	P_BP_HASH_TABLE_KEYED_ENTRY tmp         = NULL;
	P_BP_HASH_TABLE_KEYED_ENTRY sub_table   = NULL;

	// status for last data and table locks
	BP_ERROR_T data_lock_status  = ERR_FAILURE;
	BP_ERROR_T table_lock_status = ERR_FAILURE;

	// iterate through all hashes and display
	HASH_ITER(hh, entry_table, entry, tmp)
	{

		// always skip self references
		if(entry == table)
			continue;

		// reset lock status on loop
		data_lock_status  = ERR_FAILURE;
		table_lock_status = ERR_FAILURE;

		// It's either a data entry or a hash table.  If it's data, just lock it.  If it's a table,
		// check out the tables member elements.
		if(entry->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{

			// attempt to checkout data entry
			data_lock_status = BP_HashRegCheckOutData
			(
				hreg,
				entry,
				block_till_checkout_established
			);

		}
		else
		{

			// attempt to checkout the table recursively
			table_lock_status = BP_HashRegCheckOutTable
			(
				hreg,
				entry,
				block_till_checkout_established
			);

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}


// Attempts to checkout a hash table at a provided path.  The third parameter
// determines whether or not the routine will attempt to block until locking
// the table (checking it out) completes.
BP_ERROR_T BP_HashRegCheckOutTableByPath
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	BP_HASH_TABLE_TRAVERSAL_PATH  table_path,
	BP_BOOL                       block_till_checkout_established
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// ensure we have a table path to work with
	if(!table_path)
		return ERR_FAILURE;
	if(!table_path[0])
		return ERR_FAILURE;

	// attempt to lookup the table
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = BP_HashRegLookupTableByPath(hreg, table_path);
	if(!tmp_lookup)
		return ERR_FAILURE;

	// attempt to (recursively) checkout the table after lookup
	return BP_HashRegCheckOutTable
	(
		hreg,
		tmp_lookup,
		block_till_checkout_established
	);

}

// Attempt to Check-In a singular data entry.
BP_ERROR_T BP_HashRegCheckInData
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY   data
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// ensure we have data
	if(!data)
		return ERR_FAILURE;

	// blocking semaphore lock (critical section)
	if(BP_HASH_TABLE_UNLOCK_SEM(data->lock) == ERR_FAILURE)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// Attempts to check in a table which has already been checked out.
BP_ERROR_T BP_HashRegCheckInTable
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY   table
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// return indicating failure
	if(!table)
		return ERR_FAILURE;

	// ensure the table provided is the correct type
	if(table->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		return ERR_FAILURE;



	// check that the semaphore is unlocked
	int tmp_val = 0;
	BP_HASH_TABLE_SEM_IS_LOCKED(table->lock, tmp_val);

	// if the table is already unlocked, return failure immediately (1 is unlocked)
	if(tmp_val == 1)
	{
		return ERR_FAILURE;
	}

	// Indicator to describe if the parent table is locked before
	// recursing into children.
	BP_BOOL parent_unlocked_ok = BP_FALSE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Begin Table Checkin %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if there is no data, check lock status and return
	// accordingly.
	if(table->data)
	{

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Set Cascading Pointer %%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// cascading hash table
		P_BP_HASH_TABLE_KEYED_ENTRY entry_table   = (P_BP_HASH_TABLE_KEYED_ENTRY) table->data;


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Attempt to Recursively Lock Children %%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// temporary iterator values used during the HASH_ITER loop below
		P_BP_HASH_TABLE_KEYED_ENTRY entry       = NULL;
		P_BP_HASH_TABLE_KEYED_ENTRY tmp         = NULL;
		P_BP_HASH_TABLE_KEYED_ENTRY sub_table   = NULL;

		// status for last data and table locks
		BP_ERROR_T data_lock_status  = ERR_FAILURE;
		BP_ERROR_T table_lock_status = ERR_FAILURE;

		// iterate through all hashes and display
		HASH_ITER(hh, entry_table, entry, tmp)
		{

			// always skip self references
			if(entry == table)
				continue;

			// reset lock status on loop
			data_lock_status  = ERR_FAILURE;
			table_lock_status = ERR_FAILURE;

			// It's either a data entry or a hash table.  If it's data, just lock it.  If it's a table,
			// check out the tables member elements.
			if(entry->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
			{

				// attempt to checkin the data entry
				data_lock_status = BP_HashRegCheckInData
				(
					hreg,
					entry
				);

				// exit if the lock failed
				if(data_lock_status == ERR_FAILURE)
					return ERR_FAILURE;

			}
			else
			{

				// attempt to checkin the table recursively
				table_lock_status = BP_HashRegCheckInTable
				(
					hreg,
					entry
				);

				// exit if the lock failed
				if(table_lock_status == ERR_FAILURE)
					return ERR_FAILURE;

			}

		}

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Unlock Table %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to unlock the semaphore
	if(BP_HASH_TABLE_UNLOCK_SEM(table->lock) == ERR_FAILURE)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// Attempts to check in a table which has already been checked out, by path.
BP_ERROR_T BP_HashRegCheckInTableByPath
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	BP_HASH_TABLE_TRAVERSAL_PATH  table_path
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// ensure we have a table path to work with
	if(!table_path)
		return ERR_FAILURE;
	if(!table_path[0])
		return ERR_FAILURE;

	// attempt to lookup the table
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = BP_HashRegLookupTableByPath(hreg, table_path);
	if(!tmp_lookup)
		return ERR_FAILURE;

	// attempt to checkin the table
	return BP_HashRegCheckInTable(hreg, tmp_lookup);

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Destroy Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// ==============================================================
// DONT CALL THIS, THIS IS AN INTERNAL ROUTINE THAT SHOULN'T BE
// USED UNLESS YOU KNOW WHAT YOU'RE DOING.
// ==============================================================
// This destroys an individual table object.  This phyically undoes the link within
// the table structure.  This is called by BP_HashRegDestroyTable_r and should not
// be called directly.  This routine does NOT recurse, it simply destroys one single
// element.  The element being destroyed should NOT have children by the time it's
// being deleted.
BP_ERROR_T BP_HashRegDestroyKeyedEntryTable
(
	P_BP_HASH_TABLE_REGISTRY    hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY table
)
{

	// Macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// Ensure we have an entry
	if(!table)
		return ERR_FAILURE;

	// Attempt to lookup pointer in the memory table.
	P_BP_HASH_TABLE_MEM_PTR_ENTRY found_entry = BP_HashRegMemPtrFind
	(
		hreg,
		(void *) table
	);

	// ensure we have a found entry
	if(!found_entry)
		return ERR_FAILURE;

	// ensure we have a data pointer
	if(table->data)
		return ERR_FAILURE;

	// set/cast table parent
	P_BP_HASH_TABLE_KEYED_ENTRY table_parent = (P_BP_HASH_TABLE_KEYED_ENTRY) table->parent;


	// printf("\n Getting here?!: %s", table->key);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Delete Entry and Reassign Data %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(table_parent)
	{

		// table parent
		P_BP_HASH_TABLE_KEYED_ENTRY table_parent_dir = (P_BP_HASH_TABLE_KEYED_ENTRY) table_parent->data;

		// free key if its set
		if(table->key)
		{
			free(table->key);
			table->key = NULL;
		}

		if(table->data)
		{
			free(table->data);
			table->data = NULL;
		}

		// attempt to remove the entry from the parent table
		HASH_DEL(table_parent_dir, table);

		// reset data
		table_parent->data = table_parent_dir;

	}
	else
	{


		// free key if its set
		if(table->key)
		{
			free(table->key);
			table->key = NULL;
		}

		if(table->data)
		{
			free(table->data);
			table->data = NULL;
		}

		// attempt to remove the entry from top level of the hive
		HASH_DEL(hreg->hive, table);

	}


	// delete the memory entry associated with the table after the
	// record is removed from the hive.
	BP_HashRegDeleteMemoryEntry
	(
		hreg,
		(P_BP_HASH_TABLE_MEM_PTR_ENTRY) found_entry
	);

	// return indicating success
	return ERR_SUCCESS;
}

// ==============================================================
// DONT CALL THIS, THIS IS AN INTERNAL ROUTINE THAT SHOULN'T BE
// USED UNLESS YOU KNOW WHAT YOU'RE DOING.
// ==============================================================
// This destroys an individual entry and attempts to remove it from a table.  This should NEVER be called
// unless the record is locked safely first.  This is called by BP_HashRegDestroyTable_r and
// should not be called directly.
BP_ERROR_T BP_HashRegDestroyKeyedEntryData
(
	P_BP_HASH_TABLE_REGISTRY    hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY entry
)
{

	// Macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// Ensure we have an entry
	if(!entry)
		return ERR_FAILURE;

	// exit immediately if we have a table
	if(entry->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		return ERR_FAILURE;

	// sub table/data
	P_BP_HASH_TABLE_KEYED_ENTRY parent_table = (P_BP_HASH_TABLE_KEYED_ENTRY) entry->parent;
	if(!parent_table)
		return ERR_FAILURE;

	// Attempt to lookup pointer in the memory table.
	P_BP_HASH_TABLE_MEM_PTR_ENTRY found_entry = BP_HashRegMemPtrFind
	(
		hreg,
		(void *) entry
	);

	// ensure we have a found entry
	if(!found_entry)
		return ERR_FAILURE;


	// create reference to the parent table data (should never be null unless things are wacky)
	P_BP_HASH_TABLE_KEYED_ENTRY parent_table_data = (P_BP_HASH_TABLE_KEYED_ENTRY) parent_table->data;
	if(!parent_table_data)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Delete Entry and Reassign Data %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// verbosity
	// printf("\n [!] Deleting %p (%s) from %p - entry: %p - %s - %p", entry, entry->key, parent_table, found_entry, parent_table->key, parent_table_data);




	// remove the entry
	HASH_DEL(parent_table_data, entry);

	// set the data element in the parent to the modified hash table
	parent_table->data = parent_table_data;

	// attempt to unlock the semaphore.  If it can't be unlocked,
	// it means the record was not checked out successfully.  This is
	// bad behavior and should be investigated if it occurs.
	if(BP_HASH_TABLE_UNLOCK_SEM(entry->lock) == ERR_FAILURE)
		return ERR_FAILURE;

	// destroy the lock
	BP_HASH_TABLE_DESTROY_SEM(entry->lock);


	// if data is set, process it
	if(entry->data)
	{

		switch(entry->type)
		{
			case BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB:
			case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING:
			case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UNICODE16_STRING:
			case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UNICODE32_STRING:
			case BP_HASH_TABLE_KEYED_ENTRY_TYPE_SIZE_T:
			case BP_HASH_TABLE_KEYED_ENTRY_TYPE_SIGNED_INT:
			case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_JSON:

				BP_HashRegDeleteMemoryEntry(hreg, found_entry);
				hreg->total_data_entries_in_hive--;

				// free(entry->data);
				// entry->data = NULL;
				break;

			default:
				break;
		}

	}

	// return indicating success
	return ERR_SUCCESS;

}



// This is the recursing version of the destroy table routine.  This is necessary
// because we have to assure we can checkout a full directory before deleting the
// directory.  The BP_HashRegDestroyTable routine performs the initial table checkout,
// and this routine recurses into the table and unlocks/deletes members
// of the table.  This routine should never be called directly unless
// first ensuring that the table they're calling this with is checked out
// beforehand.
BP_ERROR_T BP_HashRegDestroyTable_r
(
	P_BP_HASH_TABLE_REGISTRY    hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY table
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// return indicating failure
	if(!table)
		return ERR_FAILURE;

	// ensure the table provided is the correct type
	if(table->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		return ERR_FAILURE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Begin Table Recursion %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if there is no data, check lock status and return
	// accordingly.
	if(table->data)
	{

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Set Cascading Pointer %%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// cascading hash table
		P_BP_HASH_TABLE_KEYED_ENTRY entry_table   = (P_BP_HASH_TABLE_KEYED_ENTRY) table->data;


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Attempt to Recursively Lock Children %%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// temporary iterator values used during the HASH_ITER loop below
		P_BP_HASH_TABLE_KEYED_ENTRY entry       = NULL;
		P_BP_HASH_TABLE_KEYED_ENTRY tmp         = NULL;
		P_BP_HASH_TABLE_KEYED_ENTRY sub_table   = NULL;

		// status for last data and table locks
		BP_ERROR_T data_destroy_status  = ERR_FAILURE;
		BP_ERROR_T table_destroy_status = ERR_FAILURE;

		// iterate through all hashes and display
		HASH_ITER(hh, entry_table, entry, tmp)
		{

			// always skip self references
			if(entry == table)
				continue;

			// reset lock status on loop
			data_destroy_status  = ERR_FAILURE;
			table_destroy_status = ERR_FAILURE;

			// It's either a data entry or a hash table.  If it's data, just lock it.  If it's a table,
			// check out the tables member elements.
			if(entry->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
			{

				// printf("\n Destroying data!: %s", entry->key);

				// destroy the keyed entry at this position
				data_destroy_status = BP_HashRegDestroyKeyedEntryData
				(
					hreg,
					entry
				);

				// exit if the lock failed
				if(data_destroy_status == ERR_FAILURE)
				{

					// printf("\n [!!] Failed to destroy data.");
					return ERR_FAILURE;

				}

			}
			else
			{


				// printf("\n Destroying table!: %s", entry->key);

				// recurse and destroy lower tables/data
				table_destroy_status = BP_HashRegDestroyTable_r
				(
					hreg,
					entry
				);

				// exit if the destruction fails
				if(table_destroy_status == ERR_FAILURE)
				{
					// printf("\n Failed to destroy: %s", entry->key);
					return ERR_FAILURE;
				}


			}

		}

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Unlock Table %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	char * destroyed_mem = (char *) strdup(table->key);

	// destroy the keyed entry; cascade outward from innermost (aka first/deepest return is fisrt delete)
	if( BP_HashRegDestroyKeyedEntryTable(hreg,table) == ERR_FAILURE)
	{
		// printf("\n [!!] Failed to destroy tables keyed entry: %s", destroyed_mem);
		free(destroyed_mem);
		return ERR_FAILURE;
	}
	else
	{
		// printf("\n [+] Success destroying tables keyed entry: %s", destroyed_mem);
		free(destroyed_mem);
	}
	/**/

	// decrease the total number of tables in the hive
	hreg->total_tables_in_hive--;

	// return indicating success
	return ERR_SUCCESS;

}



// Attempts to destroy the table at the element pointer, recursing
// into the path and destroying all data recursively inward till
// finally destroying itself.  This requires that everything in the
// hive is unlocked, or the request will fail with ERR_FAILURE. This
// is primarily called via BP_HashRegDestroyTableByPath.
BP_ERROR_T BP_HashRegDestroyTable
(
	P_BP_HASH_TABLE_REGISTRY hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY table
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// ensure we have a table
	if(!table)
		return ERR_FAILURE;

	// Ensure we can check out the table before continuing.  Since checkouts cascade
	if(BP_HashRegCheckOutTable(hreg, table, BP_TRUE) != ERR_SUCCESS)
	{
		return ERR_FAILURE;
	}

	// ensure it's actually a table and not just some random
	// pointer.
	if(table->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		return ERR_FAILURE;

	// verbosity
	// printf("\n Destroying table %p ", table);

	// run the recursive table destruction routine
	return BP_HashRegDestroyTable_r
	(
		hreg,
		table
	);

}




// Attempts to checkout a registry and seek a table within it.  Within that
// table, it will try to remove the provided entry parameter.
BP_ERROR_T BP_HashRegDestroyEntry
(
	P_BP_HASH_TABLE_REGISTRY    hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY entry_within_table_to_destroy
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// create reference to the parent table
	P_BP_HASH_TABLE_KEYED_ENTRY parent_table = (P_BP_HASH_TABLE_KEYED_ENTRY) entry_within_table_to_destroy->parent;

	// esnure we have a parent table to work with
	if(!parent_table)
		return ERR_FAILURE;


	// Ensure we can check in the table before continuing.  Since checkouts cascade
	if(BP_HashRegCheckOutTable(hreg, parent_table, BP_TRUE) != ERR_SUCCESS)
	{
		return ERR_FAILURE;
	}


	// check entry type, make sure it's not a table
	if(entry_within_table_to_destroy->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		return ERR_FAILURE;


	// destroy the keyed entry at this position
	BP_ERROR_T data_destroy_status = BP_HashRegDestroyKeyedEntryData
	(
		hreg,
		entry_within_table_to_destroy
	);

	// exit if the lock failed
	if(data_destroy_status == ERR_FAILURE)
	{

		// printf("\n [!!] Failed to destroy data.");
		return ERR_FAILURE;

	}

	// Ensure we can check in the table before exiting.
	if(BP_HashRegCheckInTable(hreg, parent_table) != ERR_SUCCESS)
	{
		return ERR_FAILURE;
	}


	// return indicating success
	return ERR_SUCCESS;

}


// --- Destroy By Path ---------------------------


// Attempts to destroy the table at the current path, recursing
// into the path and destroying all data recursively inward till
// finally destroying itself.  This requires that everything in the
// hive is unlocked, or the request will fail with ERR_FAILURE.
BP_ERROR_T BP_HashRegDestroyTableByPath
(
	P_BP_HASH_TABLE_REGISTRY hreg,
	BP_HASH_TABLE_TRAVERSAL_PATH table_path
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// ensure we have a table path to work with
	if(!table_path)
		return ERR_FAILURE;
	if(!table_path[0])
		return ERR_FAILURE;

	// attempt to lookup the table
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = BP_HashRegLookupTableByPath(hreg, table_path);
	if(!tmp_lookup)
	{
		return ERR_FAILURE;
	}

	// destroy the table by path
	return BP_HashRegDestroyTable(hreg, tmp_lookup);

}


// Attempts to destroy a table entry by a provided path.
BP_ERROR_T BP_HashRegDestroyEntryByPath
(
	P_BP_HASH_TABLE_REGISTRY     hreg,
	BP_HASH_TABLE_TRAVERSAL_PATH table_path
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// ensure we have a table path to work with
	if(!table_path)
		return ERR_FAILURE;
	if(!table_path[0])
		return ERR_FAILURE;


	// attempt to lookup the table
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = BP_HashRegLookupTableEntryByPath(hreg, table_path);
	if(!tmp_lookup)
		return ERR_FAILURE;


	// destroy entry
	return BP_HashRegDestroyEntry
	(
		hreg,
		tmp_lookup
	);

}



// --- Destroy Entire Hive ----------------------

// Attempts to destroy an entire hive table by table.
BP_ERROR_T BP_HashRegDestroyHive
(
	P_BP_HASH_TABLE_REGISTRY hreg
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// ensure there is a hive to destroy
	if(!hreg->hive)
		return ERR_FAILURE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Recursively Lock Children %%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// temporary iterator values used during the HASH_ITER loop below
	P_BP_HASH_TABLE_KEYED_ENTRY entry       = NULL;
	P_BP_HASH_TABLE_KEYED_ENTRY tmp         = NULL;
	P_BP_HASH_TABLE_KEYED_ENTRY sub_table   = NULL;

	// status for last data and table locks
	BP_ERROR_T data_destroy_status  = ERR_FAILURE;
	BP_ERROR_T table_destroy_status = ERR_FAILURE;

	// iterate through all hashes and display
	HASH_ITER(hh, hreg->hive, entry, tmp)
	{

		// reset lock status on loop
		data_destroy_status  = ERR_FAILURE;
		table_destroy_status = ERR_FAILURE;

		// It's either a data entry or a hash table.  If it's data, just lock it.  If it's a table,
		// check out the tables member elements.
		if(entry->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{

			// recurse and destroy lower tables/data
			table_destroy_status = BP_HashRegDestroyTable_r
			(
				hreg,
				entry
			);

			// exit if the destruction fails
			if(table_destroy_status == ERR_FAILURE)
			{
				// printf("\n Ugh.  Failed to destroy: %s", entry->key);
				return ERR_FAILURE;
			}

		}

	}


	// NOTE: hive should be null at this point

	// if all tables were deleted ok, return indicating so
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Convert To/From HREG String Paths %%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Examples of String Paths:
//
// "/some_key/some_key/some_key/item"
// "/key1/key2/key3/item1"
//
// NOTE: These routines are only to be used on items which
//       do not have the special character "/" in them. Strings
//       must be reasonable/printable and not contain any line delimiting
//       chars.

// This turns a char ** into a path lookup string.  The resulting
// string is bpcalloced and must be bpfreed.
char * BP_HashRegPathArrayToStringLookup(char **path)
{

	// basic null zero checks
	if(!path)
		return NULL;
	if(!path[0])
		return NULL;

	// count the path to determine total items in the array
	size_t path_element_n = BP_CountNullTerminatedPtrArray(path);

	// run all elements through filter test before creating joined string
	size_t path_idx = 0;
	for(; path_idx < path_element_n; path_idx++)
	{

		// get pointer to path index string
		char *path_idx_str = path[path_idx];
		if(!path_idx_str)
		{
			// printf("\n PATH ITER WAS NULL?");
			return NULL;
		}

		// check to see if index will pass the filter test
		if(BP_HashRegPathArrayIndexFilterTest(path_idx_str, bpstrlen(path_idx_str)) == ERR_FAILURE)
		{
			// printf("\n ARRAY INDEX FAILED FILTER");
			return NULL;
		}

	}

	// create temp tq
	P_BP_LINKL temp_tq = BP_CreateLinkL(".", BP_TRUE);

	// once filters are passed, build actual string
	for(path_idx = 0; path_idx < path_element_n; path_idx++)
	{

		// add prefixing slash
		tq_strndup(temp_tq, "/", 1, BPLN);

		// add target index
		tq_strndup(temp_tq, path[path_idx], bpstrlen(path[path_idx]), BPLN);

	}

	// flatten the temporary tailqueue to create our final path string
	BP_LinkLFlattenJoinStringElements(temp_tq);

	// if a path was created, return it here
	if(temp_tq->flat_buff)
	{

		// duplicate the flat buff as the final path
		char *ret_lookup = bpstrndup((char *) temp_tq->flat_buff, temp_tq->flat_buff_n);

		// destroy linkl before exiting
		BP_DestroyLinkL(temp_tq, BP_TRUE);

		// return the final buffer
		return ret_lookup;

	}

	// destroy linkl before exiting
	BP_DestroyLinkL(temp_tq, BP_TRUE);

	// if we were unable create a path for some other reason, exit with null
	return NULL;

}

// Tests to see if a hash registry index passes filter tests
BP_ERROR_T BP_HashRegPathArrayIndexFilterTest(char *index_str, size_t index_str_len)
{

	// basic zero/null checks
	if(!index_str)
		return ERR_FAILURE;
	if(!index_str_len)
		return ERR_FAILURE;

	// create ascii configuration
	BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION ascii_config;

	// zero out the structure (automagic macro)
	BP_ZERO_STACK(ascii_config);

	// set some flags in the structure (default is BP_FALSE aka 0)
	ascii_config.count_ascii                      = BP_TRUE;
	ascii_config.count_extended                   = BP_TRUE;
	ascii_config.count_individual_characters      = BP_TRUE;


	// attempt to generate analysis
	P_BP_BUFFER_ANALYSIS buff_analysis = BP_CreateBufferAnalysis
	(
		(unsigned char *) index_str,
		(size_t)          index_str_len,
		&ascii_config
	);


	// this is where the filter index will stop searching
	unsigned char filter_char_stop_idx = 0xff;

	// this is where the filter iter starts
	unsigned char filter_iter = 0x00;



	// loop through all filter combinations and identify if we have ANY bad characters
	for
	(
		;
		filter_iter < filter_char_stop_idx;
		filter_iter++
	)
	{

		// dbg display
		// printf("\n filter_iter: %u | filter_char_stop_idx: %u", filter_iter, filter_char_stop_idx);

		// allow filter bypass for 0 - 9
		if(filter_iter >= '0' && filter_iter <= '9')
			continue;

		// skip past A-Z
		if(filter_iter >= 'A' && filter_iter <= 'Z')
			continue;

		// skip past a-z
		if(filter_iter >= 'a' && filter_iter <= 'z')
			continue;

		// perform special character allow exceptions
		switch((char) filter_iter)
		{

			// skip all allowed chars
			case '!':
			case '#':
			case '$':
			case '&':
			case '(':
			case ')':
			case '{':
			case '}':
			case ' ':
			case '_':
			case '-':
			case '.':
			case '~':
			case '=':
			case '`':
			case '\'':
			case '"':
			case '\\':
				continue;
				break;

			default:
				break;

		}

		// if we have a disallowed filter, destroy analysis and exit
		if(buff_analysis->ascii->char_counts[filter_iter])
		{
			// printf("\n Filter Failed on: %s", index_str);
			BP_DestroyBufferAnalysis(buff_analysis);
			return ERR_FAILURE;
		}

	}

	// destroy the buffer analysis
	BP_DestroyBufferAnalysis(buff_analysis);

	// return indicating that the key has passed the filter checks
	return ERR_SUCCESS;

}


// This turns a char * "/lookup/path/index" styled string into a
// char ** array which is usable for path lookups.  The result is a bpcalloced
// array and must be free'd using BP_DestroyPtrArray.
char ** BP_HashRegStringLookupToPathArray(char *lookup, size_t lookup_len)
{

	// basic checks
	if(!lookup)
		return NULL;
	if(!lookup_len)
		return NULL;

	// lookups cannot end in a forward slash
	if(lookup[lookup_len-1] == '/')
		return NULL;

	// all path lookups must begin with a forward slash
	if(lookup[0] != '/')
		return NULL;

	// create ascii configuration
	BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION ascii_config;

	// zero out the structure (automagic macro)
	BP_ZERO_STACK(ascii_config);

	// set some flags in the structure (default is BP_FALSE aka 0)
	ascii_config.count_ascii                      = BP_TRUE;
	ascii_config.count_extended                   = BP_TRUE;
	ascii_config.count_individual_characters      = BP_TRUE;


	// create ret path
	char ** ret_path = NULL;

	// attempt to generate analysis
	P_BP_BUFFER_ANALYSIS buff_analysis = BP_CreateBufferAnalysis
	(
		(unsigned char *) lookup,
		(size_t)          lookup_len,
		&ascii_config
	);



	// this is where the filter index will stop searching
	unsigned char filter_char_stop_idx = 0xff;

	// this is where the filter iter starts
	unsigned char filter_iter = 0x00;

	// loop through all filter combinations and identify if we have ANY bad characters
	for
	(
		;
		filter_iter < filter_char_stop_idx;
		filter_iter++
	)
	{

		// dbg display
		// printf("\n filter_iter: %u | filter_char_stop_idx: %u", filter_iter, filter_char_stop_idx);

		// allow filter bypass for 0 - 9
		if(filter_iter >= '0' && filter_iter <= '9')
			continue;

		// skip past A-Z
		if(filter_iter >= 'A' && filter_iter <= 'Z')
			continue;

		// skip past a-z
		if(filter_iter >= 'a' && filter_iter <= 'z')
			continue;




		// perform special character allow exceptions
		switch((char) filter_iter)
		{

			// skip all allowed chars
			case '!':
			case '#':
			case '$':
			case '&':
			case '(':
			case ')':
			case '{':
			case '}':
			case ' ':
			case '_':
			case '-':
			case '~':
			case '/':
			case '`':
			case '\'':
			case '"':
			case '\\':
			case '>':
			case '<':
			case '*':
			case '^':
			case ',':
			case '.':
			case '?':
			case '=':
			case ':':
			case ';':
			case '|':
				continue;
				break;

			default:
				break;

		}

		// if we have a disallowed filter, destroy analysis and exit
		if(buff_analysis->ascii->char_counts[filter_iter])
		{
			BP_DestroyBufferAnalysis(buff_analysis);
			return NULL;
		}

	}

	// lookup bad path potentials
	if(memmem(lookup, lookup_len, "//", 2))
	{
		BP_DestroyBufferAnalysis(buff_analysis);
		return NULL;
	}

	// path cannot be "/"
	if(buff_analysis->ascii->char_counts['/'] == 1 && lookup_len == 1)
	{
		BP_DestroyBufferAnalysis(buff_analysis);
		return NULL;
	}


	// declare path buffer
	char ** path_buff = NULL;

	// if we have a shallow (first iter) path, just create the array directly
	if
	(
		buff_analysis->ascii->char_counts['/'] == 1 &&
		lookup[0] == '/'
	)
	{

		// create path iter with two entries (last will be null)
		path_buff = (char **) bpcalloc(sizeof(char *) * 2, 1);

		// set path and always truncate
		path_buff[0] = (char *) bpstrdup(&lookup[1]);
		path_buff[1] = NULL;

		// destroy the buffer analysis
		BP_DestroyBufferAnalysis(buff_analysis);

		// return the path buffer
		return path_buff;

	}
	else if(buff_analysis->ascii->char_counts['/'] > 1)
	{
		// --- create path by exploding the lookup buffer along / deliniated boundaries

		// now attempt to split the buffer (max depth is 255).  Offset past the first "/" to prevent
		// a empty string "" entry from appearing due to Explode() rules.
		path_buff = BP_ExplodeBufferBin((char *) &lookup[1], lookup_len-1, "/", bpstrlen("/"), 255);

		// destroy the buffer analysis
		BP_DestroyBufferAnalysis(buff_analysis);

		// return the parsed out path array
		return path_buff;
	}

	// destroy the buffer analysis
	BP_DestroyBufferAnalysis(buff_analysis);

	// return null if the parse buff cannot be filled for some reason
	return NULL;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Add Data To Tables %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This links arbitrary data types into the table.  These data types
// can be allocated elsewhere
BP_ERROR_T BP_HashRegAddDataToTable
(
		P_BP_HASH_TABLE_REGISTRY       hreg,
		P_BP_HASH_TABLE_KEYED_ENTRY    table,
		char *                         key,
		BP_HASH_TABLE_KEYED_ENTRY_TYPE data_type,
		void *                         data,
		size_t                         data_size
)
{



	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// esnure we have a table
	if(!table)
		return ERR_FAILURE;
	if(!key)
		return ERR_FAILURE;

	// ensure string is reasonable printable
	if(BP_StringIsReasonablePrintableString(key, bpstrlen(key), BP_FALSE, BP_FALSE) == ERR_FAILURE)
		return ERR_FAILURE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Determine Key Uniqueness %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the search table
	P_BP_HASH_TABLE_KEYED_ENTRY search_table = (P_BP_HASH_TABLE_KEYED_ENTRY) table->data;
	P_BP_HASH_TABLE_KEYED_ENTRY found_item   = NULL;

	// check to see that hash doesn't exist
	HASH_FIND
	(
		hh,
		search_table,
		key,
		bpstrlen(key),
		found_item
	);

	// if an item was found with this key, exit
	if(found_item)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Data To Table if Unique %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add to the table
	P_BP_HASH_TABLE_KEYED_ENTRY add_to_table = (P_BP_HASH_TABLE_KEYED_ENTRY) table->data;

	// allocate new entry
	P_BP_HASH_TABLE_KEYED_ENTRY new_entry = (P_BP_HASH_TABLE_KEYED_ENTRY) BP_HashRegAddMemoryEntry
	(
		hreg,
		BP_HASH_REG_MEM_ENTRY_TYPE_KEYED_ENTRY_IN_HIVE,
		sizeof(BP_HASH_TABLE_KEYED_ENTRY),
		BPLN
	);

	// ensure entry was created before continuing
	if(!new_entry)
		return ERR_FAILURE;

	// initialize the hash table semaphore
	BP_HASH_TABLE_INIT_SEM(new_entry->lock);

	// set the parent pointer to the current table
	new_entry->parent = (void *) table;

	// set key as dup key
	new_entry->key = ht_strdup(hreg, key);


	// switch on the passed in data type, behavior differs based on unmanaged or
	// managed types.
	switch(data_type)
	{

		// Externally managed data
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_POINTER_EXTERNALLY_MANAGED: /* this type is unmanaged, meaning no free occurs to the data involved. */
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_C_DATASTRUCTURE_EXTERNALLY_MANAGED:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_CPP_CLASS_POINTER_EXTERNALLY_MANAGED:

			// set data (null is ok)
			new_entry->data = (void *) data;
			new_entry->type = data_type;
			break;

		// Managed types are allocated and copied to the memory hash table
		// of the hash registry, and are deleted when the hash registry
		// is destroyed.
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UNICODE16_STRING:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UNICODE32_STRING:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_SIZE_T:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_SIGNED_INT:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_JSON:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_MESSAGE_TO_WORKER:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_MESSAGE_FROM_WORKER:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_EVENT_UNPROCESSED:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_EVENT_PROCESSED:

			// allocate space for data (in memory entries)
			new_entry->data = (void *)BP_HashRegAddMemoryEntry
			(
					hreg,
					BP_HASH_REG_MEM_ENTRY_TYPE_GENERIC_ALLOCATION,
					data_size+1,
					BPLN
			);

			// exit application immediately if we can't allocate any memory
			if(!new_entry->data)
			{
				printf("\n\n");
				printf("\n [!!] Critical error in BP_HashRegAddDataToTable() cannot calloc hash data.  Likely out of memory, exiting application immediately before any possible corruptions occur.");
				printf("\n\n");
				exit(0);
				return ERR_FAILURE;
			}

			// only copy in data if the data pointer exists
			if(data)
				memcpy(new_entry->data,  data, data_size);

			// set the entry type based on the passed in value (must be a valid type)
			new_entry->type = data_type;

			// set datasize member
			new_entry->data_size = data_size;

			// exit the case
			break;

		// if the datatype is unmanaged, exit immediately
		default:
			printf("\n [+] Bad datatype passed to BP_HashRegAddDataToTable.  This is critical.");
			return ERR_FAILURE;
			break;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Data to Table and Exit %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add to the table
	HASH_ADD_KEYPTR
	(
		hh,
		add_to_table,
		new_entry->key,
		bpstrlen(new_entry->key),
		new_entry
	);

	// rewrite the pointer location after lookup
	table->data = add_to_table;

	// increase the total number of tables in the hive
	hreg->total_data_entries_in_hive++;

	// return indicating success
	return ERR_SUCCESS;

}

// This is a frontend to BP_HashRegAddDataToTable.  It's fine for one-off use
// but in the long run it's more computationally expensive to use than using
// BP_HashRegAddDataToTable alone (if you already have the table path).
BP_ERROR_T BP_HashRegAddDataToTableByPath
(
		P_BP_HASH_TABLE_REGISTRY hreg,
		BP_HASH_TABLE_TRAVERSAL_PATH table_path,
		char * key,
		BP_HASH_TABLE_KEYED_ENTRY_TYPE data_type,
		void * data,
		size_t data_size
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// ensure we have a table path
	if(!table_path)
		return ERR_FAILURE;
	if(!table_path[0])
		return ERR_FAILURE;

	// attempt to lookup the table
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = BP_HashRegLookupTableByPath(hreg, table_path);
	if(!tmp_lookup)
		return ERR_FAILURE;

	// attempt to add data to the specified table
	return BP_HashRegAddDataToTable
	(
		hreg,
		tmp_lookup,
		key,
		data_type,
		data,
		data_size
	);

}

// This function allows you to add data to a table by using a lookup query in the form of
// "/path/whatever/index_add_point_will_be_here".
BP_ERROR_T BP_HashRegAddDataToTableByPathQuery
(
		P_BP_HASH_TABLE_REGISTRY hreg,
		char *lookup_query,
		char * key,
		BP_HASH_TABLE_KEYED_ENTRY_TYPE data_type,
		void * data,
		size_t data_size
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// ensure we have a lookup query
	if(!lookup_query)
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(lookup_query, bpstrlen(lookup_query), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// Create path array from lookup query.  This function will fail
	// if the lookup query cannot be parsed (bad characters etc)
	char ** path_lookup_array = BP_HashRegStringLookupToPathArray(lookup_query, bpstrlen(lookup_query));

	// if we can't convert the path, exit with failure code
	if(!path_lookup_array)
		return ERR_FAILURE;
	if(!path_lookup_array[0])
	{

		// destroy empty allocation before exiting
		bpfree(path_lookup_array);
		return ERR_FAILURE;
	}

	// Attempt to add data to path specified.  Path destination should be a table.
	BP_ERROR_T ret_code = BP_HashRegAddDataToTableByPath
	(
			hreg,
			path_lookup_array,
			key,
			data_type,
			data,
			data_size
	);

	// destroy the pointer array before exiting
	BP_DestroyPointerArrayNullTerminated((void **) path_lookup_array);

	// destroy the array itself
	bpfree(path_lookup_array);

	// return the addition return code
	return ret_code;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Display memory allocations.
BP_ERROR_T BP_HashRegDisplayMemoryAllocations
(
	P_BP_HASH_TABLE_REGISTRY hreg
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// ensure we have a memory table
	if(!hreg->memory_table)
		return ERR_FAILURE;
	if(!hreg->memory_table->entries)
		return ERR_FAILURE;

	// memory table
	P_BP_HASH_TABLE_MEM memory_table = hreg->memory_table;

	// tmp entries
	P_BP_HASH_TABLE_MEM_PTR_ENTRY tmp_entry   = NULL;
	P_BP_HASH_TABLE_MEM_PTR_ENTRY tmp_entries = NULL;

	// current entry
	P_BP_HASH_TABLE_MEM_PTR_ENTRY current_entry = NULL;

	// index position iterator[
	size_t idx_iter = 0;

	// iterate through all entries and display
	HASH_ITER(hh,hreg->memory_table->entries, current_entry, tmp_entry)
	{

		printf("\n\t\t [+:%u] Hash Allocator Entry %p", idx_iter, current_entry);
		printf("\n\t\t     key:  %p",       current_entry->data);
		printf("\n\t\t     size: %u bytes", current_entry->data_size);

		// display type
		printf("\n\t\t     alloc_type: ");

		switch(current_entry->alloc_type)
		{

			case BP_HASH_TABLE_ALLOCATOR_MALLOC:
				printf("BP_HASH_TABLE_ALLOCATOR_MALLOC");
				break;

			case BP_HASH_TABLE_ALLOCATOR_CALLOC:
				printf("BP_HASH_TABLE_ALLOCATOR_CALLOC");
				break;

			case BP_HASH_TABLE_ALLOCATOR_REALLOC:
				printf("BP_HASH_TABLE_ALLOCATOR_REALLOC");
				break;

			case BP_HASH_TABLE_ALLOCATOR_STRDUP:
				printf("BP_HASH_TABLE_ALLOCATOR_STRDUP");
				break;

			case BP_HASH_TABLE_ALLOCATOR_STRNDUP:
				printf("BP_HASH_TABLE_ALLOCATOR_STRNDUP");
				break;

			case BP_HASH_TABLE_ALLOCATOR_MMAP:
				printf("BP_HASH_TABLE_ALLOCATOR_MMAP");
				break;

			default:
				printf("BADVAL");
				break;


		}


		// display type
		printf("\n\t\t     entry_type: ");

		// tmp key used for dereferencing data entry into a keyed entry (so
		// key and parent can be displayed.)
		P_BP_HASH_TABLE_KEYED_ENTRY tmp_keyed        = NULL;
		P_BP_HASH_TABLE_KEYED_ENTRY tmp_parent_keyed = NULL;
		switch(current_entry->entry_type)
		{

			case BP_HASH_REG_MEM_ENTRY_TYPE_UNSET:
				printf("BP_HASH_REG_MEM_ENTRY_TYPE_UNSET");
				break;

			case BP_HASH_REG_MEM_ENTRY_TYPE_GENERIC_ALLOCATION:
				printf("BP_HASH_REG_MEM_ENTRY_TYPE_GENERIC_ALLOCATION");
				break;

			// display additional data if the type is a keyed entry
			case BP_HASH_REG_MEM_ENTRY_TYPE_KEYED_ENTRY_IN_HIVE:

				// display initial type immediately
				printf("BP_HASH_REG_MEM_ENTRY_TYPE_KEYED_ENTRY_IN_HIVE");

				// set the keyed entry to the data pointer
				tmp_keyed = (P_BP_HASH_TABLE_KEYED_ENTRY) current_entry->data;

				// if it exists,
				if(tmp_keyed)
				{
					tmp_parent_keyed = (P_BP_HASH_TABLE_KEYED_ENTRY) tmp_keyed->parent;
					printf(" (key: %s parent: %s)", tmp_keyed->key, tmp_keyed->parent ? tmp_parent_keyed->key : "UNSET");
				}
				else
					printf(" (data: NULL) ");

				break;

			default:
				printf("BADVAL");
				break;

		}


		// display info if set
		if(current_entry->func)
		{

			// display info to terminal
			printf("\n\t\t     info: %s:%u - %s", current_entry->file_name, current_entry->line_number, current_entry->func);

		}

		// display backtrace if set
		if(current_entry->alloc_backtrace_symbols && current_entry->alloc_backtrace_depth)
		{

			// display indicator
			printf("\n\t\t     alloc_backtrace:");

			// iterate through trace data
			size_t bt_iter = 0;
			for(; bt_iter < current_entry->alloc_backtrace_depth; bt_iter++)
			{

				// display symbol
				if(current_entry->alloc_backtrace_symbols[bt_iter])
				{
					printf("\n\t\t\t   [%u] %s", bt_iter, current_entry->alloc_backtrace_symbols[bt_iter]);
				}

			}

		}

		// trailing newline
		printf("\n");

		// increment the index iterator before continuing the loop
		idx_iter++;

	}

	// return indicating success
	return ERR_SUCCESS;

}

// display memory tables
BP_ERROR_T BP_HashRegDisplayMemory
(
	P_BP_HASH_TABLE_REGISTRY hreg,
	BP_BOOL                  display_entries
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	if(!hreg->memory_table)
		return ERR_FAILURE;

	printf("\n\t [+] Registry Memory Table %p", hreg->memory_table);
	printf("\n");
	printf("\n\t   write_lock:                      %p", hreg->memory_table->write_lock);
	printf("\n\t   write_lock_acquired_time:        %u", hreg->memory_table->write_lock_acquired_time);
	printf("\n\t   read_lock:                       %p", hreg->memory_table->read_lock);
	printf("\n\t   read_lock_acquired_time:         %u", hreg->memory_table->read_lock_acquired_time);
	printf("\n\t   permissions:                     %p (%u items)", hreg->memory_table->permissions, hreg->memory_table->permissions ? HASH_COUNT((P_BP_HASH_TABLE_KEYED_ENTRY) hreg->memory_table->permissions):0);
	printf("\n\t   entries:                         %p (%u items)", hreg->memory_table->entries,     hreg->memory_table->entries     ? HASH_COUNT( hreg->memory_table->entries)    :0);
	printf("\n\t   managed_heap_alloc_total_size:   %u", hreg->memory_table->managed_heap_alloc_total_size);
	printf("\n\t   managed_data_size:               %u", hreg->memory_table->managed_data_size);
	printf("\n\t   managment_structures_total_size: %u (with reg: %u)", hreg->memory_table->managment_structures_total_size, hreg->memory_table->managment_structures_total_size);
	printf("\n\t ");
	printf("\n");

	// display entries if user preference
	if(display_entries)
	{

		// display memory allocations
		BP_HashRegDisplayMemoryAllocations(hreg);

	}

	// return indicating success
	return ERR_SUCCESS;

}

// Display the hive within a registry.
BP_ERROR_T BP_HashRegDisplayHive
(
	P_BP_HASH_TABLE_REGISTRY hreg,
	BP_BOOL                  display_header,
	BP_BOOL                  display_entries
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// ensure we have  ahive
	if(!hreg->hive)
		return ERR_FAILURE;

	if(display_header)
	{

		// display the hive
		printf("\n\t [+] Registry Hive %p", hreg->hive);
		printf("\n\t  total_tables_in_hive:       %u", hreg->total_tables_in_hive);
		printf("\n\t  total_data_entries_in_hive: %u", hreg->total_data_entries_in_hive);
		printf("\n");

	}

	if(display_entries)
	{

		// display the tables
		BP_HashRegDisplayTables(hreg, BP_TRUE, BP_TRUE);


	}

	// return indicating success
	return ERR_SUCCESS;

}


// There are two real differing types in the hash registry/tables.  The first type
// is a table.  A table can contain either other tables, or data entries.  This routine
// displays data entries, not tables.
BP_ERROR_T BP_HashRegDisplayDataEntry
(
	P_BP_HASH_TABLE_REGISTRY    hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY data_entry,
	size_t                      display_tab_depth
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Format Tab Depth Buffer %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create space for tabs
	char *tabs = (char *) calloc(display_tab_depth+2, 1);
	if(!tabs)
	{
		if(BP_DEBUG_BREAKPOINT_ON_ALLOC_FAIL)
		{
			asm("int3");
		}
		return ERR_FAILURE;
	}

	// add tabs to buffer
	memset(tabs, '\t', display_tab_depth+1);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Table Line %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// display initial basic
	printf("\n%s data> %p: ", tabs, data_entry);

	// display type
	printf(" t: ");

	// display text representation of data type
	switch(data_entry->type)
	{

		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UNSET:
			printf("UNSET");
			break;

		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE:
			printf("NAMED_HASH_TABLE");
			break;

		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_C_DATASTRUCTURE_EXTERNALLY_MANAGED:
			printf("C_DATASTRUCTURE");
			break;

		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB:
			printf("BINARY_BLOB");
			break;

		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_CPP_CLASS_POINTER_EXTERNALLY_MANAGED:
			printf("CPP_CLASS_POINTER");
			break;

		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING:
			printf("UTF8_STRING");
			break;

		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UNICODE16_STRING:
			printf("UNICODE16_STRING");
			break;

		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UNICODE32_STRING:
			printf("UNICODE32_STRING");
			break;

		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_SIZE_T:
			printf("SIZE_T");
			break;

		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_SIGNED_INT:
			printf("SIGNED_INT");
			break;

		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_JSON:
			printf("UTF8_JSON");
			break;

		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_MESSAGE_TO_WORKER:
			printf("MESSAGE_TO_WORKER");
			break;

		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_MESSAGE_FROM_WORKER:
			printf("MESSAGE_FROM_WORKER");
			break;

		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_EVENT_UNPROCESSED:
			printf("EVENT_UNPROCESSED");
			break;

		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_EVENT_PROCESSED:
			printf("EVENT_PROCESSED");
			break;


		default:
			break;

	}

	// display the id
	printf(", id: \"%s\"" , data_entry->key);

	// display lock status
	int lock_tmp = 0;
	BP_HASH_TABLE_SEM_IS_LOCKED(data_entry->lock, lock_tmp);

	// check if things are locked or unlocked
	printf(", lock: %s", lock_tmp ? "UL" : "L");
	printf(" :%i", lock_tmp );

	// set the parent
	if(data_entry->parent)
	{

		// create reference to parent so we can access its members
		P_BP_HASH_TABLE_KEYED_ENTRY tmp_parent = (P_BP_HASH_TABLE_KEYED_ENTRY) data_entry->parent;

		// display the parent key and ptr
		printf(" parent: %s", tmp_parent->key);


	}
	else
	{
		// display the parent key and ptr
		printf(" parent: NULL");
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Exit %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// destroy tabs if set
	if(tabs)
		free(tabs);

	// return indicating success
	return ERR_SUCCESS;

}

// display single table (optionally set tab depth)
BP_ERROR_T BP_HashRegDisplayTable
(
	P_BP_HASH_TABLE_REGISTRY    hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY table_entry,
	size_t                      display_tab_depth,
	BP_BOOL                     recurse,
	BP_BOOL                     display_entries
)
{


	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// ensure we have a table lookup
	if(!hreg->hive)
		return ERR_FAILURE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Format Tab Depth Buffer %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create space for tabs
	char *tabs = (char *) calloc(display_tab_depth+2, 1);
	if(!tabs)
	{
		if(BP_DEBUG_BREAKPOINT_ON_ALLOC_FAIL)
		{
			asm("int3");
		}
		return ERR_FAILURE;
	}

	// add tabs to buffer
	memset(tabs, '\t', display_tab_depth+1);

	// tmp hash table
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_table = (P_BP_HASH_TABLE_KEYED_ENTRY) table_entry->data;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Table Line %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// display initial basic
	printf("\n%s [T>]  %p: %s ", tabs, table_entry, table_entry->key);

	// display entry counts
	if(!tmp_table)
		printf("( 0 entries, null table )");
	else
		printf("( %u entries in table )", tmp_table ? HASH_COUNT((P_BP_HASH_TABLE_KEYED_ENTRY) tmp_table) : 100);

	// display semaphore lock status
	int tmp_val = 0;
	printf(" lock: %s", BP_HASH_TABLE_SEM_IS_LOCKED(table_entry->lock, tmp_val) ? "UL" : "L");
	printf(" :%i", tmp_val );

	if(table_entry->parent)
	{

		// create reference to parent so we can access its members
		P_BP_HASH_TABLE_KEYED_ENTRY tmp_parent = (P_BP_HASH_TABLE_KEYED_ENTRY) table_entry->parent;

		// display the parent key and ptr
		printf(" parent: %s", tmp_parent->key);


	}
	else
	{
		// display the parent key and ptr
		printf(" parent: NULL");
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Recursion Logic %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// temporary iterator values used during the HASH_ITER loop below
	P_BP_HASH_TABLE_KEYED_ENTRY entry = NULL;
	P_BP_HASH_TABLE_KEYED_ENTRY tmp   = NULL;

	// increase the display tab depth in case of recursion (keeps
	// things lined up)
	display_tab_depth++;


	// iterate through all hashes and display
	HASH_ITER(hh, tmp_table, entry, tmp)
	{


		// set temp table
		tmp_table = (P_BP_HASH_TABLE_KEYED_ENTRY) entry->data;


		// It's either a data entry or a hash table.  Display either based on
		// parameter preference.
		if(entry->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{

			// display the data entry
			BP_HashRegDisplayDataEntry
			(
					hreg,
					entry,
					display_tab_depth
			);

			// continue the loop
			continue;

		}
		else
		{

			// recurse here
			BP_HashRegDisplayTable(hreg, entry, display_tab_depth, recurse, display_entries);

		}



	}


	// destroy tabs buffer
	if(tabs)
		free(tabs);

	// return indicating success
	return ERR_SUCCESS;

}

// display registered tables
BP_ERROR_T BP_HashRegDisplayTables
(
		P_BP_HASH_TABLE_REGISTRY hreg,
		BP_BOOL                  recurse,
		BP_BOOL                  display_entries
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// ensure we have a table lookup
	if(!hreg->hive)
		return ERR_FAILURE;

	// tmp/out values for display lookups
	P_BP_HASH_TABLE_KEYED_ENTRY entry = NULL;
	P_BP_HASH_TABLE_KEYED_ENTRY tmp   = NULL;

	// tmp hash table
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_table = NULL;

	// BP_HashRegDisplayTable(hreg, hreg->hive, 1, recurse, display_entries);

	// iterate through all hashes and display
	HASH_ITER(hh, hreg->hive, entry, tmp)
	{

		// set temp table
		tmp_table = (P_BP_HASH_TABLE_KEYED_ENTRY) entry->data;

		// It's either a data entry or a hash table.  Display either based on
		// parameter preference.
		if(entry->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{

			// display the data entry
			BP_HashRegDisplayDataEntry
			(
					hreg,
					entry,
					1
			);

			// continue the loop
			continue;

		}
		else
		{

			// display the table]
			BP_HashRegDisplayTable(hreg, entry, 1, recurse, display_entries);

		}

	}


	// display trailing newline
	printf("\n");

	// return indicating success
	return ERR_SUCCESS;

}

// this will display the
BP_ERROR_T BP_HashRegDisplayPath
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	BP_HASH_TABLE_TRAVERSAL_PATH  table_path,
	BP_BOOL                       display_table_entries,
	BP_BOOL                       display_extended_entry_info,
	size_t                        max_recurse_level
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// attempt to lookup table
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = BP_HashRegLookupTableByPath(hreg, table_path);
	if(!tmp_lookup)
		return ERR_FAILURE;

	// attempt to display the looked up table
	return BP_HashRegDisplayTable(hreg, tmp_lookup, 0, BP_TRUE, BP_TRUE);

}

// This function should not be called directly.  It's called by BP_HashRegDisplayPath internally.
BP_ERROR_T BP_HashRegDisplayPath_recurse
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	P_BP_HASH_TABLE_KEYED_ENTRY   table,
	BP_BOOL                       display_table_entries,
	BP_BOOL                       display_extended_entry_info,
	size_t                        max_recurse_level
)
{



	// return indicating success
	return ERR_SUCCESS;

}



// simple catch all routine for displaying all information stored within a hash registry
BP_ERROR_T BP_HashRegDisplay
(
	P_BP_HASH_TABLE_REGISTRY hreg,
	BP_BOOL                  display_mem_entries,
	BP_BOOL                  display_hive_entries
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);


	// &global_mprof_hash_table_registry

	// display the hash table registry
	printf("\n [+] Hash Table Registry: %p", hreg);
	printf("\n");
	printf("\n   lock:               %p", hreg->lock);
	printf("\n   lock_acquired_time: %u", hreg->lock_acquired_time);
	printf("\n   memory_table:       %p", hreg->memory_table);
	printf("\n   hive:               %p", hreg->hive);
	printf("\n");

	// display memory
	BP_HashRegDisplayMemory(hreg, display_mem_entries);

	// display the hive
	BP_HashRegDisplayHive(hreg, BP_TRUE, display_hive_entries);

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Add Hash Memory Entry %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// add a key to a registry
void * BP_HashRegAddMemoryEntry
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	BP_HASH_REG_MEM_ENTRY_TYPE    entry_type,
	size_t                        initial_data_size,
	BPLN_PARMS
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(NULL);


	// ensure we have an initial data size
	if(!initial_data_size)
		return NULL;

	// create memory table if we don't already have one
	if(!hreg->memory_table)
		hreg->memory_table = (P_BP_HASH_TABLE_MEM) calloc(sizeof(BP_HASH_TABLE_MEM), 1);

	// ensure the table was created ok
	if(!hreg->memory_table)
	{
		if(BP_DEBUG_BREAKPOINT_ON_ALLOC_FAIL)
		{
			asm("int3");
		}
		return NULL;
	}


	// allocate new entry
	P_BP_HASH_TABLE_MEM_PTR_ENTRY new_entry = (P_BP_HASH_TABLE_MEM_PTR_ENTRY) calloc(sizeof(BP_HASH_TABLE_MEM_PTR_ENTRY), 1);
	if(!new_entry)
	{
		if(BP_DEBUG_BREAKPOINT_ON_ALLOC_FAIL)
		{
			asm("int3");
		}
		return NULL;
	}

	// set initial data size
	new_entry->data_size = initial_data_size;

	// allocate space for data (must be in loop)
	do
	{
		new_entry->data = (void *) calloc(initial_data_size, 1);
		if(new_entry->data)
		{
			break;
		}

		if(BP_DEBUG_BREAKPOINT_ON_ALLOC_FAIL)
		{
			printf("\n [!!] calloc failed: (initial_data_size: %u, elem_size: 1", initial_data_size);
			printf("\n");
			asm("int3");
		}

		// sleep for a few seconds before retrying alloc
		sleep(3);
	}
	while(1);

	// set key and data pointer as the same
	new_entry->key = new_entry->data;

	// default allocator is clear/alloc
	new_entry->alloc_type = BP_HASH_TABLE_ALLOCATOR_CALLOC;

	// set the entry type from user specification
	new_entry->entry_type = entry_type;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Line/Func/File Tracking %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// counters for adding func and filename
	size_t add_func_len  = 0;
	size_t add_fname_len = 0;

	// set function
	if(func)
	{
		add_func_len    = bpstrlen(func)+1;
		new_entry->func = strdup(func);
	}

	// set filename
	if(file_name)
	{
		add_fname_len        = bpstrlen(file_name)+1;
		new_entry->file_name = strdup(file_name);
	}

	// set the line number
	new_entry->line_number = line_number;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Backtrace if Globally Configured To Do So %%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(global_mprof_create_backtrace_on_alloc)
	{

		// create depth for backtrace
		size_t max_depth = 32;

		// create temporary buffer
		char *bt_buff[max_depth+1];
		BP_ZERO_STACK(bt_buff);

		// gather backtrace (use int, not size_t)
		int bt_size = backtrace((void **) &bt_buff, max_depth);

		// gather trace depth count
		size_t trace_depth_count = BP_CountNullTerminatedPtrArray((char **) &bt_buff);

		// gather symbols
		char ** bt_syms = backtrace_symbols((void **) &bt_buff, max_depth);
		if(!bt_syms)
		{
			printf("\n [!!] Error: backtrace_symbols() failed, this should never happen.");
			printf("\n");
			__asm("int3");
			return NULL;
		}


		// allocate space for symbols
		do
		{

			// allocate space for symbols (direct calloc)
			new_entry->alloc_backtrace_symbols = (char **) calloc(sizeof(char *) * (bt_size+1), 1);
			if(new_entry->alloc_backtrace_symbols)
			{
				break;
			}

			if(BP_DEBUG_BREAKPOINT_ON_ALLOC_FAIL)
			{
				printf("\n [!!] Memory addition failed because backtrace symbols could not be allocated");
				printf("\n");
				asm("int3");
			}

			// sleep for a few seconds before retrying alloc
			sleep(3);
		}
		while(1);

		// allocate space for backtrace
		do
		{
			// allocate space for symbols (direct calloc)
			new_entry->alloc_backtrace = (char **) calloc(sizeof(char *) * (bt_size+1), 1);
			if(new_entry->alloc_backtrace)
			{
				break;
			}

			if(BP_DEBUG_BREAKPOINT_ON_ALLOC_FAIL)
			{
				printf("\n [!!] Memory addition failed because backtrace symbols could not be allocated");
				printf("\n");
				asm("int3");
			}

			// sleep for a few seconds before retrying allocs
			sleep(3);
		}
		while(1);

		// iterator
		size_t n = 0;

		// add all symbols
		for(; n < bt_size; n++)
		{

			// duplicate the symbol and trace
			new_entry->alloc_backtrace[n]         = strdup(bt_buff[n]);
			new_entry->alloc_backtrace_symbols[n] = strdup(bt_syms[n]);

			// increment the backtrace depth
			new_entry->alloc_backtrace_depth++;

		}


	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Entry %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add to the memory table
	HASH_ADD_PTR
	(
		hreg->memory_table->entries,
		key,
		new_entry
	);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Adjust Size Counters %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// increase the managed data size by the size of a new entry
	hreg->memory_table->managment_structures_total_size += sizeof(BP_HASH_TABLE_MEM_PTR_ENTRY);

	// increase managed data size
	hreg->memory_table->managed_data_size += initial_data_size;

	// increase the total alloc data size
	hreg->memory_table->managed_heap_alloc_total_size += initial_data_size + sizeof(BP_HASH_TABLE_MEM_PTR_ENTRY) + add_func_len + add_fname_len;



	// return indicating success
	return new_entry->data;

}

// resize an item within the has
void * BP_HashRegResizeMemoryEntry
(
	P_BP_HASH_TABLE_REGISTRY hreg,
	void *                   old_data_pointer,
	size_t                   new_data_size,
	BPLN_PARMS
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(NULL);

	// ensure we have a resize
	if(!new_data_size)
		return NULL;

	// ensure we have an old data pointer
	if(!old_data_pointer)
		return NULL;


	// this will hold the new allocation if it was created (used as default return)
	void *new_data = NULL;

	// lookup old pointer first
	P_BP_HASH_TABLE_MEM_PTR_ENTRY tmp_entry = BP_HashRegMemPtrFind(hreg, old_data_pointer);



	// if the entry was looked up, go ahead and resize it
	if(tmp_entry)
	{

		// attempt to add a new memory entry first (pass line parameters through from BPLN_PARMS rather than
		// from local BPLN)
		new_data = BP_HashRegAddMemoryEntry
		(
			hreg,
			tmp_entry->entry_type,
			new_data_size+1,
			file_name,
			line_number,
			func
		);

		// if we can't add a new entry, return null
		if(!new_data)
			return NULL;

		// size to copy
		size_t copy_size = 0;

		// copy in data; size  is dependent on copy size vs buffer alloc.  Buffer will be
		// pruned in the case that it's oversized.
		if(tmp_entry->data_size < new_data_size)
		{
			memcpy(new_data, old_data_pointer, tmp_entry->data_size);
		}
		else
		{
			memcpy(new_data, old_data_pointer, new_data_size);
		}


		// destroy old memory
		BP_HashRegDeleteMemoryEntryByDataPtr(hreg, old_data_pointer);



	}

	// return the new data
	return new_data;

}


// lookup an entry based on a provided address
P_BP_HASH_TABLE_MEM_PTR_ENTRY BP_HashRegMemPtrFind
(
	P_BP_HASH_TABLE_REGISTRY hreg,
	void *addr
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(NULL);

	if(!hreg->memory_table)
		return NULL;
	if(!addr)
		return NULL;


	// value which will be returned
	P_BP_HASH_TABLE_MEM_PTR_ENTRY found = NULL;

	// attempt to find the address
	HASH_FIND_PTR(hreg->memory_table->entries, &addr, found);

	// return the found item
	return found;

}

// delete a memory entry
BP_ERROR_T BP_HashRegDeleteMemoryEntry
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	P_BP_HASH_TABLE_MEM_PTR_ENTRY entry
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// ensure we have an entry
	if(!entry)
		return ERR_FAILURE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy BPLN %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we have a func destroy it
	if(entry->func)
	{

		// shrink alloc size (func)
		hreg->memory_table->managed_heap_alloc_total_size -= bpstrlen(entry->func)+1;

		// free and nullify (prevent any possibility of in-mem dangler)
		free(entry->func);
		entry->func = NULL;

	}

	// if we have a func destroy it
	if(entry->file_name)
	{

		// shrink alloc size (file_name)
		hreg->memory_table->managed_heap_alloc_total_size -= bpstrlen(entry->file_name)+1;

		// free and nullify (prevent any possibility of in-mem dangler)
		free(entry->file_name);
		entry->file_name = NULL;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Backtrace if Necessary %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we a backtrace, destroy it here
	if(entry->alloc_backtrace)
	{

		// destroy the individual elements
		size_t n = 0;
		for(; entry->alloc_backtrace[n]; n++)
		{
			free(entry->alloc_backtrace[n]);
			entry->alloc_backtrace[n] = NULL;
		}

		// destroy
		free(entry->alloc_backtrace);
		entry->alloc_backtrace = NULL;
	}

	// if we have backtrace symbols, destroy them here
	if(entry->alloc_backtrace_symbols)
	{
		// destroy the individual elements
		size_t n = 0;
		for(; entry->alloc_backtrace_symbols[n]; n++)
		{
			free(entry->alloc_backtrace_symbols[n]);
			entry->alloc_backtrace_symbols[n] = NULL;
		}

		// destroy
		free(entry->alloc_backtrace_symbols);
		entry->alloc_backtrace = NULL;

	}

	// reset depth so it doesn't dangle in mem
	entry->alloc_backtrace_depth = 0;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Data (if necessary) %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// NOTE: Only internally allocated data is destroyed here.  Data
	//       that isn't an internal type is not destroyed (as it would
	//       lead to eventual corruption)

	// destroy data if it exists
	if(entry->data)
	{

		// the data entry must be found within our hash table
		if(BP_HashRegMemPtrValid(hreg, entry->data))
		{

			// lookup the memory (this is a secondary check)
			P_BP_HASH_TABLE_MEM_PTR_ENTRY lookup_mem_tmp = BP_HashRegMemPtrFind(hreg, entry->data);

			// if we could lookup the pointer ok, destroy it
			if(lookup_mem_tmp)
			{

				// set via the switch below to determine if destroy is required
				BP_BOOL destroy_required = BP_FALSE;

				// switch on type and set destroy flag
				switch(lookup_mem_tmp->entry_type)
				{
					case BP_HASH_REG_MEM_ENTRY_TYPE_GENERIC_ALLOCATION:
					case BP_HASH_REG_MEM_ENTRY_TYPE_KEYED_ENTRY_IN_HIVE:
						destroy_required = BP_TRUE;
						break;
					default:
						break;
				}

				if(lookup_mem_tmp && destroy_required)
				{

					// shrink alloc size (data)
					hreg->memory_table->managed_heap_alloc_total_size -= lookup_mem_tmp->data_size;
					hreg->memory_table->managed_data_size             -= lookup_mem_tmp->data_size;

					// zero data size
					lookup_mem_tmp->data_size = 0;

					// remove the entry
					HASH_DEL(hreg->memory_table->entries, lookup_mem_tmp);

					// destroy data if its set (this is required as data can
					// potentially be removed early when garbage collection
					// occurs)
					if(lookup_mem_tmp->data)
					{
						free(lookup_mem_tmp->data);
						lookup_mem_tmp->data = NULL;
					}

					// destroy the entry
					free(lookup_mem_tmp);
					lookup_mem_tmp = NULL;


					// reduce the total alloc size
					hreg->memory_table->managed_heap_alloc_total_size   -= sizeof(BP_HASH_TABLE_MEM_PTR_ENTRY);
					hreg->memory_table->managment_structures_total_size -= sizeof(BP_HASH_TABLE_MEM_PTR_ENTRY);


				}

			}

		}

	}


	// reduce the total alloc size
	// hreg->memory_table->managed_heap_alloc_total_size   -= sizeof(BP_HASH_TABLE_MEM_PTR_ENTRY);
	// hreg->memory_table->managment_structures_total_size -= sizeof(BP_HASH_TABLE_MEM_PTR_ENTRY);

	// return indicating success
	return ERR_SUCCESS;

}

// delete a memory entry
BP_ERROR_T BP_HashRegDeleteMemoryEntryByDataPtr
(
	P_BP_HASH_TABLE_REGISTRY      hreg,
	void *                        data
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// ensure we hae a data pointer
	if(!data)
		return ERR_FAILURE;

	// attempt to lookup entries
	P_BP_HASH_TABLE_MEM_PTR_ENTRY lookup_entry = NULL;

	// attempt to lookup the entry based on the pointer
	lookup_entry = (P_BP_HASH_TABLE_MEM_PTR_ENTRY) BP_HashRegMemPtrFind(hreg, data);

	// ensure we have a lookup entry
	if(!lookup_entry)
		return ERR_FAILURE;



	// delete the entry
	return BP_HashRegDeleteMemoryEntry(hreg, lookup_entry);


}

// Attempts to delete all the memory entries in a registry.  This should
// be only used during the destruction of the entire hash table registry;
// failing to heed this warning and attempting to use it yourself can
// very easily lead to a corruption, as hive entries correlate to memory
// entries, and if a hive entry is missing a memory entry, it causes a
// bad reference.  READ THIS WHOLE TEXT BLOCK.
BP_ERROR_T BP_HashRegDeleteAllMemoryEntries
(
	P_BP_HASH_TABLE_REGISTRY      hreg
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);


	// ensure we have a memory table
	if(!hreg->memory_table)
		return ERR_FAILURE;
	if(!hreg->memory_table->entries)
		return ERR_FAILURE;

	// memory table
	P_BP_HASH_TABLE_MEM memory_table = hreg->memory_table;

	// tmp entries
	P_BP_HASH_TABLE_MEM_PTR_ENTRY tmp_entry   = NULL;
	P_BP_HASH_TABLE_MEM_PTR_ENTRY tmp_entries = NULL;

	// current entry
	P_BP_HASH_TABLE_MEM_PTR_ENTRY current_entry = NULL;

	// this entry will be deleted
	P_BP_HASH_TABLE_MEM_PTR_ENTRY entry_to_delete = NULL;

	// index position iterator[
	size_t idx_iter = 0;

	do
	{

		// reset entry
		entry_to_delete = NULL;

		// break if the pointer is null
		if(!hreg->memory_table->entries)
			break;

		// retrieve entry from fresh position (prevents corruption)
		HASH_ITER(hh,hreg->memory_table->entries, current_entry, tmp_entry)
		{

			// check to ensure we have a current entry
			entry_to_delete = current_entry;
			if(entry_to_delete)
				break;

		}

		// exit loop if we have no more entries
		if(!entry_to_delete)
			break;

		// delete the current entry
		BP_HashRegDeleteMemoryEntry(hreg, entry_to_delete);


	}
	while (entry_to_delete);

	// return indicating success
	return ERR_SUCCESS;

}


// remove a table entry
BP_ERROR_T BP_HashRegRemoveTableEntry
(
	P_BP_HASH_TABLE_REGISTRY hreg,
	 char *                  table,
	char *                   key
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Validation Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// checks if a memory pointer is registered as having been allocated by
// our registry.  Only chunks which are currently still registered in
// this registry will be validated as true.
BP_BOOL BP_HashRegMemPtrValid(P_BP_HASH_TABLE_REGISTRY hreg, void *ptr)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(BP_FALSE);

	// attempt to lookup the memory pointer
	P_BP_HASH_TABLE_MEM_PTR_ENTRY mementry = BP_HashRegMemPtrFind(hreg, ptr);
	if(!mementry)
	{

		// return false if there is no entry
		return BP_FALSE;

	}

	// return indicating success
	return BP_TRUE;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Entry Count Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// returns the number of entries in a table
size_t BP_HashRegCountEntriesInTable
(
	P_BP_HASH_TABLE_KEYED_ENTRY table,
	BP_HASH_TABLE_COUNT_POLICY  count_policy
)
{

	// run basic checks
	if(!table)
		return 0;
	if(table->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		return 0;
	if(!table->data)
		return 0;

	// count tables
	size_t entry_n = 0;
	BP_HASH_ITER_OPEN( (P_BP_HTKE) table->data, tmp_entry)
	{

		// switch on the count policy
		switch(count_policy)
		{

			// count data entries
			case BP_HASH_TABLE_COUNT_POLICY_COUNT_ONLY_DATA_ENTRIES:
				if(tmp_entry->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
					entry_n++;
				break;

			// count table entries only
			case BP_HASH_TABLE_COUNT_POLICY_COUNT_ONLY_TABLE_ENTRIES:
				if(tmp_entry->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
					entry_n++;
				break;

			// count all
			case BP_HASH_TABLE_COUNT_POLICY_COUNT_ALL:
				entry_n++;
				break;
		}


	}
	BP_HASH_ITER_CLOSE;

	// return the entry count
	return entry_n;

}

// count via path
size_t BP_HashRegCountEntriesInTableByPath
(
	P_BP_HASH_TABLE_REGISTRY   hreg,
	char **                    path,
	BP_HASH_TABLE_COUNT_POLICY count_policy
)
{

	// macro to ov.erride hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// run basic checks
	if(!path)
		return 0;
	if(!path[0])
		return 0;

	// attempt table lookup
	P_BP_HTKE table_lookup = BP_HashRegLookupTableByPath(hreg, path);
	if(!table_lookup)
		return 0;

	// count the entries
	size_t ret_count = BP_HashRegCountEntriesInTable(table_lookup, count_policy);

	// return the count
	return ret_count;

}

// count via path query
// count by path query (does not recurse)
size_t BP_HashRegCountEntriesInTableByPathQuery
(
	P_BP_HASH_TABLE_REGISTRY   hreg,
	char *                     lookup_query,
	BP_HASH_TABLE_COUNT_POLICY count_policy
)
{

	// macro to ov.erride hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	if(!lookup_query)
		return ERR_FAILURE;

	// validate that string is reasonable/printable
	if(!BP_StringIsReasonablePrintableString(lookup_query, bpstrlen(lookup_query), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// Create path array from lookup query.  This function will fail
	// if the lookup query cannot be parsed (bad characters etc)
	char ** path_lookup_array = BP_HashRegStringLookupToPathArray(lookup_query, bpstrlen(lookup_query));

	// if we can't convert the path, exit with failure code
	if(!path_lookup_array)
		return ERR_FAILURE;

	// ensure we have a first position
	if(!path_lookup_array[0])
	{
		bpfree(path_lookup_array);
		return ERR_FAILURE;
	}

	// set the entry count
	size_t entry_count = BP_HashRegCountEntriesInTableByPath(hreg, path_lookup_array, count_policy);

	// destroy the pointer array before exiting
	BP_DestroyPointerArrayNullTerminated((void **) path_lookup_array);

	// destroy the array structure itself
	bpfree(path_lookup_array);

	// return with the addition return code
	return entry_count;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Hash Table Generic Allocator Routines %%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/*
 * These routines are utility routines to provide the same level of
 * functionality found in the tailqueue allocator, such that allocations
 * become VERY simple to implement so long as you're working with a valid
 * hash table registry.  Most of these routines are simply frontend macros
 * for the various hash table routines found above.
 *
 */

// Destroys memory allocated in the hash table
BP_ERROR_T ht_free_real
(
		P_BP_HASH_TABLE_REGISTRY hreg,
		void *ptr,
		BPLN_PARMS
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(ERR_FAILURE);

	// ensure we have a registry
	if(!hreg)
		return ERR_FAILURE;

	// ensure we hae a data pointer
	if(!ptr)
		return ERR_FAILURE;


	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(hreg);


	// attempt to lookup entries
	P_BP_HASH_TABLE_MEM_PTR_ENTRY lookup_entry = NULL;

	// attempt to lookup the entry based on the pointer
	lookup_entry = (P_BP_HASH_TABLE_MEM_PTR_ENTRY) BP_HashRegMemPtrFind(hreg, ptr);


	// ensure we have a lookup entry
	if(!lookup_entry)
		return ERR_FAILURE;


	// delete entry
	// printf("\n Deleting entry?: %p", lookup_entry);
	BP_ERROR_T delete_status = BP_HashRegDeleteMemoryEntry(hreg, lookup_entry);


	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(hreg);

	// return the error status
	return delete_status;


}

// duplicates a string and stores it in the hash table
char * ht_strdup_real
(
		P_BP_HASH_TABLE_REGISTRY hreg,
		char *dup,
		BPLN_PARMS
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(NULL);

	// ensure we have a registry
	if(!hreg)
		return NULL;

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(hreg);

	// ensure we have a registry to alloc from
	if(!hreg)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(hreg, NULL);

	// ensure we have a string to dup
	if(!dup)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(hreg, NULL);

	// ensure we can calc length without faulting
	size_t str_len = bpstrlen(dup);
	if(!str_len)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(hreg, NULL);

	// run basic string checks (newlines/cr ok)
	if(!BP_StringIsReasonablePrintableString(dup, str_len, BP_TRUE, BP_TRUE))
	{
	 	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(hreg, NULL);
	}

	// attempt to allocate the entry within the hash registry
	char * new_string = (char *) BP_HashRegAddMemoryEntry
	(
			hreg,
			BP_HASH_REG_MEM_ENTRY_TYPE_GENERIC_ALLOCATION,
			str_len+1,
			BPLN_PARMS_PASSTHROUGH
	);

	// copy in the string
	if(new_string)
		memcpy(new_string, dup, str_len);

	// exit if we haven't allocated a new string
	if(!new_string)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(hreg, NULL);

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(hreg);

	// return indicating success
	return new_string;

}

// duplicates a string and stores it in the hash table
char * ht_strndup_real
(
		P_BP_HASH_TABLE_REGISTRY hreg,
		char * dup,
		size_t len,
		BPLN_PARMS
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(NULL);

	// ensure we have a registry
	if(!hreg)
		return NULL;

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(hreg);

	// ensure we have a registry to alloc from
	if(!hreg)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(hreg, NULL);

	// ensure we have a string to dup
	if(!dup)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(hreg, NULL);

	// ensure we can calc length without faulting
	size_t str_len = bpstrlen(dup);
	if(!str_len)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(hreg, NULL);

	// this is the calculated final size for allocation
	size_t final_len = 0;

	// calculate final length based around user specified
	// length limitation.  Also shrink buffer if it's smaller
	// than the maximum (saves memory)
	if(str_len >= len)
		final_len = len;
	else
		final_len = str_len;

	// run basic string checks (newlines/cr ok)
	if(!BP_StringIsReasonablePrintableString(dup, final_len, BP_TRUE, BP_TRUE))
	{
	 	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(hreg, NULL);
	}

	// attempt to allocate the entry within the hash registry
	char * new_string = (char *) BP_HashRegAddMemoryEntry
	(
			hreg,
			BP_HASH_REG_MEM_ENTRY_TYPE_GENERIC_ALLOCATION,
			final_len+1,
			BPLN_PARMS_PASSTHROUGH
	);

	// copy in the string
	if(new_string)
		memcpy(new_string, dup, final_len);

	// exit if we haven't allocated a new string
	if(!new_string)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(hreg, NULL);

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(hreg);

	// return indicating success
	return new_string;

}

// allocate memory
void * ht_malloc_real
(
		P_BP_HASH_TABLE_REGISTRY hreg,
		size_t size,
		BPLN_PARMS
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(NULL);

	// ensure we have a registry
	if(!hreg)
		return NULL;

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(hreg);

	// attempt to allocate the entry within the hash registry
	void *new_entry = BP_HashRegAddMemoryEntry
	(
			hreg,
			BP_HASH_REG_MEM_ENTRY_TYPE_GENERIC_ALLOCATION,
			size,
			BPLN_PARMS_PASSTHROUGH
	);

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(hreg);

	// return the newly allocated entry
	return new_entry;

}

// simple wrapper to calloc items in calloc form
void * ht_calloc_real
(
		P_BP_HASH_TABLE_REGISTRY hreg,
		size_t size,
		size_t size_n,
		BPLN_PARMS
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(NULL);

	// ensure we have a registry
	if(!hreg)
		return NULL;

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(hreg);

	// attempt to allocate the entry within the hash registry
	void * new_entry = BP_HashRegAddMemoryEntry
	(
			hreg,
			BP_HASH_REG_MEM_ENTRY_TYPE_GENERIC_ALLOCATION,
			size * size_n,
			BPLN_PARMS_PASSTHROUGH
	);

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(hreg);

	// return the new entry
	return new_entry;

}


// Attempt to reallocate a managed chunk to an arbitrary non-zero size.
void * ht_realloc_real
(
		P_BP_HASH_TABLE_REGISTRY hreg,
		void * addr,
		size_t new_size,
		BPLN_PARMS
)
{

	// macro to override hreg on null
	BP_GLOBAL_HASH_TABLE_OVERRIDE(NULL);

	// ensure we have a registry
	if(!hreg)
		return NULL;

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(hreg);

	// size must be set
	if(!new_size)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(hreg, NULL);

	// new entry will either be a new allocation or a resize
	void * new_entry = NULL;

	// if there is no address, create a new one
	if(!addr)
	{
		new_entry = ht_calloc(hreg, new_size, 1);
	}
	else
	{

		// attempt to resize a memory entry
		new_entry = BP_HashRegResizeMemoryEntry
		(
			hreg,
			addr,
			new_size,
			BPLN_PARMS_PASSTHROUGH
		);
		if(!new_entry)
			printf("\n UNABLE TO RESIZE ENTRY?!");
	}

	// ensure we can add a new entry
	if(!new_entry)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(hreg, NULL);

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(hreg);

	// return the new data
	return new_entry;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Value Basic Type Test Routines %%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// hash table entry is a blob type (can be used for ht_*cmp routines)
BP_ERROR_T ht_entry_is_blob(P_BP_HASH_TABLE_KEYED_ENTRY entry)
{

	// ensure we have an entry pointer
	if(!entry)
		return ERR_FAILURE;

	// switch on the entry type
	switch(entry->type)
	{

		// if the type matches any of our buffer types, indicate it here
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_POINTER_EXTERNALLY_MANAGED:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_C_DATASTRUCTURE_EXTERNALLY_MANAGED:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UNICODE16_STRING:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UNICODE32_STRING:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_JSON:
			return ERR_SUCCESS;
			break;
		default:
			break;
	}

	// return failure on mismatch
	return ERR_FAILURE;

}

// The entry is a named hash table containing zero or more sub entries which can be
// iterated through
BP_ERROR_T ht_entry_is_table(P_BP_HASH_TABLE_KEYED_ENTRY entry)
{

	// ensure we have an entry pointer
	if(!entry)
		return ERR_FAILURE;

	// switch on the entry type
	switch(entry->type)
	{

		// if the type matches any of our buffer types, indicate it here
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE:
			return ERR_SUCCESS;
			break;
		default:
			break;
	}

	// return failure on mismatch
	return ERR_FAILURE;

}


// Checks if the entry is a "special" type such as a class pointer.
BP_ERROR_T ht_entry_is_special_type(P_BP_HASH_TABLE_KEYED_ENTRY entry)
{

	// ensure we have an entry pointer
	if(!entry)
		return ERR_FAILURE;

	// switch on the entry type
	switch(entry->type)
	{

		// if the type matches any of our buffer types, indicate it here
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_CPP_CLASS_POINTER_EXTERNALLY_MANAGED:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_MESSAGE_TO_WORKER:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_MESSAGE_FROM_WORKER:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_EVENT_UNPROCESSED:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_EVENT_PROCESSED:
			return ERR_SUCCESS;
			break;
		default:
			break;
	}

	// return indicating failure
	return ERR_FAILURE;

}


// Checks if an entry is an event type.
BP_ERROR_T ht_entry_is_event_type(P_BP_HASH_TABLE_KEYED_ENTRY entry)
{

	// ensure we have an entry pointer
	if(!entry)
		return ERR_FAILURE;

	// switch on the entry type
	switch(entry->type)
	{

		// run event type checks
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_EVENT_UNPROCESSED:
		case BP_HASH_TABLE_KEYED_ENTRY_TYPE_EVENT_PROCESSED:
			return ERR_SUCCESS;
			break;
		default:
			break;
	}

	// return indicating failure
	return ERR_FAILURE;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Value Compare Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// memory compare hash table keyed entry (binary only compare)
BP_ERROR_T ht_memmatch_entry
(
		P_BP_HASH_TABLE_KEYED_ENTRY entry,
		char  *buff,
		size_t buff_size
)
{

	// ensure the entry is a blob
	if(!ht_entry_is_blob(entry))
		return ERR_FAILURE;

	// check that the buffer size is less than the length specified for compare
	if(entry->data_size != buff_size)
		return ERR_FAILURE;

	// ensure data pointer is set
	if(!entry->data)
		return ERR_FAILURE;

	// return true if the buffers match
	if(memcmp(entry->data, buff, buff_size) == 0)
		return ERR_SUCCESS;

	// return indicating failure
	return ERR_FAILURE;

}

// memory compare hash table keyed entry (checks that string is reasonable printable)
BP_ERROR_T ht_strncmp_entry
(
		P_BP_HASH_TABLE_KEYED_ENTRY entry,
		char  *buff,
		size_t buff_size
)
{

	// ensure the entry is a blob
	if(!ht_entry_is_blob(entry))
		return ERR_FAILURE;

	// run string check
	if(!BP_StringIsReasonablePrintableString(buff, buff_size, BP_TRUE, BP_TRUE))
		return ERR_FAILURE;

	// after basic sanity check, run memmatch search to ensure entry matches buffer exactly
	if(!ht_memmatch_entry(entry, buff, buff_size))
		return ERR_FAILURE;

	// return indicating failure
	return ERR_FAILURE;

}


// --- value compares for keys ---------------

// memory compare hash table keyed entry (binary only compare).  Returns ERR_SUCCESS if
// the keys match (are exactly the same).  Returns ERR_FAILURE if keys differ at all.
BP_ERROR_T ht_memmatch_key
(
		P_BP_HASH_TABLE_KEYED_ENTRY entry,
		char  *buff,
		size_t buff_size
)
{

	// basic null checks
	if(!buff)
		return ERR_FAILURE;
	if(!buff_size)
		return ERR_FAILURE;
	if(!entry->key)
		return ERR_FAILURE;

	// check that the buffer size is less than the length specified for compare
	if(bpstrlen(entry->key) != buff_size)
		return ERR_FAILURE;

	// return true if the buffers match
	if(memcmp(entry->key, buff, buff_size) == 0)
		return ERR_SUCCESS;

	// return indicating failure
	return ERR_FAILURE;

}


// memory compare hash table keyed entry (binary only compare/front part match only).  Returns ERR_SUCCESS if
// the keys match (are matched within their front portions)
BP_ERROR_T ht_memmatch_key_front_part
(
		P_BP_HASH_TABLE_KEYED_ENTRY entry,
		char  *buff,
		size_t buff_size
)
{

	// basic null checks
	if(!buff)
		return ERR_FAILURE;
	if(!buff_size)
		return ERR_FAILURE;
	if(!entry->key)
		return ERR_FAILURE;

	// check that the buffer size is less than the length specified for compare
	if(bpstrlen(entry->key) < buff_size)
		return ERR_FAILURE;

	// return true if the buffers match
	if(memcmp(entry->key, buff, buff_size) == 0)
		return ERR_SUCCESS;

	// return indicating failure
	return ERR_FAILURE;

}

// string compare hash table keyed entry (binary only compare).  Returns ERR_SUCCESS if
// the keys match (are exactly the same).  Returns ERR_FAILURE if keys differ at all.
BP_ERROR_T ht_strncmp_key
(
		P_BP_HASH_TABLE_KEYED_ENTRY entry,
		char  *buff,
		size_t buff_size
)
{

	// run string check
	if(!BP_StringIsReasonablePrintableString(buff, buff_size, BP_TRUE, BP_TRUE))
		return ERR_FAILURE;

	// after basic sanity check, run memmatch search to ensure entry matches buffer exactly
	if(!ht_memmatch_key(entry, buff, buff_size))
		return ERR_FAILURE;

	// return indicating failure
	return ERR_SUCCESS;

}


// Checks to see if a key and value are the same.
BP_ERROR_T ht_entry_key_and_value_are_the_same
(
		P_BP_HASH_TABLE_KEYED_ENTRY entry
)
{

	// basic null checks
	if(!entry)
		return ERR_FAILURE;
	if(!entry->key)
		return ERR_FAILURE;

	// need to do a +1 byte check due to null terminators
	if(!ht_memmatch_entry(entry, entry->key, bpstrlen(entry->key)+1))
		return ERR_FAILURE;

	// return indicating failure
	return ERR_SUCCESS;

}



/***************************************************************************
 *            BP-LinuxMemMapping.h
 *
 *  Tue Apr 24 01:41:33 2007
 *  Copyright  2007  User
 *  Email
 ****************************************************************************/

#ifndef __BP_LINUX_MEM_MAPPING_H
#define __BP_LINUX_MEM_MAPPING_H

#if USE_BLOCKPARTY_DEBUGGING == 1


/*
 *
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%% 64Bit Debugging Interface %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 *
 */
#if BLOCKPARTY_DEBUGGING_TARGET_64BIT == 1

// Permissions (set as blockparty flags)
typedef enum BP_LINUX_PROC_MAP_PERMS
{
	BP_LINUX_PROC_MAP_PERM_UNSET,
	BP_LINUX_PROC_MAP_PERM_READ,
	BP_LINUX_PROC_MAP_PERM_WRITE,
	BP_LINUX_PROC_MAP_PERM_EXECUTE,
	BP_LINUX_PROC_MAP_PERM_SHARED,
	BP_LINUX_PROC_MAP_PERM_PRIVATE
};


// type definition for perm flags
typedef  size_t BP_LINUX_PROC_MAP_PERMISSION_FLAGS;

// individual linux process map entry
typedef struct __BP_LINUX_PROC_MAP {

	// start of the map
	uint64_t addr_bottom;
	uint64_t addr_top;

	// permissions (flags)
	BP_LINUX_PROC_MAP_PERMISSION_FLAGS permissions;

	// offset
	uint64_t offset;

	// device
	size_t device_minor;
	size_t device_major;

	// inode
	ino_t inode;

	// pathname
	char * pathname;

	// calculated size
	size_t size;

} BP_LINUX_PROC_MAP, *P_BP_LINUX_PROC_MAP;



// Structure to hold process maps.
typedef struct __BP_LINUX_PROC_MAPS {

	// file parsed
	char * maps_file;

	// maps in the table
	P_BP_LINUX_PROC_MAP maps;

	// number of maps in the table
	size_t maps_n;

} BP_LINUX_PROC_MAPS, *P_BP_LINUX_PROC_MAPS;




#endif




/*
 *
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%% 32Bit Debugging Interface (OLD) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 *
 */

#if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1



// looks at an size_t corresponding to the
// heap chunk size of a heap chunk and gets an
// accurate double word aligned heap size minus
// the eight bytes for its header and the remainder
// used for its flags.
size_t GetHeapSizeFromChunkValue(size_t heapSize);

// Retrieves an accurate memory mapping from
// a process being debugged. (via /proc fs)
int GetMemMap(P_DEBUG_HANDLE dbgHandle);

// Destroys a memory map list
int DestroyMemMap(P_DEBUG_HANDLE dbgHandle);

// Returns ERR_SUCCESS if an address correlates to valid
// process memory space, ERR_FAILURE on non-valid pointer
int IsPointerValidForProcess(P_DEBUG_HANDLE dbgHandle, void *procAddr);

// Returns the memory map corresponding to a valid process pointer
P_MEMORY_MAP GetMemoryMapFromPointer(P_DEBUG_HANDLE dbgHandle, void *procAddr);

// get void pointer from hexidecimal, used to indicate
// valid process memory
void * GetVoidPtrFromHexString(char * hexString);

// Prints out one single memory map list element
// (eight byte pointer max)
int PrintMemMapElement(P_MEMORY_MAP memMapHandle);

// Prints out an entire memory map list
int PrintMemMapList(P_MEMORY_MAP memMapHandle);

#endif

#endif
#endif // __BP_LINUX_MEM_MAPPING_H


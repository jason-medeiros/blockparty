/***************************************************************************
 *            BP-LinuxMemMapping.cc
 *
 *  Tue Apr 24 01:41:57 2007
 *  Copyright  2007  User
 *  Email
 ****************************************************************************/

#include "../../include/BP-Main.h"

#if USE_BLOCKPARTY_DEBUGGING == 1


/*
 *
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%% 64Bit Debugging Interface %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 *
 */
#if BLOCKPARTY_DEBUGGING_TARGET_64BIT == 1





#endif




/*
 *
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%% 32Bit Debugging Interface (OLD) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 *
 */

#if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1

#ifdef __GNUC__

// looks at an size_teger corresponding to the
// heap chunk size of a heap chunk and gets an
// accurate double word aligned heap size minus
// the eight bytes for its header and the remainder
// used for its flags.
size_t GetHeapSizeFromChunkValue(size_t heapSize)
{

	if(heapSize == 0)
		return 0;
	
	size_t remainder = 0;

	heapSize -= 8;
	remainder = heapSize % 8;
	heapSize -= remainder;
	
	
	return heapSize;
}
	

// get void pointer from hexidecimal
void * GetVoidPtrFromHexString(char * hexString)
{

	if(hexString == NULL)
		return 0;
	
	int position = 0;
	size_t ptrVal  = 0;
	
	for(position = 0; position < 8; position++)
	{

		// eight way, first byte first, loop unrolled
		// for optimization.
		switch(hexString[position]){
			case 'f':
				if(position == 0)
					ptrVal += 0xf0000000;
				else if (position == 1)
					ptrVal += 0xf000000;
				else if (position == 2)
					ptrVal += 0xf00000;
				else if (position == 3)
					ptrVal += 0xf0000;
				else if (position == 4)
					ptrVal += 0xf000;
				else if (position == 5)
					ptrVal += 0xf00;
				else if (position == 6)
					ptrVal += 0xf0;
				else if (position ==  7)
					ptrVal += 0xf;
				break;
			case 'e':
				if(position == 0)
					ptrVal += 0xe0000000;
				else if (position == 1)
					ptrVal += 0xe000000;
				else if (position == 2)
					ptrVal += 0xe00000;
				else if (position == 3)
					ptrVal += 0xe0000;
				else if (position == 4)
					ptrVal += 0xe000;
				else if (position == 5)
					ptrVal += 0xe00;
				else if (position == 6)
					ptrVal += 0xe0;
				else if (position ==  7)
					ptrVal += 0xe;
				break;
			case 'd':
				if(position == 0)
					ptrVal += 0xd0000000;
				else if (position == 1)
					ptrVal += 0xd000000;
				else if (position == 2)
					ptrVal += 0xd00000;
				else if (position == 3)
					ptrVal += 0xd0000;
				else if (position == 4)
					ptrVal += 0xd000;
				else if (position == 5)
					ptrVal += 0xd00;
				else if (position == 6)
					ptrVal += 0xd0;
				else if (position ==  7)
					ptrVal += 0xd;
				break;
			case 'c':
				if(position == 0)
					ptrVal += 0xc0000000;
				else if (position == 1)
					ptrVal += 0xc000000;
				else if (position == 2)
					ptrVal += 0xc00000;
				else if (position == 3)
					ptrVal += 0xc0000;
				else if (position == 4)
					ptrVal += 0xc000;
				else if (position == 5)
					ptrVal += 0xc00;
				else if (position == 6)
					ptrVal += 0xc0;
				else if (position ==  7)
					ptrVal += 0xc;
				break;
			case 'b':
				if(position == 0)
					ptrVal += 0xb0000000;
				else if (position == 1)
					ptrVal += 0xb000000;
				else if (position == 2)
					ptrVal += 0xb00000;
				else if (position == 3)
					ptrVal += 0xb0000;
				else if (position == 4)
					ptrVal += 0xb000;
				else if (position == 5)
					ptrVal += 0xb00;
				else if (position == 6)
					ptrVal += 0xb0;
				else if (position ==  7)
					ptrVal += 0xb;
				break;
			case 'a':
				if(position == 0)
					ptrVal += 0xa0000000;
				else if (position == 1)
					ptrVal += 0xa000000;
				else if (position == 2)
					ptrVal += 0xa00000;
				else if (position == 3)
					ptrVal += 0xa0000;
				else if (position == 4)
					ptrVal += 0xa000;
				else if (position == 5)
					ptrVal += 0xa00;
				else if (position == 6)
					ptrVal += 0xa0;
				else if (position ==  7)
					ptrVal += 0xa;
				break;
			
			case '9':
				if(position == 0)
					ptrVal += 0x90000000;
				else if (position == 1)
					ptrVal += 0x9000000;
				else if (position == 2)
					ptrVal += 0x900000;
				else if (position == 3)
					ptrVal += 0x90000;
				else if (position == 4)
					ptrVal += 0x9000;
				else if (position == 5)
					ptrVal += 0x900;
				else if (position == 6)
					ptrVal += 0x90;
				else if (position ==  7)
					ptrVal += 0x9;
				break;
			case '8':
				if(position == 0)
					ptrVal += 0x80000000;
				else if (position == 1)
					ptrVal += 0x8000000;
				else if (position == 2)
					ptrVal += 0x800000;
				else if (position == 3)
					ptrVal += 0x80000;
				else if (position == 4)
					ptrVal += 0x8000;
				else if (position == 5)
					ptrVal += 0x800;
				else if (position == 6)
					ptrVal += 0x80;
				else if (position ==  7)
					ptrVal += 0x8;
				break;
			case '7':
				if(position == 0)
					ptrVal += 0x70000000;
				else if (position == 1)
					ptrVal += 0x7000000;
				else if (position == 2)
					ptrVal += 0x700000;
				else if (position == 3)
					ptrVal += 0x70000;
				else if (position == 4)
					ptrVal += 0x7000;
				else if (position == 5)
					ptrVal += 0x700;
				else if (position == 6)
					ptrVal += 0x70;
				else if (position ==  7)
					ptrVal += 0x7;
				break;
			case '6':
				if(position == 0)
					ptrVal += 0x60000000;
				else if (position == 1)
					ptrVal += 0x6000000;
				else if (position == 2)
					ptrVal += 0x600000;
				else if (position == 3)
					ptrVal += 0x60000;
				else if (position == 4)
					ptrVal += 0x6000;
				else if (position == 5)
					ptrVal += 0x600;
				else if (position == 6)
					ptrVal += 0x60;
				else if (position ==  7)
					ptrVal += 0x6;
				break;
			case '5':
				if(position == 0)
					ptrVal += 0x50000000;
				else if (position == 1)
					ptrVal += 0x5000000;
				else if (position == 2)
					ptrVal += 0x500000;
				else if (position == 3)
					ptrVal += 0x50000;
				else if (position == 4)
					ptrVal += 0x5000;
				else if (position == 5)
					ptrVal += 0x500;
				else if (position == 6)
					ptrVal += 0x50;
				else if (position ==  7)
					ptrVal += 0x5;
				break;
			case '4':
				if(position == 0)
					ptrVal += 0x40000000;
				else if (position == 1)
					ptrVal += 0x4000000;
				else if (position == 2)
					ptrVal += 0x400000;
				else if (position == 3)
					ptrVal += 0x40000;
				else if (position == 4)
					ptrVal += 0x4000;
				else if (position == 5)
					ptrVal += 0x400;
				else if (position == 6)
					ptrVal += 0x40;
				else if (position ==  7)
					ptrVal += 0x4;
				break;
			case '3':
				if(position == 0)
					ptrVal += 0x30000000;
				else if (position == 1)
					ptrVal += 0x3000000;
				else if (position == 2)
					ptrVal += 0x300000;
				else if (position == 3)
					ptrVal += 0x30000;
				else if (position == 4)
					ptrVal += 0x3000;
				else if (position == 5)
					ptrVal += 0x300;
				else if (position == 6)
					ptrVal += 0x30;
				else if (position ==  7)
					ptrVal += 0x3;
				break;
			case '2':
				if(position == 0)
					ptrVal += 0x20000000;
				else if (position == 1)
					ptrVal += 0x2000000;
				else if (position == 2)
					ptrVal += 0x200000;
				else if (position == 3)
					ptrVal += 0x20000;
				else if (position == 4)
					ptrVal += 0x2000;
				else if (position == 5)
					ptrVal += 0x200;
				else if (position == 6)
					ptrVal += 0x20;
				else if (position ==  7)
					ptrVal += 0x2;
				break;
			case '1':
				if(position == 0)
					ptrVal += 0x10000000;
				else if (position == 1)
					ptrVal += 0x1000000;
				else if (position == 2)
					ptrVal += 0x100000;
				else if (position == 3)
					ptrVal += 0x10000;
				else if (position == 4)
					ptrVal += 0x1000;
				else if (position == 5)
					ptrVal += 0x100;
				else if (position == 6)
					ptrVal += 0x10;
				else if (position ==  7)
					ptrVal += 0x1;
				break;
			
			// or zero (dont add any values)
			default:
				break;
		}
					
	}
	return (void *) ptrVal;
}


// Retrieves an accurate memory mapping from
// a process being debugged. (via /proc fs), 
// lots of string parsing below.
int GetMemMap(P_DEBUG_HANDLE dbgHandle)
{

	P_MEMORY_MAP memList = NULL;
	if(dbgHandle == NULL){
			dbgHandle->memory = NULL;
			return ERR_DBG_HANDLE_NULL;
	}
	
	FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
	int i;
	int isFirst = 0;
	
	unsigned char *elfReadBuff;
	
	
	P_MEMORY_MAP first = NULL;
	P_MEMORY_MAP prev = NULL;

	
	// various necessary strings
	char * strPtr  = NULL;
	void * addrPtr = NULL;
	char * valPtr  = NULL;
	
	// opener string
	char openString[1000];
	char pidString [100];
	
		memset(openString, 0x00, sizeof(openString));
		memset(pidString , 0x00, sizeof(pidString));
	
	// attach proc string verifying pid
	strcat(openString, "/proc/");
	snprintf(pidString, 10, "%i", dbgHandle->debugPid);
	strcat(openString, pidString);
	strcat(openString, "/maps");
	
    fp = fopen(openString, "r");
    if (fp == NULL){
		dbgHandle->memory = NULL;
		return ERR_CANNOT_OPEN_FILE;
	}
	
	// read lines out of the maps file
    while ((read = getline(&line, &len, fp)) != -1) {
				
        // create our first memory List
		memList = (P_MEMORY_MAP) bpmalloc(sizeof(MEMORY_MAP));
			memset(memList, 0x00, sizeof(MEMORY_MAP)); 
		
		// set the list elements first pointer if its
		// first.
		if(isFirst == 0){
			
			first = memList;
		}
		
		memList->first = first;
		if(prev != NULL){
			memList->prev = prev;
			prev->next = memList;
		}
		
		memList->next = NULL;
		strPtr = line;
		
		// this is the first read, which is the
		// memory mapping low pointer
		addrPtr = (void *) bpstrndup(strPtr, 8);
		if(addrPtr == NULL){
			dbgHandle->memory = NULL;
			return ERR_STRNDUP_FAILED;
		}
			memList->lowAddr = GetVoidPtrFromHexString((char *)addrPtr);
				bpfree(addrPtr);
			addrPtr = NULL;
			
		// retrieve the high pointer
		strPtr = &strPtr[9];
		addrPtr = (void *) bpstrndup(strPtr, 8);
		if(addrPtr == NULL){
			dbgHandle->memory = NULL;
			return ERR_STRNDUP_FAILED;
		}
			
		memList->highAddr = GetVoidPtrFromHexString((char*)addrPtr);
			bpfree(addrPtr);
			
		// Determine if section is ELF, compare first 4 bytes
		elfReadBuff = DebugReadMemory(dbgHandle, memList->lowAddr, 4);
		
		if(memcmp(elfReadBuff, "\x7f\x45\x4c\x46", 4) == 0){
				memList->elf++;
		} 
		if(elfReadBuff != NULL){
			bpfree(elfReadBuff);
		}
		
		// second line is the permissions
		strPtr = strstr(strPtr, " ");
		if(strPtr == NULL){
			
			dbgHandle->memory = NULL;
			return ERR_MEMMAP_CANNOT_ADVANCE_STRING;
		}
			strPtr = &strPtr[1];
		valPtr =  bpstrndup(strPtr, 4);
		
		if(valPtr == NULL)
			return ERR_STRNDUP_FAILED;
		
		if(valPtr[0] == 'r')
			memList->read++;
		if(valPtr[1] == 'w')
			memList->write++;
		if(valPtr[2] == 'x')
			memList->execute++;
		if(valPtr[3] == 's')
			memList->shared++;
		if(valPtr[3] == 'p')
			memList->priv++;
		
		
		
		// if(valPtr != NULL)
		//	bpfree(valPtr);
		
		// third skip
		strPtr = strstr(strPtr, " ");
		if(strPtr == NULL){
			dbgHandle->memory = NULL;
			return ERR_MEMMAP_CANNOT_ADVANCE_STRING;
		}
			strPtr = &strPtr[1];
		
		// fourth skip
		strPtr = strstr(strPtr, " ");
		if(strPtr == NULL){
			dbgHandle->memory = NULL;
			return ERR_MEMMAP_CANNOT_ADVANCE_STRING;
		}
			strPtr = &strPtr[1];
		
		// sixth skip
		strPtr = strstr(strPtr, " ");
		if(strPtr == NULL){
			dbgHandle->memory = NULL;
			return ERR_MEMMAP_CANNOT_ADVANCE_STRING;
		}
			strPtr = &strPtr[1];
			
		// seventh skip
		strPtr = strstr(strPtr, " ");
		if(strPtr == NULL){
			dbgHandle->memory = NULL;
			return ERR_MEMMAP_CANNOT_ADVANCE_STRING;
		}
			strPtr = &strPtr[1];
		
		// Eigth (or 7 from 0) is our 
		// path string
		
		// this small algorithm is used to
		// increment passed spaces or tabs.
		for(i = 0;strPtr[i] == ' ' || strPtr[i] == '\t'; i++){
			if(strPtr[i] == 0x00)
				break;
		}
			
		
		strPtr = &strPtr[i];
		memList->path = bpstrndup(strPtr, 1000);
		// remove the last new line
		memList->path[strlen(memList->path)-1] = 0x00;
		
		// save previous list
		prev = memList;
		isFirst++;
		
		// bpfree the line
		if(line != NULL){
			bpfree(line);
			line = NULL;
		}
		
    }
	
	if(line != NULL){
			bpfree(line);
			line = NULL;
	}
	
	dbgHandle->memory = first;
	
	// close and bpfree read buffer
	fclose(fp);
	
	
	return ERR_SUCCESS;
	
}

// Prints out one single memory map list element 
int PrintMemMapElement(P_MEMORY_MAP memMapHandle)
{

	if(memMapHandle == NULL)
		return ERR_MEMMAP_NULL;
	
	printf("\n");
	printf("\n+--------------------------------+");
	printf("\n| Memory Map Element: %p  |", memMapHandle);
	printf("\n+--------------------------------+");
	
	printf("\n\t Range Origin: %s", memMapHandle->path);
	printf("\n\t\t High Address: %p", memMapHandle->highAddr);
	printf("\n\t\t Low  Address: %p", memMapHandle->lowAddr);
	
	printf("\n\t\t\t Read: ");
		if(memMapHandle->read > 0)
			printf("YES");			
		else
			printf("NO");
		
	printf("\n\t\t\t Write: ");
		if(memMapHandle->write > 0)
			printf("YES");			
		else
			printf("NO");
		
	printf("\n\t\t\t execute: ");
		if(memMapHandle->execute > 0)
			printf("YES");			
		else
			printf("NO");
	printf("\n\t\t\t Private: ");
		if(memMapHandle->priv > 0)
			printf("YES");			
		else
			printf("NO");
	printf("\n\t\t\t Shared: ");
		if(memMapHandle->shared > 0)
			printf("YES");			
		else
			printf("NO");

	printf("\n\t\t first: %p", memMapHandle->first);
	printf("\n\t\t prev:  %p", memMapHandle->prev);
	printf("\n\t\t next:  %p", memMapHandle->next);
	printf("\n");
	return ERR_SUCCESS;	
}

// Prints out an entire memory map list
int PrintMemMapList(P_MEMORY_MAP memMapHandle)
{

	if(memMapHandle == NULL)
		return ERR_MEMMAP_NULL;
	
	for
	(
		memMapHandle = memMapHandle->first; \
		memMapHandle != NULL;\
		memMapHandle = memMapHandle->next){
			PrintMemMapElement(memMapHandle);
	}
	return ERR_SUCCESS;
}


// Destroys a memory map given a debugging handle
int DestroyMemMap(P_DEBUG_HANDLE dbgHandle){

	// test handle
	if(dbgHandle == NULL)
		return ERR_DBG_HANDLE_NULL;
	
	// test memory
	if(dbgHandle->memory == NULL)
		return ERR_MEMMAP_NULL;
	
	if(dbgHandle->memory->next == NULL){
		return ERR_MEMMAP_NULL;
	}
		
	P_MEMORY_MAP next = dbgHandle->memory->next;
	
	
	for(dbgHandle->memory = dbgHandle->memory->first;
		dbgHandle->memory != NULL;
		dbgHandle->memory = next){

		next = dbgHandle->memory->next;
		
		// bpfree path if allocated
		if(dbgHandle->memory->path != NULL){
			bpfree(dbgHandle->memory->path);
		}
		
		// bpfree main structure
		bpfree(dbgHandle->memory);
		if(next == NULL){
			dbgHandle->memory = NULL;
			return ERR_SUCCESS;
		}
	
	}


	return ERR_SUCCESS;
}

// Returns ERR_SUCCESS if an address correlates to valid
// process memory space, ERR_FAILURE on non-valid pointer
int IsPointerValidForProcess(P_DEBUG_HANDLE dbgHandle, void *procAddr)
{
	
	if(dbgHandle == NULL)
		return ERR_FAILURE;
	
	if(dbgHandle->memory == NULL)
		return ERR_FAILURE;
	
	P_MEMORY_MAP first = dbgHandle->memory->first;
	
	for(dbgHandle->memory = dbgHandle->memory->first;
		dbgHandle->memory != NULL;
		dbgHandle->memory = dbgHandle->memory->next){
			
		if(procAddr <= dbgHandle->memory->highAddr)
		if(procAddr >= dbgHandle->memory->lowAddr){
			dbgHandle->memory = first;
			return ERR_SUCCESS;
		}
		
	}
	
	dbgHandle->memory = first;
	return ERR_FAILURE;
}

// Returns the memory map corresponding to a valid process pointer
P_MEMORY_MAP GetMemoryMapFromPointer(P_DEBUG_HANDLE dbgHandle, void *procAddr)
{
	
	if(dbgHandle == NULL)
		return NULL;
	
	if(dbgHandle->memory == NULL)
		return NULL;
	
	// store the first pointer
	P_MEMORY_MAP first = dbgHandle->memory->first;
	P_MEMORY_MAP retPtr = NULL;
	
	if( IsPointerValidForProcess(dbgHandle, procAddr) != ERR_SUCCESS)
		return NULL;
	
	// loop through all memory 
	for(dbgHandle->memory = dbgHandle->memory->first;
		dbgHandle->memory != NULL;
		dbgHandle->memory = dbgHandle->memory->next){
			
		if(procAddr <= dbgHandle->memory->highAddr)
		if(procAddr >= dbgHandle->memory->lowAddr){
			retPtr = dbgHandle->memory;
			dbgHandle->memory = first;
			
			return retPtr;
		}
		
	}
	
	// reset memory pointer
	dbgHandle->memory = first;
	
	return NULL;
}

#endif // USE_BLOCKPARTY_DEBUGGING == 1

#endif

#endif

#include "../../../include/BP-Main.h"

#if USE_BLOCKPARTY_DEBUGGING == 1

#if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1


// extracts relevant data from the elf header
P_ELF_32BIT_LOOKUP_TABLE ELF32_ExtractAll_Mem(P_DEBUG_HANDLE dbgHandle, P_MEMORY_MAP memMap){

	// Default NULL checks
	if(dbgHandle == NULL)
		return NULL;
	if(memMap == NULL)
		return NULL;

	if(memMap != NULL){
		// Destroy the elf header before continuing
		// ELF32_DestroyAll.
	}

	// Make sure the debugger appears to be attached
	// before attempting to read in header.
	if(dbgHandle->attached != 1)
		return NULL;

	// create space for our data
	P_ELF_32BIT_LOOKUP_TABLE fullHeader = (P_ELF_32BIT_LOOKUP_TABLE) bpmalloc(sizeof(ELF_32BIT_LOOKUP_TABLE));
		memset(fullHeader, 0x00, sizeof(ELF_32BIT_LOOKUP_TABLE));

	// Copy the path of the memory map into the struct
	if(memMap->path != NULL){
		fullHeader->filePath = bpstrndup(memMap->path, MAX_CMD_LENGTH);
	} else {
		fullHeader->filePath = (char *) NULL;
	}

	// ------- BEGIN HEADER EXTRACTION --------------------------------

	// Extract ELF32 Header
	fullHeader->header = ELF32_ExtractHeader_Mem(dbgHandle, memMap);
	if(fullHeader->header == NULL)
	 	return NULL;

	// Extract Program Headers
	fullHeader->pHeaders = ELF32_ExtractProgramHeaders_Mem(fullHeader, dbgHandle, memMap);

	// Extract all the section headers from the file
	fullHeader->sections = ELF32_ExtractSectionHeaders_Mem(fullHeader, dbgHandle, memMap);

	// Now extract all sections from the file
	ELF32_ExtractSections_Mem(fullHeader, dbgHandle, memMap);

	// Extract Symbol Table from the header
	// printf("\n header?: %p", fullHeader->symTab);

	fullHeader->symTab = ELF32_TranslateSymbolTableFromData(fullHeader);

	// Extract all relocations
	fullHeader->relocs = ELF32_TranslateRelocationsFromData(fullHeader);

	// Extract Dynamics
	fullHeader->dynamics = ELF32_TranslateDynamicsFromData(fullHeader);

	// Extract GOT
	fullHeader->got = ELF32_TranslateGOTFromData(fullHeader);

	return fullHeader;
}

// returns a formatted elf header from a file on the filesystem
P_ELF_HDR_32BIT ELF32_ExtractHeader_Mem(P_DEBUG_HANDLE dbgHandle, P_MEMORY_MAP memMap){

	P_ELF_HDR_32BIT header = (P_ELF_HDR_32BIT) bpmalloc(sizeof(ELF_HDR_32BIT));
		memset(header, 0x00, sizeof(ELF_HDR_32BIT));

	// Retrieve memory
	header = (P_ELF_HDR_32BIT) DebugReadMemory(dbgHandle, memMap->lowAddr, sizeof(ELF_HDR_32BIT));
	if(header == NULL){
		return NULL;
	}

	return header;
}

// returns a section list of all sections in a file given
// a valid elf header.
P_ELF_SECTION_LIST_32BIT ELF32_ExtractSectionHeaders_Mem(P_ELF_32BIT_LOOKUP_TABLE fullHeader, P_DEBUG_HANDLE dbgHandle, P_MEMORY_MAP memMap){

	P_ELF_HDR_32BIT elfHdr = fullHeader->header;
	P_ELF_SECTION_HDR_32BIT testVal = NULL;

	if(elfHdr == NULL)
		return NULL;

	// Section Header Offset, passed to sub function to extract headers
	size_t sOffset = (size_t) elfHdr->elfSectionHeaderOffset + (unsigned int) memMap->lowAddr;

	// Retrieve Section Count
	unsigned short sectionCount = elfHdr->elfNumberSHeaderEntries;
	if(sectionCount < 1)
		return NULL;

	P_ELF_SECTION_LIST_32BIT sectionList = NULL;

	// create the first section list
	if(sectionCount > 0){
		sectionList = ELF32_NewSectionList();
		if(sectionList != NULL){
			memset(sectionList, 0x00, sizeof(ELF_SECTION_LIST_32BIT));
		} else {
			return NULL;
		}

		sectionList->first = sectionList;
		sectionList->next = NULL;
		sectionList->prev = NULL;

	} else {
		return NULL;
	}

	// Create offset to string table
	// Section Header offset + Section String Table Offset
	size_t stringTableIndex = elfHdr->elfSHeaderStringTableIndex;
	size_t i = 0;

	// attempt to extract sectionCount number of section headers from
	// the binary
	for(;sectionCount != 0; sectionCount--){

		// Extract Actual Section Info
		testVal = ELF32_ExtractSectionHeader_Mem(fullHeader, sOffset, dbgHandle, memMap);
		if(testVal == NULL){
			sectionList = NULL;
			break;
		} else {
			sectionList->section = testVal;
		}

		if(sectionList->section != NULL){
			sOffset += sizeof(ELF_SECTION_HDR_32BIT);
		}

		// printf("\n %x %i WTFn", sectionList->section->offset, sectionCount);
		if( (sectionCount - 1) == 0)
			break;

		// perform next assignments
		sectionList->next = ELF32_NewSectionList();
			sectionList->next->prev  = sectionList;
			sectionList->next->first = sectionList->first;
			sectionList = sectionList->next;
			sectionList->next = NULL;
	}

	// Reset position
	if(sectionList != NULL){
		sectionList = sectionList->first;
	}

	//-------------------------------------------------------------
	// Find and store the string table
	i = 0;
	size_t sTableOffset = 0;

	// Save first pointer before loop
	P_ELF_SECTION_LIST_32BIT firstPtr = NULL;
	if(sectionList != NULL){

		firstPtr = sectionList->first;
		for(sectionList = sectionList->first; sectionList != NULL; sectionList = sectionList->next){

			// Condition Reached: String Table Index Found
			if( i == stringTableIndex && sectionList != NULL){

				fullHeader->stringTableSize = sectionList->section->size;
				sTableOffset = sectionList->section->offset + (size_t) memMap->lowAddr;

				// Extract table from file
				// rewind(elfFile);
				//fseek(elfFile, sTableOffset, SEEK_SET);
				// fread(fullHeader->stringTable, fullHeader->stringTableSize, 1, elfFile);

				// printf("\n WTF: %p, %p, %p", dbgHandle, (void *)  sTableOffset, fullHeader->stringTableSize);
				fullHeader->stringTable = (char *) DebugReadMemory(dbgHandle, (void *)  sTableOffset, fullHeader->stringTableSize);

				break;
			}
			i++;
		}
	}


	// Now that the string table has been retrieved, assign each section its appropriate
	// name.

	if(sectionList != NULL && firstPtr != NULL){

		sectionList = firstPtr;
		for(sectionList = sectionList->first; sectionList != NULL; sectionList = sectionList->next){
			if(sectionList->section->name > 0){
				sectionList->name = bpstrndup(&fullHeader->stringTable[sectionList->section->name], ELF32_MAX_NAME_LENGTH);
			} else
				sectionList->name = "NULL";
		}
	}

	if(firstPtr != NULL){
		sectionList = firstPtr;
	}


	return sectionList;
}


// extract a section and return a section list element; all pointers are
// NULL.  Offset describes the distance to the next header.
P_ELF_SECTION_HDR_32BIT ELF32_ExtractSectionHeader_Mem(P_ELF_32BIT_LOOKUP_TABLE fullHeader, size_t sOffset,  P_DEBUG_HANDLE dbgHandle, P_MEMORY_MAP memMap){

	// default checks
	if(fullHeader == NULL)
		return NULL;

	if(fullHeader->header == NULL)
		return NULL;

	if(dbgHandle == NULL)
		return NULL;

	if(memMap == NULL)
		return NULL;

	if( (sOffset < (size_t)memMap->lowAddr) ||
		(sOffset > (size_t) memMap->highAddr) ){
		// Return null
		return NULL;
	}


	P_ELF_SECTION_HDR_32BIT header = NULL;

	// return NULL if we cannot read out of the file
	//if( fread((void *) header, 1, sizeof(ELF_SECTION_HDR_32BIT), elfFile) == 0)
	//	return NULL;


	header = (P_ELF_SECTION_HDR_32BIT) DebugReadMemory(dbgHandle, (void *) sOffset, sizeof(ELF_SECTION_HDR_32BIT));
	if(header == NULL)
		return NULL;

	return header;
}

// returns a linked list representing all of the program headers found in the
// file.
P_ELF_PROGRAM_HEADER_LIST_32BIT ELF32_ExtractProgramHeaders_Mem(P_ELF_32BIT_LOOKUP_TABLE fullHeader, P_DEBUG_HANDLE dbgHandle, P_MEMORY_MAP memMap){

	if(fullHeader == NULL)
		return NULL;

	// if(elfFile == NULL)
	// 	return NULL;

	// Get the memory map offset (mem specific)
	size_t phOffset = fullHeader->header->elfProgramHeaderOffset +  (size_t) memMap->lowAddr;;

	size_t phSize = fullHeader->header->elfProgramHeaderSize;
	size_t phEntries = fullHeader->header->elfNumberPHeaderEntries;

	size_t phCurrentOffset = phOffset;

	// Rewind to assure position
	// rewind(elfFile);

	P_ELF_PROGRAM_HEADER_LIST_32BIT pHeaderList;

	// create first structure
	if(phEntries > 0){

		// Create first element
		pHeaderList = (P_ELF_PROGRAM_HEADER_LIST_32BIT) bpmalloc(sizeof(ELF_PROGRAM_HEADER_LIST_32BIT));
			memset(pHeaderList, 0x00, sizeof(ELF_PROGRAM_HEADER_LIST_32BIT));

		// Set first pointer, other pointers are null by memset
		pHeaderList->first = pHeaderList;

		// Create space for actual header: Should Be Unnecessary for Memory Reads
		// pHeaderList->header = (P_ELF_PROGRAM_HEADER_32BIT) bpmalloc(sizeof(ELF_PROGRAM_HEADER_32BIT));
		//	memset(pHeaderList->header, 0x00, sizeof(ELF_PROGRAM_HEADER_32BIT));

	} else {
		// return Null if there are no program header entries
		return NULL;
	}

	// Loop through every entry, collecting and saving the structures
	for(;phEntries > 0; phEntries--){

		// Seek to the current offset
		// fseek(elfFile, phCurrentOffset, SEEK_SET);


		// Copy in header
		// fread(pHeaderList->header, 1, phSize, elfFile);
		pHeaderList->header = (P_ELF_PROGRAM_HEADER_32BIT) DebugReadMemory(dbgHandle, (void *) phCurrentOffset, phSize);

		// Move list position
		if(phEntries - 1 == 0){
			pHeaderList->next = NULL;
			break;

		} else {
			// Create Next List
			pHeaderList->next = (P_ELF_PROGRAM_HEADER_LIST_32BIT) bpmalloc(sizeof(ELF_PROGRAM_HEADER_LIST_32BIT));
				memset(pHeaderList->next, 0x00, sizeof(ELF_PROGRAM_HEADER_LIST_32BIT));

			// Reset Pointers
			pHeaderList->next->prev = pHeaderList;
			pHeaderList->next->first = pHeaderList->first;
			pHeaderList->next->next = NULL;

			// Move List Forward
			pHeaderList = pHeaderList->next;

		}

		// Increment offset
		phCurrentOffset += phSize;

	}


	// Return first element
	return pHeaderList->first;
}

// Extract actual sections from the full headers section list (no types)
int ELF32_ExtractSections_Mem(P_ELF_32BIT_LOOKUP_TABLE fullHeader, P_DEBUG_HANDLE dbgHandle, P_MEMORY_MAP memMap){

	if(fullHeader == NULL)
		return ERR_FAILURE;

	if(fullHeader->sections == NULL)
		return ERR_FAILURE;

	size_t runtimeOffset = 0;

	P_ELF_SECTION_LIST_32BIT firstPtr = fullHeader->sections->first;
	for(fullHeader->sections = fullHeader->sections->first; \
		fullHeader->sections != NULL; \
		fullHeader->sections = fullHeader->sections->next){

			// fseek(elfFile, fullHeader->sections->section->offset, SEEK_SET);
			// fullHeader->sections->section->size
			runtimeOffset = (size_t) fullHeader->sections->section->offset + (size_t) memMap->lowAddr;
			fullHeader->sections->data = (unsigned char *) DebugReadMemory(dbgHandle, (void *) runtimeOffset, fullHeader->sections->section->size);

			// Save Size
			fullHeader->sections->dataSize = fullHeader->sections->section->size;

	}

	// Reset pointer
	fullHeader->sections = 	firstPtr;

	return ERR_SUCCESS;
}

// This function is responsible for the extraction of all elf
// sections in memory, given a valid debug handle, and memory map.
// Will return a linked list with each element containing a
// elf lookup table which can be used to make determinations about
// process health.
P_ELF_32BIT_LOOKUP_TABLE_LIST ELF32_ExtractHeaderListAll_Mem(P_DEBUG_HANDLE dbgHandle){

	if(dbgHandle == NULL)
		return NULL;

	if(dbgHandle->memory == NULL)
		return NULL;

	P_MEMORY_MAP memMap = NULL;
	P_MEMORY_MAP memMapFirst = dbgHandle->memory->first;

	P_ELF_32BIT_LOOKUP_TABLE_LIST elfLookupList = (P_ELF_32BIT_LOOKUP_TABLE_LIST ) bpmalloc(sizeof(ELF_32BIT_LOOKUP_TABLE_LIST));
	memset(elfLookupList, 0x00, sizeof(ELF_32BIT_LOOKUP_TABLE_LIST));
	elfLookupList->first = elfLookupList;
	elfLookupList->prev = NULL;
	elfLookupList->next = NULL;

	// Used to make runtime calculations for list operations
	int fullELFCount = 0;
	int counter = 0;

	for(memMap = dbgHandle->memory->first; memMap != NULL; memMap = memMap->next){
		if(memMap->elf > 0){
			fullELFCount++;
		}
	}

	for(memMap = dbgHandle->memory->first; memMap != NULL; memMap = memMap->next){
		 if(memMap->elf > 0){

        	elfLookupList->header = ELF32_ExtractAll_Mem(dbgHandle, memMap);
        	elfLookupList->header->memMap = memMap;
        	elfLookupList->header->memMap->elfHeader = (void *) elfLookupList;


        	 if(elfLookupList->header != NULL){
        	     //  ELF32_PrintAll(elfLookupList->elf);
        	 }
        	 counter++;

        	 // Terminate list and exit if the last
        	 // elf section was retrived
        	 if(counter == fullELFCount){
        		 elfLookupList->next = NULL;
        		 break;
        	 }

        	 // Link up a new list and continue at top
        	 elfLookupList->next = (P_ELF_32BIT_LOOKUP_TABLE_LIST ) bpmalloc(sizeof(ELF_32BIT_LOOKUP_TABLE_LIST));
        	 	memset(elfLookupList->next, 0x00, sizeof(ELF_32BIT_LOOKUP_TABLE_LIST));
        	 	elfLookupList->next->first = elfLookupList->first;
        	 	elfLookupList->next->prev = elfLookupList;
        	 	elfLookupList = elfLookupList->next;
        	 	elfLookupList->next = NULL;


		 }
	}
	if(	elfLookupList != NULL )
		return elfLookupList->first;

	return NULL;
}

#endif // #if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1

#endif // USE_BLOCKPARTY_DEBUGGING == 1

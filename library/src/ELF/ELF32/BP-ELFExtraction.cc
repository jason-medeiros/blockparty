#include "../../../include/BP-Main.h"

#if USE_BLOCKPARTY_DEBUGGING == 1


#if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1

// extracts relevant data from the elf header
P_ELF_32BIT_LOOKUP_TABLE ELF32_ExtractAll_File(char * executablePath){

	// first open our target file for reading
	FILE * elfFile = FOpen(executablePath, "r");
	if(elfFile == NULL)
		return NULL;

	// create space for our data
	P_ELF_32BIT_LOOKUP_TABLE fullHeader = (P_ELF_32BIT_LOOKUP_TABLE) bpmalloc(sizeof(ELF_32BIT_LOOKUP_TABLE));
		memset(fullHeader, 0x00, sizeof(ELF_32BIT_LOOKUP_TABLE));

	// Copy the path into the struct
	fullHeader->filePath = bpstrndup(executablePath, MAX_CMD_LENGTH);

	// ------- BEGIN HEADER EXTRACTION --------------------------------

	// Extract ELF32 Header
	fullHeader->header = ELF32_ExtractHeader_File(elfFile);
	if(fullHeader->header == NULL)
		return NULL;

	// Extract Program Headers
	fullHeader->pHeaders = ELF32_ExtractProgramHeaders_File(fullHeader, elfFile);

	// Extract all the section headers from the file
	fullHeader->sections = ELF32_ExtractSectionHeaders_File(fullHeader, elfFile);
	if(fullHeader->sections == NULL)
		return NULL;



	// Now extract all sections from the file
	ELF32_ExtractSections_File(fullHeader, elfFile);


	// Extract Symbol Table from the header
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
P_ELF_HDR_32BIT ELF32_ExtractHeader_File(FILE *elfFile){

	if(elfFile == NULL)
		return NULL;

	P_ELF_HDR_32BIT header = (P_ELF_HDR_32BIT) bpmalloc(sizeof(ELF_HDR_32BIT));
		memset(header, 0x00, sizeof(ELF_HDR_32BIT));

	// return NULL if we cannot read out of the file
	if( fread((void *) header, 1, sizeof(ELF_HDR_32BIT), elfFile) == 0)
		return NULL;

	return header;
}

// returns a section list of all sections in a file given
// a valid elf header.
P_ELF_SECTION_LIST_32BIT ELF32_ExtractSectionHeaders_File(P_ELF_32BIT_LOOKUP_TABLE fullHeader, FILE *elfFile){

	P_ELF_HDR_32BIT elfHdr = fullHeader->header;

	if(elfHdr == NULL)
		return NULL;

	if(elfFile == NULL)
		return NULL;

	// rewind elf file
	rewind(elfFile);

	if(fseek(elfFile, elfHdr->elfSectionHeaderOffset, SEEK_SET) == -1)
		return NULL;

	// Retrieve Section Count
	unsigned short sectionCount = elfHdr->elfNumberSHeaderEntries;
	if(sectionCount < 1)
		return NULL;

	P_ELF_SECTION_LIST_32BIT sectionList;

	// create the first section list
	if(sectionCount > 0){
		sectionList = ELF32_NewSectionList();
		if(sectionList != NULL)
			memset(sectionList, 0x00, sizeof(ELF_SECTION_LIST_32BIT));

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
		sectionList->section = ELF32_ExtractSectionHeader_File(fullHeader, elfFile);


		if( (sectionCount - 1) == 0)
			break;

		// perform next assignments
		sectionList->next = ELF32_NewSectionList();
			sectionList->next->prev  = sectionList;
			sectionList->next->first = sectionList->first;
			sectionList = sectionList->next;
			sectionList->next = NULL;
	}

	//-------------------------------------------------------------
	// Find and store the string table
	i = 0;
	size_t sTableOffset = 0;

	// Save first pointer before loop
	P_ELF_SECTION_LIST_32BIT firstPtr = sectionList->first;
	for(sectionList = sectionList->first; sectionList != NULL; sectionList = sectionList->next){

		// arbitrary continue on a null section
		if(sectionList->section == NULL)
			continue;

		// Condition Reached: String Table Index Found
		if( i == stringTableIndex){

			// Full Header String Table
			fullHeader->stringTable = (char *) bpmalloc(sectionList->section->size+10);
				memset(fullHeader->stringTable, 0x00, sectionList->section->size+10);

			fullHeader->stringTableSize = sectionList->section->size;
			sTableOffset = sectionList->section->offset;

			// Extract table from file
			rewind(elfFile);
			fseek(elfFile, sTableOffset, SEEK_SET);
			fread(fullHeader->stringTable, fullHeader->stringTableSize, 1, elfFile);
			break;
		}
		i++;
	}

	// Now that the string table has been retrieved, assign each section its appropriate
	// name.

	sectionList = firstPtr;
	for(sectionList = sectionList->first; sectionList != NULL; sectionList = sectionList->next){

		// arbitrary continue on null section
		if(sectionList->section == NULL)
			continue;

		if(sectionList->section->name > 0){
			sectionList->name = bpstrndup(&fullHeader->stringTable[sectionList->section->name], ELF32_MAX_NAME_LENGTH);
		} else
			sectionList->name = "NULL";
	}

	// reset pointer and return
	sectionList = firstPtr;
	return sectionList;
}





// extract a section and return a section list element; all pointers are
// NULL.
P_ELF_SECTION_HDR_32BIT ELF32_ExtractSectionHeader_File(P_ELF_32BIT_LOOKUP_TABLE fullHeader, FILE *elfFile){

	// default checks
	if(fullHeader == NULL)
		return NULL;

	if(fullHeader->header == NULL)
		return NULL;

	if(elfFile == NULL)
		return NULL;

	P_ELF_HDR_32BIT elfHdr = fullHeader->header;

	P_ELF_SECTION_HDR_32BIT header = (P_ELF_SECTION_HDR_32BIT) bpmalloc(sizeof(ELF_SECTION_HDR_32BIT));
		memset(header, 0x00, sizeof(ELF_SECTION_HDR_32BIT));

	// return NULL if we cannot read out of the file
	if( fread((void *) header, 1, sizeof(ELF_SECTION_HDR_32BIT), elfFile) == 0)
		return NULL;

	return header;
}

// returns a linked list representing all of the program headers found in the
// file.
P_ELF_PROGRAM_HEADER_LIST_32BIT ELF32_ExtractProgramHeaders_File(P_ELF_32BIT_LOOKUP_TABLE fullHeader, FILE *elfFile){

	if(fullHeader == NULL)
		return NULL;

	if(elfFile == NULL)
		return NULL;

	size_t phOffset  = fullHeader->header->elfProgramHeaderOffset;
	size_t phSize    = fullHeader->header->elfProgramHeaderSize;
	size_t phEntries = fullHeader->header->elfNumberPHeaderEntries;

	size_t phCurrentOffset = phOffset;

	// Rewind to assure position
	rewind(elfFile);

	P_ELF_PROGRAM_HEADER_LIST_32BIT pHeaderList;

	// create first structure
	if(phEntries > 0){

		// Create first element
		pHeaderList = (P_ELF_PROGRAM_HEADER_LIST_32BIT) bpmalloc(sizeof(ELF_PROGRAM_HEADER_LIST_32BIT));
			memset(pHeaderList, 0x00, sizeof(ELF_PROGRAM_HEADER_LIST_32BIT));

		// Set first pointer, other pointers are null by memset
		pHeaderList->first = pHeaderList;

		// Create space for actual header
		pHeaderList->header = (P_ELF_PROGRAM_HEADER_32BIT) bpmalloc(sizeof(ELF_PROGRAM_HEADER_32BIT));
			memset(pHeaderList->header, 0x00, sizeof(ELF_PROGRAM_HEADER_32BIT));

	} else {
		// return Null if there are no program header entries
		return NULL;
	}

	// Loop through every entry, collecting and saving the structures
	for(;phEntries > 0; phEntries--){


		// Seek to the current offset
		fseek(elfFile, phCurrentOffset, SEEK_SET);

		// Copy in header
		fread(pHeaderList->header, 1, phSize, elfFile);

		// Move list position
		if(phEntries - 1 == 0){
			pHeaderList->next = NULL;
			break;

		} else {
			// Create Next List
			pHeaderList->next = (P_ELF_PROGRAM_HEADER_LIST_32BIT) bpmalloc(sizeof(ELF_PROGRAM_HEADER_LIST_32BIT));
				memset(pHeaderList->next, 0x00, sizeof(ELF_PROGRAM_HEADER_LIST_32BIT));

			// Create Next Header Structure
			pHeaderList->next->header = (P_ELF_PROGRAM_HEADER_32BIT) bpmalloc(sizeof(ELF_PROGRAM_HEADER_32BIT));
				memset(pHeaderList->next->header, 0x00, sizeof(ELF_PROGRAM_HEADER_32BIT));

			// Reset Pointers
			pHeaderList->next->prev = pHeaderList;
			pHeaderList->next->first = pHeaderList->first;
			pHeaderList->next->next = NULL;

			// Move List Forward
			pHeaderList = pHeaderList->next;

			// Rewind file
			rewind(elfFile);
		}

		// Increment offset
		phCurrentOffset += phSize;
	}

	// Reset file position
	rewind(elfFile);

	// Return first element
	return pHeaderList->first;
}

// Extract actual sections from the full headers section list (no types)
int ELF32_ExtractSections_File(P_ELF_32BIT_LOOKUP_TABLE fullHeader, FILE *elfFile){

	if(fullHeader == NULL)
		return ERR_FAILURE;

	if(fullHeader->sections == NULL)
		return ERR_FAILURE;

	P_ELF_SECTION_LIST_32BIT firstPtr = fullHeader->sections->first;
	for(fullHeader->sections = fullHeader->sections->first; \
		fullHeader->sections != NULL; \
		fullHeader->sections = fullHeader->sections->next){

			rewind(elfFile);
			fseek(elfFile, fullHeader->sections->section->offset, SEEK_SET);

			// Create space for section and read section into memory
			fullHeader->sections->data = (unsigned char *) bpmalloc(fullHeader->sections->section->size);
			fread(fullHeader->sections->data, 1, fullHeader->sections->section->size, elfFile);

			// Save Size
			fullHeader->sections->dataSize = fullHeader->sections->section->size;

	}

	// Reset pointer
	fullHeader->sections = 	firstPtr;

	return ERR_SUCCESS;
}






// ------------------------------------------------------------------
// Neutral Functions
// ------------------------------------------------------------------

// Creates a New Empty Sectionlist structure
P_ELF_SECTION_LIST_32BIT ELF32_NewSectionList(){

	P_ELF_SECTION_LIST_32BIT sectionList = (P_ELF_SECTION_LIST_32BIT) bpmalloc(sizeof(ELF_SECTION_LIST_32BIT));
	if(sectionList != NULL)
		memset(sectionList, 0x00, sizeof(ELF_SECTION_LIST_32BIT));
	else
		return NULL;

	return sectionList;
}

// Search A Section List by Name
P_ELF_SECTION_LIST_32BIT ELF32_GetSectionListElementByName(P_ELF_32BIT_LOOKUP_TABLE fullHeader, char *name){

	if(fullHeader == NULL)
		return NULL;


	if(fullHeader->sections == NULL)
		return NULL;

	if(name == NULL)
		return NULL;

	// Section Name Should Never Be Greater then 50 bytes
	if(bpstrlen(name) > 50)
		return NULL;



	P_ELF_SECTION_LIST_32BIT firstPtr = fullHeader->sections->first;
	P_ELF_SECTION_LIST_32BIT retPtr = NULL;

	for(fullHeader->sections = fullHeader->sections->first;\
		fullHeader->sections != NULL;\
		fullHeader->sections = fullHeader->sections->next){

		// first check length
		// next do string comparison
		if( bpstrlen(fullHeader->sections->name) == bpstrlen(name) ){

			if(memcmp(fullHeader->sections->name, name, bpstrlen(fullHeader->sections->name))	== 0){

				retPtr = fullHeader->sections;
				fullHeader->sections = firstPtr;

				// Return the correct section list
				return retPtr;
			}
		}

	}

	fullHeader->sections = firstPtr;


	return NULL;
}


// Destroys a Section List
int ELF32_DestroySectionList(P_ELF_SECTION_LIST_32BIT sList){

	return 0;
}



// ---------------------------------------------------------------------

// Prints out all values in a ELF32 lookup table
int ELF32_PrintAll(P_ELF_32BIT_LOOKUP_TABLE elfLookupTable){

	if(elfLookupTable == NULL)
		return ERR_FAILURE;

	if(elfLookupTable->header == NULL)
		return ERR_FAILURE;

	ELF32_PrintHeader(elfLookupTable->header);

	if(elfLookupTable->sections != NULL)
		ELF32_PrintSectionHeaders(elfLookupTable->sections->first);
	else
		printf("\n [+] Found No Section Headers");


	if(elfLookupTable->sections != NULL)
		ELF32_PrintSections(elfLookupTable->sections->first);
	else
		printf("\n [+] Found No Sections");

	if(elfLookupTable->symTab != NULL)
		ELF32_PrintDynSymbolTable(elfLookupTable->symTab);
	else
		printf("\n [+] No Dynamic Symbol Table Found");

	if(elfLookupTable->relocs != NULL)
		ELF32_PrintRelocations(elfLookupTable->relocs);
	else
		printf("\n [+] No Relocations Found");

	if(elfLookupTable->pHeaders != NULL)
		ELF32_PrintProgramHeaders(elfLookupTable->pHeaders);
	else
		printf("\n [+] No Program Headers Found");

	if(elfLookupTable->dynamics != NULL)
		ELF32_PrintDynamics(elfLookupTable->dynamics);
	else
		printf("\n [+] No dynamics found.");

	return ERR_SUCCESS;
}

// Prints the ELF32 Header, use ELF32_PrintAll for all headers
int ELF32_PrintHeader(P_ELF_HDR_32BIT header){

	if(header == NULL)
		return ERR_FAILURE;

	printf("\n[+] ELF Header Information: ");
	printf("\n  [+] Ident Fields:");
	printf("\n\t Magic: %.02x %c %c %c", header->elfMagicString[0], header->elfMagicString[1], header->elfMagicString[2], header->elfMagicString[3]);
	printf("\n\t Class: %u", header->elfClass);
	printf("\n\t Data Encoding:   %.02x", header->elfData);
	printf("\n\t ELF Version:     %.02x", header->elfVersion);
	printf("\n\t ELF OS ABI:      %.02x", header->elfOSAbi);
	printf("\n\t ELF ABI Version: %.02x", header->elfABIVersion);
	printf("\n\t ELF Pad:         %.02x", header->elfPad);

	printf("\n  [+] ELF Header Fields:");
	printf("\n\t ELF Type: %u", header->elfType);
	printf("\n\t ELF Machine Type: %u", header->elfMachine);
	printf("\n\t ELF Version: %u", header->elfVer);
	printf("\n\t ELF Entry Pointer: %p", (void *) header->elfEntry);
	printf("\n\t ELF Program Header Offset: %u", header->elfProgramHeaderOffset);
	printf("\n\t ELF Section Header Offset: %u", header->elfSectionHeaderOffset);
	printf("\n\t ELF Flags: %.08x", header->elfFlags);
	printf("\n\t ELF Header Size: %u", header->elfHeaderSize);
	printf("\n\t ELF Program Header Size: %u", header->elfProgramHeaderSize);
	printf("\n\t Number of entries in program header: %u", header->elfNumberPHeaderEntries);
	printf("\n\t ELF Section Header Size: %u", header->elfSectionHeaderSize);
	printf("\n\t Number of Section Header Entries: %u", header->elfNumberSHeaderEntries);
	printf("\n\t Index of Section Header String Table: %u", header->elfSHeaderStringTableIndex);

	return ERR_SUCCESS;
}

// Prints out the actual Section List pointer elements
int ELF32_PrintSectionListElement(P_ELF_SECTION_LIST_32BIT sList){

	if(sList == NULL)
		return ERR_FAILURE;

	printf("\tSection List: %p", sList);

	// Section Name
	if(sList->name != NULL)
		printf("\n Section Name: %s", sList->name);
	else
		printf("\n Section Name: NULL");

	// Section Type
	if(sList->type != NULL)
		printf("\n Section Type: %s", sList->type);
	else
		printf("\n Section Type: NULL");

	// List Pointers
		printf("\n\t\t first: %p", sList->first);
		printf("\n\t\t prev : %p", sList->prev );
		printf("\n\t\t next : %p", sList->next);

	return ERR_SUCCESS;

}

// Print out all program headers
int ELF32_PrintProgramHeaders(P_ELF_PROGRAM_HEADER_LIST_32BIT pList){

	if(pList == NULL)
		return ERR_FAILURE;

	P_ELF_PROGRAM_HEADER_LIST_32BIT firstPtr = pList->first;

	printf("\n\t\t [+] Program Header List Head: %p", pList->first);

	for(pList = pList->first; pList != NULL; pList = pList->next){
			printf("\n\t\t\t Program Header Struct: %p", pList->header);
			printf("\n\t\t\t   Segment Type: ............%.08x", pList->header->segType);
			printf("\n\t\t\t   Segment Offset: ..........%.08x", pList->header->segOffset);
			printf("\n\t\t\t   Segment Virtual Address: .%.08x", pList->header->segVirtAddr);
			printf("\n\t\t\t   Segment Physical Address: %.08x", pList->header->segPhysAddr);
			printf("\n\t\t\t   Segment File Size: .......%.08x", pList->header->segFileSize);
			printf("\n\t\t\t   Segment Memory Size: .....%.08x", pList->header->segMemSize);
			printf("\n\t\t\t   Segment Flags: ...........%.08x", pList->header->segFlags);
			printf("\n\t\t\t   Segment Alignment: .......%.08x", pList->header->segAlignment);
			printf("\n");
	}

	// Reset list position
	pList = firstPtr;

	return ERR_SUCCESS;
}

// Sequentially prints out all dynamic sections
int ELF32_PrintDynamics(P_ELF_DYNAMIC_LIST_32BIT dList){

	if(dList == NULL)
		return NULL;

	if(dList->info == NULL)
		return NULL;

	P_ELF_DYNAMIC_LIST_32BIT firstPtr = dList->first;
	for(dList = dList->first; dList != NULL; dList = dList->next){
		printf("\n\t\t Dynamic List Structure: %p", dList->info);
		printf("\n\t\t\t Tag: %.08x", dList->info->dyn_tag);
		printf("\n\t\t\t Val: %.08x", dList->info->d_un.dyn_ptr);
		printf("\n");
	}

	// Reset pointer
	dList = firstPtr;

	return ERR_SUCCESS;
}

// Plural: Prints out all extracted section headers
int ELF32_PrintSectionHeaders(P_ELF_SECTION_LIST_32BIT sHeader){

	if(sHeader == NULL)
		return ERR_FAILURE;

	// Begin printing out list elements/components
	printf("\n  [+] ELF32 Section Header List:");

	for(sHeader = sHeader->first; sHeader != NULL; sHeader = sHeader->next){
			// print out the actual section header
			printf("\n\n\t\t   Section Header: %p", sHeader);
			printf("\n\t\t   Section Header String Table Lookup: %s", sHeader->name);
			ELF32_PrintSectionHeader(sHeader->section);
	}

	return ERR_SUCCESS;
}

// Singular: Prints One Section Header Element
int ELF32_PrintSectionHeader(P_ELF_SECTION_HDR_32BIT sHeader){

	if(sHeader == NULL)
		return 0;

	printf("\n\t\t\t Section Header->name :     0x%.8x", sHeader->name);
	printf("\n\t\t\t Section Header->type :     0x%.8x", sHeader->type);
	printf("\n\t\t\t Section Header->flags:     0x%.8x", sHeader->flags);
	printf("\n\t\t\t Section Header->addr:      0x%.8x",  sHeader->addr);
	printf("\n\t\t\t Section Header->offset:    0x%.8x",sHeader->offset);
	printf("\n\t\t\t Section Header->size:      %u",    sHeader->size);
	printf("\n\t\t\t Section Header->link:      0x%.8x",  sHeader->link);
	printf("\n\t\t\t Section Header->info:      0x%.8x",  sHeader->info);
	printf("\n\t\t\t Section Header->addrAlign: 0x%.8x", sHeader->addrAlign);
	printf("\n\t\t\t Section Header->entSize:   %u", sHeader->entSize);

   return 0;
}


// Prints out information regarding the elf lookup table which is primarily
// for diagnostics with header pointers.
int ELF32_PrintLookupPtrForDiagnostics(P_ELF_32BIT_LOOKUP_TABLE elfLookupTable){

	if(!elfLookupTable)
		return ERR_FAILURE;

	  printf("\n Elf Lookup Table Pointer Diagnostic Table: ");
	  printf("\n\t filePath : %p : %s", elfLookupTable->filePath, elfLookupTable->filePath);
	  printf("\n\t pid      : %i", elfLookupTable->pid);
	  printf("\n\t origin   : %i", elfLookupTable->origin);
	  printf("\n\t header   : %p", elfLookupTable->header);
	  printf("\n\t program headers: %p", elfLookupTable->pHeaders);
	  printf("\n\t sections : %p", elfLookupTable->sections);
	  printf("\n\t symTable : %p", elfLookupTable->symTab);
	  printf("\n\t relocs   : %p", elfLookupTable->relocs);
	  printf("\n\t dynamics : %p", elfLookupTable->dynamics);
	  printf("\n\t GOT      : %p", elfLookupTable->got);
	  printf("\n\t memMap   : %p", elfLookupTable->memMap);
	  printf("\n\t stringTab: %p", elfLookupTable->stringTable);
	  printf("\n\t strTab Size: %u", elfLookupTable->stringTableSize);
	  printf("\n\t decodedInstructions: %p", elfLookupTable->decodedInstructions);

	  return ERR_SUCCESS;

}

#endif // #if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1

#endif // USE_BLOCKPARTY_DEBUGGING == 1

#include "../../../include/BP-Main.h"

#if USE_BLOCKPARTY_DEBUGGING == 1

#if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1

// String Table Containing ELF32 Section Types
char ELFSections[][100] = { ".bss",
							".comment",
							".data",
							".data1",
							".dynamic",
							".dynstr",
							".dynsym",
							".fini",
							".ctors",
							".dtors",
							".got",
							".got.plt",
							".hash",
							".init",
							".interp",
							".note",
							".plt",
							".rel",
							".rodata",
							".rodata1",
							".shstrtab",
							".strtab",
							".symtab",
							".text",
							".eh_frame",
							".gnu_version",
							".gnu_version_d",
							".gnu_version_r",
							".note",
							".stab",
							".stabstr",
							".SUNW_heap", // Sun Specifics
							".SUNW_reloc",
							".SUNW_version",
							NULL};




// ------------------------ SECTION DATA TRANSLATION METHODS ----------------------

P_ELF_SECTION_SYMBOL_TABLE_LIST_32BIT ELF32_TranslateSymbolTableFromData(P_ELF_32BIT_LOOKUP_TABLE fullHeader){

	if(fullHeader == NULL)
		return NULL;

	P_ELF_SECTION_LIST_32BIT dynSymbols  = ELF32_GetSectionListElementByName(fullHeader, ".dynsym");
	P_ELF_SECTION_LIST_32BIT stringTable = ELF32_GetSectionListElementByName(fullHeader, ".dynstr");



	if(dynSymbols == NULL)
		return NULL;

	if(stringTable == NULL)
		return NULL;

	// Create First List Element
	P_ELF_SECTION_SYMBOL_TABLE_LIST_32BIT symTabList = (P_ELF_SECTION_SYMBOL_TABLE_LIST_32BIT) bpmalloc(sizeof(ELF_SECTION_SYMBOL_TABLE_LIST_32BIT));
	if(symTabList != NULL){
		memset(symTabList, 0x00, sizeof(ELF_SECTION_SYMBOL_TABLE_LIST_32BIT));
		symTabList->first = symTabList;
	} else
		return NULL;

	// Holds the position in our data block
	size_t buffPos = 0;

	// Calculate the total number of dynamic symbol entries that we have
	size_t symTabCount = dynSymbols->section->size / sizeof(ELF_SECTION_SYMBOL_TABLE_32BIT);


	symTabList->index = 0;
	int i = 0;



	for(; symTabCount > 0; symTabCount--){

		// Save index variable
		symTabList->index = i;
		i++;

		// Allocate space for entry
		symTabList->symTab_entry = (P_ELF_SECTION_SYMBOL_TABLE_32BIT) bpmalloc(sizeof(ELF_SECTION_SYMBOL_TABLE_32BIT));

		// copy in the symbol table data
		memcpy(symTabList->symTab_entry, &dynSymbols->data[buffPos], sizeof(ELF_SECTION_SYMBOL_TABLE_32BIT));
		buffPos += sizeof(ELF_SECTION_SYMBOL_TABLE_32BIT);

		// extract string from string table
		symTabList->name = bpstrndup((char *) &stringTable->data[symTabList->symTab_entry->symTab_name], ELF32_MAX_NAME_LENGTH);

		// Terminate List if at last symbol
		if(symTabCount - 1 == 0){

			symTabList->next = NULL;
			break;
		} else {
			symTabList->next = (P_ELF_SECTION_SYMBOL_TABLE_LIST_32BIT) bpmalloc(sizeof(ELF_SECTION_SYMBOL_TABLE_LIST_32BIT));
			if(symTabList->next != NULL){
				memset(symTabList->next, 0x00, sizeof(ELF_SECTION_SYMBOL_TABLE_LIST_32BIT));
					symTabList->next->first = symTabList->first;
					symTabList->next->prev = symTabList;
					symTabList->next->next = NULL;
			} else
				return NULL;

			symTabList = symTabList->next;
		}

	}

	return symTabList->first;
}

// Translates All Relocations
P_ELF_RELOCATIONS_LIST_32BIT ELF32_TranslateRelocationsFromData(P_ELF_32BIT_LOOKUP_TABLE fullHeader){

	if(fullHeader == NULL)
		return NULL;

	if(fullHeader->sections == NULL)
		return NULL;

	if(fullHeader->symTab == NULL)
		return NULL;

	// Create First Structure
	size_t numRelocs = 0;
	size_t relOffset = 0;

	// Used for list placement testing
	size_t testVal;

	// Used for holding symbol offsets
	size_t symIndex;
	size_t symCount;

	// Relocations pointer
	P_ELF_RELOCATIONS_LIST_32BIT relocations = NULL;

	// Save original first pointers
	P_ELF_SECTION_LIST_32BIT firstPtr = fullHeader->sections->first;
	P_ELF_SECTION_SYMBOL_TABLE_LIST_32BIT firstSymbolPtr = fullHeader->symTab->first;


	// Find and commit all relocations
	for(fullHeader->sections = fullHeader->sections->first; \
		fullHeader->sections != NULL; \
		fullHeader->sections = fullHeader->sections->next){

				// Reloop label
				reloop:
				relOffset = 0;

				// Branch if relocation is found
				if(memcmp(fullHeader->sections->name, ".rel", 4) == 0){

						// First list creation
						if(relocations == NULL){

							relocations = (P_ELF_RELOCATIONS_LIST_32BIT) bpmalloc(sizeof(ELF_RELOCATIONS_LIST_32BIT));

							// Set list pointers
								relocations->first = relocations;
								relocations->next  = NULL;
								relocations->prev  = NULL;

						} else {

							// Allocate next
							relocations->next = (P_ELF_RELOCATIONS_LIST_32BIT) bpmalloc(sizeof(ELF_RELOCATIONS_LIST_32BIT));

							// Set list pointers
								relocations->next->first = relocations->first;
								relocations->next->prev  = relocations;
								relocations->next->next  = NULL;

							// Advance List
								relocations = relocations->next;

						}

						// Save Full Name In List
						relocations->name = bpstrndup(fullHeader->sections->name, ELF32_MAX_NAME_LENGTH);
						relocations->entries = NULL;

						//Count Number of Relocations Total
						#ifdef USE_ELF_RELA
							numRelocs = fullHeader->sections->section->size / sizeof(ELF_RELA);
						#endif

						#ifndef USE_ELF_RELA
							numRelocs = fullHeader->sections->section->size / sizeof(ELF_REL);
						#endif

						// Extract all Relocations
						for(;numRelocs > 0; numRelocs--){

							// Perform first assignments if first
							if(relocations->entries == NULL){

								relocations->entries = (P_ELF_RELOCATION_LIST_ENTRY_32BIT) bpmalloc(sizeof(ELF_RELOCATION_LIST_ENTRY_32BIT));
								memset(relocations->entries, 0x00, sizeof(ELF_RELOCATION_LIST_ENTRY_32BIT));

								#ifdef USE_ELF_RELA
									relocations->entries->rel = (P_ELF_RELA) bpmalloc(sizeof(ELF_RELA));
									memset(relocations->entries->rel, 0x00, sizeof(ELF_RELA));
								#endif

								#ifndef USE_ELF_RELA
									relocations->entries->rel = (P_ELF_REL) bpmalloc(sizeof(ELF_REL));
									memset(relocations->entries->rel, 0x00, sizeof(ELF_REL));
								#endif

									relocations->entries->first = relocations->entries;
									relocations->entries->prev = NULL;
									relocations->entries->next = NULL;

							} else {

								#ifdef USE_ELF_RELA
									relocations->entries->rel = (P_ELF_RELA) bpcalloc(sizeof(ELF_RELA), 1);
								#endif

								#ifndef USE_ELF_RELA
									relocations->entries->rel = (P_ELF_REL) bpcalloc(sizeof(ELF_REL), 1);
								#endif

							}

							// Copy in one rel entry, and advance offset to next structure
							#ifdef USE_ELF_RELA
								memcpy(relocations->entries->rel, &fullHeader->sections->data[relOffset], sizeof(ELF_RELA));
								relOffset += sizeof(ELF_RELA);
							#endif

							#ifndef USE_ELF_RELA
								memcpy(relocations->entries->rel, &fullHeader->sections->data[relOffset], sizeof(ELF_REL));
								relOffset += sizeof(ELF_REL);
							#endif

							// Extract and store name
							symIndex = relocations->entries->rel->rel_info;
							symIndex = BP_ELF32_R_SYM(symIndex);
							symCount = 0;

							// Loop through the fullheader symbol table (symTab)
							for(fullHeader->symTab = fullHeader->symTab->first;\
								fullHeader->symTab != NULL; \
								fullHeader->symTab = fullHeader->symTab->next){

								symCount++;

								if(fullHeader->symTab->index == symIndex){
									// found correct symbol in table, strdup string
									relocations->entries->name = bpstrndup(fullHeader->symTab->name, ELF32_MAX_NAME_LENGTH);
								}
							}
							fullHeader->symTab = firstSymbolPtr;

							// Copy in the test value to determine if it is null (hackish)
							memcpy(&testVal, &fullHeader->sections->data[relOffset], 4);

							// Exit or Continue Conditions
							if((numRelocs - 1 == 0) || testVal == 0x00){

								relocations->entries->next = NULL;

								// Reloop if more sections exist
								if(fullHeader->sections->next != NULL){
									fullHeader->sections = fullHeader->sections->next;
									goto reloop;
								} else {
									goto end;
								}

							} else {
								// create next data structure and link things up
								relocations->entries->next = (P_ELF_RELOCATION_LIST_ENTRY_32BIT) bpmalloc(sizeof(ELF_RELOCATION_LIST_ENTRY_32BIT));

								memset(relocations->entries->next, 0x00, sizeof(ELF_RELOCATION_LIST_ENTRY_32BIT));

								relocations->entries->next->prev  = relocations->entries;
								relocations->entries->next->first = relocations->entries->first;
								relocations->entries->next->next  = NULL;

								// shift list forward
								relocations->entries = relocations->entries->next;

							}


						}

				}
	}

	// End Label
	end:
		fullHeader->sections = firstPtr;
		if(relocations == NULL){
			return NULL;
		}

		return relocations->first;
		// return NULL;
}

// Translates GOT list form data
P_ELF_GOT_LIST_32BIT ELF32_TranslateGOTFromData(P_ELF_32BIT_LOOKUP_TABLE fullHeader){

	if(fullHeader == NULL)
		return NULL;

	if(fullHeader->sections == NULL)
		return NULL;

	P_ELF_SECTION_LIST_32BIT gotEntry = ELF32_GetSectionListElementByName(fullHeader, ".got");
	if(gotEntry == NULL)
		return NULL;

	// Save Size
	size_t gotSize = gotEntry->section->size;

	// Create First List Element
	P_ELF_GOT_LIST_32BIT gotList = (P_ELF_GOT_LIST_32BIT) bpmalloc(sizeof(ELF_GOT_LIST_32BIT));
		memset(gotList, 0x00, sizeof(ELF_GOT_LIST_32BIT));

	gotList->first = gotList;

	size_t secOffset = 0;


	// Extract All GOT Entries
	for(; gotSize > 0; gotSize -= sizeof(int)){
		memcpy(&gotList->addr, &gotEntry->data[secOffset], 4);


		secOffset += sizeof(ELF_GOT_LIST_32BIT);
		if((gotSize - sizeof(ELF_GOT_LIST_32BIT)) <= 0){
			gotList->next = NULL;
			break;

		} else {
			gotList->next = (P_ELF_GOT_LIST_32BIT) bpmalloc(sizeof(ELF_GOT_LIST_32BIT));
				memset(gotList->next, 0x00, sizeof(ELF_GOT_LIST_32BIT));

			gotList->next->prev = gotList;
			gotList->next->next = NULL;
			gotList->next->first = gotList->first;

			// Advance List Forward
			gotList = gotList->next;

		}

	}

	return gotList->first;
}

// Translates dynamic section
P_ELF_DYNAMIC_LIST_32BIT ELF32_TranslateDynamicsFromData(P_ELF_32BIT_LOOKUP_TABLE fullHeader){

	if(fullHeader == NULL)
		return NULL;


	P_ELF_SECTION_LIST_32BIT dynSection = ELF32_GetSectionListElementByName(fullHeader, ".dynamic");

	if(dynSection == NULL)
		return NULL;

	// Size of the dynamic section
	size_t dynSize = dynSection->section->size;
	size_t loopOffset = 0;

	// Create first list element
	P_ELF_DYNAMIC_LIST_32BIT dynList = (P_ELF_DYNAMIC_LIST_32BIT) bpcalloc(sizeof(ELF_DYNAMIC_LIST_32BIT), 1);

	// Create First Informational Section
	dynList->info = (P_ELF_DYNAMIC_32BIT) bpcalloc(sizeof(ELF_DYNAMIC_32BIT), 1);

		// Assign First List Pointers
		dynList->first = dynList;
		dynList->next = NULL;
		dynList->prev = NULL;

	// Save First Pointer
	P_ELF_DYNAMIC_LIST_32BIT firstPtr = dynList->first;



	// Copy in informaton
	for(;dynSize > 0; dynSize -= sizeof(ELF_DYNAMIC_32BIT)){

		memcpy(dynList->info, &dynSection->data[loopOffset], sizeof(ELF_DYNAMIC_32BIT));
		loopOffset += sizeof(ELF_DYNAMIC_32BIT);

		if((dynSize - sizeof(ELF_DYNAMIC_32BIT)) <= 0){
			dynList->next = NULL;
			break;
		} else {
			// Create Next List Object
			dynList->next = (P_ELF_DYNAMIC_LIST_32BIT) bpmalloc(sizeof(ELF_DYNAMIC_LIST_32BIT));
				memset(dynList->next, 0x00, sizeof(ELF_DYNAMIC_LIST_32BIT));

			// Create List Informational Structure
			dynList->next->info = (P_ELF_DYNAMIC_32BIT) bpmalloc(sizeof(ELF_DYNAMIC_32BIT));
				memset(dynList->next->info, 0x00, sizeof(ELF_DYNAMIC_32BIT));

			dynList->next->first = dynList->first;
			dynList->next->prev = dynList;
			dynList->next->next = NULL;

			// Move List Forward
			dynList = dynList->next;

		}
	}

	// reset list position for return
	dynList = firstPtr;

	return dynList;
}


// --------------------------------------------------------------------
int ELF32_PrintSections(P_ELF_SECTION_LIST_32BIT sList){

	if(sList == NULL)
		return ERR_FAILURE;

	P_ELF_SECTION_LIST_32BIT firstPtr = sList->first;

	for(sList = sList->first; sList != NULL; sList = sList->next){
		printf("\n\t\t[+] Section Name: %s", sList->name);
		printf("\n\t\t\t Section List Data Pointer: %p", sList->data);
		printf("\n\t\t\t Section List Data Size: %u", sList->dataSize);
		printf("\n");
	}
	sList = firstPtr;

	return ERR_SUCCESS;
}

// Prints out the .dynsym table from a symTab list
int ELF32_PrintDynSymbolTable(P_ELF_SECTION_SYMBOL_TABLE_LIST_32BIT symTab){

	if(symTab == NULL)
		return ERR_FAILURE;

	printf("\n\t\t [Symbol Table]");
	P_ELF_SECTION_SYMBOL_TABLE_LIST_32BIT firstPtr = symTab->first;

	for(symTab = symTab->first; symTab != NULL; symTab = symTab->next){
		printf("\n\t\t\t  Symbol Name: %s", symTab->name);
		printf("\n\t\t\t  Symbol Table Structure Pointer: %p", symTab->symTab_entry);
		printf("\n\t\t\t\t  Name Offset: %.8x", symTab->symTab_entry->symTab_name);
		printf("\n\t\t\t\t  Size: %.8x", symTab->symTab_entry->symTab_size);
		printf("\n\t\t\t\t  Info: %.2x", symTab->symTab_entry->symTab_info);
		printf("\n\t\t\t\t  Other: %.2x", symTab->symTab_entry->symTab_other);
		printf("\n");
	}

	symTab = firstPtr;
	return ERR_SUCCESS;
}

// Print out all relocations in a list
int ELF32_PrintRelocations(P_ELF_RELOCATIONS_LIST_32BIT relocations){

	if(relocations == NULL)
		return ERR_FAILURE;

	if(relocations->entries == NULL)
		return ERR_FAILURE;

	P_ELF_RELOCATIONS_LIST_32BIT firstPtr = relocations->first;
	P_ELF_RELOCATION_LIST_ENTRY_32BIT firstRelocPtr = NULL;

	for(relocations = relocations->first; relocations != NULL; relocations = relocations->next){
		printf("\n\t Relocation: %s", relocations->name);

		if(relocations->entries != NULL)
			printf("\n\t\t Relocation Entry List First: %p", relocations->entries->first);

			if(relocations->entries->rel != NULL){
				firstRelocPtr = relocations->entries->first;
				for(relocations->entries = relocations->entries->first; \
					relocations->entries != NULL; \
					relocations->entries = relocations->entries->next){
						printf("\n\t\t\t Rel Symbol Name: %s", relocations->entries->name);
						printf("\n\t\t\t Rel Structure: %p", relocations->entries->rel);
						printf("\n\t\t\t\t Rel. Offset: %.08x", relocations->entries->rel->rel_offset);
						printf("\n\t\t\t\t Rel. Info  : %.08x", relocations->entries->rel->rel_info);
						#ifdef USE_ELF_RELA
							printf("\n\t\t\t\t Rel. Addend: %.08x", relocations->entries->rel->rel_addend);
						#endif

						printf("\n");
				}
				relocations->entries = firstRelocPtr;

		} else {
			printf("\n\t\t Relocation Entry List First: NULL");
			continue;
		}

	}

	relocations = firstPtr;
	return ERR_SUCCESS;
}

#endif

#endif // USE_BLOCKPARTY_DEBUGGING == 1

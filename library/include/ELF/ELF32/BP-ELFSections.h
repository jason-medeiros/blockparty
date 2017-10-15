#ifndef BPELFSECTIONS_H_
#define BPELFSECTIONS_H_

#if USE_BLOCKPARTY_DEBUGGING == 1

#if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1



// ---------------------- TRANSLATION FUNCTIONALITY ----------------

P_ELF_SECTION_SYMBOL_TABLE_LIST_32BIT ELF32_TranslateSymbolTableFromData(P_ELF_32BIT_LOOKUP_TABLE fullHeader);

P_ELF_RELOCATIONS_LIST_32BIT ELF32_TranslateRelocationsFromData(P_ELF_32BIT_LOOKUP_TABLE fullHeader);

P_ELF_DYNAMIC_LIST_32BIT ELF32_TranslateDynamicsFromData(P_ELF_32BIT_LOOKUP_TABLE fullHeader);

P_ELF_GOT_LIST_32BIT ELF32_TranslateGOTFromData(P_ELF_32BIT_LOOKUP_TABLE fullHeader);

// Print out Sections
int ELF32_PrintSections(P_ELF_SECTION_LIST_32BIT sList);

// Print out Symbol Table
int ELF32_PrintDynSymbolTable(P_ELF_SECTION_SYMBOL_TABLE_LIST_32BIT symTab);

// Print out Relocations
int ELF32_PrintRelocations(P_ELF_RELOCATIONS_LIST_32BIT relocations);

#endif // #if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1


#endif // USE_BLOCKPARTY_DEBUGGING == 1

#endif /*BPELFSECTIONS_H_*/

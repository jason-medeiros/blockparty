#ifdef __GNUC__

#if USE_BLOCKPARTY_DEBUGGING == 1

#if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1


#define ELF32_MAX_NAME_LENGTH 1024


// Linux definitions can be found in /usr/include/linux/elf.h
typedef struct _ELF_HEADER_32BIT {

	//------------------------------------------------------
	// initial 16 bytes for magic string representation (16bytes total)

	unsigned char elfMagicString[4]; // stores the 0x7fELF byte string

	unsigned char elfClass; // one byte, 3 values possible
							// 0 = invalid class, 1 = 32bit, 2 = 64bit

	unsigned char elfData;  // specifies the data encoding of the binary.
							// 0 = invalid encoding, 1 least signifigant byte encoding
							// 2 = most signifigant byte encoding

	unsigned char elfVersion; // Must be set to EV_CURRENT (1 for 32bit linux)

	unsigned char elfOSAbi;   // 3 for linux, see elf definition in elf.h
							  // for additional choices.

	unsigned char elfABIVersion; // 0 for unspecified (should work)

	unsigned char elfPad; // marks the beginning of the unused bytes in e_ident

	unsigned char emptyPad[6]; // empty padding (unused)
	//------------------------------------------------------


	unsigned short elfType;
	unsigned short elfMachine;
	size_t   elfVer;
	size_t   elfEntry; 					// program entry point
  	size_t   elfProgramHeaderOffset;	// program header table offset
	size_t   elfSectionHeaderOffset;	// section header table offset
	size_t   elfFlags;					// processor specific flags

	unsigned short elfHeaderSize;			// size of the elf header in bytes

	// program header info
	unsigned short elfProgramHeaderSize;	// size of the elf program header in bytes
	unsigned short elfNumberPHeaderEntries; // number of entries in the program header

	// section header info
	unsigned short elfSectionHeaderSize;    // size of the elf section header in bytes
	unsigned short elfNumberSHeaderEntries; // number of entries in the section header
	unsigned short elfSHeaderStringTableIndex; // Index of the SHeader string table

} ELF_HDR_32BIT, *P_ELF_HDR_32BIT;

// 32BIT ELF Section Header
typedef struct _ELF_SECTION_HDR_32BIT {
	size_t name; // index into the string table section, giving the
					   // location of a null-terminated string.

	size_t type; // Section type

	size_t flags; // Section Flags

	size_t addr;  // If the section is to appear in the memory image
						// of a process, this is the address where the sections
						// first byte should occupy. (0 if not to be mmapped)

	size_t offset; // Offset from the beginning of the file to the first
						 // byte in the section.

	size_t size;  // Size of the section header in bytes

	size_t link;  // Section header table index link, interpretation depends
						// on the section type.

	size_t info;	// extra information, depends on section type

	size_t addrAlign; // address alignment constraints.  Only 0 and positive
							// integral power of two are allowed for aligment.  If
							// either 1 or 0 is used, it means the section has
							// no alignment constraints.

	size_t entSize;  //  Some sections hold a table of fixed-size entries,
						   // such as a symbol table.  This member gives the size in
						   // bytes of each entry.  Contains 0 if the section does
						   // not hold a table of fixed size entries.

} ELF_SECTION_HDR_32BIT, *P_ELF_SECTION_HDR_32BIT;

// Section list, holds one list element per
// list position.
typedef struct _ELF_SECTION_LIST_32BIT {

	char * name;	// string containing the section name
	char * type;	// string representation of type (SHT_PROGBITS, etc)

	// This stores a reference pointer to the memory
	// map in which the section corresponds at runtime.
	P_MEMORY_MAP memMap;

	unsigned char * data;    // this is the actual contents of the section
	size_t dataSize;

	P_ELF_SECTION_HDR_32BIT section;  // actual section header

		struct _ELF_SECTION_LIST_32BIT * first;
		struct _ELF_SECTION_LIST_32BIT * prev;
		struct _ELF_SECTION_LIST_32BIT * next;

} ELF_SECTION_LIST_32BIT, *P_ELF_SECTION_LIST_32BIT;

// Program header element, can be multiple
typedef struct _ELF_PROGRAM_HEADER_32BIT {
	size_t segType;
	size_t segOffset;
	size_t segVirtAddr;
	size_t segPhysAddr;
	size_t segFileSize;
	size_t segMemSize;
	size_t segFlags;
	size_t segAlignment;
} ELF_PROGRAM_HEADER_32BIT, *P_ELF_PROGRAM_HEADER_32BIT;

// Program header list, holds all program headers
// in order of occurance.
typedef struct _ELF_PROGRAM_HEADER_LIST_32BIT {
	// actual program header
	P_ELF_PROGRAM_HEADER_32BIT header;

	struct _ELF_PROGRAM_HEADER_LIST_32BIT * first;
	struct _ELF_PROGRAM_HEADER_LIST_32BIT * prev;
	struct _ELF_PROGRAM_HEADER_LIST_32BIT * next;

} ELF_PROGRAM_HEADER_LIST_32BIT, *P_ELF_PROGRAM_HEADER_LIST_32BIT;

// ---------------------------- SYMBOL TABLE --------------------------------

// Structure for .dlysym symbol table struct
typedef struct _ELF_SECTION_SYMBOL_TABLE_32BIT {

	size_t symTab_name;
	size_t symTab_value;
	size_t symTab_size;
	unsigned char symTab_info;
	unsigned char symTab_other;

} ELF_SECTION_SYMBOL_TABLE_32BIT, *P_ELF_SECTION_SYMBOL_TABLE_32BIT;

// List to house .dynsym table entries
typedef struct _ELF_SECTION_SYMBOL_TABLE_LIST_32BIT {

	char * name;
	size_t index;

	P_ELF_SECTION_SYMBOL_TABLE_32BIT symTab_entry;

	struct _ELF_SECTION_SYMBOL_TABLE_LIST_32BIT * first;
	struct _ELF_SECTION_SYMBOL_TABLE_LIST_32BIT * prev;
	struct _ELF_SECTION_SYMBOL_TABLE_LIST_32BIT * next;

} ELF_SECTION_SYMBOL_TABLE_LIST_32BIT, *P_ELF_SECTION_SYMBOL_TABLE_LIST_32BIT;

// -------------------------- RELOCATIONS --------------------------------------

#define BP_ELF32_R_SYM(i)    ((i)>>8)
#define BP_ELF32_R_TYPE(i)   ((unsigned char)(i))
#define BP_ELF32_R_INFO(s,t) (((s)<<8)+(unsigned char)(t))


typedef struct _ELF_REL {

	size_t rel_offset;
	size_t rel_info;

} ELF_REL, *P_ELF_REL;

typedef struct _ELF_RELA {

	size_t rel_offset;
	size_t rel_info;
	size_t rel_addend;

} ELF_RELA, *P_ELF_RELA;

typedef struct _ELF_RELOCATION_LIST_ENTRY_32BIT{

	// The string name of the corresponding relocation list entry
	char * name;

	// Define switch to choose between the two RELvsRELA structures.
	// The linux implementation uses the REL structure.
	#ifdef USE_ELF_RELA
		P_ELF_RELA rel;
	#endif

	#ifndef USE_ELF_RELA
		P_ELF_REL rel;
	#endif


		struct _ELF_RELOCATION_LIST_ENTRY_32BIT * first;
		struct _ELF_RELOCATION_LIST_ENTRY_32BIT * prev;
		struct _ELF_RELOCATION_LIST_ENTRY_32BIT * next;

} ELF_RELOCATION_LIST_ENTRY_32BIT, *P_ELF_RELOCATION_LIST_ENTRY_32BIT;

typedef struct _ELF_RELOCATIONS_LIST_32BIT {

	char * name;
	P_ELF_RELOCATION_LIST_ENTRY_32BIT entries;

		struct _ELF_RELOCATIONS_LIST_32BIT * first;
		struct _ELF_RELOCATIONS_LIST_32BIT * prev;
		struct _ELF_RELOCATIONS_LIST_32BIT * next;

} ELF_RELOCATIONS_LIST_32BIT, *P_ELF_RELOCATIONS_LIST_32BIT;

// -------------------- .dynamic -----------------------------------

// Dynamic Structure
typedef struct _ELF_DYNAMIC_32BIT {
	size_t dyn_tag;

		// Keep the Union for convention, although unnecessary
		union {
        	size_t dyn_val;
	        size_t dyn_ptr;
		} d_un;

} ELF_DYNAMIC_32BIT, *P_ELF_DYNAMIC_32BIT;

// Used to hold dynamic information
typedef struct _ELF_DYNAMIC_LIST_32BIT {

	P_ELF_DYNAMIC_32BIT info;

		struct _ELF_DYNAMIC_LIST_32BIT * first;
		struct _ELF_DYNAMIC_LIST_32BIT * prev;
		struct _ELF_DYNAMIC_LIST_32BIT * next;

} ELF_DYNAMIC_LIST_32BIT, *P_ELF_DYNAMIC_LIST_32BIT;


// -------------------------------- .got ---------------------------------------

// Used to hold the local GOT
typedef struct _ELF_GOT_LIST_32BIT {

	size_t addr;

		struct _ELF_GOT_LIST_32BIT * first;
		struct _ELF_GOT_LIST_32BIT * prev;
		struct _ELF_GOT_LIST_32BIT * next;

} ELF_GOT_LIST_32BIT, *P_ELF_GOT_LIST_32BIT;

// --------------------- ELF lookup table --------------------------------------

// ELF32EXTRACT enum types for origin
enum {
	ELF_FROM_FS,
	ELF_FROM_RUNNING
};

// This structure holds all relevant ELF32 Structures for easy
// access.  This data is the data that exists for the binary,
// ON THE FILESYSTEM, this may not reflect runtime values.
typedef struct _ELF_32BIT_LOOKUP_TABLE {

	char * filePath;      // Filesystem path, null if extracted from memory.
	int pid;              // Process PID if extracted from memory, 0 if not.

	unsigned char origin; // byte field describing where this table was
						  // extracted from, see elf32extract enum for
						  // possible values.

	// Depending on the system type (windows/linux/etc) save a timestamp
	// from when this data was retrieved.

	// Linux Specific Timestamp
	#ifdef __GNUC__
		struct timeval timestamp;
	#endif

	// Windows Specific Timestamp
	#ifdef WIN32

	#endif

	// ELF Header
	P_ELF_HDR_32BIT header;

	// Program Headers
	P_ELF_PROGRAM_HEADER_LIST_32BIT pHeaders;

	// ELF Header Sections
	P_ELF_SECTION_LIST_32BIT sections;

	// ELF Symbol Table
	P_ELF_SECTION_SYMBOL_TABLE_LIST_32BIT symTab;

	// ALL ELF Relocations
	P_ELF_RELOCATIONS_LIST_32BIT relocs;

	// All ELF Dynamic Information
	P_ELF_DYNAMIC_LIST_32BIT dynamics;

	// All GOT Information
	P_ELF_GOT_LIST_32BIT got;

	// Stored pointer to memory map in which it occurs
	P_MEMORY_MAP memMap;

	// ------------------------------------------------
	// ELF String Table Data
		char * stringTable;
		size_t stringTableSize;

	// void pointer representing a P_X86_32_INSTRUCTION_LIST which
	// starts at top mmap and ends at the end of the memory page.
	// This is set at the refresh data call.
		void *decodedInstructions;

	// list capable pointers (currently unimplemented)
		struct _ELF_32BIT_LOOKUP_TABLE * first;
		struct _ELF_32BIT_LOOKUP_TABLE * prev;
		struct _ELF_32BIT_LOOKUP_TABLE * next;

} ELF_32BIT_LOOKUP_TABLE, *P_ELF_32BIT_LOOKUP_TABLE;

// Elf lookup table list for holding multiple ELF images for a process
typedef struct _ELF_32BIT_LOOKUP_TABLE_LIST {

	// Lookup table pointer
	P_ELF_32BIT_LOOKUP_TABLE header;

		struct _ELF_32BIT_LOOKUP_TABLE_LIST * first;
		struct _ELF_32BIT_LOOKUP_TABLE_LIST * prev;
		struct _ELF_32BIT_LOOKUP_TABLE_LIST * next;

} ELF_32BIT_LOOKUP_TABLE_LIST, *P_ELF_32BIT_LOOKUP_TABLE_LIST;

// --------------------------------------------------------------------
// FUNCTION DECLARATIONS

// Retrieves a section list element by name, such as .got .plt etc
P_ELF_SECTION_LIST_32BIT ELF32_GetSectionListElementByName(P_ELF_32BIT_LOOKUP_TABLE fullHeader, char *name);


// IN ORDER FOR ANY OF THESE FUNCTIONS TO WORK PROPERLY
// READ ACCESS TO THE BINARY IS REQUIRED

// Additionally, several functions defined here are dependent on file stream
// read positions, and as such must be called in a specific order.

// It is recommended to use the ELF32_ExtractAll() function for proper
// extraction of ELF information from ELF binaries.


// ---------------------------------------------------------------------
// FUNCTION SET 1, EXTRACTING ELF HEADERS FROM FILES ON FILESYTEM

// extracts relevant data from the elf header
P_ELF_32BIT_LOOKUP_TABLE ELF32_ExtractAll_File(char * executablePath);


// returns a formatted elf header from a file on the filesystem
P_ELF_HDR_32BIT ELF32_ExtractHeader_File(FILE *elfFile);



// returns a section list of all sections in a file given
// a valid elf header.
P_ELF_SECTION_LIST_32BIT ELF32_ExtractSectionHeaders_File(P_ELF_32BIT_LOOKUP_TABLE fullHeader, FILE *elfFile);

// extract a section and return a section list element; all pointers are
// NULL.
P_ELF_SECTION_HDR_32BIT ELF32_ExtractSectionHeader_File(P_ELF_32BIT_LOOKUP_TABLE fullHeader, FILE *elfFile);



// returns a linked list representing all of the program headers found in the
// file.
P_ELF_PROGRAM_HEADER_LIST_32BIT ELF32_ExtractProgramHeaders_File(P_ELF_32BIT_LOOKUP_TABLE fullHeader, FILE *elfFile);

// NON-Plural version of the above function, extracts one program header from the file
// given a correct stream position.  If the file stream is in the wrong position the
// function will return invalid values.  See above for details.
P_ELF_PROGRAM_HEADER_32BIT ELF32_ExtractProgramHeader_File(P_ELF_32BIT_LOOKUP_TABLE fullHeader, FILE * elfFile);

// Extract actual sections from the full headers section list, results are stored
// within the section list elements of the lookup table
int ELF32_ExtractSections_File(P_ELF_32BIT_LOOKUP_TABLE fullHeader, FILE *elfFile);


// ---------------------------------------------------------------------
// FUNCTION SET 2, EXTRACTING ELF HEADERS FROM RUNTIME MEMORY SEGMENTS
// MARKED ELF.

// extracts relevant data from the elf header
P_ELF_32BIT_LOOKUP_TABLE ELF32_ExtractAll_Mem(P_DEBUG_HANDLE dbgHandle, P_MEMORY_MAP memMap);

// returns a formatted elf header from a file on the filesystem
P_ELF_HDR_32BIT ELF32_ExtractHeader_Mem(P_DEBUG_HANDLE dbgHandle, P_MEMORY_MAP memMap);

// returns a section list of all sections in a file given
// a valid elf header.
P_ELF_SECTION_LIST_32BIT ELF32_ExtractSectionHeaders_Mem(P_ELF_32BIT_LOOKUP_TABLE fullHeader, P_DEBUG_HANDLE dbgHandle, P_MEMORY_MAP memMap);

// extract a section and return a section list element; all pointers are
// NULL.
P_ELF_SECTION_HDR_32BIT ELF32_ExtractSectionHeader_Mem(P_ELF_32BIT_LOOKUP_TABLE fullHeader, size_t sOffset, P_DEBUG_HANDLE dbgHandle, P_MEMORY_MAP memMap);



// returns a linked list representing all of the program headers found in the
// file.
P_ELF_PROGRAM_HEADER_LIST_32BIT ELF32_ExtractProgramHeaders_Mem(P_ELF_32BIT_LOOKUP_TABLE fullHeader, P_DEBUG_HANDLE dbgHandle, P_MEMORY_MAP memMap);

// NON-Plural version of the above function, extracts one program header from the file
// given a correct stream position.  If the file stream is in the wrong position the
// function will return invalid values.  See above for details.
P_ELF_PROGRAM_HEADER_32BIT ELF32_ExtractProgramHeader_Mem(P_ELF_32BIT_LOOKUP_TABLE fullHeader, P_DEBUG_HANDLE dbgHandle, P_MEMORY_MAP memMap);

// Extract actual sections from the full headers section list, results are stored
// within the section list elements of the lookup table
int ELF32_ExtractSections_Mem(P_ELF_32BIT_LOOKUP_TABLE fullHeader, P_DEBUG_HANDLE dbgHandle, P_MEMORY_MAP memMap);



// --------------------------------------------------------
// Data source neutral functions

// Creates a new section list (empty)
P_ELF_SECTION_LIST_32BIT ELF32_NewSectionList();

// This function is responsible for the extraction of all elf
// sections in memory, given a valid debug handle, and memory map.
// Will return a linked list with each element containing a
// elf lookup table which can be used to make determinations about
// process health.
P_ELF_32BIT_LOOKUP_TABLE_LIST ELF32_ExtractHeaderListAll_Mem(P_DEBUG_HANDLE dbgHandle);

// Header Visualization (print) functions
// ---------------------------------------------------------------------
int ELF32_PrintAll(P_ELF_32BIT_LOOKUP_TABLE elfLookupTable);
int ELF32_PrintHeader(P_ELF_HDR_32BIT header);
int ELF32_PrintProgramHeaders(P_ELF_PROGRAM_HEADER_LIST_32BIT pList);
int ELF32_PrintDynamics(P_ELF_DYNAMIC_LIST_32BIT dList);

int ELF32_PrintSectionListElement(P_ELF_SECTION_LIST_32BIT sList);
int ELF32_PrintSectionHeader(P_ELF_SECTION_HDR_32BIT sHeader);
int ELF32_PrintSectionHeaders(P_ELF_SECTION_LIST_32BIT sHeader);


// Prints out information regarding the elf lookup table which is primarily
// for diagnostics.
int ELF32_PrintLookupPtrForDiagnostics(P_ELF_32BIT_LOOKUP_TABLE elfLookupTable);


#endif // #if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1

#endif // USE_BLOCKPARTY_DEBUGGING == 1

#endif

#ifndef BPX86INSTDECODE_H_
#define BPX86INSTDECODE_H_


#if USE_BLOCKPARTY_DEBUGGING == 1

#if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1




// Can be changed if instruction set changes
#define TOP_INSTRUCTION 1222


// Intel Instruction Format:
//  Instructions are 1 - 16 bytes maximum (although not theoretically reachable)
//
// Instruction Layout Table: Current as of May 2007
// ------------------------------------------------------------------
// [Instruction Prefixes] 1 to 4 bytes (Section 1)
// [     Opcode         ] 1, 2, or 3 byte opcode (Section 2)
// [     ModR/M         ] 1 byte (Section 3)
// [       SIB          ] 1 byte (Section 4)
// [    Displacement    ] 1, 2, or 4 bytes (Section 5)
// [    Immediate       ] 1, 2, or 4 bytes (Section 6)



//  Register type definitions.

enum {
	INVALID,
	VALID
};

// Displacements for RM byte, they require a SIB byte
enum {
	RM_DISP_NONE,
	RM_DISP8,
	RM_DISP16,
	RM_DISP32
};

// OA - Opcode Argument Enum Table
enum {

	OA_VOID,     // empty or unset

	// aka /digit
	OA_RM_ONLY_0, OA_RM_ONLY_1, OA_RM_ONLY_2, OA_RM_ONLY_3,
	OA_RM_ONLY_4, OA_RM_ONLY_5, OA_RM_ONLY_6, OA_RM_ONLY_7,

	OA_MODRM,    // aka /r

	OA_I,        // aka i
	OA_CB,	OA_CW,	OA_CD,  // CB Series
	OA_CP,	OA_CO,	OA_CT,

	OA_IB,  OA_IW,  OA_ID,  // IB Series
	OA_IO,

	OA_RB,  OA_RW,  OA_RD,  // +RB Series
	OA_RO,
};




// IA -Instruction Argument Enum Table
enum {
	// empty
	IA_VOID,

	// Anomolies first: Not found in the instruction information
	IA_M2BYTE, // 2 byte memory operand, FLDCW instruction
	IA_1, IA_3,
	IA_DR0_DR7,
	IA_M14_28BYTE, IA_M512BYTE, IA_M80, IA_M80BCD,
	IA_M94_108BYTE,
	IA_R16_M16, IA_R32_M16,

	// Registers
	IA_EAX,
	IA_AL, IA_AX, IA_CL, IA_DX,

	// Segment Selectors
	IA_CS, IA_DS, IA_ES, IA_FS, IA_GS, IA_SS,

	// CR Registers
	IA_CR_0, IA_CR_1, IA_CR_2, IA_CR_3,
	IA_CR_4, IA_CR_5, IA_CR_6, IA_CR_7,

	// (rel8) Relative addressing (same code segment)
	IA_REL8, IA_REL16, IA_REL32, IA_REL64,

	// (ptr16:16) Far Pointer, different code segment.
	IA_PTR16_16, IA_PTR16_32, IA_PTR16_64,

	// (r8) One of the general purpose registers
	IA_R8, IA_R16, IA_R32, IA_R64,

	// (imm8) An immediate byte value. Represented as signed.
	IA_IMM8, IA_IMM16, IA_IMM32, IA_IMM64,

	// (r/m8) Either a register, or a memory destination (RM)
	IA_RM8, IA_RM16, IA_RM32, IA_RM64,

	// (m) Operand in memory. (IA_MEM is alias for IA_M, left in to
	// follow the intel instruction documentation).
	IA_MEM, IA_M, IA_M8, IA_M16, IA_M32, IA_M64, IA_M128,

	// (m16:16) Far pointer. Left is segment selector, right is offset.
	IA_M16_16, IA_M16_32, IA_M16_64,

	// (m16&32) Memory operand consistin of data items pairs.
	IA_M16_AND_32, IA_M16_AND_16, IA_M32_AND_32, IA_M16_AND_64,

	// (moffs8) Memory offset relativie to segment base. (no mod rm)
	IA_MOFFS8, IA_MOFFS16, IA_MOFFS32, IA_MOFFS64,

	// (Sreg) Segment register. 0 - 5 (IA SDM Vol2a)
	IA_SREG,

	// (m32fp) Scaled precision floating point operands in memory.
	IA_M32FP, IA_M64FP, IA_M80FP,

	// (m16int) Integer operand in memory. Used as operands for x87.
	IA_M16INT, IA_M32INT, IA_M64INT,

	// (ST) Element on the FPU register stack.
	IA_ST_0, IA_ST_1, IA_ST_2, IA_ST_3,
	IA_ST_4, IA_ST_5, IA_ST_6, IA_ST_7,
	IA_ST_I,

	// (mm) MMX Register. MM0 - MM7
	IA_MM, IA_MM32,

	// (mm/m32) Low order bits of a MMX Register or Memory operand.
	IA_MM_M32, IA_MM_M64,

	// (xmm) A XMM Register (128-bit)
	IA_XMM,

	// (xmm/m32) XMM Register or Memory operand.
	IA_XMM_M32, IA_XMM_M64, IA_XMM_M128
};

// --------------------------------------------------------------------------
// Define Section 1: All x86 Instruction Prefixes

// Group Defines
#define IA32_PREFIX_GROUP_1 0x00
#define IA32_PREFIX_GROUP_2 0x01
#define IA32_PREFIX_GROUP_3 0x02
#define IA32_PREFIX_GROUP_4 0x03

// Group 1: Lock and Repeat Prefixes
	#define IA32_LOCK_PREFIX_G1 0xf0
	#define IA32_REPN_PREFIX_G1 0xf2
	#define IA32_REP_PREFIX_G1  0xf3

// Group 2: Segment Override Prefixes
	// Segment Overrides
	#define IA32_CS_SEGMENT_OVERRIDE_PREFIX_G2 0x2e
	#define IA32_SS_SEGMENT_OVERRIDE_PREFIX_G2 0x36
	#define IA32_DS_SEGMENT_OVERRIDE_PREFIX_G2 0x3e
	#define IA32_ES_SEGMENT_OVERRIDE_PREFIX_G2 0x26
	#define IA32_FS_SEGMENT_OVERRIDE_PREFIX_G2 0x64
	#define IA32_GS_SEGMENT_OVERRIDE_PREFIX_G2 0x65

	// Branch Hints: Used only with Jxx instructions
	#define IA32_BRANCH_NOT_TAKEN_PREFIX_G1  0x2e
	#define IA32_BRANCH_TAKEN_PREFIX_G2      0x3e

// Group 3: Operand size override prefix
	#define IA32_OPERAND_SIZE_OVERRIDE_PREFIX_G4 0x66
	#define IA32_MANDATORY_PREFIX_PREFIX_G3      0x0f

// Group 4: Address size override prefix
	#define IA32_ADDRESS_SIZE_OVERRIDE_PREFIX_G4 0x67

// --------------------------------------------------------------------------
// Define Section 3: All x86 ModR/M values/macros

// [ mod  ][reg= ][ rm ]
// [2 bits][3bits][3bits]


// Possible ModRm Values:

	// Mod Values
	#define IA32_MOD_0 00000000b
	#define IA32_MOD_1 01000000b
	#define IA32_MOD_2 10000000b
	#define IA32_MOD_3 11000000b

	// REG Values
	#define IA32_REG_EAX 00000000b
	#define IA32_REG_ECX 00001000b
	#define IA32_REG_EDX 00010000b
	#define IA32_REG_EBX 00011000b
	#define IA32_REG_ESP 00100000b
	#define IA32_REG_EBP 00101000b
	#define IA32_REG_ESI 00110000b
	#define IA32_REG_EDI 00111000b

	// RM Values
	#define IA32_RM_EAX 00000000b
	#define IA32_RM_EBX 00000011b
	#define IA32_RM_ECX 00000001b
	#define IA32_RM_EDX 00000010b
	#define IA32_RM_ESP 00000100b
	#define IA32_RM_EBP 00000101b
	#define IA32_RM_ESI 00000110b
	#define IA32_RM_EDI 00000111b

// --------------------------------------------------------------------------
// Define Section 4: All x86 SIB (Scale Index Base) Values/Macros
unsigned char IA32_SIB_GET_SCALE(int i);
unsigned char IA32_SIB_GET_INDEX(int i);
unsigned char IA32_SIB_GET_BASE(int i);

// Possible SIB Values: (Explanation Below)

	// Scale Values
	#define IA32_SCALE_0    00000000b
	#define IA32_SCALE_1    01000000b
	#define IA32_SCALE_2    10000000b
	#define IA32_SCALE_3    11000000b

	// Index Values
	#define IA32_INDEX_EAX  00000000b
	#define IA32_INDEX_ECX  00001000b
	#define IA32_INDEX_EDX  00010000b
	#define IA32_INDEX_EBX  00011000b
	#define IA32_INDEX_NONE 00100000b
	#define IA32_INDEX_EBP  00101000b
	#define IA32_INDEX_ESI  00110000b
	#define IA32_INDEX_EDI  00111000b

	// Base Values
	#define IA32_BASE_EAX   00000000b
	#define IA32_BASE_ECX   00000001b
	#define IA32_BASE_EDX   00000010b
	#define IA32_BASE_EBX   00000011b
	#define IA32_BASE_ESP   00000100b
	#define IA32_BASE_UNDEF 00000101b
	#define IA32_BASE_ESI   00000110b
	#define IA32_BASE_EDI   00000111b

// --------------------------------------------------------------

// All data is static to the structures, as they are
// stored in the binary as static values.
typedef struct _X86_32_INSTRUCTION {

	char * instructionStr;
	char * undecodedStr;

	char *dstOperandStr;
		size_t dstOperand;

	char *srcOperandStr;
		size_t srcOperand;

	char *thirdOperandStr;
		size_t thirdOperand;

	size_t valid64;
	size_t compatLeg;
	size_t VMX;

	char *description;
	char *prefixStr;

	size_t binPrefixLen;
	char *binPrefix;

	size_t binOpcodeLen;
	char *binOpcode;

	size_t opcodeOperandCount;
	size_t opcodeOperand[5];

} X86_32_INSTRUCTION, *P_X86_32_INSTRUCTION, *P_X86_32_INSTRUCTION_TABLE;


// Structure Used to catalog and store instruction prefixes
typedef struct _X86_32_PREFIX {

	// the actual byte of the prefix
	char bytePrefix;

	// if the prefix is unnecessary mark it
	// superflous.
	unsigned char superflous;

	// a string representing the literal prefix
	// definition.
	char * prefixStr;

} X86_32_PREFIX, *P_X86_32_PREFIX;

// List to actually contain instructions
typedef struct _X86_32_DECODED_INSTRUCTION_LIST {

	// if its a valid instruction, this is set to 1 or true
	unsigned char valid;

	// Holds a pointer to the relevant instruction in the
	// instruction table.
	P_X86_32_INSTRUCTION instruction;

	// Occurance in memory if it is found in memory, if not
	// this value is NULL.  Only set if instruction is
	// associated with a memory address.
	void * address;

	// Total length of the encoded instruction
	unsigned char totalLength;

	// Actual mod/rm byte if exists
	unsigned char containsModRM; // 1 or true for enabled
	unsigned char modRM;

	// Holds mod, reg, and rm values
	unsigned char mod;

	char *regStr;
		unsigned char reg;

	char *rmStr;
		unsigned char rm;

	// Actual sib byte if exists
	unsigned char containsSib; // 1 or true for enabled
	unsigned char sib;
	size_t sibDispVal; // 32bit displacement for base 0x5 encodings

	// Tests for displacements
	unsigned char containsDisp; // true or false
	unsigned char dispSize; // 0x8 0x32 for 8bit and 32bit respectively

		// This value is set directly from DecodeInstruction()
		size_t dispVal; // 4 bytes to contain 32bit max

	// Tests for immediates
	unsigned char containsImm; // true or false
	unsigned char immSize; // 0x8 0x32 for 8bit and 32bit respectively

		// This value is set directly from DecodeInstruction()
		size_t immVal; // 4 bytes to contain 32bit max

	// Tests for immediates
	unsigned char containsRel; // true or false
	unsigned char relSize; // 0x8 0x32 for 8bit and 32bit respectively

		// This value is set directly from DecodeInstruction()
		size_t relVal; // 4 bytes to contain 32bit max

	// Pointer operands
	unsigned char containsPtr; // true or false
	unsigned char ptrSize; // 0x8 0x32 for 8bit and 32bit respectively

		// This value is set directly from DecodeInstruction()
		size_t ptrVal; // 4 bytes to contain 32bit max
		size_t ptrDispVal; // 4 bytes to contain 16bit max (32 really)

	// Flag to determine if an instruction contains arbitrary
	// register instead of r/m register.
		size_t containsStaticReg;

	// String pointers that are filled at decode time to
	// reflect decoder capability.
	char *intelDecode;
	// char *attDecode;

	// prefixes: maximum of 14 as shown by investigating
	// intel documentation. They are stored statically
	// here for performance. It prevents the needless
	// malloc'ing of space for prefixes given the size
	// of a malloc chunk header.  WIN.
	X86_32_PREFIX prefixes[14];
	size_t prefixCount;

	// Quick lookups to see if the prefixes
	// contain either the 0x66 or 0x67 (opsize/addr size respectively)
	// True for yes, false for no.
	unsigned char addrSizeOverride;
	unsigned char operSizeOverride;

	// The next few members are discovered during opcode decoding and
	// are used to determine the true instruction being decoded
	// when the instructions being decoded are tricky and rely on
	// rm bytes.

	// Quick lookups for src operand types (src) true/false
		unsigned char srcMemOnly;
		unsigned char srcRegOnly;

	// Quick lookups for dst operand types (dst) true/false
		unsigned char dstMemOnly;
		unsigned char dstRegOnly;

	// Quick lookups for third operand types (third) true/false
		unsigned char thirdMemOnly;
		unsigned char thirdRegOnly;

	// List Pointers
		struct _X86_32_DECODED_INSTRUCTION_LIST *first;
		struct _X86_32_DECODED_INSTRUCTION_LIST *prev;
		struct _X86_32_DECODED_INSTRUCTION_LIST *next;

} X86_32_DECODED_INSTRUCTION_LIST, *P_X86_32_DECODED_INSTRUCTION_LIST, *P_X86_32_DECODED_INSTRUCTION;

// ---------------------------------------------------------------------
// Instruction Retrieval and Test Functions

// Retrieves a newly allocated instruction table containing ever
// instruction in the intel instruction set.
P_X86_32_INSTRUCTION_TABLE X86_32_GetX86InstructionTable();

/*
 * Instruction Test Functions: Used to test various instruction components.
 */

// General Functions
// -------------------------
// tests an instruction to see if it contains a modr/m byte.
size_t X86_32_InstructionContainsModRM(P_X86_32_INSTRUCTION instruction);

// tests an instruction to see if it contains a sib byte
size_t X86_32_InstructionContainsSib(P_X86_32_INSTRUCTION instruction);

// If an instruction can be overridden, that means that it can have
// a 32 bit variant
size_t X86_32_InstructionSizeVaries(P_X86_32_INSTRUCTION instruction);

// Src Functions:
// -------------------------
// Tests an instructions source operand and reports true if it is memory only
size_t X86_32_IsInstructionSrcMemOnly(P_X86_32_INSTRUCTION instruction);

// Tests an instruction src operand and reports true if its register (r8/16/32) only
size_t X86_32_IsInstructionSrcRegOnly(P_X86_32_INSTRUCTION instruction);


// Dst Functions
// ------------------------
// Tests an instructions dest operand and reports true if it is memory only
size_t X86_32_IsInstructionDstMemOnly(P_X86_32_INSTRUCTION instruction);

// Tests an instructions dest operand and reports true if it is memory only
size_t X86_32_IsInstructionDstRegOnly(P_X86_32_INSTRUCTION instruction);


// Third Optional Operand Functions
// ------------------------
// Tests an instructions dest operand and reports true if it is memory only
size_t X86_32_IsInstructionThirdMemOnly(P_X86_32_INSTRUCTION instruction);

// Tests an instructions dest operand and reports true if it is memory only
size_t X86_32_IsInstructionThirdRegOnly(P_X86_32_INSTRUCTION instruction);

// The above functions simply are aliases for these functions with
// instruction operands substituted for the operand values below.
size_t X86_32_IsOperandMemOnly(size_t operand);
size_t X86_32_IsOperandRegOnly(size_t operand);
size_t X86_32_IsOperandRm(size_t operand);
size_t X86_32_IsOperandImmediate(size_t operand);
size_t X86_32_IsOperandControlRegister(size_t operand);
size_t X86_32_IsOperandDebugRegister(size_t operand);
size_t X86_32_IsOperandPointer(size_t operand);
size_t X86_32_IsOperandRel(size_t operand);
size_t X86_32_IsOperandStaticRegOnly(size_t operand);

// determines if a SIB byte contains a displacement
size_t X86_32_SibContainsDisplacement(P_X86_32_DECODED_INSTRUCTION instruction);

// These functions are used to get the typeclass of a register or rm value,
// out of a rm byte. Typeclasses are defined as follows.
//
enum {
	TYPECLASS_REG_NONE,
	TYPECLASS_REG_DEBUG,
	TYPECLASS_REG_CONTROL,
	TYPECLASS_REG_GPR_8,
	TYPECLASS_REG_GPR_16,
	TYPECLASS_REG_GPR_32,
	TYPECLASS_REG_GPR_64,
	TYPECLASS_REG_MM,
	TYPECLASS_REG_XMM,
	TYPECLASS_REG_ST
};

size_t X86_32_GetRmRegTypeClass(size_t operand);

// --------------------------------------------------------
// Instruction Byte Decoders

// Retrieves a decoded instruction list from memory
P_X86_32_DECODED_INSTRUCTION_LIST X86_32_GetInstructionsFromMem(P_DEBUG_HANDLE dbgHandle, P_X86_32_INSTRUCTION instTable, void *start, void *top);

// This will literally decode an instruction, buff cannot be larger then
// 15 bytes as this is the maximum size of an instruction.
P_X86_32_DECODED_INSTRUCTION X86_32_DecodeInstruction(P_X86_32_INSTRUCTION instTable, void *buff, size_t length);

// The Base Decoder Function, will decode instructions in a buffer
// buff, with a second size parameter.  Returns the starting
// element in a walkable list of instruction. Base addr is
// an optional operand used for automatically assigning address values
// for decoded instructions.  They are null otherwise.  Address is
// Previous Instruction length + base address.
P_X86_32_DECODED_INSTRUCTION_LIST X86_32_DecodeInstructions(P_X86_32_INSTRUCTION instTable, unsigned char *buff, size_t length, void *baseAddr);


// Gets a null terminated string representing the operand string for
// an instruction. Will return string in ATT SYNTAX.
char * X86_32_GetDecodedInstOperandString_INTEL(P_X86_32_DECODED_INSTRUCTION instruction);


// This function will look at an arbitrary buffer string, and determine
// how many prefixes, and which specific prefixes are included in the
// following instruction.  This function actually is responsible for
// returning the decoded instruction element used with decoding.  This
// function must always be called before X86_32_DecodeOpcode and when
// calling X86_32_DecodeOpcode, it must be passed as a parameter.
P_X86_32_DECODED_INSTRUCTION X86_32_DecodePrefixes(void *buff);

// This function contains an optimized loop that decodes opcodes
// of 1-3 bytes in length to their respective instruction.  The byte string
// provided must be at least 3 bytes long or an improper decode or potential
// segfault could occur.  Partial decode is gathered from the decode prefixes
P_X86_32_INSTRUCTION X86_32_DecodeOpcode(P_X86_32_INSTRUCTION instTable, P_X86_32_DECODED_INSTRUCTION partialDecode, void *buff);

// Tests the RM byte in an instruction to see if it contains a valid modr/m byte.
size_t X86_32_InstructionContainsValidModRM(P_X86_32_INSTRUCTION instruction);

// this will literally fill in the mod RM character strings in a decoded
// instruction provided a valid RM byte, returns NULL on failure.
P_X86_32_DECODED_INSTRUCTION_LIST X86_32_SetRMValues(P_X86_32_INSTRUCTION instTable,
		  											 P_X86_32_DECODED_INSTRUCTION_LIST inList);

// This function retrieves the proper string for the Reg value
char * X86_32_GetRegString(P_X86_32_DECODED_INSTRUCTION_LIST partialDecode, size_t operand);

// This function retrieves the proper string for the Reg/RM values
char * X86_32_GetRmString( P_X86_32_DECODED_INSTRUCTION_LIST partialDecode, size_t operand );

// Gets the register type value if encoded rm falls on REG instruction
char * X86_32_GetRmRegString(P_X86_32_DECODED_INSTRUCTION_LIST partialDecode, size_t operand);

// Retreives a SIB encoded string reflecting an encoded rm value.
char * X86_32_GetRmSibString(P_X86_32_DECODED_INSTRUCTION_LIST partialDecode);

// Retrieves a register string for plus encoded instructions given
char * X86_32_GetPlusEncodedRegString(P_X86_32_DECODED_INSTRUCTION_LIST partialDecoded, unsigned char *binOpcode);

// ----------------------------------------------------------------

// Creates a new instruction list elements with given pointers
P_X86_32_DECODED_INSTRUCTION X86_32_CreateInstructionElement(
		       P_X86_32_DECODED_INSTRUCTION first,
		       P_X86_32_DECODED_INSTRUCTION prev,
		       P_X86_32_DECODED_INSTRUCTION next);


// Destroys an instruction list, freeing all subcomponents
int X86_32_DestroyInstructionList(P_X86_32_DECODED_INSTRUCTION_LIST destroyList);

// Retrieves list of instruction elements (instructions) given a start
// and end address.  __No list pointers are set for this element.__
P_X86_32_DECODED_INSTRUCTION_LIST X86_32_GetInstructions(P_DEBUG_HANDLE dbgHandle, P_X86_32_INSTRUCTION instTable, void *start, void *top);

// --------------------------------------------------------
// Decoding Case Functions

// Determines if a byte is a prefix, if it is it returns a character string
// containing the literal character represntation of the string. Returns NULL
// on failure.
char * X86_32_IsBytePrefix(unsigned char byte);

// Determines if an instruction is a memory only operand.  This determines
// if the operation uses the [11] modrm table values in /digit and similar
// encoded instructions.
size_t X86_32_IsInstructionMemoryOnly(P_X86_32_INSTRUCTION instruction);

// Determines if an instruction is a register only operand.  Similar IsInstructionMemoryOnly
// in purpose. If true the 00, 01, 10 mod values cannot be used.
size_t X86_32_IsInstructionRegisterOnly(P_X86_32_INSTRUCTION instruction);

// Looks at an RM byte and determines if it's a memory operand
// true for success, false for failure.
size_t X86_32_IsRmMemoryOperand(unsigned char byte);

// Looks at the RM byte and determines if it is a sib encoded
// rm value.
size_t X86_32_IsRmSibEncoded(unsigned char byte);

// Looks at an RM byte and determines if its a register operand
// true for success, false for failure.
size_t X86_32_IsRmRegisterOperand(unsigned char byte);

// Retrieves the /digit number that translates into the reg
// collumn. -1 for fail.  0-7 for digit.
size_t X86_32_GetSlashDigitFromRm(unsigned char byte);


// Determines if the byte is a +rb etc instruction, 0 for fail
int X86_32_IsBytePlusEncodedOpcode(unsigned char *opstr);

// Tests an instruction to see if it contains a size override
int X86_32_ContainsPrefixOpSizeOverride(P_X86_32_INSTRUCTION_TABLE instTable,
											P_X86_32_DECODED_INSTRUCTION_LIST inList);

// Tests an instruction to see if it contains a address size override
int X86_32_ContainsPrefixAddrSizeOverride(P_X86_32_INSTRUCTION instTable,
											P_X86_32_DECODED_INSTRUCTION_LIST inList);


// ------------------------------------------------------
// ModR/M Decoder functions

// Retrieve MOD Value From Byte 00111111 binary
unsigned char IA32_MODRM_GET_MOD(unsigned char i);

// Retrieve RM Value From Byte 11000111 binary
unsigned char IA32_MODRM_GET_REG(unsigned char i);

// Retrieve Digit/Opcode Value From Byte
unsigned char IA32_MODRM_GET_RM(unsigned char i);

// Create Mod Rm from 3 values (untested, should work)
unsigned char IA32_MODRM_CREATE(unsigned char i);


// -------------------------------------------------------
// Utility Functions:

// Returns an appropriate size_t representing the number
// of instructions in an instruction list.  Returns 0 on failure or
// no elements.
size_t X86_32_GetInstructionCount(P_X86_32_DECODED_INSTRUCTION_LIST instructions);
size_t X86_32_GetInstructionCountFromFirst(P_X86_32_DECODED_INSTRUCTION_LIST instructions);

// Retrieves an instruction element by a number offset into the list.  Will
// never return a list pointer greater then max count.  Does not reset list to first
// position. Returns NULL for failure.  0 is first position in the list.
P_X86_32_DECODED_INSTRUCTION_LIST X86_32_GetInstructionAtListPosition(P_X86_32_DECODED_INSTRUCTION_LIST instructions, size_t position);

// Prints out all instructions in a list as if they were intel instructions
int X86_32_PrintInstructionsINTEL(P_X86_32_DECODED_INSTRUCTION_LIST instList);

// An formatted null terminated buffer containing a buffer full of instructions
// as they would be formatted by the print function.  This is a utility function
// used to generate buffers for display in various widgets.
char * X86_32_GenerateInstructionsBufferINTEL(P_X86_32_DECODED_INSTRUCTION_LIST instList);


// Prints out an entire decoded instruction list
int X86_32_PrintDecodedInstructionList(P_X86_32_DECODED_INSTRUCTION_LIST instList);

// Prints out a decoded instruction structure
int X86_32_PrintDecodedInstructionStruct(P_X86_32_DECODED_INSTRUCTION instruction);

// Prints out a single instruction counter
int X86_32_PrintInstructionStruct(P_X86_32_INSTRUCTION instruction);

// Retrieves a string containing a decoded instruction in INTEL Syntax Format
char * X86_32_GetDecodedInstructionINTELSyntax(P_X86_32_DECODED_INSTRUCTION instruction);

#endif // #if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1

#endif // USE_BLOCKPARTY_DEBUGGING == 1

#endif /*BPX86INSTDECODE_H_*/

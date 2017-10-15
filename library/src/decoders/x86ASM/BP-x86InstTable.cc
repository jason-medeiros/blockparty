#include "../../../include/BP-Main.h"

#if USE_BLOCKPARTY_DEBUGGING == 1

/*
 *
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%% 64Bit Debugging Interface %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 *
 */


#if BLOCKPARTY_DEBUGGING_TARGET_64BIT == 1


#endif // #if BLOCKPARTY_DEBUGGING_TARGET_64BIT == 1


/*
 *
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%% 32Bit Debugging Interface %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 *
 */


#if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1

// Returns a null terminated array of structures representing
// the complete intel instruction set. The table is an array of instructions.
P_X86_32_INSTRUCTION X86_32_GetX86InstructionTable(){
	
	// 1211 is the arbitrary instruction count, change to define
	// later.
	X86_32_INSTRUCTION instruction[1225];
	P_X86_32_INSTRUCTION retHeapVar = (P_X86_32_INSTRUCTION) bpmalloc(sizeof(X86_32_INSTRUCTION) * 1225);
	memset(retHeapVar, 0x00, sizeof(X86_32_INSTRUCTION) * 1225);


	// Instruction Struct [0] - AAA
	instruction[0].instructionStr = strdup("AAA");
	instruction[0].undecodedStr = strdup("37");
	instruction[0].valid64 = INVALID;
	instruction[0].compatLeg = VALID;
	instruction[0].VMX = 0;
	instruction[0].description = strdup("ASCII adjust AL after addition.");
	instruction[0].binPrefixLen = 0;
	instruction[0].binOpcodeLen = 1;
	instruction[0].binOpcode = bpstrndup("\x37", 1);
	instruction[0].opcodeOperandCount = 0;

	// Instruction Struct [1] - AAD
	instruction[1].instructionStr = strdup("AAD");
	instruction[1].undecodedStr = strdup("D5 0A");
	instruction[1].valid64 = INVALID;
	instruction[1].compatLeg = VALID;
	instruction[1].VMX = 0;
	instruction[1].description = strdup("ASCII adjust AX before division.");
	instruction[1].binPrefixLen = 0;
	instruction[1].binOpcodeLen = 2;
	instruction[1].binOpcode = bpstrndup("\xD5\x0A", 2);
	instruction[1].opcodeOperandCount = 0;

	// Instruction Struct [2] - AAD
	instruction[2].instructionStr = strdup("AAD");
	instruction[2].undecodedStr = strdup("D5 ib");
	instruction[2].dstOperandStr = strdup("imm8");
	instruction[2].dstOperand = IA_IMM8;
	instruction[2].valid64 = INVALID;
	instruction[2].compatLeg = VALID;
	instruction[2].VMX = 0;
	instruction[2].description = strdup("Adjust AX before division to");
	instruction[2].binPrefixLen = 0;
	instruction[2].binOpcodeLen = 1;
	instruction[2].binOpcode = bpstrndup("\xD5", 1);
	instruction[2].opcodeOperand[0] = OA_IB;
	instruction[2].opcodeOperandCount = 1;

	// Instruction Struct [3] - AAM
	instruction[3].instructionStr = strdup("AAM");
	instruction[3].undecodedStr = strdup("D4 0A");
	instruction[3].valid64 = INVALID;
	instruction[3].compatLeg = VALID;
	instruction[3].VMX = 0;
	instruction[3].description = strdup("ASCII adjust AX after multiply.");
	instruction[3].binPrefixLen = 0;
	instruction[3].binOpcodeLen = 2;
	instruction[3].binOpcode = bpstrndup("\xD4\x0A", 2);
	instruction[3].opcodeOperandCount = 0;

	// Instruction Struct [4] - (No mnemonic) D4 ib (ADD)
	instruction[4].instructionStr = strdup("(No mnemonic) D4 ib (ADD)");
	instruction[4].undecodedStr = strdup("D4 ib");
	instruction[4].valid64 = INVALID;
	instruction[4].compatLeg = VALID;
	instruction[4].VMX = 0;
	instruction[4].description = strdup("Adjust AX after multiply to number base imm8.");
	instruction[4].binPrefixLen = 0;
	instruction[4].binOpcodeLen = 1;
	instruction[4].binOpcode = bpstrndup("\xD4", 1);
	instruction[4].opcodeOperand[0] = OA_IB;
	instruction[4].opcodeOperandCount = 1;

	// Instruction Struct [5] - AAS
	instruction[5].instructionStr = strdup("AAS");
	instruction[5].undecodedStr = strdup("3F");
	instruction[5].valid64 = INVALID;
	instruction[5].compatLeg = VALID;
	instruction[5].VMX = 0;
	instruction[5].description = strdup("ASCII adjust AL after subtraction.");
	instruction[5].binPrefixLen = 0;
	instruction[5].binOpcodeLen = 1;
	instruction[5].binOpcode = bpstrndup("\x3F", 1);
	instruction[5].opcodeOperandCount = 0;

	// Instruction Struct [6] - ADC
	instruction[6].instructionStr = strdup("ADC");
	instruction[6].undecodedStr = strdup("14 ib");
	instruction[6].dstOperandStr = strdup("AL");
	instruction[6].dstOperand = IA_AL;
	instruction[6].srcOperandStr = strdup("imm8");
	instruction[6].srcOperand = IA_IMM8;
	instruction[6].valid64 = INVALID;
	instruction[6].compatLeg = VALID;
	instruction[6].VMX = 0;
	instruction[6].description = strdup("Add with carry imm8 to AL.");
	instruction[6].binPrefixLen = 0;
	instruction[6].binOpcodeLen = 1;
	instruction[6].binOpcode = bpstrndup("\x14", 1);
	instruction[6].opcodeOperand[0] = OA_IB;
	instruction[6].opcodeOperandCount = 1;

	// Instruction Struct [7] - ADC
	instruction[7].instructionStr = strdup("ADC");
	instruction[7].undecodedStr = strdup("15 iw");
	instruction[7].dstOperandStr = strdup("AX");
	instruction[7].dstOperand = IA_AX;
	instruction[7].srcOperandStr = strdup("imm16");
	instruction[7].srcOperand = IA_IMM16;
	instruction[7].valid64 = INVALID;
	instruction[7].compatLeg = VALID;
	instruction[7].VMX = 0;
	instruction[7].description = strdup("Add with carry imm16 to AX.");
	instruction[7].binPrefixLen = 0;
	instruction[7].binOpcodeLen = 1;
	instruction[7].binOpcode = bpstrndup("\x15", 1);
	instruction[7].opcodeOperand[0] = OA_IW;
	instruction[7].opcodeOperandCount = 1;

	// Instruction Struct [8] - ADC
	instruction[8].instructionStr = strdup("ADC");
	instruction[8].undecodedStr = strdup("15 id");
	instruction[8].dstOperandStr = strdup("EAX");
	instruction[8].dstOperand = IA_EAX;
	instruction[8].srcOperandStr = strdup("imm32");
	instruction[8].srcOperand = IA_IMM32;
	instruction[8].valid64 = INVALID;
	instruction[8].compatLeg = VALID;
	instruction[8].VMX = 0;
	instruction[8].description = strdup("Add with carry imm32 to EAX. ADC EAX, imm32");
	instruction[8].binPrefixLen = 0;
	instruction[8].binOpcodeLen = 1;
	instruction[8].binOpcode = bpstrndup("\x15", 1);
	instruction[8].opcodeOperand[0] = OA_ID;
	instruction[8].opcodeOperandCount = 1;

	// Instruction Struct [9] - ADC
	instruction[9].instructionStr = strdup("ADC");
	instruction[9].undecodedStr = strdup("80 /2 ib");
	instruction[9].dstOperandStr = strdup("r/m8");
	instruction[9].dstOperand = IA_RM8;
	instruction[9].srcOperandStr = strdup("imm8");
	instruction[9].srcOperand = IA_IMM8;
	instruction[9].valid64 = INVALID;
	instruction[9].compatLeg = VALID;
	instruction[9].VMX = 0;
	instruction[9].description = strdup("Add with carry imm8 to r/m8.");
	instruction[9].binPrefixLen = 0;
	instruction[9].binOpcodeLen = 1;
	instruction[9].binOpcode = bpstrndup("\x80", 1);
	instruction[9].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[9].opcodeOperand[1] = OA_IB;
	instruction[9].opcodeOperandCount = 2;

	// Instruction Struct [10] - ADC
	instruction[10].instructionStr = strdup("ADC");
	instruction[10].undecodedStr = strdup("81 /2 id");
	instruction[10].dstOperandStr = strdup("r/m32");
	instruction[10].dstOperand = IA_RM32;
	instruction[10].srcOperandStr = strdup("imm32");
	instruction[10].srcOperand = IA_IMM32;
	instruction[10].valid64 = INVALID;
	instruction[10].compatLeg = VALID;
	instruction[10].VMX = 0;
	instruction[10].description = strdup("Add with CF imm32 to r/m32.");
	instruction[10].binPrefixLen = 0;
	instruction[10].binOpcodeLen = 1;
	instruction[10].binOpcode = bpstrndup("\x81", 1);
	instruction[10].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[10].opcodeOperand[1] = OA_ID;
	instruction[10].opcodeOperandCount = 2;

	// Instruction Struct [11] - ADC
	instruction[11].instructionStr = strdup("ADC");
	instruction[11].undecodedStr = strdup("83 /2 ib");
	instruction[11].dstOperandStr = strdup("r/m16");
	instruction[11].dstOperand = IA_RM16;
	instruction[11].srcOperandStr = strdup("imm8");
	instruction[11].srcOperand = IA_IMM8;
	instruction[11].valid64 = INVALID;
	instruction[11].compatLeg = VALID;
	instruction[11].VMX = 0;
	instruction[11].description = strdup("Add with CF sign-extended");
	instruction[11].binPrefixLen = 0;
	instruction[11].binOpcodeLen = 1;
	instruction[11].binOpcode = bpstrndup("\x83", 1);
	instruction[11].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[11].opcodeOperand[1] = OA_IB;
	instruction[11].opcodeOperandCount = 2;

	// Instruction Struct [12] - ADC
	instruction[12].instructionStr = strdup("ADC");
	instruction[12].undecodedStr = strdup("83 /2 ib");
	instruction[12].dstOperandStr = strdup("r/m32");
	instruction[12].dstOperand = IA_RM32;
	instruction[12].srcOperandStr = strdup("imm8");
	instruction[12].srcOperand = IA_IMM8;
	instruction[12].valid64 = INVALID;
	instruction[12].compatLeg = VALID;
	instruction[12].VMX = 0;
	instruction[12].description = strdup("Add with CF sign-extended");
	instruction[12].binPrefixLen = 0;
	instruction[12].binOpcodeLen = 1;
	instruction[12].binOpcode = bpstrndup("\x83", 1);
	instruction[12].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[12].opcodeOperand[1] = OA_IB;
	instruction[12].opcodeOperandCount = 2;

	// Instruction Struct [13] - ADC
	instruction[13].instructionStr = strdup("ADC");
	instruction[13].undecodedStr = strdup("10 /r");
	instruction[13].dstOperandStr = strdup("r/m8");
	instruction[13].dstOperand = IA_RM8;
	instruction[13].srcOperandStr = strdup("r8");
	instruction[13].srcOperand = IA_R8;
	instruction[13].valid64 = INVALID;
	instruction[13].compatLeg = VALID;
	instruction[13].VMX = 0;
	instruction[13].description = strdup("Add with carry byte register to");
	instruction[13].binPrefixLen = 0;
	instruction[13].binOpcodeLen = 1;
	instruction[13].binOpcode = bpstrndup("\x10", 1);
	instruction[13].opcodeOperand[0] = OA_MODRM;
	instruction[13].opcodeOperandCount = 1;

	// Instruction Struct [14] - ADC
	instruction[14].instructionStr = strdup("ADC");
	instruction[14].undecodedStr = strdup("11 /r");
	instruction[14].dstOperandStr = strdup("r/m16");
	instruction[14].dstOperand = IA_RM16;
	instruction[14].srcOperandStr = strdup("r16");
	instruction[14].srcOperand = IA_R16;
	instruction[14].valid64 = INVALID;
	instruction[14].compatLeg = VALID;
	instruction[14].VMX = 0;
	instruction[14].description = strdup("Add with carry r16 to r/m16.");
	instruction[14].binPrefixLen = 0;
	instruction[14].binOpcodeLen = 1;
	instruction[14].binOpcode = bpstrndup("\x11", 1);
	instruction[14].opcodeOperand[0] = OA_MODRM;
	instruction[14].opcodeOperandCount = 1;

	// Instruction Struct [15] - ADC
	instruction[15].instructionStr = strdup("ADC");
	instruction[15].undecodedStr = strdup("11 /r");
	instruction[15].dstOperandStr = strdup("r/m32");
	instruction[15].dstOperand = IA_RM32;
	instruction[15].srcOperandStr = strdup("r32");
	instruction[15].srcOperand = IA_R32;
	instruction[15].valid64 = INVALID;
	instruction[15].compatLeg = VALID;
	instruction[15].VMX = 0;
	instruction[15].description = strdup("Add with CF r32 to r/m32.");
	instruction[15].binPrefixLen = 0;
	instruction[15].binOpcodeLen = 1;
	instruction[15].binOpcode = bpstrndup("\x11", 1);
	instruction[15].opcodeOperand[0] = OA_MODRM;
	instruction[15].opcodeOperandCount = 1;

	// Instruction Struct [16] - ADC
	instruction[16].instructionStr = strdup("ADC");
	instruction[16].undecodedStr = strdup("12 /r");
	instruction[16].dstOperandStr = strdup("r8");
	instruction[16].dstOperand = IA_R8;
	instruction[16].srcOperandStr = strdup("r/m8");
	instruction[16].srcOperand = IA_RM8;
	instruction[16].valid64 = INVALID;
	instruction[16].compatLeg = VALID;
	instruction[16].VMX = 0;
	instruction[16].description = strdup("Add with carry r/m8 to byte");
	instruction[16].binPrefixLen = 0;
	instruction[16].binOpcodeLen = 1;
	instruction[16].binOpcode = bpstrndup("\x12", 1);
	instruction[16].opcodeOperand[0] = OA_MODRM;
	instruction[16].opcodeOperandCount = 1;

	// Instruction Struct [17] - ADC
	instruction[17].instructionStr = strdup("ADC");
	instruction[17].undecodedStr = strdup("13 /r");
	instruction[17].dstOperandStr = strdup("r16");
	instruction[17].dstOperand = IA_R16;
	instruction[17].srcOperandStr = strdup("r/m16");
	instruction[17].srcOperand = IA_RM16;
	instruction[17].valid64 = INVALID;
	instruction[17].compatLeg = VALID;
	instruction[17].VMX = 0;
	instruction[17].description = strdup("Add with carry r/m16 to r16.");
	instruction[17].binPrefixLen = 0;
	instruction[17].binOpcodeLen = 1;
	instruction[17].binOpcode = bpstrndup("\x13", 1);
	instruction[17].opcodeOperand[0] = OA_MODRM;
	instruction[17].opcodeOperandCount = 1;

	// Instruction Struct [18] - ADC
	instruction[18].instructionStr = strdup("ADC");
	instruction[18].undecodedStr = strdup("13 /r");
	instruction[18].dstOperandStr = strdup("r32");
	instruction[18].dstOperand = IA_R32;
	instruction[18].srcOperandStr = strdup("r/m32");
	instruction[18].srcOperand = IA_RM32;
	instruction[18].valid64 = INVALID;
	instruction[18].compatLeg = VALID;
	instruction[18].VMX = 0;
	instruction[18].description = strdup("Add with CF r/m32 to r32.");
	instruction[18].binPrefixLen = 0;
	instruction[18].binOpcodeLen = 1;
	instruction[18].binOpcode = bpstrndup("\x13", 1);
	instruction[18].opcodeOperand[0] = OA_MODRM;
	instruction[18].opcodeOperandCount = 1;

	// Instruction Struct [19] - ADD
	instruction[19].instructionStr = strdup("ADD");
	instruction[19].undecodedStr = strdup("04 ib");
	instruction[19].dstOperandStr = strdup("AL");
	instruction[19].dstOperand = IA_AL;
	instruction[19].srcOperandStr = strdup("imm8");
	instruction[19].srcOperand = IA_IMM8;
	instruction[19].valid64 = INVALID;
	instruction[19].compatLeg = VALID;
	instruction[19].VMX = 0;
	instruction[19].description = strdup("Add imm8 to AL.");
	instruction[19].binPrefixLen = 0;
	instruction[19].binOpcodeLen = 1;
	instruction[19].binOpcode = bpstrndup("\x04", 1);
	instruction[19].opcodeOperand[0] = OA_IB;
	instruction[19].opcodeOperandCount = 1;

	// Instruction Struct [20] - ADD
	instruction[20].instructionStr = strdup("ADD");
	instruction[20].undecodedStr = strdup("05 iw");
	instruction[20].dstOperandStr = strdup("AX");
	instruction[20].dstOperand = IA_AX;
	instruction[20].srcOperandStr = strdup("imm16");
	instruction[20].srcOperand = IA_IMM16;
	instruction[20].valid64 = INVALID;
	instruction[20].compatLeg = VALID;
	instruction[20].VMX = 0;
	instruction[20].description = strdup("Add imm16 to AX.");
	instruction[20].binPrefixLen = 0;
	instruction[20].binOpcodeLen = 1;
	instruction[20].binOpcode = bpstrndup("\x05", 1);
	instruction[20].opcodeOperand[0] = OA_IW;
	instruction[20].opcodeOperandCount = 1;

	// Instruction Struct [21] - ADD
	instruction[21].instructionStr = strdup("ADD");
	instruction[21].undecodedStr = strdup("05 id");
	instruction[21].dstOperandStr = strdup("EAX");
	instruction[21].dstOperand = IA_EAX;
	instruction[21].srcOperandStr = strdup("imm32");
	instruction[21].srcOperand = IA_IMM32;
	instruction[21].valid64 = INVALID;
	instruction[21].compatLeg = VALID;
	instruction[21].VMX = 0;
	instruction[21].description = strdup("Add imm32 to EAX.");
	instruction[21].binPrefixLen = 0;
	instruction[21].binOpcodeLen = 1;
	instruction[21].binOpcode = bpstrndup("\x05", 1);
	instruction[21].opcodeOperand[0] = OA_ID;
	instruction[21].opcodeOperandCount = 1;

	// Instruction Struct [22] - ADD
	instruction[22].instructionStr = strdup("ADD");
	instruction[22].undecodedStr = strdup("80 /0 ib");
	instruction[22].dstOperandStr = strdup("r/m8");
	instruction[22].dstOperand = IA_RM8;
	instruction[22].srcOperandStr = strdup("imm8");
	instruction[22].srcOperand = IA_IMM8;
	instruction[22].valid64 = INVALID;
	instruction[22].compatLeg = VALID;
	instruction[22].VMX = 0;
	instruction[22].description = strdup("Add imm8 to r/m8.");
	instruction[22].binPrefixLen = 0;
	instruction[22].binOpcodeLen = 1;
	instruction[22].binOpcode = bpstrndup("\x80", 1);
	instruction[22].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[22].opcodeOperand[1] = OA_IB;
	instruction[22].opcodeOperandCount = 2;

	// Instruction Struct [23] - ADD
	instruction[23].instructionStr = strdup("ADD");
	instruction[23].undecodedStr = strdup("81 /0 iw");
	instruction[23].dstOperandStr = strdup("r/m16");
	instruction[23].dstOperand = IA_RM16;
	instruction[23].srcOperandStr = strdup("imm16");
	instruction[23].srcOperand = IA_IMM16;
	instruction[23].valid64 = INVALID;
	instruction[23].compatLeg = VALID;
	instruction[23].VMX = 0;
	instruction[23].description = strdup("Add imm16 to r/m16.");
	instruction[23].binPrefixLen = 0;
	instruction[23].binOpcodeLen = 1;
	instruction[23].binOpcode = bpstrndup("\x81", 1);
	instruction[23].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[23].opcodeOperand[1] = OA_IW;
	instruction[23].opcodeOperandCount = 2;

	// Instruction Struct [24] - ADD
	instruction[24].instructionStr = strdup("ADD");
	instruction[24].undecodedStr = strdup("81 /0 id");
	instruction[24].dstOperandStr = strdup("r/m32");
	instruction[24].dstOperand = IA_RM32;
	instruction[24].srcOperandStr = strdup("imm32");
	instruction[24].srcOperand = IA_IMM32;
	instruction[24].valid64 = INVALID;
	instruction[24].compatLeg = VALID;
	instruction[24].VMX = 0;
	instruction[24].description = strdup("Add imm32 to r/m32.");
	instruction[24].binPrefixLen = 0;
	instruction[24].binOpcodeLen = 1;
	instruction[24].binOpcode = bpstrndup("\x81", 1);
	instruction[24].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[24].opcodeOperand[1] = OA_ID;
	instruction[24].opcodeOperandCount = 2;

	// Instruction Struct [25] - ADD
	instruction[25].instructionStr = strdup("ADD");
	instruction[25].undecodedStr = strdup("83 /0 ib");
	instruction[25].dstOperandStr = strdup("r/m16");
	instruction[25].dstOperand = IA_RM16;
	instruction[25].srcOperandStr = strdup("imm8");
	instruction[25].srcOperand = IA_IMM8;
	instruction[25].valid64 = INVALID;
	instruction[25].compatLeg = VALID;
	instruction[25].VMX = 0;
	instruction[25].description = strdup("Add sign-extended");
	instruction[25].binPrefixLen = 0;
	instruction[25].binOpcodeLen = 1;
	instruction[25].binOpcode = bpstrndup("\x83", 1);
	instruction[25].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[25].opcodeOperand[1] = OA_IB;
	instruction[25].opcodeOperandCount = 2;

	// Instruction Struct [26] - ADD
	instruction[26].instructionStr = strdup("ADD");
	instruction[26].undecodedStr = strdup("83 /0 ib");
	instruction[26].dstOperandStr = strdup("r/m32");
	instruction[26].dstOperand = IA_RM32;
	instruction[26].srcOperandStr = strdup("imm8");
	instruction[26].srcOperand = IA_IMM8;
	instruction[26].valid64 = INVALID;
	instruction[26].compatLeg = VALID;
	instruction[26].VMX = 0;
	instruction[26].description = strdup("Add sign-extended");
	instruction[26].binPrefixLen = 0;
	instruction[26].binOpcodeLen = 1;
	instruction[26].binOpcode = bpstrndup("\x83", 1);
	instruction[26].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[26].opcodeOperand[1] = OA_IB;
	instruction[26].opcodeOperandCount = 2;

	// Instruction Struct [27] - ADD
	instruction[27].instructionStr = strdup("ADD");
	instruction[27].undecodedStr = strdup("00 /r");
	instruction[27].dstOperandStr = strdup("r/m8");
	instruction[27].dstOperand = IA_RM8;
	instruction[27].srcOperandStr = strdup("r8");
	instruction[27].srcOperand = IA_R8;
	instruction[27].valid64 = INVALID;
	instruction[27].compatLeg = VALID;
	instruction[27].VMX = 0;
	instruction[27].description = strdup("Add r8 to r/m8.");
	instruction[27].binPrefixLen = 0;
	instruction[27].binOpcodeLen = 1;
	instruction[27].binOpcode = bpstrndup("\x00", 1);
	instruction[27].opcodeOperand[0] = OA_MODRM;
	instruction[27].opcodeOperandCount = 1;

	// Instruction Struct [28] - ADD
	instruction[28].instructionStr = strdup("ADD");
	instruction[28].undecodedStr = strdup("01 /r");
	instruction[28].dstOperandStr = strdup("r/m16");
	instruction[28].dstOperand = IA_RM16;
	instruction[28].srcOperandStr = strdup("r16");
	instruction[28].srcOperand = IA_R16;
	instruction[28].valid64 = INVALID;
	instruction[28].compatLeg = VALID;
	instruction[28].VMX = 0;
	instruction[28].description = strdup("Add r16 to r/m16.");
	instruction[28].binPrefixLen = 0;
	instruction[28].binOpcodeLen = 1;
	instruction[28].binOpcode = bpstrndup("\x01", 1);
	instruction[28].opcodeOperand[0] = OA_MODRM;
	instruction[28].opcodeOperandCount = 1;

	// Instruction Struct [29] - ADD
	instruction[29].instructionStr = strdup("ADD");
	instruction[29].undecodedStr = strdup("01 /r");
	instruction[29].dstOperandStr = strdup("r/m32");
	instruction[29].dstOperand = IA_RM32;
	instruction[29].srcOperandStr = strdup("r32");
	instruction[29].srcOperand = IA_R32;
	instruction[29].valid64 = INVALID;
	instruction[29].compatLeg = VALID;
	instruction[29].VMX = 0;
	instruction[29].description = strdup("Add r32 to r/m32.");
	instruction[29].binPrefixLen = 0;
	instruction[29].binOpcodeLen = 1;
	instruction[29].binOpcode = bpstrndup("\x01", 1);
	instruction[29].opcodeOperand[0] = OA_MODRM;
	instruction[29].opcodeOperandCount = 1;

	// Instruction Struct [30] - ADD
	instruction[30].instructionStr = strdup("ADD");
	instruction[30].undecodedStr = strdup("02 /r");
	instruction[30].dstOperandStr = strdup("r8");
	instruction[30].dstOperand = IA_R8;
	instruction[30].srcOperandStr = strdup("r/m8");
	instruction[30].srcOperand = IA_RM8;
	instruction[30].valid64 = INVALID;
	instruction[30].compatLeg = VALID;
	instruction[30].VMX = 0;
	instruction[30].description = strdup("Add r/m8 to r8.");
	instruction[30].binPrefixLen = 0;
	instruction[30].binOpcodeLen = 1;
	instruction[30].binOpcode = bpstrndup("\x02", 1);
	instruction[30].opcodeOperand[0] = OA_MODRM;
	instruction[30].opcodeOperandCount = 1;

	// Instruction Struct [31] - ADD
	instruction[31].instructionStr = strdup("ADD");
	instruction[31].undecodedStr = strdup("03 /r");
	instruction[31].dstOperandStr = strdup("r16");
	instruction[31].dstOperand = IA_R16;
	instruction[31].srcOperandStr = strdup("r/m16");
	instruction[31].srcOperand = IA_RM16;
	instruction[31].valid64 = INVALID;
	instruction[31].compatLeg = VALID;
	instruction[31].VMX = 0;
	instruction[31].description = strdup("Add r/m16 to r16.");
	instruction[31].binPrefixLen = 0;
	instruction[31].binOpcodeLen = 1;
	instruction[31].binOpcode = bpstrndup("\x03", 1);
	instruction[31].opcodeOperand[0] = OA_MODRM;
	instruction[31].opcodeOperandCount = 1;

	// Instruction Struct [32] - ADD
	instruction[32].instructionStr = strdup("ADD");
	instruction[32].undecodedStr = strdup("03 /r");
	instruction[32].dstOperandStr = strdup("r32");
	instruction[32].dstOperand = IA_R32;
	instruction[32].srcOperandStr = strdup("r/m32");
	instruction[32].srcOperand = IA_RM32;
	instruction[32].valid64 = INVALID;
	instruction[32].compatLeg = VALID;
	instruction[32].VMX = 0;
	instruction[32].description = strdup("Add r/m32 to r32.");
	instruction[32].binPrefixLen = 0;
	instruction[32].binOpcodeLen = 1;
	instruction[32].binOpcode = bpstrndup("\x03", 1);
	instruction[32].opcodeOperand[0] = OA_MODRM;
	instruction[32].opcodeOperandCount = 1;

	// Instruction Struct [33] - ADDPD
	instruction[33].instructionStr = strdup("ADDPD");
	instruction[33].undecodedStr = strdup("66 0F 58 /r");
	instruction[33].dstOperandStr = strdup("xmm1");
	instruction[33].dstOperand = IA_XMM;
	instruction[33].srcOperandStr = strdup("xmm2/m128");
	instruction[33].srcOperand = IA_XMM_M128;
	instruction[33].valid64 = INVALID;
	instruction[33].compatLeg = VALID;
	instruction[33].VMX = 0;
	instruction[33].description = strdup("Add packed double-precision floatingpoint");
	instruction[33].prefixStr = strdup("OPSIZEOVR");
	instruction[33].binPrefixLen = 1;
	instruction[33].binPrefix = bpstrndup("\x66", 1);
	instruction[33].binOpcodeLen = 2;
	instruction[33].binOpcode = bpstrndup("\x0F\x58", 2);
	instruction[33].opcodeOperand[0] = OA_MODRM;
	instruction[33].opcodeOperandCount = 1;

	// Instruction Struct [34] - ADDPS
	instruction[34].instructionStr = strdup("ADDPS");
	instruction[34].undecodedStr = strdup("0F 58 /r");
	instruction[34].dstOperandStr = strdup("xmm1");
	instruction[34].dstOperand = IA_XMM;
	instruction[34].srcOperandStr = strdup("xmm2/m128");
	instruction[34].srcOperand = IA_XMM_M128;
	instruction[34].valid64 = INVALID;
	instruction[34].compatLeg = VALID;
	instruction[34].VMX = 0;
	instruction[34].description = strdup("Add packed single-precision");
	instruction[34].binPrefixLen = 0;
	instruction[34].binOpcodeLen = 2;
	instruction[34].binOpcode = bpstrndup("\x0F\x58", 2);
	instruction[34].opcodeOperand[0] = OA_MODRM;
	instruction[34].opcodeOperandCount = 1;

	// Instruction Struct [35] - ADDSD
	instruction[35].instructionStr = strdup("ADDSD");
	instruction[35].undecodedStr = strdup("F2 0F 58 /r");
	instruction[35].dstOperandStr = strdup("xmm1");
	instruction[35].dstOperand = IA_XMM;
	instruction[35].srcOperandStr = strdup("xmm2/m64");
	instruction[35].srcOperand = IA_XMM_M64;
	instruction[35].valid64 = INVALID;
	instruction[35].compatLeg = VALID;
	instruction[35].VMX = 0;
	instruction[35].description = strdup("Add the low doubleprecision");
	instruction[35].prefixStr = strdup("REPNE");
	instruction[35].binPrefixLen = 1;
	instruction[35].binPrefix = bpstrndup("\xF2", 1);
	instruction[35].binOpcodeLen = 2;
	instruction[35].binOpcode = bpstrndup("\x0F\x58", 2);
	instruction[35].opcodeOperand[0] = OA_MODRM;
	instruction[35].opcodeOperandCount = 1;

	// Instruction Struct [36] - ADDSS
	instruction[36].instructionStr = strdup("ADDSS");
	instruction[36].undecodedStr = strdup("F3 0F 58 /r");
	instruction[36].dstOperandStr = strdup("xmm1");
	instruction[36].dstOperand = IA_XMM;
	instruction[36].srcOperandStr = strdup("xmm2/m32");
	instruction[36].srcOperand = IA_XMM_M32;
	instruction[36].valid64 = INVALID;
	instruction[36].compatLeg = VALID;
	instruction[36].VMX = 0;
	instruction[36].description = strdup("Add the low singleprecision");
	instruction[36].prefixStr = strdup("REP");
	instruction[36].binPrefixLen = 1;
	instruction[36].binPrefix = bpstrndup("\xF3", 1);
	instruction[36].binOpcodeLen = 2;
	instruction[36].binOpcode = bpstrndup("\x0F\x58", 2);
	instruction[36].opcodeOperand[0] = OA_MODRM;
	instruction[36].opcodeOperandCount = 1;

	// Instruction Struct [37] - ADDSUBPD
	instruction[37].instructionStr = strdup("ADDSUBPD");
	instruction[37].undecodedStr = strdup("66 0F D0 /r");
	instruction[37].dstOperandStr = strdup("xmm1");
	instruction[37].dstOperand = IA_XMM;
	instruction[37].srcOperandStr = strdup("xmm2/m128");
	instruction[37].srcOperand = IA_XMM_M128;
	instruction[37].valid64 = INVALID;
	instruction[37].compatLeg = VALID;
	instruction[37].VMX = 0;
	instruction[37].description = strdup("Add/subtract");
	instruction[37].prefixStr = strdup("OPSIZEOVR");
	instruction[37].binPrefixLen = 1;
	instruction[37].binPrefix = bpstrndup("\x66", 1);
	instruction[37].binOpcodeLen = 2;
	instruction[37].binOpcode = bpstrndup("\x0F\xD0", 2);
	instruction[37].opcodeOperand[0] = OA_MODRM;
	instruction[37].opcodeOperandCount = 1;

	// Instruction Struct [38] - ADDSUBPS
	instruction[38].instructionStr = strdup("ADDSUBPS");
	instruction[38].undecodedStr = strdup("F2 0F D0 /r");
	instruction[38].dstOperandStr = strdup("xmm1");
	instruction[38].dstOperand = IA_XMM;
	instruction[38].srcOperandStr = strdup("xmm2/m128");
	instruction[38].srcOperand = IA_XMM_M128;
	instruction[38].valid64 = INVALID;
	instruction[38].compatLeg = VALID;
	instruction[38].VMX = 0;
	instruction[38].description = strdup("Add/subtract singleprecision");
	instruction[38].prefixStr = strdup("REPNE");
	instruction[38].binPrefixLen = 1;
	instruction[38].binPrefix = bpstrndup("\xF2", 1);
	instruction[38].binOpcodeLen = 2;
	instruction[38].binOpcode = bpstrndup("\x0F\xD0", 2);
	instruction[38].opcodeOperand[0] = OA_MODRM;
	instruction[38].opcodeOperandCount = 1;

	// Instruction Struct [39] - AND
	instruction[39].instructionStr = strdup("AND");
	instruction[39].undecodedStr = strdup("24 ib");
	instruction[39].dstOperandStr = strdup("AL");
	instruction[39].dstOperand = IA_AL;
	instruction[39].srcOperandStr = strdup("imm8");
	instruction[39].srcOperand = IA_IMM8;
	instruction[39].valid64 = INVALID;
	instruction[39].compatLeg = VALID;
	instruction[39].VMX = 0;
	instruction[39].description = strdup("AL AND imm8.");
	instruction[39].binPrefixLen = 0;
	instruction[39].binOpcodeLen = 1;
	instruction[39].binOpcode = bpstrndup("\x24", 1);
	instruction[39].opcodeOperand[0] = OA_IB;
	instruction[39].opcodeOperandCount = 1;

	// Instruction Struct [40] - AND
	instruction[40].instructionStr = strdup("AND");
	instruction[40].undecodedStr = strdup("25 iw");
	instruction[40].dstOperandStr = strdup("AX");
	instruction[40].dstOperand = IA_AX;
	instruction[40].srcOperandStr = strdup("imm16");
	instruction[40].srcOperand = IA_IMM16;
	instruction[40].valid64 = INVALID;
	instruction[40].compatLeg = VALID;
	instruction[40].VMX = 0;
	instruction[40].description = strdup("AX AND imm16.");
	instruction[40].binPrefixLen = 0;
	instruction[40].binOpcodeLen = 1;
	instruction[40].binOpcode = bpstrndup("\x25", 1);
	instruction[40].opcodeOperand[0] = OA_IW;
	instruction[40].opcodeOperandCount = 1;

	// Instruction Struct [41] - AND
	instruction[41].instructionStr = strdup("AND");
	instruction[41].undecodedStr = strdup("25 id");
	instruction[41].dstOperandStr = strdup("EAX");
	instruction[41].dstOperand = IA_EAX;
	instruction[41].srcOperandStr = strdup("imm32");
	instruction[41].srcOperand = IA_IMM32;
	instruction[41].valid64 = INVALID;
	instruction[41].compatLeg = VALID;
	instruction[41].VMX = 0;
	instruction[41].description = strdup("EAX AND imm32.");
	instruction[41].binPrefixLen = 0;
	instruction[41].binOpcodeLen = 1;
	instruction[41].binOpcode = bpstrndup("\x25", 1);
	instruction[41].opcodeOperand[0] = OA_ID;
	instruction[41].opcodeOperandCount = 1;

	// Instruction Struct [42] - AND
	instruction[42].instructionStr = strdup("AND");
	instruction[42].undecodedStr = strdup("80 /4 ib");
	instruction[42].dstOperandStr = strdup("r/m8");
	instruction[42].dstOperand = IA_RM8;
	instruction[42].srcOperandStr = strdup("imm8");
	instruction[42].srcOperand = IA_IMM8;
	instruction[42].valid64 = INVALID;
	instruction[42].compatLeg = VALID;
	instruction[42].VMX = 0;
	instruction[42].description = strdup("r/m8 AND imm8.");
	instruction[42].binPrefixLen = 0;
	instruction[42].binOpcodeLen = 1;
	instruction[42].binOpcode = bpstrndup("\x80", 1);
	instruction[42].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[42].opcodeOperand[1] = OA_IB;
	instruction[42].opcodeOperandCount = 2;

	// Instruction Struct [43] - AND
	instruction[43].instructionStr = strdup("AND");
	instruction[43].undecodedStr = strdup("81 /4 iw");
	instruction[43].dstOperandStr = strdup("r/m16");
	instruction[43].dstOperand = IA_RM16;
	instruction[43].srcOperandStr = strdup("imm16");
	instruction[43].srcOperand = IA_IMM16;
	instruction[43].valid64 = INVALID;
	instruction[43].compatLeg = VALID;
	instruction[43].VMX = 0;
	instruction[43].description = strdup("r/m16 AND imm16.");
	instruction[43].binPrefixLen = 0;
	instruction[43].binOpcodeLen = 1;
	instruction[43].binOpcode = bpstrndup("\x81", 1);
	instruction[43].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[43].opcodeOperand[1] = OA_IW;
	instruction[43].opcodeOperandCount = 2;

	// Instruction Struct [44] - AND
	instruction[44].instructionStr = strdup("AND");
	instruction[44].undecodedStr = strdup("81 /4 id");
	instruction[44].dstOperandStr = strdup("r/m32");
	instruction[44].dstOperand = IA_RM32;
	instruction[44].srcOperandStr = strdup("imm32");
	instruction[44].srcOperand = IA_IMM32;
	instruction[44].valid64 = INVALID;
	instruction[44].compatLeg = VALID;
	instruction[44].VMX = 0;
	instruction[44].description = strdup("r/m32 AND imm32.");
	instruction[44].binPrefixLen = 0;
	instruction[44].binOpcodeLen = 1;
	instruction[44].binOpcode = bpstrndup("\x81", 1);
	instruction[44].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[44].opcodeOperand[1] = OA_ID;
	instruction[44].opcodeOperandCount = 2;

	// Instruction Struct [45] - AND
	instruction[45].instructionStr = strdup("AND");
	instruction[45].undecodedStr = strdup("83 /4 ib");
	instruction[45].dstOperandStr = strdup("r/m16");
	instruction[45].dstOperand = IA_RM16;
	instruction[45].srcOperandStr = strdup("imm8");
	instruction[45].srcOperand = IA_IMM8;
	instruction[45].valid64 = INVALID;
	instruction[45].compatLeg = VALID;
	instruction[45].VMX = 0;
	instruction[45].description = strdup("r/m16 AND imm8 (signextended).");
	instruction[45].binPrefixLen = 0;
	instruction[45].binOpcodeLen = 1;
	instruction[45].binOpcode = bpstrndup("\x83", 1);
	instruction[45].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[45].opcodeOperand[1] = OA_IB;
	instruction[45].opcodeOperandCount = 2;

	// Instruction Struct [46] - AND
	instruction[46].instructionStr = strdup("AND");
	instruction[46].undecodedStr = strdup("20 /r");
	instruction[46].dstOperandStr = strdup("r/m8");
	instruction[46].dstOperand = IA_RM8;
	instruction[46].srcOperandStr = strdup("r8");
	instruction[46].srcOperand = IA_R8;
	instruction[46].valid64 = INVALID;
	instruction[46].compatLeg = VALID;
	instruction[46].VMX = 0;
	instruction[46].description = strdup("r/m8 AND r8.");
	instruction[46].binPrefixLen = 0;
	instruction[46].binOpcodeLen = 1;
	instruction[46].binOpcode = bpstrndup("\x20", 1);
	instruction[46].opcodeOperand[0] = OA_MODRM;
	instruction[46].opcodeOperandCount = 1;

	// Instruction Struct [47] - AND
	instruction[47].instructionStr = strdup("AND");
	instruction[47].undecodedStr = strdup("21 /r");
	instruction[47].dstOperandStr = strdup("r/m16");
	instruction[47].dstOperand = IA_RM16;
	instruction[47].srcOperandStr = strdup("r16");
	instruction[47].srcOperand = IA_R16;
	instruction[47].valid64 = INVALID;
	instruction[47].compatLeg = VALID;
	instruction[47].VMX = 0;
	instruction[47].description = strdup("r/m16 AND r16.");
	instruction[47].binPrefixLen = 0;
	instruction[47].binOpcodeLen = 1;
	instruction[47].binOpcode = bpstrndup("\x21", 1);
	instruction[47].opcodeOperand[0] = OA_MODRM;
	instruction[47].opcodeOperandCount = 1;

	// Instruction Struct [48] - AND
	instruction[48].instructionStr = strdup("AND");
	instruction[48].undecodedStr = strdup("21 /r");
	instruction[48].dstOperandStr = strdup("r/m32");
	instruction[48].dstOperand = IA_RM32;
	instruction[48].srcOperandStr = strdup("r32");
	instruction[48].srcOperand = IA_R32;
	instruction[48].valid64 = INVALID;
	instruction[48].compatLeg = VALID;
	instruction[48].VMX = 0;
	instruction[48].description = strdup("r/m32 AND r32.");
	instruction[48].binPrefixLen = 0;
	instruction[48].binOpcodeLen = 1;
	instruction[48].binOpcode = bpstrndup("\x21", 1);
	instruction[48].opcodeOperand[0] = OA_MODRM;
	instruction[48].opcodeOperandCount = 1;

	// Instruction Struct [49] - AND
	instruction[49].instructionStr = strdup("AND");
	instruction[49].undecodedStr = strdup("22 /r");
	instruction[49].dstOperandStr = strdup("r8");
	instruction[49].dstOperand = IA_R8;
	instruction[49].srcOperandStr = strdup("r/m8");
	instruction[49].srcOperand = IA_RM8;
	instruction[49].valid64 = INVALID;
	instruction[49].compatLeg = VALID;
	instruction[49].VMX = 0;
	instruction[49].description = strdup("r8 AND r/m8.");
	instruction[49].binPrefixLen = 0;
	instruction[49].binOpcodeLen = 1;
	instruction[49].binOpcode = bpstrndup("\x22", 1);
	instruction[49].opcodeOperand[0] = OA_MODRM;
	instruction[49].opcodeOperandCount = 1;

	// Instruction Struct [50] - AND
	instruction[50].instructionStr = strdup("AND");
	instruction[50].undecodedStr = strdup("23 /r");
	instruction[50].dstOperandStr = strdup("r16");
	instruction[50].dstOperand = IA_R16;
	instruction[50].srcOperandStr = strdup("r/m16");
	instruction[50].srcOperand = IA_RM16;
	instruction[50].valid64 = INVALID;
	instruction[50].compatLeg = VALID;
	instruction[50].VMX = 0;
	instruction[50].description = strdup("r16 AND r/m16.");
	instruction[50].binPrefixLen = 0;
	instruction[50].binOpcodeLen = 1;
	instruction[50].binOpcode = bpstrndup("\x23", 1);
	instruction[50].opcodeOperand[0] = OA_MODRM;
	instruction[50].opcodeOperandCount = 1;

	// Instruction Struct [51] - ANDPD
	instruction[51].instructionStr = strdup("ANDPD");
	instruction[51].undecodedStr = strdup("66 0F 54 /r");
	instruction[51].dstOperandStr = strdup("xmm1");
	instruction[51].dstOperand = IA_XMM;
	instruction[51].srcOperandStr = strdup("xmm2/m128");
	instruction[51].srcOperand = IA_XMM_M128;
	instruction[51].valid64 = INVALID;
	instruction[51].compatLeg = VALID;
	instruction[51].VMX = 0;
	instruction[51].description = strdup("Bitwise logical AND of xmm2/m128 and");
	instruction[51].prefixStr = strdup("OPSIZEOVR");
	instruction[51].binPrefixLen = 1;
	instruction[51].binPrefix = bpstrndup("\x66", 1);
	instruction[51].binOpcodeLen = 2;
	instruction[51].binOpcode = bpstrndup("\x0F\x54", 2);
	instruction[51].opcodeOperand[0] = OA_MODRM;
	instruction[51].opcodeOperandCount = 1;

	// Instruction Struct [52] - ANDPS
	instruction[52].instructionStr = strdup("ANDPS");
	instruction[52].undecodedStr = strdup("0F 54 /r");
	instruction[52].dstOperandStr = strdup("xmm1");
	instruction[52].dstOperand = IA_XMM;
	instruction[52].srcOperandStr = strdup("xmm2/m128");
	instruction[52].srcOperand = IA_XMM_M128;
	instruction[52].valid64 = INVALID;
	instruction[52].compatLeg = VALID;
	instruction[52].VMX = 0;
	instruction[52].description = strdup("Bitwise logical AND of");
	instruction[52].binPrefixLen = 0;
	instruction[52].binOpcodeLen = 2;
	instruction[52].binOpcode = bpstrndup("\x0F\x54", 2);
	instruction[52].opcodeOperand[0] = OA_MODRM;
	instruction[52].opcodeOperandCount = 1;

	// Instruction Struct [53] - ANDNPD
	instruction[53].instructionStr = strdup("ANDNPD");
	instruction[53].undecodedStr = strdup("66 0F 55 /r");
	instruction[53].dstOperandStr = strdup("xmm1");
	instruction[53].dstOperand = IA_XMM;
	instruction[53].srcOperandStr = strdup("xmm2/m128");
	instruction[53].srcOperand = IA_XMM_M128;
	instruction[53].valid64 = INVALID;
	instruction[53].compatLeg = VALID;
	instruction[53].VMX = 0;
	instruction[53].description = strdup("Bitwise logical AND");
	instruction[53].prefixStr = strdup("OPSIZEOVR");
	instruction[53].binPrefixLen = 1;
	instruction[53].binPrefix = bpstrndup("\x66", 1);
	instruction[53].binOpcodeLen = 2;
	instruction[53].binOpcode = bpstrndup("\x0F\x55", 2);
	instruction[53].opcodeOperand[0] = OA_MODRM;
	instruction[53].opcodeOperandCount = 1;

	// Instruction Struct [54] - ANDNPS
	instruction[54].instructionStr = strdup("ANDNPS");
	instruction[54].undecodedStr = strdup("0F 55 /r");
	instruction[54].dstOperandStr = strdup("xmm1");
	instruction[54].dstOperand = IA_XMM;
	instruction[54].srcOperandStr = strdup("xmm2/m128");
	instruction[54].srcOperand = IA_XMM_M128;
	instruction[54].valid64 = INVALID;
	instruction[54].compatLeg = VALID;
	instruction[54].VMX = 0;
	instruction[54].description = strdup("Bitwise logical AND NOT of");
	instruction[54].binPrefixLen = 0;
	instruction[54].binOpcodeLen = 2;
	instruction[54].binOpcode = bpstrndup("\x0F\x55", 2);
	instruction[54].opcodeOperand[0] = OA_MODRM;
	instruction[54].opcodeOperandCount = 1;

	// Instruction Struct [55] - ARPL
	instruction[55].instructionStr = strdup("ARPL");
	instruction[55].undecodedStr = strdup("63 /r");
	instruction[55].dstOperandStr = strdup("r/m16");
	instruction[55].dstOperand = IA_RM16;
	instruction[55].srcOperandStr = strdup("r16");
	instruction[55].srcOperand = IA_R16;
	instruction[55].valid64 = INVALID;
	instruction[55].compatLeg = VALID;
	instruction[55].VMX = 0;
	instruction[55].description = strdup("Adjust RPL of r/m16 to not less");
	instruction[55].binPrefixLen = 0;
	instruction[55].binOpcodeLen = 1;
	instruction[55].binOpcode = bpstrndup("\x63", 1);
	instruction[55].opcodeOperand[0] = OA_MODRM;
	instruction[55].opcodeOperandCount = 1;

	// Instruction Struct [56] - BOUND
	instruction[56].instructionStr = strdup("BOUND");
	instruction[56].undecodedStr = strdup("62 /r");
	instruction[56].dstOperandStr = strdup("r16");
	instruction[56].dstOperand = IA_R16;
	instruction[56].srcOperandStr = strdup("m16&16");
	instruction[56].srcOperand = IA_M16_AND_16;
	instruction[56].valid64 = INVALID;
	instruction[56].compatLeg = VALID;
	instruction[56].VMX = 0;
	instruction[56].description = strdup("Check if r16 (array index) is");
	instruction[56].binPrefixLen = 0;
	instruction[56].binOpcodeLen = 1;
	instruction[56].binOpcode = bpstrndup("\x62", 1);
	instruction[56].opcodeOperand[0] = OA_MODRM;
	instruction[56].opcodeOperandCount = 1;

	// Instruction Struct [57] - BOUND
	instruction[57].instructionStr = strdup("BOUND");
	instruction[57].undecodedStr = strdup("62 /r");
	instruction[57].dstOperandStr = strdup("r32");
	instruction[57].dstOperand = IA_R32;
	instruction[57].srcOperandStr = strdup("m32&32");
	instruction[57].srcOperand = IA_M32_AND_32;
	instruction[57].valid64 = INVALID;
	instruction[57].compatLeg = VALID;
	instruction[57].VMX = 0;
	instruction[57].description = strdup("Check if r32 (array index) is");
	instruction[57].binPrefixLen = 0;
	instruction[57].binOpcodeLen = 1;
	instruction[57].binOpcode = bpstrndup("\x62", 1);
	instruction[57].opcodeOperand[0] = OA_MODRM;
	instruction[57].opcodeOperandCount = 1;

	// Instruction Struct [58] - BSF
	instruction[58].instructionStr = strdup("BSF");
	instruction[58].undecodedStr = strdup("0F BC /r");
	instruction[58].dstOperandStr = strdup("r16");
	instruction[58].dstOperand = IA_R16;
	instruction[58].srcOperandStr = strdup("r/m16");
	instruction[58].srcOperand = IA_RM16;
	instruction[58].valid64 = INVALID;
	instruction[58].compatLeg = VALID;
	instruction[58].VMX = 0;
	instruction[58].description = strdup("Bit scan forward on r/m16.");
	instruction[58].binPrefixLen = 0;
	instruction[58].binOpcodeLen = 2;
	instruction[58].binOpcode = bpstrndup("\x0F\xBC", 2);
	instruction[58].opcodeOperand[0] = OA_MODRM;
	instruction[58].opcodeOperandCount = 1;

	// Instruction Struct [59] - BSF
	instruction[59].instructionStr = strdup("BSF");
	instruction[59].undecodedStr = strdup("0F BC /r");
	instruction[59].dstOperandStr = strdup("r32");
	instruction[59].dstOperand = IA_R32;
	instruction[59].srcOperandStr = strdup("r/m32");
	instruction[59].srcOperand = IA_RM32;
	instruction[59].valid64 = INVALID;
	instruction[59].compatLeg = VALID;
	instruction[59].VMX = 0;
	instruction[59].description = strdup("Bit scan forward on r/m32.");
	instruction[59].binPrefixLen = 0;
	instruction[59].binOpcodeLen = 2;
	instruction[59].binOpcode = bpstrndup("\x0F\xBC", 2);
	instruction[59].opcodeOperand[0] = OA_MODRM;
	instruction[59].opcodeOperandCount = 1;

	// Instruction Struct [60] - BSR
	instruction[60].instructionStr = strdup("BSR");
	instruction[60].undecodedStr = strdup("0F BD /r");
	instruction[60].dstOperandStr = strdup("r16");
	instruction[60].dstOperand = IA_R16;
	instruction[60].srcOperandStr = strdup("r/m16");
	instruction[60].srcOperand = IA_RM16;
	instruction[60].valid64 = INVALID;
	instruction[60].compatLeg = VALID;
	instruction[60].VMX = 0;
	instruction[60].description = strdup("Bit scan reverse on r/m16.");
	instruction[60].binPrefixLen = 0;
	instruction[60].binOpcodeLen = 2;
	instruction[60].binOpcode = bpstrndup("\x0F\xBD", 2);
	instruction[60].opcodeOperand[0] = OA_MODRM;
	instruction[60].opcodeOperandCount = 1;

	// Instruction Struct [61] - BSR
	instruction[61].instructionStr = strdup("BSR");
	instruction[61].undecodedStr = strdup("0F BD /r");
	instruction[61].dstOperandStr = strdup("r32");
	instruction[61].dstOperand = IA_R32;
	instruction[61].srcOperandStr = strdup("r/m32");
	instruction[61].srcOperand = IA_RM32;
	instruction[61].valid64 = INVALID;
	instruction[61].compatLeg = VALID;
	instruction[61].VMX = 0;
	instruction[61].description = strdup("Bit scan reverse on r/m32.");
	instruction[61].binPrefixLen = 0;
	instruction[61].binOpcodeLen = 2;
	instruction[61].binOpcode = bpstrndup("\x0F\xBD", 2);
	instruction[61].opcodeOperand[0] = OA_MODRM;
	instruction[61].opcodeOperandCount = 1;

	// Instruction Struct [62] - BSWAP
	instruction[62].instructionStr = strdup("BSWAP");
	instruction[62].undecodedStr = strdup("0F C8 +rd");
	instruction[62].dstOperandStr = strdup("r32");
	instruction[62].dstOperand = IA_R32;
	instruction[62].valid64 = INVALID;
	instruction[62].compatLeg = VALID;
	instruction[62].VMX = 0;
	instruction[62].description = strdup("Reverses the byte order of a 32-");
	instruction[62].binPrefixLen = 0;
	instruction[62].binOpcodeLen = 2;
	instruction[62].binOpcode = bpstrndup("\x0F\xC8", 2);
	instruction[62].opcodeOperand[0] = OA_RD;
	instruction[62].opcodeOperandCount = 1;

	// Instruction Struct [63] - BT
	instruction[63].instructionStr = strdup("BT");
	instruction[63].undecodedStr = strdup("0F A3");
	instruction[63].dstOperandStr = strdup("r/m16");
	instruction[63].dstOperand = IA_RM16;
	instruction[63].srcOperandStr = strdup("r16");
	instruction[63].srcOperand = IA_R16;
	instruction[63].valid64 = INVALID;
	instruction[63].compatLeg = VALID;
	instruction[63].VMX = 0;
	instruction[63].description = strdup("Store selected bit in CF");
	instruction[63].binPrefixLen = 0;
	instruction[63].binOpcodeLen = 2;
	instruction[63].binOpcode = bpstrndup("\x0F\xA3", 2);
	instruction[63].opcodeOperandCount = 0;

	// Instruction Struct [64] - BT
	instruction[64].instructionStr = strdup("BT");
	instruction[64].undecodedStr = strdup("0F A3");
	instruction[64].dstOperandStr = strdup("r/m32");
	instruction[64].dstOperand = IA_RM32;
	instruction[64].srcOperandStr = strdup("r32");
	instruction[64].srcOperand = IA_R32;
	instruction[64].valid64 = INVALID;
	instruction[64].compatLeg = VALID;
	instruction[64].VMX = 0;
	instruction[64].description = strdup("Store selected bit in CF");
	instruction[64].binPrefixLen = 0;
	instruction[64].binOpcodeLen = 2;
	instruction[64].binOpcode = bpstrndup("\x0F\xA3", 2);
	instruction[64].opcodeOperandCount = 0;

	// Instruction Struct [65] - BT
	instruction[65].instructionStr = strdup("BT");
	instruction[65].undecodedStr = strdup("0F BA /4 ib");
	instruction[65].dstOperandStr = strdup("r/m16");
	instruction[65].dstOperand = IA_RM16;
	instruction[65].srcOperandStr = strdup("imm8");
	instruction[65].srcOperand = IA_IMM8;
	instruction[65].valid64 = INVALID;
	instruction[65].compatLeg = VALID;
	instruction[65].VMX = 0;
	instruction[65].description = strdup("Store selected bit in CF");
	instruction[65].binPrefixLen = 0;
	instruction[65].binOpcodeLen = 2;
	instruction[65].binOpcode = bpstrndup("\x0F\xBA", 2);
	instruction[65].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[65].opcodeOperand[1] = OA_IB;
	instruction[65].opcodeOperandCount = 2;

	// Instruction Struct [66] - BT
	instruction[66].instructionStr = strdup("BT");
	instruction[66].undecodedStr = strdup("0F BA /4 ib");
	instruction[66].dstOperandStr = strdup("r/m32");
	instruction[66].dstOperand = IA_RM32;
	instruction[66].srcOperandStr = strdup("imm8");
	instruction[66].srcOperand = IA_IMM8;
	instruction[66].valid64 = INVALID;
	instruction[66].compatLeg = VALID;
	instruction[66].VMX = 0;
	instruction[66].description = strdup("Store selected bit in CF");
	instruction[66].binPrefixLen = 0;
	instruction[66].binOpcodeLen = 2;
	instruction[66].binOpcode = bpstrndup("\x0F\xBA", 2);
	instruction[66].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[66].opcodeOperand[1] = OA_IB;
	instruction[66].opcodeOperandCount = 2;

	// Instruction Struct [67] - BTC
	instruction[67].instructionStr = strdup("BTC");
	instruction[67].undecodedStr = strdup("0F BB");
	instruction[67].dstOperandStr = strdup("r/m16");
	instruction[67].dstOperand = IA_RM16;
	instruction[67].srcOperandStr = strdup("r16");
	instruction[67].srcOperand = IA_R16;
	instruction[67].valid64 = INVALID;
	instruction[67].compatLeg = VALID;
	instruction[67].VMX = 0;
	instruction[67].description = strdup("Store selected bit in CF flag and complement.");
	instruction[67].binPrefixLen = 0;
	instruction[67].binOpcodeLen = 2;
	instruction[67].binOpcode = bpstrndup("\x0F\xBB", 2);
	instruction[67].opcodeOperandCount = 0;

	// Instruction Struct [68] - BTC
	instruction[68].instructionStr = strdup("BTC");
	instruction[68].undecodedStr = strdup("0F BB");
	instruction[68].dstOperandStr = strdup("r/m32");
	instruction[68].dstOperand = IA_RM32;
	instruction[68].srcOperandStr = strdup("r32");
	instruction[68].srcOperand = IA_R32;
	instruction[68].valid64 = INVALID;
	instruction[68].compatLeg = VALID;
	instruction[68].VMX = 0;
	instruction[68].description = strdup("Store selected bit in CF flag");
	instruction[68].binPrefixLen = 0;
	instruction[68].binOpcodeLen = 2;
	instruction[68].binOpcode = bpstrndup("\x0F\xBB", 2);
	instruction[68].opcodeOperandCount = 0;

	// Instruction Struct [69] - BTC
	instruction[69].instructionStr = strdup("BTC");
	instruction[69].undecodedStr = strdup("0F BA /7 ib");
	instruction[69].dstOperandStr = strdup("r/m16");
	instruction[69].dstOperand = IA_RM16;
	instruction[69].srcOperandStr = strdup("imm8");
	instruction[69].srcOperand = IA_IMM8;
	instruction[69].valid64 = INVALID;
	instruction[69].compatLeg = VALID;
	instruction[69].VMX = 0;
	instruction[69].description = strdup("Store selected bit in CF flag and complement.");
	instruction[69].binPrefixLen = 0;
	instruction[69].binOpcodeLen = 2;
	instruction[69].binOpcode = bpstrndup("\x0F\xBA", 2);
	instruction[69].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[69].opcodeOperand[1] = OA_IB;
	instruction[69].opcodeOperandCount = 2;

	// Instruction Struct [70] - BTC
	instruction[70].instructionStr = strdup("BTC");
	instruction[70].undecodedStr = strdup("0F BA /7 ib");
	instruction[70].dstOperandStr = strdup("r/m32");
	instruction[70].dstOperand = IA_RM32;
	instruction[70].srcOperandStr = strdup("imm8");
	instruction[70].srcOperand = IA_IMM8;
	instruction[70].valid64 = INVALID;
	instruction[70].compatLeg = VALID;
	instruction[70].VMX = 0;
	instruction[70].description = strdup("Store selected bit in CF flag and complement.");
	instruction[70].binPrefixLen = 0;
	instruction[70].binOpcodeLen = 2;
	instruction[70].binOpcode = bpstrndup("\x0F\xBA", 2);
	instruction[70].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[70].opcodeOperand[1] = OA_IB;
	instruction[70].opcodeOperandCount = 2;

	// Instruction Struct [71] - BTR
	instruction[71].instructionStr = strdup("BTR");
	instruction[71].undecodedStr = strdup("0F B3");
	instruction[71].dstOperandStr = strdup("r/m16");
	instruction[71].dstOperand = IA_RM16;
	instruction[71].srcOperandStr = strdup("r16");
	instruction[71].srcOperand = IA_R16;
	instruction[71].valid64 = INVALID;
	instruction[71].compatLeg = VALID;
	instruction[71].VMX = 0;
	instruction[71].description = strdup("Store selected bit in CF flag and clear.");
	instruction[71].binPrefixLen = 0;
	instruction[71].binOpcodeLen = 2;
	instruction[71].binOpcode = bpstrndup("\x0F\xB3", 2);
	instruction[71].opcodeOperandCount = 0;

	// Instruction Struct [72] - BTR
	instruction[72].instructionStr = strdup("BTR");
	instruction[72].undecodedStr = strdup("0F B3");
	instruction[72].dstOperandStr = strdup("r/m32");
	instruction[72].dstOperand = IA_RM32;
	instruction[72].srcOperandStr = strdup("r32");
	instruction[72].srcOperand = IA_R32;
	instruction[72].valid64 = INVALID;
	instruction[72].compatLeg = VALID;
	instruction[72].VMX = 0;
	instruction[72].description = strdup("Store selected bit in CF flag and clear.");
	instruction[72].binPrefixLen = 0;
	instruction[72].binOpcodeLen = 2;
	instruction[72].binOpcode = bpstrndup("\x0F\xB3", 2);
	instruction[72].opcodeOperandCount = 0;

	// Instruction Struct [73] - BTR
	instruction[73].instructionStr = strdup("BTR");
	instruction[73].undecodedStr = strdup("0F BA /6 ib");
	instruction[73].dstOperandStr = strdup("r/m16");
	instruction[73].dstOperand = IA_RM16;
	instruction[73].srcOperandStr = strdup("imm8");
	instruction[73].srcOperand = IA_IMM8;
	instruction[73].valid64 = INVALID;
	instruction[73].compatLeg = VALID;
	instruction[73].VMX = 0;
	instruction[73].description = strdup("Store selected bit in CF flag and clear.");
	instruction[73].binPrefixLen = 0;
	instruction[73].binOpcodeLen = 2;
	instruction[73].binOpcode = bpstrndup("\x0F\xBA", 2);
	instruction[73].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[73].opcodeOperand[1] = OA_IB;
	instruction[73].opcodeOperandCount = 2;

	// Instruction Struct [74] - BTS
	instruction[74].instructionStr = strdup("BTS");
	instruction[74].undecodedStr = strdup("0F AB");
	instruction[74].dstOperandStr = strdup("r/m16");
	instruction[74].dstOperand = IA_RM16;
	instruction[74].srcOperandStr = strdup("r16");
	instruction[74].srcOperand = IA_R16;
	instruction[74].valid64 = INVALID;
	instruction[74].compatLeg = VALID;
	instruction[74].VMX = 0;
	instruction[74].description = strdup("Store selected bit in CF flag and set.");
	instruction[74].binPrefixLen = 0;
	instruction[74].binOpcodeLen = 2;
	instruction[74].binOpcode = bpstrndup("\x0F\xAB", 2);
	instruction[74].opcodeOperandCount = 0;

	// Instruction Struct [75] - BTS
	instruction[75].instructionStr = strdup("BTS");
	instruction[75].undecodedStr = strdup("0F AB");
	instruction[75].dstOperandStr = strdup("r/m32");
	instruction[75].dstOperand = IA_RM32;
	instruction[75].srcOperandStr = strdup("r32");
	instruction[75].srcOperand = IA_R32;
	instruction[75].valid64 = INVALID;
	instruction[75].compatLeg = VALID;
	instruction[75].VMX = 0;
	instruction[75].description = strdup("Store selected bit in CF flag and set.");
	instruction[75].binPrefixLen = 0;
	instruction[75].binOpcodeLen = 2;
	instruction[75].binOpcode = bpstrndup("\x0F\xAB", 2);
	instruction[75].opcodeOperandCount = 0;

	// Instruction Struct [76] - BTS
	instruction[76].instructionStr = strdup("BTS");
	instruction[76].undecodedStr = strdup("0F BA /5 ib");
	instruction[76].dstOperandStr = strdup("r/m16");
	instruction[76].dstOperand = IA_RM16;
	instruction[76].srcOperandStr = strdup("imm8");
	instruction[76].srcOperand = IA_IMM8;
	instruction[76].valid64 = INVALID;
	instruction[76].compatLeg = VALID;
	instruction[76].VMX = 0;
	instruction[76].description = strdup("Store selected bit in CF flag and set.");
	instruction[76].binPrefixLen = 0;
	instruction[76].binOpcodeLen = 2;
	instruction[76].binOpcode = bpstrndup("\x0F\xBA", 2);
	instruction[76].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[76].opcodeOperand[1] = OA_IB;
	instruction[76].opcodeOperandCount = 2;

	// Instruction Struct [77] - BTS
	instruction[77].instructionStr = strdup("BTS");
	instruction[77].undecodedStr = strdup("0F BA /5 ib");
	instruction[77].dstOperandStr = strdup("r/m32");
	instruction[77].dstOperand = IA_RM32;
	instruction[77].srcOperandStr = strdup("imm8");
	instruction[77].srcOperand = IA_IMM8;
	instruction[77].valid64 = INVALID;
	instruction[77].compatLeg = VALID;
	instruction[77].VMX = 0;
	instruction[77].description = strdup("Store selected bit in CF flag and set.");
	instruction[77].binPrefixLen = 0;
	instruction[77].binOpcodeLen = 2;
	instruction[77].binOpcode = bpstrndup("\x0F\xBA", 2);
	instruction[77].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[77].opcodeOperand[1] = OA_IB;
	instruction[77].opcodeOperandCount = 2;

	// Instruction Struct [78] - CALL
	instruction[78].instructionStr = strdup("CALL");
	instruction[78].undecodedStr = strdup("E8 cw");
	instruction[78].dstOperandStr = strdup("rel16");
	instruction[78].dstOperand = IA_REL16;
	instruction[78].valid64 = INVALID;
	instruction[78].compatLeg = VALID;
	instruction[78].VMX = 0;
	instruction[78].description = strdup("Call near, relative, displacement relative to next instruction.");
	instruction[78].binPrefixLen = 0;
	instruction[78].binOpcodeLen = 1;
	instruction[78].binOpcode = bpstrndup("\xE8", 1);
	instruction[78].opcodeOperand[0] = OA_CW;
	instruction[78].opcodeOperandCount = 1;

	// Instruction Struct [79] - CALL
	instruction[79].instructionStr = strdup("CALL");
	instruction[79].undecodedStr = strdup("E8 cd");
	instruction[79].dstOperandStr = strdup("rel32");
	instruction[79].dstOperand = IA_REL32;
	instruction[79].valid64 = INVALID;
	instruction[79].compatLeg = VALID;
	instruction[79].VMX = 0;
	instruction[79].description = strdup("Call near, relative, displacement relative to next instruction. 32-bit displacement sign extended to 64-bits in 64-bit mode.");
	instruction[79].binPrefixLen = 0;
	instruction[79].binOpcodeLen = 1;
	instruction[79].binOpcode = bpstrndup("\xE8", 1);
	instruction[79].opcodeOperand[0] = OA_CD;
	instruction[79].opcodeOperandCount = 1;

	// Instruction Struct [80] - CALL
	instruction[80].instructionStr = strdup("CALL");
	instruction[80].undecodedStr = strdup("FF /2");
	instruction[80].dstOperandStr = strdup("r/m16");
	instruction[80].dstOperand = IA_RM16;
	instruction[80].valid64 = INVALID;
	instruction[80].compatLeg = VALID;
	instruction[80].VMX = 0;
	instruction[80].description = strdup("Call near, absolute indirect, address given in r/m16.");
	instruction[80].binPrefixLen = 0;
	instruction[80].binOpcodeLen = 1;
	instruction[80].binOpcode = bpstrndup("\xFF", 1);
	instruction[80].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[80].opcodeOperandCount = 1;

	// Instruction Struct [81] - CALL
	instruction[81].instructionStr = strdup("CALL");
	instruction[81].undecodedStr = strdup("FF /2");
	instruction[81].dstOperandStr = strdup("r/m32");
	instruction[81].dstOperand = IA_RM32;
	instruction[81].valid64 = INVALID;
	instruction[81].compatLeg = VALID;
	instruction[81].VMX = 0;
	instruction[81].description = strdup("Call near, absolute indirect, address given in r/m32.");
	instruction[81].binPrefixLen = 0;
	instruction[81].binOpcodeLen = 1;
	instruction[81].binOpcode = bpstrndup("\xFF", 1);
	instruction[81].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[81].opcodeOperandCount = 1;

	// Instruction Struct [82] - CALL
	instruction[82].instructionStr = strdup("CALL");
	instruction[82].undecodedStr = strdup("FF /2");
	instruction[82].dstOperandStr = strdup("r/m64");
	instruction[82].dstOperand = IA_RM64;
	instruction[82].valid64 = INVALID;
	instruction[82].compatLeg = INVALID;
	instruction[82].VMX = 0;
	instruction[82].description = strdup("Call near, absolute indirect, address given in r/m64.");
	instruction[82].binPrefixLen = 0;
	instruction[82].binOpcodeLen = 1;
	instruction[82].binOpcode = bpstrndup("\xFF", 1);
	instruction[82].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[82].opcodeOperandCount = 1;

	// Instruction Struct [83] - CALL
	instruction[83].instructionStr = strdup("CALL");
	instruction[83].undecodedStr = strdup("9A cd");
	instruction[83].dstOperandStr = strdup("ptr16:16");
	instruction[83].dstOperand = IA_PTR16_16;
	instruction[83].valid64 = INVALID;
	instruction[83].compatLeg = VALID;
	instruction[83].VMX = 0;
	instruction[83].description = strdup("Call far, absolute, address given in operand.");
	instruction[83].binPrefixLen = 0;
	instruction[83].binOpcodeLen = 1;
	instruction[83].binOpcode = bpstrndup("\x9A", 1);
	instruction[83].opcodeOperand[0] = OA_CD;
	instruction[83].opcodeOperandCount = 1;

	// Instruction Struct [84] - CALL
	instruction[84].instructionStr = strdup("CALL");
	instruction[84].undecodedStr = strdup("9A cp");
	instruction[84].dstOperandStr = strdup("ptr16:32");
	instruction[84].dstOperand = IA_PTR16_32;
	instruction[84].valid64 = INVALID;
	instruction[84].compatLeg = VALID;
	instruction[84].VMX = 0;
	instruction[84].description = strdup("Call far, absolute, address given in operand.");
	instruction[84].binPrefixLen = 0;
	instruction[84].binOpcodeLen = 1;
	instruction[84].binOpcode = bpstrndup("\x9A", 1);
	instruction[84].opcodeOperand[0] = OA_CP;
	instruction[84].opcodeOperandCount = 1;

	// Instruction Struct [85] - CALL
	instruction[85].instructionStr = strdup("CALL");
	instruction[85].undecodedStr = strdup("FF /3");
	instruction[85].dstOperandStr = strdup("m16:16");
	instruction[85].dstOperand = IA_M16_16;
	instruction[85].valid64 = INVALID;
	instruction[85].compatLeg = VALID;
	instruction[85].VMX = 0;
	instruction[85].description = strdup("Call far, absolute indirect address given in m16:16. In 32-bit mode: if selector points to a gate, then RIP = 32-bit zero extended displacement taken from gate; else RIP = zero extended 16-bit offset from far pointer referenced in the instruction.");
	instruction[85].binPrefixLen = 0;
	instruction[85].binOpcodeLen = 1;
	instruction[85].binOpcode = bpstrndup("\xFF", 1);
	instruction[85].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[85].opcodeOperandCount = 1;

	// Instruction Struct [86] - CALL
	instruction[86].instructionStr = strdup("CALL");
	instruction[86].undecodedStr = strdup("FF /3");
	instruction[86].dstOperandStr = strdup("m16:32");
	instruction[86].dstOperand = IA_M16_32;
	instruction[86].valid64 = INVALID;
	instruction[86].compatLeg = VALID;
	instruction[86].VMX = 0;
	instruction[86].description = strdup("In 64-bit mode: If selector points to a gate, then RIP = 64-bit displacement taken from gate; else RIP = zero extended 32-bit offset from far pointer referenced in the instruction.");
	instruction[86].binPrefixLen = 0;
	instruction[86].binOpcodeLen = 1;
	instruction[86].binOpcode = bpstrndup("\xFF", 1);
	instruction[86].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[86].opcodeOperandCount = 1;

	// Instruction Struct [87] - CBW
	instruction[87].instructionStr = strdup("CBW");
	instruction[87].undecodedStr = strdup("98");
	instruction[87].valid64 = INVALID;
	instruction[87].compatLeg = VALID;
	instruction[87].VMX = 0;
	instruction[87].description = strdup("AX ? sign-extend of AL.");
	instruction[87].binPrefixLen = 0;
	instruction[87].binOpcodeLen = 1;
	instruction[87].binOpcode = bpstrndup("\x98", 1);
	instruction[87].opcodeOperandCount = 0;

	// Instruction Struct [88] - CWDE
	instruction[88].instructionStr = strdup("CWDE");
	instruction[88].undecodedStr = strdup("98");
	instruction[88].valid64 = INVALID;
	instruction[88].compatLeg = VALID;
	instruction[88].VMX = 0;
	instruction[88].description = strdup("EAX ? sign-extend of AX.");
	instruction[88].binPrefixLen = 0;
	instruction[88].binOpcodeLen = 1;
	instruction[88].binOpcode = bpstrndup("\x98", 1);
	instruction[88].opcodeOperandCount = 0;

	// Instruction Struct [89] - CLC
	instruction[89].instructionStr = strdup("CLC");
	instruction[89].undecodedStr = strdup("F8");
	instruction[89].valid64 = INVALID;
	instruction[89].compatLeg = VALID;
	instruction[89].VMX = 0;
	instruction[89].description = strdup("Clear CF flag.");
	instruction[89].binPrefixLen = 0;
	instruction[89].binOpcodeLen = 1;
	instruction[89].binOpcode = bpstrndup("\xF8", 1);
	instruction[89].opcodeOperandCount = 0;

	// Instruction Struct [90] - CLD
	instruction[90].instructionStr = strdup("CLD");
	instruction[90].undecodedStr = strdup("FC");
	instruction[90].valid64 = INVALID;
	instruction[90].compatLeg = VALID;
	instruction[90].VMX = 0;
	instruction[90].description = strdup("Clear DF flag.");
	instruction[90].binPrefixLen = 0;
	instruction[90].binOpcodeLen = 1;
	instruction[90].binOpcode = bpstrndup("\xFC", 1);
	instruction[90].opcodeOperandCount = 0;

	// Instruction Struct [91] - CLFLUSH
	instruction[91].instructionStr = strdup("CLFLUSH");
	instruction[91].undecodedStr = strdup("0F AE /7");
	instruction[91].dstOperandStr = strdup("m8");
	instruction[91].dstOperand = IA_M8;
	instruction[91].valid64 = INVALID;
	instruction[91].compatLeg = VALID;
	instruction[91].VMX = 0;
	instruction[91].description = strdup("Flushes cache line containing m8.");
	instruction[91].binPrefixLen = 0;
	instruction[91].binOpcodeLen = 2;
	instruction[91].binOpcode = bpstrndup("\x0F\xAE", 2);
	instruction[91].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[91].opcodeOperandCount = 1;

	// Instruction Struct [92] - CLI
	instruction[92].instructionStr = strdup("CLI");
	instruction[92].undecodedStr = strdup("FA");
	instruction[92].valid64 = INVALID;
	instruction[92].compatLeg = VALID;
	instruction[92].VMX = 0;
	instruction[92].description = strdup("Clear interrupt flag; interrupts disabled when interrupt flag cleared.");
	instruction[92].binPrefixLen = 0;
	instruction[92].binOpcodeLen = 1;
	instruction[92].binOpcode = bpstrndup("\xFA", 1);
	instruction[92].opcodeOperandCount = 0;

	// Instruction Struct [93] - CLTS
	instruction[93].instructionStr = strdup("CLTS");
	instruction[93].undecodedStr = strdup("0F 06");
	instruction[93].valid64 = INVALID;
	instruction[93].compatLeg = VALID;
	instruction[93].VMX = 0;
	instruction[93].description = strdup("Clears TS flag in CR0.");
	instruction[93].binPrefixLen = 0;
	instruction[93].binOpcodeLen = 2;
	instruction[93].binOpcode = bpstrndup("\x0F\x06", 2);
	instruction[93].opcodeOperandCount = 0;

	// Instruction Struct [94] - CMC
	instruction[94].instructionStr = strdup("CMC");
	instruction[94].undecodedStr = strdup("F5");
	instruction[94].valid64 = INVALID;
	instruction[94].compatLeg = VALID;
	instruction[94].VMX = 0;
	instruction[94].description = strdup("Complement CF flag.");
	instruction[94].binPrefixLen = 0;
	instruction[94].binOpcodeLen = 1;
	instruction[94].binOpcode = bpstrndup("\xF5", 1);
	instruction[94].opcodeOperandCount = 0;

	// Instruction Struct [95] - CMOVA
	instruction[95].instructionStr = strdup("CMOVA");
	instruction[95].undecodedStr = strdup("0F 47 /r");
	instruction[95].dstOperandStr = strdup("r16");
	instruction[95].dstOperand = IA_R16;
	instruction[95].srcOperandStr = strdup("r/m16");
	instruction[95].srcOperand = IA_RM16;
	instruction[95].valid64 = INVALID;
	instruction[95].compatLeg = VALID;
	instruction[95].VMX = 0;
	instruction[95].description = strdup("Move if above (CF=0 and ZF=0).");
	instruction[95].binPrefixLen = 0;
	instruction[95].binOpcodeLen = 2;
	instruction[95].binOpcode = bpstrndup("\x0F\x47", 2);
	instruction[95].opcodeOperand[0] = OA_MODRM;
	instruction[95].opcodeOperandCount = 1;

	// Instruction Struct [96] - CMOVA
	instruction[96].instructionStr = strdup("CMOVA");
	instruction[96].undecodedStr = strdup("0F 47 /r");
	instruction[96].dstOperandStr = strdup("r32");
	instruction[96].dstOperand = IA_R32;
	instruction[96].srcOperandStr = strdup("r/m32");
	instruction[96].srcOperand = IA_RM32;
	instruction[96].valid64 = INVALID;
	instruction[96].compatLeg = VALID;
	instruction[96].VMX = 0;
	instruction[96].description = strdup("Move if above (CF=0 and ZF=0).");
	instruction[96].binPrefixLen = 0;
	instruction[96].binOpcodeLen = 2;
	instruction[96].binOpcode = bpstrndup("\x0F\x47", 2);
	instruction[96].opcodeOperand[0] = OA_MODRM;
	instruction[96].opcodeOperandCount = 1;

	// Instruction Struct [97] - CMOVAE
	instruction[97].instructionStr = strdup("CMOVAE");
	instruction[97].undecodedStr = strdup("0F 43 /r");
	instruction[97].dstOperandStr = strdup("r16");
	instruction[97].dstOperand = IA_R16;
	instruction[97].srcOperandStr = strdup("r/m16");
	instruction[97].srcOperand = IA_RM16;
	instruction[97].valid64 = INVALID;
	instruction[97].compatLeg = VALID;
	instruction[97].VMX = 0;
	instruction[97].description = strdup("Move if above or equal (CF=0).");
	instruction[97].binPrefixLen = 0;
	instruction[97].binOpcodeLen = 2;
	instruction[97].binOpcode = bpstrndup("\x0F\x43", 2);
	instruction[97].opcodeOperand[0] = OA_MODRM;
	instruction[97].opcodeOperandCount = 1;

	// Instruction Struct [98] - CMOVAE
	instruction[98].instructionStr = strdup("CMOVAE");
	instruction[98].undecodedStr = strdup("0F 43 /r");
	instruction[98].dstOperandStr = strdup("r32");
	instruction[98].dstOperand = IA_R32;
	instruction[98].srcOperandStr = strdup("r/m32");
	instruction[98].srcOperand = IA_RM32;
	instruction[98].valid64 = INVALID;
	instruction[98].compatLeg = VALID;
	instruction[98].VMX = 0;
	instruction[98].description = strdup("Move if above or equal (CF=0).");
	instruction[98].binPrefixLen = 0;
	instruction[98].binOpcodeLen = 2;
	instruction[98].binOpcode = bpstrndup("\x0F\x43", 2);
	instruction[98].opcodeOperand[0] = OA_MODRM;
	instruction[98].opcodeOperandCount = 1;

	// Instruction Struct [99] - CMOVB
	instruction[99].instructionStr = strdup("CMOVB");
	instruction[99].undecodedStr = strdup("0F 42 /r");
	instruction[99].dstOperandStr = strdup("r16");
	instruction[99].dstOperand = IA_R16;
	instruction[99].srcOperandStr = strdup("r/m16");
	instruction[99].srcOperand = IA_RM16;
	instruction[99].valid64 = INVALID;
	instruction[99].compatLeg = VALID;
	instruction[99].VMX = 0;
	instruction[99].description = strdup("Move if below (CF=1).");
	instruction[99].binPrefixLen = 0;
	instruction[99].binOpcodeLen = 2;
	instruction[99].binOpcode = bpstrndup("\x0F\x42", 2);
	instruction[99].opcodeOperand[0] = OA_MODRM;
	instruction[99].opcodeOperandCount = 1;

	// Instruction Struct [100] - CMOVB
	instruction[100].instructionStr = strdup("CMOVB");
	instruction[100].undecodedStr = strdup("0F 42 /r");
	instruction[100].dstOperandStr = strdup("r32");
	instruction[100].dstOperand = IA_R32;
	instruction[100].srcOperandStr = strdup("r/m32");
	instruction[100].srcOperand = IA_RM32;
	instruction[100].valid64 = INVALID;
	instruction[100].compatLeg = VALID;
	instruction[100].VMX = 0;
	instruction[100].description = strdup("Move if below (CF=1).");
	instruction[100].binPrefixLen = 0;
	instruction[100].binOpcodeLen = 2;
	instruction[100].binOpcode = bpstrndup("\x0F\x42", 2);
	instruction[100].opcodeOperand[0] = OA_MODRM;
	instruction[100].opcodeOperandCount = 1;

	// Instruction Struct [101] - CMOVBE
	instruction[101].instructionStr = strdup("CMOVBE");
	instruction[101].undecodedStr = strdup("0F 46 /r");
	instruction[101].dstOperandStr = strdup("r16");
	instruction[101].dstOperand = IA_R16;
	instruction[101].srcOperandStr = strdup("r/m16");
	instruction[101].srcOperand = IA_RM16;
	instruction[101].valid64 = INVALID;
	instruction[101].compatLeg = VALID;
	instruction[101].VMX = 0;
	instruction[101].description = strdup("Move if below or equal (CF=1 or ZF=1).");
	instruction[101].binPrefixLen = 0;
	instruction[101].binOpcodeLen = 2;
	instruction[101].binOpcode = bpstrndup("\x0F\x46", 2);
	instruction[101].opcodeOperand[0] = OA_MODRM;
	instruction[101].opcodeOperandCount = 1;

	// Instruction Struct [102] - CMOVBE
	instruction[102].instructionStr = strdup("CMOVBE");
	instruction[102].undecodedStr = strdup("0F 46 /r");
	instruction[102].dstOperandStr = strdup("r32");
	instruction[102].dstOperand = IA_R32;
	instruction[102].srcOperandStr = strdup("r/m32");
	instruction[102].srcOperand = IA_RM32;
	instruction[102].valid64 = INVALID;
	instruction[102].compatLeg = VALID;
	instruction[102].VMX = 0;
	instruction[102].description = strdup("Move if below or equal (CF=1 or ZF=1).");
	instruction[102].binPrefixLen = 0;
	instruction[102].binOpcodeLen = 2;
	instruction[102].binOpcode = bpstrndup("\x0F\x46", 2);
	instruction[102].opcodeOperand[0] = OA_MODRM;
	instruction[102].opcodeOperandCount = 1;

	// Instruction Struct [103] - CMOVC
	instruction[103].instructionStr = strdup("CMOVC");
	instruction[103].undecodedStr = strdup("0F 42 /r");
	instruction[103].dstOperandStr = strdup("r16");
	instruction[103].dstOperand = IA_R16;
	instruction[103].srcOperandStr = strdup("r/m16");
	instruction[103].srcOperand = IA_RM16;
	instruction[103].valid64 = INVALID;
	instruction[103].compatLeg = VALID;
	instruction[103].VMX = 0;
	instruction[103].description = strdup("Move if carry (CF=1).");
	instruction[103].binPrefixLen = 0;
	instruction[103].binOpcodeLen = 2;
	instruction[103].binOpcode = bpstrndup("\x0F\x42", 2);
	instruction[103].opcodeOperand[0] = OA_MODRM;
	instruction[103].opcodeOperandCount = 1;

	// Instruction Struct [104] - CMOVC
	instruction[104].instructionStr = strdup("CMOVC");
	instruction[104].undecodedStr = strdup("0F 42 /r");
	instruction[104].dstOperandStr = strdup("r32");
	instruction[104].dstOperand = IA_R32;
	instruction[104].srcOperandStr = strdup("r/m32");
	instruction[104].srcOperand = IA_RM32;
	instruction[104].valid64 = INVALID;
	instruction[104].compatLeg = VALID;
	instruction[104].VMX = 0;
	instruction[104].description = strdup("Move if carry (CF=1).");
	instruction[104].binPrefixLen = 0;
	instruction[104].binOpcodeLen = 2;
	instruction[104].binOpcode = bpstrndup("\x0F\x42", 2);
	instruction[104].opcodeOperand[0] = OA_MODRM;
	instruction[104].opcodeOperandCount = 1;

	// Instruction Struct [105] - CMOVE
	instruction[105].instructionStr = strdup("CMOVE");
	instruction[105].undecodedStr = strdup("0F 44 /r");
	instruction[105].dstOperandStr = strdup("r16");
	instruction[105].dstOperand = IA_R16;
	instruction[105].srcOperandStr = strdup("r/m16");
	instruction[105].srcOperand = IA_RM16;
	instruction[105].valid64 = INVALID;
	instruction[105].compatLeg = VALID;
	instruction[105].VMX = 0;
	instruction[105].description = strdup("Move if equal (ZF=1).");
	instruction[105].binPrefixLen = 0;
	instruction[105].binOpcodeLen = 2;
	instruction[105].binOpcode = bpstrndup("\x0F\x44", 2);
	instruction[105].opcodeOperand[0] = OA_MODRM;
	instruction[105].opcodeOperandCount = 1;

	// Instruction Struct [106] - CMOVE
	instruction[106].instructionStr = strdup("CMOVE");
	instruction[106].undecodedStr = strdup("0F 44 /r");
	instruction[106].dstOperandStr = strdup("r32");
	instruction[106].dstOperand = IA_R32;
	instruction[106].srcOperandStr = strdup("r/m32");
	instruction[106].srcOperand = IA_RM32;
	instruction[106].valid64 = INVALID;
	instruction[106].compatLeg = VALID;
	instruction[106].VMX = 0;
	instruction[106].description = strdup("Move if equal (ZF=1).");
	instruction[106].binPrefixLen = 0;
	instruction[106].binOpcodeLen = 2;
	instruction[106].binOpcode = bpstrndup("\x0F\x44", 2);
	instruction[106].opcodeOperand[0] = OA_MODRM;
	instruction[106].opcodeOperandCount = 1;

	// Instruction Struct [107] - CMOVG
	instruction[107].instructionStr = strdup("CMOVG");
	instruction[107].undecodedStr = strdup("0F 4F /r");
	instruction[107].dstOperandStr = strdup("r16");
	instruction[107].dstOperand = IA_R16;
	instruction[107].srcOperandStr = strdup("r/m16");
	instruction[107].srcOperand = IA_RM16;
	instruction[107].valid64 = INVALID;
	instruction[107].compatLeg = VALID;
	instruction[107].VMX = 0;
	instruction[107].description = strdup("Move if greater (ZF=0 and SF=OF).");
	instruction[107].binPrefixLen = 0;
	instruction[107].binOpcodeLen = 2;
	instruction[107].binOpcode = bpstrndup("\x0F\x4F", 2);
	instruction[107].opcodeOperand[0] = OA_MODRM;
	instruction[107].opcodeOperandCount = 1;

	// Instruction Struct [108] - CMOVG
	instruction[108].instructionStr = strdup("CMOVG");
	instruction[108].undecodedStr = strdup("0F 4F /r");
	instruction[108].dstOperandStr = strdup("r32");
	instruction[108].dstOperand = IA_R32;
	instruction[108].srcOperandStr = strdup("r/m32");
	instruction[108].srcOperand = IA_RM32;
	instruction[108].valid64 = INVALID;
	instruction[108].compatLeg = VALID;
	instruction[108].VMX = 0;
	instruction[108].description = strdup("Move if greater (ZF=0 and SF=OF).");
	instruction[108].binPrefixLen = 0;
	instruction[108].binOpcodeLen = 2;
	instruction[108].binOpcode = bpstrndup("\x0F\x4F", 2);
	instruction[108].opcodeOperand[0] = OA_MODRM;
	instruction[108].opcodeOperandCount = 1;

	// Instruction Struct [109] - CMOVGE
	instruction[109].instructionStr = strdup("CMOVGE");
	instruction[109].undecodedStr = strdup("0F 4D /r");
	instruction[109].dstOperandStr = strdup("r16");
	instruction[109].dstOperand = IA_R16;
	instruction[109].srcOperandStr = strdup("r/m16");
	instruction[109].srcOperand = IA_RM16;
	instruction[109].valid64 = INVALID;
	instruction[109].compatLeg = VALID;
	instruction[109].VMX = 0;
	instruction[109].description = strdup("Move if greater or equal (SF=OF).");
	instruction[109].binPrefixLen = 0;
	instruction[109].binOpcodeLen = 2;
	instruction[109].binOpcode = bpstrndup("\x0F\x4D", 2);
	instruction[109].opcodeOperand[0] = OA_MODRM;
	instruction[109].opcodeOperandCount = 1;

	// Instruction Struct [110] - CMOVGE
	instruction[110].instructionStr = strdup("CMOVGE");
	instruction[110].undecodedStr = strdup("0F 4D /r");
	instruction[110].dstOperandStr = strdup("r32");
	instruction[110].dstOperand = IA_R32;
	instruction[110].srcOperandStr = strdup("r/m32");
	instruction[110].srcOperand = IA_RM32;
	instruction[110].valid64 = INVALID;
	instruction[110].compatLeg = VALID;
	instruction[110].VMX = 0;
	instruction[110].description = strdup("Move if greater or equal (SF=OF).");
	instruction[110].binPrefixLen = 0;
	instruction[110].binOpcodeLen = 2;
	instruction[110].binOpcode = bpstrndup("\x0F\x4D", 2);
	instruction[110].opcodeOperand[0] = OA_MODRM;
	instruction[110].opcodeOperandCount = 1;

	// Instruction Struct [111] - CMOV
	instruction[111].instructionStr = strdup("CMOV");
	instruction[111].undecodedStr = strdup("0F 4C /r");
	instruction[111].dstOperandStr = strdup("r16");
	instruction[111].dstOperand = IA_R16;
	instruction[111].srcOperandStr = strdup("r/m16");
	instruction[111].srcOperand = IA_RM16;
	instruction[111].valid64 = INVALID;
	instruction[111].compatLeg = VALID;
	instruction[111].VMX = 0;
	instruction[111].description = strdup("Move if less (SF? OF).");
	instruction[111].binPrefixLen = 0;
	instruction[111].binOpcodeLen = 2;
	instruction[111].binOpcode = bpstrndup("\x0F\x4C", 2);
	instruction[111].opcodeOperand[0] = OA_MODRM;
	instruction[111].opcodeOperandCount = 1;

	// Instruction Struct [112] - CMOVL
	instruction[112].instructionStr = strdup("CMOVL");
	instruction[112].undecodedStr = strdup("0F 4C /r");
	instruction[112].dstOperandStr = strdup("r32");
	instruction[112].dstOperand = IA_R32;
	instruction[112].srcOperandStr = strdup("r/m32");
	instruction[112].srcOperand = IA_RM32;
	instruction[112].valid64 = INVALID;
	instruction[112].compatLeg = VALID;
	instruction[112].VMX = 0;
	instruction[112].description = strdup("Move if less (SF? OF).");
	instruction[112].binPrefixLen = 0;
	instruction[112].binOpcodeLen = 2;
	instruction[112].binOpcode = bpstrndup("\x0F\x4C", 2);
	instruction[112].opcodeOperand[0] = OA_MODRM;
	instruction[112].opcodeOperandCount = 1;

	// Instruction Struct [113] - CMOVLE
	instruction[113].instructionStr = strdup("CMOVLE");
	instruction[113].undecodedStr = strdup("0F 4E /r");
	instruction[113].dstOperandStr = strdup("r16");
	instruction[113].dstOperand = IA_R16;
	instruction[113].srcOperandStr = strdup("r/m16");
	instruction[113].srcOperand = IA_RM16;
	instruction[113].valid64 = INVALID;
	instruction[113].compatLeg = VALID;
	instruction[113].VMX = 0;
	instruction[113].description = strdup("Move if less or equal (ZF=1 or SF? OF).");
	instruction[113].binPrefixLen = 0;
	instruction[113].binOpcodeLen = 2;
	instruction[113].binOpcode = bpstrndup("\x0F\x4E", 2);
	instruction[113].opcodeOperand[0] = OA_MODRM;
	instruction[113].opcodeOperandCount = 1;

	// Instruction Struct [114] - CMOVL
	instruction[114].instructionStr = strdup("CMOVL");
	instruction[114].undecodedStr = strdup("0F 4E /r");
	instruction[114].dstOperandStr = strdup("r32");
	instruction[114].dstOperand = IA_R32;
	instruction[114].srcOperandStr = strdup("r/m32");
	instruction[114].srcOperand = IA_RM32;
	instruction[114].valid64 = INVALID;
	instruction[114].compatLeg = VALID;
	instruction[114].VMX = 0;
	instruction[114].description = strdup("Move if less or equal (ZF=1 or SF? OF).");
	instruction[114].binPrefixLen = 0;
	instruction[114].binOpcodeLen = 2;
	instruction[114].binOpcode = bpstrndup("\x0F\x4E", 2);
	instruction[114].opcodeOperand[0] = OA_MODRM;
	instruction[114].opcodeOperandCount = 1;

	// Instruction Struct [115] - CMOVNA
	instruction[115].instructionStr = strdup("CMOVNA");
	instruction[115].undecodedStr = strdup("0F 46 /r");
	instruction[115].dstOperandStr = strdup("r16");
	instruction[115].dstOperand = IA_R16;
	instruction[115].srcOperandStr = strdup("r/m16");
	instruction[115].srcOperand = IA_RM16;
	instruction[115].valid64 = INVALID;
	instruction[115].compatLeg = VALID;
	instruction[115].VMX = 0;
	instruction[115].description = strdup("Move if not above (CF=1 or ZF=1).");
	instruction[115].binPrefixLen = 0;
	instruction[115].binOpcodeLen = 2;
	instruction[115].binOpcode = bpstrndup("\x0F\x46", 2);
	instruction[115].opcodeOperand[0] = OA_MODRM;
	instruction[115].opcodeOperandCount = 1;

	// Instruction Struct [116] - CMOVNA
	instruction[116].instructionStr = strdup("CMOVNA");
	instruction[116].undecodedStr = strdup("0F 46 /r");
	instruction[116].dstOperandStr = strdup("r32");
	instruction[116].dstOperand = IA_R32;
	instruction[116].srcOperandStr = strdup("r/m32");
	instruction[116].srcOperand = IA_RM32;
	instruction[116].valid64 = INVALID;
	instruction[116].compatLeg = VALID;
	instruction[116].VMX = 0;
	instruction[116].description = strdup("Move if not above (CF=1");
	instruction[116].binPrefixLen = 0;
	instruction[116].binOpcodeLen = 2;
	instruction[116].binOpcode = bpstrndup("\x0F\x46", 2);
	instruction[116].opcodeOperand[0] = OA_MODRM;
	instruction[116].opcodeOperandCount = 1;

	// Instruction Struct [117] - CMOVNAE
	instruction[117].instructionStr = strdup("CMOVNAE");
	instruction[117].undecodedStr = strdup("0F 42 /r");
	instruction[117].dstOperandStr = strdup("r16");
	instruction[117].dstOperand = IA_R16;
	instruction[117].srcOperandStr = strdup("r/m16");
	instruction[117].srcOperand = IA_RM16;
	instruction[117].valid64 = INVALID;
	instruction[117].compatLeg = VALID;
	instruction[117].VMX = 0;
	instruction[117].description = strdup("Move if not above or equal (CF=1).");
	instruction[117].binPrefixLen = 0;
	instruction[117].binOpcodeLen = 2;
	instruction[117].binOpcode = bpstrndup("\x0F\x42", 2);
	instruction[117].opcodeOperand[0] = OA_MODRM;
	instruction[117].opcodeOperandCount = 1;

	// Instruction Struct [118] - CMOVNAE
	instruction[118].instructionStr = strdup("CMOVNAE");
	instruction[118].undecodedStr = strdup("0F 42 /r");
	instruction[118].dstOperandStr = strdup("r32");
	instruction[118].dstOperand = IA_R32;
	instruction[118].srcOperandStr = strdup("r/m32");
	instruction[118].srcOperand = IA_RM32;
	instruction[118].valid64 = INVALID;
	instruction[118].compatLeg = VALID;
	instruction[118].VMX = 0;
	instruction[118].description = strdup("Move if not above or equal (CF=1).");
	instruction[118].binPrefixLen = 0;
	instruction[118].binOpcodeLen = 2;
	instruction[118].binOpcode = bpstrndup("\x0F\x42", 2);
	instruction[118].opcodeOperand[0] = OA_MODRM;
	instruction[118].opcodeOperandCount = 1;

	// Instruction Struct [119] - CMOVNB
	instruction[119].instructionStr = strdup("CMOVNB");
	instruction[119].undecodedStr = strdup("0F 43 /r");
	instruction[119].dstOperandStr = strdup("r16");
	instruction[119].dstOperand = IA_R16;
	instruction[119].srcOperandStr = strdup("r/m16");
	instruction[119].srcOperand = IA_RM16;
	instruction[119].valid64 = INVALID;
	instruction[119].compatLeg = VALID;
	instruction[119].VMX = 0;
	instruction[119].description = strdup("Move if not below (CF=0).");
	instruction[119].binPrefixLen = 0;
	instruction[119].binOpcodeLen = 2;
	instruction[119].binOpcode = bpstrndup("\x0F\x43", 2);
	instruction[119].opcodeOperand[0] = OA_MODRM;
	instruction[119].opcodeOperandCount = 1;

	// Instruction Struct [120] - CMOVNB
	instruction[120].instructionStr = strdup("CMOVNB");
	instruction[120].undecodedStr = strdup("0F 43 /r");
	instruction[120].dstOperandStr = strdup("r32");
	instruction[120].dstOperand = IA_R32;
	instruction[120].srcOperandStr = strdup("r/m32");
	instruction[120].srcOperand = IA_RM32;
	instruction[120].valid64 = INVALID;
	instruction[120].compatLeg = VALID;
	instruction[120].VMX = 0;
	instruction[120].description = strdup("Move if not below (CF=0).");
	instruction[120].binPrefixLen = 0;
	instruction[120].binOpcodeLen = 2;
	instruction[120].binOpcode = bpstrndup("\x0F\x43", 2);
	instruction[120].opcodeOperand[0] = OA_MODRM;
	instruction[120].opcodeOperandCount = 1;

	// Instruction Struct [121] - CMOVNBE
	instruction[121].instructionStr = strdup("CMOVNBE");
	instruction[121].undecodedStr = strdup("0F 47 /r");
	instruction[121].dstOperandStr = strdup("r16");
	instruction[121].dstOperand = IA_R16;
	instruction[121].srcOperandStr = strdup("r/m16");
	instruction[121].srcOperand = IA_RM16;
	instruction[121].valid64 = INVALID;
	instruction[121].compatLeg = VALID;
	instruction[121].VMX = 0;
	instruction[121].description = strdup("Move if not below or equal (CF=0 and ZF=0).");
	instruction[121].binPrefixLen = 0;
	instruction[121].binOpcodeLen = 2;
	instruction[121].binOpcode = bpstrndup("\x0F\x47", 2);
	instruction[121].opcodeOperand[0] = OA_MODRM;
	instruction[121].opcodeOperandCount = 1;

	// Instruction Struct [122] - CMOVNBE
	instruction[122].instructionStr = strdup("CMOVNBE");
	instruction[122].undecodedStr = strdup("0F 47 /r");
	instruction[122].dstOperandStr = strdup("r32");
	instruction[122].dstOperand = IA_R32;
	instruction[122].srcOperandStr = strdup("r/m32");
	instruction[122].srcOperand = IA_RM32;
	instruction[122].valid64 = INVALID;
	instruction[122].compatLeg = VALID;
	instruction[122].VMX = 0;
	instruction[122].description = strdup("Move if not below or equal (CF=0 and ZF=0).");
	instruction[122].binPrefixLen = 0;
	instruction[122].binOpcodeLen = 2;
	instruction[122].binOpcode = bpstrndup("\x0F\x47", 2);
	instruction[122].opcodeOperand[0] = OA_MODRM;
	instruction[122].opcodeOperandCount = 1;

	// Instruction Struct [123] - CMOVNC
	instruction[123].instructionStr = strdup("CMOVNC");
	instruction[123].undecodedStr = strdup("0F 43 /r");
	instruction[123].dstOperandStr = strdup("r16");
	instruction[123].dstOperand = IA_R16;
	instruction[123].srcOperandStr = strdup("r/m16");
	instruction[123].srcOperand = IA_RM16;
	instruction[123].valid64 = INVALID;
	instruction[123].compatLeg = VALID;
	instruction[123].VMX = 0;
	instruction[123].description = strdup("Move if not carry (CF=0).");
	instruction[123].binPrefixLen = 0;
	instruction[123].binOpcodeLen = 2;
	instruction[123].binOpcode = bpstrndup("\x0F\x43", 2);
	instruction[123].opcodeOperand[0] = OA_MODRM;
	instruction[123].opcodeOperandCount = 1;

	// Instruction Struct [124] - CMOVNC
	instruction[124].instructionStr = strdup("CMOVNC");
	instruction[124].undecodedStr = strdup("0F 43 /r");
	instruction[124].dstOperandStr = strdup("r32");
	instruction[124].dstOperand = IA_R32;
	instruction[124].srcOperandStr = strdup("r/m32");
	instruction[124].srcOperand = IA_RM32;
	instruction[124].valid64 = INVALID;
	instruction[124].compatLeg = VALID;
	instruction[124].VMX = 0;
	instruction[124].description = strdup("Move if not carry (CF=0).");
	instruction[124].binPrefixLen = 0;
	instruction[124].binOpcodeLen = 2;
	instruction[124].binOpcode = bpstrndup("\x0F\x43", 2);
	instruction[124].opcodeOperand[0] = OA_MODRM;
	instruction[124].opcodeOperandCount = 1;

	// Instruction Struct [125] - CMOVNE
	instruction[125].instructionStr = strdup("CMOVNE");
	instruction[125].undecodedStr = strdup("0F 45 /r");
	instruction[125].dstOperandStr = strdup("r16");
	instruction[125].dstOperand = IA_R16;
	instruction[125].srcOperandStr = strdup("r/m16");
	instruction[125].srcOperand = IA_RM16;
	instruction[125].valid64 = INVALID;
	instruction[125].compatLeg = VALID;
	instruction[125].VMX = 0;
	instruction[125].description = strdup("Move if not equal (ZF=0).");
	instruction[125].binPrefixLen = 0;
	instruction[125].binOpcodeLen = 2;
	instruction[125].binOpcode = bpstrndup("\x0F\x45", 2);
	instruction[125].opcodeOperand[0] = OA_MODRM;
	instruction[125].opcodeOperandCount = 1;

	// Instruction Struct [126] - CMOVNE
	instruction[126].instructionStr = strdup("CMOVNE");
	instruction[126].undecodedStr = strdup("0F 45 /r");
	instruction[126].dstOperandStr = strdup("r32");
	instruction[126].dstOperand = IA_R32;
	instruction[126].srcOperandStr = strdup("r/m32");
	instruction[126].srcOperand = IA_RM32;
	instruction[126].valid64 = INVALID;
	instruction[126].compatLeg = VALID;
	instruction[126].VMX = 0;
	instruction[126].description = strdup("Move if not equal (ZF=0).");
	instruction[126].binPrefixLen = 0;
	instruction[126].binOpcodeLen = 2;
	instruction[126].binOpcode = bpstrndup("\x0F\x45", 2);
	instruction[126].opcodeOperand[0] = OA_MODRM;
	instruction[126].opcodeOperandCount = 1;

	// Instruction Struct [127] - CMOVNG
	instruction[127].instructionStr = strdup("CMOVNG");
	instruction[127].undecodedStr = strdup("0F 4E /r");
	instruction[127].dstOperandStr = strdup("r16");
	instruction[127].dstOperand = IA_R16;
	instruction[127].srcOperandStr = strdup("r/m16");
	instruction[127].srcOperand = IA_RM16;
	instruction[127].valid64 = INVALID;
	instruction[127].compatLeg = VALID;
	instruction[127].VMX = 0;
	instruction[127].description = strdup("Move if not greater (ZF=1 or SF? OF).");
	instruction[127].binPrefixLen = 0;
	instruction[127].binOpcodeLen = 2;
	instruction[127].binOpcode = bpstrndup("\x0F\x4E", 2);
	instruction[127].opcodeOperand[0] = OA_MODRM;
	instruction[127].opcodeOperandCount = 1;

	// Instruction Struct [128] - CMOVNG
	instruction[128].instructionStr = strdup("CMOVNG");
	instruction[128].undecodedStr = strdup("0F 4E /r");
	instruction[128].dstOperandStr = strdup("r32");
	instruction[128].dstOperand = IA_R32;
	instruction[128].srcOperandStr = strdup("r/m32");
	instruction[128].srcOperand = IA_RM32;
	instruction[128].valid64 = INVALID;
	instruction[128].compatLeg = VALID;
	instruction[128].VMX = 0;
	instruction[128].description = strdup("Move if not greater (ZF=1 or SF? OF).");
	instruction[128].binPrefixLen = 0;
	instruction[128].binOpcodeLen = 2;
	instruction[128].binOpcode = bpstrndup("\x0F\x4E", 2);
	instruction[128].opcodeOperand[0] = OA_MODRM;
	instruction[128].opcodeOperandCount = 1;

	// Instruction Struct [129] - CMOVNGE
	instruction[129].instructionStr = strdup("CMOVNGE");
	instruction[129].undecodedStr = strdup("0F 4C /r");
	instruction[129].dstOperandStr = strdup("r16");
	instruction[129].dstOperand = IA_R16;
	instruction[129].srcOperandStr = strdup("r/m16");
	instruction[129].srcOperand = IA_RM16;
	instruction[129].valid64 = INVALID;
	instruction[129].compatLeg = VALID;
	instruction[129].VMX = 0;
	instruction[129].description = strdup("Move if not greater or equal (SF? OF).");
	instruction[129].binPrefixLen = 0;
	instruction[129].binOpcodeLen = 2;
	instruction[129].binOpcode = bpstrndup("\x0F\x4C", 2);
	instruction[129].opcodeOperand[0] = OA_MODRM;
	instruction[129].opcodeOperandCount = 1;

	// Instruction Struct [130] - CMOVNGE
	instruction[130].instructionStr = strdup("CMOVNGE");
	instruction[130].undecodedStr = strdup("0F 4C /r");
	instruction[130].dstOperandStr = strdup("r32");
	instruction[130].dstOperand = IA_R32;
	instruction[130].srcOperandStr = strdup("r/m32");
	instruction[130].srcOperand = IA_RM32;
	instruction[130].valid64 = INVALID;
	instruction[130].compatLeg = VALID;
	instruction[130].VMX = 0;
	instruction[130].description = strdup("Move if not greater or equal (SF? OF).");
	instruction[130].binPrefixLen = 0;
	instruction[130].binOpcodeLen = 2;
	instruction[130].binOpcode = bpstrndup("\x0F\x4C", 2);
	instruction[130].opcodeOperand[0] = OA_MODRM;
	instruction[130].opcodeOperandCount = 1;

	// Instruction Struct [131] - CMOVNL
	instruction[131].instructionStr = strdup("CMOVNL");
	instruction[131].undecodedStr = strdup("0F 4D /r");
	instruction[131].dstOperandStr = strdup("r16");
	instruction[131].dstOperand = IA_R16;
	instruction[131].srcOperandStr = strdup("r/m16");
	instruction[131].srcOperand = IA_RM16;
	instruction[131].valid64 = INVALID;
	instruction[131].compatLeg = VALID;
	instruction[131].VMX = 0;
	instruction[131].description = strdup("Move if not less (SF=OF).");
	instruction[131].binPrefixLen = 0;
	instruction[131].binOpcodeLen = 2;
	instruction[131].binOpcode = bpstrndup("\x0F\x4D", 2);
	instruction[131].opcodeOperand[0] = OA_MODRM;
	instruction[131].opcodeOperandCount = 1;

	// Instruction Struct [132] - CMOVNL
	instruction[132].instructionStr = strdup("CMOVNL");
	instruction[132].undecodedStr = strdup("0F 4D /r");
	instruction[132].dstOperandStr = strdup("r32");
	instruction[132].dstOperand = IA_R32;
	instruction[132].srcOperandStr = strdup("r/m32");
	instruction[132].srcOperand = IA_RM32;
	instruction[132].valid64 = INVALID;
	instruction[132].compatLeg = VALID;
	instruction[132].VMX = 0;
	instruction[132].description = strdup("Move if not less (SF=OF).");
	instruction[132].binPrefixLen = 0;
	instruction[132].binOpcodeLen = 2;
	instruction[132].binOpcode = bpstrndup("\x0F\x4D", 2);
	instruction[132].opcodeOperand[0] = OA_MODRM;
	instruction[132].opcodeOperandCount = 1;

	// Instruction Struct [133] - CMOVNLE
	instruction[133].instructionStr = strdup("CMOVNLE");
	instruction[133].undecodedStr = strdup("0F 4F /r");
	instruction[133].dstOperandStr = strdup("r16");
	instruction[133].dstOperand = IA_R16;
	instruction[133].srcOperandStr = strdup("r/m16");
	instruction[133].srcOperand = IA_RM16;
	instruction[133].valid64 = INVALID;
	instruction[133].compatLeg = VALID;
	instruction[133].VMX = 0;
	instruction[133].description = strdup("Move if not less or equal (ZF=0 and SF=OF).");
	instruction[133].binPrefixLen = 0;
	instruction[133].binOpcodeLen = 2;
	instruction[133].binOpcode = bpstrndup("\x0F\x4F", 2);
	instruction[133].opcodeOperand[0] = OA_MODRM;
	instruction[133].opcodeOperandCount = 1;

	// Instruction Struct [134] - CMOVNLE
	instruction[134].instructionStr = strdup("CMOVNLE");
	instruction[134].undecodedStr = strdup("0F 4F /r");
	instruction[134].dstOperandStr = strdup("r32");
	instruction[134].dstOperand = IA_R32;
	instruction[134].srcOperandStr = strdup("r/m32");
	instruction[134].srcOperand = IA_RM32;
	instruction[134].valid64 = INVALID;
	instruction[134].compatLeg = VALID;
	instruction[134].VMX = 0;
	instruction[134].description = strdup("Move if not less or equal (ZF=0 and SF=OF).");
	instruction[134].binPrefixLen = 0;
	instruction[134].binOpcodeLen = 2;
	instruction[134].binOpcode = bpstrndup("\x0F\x4F", 2);
	instruction[134].opcodeOperand[0] = OA_MODRM;
	instruction[134].opcodeOperandCount = 1;

	// Instruction Struct [135] - CMOVNO
	instruction[135].instructionStr = strdup("CMOVNO");
	instruction[135].undecodedStr = strdup("0F 41 /r");
	instruction[135].dstOperandStr = strdup("r16");
	instruction[135].dstOperand = IA_R16;
	instruction[135].srcOperandStr = strdup("r/m16");
	instruction[135].srcOperand = IA_RM16;
	instruction[135].valid64 = INVALID;
	instruction[135].compatLeg = VALID;
	instruction[135].VMX = 0;
	instruction[135].description = strdup("Move if not overflow (OF=0).");
	instruction[135].binPrefixLen = 0;
	instruction[135].binOpcodeLen = 2;
	instruction[135].binOpcode = bpstrndup("\x0F\x41", 2);
	instruction[135].opcodeOperand[0] = OA_MODRM;
	instruction[135].opcodeOperandCount = 1;

	// Instruction Struct [136] - CMOVNO
	instruction[136].instructionStr = strdup("CMOVNO");
	instruction[136].undecodedStr = strdup("0F 41 /r");
	instruction[136].dstOperandStr = strdup("r32");
	instruction[136].dstOperand = IA_R32;
	instruction[136].srcOperandStr = strdup("r/m32");
	instruction[136].srcOperand = IA_RM32;
	instruction[136].valid64 = INVALID;
	instruction[136].compatLeg = VALID;
	instruction[136].VMX = 0;
	instruction[136].description = strdup("Move if not overflow (OF=0).");
	instruction[136].binPrefixLen = 0;
	instruction[136].binOpcodeLen = 2;
	instruction[136].binOpcode = bpstrndup("\x0F\x41", 2);
	instruction[136].opcodeOperand[0] = OA_MODRM;
	instruction[136].opcodeOperandCount = 1;

	// Instruction Struct [137] - CMOVNP
	instruction[137].instructionStr = strdup("CMOVNP");
	instruction[137].undecodedStr = strdup("0F 4B /r");
	instruction[137].dstOperandStr = strdup("r16");
	instruction[137].dstOperand = IA_R16;
	instruction[137].srcOperandStr = strdup("r/m16");
	instruction[137].srcOperand = IA_RM16;
	instruction[137].valid64 = INVALID;
	instruction[137].compatLeg = VALID;
	instruction[137].VMX = 0;
	instruction[137].description = strdup("Move if not parity (PF=0).");
	instruction[137].binPrefixLen = 0;
	instruction[137].binOpcodeLen = 2;
	instruction[137].binOpcode = bpstrndup("\x0F\x4B", 2);
	instruction[137].opcodeOperand[0] = OA_MODRM;
	instruction[137].opcodeOperandCount = 1;

	// Instruction Struct [138] - CMOVNP
	instruction[138].instructionStr = strdup("CMOVNP");
	instruction[138].undecodedStr = strdup("0F 4B /r");
	instruction[138].dstOperandStr = strdup("r32");
	instruction[138].dstOperand = IA_R32;
	instruction[138].srcOperandStr = strdup("r/m32");
	instruction[138].srcOperand = IA_RM32;
	instruction[138].valid64 = INVALID;
	instruction[138].compatLeg = VALID;
	instruction[138].VMX = 0;
	instruction[138].description = strdup("Move if not parity (PF=0).");
	instruction[138].binPrefixLen = 0;
	instruction[138].binOpcodeLen = 2;
	instruction[138].binOpcode = bpstrndup("\x0F\x4B", 2);
	instruction[138].opcodeOperand[0] = OA_MODRM;
	instruction[138].opcodeOperandCount = 1;

	// Instruction Struct [139] - CMOVNS
	instruction[139].instructionStr = strdup("CMOVNS");
	instruction[139].undecodedStr = strdup("0F 49 /r");
	instruction[139].dstOperandStr = strdup("r16");
	instruction[139].dstOperand = IA_R16;
	instruction[139].srcOperandStr = strdup("r/m16");
	instruction[139].srcOperand = IA_RM16;
	instruction[139].valid64 = INVALID;
	instruction[139].compatLeg = VALID;
	instruction[139].VMX = 0;
	instruction[139].description = strdup("Move if not sign (SF=0).");
	instruction[139].binPrefixLen = 0;
	instruction[139].binOpcodeLen = 2;
	instruction[139].binOpcode = bpstrndup("\x0F\x49", 2);
	instruction[139].opcodeOperand[0] = OA_MODRM;
	instruction[139].opcodeOperandCount = 1;

	// Instruction Struct [140] - CMOVNS
	instruction[140].instructionStr = strdup("CMOVNS");
	instruction[140].undecodedStr = strdup("0F 49 /r");
	instruction[140].dstOperandStr = strdup("r32");
	instruction[140].dstOperand = IA_R32;
	instruction[140].srcOperandStr = strdup("r/m32");
	instruction[140].srcOperand = IA_RM32;
	instruction[140].valid64 = INVALID;
	instruction[140].compatLeg = VALID;
	instruction[140].VMX = 0;
	instruction[140].description = strdup("Move if not sign (SF=0).Move if not sign (SF=0).");
	instruction[140].binPrefixLen = 0;
	instruction[140].binOpcodeLen = 2;
	instruction[140].binOpcode = bpstrndup("\x0F\x49", 2);
	instruction[140].opcodeOperand[0] = OA_MODRM;
	instruction[140].opcodeOperandCount = 1;

	// Instruction Struct [141] - CMOVNZ
	instruction[141].instructionStr = strdup("CMOVNZ");
	instruction[141].undecodedStr = strdup("0F 45 /r");
	instruction[141].dstOperandStr = strdup("r16");
	instruction[141].dstOperand = IA_R16;
	instruction[141].srcOperandStr = strdup("r/m16");
	instruction[141].srcOperand = IA_RM16;
	instruction[141].valid64 = INVALID;
	instruction[141].compatLeg = VALID;
	instruction[141].VMX = 0;
	instruction[141].description = strdup("Move if not zero (ZF=0).");
	instruction[141].binPrefixLen = 0;
	instruction[141].binOpcodeLen = 2;
	instruction[141].binOpcode = bpstrndup("\x0F\x45", 2);
	instruction[141].opcodeOperand[0] = OA_MODRM;
	instruction[141].opcodeOperandCount = 1;

	// Instruction Struct [142] - CMOVNZ
	instruction[142].instructionStr = strdup("CMOVNZ");
	instruction[142].undecodedStr = strdup("0F 45 /r");
	instruction[142].dstOperandStr = strdup("r32");
	instruction[142].dstOperand = IA_R32;
	instruction[142].srcOperandStr = strdup("r/m32");
	instruction[142].srcOperand = IA_RM32;
	instruction[142].valid64 = INVALID;
	instruction[142].compatLeg = VALID;
	instruction[142].VMX = 0;
	instruction[142].description = strdup("Move if not zero (ZF=0).");
	instruction[142].binPrefixLen = 0;
	instruction[142].binOpcodeLen = 2;
	instruction[142].binOpcode = bpstrndup("\x0F\x45", 2);
	instruction[142].opcodeOperand[0] = OA_MODRM;
	instruction[142].opcodeOperandCount = 1;

	// Instruction Struct [143] - CMOVO
	instruction[143].instructionStr = strdup("CMOVO");
	instruction[143].undecodedStr = strdup("0F 40 /r");
	instruction[143].dstOperandStr = strdup("r16");
	instruction[143].dstOperand = IA_R16;
	instruction[143].srcOperandStr = strdup("r/m16");
	instruction[143].srcOperand = IA_RM16;
	instruction[143].valid64 = INVALID;
	instruction[143].compatLeg = VALID;
	instruction[143].VMX = 0;
	instruction[143].description = strdup("Move if overflow (OF=0).");
	instruction[143].binPrefixLen = 0;
	instruction[143].binOpcodeLen = 2;
	instruction[143].binOpcode = bpstrndup("\x0F\x40", 2);
	instruction[143].opcodeOperand[0] = OA_MODRM;
	instruction[143].opcodeOperandCount = 1;

	// Instruction Struct [144] - CMOVO
	instruction[144].instructionStr = strdup("CMOVO");
	instruction[144].undecodedStr = strdup("0F 40 /r");
	instruction[144].dstOperandStr = strdup("r32");
	instruction[144].dstOperand = IA_R32;
	instruction[144].srcOperandStr = strdup("r/m32");
	instruction[144].srcOperand = IA_RM32;
	instruction[144].valid64 = INVALID;
	instruction[144].compatLeg = VALID;
	instruction[144].VMX = 0;
	instruction[144].description = strdup("Move if overflow (OF=0).");
	instruction[144].binPrefixLen = 0;
	instruction[144].binOpcodeLen = 2;
	instruction[144].binOpcode = bpstrndup("\x0F\x40", 2);
	instruction[144].opcodeOperand[0] = OA_MODRM;
	instruction[144].opcodeOperandCount = 1;

	// Instruction Struct [145] - CMOVP
	instruction[145].instructionStr = strdup("CMOVP");
	instruction[145].undecodedStr = strdup("0F 4A /r");
	instruction[145].dstOperandStr = strdup("r16");
	instruction[145].dstOperand = IA_R16;
	instruction[145].srcOperandStr = strdup("r/m16");
	instruction[145].srcOperand = IA_RM16;
	instruction[145].valid64 = INVALID;
	instruction[145].compatLeg = VALID;
	instruction[145].VMX = 0;
	instruction[145].description = strdup("Move if parity (PF=1).");
	instruction[145].binPrefixLen = 0;
	instruction[145].binOpcodeLen = 2;
	instruction[145].binOpcode = bpstrndup("\x0F\x4A", 2);
	instruction[145].opcodeOperand[0] = OA_MODRM;
	instruction[145].opcodeOperandCount = 1;

	// Instruction Struct [146] - CMOVP
	instruction[146].instructionStr = strdup("CMOVP");
	instruction[146].undecodedStr = strdup("0F 4A /r");
	instruction[146].dstOperandStr = strdup("r32");
	instruction[146].dstOperand = IA_R32;
	instruction[146].srcOperandStr = strdup("r/m32");
	instruction[146].srcOperand = IA_RM32;
	instruction[146].valid64 = INVALID;
	instruction[146].compatLeg = VALID;
	instruction[146].VMX = 0;
	instruction[146].description = strdup("Move if parity (PF=1).");
	instruction[146].binPrefixLen = 0;
	instruction[146].binOpcodeLen = 2;
	instruction[146].binOpcode = bpstrndup("\x0F\x4A", 2);
	instruction[146].opcodeOperand[0] = OA_MODRM;
	instruction[146].opcodeOperandCount = 1;

	// Instruction Struct [147] - CMOVPE
	instruction[147].instructionStr = strdup("CMOVPE");
	instruction[147].undecodedStr = strdup("0F 4A /r");
	instruction[147].dstOperandStr = strdup("r16");
	instruction[147].dstOperand = IA_R16;
	instruction[147].srcOperandStr = strdup("r/m16");
	instruction[147].srcOperand = IA_RM16;
	instruction[147].valid64 = INVALID;
	instruction[147].compatLeg = VALID;
	instruction[147].VMX = 0;
	instruction[147].description = strdup("Move if parity even (PF=1).");
	instruction[147].binPrefixLen = 0;
	instruction[147].binOpcodeLen = 2;
	instruction[147].binOpcode = bpstrndup("\x0F\x4A", 2);
	instruction[147].opcodeOperand[0] = OA_MODRM;
	instruction[147].opcodeOperandCount = 1;

	// Instruction Struct [148] - CMOVPE
	instruction[148].instructionStr = strdup("CMOVPE");
	instruction[148].undecodedStr = strdup("0F 4A /r");
	instruction[148].dstOperandStr = strdup("r32");
	instruction[148].dstOperand = IA_R32;
	instruction[148].srcOperandStr = strdup("r/m32");
	instruction[148].srcOperand = IA_RM32;
	instruction[148].valid64 = INVALID;
	instruction[148].compatLeg = VALID;
	instruction[148].VMX = 0;
	instruction[148].description = strdup("Move if parity even (PF=1).");
	instruction[148].binPrefixLen = 0;
	instruction[148].binOpcodeLen = 2;
	instruction[148].binOpcode = bpstrndup("\x0F\x4A", 2);
	instruction[148].opcodeOperand[0] = OA_MODRM;
	instruction[148].opcodeOperandCount = 1;

	// Instruction Struct [149] - CMOVPO
	instruction[149].instructionStr = strdup("CMOVPO");
	instruction[149].undecodedStr = strdup("0F 4B /r");
	instruction[149].dstOperandStr = strdup("r16");
	instruction[149].dstOperand = IA_R16;
	instruction[149].srcOperandStr = strdup("r/m16");
	instruction[149].srcOperand = IA_RM16;
	instruction[149].valid64 = INVALID;
	instruction[149].compatLeg = VALID;
	instruction[149].VMX = 0;
	instruction[149].description = strdup("Move if parity odd (PF=0).");
	instruction[149].binPrefixLen = 0;
	instruction[149].binOpcodeLen = 2;
	instruction[149].binOpcode = bpstrndup("\x0F\x4B", 2);
	instruction[149].opcodeOperand[0] = OA_MODRM;
	instruction[149].opcodeOperandCount = 1;

	// Instruction Struct [150] - CMOVPO
	instruction[150].instructionStr = strdup("CMOVPO");
	instruction[150].undecodedStr = strdup("0F 4B /r");
	instruction[150].dstOperandStr = strdup("r32");
	instruction[150].dstOperand = IA_R32;
	instruction[150].srcOperandStr = strdup("r/m32");
	instruction[150].srcOperand = IA_RM32;
	instruction[150].valid64 = INVALID;
	instruction[150].compatLeg = VALID;
	instruction[150].VMX = 0;
	instruction[150].description = strdup("Move if parity odd (PF=0).");
	instruction[150].binPrefixLen = 0;
	instruction[150].binOpcodeLen = 2;
	instruction[150].binOpcode = bpstrndup("\x0F\x4B", 2);
	instruction[150].opcodeOperand[0] = OA_MODRM;
	instruction[150].opcodeOperandCount = 1;

	// Instruction Struct [151] - CMOVS
	instruction[151].instructionStr = strdup("CMOVS");
	instruction[151].undecodedStr = strdup("0F 48 /r");
	instruction[151].dstOperandStr = strdup("r16");
	instruction[151].dstOperand = IA_R16;
	instruction[151].srcOperandStr = strdup("r/m16");
	instruction[151].srcOperand = IA_RM16;
	instruction[151].valid64 = INVALID;
	instruction[151].compatLeg = VALID;
	instruction[151].VMX = 0;
	instruction[151].description = strdup("Move if sign (SF=1).");
	instruction[151].binPrefixLen = 0;
	instruction[151].binOpcodeLen = 2;
	instruction[151].binOpcode = bpstrndup("\x0F\x48", 2);
	instruction[151].opcodeOperand[0] = OA_MODRM;
	instruction[151].opcodeOperandCount = 1;

	// Instruction Struct [152] - CMOVS
	instruction[152].instructionStr = strdup("CMOVS");
	instruction[152].undecodedStr = strdup("0F 48 /r");
	instruction[152].dstOperandStr = strdup("r32");
	instruction[152].dstOperand = IA_R32;
	instruction[152].srcOperandStr = strdup("r/m32");
	instruction[152].srcOperand = IA_RM32;
	instruction[152].valid64 = INVALID;
	instruction[152].compatLeg = VALID;
	instruction[152].VMX = 0;
	instruction[152].description = strdup("Move if sign (SF=1).");
	instruction[152].binPrefixLen = 0;
	instruction[152].binOpcodeLen = 2;
	instruction[152].binOpcode = bpstrndup("\x0F\x48", 2);
	instruction[152].opcodeOperand[0] = OA_MODRM;
	instruction[152].opcodeOperandCount = 1;

	// Instruction Struct [153] - CMOVZ
	instruction[153].instructionStr = strdup("CMOVZ");
	instruction[153].undecodedStr = strdup("0F 44 /r");
	instruction[153].dstOperandStr = strdup("r16");
	instruction[153].dstOperand = IA_R16;
	instruction[153].srcOperandStr = strdup("r/m16");
	instruction[153].srcOperand = IA_RM16;
	instruction[153].valid64 = INVALID;
	instruction[153].compatLeg = VALID;
	instruction[153].VMX = 0;
	instruction[153].description = strdup("Move if zero (ZF=1).");
	instruction[153].binPrefixLen = 0;
	instruction[153].binOpcodeLen = 2;
	instruction[153].binOpcode = bpstrndup("\x0F\x44", 2);
	instruction[153].opcodeOperand[0] = OA_MODRM;
	instruction[153].opcodeOperandCount = 1;

	// Instruction Struct [154] - CMOVZ
	instruction[154].instructionStr = strdup("CMOVZ");
	instruction[154].undecodedStr = strdup("0F 44 /r");
	instruction[154].dstOperandStr = strdup("r32");
	instruction[154].dstOperand = IA_R32;
	instruction[154].srcOperandStr = strdup("r/m32");
	instruction[154].srcOperand = IA_RM32;
	instruction[154].valid64 = INVALID;
	instruction[154].compatLeg = VALID;
	instruction[154].VMX = 0;
	instruction[154].description = strdup("Move if zero (ZF=1).");
	instruction[154].binPrefixLen = 0;
	instruction[154].binOpcodeLen = 2;
	instruction[154].binOpcode = bpstrndup("\x0F\x44", 2);
	instruction[154].opcodeOperand[0] = OA_MODRM;
	instruction[154].opcodeOperandCount = 1;

	// Instruction Struct [155] - CMPPD
	instruction[155].instructionStr = strdup("CMPPD");
	instruction[155].undecodedStr = strdup("66 0F C2 /r ib");
	instruction[155].dstOperandStr = strdup("xmm1");
	instruction[155].dstOperand = IA_XMM;
	instruction[155].srcOperandStr = strdup("xmm2/m128");
	instruction[155].srcOperand = IA_XMM_M128;
	instruction[155].thirdOperandStr = strdup("imm8");
	instruction[155].thirdOperand = IA_IMM8;
	instruction[155].valid64 = INVALID;
	instruction[155].compatLeg = VALID;
	instruction[155].VMX = 0;
	instruction[155].description = strdup("Compare packed doubleprecision floating-point values in xmm2/m128 and xmm1 using imm8 as comparison predicate.");
	instruction[155].prefixStr = strdup("OPSIZEOVR");
	instruction[155].binPrefixLen = 1;
	instruction[155].binPrefix = bpstrndup("\x66", 1);
	instruction[155].binOpcodeLen = 2;
	instruction[155].binOpcode = bpstrndup("\x0F\xC2", 2);
	instruction[155].opcodeOperand[0] = OA_MODRM;
	instruction[155].opcodeOperand[1] = OA_IB;
	instruction[155].opcodeOperandCount = 2;

	// Instruction Struct [156] - CMPPS
	instruction[156].instructionStr = strdup("CMPPS");
	instruction[156].undecodedStr = strdup("0F C2 /r ib");
	instruction[156].dstOperandStr = strdup("xmm1");
	instruction[156].dstOperand = IA_XMM;
	instruction[156].srcOperandStr = strdup("xmm2/m128");
	instruction[156].srcOperand = IA_XMM_M128;
	instruction[156].thirdOperandStr = strdup("imm8");
	instruction[156].thirdOperand = IA_IMM8;
	instruction[156].valid64 = INVALID;
	instruction[156].compatLeg = VALID;
	instruction[156].VMX = 0;
	instruction[156].description = strdup("Compare packed singleprecision floating-point values in xmm2/mem and xmm1 using imm8 as comparison predicate.");
	instruction[156].binPrefixLen = 0;
	instruction[156].binOpcodeLen = 2;
	instruction[156].binOpcode = bpstrndup("\x0F\xC2", 2);
	instruction[156].opcodeOperand[0] = OA_MODRM;
	instruction[156].opcodeOperand[1] = OA_IB;
	instruction[156].opcodeOperandCount = 2;

	// Instruction Struct [157] - CMPS
	instruction[157].instructionStr = strdup("CMPS");
	instruction[157].undecodedStr = strdup("A6");
	instruction[157].dstOperandStr = strdup("m8");
	instruction[157].dstOperand = IA_M8;
	instruction[157].srcOperandStr = strdup("m8");
	instruction[157].srcOperand = IA_M8;
	instruction[157].valid64 = INVALID;
	instruction[157].compatLeg = VALID;
	instruction[157].VMX = 0;
	instruction[157].description = strdup("For legacy mode, compare byte at address DS:(E)SI with byte at address ES:(E)DI; For 64-bit mode compare byte at address (R|E)SI to byte at address (R|E)DI. The status flags are set accordingly.");
	instruction[157].binPrefixLen = 0;
	instruction[157].binOpcodeLen = 1;
	instruction[157].binOpcode = bpstrndup("\xA6", 1);
	instruction[157].opcodeOperandCount = 0;

	// Instruction Struct [158] - CMPS
	instruction[158].instructionStr = strdup("CMPS");
	instruction[158].undecodedStr = strdup("A7");
	instruction[158].dstOperandStr = strdup("m16");
	instruction[158].dstOperand = IA_M16;
	instruction[158].srcOperandStr = strdup("m16");
	instruction[158].srcOperand = IA_M16;
	instruction[158].valid64 = INVALID;
	instruction[158].compatLeg = VALID;
	instruction[158].VMX = 0;
	instruction[158].description = strdup("For legacy mode, compare word at address DS:(E)SI with word at address ES:(E)DI; For 64-bit mode compare word at address (R|E)SI with word at address (R|E)DI. The status flags are set accordingly.");
	instruction[158].binPrefixLen = 0;
	instruction[158].binOpcodeLen = 1;
	instruction[158].binOpcode = bpstrndup("\xA7", 1);
	instruction[158].opcodeOperandCount = 0;

	// Instruction Struct [159] - CMPS
	instruction[159].instructionStr = strdup("CMPS");
	instruction[159].undecodedStr = strdup("A7");
	instruction[159].dstOperandStr = strdup("m32");
	instruction[159].dstOperand = IA_M32;
	instruction[159].srcOperandStr = strdup("m32");
	instruction[159].srcOperand = IA_M32;
	instruction[159].valid64 = INVALID;
	instruction[159].compatLeg = VALID;
	instruction[159].VMX = 0;
	instruction[159].description = strdup("For legacy mode, compare dword at address DS:(E)SI at dword at address ES:(E)DI; For 64-bit mode compare dword at address (R|E)SI at dword at address (R|E)DI. The status flags are set accordingly.");
	instruction[159].binPrefixLen = 0;
	instruction[159].binOpcodeLen = 1;
	instruction[159].binOpcode = bpstrndup("\xA7", 1);
	instruction[159].opcodeOperandCount = 0;

	// Instruction Struct [160] - CMPSB
	instruction[160].instructionStr = strdup("CMPSB");
	instruction[160].undecodedStr = strdup("A6");
	instruction[160].valid64 = INVALID;
	instruction[160].compatLeg = VALID;
	instruction[160].VMX = 0;
	instruction[160].description = strdup("For legacy mode, compare byte at address DS:(E)SI with byte at address ES:(E)DI; For 64-bit mode compare byte at address (R|E)SI with byte at address (R|E)DI. The status flags are set accordingly.");
	instruction[160].binPrefixLen = 0;
	instruction[160].binOpcodeLen = 1;
	instruction[160].binOpcode = bpstrndup("\xA6", 1);
	instruction[160].opcodeOperandCount = 0;

	// Instruction Struct [161] - CMPSW
	instruction[161].instructionStr = strdup("CMPSW");
	instruction[161].undecodedStr = strdup("A7");
	instruction[161].valid64 = INVALID;
	instruction[161].compatLeg = VALID;
	instruction[161].VMX = 0;
	instruction[161].description = strdup("For legacy mode, compare word at address DS:(E)SI with word at address ES:(E)DI; For 64-bit mode compare word at address (R|E)SI with word at address (R|E)DI. The status flags are set accordingly.");
	instruction[161].binPrefixLen = 0;
	instruction[161].binOpcodeLen = 1;
	instruction[161].binOpcode = bpstrndup("\xA7", 1);
	instruction[161].opcodeOperandCount = 0;

	// Instruction Struct [162] - CMPSD
	instruction[162].instructionStr = strdup("CMPSD");
	instruction[162].undecodedStr = strdup("A7");
	instruction[162].valid64 = INVALID;
	instruction[162].compatLeg = VALID;
	instruction[162].VMX = 0;
	instruction[162].description = strdup("For legacy mode, compare dword at address DS:(E)SI with dword at address ES:(E)DI; For 64-bit mode compare dword at address (R|E)SI with dword at address (R|E)DI. The status flags are set accordingly.");
	instruction[162].binPrefixLen = 0;
	instruction[162].binOpcodeLen = 1;
	instruction[162].binOpcode = bpstrndup("\xA7", 1);
	instruction[162].opcodeOperandCount = 0;

	// Instruction Struct [163] - CMPSD
	instruction[163].instructionStr = strdup("CMPSD");
	instruction[163].undecodedStr = strdup("F2 0F C2 /r ib");
	instruction[163].dstOperandStr = strdup("xmm1");
	instruction[163].dstOperand = IA_XMM;
	instruction[163].srcOperandStr = strdup("xmm2/m64");
	instruction[163].srcOperand = IA_XMM_M64;
	instruction[163].thirdOperandStr = strdup("imm8");
	instruction[163].thirdOperand = IA_IMM8;
	instruction[163].valid64 = INVALID;
	instruction[163].compatLeg = VALID;
	instruction[163].VMX = 0;
	instruction[163].description = strdup("Compare low doubleprecision floating-point value in xmm2/m64 and xmm1 using imm8 as comparison predicate.");
	instruction[163].prefixStr = strdup("REPNE");
	instruction[163].binPrefixLen = 1;
	instruction[163].binPrefix = bpstrndup("\xF2", 1);
	instruction[163].binOpcodeLen = 2;
	instruction[163].binOpcode = bpstrndup("\x0F\xC2", 2);
	instruction[163].opcodeOperand[0] = OA_MODRM;
	instruction[163].opcodeOperand[1] = OA_IB;
	instruction[163].opcodeOperandCount = 2;

	// Instruction Struct [164] - CMPSS
	instruction[164].instructionStr = strdup("CMPSS");
	instruction[164].undecodedStr = strdup("F3 0F C2 /r ib");
	instruction[164].dstOperandStr = strdup("xmm1");
	instruction[164].dstOperand = IA_XMM;
	instruction[164].srcOperandStr = strdup("xmm2/m32");
	instruction[164].srcOperand = IA_XMM_M32;
	instruction[164].thirdOperandStr = strdup("imm8");
	instruction[164].thirdOperand = IA_IMM8;
	instruction[164].valid64 = INVALID;
	instruction[164].compatLeg = VALID;
	instruction[164].VMX = 0;
	instruction[164].description = strdup("Compare low single-precision floating-point value in xmm2/m32 and xmm1 using imm8 as comparison predicate.");
	instruction[164].prefixStr = strdup("REP");
	instruction[164].binPrefixLen = 1;
	instruction[164].binPrefix = bpstrndup("\xF3", 1);
	instruction[164].binOpcodeLen = 2;
	instruction[164].binOpcode = bpstrndup("\x0F\xC2", 2);
	instruction[164].opcodeOperand[0] = OA_MODRM;
	instruction[164].opcodeOperand[1] = OA_IB;
	instruction[164].opcodeOperandCount = 2;

	// Instruction Struct [165] - CMPXCHG
	instruction[165].instructionStr = strdup("CMPXCHG");
	instruction[165].undecodedStr = strdup("0F B0 /r");
	instruction[165].dstOperandStr = strdup("r/m8");
	instruction[165].dstOperand = IA_RM8;
	instruction[165].srcOperandStr = strdup("r8");
	instruction[165].srcOperand = IA_R8;
	instruction[165].valid64 = INVALID;
	instruction[165].compatLeg = VALID;
	instruction[165].VMX = 0;
	instruction[165].description = strdup("Compare AL with r/m8. If");
	instruction[165].binPrefixLen = 0;
	instruction[165].binOpcodeLen = 2;
	instruction[165].binOpcode = bpstrndup("\x0F\xB0", 2);
	instruction[165].opcodeOperand[0] = OA_MODRM;
	instruction[165].opcodeOperandCount = 1;

	// Instruction Struct [166] - CMP
	instruction[166].instructionStr = strdup("CMP");
	instruction[166].undecodedStr = strdup("81 /7 iw");
	instruction[166].dstOperandStr = strdup("r/m16");
	instruction[166].dstOperand = IA_RM16;
	instruction[166].srcOperandStr = strdup("imm16");
	instruction[166].srcOperand = IA_IMM16;
	instruction[166].valid64 = INVALID;
	instruction[166].compatLeg = VALID;
	instruction[166].VMX = 0;
	instruction[166].description = strdup("Compare imm16 with r/m16.");
	instruction[166].binPrefixLen = 0;
	instruction[166].binOpcodeLen = 1;
	instruction[166].binOpcode = bpstrndup("\x81", 1);
	instruction[166].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[166].opcodeOperand[1] = OA_IW;
	instruction[166].opcodeOperandCount = 2;

	// Instruction Struct [167] - CMPXCHG
	instruction[167].instructionStr = strdup("CMPXCHG");
	instruction[167].undecodedStr = strdup("0F B1 /r");
	instruction[167].dstOperandStr = strdup("r/m16");
	instruction[167].dstOperand = IA_RM16;
	instruction[167].srcOperandStr = strdup("r16");
	instruction[167].srcOperand = IA_R16;
	instruction[167].valid64 = INVALID;
	instruction[167].compatLeg = VALID;
	instruction[167].VMX = 0;
	instruction[167].description = strdup("Compare AX with r/m16. If");
	instruction[167].binPrefixLen = 0;
	instruction[167].binOpcodeLen = 2;
	instruction[167].binOpcode = bpstrndup("\x0F\xB1", 2);
	instruction[167].opcodeOperand[0] = OA_MODRM;
	instruction[167].opcodeOperandCount = 1;

	// Instruction Struct [168] - CMPXCHG
	instruction[168].instructionStr = strdup("CMPXCHG");
	instruction[168].undecodedStr = strdup("0F B1 /r");
	instruction[168].dstOperandStr = strdup("r/m32");
	instruction[168].dstOperand = IA_RM32;
	instruction[168].srcOperandStr = strdup("r32");
	instruction[168].srcOperand = IA_R32;
	instruction[168].valid64 = INVALID;
	instruction[168].compatLeg = VALID;
	instruction[168].VMX = 0;
	instruction[168].description = strdup("Compare EAX with r/m32.");
	instruction[168].binPrefixLen = 0;
	instruction[168].binOpcodeLen = 2;
	instruction[168].binOpcode = bpstrndup("\x0F\xB1", 2);
	instruction[168].opcodeOperand[0] = OA_MODRM;
	instruction[168].opcodeOperandCount = 1;

	// Instruction Struct [169] - CMP
	instruction[169].instructionStr = strdup("CMP");
	instruction[169].undecodedStr = strdup("80 /7 ib");
	instruction[169].dstOperandStr = strdup("r/m8");
	instruction[169].dstOperand = IA_RM8;
	instruction[169].srcOperandStr = strdup("imm8");
	instruction[169].srcOperand = IA_IMM8;
	instruction[169].valid64 = INVALID;
	instruction[169].compatLeg = VALID;
	instruction[169].VMX = 0;
	instruction[169].description = strdup("Compare imm8 with r/m8.");
	instruction[169].binPrefixLen = 0;
	instruction[169].binOpcodeLen = 1;
	instruction[169].binOpcode = bpstrndup("\x80", 1);
	instruction[169].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[169].opcodeOperand[1] = OA_IB;
	instruction[169].opcodeOperandCount = 2;

	// Instruction Struct [170] - CMPXCHG8B
	instruction[170].instructionStr = strdup("CMPXCHG8B");
	instruction[170].undecodedStr = strdup("0F C7 /1 m64");
	instruction[170].dstOperandStr = strdup("m64");
	instruction[170].dstOperand = IA_M64;
	instruction[170].valid64 = INVALID;
	instruction[170].compatLeg = VALID;
	instruction[170].VMX = 0;
	instruction[170].description = strdup("Compare EDX:EAX with m64. If equal, set ZF and load ECX:EBX into m64. Else, clear ZF and load m64 into EDX:EAX.");
	instruction[170].binPrefixLen = 0;
	instruction[170].binOpcodeLen = 2;
	instruction[170].binOpcode = bpstrndup("\x0F\xC7", 2);
	instruction[170].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[170].opcodeOperandCount = 1;

	// Instruction Struct [171] - COMISD
	instruction[171].instructionStr = strdup("COMISD");
	instruction[171].undecodedStr = strdup("66 0F 2F /r");
	instruction[171].dstOperandStr = strdup("xmm1");
	instruction[171].dstOperand = IA_XMM;
	instruction[171].srcOperandStr = strdup("xmm2/m64");
	instruction[171].srcOperand = IA_XMM_M64;
	instruction[171].valid64 = INVALID;
	instruction[171].compatLeg = VALID;
	instruction[171].VMX = 0;
	instruction[171].description = strdup("Compare low double-precision floating-point values in xmm1 and xmm2/mem64 and set the EFLAGS flags accordingly.");
	instruction[171].prefixStr = strdup("OPSIZEOVR");
	instruction[171].binPrefixLen = 1;
	instruction[171].binPrefix = bpstrndup("\x66", 1);
	instruction[171].binOpcodeLen = 2;
	instruction[171].binOpcode = bpstrndup("\x0F\x2F", 2);
	instruction[171].opcodeOperand[0] = OA_MODRM;
	instruction[171].opcodeOperandCount = 1;

	// Instruction Struct [172] - COMISS
	instruction[172].instructionStr = strdup("COMISS");
	instruction[172].undecodedStr = strdup("0F 2F /r");
	instruction[172].dstOperandStr = strdup("xmm1");
	instruction[172].dstOperand = IA_XMM;
	instruction[172].srcOperandStr = strdup("xmm2/m32");
	instruction[172].srcOperand = IA_XMM_M32;
	instruction[172].valid64 = INVALID;
	instruction[172].compatLeg = VALID;
	instruction[172].VMX = 0;
	instruction[172].description = strdup("Compare low single-precision floating-point values in xmm1 and xmm2/mem32 and set the EFLAGS flags accordingly.");
	instruction[172].binPrefixLen = 0;
	instruction[172].binOpcodeLen = 2;
	instruction[172].binOpcode = bpstrndup("\x0F\x2F", 2);
	instruction[172].opcodeOperand[0] = OA_MODRM;
	instruction[172].opcodeOperandCount = 1;

	// Instruction Struct [173] - CPUID
	instruction[173].instructionStr = strdup("CPUID");
	instruction[173].undecodedStr = strdup("0F A2");
	instruction[173].valid64 = INVALID;
	instruction[173].compatLeg = VALID;
	instruction[173].VMX = 0;
	instruction[173].description = strdup("Returns processor identification and feature information to the EAX, EBX, ECX, and EDX registers, as determined by input entered in EAX (in some cases, ECX as well).");
	instruction[173].binPrefixLen = 0;
	instruction[173].binOpcodeLen = 2;
	instruction[173].binOpcode = bpstrndup("\x0F\xA2", 2);
	instruction[173].opcodeOperandCount = 0;

	// Instruction Struct [174] - CVTDQ2PD
	instruction[174].instructionStr = strdup("CVTDQ2PD");
	instruction[174].undecodedStr = strdup("F3 0F E6");
	instruction[174].dstOperandStr = strdup("xmm1");
	instruction[174].dstOperand = IA_XMM;
	instruction[174].srcOperandStr = strdup("xmm2/m64");
	instruction[174].srcOperand = IA_XMM_M64;
	instruction[174].valid64 = INVALID;
	instruction[174].compatLeg = VALID;
	instruction[174].VMX = 0;
	instruction[174].description = strdup("Convert two packed signed doubleword integers from xmm2/m128 to two packed double-precision floating-point values in xmm1.");
	instruction[174].prefixStr = strdup("REP");
	instruction[174].binPrefixLen = 1;
	instruction[174].binPrefix = bpstrndup("\xF3", 1);
	instruction[174].binOpcodeLen = 2;
	instruction[174].binOpcode = bpstrndup("\x0F\xE6", 2);
	instruction[174].opcodeOperandCount = 0;

	// Instruction Struct [175] - CVTDQ2PS
	instruction[175].instructionStr = strdup("CVTDQ2PS");
	instruction[175].undecodedStr = strdup("0F 5B /r");
	instruction[175].dstOperandStr = strdup("xmm1");
	instruction[175].dstOperand = IA_XMM;
	instruction[175].srcOperandStr = strdup("xmm2/m128");
	instruction[175].srcOperand = IA_XMM_M128;
	instruction[175].valid64 = INVALID;
	instruction[175].compatLeg = VALID;
	instruction[175].VMX = 0;
	instruction[175].description = strdup("Convert four packed signed doubleword integers from xmm2/m128 to four packed single-precision floating-point values in xmm1.");
	instruction[175].binPrefixLen = 0;
	instruction[175].binOpcodeLen = 2;
	instruction[175].binOpcode = bpstrndup("\x0F\x5B", 2);
	instruction[175].opcodeOperand[0] = OA_MODRM;
	instruction[175].opcodeOperandCount = 1;

	// Instruction Struct [176] - CVTPD2DQ
	instruction[176].instructionStr = strdup("CVTPD2DQ");
	instruction[176].undecodedStr = strdup("F2 0F E6");
	instruction[176].dstOperandStr = strdup("xmm1");
	instruction[176].dstOperand = IA_XMM;
	instruction[176].srcOperandStr = strdup("xmm2/m128");
	instruction[176].srcOperand = IA_XMM_M128;
	instruction[176].valid64 = INVALID;
	instruction[176].compatLeg = VALID;
	instruction[176].VMX = 0;
	instruction[176].description = strdup("Convert two packed doubleprecision floating-point values from xmm2/m128 to two packed signed doubleword integers in xmm1.");
	instruction[176].prefixStr = strdup("REPNE");
	instruction[176].binPrefixLen = 1;
	instruction[176].binPrefix = bpstrndup("\xF2", 1);
	instruction[176].binOpcodeLen = 2;
	instruction[176].binOpcode = bpstrndup("\x0F\xE6", 2);
	instruction[176].opcodeOperandCount = 0;

	// Instruction Struct [177] - CVTPD2PI
	instruction[177].instructionStr = strdup("CVTPD2PI");
	instruction[177].undecodedStr = strdup("66 0F 2D /r");
	instruction[177].dstOperandStr = strdup("mm");
	instruction[177].dstOperand = IA_MM;
	instruction[177].srcOperandStr = strdup("xmm/m128");
	instruction[177].srcOperand = IA_XMM_M128;
	instruction[177].valid64 = INVALID;
	instruction[177].compatLeg = VALID;
	instruction[177].VMX = 0;
	instruction[177].description = strdup("Convert two packed doubleprecision floating-point values from xmm/m128 to two packed signed doubleword integers in mm.");
	instruction[177].prefixStr = strdup("OPSIZEOVR");
	instruction[177].binPrefixLen = 1;
	instruction[177].binPrefix = bpstrndup("\x66", 1);
	instruction[177].binOpcodeLen = 2;
	instruction[177].binOpcode = bpstrndup("\x0F\x2D", 2);
	instruction[177].opcodeOperand[0] = OA_MODRM;
	instruction[177].opcodeOperandCount = 1;

	// Instruction Struct [178] - CVTPD2PS
	instruction[178].instructionStr = strdup("CVTPD2PS");
	instruction[178].undecodedStr = strdup("66 0F 5A /r");
	instruction[178].dstOperandStr = strdup("xmm1");
	instruction[178].dstOperand = IA_XMM;
	instruction[178].srcOperandStr = strdup("xmm2/m128");
	instruction[178].srcOperand = IA_XMM_M128;
	instruction[178].valid64 = INVALID;
	instruction[178].compatLeg = VALID;
	instruction[178].VMX = 0;
	instruction[178].description = strdup("Convert two packed doubleprecision floating-point values in xmm2/m128 to two packed singleprecision floating-point values in xmm1.");
	instruction[178].prefixStr = strdup("OPSIZEOVR");
	instruction[178].binPrefixLen = 1;
	instruction[178].binPrefix = bpstrndup("\x66", 1);
	instruction[178].binOpcodeLen = 2;
	instruction[178].binOpcode = bpstrndup("\x0F\x5A", 2);
	instruction[178].opcodeOperand[0] = OA_MODRM;
	instruction[178].opcodeOperandCount = 1;

	// Instruction Struct [179] - CVTPI2PD
	instruction[179].instructionStr = strdup("CVTPI2PD");
	instruction[179].undecodedStr = strdup("66 0F 2A /r");
	instruction[179].dstOperandStr = strdup("xmm");
	instruction[179].dstOperand = IA_XMM;
	instruction[179].srcOperandStr = strdup("mm/m64");
	instruction[179].srcOperand = IA_MM_M64;
	instruction[179].valid64 = INVALID;
	instruction[179].compatLeg = VALID;
	instruction[179].VMX = 0;
	instruction[179].description = strdup("Convert two packed signed doubleword integers from mm/mem64 to two packed doubleprecision floating-point values in xmm.");
	instruction[179].prefixStr = strdup("OPSIZEOVR");
	instruction[179].binPrefixLen = 1;
	instruction[179].binPrefix = bpstrndup("\x66", 1);
	instruction[179].binOpcodeLen = 2;
	instruction[179].binOpcode = bpstrndup("\x0F\x2A", 2);
	instruction[179].opcodeOperand[0] = OA_MODRM;
	instruction[179].opcodeOperandCount = 1;

	// Instruction Struct [180] - CVTPI2PS
	instruction[180].instructionStr = strdup("CVTPI2PS");
	instruction[180].undecodedStr = strdup("0F 2A /r");
	instruction[180].dstOperandStr = strdup("xmm");
	instruction[180].dstOperand = IA_XMM;
	instruction[180].srcOperandStr = strdup("mm/m64");
	instruction[180].srcOperand = IA_MM_M64;
	instruction[180].valid64 = INVALID;
	instruction[180].compatLeg = VALID;
	instruction[180].VMX = 0;
	instruction[180].description = strdup("Convert two signed doubleword integers from mm/m64 to two singleprecision floating-point values in xmm.");
	instruction[180].binPrefixLen = 0;
	instruction[180].binOpcodeLen = 2;
	instruction[180].binOpcode = bpstrndup("\x0F\x2A", 2);
	instruction[180].opcodeOperand[0] = OA_MODRM;
	instruction[180].opcodeOperandCount = 1;

	// Instruction Struct [181] - CVTPS2DQ
	instruction[181].instructionStr = strdup("CVTPS2DQ");
	instruction[181].undecodedStr = strdup("66 0F 5B /r");
	instruction[181].dstOperandStr = strdup("xmm1");
	instruction[181].dstOperand = IA_XMM;
	instruction[181].srcOperandStr = strdup("xmm2/m128");
	instruction[181].srcOperand = IA_XMM_M128;
	instruction[181].valid64 = INVALID;
	instruction[181].compatLeg = VALID;
	instruction[181].VMX = 0;
	instruction[181].description = strdup("Convert four packed single-precision floating-point values from xmm2/m128 to four packed signed doubleword integers in xmm1.");
	instruction[181].prefixStr = strdup("OPSIZEOVR");
	instruction[181].binPrefixLen = 1;
	instruction[181].binPrefix = bpstrndup("\x66", 1);
	instruction[181].binOpcodeLen = 2;
	instruction[181].binOpcode = bpstrndup("\x0F\x5B", 2);
	instruction[181].opcodeOperand[0] = OA_MODRM;
	instruction[181].opcodeOperandCount = 1;

	// Instruction Struct [182] - CVTPS2PD
	instruction[182].instructionStr = strdup("CVTPS2PD");
	instruction[182].undecodedStr = strdup("0F 5A /r");
	instruction[182].dstOperandStr = strdup("xmm1");
	instruction[182].dstOperand = IA_XMM;
	instruction[182].srcOperandStr = strdup("xmm2/m64");
	instruction[182].srcOperand = IA_XMM_M64;
	instruction[182].valid64 = INVALID;
	instruction[182].compatLeg = VALID;
	instruction[182].VMX = 0;
	instruction[182].description = strdup("Convert two packed single-precision floating-point values in xmm2/m64 to two packed double-precision floating-point values in xmm1.");
	instruction[182].binPrefixLen = 0;
	instruction[182].binOpcodeLen = 2;
	instruction[182].binOpcode = bpstrndup("\x0F\x5A", 2);
	instruction[182].opcodeOperand[0] = OA_MODRM;
	instruction[182].opcodeOperandCount = 1;

	// Instruction Struct [183] - CVTPS2PI
	instruction[183].instructionStr = strdup("CVTPS2PI");
	instruction[183].undecodedStr = strdup("0F 2D /r");
	instruction[183].dstOperandStr = strdup("mm");
	instruction[183].dstOperand = IA_MM;
	instruction[183].srcOperandStr = strdup("xmm/m64");
	instruction[183].srcOperand = IA_XMM_M64;
	instruction[183].valid64 = INVALID;
	instruction[183].compatLeg = VALID;
	instruction[183].VMX = 0;
	instruction[183].description = strdup("Convert two packed single-precision floating-point values from xmm/m64 to two packed signed doubleword integers in mm.");
	instruction[183].binPrefixLen = 0;
	instruction[183].binOpcodeLen = 2;
	instruction[183].binOpcode = bpstrndup("\x0F\x2D", 2);
	instruction[183].opcodeOperand[0] = OA_MODRM;
	instruction[183].opcodeOperandCount = 1;

	// Instruction Struct [184] - CVTSD2SI
	instruction[184].instructionStr = strdup("CVTSD2SI");
	instruction[184].undecodedStr = strdup("F2 0F 2D /r");
	instruction[184].dstOperandStr = strdup("r32");
	instruction[184].dstOperand = IA_R32;
	instruction[184].srcOperandStr = strdup("xmm/m64");
	instruction[184].srcOperand = IA_XMM_M64;
	instruction[184].valid64 = INVALID;
	instruction[184].compatLeg = VALID;
	instruction[184].VMX = 0;
	instruction[184].description = strdup("Convert one double-precision");
	instruction[184].prefixStr = strdup("REPNE");
	instruction[184].binPrefixLen = 1;
	instruction[184].binPrefix = bpstrndup("\xF2", 1);
	instruction[184].binOpcodeLen = 2;
	instruction[184].binOpcode = bpstrndup("\x0F\x2D", 2);
	instruction[184].opcodeOperand[0] = OA_MODRM;
	instruction[184].opcodeOperandCount = 1;

	// Instruction Struct [185] - CVTSD2SS
	instruction[185].instructionStr = strdup("CVTSD2SS");
	instruction[185].undecodedStr = strdup("F2 0F 5A /r");
	instruction[185].dstOperandStr = strdup("xmm1");
	instruction[185].dstOperand = IA_XMM;
	instruction[185].srcOperandStr = strdup("xmm2/m64");
	instruction[185].srcOperand = IA_XMM_M64;
	instruction[185].valid64 = INVALID;
	instruction[185].compatLeg = VALID;
	instruction[185].VMX = 0;
	instruction[185].description = strdup("Convert one double-precision floatingpoint value in xmm2/m64 to one single-precision floating-point value in xmm1.");
	instruction[185].prefixStr = strdup("REPNE");
	instruction[185].binPrefixLen = 1;
	instruction[185].binPrefix = bpstrndup("\xF2", 1);
	instruction[185].binOpcodeLen = 2;
	instruction[185].binOpcode = bpstrndup("\x0F\x5A", 2);
	instruction[185].opcodeOperand[0] = OA_MODRM;
	instruction[185].opcodeOperandCount = 1;

	// Instruction Struct [186] - CVTSI2S
	instruction[186].instructionStr = strdup("CVTSI2S");
	instruction[186].undecodedStr = strdup("F2 0F 2A /r");
	instruction[186].dstOperandStr = strdup("xmm");
	instruction[186].dstOperand = IA_XMM;
	instruction[186].srcOperandStr = strdup("r/m32");
	instruction[186].srcOperand = IA_RM32;
	instruction[186].valid64 = INVALID;
	instruction[186].compatLeg = VALID;
	instruction[186].VMX = 0;
	instruction[186].description = strdup("Convert one signed doubleword integer from r/m32 to one double-precision floating-point value in xmm.");
	instruction[186].prefixStr = strdup("REPNE");
	instruction[186].binPrefixLen = 1;
	instruction[186].binPrefix = bpstrndup("\xF2", 1);
	instruction[186].binOpcodeLen = 2;
	instruction[186].binOpcode = bpstrndup("\x0F\x2A", 2);
	instruction[186].opcodeOperand[0] = OA_MODRM;
	instruction[186].opcodeOperandCount = 1;

	// Instruction Struct [187] - CVTSI2SS
	instruction[187].instructionStr = strdup("CVTSI2SS");
	instruction[187].undecodedStr = strdup("F3 0F 2A /r");
	instruction[187].dstOperandStr = strdup("xmm");
	instruction[187].dstOperand = IA_XMM;
	instruction[187].srcOperandStr = strdup("r/m32");
	instruction[187].srcOperand = IA_RM32;
	instruction[187].valid64 = INVALID;
	instruction[187].compatLeg = VALID;
	instruction[187].VMX = 0;
	instruction[187].description = strdup("Convert one signed doubleword integer from r/m32 to one singleprecision floating-point value in xmm.");
	instruction[187].prefixStr = strdup("REP");
	instruction[187].binPrefixLen = 1;
	instruction[187].binPrefix = bpstrndup("\xF3", 1);
	instruction[187].binOpcodeLen = 2;
	instruction[187].binOpcode = bpstrndup("\x0F\x2A", 2);
	instruction[187].opcodeOperand[0] = OA_MODRM;
	instruction[187].opcodeOperandCount = 1;

	// Instruction Struct [188] - CVTSS2SD
	instruction[188].instructionStr = strdup("CVTSS2SD");
	instruction[188].undecodedStr = strdup("F3 0F 5A /r");
	instruction[188].dstOperandStr = strdup("xmm1");
	instruction[188].dstOperand = IA_XMM;
	instruction[188].srcOperandStr = strdup("xmm2/m32");
	instruction[188].srcOperand = IA_XMM_M32;
	instruction[188].valid64 = INVALID;
	instruction[188].compatLeg = VALID;
	instruction[188].VMX = 0;
	instruction[188].description = strdup("Convert one single-precision floatingpoint value in xmm2/m32 to one double-precision floating-point value in xmm1.");
	instruction[188].prefixStr = strdup("REP");
	instruction[188].binPrefixLen = 1;
	instruction[188].binPrefix = bpstrndup("\xF3", 1);
	instruction[188].binOpcodeLen = 2;
	instruction[188].binOpcode = bpstrndup("\x0F\x5A", 2);
	instruction[188].opcodeOperand[0] = OA_MODRM;
	instruction[188].opcodeOperandCount = 1;

	// Instruction Struct [189] - CVTSS2SI
	instruction[189].instructionStr = strdup("CVTSS2SI");
	instruction[189].undecodedStr = strdup("F3 0F 2D /r");
	instruction[189].dstOperandStr = strdup("r32");
	instruction[189].dstOperand = IA_R32;
	instruction[189].srcOperandStr = strdup("xmm/m32");
	instruction[189].srcOperand = IA_XMM_M32;
	instruction[189].valid64 = INVALID;
	instruction[189].compatLeg = VALID;
	instruction[189].VMX = 0;
	instruction[189].description = strdup("Convert one single-precision floating-point value from xmm/m32 to one signed doubleword integer in r32.");
	instruction[189].prefixStr = strdup("REP");
	instruction[189].binPrefixLen = 1;
	instruction[189].binPrefix = bpstrndup("\xF3", 1);
	instruction[189].binOpcodeLen = 2;
	instruction[189].binOpcode = bpstrndup("\x0F\x2D", 2);
	instruction[189].opcodeOperand[0] = OA_MODRM;
	instruction[189].opcodeOperandCount = 1;

	// Instruction Struct [190] - CVTTPD2PI
	instruction[190].instructionStr = strdup("CVTTPD2PI");
	instruction[190].undecodedStr = strdup("66 0F 2C /r");
	instruction[190].dstOperandStr = strdup("mm");
	instruction[190].dstOperand = IA_MM;
	instruction[190].srcOperandStr = strdup("xmm/m128");
	instruction[190].srcOperand = IA_XMM_M128;
	instruction[190].valid64 = INVALID;
	instruction[190].compatLeg = VALID;
	instruction[190].VMX = 0;
	instruction[190].description = strdup("Convert two packer double-precision floating-point values from xmm/m128 to two packed signed doubleword integers in mm using truncation.");
	instruction[190].prefixStr = strdup("OPSIZEOVR");
	instruction[190].binPrefixLen = 1;
	instruction[190].binPrefix = bpstrndup("\x66", 1);
	instruction[190].binOpcodeLen = 2;
	instruction[190].binOpcode = bpstrndup("\x0F\x2C", 2);
	instruction[190].opcodeOperand[0] = OA_MODRM;
	instruction[190].opcodeOperandCount = 1;

	// Instruction Struct [191] - CVTTPD2DQ
	instruction[191].instructionStr = strdup("CVTTPD2DQ");
	instruction[191].undecodedStr = strdup("66 0F E6");
	instruction[191].dstOperandStr = strdup("xmm1");
	instruction[191].dstOperand = IA_XMM;
	instruction[191].srcOperandStr = strdup("xmm2/m128");
	instruction[191].srcOperand = IA_XMM_M128;
	instruction[191].valid64 = INVALID;
	instruction[191].compatLeg = VALID;
	instruction[191].VMX = 0;
	instruction[191].description = strdup("Convert two packed doubleprecision floating-point values from xmm2/m128 to two packed signed doubleword integers in xmm1 using truncation.");
	instruction[191].prefixStr = strdup("OPSIZEOVR");
	instruction[191].binPrefixLen = 1;
	instruction[191].binPrefix = bpstrndup("\x66", 1);
	instruction[191].binOpcodeLen = 2;
	instruction[191].binOpcode = bpstrndup("\x0F\xE6", 2);
	instruction[191].opcodeOperandCount = 0;

	// Instruction Struct [192] - VTTPS2DQ
	instruction[192].instructionStr = strdup("VTTPS2DQ");
	instruction[192].undecodedStr = strdup("F3 0F 5B /r");
	instruction[192].dstOperandStr = strdup("xmm1");
	instruction[192].dstOperand = IA_XMM;
	instruction[192].srcOperandStr = strdup("xmm2/m128");
	instruction[192].srcOperand = IA_XMM_M128;
	instruction[192].valid64 = INVALID;
	instruction[192].compatLeg = VALID;
	instruction[192].VMX = 0;
	instruction[192].description = strdup("Convert four single-precision floating-point values from xmm2/m128 to four signed doubleword integers in xmm1 using truncation.");
	instruction[192].prefixStr = strdup("REP");
	instruction[192].binPrefixLen = 1;
	instruction[192].binPrefix = bpstrndup("\xF3", 1);
	instruction[192].binOpcodeLen = 2;
	instruction[192].binOpcode = bpstrndup("\x0F\x5B", 2);
	instruction[192].opcodeOperand[0] = OA_MODRM;
	instruction[192].opcodeOperandCount = 1;

	// Instruction Struct [193] - CVTTPS2PI
	instruction[193].instructionStr = strdup("CVTTPS2PI");
	instruction[193].undecodedStr = strdup("0F 2C /r");
	instruction[193].dstOperandStr = strdup("mm");
	instruction[193].dstOperand = IA_MM;
	instruction[193].srcOperandStr = strdup("xmm/m64");
	instruction[193].srcOperand = IA_XMM_M64;
	instruction[193].valid64 = INVALID;
	instruction[193].compatLeg = VALID;
	instruction[193].VMX = 0;
	instruction[193].description = strdup("Convert two single-precision floatingpoint values from xmm/m64 to two signed doubleword signed integers in mm using truncation.");
	instruction[193].binPrefixLen = 0;
	instruction[193].binOpcodeLen = 2;
	instruction[193].binOpcode = bpstrndup("\x0F\x2C", 2);
	instruction[193].opcodeOperand[0] = OA_MODRM;
	instruction[193].opcodeOperandCount = 1;

	// Instruction Struct [194] - CVTTSD2SI
	instruction[194].instructionStr = strdup("CVTTSD2SI");
	instruction[194].undecodedStr = strdup("F2 0F 2C /r");
	instruction[194].dstOperandStr = strdup("r32");
	instruction[194].dstOperand = IA_R32;
	instruction[194].srcOperandStr = strdup("xmm/m64");
	instruction[194].srcOperand = IA_XMM_M64;
	instruction[194].valid64 = INVALID;
	instruction[194].compatLeg = VALID;
	instruction[194].VMX = 0;
	instruction[194].description = strdup("Convert one double-precision floating-point value from xmm/m64 to one signed doubleword integer in r32 using truncation.");
	instruction[194].prefixStr = strdup("REPNE");
	instruction[194].binPrefixLen = 1;
	instruction[194].binPrefix = bpstrndup("\xF2", 1);
	instruction[194].binOpcodeLen = 2;
	instruction[194].binOpcode = bpstrndup("\x0F\x2C", 2);
	instruction[194].opcodeOperand[0] = OA_MODRM;
	instruction[194].opcodeOperandCount = 1;

	// Instruction Struct [195] - CVTTSS2SI
	instruction[195].instructionStr = strdup("CVTTSS2SI");
	instruction[195].undecodedStr = strdup("F3 0F 2C /r");
	instruction[195].dstOperandStr = strdup("r32");
	instruction[195].dstOperand = IA_R32;
	instruction[195].srcOperandStr = strdup("xmm/m32");
	instruction[195].srcOperand = IA_XMM_M32;
	instruction[195].valid64 = INVALID;
	instruction[195].compatLeg = VALID;
	instruction[195].VMX = 0;
	instruction[195].description = strdup("Convert one single-precision floating-point value from xmm/m32 to one signed doubleword integer in r32 using truncation.");
	instruction[195].prefixStr = strdup("REP");
	instruction[195].binPrefixLen = 1;
	instruction[195].binPrefix = bpstrndup("\xF3", 1);
	instruction[195].binOpcodeLen = 2;
	instruction[195].binOpcode = bpstrndup("\x0F\x2C", 2);
	instruction[195].opcodeOperand[0] = OA_MODRM;
	instruction[195].opcodeOperandCount = 1;

	// Instruction Struct [196] - CWD
	instruction[196].instructionStr = strdup("CWD");
	instruction[196].undecodedStr = strdup("99");
	instruction[196].valid64 = INVALID;
	instruction[196].compatLeg = VALID;
	instruction[196].VMX = 0;
	instruction[196].description = strdup("DX:AX ? sign-extend of AX.");
	instruction[196].binPrefixLen = 0;
	instruction[196].binOpcodeLen = 1;
	instruction[196].binOpcode = bpstrndup("\x99", 1);
	instruction[196].opcodeOperandCount = 0;

	// Instruction Struct [197] - CDQ
	instruction[197].instructionStr = strdup("CDQ");
	instruction[197].undecodedStr = strdup("99");
	instruction[197].valid64 = INVALID;
	instruction[197].compatLeg = VALID;
	instruction[197].VMX = 0;
	instruction[197].description = strdup("EDX:EAX ? sign-extend of EAX.");
	instruction[197].binPrefixLen = 0;
	instruction[197].binOpcodeLen = 1;
	instruction[197].binOpcode = bpstrndup("\x99", 1);
	instruction[197].opcodeOperandCount = 0;

	// Instruction Struct [198] - DAA
	instruction[198].instructionStr = strdup("DAA");
	instruction[198].undecodedStr = strdup("27");
	instruction[198].valid64 = INVALID;
	instruction[198].compatLeg = INVALID;
	instruction[198].VMX = 0;
	instruction[198].description = strdup("Decimal adjust AL after addition.");
	instruction[198].binPrefixLen = 0;
	instruction[198].binOpcodeLen = 1;
	instruction[198].binOpcode = bpstrndup("\x27", 1);
	instruction[198].opcodeOperandCount = 0;

	// Instruction Struct [199] - DAS
	instruction[199].instructionStr = strdup("DAS");
	instruction[199].undecodedStr = strdup("2F");
	instruction[199].valid64 = INVALID;
	instruction[199].compatLeg = INVALID;
	instruction[199].VMX = 0;
	instruction[199].description = strdup("Decimal adjust AL after subtraction.");
	instruction[199].binPrefixLen = 0;
	instruction[199].binOpcodeLen = 1;
	instruction[199].binOpcode = bpstrndup("\x2F", 1);
	instruction[199].opcodeOperandCount = 0;

	// Instruction Struct [200] - DEC
	instruction[200].instructionStr = strdup("DEC");
	instruction[200].undecodedStr = strdup("FE /1");
	instruction[200].dstOperandStr = strdup("r/m8");
	instruction[200].dstOperand = IA_RM8;
	instruction[200].valid64 = INVALID;
	instruction[200].compatLeg = VALID;
	instruction[200].VMX = 0;
	instruction[200].description = strdup("Decrement r/m8 by 1.");
	instruction[200].binPrefixLen = 0;
	instruction[200].binOpcodeLen = 1;
	instruction[200].binOpcode = bpstrndup("\xFE", 1);
	instruction[200].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[200].opcodeOperandCount = 1;

	// Instruction Struct [201] - DEC
	instruction[201].instructionStr = strdup("DEC");
	instruction[201].undecodedStr = strdup("FF /1");
	instruction[201].dstOperandStr = strdup("r/m16");
	instruction[201].dstOperand = IA_RM16;
	instruction[201].valid64 = INVALID;
	instruction[201].compatLeg = VALID;
	instruction[201].VMX = 0;
	instruction[201].description = strdup("Decrement r/m16 by 1.");
	instruction[201].binPrefixLen = 0;
	instruction[201].binOpcodeLen = 1;
	instruction[201].binOpcode = bpstrndup("\xFF", 1);
	instruction[201].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[201].opcodeOperandCount = 1;

	// Instruction Struct [202] - DEC
	instruction[202].instructionStr = strdup("DEC");
	instruction[202].undecodedStr = strdup("FF /1");
	instruction[202].dstOperandStr = strdup("r/m32");
	instruction[202].dstOperand = IA_RM32;
	instruction[202].valid64 = INVALID;
	instruction[202].compatLeg = VALID;
	instruction[202].VMX = 0;
	instruction[202].description = strdup("Decrement r/m32 by 1.");
	instruction[202].binPrefixLen = 0;
	instruction[202].binOpcodeLen = 1;
	instruction[202].binOpcode = bpstrndup("\xFF", 1);
	instruction[202].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[202].opcodeOperandCount = 1;

	// Instruction Struct [203] - DEC
	instruction[203].instructionStr = strdup("DEC");
	instruction[203].undecodedStr = strdup("48 +rw");
	instruction[203].dstOperandStr = strdup("r16");
	instruction[203].dstOperand = IA_R16;
	instruction[203].valid64 = INVALID;
	instruction[203].compatLeg = VALID;
	instruction[203].VMX = 0;
	instruction[203].description = strdup("Decrement r16 by 1.");
	instruction[203].binPrefixLen = 0;
	instruction[203].binOpcodeLen = 1;
	instruction[203].binOpcode = bpstrndup("\x48", 1);
	instruction[203].opcodeOperand[0] = OA_RW;
	instruction[203].opcodeOperandCount = 1;

	// Instruction Struct [204] - DEC
	instruction[204].instructionStr = strdup("DEC");
	instruction[204].undecodedStr = strdup("48 +rd");
	instruction[204].dstOperandStr = strdup("r32");
	instruction[204].dstOperand = IA_R32;
	instruction[204].valid64 = INVALID;
	instruction[204].compatLeg = VALID;
	instruction[204].VMX = 0;
	instruction[204].description = strdup("Decrement r32 by 1.");
	instruction[204].binPrefixLen = 0;
	instruction[204].binOpcodeLen = 1;
	instruction[204].binOpcode = bpstrndup("\x48", 1);
	instruction[204].opcodeOperand[0] = OA_RD;
	instruction[204].opcodeOperandCount = 1;

	// Instruction Struct [205] - DIV
	instruction[205].instructionStr = strdup("DIV");
	instruction[205].undecodedStr = strdup("F6 /6");
	instruction[205].dstOperandStr = strdup("r/m8");
	instruction[205].dstOperand = IA_RM8;
	instruction[205].valid64 = INVALID;
	instruction[205].compatLeg = VALID;
	instruction[205].VMX = 0;
	instruction[205].description = strdup("Unsigned divide AX by r/m8, with result stored in AL ? Quotient, AH ? Remainder.");
	instruction[205].binPrefixLen = 0;
	instruction[205].binOpcodeLen = 1;
	instruction[205].binOpcode = bpstrndup("\xF6", 1);
	instruction[205].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[205].opcodeOperandCount = 1;

	// Instruction Struct [206] - DIV
	instruction[206].instructionStr = strdup("DIV");
	instruction[206].undecodedStr = strdup("F7 /6");
	instruction[206].dstOperandStr = strdup("r/m32");
	instruction[206].dstOperand = IA_RM32;
	instruction[206].valid64 = INVALID;
	instruction[206].compatLeg = VALID;
	instruction[206].VMX = 0;
	instruction[206].description = strdup("Unsigned divide EDX:EAX by r/m32, with result stored in EAX ? Quotient, EDX ? Remainder.");
	instruction[206].binPrefixLen = 0;
	instruction[206].binOpcodeLen = 1;
	instruction[206].binOpcode = bpstrndup("\xF7", 1);
	instruction[206].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[206].opcodeOperandCount = 1;

	// Instruction Struct [207] - DIVPD
	instruction[207].instructionStr = strdup("DIVPD");
	instruction[207].undecodedStr = strdup("66 0F 5E /r");
	instruction[207].dstOperandStr = strdup("xmm1");
	instruction[207].dstOperand = IA_XMM;
	instruction[207].srcOperandStr = strdup("xmm2/m128");
	instruction[207].srcOperand = IA_XMM_M128;
	instruction[207].valid64 = INVALID;
	instruction[207].compatLeg = VALID;
	instruction[207].VMX = 0;
	instruction[207].description = strdup("Divide packed double-precision floatingpoint values in xmm1 by packed doubleprecision floating-point values xmm2/m128.");
	instruction[207].prefixStr = strdup("OPSIZEOVR");
	instruction[207].binPrefixLen = 1;
	instruction[207].binPrefix = bpstrndup("\x66", 1);
	instruction[207].binOpcodeLen = 2;
	instruction[207].binOpcode = bpstrndup("\x0F\x5E", 2);
	instruction[207].opcodeOperand[0] = OA_MODRM;
	instruction[207].opcodeOperandCount = 1;

	// Instruction Struct [208] - DIVPS
	instruction[208].instructionStr = strdup("DIVPS");
	instruction[208].undecodedStr = strdup("0F 5E /r");
	instruction[208].dstOperandStr = strdup("xmm1");
	instruction[208].dstOperand = IA_XMM;
	instruction[208].srcOperandStr = strdup("xmm2/m128");
	instruction[208].srcOperand = IA_XMM_M128;
	instruction[208].valid64 = INVALID;
	instruction[208].compatLeg = VALID;
	instruction[208].VMX = 0;
	instruction[208].description = strdup("Divide packed single-precision floatingpoint values in xmm1 by packed singleprecision floating-point values xmm2/m128.");
	instruction[208].binPrefixLen = 0;
	instruction[208].binOpcodeLen = 2;
	instruction[208].binOpcode = bpstrndup("\x0F\x5E", 2);
	instruction[208].opcodeOperand[0] = OA_MODRM;
	instruction[208].opcodeOperandCount = 1;

	// Instruction Struct [209] - DIVSD
	instruction[209].instructionStr = strdup("DIVSD");
	instruction[209].undecodedStr = strdup("F2 0F 5E /r");
	instruction[209].dstOperandStr = strdup("xmm1");
	instruction[209].dstOperand = IA_XMM;
	instruction[209].srcOperandStr = strdup("xmm2/m64");
	instruction[209].srcOperand = IA_XMM_M64;
	instruction[209].valid64 = INVALID;
	instruction[209].compatLeg = VALID;
	instruction[209].VMX = 0;
	instruction[209].description = strdup("Divide low double-precision floatingpoint value n xmm1 by low doubleprecision floating-point value in xmm2/mem64.");
	instruction[209].prefixStr = strdup("REPNE");
	instruction[209].binPrefixLen = 1;
	instruction[209].binPrefix = bpstrndup("\xF2", 1);
	instruction[209].binOpcodeLen = 2;
	instruction[209].binOpcode = bpstrndup("\x0F\x5E", 2);
	instruction[209].opcodeOperand[0] = OA_MODRM;
	instruction[209].opcodeOperandCount = 1;

	// Instruction Struct [210] - DIVSS
	instruction[210].instructionStr = strdup("DIVSS");
	instruction[210].undecodedStr = strdup("F3 0F 5E /r");
	instruction[210].dstOperandStr = strdup("xmm1");
	instruction[210].dstOperand = IA_XMM;
	instruction[210].srcOperandStr = strdup("xmm2/m32");
	instruction[210].srcOperand = IA_XMM_M32;
	instruction[210].valid64 = INVALID;
	instruction[210].compatLeg = VALID;
	instruction[210].VMX = 0;
	instruction[210].description = strdup("Divide low single-precision floatingpoint value in xmm1 by low singleprecision floating-point value in xmm2/m32.");
	instruction[210].prefixStr = strdup("REP");
	instruction[210].binPrefixLen = 1;
	instruction[210].binPrefix = bpstrndup("\xF3", 1);
	instruction[210].binOpcodeLen = 2;
	instruction[210].binOpcode = bpstrndup("\x0F\x5E", 2);
	instruction[210].opcodeOperand[0] = OA_MODRM;
	instruction[210].opcodeOperandCount = 1;

	// Instruction Struct [211] - EMMS
	instruction[211].instructionStr = strdup("EMMS");
	instruction[211].undecodedStr = strdup("0F 77");
	instruction[211].valid64 = INVALID;
	instruction[211].compatLeg = VALID;
	instruction[211].VMX = 0;
	instruction[211].description = strdup("Set the x87 FPU tag word to empty.");
	instruction[211].binPrefixLen = 0;
	instruction[211].binOpcodeLen = 2;
	instruction[211].binOpcode = bpstrndup("\x0F\x77", 2);
	instruction[211].opcodeOperandCount = 0;

	// Instruction Struct [212] - ENTER
	instruction[212].instructionStr = strdup("ENTER");
	instruction[212].undecodedStr = strdup("C8 iw 00");
	instruction[212].dstOperandStr = strdup("imm16");
	instruction[212].dstOperand = IA_IMM16;
	instruction[212].valid64 = INVALID;
	instruction[212].compatLeg = VALID;
	instruction[212].VMX = 0;
	instruction[212].description = strdup("Create a stack frame for a procedure.");
	instruction[212].binPrefixLen = 0;
	instruction[212].binOpcodeLen = 1;
	instruction[212].binOpcode = bpstrndup("\xC8", 1);
	instruction[212].opcodeOperand[0] = OA_IW;
	instruction[212].opcodeOperandCount = 1;

	// Instruction Struct [213] - ENTER
	instruction[213].instructionStr = strdup("ENTER");
	instruction[213].undecodedStr = strdup("C8 iw 01");
	instruction[213].dstOperandStr = strdup("imm16");
	instruction[213].dstOperand = IA_IMM16;
	instruction[213].srcOperandStr = strdup("1");
	instruction[213].srcOperand = IA_1;
	instruction[213].valid64 = INVALID;
	instruction[213].compatLeg = VALID;
	instruction[213].VMX = 0;
	instruction[213].description = strdup("Create a nested stack frame for a procedure.");
	instruction[213].binPrefixLen = 0;
	instruction[213].binOpcodeLen = 1;
	instruction[213].binOpcode = bpstrndup("\xC8", 1);
	instruction[213].opcodeOperand[0] = OA_IW;
	instruction[213].opcodeOperandCount = 1;

	// Instruction Struct [214] - ENTER
	instruction[214].instructionStr = strdup("ENTER");
	instruction[214].undecodedStr = strdup("C8 iw ib");
	instruction[214].dstOperandStr = strdup("imm16");
	instruction[214].dstOperand = IA_IMM16;
	instruction[214].srcOperandStr = strdup("imm8");
	instruction[214].srcOperand = IA_IMM8;
	instruction[214].valid64 = INVALID;
	instruction[214].compatLeg = VALID;
	instruction[214].VMX = 0;
	instruction[214].description = strdup("Create a nested stack frame for a procedure.");
	instruction[214].binPrefixLen = 0;
	instruction[214].binOpcodeLen = 1;
	instruction[214].binOpcode = bpstrndup("\xC8", 1);
	instruction[214].opcodeOperand[0] = OA_IW;
	instruction[214].opcodeOperand[1] = OA_IB;
	instruction[214].opcodeOperandCount = 2;

	// Instruction Struct [215] - F2XM1
	instruction[215].instructionStr = strdup("F2XM1");
	instruction[215].undecodedStr = strdup("D9 F0");
	instruction[215].valid64 = INVALID;
	instruction[215].compatLeg = VALID;
	instruction[215].VMX = 0;
	instruction[215].description = strdup("Replace ST(0) with (2ST(0) ? 1).");
	instruction[215].binPrefixLen = 0;
	instruction[215].binOpcodeLen = 2;
	instruction[215].binOpcode = bpstrndup("\xD9\xF0", 2);
	instruction[215].opcodeOperandCount = 0;

	// Instruction Struct [216] - FABS
	instruction[216].instructionStr = strdup("FABS");
	instruction[216].undecodedStr = strdup("D9 E1");
	instruction[216].valid64 = INVALID;
	instruction[216].compatLeg = VALID;
	instruction[216].VMX = 0;
	instruction[216].description = strdup("Replace ST with its absolute value.");
	instruction[216].binPrefixLen = 0;
	instruction[216].binOpcodeLen = 2;
	instruction[216].binOpcode = bpstrndup("\xD9\xE1", 2);
	instruction[216].opcodeOperandCount = 0;

	// Instruction Struct [217] - FADD
	instruction[217].instructionStr = strdup("FADD");
	instruction[217].undecodedStr = strdup("D8 /0");
	instruction[217].dstOperandStr = strdup("m32fp");
	instruction[217].dstOperand = IA_M32FP;
	instruction[217].valid64 = INVALID;
	instruction[217].compatLeg = VALID;
	instruction[217].VMX = 0;
	instruction[217].description = strdup("Add m32fp to ST(0) and store result in ST(0).");
	instruction[217].binPrefixLen = 0;
	instruction[217].binOpcodeLen = 1;
	instruction[217].binOpcode = bpstrndup("\xD8", 1);
	instruction[217].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[217].opcodeOperandCount = 1;

	// Instruction Struct [218] - FADD
	instruction[218].instructionStr = strdup("FADD");
	instruction[218].undecodedStr = strdup("DC /0");
	instruction[218].dstOperandStr = strdup("m64fp");
	instruction[218].dstOperand = IA_M64FP;
	instruction[218].valid64 = INVALID;
	instruction[218].compatLeg = VALID;
	instruction[218].VMX = 0;
	instruction[218].description = strdup("Add m64fp to ST(0) and store result in ST(0).");
	instruction[218].binPrefixLen = 0;
	instruction[218].binOpcodeLen = 1;
	instruction[218].binOpcode = bpstrndup("\xDC", 1);
	instruction[218].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[218].opcodeOperandCount = 1;

	// Instruction Struct [219] - FADD
	instruction[219].instructionStr = strdup("FADD");
	instruction[219].undecodedStr = strdup("D8 C0 +i");
	instruction[219].dstOperandStr = strdup("ST(0)");
	instruction[219].dstOperand = IA_ST_0;
	instruction[219].srcOperandStr = strdup("ST(i)");
	instruction[219].srcOperand = IA_ST_I;
	instruction[219].valid64 = INVALID;
	instruction[219].compatLeg = VALID;
	instruction[219].VMX = 0;
	instruction[219].description = strdup("Add ST(0) to ST(i) and store result in ST(0).");
	instruction[219].binPrefixLen = 0;
	instruction[219].binOpcodeLen = 2;
	instruction[219].binOpcode = bpstrndup("\xD8\xC0", 2);
	instruction[219].opcodeOperand[0] = OA_I;
	instruction[219].opcodeOperandCount = 1;

	// Instruction Struct [220] - FADD
	instruction[220].instructionStr = strdup("FADD");
	instruction[220].undecodedStr = strdup("DC C0 +i");
	instruction[220].dstOperandStr = strdup("ST(i)");
	instruction[220].dstOperand = IA_ST_I;
	instruction[220].srcOperandStr = strdup("ST(0)");
	instruction[220].srcOperand = IA_ST_0;
	instruction[220].valid64 = INVALID;
	instruction[220].compatLeg = VALID;
	instruction[220].VMX = 0;
	instruction[220].description = strdup("Add ST(i) to ST(0) and store result in ST(i).");
	instruction[220].binPrefixLen = 0;
	instruction[220].binOpcodeLen = 2;
	instruction[220].binOpcode = bpstrndup("\xDC\xC0", 2);
	instruction[220].opcodeOperand[0] = OA_I;
	instruction[220].opcodeOperandCount = 1;

	// Instruction Struct [221] - FADDP
	instruction[221].instructionStr = strdup("FADDP");
	instruction[221].undecodedStr = strdup("DE C0 +i");
	instruction[221].dstOperandStr = strdup("ST(i)");
	instruction[221].dstOperand = IA_ST_I;
	instruction[221].srcOperandStr = strdup("ST(0)");
	instruction[221].srcOperand = IA_ST_0;
	instruction[221].valid64 = INVALID;
	instruction[221].compatLeg = VALID;
	instruction[221].VMX = 0;
	instruction[221].description = strdup("Add ST(0) to ST(i), store result in ST(i), and pop the register stack.");
	instruction[221].binPrefixLen = 0;
	instruction[221].binOpcodeLen = 2;
	instruction[221].binOpcode = bpstrndup("\xDE\xC0", 2);
	instruction[221].opcodeOperand[0] = OA_I;
	instruction[221].opcodeOperandCount = 1;

	// Instruction Struct [222] - FADDP
	instruction[222].instructionStr = strdup("FADDP");
	instruction[222].undecodedStr = strdup("DE C1");
	instruction[222].valid64 = INVALID;
	instruction[222].compatLeg = VALID;
	instruction[222].VMX = 0;
	instruction[222].description = strdup("Add ST(0) to ST(1), store result in ST(1), and pop the register stack.");
	instruction[222].binPrefixLen = 0;
	instruction[222].binOpcodeLen = 2;
	instruction[222].binOpcode = bpstrndup("\xDE\xC1", 2);
	instruction[222].opcodeOperandCount = 0;

	// Instruction Struct [223] - FIADD
	instruction[223].instructionStr = strdup("FIADD");
	instruction[223].undecodedStr = strdup("DA /0");
	instruction[223].dstOperandStr = strdup("m32int");
	instruction[223].dstOperand = IA_M32INT;
	instruction[223].valid64 = INVALID;
	instruction[223].compatLeg = VALID;
	instruction[223].VMX = 0;
	instruction[223].description = strdup("Add m32int to ST(0) and store result in ST(0).");
	instruction[223].binPrefixLen = 0;
	instruction[223].binOpcodeLen = 1;
	instruction[223].binOpcode = bpstrndup("\xDA", 1);
	instruction[223].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[223].opcodeOperandCount = 1;

	// Instruction Struct [224] - FIADD
	instruction[224].instructionStr = strdup("FIADD");
	instruction[224].undecodedStr = strdup("DE /0");
	instruction[224].dstOperandStr = strdup("m16int");
	instruction[224].dstOperand = IA_M16INT;
	instruction[224].valid64 = INVALID;
	instruction[224].compatLeg = VALID;
	instruction[224].VMX = 0;
	instruction[224].description = strdup("Add m16int to ST(0) and store result in ST(0).");
	instruction[224].binPrefixLen = 0;
	instruction[224].binOpcodeLen = 1;
	instruction[224].binOpcode = bpstrndup("\xDE", 1);
	instruction[224].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[224].opcodeOperandCount = 1;

	// Instruction Struct [225] - FBLD
	instruction[225].instructionStr = strdup("FBLD");
	instruction[225].undecodedStr = strdup("DF /4");
	instruction[225].srcOperandStr = strdup("m80");
	instruction[225].srcOperand = IA_M80;
	instruction[225].valid64 = INVALID;
	instruction[225].compatLeg = VALID;
	instruction[225].VMX = 0;
	instruction[225].description = strdup("Convert BCD value to floating-point and");
	instruction[225].binPrefixLen = 0;
	instruction[225].binOpcodeLen = 1;
	instruction[225].binOpcode = bpstrndup("\xDF", 1);
	instruction[225].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[225].opcodeOperandCount = 1;

	// Instruction Struct [226] - FBSTP
	instruction[226].instructionStr = strdup("FBSTP");
	instruction[226].undecodedStr = strdup("DF /6");
	instruction[226].dstOperandStr = strdup("m80bcd");
	instruction[226].dstOperand = IA_M80BCD;
	instruction[226].valid64 = INVALID;
	instruction[226].compatLeg = VALID;
	instruction[226].VMX = 0;
	instruction[226].description = strdup("Store ST(0) in m80bcd and pop ST(0).");
	instruction[226].binPrefixLen = 0;
	instruction[226].binOpcodeLen = 1;
	instruction[226].binOpcode = bpstrndup("\xDF", 1);
	instruction[226].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[226].opcodeOperandCount = 1;

	// Instruction Struct [227] - FCHS
	instruction[227].instructionStr = strdup("FCHS");
	instruction[227].undecodedStr = strdup("D9 E0");
	instruction[227].valid64 = INVALID;
	instruction[227].compatLeg = VALID;
	instruction[227].VMX = 0;
	instruction[227].description = strdup("Complements sign of ST(0).");
	instruction[227].binPrefixLen = 0;
	instruction[227].binOpcodeLen = 2;
	instruction[227].binOpcode = bpstrndup("\xD9\xE0", 2);
	instruction[227].opcodeOperandCount = 0;

	// Instruction Struct [228] - FCLEX
	instruction[228].instructionStr = strdup("FCLEX");
	instruction[228].undecodedStr = strdup("9B DB E2");
	instruction[228].valid64 = INVALID;
	instruction[228].compatLeg = VALID;
	instruction[228].VMX = 0;
	instruction[228].description = strdup("Clear floating-point exception flags after checking for pending unmasked floatingpoint exceptions.");
	instruction[228].binPrefixLen = 0;
	instruction[228].binOpcodeLen = 3;
	instruction[228].binOpcode = bpstrndup("\x9B\xDB\xE2", 3);
	instruction[228].opcodeOperandCount = 0;

	// Instruction Struct [229] - FNCLEX
	instruction[229].instructionStr = strdup("FNCLEX");
	instruction[229].undecodedStr = strdup("DB E2");
	instruction[229].valid64 = INVALID;
	instruction[229].compatLeg = VALID;
	instruction[229].VMX = 0;
	instruction[229].description = strdup("Clear floating-point exception flags without checking for pending unmasked floating-point exceptions.");
	instruction[229].binPrefixLen = 0;
	instruction[229].binOpcodeLen = 2;
	instruction[229].binOpcode = bpstrndup("\xDB\xE2", 2);
	instruction[229].opcodeOperandCount = 0;

	// Instruction Struct [230] - FCMOVB
	instruction[230].instructionStr = strdup("FCMOVB");
	instruction[230].undecodedStr = strdup("DA C0 +i");
	instruction[230].dstOperandStr = strdup("ST(0)");
	instruction[230].dstOperand = IA_ST_0;
	instruction[230].srcOperandStr = strdup("ST(i)");
	instruction[230].srcOperand = IA_ST_I;
	instruction[230].valid64 = INVALID;
	instruction[230].compatLeg = VALID;
	instruction[230].VMX = 0;
	instruction[230].description = strdup("Move if below (CF=1).");
	instruction[230].binPrefixLen = 0;
	instruction[230].binOpcodeLen = 2;
	instruction[230].binOpcode = bpstrndup("\xDA\xC0", 2);
	instruction[230].opcodeOperand[0] = OA_I;
	instruction[230].opcodeOperandCount = 1;

	// Instruction Struct [231] - FCMOVE
	instruction[231].instructionStr = strdup("FCMOVE");
	instruction[231].undecodedStr = strdup("DA C8 +i");
	instruction[231].dstOperandStr = strdup("ST(0)");
	instruction[231].dstOperand = IA_ST_0;
	instruction[231].srcOperandStr = strdup("ST(i)");
	instruction[231].srcOperand = IA_ST_I;
	instruction[231].valid64 = INVALID;
	instruction[231].compatLeg = VALID;
	instruction[231].VMX = 0;
	instruction[231].description = strdup("Move if equal (ZF=1).");
	instruction[231].binPrefixLen = 0;
	instruction[231].binOpcodeLen = 2;
	instruction[231].binOpcode = bpstrndup("\xDA\xC8", 2);
	instruction[231].opcodeOperand[0] = OA_I;
	instruction[231].opcodeOperandCount = 1;

	// Instruction Struct [232] - FCMOVBE
	instruction[232].instructionStr = strdup("FCMOVBE");
	instruction[232].undecodedStr = strdup("DA D0 +i");
	instruction[232].dstOperandStr = strdup("ST(0)");
	instruction[232].dstOperand = IA_ST_0;
	instruction[232].srcOperandStr = strdup("ST(i)");
	instruction[232].srcOperand = IA_ST_I;
	instruction[232].valid64 = INVALID;
	instruction[232].compatLeg = VALID;
	instruction[232].VMX = 0;
	instruction[232].description = strdup("Move if below or equal (CF=1 or ZF=1).");
	instruction[232].binPrefixLen = 0;
	instruction[232].binOpcodeLen = 2;
	instruction[232].binOpcode = bpstrndup("\xDA\xD0", 2);
	instruction[232].opcodeOperand[0] = OA_I;
	instruction[232].opcodeOperandCount = 1;

	// Instruction Struct [233] - FCMOVU
	instruction[233].instructionStr = strdup("FCMOVU");
	instruction[233].undecodedStr = strdup("DA D8 +i");
	instruction[233].dstOperandStr = strdup("ST(0)");
	instruction[233].dstOperand = IA_ST_0;
	instruction[233].srcOperandStr = strdup("ST(i)");
	instruction[233].srcOperand = IA_ST_I;
	instruction[233].valid64 = INVALID;
	instruction[233].compatLeg = VALID;
	instruction[233].VMX = 0;
	instruction[233].description = strdup("Move if unordered (PF=1).");
	instruction[233].binPrefixLen = 0;
	instruction[233].binOpcodeLen = 2;
	instruction[233].binOpcode = bpstrndup("\xDA\xD8", 2);
	instruction[233].opcodeOperand[0] = OA_I;
	instruction[233].opcodeOperandCount = 1;

	// Instruction Struct [234] - FCMOVNB
	instruction[234].instructionStr = strdup("FCMOVNB");
	instruction[234].undecodedStr = strdup("DB C0 +i");
	instruction[234].dstOperandStr = strdup("ST(0)");
	instruction[234].dstOperand = IA_ST_0;
	instruction[234].srcOperandStr = strdup("ST(i)");
	instruction[234].srcOperand = IA_ST_I;
	instruction[234].valid64 = INVALID;
	instruction[234].compatLeg = VALID;
	instruction[234].VMX = 0;
	instruction[234].description = strdup("Move if not below (CF=0).");
	instruction[234].binPrefixLen = 0;
	instruction[234].binOpcodeLen = 2;
	instruction[234].binOpcode = bpstrndup("\xDB\xC0", 2);
	instruction[234].opcodeOperand[0] = OA_I;
	instruction[234].opcodeOperandCount = 1;

	// Instruction Struct [235] - FCMOVNE
	instruction[235].instructionStr = strdup("FCMOVNE");
	instruction[235].undecodedStr = strdup("DB C8 +i");
	instruction[235].dstOperandStr = strdup("ST(0)");
	instruction[235].dstOperand = IA_ST_0;
	instruction[235].srcOperandStr = strdup("ST(i)");
	instruction[235].srcOperand = IA_ST_I;
	instruction[235].valid64 = INVALID;
	instruction[235].compatLeg = VALID;
	instruction[235].VMX = 0;
	instruction[235].description = strdup("Move if not equal (ZF=0).");
	instruction[235].binPrefixLen = 0;
	instruction[235].binOpcodeLen = 2;
	instruction[235].binOpcode = bpstrndup("\xDB\xC8", 2);
	instruction[235].opcodeOperand[0] = OA_I;
	instruction[235].opcodeOperandCount = 1;

	// Instruction Struct [236] - FCMOVNBE
	instruction[236].instructionStr = strdup("FCMOVNBE");
	instruction[236].undecodedStr = strdup("DB D0 +i");
	instruction[236].dstOperandStr = strdup("ST(0)");
	instruction[236].dstOperand = IA_ST_0;
	instruction[236].srcOperandStr = strdup("ST(i)");
	instruction[236].srcOperand = IA_ST_I;
	instruction[236].valid64 = INVALID;
	instruction[236].compatLeg = VALID;
	instruction[236].VMX = 0;
	instruction[236].description = strdup("Move if not below or equal (CF=0 and ZF=0).");
	instruction[236].binPrefixLen = 0;
	instruction[236].binOpcodeLen = 2;
	instruction[236].binOpcode = bpstrndup("\xDB\xD0", 2);
	instruction[236].opcodeOperand[0] = OA_I;
	instruction[236].opcodeOperandCount = 1;

	// Instruction Struct [237] - FCMOVNU
	instruction[237].instructionStr = strdup("FCMOVNU");
	instruction[237].undecodedStr = strdup("DB D8 +i");
	instruction[237].dstOperandStr = strdup("ST(0)");
	instruction[237].dstOperand = IA_ST_0;
	instruction[237].srcOperandStr = strdup("ST(i)");
	instruction[237].srcOperand = IA_ST_I;
	instruction[237].valid64 = INVALID;
	instruction[237].compatLeg = VALID;
	instruction[237].VMX = 0;
	instruction[237].description = strdup("Move if not unordered (PF=0).");
	instruction[237].binPrefixLen = 0;
	instruction[237].binOpcodeLen = 2;
	instruction[237].binOpcode = bpstrndup("\xDB\xD8", 2);
	instruction[237].opcodeOperand[0] = OA_I;
	instruction[237].opcodeOperandCount = 1;

	// Instruction Struct [238] - FCOM
	instruction[238].instructionStr = strdup("FCOM");
	instruction[238].undecodedStr = strdup("D8 /2");
	instruction[238].dstOperandStr = strdup("m32fp");
	instruction[238].dstOperand = IA_M32FP;
	instruction[238].valid64 = INVALID;
	instruction[238].compatLeg = VALID;
	instruction[238].VMX = 0;
	instruction[238].description = strdup("Compare ST(0) with m32fp.");
	instruction[238].binPrefixLen = 0;
	instruction[238].binOpcodeLen = 1;
	instruction[238].binOpcode = bpstrndup("\xD8", 1);
	instruction[238].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[238].opcodeOperandCount = 1;

	// Instruction Struct [239] - FCOM
	instruction[239].instructionStr = strdup("FCOM");
	instruction[239].undecodedStr = strdup("DC /2");
	instruction[239].dstOperandStr = strdup("m64fp");
	instruction[239].dstOperand = IA_M64FP;
	instruction[239].valid64 = INVALID;
	instruction[239].compatLeg = VALID;
	instruction[239].VMX = 0;
	instruction[239].description = strdup("Compare ST(0) with m64fp.");
	instruction[239].binPrefixLen = 0;
	instruction[239].binOpcodeLen = 1;
	instruction[239].binOpcode = bpstrndup("\xDC", 1);
	instruction[239].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[239].opcodeOperandCount = 1;

	// Instruction Struct [240] - FCOM
	instruction[240].instructionStr = strdup("FCOM");
	instruction[240].undecodedStr = strdup("D8 D0 +i");
	instruction[240].dstOperandStr = strdup("ST(i)");
	instruction[240].dstOperand = IA_ST_I;
	instruction[240].valid64 = INVALID;
	instruction[240].compatLeg = VALID;
	instruction[240].VMX = 0;
	instruction[240].description = strdup("Compare ST(0) with ST(i).");
	instruction[240].binPrefixLen = 0;
	instruction[240].binOpcodeLen = 2;
	instruction[240].binOpcode = bpstrndup("\xD8\xD0", 2);
	instruction[240].opcodeOperand[0] = OA_I;
	instruction[240].opcodeOperandCount = 1;

	// Instruction Struct [241] - FCOM
	instruction[241].instructionStr = strdup("FCOM");
	instruction[241].undecodedStr = strdup("D8 D1");
	instruction[241].valid64 = INVALID;
	instruction[241].compatLeg = VALID;
	instruction[241].VMX = 0;
	instruction[241].description = strdup("Compare ST(0) with ST(1).");
	instruction[241].binPrefixLen = 0;
	instruction[241].binOpcodeLen = 2;
	instruction[241].binOpcode = bpstrndup("\xD8\xD1", 2);
	instruction[241].opcodeOperandCount = 0;

	// Instruction Struct [242] - FCOMP
	instruction[242].instructionStr = strdup("FCOMP");
	instruction[242].undecodedStr = strdup("D8 /3");
	instruction[242].dstOperandStr = strdup("m32fp");
	instruction[242].dstOperand = IA_M32FP;
	instruction[242].valid64 = INVALID;
	instruction[242].compatLeg = VALID;
	instruction[242].VMX = 0;
	instruction[242].description = strdup("Compare ST(0) with m32fp and pop register stack.");
	instruction[242].binPrefixLen = 0;
	instruction[242].binOpcodeLen = 1;
	instruction[242].binOpcode = bpstrndup("\xD8", 1);
	instruction[242].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[242].opcodeOperandCount = 1;

	// Instruction Struct [243] - FCOMP
	instruction[243].instructionStr = strdup("FCOMP");
	instruction[243].undecodedStr = strdup("DC /3");
	instruction[243].dstOperandStr = strdup("m64fp");
	instruction[243].dstOperand = IA_M64FP;
	instruction[243].valid64 = INVALID;
	instruction[243].compatLeg = VALID;
	instruction[243].VMX = 0;
	instruction[243].description = strdup("Compare ST(0) with m64fp and pop register stack.");
	instruction[243].binPrefixLen = 0;
	instruction[243].binOpcodeLen = 1;
	instruction[243].binOpcode = bpstrndup("\xDC", 1);
	instruction[243].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[243].opcodeOperandCount = 1;

	// Instruction Struct [244] - FCOMP
	instruction[244].instructionStr = strdup("FCOMP");
	instruction[244].undecodedStr = strdup("D8 D8 +i");
	instruction[244].dstOperandStr = strdup("ST(i)");
	instruction[244].dstOperand = IA_ST_I;
	instruction[244].valid64 = INVALID;
	instruction[244].compatLeg = VALID;
	instruction[244].VMX = 0;
	instruction[244].description = strdup("Compare ST(0) with ST(i) and pop register stack.");
	instruction[244].binPrefixLen = 0;
	instruction[244].binOpcodeLen = 2;
	instruction[244].binOpcode = bpstrndup("\xD8\xD8", 2);
	instruction[244].opcodeOperand[0] = OA_I;
	instruction[244].opcodeOperandCount = 1;

	// Instruction Struct [245] - FCOMP
	instruction[245].instructionStr = strdup("FCOMP");
	instruction[245].undecodedStr = strdup("D8 D9");
	instruction[245].valid64 = INVALID;
	instruction[245].compatLeg = VALID;
	instruction[245].VMX = 0;
	instruction[245].description = strdup("Compare ST(0) with ST(1) and pop register stack.");
	instruction[245].binPrefixLen = 0;
	instruction[245].binOpcodeLen = 2;
	instruction[245].binOpcode = bpstrndup("\xD8\xD9", 2);
	instruction[245].opcodeOperandCount = 0;

	// Instruction Struct [246] - FCOMPP
	instruction[246].instructionStr = strdup("FCOMPP");
	instruction[246].undecodedStr = strdup("DE D9");
	instruction[246].valid64 = INVALID;
	instruction[246].compatLeg = VALID;
	instruction[246].VMX = 0;
	instruction[246].description = strdup("Compare ST(0) with ST(1) and pop register stack twice.");
	instruction[246].binPrefixLen = 0;
	instruction[246].binOpcodeLen = 2;
	instruction[246].binOpcode = bpstrndup("\xDE\xD9", 2);
	instruction[246].opcodeOperandCount = 0;

	// Instruction Struct [247] - FCOMI
	instruction[247].instructionStr = strdup("FCOMI");
	instruction[247].undecodedStr = strdup("DB F0 +i");
	instruction[247].dstOperandStr = strdup("ST");
	instruction[247].dstOperand = IA_ST_0;
	instruction[247].srcOperandStr = strdup("ST(i)");
	instruction[247].srcOperand = IA_ST_I;
	instruction[247].valid64 = INVALID;
	instruction[247].compatLeg = VALID;
	instruction[247].VMX = 0;
	instruction[247].description = strdup("Compare ST(0) with ST(i) and set status flags accordingly.");
	instruction[247].binPrefixLen = 0;
	instruction[247].binOpcodeLen = 2;
	instruction[247].binOpcode = bpstrndup("\xDB\xF0", 2);
	instruction[247].opcodeOperand[0] = OA_I;
	instruction[247].opcodeOperandCount = 1;

	// Instruction Struct [248] - FCOMIP
	instruction[248].instructionStr = strdup("FCOMIP");
	instruction[248].undecodedStr = strdup("DF F0 +i");
	instruction[248].dstOperandStr = strdup("ST");
	instruction[248].dstOperand = IA_ST_0;
	instruction[248].srcOperandStr = strdup("ST(i)");
	instruction[248].srcOperand = IA_ST_I;
	instruction[248].valid64 = INVALID;
	instruction[248].compatLeg = VALID;
	instruction[248].VMX = 0;
	instruction[248].description = strdup("Compare ST(0) with ST(i), set status flags accordingly, and pop register stack.");
	instruction[248].binPrefixLen = 0;
	instruction[248].binOpcodeLen = 2;
	instruction[248].binOpcode = bpstrndup("\xDF\xF0", 2);
	instruction[248].opcodeOperand[0] = OA_I;
	instruction[248].opcodeOperandCount = 1;

	// Instruction Struct [249] - FUCOMI
	instruction[249].instructionStr = strdup("FUCOMI");
	instruction[249].undecodedStr = strdup("DB E8 +i");
	instruction[249].dstOperandStr = strdup("ST");
	instruction[249].dstOperand = IA_ST_0;
	instruction[249].srcOperandStr = strdup("ST(i)");
	instruction[249].srcOperand = IA_ST_I;
	instruction[249].valid64 = INVALID;
	instruction[249].compatLeg = VALID;
	instruction[249].VMX = 0;
	instruction[249].description = strdup("Compare ST(0) with ST(i), check for ordered values, and set status flags accordingly.");
	instruction[249].binPrefixLen = 0;
	instruction[249].binOpcodeLen = 2;
	instruction[249].binOpcode = bpstrndup("\xDB\xE8", 2);
	instruction[249].opcodeOperand[0] = OA_I;
	instruction[249].opcodeOperandCount = 1;

	// Instruction Struct [250] - FUCOMIP
	instruction[250].instructionStr = strdup("FUCOMIP");
	instruction[250].undecodedStr = strdup("DF E8 +i");
	instruction[250].dstOperandStr = strdup("ST");
	instruction[250].dstOperand = IA_ST_0;
	instruction[250].srcOperandStr = strdup("ST(i)");
	instruction[250].srcOperand = IA_ST_I;
	instruction[250].valid64 = INVALID;
	instruction[250].compatLeg = VALID;
	instruction[250].VMX = 0;
	instruction[250].description = strdup("Compare ST(0) with ST(i), check for ordered values, set status flags accordingly, and pop register stack.");
	instruction[250].binPrefixLen = 0;
	instruction[250].binOpcodeLen = 2;
	instruction[250].binOpcode = bpstrndup("\xDF\xE8", 2);
	instruction[250].opcodeOperand[0] = OA_I;
	instruction[250].opcodeOperandCount = 1;

	// Instruction Struct [251] - FCOS
	instruction[251].instructionStr = strdup("FCOS");
	instruction[251].undecodedStr = strdup("D9 FF");
	instruction[251].valid64 = INVALID;
	instruction[251].compatLeg = VALID;
	instruction[251].VMX = 0;
	instruction[251].description = strdup("Replace ST(0) with its cosine.");
	instruction[251].binPrefixLen = 0;
	instruction[251].binOpcodeLen = 2;
	instruction[251].binOpcode = bpstrndup("\xD9\xFF", 2);
	instruction[251].opcodeOperandCount = 0;

	// Instruction Struct [252] - FDECSTP
	instruction[252].instructionStr = strdup("FDECSTP");
	instruction[252].undecodedStr = strdup("D9 F6");
	instruction[252].valid64 = INVALID;
	instruction[252].compatLeg = VALID;
	instruction[252].VMX = 0;
	instruction[252].description = strdup("Decrement TOP field in FPU status word.");
	instruction[252].binPrefixLen = 0;
	instruction[252].binOpcodeLen = 2;
	instruction[252].binOpcode = bpstrndup("\xD9\xF6", 2);
	instruction[252].opcodeOperandCount = 0;

	// Instruction Struct [253] - FDIV
	instruction[253].instructionStr = strdup("FDIV");
	instruction[253].undecodedStr = strdup("D8 /6");
	instruction[253].dstOperandStr = strdup("m32fp");
	instruction[253].dstOperand = IA_M32FP;
	instruction[253].valid64 = INVALID;
	instruction[253].compatLeg = VALID;
	instruction[253].VMX = 0;
	instruction[253].description = strdup("Divide ST(0) by m32fp and store result in ST(0).");
	instruction[253].binPrefixLen = 0;
	instruction[253].binOpcodeLen = 1;
	instruction[253].binOpcode = bpstrndup("\xD8", 1);
	instruction[253].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[253].opcodeOperandCount = 1;

	// Instruction Struct [254] - FDIV
	instruction[254].instructionStr = strdup("FDIV");
	instruction[254].undecodedStr = strdup("DC /6");
	instruction[254].dstOperandStr = strdup("m64fp");
	instruction[254].dstOperand = IA_M64FP;
	instruction[254].valid64 = INVALID;
	instruction[254].compatLeg = VALID;
	instruction[254].VMX = 0;
	instruction[254].description = strdup("Divide ST(0) by m64fp and store result in ST(0).");
	instruction[254].binPrefixLen = 0;
	instruction[254].binOpcodeLen = 1;
	instruction[254].binOpcode = bpstrndup("\xDC", 1);
	instruction[254].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[254].opcodeOperandCount = 1;

	// Instruction Struct [255] - FDIV
	instruction[255].instructionStr = strdup("FDIV");
	instruction[255].undecodedStr = strdup("D8 F0 +i");
	instruction[255].dstOperandStr = strdup("ST(0)");
	instruction[255].dstOperand = IA_ST_0;
	instruction[255].srcOperandStr = strdup("ST(i)");
	instruction[255].srcOperand = IA_ST_I;
	instruction[255].valid64 = INVALID;
	instruction[255].compatLeg = VALID;
	instruction[255].VMX = 0;
	instruction[255].description = strdup("Divide ST(0) by ST(i) and store result in ST(0).");
	instruction[255].binPrefixLen = 0;
	instruction[255].binOpcodeLen = 2;
	instruction[255].binOpcode = bpstrndup("\xD8\xF0", 2);
	instruction[255].opcodeOperand[0] = OA_I;
	instruction[255].opcodeOperandCount = 1;

	// Instruction Struct [256] - FDIV
	instruction[256].instructionStr = strdup("FDIV");
	instruction[256].undecodedStr = strdup("DC F8 +i");
	instruction[256].dstOperandStr = strdup("ST(i)");
	instruction[256].dstOperand = IA_ST_I;
	instruction[256].srcOperandStr = strdup("ST(0)");
	instruction[256].srcOperand = IA_ST_0;
	instruction[256].valid64 = INVALID;
	instruction[256].compatLeg = VALID;
	instruction[256].VMX = 0;
	instruction[256].description = strdup("Divide ST(i) by ST(0) and store result in ST(i).");
	instruction[256].binPrefixLen = 0;
	instruction[256].binOpcodeLen = 2;
	instruction[256].binOpcode = bpstrndup("\xDC\xF8", 2);
	instruction[256].opcodeOperand[0] = OA_I;
	instruction[256].opcodeOperandCount = 1;

	// Instruction Struct [257] - FDIVP
	instruction[257].instructionStr = strdup("FDIVP");
	instruction[257].undecodedStr = strdup("DE F8 +i");
	instruction[257].dstOperandStr = strdup("ST(i)");
	instruction[257].dstOperand = IA_ST_I;
	instruction[257].srcOperandStr = strdup("ST(0)");
	instruction[257].srcOperand = IA_ST_0;
	instruction[257].valid64 = INVALID;
	instruction[257].compatLeg = VALID;
	instruction[257].VMX = 0;
	instruction[257].description = strdup("Divide ST(i) by ST(0), store result in ST(i), and pop the register stack.");
	instruction[257].binPrefixLen = 0;
	instruction[257].binOpcodeLen = 2;
	instruction[257].binOpcode = bpstrndup("\xDE\xF8", 2);
	instruction[257].opcodeOperand[0] = OA_I;
	instruction[257].opcodeOperandCount = 1;

	// Instruction Struct [258] - FDIVP
	instruction[258].instructionStr = strdup("FDIVP");
	instruction[258].undecodedStr = strdup("DE F9");
	instruction[258].valid64 = INVALID;
	instruction[258].compatLeg = VALID;
	instruction[258].VMX = 0;
	instruction[258].description = strdup("Divide ST(1) by ST(0), store result in ST(1), and pop the register stack.");
	instruction[258].binPrefixLen = 0;
	instruction[258].binOpcodeLen = 2;
	instruction[258].binOpcode = bpstrndup("\xDE\xF9", 2);
	instruction[258].opcodeOperandCount = 0;

	// Instruction Struct [259] - FIDIV
	instruction[259].instructionStr = strdup("FIDIV");
	instruction[259].undecodedStr = strdup("DA /6");
	instruction[259].dstOperandStr = strdup("m32int");
	instruction[259].dstOperand = IA_M32INT;
	instruction[259].valid64 = INVALID;
	instruction[259].compatLeg = VALID;
	instruction[259].VMX = 0;
	instruction[259].description = strdup("Divide ST(0) by m32int and store result in ST(0).");
	instruction[259].binPrefixLen = 0;
	instruction[259].binOpcodeLen = 1;
	instruction[259].binOpcode = bpstrndup("\xDA", 1);
	instruction[259].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[259].opcodeOperandCount = 1;

	// Instruction Struct [260] - FIDIV
	instruction[260].instructionStr = strdup("FIDIV");
	instruction[260].undecodedStr = strdup("DE /6");
	instruction[260].dstOperandStr = strdup("m16int");
	instruction[260].dstOperand = IA_M16INT;
	instruction[260].valid64 = INVALID;
	instruction[260].compatLeg = VALID;
	instruction[260].VMX = 0;
	instruction[260].description = strdup("Divide ST(0) by m64int and store result in ST(0).");
	instruction[260].binPrefixLen = 0;
	instruction[260].binOpcodeLen = 1;
	instruction[260].binOpcode = bpstrndup("\xDE", 1);
	instruction[260].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[260].opcodeOperandCount = 1;

	// Instruction Struct [261] - FDIVR
	instruction[261].instructionStr = strdup("FDIVR");
	instruction[261].undecodedStr = strdup("D8 /7");
	instruction[261].dstOperandStr = strdup("m32fp");
	instruction[261].dstOperand = IA_M32FP;
	instruction[261].valid64 = INVALID;
	instruction[261].compatLeg = VALID;
	instruction[261].VMX = 0;
	instruction[261].description = strdup("Divide m32fp by ST(0) and store result in ST(0).");
	instruction[261].binPrefixLen = 0;
	instruction[261].binOpcodeLen = 1;
	instruction[261].binOpcode = bpstrndup("\xD8", 1);
	instruction[261].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[261].opcodeOperandCount = 1;

	// Instruction Struct [262] - FDIVR
	instruction[262].instructionStr = strdup("FDIVR");
	instruction[262].undecodedStr = strdup("DC /7");
	instruction[262].dstOperandStr = strdup("m64fp");
	instruction[262].dstOperand = IA_M64FP;
	instruction[262].valid64 = INVALID;
	instruction[262].compatLeg = VALID;
	instruction[262].VMX = 0;
	instruction[262].description = strdup("Divide m64fp by ST(0) and store result in ST(0).");
	instruction[262].binPrefixLen = 0;
	instruction[262].binOpcodeLen = 1;
	instruction[262].binOpcode = bpstrndup("\xDC", 1);
	instruction[262].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[262].opcodeOperandCount = 1;

	// Instruction Struct [263] - FDIVR
	instruction[263].instructionStr = strdup("FDIVR");
	instruction[263].undecodedStr = strdup("D8 F8 +i");
	instruction[263].dstOperandStr = strdup("ST(0)");
	instruction[263].dstOperand = IA_ST_0;
	instruction[263].srcOperandStr = strdup("ST(i)");
	instruction[263].srcOperand = IA_ST_I;
	instruction[263].valid64 = INVALID;
	instruction[263].compatLeg = VALID;
	instruction[263].VMX = 0;
	instruction[263].description = strdup("Divide ST(i) by ST(0) and store result in ST(0).");
	instruction[263].binPrefixLen = 0;
	instruction[263].binOpcodeLen = 2;
	instruction[263].binOpcode = bpstrndup("\xD8\xF8", 2);
	instruction[263].opcodeOperand[0] = OA_I;
	instruction[263].opcodeOperandCount = 1;

	// Instruction Struct [264] - FDIVR
	instruction[264].instructionStr = strdup("FDIVR");
	instruction[264].undecodedStr = strdup("DC F0 +i");
	instruction[264].dstOperandStr = strdup("ST(i)");
	instruction[264].dstOperand = IA_ST_I;
	instruction[264].srcOperandStr = strdup("ST(0)");
	instruction[264].srcOperand = IA_ST_0;
	instruction[264].valid64 = INVALID;
	instruction[264].compatLeg = VALID;
	instruction[264].VMX = 0;
	instruction[264].description = strdup("Divide ST(0) by ST(i) and store result in ST(i).");
	instruction[264].binPrefixLen = 0;
	instruction[264].binOpcodeLen = 2;
	instruction[264].binOpcode = bpstrndup("\xDC\xF0", 2);
	instruction[264].opcodeOperand[0] = OA_I;
	instruction[264].opcodeOperandCount = 1;

	// Instruction Struct [265] - FDIVRP
	instruction[265].instructionStr = strdup("FDIVRP");
	instruction[265].undecodedStr = strdup("DE F0 +i");
	instruction[265].dstOperandStr = strdup("ST(i)");
	instruction[265].dstOperand = IA_ST_I;
	instruction[265].srcOperandStr = strdup("ST(0)");
	instruction[265].srcOperand = IA_ST_0;
	instruction[265].valid64 = INVALID;
	instruction[265].compatLeg = VALID;
	instruction[265].VMX = 0;
	instruction[265].description = strdup("Divide ST(0) by ST(i), store result in ST(i), and pop the register stack.");
	instruction[265].binPrefixLen = 0;
	instruction[265].binOpcodeLen = 2;
	instruction[265].binOpcode = bpstrndup("\xDE\xF0", 2);
	instruction[265].opcodeOperand[0] = OA_I;
	instruction[265].opcodeOperandCount = 1;

	// Instruction Struct [266] - FDIVRP
	instruction[266].instructionStr = strdup("FDIVRP");
	instruction[266].undecodedStr = strdup("DE F1");
	instruction[266].valid64 = INVALID;
	instruction[266].compatLeg = VALID;
	instruction[266].VMX = 0;
	instruction[266].description = strdup("Divide ST(0) by ST(1), store result in ST(1), and pop the register stack.");
	instruction[266].binPrefixLen = 0;
	instruction[266].binOpcodeLen = 2;
	instruction[266].binOpcode = bpstrndup("\xDE\xF1", 2);
	instruction[266].opcodeOperandCount = 0;

	// Instruction Struct [267] - FIDIVR
	instruction[267].instructionStr = strdup("FIDIVR");
	instruction[267].undecodedStr = strdup("DA /7");
	instruction[267].dstOperandStr = strdup("m32int");
	instruction[267].dstOperand = IA_M32INT;
	instruction[267].valid64 = INVALID;
	instruction[267].compatLeg = VALID;
	instruction[267].VMX = 0;
	instruction[267].description = strdup("Divide m32int by ST(0) and store result in ST(0).");
	instruction[267].binPrefixLen = 0;
	instruction[267].binOpcodeLen = 1;
	instruction[267].binOpcode = bpstrndup("\xDA", 1);
	instruction[267].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[267].opcodeOperandCount = 1;

	// Instruction Struct [268] - FIDIVR
	instruction[268].instructionStr = strdup("FIDIVR");
	instruction[268].undecodedStr = strdup("DE /7");
	instruction[268].dstOperandStr = strdup("m16int");
	instruction[268].dstOperand = IA_M16INT;
	instruction[268].valid64 = INVALID;
	instruction[268].compatLeg = VALID;
	instruction[268].VMX = 0;
	instruction[268].description = strdup("Divide m16int by ST(0) and store result in ST(0).");
	instruction[268].binPrefixLen = 0;
	instruction[268].binOpcodeLen = 1;
	instruction[268].binOpcode = bpstrndup("\xDE", 1);
	instruction[268].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[268].opcodeOperandCount = 1;

	// Instruction Struct [269] - FFREE
	instruction[269].instructionStr = strdup("FFREE");
	instruction[269].undecodedStr = strdup("DD C0 +i");
	instruction[269].dstOperandStr = strdup("ST(i)");
	instruction[269].dstOperand = IA_ST_I;
	instruction[269].valid64 = INVALID;
	instruction[269].compatLeg = VALID;
	instruction[269].VMX = 0;
	instruction[269].description = strdup("Sets tag for ST(i) to empty.");
	instruction[269].binPrefixLen = 0;
	instruction[269].binOpcodeLen = 2;
	instruction[269].binOpcode = bpstrndup("\xDD\xC0", 2);
	instruction[269].opcodeOperand[0] = OA_I;
	instruction[269].opcodeOperandCount = 1;

	// Instruction Struct [270] - FICOM
	instruction[270].instructionStr = strdup("FICOM");
	instruction[270].undecodedStr = strdup("DE /2");
	instruction[270].dstOperandStr = strdup("m16int");
	instruction[270].dstOperand = IA_M16INT;
	instruction[270].valid64 = INVALID;
	instruction[270].compatLeg = VALID;
	instruction[270].VMX = 0;
	instruction[270].description = strdup("Compare ST(0) with m16int.");
	instruction[270].binPrefixLen = 0;
	instruction[270].binOpcodeLen = 1;
	instruction[270].binOpcode = bpstrndup("\xDE", 1);
	instruction[270].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[270].opcodeOperandCount = 1;

	// Instruction Struct [271] - FICOM
	instruction[271].instructionStr = strdup("FICOM");
	instruction[271].undecodedStr = strdup("DA /2");
	instruction[271].dstOperandStr = strdup("m32int");
	instruction[271].dstOperand = IA_M32INT;
	instruction[271].valid64 = INVALID;
	instruction[271].compatLeg = VALID;
	instruction[271].VMX = 0;
	instruction[271].description = strdup("Compare ST(0) with m32int.");
	instruction[271].binPrefixLen = 0;
	instruction[271].binOpcodeLen = 1;
	instruction[271].binOpcode = bpstrndup("\xDA", 1);
	instruction[271].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[271].opcodeOperandCount = 1;

	// Instruction Struct [272] - FICOMP
	instruction[272].instructionStr = strdup("FICOMP");
	instruction[272].undecodedStr = strdup("DE /3");
	instruction[272].dstOperandStr = strdup("m16int");
	instruction[272].dstOperand = IA_M16INT;
	instruction[272].valid64 = INVALID;
	instruction[272].compatLeg = VALID;
	instruction[272].VMX = 0;
	instruction[272].description = strdup("Compare ST(0) with m16int and pop stack register.");
	instruction[272].binPrefixLen = 0;
	instruction[272].binOpcodeLen = 1;
	instruction[272].binOpcode = bpstrndup("\xDE", 1);
	instruction[272].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[272].opcodeOperandCount = 1;

	// Instruction Struct [273] - FICOMP
	instruction[273].instructionStr = strdup("FICOMP");
	instruction[273].undecodedStr = strdup("DA /3");
	instruction[273].dstOperandStr = strdup("m32int");
	instruction[273].dstOperand = IA_M32INT;
	instruction[273].valid64 = INVALID;
	instruction[273].compatLeg = VALID;
	instruction[273].VMX = 0;
	instruction[273].description = strdup("Compare ST(0) with m32int and pop stack register.");
	instruction[273].binPrefixLen = 0;
	instruction[273].binOpcodeLen = 1;
	instruction[273].binOpcode = bpstrndup("\xDA", 1);
	instruction[273].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[273].opcodeOperandCount = 1;

	// Instruction Struct [274] - FILD
	instruction[274].instructionStr = strdup("FILD");
	instruction[274].undecodedStr = strdup("DF /0");
	instruction[274].dstOperandStr = strdup("m16int");
	instruction[274].dstOperand = IA_M16INT;
	instruction[274].valid64 = INVALID;
	instruction[274].compatLeg = VALID;
	instruction[274].VMX = 0;
	instruction[274].description = strdup("Push m16int onto the FPU register stack.");
	instruction[274].binPrefixLen = 0;
	instruction[274].binOpcodeLen = 1;
	instruction[274].binOpcode = bpstrndup("\xDF", 1);
	instruction[274].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[274].opcodeOperandCount = 1;

	// Instruction Struct [275] - FILD
	instruction[275].instructionStr = strdup("FILD");
	instruction[275].undecodedStr = strdup("DB /0");
	instruction[275].dstOperandStr = strdup("m32int");
	instruction[275].dstOperand = IA_M32INT;
	instruction[275].valid64 = INVALID;
	instruction[275].compatLeg = VALID;
	instruction[275].VMX = 0;
	instruction[275].description = strdup("Push m32int onto the FPU register stack.");
	instruction[275].binPrefixLen = 0;
	instruction[275].binOpcodeLen = 1;
	instruction[275].binOpcode = bpstrndup("\xDB", 1);
	instruction[275].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[275].opcodeOperandCount = 1;

	// Instruction Struct [276] - FILD
	instruction[276].instructionStr = strdup("FILD");
	instruction[276].undecodedStr = strdup("DF /5");
	instruction[276].dstOperandStr = strdup("m64int");
	instruction[276].dstOperand = IA_M64INT;
	instruction[276].valid64 = INVALID;
	instruction[276].compatLeg = VALID;
	instruction[276].VMX = 0;
	instruction[276].description = strdup("Push m64int onto the FPU register stack.");
	instruction[276].binPrefixLen = 0;
	instruction[276].binOpcodeLen = 1;
	instruction[276].binOpcode = bpstrndup("\xDF", 1);
	instruction[276].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[276].opcodeOperandCount = 1;

	// Instruction Struct [277] - FINCSTP
	instruction[277].instructionStr = strdup("FINCSTP");
	instruction[277].undecodedStr = strdup("D9 F7");
	instruction[277].valid64 = INVALID;
	instruction[277].compatLeg = VALID;
	instruction[277].VMX = 0;
	instruction[277].description = strdup("Increment the TOP field in the FPU status register.");
	instruction[277].binPrefixLen = 0;
	instruction[277].binOpcodeLen = 2;
	instruction[277].binOpcode = bpstrndup("\xD9\xF7", 2);
	instruction[277].opcodeOperandCount = 0;

	// Instruction Struct [278] - FINIT
	instruction[278].instructionStr = strdup("FINIT");
	instruction[278].undecodedStr = strdup("9B DB E3");
	instruction[278].valid64 = INVALID;
	instruction[278].compatLeg = VALID;
	instruction[278].VMX = 0;
	instruction[278].description = strdup("Initialize FPU after checking for pending unmasked floating-point exceptions.");
	instruction[278].binPrefixLen = 0;
	instruction[278].binOpcodeLen = 3;
	instruction[278].binOpcode = bpstrndup("\x9B\xDB\xE3", 3);
	instruction[278].opcodeOperandCount = 0;

	// Instruction Struct [279] - FNINIT
	instruction[279].instructionStr = strdup("FNINIT");
	instruction[279].undecodedStr = strdup("DB E3");
	instruction[279].valid64 = INVALID;
	instruction[279].compatLeg = VALID;
	instruction[279].VMX = 0;
	instruction[279].description = strdup("Initialize FPU without checking for pending unmasked floating-point exceptions.");
	instruction[279].binPrefixLen = 0;
	instruction[279].binOpcodeLen = 2;
	instruction[279].binOpcode = bpstrndup("\xDB\xE3", 2);
	instruction[279].opcodeOperandCount = 0;

	// Instruction Struct [280] - FIST
	instruction[280].instructionStr = strdup("FIST");
	instruction[280].undecodedStr = strdup("DF /2");
	instruction[280].dstOperandStr = strdup("m16int");
	instruction[280].dstOperand = IA_M16INT;
	instruction[280].valid64 = INVALID;
	instruction[280].compatLeg = VALID;
	instruction[280].VMX = 0;
	instruction[280].description = strdup("Store ST(0) in m16int.");
	instruction[280].binPrefixLen = 0;
	instruction[280].binOpcodeLen = 1;
	instruction[280].binOpcode = bpstrndup("\xDF", 1);
	instruction[280].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[280].opcodeOperandCount = 1;

	// Instruction Struct [281] - FIST
	instruction[281].instructionStr = strdup("FIST");
	instruction[281].undecodedStr = strdup("DB /2");
	instruction[281].dstOperandStr = strdup("m32int");
	instruction[281].dstOperand = IA_M32INT;
	instruction[281].valid64 = INVALID;
	instruction[281].compatLeg = VALID;
	instruction[281].VMX = 0;
	instruction[281].description = strdup("Store ST(0) in m32int.");
	instruction[281].binPrefixLen = 0;
	instruction[281].binOpcodeLen = 1;
	instruction[281].binOpcode = bpstrndup("\xDB", 1);
	instruction[281].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[281].opcodeOperandCount = 1;

	// Instruction Struct [282] - FISTP
	instruction[282].instructionStr = strdup("FISTP");
	instruction[282].undecodedStr = strdup("DF /3");
	instruction[282].dstOperandStr = strdup("m16int");
	instruction[282].dstOperand = IA_M16INT;
	instruction[282].valid64 = INVALID;
	instruction[282].compatLeg = VALID;
	instruction[282].VMX = 0;
	instruction[282].description = strdup("Store ST(0) in m16int and pop register stack.");
	instruction[282].binPrefixLen = 0;
	instruction[282].binOpcodeLen = 1;
	instruction[282].binOpcode = bpstrndup("\xDF", 1);
	instruction[282].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[282].opcodeOperandCount = 1;

	// Instruction Struct [283] - FISTP
	instruction[283].instructionStr = strdup("FISTP");
	instruction[283].undecodedStr = strdup("DB /3");
	instruction[283].dstOperandStr = strdup("m32int");
	instruction[283].dstOperand = IA_M32INT;
	instruction[283].valid64 = INVALID;
	instruction[283].compatLeg = VALID;
	instruction[283].VMX = 0;
	instruction[283].description = strdup("Store ST(0) in m32int and pop register stack.");
	instruction[283].binPrefixLen = 0;
	instruction[283].binOpcodeLen = 1;
	instruction[283].binOpcode = bpstrndup("\xDB", 1);
	instruction[283].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[283].opcodeOperandCount = 1;

	// Instruction Struct [284] - FISTP
	instruction[284].instructionStr = strdup("FISTP");
	instruction[284].undecodedStr = strdup("DF /7");
	instruction[284].dstOperandStr = strdup("m64int");
	instruction[284].dstOperand = IA_M64INT;
	instruction[284].valid64 = INVALID;
	instruction[284].compatLeg = VALID;
	instruction[284].VMX = 0;
	instruction[284].description = strdup("Store ST(0) in m64int and pop register stack.");
	instruction[284].binPrefixLen = 0;
	instruction[284].binOpcodeLen = 1;
	instruction[284].binOpcode = bpstrndup("\xDF", 1);
	instruction[284].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[284].opcodeOperandCount = 1;

	// Instruction Struct [285] - FISTTP
	instruction[285].instructionStr = strdup("FISTTP");
	instruction[285].undecodedStr = strdup("DF /1");
	instruction[285].dstOperandStr = strdup("m16int");
	instruction[285].dstOperand = IA_M16INT;
	instruction[285].valid64 = INVALID;
	instruction[285].compatLeg = VALID;
	instruction[285].VMX = 0;
	instruction[285].description = strdup("Store ST(0) in m16int with truncation.");
	instruction[285].binPrefixLen = 0;
	instruction[285].binOpcodeLen = 1;
	instruction[285].binOpcode = bpstrndup("\xDF", 1);
	instruction[285].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[285].opcodeOperandCount = 1;

	// Instruction Struct [286] - FISTTP
	instruction[286].instructionStr = strdup("FISTTP");
	instruction[286].undecodedStr = strdup("DB /1");
	instruction[286].dstOperandStr = strdup("m32int");
	instruction[286].dstOperand = IA_M32INT;
	instruction[286].valid64 = INVALID;
	instruction[286].compatLeg = VALID;
	instruction[286].VMX = 0;
	instruction[286].description = strdup("Store ST(0) in m32int with truncation.");
	instruction[286].binPrefixLen = 0;
	instruction[286].binOpcodeLen = 1;
	instruction[286].binOpcode = bpstrndup("\xDB", 1);
	instruction[286].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[286].opcodeOperandCount = 1;

	// Instruction Struct [287] - FISTTP
	instruction[287].instructionStr = strdup("FISTTP");
	instruction[287].undecodedStr = strdup("DD /1");
	instruction[287].dstOperandStr = strdup("m64int");
	instruction[287].dstOperand = IA_M64INT;
	instruction[287].valid64 = INVALID;
	instruction[287].compatLeg = VALID;
	instruction[287].VMX = 0;
	instruction[287].description = strdup("Store ST(0) in m64int with truncation.");
	instruction[287].binPrefixLen = 0;
	instruction[287].binOpcodeLen = 1;
	instruction[287].binOpcode = bpstrndup("\xDD", 1);
	instruction[287].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[287].opcodeOperandCount = 1;

	// Instruction Struct [288] - FLD
	instruction[288].instructionStr = strdup("FLD");
	instruction[288].undecodedStr = strdup("D9 /0");
	instruction[288].dstOperandStr = strdup("m32fp");
	instruction[288].dstOperand = IA_M32FP;
	instruction[288].valid64 = INVALID;
	instruction[288].compatLeg = VALID;
	instruction[288].VMX = 0;
	instruction[288].description = strdup("Push m32fp onto the FPU register stack.");
	instruction[288].binPrefixLen = 0;
	instruction[288].binOpcodeLen = 1;
	instruction[288].binOpcode = bpstrndup("\xD9", 1);
	instruction[288].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[288].opcodeOperandCount = 1;

	// Instruction Struct [289] - FLD
	instruction[289].instructionStr = strdup("FLD");
	instruction[289].undecodedStr = strdup("DD /0");
	instruction[289].dstOperandStr = strdup("m64fp");
	instruction[289].dstOperand = IA_M64FP;
	instruction[289].valid64 = INVALID;
	instruction[289].compatLeg = VALID;
	instruction[289].VMX = 0;
	instruction[289].description = strdup("Push m64fp onto the FPU register stack.");
	instruction[289].binPrefixLen = 0;
	instruction[289].binOpcodeLen = 1;
	instruction[289].binOpcode = bpstrndup("\xDD", 1);
	instruction[289].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[289].opcodeOperandCount = 1;

	// Instruction Struct [290] - FLD
	instruction[290].instructionStr = strdup("FLD");
	instruction[290].undecodedStr = strdup("DB /5");
	instruction[290].dstOperandStr = strdup("m80fp");
	instruction[290].dstOperand = IA_M80FP;
	instruction[290].valid64 = INVALID;
	instruction[290].compatLeg = VALID;
	instruction[290].VMX = 0;
	instruction[290].description = strdup("Push m80fp onto the FPU register stack.");
	instruction[290].binPrefixLen = 0;
	instruction[290].binOpcodeLen = 1;
	instruction[290].binOpcode = bpstrndup("\xDB", 1);
	instruction[290].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[290].opcodeOperandCount = 1;

	// Instruction Struct [291] - FLD
	instruction[291].instructionStr = strdup("FLD");
	instruction[291].undecodedStr = strdup("D9 C0 +i");
	instruction[291].dstOperandStr = strdup("ST(i)");
	instruction[291].dstOperand = IA_ST_I;
	instruction[291].valid64 = INVALID;
	instruction[291].compatLeg = VALID;
	instruction[291].VMX = 0;
	instruction[291].description = strdup("Push ST(i) onto the FPU register stack.");
	instruction[291].binPrefixLen = 0;
	instruction[291].binOpcodeLen = 2;
	instruction[291].binOpcode = bpstrndup("\xD9\xC0", 2);
	instruction[291].opcodeOperand[0] = OA_I;
	instruction[291].opcodeOperandCount = 1;

	// Instruction Struct [292] - FLD1
	instruction[292].instructionStr = strdup("FLD1");
	instruction[292].undecodedStr = strdup("D9 E8");
	instruction[292].valid64 = INVALID;
	instruction[292].compatLeg = VALID;
	instruction[292].VMX = 0;
	instruction[292].description = strdup("Push +1.0 onto the FPU register stack.");
	instruction[292].binPrefixLen = 0;
	instruction[292].binOpcodeLen = 2;
	instruction[292].binOpcode = bpstrndup("\xD9\xE8", 2);
	instruction[292].opcodeOperandCount = 0;

	// Instruction Struct [293] - FLDL2T
	instruction[293].instructionStr = strdup("FLDL2T");
	instruction[293].undecodedStr = strdup("D9 E9");
	instruction[293].valid64 = INVALID;
	instruction[293].compatLeg = VALID;
	instruction[293].VMX = 0;
	instruction[293].description = strdup("Push log210 onto the FPU register stack.");
	instruction[293].binPrefixLen = 0;
	instruction[293].binOpcodeLen = 2;
	instruction[293].binOpcode = bpstrndup("\xD9\xE9", 2);
	instruction[293].opcodeOperandCount = 0;

	// Instruction Struct [294] - FLDL2E
	instruction[294].instructionStr = strdup("FLDL2E");
	instruction[294].undecodedStr = strdup("D9 EA");
	instruction[294].valid64 = INVALID;
	instruction[294].compatLeg = VALID;
	instruction[294].VMX = 0;
	instruction[294].description = strdup("Push log2e onto the FPU register stack.");
	instruction[294].binPrefixLen = 0;
	instruction[294].binOpcodeLen = 2;
	instruction[294].binOpcode = bpstrndup("\xD9\xEA", 2);
	instruction[294].opcodeOperandCount = 0;

	// Instruction Struct [295] - FLDPI
	instruction[295].instructionStr = strdup("FLDPI");
	instruction[295].undecodedStr = strdup("D9 EB");
	instruction[295].valid64 = INVALID;
	instruction[295].compatLeg = VALID;
	instruction[295].VMX = 0;
	instruction[295].description = strdup("Push pi onto the FPU register stack.");
	instruction[295].binPrefixLen = 0;
	instruction[295].binOpcodeLen = 2;
	instruction[295].binOpcode = bpstrndup("\xD9\xEB", 2);
	instruction[295].opcodeOperandCount = 0;

	// Instruction Struct [296] - FLDLG2
	instruction[296].instructionStr = strdup("FLDLG2");
	instruction[296].undecodedStr = strdup("D9 EC");
	instruction[296].valid64 = INVALID;
	instruction[296].compatLeg = VALID;
	instruction[296].VMX = 0;
	instruction[296].description = strdup("Push log102 onto the FPU register stack.");
	instruction[296].binPrefixLen = 0;
	instruction[296].binOpcodeLen = 2;
	instruction[296].binOpcode = bpstrndup("\xD9\xEC", 2);
	instruction[296].opcodeOperandCount = 0;

	// Instruction Struct [297] - FLDLN2
	instruction[297].instructionStr = strdup("FLDLN2");
	instruction[297].undecodedStr = strdup("D9 ED");
	instruction[297].valid64 = INVALID;
	instruction[297].compatLeg = VALID;
	instruction[297].VMX = 0;
	instruction[297].description = strdup("Push loge2 onto the FPU register stack.");
	instruction[297].binPrefixLen = 0;
	instruction[297].binOpcodeLen = 2;
	instruction[297].binOpcode = bpstrndup("\xD9\xED", 2);
	instruction[297].opcodeOperandCount = 0;

	// Instruction Struct [298] - FLDZ
	instruction[298].instructionStr = strdup("FLDZ");
	instruction[298].undecodedStr = strdup("D9 EE");
	instruction[298].valid64 = INVALID;
	instruction[298].compatLeg = VALID;
	instruction[298].VMX = 0;
	instruction[298].description = strdup("Push +0.0 onto the FPU register stack.");
	instruction[298].binPrefixLen = 0;
	instruction[298].binOpcodeLen = 2;
	instruction[298].binOpcode = bpstrndup("\xD9\xEE", 2);
	instruction[298].opcodeOperandCount = 0;

	// Instruction Struct [299] - FLDCW
	instruction[299].instructionStr = strdup("FLDCW");
	instruction[299].undecodedStr = strdup("D9 /5");
	instruction[299].dstOperandStr = strdup("m2byte");
	instruction[299].dstOperand = IA_M2BYTE;
	instruction[299].valid64 = INVALID;
	instruction[299].compatLeg = VALID;
	instruction[299].VMX = 0;
	instruction[299].description = strdup("Load FPU control word from m2byte.");
	instruction[299].binPrefixLen = 0;
	instruction[299].binOpcodeLen = 1;
	instruction[299].binOpcode = bpstrndup("\xD9", 1);
	instruction[299].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[299].opcodeOperandCount = 1;

	// Instruction Struct [300] - FLDENV
	instruction[300].instructionStr = strdup("FLDENV");
	instruction[300].undecodedStr = strdup("D9 /4");
	instruction[300].dstOperandStr = strdup("m14/28byte");
	instruction[300].dstOperand = IA_M14_28BYTE;
	instruction[300].valid64 = INVALID;
	instruction[300].compatLeg = VALID;
	instruction[300].VMX = 0;
	instruction[300].description = strdup("Load FPU environment from m14byte or m28byte.");
	instruction[300].binPrefixLen = 0;
	instruction[300].binOpcodeLen = 1;
	instruction[300].binOpcode = bpstrndup("\xD9", 1);
	instruction[300].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[300].opcodeOperandCount = 1;

	// Instruction Struct [301] - FMUL
	instruction[301].instructionStr = strdup("FMUL");
	instruction[301].undecodedStr = strdup("D8 /1");
	instruction[301].dstOperandStr = strdup("m32fp");
	instruction[301].dstOperand = IA_M32FP;
	instruction[301].valid64 = INVALID;
	instruction[301].compatLeg = VALID;
	instruction[301].VMX = 0;
	instruction[301].description = strdup("Multiply ST(0) by m32fp and store result in ST(0).");
	instruction[301].binPrefixLen = 0;
	instruction[301].binOpcodeLen = 1;
	instruction[301].binOpcode = bpstrndup("\xD8", 1);
	instruction[301].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[301].opcodeOperandCount = 1;

	// Instruction Struct [302] - FMUL
	instruction[302].instructionStr = strdup("FMUL");
	instruction[302].undecodedStr = strdup("DC /1");
	instruction[302].dstOperandStr = strdup("m64fp");
	instruction[302].dstOperand = IA_M64FP;
	instruction[302].valid64 = INVALID;
	instruction[302].compatLeg = VALID;
	instruction[302].VMX = 0;
	instruction[302].description = strdup("Multiply ST(0) by m64fp and store result in ST(0).");
	instruction[302].binPrefixLen = 0;
	instruction[302].binOpcodeLen = 1;
	instruction[302].binOpcode = bpstrndup("\xDC", 1);
	instruction[302].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[302].opcodeOperandCount = 1;

	// Instruction Struct [303] - FMUL
	instruction[303].instructionStr = strdup("FMUL");
	instruction[303].undecodedStr = strdup("D8 C8 +i");
	instruction[303].dstOperandStr = strdup("ST(0)");
	instruction[303].dstOperand = IA_ST_0;
	instruction[303].srcOperandStr = strdup("ST(i)");
	instruction[303].srcOperand = IA_ST_I;
	instruction[303].valid64 = INVALID;
	instruction[303].compatLeg = VALID;
	instruction[303].VMX = 0;
	instruction[303].description = strdup("Multiply ST(0) by ST(i) and store result in ST(0).");
	instruction[303].binPrefixLen = 0;
	instruction[303].binOpcodeLen = 2;
	instruction[303].binOpcode = bpstrndup("\xD8\xC8", 2);
	instruction[303].opcodeOperand[0] = OA_I;
	instruction[303].opcodeOperandCount = 1;

	// Instruction Struct [304] - FMUL
	instruction[304].instructionStr = strdup("FMUL");
	instruction[304].undecodedStr = strdup("DC C8 +i");
	instruction[304].dstOperandStr = strdup("ST(i)");
	instruction[304].dstOperand = IA_ST_I;
	instruction[304].srcOperandStr = strdup("ST(0)");
	instruction[304].srcOperand = IA_ST_0;
	instruction[304].valid64 = INVALID;
	instruction[304].compatLeg = VALID;
	instruction[304].VMX = 0;
	instruction[304].description = strdup("Multiply ST(i) by ST(0) and store result in ST(i).");
	instruction[304].binPrefixLen = 0;
	instruction[304].binOpcodeLen = 2;
	instruction[304].binOpcode = bpstrndup("\xDC\xC8", 2);
	instruction[304].opcodeOperand[0] = OA_I;
	instruction[304].opcodeOperandCount = 1;

	// Instruction Struct [305] - FMULP
	instruction[305].instructionStr = strdup("FMULP");
	instruction[305].undecodedStr = strdup("DE C8 +i");
	instruction[305].dstOperandStr = strdup("ST(i)");
	instruction[305].dstOperand = IA_ST_I;
	instruction[305].srcOperandStr = strdup("ST(0)");
	instruction[305].srcOperand = IA_ST_0;
	instruction[305].valid64 = INVALID;
	instruction[305].compatLeg = VALID;
	instruction[305].VMX = 0;
	instruction[305].description = strdup("Multiply ST(i) by ST(0), store result in ST(i), and pop the register stack.");
	instruction[305].binPrefixLen = 0;
	instruction[305].binOpcodeLen = 2;
	instruction[305].binOpcode = bpstrndup("\xDE\xC8", 2);
	instruction[305].opcodeOperand[0] = OA_I;
	instruction[305].opcodeOperandCount = 1;

	// Instruction Struct [306] - FMULP
	instruction[306].instructionStr = strdup("FMULP");
	instruction[306].undecodedStr = strdup("DE C9");
	instruction[306].valid64 = INVALID;
	instruction[306].compatLeg = VALID;
	instruction[306].VMX = 0;
	instruction[306].description = strdup("Multiply ST(1) by ST(0), store result in ST(1), and pop the register stack.");
	instruction[306].binPrefixLen = 0;
	instruction[306].binOpcodeLen = 2;
	instruction[306].binOpcode = bpstrndup("\xDE\xC9", 2);
	instruction[306].opcodeOperandCount = 0;

	// Instruction Struct [307] - FIMUL
	instruction[307].instructionStr = strdup("FIMUL");
	instruction[307].undecodedStr = strdup("DA /1");
	instruction[307].dstOperandStr = strdup("m32int");
	instruction[307].dstOperand = IA_M32INT;
	instruction[307].valid64 = INVALID;
	instruction[307].compatLeg = VALID;
	instruction[307].VMX = 0;
	instruction[307].description = strdup("Multiply ST(0) by m32int and store result in ST(0).");
	instruction[307].binPrefixLen = 0;
	instruction[307].binOpcodeLen = 1;
	instruction[307].binOpcode = bpstrndup("\xDA", 1);
	instruction[307].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[307].opcodeOperandCount = 1;

	// Instruction Struct [308] - FIMUL
	instruction[308].instructionStr = strdup("FIMUL");
	instruction[308].undecodedStr = strdup("DE /1");
	instruction[308].dstOperandStr = strdup("m16int");
	instruction[308].dstOperand = IA_M16INT;
	instruction[308].valid64 = INVALID;
	instruction[308].compatLeg = VALID;
	instruction[308].VMX = 0;
	instruction[308].description = strdup("Multiply ST(0) by m16int and store result in ST(0).");
	instruction[308].binPrefixLen = 0;
	instruction[308].binOpcodeLen = 1;
	instruction[308].binOpcode = bpstrndup("\xDE", 1);
	instruction[308].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[308].opcodeOperandCount = 1;

	// Instruction Struct [309] - FNOP
	instruction[309].instructionStr = strdup("FNOP");
	instruction[309].undecodedStr = strdup("D9 D0");
	instruction[309].valid64 = INVALID;
	instruction[309].compatLeg = VALID;
	instruction[309].VMX = 0;
	instruction[309].description = strdup("No operation is performed.");
	instruction[309].binPrefixLen = 0;
	instruction[309].binOpcodeLen = 2;
	instruction[309].binOpcode = bpstrndup("\xD9\xD0", 2);
	instruction[309].opcodeOperandCount = 0;

	// Instruction Struct [310] - FPATAN
	instruction[310].instructionStr = strdup("FPATAN");
	instruction[310].undecodedStr = strdup("D9 F3");
	instruction[310].valid64 = INVALID;
	instruction[310].compatLeg = VALID;
	instruction[310].VMX = 0;
	instruction[310].description = strdup("Replace ST(1) with arctan(ST(1)/ST(0)) and pop the register stack.");
	instruction[310].binPrefixLen = 0;
	instruction[310].binOpcodeLen = 2;
	instruction[310].binOpcode = bpstrndup("\xD9\xF3", 2);
	instruction[310].opcodeOperandCount = 0;

	// Instruction Struct [311] - FPREM
	instruction[311].instructionStr = strdup("FPREM");
	instruction[311].undecodedStr = strdup("D9 F8");
	instruction[311].valid64 = INVALID;
	instruction[311].compatLeg = VALID;
	instruction[311].VMX = 0;
	instruction[311].description = strdup("Replace ST(0) with the remainder obtained from dividing ST(0) by ST(1).");
	instruction[311].binPrefixLen = 0;
	instruction[311].binOpcodeLen = 2;
	instruction[311].binOpcode = bpstrndup("\xD9\xF8", 2);
	instruction[311].opcodeOperandCount = 0;

	// Instruction Struct [312] - FPREM1
	instruction[312].instructionStr = strdup("FPREM1");
	instruction[312].undecodedStr = strdup("D9 F5");
	instruction[312].valid64 = INVALID;
	instruction[312].compatLeg = VALID;
	instruction[312].VMX = 0;
	instruction[312].description = strdup("Replace ST(0) with the IEEE remainder obtained from dividing ST(0) by ST(1).");
	instruction[312].binPrefixLen = 0;
	instruction[312].binOpcodeLen = 2;
	instruction[312].binOpcode = bpstrndup("\xD9\xF5", 2);
	instruction[312].opcodeOperandCount = 0;

	// Instruction Struct [313] - FPTAN
	instruction[313].instructionStr = strdup("FPTAN");
	instruction[313].undecodedStr = strdup("D9 F2");
	instruction[313].valid64 = INVALID;
	instruction[313].compatLeg = VALID;
	instruction[313].VMX = 0;
	instruction[313].description = strdup("Replace ST(0) with its tangent and push 1 onto the FPU stack.");
	instruction[313].binPrefixLen = 0;
	instruction[313].binOpcodeLen = 2;
	instruction[313].binOpcode = bpstrndup("\xD9\xF2", 2);
	instruction[313].opcodeOperandCount = 0;

	// Instruction Struct [314] - FRNDINT
	instruction[314].instructionStr = strdup("FRNDINT");
	instruction[314].undecodedStr = strdup("D9 FC");
	instruction[314].valid64 = INVALID;
	instruction[314].compatLeg = VALID;
	instruction[314].VMX = 0;
	instruction[314].description = strdup("Round ST(0) to an integer.");
	instruction[314].binPrefixLen = 0;
	instruction[314].binOpcodeLen = 2;
	instruction[314].binOpcode = bpstrndup("\xD9\xFC", 2);
	instruction[314].opcodeOperandCount = 0;

	// Instruction Struct [315] - FRSTOR
	instruction[315].instructionStr = strdup("FRSTOR");
	instruction[315].undecodedStr = strdup("DD /4");
	instruction[315].dstOperandStr = strdup("m94/108byte");
	instruction[315].dstOperand = IA_M94_108BYTE;
	instruction[315].valid64 = INVALID;
	instruction[315].compatLeg = VALID;
	instruction[315].VMX = 0;
	instruction[315].description = strdup("Load FPU state from m94byte or m108byte.");
	instruction[315].binPrefixLen = 0;
	instruction[315].binOpcodeLen = 1;
	instruction[315].binOpcode = bpstrndup("\xDD", 1);
	instruction[315].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[315].opcodeOperandCount = 1;

	// Instruction Struct [316] - FSAVE
	instruction[316].instructionStr = strdup("FSAVE");
	instruction[316].undecodedStr = strdup("9B DD /6");
	instruction[316].dstOperandStr = strdup("m94/108byte");
	instruction[316].dstOperand = IA_M94_108BYTE;
	instruction[316].valid64 = INVALID;
	instruction[316].compatLeg = VALID;
	instruction[316].VMX = 0;
	instruction[316].description = strdup("Store FPU state to m94byte or m108byte after checking for pending unmasked floatingpoint exceptions. Then reinitialize the FPU.");
	instruction[316].binPrefixLen = 0;
	instruction[316].binOpcodeLen = 2;
	instruction[316].binOpcode = bpstrndup("\x9B\xDD", 2);
	instruction[316].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[316].opcodeOperandCount = 1;

	// Instruction Struct [317] - FNSAVE
	instruction[317].instructionStr = strdup("FNSAVE");
	instruction[317].undecodedStr = strdup("DD /6");
	instruction[317].dstOperandStr = strdup("m94/108byte");
	instruction[317].dstOperand = IA_M94_108BYTE;
	instruction[317].valid64 = INVALID;
	instruction[317].compatLeg = VALID;
	instruction[317].VMX = 0;
	instruction[317].description = strdup("Store FPU environment to m94byte or m108byte without checking for pending unmasked floating-point exceptions. Then re-initialize the FPU.");
	instruction[317].binPrefixLen = 0;
	instruction[317].binOpcodeLen = 1;
	instruction[317].binOpcode = bpstrndup("\xDD", 1);
	instruction[317].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[317].opcodeOperandCount = 1;

	// Instruction Struct [318] - FSCALE
	instruction[318].instructionStr = strdup("FSCALE");
	instruction[318].undecodedStr = strdup("D9 FD");
	instruction[318].valid64 = INVALID;
	instruction[318].compatLeg = VALID;
	instruction[318].VMX = 0;
	instruction[318].description = strdup("Scale ST(0) by ST(1).");
	instruction[318].binPrefixLen = 0;
	instruction[318].binOpcodeLen = 2;
	instruction[318].binOpcode = bpstrndup("\xD9\xFD", 2);
	instruction[318].opcodeOperandCount = 0;

	// Instruction Struct [319] - FSIN
	instruction[319].instructionStr = strdup("FSIN");
	instruction[319].undecodedStr = strdup("D9 FE");
	instruction[319].valid64 = INVALID;
	instruction[319].compatLeg = VALID;
	instruction[319].VMX = 0;
	instruction[319].description = strdup("Replace ST(0) with its sine. Table 3-40. FSIN Results SRC (ST(0)) DEST (ST(0)) -? * -F -1 to +1 -0 -0 +0 +0 +F -1 to +1 +? * NaN NaN NOTES: F Means finite floating-point value. * Indicates floating-point invalid-arithmetic-operand (#IA) exception.");
	instruction[319].binPrefixLen = 0;
	instruction[319].binOpcodeLen = 2;
	instruction[319].binOpcode = bpstrndup("\xD9\xFE", 2);
	instruction[319].opcodeOperandCount = 0;

	// Instruction Struct [320] - FSINCOS
	instruction[320].instructionStr = strdup("FSINCOS");
	instruction[320].undecodedStr = strdup("D9 FB");
	instruction[320].valid64 = INVALID;
	instruction[320].compatLeg = VALID;
	instruction[320].VMX = 0;
	instruction[320].description = strdup("Compute the sine and cosine of ST(0); replace ST(0) with the sine, and push the cosine onto the register stack.");
	instruction[320].binPrefixLen = 0;
	instruction[320].binOpcodeLen = 2;
	instruction[320].binOpcode = bpstrndup("\xD9\xFB", 2);
	instruction[320].opcodeOperandCount = 0;

	// Instruction Struct [321] - FSQRT
	instruction[321].instructionStr = strdup("FSQRT");
	instruction[321].undecodedStr = strdup("D9 FA");
	instruction[321].valid64 = INVALID;
	instruction[321].compatLeg = VALID;
	instruction[321].VMX = 0;
	instruction[321].description = strdup("Computes square root of ST(0) and stores the result in ST(0).");
	instruction[321].binPrefixLen = 0;
	instruction[321].binOpcodeLen = 2;
	instruction[321].binOpcode = bpstrndup("\xD9\xFA", 2);
	instruction[321].opcodeOperandCount = 0;

	// Instruction Struct [322] - FST
	instruction[322].instructionStr = strdup("FST");
	instruction[322].undecodedStr = strdup("D9 /2");
	instruction[322].dstOperandStr = strdup("m32fp");
	instruction[322].dstOperand = IA_M32FP;
	instruction[322].valid64 = INVALID;
	instruction[322].compatLeg = VALID;
	instruction[322].VMX = 0;
	instruction[322].description = strdup("Copy ST(0) to m32fp.");
	instruction[322].binPrefixLen = 0;
	instruction[322].binOpcodeLen = 1;
	instruction[322].binOpcode = bpstrndup("\xD9", 1);
	instruction[322].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[322].opcodeOperandCount = 1;

	// Instruction Struct [323] - FST
	instruction[323].instructionStr = strdup("FST");
	instruction[323].undecodedStr = strdup("DD /2");
	instruction[323].dstOperandStr = strdup("m64fp");
	instruction[323].dstOperand = IA_M64FP;
	instruction[323].valid64 = INVALID;
	instruction[323].compatLeg = VALID;
	instruction[323].VMX = 0;
	instruction[323].description = strdup("Copy ST(0) to m64fp.");
	instruction[323].binPrefixLen = 0;
	instruction[323].binOpcodeLen = 1;
	instruction[323].binOpcode = bpstrndup("\xDD", 1);
	instruction[323].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[323].opcodeOperandCount = 1;

	// Instruction Struct [324] - FST
	instruction[324].instructionStr = strdup("FST");
	instruction[324].undecodedStr = strdup("DD D0 +i");
	instruction[324].dstOperandStr = strdup("ST(i)");
	instruction[324].dstOperand = IA_ST_I;
	instruction[324].valid64 = INVALID;
	instruction[324].compatLeg = VALID;
	instruction[324].VMX = 0;
	instruction[324].description = strdup("Copy ST(0) to ST(i).");
	instruction[324].binPrefixLen = 0;
	instruction[324].binOpcodeLen = 2;
	instruction[324].binOpcode = bpstrndup("\xDD\xD0", 2);
	instruction[324].opcodeOperand[0] = OA_I;
	instruction[324].opcodeOperandCount = 1;

	// Instruction Struct [325] - FSTP
	instruction[325].instructionStr = strdup("FSTP");
	instruction[325].undecodedStr = strdup("D9 /3");
	instruction[325].dstOperandStr = strdup("m32fp");
	instruction[325].dstOperand = IA_M32FP;
	instruction[325].valid64 = INVALID;
	instruction[325].compatLeg = VALID;
	instruction[325].VMX = 0;
	instruction[325].description = strdup("Copy ST(0) to m32fp and pop register stack.");
	instruction[325].binPrefixLen = 0;
	instruction[325].binOpcodeLen = 1;
	instruction[325].binOpcode = bpstrndup("\xD9", 1);
	instruction[325].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[325].opcodeOperandCount = 1;

	// Instruction Struct [326] - FSTP
	instruction[326].instructionStr = strdup("FSTP");
	instruction[326].undecodedStr = strdup("DD /3");
	instruction[326].dstOperandStr = strdup("m64fp");
	instruction[326].dstOperand = IA_M64FP;
	instruction[326].valid64 = INVALID;
	instruction[326].compatLeg = VALID;
	instruction[326].VMX = 0;
	instruction[326].description = strdup("Copy ST(0) to m64fp and pop register stack.");
	instruction[326].binPrefixLen = 0;
	instruction[326].binOpcodeLen = 1;
	instruction[326].binOpcode = bpstrndup("\xDD", 1);
	instruction[326].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[326].opcodeOperandCount = 1;

	// Instruction Struct [327] - FSTP
	instruction[327].instructionStr = strdup("FSTP");
	instruction[327].undecodedStr = strdup("DB /7");
	instruction[327].dstOperandStr = strdup("m80fp");
	instruction[327].dstOperand = IA_M80FP;
	instruction[327].valid64 = INVALID;
	instruction[327].compatLeg = VALID;
	instruction[327].VMX = 0;
	instruction[327].description = strdup("Copy ST(0) to m80fp and pop register stack.");
	instruction[327].binPrefixLen = 0;
	instruction[327].binOpcodeLen = 1;
	instruction[327].binOpcode = bpstrndup("\xDB", 1);
	instruction[327].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[327].opcodeOperandCount = 1;

	// Instruction Struct [328] - FSTP
	instruction[328].instructionStr = strdup("FSTP");
	instruction[328].undecodedStr = strdup("DD D8 +i");
	instruction[328].dstOperandStr = strdup("ST(i)");
	instruction[328].dstOperand = IA_ST_I;
	instruction[328].valid64 = INVALID;
	instruction[328].compatLeg = VALID;
	instruction[328].VMX = 0;
	instruction[328].description = strdup("Copy ST(0) to ST(i) and pop register stack.");
	instruction[328].binPrefixLen = 0;
	instruction[328].binOpcodeLen = 2;
	instruction[328].binOpcode = bpstrndup("\xDD\xD8", 2);
	instruction[328].opcodeOperand[0] = OA_I;
	instruction[328].opcodeOperandCount = 1;

	// Instruction Struct [329] - FSTCW
	instruction[329].instructionStr = strdup("FSTCW");
	instruction[329].undecodedStr = strdup("9B D9 /7");
	instruction[329].dstOperandStr = strdup("m2byte");
	instruction[329].dstOperand = IA_M2BYTE;
	instruction[329].valid64 = INVALID;
	instruction[329].compatLeg = VALID;
	instruction[329].VMX = 0;
	instruction[329].description = strdup("Store FPU control word to m2byte after checking for pending unmasked floating-point exceptions.");
	instruction[329].binPrefixLen = 0;
	instruction[329].binOpcodeLen = 2;
	instruction[329].binOpcode = bpstrndup("\x9B\xD9", 2);
	instruction[329].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[329].opcodeOperandCount = 1;

	// Instruction Struct [330] - FNSTCW
	instruction[330].instructionStr = strdup("FNSTCW");
	instruction[330].undecodedStr = strdup("D9 /7");
	instruction[330].dstOperandStr = strdup("m2byte");
	instruction[330].dstOperand = IA_M2BYTE;
	instruction[330].valid64 = INVALID;
	instruction[330].compatLeg = VALID;
	instruction[330].VMX = 0;
	instruction[330].description = strdup("Store FPU control word to m2byte without checking for pending unmasked floating-point exceptions.");
	instruction[330].binPrefixLen = 0;
	instruction[330].binOpcodeLen = 1;
	instruction[330].binOpcode = bpstrndup("\xD9", 1);
	instruction[330].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[330].opcodeOperandCount = 1;

	// Instruction Struct [331] - FSTENV
	instruction[331].instructionStr = strdup("FSTENV");
	instruction[331].undecodedStr = strdup("9B D9 /6");
	instruction[331].dstOperandStr = strdup("m14/28byte");
	instruction[331].dstOperand = IA_M14_28BYTE;
	instruction[331].valid64 = INVALID;
	instruction[331].compatLeg = VALID;
	instruction[331].VMX = 0;
	instruction[331].description = strdup("Store FPU environment to m14byte or m28byte after checking for pending unmasked floating-point exceptions. Then mask all floatingpoint exceptions.");
	instruction[331].binPrefixLen = 0;
	instruction[331].binOpcodeLen = 2;
	instruction[331].binOpcode = bpstrndup("\x9B\xD9", 2);
	instruction[331].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[331].opcodeOperandCount = 1;

	// Instruction Struct [332] - FNSTENV
	instruction[332].instructionStr = strdup("FNSTENV");
	instruction[332].undecodedStr = strdup("D9 /6");
	instruction[332].dstOperandStr = strdup("m14/28byte");
	instruction[332].dstOperand = IA_M14_28BYTE;
	instruction[332].valid64 = INVALID;
	instruction[332].compatLeg = VALID;
	instruction[332].VMX = 0;
	instruction[332].description = strdup("Store FPU environment to m14byte or m28byte without checking for pending unmasked floating-point exceptions. Then mask all floatingpoint exceptions.");
	instruction[332].binPrefixLen = 0;
	instruction[332].binOpcodeLen = 1;
	instruction[332].binOpcode = bpstrndup("\xD9", 1);
	instruction[332].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[332].opcodeOperandCount = 1;

	// Instruction Struct [333] - FSTSW
	instruction[333].instructionStr = strdup("FSTSW");
	instruction[333].undecodedStr = strdup("9B DD /7");
	instruction[333].dstOperandStr = strdup("m2byte");
	instruction[333].dstOperand = IA_M2BYTE;
	instruction[333].valid64 = INVALID;
	instruction[333].compatLeg = VALID;
	instruction[333].VMX = 0;
	instruction[333].description = strdup("Store FPU status word at m2byte after checking for pending unmasked floatingpoint exceptions.");
	instruction[333].binPrefixLen = 0;
	instruction[333].binOpcodeLen = 2;
	instruction[333].binOpcode = bpstrndup("\x9B\xDD", 2);
	instruction[333].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[333].opcodeOperandCount = 1;

	// Instruction Struct [334] - FSTSW
	instruction[334].instructionStr = strdup("FSTSW");
	instruction[334].undecodedStr = strdup("9B DF E0");
	instruction[334].dstOperandStr = strdup("AX");
	instruction[334].dstOperand = IA_AX;
	instruction[334].valid64 = INVALID;
	instruction[334].compatLeg = VALID;
	instruction[334].VMX = 0;
	instruction[334].description = strdup("Store FPU status word in AX register after checking for pending unmasked floatingpoint exceptions.");
	instruction[334].binPrefixLen = 0;
	instruction[334].binOpcodeLen = 3;
	instruction[334].binOpcode = bpstrndup("\x9B\xDF\xE0", 3);
	instruction[334].opcodeOperandCount = 0;

	// Instruction Struct [335] - FNSTSW
	instruction[335].instructionStr = strdup("FNSTSW");
	instruction[335].undecodedStr = strdup("DD /7");
	instruction[335].dstOperandStr = strdup("m2byte");
	instruction[335].dstOperand = IA_M2BYTE;
	instruction[335].valid64 = INVALID;
	instruction[335].compatLeg = VALID;
	instruction[335].VMX = 0;
	instruction[335].description = strdup("Store FPU status word at m2byte without checking for pending unmasked floatingpoint exceptions.");
	instruction[335].binPrefixLen = 0;
	instruction[335].binOpcodeLen = 1;
	instruction[335].binOpcode = bpstrndup("\xDD", 1);
	instruction[335].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[335].opcodeOperandCount = 1;

	// Instruction Struct [336] - FNSTSW
	instruction[336].instructionStr = strdup("FNSTSW");
	instruction[336].undecodedStr = strdup("DF E0");
	instruction[336].dstOperandStr = strdup("AX");
	instruction[336].dstOperand = IA_AX;
	instruction[336].valid64 = INVALID;
	instruction[336].compatLeg = VALID;
	instruction[336].VMX = 0;
	instruction[336].description = strdup("Store FPU status word in AX register without checking for pending unmasked floatingpoint exceptions.");
	instruction[336].binPrefixLen = 0;
	instruction[336].binOpcodeLen = 2;
	instruction[336].binOpcode = bpstrndup("\xDF\xE0", 2);
	instruction[336].opcodeOperandCount = 0;

	// Instruction Struct [337] - FSUB
	instruction[337].instructionStr = strdup("FSUB");
	instruction[337].undecodedStr = strdup("D8 /4");
	instruction[337].dstOperandStr = strdup("m32fp");
	instruction[337].dstOperand = IA_M32FP;
	instruction[337].valid64 = INVALID;
	instruction[337].compatLeg = VALID;
	instruction[337].VMX = 0;
	instruction[337].description = strdup("Subtract m32fp from ST(0) and store result in ST(0).");
	instruction[337].binPrefixLen = 0;
	instruction[337].binOpcodeLen = 1;
	instruction[337].binOpcode = bpstrndup("\xD8", 1);
	instruction[337].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[337].opcodeOperandCount = 1;

	// Instruction Struct [338] - FSUB
	instruction[338].instructionStr = strdup("FSUB");
	instruction[338].undecodedStr = strdup("DC /4");
	instruction[338].dstOperandStr = strdup("m64fp");
	instruction[338].dstOperand = IA_M64FP;
	instruction[338].valid64 = INVALID;
	instruction[338].compatLeg = VALID;
	instruction[338].VMX = 0;
	instruction[338].description = strdup("Subtract m64fp from ST(0) and store result in ST(0).");
	instruction[338].binPrefixLen = 0;
	instruction[338].binOpcodeLen = 1;
	instruction[338].binOpcode = bpstrndup("\xDC", 1);
	instruction[338].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[338].opcodeOperandCount = 1;

	// Instruction Struct [339] - FSUB
	instruction[339].instructionStr = strdup("FSUB");
	instruction[339].undecodedStr = strdup("D8 E0 +i");
	instruction[339].dstOperandStr = strdup("ST(0)");
	instruction[339].dstOperand = IA_ST_0;
	instruction[339].srcOperandStr = strdup("ST(i)");
	instruction[339].srcOperand = IA_ST_I;
	instruction[339].valid64 = INVALID;
	instruction[339].compatLeg = VALID;
	instruction[339].VMX = 0;
	instruction[339].description = strdup("Subtract ST(i) from ST(0) and store result in ST(0).");
	instruction[339].binPrefixLen = 0;
	instruction[339].binOpcodeLen = 2;
	instruction[339].binOpcode = bpstrndup("\xD8\xE0", 2);
	instruction[339].opcodeOperand[0] = OA_I;
	instruction[339].opcodeOperandCount = 1;

	// Instruction Struct [340] - FSUB
	instruction[340].instructionStr = strdup("FSUB");
	instruction[340].undecodedStr = strdup("DC E8 +i");
	instruction[340].dstOperandStr = strdup("ST(i)");
	instruction[340].dstOperand = IA_ST_I;
	instruction[340].srcOperandStr = strdup("ST(0)");
	instruction[340].srcOperand = IA_ST_0;
	instruction[340].valid64 = INVALID;
	instruction[340].compatLeg = VALID;
	instruction[340].VMX = 0;
	instruction[340].description = strdup("Subtract ST(0) from ST(i) and store result in ST(i).");
	instruction[340].binPrefixLen = 0;
	instruction[340].binOpcodeLen = 2;
	instruction[340].binOpcode = bpstrndup("\xDC\xE8", 2);
	instruction[340].opcodeOperand[0] = OA_I;
	instruction[340].opcodeOperandCount = 1;

	// Instruction Struct [341] - FSUBP
	instruction[341].instructionStr = strdup("FSUBP");
	instruction[341].undecodedStr = strdup("DE E8 +i");
	instruction[341].dstOperandStr = strdup("ST(i)");
	instruction[341].dstOperand = IA_ST_I;
	instruction[341].srcOperandStr = strdup("ST(0)");
	instruction[341].srcOperand = IA_ST_0;
	instruction[341].valid64 = INVALID;
	instruction[341].compatLeg = VALID;
	instruction[341].VMX = 0;
	instruction[341].description = strdup("Subtract ST(0) from ST(i), store result in ST(i), and pop register stack.");
	instruction[341].binPrefixLen = 0;
	instruction[341].binOpcodeLen = 2;
	instruction[341].binOpcode = bpstrndup("\xDE\xE8", 2);
	instruction[341].opcodeOperand[0] = OA_I;
	instruction[341].opcodeOperandCount = 1;

	// Instruction Struct [342] - FSUBP
	instruction[342].instructionStr = strdup("FSUBP");
	instruction[342].undecodedStr = strdup("DE E9");
	instruction[342].valid64 = INVALID;
	instruction[342].compatLeg = VALID;
	instruction[342].VMX = 0;
	instruction[342].description = strdup("Subtract ST(0) from ST(1), store result in ST(1), and pop register stack.");
	instruction[342].binPrefixLen = 0;
	instruction[342].binOpcodeLen = 2;
	instruction[342].binOpcode = bpstrndup("\xDE\xE9", 2);
	instruction[342].opcodeOperandCount = 0;

	// Instruction Struct [343] - FISUB
	instruction[343].instructionStr = strdup("FISUB");
	instruction[343].undecodedStr = strdup("DA /4");
	instruction[343].dstOperandStr = strdup("m32int");
	instruction[343].dstOperand = IA_M32INT;
	instruction[343].valid64 = INVALID;
	instruction[343].compatLeg = VALID;
	instruction[343].VMX = 0;
	instruction[343].description = strdup("Subtract m32int from ST(0) and store result in ST(0).");
	instruction[343].binPrefixLen = 0;
	instruction[343].binOpcodeLen = 1;
	instruction[343].binOpcode = bpstrndup("\xDA", 1);
	instruction[343].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[343].opcodeOperandCount = 1;

	// Instruction Struct [344] - FISUB
	instruction[344].instructionStr = strdup("FISUB");
	instruction[344].undecodedStr = strdup("DE /4");
	instruction[344].dstOperandStr = strdup("m16int");
	instruction[344].dstOperand = IA_M16INT;
	instruction[344].valid64 = INVALID;
	instruction[344].compatLeg = VALID;
	instruction[344].VMX = 0;
	instruction[344].description = strdup("Subtract m16int from ST(0) and store result in ST(0).");
	instruction[344].binPrefixLen = 0;
	instruction[344].binOpcodeLen = 1;
	instruction[344].binOpcode = bpstrndup("\xDE", 1);
	instruction[344].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[344].opcodeOperandCount = 1;

	// Instruction Struct [345] - FSUBR
	instruction[345].instructionStr = strdup("FSUBR");
	instruction[345].undecodedStr = strdup("D8 /5");
	instruction[345].dstOperandStr = strdup("m32fp");
	instruction[345].dstOperand = IA_M32FP;
	instruction[345].valid64 = INVALID;
	instruction[345].compatLeg = VALID;
	instruction[345].VMX = 0;
	instruction[345].description = strdup("Subtract ST(0) from m32fp and store result in ST(0).");
	instruction[345].binPrefixLen = 0;
	instruction[345].binOpcodeLen = 1;
	instruction[345].binOpcode = bpstrndup("\xD8", 1);
	instruction[345].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[345].opcodeOperandCount = 1;

	// Instruction Struct [346] - FSUBR
	instruction[346].instructionStr = strdup("FSUBR");
	instruction[346].undecodedStr = strdup("DC /5");
	instruction[346].dstOperandStr = strdup("m64fp");
	instruction[346].dstOperand = IA_M64FP;
	instruction[346].valid64 = INVALID;
	instruction[346].compatLeg = VALID;
	instruction[346].VMX = 0;
	instruction[346].description = strdup("Subtract ST(0) from m64fp and store result in ST(0).");
	instruction[346].binPrefixLen = 0;
	instruction[346].binOpcodeLen = 1;
	instruction[346].binOpcode = bpstrndup("\xDC", 1);
	instruction[346].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[346].opcodeOperandCount = 1;

	// Instruction Struct [347] - FSUBR
	instruction[347].instructionStr = strdup("FSUBR");
	instruction[347].undecodedStr = strdup("D8 E8 +i");
	instruction[347].dstOperandStr = strdup("ST(0)");
	instruction[347].dstOperand = IA_ST_0;
	instruction[347].srcOperandStr = strdup("ST(i)");
	instruction[347].srcOperand = IA_ST_I;
	instruction[347].valid64 = INVALID;
	instruction[347].compatLeg = VALID;
	instruction[347].VMX = 0;
	instruction[347].description = strdup("Subtract ST(0) from ST(i) and store result in ST(0).");
	instruction[347].binPrefixLen = 0;
	instruction[347].binOpcodeLen = 2;
	instruction[347].binOpcode = bpstrndup("\xD8\xE8", 2);
	instruction[347].opcodeOperand[0] = OA_I;
	instruction[347].opcodeOperandCount = 1;

	// Instruction Struct [348] - FSUBR
	instruction[348].instructionStr = strdup("FSUBR");
	instruction[348].undecodedStr = strdup("DC E0 +i");
	instruction[348].dstOperandStr = strdup("ST(i)");
	instruction[348].dstOperand = IA_ST_I;
	instruction[348].srcOperandStr = strdup("ST(0)");
	instruction[348].srcOperand = IA_ST_0;
	instruction[348].valid64 = INVALID;
	instruction[348].compatLeg = VALID;
	instruction[348].VMX = 0;
	instruction[348].description = strdup("Subtract ST(i) from ST(0) and store result in ST(i).");
	instruction[348].binPrefixLen = 0;
	instruction[348].binOpcodeLen = 2;
	instruction[348].binOpcode = bpstrndup("\xDC\xE0", 2);
	instruction[348].opcodeOperand[0] = OA_I;
	instruction[348].opcodeOperandCount = 1;

	// Instruction Struct [349] - FSUBRP
	instruction[349].instructionStr = strdup("FSUBRP");
	instruction[349].undecodedStr = strdup("DE E0 +i");
	instruction[349].dstOperandStr = strdup("ST(i)");
	instruction[349].dstOperand = IA_ST_I;
	instruction[349].srcOperandStr = strdup("ST(0)");
	instruction[349].srcOperand = IA_ST_0;
	instruction[349].valid64 = INVALID;
	instruction[349].compatLeg = VALID;
	instruction[349].VMX = 0;
	instruction[349].description = strdup("Subtract ST(i) from ST(0), store result in ST(i), and pop register stack.");
	instruction[349].binPrefixLen = 0;
	instruction[349].binOpcodeLen = 2;
	instruction[349].binOpcode = bpstrndup("\xDE\xE0", 2);
	instruction[349].opcodeOperand[0] = OA_I;
	instruction[349].opcodeOperandCount = 1;

	// Instruction Struct [350] - FSUBRP
	instruction[350].instructionStr = strdup("FSUBRP");
	instruction[350].undecodedStr = strdup("DE E1");
	instruction[350].valid64 = INVALID;
	instruction[350].compatLeg = VALID;
	instruction[350].VMX = 0;
	instruction[350].description = strdup("Subtract ST(1) from ST(0), store result in ST(1), and pop register stack.");
	instruction[350].binPrefixLen = 0;
	instruction[350].binOpcodeLen = 2;
	instruction[350].binOpcode = bpstrndup("\xDE\xE1", 2);
	instruction[350].opcodeOperandCount = 0;

	// Instruction Struct [351] - FISUBR
	instruction[351].instructionStr = strdup("FISUBR");
	instruction[351].undecodedStr = strdup("DA /5");
	instruction[351].dstOperandStr = strdup("m32int");
	instruction[351].dstOperand = IA_M32INT;
	instruction[351].valid64 = INVALID;
	instruction[351].compatLeg = VALID;
	instruction[351].VMX = 0;
	instruction[351].description = strdup("Subtract ST(0) from m32int and store result in ST(0).");
	instruction[351].binPrefixLen = 0;
	instruction[351].binOpcodeLen = 1;
	instruction[351].binOpcode = bpstrndup("\xDA", 1);
	instruction[351].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[351].opcodeOperandCount = 1;

	// Instruction Struct [352] - FISUBR
	instruction[352].instructionStr = strdup("FISUBR");
	instruction[352].undecodedStr = strdup("DE /5");
	instruction[352].dstOperandStr = strdup("m16int");
	instruction[352].dstOperand = IA_M16INT;
	instruction[352].valid64 = INVALID;
	instruction[352].compatLeg = VALID;
	instruction[352].VMX = 0;
	instruction[352].description = strdup("Subtract ST(0) from m16int and store result in ST(0).");
	instruction[352].binPrefixLen = 0;
	instruction[352].binOpcodeLen = 1;
	instruction[352].binOpcode = bpstrndup("\xDE", 1);
	instruction[352].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[352].opcodeOperandCount = 1;

	// Instruction Struct [353] - FTST
	instruction[353].instructionStr = strdup("FTST");
	instruction[353].undecodedStr = strdup("D9 E4");
	instruction[353].valid64 = INVALID;
	instruction[353].compatLeg = VALID;
	instruction[353].VMX = 0;
	instruction[353].description = strdup("Compare ST(0) with 0.0.");
	instruction[353].binPrefixLen = 0;
	instruction[353].binOpcodeLen = 2;
	instruction[353].binOpcode = bpstrndup("\xD9\xE4", 2);
	instruction[353].opcodeOperandCount = 0;

	// Instruction Struct [354] - FUCOM
	instruction[354].instructionStr = strdup("FUCOM");
	instruction[354].undecodedStr = strdup("DD E0 +i");
	instruction[354].dstOperandStr = strdup("ST(i)");
	instruction[354].dstOperand = IA_ST_I;
	instruction[354].valid64 = INVALID;
	instruction[354].compatLeg = VALID;
	instruction[354].VMX = 0;
	instruction[354].description = strdup("Compare ST(0) with ST(i).");
	instruction[354].binPrefixLen = 0;
	instruction[354].binOpcodeLen = 2;
	instruction[354].binOpcode = bpstrndup("\xDD\xE0", 2);
	instruction[354].opcodeOperand[0] = OA_I;
	instruction[354].opcodeOperandCount = 1;

	// Instruction Struct [355] - FUCOM
	instruction[355].instructionStr = strdup("FUCOM");
	instruction[355].undecodedStr = strdup("DD E1");
	instruction[355].valid64 = INVALID;
	instruction[355].compatLeg = VALID;
	instruction[355].VMX = 0;
	instruction[355].description = strdup("Compare ST(0) with ST(1).");
	instruction[355].binPrefixLen = 0;
	instruction[355].binOpcodeLen = 2;
	instruction[355].binOpcode = bpstrndup("\xDD\xE1", 2);
	instruction[355].opcodeOperandCount = 0;

	// Instruction Struct [356] - FUCOMP
	instruction[356].instructionStr = strdup("FUCOMP");
	instruction[356].undecodedStr = strdup("DD E8 +i");
	instruction[356].dstOperandStr = strdup("ST(i)");
	instruction[356].dstOperand = IA_ST_I;
	instruction[356].valid64 = INVALID;
	instruction[356].compatLeg = VALID;
	instruction[356].VMX = 0;
	instruction[356].description = strdup("Compare ST(0) with ST(i) and pop register stack.");
	instruction[356].binPrefixLen = 0;
	instruction[356].binOpcodeLen = 2;
	instruction[356].binOpcode = bpstrndup("\xDD\xE8", 2);
	instruction[356].opcodeOperand[0] = OA_I;
	instruction[356].opcodeOperandCount = 1;

	// Instruction Struct [357] - FUCOMP
	instruction[357].instructionStr = strdup("FUCOMP");
	instruction[357].undecodedStr = strdup("DD E9");
	instruction[357].valid64 = INVALID;
	instruction[357].compatLeg = VALID;
	instruction[357].VMX = 0;
	instruction[357].description = strdup("Compare ST(0) with ST(1) and pop register stack.");
	instruction[357].binPrefixLen = 0;
	instruction[357].binOpcodeLen = 2;
	instruction[357].binOpcode = bpstrndup("\xDD\xE9", 2);
	instruction[357].opcodeOperandCount = 0;

	// Instruction Struct [358] - FUCOMPP
	instruction[358].instructionStr = strdup("FUCOMPP");
	instruction[358].undecodedStr = strdup("DA E9");
	instruction[358].valid64 = INVALID;
	instruction[358].compatLeg = VALID;
	instruction[358].VMX = 0;
	instruction[358].description = strdup("Compare ST(0) with ST(1) and pop register stack twice.");
	instruction[358].binPrefixLen = 0;
	instruction[358].binOpcodeLen = 2;
	instruction[358].binOpcode = bpstrndup("\xDA\xE9", 2);
	instruction[358].opcodeOperandCount = 0;

	// Instruction Struct [359] - FXAM
	instruction[359].instructionStr = strdup("FXAM");
	instruction[359].undecodedStr = strdup("D9 E5");
	instruction[359].valid64 = INVALID;
	instruction[359].compatLeg = VALID;
	instruction[359].VMX = 0;
	instruction[359].description = strdup("Classify value or number in ST(0).");
	instruction[359].binPrefixLen = 0;
	instruction[359].binOpcodeLen = 2;
	instruction[359].binOpcode = bpstrndup("\xD9\xE5", 2);
	instruction[359].opcodeOperandCount = 0;

	// Instruction Struct [360] - FXCH
	instruction[360].instructionStr = strdup("FXCH");
	instruction[360].undecodedStr = strdup("D9 C8 +i");
	instruction[360].dstOperandStr = strdup("ST(i)");
	instruction[360].dstOperand = IA_ST_I;
	instruction[360].valid64 = INVALID;
	instruction[360].compatLeg = VALID;
	instruction[360].VMX = 0;
	instruction[360].description = strdup("Exchange the contents of ST(0) and ST(i).");
	instruction[360].binPrefixLen = 0;
	instruction[360].binOpcodeLen = 2;
	instruction[360].binOpcode = bpstrndup("\xD9\xC8", 2);
	instruction[360].opcodeOperand[0] = OA_I;
	instruction[360].opcodeOperandCount = 1;

	// Instruction Struct [361] - FXCH
	instruction[361].instructionStr = strdup("FXCH");
	instruction[361].undecodedStr = strdup("D9 C9");
	instruction[361].valid64 = INVALID;
	instruction[361].compatLeg = VALID;
	instruction[361].VMX = 0;
	instruction[361].description = strdup("Exchange the contents of ST(0) and ST(1).");
	instruction[361].binPrefixLen = 0;
	instruction[361].binOpcodeLen = 2;
	instruction[361].binOpcode = bpstrndup("\xD9\xC9", 2);
	instruction[361].opcodeOperandCount = 0;

	// Instruction Struct [362] - FXRSTOR
	instruction[362].instructionStr = strdup("FXRSTOR");
	instruction[362].undecodedStr = strdup("0F AE /1");
	instruction[362].dstOperandStr = strdup("m512byte");
	instruction[362].dstOperand = IA_M512BYTE;
	instruction[362].valid64 = INVALID;
	instruction[362].compatLeg = VALID;
	instruction[362].VMX = 0;
	instruction[362].description = strdup("Restore the x87 FPU, MMX, XMM, and MXCSR register state from m512byte.");
	instruction[362].binPrefixLen = 0;
	instruction[362].binOpcodeLen = 2;
	instruction[362].binOpcode = bpstrndup("\x0F\xAE", 2);
	instruction[362].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[362].opcodeOperandCount = 1;

	// Instruction Struct [363] - FXSAVE
	instruction[363].instructionStr = strdup("FXSAVE");
	instruction[363].undecodedStr = strdup("0F AE /0");
	instruction[363].dstOperandStr = strdup("m512byte");
	instruction[363].dstOperand = IA_M512BYTE;
	instruction[363].valid64 = INVALID;
	instruction[363].compatLeg = VALID;
	instruction[363].VMX = 0;
	instruction[363].description = strdup("Save the x87 FPU, MMX, XMM, and MXCSR register state to m512byte.");
	instruction[363].binPrefixLen = 0;
	instruction[363].binOpcodeLen = 2;
	instruction[363].binOpcode = bpstrndup("\x0F\xAE", 2);
	instruction[363].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[363].opcodeOperandCount = 1;

	// Instruction Struct [364] - FXTRACT
	instruction[364].instructionStr = strdup("FXTRACT");
	instruction[364].undecodedStr = strdup("D9 F4");
	instruction[364].valid64 = INVALID;
	instruction[364].compatLeg = VALID;
	instruction[364].VMX = 0;
	instruction[364].description = strdup("Separate value in ST(0) into exponent and significand, store exponent in ST(0), and push the significand onto the register stack.");
	instruction[364].binPrefixLen = 0;
	instruction[364].binOpcodeLen = 2;
	instruction[364].binOpcode = bpstrndup("\xD9\xF4", 2);
	instruction[364].opcodeOperandCount = 0;

	// Instruction Struct [365] - FYL2X
	instruction[365].instructionStr = strdup("FYL2X");
	instruction[365].undecodedStr = strdup("D9 F1");
	instruction[365].valid64 = INVALID;
	instruction[365].compatLeg = VALID;
	instruction[365].VMX = 0;
	instruction[365].description = strdup("Replace ST(1) with (ST(1) ? log2ST(0)) and pop the register stack.");
	instruction[365].binPrefixLen = 0;
	instruction[365].binOpcodeLen = 2;
	instruction[365].binOpcode = bpstrndup("\xD9\xF1", 2);
	instruction[365].opcodeOperandCount = 0;

	// Instruction Struct [366] - FYL2XP1
	instruction[366].instructionStr = strdup("FYL2XP1");
	instruction[366].undecodedStr = strdup("D9 F9");
	instruction[366].valid64 = INVALID;
	instruction[366].compatLeg = VALID;
	instruction[366].VMX = 0;
	instruction[366].description = strdup("Replace ST(1) with ST(1) ? log2(ST(0) + 1.0) and pop the register stack.");
	instruction[366].binPrefixLen = 0;
	instruction[366].binOpcodeLen = 2;
	instruction[366].binOpcode = bpstrndup("\xD9\xF9", 2);
	instruction[366].opcodeOperandCount = 0;

	// Instruction Struct [367] - HADDPD
	instruction[367].instructionStr = strdup("HADDPD");
	instruction[367].undecodedStr = strdup("66 0F 7C /r");
	instruction[367].dstOperandStr = strdup("xmm1");
	instruction[367].dstOperand = IA_XMM;
	instruction[367].srcOperandStr = strdup("xmm2/m128");
	instruction[367].srcOperand = IA_XMM_M128;
	instruction[367].valid64 = INVALID;
	instruction[367].compatLeg = VALID;
	instruction[367].VMX = 0;
	instruction[367].description = strdup("Horizontal add packed doubleprecision floating-point values from xmm2/m128 to xmm1.");
	instruction[367].prefixStr = strdup("OPSIZEOVR");
	instruction[367].binPrefixLen = 1;
	instruction[367].binPrefix = bpstrndup("\x66", 1);
	instruction[367].binOpcodeLen = 2;
	instruction[367].binOpcode = bpstrndup("\x0F\x7C", 2);
	instruction[367].opcodeOperand[0] = OA_MODRM;
	instruction[367].opcodeOperandCount = 1;

	// Instruction Struct [368] - HADDPS
	instruction[368].instructionStr = strdup("HADDPS");
	instruction[368].undecodedStr = strdup("F2 0F 7C /r");
	instruction[368].dstOperandStr = strdup("xmm1");
	instruction[368].dstOperand = IA_XMM;
	instruction[368].srcOperandStr = strdup("xmm2/m128");
	instruction[368].srcOperand = IA_XMM_M128;
	instruction[368].valid64 = INVALID;
	instruction[368].compatLeg = VALID;
	instruction[368].VMX = 0;
	instruction[368].description = strdup("Horizontal add packed singleprecision floating-point values from xmm2/m128 to xmm1.");
	instruction[368].prefixStr = strdup("REPNE");
	instruction[368].binPrefixLen = 1;
	instruction[368].binPrefix = bpstrndup("\xF2", 1);
	instruction[368].binOpcodeLen = 2;
	instruction[368].binOpcode = bpstrndup("\x0F\x7C", 2);
	instruction[368].opcodeOperand[0] = OA_MODRM;
	instruction[368].opcodeOperandCount = 1;

	// Instruction Struct [369] - HLT
	instruction[369].instructionStr = strdup("HLT");
	instruction[369].undecodedStr = strdup("F4");
	instruction[369].valid64 = INVALID;
	instruction[369].compatLeg = VALID;
	instruction[369].VMX = 0;
	instruction[369].description = strdup("Stops instruction execution and places the processor in a HALT state.");
	instruction[369].binPrefixLen = 0;
	instruction[369].binOpcodeLen = 1;
	instruction[369].binOpcode = bpstrndup("\xF4", 1);
	instruction[369].opcodeOperandCount = 0;

	// Instruction Struct [370] - HSUBPD
	instruction[370].instructionStr = strdup("HSUBPD");
	instruction[370].undecodedStr = strdup("66 0F 7D /r");
	instruction[370].dstOperandStr = strdup("xmm1");
	instruction[370].dstOperand = IA_XMM;
	instruction[370].srcOperandStr = strdup("xmm2/m128");
	instruction[370].srcOperand = IA_XMM_M128;
	instruction[370].valid64 = INVALID;
	instruction[370].compatLeg = VALID;
	instruction[370].VMX = 0;
	instruction[370].description = strdup("Horizontal subtract packed doubleprecision floating-point values from xmm2/m128 to xmm1.");
	instruction[370].prefixStr = strdup("OPSIZEOVR");
	instruction[370].binPrefixLen = 1;
	instruction[370].binPrefix = bpstrndup("\x66", 1);
	instruction[370].binOpcodeLen = 2;
	instruction[370].binOpcode = bpstrndup("\x0F\x7D", 2);
	instruction[370].opcodeOperand[0] = OA_MODRM;
	instruction[370].opcodeOperandCount = 1;

	// Instruction Struct [371] - HSUBPS
	instruction[371].instructionStr = strdup("HSUBPS");
	instruction[371].undecodedStr = strdup("F2 0F 7D /r");
	instruction[371].dstOperandStr = strdup("xmm1");
	instruction[371].dstOperand = IA_XMM;
	instruction[371].srcOperandStr = strdup("xmm2/m128");
	instruction[371].srcOperand = IA_XMM_M128;
	instruction[371].valid64 = INVALID;
	instruction[371].compatLeg = VALID;
	instruction[371].VMX = 0;
	instruction[371].description = strdup("Horizontal subtract packed singleprecision floating-point values from xmm2/m128 to xmm1.");
	instruction[371].prefixStr = strdup("REPNE");
	instruction[371].binPrefixLen = 1;
	instruction[371].binPrefix = bpstrndup("\xF2", 1);
	instruction[371].binOpcodeLen = 2;
	instruction[371].binOpcode = bpstrndup("\x0F\x7D", 2);
	instruction[371].opcodeOperand[0] = OA_MODRM;
	instruction[371].opcodeOperandCount = 1;

	// Instruction Struct [372] - IDIV
	instruction[372].instructionStr = strdup("IDIV");
	instruction[372].undecodedStr = strdup("F6 /7");
	instruction[372].dstOperandStr = strdup("r/m8");
	instruction[372].dstOperand = IA_RM8;
	instruction[372].valid64 = INVALID;
	instruction[372].compatLeg = VALID;
	instruction[372].VMX = 0;
	instruction[372].description = strdup("Signed divide AX by r/m8, with result stored in: AL ? Quotient, AH ? Remainder.");
	instruction[372].binPrefixLen = 0;
	instruction[372].binOpcodeLen = 1;
	instruction[372].binOpcode = bpstrndup("\xF6", 1);
	instruction[372].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[372].opcodeOperandCount = 1;

	// Instruction Struct [373] - IDIV
	instruction[373].instructionStr = strdup("IDIV");
	instruction[373].undecodedStr = strdup("F7 /7");
	instruction[373].dstOperandStr = strdup("r/m16");
	instruction[373].dstOperand = IA_RM16;
	instruction[373].valid64 = INVALID;
	instruction[373].compatLeg = VALID;
	instruction[373].VMX = 0;
	instruction[373].description = strdup("Signed divide DX:AX by r/m16, with result stored in AX ? Quotient, DX ? Remainder.");
	instruction[373].binPrefixLen = 0;
	instruction[373].binOpcodeLen = 1;
	instruction[373].binOpcode = bpstrndup("\xF7", 1);
	instruction[373].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[373].opcodeOperandCount = 1;

	// Instruction Struct [374] - IDIV
	instruction[374].instructionStr = strdup("IDIV");
	instruction[374].undecodedStr = strdup("F7 /7");
	instruction[374].dstOperandStr = strdup("r/m32");
	instruction[374].dstOperand = IA_RM32;
	instruction[374].valid64 = INVALID;
	instruction[374].compatLeg = VALID;
	instruction[374].VMX = 0;
	instruction[374].description = strdup("Signed divide EDX:EAX by r/m32, with result stored in EAX ? Quotient, EDX ? Remainder.");
	instruction[374].binPrefixLen = 0;
	instruction[374].binOpcodeLen = 1;
	instruction[374].binOpcode = bpstrndup("\xF7", 1);
	instruction[374].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[374].opcodeOperandCount = 1;

	// Instruction Struct [375] - IMUL
	instruction[375].instructionStr = strdup("IMUL");
	instruction[375].undecodedStr = strdup("F6 /5");
	instruction[375].dstOperandStr = strdup("r/m8");
	instruction[375].dstOperand = IA_RM8;
	instruction[375].valid64 = INVALID;
	instruction[375].compatLeg = VALID;
	instruction[375].VMX = 0;
	instruction[375].description = strdup("AX? AL ? r/m byte.");
	instruction[375].binPrefixLen = 0;
	instruction[375].binOpcodeLen = 1;
	instruction[375].binOpcode = bpstrndup("\xF6", 1);
	instruction[375].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[375].opcodeOperandCount = 1;

	// Instruction Struct [376] - MUL
	instruction[376].instructionStr = strdup("MUL");
	instruction[376].undecodedStr = strdup("F7 /5");
	instruction[376].dstOperandStr = strdup("r/m16");
	instruction[376].dstOperand = IA_RM16;
	instruction[376].valid64 = INVALID;
	instruction[376].compatLeg = VALID;
	instruction[376].VMX = 0;
	instruction[376].description = strdup("DX:AX ? AX ? r/m word.");
	instruction[376].binPrefixLen = 0;
	instruction[376].binOpcodeLen = 1;
	instruction[376].binOpcode = bpstrndup("\xF7", 1);
	instruction[376].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[376].opcodeOperandCount = 1;

	// Instruction Struct [377] - IMUL
	instruction[377].instructionStr = strdup("IMUL");
	instruction[377].undecodedStr = strdup("F7 /5");
	instruction[377].dstOperandStr = strdup("r/m32");
	instruction[377].dstOperand = IA_RM32;
	instruction[377].valid64 = INVALID;
	instruction[377].compatLeg = VALID;
	instruction[377].VMX = 0;
	instruction[377].description = strdup("EDX:EAX ? EAX ? r/m32.");
	instruction[377].binPrefixLen = 0;
	instruction[377].binOpcodeLen = 1;
	instruction[377].binOpcode = bpstrndup("\xF7", 1);
	instruction[377].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[377].opcodeOperandCount = 1;

	// Instruction Struct [378] - IMUL
	instruction[378].instructionStr = strdup("IMUL");
	instruction[378].undecodedStr = strdup("0F AF /r");
	instruction[378].dstOperandStr = strdup("r16");
	instruction[378].dstOperand = IA_R16;
	instruction[378].srcOperandStr = strdup("r/m16");
	instruction[378].srcOperand = IA_RM16;
	instruction[378].valid64 = INVALID;
	instruction[378].compatLeg = VALID;
	instruction[378].VMX = 0;
	instruction[378].description = strdup("word register ? word register ? r/m16.");
	instruction[378].binPrefixLen = 0;
	instruction[378].binOpcodeLen = 2;
	instruction[378].binOpcode = bpstrndup("\x0F\xAF", 2);
	instruction[378].opcodeOperand[0] = OA_MODRM;
	instruction[378].opcodeOperandCount = 1;

	// Instruction Struct [379] - IMUL
	instruction[379].instructionStr = strdup("IMUL");
	instruction[379].undecodedStr = strdup("0F AF /r");
	instruction[379].dstOperandStr = strdup("r32");
	instruction[379].dstOperand = IA_R32;
	instruction[379].srcOperandStr = strdup("r/m32");
	instruction[379].srcOperand = IA_RM32;
	instruction[379].valid64 = INVALID;
	instruction[379].compatLeg = VALID;
	instruction[379].VMX = 0;
	instruction[379].description = strdup("doubleword register ? doubleword register ? r/m32.");
	instruction[379].binPrefixLen = 0;
	instruction[379].binOpcodeLen = 2;
	instruction[379].binOpcode = bpstrndup("\x0F\xAF", 2);
	instruction[379].opcodeOperand[0] = OA_MODRM;
	instruction[379].opcodeOperandCount = 1;

	// Instruction Struct [380] - IMUL
	instruction[380].instructionStr = strdup("IMUL");
	instruction[380].undecodedStr = strdup("6B /r ib");
	instruction[380].dstOperandStr = strdup("r16");
	instruction[380].dstOperand = IA_R16;
	instruction[380].srcOperandStr = strdup("r/m16");
	instruction[380].srcOperand = IA_RM16;
	instruction[380].thirdOperandStr = strdup("imm8");
	instruction[380].thirdOperand = IA_IMM8;
	instruction[380].valid64 = INVALID;
	instruction[380].compatLeg = VALID;
	instruction[380].VMX = 0;
	instruction[380].description = strdup("word register ? r/m16 ? signextended immediate byte.");
	instruction[380].binPrefixLen = 0;
	instruction[380].binOpcodeLen = 1;
	instruction[380].binOpcode = bpstrndup("\x6B", 1);
	instruction[380].opcodeOperand[0] = OA_MODRM;
	instruction[380].opcodeOperand[1] = OA_IB;
	instruction[380].opcodeOperandCount = 2;

	// Instruction Struct [381] - IMUL
	instruction[381].instructionStr = strdup("IMUL");
	instruction[381].undecodedStr = strdup("6B /r ib");
	instruction[381].dstOperandStr = strdup("r32");
	instruction[381].dstOperand = IA_R32;
	instruction[381].srcOperandStr = strdup("r/m32");
	instruction[381].srcOperand = IA_RM32;
	instruction[381].thirdOperandStr = strdup("imm8");
	instruction[381].thirdOperand = IA_IMM8;
	instruction[381].valid64 = INVALID;
	instruction[381].compatLeg = VALID;
	instruction[381].VMX = 0;
	instruction[381].description = strdup("doubleword register ? r/m32 ? sign-extended immediate byte.");
	instruction[381].binPrefixLen = 0;
	instruction[381].binOpcodeLen = 1;
	instruction[381].binOpcode = bpstrndup("\x6B", 1);
	instruction[381].opcodeOperand[0] = OA_MODRM;
	instruction[381].opcodeOperand[1] = OA_IB;
	instruction[381].opcodeOperandCount = 2;

	// Instruction Struct [382] - IMUL
	instruction[382].instructionStr = strdup("IMUL");
	instruction[382].undecodedStr = strdup("6B /r ib");
	instruction[382].dstOperandStr = strdup("r16");
	instruction[382].dstOperand = IA_R16;
	instruction[382].srcOperandStr = strdup("imm8");
	instruction[382].srcOperand = IA_IMM8;
	instruction[382].valid64 = INVALID;
	instruction[382].compatLeg = VALID;
	instruction[382].VMX = 0;
	instruction[382].description = strdup("word register ? word register ? sign-extended immediate byte.");
	instruction[382].binPrefixLen = 0;
	instruction[382].binOpcodeLen = 1;
	instruction[382].binOpcode = bpstrndup("\x6B", 1);
	instruction[382].opcodeOperand[0] = OA_MODRM;
	instruction[382].opcodeOperand[1] = OA_IB;
	instruction[382].opcodeOperandCount = 2;

	// Instruction Struct [383] - IMUL
	instruction[383].instructionStr = strdup("IMUL");
	instruction[383].undecodedStr = strdup("6B /r ib");
	instruction[383].dstOperandStr = strdup("r32");
	instruction[383].dstOperand = IA_R32;
	instruction[383].srcOperandStr = strdup("imm8");
	instruction[383].srcOperand = IA_IMM8;
	instruction[383].valid64 = INVALID;
	instruction[383].compatLeg = VALID;
	instruction[383].VMX = 0;
	instruction[383].description = strdup("doubleword register ? doubleword register ? signextended immediate byte.");
	instruction[383].binPrefixLen = 0;
	instruction[383].binOpcodeLen = 1;
	instruction[383].binOpcode = bpstrndup("\x6B", 1);
	instruction[383].opcodeOperand[0] = OA_MODRM;
	instruction[383].opcodeOperand[1] = OA_IB;
	instruction[383].opcodeOperandCount = 2;

	// Instruction Struct [384] - IMUL
	instruction[384].instructionStr = strdup("IMUL");
	instruction[384].undecodedStr = strdup("69 /r iw");
	instruction[384].dstOperandStr = strdup("r16");
	instruction[384].dstOperand = IA_R16;
	instruction[384].srcOperandStr = strdup("r/m16");
	instruction[384].srcOperand = IA_RM16;
	instruction[384].thirdOperandStr = strdup("imm16");
	instruction[384].thirdOperand = IA_IMM16;
	instruction[384].valid64 = INVALID;
	instruction[384].compatLeg = VALID;
	instruction[384].VMX = 0;
	instruction[384].description = strdup("word register ? r/m16 ? immediate word.");
	instruction[384].binPrefixLen = 0;
	instruction[384].binOpcodeLen = 1;
	instruction[384].binOpcode = bpstrndup("\x69", 1);
	instruction[384].opcodeOperand[0] = OA_MODRM;
	instruction[384].opcodeOperand[1] = OA_IW;
	instruction[384].opcodeOperandCount = 2;

	// Instruction Struct [385] - IMUL
	instruction[385].instructionStr = strdup("IMUL");
	instruction[385].undecodedStr = strdup("69 /r id");
	instruction[385].dstOperandStr = strdup("r32");
	instruction[385].dstOperand = IA_R32;
	instruction[385].srcOperandStr = strdup("r/m32");
	instruction[385].srcOperand = IA_RM32;
	instruction[385].thirdOperandStr = strdup("imm32");
	instruction[385].thirdOperand = IA_IMM32;
	instruction[385].valid64 = INVALID;
	instruction[385].compatLeg = VALID;
	instruction[385].VMX = 0;
	instruction[385].description = strdup("doubleword register ? r/m32 ? immediate doubleword.");
	instruction[385].binPrefixLen = 0;
	instruction[385].binOpcodeLen = 1;
	instruction[385].binOpcode = bpstrndup("\x69", 1);
	instruction[385].opcodeOperand[0] = OA_MODRM;
	instruction[385].opcodeOperand[1] = OA_ID;
	instruction[385].opcodeOperandCount = 2;

	// Instruction Struct [386] - IMUL
	instruction[386].instructionStr = strdup("IMUL");
	instruction[386].undecodedStr = strdup("69 /r iw");
	instruction[386].dstOperandStr = strdup("r16");
	instruction[386].dstOperand = IA_R16;
	instruction[386].srcOperandStr = strdup("imm16");
	instruction[386].srcOperand = IA_IMM16;
	instruction[386].valid64 = INVALID;
	instruction[386].compatLeg = VALID;
	instruction[386].VMX = 0;
	instruction[386].description = strdup("word register ? r/m16 ? immediate word.");
	instruction[386].binPrefixLen = 0;
	instruction[386].binOpcodeLen = 1;
	instruction[386].binOpcode = bpstrndup("\x69", 1);
	instruction[386].opcodeOperand[0] = OA_MODRM;
	instruction[386].opcodeOperand[1] = OA_IW;
	instruction[386].opcodeOperandCount = 2;

	// Instruction Struct [387] - IMUL
	instruction[387].instructionStr = strdup("IMUL");
	instruction[387].undecodedStr = strdup("69 /r id");
	instruction[387].dstOperandStr = strdup("r32");
	instruction[387].dstOperand = IA_R32;
	instruction[387].srcOperandStr = strdup("imm32");
	instruction[387].srcOperand = IA_IMM32;
	instruction[387].valid64 = INVALID;
	instruction[387].compatLeg = VALID;
	instruction[387].VMX = 0;
	instruction[387].description = strdup("doubleword register ? r/m32 ?");
	instruction[387].binPrefixLen = 0;
	instruction[387].binOpcodeLen = 1;
	instruction[387].binOpcode = bpstrndup("\x69", 1);
	instruction[387].opcodeOperand[0] = OA_MODRM;
	instruction[387].opcodeOperand[1] = OA_ID;
	instruction[387].opcodeOperandCount = 2;

	// Instruction Struct [388] - MUL
	instruction[388].instructionStr = strdup("MUL");
	instruction[388].undecodedStr = strdup("69 /r id");
	instruction[388].dstOperandStr = strdup("r32");
	instruction[388].dstOperand = IA_R32;
	instruction[388].srcOperandStr = strdup("imm32");
	instruction[388].srcOperand = IA_IMM32;
	instruction[388].valid64 = INVALID;
	instruction[388].compatLeg = VALID;
	instruction[388].VMX = 0;
	instruction[388].description = strdup("doubleword register ? r/m32 ? immediate doubleword.");
	instruction[388].binPrefixLen = 0;
	instruction[388].binOpcodeLen = 1;
	instruction[388].binOpcode = bpstrndup("\x69", 1);
	instruction[388].opcodeOperand[0] = OA_MODRM;
	instruction[388].opcodeOperand[1] = OA_ID;
	instruction[388].opcodeOperandCount = 2;

	// Instruction Struct [389] - IN
	instruction[389].instructionStr = strdup("IN");
	instruction[389].undecodedStr = strdup("E4 ib");
	instruction[389].dstOperandStr = strdup("AL");
	instruction[389].dstOperand = IA_AL;
	instruction[389].srcOperandStr = strdup("imm8");
	instruction[389].srcOperand = IA_IMM8;
	instruction[389].valid64 = INVALID;
	instruction[389].compatLeg = VALID;
	instruction[389].VMX = 0;
	instruction[389].description = strdup("Input byte from imm8 I/O port address into AL.");
	instruction[389].binPrefixLen = 0;
	instruction[389].binOpcodeLen = 1;
	instruction[389].binOpcode = bpstrndup("\xE4", 1);
	instruction[389].opcodeOperand[0] = OA_IB;
	instruction[389].opcodeOperandCount = 1;

	// Instruction Struct [390] - IN
	instruction[390].instructionStr = strdup("IN");
	instruction[390].undecodedStr = strdup("E5 ib");
	instruction[390].dstOperandStr = strdup("AX");
	instruction[390].dstOperand = IA_AX;
	instruction[390].srcOperandStr = strdup("imm8");
	instruction[390].srcOperand = IA_IMM8;
	instruction[390].valid64 = INVALID;
	instruction[390].compatLeg = VALID;
	instruction[390].VMX = 0;
	instruction[390].description = strdup("Input word from imm8 I/O port address into AX.");
	instruction[390].binPrefixLen = 0;
	instruction[390].binOpcodeLen = 1;
	instruction[390].binOpcode = bpstrndup("\xE5", 1);
	instruction[390].opcodeOperand[0] = OA_IB;
	instruction[390].opcodeOperandCount = 1;

	// Instruction Struct [391] - IN
	instruction[391].instructionStr = strdup("IN");
	instruction[391].undecodedStr = strdup("E5 ib");
	instruction[391].dstOperandStr = strdup("EAX");
	instruction[391].dstOperand = IA_EAX;
	instruction[391].srcOperandStr = strdup("imm8");
	instruction[391].srcOperand = IA_IMM8;
	instruction[391].valid64 = INVALID;
	instruction[391].compatLeg = VALID;
	instruction[391].VMX = 0;
	instruction[391].description = strdup("Input dword from imm8 I/O port address into EAX.");
	instruction[391].binPrefixLen = 0;
	instruction[391].binOpcodeLen = 1;
	instruction[391].binOpcode = bpstrndup("\xE5", 1);
	instruction[391].opcodeOperand[0] = OA_IB;
	instruction[391].opcodeOperandCount = 1;

	// Instruction Struct [392] - IN
	instruction[392].instructionStr = strdup("IN");
	instruction[392].undecodedStr = strdup("EC");
	instruction[392].dstOperandStr = strdup("AL");
	instruction[392].dstOperand = IA_AL;
	instruction[392].srcOperandStr = strdup("DX");
	instruction[392].srcOperand = IA_DX;
	instruction[392].valid64 = INVALID;
	instruction[392].compatLeg = VALID;
	instruction[392].VMX = 0;
	instruction[392].description = strdup("Input byte from I/O port in DX into AL.");
	instruction[392].binPrefixLen = 0;
	instruction[392].binOpcodeLen = 1;
	instruction[392].binOpcode = bpstrndup("\xEC", 1);
	instruction[392].opcodeOperandCount = 0;

	// Instruction Struct [393] - IN
	instruction[393].instructionStr = strdup("IN");
	instruction[393].undecodedStr = strdup("ED");
	instruction[393].dstOperandStr = strdup("AX");
	instruction[393].dstOperand = IA_AX;
	instruction[393].srcOperandStr = strdup("DX");
	instruction[393].srcOperand = IA_DX;
	instruction[393].valid64 = INVALID;
	instruction[393].compatLeg = VALID;
	instruction[393].VMX = 0;
	instruction[393].description = strdup("Input byte from I/O port in DX into AL.");
	instruction[393].binPrefixLen = 0;
	instruction[393].binOpcodeLen = 1;
	instruction[393].binOpcode = bpstrndup("\xED", 1);
	instruction[393].opcodeOperandCount = 0;

	// Instruction Struct [394] - IN
	instruction[394].instructionStr = strdup("IN");
	instruction[394].undecodedStr = strdup("ED");
	instruction[394].dstOperandStr = strdup("AX");
	instruction[394].dstOperand = IA_AX;
	instruction[394].srcOperandStr = strdup("DX");
	instruction[394].srcOperand = IA_DX;
	instruction[394].valid64 = INVALID;
	instruction[394].compatLeg = VALID;
	instruction[394].VMX = 0;
	instruction[394].description = strdup("Input word from I/O port in DX into AX.");
	instruction[394].binPrefixLen = 0;
	instruction[394].binOpcodeLen = 1;
	instruction[394].binOpcode = bpstrndup("\xED", 1);
	instruction[394].opcodeOperandCount = 0;

	// Instruction Struct [395] - IN
	instruction[395].instructionStr = strdup("IN");
	instruction[395].undecodedStr = strdup("ED");
	instruction[395].dstOperandStr = strdup("EAX");
	instruction[395].dstOperand = IA_EAX;
	instruction[395].srcOperandStr = strdup("DX");
	instruction[395].srcOperand = IA_DX;
	instruction[395].valid64 = INVALID;
	instruction[395].compatLeg = VALID;
	instruction[395].VMX = 0;
	instruction[395].description = strdup("Input doubleword from I/O port in DX into EAX.");
	instruction[395].binPrefixLen = 0;
	instruction[395].binOpcodeLen = 1;
	instruction[395].binOpcode = bpstrndup("\xED", 1);
	instruction[395].opcodeOperandCount = 0;

	// Instruction Struct [396] - INC
	instruction[396].instructionStr = strdup("INC");
	instruction[396].undecodedStr = strdup("FE /0");
	instruction[396].dstOperandStr = strdup("r/m8");
	instruction[396].dstOperand = IA_RM8;
	instruction[396].valid64 = INVALID;
	instruction[396].compatLeg = VALID;
	instruction[396].VMX = 0;
	instruction[396].description = strdup("Increment r/m byte by 1.");
	instruction[396].binPrefixLen = 0;
	instruction[396].binOpcodeLen = 1;
	instruction[396].binOpcode = bpstrndup("\xFE", 1);
	instruction[396].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[396].opcodeOperandCount = 1;

	// Instruction Struct [397] - INC
	instruction[397].instructionStr = strdup("INC");
	instruction[397].undecodedStr = strdup("FF /0");
	instruction[397].dstOperandStr = strdup("r/m16");
	instruction[397].dstOperand = IA_RM16;
	instruction[397].valid64 = INVALID;
	instruction[397].compatLeg = VALID;
	instruction[397].VMX = 0;
	instruction[397].description = strdup("Increment r/m word by 1.");
	instruction[397].binPrefixLen = 0;
	instruction[397].binOpcodeLen = 1;
	instruction[397].binOpcode = bpstrndup("\xFF", 1);
	instruction[397].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[397].opcodeOperandCount = 1;

	// Instruction Struct [398] - INC
	instruction[398].instructionStr = strdup("INC");
	instruction[398].undecodedStr = strdup("FF /0");
	instruction[398].dstOperandStr = strdup("r/m32");
	instruction[398].dstOperand = IA_RM32;
	instruction[398].valid64 = INVALID;
	instruction[398].compatLeg = VALID;
	instruction[398].VMX = 0;
	instruction[398].description = strdup("Increment r/m doubleword by 1.");
	instruction[398].binPrefixLen = 0;
	instruction[398].binOpcodeLen = 1;
	instruction[398].binOpcode = bpstrndup("\xFF", 1);
	instruction[398].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[398].opcodeOperandCount = 1;

	// Instruction Struct [399] - INC
	instruction[399].instructionStr = strdup("INC");
	instruction[399].undecodedStr = strdup("40 +rw");
	instruction[399].dstOperandStr = strdup("r16");
	instruction[399].dstOperand = IA_R16;
	instruction[399].valid64 = INVALID;
	instruction[399].compatLeg = VALID;
	instruction[399].VMX = 0;
	instruction[399].description = strdup("Increment word register by 1.");
	instruction[399].binPrefixLen = 0;
	instruction[399].binOpcodeLen = 1;
	instruction[399].binOpcode = bpstrndup("\x40", 1);
	instruction[399].opcodeOperand[0] = OA_RW;
	instruction[399].opcodeOperandCount = 1;

	// Instruction Struct [400] - INC
	instruction[400].instructionStr = strdup("INC");
	instruction[400].undecodedStr = strdup("40 +rd");
	instruction[400].dstOperandStr = strdup("r32");
	instruction[400].dstOperand = IA_R32;
	instruction[400].valid64 = INVALID;
	instruction[400].compatLeg = VALID;
	instruction[400].VMX = 0;
	instruction[400].description = strdup("Increment doubleword register by 1.");
	instruction[400].binPrefixLen = 0;
	instruction[400].binOpcodeLen = 1;
	instruction[400].binOpcode = bpstrndup("\x40", 1);
	instruction[400].opcodeOperand[0] = OA_RD;
	instruction[400].opcodeOperandCount = 1;

	// Instruction Struct [401] - INS
	instruction[401].instructionStr = strdup("INS");
	instruction[401].undecodedStr = strdup("6C");
	instruction[401].dstOperandStr = strdup("m8");
	instruction[401].dstOperand = IA_M8;
	instruction[401].srcOperandStr = strdup("DX");
	instruction[401].srcOperand = IA_DX;
	instruction[401].valid64 = INVALID;
	instruction[401].compatLeg = VALID;
	instruction[401].VMX = 0;
	instruction[401].description = strdup("Input byte from I/O port specified in DX into memory location specified in ES:(E)DI or RDI.");
	instruction[401].binPrefixLen = 0;
	instruction[401].binOpcodeLen = 1;
	instruction[401].binOpcode = bpstrndup("\x6C", 1);
	instruction[401].opcodeOperandCount = 0;

	// Instruction Struct [402] - INS
	instruction[402].instructionStr = strdup("INS");
	instruction[402].undecodedStr = strdup("6D");
	instruction[402].dstOperandStr = strdup("m16");
	instruction[402].dstOperand = IA_M16;
	instruction[402].srcOperandStr = strdup("DX");
	instruction[402].srcOperand = IA_DX;
	instruction[402].valid64 = INVALID;
	instruction[402].compatLeg = VALID;
	instruction[402].VMX = 0;
	instruction[402].description = strdup("Input word from I/O port specified in DX into memory location specified in ES:(E)DI or RDI.");
	instruction[402].binPrefixLen = 0;
	instruction[402].binOpcodeLen = 1;
	instruction[402].binOpcode = bpstrndup("\x6D", 1);
	instruction[402].opcodeOperandCount = 0;

	// Instruction Struct [403] - INS
	instruction[403].instructionStr = strdup("INS");
	instruction[403].undecodedStr = strdup("6D");
	instruction[403].dstOperandStr = strdup("m32");
	instruction[403].dstOperand = IA_M32;
	instruction[403].srcOperandStr = strdup("DX");
	instruction[403].srcOperand = IA_DX;
	instruction[403].valid64 = INVALID;
	instruction[403].compatLeg = VALID;
	instruction[403].VMX = 0;
	instruction[403].description = strdup("Input doubleword from I/O port specified in DX into memory location specified in ES:(E)DI or RDI.");
	instruction[403].binPrefixLen = 0;
	instruction[403].binOpcodeLen = 1;
	instruction[403].binOpcode = bpstrndup("\x6D", 1);
	instruction[403].opcodeOperandCount = 0;

	// Instruction Struct [404] - INSB
	instruction[404].instructionStr = strdup("INSB");
	instruction[404].undecodedStr = strdup("6C");
	instruction[404].valid64 = INVALID;
	instruction[404].compatLeg = VALID;
	instruction[404].VMX = 0;
	instruction[404].description = strdup("Input byte from I/O port specified in DX into memory location specified with ES:(E)DI or RDI.");
	instruction[404].binPrefixLen = 0;
	instruction[404].binOpcodeLen = 1;
	instruction[404].binOpcode = bpstrndup("\x6C", 1);
	instruction[404].opcodeOperandCount = 0;

	// Instruction Struct [405] - INSW
	instruction[405].instructionStr = strdup("INSW");
	instruction[405].undecodedStr = strdup("6D");
	instruction[405].valid64 = INVALID;
	instruction[405].compatLeg = VALID;
	instruction[405].VMX = 0;
	instruction[405].description = strdup("Input word from I/O port specified in DX into memory location specified in ES:(E)DI or RDI.");
	instruction[405].binPrefixLen = 0;
	instruction[405].binOpcodeLen = 1;
	instruction[405].binOpcode = bpstrndup("\x6D", 1);
	instruction[405].opcodeOperandCount = 0;

	// Instruction Struct [406] - INSD
	instruction[406].instructionStr = strdup("INSD");
	instruction[406].undecodedStr = strdup("6D");
	instruction[406].valid64 = INVALID;
	instruction[406].compatLeg = VALID;
	instruction[406].VMX = 0;
	instruction[406].description = strdup("Input doubleword from I/O port specified in DX into memory location specified in ES:(E)DI or RDI.");
	instruction[406].binPrefixLen = 0;
	instruction[406].binOpcodeLen = 1;
	instruction[406].binOpcode = bpstrndup("\x6D", 1);
	instruction[406].opcodeOperandCount = 0;

	// Instruction Struct [407] - INT
	instruction[407].instructionStr = strdup("INT");
	instruction[407].undecodedStr = strdup("CC");
	instruction[407].dstOperandStr = strdup("3");
	instruction[407].dstOperand = IA_3;
	instruction[407].valid64 = INVALID;
	instruction[407].compatLeg = VALID;
	instruction[407].VMX = 0;
	instruction[407].description = strdup("Interrupt 3?trap to debugger.");
	instruction[407].binPrefixLen = 0;
	instruction[407].binOpcodeLen = 1;
	instruction[407].binOpcode = bpstrndup("\xCC", 1);
	instruction[407].opcodeOperandCount = 0;

	// Instruction Struct [408] - INT
	instruction[408].instructionStr = strdup("INT");
	instruction[408].undecodedStr = strdup("CD ib");
	instruction[408].dstOperandStr = strdup("imm8");
	instruction[408].dstOperand = IA_IMM8;
	instruction[408].valid64 = INVALID;
	instruction[408].compatLeg = VALID;
	instruction[408].VMX = 0;
	instruction[408].description = strdup("Interrupt vector number specified by immediate byte.");
	instruction[408].binPrefixLen = 0;
	instruction[408].binOpcodeLen = 1;
	instruction[408].binOpcode = bpstrndup("\xCD", 1);
	instruction[408].opcodeOperand[0] = OA_IB;
	instruction[408].opcodeOperandCount = 1;

	// Instruction Struct [409] - INTO
	instruction[409].instructionStr = strdup("INTO");
	instruction[409].undecodedStr = strdup("CE");
	instruction[409].valid64 = INVALID;
	instruction[409].compatLeg = VALID;
	instruction[409].VMX = 0;
	instruction[409].description = strdup("Interrupt 4?if overflow flag is 1.");
	instruction[409].binPrefixLen = 0;
	instruction[409].binOpcodeLen = 1;
	instruction[409].binOpcode = bpstrndup("\xCE", 1);
	instruction[409].opcodeOperandCount = 0;

	// Instruction Struct [410] - INVD
	instruction[410].instructionStr = strdup("INVD");
	instruction[410].undecodedStr = strdup("0F 08");
	instruction[410].valid64 = INVALID;
	instruction[410].compatLeg = VALID;
	instruction[410].VMX = 0;
	instruction[410].description = strdup("Flush internal caches; initiate flushing of external caches.");
	instruction[410].binPrefixLen = 0;
	instruction[410].binOpcodeLen = 2;
	instruction[410].binOpcode = bpstrndup("\x0F\x08", 2);
	instruction[410].opcodeOperandCount = 0;

	// Instruction Struct [411] - INVLPG
	instruction[411].instructionStr = strdup("INVLPG");
	instruction[411].undecodedStr = strdup("0F 01 /7");
	instruction[411].dstOperandStr = strdup("m");
	instruction[411].dstOperand = IA_M;
	instruction[411].valid64 = INVALID;
	instruction[411].compatLeg = VALID;
	instruction[411].VMX = 0;
	instruction[411].description = strdup("Invalidate TLB Entry for page that");
	instruction[411].binPrefixLen = 0;
	instruction[411].binOpcodeLen = 2;
	instruction[411].binOpcode = bpstrndup("\x0F\x01", 2);
	instruction[411].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[411].opcodeOperandCount = 1;

	// Instruction Struct [412] - IRET
	instruction[412].instructionStr = strdup("IRET");
	instruction[412].undecodedStr = strdup("CF");
	instruction[412].valid64 = INVALID;
	instruction[412].compatLeg = VALID;
	instruction[412].VMX = 0;
	instruction[412].description = strdup("Interrupt return (16-bit operand size).");
	instruction[412].binPrefixLen = 0;
	instruction[412].binOpcodeLen = 1;
	instruction[412].binOpcode = bpstrndup("\xCF", 1);
	instruction[412].opcodeOperandCount = 0;

	// Instruction Struct [413] - IRETD
	instruction[413].instructionStr = strdup("IRETD");
	instruction[413].undecodedStr = strdup("CF");
	instruction[413].valid64 = INVALID;
	instruction[413].compatLeg = VALID;
	instruction[413].VMX = 0;
	instruction[413].description = strdup("Interrupt return (32-bit operand size).");
	instruction[413].binPrefixLen = 0;
	instruction[413].binOpcodeLen = 1;
	instruction[413].binOpcode = bpstrndup("\xCF", 1);
	instruction[413].opcodeOperandCount = 0;

	// Instruction Struct [414] - JA
	instruction[414].instructionStr = strdup("JA");
	instruction[414].undecodedStr = strdup("77 cb");
	instruction[414].dstOperandStr = strdup("rel8");
	instruction[414].dstOperand = IA_REL8;
	instruction[414].valid64 = INVALID;
	instruction[414].compatLeg = VALID;
	instruction[414].VMX = 0;
	instruction[414].description = strdup("Jump short if above (CF=0 and ZF=0).");
	instruction[414].binPrefixLen = 0;
	instruction[414].binOpcodeLen = 1;
	instruction[414].binOpcode = bpstrndup("\x77", 1);
	instruction[414].opcodeOperand[0] = OA_CB;
	instruction[414].opcodeOperandCount = 1;

	// Instruction Struct [415] - JAE
	instruction[415].instructionStr = strdup("JAE");
	instruction[415].undecodedStr = strdup("73 cb");
	instruction[415].dstOperandStr = strdup("rel8");
	instruction[415].dstOperand = IA_REL8;
	instruction[415].valid64 = INVALID;
	instruction[415].compatLeg = VALID;
	instruction[415].VMX = 0;
	instruction[415].description = strdup("Jump short if above or equal (CF=0).");
	instruction[415].binPrefixLen = 0;
	instruction[415].binOpcodeLen = 1;
	instruction[415].binOpcode = bpstrndup("\x73", 1);
	instruction[415].opcodeOperand[0] = OA_CB;
	instruction[415].opcodeOperandCount = 1;

	// Instruction Struct [416] - JB
	instruction[416].instructionStr = strdup("JB");
	instruction[416].undecodedStr = strdup("72 cb");
	instruction[416].dstOperandStr = strdup("rel8");
	instruction[416].dstOperand = IA_REL8;
	instruction[416].valid64 = INVALID;
	instruction[416].compatLeg = VALID;
	instruction[416].VMX = 0;
	instruction[416].description = strdup("Jump short if below (CF=1).");
	instruction[416].binPrefixLen = 0;
	instruction[416].binOpcodeLen = 1;
	instruction[416].binOpcode = bpstrndup("\x72", 1);
	instruction[416].opcodeOperand[0] = OA_CB;
	instruction[416].opcodeOperandCount = 1;

	// Instruction Struct [417] - JBE
	instruction[417].instructionStr = strdup("JBE");
	instruction[417].undecodedStr = strdup("76 cb");
	instruction[417].dstOperandStr = strdup("rel8");
	instruction[417].dstOperand = IA_REL8;
	instruction[417].valid64 = INVALID;
	instruction[417].compatLeg = VALID;
	instruction[417].VMX = 0;
	instruction[417].description = strdup("Jump short if below or equal (CF=1 or ZF=1).");
	instruction[417].binPrefixLen = 0;
	instruction[417].binOpcodeLen = 1;
	instruction[417].binOpcode = bpstrndup("\x76", 1);
	instruction[417].opcodeOperand[0] = OA_CB;
	instruction[417].opcodeOperandCount = 1;

	// Instruction Struct [418] - JC
	instruction[418].instructionStr = strdup("JC");
	instruction[418].undecodedStr = strdup("72 cb");
	instruction[418].dstOperandStr = strdup("rel8");
	instruction[418].dstOperand = IA_REL8;
	instruction[418].valid64 = INVALID;
	instruction[418].compatLeg = VALID;
	instruction[418].VMX = 0;
	instruction[418].description = strdup("Jump short if carry (CF=1).");
	instruction[418].binPrefixLen = 0;
	instruction[418].binOpcodeLen = 1;
	instruction[418].binOpcode = bpstrndup("\x72", 1);
	instruction[418].opcodeOperand[0] = OA_CB;
	instruction[418].opcodeOperandCount = 1;

	// Instruction Struct [419] - JCXZ
	instruction[419].instructionStr = strdup("JCXZ");
	instruction[419].undecodedStr = strdup("E3 cb");
	instruction[419].dstOperandStr = strdup("rel8");
	instruction[419].dstOperand = IA_REL8;
	instruction[419].valid64 = INVALID;
	instruction[419].compatLeg = VALID;
	instruction[419].VMX = 0;
	instruction[419].description = strdup("Jump short if CX register is 0.");
	instruction[419].binPrefixLen = 0;
	instruction[419].binOpcodeLen = 1;
	instruction[419].binOpcode = bpstrndup("\xE3", 1);
	instruction[419].opcodeOperand[0] = OA_CB;
	instruction[419].opcodeOperandCount = 1;

	// Instruction Struct [420] - JECXZ
	instruction[420].instructionStr = strdup("JECXZ");
	instruction[420].undecodedStr = strdup("E3 cb");
	instruction[420].dstOperandStr = strdup("rel8");
	instruction[420].dstOperand = IA_REL8;
	instruction[420].valid64 = INVALID;
	instruction[420].compatLeg = VALID;
	instruction[420].VMX = 0;
	instruction[420].description = strdup("Jump short if ECX register is 0.");
	instruction[420].binPrefixLen = 0;
	instruction[420].binOpcodeLen = 1;
	instruction[420].binOpcode = bpstrndup("\xE3", 1);
	instruction[420].opcodeOperand[0] = OA_CB;
	instruction[420].opcodeOperandCount = 1;

	// Instruction Struct [421] - JRCXZ
	instruction[421].instructionStr = strdup("JRCXZ");
	instruction[421].undecodedStr = strdup("E3 cb");
	instruction[421].dstOperandStr = strdup("rel8");
	instruction[421].dstOperand = IA_REL8;
	instruction[421].valid64 = INVALID;
	instruction[421].compatLeg = INVALID;
	instruction[421].VMX = 0;
	instruction[421].description = strdup("Jump short if RCX register is 0.");
	instruction[421].binPrefixLen = 0;
	instruction[421].binOpcodeLen = 1;
	instruction[421].binOpcode = bpstrndup("\xE3", 1);
	instruction[421].opcodeOperand[0] = OA_CB;
	instruction[421].opcodeOperandCount = 1;

	// Instruction Struct [422] - JE
	instruction[422].instructionStr = strdup("JE");
	instruction[422].undecodedStr = strdup("74 cb");
	instruction[422].dstOperandStr = strdup("rel8");
	instruction[422].dstOperand = IA_REL8;
	instruction[422].valid64 = INVALID;
	instruction[422].compatLeg = VALID;
	instruction[422].VMX = 0;
	instruction[422].description = strdup("Jump short if equal (ZF=1).");
	instruction[422].binPrefixLen = 0;
	instruction[422].binOpcodeLen = 1;
	instruction[422].binOpcode = bpstrndup("\x74", 1);
	instruction[422].opcodeOperand[0] = OA_CB;
	instruction[422].opcodeOperandCount = 1;

	// Instruction Struct [423] - JG
	instruction[423].instructionStr = strdup("JG");
	instruction[423].undecodedStr = strdup("7F cb");
	instruction[423].dstOperandStr = strdup("rel8");
	instruction[423].dstOperand = IA_REL8;
	instruction[423].valid64 = INVALID;
	instruction[423].compatLeg = VALID;
	instruction[423].VMX = 0;
	instruction[423].description = strdup("Jump short if greater (ZF=0 and SF=OF).");
	instruction[423].binPrefixLen = 0;
	instruction[423].binOpcodeLen = 1;
	instruction[423].binOpcode = bpstrndup("\x7F", 1);
	instruction[423].opcodeOperand[0] = OA_CB;
	instruction[423].opcodeOperandCount = 1;

	// Instruction Struct [424] - JGE
	instruction[424].instructionStr = strdup("JGE");
	instruction[424].undecodedStr = strdup("7D cb");
	instruction[424].dstOperandStr = strdup("rel8");
	instruction[424].dstOperand = IA_REL8;
	instruction[424].valid64 = INVALID;
	instruction[424].compatLeg = VALID;
	instruction[424].VMX = 0;
	instruction[424].description = strdup("Jump short if greater or equal (SF=OF).");
	instruction[424].binPrefixLen = 0;
	instruction[424].binOpcodeLen = 1;
	instruction[424].binOpcode = bpstrndup("\x7D", 1);
	instruction[424].opcodeOperand[0] = OA_CB;
	instruction[424].opcodeOperandCount = 1;

	// Instruction Struct [425] - JL
	instruction[425].instructionStr = strdup("JL");
	instruction[425].undecodedStr = strdup("7C cb");
	instruction[425].dstOperandStr = strdup("rel8");
	instruction[425].dstOperand = IA_REL8;
	instruction[425].valid64 = INVALID;
	instruction[425].compatLeg = VALID;
	instruction[425].VMX = 0;
	instruction[425].description = strdup("Jump short if less (SF? OF).");
	instruction[425].binPrefixLen = 0;
	instruction[425].binOpcodeLen = 1;
	instruction[425].binOpcode = bpstrndup("\x7C", 1);
	instruction[425].opcodeOperand[0] = OA_CB;
	instruction[425].opcodeOperandCount = 1;

	// Instruction Struct [426] - JLE
	instruction[426].instructionStr = strdup("JLE");
	instruction[426].undecodedStr = strdup("7E cb");
	instruction[426].dstOperandStr = strdup("rel8");
	instruction[426].dstOperand = IA_REL8;
	instruction[426].valid64 = INVALID;
	instruction[426].compatLeg = VALID;
	instruction[426].VMX = 0;
	instruction[426].description = strdup("Jump short if less or equal (ZF=1 or SF? OF).");
	instruction[426].binPrefixLen = 0;
	instruction[426].binOpcodeLen = 1;
	instruction[426].binOpcode = bpstrndup("\x7E", 1);
	instruction[426].opcodeOperand[0] = OA_CB;
	instruction[426].opcodeOperandCount = 1;

	// Instruction Struct [427] - JNA
	instruction[427].instructionStr = strdup("JNA");
	instruction[427].undecodedStr = strdup("76 cb");
	instruction[427].dstOperandStr = strdup("rel8");
	instruction[427].dstOperand = IA_REL8;
	instruction[427].valid64 = INVALID;
	instruction[427].compatLeg = VALID;
	instruction[427].VMX = 0;
	instruction[427].description = strdup("Jump short if not above (CF=1 or ZF=1).");
	instruction[427].binPrefixLen = 0;
	instruction[427].binOpcodeLen = 1;
	instruction[427].binOpcode = bpstrndup("\x76", 1);
	instruction[427].opcodeOperand[0] = OA_CB;
	instruction[427].opcodeOperandCount = 1;

	// Instruction Struct [428] - JNAE
	instruction[428].instructionStr = strdup("JNAE");
	instruction[428].undecodedStr = strdup("72 cb");
	instruction[428].dstOperandStr = strdup("rel8");
	instruction[428].dstOperand = IA_REL8;
	instruction[428].valid64 = INVALID;
	instruction[428].compatLeg = VALID;
	instruction[428].VMX = 0;
	instruction[428].description = strdup("Jump short if not above or equal (CF=1).");
	instruction[428].binPrefixLen = 0;
	instruction[428].binOpcodeLen = 1;
	instruction[428].binOpcode = bpstrndup("\x72", 1);
	instruction[428].opcodeOperand[0] = OA_CB;
	instruction[428].opcodeOperandCount = 1;

	// Instruction Struct [429] - JNB
	instruction[429].instructionStr = strdup("JNB");
	instruction[429].undecodedStr = strdup("73 cb");
	instruction[429].dstOperandStr = strdup("rel8");
	instruction[429].dstOperand = IA_REL8;
	instruction[429].valid64 = INVALID;
	instruction[429].compatLeg = VALID;
	instruction[429].VMX = 0;
	instruction[429].description = strdup("Jump short if not below (CF=0).");
	instruction[429].binPrefixLen = 0;
	instruction[429].binOpcodeLen = 1;
	instruction[429].binOpcode = bpstrndup("\x73", 1);
	instruction[429].opcodeOperand[0] = OA_CB;
	instruction[429].opcodeOperandCount = 1;

	// Instruction Struct [430] - JNBE
	instruction[430].instructionStr = strdup("JNBE");
	instruction[430].undecodedStr = strdup("77 cb");
	instruction[430].dstOperandStr = strdup("rel8");
	instruction[430].dstOperand = IA_REL8;
	instruction[430].valid64 = INVALID;
	instruction[430].compatLeg = VALID;
	instruction[430].VMX = 0;
	instruction[430].description = strdup("Jump short if not below or equal (CF=0 and ZF=0).");
	instruction[430].binPrefixLen = 0;
	instruction[430].binOpcodeLen = 1;
	instruction[430].binOpcode = bpstrndup("\x77", 1);
	instruction[430].opcodeOperand[0] = OA_CB;
	instruction[430].opcodeOperandCount = 1;

	// Instruction Struct [431] - JNC
	instruction[431].instructionStr = strdup("JNC");
	instruction[431].undecodedStr = strdup("73 cb");
	instruction[431].dstOperandStr = strdup("rel8");
	instruction[431].dstOperand = IA_REL8;
	instruction[431].valid64 = INVALID;
	instruction[431].compatLeg = VALID;
	instruction[431].VMX = 0;
	instruction[431].description = strdup("Jump short if not carry (CF=0).");
	instruction[431].binPrefixLen = 0;
	instruction[431].binOpcodeLen = 1;
	instruction[431].binOpcode = bpstrndup("\x73", 1);
	instruction[431].opcodeOperand[0] = OA_CB;
	instruction[431].opcodeOperandCount = 1;

	// Instruction Struct [432] - JNE
	instruction[432].instructionStr = strdup("JNE");
	instruction[432].undecodedStr = strdup("75 cb");
	instruction[432].dstOperandStr = strdup("rel8");
	instruction[432].dstOperand = IA_REL8;
	instruction[432].valid64 = INVALID;
	instruction[432].compatLeg = VALID;
	instruction[432].VMX = 0;
	instruction[432].description = strdup("Jump short if not equal (ZF=0).");
	instruction[432].binPrefixLen = 0;
	instruction[432].binOpcodeLen = 1;
	instruction[432].binOpcode = bpstrndup("\x75", 1);
	instruction[432].opcodeOperand[0] = OA_CB;
	instruction[432].opcodeOperandCount = 1;

	// Instruction Struct [433] - JNG
	instruction[433].instructionStr = strdup("JNG");
	instruction[433].undecodedStr = strdup("7E cb");
	instruction[433].dstOperandStr = strdup("rel8");
	instruction[433].dstOperand = IA_REL8;
	instruction[433].valid64 = INVALID;
	instruction[433].compatLeg = VALID;
	instruction[433].VMX = 0;
	instruction[433].description = strdup("Jump short if not greater (ZF=1 or SF? OF).");
	instruction[433].binPrefixLen = 0;
	instruction[433].binOpcodeLen = 1;
	instruction[433].binOpcode = bpstrndup("\x7E", 1);
	instruction[433].opcodeOperand[0] = OA_CB;
	instruction[433].opcodeOperandCount = 1;

	// Instruction Struct [434] - JNGE
	instruction[434].instructionStr = strdup("JNGE");
	instruction[434].undecodedStr = strdup("7C cb");
	instruction[434].dstOperandStr = strdup("rel8");
	instruction[434].dstOperand = IA_REL8;
	instruction[434].valid64 = INVALID;
	instruction[434].compatLeg = VALID;
	instruction[434].VMX = 0;
	instruction[434].description = strdup("Jump short if not greater or equal (SF? OF).");
	instruction[434].binPrefixLen = 0;
	instruction[434].binOpcodeLen = 1;
	instruction[434].binOpcode = bpstrndup("\x7C", 1);
	instruction[434].opcodeOperand[0] = OA_CB;
	instruction[434].opcodeOperandCount = 1;

	// Instruction Struct [435] - JNL
	instruction[435].instructionStr = strdup("JNL");
	instruction[435].undecodedStr = strdup("7D cb");
	instruction[435].dstOperandStr = strdup("rel8");
	instruction[435].dstOperand = IA_REL8;
	instruction[435].valid64 = INVALID;
	instruction[435].compatLeg = VALID;
	instruction[435].VMX = 0;
	instruction[435].description = strdup("Jump short if not less (SF=OF).");
	instruction[435].binPrefixLen = 0;
	instruction[435].binOpcodeLen = 1;
	instruction[435].binOpcode = bpstrndup("\x7D", 1);
	instruction[435].opcodeOperand[0] = OA_CB;
	instruction[435].opcodeOperandCount = 1;

	// Instruction Struct [436] - JNLE
	instruction[436].instructionStr = strdup("JNLE");
	instruction[436].undecodedStr = strdup("7F cb");
	instruction[436].dstOperandStr = strdup("rel8");
	instruction[436].dstOperand = IA_REL8;
	instruction[436].valid64 = INVALID;
	instruction[436].compatLeg = VALID;
	instruction[436].VMX = 0;
	instruction[436].description = strdup("Jump short if not less or equal (ZF=0 and SF=OF).");
	instruction[436].binPrefixLen = 0;
	instruction[436].binOpcodeLen = 1;
	instruction[436].binOpcode = bpstrndup("\x7F", 1);
	instruction[436].opcodeOperand[0] = OA_CB;
	instruction[436].opcodeOperandCount = 1;

	// Instruction Struct [437] - JNO
	instruction[437].instructionStr = strdup("JNO");
	instruction[437].undecodedStr = strdup("71 cb");
	instruction[437].dstOperandStr = strdup("rel8");
	instruction[437].dstOperand = IA_REL8;
	instruction[437].valid64 = INVALID;
	instruction[437].compatLeg = VALID;
	instruction[437].VMX = 0;
	instruction[437].description = strdup("Jump short if not overflow (OF=0).");
	instruction[437].binPrefixLen = 0;
	instruction[437].binOpcodeLen = 1;
	instruction[437].binOpcode = bpstrndup("\x71", 1);
	instruction[437].opcodeOperand[0] = OA_CB;
	instruction[437].opcodeOperandCount = 1;

	// Instruction Struct [438] - JNP
	instruction[438].instructionStr = strdup("JNP");
	instruction[438].undecodedStr = strdup("7B cb");
	instruction[438].dstOperandStr = strdup("rel8");
	instruction[438].dstOperand = IA_REL8;
	instruction[438].valid64 = INVALID;
	instruction[438].compatLeg = VALID;
	instruction[438].VMX = 0;
	instruction[438].description = strdup("Jump short if not parity (PF=0).");
	instruction[438].binPrefixLen = 0;
	instruction[438].binOpcodeLen = 1;
	instruction[438].binOpcode = bpstrndup("\x7B", 1);
	instruction[438].opcodeOperand[0] = OA_CB;
	instruction[438].opcodeOperandCount = 1;

	// Instruction Struct [439] - JNS
	instruction[439].instructionStr = strdup("JNS");
	instruction[439].undecodedStr = strdup("79 cb");
	instruction[439].dstOperandStr = strdup("rel8");
	instruction[439].dstOperand = IA_REL8;
	instruction[439].valid64 = INVALID;
	instruction[439].compatLeg = VALID;
	instruction[439].VMX = 0;
	instruction[439].description = strdup("Jump short if not sign (SF=0).");
	instruction[439].binPrefixLen = 0;
	instruction[439].binOpcodeLen = 1;
	instruction[439].binOpcode = bpstrndup("\x79", 1);
	instruction[439].opcodeOperand[0] = OA_CB;
	instruction[439].opcodeOperandCount = 1;

	// Instruction Struct [440] - JNZ
	instruction[440].instructionStr = strdup("JNZ");
	instruction[440].undecodedStr = strdup("75 cb");
	instruction[440].dstOperandStr = strdup("rel8");
	instruction[440].dstOperand = IA_REL8;
	instruction[440].valid64 = INVALID;
	instruction[440].compatLeg = VALID;
	instruction[440].VMX = 0;
	instruction[440].description = strdup("Jump short if not zero (ZF=0).");
	instruction[440].binPrefixLen = 0;
	instruction[440].binOpcodeLen = 1;
	instruction[440].binOpcode = bpstrndup("\x75", 1);
	instruction[440].opcodeOperand[0] = OA_CB;
	instruction[440].opcodeOperandCount = 1;

	// Instruction Struct [441] - JO
	instruction[441].instructionStr = strdup("JO");
	instruction[441].undecodedStr = strdup("70 cb");
	instruction[441].dstOperandStr = strdup("rel8");
	instruction[441].dstOperand = IA_REL8;
	instruction[441].valid64 = INVALID;
	instruction[441].compatLeg = VALID;
	instruction[441].VMX = 0;
	instruction[441].description = strdup("Jump short if overflow (OF=1).");
	instruction[441].binPrefixLen = 0;
	instruction[441].binOpcodeLen = 1;
	instruction[441].binOpcode = bpstrndup("\x70", 1);
	instruction[441].opcodeOperand[0] = OA_CB;
	instruction[441].opcodeOperandCount = 1;

	// Instruction Struct [442] - JP
	instruction[442].instructionStr = strdup("JP");
	instruction[442].undecodedStr = strdup("7A cb");
	instruction[442].dstOperandStr = strdup("rel8");
	instruction[442].dstOperand = IA_REL8;
	instruction[442].valid64 = INVALID;
	instruction[442].compatLeg = VALID;
	instruction[442].VMX = 0;
	instruction[442].description = strdup("Jump short if parity (PF=1).");
	instruction[442].binPrefixLen = 0;
	instruction[442].binOpcodeLen = 1;
	instruction[442].binOpcode = bpstrndup("\x7A", 1);
	instruction[442].opcodeOperand[0] = OA_CB;
	instruction[442].opcodeOperandCount = 1;

	// Instruction Struct [443] - JPE
	instruction[443].instructionStr = strdup("JPE");
	instruction[443].undecodedStr = strdup("7A cb");
	instruction[443].dstOperandStr = strdup("rel8");
	instruction[443].dstOperand = IA_REL8;
	instruction[443].valid64 = INVALID;
	instruction[443].compatLeg = VALID;
	instruction[443].VMX = 0;
	instruction[443].description = strdup("Jump short if parity even (PF=1).");
	instruction[443].binPrefixLen = 0;
	instruction[443].binOpcodeLen = 1;
	instruction[443].binOpcode = bpstrndup("\x7A", 1);
	instruction[443].opcodeOperand[0] = OA_CB;
	instruction[443].opcodeOperandCount = 1;

	// Instruction Struct [444] - JPO
	instruction[444].instructionStr = strdup("JPO");
	instruction[444].undecodedStr = strdup("7B cb");
	instruction[444].dstOperandStr = strdup("rel8");
	instruction[444].dstOperand = IA_REL8;
	instruction[444].valid64 = INVALID;
	instruction[444].compatLeg = VALID;
	instruction[444].VMX = 0;
	instruction[444].description = strdup("Jump short if parity odd (PF=0).");
	instruction[444].binPrefixLen = 0;
	instruction[444].binOpcodeLen = 1;
	instruction[444].binOpcode = bpstrndup("\x7B", 1);
	instruction[444].opcodeOperand[0] = OA_CB;
	instruction[444].opcodeOperandCount = 1;

	// Instruction Struct [445] - JS
	instruction[445].instructionStr = strdup("JS");
	instruction[445].undecodedStr = strdup("78 cb");
	instruction[445].dstOperandStr = strdup("rel8");
	instruction[445].dstOperand = IA_REL8;
	instruction[445].valid64 = INVALID;
	instruction[445].compatLeg = VALID;
	instruction[445].VMX = 0;
	instruction[445].description = strdup("Jump short if sign (SF=1).");
	instruction[445].binPrefixLen = 0;
	instruction[445].binOpcodeLen = 1;
	instruction[445].binOpcode = bpstrndup("\x78", 1);
	instruction[445].opcodeOperand[0] = OA_CB;
	instruction[445].opcodeOperandCount = 1;

	// Instruction Struct [446] - JZ
	instruction[446].instructionStr = strdup("JZ");
	instruction[446].undecodedStr = strdup("74 cb");
	instruction[446].dstOperandStr = strdup("rel8");
	instruction[446].dstOperand = IA_REL8;
	instruction[446].valid64 = INVALID;
	instruction[446].compatLeg = VALID;
	instruction[446].VMX = 0;
	instruction[446].description = strdup("Jump short if zero (ZF ? 1).");
	instruction[446].binPrefixLen = 0;
	instruction[446].binOpcodeLen = 1;
	instruction[446].binOpcode = bpstrndup("\x74", 1);
	instruction[446].opcodeOperand[0] = OA_CB;
	instruction[446].opcodeOperandCount = 1;

	// Instruction Struct [447] - JA
	instruction[447].instructionStr = strdup("JA");
	instruction[447].undecodedStr = strdup("0F 87 cw");
	instruction[447].dstOperandStr = strdup("rel16");
	instruction[447].dstOperand = IA_REL16;
	instruction[447].valid64 = INVALID;
	instruction[447].compatLeg = VALID;
	instruction[447].VMX = 0;
	instruction[447].description = strdup("Jump near if above (CF=0 and ZF=0). Not supported in 64-bit mode.");
	instruction[447].binPrefixLen = 0;
	instruction[447].binOpcodeLen = 2;
	instruction[447].binOpcode = bpstrndup("\x0F\x87", 2);
	instruction[447].opcodeOperand[0] = OA_CW;
	instruction[447].opcodeOperandCount = 1;

	// Instruction Struct [448] - JA
	instruction[448].instructionStr = strdup("JA");
	instruction[448].undecodedStr = strdup("0F 87 cd");
	instruction[448].dstOperandStr = strdup("rel32");
	instruction[448].dstOperand = IA_REL32;
	instruction[448].valid64 = INVALID;
	instruction[448].compatLeg = VALID;
	instruction[448].VMX = 0;
	instruction[448].description = strdup("Jump near if above (CF=0 and ZF=0).");
	instruction[448].binPrefixLen = 0;
	instruction[448].binOpcodeLen = 2;
	instruction[448].binOpcode = bpstrndup("\x0F\x87", 2);
	instruction[448].opcodeOperand[0] = OA_CD;
	instruction[448].opcodeOperandCount = 1;

	// Instruction Struct [449] - JAE
	instruction[449].instructionStr = strdup("JAE");
	instruction[449].undecodedStr = strdup("0F 83 cw");
	instruction[449].dstOperandStr = strdup("rel16");
	instruction[449].dstOperand = IA_REL16;
	instruction[449].valid64 = INVALID;
	instruction[449].compatLeg = VALID;
	instruction[449].VMX = 0;
	instruction[449].description = strdup("Jump near if above or equal (CF=0). Not supported in 64-bit mode.");
	instruction[449].binPrefixLen = 0;
	instruction[449].binOpcodeLen = 2;
	instruction[449].binOpcode = bpstrndup("\x0F\x83", 2);
	instruction[449].opcodeOperand[0] = OA_CW;
	instruction[449].opcodeOperandCount = 1;

	// Instruction Struct [450] - JAE
	instruction[450].instructionStr = strdup("JAE");
	instruction[450].undecodedStr = strdup("0F 83 cd");
	instruction[450].dstOperandStr = strdup("rel32");
	instruction[450].dstOperand = IA_REL32;
	instruction[450].valid64 = INVALID;
	instruction[450].compatLeg = VALID;
	instruction[450].VMX = 0;
	instruction[450].description = strdup("Jump near if above or equal (CF=0).");
	instruction[450].binPrefixLen = 0;
	instruction[450].binOpcodeLen = 2;
	instruction[450].binOpcode = bpstrndup("\x0F\x83", 2);
	instruction[450].opcodeOperand[0] = OA_CD;
	instruction[450].opcodeOperandCount = 1;

	// Instruction Struct [451] - JB
	instruction[451].instructionStr = strdup("JB");
	instruction[451].undecodedStr = strdup("0F 82 cw");
	instruction[451].dstOperandStr = strdup("rel16");
	instruction[451].dstOperand = IA_REL16;
	instruction[451].valid64 = INVALID;
	instruction[451].compatLeg = VALID;
	instruction[451].VMX = 0;
	instruction[451].description = strdup("Jump near if below (CF=1). Not supported in 64-bit mode.");
	instruction[451].binPrefixLen = 0;
	instruction[451].binOpcodeLen = 2;
	instruction[451].binOpcode = bpstrndup("\x0F\x82", 2);
	instruction[451].opcodeOperand[0] = OA_CW;
	instruction[451].opcodeOperandCount = 1;

	// Instruction Struct [452] - JB
	instruction[452].instructionStr = strdup("JB");
	instruction[452].undecodedStr = strdup("0F 82 cd");
	instruction[452].dstOperandStr = strdup("rel32");
	instruction[452].dstOperand = IA_REL32;
	instruction[452].valid64 = INVALID;
	instruction[452].compatLeg = VALID;
	instruction[452].VMX = 0;
	instruction[452].description = strdup("Jump near if below (CF=1).");
	instruction[452].binPrefixLen = 0;
	instruction[452].binOpcodeLen = 2;
	instruction[452].binOpcode = bpstrndup("\x0F\x82", 2);
	instruction[452].opcodeOperand[0] = OA_CD;
	instruction[452].opcodeOperandCount = 1;

	// Instruction Struct [453] - JBE
	instruction[453].instructionStr = strdup("JBE");
	instruction[453].undecodedStr = strdup("0F 86 cw");
	instruction[453].dstOperandStr = strdup("rel16");
	instruction[453].dstOperand = IA_REL16;
	instruction[453].valid64 = INVALID;
	instruction[453].compatLeg = VALID;
	instruction[453].VMX = 0;
	instruction[453].description = strdup("Jump near if below or equal (CF=1 or ZF=1). Not supported in 64-bit mode.");
	instruction[453].binPrefixLen = 0;
	instruction[453].binOpcodeLen = 2;
	instruction[453].binOpcode = bpstrndup("\x0F\x86", 2);
	instruction[453].opcodeOperand[0] = OA_CW;
	instruction[453].opcodeOperandCount = 1;

	// Instruction Struct [454] - JBE
	instruction[454].instructionStr = strdup("JBE");
	instruction[454].undecodedStr = strdup("0F 86 cd");
	instruction[454].dstOperandStr = strdup("rel32");
	instruction[454].dstOperand = IA_REL32;
	instruction[454].valid64 = INVALID;
	instruction[454].compatLeg = VALID;
	instruction[454].VMX = 0;
	instruction[454].description = strdup("Jump near if below or equal (CF=1 or ZF=1).");
	instruction[454].binPrefixLen = 0;
	instruction[454].binOpcodeLen = 2;
	instruction[454].binOpcode = bpstrndup("\x0F\x86", 2);
	instruction[454].opcodeOperand[0] = OA_CD;
	instruction[454].opcodeOperandCount = 1;

	// Instruction Struct [455] - JC
	instruction[455].instructionStr = strdup("JC");
	instruction[455].undecodedStr = strdup("0F 82 cw");
	instruction[455].dstOperandStr = strdup("rel16");
	instruction[455].dstOperand = IA_REL16;
	instruction[455].valid64 = INVALID;
	instruction[455].compatLeg = VALID;
	instruction[455].VMX = 0;
	instruction[455].description = strdup("Jump near if carry (CF=1). Not supported in 64-bit mode.");
	instruction[455].binPrefixLen = 0;
	instruction[455].binOpcodeLen = 2;
	instruction[455].binOpcode = bpstrndup("\x0F\x82", 2);
	instruction[455].opcodeOperand[0] = OA_CW;
	instruction[455].opcodeOperandCount = 1;

	// Instruction Struct [456] - JC
	instruction[456].instructionStr = strdup("JC");
	instruction[456].undecodedStr = strdup("0F 82 cd");
	instruction[456].dstOperandStr = strdup("rel32");
	instruction[456].dstOperand = IA_REL32;
	instruction[456].valid64 = INVALID;
	instruction[456].compatLeg = VALID;
	instruction[456].VMX = 0;
	instruction[456].description = strdup("Jump near if carry (CF=1).");
	instruction[456].binPrefixLen = 0;
	instruction[456].binOpcodeLen = 2;
	instruction[456].binOpcode = bpstrndup("\x0F\x82", 2);
	instruction[456].opcodeOperand[0] = OA_CD;
	instruction[456].opcodeOperandCount = 1;

	// Instruction Struct [457] - JE
	instruction[457].instructionStr = strdup("JE");
	instruction[457].undecodedStr = strdup("0F 84 cw");
	instruction[457].dstOperandStr = strdup("rel16");
	instruction[457].dstOperand = IA_REL16;
	instruction[457].valid64 = INVALID;
	instruction[457].compatLeg = VALID;
	instruction[457].VMX = 0;
	instruction[457].description = strdup("Jump near if equal (ZF=1). Not supported in 64-bit mode.");
	instruction[457].binPrefixLen = 0;
	instruction[457].binOpcodeLen = 2;
	instruction[457].binOpcode = bpstrndup("\x0F\x84", 2);
	instruction[457].opcodeOperand[0] = OA_CW;
	instruction[457].opcodeOperandCount = 1;

	// Instruction Struct [458] - JE
	instruction[458].instructionStr = strdup("JE");
	instruction[458].undecodedStr = strdup("0F 84 cd");
	instruction[458].dstOperandStr = strdup("rel32");
	instruction[458].dstOperand = IA_REL32;
	instruction[458].valid64 = INVALID;
	instruction[458].compatLeg = VALID;
	instruction[458].VMX = 0;
	instruction[458].description = strdup("Jump near if equal (ZF=1).");
	instruction[458].binPrefixLen = 0;
	instruction[458].binOpcodeLen = 2;
	instruction[458].binOpcode = bpstrndup("\x0F\x84", 2);
	instruction[458].opcodeOperand[0] = OA_CD;
	instruction[458].opcodeOperandCount = 1;

	// Instruction Struct [459] - JZ
	instruction[459].instructionStr = strdup("JZ");
	instruction[459].undecodedStr = strdup("0F 84 cw");
	instruction[459].dstOperandStr = strdup("rel16");
	instruction[459].dstOperand = IA_REL16;
	instruction[459].valid64 = INVALID;
	instruction[459].compatLeg = VALID;
	instruction[459].VMX = 0;
	instruction[459].description = strdup("Jump near if 0 (ZF=1). Not supported in 64-bit mode.");
	instruction[459].binPrefixLen = 0;
	instruction[459].binOpcodeLen = 2;
	instruction[459].binOpcode = bpstrndup("\x0F\x84", 2);
	instruction[459].opcodeOperand[0] = OA_CW;
	instruction[459].opcodeOperandCount = 1;

	// Instruction Struct [460] - JZ
	instruction[460].instructionStr = strdup("JZ");
	instruction[460].undecodedStr = strdup("0F 84 cd");
	instruction[460].dstOperandStr = strdup("rel32");
	instruction[460].dstOperand = IA_REL32;
	instruction[460].valid64 = INVALID;
	instruction[460].compatLeg = VALID;
	instruction[460].VMX = 0;
	instruction[460].description = strdup("Jump near if 0 (ZF=1).");
	instruction[460].binPrefixLen = 0;
	instruction[460].binOpcodeLen = 2;
	instruction[460].binOpcode = bpstrndup("\x0F\x84", 2);
	instruction[460].opcodeOperand[0] = OA_CD;
	instruction[460].opcodeOperandCount = 1;

	// Instruction Struct [461] - JG
	instruction[461].instructionStr = strdup("JG");
	instruction[461].undecodedStr = strdup("0F 8F cw");
	instruction[461].dstOperandStr = strdup("rel16");
	instruction[461].dstOperand = IA_REL16;
	instruction[461].valid64 = INVALID;
	instruction[461].compatLeg = VALID;
	instruction[461].VMX = 0;
	instruction[461].description = strdup("Jump near if greater (ZF=0 and SF=OF). Not supported in 64-bit mode.");
	instruction[461].binPrefixLen = 0;
	instruction[461].binOpcodeLen = 2;
	instruction[461].binOpcode = bpstrndup("\x0F\x8F", 2);
	instruction[461].opcodeOperand[0] = OA_CW;
	instruction[461].opcodeOperandCount = 1;

	// Instruction Struct [462] - JG
	instruction[462].instructionStr = strdup("JG");
	instruction[462].undecodedStr = strdup("0F 8F cd");
	instruction[462].dstOperandStr = strdup("rel32");
	instruction[462].dstOperand = IA_REL32;
	instruction[462].valid64 = INVALID;
	instruction[462].compatLeg = VALID;
	instruction[462].VMX = 0;
	instruction[462].description = strdup("Jump near if greater (ZF=0 and SF=OF).");
	instruction[462].binPrefixLen = 0;
	instruction[462].binOpcodeLen = 2;
	instruction[462].binOpcode = bpstrndup("\x0F\x8F", 2);
	instruction[462].opcodeOperand[0] = OA_CD;
	instruction[462].opcodeOperandCount = 1;

	// Instruction Struct [463] - JGE
	instruction[463].instructionStr = strdup("JGE");
	instruction[463].undecodedStr = strdup("0F 8D cw");
	instruction[463].dstOperandStr = strdup("rel16");
	instruction[463].dstOperand = IA_REL16;
	instruction[463].valid64 = INVALID;
	instruction[463].compatLeg = VALID;
	instruction[463].VMX = 0;
	instruction[463].description = strdup("Jump near if greater or equal (SF=OF). Not supported in 64-bit mode.");
	instruction[463].binPrefixLen = 0;
	instruction[463].binOpcodeLen = 2;
	instruction[463].binOpcode = bpstrndup("\x0F\x8D", 2);
	instruction[463].opcodeOperand[0] = OA_CW;
	instruction[463].opcodeOperandCount = 1;

	// Instruction Struct [464] - JGE
	instruction[464].instructionStr = strdup("JGE");
	instruction[464].undecodedStr = strdup("0F 8D cd");
	instruction[464].dstOperandStr = strdup("rel32");
	instruction[464].dstOperand = IA_REL32;
	instruction[464].valid64 = INVALID;
	instruction[464].compatLeg = VALID;
	instruction[464].VMX = 0;
	instruction[464].description = strdup("Jump near if greater or equal (SF=OF).");
	instruction[464].binPrefixLen = 0;
	instruction[464].binOpcodeLen = 2;
	instruction[464].binOpcode = bpstrndup("\x0F\x8D", 2);
	instruction[464].opcodeOperand[0] = OA_CD;
	instruction[464].opcodeOperandCount = 1;

	// Instruction Struct [465] - JL
	instruction[465].instructionStr = strdup("JL");
	instruction[465].undecodedStr = strdup("0F 8C cw");
	instruction[465].dstOperandStr = strdup("rel16");
	instruction[465].dstOperand = IA_REL16;
	instruction[465].valid64 = INVALID;
	instruction[465].compatLeg = VALID;
	instruction[465].VMX = 0;
	instruction[465].description = strdup("Jump near if less (SF? OF). Not supported in 64-bit mode.");
	instruction[465].binPrefixLen = 0;
	instruction[465].binOpcodeLen = 2;
	instruction[465].binOpcode = bpstrndup("\x0F\x8C", 2);
	instruction[465].opcodeOperand[0] = OA_CW;
	instruction[465].opcodeOperandCount = 1;

	// Instruction Struct [466] - JL
	instruction[466].instructionStr = strdup("JL");
	instruction[466].undecodedStr = strdup("0F 8C cd");
	instruction[466].dstOperandStr = strdup("rel32");
	instruction[466].dstOperand = IA_REL32;
	instruction[466].valid64 = INVALID;
	instruction[466].compatLeg = VALID;
	instruction[466].VMX = 0;
	instruction[466].description = strdup("Jump near if less (SF? OF).");
	instruction[466].binPrefixLen = 0;
	instruction[466].binOpcodeLen = 2;
	instruction[466].binOpcode = bpstrndup("\x0F\x8C", 2);
	instruction[466].opcodeOperand[0] = OA_CD;
	instruction[466].opcodeOperandCount = 1;

	// Instruction Struct [467] - JLE
	instruction[467].instructionStr = strdup("JLE");
	instruction[467].undecodedStr = strdup("0F 8E cw");
	instruction[467].dstOperandStr = strdup("rel16");
	instruction[467].dstOperand = IA_REL16;
	instruction[467].valid64 = INVALID;
	instruction[467].compatLeg = VALID;
	instruction[467].VMX = 0;
	instruction[467].description = strdup("Jump near if less or equal (ZF=1 or SF? OF). Not supported in 64-bit mode.");
	instruction[467].binPrefixLen = 0;
	instruction[467].binOpcodeLen = 2;
	instruction[467].binOpcode = bpstrndup("\x0F\x8E", 2);
	instruction[467].opcodeOperand[0] = OA_CW;
	instruction[467].opcodeOperandCount = 1;

	// Instruction Struct [468] - JLE rel32
	instruction[468].instructionStr = strdup("JLE rel32");
	instruction[468].undecodedStr = strdup("0F 8E cd");
	instruction[468].valid64 = INVALID;
	instruction[468].compatLeg = VALID;
	instruction[468].VMX = 0;
	instruction[468].description = strdup("Jump near if less or equal (ZF=1 or SF? OF).");
	instruction[468].binPrefixLen = 0;
	instruction[468].binOpcodeLen = 2;
	instruction[468].binOpcode = bpstrndup("\x0F\x8E", 2);
	instruction[468].opcodeOperand[0] = OA_CD;
	instruction[468].opcodeOperandCount = 1;

	// Instruction Struct [469] - JNA
	instruction[469].instructionStr = strdup("JNA");
	instruction[469].undecodedStr = strdup("0F 86 cw");
	instruction[469].dstOperandStr = strdup("rel16");
	instruction[469].dstOperand = IA_REL16;
	instruction[469].valid64 = INVALID;
	instruction[469].compatLeg = VALID;
	instruction[469].VMX = 0;
	instruction[469].description = strdup("Jump near if not above (CF=1 or ZF=1). Not supported in 64-bit mode.");
	instruction[469].binPrefixLen = 0;
	instruction[469].binOpcodeLen = 2;
	instruction[469].binOpcode = bpstrndup("\x0F\x86", 2);
	instruction[469].opcodeOperand[0] = OA_CW;
	instruction[469].opcodeOperandCount = 1;

	// Instruction Struct [470] - JNA
	instruction[470].instructionStr = strdup("JNA");
	instruction[470].undecodedStr = strdup("0F 86 cd");
	instruction[470].dstOperandStr = strdup("rel32");
	instruction[470].dstOperand = IA_REL32;
	instruction[470].valid64 = INVALID;
	instruction[470].compatLeg = VALID;
	instruction[470].VMX = 0;
	instruction[470].description = strdup("Jump near if not above (CF=1 or ZF=1).");
	instruction[470].binPrefixLen = 0;
	instruction[470].binOpcodeLen = 2;
	instruction[470].binOpcode = bpstrndup("\x0F\x86", 2);
	instruction[470].opcodeOperand[0] = OA_CD;
	instruction[470].opcodeOperandCount = 1;

	// Instruction Struct [471] - JNAE
	instruction[471].instructionStr = strdup("JNAE");
	instruction[471].undecodedStr = strdup("0F 82 cw");
	instruction[471].dstOperandStr = strdup("rel16");
	instruction[471].dstOperand = IA_REL16;
	instruction[471].valid64 = INVALID;
	instruction[471].compatLeg = VALID;
	instruction[471].VMX = 0;
	instruction[471].description = strdup("Jump near if not above or equal (CF=1). Not supported in 64-bit mode.");
	instruction[471].binPrefixLen = 0;
	instruction[471].binOpcodeLen = 2;
	instruction[471].binOpcode = bpstrndup("\x0F\x82", 2);
	instruction[471].opcodeOperand[0] = OA_CW;
	instruction[471].opcodeOperandCount = 1;

	// Instruction Struct [472] - JNAE
	instruction[472].instructionStr = strdup("JNAE");
	instruction[472].undecodedStr = strdup("0F 82 cd");
	instruction[472].dstOperandStr = strdup("rel32");
	instruction[472].dstOperand = IA_REL32;
	instruction[472].valid64 = INVALID;
	instruction[472].compatLeg = VALID;
	instruction[472].VMX = 0;
	instruction[472].description = strdup("Jump near if not above or equal (CF=1).");
	instruction[472].binPrefixLen = 0;
	instruction[472].binOpcodeLen = 2;
	instruction[472].binOpcode = bpstrndup("\x0F\x82", 2);
	instruction[472].opcodeOperand[0] = OA_CD;
	instruction[472].opcodeOperandCount = 1;

	// Instruction Struct [473] - JNB
	instruction[473].instructionStr = strdup("JNB");
	instruction[473].undecodedStr = strdup("0F 83 cw");
	instruction[473].dstOperandStr = strdup("rel16");
	instruction[473].dstOperand = IA_REL16;
	instruction[473].valid64 = INVALID;
	instruction[473].compatLeg = VALID;
	instruction[473].VMX = 0;
	instruction[473].description = strdup("Jump near if not below (CF=0). Not supported in 64-bit mode.");
	instruction[473].binPrefixLen = 0;
	instruction[473].binOpcodeLen = 2;
	instruction[473].binOpcode = bpstrndup("\x0F\x83", 2);
	instruction[473].opcodeOperand[0] = OA_CW;
	instruction[473].opcodeOperandCount = 1;

	// Instruction Struct [474] - JNB
	instruction[474].instructionStr = strdup("JNB");
	instruction[474].undecodedStr = strdup("0F 83 cd");
	instruction[474].dstOperandStr = strdup("rel32");
	instruction[474].dstOperand = IA_REL32;
	instruction[474].valid64 = INVALID;
	instruction[474].compatLeg = VALID;
	instruction[474].VMX = 0;
	instruction[474].description = strdup("Jump near if not below (CF=0).");
	instruction[474].binPrefixLen = 0;
	instruction[474].binOpcodeLen = 2;
	instruction[474].binOpcode = bpstrndup("\x0F\x83", 2);
	instruction[474].opcodeOperand[0] = OA_CD;
	instruction[474].opcodeOperandCount = 1;

	// Instruction Struct [475] - JNBE
	instruction[475].instructionStr = strdup("JNBE");
	instruction[475].undecodedStr = strdup("0F 87 cw");
	instruction[475].dstOperandStr = strdup("rel16");
	instruction[475].dstOperand = IA_REL16;
	instruction[475].valid64 = INVALID;
	instruction[475].compatLeg = VALID;
	instruction[475].VMX = 0;
	instruction[475].description = strdup("Jump near if not below or equal (CF=0 and ZF=0). Not supported in 64-bit mode.");
	instruction[475].binPrefixLen = 0;
	instruction[475].binOpcodeLen = 2;
	instruction[475].binOpcode = bpstrndup("\x0F\x87", 2);
	instruction[475].opcodeOperand[0] = OA_CW;
	instruction[475].opcodeOperandCount = 1;

	// Instruction Struct [476] - JNBE
	instruction[476].instructionStr = strdup("JNBE");
	instruction[476].undecodedStr = strdup("0F 87 cd");
	instruction[476].dstOperandStr = strdup("rel32");
	instruction[476].dstOperand = IA_REL32;
	instruction[476].valid64 = INVALID;
	instruction[476].compatLeg = VALID;
	instruction[476].VMX = 0;
	instruction[476].description = strdup("Jump near if not below or equal (CF=0 and ZF=0).");
	instruction[476].binPrefixLen = 0;
	instruction[476].binOpcodeLen = 2;
	instruction[476].binOpcode = bpstrndup("\x0F\x87", 2);
	instruction[476].opcodeOperand[0] = OA_CD;
	instruction[476].opcodeOperandCount = 1;

	// Instruction Struct [477] - JNC
	instruction[477].instructionStr = strdup("JNC");
	instruction[477].undecodedStr = strdup("0F 83 cw");
	instruction[477].dstOperandStr = strdup("rel16");
	instruction[477].dstOperand = IA_REL16;
	instruction[477].valid64 = INVALID;
	instruction[477].compatLeg = VALID;
	instruction[477].VMX = 0;
	instruction[477].description = strdup("Jump near if not carry (CF=0). Not supported in 64-bit mode.");
	instruction[477].binPrefixLen = 0;
	instruction[477].binOpcodeLen = 2;
	instruction[477].binOpcode = bpstrndup("\x0F\x83", 2);
	instruction[477].opcodeOperand[0] = OA_CW;
	instruction[477].opcodeOperandCount = 1;

	// Instruction Struct [478] - JNC
	instruction[478].instructionStr = strdup("JNC");
	instruction[478].undecodedStr = strdup("0F 83 cd");
	instruction[478].dstOperandStr = strdup("rel32");
	instruction[478].dstOperand = IA_REL32;
	instruction[478].valid64 = INVALID;
	instruction[478].compatLeg = VALID;
	instruction[478].VMX = 0;
	instruction[478].description = strdup("Jump near if not carry (CF=0).");
	instruction[478].binPrefixLen = 0;
	instruction[478].binOpcodeLen = 2;
	instruction[478].binOpcode = bpstrndup("\x0F\x83", 2);
	instruction[478].opcodeOperand[0] = OA_CD;
	instruction[478].opcodeOperandCount = 1;

	// Instruction Struct [479] - JNE
	instruction[479].instructionStr = strdup("JNE");
	instruction[479].undecodedStr = strdup("0F 85 cw");
	instruction[479].dstOperandStr = strdup("rel16");
	instruction[479].dstOperand = IA_REL16;
	instruction[479].valid64 = INVALID;
	instruction[479].compatLeg = VALID;
	instruction[479].VMX = 0;
	instruction[479].description = strdup("Jump near if not equal (ZF=0). Not supported in 64-bit mode.");
	instruction[479].binPrefixLen = 0;
	instruction[479].binOpcodeLen = 2;
	instruction[479].binOpcode = bpstrndup("\x0F\x85", 2);
	instruction[479].opcodeOperand[0] = OA_CW;
	instruction[479].opcodeOperandCount = 1;

	// Instruction Struct [480] - JNE
	instruction[480].instructionStr = strdup("JNE");
	instruction[480].undecodedStr = strdup("0F 85 cd");
	instruction[480].dstOperandStr = strdup("rel32");
	instruction[480].dstOperand = IA_REL32;
	instruction[480].valid64 = INVALID;
	instruction[480].compatLeg = VALID;
	instruction[480].VMX = 0;
	instruction[480].description = strdup("Jump near if not equal (ZF=0).");
	instruction[480].binPrefixLen = 0;
	instruction[480].binOpcodeLen = 2;
	instruction[480].binOpcode = bpstrndup("\x0F\x85", 2);
	instruction[480].opcodeOperand[0] = OA_CD;
	instruction[480].opcodeOperandCount = 1;

	// Instruction Struct [481] - JNG
	instruction[481].instructionStr = strdup("JNG");
	instruction[481].undecodedStr = strdup("0F 8E cw");
	instruction[481].dstOperandStr = strdup("rel16");
	instruction[481].dstOperand = IA_REL16;
	instruction[481].valid64 = INVALID;
	instruction[481].compatLeg = VALID;
	instruction[481].VMX = 0;
	instruction[481].description = strdup("Jump near if not greater (ZF=1 or SF? OF). Not supported in 64-bit mode.");
	instruction[481].binPrefixLen = 0;
	instruction[481].binOpcodeLen = 2;
	instruction[481].binOpcode = bpstrndup("\x0F\x8E", 2);
	instruction[481].opcodeOperand[0] = OA_CW;
	instruction[481].opcodeOperandCount = 1;

	// Instruction Struct [482] - JNG
	instruction[482].instructionStr = strdup("JNG");
	instruction[482].undecodedStr = strdup("0F 8E cd");
	instruction[482].dstOperandStr = strdup("rel32");
	instruction[482].dstOperand = IA_REL32;
	instruction[482].valid64 = INVALID;
	instruction[482].compatLeg = VALID;
	instruction[482].VMX = 0;
	instruction[482].description = strdup("Jump near if not greater (ZF=1 or SF? OF).");
	instruction[482].binPrefixLen = 0;
	instruction[482].binOpcodeLen = 2;
	instruction[482].binOpcode = bpstrndup("\x0F\x8E", 2);
	instruction[482].opcodeOperand[0] = OA_CD;
	instruction[482].opcodeOperandCount = 1;

	// Instruction Struct [483] - JNGE
	instruction[483].instructionStr = strdup("JNGE");
	instruction[483].undecodedStr = strdup("0F 8C cw");
	instruction[483].dstOperandStr = strdup("rel16");
	instruction[483].dstOperand = IA_REL16;
	instruction[483].valid64 = INVALID;
	instruction[483].compatLeg = VALID;
	instruction[483].VMX = 0;
	instruction[483].description = strdup("Jump near if not greater or equal (SF? OF). Not supported in 64-bit mode.");
	instruction[483].binPrefixLen = 0;
	instruction[483].binOpcodeLen = 2;
	instruction[483].binOpcode = bpstrndup("\x0F\x8C", 2);
	instruction[483].opcodeOperand[0] = OA_CW;
	instruction[483].opcodeOperandCount = 1;

	// Instruction Struct [484] - JNGE
	instruction[484].instructionStr = strdup("JNGE");
	instruction[484].undecodedStr = strdup("0F 8C cd");
	instruction[484].dstOperandStr = strdup("rel32");
	instruction[484].dstOperand = IA_REL32;
	instruction[484].valid64 = INVALID;
	instruction[484].compatLeg = VALID;
	instruction[484].VMX = 0;
	instruction[484].description = strdup("Jump near if not greater or equal (SF? OF).");
	instruction[484].binPrefixLen = 0;
	instruction[484].binOpcodeLen = 2;
	instruction[484].binOpcode = bpstrndup("\x0F\x8C", 2);
	instruction[484].opcodeOperand[0] = OA_CD;
	instruction[484].opcodeOperandCount = 1;

	// Instruction Struct [485] - JNL
	instruction[485].instructionStr = strdup("JNL");
	instruction[485].undecodedStr = strdup("0F 8D cw");
	instruction[485].dstOperandStr = strdup("rel16");
	instruction[485].dstOperand = IA_REL16;
	instruction[485].valid64 = INVALID;
	instruction[485].compatLeg = VALID;
	instruction[485].VMX = 0;
	instruction[485].description = strdup("Jump near if not less (SF=OF). Not supported in 64-bit mode.");
	instruction[485].binPrefixLen = 0;
	instruction[485].binOpcodeLen = 2;
	instruction[485].binOpcode = bpstrndup("\x0F\x8D", 2);
	instruction[485].opcodeOperand[0] = OA_CW;
	instruction[485].opcodeOperandCount = 1;

	// Instruction Struct [486] - JNL
	instruction[486].instructionStr = strdup("JNL");
	instruction[486].undecodedStr = strdup("0F 8D cd");
	instruction[486].dstOperandStr = strdup("rel32");
	instruction[486].dstOperand = IA_REL32;
	instruction[486].valid64 = INVALID;
	instruction[486].compatLeg = VALID;
	instruction[486].VMX = 0;
	instruction[486].description = strdup("Jump near if not less (SF=OF).");
	instruction[486].binPrefixLen = 0;
	instruction[486].binOpcodeLen = 2;
	instruction[486].binOpcode = bpstrndup("\x0F\x8D", 2);
	instruction[486].opcodeOperand[0] = OA_CD;
	instruction[486].opcodeOperandCount = 1;

	// Instruction Struct [487] - JNLE
	instruction[487].instructionStr = strdup("JNLE");
	instruction[487].undecodedStr = strdup("0F 8F cd");
	instruction[487].dstOperandStr = strdup("rel32");
	instruction[487].dstOperand = IA_REL32;
	instruction[487].valid64 = INVALID;
	instruction[487].compatLeg = VALID;
	instruction[487].VMX = 0;
	instruction[487].description = strdup("Jump near if not less or equal (ZF=0 and SF=OF).");
	instruction[487].binPrefixLen = 0;
	instruction[487].binOpcodeLen = 2;
	instruction[487].binOpcode = bpstrndup("\x0F\x8F", 2);
	instruction[487].opcodeOperand[0] = OA_CD;
	instruction[487].opcodeOperandCount = 1;

	// Instruction Struct [488] - JNO
	instruction[488].instructionStr = strdup("JNO");
	instruction[488].undecodedStr = strdup("0F 81 cw");
	instruction[488].dstOperandStr = strdup("rel16");
	instruction[488].dstOperand = IA_REL16;
	instruction[488].valid64 = INVALID;
	instruction[488].compatLeg = VALID;
	instruction[488].VMX = 0;
	instruction[488].description = strdup("Jump near if not overflow (OF=0). Not supported in 64-bit mode.");
	instruction[488].binPrefixLen = 0;
	instruction[488].binOpcodeLen = 2;
	instruction[488].binOpcode = bpstrndup("\x0F\x81", 2);
	instruction[488].opcodeOperand[0] = OA_CW;
	instruction[488].opcodeOperandCount = 1;

	// Instruction Struct [489] - JNO
	instruction[489].instructionStr = strdup("JNO");
	instruction[489].undecodedStr = strdup("0F 81 cd");
	instruction[489].dstOperandStr = strdup("rel32");
	instruction[489].dstOperand = IA_REL32;
	instruction[489].valid64 = INVALID;
	instruction[489].compatLeg = VALID;
	instruction[489].VMX = 0;
	instruction[489].description = strdup("Jump near if not overflow (OF=0).");
	instruction[489].binPrefixLen = 0;
	instruction[489].binOpcodeLen = 2;
	instruction[489].binOpcode = bpstrndup("\x0F\x81", 2);
	instruction[489].opcodeOperand[0] = OA_CD;
	instruction[489].opcodeOperandCount = 1;

	// Instruction Struct [490] - JNP
	instruction[490].instructionStr = strdup("JNP");
	instruction[490].undecodedStr = strdup("0F 8B cw");
	instruction[490].dstOperandStr = strdup("rel16");
	instruction[490].dstOperand = IA_REL16;
	instruction[490].valid64 = INVALID;
	instruction[490].compatLeg = VALID;
	instruction[490].VMX = 0;
	instruction[490].description = strdup("Jump near if not parity (PF=0). Not supported in 64-bit mode.");
	instruction[490].binPrefixLen = 0;
	instruction[490].binOpcodeLen = 2;
	instruction[490].binOpcode = bpstrndup("\x0F\x8B", 2);
	instruction[490].opcodeOperand[0] = OA_CW;
	instruction[490].opcodeOperandCount = 1;

	// Instruction Struct [491] - JNP
	instruction[491].instructionStr = strdup("JNP");
	instruction[491].undecodedStr = strdup("0F 8B cd");
	instruction[491].dstOperandStr = strdup("rel32");
	instruction[491].dstOperand = IA_REL32;
	instruction[491].valid64 = INVALID;
	instruction[491].compatLeg = VALID;
	instruction[491].VMX = 0;
	instruction[491].description = strdup("Jump near if not parity (PF=0).");
	instruction[491].binPrefixLen = 0;
	instruction[491].binOpcodeLen = 2;
	instruction[491].binOpcode = bpstrndup("\x0F\x8B", 2);
	instruction[491].opcodeOperand[0] = OA_CD;
	instruction[491].opcodeOperandCount = 1;

	// Instruction Struct [492] - JNS
	instruction[492].instructionStr = strdup("JNS");
	instruction[492].undecodedStr = strdup("0F 89 cw");
	instruction[492].dstOperandStr = strdup("rel16");
	instruction[492].dstOperand = IA_REL16;
	instruction[492].valid64 = INVALID;
	instruction[492].compatLeg = VALID;
	instruction[492].VMX = 0;
	instruction[492].description = strdup("Jump near if not sign (SF=0). Not supported in 64-bit mode.");
	instruction[492].binPrefixLen = 0;
	instruction[492].binOpcodeLen = 2;
	instruction[492].binOpcode = bpstrndup("\x0F\x89", 2);
	instruction[492].opcodeOperand[0] = OA_CW;
	instruction[492].opcodeOperandCount = 1;

	// Instruction Struct [493] - JNS
	instruction[493].instructionStr = strdup("JNS");
	instruction[493].undecodedStr = strdup("0F 89 cd");
	instruction[493].dstOperandStr = strdup("rel32");
	instruction[493].dstOperand = IA_REL32;
	instruction[493].valid64 = INVALID;
	instruction[493].compatLeg = VALID;
	instruction[493].VMX = 0;
	instruction[493].description = strdup("Jump near if not sign (SF=0).");
	instruction[493].binPrefixLen = 0;
	instruction[493].binOpcodeLen = 2;
	instruction[493].binOpcode = bpstrndup("\x0F\x89", 2);
	instruction[493].opcodeOperand[0] = OA_CD;
	instruction[493].opcodeOperandCount = 1;

	// Instruction Struct [494] - JNZ
	instruction[494].instructionStr = strdup("JNZ");
	instruction[494].undecodedStr = strdup("0F 85 cw");
	instruction[494].dstOperandStr = strdup("rel16");
	instruction[494].dstOperand = IA_REL16;
	instruction[494].valid64 = INVALID;
	instruction[494].compatLeg = VALID;
	instruction[494].VMX = 0;
	instruction[494].description = strdup("Jump near if not zero (ZF=0). Not supported in 64-bit mode.");
	instruction[494].binPrefixLen = 0;
	instruction[494].binOpcodeLen = 2;
	instruction[494].binOpcode = bpstrndup("\x0F\x85", 2);
	instruction[494].opcodeOperand[0] = OA_CW;
	instruction[494].opcodeOperandCount = 1;

	// Instruction Struct [495] - JNZ
	instruction[495].instructionStr = strdup("JNZ");
	instruction[495].undecodedStr = strdup("0F 85 cd");
	instruction[495].dstOperandStr = strdup("rel32");
	instruction[495].dstOperand = IA_REL32;
	instruction[495].valid64 = INVALID;
	instruction[495].compatLeg = VALID;
	instruction[495].VMX = 0;
	instruction[495].description = strdup("Jump near if not zero (ZF=0).");
	instruction[495].binPrefixLen = 0;
	instruction[495].binOpcodeLen = 2;
	instruction[495].binOpcode = bpstrndup("\x0F\x85", 2);
	instruction[495].opcodeOperand[0] = OA_CD;
	instruction[495].opcodeOperandCount = 1;

	// Instruction Struct [496] - JO
	instruction[496].instructionStr = strdup("JO");
	instruction[496].undecodedStr = strdup("0F 80 cw");
	instruction[496].dstOperandStr = strdup("rel16");
	instruction[496].dstOperand = IA_REL16;
	instruction[496].valid64 = INVALID;
	instruction[496].compatLeg = VALID;
	instruction[496].VMX = 0;
	instruction[496].description = strdup("Jump near if overflow (OF=1). Not supported in 64-bit mode.");
	instruction[496].binPrefixLen = 0;
	instruction[496].binOpcodeLen = 2;
	instruction[496].binOpcode = bpstrndup("\x0F\x80", 2);
	instruction[496].opcodeOperand[0] = OA_CW;
	instruction[496].opcodeOperandCount = 1;

	// Instruction Struct [497] - JO
	instruction[497].instructionStr = strdup("JO");
	instruction[497].undecodedStr = strdup("0F 80 cd");
	instruction[497].dstOperandStr = strdup("rel32");
	instruction[497].dstOperand = IA_REL32;
	instruction[497].valid64 = INVALID;
	instruction[497].compatLeg = VALID;
	instruction[497].VMX = 0;
	instruction[497].description = strdup("Jump near if overflow (OF=1).");
	instruction[497].binPrefixLen = 0;
	instruction[497].binOpcodeLen = 2;
	instruction[497].binOpcode = bpstrndup("\x0F\x80", 2);
	instruction[497].opcodeOperand[0] = OA_CD;
	instruction[497].opcodeOperandCount = 1;

	// Instruction Struct [498] - 0F 8A cw
	instruction[498].instructionStr = strdup("0F 8A cw");
	instruction[498].undecodedStr = strdup("0F 8A cw");
	instruction[498].dstOperandStr = strdup("rel16");
	instruction[498].dstOperand = IA_REL16;
	instruction[498].valid64 = INVALID;
	instruction[498].compatLeg = VALID;
	instruction[498].VMX = 0;
	instruction[498].description = strdup("Jump near if parity (PF=1). Not supported in 64-bit mode.");
	instruction[498].binPrefixLen = 0;
	instruction[498].binOpcodeLen = 2;
	instruction[498].binOpcode = bpstrndup("\x0F\x8A", 2);
	instruction[498].opcodeOperand[0] = OA_CW;
	instruction[498].opcodeOperandCount = 1;

	// Instruction Struct [499] - JP
	instruction[499].instructionStr = strdup("JP");
	instruction[499].undecodedStr = strdup("0F 8A cd");
	instruction[499].dstOperandStr = strdup("rel32");
	instruction[499].dstOperand = IA_REL32;
	instruction[499].valid64 = INVALID;
	instruction[499].compatLeg = VALID;
	instruction[499].VMX = 0;
	instruction[499].description = strdup("Jump near if parity (PF=1).");
	instruction[499].binPrefixLen = 0;
	instruction[499].binOpcodeLen = 2;
	instruction[499].binOpcode = bpstrndup("\x0F\x8A", 2);
	instruction[499].opcodeOperand[0] = OA_CD;
	instruction[499].opcodeOperandCount = 1;

	// Instruction Struct [500] - JPE
	instruction[500].instructionStr = strdup("JPE");
	instruction[500].undecodedStr = strdup("0F 8A cw");
	instruction[500].dstOperandStr = strdup("rel16");
	instruction[500].dstOperand = IA_REL16;
	instruction[500].valid64 = INVALID;
	instruction[500].compatLeg = VALID;
	instruction[500].VMX = 0;
	instruction[500].description = strdup("Jump near if parity even (PF=1). Not supported in 64-bit mode.");
	instruction[500].binPrefixLen = 0;
	instruction[500].binOpcodeLen = 2;
	instruction[500].binOpcode = bpstrndup("\x0F\x8A", 2);
	instruction[500].opcodeOperand[0] = OA_CW;
	instruction[500].opcodeOperandCount = 1;

	// Instruction Struct [501] - JPE
	instruction[501].instructionStr = strdup("JPE");
	instruction[501].undecodedStr = strdup("0F 8A cd");
	instruction[501].dstOperandStr = strdup("rel32");
	instruction[501].dstOperand = IA_REL32;
	instruction[501].valid64 = INVALID;
	instruction[501].compatLeg = VALID;
	instruction[501].VMX = 0;
	instruction[501].description = strdup("Jump near if parity even (PF=1).");
	instruction[501].binPrefixLen = 0;
	instruction[501].binOpcodeLen = 2;
	instruction[501].binOpcode = bpstrndup("\x0F\x8A", 2);
	instruction[501].opcodeOperand[0] = OA_CD;
	instruction[501].opcodeOperandCount = 1;

	// Instruction Struct [502] - JPO
	instruction[502].instructionStr = strdup("JPO");
	instruction[502].undecodedStr = strdup("0F 8B cw");
	instruction[502].dstOperandStr = strdup("rel16");
	instruction[502].dstOperand = IA_REL16;
	instruction[502].valid64 = INVALID;
	instruction[502].compatLeg = VALID;
	instruction[502].VMX = 0;
	instruction[502].description = strdup("Jump near if parity odd (PF=0). Not supported in 64-bit mode.");
	instruction[502].binPrefixLen = 0;
	instruction[502].binOpcodeLen = 2;
	instruction[502].binOpcode = bpstrndup("\x0F\x8B", 2);
	instruction[502].opcodeOperand[0] = OA_CW;
	instruction[502].opcodeOperandCount = 1;

	// Instruction Struct [503] - JPO
	instruction[503].instructionStr = strdup("JPO");
	instruction[503].undecodedStr = strdup("0F 8B cd");
	instruction[503].dstOperandStr = strdup("rel32");
	instruction[503].dstOperand = IA_REL32;
	instruction[503].valid64 = INVALID;
	instruction[503].compatLeg = VALID;
	instruction[503].VMX = 0;
	instruction[503].description = strdup("Jump near if parity odd (PF=0).");
	instruction[503].binPrefixLen = 0;
	instruction[503].binOpcodeLen = 2;
	instruction[503].binOpcode = bpstrndup("\x0F\x8B", 2);
	instruction[503].opcodeOperand[0] = OA_CD;
	instruction[503].opcodeOperandCount = 1;

	// Instruction Struct [504] - JS
	instruction[504].instructionStr = strdup("JS");
	instruction[504].undecodedStr = strdup("0F 88 cw");
	instruction[504].dstOperandStr = strdup("rel16");
	instruction[504].dstOperand = IA_REL16;
	instruction[504].valid64 = INVALID;
	instruction[504].compatLeg = VALID;
	instruction[504].VMX = 0;
	instruction[504].description = strdup("Jump near if sign (SF=1). Not supported in 64-bit mode.");
	instruction[504].binPrefixLen = 0;
	instruction[504].binOpcodeLen = 2;
	instruction[504].binOpcode = bpstrndup("\x0F\x88", 2);
	instruction[504].opcodeOperand[0] = OA_CW;
	instruction[504].opcodeOperandCount = 1;

	// Instruction Struct [505] - JS
	instruction[505].instructionStr = strdup("JS");
	instruction[505].undecodedStr = strdup("0F 88 cd");
	instruction[505].dstOperandStr = strdup("rel32");
	instruction[505].dstOperand = IA_REL32;
	instruction[505].valid64 = INVALID;
	instruction[505].compatLeg = VALID;
	instruction[505].VMX = 0;
	instruction[505].description = strdup("Jump near if sign (SF=1).");
	instruction[505].binPrefixLen = 0;
	instruction[505].binOpcodeLen = 2;
	instruction[505].binOpcode = bpstrndup("\x0F\x88", 2);
	instruction[505].opcodeOperand[0] = OA_CD;
	instruction[505].opcodeOperandCount = 1;

	// Instruction Struct [506] - JZ
	instruction[506].instructionStr = strdup("JZ");
	instruction[506].undecodedStr = strdup("0F 84 cw");
	instruction[506].dstOperandStr = strdup("rel16");
	instruction[506].dstOperand = IA_REL16;
	instruction[506].valid64 = INVALID;
	instruction[506].compatLeg = VALID;
	instruction[506].VMX = 0;
	instruction[506].description = strdup("Jump near if 0 (ZF=1). Not supported in 64-bit mode.");
	instruction[506].binPrefixLen = 0;
	instruction[506].binOpcodeLen = 2;
	instruction[506].binOpcode = bpstrndup("\x0F\x84", 2);
	instruction[506].opcodeOperand[0] = OA_CW;
	instruction[506].opcodeOperandCount = 1;

	// Instruction Struct [507] - JZ
	instruction[507].instructionStr = strdup("JZ");
	instruction[507].undecodedStr = strdup("0F 84 cd");
	instruction[507].dstOperandStr = strdup("rel32");
	instruction[507].dstOperand = IA_REL32;
	instruction[507].valid64 = INVALID;
	instruction[507].compatLeg = VALID;
	instruction[507].VMX = 0;
	instruction[507].description = strdup("Jump near if 0 (ZF=1).");
	instruction[507].binPrefixLen = 0;
	instruction[507].binOpcodeLen = 2;
	instruction[507].binOpcode = bpstrndup("\x0F\x84", 2);
	instruction[507].opcodeOperand[0] = OA_CD;
	instruction[507].opcodeOperandCount = 1;

	// Instruction Struct [508] - JMP
	instruction[508].instructionStr = strdup("JMP");
	instruction[508].undecodedStr = strdup("EB cb");
	instruction[508].dstOperandStr = strdup("rel8");
	instruction[508].dstOperand = IA_REL8;
	instruction[508].valid64 = INVALID;
	instruction[508].compatLeg = VALID;
	instruction[508].VMX = 0;
	instruction[508].description = strdup("Jump short, RIP = RIP + 8-bit displacement sign extended to 64-bits");
	instruction[508].binPrefixLen = 0;
	instruction[508].binOpcodeLen = 1;
	instruction[508].binOpcode = bpstrndup("\xEB", 1);
	instruction[508].opcodeOperand[0] = OA_CB;
	instruction[508].opcodeOperandCount = 1;

	// Instruction Struct [509] - JMP
	instruction[509].instructionStr = strdup("JMP");
	instruction[509].undecodedStr = strdup("E9 cw");
	instruction[509].dstOperandStr = strdup("rel16");
	instruction[509].dstOperand = IA_REL16;
	instruction[509].valid64 = INVALID;
	instruction[509].compatLeg = VALID;
	instruction[509].VMX = 0;
	instruction[509].description = strdup("Jump near, relative, displacement relative to next instruction. Not supported in 64-bit mode.");
	instruction[509].binPrefixLen = 0;
	instruction[509].binOpcodeLen = 1;
	instruction[509].binOpcode = bpstrndup("\xE9", 1);
	instruction[509].opcodeOperand[0] = OA_CW;
	instruction[509].opcodeOperandCount = 1;

	// Instruction Struct [510] - JMP
	instruction[510].instructionStr = strdup("JMP");
	instruction[510].undecodedStr = strdup("E9 cd");
	instruction[510].dstOperandStr = strdup("rel32");
	instruction[510].dstOperand = IA_REL32;
	instruction[510].valid64 = INVALID;
	instruction[510].compatLeg = VALID;
	instruction[510].VMX = 0;
	instruction[510].description = strdup("Jump near, relative, RIP = RIP + 32-bit displacement sign extended to 64-bits");
	instruction[510].binPrefixLen = 0;
	instruction[510].binOpcodeLen = 1;
	instruction[510].binOpcode = bpstrndup("\xE9", 1);
	instruction[510].opcodeOperand[0] = OA_CD;
	instruction[510].opcodeOperandCount = 1;

	// Instruction Struct [511] - JMP
	instruction[511].instructionStr = strdup("JMP");
	instruction[511].undecodedStr = strdup("FF /4");
	instruction[511].dstOperandStr = strdup("r/m16");
	instruction[511].dstOperand = IA_RM16;
	instruction[511].valid64 = INVALID;
	instruction[511].compatLeg = VALID;
	instruction[511].VMX = 0;
	instruction[511].description = strdup("Jump near, absolute indirect, address = sign-extended r/m16. Not supported in 64- bit mode.");
	instruction[511].binPrefixLen = 0;
	instruction[511].binOpcodeLen = 1;
	instruction[511].binOpcode = bpstrndup("\xFF", 1);
	instruction[511].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[511].opcodeOperandCount = 1;

	// Instruction Struct [512] - JMP
	instruction[512].instructionStr = strdup("JMP");
	instruction[512].undecodedStr = strdup("FF /4");
	instruction[512].dstOperandStr = strdup("r/m32");
	instruction[512].dstOperand = IA_RM32;
	instruction[512].valid64 = INVALID;
	instruction[512].compatLeg = VALID;
	instruction[512].VMX = 0;
	instruction[512].description = strdup("Jump near, absolute indirect, address = sign-extended r/m32. Not supported in 64- bit mode.");
	instruction[512].binPrefixLen = 0;
	instruction[512].binOpcodeLen = 1;
	instruction[512].binOpcode = bpstrndup("\xFF", 1);
	instruction[512].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[512].opcodeOperandCount = 1;

	// Instruction Struct [513] - JMP
	instruction[513].instructionStr = strdup("JMP");
	instruction[513].undecodedStr = strdup("FF /4");
	instruction[513].dstOperandStr = strdup("r/m64");
	instruction[513].dstOperand = IA_RM64;
	instruction[513].valid64 = INVALID;
	instruction[513].compatLeg = INVALID;
	instruction[513].VMX = 0;
	instruction[513].description = strdup("Jump near, absolute indirect, RIP = 64-Bit offset from register or memory");
	instruction[513].binPrefixLen = 0;
	instruction[513].binOpcodeLen = 1;
	instruction[513].binOpcode = bpstrndup("\xFF", 1);
	instruction[513].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[513].opcodeOperandCount = 1;

	// Instruction Struct [514] - JMP
	instruction[514].instructionStr = strdup("JMP");
	instruction[514].undecodedStr = strdup("EA cd");
	instruction[514].dstOperandStr = strdup("ptr16:16");
	instruction[514].dstOperand = IA_PTR16_16;
	instruction[514].valid64 = INVALID;
	instruction[514].compatLeg = VALID;
	instruction[514].VMX = 0;
	instruction[514].description = strdup("Jump far, absolute, address given in operand");
	instruction[514].binPrefixLen = 0;
	instruction[514].binOpcodeLen = 1;
	instruction[514].binOpcode = bpstrndup("\xEA", 1);
	instruction[514].opcodeOperand[0] = OA_CD;
	instruction[514].opcodeOperandCount = 1;

	// Instruction Struct [515] - JMP
	instruction[515].instructionStr = strdup("JMP");
	instruction[515].undecodedStr = strdup("EA cp");
	instruction[515].dstOperandStr = strdup("ptr16:32");
	instruction[515].dstOperand = IA_PTR16_32;
	instruction[515].valid64 = INVALID;
	instruction[515].compatLeg = VALID;
	instruction[515].VMX = 0;
	instruction[515].description = strdup("Jump far, absolute, address given in operand");
	instruction[515].binPrefixLen = 0;
	instruction[515].binOpcodeLen = 1;
	instruction[515].binOpcode = bpstrndup("\xEA", 1);
	instruction[515].opcodeOperand[0] = OA_CP;
	instruction[515].opcodeOperandCount = 1;

	// Instruction Struct [516] - JMP
	instruction[516].instructionStr = strdup("JMP");
	instruction[516].undecodedStr = strdup("FF /5");
	instruction[516].dstOperandStr = strdup("m16:16");
	instruction[516].dstOperand = IA_M16_16;
	instruction[516].valid64 = INVALID;
	instruction[516].compatLeg = VALID;
	instruction[516].VMX = 0;
	instruction[516].description = strdup("Jump far, absolute indirect, address given in m16:16");
	instruction[516].binPrefixLen = 0;
	instruction[516].binOpcodeLen = 1;
	instruction[516].binOpcode = bpstrndup("\xFF", 1);
	instruction[516].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[516].opcodeOperandCount = 1;

	// Instruction Struct [517] - JMP
	instruction[517].instructionStr = strdup("JMP");
	instruction[517].undecodedStr = strdup("FF /5");
	instruction[517].dstOperandStr = strdup("m16:32");
	instruction[517].dstOperand = IA_M16_32;
	instruction[517].valid64 = INVALID;
	instruction[517].compatLeg = VALID;
	instruction[517].VMX = 0;
	instruction[517].description = strdup("Jump far, absolute indirect, address given in m16:32.");
	instruction[517].binPrefixLen = 0;
	instruction[517].binOpcodeLen = 1;
	instruction[517].binOpcode = bpstrndup("\xFF", 1);
	instruction[517].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[517].opcodeOperandCount = 1;

	// Instruction Struct [518] - LAHF
	instruction[518].instructionStr = strdup("LAHF");
	instruction[518].undecodedStr = strdup("9F");
	instruction[518].valid64 = INVALID;
	instruction[518].compatLeg = VALID;
	instruction[518].VMX = 0;
	instruction[518].description = strdup("Load: AH ? EFLAGS(SF:ZF:0:AF:0:PF:1:CF).");
	instruction[518].binPrefixLen = 0;
	instruction[518].binOpcodeLen = 1;
	instruction[518].binOpcode = bpstrndup("\x9F", 1);
	instruction[518].opcodeOperandCount = 0;

	// Instruction Struct [519] - LAR
	instruction[519].instructionStr = strdup("LAR");
	instruction[519].undecodedStr = strdup("0F 02 /r");
	instruction[519].dstOperandStr = strdup("r16");
	instruction[519].dstOperand = IA_R16;
	instruction[519].srcOperandStr = strdup("r16/m16");
	instruction[519].srcOperand = IA_R16_M16;
	instruction[519].valid64 = INVALID;
	instruction[519].compatLeg = VALID;
	instruction[519].VMX = 0;
	instruction[519].description = strdup("r16 ? r16/m16 masked by FF00H.");
	instruction[519].binPrefixLen = 0;
	instruction[519].binOpcodeLen = 2;
	instruction[519].binOpcode = bpstrndup("\x0F\x02", 2);
	instruction[519].opcodeOperand[0] = OA_MODRM;
	instruction[519].opcodeOperandCount = 1;

	// Instruction Struct [520] - LAR
	instruction[520].instructionStr = strdup("LAR");
	instruction[520].undecodedStr = strdup("0F 02 /r");
	instruction[520].dstOperandStr = strdup("r32");
	instruction[520].dstOperand = IA_R32;
	instruction[520].srcOperandStr = strdup("r32/m16");
	instruction[520].srcOperand = IA_R32_M16;
	instruction[520].valid64 = INVALID;
	instruction[520].compatLeg = VALID;
	instruction[520].VMX = 0;
	instruction[520].description = strdup("r32 ? r32/m16 masked by 00FxFF00H");
	instruction[520].binPrefixLen = 0;
	instruction[520].binOpcodeLen = 2;
	instruction[520].binOpcode = bpstrndup("\x0F\x02", 2);
	instruction[520].opcodeOperand[0] = OA_MODRM;
	instruction[520].opcodeOperandCount = 1;

	// Instruction Struct [521] - LDDQU
	instruction[521].instructionStr = strdup("LDDQU");
	instruction[521].undecodedStr = strdup("F2 0F F0 /r");
	instruction[521].dstOperandStr = strdup("xmm1");
	instruction[521].dstOperand = IA_XMM;
	instruction[521].srcOperandStr = strdup("mem");
	instruction[521].srcOperand = IA_MEM;
	instruction[521].valid64 = INVALID;
	instruction[521].compatLeg = VALID;
	instruction[521].VMX = 0;
	instruction[521].description = strdup("Load unaligned data from mem and return double quadword in xmm1.");
	instruction[521].prefixStr = strdup("REPNE");
	instruction[521].binPrefixLen = 1;
	instruction[521].binPrefix = bpstrndup("\xF2", 1);
	instruction[521].binOpcodeLen = 2;
	instruction[521].binOpcode = bpstrndup("\x0F\xF0", 2);
	instruction[521].opcodeOperand[0] = OA_MODRM;
	instruction[521].opcodeOperandCount = 1;

	// Instruction Struct [522] - LDMXCSR
	instruction[522].instructionStr = strdup("LDMXCSR");
	instruction[522].undecodedStr = strdup("0F AE /2");
	instruction[522].dstOperandStr = strdup("m32");
	instruction[522].dstOperand = IA_M32;
	instruction[522].valid64 = INVALID;
	instruction[522].compatLeg = VALID;
	instruction[522].VMX = 0;
	instruction[522].description = strdup("Load MXCSR register from m32. (Opcode in Intel Manual shows 0F,AE,/2 as the opcode, however the commas appear to be a typo)");
	instruction[522].binPrefixLen = 0;
	instruction[522].binOpcodeLen = 2;
	instruction[522].binOpcode = bpstrndup("\x0F\xAE", 2);
	instruction[522].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[522].opcodeOperandCount = 1;

	// Instruction Struct [523] - LDS
	instruction[523].instructionStr = strdup("LDS");
	instruction[523].undecodedStr = strdup("C5 /r");
	instruction[523].dstOperandStr = strdup("r16");
	instruction[523].dstOperand = IA_R16;
	instruction[523].srcOperandStr = strdup("m16:16");
	instruction[523].srcOperand = IA_M16_16;
	instruction[523].valid64 = INVALID;
	instruction[523].compatLeg = VALID;
	instruction[523].VMX = 0;
	instruction[523].description = strdup("Load DS:r16 with far pointer from memory.");
	instruction[523].binPrefixLen = 0;
	instruction[523].binOpcodeLen = 1;
	instruction[523].binOpcode = bpstrndup("\xC5", 1);
	instruction[523].opcodeOperand[0] = OA_MODRM;
	instruction[523].opcodeOperandCount = 1;

	// Instruction Struct [524] - LDS
	instruction[524].instructionStr = strdup("LDS");
	instruction[524].undecodedStr = strdup("C5 /r");
	instruction[524].dstOperandStr = strdup("r32");
	instruction[524].dstOperand = IA_R32;
	instruction[524].srcOperandStr = strdup("m16:32");
	instruction[524].srcOperand = IA_M16_32;
	instruction[524].valid64 = INVALID;
	instruction[524].compatLeg = VALID;
	instruction[524].VMX = 0;
	instruction[524].description = strdup("Load DS:r32 with far pointer from memory.");
	instruction[524].binPrefixLen = 0;
	instruction[524].binOpcodeLen = 1;
	instruction[524].binOpcode = bpstrndup("\xC5", 1);
	instruction[524].opcodeOperand[0] = OA_MODRM;
	instruction[524].opcodeOperandCount = 1;

	// Instruction Struct [525] - LSS
	instruction[525].instructionStr = strdup("LSS");
	instruction[525].undecodedStr = strdup("0F B2 /r");
	instruction[525].dstOperandStr = strdup("r16");
	instruction[525].dstOperand = IA_R16;
	instruction[525].srcOperandStr = strdup("m16:16");
	instruction[525].srcOperand = IA_M16_16;
	instruction[525].valid64 = INVALID;
	instruction[525].compatLeg = VALID;
	instruction[525].VMX = 0;
	instruction[525].description = strdup("Load SS:r16 with far pointer from memory.");
	instruction[525].binPrefixLen = 0;
	instruction[525].binOpcodeLen = 2;
	instruction[525].binOpcode = bpstrndup("\x0F\xB2", 2);
	instruction[525].opcodeOperand[0] = OA_MODRM;
	instruction[525].opcodeOperandCount = 1;

	// Instruction Struct [526] - LSS
	instruction[526].instructionStr = strdup("LSS");
	instruction[526].undecodedStr = strdup("0F B2 /r");
	instruction[526].dstOperandStr = strdup("r32");
	instruction[526].dstOperand = IA_R32;
	instruction[526].srcOperandStr = strdup("m16:32");
	instruction[526].srcOperand = IA_M16_32;
	instruction[526].valid64 = INVALID;
	instruction[526].compatLeg = VALID;
	instruction[526].VMX = 0;
	instruction[526].description = strdup("Load SS:r32 with far pointer from memory.");
	instruction[526].binPrefixLen = 0;
	instruction[526].binOpcodeLen = 2;
	instruction[526].binOpcode = bpstrndup("\x0F\xB2", 2);
	instruction[526].opcodeOperand[0] = OA_MODRM;
	instruction[526].opcodeOperandCount = 1;

	// Instruction Struct [527] - LES
	instruction[527].instructionStr = strdup("LES");
	instruction[527].undecodedStr = strdup("C4 /r");
	instruction[527].dstOperandStr = strdup("r16");
	instruction[527].dstOperand = IA_R16;
	instruction[527].srcOperandStr = strdup("m16:16");
	instruction[527].srcOperand = IA_M16_16;
	instruction[527].valid64 = INVALID;
	instruction[527].compatLeg = VALID;
	instruction[527].VMX = 0;
	instruction[527].description = strdup("Load ES:r16 with far pointer from memory.");
	instruction[527].binPrefixLen = 0;
	instruction[527].binOpcodeLen = 1;
	instruction[527].binOpcode = bpstrndup("\xC4", 1);
	instruction[527].opcodeOperand[0] = OA_MODRM;
	instruction[527].opcodeOperandCount = 1;

	// Instruction Struct [528] - LES
	instruction[528].instructionStr = strdup("LES");
	instruction[528].undecodedStr = strdup("C4 /r");
	instruction[528].dstOperandStr = strdup("r32");
	instruction[528].dstOperand = IA_R32;
	instruction[528].srcOperandStr = strdup("m16:32");
	instruction[528].srcOperand = IA_M16_32;
	instruction[528].valid64 = INVALID;
	instruction[528].compatLeg = VALID;
	instruction[528].VMX = 0;
	instruction[528].description = strdup("Load ES:r32 with far pointer from memory.");
	instruction[528].binPrefixLen = 0;
	instruction[528].binOpcodeLen = 1;
	instruction[528].binOpcode = bpstrndup("\xC4", 1);
	instruction[528].opcodeOperand[0] = OA_MODRM;
	instruction[528].opcodeOperandCount = 1;

	// Instruction Struct [529] - LFS
	instruction[529].instructionStr = strdup("LFS");
	instruction[529].undecodedStr = strdup("0F B4 /r");
	instruction[529].dstOperandStr = strdup("r16");
	instruction[529].dstOperand = IA_R16;
	instruction[529].srcOperandStr = strdup("m16:16");
	instruction[529].srcOperand = IA_M16_16;
	instruction[529].valid64 = INVALID;
	instruction[529].compatLeg = VALID;
	instruction[529].VMX = 0;
	instruction[529].description = strdup("Load FS:r16 with far pointer from memory.");
	instruction[529].binPrefixLen = 0;
	instruction[529].binOpcodeLen = 2;
	instruction[529].binOpcode = bpstrndup("\x0F\xB4", 2);
	instruction[529].opcodeOperand[0] = OA_MODRM;
	instruction[529].opcodeOperandCount = 1;

	// Instruction Struct [530] - LFS
	instruction[530].instructionStr = strdup("LFS");
	instruction[530].undecodedStr = strdup("0F B4 /r");
	instruction[530].dstOperandStr = strdup("r32");
	instruction[530].dstOperand = IA_R32;
	instruction[530].srcOperandStr = strdup("m16:32");
	instruction[530].srcOperand = IA_M16_32;
	instruction[530].valid64 = INVALID;
	instruction[530].compatLeg = VALID;
	instruction[530].VMX = 0;
	instruction[530].description = strdup("Load FS:r32 with far pointer from memory.");
	instruction[530].binPrefixLen = 0;
	instruction[530].binOpcodeLen = 2;
	instruction[530].binOpcode = bpstrndup("\x0F\xB4", 2);
	instruction[530].opcodeOperand[0] = OA_MODRM;
	instruction[530].opcodeOperandCount = 1;

	// Instruction Struct [531] - LGS
	instruction[531].instructionStr = strdup("LGS");
	instruction[531].undecodedStr = strdup("0F B5 /r");
	instruction[531].dstOperandStr = strdup("r16");
	instruction[531].dstOperand = IA_R16;
	instruction[531].srcOperandStr = strdup("m16:16");
	instruction[531].srcOperand = IA_M16_16;
	instruction[531].valid64 = INVALID;
	instruction[531].compatLeg = VALID;
	instruction[531].VMX = 0;
	instruction[531].description = strdup("Load GS:r16 with far pointer from memory.");
	instruction[531].binPrefixLen = 0;
	instruction[531].binOpcodeLen = 2;
	instruction[531].binOpcode = bpstrndup("\x0F\xB5", 2);
	instruction[531].opcodeOperand[0] = OA_MODRM;
	instruction[531].opcodeOperandCount = 1;

	// Instruction Struct [532] - LGS
	instruction[532].instructionStr = strdup("LGS");
	instruction[532].undecodedStr = strdup("0F B5 /r");
	instruction[532].dstOperandStr = strdup("r32");
	instruction[532].dstOperand = IA_R32;
	instruction[532].srcOperandStr = strdup("m16:32");
	instruction[532].srcOperand = IA_M16_32;
	instruction[532].valid64 = INVALID;
	instruction[532].compatLeg = VALID;
	instruction[532].VMX = 0;
	instruction[532].description = strdup("Load GS:r32 with far pointer from memory.");
	instruction[532].binPrefixLen = 0;
	instruction[532].binOpcodeLen = 2;
	instruction[532].binOpcode = bpstrndup("\x0F\xB5", 2);
	instruction[532].opcodeOperand[0] = OA_MODRM;
	instruction[532].opcodeOperandCount = 1;

	// Instruction Struct [533] - LEA
	instruction[533].instructionStr = strdup("LEA");
	instruction[533].undecodedStr = strdup("8D /r");
	instruction[533].dstOperandStr = strdup("r16");
	instruction[533].dstOperand = IA_R16;
	instruction[533].srcOperandStr = strdup("m");
	instruction[533].srcOperand = IA_M;
	instruction[533].valid64 = INVALID;
	instruction[533].compatLeg = VALID;
	instruction[533].VMX = 0;
	instruction[533].description = strdup("Store effective address for m in register r16.");
	instruction[533].binPrefixLen = 0;
	instruction[533].binOpcodeLen = 1;
	instruction[533].binOpcode = bpstrndup("\x8D", 1);
	instruction[533].opcodeOperand[0] = OA_MODRM;
	instruction[533].opcodeOperandCount = 1;

	// Instruction Struct [534] - LEA
	instruction[534].instructionStr = strdup("LEA");
	instruction[534].undecodedStr = strdup("8D /r");
	instruction[534].dstOperandStr = strdup("r32");
	instruction[534].dstOperand = IA_R32;
	instruction[534].srcOperandStr = strdup("m");
	instruction[534].srcOperand = IA_M;
	instruction[534].valid64 = INVALID;
	instruction[534].compatLeg = VALID;
	instruction[534].VMX = 0;
	instruction[534].description = strdup("Store effective address for m in register r32.");
	instruction[534].binPrefixLen = 0;
	instruction[534].binOpcodeLen = 1;
	instruction[534].binOpcode = bpstrndup("\x8D", 1);
	instruction[534].opcodeOperand[0] = OA_MODRM;
	instruction[534].opcodeOperandCount = 1;

	// Instruction Struct [535] - LEAVE
	instruction[535].instructionStr = strdup("LEAVE");
	instruction[535].undecodedStr = strdup("C9");
	instruction[535].valid64 = INVALID;
	instruction[535].compatLeg = VALID;
	instruction[535].VMX = 0;
	instruction[535].description = strdup("Set SP to BP, then pop BP.");
	instruction[535].binPrefixLen = 0;
	instruction[535].binOpcodeLen = 1;
	instruction[535].binOpcode = bpstrndup("\xC9", 1);
	instruction[535].opcodeOperandCount = 0;

	// Instruction Struct [536] - LEAVE
	instruction[536].instructionStr = strdup("LEAVE");
	instruction[536].undecodedStr = strdup("C9");
	instruction[536].valid64 = INVALID;
	instruction[536].compatLeg = VALID;
	instruction[536].VMX = 0;
	instruction[536].description = strdup("Set ESP to EBP, then pop EBP.");
	instruction[536].binPrefixLen = 0;
	instruction[536].binOpcodeLen = 1;
	instruction[536].binOpcode = bpstrndup("\xC9", 1);
	instruction[536].opcodeOperandCount = 0;

	// Instruction Struct [537] - LEAVE
	instruction[537].instructionStr = strdup("LEAVE");
	instruction[537].undecodedStr = strdup("C9");
	instruction[537].valid64 = INVALID;
	instruction[537].compatLeg = INVALID;
	instruction[537].VMX = 0;
	instruction[537].description = strdup("Set RSP to RBP, then pop RBP.");
	instruction[537].binPrefixLen = 0;
	instruction[537].binOpcodeLen = 1;
	instruction[537].binOpcode = bpstrndup("\xC9", 1);
	instruction[537].opcodeOperandCount = 0;

	// Instruction Struct [538] - LFENCE
	instruction[538].instructionStr = strdup("LFENCE");
	instruction[538].undecodedStr = strdup("0F AE /5");
	instruction[538].valid64 = INVALID;
	instruction[538].compatLeg = VALID;
	instruction[538].VMX = 0;
	instruction[538].description = strdup("Serializes load operations.");
	instruction[538].binPrefixLen = 0;
	instruction[538].binOpcodeLen = 2;
	instruction[538].binOpcode = bpstrndup("\x0F\xAE", 2);
	instruction[538].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[538].opcodeOperandCount = 1;

	// Instruction Struct [539] - LGDT
	instruction[539].instructionStr = strdup("LGDT");
	instruction[539].undecodedStr = strdup("0F 01 /2");
	instruction[539].dstOperandStr = strdup("m16&32");
	instruction[539].dstOperand = IA_M16_AND_32;
	instruction[539].valid64 = INVALID;
	instruction[539].compatLeg = VALID;
	instruction[539].VMX = 0;
	instruction[539].description = strdup("Load m into GDTR.");
	instruction[539].binPrefixLen = 0;
	instruction[539].binOpcodeLen = 2;
	instruction[539].binOpcode = bpstrndup("\x0F\x01", 2);
	instruction[539].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[539].opcodeOperandCount = 1;

	// Instruction Struct [540] - LIDT
	instruction[540].instructionStr = strdup("LIDT");
	instruction[540].undecodedStr = strdup("0F 01 /3");
	instruction[540].dstOperandStr = strdup("m16&32");
	instruction[540].dstOperand = IA_M16_AND_32;
	instruction[540].valid64 = INVALID;
	instruction[540].compatLeg = VALID;
	instruction[540].VMX = 0;
	instruction[540].description = strdup("Load m into IDTR.");
	instruction[540].binPrefixLen = 0;
	instruction[540].binOpcodeLen = 2;
	instruction[540].binOpcode = bpstrndup("\x0F\x01", 2);
	instruction[540].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[540].opcodeOperandCount = 1;

	// Instruction Struct [541] - LIDT
	instruction[541].instructionStr = strdup("LIDT");
	instruction[541].undecodedStr = strdup("0F 01 /2");
	instruction[541].dstOperandStr = strdup("m16&64");
	instruction[541].dstOperand = IA_M16_AND_64;
	instruction[541].valid64 = INVALID;
	instruction[541].compatLeg = INVALID;
	instruction[541].VMX = 0;
	instruction[541].description = strdup("Load m into IDTR.");
	instruction[541].binPrefixLen = 0;
	instruction[541].binOpcodeLen = 2;
	instruction[541].binOpcode = bpstrndup("\x0F\x01", 2);
	instruction[541].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[541].opcodeOperandCount = 1;

	// Instruction Struct [542] - LLDT
	instruction[542].instructionStr = strdup("LLDT");
	instruction[542].undecodedStr = strdup("0F 00 /2");
	instruction[542].dstOperandStr = strdup("r/m16");
	instruction[542].dstOperand = IA_RM16;
	instruction[542].valid64 = INVALID;
	instruction[542].compatLeg = VALID;
	instruction[542].VMX = 0;
	instruction[542].description = strdup("Load segment selector r/m16 into LDTR.");
	instruction[542].binPrefixLen = 0;
	instruction[542].binOpcodeLen = 2;
	instruction[542].binOpcode = bpstrndup("\x0F\x00", 2);
	instruction[542].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[542].opcodeOperandCount = 1;

	// Instruction Struct [543] - LMSW
	instruction[543].instructionStr = strdup("LMSW");
	instruction[543].undecodedStr = strdup("0F 01 /6");
	instruction[543].dstOperandStr = strdup("r/m16");
	instruction[543].dstOperand = IA_RM16;
	instruction[543].valid64 = INVALID;
	instruction[543].compatLeg = VALID;
	instruction[543].VMX = 0;
	instruction[543].description = strdup("Loads r/m16 in machine status word of CR0.");
	instruction[543].binPrefixLen = 0;
	instruction[543].binOpcodeLen = 2;
	instruction[543].binOpcode = bpstrndup("\x0F\x01", 2);
	instruction[543].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[543].opcodeOperandCount = 1;

	// Instruction Struct [544] - LOCK
	instruction[544].instructionStr = strdup("LOCK");
	instruction[544].undecodedStr = strdup("F0");
	instruction[544].valid64 = INVALID;
	instruction[544].compatLeg = VALID;
	instruction[544].VMX = 0;
	instruction[544].description = strdup("Asserts LOCK# signal for duration the accompanying instruction.");
	instruction[544].prefixStr = strdup("LOCK");
	instruction[544].binPrefixLen = 1;
	instruction[544].binPrefix = bpstrndup("\xF0", 1);
	instruction[544].binOpcodeLen = 0;
	instruction[544].binOpcode = bpstrndup("", 0);
	instruction[544].opcodeOperandCount = 0;

	// Instruction Struct [545] - LODS
	instruction[545].instructionStr = strdup("LODS");
	instruction[545].undecodedStr = strdup("AC");
	instruction[545].dstOperandStr = strdup("m8");
	instruction[545].dstOperand = IA_M8;
	instruction[545].valid64 = INVALID;
	instruction[545].compatLeg = VALID;
	instruction[545].VMX = 0;
	instruction[545].description = strdup("For legacy mode, Load byte at address DS:(E)SI into AL. For 64-bit mode load byte at address (R)SI into AL.");
	instruction[545].binPrefixLen = 0;
	instruction[545].binOpcodeLen = 1;
	instruction[545].binOpcode = bpstrndup("\xAC", 1);
	instruction[545].opcodeOperandCount = 0;

	// Instruction Struct [546] - LODS
	instruction[546].instructionStr = strdup("LODS");
	instruction[546].undecodedStr = strdup("AD");
	instruction[546].dstOperandStr = strdup("m16");
	instruction[546].dstOperand = IA_M16;
	instruction[546].valid64 = INVALID;
	instruction[546].compatLeg = VALID;
	instruction[546].VMX = 0;
	instruction[546].description = strdup("For legacy mode, Load word at address DS:(E)SI into AX. For 64-bit mode load word at address (R)SI into AX.");
	instruction[546].binPrefixLen = 0;
	instruction[546].binOpcodeLen = 1;
	instruction[546].binOpcode = bpstrndup("\xAD", 1);
	instruction[546].opcodeOperandCount = 0;

	// Instruction Struct [547] - LODS
	instruction[547].instructionStr = strdup("LODS");
	instruction[547].undecodedStr = strdup("AD");
	instruction[547].dstOperandStr = strdup("m32");
	instruction[547].dstOperand = IA_M32;
	instruction[547].valid64 = INVALID;
	instruction[547].compatLeg = VALID;
	instruction[547].VMX = 0;
	instruction[547].description = strdup("For legacy mode, Load dword at address DS:(E)SI into EAX. For 64-bit mode load dword at address (R)SI into EAX.");
	instruction[547].binPrefixLen = 0;
	instruction[547].binOpcodeLen = 1;
	instruction[547].binOpcode = bpstrndup("\xAD", 1);
	instruction[547].opcodeOperandCount = 0;

	// Instruction Struct [548] - LODSB
	instruction[548].instructionStr = strdup("LODSB");
	instruction[548].undecodedStr = strdup("AC");
	instruction[548].valid64 = INVALID;
	instruction[548].compatLeg = VALID;
	instruction[548].VMX = 0;
	instruction[548].description = strdup("For legacy mode, Load byte at address DS:(E)SI into AL. For 64-bit mode load byte at address (R)SI into AL.");
	instruction[548].binPrefixLen = 0;
	instruction[548].binOpcodeLen = 1;
	instruction[548].binOpcode = bpstrndup("\xAC", 1);
	instruction[548].opcodeOperandCount = 0;

	// Instruction Struct [549] - LODSW
	instruction[549].instructionStr = strdup("LODSW");
	instruction[549].undecodedStr = strdup("AD");
	instruction[549].valid64 = INVALID;
	instruction[549].compatLeg = VALID;
	instruction[549].VMX = 0;
	instruction[549].description = strdup("For legacy mode, Load word at address DS:(E)SI into AX. For 64-bit mode load word at address (R)SI into AX.");
	instruction[549].binPrefixLen = 0;
	instruction[549].binOpcodeLen = 1;
	instruction[549].binOpcode = bpstrndup("\xAD", 1);
	instruction[549].opcodeOperandCount = 0;

	// Instruction Struct [550] - LODSD
	instruction[550].instructionStr = strdup("LODSD");
	instruction[550].undecodedStr = strdup("AD");
	instruction[550].valid64 = INVALID;
	instruction[550].compatLeg = VALID;
	instruction[550].VMX = 0;
	instruction[550].description = strdup("For legacy mode, Load dword at address DS:(E)SI into EAX. For 64-bit mode load dword at address (R)SI into EAX.");
	instruction[550].binPrefixLen = 0;
	instruction[550].binOpcodeLen = 1;
	instruction[550].binOpcode = bpstrndup("\xAD", 1);
	instruction[550].opcodeOperandCount = 0;

	// Instruction Struct [551] - LOOP
	instruction[551].instructionStr = strdup("LOOP");
	instruction[551].undecodedStr = strdup("E2 cb");
	instruction[551].dstOperandStr = strdup("rel8");
	instruction[551].dstOperand = IA_REL8;
	instruction[551].valid64 = INVALID;
	instruction[551].compatLeg = VALID;
	instruction[551].VMX = 0;
	instruction[551].description = strdup("Decrement count; jump short if count ? 0.");
	instruction[551].binPrefixLen = 0;
	instruction[551].binOpcodeLen = 1;
	instruction[551].binOpcode = bpstrndup("\xE2", 1);
	instruction[551].opcodeOperand[0] = OA_CB;
	instruction[551].opcodeOperandCount = 1;

	// Instruction Struct [552] - LOOPE
	instruction[552].instructionStr = strdup("LOOPE");
	instruction[552].undecodedStr = strdup("E1 cb");
	instruction[552].dstOperandStr = strdup("rel8");
	instruction[552].dstOperand = IA_REL8;
	instruction[552].valid64 = INVALID;
	instruction[552].compatLeg = VALID;
	instruction[552].VMX = 0;
	instruction[552].description = strdup("Decrement count; jump short if count ? 0 and ZF = 1.");
	instruction[552].binPrefixLen = 0;
	instruction[552].binOpcodeLen = 1;
	instruction[552].binOpcode = bpstrndup("\xE1", 1);
	instruction[552].opcodeOperand[0] = OA_CB;
	instruction[552].opcodeOperandCount = 1;

	// Instruction Struct [553] - LOOPNE
	instruction[553].instructionStr = strdup("LOOPNE");
	instruction[553].undecodedStr = strdup("E0 cb");
	instruction[553].dstOperandStr = strdup("rel8");
	instruction[553].dstOperand = IA_REL8;
	instruction[553].valid64 = INVALID;
	instruction[553].compatLeg = VALID;
	instruction[553].VMX = 0;
	instruction[553].description = strdup("Decrement count; jump short if count ? 0 and ZF = 0.");
	instruction[553].binPrefixLen = 0;
	instruction[553].binOpcodeLen = 1;
	instruction[553].binOpcode = bpstrndup("\xE0", 1);
	instruction[553].opcodeOperand[0] = OA_CB;
	instruction[553].opcodeOperandCount = 1;

	// Instruction Struct [554] - LSL
	instruction[554].instructionStr = strdup("LSL");
	instruction[554].undecodedStr = strdup("0F 03 /r");
	instruction[554].dstOperandStr = strdup("r16");
	instruction[554].dstOperand = IA_R16;
	instruction[554].srcOperandStr = strdup("r16/m16");
	instruction[554].srcOperand = IA_R16_M16;
	instruction[554].valid64 = INVALID;
	instruction[554].compatLeg = VALID;
	instruction[554].VMX = 0;
	instruction[554].description = strdup("Load: r16 ? segment limit, selector r16/m16.");
	instruction[554].binPrefixLen = 0;
	instruction[554].binOpcodeLen = 2;
	instruction[554].binOpcode = bpstrndup("\x0F\x03", 2);
	instruction[554].opcodeOperand[0] = OA_MODRM;
	instruction[554].opcodeOperandCount = 1;

	// Instruction Struct [555] - LSL
	instruction[555].instructionStr = strdup("LSL");
	instruction[555].undecodedStr = strdup("0F 03 /r");
	instruction[555].dstOperandStr = strdup("r32");
	instruction[555].dstOperand = IA_R32;
	instruction[555].srcOperandStr = strdup("r32/m16");
	instruction[555].srcOperand = IA_R32_M16;
	instruction[555].valid64 = INVALID;
	instruction[555].compatLeg = VALID;
	instruction[555].VMX = 0;
	instruction[555].description = strdup("Load: r32 ? segment limit,");
	instruction[555].binPrefixLen = 0;
	instruction[555].binOpcodeLen = 2;
	instruction[555].binOpcode = bpstrndup("\x0F\x03", 2);
	instruction[555].opcodeOperand[0] = OA_MODRM;
	instruction[555].opcodeOperandCount = 1;

	// Instruction Struct [556] - LTR
	instruction[556].instructionStr = strdup("LTR");
	instruction[556].undecodedStr = strdup("0F 00 /3");
	instruction[556].dstOperandStr = strdup("r/m16");
	instruction[556].dstOperand = IA_RM16;
	instruction[556].valid64 = INVALID;
	instruction[556].compatLeg = VALID;
	instruction[556].VMX = 0;
	instruction[556].description = strdup("Load r/m16 into task register.");
	instruction[556].binPrefixLen = 0;
	instruction[556].binOpcodeLen = 2;
	instruction[556].binOpcode = bpstrndup("\x0F\x00", 2);
	instruction[556].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[556].opcodeOperandCount = 1;

	// Instruction Struct [557] - MASKMOVDQU
	instruction[557].instructionStr = strdup("MASKMOVDQU");
	instruction[557].undecodedStr = strdup("66 0F F7 /r");
	instruction[557].dstOperandStr = strdup("xmm1");
	instruction[557].dstOperand = IA_XMM;
	instruction[557].srcOperandStr = strdup("xmm2");
	instruction[557].srcOperand = IA_XMM;
	instruction[557].valid64 = INVALID;
	instruction[557].compatLeg = VALID;
	instruction[557].VMX = 0;
	instruction[557].description = strdup("Selectively write bytes from xmm1 to memory location using the byte mask in xmm2. The default memory location is specified by DS:EDI.");
	instruction[557].prefixStr = strdup("OPSIZEOVR");
	instruction[557].binPrefixLen = 1;
	instruction[557].binPrefix = bpstrndup("\x66", 1);
	instruction[557].binOpcodeLen = 2;
	instruction[557].binOpcode = bpstrndup("\x0F\xF7", 2);
	instruction[557].opcodeOperand[0] = OA_MODRM;
	instruction[557].opcodeOperandCount = 1;

	// Instruction Struct [558] - MASKMOVQ
	instruction[558].instructionStr = strdup("MASKMOVQ");
	instruction[558].undecodedStr = strdup("0F F7 /r");
	instruction[558].dstOperandStr = strdup("mm1");
	instruction[558].dstOperand = IA_MM;
	instruction[558].srcOperandStr = strdup("mm2");
	instruction[558].srcOperand = IA_MM;
	instruction[558].valid64 = INVALID;
	instruction[558].compatLeg = VALID;
	instruction[558].VMX = 0;
	instruction[558].description = strdup("Selectively write bytes from mm1 to memory location using the byte mask in mm2. The default memory location is specified by DS:EDI.");
	instruction[558].binPrefixLen = 0;
	instruction[558].binOpcodeLen = 2;
	instruction[558].binOpcode = bpstrndup("\x0F\xF7", 2);
	instruction[558].opcodeOperand[0] = OA_MODRM;
	instruction[558].opcodeOperandCount = 1;

	// Instruction Struct [559] - MAXPD
	instruction[559].instructionStr = strdup("MAXPD");
	instruction[559].undecodedStr = strdup("66 0F 5F /r");
	instruction[559].dstOperandStr = strdup("xmm1");
	instruction[559].dstOperand = IA_XMM;
	instruction[559].srcOperandStr = strdup("xmm2/m128");
	instruction[559].srcOperand = IA_XMM_M128;
	instruction[559].valid64 = INVALID;
	instruction[559].compatLeg = VALID;
	instruction[559].VMX = 0;
	instruction[559].description = strdup("Return the maximum doubleprecision floating-point values between xmm2/m128 and xmm1.");
	instruction[559].prefixStr = strdup("OPSIZEOVR");
	instruction[559].binPrefixLen = 1;
	instruction[559].binPrefix = bpstrndup("\x66", 1);
	instruction[559].binOpcodeLen = 2;
	instruction[559].binOpcode = bpstrndup("\x0F\x5F", 2);
	instruction[559].opcodeOperand[0] = OA_MODRM;
	instruction[559].opcodeOperandCount = 1;

	// Instruction Struct [560] - MAXPS
	instruction[560].instructionStr = strdup("MAXPS");
	instruction[560].undecodedStr = strdup("0F 5F /r");
	instruction[560].dstOperandStr = strdup("xmm1");
	instruction[560].dstOperand = IA_XMM;
	instruction[560].srcOperandStr = strdup("xmm2/m128");
	instruction[560].srcOperand = IA_XMM_M128;
	instruction[560].valid64 = INVALID;
	instruction[560].compatLeg = VALID;
	instruction[560].VMX = 0;
	instruction[560].description = strdup("Return the maximum single-precision floating-point values between xmm2/m128 and xmm1.");
	instruction[560].binPrefixLen = 0;
	instruction[560].binOpcodeLen = 2;
	instruction[560].binOpcode = bpstrndup("\x0F\x5F", 2);
	instruction[560].opcodeOperand[0] = OA_MODRM;
	instruction[560].opcodeOperandCount = 1;

	// Instruction Struct [561] - MAXSD
	instruction[561].instructionStr = strdup("MAXSD");
	instruction[561].undecodedStr = strdup("F2 0F 5F /r");
	instruction[561].dstOperandStr = strdup("xmm1");
	instruction[561].dstOperand = IA_XMM;
	instruction[561].srcOperandStr = strdup("xmm2/m64");
	instruction[561].srcOperand = IA_XMM_M64;
	instruction[561].valid64 = INVALID;
	instruction[561].compatLeg = VALID;
	instruction[561].VMX = 0;
	instruction[561].description = strdup("Return the maximum scalar doubleprecision floating-point value between xmm2/mem64 and xmm1.");
	instruction[561].prefixStr = strdup("REPNE");
	instruction[561].binPrefixLen = 1;
	instruction[561].binPrefix = bpstrndup("\xF2", 1);
	instruction[561].binOpcodeLen = 2;
	instruction[561].binOpcode = bpstrndup("\x0F\x5F", 2);
	instruction[561].opcodeOperand[0] = OA_MODRM;
	instruction[561].opcodeOperandCount = 1;

	// Instruction Struct [562] - MAXSS
	instruction[562].instructionStr = strdup("MAXSS");
	instruction[562].undecodedStr = strdup("F3 0F 5F /r");
	instruction[562].dstOperandStr = strdup("xmm1");
	instruction[562].dstOperand = IA_XMM;
	instruction[562].srcOperandStr = strdup("xmm2/m32");
	instruction[562].srcOperand = IA_XMM_M32;
	instruction[562].valid64 = INVALID;
	instruction[562].compatLeg = VALID;
	instruction[562].VMX = 0;
	instruction[562].description = strdup("Return the maximum scalar singleprecision floating-point value between xmm2/mem32 and xmm1.");
	instruction[562].prefixStr = strdup("REP");
	instruction[562].binPrefixLen = 1;
	instruction[562].binPrefix = bpstrndup("\xF3", 1);
	instruction[562].binOpcodeLen = 2;
	instruction[562].binOpcode = bpstrndup("\x0F\x5F", 2);
	instruction[562].opcodeOperand[0] = OA_MODRM;
	instruction[562].opcodeOperandCount = 1;

	// Instruction Struct [563] - MFENCE
	instruction[563].instructionStr = strdup("MFENCE");
	instruction[563].undecodedStr = strdup("0F AE /6");
	instruction[563].valid64 = INVALID;
	instruction[563].compatLeg = VALID;
	instruction[563].VMX = 0;
	instruction[563].description = strdup("Serializes load and store operations.");
	instruction[563].binPrefixLen = 0;
	instruction[563].binOpcodeLen = 2;
	instruction[563].binOpcode = bpstrndup("\x0F\xAE", 2);
	instruction[563].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[563].opcodeOperandCount = 1;

	// Instruction Struct [564] - MINPD
	instruction[564].instructionStr = strdup("MINPD");
	instruction[564].undecodedStr = strdup("66 0F 5D /r");
	instruction[564].dstOperandStr = strdup("xmm1");
	instruction[564].dstOperand = IA_XMM;
	instruction[564].srcOperandStr = strdup("xmm2/m128");
	instruction[564].srcOperand = IA_XMM_M128;
	instruction[564].valid64 = INVALID;
	instruction[564].compatLeg = VALID;
	instruction[564].VMX = 0;
	instruction[564].description = strdup("Return the minimum doubleprecision floating-point values between xmm2/m128 and xmm1.");
	instruction[564].prefixStr = strdup("OPSIZEOVR");
	instruction[564].binPrefixLen = 1;
	instruction[564].binPrefix = bpstrndup("\x66", 1);
	instruction[564].binOpcodeLen = 2;
	instruction[564].binOpcode = bpstrndup("\x0F\x5D", 2);
	instruction[564].opcodeOperand[0] = OA_MODRM;
	instruction[564].opcodeOperandCount = 1;

	// Instruction Struct [565] - MINPS
	instruction[565].instructionStr = strdup("MINPS");
	instruction[565].undecodedStr = strdup("0F 5D /r");
	instruction[565].dstOperandStr = strdup("xmm1");
	instruction[565].dstOperand = IA_XMM;
	instruction[565].srcOperandStr = strdup("xmm2/m128");
	instruction[565].srcOperand = IA_XMM_M128;
	instruction[565].valid64 = INVALID;
	instruction[565].compatLeg = VALID;
	instruction[565].VMX = 0;
	instruction[565].description = strdup("Return the minimum single-precision floating-point values between xmm2/m128 and xmm1.");
	instruction[565].binPrefixLen = 0;
	instruction[565].binOpcodeLen = 2;
	instruction[565].binOpcode = bpstrndup("\x0F\x5D", 2);
	instruction[565].opcodeOperand[0] = OA_MODRM;
	instruction[565].opcodeOperandCount = 1;

	// Instruction Struct [566] - MINSD
	instruction[566].instructionStr = strdup("MINSD");
	instruction[566].undecodedStr = strdup("F2 0F 5D /r");
	instruction[566].dstOperandStr = strdup("xmm1");
	instruction[566].dstOperand = IA_XMM;
	instruction[566].srcOperandStr = strdup("xmm2/m64");
	instruction[566].srcOperand = IA_XMM_M64;
	instruction[566].valid64 = INVALID;
	instruction[566].compatLeg = VALID;
	instruction[566].VMX = 0;
	instruction[566].description = strdup("Return the minimum scalar doubleprecision floating-point value between xmm2/mem64 and xmm1.");
	instruction[566].prefixStr = strdup("REPNE");
	instruction[566].binPrefixLen = 1;
	instruction[566].binPrefix = bpstrndup("\xF2", 1);
	instruction[566].binOpcodeLen = 2;
	instruction[566].binOpcode = bpstrndup("\x0F\x5D", 2);
	instruction[566].opcodeOperand[0] = OA_MODRM;
	instruction[566].opcodeOperandCount = 1;

	// Instruction Struct [567] - MINSS
	instruction[567].instructionStr = strdup("MINSS");
	instruction[567].undecodedStr = strdup("F3 0F 5D /r");
	instruction[567].dstOperandStr = strdup("xmm1");
	instruction[567].dstOperand = IA_XMM;
	instruction[567].srcOperandStr = strdup("xmm2/m32");
	instruction[567].srcOperand = IA_XMM_M32;
	instruction[567].valid64 = INVALID;
	instruction[567].compatLeg = VALID;
	instruction[567].VMX = 0;
	instruction[567].description = strdup("Return the minimum scalar singleprecision floating-point value between xmm2/mem32 and xmm1.");
	instruction[567].prefixStr = strdup("REP");
	instruction[567].binPrefixLen = 1;
	instruction[567].binPrefix = bpstrndup("\xF3", 1);
	instruction[567].binOpcodeLen = 2;
	instruction[567].binOpcode = bpstrndup("\x0F\x5D", 2);
	instruction[567].opcodeOperand[0] = OA_MODRM;
	instruction[567].opcodeOperandCount = 1;

	// Instruction Struct [568] - MONITOR
	instruction[568].instructionStr = strdup("MONITOR");
	instruction[568].undecodedStr = strdup("0F 01 C8");
	instruction[568].valid64 = INVALID;
	instruction[568].compatLeg = VALID;
	instruction[568].VMX = 0;
	instruction[568].description = strdup("Sets up a linear address range to be");
	instruction[568].binPrefixLen = 0;
	instruction[568].binOpcodeLen = 3;
	instruction[568].binOpcode = bpstrndup("\x0F\x01\xC8", 3);
	instruction[568].opcodeOperandCount = 0;

	// Instruction Struct [569] - CMP
	instruction[569].instructionStr = strdup("CMP");
	instruction[569].undecodedStr = strdup("81 /7 id");
	instruction[569].dstOperandStr = strdup("r/m32");
	instruction[569].dstOperand = IA_RM32;
	instruction[569].srcOperandStr = strdup("imm32");
	instruction[569].srcOperand = IA_IMM32;
	instruction[569].valid64 = INVALID;
	instruction[569].compatLeg = VALID;
	instruction[569].VMX = 0;
	instruction[569].description = strdup("Compare imm32 with r/m32.");
	instruction[569].binPrefixLen = 0;
	instruction[569].binOpcodeLen = 1;
	instruction[569].binOpcode = bpstrndup("\x81", 1);
	instruction[569].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[569].opcodeOperand[1] = OA_ID;
	instruction[569].opcodeOperandCount = 2;

	// Instruction Struct [570] - MOV
	instruction[570].instructionStr = strdup("MOV");
	instruction[570].undecodedStr = strdup("88 /r");
	instruction[570].dstOperandStr = strdup("r/m8");
	instruction[570].dstOperand = IA_RM8;
	instruction[570].srcOperandStr = strdup("r8");
	instruction[570].srcOperand = IA_R8;
	instruction[570].valid64 = INVALID;
	instruction[570].compatLeg = VALID;
	instruction[570].VMX = 0;
	instruction[570].description = strdup("Move r8 to r/m8.");
	instruction[570].binPrefixLen = 0;
	instruction[570].binOpcodeLen = 1;
	instruction[570].binOpcode = bpstrndup("\x88", 1);
	instruction[570].opcodeOperand[0] = OA_MODRM;
	instruction[570].opcodeOperandCount = 1;

	// Instruction Struct [571] - MOV
	instruction[571].instructionStr = strdup("MOV");
	instruction[571].undecodedStr = strdup("89 /r");
	instruction[571].dstOperandStr = strdup("r/m16");
	instruction[571].dstOperand = IA_RM16;
	instruction[571].srcOperandStr = strdup("r16");
	instruction[571].srcOperand = IA_R16;
	instruction[571].valid64 = INVALID;
	instruction[571].compatLeg = VALID;
	instruction[571].VMX = 0;
	instruction[571].description = strdup("Move r16 to r/m16.");
	instruction[571].binPrefixLen = 0;
	instruction[571].binOpcodeLen = 1;
	instruction[571].binOpcode = bpstrndup("\x89", 1);
	instruction[571].opcodeOperand[0] = OA_MODRM;
	instruction[571].opcodeOperandCount = 1;

	// Instruction Struct [572] - MOV
	instruction[572].instructionStr = strdup("MOV");
	instruction[572].undecodedStr = strdup("89 /r");
	instruction[572].dstOperandStr = strdup("r/m32");
	instruction[572].dstOperand = IA_RM32;
	instruction[572].srcOperandStr = strdup("r32");
	instruction[572].srcOperand = IA_R32;
	instruction[572].valid64 = INVALID;
	instruction[572].compatLeg = VALID;
	instruction[572].VMX = 0;
	instruction[572].description = strdup("Move r32 to r/m32.");
	instruction[572].binPrefixLen = 0;
	instruction[572].binOpcodeLen = 1;
	instruction[572].binOpcode = bpstrndup("\x89", 1);
	instruction[572].opcodeOperand[0] = OA_MODRM;
	instruction[572].opcodeOperandCount = 1;

	// Instruction Struct [573] - MOV
	instruction[573].instructionStr = strdup("MOV");
	instruction[573].undecodedStr = strdup("8A /r");
	instruction[573].dstOperandStr = strdup("r8");
	instruction[573].dstOperand = IA_R8;
	instruction[573].srcOperandStr = strdup("r/m8");
	instruction[573].srcOperand = IA_RM8;
	instruction[573].valid64 = INVALID;
	instruction[573].compatLeg = VALID;
	instruction[573].VMX = 0;
	instruction[573].description = strdup("Move r/m8 to r8.");
	instruction[573].binPrefixLen = 0;
	instruction[573].binOpcodeLen = 1;
	instruction[573].binOpcode = bpstrndup("\x8A", 1);
	instruction[573].opcodeOperand[0] = OA_MODRM;
	instruction[573].opcodeOperandCount = 1;

	// Instruction Struct [574] - MOV
	instruction[574].instructionStr = strdup("MOV");
	instruction[574].undecodedStr = strdup("8B /r");
	instruction[574].dstOperandStr = strdup("r16");
	instruction[574].dstOperand = IA_R16;
	instruction[574].srcOperandStr = strdup("r/m16");
	instruction[574].srcOperand = IA_RM16;
	instruction[574].valid64 = INVALID;
	instruction[574].compatLeg = VALID;
	instruction[574].VMX = 0;
	instruction[574].description = strdup("Move r/m16 to r16.");
	instruction[574].binPrefixLen = 0;
	instruction[574].binOpcodeLen = 1;
	instruction[574].binOpcode = bpstrndup("\x8B", 1);
	instruction[574].opcodeOperand[0] = OA_MODRM;
	instruction[574].opcodeOperandCount = 1;

	// Instruction Struct [575] - MOV
	instruction[575].instructionStr = strdup("MOV");
	instruction[575].undecodedStr = strdup("8B /r");
	instruction[575].dstOperandStr = strdup("r32");
	instruction[575].dstOperand = IA_R32;
	instruction[575].srcOperandStr = strdup("r/m32");
	instruction[575].srcOperand = IA_RM32;
	instruction[575].valid64 = INVALID;
	instruction[575].compatLeg = VALID;
	instruction[575].VMX = 0;
	instruction[575].description = strdup("Move r/m32 to r32.");
	instruction[575].binPrefixLen = 0;
	instruction[575].binOpcodeLen = 1;
	instruction[575].binOpcode = bpstrndup("\x8B", 1);
	instruction[575].opcodeOperand[0] = OA_MODRM;
	instruction[575].opcodeOperandCount = 1;

	// Instruction Struct [576] - MOV
	instruction[576].instructionStr = strdup("MOV");
	instruction[576].undecodedStr = strdup("8C /r");
	instruction[576].dstOperandStr = strdup("r/m16");
	instruction[576].dstOperand = IA_RM16;
	instruction[576].srcOperandStr = strdup("Sreg");
	instruction[576].srcOperand = IA_SREG;
	instruction[576].valid64 = INVALID;
	instruction[576].compatLeg = VALID;
	instruction[576].VMX = 0;
	instruction[576].description = strdup("Move segment register to r/m16.");
	instruction[576].binPrefixLen = 0;
	instruction[576].binOpcodeLen = 1;
	instruction[576].binOpcode = bpstrndup("\x8C", 1);
	instruction[576].opcodeOperand[0] = OA_MODRM;
	instruction[576].opcodeOperandCount = 1;

	// Instruction Struct [577] - MOV
	instruction[577].instructionStr = strdup("MOV");
	instruction[577].undecodedStr = strdup("8E /r");
	instruction[577].dstOperandStr = strdup("Sreg");
	instruction[577].dstOperand = IA_SREG;
	instruction[577].srcOperandStr = strdup("r/m16");
	instruction[577].srcOperand = IA_RM16;
	instruction[577].valid64 = INVALID;
	instruction[577].compatLeg = VALID;
	instruction[577].VMX = 0;
	instruction[577].description = strdup("Move r/m16 to segment register.");
	instruction[577].binPrefixLen = 0;
	instruction[577].binOpcodeLen = 1;
	instruction[577].binOpcode = bpstrndup("\x8E", 1);
	instruction[577].opcodeOperand[0] = OA_MODRM;
	instruction[577].opcodeOperandCount = 1;

	// Instruction Struct [578] - MOV
	instruction[578].instructionStr = strdup("MOV");
	instruction[578].undecodedStr = strdup("A0");
	instruction[578].dstOperandStr = strdup("AL");
	instruction[578].dstOperand = IA_AL;
	instruction[578].srcOperandStr = strdup("moffs8");
	instruction[578].srcOperand = IA_MOFFS8;
	instruction[578].valid64 = INVALID;
	instruction[578].compatLeg = VALID;
	instruction[578].VMX = 0;
	instruction[578].description = strdup("Move byte at (seg:offset) to AL.");
	instruction[578].binPrefixLen = 0;
	instruction[578].binOpcodeLen = 1;
	instruction[578].binOpcode = bpstrndup("\xA0", 1);
	instruction[578].opcodeOperandCount = 0;

	// Instruction Struct [579] - MOV
	instruction[579].instructionStr = strdup("MOV");
	instruction[579].undecodedStr = strdup("A1");
	instruction[579].dstOperandStr = strdup("AX");
	instruction[579].dstOperand = IA_AX;
	instruction[579].srcOperandStr = strdup("moffs16");
	instruction[579].srcOperand = IA_MOFFS16;
	instruction[579].valid64 = INVALID;
	instruction[579].compatLeg = VALID;
	instruction[579].VMX = 0;
	instruction[579].description = strdup("Move word at (seg:offset) to AX.");
	instruction[579].binPrefixLen = 0;
	instruction[579].binOpcodeLen = 1;
	instruction[579].binOpcode = bpstrndup("\xA1", 1);
	instruction[579].opcodeOperandCount = 0;

	// Instruction Struct [580] - MOV
	instruction[580].instructionStr = strdup("MOV");
	instruction[580].undecodedStr = strdup("A1");
	instruction[580].dstOperandStr = strdup("EAX");
	instruction[580].dstOperand = IA_EAX;
	instruction[580].srcOperandStr = strdup("moffs32");
	instruction[580].srcOperand = IA_MOFFS32;
	instruction[580].valid64 = INVALID;
	instruction[580].compatLeg = VALID;
	instruction[580].VMX = 0;
	instruction[580].description = strdup("Move doubleword at (seg:offset) to EAX.");
	instruction[580].binPrefixLen = 0;
	instruction[580].binOpcodeLen = 1;
	instruction[580].binOpcode = bpstrndup("\xA1", 1);
	instruction[580].opcodeOperandCount = 0;

	// Instruction Struct [581] - MOV
	instruction[581].instructionStr = strdup("MOV");
	instruction[581].undecodedStr = strdup("A2");
	instruction[581].dstOperandStr = strdup("moffs8");
	instruction[581].dstOperand = IA_MOFFS8;
	instruction[581].srcOperandStr = strdup("AL");
	instruction[581].srcOperand = IA_AL;
	instruction[581].valid64 = INVALID;
	instruction[581].compatLeg = VALID;
	instruction[581].VMX = 0;
	instruction[581].description = strdup("Move AL to (seg:offset).");
	instruction[581].binPrefixLen = 0;
	instruction[581].binOpcodeLen = 1;
	instruction[581].binOpcode = bpstrndup("\xA2", 1);
	instruction[581].opcodeOperandCount = 0;

	// Instruction Struct [582] - MOV
	instruction[582].instructionStr = strdup("MOV");
	instruction[582].undecodedStr = strdup("A3");
	instruction[582].dstOperandStr = strdup("moffs16");
	instruction[582].dstOperand = IA_MOFFS16;
	instruction[582].srcOperandStr = strdup("AX");
	instruction[582].srcOperand = IA_AX;
	instruction[582].valid64 = INVALID;
	instruction[582].compatLeg = VALID;
	instruction[582].VMX = 0;
	instruction[582].description = strdup("Move AX to (seg:offset).");
	instruction[582].binPrefixLen = 0;
	instruction[582].binOpcodeLen = 1;
	instruction[582].binOpcode = bpstrndup("\xA3", 1);
	instruction[582].opcodeOperandCount = 0;

	// Instruction Struct [583] - MOV
	instruction[583].instructionStr = strdup("MOV");
	instruction[583].undecodedStr = strdup("A3");
	instruction[583].dstOperandStr = strdup("moffs32");
	instruction[583].dstOperand = IA_MOFFS32;
	instruction[583].srcOperandStr = strdup("EAX");
	instruction[583].srcOperand = IA_EAX;
	instruction[583].valid64 = INVALID;
	instruction[583].compatLeg = VALID;
	instruction[583].VMX = 0;
	instruction[583].description = strdup("Move EAX to (seg:offset).");
	instruction[583].binPrefixLen = 0;
	instruction[583].binOpcodeLen = 1;
	instruction[583].binOpcode = bpstrndup("\xA3", 1);
	instruction[583].opcodeOperandCount = 0;

	// Instruction Struct [584] - MOV
	instruction[584].instructionStr = strdup("MOV");
	instruction[584].undecodedStr = strdup("B0 +rb");
	instruction[584].dstOperandStr = strdup("r8");
	instruction[584].dstOperand = IA_R8;
	instruction[584].srcOperandStr = strdup("imm8");
	instruction[584].srcOperand = IA_IMM8;
	instruction[584].valid64 = INVALID;
	instruction[584].compatLeg = VALID;
	instruction[584].VMX = 0;
	instruction[584].description = strdup("Move imm8 to r8.");
	instruction[584].binPrefixLen = 0;
	instruction[584].binOpcodeLen = 1;
	instruction[584].binOpcode = bpstrndup("\xB0", 1);
	instruction[584].opcodeOperand[0] = OA_RB;
	instruction[584].opcodeOperandCount = 1;

	// Instruction Struct [585] - MOV
	instruction[585].instructionStr = strdup("MOV");
	instruction[585].undecodedStr = strdup("B8 +rw");
	instruction[585].dstOperandStr = strdup("r16");
	instruction[585].dstOperand = IA_R16;
	instruction[585].srcOperandStr = strdup("imm16");
	instruction[585].srcOperand = IA_IMM16;
	instruction[585].valid64 = INVALID;
	instruction[585].compatLeg = VALID;
	instruction[585].VMX = 0;
	instruction[585].description = strdup("Move imm16 to r16.");
	instruction[585].binPrefixLen = 0;
	instruction[585].binOpcodeLen = 1;
	instruction[585].binOpcode = bpstrndup("\xB8", 1);
	instruction[585].opcodeOperand[0] = OA_RW;
	instruction[585].opcodeOperandCount = 1;

	// Instruction Struct [586] - MOV
	instruction[586].instructionStr = strdup("MOV");
	instruction[586].undecodedStr = strdup("B8 +rd");
	instruction[586].dstOperandStr = strdup("r32");
	instruction[586].dstOperand = IA_R32;
	instruction[586].srcOperandStr = strdup("imm32");
	instruction[586].srcOperand = IA_IMM32;
	instruction[586].valid64 = INVALID;
	instruction[586].compatLeg = VALID;
	instruction[586].VMX = 0;
	instruction[586].description = strdup("Move imm32 to r32.");
	instruction[586].binPrefixLen = 0;
	instruction[586].binOpcodeLen = 1;
	instruction[586].binOpcode = bpstrndup("\xB8", 1);
	instruction[586].opcodeOperand[0] = OA_RD;
	instruction[586].opcodeOperandCount = 1;

	// Instruction Struct [587] - MOV
	instruction[587].instructionStr = strdup("MOV");
	instruction[587].undecodedStr = strdup("C6 /0");
	instruction[587].dstOperandStr = strdup("r/m8");
	instruction[587].dstOperand = IA_RM8;
	instruction[587].srcOperandStr = strdup("imm8");
	instruction[587].srcOperand = IA_IMM8;
	instruction[587].valid64 = INVALID;
	instruction[587].compatLeg = VALID;
	instruction[587].VMX = 0;
	instruction[587].description = strdup("Move imm8 to r/m8.");
	instruction[587].binPrefixLen = 0;
	instruction[587].binOpcodeLen = 1;
	instruction[587].binOpcode = bpstrndup("\xC6", 1);
	instruction[587].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[587].opcodeOperandCount = 1;

	// Instruction Struct [588] - MOV
	instruction[588].instructionStr = strdup("MOV");
	instruction[588].undecodedStr = strdup("C7 /0");
	instruction[588].dstOperandStr = strdup("r/m16");
	instruction[588].dstOperand = IA_RM16;
	instruction[588].srcOperandStr = strdup("imm16");
	instruction[588].srcOperand = IA_IMM16;
	instruction[588].valid64 = INVALID;
	instruction[588].compatLeg = VALID;
	instruction[588].VMX = 0;
	instruction[588].description = strdup("Move imm16 to r/m16.");
	instruction[588].binPrefixLen = 0;
	instruction[588].binOpcodeLen = 1;
	instruction[588].binOpcode = bpstrndup("\xC7", 1);
	instruction[588].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[588].opcodeOperandCount = 1;

	// Instruction Struct [589] - MOV
	instruction[589].instructionStr = strdup("MOV");
	instruction[589].undecodedStr = strdup("C7 /0");
	instruction[589].dstOperandStr = strdup("r/m32");
	instruction[589].dstOperand = IA_RM32;
	instruction[589].srcOperandStr = strdup("imm32");
	instruction[589].srcOperand = IA_IMM32;
	instruction[589].valid64 = INVALID;
	instruction[589].compatLeg = VALID;
	instruction[589].VMX = 0;
	instruction[589].description = strdup("Move imm32 to r/m32.");
	instruction[589].binPrefixLen = 0;
	instruction[589].binOpcodeLen = 1;
	instruction[589].binOpcode = bpstrndup("\xC7", 1);
	instruction[589].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[589].opcodeOperandCount = 1;

	// Instruction Struct [590] - MOV
	instruction[590].instructionStr = strdup("MOV");
	instruction[590].undecodedStr = strdup("0F 20 /0");
	instruction[590].dstOperandStr = strdup("r32");
	instruction[590].dstOperand = IA_R32;
	instruction[590].srcOperandStr = strdup("CR0");
	instruction[590].srcOperand = IA_CR_0;
	instruction[590].valid64 = INVALID;
	instruction[590].compatLeg = VALID;
	instruction[590].VMX = 0;
	instruction[590].description = strdup("Move CR0 to r32.");
	instruction[590].binPrefixLen = 0;
	instruction[590].binOpcodeLen = 2;
	instruction[590].binOpcode = bpstrndup("\x0F\x20", 2);
	instruction[590].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[590].opcodeOperandCount = 1;

	// Instruction Struct [591] - MOV
	instruction[591].instructionStr = strdup("MOV");
	instruction[591].undecodedStr = strdup("0F 20 /0");
	instruction[591].dstOperandStr = strdup("r64");
	instruction[591].dstOperand = IA_R64;
	instruction[591].srcOperandStr = strdup("CR0");
	instruction[591].srcOperand = IA_CR_0;
	instruction[591].valid64 = INVALID;
	instruction[591].compatLeg = INVALID;
	instruction[591].VMX = 0;
	instruction[591].description = strdup("Move extended CR0 to r64.");
	instruction[591].binPrefixLen = 0;
	instruction[591].binOpcodeLen = 2;
	instruction[591].binOpcode = bpstrndup("\x0F\x20", 2);
	instruction[591].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[591].opcodeOperandCount = 1;

	// Instruction Struct [592] - MOV
	instruction[592].instructionStr = strdup("MOV");
	instruction[592].undecodedStr = strdup("0F 20 /2");
	instruction[592].dstOperandStr = strdup("r32");
	instruction[592].dstOperand = IA_R32;
	instruction[592].srcOperandStr = strdup("CR2");
	instruction[592].srcOperand = IA_CR_2;
	instruction[592].valid64 = INVALID;
	instruction[592].compatLeg = VALID;
	instruction[592].VMX = 0;
	instruction[592].description = strdup("Move CR2 to r32.");
	instruction[592].binPrefixLen = 0;
	instruction[592].binOpcodeLen = 2;
	instruction[592].binOpcode = bpstrndup("\x0F\x20", 2);
	instruction[592].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[592].opcodeOperandCount = 1;

	// Instruction Struct [593] - MOV
	instruction[593].instructionStr = strdup("MOV");
	instruction[593].undecodedStr = strdup("0F 20 /2");
	instruction[593].dstOperandStr = strdup("r64");
	instruction[593].dstOperand = IA_R64;
	instruction[593].srcOperandStr = strdup("CR2");
	instruction[593].srcOperand = IA_CR_2;
	instruction[593].valid64 = INVALID;
	instruction[593].compatLeg = INVALID;
	instruction[593].VMX = 0;
	instruction[593].description = strdup("Move extended CR2 to r64.");
	instruction[593].binPrefixLen = 0;
	instruction[593].binOpcodeLen = 2;
	instruction[593].binOpcode = bpstrndup("\x0F\x20", 2);
	instruction[593].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[593].opcodeOperandCount = 1;

	// Instruction Struct [594] - MOV
	instruction[594].instructionStr = strdup("MOV");
	instruction[594].undecodedStr = strdup("0F 20 /3");
	instruction[594].dstOperandStr = strdup("r32");
	instruction[594].dstOperand = IA_R32;
	instruction[594].srcOperandStr = strdup("CR3");
	instruction[594].srcOperand = IA_CR_3;
	instruction[594].valid64 = INVALID;
	instruction[594].compatLeg = VALID;
	instruction[594].VMX = 0;
	instruction[594].description = strdup("Move CR3 to r32.");
	instruction[594].binPrefixLen = 0;
	instruction[594].binOpcodeLen = 2;
	instruction[594].binOpcode = bpstrndup("\x0F\x20", 2);
	instruction[594].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[594].opcodeOperandCount = 1;

	// Instruction Struct [595] - MOV
	instruction[595].instructionStr = strdup("MOV");
	instruction[595].undecodedStr = strdup("0F 20 /3");
	instruction[595].dstOperandStr = strdup("r64");
	instruction[595].dstOperand = IA_R64;
	instruction[595].srcOperandStr = strdup("CR3");
	instruction[595].srcOperand = IA_CR_3;
	instruction[595].valid64 = INVALID;
	instruction[595].compatLeg = INVALID;
	instruction[595].VMX = 0;
	instruction[595].description = strdup("Move extended CR3 to r64.");
	instruction[595].binPrefixLen = 0;
	instruction[595].binOpcodeLen = 2;
	instruction[595].binOpcode = bpstrndup("\x0F\x20", 2);
	instruction[595].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[595].opcodeOperandCount = 1;

	// Instruction Struct [596] - MOV
	instruction[596].instructionStr = strdup("MOV");
	instruction[596].undecodedStr = strdup("0F 20 /4");
	instruction[596].dstOperandStr = strdup("r64");
	instruction[596].dstOperand = IA_R64;
	instruction[596].srcOperandStr = strdup("CR4");
	instruction[596].srcOperand = IA_CR_4;
	instruction[596].valid64 = INVALID;
	instruction[596].compatLeg = INVALID;
	instruction[596].VMX = 0;
	instruction[596].description = strdup("Move extended CR4 to r64.");
	instruction[596].binPrefixLen = 0;
	instruction[596].binOpcodeLen = 2;
	instruction[596].binOpcode = bpstrndup("\x0F\x20", 2);
	instruction[596].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[596].opcodeOperandCount = 1;

	// Instruction Struct [597] - MOV
	instruction[597].instructionStr = strdup("MOV");
	instruction[597].undecodedStr = strdup("0F 22 /0");
	instruction[597].dstOperandStr = strdup("CR0");
	instruction[597].dstOperand = IA_CR_0;
	instruction[597].srcOperandStr = strdup("r32");
	instruction[597].srcOperand = IA_R32;
	instruction[597].valid64 = INVALID;
	instruction[597].compatLeg = VALID;
	instruction[597].VMX = 0;
	instruction[597].description = strdup("Move r32 to CR0.");
	instruction[597].binPrefixLen = 0;
	instruction[597].binOpcodeLen = 2;
	instruction[597].binOpcode = bpstrndup("\x0F\x22", 2);
	instruction[597].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[597].opcodeOperandCount = 1;

	// Instruction Struct [598] - MOV
	instruction[598].instructionStr = strdup("MOV");
	instruction[598].undecodedStr = strdup("0F 22 /0");
	instruction[598].dstOperandStr = strdup("CR0");
	instruction[598].dstOperand = IA_CR_0;
	instruction[598].srcOperandStr = strdup("r64");
	instruction[598].srcOperand = IA_R64;
	instruction[598].valid64 = INVALID;
	instruction[598].compatLeg = INVALID;
	instruction[598].VMX = 0;
	instruction[598].description = strdup("Move r64 to extended CR0.");
	instruction[598].binPrefixLen = 0;
	instruction[598].binOpcodeLen = 2;
	instruction[598].binOpcode = bpstrndup("\x0F\x22", 2);
	instruction[598].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[598].opcodeOperandCount = 1;

	// Instruction Struct [599] - MOV
	instruction[599].instructionStr = strdup("MOV");
	instruction[599].undecodedStr = strdup("0F 22 /2");
	instruction[599].dstOperandStr = strdup("CR2");
	instruction[599].dstOperand = IA_CR_2;
	instruction[599].srcOperandStr = strdup("r32");
	instruction[599].srcOperand = IA_R32;
	instruction[599].valid64 = INVALID;
	instruction[599].compatLeg = VALID;
	instruction[599].VMX = 0;
	instruction[599].description = strdup("Move r32 to CR2.");
	instruction[599].binPrefixLen = 0;
	instruction[599].binOpcodeLen = 2;
	instruction[599].binOpcode = bpstrndup("\x0F\x22", 2);
	instruction[599].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[599].opcodeOperandCount = 1;

	// Instruction Struct [600] - MOV
	instruction[600].instructionStr = strdup("MOV");
	instruction[600].undecodedStr = strdup("0F 22 /2");
	instruction[600].dstOperandStr = strdup("CR2");
	instruction[600].dstOperand = IA_CR_2;
	instruction[600].srcOperandStr = strdup("r64");
	instruction[600].srcOperand = IA_R64;
	instruction[600].valid64 = INVALID;
	instruction[600].compatLeg = INVALID;
	instruction[600].VMX = 0;
	instruction[600].description = strdup("Move r64 to extended CR2.");
	instruction[600].binPrefixLen = 0;
	instruction[600].binOpcodeLen = 2;
	instruction[600].binOpcode = bpstrndup("\x0F\x22", 2);
	instruction[600].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[600].opcodeOperandCount = 1;

	// Instruction Struct [601] - MOV
	instruction[601].instructionStr = strdup("MOV");
	instruction[601].undecodedStr = strdup("0F 22 /3");
	instruction[601].dstOperandStr = strdup("CR3");
	instruction[601].dstOperand = IA_CR_3;
	instruction[601].srcOperandStr = strdup("r32");
	instruction[601].srcOperand = IA_R32;
	instruction[601].valid64 = INVALID;
	instruction[601].compatLeg = VALID;
	instruction[601].VMX = 0;
	instruction[601].description = strdup("Move r32 to CR3.");
	instruction[601].binPrefixLen = 0;
	instruction[601].binOpcodeLen = 2;
	instruction[601].binOpcode = bpstrndup("\x0F\x22", 2);
	instruction[601].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[601].opcodeOperandCount = 1;

	// Instruction Struct [602] - MOV
	instruction[602].instructionStr = strdup("MOV");
	instruction[602].undecodedStr = strdup("0F 22 /3");
	instruction[602].dstOperandStr = strdup("CR3");
	instruction[602].dstOperand = IA_CR_3;
	instruction[602].srcOperandStr = strdup("r64");
	instruction[602].srcOperand = IA_R64;
	instruction[602].valid64 = INVALID;
	instruction[602].compatLeg = INVALID;
	instruction[602].VMX = 0;
	instruction[602].description = strdup("Move r64 to extended CR3.");
	instruction[602].binPrefixLen = 0;
	instruction[602].binOpcodeLen = 2;
	instruction[602].binOpcode = bpstrndup("\x0F\x22", 2);
	instruction[602].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[602].opcodeOperandCount = 1;

	// Instruction Struct [603] - MOV
	instruction[603].instructionStr = strdup("MOV");
	instruction[603].undecodedStr = strdup("0F 22 /4");
	instruction[603].dstOperandStr = strdup("CR4");
	instruction[603].dstOperand = IA_CR_4;
	instruction[603].srcOperandStr = strdup("r32");
	instruction[603].srcOperand = IA_R32;
	instruction[603].valid64 = INVALID;
	instruction[603].compatLeg = VALID;
	instruction[603].VMX = 0;
	instruction[603].description = strdup("Move r32 to CR4.");
	instruction[603].binPrefixLen = 0;
	instruction[603].binOpcodeLen = 2;
	instruction[603].binOpcode = bpstrndup("\x0F\x22", 2);
	instruction[603].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[603].opcodeOperandCount = 1;

	// Instruction Struct [604] - MOV
	instruction[604].instructionStr = strdup("MOV");
	instruction[604].undecodedStr = strdup("0F 22 /4");
	instruction[604].dstOperandStr = strdup("CR4");
	instruction[604].dstOperand = IA_CR_4;
	instruction[604].srcOperandStr = strdup("r64");
	instruction[604].srcOperand = IA_R64;
	instruction[604].valid64 = INVALID;
	instruction[604].compatLeg = INVALID;
	instruction[604].VMX = 0;
	instruction[604].description = strdup("Move r64 to extended CR4.");
	instruction[604].binPrefixLen = 0;
	instruction[604].binOpcodeLen = 2;
	instruction[604].binOpcode = bpstrndup("\x0F\x22", 2);
	instruction[604].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[604].opcodeOperandCount = 1;

	// Instruction Struct [605] - MOV
	instruction[605].instructionStr = strdup("MOV");
	instruction[605].undecodedStr = strdup("0F 21 /r");
	instruction[605].dstOperandStr = strdup("r32");
	instruction[605].dstOperand = IA_R32;
	instruction[605].srcOperandStr = strdup("DR0-DR7");
	instruction[605].srcOperand = IA_DR0_DR7;
	instruction[605].valid64 = INVALID;
	instruction[605].compatLeg = VALID;
	instruction[605].VMX = 0;
	instruction[605].description = strdup("Move debug register to r32");
	instruction[605].binPrefixLen = 0;
	instruction[605].binOpcodeLen = 2;
	instruction[605].binOpcode = bpstrndup("\x0F\x21", 2);
	instruction[605].opcodeOperand[0] = OA_MODRM;
	instruction[605].opcodeOperandCount = 1;

	// Instruction Struct [606] - MOV
	instruction[606].instructionStr = strdup("MOV");
	instruction[606].undecodedStr = strdup("0F 21 /r");
	instruction[606].dstOperandStr = strdup("r64");
	instruction[606].dstOperand = IA_R64;
	instruction[606].srcOperandStr = strdup("DR0-DR7");
	instruction[606].srcOperand = IA_DR0_DR7;
	instruction[606].valid64 = INVALID;
	instruction[606].compatLeg = INVALID;
	instruction[606].VMX = 0;
	instruction[606].description = strdup("Move extended debug register");
	instruction[606].binPrefixLen = 0;
	instruction[606].binOpcodeLen = 2;
	instruction[606].binOpcode = bpstrndup("\x0F\x21", 2);
	instruction[606].opcodeOperand[0] = OA_MODRM;
	instruction[606].opcodeOperandCount = 1;

	// Instruction Struct [607] - MOV
	instruction[607].instructionStr = strdup("MOV");
	instruction[607].undecodedStr = strdup("0F 23 /r");
	instruction[607].dstOperandStr = strdup("DR0-DR7");
	instruction[607].dstOperand = IA_DR0_DR7;
	instruction[607].srcOperandStr = strdup("r32");
	instruction[607].srcOperand = IA_R32;
	instruction[607].valid64 = INVALID;
	instruction[607].compatLeg = VALID;
	instruction[607].VMX = 0;
	instruction[607].description = strdup("Move r32 to debug register");
	instruction[607].binPrefixLen = 0;
	instruction[607].binOpcodeLen = 2;
	instruction[607].binOpcode = bpstrndup("\x0F\x23", 2);
	instruction[607].opcodeOperand[0] = OA_MODRM;
	instruction[607].opcodeOperandCount = 1;

	// Instruction Struct [608] - MOV
	instruction[608].instructionStr = strdup("MOV");
	instruction[608].undecodedStr = strdup("0F 23 /r");
	instruction[608].dstOperandStr = strdup("DR0-DR7");
	instruction[608].dstOperand = IA_DR0_DR7;
	instruction[608].srcOperandStr = strdup("r64");
	instruction[608].srcOperand = IA_R64;
	instruction[608].valid64 = INVALID;
	instruction[608].compatLeg = INVALID;
	instruction[608].VMX = 0;
	instruction[608].description = strdup("Move r64 to extended debug register.");
	instruction[608].binPrefixLen = 0;
	instruction[608].binOpcodeLen = 2;
	instruction[608].binOpcode = bpstrndup("\x0F\x23", 2);
	instruction[608].opcodeOperand[0] = OA_MODRM;
	instruction[608].opcodeOperandCount = 1;

	// Instruction Struct [609] - MOVAPD
	instruction[609].instructionStr = strdup("MOVAPD");
	instruction[609].undecodedStr = strdup("66 0F 28 /r");
	instruction[609].dstOperandStr = strdup("xmm1");
	instruction[609].dstOperand = IA_XMM;
	instruction[609].srcOperandStr = strdup("xmm2/m128");
	instruction[609].srcOperand = IA_XMM_M128;
	instruction[609].valid64 = INVALID;
	instruction[609].compatLeg = VALID;
	instruction[609].VMX = 0;
	instruction[609].description = strdup("Move packed double-precision floating-point values from xmm2/m128 to xmm1.");
	instruction[609].prefixStr = strdup("OPSIZEOVR");
	instruction[609].binPrefixLen = 1;
	instruction[609].binPrefix = bpstrndup("\x66", 1);
	instruction[609].binOpcodeLen = 2;
	instruction[609].binOpcode = bpstrndup("\x0F\x28", 2);
	instruction[609].opcodeOperand[0] = OA_MODRM;
	instruction[609].opcodeOperandCount = 1;

	// Instruction Struct [610] - MOVAPD
	instruction[610].instructionStr = strdup("MOVAPD");
	instruction[610].undecodedStr = strdup("66 0F 29 /r");
	instruction[610].dstOperandStr = strdup("xmm2/m128");
	instruction[610].dstOperand = IA_XMM_M128;
	instruction[610].srcOperandStr = strdup("xmm1");
	instruction[610].srcOperand = IA_XMM;
	instruction[610].valid64 = INVALID;
	instruction[610].compatLeg = VALID;
	instruction[610].VMX = 0;
	instruction[610].description = strdup("Move packed double-precision floating-point values from xmm1 to xmm2/m128.");
	instruction[610].prefixStr = strdup("OPSIZEOVR");
	instruction[610].binPrefixLen = 1;
	instruction[610].binPrefix = bpstrndup("\x66", 1);
	instruction[610].binOpcodeLen = 2;
	instruction[610].binOpcode = bpstrndup("\x0F\x29", 2);
	instruction[610].opcodeOperand[0] = OA_MODRM;
	instruction[610].opcodeOperandCount = 1;

	// Instruction Struct [611] - MOVAPS
	instruction[611].instructionStr = strdup("MOVAPS");
	instruction[611].undecodedStr = strdup("0F 28 /r");
	instruction[611].dstOperandStr = strdup("xmm1");
	instruction[611].dstOperand = IA_XMM;
	instruction[611].srcOperandStr = strdup("xmm2/m128");
	instruction[611].srcOperand = IA_XMM_M128;
	instruction[611].valid64 = INVALID;
	instruction[611].compatLeg = VALID;
	instruction[611].VMX = 0;
	instruction[611].description = strdup("Move packed single-precision floating-point values from xmm2/m128 to xmm1.");
	instruction[611].binPrefixLen = 0;
	instruction[611].binOpcodeLen = 2;
	instruction[611].binOpcode = bpstrndup("\x0F\x28", 2);
	instruction[611].opcodeOperand[0] = OA_MODRM;
	instruction[611].opcodeOperandCount = 1;

	// Instruction Struct [612] - MOVAPS
	instruction[612].instructionStr = strdup("MOVAPS");
	instruction[612].undecodedStr = strdup("0F 29 /r");
	instruction[612].dstOperandStr = strdup("xmm2/m128");
	instruction[612].dstOperand = IA_XMM_M128;
	instruction[612].srcOperandStr = strdup("xmm1");
	instruction[612].srcOperand = IA_XMM;
	instruction[612].valid64 = INVALID;
	instruction[612].compatLeg = VALID;
	instruction[612].VMX = 0;
	instruction[612].description = strdup("Move packed single-precision floating-point values from xmm1 to xmm2/m128.");
	instruction[612].binPrefixLen = 0;
	instruction[612].binOpcodeLen = 2;
	instruction[612].binOpcode = bpstrndup("\x0F\x29", 2);
	instruction[612].opcodeOperand[0] = OA_MODRM;
	instruction[612].opcodeOperandCount = 1;

	// Instruction Struct [613] - MOVD
	instruction[613].instructionStr = strdup("MOVD");
	instruction[613].undecodedStr = strdup("0F 6E /r");
	instruction[613].dstOperandStr = strdup("mm");
	instruction[613].dstOperand = IA_MM;
	instruction[613].srcOperandStr = strdup("r/m32");
	instruction[613].srcOperand = IA_RM32;
	instruction[613].valid64 = INVALID;
	instruction[613].compatLeg = VALID;
	instruction[613].VMX = 0;
	instruction[613].description = strdup("Move doubleword from r/m32 to mm.");
	instruction[613].binPrefixLen = 0;
	instruction[613].binOpcodeLen = 2;
	instruction[613].binOpcode = bpstrndup("\x0F\x6E", 2);
	instruction[613].opcodeOperand[0] = OA_MODRM;
	instruction[613].opcodeOperandCount = 1;

	// Instruction Struct [614] - MOVD
	instruction[614].instructionStr = strdup("MOVD");
	instruction[614].undecodedStr = strdup("0F 7E /r");
	instruction[614].dstOperandStr = strdup("r/m32");
	instruction[614].dstOperand = IA_RM32;
	instruction[614].srcOperandStr = strdup("mm");
	instruction[614].srcOperand = IA_MM;
	instruction[614].valid64 = INVALID;
	instruction[614].compatLeg = VALID;
	instruction[614].VMX = 0;
	instruction[614].description = strdup("Move doubleword from mm to r/m32.");
	instruction[614].binPrefixLen = 0;
	instruction[614].binOpcodeLen = 2;
	instruction[614].binOpcode = bpstrndup("\x0F\x7E", 2);
	instruction[614].opcodeOperand[0] = OA_MODRM;
	instruction[614].opcodeOperandCount = 1;

	// Instruction Struct [615] - MOVD
	instruction[615].instructionStr = strdup("MOVD");
	instruction[615].undecodedStr = strdup("66 0F 6E /r");
	instruction[615].dstOperandStr = strdup("xmm");
	instruction[615].dstOperand = IA_XMM;
	instruction[615].srcOperandStr = strdup("r/m32");
	instruction[615].srcOperand = IA_RM32;
	instruction[615].valid64 = INVALID;
	instruction[615].compatLeg = VALID;
	instruction[615].VMX = 0;
	instruction[615].description = strdup("Move doubleword from r/m32 to xmm.");
	instruction[615].prefixStr = strdup("OPSIZEOVR");
	instruction[615].binPrefixLen = 1;
	instruction[615].binPrefix = bpstrndup("\x66", 1);
	instruction[615].binOpcodeLen = 2;
	instruction[615].binOpcode = bpstrndup("\x0F\x6E", 2);
	instruction[615].opcodeOperand[0] = OA_MODRM;
	instruction[615].opcodeOperandCount = 1;

	// Instruction Struct [616] - MOVD
	instruction[616].instructionStr = strdup("MOVD");
	instruction[616].undecodedStr = strdup("66 0F 7E /r");
	instruction[616].dstOperandStr = strdup("r/m32");
	instruction[616].dstOperand = IA_RM32;
	instruction[616].srcOperandStr = strdup("xmm");
	instruction[616].srcOperand = IA_XMM;
	instruction[616].valid64 = INVALID;
	instruction[616].compatLeg = VALID;
	instruction[616].VMX = 0;
	instruction[616].description = strdup("Move doubleword from xmm register to r/m32.");
	instruction[616].prefixStr = strdup("OPSIZEOVR");
	instruction[616].binPrefixLen = 1;
	instruction[616].binPrefix = bpstrndup("\x66", 1);
	instruction[616].binOpcodeLen = 2;
	instruction[616].binOpcode = bpstrndup("\x0F\x7E", 2);
	instruction[616].opcodeOperand[0] = OA_MODRM;
	instruction[616].opcodeOperandCount = 1;

	// Instruction Struct [617] - MOVDDUP
	instruction[617].instructionStr = strdup("MOVDDUP");
	instruction[617].undecodedStr = strdup("F2 0F 12 /r");
	instruction[617].dstOperandStr = strdup("xmm1");
	instruction[617].dstOperand = IA_XMM;
	instruction[617].srcOperandStr = strdup("xmm2/m64");
	instruction[617].srcOperand = IA_XMM_M64;
	instruction[617].valid64 = INVALID;
	instruction[617].compatLeg = VALID;
	instruction[617].VMX = 0;
	instruction[617].description = strdup("Move one double-precision floatingpoint value from the lower 64-bit operand in xmm2/m64 to xmm1 and duplicate.");
	instruction[617].prefixStr = strdup("REPNE");
	instruction[617].binPrefixLen = 1;
	instruction[617].binPrefix = bpstrndup("\xF2", 1);
	instruction[617].binOpcodeLen = 2;
	instruction[617].binOpcode = bpstrndup("\x0F\x12", 2);
	instruction[617].opcodeOperand[0] = OA_MODRM;
	instruction[617].opcodeOperandCount = 1;

	// Instruction Struct [618] - MOVDQA
	instruction[618].instructionStr = strdup("MOVDQA");
	instruction[618].undecodedStr = strdup("66 0F 6F /r");
	instruction[618].dstOperandStr = strdup("xmm1");
	instruction[618].dstOperand = IA_XMM;
	instruction[618].srcOperandStr = strdup("xmm2/m128");
	instruction[618].srcOperand = IA_XMM_M128;
	instruction[618].valid64 = INVALID;
	instruction[618].compatLeg = VALID;
	instruction[618].VMX = 0;
	instruction[618].description = strdup("Move aligned double quadword from xmm2/m128 to xmm1.");
	instruction[618].prefixStr = strdup("OPSIZEOVR");
	instruction[618].binPrefixLen = 1;
	instruction[618].binPrefix = bpstrndup("\x66", 1);
	instruction[618].binOpcodeLen = 2;
	instruction[618].binOpcode = bpstrndup("\x0F\x6F", 2);
	instruction[618].opcodeOperand[0] = OA_MODRM;
	instruction[618].opcodeOperandCount = 1;

	// Instruction Struct [619] - MOVDQA
	instruction[619].instructionStr = strdup("MOVDQA");
	instruction[619].undecodedStr = strdup("66 0F 7F /r");
	instruction[619].dstOperandStr = strdup("xmm2/m128");
	instruction[619].dstOperand = IA_XMM_M128;
	instruction[619].srcOperandStr = strdup("xmm1");
	instruction[619].srcOperand = IA_XMM;
	instruction[619].valid64 = INVALID;
	instruction[619].compatLeg = VALID;
	instruction[619].VMX = 0;
	instruction[619].description = strdup("Move aligned double quadword from xmm1 to xmm2/m128.");
	instruction[619].prefixStr = strdup("OPSIZEOVR");
	instruction[619].binPrefixLen = 1;
	instruction[619].binPrefix = bpstrndup("\x66", 1);
	instruction[619].binOpcodeLen = 2;
	instruction[619].binOpcode = bpstrndup("\x0F\x7F", 2);
	instruction[619].opcodeOperand[0] = OA_MODRM;
	instruction[619].opcodeOperandCount = 1;

	// Instruction Struct [620] - MOVDQU
	instruction[620].instructionStr = strdup("MOVDQU");
	instruction[620].undecodedStr = strdup("F3 0F 6F /r");
	instruction[620].dstOperandStr = strdup("xmm1");
	instruction[620].dstOperand = IA_XMM;
	instruction[620].srcOperandStr = strdup("xmm2/m128");
	instruction[620].srcOperand = IA_XMM_M128;
	instruction[620].valid64 = INVALID;
	instruction[620].compatLeg = VALID;
	instruction[620].VMX = 0;
	instruction[620].description = strdup("Move unaligned double quadword from xmm2/m128 to xmm1.");
	instruction[620].prefixStr = strdup("REP");
	instruction[620].binPrefixLen = 1;
	instruction[620].binPrefix = bpstrndup("\xF3", 1);
	instruction[620].binOpcodeLen = 2;
	instruction[620].binOpcode = bpstrndup("\x0F\x6F", 2);
	instruction[620].opcodeOperand[0] = OA_MODRM;
	instruction[620].opcodeOperandCount = 1;

	// Instruction Struct [621] - MOVDQU
	instruction[621].instructionStr = strdup("MOVDQU");
	instruction[621].undecodedStr = strdup("F3 0F 7F /r");
	instruction[621].dstOperandStr = strdup("xmm2/m128");
	instruction[621].dstOperand = IA_XMM_M128;
	instruction[621].srcOperandStr = strdup("xmm1");
	instruction[621].srcOperand = IA_XMM;
	instruction[621].valid64 = INVALID;
	instruction[621].compatLeg = VALID;
	instruction[621].VMX = 0;
	instruction[621].description = strdup("Move unaligned double quadword from xmm1 to xmm2/m128.");
	instruction[621].prefixStr = strdup("REP");
	instruction[621].binPrefixLen = 1;
	instruction[621].binPrefix = bpstrndup("\xF3", 1);
	instruction[621].binOpcodeLen = 2;
	instruction[621].binOpcode = bpstrndup("\x0F\x7F", 2);
	instruction[621].opcodeOperand[0] = OA_MODRM;
	instruction[621].opcodeOperandCount = 1;

	// Instruction Struct [622] - MOVDQ2Q
	instruction[622].instructionStr = strdup("MOVDQ2Q");
	instruction[622].undecodedStr = strdup("F2 0F D6");
	instruction[622].dstOperandStr = strdup("mm");
	instruction[622].dstOperand = IA_MM;
	instruction[622].srcOperandStr = strdup("xmm");
	instruction[622].srcOperand = IA_XMM;
	instruction[622].valid64 = INVALID;
	instruction[622].compatLeg = VALID;
	instruction[622].VMX = 0;
	instruction[622].description = strdup("Move low quadword from xmm to mmx register.");
	instruction[622].prefixStr = strdup("REPNE");
	instruction[622].binPrefixLen = 1;
	instruction[622].binPrefix = bpstrndup("\xF2", 1);
	instruction[622].binOpcodeLen = 2;
	instruction[622].binOpcode = bpstrndup("\x0F\xD6", 2);
	instruction[622].opcodeOperandCount = 0;

	// Instruction Struct [623] - MOVHLPS
	instruction[623].instructionStr = strdup("MOVHLPS");
	instruction[623].undecodedStr = strdup("0F 12 /r");
	instruction[623].dstOperandStr = strdup("xmm1");
	instruction[623].dstOperand = IA_XMM;
	instruction[623].srcOperandStr = strdup("xmm2");
	instruction[623].srcOperand = IA_XMM;
	instruction[623].valid64 = INVALID;
	instruction[623].compatLeg = VALID;
	instruction[623].VMX = 0;
	instruction[623].description = strdup("Move two packed singleprecision");
	instruction[623].binPrefixLen = 0;
	instruction[623].binOpcodeLen = 2;
	instruction[623].binOpcode = bpstrndup("\x0F\x12", 2);
	instruction[623].opcodeOperand[0] = OA_MODRM;
	instruction[623].opcodeOperandCount = 1;

	// Instruction Struct [624] - MOVHPD
	instruction[624].instructionStr = strdup("MOVHPD");
	instruction[624].undecodedStr = strdup("66 0F 16 /r");
	instruction[624].dstOperandStr = strdup("xmm");
	instruction[624].dstOperand = IA_XMM;
	instruction[624].srcOperandStr = strdup("m64");
	instruction[624].srcOperand = IA_M64;
	instruction[624].valid64 = INVALID;
	instruction[624].compatLeg = VALID;
	instruction[624].VMX = 0;
	instruction[624].description = strdup("Move double-precision floating-point value from m64 to high quadword of xmm.");
	instruction[624].prefixStr = strdup("OPSIZEOVR");
	instruction[624].binPrefixLen = 1;
	instruction[624].binPrefix = bpstrndup("\x66", 1);
	instruction[624].binOpcodeLen = 2;
	instruction[624].binOpcode = bpstrndup("\x0F\x16", 2);
	instruction[624].opcodeOperand[0] = OA_MODRM;
	instruction[624].opcodeOperandCount = 1;

	// Instruction Struct [625] - MOVHPD
	instruction[625].instructionStr = strdup("MOVHPD");
	instruction[625].undecodedStr = strdup("66 0F 17 /r");
	instruction[625].dstOperandStr = strdup("m64");
	instruction[625].dstOperand = IA_M64;
	instruction[625].srcOperandStr = strdup("xmm");
	instruction[625].srcOperand = IA_XMM;
	instruction[625].valid64 = INVALID;
	instruction[625].compatLeg = VALID;
	instruction[625].VMX = 0;
	instruction[625].description = strdup("Move double-precision floating-point value from high quadword of xmm to m64.");
	instruction[625].prefixStr = strdup("OPSIZEOVR");
	instruction[625].binPrefixLen = 1;
	instruction[625].binPrefix = bpstrndup("\x66", 1);
	instruction[625].binOpcodeLen = 2;
	instruction[625].binOpcode = bpstrndup("\x0F\x17", 2);
	instruction[625].opcodeOperand[0] = OA_MODRM;
	instruction[625].opcodeOperandCount = 1;

	// Instruction Struct [626] - MOVHPS
	instruction[626].instructionStr = strdup("MOVHPS");
	instruction[626].undecodedStr = strdup("0F 16 /r");
	instruction[626].dstOperandStr = strdup("xmm");
	instruction[626].dstOperand = IA_XMM;
	instruction[626].srcOperandStr = strdup("m64");
	instruction[626].srcOperand = IA_M64;
	instruction[626].valid64 = INVALID;
	instruction[626].compatLeg = VALID;
	instruction[626].VMX = 0;
	instruction[626].description = strdup("Move two packed single-precision floating-point values from m64 to high quadword of xmm.");
	instruction[626].binPrefixLen = 0;
	instruction[626].binOpcodeLen = 2;
	instruction[626].binOpcode = bpstrndup("\x0F\x16", 2);
	instruction[626].opcodeOperand[0] = OA_MODRM;
	instruction[626].opcodeOperandCount = 1;

	// Instruction Struct [627] - MOVHPS
	instruction[627].instructionStr = strdup("MOVHPS");
	instruction[627].undecodedStr = strdup("0F 17 /r");
	instruction[627].dstOperandStr = strdup("m64");
	instruction[627].dstOperand = IA_M64;
	instruction[627].srcOperandStr = strdup("xmm");
	instruction[627].srcOperand = IA_XMM;
	instruction[627].valid64 = INVALID;
	instruction[627].compatLeg = VALID;
	instruction[627].VMX = 0;
	instruction[627].description = strdup("Move two packed single-precision floating-point values from high quadword of xmm to m64.");
	instruction[627].binPrefixLen = 0;
	instruction[627].binOpcodeLen = 2;
	instruction[627].binOpcode = bpstrndup("\x0F\x17", 2);
	instruction[627].opcodeOperand[0] = OA_MODRM;
	instruction[627].opcodeOperandCount = 1;

	// Instruction Struct [628] - MOVLHPS
	instruction[628].instructionStr = strdup("MOVLHPS");
	instruction[628].undecodedStr = strdup("0F 16 /r");
	instruction[628].dstOperandStr = strdup("xmm1");
	instruction[628].dstOperand = IA_XMM;
	instruction[628].srcOperandStr = strdup("xmm2");
	instruction[628].srcOperand = IA_XMM;
	instruction[628].valid64 = INVALID;
	instruction[628].compatLeg = VALID;
	instruction[628].VMX = 0;
	instruction[628].description = strdup("Move two packed single-precision");
	instruction[628].binPrefixLen = 0;
	instruction[628].binOpcodeLen = 2;
	instruction[628].binOpcode = bpstrndup("\x0F\x16", 2);
	instruction[628].opcodeOperand[0] = OA_MODRM;
	instruction[628].opcodeOperandCount = 1;

	// Instruction Struct [629] - MOVLPD
	instruction[629].instructionStr = strdup("MOVLPD");
	instruction[629].undecodedStr = strdup("66 0F 12 /r");
	instruction[629].dstOperandStr = strdup("xmm");
	instruction[629].dstOperand = IA_XMM;
	instruction[629].srcOperandStr = strdup("m64");
	instruction[629].srcOperand = IA_M64;
	instruction[629].valid64 = INVALID;
	instruction[629].compatLeg = VALID;
	instruction[629].VMX = 0;
	instruction[629].description = strdup("Move double-precision floating-point value from m64 to low quadword of xmm register.");
	instruction[629].prefixStr = strdup("OPSIZEOVR");
	instruction[629].binPrefixLen = 1;
	instruction[629].binPrefix = bpstrndup("\x66", 1);
	instruction[629].binOpcodeLen = 2;
	instruction[629].binOpcode = bpstrndup("\x0F\x12", 2);
	instruction[629].opcodeOperand[0] = OA_MODRM;
	instruction[629].opcodeOperandCount = 1;

	// Instruction Struct [630] - MOVLPD
	instruction[630].instructionStr = strdup("MOVLPD");
	instruction[630].undecodedStr = strdup("66 0F 13 /r");
	instruction[630].dstOperandStr = strdup("m64");
	instruction[630].dstOperand = IA_M64;
	instruction[630].srcOperandStr = strdup("xmm");
	instruction[630].srcOperand = IA_XMM;
	instruction[630].valid64 = INVALID;
	instruction[630].compatLeg = VALID;
	instruction[630].VMX = 0;
	instruction[630].description = strdup("Move double-precision floating-point nvalue from low quadword of xmm register to m64.");
	instruction[630].prefixStr = strdup("OPSIZEOVR");
	instruction[630].binPrefixLen = 1;
	instruction[630].binPrefix = bpstrndup("\x66", 1);
	instruction[630].binOpcodeLen = 2;
	instruction[630].binOpcode = bpstrndup("\x0F\x13", 2);
	instruction[630].opcodeOperand[0] = OA_MODRM;
	instruction[630].opcodeOperandCount = 1;

	// Instruction Struct [631] - MOVLPS
	instruction[631].instructionStr = strdup("MOVLPS");
	instruction[631].undecodedStr = strdup("0F 12 /r");
	instruction[631].dstOperandStr = strdup("xmm");
	instruction[631].dstOperand = IA_XMM;
	instruction[631].srcOperandStr = strdup("m64");
	instruction[631].srcOperand = IA_M64;
	instruction[631].valid64 = INVALID;
	instruction[631].compatLeg = VALID;
	instruction[631].VMX = 0;
	instruction[631].description = strdup("Move two packed single-precision floating-point values from m64 to low quadword of xmm.");
	instruction[631].binPrefixLen = 0;
	instruction[631].binOpcodeLen = 2;
	instruction[631].binOpcode = bpstrndup("\x0F\x12", 2);
	instruction[631].opcodeOperand[0] = OA_MODRM;
	instruction[631].opcodeOperandCount = 1;

	// Instruction Struct [632] - MOVLPS
	instruction[632].instructionStr = strdup("MOVLPS");
	instruction[632].undecodedStr = strdup("0F 13 /r");
	instruction[632].dstOperandStr = strdup("m64");
	instruction[632].dstOperand = IA_M64;
	instruction[632].srcOperandStr = strdup("xmm");
	instruction[632].srcOperand = IA_XMM;
	instruction[632].valid64 = INVALID;
	instruction[632].compatLeg = VALID;
	instruction[632].VMX = 0;
	instruction[632].description = strdup("Move two packed single-precision floating-point values from low quadword of xmm to m64.");
	instruction[632].binPrefixLen = 0;
	instruction[632].binOpcodeLen = 2;
	instruction[632].binOpcode = bpstrndup("\x0F\x13", 2);
	instruction[632].opcodeOperand[0] = OA_MODRM;
	instruction[632].opcodeOperandCount = 1;

	// Instruction Struct [633] - MOVMSKPD
	instruction[633].instructionStr = strdup("MOVMSKPD");
	instruction[633].undecodedStr = strdup("66 0F 50 /r");
	instruction[633].dstOperandStr = strdup("r32");
	instruction[633].dstOperand = IA_R32;
	instruction[633].srcOperandStr = strdup("xmm");
	instruction[633].srcOperand = IA_XMM;
	instruction[633].valid64 = INVALID;
	instruction[633].compatLeg = VALID;
	instruction[633].VMX = 0;
	instruction[633].description = strdup("Extract 2-bit sign mask from xmm and store in r32.");
	instruction[633].prefixStr = strdup("OPSIZEOVR");
	instruction[633].binPrefixLen = 1;
	instruction[633].binPrefix = bpstrndup("\x66", 1);
	instruction[633].binOpcodeLen = 2;
	instruction[633].binOpcode = bpstrndup("\x0F\x50", 2);
	instruction[633].opcodeOperand[0] = OA_MODRM;
	instruction[633].opcodeOperandCount = 1;

	// Instruction Struct [634] - MOVMSKPS
	instruction[634].instructionStr = strdup("MOVMSKPS");
	instruction[634].undecodedStr = strdup("0F 50 /r");
	instruction[634].dstOperandStr = strdup("r32");
	instruction[634].dstOperand = IA_R32;
	instruction[634].srcOperandStr = strdup("xmm");
	instruction[634].srcOperand = IA_XMM;
	instruction[634].valid64 = INVALID;
	instruction[634].compatLeg = VALID;
	instruction[634].VMX = 0;
	instruction[634].description = strdup("Extract 4-bit sign mask from xmm and store in r32.");
	instruction[634].binPrefixLen = 0;
	instruction[634].binOpcodeLen = 2;
	instruction[634].binOpcode = bpstrndup("\x0F\x50", 2);
	instruction[634].opcodeOperand[0] = OA_MODRM;
	instruction[634].opcodeOperandCount = 1;

	// Instruction Struct [635] - MOVNTDQ
	instruction[635].instructionStr = strdup("MOVNTDQ");
	instruction[635].undecodedStr = strdup("66 0F E7 /r");
	instruction[635].dstOperandStr = strdup("m128");
	instruction[635].dstOperand = IA_M128;
	instruction[635].srcOperandStr = strdup("xmm");
	instruction[635].srcOperand = IA_XMM;
	instruction[635].valid64 = INVALID;
	instruction[635].compatLeg = VALID;
	instruction[635].VMX = 0;
	instruction[635].description = strdup("Move double quadword from xmm to m128 using non-temporal hint.");
	instruction[635].prefixStr = strdup("OPSIZEOVR");
	instruction[635].binPrefixLen = 1;
	instruction[635].binPrefix = bpstrndup("\x66", 1);
	instruction[635].binOpcodeLen = 2;
	instruction[635].binOpcode = bpstrndup("\x0F\xE7", 2);
	instruction[635].opcodeOperand[0] = OA_MODRM;
	instruction[635].opcodeOperandCount = 1;

	// Instruction Struct [636] - MOVNTI
	instruction[636].instructionStr = strdup("MOVNTI");
	instruction[636].undecodedStr = strdup("0F C3 /r");
	instruction[636].dstOperandStr = strdup("m32");
	instruction[636].dstOperand = IA_M32;
	instruction[636].srcOperandStr = strdup("r32");
	instruction[636].srcOperand = IA_R32;
	instruction[636].valid64 = INVALID;
	instruction[636].compatLeg = VALID;
	instruction[636].VMX = 0;
	instruction[636].description = strdup("Move doubleword from r32 to m32 using non-temporal hint.");
	instruction[636].binPrefixLen = 0;
	instruction[636].binOpcodeLen = 2;
	instruction[636].binOpcode = bpstrndup("\x0F\xC3", 2);
	instruction[636].opcodeOperand[0] = OA_MODRM;
	instruction[636].opcodeOperandCount = 1;

	// Instruction Struct [637] - MOVNTPD
	instruction[637].instructionStr = strdup("MOVNTPD");
	instruction[637].undecodedStr = strdup("66 0F 2B /r");
	instruction[637].dstOperandStr = strdup("m128");
	instruction[637].dstOperand = IA_M128;
	instruction[637].srcOperandStr = strdup("xmm");
	instruction[637].srcOperand = IA_XMM;
	instruction[637].valid64 = INVALID;
	instruction[637].compatLeg = VALID;
	instruction[637].VMX = 0;
	instruction[637].description = strdup("Move packed double-precision floating-point values from xmm to m128 using non-temporal hint.");
	instruction[637].prefixStr = strdup("OPSIZEOVR");
	instruction[637].binPrefixLen = 1;
	instruction[637].binPrefix = bpstrndup("\x66", 1);
	instruction[637].binOpcodeLen = 2;
	instruction[637].binOpcode = bpstrndup("\x0F\x2B", 2);
	instruction[637].opcodeOperand[0] = OA_MODRM;
	instruction[637].opcodeOperandCount = 1;

	// Instruction Struct [638] - MOVNTPS
	instruction[638].instructionStr = strdup("MOVNTPS");
	instruction[638].undecodedStr = strdup("0F 2B /r");
	instruction[638].dstOperandStr = strdup("m128");
	instruction[638].dstOperand = IA_M128;
	instruction[638].srcOperandStr = strdup("xmm");
	instruction[638].srcOperand = IA_XMM;
	instruction[638].valid64 = INVALID;
	instruction[638].compatLeg = VALID;
	instruction[638].VMX = 0;
	instruction[638].description = strdup("Move packed single-precision floatingpoint values from xmm to m128 using non-temporal hint.");
	instruction[638].binPrefixLen = 0;
	instruction[638].binOpcodeLen = 2;
	instruction[638].binOpcode = bpstrndup("\x0F\x2B", 2);
	instruction[638].opcodeOperand[0] = OA_MODRM;
	instruction[638].opcodeOperandCount = 1;

	// Instruction Struct [639] - MOVNTQ
	instruction[639].instructionStr = strdup("MOVNTQ");
	instruction[639].undecodedStr = strdup("0F E7 /r");
	instruction[639].dstOperandStr = strdup("m64");
	instruction[639].dstOperand = IA_M64;
	instruction[639].srcOperandStr = strdup("mm");
	instruction[639].srcOperand = IA_MM;
	instruction[639].valid64 = INVALID;
	instruction[639].compatLeg = VALID;
	instruction[639].VMX = 0;
	instruction[639].description = strdup("Move quadword from mm to m64 using non-temporal hint.");
	instruction[639].binPrefixLen = 0;
	instruction[639].binOpcodeLen = 2;
	instruction[639].binOpcode = bpstrndup("\x0F\xE7", 2);
	instruction[639].opcodeOperand[0] = OA_MODRM;
	instruction[639].opcodeOperandCount = 1;

	// Instruction Struct [640] - MOVQ
	instruction[640].instructionStr = strdup("MOVQ");
	instruction[640].undecodedStr = strdup("0F 6F /r");
	instruction[640].dstOperandStr = strdup("mm");
	instruction[640].dstOperand = IA_MM;
	instruction[640].srcOperandStr = strdup("mm/m64");
	instruction[640].srcOperand = IA_MM_M64;
	instruction[640].valid64 = INVALID;
	instruction[640].compatLeg = VALID;
	instruction[640].VMX = 0;
	instruction[640].description = strdup("Move quadword from mm to mm/m64.");
	instruction[640].binPrefixLen = 0;
	instruction[640].binOpcodeLen = 2;
	instruction[640].binOpcode = bpstrndup("\x0F\x6F", 2);
	instruction[640].opcodeOperand[0] = OA_MODRM;
	instruction[640].opcodeOperandCount = 1;

	// Instruction Struct [641] - MOVQ
	instruction[641].instructionStr = strdup("MOVQ");
	instruction[641].undecodedStr = strdup("F3 0F 7E");
	instruction[641].dstOperandStr = strdup("xmm1");
	instruction[641].dstOperand = IA_XMM;
	instruction[641].srcOperandStr = strdup("xmm2/m64");
	instruction[641].srcOperand = IA_XMM_M64;
	instruction[641].valid64 = INVALID;
	instruction[641].compatLeg = VALID;
	instruction[641].VMX = 0;
	instruction[641].description = strdup("Move quadword from xmm2/mem64 to xmm1.");
	instruction[641].prefixStr = strdup("REP");
	instruction[641].binPrefixLen = 1;
	instruction[641].binPrefix = bpstrndup("\xF3", 1);
	instruction[641].binOpcodeLen = 2;
	instruction[641].binOpcode = bpstrndup("\x0F\x7E", 2);
	instruction[641].opcodeOperandCount = 0;

	// Instruction Struct [642] - MOVQ
	instruction[642].instructionStr = strdup("MOVQ");
	instruction[642].undecodedStr = strdup("66 0F D6");
	instruction[642].dstOperandStr = strdup("xmm2/m64");
	instruction[642].dstOperand = IA_XMM_M64;
	instruction[642].srcOperandStr = strdup("xmm1");
	instruction[642].srcOperand = IA_XMM;
	instruction[642].valid64 = INVALID;
	instruction[642].compatLeg = VALID;
	instruction[642].VMX = 0;
	instruction[642].description = strdup("Move quadword from xmm1 to xmm2/mem64.");
	instruction[642].prefixStr = strdup("OPSIZEOVR");
	instruction[642].binPrefixLen = 1;
	instruction[642].binPrefix = bpstrndup("\x66", 1);
	instruction[642].binOpcodeLen = 2;
	instruction[642].binOpcode = bpstrndup("\x0F\xD6", 2);
	instruction[642].opcodeOperandCount = 0;

	// Instruction Struct [643] - MOVQ2DQ
	instruction[643].instructionStr = strdup("MOVQ2DQ");
	instruction[643].undecodedStr = strdup("F3 0F D6");
	instruction[643].dstOperandStr = strdup("xmm");
	instruction[643].dstOperand = IA_XMM;
	instruction[643].srcOperandStr = strdup("mm");
	instruction[643].srcOperand = IA_MM;
	instruction[643].valid64 = INVALID;
	instruction[643].compatLeg = VALID;
	instruction[643].VMX = 0;
	instruction[643].description = strdup("Move quadword from mmx to low quadword of xmm.");
	instruction[643].prefixStr = strdup("REP");
	instruction[643].binPrefixLen = 1;
	instruction[643].binPrefix = bpstrndup("\xF3", 1);
	instruction[643].binOpcodeLen = 2;
	instruction[643].binOpcode = bpstrndup("\x0F\xD6", 2);
	instruction[643].opcodeOperandCount = 0;

	// Instruction Struct [644] - MOVS
	instruction[644].instructionStr = strdup("MOVS");
	instruction[644].undecodedStr = strdup("A4");
	instruction[644].dstOperandStr = strdup("m8");
	instruction[644].dstOperand = IA_M8;
	instruction[644].srcOperandStr = strdup("m8");
	instruction[644].srcOperand = IA_M8;
	instruction[644].valid64 = INVALID;
	instruction[644].compatLeg = VALID;
	instruction[644].VMX = 0;
	instruction[644].description = strdup("For legacy mode, Move byte from address DS:(E)SI to ES:(E)DI. For 64-bit mode move byte from address (R|E)SI to (R|E)DI.");
	instruction[644].binPrefixLen = 0;
	instruction[644].binOpcodeLen = 1;
	instruction[644].binOpcode = bpstrndup("\xA4", 1);
	instruction[644].opcodeOperandCount = 0;

	// Instruction Struct [645] - MOVS
	instruction[645].instructionStr = strdup("MOVS");
	instruction[645].undecodedStr = strdup("A5");
	instruction[645].dstOperandStr = strdup("m16");
	instruction[645].dstOperand = IA_M16;
	instruction[645].srcOperandStr = strdup("m16");
	instruction[645].srcOperand = IA_M16;
	instruction[645].valid64 = INVALID;
	instruction[645].compatLeg = VALID;
	instruction[645].VMX = 0;
	instruction[645].description = strdup("For legacy mode, move word from address DS:(E)SI to ES:(E)DI. For 64-bit mode move word at address (R|E)SI to (R|E)DI.");
	instruction[645].binPrefixLen = 0;
	instruction[645].binOpcodeLen = 1;
	instruction[645].binOpcode = bpstrndup("\xA5", 1);
	instruction[645].opcodeOperandCount = 0;

	// Instruction Struct [646] - MOVS
	instruction[646].instructionStr = strdup("MOVS");
	instruction[646].undecodedStr = strdup("A5");
	instruction[646].dstOperandStr = strdup("m32");
	instruction[646].dstOperand = IA_M32;
	instruction[646].srcOperandStr = strdup("m32");
	instruction[646].srcOperand = IA_M32;
	instruction[646].valid64 = INVALID;
	instruction[646].compatLeg = VALID;
	instruction[646].VMX = 0;
	instruction[646].description = strdup("For legacy mode, move dword from address DS:(E)SI to ES:(E)DI. For 64-bit mode move dword from address (R|E)SI to (R|E)DI.");
	instruction[646].binPrefixLen = 0;
	instruction[646].binOpcodeLen = 1;
	instruction[646].binOpcode = bpstrndup("\xA5", 1);
	instruction[646].opcodeOperandCount = 0;

	// Instruction Struct [647] - MOVSB
	instruction[647].instructionStr = strdup("MOVSB");
	instruction[647].undecodedStr = strdup("A4");
	instruction[647].valid64 = INVALID;
	instruction[647].compatLeg = VALID;
	instruction[647].VMX = 0;
	instruction[647].description = strdup("For legacy mode, Move byte from address DS:(E)SI to ES:(E)DI. For 64-bit mode move byte from address (R|E)SI to (R|E)DI.");
	instruction[647].binPrefixLen = 0;
	instruction[647].binOpcodeLen = 1;
	instruction[647].binOpcode = bpstrndup("\xA4", 1);
	instruction[647].opcodeOperandCount = 0;

	// Instruction Struct [648] - MOVSW
	instruction[648].instructionStr = strdup("MOVSW");
	instruction[648].undecodedStr = strdup("A5");
	instruction[648].valid64 = INVALID;
	instruction[648].compatLeg = VALID;
	instruction[648].VMX = 0;
	instruction[648].description = strdup("For legacy mode, move word from address DS:(E)SI to ES:(E)DI. For 64-bit mode move word at address (R|E)SI to (R|E)DI.");
	instruction[648].binPrefixLen = 0;
	instruction[648].binOpcodeLen = 1;
	instruction[648].binOpcode = bpstrndup("\xA5", 1);
	instruction[648].opcodeOperandCount = 0;

	// Instruction Struct [649] - MOVSD
	instruction[649].instructionStr = strdup("MOVSD");
	instruction[649].undecodedStr = strdup("A5");
	instruction[649].valid64 = INVALID;
	instruction[649].compatLeg = VALID;
	instruction[649].VMX = 0;
	instruction[649].description = strdup("For legacy mode, move dword from address DS:(E)SI to ES:(E)DI. For 64-bit mode move dword from address (R|E)SI to (R|E)DI.");
	instruction[649].binPrefixLen = 0;
	instruction[649].binOpcodeLen = 1;
	instruction[649].binOpcode = bpstrndup("\xA5", 1);
	instruction[649].opcodeOperandCount = 0;

	// Instruction Struct [650] - MOVSD
	instruction[650].instructionStr = strdup("MOVSD");
	instruction[650].undecodedStr = strdup("F2 0F 10 /r");
	instruction[650].dstOperandStr = strdup("xmm1");
	instruction[650].dstOperand = IA_XMM;
	instruction[650].srcOperandStr = strdup("xmm2/m64");
	instruction[650].srcOperand = IA_XMM_M64;
	instruction[650].valid64 = INVALID;
	instruction[650].compatLeg = VALID;
	instruction[650].VMX = 0;
	instruction[650].description = strdup("Move scalar double-precision floating-point value from xmm2/m64 to xmm1 register.");
	instruction[650].prefixStr = strdup("REPNE");
	instruction[650].binPrefixLen = 1;
	instruction[650].binPrefix = bpstrndup("\xF2", 1);
	instruction[650].binOpcodeLen = 2;
	instruction[650].binOpcode = bpstrndup("\x0F\x10", 2);
	instruction[650].opcodeOperand[0] = OA_MODRM;
	instruction[650].opcodeOperandCount = 1;

	// Instruction Struct [651] - MOVSD
	instruction[651].instructionStr = strdup("MOVSD");
	instruction[651].undecodedStr = strdup("F2 0F 11 /r");
	instruction[651].dstOperandStr = strdup("xmm2/m64");
	instruction[651].dstOperand = IA_XMM_M64;
	instruction[651].srcOperandStr = strdup("xmm1");
	instruction[651].srcOperand = IA_XMM;
	instruction[651].valid64 = INVALID;
	instruction[651].compatLeg = VALID;
	instruction[651].VMX = 0;
	instruction[651].description = strdup("Move scalar double-precision floating-point value from xmm1 register to xmm2/m64.");
	instruction[651].prefixStr = strdup("REPNE");
	instruction[651].binPrefixLen = 1;
	instruction[651].binPrefix = bpstrndup("\xF2", 1);
	instruction[651].binOpcodeLen = 2;
	instruction[651].binOpcode = bpstrndup("\x0F\x11", 2);
	instruction[651].opcodeOperand[0] = OA_MODRM;
	instruction[651].opcodeOperandCount = 1;

	// Instruction Struct [652] - MOVSHDUP
	instruction[652].instructionStr = strdup("MOVSHDUP");
	instruction[652].undecodedStr = strdup("F3 0F 16 /r");
	instruction[652].dstOperandStr = strdup("xmm1");
	instruction[652].dstOperand = IA_XMM;
	instruction[652].srcOperandStr = strdup("xmm2/m128");
	instruction[652].srcOperand = IA_XMM_M128;
	instruction[652].valid64 = INVALID;
	instruction[652].compatLeg = VALID;
	instruction[652].VMX = 0;
	instruction[652].description = strdup("Move two single-precision floatingpoint values from the higher 32-bit operand of each qword in xmm2/m128 to xmm1 and duplicate each 32-bit operand to the lower 32-bits of each qword.");
	instruction[652].prefixStr = strdup("REP");
	instruction[652].binPrefixLen = 1;
	instruction[652].binPrefix = bpstrndup("\xF3", 1);
	instruction[652].binOpcodeLen = 2;
	instruction[652].binOpcode = bpstrndup("\x0F\x16", 2);
	instruction[652].opcodeOperand[0] = OA_MODRM;
	instruction[652].opcodeOperandCount = 1;

	// Instruction Struct [653] - MOVSLDUP
	instruction[653].instructionStr = strdup("MOVSLDUP");
	instruction[653].undecodedStr = strdup("F3 0F 12 /r");
	instruction[653].dstOperandStr = strdup("xmm1");
	instruction[653].dstOperand = IA_XMM;
	instruction[653].srcOperandStr = strdup("xmm2/m128");
	instruction[653].srcOperand = IA_XMM_M128;
	instruction[653].valid64 = INVALID;
	instruction[653].compatLeg = VALID;
	instruction[653].VMX = 0;
	instruction[653].description = strdup("Move two single-precision floating-point values from the lower 32-bit operand of each qword in xmm2/m128 to xmm1 and duplicate each 32-bit operand to the higher 32-bits of each qword.");
	instruction[653].prefixStr = strdup("REP");
	instruction[653].binPrefixLen = 1;
	instruction[653].binPrefix = bpstrndup("\xF3", 1);
	instruction[653].binOpcodeLen = 2;
	instruction[653].binOpcode = bpstrndup("\x0F\x12", 2);
	instruction[653].opcodeOperand[0] = OA_MODRM;
	instruction[653].opcodeOperandCount = 1;

	// Instruction Struct [654] - MOVSS
	instruction[654].instructionStr = strdup("MOVSS");
	instruction[654].undecodedStr = strdup("F3 0F 10 /r");
	instruction[654].dstOperandStr = strdup("xmm1");
	instruction[654].dstOperand = IA_XMM;
	instruction[654].srcOperandStr = strdup("xmm2/m32");
	instruction[654].srcOperand = IA_XMM_M32;
	instruction[654].valid64 = INVALID;
	instruction[654].compatLeg = VALID;
	instruction[654].VMX = 0;
	instruction[654].description = strdup("Move scalar single-precision floating-point value from xmm2/m32 to xmm1 register.");
	instruction[654].prefixStr = strdup("REP");
	instruction[654].binPrefixLen = 1;
	instruction[654].binPrefix = bpstrndup("\xF3", 1);
	instruction[654].binOpcodeLen = 2;
	instruction[654].binOpcode = bpstrndup("\x0F\x10", 2);
	instruction[654].opcodeOperand[0] = OA_MODRM;
	instruction[654].opcodeOperandCount = 1;

	// Instruction Struct [655] - MOVSS
	instruction[655].instructionStr = strdup("MOVSS");
	instruction[655].undecodedStr = strdup("F3 0F 11 /r");
	instruction[655].dstOperandStr = strdup("xmm2/m32");
	instruction[655].dstOperand = IA_XMM_M32;
	instruction[655].srcOperandStr = strdup("xmm");
	instruction[655].srcOperand = IA_XMM;
	instruction[655].valid64 = INVALID;
	instruction[655].compatLeg = VALID;
	instruction[655].VMX = 0;
	instruction[655].description = strdup("Move scalar single-precision floating-point value from xmm1 register to xmm2/m32.");
	instruction[655].prefixStr = strdup("REP");
	instruction[655].binPrefixLen = 1;
	instruction[655].binPrefix = bpstrndup("\xF3", 1);
	instruction[655].binOpcodeLen = 2;
	instruction[655].binOpcode = bpstrndup("\x0F\x11", 2);
	instruction[655].opcodeOperand[0] = OA_MODRM;
	instruction[655].opcodeOperandCount = 1;

	// Instruction Struct [656] - MOVSX
	instruction[656].instructionStr = strdup("MOVSX");
	instruction[656].undecodedStr = strdup("0F BE /r");
	instruction[656].dstOperandStr = strdup("r16");
	instruction[656].dstOperand = IA_R16;
	instruction[656].srcOperandStr = strdup("r/m8");
	instruction[656].srcOperand = IA_RM8;
	instruction[656].valid64 = INVALID;
	instruction[656].compatLeg = VALID;
	instruction[656].VMX = 0;
	instruction[656].description = strdup("Move byte to word with signextension.");
	instruction[656].binPrefixLen = 0;
	instruction[656].binOpcodeLen = 2;
	instruction[656].binOpcode = bpstrndup("\x0F\xBE", 2);
	instruction[656].opcodeOperand[0] = OA_MODRM;
	instruction[656].opcodeOperandCount = 1;

	// Instruction Struct [657] - MOVSX
	instruction[657].instructionStr = strdup("MOVSX");
	instruction[657].undecodedStr = strdup("0F BE /r");
	instruction[657].dstOperandStr = strdup("r32");
	instruction[657].dstOperand = IA_R32;
	instruction[657].srcOperandStr = strdup("r/m8");
	instruction[657].srcOperand = IA_RM8;
	instruction[657].valid64 = INVALID;
	instruction[657].compatLeg = VALID;
	instruction[657].VMX = 0;
	instruction[657].description = strdup("Move byte to doubleword with sign-extension.");
	instruction[657].binPrefixLen = 0;
	instruction[657].binOpcodeLen = 2;
	instruction[657].binOpcode = bpstrndup("\x0F\xBE", 2);
	instruction[657].opcodeOperand[0] = OA_MODRM;
	instruction[657].opcodeOperandCount = 1;

	// Instruction Struct [658] - MOVSX
	instruction[658].instructionStr = strdup("MOVSX");
	instruction[658].undecodedStr = strdup("0F BF /r");
	instruction[658].dstOperandStr = strdup("r32");
	instruction[658].dstOperand = IA_R32;
	instruction[658].srcOperandStr = strdup("r/m16");
	instruction[658].srcOperand = IA_RM16;
	instruction[658].valid64 = INVALID;
	instruction[658].compatLeg = VALID;
	instruction[658].VMX = 0;
	instruction[658].description = strdup("Move word to doubleword, with sign-extension.");
	instruction[658].binPrefixLen = 0;
	instruction[658].binOpcodeLen = 2;
	instruction[658].binOpcode = bpstrndup("\x0F\xBF", 2);
	instruction[658].opcodeOperand[0] = OA_MODRM;
	instruction[658].opcodeOperandCount = 1;

	// Instruction Struct [659] - MOVUPD
	instruction[659].instructionStr = strdup("MOVUPD");
	instruction[659].undecodedStr = strdup("66 0F 10 /r");
	instruction[659].dstOperandStr = strdup("xmm1");
	instruction[659].dstOperand = IA_XMM;
	instruction[659].srcOperandStr = strdup("xmm2/m128");
	instruction[659].srcOperand = IA_XMM_M128;
	instruction[659].valid64 = INVALID;
	instruction[659].compatLeg = VALID;
	instruction[659].VMX = 0;
	instruction[659].description = strdup("Move packed double-precision floating-point values from xmm2/m128 to xmm1.");
	instruction[659].prefixStr = strdup("OPSIZEOVR");
	instruction[659].binPrefixLen = 1;
	instruction[659].binPrefix = bpstrndup("\x66", 1);
	instruction[659].binOpcodeLen = 2;
	instruction[659].binOpcode = bpstrndup("\x0F\x10", 2);
	instruction[659].opcodeOperand[0] = OA_MODRM;
	instruction[659].opcodeOperandCount = 1;

	// Instruction Struct [660] - MOVUPD
	instruction[660].instructionStr = strdup("MOVUPD");
	instruction[660].undecodedStr = strdup("66 0F 11 /r");
	instruction[660].dstOperandStr = strdup("xmm2/m128");
	instruction[660].dstOperand = IA_XMM_M128;
	instruction[660].srcOperandStr = strdup("xmm");
	instruction[660].srcOperand = IA_XMM;
	instruction[660].valid64 = INVALID;
	instruction[660].compatLeg = VALID;
	instruction[660].VMX = 0;
	instruction[660].description = strdup("Move packed double-precision floating-point values from xmm1 to xmm2/m128.");
	instruction[660].prefixStr = strdup("OPSIZEOVR");
	instruction[660].binPrefixLen = 1;
	instruction[660].binPrefix = bpstrndup("\x66", 1);
	instruction[660].binOpcodeLen = 2;
	instruction[660].binOpcode = bpstrndup("\x0F\x11", 2);
	instruction[660].opcodeOperand[0] = OA_MODRM;
	instruction[660].opcodeOperandCount = 1;

	// Instruction Struct [661] - MOVUPS
	instruction[661].instructionStr = strdup("MOVUPS");
	instruction[661].undecodedStr = strdup("0F 10 /r");
	instruction[661].dstOperandStr = strdup("xmm1");
	instruction[661].dstOperand = IA_XMM;
	instruction[661].srcOperandStr = strdup("xmm2/m128");
	instruction[661].srcOperand = IA_XMM_M128;
	instruction[661].valid64 = INVALID;
	instruction[661].compatLeg = VALID;
	instruction[661].VMX = 0;
	instruction[661].description = strdup("Move packed single-precision floatingpoint values from xmm2/m128 to xmm1.");
	instruction[661].binPrefixLen = 0;
	instruction[661].binOpcodeLen = 2;
	instruction[661].binOpcode = bpstrndup("\x0F\x10", 2);
	instruction[661].opcodeOperand[0] = OA_MODRM;
	instruction[661].opcodeOperandCount = 1;

	// Instruction Struct [662] - MOVUPS
	instruction[662].instructionStr = strdup("MOVUPS");
	instruction[662].undecodedStr = strdup("0F 11 /r");
	instruction[662].dstOperandStr = strdup("xmm2/m128");
	instruction[662].dstOperand = IA_XMM_M128;
	instruction[662].srcOperandStr = strdup("xmm1");
	instruction[662].srcOperand = IA_XMM;
	instruction[662].valid64 = INVALID;
	instruction[662].compatLeg = VALID;
	instruction[662].VMX = 0;
	instruction[662].description = strdup("Move packed single-precision floatingpoint");
	instruction[662].binPrefixLen = 0;
	instruction[662].binOpcodeLen = 2;
	instruction[662].binOpcode = bpstrndup("\x0F\x11", 2);
	instruction[662].opcodeOperand[0] = OA_MODRM;
	instruction[662].opcodeOperandCount = 1;

	// Instruction Struct [663] - MOVZX
	instruction[663].instructionStr = strdup("MOVZX");
	instruction[663].undecodedStr = strdup("0F B6 /r");
	instruction[663].dstOperandStr = strdup("r16");
	instruction[663].dstOperand = IA_R16;
	instruction[663].srcOperandStr = strdup("r/m8");
	instruction[663].srcOperand = IA_RM8;
	instruction[663].valid64 = INVALID;
	instruction[663].compatLeg = VALID;
	instruction[663].VMX = 0;
	instruction[663].description = strdup("Move byte to word with zeroextension.");
	instruction[663].binPrefixLen = 0;
	instruction[663].binOpcodeLen = 2;
	instruction[663].binOpcode = bpstrndup("\x0F\xB6", 2);
	instruction[663].opcodeOperand[0] = OA_MODRM;
	instruction[663].opcodeOperandCount = 1;

	// Instruction Struct [664] - MOVZX
	instruction[664].instructionStr = strdup("MOVZX");
	instruction[664].undecodedStr = strdup("0F B6 /r");
	instruction[664].dstOperandStr = strdup("r32");
	instruction[664].dstOperand = IA_R32;
	instruction[664].srcOperandStr = strdup("r/m8");
	instruction[664].srcOperand = IA_RM8;
	instruction[664].valid64 = INVALID;
	instruction[664].compatLeg = VALID;
	instruction[664].VMX = 0;
	instruction[664].description = strdup("Move byte to doubleword, zero-extension.");
	instruction[664].binPrefixLen = 0;
	instruction[664].binOpcodeLen = 2;
	instruction[664].binOpcode = bpstrndup("\x0F\xB6", 2);
	instruction[664].opcodeOperand[0] = OA_MODRM;
	instruction[664].opcodeOperandCount = 1;

	// Instruction Struct [665] - MOVZX
	instruction[665].instructionStr = strdup("MOVZX");
	instruction[665].undecodedStr = strdup("0F B7 /r");
	instruction[665].dstOperandStr = strdup("r32");
	instruction[665].dstOperand = IA_R32;
	instruction[665].srcOperandStr = strdup("r/m16");
	instruction[665].srcOperand = IA_RM16;
	instruction[665].valid64 = INVALID;
	instruction[665].compatLeg = VALID;
	instruction[665].VMX = 0;
	instruction[665].description = strdup("Move word to doubleword, zero-extension.");
	instruction[665].binPrefixLen = 0;
	instruction[665].binOpcodeLen = 2;
	instruction[665].binOpcode = bpstrndup("\x0F\xB7", 2);
	instruction[665].opcodeOperand[0] = OA_MODRM;
	instruction[665].opcodeOperandCount = 1;

	// Instruction Struct [666] - MUL
	instruction[666].instructionStr = strdup("MUL");
	instruction[666].undecodedStr = strdup("F6 /4");
	instruction[666].dstOperandStr = strdup("r/m8");
	instruction[666].dstOperand = IA_RM8;
	instruction[666].valid64 = INVALID;
	instruction[666].compatLeg = VALID;
	instruction[666].VMX = 0;
	instruction[666].description = strdup("Unsigned multiply (AX ? AL ? r/m8).");
	instruction[666].binPrefixLen = 0;
	instruction[666].binOpcodeLen = 1;
	instruction[666].binOpcode = bpstrndup("\xF6", 1);
	instruction[666].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[666].opcodeOperandCount = 1;

	// Instruction Struct [667] - MUL
	instruction[667].instructionStr = strdup("MUL");
	instruction[667].undecodedStr = strdup("F7 /4");
	instruction[667].dstOperandStr = strdup("r/m16");
	instruction[667].dstOperand = IA_RM16;
	instruction[667].valid64 = INVALID;
	instruction[667].compatLeg = VALID;
	instruction[667].VMX = 0;
	instruction[667].description = strdup("Unsigned multiply (DX:AX ? AX ? r/m16).");
	instruction[667].binPrefixLen = 0;
	instruction[667].binOpcodeLen = 1;
	instruction[667].binOpcode = bpstrndup("\xF7", 1);
	instruction[667].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[667].opcodeOperandCount = 1;

	// Instruction Struct [668] - MUL
	instruction[668].instructionStr = strdup("MUL");
	instruction[668].undecodedStr = strdup("F7 /4");
	instruction[668].dstOperandStr = strdup("r/m32");
	instruction[668].dstOperand = IA_RM32;
	instruction[668].valid64 = INVALID;
	instruction[668].compatLeg = VALID;
	instruction[668].VMX = 0;
	instruction[668].description = strdup("Unsigned multiply (EDX:EAX ? EAX ? r/m32).");
	instruction[668].binPrefixLen = 0;
	instruction[668].binOpcodeLen = 1;
	instruction[668].binOpcode = bpstrndup("\xF7", 1);
	instruction[668].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[668].opcodeOperandCount = 1;

	// Instruction Struct [669] - MULPD
	instruction[669].instructionStr = strdup("MULPD");
	instruction[669].undecodedStr = strdup("66 0F 59 /r");
	instruction[669].dstOperandStr = strdup("xmm1");
	instruction[669].dstOperand = IA_XMM;
	instruction[669].srcOperandStr = strdup("xmm2/m128");
	instruction[669].srcOperand = IA_XMM_M128;
	instruction[669].valid64 = INVALID;
	instruction[669].compatLeg = VALID;
	instruction[669].VMX = 0;
	instruction[669].description = strdup("Multiply packed double-precision floating-point values in xmm2/m128 by xmm1.");
	instruction[669].prefixStr = strdup("OPSIZEOVR");
	instruction[669].binPrefixLen = 1;
	instruction[669].binPrefix = bpstrndup("\x66", 1);
	instruction[669].binOpcodeLen = 2;
	instruction[669].binOpcode = bpstrndup("\x0F\x59", 2);
	instruction[669].opcodeOperand[0] = OA_MODRM;
	instruction[669].opcodeOperandCount = 1;

	// Instruction Struct [670] - MULPS
	instruction[670].instructionStr = strdup("MULPS");
	instruction[670].undecodedStr = strdup("0F 59 /r");
	instruction[670].dstOperandStr = strdup("xmm1");
	instruction[670].dstOperand = IA_XMM;
	instruction[670].srcOperandStr = strdup("xmm2/m128");
	instruction[670].srcOperand = IA_XMM_M128;
	instruction[670].valid64 = INVALID;
	instruction[670].compatLeg = VALID;
	instruction[670].VMX = 0;
	instruction[670].description = strdup("Multiply packed single-precision floating-point values in xmm2/mem by xmm1.");
	instruction[670].binPrefixLen = 0;
	instruction[670].binOpcodeLen = 2;
	instruction[670].binOpcode = bpstrndup("\x0F\x59", 2);
	instruction[670].opcodeOperand[0] = OA_MODRM;
	instruction[670].opcodeOperandCount = 1;

	// Instruction Struct [671] - MULSD
	instruction[671].instructionStr = strdup("MULSD");
	instruction[671].undecodedStr = strdup("F2 0F 59 /r");
	instruction[671].dstOperandStr = strdup("xmm1");
	instruction[671].dstOperand = IA_XMM;
	instruction[671].srcOperandStr = strdup("xmm2/m64");
	instruction[671].srcOperand = IA_XMM_M64;
	instruction[671].valid64 = INVALID;
	instruction[671].compatLeg = VALID;
	instruction[671].VMX = 0;
	instruction[671].description = strdup("Multiply the low double-precision floating-point value in xmm2/mem64 by low double-precision floating-point value in xmm1.");
	instruction[671].prefixStr = strdup("REPNE");
	instruction[671].binPrefixLen = 1;
	instruction[671].binPrefix = bpstrndup("\xF2", 1);
	instruction[671].binOpcodeLen = 2;
	instruction[671].binOpcode = bpstrndup("\x0F\x59", 2);
	instruction[671].opcodeOperand[0] = OA_MODRM;
	instruction[671].opcodeOperandCount = 1;

	// Instruction Struct [672] - MULS
	instruction[672].instructionStr = strdup("MULS");
	instruction[672].undecodedStr = strdup("F3 0F 59 /r");
	instruction[672].dstOperandStr = strdup("xmm1");
	instruction[672].dstOperand = IA_XMM;
	instruction[672].srcOperandStr = strdup("xmm2/m32");
	instruction[672].srcOperand = IA_XMM_M32;
	instruction[672].valid64 = INVALID;
	instruction[672].compatLeg = VALID;
	instruction[672].VMX = 0;
	instruction[672].description = strdup("Multiply the low single-precision floatingpoint value in xmm2/mem by the low single-precision floating-point value in xmm1.");
	instruction[672].prefixStr = strdup("REP");
	instruction[672].binPrefixLen = 1;
	instruction[672].binPrefix = bpstrndup("\xF3", 1);
	instruction[672].binOpcodeLen = 2;
	instruction[672].binOpcode = bpstrndup("\x0F\x59", 2);
	instruction[672].opcodeOperand[0] = OA_MODRM;
	instruction[672].opcodeOperandCount = 1;

	// Instruction Struct [673] - MWAIT
	instruction[673].instructionStr = strdup("MWAIT");
	instruction[673].undecodedStr = strdup("0F 01 C9");
	instruction[673].valid64 = INVALID;
	instruction[673].compatLeg = VALID;
	instruction[673].VMX = 0;
	instruction[673].description = strdup("A hint that allow the processor to stop");
	instruction[673].binPrefixLen = 0;
	instruction[673].binOpcodeLen = 3;
	instruction[673].binOpcode = bpstrndup("\x0F\x01\xC9", 3);
	instruction[673].opcodeOperandCount = 0;

	// Instruction Struct [674] - CMP
	instruction[674].instructionStr = strdup("CMP");
	instruction[674].undecodedStr = strdup("3D id");
	instruction[674].dstOperandStr = strdup("EAX");
	instruction[674].dstOperand = IA_EAX;
	instruction[674].srcOperandStr = strdup("imm32");
	instruction[674].srcOperand = IA_IMM32;
	instruction[674].valid64 = INVALID;
	instruction[674].compatLeg = VALID;
	instruction[674].VMX = 0;
	instruction[674].description = strdup("Compare imm32 with EAX.");
	instruction[674].binPrefixLen = 0;
	instruction[674].binOpcodeLen = 1;
	instruction[674].binOpcode = bpstrndup("\x3D", 1);
	instruction[674].opcodeOperand[0] = OA_ID;
	instruction[674].opcodeOperandCount = 1;

	// Instruction Struct [675] - NEG
	instruction[675].instructionStr = strdup("NEG");
	instruction[675].undecodedStr = strdup("F6 /3");
	instruction[675].dstOperandStr = strdup("r/m8");
	instruction[675].dstOperand = IA_RM8;
	instruction[675].valid64 = INVALID;
	instruction[675].compatLeg = VALID;
	instruction[675].VMX = 0;
	instruction[675].description = strdup("Two's complement negate r/m8.");
	instruction[675].binPrefixLen = 0;
	instruction[675].binOpcodeLen = 1;
	instruction[675].binOpcode = bpstrndup("\xF6", 1);
	instruction[675].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[675].opcodeOperandCount = 1;

	// Instruction Struct [676] - NEG
	instruction[676].instructionStr = strdup("NEG");
	instruction[676].undecodedStr = strdup("F7 /3");
	instruction[676].dstOperandStr = strdup("r/m16");
	instruction[676].dstOperand = IA_RM16;
	instruction[676].valid64 = INVALID;
	instruction[676].compatLeg = VALID;
	instruction[676].VMX = 0;
	instruction[676].description = strdup("Two's complement negate r/m16.");
	instruction[676].binPrefixLen = 0;
	instruction[676].binOpcodeLen = 1;
	instruction[676].binOpcode = bpstrndup("\xF7", 1);
	instruction[676].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[676].opcodeOperandCount = 1;

	// Instruction Struct [677] - NEG
	instruction[677].instructionStr = strdup("NEG");
	instruction[677].undecodedStr = strdup("F7 /3");
	instruction[677].dstOperandStr = strdup("r/m32");
	instruction[677].dstOperand = IA_RM32;
	instruction[677].valid64 = INVALID;
	instruction[677].compatLeg = VALID;
	instruction[677].VMX = 0;
	instruction[677].description = strdup("Two's complement negate r/m32.");
	instruction[677].binPrefixLen = 0;
	instruction[677].binOpcodeLen = 1;
	instruction[677].binOpcode = bpstrndup("\xF7", 1);
	instruction[677].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[677].opcodeOperandCount = 1;

	// Instruction Struct [678] - NOP
	instruction[678].instructionStr = strdup("NOP");
	instruction[678].undecodedStr = strdup("90");
	instruction[678].valid64 = INVALID;
	instruction[678].compatLeg = VALID;
	instruction[678].VMX = 0;
	instruction[678].description = strdup("One byte no-operation instruction.");
	instruction[678].binPrefixLen = 0;
	instruction[678].binOpcodeLen = 1;
	instruction[678].binOpcode = bpstrndup("\x90", 1);
	instruction[678].opcodeOperandCount = 0;

	// Instruction Struct [679] - NOP
	instruction[679].instructionStr = strdup("NOP");
	instruction[679].undecodedStr = strdup("0F 1F /0");
	instruction[679].dstOperandStr = strdup("r/m16");
	instruction[679].dstOperand = IA_RM16;
	instruction[679].valid64 = INVALID;
	instruction[679].compatLeg = VALID;
	instruction[679].VMX = 0;
	instruction[679].description = strdup("Multi-byte no-operation instruction.");
	instruction[679].binPrefixLen = 0;
	instruction[679].binOpcodeLen = 2;
	instruction[679].binOpcode = bpstrndup("\x0F\x1F", 2);
	instruction[679].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[679].opcodeOperandCount = 1;

	// Instruction Struct [680] - NOP
	instruction[680].instructionStr = strdup("NOP");
	instruction[680].undecodedStr = strdup("0F 1F /0");
	instruction[680].dstOperandStr = strdup("r/m32");
	instruction[680].dstOperand = IA_RM32;
	instruction[680].valid64 = INVALID;
	instruction[680].compatLeg = VALID;
	instruction[680].VMX = 0;
	instruction[680].description = strdup("Multi-byte no-operation instruction.");
	instruction[680].binPrefixLen = 0;
	instruction[680].binOpcodeLen = 2;
	instruction[680].binOpcode = bpstrndup("\x0F\x1F", 2);
	instruction[680].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[680].opcodeOperandCount = 1;

	// Instruction Struct [681] - NOT
	instruction[681].instructionStr = strdup("NOT");
	instruction[681].undecodedStr = strdup("F6 /2");
	instruction[681].dstOperandStr = strdup("r/m8");
	instruction[681].dstOperand = IA_RM8;
	instruction[681].valid64 = INVALID;
	instruction[681].compatLeg = VALID;
	instruction[681].VMX = 0;
	instruction[681].description = strdup("Reverse each bit of r/m8.");
	instruction[681].binPrefixLen = 0;
	instruction[681].binOpcodeLen = 1;
	instruction[681].binOpcode = bpstrndup("\xF6", 1);
	instruction[681].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[681].opcodeOperandCount = 1;

	// Instruction Struct [682] - NOT
	instruction[682].instructionStr = strdup("NOT");
	instruction[682].undecodedStr = strdup("F7 /2");
	instruction[682].dstOperandStr = strdup("r/m16");
	instruction[682].dstOperand = IA_RM16;
	instruction[682].valid64 = INVALID;
	instruction[682].compatLeg = VALID;
	instruction[682].VMX = 0;
	instruction[682].description = strdup("Reverse each bit of r/m16.");
	instruction[682].binPrefixLen = 0;
	instruction[682].binOpcodeLen = 1;
	instruction[682].binOpcode = bpstrndup("\xF7", 1);
	instruction[682].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[682].opcodeOperandCount = 1;

	// Instruction Struct [683] - NOT
	instruction[683].instructionStr = strdup("NOT");
	instruction[683].undecodedStr = strdup("F7 /2");
	instruction[683].dstOperandStr = strdup("r/m32");
	instruction[683].dstOperand = IA_RM32;
	instruction[683].valid64 = INVALID;
	instruction[683].compatLeg = VALID;
	instruction[683].VMX = 0;
	instruction[683].description = strdup("Reverse each bit of r/m32.");
	instruction[683].binPrefixLen = 0;
	instruction[683].binOpcodeLen = 1;
	instruction[683].binOpcode = bpstrndup("\xF7", 1);
	instruction[683].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[683].opcodeOperandCount = 1;

	// Instruction Struct [684] - OR
	instruction[684].instructionStr = strdup("OR");
	instruction[684].undecodedStr = strdup("0C ib");
	instruction[684].dstOperandStr = strdup("AL");
	instruction[684].dstOperand = IA_AL;
	instruction[684].srcOperandStr = strdup("imm8");
	instruction[684].srcOperand = IA_IMM8;
	instruction[684].valid64 = INVALID;
	instruction[684].compatLeg = VALID;
	instruction[684].VMX = 0;
	instruction[684].description = strdup("AL OR imm8.");
	instruction[684].binPrefixLen = 0;
	instruction[684].binOpcodeLen = 1;
	instruction[684].binOpcode = bpstrndup("\x0C", 1);
	instruction[684].opcodeOperand[0] = OA_IB;
	instruction[684].opcodeOperandCount = 1;

	// Instruction Struct [685] - OR
	instruction[685].instructionStr = strdup("OR");
	instruction[685].undecodedStr = strdup("0D iw");
	instruction[685].dstOperandStr = strdup("AX");
	instruction[685].dstOperand = IA_AX;
	instruction[685].srcOperandStr = strdup("imm16");
	instruction[685].srcOperand = IA_IMM16;
	instruction[685].valid64 = INVALID;
	instruction[685].compatLeg = VALID;
	instruction[685].VMX = 0;
	instruction[685].description = strdup("AX OR imm16.");
	instruction[685].binPrefixLen = 0;
	instruction[685].binOpcodeLen = 1;
	instruction[685].binOpcode = bpstrndup("\x0D", 1);
	instruction[685].opcodeOperand[0] = OA_IW;
	instruction[685].opcodeOperandCount = 1;

	// Instruction Struct [686] - OR
	instruction[686].instructionStr = strdup("OR");
	instruction[686].undecodedStr = strdup("0D id");
	instruction[686].dstOperandStr = strdup("EAX");
	instruction[686].dstOperand = IA_EAX;
	instruction[686].srcOperandStr = strdup("imm32");
	instruction[686].srcOperand = IA_IMM32;
	instruction[686].valid64 = INVALID;
	instruction[686].compatLeg = VALID;
	instruction[686].VMX = 0;
	instruction[686].description = strdup("EAX OR imm32.");
	instruction[686].binPrefixLen = 0;
	instruction[686].binOpcodeLen = 1;
	instruction[686].binOpcode = bpstrndup("\x0D", 1);
	instruction[686].opcodeOperand[0] = OA_ID;
	instruction[686].opcodeOperandCount = 1;

	// Instruction Struct [687] - OR
	instruction[687].instructionStr = strdup("OR");
	instruction[687].undecodedStr = strdup("80 /1 ib");
	instruction[687].dstOperandStr = strdup("r/m8");
	instruction[687].dstOperand = IA_RM8;
	instruction[687].srcOperandStr = strdup("imm8");
	instruction[687].srcOperand = IA_IMM8;
	instruction[687].valid64 = INVALID;
	instruction[687].compatLeg = VALID;
	instruction[687].VMX = 0;
	instruction[687].description = strdup("r/m8 OR imm8.");
	instruction[687].binPrefixLen = 0;
	instruction[687].binOpcodeLen = 1;
	instruction[687].binOpcode = bpstrndup("\x80", 1);
	instruction[687].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[687].opcodeOperand[1] = OA_IB;
	instruction[687].opcodeOperandCount = 2;

	// Instruction Struct [688] - OR
	instruction[688].instructionStr = strdup("OR");
	instruction[688].undecodedStr = strdup("81 /1 iw");
	instruction[688].dstOperandStr = strdup("r/m16");
	instruction[688].dstOperand = IA_RM16;
	instruction[688].srcOperandStr = strdup("imm16");
	instruction[688].srcOperand = IA_IMM16;
	instruction[688].valid64 = INVALID;
	instruction[688].compatLeg = VALID;
	instruction[688].VMX = 0;
	instruction[688].description = strdup("r/m16 OR imm16.");
	instruction[688].binPrefixLen = 0;
	instruction[688].binOpcodeLen = 1;
	instruction[688].binOpcode = bpstrndup("\x81", 1);
	instruction[688].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[688].opcodeOperand[1] = OA_IW;
	instruction[688].opcodeOperandCount = 2;

	// Instruction Struct [689] - OR
	instruction[689].instructionStr = strdup("OR");
	instruction[689].undecodedStr = strdup("81 /1 id");
	instruction[689].dstOperandStr = strdup("r/m32");
	instruction[689].dstOperand = IA_RM32;
	instruction[689].srcOperandStr = strdup("mm32");
	instruction[689].srcOperand = IA_MM32;
	instruction[689].valid64 = INVALID;
	instruction[689].compatLeg = VALID;
	instruction[689].VMX = 0;
	instruction[689].description = strdup("r/m32 OR imm32.");
	instruction[689].binPrefixLen = 0;
	instruction[689].binOpcodeLen = 1;
	instruction[689].binOpcode = bpstrndup("\x81", 1);
	instruction[689].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[689].opcodeOperand[1] = OA_ID;
	instruction[689].opcodeOperandCount = 2;

	// Instruction Struct [690] - OR
	instruction[690].instructionStr = strdup("OR");
	instruction[690].undecodedStr = strdup("83 /1 ib");
	instruction[690].dstOperandStr = strdup("r/m16");
	instruction[690].dstOperand = IA_RM16;
	instruction[690].srcOperandStr = strdup("imm8");
	instruction[690].srcOperand = IA_IMM8;
	instruction[690].valid64 = INVALID;
	instruction[690].compatLeg = VALID;
	instruction[690].VMX = 0;
	instruction[690].description = strdup("r/m16 OR imm8 (signextended).");
	instruction[690].binPrefixLen = 0;
	instruction[690].binOpcodeLen = 1;
	instruction[690].binOpcode = bpstrndup("\x83", 1);
	instruction[690].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[690].opcodeOperand[1] = OA_IB;
	instruction[690].opcodeOperandCount = 2;

	// Instruction Struct [691] - OR
	instruction[691].instructionStr = strdup("OR");
	instruction[691].undecodedStr = strdup("83 /1 ib");
	instruction[691].dstOperandStr = strdup("r/m32");
	instruction[691].dstOperand = IA_RM32;
	instruction[691].srcOperandStr = strdup("imm8");
	instruction[691].srcOperand = IA_IMM8;
	instruction[691].valid64 = INVALID;
	instruction[691].compatLeg = VALID;
	instruction[691].VMX = 0;
	instruction[691].description = strdup("r/m32 OR imm8 (signextended).");
	instruction[691].binPrefixLen = 0;
	instruction[691].binOpcodeLen = 1;
	instruction[691].binOpcode = bpstrndup("\x83", 1);
	instruction[691].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[691].opcodeOperand[1] = OA_IB;
	instruction[691].opcodeOperandCount = 2;

	// Instruction Struct [692] - OR
	instruction[692].instructionStr = strdup("OR");
	instruction[692].undecodedStr = strdup("08 /r");
	instruction[692].dstOperandStr = strdup("r/m8");
	instruction[692].dstOperand = IA_RM8;
	instruction[692].srcOperandStr = strdup("r8");
	instruction[692].srcOperand = IA_R8;
	instruction[692].valid64 = INVALID;
	instruction[692].compatLeg = VALID;
	instruction[692].VMX = 0;
	instruction[692].description = strdup("r/m8 OR r8.");
	instruction[692].binPrefixLen = 0;
	instruction[692].binOpcodeLen = 1;
	instruction[692].binOpcode = bpstrndup("\x08", 1);
	instruction[692].opcodeOperand[0] = OA_MODRM;
	instruction[692].opcodeOperandCount = 1;

	// Instruction Struct [693] - OR
	instruction[693].instructionStr = strdup("OR");
	instruction[693].undecodedStr = strdup("09 /r");
	instruction[693].dstOperandStr = strdup("r/m16");
	instruction[693].dstOperand = IA_RM16;
	instruction[693].srcOperandStr = strdup("r16");
	instruction[693].srcOperand = IA_R16;
	instruction[693].valid64 = INVALID;
	instruction[693].compatLeg = VALID;
	instruction[693].VMX = 0;
	instruction[693].description = strdup("r/m16 OR r16.");
	instruction[693].binPrefixLen = 0;
	instruction[693].binOpcodeLen = 1;
	instruction[693].binOpcode = bpstrndup("\x09", 1);
	instruction[693].opcodeOperand[0] = OA_MODRM;
	instruction[693].opcodeOperandCount = 1;

	// Instruction Struct [694] - OR
	instruction[694].instructionStr = strdup("OR");
	instruction[694].undecodedStr = strdup("09 /r");
	instruction[694].dstOperandStr = strdup("r/m32");
	instruction[694].dstOperand = IA_RM32;
	instruction[694].srcOperandStr = strdup("r32");
	instruction[694].srcOperand = IA_R32;
	instruction[694].valid64 = INVALID;
	instruction[694].compatLeg = VALID;
	instruction[694].VMX = 0;
	instruction[694].description = strdup("r/m32 OR r32.");
	instruction[694].binPrefixLen = 0;
	instruction[694].binOpcodeLen = 1;
	instruction[694].binOpcode = bpstrndup("\x09", 1);
	instruction[694].opcodeOperand[0] = OA_MODRM;
	instruction[694].opcodeOperandCount = 1;

	// Instruction Struct [695] - OR
	instruction[695].instructionStr = strdup("OR");
	instruction[695].undecodedStr = strdup("0A /r");
	instruction[695].dstOperandStr = strdup("r8");
	instruction[695].dstOperand = IA_R8;
	instruction[695].srcOperandStr = strdup("r/m8");
	instruction[695].srcOperand = IA_RM8;
	instruction[695].valid64 = INVALID;
	instruction[695].compatLeg = VALID;
	instruction[695].VMX = 0;
	instruction[695].description = strdup("r8 OR r/m8.");
	instruction[695].binPrefixLen = 0;
	instruction[695].binOpcodeLen = 1;
	instruction[695].binOpcode = bpstrndup("\x0A", 1);
	instruction[695].opcodeOperand[0] = OA_MODRM;
	instruction[695].opcodeOperandCount = 1;

	// Instruction Struct [696] - OR
	instruction[696].instructionStr = strdup("OR");
	instruction[696].undecodedStr = strdup("0B /r");
	instruction[696].dstOperandStr = strdup("r16");
	instruction[696].dstOperand = IA_R16;
	instruction[696].srcOperandStr = strdup("r/m16");
	instruction[696].srcOperand = IA_RM16;
	instruction[696].valid64 = INVALID;
	instruction[696].compatLeg = VALID;
	instruction[696].VMX = 0;
	instruction[696].description = strdup("r16 OR r/m16.");
	instruction[696].binPrefixLen = 0;
	instruction[696].binOpcodeLen = 1;
	instruction[696].binOpcode = bpstrndup("\x0B", 1);
	instruction[696].opcodeOperand[0] = OA_MODRM;
	instruction[696].opcodeOperandCount = 1;

	// Instruction Struct [697] - OR
	instruction[697].instructionStr = strdup("OR");
	instruction[697].undecodedStr = strdup("0B /r");
	instruction[697].dstOperandStr = strdup("r32");
	instruction[697].dstOperand = IA_R32;
	instruction[697].srcOperandStr = strdup("r/m32");
	instruction[697].srcOperand = IA_RM32;
	instruction[697].valid64 = INVALID;
	instruction[697].compatLeg = VALID;
	instruction[697].VMX = 0;
	instruction[697].description = strdup("r32 OR r/m32.");
	instruction[697].binPrefixLen = 0;
	instruction[697].binOpcodeLen = 1;
	instruction[697].binOpcode = bpstrndup("\x0B", 1);
	instruction[697].opcodeOperand[0] = OA_MODRM;
	instruction[697].opcodeOperandCount = 1;

	// Instruction Struct [698] - ORPD
	instruction[698].instructionStr = strdup("ORPD");
	instruction[698].undecodedStr = strdup("66 0F 56 /r");
	instruction[698].dstOperandStr = strdup("xmm1");
	instruction[698].dstOperand = IA_XMM;
	instruction[698].srcOperandStr = strdup("xmm2/m128");
	instruction[698].srcOperand = IA_XMM_M128;
	instruction[698].valid64 = INVALID;
	instruction[698].compatLeg = VALID;
	instruction[698].VMX = 0;
	instruction[698].description = strdup("Bitwise OR of xmm2/m128 and xmm1.");
	instruction[698].prefixStr = strdup("OPSIZEOVR");
	instruction[698].binPrefixLen = 1;
	instruction[698].binPrefix = bpstrndup("\x66", 1);
	instruction[698].binOpcodeLen = 2;
	instruction[698].binOpcode = bpstrndup("\x0F\x56", 2);
	instruction[698].opcodeOperand[0] = OA_MODRM;
	instruction[698].opcodeOperandCount = 1;

	// Instruction Struct [699] - ORPS
	instruction[699].instructionStr = strdup("ORPS");
	instruction[699].undecodedStr = strdup("0F 56 /r");
	instruction[699].dstOperandStr = strdup("xmm1");
	instruction[699].dstOperand = IA_XMM;
	instruction[699].srcOperandStr = strdup("xmm2/m128");
	instruction[699].srcOperand = IA_XMM_M128;
	instruction[699].valid64 = INVALID;
	instruction[699].compatLeg = VALID;
	instruction[699].VMX = 0;
	instruction[699].description = strdup("Bitwise OR of xmm2/m128 and xmm1.");
	instruction[699].binPrefixLen = 0;
	instruction[699].binOpcodeLen = 2;
	instruction[699].binOpcode = bpstrndup("\x0F\x56", 2);
	instruction[699].opcodeOperand[0] = OA_MODRM;
	instruction[699].opcodeOperandCount = 1;

	// Instruction Struct [700] - OUT
	instruction[700].instructionStr = strdup("OUT");
	instruction[700].undecodedStr = strdup("E6 ib");
	instruction[700].dstOperandStr = strdup("imm8");
	instruction[700].dstOperand = IA_IMM8;
	instruction[700].srcOperandStr = strdup("AL");
	instruction[700].srcOperand = IA_AL;
	instruction[700].valid64 = INVALID;
	instruction[700].compatLeg = VALID;
	instruction[700].VMX = 0;
	instruction[700].description = strdup("Output byte in AL to I/O port address imm8.");
	instruction[700].binPrefixLen = 0;
	instruction[700].binOpcodeLen = 1;
	instruction[700].binOpcode = bpstrndup("\xE6", 1);
	instruction[700].opcodeOperand[0] = OA_IB;
	instruction[700].opcodeOperandCount = 1;

	// Instruction Struct [701] - OUT
	instruction[701].instructionStr = strdup("OUT");
	instruction[701].undecodedStr = strdup("E7 ib");
	instruction[701].dstOperandStr = strdup("imm8");
	instruction[701].dstOperand = IA_IMM8;
	instruction[701].srcOperandStr = strdup("AX");
	instruction[701].srcOperand = IA_AX;
	instruction[701].valid64 = INVALID;
	instruction[701].compatLeg = VALID;
	instruction[701].VMX = 0;
	instruction[701].description = strdup("Output word in AX to I/O port");
	instruction[701].binPrefixLen = 0;
	instruction[701].binOpcodeLen = 1;
	instruction[701].binOpcode = bpstrndup("\xE7", 1);
	instruction[701].opcodeOperand[0] = OA_IB;
	instruction[701].opcodeOperandCount = 1;

	// Instruction Struct [702] - OUT
	instruction[702].instructionStr = strdup("OUT");
	instruction[702].undecodedStr = strdup("E7 ib");
	instruction[702].dstOperandStr = strdup("imm8");
	instruction[702].dstOperand = IA_IMM8;
	instruction[702].srcOperandStr = strdup("EAX");
	instruction[702].srcOperand = IA_EAX;
	instruction[702].valid64 = INVALID;
	instruction[702].compatLeg = VALID;
	instruction[702].VMX = 0;
	instruction[702].description = strdup("Output doubleword in EAX to I/O port address imm8.");
	instruction[702].binPrefixLen = 0;
	instruction[702].binOpcodeLen = 1;
	instruction[702].binOpcode = bpstrndup("\xE7", 1);
	instruction[702].opcodeOperand[0] = OA_IB;
	instruction[702].opcodeOperandCount = 1;

	// Instruction Struct [703] - OUT
	instruction[703].instructionStr = strdup("OUT");
	instruction[703].undecodedStr = strdup("EE");
	instruction[703].dstOperandStr = strdup("DX");
	instruction[703].dstOperand = IA_DX;
	instruction[703].srcOperandStr = strdup("AL");
	instruction[703].srcOperand = IA_AL;
	instruction[703].valid64 = INVALID;
	instruction[703].compatLeg = VALID;
	instruction[703].VMX = 0;
	instruction[703].description = strdup("Output byte in AL to I/O port address in DX.");
	instruction[703].binPrefixLen = 0;
	instruction[703].binOpcodeLen = 1;
	instruction[703].binOpcode = bpstrndup("\xEE", 1);
	instruction[703].opcodeOperandCount = 0;

	// Instruction Struct [704] - OUT
	instruction[704].instructionStr = strdup("OUT");
	instruction[704].undecodedStr = strdup("EF");
	instruction[704].dstOperandStr = strdup("DX");
	instruction[704].dstOperand = IA_DX;
	instruction[704].srcOperandStr = strdup("AX");
	instruction[704].srcOperand = IA_AX;
	instruction[704].valid64 = INVALID;
	instruction[704].compatLeg = VALID;
	instruction[704].VMX = 0;
	instruction[704].description = strdup("Output word in AX to I/O port address in DX.");
	instruction[704].binPrefixLen = 0;
	instruction[704].binOpcodeLen = 1;
	instruction[704].binOpcode = bpstrndup("\xEF", 1);
	instruction[704].opcodeOperandCount = 0;

	// Instruction Struct [705] - OUT
	instruction[705].instructionStr = strdup("OUT");
	instruction[705].undecodedStr = strdup("EF");
	instruction[705].dstOperandStr = strdup("DX");
	instruction[705].dstOperand = IA_DX;
	instruction[705].srcOperandStr = strdup("EAX");
	instruction[705].srcOperand = IA_EAX;
	instruction[705].valid64 = INVALID;
	instruction[705].compatLeg = VALID;
	instruction[705].VMX = 0;
	instruction[705].description = strdup("Output doubleword in EAX to I/O port address in DX.");
	instruction[705].binPrefixLen = 0;
	instruction[705].binOpcodeLen = 1;
	instruction[705].binOpcode = bpstrndup("\xEF", 1);
	instruction[705].opcodeOperandCount = 0;

	// Instruction Struct [706] - OUTS
	instruction[706].instructionStr = strdup("OUTS");
	instruction[706].undecodedStr = strdup("6E");
	instruction[706].dstOperandStr = strdup("DX");
	instruction[706].dstOperand = IA_DX;
	instruction[706].srcOperandStr = strdup("m8");
	instruction[706].srcOperand = IA_M8;
	instruction[706].valid64 = INVALID;
	instruction[706].compatLeg = VALID;
	instruction[706].VMX = 0;
	instruction[706].description = strdup("Output byte from memory location specified in DS:(E)SI or RSI to I/O port specified in DX");
	instruction[706].binPrefixLen = 0;
	instruction[706].binOpcodeLen = 1;
	instruction[706].binOpcode = bpstrndup("\x6E", 1);
	instruction[706].opcodeOperandCount = 0;

	// Instruction Struct [707] - OUTS
	instruction[707].instructionStr = strdup("OUTS");
	instruction[707].undecodedStr = strdup("6F");
	instruction[707].dstOperandStr = strdup("DX");
	instruction[707].dstOperand = IA_DX;
	instruction[707].srcOperandStr = strdup("m16");
	instruction[707].srcOperand = IA_M16;
	instruction[707].valid64 = INVALID;
	instruction[707].compatLeg = VALID;
	instruction[707].VMX = 0;
	instruction[707].description = strdup("Output word from memory location specified in DS:(E)SI or RSI to I/O port specified in DX");
	instruction[707].binPrefixLen = 0;
	instruction[707].binOpcodeLen = 1;
	instruction[707].binOpcode = bpstrndup("\x6F", 1);
	instruction[707].opcodeOperandCount = 0;

	// Instruction Struct [708] - OUTS
	instruction[708].instructionStr = strdup("OUTS");
	instruction[708].undecodedStr = strdup("6F");
	instruction[708].dstOperandStr = strdup("DX");
	instruction[708].dstOperand = IA_DX;
	instruction[708].srcOperandStr = strdup("m32");
	instruction[708].srcOperand = IA_M32;
	instruction[708].valid64 = INVALID;
	instruction[708].compatLeg = VALID;
	instruction[708].VMX = 0;
	instruction[708].description = strdup("Output doubleword from memory location specified in DS:(E)SI or RSI to I/O port specified in DX");
	instruction[708].binPrefixLen = 0;
	instruction[708].binOpcodeLen = 1;
	instruction[708].binOpcode = bpstrndup("\x6F", 1);
	instruction[708].opcodeOperandCount = 0;

	// Instruction Struct [709] - OUTSB
	instruction[709].instructionStr = strdup("OUTSB");
	instruction[709].undecodedStr = strdup("6E");
	instruction[709].valid64 = INVALID;
	instruction[709].compatLeg = VALID;
	instruction[709].VMX = 0;
	instruction[709].description = strdup("Output byte from memory location specified in DS:(E)SI or RSI to I/O port specified in DX");
	instruction[709].binPrefixLen = 0;
	instruction[709].binOpcodeLen = 1;
	instruction[709].binOpcode = bpstrndup("\x6E", 1);
	instruction[709].opcodeOperandCount = 0;

	// Instruction Struct [710] - OUTSW
	instruction[710].instructionStr = strdup("OUTSW");
	instruction[710].undecodedStr = strdup("6F");
	instruction[710].valid64 = INVALID;
	instruction[710].compatLeg = VALID;
	instruction[710].VMX = 0;
	instruction[710].description = strdup("Output word from memory location specified in DS:(E)SI or RSI to I/O port specified in DX");
	instruction[710].binPrefixLen = 0;
	instruction[710].binOpcodeLen = 1;
	instruction[710].binOpcode = bpstrndup("\x6F", 1);
	instruction[710].opcodeOperandCount = 0;

	// Instruction Struct [711] - OUTSD
	instruction[711].instructionStr = strdup("OUTSD");
	instruction[711].undecodedStr = strdup("6F");
	instruction[711].valid64 = INVALID;
	instruction[711].compatLeg = VALID;
	instruction[711].VMX = 0;
	instruction[711].description = strdup("Output doubleword from memory location specified in DS:(E)SI or RSI to I/O port specified in DX");
	instruction[711].binPrefixLen = 0;
	instruction[711].binOpcodeLen = 1;
	instruction[711].binOpcode = bpstrndup("\x6F", 1);
	instruction[711].opcodeOperandCount = 0;

	// Instruction Struct [712] - PABSB
	instruction[712].instructionStr = strdup("PABSB");
	instruction[712].undecodedStr = strdup("0F 38 1C /r");
	instruction[712].dstOperandStr = strdup("mm1");
	instruction[712].dstOperand = IA_MM;
	instruction[712].srcOperandStr = strdup("mm2/m64");
	instruction[712].srcOperand = IA_MM_M64;
	instruction[712].valid64 = INVALID;
	instruction[712].compatLeg = VALID;
	instruction[712].VMX = 0;
	instruction[712].description = strdup("Compute the absolute value of bytes in mm2/m64 and store UNSIGNED result in mm1.");
	instruction[712].binPrefixLen = 0;
	instruction[712].binOpcodeLen = 3;
	instruction[712].binOpcode = bpstrndup("\x0F\x38\x1C", 3);
	instruction[712].opcodeOperand[0] = OA_MODRM;
	instruction[712].opcodeOperandCount = 1;

	// Instruction Struct [713] - PABSB
	instruction[713].instructionStr = strdup("PABSB");
	instruction[713].undecodedStr = strdup("66 0F 38 1C /r");
	instruction[713].dstOperandStr = strdup("xmm1");
	instruction[713].dstOperand = IA_XMM;
	instruction[713].srcOperandStr = strdup("xmm2/m128");
	instruction[713].srcOperand = IA_XMM_M128;
	instruction[713].valid64 = INVALID;
	instruction[713].compatLeg = VALID;
	instruction[713].VMX = 0;
	instruction[713].description = strdup("Compute the absolute value of bytes in xmm2/m128 and store UNSIGNED result in xmm1.");
	instruction[713].prefixStr = strdup("OPSIZEOVR");
	instruction[713].binPrefixLen = 1;
	instruction[713].binPrefix = bpstrndup("\x66", 1);
	instruction[713].binOpcodeLen = 3;
	instruction[713].binOpcode = bpstrndup("\x0F\x38\x1C", 3);
	instruction[713].opcodeOperand[0] = OA_MODRM;
	instruction[713].opcodeOperandCount = 1;

	// Instruction Struct [714] - PABSW
	instruction[714].instructionStr = strdup("PABSW");
	instruction[714].undecodedStr = strdup("0F 38 1D /r");
	instruction[714].dstOperandStr = strdup("mm1");
	instruction[714].dstOperand = IA_MM;
	instruction[714].srcOperandStr = strdup("mm2/m64");
	instruction[714].srcOperand = IA_MM_M64;
	instruction[714].valid64 = INVALID;
	instruction[714].compatLeg = VALID;
	instruction[714].VMX = 0;
	instruction[714].description = strdup("Compute the absolute value of 16- bit integers in mm2/m64 and store UNSIGNED result in mm1.");
	instruction[714].binPrefixLen = 0;
	instruction[714].binOpcodeLen = 3;
	instruction[714].binOpcode = bpstrndup("\x0F\x38\x1D", 3);
	instruction[714].opcodeOperand[0] = OA_MODRM;
	instruction[714].opcodeOperandCount = 1;

	// Instruction Struct [715] - PABSW
	instruction[715].instructionStr = strdup("PABSW");
	instruction[715].undecodedStr = strdup("66 0F 38 1D /r");
	instruction[715].dstOperandStr = strdup("xmm1");
	instruction[715].dstOperand = IA_XMM;
	instruction[715].srcOperandStr = strdup("xmm2/m128");
	instruction[715].srcOperand = IA_XMM_M128;
	instruction[715].valid64 = INVALID;
	instruction[715].compatLeg = VALID;
	instruction[715].VMX = 0;
	instruction[715].description = strdup("Compute the absolute value of 16- bit integers in xmm2/m128 and store UNSIGNED result in xmm1.");
	instruction[715].prefixStr = strdup("OPSIZEOVR");
	instruction[715].binPrefixLen = 1;
	instruction[715].binPrefix = bpstrndup("\x66", 1);
	instruction[715].binOpcodeLen = 3;
	instruction[715].binOpcode = bpstrndup("\x0F\x38\x1D", 3);
	instruction[715].opcodeOperand[0] = OA_MODRM;
	instruction[715].opcodeOperandCount = 1;

	// Instruction Struct [716] - PABSD
	instruction[716].instructionStr = strdup("PABSD");
	instruction[716].undecodedStr = strdup("0F 38 1E /r");
	instruction[716].dstOperandStr = strdup("mm1");
	instruction[716].dstOperand = IA_MM;
	instruction[716].srcOperandStr = strdup("mm2/m64");
	instruction[716].srcOperand = IA_MM_M64;
	instruction[716].valid64 = INVALID;
	instruction[716].compatLeg = VALID;
	instruction[716].VMX = 0;
	instruction[716].description = strdup("Compute the absolute value of 32- bit integers in mm2/m64 and store UNSIGNED result in mm1.");
	instruction[716].binPrefixLen = 0;
	instruction[716].binOpcodeLen = 3;
	instruction[716].binOpcode = bpstrndup("\x0F\x38\x1E", 3);
	instruction[716].opcodeOperand[0] = OA_MODRM;
	instruction[716].opcodeOperandCount = 1;

	// Instruction Struct [717] - PABSD
	instruction[717].instructionStr = strdup("PABSD");
	instruction[717].undecodedStr = strdup("66 0F 38 1E /r");
	instruction[717].dstOperandStr = strdup("xmm1");
	instruction[717].dstOperand = IA_XMM;
	instruction[717].srcOperandStr = strdup("xmm2/m128");
	instruction[717].srcOperand = IA_XMM_M128;
	instruction[717].valid64 = INVALID;
	instruction[717].compatLeg = VALID;
	instruction[717].VMX = 0;
	instruction[717].description = strdup("Compute the absolute value of 32- bit integers in xmm2/m128 and store UNSIGNED result in xmm1.");
	instruction[717].prefixStr = strdup("OPSIZEOVR");
	instruction[717].binPrefixLen = 1;
	instruction[717].binPrefix = bpstrndup("\x66", 1);
	instruction[717].binOpcodeLen = 3;
	instruction[717].binOpcode = bpstrndup("\x0F\x38\x1E", 3);
	instruction[717].opcodeOperand[0] = OA_MODRM;
	instruction[717].opcodeOperandCount = 1;

	// Instruction Struct [718] - PACKSSWB
	instruction[718].instructionStr = strdup("PACKSSWB");
	instruction[718].undecodedStr = strdup("0F 63 /r");
	instruction[718].dstOperandStr = strdup("mm1");
	instruction[718].dstOperand = IA_MM;
	instruction[718].srcOperandStr = strdup("mm2/m64");
	instruction[718].srcOperand = IA_MM_M64;
	instruction[718].valid64 = INVALID;
	instruction[718].compatLeg = VALID;
	instruction[718].VMX = 0;
	instruction[718].description = strdup("Converts 4 packed signed word integers from mm1 and from mm2/m64 into 8 packed signed byte integers in mm1 using signed saturation.");
	instruction[718].binPrefixLen = 0;
	instruction[718].binOpcodeLen = 2;
	instruction[718].binOpcode = bpstrndup("\x0F\x63", 2);
	instruction[718].opcodeOperand[0] = OA_MODRM;
	instruction[718].opcodeOperandCount = 1;

	// Instruction Struct [719] - PACKSSWB
	instruction[719].instructionStr = strdup("PACKSSWB");
	instruction[719].undecodedStr = strdup("66 0F 63 /r");
	instruction[719].dstOperandStr = strdup("xmm1");
	instruction[719].dstOperand = IA_XMM;
	instruction[719].srcOperandStr = strdup("xmm2/m128");
	instruction[719].srcOperand = IA_XMM_M128;
	instruction[719].valid64 = INVALID;
	instruction[719].compatLeg = VALID;
	instruction[719].VMX = 0;
	instruction[719].description = strdup("Converts 8 packed signed word integers from xmm1 and from xxm2/m128 into 16 packed signed byte integers in xxm1 using signed saturation.");
	instruction[719].prefixStr = strdup("OPSIZEOVR");
	instruction[719].binPrefixLen = 1;
	instruction[719].binPrefix = bpstrndup("\x66", 1);
	instruction[719].binOpcodeLen = 2;
	instruction[719].binOpcode = bpstrndup("\x0F\x63", 2);
	instruction[719].opcodeOperand[0] = OA_MODRM;
	instruction[719].opcodeOperandCount = 1;

	// Instruction Struct [720] - PACKSSDW
	instruction[720].instructionStr = strdup("PACKSSDW");
	instruction[720].undecodedStr = strdup("0F 6B /r");
	instruction[720].dstOperandStr = strdup("mm1");
	instruction[720].dstOperand = IA_MM;
	instruction[720].srcOperandStr = strdup("mm2/m64");
	instruction[720].srcOperand = IA_MM_M64;
	instruction[720].valid64 = INVALID;
	instruction[720].compatLeg = VALID;
	instruction[720].VMX = 0;
	instruction[720].description = strdup("Converts 2 packed signed doubleword integers from mm1 and from mm2/m64 into 4 packed signed word integers in mm1 using signed saturation.");
	instruction[720].binPrefixLen = 0;
	instruction[720].binOpcodeLen = 2;
	instruction[720].binOpcode = bpstrndup("\x0F\x6B", 2);
	instruction[720].opcodeOperand[0] = OA_MODRM;
	instruction[720].opcodeOperandCount = 1;

	// Instruction Struct [721] - PACKSSDW
	instruction[721].instructionStr = strdup("PACKSSDW");
	instruction[721].undecodedStr = strdup("66 0F 6B /r");
	instruction[721].dstOperandStr = strdup("xmm1");
	instruction[721].dstOperand = IA_XMM;
	instruction[721].srcOperandStr = strdup("xmm2/m128");
	instruction[721].srcOperand = IA_XMM_M128;
	instruction[721].valid64 = INVALID;
	instruction[721].compatLeg = VALID;
	instruction[721].VMX = 0;
	instruction[721].description = strdup("Converts 4 packed signed doubleword integers from xmm1 and from xxm2/m128 into 8 packed signed word integers in xxm1 using signed saturation.");
	instruction[721].prefixStr = strdup("OPSIZEOVR");
	instruction[721].binPrefixLen = 1;
	instruction[721].binPrefix = bpstrndup("\x66", 1);
	instruction[721].binOpcodeLen = 2;
	instruction[721].binOpcode = bpstrndup("\x0F\x6B", 2);
	instruction[721].opcodeOperand[0] = OA_MODRM;
	instruction[721].opcodeOperandCount = 1;

	// Instruction Struct [722] - PACKUSWB
	instruction[722].instructionStr = strdup("PACKUSWB");
	instruction[722].undecodedStr = strdup("0F 67 /r");
	instruction[722].dstOperandStr = strdup("mm");
	instruction[722].dstOperand = IA_MM;
	instruction[722].srcOperandStr = strdup("mm/m64");
	instruction[722].srcOperand = IA_MM_M64;
	instruction[722].valid64 = INVALID;
	instruction[722].compatLeg = VALID;
	instruction[722].VMX = 0;
	instruction[722].description = strdup("Converts 4 signed word integers from mm and 4 signed word integers from mm/m64 into 8 unsigned byte integers in mm using unsigned saturation.");
	instruction[722].binPrefixLen = 0;
	instruction[722].binOpcodeLen = 2;
	instruction[722].binOpcode = bpstrndup("\x0F\x67", 2);
	instruction[722].opcodeOperand[0] = OA_MODRM;
	instruction[722].opcodeOperandCount = 1;

	// Instruction Struct [723] - PACKUSWB
	instruction[723].instructionStr = strdup("PACKUSWB");
	instruction[723].undecodedStr = strdup("66 0F 67 /r");
	instruction[723].dstOperandStr = strdup("xmm1");
	instruction[723].dstOperand = IA_XMM;
	instruction[723].srcOperandStr = strdup("xmm2/m128");
	instruction[723].srcOperand = IA_XMM_M128;
	instruction[723].valid64 = INVALID;
	instruction[723].compatLeg = VALID;
	instruction[723].VMX = 0;
	instruction[723].description = strdup("Converts 8 signed word integers from xmm1 and 8 signed word integers from xmm2/m128 into 16 unsigned byte integers in xmm1 using unsigned saturation.");
	instruction[723].prefixStr = strdup("OPSIZEOVR");
	instruction[723].binPrefixLen = 1;
	instruction[723].binPrefix = bpstrndup("\x66", 1);
	instruction[723].binOpcodeLen = 2;
	instruction[723].binOpcode = bpstrndup("\x0F\x67", 2);
	instruction[723].opcodeOperand[0] = OA_MODRM;
	instruction[723].opcodeOperandCount = 1;

	// Instruction Struct [724] - PADDB
	instruction[724].instructionStr = strdup("PADDB");
	instruction[724].undecodedStr = strdup("0F FC /r");
	instruction[724].dstOperandStr = strdup("mm");
	instruction[724].dstOperand = IA_MM;
	instruction[724].srcOperandStr = strdup("mm/m64");
	instruction[724].srcOperand = IA_MM_M64;
	instruction[724].valid64 = INVALID;
	instruction[724].compatLeg = VALID;
	instruction[724].VMX = 0;
	instruction[724].description = strdup("Add packed byte integers from mm/m64 and mm.");
	instruction[724].binPrefixLen = 0;
	instruction[724].binOpcodeLen = 2;
	instruction[724].binOpcode = bpstrndup("\x0F\xFC", 2);
	instruction[724].opcodeOperand[0] = OA_MODRM;
	instruction[724].opcodeOperandCount = 1;

	// Instruction Struct [725] - PADDB
	instruction[725].instructionStr = strdup("PADDB");
	instruction[725].undecodedStr = strdup("66 0F FC /r");
	instruction[725].dstOperandStr = strdup("xmm1");
	instruction[725].dstOperand = IA_XMM;
	instruction[725].srcOperandStr = strdup("xmm2/m128");
	instruction[725].srcOperand = IA_XMM_M128;
	instruction[725].valid64 = INVALID;
	instruction[725].compatLeg = VALID;
	instruction[725].VMX = 0;
	instruction[725].description = strdup("Add packed byte integers from xmm2/m128 and xmm1.");
	instruction[725].prefixStr = strdup("OPSIZEOVR");
	instruction[725].binPrefixLen = 1;
	instruction[725].binPrefix = bpstrndup("\x66", 1);
	instruction[725].binOpcodeLen = 2;
	instruction[725].binOpcode = bpstrndup("\x0F\xFC", 2);
	instruction[725].opcodeOperand[0] = OA_MODRM;
	instruction[725].opcodeOperandCount = 1;

	// Instruction Struct [726] - PADDW
	instruction[726].instructionStr = strdup("PADDW");
	instruction[726].undecodedStr = strdup("0F FD /r");
	instruction[726].dstOperandStr = strdup("mm");
	instruction[726].dstOperand = IA_MM;
	instruction[726].srcOperandStr = strdup("mm/m64");
	instruction[726].srcOperand = IA_MM_M64;
	instruction[726].valid64 = INVALID;
	instruction[726].compatLeg = VALID;
	instruction[726].VMX = 0;
	instruction[726].description = strdup("Add packed word integers from mm/m64 and mm.");
	instruction[726].binPrefixLen = 0;
	instruction[726].binOpcodeLen = 2;
	instruction[726].binOpcode = bpstrndup("\x0F\xFD", 2);
	instruction[726].opcodeOperand[0] = OA_MODRM;
	instruction[726].opcodeOperandCount = 1;

	// Instruction Struct [727] - PADDW
	instruction[727].instructionStr = strdup("PADDW");
	instruction[727].undecodedStr = strdup("66 0F FD /r");
	instruction[727].dstOperandStr = strdup("xmm1");
	instruction[727].dstOperand = IA_XMM;
	instruction[727].srcOperandStr = strdup("xmm2/m128");
	instruction[727].srcOperand = IA_XMM_M128;
	instruction[727].valid64 = INVALID;
	instruction[727].compatLeg = VALID;
	instruction[727].VMX = 0;
	instruction[727].description = strdup("Add packed word integers from xmm2/m128 and xmm1.");
	instruction[727].prefixStr = strdup("OPSIZEOVR");
	instruction[727].binPrefixLen = 1;
	instruction[727].binPrefix = bpstrndup("\x66", 1);
	instruction[727].binOpcodeLen = 2;
	instruction[727].binOpcode = bpstrndup("\x0F\xFD", 2);
	instruction[727].opcodeOperand[0] = OA_MODRM;
	instruction[727].opcodeOperandCount = 1;

	// Instruction Struct [728] - PADDD
	instruction[728].instructionStr = strdup("PADDD");
	instruction[728].undecodedStr = strdup("0F FE /r");
	instruction[728].dstOperandStr = strdup("mm");
	instruction[728].dstOperand = IA_MM;
	instruction[728].srcOperandStr = strdup("mm/m64");
	instruction[728].srcOperand = IA_MM_M64;
	instruction[728].valid64 = INVALID;
	instruction[728].compatLeg = VALID;
	instruction[728].VMX = 0;
	instruction[728].description = strdup("Add packed doubleword integers from mm/m64 and mm.");
	instruction[728].binPrefixLen = 0;
	instruction[728].binOpcodeLen = 2;
	instruction[728].binOpcode = bpstrndup("\x0F\xFE", 2);
	instruction[728].opcodeOperand[0] = OA_MODRM;
	instruction[728].opcodeOperandCount = 1;

	// Instruction Struct [729] - PADDD
	instruction[729].instructionStr = strdup("PADDD");
	instruction[729].undecodedStr = strdup("66 0F FE /r");
	instruction[729].dstOperandStr = strdup("xmm1");
	instruction[729].dstOperand = IA_XMM;
	instruction[729].srcOperandStr = strdup("xmm2/m128");
	instruction[729].srcOperand = IA_XMM_M128;
	instruction[729].valid64 = INVALID;
	instruction[729].compatLeg = VALID;
	instruction[729].VMX = 0;
	instruction[729].description = strdup("Add packed doubleword integers from xmm2/m128 and xmm1.");
	instruction[729].prefixStr = strdup("OPSIZEOVR");
	instruction[729].binPrefixLen = 1;
	instruction[729].binPrefix = bpstrndup("\x66", 1);
	instruction[729].binOpcodeLen = 2;
	instruction[729].binOpcode = bpstrndup("\x0F\xFE", 2);
	instruction[729].opcodeOperand[0] = OA_MODRM;
	instruction[729].opcodeOperandCount = 1;

	// Instruction Struct [730] - PADDQ
	instruction[730].instructionStr = strdup("PADDQ");
	instruction[730].undecodedStr = strdup("0F D4 /r");
	instruction[730].dstOperandStr = strdup("mm1");
	instruction[730].dstOperand = IA_MM;
	instruction[730].srcOperandStr = strdup("mm2/m64");
	instruction[730].srcOperand = IA_MM_M64;
	instruction[730].valid64 = INVALID;
	instruction[730].compatLeg = VALID;
	instruction[730].VMX = 0;
	instruction[730].description = strdup("Add quadword integer mm2/m64 to mm1.");
	instruction[730].binPrefixLen = 0;
	instruction[730].binOpcodeLen = 2;
	instruction[730].binOpcode = bpstrndup("\x0F\xD4", 2);
	instruction[730].opcodeOperand[0] = OA_MODRM;
	instruction[730].opcodeOperandCount = 1;

	// Instruction Struct [731] - PADDQ
	instruction[731].instructionStr = strdup("PADDQ");
	instruction[731].undecodedStr = strdup("66 0F D4 /r");
	instruction[731].dstOperandStr = strdup("xmm1");
	instruction[731].dstOperand = IA_XMM;
	instruction[731].srcOperandStr = strdup("xmm2/m128");
	instruction[731].srcOperand = IA_XMM_M128;
	instruction[731].valid64 = INVALID;
	instruction[731].compatLeg = VALID;
	instruction[731].VMX = 0;
	instruction[731].description = strdup("Add packed quadword integers xmm2/m128 to xmm1.");
	instruction[731].prefixStr = strdup("OPSIZEOVR");
	instruction[731].binPrefixLen = 1;
	instruction[731].binPrefix = bpstrndup("\x66", 1);
	instruction[731].binOpcodeLen = 2;
	instruction[731].binOpcode = bpstrndup("\x0F\xD4", 2);
	instruction[731].opcodeOperand[0] = OA_MODRM;
	instruction[731].opcodeOperandCount = 1;

	// Instruction Struct [732] - PADDSB
	instruction[732].instructionStr = strdup("PADDSB");
	instruction[732].undecodedStr = strdup("0F EC /r");
	instruction[732].dstOperandStr = strdup("mm");
	instruction[732].dstOperand = IA_MM;
	instruction[732].srcOperandStr = strdup("mm/m64");
	instruction[732].srcOperand = IA_MM_M64;
	instruction[732].valid64 = INVALID;
	instruction[732].compatLeg = VALID;
	instruction[732].VMX = 0;
	instruction[732].description = strdup("Add packed signed byte integers from mm/m64 and mm and saturate the results.");
	instruction[732].binPrefixLen = 0;
	instruction[732].binOpcodeLen = 2;
	instruction[732].binOpcode = bpstrndup("\x0F\xEC", 2);
	instruction[732].opcodeOperand[0] = OA_MODRM;
	instruction[732].opcodeOperandCount = 1;

	// Instruction Struct [733] - PADDSB
	instruction[733].instructionStr = strdup("PADDSB");
	instruction[733].undecodedStr = strdup("66 0F EC /r");
	instruction[733].dstOperandStr = strdup("xmm1");
	instruction[733].dstOperand = IA_XMM;
	instruction[733].srcOperandStr = strdup("xmm2/m128");
	instruction[733].srcOperand = IA_XMM_M128;
	instruction[733].valid64 = INVALID;
	instruction[733].compatLeg = VALID;
	instruction[733].VMX = 0;
	instruction[733].description = strdup("Add packed signed byte integers from xmm2/m128 and xmm1 saturate the results.");
	instruction[733].prefixStr = strdup("OPSIZEOVR");
	instruction[733].binPrefixLen = 1;
	instruction[733].binPrefix = bpstrndup("\x66", 1);
	instruction[733].binOpcodeLen = 2;
	instruction[733].binOpcode = bpstrndup("\x0F\xEC", 2);
	instruction[733].opcodeOperand[0] = OA_MODRM;
	instruction[733].opcodeOperandCount = 1;

	// Instruction Struct [734] - PADDSW
	instruction[734].instructionStr = strdup("PADDSW");
	instruction[734].undecodedStr = strdup("0F ED /r");
	instruction[734].dstOperandStr = strdup("mm");
	instruction[734].dstOperand = IA_MM;
	instruction[734].srcOperandStr = strdup("mm/m64");
	instruction[734].srcOperand = IA_MM_M64;
	instruction[734].valid64 = INVALID;
	instruction[734].compatLeg = VALID;
	instruction[734].VMX = 0;
	instruction[734].description = strdup("Add packed signed word integers from mm/m64 and mm and saturate the results.");
	instruction[734].binPrefixLen = 0;
	instruction[734].binOpcodeLen = 2;
	instruction[734].binOpcode = bpstrndup("\x0F\xED", 2);
	instruction[734].opcodeOperand[0] = OA_MODRM;
	instruction[734].opcodeOperandCount = 1;

	// Instruction Struct [735] - PADDSW
	instruction[735].instructionStr = strdup("PADDSW");
	instruction[735].undecodedStr = strdup("66 0F ED /r");
	instruction[735].dstOperandStr = strdup("xmm1");
	instruction[735].dstOperand = IA_XMM;
	instruction[735].srcOperandStr = strdup("xmm2/m128");
	instruction[735].srcOperand = IA_XMM_M128;
	instruction[735].valid64 = INVALID;
	instruction[735].compatLeg = VALID;
	instruction[735].VMX = 0;
	instruction[735].description = strdup("Add packed signed word integers from xmm2/m128 and xmm1 and saturate the results.");
	instruction[735].prefixStr = strdup("OPSIZEOVR");
	instruction[735].binPrefixLen = 1;
	instruction[735].binPrefix = bpstrndup("\x66", 1);
	instruction[735].binOpcodeLen = 2;
	instruction[735].binOpcode = bpstrndup("\x0F\xED", 2);
	instruction[735].opcodeOperand[0] = OA_MODRM;
	instruction[735].opcodeOperandCount = 1;

	// Instruction Struct [736] - PADDUSB
	instruction[736].instructionStr = strdup("PADDUSB");
	instruction[736].undecodedStr = strdup("0F DC /r");
	instruction[736].dstOperandStr = strdup("mm");
	instruction[736].dstOperand = IA_MM;
	instruction[736].srcOperandStr = strdup("mm/m64");
	instruction[736].srcOperand = IA_MM_M64;
	instruction[736].valid64 = INVALID;
	instruction[736].compatLeg = VALID;
	instruction[736].VMX = 0;
	instruction[736].description = strdup("Add packed unsigned byte integers from mm/m64 and mm and saturate the results.");
	instruction[736].binPrefixLen = 0;
	instruction[736].binOpcodeLen = 2;
	instruction[736].binOpcode = bpstrndup("\x0F\xDC", 2);
	instruction[736].opcodeOperand[0] = OA_MODRM;
	instruction[736].opcodeOperandCount = 1;

	// Instruction Struct [737] - PADDUSB
	instruction[737].instructionStr = strdup("PADDUSB");
	instruction[737].undecodedStr = strdup("66 0F DC /r");
	instruction[737].dstOperandStr = strdup("xmm1");
	instruction[737].dstOperand = IA_XMM;
	instruction[737].srcOperandStr = strdup("xmm2/m128");
	instruction[737].srcOperand = IA_XMM_M128;
	instruction[737].valid64 = INVALID;
	instruction[737].compatLeg = VALID;
	instruction[737].VMX = 0;
	instruction[737].description = strdup("Add packed unsigned byte integers from xmm2/m128 and xmm1 saturate the results.");
	instruction[737].prefixStr = strdup("OPSIZEOVR");
	instruction[737].binPrefixLen = 1;
	instruction[737].binPrefix = bpstrndup("\x66", 1);
	instruction[737].binOpcodeLen = 2;
	instruction[737].binOpcode = bpstrndup("\x0F\xDC", 2);
	instruction[737].opcodeOperand[0] = OA_MODRM;
	instruction[737].opcodeOperandCount = 1;

	// Instruction Struct [738] - PADDUSW
	instruction[738].instructionStr = strdup("PADDUSW");
	instruction[738].undecodedStr = strdup("0F DD /r");
	instruction[738].dstOperandStr = strdup("mm");
	instruction[738].dstOperand = IA_MM;
	instruction[738].srcOperandStr = strdup("mm/m64");
	instruction[738].srcOperand = IA_MM_M64;
	instruction[738].valid64 = INVALID;
	instruction[738].compatLeg = VALID;
	instruction[738].VMX = 0;
	instruction[738].description = strdup("Add packed unsigned word integers from mm/m64 and mm and saturate the results.");
	instruction[738].binPrefixLen = 0;
	instruction[738].binOpcodeLen = 2;
	instruction[738].binOpcode = bpstrndup("\x0F\xDD", 2);
	instruction[738].opcodeOperand[0] = OA_MODRM;
	instruction[738].opcodeOperandCount = 1;

	// Instruction Struct [739] - PADDUSW
	instruction[739].instructionStr = strdup("PADDUSW");
	instruction[739].undecodedStr = strdup("66 0F DD /r");
	instruction[739].dstOperandStr = strdup("xmm1");
	instruction[739].dstOperand = IA_XMM;
	instruction[739].srcOperandStr = strdup("xmm2/m128");
	instruction[739].srcOperand = IA_XMM_M128;
	instruction[739].valid64 = INVALID;
	instruction[739].compatLeg = VALID;
	instruction[739].VMX = 0;
	instruction[739].description = strdup("Add packed unsigned word integers from xmm2/m128 to xmm1 and saturate the results.");
	instruction[739].prefixStr = strdup("OPSIZEOVR");
	instruction[739].binPrefixLen = 1;
	instruction[739].binPrefix = bpstrndup("\x66", 1);
	instruction[739].binOpcodeLen = 2;
	instruction[739].binOpcode = bpstrndup("\x0F\xDD", 2);
	instruction[739].opcodeOperand[0] = OA_MODRM;
	instruction[739].opcodeOperandCount = 1;

	// Instruction Struct [740] - PALIGNR
	instruction[740].instructionStr = strdup("PALIGNR");
	instruction[740].undecodedStr = strdup("0F 3A 0F");
	instruction[740].dstOperandStr = strdup("mm1");
	instruction[740].dstOperand = IA_MM;
	instruction[740].srcOperandStr = strdup("mm2/m64");
	instruction[740].srcOperand = IA_MM_M64;
	instruction[740].thirdOperandStr = strdup("imm8");
	instruction[740].thirdOperand = IA_IMM8;
	instruction[740].valid64 = INVALID;
	instruction[740].compatLeg = VALID;
	instruction[740].VMX = 0;
	instruction[740].description = strdup("Concatenate destination and source operands, extract byte-aligned result shifted to the right by constant into mm1.");
	instruction[740].binPrefixLen = 0;
	instruction[740].binOpcodeLen = 3;
	instruction[740].binOpcode = bpstrndup("\x0F\x3A\x0F", 3);
	instruction[740].opcodeOperandCount = 0;

	// Instruction Struct [741] - PALIGNR
	instruction[741].instructionStr = strdup("PALIGNR");
	instruction[741].undecodedStr = strdup("66 0F 3A 0F");
	instruction[741].dstOperandStr = strdup("xmm1");
	instruction[741].dstOperand = IA_XMM;
	instruction[741].srcOperandStr = strdup("xmm2/m128");
	instruction[741].srcOperand = IA_XMM_M128;
	instruction[741].thirdOperandStr = strdup("imm8");
	instruction[741].thirdOperand = IA_IMM8;
	instruction[741].valid64 = INVALID;
	instruction[741].compatLeg = VALID;
	instruction[741].VMX = 0;
	instruction[741].description = strdup("Concatenate destination and source operands, extract byte-aligned result shifted to the right by constant into xmm1");
	instruction[741].prefixStr = strdup("OPSIZEOVR");
	instruction[741].binPrefixLen = 1;
	instruction[741].binPrefix = bpstrndup("\x66", 1);
	instruction[741].binOpcodeLen = 3;
	instruction[741].binOpcode = bpstrndup("\x0F\x3A\x0F", 3);
	instruction[741].opcodeOperandCount = 0;

	// Instruction Struct [742] - PAND
	instruction[742].instructionStr = strdup("PAND");
	instruction[742].undecodedStr = strdup("0F DB /r");
	instruction[742].dstOperandStr = strdup("mm");
	instruction[742].dstOperand = IA_MM;
	instruction[742].srcOperandStr = strdup("mm/m64");
	instruction[742].srcOperand = IA_MM_M64;
	instruction[742].valid64 = INVALID;
	instruction[742].compatLeg = VALID;
	instruction[742].VMX = 0;
	instruction[742].description = strdup("Bitwise AND mm/m64 and mm.");
	instruction[742].binPrefixLen = 0;
	instruction[742].binOpcodeLen = 2;
	instruction[742].binOpcode = bpstrndup("\x0F\xDB", 2);
	instruction[742].opcodeOperand[0] = OA_MODRM;
	instruction[742].opcodeOperandCount = 1;

	// Instruction Struct [743] - PAND
	instruction[743].instructionStr = strdup("PAND");
	instruction[743].undecodedStr = strdup("66 0F DB /r");
	instruction[743].dstOperandStr = strdup("xmm1");
	instruction[743].dstOperand = IA_XMM;
	instruction[743].srcOperandStr = strdup("xmm2/m128");
	instruction[743].srcOperand = IA_XMM_M128;
	instruction[743].valid64 = INVALID;
	instruction[743].compatLeg = VALID;
	instruction[743].VMX = 0;
	instruction[743].description = strdup("Bitwise AND of xmm2/m128 and xmm1.");
	instruction[743].prefixStr = strdup("OPSIZEOVR");
	instruction[743].binPrefixLen = 1;
	instruction[743].binPrefix = bpstrndup("\x66", 1);
	instruction[743].binOpcodeLen = 2;
	instruction[743].binOpcode = bpstrndup("\x0F\xDB", 2);
	instruction[743].opcodeOperand[0] = OA_MODRM;
	instruction[743].opcodeOperandCount = 1;

	// Instruction Struct [744] - PANDN
	instruction[744].instructionStr = strdup("PANDN");
	instruction[744].undecodedStr = strdup("0F DF /r");
	instruction[744].dstOperandStr = strdup("mm");
	instruction[744].dstOperand = IA_MM;
	instruction[744].srcOperandStr = strdup("mm/m64");
	instruction[744].srcOperand = IA_MM_M64;
	instruction[744].valid64 = INVALID;
	instruction[744].compatLeg = VALID;
	instruction[744].VMX = 0;
	instruction[744].description = strdup("Bitwise AND NOT of mm/m64 and mm.");
	instruction[744].binPrefixLen = 0;
	instruction[744].binOpcodeLen = 2;
	instruction[744].binOpcode = bpstrndup("\x0F\xDF", 2);
	instruction[744].opcodeOperand[0] = OA_MODRM;
	instruction[744].opcodeOperandCount = 1;

	// Instruction Struct [745] - PANDN
	instruction[745].instructionStr = strdup("PANDN");
	instruction[745].undecodedStr = strdup("66 0F DF /r");
	instruction[745].dstOperandStr = strdup("xmm1");
	instruction[745].dstOperand = IA_XMM;
	instruction[745].srcOperandStr = strdup("xmm2/m128");
	instruction[745].srcOperand = IA_XMM_M128;
	instruction[745].valid64 = INVALID;
	instruction[745].compatLeg = VALID;
	instruction[745].VMX = 0;
	instruction[745].description = strdup("ValidBitwise AND NOT of xmm2/m128 and xmm1.");
	instruction[745].prefixStr = strdup("OPSIZEOVR");
	instruction[745].binPrefixLen = 1;
	instruction[745].binPrefix = bpstrndup("\x66", 1);
	instruction[745].binOpcodeLen = 2;
	instruction[745].binOpcode = bpstrndup("\x0F\xDF", 2);
	instruction[745].opcodeOperand[0] = OA_MODRM;
	instruction[745].opcodeOperandCount = 1;

	// Instruction Struct [746] - PAUSE
	instruction[746].instructionStr = strdup("PAUSE");
	instruction[746].undecodedStr = strdup("F3 90");
	instruction[746].valid64 = INVALID;
	instruction[746].compatLeg = VALID;
	instruction[746].VMX = 0;
	instruction[746].description = strdup("Gives hint to processor that improves performance of spin-wait loops.");
	instruction[746].prefixStr = strdup("REP");
	instruction[746].binPrefixLen = 1;
	instruction[746].binPrefix = bpstrndup("\xF3", 1);
	instruction[746].binOpcodeLen = 1;
	instruction[746].binOpcode = bpstrndup("\x90", 1);
	instruction[746].opcodeOperandCount = 0;

	// Instruction Struct [747] - PAVGB
	instruction[747].instructionStr = strdup("PAVGB");
	instruction[747].undecodedStr = strdup("0F E0 /r");
	instruction[747].dstOperandStr = strdup("mm1");
	instruction[747].dstOperand = IA_MM;
	instruction[747].srcOperandStr = strdup("mm2/m64");
	instruction[747].srcOperand = IA_MM_M64;
	instruction[747].valid64 = INVALID;
	instruction[747].compatLeg = VALID;
	instruction[747].VMX = 0;
	instruction[747].description = strdup("Average packed unsigned byte integers from mm2/m64 and mm1 with rounding.");
	instruction[747].binPrefixLen = 0;
	instruction[747].binOpcodeLen = 2;
	instruction[747].binOpcode = bpstrndup("\x0F\xE0", 2);
	instruction[747].opcodeOperand[0] = OA_MODRM;
	instruction[747].opcodeOperandCount = 1;

	// Instruction Struct [748] - PAVGB
	instruction[748].instructionStr = strdup("PAVGB");
	instruction[748].undecodedStr = strdup("66 0F E0 /r");
	instruction[748].dstOperandStr = strdup("xmm1");
	instruction[748].dstOperand = IA_XMM;
	instruction[748].srcOperandStr = strdup("xmm2/m128");
	instruction[748].srcOperand = IA_XMM_M128;
	instruction[748].valid64 = INVALID;
	instruction[748].compatLeg = VALID;
	instruction[748].VMX = 0;
	instruction[748].description = strdup("Average packed unsigned byte. (Previously was 66 0F E0, /r ; removed comma suggestive of typo)");
	instruction[748].prefixStr = strdup("OPSIZEOVR");
	instruction[748].binPrefixLen = 1;
	instruction[748].binPrefix = bpstrndup("\x66", 1);
	instruction[748].binOpcodeLen = 2;
	instruction[748].binOpcode = bpstrndup("\x0F\xE0", 2);
	instruction[748].opcodeOperand[0] = OA_MODRM;
	instruction[748].opcodeOperandCount = 1;

	// Instruction Struct [749] - PAVGW
	instruction[749].instructionStr = strdup("PAVGW");
	instruction[749].undecodedStr = strdup("0F E3 /r");
	instruction[749].dstOperandStr = strdup("mm1");
	instruction[749].dstOperand = IA_MM;
	instruction[749].srcOperandStr = strdup("mm2/m64");
	instruction[749].srcOperand = IA_MM_M64;
	instruction[749].valid64 = INVALID;
	instruction[749].compatLeg = VALID;
	instruction[749].VMX = 0;
	instruction[749].description = strdup("Average packed unsigned word integers from mm2/m64 and mm1 with rounding.");
	instruction[749].binPrefixLen = 0;
	instruction[749].binOpcodeLen = 2;
	instruction[749].binOpcode = bpstrndup("\x0F\xE3", 2);
	instruction[749].opcodeOperand[0] = OA_MODRM;
	instruction[749].opcodeOperandCount = 1;

	// Instruction Struct [750] - PAVGW
	instruction[750].instructionStr = strdup("PAVGW");
	instruction[750].undecodedStr = strdup("66 0F E3 /r");
	instruction[750].dstOperandStr = strdup("xmm1");
	instruction[750].dstOperand = IA_XMM;
	instruction[750].srcOperandStr = strdup("xmm2/m128");
	instruction[750].srcOperand = IA_XMM_M128;
	instruction[750].valid64 = INVALID;
	instruction[750].compatLeg = VALID;
	instruction[750].VMX = 0;
	instruction[750].description = strdup("Average packed unsigned word integers from xmm2/m128 and xmm1 with rounding.");
	instruction[750].prefixStr = strdup("OPSIZEOVR");
	instruction[750].binPrefixLen = 1;
	instruction[750].binPrefix = bpstrndup("\x66", 1);
	instruction[750].binOpcodeLen = 2;
	instruction[750].binOpcode = bpstrndup("\x0F\xE3", 2);
	instruction[750].opcodeOperand[0] = OA_MODRM;
	instruction[750].opcodeOperandCount = 1;

	// Instruction Struct [751] - PCMPEQB
	instruction[751].instructionStr = strdup("PCMPEQB");
	instruction[751].undecodedStr = strdup("0F 74 /r");
	instruction[751].dstOperandStr = strdup("mm");
	instruction[751].dstOperand = IA_MM;
	instruction[751].srcOperandStr = strdup("mm/m64");
	instruction[751].srcOperand = IA_MM_M64;
	instruction[751].valid64 = INVALID;
	instruction[751].compatLeg = VALID;
	instruction[751].VMX = 0;
	instruction[751].description = strdup("Compare packed bytes in mm/m64 and mm for equality.");
	instruction[751].binPrefixLen = 0;
	instruction[751].binOpcodeLen = 2;
	instruction[751].binOpcode = bpstrndup("\x0F\x74", 2);
	instruction[751].opcodeOperand[0] = OA_MODRM;
	instruction[751].opcodeOperandCount = 1;

	// Instruction Struct [752] - PCMPEQB
	instruction[752].instructionStr = strdup("PCMPEQB");
	instruction[752].undecodedStr = strdup("66 0F 74 /r");
	instruction[752].dstOperandStr = strdup("xmm1");
	instruction[752].dstOperand = IA_XMM;
	instruction[752].srcOperandStr = strdup("xmm2/m128");
	instruction[752].srcOperand = IA_XMM_M128;
	instruction[752].valid64 = INVALID;
	instruction[752].compatLeg = VALID;
	instruction[752].VMX = 0;
	instruction[752].description = strdup("Compare packed bytes in xmm2/m128 and xmm1 for equality.");
	instruction[752].prefixStr = strdup("OPSIZEOVR");
	instruction[752].binPrefixLen = 1;
	instruction[752].binPrefix = bpstrndup("\x66", 1);
	instruction[752].binOpcodeLen = 2;
	instruction[752].binOpcode = bpstrndup("\x0F\x74", 2);
	instruction[752].opcodeOperand[0] = OA_MODRM;
	instruction[752].opcodeOperandCount = 1;

	// Instruction Struct [753] - PCMPEQW
	instruction[753].instructionStr = strdup("PCMPEQW");
	instruction[753].undecodedStr = strdup("0F 75 /r");
	instruction[753].dstOperandStr = strdup("mm");
	instruction[753].dstOperand = IA_MM;
	instruction[753].srcOperandStr = strdup("mm/m64");
	instruction[753].srcOperand = IA_MM_M64;
	instruction[753].valid64 = INVALID;
	instruction[753].compatLeg = VALID;
	instruction[753].VMX = 0;
	instruction[753].description = strdup("Compare packed words in mm/m64 and mm for equality.");
	instruction[753].binPrefixLen = 0;
	instruction[753].binOpcodeLen = 2;
	instruction[753].binOpcode = bpstrndup("\x0F\x75", 2);
	instruction[753].opcodeOperand[0] = OA_MODRM;
	instruction[753].opcodeOperandCount = 1;

	// Instruction Struct [754] - PCMPEQW
	instruction[754].instructionStr = strdup("PCMPEQW");
	instruction[754].undecodedStr = strdup("66 0F 75 /r");
	instruction[754].dstOperandStr = strdup("xmm1");
	instruction[754].dstOperand = IA_XMM;
	instruction[754].srcOperandStr = strdup("xmm2/m128");
	instruction[754].srcOperand = IA_XMM_M128;
	instruction[754].valid64 = INVALID;
	instruction[754].compatLeg = VALID;
	instruction[754].VMX = 0;
	instruction[754].description = strdup("Compare packed words in xmm2/m128 and xmm1 for equality.");
	instruction[754].prefixStr = strdup("OPSIZEOVR");
	instruction[754].binPrefixLen = 1;
	instruction[754].binPrefix = bpstrndup("\x66", 1);
	instruction[754].binOpcodeLen = 2;
	instruction[754].binOpcode = bpstrndup("\x0F\x75", 2);
	instruction[754].opcodeOperand[0] = OA_MODRM;
	instruction[754].opcodeOperandCount = 1;

	// Instruction Struct [755] - PCMPEQD
	instruction[755].instructionStr = strdup("PCMPEQD");
	instruction[755].undecodedStr = strdup("0F 76 /r");
	instruction[755].dstOperandStr = strdup("mm");
	instruction[755].dstOperand = IA_MM;
	instruction[755].srcOperandStr = strdup("mm/m64");
	instruction[755].srcOperand = IA_MM_M64;
	instruction[755].valid64 = INVALID;
	instruction[755].compatLeg = VALID;
	instruction[755].VMX = 0;
	instruction[755].description = strdup("Compare packed doublewords in mm/m64 and mm for equality.");
	instruction[755].binPrefixLen = 0;
	instruction[755].binOpcodeLen = 2;
	instruction[755].binOpcode = bpstrndup("\x0F\x76", 2);
	instruction[755].opcodeOperand[0] = OA_MODRM;
	instruction[755].opcodeOperandCount = 1;

	// Instruction Struct [756] - PCMPEQD
	instruction[756].instructionStr = strdup("PCMPEQD");
	instruction[756].undecodedStr = strdup("66 0F 76 /r");
	instruction[756].dstOperandStr = strdup("xmm1");
	instruction[756].dstOperand = IA_XMM;
	instruction[756].srcOperandStr = strdup("xmm2/m128");
	instruction[756].srcOperand = IA_XMM_M128;
	instruction[756].valid64 = INVALID;
	instruction[756].compatLeg = VALID;
	instruction[756].VMX = 0;
	instruction[756].description = strdup("Compare packed doublewords in xmm2/m128 and xmm1 for equality.");
	instruction[756].prefixStr = strdup("OPSIZEOVR");
	instruction[756].binPrefixLen = 1;
	instruction[756].binPrefix = bpstrndup("\x66", 1);
	instruction[756].binOpcodeLen = 2;
	instruction[756].binOpcode = bpstrndup("\x0F\x76", 2);
	instruction[756].opcodeOperand[0] = OA_MODRM;
	instruction[756].opcodeOperandCount = 1;

	// Instruction Struct [757] - PCMPGTB
	instruction[757].instructionStr = strdup("PCMPGTB");
	instruction[757].undecodedStr = strdup("0F 64 /r");
	instruction[757].dstOperandStr = strdup("mm");
	instruction[757].dstOperand = IA_MM;
	instruction[757].srcOperandStr = strdup("mm/m64");
	instruction[757].srcOperand = IA_MM_M64;
	instruction[757].valid64 = INVALID;
	instruction[757].compatLeg = VALID;
	instruction[757].VMX = 0;
	instruction[757].description = strdup("Compare packed signed byte");
	instruction[757].binPrefixLen = 0;
	instruction[757].binOpcodeLen = 2;
	instruction[757].binOpcode = bpstrndup("\x0F\x64", 2);
	instruction[757].opcodeOperand[0] = OA_MODRM;
	instruction[757].opcodeOperandCount = 1;

	// Instruction Struct [758] - PCMPGTB
	instruction[758].instructionStr = strdup("PCMPGTB");
	instruction[758].undecodedStr = strdup("66 0F 64 /r");
	instruction[758].dstOperandStr = strdup("xmm1");
	instruction[758].dstOperand = IA_XMM;
	instruction[758].srcOperandStr = strdup("xmm2/m128");
	instruction[758].srcOperand = IA_XMM_M128;
	instruction[758].valid64 = INVALID;
	instruction[758].compatLeg = VALID;
	instruction[758].VMX = 0;
	instruction[758].description = strdup("Compare packed signed byte integers in xmm1 and xmm2/m128 for greater than.");
	instruction[758].prefixStr = strdup("OPSIZEOVR");
	instruction[758].binPrefixLen = 1;
	instruction[758].binPrefix = bpstrndup("\x66", 1);
	instruction[758].binOpcodeLen = 2;
	instruction[758].binOpcode = bpstrndup("\x0F\x64", 2);
	instruction[758].opcodeOperand[0] = OA_MODRM;
	instruction[758].opcodeOperandCount = 1;

	// Instruction Struct [759] - PCMPGTW
	instruction[759].instructionStr = strdup("PCMPGTW");
	instruction[759].undecodedStr = strdup("0F 65 /r");
	instruction[759].dstOperandStr = strdup("mm");
	instruction[759].dstOperand = IA_MM;
	instruction[759].srcOperandStr = strdup("mm/m64");
	instruction[759].srcOperand = IA_MM_M64;
	instruction[759].valid64 = INVALID;
	instruction[759].compatLeg = VALID;
	instruction[759].VMX = 0;
	instruction[759].description = strdup("Compare packed signed word integers in mm and mm/m64 for greater than.");
	instruction[759].binPrefixLen = 0;
	instruction[759].binOpcodeLen = 2;
	instruction[759].binOpcode = bpstrndup("\x0F\x65", 2);
	instruction[759].opcodeOperand[0] = OA_MODRM;
	instruction[759].opcodeOperandCount = 1;

	// Instruction Struct [760] - PCMPGTW
	instruction[760].instructionStr = strdup("PCMPGTW");
	instruction[760].undecodedStr = strdup("66 0F 65 /r");
	instruction[760].dstOperandStr = strdup("xmm1");
	instruction[760].dstOperand = IA_XMM;
	instruction[760].srcOperandStr = strdup("xmm2/m128");
	instruction[760].srcOperand = IA_XMM_M128;
	instruction[760].valid64 = INVALID;
	instruction[760].compatLeg = VALID;
	instruction[760].VMX = 0;
	instruction[760].description = strdup("Compare packed signed word integers in xmm1 and xmm2/m128 for greater than.");
	instruction[760].prefixStr = strdup("OPSIZEOVR");
	instruction[760].binPrefixLen = 1;
	instruction[760].binPrefix = bpstrndup("\x66", 1);
	instruction[760].binOpcodeLen = 2;
	instruction[760].binOpcode = bpstrndup("\x0F\x65", 2);
	instruction[760].opcodeOperand[0] = OA_MODRM;
	instruction[760].opcodeOperandCount = 1;

	// Instruction Struct [761] - PCMPGTD
	instruction[761].instructionStr = strdup("PCMPGTD");
	instruction[761].undecodedStr = strdup("0F 66 /r");
	instruction[761].dstOperandStr = strdup("mm");
	instruction[761].dstOperand = IA_MM;
	instruction[761].srcOperandStr = strdup("mm/m64");
	instruction[761].srcOperand = IA_MM_M64;
	instruction[761].valid64 = INVALID;
	instruction[761].compatLeg = VALID;
	instruction[761].VMX = 0;
	instruction[761].description = strdup("Compare packed signed doubleword integers in mm and mm/m64 for greater than.");
	instruction[761].binPrefixLen = 0;
	instruction[761].binOpcodeLen = 2;
	instruction[761].binOpcode = bpstrndup("\x0F\x66", 2);
	instruction[761].opcodeOperand[0] = OA_MODRM;
	instruction[761].opcodeOperandCount = 1;

	// Instruction Struct [762] - PCMPGTD
	instruction[762].instructionStr = strdup("PCMPGTD");
	instruction[762].undecodedStr = strdup("66 0F 66 /r");
	instruction[762].dstOperandStr = strdup("xmm1");
	instruction[762].dstOperand = IA_XMM;
	instruction[762].srcOperandStr = strdup("xmm2/m128");
	instruction[762].srcOperand = IA_XMM_M128;
	instruction[762].valid64 = INVALID;
	instruction[762].compatLeg = VALID;
	instruction[762].VMX = 0;
	instruction[762].description = strdup("Compare packed signed doubleword integers in xmm1 and xmm2/m128 for greater than.");
	instruction[762].prefixStr = strdup("OPSIZEOVR");
	instruction[762].binPrefixLen = 1;
	instruction[762].binPrefix = bpstrndup("\x66", 1);
	instruction[762].binOpcodeLen = 2;
	instruction[762].binOpcode = bpstrndup("\x0F\x66", 2);
	instruction[762].opcodeOperand[0] = OA_MODRM;
	instruction[762].opcodeOperandCount = 1;

	// Instruction Struct [763] - PEXTRW
	instruction[763].instructionStr = strdup("PEXTRW");
	instruction[763].undecodedStr = strdup("0F C5 /r ib");
	instruction[763].dstOperandStr = strdup("r32");
	instruction[763].dstOperand = IA_R32;
	instruction[763].srcOperandStr = strdup("mm");
	instruction[763].srcOperand = IA_MM;
	instruction[763].thirdOperandStr = strdup("imm8");
	instruction[763].thirdOperand = IA_IMM8;
	instruction[763].valid64 = INVALID;
	instruction[763].compatLeg = VALID;
	instruction[763].VMX = 0;
	instruction[763].description = strdup("Extract the word specified by imm8 from mm and move it to r32, bits 15-0. Zero-extend the result.");
	instruction[763].binPrefixLen = 0;
	instruction[763].binOpcodeLen = 2;
	instruction[763].binOpcode = bpstrndup("\x0F\xC5", 2);
	instruction[763].opcodeOperand[0] = OA_MODRM;
	instruction[763].opcodeOperand[1] = OA_IB;
	instruction[763].opcodeOperandCount = 2;

	// Instruction Struct [764] - PEXTRW
	instruction[764].instructionStr = strdup("PEXTRW");
	instruction[764].undecodedStr = strdup("66 0F C5 /r ib");
	instruction[764].dstOperandStr = strdup("r32");
	instruction[764].dstOperand = IA_R32;
	instruction[764].srcOperandStr = strdup("xmm");
	instruction[764].srcOperand = IA_XMM;
	instruction[764].thirdOperandStr = strdup("imm8");
	instruction[764].thirdOperand = IA_IMM8;
	instruction[764].valid64 = INVALID;
	instruction[764].compatLeg = VALID;
	instruction[764].VMX = 0;
	instruction[764].description = strdup("Extract the word specified by imm8 from xmm and move it to r32, bits 15-0. Zero-extend the result.");
	instruction[764].prefixStr = strdup("OPSIZEOVR");
	instruction[764].binPrefixLen = 1;
	instruction[764].binPrefix = bpstrndup("\x66", 1);
	instruction[764].binOpcodeLen = 2;
	instruction[764].binOpcode = bpstrndup("\x0F\xC5", 2);
	instruction[764].opcodeOperand[0] = OA_MODRM;
	instruction[764].opcodeOperand[1] = OA_IB;
	instruction[764].opcodeOperandCount = 2;

	// Instruction Struct [765] - PHADDW
	instruction[765].instructionStr = strdup("PHADDW");
	instruction[765].undecodedStr = strdup("0F 38 01 /r");
	instruction[765].dstOperandStr = strdup("mm1");
	instruction[765].dstOperand = IA_MM;
	instruction[765].srcOperandStr = strdup("mm2/m64");
	instruction[765].srcOperand = IA_MM_M64;
	instruction[765].valid64 = INVALID;
	instruction[765].compatLeg = VALID;
	instruction[765].VMX = 0;
	instruction[765].description = strdup("Add 16-bit signed integers horizontally, pack to MM1.");
	instruction[765].binPrefixLen = 0;
	instruction[765].binOpcodeLen = 3;
	instruction[765].binOpcode = bpstrndup("\x0F\x38\x01", 3);
	instruction[765].opcodeOperand[0] = OA_MODRM;
	instruction[765].opcodeOperandCount = 1;

	// Instruction Struct [766] - PHADDW
	instruction[766].instructionStr = strdup("PHADDW");
	instruction[766].undecodedStr = strdup("66 0F 38 01 /r");
	instruction[766].dstOperandStr = strdup("xmm1");
	instruction[766].dstOperand = IA_XMM;
	instruction[766].srcOperandStr = strdup("xmm2/m128");
	instruction[766].srcOperand = IA_XMM_M128;
	instruction[766].valid64 = INVALID;
	instruction[766].compatLeg = VALID;
	instruction[766].VMX = 0;
	instruction[766].description = strdup("Add 16-bit signed integers horizontally, pack to XMM1.");
	instruction[766].prefixStr = strdup("OPSIZEOVR");
	instruction[766].binPrefixLen = 1;
	instruction[766].binPrefix = bpstrndup("\x66", 1);
	instruction[766].binOpcodeLen = 3;
	instruction[766].binOpcode = bpstrndup("\x0F\x38\x01", 3);
	instruction[766].opcodeOperand[0] = OA_MODRM;
	instruction[766].opcodeOperandCount = 1;

	// Instruction Struct [767] - PHADDD
	instruction[767].instructionStr = strdup("PHADDD");
	instruction[767].undecodedStr = strdup("0F 38 02 /r");
	instruction[767].dstOperandStr = strdup("mm1");
	instruction[767].dstOperand = IA_MM;
	instruction[767].srcOperandStr = strdup("mm2/m64");
	instruction[767].srcOperand = IA_MM_M64;
	instruction[767].valid64 = INVALID;
	instruction[767].compatLeg = VALID;
	instruction[767].VMX = 0;
	instruction[767].description = strdup("Add 32-bit signed integers horizontally, pack to MM1.");
	instruction[767].binPrefixLen = 0;
	instruction[767].binOpcodeLen = 3;
	instruction[767].binOpcode = bpstrndup("\x0F\x38\x02", 3);
	instruction[767].opcodeOperand[0] = OA_MODRM;
	instruction[767].opcodeOperandCount = 1;

	// Instruction Struct [768] - PHADDD
	instruction[768].instructionStr = strdup("PHADDD");
	instruction[768].undecodedStr = strdup("66 0F 38 02 /r");
	instruction[768].dstOperandStr = strdup("xmm1");
	instruction[768].dstOperand = IA_XMM;
	instruction[768].srcOperandStr = strdup("xmm2/m128");
	instruction[768].srcOperand = IA_XMM_M128;
	instruction[768].valid64 = INVALID;
	instruction[768].compatLeg = VALID;
	instruction[768].VMX = 0;
	instruction[768].description = strdup("Add 32-bit signed integers horizontally, pack to XMM1.");
	instruction[768].prefixStr = strdup("OPSIZEOVR");
	instruction[768].binPrefixLen = 1;
	instruction[768].binPrefix = bpstrndup("\x66", 1);
	instruction[768].binOpcodeLen = 3;
	instruction[768].binOpcode = bpstrndup("\x0F\x38\x02", 3);
	instruction[768].opcodeOperand[0] = OA_MODRM;
	instruction[768].opcodeOperandCount = 1;

	// Instruction Struct [769] - PHADDSW
	instruction[769].instructionStr = strdup("PHADDSW");
	instruction[769].undecodedStr = strdup("0F 38 03 /r");
	instruction[769].dstOperandStr = strdup("mm1");
	instruction[769].dstOperand = IA_MM;
	instruction[769].srcOperandStr = strdup("mm2/m64");
	instruction[769].srcOperand = IA_MM_M64;
	instruction[769].valid64 = INVALID;
	instruction[769].compatLeg = VALID;
	instruction[769].VMX = 0;
	instruction[769].description = strdup("Add 16-bit signed integers horizontally, pack saturated integers to MM1.");
	instruction[769].binPrefixLen = 0;
	instruction[769].binOpcodeLen = 3;
	instruction[769].binOpcode = bpstrndup("\x0F\x38\x03", 3);
	instruction[769].opcodeOperand[0] = OA_MODRM;
	instruction[769].opcodeOperandCount = 1;

	// Instruction Struct [770] - PHADDSW
	instruction[770].instructionStr = strdup("PHADDSW");
	instruction[770].undecodedStr = strdup("66 0F 38 03 /r");
	instruction[770].dstOperandStr = strdup("xmm1");
	instruction[770].dstOperand = IA_XMM;
	instruction[770].srcOperandStr = strdup("xmm2/m128");
	instruction[770].srcOperand = IA_XMM_M128;
	instruction[770].valid64 = INVALID;
	instruction[770].compatLeg = VALID;
	instruction[770].VMX = 0;
	instruction[770].description = strdup("Add 16-bit signed integers horizontally, pack saturated integers to XMM1.");
	instruction[770].prefixStr = strdup("OPSIZEOVR");
	instruction[770].binPrefixLen = 1;
	instruction[770].binPrefix = bpstrndup("\x66", 1);
	instruction[770].binOpcodeLen = 3;
	instruction[770].binOpcode = bpstrndup("\x0F\x38\x03", 3);
	instruction[770].opcodeOperand[0] = OA_MODRM;
	instruction[770].opcodeOperandCount = 1;

	// Instruction Struct [771] - PHSUBW
	instruction[771].instructionStr = strdup("PHSUBW");
	instruction[771].undecodedStr = strdup("0F 38 05 /r");
	instruction[771].dstOperandStr = strdup("mm1");
	instruction[771].dstOperand = IA_MM;
	instruction[771].srcOperandStr = strdup("mm2/m64");
	instruction[771].srcOperand = IA_MM_M64;
	instruction[771].valid64 = INVALID;
	instruction[771].compatLeg = VALID;
	instruction[771].VMX = 0;
	instruction[771].description = strdup("Subtract 16-bit signed integers horizontally, pack to MM1.");
	instruction[771].binPrefixLen = 0;
	instruction[771].binOpcodeLen = 3;
	instruction[771].binOpcode = bpstrndup("\x0F\x38\x05", 3);
	instruction[771].opcodeOperand[0] = OA_MODRM;
	instruction[771].opcodeOperandCount = 1;

	// Instruction Struct [772] - PHSUBW
	instruction[772].instructionStr = strdup("PHSUBW");
	instruction[772].undecodedStr = strdup("66 0F 38 05 /r");
	instruction[772].dstOperandStr = strdup("xmm1");
	instruction[772].dstOperand = IA_XMM;
	instruction[772].srcOperandStr = strdup("xmm2/m128");
	instruction[772].srcOperand = IA_XMM_M128;
	instruction[772].valid64 = INVALID;
	instruction[772].compatLeg = VALID;
	instruction[772].VMX = 0;
	instruction[772].description = strdup("Subtract 16-bit signed integers horizontally, pack to XMM1.");
	instruction[772].prefixStr = strdup("OPSIZEOVR");
	instruction[772].binPrefixLen = 1;
	instruction[772].binPrefix = bpstrndup("\x66", 1);
	instruction[772].binOpcodeLen = 3;
	instruction[772].binOpcode = bpstrndup("\x0F\x38\x05", 3);
	instruction[772].opcodeOperand[0] = OA_MODRM;
	instruction[772].opcodeOperandCount = 1;

	// Instruction Struct [773] - PHSUBD
	instruction[773].instructionStr = strdup("PHSUBD");
	instruction[773].undecodedStr = strdup("0F 38 06 /r");
	instruction[773].dstOperandStr = strdup("mm1");
	instruction[773].dstOperand = IA_MM;
	instruction[773].srcOperandStr = strdup("mm2/m64");
	instruction[773].srcOperand = IA_MM_M64;
	instruction[773].valid64 = INVALID;
	instruction[773].compatLeg = VALID;
	instruction[773].VMX = 0;
	instruction[773].description = strdup("Subtract 32-bit signed integers horizontally, pack to MM1.");
	instruction[773].binPrefixLen = 0;
	instruction[773].binOpcodeLen = 3;
	instruction[773].binOpcode = bpstrndup("\x0F\x38\x06", 3);
	instruction[773].opcodeOperand[0] = OA_MODRM;
	instruction[773].opcodeOperandCount = 1;

	// Instruction Struct [774] - PHSUBD
	instruction[774].instructionStr = strdup("PHSUBD");
	instruction[774].undecodedStr = strdup("66 0F 38 06 /r");
	instruction[774].dstOperandStr = strdup("xmm1");
	instruction[774].dstOperand = IA_XMM;
	instruction[774].srcOperandStr = strdup("xmm2/m128");
	instruction[774].srcOperand = IA_XMM_M128;
	instruction[774].valid64 = INVALID;
	instruction[774].compatLeg = VALID;
	instruction[774].VMX = 0;
	instruction[774].description = strdup("Subtract 32-bit signed");
	instruction[774].prefixStr = strdup("OPSIZEOVR");
	instruction[774].binPrefixLen = 1;
	instruction[774].binPrefix = bpstrndup("\x66", 1);
	instruction[774].binOpcodeLen = 3;
	instruction[774].binOpcode = bpstrndup("\x0F\x38\x06", 3);
	instruction[774].opcodeOperand[0] = OA_MODRM;
	instruction[774].opcodeOperandCount = 1;

	// Instruction Struct [775] - PHSUBSW
	instruction[775].instructionStr = strdup("PHSUBSW");
	instruction[775].undecodedStr = strdup("0F 38 07 /r");
	instruction[775].dstOperandStr = strdup("mm1");
	instruction[775].dstOperand = IA_MM;
	instruction[775].srcOperandStr = strdup("mm2/m64");
	instruction[775].srcOperand = IA_MM_M64;
	instruction[775].valid64 = INVALID;
	instruction[775].compatLeg = INVALID;
	instruction[775].VMX = 0;
	instruction[775].description = strdup("Subtract 16-bit signed integer horizontally, pack saturated integers to MM1.");
	instruction[775].binPrefixLen = 0;
	instruction[775].binOpcodeLen = 3;
	instruction[775].binOpcode = bpstrndup("\x0F\x38\x07", 3);
	instruction[775].opcodeOperand[0] = OA_MODRM;
	instruction[775].opcodeOperandCount = 1;

	// Instruction Struct [776] - PHSUBSW
	instruction[776].instructionStr = strdup("PHSUBSW");
	instruction[776].undecodedStr = strdup("66 0F 38 07 /r");
	instruction[776].dstOperandStr = strdup("xmm1");
	instruction[776].dstOperand = IA_XMM;
	instruction[776].srcOperandStr = strdup("xmm2/m128");
	instruction[776].srcOperand = IA_XMM_M128;
	instruction[776].valid64 = INVALID;
	instruction[776].compatLeg = VALID;
	instruction[776].VMX = 0;
	instruction[776].description = strdup("Subtract 16-bit signed integer horizontally, pack saturated integers to XMM1");
	instruction[776].prefixStr = strdup("OPSIZEOVR");
	instruction[776].binPrefixLen = 1;
	instruction[776].binPrefix = bpstrndup("\x66", 1);
	instruction[776].binOpcodeLen = 3;
	instruction[776].binOpcode = bpstrndup("\x0F\x38\x07", 3);
	instruction[776].opcodeOperand[0] = OA_MODRM;
	instruction[776].opcodeOperandCount = 1;

	// Instruction Struct [777] - PINSRW
	instruction[777].instructionStr = strdup("PINSRW");
	instruction[777].undecodedStr = strdup("0F C4 /r ib");
	instruction[777].dstOperandStr = strdup("mm");
	instruction[777].dstOperand = IA_MM;
	instruction[777].srcOperandStr = strdup("r32/m16");
	instruction[777].srcOperand = IA_R32_M16;
	instruction[777].thirdOperandStr = strdup("imm8");
	instruction[777].thirdOperand = IA_IMM8;
	instruction[777].valid64 = INVALID;
	instruction[777].compatLeg = VALID;
	instruction[777].VMX = 0;
	instruction[777].description = strdup("Insert the low word from r32 or from m16 into mm at the word position specified by imm8");
	instruction[777].binPrefixLen = 0;
	instruction[777].binOpcodeLen = 2;
	instruction[777].binOpcode = bpstrndup("\x0F\xC4", 2);
	instruction[777].opcodeOperand[0] = OA_MODRM;
	instruction[777].opcodeOperand[1] = OA_IB;
	instruction[777].opcodeOperandCount = 2;

	// Instruction Struct [778] - PINSRW
	instruction[778].instructionStr = strdup("PINSRW");
	instruction[778].undecodedStr = strdup("66 0F C4 /r ib");
	instruction[778].dstOperandStr = strdup("xmm");
	instruction[778].dstOperand = IA_XMM;
	instruction[778].srcOperandStr = strdup("r32/m16");
	instruction[778].srcOperand = IA_R32_M16;
	instruction[778].thirdOperandStr = strdup("imm8");
	instruction[778].thirdOperand = IA_IMM8;
	instruction[778].valid64 = INVALID;
	instruction[778].compatLeg = VALID;
	instruction[778].VMX = 0;
	instruction[778].description = strdup("Move the low word of r32 or from m16 into xmm at the word position specified by imm8.");
	instruction[778].prefixStr = strdup("OPSIZEOVR");
	instruction[778].binPrefixLen = 1;
	instruction[778].binPrefix = bpstrndup("\x66", 1);
	instruction[778].binOpcodeLen = 2;
	instruction[778].binOpcode = bpstrndup("\x0F\xC4", 2);
	instruction[778].opcodeOperand[0] = OA_MODRM;
	instruction[778].opcodeOperand[1] = OA_IB;
	instruction[778].opcodeOperandCount = 2;

	// Instruction Struct [779] - PMADDUBSW
	instruction[779].instructionStr = strdup("PMADDUBSW");
	instruction[779].undecodedStr = strdup("0F 38 04 /r");
	instruction[779].dstOperandStr = strdup("mm1");
	instruction[779].dstOperand = IA_MM;
	instruction[779].srcOperandStr = strdup("mm2/m64");
	instruction[779].srcOperand = IA_MM_M64;
	instruction[779].valid64 = INVALID;
	instruction[779].compatLeg = VALID;
	instruction[779].VMX = 0;
	instruction[779].description = strdup("Multiply signed and unsigned bytes, add horizontal pair of signed words, pack saturated signed-words to MM1.");
	instruction[779].binPrefixLen = 0;
	instruction[779].binOpcodeLen = 3;
	instruction[779].binOpcode = bpstrndup("\x0F\x38\x04", 3);
	instruction[779].opcodeOperand[0] = OA_MODRM;
	instruction[779].opcodeOperandCount = 1;

	// Instruction Struct [780] - PMADDUBSW
	instruction[780].instructionStr = strdup("PMADDUBSW");
	instruction[780].undecodedStr = strdup("66 0F 38 04 /r");
	instruction[780].dstOperandStr = strdup("xmm1");
	instruction[780].dstOperand = IA_XMM;
	instruction[780].srcOperandStr = strdup("xmm2/m128");
	instruction[780].srcOperand = IA_XMM_M128;
	instruction[780].valid64 = INVALID;
	instruction[780].compatLeg = VALID;
	instruction[780].VMX = 0;
	instruction[780].description = strdup("Multiply signed and unsigned bytes, add horizontal pair of signed words, pack saturated signed-words to XMM1.");
	instruction[780].prefixStr = strdup("OPSIZEOVR");
	instruction[780].binPrefixLen = 1;
	instruction[780].binPrefix = bpstrndup("\x66", 1);
	instruction[780].binOpcodeLen = 3;
	instruction[780].binOpcode = bpstrndup("\x0F\x38\x04", 3);
	instruction[780].opcodeOperand[0] = OA_MODRM;
	instruction[780].opcodeOperandCount = 1;

	// Instruction Struct [781] - PMADDWD
	instruction[781].instructionStr = strdup("PMADDWD");
	instruction[781].undecodedStr = strdup("0F F5 /r");
	instruction[781].dstOperandStr = strdup("mm");
	instruction[781].dstOperand = IA_MM;
	instruction[781].srcOperandStr = strdup("mm/m64");
	instruction[781].srcOperand = IA_MM_M64;
	instruction[781].valid64 = INVALID;
	instruction[781].compatLeg = VALID;
	instruction[781].VMX = 0;
	instruction[781].description = strdup("Multiply the packed words in mm by the packed words in mm/m64, add adjacent doubleword results, and store in mm.");
	instruction[781].binPrefixLen = 0;
	instruction[781].binOpcodeLen = 2;
	instruction[781].binOpcode = bpstrndup("\x0F\xF5", 2);
	instruction[781].opcodeOperand[0] = OA_MODRM;
	instruction[781].opcodeOperandCount = 1;

	// Instruction Struct [782] - PMADDWD
	instruction[782].instructionStr = strdup("PMADDWD");
	instruction[782].undecodedStr = strdup("66 0F F5 /r");
	instruction[782].dstOperandStr = strdup("xmm1");
	instruction[782].dstOperand = IA_XMM;
	instruction[782].srcOperandStr = strdup("xmm2/m128");
	instruction[782].srcOperand = IA_XMM_M128;
	instruction[782].valid64 = INVALID;
	instruction[782].compatLeg = VALID;
	instruction[782].VMX = 0;
	instruction[782].description = strdup("Multiply the packed word integers in xmm1 by the packed word integers in xmm2/m128, add adjacent doubleword results, and store in xmm1.");
	instruction[782].prefixStr = strdup("OPSIZEOVR");
	instruction[782].binPrefixLen = 1;
	instruction[782].binPrefix = bpstrndup("\x66", 1);
	instruction[782].binOpcodeLen = 2;
	instruction[782].binOpcode = bpstrndup("\x0F\xF5", 2);
	instruction[782].opcodeOperand[0] = OA_MODRM;
	instruction[782].opcodeOperandCount = 1;

	// Instruction Struct [783] - PMAXSW
	instruction[783].instructionStr = strdup("PMAXSW");
	instruction[783].undecodedStr = strdup("0F EE /r");
	instruction[783].dstOperandStr = strdup("mm1");
	instruction[783].dstOperand = IA_MM;
	instruction[783].srcOperandStr = strdup("mm2/m64");
	instruction[783].srcOperand = IA_MM_M64;
	instruction[783].valid64 = INVALID;
	instruction[783].compatLeg = VALID;
	instruction[783].VMX = 0;
	instruction[783].description = strdup("Compare signed word integers in mm2/m64 and mm1 and return maximum values.");
	instruction[783].binPrefixLen = 0;
	instruction[783].binOpcodeLen = 2;
	instruction[783].binOpcode = bpstrndup("\x0F\xEE", 2);
	instruction[783].opcodeOperand[0] = OA_MODRM;
	instruction[783].opcodeOperandCount = 1;

	// Instruction Struct [784] - PMAXSW
	instruction[784].instructionStr = strdup("PMAXSW");
	instruction[784].undecodedStr = strdup("66 0F EE /r");
	instruction[784].dstOperandStr = strdup("xmm1");
	instruction[784].dstOperand = IA_XMM;
	instruction[784].srcOperandStr = strdup("xmm2/m128");
	instruction[784].srcOperand = IA_XMM_M128;
	instruction[784].valid64 = INVALID;
	instruction[784].compatLeg = VALID;
	instruction[784].VMX = 0;
	instruction[784].description = strdup("Compare signed word integers in xmm2/m128 and xmm1 and return maximum values.");
	instruction[784].prefixStr = strdup("OPSIZEOVR");
	instruction[784].binPrefixLen = 1;
	instruction[784].binPrefix = bpstrndup("\x66", 1);
	instruction[784].binOpcodeLen = 2;
	instruction[784].binOpcode = bpstrndup("\x0F\xEE", 2);
	instruction[784].opcodeOperand[0] = OA_MODRM;
	instruction[784].opcodeOperandCount = 1;

	// Instruction Struct [785] - PMAXUB
	instruction[785].instructionStr = strdup("PMAXUB");
	instruction[785].undecodedStr = strdup("0F DE /r");
	instruction[785].dstOperandStr = strdup("mm1");
	instruction[785].dstOperand = IA_MM;
	instruction[785].srcOperandStr = strdup("mm2/m64");
	instruction[785].srcOperand = IA_MM_M64;
	instruction[785].valid64 = INVALID;
	instruction[785].compatLeg = VALID;
	instruction[785].VMX = 0;
	instruction[785].description = strdup("Compare unsigned byte integers in mm2/m64 and mm1 and returns maximum values.");
	instruction[785].binPrefixLen = 0;
	instruction[785].binOpcodeLen = 2;
	instruction[785].binOpcode = bpstrndup("\x0F\xDE", 2);
	instruction[785].opcodeOperand[0] = OA_MODRM;
	instruction[785].opcodeOperandCount = 1;

	// Instruction Struct [786] - PMAXUB
	instruction[786].instructionStr = strdup("PMAXUB");
	instruction[786].undecodedStr = strdup("66 0F DE /r");
	instruction[786].dstOperandStr = strdup("xmm1");
	instruction[786].dstOperand = IA_XMM;
	instruction[786].srcOperandStr = strdup("xmm2/m128");
	instruction[786].srcOperand = IA_XMM_M128;
	instruction[786].valid64 = INVALID;
	instruction[786].compatLeg = VALID;
	instruction[786].VMX = 0;
	instruction[786].description = strdup("Compare unsigned byte integers in xmm2/m128 and xmm1 and returns maximum values.");
	instruction[786].prefixStr = strdup("OPSIZEOVR");
	instruction[786].binPrefixLen = 1;
	instruction[786].binPrefix = bpstrndup("\x66", 1);
	instruction[786].binOpcodeLen = 2;
	instruction[786].binOpcode = bpstrndup("\x0F\xDE", 2);
	instruction[786].opcodeOperand[0] = OA_MODRM;
	instruction[786].opcodeOperandCount = 1;

	// Instruction Struct [787] - PMINSW
	instruction[787].instructionStr = strdup("PMINSW");
	instruction[787].undecodedStr = strdup("0F EA /r");
	instruction[787].dstOperandStr = strdup("mm1");
	instruction[787].dstOperand = IA_MM;
	instruction[787].srcOperandStr = strdup("mm2/m64");
	instruction[787].srcOperand = IA_MM_M64;
	instruction[787].valid64 = INVALID;
	instruction[787].compatLeg = VALID;
	instruction[787].VMX = 0;
	instruction[787].description = strdup("Compare signed word integers in mm2/m64 and mm1 and return minimum values.");
	instruction[787].binPrefixLen = 0;
	instruction[787].binOpcodeLen = 2;
	instruction[787].binOpcode = bpstrndup("\x0F\xEA", 2);
	instruction[787].opcodeOperand[0] = OA_MODRM;
	instruction[787].opcodeOperandCount = 1;

	// Instruction Struct [788] - PMINSW
	instruction[788].instructionStr = strdup("PMINSW");
	instruction[788].undecodedStr = strdup("66 0F EA /r");
	instruction[788].dstOperandStr = strdup("xmm1");
	instruction[788].dstOperand = IA_XMM;
	instruction[788].srcOperandStr = strdup("xmm2/m128");
	instruction[788].srcOperand = IA_XMM_M128;
	instruction[788].valid64 = INVALID;
	instruction[788].compatLeg = VALID;
	instruction[788].VMX = 0;
	instruction[788].description = strdup("Compare signed word integers in xmm2/m128 and xmm1 and return minimum values.");
	instruction[788].prefixStr = strdup("OPSIZEOVR");
	instruction[788].binPrefixLen = 1;
	instruction[788].binPrefix = bpstrndup("\x66", 1);
	instruction[788].binOpcodeLen = 2;
	instruction[788].binOpcode = bpstrndup("\x0F\xEA", 2);
	instruction[788].opcodeOperand[0] = OA_MODRM;
	instruction[788].opcodeOperandCount = 1;

	// Instruction Struct [789] - PMINUB
	instruction[789].instructionStr = strdup("PMINUB");
	instruction[789].undecodedStr = strdup("0F DA /r");
	instruction[789].dstOperandStr = strdup("mm1");
	instruction[789].dstOperand = IA_MM;
	instruction[789].srcOperandStr = strdup("mm2/m64");
	instruction[789].srcOperand = IA_MM_M64;
	instruction[789].valid64 = INVALID;
	instruction[789].compatLeg = VALID;
	instruction[789].VMX = 0;
	instruction[789].description = strdup("Compare unsigned byte integers in mm2/m64 and mm1 and returns minimum values.");
	instruction[789].binPrefixLen = 0;
	instruction[789].binOpcodeLen = 2;
	instruction[789].binOpcode = bpstrndup("\x0F\xDA", 2);
	instruction[789].opcodeOperand[0] = OA_MODRM;
	instruction[789].opcodeOperandCount = 1;

	// Instruction Struct [790] - PMINUB
	instruction[790].instructionStr = strdup("PMINUB");
	instruction[790].undecodedStr = strdup("66 0F DA /r");
	instruction[790].dstOperandStr = strdup("xmm1");
	instruction[790].dstOperand = IA_XMM;
	instruction[790].srcOperandStr = strdup("xmm2/m128");
	instruction[790].srcOperand = IA_XMM_M128;
	instruction[790].valid64 = INVALID;
	instruction[790].compatLeg = VALID;
	instruction[790].VMX = 0;
	instruction[790].description = strdup("Compare unsigned byte integers in xmm2/m128 and xmm1 and returns minimum values.");
	instruction[790].prefixStr = strdup("OPSIZEOVR");
	instruction[790].binPrefixLen = 1;
	instruction[790].binPrefix = bpstrndup("\x66", 1);
	instruction[790].binOpcodeLen = 2;
	instruction[790].binOpcode = bpstrndup("\x0F\xDA", 2);
	instruction[790].opcodeOperand[0] = OA_MODRM;
	instruction[790].opcodeOperandCount = 1;

	// Instruction Struct [791] - PMOVMSKB
	instruction[791].instructionStr = strdup("PMOVMSKB");
	instruction[791].undecodedStr = strdup("0F D7 /r");
	instruction[791].dstOperandStr = strdup("r32");
	instruction[791].dstOperand = IA_R32;
	instruction[791].srcOperandStr = strdup("mm");
	instruction[791].srcOperand = IA_MM;
	instruction[791].valid64 = INVALID;
	instruction[791].compatLeg = VALID;
	instruction[791].VMX = 0;
	instruction[791].description = strdup("Move a byte mask of mm to r32.");
	instruction[791].binPrefixLen = 0;
	instruction[791].binOpcodeLen = 2;
	instruction[791].binOpcode = bpstrndup("\x0F\xD7", 2);
	instruction[791].opcodeOperand[0] = OA_MODRM;
	instruction[791].opcodeOperandCount = 1;

	// Instruction Struct [792] - PMOVMSKB
	instruction[792].instructionStr = strdup("PMOVMSKB");
	instruction[792].undecodedStr = strdup("66 0F D7 /r");
	instruction[792].dstOperandStr = strdup("r32");
	instruction[792].dstOperand = IA_R32;
	instruction[792].srcOperandStr = strdup("xmm");
	instruction[792].srcOperand = IA_XMM;
	instruction[792].valid64 = INVALID;
	instruction[792].compatLeg = VALID;
	instruction[792].VMX = 0;
	instruction[792].description = strdup("Move a byte mask of xmm to r32.");
	instruction[792].prefixStr = strdup("OPSIZEOVR");
	instruction[792].binPrefixLen = 1;
	instruction[792].binPrefix = bpstrndup("\x66", 1);
	instruction[792].binOpcodeLen = 2;
	instruction[792].binOpcode = bpstrndup("\x0F\xD7", 2);
	instruction[792].opcodeOperand[0] = OA_MODRM;
	instruction[792].opcodeOperandCount = 1;

	// Instruction Struct [793] - PMULHRSW
	instruction[793].instructionStr = strdup("PMULHRSW");
	instruction[793].undecodedStr = strdup("0F 38 0B /r");
	instruction[793].dstOperandStr = strdup("mm1");
	instruction[793].dstOperand = IA_MM;
	instruction[793].srcOperandStr = strdup("mm2/m64");
	instruction[793].srcOperand = IA_MM_M64;
	instruction[793].valid64 = INVALID;
	instruction[793].compatLeg = VALID;
	instruction[793].VMX = 0;
	instruction[793].description = strdup("Multiply 16-bit signed words, scale and round signed doublewords, pack high 16 bits to MM1.");
	instruction[793].binPrefixLen = 0;
	instruction[793].binOpcodeLen = 3;
	instruction[793].binOpcode = bpstrndup("\x0F\x38\x0B", 3);
	instruction[793].opcodeOperand[0] = OA_MODRM;
	instruction[793].opcodeOperandCount = 1;

	// Instruction Struct [794] - PMULHRSW
	instruction[794].instructionStr = strdup("PMULHRSW");
	instruction[794].undecodedStr = strdup("66 0F 38 0B /r");
	instruction[794].dstOperandStr = strdup("xmm1");
	instruction[794].dstOperand = IA_XMM;
	instruction[794].srcOperandStr = strdup("xmm2/m128");
	instruction[794].srcOperand = IA_XMM_M128;
	instruction[794].valid64 = INVALID;
	instruction[794].compatLeg = VALID;
	instruction[794].VMX = 0;
	instruction[794].description = strdup("Multiply 16-bit signed words, scale and round signed doublewords, pack high 16 bits to XMM1.");
	instruction[794].prefixStr = strdup("OPSIZEOVR");
	instruction[794].binPrefixLen = 1;
	instruction[794].binPrefix = bpstrndup("\x66", 1);
	instruction[794].binOpcodeLen = 3;
	instruction[794].binOpcode = bpstrndup("\x0F\x38\x0B", 3);
	instruction[794].opcodeOperand[0] = OA_MODRM;
	instruction[794].opcodeOperandCount = 1;

	// Instruction Struct [795] - PMULHUW
	instruction[795].instructionStr = strdup("PMULHUW");
	instruction[795].undecodedStr = strdup("0F E4 /r");
	instruction[795].dstOperandStr = strdup("mm1");
	instruction[795].dstOperand = IA_MM;
	instruction[795].srcOperandStr = strdup("mm2/m64");
	instruction[795].srcOperand = IA_MM_M64;
	instruction[795].valid64 = INVALID;
	instruction[795].compatLeg = VALID;
	instruction[795].VMX = 0;
	instruction[795].description = strdup("Multiply the packed unsigned word integers in mm1 register and mm2/m64, and store the high 16 bits of the results in mm1.");
	instruction[795].binPrefixLen = 0;
	instruction[795].binOpcodeLen = 2;
	instruction[795].binOpcode = bpstrndup("\x0F\xE4", 2);
	instruction[795].opcodeOperand[0] = OA_MODRM;
	instruction[795].opcodeOperandCount = 1;

	// Instruction Struct [796] - PMULHUW
	instruction[796].instructionStr = strdup("PMULHUW");
	instruction[796].undecodedStr = strdup("66 0F E4 /r");
	instruction[796].dstOperandStr = strdup("xmm1");
	instruction[796].dstOperand = IA_XMM;
	instruction[796].srcOperandStr = strdup("xmm2/m128");
	instruction[796].srcOperand = IA_XMM_M128;
	instruction[796].valid64 = INVALID;
	instruction[796].compatLeg = VALID;
	instruction[796].VMX = 0;
	instruction[796].description = strdup("Multiply the packed unsigned word integers in xmm1 and xmm2/m128, and store the high 16 bits of the results in xmm1");
	instruction[796].prefixStr = strdup("OPSIZEOVR");
	instruction[796].binPrefixLen = 1;
	instruction[796].binPrefix = bpstrndup("\x66", 1);
	instruction[796].binOpcodeLen = 2;
	instruction[796].binOpcode = bpstrndup("\x0F\xE4", 2);
	instruction[796].opcodeOperand[0] = OA_MODRM;
	instruction[796].opcodeOperandCount = 1;

	// Instruction Struct [797] - PMULHW
	instruction[797].instructionStr = strdup("PMULHW");
	instruction[797].undecodedStr = strdup("0F E5 /r");
	instruction[797].dstOperandStr = strdup("mm");
	instruction[797].dstOperand = IA_MM;
	instruction[797].srcOperandStr = strdup("mm/m64");
	instruction[797].srcOperand = IA_MM_M64;
	instruction[797].valid64 = INVALID;
	instruction[797].compatLeg = VALID;
	instruction[797].VMX = 0;
	instruction[797].description = strdup("Multiply the packed signed word integers in mm1 register and mm2/m64, and store the high 16 bits of the results in mm1.");
	instruction[797].binPrefixLen = 0;
	instruction[797].binOpcodeLen = 2;
	instruction[797].binOpcode = bpstrndup("\x0F\xE5", 2);
	instruction[797].opcodeOperand[0] = OA_MODRM;
	instruction[797].opcodeOperandCount = 1;

	// Instruction Struct [798] - PMULHW
	instruction[798].instructionStr = strdup("PMULHW");
	instruction[798].undecodedStr = strdup("66 0F E5 /r");
	instruction[798].dstOperandStr = strdup("xmm1");
	instruction[798].dstOperand = IA_XMM;
	instruction[798].srcOperandStr = strdup("xmm2/m128");
	instruction[798].srcOperand = IA_XMM_M128;
	instruction[798].valid64 = INVALID;
	instruction[798].compatLeg = VALID;
	instruction[798].VMX = 0;
	instruction[798].description = strdup("Multiply the packed signed word integers in xmm1 and xmm2/m128, and store the high 16 bits of the results in xmm1.");
	instruction[798].prefixStr = strdup("OPSIZEOVR");
	instruction[798].binPrefixLen = 1;
	instruction[798].binPrefix = bpstrndup("\x66", 1);
	instruction[798].binOpcodeLen = 2;
	instruction[798].binOpcode = bpstrndup("\x0F\xE5", 2);
	instruction[798].opcodeOperand[0] = OA_MODRM;
	instruction[798].opcodeOperandCount = 1;

	// Instruction Struct [799] - PMULLW
	instruction[799].instructionStr = strdup("PMULLW");
	instruction[799].undecodedStr = strdup("0F D5 /r");
	instruction[799].dstOperandStr = strdup("mm");
	instruction[799].dstOperand = IA_MM;
	instruction[799].srcOperandStr = strdup("mm/m64");
	instruction[799].srcOperand = IA_MM_M64;
	instruction[799].valid64 = INVALID;
	instruction[799].compatLeg = VALID;
	instruction[799].VMX = 0;
	instruction[799].description = strdup("Multiply the packed signed word integers in mm1 register and mm2/m64, and store the low 16 bits of the results in mm1.");
	instruction[799].binPrefixLen = 0;
	instruction[799].binOpcodeLen = 2;
	instruction[799].binOpcode = bpstrndup("\x0F\xD5", 2);
	instruction[799].opcodeOperand[0] = OA_MODRM;
	instruction[799].opcodeOperandCount = 1;

	// Instruction Struct [800] - PMULLW
	instruction[800].instructionStr = strdup("PMULLW");
	instruction[800].undecodedStr = strdup("66 0F D5 /r");
	instruction[800].dstOperandStr = strdup("xmm1");
	instruction[800].dstOperand = IA_XMM;
	instruction[800].srcOperandStr = strdup("xmm2/m128");
	instruction[800].srcOperand = IA_XMM_M128;
	instruction[800].valid64 = INVALID;
	instruction[800].compatLeg = VALID;
	instruction[800].VMX = 0;
	instruction[800].description = strdup("Multiply the packed signed word integers in xmm1 and xmm2/m128, and store the low 16 bits of the results in xmm1.");
	instruction[800].prefixStr = strdup("OPSIZEOVR");
	instruction[800].binPrefixLen = 1;
	instruction[800].binPrefix = bpstrndup("\x66", 1);
	instruction[800].binOpcodeLen = 2;
	instruction[800].binOpcode = bpstrndup("\x0F\xD5", 2);
	instruction[800].opcodeOperand[0] = OA_MODRM;
	instruction[800].opcodeOperandCount = 1;

	// Instruction Struct [801] - PMULUDQ
	instruction[801].instructionStr = strdup("PMULUDQ");
	instruction[801].undecodedStr = strdup("0F F4 /r");
	instruction[801].dstOperandStr = strdup("mm1");
	instruction[801].dstOperand = IA_MM;
	instruction[801].srcOperandStr = strdup("mm2/m64");
	instruction[801].srcOperand = IA_MM_M64;
	instruction[801].valid64 = INVALID;
	instruction[801].compatLeg = VALID;
	instruction[801].VMX = 0;
	instruction[801].description = strdup("Multiply unsigned doubleword integer in mm1 by unsigned doubleword integer in mm2/m64, and store the quadword result in mm1.");
	instruction[801].binPrefixLen = 0;
	instruction[801].binOpcodeLen = 2;
	instruction[801].binOpcode = bpstrndup("\x0F\xF4", 2);
	instruction[801].opcodeOperand[0] = OA_MODRM;
	instruction[801].opcodeOperandCount = 1;

	// Instruction Struct [802] - PMULUDQ
	instruction[802].instructionStr = strdup("PMULUDQ");
	instruction[802].undecodedStr = strdup("66 0F F4 /r");
	instruction[802].dstOperandStr = strdup("xmm1");
	instruction[802].dstOperand = IA_XMM;
	instruction[802].srcOperandStr = strdup("xmm2/m128");
	instruction[802].srcOperand = IA_XMM_M128;
	instruction[802].valid64 = INVALID;
	instruction[802].compatLeg = VALID;
	instruction[802].VMX = 0;
	instruction[802].description = strdup("Multiply packed unsigned");
	instruction[802].prefixStr = strdup("OPSIZEOVR");
	instruction[802].binPrefixLen = 1;
	instruction[802].binPrefix = bpstrndup("\x66", 1);
	instruction[802].binOpcodeLen = 2;
	instruction[802].binOpcode = bpstrndup("\x0F\xF4", 2);
	instruction[802].opcodeOperand[0] = OA_MODRM;
	instruction[802].opcodeOperandCount = 1;

	// Instruction Struct [803] - POP
	instruction[803].instructionStr = strdup("POP");
	instruction[803].undecodedStr = strdup("8F /0");
	instruction[803].dstOperandStr = strdup("r/m16");
	instruction[803].dstOperand = IA_RM16;
	instruction[803].valid64 = INVALID;
	instruction[803].compatLeg = VALID;
	instruction[803].VMX = 0;
	instruction[803].description = strdup("Pop top of stack into m16; increment stack pointer.");
	instruction[803].binPrefixLen = 0;
	instruction[803].binOpcodeLen = 1;
	instruction[803].binOpcode = bpstrndup("\x8F", 1);
	instruction[803].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[803].opcodeOperandCount = 1;

	// Instruction Struct [804] - POP
	instruction[804].instructionStr = strdup("POP");
	instruction[804].undecodedStr = strdup("8F /0");
	instruction[804].dstOperandStr = strdup("r/m32");
	instruction[804].dstOperand = IA_RM32;
	instruction[804].valid64 = INVALID;
	instruction[804].compatLeg = VALID;
	instruction[804].VMX = 0;
	instruction[804].description = strdup("Pop top of stack into m32; increment stack pointer.");
	instruction[804].binPrefixLen = 0;
	instruction[804].binOpcodeLen = 1;
	instruction[804].binOpcode = bpstrndup("\x8F", 1);
	instruction[804].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[804].opcodeOperandCount = 1;

	// Instruction Struct [805] - POP
	instruction[805].instructionStr = strdup("POP");
	instruction[805].undecodedStr = strdup("8F /0");
	instruction[805].dstOperandStr = strdup("r/m64");
	instruction[805].dstOperand = IA_RM64;
	instruction[805].valid64 = INVALID;
	instruction[805].compatLeg = INVALID;
	instruction[805].VMX = 0;
	instruction[805].description = strdup("Pop top of stack into m64; increment stack pointer. Cannot encode 32-bit operand size.");
	instruction[805].binPrefixLen = 0;
	instruction[805].binOpcodeLen = 1;
	instruction[805].binOpcode = bpstrndup("\x8F", 1);
	instruction[805].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[805].opcodeOperandCount = 1;

	// Instruction Struct [806] - POP
	instruction[806].instructionStr = strdup("POP");
	instruction[806].undecodedStr = strdup("58 +rw");
	instruction[806].dstOperandStr = strdup("r16");
	instruction[806].dstOperand = IA_R16;
	instruction[806].valid64 = INVALID;
	instruction[806].compatLeg = VALID;
	instruction[806].VMX = 0;
	instruction[806].description = strdup("Pop top of stack into r16; increment stack");
	instruction[806].binPrefixLen = 0;
	instruction[806].binOpcodeLen = 1;
	instruction[806].binOpcode = bpstrndup("\x58", 1);
	instruction[806].opcodeOperand[0] = OA_RW;
	instruction[806].opcodeOperandCount = 1;

	// Instruction Struct [807] - POP
	instruction[807].instructionStr = strdup("POP");
	instruction[807].undecodedStr = strdup("58 +rd");
	instruction[807].dstOperandStr = strdup("r32");
	instruction[807].dstOperand = IA_R32;
	instruction[807].valid64 = INVALID;
	instruction[807].compatLeg = VALID;
	instruction[807].VMX = 0;
	instruction[807].description = strdup("Pop top of stack into r32; increment stack");
	instruction[807].binPrefixLen = 0;
	instruction[807].binOpcodeLen = 1;
	instruction[807].binOpcode = bpstrndup("\x58", 1);
	instruction[807].opcodeOperand[0] = OA_RD;
	instruction[807].opcodeOperandCount = 1;

	// Instruction Struct [808] - POP
	instruction[808].instructionStr = strdup("POP");
	instruction[808].undecodedStr = strdup("58 +rd");
	instruction[808].dstOperandStr = strdup("r64");
	instruction[808].dstOperand = IA_R64;
	instruction[808].valid64 = INVALID;
	instruction[808].compatLeg = INVALID;
	instruction[808].VMX = 0;
	instruction[808].description = strdup("Pop top of stack into r64; increment stack");
	instruction[808].binPrefixLen = 0;
	instruction[808].binOpcodeLen = 1;
	instruction[808].binOpcode = bpstrndup("\x58", 1);
	instruction[808].opcodeOperand[0] = OA_RD;
	instruction[808].opcodeOperandCount = 1;

	// Instruction Struct [809] - POP DS
	instruction[809].instructionStr = strdup("POP DS");
	instruction[809].undecodedStr = strdup("1F");
	instruction[809].valid64 = INVALID;
	instruction[809].compatLeg = VALID;
	instruction[809].VMX = 0;
	instruction[809].description = strdup("Pop top of stack into DS; increment stack pointer.");
	instruction[809].binPrefixLen = 0;
	instruction[809].binOpcodeLen = 1;
	instruction[809].binOpcode = bpstrndup("\x1F", 1);
	instruction[809].opcodeOperandCount = 0;

	// Instruction Struct [810] - POP
	instruction[810].instructionStr = strdup("POP");
	instruction[810].undecodedStr = strdup("07");
	instruction[810].dstOperandStr = strdup("ES");
	instruction[810].dstOperand = IA_ES;
	instruction[810].valid64 = INVALID;
	instruction[810].compatLeg = VALID;
	instruction[810].VMX = 0;
	instruction[810].description = strdup("Pop top of stack into ES; increment stack pointer.");
	instruction[810].binPrefixLen = 0;
	instruction[810].binOpcodeLen = 1;
	instruction[810].binOpcode = bpstrndup("\x07", 1);
	instruction[810].opcodeOperandCount = 0;

	// Instruction Struct [811] - POP
	instruction[811].instructionStr = strdup("POP");
	instruction[811].undecodedStr = strdup("17");
	instruction[811].dstOperandStr = strdup("SS");
	instruction[811].dstOperand = IA_SS;
	instruction[811].valid64 = INVALID;
	instruction[811].compatLeg = VALID;
	instruction[811].VMX = 0;
	instruction[811].description = strdup("Pop top of stack into SS; increment stack pointer.");
	instruction[811].binPrefixLen = 0;
	instruction[811].binOpcodeLen = 1;
	instruction[811].binOpcode = bpstrndup("\x17", 1);
	instruction[811].opcodeOperandCount = 0;

	// Instruction Struct [812] - POP
	instruction[812].instructionStr = strdup("POP");
	instruction[812].undecodedStr = strdup("0F A1");
	instruction[812].dstOperandStr = strdup("FS");
	instruction[812].dstOperand = IA_FS;
	instruction[812].valid64 = INVALID;
	instruction[812].compatLeg = VALID;
	instruction[812].VMX = 0;
	instruction[812].description = strdup("Pop top of stack into FS; increment stack pointer by 16 bits.");
	instruction[812].binPrefixLen = 0;
	instruction[812].binOpcodeLen = 2;
	instruction[812].binOpcode = bpstrndup("\x0F\xA1", 2);
	instruction[812].opcodeOperandCount = 0;

	// Instruction Struct [813] - POP
	instruction[813].instructionStr = strdup("POP");
	instruction[813].undecodedStr = strdup("0F A1");
	instruction[813].dstOperandStr = strdup("FS");
	instruction[813].dstOperand = IA_FS;
	instruction[813].valid64 = INVALID;
	instruction[813].compatLeg = VALID;
	instruction[813].VMX = 0;
	instruction[813].description = strdup("Pop top of stack into FS; increment stack pointer by 32 bits.");
	instruction[813].binPrefixLen = 0;
	instruction[813].binOpcodeLen = 2;
	instruction[813].binOpcode = bpstrndup("\x0F\xA1", 2);
	instruction[813].opcodeOperandCount = 0;

	// Instruction Struct [814] - POP
	instruction[814].instructionStr = strdup("POP");
	instruction[814].undecodedStr = strdup("0F A1");
	instruction[814].dstOperandStr = strdup("FS");
	instruction[814].dstOperand = IA_FS;
	instruction[814].valid64 = INVALID;
	instruction[814].compatLeg = INVALID;
	instruction[814].VMX = 0;
	instruction[814].description = strdup("Pop top of stack into FS; increment stack pointer by 64 bits.");
	instruction[814].binPrefixLen = 0;
	instruction[814].binOpcodeLen = 2;
	instruction[814].binOpcode = bpstrndup("\x0F\xA1", 2);
	instruction[814].opcodeOperandCount = 0;

	// Instruction Struct [815] - POP
	instruction[815].instructionStr = strdup("POP");
	instruction[815].undecodedStr = strdup("0F A9");
	instruction[815].dstOperandStr = strdup("GS");
	instruction[815].dstOperand = IA_GS;
	instruction[815].valid64 = INVALID;
	instruction[815].compatLeg = VALID;
	instruction[815].VMX = 0;
	instruction[815].description = strdup("Pop top of stack into GS; increment stack pointer by 16 bits.");
	instruction[815].binPrefixLen = 0;
	instruction[815].binOpcodeLen = 2;
	instruction[815].binOpcode = bpstrndup("\x0F\xA9", 2);
	instruction[815].opcodeOperandCount = 0;

	// Instruction Struct [816] - POP
	instruction[816].instructionStr = strdup("POP");
	instruction[816].undecodedStr = strdup("0F A9");
	instruction[816].dstOperandStr = strdup("GS");
	instruction[816].dstOperand = IA_GS;
	instruction[816].valid64 = INVALID;
	instruction[816].compatLeg = VALID;
	instruction[816].VMX = 0;
	instruction[816].description = strdup("Pop top of stack into GS; increment stack pointer by 32 bits.");
	instruction[816].binPrefixLen = 0;
	instruction[816].binOpcodeLen = 2;
	instruction[816].binOpcode = bpstrndup("\x0F\xA9", 2);
	instruction[816].opcodeOperandCount = 0;

	// Instruction Struct [817] - POP
	instruction[817].instructionStr = strdup("POP");
	instruction[817].undecodedStr = strdup("0F A9");
	instruction[817].dstOperandStr = strdup("GS");
	instruction[817].dstOperand = IA_GS;
	instruction[817].valid64 = INVALID;
	instruction[817].compatLeg = INVALID;
	instruction[817].VMX = 0;
	instruction[817].description = strdup("Pop top of stack into GS; increment stack pointer by 64 bits.");
	instruction[817].binPrefixLen = 0;
	instruction[817].binOpcodeLen = 2;
	instruction[817].binOpcode = bpstrndup("\x0F\xA9", 2);
	instruction[817].opcodeOperandCount = 0;

	// Instruction Struct [818] - POPA
	instruction[818].instructionStr = strdup("POPA");
	instruction[818].undecodedStr = strdup("61");
	instruction[818].valid64 = INVALID;
	instruction[818].compatLeg = VALID;
	instruction[818].VMX = 0;
	instruction[818].description = strdup("Pop DI, SI, BP, BX, DX, CX, and AX.");
	instruction[818].binPrefixLen = 0;
	instruction[818].binOpcodeLen = 1;
	instruction[818].binOpcode = bpstrndup("\x61", 1);
	instruction[818].opcodeOperandCount = 0;

	// Instruction Struct [819] - POPAD
	instruction[819].instructionStr = strdup("POPAD");
	instruction[819].undecodedStr = strdup("61");
	instruction[819].valid64 = INVALID;
	instruction[819].compatLeg = VALID;
	instruction[819].VMX = 0;
	instruction[819].description = strdup("Pop EDI, ESI, EBP, EBX, EDX, ECX, and EAX.");
	instruction[819].binPrefixLen = 0;
	instruction[819].binOpcodeLen = 1;
	instruction[819].binOpcode = bpstrndup("\x61", 1);
	instruction[819].opcodeOperandCount = 0;

	// Instruction Struct [820] - POPF
	instruction[820].instructionStr = strdup("POPF");
	instruction[820].undecodedStr = strdup("9D");
	instruction[820].valid64 = INVALID;
	instruction[820].compatLeg = VALID;
	instruction[820].VMX = 0;
	instruction[820].description = strdup("Pop top of stack into lower 16 bits of EFLAGS.");
	instruction[820].binPrefixLen = 0;
	instruction[820].binOpcodeLen = 1;
	instruction[820].binOpcode = bpstrndup("\x9D", 1);
	instruction[820].opcodeOperandCount = 0;

	// Instruction Struct [821] - POR
	instruction[821].instructionStr = strdup("POR");
	instruction[821].undecodedStr = strdup("0F EB /r");
	instruction[821].dstOperandStr = strdup("mm");
	instruction[821].dstOperand = IA_MM;
	instruction[821].srcOperandStr = strdup("mm/m64");
	instruction[821].srcOperand = IA_MM_M64;
	instruction[821].valid64 = INVALID;
	instruction[821].compatLeg = VALID;
	instruction[821].VMX = 0;
	instruction[821].description = strdup("Bitwise OR of mm/m64 and mm.");
	instruction[821].binPrefixLen = 0;
	instruction[821].binOpcodeLen = 2;
	instruction[821].binOpcode = bpstrndup("\x0F\xEB", 2);
	instruction[821].opcodeOperand[0] = OA_MODRM;
	instruction[821].opcodeOperandCount = 1;

	// Instruction Struct [822] - POR
	instruction[822].instructionStr = strdup("POR");
	instruction[822].undecodedStr = strdup("66 0F EB /r");
	instruction[822].dstOperandStr = strdup("xmm1");
	instruction[822].dstOperand = IA_XMM;
	instruction[822].srcOperandStr = strdup("xmm2/m128");
	instruction[822].srcOperand = IA_XMM_M128;
	instruction[822].valid64 = INVALID;
	instruction[822].compatLeg = VALID;
	instruction[822].VMX = 0;
	instruction[822].description = strdup("Bitwise OR of xmm2/m128 and xmm1.");
	instruction[822].prefixStr = strdup("OPSIZEOVR");
	instruction[822].binPrefixLen = 1;
	instruction[822].binPrefix = bpstrndup("\x66", 1);
	instruction[822].binOpcodeLen = 2;
	instruction[822].binOpcode = bpstrndup("\x0F\xEB", 2);
	instruction[822].opcodeOperand[0] = OA_MODRM;
	instruction[822].opcodeOperandCount = 1;

	// Instruction Struct [823] - PREFETCHT0
	instruction[823].instructionStr = strdup("PREFETCHT0");
	instruction[823].undecodedStr = strdup("0F 18 /1");
	instruction[823].dstOperandStr = strdup("m8");
	instruction[823].dstOperand = IA_M8;
	instruction[823].valid64 = INVALID;
	instruction[823].compatLeg = VALID;
	instruction[823].VMX = 0;
	instruction[823].description = strdup("Move data from m8 closer to the processor using T0 hint.");
	instruction[823].binPrefixLen = 0;
	instruction[823].binOpcodeLen = 2;
	instruction[823].binOpcode = bpstrndup("\x0F\x18", 2);
	instruction[823].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[823].opcodeOperandCount = 1;

	// Instruction Struct [824] - PREFETCHT1
	instruction[824].instructionStr = strdup("PREFETCHT1");
	instruction[824].undecodedStr = strdup("0F 18 /2");
	instruction[824].dstOperandStr = strdup("m8");
	instruction[824].dstOperand = IA_M8;
	instruction[824].valid64 = INVALID;
	instruction[824].compatLeg = VALID;
	instruction[824].VMX = 0;
	instruction[824].description = strdup("Move data from m8 closer to the processor using T1 hint.");
	instruction[824].binPrefixLen = 0;
	instruction[824].binOpcodeLen = 2;
	instruction[824].binOpcode = bpstrndup("\x0F\x18", 2);
	instruction[824].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[824].opcodeOperandCount = 1;

	// Instruction Struct [825] - PREFETCHT2
	instruction[825].instructionStr = strdup("PREFETCHT2");
	instruction[825].undecodedStr = strdup("0F 18 /3");
	instruction[825].dstOperandStr = strdup("m8");
	instruction[825].dstOperand = IA_M8;
	instruction[825].valid64 = INVALID;
	instruction[825].compatLeg = VALID;
	instruction[825].VMX = 0;
	instruction[825].description = strdup("Move data from m8 closer to the processor using T2 hint.");
	instruction[825].binPrefixLen = 0;
	instruction[825].binOpcodeLen = 2;
	instruction[825].binOpcode = bpstrndup("\x0F\x18", 2);
	instruction[825].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[825].opcodeOperandCount = 1;

	// Instruction Struct [826] - PREFETCHNTA
	instruction[826].instructionStr = strdup("PREFETCHNTA");
	instruction[826].undecodedStr = strdup("0F 18 /0");
	instruction[826].dstOperandStr = strdup("m8");
	instruction[826].dstOperand = IA_M8;
	instruction[826].valid64 = INVALID;
	instruction[826].compatLeg = VALID;
	instruction[826].VMX = 0;
	instruction[826].description = strdup("Move data from m8 closer to the processor using NTA hint.");
	instruction[826].binPrefixLen = 0;
	instruction[826].binOpcodeLen = 2;
	instruction[826].binOpcode = bpstrndup("\x0F\x18", 2);
	instruction[826].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[826].opcodeOperandCount = 1;

	// Instruction Struct [827] - PSADBW
	instruction[827].instructionStr = strdup("PSADBW");
	instruction[827].undecodedStr = strdup("0F F6 /r");
	instruction[827].dstOperandStr = strdup("mm1");
	instruction[827].dstOperand = IA_MM;
	instruction[827].srcOperandStr = strdup("mm2/m64");
	instruction[827].srcOperand = IA_MM_M64;
	instruction[827].valid64 = INVALID;
	instruction[827].compatLeg = VALID;
	instruction[827].VMX = 0;
	instruction[827].description = strdup("Computes the absolute differences of the packed unsigned byte integers from mm2 /m64 and mm1; differences are then summed to produce an unsigned word integer result.");
	instruction[827].binPrefixLen = 0;
	instruction[827].binOpcodeLen = 2;
	instruction[827].binOpcode = bpstrndup("\x0F\xF6", 2);
	instruction[827].opcodeOperand[0] = OA_MODRM;
	instruction[827].opcodeOperandCount = 1;

	// Instruction Struct [828] - PSADBW
	instruction[828].instructionStr = strdup("PSADBW");
	instruction[828].undecodedStr = strdup("66 0F F6 /r");
	instruction[828].dstOperandStr = strdup("xmm1");
	instruction[828].dstOperand = IA_XMM;
	instruction[828].srcOperandStr = strdup("xmm2/m128");
	instruction[828].srcOperand = IA_XMM_M128;
	instruction[828].valid64 = INVALID;
	instruction[828].compatLeg = VALID;
	instruction[828].VMX = 0;
	instruction[828].description = strdup("Computes the absolute differences of the packed unsigned byte integers from xmm2 /m128 and xmm1; the 8 low differences and 8 high differences are then summed separately to produce two unsigned word integer results.");
	instruction[828].prefixStr = strdup("OPSIZEOVR");
	instruction[828].binPrefixLen = 1;
	instruction[828].binPrefix = bpstrndup("\x66", 1);
	instruction[828].binOpcodeLen = 2;
	instruction[828].binOpcode = bpstrndup("\x0F\xF6", 2);
	instruction[828].opcodeOperand[0] = OA_MODRM;
	instruction[828].opcodeOperandCount = 1;

	// Instruction Struct [829] - PSHUFB
	instruction[829].instructionStr = strdup("PSHUFB");
	instruction[829].undecodedStr = strdup("0F 38 00 /r");
	instruction[829].dstOperandStr = strdup("mm1");
	instruction[829].dstOperand = IA_MM;
	instruction[829].srcOperandStr = strdup("mm2/m64");
	instruction[829].srcOperand = IA_MM_M64;
	instruction[829].valid64 = INVALID;
	instruction[829].compatLeg = VALID;
	instruction[829].VMX = 0;
	instruction[829].description = strdup("Shuffle bytes in mm1 according to contents of mm2/m64.");
	instruction[829].binPrefixLen = 0;
	instruction[829].binOpcodeLen = 3;
	instruction[829].binOpcode = bpstrndup("\x0F\x38\x00", 3);
	instruction[829].opcodeOperand[0] = OA_MODRM;
	instruction[829].opcodeOperandCount = 1;

	// Instruction Struct [830] - PSHUFB
	instruction[830].instructionStr = strdup("PSHUFB");
	instruction[830].undecodedStr = strdup("66 0F 38 00 /r");
	instruction[830].dstOperandStr = strdup("xmm1");
	instruction[830].dstOperand = IA_XMM;
	instruction[830].srcOperandStr = strdup("xmm2/m128");
	instruction[830].srcOperand = IA_XMM_M128;
	instruction[830].valid64 = INVALID;
	instruction[830].compatLeg = VALID;
	instruction[830].VMX = 0;
	instruction[830].description = strdup("Shuffle bytes in xmm1 according to contents of xmm2/m128.");
	instruction[830].prefixStr = strdup("OPSIZEOVR");
	instruction[830].binPrefixLen = 1;
	instruction[830].binPrefix = bpstrndup("\x66", 1);
	instruction[830].binOpcodeLen = 3;
	instruction[830].binOpcode = bpstrndup("\x0F\x38\x00", 3);
	instruction[830].opcodeOperand[0] = OA_MODRM;
	instruction[830].opcodeOperandCount = 1;

	// Instruction Struct [831] - PSHUFD
	instruction[831].instructionStr = strdup("PSHUFD");
	instruction[831].undecodedStr = strdup("66 0F 70 /r ib");
	instruction[831].dstOperandStr = strdup("xmm1");
	instruction[831].dstOperand = IA_XMM;
	instruction[831].srcOperandStr = strdup("xmm2/m128");
	instruction[831].srcOperand = IA_XMM_M128;
	instruction[831].thirdOperandStr = strdup("imm8");
	instruction[831].thirdOperand = IA_IMM8;
	instruction[831].valid64 = INVALID;
	instruction[831].compatLeg = VALID;
	instruction[831].VMX = 0;
	instruction[831].description = strdup("Shuffle the doublewords in xmm2/m128 based on the encoding in imm8 and store the result in xmm1.");
	instruction[831].prefixStr = strdup("OPSIZEOVR");
	instruction[831].binPrefixLen = 1;
	instruction[831].binPrefix = bpstrndup("\x66", 1);
	instruction[831].binOpcodeLen = 2;
	instruction[831].binOpcode = bpstrndup("\x0F\x70", 2);
	instruction[831].opcodeOperand[0] = OA_MODRM;
	instruction[831].opcodeOperand[1] = OA_IB;
	instruction[831].opcodeOperandCount = 2;

	// Instruction Struct [832] - PSHUFHW
	instruction[832].instructionStr = strdup("PSHUFHW");
	instruction[832].undecodedStr = strdup("F3 0F 70 /r ib");
	instruction[832].dstOperandStr = strdup("xmm1");
	instruction[832].dstOperand = IA_XMM;
	instruction[832].srcOperandStr = strdup("xmm2/m128");
	instruction[832].srcOperand = IA_XMM_M128;
	instruction[832].thirdOperandStr = strdup("imm8");
	instruction[832].thirdOperand = IA_IMM8;
	instruction[832].valid64 = INVALID;
	instruction[832].compatLeg = VALID;
	instruction[832].VMX = 0;
	instruction[832].description = strdup("Shuffle the high words in xmm2/m128 based on the encoding in imm8 and store the result in xmm1.");
	instruction[832].prefixStr = strdup("REP");
	instruction[832].binPrefixLen = 1;
	instruction[832].binPrefix = bpstrndup("\xF3", 1);
	instruction[832].binOpcodeLen = 2;
	instruction[832].binOpcode = bpstrndup("\x0F\x70", 2);
	instruction[832].opcodeOperand[0] = OA_MODRM;
	instruction[832].opcodeOperand[1] = OA_IB;
	instruction[832].opcodeOperandCount = 2;

	// Instruction Struct [833] - PSHUFLW
	instruction[833].instructionStr = strdup("PSHUFLW");
	instruction[833].undecodedStr = strdup("F2 0F 70 /r ib");
	instruction[833].dstOperandStr = strdup("xmm1");
	instruction[833].dstOperand = IA_XMM;
	instruction[833].srcOperandStr = strdup("xmm2/m128");
	instruction[833].srcOperand = IA_XMM_M128;
	instruction[833].thirdOperandStr = strdup("imm8");
	instruction[833].thirdOperand = IA_IMM8;
	instruction[833].valid64 = INVALID;
	instruction[833].compatLeg = VALID;
	instruction[833].VMX = 0;
	instruction[833].description = strdup("Shuffle the low words in xmm2/m128 based on the encoding in imm8 and store the result in xmm1.");
	instruction[833].prefixStr = strdup("REPNE");
	instruction[833].binPrefixLen = 1;
	instruction[833].binPrefix = bpstrndup("\xF2", 1);
	instruction[833].binOpcodeLen = 2;
	instruction[833].binOpcode = bpstrndup("\x0F\x70", 2);
	instruction[833].opcodeOperand[0] = OA_MODRM;
	instruction[833].opcodeOperand[1] = OA_IB;
	instruction[833].opcodeOperandCount = 2;

	// Instruction Struct [834] - PSHUFW
	instruction[834].instructionStr = strdup("PSHUFW");
	instruction[834].undecodedStr = strdup("0F 70 /r ib");
	instruction[834].dstOperandStr = strdup("mm1");
	instruction[834].dstOperand = IA_MM;
	instruction[834].srcOperandStr = strdup("mm2/m64");
	instruction[834].srcOperand = IA_MM_M64;
	instruction[834].thirdOperandStr = strdup("imm8");
	instruction[834].thirdOperand = IA_IMM8;
	instruction[834].valid64 = INVALID;
	instruction[834].compatLeg = VALID;
	instruction[834].VMX = 0;
	instruction[834].description = strdup("Shuffle the words in mm2/m64 based on the encoding in imm8 and store the result in mm1.");
	instruction[834].binPrefixLen = 0;
	instruction[834].binOpcodeLen = 2;
	instruction[834].binOpcode = bpstrndup("\x0F\x70", 2);
	instruction[834].opcodeOperand[0] = OA_MODRM;
	instruction[834].opcodeOperand[1] = OA_IB;
	instruction[834].opcodeOperandCount = 2;

	// Instruction Struct [835] - PSIGNB
	instruction[835].instructionStr = strdup("PSIGNB");
	instruction[835].undecodedStr = strdup("0F 38 08 /r");
	instruction[835].dstOperandStr = strdup("mm1");
	instruction[835].dstOperand = IA_MM;
	instruction[835].srcOperandStr = strdup("mm2/m64");
	instruction[835].srcOperand = IA_MM_M64;
	instruction[835].valid64 = INVALID;
	instruction[835].compatLeg = VALID;
	instruction[835].VMX = 0;
	instruction[835].description = strdup("Negate packed byte integers in mm1 if the corresponding sign in mm2/m64 is less than zero.");
	instruction[835].binPrefixLen = 0;
	instruction[835].binOpcodeLen = 3;
	instruction[835].binOpcode = bpstrndup("\x0F\x38\x08", 3);
	instruction[835].opcodeOperand[0] = OA_MODRM;
	instruction[835].opcodeOperandCount = 1;

	// Instruction Struct [836] - PSIGNB
	instruction[836].instructionStr = strdup("PSIGNB");
	instruction[836].undecodedStr = strdup("66 0F 38 08 /r");
	instruction[836].dstOperandStr = strdup("xmm1");
	instruction[836].dstOperand = IA_XMM;
	instruction[836].srcOperandStr = strdup("xmm2/m128");
	instruction[836].srcOperand = IA_XMM_M128;
	instruction[836].valid64 = INVALID;
	instruction[836].compatLeg = VALID;
	instruction[836].VMX = 0;
	instruction[836].description = strdup("Negate packed byte integers in xmm1 if the corresponding sign in xmm2/m128 is less than zero.");
	instruction[836].prefixStr = strdup("OPSIZEOVR");
	instruction[836].binPrefixLen = 1;
	instruction[836].binPrefix = bpstrndup("\x66", 1);
	instruction[836].binOpcodeLen = 3;
	instruction[836].binOpcode = bpstrndup("\x0F\x38\x08", 3);
	instruction[836].opcodeOperand[0] = OA_MODRM;
	instruction[836].opcodeOperandCount = 1;

	// Instruction Struct [837] - PSIGNW
	instruction[837].instructionStr = strdup("PSIGNW");
	instruction[837].undecodedStr = strdup("0F 38 09 /r");
	instruction[837].dstOperandStr = strdup("mm1");
	instruction[837].dstOperand = IA_MM;
	instruction[837].srcOperandStr = strdup("mm2/m64");
	instruction[837].srcOperand = IA_MM_M64;
	instruction[837].valid64 = INVALID;
	instruction[837].compatLeg = VALID;
	instruction[837].VMX = 0;
	instruction[837].description = strdup("Negate packed 16-bit integers in mm1");
	instruction[837].binPrefixLen = 0;
	instruction[837].binOpcodeLen = 3;
	instruction[837].binOpcode = bpstrndup("\x0F\x38\x09", 3);
	instruction[837].opcodeOperand[0] = OA_MODRM;
	instruction[837].opcodeOperandCount = 1;

	// Instruction Struct [838] - CMP
	instruction[838].instructionStr = strdup("CMP");
	instruction[838].undecodedStr = strdup("3D iw");
	instruction[838].dstOperandStr = strdup("AX");
	instruction[838].dstOperand = IA_AX;
	instruction[838].srcOperandStr = strdup("imm16");
	instruction[838].srcOperand = IA_IMM16;
	instruction[838].valid64 = INVALID;
	instruction[838].compatLeg = VALID;
	instruction[838].VMX = 0;
	instruction[838].description = strdup("Compare imm16 with AX.");
	instruction[838].binPrefixLen = 0;
	instruction[838].binOpcodeLen = 1;
	instruction[838].binOpcode = bpstrndup("\x3D", 1);
	instruction[838].opcodeOperand[0] = OA_IW;
	instruction[838].opcodeOperandCount = 1;

	// Instruction Struct [839] - PSIGNW
	instruction[839].instructionStr = strdup("PSIGNW");
	instruction[839].undecodedStr = strdup("66 0F 38 09 /r");
	instruction[839].dstOperandStr = strdup("xmm1");
	instruction[839].dstOperand = IA_XMM;
	instruction[839].srcOperandStr = strdup("xmm2/m128");
	instruction[839].srcOperand = IA_XMM_M128;
	instruction[839].valid64 = INVALID;
	instruction[839].compatLeg = VALID;
	instruction[839].VMX = 0;
	instruction[839].description = strdup("Negate packed 16-bit integers in xmm1 if the corresponding sign in xmm2/m128 is less than zero.");
	instruction[839].prefixStr = strdup("OPSIZEOVR");
	instruction[839].binPrefixLen = 1;
	instruction[839].binPrefix = bpstrndup("\x66", 1);
	instruction[839].binOpcodeLen = 3;
	instruction[839].binOpcode = bpstrndup("\x0F\x38\x09", 3);
	instruction[839].opcodeOperand[0] = OA_MODRM;
	instruction[839].opcodeOperandCount = 1;

	// Instruction Struct [840] - PSIGND
	instruction[840].instructionStr = strdup("PSIGND");
	instruction[840].undecodedStr = strdup("0F 38 0A /r");
	instruction[840].dstOperandStr = strdup("mm1");
	instruction[840].dstOperand = IA_MM;
	instruction[840].srcOperandStr = strdup("mm2/m64");
	instruction[840].srcOperand = IA_MM_M64;
	instruction[840].valid64 = INVALID;
	instruction[840].compatLeg = VALID;
	instruction[840].VMX = 0;
	instruction[840].description = strdup("Negate packed doubleword integers in");
	instruction[840].binPrefixLen = 0;
	instruction[840].binOpcodeLen = 3;
	instruction[840].binOpcode = bpstrndup("\x0F\x38\x0A", 3);
	instruction[840].opcodeOperand[0] = OA_MODRM;
	instruction[840].opcodeOperandCount = 1;

	// Instruction Struct [841] - PSIGND
	instruction[841].instructionStr = strdup("PSIGND");
	instruction[841].undecodedStr = strdup("66 0F 38 0A /r");
	instruction[841].dstOperandStr = strdup("xmm1");
	instruction[841].dstOperand = IA_XMM;
	instruction[841].srcOperandStr = strdup("xmm2/m128");
	instruction[841].srcOperand = IA_XMM_M128;
	instruction[841].valid64 = INVALID;
	instruction[841].compatLeg = VALID;
	instruction[841].VMX = 0;
	instruction[841].description = strdup("Negate packed doubleword integers in xmm1 if the corresponding sign in xmm2/m128 is less than zero.");
	instruction[841].prefixStr = strdup("OPSIZEOVR");
	instruction[841].binPrefixLen = 1;
	instruction[841].binPrefix = bpstrndup("\x66", 1);
	instruction[841].binOpcodeLen = 3;
	instruction[841].binOpcode = bpstrndup("\x0F\x38\x0A", 3);
	instruction[841].opcodeOperand[0] = OA_MODRM;
	instruction[841].opcodeOperandCount = 1;

	// Instruction Struct [842] - PSLLDQ
	instruction[842].instructionStr = strdup("PSLLDQ");
	instruction[842].undecodedStr = strdup("66 0F 73 /7 ib");
	instruction[842].dstOperandStr = strdup("xmm1");
	instruction[842].dstOperand = IA_XMM;
	instruction[842].srcOperandStr = strdup("imm8");
	instruction[842].srcOperand = IA_IMM8;
	instruction[842].valid64 = INVALID;
	instruction[842].compatLeg = VALID;
	instruction[842].VMX = 0;
	instruction[842].description = strdup("Shift xmm1 left by imm8 bytes while shifting in 0s.");
	instruction[842].prefixStr = strdup("OPSIZEOVR");
	instruction[842].binPrefixLen = 1;
	instruction[842].binPrefix = bpstrndup("\x66", 1);
	instruction[842].binOpcodeLen = 2;
	instruction[842].binOpcode = bpstrndup("\x0F\x73", 2);
	instruction[842].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[842].opcodeOperand[1] = OA_IB;
	instruction[842].opcodeOperandCount = 2;

	// Instruction Struct [843] - PSLLW
	instruction[843].instructionStr = strdup("PSLLW");
	instruction[843].undecodedStr = strdup("0F F1 /r");
	instruction[843].dstOperandStr = strdup("mm");
	instruction[843].dstOperand = IA_MM;
	instruction[843].srcOperandStr = strdup("mm/m64");
	instruction[843].srcOperand = IA_MM_M64;
	instruction[843].valid64 = INVALID;
	instruction[843].compatLeg = VALID;
	instruction[843].VMX = 0;
	instruction[843].description = strdup("Shift words in mm left mm/m64 while shifting in 0s.");
	instruction[843].binPrefixLen = 0;
	instruction[843].binOpcodeLen = 2;
	instruction[843].binOpcode = bpstrndup("\x0F\xF1", 2);
	instruction[843].opcodeOperand[0] = OA_MODRM;
	instruction[843].opcodeOperandCount = 1;

	// Instruction Struct [844] - PSLLW
	instruction[844].instructionStr = strdup("PSLLW");
	instruction[844].undecodedStr = strdup("66 0F F1 /r");
	instruction[844].dstOperandStr = strdup("xmm1");
	instruction[844].dstOperand = IA_XMM;
	instruction[844].srcOperandStr = strdup("xmm2/m128");
	instruction[844].srcOperand = IA_XMM_M128;
	instruction[844].valid64 = INVALID;
	instruction[844].compatLeg = VALID;
	instruction[844].VMX = 0;
	instruction[844].description = strdup("Shift words in xmm1 left by xmm2/m128 while shifting in 0s.");
	instruction[844].prefixStr = strdup("OPSIZEOVR");
	instruction[844].binPrefixLen = 1;
	instruction[844].binPrefix = bpstrndup("\x66", 1);
	instruction[844].binOpcodeLen = 2;
	instruction[844].binOpcode = bpstrndup("\x0F\xF1", 2);
	instruction[844].opcodeOperand[0] = OA_MODRM;
	instruction[844].opcodeOperandCount = 1;

	// Instruction Struct [845] - PSLLW
	instruction[845].instructionStr = strdup("PSLLW");
	instruction[845].undecodedStr = strdup("0F 71 /6 ib");
	instruction[845].dstOperandStr = strdup("xmm1");
	instruction[845].dstOperand = IA_XMM;
	instruction[845].srcOperandStr = strdup("imm8");
	instruction[845].srcOperand = IA_IMM8;
	instruction[845].valid64 = INVALID;
	instruction[845].compatLeg = VALID;
	instruction[845].VMX = 0;
	instruction[845].description = strdup("Shift words in mm left by imm8 while shifting in 0s.");
	instruction[845].binPrefixLen = 0;
	instruction[845].binOpcodeLen = 2;
	instruction[845].binOpcode = bpstrndup("\x0F\x71", 2);
	instruction[845].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[845].opcodeOperand[1] = OA_IB;
	instruction[845].opcodeOperandCount = 2;

	// Instruction Struct [846] - PSLLW
	instruction[846].instructionStr = strdup("PSLLW");
	instruction[846].undecodedStr = strdup("66 0F 71 /6 ib");
	instruction[846].dstOperandStr = strdup("xmm1");
	instruction[846].dstOperand = IA_XMM;
	instruction[846].srcOperandStr = strdup("imm8");
	instruction[846].srcOperand = IA_IMM8;
	instruction[846].valid64 = INVALID;
	instruction[846].compatLeg = VALID;
	instruction[846].VMX = 0;
	instruction[846].description = strdup("Shift words in xmm1 left by imm8 while shifting in 0s.");
	instruction[846].prefixStr = strdup("OPSIZEOVR");
	instruction[846].binPrefixLen = 1;
	instruction[846].binPrefix = bpstrndup("\x66", 1);
	instruction[846].binOpcodeLen = 2;
	instruction[846].binOpcode = bpstrndup("\x0F\x71", 2);
	instruction[846].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[846].opcodeOperand[1] = OA_IB;
	instruction[846].opcodeOperandCount = 2;

	// Instruction Struct [847] - PSLLD
	instruction[847].instructionStr = strdup("PSLLD");
	instruction[847].undecodedStr = strdup("0F F2 /r");
	instruction[847].dstOperandStr = strdup("mm");
	instruction[847].dstOperand = IA_MM;
	instruction[847].srcOperandStr = strdup("mm/m64");
	instruction[847].srcOperand = IA_MM_M64;
	instruction[847].valid64 = INVALID;
	instruction[847].compatLeg = VALID;
	instruction[847].VMX = 0;
	instruction[847].description = strdup("Shift doublewords in mm left by mm/m64 while shifting in 0s.");
	instruction[847].binPrefixLen = 0;
	instruction[847].binOpcodeLen = 2;
	instruction[847].binOpcode = bpstrndup("\x0F\xF2", 2);
	instruction[847].opcodeOperand[0] = OA_MODRM;
	instruction[847].opcodeOperandCount = 1;

	// Instruction Struct [848] - PSLLD
	instruction[848].instructionStr = strdup("PSLLD");
	instruction[848].undecodedStr = strdup("66 0F F2 /r");
	instruction[848].dstOperandStr = strdup("xmm1");
	instruction[848].dstOperand = IA_XMM;
	instruction[848].srcOperandStr = strdup("xmm2/m128");
	instruction[848].srcOperand = IA_XMM_M128;
	instruction[848].valid64 = INVALID;
	instruction[848].compatLeg = VALID;
	instruction[848].VMX = 0;
	instruction[848].description = strdup("Shift doublewords in xmm1 left by xmm2/m128 while shifting in 0s.");
	instruction[848].prefixStr = strdup("OPSIZEOVR");
	instruction[848].binPrefixLen = 1;
	instruction[848].binPrefix = bpstrndup("\x66", 1);
	instruction[848].binOpcodeLen = 2;
	instruction[848].binOpcode = bpstrndup("\x0F\xF2", 2);
	instruction[848].opcodeOperand[0] = OA_MODRM;
	instruction[848].opcodeOperandCount = 1;

	// Instruction Struct [849] - PSLLD
	instruction[849].instructionStr = strdup("PSLLD");
	instruction[849].undecodedStr = strdup("0F 72 /6 ib");
	instruction[849].dstOperandStr = strdup("mm");
	instruction[849].dstOperand = IA_MM;
	instruction[849].srcOperandStr = strdup("imm8");
	instruction[849].srcOperand = IA_IMM8;
	instruction[849].valid64 = INVALID;
	instruction[849].compatLeg = VALID;
	instruction[849].VMX = 0;
	instruction[849].description = strdup("Shift doublewords in mm left by imm8 while shifting in 0s.");
	instruction[849].binPrefixLen = 0;
	instruction[849].binOpcodeLen = 2;
	instruction[849].binOpcode = bpstrndup("\x0F\x72", 2);
	instruction[849].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[849].opcodeOperand[1] = OA_IB;
	instruction[849].opcodeOperandCount = 2;

	// Instruction Struct [850] - PSLLD
	instruction[850].instructionStr = strdup("PSLLD");
	instruction[850].undecodedStr = strdup("66 0F 72 /6 ib");
	instruction[850].dstOperandStr = strdup("xmm1");
	instruction[850].dstOperand = IA_XMM;
	instruction[850].srcOperandStr = strdup("imm8");
	instruction[850].srcOperand = IA_IMM8;
	instruction[850].valid64 = INVALID;
	instruction[850].compatLeg = VALID;
	instruction[850].VMX = 0;
	instruction[850].description = strdup("Shift doublewords in xmm1 left by imm8 while shifting in 0s.");
	instruction[850].prefixStr = strdup("OPSIZEOVR");
	instruction[850].binPrefixLen = 1;
	instruction[850].binPrefix = bpstrndup("\x66", 1);
	instruction[850].binOpcodeLen = 2;
	instruction[850].binOpcode = bpstrndup("\x0F\x72", 2);
	instruction[850].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[850].opcodeOperand[1] = OA_IB;
	instruction[850].opcodeOperandCount = 2;

	// Instruction Struct [851] - PSLLQ
	instruction[851].instructionStr = strdup("PSLLQ");
	instruction[851].undecodedStr = strdup("0F F3 /r");
	instruction[851].dstOperandStr = strdup("mm");
	instruction[851].dstOperand = IA_MM;
	instruction[851].srcOperandStr = strdup("mm/m64");
	instruction[851].srcOperand = IA_MM_M64;
	instruction[851].valid64 = INVALID;
	instruction[851].compatLeg = VALID;
	instruction[851].VMX = 0;
	instruction[851].description = strdup("Shift quadword in mm left by mm/m64 while shifting in 0s.");
	instruction[851].binPrefixLen = 0;
	instruction[851].binOpcodeLen = 2;
	instruction[851].binOpcode = bpstrndup("\x0F\xF3", 2);
	instruction[851].opcodeOperand[0] = OA_MODRM;
	instruction[851].opcodeOperandCount = 1;

	// Instruction Struct [852] - PSLLQ
	instruction[852].instructionStr = strdup("PSLLQ");
	instruction[852].undecodedStr = strdup("66 0F F3 /r");
	instruction[852].dstOperandStr = strdup("xmm1");
	instruction[852].dstOperand = IA_XMM;
	instruction[852].srcOperandStr = strdup("xmm2/m128");
	instruction[852].srcOperand = IA_XMM_M128;
	instruction[852].valid64 = INVALID;
	instruction[852].compatLeg = VALID;
	instruction[852].VMX = 0;
	instruction[852].description = strdup("Shift quadwords in xmm1 left by xmm2/m128 while shifting in 0s.");
	instruction[852].prefixStr = strdup("OPSIZEOVR");
	instruction[852].binPrefixLen = 1;
	instruction[852].binPrefix = bpstrndup("\x66", 1);
	instruction[852].binOpcodeLen = 2;
	instruction[852].binOpcode = bpstrndup("\x0F\xF3", 2);
	instruction[852].opcodeOperand[0] = OA_MODRM;
	instruction[852].opcodeOperandCount = 1;

	// Instruction Struct [853] - PSLLQ
	instruction[853].instructionStr = strdup("PSLLQ");
	instruction[853].undecodedStr = strdup("0F 73 /6 ib");
	instruction[853].dstOperandStr = strdup("mm");
	instruction[853].dstOperand = IA_MM;
	instruction[853].srcOperandStr = strdup("imm8");
	instruction[853].srcOperand = IA_IMM8;
	instruction[853].valid64 = INVALID;
	instruction[853].compatLeg = VALID;
	instruction[853].VMX = 0;
	instruction[853].description = strdup("Shift quadword in mm left by imm8 while shifting in 0s.");
	instruction[853].binPrefixLen = 0;
	instruction[853].binOpcodeLen = 2;
	instruction[853].binOpcode = bpstrndup("\x0F\x73", 2);
	instruction[853].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[853].opcodeOperand[1] = OA_IB;
	instruction[853].opcodeOperandCount = 2;

	// Instruction Struct [854] - PSLLQ
	instruction[854].instructionStr = strdup("PSLLQ");
	instruction[854].undecodedStr = strdup("66 0F 73 /6 ib");
	instruction[854].dstOperandStr = strdup("xmm1");
	instruction[854].dstOperand = IA_XMM;
	instruction[854].srcOperandStr = strdup("imm8");
	instruction[854].srcOperand = IA_IMM8;
	instruction[854].valid64 = INVALID;
	instruction[854].compatLeg = VALID;
	instruction[854].VMX = 0;
	instruction[854].description = strdup("Shift quadwords in xmm1 left by imm8 while shifting in 0s.");
	instruction[854].prefixStr = strdup("OPSIZEOVR");
	instruction[854].binPrefixLen = 1;
	instruction[854].binPrefix = bpstrndup("\x66", 1);
	instruction[854].binOpcodeLen = 2;
	instruction[854].binOpcode = bpstrndup("\x0F\x73", 2);
	instruction[854].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[854].opcodeOperand[1] = OA_IB;
	instruction[854].opcodeOperandCount = 2;

	// Instruction Struct [855] - PSRAW
	instruction[855].instructionStr = strdup("PSRAW");
	instruction[855].undecodedStr = strdup("0F E1 /r");
	instruction[855].dstOperandStr = strdup("mm");
	instruction[855].dstOperand = IA_MM;
	instruction[855].srcOperandStr = strdup("mm/m64");
	instruction[855].srcOperand = IA_MM_M64;
	instruction[855].valid64 = INVALID;
	instruction[855].compatLeg = VALID;
	instruction[855].VMX = 0;
	instruction[855].description = strdup("Shift words in mm right by mm/m64 while shifting in sign bits.");
	instruction[855].binPrefixLen = 0;
	instruction[855].binOpcodeLen = 2;
	instruction[855].binOpcode = bpstrndup("\x0F\xE1", 2);
	instruction[855].opcodeOperand[0] = OA_MODRM;
	instruction[855].opcodeOperandCount = 1;

	// Instruction Struct [856] - PSRAW
	instruction[856].instructionStr = strdup("PSRAW");
	instruction[856].undecodedStr = strdup("66 0F E1 /r");
	instruction[856].dstOperandStr = strdup("xmm1");
	instruction[856].dstOperand = IA_XMM;
	instruction[856].srcOperandStr = strdup("xmm2/m128");
	instruction[856].srcOperand = IA_XMM_M128;
	instruction[856].valid64 = INVALID;
	instruction[856].compatLeg = VALID;
	instruction[856].VMX = 0;
	instruction[856].description = strdup("Shift words in xmm1 right by xmm2/m128 while shifting in sign bits.");
	instruction[856].prefixStr = strdup("OPSIZEOVR");
	instruction[856].binPrefixLen = 1;
	instruction[856].binPrefix = bpstrndup("\x66", 1);
	instruction[856].binOpcodeLen = 2;
	instruction[856].binOpcode = bpstrndup("\x0F\xE1", 2);
	instruction[856].opcodeOperand[0] = OA_MODRM;
	instruction[856].opcodeOperandCount = 1;

	// Instruction Struct [857] - PSRAW
	instruction[857].instructionStr = strdup("PSRAW");
	instruction[857].undecodedStr = strdup("0F 71 /4 ib");
	instruction[857].dstOperandStr = strdup("mm");
	instruction[857].dstOperand = IA_MM;
	instruction[857].srcOperandStr = strdup("imm8");
	instruction[857].srcOperand = IA_IMM8;
	instruction[857].valid64 = INVALID;
	instruction[857].compatLeg = VALID;
	instruction[857].VMX = 0;
	instruction[857].description = strdup("Shift words in mm right by imm8 while shifting in sign bits");
	instruction[857].binPrefixLen = 0;
	instruction[857].binOpcodeLen = 2;
	instruction[857].binOpcode = bpstrndup("\x0F\x71", 2);
	instruction[857].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[857].opcodeOperand[1] = OA_IB;
	instruction[857].opcodeOperandCount = 2;

	// Instruction Struct [858] - PSRAW
	instruction[858].instructionStr = strdup("PSRAW");
	instruction[858].undecodedStr = strdup("66 0F 71 /4 ib");
	instruction[858].dstOperandStr = strdup("xmm1");
	instruction[858].dstOperand = IA_XMM;
	instruction[858].srcOperandStr = strdup("imm8");
	instruction[858].srcOperand = IA_IMM8;
	instruction[858].valid64 = INVALID;
	instruction[858].compatLeg = VALID;
	instruction[858].VMX = 0;
	instruction[858].description = strdup("Shift words in xmm1 right by imm8 while shifting in sign bits");
	instruction[858].prefixStr = strdup("OPSIZEOVR");
	instruction[858].binPrefixLen = 1;
	instruction[858].binPrefix = bpstrndup("\x66", 1);
	instruction[858].binOpcodeLen = 2;
	instruction[858].binOpcode = bpstrndup("\x0F\x71", 2);
	instruction[858].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[858].opcodeOperand[1] = OA_IB;
	instruction[858].opcodeOperandCount = 2;

	// Instruction Struct [859] - PSRAD
	instruction[859].instructionStr = strdup("PSRAD");
	instruction[859].undecodedStr = strdup("0F E2 /r");
	instruction[859].dstOperandStr = strdup("mm");
	instruction[859].dstOperand = IA_MM;
	instruction[859].srcOperandStr = strdup("mm/m64");
	instruction[859].srcOperand = IA_MM_M64;
	instruction[859].valid64 = INVALID;
	instruction[859].compatLeg = VALID;
	instruction[859].VMX = 0;
	instruction[859].description = strdup("Shift doublewords in mm right by mm/m64 while shifting in sign bits.");
	instruction[859].binPrefixLen = 0;
	instruction[859].binOpcodeLen = 2;
	instruction[859].binOpcode = bpstrndup("\x0F\xE2", 2);
	instruction[859].opcodeOperand[0] = OA_MODRM;
	instruction[859].opcodeOperandCount = 1;

	// Instruction Struct [860] - PSRAD
	instruction[860].instructionStr = strdup("PSRAD");
	instruction[860].undecodedStr = strdup("66 0F E2 /r");
	instruction[860].dstOperandStr = strdup("xmm1");
	instruction[860].dstOperand = IA_XMM;
	instruction[860].srcOperandStr = strdup("xmm2/m128");
	instruction[860].srcOperand = IA_XMM_M128;
	instruction[860].valid64 = INVALID;
	instruction[860].compatLeg = VALID;
	instruction[860].VMX = 0;
	instruction[860].description = strdup("Shift doubleword in xmm1 right by xmm2 /m128 while shifting in sign bits.");
	instruction[860].prefixStr = strdup("OPSIZEOVR");
	instruction[860].binPrefixLen = 1;
	instruction[860].binPrefix = bpstrndup("\x66", 1);
	instruction[860].binOpcodeLen = 2;
	instruction[860].binOpcode = bpstrndup("\x0F\xE2", 2);
	instruction[860].opcodeOperand[0] = OA_MODRM;
	instruction[860].opcodeOperandCount = 1;

	// Instruction Struct [861] - PSRAD
	instruction[861].instructionStr = strdup("PSRAD");
	instruction[861].undecodedStr = strdup("0F 72 /4 ib");
	instruction[861].dstOperandStr = strdup("mm");
	instruction[861].dstOperand = IA_MM;
	instruction[861].srcOperandStr = strdup("imm8");
	instruction[861].srcOperand = IA_IMM8;
	instruction[861].valid64 = INVALID;
	instruction[861].compatLeg = VALID;
	instruction[861].VMX = 0;
	instruction[861].description = strdup("Shift doublewords in mm right by imm8 while shifting in sign bits.");
	instruction[861].binPrefixLen = 0;
	instruction[861].binOpcodeLen = 2;
	instruction[861].binOpcode = bpstrndup("\x0F\x72", 2);
	instruction[861].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[861].opcodeOperand[1] = OA_IB;
	instruction[861].opcodeOperandCount = 2;

	// Instruction Struct [862] - PSRAD
	instruction[862].instructionStr = strdup("PSRAD");
	instruction[862].undecodedStr = strdup("66 0F 72 /4 ib");
	instruction[862].dstOperandStr = strdup("xmm1");
	instruction[862].dstOperand = IA_XMM;
	instruction[862].srcOperandStr = strdup("imm8");
	instruction[862].srcOperand = IA_IMM8;
	instruction[862].valid64 = INVALID;
	instruction[862].compatLeg = VALID;
	instruction[862].VMX = 0;
	instruction[862].description = strdup("Shift doublewords in xmm1 right by imm8 while shifting in sign bits.");
	instruction[862].prefixStr = strdup("OPSIZEOVR");
	instruction[862].binPrefixLen = 1;
	instruction[862].binPrefix = bpstrndup("\x66", 1);
	instruction[862].binOpcodeLen = 2;
	instruction[862].binOpcode = bpstrndup("\x0F\x72", 2);
	instruction[862].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[862].opcodeOperand[1] = OA_IB;
	instruction[862].opcodeOperandCount = 2;

	// Instruction Struct [863] - PSRLDQ
	instruction[863].instructionStr = strdup("PSRLDQ");
	instruction[863].undecodedStr = strdup("66 0F 73 /3 ib");
	instruction[863].dstOperandStr = strdup("imm8");
	instruction[863].dstOperand = IA_IMM8;
	instruction[863].valid64 = INVALID;
	instruction[863].compatLeg = VALID;
	instruction[863].VMX = 0;
	instruction[863].description = strdup("Shift xmm1 right by imm8 while shifting in 0s.");
	instruction[863].prefixStr = strdup("OPSIZEOVR");
	instruction[863].binPrefixLen = 1;
	instruction[863].binPrefix = bpstrndup("\x66", 1);
	instruction[863].binOpcodeLen = 2;
	instruction[863].binOpcode = bpstrndup("\x0F\x73", 2);
	instruction[863].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[863].opcodeOperand[1] = OA_IB;
	instruction[863].opcodeOperandCount = 2;

	// Instruction Struct [864] - PSRLW
	instruction[864].instructionStr = strdup("PSRLW");
	instruction[864].undecodedStr = strdup("0F D1 /r");
	instruction[864].dstOperandStr = strdup("mm");
	instruction[864].dstOperand = IA_MM;
	instruction[864].srcOperandStr = strdup("mm/m64");
	instruction[864].srcOperand = IA_MM_M64;
	instruction[864].valid64 = INVALID;
	instruction[864].compatLeg = VALID;
	instruction[864].VMX = 0;
	instruction[864].description = strdup("Shift words in mm right by amount specified in mm/m64 while shifting in 0s.");
	instruction[864].binPrefixLen = 0;
	instruction[864].binOpcodeLen = 2;
	instruction[864].binOpcode = bpstrndup("\x0F\xD1", 2);
	instruction[864].opcodeOperand[0] = OA_MODRM;
	instruction[864].opcodeOperandCount = 1;

	// Instruction Struct [865] - PSRLW
	instruction[865].instructionStr = strdup("PSRLW");
	instruction[865].undecodedStr = strdup("66 0F D1 /r");
	instruction[865].dstOperandStr = strdup("xmm1");
	instruction[865].dstOperand = IA_XMM;
	instruction[865].srcOperandStr = strdup("xmm2/m128");
	instruction[865].srcOperand = IA_XMM_M128;
	instruction[865].valid64 = INVALID;
	instruction[865].compatLeg = VALID;
	instruction[865].VMX = 0;
	instruction[865].description = strdup("Shift words in xmm1 right by amount specified in xmm2/m128 while shifting in 0s.");
	instruction[865].prefixStr = strdup("OPSIZEOVR");
	instruction[865].binPrefixLen = 1;
	instruction[865].binPrefix = bpstrndup("\x66", 1);
	instruction[865].binOpcodeLen = 2;
	instruction[865].binOpcode = bpstrndup("\x0F\xD1", 2);
	instruction[865].opcodeOperand[0] = OA_MODRM;
	instruction[865].opcodeOperandCount = 1;

	// Instruction Struct [866] - PSRLW
	instruction[866].instructionStr = strdup("PSRLW");
	instruction[866].undecodedStr = strdup("0F 71 /2 ib");
	instruction[866].dstOperandStr = strdup("mm");
	instruction[866].dstOperand = IA_MM;
	instruction[866].srcOperandStr = strdup("imm8");
	instruction[866].srcOperand = IA_IMM8;
	instruction[866].valid64 = INVALID;
	instruction[866].compatLeg = VALID;
	instruction[866].VMX = 0;
	instruction[866].description = strdup("Shift words in mm right by imm8 while shifting in 0s.");
	instruction[866].binPrefixLen = 0;
	instruction[866].binOpcodeLen = 2;
	instruction[866].binOpcode = bpstrndup("\x0F\x71", 2);
	instruction[866].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[866].opcodeOperand[1] = OA_IB;
	instruction[866].opcodeOperandCount = 2;

	// Instruction Struct [867] - PSRLW
	instruction[867].instructionStr = strdup("PSRLW");
	instruction[867].undecodedStr = strdup("66 0F 71 /2 ib");
	instruction[867].dstOperandStr = strdup("xmm1");
	instruction[867].dstOperand = IA_XMM;
	instruction[867].srcOperandStr = strdup("imm8");
	instruction[867].srcOperand = IA_IMM8;
	instruction[867].valid64 = INVALID;
	instruction[867].compatLeg = VALID;
	instruction[867].VMX = 0;
	instruction[867].description = strdup("Shift words in xmm1 right by imm8 while shifting in 0s.");
	instruction[867].prefixStr = strdup("OPSIZEOVR");
	instruction[867].binPrefixLen = 1;
	instruction[867].binPrefix = bpstrndup("\x66", 1);
	instruction[867].binOpcodeLen = 2;
	instruction[867].binOpcode = bpstrndup("\x0F\x71", 2);
	instruction[867].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[867].opcodeOperand[1] = OA_IB;
	instruction[867].opcodeOperandCount = 2;

	// Instruction Struct [868] - PSRLD
	instruction[868].instructionStr = strdup("PSRLD");
	instruction[868].undecodedStr = strdup("0F D2 /r");
	instruction[868].dstOperandStr = strdup("mm");
	instruction[868].dstOperand = IA_MM;
	instruction[868].srcOperandStr = strdup("mm/m64");
	instruction[868].srcOperand = IA_MM_M64;
	instruction[868].valid64 = INVALID;
	instruction[868].compatLeg = VALID;
	instruction[868].VMX = 0;
	instruction[868].description = strdup("Shift doublewords in mm right by amount specified in mm/m64 while shifting in 0s.");
	instruction[868].binPrefixLen = 0;
	instruction[868].binOpcodeLen = 2;
	instruction[868].binOpcode = bpstrndup("\x0F\xD2", 2);
	instruction[868].opcodeOperand[0] = OA_MODRM;
	instruction[868].opcodeOperandCount = 1;

	// Instruction Struct [869] - PSRLD
	instruction[869].instructionStr = strdup("PSRLD");
	instruction[869].undecodedStr = strdup("66 0F D2 /r");
	instruction[869].dstOperandStr = strdup("xmm1");
	instruction[869].dstOperand = IA_XMM;
	instruction[869].srcOperandStr = strdup("xmm2/m128");
	instruction[869].srcOperand = IA_XMM_M128;
	instruction[869].valid64 = INVALID;
	instruction[869].compatLeg = VALID;
	instruction[869].VMX = 0;
	instruction[869].description = strdup("Shift doublewords in xmm1 right by amount specified in xmm2 /m128 while shifting in 0s.");
	instruction[869].prefixStr = strdup("OPSIZEOVR");
	instruction[869].binPrefixLen = 1;
	instruction[869].binPrefix = bpstrndup("\x66", 1);
	instruction[869].binOpcodeLen = 2;
	instruction[869].binOpcode = bpstrndup("\x0F\xD2", 2);
	instruction[869].opcodeOperand[0] = OA_MODRM;
	instruction[869].opcodeOperandCount = 1;

	// Instruction Struct [870] - PSRLD
	instruction[870].instructionStr = strdup("PSRLD");
	instruction[870].undecodedStr = strdup("0F 72 /2 ib");
	instruction[870].dstOperandStr = strdup("mm");
	instruction[870].dstOperand = IA_MM;
	instruction[870].srcOperandStr = strdup("imm8");
	instruction[870].srcOperand = IA_IMM8;
	instruction[870].valid64 = INVALID;
	instruction[870].compatLeg = VALID;
	instruction[870].VMX = 0;
	instruction[870].description = strdup("Shift doublewords in mm right by imm8 while shifting in 0s.");
	instruction[870].binPrefixLen = 0;
	instruction[870].binOpcodeLen = 2;
	instruction[870].binOpcode = bpstrndup("\x0F\x72", 2);
	instruction[870].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[870].opcodeOperand[1] = OA_IB;
	instruction[870].opcodeOperandCount = 2;

	// Instruction Struct [871] - PSRLD
	instruction[871].instructionStr = strdup("PSRLD");
	instruction[871].undecodedStr = strdup("66 0F 72 /2 ib");
	instruction[871].dstOperandStr = strdup("xmm1");
	instruction[871].dstOperand = IA_XMM;
	instruction[871].srcOperandStr = strdup("imm8");
	instruction[871].srcOperand = IA_IMM8;
	instruction[871].valid64 = INVALID;
	instruction[871].compatLeg = VALID;
	instruction[871].VMX = 0;
	instruction[871].description = strdup("Shift doublewords in xmm1 right by imm8 while shifting in 0s.");
	instruction[871].prefixStr = strdup("OPSIZEOVR");
	instruction[871].binPrefixLen = 1;
	instruction[871].binPrefix = bpstrndup("\x66", 1);
	instruction[871].binOpcodeLen = 2;
	instruction[871].binOpcode = bpstrndup("\x0F\x72", 2);
	instruction[871].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[871].opcodeOperand[1] = OA_IB;
	instruction[871].opcodeOperandCount = 2;

	// Instruction Struct [872] - PSRLQ
	instruction[872].instructionStr = strdup("PSRLQ");
	instruction[872].undecodedStr = strdup("0F D3 /r");
	instruction[872].dstOperandStr = strdup("xmm1");
	instruction[872].dstOperand = IA_XMM;
	instruction[872].srcOperandStr = strdup("xmm2/m128");
	instruction[872].srcOperand = IA_XMM_M128;
	instruction[872].valid64 = INVALID;
	instruction[872].compatLeg = VALID;
	instruction[872].VMX = 0;
	instruction[872].description = strdup("Shift quadwords in xmm1 right by amount specified in xmm2/m128 while shifting in 0s.");
	instruction[872].binPrefixLen = 0;
	instruction[872].binOpcodeLen = 2;
	instruction[872].binOpcode = bpstrndup("\x0F\xD3", 2);
	instruction[872].opcodeOperand[0] = OA_MODRM;
	instruction[872].opcodeOperandCount = 1;

	// Instruction Struct [873] - PSRLQ
	instruction[873].instructionStr = strdup("PSRLQ");
	instruction[873].undecodedStr = strdup("0F 73 /2 ib");
	instruction[873].dstOperandStr = strdup("mm");
	instruction[873].dstOperand = IA_MM;
	instruction[873].srcOperandStr = strdup("imm8");
	instruction[873].srcOperand = IA_IMM8;
	instruction[873].valid64 = INVALID;
	instruction[873].compatLeg = VALID;
	instruction[873].VMX = 0;
	instruction[873].description = strdup("Shift mm right by imm8 while shifting in 0s.");
	instruction[873].binPrefixLen = 0;
	instruction[873].binOpcodeLen = 2;
	instruction[873].binOpcode = bpstrndup("\x0F\x73", 2);
	instruction[873].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[873].opcodeOperand[1] = OA_IB;
	instruction[873].opcodeOperandCount = 2;

	// Instruction Struct [874] - PSRLQ
	instruction[874].instructionStr = strdup("PSRLQ");
	instruction[874].undecodedStr = strdup("66 0F 73 /2 ib");
	instruction[874].dstOperandStr = strdup("xmm1");
	instruction[874].dstOperand = IA_XMM;
	instruction[874].srcOperandStr = strdup("imm8");
	instruction[874].srcOperand = IA_IMM8;
	instruction[874].valid64 = INVALID;
	instruction[874].compatLeg = VALID;
	instruction[874].VMX = 0;
	instruction[874].description = strdup("Shift quadwords in xmm1 right by imm8 while shifting in 0s.");
	instruction[874].prefixStr = strdup("OPSIZEOVR");
	instruction[874].binPrefixLen = 1;
	instruction[874].binPrefix = bpstrndup("\x66", 1);
	instruction[874].binOpcodeLen = 2;
	instruction[874].binOpcode = bpstrndup("\x0F\x73", 2);
	instruction[874].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[874].opcodeOperand[1] = OA_IB;
	instruction[874].opcodeOperandCount = 2;

	// Instruction Struct [875] - PSUBB
	instruction[875].instructionStr = strdup("PSUBB");
	instruction[875].undecodedStr = strdup("0F F8 /r");
	instruction[875].dstOperandStr = strdup("mm");
	instruction[875].dstOperand = IA_MM;
	instruction[875].srcOperandStr = strdup("mm/m64");
	instruction[875].srcOperand = IA_MM_M64;
	instruction[875].valid64 = INVALID;
	instruction[875].compatLeg = VALID;
	instruction[875].VMX = 0;
	instruction[875].description = strdup("Subtract packed byte integers in mm/m64 from packed byte integers in mm.");
	instruction[875].binPrefixLen = 0;
	instruction[875].binOpcodeLen = 2;
	instruction[875].binOpcode = bpstrndup("\x0F\xF8", 2);
	instruction[875].opcodeOperand[0] = OA_MODRM;
	instruction[875].opcodeOperandCount = 1;

	// Instruction Struct [876] - PSUBB
	instruction[876].instructionStr = strdup("PSUBB");
	instruction[876].undecodedStr = strdup("66 0F F8 /r");
	instruction[876].dstOperandStr = strdup("xmm1");
	instruction[876].dstOperand = IA_XMM;
	instruction[876].srcOperandStr = strdup("xmm2/m128");
	instruction[876].srcOperand = IA_XMM_M128;
	instruction[876].valid64 = INVALID;
	instruction[876].compatLeg = VALID;
	instruction[876].VMX = 0;
	instruction[876].description = strdup("Subtract packed byte integers in");
	instruction[876].prefixStr = strdup("OPSIZEOVR");
	instruction[876].binPrefixLen = 1;
	instruction[876].binPrefix = bpstrndup("\x66", 1);
	instruction[876].binOpcodeLen = 2;
	instruction[876].binOpcode = bpstrndup("\x0F\xF8", 2);
	instruction[876].opcodeOperand[0] = OA_MODRM;
	instruction[876].opcodeOperandCount = 1;

	// Instruction Struct [877] - PSUBW
	instruction[877].instructionStr = strdup("PSUBW");
	instruction[877].undecodedStr = strdup("0F F9 /r");
	instruction[877].dstOperandStr = strdup("mm");
	instruction[877].dstOperand = IA_MM;
	instruction[877].srcOperandStr = strdup("mm/m64");
	instruction[877].srcOperand = IA_MM_M64;
	instruction[877].valid64 = INVALID;
	instruction[877].compatLeg = VALID;
	instruction[877].VMX = 0;
	instruction[877].description = strdup("Subtract packed word integers in mm/m64 from packed word integers in mm.");
	instruction[877].binPrefixLen = 0;
	instruction[877].binOpcodeLen = 2;
	instruction[877].binOpcode = bpstrndup("\x0F\xF9", 2);
	instruction[877].opcodeOperand[0] = OA_MODRM;
	instruction[877].opcodeOperandCount = 1;

	// Instruction Struct [878] - PSUBW
	instruction[878].instructionStr = strdup("PSUBW");
	instruction[878].undecodedStr = strdup("66 0F F9 /r");
	instruction[878].dstOperandStr = strdup("xmm1");
	instruction[878].dstOperand = IA_XMM;
	instruction[878].srcOperandStr = strdup("xmm2/m128");
	instruction[878].srcOperand = IA_XMM_M128;
	instruction[878].valid64 = INVALID;
	instruction[878].compatLeg = VALID;
	instruction[878].VMX = 0;
	instruction[878].description = strdup("Subtract packed word integers in xmm2/m128 from packed word integers in xmm1.");
	instruction[878].prefixStr = strdup("OPSIZEOVR");
	instruction[878].binPrefixLen = 1;
	instruction[878].binPrefix = bpstrndup("\x66", 1);
	instruction[878].binOpcodeLen = 2;
	instruction[878].binOpcode = bpstrndup("\x0F\xF9", 2);
	instruction[878].opcodeOperand[0] = OA_MODRM;
	instruction[878].opcodeOperandCount = 1;

	// Instruction Struct [879] - PSUBD
	instruction[879].instructionStr = strdup("PSUBD");
	instruction[879].undecodedStr = strdup("0F FA /r");
	instruction[879].dstOperandStr = strdup("mm");
	instruction[879].dstOperand = IA_MM;
	instruction[879].srcOperandStr = strdup("mm/m64");
	instruction[879].srcOperand = IA_MM_M64;
	instruction[879].valid64 = INVALID;
	instruction[879].compatLeg = VALID;
	instruction[879].VMX = 0;
	instruction[879].description = strdup("Subtract packed doubleword integers in mm/m64 from packed doubleword integers in mm.");
	instruction[879].binPrefixLen = 0;
	instruction[879].binOpcodeLen = 2;
	instruction[879].binOpcode = bpstrndup("\x0F\xFA", 2);
	instruction[879].opcodeOperand[0] = OA_MODRM;
	instruction[879].opcodeOperandCount = 1;

	// Instruction Struct [880] - PSUBD
	instruction[880].instructionStr = strdup("PSUBD");
	instruction[880].undecodedStr = strdup("66 0F FA /r");
	instruction[880].dstOperandStr = strdup("xmm1");
	instruction[880].dstOperand = IA_XMM;
	instruction[880].srcOperandStr = strdup("xmm2/m128");
	instruction[880].srcOperand = IA_XMM_M128;
	instruction[880].valid64 = INVALID;
	instruction[880].compatLeg = VALID;
	instruction[880].VMX = 0;
	instruction[880].description = strdup("Subtract packed doubleword integers in xmm2/mem128 from packed doubleword integers in xmm1.");
	instruction[880].prefixStr = strdup("OPSIZEOVR");
	instruction[880].binPrefixLen = 1;
	instruction[880].binPrefix = bpstrndup("\x66", 1);
	instruction[880].binOpcodeLen = 2;
	instruction[880].binOpcode = bpstrndup("\x0F\xFA", 2);
	instruction[880].opcodeOperand[0] = OA_MODRM;
	instruction[880].opcodeOperandCount = 1;

	// Instruction Struct [881] - PSUBQ
	instruction[881].instructionStr = strdup("PSUBQ");
	instruction[881].undecodedStr = strdup("0F FB /r");
	instruction[881].dstOperandStr = strdup("mm1");
	instruction[881].dstOperand = IA_MM;
	instruction[881].srcOperandStr = strdup("mm2/m64");
	instruction[881].srcOperand = IA_MM_M64;
	instruction[881].valid64 = INVALID;
	instruction[881].compatLeg = VALID;
	instruction[881].VMX = 0;
	instruction[881].description = strdup("Subtract quadword integer in mm1 from mm2 /m64.");
	instruction[881].binPrefixLen = 0;
	instruction[881].binOpcodeLen = 2;
	instruction[881].binOpcode = bpstrndup("\x0F\xFB", 2);
	instruction[881].opcodeOperand[0] = OA_MODRM;
	instruction[881].opcodeOperandCount = 1;

	// Instruction Struct [882] - PSUBQ
	instruction[882].instructionStr = strdup("PSUBQ");
	instruction[882].undecodedStr = strdup("66 0F FB /r");
	instruction[882].dstOperandStr = strdup("xmm1");
	instruction[882].dstOperand = IA_XMM;
	instruction[882].srcOperandStr = strdup("xmm2/m128");
	instruction[882].srcOperand = IA_XMM_M128;
	instruction[882].valid64 = INVALID;
	instruction[882].compatLeg = VALID;
	instruction[882].VMX = 0;
	instruction[882].description = strdup("Subtract packed quadword integers in xmm1 from xmm2 /m128.");
	instruction[882].prefixStr = strdup("OPSIZEOVR");
	instruction[882].binPrefixLen = 1;
	instruction[882].binPrefix = bpstrndup("\x66", 1);
	instruction[882].binOpcodeLen = 2;
	instruction[882].binOpcode = bpstrndup("\x0F\xFB", 2);
	instruction[882].opcodeOperand[0] = OA_MODRM;
	instruction[882].opcodeOperandCount = 1;

	// Instruction Struct [883] - PSUBSB
	instruction[883].instructionStr = strdup("PSUBSB");
	instruction[883].undecodedStr = strdup("0F E8 /r");
	instruction[883].dstOperandStr = strdup("mm");
	instruction[883].dstOperand = IA_MM;
	instruction[883].srcOperandStr = strdup("mm/m64");
	instruction[883].srcOperand = IA_MM_M64;
	instruction[883].valid64 = INVALID;
	instruction[883].compatLeg = VALID;
	instruction[883].VMX = 0;
	instruction[883].description = strdup("Subtract signed packed bytes in mm/m64 from signed packed bytes in mm and saturate results.");
	instruction[883].binPrefixLen = 0;
	instruction[883].binOpcodeLen = 2;
	instruction[883].binOpcode = bpstrndup("\x0F\xE8", 2);
	instruction[883].opcodeOperand[0] = OA_MODRM;
	instruction[883].opcodeOperandCount = 1;

	// Instruction Struct [884] - PSUBSB
	instruction[884].instructionStr = strdup("PSUBSB");
	instruction[884].undecodedStr = strdup("66 0F E8 /r");
	instruction[884].dstOperandStr = strdup("xmm1");
	instruction[884].dstOperand = IA_XMM;
	instruction[884].srcOperandStr = strdup("xmm2/m128");
	instruction[884].srcOperand = IA_XMM_M128;
	instruction[884].valid64 = INVALID;
	instruction[884].compatLeg = VALID;
	instruction[884].VMX = 0;
	instruction[884].description = strdup("Subtract packed signed byte integers in xmm2/m128 from packed signed byte integers in xmm1 and saturate results.");
	instruction[884].prefixStr = strdup("OPSIZEOVR");
	instruction[884].binPrefixLen = 1;
	instruction[884].binPrefix = bpstrndup("\x66", 1);
	instruction[884].binOpcodeLen = 2;
	instruction[884].binOpcode = bpstrndup("\x0F\xE8", 2);
	instruction[884].opcodeOperand[0] = OA_MODRM;
	instruction[884].opcodeOperandCount = 1;

	// Instruction Struct [885] - PSUBSW
	instruction[885].instructionStr = strdup("PSUBSW");
	instruction[885].undecodedStr = strdup("0F E9 /r");
	instruction[885].dstOperandStr = strdup("mm");
	instruction[885].dstOperand = IA_MM;
	instruction[885].srcOperandStr = strdup("mm/m64");
	instruction[885].srcOperand = IA_MM_M64;
	instruction[885].valid64 = INVALID;
	instruction[885].compatLeg = VALID;
	instruction[885].VMX = 0;
	instruction[885].description = strdup("Subtract signed packed words in mm/m64 from signed packed words in mm and saturate results.");
	instruction[885].binPrefixLen = 0;
	instruction[885].binOpcodeLen = 2;
	instruction[885].binOpcode = bpstrndup("\x0F\xE9", 2);
	instruction[885].opcodeOperand[0] = OA_MODRM;
	instruction[885].opcodeOperandCount = 1;

	// Instruction Struct [886] - PSUBSW
	instruction[886].instructionStr = strdup("PSUBSW");
	instruction[886].undecodedStr = strdup("66 0F E9 /r");
	instruction[886].dstOperandStr = strdup("xmm1");
	instruction[886].dstOperand = IA_XMM;
	instruction[886].srcOperandStr = strdup("xmm2/m128");
	instruction[886].srcOperand = IA_XMM_M128;
	instruction[886].valid64 = INVALID;
	instruction[886].compatLeg = VALID;
	instruction[886].VMX = 0;
	instruction[886].description = strdup("Subtract packed signed word integers in xmm2/m128 from packed signed word integers in xmm1 and saturate results.");
	instruction[886].prefixStr = strdup("OPSIZEOVR");
	instruction[886].binPrefixLen = 1;
	instruction[886].binPrefix = bpstrndup("\x66", 1);
	instruction[886].binOpcodeLen = 2;
	instruction[886].binOpcode = bpstrndup("\x0F\xE9", 2);
	instruction[886].opcodeOperand[0] = OA_MODRM;
	instruction[886].opcodeOperandCount = 1;

	// Instruction Struct [887] - PSUBUSB
	instruction[887].instructionStr = strdup("PSUBUSB");
	instruction[887].undecodedStr = strdup("0F D8 /r");
	instruction[887].dstOperandStr = strdup("mm");
	instruction[887].dstOperand = IA_MM;
	instruction[887].srcOperandStr = strdup("mm/m64");
	instruction[887].srcOperand = IA_MM_M64;
	instruction[887].valid64 = INVALID;
	instruction[887].compatLeg = VALID;
	instruction[887].VMX = 0;
	instruction[887].description = strdup("Subtract unsigned packed bytes in mm/m64 from unsigned packed bytes in mm and saturate result.");
	instruction[887].binPrefixLen = 0;
	instruction[887].binOpcodeLen = 2;
	instruction[887].binOpcode = bpstrndup("\x0F\xD8", 2);
	instruction[887].opcodeOperand[0] = OA_MODRM;
	instruction[887].opcodeOperandCount = 1;

	// Instruction Struct [888] - PSUBUSB
	instruction[888].instructionStr = strdup("PSUBUSB");
	instruction[888].undecodedStr = strdup("66 0F D8 /r");
	instruction[888].dstOperandStr = strdup("xmm1");
	instruction[888].dstOperand = IA_XMM;
	instruction[888].srcOperandStr = strdup("xmm2/m128");
	instruction[888].srcOperand = IA_XMM_M128;
	instruction[888].valid64 = INVALID;
	instruction[888].compatLeg = VALID;
	instruction[888].VMX = 0;
	instruction[888].description = strdup("Subtract packed unsigned byte integers in xmm2/m128 from packed unsigned byte integers in xmm1 and saturate result.");
	instruction[888].prefixStr = strdup("OPSIZEOVR");
	instruction[888].binPrefixLen = 1;
	instruction[888].binPrefix = bpstrndup("\x66", 1);
	instruction[888].binOpcodeLen = 2;
	instruction[888].binOpcode = bpstrndup("\x0F\xD8", 2);
	instruction[888].opcodeOperand[0] = OA_MODRM;
	instruction[888].opcodeOperandCount = 1;

	// Instruction Struct [889] - PSUBUSW
	instruction[889].instructionStr = strdup("PSUBUSW");
	instruction[889].undecodedStr = strdup("0F D9 /r");
	instruction[889].dstOperandStr = strdup("mm");
	instruction[889].dstOperand = IA_MM;
	instruction[889].srcOperandStr = strdup("mm/m64");
	instruction[889].srcOperand = IA_MM_M64;
	instruction[889].valid64 = INVALID;
	instruction[889].compatLeg = VALID;
	instruction[889].VMX = 0;
	instruction[889].description = strdup("Subtract unsigned packed words in mm/m64 from unsigned packed words in mm and saturate result.");
	instruction[889].binPrefixLen = 0;
	instruction[889].binOpcodeLen = 2;
	instruction[889].binOpcode = bpstrndup("\x0F\xD9", 2);
	instruction[889].opcodeOperand[0] = OA_MODRM;
	instruction[889].opcodeOperandCount = 1;

	// Instruction Struct [890] - PSUBUSW
	instruction[890].instructionStr = strdup("PSUBUSW");
	instruction[890].undecodedStr = strdup("66 0F D9 /r");
	instruction[890].dstOperandStr = strdup("xmm1");
	instruction[890].dstOperand = IA_XMM;
	instruction[890].srcOperandStr = strdup("xmm2/m128");
	instruction[890].srcOperand = IA_XMM_M128;
	instruction[890].valid64 = INVALID;
	instruction[890].compatLeg = VALID;
	instruction[890].VMX = 0;
	instruction[890].description = strdup("Subtract packed unsigned word integers in xmm2/m128 from packed unsigned word integers in xmm1 and saturate result.");
	instruction[890].prefixStr = strdup("OPSIZEOVR");
	instruction[890].binPrefixLen = 1;
	instruction[890].binPrefix = bpstrndup("\x66", 1);
	instruction[890].binOpcodeLen = 2;
	instruction[890].binOpcode = bpstrndup("\x0F\xD9", 2);
	instruction[890].opcodeOperand[0] = OA_MODRM;
	instruction[890].opcodeOperandCount = 1;

	// Instruction Struct [891] - PUNPCKHBW
	instruction[891].instructionStr = strdup("PUNPCKHBW");
	instruction[891].undecodedStr = strdup("0F 68 /r");
	instruction[891].dstOperandStr = strdup("mm");
	instruction[891].dstOperand = IA_MM;
	instruction[891].srcOperandStr = strdup("mm/m64");
	instruction[891].srcOperand = IA_MM_M64;
	instruction[891].valid64 = INVALID;
	instruction[891].compatLeg = VALID;
	instruction[891].VMX = 0;
	instruction[891].description = strdup("Unpack and interleave high-order bytes from mm and mm/m64 into mm.");
	instruction[891].binPrefixLen = 0;
	instruction[891].binOpcodeLen = 2;
	instruction[891].binOpcode = bpstrndup("\x0F\x68", 2);
	instruction[891].opcodeOperand[0] = OA_MODRM;
	instruction[891].opcodeOperandCount = 1;

	// Instruction Struct [892] - PUNPCKHBW
	instruction[892].instructionStr = strdup("PUNPCKHBW");
	instruction[892].undecodedStr = strdup("66 0F 68 /r");
	instruction[892].dstOperandStr = strdup("xmm1");
	instruction[892].dstOperand = IA_XMM;
	instruction[892].srcOperandStr = strdup("xmm2/m128");
	instruction[892].srcOperand = IA_XMM_M128;
	instruction[892].valid64 = INVALID;
	instruction[892].compatLeg = VALID;
	instruction[892].VMX = 0;
	instruction[892].description = strdup("Unpack and interleave high-order bytes from xmm1 and xmm2/m128 into xmm1.");
	instruction[892].prefixStr = strdup("OPSIZEOVR");
	instruction[892].binPrefixLen = 1;
	instruction[892].binPrefix = bpstrndup("\x66", 1);
	instruction[892].binOpcodeLen = 2;
	instruction[892].binOpcode = bpstrndup("\x0F\x68", 2);
	instruction[892].opcodeOperand[0] = OA_MODRM;
	instruction[892].opcodeOperandCount = 1;

	// Instruction Struct [893] - PUNPCKHWD
	instruction[893].instructionStr = strdup("PUNPCKHWD");
	instruction[893].undecodedStr = strdup("0F 69 /r");
	instruction[893].dstOperandStr = strdup("mm");
	instruction[893].dstOperand = IA_MM;
	instruction[893].srcOperandStr = strdup("mm/m64");
	instruction[893].srcOperand = IA_MM_M64;
	instruction[893].valid64 = INVALID;
	instruction[893].compatLeg = VALID;
	instruction[893].VMX = 0;
	instruction[893].description = strdup("Unpack and interleave high-order words from mm and mm/m64 into mm.");
	instruction[893].binPrefixLen = 0;
	instruction[893].binOpcodeLen = 2;
	instruction[893].binOpcode = bpstrndup("\x0F\x69", 2);
	instruction[893].opcodeOperand[0] = OA_MODRM;
	instruction[893].opcodeOperandCount = 1;

	// Instruction Struct [894] - PUNPCKHWD
	instruction[894].instructionStr = strdup("PUNPCKHWD");
	instruction[894].undecodedStr = strdup("66 0F 69 /r");
	instruction[894].dstOperandStr = strdup("xmm1");
	instruction[894].dstOperand = IA_XMM;
	instruction[894].srcOperandStr = strdup("xmm2/m128");
	instruction[894].srcOperand = IA_XMM_M128;
	instruction[894].valid64 = INVALID;
	instruction[894].compatLeg = VALID;
	instruction[894].VMX = 0;
	instruction[894].description = strdup("Unpack and interleave high-order words from xmm1 and xmm2/m128 into xmm1.");
	instruction[894].prefixStr = strdup("OPSIZEOVR");
	instruction[894].binPrefixLen = 1;
	instruction[894].binPrefix = bpstrndup("\x66", 1);
	instruction[894].binOpcodeLen = 2;
	instruction[894].binOpcode = bpstrndup("\x0F\x69", 2);
	instruction[894].opcodeOperand[0] = OA_MODRM;
	instruction[894].opcodeOperandCount = 1;

	// Instruction Struct [895] - PUNPCKHDQ
	instruction[895].instructionStr = strdup("PUNPCKHDQ");
	instruction[895].undecodedStr = strdup("0F 6A /r");
	instruction[895].dstOperandStr = strdup("mm");
	instruction[895].dstOperand = IA_MM;
	instruction[895].srcOperandStr = strdup("mm/m64");
	instruction[895].srcOperand = IA_MM_M64;
	instruction[895].valid64 = INVALID;
	instruction[895].compatLeg = VALID;
	instruction[895].VMX = 0;
	instruction[895].description = strdup("Unpack and interleave high-order doublewords from mm and mm/m64 into mm.");
	instruction[895].binPrefixLen = 0;
	instruction[895].binOpcodeLen = 2;
	instruction[895].binOpcode = bpstrndup("\x0F\x6A", 2);
	instruction[895].opcodeOperand[0] = OA_MODRM;
	instruction[895].opcodeOperandCount = 1;

	// Instruction Struct [896] - PUNPCKHDQ
	instruction[896].instructionStr = strdup("PUNPCKHDQ");
	instruction[896].undecodedStr = strdup("66 0F 6A /r");
	instruction[896].dstOperandStr = strdup("xmm1");
	instruction[896].dstOperand = IA_XMM;
	instruction[896].srcOperandStr = strdup("xmm2/m128");
	instruction[896].srcOperand = IA_XMM_M128;
	instruction[896].valid64 = INVALID;
	instruction[896].compatLeg = VALID;
	instruction[896].VMX = 0;
	instruction[896].description = strdup("Unpack and interleave high-order doublewords from xmm1 and xmm2/m128 into xmm1.");
	instruction[896].prefixStr = strdup("OPSIZEOVR");
	instruction[896].binPrefixLen = 1;
	instruction[896].binPrefix = bpstrndup("\x66", 1);
	instruction[896].binOpcodeLen = 2;
	instruction[896].binOpcode = bpstrndup("\x0F\x6A", 2);
	instruction[896].opcodeOperand[0] = OA_MODRM;
	instruction[896].opcodeOperandCount = 1;

	// Instruction Struct [897] - PUNPCKHQDQ
	instruction[897].instructionStr = strdup("PUNPCKHQDQ");
	instruction[897].undecodedStr = strdup("66 0F 6D /r");
	instruction[897].dstOperandStr = strdup("xmm1");
	instruction[897].dstOperand = IA_XMM;
	instruction[897].srcOperandStr = strdup("xmm2/m128");
	instruction[897].srcOperand = IA_XMM_M128;
	instruction[897].valid64 = INVALID;
	instruction[897].compatLeg = VALID;
	instruction[897].VMX = 0;
	instruction[897].description = strdup("Unpack and interleave high-order quadwords from xmm1 and xmm2/m128 into xmm1.");
	instruction[897].prefixStr = strdup("OPSIZEOVR");
	instruction[897].binPrefixLen = 1;
	instruction[897].binPrefix = bpstrndup("\x66", 1);
	instruction[897].binOpcodeLen = 2;
	instruction[897].binOpcode = bpstrndup("\x0F\x6D", 2);
	instruction[897].opcodeOperand[0] = OA_MODRM;
	instruction[897].opcodeOperandCount = 1;

	// Instruction Struct [898] - PUNPCKLBW
	instruction[898].instructionStr = strdup("PUNPCKLBW");
	instruction[898].undecodedStr = strdup("0F 60 /r");
	instruction[898].dstOperandStr = strdup("mm");
	instruction[898].dstOperand = IA_MM;
	instruction[898].srcOperandStr = strdup("mm/m32");
	instruction[898].srcOperand = IA_MM_M32;
	instruction[898].valid64 = INVALID;
	instruction[898].compatLeg = VALID;
	instruction[898].VMX = 0;
	instruction[898].description = strdup("Interleave low-order bytes from mm and mm/m32 into mm.");
	instruction[898].binPrefixLen = 0;
	instruction[898].binOpcodeLen = 2;
	instruction[898].binOpcode = bpstrndup("\x0F\x60", 2);
	instruction[898].opcodeOperand[0] = OA_MODRM;
	instruction[898].opcodeOperandCount = 1;

	// Instruction Struct [899] - PUNPCKLBW
	instruction[899].instructionStr = strdup("PUNPCKLBW");
	instruction[899].undecodedStr = strdup("66 0F 60 /r");
	instruction[899].dstOperandStr = strdup("xmm1");
	instruction[899].dstOperand = IA_XMM;
	instruction[899].srcOperandStr = strdup("xmm2/m128");
	instruction[899].srcOperand = IA_XMM_M128;
	instruction[899].valid64 = INVALID;
	instruction[899].compatLeg = VALID;
	instruction[899].VMX = 0;
	instruction[899].description = strdup("Interleave low-order bytes from xmm1 and xmm2/m128 into xmm1.");
	instruction[899].prefixStr = strdup("OPSIZEOVR");
	instruction[899].binPrefixLen = 1;
	instruction[899].binPrefix = bpstrndup("\x66", 1);
	instruction[899].binOpcodeLen = 2;
	instruction[899].binOpcode = bpstrndup("\x0F\x60", 2);
	instruction[899].opcodeOperand[0] = OA_MODRM;
	instruction[899].opcodeOperandCount = 1;

	// Instruction Struct [900] - PUNPCKLWD
	instruction[900].instructionStr = strdup("PUNPCKLWD");
	instruction[900].undecodedStr = strdup("0F 61 /r");
	instruction[900].dstOperandStr = strdup("mm");
	instruction[900].dstOperand = IA_MM;
	instruction[900].srcOperandStr = strdup("mm/m32");
	instruction[900].srcOperand = IA_MM_M32;
	instruction[900].valid64 = INVALID;
	instruction[900].compatLeg = VALID;
	instruction[900].VMX = 0;
	instruction[900].description = strdup("Interleave low-order words from mm and mm/m32 into mm.");
	instruction[900].binPrefixLen = 0;
	instruction[900].binOpcodeLen = 2;
	instruction[900].binOpcode = bpstrndup("\x0F\x61", 2);
	instruction[900].opcodeOperand[0] = OA_MODRM;
	instruction[900].opcodeOperandCount = 1;

	// Instruction Struct [901] - PUNPCKLWD
	instruction[901].instructionStr = strdup("PUNPCKLWD");
	instruction[901].undecodedStr = strdup("66 0F 61 /r");
	instruction[901].dstOperandStr = strdup("xmm1");
	instruction[901].dstOperand = IA_XMM;
	instruction[901].srcOperandStr = strdup("xmm2/m128");
	instruction[901].srcOperand = IA_XMM_M128;
	instruction[901].valid64 = INVALID;
	instruction[901].compatLeg = VALID;
	instruction[901].VMX = 0;
	instruction[901].description = strdup("Interleave low-order words from xmm1 and xmm2/m128 into xmm1.");
	instruction[901].prefixStr = strdup("OPSIZEOVR");
	instruction[901].binPrefixLen = 1;
	instruction[901].binPrefix = bpstrndup("\x66", 1);
	instruction[901].binOpcodeLen = 2;
	instruction[901].binOpcode = bpstrndup("\x0F\x61", 2);
	instruction[901].opcodeOperand[0] = OA_MODRM;
	instruction[901].opcodeOperandCount = 1;

	// Instruction Struct [902] - PUNPCKLDQ
	instruction[902].instructionStr = strdup("PUNPCKLDQ");
	instruction[902].undecodedStr = strdup("0F 62 /r");
	instruction[902].dstOperandStr = strdup("mm");
	instruction[902].dstOperand = IA_MM;
	instruction[902].srcOperandStr = strdup("mm/m32");
	instruction[902].srcOperand = IA_MM_M32;
	instruction[902].valid64 = INVALID;
	instruction[902].compatLeg = VALID;
	instruction[902].VMX = 0;
	instruction[902].description = strdup("Interleave low-order doublewords from mm and mm/m32 into mm.");
	instruction[902].binPrefixLen = 0;
	instruction[902].binOpcodeLen = 2;
	instruction[902].binOpcode = bpstrndup("\x0F\x62", 2);
	instruction[902].opcodeOperand[0] = OA_MODRM;
	instruction[902].opcodeOperandCount = 1;

	// Instruction Struct [903] - PUNPCKLDQ
	instruction[903].instructionStr = strdup("PUNPCKLDQ");
	instruction[903].undecodedStr = strdup("66 0F 62 /r");
	instruction[903].dstOperandStr = strdup("xmm1");
	instruction[903].dstOperand = IA_XMM;
	instruction[903].srcOperandStr = strdup("xmm2/m128");
	instruction[903].srcOperand = IA_XMM_M128;
	instruction[903].valid64 = INVALID;
	instruction[903].compatLeg = VALID;
	instruction[903].VMX = 0;
	instruction[903].description = strdup("Interleave low-order doublewords from xmm1 and xmm2/m128 into xmm1.");
	instruction[903].prefixStr = strdup("OPSIZEOVR");
	instruction[903].binPrefixLen = 1;
	instruction[903].binPrefix = bpstrndup("\x66", 1);
	instruction[903].binOpcodeLen = 2;
	instruction[903].binOpcode = bpstrndup("\x0F\x62", 2);
	instruction[903].opcodeOperand[0] = OA_MODRM;
	instruction[903].opcodeOperandCount = 1;

	// Instruction Struct [904] - PUNPCKLQDQ
	instruction[904].instructionStr = strdup("PUNPCKLQDQ");
	instruction[904].undecodedStr = strdup("66 0F 6C /r");
	instruction[904].dstOperandStr = strdup("xmm1");
	instruction[904].dstOperand = IA_XMM;
	instruction[904].srcOperandStr = strdup("xmm2/m128");
	instruction[904].srcOperand = IA_XMM_M128;
	instruction[904].valid64 = INVALID;
	instruction[904].compatLeg = VALID;
	instruction[904].VMX = 0;
	instruction[904].description = strdup("Interleave low-order quadword from xmm1 and xmm2/m128 into xmm1 register.");
	instruction[904].prefixStr = strdup("OPSIZEOVR");
	instruction[904].binPrefixLen = 1;
	instruction[904].binPrefix = bpstrndup("\x66", 1);
	instruction[904].binOpcodeLen = 2;
	instruction[904].binOpcode = bpstrndup("\x0F\x6C", 2);
	instruction[904].opcodeOperand[0] = OA_MODRM;
	instruction[904].opcodeOperandCount = 1;

	// Instruction Struct [905] - PUSH
	instruction[905].instructionStr = strdup("PUSH");
	instruction[905].undecodedStr = strdup("FF /6");
	instruction[905].dstOperandStr = strdup("r/m16");
	instruction[905].dstOperand = IA_RM16;
	instruction[905].valid64 = INVALID;
	instruction[905].compatLeg = VALID;
	instruction[905].VMX = 0;
	instruction[905].description = strdup("Push r/m16.");
	instruction[905].binPrefixLen = 0;
	instruction[905].binOpcodeLen = 1;
	instruction[905].binOpcode = bpstrndup("\xFF", 1);
	instruction[905].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[905].opcodeOperandCount = 1;

	// Instruction Struct [906] - PUSH
	instruction[906].instructionStr = strdup("PUSH");
	instruction[906].undecodedStr = strdup("FF /6");
	instruction[906].dstOperandStr = strdup("r/m32");
	instruction[906].dstOperand = IA_RM32;
	instruction[906].valid64 = INVALID;
	instruction[906].compatLeg = VALID;
	instruction[906].VMX = 0;
	instruction[906].description = strdup("Push r/m32.");
	instruction[906].binPrefixLen = 0;
	instruction[906].binOpcodeLen = 1;
	instruction[906].binOpcode = bpstrndup("\xFF", 1);
	instruction[906].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[906].opcodeOperandCount = 1;

	// Instruction Struct [907] - PUSH
	instruction[907].instructionStr = strdup("PUSH");
	instruction[907].undecodedStr = strdup("FF /6");
	instruction[907].dstOperandStr = strdup("r/m64");
	instruction[907].dstOperand = IA_RM64;
	instruction[907].valid64 = INVALID;
	instruction[907].compatLeg = INVALID;
	instruction[907].VMX = 0;
	instruction[907].description = strdup("Push r/m64. Default operand size 64- bits.");
	instruction[907].binPrefixLen = 0;
	instruction[907].binOpcodeLen = 1;
	instruction[907].binOpcode = bpstrndup("\xFF", 1);
	instruction[907].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[907].opcodeOperandCount = 1;

	// Instruction Struct [908] - PUSH
	instruction[908].instructionStr = strdup("PUSH");
	instruction[908].undecodedStr = strdup("50 +rw");
	instruction[908].dstOperandStr = strdup("r16");
	instruction[908].dstOperand = IA_R16;
	instruction[908].valid64 = INVALID;
	instruction[908].compatLeg = VALID;
	instruction[908].VMX = 0;
	instruction[908].description = strdup("Push r16.");
	instruction[908].binPrefixLen = 0;
	instruction[908].binOpcodeLen = 1;
	instruction[908].binOpcode = bpstrndup("\x50", 1);
	instruction[908].opcodeOperand[0] = OA_RW;
	instruction[908].opcodeOperandCount = 1;

	// Instruction Struct [909] - PUSH
	instruction[909].instructionStr = strdup("PUSH");
	instruction[909].undecodedStr = strdup("50 +rd");
	instruction[909].dstOperandStr = strdup("r16");
	instruction[909].dstOperand = IA_R16;
	instruction[909].valid64 = INVALID;
	instruction[909].compatLeg = VALID;
	instruction[909].VMX = 0;
	instruction[909].description = strdup("Push r16.");
	instruction[909].binPrefixLen = 0;
	instruction[909].binOpcodeLen = 1;
	instruction[909].binOpcode = bpstrndup("\x50", 1);
	instruction[909].opcodeOperand[0] = OA_RD;
	instruction[909].opcodeOperandCount = 1;

	// Instruction Struct [910] - PUSH
	instruction[910].instructionStr = strdup("PUSH");
	instruction[910].undecodedStr = strdup("50 +rd");
	instruction[910].dstOperandStr = strdup("r32");
	instruction[910].dstOperand = IA_R32;
	instruction[910].valid64 = INVALID;
	instruction[910].compatLeg = VALID;
	instruction[910].VMX = 0;
	instruction[910].description = strdup("Push r32.");
	instruction[910].binPrefixLen = 0;
	instruction[910].binOpcodeLen = 1;
	instruction[910].binOpcode = bpstrndup("\x50", 1);
	instruction[910].opcodeOperand[0] = OA_RD;
	instruction[910].opcodeOperandCount = 1;

	// Instruction Struct [911] - PUSH
	instruction[911].instructionStr = strdup("PUSH");
	instruction[911].undecodedStr = strdup("50 +rd");
	instruction[911].dstOperandStr = strdup("r64");
	instruction[911].dstOperand = IA_R64;
	instruction[911].valid64 = INVALID;
	instruction[911].compatLeg = INVALID;
	instruction[911].VMX = 0;
	instruction[911].description = strdup("Push r64. Default operand size 64-bits.");
	instruction[911].binPrefixLen = 0;
	instruction[911].binOpcodeLen = 1;
	instruction[911].binOpcode = bpstrndup("\x50", 1);
	instruction[911].opcodeOperand[0] = OA_RD;
	instruction[911].opcodeOperandCount = 1;

	// Instruction Struct [912] - PUSH
	instruction[912].instructionStr = strdup("PUSH");
	instruction[912].undecodedStr = strdup("6A");
	instruction[912].dstOperandStr = strdup("imm8");
	instruction[912].dstOperand = IA_IMM8;
	instruction[912].valid64 = INVALID;
	instruction[912].compatLeg = VALID;
	instruction[912].VMX = 0;
	instruction[912].description = strdup("Push sign-extended imm8. Stack pointer is incremented by the size of stack pointer.");
	instruction[912].binPrefixLen = 0;
	instruction[912].binOpcodeLen = 1;
	instruction[912].binOpcode = bpstrndup("\x6A", 1);
	instruction[912].opcodeOperandCount = 0;

	// Instruction Struct [913] - PUSH
	instruction[913].instructionStr = strdup("PUSH");
	instruction[913].undecodedStr = strdup("68");
	instruction[913].dstOperandStr = strdup("imm16");
	instruction[913].dstOperand = IA_IMM16;
	instruction[913].valid64 = INVALID;
	instruction[913].compatLeg = VALID;
	instruction[913].VMX = 0;
	instruction[913].description = strdup("Push sign-extended imm16. Stack pointer is incremented by the size of stack pointer.");
	instruction[913].binPrefixLen = 0;
	instruction[913].binOpcodeLen = 1;
	instruction[913].binOpcode = bpstrndup("\x68", 1);
	instruction[913].opcodeOperandCount = 0;

	// Instruction Struct [914] - PUSH
	instruction[914].instructionStr = strdup("PUSH");
	instruction[914].undecodedStr = strdup("68");
	instruction[914].dstOperandStr = strdup("imm32");
	instruction[914].dstOperand = IA_IMM32;
	instruction[914].valid64 = INVALID;
	instruction[914].compatLeg = VALID;
	instruction[914].VMX = 0;
	instruction[914].description = strdup("Push sign-extended imm32. Stack pointer is incremented by the size of stack pointer.");
	instruction[914].binPrefixLen = 0;
	instruction[914].binOpcodeLen = 1;
	instruction[914].binOpcode = bpstrndup("\x68", 1);
	instruction[914].opcodeOperandCount = 0;

	// Instruction Struct [915] - PUSH
	instruction[915].instructionStr = strdup("PUSH");
	instruction[915].undecodedStr = strdup("0E");
	instruction[915].dstOperandStr = strdup("CS");
	instruction[915].dstOperand = IA_CS;
	instruction[915].valid64 = INVALID;
	instruction[915].compatLeg = VALID;
	instruction[915].VMX = 0;
	instruction[915].description = strdup("Push CS.");
	instruction[915].binPrefixLen = 0;
	instruction[915].binOpcodeLen = 1;
	instruction[915].binOpcode = bpstrndup("\x0E", 1);
	instruction[915].opcodeOperandCount = 0;

	// Instruction Struct [916] - PUSH
	instruction[916].instructionStr = strdup("PUSH");
	instruction[916].undecodedStr = strdup("16");
	instruction[916].dstOperandStr = strdup("SS");
	instruction[916].dstOperand = IA_SS;
	instruction[916].valid64 = INVALID;
	instruction[916].compatLeg = VALID;
	instruction[916].VMX = 0;
	instruction[916].description = strdup("Push SS.");
	instruction[916].binPrefixLen = 0;
	instruction[916].binOpcodeLen = 1;
	instruction[916].binOpcode = bpstrndup("\x16", 1);
	instruction[916].opcodeOperandCount = 0;

	// Instruction Struct [917] - PUSH
	instruction[917].instructionStr = strdup("PUSH");
	instruction[917].undecodedStr = strdup("1E");
	instruction[917].dstOperandStr = strdup("DS");
	instruction[917].dstOperand = IA_DS;
	instruction[917].valid64 = INVALID;
	instruction[917].compatLeg = VALID;
	instruction[917].VMX = 0;
	instruction[917].description = strdup("Push DS.");
	instruction[917].binPrefixLen = 0;
	instruction[917].binOpcodeLen = 1;
	instruction[917].binOpcode = bpstrndup("\x1E", 1);
	instruction[917].opcodeOperandCount = 0;

	// Instruction Struct [918] - PUSH
	instruction[918].instructionStr = strdup("PUSH");
	instruction[918].undecodedStr = strdup("06");
	instruction[918].dstOperandStr = strdup("ES");
	instruction[918].dstOperand = IA_ES;
	instruction[918].valid64 = INVALID;
	instruction[918].compatLeg = VALID;
	instruction[918].VMX = 0;
	instruction[918].description = strdup("Push ES.");
	instruction[918].binPrefixLen = 0;
	instruction[918].binOpcodeLen = 1;
	instruction[918].binOpcode = bpstrndup("\x06", 1);
	instruction[918].opcodeOperandCount = 0;

	// Instruction Struct [919] - PUSH
	instruction[919].instructionStr = strdup("PUSH");
	instruction[919].undecodedStr = strdup("0F A0");
	instruction[919].dstOperandStr = strdup("FS");
	instruction[919].dstOperand = IA_FS;
	instruction[919].valid64 = INVALID;
	instruction[919].compatLeg = VALID;
	instruction[919].VMX = 0;
	instruction[919].description = strdup("Push FS and decrement stack pointer by 16 bits.");
	instruction[919].binPrefixLen = 0;
	instruction[919].binOpcodeLen = 2;
	instruction[919].binOpcode = bpstrndup("\x0F\xA0", 2);
	instruction[919].opcodeOperandCount = 0;

	// Instruction Struct [920] - PUSH
	instruction[920].instructionStr = strdup("PUSH");
	instruction[920].undecodedStr = strdup("0F A0");
	instruction[920].dstOperandStr = strdup("FS");
	instruction[920].dstOperand = IA_FS;
	instruction[920].valid64 = INVALID;
	instruction[920].compatLeg = VALID;
	instruction[920].VMX = 0;
	instruction[920].description = strdup("Push FS and decrement stack pointer by 32 bits.");
	instruction[920].binPrefixLen = 0;
	instruction[920].binOpcodeLen = 2;
	instruction[920].binOpcode = bpstrndup("\x0F\xA0", 2);
	instruction[920].opcodeOperandCount = 0;

	// Instruction Struct [921] - PUSH
	instruction[921].instructionStr = strdup("PUSH");
	instruction[921].undecodedStr = strdup("0F A0");
	instruction[921].dstOperandStr = strdup("FS");
	instruction[921].dstOperand = IA_FS;
	instruction[921].valid64 = INVALID;
	instruction[921].compatLeg = INVALID;
	instruction[921].VMX = 0;
	instruction[921].description = strdup("Push FS. Default operand size 64-bits. (66H override causes 16-bit operation).");
	instruction[921].binPrefixLen = 0;
	instruction[921].binOpcodeLen = 2;
	instruction[921].binOpcode = bpstrndup("\x0F\xA0", 2);
	instruction[921].opcodeOperandCount = 0;

	// Instruction Struct [922] - PUSH
	instruction[922].instructionStr = strdup("PUSH");
	instruction[922].undecodedStr = strdup("0F A8");
	instruction[922].dstOperandStr = strdup("GS");
	instruction[922].dstOperand = IA_GS;
	instruction[922].valid64 = INVALID;
	instruction[922].compatLeg = VALID;
	instruction[922].VMX = 0;
	instruction[922].description = strdup("Push GS and decrement stack pointer");
	instruction[922].binPrefixLen = 0;
	instruction[922].binOpcodeLen = 2;
	instruction[922].binOpcode = bpstrndup("\x0F\xA8", 2);
	instruction[922].opcodeOperandCount = 0;

	// Instruction Struct [923] - PUSH
	instruction[923].instructionStr = strdup("PUSH");
	instruction[923].undecodedStr = strdup("0F A8");
	instruction[923].dstOperandStr = strdup("GS");
	instruction[923].dstOperand = IA_GS;
	instruction[923].valid64 = INVALID;
	instruction[923].compatLeg = VALID;
	instruction[923].VMX = 0;
	instruction[923].description = strdup("Push GS and decrement stack pointer by 32 bits.");
	instruction[923].binPrefixLen = 0;
	instruction[923].binOpcodeLen = 2;
	instruction[923].binOpcode = bpstrndup("\x0F\xA8", 2);
	instruction[923].opcodeOperandCount = 0;

	// Instruction Struct [924] - PUSH
	instruction[924].instructionStr = strdup("PUSH");
	instruction[924].undecodedStr = strdup("0F A8");
	instruction[924].dstOperandStr = strdup("GS");
	instruction[924].dstOperand = IA_GS;
	instruction[924].valid64 = INVALID;
	instruction[924].compatLeg = INVALID;
	instruction[924].VMX = 0;
	instruction[924].description = strdup("Push GS, default operand size 64-bits. (66H override causes 16-bit operation).");
	instruction[924].binPrefixLen = 0;
	instruction[924].binOpcodeLen = 2;
	instruction[924].binOpcode = bpstrndup("\x0F\xA8", 2);
	instruction[924].opcodeOperandCount = 0;

	// Instruction Struct [925] - PUSHA
	instruction[925].instructionStr = strdup("PUSHA");
	instruction[925].undecodedStr = strdup("60");
	instruction[925].valid64 = INVALID;
	instruction[925].compatLeg = VALID;
	instruction[925].VMX = 0;
	instruction[925].description = strdup("Push AX, CX, DX, BX, original SP, BP, SI, and DI.");
	instruction[925].binPrefixLen = 0;
	instruction[925].binOpcodeLen = 1;
	instruction[925].binOpcode = bpstrndup("\x60", 1);
	instruction[925].opcodeOperandCount = 0;

	// Instruction Struct [926] - PUSHAD
	instruction[926].instructionStr = strdup("PUSHAD");
	instruction[926].undecodedStr = strdup("60");
	instruction[926].valid64 = INVALID;
	instruction[926].compatLeg = VALID;
	instruction[926].VMX = 0;
	instruction[926].description = strdup("Push EAX, ECX, EDX, EBX, original ESP, EBP, ESI, and EDI.");
	instruction[926].binPrefixLen = 0;
	instruction[926].binOpcodeLen = 1;
	instruction[926].binOpcode = bpstrndup("\x60", 1);
	instruction[926].opcodeOperandCount = 0;

	// Instruction Struct [927] - PUSHF
	instruction[927].instructionStr = strdup("PUSHF");
	instruction[927].undecodedStr = strdup("9C");
	instruction[927].valid64 = INVALID;
	instruction[927].compatLeg = VALID;
	instruction[927].VMX = 0;
	instruction[927].description = strdup("Push lower 16 bits of EFLAGS.");
	instruction[927].binPrefixLen = 0;
	instruction[927].binOpcodeLen = 1;
	instruction[927].binOpcode = bpstrndup("\x9C", 1);
	instruction[927].opcodeOperandCount = 0;

	// Instruction Struct [928] - PUSHFD
	instruction[928].instructionStr = strdup("PUSHFD");
	instruction[928].undecodedStr = strdup("9C");
	instruction[928].valid64 = INVALID;
	instruction[928].compatLeg = VALID;
	instruction[928].VMX = 0;
	instruction[928].description = strdup("Push EFLAGS.");
	instruction[928].binPrefixLen = 0;
	instruction[928].binOpcodeLen = 1;
	instruction[928].binOpcode = bpstrndup("\x9C", 1);
	instruction[928].opcodeOperandCount = 0;

	// Instruction Struct [929] - PUSHFQ
	instruction[929].instructionStr = strdup("PUSHFQ");
	instruction[929].undecodedStr = strdup("9C");
	instruction[929].valid64 = INVALID;
	instruction[929].compatLeg = INVALID;
	instruction[929].VMX = 0;
	instruction[929].description = strdup("Push RFLAGS.");
	instruction[929].binPrefixLen = 0;
	instruction[929].binOpcodeLen = 1;
	instruction[929].binOpcode = bpstrndup("\x9C", 1);
	instruction[929].opcodeOperandCount = 0;

	// Instruction Struct [930] - PXOR
	instruction[930].instructionStr = strdup("PXOR");
	instruction[930].undecodedStr = strdup("0F EF /r");
	instruction[930].dstOperandStr = strdup("mm");
	instruction[930].dstOperand = IA_MM;
	instruction[930].srcOperandStr = strdup("mm/m64");
	instruction[930].srcOperand = IA_MM_M64;
	instruction[930].valid64 = INVALID;
	instruction[930].compatLeg = VALID;
	instruction[930].VMX = 0;
	instruction[930].description = strdup("Bitwise XOR of mm/m64 and mm.");
	instruction[930].binPrefixLen = 0;
	instruction[930].binOpcodeLen = 2;
	instruction[930].binOpcode = bpstrndup("\x0F\xEF", 2);
	instruction[930].opcodeOperand[0] = OA_MODRM;
	instruction[930].opcodeOperandCount = 1;

	// Instruction Struct [931] - PXOR
	instruction[931].instructionStr = strdup("PXOR");
	instruction[931].undecodedStr = strdup("66 0F EF /r");
	instruction[931].dstOperandStr = strdup("xmm1");
	instruction[931].dstOperand = IA_XMM;
	instruction[931].srcOperandStr = strdup("xmm2/m128");
	instruction[931].srcOperand = IA_XMM_M128;
	instruction[931].valid64 = INVALID;
	instruction[931].compatLeg = VALID;
	instruction[931].VMX = 0;
	instruction[931].description = strdup("Bitwise XOR of xmm2/m128 and xmm1.");
	instruction[931].prefixStr = strdup("OPSIZEOVR");
	instruction[931].binPrefixLen = 1;
	instruction[931].binPrefix = bpstrndup("\x66", 1);
	instruction[931].binOpcodeLen = 2;
	instruction[931].binOpcode = bpstrndup("\x0F\xEF", 2);
	instruction[931].opcodeOperand[0] = OA_MODRM;
	instruction[931].opcodeOperandCount = 1;

	// Instruction Struct [932] - RCL
	instruction[932].instructionStr = strdup("RCL");
	instruction[932].undecodedStr = strdup("D0 /2");
	instruction[932].dstOperandStr = strdup("r/m8");
	instruction[932].dstOperand = IA_RM8;
	instruction[932].srcOperandStr = strdup("1");
	instruction[932].srcOperand = IA_1;
	instruction[932].valid64 = INVALID;
	instruction[932].compatLeg = VALID;
	instruction[932].VMX = 0;
	instruction[932].description = strdup("Rotate 9 bits (CF, r/m8) left once.");
	instruction[932].binPrefixLen = 0;
	instruction[932].binOpcodeLen = 1;
	instruction[932].binOpcode = bpstrndup("\xD0", 1);
	instruction[932].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[932].opcodeOperandCount = 1;

	// Instruction Struct [933] - RCL
	instruction[933].instructionStr = strdup("RCL");
	instruction[933].undecodedStr = strdup("D2 /2");
	instruction[933].dstOperandStr = strdup("r/m8");
	instruction[933].dstOperand = IA_RM8;
	instruction[933].srcOperandStr = strdup("CL");
	instruction[933].srcOperand = IA_CL;
	instruction[933].valid64 = INVALID;
	instruction[933].compatLeg = VALID;
	instruction[933].VMX = 0;
	instruction[933].description = strdup("Rotate 9 bits (CF, r/m8) left CL times.");
	instruction[933].binPrefixLen = 0;
	instruction[933].binOpcodeLen = 1;
	instruction[933].binOpcode = bpstrndup("\xD2", 1);
	instruction[933].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[933].opcodeOperandCount = 1;

	// Instruction Struct [934] - RCL
	instruction[934].instructionStr = strdup("RCL");
	instruction[934].undecodedStr = strdup("C0 /2 ib");
	instruction[934].dstOperandStr = strdup("r/m8");
	instruction[934].dstOperand = IA_RM8;
	instruction[934].srcOperandStr = strdup("imm8");
	instruction[934].srcOperand = IA_IMM8;
	instruction[934].valid64 = INVALID;
	instruction[934].compatLeg = VALID;
	instruction[934].VMX = 0;
	instruction[934].description = strdup("Rotate 9 bits (CF, r/m8) left imm8 times.");
	instruction[934].binPrefixLen = 0;
	instruction[934].binOpcodeLen = 1;
	instruction[934].binOpcode = bpstrndup("\xC0", 1);
	instruction[934].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[934].opcodeOperand[1] = OA_IB;
	instruction[934].opcodeOperandCount = 2;

	// Instruction Struct [935] - RCL
	instruction[935].instructionStr = strdup("RCL");
	instruction[935].undecodedStr = strdup("D1 /2");
	instruction[935].dstOperandStr = strdup("r/m16");
	instruction[935].dstOperand = IA_RM16;
	instruction[935].srcOperandStr = strdup("1");
	instruction[935].srcOperand = IA_1;
	instruction[935].valid64 = INVALID;
	instruction[935].compatLeg = VALID;
	instruction[935].VMX = 0;
	instruction[935].description = strdup("Rotate 17 bits (CF, r/m16) left once.");
	instruction[935].binPrefixLen = 0;
	instruction[935].binOpcodeLen = 1;
	instruction[935].binOpcode = bpstrndup("\xD1", 1);
	instruction[935].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[935].opcodeOperandCount = 1;

	// Instruction Struct [936] - RCL
	instruction[936].instructionStr = strdup("RCL");
	instruction[936].undecodedStr = strdup("D3 /2");
	instruction[936].dstOperandStr = strdup("r/m16");
	instruction[936].dstOperand = IA_RM16;
	instruction[936].srcOperandStr = strdup("CL");
	instruction[936].srcOperand = IA_CL;
	instruction[936].valid64 = INVALID;
	instruction[936].compatLeg = VALID;
	instruction[936].VMX = 0;
	instruction[936].description = strdup("Rotate 17 bits (CF, r/m16) left CL times.");
	instruction[936].binPrefixLen = 0;
	instruction[936].binOpcodeLen = 1;
	instruction[936].binOpcode = bpstrndup("\xD3", 1);
	instruction[936].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[936].opcodeOperandCount = 1;

	// Instruction Struct [937] - RCL
	instruction[937].instructionStr = strdup("RCL");
	instruction[937].undecodedStr = strdup("C1 /2 ib");
	instruction[937].dstOperandStr = strdup("r/m16");
	instruction[937].dstOperand = IA_RM16;
	instruction[937].srcOperandStr = strdup("imm8");
	instruction[937].srcOperand = IA_IMM8;
	instruction[937].valid64 = INVALID;
	instruction[937].compatLeg = VALID;
	instruction[937].VMX = 0;
	instruction[937].description = strdup("Rotate 17 bits (CF, r/m16) left imm8 times.");
	instruction[937].binPrefixLen = 0;
	instruction[937].binOpcodeLen = 1;
	instruction[937].binOpcode = bpstrndup("\xC1", 1);
	instruction[937].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[937].opcodeOperand[1] = OA_IB;
	instruction[937].opcodeOperandCount = 2;

	// Instruction Struct [938] - RCL
	instruction[938].instructionStr = strdup("RCL");
	instruction[938].undecodedStr = strdup("D1 /2");
	instruction[938].dstOperandStr = strdup("r/m32");
	instruction[938].dstOperand = IA_RM32;
	instruction[938].srcOperandStr = strdup("1");
	instruction[938].srcOperand = IA_1;
	instruction[938].valid64 = INVALID;
	instruction[938].compatLeg = VALID;
	instruction[938].VMX = 0;
	instruction[938].description = strdup("Rotate 33 bits (CF, r/m32) left once.");
	instruction[938].binPrefixLen = 0;
	instruction[938].binOpcodeLen = 1;
	instruction[938].binOpcode = bpstrndup("\xD1", 1);
	instruction[938].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[938].opcodeOperandCount = 1;

	// Instruction Struct [939] - RCL
	instruction[939].instructionStr = strdup("RCL");
	instruction[939].undecodedStr = strdup("D3 /2");
	instruction[939].dstOperandStr = strdup("r/m32");
	instruction[939].dstOperand = IA_RM32;
	instruction[939].srcOperandStr = strdup("CL");
	instruction[939].srcOperand = IA_CL;
	instruction[939].valid64 = INVALID;
	instruction[939].compatLeg = VALID;
	instruction[939].VMX = 0;
	instruction[939].description = strdup("Rotate 33 bits (CF, r/m32) left CL times.");
	instruction[939].binPrefixLen = 0;
	instruction[939].binOpcodeLen = 1;
	instruction[939].binOpcode = bpstrndup("\xD3", 1);
	instruction[939].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[939].opcodeOperandCount = 1;

	// Instruction Struct [940] - RCL
	instruction[940].instructionStr = strdup("RCL");
	instruction[940].undecodedStr = strdup("C1 /2 ib");
	instruction[940].dstOperandStr = strdup("r/m32");
	instruction[940].dstOperand = IA_RM32;
	instruction[940].srcOperandStr = strdup("imm8");
	instruction[940].srcOperand = IA_IMM8;
	instruction[940].valid64 = INVALID;
	instruction[940].compatLeg = VALID;
	instruction[940].VMX = 0;
	instruction[940].description = strdup("Rotate 33 bits (CF, r/m32) left imm8 times.");
	instruction[940].binPrefixLen = 0;
	instruction[940].binOpcodeLen = 1;
	instruction[940].binOpcode = bpstrndup("\xC1", 1);
	instruction[940].opcodeOperand[0] = OA_RM_ONLY_2;
	instruction[940].opcodeOperand[1] = OA_IB;
	instruction[940].opcodeOperandCount = 2;

	// Instruction Struct [941] - RCR
	instruction[941].instructionStr = strdup("RCR");
	instruction[941].undecodedStr = strdup("D0 /3");
	instruction[941].dstOperandStr = strdup("r/m8");
	instruction[941].dstOperand = IA_RM8;
	instruction[941].srcOperandStr = strdup("1");
	instruction[941].srcOperand = IA_1;
	instruction[941].valid64 = INVALID;
	instruction[941].compatLeg = VALID;
	instruction[941].VMX = 0;
	instruction[941].description = strdup("Rotate 9 bits (CF, r/m8) right once.");
	instruction[941].binPrefixLen = 0;
	instruction[941].binOpcodeLen = 1;
	instruction[941].binOpcode = bpstrndup("\xD0", 1);
	instruction[941].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[941].opcodeOperandCount = 1;

	// Instruction Struct [942] - RCR
	instruction[942].instructionStr = strdup("RCR");
	instruction[942].undecodedStr = strdup("D2 /3");
	instruction[942].dstOperandStr = strdup("r/m8");
	instruction[942].dstOperand = IA_RM8;
	instruction[942].srcOperandStr = strdup("CL");
	instruction[942].srcOperand = IA_CL;
	instruction[942].valid64 = INVALID;
	instruction[942].compatLeg = VALID;
	instruction[942].VMX = 0;
	instruction[942].description = strdup("Rotate 9 bits (CF, r/m8) right CL times.");
	instruction[942].binPrefixLen = 0;
	instruction[942].binOpcodeLen = 1;
	instruction[942].binOpcode = bpstrndup("\xD2", 1);
	instruction[942].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[942].opcodeOperandCount = 1;

	// Instruction Struct [943] - RCR
	instruction[943].instructionStr = strdup("RCR ");
	instruction[943].undecodedStr = strdup("C0 /3 ib");
	instruction[943].dstOperandStr = strdup("r/m8");
	instruction[943].dstOperand = IA_RM8;
	instruction[943].srcOperandStr = strdup("imm8");
	instruction[943].srcOperand = IA_IMM8;
	instruction[943].valid64 = INVALID;
	instruction[943].compatLeg = VALID;
	instruction[943].VMX = 0;
	instruction[943].description = strdup("Rotate 9 bits (CF, r/m8) right imm8 times.");
	instruction[943].binPrefixLen = 0;
	instruction[943].binOpcodeLen = 1;
	instruction[943].binOpcode = bpstrndup("\xC0", 1);
	instruction[943].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[943].opcodeOperand[1] = OA_IB;
	instruction[943].opcodeOperandCount = 2;

	// Instruction Struct [944] - RCR
	instruction[944].instructionStr = strdup("RCR");
	instruction[944].undecodedStr = strdup("D1 /3");
	instruction[944].dstOperandStr = strdup("r/m16");
	instruction[944].dstOperand = IA_RM16;
	instruction[944].srcOperandStr = strdup("1");
	instruction[944].srcOperand = IA_1;
	instruction[944].valid64 = INVALID;
	instruction[944].compatLeg = VALID;
	instruction[944].VMX = 0;
	instruction[944].description = strdup("Rotate 17 bits (CF, r/m16) right once.");
	instruction[944].binPrefixLen = 0;
	instruction[944].binOpcodeLen = 1;
	instruction[944].binOpcode = bpstrndup("\xD1", 1);
	instruction[944].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[944].opcodeOperandCount = 1;

	// Instruction Struct [945] - RCR
	instruction[945].instructionStr = strdup("RCR");
	instruction[945].undecodedStr = strdup("D3 /3");
	instruction[945].dstOperandStr = strdup("r/m16");
	instruction[945].dstOperand = IA_RM16;
	instruction[945].srcOperandStr = strdup("CL");
	instruction[945].srcOperand = IA_CL;
	instruction[945].valid64 = INVALID;
	instruction[945].compatLeg = VALID;
	instruction[945].VMX = 0;
	instruction[945].description = strdup("Rotate 17 bits (CF, r/m16) right CL times.");
	instruction[945].binPrefixLen = 0;
	instruction[945].binOpcodeLen = 1;
	instruction[945].binOpcode = bpstrndup("\xD3", 1);
	instruction[945].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[945].opcodeOperandCount = 1;

	// Instruction Struct [946] - RCR
	instruction[946].instructionStr = strdup("RCR");
	instruction[946].undecodedStr = strdup("C1 /3 ib");
	instruction[946].dstOperandStr = strdup("r/m16");
	instruction[946].dstOperand = IA_RM16;
	instruction[946].srcOperandStr = strdup("imm8");
	instruction[946].srcOperand = IA_IMM8;
	instruction[946].valid64 = INVALID;
	instruction[946].compatLeg = VALID;
	instruction[946].VMX = 0;
	instruction[946].description = strdup("Rotate 17 bits (CF, r/m16) right imm8 times.");
	instruction[946].binPrefixLen = 0;
	instruction[946].binOpcodeLen = 1;
	instruction[946].binOpcode = bpstrndup("\xC1", 1);
	instruction[946].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[946].opcodeOperand[1] = OA_IB;
	instruction[946].opcodeOperandCount = 2;

	// Instruction Struct [947] - RCR
	instruction[947].instructionStr = strdup("RCR");
	instruction[947].undecodedStr = strdup("D1 /3");
	instruction[947].dstOperandStr = strdup("r/m32");
	instruction[947].dstOperand = IA_RM32;
	instruction[947].srcOperandStr = strdup("1");
	instruction[947].srcOperand = IA_1;
	instruction[947].valid64 = INVALID;
	instruction[947].compatLeg = VALID;
	instruction[947].VMX = 0;
	instruction[947].description = strdup("Rotate 33 bits (CF, r/m32) right once. Uses a 6 bit count.");
	instruction[947].binPrefixLen = 0;
	instruction[947].binOpcodeLen = 1;
	instruction[947].binOpcode = bpstrndup("\xD1", 1);
	instruction[947].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[947].opcodeOperandCount = 1;

	// Instruction Struct [948] - RCR
	instruction[948].instructionStr = strdup("RCR");
	instruction[948].undecodedStr = strdup("D3 /3");
	instruction[948].dstOperandStr = strdup("r/m32");
	instruction[948].dstOperand = IA_RM32;
	instruction[948].srcOperandStr = strdup("CL");
	instruction[948].srcOperand = IA_CL;
	instruction[948].valid64 = INVALID;
	instruction[948].compatLeg = VALID;
	instruction[948].VMX = 0;
	instruction[948].description = strdup("Rotate 33 bits (CF, r/m32) right CL times.");
	instruction[948].binPrefixLen = 0;
	instruction[948].binOpcodeLen = 1;
	instruction[948].binOpcode = bpstrndup("\xD3", 1);
	instruction[948].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[948].opcodeOperandCount = 1;

	// Instruction Struct [949] - RCR
	instruction[949].instructionStr = strdup("RCR");
	instruction[949].undecodedStr = strdup("C1 /3 ib");
	instruction[949].dstOperandStr = strdup("r/m32");
	instruction[949].dstOperand = IA_RM32;
	instruction[949].srcOperandStr = strdup("imm8");
	instruction[949].srcOperand = IA_IMM8;
	instruction[949].valid64 = INVALID;
	instruction[949].compatLeg = VALID;
	instruction[949].VMX = 0;
	instruction[949].description = strdup("Rotate 33 bits (CF, r/m32) right imm8 times.");
	instruction[949].binPrefixLen = 0;
	instruction[949].binOpcodeLen = 1;
	instruction[949].binOpcode = bpstrndup("\xC1", 1);
	instruction[949].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[949].opcodeOperand[1] = OA_IB;
	instruction[949].opcodeOperandCount = 2;

	// Instruction Struct [950] - ROL
	instruction[950].instructionStr = strdup("ROL");
	instruction[950].undecodedStr = strdup("D0 /0");
	instruction[950].dstOperandStr = strdup("r/m8");
	instruction[950].dstOperand = IA_RM8;
	instruction[950].srcOperandStr = strdup("1");
	instruction[950].srcOperand = IA_1;
	instruction[950].valid64 = INVALID;
	instruction[950].compatLeg = VALID;
	instruction[950].VMX = 0;
	instruction[950].description = strdup("Rotate 8 bits r/m8 left once.");
	instruction[950].binPrefixLen = 0;
	instruction[950].binOpcodeLen = 1;
	instruction[950].binOpcode = bpstrndup("\xD0", 1);
	instruction[950].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[950].opcodeOperandCount = 1;

	// Instruction Struct [951] - ROL
	instruction[951].instructionStr = strdup("ROL");
	instruction[951].undecodedStr = strdup("D2 /0");
	instruction[951].dstOperandStr = strdup("r/m8");
	instruction[951].dstOperand = IA_RM8;
	instruction[951].srcOperandStr = strdup("CL");
	instruction[951].srcOperand = IA_CL;
	instruction[951].valid64 = INVALID;
	instruction[951].compatLeg = VALID;
	instruction[951].VMX = 0;
	instruction[951].description = strdup("Rotate 8 bits r/m8 left CL times.");
	instruction[951].binPrefixLen = 0;
	instruction[951].binOpcodeLen = 1;
	instruction[951].binOpcode = bpstrndup("\xD2", 1);
	instruction[951].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[951].opcodeOperandCount = 1;

	// Instruction Struct [952] - ROL
	instruction[952].instructionStr = strdup("ROL");
	instruction[952].undecodedStr = strdup("C0 /0 ib");
	instruction[952].dstOperandStr = strdup("r/m8");
	instruction[952].dstOperand = IA_RM8;
	instruction[952].srcOperandStr = strdup("imm8");
	instruction[952].srcOperand = IA_IMM8;
	instruction[952].valid64 = INVALID;
	instruction[952].compatLeg = VALID;
	instruction[952].VMX = 0;
	instruction[952].description = strdup("Rotate 8 bits r/m8 left imm8 times.");
	instruction[952].binPrefixLen = 0;
	instruction[952].binOpcodeLen = 1;
	instruction[952].binOpcode = bpstrndup("\xC0", 1);
	instruction[952].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[952].opcodeOperand[1] = OA_IB;
	instruction[952].opcodeOperandCount = 2;

	// Instruction Struct [953] - ROL
	instruction[953].instructionStr = strdup("ROL");
	instruction[953].undecodedStr = strdup("D1 /0");
	instruction[953].dstOperandStr = strdup("r/m16");
	instruction[953].dstOperand = IA_RM16;
	instruction[953].srcOperandStr = strdup("1");
	instruction[953].srcOperand = IA_1;
	instruction[953].valid64 = INVALID;
	instruction[953].compatLeg = VALID;
	instruction[953].VMX = 0;
	instruction[953].description = strdup("Rotate 16 bits r/m16 left once.");
	instruction[953].binPrefixLen = 0;
	instruction[953].binOpcodeLen = 1;
	instruction[953].binOpcode = bpstrndup("\xD1", 1);
	instruction[953].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[953].opcodeOperandCount = 1;

	// Instruction Struct [954] - ROL
	instruction[954].instructionStr = strdup("ROL");
	instruction[954].undecodedStr = strdup("D3 /0");
	instruction[954].dstOperandStr = strdup("r/m16");
	instruction[954].dstOperand = IA_RM16;
	instruction[954].srcOperandStr = strdup("CL");
	instruction[954].srcOperand = IA_CL;
	instruction[954].valid64 = INVALID;
	instruction[954].compatLeg = VALID;
	instruction[954].VMX = 0;
	instruction[954].description = strdup("Rotate 16 bits r/m16 left CL times.");
	instruction[954].binPrefixLen = 0;
	instruction[954].binOpcodeLen = 1;
	instruction[954].binOpcode = bpstrndup("\xD3", 1);
	instruction[954].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[954].opcodeOperandCount = 1;

	// Instruction Struct [955] - ROL
	instruction[955].instructionStr = strdup("ROL");
	instruction[955].undecodedStr = strdup("C1 /0 ib");
	instruction[955].dstOperandStr = strdup("r/m16");
	instruction[955].dstOperand = IA_RM16;
	instruction[955].srcOperandStr = strdup("imm8");
	instruction[955].srcOperand = IA_IMM8;
	instruction[955].valid64 = INVALID;
	instruction[955].compatLeg = VALID;
	instruction[955].VMX = 0;
	instruction[955].description = strdup("Rotate 16 bits r/m16 left imm8 times.");
	instruction[955].binPrefixLen = 0;
	instruction[955].binOpcodeLen = 1;
	instruction[955].binOpcode = bpstrndup("\xC1", 1);
	instruction[955].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[955].opcodeOperand[1] = OA_IB;
	instruction[955].opcodeOperandCount = 2;

	// Instruction Struct [956] - ROL
	instruction[956].instructionStr = strdup("ROL");
	instruction[956].undecodedStr = strdup("D1 /0");
	instruction[956].dstOperandStr = strdup("r/m32");
	instruction[956].dstOperand = IA_RM32;
	instruction[956].srcOperandStr = strdup("1");
	instruction[956].srcOperand = IA_1;
	instruction[956].valid64 = INVALID;
	instruction[956].compatLeg = VALID;
	instruction[956].VMX = 0;
	instruction[956].description = strdup("Rotate 32 bits r/m32 left once.");
	instruction[956].binPrefixLen = 0;
	instruction[956].binOpcodeLen = 1;
	instruction[956].binOpcode = bpstrndup("\xD1", 1);
	instruction[956].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[956].opcodeOperandCount = 1;

	// Instruction Struct [957] - ROL
	instruction[957].instructionStr = strdup("ROL");
	instruction[957].undecodedStr = strdup("D3 /0");
	instruction[957].dstOperandStr = strdup("r/m32");
	instruction[957].dstOperand = IA_RM32;
	instruction[957].srcOperandStr = strdup("CL");
	instruction[957].srcOperand = IA_CL;
	instruction[957].valid64 = INVALID;
	instruction[957].compatLeg = VALID;
	instruction[957].VMX = 0;
	instruction[957].description = strdup("Rotate 32 bits r/m32 left CL times.");
	instruction[957].binPrefixLen = 0;
	instruction[957].binOpcodeLen = 1;
	instruction[957].binOpcode = bpstrndup("\xD3", 1);
	instruction[957].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[957].opcodeOperandCount = 1;

	// Instruction Struct [958] - ROL
	instruction[958].instructionStr = strdup("ROL");
	instruction[958].undecodedStr = strdup("C1 /0 ib");
	instruction[958].dstOperandStr = strdup("r/m32");
	instruction[958].dstOperand = IA_RM32;
	instruction[958].srcOperandStr = strdup("imm8");
	instruction[958].srcOperand = IA_IMM8;
	instruction[958].valid64 = INVALID;
	instruction[958].compatLeg = VALID;
	instruction[958].VMX = 0;
	instruction[958].description = strdup("Rotate 32 bits r/m32 left imm8 times.");
	instruction[958].binPrefixLen = 0;
	instruction[958].binOpcodeLen = 1;
	instruction[958].binOpcode = bpstrndup("\xC1", 1);
	instruction[958].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[958].opcodeOperand[1] = OA_IB;
	instruction[958].opcodeOperandCount = 2;

	// Instruction Struct [959] - ROL
	instruction[959].instructionStr = strdup("ROL");
	instruction[959].undecodedStr = strdup("C1 /0 ib");
	instruction[959].dstOperandStr = strdup("r/m64");
	instruction[959].dstOperand = IA_RM64;
	instruction[959].srcOperandStr = strdup("imm8");
	instruction[959].srcOperand = IA_IMM8;
	instruction[959].valid64 = INVALID;
	instruction[959].compatLeg = INVALID;
	instruction[959].VMX = 0;
	instruction[959].description = strdup("Rotate 64 bits r/m64 left imm8 times. Uses a 6 bit count.");
	instruction[959].binPrefixLen = 0;
	instruction[959].binOpcodeLen = 1;
	instruction[959].binOpcode = bpstrndup("\xC1", 1);
	instruction[959].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[959].opcodeOperand[1] = OA_IB;
	instruction[959].opcodeOperandCount = 2;

	// Instruction Struct [960] - ROR
	instruction[960].instructionStr = strdup("ROR");
	instruction[960].undecodedStr = strdup("D0 /1");
	instruction[960].dstOperandStr = strdup("r/m8");
	instruction[960].dstOperand = IA_RM8;
	instruction[960].srcOperandStr = strdup("1");
	instruction[960].srcOperand = IA_1;
	instruction[960].valid64 = INVALID;
	instruction[960].compatLeg = VALID;
	instruction[960].VMX = 0;
	instruction[960].description = strdup("Rotate 8 bits r/m8 right once.");
	instruction[960].binPrefixLen = 0;
	instruction[960].binOpcodeLen = 1;
	instruction[960].binOpcode = bpstrndup("\xD0", 1);
	instruction[960].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[960].opcodeOperandCount = 1;

	// Instruction Struct [961] - ROR
	instruction[961].instructionStr = strdup("ROR");
	instruction[961].undecodedStr = strdup("D2 /1");
	instruction[961].dstOperandStr = strdup("r/m8");
	instruction[961].dstOperand = IA_RM8;
	instruction[961].srcOperandStr = strdup("CL");
	instruction[961].srcOperand = IA_CL;
	instruction[961].valid64 = INVALID;
	instruction[961].compatLeg = VALID;
	instruction[961].VMX = 0;
	instruction[961].description = strdup("Rotate 8 bits r/m8 right CL times.");
	instruction[961].binPrefixLen = 0;
	instruction[961].binOpcodeLen = 1;
	instruction[961].binOpcode = bpstrndup("\xD2", 1);
	instruction[961].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[961].opcodeOperandCount = 1;

	// Instruction Struct [962] - ROR
	instruction[962].instructionStr = strdup("ROR");
	instruction[962].undecodedStr = strdup("C0 /1 ib");
	instruction[962].dstOperandStr = strdup("r/m8");
	instruction[962].dstOperand = IA_RM8;
	instruction[962].srcOperandStr = strdup("imm8");
	instruction[962].srcOperand = IA_IMM8;
	instruction[962].valid64 = INVALID;
	instruction[962].compatLeg = VALID;
	instruction[962].VMX = 0;
	instruction[962].description = strdup("Rotate 8 bits r/m16 right imm8 times.");
	instruction[962].binPrefixLen = 0;
	instruction[962].binOpcodeLen = 1;
	instruction[962].binOpcode = bpstrndup("\xC0", 1);
	instruction[962].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[962].opcodeOperand[1] = OA_IB;
	instruction[962].opcodeOperandCount = 2;

	// Instruction Struct [963] - ROR
	instruction[963].instructionStr = strdup("ROR");
	instruction[963].undecodedStr = strdup("D1 /1");
	instruction[963].dstOperandStr = strdup("r/m16");
	instruction[963].dstOperand = IA_RM16;
	instruction[963].srcOperandStr = strdup("1");
	instruction[963].srcOperand = IA_1;
	instruction[963].valid64 = INVALID;
	instruction[963].compatLeg = VALID;
	instruction[963].VMX = 0;
	instruction[963].description = strdup("Rotate 16 bits r/m16 right once.");
	instruction[963].binPrefixLen = 0;
	instruction[963].binOpcodeLen = 1;
	instruction[963].binOpcode = bpstrndup("\xD1", 1);
	instruction[963].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[963].opcodeOperandCount = 1;

	// Instruction Struct [964] - ROR
	instruction[964].instructionStr = strdup("ROR");
	instruction[964].undecodedStr = strdup("D3 /1");
	instruction[964].dstOperandStr = strdup("r/m16");
	instruction[964].dstOperand = IA_RM16;
	instruction[964].srcOperandStr = strdup("CL");
	instruction[964].srcOperand = IA_CL;
	instruction[964].valid64 = INVALID;
	instruction[964].compatLeg = VALID;
	instruction[964].VMX = 0;
	instruction[964].description = strdup("Rotate 16 bits r/m16 right CL times.");
	instruction[964].binPrefixLen = 0;
	instruction[964].binOpcodeLen = 1;
	instruction[964].binOpcode = bpstrndup("\xD3", 1);
	instruction[964].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[964].opcodeOperandCount = 1;

	// Instruction Struct [965] - ROR
	instruction[965].instructionStr = strdup("ROR");
	instruction[965].undecodedStr = strdup("C1 /1 ib");
	instruction[965].dstOperandStr = strdup("r/m16");
	instruction[965].dstOperand = IA_RM16;
	instruction[965].srcOperandStr = strdup("imm8");
	instruction[965].srcOperand = IA_IMM8;
	instruction[965].valid64 = INVALID;
	instruction[965].compatLeg = VALID;
	instruction[965].VMX = 0;
	instruction[965].description = strdup("Rotate 16 bits r/m16 right imm8 times.");
	instruction[965].binPrefixLen = 0;
	instruction[965].binOpcodeLen = 1;
	instruction[965].binOpcode = bpstrndup("\xC1", 1);
	instruction[965].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[965].opcodeOperand[1] = OA_IB;
	instruction[965].opcodeOperandCount = 2;

	// Instruction Struct [966] - ROR
	instruction[966].instructionStr = strdup("ROR");
	instruction[966].undecodedStr = strdup("D1 /1");
	instruction[966].dstOperandStr = strdup("r/m32");
	instruction[966].dstOperand = IA_RM32;
	instruction[966].srcOperandStr = strdup("1");
	instruction[966].srcOperand = IA_1;
	instruction[966].valid64 = INVALID;
	instruction[966].compatLeg = VALID;
	instruction[966].VMX = 0;
	instruction[966].description = strdup("Rotate 32 bits r/m32 right once.");
	instruction[966].binPrefixLen = 0;
	instruction[966].binOpcodeLen = 1;
	instruction[966].binOpcode = bpstrndup("\xD1", 1);
	instruction[966].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[966].opcodeOperandCount = 1;

	// Instruction Struct [967] - ROR
	instruction[967].instructionStr = strdup("ROR");
	instruction[967].undecodedStr = strdup("D3 /1");
	instruction[967].dstOperandStr = strdup("r/m32");
	instruction[967].dstOperand = IA_RM32;
	instruction[967].srcOperandStr = strdup("CL");
	instruction[967].srcOperand = IA_CL;
	instruction[967].valid64 = INVALID;
	instruction[967].compatLeg = VALID;
	instruction[967].VMX = 0;
	instruction[967].description = strdup("Rotate 32 bits r/m32 right CL times.");
	instruction[967].binPrefixLen = 0;
	instruction[967].binOpcodeLen = 1;
	instruction[967].binOpcode = bpstrndup("\xD3", 1);
	instruction[967].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[967].opcodeOperandCount = 1;

	// Instruction Struct [968] - ROR
	instruction[968].instructionStr = strdup("ROR");
	instruction[968].undecodedStr = strdup("C1 /1 ib");
	instruction[968].dstOperandStr = strdup("r/m32");
	instruction[968].dstOperand = IA_RM32;
	instruction[968].srcOperandStr = strdup("imm8");
	instruction[968].srcOperand = IA_IMM8;
	instruction[968].valid64 = INVALID;
	instruction[968].compatLeg = VALID;
	instruction[968].VMX = 0;
	instruction[968].description = strdup("Rotate 32 bits r/m32 right imm8 times.");
	instruction[968].binPrefixLen = 0;
	instruction[968].binOpcodeLen = 1;
	instruction[968].binOpcode = bpstrndup("\xC1", 1);
	instruction[968].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[968].opcodeOperand[1] = OA_IB;
	instruction[968].opcodeOperandCount = 2;

	// Instruction Struct [969] - RCPPS
	instruction[969].instructionStr = strdup("RCPPS");
	instruction[969].undecodedStr = strdup("0F 53 /r");
	instruction[969].dstOperandStr = strdup("xmm1");
	instruction[969].dstOperand = IA_XMM;
	instruction[969].srcOperandStr = strdup("xmm2/m128");
	instruction[969].srcOperand = IA_XMM_M128;
	instruction[969].valid64 = INVALID;
	instruction[969].compatLeg = VALID;
	instruction[969].VMX = 0;
	instruction[969].description = strdup("Computes the approximate reciprocals of the packed single-precision floatingpoint values in xmm2/m128 and stores the results in xmm1.");
	instruction[969].binPrefixLen = 0;
	instruction[969].binOpcodeLen = 2;
	instruction[969].binOpcode = bpstrndup("\x0F\x53", 2);
	instruction[969].opcodeOperand[0] = OA_MODRM;
	instruction[969].opcodeOperandCount = 1;

	// Instruction Struct [970] - RCPSS
	instruction[970].instructionStr = strdup("RCPSS");
	instruction[970].undecodedStr = strdup("F3 0F 53 /r");
	instruction[970].dstOperandStr = strdup("xmm1");
	instruction[970].dstOperand = IA_XMM;
	instruction[970].srcOperandStr = strdup("xmm2/m32");
	instruction[970].srcOperand = IA_XMM_M32;
	instruction[970].valid64 = INVALID;
	instruction[970].compatLeg = VALID;
	instruction[970].VMX = 0;
	instruction[970].description = strdup("Computes the approximate reciprocal of the scalar single-precision floating-point value in xmm2/m32 and stores the result in xmm1.");
	instruction[970].prefixStr = strdup("REP");
	instruction[970].binPrefixLen = 1;
	instruction[970].binPrefix = bpstrndup("\xF3", 1);
	instruction[970].binOpcodeLen = 2;
	instruction[970].binOpcode = bpstrndup("\x0F\x53", 2);
	instruction[970].opcodeOperand[0] = OA_MODRM;
	instruction[970].opcodeOperandCount = 1;

	// Instruction Struct [971] - RDMSR
	instruction[971].instructionStr = strdup("RDMSR");
	instruction[971].undecodedStr = strdup("0F 32");
	instruction[971].valid64 = INVALID;
	instruction[971].compatLeg = VALID;
	instruction[971].VMX = 0;
	instruction[971].description = strdup("Load MSR specified by ECX into EDX:EAX.");
	instruction[971].binPrefixLen = 0;
	instruction[971].binOpcodeLen = 2;
	instruction[971].binOpcode = bpstrndup("\x0F\x32", 2);
	instruction[971].opcodeOperandCount = 0;

	// Instruction Struct [972] - RDPMC
	instruction[972].instructionStr = strdup("RDPMC");
	instruction[972].undecodedStr = strdup("0F 33");
	instruction[972].valid64 = INVALID;
	instruction[972].compatLeg = VALID;
	instruction[972].VMX = 0;
	instruction[972].description = strdup("Read performance-monitoring counter specified by ECX into EDX:EAX.");
	instruction[972].binPrefixLen = 0;
	instruction[972].binOpcodeLen = 2;
	instruction[972].binOpcode = bpstrndup("\x0F\x33", 2);
	instruction[972].opcodeOperandCount = 0;

	// Instruction Struct [973] - RDTSC
	instruction[973].instructionStr = strdup("RDTSC");
	instruction[973].undecodedStr = strdup("0F 31");
	instruction[973].valid64 = INVALID;
	instruction[973].compatLeg = VALID;
	instruction[973].VMX = 0;
	instruction[973].description = strdup("Read time-stamp counter into EDX:EAX.");
	instruction[973].binPrefixLen = 0;
	instruction[973].binOpcodeLen = 2;
	instruction[973].binOpcode = bpstrndup("\x0F\x31", 2);
	instruction[973].opcodeOperandCount = 0;

	// Instruction Struct [974] - REP INS
	instruction[974].instructionStr = strdup("REP INS");
	instruction[974].undecodedStr = strdup("F3 6C");
	instruction[974].dstOperandStr = strdup("m8");
	instruction[974].dstOperand = IA_M8;
	instruction[974].srcOperandStr = strdup("DX");
	instruction[974].srcOperand = IA_DX;
	instruction[974].valid64 = INVALID;
	instruction[974].compatLeg = VALID;
	instruction[974].VMX = 0;
	instruction[974].description = strdup("Input (E)CX bytes from port DX into ES:[(E)DI].");
	instruction[974].prefixStr = strdup("REP");
	instruction[974].binPrefixLen = 1;
	instruction[974].binPrefix = bpstrndup("\xF3", 1);
	instruction[974].binOpcodeLen = 1;
	instruction[974].binOpcode = bpstrndup("\x6C", 1);
	instruction[974].opcodeOperandCount = 0;

	// Instruction Struct [975] - REP INS
	instruction[975].instructionStr = strdup("REP INS");
	instruction[975].undecodedStr = strdup("F3 6C");
	instruction[975].dstOperandStr = strdup("m8");
	instruction[975].dstOperand = IA_M8;
	instruction[975].srcOperandStr = strdup("DX");
	instruction[975].srcOperand = IA_DX;
	instruction[975].valid64 = INVALID;
	instruction[975].compatLeg = INVALID;
	instruction[975].VMX = 0;
	instruction[975].description = strdup("Input RCX bytes from port DX into [RDI].");
	instruction[975].prefixStr = strdup("REP");
	instruction[975].binPrefixLen = 1;
	instruction[975].binPrefix = bpstrndup("\xF3", 1);
	instruction[975].binOpcodeLen = 1;
	instruction[975].binOpcode = bpstrndup("\x6C", 1);
	instruction[975].opcodeOperandCount = 0;

	// Instruction Struct [976] - REP INS
	instruction[976].instructionStr = strdup("REP INS");
	instruction[976].undecodedStr = strdup("F3 6D");
	instruction[976].dstOperandStr = strdup("m16");
	instruction[976].dstOperand = IA_M16;
	instruction[976].srcOperandStr = strdup("DX");
	instruction[976].srcOperand = IA_DX;
	instruction[976].valid64 = INVALID;
	instruction[976].compatLeg = VALID;
	instruction[976].VMX = 0;
	instruction[976].description = strdup("Input (E)CX words from port DX into ES:[(E)DI.]");
	instruction[976].prefixStr = strdup("REP");
	instruction[976].binPrefixLen = 1;
	instruction[976].binPrefix = bpstrndup("\xF3", 1);
	instruction[976].binOpcodeLen = 1;
	instruction[976].binOpcode = bpstrndup("\x6D", 1);
	instruction[976].opcodeOperandCount = 0;

	// Instruction Struct [977] - REP INS
	instruction[977].instructionStr = strdup("REP INS");
	instruction[977].undecodedStr = strdup("F3 6D");
	instruction[977].dstOperandStr = strdup("m32");
	instruction[977].dstOperand = IA_M32;
	instruction[977].srcOperandStr = strdup("DX");
	instruction[977].srcOperand = IA_DX;
	instruction[977].valid64 = INVALID;
	instruction[977].compatLeg = VALID;
	instruction[977].VMX = 0;
	instruction[977].description = strdup("Input (E)CX doublewords from port DX into ES:[(E)DI].");
	instruction[977].prefixStr = strdup("REP");
	instruction[977].binPrefixLen = 1;
	instruction[977].binPrefix = bpstrndup("\xF3", 1);
	instruction[977].binOpcodeLen = 1;
	instruction[977].binOpcode = bpstrndup("\x6D", 1);
	instruction[977].opcodeOperandCount = 0;

	// Instruction Struct [978] - REP INS
	instruction[978].instructionStr = strdup("REP INS");
	instruction[978].undecodedStr = strdup("F3 6D");
	instruction[978].dstOperandStr = strdup("r/m32");
	instruction[978].dstOperand = IA_RM32;
	instruction[978].srcOperandStr = strdup("DX");
	instruction[978].srcOperand = IA_DX;
	instruction[978].valid64 = INVALID;
	instruction[978].compatLeg = INVALID;
	instruction[978].VMX = 0;
	instruction[978].description = strdup("Input RCX default size from port DX into [RDI].");
	instruction[978].prefixStr = strdup("REP");
	instruction[978].binPrefixLen = 1;
	instruction[978].binPrefix = bpstrndup("\xF3", 1);
	instruction[978].binOpcodeLen = 1;
	instruction[978].binOpcode = bpstrndup("\x6D", 1);
	instruction[978].opcodeOperandCount = 0;

	// Instruction Struct [979] - REP MOVS
	instruction[979].instructionStr = strdup("REP MOVS");
	instruction[979].undecodedStr = strdup("F3 A4");
	instruction[979].dstOperandStr = strdup("m8");
	instruction[979].dstOperand = IA_M8;
	instruction[979].srcOperandStr = strdup("m8");
	instruction[979].srcOperand = IA_M8;
	instruction[979].valid64 = INVALID;
	instruction[979].compatLeg = VALID;
	instruction[979].VMX = 0;
	instruction[979].description = strdup("Move (E)CX bytes from DS:[(E)SI] to ES:[(E)DI].");
	instruction[979].prefixStr = strdup("REP");
	instruction[979].binPrefixLen = 1;
	instruction[979].binPrefix = bpstrndup("\xF3", 1);
	instruction[979].binOpcodeLen = 1;
	instruction[979].binOpcode = bpstrndup("\xA4", 1);
	instruction[979].opcodeOperandCount = 0;

	// Instruction Struct [980] - REP MOVS
	instruction[980].instructionStr = strdup("REP MOVS");
	instruction[980].undecodedStr = strdup("F3 A5");
	instruction[980].dstOperandStr = strdup("m16");
	instruction[980].dstOperand = IA_M16;
	instruction[980].srcOperandStr = strdup("m16");
	instruction[980].srcOperand = IA_M16;
	instruction[980].valid64 = INVALID;
	instruction[980].compatLeg = VALID;
	instruction[980].VMX = 0;
	instruction[980].description = strdup("Move (E)CX words from DS:[(E)SI] to ES:[(E)DI].");
	instruction[980].prefixStr = strdup("REP");
	instruction[980].binPrefixLen = 1;
	instruction[980].binPrefix = bpstrndup("\xF3", 1);
	instruction[980].binOpcodeLen = 1;
	instruction[980].binOpcode = bpstrndup("\xA5", 1);
	instruction[980].opcodeOperandCount = 0;

	// Instruction Struct [981] - REP MOVS
	instruction[981].instructionStr = strdup("REP MOVS");
	instruction[981].undecodedStr = strdup("F3 A5");
	instruction[981].dstOperandStr = strdup("m32");
	instruction[981].dstOperand = IA_M32;
	instruction[981].srcOperandStr = strdup("m32");
	instruction[981].srcOperand = IA_M32;
	instruction[981].valid64 = INVALID;
	instruction[981].compatLeg = VALID;
	instruction[981].VMX = 0;
	instruction[981].description = strdup("Move (E)CX doublewords from DS:[(E)SI] to ES:[(E)DI].");
	instruction[981].prefixStr = strdup("REP");
	instruction[981].binPrefixLen = 1;
	instruction[981].binPrefix = bpstrndup("\xF3", 1);
	instruction[981].binOpcodeLen = 1;
	instruction[981].binOpcode = bpstrndup("\xA5", 1);
	instruction[981].opcodeOperandCount = 0;

	// Instruction Struct [982] - REP OUTS
	instruction[982].instructionStr = strdup("REP OUTS");
	instruction[982].undecodedStr = strdup("F3 6E");
	instruction[982].dstOperandStr = strdup("DX");
	instruction[982].dstOperand = IA_DX;
	instruction[982].srcOperandStr = strdup("r/m8");
	instruction[982].srcOperand = IA_RM8;
	instruction[982].valid64 = INVALID;
	instruction[982].compatLeg = VALID;
	instruction[982].VMX = 0;
	instruction[982].description = strdup("Output (E)CX bytes from DS:[(E)SI] to port DX.");
	instruction[982].prefixStr = strdup("REP");
	instruction[982].binPrefixLen = 1;
	instruction[982].binPrefix = bpstrndup("\xF3", 1);
	instruction[982].binOpcodeLen = 1;
	instruction[982].binOpcode = bpstrndup("\x6E", 1);
	instruction[982].opcodeOperandCount = 0;

	// Instruction Struct [983] - REP OUTS
	instruction[983].instructionStr = strdup("REP OUTS");
	instruction[983].undecodedStr = strdup("F3 6F");
	instruction[983].dstOperandStr = strdup("DX");
	instruction[983].dstOperand = IA_DX;
	instruction[983].srcOperandStr = strdup("r/m16");
	instruction[983].srcOperand = IA_RM16;
	instruction[983].valid64 = INVALID;
	instruction[983].compatLeg = VALID;
	instruction[983].VMX = 0;
	instruction[983].description = strdup("Output (E)CX words from DS:[(E)SI] to port DX.");
	instruction[983].prefixStr = strdup("REP");
	instruction[983].binPrefixLen = 1;
	instruction[983].binPrefix = bpstrndup("\xF3", 1);
	instruction[983].binOpcodeLen = 1;
	instruction[983].binOpcode = bpstrndup("\x6F", 1);
	instruction[983].opcodeOperandCount = 0;

	// Instruction Struct [984] - REP OUTS
	instruction[984].instructionStr = strdup("REP OUTS");
	instruction[984].undecodedStr = strdup("F3 6F");
	instruction[984].dstOperandStr = strdup("DX");
	instruction[984].dstOperand = IA_DX;
	instruction[984].srcOperandStr = strdup("r/m32");
	instruction[984].srcOperand = IA_RM32;
	instruction[984].valid64 = INVALID;
	instruction[984].compatLeg = VALID;
	instruction[984].VMX = 0;
	instruction[984].description = strdup("Output (E)CX doublewords from DS:[(E)SI] to port DX.");
	instruction[984].prefixStr = strdup("REP");
	instruction[984].binPrefixLen = 1;
	instruction[984].binPrefix = bpstrndup("\xF3", 1);
	instruction[984].binOpcodeLen = 1;
	instruction[984].binOpcode = bpstrndup("\x6F", 1);
	instruction[984].opcodeOperandCount = 0;

	// Instruction Struct [985] - REP LODS
	instruction[985].instructionStr = strdup("REP LODS");
	instruction[985].undecodedStr = strdup("F3 AC");
	instruction[985].dstOperandStr = strdup("AL");
	instruction[985].dstOperand = IA_AL;
	instruction[985].valid64 = INVALID;
	instruction[985].compatLeg = VALID;
	instruction[985].VMX = 0;
	instruction[985].description = strdup("Load (E)CX bytes from DS:[(E)SI] to AL.");
	instruction[985].prefixStr = strdup("REP");
	instruction[985].binPrefixLen = 1;
	instruction[985].binPrefix = bpstrndup("\xF3", 1);
	instruction[985].binOpcodeLen = 1;
	instruction[985].binOpcode = bpstrndup("\xAC", 1);
	instruction[985].opcodeOperandCount = 0;

	// Instruction Struct [986] - REP LODS
	instruction[986].instructionStr = strdup("REP LODS");
	instruction[986].undecodedStr = strdup("F3 AD");
	instruction[986].dstOperandStr = strdup("AX");
	instruction[986].dstOperand = IA_AX;
	instruction[986].valid64 = INVALID;
	instruction[986].compatLeg = VALID;
	instruction[986].VMX = 0;
	instruction[986].description = strdup("Load (E)CX words from DS:[(E)SI] to AX.");
	instruction[986].prefixStr = strdup("REP");
	instruction[986].binPrefixLen = 1;
	instruction[986].binPrefix = bpstrndup("\xF3", 1);
	instruction[986].binOpcodeLen = 1;
	instruction[986].binOpcode = bpstrndup("\xAD", 1);
	instruction[986].opcodeOperandCount = 0;

	// Instruction Struct [987] - REP LODS
	instruction[987].instructionStr = strdup("REP LODS");
	instruction[987].undecodedStr = strdup("F3 AD");
	instruction[987].dstOperandStr = strdup("EAX");
	instruction[987].dstOperand = IA_EAX;
	instruction[987].valid64 = INVALID;
	instruction[987].compatLeg = VALID;
	instruction[987].VMX = 0;
	instruction[987].description = strdup("Load (E)CX doublewords from DS:[(E)SI] to EAX.");
	instruction[987].prefixStr = strdup("REP");
	instruction[987].binPrefixLen = 1;
	instruction[987].binPrefix = bpstrndup("\xF3", 1);
	instruction[987].binOpcodeLen = 1;
	instruction[987].binOpcode = bpstrndup("\xAD", 1);
	instruction[987].opcodeOperandCount = 0;

	// Instruction Struct [988] - REP STOS
	instruction[988].instructionStr = strdup("REP STOS");
	instruction[988].undecodedStr = strdup("F3 AA");
	instruction[988].dstOperandStr = strdup("m8");
	instruction[988].dstOperand = IA_M8;
	instruction[988].valid64 = INVALID;
	instruction[988].compatLeg = VALID;
	instruction[988].VMX = 0;
	instruction[988].description = strdup("Fill (E)CX bytes at ES:[(E)DI] with AL.");
	instruction[988].prefixStr = strdup("REP");
	instruction[988].binPrefixLen = 1;
	instruction[988].binPrefix = bpstrndup("\xF3", 1);
	instruction[988].binOpcodeLen = 1;
	instruction[988].binOpcode = bpstrndup("\xAA", 1);
	instruction[988].opcodeOperandCount = 0;

	// Instruction Struct [989] - REP STOS
	instruction[989].instructionStr = strdup("REP STOS");
	instruction[989].undecodedStr = strdup("F3 AB");
	instruction[989].dstOperandStr = strdup("m16");
	instruction[989].dstOperand = IA_M16;
	instruction[989].valid64 = INVALID;
	instruction[989].compatLeg = VALID;
	instruction[989].VMX = 0;
	instruction[989].description = strdup("Fill (E)CX words at ES:[(E)DI] with AX.");
	instruction[989].prefixStr = strdup("REP");
	instruction[989].binPrefixLen = 1;
	instruction[989].binPrefix = bpstrndup("\xF3", 1);
	instruction[989].binOpcodeLen = 1;
	instruction[989].binOpcode = bpstrndup("\xAB", 1);
	instruction[989].opcodeOperandCount = 0;

	// Instruction Struct [990] - REP STOS
	instruction[990].instructionStr = strdup("REP STOS");
	instruction[990].undecodedStr = strdup("F3 AB");
	instruction[990].dstOperandStr = strdup("m32");
	instruction[990].dstOperand = IA_M32;
	instruction[990].valid64 = INVALID;
	instruction[990].compatLeg = VALID;
	instruction[990].VMX = 0;
	instruction[990].description = strdup("Fill (E)CX doublewords at ES:[(E)DI] with EAX.");
	instruction[990].prefixStr = strdup("REP");
	instruction[990].binPrefixLen = 1;
	instruction[990].binPrefix = bpstrndup("\xF3", 1);
	instruction[990].binOpcodeLen = 1;
	instruction[990].binOpcode = bpstrndup("\xAB", 1);
	instruction[990].opcodeOperandCount = 0;

	// Instruction Struct [991] - REPE CMPS
	instruction[991].instructionStr = strdup("REPE CMPS");
	instruction[991].undecodedStr = strdup("F3 A6");
	instruction[991].dstOperandStr = strdup("m8");
	instruction[991].dstOperand = IA_M8;
	instruction[991].srcOperandStr = strdup("m8");
	instruction[991].srcOperand = IA_M8;
	instruction[991].valid64 = INVALID;
	instruction[991].compatLeg = VALID;
	instruction[991].VMX = 0;
	instruction[991].description = strdup("Find nonmatching bytes in ES:[(E)DI] and DS:[(E)SI].");
	instruction[991].prefixStr = strdup("REP");
	instruction[991].binPrefixLen = 1;
	instruction[991].binPrefix = bpstrndup("\xF3", 1);
	instruction[991].binOpcodeLen = 1;
	instruction[991].binOpcode = bpstrndup("\xA6", 1);
	instruction[991].opcodeOperandCount = 0;

	// Instruction Struct [992] - REPE CMPS
	instruction[992].instructionStr = strdup("REPE CMPS");
	instruction[992].undecodedStr = strdup("F3 A7");
	instruction[992].dstOperandStr = strdup("m16");
	instruction[992].dstOperand = IA_M16;
	instruction[992].srcOperandStr = strdup("m16");
	instruction[992].srcOperand = IA_M16;
	instruction[992].valid64 = INVALID;
	instruction[992].compatLeg = VALID;
	instruction[992].VMX = 0;
	instruction[992].description = strdup("Find nonmatching words in ES:[(E)DI] and DS:[(E)SI].");
	instruction[992].prefixStr = strdup("REP");
	instruction[992].binPrefixLen = 1;
	instruction[992].binPrefix = bpstrndup("\xF3", 1);
	instruction[992].binOpcodeLen = 1;
	instruction[992].binOpcode = bpstrndup("\xA7", 1);
	instruction[992].opcodeOperandCount = 0;

	// Instruction Struct [993] - REPE CMPS
	instruction[993].instructionStr = strdup("REPE CMPS");
	instruction[993].undecodedStr = strdup("F3 A7");
	instruction[993].dstOperandStr = strdup("m32");
	instruction[993].dstOperand = IA_M32;
	instruction[993].srcOperandStr = strdup("m32");
	instruction[993].srcOperand = IA_M32;
	instruction[993].valid64 = INVALID;
	instruction[993].compatLeg = VALID;
	instruction[993].VMX = 0;
	instruction[993].description = strdup("Find nonmatching doublewords in ES:[(E)DI] and DS:[(E)SI].");
	instruction[993].prefixStr = strdup("REP");
	instruction[993].binPrefixLen = 1;
	instruction[993].binPrefix = bpstrndup("\xF3", 1);
	instruction[993].binOpcodeLen = 1;
	instruction[993].binOpcode = bpstrndup("\xA7", 1);
	instruction[993].opcodeOperandCount = 0;

	// Instruction Struct [994] - REPE SCAS
	instruction[994].instructionStr = strdup("REPE SCAS");
	instruction[994].undecodedStr = strdup("F3 AE");
	instruction[994].dstOperandStr = strdup("m8");
	instruction[994].dstOperand = IA_M8;
	instruction[994].valid64 = INVALID;
	instruction[994].compatLeg = VALID;
	instruction[994].VMX = 0;
	instruction[994].description = strdup("Find non-AL byte starting at ES:[(E)DI].");
	instruction[994].prefixStr = strdup("REP");
	instruction[994].binPrefixLen = 1;
	instruction[994].binPrefix = bpstrndup("\xF3", 1);
	instruction[994].binOpcodeLen = 1;
	instruction[994].binOpcode = bpstrndup("\xAE", 1);
	instruction[994].opcodeOperandCount = 0;

	// Instruction Struct [995] - REPE SCAS
	instruction[995].instructionStr = strdup("REPE SCAS");
	instruction[995].undecodedStr = strdup("F3 AF");
	instruction[995].dstOperandStr = strdup("m16");
	instruction[995].dstOperand = IA_M16;
	instruction[995].valid64 = INVALID;
	instruction[995].compatLeg = VALID;
	instruction[995].VMX = 0;
	instruction[995].description = strdup("Find non-AX word starting at ES:[(E)DI].");
	instruction[995].prefixStr = strdup("REP");
	instruction[995].binPrefixLen = 1;
	instruction[995].binPrefix = bpstrndup("\xF3", 1);
	instruction[995].binOpcodeLen = 1;
	instruction[995].binOpcode = bpstrndup("\xAF", 1);
	instruction[995].opcodeOperandCount = 0;

	// Instruction Struct [996] - REPE SCAS
	instruction[996].instructionStr = strdup("REPE SCAS");
	instruction[996].undecodedStr = strdup("F3 AF");
	instruction[996].dstOperandStr = strdup("m32");
	instruction[996].dstOperand = IA_M32;
	instruction[996].valid64 = INVALID;
	instruction[996].compatLeg = VALID;
	instruction[996].VMX = 0;
	instruction[996].description = strdup("Find non-EAX doubleword starting at ES:[(E)DI].");
	instruction[996].prefixStr = strdup("REP");
	instruction[996].binPrefixLen = 1;
	instruction[996].binPrefix = bpstrndup("\xF3", 1);
	instruction[996].binOpcodeLen = 1;
	instruction[996].binOpcode = bpstrndup("\xAF", 1);
	instruction[996].opcodeOperandCount = 0;

	// Instruction Struct [997] - REPNE CMPS
	instruction[997].instructionStr = strdup("REPNE CMPS");
	instruction[997].undecodedStr = strdup("F2 A6");
	instruction[997].dstOperandStr = strdup("m8");
	instruction[997].dstOperand = IA_M8;
	instruction[997].srcOperandStr = strdup("m8");
	instruction[997].srcOperand = IA_M8;
	instruction[997].valid64 = INVALID;
	instruction[997].compatLeg = VALID;
	instruction[997].VMX = 0;
	instruction[997].description = strdup("Find matching bytes in ES:[(E)DI] and DS:[(E)SI].");
	instruction[997].prefixStr = strdup("REPNE");
	instruction[997].binPrefixLen = 1;
	instruction[997].binPrefix = bpstrndup("\xF2", 1);
	instruction[997].binOpcodeLen = 1;
	instruction[997].binOpcode = bpstrndup("\xA6", 1);
	instruction[997].opcodeOperandCount = 0;

	// Instruction Struct [998] - REPNE CMPS
	instruction[998].instructionStr = strdup("REPNE CMPS");
	instruction[998].undecodedStr = strdup("F2 A7");
	instruction[998].dstOperandStr = strdup("m16");
	instruction[998].dstOperand = IA_M16;
	instruction[998].srcOperandStr = strdup("m16");
	instruction[998].srcOperand = IA_M16;
	instruction[998].valid64 = INVALID;
	instruction[998].compatLeg = VALID;
	instruction[998].VMX = 0;
	instruction[998].description = strdup("Find matching words in ES:[(E)DI] and DS:[(E)SI].");
	instruction[998].prefixStr = strdup("REPNE");
	instruction[998].binPrefixLen = 1;
	instruction[998].binPrefix = bpstrndup("\xF2", 1);
	instruction[998].binOpcodeLen = 1;
	instruction[998].binOpcode = bpstrndup("\xA7", 1);
	instruction[998].opcodeOperandCount = 0;

	// Instruction Struct [999] - REPNE CMPS
	instruction[999].instructionStr = strdup("REPNE CMPS");
	instruction[999].undecodedStr = strdup("F2 A7");
	instruction[999].dstOperandStr = strdup("m32");
	instruction[999].dstOperand = IA_M32;
	instruction[999].srcOperandStr = strdup("m32");
	instruction[999].srcOperand = IA_M32;
	instruction[999].valid64 = INVALID;
	instruction[999].compatLeg = VALID;
	instruction[999].VMX = 0;
	instruction[999].description = strdup("Find matching doublewords in ES:[(E)DI] and DS:[(E)SI].");
	instruction[999].prefixStr = strdup("REPNE");
	instruction[999].binPrefixLen = 1;
	instruction[999].binPrefix = bpstrndup("\xF2", 1);
	instruction[999].binOpcodeLen = 1;
	instruction[999].binOpcode = bpstrndup("\xA7", 1);
	instruction[999].opcodeOperandCount = 0;

	// Instruction Struct [1000] - REPNE SCAS
	instruction[1000].instructionStr = strdup("REPNE SCAS");
	instruction[1000].undecodedStr = strdup("F2 AE");
	instruction[1000].dstOperandStr = strdup("m8");
	instruction[1000].dstOperand = IA_M8;
	instruction[1000].valid64 = INVALID;
	instruction[1000].compatLeg = VALID;
	instruction[1000].VMX = 0;
	instruction[1000].description = strdup("Find AL, starting at ES:[(E)DI].");
	instruction[1000].prefixStr = strdup("REPNE");
	instruction[1000].binPrefixLen = 1;
	instruction[1000].binPrefix = bpstrndup("\xF2", 1);
	instruction[1000].binOpcodeLen = 1;
	instruction[1000].binOpcode = bpstrndup("\xAE", 1);
	instruction[1000].opcodeOperandCount = 0;

	// Instruction Struct [1001] - REPNE SCAS
	instruction[1001].instructionStr = strdup("REPNE SCAS");
	instruction[1001].undecodedStr = strdup("F2 AF");
	instruction[1001].dstOperandStr = strdup("m16");
	instruction[1001].dstOperand = IA_M16;
	instruction[1001].valid64 = INVALID;
	instruction[1001].compatLeg = VALID;
	instruction[1001].VMX = 0;
	instruction[1001].description = strdup("Find AX, starting at ES:[(E)DI].");
	instruction[1001].prefixStr = strdup("REPNE");
	instruction[1001].binPrefixLen = 1;
	instruction[1001].binPrefix = bpstrndup("\xF2", 1);
	instruction[1001].binOpcodeLen = 1;
	instruction[1001].binOpcode = bpstrndup("\xAF", 1);
	instruction[1001].opcodeOperandCount = 0;

	// Instruction Struct [1002] - REPNE SCAS
	instruction[1002].instructionStr = strdup("REPNE SCAS");
	instruction[1002].undecodedStr = strdup("F2 AF");
	instruction[1002].dstOperandStr = strdup("m32");
	instruction[1002].dstOperand = IA_M32;
	instruction[1002].valid64 = INVALID;
	instruction[1002].compatLeg = VALID;
	instruction[1002].VMX = 0;
	instruction[1002].description = strdup("Find EAX, starting at ES:[(E)DI].");
	instruction[1002].prefixStr = strdup("REPNE");
	instruction[1002].binPrefixLen = 1;
	instruction[1002].binPrefix = bpstrndup("\xF2", 1);
	instruction[1002].binOpcodeLen = 1;
	instruction[1002].binOpcode = bpstrndup("\xAF", 1);
	instruction[1002].opcodeOperandCount = 0;

	// Instruction Struct [1003] - RET
	instruction[1003].instructionStr = strdup("RET");
	instruction[1003].undecodedStr = strdup("C3");
	instruction[1003].valid64 = INVALID;
	instruction[1003].compatLeg = VALID;
	instruction[1003].VMX = 0;
	instruction[1003].description = strdup("Near return to calling procedure.");
	instruction[1003].binPrefixLen = 0;
	instruction[1003].binOpcodeLen = 1;
	instruction[1003].binOpcode = bpstrndup("\xC3", 1);
	instruction[1003].opcodeOperandCount = 0;

	// Instruction Struct [1004] - RET
	instruction[1004].instructionStr = strdup("RET");
	instruction[1004].undecodedStr = strdup("CB");
	instruction[1004].valid64 = INVALID;
	instruction[1004].compatLeg = VALID;
	instruction[1004].VMX = 0;
	instruction[1004].description = strdup("Far return to calling procedure.");
	instruction[1004].binPrefixLen = 0;
	instruction[1004].binOpcodeLen = 1;
	instruction[1004].binOpcode = bpstrndup("\xCB", 1);
	instruction[1004].opcodeOperandCount = 0;

	// Instruction Struct [1005] - RET
	instruction[1005].instructionStr = strdup("RET");
	instruction[1005].undecodedStr = strdup("C2 iw");
	instruction[1005].dstOperandStr = strdup("imm16");
	instruction[1005].dstOperand = IA_IMM16;
	instruction[1005].valid64 = INVALID;
	instruction[1005].compatLeg = VALID;
	instruction[1005].VMX = 0;
	instruction[1005].description = strdup("Near return to calling procedure and pop imm16 bytes from stack.");
	instruction[1005].binPrefixLen = 0;
	instruction[1005].binOpcodeLen = 1;
	instruction[1005].binOpcode = bpstrndup("\xC2", 1);
	instruction[1005].opcodeOperand[0] = OA_IW;
	instruction[1005].opcodeOperandCount = 1;

	// Instruction Struct [1006] - RET
	instruction[1006].instructionStr = strdup("RET");
	instruction[1006].undecodedStr = strdup("CA iw");
	instruction[1006].dstOperandStr = strdup("imm16");
	instruction[1006].dstOperand = IA_IMM16;
	instruction[1006].valid64 = INVALID;
	instruction[1006].compatLeg = VALID;
	instruction[1006].VMX = 0;
	instruction[1006].description = strdup("Far return to calling procedure and pop imm16 bytes from stack.");
	instruction[1006].binPrefixLen = 0;
	instruction[1006].binOpcodeLen = 1;
	instruction[1006].binOpcode = bpstrndup("\xCA", 1);
	instruction[1006].opcodeOperand[0] = OA_IW;
	instruction[1006].opcodeOperandCount = 1;

	// Instruction Struct [1007] - RSM
	instruction[1007].instructionStr = strdup("RSM");
	instruction[1007].undecodedStr = strdup("0F AA");
	instruction[1007].valid64 = INVALID;
	instruction[1007].compatLeg = VALID;
	instruction[1007].VMX = 0;
	instruction[1007].description = strdup("Resume operation of interrupted program.");
	instruction[1007].binPrefixLen = 0;
	instruction[1007].binOpcodeLen = 2;
	instruction[1007].binOpcode = bpstrndup("\x0F\xAA", 2);
	instruction[1007].opcodeOperandCount = 0;

	// Instruction Struct [1008] - RSQRTPS
	instruction[1008].instructionStr = strdup("RSQRTPS");
	instruction[1008].undecodedStr = strdup("0F 52 /r");
	instruction[1008].dstOperandStr = strdup("xmm1");
	instruction[1008].dstOperand = IA_XMM;
	instruction[1008].srcOperandStr = strdup("xmm2/m128");
	instruction[1008].srcOperand = IA_XMM_M128;
	instruction[1008].valid64 = INVALID;
	instruction[1008].compatLeg = VALID;
	instruction[1008].VMX = 0;
	instruction[1008].description = strdup("Computes the approximate reciprocals of the square roots of the packed single-precision floating-point values in xmm2/m128 and stores the results in xmm1.");
	instruction[1008].binPrefixLen = 0;
	instruction[1008].binOpcodeLen = 2;
	instruction[1008].binOpcode = bpstrndup("\x0F\x52", 2);
	instruction[1008].opcodeOperand[0] = OA_MODRM;
	instruction[1008].opcodeOperandCount = 1;

	// Instruction Struct [1009] - RSQRTSS
	instruction[1009].instructionStr = strdup("RSQRTSS");
	instruction[1009].undecodedStr = strdup("F3 0F 52 /r");
	instruction[1009].dstOperandStr = strdup("xmm1");
	instruction[1009].dstOperand = IA_XMM;
	instruction[1009].srcOperandStr = strdup("xmm2/m32");
	instruction[1009].srcOperand = IA_XMM_M32;
	instruction[1009].valid64 = INVALID;
	instruction[1009].compatLeg = VALID;
	instruction[1009].VMX = 0;
	instruction[1009].description = strdup("Computes the approximate reciprocal of the square root of the low singleprecision floating-point value in xmm2/m32 and stores the results in xmm1.");
	instruction[1009].prefixStr = strdup("REP");
	instruction[1009].binPrefixLen = 1;
	instruction[1009].binPrefix = bpstrndup("\xF3", 1);
	instruction[1009].binOpcodeLen = 2;
	instruction[1009].binOpcode = bpstrndup("\x0F\x52", 2);
	instruction[1009].opcodeOperand[0] = OA_MODRM;
	instruction[1009].opcodeOperandCount = 1;

	// Instruction Struct [1010] - SAHF
	instruction[1010].instructionStr = strdup("SAHF");
	instruction[1010].undecodedStr = strdup("9E");
	instruction[1010].valid64 = INVALID;
	instruction[1010].compatLeg = VALID;
	instruction[1010].VMX = 0;
	instruction[1010].description = strdup("Loads SF, ZF, AF, PF, and CF from AH into EFLAGS register.");
	instruction[1010].binPrefixLen = 0;
	instruction[1010].binOpcodeLen = 1;
	instruction[1010].binOpcode = bpstrndup("\x9E", 1);
	instruction[1010].opcodeOperandCount = 0;

	// Instruction Struct [1011] - SAL
	instruction[1011].instructionStr = strdup("SAL");
	instruction[1011].undecodedStr = strdup("D0 /4");
	instruction[1011].dstOperandStr = strdup("r/m8");
	instruction[1011].dstOperand = IA_RM8;
	instruction[1011].srcOperandStr = strdup("1");
	instruction[1011].srcOperand = IA_1;
	instruction[1011].valid64 = INVALID;
	instruction[1011].compatLeg = VALID;
	instruction[1011].VMX = 0;
	instruction[1011].description = strdup("Multiply r/m8 by 2, once.");
	instruction[1011].binPrefixLen = 0;
	instruction[1011].binOpcodeLen = 1;
	instruction[1011].binOpcode = bpstrndup("\xD0", 1);
	instruction[1011].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[1011].opcodeOperandCount = 1;

	// Instruction Struct [1012] - SAL
	instruction[1012].instructionStr = strdup("SAL");
	instruction[1012].undecodedStr = strdup("D2 /4");
	instruction[1012].dstOperandStr = strdup("r/m8");
	instruction[1012].dstOperand = IA_RM8;
	instruction[1012].srcOperandStr = strdup("CL");
	instruction[1012].srcOperand = IA_CL;
	instruction[1012].valid64 = INVALID;
	instruction[1012].compatLeg = VALID;
	instruction[1012].VMX = 0;
	instruction[1012].description = strdup("Multiply r/m8 by 2, CL times.");
	instruction[1012].binPrefixLen = 0;
	instruction[1012].binOpcodeLen = 1;
	instruction[1012].binOpcode = bpstrndup("\xD2", 1);
	instruction[1012].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[1012].opcodeOperandCount = 1;

	// Instruction Struct [1013] - SAL
	instruction[1013].instructionStr = strdup("SAL");
	instruction[1013].undecodedStr = strdup("C0 /4 ib");
	instruction[1013].dstOperandStr = strdup("r/m8");
	instruction[1013].dstOperand = IA_RM8;
	instruction[1013].srcOperandStr = strdup("imm8");
	instruction[1013].srcOperand = IA_IMM8;
	instruction[1013].valid64 = INVALID;
	instruction[1013].compatLeg = VALID;
	instruction[1013].VMX = 0;
	instruction[1013].description = strdup("Multiply r/m8 by 2, imm8 times.");
	instruction[1013].binPrefixLen = 0;
	instruction[1013].binOpcodeLen = 1;
	instruction[1013].binOpcode = bpstrndup("\xC0", 1);
	instruction[1013].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[1013].opcodeOperand[1] = OA_IB;
	instruction[1013].opcodeOperandCount = 2;

	// Instruction Struct [1014] - SAL
	instruction[1014].instructionStr = strdup("SAL");
	instruction[1014].undecodedStr = strdup("D1 /4");
	instruction[1014].dstOperandStr = strdup("r/m16");
	instruction[1014].dstOperand = IA_RM16;
	instruction[1014].srcOperandStr = strdup("1");
	instruction[1014].srcOperand = IA_1;
	instruction[1014].valid64 = INVALID;
	instruction[1014].compatLeg = VALID;
	instruction[1014].VMX = 0;
	instruction[1014].description = strdup("Multiply r/m16 by 2, once.");
	instruction[1014].binPrefixLen = 0;
	instruction[1014].binOpcodeLen = 1;
	instruction[1014].binOpcode = bpstrndup("\xD1", 1);
	instruction[1014].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[1014].opcodeOperandCount = 1;

	// Instruction Struct [1015] - SAL
	instruction[1015].instructionStr = strdup("SAL");
	instruction[1015].undecodedStr = strdup("D3 /4");
	instruction[1015].dstOperandStr = strdup("r/m16");
	instruction[1015].dstOperand = IA_RM16;
	instruction[1015].srcOperandStr = strdup("CL");
	instruction[1015].srcOperand = IA_CL;
	instruction[1015].valid64 = INVALID;
	instruction[1015].compatLeg = VALID;
	instruction[1015].VMX = 0;
	instruction[1015].description = strdup("Multiply r/m16 by 2, CL times.");
	instruction[1015].binPrefixLen = 0;
	instruction[1015].binOpcodeLen = 1;
	instruction[1015].binOpcode = bpstrndup("\xD3", 1);
	instruction[1015].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[1015].opcodeOperandCount = 1;

	// Instruction Struct [1016] - SAL
	instruction[1016].instructionStr = strdup("SAL");
	instruction[1016].undecodedStr = strdup("C1 /4 ib");
	instruction[1016].dstOperandStr = strdup("r/m16");
	instruction[1016].dstOperand = IA_RM16;
	instruction[1016].srcOperandStr = strdup("r/m16");
	instruction[1016].srcOperand = IA_RM16;
	instruction[1016].valid64 = INVALID;
	instruction[1016].compatLeg = VALID;
	instruction[1016].VMX = 0;
	instruction[1016].description = strdup("Multiply r/m16 by 2, imm8 times.");
	instruction[1016].binPrefixLen = 0;
	instruction[1016].binOpcodeLen = 1;
	instruction[1016].binOpcode = bpstrndup("\xC1", 1);
	instruction[1016].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[1016].opcodeOperand[1] = OA_IB;
	instruction[1016].opcodeOperandCount = 2;

	// Instruction Struct [1017] - SAL
	instruction[1017].instructionStr = strdup("SAL");
	instruction[1017].undecodedStr = strdup("D1 /4");
	instruction[1017].dstOperandStr = strdup("r/m32");
	instruction[1017].dstOperand = IA_RM32;
	instruction[1017].srcOperandStr = strdup("1");
	instruction[1017].srcOperand = IA_1;
	instruction[1017].valid64 = INVALID;
	instruction[1017].compatLeg = VALID;
	instruction[1017].VMX = 0;
	instruction[1017].description = strdup("Multiply r/m32 by 2, once.");
	instruction[1017].binPrefixLen = 0;
	instruction[1017].binOpcodeLen = 1;
	instruction[1017].binOpcode = bpstrndup("\xD1", 1);
	instruction[1017].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[1017].opcodeOperandCount = 1;

	// Instruction Struct [1018] - SAL
	instruction[1018].instructionStr = strdup("SAL");
	instruction[1018].undecodedStr = strdup("D3 /4");
	instruction[1018].dstOperandStr = strdup("r/m32");
	instruction[1018].dstOperand = IA_RM32;
	instruction[1018].srcOperandStr = strdup("CL");
	instruction[1018].srcOperand = IA_CL;
	instruction[1018].valid64 = INVALID;
	instruction[1018].compatLeg = VALID;
	instruction[1018].VMX = 0;
	instruction[1018].description = strdup("Multiply r/m32 by 2, CL times.");
	instruction[1018].binPrefixLen = 0;
	instruction[1018].binOpcodeLen = 1;
	instruction[1018].binOpcode = bpstrndup("\xD3", 1);
	instruction[1018].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[1018].opcodeOperandCount = 1;

	// Instruction Struct [1019] - SAL
	instruction[1019].instructionStr = strdup("SAL");
	instruction[1019].undecodedStr = strdup("C1 /4 ib");
	instruction[1019].dstOperandStr = strdup("r/m32");
	instruction[1019].dstOperand = IA_RM32;
	instruction[1019].srcOperandStr = strdup("imm8");
	instruction[1019].srcOperand = IA_IMM8;
	instruction[1019].valid64 = INVALID;
	instruction[1019].compatLeg = VALID;
	instruction[1019].VMX = 0;
	instruction[1019].description = strdup("Multiply r/m32 by 2, imm8 times.");
	instruction[1019].binPrefixLen = 0;
	instruction[1019].binOpcodeLen = 1;
	instruction[1019].binOpcode = bpstrndup("\xC1", 1);
	instruction[1019].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[1019].opcodeOperand[1] = OA_IB;
	instruction[1019].opcodeOperandCount = 2;

	// Instruction Struct [1020] - SAR
	instruction[1020].instructionStr = strdup("SAR");
	instruction[1020].undecodedStr = strdup("D0 /7");
	instruction[1020].dstOperandStr = strdup("r/m8");
	instruction[1020].dstOperand = IA_RM8;
	instruction[1020].srcOperandStr = strdup("1");
	instruction[1020].srcOperand = IA_1;
	instruction[1020].valid64 = INVALID;
	instruction[1020].compatLeg = VALID;
	instruction[1020].VMX = 0;
	instruction[1020].description = strdup("Signed divide* r/m8 by 2, once.");
	instruction[1020].binPrefixLen = 0;
	instruction[1020].binOpcodeLen = 1;
	instruction[1020].binOpcode = bpstrndup("\xD0", 1);
	instruction[1020].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[1020].opcodeOperandCount = 1;

	// Instruction Struct [1021] - SAR
	instruction[1021].instructionStr = strdup("SAR");
	instruction[1021].undecodedStr = strdup("D2 /7");
	instruction[1021].dstOperandStr = strdup("r/m8");
	instruction[1021].dstOperand = IA_RM8;
	instruction[1021].srcOperandStr = strdup("CL");
	instruction[1021].srcOperand = IA_CL;
	instruction[1021].valid64 = INVALID;
	instruction[1021].compatLeg = VALID;
	instruction[1021].VMX = 0;
	instruction[1021].description = strdup("Signed divide* r/m8 by 2, CL times.");
	instruction[1021].binPrefixLen = 0;
	instruction[1021].binOpcodeLen = 1;
	instruction[1021].binOpcode = bpstrndup("\xD2", 1);
	instruction[1021].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[1021].opcodeOperandCount = 1;

	// Instruction Struct [1022] - SAR
	instruction[1022].instructionStr = strdup("SAR");
	instruction[1022].undecodedStr = strdup("C0 /7 ib");
	instruction[1022].dstOperandStr = strdup("r/m8");
	instruction[1022].dstOperand = IA_RM8;
	instruction[1022].srcOperandStr = strdup("imm8");
	instruction[1022].srcOperand = IA_IMM8;
	instruction[1022].valid64 = INVALID;
	instruction[1022].compatLeg = VALID;
	instruction[1022].VMX = 0;
	instruction[1022].description = strdup("Signed divide* r/m8 by 2,");
	instruction[1022].binPrefixLen = 0;
	instruction[1022].binOpcodeLen = 1;
	instruction[1022].binOpcode = bpstrndup("\xC0", 1);
	instruction[1022].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[1022].opcodeOperand[1] = OA_IB;
	instruction[1022].opcodeOperandCount = 2;

	// Instruction Struct [1023] - SAR
	instruction[1023].instructionStr = strdup("SAR");
	instruction[1023].undecodedStr = strdup("D1 /7");
	instruction[1023].dstOperandStr = strdup("r/m16");
	instruction[1023].dstOperand = IA_RM16;
	instruction[1023].srcOperandStr = strdup("1");
	instruction[1023].srcOperand = IA_1;
	instruction[1023].valid64 = INVALID;
	instruction[1023].compatLeg = VALID;
	instruction[1023].VMX = 0;
	instruction[1023].description = strdup("Signed divide* r/m16 by 2, once.");
	instruction[1023].binPrefixLen = 0;
	instruction[1023].binOpcodeLen = 1;
	instruction[1023].binOpcode = bpstrndup("\xD1", 1);
	instruction[1023].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[1023].opcodeOperandCount = 1;

	// Instruction Struct [1024] - SAR
	instruction[1024].instructionStr = strdup("SAR");
	instruction[1024].undecodedStr = strdup("D3 /7");
	instruction[1024].dstOperandStr = strdup("r/m16");
	instruction[1024].dstOperand = IA_RM16;
	instruction[1024].srcOperandStr = strdup("CL");
	instruction[1024].srcOperand = IA_CL;
	instruction[1024].valid64 = INVALID;
	instruction[1024].compatLeg = VALID;
	instruction[1024].VMX = 0;
	instruction[1024].description = strdup("Signed divide* r/m16 by 2, CL times.");
	instruction[1024].binPrefixLen = 0;
	instruction[1024].binOpcodeLen = 1;
	instruction[1024].binOpcode = bpstrndup("\xD3", 1);
	instruction[1024].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[1024].opcodeOperandCount = 1;

	// Instruction Struct [1025] - SAR
	instruction[1025].instructionStr = strdup("SAR");
	instruction[1025].undecodedStr = strdup("C1 /7 ib");
	instruction[1025].dstOperandStr = strdup("r/m16");
	instruction[1025].dstOperand = IA_RM16;
	instruction[1025].srcOperandStr = strdup("imm8");
	instruction[1025].srcOperand = IA_IMM8;
	instruction[1025].valid64 = INVALID;
	instruction[1025].compatLeg = VALID;
	instruction[1025].VMX = 0;
	instruction[1025].description = strdup("Signed divide* r/m16 by 2, imm8 times.");
	instruction[1025].binPrefixLen = 0;
	instruction[1025].binOpcodeLen = 1;
	instruction[1025].binOpcode = bpstrndup("\xC1", 1);
	instruction[1025].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[1025].opcodeOperand[1] = OA_IB;
	instruction[1025].opcodeOperandCount = 2;

	// Instruction Struct [1026] - SAR
	instruction[1026].instructionStr = strdup("SAR");
	instruction[1026].undecodedStr = strdup("D1 /7");
	instruction[1026].dstOperandStr = strdup("r/m32");
	instruction[1026].dstOperand = IA_RM32;
	instruction[1026].srcOperandStr = strdup("1");
	instruction[1026].srcOperand = IA_1;
	instruction[1026].valid64 = INVALID;
	instruction[1026].compatLeg = VALID;
	instruction[1026].VMX = 0;
	instruction[1026].description = strdup("Signed divide* r/m32 by 2, once.");
	instruction[1026].binPrefixLen = 0;
	instruction[1026].binOpcodeLen = 1;
	instruction[1026].binOpcode = bpstrndup("\xD1", 1);
	instruction[1026].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[1026].opcodeOperandCount = 1;

	// Instruction Struct [1027] - SAR
	instruction[1027].instructionStr = strdup("SAR");
	instruction[1027].undecodedStr = strdup("D3 /7");
	instruction[1027].dstOperandStr = strdup("r/m32");
	instruction[1027].dstOperand = IA_RM32;
	instruction[1027].srcOperandStr = strdup("CL");
	instruction[1027].srcOperand = IA_CL;
	instruction[1027].valid64 = INVALID;
	instruction[1027].compatLeg = VALID;
	instruction[1027].VMX = 0;
	instruction[1027].description = strdup("Signed divide* r/m32 by 2, CL times.");
	instruction[1027].binPrefixLen = 0;
	instruction[1027].binOpcodeLen = 1;
	instruction[1027].binOpcode = bpstrndup("\xD3", 1);
	instruction[1027].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[1027].opcodeOperandCount = 1;

	// Instruction Struct [1028] - SAR
	instruction[1028].instructionStr = strdup("SAR");
	instruction[1028].undecodedStr = strdup("C1 /7 ib");
	instruction[1028].dstOperandStr = strdup("r/m32");
	instruction[1028].dstOperand = IA_RM32;
	instruction[1028].srcOperandStr = strdup("imm8");
	instruction[1028].srcOperand = IA_IMM8;
	instruction[1028].valid64 = INVALID;
	instruction[1028].compatLeg = VALID;
	instruction[1028].VMX = 0;
	instruction[1028].description = strdup("Signed divide* r/m32 by 2, imm8 times.");
	instruction[1028].binPrefixLen = 0;
	instruction[1028].binOpcodeLen = 1;
	instruction[1028].binOpcode = bpstrndup("\xC1", 1);
	instruction[1028].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[1028].opcodeOperand[1] = OA_IB;
	instruction[1028].opcodeOperandCount = 2;

	// Instruction Struct [1029] - SHL
	instruction[1029].instructionStr = strdup("SHL");
	instruction[1029].undecodedStr = strdup("D0 /4");
	instruction[1029].dstOperandStr = strdup("r/m8");
	instruction[1029].dstOperand = IA_RM8;
	instruction[1029].srcOperandStr = strdup("1");
	instruction[1029].srcOperand = IA_1;
	instruction[1029].valid64 = INVALID;
	instruction[1029].compatLeg = VALID;
	instruction[1029].VMX = 0;
	instruction[1029].description = strdup("Multiply r/m8 by 2, once.");
	instruction[1029].binPrefixLen = 0;
	instruction[1029].binOpcodeLen = 1;
	instruction[1029].binOpcode = bpstrndup("\xD0", 1);
	instruction[1029].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[1029].opcodeOperandCount = 1;

	// Instruction Struct [1030] - SHL
	instruction[1030].instructionStr = strdup("SHL");
	instruction[1030].undecodedStr = strdup("D2 /4");
	instruction[1030].dstOperandStr = strdup("r/m8");
	instruction[1030].dstOperand = IA_RM8;
	instruction[1030].srcOperandStr = strdup("CL");
	instruction[1030].srcOperand = IA_CL;
	instruction[1030].valid64 = INVALID;
	instruction[1030].compatLeg = VALID;
	instruction[1030].VMX = 0;
	instruction[1030].description = strdup("Multiply r/m8 by 2, CL times.");
	instruction[1030].binPrefixLen = 0;
	instruction[1030].binOpcodeLen = 1;
	instruction[1030].binOpcode = bpstrndup("\xD2", 1);
	instruction[1030].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[1030].opcodeOperandCount = 1;

	// Instruction Struct [1031] - SHL
	instruction[1031].instructionStr = strdup("SHL");
	instruction[1031].undecodedStr = strdup("C0 /4 ib");
	instruction[1031].dstOperandStr = strdup("r/m8");
	instruction[1031].dstOperand = IA_RM8;
	instruction[1031].srcOperandStr = strdup("imm8");
	instruction[1031].srcOperand = IA_IMM8;
	instruction[1031].valid64 = INVALID;
	instruction[1031].compatLeg = VALID;
	instruction[1031].VMX = 0;
	instruction[1031].description = strdup("Multiply r/m8 by 2, imm8 times.");
	instruction[1031].binPrefixLen = 0;
	instruction[1031].binOpcodeLen = 1;
	instruction[1031].binOpcode = bpstrndup("\xC0", 1);
	instruction[1031].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[1031].opcodeOperand[1] = OA_IB;
	instruction[1031].opcodeOperandCount = 2;

	// Instruction Struct [1032] - SHL
	instruction[1032].instructionStr = strdup("SHL");
	instruction[1032].undecodedStr = strdup("D1 /4");
	instruction[1032].dstOperandStr = strdup("r/m16");
	instruction[1032].dstOperand = IA_RM16;
	instruction[1032].srcOperandStr = strdup("1");
	instruction[1032].srcOperand = IA_1;
	instruction[1032].valid64 = INVALID;
	instruction[1032].compatLeg = VALID;
	instruction[1032].VMX = 0;
	instruction[1032].description = strdup("Multiply r/m16 by 2, once.");
	instruction[1032].binPrefixLen = 0;
	instruction[1032].binOpcodeLen = 1;
	instruction[1032].binOpcode = bpstrndup("\xD1", 1);
	instruction[1032].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[1032].opcodeOperandCount = 1;

	// Instruction Struct [1033] - SHL
	instruction[1033].instructionStr = strdup("SHL");
	instruction[1033].undecodedStr = strdup("D3 /4");
	instruction[1033].dstOperandStr = strdup("r/m16");
	instruction[1033].dstOperand = IA_RM16;
	instruction[1033].srcOperandStr = strdup("CL");
	instruction[1033].srcOperand = IA_CL;
	instruction[1033].valid64 = INVALID;
	instruction[1033].compatLeg = VALID;
	instruction[1033].VMX = 0;
	instruction[1033].description = strdup("Multiply r/m16 by 2, CL times.");
	instruction[1033].binPrefixLen = 0;
	instruction[1033].binOpcodeLen = 1;
	instruction[1033].binOpcode = bpstrndup("\xD3", 1);
	instruction[1033].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[1033].opcodeOperandCount = 1;

	// Instruction Struct [1034] - SHL
	instruction[1034].instructionStr = strdup("SHL");
	instruction[1034].undecodedStr = strdup("C1 /4 ib");
	instruction[1034].dstOperandStr = strdup("r/m16");
	instruction[1034].dstOperand = IA_RM16;
	instruction[1034].srcOperandStr = strdup("imm8");
	instruction[1034].srcOperand = IA_IMM8;
	instruction[1034].valid64 = INVALID;
	instruction[1034].compatLeg = VALID;
	instruction[1034].VMX = 0;
	instruction[1034].description = strdup("Multiply r/m16 by 2, imm8 times.");
	instruction[1034].binPrefixLen = 0;
	instruction[1034].binOpcodeLen = 1;
	instruction[1034].binOpcode = bpstrndup("\xC1", 1);
	instruction[1034].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[1034].opcodeOperand[1] = OA_IB;
	instruction[1034].opcodeOperandCount = 2;

	// Instruction Struct [1035] - SHL
	instruction[1035].instructionStr = strdup("SHL");
	instruction[1035].undecodedStr = strdup("D1 /4");
	instruction[1035].dstOperandStr = strdup("r/m32");
	instruction[1035].dstOperand = IA_RM32;
	instruction[1035].srcOperandStr = strdup("1");
	instruction[1035].srcOperand = IA_1;
	instruction[1035].valid64 = INVALID;
	instruction[1035].compatLeg = VALID;
	instruction[1035].VMX = 0;
	instruction[1035].description = strdup("Multiply r/m32 by 2, once.");
	instruction[1035].binPrefixLen = 0;
	instruction[1035].binOpcodeLen = 1;
	instruction[1035].binOpcode = bpstrndup("\xD1", 1);
	instruction[1035].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[1035].opcodeOperandCount = 1;

	// Instruction Struct [1036] - SHL
	instruction[1036].instructionStr = strdup("SHL");
	instruction[1036].undecodedStr = strdup("D3 /4");
	instruction[1036].dstOperandStr = strdup("r/m32");
	instruction[1036].dstOperand = IA_RM32;
	instruction[1036].srcOperandStr = strdup("CL");
	instruction[1036].srcOperand = IA_CL;
	instruction[1036].valid64 = INVALID;
	instruction[1036].compatLeg = VALID;
	instruction[1036].VMX = 0;
	instruction[1036].description = strdup("Multiply r/m32 by 2, CL times.");
	instruction[1036].binPrefixLen = 0;
	instruction[1036].binOpcodeLen = 1;
	instruction[1036].binOpcode = bpstrndup("\xD3", 1);
	instruction[1036].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[1036].opcodeOperandCount = 1;

	// Instruction Struct [1037] - SHL
	instruction[1037].instructionStr = strdup("SHL");
	instruction[1037].undecodedStr = strdup("C1 /4 ib");
	instruction[1037].dstOperandStr = strdup("r/m32");
	instruction[1037].dstOperand = IA_RM32;
	instruction[1037].srcOperandStr = strdup("imm8");
	instruction[1037].srcOperand = IA_IMM8;
	instruction[1037].valid64 = INVALID;
	instruction[1037].compatLeg = VALID;
	instruction[1037].VMX = 0;
	instruction[1037].description = strdup("Multiply r/m32 by 2, imm8 times.");
	instruction[1037].binPrefixLen = 0;
	instruction[1037].binOpcodeLen = 1;
	instruction[1037].binOpcode = bpstrndup("\xC1", 1);
	instruction[1037].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[1037].opcodeOperand[1] = OA_IB;
	instruction[1037].opcodeOperandCount = 2;

	// Instruction Struct [1038] - SHR
	instruction[1038].instructionStr = strdup("SHR");
	instruction[1038].undecodedStr = strdup("D0 /5");
	instruction[1038].dstOperandStr = strdup("r/m8");
	instruction[1038].dstOperand = IA_RM8;
	instruction[1038].srcOperandStr = strdup("1");
	instruction[1038].srcOperand = IA_1;
	instruction[1038].valid64 = INVALID;
	instruction[1038].compatLeg = VALID;
	instruction[1038].VMX = 0;
	instruction[1038].description = strdup("Unsigned divide r/m8 by 2, once.");
	instruction[1038].binPrefixLen = 0;
	instruction[1038].binOpcodeLen = 1;
	instruction[1038].binOpcode = bpstrndup("\xD0", 1);
	instruction[1038].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[1038].opcodeOperandCount = 1;

	// Instruction Struct [1039] - SHR
	instruction[1039].instructionStr = strdup("SHR");
	instruction[1039].undecodedStr = strdup("D2 /5");
	instruction[1039].dstOperandStr = strdup("r/m8");
	instruction[1039].dstOperand = IA_RM8;
	instruction[1039].srcOperandStr = strdup("CL");
	instruction[1039].srcOperand = IA_CL;
	instruction[1039].valid64 = INVALID;
	instruction[1039].compatLeg = VALID;
	instruction[1039].VMX = 0;
	instruction[1039].description = strdup("Unsigned divide r/m8 by 2, CL times.");
	instruction[1039].binPrefixLen = 0;
	instruction[1039].binOpcodeLen = 1;
	instruction[1039].binOpcode = bpstrndup("\xD2", 1);
	instruction[1039].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[1039].opcodeOperandCount = 1;

	// Instruction Struct [1040] - SHR
	instruction[1040].instructionStr = strdup("SHR");
	instruction[1040].undecodedStr = strdup("C0 /5 ib");
	instruction[1040].dstOperandStr = strdup("r/m8");
	instruction[1040].dstOperand = IA_RM8;
	instruction[1040].srcOperandStr = strdup("imm8");
	instruction[1040].srcOperand = IA_IMM8;
	instruction[1040].valid64 = INVALID;
	instruction[1040].compatLeg = VALID;
	instruction[1040].VMX = 0;
	instruction[1040].description = strdup("Unsigned divide r/m8 by 2, imm8 times.");
	instruction[1040].binPrefixLen = 0;
	instruction[1040].binOpcodeLen = 1;
	instruction[1040].binOpcode = bpstrndup("\xC0", 1);
	instruction[1040].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[1040].opcodeOperand[1] = OA_IB;
	instruction[1040].opcodeOperandCount = 2;

	// Instruction Struct [1041] - SHR
	instruction[1041].instructionStr = strdup("SHR");
	instruction[1041].undecodedStr = strdup("D1 /5");
	instruction[1041].dstOperandStr = strdup("r/m16");
	instruction[1041].dstOperand = IA_RM16;
	instruction[1041].srcOperandStr = strdup("1");
	instruction[1041].srcOperand = IA_1;
	instruction[1041].valid64 = INVALID;
	instruction[1041].compatLeg = VALID;
	instruction[1041].VMX = 0;
	instruction[1041].description = strdup("Unsigned divide r/m16 by 2, once.");
	instruction[1041].binPrefixLen = 0;
	instruction[1041].binOpcodeLen = 1;
	instruction[1041].binOpcode = bpstrndup("\xD1", 1);
	instruction[1041].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[1041].opcodeOperandCount = 1;

	// Instruction Struct [1042] - SHR
	instruction[1042].instructionStr = strdup("SHR");
	instruction[1042].undecodedStr = strdup("D3 /5");
	instruction[1042].dstOperandStr = strdup("r/m16");
	instruction[1042].dstOperand = IA_RM16;
	instruction[1042].srcOperandStr = strdup("CL");
	instruction[1042].srcOperand = IA_CL;
	instruction[1042].valid64 = INVALID;
	instruction[1042].compatLeg = VALID;
	instruction[1042].VMX = 0;
	instruction[1042].description = strdup("Unsigned divide r/m16 by 2, CL times");
	instruction[1042].binPrefixLen = 0;
	instruction[1042].binOpcodeLen = 1;
	instruction[1042].binOpcode = bpstrndup("\xD3", 1);
	instruction[1042].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[1042].opcodeOperandCount = 1;

	// Instruction Struct [1043] - SHR
	instruction[1043].instructionStr = strdup("SHR");
	instruction[1043].undecodedStr = strdup("C1 /5 ib");
	instruction[1043].dstOperandStr = strdup("r/m16");
	instruction[1043].dstOperand = IA_RM16;
	instruction[1043].srcOperandStr = strdup("imm8");
	instruction[1043].srcOperand = IA_IMM8;
	instruction[1043].valid64 = INVALID;
	instruction[1043].compatLeg = VALID;
	instruction[1043].VMX = 0;
	instruction[1043].description = strdup("Unsigned divide r/m16 by 2, imm8 times.");
	instruction[1043].binPrefixLen = 0;
	instruction[1043].binOpcodeLen = 1;
	instruction[1043].binOpcode = bpstrndup("\xC1", 1);
	instruction[1043].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[1043].opcodeOperand[1] = OA_IB;
	instruction[1043].opcodeOperandCount = 2;

	// Instruction Struct [1044] - SHR
	instruction[1044].instructionStr = strdup("SHR");
	instruction[1044].undecodedStr = strdup("D1 /5");
	instruction[1044].dstOperandStr = strdup("r/m32");
	instruction[1044].dstOperand = IA_RM32;
	instruction[1044].srcOperandStr = strdup("1");
	instruction[1044].srcOperand = IA_1;
	instruction[1044].valid64 = INVALID;
	instruction[1044].compatLeg = VALID;
	instruction[1044].VMX = 0;
	instruction[1044].description = strdup("Unsigned divide r/m32 by 2, once.");
	instruction[1044].binPrefixLen = 0;
	instruction[1044].binOpcodeLen = 1;
	instruction[1044].binOpcode = bpstrndup("\xD1", 1);
	instruction[1044].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[1044].opcodeOperandCount = 1;

	// Instruction Struct [1045] - SHR
	instruction[1045].instructionStr = strdup("SHR");
	instruction[1045].undecodedStr = strdup("D3 /5");
	instruction[1045].dstOperandStr = strdup("r/m32");
	instruction[1045].dstOperand = IA_RM32;
	instruction[1045].srcOperandStr = strdup("CL");
	instruction[1045].srcOperand = IA_CL;
	instruction[1045].valid64 = INVALID;
	instruction[1045].compatLeg = VALID;
	instruction[1045].VMX = 0;
	instruction[1045].description = strdup("Unsigned divide r/m32 by 2, CL times.");
	instruction[1045].binPrefixLen = 0;
	instruction[1045].binOpcodeLen = 1;
	instruction[1045].binOpcode = bpstrndup("\xD3", 1);
	instruction[1045].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[1045].opcodeOperandCount = 1;

	// Instruction Struct [1046] - SHR
	instruction[1046].instructionStr = strdup("SHR");
	instruction[1046].undecodedStr = strdup("C1 /5 ib");
	instruction[1046].dstOperandStr = strdup("r/m32");
	instruction[1046].dstOperand = IA_RM32;
	instruction[1046].srcOperandStr = strdup("imm8");
	instruction[1046].srcOperand = IA_IMM8;
	instruction[1046].valid64 = INVALID;
	instruction[1046].compatLeg = VALID;
	instruction[1046].VMX = 0;
	instruction[1046].description = strdup("Unsigned divide r/m32 by 2, imm8 times.");
	instruction[1046].binPrefixLen = 0;
	instruction[1046].binOpcodeLen = 1;
	instruction[1046].binOpcode = bpstrndup("\xC1", 1);
	instruction[1046].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[1046].opcodeOperand[1] = OA_IB;
	instruction[1046].opcodeOperandCount = 2;

	// Instruction Struct [1047] - SBB
	instruction[1047].instructionStr = strdup("SBB");
	instruction[1047].undecodedStr = strdup("1C ib");
	instruction[1047].dstOperandStr = strdup("AL");
	instruction[1047].dstOperand = IA_AL;
	instruction[1047].srcOperandStr = strdup("imm8");
	instruction[1047].srcOperand = IA_IMM8;
	instruction[1047].valid64 = INVALID;
	instruction[1047].compatLeg = VALID;
	instruction[1047].VMX = 0;
	instruction[1047].description = strdup("Subtract with borrow imm8");
	instruction[1047].binPrefixLen = 0;
	instruction[1047].binOpcodeLen = 1;
	instruction[1047].binOpcode = bpstrndup("\x1C", 1);
	instruction[1047].opcodeOperand[0] = OA_IB;
	instruction[1047].opcodeOperandCount = 1;

	// Instruction Struct [1048] - SBB
	instruction[1048].instructionStr = strdup("SBB");
	instruction[1048].undecodedStr = strdup("1D iw");
	instruction[1048].dstOperandStr = strdup("AX");
	instruction[1048].dstOperand = IA_AX;
	instruction[1048].srcOperandStr = strdup("imm16");
	instruction[1048].srcOperand = IA_IMM16;
	instruction[1048].valid64 = INVALID;
	instruction[1048].compatLeg = VALID;
	instruction[1048].VMX = 0;
	instruction[1048].description = strdup("Subtract with borrow imm16 from AX.");
	instruction[1048].binPrefixLen = 0;
	instruction[1048].binOpcodeLen = 1;
	instruction[1048].binOpcode = bpstrndup("\x1D", 1);
	instruction[1048].opcodeOperand[0] = OA_IW;
	instruction[1048].opcodeOperandCount = 1;

	// Instruction Struct [1049] - SBB
	instruction[1049].instructionStr = strdup("SBB");
	instruction[1049].undecodedStr = strdup("1D id");
	instruction[1049].dstOperandStr = strdup("EAX");
	instruction[1049].dstOperand = IA_EAX;
	instruction[1049].srcOperandStr = strdup("imm32");
	instruction[1049].srcOperand = IA_IMM32;
	instruction[1049].valid64 = INVALID;
	instruction[1049].compatLeg = VALID;
	instruction[1049].VMX = 0;
	instruction[1049].description = strdup("Subtract with borrow imm32 from EAX.");
	instruction[1049].binPrefixLen = 0;
	instruction[1049].binOpcodeLen = 1;
	instruction[1049].binOpcode = bpstrndup("\x1D", 1);
	instruction[1049].opcodeOperand[0] = OA_ID;
	instruction[1049].opcodeOperandCount = 1;

	// Instruction Struct [1050] - SBB
	instruction[1050].instructionStr = strdup("SBB");
	instruction[1050].undecodedStr = strdup("80 /3 ib");
	instruction[1050].dstOperandStr = strdup("r/m8");
	instruction[1050].dstOperand = IA_RM8;
	instruction[1050].srcOperandStr = strdup("imm8");
	instruction[1050].srcOperand = IA_IMM8;
	instruction[1050].valid64 = INVALID;
	instruction[1050].compatLeg = VALID;
	instruction[1050].VMX = 0;
	instruction[1050].description = strdup("Subtract with borrow imm8 from r/m8.");
	instruction[1050].binPrefixLen = 0;
	instruction[1050].binOpcodeLen = 1;
	instruction[1050].binOpcode = bpstrndup("\x80", 1);
	instruction[1050].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[1050].opcodeOperand[1] = OA_IB;
	instruction[1050].opcodeOperandCount = 2;

	// Instruction Struct [1051] - SBB
	instruction[1051].instructionStr = strdup("SBB");
	instruction[1051].undecodedStr = strdup("81 /3 iw");
	instruction[1051].dstOperandStr = strdup("r/m16");
	instruction[1051].dstOperand = IA_RM16;
	instruction[1051].srcOperandStr = strdup("imm16");
	instruction[1051].srcOperand = IA_IMM16;
	instruction[1051].valid64 = INVALID;
	instruction[1051].compatLeg = VALID;
	instruction[1051].VMX = 0;
	instruction[1051].description = strdup("Subtract with borrow imm16 from r/m16.");
	instruction[1051].binPrefixLen = 0;
	instruction[1051].binOpcodeLen = 1;
	instruction[1051].binOpcode = bpstrndup("\x81", 1);
	instruction[1051].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[1051].opcodeOperand[1] = OA_IW;
	instruction[1051].opcodeOperandCount = 2;

	// Instruction Struct [1052] - SBB
	instruction[1052].instructionStr = strdup("SBB");
	instruction[1052].undecodedStr = strdup("81 /3 id");
	instruction[1052].dstOperandStr = strdup("r/m32");
	instruction[1052].dstOperand = IA_RM32;
	instruction[1052].srcOperandStr = strdup("imm32");
	instruction[1052].srcOperand = IA_IMM32;
	instruction[1052].valid64 = INVALID;
	instruction[1052].compatLeg = VALID;
	instruction[1052].VMX = 0;
	instruction[1052].description = strdup("Subtract with borrow imm32 from r/m32.");
	instruction[1052].binPrefixLen = 0;
	instruction[1052].binOpcodeLen = 1;
	instruction[1052].binOpcode = bpstrndup("\x81", 1);
	instruction[1052].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[1052].opcodeOperand[1] = OA_ID;
	instruction[1052].opcodeOperandCount = 2;

	// Instruction Struct [1053] - SBB
	instruction[1053].instructionStr = strdup("SBB");
	instruction[1053].undecodedStr = strdup("83 /3 ib");
	instruction[1053].dstOperandStr = strdup("r/m16");
	instruction[1053].dstOperand = IA_RM16;
	instruction[1053].srcOperandStr = strdup("imm8");
	instruction[1053].srcOperand = IA_IMM8;
	instruction[1053].valid64 = INVALID;
	instruction[1053].compatLeg = VALID;
	instruction[1053].VMX = 0;
	instruction[1053].description = strdup("Subtract with borrow signextended imm8 from r/m16.");
	instruction[1053].binPrefixLen = 0;
	instruction[1053].binOpcodeLen = 1;
	instruction[1053].binOpcode = bpstrndup("\x83", 1);
	instruction[1053].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[1053].opcodeOperand[1] = OA_IB;
	instruction[1053].opcodeOperandCount = 2;

	// Instruction Struct [1054] - SBB
	instruction[1054].instructionStr = strdup("SBB");
	instruction[1054].undecodedStr = strdup("83 /3 ib");
	instruction[1054].dstOperandStr = strdup("r/m32");
	instruction[1054].dstOperand = IA_RM32;
	instruction[1054].srcOperandStr = strdup("imm8");
	instruction[1054].srcOperand = IA_IMM8;
	instruction[1054].valid64 = INVALID;
	instruction[1054].compatLeg = VALID;
	instruction[1054].VMX = 0;
	instruction[1054].description = strdup("Subtract with borrow signextended imm8 from r/m32.");
	instruction[1054].binPrefixLen = 0;
	instruction[1054].binOpcodeLen = 1;
	instruction[1054].binOpcode = bpstrndup("\x83", 1);
	instruction[1054].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[1054].opcodeOperand[1] = OA_IB;
	instruction[1054].opcodeOperandCount = 2;

	// Instruction Struct [1055] - SBB
	instruction[1055].instructionStr = strdup("SBB");
	instruction[1055].undecodedStr = strdup("18 /r");
	instruction[1055].dstOperandStr = strdup("r/m8");
	instruction[1055].dstOperand = IA_RM8;
	instruction[1055].srcOperandStr = strdup("r8");
	instruction[1055].srcOperand = IA_R8;
	instruction[1055].valid64 = INVALID;
	instruction[1055].compatLeg = VALID;
	instruction[1055].VMX = 0;
	instruction[1055].description = strdup("Subtract with borrow r8 from r/m8.");
	instruction[1055].binPrefixLen = 0;
	instruction[1055].binOpcodeLen = 1;
	instruction[1055].binOpcode = bpstrndup("\x18", 1);
	instruction[1055].opcodeOperand[0] = OA_MODRM;
	instruction[1055].opcodeOperandCount = 1;

	// Instruction Struct [1056] - SBB
	instruction[1056].instructionStr = strdup("SBB");
	instruction[1056].undecodedStr = strdup("19 /r");
	instruction[1056].dstOperandStr = strdup("r/m16");
	instruction[1056].dstOperand = IA_RM16;
	instruction[1056].srcOperandStr = strdup("r16");
	instruction[1056].srcOperand = IA_R16;
	instruction[1056].valid64 = INVALID;
	instruction[1056].compatLeg = VALID;
	instruction[1056].VMX = 0;
	instruction[1056].description = strdup("Subtract with borrow r16 from r/m16.");
	instruction[1056].binPrefixLen = 0;
	instruction[1056].binOpcodeLen = 1;
	instruction[1056].binOpcode = bpstrndup("\x19", 1);
	instruction[1056].opcodeOperand[0] = OA_MODRM;
	instruction[1056].opcodeOperandCount = 1;

	// Instruction Struct [1057] - SBB
	instruction[1057].instructionStr = strdup("SBB");
	instruction[1057].undecodedStr = strdup("19 /r");
	instruction[1057].dstOperandStr = strdup("r/m32");
	instruction[1057].dstOperand = IA_RM32;
	instruction[1057].srcOperandStr = strdup("r32");
	instruction[1057].srcOperand = IA_R32;
	instruction[1057].valid64 = INVALID;
	instruction[1057].compatLeg = VALID;
	instruction[1057].VMX = 0;
	instruction[1057].description = strdup("Subtract with borrow r32 from r/m32.");
	instruction[1057].binPrefixLen = 0;
	instruction[1057].binOpcodeLen = 1;
	instruction[1057].binOpcode = bpstrndup("\x19", 1);
	instruction[1057].opcodeOperand[0] = OA_MODRM;
	instruction[1057].opcodeOperandCount = 1;

	// Instruction Struct [1058] - SBB
	instruction[1058].instructionStr = strdup("SBB");
	instruction[1058].undecodedStr = strdup("1A /r");
	instruction[1058].dstOperandStr = strdup("r8");
	instruction[1058].dstOperand = IA_R8;
	instruction[1058].srcOperandStr = strdup("r/m8");
	instruction[1058].srcOperand = IA_RM8;
	instruction[1058].valid64 = INVALID;
	instruction[1058].compatLeg = VALID;
	instruction[1058].VMX = 0;
	instruction[1058].description = strdup("Subtract with borrow r/m8 from r8.");
	instruction[1058].binPrefixLen = 0;
	instruction[1058].binOpcodeLen = 1;
	instruction[1058].binOpcode = bpstrndup("\x1A", 1);
	instruction[1058].opcodeOperand[0] = OA_MODRM;
	instruction[1058].opcodeOperandCount = 1;

	// Instruction Struct [1059] - SBB
	instruction[1059].instructionStr = strdup("SBB");
	instruction[1059].undecodedStr = strdup("1B /r");
	instruction[1059].dstOperandStr = strdup("r16");
	instruction[1059].dstOperand = IA_R16;
	instruction[1059].srcOperandStr = strdup("r/m16");
	instruction[1059].srcOperand = IA_RM16;
	instruction[1059].valid64 = INVALID;
	instruction[1059].compatLeg = VALID;
	instruction[1059].VMX = 0;
	instruction[1059].description = strdup("Subtract with borrow r/m16 from r16.");
	instruction[1059].binPrefixLen = 0;
	instruction[1059].binOpcodeLen = 1;
	instruction[1059].binOpcode = bpstrndup("\x1B", 1);
	instruction[1059].opcodeOperand[0] = OA_MODRM;
	instruction[1059].opcodeOperandCount = 1;

	// Instruction Struct [1060] - SBB
	instruction[1060].instructionStr = strdup("SBB");
	instruction[1060].undecodedStr = strdup("1B /r");
	instruction[1060].dstOperandStr = strdup("r32");
	instruction[1060].dstOperand = IA_R32;
	instruction[1060].srcOperandStr = strdup("r/m32");
	instruction[1060].srcOperand = IA_RM32;
	instruction[1060].valid64 = INVALID;
	instruction[1060].compatLeg = VALID;
	instruction[1060].VMX = 0;
	instruction[1060].description = strdup("Subtract with borrow r/m32 from r32.");
	instruction[1060].binPrefixLen = 0;
	instruction[1060].binOpcodeLen = 1;
	instruction[1060].binOpcode = bpstrndup("\x1B", 1);
	instruction[1060].opcodeOperand[0] = OA_MODRM;
	instruction[1060].opcodeOperandCount = 1;

	// Instruction Struct [1061] - SCAS
	instruction[1061].instructionStr = strdup("SCAS");
	instruction[1061].undecodedStr = strdup("AE");
	instruction[1061].dstOperandStr = strdup("m8");
	instruction[1061].dstOperand = IA_M8;
	instruction[1061].valid64 = INVALID;
	instruction[1061].compatLeg = VALID;
	instruction[1061].VMX = 0;
	instruction[1061].description = strdup("Compare AL with byte at ES:(E)DI or RDI, then set status flags.");
	instruction[1061].binPrefixLen = 0;
	instruction[1061].binOpcodeLen = 1;
	instruction[1061].binOpcode = bpstrndup("\xAE", 1);
	instruction[1061].opcodeOperandCount = 0;

	// Instruction Struct [1062] - SCAS
	instruction[1062].instructionStr = strdup("SCAS");
	instruction[1062].undecodedStr = strdup("AF");
	instruction[1062].dstOperandStr = strdup("m16");
	instruction[1062].dstOperand = IA_M16;
	instruction[1062].valid64 = INVALID;
	instruction[1062].compatLeg = VALID;
	instruction[1062].VMX = 0;
	instruction[1062].description = strdup("Compare AX with word at ES:(E)DI or RDI, then set status flags.");
	instruction[1062].binPrefixLen = 0;
	instruction[1062].binOpcodeLen = 1;
	instruction[1062].binOpcode = bpstrndup("\xAF", 1);
	instruction[1062].opcodeOperandCount = 0;

	// Instruction Struct [1063] - SCAS
	instruction[1063].instructionStr = strdup("SCAS");
	instruction[1063].undecodedStr = strdup("AF");
	instruction[1063].dstOperandStr = strdup("m32");
	instruction[1063].dstOperand = IA_M32;
	instruction[1063].valid64 = INVALID;
	instruction[1063].compatLeg = VALID;
	instruction[1063].VMX = 0;
	instruction[1063].description = strdup("Compare EAX with doubleword at ES(E)DI or RDI then set status flags.");
	instruction[1063].binPrefixLen = 0;
	instruction[1063].binOpcodeLen = 1;
	instruction[1063].binOpcode = bpstrndup("\xAF", 1);
	instruction[1063].opcodeOperandCount = 0;

	// Instruction Struct [1064] - SCASB
	instruction[1064].instructionStr = strdup("SCASB");
	instruction[1064].undecodedStr = strdup("AE");
	instruction[1064].valid64 = INVALID;
	instruction[1064].compatLeg = VALID;
	instruction[1064].VMX = 0;
	instruction[1064].description = strdup("Compare AL with byte at ES:(E)DI or RDI then set status flags.");
	instruction[1064].binPrefixLen = 0;
	instruction[1064].binOpcodeLen = 1;
	instruction[1064].binOpcode = bpstrndup("\xAE", 1);
	instruction[1064].opcodeOperandCount = 0;

	// Instruction Struct [1065] - SCASW
	instruction[1065].instructionStr = strdup("SCASW");
	instruction[1065].undecodedStr = strdup("AF");
	instruction[1065].valid64 = INVALID;
	instruction[1065].compatLeg = VALID;
	instruction[1065].VMX = 0;
	instruction[1065].description = strdup("Compare AX with word at ES:(E)DI or RDI then set status flags.");
	instruction[1065].binPrefixLen = 0;
	instruction[1065].binOpcodeLen = 1;
	instruction[1065].binOpcode = bpstrndup("\xAF", 1);
	instruction[1065].opcodeOperandCount = 0;

	// Instruction Struct [1066] - SCASD
	instruction[1066].instructionStr = strdup("SCASD");
	instruction[1066].undecodedStr = strdup("AF");
	instruction[1066].valid64 = INVALID;
	instruction[1066].compatLeg = VALID;
	instruction[1066].VMX = 0;
	instruction[1066].description = strdup("Compare EAX with doubleword at ES:(E)DI or RDI then set status flags.");
	instruction[1066].binPrefixLen = 0;
	instruction[1066].binOpcodeLen = 1;
	instruction[1066].binOpcode = bpstrndup("\xAF", 1);
	instruction[1066].opcodeOperandCount = 0;

	// Instruction Struct [1067] - SETA
	instruction[1067].instructionStr = strdup("SETA");
	instruction[1067].undecodedStr = strdup("0F 97");
	instruction[1067].dstOperandStr = strdup("r/m8");
	instruction[1067].dstOperand = IA_RM8;
	instruction[1067].valid64 = INVALID;
	instruction[1067].compatLeg = VALID;
	instruction[1067].VMX = 0;
	instruction[1067].description = strdup("Set byte if above (CF=0 and ZF=0).");
	instruction[1067].binPrefixLen = 0;
	instruction[1067].binOpcodeLen = 2;
	instruction[1067].binOpcode = bpstrndup("\x0F\x97", 2);
	instruction[1067].opcodeOperandCount = 0;

	// Instruction Struct [1068] - SETAE
	instruction[1068].instructionStr = strdup("SETAE");
	instruction[1068].undecodedStr = strdup("0F 93");
	instruction[1068].dstOperandStr = strdup("r/m8");
	instruction[1068].dstOperand = IA_RM8;
	instruction[1068].valid64 = INVALID;
	instruction[1068].compatLeg = VALID;
	instruction[1068].VMX = 0;
	instruction[1068].description = strdup("Set byte if above or equal (CF=0).");
	instruction[1068].binPrefixLen = 0;
	instruction[1068].binOpcodeLen = 2;
	instruction[1068].binOpcode = bpstrndup("\x0F\x93", 2);
	instruction[1068].opcodeOperandCount = 0;

	// Instruction Struct [1069] - SETB
	instruction[1069].instructionStr = strdup("SETB");
	instruction[1069].undecodedStr = strdup("0F 92");
	instruction[1069].dstOperandStr = strdup("r/m8");
	instruction[1069].dstOperand = IA_RM8;
	instruction[1069].valid64 = INVALID;
	instruction[1069].compatLeg = VALID;
	instruction[1069].VMX = 0;
	instruction[1069].description = strdup("Set byte if below (CF=1).");
	instruction[1069].binPrefixLen = 0;
	instruction[1069].binOpcodeLen = 2;
	instruction[1069].binOpcode = bpstrndup("\x0F\x92", 2);
	instruction[1069].opcodeOperandCount = 0;

	// Instruction Struct [1070] - SETBE
	instruction[1070].instructionStr = strdup("SETBE");
	instruction[1070].undecodedStr = strdup("0F 96");
	instruction[1070].dstOperandStr = strdup("r/m8");
	instruction[1070].dstOperand = IA_RM8;
	instruction[1070].valid64 = INVALID;
	instruction[1070].compatLeg = VALID;
	instruction[1070].VMX = 0;
	instruction[1070].description = strdup("Set byte if below or equal (CF=1 or ZF=1).");
	instruction[1070].binPrefixLen = 0;
	instruction[1070].binOpcodeLen = 2;
	instruction[1070].binOpcode = bpstrndup("\x0F\x96", 2);
	instruction[1070].opcodeOperandCount = 0;

	// Instruction Struct [1071] - SETC
	instruction[1071].instructionStr = strdup("SETC");
	instruction[1071].undecodedStr = strdup("0F 92");
	instruction[1071].dstOperandStr = strdup("r/m8");
	instruction[1071].dstOperand = IA_RM8;
	instruction[1071].valid64 = INVALID;
	instruction[1071].compatLeg = VALID;
	instruction[1071].VMX = 0;
	instruction[1071].description = strdup("Set byte if carry (CF=1).");
	instruction[1071].binPrefixLen = 0;
	instruction[1071].binOpcodeLen = 2;
	instruction[1071].binOpcode = bpstrndup("\x0F\x92", 2);
	instruction[1071].opcodeOperandCount = 0;

	// Instruction Struct [1072] - SETE
	instruction[1072].instructionStr = strdup("SETE");
	instruction[1072].undecodedStr = strdup("0F 94");
	instruction[1072].dstOperandStr = strdup("r/m8");
	instruction[1072].dstOperand = IA_RM8;
	instruction[1072].valid64 = INVALID;
	instruction[1072].compatLeg = VALID;
	instruction[1072].VMX = 0;
	instruction[1072].description = strdup("Set byte if equal (ZF=1).");
	instruction[1072].binPrefixLen = 0;
	instruction[1072].binOpcodeLen = 2;
	instruction[1072].binOpcode = bpstrndup("\x0F\x94", 2);
	instruction[1072].opcodeOperandCount = 0;

	// Instruction Struct [1073] - SETG
	instruction[1073].instructionStr = strdup("SETG");
	instruction[1073].undecodedStr = strdup("0F 9F");
	instruction[1073].dstOperandStr = strdup("r/m8");
	instruction[1073].dstOperand = IA_RM8;
	instruction[1073].valid64 = INVALID;
	instruction[1073].compatLeg = VALID;
	instruction[1073].VMX = 0;
	instruction[1073].description = strdup("Set byte if greater (ZF=0 and SF=OF).");
	instruction[1073].binPrefixLen = 0;
	instruction[1073].binOpcodeLen = 2;
	instruction[1073].binOpcode = bpstrndup("\x0F\x9F", 2);
	instruction[1073].opcodeOperandCount = 0;

	// Instruction Struct [1074] - SETGE
	instruction[1074].instructionStr = strdup("SETGE");
	instruction[1074].undecodedStr = strdup("0F 9D");
	instruction[1074].dstOperandStr = strdup("r/m8");
	instruction[1074].dstOperand = IA_RM8;
	instruction[1074].valid64 = INVALID;
	instruction[1074].compatLeg = VALID;
	instruction[1074].VMX = 0;
	instruction[1074].description = strdup("Set byte if greater or equal (SF=OF).");
	instruction[1074].binPrefixLen = 0;
	instruction[1074].binOpcodeLen = 2;
	instruction[1074].binOpcode = bpstrndup("\x0F\x9D", 2);
	instruction[1074].opcodeOperandCount = 0;

	// Instruction Struct [1075] - SETL
	instruction[1075].instructionStr = strdup("SETL");
	instruction[1075].undecodedStr = strdup("0F 9C");
	instruction[1075].dstOperandStr = strdup("r/m8");
	instruction[1075].dstOperand = IA_RM8;
	instruction[1075].valid64 = INVALID;
	instruction[1075].compatLeg = VALID;
	instruction[1075].VMX = 0;
	instruction[1075].description = strdup("Set byte if less (SF? OF).");
	instruction[1075].binPrefixLen = 0;
	instruction[1075].binOpcodeLen = 2;
	instruction[1075].binOpcode = bpstrndup("\x0F\x9C", 2);
	instruction[1075].opcodeOperandCount = 0;

	// Instruction Struct [1076] - SETLE
	instruction[1076].instructionStr = strdup("SETLE");
	instruction[1076].undecodedStr = strdup("0F 9E");
	instruction[1076].dstOperandStr = strdup("r/m8");
	instruction[1076].dstOperand = IA_RM8;
	instruction[1076].valid64 = INVALID;
	instruction[1076].compatLeg = VALID;
	instruction[1076].VMX = 0;
	instruction[1076].description = strdup("Set byte if less or equal (ZF=1 or SF? OF).");
	instruction[1076].binPrefixLen = 0;
	instruction[1076].binOpcodeLen = 2;
	instruction[1076].binOpcode = bpstrndup("\x0F\x9E", 2);
	instruction[1076].opcodeOperandCount = 0;

	// Instruction Struct [1077] - SETNA
	instruction[1077].instructionStr = strdup("SETNA");
	instruction[1077].undecodedStr = strdup("0F 96");
	instruction[1077].dstOperandStr = strdup("r/m8");
	instruction[1077].dstOperand = IA_RM8;
	instruction[1077].valid64 = INVALID;
	instruction[1077].compatLeg = VALID;
	instruction[1077].VMX = 0;
	instruction[1077].description = strdup("Set byte if not above (CF=1 or ZF=1).");
	instruction[1077].binPrefixLen = 0;
	instruction[1077].binOpcodeLen = 2;
	instruction[1077].binOpcode = bpstrndup("\x0F\x96", 2);
	instruction[1077].opcodeOperandCount = 0;

	// Instruction Struct [1078] - SETNAE
	instruction[1078].instructionStr = strdup("SETNAE");
	instruction[1078].undecodedStr = strdup("0F 92");
	instruction[1078].dstOperandStr = strdup("r/m8");
	instruction[1078].dstOperand = IA_RM8;
	instruction[1078].valid64 = INVALID;
	instruction[1078].compatLeg = VALID;
	instruction[1078].VMX = 0;
	instruction[1078].description = strdup("Set byte if not above or equal (CF=1).");
	instruction[1078].binPrefixLen = 0;
	instruction[1078].binOpcodeLen = 2;
	instruction[1078].binOpcode = bpstrndup("\x0F\x92", 2);
	instruction[1078].opcodeOperandCount = 0;

	// Instruction Struct [1079] - SETNB
	instruction[1079].instructionStr = strdup("SETNB");
	instruction[1079].undecodedStr = strdup("0F 93");
	instruction[1079].dstOperandStr = strdup("r/m8");
	instruction[1079].dstOperand = IA_RM8;
	instruction[1079].valid64 = INVALID;
	instruction[1079].compatLeg = VALID;
	instruction[1079].VMX = 0;
	instruction[1079].description = strdup("Set byte if not below (CF=0).");
	instruction[1079].binPrefixLen = 0;
	instruction[1079].binOpcodeLen = 2;
	instruction[1079].binOpcode = bpstrndup("\x0F\x93", 2);
	instruction[1079].opcodeOperandCount = 0;

	// Instruction Struct [1080] - SETNBE
	instruction[1080].instructionStr = strdup("SETNBE");
	instruction[1080].undecodedStr = strdup("0F 97");
	instruction[1080].dstOperandStr = strdup("r/m8");
	instruction[1080].dstOperand = IA_RM8;
	instruction[1080].valid64 = INVALID;
	instruction[1080].compatLeg = VALID;
	instruction[1080].VMX = 0;
	instruction[1080].description = strdup("Set byte if not below or equal (CF=0 and ZF=0).");
	instruction[1080].binPrefixLen = 0;
	instruction[1080].binOpcodeLen = 2;
	instruction[1080].binOpcode = bpstrndup("\x0F\x97", 2);
	instruction[1080].opcodeOperandCount = 0;

	// Instruction Struct [1081] - SETNC
	instruction[1081].instructionStr = strdup("SETNC");
	instruction[1081].undecodedStr = strdup("0F 93");
	instruction[1081].dstOperandStr = strdup("r/m8");
	instruction[1081].dstOperand = IA_RM8;
	instruction[1081].valid64 = INVALID;
	instruction[1081].compatLeg = VALID;
	instruction[1081].VMX = 0;
	instruction[1081].description = strdup("Set byte if not carry (CF=0).");
	instruction[1081].binPrefixLen = 0;
	instruction[1081].binOpcodeLen = 2;
	instruction[1081].binOpcode = bpstrndup("\x0F\x93", 2);
	instruction[1081].opcodeOperandCount = 0;

	// Instruction Struct [1082] - SETNE
	instruction[1082].instructionStr = strdup("SETNE");
	instruction[1082].undecodedStr = strdup("0F 95");
	instruction[1082].dstOperandStr = strdup("r/m8");
	instruction[1082].dstOperand = IA_RM8;
	instruction[1082].valid64 = INVALID;
	instruction[1082].compatLeg = VALID;
	instruction[1082].VMX = 0;
	instruction[1082].description = strdup("Set byte if not equal (ZF=0).");
	instruction[1082].binPrefixLen = 0;
	instruction[1082].binOpcodeLen = 2;
	instruction[1082].binOpcode = bpstrndup("\x0F\x95", 2);
	instruction[1082].opcodeOperandCount = 0;

	// Instruction Struct [1083] - SETNG
	instruction[1083].instructionStr = strdup("SETNG");
	instruction[1083].undecodedStr = strdup("0F 9E");
	instruction[1083].dstOperandStr = strdup("r/m8");
	instruction[1083].dstOperand = IA_RM8;
	instruction[1083].valid64 = INVALID;
	instruction[1083].compatLeg = VALID;
	instruction[1083].VMX = 0;
	instruction[1083].description = strdup("Set byte if not greater (ZF=1 or SF? OF)");
	instruction[1083].binPrefixLen = 0;
	instruction[1083].binOpcodeLen = 2;
	instruction[1083].binOpcode = bpstrndup("\x0F\x9E", 2);
	instruction[1083].opcodeOperandCount = 0;

	// Instruction Struct [1084] - SETNGE
	instruction[1084].instructionStr = strdup("SETNGE");
	instruction[1084].undecodedStr = strdup("0F 9C");
	instruction[1084].dstOperandStr = strdup("r/m8");
	instruction[1084].dstOperand = IA_RM8;
	instruction[1084].valid64 = INVALID;
	instruction[1084].compatLeg = VALID;
	instruction[1084].VMX = 0;
	instruction[1084].description = strdup("Set byte if not greater or equal (SF? OF).");
	instruction[1084].binPrefixLen = 0;
	instruction[1084].binOpcodeLen = 2;
	instruction[1084].binOpcode = bpstrndup("\x0F\x9C", 2);
	instruction[1084].opcodeOperandCount = 0;

	// Instruction Struct [1085] - SETNL
	instruction[1085].instructionStr = strdup("SETNL");
	instruction[1085].undecodedStr = strdup("0F 9D");
	instruction[1085].dstOperandStr = strdup("r/m8");
	instruction[1085].dstOperand = IA_RM8;
	instruction[1085].valid64 = INVALID;
	instruction[1085].compatLeg = VALID;
	instruction[1085].VMX = 0;
	instruction[1085].description = strdup("Set byte if not less (SF=OF).");
	instruction[1085].binPrefixLen = 0;
	instruction[1085].binOpcodeLen = 2;
	instruction[1085].binOpcode = bpstrndup("\x0F\x9D", 2);
	instruction[1085].opcodeOperandCount = 0;

	// Instruction Struct [1086] - SETNLE
	instruction[1086].instructionStr = strdup("SETNLE");
	instruction[1086].undecodedStr = strdup("0F 9F");
	instruction[1086].dstOperandStr = strdup("r/m8");
	instruction[1086].dstOperand = IA_RM8;
	instruction[1086].valid64 = INVALID;
	instruction[1086].compatLeg = VALID;
	instruction[1086].VMX = 0;
	instruction[1086].description = strdup("Set byte if not less or equal (ZF=0 and SF=OF).");
	instruction[1086].binPrefixLen = 0;
	instruction[1086].binOpcodeLen = 2;
	instruction[1086].binOpcode = bpstrndup("\x0F\x9F", 2);
	instruction[1086].opcodeOperandCount = 0;

	// Instruction Struct [1087] - SETNO
	instruction[1087].instructionStr = strdup("SETNO");
	instruction[1087].undecodedStr = strdup("0F 91");
	instruction[1087].dstOperandStr = strdup("r/m8");
	instruction[1087].dstOperand = IA_RM8;
	instruction[1087].valid64 = INVALID;
	instruction[1087].compatLeg = VALID;
	instruction[1087].VMX = 0;
	instruction[1087].description = strdup("Set byte if not overflow (OF=0).");
	instruction[1087].binPrefixLen = 0;
	instruction[1087].binOpcodeLen = 2;
	instruction[1087].binOpcode = bpstrndup("\x0F\x91", 2);
	instruction[1087].opcodeOperandCount = 0;

	// Instruction Struct [1088] - SETNP
	instruction[1088].instructionStr = strdup("SETNP");
	instruction[1088].undecodedStr = strdup("0F 9B");
	instruction[1088].dstOperandStr = strdup("r/m8");
	instruction[1088].dstOperand = IA_RM8;
	instruction[1088].valid64 = INVALID;
	instruction[1088].compatLeg = VALID;
	instruction[1088].VMX = 0;
	instruction[1088].description = strdup("Set byte if not parity (PF=0).");
	instruction[1088].binPrefixLen = 0;
	instruction[1088].binOpcodeLen = 2;
	instruction[1088].binOpcode = bpstrndup("\x0F\x9B", 2);
	instruction[1088].opcodeOperandCount = 0;

	// Instruction Struct [1089] - SETNS
	instruction[1089].instructionStr = strdup("SETNS");
	instruction[1089].undecodedStr = strdup("0F 99");
	instruction[1089].dstOperandStr = strdup("r/m8");
	instruction[1089].dstOperand = IA_RM8;
	instruction[1089].valid64 = INVALID;
	instruction[1089].compatLeg = VALID;
	instruction[1089].VMX = 0;
	instruction[1089].description = strdup("Set byte if not sign (SF=0).");
	instruction[1089].binPrefixLen = 0;
	instruction[1089].binOpcodeLen = 2;
	instruction[1089].binOpcode = bpstrndup("\x0F\x99", 2);
	instruction[1089].opcodeOperandCount = 0;

	// Instruction Struct [1090] - SETNZ
	instruction[1090].instructionStr = strdup("SETNZ");
	instruction[1090].undecodedStr = strdup("0F 95");
	instruction[1090].dstOperandStr = strdup("r/m8");
	instruction[1090].dstOperand = IA_RM8;
	instruction[1090].valid64 = INVALID;
	instruction[1090].compatLeg = VALID;
	instruction[1090].VMX = 0;
	instruction[1090].description = strdup("Set byte if not zero (ZF=0).");
	instruction[1090].binPrefixLen = 0;
	instruction[1090].binOpcodeLen = 2;
	instruction[1090].binOpcode = bpstrndup("\x0F\x95", 2);
	instruction[1090].opcodeOperandCount = 0;

	// Instruction Struct [1091] - SETO
	instruction[1091].instructionStr = strdup("SETO");
	instruction[1091].undecodedStr = strdup("0F 90");
	instruction[1091].dstOperandStr = strdup("r/m8");
	instruction[1091].dstOperand = IA_RM8;
	instruction[1091].valid64 = INVALID;
	instruction[1091].compatLeg = VALID;
	instruction[1091].VMX = 0;
	instruction[1091].description = strdup("Set byte if overflow (OF=1)");
	instruction[1091].binPrefixLen = 0;
	instruction[1091].binOpcodeLen = 2;
	instruction[1091].binOpcode = bpstrndup("\x0F\x90", 2);
	instruction[1091].opcodeOperandCount = 0;

	// Instruction Struct [1092] - SETP
	instruction[1092].instructionStr = strdup("SETP");
	instruction[1092].undecodedStr = strdup("0F 9A");
	instruction[1092].dstOperandStr = strdup("r/m8");
	instruction[1092].dstOperand = IA_RM8;
	instruction[1092].valid64 = INVALID;
	instruction[1092].compatLeg = VALID;
	instruction[1092].VMX = 0;
	instruction[1092].description = strdup("Set byte if parity (PF=1).");
	instruction[1092].binPrefixLen = 0;
	instruction[1092].binOpcodeLen = 2;
	instruction[1092].binOpcode = bpstrndup("\x0F\x9A", 2);
	instruction[1092].opcodeOperandCount = 0;

	// Instruction Struct [1093] - SETPE
	instruction[1093].instructionStr = strdup("SETPE");
	instruction[1093].undecodedStr = strdup("0F 9A");
	instruction[1093].dstOperandStr = strdup("r/m8");
	instruction[1093].dstOperand = IA_RM8;
	instruction[1093].valid64 = INVALID;
	instruction[1093].compatLeg = VALID;
	instruction[1093].VMX = 0;
	instruction[1093].description = strdup("Set byte if parity even (PF=1).");
	instruction[1093].binPrefixLen = 0;
	instruction[1093].binOpcodeLen = 2;
	instruction[1093].binOpcode = bpstrndup("\x0F\x9A", 2);
	instruction[1093].opcodeOperandCount = 0;

	// Instruction Struct [1094] - SETPO
	instruction[1094].instructionStr = strdup("SETPO");
	instruction[1094].undecodedStr = strdup("0F 9B");
	instruction[1094].dstOperandStr = strdup("r/m8");
	instruction[1094].dstOperand = IA_RM8;
	instruction[1094].valid64 = INVALID;
	instruction[1094].compatLeg = VALID;
	instruction[1094].VMX = 0;
	instruction[1094].description = strdup("Set byte if parity odd (PF=0).");
	instruction[1094].binPrefixLen = 0;
	instruction[1094].binOpcodeLen = 2;
	instruction[1094].binOpcode = bpstrndup("\x0F\x9B", 2);
	instruction[1094].opcodeOperandCount = 0;

	// Instruction Struct [1095] - SETS
	instruction[1095].instructionStr = strdup("SETS");
	instruction[1095].undecodedStr = strdup("0F 98");
	instruction[1095].dstOperandStr = strdup("r/m8");
	instruction[1095].dstOperand = IA_RM8;
	instruction[1095].valid64 = INVALID;
	instruction[1095].compatLeg = VALID;
	instruction[1095].VMX = 0;
	instruction[1095].description = strdup("Set byte if sign (SF=1).");
	instruction[1095].binPrefixLen = 0;
	instruction[1095].binOpcodeLen = 2;
	instruction[1095].binOpcode = bpstrndup("\x0F\x98", 2);
	instruction[1095].opcodeOperandCount = 0;

	// Instruction Struct [1096] - SETZ
	instruction[1096].instructionStr = strdup("SETZ");
	instruction[1096].undecodedStr = strdup("0F 94");
	instruction[1096].dstOperandStr = strdup("r/m8");
	instruction[1096].dstOperand = IA_RM8;
	instruction[1096].valid64 = INVALID;
	instruction[1096].compatLeg = VALID;
	instruction[1096].VMX = 0;
	instruction[1096].description = strdup("Set byte if zero (ZF=1).");
	instruction[1096].binPrefixLen = 0;
	instruction[1096].binOpcodeLen = 2;
	instruction[1096].binOpcode = bpstrndup("\x0F\x94", 2);
	instruction[1096].opcodeOperandCount = 0;

	// Instruction Struct [1097] - SFENCE
	instruction[1097].instructionStr = strdup("SFENCE");
	instruction[1097].undecodedStr = strdup("0F AE /7");
	instruction[1097].valid64 = INVALID;
	instruction[1097].compatLeg = VALID;
	instruction[1097].VMX = 0;
	instruction[1097].description = strdup("Serializes store operations.");
	instruction[1097].binPrefixLen = 0;
	instruction[1097].binOpcodeLen = 2;
	instruction[1097].binOpcode = bpstrndup("\x0F\xAE", 2);
	instruction[1097].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[1097].opcodeOperandCount = 1;

	// Instruction Struct [1098] - SGDT
	instruction[1098].instructionStr = strdup("SGDT");
	instruction[1098].undecodedStr = strdup("0F 01 /0");
	instruction[1098].dstOperandStr = strdup("m");
	instruction[1098].dstOperand = IA_M;
	instruction[1098].valid64 = INVALID;
	instruction[1098].compatLeg = VALID;
	instruction[1098].VMX = 0;
	instruction[1098].description = strdup("Store GDTR to m.");
	instruction[1098].binPrefixLen = 0;
	instruction[1098].binOpcodeLen = 2;
	instruction[1098].binOpcode = bpstrndup("\x0F\x01", 2);
	instruction[1098].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[1098].opcodeOperandCount = 1;

	// Instruction Struct [1099] - SHLD
	instruction[1099].instructionStr = strdup("SHLD");
	instruction[1099].undecodedStr = strdup("0F A4");
	instruction[1099].dstOperandStr = strdup("r/m16");
	instruction[1099].dstOperand = IA_RM16;
	instruction[1099].srcOperandStr = strdup("r16");
	instruction[1099].srcOperand = IA_R16;
	instruction[1099].thirdOperandStr = strdup("imm8");
	instruction[1099].thirdOperand = IA_IMM8;
	instruction[1099].valid64 = INVALID;
	instruction[1099].compatLeg = VALID;
	instruction[1099].VMX = 0;
	instruction[1099].description = strdup("Shift r/m16 to left imm8 places while shifting bits from r16 in from the right.");
	instruction[1099].binPrefixLen = 0;
	instruction[1099].binOpcodeLen = 2;
	instruction[1099].binOpcode = bpstrndup("\x0F\xA4", 2);
	instruction[1099].opcodeOperandCount = 0;

	// Instruction Struct [1100] - SHLD
	instruction[1100].instructionStr = strdup("SHLD");
	instruction[1100].undecodedStr = strdup("0F A5");
	instruction[1100].dstOperandStr = strdup("r/m16");
	instruction[1100].dstOperand = IA_RM16;
	instruction[1100].srcOperandStr = strdup("r16");
	instruction[1100].srcOperand = IA_R16;
	instruction[1100].thirdOperandStr = strdup("CL");
	instruction[1100].thirdOperand = IA_CL;
	instruction[1100].valid64 = INVALID;
	instruction[1100].compatLeg = VALID;
	instruction[1100].VMX = 0;
	instruction[1100].description = strdup("Shift r/m16 to left CL places while shifting bits from r16 in from the right.");
	instruction[1100].binPrefixLen = 0;
	instruction[1100].binOpcodeLen = 2;
	instruction[1100].binOpcode = bpstrndup("\x0F\xA5", 2);
	instruction[1100].opcodeOperandCount = 0;

	// Instruction Struct [1101] - SHLD
	instruction[1101].instructionStr = strdup("SHLD");
	instruction[1101].undecodedStr = strdup("0F A4");
	instruction[1101].dstOperandStr = strdup("r/m32");
	instruction[1101].dstOperand = IA_RM32;
	instruction[1101].srcOperandStr = strdup("r32");
	instruction[1101].srcOperand = IA_R32;
	instruction[1101].thirdOperandStr = strdup("imm8");
	instruction[1101].thirdOperand = IA_IMM8;
	instruction[1101].valid64 = INVALID;
	instruction[1101].compatLeg = VALID;
	instruction[1101].VMX = 0;
	instruction[1101].description = strdup("Shift r/m32 to left imm8 places while shifting bits from r32 in from the right.");
	instruction[1101].binPrefixLen = 0;
	instruction[1101].binOpcodeLen = 2;
	instruction[1101].binOpcode = bpstrndup("\x0F\xA4", 2);
	instruction[1101].opcodeOperandCount = 0;

	// Instruction Struct [1102] - SHLD
	instruction[1102].instructionStr = strdup("SHLD");
	instruction[1102].undecodedStr = strdup("0F A5");
	instruction[1102].dstOperandStr = strdup("r/m32");
	instruction[1102].dstOperand = IA_RM32;
	instruction[1102].srcOperandStr = strdup("r32");
	instruction[1102].srcOperand = IA_R32;
	instruction[1102].thirdOperandStr = strdup("CL");
	instruction[1102].thirdOperand = IA_CL;
	instruction[1102].valid64 = INVALID;
	instruction[1102].compatLeg = VALID;
	instruction[1102].VMX = 0;
	instruction[1102].description = strdup("Shift r/m32 to left CL places while shifting bits from r32 in from the right.");
	instruction[1102].binPrefixLen = 0;
	instruction[1102].binOpcodeLen = 2;
	instruction[1102].binOpcode = bpstrndup("\x0F\xA5", 2);
	instruction[1102].opcodeOperandCount = 0;

	// Instruction Struct [1103] - SHRD
	instruction[1103].instructionStr = strdup("SHRD");
	instruction[1103].undecodedStr = strdup("0F AC");
	instruction[1103].dstOperandStr = strdup("r/m16");
	instruction[1103].dstOperand = IA_RM16;
	instruction[1103].srcOperandStr = strdup("r16");
	instruction[1103].srcOperand = IA_R16;
	instruction[1103].thirdOperandStr = strdup("imm8");
	instruction[1103].thirdOperand = IA_IMM8;
	instruction[1103].valid64 = INVALID;
	instruction[1103].compatLeg = VALID;
	instruction[1103].VMX = 0;
	instruction[1103].description = strdup("Shift r/m16 to right imm8 places while shifting bits from r16 in from the left.");
	instruction[1103].binPrefixLen = 0;
	instruction[1103].binOpcodeLen = 2;
	instruction[1103].binOpcode = bpstrndup("\x0F\xAC", 2);
	instruction[1103].opcodeOperandCount = 0;

	// Instruction Struct [1104] - SHRD
	instruction[1104].instructionStr = strdup("SHRD");
	instruction[1104].undecodedStr = strdup("0F AD");
	instruction[1104].dstOperandStr = strdup("r/m16");
	instruction[1104].dstOperand = IA_RM16;
	instruction[1104].srcOperandStr = strdup("r16");
	instruction[1104].srcOperand = IA_R16;
	instruction[1104].thirdOperandStr = strdup("CL");
	instruction[1104].thirdOperand = IA_CL;
	instruction[1104].valid64 = INVALID;
	instruction[1104].compatLeg = VALID;
	instruction[1104].VMX = 0;
	instruction[1104].description = strdup("Shift r/m16 to right CL places while shifting bits from r16 in from the left.");
	instruction[1104].binPrefixLen = 0;
	instruction[1104].binOpcodeLen = 2;
	instruction[1104].binOpcode = bpstrndup("\x0F\xAD", 2);
	instruction[1104].opcodeOperandCount = 0;

	// Instruction Struct [1105] - SHRD
	instruction[1105].instructionStr = strdup("SHRD");
	instruction[1105].undecodedStr = strdup("0F AC");
	instruction[1105].dstOperandStr = strdup("r/m32");
	instruction[1105].dstOperand = IA_RM32;
	instruction[1105].srcOperandStr = strdup("r32");
	instruction[1105].srcOperand = IA_R32;
	instruction[1105].thirdOperandStr = strdup("imm8");
	instruction[1105].thirdOperand = IA_IMM8;
	instruction[1105].valid64 = INVALID;
	instruction[1105].compatLeg = VALID;
	instruction[1105].VMX = 0;
	instruction[1105].description = strdup("Shift r/m32 to right imm8 places while shifting bits from r32 in from the left.");
	instruction[1105].binPrefixLen = 0;
	instruction[1105].binOpcodeLen = 2;
	instruction[1105].binOpcode = bpstrndup("\x0F\xAC", 2);
	instruction[1105].opcodeOperandCount = 0;

	// Instruction Struct [1106] - SHRD
	instruction[1106].instructionStr = strdup("SHRD");
	instruction[1106].undecodedStr = strdup("0F AD");
	instruction[1106].dstOperandStr = strdup("r/m32");
	instruction[1106].dstOperand = IA_RM32;
	instruction[1106].srcOperandStr = strdup("r32");
	instruction[1106].srcOperand = IA_R32;
	instruction[1106].thirdOperandStr = strdup("CL");
	instruction[1106].thirdOperand = IA_CL;
	instruction[1106].valid64 = INVALID;
	instruction[1106].compatLeg = VALID;
	instruction[1106].VMX = 0;
	instruction[1106].description = strdup("Shift r/m32 to right CL places while shifting bits from r32 in from the left.");
	instruction[1106].binPrefixLen = 0;
	instruction[1106].binOpcodeLen = 2;
	instruction[1106].binOpcode = bpstrndup("\x0F\xAD", 2);
	instruction[1106].opcodeOperandCount = 0;

	// Instruction Struct [1107] - SHUFPD
	instruction[1107].instructionStr = strdup("SHUFPD");
	instruction[1107].undecodedStr = strdup("66 0F C6 /r ib");
	instruction[1107].dstOperandStr = strdup("xmm1");
	instruction[1107].dstOperand = IA_XMM;
	instruction[1107].srcOperandStr = strdup("xmm2/m128");
	instruction[1107].srcOperand = IA_XMM_M128;
	instruction[1107].thirdOperandStr = strdup("imm8");
	instruction[1107].thirdOperand = IA_IMM8;
	instruction[1107].valid64 = INVALID;
	instruction[1107].compatLeg = VALID;
	instruction[1107].VMX = 0;
	instruction[1107].description = strdup("Shuffle packed double-precision floating-point values selected by imm8 from xmm1 and xmm2/m128 to xmm1.");
	instruction[1107].prefixStr = strdup("OPSIZEOVR");
	instruction[1107].binPrefixLen = 1;
	instruction[1107].binPrefix = bpstrndup("\x66", 1);
	instruction[1107].binOpcodeLen = 2;
	instruction[1107].binOpcode = bpstrndup("\x0F\xC6", 2);
	instruction[1107].opcodeOperand[0] = OA_MODRM;
	instruction[1107].opcodeOperand[1] = OA_IB;
	instruction[1107].opcodeOperandCount = 2;

	// Instruction Struct [1108] - SHUFPS
	instruction[1108].instructionStr = strdup("SHUFPS");
	instruction[1108].undecodedStr = strdup("0F C6 /r ib");
	instruction[1108].dstOperandStr = strdup("xmm1");
	instruction[1108].dstOperand = IA_XMM;
	instruction[1108].srcOperandStr = strdup("xmm2/m128");
	instruction[1108].srcOperand = IA_XMM_M128;
	instruction[1108].thirdOperandStr = strdup("imm8");
	instruction[1108].thirdOperand = IA_IMM8;
	instruction[1108].valid64 = INVALID;
	instruction[1108].compatLeg = VALID;
	instruction[1108].VMX = 0;
	instruction[1108].description = strdup("Shuffle packed single-precision floating-point values selected by imm8 from xmm1 and xmm1/m128 to xmm1.");
	instruction[1108].binPrefixLen = 0;
	instruction[1108].binOpcodeLen = 2;
	instruction[1108].binOpcode = bpstrndup("\x0F\xC6", 2);
	instruction[1108].opcodeOperand[0] = OA_MODRM;
	instruction[1108].opcodeOperand[1] = OA_IB;
	instruction[1108].opcodeOperandCount = 2;

	// Instruction Struct [1109] - SIDT
	instruction[1109].instructionStr = strdup("SIDT");
	instruction[1109].undecodedStr = strdup("0F 01 /1");
	instruction[1109].dstOperandStr = strdup("m");
	instruction[1109].dstOperand = IA_M;
	instruction[1109].valid64 = INVALID;
	instruction[1109].compatLeg = VALID;
	instruction[1109].VMX = 0;
	instruction[1109].description = strdup("Store IDTR to m.");
	instruction[1109].binPrefixLen = 0;
	instruction[1109].binOpcodeLen = 2;
	instruction[1109].binOpcode = bpstrndup("\x0F\x01", 2);
	instruction[1109].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[1109].opcodeOperandCount = 1;

	// Instruction Struct [1110] - SLDT
	instruction[1110].instructionStr = strdup("SLDT");
	instruction[1110].undecodedStr = strdup("0F 00 /0");
	instruction[1110].dstOperandStr = strdup("r/m16");
	instruction[1110].dstOperand = IA_RM16;
	instruction[1110].valid64 = INVALID;
	instruction[1110].compatLeg = VALID;
	instruction[1110].VMX = 0;
	instruction[1110].description = strdup("Stores segment selector from LDTR in r/m16.");
	instruction[1110].binPrefixLen = 0;
	instruction[1110].binOpcodeLen = 2;
	instruction[1110].binOpcode = bpstrndup("\x0F\x00", 2);
	instruction[1110].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[1110].opcodeOperandCount = 1;

	// Instruction Struct [1111] - SMSW
	instruction[1111].instructionStr = strdup("SMSW");
	instruction[1111].undecodedStr = strdup("0F 01 /4");
	instruction[1111].dstOperandStr = strdup("r/m16");
	instruction[1111].dstOperand = IA_RM16;
	instruction[1111].valid64 = INVALID;
	instruction[1111].compatLeg = VALID;
	instruction[1111].VMX = 0;
	instruction[1111].description = strdup("Store machine status word to r/m16.");
	instruction[1111].binPrefixLen = 0;
	instruction[1111].binOpcodeLen = 2;
	instruction[1111].binOpcode = bpstrndup("\x0F\x01", 2);
	instruction[1111].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[1111].opcodeOperandCount = 1;

	// Instruction Struct [1112] - SMSW
	instruction[1112].instructionStr = strdup("SMSW");
	instruction[1112].undecodedStr = strdup("0F 01 /4");
	instruction[1112].dstOperandStr = strdup("r32/m16");
	instruction[1112].dstOperand = IA_R32_M16;
	instruction[1112].valid64 = INVALID;
	instruction[1112].compatLeg = VALID;
	instruction[1112].VMX = 0;
	instruction[1112].description = strdup("Store machine status word in low-order 16 bits of r32/m16; high-order 16 bits of r32 are undefined.");
	instruction[1112].binPrefixLen = 0;
	instruction[1112].binOpcodeLen = 2;
	instruction[1112].binOpcode = bpstrndup("\x0F\x01", 2);
	instruction[1112].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[1112].opcodeOperandCount = 1;

	// Instruction Struct [1113] - SQRTPD
	instruction[1113].instructionStr = strdup("SQRTPD");
	instruction[1113].undecodedStr = strdup("66 0F 51 /r");
	instruction[1113].dstOperandStr = strdup("xmm1");
	instruction[1113].dstOperand = IA_XMM;
	instruction[1113].srcOperandStr = strdup("xmm2/m128");
	instruction[1113].srcOperand = IA_XMM_M128;
	instruction[1113].valid64 = INVALID;
	instruction[1113].compatLeg = VALID;
	instruction[1113].VMX = 0;
	instruction[1113].description = strdup("Computes square roots of the packed double-precision floatingpoint values in xmm2/m128 and stores the results in xmm1.");
	instruction[1113].prefixStr = strdup("OPSIZEOVR");
	instruction[1113].binPrefixLen = 1;
	instruction[1113].binPrefix = bpstrndup("\x66", 1);
	instruction[1113].binOpcodeLen = 2;
	instruction[1113].binOpcode = bpstrndup("\x0F\x51", 2);
	instruction[1113].opcodeOperand[0] = OA_MODRM;
	instruction[1113].opcodeOperandCount = 1;

	// Instruction Struct [1114] - SQRTPS
	instruction[1114].instructionStr = strdup("SQRTPS");
	instruction[1114].undecodedStr = strdup("0F 51 /r");
	instruction[1114].dstOperandStr = strdup("xmm1");
	instruction[1114].dstOperand = IA_XMM;
	instruction[1114].srcOperandStr = strdup("xmm2/m128");
	instruction[1114].srcOperand = IA_XMM_M128;
	instruction[1114].valid64 = INVALID;
	instruction[1114].compatLeg = VALID;
	instruction[1114].VMX = 0;
	instruction[1114].description = strdup("Computes square roots of the packed single-precision floating-point values in xmm2/m128 and stores the results in xmm1.");
	instruction[1114].binPrefixLen = 0;
	instruction[1114].binOpcodeLen = 2;
	instruction[1114].binOpcode = bpstrndup("\x0F\x51", 2);
	instruction[1114].opcodeOperand[0] = OA_MODRM;
	instruction[1114].opcodeOperandCount = 1;

	// Instruction Struct [1115] - SQRTSD
	instruction[1115].instructionStr = strdup("SQRTSD");
	instruction[1115].undecodedStr = strdup("F2 0F 51 /r");
	instruction[1115].dstOperandStr = strdup("xmm1");
	instruction[1115].dstOperand = IA_XMM;
	instruction[1115].srcOperandStr = strdup("xmm2/m64");
	instruction[1115].srcOperand = IA_XMM_M64;
	instruction[1115].valid64 = INVALID;
	instruction[1115].compatLeg = VALID;
	instruction[1115].VMX = 0;
	instruction[1115].description = strdup("Computes square root of the low double-precision floatingpoint value in xmm2/m64 and stores the results in xmm1.");
	instruction[1115].prefixStr = strdup("REPNE");
	instruction[1115].binPrefixLen = 1;
	instruction[1115].binPrefix = bpstrndup("\xF2", 1);
	instruction[1115].binOpcodeLen = 2;
	instruction[1115].binOpcode = bpstrndup("\x0F\x51", 2);
	instruction[1115].opcodeOperand[0] = OA_MODRM;
	instruction[1115].opcodeOperandCount = 1;

	// Instruction Struct [1116] - SQRTSS
	instruction[1116].instructionStr = strdup("SQRTSS");
	instruction[1116].undecodedStr = strdup("F3 0F 51 /r");
	instruction[1116].dstOperandStr = strdup("xmm1");
	instruction[1116].dstOperand = IA_XMM;
	instruction[1116].srcOperandStr = strdup("xmm2/m32");
	instruction[1116].srcOperand = IA_XMM_M32;
	instruction[1116].valid64 = INVALID;
	instruction[1116].compatLeg = VALID;
	instruction[1116].VMX = 0;
	instruction[1116].description = strdup("Computes square root of the low");
	instruction[1116].prefixStr = strdup("REP");
	instruction[1116].binPrefixLen = 1;
	instruction[1116].binPrefix = bpstrndup("\xF3", 1);
	instruction[1116].binOpcodeLen = 2;
	instruction[1116].binOpcode = bpstrndup("\x0F\x51", 2);
	instruction[1116].opcodeOperand[0] = OA_MODRM;
	instruction[1116].opcodeOperandCount = 1;

	// Instruction Struct [1117] - CMP
	instruction[1117].instructionStr = strdup("CMP");
	instruction[1117].undecodedStr = strdup("3C ib");
	instruction[1117].dstOperandStr = strdup("AL");
	instruction[1117].dstOperand = IA_AL;
	instruction[1117].srcOperandStr = strdup("imm8");
	instruction[1117].srcOperand = IA_IMM8;
	instruction[1117].valid64 = INVALID;
	instruction[1117].compatLeg = VALID;
	instruction[1117].VMX = 0;
	instruction[1117].description = strdup("Compare imm8 with AL.");
	instruction[1117].binPrefixLen = 0;
	instruction[1117].binOpcodeLen = 1;
	instruction[1117].binOpcode = bpstrndup("\x3C", 1);
	instruction[1117].opcodeOperand[0] = OA_IB;
	instruction[1117].opcodeOperandCount = 1;

	// Instruction Struct [1118] - STC
	instruction[1118].instructionStr = strdup("STC");
	instruction[1118].undecodedStr = strdup("F9");
	instruction[1118].valid64 = INVALID;
	instruction[1118].compatLeg = VALID;
	instruction[1118].VMX = 0;
	instruction[1118].description = strdup("Set CF flag.");
	instruction[1118].binPrefixLen = 0;
	instruction[1118].binOpcodeLen = 1;
	instruction[1118].binOpcode = bpstrndup("\xF9", 1);
	instruction[1118].opcodeOperandCount = 0;

	// Instruction Struct [1119] - STD
	instruction[1119].instructionStr = strdup("STD");
	instruction[1119].undecodedStr = strdup("FD");
	instruction[1119].valid64 = INVALID;
	instruction[1119].compatLeg = VALID;
	instruction[1119].VMX = 0;
	instruction[1119].description = strdup("Set DF flag.");
	instruction[1119].binPrefixLen = 0;
	instruction[1119].binOpcodeLen = 1;
	instruction[1119].binOpcode = bpstrndup("\xFD", 1);
	instruction[1119].opcodeOperandCount = 0;

	// Instruction Struct [1120] - STI
	instruction[1120].instructionStr = strdup("STI");
	instruction[1120].undecodedStr = strdup("FB");
	instruction[1120].valid64 = INVALID;
	instruction[1120].compatLeg = VALID;
	instruction[1120].VMX = 0;
	instruction[1120].description = strdup("Set interrupt flag; external, maskable interrupts enabled at the end of the next instruction.");
	instruction[1120].binPrefixLen = 0;
	instruction[1120].binOpcodeLen = 1;
	instruction[1120].binOpcode = bpstrndup("\xFB", 1);
	instruction[1120].opcodeOperandCount = 0;

	// Instruction Struct [1121] - STMXCSR
	instruction[1121].instructionStr = strdup("STMXCSR");
	instruction[1121].undecodedStr = strdup("0F AE /3");
	instruction[1121].dstOperandStr = strdup("m32");
	instruction[1121].dstOperand = IA_M32;
	instruction[1121].valid64 = INVALID;
	instruction[1121].compatLeg = VALID;
	instruction[1121].VMX = 0;
	instruction[1121].description = strdup("Store contents of MXCSR register to m32.");
	instruction[1121].binPrefixLen = 0;
	instruction[1121].binOpcodeLen = 2;
	instruction[1121].binOpcode = bpstrndup("\x0F\xAE", 2);
	instruction[1121].opcodeOperand[0] = OA_RM_ONLY_3;
	instruction[1121].opcodeOperandCount = 1;

	// Instruction Struct [1122] - STOS
	instruction[1122].instructionStr = strdup("STOS");
	instruction[1122].undecodedStr = strdup("AA");
	instruction[1122].dstOperandStr = strdup("m8");
	instruction[1122].dstOperand = IA_M8;
	instruction[1122].valid64 = INVALID;
	instruction[1122].compatLeg = VALID;
	instruction[1122].VMX = 0;
	instruction[1122].description = strdup("For legacy mode, store AL at address ES:(E)DI; For 64-bit mode store AL at address RDI or EDI.");
	instruction[1122].binPrefixLen = 0;
	instruction[1122].binOpcodeLen = 1;
	instruction[1122].binOpcode = bpstrndup("\xAA", 1);
	instruction[1122].opcodeOperandCount = 0;

	// Instruction Struct [1123] - STOS
	instruction[1123].instructionStr = strdup("STOS");
	instruction[1123].undecodedStr = strdup("AB");
	instruction[1123].dstOperandStr = strdup("m16");
	instruction[1123].dstOperand = IA_M16;
	instruction[1123].valid64 = INVALID;
	instruction[1123].compatLeg = VALID;
	instruction[1123].VMX = 0;
	instruction[1123].description = strdup("For legacy mode, store AX at address ES:(E)DI; For 64-bit mode store AX at address RDI or EDI.");
	instruction[1123].binPrefixLen = 0;
	instruction[1123].binOpcodeLen = 1;
	instruction[1123].binOpcode = bpstrndup("\xAB", 1);
	instruction[1123].opcodeOperandCount = 0;

	// Instruction Struct [1124] - STOS
	instruction[1124].instructionStr = strdup("STOS");
	instruction[1124].undecodedStr = strdup("AB");
	instruction[1124].dstOperandStr = strdup("m32");
	instruction[1124].dstOperand = IA_M32;
	instruction[1124].valid64 = INVALID;
	instruction[1124].compatLeg = VALID;
	instruction[1124].VMX = 0;
	instruction[1124].description = strdup("For legacy mode, store EAX at address ES:(E)DI; For 64-bit mode store EAX at address RDI or EDI.");
	instruction[1124].binPrefixLen = 0;
	instruction[1124].binOpcodeLen = 1;
	instruction[1124].binOpcode = bpstrndup("\xAB", 1);
	instruction[1124].opcodeOperandCount = 0;

	// Instruction Struct [1125] - STOSB
	instruction[1125].instructionStr = strdup("STOSB");
	instruction[1125].undecodedStr = strdup("AA");
	instruction[1125].valid64 = INVALID;
	instruction[1125].compatLeg = VALID;
	instruction[1125].VMX = 0;
	instruction[1125].description = strdup("For legacy mode, store AL at address ES:(E)DI; For 64-bit mode store AL at address RDI or EDI.");
	instruction[1125].binPrefixLen = 0;
	instruction[1125].binOpcodeLen = 1;
	instruction[1125].binOpcode = bpstrndup("\xAA", 1);
	instruction[1125].opcodeOperandCount = 0;

	// Instruction Struct [1126] - STOSW
	instruction[1126].instructionStr = strdup("STOSW");
	instruction[1126].undecodedStr = strdup("AB");
	instruction[1126].valid64 = INVALID;
	instruction[1126].compatLeg = VALID;
	instruction[1126].VMX = 0;
	instruction[1126].description = strdup("For legacy mode, store AX at address ES:(E)DI; For 64-bit mode store AX at address RDI or EDI.");
	instruction[1126].binPrefixLen = 0;
	instruction[1126].binOpcodeLen = 1;
	instruction[1126].binOpcode = bpstrndup("\xAB", 1);
	instruction[1126].opcodeOperandCount = 0;

	// Instruction Struct [1127] - STOSDc
	instruction[1127].instructionStr = strdup("STOSDc");
	instruction[1127].undecodedStr = strdup("AB");
	instruction[1127].srcOperandStr = strdup(" ");
	instruction[1127].valid64 = INVALID;
	instruction[1127].compatLeg = VALID;
	instruction[1127].VMX = 0;
	instruction[1127].description = strdup("For legacy mode, store EAX at address ES:(E)DI; For 64-bit mode store EAX at address RDI or EDI.");
	instruction[1127].binPrefixLen = 0;
	instruction[1127].binOpcodeLen = 1;
	instruction[1127].binOpcode = bpstrndup("\xAB", 1);
	instruction[1127].opcodeOperandCount = 0;

	// Instruction Struct [1128] - STR
	instruction[1128].instructionStr = strdup("STR");
	instruction[1128].undecodedStr = strdup("0F 00 /1");
	instruction[1128].dstOperandStr = strdup("r/m16");
	instruction[1128].dstOperand = IA_RM16;
	instruction[1128].valid64 = INVALID;
	instruction[1128].compatLeg = VALID;
	instruction[1128].VMX = 0;
	instruction[1128].description = strdup("Stores segment selector from TR in r/m16.");
	instruction[1128].binPrefixLen = 0;
	instruction[1128].binOpcodeLen = 2;
	instruction[1128].binOpcode = bpstrndup("\x0F\x00", 2);
	instruction[1128].opcodeOperand[0] = OA_RM_ONLY_1;
	instruction[1128].opcodeOperandCount = 1;

	// Instruction Struct [1129] - SUB
	instruction[1129].instructionStr = strdup("SUB");
	instruction[1129].undecodedStr = strdup("2C ib");
	instruction[1129].dstOperandStr = strdup("AL");
	instruction[1129].dstOperand = IA_AL;
	instruction[1129].srcOperandStr = strdup("imm8");
	instruction[1129].srcOperand = IA_IMM8;
	instruction[1129].valid64 = INVALID;
	instruction[1129].compatLeg = VALID;
	instruction[1129].VMX = 0;
	instruction[1129].description = strdup("Subtract imm8 from AL.");
	instruction[1129].binPrefixLen = 0;
	instruction[1129].binOpcodeLen = 1;
	instruction[1129].binOpcode = bpstrndup("\x2C", 1);
	instruction[1129].opcodeOperand[0] = OA_IB;
	instruction[1129].opcodeOperandCount = 1;

	// Instruction Struct [1130] - SUB
	instruction[1130].instructionStr = strdup("SUB");
	instruction[1130].undecodedStr = strdup("2D iw");
	instruction[1130].dstOperandStr = strdup("AX");
	instruction[1130].dstOperand = IA_AX;
	instruction[1130].srcOperandStr = strdup("imm16");
	instruction[1130].srcOperand = IA_IMM16;
	instruction[1130].valid64 = INVALID;
	instruction[1130].compatLeg = VALID;
	instruction[1130].VMX = 0;
	instruction[1130].description = strdup("Subtract imm16 from AX.");
	instruction[1130].binPrefixLen = 0;
	instruction[1130].binOpcodeLen = 1;
	instruction[1130].binOpcode = bpstrndup("\x2D", 1);
	instruction[1130].opcodeOperand[0] = OA_IW;
	instruction[1130].opcodeOperandCount = 1;

	// Instruction Struct [1131] - SUB
	instruction[1131].instructionStr = strdup("SUB");
	instruction[1131].undecodedStr = strdup("2D id");
	instruction[1131].dstOperandStr = strdup("EAX");
	instruction[1131].dstOperand = IA_EAX;
	instruction[1131].srcOperandStr = strdup("imm32");
	instruction[1131].srcOperand = IA_IMM32;
	instruction[1131].valid64 = INVALID;
	instruction[1131].compatLeg = VALID;
	instruction[1131].VMX = 0;
	instruction[1131].description = strdup("Subtract imm32 from EAX.");
	instruction[1131].binPrefixLen = 0;
	instruction[1131].binOpcodeLen = 1;
	instruction[1131].binOpcode = bpstrndup("\x2D", 1);
	instruction[1131].opcodeOperand[0] = OA_ID;
	instruction[1131].opcodeOperandCount = 1;

	// Instruction Struct [1132] - SUB
	instruction[1132].instructionStr = strdup("SUB");
	instruction[1132].undecodedStr = strdup("80 /5 ib");
	instruction[1132].dstOperandStr = strdup("r/m8");
	instruction[1132].dstOperand = IA_RM8;
	instruction[1132].srcOperandStr = strdup("imm8");
	instruction[1132].srcOperand = IA_IMM8;
	instruction[1132].valid64 = INVALID;
	instruction[1132].compatLeg = VALID;
	instruction[1132].VMX = 0;
	instruction[1132].description = strdup("Subtract imm8 from r/m8.");
	instruction[1132].binPrefixLen = 0;
	instruction[1132].binOpcodeLen = 1;
	instruction[1132].binOpcode = bpstrndup("\x80", 1);
	instruction[1132].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[1132].opcodeOperand[1] = OA_IB;
	instruction[1132].opcodeOperandCount = 2;

	// Instruction Struct [1133] - SUB
	instruction[1133].instructionStr = strdup("SUB");
	instruction[1133].undecodedStr = strdup("81 /5 iw");
	instruction[1133].dstOperandStr = strdup("r/m16");
	instruction[1133].dstOperand = IA_RM16;
	instruction[1133].srcOperandStr = strdup("imm16");
	instruction[1133].srcOperand = IA_IMM16;
	instruction[1133].valid64 = INVALID;
	instruction[1133].compatLeg = VALID;
	instruction[1133].VMX = 0;
	instruction[1133].description = strdup("Subtract imm16 from r/m16.");
	instruction[1133].binPrefixLen = 0;
	instruction[1133].binOpcodeLen = 1;
	instruction[1133].binOpcode = bpstrndup("\x81", 1);
	instruction[1133].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[1133].opcodeOperand[1] = OA_IW;
	instruction[1133].opcodeOperandCount = 2;

	// Instruction Struct [1134] - SUB
	instruction[1134].instructionStr = strdup("SUB");
	instruction[1134].undecodedStr = strdup("81 /5 id");
	instruction[1134].dstOperandStr = strdup("r/m32");
	instruction[1134].dstOperand = IA_RM32;
	instruction[1134].srcOperandStr = strdup("imm32");
	instruction[1134].srcOperand = IA_IMM32;
	instruction[1134].valid64 = INVALID;
	instruction[1134].compatLeg = VALID;
	instruction[1134].VMX = 0;
	instruction[1134].description = strdup("Subtract imm32 from r/m32.");
	instruction[1134].binPrefixLen = 0;
	instruction[1134].binOpcodeLen = 1;
	instruction[1134].binOpcode = bpstrndup("\x81", 1);
	instruction[1134].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[1134].opcodeOperand[1] = OA_ID;
	instruction[1134].opcodeOperandCount = 2;

	// Instruction Struct [1135] - SUB
	instruction[1135].instructionStr = strdup("SUB");
	instruction[1135].undecodedStr = strdup("83 /5 ib");
	instruction[1135].dstOperandStr = strdup("r/m16");
	instruction[1135].dstOperand = IA_RM16;
	instruction[1135].srcOperandStr = strdup("imm8");
	instruction[1135].srcOperand = IA_IMM8;
	instruction[1135].valid64 = INVALID;
	instruction[1135].compatLeg = VALID;
	instruction[1135].VMX = 0;
	instruction[1135].description = strdup("Subtract sign-extended imm8 from r/m16.");
	instruction[1135].binPrefixLen = 0;
	instruction[1135].binOpcodeLen = 1;
	instruction[1135].binOpcode = bpstrndup("\x83", 1);
	instruction[1135].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[1135].opcodeOperand[1] = OA_IB;
	instruction[1135].opcodeOperandCount = 2;

	// Instruction Struct [1136] - SUB
	instruction[1136].instructionStr = strdup("SUB");
	instruction[1136].undecodedStr = strdup("83 /5 ib");
	instruction[1136].dstOperandStr = strdup("r/m32");
	instruction[1136].dstOperand = IA_RM32;
	instruction[1136].srcOperandStr = strdup("imm8");
	instruction[1136].srcOperand = IA_IMM8;
	instruction[1136].valid64 = INVALID;
	instruction[1136].compatLeg = VALID;
	instruction[1136].VMX = 0;
	instruction[1136].description = strdup("Subtract sign-extended imm8 from r/m32.");
	instruction[1136].binPrefixLen = 0;
	instruction[1136].binOpcodeLen = 1;
	instruction[1136].binOpcode = bpstrndup("\x83", 1);
	instruction[1136].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[1136].opcodeOperand[1] = OA_IB;
	instruction[1136].opcodeOperandCount = 2;

	// Instruction Struct [1137] - SUB
	instruction[1137].instructionStr = strdup("SUB");
	instruction[1137].undecodedStr = strdup("28 /r");
	instruction[1137].dstOperandStr = strdup("r/m8");
	instruction[1137].dstOperand = IA_RM8;
	instruction[1137].srcOperandStr = strdup("r8");
	instruction[1137].srcOperand = IA_R8;
	instruction[1137].valid64 = INVALID;
	instruction[1137].compatLeg = VALID;
	instruction[1137].VMX = 0;
	instruction[1137].description = strdup("Subtract r8 from r/m8.");
	instruction[1137].binPrefixLen = 0;
	instruction[1137].binOpcodeLen = 1;
	instruction[1137].binOpcode = bpstrndup("\x28", 1);
	instruction[1137].opcodeOperand[0] = OA_MODRM;
	instruction[1137].opcodeOperandCount = 1;

	// Instruction Struct [1138] - SUB
	instruction[1138].instructionStr = strdup("SUB");
	instruction[1138].undecodedStr = strdup("29 /r");
	instruction[1138].dstOperandStr = strdup("r/m16");
	instruction[1138].dstOperand = IA_RM16;
	instruction[1138].srcOperandStr = strdup("r16");
	instruction[1138].srcOperand = IA_R16;
	instruction[1138].valid64 = INVALID;
	instruction[1138].compatLeg = VALID;
	instruction[1138].VMX = 0;
	instruction[1138].description = strdup("Subtract r16 from r/m16.");
	instruction[1138].binPrefixLen = 0;
	instruction[1138].binOpcodeLen = 1;
	instruction[1138].binOpcode = bpstrndup("\x29", 1);
	instruction[1138].opcodeOperand[0] = OA_MODRM;
	instruction[1138].opcodeOperandCount = 1;

	// Instruction Struct [1139] - SUB
	instruction[1139].instructionStr = strdup("SUB");
	instruction[1139].undecodedStr = strdup("29 /r");
	instruction[1139].dstOperandStr = strdup("r/m32");
	instruction[1139].dstOperand = IA_RM32;
	instruction[1139].srcOperandStr = strdup("r32");
	instruction[1139].srcOperand = IA_R32;
	instruction[1139].valid64 = INVALID;
	instruction[1139].compatLeg = VALID;
	instruction[1139].VMX = 0;
	instruction[1139].description = strdup("Subtract r32 from r/m32.");
	instruction[1139].binPrefixLen = 0;
	instruction[1139].binOpcodeLen = 1;
	instruction[1139].binOpcode = bpstrndup("\x29", 1);
	instruction[1139].opcodeOperand[0] = OA_MODRM;
	instruction[1139].opcodeOperandCount = 1;

	// Instruction Struct [1140] - SUB
	instruction[1140].instructionStr = strdup("SUB");
	instruction[1140].undecodedStr = strdup("2A /r");
	instruction[1140].dstOperandStr = strdup("r8");
	instruction[1140].dstOperand = IA_R8;
	instruction[1140].srcOperandStr = strdup("r/m8");
	instruction[1140].srcOperand = IA_RM8;
	instruction[1140].valid64 = INVALID;
	instruction[1140].compatLeg = VALID;
	instruction[1140].VMX = 0;
	instruction[1140].description = strdup("Subtract r/m8 from r8.");
	instruction[1140].binPrefixLen = 0;
	instruction[1140].binOpcodeLen = 1;
	instruction[1140].binOpcode = bpstrndup("\x2A", 1);
	instruction[1140].opcodeOperand[0] = OA_MODRM;
	instruction[1140].opcodeOperandCount = 1;

	// Instruction Struct [1141] - SUB
	instruction[1141].instructionStr = strdup("SUB");
	instruction[1141].undecodedStr = strdup("2B /r");
	instruction[1141].dstOperandStr = strdup("r16");
	instruction[1141].dstOperand = IA_R16;
	instruction[1141].srcOperandStr = strdup("r/m16");
	instruction[1141].srcOperand = IA_RM16;
	instruction[1141].valid64 = INVALID;
	instruction[1141].compatLeg = VALID;
	instruction[1141].VMX = 0;
	instruction[1141].description = strdup("Subtract r/m16 from r16.");
	instruction[1141].binPrefixLen = 0;
	instruction[1141].binOpcodeLen = 1;
	instruction[1141].binOpcode = bpstrndup("\x2B", 1);
	instruction[1141].opcodeOperand[0] = OA_MODRM;
	instruction[1141].opcodeOperandCount = 1;

	// Instruction Struct [1142] - SUB
	instruction[1142].instructionStr = strdup("SUB");
	instruction[1142].undecodedStr = strdup("2B /r");
	instruction[1142].dstOperandStr = strdup("r32");
	instruction[1142].dstOperand = IA_R32;
	instruction[1142].srcOperandStr = strdup("r/m32");
	instruction[1142].srcOperand = IA_RM32;
	instruction[1142].valid64 = INVALID;
	instruction[1142].compatLeg = VALID;
	instruction[1142].VMX = 0;
	instruction[1142].description = strdup("Subtract r/m32 from r32.");
	instruction[1142].binPrefixLen = 0;
	instruction[1142].binOpcodeLen = 1;
	instruction[1142].binOpcode = bpstrndup("\x2B", 1);
	instruction[1142].opcodeOperand[0] = OA_MODRM;
	instruction[1142].opcodeOperandCount = 1;

	// Instruction Struct [1143] - SUBPD
	instruction[1143].instructionStr = strdup("SUBPD");
	instruction[1143].undecodedStr = strdup("66 0F 5C /r");
	instruction[1143].dstOperandStr = strdup("xmm1");
	instruction[1143].dstOperand = IA_XMM;
	instruction[1143].srcOperandStr = strdup("xmm2/m128");
	instruction[1143].srcOperand = IA_XMM_M128;
	instruction[1143].valid64 = INVALID;
	instruction[1143].compatLeg = VALID;
	instruction[1143].VMX = 0;
	instruction[1143].description = strdup("Subtract packed double-precision floating-point values in xmm2/m128 from xmm1.");
	instruction[1143].prefixStr = strdup("OPSIZEOVR");
	instruction[1143].binPrefixLen = 1;
	instruction[1143].binPrefix = bpstrndup("\x66", 1);
	instruction[1143].binOpcodeLen = 2;
	instruction[1143].binOpcode = bpstrndup("\x0F\x5C", 2);
	instruction[1143].opcodeOperand[0] = OA_MODRM;
	instruction[1143].opcodeOperandCount = 1;

	// Instruction Struct [1144] - SUBPS
	instruction[1144].instructionStr = strdup("SUBPS");
	instruction[1144].undecodedStr = strdup("0F 5C /r");
	instruction[1144].dstOperandStr = strdup("xmm1");
	instruction[1144].dstOperand = IA_XMM;
	instruction[1144].srcOperandStr = strdup("xmm2/m128");
	instruction[1144].srcOperand = IA_XMM_M128;
	instruction[1144].valid64 = INVALID;
	instruction[1144].compatLeg = VALID;
	instruction[1144].VMX = 0;
	instruction[1144].description = strdup("Subtract packed single-precision floating-point values in xmm2/mem from xmm1.");
	instruction[1144].binPrefixLen = 0;
	instruction[1144].binOpcodeLen = 2;
	instruction[1144].binOpcode = bpstrndup("\x0F\x5C", 2);
	instruction[1144].opcodeOperand[0] = OA_MODRM;
	instruction[1144].opcodeOperandCount = 1;

	// Instruction Struct [1145] - SUBSD
	instruction[1145].instructionStr = strdup("SUBSD");
	instruction[1145].undecodedStr = strdup("F2 0F 5C /r");
	instruction[1145].dstOperandStr = strdup("xmm1");
	instruction[1145].dstOperand = IA_XMM;
	instruction[1145].srcOperandStr = strdup("xmm2/m64");
	instruction[1145].srcOperand = IA_XMM_M64;
	instruction[1145].valid64 = INVALID;
	instruction[1145].compatLeg = VALID;
	instruction[1145].VMX = 0;
	instruction[1145].description = strdup("Subtracts the low doubleprecision floating-point values in xmm2/mem64 from xmm1.");
	instruction[1145].prefixStr = strdup("REPNE");
	instruction[1145].binPrefixLen = 1;
	instruction[1145].binPrefix = bpstrndup("\xF2", 1);
	instruction[1145].binOpcodeLen = 2;
	instruction[1145].binOpcode = bpstrndup("\x0F\x5C", 2);
	instruction[1145].opcodeOperand[0] = OA_MODRM;
	instruction[1145].opcodeOperandCount = 1;

	// Instruction Struct [1146] - SUBSS
	instruction[1146].instructionStr = strdup("SUBSS");
	instruction[1146].undecodedStr = strdup("F3 0F 5C /r");
	instruction[1146].dstOperandStr = strdup("xmm1");
	instruction[1146].dstOperand = IA_XMM;
	instruction[1146].srcOperandStr = strdup("xmm2/m32");
	instruction[1146].srcOperand = IA_XMM_M32;
	instruction[1146].valid64 = INVALID;
	instruction[1146].compatLeg = VALID;
	instruction[1146].VMX = 0;
	instruction[1146].description = strdup("Subtract the lower single-precision floating-point values in xmm2/m32 from xmm1.");
	instruction[1146].prefixStr = strdup("REP");
	instruction[1146].binPrefixLen = 1;
	instruction[1146].binPrefix = bpstrndup("\xF3", 1);
	instruction[1146].binOpcodeLen = 2;
	instruction[1146].binOpcode = bpstrndup("\x0F\x5C", 2);
	instruction[1146].opcodeOperand[0] = OA_MODRM;
	instruction[1146].opcodeOperandCount = 1;

	// Instruction Struct [1147] - SWAPGS
	instruction[1147].instructionStr = strdup("SWAPGS");
	instruction[1147].undecodedStr = strdup("0F 01 /7");
	instruction[1147].valid64 = INVALID;
	instruction[1147].compatLeg = INVALID;
	instruction[1147].VMX = 0;
	instruction[1147].description = strdup("Exchanges the current GS base register value with the value contained in MSR address C0000102H.");
	instruction[1147].binPrefixLen = 0;
	instruction[1147].binOpcodeLen = 2;
	instruction[1147].binOpcode = bpstrndup("\x0F\x01", 2);
	instruction[1147].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[1147].opcodeOperandCount = 1;

	// Instruction Struct [1148] - SYSCALL
	instruction[1148].instructionStr = strdup("SYSCALL");
	instruction[1148].undecodedStr = strdup("0F 05");
	instruction[1148].valid64 = INVALID;
	instruction[1148].compatLeg = INVALID;
	instruction[1148].VMX = 0;
	instruction[1148].description = strdup("Fast call to privilege level 0 system procedures.");
	instruction[1148].binPrefixLen = 0;
	instruction[1148].binOpcodeLen = 2;
	instruction[1148].binOpcode = bpstrndup("\x0F\x05", 2);
	instruction[1148].opcodeOperandCount = 0;

	// Instruction Struct [1149] - SYSENTER
	instruction[1149].instructionStr = strdup("SYSENTER");
	instruction[1149].undecodedStr = strdup("0F 34");
	instruction[1149].valid64 = INVALID;
	instruction[1149].compatLeg = VALID;
	instruction[1149].VMX = 0;
	instruction[1149].description = strdup("Fast call to privilege level 0 system procedures.");
	instruction[1149].binPrefixLen = 0;
	instruction[1149].binOpcodeLen = 2;
	instruction[1149].binOpcode = bpstrndup("\x0F\x34", 2);
	instruction[1149].opcodeOperandCount = 0;

	// Instruction Struct [1150] - SYSEXIT
	instruction[1150].instructionStr = strdup("SYSEXIT");
	instruction[1150].undecodedStr = strdup("0F 35");
	instruction[1150].valid64 = INVALID;
	instruction[1150].compatLeg = VALID;
	instruction[1150].VMX = 0;
	instruction[1150].description = strdup("Fast return to privilege level 3 user code.");
	instruction[1150].binPrefixLen = 0;
	instruction[1150].binOpcodeLen = 2;
	instruction[1150].binOpcode = bpstrndup("\x0F\x35", 2);
	instruction[1150].opcodeOperandCount = 0;

	// Instruction Struct [1151] - SYSRET
	instruction[1151].instructionStr = strdup("SYSRET");
	instruction[1151].undecodedStr = strdup("0F 07");
	instruction[1151].valid64 = INVALID;
	instruction[1151].compatLeg = INVALID;
	instruction[1151].VMX = 0;
	instruction[1151].description = strdup("Return from fast system call");
	instruction[1151].binPrefixLen = 0;
	instruction[1151].binOpcodeLen = 2;
	instruction[1151].binOpcode = bpstrndup("\x0F\x07", 2);
	instruction[1151].opcodeOperandCount = 0;

	// Instruction Struct [1152] - TEST
	instruction[1152].instructionStr = strdup("TEST");
	instruction[1152].undecodedStr = strdup("A8 ib");
	instruction[1152].dstOperandStr = strdup("AL");
	instruction[1152].dstOperand = IA_AL;
	instruction[1152].srcOperandStr = strdup("imm8");
	instruction[1152].srcOperand = IA_IMM8;
	instruction[1152].valid64 = INVALID;
	instruction[1152].compatLeg = VALID;
	instruction[1152].VMX = 0;
	instruction[1152].description = strdup("AND imm8 with AL; set SF, ZF, PF according to result.");
	instruction[1152].binPrefixLen = 0;
	instruction[1152].binOpcodeLen = 1;
	instruction[1152].binOpcode = bpstrndup("\xA8", 1);
	instruction[1152].opcodeOperand[0] = OA_IB;
	instruction[1152].opcodeOperandCount = 1;

	// Instruction Struct [1153] - TEST
	instruction[1153].instructionStr = strdup("TEST");
	instruction[1153].undecodedStr = strdup("A9 iw");
	instruction[1153].dstOperandStr = strdup("AX");
	instruction[1153].dstOperand = IA_AX;
	instruction[1153].srcOperandStr = strdup("imm16");
	instruction[1153].srcOperand = IA_IMM16;
	instruction[1153].valid64 = INVALID;
	instruction[1153].compatLeg = VALID;
	instruction[1153].VMX = 0;
	instruction[1153].description = strdup("AND imm16 with AX; set SF, ZF, PF according to result.");
	instruction[1153].binPrefixLen = 0;
	instruction[1153].binOpcodeLen = 1;
	instruction[1153].binOpcode = bpstrndup("\xA9", 1);
	instruction[1153].opcodeOperand[0] = OA_IW;
	instruction[1153].opcodeOperandCount = 1;

	// Instruction Struct [1154] - TEST
	instruction[1154].instructionStr = strdup("TEST");
	instruction[1154].undecodedStr = strdup("A9 id");
	instruction[1154].dstOperandStr = strdup("EAX");
	instruction[1154].dstOperand = IA_EAX;
	instruction[1154].srcOperandStr = strdup("imm32");
	instruction[1154].srcOperand = IA_IMM32;
	instruction[1154].valid64 = INVALID;
	instruction[1154].compatLeg = VALID;
	instruction[1154].VMX = 0;
	instruction[1154].description = strdup("AND imm32 with EAX; set SF, ZF, PF according to result.");
	instruction[1154].binPrefixLen = 0;
	instruction[1154].binOpcodeLen = 1;
	instruction[1154].binOpcode = bpstrndup("\xA9", 1);
	instruction[1154].opcodeOperand[0] = OA_ID;
	instruction[1154].opcodeOperandCount = 1;

	// Instruction Struct [1155] - TEST
	instruction[1155].instructionStr = strdup("TEST");
	instruction[1155].undecodedStr = strdup("F6 /0 ib");
	instruction[1155].dstOperandStr = strdup("r/m8");
	instruction[1155].dstOperand = IA_RM8;
	instruction[1155].srcOperandStr = strdup("imm8");
	instruction[1155].srcOperand = IA_IMM8;
	instruction[1155].valid64 = INVALID;
	instruction[1155].compatLeg = VALID;
	instruction[1155].VMX = 0;
	instruction[1155].description = strdup("AND imm8 with r/m8; set SF, ZF, PF according to result.");
	instruction[1155].binPrefixLen = 0;
	instruction[1155].binOpcodeLen = 1;
	instruction[1155].binOpcode = bpstrndup("\xF6", 1);
	instruction[1155].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[1155].opcodeOperand[1] = OA_IB;
	instruction[1155].opcodeOperandCount = 2;

	// Instruction Struct [1156] - TEST
	instruction[1156].instructionStr = strdup("TEST");
	instruction[1156].undecodedStr = strdup("F7 /0 iw");
	instruction[1156].dstOperandStr = strdup("r/m16");
	instruction[1156].dstOperand = IA_RM16;
	instruction[1156].srcOperandStr = strdup("imm16");
	instruction[1156].srcOperand = IA_IMM16;
	instruction[1156].valid64 = INVALID;
	instruction[1156].compatLeg = VALID;
	instruction[1156].VMX = 0;
	instruction[1156].description = strdup("AND imm16 with r/m16; set SF, ZF, PF according to result.");
	instruction[1156].binPrefixLen = 0;
	instruction[1156].binOpcodeLen = 1;
	instruction[1156].binOpcode = bpstrndup("\xF7", 1);
	instruction[1156].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[1156].opcodeOperand[1] = OA_IW;
	instruction[1156].opcodeOperandCount = 2;

	// Instruction Struct [1157] - TEST
	instruction[1157].instructionStr = strdup("TEST");
	instruction[1157].undecodedStr = strdup("F7 /0 id");
	instruction[1157].dstOperandStr = strdup("r/m32");
	instruction[1157].dstOperand = IA_RM32;
	instruction[1157].srcOperandStr = strdup("imm32");
	instruction[1157].srcOperand = IA_IMM32;
	instruction[1157].valid64 = INVALID;
	instruction[1157].compatLeg = VALID;
	instruction[1157].VMX = 0;
	instruction[1157].description = strdup("AND imm32 with r/m32; set SF, ZF, PF according to result.");
	instruction[1157].binPrefixLen = 0;
	instruction[1157].binOpcodeLen = 1;
	instruction[1157].binOpcode = bpstrndup("\xF7", 1);
	instruction[1157].opcodeOperand[0] = OA_RM_ONLY_0;
	instruction[1157].opcodeOperand[1] = OA_ID;
	instruction[1157].opcodeOperandCount = 2;

	// Instruction Struct [1158] - TEST
	instruction[1158].instructionStr = strdup("TEST");
	instruction[1158].undecodedStr = strdup("84 /r");
	instruction[1158].dstOperandStr = strdup("r/m8");
	instruction[1158].dstOperand = IA_RM8;
	instruction[1158].srcOperandStr = strdup("r8");
	instruction[1158].srcOperand = IA_R8;
	instruction[1158].valid64 = INVALID;
	instruction[1158].compatLeg = VALID;
	instruction[1158].VMX = 0;
	instruction[1158].description = strdup("AND r8 with r/m8; set SF, ZF, PF according to result.");
	instruction[1158].binPrefixLen = 0;
	instruction[1158].binOpcodeLen = 1;
	instruction[1158].binOpcode = bpstrndup("\x84", 1);
	instruction[1158].opcodeOperand[0] = OA_MODRM;
	instruction[1158].opcodeOperandCount = 1;

	// Instruction Struct [1159] - TEST
	instruction[1159].instructionStr = strdup("TEST");
	instruction[1159].undecodedStr = strdup("85 /r");
	instruction[1159].dstOperandStr = strdup("r/m16");
	instruction[1159].dstOperand = IA_RM16;
	instruction[1159].srcOperandStr = strdup("r16");
	instruction[1159].srcOperand = IA_R16;
	instruction[1159].valid64 = INVALID;
	instruction[1159].compatLeg = VALID;
	instruction[1159].VMX = 0;
	instruction[1159].description = strdup("AND r16 with r/m16; set SF, ZF, PF according to result.");
	instruction[1159].binPrefixLen = 0;
	instruction[1159].binOpcodeLen = 1;
	instruction[1159].binOpcode = bpstrndup("\x85", 1);
	instruction[1159].opcodeOperand[0] = OA_MODRM;
	instruction[1159].opcodeOperandCount = 1;

	// Instruction Struct [1160] - TEST
	instruction[1160].instructionStr = strdup("TEST");
	instruction[1160].undecodedStr = strdup("85 /r");
	instruction[1160].dstOperandStr = strdup("r/m32");
	instruction[1160].dstOperand = IA_RM32;
	instruction[1160].srcOperandStr = strdup("r32");
	instruction[1160].srcOperand = IA_R32;
	instruction[1160].valid64 = INVALID;
	instruction[1160].compatLeg = VALID;
	instruction[1160].VMX = 0;
	instruction[1160].description = strdup("AND r32 with r/m32; set SF, ZF, PF according to result.");
	instruction[1160].binPrefixLen = 0;
	instruction[1160].binOpcodeLen = 1;
	instruction[1160].binOpcode = bpstrndup("\x85", 1);
	instruction[1160].opcodeOperand[0] = OA_MODRM;
	instruction[1160].opcodeOperandCount = 1;

	// Instruction Struct [1161] - UCOMISD
	instruction[1161].instructionStr = strdup("UCOMISD");
	instruction[1161].undecodedStr = strdup("66 0F 2E /r");
	instruction[1161].dstOperandStr = strdup("xmm1");
	instruction[1161].dstOperand = IA_XMM;
	instruction[1161].srcOperandStr = strdup("xmm2/m64");
	instruction[1161].srcOperand = IA_XMM_M64;
	instruction[1161].valid64 = INVALID;
	instruction[1161].compatLeg = VALID;
	instruction[1161].VMX = 0;
	instruction[1161].description = strdup("Compares (unordered) the low doubleprecision floating-point values in xmm1 and xmm2/m64 and set the EFLAGS accordingly.");
	instruction[1161].prefixStr = strdup("OPSIZEOVR");
	instruction[1161].binPrefixLen = 1;
	instruction[1161].binPrefix = bpstrndup("\x66", 1);
	instruction[1161].binOpcodeLen = 2;
	instruction[1161].binOpcode = bpstrndup("\x0F\x2E", 2);
	instruction[1161].opcodeOperand[0] = OA_MODRM;
	instruction[1161].opcodeOperandCount = 1;

	// Instruction Struct [1162] - UCOMISS
	instruction[1162].instructionStr = strdup("UCOMISS");
	instruction[1162].undecodedStr = strdup("0F 2E /r");
	instruction[1162].dstOperandStr = strdup("xmm1");
	instruction[1162].dstOperand = IA_XMM;
	instruction[1162].srcOperandStr = strdup("xmm2/m32");
	instruction[1162].srcOperand = IA_XMM_M32;
	instruction[1162].valid64 = INVALID;
	instruction[1162].compatLeg = VALID;
	instruction[1162].VMX = 0;
	instruction[1162].description = strdup("Compare lower single-precision floatingpoint value in xmm1 register with lower single-precision floating-point value in xmm2/mem and set the status flags accordingly.");
	instruction[1162].binPrefixLen = 0;
	instruction[1162].binOpcodeLen = 2;
	instruction[1162].binOpcode = bpstrndup("\x0F\x2E", 2);
	instruction[1162].opcodeOperand[0] = OA_MODRM;
	instruction[1162].opcodeOperandCount = 1;

	// Instruction Struct [1163] - UD2
	instruction[1163].instructionStr = strdup("UD2");
	instruction[1163].undecodedStr = strdup("0F 0B");
	instruction[1163].valid64 = INVALID;
	instruction[1163].compatLeg = VALID;
	instruction[1163].VMX = 0;
	instruction[1163].description = strdup("Raise invalid opcode exception.");
	instruction[1163].binPrefixLen = 0;
	instruction[1163].binOpcodeLen = 2;
	instruction[1163].binOpcode = bpstrndup("\x0F\x0B", 2);
	instruction[1163].opcodeOperandCount = 0;

	// Instruction Struct [1164] - UNPCKHPD
	instruction[1164].instructionStr = strdup("UNPCKHPD");
	instruction[1164].undecodedStr = strdup("66 0F 15 /r");
	instruction[1164].dstOperandStr = strdup("xmm1");
	instruction[1164].dstOperand = IA_XMM;
	instruction[1164].srcOperandStr = strdup("xmm2/m128");
	instruction[1164].srcOperand = IA_XMM_M128;
	instruction[1164].valid64 = INVALID;
	instruction[1164].compatLeg = VALID;
	instruction[1164].VMX = 0;
	instruction[1164].description = strdup("Unpacks and Interleaves doubleprecision floating-point values from high quadwords of xmm1 and xmm2/m128.");
	instruction[1164].prefixStr = strdup("OPSIZEOVR");
	instruction[1164].binPrefixLen = 1;
	instruction[1164].binPrefix = bpstrndup("\x66", 1);
	instruction[1164].binOpcodeLen = 2;
	instruction[1164].binOpcode = bpstrndup("\x0F\x15", 2);
	instruction[1164].opcodeOperand[0] = OA_MODRM;
	instruction[1164].opcodeOperandCount = 1;

	// Instruction Struct [1165] - UNPCKHPS
	instruction[1165].instructionStr = strdup("UNPCKHPS");
	instruction[1165].undecodedStr = strdup("0F 15 /r");
	instruction[1165].dstOperandStr = strdup("xmm1");
	instruction[1165].dstOperand = IA_XMM;
	instruction[1165].srcOperandStr = strdup("xmm2/m128");
	instruction[1165].srcOperand = IA_XMM_M128;
	instruction[1165].valid64 = INVALID;
	instruction[1165].compatLeg = VALID;
	instruction[1165].VMX = 0;
	instruction[1165].description = strdup("Unpacks and Interleaves singleprecision floating-point values from high quadwords of xmm1 and xmm2/mem into xmm1.");
	instruction[1165].binPrefixLen = 0;
	instruction[1165].binOpcodeLen = 2;
	instruction[1165].binOpcode = bpstrndup("\x0F\x15", 2);
	instruction[1165].opcodeOperand[0] = OA_MODRM;
	instruction[1165].opcodeOperandCount = 1;

	// Instruction Struct [1166] - UNPCKLPD
	instruction[1166].instructionStr = strdup("UNPCKLPD");
	instruction[1166].undecodedStr = strdup("66 0F 14 /r");
	instruction[1166].dstOperandStr = strdup("xmm1");
	instruction[1166].dstOperand = IA_XMM;
	instruction[1166].srcOperandStr = strdup("xmm2/m128");
	instruction[1166].srcOperand = IA_XMM_M128;
	instruction[1166].valid64 = INVALID;
	instruction[1166].compatLeg = VALID;
	instruction[1166].VMX = 0;
	instruction[1166].description = strdup("Unpacks and Interleaves doubleprecision floating-point values from low quadwords of xmm1 and xmm2/m128.");
	instruction[1166].prefixStr = strdup("OPSIZEOVR");
	instruction[1166].binPrefixLen = 1;
	instruction[1166].binPrefix = bpstrndup("\x66", 1);
	instruction[1166].binOpcodeLen = 2;
	instruction[1166].binOpcode = bpstrndup("\x0F\x14", 2);
	instruction[1166].opcodeOperand[0] = OA_MODRM;
	instruction[1166].opcodeOperandCount = 1;

	// Instruction Struct [1167] - UNPCKLPS
	instruction[1167].instructionStr = strdup("UNPCKLPS");
	instruction[1167].undecodedStr = strdup("0F 14 /r");
	instruction[1167].dstOperandStr = strdup("xmm1");
	instruction[1167].dstOperand = IA_XMM;
	instruction[1167].srcOperandStr = strdup("xmm2/m128");
	instruction[1167].srcOperand = IA_XMM_M128;
	instruction[1167].valid64 = INVALID;
	instruction[1167].compatLeg = VALID;
	instruction[1167].VMX = 0;
	instruction[1167].description = strdup("Unpacks and Interleaves singleprecision floating-point values from low quadwords of xmm1 and xmm2/mem into xmm1.");
	instruction[1167].binPrefixLen = 0;
	instruction[1167].binOpcodeLen = 2;
	instruction[1167].binOpcode = bpstrndup("\x0F\x14", 2);
	instruction[1167].opcodeOperand[0] = OA_MODRM;
	instruction[1167].opcodeOperandCount = 1;

	// Instruction Struct [1168] - VERR
	instruction[1168].instructionStr = strdup("VERR");
	instruction[1168].undecodedStr = strdup("0F 00 /4");
	instruction[1168].dstOperandStr = strdup("r/m16");
	instruction[1168].dstOperand = IA_RM16;
	instruction[1168].valid64 = INVALID;
	instruction[1168].compatLeg = VALID;
	instruction[1168].VMX = 0;
	instruction[1168].description = strdup("Set ZF=1 if segment specified with r/m16 can be read.");
	instruction[1168].binPrefixLen = 0;
	instruction[1168].binOpcodeLen = 2;
	instruction[1168].binOpcode = bpstrndup("\x0F\x00", 2);
	instruction[1168].opcodeOperand[0] = OA_RM_ONLY_4;
	instruction[1168].opcodeOperandCount = 1;

	// Instruction Struct [1169] - VERW
	instruction[1169].instructionStr = strdup("VERW");
	instruction[1169].undecodedStr = strdup("0F 00 /5");
	instruction[1169].dstOperandStr = strdup("r/m16");
	instruction[1169].dstOperand = IA_RM16;
	instruction[1169].valid64 = INVALID;
	instruction[1169].compatLeg = VALID;
	instruction[1169].VMX = 0;
	instruction[1169].description = strdup("Set ZF=1 if segment specified with r/m16 can be written.");
	instruction[1169].binPrefixLen = 0;
	instruction[1169].binOpcodeLen = 2;
	instruction[1169].binOpcode = bpstrndup("\x0F\x00", 2);
	instruction[1169].opcodeOperand[0] = OA_RM_ONLY_5;
	instruction[1169].opcodeOperandCount = 1;

	// Instruction Struct [1170] - WAIT
	instruction[1170].instructionStr = strdup("WAIT");
	instruction[1170].undecodedStr = strdup("9B");
	instruction[1170].valid64 = INVALID;
	instruction[1170].compatLeg = VALID;
	instruction[1170].VMX = 0;
	instruction[1170].description = strdup("Check pending unmasked floatingpoint exceptions.");
	instruction[1170].binPrefixLen = 0;
	instruction[1170].binOpcodeLen = 1;
	instruction[1170].binOpcode = bpstrndup("\x9B", 1);
	instruction[1170].opcodeOperandCount = 0;

	// Instruction Struct [1171] - FWAIT
	instruction[1171].instructionStr = strdup("FWAIT");
	instruction[1171].undecodedStr = strdup("9B");
	instruction[1171].valid64 = INVALID;
	instruction[1171].compatLeg = VALID;
	instruction[1171].VMX = 0;
	instruction[1171].description = strdup("Check pending unmasked floatingpoint exceptions.");
	instruction[1171].binPrefixLen = 0;
	instruction[1171].binOpcodeLen = 1;
	instruction[1171].binOpcode = bpstrndup("\x9B", 1);
	instruction[1171].opcodeOperandCount = 0;

	// Instruction Struct [1172] - WBINVD
	instruction[1172].instructionStr = strdup("WBINVD");
	instruction[1172].undecodedStr = strdup("0F 09");
	instruction[1172].valid64 = INVALID;
	instruction[1172].compatLeg = VALID;
	instruction[1172].VMX = 0;
	instruction[1172].description = strdup("Write back and flush Internal caches; initiate writing-back and flushing of external caches.");
	instruction[1172].binPrefixLen = 0;
	instruction[1172].binOpcodeLen = 2;
	instruction[1172].binOpcode = bpstrndup("\x0F\x09", 2);
	instruction[1172].opcodeOperandCount = 0;

	// Instruction Struct [1173] - WRMSR
	instruction[1173].instructionStr = strdup("WRMSR");
	instruction[1173].undecodedStr = strdup("0F 30");
	instruction[1173].valid64 = INVALID;
	instruction[1173].compatLeg = VALID;
	instruction[1173].VMX = 0;
	instruction[1173].description = strdup("Write the value in EDX:EAX to MSR specified by ECX.");
	instruction[1173].binPrefixLen = 0;
	instruction[1173].binOpcodeLen = 2;
	instruction[1173].binOpcode = bpstrndup("\x0F\x30", 2);
	instruction[1173].opcodeOperandCount = 0;

	// Instruction Struct [1174] - XADD
	instruction[1174].instructionStr = strdup("XADD");
	instruction[1174].undecodedStr = strdup("0F C0 /r");
	instruction[1174].dstOperandStr = strdup("r/m8");
	instruction[1174].dstOperand = IA_RM8;
	instruction[1174].srcOperandStr = strdup("r8");
	instruction[1174].srcOperand = IA_R8;
	instruction[1174].valid64 = INVALID;
	instruction[1174].compatLeg = VALID;
	instruction[1174].VMX = 0;
	instruction[1174].description = strdup("Exchange r8 and r/m8; load sum into r/m8.");
	instruction[1174].binPrefixLen = 0;
	instruction[1174].binOpcodeLen = 2;
	instruction[1174].binOpcode = bpstrndup("\x0F\xC0", 2);
	instruction[1174].opcodeOperand[0] = OA_MODRM;
	instruction[1174].opcodeOperandCount = 1;

	// Instruction Struct [1175] - XADD
	instruction[1175].instructionStr = strdup("XADD");
	instruction[1175].undecodedStr = strdup("0F C1 /r");
	instruction[1175].dstOperandStr = strdup("r/m16");
	instruction[1175].dstOperand = IA_RM16;
	instruction[1175].srcOperandStr = strdup("r16");
	instruction[1175].srcOperand = IA_R16;
	instruction[1175].valid64 = INVALID;
	instruction[1175].compatLeg = VALID;
	instruction[1175].VMX = 0;
	instruction[1175].description = strdup("Exchange r16 and r/m16; load sum into r/m16.");
	instruction[1175].binPrefixLen = 0;
	instruction[1175].binOpcodeLen = 2;
	instruction[1175].binOpcode = bpstrndup("\x0F\xC1", 2);
	instruction[1175].opcodeOperand[0] = OA_MODRM;
	instruction[1175].opcodeOperandCount = 1;

	// Instruction Struct [1176] - XADD
	instruction[1176].instructionStr = strdup("XADD");
	instruction[1176].undecodedStr = strdup("0F C1 /r");
	instruction[1176].dstOperandStr = strdup("r/m32");
	instruction[1176].dstOperand = IA_RM32;
	instruction[1176].srcOperandStr = strdup("r32");
	instruction[1176].srcOperand = IA_R32;
	instruction[1176].valid64 = INVALID;
	instruction[1176].compatLeg = VALID;
	instruction[1176].VMX = 0;
	instruction[1176].description = strdup("Exchange r32 and r/m32; load sum into r/m32.");
	instruction[1176].binPrefixLen = 0;
	instruction[1176].binOpcodeLen = 2;
	instruction[1176].binOpcode = bpstrndup("\x0F\xC1", 2);
	instruction[1176].opcodeOperand[0] = OA_MODRM;
	instruction[1176].opcodeOperandCount = 1;

	// Instruction Struct [1177] - XCHG
	instruction[1177].instructionStr = strdup("XCHG");
	instruction[1177].undecodedStr = strdup("90 +rw");
	instruction[1177].dstOperandStr = strdup("AX");
	instruction[1177].dstOperand = IA_AX;
	instruction[1177].srcOperandStr = strdup("r16");
	instruction[1177].srcOperand = IA_R16;
	instruction[1177].valid64 = INVALID;
	instruction[1177].compatLeg = VALID;
	instruction[1177].VMX = 0;
	instruction[1177].description = strdup("Exchange r16 with AX.");
	instruction[1177].binPrefixLen = 0;
	instruction[1177].binOpcodeLen = 1;
	instruction[1177].binOpcode = bpstrndup("\x90", 1);
	instruction[1177].opcodeOperand[0] = OA_RW;
	instruction[1177].opcodeOperandCount = 1;

	// Instruction Struct [1178] - XCHG
	instruction[1178].instructionStr = strdup("XCHG");
	instruction[1178].undecodedStr = strdup("90 +rw");
	instruction[1178].dstOperandStr = strdup("r16");
	instruction[1178].dstOperand = IA_R16;
	instruction[1178].srcOperandStr = strdup("AX");
	instruction[1178].srcOperand = IA_AX;
	instruction[1178].valid64 = INVALID;
	instruction[1178].compatLeg = VALID;
	instruction[1178].VMX = 0;
	instruction[1178].description = strdup("Exchange AX with r16.");
	instruction[1178].binPrefixLen = 0;
	instruction[1178].binOpcodeLen = 1;
	instruction[1178].binOpcode = bpstrndup("\x90", 1);
	instruction[1178].opcodeOperand[0] = OA_RW;
	instruction[1178].opcodeOperandCount = 1;

	// Instruction Struct [1179] - XCHG
	instruction[1179].instructionStr = strdup("XCHG");
	instruction[1179].undecodedStr = strdup("90 +rd");
	instruction[1179].dstOperandStr = strdup("EAX");
	instruction[1179].dstOperand = IA_EAX;
	instruction[1179].srcOperandStr = strdup("r32");
	instruction[1179].srcOperand = IA_R32;
	instruction[1179].valid64 = INVALID;
	instruction[1179].compatLeg = VALID;
	instruction[1179].VMX = 0;
	instruction[1179].description = strdup("Exchange r32 with EAX.");
	instruction[1179].binPrefixLen = 0;
	instruction[1179].binOpcodeLen = 1;
	instruction[1179].binOpcode = bpstrndup("\x90", 1);
	instruction[1179].opcodeOperand[0] = OA_RD;
	instruction[1179].opcodeOperandCount = 1;

	// Instruction Struct [1180] - XCHG
	instruction[1180].instructionStr = strdup("XCHG");
	instruction[1180].undecodedStr = strdup("90 +rd");
	instruction[1180].dstOperandStr = strdup("r32");
	instruction[1180].dstOperand = IA_R32;
	instruction[1180].srcOperandStr = strdup("EAX");
	instruction[1180].srcOperand = IA_EAX;
	instruction[1180].valid64 = INVALID;
	instruction[1180].compatLeg = VALID;
	instruction[1180].VMX = 0;
	instruction[1180].description = strdup("Exchange EAX with r32.");
	instruction[1180].binPrefixLen = 0;
	instruction[1180].binOpcodeLen = 1;
	instruction[1180].binOpcode = bpstrndup("\x90", 1);
	instruction[1180].opcodeOperand[0] = OA_RD;
	instruction[1180].opcodeOperandCount = 1;

	// Instruction Struct [1181] - XCHG
	instruction[1181].instructionStr = strdup("XCHG");
	instruction[1181].undecodedStr = strdup("86 /r");
	instruction[1181].dstOperandStr = strdup("r/m8");
	instruction[1181].dstOperand = IA_RM8;
	instruction[1181].srcOperandStr = strdup("r8");
	instruction[1181].srcOperand = IA_R8;
	instruction[1181].valid64 = INVALID;
	instruction[1181].compatLeg = VALID;
	instruction[1181].VMX = 0;
	instruction[1181].description = strdup("Exchange r8 (byte register) with byte from r/m8.");
	instruction[1181].binPrefixLen = 0;
	instruction[1181].binOpcodeLen = 1;
	instruction[1181].binOpcode = bpstrndup("\x86", 1);
	instruction[1181].opcodeOperand[0] = OA_MODRM;
	instruction[1181].opcodeOperandCount = 1;

	// Instruction Struct [1182] - XCHG
	instruction[1182].instructionStr = strdup("XCHG");
	instruction[1182].undecodedStr = strdup("86 /r");
	instruction[1182].dstOperandStr = strdup("r8");
	instruction[1182].dstOperand = IA_R8;
	instruction[1182].srcOperandStr = strdup("r/m8");
	instruction[1182].srcOperand = IA_RM8;
	instruction[1182].valid64 = INVALID;
	instruction[1182].compatLeg = VALID;
	instruction[1182].VMX = 0;
	instruction[1182].description = strdup("Exchange byte from r/m8 with r8 (byte register).");
	instruction[1182].binPrefixLen = 0;
	instruction[1182].binOpcodeLen = 1;
	instruction[1182].binOpcode = bpstrndup("\x86", 1);
	instruction[1182].opcodeOperand[0] = OA_MODRM;
	instruction[1182].opcodeOperandCount = 1;

	// Instruction Struct [1183] - XCHG
	instruction[1183].instructionStr = strdup("XCHG");
	instruction[1183].undecodedStr = strdup("87 /r");
	instruction[1183].dstOperandStr = strdup("r/m16");
	instruction[1183].dstOperand = IA_RM16;
	instruction[1183].srcOperandStr = strdup("r16");
	instruction[1183].srcOperand = IA_R16;
	instruction[1183].valid64 = INVALID;
	instruction[1183].compatLeg = VALID;
	instruction[1183].VMX = 0;
	instruction[1183].description = strdup("Exchange r16 with word from r/m16.");
	instruction[1183].binPrefixLen = 0;
	instruction[1183].binOpcodeLen = 1;
	instruction[1183].binOpcode = bpstrndup("\x87", 1);
	instruction[1183].opcodeOperand[0] = OA_MODRM;
	instruction[1183].opcodeOperandCount = 1;

	// Instruction Struct [1184] - XCHG
	instruction[1184].instructionStr = strdup("XCHG");
	instruction[1184].undecodedStr = strdup("87 /r");
	instruction[1184].dstOperandStr = strdup("r16");
	instruction[1184].dstOperand = IA_R16;
	instruction[1184].srcOperandStr = strdup("r/m16");
	instruction[1184].srcOperand = IA_RM16;
	instruction[1184].valid64 = INVALID;
	instruction[1184].compatLeg = VALID;
	instruction[1184].VMX = 0;
	instruction[1184].description = strdup("Exchange word from r/m16 with r16.");
	instruction[1184].binPrefixLen = 0;
	instruction[1184].binOpcodeLen = 1;
	instruction[1184].binOpcode = bpstrndup("\x87", 1);
	instruction[1184].opcodeOperand[0] = OA_MODRM;
	instruction[1184].opcodeOperandCount = 1;

	// Instruction Struct [1185] - XCHG
	instruction[1185].instructionStr = strdup("XCHG");
	instruction[1185].undecodedStr = strdup("87 /r");
	instruction[1185].dstOperandStr = strdup("r/m32");
	instruction[1185].dstOperand = IA_RM32;
	instruction[1185].srcOperandStr = strdup("r32");
	instruction[1185].srcOperand = IA_R32;
	instruction[1185].valid64 = INVALID;
	instruction[1185].compatLeg = VALID;
	instruction[1185].VMX = 0;
	instruction[1185].description = strdup("Exchange r32 with doubleword from r/m32.");
	instruction[1185].binPrefixLen = 0;
	instruction[1185].binOpcodeLen = 1;
	instruction[1185].binOpcode = bpstrndup("\x87", 1);
	instruction[1185].opcodeOperand[0] = OA_MODRM;
	instruction[1185].opcodeOperandCount = 1;

	// Instruction Struct [1186] - XCHG
	instruction[1186].instructionStr = strdup("XCHG");
	instruction[1186].undecodedStr = strdup("87 /r");
	instruction[1186].dstOperandStr = strdup("r32");
	instruction[1186].dstOperand = IA_R32;
	instruction[1186].srcOperandStr = strdup("r/m32");
	instruction[1186].srcOperand = IA_RM32;
	instruction[1186].valid64 = INVALID;
	instruction[1186].compatLeg = VALID;
	instruction[1186].VMX = 0;
	instruction[1186].description = strdup("Exchange doubleword from");
	instruction[1186].binPrefixLen = 0;
	instruction[1186].binOpcodeLen = 1;
	instruction[1186].binOpcode = bpstrndup("\x87", 1);
	instruction[1186].opcodeOperand[0] = OA_MODRM;
	instruction[1186].opcodeOperandCount = 1;

	// Instruction Struct [1187] - XLAT
	instruction[1187].instructionStr = strdup("XLAT");
	instruction[1187].undecodedStr = strdup("D7");
	instruction[1187].dstOperandStr = strdup("m8");
	instruction[1187].dstOperand = IA_M8;
	instruction[1187].valid64 = INVALID;
	instruction[1187].compatLeg = VALID;
	instruction[1187].VMX = 0;
	instruction[1187].description = strdup("Set AL to memory byte DS:[(E)BX + unsigned AL].");
	instruction[1187].binPrefixLen = 0;
	instruction[1187].binOpcodeLen = 1;
	instruction[1187].binOpcode = bpstrndup("\xD7", 1);
	instruction[1187].opcodeOperandCount = 0;

	// Instruction Struct [1188] - XLATB
	instruction[1188].instructionStr = strdup("XLATB");
	instruction[1188].undecodedStr = strdup("D7");
	instruction[1188].valid64 = INVALID;
	instruction[1188].compatLeg = VALID;
	instruction[1188].VMX = 0;
	instruction[1188].description = strdup("Set AL to memory byte DS:[(E)BX + unsigned AL].");
	instruction[1188].binPrefixLen = 0;
	instruction[1188].binOpcodeLen = 1;
	instruction[1188].binOpcode = bpstrndup("\xD7", 1);
	instruction[1188].opcodeOperandCount = 0;

	// Instruction Struct [1189] - XOR
	instruction[1189].instructionStr = strdup("XOR");
	instruction[1189].undecodedStr = strdup("34 ib");
	instruction[1189].dstOperandStr = strdup("AL");
	instruction[1189].dstOperand = IA_AL;
	instruction[1189].srcOperandStr = strdup("imm8");
	instruction[1189].srcOperand = IA_IMM8;
	instruction[1189].valid64 = INVALID;
	instruction[1189].compatLeg = VALID;
	instruction[1189].VMX = 0;
	instruction[1189].description = strdup("AL XOR imm8.");
	instruction[1189].binPrefixLen = 0;
	instruction[1189].binOpcodeLen = 1;
	instruction[1189].binOpcode = bpstrndup("\x34", 1);
	instruction[1189].opcodeOperand[0] = OA_IB;
	instruction[1189].opcodeOperandCount = 1;

	// Instruction Struct [1190] - XOR
	instruction[1190].instructionStr = strdup("XOR");
	instruction[1190].undecodedStr = strdup("35 iw");
	instruction[1190].dstOperandStr = strdup("AX");
	instruction[1190].dstOperand = IA_AX;
	instruction[1190].srcOperandStr = strdup("imm16");
	instruction[1190].srcOperand = IA_IMM16;
	instruction[1190].valid64 = INVALID;
	instruction[1190].compatLeg = VALID;
	instruction[1190].VMX = 0;
	instruction[1190].description = strdup("AX XOR imm16.");
	instruction[1190].binPrefixLen = 0;
	instruction[1190].binOpcodeLen = 1;
	instruction[1190].binOpcode = bpstrndup("\x35", 1);
	instruction[1190].opcodeOperand[0] = OA_IW;
	instruction[1190].opcodeOperandCount = 1;

	// Instruction Struct [1191] - XOR
	instruction[1191].instructionStr = strdup("XOR");
	instruction[1191].undecodedStr = strdup("35 id");
	instruction[1191].dstOperandStr = strdup("EAX");
	instruction[1191].dstOperand = IA_EAX;
	instruction[1191].srcOperandStr = strdup("imm32");
	instruction[1191].srcOperand = IA_IMM32;
	instruction[1191].valid64 = INVALID;
	instruction[1191].compatLeg = VALID;
	instruction[1191].VMX = 0;
	instruction[1191].description = strdup("EAX XOR imm32.");
	instruction[1191].binPrefixLen = 0;
	instruction[1191].binOpcodeLen = 1;
	instruction[1191].binOpcode = bpstrndup("\x35", 1);
	instruction[1191].opcodeOperand[0] = OA_ID;
	instruction[1191].opcodeOperandCount = 1;

	// Instruction Struct [1192] - XOR
	instruction[1192].instructionStr = strdup("XOR");
	instruction[1192].undecodedStr = strdup("80 /6 ib");
	instruction[1192].dstOperandStr = strdup("r/m8");
	instruction[1192].dstOperand = IA_RM8;
	instruction[1192].srcOperandStr = strdup("imm8");
	instruction[1192].srcOperand = IA_IMM8;
	instruction[1192].valid64 = INVALID;
	instruction[1192].compatLeg = VALID;
	instruction[1192].VMX = 0;
	instruction[1192].description = strdup("r/m8 XOR imm8.");
	instruction[1192].binPrefixLen = 0;
	instruction[1192].binOpcodeLen = 1;
	instruction[1192].binOpcode = bpstrndup("\x80", 1);
	instruction[1192].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[1192].opcodeOperand[1] = OA_IB;
	instruction[1192].opcodeOperandCount = 2;

	// Instruction Struct [1193] - XOR
	instruction[1193].instructionStr = strdup("XOR");
	instruction[1193].undecodedStr = strdup("81 /6 iw");
	instruction[1193].dstOperandStr = strdup("r/m16");
	instruction[1193].dstOperand = IA_RM16;
	instruction[1193].srcOperandStr = strdup("imm16");
	instruction[1193].srcOperand = IA_IMM16;
	instruction[1193].valid64 = INVALID;
	instruction[1193].compatLeg = VALID;
	instruction[1193].VMX = 0;
	instruction[1193].description = strdup("r/m16 XOR imm16.");
	instruction[1193].binPrefixLen = 0;
	instruction[1193].binOpcodeLen = 1;
	instruction[1193].binOpcode = bpstrndup("\x81", 1);
	instruction[1193].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[1193].opcodeOperand[1] = OA_IW;
	instruction[1193].opcodeOperandCount = 2;

	// Instruction Struct [1194] - XOR
	instruction[1194].instructionStr = strdup("XOR");
	instruction[1194].undecodedStr = strdup("81 /6 id");
	instruction[1194].dstOperandStr = strdup("r/m32");
	instruction[1194].dstOperand = IA_RM32;
	instruction[1194].srcOperandStr = strdup("imm32");
	instruction[1194].srcOperand = IA_IMM32;
	instruction[1194].valid64 = INVALID;
	instruction[1194].compatLeg = VALID;
	instruction[1194].VMX = 0;
	instruction[1194].description = strdup("r/m32 XOR imm32.");
	instruction[1194].binPrefixLen = 0;
	instruction[1194].binOpcodeLen = 1;
	instruction[1194].binOpcode = bpstrndup("\x81", 1);
	instruction[1194].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[1194].opcodeOperand[1] = OA_ID;
	instruction[1194].opcodeOperandCount = 2;

	// Instruction Struct [1195] - XOR
	instruction[1195].instructionStr = strdup("XOR");
	instruction[1195].undecodedStr = strdup("83 /6 ib");
	instruction[1195].dstOperandStr = strdup("r/m16");
	instruction[1195].dstOperand = IA_RM16;
	instruction[1195].srcOperandStr = strdup("imm8");
	instruction[1195].srcOperand = IA_IMM8;
	instruction[1195].valid64 = INVALID;
	instruction[1195].compatLeg = VALID;
	instruction[1195].VMX = 0;
	instruction[1195].description = strdup("r/m16 XOR imm8 (signextended).");
	instruction[1195].binPrefixLen = 0;
	instruction[1195].binOpcodeLen = 1;
	instruction[1195].binOpcode = bpstrndup("\x83", 1);
	instruction[1195].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[1195].opcodeOperand[1] = OA_IB;
	instruction[1195].opcodeOperandCount = 2;

	// Instruction Struct [1196] - XOR
	instruction[1196].instructionStr = strdup("XOR");
	instruction[1196].undecodedStr = strdup("83 /6 ib");
	instruction[1196].dstOperandStr = strdup("r/m32");
	instruction[1196].dstOperand = IA_RM32;
	instruction[1196].srcOperandStr = strdup("imm8");
	instruction[1196].srcOperand = IA_IMM8;
	instruction[1196].valid64 = INVALID;
	instruction[1196].compatLeg = VALID;
	instruction[1196].VMX = 0;
	instruction[1196].description = strdup("r/m32 XOR imm8 (signextended).");
	instruction[1196].binPrefixLen = 0;
	instruction[1196].binOpcodeLen = 1;
	instruction[1196].binOpcode = bpstrndup("\x83", 1);
	instruction[1196].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[1196].opcodeOperand[1] = OA_IB;
	instruction[1196].opcodeOperandCount = 2;

	// Instruction Struct [1197] - XOR
	instruction[1197].instructionStr = strdup("XOR");
	instruction[1197].undecodedStr = strdup("30 /r");
	instruction[1197].dstOperandStr = strdup("r/m8");
	instruction[1197].dstOperand = IA_RM8;
	instruction[1197].srcOperandStr = strdup("r8");
	instruction[1197].srcOperand = IA_R8;
	instruction[1197].valid64 = INVALID;
	instruction[1197].compatLeg = VALID;
	instruction[1197].VMX = 0;
	instruction[1197].description = strdup("r/m8 XOR r8.");
	instruction[1197].binPrefixLen = 0;
	instruction[1197].binOpcodeLen = 1;
	instruction[1197].binOpcode = bpstrndup("\x30", 1);
	instruction[1197].opcodeOperand[0] = OA_MODRM;
	instruction[1197].opcodeOperandCount = 1;

	// Instruction Struct [1198] - XOR
	instruction[1198].instructionStr = strdup("XOR");
	instruction[1198].undecodedStr = strdup("31 /r");
	instruction[1198].dstOperandStr = strdup("r/m16");
	instruction[1198].dstOperand = IA_RM16;
	instruction[1198].srcOperandStr = strdup("r16");
	instruction[1198].srcOperand = IA_R16;
	instruction[1198].valid64 = INVALID;
	instruction[1198].compatLeg = VALID;
	instruction[1198].VMX = 0;
	instruction[1198].description = strdup("r/m16 XOR r16.");
	instruction[1198].binPrefixLen = 0;
	instruction[1198].binOpcodeLen = 1;
	instruction[1198].binOpcode = bpstrndup("\x31", 1);
	instruction[1198].opcodeOperand[0] = OA_MODRM;
	instruction[1198].opcodeOperandCount = 1;

	// Instruction Struct [1199] - XOR
	instruction[1199].instructionStr = strdup("XOR");
	instruction[1199].undecodedStr = strdup("31 /r");
	instruction[1199].dstOperandStr = strdup("r/m32");
	instruction[1199].dstOperand = IA_RM32;
	instruction[1199].srcOperandStr = strdup("r32");
	instruction[1199].srcOperand = IA_R32;
	instruction[1199].valid64 = INVALID;
	instruction[1199].compatLeg = VALID;
	instruction[1199].VMX = 0;
	instruction[1199].description = strdup("r/m32 XOR r32.");
	instruction[1199].binPrefixLen = 0;
	instruction[1199].binOpcodeLen = 1;
	instruction[1199].binOpcode = bpstrndup("\x31", 1);
	instruction[1199].opcodeOperand[0] = OA_MODRM;
	instruction[1199].opcodeOperandCount = 1;

	// Instruction Struct [1200] - XOR
	instruction[1200].instructionStr = strdup("XOR");
	instruction[1200].undecodedStr = strdup("32 /r");
	instruction[1200].dstOperandStr = strdup("r8");
	instruction[1200].dstOperand = IA_R8;
	instruction[1200].srcOperandStr = strdup("r/m8");
	instruction[1200].srcOperand = IA_RM8;
	instruction[1200].valid64 = INVALID;
	instruction[1200].compatLeg = VALID;
	instruction[1200].VMX = 0;
	instruction[1200].description = strdup("r8 XOR r/m8.");
	instruction[1200].binPrefixLen = 0;
	instruction[1200].binOpcodeLen = 1;
	instruction[1200].binOpcode = bpstrndup("\x32", 1);
	instruction[1200].opcodeOperand[0] = OA_MODRM;
	instruction[1200].opcodeOperandCount = 1;

	// Instruction Struct [1201] - XOR
	instruction[1201].instructionStr = strdup("XOR");
	instruction[1201].undecodedStr = strdup("33 /r");
	instruction[1201].dstOperandStr = strdup("r16");
	instruction[1201].dstOperand = IA_R16;
	instruction[1201].srcOperandStr = strdup("r/m16");
	instruction[1201].srcOperand = IA_RM16;
	instruction[1201].valid64 = INVALID;
	instruction[1201].compatLeg = VALID;
	instruction[1201].VMX = 0;
	instruction[1201].description = strdup("r16 XOR r/m16.");
	instruction[1201].binPrefixLen = 0;
	instruction[1201].binOpcodeLen = 1;
	instruction[1201].binOpcode = bpstrndup("\x33", 1);
	instruction[1201].opcodeOperand[0] = OA_MODRM;
	instruction[1201].opcodeOperandCount = 1;

	// Instruction Struct [1202] - XOR
	instruction[1202].instructionStr = strdup("XOR");
	instruction[1202].undecodedStr = strdup("33 /r");
	instruction[1202].dstOperandStr = strdup("r32");
	instruction[1202].dstOperand = IA_R32;
	instruction[1202].srcOperandStr = strdup("r/m32");
	instruction[1202].srcOperand = IA_RM32;
	instruction[1202].valid64 = INVALID;
	instruction[1202].compatLeg = VALID;
	instruction[1202].VMX = 0;
	instruction[1202].description = strdup("r32 XOR r/m32.");
	instruction[1202].binPrefixLen = 0;
	instruction[1202].binOpcodeLen = 1;
	instruction[1202].binOpcode = bpstrndup("\x33", 1);
	instruction[1202].opcodeOperand[0] = OA_MODRM;
	instruction[1202].opcodeOperandCount = 1;

	// Instruction Struct [1203] - XORPD
	instruction[1203].instructionStr = strdup("XORPD");
	instruction[1203].undecodedStr = strdup("66 0F 57 /r");
	instruction[1203].dstOperandStr = strdup("xmm1");
	instruction[1203].dstOperand = IA_XMM;
	instruction[1203].srcOperandStr = strdup("xmm2/m128");
	instruction[1203].srcOperand = IA_XMM_M128;
	instruction[1203].valid64 = INVALID;
	instruction[1203].compatLeg = VALID;
	instruction[1203].VMX = 0;
	instruction[1203].description = strdup("Bitwise exclusive-OR of xmm2/m128 and xmm1.");
	instruction[1203].prefixStr = strdup("OPSIZEOVR");
	instruction[1203].binPrefixLen = 1;
	instruction[1203].binPrefix = bpstrndup("\x66", 1);
	instruction[1203].binOpcodeLen = 2;
	instruction[1203].binOpcode = bpstrndup("\x0F\x57", 2);
	instruction[1203].opcodeOperand[0] = OA_MODRM;
	instruction[1203].opcodeOperandCount = 1;

	// Instruction Struct [1204] - VMCALL
	instruction[1204].instructionStr = strdup("VMCALL");
	instruction[1204].undecodedStr = strdup("0F 01 C1");
	instruction[1204].valid64 = INVALID;
	instruction[1204].compatLeg = INVALID;
	instruction[1204].VMX = 1;
	instruction[1204].description = strdup("Call to VM monitor by causing VM exit.");
	instruction[1204].binPrefixLen = 0;
	instruction[1204].binOpcodeLen = 3;
	instruction[1204].binOpcode = bpstrndup("\x0F\x01\xC1", 3);
	instruction[1204].opcodeOperandCount = 0;

	// Instruction Struct [1205] - VMCLEAR
	instruction[1205].instructionStr = strdup("VMCLEAR");
	instruction[1205].undecodedStr = strdup("66 0F C7 /6");
	instruction[1205].dstOperandStr = strdup("m64");
	instruction[1205].dstOperand = IA_M64;
	instruction[1205].valid64 = INVALID;
	instruction[1205].compatLeg = INVALID;
	instruction[1205].VMX = 1;
	instruction[1205].description = strdup("Copy VMCS data to VMCS region in memory.");
	instruction[1205].prefixStr = strdup("OPSIZEOVR");
	instruction[1205].binPrefixLen = 1;
	instruction[1205].binPrefix = bpstrndup("\x66", 1);
	instruction[1205].binOpcodeLen = 2;
	instruction[1205].binOpcode = bpstrndup("\x0F\xC7", 2);
	instruction[1205].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[1205].opcodeOperandCount = 1;

	// Instruction Struct [1206] - VMLAUNCH
	instruction[1206].instructionStr = strdup("VMLAUNCH");
	instruction[1206].undecodedStr = strdup("0F 01 C2");
	instruction[1206].valid64 = INVALID;
	instruction[1206].compatLeg = INVALID;
	instruction[1206].VMX = 1;
	instruction[1206].description = strdup("Launch virtual machine managed by current VMCS.");
	instruction[1206].binPrefixLen = 0;
	instruction[1206].binOpcodeLen = 3;
	instruction[1206].binOpcode = bpstrndup("\x0F\x01\xC2", 3);
	instruction[1206].opcodeOperandCount = 0;

	// Instruction Struct [1207] - VMRESUME
	instruction[1207].instructionStr = strdup("VMRESUME");
	instruction[1207].undecodedStr = strdup("0F 01 C3");
	instruction[1207].valid64 = INVALID;
	instruction[1207].compatLeg = INVALID;
	instruction[1207].VMX = 1;
	instruction[1207].description = strdup("Resume virtual machine managed by current VMCS.");
	instruction[1207].binPrefixLen = 0;
	instruction[1207].binOpcodeLen = 3;
	instruction[1207].binOpcode = bpstrndup("\x0F\x01\xC3", 3);
	instruction[1207].opcodeOperandCount = 0;

	// Instruction Struct [1208] - VMPTRLD
	instruction[1208].instructionStr = strdup("VMPTRLD");
	instruction[1208].undecodedStr = strdup("0F C7 /6");
	instruction[1208].dstOperandStr = strdup("m64");
	instruction[1208].dstOperand = IA_M64;
	instruction[1208].valid64 = INVALID;
	instruction[1208].compatLeg = INVALID;
	instruction[1208].VMX = 1;
	instruction[1208].description = strdup("Loads the current VMCS pointer from memory.");
	instruction[1208].binPrefixLen = 0;
	instruction[1208].binOpcodeLen = 2;
	instruction[1208].binOpcode = bpstrndup("\x0F\xC7", 2);
	instruction[1208].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[1208].opcodeOperandCount = 1;

	// Instruction Struct [1209] - VMPTRST
	instruction[1209].instructionStr = strdup("VMPTRST");
	instruction[1209].undecodedStr = strdup("0F C7 /7");
	instruction[1209].dstOperandStr = strdup("m64");
	instruction[1209].dstOperand = IA_M64;
	instruction[1209].valid64 = INVALID;
	instruction[1209].compatLeg = INVALID;
	instruction[1209].VMX = 1;
	instruction[1209].description = strdup("Stores the current VMCS pointer into memory.");
	instruction[1209].binPrefixLen = 0;
	instruction[1209].binOpcodeLen = 2;
	instruction[1209].binOpcode = bpstrndup("\x0F\xC7", 2);
	instruction[1209].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[1209].opcodeOperandCount = 1;

	// Instruction Struct [1210] - VMREAD
	instruction[1210].instructionStr = strdup("VMREAD");
	instruction[1210].undecodedStr = strdup("0F 78");
	instruction[1210].dstOperandStr = strdup("r/m64");
	instruction[1210].dstOperand = IA_RM64;
	instruction[1210].srcOperandStr = strdup("r64");
	instruction[1210].srcOperand = IA_R64;
	instruction[1210].valid64 = INVALID;
	instruction[1210].compatLeg = INVALID;
	instruction[1210].VMX = 1;
	instruction[1210].description = strdup("Reads a specified VMCS field (in 64-bit mode).");
	instruction[1210].binPrefixLen = 0;
	instruction[1210].binOpcodeLen = 2;
	instruction[1210].binOpcode = bpstrndup("\x0F\x78", 2);
	instruction[1210].opcodeOperandCount = 0;

	// Instruction Struct [1211] - VMREAD
	instruction[1211].instructionStr = strdup("VMREAD");
	instruction[1211].undecodedStr = strdup("0F 78");
	instruction[1211].dstOperandStr = strdup("r/m32");
	instruction[1211].dstOperand = IA_RM32;
	instruction[1211].srcOperandStr = strdup("r32");
	instruction[1211].srcOperand = IA_R32;
	instruction[1211].valid64 = INVALID;
	instruction[1211].compatLeg = INVALID;
	instruction[1211].VMX = 1;
	instruction[1211].description = strdup("Reads a specified VMCS field (outside 64-bit mode).");
	instruction[1211].binPrefixLen = 0;
	instruction[1211].binOpcodeLen = 2;
	instruction[1211].binOpcode = bpstrndup("\x0F\x78", 2);
	instruction[1211].opcodeOperandCount = 0;

	// Instruction Struct [1212] - VMWRITE
	instruction[1212].instructionStr = strdup("VMWRITE");
	instruction[1212].undecodedStr = strdup("0F 79");
	instruction[1212].dstOperandStr = strdup("r64");
	instruction[1212].dstOperand = IA_R64;
	instruction[1212].srcOperandStr = strdup("r/m64");
	instruction[1212].srcOperand = IA_RM64;
	instruction[1212].valid64 = INVALID;
	instruction[1212].compatLeg = INVALID;
	instruction[1212].VMX = 1;
	instruction[1212].description = strdup("Writes.a specified VMCS field (in 64-bit mode)");
	instruction[1212].binPrefixLen = 0;
	instruction[1212].binOpcodeLen = 2;
	instruction[1212].binOpcode = bpstrndup("\x0F\x79", 2);
	instruction[1212].opcodeOperandCount = 0;

	// Instruction Struct [1213] - VMWRITE
	instruction[1213].instructionStr = strdup("VMWRITE");
	instruction[1213].undecodedStr = strdup("0F 79");
	instruction[1213].dstOperandStr = strdup("r32");
	instruction[1213].dstOperand = IA_R32;
	instruction[1213].srcOperandStr = strdup("r/m32");
	instruction[1213].srcOperand = IA_RM32;
	instruction[1213].valid64 = INVALID;
	instruction[1213].compatLeg = INVALID;
	instruction[1213].VMX = 1;
	instruction[1213].description = strdup("Writes.a specified VMCS field (outside 64-bit mode)");
	instruction[1213].binPrefixLen = 0;
	instruction[1213].binOpcodeLen = 2;
	instruction[1213].binOpcode = bpstrndup("\x0F\x79", 2);
	instruction[1213].opcodeOperandCount = 0;

	// Instruction Struct [1214] - VMXOFF
	instruction[1214].instructionStr = strdup("VMXOFF");
	instruction[1214].undecodedStr = strdup("0F 01 C4");
	instruction[1214].valid64 = INVALID;
	instruction[1214].compatLeg = INVALID;
	instruction[1214].VMX = 1;
	instruction[1214].description = strdup("Leaves VMX operation.");
	instruction[1214].binPrefixLen = 0;
	instruction[1214].binOpcodeLen = 3;
	instruction[1214].binOpcode = bpstrndup("\x0F\x01\xC4", 3);
	instruction[1214].opcodeOperandCount = 0;

	// Instruction Struct [1215] - VMXON
	instruction[1215].instructionStr = strdup("VMXON");
	instruction[1215].undecodedStr = strdup("F3 0F C7 /6");
	instruction[1215].dstOperandStr = strdup("m64");
	instruction[1215].dstOperand = IA_M64;
	instruction[1215].valid64 = INVALID;
	instruction[1215].compatLeg = INVALID;
	instruction[1215].VMX = 1;
	instruction[1215].description = strdup("Enter VMX root operation.");
	instruction[1215].prefixStr = strdup("REP");
	instruction[1215].binPrefixLen = 1;
	instruction[1215].binPrefix = bpstrndup("\xF3", 1);
	instruction[1215].binOpcodeLen = 2;
	instruction[1215].binOpcode = bpstrndup("\x0F\xC7", 2);
	instruction[1215].opcodeOperand[0] = OA_RM_ONLY_6;
	instruction[1215].opcodeOperandCount = 1;

	// Instruction Struct [1216] - CMP
	instruction[1216].instructionStr = strdup("CMP");
	instruction[1216].undecodedStr = strdup("83 /7 ib");
	instruction[1216].dstOperandStr = strdup("r/m16");
	instruction[1216].dstOperand = IA_RM16;
	instruction[1216].srcOperandStr = strdup("imm8");
	instruction[1216].srcOperand = IA_IMM8;
	instruction[1216].valid64 = INVALID;
	instruction[1216].compatLeg = VALID;
	instruction[1216].VMX = 0;
	instruction[1216].description = strdup("Compare imm8 with r/m16.");
	instruction[1216].binPrefixLen = 0;
	instruction[1216].binOpcodeLen = 1;
	instruction[1216].binOpcode = bpstrndup("\x83", 1);
	instruction[1216].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[1216].opcodeOperand[1] = OA_IB;
	instruction[1216].opcodeOperandCount = 2;

	// Instruction Struct [1217] - CMP
	instruction[1217].instructionStr = strdup("CMP");
	instruction[1217].undecodedStr = strdup("83 /7 ib");
	instruction[1217].dstOperandStr = strdup("r/m32");
	instruction[1217].dstOperand = IA_RM32;
	instruction[1217].srcOperandStr = strdup("imm8");
	instruction[1217].srcOperand = IA_IMM8;
	instruction[1217].valid64 = INVALID;
	instruction[1217].compatLeg = VALID;
	instruction[1217].VMX = 0;
	instruction[1217].description = strdup("Compare imm8 with r/m32.");
	instruction[1217].binPrefixLen = 0;
	instruction[1217].binOpcodeLen = 1;
	instruction[1217].binOpcode = bpstrndup("\x83", 1);
	instruction[1217].opcodeOperand[0] = OA_RM_ONLY_7;
	instruction[1217].opcodeOperand[1] = OA_IB;
	instruction[1217].opcodeOperandCount = 2;

	// Instruction Struct [1218] - CMP
	instruction[1218].instructionStr = strdup("CMP");
	instruction[1218].undecodedStr = strdup("38 /r");
	instruction[1218].dstOperandStr = strdup("r/m8");
	instruction[1218].dstOperand = IA_RM8;
	instruction[1218].srcOperandStr = strdup("r8");
	instruction[1218].srcOperand = IA_R8;
	instruction[1218].valid64 = INVALID;
	instruction[1218].compatLeg = VALID;
	instruction[1218].VMX = 0;
	instruction[1218].description = strdup("Compare r8 with r/m8.");
	instruction[1218].binPrefixLen = 0;
	instruction[1218].binOpcodeLen = 1;
	instruction[1218].binOpcode = bpstrndup("\x38", 1);
	instruction[1218].opcodeOperand[0] = OA_MODRM;
	instruction[1218].opcodeOperandCount = 1;

	// Instruction Struct [1219] - CMP
	instruction[1219].instructionStr = strdup("CMP");
	instruction[1219].undecodedStr = strdup("39 /r");
	instruction[1219].dstOperandStr = strdup("r/m16");
	instruction[1219].dstOperand = IA_RM16;
	instruction[1219].srcOperandStr = strdup("r16");
	instruction[1219].srcOperand = IA_R16;
	instruction[1219].valid64 = INVALID;
	instruction[1219].compatLeg = VALID;
	instruction[1219].VMX = 0;
	instruction[1219].description = strdup("Compare r16 with r/m16.");
	instruction[1219].binPrefixLen = 0;
	instruction[1219].binOpcodeLen = 1;
	instruction[1219].binOpcode = bpstrndup("\x39", 1);
	instruction[1219].opcodeOperand[0] = OA_MODRM;
	instruction[1219].opcodeOperandCount = 1;

	// Instruction Struct [1220] - CMP
	instruction[1220].instructionStr = strdup("CMP");
	instruction[1220].undecodedStr = strdup("39 /r");
	instruction[1220].dstOperandStr = strdup("r/m32");
	instruction[1220].dstOperand = IA_RM32;
	instruction[1220].srcOperandStr = strdup("r32");
	instruction[1220].srcOperand = IA_R32;
	instruction[1220].valid64 = INVALID;
	instruction[1220].compatLeg = VALID;
	instruction[1220].VMX = 0;
	instruction[1220].description = strdup("Compare r32 with r/m32.");
	instruction[1220].binPrefixLen = 0;
	instruction[1220].binOpcodeLen = 1;
	instruction[1220].binOpcode = bpstrndup("\x39", 1);
	instruction[1220].opcodeOperand[0] = OA_MODRM;
	instruction[1220].opcodeOperandCount = 1;

	// Instruction Struct [1221] - CMP
	instruction[1221].instructionStr = strdup("CMP");
	instruction[1221].undecodedStr = strdup("3A /r");
	instruction[1221].dstOperandStr = strdup("r8");
	instruction[1221].dstOperand = IA_R8;
	instruction[1221].srcOperandStr = strdup("r/m8");
	instruction[1221].srcOperand = IA_RM8;
	instruction[1221].valid64 = INVALID;
	instruction[1221].compatLeg = VALID;
	instruction[1221].VMX = 0;
	instruction[1221].description = strdup("Compare r/m8 with r8.");
	instruction[1221].binPrefixLen = 0;
	instruction[1221].binOpcodeLen = 1;
	instruction[1221].binOpcode = bpstrndup("\x3A", 1);
	instruction[1221].opcodeOperand[0] = OA_MODRM;
	instruction[1221].opcodeOperandCount = 1;

	// Instruction Struct [1222] - CMP
	instruction[1222].instructionStr = strdup("CMP");
	instruction[1222].undecodedStr = strdup("3B /r");
	instruction[1222].dstOperandStr = strdup("r16");
	instruction[1222].dstOperand = IA_R16;
	instruction[1222].srcOperandStr = strdup("r/m16");
	instruction[1222].srcOperand = IA_RM16;
	instruction[1222].valid64 = INVALID;
	instruction[1222].compatLeg = VALID;
	instruction[1222].VMX = 0;
	instruction[1222].description = strdup("Compare r/m16 with r16.");
	instruction[1222].binPrefixLen = 0;
	instruction[1222].binOpcodeLen = 1;
	instruction[1222].binOpcode = bpstrndup("\x3B", 1);
	instruction[1222].opcodeOperand[0] = OA_MODRM;
	instruction[1222].opcodeOperandCount = 1;

	// Instruction Struct [1223] - CMP
	instruction[1223].instructionStr = strdup("CMP");
	instruction[1223].undecodedStr = strdup("3B /r");
	instruction[1223].dstOperandStr = strdup("r32");
	instruction[1223].dstOperand = IA_R32;
	instruction[1223].srcOperandStr = strdup("r/m32");
	instruction[1223].srcOperand = IA_RM32;
	instruction[1223].valid64 = INVALID;
	instruction[1223].compatLeg = VALID;
	instruction[1223].VMX = 0;
	instruction[1223].description = strdup("Compare r/m32 with r32.");
	instruction[1223].binPrefixLen = 0;
	instruction[1223].binOpcodeLen = 1;
	instruction[1223].binOpcode = bpstrndup("\x3B", 1);
	instruction[1223].opcodeOperand[0] = OA_MODRM;
	instruction[1223].opcodeOperandCount = 1;
		
	memcpy(retHeapVar, &instruction[0], sizeof(X86_32_INSTRUCTION) *1223 );
	
	return retHeapVar;
}

#endif // 32bit debugging

#endif // USE_BLOCKPARTY_DEBUGGING == 1

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

// Retrieves list of instruction elements (instructions) given a start
// and end address.  __No list pointers are set for this element.__
P_X86_32_DECODED_INSTRUCTION_LIST X86_32_GetInstructionsFromMem(P_DEBUG_HANDLE dbgHandle, P_X86_32_INSTRUCTION instTable, void *start, void *top){

	if(dbgHandle == NULL)
		return NULL;

	if(top == NULL)
		return NULL;

	if(start == NULL)
		return NULL;

	if(top <= start)
		return NULL;

	// Read in the bytes to decode
	unsigned char * binaryBlob = NULL;
	binaryBlob = DebugReadMemory(dbgHandle, start, (size_t) top - (size_t) start);

	printf("\n undecoded read: %.02x %.02x %.02x %.02x", binaryBlob[0], binaryBlob[1], binaryBlob[2], binaryBlob[3]);

	if(binaryBlob == NULL)
		return NULL;

	// detach immediately and trace process so we can attach
	// ptrace(PT_DETACH, dbgHandle->debugPid, 0, 0);
	// printf("\n wtf: %i", dbgHandle->debugPid);



	// 1 if it matches, -1 if its borked/invalid, 0 if there is no prefix
	int prefixMatch = 0;

	// 1 if it matches, -1 if its invalid,
	int opcodeMatch = 0;

	// 1 if it matches, 0 if there is no encoding of rPlus instructions
	unsigned char rPlusEncoded = 0;

	// Instruction Vars
	size_t instLength = 0;
	size_t instPosition = 0;

	P_X86_32_INSTRUCTION tableFirst = instTable;


	// Exit Label
	end:
	if(instTable->dstOperand != NULL){
		// printf(" %s ", instTable->dstOperand);
	}

	if(instTable->srcOperand != NULL){
		// printf(" %s ", instTable->dstOperand);
	}

	return NULL;
}

// Determines if a opcode byte is a prefix or not
char * X86_32_IsBytePrefix(unsigned char byte){

	switch(byte){

		case 0xf2:
			return "REPNE";

		case 0xf3:
			return "REP";

		case 0x2e:
			return "CS:";

		case 0x36:
			return "SS:";

		case 0x3e:
			return "DS:";

		case 0x26:
			return "ES:";

		case 0x64:
			return "FS:";

		case 0x65:
			return "GS:";

		// Operand Size operand prefix
		case 0x66:
			return "OPSIZE";

		// Address Size Override Pefix
		case 0x67:
			return "ADDRSIZE";

		default:
			return NULL;

	}

	return NULL;
}

// Determines if an instruction is +i encoded for floating
// point operations.
int X86_32_IsInstructionFloatingPointPlusEncoded(P_X86_32_DECODED_INSTRUCTION partialDecode){
	if(partialDecode == NULL)
		return false;

	// potential 2 byte opcodes maximum
	// match first byte first, then check second bytes
	switch((unsigned char ) partialDecode->instruction->binOpcode[0]){
		// First primary, FADD
		case 0xD8:
			switch(partialDecode->instruction->binOpcode[1]){
				// FADD
				case 0xc0:
					return true;
				// FMUL
				case 0xc8:
					return true;
				// FCOM
				case 0xd0:
					return true;
				// FCOMP
				case 0xd8:
					return true;
				// FSUB
				case 0xe0:
					return true;
				// FSUBR
				case 0xe8:
					return true;
				// FDIV
				case 0xf0:
					return true;
				// FDIVR
				case 0xf8:
					return true;
			}
			break;
		case 0xD9:
			switch(partialDecode->instruction->binOpcode[1]){
				// FLD
				case 0xc0:
					return true;
				case 0xc8:
					return true;
			}
			break;

		case 0xDA:
			switch(partialDecode->instruction->binOpcode[1]){
				// FLD
				case 0xc0:
					return true;
				case 0xc8:
					return true;
				case 0xd0:
					return true;
				case 0xd8:
					return true;

			}
			break;

		case 0xDB:
			switch(partialDecode->instruction->binOpcode[1]){
				// FLD
				case 0xc0:
					return true;
				case 0xc8:
					return true;
				case 0xd0:
					return true;
				case 0xd8:
					return true;
				case 0xe8:
					return true;
				case 0xf0:
					return true;
			}
		case 0xDC:
			switch(partialDecode->instruction->binOpcode[1]){
				// FLD
				case 0xc0:
					return true;
				case 0xc8:
					return true;
				case 0xe0:
					return true;
				case 0xe8:
					return true;
				case 0xf0:
					return true;
				case 0xf8:
					return true;
			}
		case 0xDD:
			switch(partialDecode->instruction->binOpcode[1]){
				// FLD
				case 0xc0:
					return true;
				case 0xd0:
					return true;
				case 0xd8:
					return true;
				case 0xe0:
					return true;
				case 0xe8:
					return true;
			}

		case 0xDE:
			switch(partialDecode->instruction->binOpcode[1]){
				// FLD
				case 0xc0:
					return true;
				case 0xc8:
					return true;
				case 0xe0:
					return true;
				case 0xe8:
					return true;
				case 0xf0:
					return true;
				case 0xf8:
					return true;
			}


		case 0xDF:
			switch(partialDecode->instruction->binOpcode[1]){
				case 0xe8:
					return true;
				case 0xf0:
					return true;

			}


	}


}


// Determines if the byte is a +rb etc instruction, string must be at least 2
// bytes long.
int X86_32_IsBytePlusEncodedOpcode(unsigned char *opstr){

	if(opstr == NULL)
		return false;

	// Database shows suprisingly 0 +ro instructions
	// if bug is found, investigate later.

	//  MOV + rb check
	if(opstr[0] >= 0xb0 && opstr[0] < (0xb0+8)){
		return 0xB0;
	}

	// MOV + rw / rd check
	if(opstr[0] >= 0xb8 && opstr[0] < (0xb8+8)){
		return 0xB8;
	}

	// BSWAP + rd check (requires first byte as simd 0f prefix)
	if(opstr[0] == 0x0f)
	if(opstr[1] >= 0xC8 && opstr[1] < (0xc8+8)){
		return 0xC8;
	}

	// DEC + rd check
	if(opstr[0] >= 0x48 && opstr[0] < (0x48+8)){
		return 0x48;
	}

	// INC + rd check
	if(opstr[0] >= 0x40 && opstr[0] < (0x40+8)){
		return 0x40;
	}

	// POP + rd check (also a 64 bit instructions)
	if(opstr[0] >= 0x58 && opstr[0] < (0x58+8)){
		return 0x58;
	}

	// PUSH + rd check
	if(opstr[0] >= 0x50 && opstr[0] < (0x50+8)){
		return 0x50;
	}

	// XCHG + rd check
	if(opstr[0] >= 0x90 && opstr[0] < (0x90+8)){
		return 0x90;
	}

	return 0;
}


// Tests the RM byte in an instruction to see if it contains a valid modr/m byte.
// greater then 1 for true, 0 for false
size_t X86_32_InstructionContainsValidModRM(P_X86_32_INSTRUCTION instruction){

	for(size_t i = 0; i < instruction->opcodeOperandCount; i++){

		// If there is a forward slash in the operand then maybee
		if(instruction->opcodeOperand[i] != 0){

			if(X86_32_IsOperandMemOnly(instruction->opcodeOperand[i]) != 0)
				return true;

			if(X86_32_IsOperandRegOnly(instruction->opcodeOperand[i]) != 0)
				return true;

			if(X86_32_IsOperandRm(instruction->opcodeOperand[i]) != 0)
				return true;

			// for slash digit ones
			switch(instruction->opcodeOperand[i]){
				case OA_RM_ONLY_0:
					return true;
				case OA_RM_ONLY_1:
					return true;
				case OA_RM_ONLY_2:
					return true;
				case OA_RM_ONLY_3:
					return true;
				case OA_RM_ONLY_4:
					return true;
				case OA_RM_ONLY_5:
					return true;
				case OA_RM_ONLY_6:
					return true;
				case OA_RM_ONLY_7:
					return true;

			}
		}

	}

	// Not a valid byte, returning
	return 0;
}

// 32 bit MOD RM Decoder, fills in the string values in the struct.  This is reliant
// on source and destination operand formatting.
P_X86_32_DECODED_INSTRUCTION_LIST X86_32_SetRMValues(P_X86_32_INSTRUCTION instTable, P_X86_32_DECODED_INSTRUCTION_LIST inList){

	if(inList == NULL)
		return NULL;

	// If either of these values are set it means the instruction
	// has been either corrupted, or previously visited.
	if(inList->regStr != NULL)
		return NULL;

	if(inList->rmStr != NULL)
		return NULL;

	// Retrieve MOD Value From Byte
	inList->mod = IA32_MODRM_GET_MOD(inList->modRM);

	// Retrieve RM Value From Byte
	inList->reg = IA32_MODRM_GET_REG(inList->modRM);

	// Retrieve Digit/Opcode Value From Byte
	inList->rm = IA32_MODRM_GET_RM(inList->modRM);

	// Get Src Operand
	size_t srcOperand = instTable->srcOperand;
	size_t dstOperand = instTable->dstOperand;
		char *regStr = NULL;
		char *rmStr = NULL;

	printf("\n\t Initial Value: %.02x", inList->modRM);
	printf("\n\t mod: %.02x", inList->mod);
	printf("\n\t reg: %.02x", inList->reg);
	printf("\n\t rm:  %.02x", inList->rm);

	// First determine the register
	switch(inList->reg){

		// 00 [000] 000 -  [al]
		case 0x0:
			// Need to check operands with big switch
			// Src First
			switch(instTable->srcOperand){
				case IA_R8:
					regStr = "AL";
					break;
				case IA_R16:
					regStr = "AX";
					break;
				case IA_R32:
					regStr = "EAX";
					break;
				case IA_MM:
					regStr = "MM0";
					break;
				case IA_XMM:
					regStr = "XMM0";
					break;
			}

			// Dst Next
			switch(instTable->dstOperand){
				case IA_R8:
					regStr = "AL";
					break;
				case IA_R16:
					regStr = "AX";
					break;
				case IA_R32:
					regStr = "EAX";
					break;
				case IA_MM:
					regStr = "MM0";
					break;
				case IA_XMM:
					regStr = "XMM0";
					break;
			}
			break;

		// 00 [001] 000 - cl
		case 0x8:
			// Need to check operands with big switch
			// Src First
			switch(instTable->srcOperand){
				case IA_R8:
					regStr = "CL";
					break;
				case IA_R16:
					regStr = "CX";
					break;
				case IA_R32:
					regStr = "ECX";
					break;
				case IA_MM:
					regStr = "MM1";
					break;
				case IA_XMM:
					regStr = "XMM1";
					break;
			}

			// Dst Next
			switch(instTable->dstOperand){
				case IA_R8:
					regStr = "CL";
					break;
				case IA_R16:
					regStr = "CX";
					break;
				case IA_R32:
					regStr = "ECX";
					break;
				case IA_MM:
					regStr = "MM1";
					break;
				case IA_XMM:
					regStr = "XMM1";
					break;
			}
			break;

		// 00 [010] 000 0 - dl
		case 0x10:
			// Need to check operands with big switch
			// Src First
			switch(instTable->srcOperand){
				case IA_R8:
					regStr = "DL";
					break;
				case IA_R16:
					regStr = "DX";
					break;
				case IA_R32:
					regStr = "EDX";
					break;
				case IA_MM:
					regStr = "MM2";
					break;
				case IA_XMM:
					regStr = "XMM2";
					break;
			}

			// Dst Next
			switch(instTable->dstOperand){
				case IA_R8:
					regStr = "DL";
					break;
				case IA_R16:
					regStr = "DX";
					break;
				case IA_R32:
					regStr = "EDX";
					break;
				case IA_MM:
					regStr = "MM2";
					break;
				case IA_XMM:
					regStr = "XMM2";
					break;
			}
			break;

		// 00 [011] 000 - bl
		case 0x18:
			// Need to check operands with big switch
			// Src First
			switch(instTable->srcOperand){
				case IA_R8:
					regStr = "BL";
					break;
				case IA_R16:
					regStr = "BX";
					break;
				case IA_R32:
					regStr = "EBX";
					break;
				case IA_MM:
					regStr = "MM3";
					break;
				case IA_XMM:
					regStr = "XMM3";
					break;
			}

			// Dst Next
			switch(instTable->dstOperand){
				case IA_R8:
					regStr = "BL";
					break;
				case IA_R16:
					regStr = "BX";
					break;
				case IA_R32:
					regStr = "EBX";
					break;
				case IA_MM:
					regStr = "MM3";
					break;
				case IA_XMM:
					regStr = "XMM3";
					break;
			}
			break;

		// 00 [100] 000 - ah
		case 0x20:
			// Need to check operands with big switch
			// Src First
			switch(instTable->srcOperand){
				case IA_R8:
					regStr = "AH";
					break;
				case IA_R16:
					regStr = "SP";
					break;
				case IA_R32:
					regStr = "ESP";
					break;
				case IA_MM:
					regStr = "MM4";
					break;
				case IA_XMM:
					regStr = "XMM4";
					break;
			}

			// Dst Next
			switch(instTable->dstOperand){
				case IA_R8:
					regStr = "AH";
					break;
				case IA_R16:
					regStr = "SP";
					break;
				case IA_R32:
					regStr = "ESP";
					break;
				case IA_MM:
					regStr = "MM4";
					break;
				case IA_XMM:
					regStr = "XMM4";
					break;
			}
			break;

		// 00 [101] 000 - ch
		case 0x28:
			// Need to check operands with big switch
			// Src First
			switch(instTable->srcOperand){
				case IA_R8:
					regStr = "CH";
					break;
				case IA_R16:
					regStr = "BP";
					break;
				case IA_R32:
					regStr = "EBP";
					break;
				case IA_MM:
					regStr = "MM5";
					break;
				case IA_XMM:
					regStr = "XMM5";
					break;
			}

			// Dst Next
			switch(instTable->dstOperand){
				case IA_R8:
					regStr = "CH";
					break;
				case IA_R16:
					regStr = "BP";
					break;
				case IA_R32:
					regStr = "EBP";
					break;
				case IA_MM:
					regStr = "MM5";
					break;
				case IA_XMM:
					regStr = "XMM5";
					break;
			}
			break;

		// 00 [110] 000 - dh
		case 0x30:
			// Need to check operands with big switch
			// Src First
			switch(instTable->srcOperand){
				case IA_R8:
					regStr = "DH";
					break;
				case IA_R16:
					regStr = "SI";
					break;
				case IA_R32:
					regStr = "ESI";
					break;
				case IA_MM:
					regStr = "MM6";
					break;
				case IA_XMM:
					regStr = "XMM6";
					break;
			}

			// Dst Next
			switch(instTable->dstOperand){
				case IA_R8:
					regStr = "DH";
					break;
				case IA_R16:
					regStr = "SI";
					break;
				case IA_R32:
					regStr = "ESI";
					break;
				case IA_MM:
					regStr = "MM6";
					break;
				case IA_XMM:
					regStr = "XMM6";
					break;
			}
			break;

		// 00 [111] 000 - bh
		case 0x38:
			// Need to check operands with big switch
			// Src First
			switch(instTable->srcOperand){
				case IA_R8:
					regStr = "BH";
					break;
				case IA_R16:
					regStr = "DI";
					break;
				case IA_R32:
					regStr = "EDI";
					break;
				case IA_MM:
					regStr = "MM7";
					break;
				case IA_XMM:
					regStr = "XMM7";
					break;
			}

			// Dst Next
			switch(instTable->dstOperand){
				case IA_R8:
					regStr = "BH";
					break;
				case IA_R16:
					regStr = "DI";
					break;
				case IA_R32:
					regStr = "EDI";
					break;
				case IA_MM:
					regStr = "MM7";
					break;
				case IA_XMM:
					regStr = "XMM7";
					break;
			}
			break;
	}

	if(regStr != NULL){
		// printf("\n decoded reg: %s", regStr);
	}

	// Big R/M Decoder

	// First Modifier Translation
	if(inList->mod == 0x00){

		// Next determine the reg/memory
		switch(inList->rm){
			case 0x0:
				rmStr = "EAX";
				break;
			case 0x1:
				rmStr = "ECX";
				break;
			case 0x2:
				rmStr = "EDX";
				break;
			case 0x3:
				rmStr = "EBX";
				break;
			// SIB Encoding - Write Function to Decode SIB Byte
			case 0x4:
				rmStr = "[--][--]";
				break;
			case 0x5:
				rmStr = "disp32";
				break;
			case 0x6:
				rmStr = "ESI";
				break;
			case 0x7:
				rmStr = "EDI";
				break;
		}
		goto end;
	}



	// Second Modifier Translation
	if(inList->mod == 0x40){
		printf("\n MOD IS 0x40");
		// Next determine the reg/memory
		switch(inList->rm){
			case 0x0:
				rmStr = "EAX+disp8";
				break;
			case 0x1:
				rmStr = "ECX+disp8";
				break;
			case 0x2:
				rmStr = "EDX+disp8";
				break;
			case 0x3:
				rmStr = "EBX+disp8";
				break;
			// SIB Encoding - Write Function to Decode SIB Byte
			case 0x4:
				rmStr = "[--][--] + disp8";
				break;
			case 0x5:
				rmStr = "EBP+disp8";
				break;
			case 0x6:
				rmStr = "ESI+disp8";
				break;
			case 0x7:
				rmStr = "EDI+disp8";
				break;
		}
		goto end;
	}

	// Third Modifier Translation
	if(inList->mod == 0x80){
		printf("\n MOD IS 0x80");
		printf("\n\t rm: %.02x", inList->rm);

		// Next determine the reg/memory
		switch(inList->rm){
			case 0x0:
				rmStr = "EAX+disp32";
				break;
			case 0x1:
				rmStr = "ECX+disp32";
				break;
			case 0x2:
				rmStr = "EDX+disp32";
				break;
			case 0x3:
				rmStr = "EBX+disp32";
				break;
			// SIB Encoding - Write Function to Decode SIB Byte
			case 0x4:
				rmStr = "[--][--] + disp32";
				break;
			case 0x5:
				rmStr = "EBP+disp32";
				break;
			case 0x6:
				rmStr = "ESI+disp32";
				break;
			case 0x7:
				rmStr = "EDI+disp32";
				break;
		}

		goto end;
	}

	// Fourth Modifier Translation
	if(inList->mod == 0xc0){

		printf("\n MOD IS 0xc0");
		printf("\n rm === %.02x", inList->rm);
		// Next determine the reg/memory
		switch(inList->rm){
			case 0x0:
				// Src First
				switch(instTable->srcOperand){
					case IA_RM8:
						rmStr = "AL";
						break;
					case IA_RM16:
						rmStr = "AX";
						break;
					case IA_RM32:
						rmStr = "EAX";
						break;
					case IA_MM:
						rmStr = "MM0";
						break;
					case IA_XMM:
						rmStr = "XMM0";
						break;

					case IA_XMM_M32:
						rmStr = "XMM0";
						break;
					case IA_XMM_M64:
						rmStr = "XMM0";
						break;
					case IA_XMM_M128:
						rmStr = "XMM0";
						break;


				}

				// Dst Next
				switch(instTable->dstOperand){
					case IA_RM8:
						rmStr = "AL";
						break;
					case IA_RM16:
						rmStr = "AX";
						break;
					case IA_RM32:
						rmStr = "EAX";
						break;
					case IA_MM:
						rmStr = "MM0";
						break;
					case IA_XMM:
						rmStr = "XMM0";
						break;
				}
				break;

			// ECX
			case 0x1:
				printf("\n BUMMSNAGGERS: %i", (size_t) instTable->srcOperand);
				// Src First
				switch(instTable->srcOperand){
					case IA_RM8:
						rmStr = "CL";
						break;
					case IA_RM16:
						rmStr = "CX";
						break;
					case IA_RM32:
						rmStr = "ECX";
						break;
					case IA_MM:
						rmStr = "MM1";
						break;
					case IA_XMM:
						rmStr = "XMM1";
						break;
				}

				// Dst Next
				switch(instTable->dstOperand){
					case IA_RM8:
						rmStr = "CL";
						break;
					case IA_RM16:
						rmStr = "CX";
						break;
					case IA_RM32:
						rmStr = "ECX";
						break;
					case IA_MM:
						rmStr = "MM1";
						break;
					case IA_XMM:
						rmStr = "XMM1";
						break;
				}
				break;

			// EDX
			case 0x2:
				// Src First
				switch(instTable->srcOperand){
					case IA_RM8:
						rmStr = "DL";
						break;
					case IA_RM16:
						rmStr = "DX";
						break;
					case IA_RM32:
						rmStr = "EDX";
						break;
					case IA_MM:
						rmStr = "MM2";
						break;
					case IA_XMM:
						rmStr = "XMM2";
						break;
				}

				// Dst Next
				switch(instTable->dstOperand){
					case IA_RM8:
						rmStr = "DL";
						break;
					case IA_RM16:
						rmStr = "DX";
						break;
					case IA_RM32:
						rmStr = "EDX";
						break;
					case IA_MM:
						rmStr = "MM2";
						break;
					case IA_XMM:
						rmStr = "XMM2";
						break;
				}
				break;

			// EBX
			case 0x3:
				// Src First
				switch(instTable->srcOperand){
					case IA_RM8:
						rmStr = "BL";
						break;
					case IA_RM16:
						rmStr = "BX";
						break;
					case IA_RM32:
						rmStr = "EBX";
						break;
					case IA_MM:
						rmStr = "MM3";
						break;
					case IA_XMM:
						rmStr = "XMM3";
						break;
				}

				// Dst Next
				switch(instTable->dstOperand){
					case IA_RM8:
						rmStr = "BL";
						break;
					case IA_RM16:
						rmStr = "BX";
						break;
					case IA_RM32:
						rmStr = "EBX";
						break;
					case IA_MM:
						rmStr = "MM3";
						break;
					case IA_XMM:
						rmStr = "XMM3";
						break;
				}
				break;

			// ESP
			case 0x4:
				// Src First
				switch(instTable->srcOperand){
				case IA_RM8:
					rmStr = "AH";
						break;
					case IA_RM16:
						rmStr = "SP";
						break;
					case IA_RM32:
						rmStr = "ESP";
						break;
					case IA_MM:
						rmStr = "MM4";
						break;
					case IA_XMM:
						rmStr = "XMM4";
						break;
				}

			// Dst Next
			switch(instTable->dstOperand){
				case IA_RM8:
					rmStr = "AH";
					break;
				case IA_RM16:
					rmStr = "SP";
					break;
				case IA_RM32:
					rmStr = "ESP";
					break;
				case IA_MM:
					rmStr = "MM4";
					break;
				case IA_XMM:
					rmStr = "XMM4";
					break;
				}
				break;

			// EBP
			case 0x5:
				// Src First
				switch(instTable->srcOperand){
					case IA_RM8:
						rmStr = "CH";
						break;
					case IA_RM16:
						rmStr = "BP";
						break;
					case IA_RM32:
						rmStr = "EBP";
						break;
					case IA_MM:
						rmStr = "MM5";
						break;
					case IA_XMM:
						rmStr = "XMM5";
						break;
				}

				// Dst Next
				switch(instTable->dstOperand){
					case IA_RM8:
						rmStr = "CH";
						break;
					case IA_RM16:
						rmStr = "BP";
						break;
					case IA_RM32:
						rmStr = "EBP";
						break;
					case IA_MM:
						rmStr = "MM5";
						break;
					case IA_XMM:
						rmStr = "XMM5";
						break;
				}
				break;

			// ESI
			case 0x6:
				// Src First
				switch(instTable->srcOperand){
					case IA_RM8:
						rmStr = "DH";
						break;
					case IA_RM16:
						rmStr = "SI";
						break;
					case IA_RM32:
						rmStr = "ESI";
						break;
					case IA_MM:
						rmStr = "MM6";
						break;
					case IA_XMM:
						rmStr = "XMM6";
						break;
				}

				// Dst Next
				switch(instTable->dstOperand){
					case IA_RM8:
						rmStr = "DH";
						break;
					case IA_RM16:
						rmStr = "SI";
						break;
					case IA_RM32:
						rmStr = "ESI";
						break;
					case IA_MM:
						rmStr = "MM6";
						break;
					case IA_XMM:
						rmStr = "XMM6";
						break;
				}
				break;

			// EDI
			case 0x7:
				// Src First
				switch(instTable->srcOperand){
					case IA_RM8:
						rmStr = "BH";
						break;
					case IA_RM16:
						rmStr = "DI";
						break;
					case IA_RM32:
						rmStr = "EDI";
						break;
					case IA_MM:
						rmStr = "MM7";
						break;
					case IA_XMM:
						rmStr = "XMM7";
						break;
				}

				// Dst Next
				switch(instTable->dstOperand){
					case IA_RM8:
						rmStr = "BH";
						break;
					case IA_RM16:
						rmStr = "DI";
						break;
					case IA_RM32:
						rmStr = "EDI";
						break;
					case IA_MM:
						rmStr = "MM7";
						break;
					case IA_XMM:
						rmStr = "XMM7";
						break;
				}

				break;
		}

		goto end;
	}

	end:
	if(rmStr == NULL){
		printf("\n rm = %s", rmStr);
	} else {
		printf("\n rm = ??");
	}

	return NULL;
}

// Creates an empty instruction element
P_X86_32_DECODED_INSTRUCTION_LIST X86_32_CreateInstructionElement(
		       P_X86_32_DECODED_INSTRUCTION_LIST first,
		       P_X86_32_DECODED_INSTRUCTION_LIST prev,
		       P_X86_32_DECODED_INSTRUCTION_LIST next){

	P_X86_32_DECODED_INSTRUCTION_LIST pInstruction = (P_X86_32_DECODED_INSTRUCTION_LIST) bpmalloc(sizeof(X86_32_DECODED_INSTRUCTION_LIST));

	if(pInstruction != NULL){
		memset(pInstruction, 0x00, sizeof(X86_32_DECODED_INSTRUCTION_LIST));
	} else {
		return NULL;
	}

	// Set up pointers
	if(first != NULL)
		pInstruction->first = first;

	if(prev != NULL)
		pInstruction->prev = prev;

	if(next != NULL)
		pInstruction->next = next;

	return pInstruction;
}


// Destroys an instruction list, bpfreeing all subcomponents
int X86_32_DestroyInstructionList(P_X86_32_DECODED_INSTRUCTION_LIST destroyList){

	// Argument checks
	if(destroyList == NULL)
		return ERR_FAILURE;

	if(destroyList->first == NULL)
		return ERR_FAILURE;

	P_X86_32_DECODED_INSTRUCTION_LIST nextList = NULL;

	// Counts prefixes
	unsigned char x = 0;

	// Loop through the list and bpfree elements
	for(destroyList = destroyList->first;
		destroyList != NULL; ){

		// Set next
		nextList = destroyList->next;

		// Destroy decoded modrm REG string (INTEL)
		if(destroyList->regStr != NULL){
			bpfree(destroyList->regStr);
			destroyList->regStr = NULL;
		}

		// Destroy decoded rm REG string (INTEL)
		if(destroyList->rmStr != NULL){
			bpfree(destroyList->rmStr);
			destroyList->rmStr = NULL;
		}

		// Destroy decoded instruction string (INTEL)
		if(destroyList->intelDecode != NULL){
			bpfree(destroyList->intelDecode);
			destroyList->intelDecode = NULL;
		}

		// Go through prefix array and destroy strings if
		// found.
		for(x = 0; x < destroyList->prefixCount; x++){
			if(destroyList->prefixes[x].prefixStr != NULL)
				bpfree(destroyList->prefixes[x].prefixStr);
		}

		bpfree(destroyList);


		// Check if next is null, if it is, exit the loop.
		if(destroyList != NULL)
			destroyList = nextList;
		else
			break;

	}

	return ERR_SUCCESS;
}


// ------------------------------------------------------
// ModR/M Decoder functions

// Retrieve MOD Value From Byte 00111111 binary
unsigned char IA32_MODRM_GET_MOD(unsigned char i){
	// shift one way to isolate
	i = i >> 6;

	// shift back to properly represent
	i = i << 6;

	return i;
}

// Retrieve RM Value From Byte 11000111 binary
unsigned char IA32_MODRM_GET_REG(unsigned char i){

	// shift into bit isolation
	i = i << 2;
	i = i >> 5;
	i = i << 3;

	return i;

}

// Retrieve Digit/Opcode Value From Byte
unsigned char IA32_MODRM_GET_RM(unsigned char i){
	// shift into bit isolation
	i = i << 5;
	i = i >> 5;
	return i;
}

// Create Mod Rm from 3 values (untested, should work)
unsigned char IA32_MODRM_CREATE(int i){

	return 0x00;
}

// Macros:
// Retrieve Scale
unsigned char IA32_SIB_GET_SCALE(int i){
	// (i & 00111111b)

	// shift into bit isolation
	unsigned char x = i >> 6;
	x = x << 6;
	return x;
}

// Retrieve Index
unsigned char IA32_SIB_GET_INDEX(int i){
	// (i & 11000111b)
	unsigned char x = i << 2;
	x = x >> 5;
	i = x << 3;
	x = i;
	return x;
}

// Retrieve Base
unsigned char IA32_SIB_GET_BASE(int i){
	// (i & 11111000b)
	// shift into bit isolation
	unsigned char x = i << 5;
	x = x >> 5;
	return x;
}


// Tests an instruction to see if it contains a size override
int X86_32_ContainsPrefixOpSizeOverride(P_X86_32_INSTRUCTION instTable,
											P_X86_32_DECODED_INSTRUCTION_LIST inList){

	// Loop through the prefix to find operand override
	for(size_t i = 0; i < instTable->binPrefixLen; i++){
		if(instTable->binPrefix[i] == 0x66){
			return true;
		}
	}

	return false;
}

// Tests an instruction to see if it contains a address size override
int X86_32_ContainsPrefixAddrSizeOverride(P_X86_32_INSTRUCTION instTable,
											P_X86_32_DECODED_INSTRUCTION_LIST inList){

	return 0;
}


// Decodes REG field of modrm byte in partialDecode.  It templates the register
// type by observing the operand passed.  This is for the reg string only.
char * X86_32_GetRegString(P_X86_32_DECODED_INSTRUCTION_LIST partialDecode, size_t operand){

	if(partialDecode == NULL)
		return NULL;

	// initial switch to determine if its a static register
	switch(operand){
		// normal registers
		case IA_EAX:
			return strdup("EAX");
		case IA_AX:
			return strdup("AX");
		case IA_CL:
			return strdup("CL");
		case IA_DX:
			return strdup("DX");

		// segment registers
		case IA_CS:
			return strdup("CS");
		case IA_DS:
			return strdup("DS");
		case IA_ES:
			return strdup("ES");
		case IA_FS:
			return strdup("FS");
		case IA_SS:
			return strdup("SS");

		// control registers
		case IA_CR_0:
			return strdup("cr0");
		case IA_CR_1:
			return strdup("cr1");
		case IA_CR_2:
			return strdup("cr2");
		case IA_CR_3:
			return strdup("cr3");
		case IA_CR_4:
			return strdup("cr4");
		case IA_CR_5:
			return strdup("cr5");
		case IA_CR_6:
			return strdup("cr6");
		case IA_CR_7:
			return strdup("cr7");

	}

	// get /digit first , will represent what register family the
	// register is coming from.
	size_t slashDigit = X86_32_GetSlashDigitFromRm(partialDecode->modRM);
	size_t regClass = X86_32_GetRmRegTypeClass(operand);


	// See notes in first case structure for switch operational details.
	switch(slashDigit){

		case 0:

			if(regClass == TYPECLASS_REG_GPR_8)
				return strdup("AL");

			// check for modrm prefix override, the override directly
			// affects the size of the register.  In 32bit mode it will
			// cause a 16bit operand to be returned.  32bit is used as
			// a default size.  This general logic is followed through the
			// rest of this switches constructs.
			if(partialDecode->operSizeOverride > 0){
				if(regClass == TYPECLASS_REG_GPR_16 || regClass == TYPECLASS_REG_GPR_32)
					return strdup("AX");
			} else if(regClass == TYPECLASS_REG_GPR_16 || regClass == TYPECLASS_REG_GPR_32) {
				return strdup("EAX");
			}

			// Return remaining register types.
			if(regClass == TYPECLASS_REG_GPR_64)
				return strdup("RAX");
			if(regClass == TYPECLASS_REG_MM)
				return strdup("MM0");
			if(regClass == TYPECLASS_REG_XMM)
				return strdup("XMM0");
			break;

		// Decode ECX
		case 1:
			if(regClass == TYPECLASS_REG_GPR_8)
				return strdup("CL");

			if(partialDecode->operSizeOverride > 0){
				if(regClass == TYPECLASS_REG_GPR_16 || regClass == TYPECLASS_REG_GPR_32)
					return strdup("CX");
			} else if(regClass == TYPECLASS_REG_GPR_16 || regClass == TYPECLASS_REG_GPR_32) {
				return strdup("ECX");
			}

			if(regClass == TYPECLASS_REG_GPR_64)
				return strdup("RCX");
			if(regClass == TYPECLASS_REG_MM)
				return strdup("MM1");
			if(regClass == TYPECLASS_REG_XMM)
				return strdup("XMM1");

		// Decode EDX
		case 2:
			if(regClass == TYPECLASS_REG_GPR_8)
				return strdup("DL");

			if(partialDecode->operSizeOverride > 0){
				if(regClass == TYPECLASS_REG_GPR_16 || regClass == TYPECLASS_REG_GPR_32)
					return strdup("DX");
			} else if(regClass == TYPECLASS_REG_GPR_16 || regClass == TYPECLASS_REG_GPR_32) {
				return strdup("EDX");
			}

			if(regClass == TYPECLASS_REG_GPR_64)
				return strdup("RDX");
			if(regClass == TYPECLASS_REG_MM)
				return strdup("MM2");
			if(regClass == TYPECLASS_REG_XMM)
				return strdup("XMM2");

		// Decode EBX
		case 3:
			if(regClass == TYPECLASS_REG_GPR_8)
				return strdup("BL");

			if(partialDecode->operSizeOverride > 0){
				if(regClass == TYPECLASS_REG_GPR_16 || regClass == TYPECLASS_REG_GPR_32)
					return strdup("BX");
			} else if(regClass == TYPECLASS_REG_GPR_16 || regClass == TYPECLASS_REG_GPR_32) {
				return strdup("EBX");
			}

			if(regClass == TYPECLASS_REG_GPR_64)
				return strdup("RBX");
			if(regClass == TYPECLASS_REG_MM)
				return strdup("MM3");
			if(regClass == TYPECLASS_REG_XMM)
				return strdup("XMM3");

		// Decode ESP
		case 4:
			if(regClass == TYPECLASS_REG_GPR_8)
				return strdup("AH");

			if(partialDecode->operSizeOverride > 0){
				if(regClass == TYPECLASS_REG_GPR_16 || regClass == TYPECLASS_REG_GPR_32)
					return strdup("SP");
			} else if(regClass == TYPECLASS_REG_GPR_16 || regClass == TYPECLASS_REG_GPR_32) {
				return strdup("ESP");
			}

			if(regClass == TYPECLASS_REG_GPR_64)
				return strdup("RSP"); // dunno if this is a real register, doesn't hurt
			if(regClass == TYPECLASS_REG_MM)
				return strdup("MM4");
			if(regClass == TYPECLASS_REG_XMM)
				return strdup("XMM4");

		// Decode EBP
		case 5:
			if(regClass == TYPECLASS_REG_GPR_8)
				return strdup("CH");

			if(partialDecode->operSizeOverride > 0){
				if(regClass == TYPECLASS_REG_GPR_16 || regClass == TYPECLASS_REG_GPR_32)
					return strdup("BP");
			} else if(regClass == TYPECLASS_REG_GPR_16 || regClass == TYPECLASS_REG_GPR_32) {
				return strdup("EBP");
			}

			if(regClass == TYPECLASS_REG_GPR_64)
				return strdup("RBP"); // dunno if this is a real register, doesn't hurt
			if(regClass == TYPECLASS_REG_MM)
				return strdup("MM5");
			if(regClass == TYPECLASS_REG_XMM)
				return strdup("XMM5");

		// Decode ESI
		case 6:
			if(regClass == TYPECLASS_REG_GPR_8)
				return strdup("DH");

			if(partialDecode->operSizeOverride > 0){
				if(regClass == TYPECLASS_REG_GPR_16 || regClass == TYPECLASS_REG_GPR_32)
					return strdup("SI");
			} else if(regClass == TYPECLASS_REG_GPR_16 || regClass == TYPECLASS_REG_GPR_32) {
				return strdup("ESI");
			}

			if(regClass == TYPECLASS_REG_GPR_64)
				return strdup("RSI"); // dunno if this is a real register, doesn't hurt
			if(regClass == TYPECLASS_REG_MM)
				return strdup("MM6");
			if(regClass == TYPECLASS_REG_XMM)
				return strdup("XMM6");

		case 7:
			if(regClass == TYPECLASS_REG_GPR_8)
				return strdup("BH");

			if(partialDecode->operSizeOverride > 0){
				if(regClass == TYPECLASS_REG_GPR_16 || regClass == TYPECLASS_REG_GPR_32)
					return strdup("DI");
			} else if(regClass == TYPECLASS_REG_GPR_16 || regClass == TYPECLASS_REG_GPR_32) {
				return strdup("EDI");
			}

			if(regClass == TYPECLASS_REG_GPR_64)
				return strdup("RDI"); // dunno if this is a real register, doesn't hurt
			if(regClass == TYPECLASS_REG_MM)
				return strdup("MM7");
			if(regClass == TYPECLASS_REG_XMM)
				return strdup("XMM7");
			break;

		default:
			break;
	}

	//--  here

	return NULL;
}


// Decodes The RM value as if it were a register [mod 11]
char * X86_32_GetRmRegString(P_X86_32_DECODED_INSTRUCTION_LIST partialDecode, size_t operand){

	if(partialDecode == NULL)
		return NULL;

	// get /digit first , will represent what register family the
	// register is coming from.
	size_t rm = IA32_MODRM_GET_RM(partialDecode->modRM);

	// See notes in first case structure for switch operational details.
	switch(rm){

		// EAX Series
		case 0x0:
			if(operand == IA_RM8)
				return strdup("AL");

			if(partialDecode->operSizeOverride > 0){
				if(operand == IA_RM16 || operand == IA_R16_M16 || operand == IA_R32_M16)
					return strdup("AX");
			} else if(operand == IA_RM16 || operand == IA_R16_M16 || operand == IA_R32_M16 || operand == IA_RM32){
					return strdup("EAX");
			}

			if(operand == IA_RM64)
				return strdup("RAX");

			if(operand == IA_MM_M32 || operand == IA_MM_M64)
				return strdup("MM0");

			if(operand == IA_XMM_M32 || operand == IA_XMM_M64 || operand == IA_XMM_M128)
				return strdup("XMM0");

		// ECX Decode
		case 0x1:
			if(operand == IA_RM8)
				return strdup("CL");

			if(partialDecode->operSizeOverride > 0){
				if(operand == IA_RM16 || operand == IA_R16_M16 || operand == IA_R32_M16)
					return strdup("CX");
			} else if(operand == IA_RM16 || operand == IA_R16_M16 || operand == IA_R32_M16 || operand == IA_RM32){
					return strdup("ECX");
			}

			if(operand == IA_RM64)
				return strdup("RCX");

			if(operand == IA_MM_M32 || operand == IA_MM_M64)
				return strdup("MM1");

			if(operand == IA_XMM_M32 || operand == IA_XMM_M64 || operand == IA_XMM_M128)
				return strdup("XMM1");

		case 0x2:
			if(operand == IA_RM8)
				return strdup("DL");

			if(partialDecode->operSizeOverride > 0){
				if(operand == IA_RM16 || operand == IA_R16_M16 || operand == IA_R32_M16)
					return strdup("DX");
			} else if(operand == IA_RM16 || operand == IA_R16_M16 || operand == IA_R32_M16 || operand == IA_RM32){
					return strdup("EDX");
			}

			if(operand == IA_RM64)
				return strdup("RDX");

			if(operand == IA_MM_M32 || operand == IA_MM_M64)
				return strdup("MM2");

			if(operand == IA_XMM_M32 || operand == IA_XMM_M64 || operand == IA_XMM_M128)
				return strdup("XMM2");

		// EBX Decode
		case 0x3:
			if(operand == IA_RM8)
				return strdup("BL");

			if(partialDecode->operSizeOverride > 0){
				if(operand == IA_RM16 || operand == IA_R16_M16 || operand == IA_R32_M16)
					return strdup("BX");
			} else if(operand == IA_RM16 || operand == IA_R16_M16 || operand == IA_R32_M16 || operand == IA_RM32){
					return strdup("EBX");
			}

			if(operand == IA_RM64)
				return strdup("RBX");

			if(operand == IA_MM_M32 || operand == IA_MM_M64)
				return strdup("MM3");

			if(operand == IA_XMM_M32 || operand == IA_XMM_M64 || operand == IA_XMM_M128)
				return strdup("XMM3");


			// ESP Decode
			case 0x4:
				if(operand == IA_RM8)
					return strdup("AH");

				if(partialDecode->operSizeOverride > 0){
					if(operand == IA_RM16 || operand == IA_R16_M16 || operand == IA_R32_M16)
						return strdup("SP");
				} else if(operand == IA_RM16 || operand == IA_R16_M16 || operand == IA_R32_M16 || operand == IA_RM32){
						return strdup("ESP");
				}

				if(operand == IA_RM64)
					return strdup("RSP");

				if(operand == IA_MM_M32 || operand == IA_MM_M64)
					return strdup("MM4");

				if(operand == IA_XMM_M32 || operand == IA_XMM_M64 || operand == IA_XMM_M128)
					return strdup("XMM4");

				// EBX Decode
				case 0x5:
					if(operand == IA_RM8)
						return strdup("CH");

					if(partialDecode->operSizeOverride > 0){
						if(operand == IA_RM16 || operand == IA_R16_M16 || operand == IA_R32_M16)
							return strdup("BP");
					} else if(operand == IA_RM16 || operand == IA_R16_M16 || operand == IA_R32_M16 || operand == IA_RM32){
							return strdup("EBP");
					}

					if(operand == IA_RM64)
						return strdup("RBP");

					if(operand == IA_MM_M32 || operand == IA_MM_M64)
						return strdup("MM5");

					if(operand == IA_XMM_M32 || operand == IA_XMM_M64 || operand == IA_XMM_M128)
						return strdup("XMM5");

				// ESI Decode
				case 0x6:
					if(operand == IA_RM8)
						return strdup("DH");

					if(partialDecode->operSizeOverride > 0){
						if(operand == IA_RM16 || operand == IA_R16_M16 || operand == IA_R32_M16)
							return strdup("SI");
					} else if(operand == IA_RM16 || operand == IA_R16_M16 || operand == IA_R32_M16 || operand == IA_RM32){
							return strdup("ESI");
					}

					if(operand == IA_RM64)
						return strdup("RSI");

					if(operand == IA_MM_M32 || operand == IA_MM_M64)
						return strdup("MM6");

					if(operand == IA_XMM_M32 || operand == IA_XMM_M64 || operand == IA_XMM_M128)
						return strdup("XMM6");

				// EDI Decode
				case 0x7:
					if(operand == IA_RM8)
						return strdup("BH");

					if(partialDecode->operSizeOverride > 0){
						if(operand == IA_RM16 || operand == IA_R16_M16 || operand == IA_R32_M16)
							return strdup("DI");
					} else if(operand == IA_RM16 || operand == IA_R16_M16 || operand == IA_R32_M16 || operand == IA_RM32){
							return strdup("EDI");
					}

					if(operand == IA_RM64)
						return strdup("RDI");

					if(operand == IA_MM_M32 || operand == IA_MM_M64)
						return strdup("MM7");

					if(operand == IA_XMM_M32 || operand == IA_XMM_M64 || operand == IA_XMM_M128)
						return strdup("XMM7");


		default:
			break;

	}

	return NULL;
}

// Retreives a SIB encoded string reflecting an encoded rm value. Operand
// is un-needed because operands are 32 bit only in SIB encodes.
char * X86_32_GetRmSibString(P_X86_32_DECODED_INSTRUCTION_LIST partialDecode){

	// Retrieve Scale Index And Base
	size_t scale = IA32_SIB_GET_SCALE(partialDecode->sib);
	size_t index = IA32_SIB_GET_INDEX(partialDecode->sib);
	size_t base  = IA32_SIB_GET_BASE (partialDecode->sib);

	// Could Probably Be A Smaller Value
	char sibStr[200];
	memset(sibStr, 0x00, 200);


	switch(base){
		case 0x00:
			strncat(sibStr, "[EAX", 5);
			break;
		case 0x01:
			strncat(sibStr, "[ECX", 5);
				break;
		case 0x02:
			strncat(sibStr, "[EDX", 5);
				break;
		case 0x03:
			strncat(sibStr, "[EBX", 5);
				break;
		case 0x04:
			strncat(sibStr, "[ESP", 5);
				break;

		// Contains none
		case 0x05:
			// strncat(sibStr, "[", 1);
			// displacement is always 32bits, must be
			// set before function entrancy though.
			// sprintf(&sibStr, "[%.08x", )

			break;

		case 0x06:
			strncat(sibStr, "[ESI", 5);
				break;
		case 0x07:
			strncat(sibStr, "[EDI", 5);
				break;
		default:
			break;
	}

	// Add the obligitary + symbol if the sib
	// base is not empty.  If it is omit the "+"
	if(strlen(sibStr) != 1){
		strncat(sibStr, " + ", 3);
	}

	switch(scale){
		//
		case 0x00:
			switch(index){
				case 0x00:
					strncat(sibStr, "EAX]", 7);
					return bpstrndup(sibStr, 100);
				case 0x08:
					strncat(sibStr, "ECX]", 7);
					return bpstrndup(sibStr, 100);
				case 0x10:
					strncat(sibStr, "EDX]", 7);
					return bpstrndup(sibStr, 100);
				case 0x18:
					strncat(sibStr, "EBX]", 7);
					return bpstrndup(sibStr, 100);
				// Empty Set
				case 0x20:
					memset(&sibStr[strlen(sibStr)-3], 0x00, 3);
					strncat(sibStr, "]", 7);
					return bpstrndup(sibStr, 100);
				case 0x28:
					strncat(sibStr, "EBP]", 7);
					return bpstrndup(sibStr, 100);
				case 0x30:
					strncat(sibStr, "ESI]", 7);
					return bpstrndup(sibStr, 100);
				case 0x38:
					strncat(sibStr, "EDI]", 7);
					return bpstrndup(sibStr, 100);
			}
			break;

		// Scale Row 1
		case 0x40:
			switch(index){
				case 0x00:
					strncat(sibStr, "EAX * 2]", 15);
					return bpstrndup(sibStr, 100);
				case 0x08:
					strncat(sibStr, "ECX * 2]", 15);
					return bpstrndup(sibStr, 100);
				case 0x10:
					strncat(sibStr, "EDX * 2]", 15);
					return bpstrndup(sibStr, 100);
				case 0x18:
					strncat(sibStr, "EBX * 2]", 15);
					return bpstrndup(sibStr, 100);
				// Empty Set
				case 0x20:
					memset(&sibStr[strlen(sibStr)-3], 0x00, 3);
					strncat(sibStr, "]", 7);
					return bpstrndup(sibStr, 100);
				case 0x28:
					strncat(sibStr, " EBP * 2]", 15);
					return bpstrndup(sibStr, 100);
				case 0x30:
					strncat(sibStr, " ESI * 2]", 15);
					return bpstrndup(sibStr, 100);
				case 0x38:
					strncat(sibStr, "EDI * 2]", 15);
					return bpstrndup(sibStr, 100);
			}
			break;

		// Scale row 0x2
		case 0x80:
			switch(index){
				case 0x00:
					strncat(sibStr, "EAX * 4]", 15);
					return bpstrndup(sibStr, 100);
				case 0x08:
					strncat(sibStr, "ECX * 4]", 15);
					return bpstrndup(sibStr, 100);
				case 0x10:
					strncat(sibStr, "EDX * 4]", 15);
					return bpstrndup(sibStr, 100);
				case 0x18:
					strncat(sibStr, "EBX * 4]", 15);
					return bpstrndup(sibStr, 100);
				// Empty Set
				case 0x20:
					memset(&sibStr[strlen(sibStr)-3], 0x00, 3);
					strncat(sibStr, "]", 7);
					return bpstrndup(sibStr, 100);
				case 0x28:
					strncat(sibStr, "EBP * 4]", 15);
					return bpstrndup(sibStr, 100);
				case 0x30:
					strncat(sibStr, "ESI * 4]", 15);
					return bpstrndup(sibStr, 100);
				case 0x38:
					strncat(sibStr, "EDI * 4]", 15);
					return bpstrndup(sibStr, 100);
			}
			break;
		case 0xc0:
			switch(index){
				case 0x00:
					strncat(sibStr, "EAX * 8]", 15);
					return bpstrndup(sibStr, 100);
				case 0x08:
					strncat(sibStr, "ECX * 8]", 15);
					return bpstrndup(sibStr, 100);
				case 0x10:
					strncat(sibStr, "EDX * 8]", 15);
					return bpstrndup(sibStr, 100);
				case 0x18:
					strncat(sibStr, "EBX * 8]", 15);
					return bpstrndup(sibStr, 100);
				// Empty Set
				case 0x20:
					memset(&sibStr[strlen(sibStr)-3], 0x00, 3);
					strncat(sibStr, "]", 1);
					return bpstrndup(sibStr, 100);
				case 0x28:
					strncat(sibStr, "EBP * 8]", 15);
					return bpstrndup(sibStr, 100);
				case 0x30:
					strncat(sibStr, "ESI * 8]", 15);
					return bpstrndup(sibStr, 100);
				case 0x38:
					strncat(sibStr, "EDI * 8]", 15);
					return bpstrndup(sibStr, 100);
			}
			break;


		default:
			break;
	}

	// add displacement here (sigh)
	return NULL;
}


// Retrieves a register string for plus encoded instructions given
char * X86_32_GetPlusEncodedRegString(P_X86_32_DECODED_INSTRUCTION_LIST partialDecoded, unsigned char *inBuff){

	// if the instruction is plusbase encoded
	// set instruction and jump to end label.
	unsigned char *binOpcode = (unsigned char *) partialDecoded->instruction->binOpcode;
	size_t encodedOpcode = X86_32_IsBytePlusEncodedOpcode((unsigned char *) binOpcode);
	size_t regField = 0;

	// test the plus base encoded opcode
	if(encodedOpcode != 0){

		// BSWAP + rd check (requires first byte as simd 0f prefix)
		if(binOpcode[0] == 0x0f)
		if(binOpcode[1] >= 0xC8 && binOpcode[1] < (0xc8+8)){
			// advance and jump to main decoder
			binOpcode = &binOpcode[1];
			goto decode;
			//return 0xC8;
		}

		// check all 1 byte plus encoded values and return string
		if(binOpcode[0] >= 0xb0 && binOpcode[0] < (0xb0+8) ||
		   binOpcode[0] >= 0xb8 && binOpcode[0] < (0xb8+8) ||
		   binOpcode[0] >= 0x48 && binOpcode[0] < (0x48+8) ||
		   binOpcode[0] >= 0x40 && binOpcode[0] < (0x40+8) ||
		   binOpcode[0] >= 0x58 && binOpcode[0] < (0x58+8) ||
		   binOpcode[0] >= 0x50 && binOpcode[0] < (0x50+8) ||
		   binOpcode[0] >= 0x90 && binOpcode[0] < (0x90+8) ){

			// arbitrary jump for 0xf0 encoded instruction
			decode:

			regField = inBuff[partialDecoded->totalLength -
							partialDecoded->instruction->binOpcodeLen] - encodedOpcode ;

			switch(regField){
				case 0x0:
					if(partialDecoded->operSizeOverride > 0){
						return strdup("AX");
					} else {
						return strdup("EAX");
					}
					goto end;
					break;
				case 0x01:
					if(partialDecoded->operSizeOverride > 0){
						return strdup("CX");
					} else {
						return strdup("ECX");
					}
					goto end;
					break;
				case 0x02:
					if(partialDecoded->operSizeOverride > 0){
						return strdup("DX");
					} else {
						return strdup("EDX");
					}
					goto end;
					break;
				case 0x03:
					if(partialDecoded->operSizeOverride > 0){
						return strdup("BX");
					} else {
						return strdup("EBX");
					}
					goto end;
					break;

				case 0x04:
					if(partialDecoded->operSizeOverride > 0){
						return strdup("SP");
					} else {
						return strdup("ESP");
					}
					goto end;
					break;

				case 0x05:
					if(partialDecoded->operSizeOverride > 0){
						return strdup("BP");
					} else {
						return strdup("EBP");
					}
					goto end;
					break;

				case 0x06:
					if(partialDecoded->operSizeOverride > 0){
						return strdup("SI");
					} else {
						return strdup("ESI");
					}
					goto end;
					break;

				case 0x07:
					if(partialDecoded->operSizeOverride > 0){
						return strdup("DI");
					} else {
						return strdup("EDI");
					}
					goto end;
					break;

				default:
					goto end;
					break;
			}
		}






		// goto end;
	}


	end:

	return NULL;
}



// Looks at the register/memory value in a decoded list
// and sets a string indicating what it actually reflects
char * X86_32_GetRmString( P_X86_32_DECODED_INSTRUCTION_LIST partialDecode, size_t operand ){

	if(partialDecode == NULL)
		return NULL;

	// First for the rm decode, we will need to check the base address size
	// mode
	if(partialDecode->addrSizeOverride == 0){

		// will issue a jump to the 32 bit decoding section when in 32
		// bit mode.
		goto default32;
	} else {

		// Default to 16bit register analysis
		goto prefix16;
	}

	/*
	 * Functional R/M Decoder Logic:
	 *
	 * MOD ANALYSIS:
	 * First approach the mod bits, to determine the 0-3 (00-01)
	 * of the MODR/M bits.
	 *
	 * SIB OCCURANCES:
	 * Second, test the r/m bits to determine a row offset.
	 * 		If the row offset lands on a SIB byte.  Immediately enter SIB
	 * 		decoding.  Displacements must be considered here if SIB encoded.
	 * 		Exit function here.
	 *
	 * NORMAL RM DECODE:
	 * (need 32Bit/16bit/Sib versions of disp type regtrieval)
	 * Third, if the rm contains no SIB byte check for rm displacement ammount.
	 * 		Add displacement to byte string in INTEL syntax format.
	 *
	 */



	// 16bit override is present, jumps to exit at end of large
	// switch nest.
	prefix16:
	switch( IA32_MODRM_GET_MOD(partialDecode->modRM)){
		// 32bit Mod row 0
		case 0x00:
			// MODRM32: First Row (0bit default disp)
			switch(IA32_MODRM_GET_RM(partialDecode->modRM)){
				case 0x00:
					return strdup("[BX+SI]");
				case 0x01:
					return strdup("[BX+DI]");
				case 0x02:
					return strdup("[BP+SI]");
				case 0x03:
					return strdup("[BP+DI]");
				case 0x4:
					return strdup("[SI]");

				// encode displacement and return string
				case 0x05:
					return strdup("[DI]");

				case 0x06:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x16;
					return strdup("DISP16:");
				case 0x07:
					return strdup("[BX]");
			}
			break;

		// 32bit Mod row 1
		case 0x40:
			// MODRM32: Second Row (8bit default disp)
			switch(IA32_MODRM_GET_RM(partialDecode->modRM)){

				// EAX + 8bit Disp
				case 0x00:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x8;
					return strdup("[BX+SI]");

				// ECX + 8bit Disp
				case 0x01:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x8;
					return strdup("[BX+DI]");

				// EDX + 8bit Disp
				case 0x02:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x8;
					return strdup("[BP+SI]");

				// EBX + 8bit Disp
				case 0x03:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x8;
					return strdup("[BP+DI]");

				// SIB Encoded + 8bit Disp
				case 0x4:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x8;
					return strdup("[SI]");

				// encode displacement and return string
				// EBP + 8bit Disp
				case 0x05:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x8;
					return strdup("[DI]");

				// ESI + 8bit Disp
				case 0x06:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x8;
					return strdup("[BP]");

				// EDI + 8bit Disp
				case 0x07:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x8;
					return strdup("[BX]");
			}

			break;


		// 32bit Mod row 2
		case 0x80:
			// MODRM32: Second Row (32bit default disp)
			switch(IA32_MODRM_GET_RM(partialDecode->modRM)){

				// EAX + 32bit Disp
				case 0x00:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x16;
					return strdup("[BX+SI]");

				// ECX + 32bit Disp
				case 0x01:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x16;
					return strdup("[BX+DI]");

				// EDX + 32bit Disp
				case 0x02:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x16;
					return strdup("[BP+SI]");

				// EBX + 32bit Disp
				case 0x03:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x16;
					return strdup("[BP+DI]");

				// SIB Encoded + 32bit Disp
				case 0x4:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x16;
					return strdup("[SI]");

				// encode displacement and return string
				// EBP + 32bit Disp
				case 0x05:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x16;
					return strdup("[DI]");

				// ESI + 32bit Disp
				case 0x06:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x16;
					return strdup("[BP]");

				// EDI + 32bit Disp
				case 0x07:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x16;
					return strdup("[BX]");
			}
			break;

		// 32bit Mod row 3 [ This requires GetRegString clone that uses
		// the getRegStr template with r/m row template instead of the
		// slashdigit. (basically swap slashdigit with modrm binary val)
		case 0xc0:
			return X86_32_GetRmRegString(partialDecode, operand);
			break;

		default:
			break;
	}

	// Default 16 bit exit point
	goto end;



	// 32bit override is present, exits at end.
	default32:
	switch( IA32_MODRM_GET_MOD(partialDecode->modRM)){
		// 32bit Mod row 0
		case 0x00:
			// MODRM32: First Row (0bit default disp)
			switch(IA32_MODRM_GET_RM(partialDecode->modRM)){
				case 0x00:
					return strdup("[EAX]");
				case 0x01:
					return strdup("[ECX]");
				case 0x02:
					return strdup("[EDX]");
				case 0x03:
					return strdup("[EBX]");

				// SIB Encoded
				case 0x4:
					partialDecode->containsSib=true;
					return NULL;

				// encode displacement and return string
				case 0x05:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x32;
					// return strdup("DISP32:");
					return NULL;

				case 0x06:
					return strdup("[ESI]");
				case 0x07:
					return strdup("[EDI]");
			}
			break;

		// 32bit Mod row 1
		case 0x40:
			// MODRM32: Second Row (8bit default disp)
			switch(IA32_MODRM_GET_RM(partialDecode->modRM)){

				// EAX + 8bit Disp
				case 0x00:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x8;
					return strdup("[EAX]");

				// ECX + 8bit Disp
				case 0x01:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x8;
					return strdup("[ECX]");

				// EDX + 8bit Disp
				case 0x02:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x8;
					return strdup("[EDX]");

				// EBX + 8bit Disp
				case 0x03:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x8;
					return strdup("[EBX]");

				// SIB Encoded + 8bit Disp
				case 0x4:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x8;
					partialDecode->containsSib=true;
					return NULL;

				// encode displacement and return string
				// EBP + 8bit Disp
				case 0x05:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x8;
					return strdup("[EBP]");

				// ESI + 8bit Disp
				case 0x06:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x8;
					return strdup("[ESI]");

				// EDI + 8bit Disp
				case 0x07:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x8;
					return strdup("[EDI]");
			}

			break;


		// 32bit Mod row 2
		case 0x80:
			// MODRM32: Second Row (32bit default disp)
			switch(IA32_MODRM_GET_RM(partialDecode->modRM)){

				// EAX + 32bit Disp
				case 0x00:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x32;
					return strdup("[EAX]");

				// ECX + 32bit Disp
				case 0x01:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x32;
					return strdup("[ECX]");

				// EDX + 32bit Disp
				case 0x02:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x32;
					return strdup("[EDX]");

				// EBX + 32bit Disp
				case 0x03:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x32;
					return strdup("[EBX]");

				// SIB Encoded + 32bit Disp
				case 0x4:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x32;
					partialDecode->containsSib=true;
					return NULL;

				// encode displacement and return string
				// EBP + 32bit Disp
				case 0x05:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x32;
					return strdup("[EBP]");

				// ESI + 32bit Disp
				case 0x06:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x32;
					return strdup("[ESI]");

				// EDI + 32bit Disp
				case 0x07:
					partialDecode->containsDisp = true;
					partialDecode->dispSize = 0x32;
					return strdup("[EDI]");
			}


			break;

		// 32bit Mod row 3 [ This requires GetRegString clone that uses
		// the getRegStr template with r/m row template instead of the
		// slashdigit. (basically swap slashdigit with modrm binary val)
		case 0xc0:
			return X86_32_GetRmRegString(partialDecode, operand);
			break;

		default:
			break;
	}

	// Function Exit Point, jump from 16 bit only.
	end:

	return NULL;
}

// This will literally decode an instruction, buff cannot be larger then
// 15 bytes as this is the maximum size of an instruction.
P_X86_32_DECODED_INSTRUCTION_LIST X86_32_DecodeInstruction(P_X86_32_INSTRUCTION instTable, void *buff, size_t length){

	if(length == 0)
		return NULL;
	if(length > 15)
		return NULL;

	if(instTable == NULL){
		return NULL;
	}


	// save pointer
	unsigned char *inBuff = (unsigned char *) buff;

	// Create decoded instruction structure and search prefixes in same call
	P_X86_32_DECODED_INSTRUCTION decodedInstruction = X86_32_DecodePrefixes(buff);

	// Retrieve Instruction, use partially decoded instruction to retrive instruction pointer
	// full length should be the #of prefixes + sizeof opcode at this point.  There
	// will be no
	decodedInstruction->instruction = X86_32_DecodeOpcode(instTable, decodedInstruction, buff);
	if(decodedInstruction->instruction == NULL){
		// printf("\n Found Bad Instruction: %.02x", inBuff[0]);
		return NULL;
	}



	// set length next: this is a partial set that does not factor in sib/disp/imm
	// values.
	decodedInstruction->totalLength = decodedInstruction->instruction->binOpcodeLen +
										decodedInstruction->prefixCount;


	// Retrieves plus encoded value of plus encoded instruction
	if(X86_32_IsBytePlusEncodedOpcode((unsigned char *) decodedInstruction->instruction->binOpcode)){
		decodedInstruction->regStr = X86_32_GetPlusEncodedRegString(decodedInstruction, &inBuff[decodedInstruction->prefixCount]);
		goto plusEncodedJump;
	}

	// Tests if the dest operand is a pointer instruction
	if(X86_32_IsOperandPointer(decodedInstruction->instruction->dstOperand)){

		// set pointer flag
		decodedInstruction->containsPtr = true;

		// 16bit assignment
		if(decodedInstruction->addrSizeOverride > 0 ||
		   decodedInstruction->operSizeOverride > 0){

			// copy over 2 bytes
			memcpy(&decodedInstruction->ptrVal, &inBuff[decodedInstruction->totalLength], 2);
				decodedInstruction->totalLength+=2;

			memcpy(&decodedInstruction->ptrDispVal, &inBuff[decodedInstruction->totalLength], 2);
				decodedInstruction->ptrSize = 0x16;
				decodedInstruction->totalLength+=2;

		// 32bit assignment
		} else {
			memcpy(&decodedInstruction->ptrVal, &inBuff[decodedInstruction->totalLength],42);
				decodedInstruction->totalLength+=4;
			memcpy(&decodedInstruction->ptrDispVal, &inBuff[decodedInstruction->totalLength], 2);
				decodedInstruction->ptrSize = 0x32;
				decodedInstruction->totalLength+=2;
		}


		goto end;
	}

	// test and verify that the instruction isn't langer then max size
	if(decodedInstruction->totalLength > 15)
		return NULL;

	// What needs to be done.  Determine if an instruction
	// parameter is a static parameter.

	// If the partially decoded instruction contains a a mod rm byte
	// evaluate both the reg and rm values.
	if(X86_32_InstructionContainsValidModRM(decodedInstruction->instruction) > 0){

		// save the rm value if found

		decodedInstruction->containsModRM = true;
		decodedInstruction->modRM = inBuff[decodedInstruction->totalLength];

		decodedInstruction->mod = IA32_MODRM_GET_MOD(decodedInstruction->modRM);
		decodedInstruction->rm = IA32_MODRM_GET_RM(decodedInstruction->modRM);
		decodedInstruction->reg = IA32_MODRM_GET_REG(decodedInstruction->modRM);
		decodedInstruction->totalLength++;

		// If Dest is register only
		if(X86_32_IsOperandRegOnly(decodedInstruction->instruction->dstOperand) > 0 ||
		   X86_32_IsOperandMemOnly(decodedInstruction->instruction->dstOperand) > 0){

			// Source and destination may be filled out wrong in the encoder
			decodedInstruction->regStr = X86_32_GetRegString(decodedInstruction, decodedInstruction->instruction->dstOperand);

		}
		// If Src is register
		if(X86_32_IsOperandRegOnly(decodedInstruction->instruction->srcOperand) > 0 ){
			// Source and destination may be filled out wrong in the encoder
			decodedInstruction->regStr = X86_32_GetRegString(decodedInstruction, decodedInstruction->instruction->srcOperand);

		}

		// If Dest is rm
		if(X86_32_IsOperandRm(decodedInstruction->instruction->dstOperand) > 0 ||
		   X86_32_IsOperandMemOnly(decodedInstruction->instruction->dstOperand) > 0){

			// Gets the rm value next, will set respective SIB and
			// displacement values within. Will fully decode SIB value if found
			// as well, and return that value.
			decodedInstruction->rmStr = X86_32_GetRmString( decodedInstruction, decodedInstruction->instruction->dstOperand);

		}
		// If Src is register
		if(X86_32_IsOperandRm(decodedInstruction->instruction->srcOperand) > 0 ){
			// Gets the rm value next, will set respective SIB and
			// displacement values within. Will fully decode SIB value if found
			// as well, and return that value.
			decodedInstruction->rmStr = X86_32_GetRmString( decodedInstruction, decodedInstruction->instruction->srcOperand);


		}

		// Test for SIB byte and decode if found
		if(decodedInstruction->containsSib > 0){

			decodedInstruction->sib =  inBuff[decodedInstruction->totalLength];
			decodedInstruction->totalLength++;

			// test if SIB byte contains displacement, if so
			// extract.
			if(X86_32_SibContainsDisplacement(decodedInstruction) > 0){
				// switch(decodedInstruction->sib){

				memcpy(&decodedInstruction->sibDispVal,  &inBuff[decodedInstruction->totalLength], 4);
				decodedInstruction->totalLength+=4;

			}

			decodedInstruction->rmStr = X86_32_GetRmSibString(decodedInstruction);
		}

		// Check for displacement
		if(decodedInstruction->containsDisp){
			if(decodedInstruction->dispSize == 0x8){
				decodedInstruction->dispVal = (unsigned char) inBuff[decodedInstruction->totalLength];
				decodedInstruction->totalLength++;
			}

			// record 32bit displacement
			if(decodedInstruction->dispSize == 0x32){
				memcpy(&decodedInstruction->dispVal, &inBuff[decodedInstruction->totalLength], 4);

				// could potentially cause off by one, doublecheck
				decodedInstruction->totalLength+=4;
			}

		}

		// Check for immediates [certain instructions only]
		if(X86_32_IsOperandImmediate(decodedInstruction->instruction->srcOperand)){
			decodedInstruction->containsImm = true;
			switch(decodedInstruction->instruction->srcOperand){
				case IA_IMM8:
					decodedInstruction->immSize = 0x8;
					break;
				case IA_IMM16:
					decodedInstruction->immSize = 0x16;
					break;
				case IA_IMM32:
					decodedInstruction->immSize = 0x32;
					break;
				case IA_IMM64:
					decodedInstruction->immSize = 0x64;
					break;
				default:
					break;
			}
		} else if(X86_32_IsOperandImmediate(decodedInstruction->instruction->dstOperand)){
			decodedInstruction->containsImm = true;
			switch(decodedInstruction->instruction->dstOperand){
				case IA_IMM8:
					decodedInstruction->immSize = 0x8;
					break;
				case IA_IMM16:
					decodedInstruction->immSize = 0x16;
					break;
				case IA_IMM32:
					decodedInstruction->immSize = 0x32;
					break;
				case IA_IMM64:
					decodedInstruction->immSize = 0x64;
					break;
				default:
					break;
			}
		} else if(X86_32_IsOperandImmediate(decodedInstruction->instruction->thirdOperand)){
			decodedInstruction->containsImm = true;
			switch(decodedInstruction->instruction->thirdOperand){
				case IA_IMM8:
					decodedInstruction->immSize = 0x8;
					break;
				case IA_IMM16:
					decodedInstruction->immSize = 0x16;
					break;
				case IA_IMM32:
					decodedInstruction->immSize = 0x32;
					break;
				case IA_IMM64:
					decodedInstruction->immSize = 0x64;
					break;
				default:
					break;
			}

		}

		// exit to end, modrm decoder set displacement and immediate

	}

	// Aribitrary Jump to pass rm if its plus encoded
	plusEncodedJump:
	// printf("\n Instruction Length Total at plus jump label: %i", decodedInstruction->totalLength);
	// If operand register is a static register type, set the
	// register string here.
	if( X86_32_IsOperandStaticRegOnly(decodedInstruction->instruction->srcOperand) > 0 ||
		X86_32_IsOperandStaticRegOnly(decodedInstruction->instruction->dstOperand) > 0 ||
		X86_32_IsOperandStaticRegOnly(decodedInstruction->instruction->thirdOperand) > 0){

		// set flag in structure
		decodedInstruction->containsStaticReg = true;

		// decodedInstruction->regStr =
		if(X86_32_IsOperandStaticRegOnly(decodedInstruction->instruction->dstOperand) > 0)
			decodedInstruction->regStr = X86_32_GetRegString(decodedInstruction, decodedInstruction->instruction->dstOperand);

		// Check Source Operand
		if(X86_32_IsOperandStaticRegOnly(decodedInstruction->instruction->srcOperand) > 0)
			decodedInstruction->regStr = X86_32_GetRegString(decodedInstruction, decodedInstruction->instruction->srcOperand);

		// Check Third Operand
		if(X86_32_IsOperandStaticRegOnly(decodedInstruction->instruction->thirdOperand) > 0)
			decodedInstruction->regStr = X86_32_GetRegString(decodedInstruction, decodedInstruction->instruction->thirdOperand);

		// printf("\n Operand decoded as: %s", decodedInstruction->regStr);

	}


	if(X86_32_IsOperandRel(decodedInstruction->instruction->srcOperand) > 0 ||
	   X86_32_IsOperandRel(decodedInstruction->instruction->dstOperand) > 0 ||
	   X86_32_IsOperandRel(decodedInstruction->instruction->thirdOperand) > 0){

		decodedInstruction->containsRel = true;

		if(decodedInstruction->instruction->srcOperand == IA_REL8||
			decodedInstruction->instruction->dstOperand == IA_REL8||
			decodedInstruction->instruction->thirdOperand == IA_REL8){

			decodedInstruction->relSize = 0x08;
			memcpy(&decodedInstruction->relVal, &inBuff[decodedInstruction->totalLength-1], 1);

			goto end;
		}


		if(decodedInstruction->instruction->srcOperand == IA_REL16||
			decodedInstruction->instruction->dstOperand == IA_REL16||
			decodedInstruction->instruction->thirdOperand == IA_REL16){

			if(decodedInstruction->operSizeOverride > 0 || decodedInstruction->addrSizeOverride > 0){
				decodedInstruction->relSize = 0x16;
				memcpy(&decodedInstruction->relVal, &inBuff[decodedInstruction->totalLength], 2);
					decodedInstruction->totalLength+=2;
			} else {
				decodedInstruction->relSize = 0x32;
				memcpy(&decodedInstruction->relVal, &inBuff[decodedInstruction->totalLength], 4);
					decodedInstruction->totalLength+=4;
			}

			goto end;
		}

		if(decodedInstruction->instruction->srcOperand == IA_REL32||
			decodedInstruction->instruction->dstOperand == IA_REL32||
			decodedInstruction->instruction->thirdOperand == IA_REL32){

			decodedInstruction->relSize = 0x32;
			memcpy(&decodedInstruction->relVal, &inBuff[decodedInstruction->totalLength], 4);
					decodedInstruction->totalLength+=4;

			goto end;
		}
		// At the end of rel, jump to end as a instruction
		// containing rel + extra cannot exit


		// printf("\n aquired rel size: %.02x", decodedInstruction->relSize);
	}


	// Set immediate size/value if not already set
	if(X86_32_IsOperandImmediate(decodedInstruction->instruction->srcOperand) > 0 ||
	   X86_32_IsOperandImmediate(decodedInstruction->instruction->dstOperand) > 0 ||
	   X86_32_IsOperandImmediate(decodedInstruction->instruction->thirdOperand) > 0){


		decodedInstruction->containsImm = true;
		if(decodedInstruction->instruction->srcOperand == IA_IMM32||
		   decodedInstruction->instruction->dstOperand == IA_IMM32||
		   decodedInstruction->instruction->thirdOperand == IA_IMM32){
				decodedInstruction->immSize = 0x32;
		}

		if(decodedInstruction->instruction->srcOperand == IA_IMM16||
		   decodedInstruction->instruction->dstOperand == IA_IMM16||
		   decodedInstruction->instruction->thirdOperand == IA_IMM16){

			// Change if operand is broken
			//if(decodedInstruction->operSizeOverride > 0 ||
			//	decodedInstruction->addrSizeOverride > 0){

				decodedInstruction->immSize = 0x16;

			/*
			else {
				decodedInstruction->immSize = 0x32;
			}
			*/
		}

		if(decodedInstruction->instruction->srcOperand == IA_IMM8||
		   decodedInstruction->instruction->dstOperand == IA_IMM8||
		   decodedInstruction->instruction->thirdOperand == IA_IMM8){
			decodedInstruction->immSize = 0x08;
		}


		// printf("\n hrm. contains immediate: %.02x", decodedInstruction->immSize);
		// find imm size

	}

	if(decodedInstruction->containsImm == true){
		if(decodedInstruction->immSize == 0x8){
			decodedInstruction->immVal = (unsigned char) inBuff[decodedInstruction->totalLength];
			decodedInstruction->totalLength++;
		}
		// record 16bit immediate
		if(decodedInstruction->immSize == 0x16){
			memcpy(&decodedInstruction->immVal, &inBuff[decodedInstruction->totalLength], 2);

			// could potentially cause off by one, doublecheck
			decodedInstruction->totalLength+=2;
		}

		// record 32bit immediate
		if(decodedInstruction->immSize == 0x32){
			memcpy(&decodedInstruction->immVal, &inBuff[decodedInstruction->totalLength], 4);

			// could potentially cause off by one, doublecheck
			decodedInstruction->totalLength+=4;
		}

		// record 64bit displacement
		if(decodedInstruction->immSize == 0x64){
			memcpy(&decodedInstruction->immVal, &inBuff[decodedInstruction->totalLength], 4);

			// could potentially cause off by one, doublecheck
			decodedInstruction->totalLength+=4;
		}

	}

	// test and verify that the instruction isn't langer then max size
	if(decodedInstruction->totalLength > 15)
			return NULL;

	// examine the modr/m byte value and find out if the r/m is a sib
	// with a displacement. If so set and decode SIB values.
	/*
	decodedInstruction->containsSib = X86_32_IsRmSibEncoded(decodedInstruction->modRM);
	if(decodedInstruction->containsSib != 0){
		decodedInstruction->sib = inBuff[decodedInstruction->totalLength];
		decodedInstruction->totalLength++;
	}
	*/

	// Exit Label
	end:
	// printf("\n %s %s, %s, %x", decodedInstruction->instruction->instructionStr, decodedInstruction->regStr, decodedInstruction->rmStr, decodedInstruction->dispVal);
	// printf("\n total length?: %i", decodedInstruction->totalLength);
	// printf("\n -> %.2x", inBuff[0]);
	// test and verify that the instruction isn't langer then max size
	if(decodedInstruction->totalLength > 15)
			return NULL;


	// get the rm string + sib if present

	// get displacement if required

	// get immediate if required

	return decodedInstruction;
}


// The Base Decoder Function, will decode instructions in a buffer
// buff, with a second size parameter.  Returns the starting
// element in a walkable list of instruction. Base addr is
// an optional operand used for automatically assigning address values
// for decoded instructions.  They are null otherwise.  Address is
// Previous Instruction length + base address.
P_X86_32_DECODED_INSTRUCTION_LIST X86_32_DecodeInstructions(P_X86_32_INSTRUCTION instTable, unsigned char *buff, size_t length, void *baseAddr){

	if(length==0)
		return NULL;

	P_X86_32_DECODED_INSTRUCTION_LIST decodedList = NULL;

	// arbitrary list pointers used for list calculations below
	P_X86_32_DECODED_INSTRUCTION_LIST decodedListFirst = NULL;
	P_X86_32_DECODED_INSTRUCTION_LIST decodedListPrev  = NULL;

	// Save address
	size_t address = (size_t) baseAddr;

	// Total instruction list cannot pass [length]
	size_t totalLength = 0;
	size_t loopLen = 0;

	for(totalLength = 0; totalLength < length; ){

		// Length Calculation
		if((length - totalLength) < 15){
			loopLen = length-totalLength;
		} else {
			loopLen = 15;
		}

		// Decode instruction
		decodedList = X86_32_DecodeInstruction(instTable, &buff[totalLength], loopLen);

		// Check to verify if the decoded loop was null.  If it was
		// create an empty element with a instruction string "(bad)"
		if(decodedList == NULL){

			decodedList = X86_32_CreateInstructionElement(decodedListFirst, decodedListPrev, NULL);

			// store previous and next in previous element
			if(decodedListPrev != NULL)
				decodedListPrev->next = decodedList;

			decodedList->prev = decodedListPrev;

			// reset previous
			decodedListPrev = decodedList;
			decodedList->address = (void *) (address + totalLength);

			// Set first pointer
			if(decodedListFirst == NULL){
				decodedListFirst = decodedList;
			}
			totalLength++;
		    continue;

		} else {

			// Set first pointer
			if(decodedListFirst == NULL){
				decodedListFirst = decodedList;
			}

			// set address
			decodedList->address = (void *) (address + totalLength);

			// Set first and prev, next is already null from memset
			decodedList->first = decodedListFirst;
			decodedList->prev = decodedListPrev;


			// store previous and next in previous element
			if(decodedListPrev != NULL)
				decodedListPrev->next = decodedList;

			decodedList->prev = decodedListPrev;

			// set previous for loop
			decodedListPrev = decodedList;

			// Increase total length
			totalLength += decodedList->totalLength;
		}

		// instead of printing the instruction struct, perhaps
		decodedList->intelDecode = X86_32_GetDecodedInstOperandString_INTEL(decodedList);

	}

	// kind of a hack to fix the initial missing of the first assignment
	decodedList->first->first = decodedList->first;

	return decodedList->first;
}


// Gets a null terminated string representing the operand string for
// an instruction. Will return string in ATT SYNTAX.
char * X86_32_GetDecodedInstOperandString_INTEL(P_X86_32_DECODED_INSTRUCTION instruction){

	// Stack, builder strings.
	char srcString[100];
		memset(srcString, 0x00, 100);

	char dstString[100];
		memset(dstString, 0x00, 100);

	char thirdString[100];
		memset(thirdString, 0x00, 100);

	// Final String
	char instString[300];
		memset(instString, 0x00, 300);


	// printf("\n instruction strings: [reg] %s [rm] %s", instruction->regStr, instruction->rmStr);

	// Pointer Type
	// Assignment Detail: Source
	if(instruction->containsPtr > 0){
		if(instruction->ptrSize == 0x32){
			sprintf(srcString, "%.02x:%.08x", (unsigned short) instruction->ptrDispVal, instruction->ptrVal);
		} else {
			sprintf(srcString, "%.02x:%.04x", (unsigned short) instruction->ptrDispVal, instruction->ptrVal);
		}
		goto end;
	}

	// Rel Type
	// Assignment Detail: Source
	if(instruction->containsRel > 0){
		if(srcString[strlen(srcString)-1] == ','){
			srcString[strlen(srcString)-1] = ' ';
		}
		switch(instruction->relSize){
			case 0x08:
				sprintf(&srcString[strlen(srcString)], "0x%.02x", instruction->relVal);
				break;
			case 0x16:
				sprintf(&srcString[strlen(srcString)], "0x%.04x", instruction->relVal);
				break;
			case 0x32:
				sprintf(&srcString[strlen(srcString)], "0x%.02x", instruction->relVal);
				break;
			default:
				break;
		}

		goto end;
	}

	// check third first
	// Assignment Detail: Third
	if(instruction->instruction->thirdOperandStr != NULL){

		// Rm/Mem Only Test
		if(X86_32_IsOperandMemOnly(instruction->instruction->thirdOperand) > 0 ||
		   X86_32_IsOperandRm(instruction->instruction->thirdOperand) > 0){

			if(instruction->rmStr != NULL)
				strcat(thirdString, instruction->rmStr);
			else {
				strcat(thirdString, "(bad)");
			}

			// Append Displacement if Found
			if(instruction->containsDisp > 0){
				switch(instruction->dispSize){
					case 0x08:
						sprintf(&thirdString[strlen(thirdString)], " + 0x%.02x", instruction->dispVal);
						break;
					case 0x16:
						sprintf(&thirdString[strlen(thirdString)], " + 0x%.04x", instruction->dispVal);
						break;
					case 0x32:
						sprintf(&thirdString[strlen(thirdString)], " + 0x%.08x", instruction->dispVal);
						break;
					case 0x64:
						sprintf(&thirdString[strlen(thirdString)], " + 0x%.08x", instruction->dispVal);
						break;
					default:
						break;
				}
			}
		}

		if(X86_32_IsOperandRegOnly(instruction->instruction->thirdOperand) > 0 ||
			X86_32_IsOperandStaticRegOnly(instruction->instruction->thirdOperand) > 0){

			if(instruction->regStr != NULL)
				strcat(thirdString, instruction->regStr);
			else
				strcat(thirdString, "(bad)");

		}

		// if its an immediate, print out the immediate
		if(X86_32_IsOperandImmediate(instruction->instruction->thirdOperand) > 0){
			// Check the immediate size
			switch(instruction->immSize){
				case 0x08:
					sprintf(&thirdString[strlen(thirdString)], "0x%.02x", instruction->immVal);
					break;
				case 0x16:
					sprintf(&thirdString[strlen(thirdString)], "0x%.04x", instruction->immVal);
					break;
				case 0x32:
					sprintf(&thirdString[strlen(thirdString)], "0x%.08x", instruction->immVal);
					break;
				default:
					break;
			}
		}


		// add the terminating comma (no str functions for optimization)
		// add terminator at bottom instString[strlen(instString)] = ',';

	}


	// Verify intel dst operand information.
	// Assignment: Destination
	if(instruction->instruction->dstOperandStr != NULL){

		// Reg / Mem Test
		if(X86_32_IsOperandMemOnly(instruction->instruction->dstOperand) > 0||
		   X86_32_IsOperandRm(instruction->instruction->dstOperand) > 0){

			if(instruction->rmStr != NULL){
				// printf("\n hrmm, rm string %s", instruction->rmStr );
				strcat(dstString, instruction->rmStr);
			} else {
				strcat(dstString, "(bad)");
			}

			// Append Displacement if Found
			if(instruction->containsDisp > 0){

				switch(instruction->dispSize){
					case 0x08:
						sprintf(&dstString[strlen(dstString)], " + 0x%.02x", instruction->dispVal);
						break;
					case 0x16:
						sprintf(&dstString[strlen(dstString)], " + 0x%.04x", instruction->dispVal);
						break;
					case 0x32:
						sprintf(&dstString[strlen(dstString)], " + 0x%.08x", instruction->dispVal);
						break;
					case 0x64:
						sprintf(&dstString[strlen(dstString)], " + 0x%.08x", instruction->dispVal);
						break;
					default:
						break;
				}
			}

		}

		if(X86_32_IsOperandRegOnly(instruction->instruction->dstOperand) > 0 ||
		X86_32_IsOperandStaticRegOnly(instruction->instruction->dstOperand) > 0){
			if(instruction->regStr != NULL){
				strcat(dstString, instruction->regStr);
			} else {

				strcat(dstString, "(bad)");
			}
		}

		// if its an immediate, print out the immediate
		if(X86_32_IsOperandImmediate(instruction->instruction->dstOperand) > 0){

			// Check the immediate size
			switch(instruction->immSize){
				case 0x08:
					sprintf(&dstString[strlen(dstString)], "0x%.02x", instruction->immVal);
					break;
				case 0x16:
					sprintf(&dstString[strlen(dstString)], "0x%.04x", instruction->immVal);
					break;
				case 0x32:
					sprintf(&dstString[strlen(dstString)], "0x%.08x", instruction->immVal);
					break;
				default:
					break;
			}
		}

		// add the terminating comma (no str functions for optimization)
		// add at end instString[strlen(instString)] = ',';

	}

	// Check assignments.
	// Assignment: Source
	if(instruction->instruction->srcOperandStr != NULL){

		// Mem Only Test
		if(X86_32_IsOperandMemOnly(instruction->instruction->srcOperand) > 0 ||
			X86_32_IsOperandRm(instruction->instruction->srcOperand) > 0){
			if(instruction->rmStr != NULL){

				strcat(srcString, instruction->rmStr);
			} else {
				strcat(srcString, "(bad)");
			}

			// Append Displacement if Found
			if(instruction->containsDisp > 0){

				switch(instruction->dispSize){
					case 0x08:
						sprintf(&srcString[strlen(srcString)], " + 0x%.02x", instruction->dispVal);
						break;
					case 0x16:
						sprintf(&srcString[strlen(srcString)], " + 0x%.04x", instruction->dispVal);
						break;
					case 0x32:
						sprintf(&srcString[strlen(srcString)], " + 0x%.08x", instruction->dispVal);
						break;
					case 0x64:
						sprintf(&srcString[strlen(srcString)], " + 0x%.08x", instruction->dispVal);
						break;
					default:
						break;
				}
			}


		}

		if(X86_32_IsOperandRegOnly(instruction->instruction->srcOperand) > 0 ||
		X86_32_IsOperandStaticRegOnly(instruction->instruction->srcOperand) > 0){
			if(instruction->regStr != NULL) {
				strcat(srcString, instruction->regStr);
			}
			else {
				strcat(srcString, "(bad)");
			}
		}

		// if its an immediate, print out the immediate
		if(X86_32_IsOperandImmediate(instruction->instruction->srcOperand) > 0){
			// Check the immediate size
			switch(instruction->immSize){
				case 0x08:
					sprintf(&srcString[strlen(srcString)], "0x%.02x", instruction->immVal);
					break;
				case 0x16:
					sprintf(&srcString[strlen(srcString)], "0x%.04x", instruction->immVal);
					break;
				case 0x32:
					sprintf(&srcString[strlen(srcString)], "0x%.08x", instruction->immVal);
					break;
				default:
					break;
			}
		}
		// add the terminating comma (no str functions for optimization)
		// un-needed for third operand
		// instString[strlen(instString)] = ',';

	}


	end:
	/*
	if(instString[strlen(instString)-1] == ',' ||
	   instString[strlen(instString)-1] == '['){
		instString[strlen(instString)-1] = ' ';
		// instString[strlen(instString)] = ' ';
	}
	*/

	// String Setup
	// Append Third
	if(strlen(thirdString) > 0){
		strncat(instString, thirdString, 80);
	}



	// Append Destination
	if(strnlen(dstString, 100) > 0){
		if(strlen(thirdString) > 0)
			strncat(instString, ", ", 3);
		strncat(instString, dstString, 80);
	}

	// Append Source
	if(strlen(srcString) > 0 ){
		if(strlen(dstString)> 0)
			strncat(instString, ", ", 3);

		strncat(instString, srcString, 80);
	}

	// printf("\n %s  %s", instruction->instruction->instructionStr, instString);

	return bpstrndup(instString, 299);
}



// This function will look at an arbitrary buffer string, and determine
// how many prefixes, and which specific prefixes are included in the
// following instruction.  This function actually is responsible for
// returning the decoded instruction element used with decoding.  This
// function must always be called before X86_32_DecodeOpcode and when
// calling X86_32_DecodeOpcode, it must be passed as a parameter.
P_X86_32_DECODED_INSTRUCTION X86_32_DecodePrefixes(void *buff){

	if(buff == NULL) return NULL;

	unsigned char *inBuff = (unsigned char *) buff;
	char *prefixStr = NULL;

	// create the decoded structure
	P_X86_32_DECODED_INSTRUCTION instruction = X86_32_CreateInstructionElement(
			       (P_X86_32_DECODED_INSTRUCTION) NULL,
			       (P_X86_32_DECODED_INSTRUCTION) NULL,
			       (P_X86_32_DECODED_INSTRUCTION) NULL);

	// Return null if we cannot allocate the instruction
	if(instruction == NULL) return NULL;

	// An instruction can have a maximum of 14 prefixes
	// so the string will only ever be a maximum of 14 bytes.
	for(unsigned char i = 0; i < 14; i++){

		prefixStr = X86_32_IsBytePrefix(inBuff[i]);
		if(prefixStr == NULL){
			break;
		}

		// increment the base prefix count
		instruction->prefixCount++;

		// assign the actual prefix values
		instruction->prefixes[i].bytePrefix = inBuff[i];
		if(inBuff[i] == 0x66)
			instruction->operSizeOverride = true;

		if(inBuff[i] == 0x67)
			instruction->addrSizeOverride = true;

		// a string representing the literal prefix
		// definition.
		instruction->prefixes[i].prefixStr = bpstrndup(prefixStr, 10);

	}

	return instruction;
}

// This function contains an optimized loop that decodes opcodes
// of 1-3 bytes in length to their respective instruction.  The byte string
// provided must be at least 3 bytes long or an improper decode or potential
// segfault could occur.
P_X86_32_INSTRUCTION X86_32_DecodeOpcode(P_X86_32_INSTRUCTION_TABLE instTable, P_X86_32_DECODED_INSTRUCTION partialDecode, void *buff){

	// Must have a partial decode to continue
	if(partialDecode == NULL)
		return NULL;

	size_t x = 0;
	size_t baseCounter;
	size_t instLen;
	size_t i = 0;

	// Local function flag used to determine if an opcode does not match
	// the current instruction being analyzed
	size_t badMatch = false;

	// generic unsigned pointer for opcode operations
	unsigned char * inBuff = (unsigned char *) buff;

	// advance the buffer past the total length of the prefixes
	// inBuff = &inBuff[partialDecode->prefixCount];

	// character to hold a temporary rm value
	unsigned char rm = 0;

	// check first to see if its +digit encoded
	unsigned char plusBaseOpcode = X86_32_IsBytePlusEncodedOpcode(inBuff);
	unsigned char inBuffPlusEncodedDup[15];
	if(plusBaseOpcode != 0){

		// This effectively copies the in buffer to
		// be able to make modifications without modifying the
		// original buffer.

		// Set and copy over data
		memset(inBuffPlusEncodedDup, 0x00, 15);
		memcpy(inBuffPlusEncodedDup, inBuff, 15);

		// Swap out first byte
		inBuffPlusEncodedDup[0] = (unsigned char) plusBaseOpcode;

		// Swqp out pointers
		inBuff = inBuffPlusEncodedDup;

	}


	// NOTE: THIS WILL BE REPLACED IN THE FUTURE WITH A B+ TREE TO SPEED
	// IT UP A GOOD AMOUNT.

	// Loop through instruction table
	for(baseCounter = 0; baseCounter < TOP_INSTRUCTION; ){

		nextInst:
		baseCounter++;

		// set the local instruction length
		instLen = instTable[baseCounter].binOpcodeLen;

		// reset the badmatch flag on loop so that we start with
		// a clean slate.
		badMatch = false;

		// reloop if the opcode length is 0, as then it is a
		// prefix not an opcode.
		if(instTable[baseCounter].binOpcodeLen == 0)
			continue;

		// Find the correct instruction
		for(i = 0; i < instLen; i++ ){

			if( (unsigned char) inBuff[i] != (unsigned char) instTable[baseCounter].binOpcode[i] ){
				badMatch=true;
				break;
			}

			if(instTable[baseCounter].binOpcodeLen > 1){
				if( inBuff[i+1] != (unsigned char) instTable[baseCounter].binOpcode[i+1] ){
					badMatch = true;
					break;
				}
			}

			// return to the top of the loop if we activated
			// a bad match above (aka move to next instruction comparison)
			if(badMatch == true)
				goto nextInst;


			// 16bit/32bit determination
			if(X86_32_InstructionSizeVaries(&instTable[baseCounter]) > 0){

				if(partialDecode->addrSizeOverride <= 0 ||
				   partialDecode->operSizeOverride <= 0){
						baseCounter++;
				}

			}


			// set the rm byte if it determined that the opcode
			// contains an rm value.
			rm = inBuff[instTable[baseCounter].binOpcodeLen];

			// Now we need to verify against operands to make sure that it syncs up
			if(instTable[baseCounter].opcodeOperandCount != 0)
			for(x = 0; x < instTable[baseCounter].opcodeOperandCount; x++){

				// Test the operands one by one to see if they're a /digit.  if
				// they are, match up the rm byte to see if this is the correct
				// instruction, if not: break the loop.
				switch(instTable[baseCounter].opcodeOperand[x]){

					// digit /0
					case OA_RM_ONLY_0:
						if(X86_32_GetSlashDigitFromRm(rm) != 0)
							badMatch = true;
						break;

					// digit /1
					case OA_RM_ONLY_1:
						if(X86_32_GetSlashDigitFromRm(rm) != 1)
							badMatch = true;
						break;

					// digit /2
					case OA_RM_ONLY_2:
						if(X86_32_GetSlashDigitFromRm(rm) != 2)
							badMatch = true;
						break;

					// digit /3
					case OA_RM_ONLY_3:
						if(X86_32_GetSlashDigitFromRm(rm) != 3)
							badMatch = true;
						break;

					// digit /4
					case OA_RM_ONLY_4:
						if(X86_32_GetSlashDigitFromRm(rm) != 4)
							badMatch = true;
						break;

					// digit /5
					case OA_RM_ONLY_5:
						if(X86_32_GetSlashDigitFromRm(rm) != 5)
							badMatch = true;
						break;

					// digit /6
					case OA_RM_ONLY_6:
						if(X86_32_GetSlashDigitFromRm(rm) != 6)
							badMatch = true;
						break;

					// digit /7
					case OA_RM_ONLY_7:
						if(X86_32_GetSlashDigitFromRm(rm) != 7)
							badMatch = true;
						break;

					default:
						break;

				}

			}

			// now we have to verify operand sizes and types, this will depend
			// on previous prefix calculations.

		}

		// Because of intels insanity, this double check must be here
		// to make sure we didn't logically skip to the wrong instruction based
		// on previous instruction assumptions.
		if((unsigned char ) inBuff[0] != (unsigned char) instTable[baseCounter].binOpcode[0]){
			badMatch = true;
		}

		// if badmatch is false, we have a correct match
		// break the loop and return the instruction
		if(badMatch == false){

			// double check our opcodes
/*
			printf("\n Found Correct Instruction at Offs: %i ", baseCounter);
			printf("\n RM IS?: %u", X86_32_GetSlashDigitFromRm(rm));
			printf("\n First byte is?: %.02x", (unsigned char) instTable[baseCounter].binOpcode[0]);
*/

			goto end;
		}

	}

	// if the flag is set to which a bad match is found,
	// return NULL as an error.
	if(badMatch == true){
		return NULL;
	}

	// Exits with a legitimate opcode
	end:
	if(badMatch == false){
		// printf("\n Returning: %i", instTable[baseCounter].binOpcodeLen);
		return &instTable[baseCounter];
	}

	// shouldn't actually get here.. bad match should return null above
	// this is simply here to remove the warning message.
	return NULL;
}

#endif // #if BLOCKPARTY_DEBUGGING_TARGET_32BIT == 1

#endif // USE_BLOCKPARTY_DEBUGGING == 1

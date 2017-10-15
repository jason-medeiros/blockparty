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

size_t X86_32_SibContainsDisplacement(P_X86_32_DECODED_INSTRUCTION instruction){
	size_t base = IA32_SIB_GET_BASE(instruction->sib);
	if(base == 0x5){
		return true;
	}
	return 0;
}



// tests an instruction to see if it contains a modr/m byte.
size_t X86_32_InstructionContainsModRM(P_X86_32_INSTRUCTION instruction){

	return 0;
}

// tests an instruction to see if it contains a sib byte
size_t X86_32_InstructionContainsSib(P_X86_32_INSTRUCTION instruction){
	return 0;
}


// Looks at an RM byte and determines if it's a memory operand
// true for success, false for failure.
size_t X86_32_IsRmMemoryOperand(unsigned char byte){
	if(byte < 0xc0)
		return true;

	return 0;
}

// Looks at an RM byte and determines if the "register/memory" operand is a register type
// aka. not memory [*ax] type. true for success, false for failure.
size_t X86_32_IsRmRegisterOperand(unsigned char byte){
	if(byte > 0xbf)
		return true;

	return 0;
}

// Looks at the RM byte and determines if it is a sib encoded
// rm value.
size_t X86_32_IsRmSibEncoded(unsigned char byte){

	// this is effectively this
	// if ( ((i & 0xf) == 0x4) || ((i&0xf) == 0xc) ) return true;

	// i wrote out the switches to make it easier to understand for whoever
	// ends up doing maintainence.

	switch(byte){

		// SIB encoded with no displacement
		case 0x04:
			return true;
		case 0x0c:
			return true;
		case 0x14:
			return true;
		case 0x1c:
			return true;
		case 0x24:
			return true;
		case 0x2c:
			return true;
		case 0x34:
			return true;
		case 0x3c:
			return true;

		// now disp8 sib encoding
		case 0x44:
			return true;
		case 0x4c:
			return true;
		case 0x54:
			return true;
		case 0x5c:
			return true;
		case 0x64:
			return true;
		case 0x6c:
			return true;
		case 0x74:
			return true;
		case 0x7c:
			return true;

		// sib 32bit disp encoding
		case 0x84:
			return true;
		case 0x8c:
			return true;
		case 0x94:
			return true;
		case 0x9c:
			return true;
		case 0xa4:
			return true;
		case 0xac:
			return true;
		case 0xb4:
			return true;
		case 0xbc:
			return true;

		default:
			break;
	}

	return 0;
}

// Retrieves the /digit number that translates into the reg
// collumn. -1 for fail.  0-7 for digit.
size_t X86_32_GetSlashDigitFromRm(unsigned char byte){

	// slash 0 first
	if(
			(byte <= 0x7) ||
			(byte >= 0x40 && byte <= 0x47) ||
			(byte >= 0x80 && byte <= 0x87) ||
			(byte >= 0xc0 && byte <= 0xc7)
	){
		return 0;
	}

	// slash 1 next
	if(
			(byte >= 0x08 && byte <= 0x0f) ||
			(byte >= 0x48 && byte <= 0x4f) ||
			(byte >= 0x88 && byte <= 0x8f) ||
			(byte >= 0xc8 && byte <= 0xcf)
	){
		return 1;
	}

	// slash 2
	if(
			(byte >= 0x10 && byte <= 0x17) ||
			(byte >= 0x50 && byte <= 0x57) ||
			(byte >= 0x90 && byte <= 0x97) ||
			(byte >= 0xd0 && byte <= 0xd7)
	){
		return 2;
	}

	// slash 3
	if(
			(byte >= 0x18 && byte <= 0x1f) ||
			(byte >= 0x58 && byte <= 0x5f) ||
			(byte >= 0x98 && byte <= 0x9f) ||
			(byte >= 0xd8 && byte <= 0xdf)
	){
		return 3;
	}

	// slash 4
	if(
			(byte >= 0x20 && byte <= 0x27) ||
			(byte >= 0x60 && byte <= 0x67) ||
			(byte >= 0xa0 && byte <= 0xa7) ||
			(byte >= 0xe0 && byte <= 0xe7)
	){
		return 4;
	}

	// slash 5
	if(
			(byte >= 0x28 && byte <= 0x2f) ||
			(byte >= 0x68 && byte <= 0x6f) ||
			(byte >= 0xa8 && byte <= 0xaf) ||
			(byte >= 0xe8 && byte <= 0xef)
	){
		return 5;
	}

	// slash 6
	if(
			(byte >= 0x30 && byte <= 0x37) ||
			(byte >= 0x70 && byte <= 0x77) ||
			(byte >= 0xb0 && byte <= 0xb7) ||
			(byte >= 0xf0 && byte <= 0xf7)
	){
		return 6;
	}

	// slash 7 - last has to be 0xfe because of signed comparissons
	if(
			(byte >= 0x38 && byte <= 0x3f) ||
			(byte >= 0x78 && byte <= 0x7f) ||
			(byte >= 0xb8 && byte <= 0xbf) ||
			(byte >= 0xf8 && byte <= 0xff)
	){
		return 7;
	}

	// should never get here, ever.
	return 0xbad;
}


/*
 * Instruction Test Functions: Used to test various instruction components.
 */

// Src Functions:
// -------------------------
// Tests an instructions source operand and reports true if it is memory only
size_t X86_32_IsInstructionSrcMemOnly(P_X86_32_INSTRUCTION instruction){
	return X86_32_IsOperandMemOnly(instruction->srcOperand);
}

// Tests an instruction src operand and reports true if its register (r8/16/32) only
size_t X86_32_IsInstructionSrcRegOnly(P_X86_32_INSTRUCTION instruction){
	return X86_32_IsOperandRegOnly(instruction->srcOperand);
}


// Dst Functions
// ------------------------
// Tests an instructions dest operand and reports true if it is memory only
size_t X86_32_IsInstructionDstMemOnly(P_X86_32_INSTRUCTION instruction){
	return X86_32_IsOperandMemOnly(instruction->dstOperand);
}

// Tests an instructions dest operand and reports true if it is memory only
size_t X86_32_IsInstructionDstRegOnly(P_X86_32_INSTRUCTION instruction){
	return X86_32_IsOperandRegOnly(instruction->dstOperand);
}


// Third Optional Operand Functions
// ------------------------
// Tests an instructions dest operand and reports true if it is memory only
size_t X86_32_IsInstructionThirdMemOnly(P_X86_32_INSTRUCTION instruction){
	return X86_32_IsOperandMemOnly(instruction->thirdOperand);
}

// Tests an instructions dest operand and reports true if it is memory only
size_t X86_32_IsInstructionThirdRegOnly(P_X86_32_INSTRUCTION instruction){
	return X86_32_IsOperandRegOnly(instruction->thirdOperand);
}

// If an instruction can be overridden, that means that it can have
// a 32 bit variant
size_t X86_32_InstructionSizeVaries(P_X86_32_INSTRUCTION instruction){

	// ADC : 15 ib
	if((unsigned char) instruction->binOpcode[0] == 0x15){
		return true;
	}

	// ADC : 83 /2 ib
	if((unsigned char) instruction->binOpcode[0] == 0x83){
		if(instruction->opcodeOperand[0] == OA_RM_ONLY_2 &&
		   instruction->opcodeOperand[1] == OA_IB){
			return true;
		}
	}

	// ADC : 83 /2 ib
	if((unsigned char)  instruction->binOpcode[0] == 0x11){
		if(instruction->opcodeOperand[0] == OA_MODRM){
			return true;
		}
	}

	// ADC : 83 /2 ib
	if((unsigned char)  instruction->binOpcode[0] == 0x13){
		if(instruction->opcodeOperand[0] == OA_MODRM){
			return true;
		}
	}

	// ADD : 83 /0 ib
	if((unsigned char) instruction->binOpcode[0] == 0x83){
		if(instruction->opcodeOperand[0] == OA_RM_ONLY_0 &&
		   instruction->opcodeOperand[1] == OA_IB){
			return true;
		}
	}

	// ADD : 01 /r
	if((unsigned char) instruction->binOpcode[0] == 0x01){
		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// ADD : 03 /r
	if((unsigned char) instruction->binOpcode[0] == 0x03){
		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// ADD : 03 /r
	if((unsigned char) instruction->binOpcode[0] == 0x05){
		if(instruction->opcodeOperand[0] == OA_IW ){
			return true;
		}
	}

	// AND : 21 /r
	if((unsigned char) instruction->binOpcode[0] == 0x21){
		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// AND : 25 /r
	if((unsigned char) instruction->binOpcode[0] == 0x25){
			if(instruction->opcodeOperand[0] == OA_IW ){
				return true;
			}
	}
	// AND : 81 /4 iw
	if((unsigned char) instruction->binOpcode[0] == 0x81){
			if(instruction->opcodeOperand[0] == OA_RM_ONLY_4 &&
			   instruction->opcodeOperand[1] == OA_IW)
				return true;
	}

	// BOUND : 62 /r
	if((unsigned char) instruction->binOpcode[0] == 0x61){
		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// BSF : 0F BC /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0xbc){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}

	}

	// BSR : 0F BD /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0xbd){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}

	}

	// BT : 0F A3
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0xa3){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}

	}

	// BT : 0F BA /4 ib
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0xba){

		if(instruction->opcodeOperand[0] == OA_RM_ONLY_4 ){
			return true;
		}

	}

	// BTC : 0F BB /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0xa3){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}

	}

	// BTC : 0F BA /7 ib
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0xba){

		if(instruction->opcodeOperand[0] == OA_RM_ONLY_7 ){
			return true;
		}

	}

	// BTR : 0F B3 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0xb3){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}

	}

	// BTS : 0F AB /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0xab){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}

	}


	// BTS : 0F BA /5 ib
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0xba){

		if(instruction->opcodeOperand[0] == OA_RM_ONLY_5 ){
			return true;
		}

	}

	// CALL : FF /2
	if((unsigned char) instruction->binOpcode[0] == 0xff ){

		if(instruction->opcodeOperand[0] == OA_RM_ONLY_2 ){
			return true;
		}

	}

	// CALL : FF /3
	if((unsigned char) instruction->binOpcode[0] == 0xff ){

		if(instruction->opcodeOperand[0] == OA_RM_ONLY_3 ){
			return true;
		}

	}

	// CMOVA : 0F 47 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0x47){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}

	}

	// CMOVAE : 0F 43 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0x43){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}

	}

	// CMOVB : 0F 42 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0x42){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}

	}

	// CMOVBE : 0F 46 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0x46){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}

	}

	// CMOVC : 0F 42 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0x42){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}

	}

	// CMOVE : 0F 44 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0x44){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}

	}

	// CMOVG : 0F 4F /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0x4f){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}

	}

	// CMOVGE : 0F 4D /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0x4d){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}

	}

	// CMOVL : 0F 4C /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0x4c){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}

	}

	// CMOVL : 0F 4E /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0x4e){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}

	}

	// CMOVNA : 0F 46 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0x46){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}

	}

	// CMOVNAE : 0F 42 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0x42){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}

	}

	// CMOVNB : 0F 43 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0x43){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}

	}

	// CMOVNBE : 0F 47 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	   (unsigned char) instruction->binOpcode[1] == 0x47){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}

	}

	// CMOVNC : 0F 43 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x43){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// CMOVNE : 0F 45 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x45){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// CMOVNG : 0F 4E /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x4E){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// CMOVNGE : 0F 4C /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x4C){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// CMOVNL : 0F 4D /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x4D){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// CMOVNLE : 0F 4F /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x4F){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// CMOVNO : 0F 41 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x41){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// CMOVNP : 0F 4B /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x4B){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// CMOVNS : 0F 49 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x49){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// CMOVNZ : 0F 45 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x45){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// CMOVO : 0F 40 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x40){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// CMOVP : 0F 4A /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x4A){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// CMOVPE : 0F 4A /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x4A){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// CMOVPE : 0F 4A /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x4A){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// CMOVPO : 0F 4B /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x4B){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// CMOVS : 0F 48 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x48){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// CMOVZ : 0F 44 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x44){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// CMPS: A7
	if((unsigned char) instruction->binOpcode[0] == 0xA7){
		return true;
	}

	// CMPXCHG : 0F B1 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0xB1){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// CDQ: 99
	if((unsigned char) instruction->binOpcode[0] == 0x99){
		return true;
	}


	// DEC : FF /1
	if((unsigned char) instruction->binOpcode[0] == 0xFF){
		if(instruction->opcodeOperand[0] == OA_RM_ONLY_1 ){
			return true;
		}
	}

	// DEC : 49
	if((unsigned char) instruction->binOpcode[0] == 0x49){
		return true;
	}

	// IDIV : F7 /7
	if((unsigned char) instruction->binOpcode[0] == 0xF7){

		if(instruction->opcodeOperand[0] == OA_RM_ONLY_7 ){
			return true;
		}
	}

	// IMUL : F7 /5
	if((unsigned char) instruction->binOpcode[0] == 0xF7){

		if(instruction->opcodeOperand[0] == OA_RM_ONLY_5 ){
			return true;
		}
	}

	// IMUL : 0F AF /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0xAF){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// IMUL : 6B /r ib
	if((unsigned char) instruction->binOpcode[0] == 0x6B){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// MUL : 69 /r id
	if((unsigned char) instruction->binOpcode[0] == 0x69){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// IN : E5 ib
	if((unsigned char) instruction->binOpcode[0] == 0xe5){

		if(instruction->opcodeOperand[0] == OA_IB ){
			return true;
		}
	}

	// IN : ED
	if((unsigned char) instruction->binOpcode[0] == 0xed){
		return true;
	}

	// INC : FF /0
	if((unsigned char) instruction->binOpcode[0] == 0xFF){

		if(instruction->opcodeOperand[0] == OA_RM_ONLY_0 ){
			return true;
		}
	}

	// INS : 6D
	// INSD : 6D
	if((unsigned char) instruction->binOpcode[0] == 0x6d){
		return true;
	}

	// IRETD : CF
	if((unsigned char) instruction->binOpcode[0] == 0xcf){
		return true;
	}

	// JECXZ : E3 cb
	if((unsigned char) instruction->binOpcode[0] == 0xe3){

		if(instruction->opcodeOperand[0] == OA_CB){
			return true;
		}
	}

	// JRCXZ : E3 cb (looks like a duplicate)
	if((unsigned char) instruction->binOpcode[0] == 0xe3){

		if(instruction->opcodeOperand[0] == OA_CB){
			return true;
		}
	}

	// JPE : 7A cb
	if((unsigned char) instruction->binOpcode[0] == 0x7a){

		if(instruction->opcodeOperand[0] == OA_CB){
			return true;
		}
	}

	// JMP : FF /4
	if((unsigned char) instruction->binOpcode[0] == 0xFF){

		if(instruction->opcodeOperand[0] == OA_RM_ONLY_4){
			return true;
		}
	}

	// JMP : FF /5
	if((unsigned char) instruction->binOpcode[0] == 0xff){

		if(instruction->opcodeOperand[0] == OA_RM_ONLY_5){
			return true;
		}
	}

	// LAR : 0F 02 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x02){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// LDS : C5 /r
	if((unsigned char) instruction->binOpcode[0] == 0xc5){

		if(instruction->opcodeOperand[0] == OA_MODRM){
			return true;
		}
	}

	// LSS : 0F B2 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x82){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// LES : C4 /r
	if((unsigned char) instruction->binOpcode[0] == 0xc4){
		if(instruction->opcodeOperand[0] == OA_MODRM){
			return true;
		}
	}

	// LFS : 0F B4 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0xB4){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// LGS : 0F B5 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0xB5){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// LEA : 8D /r
	if((unsigned char) instruction->binOpcode[0] == 0x8d){
		if(instruction->opcodeOperand[0] == OA_MODRM){
			return true;
		}
	}

	// LEAVE : C9
	if((unsigned char) instruction->binOpcode[0] == 0xc9){
		return true;
	}

	// LODS : AD
	if((unsigned char) instruction->binOpcode[0] == 0xc4){
			return true;
	}

	// LSL : 0F 03 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x03){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// MOV : 89 /r
	if((unsigned char) instruction->binOpcode[0] == 0x89){
		if(instruction->opcodeOperand[0] == OA_MODRM){
			return true;
		}
	}

	// MOV : 8B /r
	if((unsigned char) instruction->binOpcode[0] == 0x8b){
		if(instruction->opcodeOperand[0] == OA_MODRM){
			return true;
		}
	}

	// MOV : A1
	if((unsigned char) instruction->binOpcode[0] == 0xa1){
			return true;
	}

	// MOV : A3
	if((unsigned char) instruction->binOpcode[0] == 0xa3){
		return true;
	}

	// MOV : C7 /0
	if((unsigned char) instruction->binOpcode[0] == 0xc7){
		if(instruction->opcodeOperand[0] ==  OA_RM_ONLY_0){
			return true;
		}
	}

	// MOV : 0F 20 /0
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x20){

		if(instruction->opcodeOperand[0] == OA_RM_ONLY_0){
			return true;
		}

	}

	// MOV : 0F 20 /2
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x20){

		if(instruction->opcodeOperand[0] == OA_RM_ONLY_2 ){
			return true;
		}
	}

	// MOV : 0F 20 /3
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x20){

		if(instruction->opcodeOperand[0] == OA_RM_ONLY_3 ){
			return true;
		}
	}
	// MOV : 0F 22 /0
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x22){

		if(instruction->opcodeOperand[0] == OA_RM_ONLY_0 ){
			return true;
		}
	}

	// MOV : 0F 22 /2
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x22){

		if(instruction->opcodeOperand[0] == OA_RM_ONLY_2 ){
			return true;
		}
	}

	// MOV : 0F 22 /3
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x22){

		if(instruction->opcodeOperand[0] == OA_RM_ONLY_3 ){
			return true;
		}
	}

	// MOV : 0F 22 /4
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x22){

		if(instruction->opcodeOperand[0] == OA_RM_ONLY_4 ){
			return true;
		}
	}

	// MOV : 0F 21 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x21){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// MOV : 0F 23 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x23){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// MOV : B8
	if((unsigned char) instruction->binOpcode[0] == 0xb8){
		return true;
	}


	// MOVS : A5
	if((unsigned char) instruction->binOpcode[0] == 0xa5){
		return true;
	}

	// MOVSX : 0F BE /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0xbe){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}

	}

	// MOVZX : 0F B6 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0xb6){

		if(instruction->opcodeOperand[0] == OA_MODRM ){
			return true;
		}
	}

	// MUL : F7 /4
	if((unsigned char) instruction->binOpcode[0] == 0xf7){
		if(instruction->opcodeOperand[0] == OA_RM_ONLY_4){
			return true;
		}
	}

	// NEG : F7 /3
	if((unsigned char) instruction->binOpcode[0] == 0xf7){
		if(instruction->opcodeOperand[0] == OA_RM_ONLY_3){
			return true;
		}
	}

	// NOP : 0F 1F /0
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0x1f){

		if(instruction->opcodeOperand[0] == OA_RM_ONLY_0 ){
			return true;
		}

	}

	// NOT : F7 /2
	if((unsigned char) instruction->binOpcode[0] == 0xf7){
		if(instruction->opcodeOperand[0] == OA_RM_ONLY_2){
			return true;
		}
	}

	// OR : 0c ib
	if((unsigned char) instruction->binOpcode[0] == 0x0d){
			return true;
	}

	// OR : 83 /1 ib
	if((unsigned char) instruction->binOpcode[0] == 0x83){
		if(instruction->opcodeOperand[0] == OA_RM_ONLY_1){
			return true;
		}
	}

	// OR : 09 /r
	if((unsigned char) instruction->binOpcode[0] == 0x09){
		if(instruction->opcodeOperand[0] == OA_MODRM){
			return true;
		}
	}

	// OR : 0B /r
	if((unsigned char) instruction->binOpcode[0] == 0x0b){
		if(instruction->opcodeOperand[0] == OA_MODRM){
			return true;
		}
	}

	// OUT : E7 ib
	if((unsigned char) instruction->binOpcode[0] == 0xe7){
		if(instruction->opcodeOperand[0] == OA_IB){
			return true;
		}
	}

	// OUT : EF
	if((unsigned char) instruction->binOpcode[0] == 0xef){
			return true;
	}

	// OUTS : 6F
	if((unsigned char) instruction->binOpcode[0] == 0x6f){
			return true;
	}

	// POP : 8F /0
	if((unsigned char) instruction->binOpcode[0] == 0x8f){
		if(instruction->opcodeOperand[0] == OA_RM_ONLY_0){
			return true;
		}
	}

	// POP : 8F /0
	if((unsigned char) instruction->binOpcode[0] == 0x8f){
		if(instruction->opcodeOperand[0] == OA_RM_ONLY_0){
			return true;
		}
	}

	// POP : 58 +rd
	if((unsigned char) instruction->binOpcode[0] == 0x58){
		return true;
	}

	// POP : 0F A1
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0xa1){
			return true;
	}

	// POP : 0F A9
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0xa9){
			return true;
	}

	// POPAD : 61
	if((unsigned char) instruction->binOpcode[0] == 0x61){
		return true;
	}

	// PUSH : FF /6
	if((unsigned char) instruction->binOpcode[0] == 0xff){
		if(instruction->opcodeOperand[0] == OA_RM_ONLY_6){
			return true;
		}
	}

	// PUSH : 50+rd
	if((unsigned char) instruction->binOpcode[0] == 0x50){
		return true;
	}

	// PUSH : 68
	if((unsigned char) instruction->binOpcode[0] == 0x68){
		return true;
	}

	// PUSH : 0F A0
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0xa0){
			return true;
	}

	// PUSH : 0F A8
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0F &&
	(unsigned char) instruction->binOpcode[1] == 0xa8){
			return true;
	}

	// PUSHAD : 60
	if((unsigned char) instruction->binOpcode[0] == 0x68){
		return true;
	}

	// PUSHFQ : 9C
	if((unsigned char) instruction->binOpcode[0] == 0x9c){
		return true;
	}

	// ROL : C1 /0 ib
	if((unsigned char) instruction->binOpcode[0] == 0xff){
		if(instruction->opcodeOperand[0] == OA_RM_ONLY_0){
			return true;
		}
	}

	// REP INS : F3 6C
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0xf3 &&
	(unsigned char) instruction->binOpcode[1] == 0x6c){
			return true;
	}

	// REP INS : F3 6D
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0xf3 &&
	(unsigned char) instruction->binOpcode[1] == 0x6d){
			return true;
	}

	// REP MOVS : F3 A5
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0xf3 &&
	(unsigned char) instruction->binOpcode[1] == 0xa5){
			return true;
	}

	// REP OUTS : F3 6F
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0xF3 &&
	(unsigned char) instruction->binOpcode[1] == 0x6f){
			return true;
	}

	// REP LODS : F3 AD
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0xf3 &&
	(unsigned char) instruction->binOpcode[1] == 0xad){
			return true;
	}

	// REP STOS : F3 AB
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0xf3 &&
	(unsigned char) instruction->binOpcode[1] == 0xab){
			return true;
	}

	// REPE CMPS : F3 A7
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0xf3 &&
	(unsigned char) instruction->binOpcode[1] == 0xa7){
			return true;
	}

	// REPE SCAS : F3 AF
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0xf3 &&
	(unsigned char) instruction->binOpcode[1] == 0xaf){
			return true;
	}

	// REPNE CMPS : F2 A7
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0xf2 &&
	(unsigned char) instruction->binOpcode[1] == 0xa7){
			return true;
	}

	// REPNE SCAS : F2 AF
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0xf2 &&
	(unsigned char) instruction->binOpcode[1] == 0xaf){
			return true;
	}

	// SBB : 81 /3 ib
	if((unsigned char) instruction->binOpcode[0] == 0x81){
		if(instruction->opcodeOperand[0] == OA_RM_ONLY_3){
			return true;
		}
	}

	// SBB : 1d +rd
	if((unsigned char) instruction->binOpcode[0] == 0x1d){
		return true;
	}

	// SBB : 83 /3 ib
	if((unsigned char) instruction->binOpcode[0] == 0x83){
		if(instruction->opcodeOperand[0] == OA_RM_ONLY_3){
			return true;
		}
	}

	// SBB : 19 /r
	if((unsigned char) instruction->binOpcode[0] == 0x19){
		if(instruction->opcodeOperand[0] == OA_MODRM){
			return true;
		}
	}

	// SBB : 1B /r
	if((unsigned char) instruction->binOpcode[0] == 0x1b){
		if(instruction->opcodeOperand[0] == OA_MODRM){
			return true;
		}
	}

	// SCAS : AF
	if((unsigned char) instruction->binOpcode[0] == 0xaf){
		return true;
	}

	// SETPE : 0F 9A
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	(unsigned char) instruction->binOpcode[1] == 0x9a){
			return true;
	}


	// SMSW : 0F 01 /4
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0xf2 &&
	(unsigned char) instruction->binOpcode[1] == 0xa7){

		if(instruction->opcodeOperand[0] == OA_RM_ONLY_4)
			return true;

	}

	// STOS : AB
	if((unsigned char) instruction->binOpcode[0] == 0xab){
		return true;
	}

	// SUB : 83 /5 ib
	if((unsigned char) instruction->binOpcode[0] == 0x83){
		if(instruction->opcodeOperand[0] == OA_RM_ONLY_5){
			return true;
		}
	}

	// SUB : 29 /r
	if((unsigned char) instruction->binOpcode[0] == 0x29){
		if(instruction->opcodeOperand[0] == OA_MODRM){
			return true;
		}
	}

	// SUB : 2B /r
	if((unsigned char) instruction->binOpcode[0] == 0x2b){
		if(instruction->opcodeOperand[0] == OA_MODRM){
			return true;
		}
	}

	// SUB : 2D iw
	if((unsigned char) instruction->binOpcode[0] == 0x2d){
			return true;
	}


	// TEST : 85 /r
	if((unsigned char) instruction->binOpcode[0] == 0x85){
		if(instruction->opcodeOperand[0] == OA_MODRM){
			return true;
		}
	}

	// FWAIT : 9B
	if((unsigned char) instruction->binOpcode[0] == 0x9b){
		return true;
	}

	// XADD : 0F C1 /r
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	(unsigned char) instruction->binOpcode[1] == 0xc1){

		if(instruction->opcodeOperand[0] == OA_RM_ONLY_4)
			return true;

	}


	// XCHG : 86 /r
	if((unsigned char) instruction->binOpcode[0] == 0x86){
		if(instruction->opcodeOperand[0] == OA_MODRM){
			return true;
		}
	}

	// XCHG : 87 /r
	if((unsigned char) instruction->binOpcode[0] == 0x87){
		if(instruction->opcodeOperand[0] == OA_MODRM){
			return true;
		}
	}

	// XLATB : D7
	if((unsigned char) instruction->binOpcode[0] == 0xd7){
		return true;
	}

	// XOR : 83 /6 ib
	if((unsigned char) instruction->binOpcode[0] == 0x83){
		if(instruction->opcodeOperand[0] == OA_RM_ONLY_6){
			return true;
		}
	}

	// XOR : 31 /r
	if((unsigned char) instruction->binOpcode[0] == 0x31){
		if(instruction->opcodeOperand[0] == OA_MODRM){
			return true;
		}
	}

	// XOR : 33 /r
	if((unsigned char) instruction->binOpcode[0] == 0x33){
		if(instruction->opcodeOperand[0] == OA_MODRM){
			return true;
		}
	}

	// VMREAD : 0F 78
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	(unsigned char) instruction->binOpcode[1] == 0x78){

		return true;

	}

	// VMWRITE : 0F 79
	if(instruction->binOpcodeLen > 1)
	if((unsigned char) instruction->binOpcode[0] == 0x0f &&
	(unsigned char) instruction->binOpcode[1] == 0x79){

			return true;

	}

	// CMP : 83 /7 ib
	if((unsigned char) instruction->binOpcode[0] == 0x83){
		if(instruction->opcodeOperand[0] == OA_RM_ONLY_7){
			return true;
		}
	}

	// CMP : 39 /r
	if((unsigned char) instruction->binOpcode[0] == 0x39){
		if(instruction->opcodeOperand[0] == OA_MODRM){
			return true;
		}
	}

	// CMP : 3B /r
	if((unsigned char) instruction->binOpcode[0] == 0x33){
		if(instruction->opcodeOperand[0] == OA_MODRM){
			return true;
		}
	}

	return false;
}


// determines if an operand is a memory only operand
size_t X86_32_IsOperandMemOnly(size_t operand){
	// Anomolies first: Not found in the instruction information
	switch(operand){

			// 2 byte memory operand
			case IA_M2BYTE:
				return true;

			// anomolies
			case IA_M14_28BYTE:
				return true;
			case IA_M512BYTE:
				return true;
			case IA_M80:
				return true;
			case IA_M80BCD:
				return true;
			case IA_M94_108BYTE:
				return true;
			case IA_MEM:
				return true;
			// ptr operands
			case IA_M16_16:
				return true;
			case IA_M16_32:
				return true;
			case IA_M16_64:
				return true;
			// (m) Operand in memory.
			case IA_M:
				return true;
			case IA_M8:
				return true;
			case IA_M16:
				return true;
			case IA_M32:
				return true;
			case IA_M64:
				return true;
			case IA_M128:
				return true;

			// (m16&32) Memory operand consistin of data items pairs.
			case IA_M16_AND_32:
				return true;
			case IA_M16_AND_16:
				return true;
			case IA_M32_AND_32:
				return true;
			case IA_M16_AND_64:
				return true;

			// (m32fp) Scaled precision floating point operands in memory.
			case IA_M32FP:
				return true;
			case IA_M64FP:
				return true;
			case IA_M80FP:
				return true;

			// (m16int) Integer operand in memory. Used as operands for x87.
			case IA_M16INT:
				return true;
			case IA_M32INT:
				return true;
			case IA_M64INT:
				return true;
			default:
					break;
		}

	return false;
}

// determines if an operand is a static register
size_t X86_32_IsOperandStaticRegOnly(size_t operand){
	switch(operand){
			// Static Registers
			case IA_EAX:
				return true;
			case IA_AL:
				return true;
			case IA_AX:
				return true;
			case IA_CL:
				return true;
			case IA_DX:
				return true;
			case IA_CS:
				return true;
			case IA_DS:
				return true;
			case IA_ES:
				return true;
			case IA_FS:
				return true;
			case IA_GS:
				return true;
			case IA_SS:
				return true;
			default:
				return false;
	}
	return false;
}


// determines if an operand is a register only operand
size_t X86_32_IsOperandRegOnly(size_t operand){

	switch(operand){
		/*
		// Static Registers

		case IA_EAX:
			return true;
		case IA_AL:
			return true;
		case IA_AX:
			return true;
		case IA_CL:
			return true;
		case IA_DX:
			return true;
		case IA_CS:
			return true;
		case IA_DS:
			return true;
		case IA_ES:
			return true;
		case IA_FS:
			return true;
		case IA_GS:
			return true;
		case IA_SS:
			return true;
		*/
		// (r8) One of the general purpose registers
		case IA_R8:
			return true;
		case IA_R16:
			return true;
		case IA_R32:
			return true;
		case IA_R64:
			return true;

		// (mm) MMX Register. MM0 - MM7
		case IA_MM:
			return true;

		// (xmm) A XMM Register (128-bit)
		case IA_XMM:
			return true;
		default:
				break;
	}

	return false;
}

// Determines if an operand is a control register
size_t X86_32_IsOperandControlRegister(size_t operand){

	switch(operand){
		// CR Registers
		case IA_CR_0:
			return true;
		case IA_CR_1:
			return true;
		case IA_CR_2:
			return true;
		case IA_CR_3:
			return true;
		case IA_CR_4:
			return true;
		case IA_CR_5:
			return true;
		case IA_CR_6:
			return true;
		case IA_CR_7:
			return true;
		default:
				break;
	}

	return false;
}

// determines if an operand is of an R/M Type
size_t X86_32_IsOperandRm(size_t operand){

	switch(operand){
		case IA_R16_M16:
			return true;
		case IA_R32_M16:
			return true;

		// (r/m8) Either a register, or a memory destination (RM)
		case IA_RM8:
			return true;
		case IA_RM16:
			return true;
		case IA_RM32:
			return true;
		case IA_RM64:
			return true;

		// (mm/m32) Low order bits of a MMX Register or Memory operand.
		case IA_MM_M32:
			return true;
		case IA_MM_M64:
			return true;

		// (xmm/m32) XMM Register or Memory operand.
		case IA_XMM_M32:
			return true;
		case IA_XMM_M64:
			return true;
		case IA_XMM_M128:
			return true;
		default:
			break;
	}

	return false;
}

// determines if an operand is ptr Type aka [16bit][32bit]
size_t X86_32_IsOperandPointer(size_t operand){

	switch(operand){
		case IA_PTR16_16:
			return true;
		case IA_PTR16_32:
			return true;
		case IA_PTR16_64:
			return true;
		default:
			break;
	}

	return false;
}

// Tests an operand and determines if an operand is a rel type
size_t X86_32_IsOperandRel(size_t operand){
	switch(operand){
		case IA_REL8:
			return true;
		case IA_REL16:
			return true;
		case IA_REL32:
			return true;
		case IA_REL64:
			return true;
		default:
			break;
	}

	return false;
}

// Check for immediate
size_t X86_32_IsOperandImmediate(size_t operand){
	switch(operand){
		case IA_IMM8:
			return true;
		case IA_IMM16:
			return true;
		case IA_IMM32:
			return true;
		case IA_IMM64:
			return true;
	}
	return false;
}


// Tests to determine if the operand is a debug register
size_t X86_32_IsOperandDebugRegister(size_t operand){

	if(operand == IA_DR0_DR7)
		return true;

	return false;
}



// These functions are used to get the typeclass of a register or rm value,
// out of a rm byte. Typeclasses are defined as follows.
size_t X86_32_GetRmRegTypeClass(size_t operand){

	// TYPECLASS_REG_NONE
	// TYPECLASS_REG_DEBUG
	// TYPECLASS_REG_CONTROL
	// TYPECLASS_REG_GPR_8
	// TYPECLASS_REG_GPR_16
	// TYPECLASS_REG_GPR_32
	// TYPECLASS_REG_ST

	// Anomolies first: Not found in the instruction information
	switch(operand){

		/*
		 * First all possible REG values
		 */

		// (r8) One of the general purpose registers
		case IA_R8:
			return TYPECLASS_REG_GPR_8;
		case IA_R16:
			return TYPECLASS_REG_GPR_16;
		case IA_R32:
			return TYPECLASS_REG_GPR_32;
		case IA_R64:
			return TYPECLASS_REG_GPR_64;

		// (mm) MMX Register. MM0 - MM7
		case IA_MM:
			return TYPECLASS_REG_MM;
		case IA_MM32:
			return TYPECLASS_REG_MM;

		// (xmm) A XMM Register (128-bit)
		case IA_XMM:
			return TYPECLASS_REG_XMM;


	}
	return 0;
}

#endif

#endif // USE_BLOCKPARTY_DEBUGGING == 1

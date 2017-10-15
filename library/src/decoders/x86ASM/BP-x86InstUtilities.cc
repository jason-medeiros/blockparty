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

// Returns an appropriate size_t representing the number
// of instructions in an instruction list.  Returns 0 on failure or
// no elements.  Resets list to first pointer, then counts.
size_t X86_32_GetInstructionCountFromFirst(P_X86_32_DECODED_INSTRUCTION_LIST instructions){

	// return 0 if the list is null
	if(instructions == NULL)
		return 0;

	size_t count = 0;
	for(instructions = instructions->first; instructions != NULL; instructions = instructions->next){

		// increment count per each loop
		count++;

		// exit if next is null
		if(instructions->next == NULL){
			break;
		}
	}
	return count;
}

// Returns an appropriate size_t representing the number
// of instructions in an instruction list.  Returns 0 on failure or
// no elements.  Does not reset first pointer before count.
size_t X86_32_GetInstructionCount(P_X86_32_DECODED_INSTRUCTION_LIST instructions){

	// return 0 if the list is null
	if(instructions == NULL)
		return 0;

	size_t count = 0;
	for(; instructions != NULL; instructions = instructions->next){

		// increment count per each loop
		count++;

		// exit if next is null
		if(instructions->next == NULL){
			break;
		}
	}
	return count;
}


// Retrieves an instruction element by a number offset into the list.  Will
// never return a list pointer greater then max count.  Does not reset list to first
// position. Returns NULL for failure.  0 is first position in the list.
P_X86_32_DECODED_INSTRUCTION_LIST X86_32_GetInstructionAtListPosition(P_X86_32_DECODED_INSTRUCTION_LIST instructions, size_t position){

	size_t elementCount = X86_32_GetInstructionCountFromFirst(instructions);
	if(position > elementCount)
		return NULL;

	size_t positionCounter = 0;

	printf("\n WTF INSTRUCTION TOP: %p\n", instructions->first);
	printf("\n WTF INSTRUCTION TOP: %p\n", instructions->first);
	printf("\n WTF INSTRUCTION TOP: %p\n", instructions->first);

	// loop through the list till you find the position
	for(instructions=instructions->first;instructions != NULL;){
		if(positionCounter == position)
			return instructions;
		positionCounter++;

		instructions = instructions->next;
	}

	return NULL;
}



// Prints out an entire decoded instruction list
int X86_32_PrintDecodedInstructionList(P_X86_32_DECODED_INSTRUCTION_LIST instList){

	for(instList = instList->first; instList != NULL; instList = instList->next){

		if(instList->instruction != NULL)
			X86_32_PrintDecodedInstructionStruct(instList);
		else
			printf("\n Bad instruction found.");

		if(instList->next == NULL)
			break;
	}

	return ERR_SUCCESS;
}

// Prints out all instructions in a list as if they were intel instructions
int X86_32_PrintInstructionsINTEL(P_X86_32_DECODED_INSTRUCTION_LIST instList){


	for(instList = instList->first; instList != NULL; instList = instList->next){

		if(instList->instruction != NULL){
			printf("\n%.08x: \t%s \t %s", (size_t) instList->address, instList->instruction->instructionStr, instList->intelDecode);
		} else {
			printf("\n%.08x: \tBad instruction.", (size_t) instList->address);
		}

		if(instList->next == NULL)
			break;
	}

	return ERR_SUCCESS;
}



// An formatted null terminated buffer containing a buffer full of instructions
// as they would be formatted by the print function.  This is a utility function
// used to generate buffers for display in various widgets.
char * X86_32_GenerateInstructionsBufferINTEL(P_X86_32_DECODED_INSTRUCTION_LIST instList){

	char *outBuff = NULL;
	int i = 0;
	for(instList = instList->first; instList != NULL; instList = instList->next){

		// generate arbitrary space for buffer line
		i+=100;
		outBuff = (char *) bprealloc(outBuff, i);

		// zero out new memory
		memset(&outBuff[i-100], 0x00, 100);


		if(instList->instruction != NULL){
			snprintf(&outBuff[strlen(outBuff)], 100, "\n%.08x: \t%s \t %s", (size_t) instList->address, instList->instruction->instructionStr, instList->intelDecode);
		} else {
			snprintf(&outBuff[strlen(outBuff)], 100, "\n%.08x: \tBad instruction.", (size_t) instList->address);
		}


		if(instList->next == NULL)
			break;
	}

	// return the newly formatted buffer
	return outBuff;
}




// Prints out a decoded instruction structure
int X86_32_PrintDecodedInstructionStruct(P_X86_32_DECODED_INSTRUCTION instruction){

	printf("\n[+]-------------------------------------------------------[+]");
	printf("\n Decoded Instruction: %p", instruction);
	switch(instruction->valid){
		case 0:
			printf("\n\n\t Validity: Valid");
			break;
		default:
			printf("\n\n\t Validity: Invalid");
	}

	if(instruction->address != NULL){
		printf("\n\t Extracted From Address: 0x%p", instruction->address);
	}

	printf("\n\t Instruction Length: %u", instruction->totalLength);

	// Contains SIB Byte
	printf("\n\n\t Contains SIB: ");
	if(instruction->containsSib > 0){
		printf("yes [%.02x]", instruction->sib);
	} else {
		printf("no");
	}

	printf("\n\t Contains Displacement: ");
	if(instruction->containsDisp > 0){
		printf("yes");
		printf("\n\t Displacement Size: %.02x", instruction->dispSize);
		printf("\n\t Displacement Value: %x", instruction->dispVal);
	} else {
		printf("no");
	}


	printf("\n\t Contains Immediate: ");
	if(instruction->containsImm > 0){
		printf("yes");
		printf("\n\t Immediate Size: %.02x", instruction->immSize);
		printf("\n\t Immediate Value: %x", instruction->immVal);
	} else {
		printf("no");
	}


	printf("\n\t Contains ModRM:");
	if(instruction->containsModRM > 0){
		printf(" yes [%.02x]", instruction->modRM);
		// print out modrm values
		printf("\n\t\t[mod] %.02x [reg] %.02x [rm] %.02x", instruction->mod, instruction->reg, instruction->rm);
		printf("\n\t\t Reg Decoded Str: %s", instruction->regStr);
		printf("\n\t\t Rm  Decoded Str: %s", instruction->rmStr);
	} else {
		printf(" no");
	}


	printf("\n\n\t[Prefixes]-------------------------------------");
	int i = 0;
	while(instruction->prefixes[i].prefixStr != NULL){
		printf("\n\t| [+] %p", instruction->prefixes[i].prefixStr);
		i++;
	}
	printf("\n\n\t-----------------------------------------------");

	printf("\n\n\t[ Instruction Table Decode ]");
	if(instruction->instruction != NULL){
		X86_32_PrintInstructionStruct(instruction->instruction);
	}

	// list pointers
	printf("\n\n\t-----------------------------------------------");
	printf("\n\tFirst:    %p", instruction->first);
	printf("\n\tPrevious: %p", instruction->prev);
	printf("\n\tNext:     %p", instruction->next);

	return 0;
}

// Prints out a single instruction counter
int X86_32_PrintInstructionStruct(P_X86_32_INSTRUCTION instruction){

	if(instruction == NULL){
		printf("\n\tInstruction: NULL");
		return -1;
	} else {
		printf("\n\tInstruction: %s", instruction->instructionStr);
	}

	if(instruction->dstOperandStr != NULL){
		printf("\n\t\tDest Operand: %s", instruction->dstOperandStr);
	}
	if(instruction->srcOperandStr != NULL){
		printf("\n\t\tSrc Operand: %s", instruction->srcOperandStr);
	}
	if(instruction->thirdOperandStr != NULL){
		printf("\n\t\tThird Operand: %p", instruction->thirdOperandStr);
	}

	if(instruction->valid64 == true){
		printf("\n\t\t\tValid64: yes");
	} else {
		printf("\n\t\t\tValid64: no");
	}

	if(instruction->compatLeg == true){
		printf("\n\t\t\tLegacy Compatible: yes");
	} else {
		printf("\n\t\t\tLegacy Compatible: no");
	}

	if(instruction->VMX == true){
		printf("\n\t\t\tVMX: yes");
	} else {
		printf("\n\t\t\tVMX: no");
	}

	if(instruction->description!=NULL){
		printf("\n Description: %s", instruction->description);
	}


	return 0;
}

#endif // 32bit

#endif USE_BLOCKPARTY_DEBUGGING == 1

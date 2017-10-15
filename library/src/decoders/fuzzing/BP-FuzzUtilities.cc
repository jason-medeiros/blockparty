/***************************************************************************
 *            BP-FuzzUtilities.cc
 *
 *  Wed Nov 15 10:32:32 2006
 *  Copyright  2006  User
 *  Email
 ****************************************************************************/
#include "../../../include/BP-Main.h"

// Generate A random ammount of bytes
P_BP_DATA_LIST CreateRandomList(int size){
	
	P_BP_DATA_LIST outList = NULL;

	// return the random list element
	return outList;
}


// recreates all of the BP_DATA_LIST_TYPE_RANDOM typed elements in the
// data list.  This function broke my brain.  Fix it later.
int RecreateRandomElements(P_BP_DATA_LIST inList)
{


	// return indicating success
	return ERR_SUCCESS;
	
}

// creates a list containing `int size` bytes worth of format characters 
// (note: if the size is not divisible by two, it is incremented by 1
// to allow for normal 2byte format string alignment)
P_BP_DATA_LIST CreateFormatSpecifierList(int size, char specifier)
{

	P_BP_DATA_LIST outList = NULL;

	return outList;

}

// creates a list containing count number of sequences at seqLen
// bytes each (seqLen is required considering binary (nonascii) sequences)
P_BP_DATA_LIST CreateSequenceList(int count, char *seq, int seqLen)
{
	
	P_BP_DATA_LIST outList = NULL;

	return outList;
}

// Writes 'length' number of bytes from 'inBuff' and stores them in a buffer list
P_BP_DATA_LIST DL_CreateBinaryListElement(unsigned char *inBuff, size_t length)
{
	
	P_BP_DATA_LIST outList = NULL;

	return outList;

}


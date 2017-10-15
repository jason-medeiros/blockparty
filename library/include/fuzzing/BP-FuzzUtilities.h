/***************************************************************************
 *            BP-FuzzUtilities.h
 *
 *  Wed Nov 15 10:32:22 2006
 *  Copyright  2006  User
 *  Email
 ****************************************************************************/
// creates a list containing `int size` number of random bytes
P_BP_DATA_LIST CreateRandomList(int size);


// recreates all of the RANDOM typed elements in the
// data list.
int RecreateRandomElements(P_BP_DATA_LIST inList);

// creates a list containing `int size` bytes worth of format characters 
// (note: if the size is not divisible by two, it is incremented by 1
// to allow for normal 2byte format string alignment)
P_BP_DATA_LIST CreateFormatSpecifierList(int size, char specifier);

// creates a list containing count number of sequences at seqLen
// bytes each (seqLen is required considering binary (nonascii) sequences)
P_BP_DATA_LIST CreateSequenceList(int count, char *seq, int seqLen);

// creates a list element containing a binary buffer
P_BP_DATA_LIST DL_CreateBinaryListElement(unsigned char *inBuff, size_t length);

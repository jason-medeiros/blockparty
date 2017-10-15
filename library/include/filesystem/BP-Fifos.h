/***************************************************************************
 *            BP-Fifos.h
 *
 *  Mon Jan 27 21:25:05 2003
 *  Copyright  2003  User
 *  Email
 ****************************************************************************/

// Todo:  These routines are some of the oldest in the library and are in need
//        of stylistic updates.
			
// opens a fifo pipe
FILE * BP_FifoOpen(char *rootDirPath);

// creates a new fifo pipe
FILE * BP_FifoCreate(char *rootDirPath);

// writes to a fifo pipe
int BP_FifoWrite(FILE * fifo, char *buff, int size);
			
// reads from a fifo pipe
int BP_FifoRead(FILE *fifo, char *buff, int size);

// closes fifo
int BP_FifoClose(FILE * fifo);

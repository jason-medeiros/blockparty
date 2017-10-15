/***************************************************************************
 *            BP-Fifos.cc
 *
 *  Mon Jan 27 21:25:42 2003
 *  Copyright  2003  User
 *  Email
 ****************************************************************************/

#include "../../include/BP-Main.h"




// opens a fifo pipe
FILE * BP_FifoOpen(char *rootDirPath)
{

	// ensure root dir path is set and reasonable
	if(!rootDirPath)
		return NULL;
	if(!BP_StringIsReasonableForUnixEXT4Directory(rootDirPath, bpstrlen(rootDirPath), BP_FALSE))
		return NULL;

	// fifo pointer
	FILE * fifo = NULL;

	// open fifo
	fifo = fopen(rootDirPath, "rw");

	// return the fifo
	return fifo;
}

// creates a new fifo on the filesystem and opens it for read/write
// operations.
FILE * BP_FifoCreate(char *rootDirPath)
{

	if(!rootDirPath)
		return NULL;
	if(!BP_StringIsReasonableForUnixEXT4Directory(rootDirPath, bpstrlen(rootDirPath), BP_FALSE))
		return NULL;

	FILE *fifo = NULL;
	int err = 0;
	
	// make fifo read/write
	err = mkfifo(rootDirPath, S_IREAD | S_IWRITE);
	if(err == EEXIST || err == ENOSPC || EROFS)
		return NULL;
	
	fifo = BP_FifoOpen(rootDirPath);
	return fifo;

}


// writes to a fifo pipe
int BP_FifoWrite(FILE * fifo, char *buff, int size)
{

	// ensure we have valid write semantics
	if(!fifo || !buff || !size )
		return ERR_FAILURE;

	// perform the actual write
	int sizeOut = fwrite(buff, 1, size, fifo);

	// exit indicating success
	return sizeOut;

}
			
// reads from a fifo pipe
int BP_FifoRead(FILE *fifo, char *buff, int size)
{
	if(!fifo || !buff || !size)
		return 0;

	int sizeOut = fread(buff, 1, size, fifo);
	return sizeOut;
}

// closes the fifo
int BP_FifoClose(FILE * fifo)
{

	// ensure we have a fifo
	if(!fifo)
		return ERR_FAILURE;

	// close
	int err = fclose(fifo);
	if(err = EOF)
		return ERR_FAILURE;
	
	// return indicating success
	return ERR_SUCCESS;

}

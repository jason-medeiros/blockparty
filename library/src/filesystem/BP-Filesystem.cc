/***************************************************************************
 *            BP-Filesystem.cc
 *
 *  Mon Jan 27 21:56:29 2003
 *  Copyright  2003  User
 *  Email
 ****************************************************************************/

#include "../../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %% File List for Linear Traversals %%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Retrieves all of TYPE files from the filesystem, useful for scouring the filesystem
// for rogue fifos or other file types.  Elf's too.. 
P_BP_FILE_LIST BP_FSFindAllType(char *rootDirPath, BP_FILETYPE type, bool recursive, P_BP_FILE_LIST prev)
{

	// ensure we have a valid root directory path
	if(!rootDirPath)
		return NULL;

	// ensure path is reasonable pringable
	if(!BP_StringIsReasonablePrintableString(rootDirPath, bpstrlen(rootDirPath), BP_FALSE, BP_FALSE))
		return NULL;

	// directory info
	DIR * dirPointer         = NULL;
	struct dirent * dirEntry = NULL;
	char *recurseDir         = NULL;
	
	// file list 
	P_BP_FILE_LIST fileList= NULL;
	
	// increment to the end of the previous list if its not null
	if(prev != NULL)
	{

		// iterate to end of the list
		for
		(
				prev = prev->first;
				prev->next;
				prev = prev->next
		){}

		// set list as the end of the list
		fileList = prev;

	}
	else
	{
		fileList = BP_NewFileList(NULL, NULL, NULL);
	}
	
	// set errno to zero
	errno = 0;

	// open the directory
	dirPointer = opendir(rootDirPath);
	
	// check to ensure we opened the pointer ok
	if(dirPointer)
	{

		// read directory entries
		while
		(
			(dirEntry = readdir(dirPointer))
		)
		{
			
			// continue the loop if its either current or previous directory
			if
			(
				memcmp(dirEntry->d_name, ".",  bpstrlen(dirEntry->d_name))==0 ||
				memcmp(dirEntry->d_name, "..", bpstrlen(dirEntry->d_name))==0
			)
			{

					continue;
			}


			// Found File: Creating new P_BP_FILE_LIST object
			if(dirEntry->d_type == type)
			{

				/*
					char *pathToFile;
					FILETYPE type;
					mode_t fileMode;
					bool writable;
				*/
								
				// list operations
				if
				(
					fileList &&
					fileList->pathToFile
				)
				{
					fileList->next = BP_NewFileList(fileList->first, fileList->next, fileList);
					fileList = fileList->next;
				}
				else if(!fileList)
				{
					fileList = BP_NewFileList(NULL, NULL, NULL);
				}
				


				// type assignment
				// fileList->type = dirEntry->d_type;
				
				// allocate room for path - SPAGHETTI CITY ;)
				fileList->pathToFile = (char *) bpmalloc(bpstrlen(rootDirPath)+bpstrlen(dirEntry->d_name)+100);

				// zero out allocated memory
				memset(fileList->pathToFile, 0x00, bpstrlen(rootDirPath)+bpstrlen(dirEntry->d_name)+100);

				// set the path to file
				strcat(fileList->pathToFile, rootDirPath);

				if(fileList->pathToFile[bpstrlen(fileList->pathToFile)] != '/' && memcmp(fileList->pathToFile, "/", bpstrlen(fileList->pathToFile)) != 0)
				{
					strcat(fileList->pathToFile, "/");
				}

				// add entry
				strcat(fileList->pathToFile, dirEntry->d_name);
				
				// set type
				fileList->type = type;
				
				// set permissions
				if(access(fileList->pathToFile, R_OK) == 0)
					fileList->read = true;
		
				if(access(fileList->pathToFile, W_OK) == 0)
					fileList->write = true;
		
				if(access(fileList->pathToFile, X_OK) == 0)
					fileList->execute = true;
		
				// retrieve stat information about the file
				stat(fileList->pathToFile, &fileList->attrs);
								
			}
			
			if(dirEntry->d_type == DT_DIR && recursive == 1)
			{
				
				// allocate buffer before recursion
				recurseDir = (char *) bpmalloc(bpstrlen(rootDirPath)+bpstrlen(dirEntry->d_name)+100);
				memset(recurseDir, 0x00, bpstrlen(rootDirPath)+bpstrlen(dirEntry->d_name)+100);
				
				strcat(recurseDir, rootDirPath);
				// add trailing slash if its not there
				if(recurseDir[bpstrlen(recurseDir)-1] != '/' && memcmp(recurseDir, "/", bpstrlen(recurseDir)) != 0)
				{
					strcat(recurseDir, "/");
				}
				
				if(bpstrlen(dirEntry->d_name) > 0)
					strcat(recurseDir, dirEntry->d_name);
				
				
				// right here we should incorporate a quick hack, to close the dir pointer
				// then right after the call, reopen dir and iterate till our dirent structure
				// contains the right string.
			
				// recurse
				BP_FSFindAllType(recurseDir, type, 1, fileList);
				
				// recursion just returned, you must bpfree the dir pointer before you get here
				
				// bpfree buffer after recursion
				bpfree(recurseDir);

			}
						
		}
		
		// this is where the close should happen i think
		closedir(dirPointer);

	} 
	
	// if we've created a file list, return it
	if(fileList)
		return fileList->first;

	// null on failure
	return NULL;

}

// searches an existing list for the following conditions (note: elf detection must have read permissions on file)
P_BP_FILE_LIST BP_FSSearchAll(P_BP_FILE_LIST inList, char *name, bool execute, bool write, bool read, bool elf, uid_t uid, gid_t gid)
{

	// ensure list pointers are navigatable
	if(!inList)
		return NULL;
	if(!inList->first)
		return NULL;
	
	// local vars
	P_BP_FILE_LIST elfList = NULL;
	FILE * elfFile      = NULL;
	int readBytes       = 0;
	int i               = 0;
		
	// holds signifigant bytes
	char elfBuff[10];
		memset(elfBuff, 0x00, 10);
	
	// walk the list and search for values
	for(inList = inList->first; inList != NULL; inList = inList->next)
	{
		
		// search for name if it's set
		if(name)
		{
			
			// not win32 compatible [search key: portable]
			#ifndef WIN32
			if(inList->pathToFile == NULL)
				continue;
			for(i = bpstrlen(inList->pathToFile); \
				inList->pathToFile[i] != '/'; i--){}
			#endif
					
			#ifdef WIN32
			for(i = bpstrlen(inList->pathToFile); \
				inList->pathToFile != '\\'; i--){}
			#endif
			
			// if it found the last /, iterate it to find the beginning of the
			// file name
			if(i != 0)
				i++;
			
			// compare and continue loop if not the same
			if(memcmp(&inList->pathToFile[i], name, bpstrlen(name)) != 0)
				continue;
			
			if(memcmp(&inList->pathToFile[i], name, bpstrlen(&inList->pathToFile[i])) != 0)
				continue;
				
		} 
		
		
		// continue the loop if the file is not executable
		// and the executable flag has been set to true.
		if(execute == true && inList->execute != true)
			continue;
		
		// continue the loop if the file is not writable
		// and the writable flag has been set to true
		if(write == true && inList->write != true)
			continue;
		
		// continue the loop if the file is not readable
		// and the readable flag has been set to true
		// continue the loop if file is unreadable
		if(read == true && inList->read != true)
			continue;
		
		// continue the loop if the uid does not match
		if(gid != -1 && inList->attrs.st_gid != gid)
			continue;
			
		// continue the loop if the gid does not match
		if(gid != -1 && inList->attrs.st_uid != uid)
			continue;
		
		// check to see if it's elf - note: may be possible to get false
		// comparissons for elf due to the inList->attrs.st_size comparisson
		// below.  Regardless, its pretty safe
		if(elf == true && inList->read == true){
			if(inList->attrs.st_size <= 4)
				continue;
			
			elfFile = fopen(inList->pathToFile, "r");
			
			if(elfFile != NULL){
				readBytes = fread(elfBuff, 1, 4, elfFile);
				fclose(elfFile);
				// 	if its not elf, continue
				if(readBytes == 4 && memcmp(elfBuff, "\x7f\x45\x4c\x46", 4) != 0)
					continue;
			} else continue;
		}
		
		// if it passes all the previous conditions assign the element to outList
		
		// assignments
		if(elfList == NULL)
		{

			elfList = BP_FileListDupElement(inList);
			elfList->first = elfList;
			elfList->prev = NULL;
			elfList->next = NULL;

		}
		else
		{

			elfList->next = BP_FileListDupElement(inList);
			elfList->next->prev = elfList;
			elfList = elfList->next;
			elfList->first = elfList->prev->first;
			elfList->next = NULL;

		}
			
		// reset the elf buffer
		memset(elfBuff, 0x00, 4);
	}
	
	if(elfList != NULL)
		return elfList->first;
	else
		return NULL;
}

// duplicates an element, no pointers are set
P_BP_FILE_LIST BP_FileListDupElement(P_BP_FILE_LIST inList)
{

	P_BP_FILE_LIST outList;
	if(inList == NULL)
		return NULL;
	
	outList = (P_BP_FILE_LIST) bpmalloc(sizeof(BP_FILE_LIST));
	outList->pathToFile = (char *) bpmalloc(bpstrlen(inList->pathToFile)+1);
		strcpy(outList->pathToFile, inList->pathToFile);
	
	// set type
	outList->type = inList->type;
		memcpy(&outList->attrs, &inList->attrs, sizeof(struct stat));
			
	// set rwx
	outList->read = inList->read;
	outList->write = inList->write;
	outList->execute = inList->execute;
	
	// nullify pointers
	outList->first = NULL;
	outList->prev  = NULL;
	outList->next  = NULL;
		
	return outList;
}

// creates a new file list:
P_BP_FILE_LIST BP_NewFileList
(
		P_BP_FILE_LIST first,
		P_BP_FILE_LIST next,
		P_BP_FILE_LIST prev
)
{

	// attempt to create a new list item
	P_BP_FILE_LIST outList = (P_BP_FILE_LIST) bpcalloc(sizeof(BP_FILE_LIST), 1);
	if(!outList)
		return NULL;
	
	// mark first pointer position
	outList->first = first;
	
	// set default first pointer if unset
	if(!outList->first)
		outList->first = outList;


	// set next and prev directly
	outList->next = next;
	outList->prev = prev;
		
	// return the newly generated list
	return outList;
}

// Destroys a file list including all associated memory.
BP_ERROR_T BP_DestroyFileList(P_BP_FILE_LIST inList)
{
	
	// ensure we have an item to destroy
	if(!inList)
		return ERR_FAILURE;

	// create next iterator
	P_BP_FILE_LIST nextList = NULL;
	

	// note: this function is perfect, dont mess with it
	for
	(
		inList = inList->first;
		inList;
	)
	{

		// zero the next list pointer on loop
		nextList = NULL;

		// if there is a next element, preserve it
		if(inList->next)
		{

			// preserve list item
			nextList = inList->next;
			
			// bpfree file name since its dynamically allocated
			if(inList->pathToFile != NULL)
			{
				bpfree(inList->pathToFile);
				inList->pathToFile = NULL;
			}
			
			// destroy the list
			bpfree(inList);

			// set the list position to next in list
			inList = nextList;
			
		}
		else
		{

			// destroy the path to the file
			if(inList->pathToFile != NULL)
				bpfree(inList->pathToFile);
			
			// destroy the list position
			bpfree(inList);
			inList = NULL;
			break;

		}

	}
	
	// return indicating success
	return ERR_SUCCESS;

}

// prints out file list element
int BP_FileListPrintElement(P_BP_FILE_LIST inList)
{
	
	
	// used to see if a structure is filled
	char structTest[sizeof(struct stat)];
	memset(structTest, 0x00, sizeof(struct stat));
	
	
	if(inList == NULL){
		return 0;
	} else {
		printf("\n File List: %p", inList);
	}
	
	// file path
	if(inList->pathToFile != NULL){
		printf("\n\t element->pathToFile: %s", inList->pathToFile);
	} else {
		printf("\n\t element->pathToFile: (nil)");
	}
	
	// file type
	switch(inList->type){
		case DT_UNKNOWN:
			printf("\n\t element->type: DT_UNKNOWN");
			break;
		case DT_REG:
			printf("\n\t element->type: DT_REG");
			break;
		case DT_DIR:
			printf("\n\t element->type: DT_DIR");
			break;
		case DT_FIFO:
			printf("\n\t element->type: DT_FIFO");
			break;
		case DT_SOCK:
			printf("\n\t element->type: DT_SOCK");
			break;
		case DT_CHR:
			printf("\n\t element->type: DT_CHR");
			break;
		case DT_BLK:
			printf("\n\t element->type: DT_BLK");
			break;
		default:
			printf("\n\t element->type: TYPE UNRECOGNIZED");
			break;
	}
	
	// print out stat structure elements
	if(memcmp(structTest, &inList->attrs, sizeof(struct stat)) != 0){
		printf("\n\t element->attrs.st_mode: %x", \
			(size_t) inList->attrs.st_mode);
		printf("\n\t element->attrs.st_uid: %i", \
			inList->attrs.st_uid);
		printf("\n\t element->attrs.st_gid: %i", \
			inList->attrs.st_gid);
		printf("\n\t element->attrs.st_size: %i", \
			(int) inList->attrs.st_size);
	} else {
		printf("\n\t element->attrs == (nil)");
	}
	
	// determine access to file
	printf("\n\t Current User Has Following Permissions: ");

		if(inList->read == true)
			printf(" READ");
		
		if(inList->write == true)
			printf(" WRITE");
		
		if(inList->execute == true)
			printf(" EXECUTE");
		
		
		// prints none if no access exists
		if( inList->read == false && \
			inList->write == false && \
			inList->execute == false)
			printf(" NONE");

	// struct pointers now
	
	printf("\n\t\t element->first: %p", inList->first);
	printf("\n\t\t element->next : %p", inList->next);
	printf("\n\t\t element->prev : %p", inList->prev);

	return 0;
}

// prints out all items in a file list starting at first
int BP_FileListPrintAll(P_BP_FILE_LIST inList)
{

	// ensure we have a valid inptr
	if(!inList)
		return ERR_FAILURE;
	
	// walk list and display elements
	for(inList = inList->first; inList != NULL; inList = inList->next)
		BP_FileListPrintElement(inList);
	
	// return indicating success
	return ERR_SUCCESS;

}




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% File Execution Wrappers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// executes a file given parms, if retFds is true.. returns a structure
// containing the file descriptors which can be read from or written to
P_BP_FDSET BP_FExec
(
		char *rootBinPath,
		char **params,
		char **envp,
		BP_BOOL retFds,
		BP_BOOL fork_pty
)
{

	// ensure we have bin path
	if(!rootBinPath)
		return NULL;

	// if we're forking ptys we must return fds
	if(fork_pty && !retFds)
		return NULL;

	// set of file descriptors stdin/stdout/stderr
	P_BP_FDSET fdset = NULL;

	// declare pid
	int pid = 0;
	
	// If we're returning file descriptors, create them now.
	if(retFds)
	{

		// allocate space for set
		fdset = (P_BP_FDSET) bpcalloc(sizeof(BP_FDSET), 1);

	}

	// create pipes if we're not forking a pty
	if(retFds && !fork_pty)
	{


		// create replacement pipes to process
		if
		(
			(pipe(fdset->stdin) == 0) && (pipe(fdset->stdout) == 0) && (pipe(fdset->stderr) == 0)
		)
		{}
		else
			return NULL;

	}
	


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Fork Application and Handle FDs %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// run normal fork w/o pty
	if(!fork_pty)
	{

		// fork the process (doesn't fork terminal as well)
		pid = fork();
		if(pid == -1)
		{
			if(fdset)
				bpfree(fdset);
			return NULL;
		}

	}
	else
	{


		// This is required.  Value cannot be -1 or forkpty fails for some arbitrary reason.
		fdset->forkpty_master = 0;

		// using forkpty allows us to clone our entire terminal
		pid = forkpty(&fdset->forkpty_master, NULL, NULL, NULL);

		// if we got no pid, exit
		if(pid == -1)
		{

			// ensure we have a fdset
			if(fdset)
			{

				// destroy the fdset
				bpfree(fdset);

			}

			// return null indicating failure
			return NULL;

		}

		// mark session as forkpty
		fdset->session_is_forkpty = BP_TRUE;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Preserve Terminal Information If Forkpty %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(fdset->session_is_forkpty)
	{

		// get stdin from current terminal
        tcgetattr(STDIN_FILENO, &fdset->oldt);

        // set terminals
        fdset->newt = fdset->oldt;

        // set terminal flags
        fdset->newt.c_lflag &= ~(ECHO | ECHONL | ICANON);

        // set attributes (flush/nonblock)
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &fdset->newt);
        tcsetattr(STDIN_FILENO, TCSANOW,   &fdset->newt);

	}
	

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Handle Child %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// Fork successful, we are in child here
	if(!pid)
	{

		// replace pipes/replumb
		if(retFds)
		{

			if(!fdset->session_is_forkpty)
			{

				// stdin (STDIN_FILENO)
				close(fdset->stdin[1]);
				dup2 (fdset->stdin [0], STDIN_FILENO);

				// stdout (STDOUT_FILENO)
				close(fdset->stdout[0]);
				dup2 (fdset->stdout[1], STDOUT_FILENO);

				// stderr (STDERR_FILENO)
				close(fdset->stderr[0]);
				dup2 (fdset->stderr[1], STDERR_FILENO);

			}
			else
			{


			}

		}

		// !! Actually executes the program here !!
		execve(rootBinPath, params, envp);

		// exit the app after execve finishes
		exit(0);
		
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Handle Parent %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(pid)
	{
		
		if(retFds == BP_TRUE)
		{

			// set pid
			fdset->pid = pid;

			// check if the session comes from forkpty
			if(fdset->session_is_forkpty)
			{


			}

		}

	}
	
	// return the populated fdset
	return fdset;

}

// Attempts to stop a running program (killing it) provided a valid fdset with
// a valid pid.
BP_ERROR_T BP_FExecKillApplication(P_BP_FDSET app_to_kill)
{

	// if we have no app to kill, exit
	if(!app_to_kill)
		return ERR_FAILURE;
	if(!app_to_kill->pid)
		return ERR_FAILURE;

	// attempt to kill the app (error is non-zero)
	if(kill(app_to_kill->pid, SIGKILL))
		return ERR_FAILURE;

	// wait here to cleanup zombies if they exist
	waitpid(-1, NULL, WNOHANG);

	// set flag to run still running check
	BP_BOOL still_running = BP_FALSE;

	do
	{

		// check if application is still running
		still_running = BP_FExecApplicationStillRunning(app_to_kill) ? BP_TRUE : BP_FALSE;

		// if it's still running, send another kill signal after sleeping 1 second
		if(still_running)
		{
			sleep(1);
			BP_FExecKillApplication(app_to_kill);

		}

	} while(still_running);

    // Restore initial properties of STDIN to the old term
	app_to_kill->oldt.c_lflag |= ECHO | ECHONL | ICANON;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &app_to_kill->oldt);

	// return indicating success after kill signal is sent
	return ERR_SUCCESS;

}



// Attempts to check if an application is still running (using getpgid)
BP_ERROR_T BP_FExecApplicationStillRunning(P_BP_FDSET app)
{

	// if we have no app to kill, exit
	if(!app)
		return ERR_FAILURE;

	// attempt to get process group id
	if(getpgid(app->pid) == -1)
		return ERR_FAILURE;

	// return success if getpgid is successful
	return ERR_SUCCESS;

}


// Set nonblocking on file descriptors after program is executed
// (mainly used for command and control interface) NEVER FAILS.
int BP_FSetNonBlockingIO(P_BP_FDSET fds)
{

    // set all pipes to non blocking
    int flags = 0;

    if(!fds->forkpty_master)

    // set nonblock on stdin
    flags = fcntl(fds->stdin[1], F_GETFL, 0);
    fcntl(fds->stdin[1], F_SETFL, flags | O_NONBLOCK);

    // set nonblock on stdout
    flags = fcntl(fds->stdout[0], F_GETFL, 0);
    fcntl(fds->stdout[0], F_SETFL, flags | O_NONBLOCK);

    // set nonblock on stderr
    flags = fcntl(fds->stderr[0], F_GETFL, 0);
    fcntl(fds->stderr[0], F_SETFL, flags | O_NONBLOCK);

    // return indicating success
    return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Directory Wrappers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to create a directory
BP_ERROR_T BP_FSDirectoryCreate(char *filesystem_path, size_t mode)
{

	// ensure we have a filesystem path
	if(!filesystem_path)
		return ERR_FAILURE;

	// check that path is reasonable
	if(!BP_StringIsReasonableForUnixEXT4Directory(filesystem_path, bpstrlen(filesystem_path), BP_FALSE))
		return ERR_FAILURE;

	// create stat information
	struct stat file_stat;
	BP_ZERO_STACK(file_stat);

	// attempt to stat file
	if(stat(filesystem_path, &file_stat) == -1)
	{

		// reset errno to ensure value
		errno = 0;

		// attempt to create directory
		if(mkdir(filesystem_path, mode) == -1)
			return ERR_FAILURE;
		else
			return ERR_SUCCESS;

	}

	// return indicating failure if the stat fails
	return ERR_FAILURE;

}

// checks to see if a directory exists
BP_BOOL BP_FSDirectoryExists(char *filesystem_path)
{

	// ensure we have a filesystem path
	if(!filesystem_path)
		return BP_FALSE;

	// check that path is reasonable
	if(!BP_StringIsReasonableForUnixEXT4Directory(filesystem_path, bpstrlen(filesystem_path), BP_FALSE))
		return BP_FALSE;

	// create stat information
	struct stat file_stat;
	BP_ZERO_STACK(file_stat);

	// attempt to stat file
	if(stat(filesystem_path, &file_stat) == -1)
	{

		// return false if it doesn't exist
		return BP_FALSE;

	}

	// if the item is a directory, return true
	if(S_ISDIR(file_stat.st_mode))
		return BP_TRUE;

	// if it isn't a directory, return false
	return BP_TRUE;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Recursive Filesystem Deletion %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to destroy all items in a directory.  Skip symlinks.
BP_BOOL BP_FSDirectoryRecursiveDeleteAll(char *filesystem_path)
{

	// check that path is reasonable
	if(!BP_StringIsReasonableForUnixEXT4Directory(filesystem_path, bpstrlen(filesystem_path), BP_FALSE))
		return BP_FALSE;

	// set recursion parameters
	FTS    *ftsp = NULL;
	FTSENT *curr = NULL;

	// set files
	char *files[] =
	{
			(char *) filesystem_path,
			NULL
	};

	// FTS_NOCHDIR  - Avoid changing cwd, which could cause unexpected behavior
	//                in multithreaded programs
	// FTS_PHYSICAL - Don't follow symlinks. Prevents deletion of files outside
	//                of the specified directory
	// FTS_XDEV     - Don't cross filesystem boundaries
	ftsp = fts_open(files, FTS_NOCHDIR | FTS_PHYSICAL | FTS_XDEV, NULL);

	// ensure we could open the fts tree
	if (!ftsp)
		return BP_FALSE;

	// crawl and remove files
	while ((curr = fts_read(ftsp)))
	{

		switch (curr->fts_info)
		{

			case FTS_NS:
			case FTS_DNR:
			case FTS_ERR:
				// fprintf(stderr, "%s: fts_read error: %s\n",	curr->fts_accpath, strerror(curr->fts_errno));
				break;

			case FTS_DC:
			case FTS_DOT:
			case FTS_NSOK:
				// Not reached unless FTS_LOGICAL, FTS_SEEDOT, or FTS_NOSTAT were
				// passed to fts_open()
				break;

			case FTS_D:
				// Do nothing. Need depth-first search, so directories are deleted
				// in FTS_DP
				break;

			case FTS_DP:
			case FTS_F:
			case FTS_SL:
			case FTS_SLNONE:
			case FTS_DEFAULT:
				printf("\n Attempting to remove?: %s", curr->fts_path);
				/*
				if (remove(curr->fts_accpath) < 0)
				{
					// fprintf(stderr, "%s: Failed to remove: %s\n", curr->fts_path, strerror(errno));
				}
				*/
				break;

		}

	}



	// if the ftsp is open, close it here
	if (ftsp)
		fts_close(ftsp);

	// return indicating that the directory was able to be destroyed
	return BP_TRUE;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% File Stream Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// opens a fifo pipe: Access string is null terminated access parameter
// you'd pass to fopen. (aka. its a fopen wrapper.) "r+" for read write
FILE * BP_FOpen(char *rootDirPath, char *accessString)
{

	// ensure we have a root directory path and access string
	if(!rootDirPath || !accessString)
		return NULL;

	// file pointer
	FILE * file = NULL;

	// attempt to open file (return bad pointer if not able to open the file)
	file = fopen(rootDirPath, accessString);

	// return the file open attempt
	return file;

}

// closes the file/stream/fifo
int BP_FClose(FILE * file)
{

	// ensure we have a valid file pointer
	if(!file)
		return ERR_FAILURE;

	// attempt to close the file
	int err = fclose(file);
	if(err == EOF)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// writes to a file/stream/fifo pipe
int BP_FWrite(FILE * file, char *buff, int size)
{

	// ensure we have a valid file pointer
	if(!file || !buff || !size)
		return 0;

	// attempt to write the file
	int sizeOut = fwrite(buff, 1, size, file);

	// return the size of the data written
	return sizeOut;

}


// reads from a file/stream/fifo pipe
int BP_FRead(FILE *file, char *buff, int size)
{

	// ensure we have a valid file pointer
	if(!file || !buff || !size)
		return 0;

	// read from file ptr into our buffer
	int sizeRead = fread(buff, 1, size, file);

	// return the number of bytes read
	return sizeRead;

}


// attempt to get file size
size_t BP_FGetSize(char *filePath)
{

	// ensure we have a file path
	if(!filePath)
		return 0;

	// intiialize size
	size_t size = 0;

	// declare and initialize filestats
	struct stat fileStats;
	memset(&fileStats, 0x00, sizeof(fileStats));
		
	// attempt to store stats
	stat(filePath, &fileStats);

	// set size from returned structure
	size = fileStats.st_size;
	
	// return the size
	return size;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Quick Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempt to retrieve the entirety of a files contents.  Result must be bpfreed.
char * BP_FileGetContents
(
		char *file_path,
		P_BP_FILE_LOAD_EXTRA_INFO extra_data_in_out
)
{



	// create tmp info for use in case no extra data is passed in
	BP_FILE_LOAD_EXTRA_INFO tmp_info;
	BP_ZERO_STACK(tmp_info);

	// overide tmp info if nothing is passed in
	if(!extra_data_in_out)
		extra_data_in_out = (P_BP_FILE_LOAD_EXTRA_INFO) &tmp_info;

	// ensure the path is reasonable and that the file is there
	extra_data_in_out->file_path_is_reasonable_for_ext4_filename = BP_StringIsReasonableForUnixEXT4Filename(file_path, bpstrlen(file_path), BP_TRUE);

	// if the path is not reasonable we must exit immediately to prevent any potential
	// shenanigans.
	if(!extra_data_in_out->file_path_is_reasonable_for_ext4_filename)
		return NULL;

	// check rwx for current user
	extra_data_in_out->is_readable   = BP_FileAtPathExistsAndIsReadable  (file_path) ? BP_TRUE : BP_FALSE;
	extra_data_in_out->is_writable   = BP_FileAtPathExistsAndIsWritable  (file_path) ? BP_TRUE : BP_FALSE;
	extra_data_in_out->is_executable = BP_FileAtPathExistsAndIsExecutable(file_path) ? BP_TRUE : BP_FALSE;

	// mark file existing if it's readable/writable/executable
	if
	(
		extra_data_in_out->is_readable ||
		extra_data_in_out->is_writable ||
		extra_data_in_out->is_executable
	)
	{
		extra_data_in_out->file_exists = BP_TRUE;
	}

	// retrieve size
	extra_data_in_out->file_size = BP_FGetSize(file_path);
	if(!extra_data_in_out->file_size)
	{

		// mark file as empty if the size is zero
		extra_data_in_out->is_empty = BP_TRUE;
		return NULL;

	}



	// create new memory to hold file contents (or fail if memory cannot be created)
	char * new_mem = (char *) bpcalloc(extra_data_in_out->file_size+16, 1);
	if(!new_mem)
	{
		extra_data_in_out->failed_to_alloc_buff = BP_TRUE;
		return NULL;
	}

	// open file
	FILE * open_file = BP_FOpen(file_path, "r");
	if(open_file)
	{
		extra_data_in_out->failed_to_open = BP_TRUE;
	}


	// read the data into the new buffer
	int tmp_file_read_size = (size_t) BP_FRead(open_file, new_mem, extra_data_in_out->file_size);

	// ensure we have a file read size 0 or higher
	if(tmp_file_read_size < 0)
	{

		// mark it as a failed read
		extra_data_in_out->failed_to_read = BP_TRUE;

		// close the file since somehow we were able to open it but not read
		if(!BP_FClose(open_file))
		{
			extra_data_in_out->failed_to_close = BP_TRUE;
		}

		// destroy the allocated memory since the read failed
		bpfree(new_mem);

		// return a null pointer
		return NULL;

	}

	// set the file read size by typecasting to sizet
	extra_data_in_out->file_read_size = (size_t) tmp_file_read_size;

	// close the file
	if(!BP_FClose(open_file))
	{
		extra_data_in_out->failed_to_close = BP_TRUE;
	}

	//
	// NOTE: At this point we have loaded data from the buffer and we can
	//       run data checks.
	//


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run After Load Checks %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set checkall
	if(extra_data_in_out->check_all)
		extra_data_in_out->check_buffer_is_reasonable_printable = BP_TRUE;

	// check contents sha1 hash
	if(extra_data_in_out->check_all)
		extra_data_in_out->check_contents_sha1 = BP_TRUE;

	// run reasonable printable checks if set
	if(extra_data_in_out->check_buffer_is_reasonable_printable)
	{


		// check printable with newlines
		extra_data_in_out->is_reasonable_printable_with_newlines = BP_StringIsReasonablePrintableString
		(
			new_mem,
			extra_data_in_out->file_size,
			BP_FALSE,
			BP_TRUE
		);

		// check printable with carriage returns
		extra_data_in_out->is_reasonable_printable_with_carriage_returns = BP_StringIsReasonablePrintableString
		(
			new_mem,
			extra_data_in_out->file_size,
			BP_TRUE,
			BP_FALSE
		);

		// check if printable with both
		extra_data_in_out->is_reasonable_printable_with_carriage_returns_and_newlines = BP_StringIsReasonablePrintableString
		(
			new_mem,
			extra_data_in_out->file_size,
			BP_TRUE,
			BP_TRUE
		);

		// check if printable with neither
		extra_data_in_out->is_reasonable_printable_without_carriage_returns_and_newlines = BP_StringIsReasonablePrintableString
		(
			new_mem,
			extra_data_in_out->file_size,
			BP_FALSE,
			BP_FALSE
		);
	}

	// run buffer sha1 generation
	if(!extra_data_in_out->is_empty)
	{

		// if we're supposed to check sha1 sum, do it now
		if(extra_data_in_out->check_contents_sha1)
		{

			// generate sum
			char *tmp_sha1 = BP_GenSHA1(new_mem, extra_data_in_out->file_size);

			// if we could generate our sha1, store it
			if(tmp_sha1)
			{

				// mark sum as having generated ok
				extra_data_in_out->sha1_sum_generated_ok = BP_TRUE;

				// copy in the sha1
				memcpy(extra_data_in_out->sha1_sum, tmp_sha1, bpstrlen(tmp_sha1));

				// free tmp buffer
				bpfree(tmp_sha1);

			}

		}

	}

	// return the new buffer
	return new_mem;

}


// Opposite of file get contents.  Attempts to open a file and write data to it.
BP_ERROR_T BP_FilePutContents
(
		char *file_path,
		char *buff,
		size_t buff_size,
		BP_BOOL append
)
{

	// ensure we have a path
	if(!file_path)
		return ERR_FAILURE;


	// Run only basic checks on buffer.  Since this is a file-write utility, we
	// cant get too nitpicky with buffer sanity.
	if(!buff)
		return ERR_FAILURE;
	if(!buff_size)
		return ERR_FAILURE;

	// ensure the passed string is reasonable
	if(!BP_StringIsReasonablePrintableString(file_path, bpstrlen(file_path), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// open the file
	FILE * open_file = BP_FOpen(file_path, append ? (char *) "a" : (char *) "w");
	if(!open_file)
		return ERR_FAILURE;

	// read the data into the new buffer
	BP_FWrite(open_file, buff, buff_size);

	// close the file
	BP_FClose(open_file);

	// return indicating success
	return ERR_SUCCESS;

}


// Simple routine to return a SHA1 hash for a file.  The
// sha1_ret_buffer parameter must be a buffer at least 40 bytes
// long (preferably 41 bytes for null terminator).
BP_ERROR_T BP_FileSHA1Contents
(
	char *file_path,
	char *sha1_ret_buffer
)
{

	// ensure the path is reasonable and that the file is there
	if(!BP_StringIsReasonableForUnixEXT4Filename(file_path, bpstrlen(file_path), BP_TRUE))
		return ERR_FAILURE;
	if(!sha1_ret_buffer)
		return ERR_FAILURE;


	// get content
	char *file_content = BP_FileGetContents(file_path);
	if(!file_content)
		return ERR_FAILURE;

	// retrieve size
	size_t file_size = BP_FGetSize(file_path);
	if(!file_size)
	{
		bpfree(file_content);
		return ERR_FAILURE;
	}

	// generate sha1 from content
	char *file_sha1 = BP_GenSHA1(file_content, file_size);
	if(!file_sha1)
	{
		bpfree(file_content);
		return ERR_FAILURE;
	}

	// copy in the hash (40 bytes always by standard)
	memcpy(sha1_ret_buffer, file_sha1, 40);

	// free the file content
	bpfree(file_content);
	bpfree(file_sha1);

	// return indicating success
	return ERR_SUCCESS;

}


// Simple routine used for performing sha1 match verifications against
// files that exist on the filesystem.  The sha1_compare_match_buffer parameter
// must contain a sha1 hash (40 bytes long + null terminator) which will be
// matched against a hash generated directly via file-read.
BP_ERROR_T BP_FileSHA1ContentsAndAttemptMatch
(
		char *file_path,
		char *sha1_compare_match_buffer
)
{

	// ensure we have a buffer to match
	if(!sha1_compare_match_buffer)
		return ERR_FAILURE;

	// create space to hold buffer
	char sha1_ret_buff[41];
	BP_ZERO_STACK(sha1_ret_buff);

	// ensure we can create sha1
	if(!BP_FileSHA1Contents(file_path, (char *) &sha1_ret_buff))
		return ERR_FAILURE;

	// ensure the buffers match
	if(memcmp(&sha1_ret_buff, sha1_compare_match_buffer, 40) != 0)
		return ERR_FAILURE;


	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% File Destruction %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// delete file, overwriting if necessary
BP_ERROR_T BP_FileDelete(char *file_path, size_t zero_overwrite_loop_count)
{

	// ensure string is reasonable for ext4 filename (also attempt to stat file)
	if(!BP_StringIsReasonableForUnixEXT4Filename(file_path, bpstrlen(file_path), BP_TRUE))
		return ERR_FAILURE;


	// -- run basic checks ---------------------

	// check if we're deleting root
	if(BP_StringsMatchAsReasonableStrings(file_path, "/"))
	{
		printf("\n [!!] Attempted to delete / path within BP_FileDelete, this is not allowed.");
		return ERR_FAILURE;
	}

	// check if we're deleting current directory
	if(BP_StringsMatchAsReasonableStrings(file_path, "."))
	{
		printf("\n [!!] Attempted to delete '.' path within BP_FileDelete, this is not allowed.");
		return ERR_FAILURE;
	}

	// check if we're deleting previous directory
	if(BP_StringsMatchAsReasonableStrings(file_path, ".."))
	{
		printf("\n [!!] Attempted to delete '.' path within BP_FileDelete, this is not allowed.");
		return ERR_FAILURE;
	}

	// -- run overwrite loop --------------------

	// get the file size
	size_t file_size = BP_FGetSize(file_path);

	// if the size of the file is zero, just unlink it
	if(!file_size)
	{

		// unlink the file path
		unlink(file_path);
		return ERR_SUCCESS;

	}

	// overwrite the loop count
	if(!zero_overwrite_loop_count)
	{

		// unlink the file path
		unlink(file_path);
		return ERR_SUCCESS;

	}

	if(file_size == (size_t) -1)
		return ERR_FAILURE;

	// attempt to open file
	FILE *fptr = fopen(file_path, "w+");

	// ensure we can open our file
	if(!fptr)
		return ERR_FAILURE;


	// loop iterator
	size_t loop_iter = 0;

	// iterate through the loop count
	for(; loop_iter < zero_overwrite_loop_count; loop_iter++)
	{

		// set the position iter
		size_t file_pos_iter = 0;

		// iterate through the file and write zero bytes
		for(; file_pos_iter < file_size; file_pos_iter++)
		{

			// set the file position
			fseek(fptr, file_pos_iter, SEEK_SET);

			// write a null byte in place
			fwrite("\x00", 1, 1, fptr);

		}

	}

	// --- close and unlink file --------------------------

	// close the file pointer
	fclose(fptr);

	// unlink the file path
	unlink(file_path);

	// return indicating that the unlinking has worked out
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% File Descriptors %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// opens a file descriptor, and returns a stream FILE object to the fd, use mode
// to specify the mode to open with.
// Note: Use FClose to close the descriptor not FDClose
FILE * BP_FDOpen(int fd, char *mode)
{

	// ensure we have a mode pointer
	if(!mode)
		return NULL;

	// attempt to open the file descriptor
	FILE * file = fdopen(fd, mode);

	// return the file pointer
	return file;

}

// closes an open file descriptor (not a FILE)
int BP_FDClose(int fd)
{

	// attempt to close the specified file descriptor
	int err = close(fd);
	if(err != 0)
		return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

// Writes to a given file descriptor (wrapper)
int BP_FDWrite(int fd, char *buff, int size)
{

	// ensure we have a buffer and a size
	if(!buff || !size)
		return 0;

	// attempt to write specified bytes to the file descriptor
	int writeBytes = write(fd, buff, size);

	// return the number of bytes written
	return writeBytes;

}

// Reads from a given file descriptor (wrapper)
int BP_FDRead(int fd, char *buff, int size)
{

	// ensure we have a buffer to read into, and a size
	if(!buff || !size)
		return 0;

	// attempt to read specified bytes from the file descriptor
	int readBytes = read(fd, buff, size);

	// return the number of bytes read
	return readBytes;

}


// This uses the fsync() routine to flush all pipe data through the
// fd and onto the disk.
int BP_FDFlush(int fd)
{

	// fsync the file descriptor
	switch(fsync(fd))
	{
		case EBADF:
			return ERR_FAILURE;
		case EIO:
			return ERR_FAILURE;
		case EROFS:
			return ERR_FAILURE;
		case EINVAL:
			return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Secure Tmpfiles %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Returns a file descriptor which can be used to write data to a temporary file.  It creates
// the temporary file using the correct permissions required.  Template must be something like:
//  ex.
//     /tmp/whateverdir/filewhatever-XXXXXX where the XXXXXX must be literally in the template and
//     is replaced dynamically via mkstemp.  Due to the nature of file descriptors, this function
//     has to return a non-standard error code.  It returns -1 on failure.
int BP_FDCreateSecureTempFile(char *tmpfile_template_path)
{

	// ensure we have a template path
	if(!tmpfile_template_path)
		return -1;

	// ensure the file path is reasonable
	if(!BP_StringIsReasonablePrintableString(tmpfile_template_path, bpstrlen(tmpfile_template_path), BP_FALSE, BP_FALSE))
		return -1;

	// the tempfile must have XXXXXX
	if(!strstr(tmpfile_template_path, "XXXXXX"))
	{
		printf("\n [!!] Err: Secure temporary file templates must contain XXXXXX");
		return -1;
	}

	// printf("\n Attempting to create tmpfile?: %s", tmpfile_template_path);

	// attempt to open temp file
	int fd = mkstemp(tmpfile_template_path);

	// check return
	if( fd == -1)
	{

		// printf("\n Failed to create tmpfile: %s", tmpfile_template_path);
		return -1;
	}

	// Unlink the file descriptor so that the files are
	// destroyed immediately after use (required for secure
	// temp file race-condition avoidance).  The file descriptor
	// will still be able to read and write from the filesystem
	// using the descriptor, but the file will be destroyed
	// securely when the descriptor closes.
	unlink(tmpfile_template_path);

	// return the file descriptor
	return fd;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% FStat Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This will attempt to get a mode string from a stats buff.  Buffer is returned
// via bpcalloc and must be free'd.
char * BP_FStatGetFiletypeModeString(struct stat * stats)
{

	// ensure we have stats
	if(!stats)
		return NULL;

	// declare mode string and joiner string
	char * mode_str = NULL;
	char * joined_str = NULL;

	// --- Blockparty Internal Type Macros ----------

	// add FMT entry
	if(BP_STAT_MODE_IS_IFMT(stats->st_mode))
	{

		// add fmt string
		if(!mode_str)
		{
			mode_str = bpstrdup("FMT ");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "FMT ", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}
	}



	// --- Stdlib Macros

	// add REG entry
	if(S_ISREG(stats->st_mode))
	{

		// add reg string
		if(!mode_str)
		{
			mode_str = bpstrdup("REG ");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "REG ", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}
	}

	// add DIR entry
	if(S_ISDIR(stats->st_mode))
	{

		// add string
		if(!mode_str)
		{
			mode_str = bpstrdup("DIR ");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "DIR ", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}
	}

	// add CHR entry
	if(S_ISCHR(stats->st_mode))
	{

		// add string
		if(!mode_str)
		{
			mode_str = bpstrdup("CHR ");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "CHR ", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}
	}

	// add BLK entry
	if(S_ISBLK(stats->st_mode))
	{

		// add string
		if(!mode_str)
		{
			mode_str = bpstrdup("BLK ");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "BLK ", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}
	}

	// add FIFO entry
	if(S_ISFIFO(stats->st_mode))
	{

		// add string
		if(!mode_str)
		{
			mode_str = bpstrdup("FIFO ");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "FIFO ", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}
	}

	// add LNK entry
	if(S_ISLNK(stats->st_mode))
	{

		// add string
		if(!mode_str)
		{
			mode_str = bpstrdup("LNK ");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "LNK ", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}

	}

	// add SOCK entry
	if(S_ISSOCK(stats->st_mode))
	{

		// add string
		if(!mode_str)
		{
			mode_str = bpstrdup("SOCK ");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "SOCK ", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}
	}

	// trim string to get rid of whitespace
	char * trimmed = NULL;

	// if we have a string we can trim it
	if(mode_str)
	{

		// trim whitespace off both ends
		trimmed = BP_StringTrimWhitespace(mode_str, BP_STRING_TRIM_DIRECTION_BOTH, bpstrlen(mode_str));

		// if any trimming occured, destroy old mode and reassign with trimmed
		if(trimmed)
		{
			bpfree(mode_str);
			mode_str = trimmed;
		}

	}

	// return the mode string
	return mode_str;

}


// This will attempt to get a mode string from a stats buff.  Buffer is returned
// via bpcalloc and must be free'd.
char * BP_FStatGetOwnerPermissionsModeString(struct stat * stats)
{

	// ensure we have stats
	if(!stats)
		return NULL;

	// declare mode string and joiner string
	char * mode_str = NULL;
	char * joined_str = NULL;

	// --- Blockparty Internal Type Macros ----------

	// add RWXU entry
	if(BP_STAT_MODE_IS_IRWXU(stats->st_mode))
	{

		// add fmt string
		if(!mode_str)
		{
			mode_str = bpstrdup("usr:");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "usr:", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}

	}


	// add RUSR entry
	if(BP_STAT_MODE_IS_IRUSR(stats->st_mode))
	{

		// add RUSR string
		if(!mode_str)
		{
			mode_str = bpstrdup("R");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "R", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}

	}


	// add WUSR entry
	if(BP_STAT_MODE_IS_IWUSR(stats->st_mode))
	{

		// add WUSR string
		if(!mode_str)
		{
			mode_str = bpstrdup("W");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "W", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}

	}



	// add XUSR entry
	if(BP_STAT_MODE_IS_IXUSR(stats->st_mode))
	{

		// add WUSR string
		if(!mode_str)
		{
			mode_str = bpstrdup("X");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "X", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}

	}

	// trim string to get rid of whitespace
	char * trimmed = NULL;

	// if we have a string we can trim it
	if(mode_str)
	{

		// trim whitespace off both ends
		trimmed = BP_StringTrimWhitespace(mode_str, BP_STRING_TRIM_DIRECTION_BOTH, bpstrlen(mode_str));

		// if any trimming occured, destroy old mode and reassign with trimmed
		if(trimmed)
		{
			bpfree(mode_str);
			mode_str = trimmed;
		}

	}


	// return the mode string
	return mode_str;

}


// This will attempt to get a mode string from a stats buff.  Buffer is returned
// via bpcalloc and must be free'd.
char * BP_FStatGetGroupPermissionsModeString(struct stat * stats)
{

	// ensure we have stats
	if(!stats)
		return NULL;

	// declare mode string and joiner string
	char * mode_str = NULL;
	char * joined_str = NULL;

	// --- Blockparty Internal Type Macros ----------

	// add RWXU entry
	if(BP_STAT_MODE_IS_IRWXG(stats->st_mode))
	{

		// add fmt string
		if(!mode_str)
		{
			mode_str = bpstrdup("grp:");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "grp:", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}

	}


	// add RGRP entry
	if(BP_STAT_MODE_IS_IRGRP(stats->st_mode))
	{

		// add RGRP string
		if(!mode_str)
		{
			mode_str = bpstrdup("R");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "R", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}

	}


	// add WGRP entry
	if(BP_STAT_MODE_IS_IWGRP(stats->st_mode))
	{

		// add WGRP string
		if(!mode_str)
		{
			mode_str = bpstrdup("W");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "W", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}

	}



	// add XGRP entry
	if(BP_STAT_MODE_IS_IXGRP(stats->st_mode))
	{

		// add WGRP string
		if(!mode_str)
		{
			mode_str = bpstrdup("X");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "X", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}

	}

	// trim string to get rid of whitespace
	char * trimmed = NULL;

	// if we have a string we can trim it
	if(mode_str)
	{

		// trim whitespace off both ends
		trimmed = BP_StringTrimWhitespace(mode_str, BP_STRING_TRIM_DIRECTION_BOTH, bpstrlen(mode_str));

		// if any trimming occured, destroy old mode and reassign with trimmed
		if(trimmed)
		{
			bpfree(mode_str);
			mode_str = trimmed;
		}

	}

	// return the mode string
	return mode_str;

}


// This will attempt to get a mode string from a stats buff.  Buffer is returned
// via bpcalloc and must be free'd. (fetches perms for "others", aka non-owner, non-group)
char * BP_FStatGetOthersPermissionsModeString(struct stat * stats)
{

	// ensure we have stats
	if(!stats)
		return NULL;

	// declare mode string and joiner string
	char * mode_str = NULL;
	char * joined_str = NULL;

	// --- Blockparty Internal Type Macros ----------

	// add RWXU entry
	if(BP_STAT_MODE_IS_IRWXO(stats->st_mode))
	{

		// add fmt string
		if(!mode_str)
		{
			mode_str = bpstrdup("oth:");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "oth:", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}

	}


	// add ROTH entry
	if(BP_STAT_MODE_IS_IROTH(stats->st_mode))
	{

		// add RGRP string
		if(!mode_str)
		{
			mode_str = bpstrdup("R");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "R", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}

	}


	// add WOTH entry
	if(BP_STAT_MODE_IS_IWOTH(stats->st_mode))
	{

		// add WOTH string
		if(!mode_str)
		{
			mode_str = bpstrdup("W");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "W", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}

	}



	// add XOTH entry
	if(BP_STAT_MODE_IS_IXOTH(stats->st_mode))
	{

		// add WOTH string
		if(!mode_str)
		{
			mode_str = bpstrdup("X");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "X", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}

	}

	// trim string to get rid of whitespace
	char * trimmed = NULL;

	// if we have a string we can trim it
	if(mode_str)
	{

		// trim whitespace off both ends
		trimmed = BP_StringTrimWhitespace(mode_str, BP_STRING_TRIM_DIRECTION_BOTH, bpstrlen(mode_str));

		// if any trimming occured, destroy old mode and reassign with trimmed
		if(trimmed)
		{
			bpfree(mode_str);
			mode_str = trimmed;
		}

	}


	// return the mode string
	return mode_str;

}


// This will attempt to get a mode string from a stats buff.  Buffer is returned
// via bpcalloc and must be free'd. (fetches perms for "special" aka suid/sgid etc)
char * BP_FStatGetSpecialPermissionsModeString(struct stat * stats)
{

	// ensure we have stats
	if(!stats)
		return NULL;

	// declare mode string and joiner string
	char * mode_str   = NULL;
	char * joined_str = NULL;



	// --- Blockparty Internal Type Macros ----------

	// add ISVTX entry
	if(BP_STAT_MODE_IS_ISVTX(stats->st_mode))
	{

		// add fmt string
		if(!mode_str)
		{
			mode_str = bpstrdup("SVTX ");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "SVTX ", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}

	}


	// add SUID entry
	if(BP_STAT_MODE_IS_ISUID(stats->st_mode))
	{

		// add SUID string
		if(!mode_str)
		{
			mode_str = bpstrdup("SUID ");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "SUID ", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}

	}


	// add ISGID entry
	if(BP_STAT_MODE_IS_ISGID(stats->st_mode))
	{

		// add WOTH string
		if(!mode_str)
		{
			mode_str = bpstrdup("SGID ");
		}
		else
		{

			// join mode
			joined_str = BP_JoinStrings(mode_str, "SGID ", NULL);

			// free old
			bpfree(mode_str);

			// overwrite
			mode_str = joined_str;

		}

	}

	// join front part if some modes were matched
	char * tmp_str = NULL;
	if(mode_str)
	{
		tmp_str = BP_JoinStrings("spec:", mode_str);
		if(tmp_str)
		{
			bpfree(mode_str);
			mode_str = tmp_str;
		}
	}


	// trim string to get rid of whitespace
	char * trimmed = NULL;

	// if we have a string we can trim it
	if(mode_str)
	{

		// trim whitespace off both ends
		trimmed = BP_StringTrimWhitespace(mode_str, BP_STRING_TRIM_DIRECTION_BOTH, bpstrlen(mode_str));

		// if any trimming occured, destroy old mode and reassign with trimmed
		if(trimmed)
		{
			bpfree(mode_str);
			mode_str = trimmed;
		}

	}

	// return the mode string
	return mode_str;

}


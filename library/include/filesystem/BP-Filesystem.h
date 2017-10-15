/***************************************************************************
 *            BP-Filesystem.h
 *
 *  Mon Jan 27 21:50:41 2003
 *  Copyright  2003  User
 *  Email
 ****************************************************************************/


#ifndef __BP_FILESYSTEM_H
#define __BP_FILESYSTEM_H


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Stat Mode Macros %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// --- file types ------

// file type bitfields
#define BP_STAT_MODE_IS_IFMT(stat_mode)\
	(stat_mode & S_IFMT)

// mask for socket
#define BP_STAT_MODE_IS_IFSOCK(stat_mode)\
	(stat_mode & S_IFSOCK)

// mask for symbolic link
#define BP_STAT_MODE_IS_IFLNK(stat_mode)\
	(stat_mode & S_IFLNK)

// mask for regular file
#define BP_STAT_MODE_IS_IREG(stat_mode)\
	(stat_mode & S_IFREG)

// mask for block device
#define BP_STAT_MODE_IS_IFBLK(stat_mode)\
	(stat_mode & S_IFBLK)

// mask for directory
#define BP_STAT_MODE_IS_IFDIR(stat_mode)\
	(stat_mode & S_IFDIR)

// mask for character device
#define BP_STAT_MODE_IS_IFCHR(stat_mode)\
	(stat_mode & S_IFCHR)

// mask for fifo
#define BP_STAT_MODE_IS_IFIFO(stat_mode)\
	(stat_mode & S_IFIFO)



// --- special permissions ---

// mask for sticky bit
#define BP_STAT_MODE_IS_ISVTX(stat_mode)\
	(stat_mode & S_ISVTX)

// mask for setuid
#define BP_STAT_MODE_IS_ISUID(stat_mode)\
	(stat_mode & S_ISUID)

// mask for setgid
#define BP_STAT_MODE_IS_ISGID(stat_mode)\
	(stat_mode & S_ISGID)





// --- owner masks --------

#define BP_STAT_MODE_IS_IRWXU(stat_mode)\
	(stat_mode & S_IRWXU)

#define BP_STAT_MODE_IS_IRUSR(stat_mode)\
	(stat_mode & S_IRUSR)

#define BP_STAT_MODE_IS_IWUSR(stat_mode)\
	(stat_mode & S_IWUSR)

#define BP_STAT_MODE_IS_IXUSR(stat_mode)\
	(stat_mode & S_IXUSR)


// --- group masks --------

#define BP_STAT_MODE_IS_IRWXG(stat_mode)\
	(stat_mode & S_IRWXG)

#define BP_STAT_MODE_IS_IRGRP(stat_mode)\
	(stat_mode & S_IRGRP)

#define BP_STAT_MODE_IS_IWGRP(stat_mode)\
	(stat_mode & S_IWGRP)

#define BP_STAT_MODE_IS_IXGRP(stat_mode)\
	(stat_mode & S_IXGRP)


// --- other(s) masks ----

#define BP_STAT_MODE_IS_IRWXO(stat_mode)\
	(stat_mode & S_IRWXU)

#define BP_STAT_MODE_IS_IROTH(stat_mode)\
	(stat_mode & S_IROTH)

#define BP_STAT_MODE_IS_IWOTH(stat_mode)\
	(stat_mode & S_IWOTH)

#define BP_STAT_MODE_IS_IXOTH(stat_mode)\
	(stat_mode & S_IXOTH)


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %% File List for Linear Traversals %%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// FILE-LIST IS DEPRECATED :: USE FILESYSTEM REGISTRY INSTEAD
// type corresponds to one of the following values
// 	DT_DIR  - directory
//  DT_CHR  - character special file (aka a terminal or somesuch)
//  DT_BLK  - block special file (aka a disk or somesuch)
//  DT_REG  - regular file
//  DT_FIFO - fifo pipe
//  DT_LNK  - symbolic link
//  DT_SOCK - socket file/pipe
typedef enum
{
	BP_FILETYPE_DIR   = DT_DIR,
	BP_FILETYPE_CHR   = DT_CHR,
	BP_FILETYPE_BLK   = DT_BLK,
	BP_FILETYPE_REG   = DT_REG,
	BP_FILETYPE_FIFO  = DT_FIFO,
	BP_FILETYPE_LNK   = DT_LNK,
	BP_FILETYPE_SOCK  = DT_SOCK
} BP_FILETYPE;


// File list element.
typedef struct _BP_FILE_LIST {

	// path to the file
	char *pathToFile;

	// type of file
	BP_FILETYPE type;

	// attributes
	struct stat attrs;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Permission Indicators %%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// file is readable
	bool read;

	// file is writable
	bool write;

	// file can be executed
	bool execute;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% List Pointers %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// list pointers
	struct _BP_FILE_LIST *first;
	struct _BP_FILE_LIST *next;
	struct _BP_FILE_LIST *prev;


} BP_FILE_LIST, *P_BP_FILE_LIST;



// holds dup2'd file descriptors returned from executing programs
typedef struct __BP_FDSET {

	// set if invoked form FExec
	int pid;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% File Descriptors if fork() %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// processes stdin (stdin[1] is what we actually write to)
	int stdin[2];

	// process stdout (stdout[0] is what we actually read from)
	int stdout[2];

	// process stderr (stderr[0] is what we actually read from)
	int stderr[2];


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% FDSets if forkpty() %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// this is set to true if this session is using a pty
	BP_BOOL session_is_forkpty;

	// set pty master pid if created with forkpty
	int forkpty_master;

	// old and new terminals (used by forkpty)
	termios oldt;
	termios newt;


	// read file descriptors
	fd_set pty_read_fd;

	// write file descriptors
	fd_set pty_write_fd;

	// error file descriptors
	fd_set pty_error_fd;


} BP_FDSET, *P_BP_FDSET;




// returns a list of all files of a specific type found starting
// at rootDirPath;
P_BP_FILE_LIST BP_FSFindAllType
(
	char *         rootDirPath,
	BP_FILETYPE    type,
	bool           recursive,
	P_BP_FILE_LIST prev
);

// returns a list of executable elf binaries given the parameters
// set false (for boolean)/-1(for gid&uid) to every parameter you dont want to examine
P_BP_FILE_LIST BP_FSSearchAll
(
	P_BP_FILE_LIST inList,
	char *name,
	bool execute,
	bool write,
	bool read,
	bool elf,
	uid_t uid,
	gid_t gid
);

// duplicates a list element
P_BP_FILE_LIST BP_FileListDupElement(P_BP_FILE_LIST inList);

// creates a new file list
P_BP_FILE_LIST BP_NewFileList(P_BP_FILE_LIST first, P_BP_FILE_LIST next, P_BP_FILE_LIST prev);

// destroys a file list
BP_ERROR_T BP_DestroyFileList(P_BP_FILE_LIST inList);

// prints out file list element
int BP_FileListPrintElement(P_BP_FILE_LIST inList);

// prints out all items in a file list starting at first
int BP_FileListPrintAll(P_BP_FILE_LIST inList);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% File Execution Wrappers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Executes a file given parms, if retFds is true.. returns a structure
// containing the file descriptors which can be read from or written to.
P_BP_FDSET BP_FExec
(
	char *rootBinPath,
	char **params,
	char **envp,
	BP_BOOL retFds,
	BP_BOOL fork_pty = BP_TRUE
);


// Attempts to stop a running program (killing it) provided a valid fdset with
// a valid pid.
BP_ERROR_T BP_FExecKillApplication(P_BP_FDSET app_to_kill);


// Attempts to check if an application is still running (using getpgid)
BP_ERROR_T BP_FExecApplicationStillRunning(P_BP_FDSET app);

// Set nonblocking on file descriptors after program is executed
// (mainly used for command and control interface) NEVER FAILS
int BP_FSetNonBlockingIO(P_BP_FDSET fds);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Directory Wrappers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// attempt to create a directory
BP_ERROR_T BP_FSDirectoryCreate(char *filesystem_path, size_t mode);

// checks to see if a directory exists
BP_BOOL BP_FSDirectoryExists(char *filesystem_path);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Recursive Filesystem Deletion %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to destroy all items in a directory.  Skip symlinks.
BP_BOOL BP_FSDirectoryRecursiveDeleteAll(char *filesystem_path);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% File Descriptors %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// opens a file descriptor, and returns a stream FILE object
// use FClose to close the descriptor not FDClose
FILE * BP_FDOpen(int fd);

// closes an open file descriptor (not a FILE)
int BP_FDClose(int fd);

// Writes to a given file descriptor
int BP_FDWrite(int fd, char *buff, int size);

// Reads from a given file descriptor
int BP_FDRead(int fd, char *buff, int size);

// This uses the fsync() routine to flush all pipe data through the
// fd and onto the disk.
int BP_FDFlush(int fd);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Secure Tmpfiles %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Returns a file descriptor which can be used to write data to a temporary file.  It creates
// the temporary file using the correct permissions required.  Template must be something like:
//  ex.
//     /tmp/whateverdir/filewhatever-XXXXXX where the XXXXXX must be literally in the template and
//     is replaced dynamically via mkstemp.
int BP_FDCreateSecureTempFile(char *tmpfile_template_path);




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% File Stream Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// opens a file on the filesystem with accessString permissions (aka. "r", "r+", "w")
FILE * BP_FOpen(char *rootDirPath, char *accessString);

// closes a file stream
int BP_FClose(FILE * file);

// writes across a file stream
int BP_FWrite(FILE * file, char *buff, int size);

// reads from a file stream
int BP_FRead(FILE *file, char *buff, int size);

// returns the size of a file (unsigned)
size_t BP_FGetSize(char *filePath);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Quick Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// this is an in-out data structure you can pass to FileGetContents in order to get extra
// data about the loaded file.
typedef struct __BP_FILE_LOAD_EXTRA_INFO {

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Config Flags %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// run all checks
	BP_BOOL check_all;

	// run individual checks
	BP_BOOL check_buffer_is_reasonable_printable;

	// run sha1 check if set
	BP_BOOL check_contents_sha1;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Output Values %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// size of the file (reported)
	size_t file_size;

	// size of the file read in from BP_FRead()
	size_t file_read_size;

	// if the path is determined to be preasonable for an ext4 filename
	BP_BOOL file_path_is_reasonable_for_ext4_filename;

	// if the file exists, this is set
	BP_BOOL file_exists;

	// if the file is readable, this is set
	BP_BOOL is_readable;

	// if the file is readable, this is set
	BP_BOOL is_writable;

	// if the file is executable, this is set
	BP_BOOL is_executable;

	// if the file sizeis zero, this is set
	BP_BOOL is_empty;

	// check if the file is reasonable printable with only newlines
	BP_BOOL is_reasonable_printable_with_newlines;
	BP_BOOL is_reasonable_printable_without_newlines;

	// check if the file is reasonable/printable with only carriage returns
	BP_BOOL is_reasonable_printable_with_carriage_returns;
	BP_BOOL is_reasonable_printable_without_carriage_returns;

	// check if the file is reasonable/printable with only carriage returns
	BP_BOOL is_reasonable_printable_with_carriage_returns_and_newlines;

	// check if it's reasonable printable without cr/nl
	BP_BOOL is_reasonable_printable_without_carriage_returns_and_newlines;

	// space for sha1 sum if set
	BP_BOOL sha1_sum_generated_ok;

	// buffer allocation failed
	BP_BOOL failed_to_alloc_buff;

	// set to BP_TRUE if we failed to open the file
	BP_BOOL failed_to_open;

	// set to BP_TRUE if we failed to read the file into our buffer
	BP_BOOL failed_to_read;

	// set to BP_TRUE if we failed to close the file
	BP_BOOL failed_to_close;

	// actual ascii buffer containing our sha1 sum
	char sha1_sum[100];

} BP_FILE_LOAD_EXTRA_INFO, *P_BP_FILE_LOAD_EXTRA_INFO;

// Attempt to retrieve the entirety of a files contents.  Result must be bpfreed.  Caveat
// to using this is that the file must be able to fit in available memory.  If it's not,
// the function will return null.
char * BP_FileGetContents(char *file_path, P_BP_FILE_LOAD_EXTRA_INFO extra_data_in_out = NULL);

// Opposite of file get contents.  Attempts to open a file and write data to it.
BP_ERROR_T BP_FilePutContents
(
		char *file_path,
		char *buff,
		size_t buff_size,
		BP_BOOL append
);

// Simple routine to return a SHA1 hash for a file.  The
// sha1_ret_buffer parameter must be a buffer at least 40 bytes
// long (preferably 41 bytes for null terminator).
BP_ERROR_T BP_FileSHA1Contents
(
		char *file_path,
		char *sha1_ret_buffer
);

// Simple routine used for performing sha1 match verifications against
// files that exist on the filesystem.  The sha1_compare_match_buffer parameter
// must contain a sha1 hash (40 bytes long + null terminator) which will be
// matched against a hash generated directly via file-read.
BP_ERROR_T BP_FileSHA1ContentsAndAttemptMatch
(
		char *file_path,
		char *sha1_compare_match_buffer
);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% File Destruction %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// delete file, overwriting if necessary
BP_ERROR_T BP_FileDelete(char *file_path, size_t zero_overwrite_loop_count);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% FStat Utilities %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This will attempt to get a mode string from a stats buff.  Buffer is returned
// via bpcalloc and must be free'd. (Example: FMT REG is returned for a regular file, FMT LNK is returned
// for a symlink, etc).
char * BP_FStatGetFiletypeModeString(struct stat * stats);

// This will attempt to get a mode string from a stats buff.  Buffer is returned
// via bpcalloc and must be free'd.
char * BP_FStatGetOwnerPermissionsModeString(struct stat * stats);

// This will attempt to get a mode string from a stats buff.  Buffer is returned
// via bpcalloc and must be free'd. (fetches perms for "others", aka non-owner, non-group)
char * BP_FStatGetGroupPermissionsModeString(struct stat * stats);

// This will attempt to get a mode string from a stats buff.  Buffer is returned
// via bpcalloc and must be free'd. (fetches perms for "others", aka non-owner, non-group)
char * BP_FStatGetOthersPermissionsModeString(struct stat * stats);

// This will attempt to get a mode string from a stats buff.  Buffer is returned
// via bpcalloc and must be free'd. (fetches perms for "special" aka suid/sgid etc)
char * BP_FStatGetSpecialPermissionsModeString(struct stat * stats);


# endif // __BP_FILESYSTEM_H

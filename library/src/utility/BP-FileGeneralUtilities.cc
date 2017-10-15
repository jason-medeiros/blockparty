
// include main functions
#include "../../include/BP-Main.h"

//! returns ERR_FAILURE or ERR_SUCCESS depending on if the file simply exists or not.
BP_ERROR_T BP_FileAtPathExists(char *file_path)
{

	// ensure we have a file pointer
	if(!file_path)
		return ERR_FAILURE;

	// ensure path is reasonable printable
	if(!BP_StringIsReasonableForUnixEXT4Filename(file_path, bpstrlen(file_path), BP_FALSE))
		return ERR_FAILURE;

	// attempt to access the file
	if(access(file_path, F_OK) != -1)
	{
		return ERR_SUCCESS;
	}

	// return indicating failure
	return ERR_FAILURE;

}


//! returns ERR_FAILURE or ERR_SUCCESS (file is there and can be opened) depending on if the file is there and readable.
BP_ERROR_T BP_FileAtPathExistsAndIsReadable(char *file_path)
{

	// ensure pointer is not null
	if(!file_path)
		return ERR_FAILURE;

	// ensure path is reasonable printable
	if(!BP_StringIsReasonableForUnixEXT4Filename(file_path, bpstrlen(file_path), BP_FALSE))
		return ERR_FAILURE;

	// create sb buffer and zero it
	struct stat sb;
	BP_ZERO_STACK(sb);

	// run stat to determine if it's executable
	stat(file_path, &sb);

	// if stat reports its executable, return error success
	if(sb.st_mode & S_IRUSR)
		return ERR_SUCCESS;

	// file is not readable
	return ERR_FAILURE;

}

//! returns ERR_FAILURE or ERR_SUCCESS (file is there and can be opened) depending on if the file is there and readable.
BP_ERROR_T BP_FileAtPathExistsAndIsWritable(char *file_path)
{

	// ensure pointer is not null
	if(!file_path)
		return ERR_FAILURE;

	// ensure path is reasonable printable
	if(!BP_StringIsReasonableForUnixEXT4Filename(file_path, bpstrlen(file_path), BP_FALSE))
		return ERR_FAILURE;

	// create sb buffer and zero it
	struct stat sb;
	BP_ZERO_STACK(sb);

	// run stat to determine if it's executable
	stat(file_path, &sb);

	// if stat reports its executable, return error success
	if(sb.st_mode & S_IWUSR)
		return ERR_SUCCESS;

	// cannot open file as read, return error failure as indicator
	return ERR_FAILURE;

}



//! returns ERR_FAILURE or ERR_SUCCESS (file is there and can be opened) depending on if the file is there and executable.
BP_ERROR_T BP_FileAtPathExistsAndIsExecutable(char *file_path)
{

	// ensure pointer is not null
	if(!file_path)
		return ERR_FAILURE;

	// ensure path is reasonable printable
	if(!BP_StringIsReasonableForUnixEXT4Filename(file_path, bpstrlen(file_path), BP_FALSE))
		return ERR_FAILURE;

	// create sb buffer and zero it
	struct stat sb;
	BP_ZERO_STACK(sb);

	// run stat to determine if it's executable
	stat(file_path, &sb);

	// if stat reports its executable, return error success
	if(sb.st_mode & S_IXUSR)
		return ERR_SUCCESS;

	// cannot open file as read, return error failure as indicator
	return ERR_FAILURE;

}

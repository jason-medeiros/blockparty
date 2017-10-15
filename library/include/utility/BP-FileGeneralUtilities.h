#ifndef BP_FILEGENERALUTILITIES_H
#define BP_FILEGENERALUTILITIES_H

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Non-Atomic File Testing %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/*
 *
 * NOTE: These routines are not atomic, and can be vulnerable to race conditions.
 *
 */


//! returns ERR_FAILURE or ERR_SUCCESS depending on if the file simply exists or not.
BP_ERROR_T BP_FileAtPathExists(char *file_path);

//! returns ERR_FAILURE or ERR_SUCCESS (file is there) depending on if the file is there and readable.
BP_ERROR_T BP_FileAtPathExistsAndIsReadable(char *file_path);

//! returns ERR_FAILURE or ERR_SUCCESS (file is there and can be opened) depending on if the file is there and writable.
BP_ERROR_T BP_FileAtPathExistsAndIsWritable(char *file_path);

//! returns ERR_FAILURE or ERR_SUCCESS (file is there and can be opened) depending on if the file is there and executable.
BP_ERROR_T BP_FileAtPathExistsAndIsExecutable(char *file_path);

#endif /* BP_FILEGENERALUTILITIES_H */

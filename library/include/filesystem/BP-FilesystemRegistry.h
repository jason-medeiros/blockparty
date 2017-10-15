/*
 * BP-FilesystemRegistry.h
 *
 *  Created on: Jul 20, 2015
 *      Author: root
 */

#ifndef __BP_FILESYSTEMREGISTRY_H_
#define __BP_FILESYSTEMREGISTRY_H_


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Enforce Limits %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// internal macro used for enforcing that limits are set ok within a fsreg.
#define BP_FSREG_ENFORCE_LIMITS_CRITICAL_SECTION(fs_reg,ret_val)\
		if(fs_reg->limit_at_entry_n)                                                                       \
		if(fs_reg->entry_n >= fs_reg->limit_at_entry_n)                                                    \
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ret_val);              \
                                                                                                           \
		if(fs_reg->limit_at_directory_n)                                                                   \
		if(fs_reg->directory_n >= fs_reg->limit_at_directory_n)                                            \
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ret_val);              \
                                                                                                           \
		if(fs_reg->limit_at_file_n)                                                                        \
		if(fs_reg->file_n >= fs_reg->limit_at_file_n)                                                      \
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ret_val);              \
                                                                                                           \
		if(fs_reg->limit_at_sock_n)                                                                        \
		if(fs_reg->sock_n >= fs_reg->limit_at_sock_n)                                                      \
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ret_val);              \
                                                                                                           \
		if(fs_reg->limit_at_chr_n)                                                                         \
		if(fs_reg->chr_n >= fs_reg->limit_at_chr_n)                                                        \
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ret_val);              \
                                                                                                           \
		if(fs_reg->limit_at_blk_n)                                                                         \
		if(fs_reg->blk_n >= fs_reg->limit_at_blk_n)                                                        \
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ret_val);              \
                                                                                                           \
		if(fs_reg->limit_at_symlink_n)                                                                     \
		if(fs_reg->symlink_n >= fs_reg->limit_at_symlink_n)                                                \
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ret_val);              \
                                                                                                           \
		if(fs_reg->limit_at_fifo_n)                                                                        \
		if(fs_reg->fifo_n >= fs_reg->limit_at_fifo_n)                                                      \
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ret_val);              \
                                                                                                           \
	  	if(fs_reg->limit_at_total_alloc_size)                                                              \
		if(fs_reg->hreg->memory_table->managed_heap_alloc_total_size >= fs_reg->limit_at_total_alloc_size) \
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ret_val);              \



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Policy Enforcement %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Policy Enforcement Cascading:
//
//  first policy: symlink policy
//  second policy: hidden file policy
//  third policy: fifo policy
//  second policy: directory policy
//  third policy: file policy


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Symlink Policy %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Symlink policy the fs registry routines will follow when
// working with symlinks.
typedef enum
{

	BP_FS_SYMLINK_POLICY_FLAG_IGNORE_ALL         = 1,
	BP_FS_SYMLINK_POLICY_FLAG_ONLY               = 2,
	BP_FS_SYMLINK_POLICY_FLAG_FOLLOW_DIRECTORIES = 3

} BP_FSREG_SYMLINK_POLICY_FLAG;

// Binary flag array of any of the BP_FSREG_SYMLINK_POLICY_FLAG values
// (set with BP_SET_FLAG()) shown above.
typedef BP_FLAGS_T BP_FSREG_SYMLINK_POLICY;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Hidden Policy %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Hidden policy the fs registry routines will follow when
// working with Hidden files.
typedef enum
{

	BP_FS_HIDDEN_POLICY_FLAG_IGNORE_ALL                      = 1,
	BP_FS_HIDDEN_POLICY_FLAG_ONLY                            = 2,
	BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_HIDDEN_DIRECTORIES      = 3,
	BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_HIDDEN_FILES            = 4,
	BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_ONLY_HIDDEN_DIRECTORIES = 5,
	BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_ONLY_HIDDEN_FILES       = 6,

} BP_FSREG_HIDDEN_POLICY_FLAG;


// Binary flag array of any of the BP_FSREG_HIDDEN_POLICY_FLAG values
// (set with BP_SET_FLAG()) shown above.
typedef BP_FLAGS_T BP_FSREG_HIDDEN_POLICY;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Fifo Policy %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Fifo policy the fs registry routines will follow when
// working with Fifos.
typedef enum
{

	BP_FS_FIFO_POLICY_FLAG_IGNORE_ALL         = 1,
	BP_FS_FIFO_POLICY_FLAG_ONLY               = 2


} BP_FSREG_FIFO_POLICY_FLAG;


// Binary flag array of any of the BP_FSREG_FIFO_POLICY_FLAG values
// (set with BP_SET_FLAG()) shown above.
typedef BP_FLAGS_T BP_FSREG_FIFO_POLICY;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Directory Policy %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Symlink policy the fs registry routines will follow when
// working with symlinks.
typedef enum
{

	BP_FS_DIRECTORY_POLICY_FLAG_IGNORE_ALL                 = 1,
	BP_FS_DIRECTORY_POLICY_FLAG_ONLY                       = 2,
	BP_FS_DIRECTORY_POLICY_FLAG_READABLE_BY_CURRENT_USER   = 3,
	BP_FS_DIRECTORY_POLICY_FLAG_WRITABLE_BY_CURRENT_USER   = 4,
	BP_FS_DIRECTORY_POLICY_FLAG_EXECUTABLE_BY_CURRENT_USER = 5,
	BP_FS_DIRECTORY_POLICY_FLAG_SUID                       = 6, /* unsupported on most systems */
	BP_FS_DIRECTORY_POLICY_FLAG_SGID                       = 7  /* new files in directory will support group inheritance */

} BP_FSREG_DIRECTORY_POLICY_FLAG;


// Binary flag array of any of the BP_FSREG_DIRECTORY_POLICY_FLAG values
// (set with BP_SET_FLAG()) shown above.
typedef BP_FLAGS_T BP_FSREG_DIRECTORY_POLICY;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% File Policy %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// File policy the fs registry routines will follow when
// working with standard files.
typedef enum
{

	BP_FS_FILE_POLICY_FLAG_IGNORE_ALL                        = 1,
	BP_FS_FILE_POLICY_FLAG_ONLY                              = 2,
	BP_FS_FILE_POLICY_FLAG_NOT_OWNED_BY_CURRENT_USER         = 3,
	BP_FS_FILE_POLICY_FLAG_OWNED_BY_CURRENT_USER             = 4,
	BP_FS_FILE_POLICY_FLAG_READABLE_BY_CURRENT_USER          = 5,
	BP_FS_FILE_POLICY_FLAG_WRITABLE_BY_CURRENT_USER          = 6,
	BP_FS_FILE_POLICY_FLAG_EXECUTABLE_BY_CURRENT_USER        = 7,
	BP_FS_FILE_POLICY_FLAG_SUID                              = 8,
	BP_FS_FILE_POLICY_FLAG_SGID                              = 9,
	BP_FS_FILE_POLICY_FLAG_OWNER_HAS_HIGHER_UID_THAN_CURRENT = 10,
	BP_FS_FILE_POLICY_FLAG_OWNER_HAS_LOWER_UID_THAN_CURRENT  = 11

} BP_FSREG_FILE_POLICY_FLAG;


// Binary flag array of any of the BP_FSREG_HIDDEN_POLICY_FLAG values
// (set with BP_SET_FLAG()) shown above.
typedef BP_FLAGS_T BP_FSREG_FILE_POLICY;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Character Device File Policy %%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// File policy the fs registry routines will follow when
// working with standard files.
typedef enum
{

	BP_FS_CHR_POLICY_FLAG_IGNORE_ALL                 = 1,
	BP_FS_CHR_POLICY_FLAG_ONLY                       = 2

} BP_FSREG_CHR_POLICY_FLAG;


// character device policy
typedef BP_FLAGS_T BP_FSREG_CHR_POLICY;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Block Device File Policy %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// File policy the fs registry routines will follow when
// working with standard files.
typedef enum
{

	BP_FS_BLK_POLICY_FLAG_IGNORE_ALL                 = 1,
	BP_FS_BLK_POLICY_FLAG_ONLY                       = 2

} BP_FSREG_BLK_POLICY_FLAG;


// block device policy
typedef BP_FLAGS_T BP_FSREG_BLK_POLICY;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Socket File Policy %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// socket policy enum
typedef enum
{

	BP_FS_SOCK_POLICY_FLAG_IGNORE_ALL                 = 1,
	BP_FS_SOCK_POLICY_FLAG_ONLY                       = 2

} BP_FSREG_SOCK_POLICY_FLAG;


// socket device policy
typedef BP_FLAGS_T BP_FSREG_SOCK_POLICY;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% PRegexp Policy %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// policy enum (used for adding regex policies, determines
// which string stack a policy will be added to)
typedef enum
{
	BP_FSREG_PREG_POLICY_TYPE_UNSET,
	BP_FSREG_PREG_POLICY_TYPE_FNAME_MATCH,
	BP_FSREG_PREG_POLICY_TYPE_PATHONLY_MATCH,
	BP_FSREG_PREG_POLICY_TYPE_FULLPATH_MATCH

} BP_FSREG_PREG_POLICY_TYPE;





// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Policies Structure %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This structure holds the individual polices used by the registry
// crawler to do its crawling.
typedef struct __BP_FSREG_POLICIES {

	// flags indicating the symlink policy
	BP_FSREG_SYMLINK_POLICY   symlink_policy;
	size_t                    symlink_policy_matched_n;

	// flags indicating hidden file policy
	BP_FSREG_HIDDEN_POLICY    hidden_policy;
	size_t                    hidden_policy_matched_n;

	// flags indicating fifo policy
	BP_FSREG_FIFO_POLICY      fifo_policy;
	size_t                    fifo_policy_matched_n;

	// flags indicating the directory crawl policy
	BP_FSREG_DIRECTORY_POLICY directory_policy;
	size_t                    directory_policy_matched_n;

	// flags indicating file matching policy
	BP_FSREG_FILE_POLICY      file_policy;
	size_t                    file_policy_matched_n;

	// flags indicating character device matching policy
	BP_FSREG_CHR_POLICY       chr_policy;
	size_t                    chr_policy_matched_n;

	// flags indicating block device matching policy
	BP_FSREG_BLK_POLICY       blk_policy;
	size_t                    blk_policy_matched_n;

	// flags indicating socket matching policy
	BP_FSREG_SOCK_POLICY      sock_policy;
	size_t                    sock_policy_matched_n;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Regular Expression Policies %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// filename matching policies
	char ** preg_fname_policies;
	size_t  preg_fname_policies_n;

	// path-only matching policies
	char ** preg_pathonly_matching_policies;
	size_t  preg_pathonly_matching_policies_n;

	// full path matching policies
	char ** preg_fullpath_matching_policies;
	size_t  preg_fullpath_matching_policies_n;


} BP_FSREG_POLICIES, *P_BP_FSREG_POLICIES;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Policy Analysis %%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



#define BP_FSREG_POLICY_RUN_ONLY_CHECK(type_indicator, type_indicator_flag)\
if\
(\
		BP_FLAG_IS_SET(fs_reg->crawl_policies.type_indicator##_policy, type_indicator_flag)\
)\
{\
\
	\
	if(!policy_analysis_inout->type_indicator##_only_match)\
	{\
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, BP_FALSE);\
	}\
	\
}\




// run the ignore policy type
#define BP_FSREG_POLICY_RUN_IGNORE_CHECK(type_indicator)\
if\
(\
		policy_analysis_inout->type_indicator##_ignore_all_match &&\
		policy_analysis_inout->is_##type_indicator\
)\
{\
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, BP_FALSE);\
}


// Analysis structure passed between policy routines (used for
// correlating individual data sets).  One structure is used
// as a matching join for comparing files with regard to policy
// enforcement.
typedef struct __BP_FSREG_FILE_POLICY_ANALYSIS {

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Simple Indicator Flags %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// policy check indicates this is a symlink
	BP_BOOL is_symlink;

	// policy check indicates this is a hidden file
	BP_BOOL is_hidden;

	// policy check indicates this is a fifo
	BP_BOOL is_fifo;

	// policy check indicates that this is a directory
	BP_BOOL is_directory;

	// policy check indicates that this is a file
	BP_BOOL is_file;

	// -- todo

	// policy check indicates that this is a character device
	BP_BOOL is_chr;

	// policy check indicates that this is a block device
	BP_BOOL is_blk;

	// policy check indicates that this is a socket
	BP_BOOL is_sock;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Symlink Matches %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// symlink ignore all matched
	BP_BOOL symlink_ignore_all_match;

	// symlink only
	BP_BOOL symlink_only_match;

	// symlink follow directories matched (aka is a symlinked directory
	// and is also the flag is set to follow).  This is primarily
	// used for generating recursing entries.
	BP_BOOL symlink_follow_directories_match;

	// set to true if the target is a file
	BP_BOOL symlink_target_is_file;

	// set to true if the target is a directory
	BP_BOOL symlink_target_is_directory;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Hidden File Matches %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// hidden only
	BP_BOOL hidden_only_match;

	// hidden file ignore all match
	BP_BOOL hidden_ignore_all_match;



	// only within a hidden parent
	BP_BOOL hidden_only_within_hidden_parent_match;

	// hidden examine directories match
	BP_BOOL hidden_examine_directories_match;

	// hidden file examine match
	BP_BOOL hidden_examine_files_match;

	// match only hidden directories
	BP_BOOL hidden_examine_only_directories_match;

	// match only hidden files
	BP_BOOL hidden_examine_only_files_match;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Fifo Matches %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// fifo only
	BP_BOOL fifo_only_match;

	// fifo ignore all matched
	BP_BOOL fifo_ignore_all_match;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Directory Matches %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// directory only match
	BP_BOOL directory_only_match;

	// ignore all directories
	BP_BOOL directory_ignore_all_match;

	// directory is readable by current user
	BP_BOOL directory_readable_by_current_user_match;

	// directory is writable by current user
	BP_BOOL directory_writable_by_current_user_match;

	// directory is executable by current user
	BP_BOOL directory_executable_by_current_user_match;

	// directory has suid bit set for some reason
	BP_BOOL directory_suid_bit_match;

	// directory has sgid bit (inheriting gid)
	BP_BOOL directory_sgid_bit_match;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% File Matches %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// file only match
	BP_BOOL file_only_match;

	// file ignore match
	BP_BOOL file_ignore_all_match;

	// file is owned by current user
	BP_BOOL file_owned_by_current_user;

	// file is not_owned by current user
	BP_BOOL file_not_owned_by_current_user;


	// file is readable by current user
	BP_BOOL file_readable_by_current_user_match;
	BP_BOOL file_readable_by_current_user_because_user_is_root_match;
	BP_BOOL file_readable_by_current_user_because_user_is_owner_match;
	BP_BOOL file_readable_by_current_user_because_group_match;
	BP_BOOL file_readable_by_current_user_because_world_match;

	// file is writable by current user
	BP_BOOL file_writable_by_current_user_match;
	BP_BOOL file_writable_by_current_user_because_user_is_root_match;
	BP_BOOL file_writable_by_current_user_because_user_is_owner_match;
	BP_BOOL file_writable_by_current_user_because_group_match;
	BP_BOOL file_writable_by_current_user_because_world_match;

	// file is executable by current user
	BP_BOOL file_executable_by_current_user_match;
	BP_BOOL file_executable_by_current_user_because_user_is_root_match;
	BP_BOOL file_executable_by_current_user_because_user_is_owner_match;
	BP_BOOL file_executable_by_current_user_because_group_match;
	BP_BOOL file_executable_by_current_user_because_world_match;


	// file has suid bit set
	BP_BOOL file_suid_bit_match;

	// file has sgid bit set
	BP_BOOL file_sgid_bit_match;

	// the owner of this file has a id higher than the current user
	BP_BOOL file_owner_has_higher_uid_than_current;

	// the owner of this file has a id lower than the current user
	BP_BOOL file_owner_has_lower_uid_than_current;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Char Device Matches %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// chr only match
	BP_BOOL chr_only_match;

	// chr ignore match
	BP_BOOL chr_ignore_all_match;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Block Device Matches %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// blk only match
	BP_BOOL blk_only_match;

	// blk ignore match
	BP_BOOL blk_ignore_all_match;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Sock Matches %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// sock only match
	BP_BOOL sock_only_match;

	// sock ignore match
	BP_BOOL sock_ignore_all_match;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Preg Matches %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// filename matched the filename policy (set to true by
	// default if no policies are set)
	BP_BOOL fname_preg_policies_match;

	// path-only part matched the path-only policy (set to true by
	// default if no policies are set)
	BP_BOOL pathonly_preg_policies_match;

	// fullpath matched the fullpath policy (set to true by
	// default if no policies are set)
	BP_BOOL fullpath_preg_policies_match;



} BP_FSREG_FILE_POLICY_ANALYSIS, *P_BP_FSREG_FILE_POLICY_ANALYSIS;







// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Filesystem Registry Handle %%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// entry type
typedef enum
{
	BP_FSREG_ENTRY_TYPE_UNSET   = 0,
	BP_FSREG_ENTRY_TYPE_DIR     = DT_DIR,
	BP_FSREG_ENTRY_TYPE_CHR     = DT_CHR,
	BP_FSREG_ENTRY_TYPE_BLK     = DT_BLK,
	BP_FSREG_ENTRY_TYPE_REG     = DT_REG,
	BP_FSREG_ENTRY_TYPE_FIFO    = DT_FIFO,
	BP_FSREG_ENTRY_TYPE_LNK     = DT_LNK,
	BP_FSREG_ENTRY_TYPE_SOCK    = DT_SOCK,
	BP_FSREG_ENTRY_TYPE_UNKNOWN = DT_UNKNOWN

} BP_FSREG_ENTRY_TYPE;



// The registry is filled with these structures
typedef struct __BP_FSREG_ENTRY {


	// entry type
	BP_FSREG_ENTRY_TYPE type;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Directory Entry Info %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// full path
	char * full_path;

	// dirent entry information (copied)
	struct dirent dirent_data;

	// stat data (from stat() call).  This
	// has resolved symlink data, but no symlink data.  Use lstat_data
	// if you want to see symlink info.
	struct stat stat_data;

	// stat data (from lstat() call).  This has
	// symlink data unresolved.  Use stat_data if you
	// want to see resolved info.
	struct stat lstat_data;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Policy Analysis %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if the policy matched, this flag is set
	BP_BOOL policy_matched;

	// this structure contains all the flags relating to
	// policy analysis.
	BP_FSREG_FILE_POLICY_ANALYSIS policy_analysis;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Indicators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if this is set, this entry represents the directory
	// entry for the current working directory (the hash
	// table-type entry for the current directory is used
	// for maintaining table data and can't maintain specific
	// data which can be easily stored in a BP_FSREG_ENTRY)
	BP_BOOL is_current_directory;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Directory Handles %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// if this is a directory type, and the directory is open
	// this is set to true.
	BP_BOOL dir_open;

	// if the entry is a directory, this will hold a pointer if
	// the directory is open.
	DIR * dir_pointer;


} BP_FSREG_ENTRY, *P_BP_FSREG_ENTRY;


typedef struct __BP_FSREG {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Blockparty Hash Registry For Files %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// hash table registry used for representing the file system
	P_BP_HASH_TABLE_REGISTRY hreg;

	// top directory (reasonable) string set as directory.
	char * top_dir;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Info %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// since this runs a stat on all files in the
	// specified directory, it can also check to see
	// the sizes of files individually.  This member is
	// updated every time that a check is performed.  This
	// is specfically to allow us to monitor directories to make sure
	// they're not growing too large.
	size_t full_size_of_all_files;

	// this is similar to above, except this is just the size of
	// the files which were initially selected during crawling.
	size_t full_size_of_selected_files;

	// The depth of the lowest recursing.
	size_t lowest_recursion_directory;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Counters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// count of entries encountered
	size_t entry_n;

	// individual type counters
	size_t directory_n;
	size_t file_n;
	size_t sock_n;
	size_t chr_n;
	size_t blk_n;
	size_t symlink_n;
	size_t fifo_n;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Indicators %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// --- completion indicators --

	// If the crawler has completed crawling, this is set.
	BP_BOOL crawling_finished;

	// if the crawler finished ok this is set (meaning no limits were hit)
	BP_BOOL crawling_finished_ok;

	// --- recursion limit indicators ------

	// if this flag is present, the filesystem registry parser has reached
	// its maximum depth.
	BP_BOOL recursion_max_directory_depth_reached;

	// If this flag is present, the lowest recursion level
	BP_BOOL recursed_bottom_directory_has_child_directory;

	// --- basic directory indicators ----

	// Indicates whether or not the registry contains at least
	// one subdirectory.
	BP_BOOL contains_directory;

	// Indicates whether or not a directory is hidden.
	BP_BOOL contains_hidden_directories;

	// Indicates whether or not a directory contains a FIFO.
	BP_BOOL contains_fifo;

	// Indicates whether or not the directory contains symbolic links.
	BP_BOOL contains_symlink;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Limiters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Since our registry can track allocation sizes, we can force
	// a limit on the total allocation size possible before exiting.
	// Set this to a size in bytes, and when a allocation crosses that
	// threshold, allocation will exit.  This can be useful for
	// if allocations are too large.
	size_t limit_at_total_alloc_size;

	// count of entries encountered
	size_t limit_at_entry_n;

	// individual type counters
	size_t limit_at_directory_n;
	size_t limit_at_file_n;
	size_t limit_at_sock_n;
	size_t limit_at_chr_n;
	size_t limit_at_blk_n;
	size_t limit_at_symlink_n;
	size_t limit_at_fifo_n;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Crawl Policies %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// array of user set policies to determine what files are crawled
	// and how.
	BP_FSREG_POLICIES crawl_policies;




} BP_FSREG, *P_BP_FSREG;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Open / Close Filesystem Registries %%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Open a handle to the filesystem.
P_BP_FSREG BP_FSRegOpen
(
		char *  fs_dir,
		BP_BOOL recurse,
		size_t  max_recurse_depth
);

// Closes a handle and destroys all associated memory.
BP_ERROR_T BP_FSRegCloseDestroy
(
		P_BP_FSREG fs_reg
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Flag Policy Settrs %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



// ---- Individual Policy Settrs -----

BP_ERROR_T BP_FSRegSetSymlinkPolicy
(
	P_BP_FSREG fs_reg,
	BP_FSREG_SYMLINK_POLICY_FLAG flag,
	BP_BOOL setting
);

BP_ERROR_T BP_FSRegSetHiddenPolicy
(
	P_BP_FSREG fs_reg,
	BP_FSREG_HIDDEN_POLICY_FLAG flag,
	BP_BOOL setting
);

BP_ERROR_T BP_FSRegSetFifoPolicy
(
	P_BP_FSREG fs_reg,
	BP_FSREG_FIFO_POLICY_FLAG flag,
	BP_BOOL setting
);

BP_ERROR_T BP_FSRegSetDirectoryPolicy
(
	P_BP_FSREG fs_reg,
	BP_FSREG_DIRECTORY_POLICY_FLAG flag,
	BP_BOOL settingq
);

BP_ERROR_T BP_FSRegSetFilePolicy
(
	P_BP_FSREG fs_reg,
	BP_FSREG_FILE_POLICY_FLAG flag,
	BP_BOOL setting
);

BP_ERROR_T BP_FSRegSetChrPolicy
(
	P_BP_FSREG fs_reg,
	BP_FSREG_CHR_POLICY_FLAG flag,
	BP_BOOL setting
);

BP_ERROR_T BP_FSRegSetBlkPolicy
(
	P_BP_FSREG fs_reg,
	BP_FSREG_BLK_POLICY_FLAG flag,
	BP_BOOL setting
);

BP_ERROR_T BP_FSRegSetSockPolicy
(
		P_BP_FSREG fs_reg,
		BP_FSREG_SOCK_POLICY_FLAG flag,
		BP_BOOL setting
);



// this sets a perl compatible regular expression policy for
// filesystem crawls.
BP_ERROR_T BP_FSRegAddPregexPolicy
(
	P_BP_FSREG                fs_reg,
	BP_FSREG_PREG_POLICY_TYPE preg_policy_type,
	char *                    pregex
);

// Selection iterator for policy.
typedef enum
{

	BP_FSREG_POLICY_TYPE_UNSET    ,
	BP_FSREG_POLICY_TYPE_SYMLINK  ,
	BP_FSREG_POLICY_TYPE_HIDDEN   ,
	BP_FSREG_POLICY_TYPE_FIFO     ,
	BP_FSREG_POLICY_TYPE_DIRECTORY,
	BP_FSREG_POLICY_TYPE_FILE     ,
	BP_FSREG_POLICY_TYPE_CHR      ,
	BP_FSREG_POLICY_TYPE_BLK      ,
	BP_FSREG_POLICY_TYPE_SOCK

} BP_FSREG_POLICY_TYPE;


// ---- Multi Policy Settrs ----------

// set the policy
BP_ERROR_T BP_FSRegPolicySetFlag
(
	P_BP_FSREG           fs_reg,
	BP_FSREG_POLICY_TYPE policy_type,
	size_t               flag_id,
	BP_BOOL              flag_value

);

// A set of policy flags.
typedef struct __BP_FSREG_POLICY_FLAG_SET {

	// id of the flag being set
	size_t flag_id;

	// value being set
	BP_BOOL flag_value;

} BP_FSREG_POLICY_FLAG_SET, *P_BP_FSREG_POLICY_FLAG_SET;

// This is used to terminate flag sets (when passed to BP_FSRegPolicySetFlags).  It's
// essentially just a null record.
#define BP_FSREG_FLAG_SET_TERMINATOR\
		{0,BP_FALSE}


// set the policy
BP_ERROR_T BP_FSRegPolicySetFlags
(
	P_BP_FSREG               fs_reg,
	BP_FSREG_POLICY_TYPE     policy_type,
	P_BP_FSREG_POLICY_FLAG_SET policy_flag_set
);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Policy Matching: Checks %%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



// macro for parameter definitions for a check file function.
#define BP_FSREG_CHECK_FILE_FUNC_PARMS\
		P_BP_FSREG                      fs_reg,               \
		P_BP_FSREG_POLICIES             fs_reg_pol,           \
		P_BP_HASH_TABLE_KEYED_ENTRY     parent_table,         \
		char *                          filename,             \
		char *                          path,                 \
		char *                          full_path,            \
		struct stat *                   file_stat,            \
		struct stat *                   file_lstat,           \
		struct dirent *                 file_dirent,          \
		P_BP_FSREG_FILE_POLICY_ANALYSIS policy_analysis_inout

// passthrough for passing args
#define BP_FSREG_CHECK_FILE_FUNC_PARMS_PASSTHROUGH\
		fs_reg,               \
		fs_reg_pol,           \
		parent_table,         \
		filename,             \
		path,                 \
		full_path,            \
		file_stat,            \
		file_lstat,           \
		file_dirent,          \
		policy_analysis_inout

// this is a simple entry macro for holding policy function entry logic (null checks
// mainly).  Parameter is the value you want to return on check failure (opaque type)
#define BP_FSREG_CHECK_POLICY_FUNC_ENTRY_RUN_NULL_CHECKS(return_value_on_failure)\
		if(!fs_reg)                            \
			return return_value_on_failure;    \
		if(!fs_reg->hreg)                      \
			return return_value_on_failure;    \
		if(!fs_reg_pol)                        \
			return return_value_on_failure;    \
		if(!filename)                          \
			return return_value_on_failure;    \
		if(!file_stat)                         \
			return return_value_on_failure;    \
		if(!file_lstat)                        \
			return return_value_on_failure;    \
		if(!path)                              \
			return return_value_on_failure;    \
		if(!full_path)                         \
			return return_value_on_failure;    \
		if(!file_dirent)                       \
			return return_value_on_failure;    \
		if(!policy_analysis_inout)             \
			return return_value_on_failure;


// DEVELOPER NOTE: *Check* functions only store boolean policy flags which match developer flag .
//                 configurations You need to first call a check function to store matching flags, then call
//                 the corresponding enforce functions (with the returned checked flags) to enforce the
//                 individual policy. Enforcement functions return boolean values which can let you know
//                 immediately if the configured policy fails for the currently analyzed file.



// This will check a files stat, dirent, and filename policies and will
// return true or false based around policy matching.  The result of the
// analysis will be stored in a BP_FSREG_FILE_POLICY_ANALYSIS structure
// pointed to by policy_analysis_inout.  After the function returns, you can
// examine policy_analysis_inout to check policy flag configurations.
BP_BOOL BP_FSRegCheckFileAgainstCrawlPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS);


// --- individual policy check passthroughs (called from BP_FSRegCheckFileAgainstCrawlPolicy) ---

// symlink policy checker (ready for testing)
BP_BOOL BP_FSRegCheckFileAgainstCrawlSymlinkPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS);

// hidden policy checker (ready for testing)
BP_BOOL BP_FSRegCheckFileAgainstCrawlHiddenPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS);

// fifo policy checker (ready for testing)
BP_BOOL BP_FSRegCheckFileAgainstCrawlFifoPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS);

// directory policy checker (ready for testing)
BP_BOOL BP_FSRegCheckFileAgainstCrawlDirectoryPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS);

// file policy checker
BP_BOOL BP_FSRegCheckFileAgainstCrawlFilePolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS);

// chr device policy checker
BP_BOOL BP_FSRegCheckFileAgainstCrawlChrDevicePolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS);

// block device policy checker
BP_BOOL BP_FSRegCheckFileAgainstCrawlBlockDevicePolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS);

// socket policy checker
BP_BOOL BP_FSRegCheckFileAgainstCrawlSocketPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS);

// filename perl compatible regex checker
BP_BOOL BP_FSRegCheckFileAgainstCrawlFilenamePregPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS);




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Policy Matching: Enforcement %%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Individual policy check enforcers (these check policies 1 to 1 with the flags returned
// returned from any of the above check functions (typically they're all called successively)

// symlink policy enforcer
BP_BOOL BP_FSRegEnforceFileAgainstCrawlSymlinkPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS);

// hidden policy enforcer
BP_BOOL BP_FSRegEnforceFileAgainstCrawlHiddenPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS);

// fifo policy enforcer
BP_BOOL BP_FSRegEnforceFileAgainstCrawlFifoPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS);

// directory policy enforcer
BP_BOOL BP_FSRegEnforceFileAgainstCrawlDirectoryPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS);

// file policy enforcer
BP_BOOL BP_FSRegEnforceFileAgainstCrawlFilePolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS);

// chr device policy enforcer
BP_BOOL BP_FSRegEnforceFileAgainstCrawlChrDevicePolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS);

// block device policy enforcer
BP_BOOL BP_FSRegEnforceFileAgainstCrawlBlockDevicePolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS);

// socket policy enforcer
BP_BOOL BP_FSRegEnforceFileAgainstCrawlSocketPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS);

// filename perl compatible regex enforcer
BP_BOOL BP_FSRegEnforceFileAgainstCrawlFilenamePregPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Filesystem Record Addition %%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This adds an arbitrary entry to fs_reg->hreg registry
// based on the table pointer supplied.  This routine
// performs a relative add.  This function returns a pointer
// to the newly added keyed entry, or NULL if the record
// cannot be created.  A prime use of this add->return functionality
// is to provide reentrant keyed entry pointers for recursing
// directories. (aka, add directory record, recurse with that record using BP_FSRegCrawl_r)
P_BP_FSREG_ENTRY BP_FSRegAddRecord
(
		P_BP_FSREG                  fs_reg,
		P_BP_HASH_TABLE_KEYED_ENTRY add_to_this_table,
		struct dirent *             add_this_entry
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Filesystem Crawler %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// USE THIS: Attempt to crawl the filesystem based on the registry configuration.  This should be used
// unless you've studied BP_FSRegCrawl_r and understand how to use it safely on it's own.  Basically,
// use this in all cases, don't use BP_FSRegCrawl_r directly.
BP_ERROR_T BP_FSRegCrawl
(
		P_BP_FSREG fs_reg
);

// DO NOT USE THIS DIRECTLY:
// Attempt to crawl the filesystem based on the registry configuration, recursively with a
// recursing handle set.
BP_ERROR_T BP_FSRegCrawl_r
(
		P_BP_FSREG fs_reg,
		char * crawl_dir,
		P_BP_HASH_TABLE_KEYED_ENTRY parent_dir
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Filesystem Lookups %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to lookup a keyed entry within the fs registry.  If you don't
// treat this function with care with regard to user input, it can end badly
// with potential traversals or other undefined behavior ONLY IF the lookups
// are used in an unexpected way (within your own code).  This lookup
// itself is fairly safe from misues.
P_BP_HASH_TABLE_KEYED_ENTRY BP_FSRegLookupEntryFromPath
(
	P_BP_FSREG fs_reg,
	char * filesystem_path
);

// This attempts to lookup a filesystem path from an entry by crawling backwards
// to the root directory.  The result is stored in the hash table, so it
// must be free'd later with ht_free.  The result from this should be valid
// for a filesystem lookup (aka BP_FOpen etc).  This routine automatically
// strips out double slashes.
char * BP_FSRegLookupFilesystemPathFromEntry
(
		P_BP_FSREG                  fs_reg,
		P_BP_HASH_TABLE_KEYED_ENTRY lookup_entry
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Special Easy Creators %%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Runs a filesystem search from a specified root.  Returns all files
P_BP_FSREG BP_FSRegOpenSearchForSuidSgid
(
		char * search_dir,
		BP_BOOL suid,
		BP_BOOL sgid
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Display an entier filesystem registry
BP_ERROR_T BP_FSRegDisplay(P_BP_FSREG fs_reg);

// Display a filesystem path in a registry.
BP_ERROR_T BP_FSRegDisplayPath(P_BP_FSREG fs_reg, char ** path);

// Display a filesystem path in a registry, recursing.
BP_ERROR_T BP_FSRegDisplayPath_r(P_BP_FSREG fs_reg, char ** path);


// ---- Display Policies -------------

// display string arrays fof preg policies
BP_ERROR_T BP_FSRegDisplayPregPolicy(P_BP_FSREG_POLICIES policies);

// display flags indicating the symlink policy
BP_ERROR_T BP_FSRegDisplaySymlinkPolicy(BP_FSREG_SYMLINK_POLICY symlink_policy);

// display flags indicating hidden file policy
BP_ERROR_T BP_FSRegDisplayHiddenPolicy(BP_FSREG_HIDDEN_POLICY hidden_policy);

// display flags indicating fifo policy
BP_ERROR_T BP_FSRegDisplayFifoPolicy(BP_FSREG_FIFO_POLICY fifo_policy);

// display flags indicating the directory crawl policy
BP_ERROR_T BP_FSRegDisplayDirectoryPolicy(BP_FSREG_DIRECTORY_POLICY directory_policy);

// display flags indicating file matching policy
BP_ERROR_T BP_FSRegDisplayFilePolicy(BP_FSREG_FILE_POLICY file_policy);


// display flags indicating chr matching policy
BP_ERROR_T BP_FSRegDisplayChrPolicy(BP_FSREG_CHR_POLICY chr_policy);

// display flags indicating blk matching policy
BP_ERROR_T BP_FSRegDisplayBlkPolicy(BP_FSREG_BLK_POLICY blk_policy);

// display flags indicating sock matching policy
BP_ERROR_T BP_FSRegDisplaySockPolicy(BP_FSREG_SOCK_POLICY sock_policy);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Filesystem %%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// this will display all items in a filesystem.  A policy filter can be applied to selection.
BP_ERROR_T BP_FSRegDisplayFilesystem
(
		P_BP_FSREG fs_reg,
		BP_BOOL display_files,
		BP_BOOL display_directories,
		BP_BOOL recurse,
		P_BP_FSREG_POLICIES policy_filter
);

// display directory entry (recurses)
BP_ERROR_T BP_FSRegDisplayFilesystemDirectory
(
		P_BP_FSREG                  fs_reg,
		P_BP_HASH_TABLE_KEYED_ENTRY dir,
		BP_BOOL                     recurse,
		size_t                      tab_depth

);

// display the filesystem entry
BP_ERROR_T BP_FSRegDisplayFilesystemEntry
(
		P_BP_FSREG                  fs_reg,
		P_BP_HASH_TABLE_KEYED_ENTRY file_entry,
		BP_BOOL                     display_dirent,
		BP_BOOL                     display_stat,
		BP_BOOL                     display_path,
		size_t                      tab_depth
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Files %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Simple utility routine for displaying a list of files stored in the registry
// by using a linear search.

// this will display all items in a filesystem.  A policy filter can be applied to selection.
BP_ERROR_T BP_FSRegDisplayFiles
(
		P_BP_FSREG fs_reg,
		BP_BOOL display_files,
		BP_BOOL display_directories,
		BP_BOOL recurse,
		P_BP_FSREG_POLICIES policy_filter
);

// display directory entry (recurses)
BP_ERROR_T BP_FSRegDisplayFilesDirectory
(
		P_BP_FSREG                  fs_reg,
		P_BP_HASH_TABLE_KEYED_ENTRY dir,
		BP_BOOL                     recurse,
		size_t                      tab_depth

);

// display the filesystem entry
BP_ERROR_T BP_FSRegDisplayFilesEntry
(
		P_BP_FSREG                  fs_reg,
		P_BP_HASH_TABLE_KEYED_ENTRY file_entry,
		BP_BOOL                     display_dirent,
		BP_BOOL                     display_stat,
		BP_BOOL                     display_path,
		size_t                      tab_depth
);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Policy Analysis %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This is simply used to run inline checks within BP_FSRegDisplayFilePolicyAnalysis.  You shouldn't
// use it directly unless you have some specific/arbitrary reason I can't think of.
#define BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, check_symbol, symbol_string, space_buff)\
		if(only_display_matched)\
		{\
			if(policy_analysis->check_symbol)\
			{\
				printf("\n%s\t%s:%s%s", optional_front_buffer ? optional_front_buffer : "",  symbol_string, space_buff,policy_analysis->check_symbol ? "matched" : "no");\
			}\
		}\
		else\
		{\
			printf("\n%s\t%s:%s%s", optional_front_buffer ? optional_front_buffer : "", symbol_string, space_buff, policy_analysis->check_symbol ? "matched" : "no");\
		}


BP_ERROR_T BP_FSRegDisplayFilePolicyAnalysis
(
	P_BP_FSREG_FILE_POLICY_ANALYSIS policy_analysis,
	BP_BOOL                         only_display_matched,
	char *                          optional_front_buffer
);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Utility Path Retrieval Routines %%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This will generate a char ** of all the full paths to files found within
// the hash file registry.  This array is generated for the use in outside
// applications and uses bpcalloc to allocate the returned data.  This data
// is not managed by the filesystem registry.
char ** BP_FSRegRetrieveEntryFullPathsAsBPCallocedNullTerminatedPointerArray
(
		P_BP_FSREG fs_reg,
		P_BP_FSREG_FILE_POLICY_ANALYSIS optional_policy_analysis_filter
);


// this is an internal function, do not use this
// Simple internal routine to parse individual entries (needed for recursion)
BP_ERROR_T BP_FSRegRetrieveEntryFullPathsAsBPCallocedNullTerminatedPointerArray__dir_r
(
		P_BP_FSREG                      fs_reg,
		P_BP_HASH_TABLE_KEYED_ENTRY     dir,
		P_BP_FSREG_FILE_POLICY_ANALYSIS optional_policy_analysis_filter,
		char ***                        entries_in_out,
		size_t *                        entries_n
);

// this is an internal function, do not use this
// Simple internal routine to parse individual entries (needed for recursion)
BP_ERROR_T BP_FSRegRetrieveEntryFullPathsAsBPCallocedNullTerminatedPointerArray__entry_r
(
		P_BP_FSREG                      fs_reg,
		P_BP_HASH_TABLE_KEYED_ENTRY     file_entry,
		P_BP_FSREG_FILE_POLICY_ANALYSIS optional_policy_analysis_filter,
		char ***                        entries_in_out,
		size_t *                        entries_n
);

#endif /* __BP_FILESYSTEMREGISTRY_H_ */

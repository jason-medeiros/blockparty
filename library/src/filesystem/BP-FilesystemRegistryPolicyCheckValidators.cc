/*
 * BP-FilesystemRegistryPolicyValidators.cc
 *
 *  Created on: Jul 27, 2015
 *      Author: root
 */



// main include file
#include "../../include/BP-Main.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Policy Matching %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This will check a files stat, dirent, and filename policies and will
// return true or false based around policy matching.
BP_BOOL BP_FSRegCheckFileAgainstCrawlPolicy
(
		P_BP_FSREG                      fs_reg,
		P_BP_FSREG_POLICIES             fs_reg_pol,
		P_BP_HASH_TABLE_KEYED_ENTRY     parent_table,
		char *                          filename,
		char *                          path,
		char *                          full_path,
		struct stat *                   file_stat,
		struct stat *                   file_lstat,
		struct dirent *                 file_dirent,
		P_BP_FSREG_FILE_POLICY_ANALYSIS policy_analysis_inout
)
{

	// ensure we have a valid registry
	if(!fs_reg)
		return BP_FALSE;
	if(!fs_reg->hreg)
		return BP_FALSE;
	if(!fs_reg_pol)
		return BP_FALSE;
	if(!filename)
		return BP_FALSE;
	if(!file_stat)
		return BP_FALSE;
	if(!file_dirent)
		return BP_FALSE;
	if(!policy_analysis_inout)
		return BP_FALSE;

	// enter critical section before creating new record
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);


	// file_stat,
	// file_lstat,
	// file_dirent

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Generate Appropriate buffers %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// printf("\n [+]  Running Policy Check: ");
	// printf("\n    Got filename: %s", filename );
	// printf("\n    Got path:     %s", path     );
	// printf("\n    Got fullpath: %s", full_path);
	// printf("\n");



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Policy Checks to Generate Flag Matrix %%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// run linear checks
	BP_BOOL symlink_check_ok   =  BP_FSRegCheckFileAgainstCrawlSymlinkPolicy     (BP_FSREG_CHECK_FILE_FUNC_PARMS_PASSTHROUGH);
	BP_BOOL hidden_check_ok    =  BP_FSRegCheckFileAgainstCrawlHiddenPolicy      (BP_FSREG_CHECK_FILE_FUNC_PARMS_PASSTHROUGH);
	BP_BOOL fifo_check_ok      =  BP_FSRegCheckFileAgainstCrawlFifoPolicy        (BP_FSREG_CHECK_FILE_FUNC_PARMS_PASSTHROUGH);
	BP_BOOL directory_check_ok =  BP_FSRegCheckFileAgainstCrawlDirectoryPolicy   (BP_FSREG_CHECK_FILE_FUNC_PARMS_PASSTHROUGH);
	BP_BOOL file_check_ok      =  BP_FSRegCheckFileAgainstCrawlFilePolicy        (BP_FSREG_CHECK_FILE_FUNC_PARMS_PASSTHROUGH);
	BP_BOOL chr_check_ok       =  BP_FSRegCheckFileAgainstCrawlChrDevicePolicy   (BP_FSREG_CHECK_FILE_FUNC_PARMS_PASSTHROUGH);
	BP_BOOL blk_check_ok       =  BP_FSRegCheckFileAgainstCrawlBlockDevicePolicy (BP_FSREG_CHECK_FILE_FUNC_PARMS_PASSTHROUGH);
	BP_BOOL sock_check_ok      =  BP_FSRegCheckFileAgainstCrawlSocketPolicy      (BP_FSREG_CHECK_FILE_FUNC_PARMS_PASSTHROUGH);
	BP_BOOL preg_check_ok      =  BP_FSRegCheckFileAgainstCrawlFilenamePregPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS_PASSTHROUGH);


	// --- Begin Cascading Exclusionary Checks ----------------

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Enforcement Checks %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	BP_BOOL symlink_enforce_ok   = BP_FSRegEnforceFileAgainstCrawlSymlinkPolicy     (BP_FSREG_CHECK_FILE_FUNC_PARMS_PASSTHROUGH);
	BP_BOOL hidden_enforce_ok    = BP_FSRegEnforceFileAgainstCrawlHiddenPolicy      (BP_FSREG_CHECK_FILE_FUNC_PARMS_PASSTHROUGH);
	BP_BOOL fifo_enforce_ok      = BP_FSRegEnforceFileAgainstCrawlFifoPolicy        (BP_FSREG_CHECK_FILE_FUNC_PARMS_PASSTHROUGH);
	BP_BOOL directory_enforce_ok = BP_FSRegEnforceFileAgainstCrawlDirectoryPolicy   (BP_FSREG_CHECK_FILE_FUNC_PARMS_PASSTHROUGH);
	BP_BOOL file_enforce_ok      = BP_FSRegEnforceFileAgainstCrawlFilePolicy        (BP_FSREG_CHECK_FILE_FUNC_PARMS_PASSTHROUGH);
	BP_BOOL chr_enforce_ok       = BP_FSRegEnforceFileAgainstCrawlChrDevicePolicy   (BP_FSREG_CHECK_FILE_FUNC_PARMS_PASSTHROUGH);
	BP_BOOL blk_enforce_ok       = BP_FSRegEnforceFileAgainstCrawlBlockDevicePolicy (BP_FSREG_CHECK_FILE_FUNC_PARMS_PASSTHROUGH);
	BP_BOOL sock_enforce_ok      = BP_FSRegEnforceFileAgainstCrawlSocketPolicy      (BP_FSREG_CHECK_FILE_FUNC_PARMS_PASSTHROUGH);
	BP_BOOL preg_enforce_ok      = BP_FSRegEnforceFileAgainstCrawlFilenamePregPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS_PASSTHROUGH);

	// final exit checks after policy enforcement checks
	// run.
	if
	(
		!symlink_enforce_ok   ||
		!hidden_enforce_ok    ||
		!fifo_enforce_ok      ||
		!directory_enforce_ok ||
		!file_enforce_ok      ||
		!chr_enforce_ok       ||
		!blk_enforce_ok       ||
		!sock_enforce_ok      ||
		!preg_enforce_ok
	)
	{

		// printf("\n One or more policy validations failed?!");

		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, BP_FALSE);
	}

	// exit critical section and return
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating false
	return BP_TRUE;

}

// Check a file against the symlink policy.
BP_BOOL BP_FSRegCheckFileAgainstCrawlSymlinkPolicy
(
		BP_FSREG_CHECK_FILE_FUNC_PARMS
)
{


	// run initial null parameter checks
	BP_FSREG_CHECK_POLICY_FUNC_ENTRY_RUN_NULL_CHECKS(BP_FALSE);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check if Symlink %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// type must be link data directly from opendir/readdir
	if(file_dirent->d_type == DT_LNK)
		policy_analysis_inout->is_symlink = BP_TRUE;


	// enter critical section before creating new record
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Individual Flag Checks %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// BP_FS_SYMLINK_POLICY_FLAG_IGNORE_ALL         = 1,
	// BP_FS_SYMLINK_POLICY_FLAG_ONLY               = 2,
	// BP_FS_SYMLINK_POLICY_FLAG_FOLLOW_DIRECTORIES = 3

	// run ignore symlink check
	if(BP_FLAG_IS_SET(fs_reg_pol->symlink_policy, BP_FS_SYMLINK_POLICY_FLAG_IGNORE_ALL) && policy_analysis_inout->is_symlink)
	{

		// set ignore all match
		policy_analysis_inout->symlink_ignore_all_match = BP_TRUE;

	}

	// only check
	if(BP_FLAG_IS_SET(fs_reg_pol->symlink_policy, BP_FS_SYMLINK_POLICY_FLAG_ONLY) && policy_analysis_inout->is_symlink)
	{

		// set ignore all match
		policy_analysis_inout->symlink_only_match = BP_TRUE;

	}

	// run follow directories check
	if(BP_FLAG_IS_SET(fs_reg_pol->symlink_policy, BP_FS_SYMLINK_POLICY_FLAG_FOLLOW_DIRECTORIES) && policy_analysis_inout->is_symlink)
	{

		// if the file stat (NOT LSTAT) st_mode is a directory, set the match here
		if(BP_STAT_MODE_IS_IFDIR(file_stat->st_mode))
			policy_analysis_inout->symlink_follow_directories_match = BP_TRUE;

	}

	// if it's a symlink, we have to mark whatever the symlink points to
	if(policy_analysis_inout->is_symlink)
	{

		if(BP_STAT_MODE_IS_IFDIR(file_stat->st_mode))
			policy_analysis_inout->symlink_target_is_directory = BP_TRUE;

		if(BP_STAT_MODE_IS_IREG(file_stat->st_mode))
			policy_analysis_inout->symlink_target_is_file = BP_TRUE;

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Unlock and Exit %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// unlock and exit
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating true (all checks passed ok)
	return BP_TRUE;

}

// Check a file against hidden policy.
BP_BOOL BP_FSRegCheckFileAgainstCrawlHiddenPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS)
{

	// run initial null parameter checks
	BP_FSREG_CHECK_POLICY_FUNC_ENTRY_RUN_NULL_CHECKS(BP_FALSE);

	// enter critical section before creating new record
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check if Hidden %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// We need to determine if the file is a hidden file, so run initial check.
	// if the file starts with a dot, it's a hidden file
	BP_BOOL is_hidden = (filename[0] == '.') ? BP_TRUE : BP_FALSE;

	// set hidden flag in policy
	policy_analysis_inout->is_hidden = is_hidden;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Individual Flag Checks %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// BP_FS_HIDDEN_POLICY_FLAG_IGNORE_ALL
	// BP_FS_HIDDEN_POLICY_FLAG_ONLY
	// BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_HIDDEN_DIRECTORIES
	// BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_HIDDEN_FILES
	// BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_ONLY_HIDDEN_DIRECTORIES
	// BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_ONLY_HIDDEN_FILES


	if(BP_FLAG_IS_SET(fs_reg_pol->hidden_policy, BP_FS_HIDDEN_POLICY_FLAG_ONLY) && is_hidden)
	{

		policy_analysis_inout->hidden_only_match = BP_TRUE;

	}

	if(BP_FLAG_IS_SET(fs_reg_pol->hidden_policy, BP_FS_HIDDEN_POLICY_FLAG_IGNORE_ALL) && is_hidden)
	{

		policy_analysis_inout->hidden_ignore_all_match = BP_TRUE;

	}

	// -- basic examine checks ----

	if(BP_FLAG_IS_SET(fs_reg_pol->hidden_policy, BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_HIDDEN_DIRECTORIES) && is_hidden)
	{

		policy_analysis_inout->hidden_examine_directories_match = BP_TRUE;

	}

	if(BP_FLAG_IS_SET(fs_reg_pol->hidden_policy, BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_HIDDEN_FILES) && is_hidden)
	{

		policy_analysis_inout->hidden_examine_files_match = BP_TRUE;

	}

	// -- only examine checks ---

	if(BP_FLAG_IS_SET(fs_reg_pol->hidden_policy, BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_ONLY_HIDDEN_DIRECTORIES) && is_hidden)
	{

		policy_analysis_inout->hidden_examine_only_directories_match = BP_TRUE;

	}

	if(BP_FLAG_IS_SET(fs_reg_pol->hidden_policy, BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_ONLY_HIDDEN_FILES) && is_hidden)
	{

		policy_analysis_inout->hidden_examine_only_files_match = BP_TRUE;

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Unlock and Exit %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// unlock and exit
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating true (all checks passed ok)
	return BP_TRUE;

}




BP_BOOL BP_FSRegCheckFileAgainstCrawlFifoPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS)
{

	// run initial null parameter checks
	BP_FSREG_CHECK_POLICY_FUNC_ENTRY_RUN_NULL_CHECKS(BP_FALSE);

	// enter critical section before creating new record
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check if Fifo %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// check if we have a fifo
	if(file_dirent->d_type == DT_FIFO)
		policy_analysis_inout->is_fifo = BP_TRUE;





	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Individual Flag Checks %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// BP_FS_FIFO_POLICY_FLAG_IGNORE_ALL
	// BP_FS_FIFO_POLICY_FLAG_ONLY

	// ignore check
	if(BP_FLAG_IS_SET(fs_reg_pol->fifo_policy, BP_FS_FIFO_POLICY_FLAG_IGNORE_ALL) && policy_analysis_inout->is_fifo)
	{

		// set ignore all match
		policy_analysis_inout->fifo_ignore_all_match = BP_TRUE;

	}

	// only check
	if(BP_FLAG_IS_SET(fs_reg_pol->fifo_policy, BP_FS_FIFO_POLICY_FLAG_ONLY) && policy_analysis_inout->is_fifo)
	{

		// set follow directories match
		policy_analysis_inout->fifo_only_match = BP_TRUE;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Unlock and Exit %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// unlock and exit
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating true (all checks passed ok)
	return BP_TRUE;

}


// directory policy checker
BP_BOOL BP_FSRegCheckFileAgainstCrawlDirectoryPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS)
{

	// run initial null parameter checks
	BP_FSREG_CHECK_POLICY_FUNC_ENTRY_RUN_NULL_CHECKS(BP_FALSE);

	// enter critical section before creating new record
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check if Directory %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// check if we have a fifo
	if(file_dirent->d_type == DT_DIR)
		policy_analysis_inout->is_directory = BP_TRUE;





	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Individual Flag Checks %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// BP_FS_DIRECTORY_POLICY_FLAG_IGNORE_ALL
	// BP_FS_DIRECTORY_POLICY_FLAG_ONLY
	// BP_FS_DIRECTORY_POLICY_FLAG_READABLE_BY_CURRENT_USER
	// BP_FS_DIRECTORY_POLICY_FLAG_WRITABLE_BY_CURRENT_USER
	// BP_FS_DIRECTORY_POLICY_FLAG_EXECUTABLE_BY_CURRENT_USER
	// BP_FS_DIRECTORY_POLICY_FLAG_SUID
	// BP_FS_DIRECTORY_POLICY_FLAG_SGID

	// ignore check
	if(BP_FLAG_IS_SET(fs_reg_pol->directory_policy, BP_FS_DIRECTORY_POLICY_FLAG_IGNORE_ALL) && policy_analysis_inout->is_directory)
	{


		// set ignore all match
		policy_analysis_inout->directory_ignore_all_match = BP_TRUE;

	}

	// only check
	if(BP_FLAG_IS_SET(fs_reg_pol->directory_policy, BP_FS_DIRECTORY_POLICY_FLAG_ONLY) && policy_analysis_inout->is_directory)
	{

		// set follow directories match
		policy_analysis_inout->directory_only_match = BP_TRUE;

	}


	// -- rwx checks ----------------

	if(BP_FLAG_IS_SET(fs_reg_pol->directory_policy, BP_FS_DIRECTORY_POLICY_FLAG_READABLE_BY_CURRENT_USER) && policy_analysis_inout->is_directory)
	{

		if(BP_STAT_MODE_IS_IRUSR(file_stat->st_mode))
			policy_analysis_inout->directory_readable_by_current_user_match = BP_TRUE;

	}

	if(BP_FLAG_IS_SET(fs_reg_pol->directory_policy, BP_FS_DIRECTORY_POLICY_FLAG_WRITABLE_BY_CURRENT_USER) && policy_analysis_inout->is_directory)
	{

		policy_analysis_inout->directory_writable_by_current_user_match = BP_TRUE;

	}

	if(BP_FLAG_IS_SET(fs_reg_pol->directory_policy, BP_FS_DIRECTORY_POLICY_FLAG_EXECUTABLE_BY_CURRENT_USER) && policy_analysis_inout->is_directory)
	{

		policy_analysis_inout->directory_executable_by_current_user_match = BP_TRUE;

	}


	// --- suid / sgid checks -----------

	if(BP_FLAG_IS_SET(fs_reg_pol->directory_policy, BP_FS_DIRECTORY_POLICY_FLAG_SUID) && policy_analysis_inout->is_directory)
	{

		policy_analysis_inout->directory_suid_bit_match = BP_TRUE;

	}

	if(BP_FLAG_IS_SET(fs_reg_pol->directory_policy, BP_FS_DIRECTORY_POLICY_FLAG_SGID) && policy_analysis_inout->is_directory)
	{

		policy_analysis_inout->directory_sgid_bit_match = BP_TRUE;

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Unlock and Exit %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// unlock and exit
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating true (all checks passed ok)
	return BP_TRUE;

}

// file policy checker
BP_BOOL BP_FSRegCheckFileAgainstCrawlFilePolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS)
{

	// run initial null parameter checks
	BP_FSREG_CHECK_POLICY_FUNC_ENTRY_RUN_NULL_CHECKS(BP_FALSE);

	// enter critical section before creating new record
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check if File %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// check if we have a fifo
	if(file_dirent->d_type == DT_REG)
		policy_analysis_inout->is_file = BP_TRUE;


	// get the current user/group id
	uid_t curr_uid = getuid();
	gid_t curr_gid = getgid();


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Individual Flag Checks %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// BP_FS_FILE_POLICY_FLAG_IGNORE_ALL
	// BP_FS_FILE_POLICY_FLAG_ONLY
	// BP_FS_FILE_POLICY_FLAG_READABLE_BY_CURRENT_USER
	// BP_FS_FILE_POLICY_FLAG_WRITABLE_BY_CURRENT_USER
	// BP_FS_FILE_POLICY_FLAG_EXECUTABLE_BY_CURRENT_USER
	// BP_FS_FILE_POLICY_FLAG_SUID
	// BP_FS_FILE_POLICY_FLAG_SGID

	// IGNORE_ALL
	if(BP_FLAG_IS_SET(fs_reg_pol->file_policy, BP_FS_FILE_POLICY_FLAG_IGNORE_ALL) && policy_analysis_inout->is_file)
	{

		// set ignore all match
		policy_analysis_inout->file_ignore_all_match = BP_TRUE;

	}

	// ONLY
	if(BP_FLAG_IS_SET(fs_reg_pol->file_policy, BP_FS_FILE_POLICY_FLAG_ONLY) && policy_analysis_inout->is_file)
	{

		// set follow directories match
		policy_analysis_inout->file_only_match = BP_TRUE;

	}

	// OWNED_BY_CURRENT_USER
	if(BP_FLAG_IS_SET(fs_reg_pol->file_policy, BP_FS_FILE_POLICY_FLAG_OWNED_BY_CURRENT_USER) && policy_analysis_inout->is_file)
	{

		// if the file is owned by the current user, we can match things
		if(file_lstat->st_uid == curr_uid)
		{

			policy_analysis_inout->file_owned_by_current_user = BP_TRUE;

		}

	}

	// NOT_OWNED_BY_CURRENT_USER
	if(BP_FLAG_IS_SET(fs_reg_pol->file_policy, BP_FS_FILE_POLICY_FLAG_NOT_OWNED_BY_CURRENT_USER) && policy_analysis_inout->is_file)
	{

		// if the file is owned by the current user, we can match things
		if(file_lstat->st_uid != curr_uid && file_stat->st_uid != curr_uid )
		{

			policy_analysis_inout->file_not_owned_by_current_user = BP_TRUE;

		}

	}

	// OWNER_HAS_HIGHER_UID_THAN_CURRENT
	if(BP_FLAG_IS_SET(fs_reg_pol->file_policy, BP_FS_FILE_POLICY_FLAG_OWNER_HAS_HIGHER_UID_THAN_CURRENT) && policy_analysis_inout->is_file)
	{


		if(file_lstat->st_uid < curr_uid)
		{
			policy_analysis_inout->file_owner_has_higher_uid_than_current = BP_TRUE;
		}

	}

	// OWNER_HAS_LOWER_UID_THAN_CURRENT
	if(BP_FLAG_IS_SET(fs_reg_pol->file_policy, BP_FS_FILE_POLICY_FLAG_OWNER_HAS_LOWER_UID_THAN_CURRENT) && policy_analysis_inout->is_file)
	{

		if(file_lstat->st_uid > curr_uid)
		{

			policy_analysis_inout->file_owner_has_lower_uid_than_current = BP_TRUE;

		}

	}

	// READABLE_BY_CURRENT_USER
	if(BP_FLAG_IS_SET(fs_reg_pol->file_policy, BP_FS_FILE_POLICY_FLAG_READABLE_BY_CURRENT_USER) && policy_analysis_inout->is_file)
	{

		if(file_lstat->st_uid == curr_uid)
		{
			policy_analysis_inout->file_readable_by_current_user_match = BP_TRUE;
			policy_analysis_inout->file_readable_by_current_user_because_user_is_owner_match = BP_TRUE;
		}
		else
		{

			if(curr_gid == file_lstat->st_gid)
			{
				if(BP_STAT_MODE_IS_IRGRP(file_lstat->st_mode))
				{
					policy_analysis_inout->file_readable_by_current_user_match               = BP_TRUE;
					policy_analysis_inout->file_readable_by_current_user_because_group_match = BP_TRUE;
				}
			}

			if(BP_STAT_MODE_IS_IROTH(file_lstat->st_mode))
			{
				policy_analysis_inout->file_readable_by_current_user_match               = BP_TRUE;
				policy_analysis_inout->file_readable_by_current_user_because_world_match = BP_TRUE;
			}

		}



		if(curr_uid == 0)
		{
			policy_analysis_inout->file_readable_by_current_user_match                      = BP_TRUE;
			policy_analysis_inout->file_readable_by_current_user_because_user_is_root_match = BP_TRUE;
		}

		if(file_lstat->st_uid == curr_uid)
		{
			policy_analysis_inout->file_readable_by_current_user_match                       = BP_TRUE;
			policy_analysis_inout->file_readable_by_current_user_because_user_is_owner_match = BP_TRUE;
		}



	}


	// WRITABLE_BY_CURRENT_USER
	if(BP_FLAG_IS_SET(fs_reg_pol->file_policy, BP_FS_FILE_POLICY_FLAG_WRITABLE_BY_CURRENT_USER) && policy_analysis_inout->is_file)
	{

		if(file_lstat->st_uid == curr_uid)
		{
			policy_analysis_inout->file_writable_by_current_user_match = BP_TRUE;
			policy_analysis_inout->file_writable_by_current_user_because_user_is_owner_match = BP_TRUE;
		}
		else
		{

			if(curr_gid == file_lstat->st_gid)
			{
				if(BP_STAT_MODE_IS_IWGRP(file_lstat->st_mode))
				{
					policy_analysis_inout->file_writable_by_current_user_match               = BP_TRUE;
					policy_analysis_inout->file_writable_by_current_user_because_group_match = BP_TRUE;
				}
			}

			if(BP_STAT_MODE_IS_IWOTH(file_lstat->st_mode))
			{
				policy_analysis_inout->file_writable_by_current_user_match               = BP_TRUE;
				policy_analysis_inout->file_writable_by_current_user_because_world_match = BP_TRUE;
			}

		}



		if(curr_uid == 0)
		{
			policy_analysis_inout->file_writable_by_current_user_match                      = BP_TRUE;
			policy_analysis_inout->file_writable_by_current_user_because_user_is_root_match = BP_TRUE;
		}

		if(file_lstat->st_uid == curr_uid)
		{
			policy_analysis_inout->file_writable_by_current_user_match                       = BP_TRUE;
			policy_analysis_inout->file_writable_by_current_user_because_user_is_owner_match = BP_TRUE;
		}

	}

	// EXECUTABLE_BY_CURRENT_USER
	if(BP_FLAG_IS_SET(fs_reg_pol->file_policy, BP_FS_FILE_POLICY_FLAG_EXECUTABLE_BY_CURRENT_USER) && policy_analysis_inout->is_file)
	{

		if(file_lstat->st_uid == curr_uid)
		{
			policy_analysis_inout->file_executable_by_current_user_match                       = BP_TRUE;
			policy_analysis_inout->file_executable_by_current_user_because_user_is_owner_match = BP_TRUE;
		}
		else
		{

			if(curr_gid == file_lstat->st_gid)
			{
				if(BP_STAT_MODE_IS_IXGRP(file_lstat->st_mode))
				{
					policy_analysis_inout->file_executable_by_current_user_match               = BP_TRUE;
					policy_analysis_inout->file_executable_by_current_user_because_group_match = BP_TRUE;
				}
			}

			if(BP_STAT_MODE_IS_IXOTH(file_lstat->st_mode))
			{
				policy_analysis_inout->file_executable_by_current_user_match               = BP_TRUE;
				policy_analysis_inout->file_executable_by_current_user_because_world_match = BP_TRUE;
			}

		}



		if(curr_uid == 0)
		{
			policy_analysis_inout->file_executable_by_current_user_match                      = BP_TRUE;
			policy_analysis_inout->file_executable_by_current_user_because_user_is_root_match = BP_TRUE;
		}

		if(file_lstat->st_uid == curr_uid)
		{
			policy_analysis_inout->file_executable_by_current_user_match                       = BP_TRUE;
			policy_analysis_inout->file_executable_by_current_user_because_user_is_owner_match = BP_TRUE;
		}

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Unlock and Exit %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// unlock and exit
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating true (all checks passed ok)
	return BP_TRUE;
}

BP_BOOL BP_FSRegCheckFileAgainstCrawlChrDevicePolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS)
{

	// run initial null parameter checks
	BP_FSREG_CHECK_POLICY_FUNC_ENTRY_RUN_NULL_CHECKS(BP_FALSE);

	// enter critical section before creating new record
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check if Character Device %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// check file type
	if(file_dirent->d_type == DT_CHR)
		policy_analysis_inout->is_chr = BP_TRUE;





	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Individual Flag Checks %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// BP_FS_CHR_POLICY_FLAG_IGNORE_ALL
	// BP_FS_CHR_POLICY_FLAG_ONLY

	// ignore check
	if(BP_FLAG_IS_SET(fs_reg_pol->chr_policy, BP_FS_CHR_POLICY_FLAG_IGNORE_ALL) && policy_analysis_inout->is_chr)
	{

		// set ignore all match
		policy_analysis_inout->chr_ignore_all_match = BP_TRUE;

	}

	// only check
	if(BP_FLAG_IS_SET(fs_reg_pol->chr_policy, BP_FS_CHR_POLICY_FLAG_ONLY) && policy_analysis_inout->is_chr)
	{

		// set follow directories match
		policy_analysis_inout->chr_only_match = BP_TRUE;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Unlock and Exit %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// unlock and exit
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating true (all checks passed ok)
	return BP_TRUE;

}

BP_BOOL BP_FSRegCheckFileAgainstCrawlBlockDevicePolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS)
{
	// run initial null parameter checks
	BP_FSREG_CHECK_POLICY_FUNC_ENTRY_RUN_NULL_CHECKS(BP_FALSE);

	// enter critical section before creating new record
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check if Block Device %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// check file type
	if(file_dirent->d_type == DT_BLK)
		policy_analysis_inout->is_blk = BP_TRUE;





	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Individual Flag Checks %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// BP_FS_BLK_POLICY_FLAG_IGNORE_ALL
	// BP_FS_BLK_POLICY_FLAG_ONLY

	// ignore check
	if(BP_FLAG_IS_SET(fs_reg_pol->chr_policy, BP_FS_BLK_POLICY_FLAG_IGNORE_ALL) && policy_analysis_inout->is_blk)
	{

		// set ignore all match
		policy_analysis_inout->blk_ignore_all_match = BP_TRUE;

	}

	// only check
	if(BP_FLAG_IS_SET(fs_reg_pol->chr_policy, BP_FS_BLK_POLICY_FLAG_ONLY) && policy_analysis_inout->is_blk)
	{

		// set follow directories match
		policy_analysis_inout->blk_only_match = BP_TRUE;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Unlock and Exit %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// unlock and exit
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating true (all checks passed ok)
	return BP_TRUE;

}

// Check a file against socket policy
BP_BOOL BP_FSRegCheckFileAgainstCrawlSocketPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS)
{

	// run initial null parameter checks
	BP_FSREG_CHECK_POLICY_FUNC_ENTRY_RUN_NULL_CHECKS(BP_FALSE);

	// enter critical section before creating new record
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check if Sock Device %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// check file type
	if(file_dirent->d_type == DT_SOCK)
		policy_analysis_inout->is_sock = BP_TRUE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Individual Flag Checks %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// BP_FS_SOCK_POLICY_FLAG_IGNORE_ALL
	// BP_FS_SOCK_POLICY_FLAG_ONLY

	// ignore check
	if(BP_FLAG_IS_SET(fs_reg_pol->sock_policy, BP_FS_SOCK_POLICY_FLAG_IGNORE_ALL) && policy_analysis_inout->is_sock)
	{

		// set ignore all match
		policy_analysis_inout->sock_ignore_all_match = BP_TRUE;

	}

	// only check
	if(BP_FLAG_IS_SET(fs_reg_pol->sock_policy, BP_FS_SOCK_POLICY_FLAG_ONLY) && policy_analysis_inout->is_sock)
	{

		// set follow directories match
		policy_analysis_inout->sock_only_match = BP_TRUE;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Unlock and Exit %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// unlock and exit
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating true (all checks passed ok)
	return BP_TRUE;
}


// filename perl compatible regex checker
BP_BOOL BP_FSRegCheckFileAgainstCrawlFilenamePregPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS)
{

	// run initial null parameter checks
	BP_FSREG_CHECK_POLICY_FUNC_ENTRY_RUN_NULL_CHECKS(BP_FALSE);

	// enter critical section before creating new record
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// check string is reasonable/printable before starting
	if(!BP_StringIsReasonablePrintableString(filename, bpstrlen(filename), BP_FALSE, BP_FALSE))
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, BP_FALSE);

	// matched
	size_t matched         = 0;
	size_t max_match_count = 10;

	// create new match info structure and zero it
	BP_REGEX_MATCH_INFO match_info;
	BP_ZERO_STACK(match_info);

	// set flags (no flags to start)
	int regexp_flags = PCRE_MULTILINE;



	// --- Run Fname Checks if Any

	// run preg checks
	size_t n = 0;
	for
	(
		;
		n < fs_reg_pol->preg_fname_policies_n;
		n++
	)
	{

		// ensure there is a file length
		if(!bpstrlen(filename))
			continue;

		// printf("\n Running fname policy check: %u of %u - %s", n, fs_reg_pol->preg_fname_policies_n, fs_reg_pol->preg_fname_policies[n]);

		// zero the match information
		BP_ZERO_STACK(match_info);

		// attempt to match regexp (don't save info)
		matched = BP_MatchRegularExpressionPCRE
		(
			filename,
			bpstrlen(filename),
			fs_reg_pol->preg_fname_policies[n],
			regexp_flags,
			max_match_count,
			&match_info,
			false
		);

		// optional debug print
		/*
		if(matched)
		 	printf("\n Fname Regexp Matched?: %s - %s",fs_reg_pol->preg_fname_policies[n], filename );
		 else
		 	printf("\n Match failed?: %s - %s",fs_reg_pol->preg_fname_policies[n], filename );
		*/

		// if we have a match, set the flag
		if(matched)
			policy_analysis_inout->fname_preg_policies_match = BP_TRUE;
		else
			policy_analysis_inout->fname_preg_policies_match = BP_FALSE;

		// reset on loop
		matched = 0;

	}


	// --- Run Full Path Checks if Any

	// run preg checks
	for
	(
		n = 0;
		n < fs_reg_pol->preg_fullpath_matching_policies_n;
		n++
	)
	{

		// ensure there is a file length
		if(!bpstrlen(filename))
			continue;

		// printf("\n Running fname policy check: %u of %u - %s", n, fs_reg_pol->preg_fname_policies_n, fs_reg_pol->preg_fname_policies[n]);

		// zero the match information
		BP_ZERO_STACK(match_info);

		// attempt to match regexp (don't save info)
		matched = BP_MatchRegularExpressionPCRE
		(
			filename,
			bpstrlen(filename),
			fs_reg_pol->preg_fullpath_matching_policies[n],
			regexp_flags,
			max_match_count,
			&match_info,
			false
		);

		// if(matched)
		// 	printf("\n Full Path Regexp Matched?: %s - %s",fs_reg_pol->preg_fullpath_matching_policies[n], full_path );
		// else
		// 	printf("\n Full Path Failed?: %s - %s",fs_reg_pol->preg_fullpath_matching_policies[n], full_path );

		// if we have a match, set the flag
		if(matched)
			policy_analysis_inout->fullpath_preg_policies_match = BP_TRUE;
		else
			policy_analysis_inout->fullpath_preg_policies_match = BP_FALSE;

		// reset on loop
		matched = 0;

	}


	// --- Run Path Only Checks if Any

	// run preg checks
	for
	(
		n = 0;
		n < fs_reg_pol->preg_pathonly_matching_policies_n;
		n++
	)
	{

		// ensure there is a file length
		if(!bpstrlen(filename))
			continue;

		// printf("\n Running fname policy check: %u of %u - %s", n, fs_reg_pol->preg_fname_policies_n, fs_reg_pol->preg_fname_policies[n]);

		// zero the match information
		BP_ZERO_STACK(match_info);

		// attempt to match regexp (don't save info)
		matched = BP_MatchRegularExpressionPCRE
		(
			filename,
			bpstrlen(filename),
			fs_reg_pol->preg_pathonly_matching_policies[n],
			regexp_flags,
			max_match_count,
			&match_info,
			false
		);


		// if(matched)
		// 	printf("\n Path Regexp Matched?: %s - %s",fs_reg_pol->preg_pathonly_matching_policies[n], path );
		// else
		// 	printf("\n Path Failed?: %s - %s",fs_reg_pol->preg_pathonly_matching_policies[n], path );

		// if we have a match, set the flag
		if(matched)
			policy_analysis_inout->pathonly_preg_policies_match = BP_TRUE;
		else
			policy_analysis_inout->pathonly_preg_policies_match = BP_FALSE;

		// reset on loop
		matched = 0;

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Unlock and Exit %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// unlock and exit
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating true (all checks passed ok)
	return BP_TRUE;

}




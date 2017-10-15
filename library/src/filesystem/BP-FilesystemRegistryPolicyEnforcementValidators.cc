/*
 * BP-FilesystemRegistryPolicyEnforcementValidators.cc
 *
 *  Created on: Jul 27, 2015
 *      Author: root
 */


// main include file
#include "../../include/BP-Main.h"

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Policy Matching: Enforcement %%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Individual policy check enforcers (these check policies 1 to 1 with the flags returned
// returned from any of the above check functions (typically they're all called successively)

// symlink policy enforcer
BP_BOOL BP_FSRegEnforceFileAgainstCrawlSymlinkPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS)
{

	// run initial null parameter checks
	BP_FSREG_CHECK_POLICY_FUNC_ENTRY_RUN_NULL_CHECKS(BP_FALSE);

	// enter critical section before creating new record
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// default exit if no policy set
	if(!fs_reg->crawl_policies.symlink_policy)
		return BP_TRUE;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Common Only/Ignore Checks %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// run all basic exclusionary checks
	BP_FSREG_POLICY_RUN_ONLY_CHECK  (symlink,   BP_FS_SYMLINK_POLICY_FLAG_ONLY);
	BP_FSREG_POLICY_RUN_IGNORE_CHECK(symlink);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Specific Checks %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// BP_FS_SYMLINK_POLICY_FLAG_IGNORE_ALL         = 1,
	// BP_FS_SYMLINK_POLICY_FLAG_ONLY               = 2,
	// BP_FS_SYMLINK_POLICY_FLAG_FOLLOW_DIRECTORIES = 3



	// unlock and exit
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating a successful match
	return BP_TRUE;

}

// hidden policy enforcer
BP_BOOL BP_FSRegEnforceFileAgainstCrawlHiddenPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS)
{

	// run initial null parameter checks
	BP_FSREG_CHECK_POLICY_FUNC_ENTRY_RUN_NULL_CHECKS(BP_FALSE);

	// enter critical section before creating new record
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// default exit if no policy set
	if(!fs_reg->crawl_policies.hidden_policy)
		return BP_TRUE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Common Only/Ignore Checks %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// BP_FS_HIDDEN_POLICY_FLAG_IGNORE_ALL                      = 1,
	// BP_FS_HIDDEN_POLICY_FLAG_ONLY                            = 2,
	// BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_HIDDEN_DIRECTORIES      = 3,
	// BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_HIDDEN_FILES            = 4,
	// BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_ONLY_HIDDEN_DIRECTORIES = 5,
	// BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_ONLY_HIDDEN_FILES       = 6,

	// run all basic exclusionary checks
	BP_FSREG_POLICY_RUN_ONLY_CHECK  (hidden,   BP_FS_HIDDEN_POLICY_FLAG_ONLY);
	BP_FSREG_POLICY_RUN_IGNORE_CHECK(hidden);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Specific Checks %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// unlock and exit
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating a successful match
	return BP_TRUE;

}

// fifo policy enforcer
BP_BOOL BP_FSRegEnforceFileAgainstCrawlFifoPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS)
{

	// run initial null parameter checks
	BP_FSREG_CHECK_POLICY_FUNC_ENTRY_RUN_NULL_CHECKS(BP_FALSE);

	// enter critical section before creating new record
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// default exit if no policy set
	if(!fs_reg->crawl_policies.fifo_policy)
		return BP_TRUE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Common Only/Ignore Checks %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// BP_FS_FIFO_POLICY_FLAG_IGNORE_ALL         = 1,
	// BP_FS_FIFO_POLICY_FLAG_ONLY               = 2

	// run all basic exclusionary checks
	BP_FSREG_POLICY_RUN_ONLY_CHECK  (fifo,   BP_FS_FIFO_POLICY_FLAG_ONLY);
	BP_FSREG_POLICY_RUN_IGNORE_CHECK(fifo);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Specific Checks %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// unlock and exit
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating a successful match
	return BP_TRUE;

}

// directory policy enforcer
BP_BOOL BP_FSRegEnforceFileAgainstCrawlDirectoryPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS)
{

	// run initial null parameter checks
	BP_FSREG_CHECK_POLICY_FUNC_ENTRY_RUN_NULL_CHECKS(BP_FALSE);

	// enter critical section before creating new record
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// default exit if no policy set
	if(!fs_reg->crawl_policies.directory_policy)
		return BP_TRUE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Common Only/Ignore Checks %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// BP_FS_DIRECTORY_POLICY_FLAG_IGNORE_ALL                 = 1,
	// BP_FS_DIRECTORY_POLICY_FLAG_ONLY                       = 2,
	// BP_FS_DIRECTORY_POLICY_FLAG_READABLE_BY_CURRENT_USER   = 3,
	// BP_FS_DIRECTORY_POLICY_FLAG_WRITABLE_BY_CURRENT_USER   = 4,
	// BP_FS_DIRECTORY_POLICY_FLAG_EXECUTABLE_BY_CURRENT_USER = 5,
	// BP_FS_DIRECTORY_POLICY_FLAG_SUID                       = 6,
	// BP_FS_DIRECTORY_POLICY_FLAG_SGID                       = 7

	// run all basic exclusionary checks
	BP_FSREG_POLICY_RUN_ONLY_CHECK  (directory,   BP_FS_DIRECTORY_POLICY_FLAG_ONLY);
	BP_FSREG_POLICY_RUN_IGNORE_CHECK(directory);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Specific Checks %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// unlock and exit
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating a successful match
	return BP_TRUE;

}

// file policy enforcer
BP_BOOL BP_FSRegEnforceFileAgainstCrawlFilePolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS)
{

	// run initial null parameter checks
	BP_FSREG_CHECK_POLICY_FUNC_ENTRY_RUN_NULL_CHECKS(BP_FALSE);

	// enter critical section before creating new record
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// default exit if no policy set
	if(!fs_reg->crawl_policies.file_policy)
		return BP_TRUE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Common Only/Ignore Checks %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// run all basic exclusionary checks
	BP_FSREG_POLICY_RUN_ONLY_CHECK  (file,   BP_FS_FILE_POLICY_FLAG_ONLY);
	BP_FSREG_POLICY_RUN_IGNORE_CHECK(file);

	// BP_FS_FILE_POLICY_FLAG_IGNORE_ALL                        = DONE,
	// BP_FS_FILE_POLICY_FLAG_ONLY                              = DONE,
	// BP_FS_FILE_POLICY_FLAG_NOT_OWNED_BY_CURRENT_USER         = DONE,
	// BP_FS_FILE_POLICY_FLAG_OWNED_BY_CURRENT_USER             = DONE,
	// BP_FS_FILE_POLICY_FLAG_READABLE_BY_CURRENT_USER          = DONE,
	// BP_FS_FILE_POLICY_FLAG_WRITABLE_BY_CURRENT_USER          = DONE,
	// BP_FS_FILE_POLICY_FLAG_EXECUTABLE_BY_CURRENT_USER        = DONE,
	// BP_FS_FILE_POLICY_FLAG_SUID                              = DONE,
	// BP_FS_FILE_POLICY_FLAG_SGID                              = DONE,
	// BP_FS_FILE_POLICY_FLAG_OWNER_HAS_HIGHER_UID_THAN_CURRENT = DONE,
	// BP_FS_FILE_POLICY_FLAG_OWNER_HAS_LOWER_UID_THAN_CURRENT  = DONE



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Specific Checks %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// BP_FS_FILE_POLICY_FLAG_NOT_OWNED_BY_CURRENT_USER
	if(BP_FLAG_IS_SET(fs_reg->crawl_policies.file_policy, BP_FS_FILE_POLICY_FLAG_NOT_OWNED_BY_CURRENT_USER))
	{
		if(!policy_analysis_inout->file_not_owned_by_current_user)
		{
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, BP_FALSE);
		}
	}


	// BP_FS_FILE_POLICY_FLAG_OWNED_BY_CURRENT_USER
	if(BP_FLAG_IS_SET(fs_reg->crawl_policies.file_policy, BP_FS_FILE_POLICY_FLAG_OWNED_BY_CURRENT_USER))
	{
		if(!policy_analysis_inout->file_owned_by_current_user)
		{
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, BP_FALSE);
		}
	}


	// BP_FS_FILE_POLICY_FLAG_READABLE_BY_CURRENT_USER
	if(BP_FLAG_IS_SET(fs_reg->crawl_policies.file_policy, BP_FS_FILE_POLICY_FLAG_READABLE_BY_CURRENT_USER))
	{
		if(!policy_analysis_inout->file_readable_by_current_user_match)
		{
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, BP_FALSE);
		}
	}

	// BP_FS_FILE_POLICY_FLAG_WRITABLE_BY_CURRENT_USER
	if(BP_FLAG_IS_SET(fs_reg->crawl_policies.file_policy, BP_FS_FILE_POLICY_FLAG_WRITABLE_BY_CURRENT_USER))
	{
		if(!policy_analysis_inout->file_writable_by_current_user_match)
		{
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, BP_FALSE);
		}
	}

	// BP_FS_FILE_POLICY_FLAG_EXECUTABLE_BY_CURRENT_USER
	if(BP_FLAG_IS_SET(fs_reg->crawl_policies.file_policy, BP_FS_FILE_POLICY_FLAG_EXECUTABLE_BY_CURRENT_USER))
	{
		if(!policy_analysis_inout->file_executable_by_current_user_match)
		{
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, BP_FALSE);
		}
	}


	// BP_FS_FILE_POLICY_FLAG_SUID
	if(BP_FLAG_IS_SET(fs_reg->crawl_policies.file_policy, BP_FS_FILE_POLICY_FLAG_SUID))
	{
		if(!policy_analysis_inout->file_suid_bit_match)
		{
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, BP_FALSE);
		}
	}

	// BP_FS_FILE_POLICY_FLAG_SGID
	if(BP_FLAG_IS_SET(fs_reg->crawl_policies.file_policy, BP_FS_FILE_POLICY_FLAG_SGID))
	{
		if(!policy_analysis_inout->file_sgid_bit_match)
		{
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, BP_FALSE);
		}
	}

	// BP_FS_FILE_POLICY_FLAG_OWNER_HAS_HIGHER_UID_THAN_CURRENT
	if(BP_FLAG_IS_SET(fs_reg->crawl_policies.file_policy, BP_FS_FILE_POLICY_FLAG_OWNER_HAS_HIGHER_UID_THAN_CURRENT))
	{
		if(!policy_analysis_inout->file_owner_has_higher_uid_than_current)
		{
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, BP_FALSE);
		}
	}


	// BP_FS_FILE_POLICY_FLAG_OWNER_HAS_LOWER_UID_THAN_CURRENT
	if(BP_FLAG_IS_SET(fs_reg->crawl_policies.file_policy, BP_FS_FILE_POLICY_FLAG_OWNER_HAS_LOWER_UID_THAN_CURRENT))
	{
		if(!policy_analysis_inout->file_owner_has_lower_uid_than_current)
		{
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, BP_FALSE);
		}
	}

	// unlock and exit
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating a successful match
	return BP_TRUE;

}

// chr device policy enforcer
BP_BOOL BP_FSRegEnforceFileAgainstCrawlChrDevicePolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS)
{

	// run initial null parameter checks
	BP_FSREG_CHECK_POLICY_FUNC_ENTRY_RUN_NULL_CHECKS(BP_FALSE);

	// enter critical section before creating new record
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// default exit if no policy set
	if(!fs_reg->crawl_policies.chr_policy)
		return BP_TRUE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Common Only/Ignore Checks %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// BP_FS_CHR_POLICY_FLAG_IGNORE_ALL                 = 1,
	// BP_FS_CHR_POLICY_FLAG_ONLY                       = 2

	// run all basic exclusionary checks
	BP_FSREG_POLICY_RUN_ONLY_CHECK  (chr,   BP_FS_CHR_POLICY_FLAG_ONLY);
	BP_FSREG_POLICY_RUN_IGNORE_CHECK(chr);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Specific Checks %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// unlock and exit
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating a successful match
	return BP_TRUE;

}

// block device policy enforcer
BP_BOOL BP_FSRegEnforceFileAgainstCrawlBlockDevicePolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS)
{

	// run initial null parameter checks
	BP_FSREG_CHECK_POLICY_FUNC_ENTRY_RUN_NULL_CHECKS(BP_FALSE);

	// enter critical section before creating new record
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// default exit if no policy set
	if(!fs_reg->crawl_policies.blk_policy)
		return BP_TRUE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Common Only/Ignore Checks %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// BP_FS_BLK_POLICY_FLAG_IGNORE_ALL                 = 1,
	// BP_FS_BLK_POLICY_FLAG_ONLY                       = 2

	// run all basic exclusionary checks
	BP_FSREG_POLICY_RUN_ONLY_CHECK  (blk,   BP_FS_BLK_POLICY_FLAG_ONLY);
	BP_FSREG_POLICY_RUN_IGNORE_CHECK(blk);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Specific Checks %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// unlock and exit
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating a successful match
	return BP_TRUE;

}

// socket policy enforcer
BP_BOOL BP_FSRegEnforceFileAgainstCrawlSocketPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS)
{


	// run initial null parameter checks
	BP_FSREG_CHECK_POLICY_FUNC_ENTRY_RUN_NULL_CHECKS(BP_FALSE);

	// enter critical section before creating new record
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// default exit if no policy set
	if(!fs_reg->crawl_policies.sock_policy)
		return BP_TRUE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Common Only/Ignore Checks %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// BP_FS_SOCK_POLICY_FLAG_IGNORE_ALL                 = 1,
	// BP_FS_SOCK_POLICY_FLAG_ONLY                       = 2

	// run all basic exclusionary checks
	BP_FSREG_POLICY_RUN_ONLY_CHECK  (sock,   BP_FS_SOCK_POLICY_FLAG_ONLY);
	BP_FSREG_POLICY_RUN_IGNORE_CHECK(sock);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Specific Checks %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// unlock and exit
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating a successful match
	return BP_TRUE;

}

// filename perl compatible regex enforcer
BP_BOOL BP_FSRegEnforceFileAgainstCrawlFilenamePregPolicy(BP_FSREG_CHECK_FILE_FUNC_PARMS)
{

	// run initial null parameter checks
	BP_FSREG_CHECK_POLICY_FUNC_ENTRY_RUN_NULL_CHECKS(BP_FALSE);

	// enter critical section before creating new record
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// default exit true if no policies are set (no restrictions returns ALL files)
	if
	(
		!fs_reg->crawl_policies.preg_fname_policies             &&
		!fs_reg->crawl_policies.preg_fullpath_matching_policies &&
		!fs_reg->crawl_policies.preg_pathonly_matching_policies
	)
	{
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, BP_TRUE);
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run PREG Checks %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure we match filename policies if one is set
	if(fs_reg->crawl_policies.preg_fname_policies)
	{

		// if fname policies did not match, exit failure here
		if(!policy_analysis_inout->fname_preg_policies_match)
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, BP_FALSE);
	}


	// ensure we match path only matching policies if set
	if(fs_reg->crawl_policies.preg_pathonly_matching_policies)
	{

		// if fname policies did not match, exit failure here
		if(!policy_analysis_inout->pathonly_preg_policies_match)
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, BP_FALSE);

	}


	// ensure we match full path policies only
	if(fs_reg->crawl_policies.preg_fullpath_matching_policies)
	{

		// if fname policies did not match, exit failure here
		if(!policy_analysis_inout->fullpath_preg_policies_match)
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, BP_FALSE);


	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Specific Checks %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// unlock and exit
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating a successful match
	return BP_TRUE;

}


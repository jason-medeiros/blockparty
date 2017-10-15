/*
 * BP-FilesystemRegistry.cc
 *
 *  Created on: Jul 20, 2015
 *      Author: root
 */

// A Filesystem Registry is a type of hash registry used for mapping
// a known filesystem into a blockparty native data type.  Hash table
// registries are very useful for tracking data, so tracking filesystem
// data shouldn't be too difficult.


// main include file
#include "../../include/BP-Main.h"

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Open / Close Filesystem Registries %%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Open a handle to the filesystem.
P_BP_FSREG BP_FSRegOpen
(
		char *  fs_dir,
		BP_BOOL recurse,
		size_t  max_recurse_depth
)
{

	// ensure we have a directory
	if(!fs_dir)
		return NULL;

	// ensure directory string is reasonable
	if(!BP_StringIsReasonablePrintableString(fs_dir, bpstrlen(fs_dir), BP_FALSE, BP_FALSE))
		return NULL;

	// create a new hash table registry
	P_BP_HASH_TABLE_REGISTRY new_reg = BP_CreateHashTableRegistry();
	if(!new_reg)
		return NULL;

	// attempt to create our filesystem registry from the newly created hash registry
	P_BP_FSREG fs_reg = (P_BP_FSREG) ht_malloc(new_reg, sizeof(BP_FSREG));
	if(!fs_reg)
	{
		BP_DestroyHashTableRegistry(new_reg);
		return NULL;
	}

	// set hreg (self referencing)
	fs_reg->hreg = new_reg;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Open Directory %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the top directory
	fs_reg->top_dir = ht_strdup(fs_reg->hreg, fs_dir);

	// return the filesystem registry
	return fs_reg;

}

// Closes a handle and destroys all associated memory.
BP_ERROR_T BP_FSRegCloseDestroy
(
		P_BP_FSREG fs_reg
)
{

	// ensure we have a filesystem registry
	if(!fs_reg)
		return ERR_FAILURE;
	if(!fs_reg->hreg)
		return ERR_FAILURE;


	// destroy the registry (registry destroys everything, including
	// fs_reg itself.
	return BP_DestroyHashTableRegistry(fs_reg->hreg);

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Flag Policy Settrs %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


BP_ERROR_T BP_FSRegSetSymlinkPolicy
(
	P_BP_FSREG fs_reg,
	BP_FSREG_SYMLINK_POLICY_FLAG flag,
	BP_BOOL setting
)
{

	// ensure we have a registry
	if(!fs_reg)
		return ERR_FAILURE;
	if(!fs_reg->hreg)
		return ERR_FAILURE;

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// set flag
	if(setting == BP_TRUE)
	{
		BP_SET_FLAG(fs_reg->crawl_policies.symlink_policy, flag);
	}
	else
	{
		BP_UNSET_FLAG(fs_reg->crawl_policies.symlink_policy, flag);
	}

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating success
	return ERR_SUCCESS;

}

BP_ERROR_T BP_FSRegSetHiddenPolicy
(
		P_BP_FSREG fs_reg,
		BP_FSREG_HIDDEN_POLICY_FLAG flag,
		BP_BOOL setting
)
{
	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// set flag
	if(setting == BP_TRUE)
	{
		BP_SET_FLAG(fs_reg->crawl_policies.hidden_policy, flag);
	}
	else
	{
		BP_UNSET_FLAG(fs_reg->crawl_policies.hidden_policy, flag);
	}

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating success
	return ERR_SUCCESS;

}

BP_ERROR_T BP_FSRegSetFifoPolicy
(
		P_BP_FSREG fs_reg,
		BP_FSREG_FIFO_POLICY_FLAG flag,
		BP_BOOL setting
)
{

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// set flag
	if(setting == BP_TRUE)
	{
		BP_SET_FLAG(fs_reg->crawl_policies.fifo_policy, flag);
	}
	else
	{
		BP_UNSET_FLAG(fs_reg->crawl_policies.fifo_policy, flag);
	}

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating success
	return ERR_SUCCESS;

}

BP_ERROR_T BP_FSRegSetDirectoryPolicy
(
		P_BP_FSREG fs_reg,
		BP_FSREG_DIRECTORY_POLICY_FLAG flag,
		BP_BOOL setting
)
{

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// set flag
	if(setting == BP_TRUE)
	{
		BP_SET_FLAG(fs_reg->crawl_policies.directory_policy, flag);
	}
	else
	{
		BP_UNSET_FLAG(fs_reg->crawl_policies.directory_policy, flag);
	}

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating success
	return ERR_SUCCESS;

}

BP_ERROR_T BP_FSRegSetFilePolicy
(
		P_BP_FSREG fs_reg,
		BP_FSREG_FILE_POLICY_FLAG flag,
		BP_BOOL setting
)
{

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// set flag
	if(setting == BP_TRUE)
	{
		BP_SET_FLAG(fs_reg->crawl_policies.file_policy, flag);
	}
	else
	{
		BP_UNSET_FLAG(fs_reg->crawl_policies.file_policy, flag);
	}

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating success
	return ERR_SUCCESS;

}



BP_ERROR_T BP_FSRegSetChrPolicy
(
	P_BP_FSREG fs_reg,
	BP_FSREG_CHR_POLICY_FLAG flag,
	BP_BOOL setting
)
{

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// set flag
	if(setting == BP_TRUE)
	{
		BP_SET_FLAG(fs_reg->crawl_policies.chr_policy, flag);
	}
	else
	{
		BP_UNSET_FLAG(fs_reg->crawl_policies.chr_policy, flag);
	}

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating success
	return ERR_SUCCESS;

}

BP_ERROR_T BP_FSRegSetBlkPolicy
(
	P_BP_FSREG fs_reg,
	BP_FSREG_BLK_POLICY_FLAG flag,
	BP_BOOL setting
)
{

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// set flag
	if(setting == BP_TRUE)
	{
		BP_SET_FLAG(fs_reg->crawl_policies.blk_policy, flag);
	}
	else
	{
		BP_UNSET_FLAG(fs_reg->crawl_policies.blk_policy, flag);
	}

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating success
	return ERR_SUCCESS;

}

BP_ERROR_T BP_FSRegSetSockPolicy
(
		P_BP_FSREG fs_reg,
		BP_FSREG_SOCK_POLICY_FLAG flag,
		BP_BOOL setting
)
{

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// set flag
	if(setting == BP_TRUE)
	{
		BP_SET_FLAG(fs_reg->crawl_policies.sock_policy, flag);
	}
	else
	{
		BP_UNSET_FLAG(fs_reg->crawl_policies.sock_policy, flag);
	}

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating success
	return ERR_SUCCESS;

}


// this sets a perl compatible regular expression policy for
// filesystem crawls.
BP_ERROR_T BP_FSRegAddPregexPolicy
(
		P_BP_FSREG                fs_reg,
		BP_FSREG_PREG_POLICY_TYPE preg_policy_type,
		char *                    pregex
)
{

	// basic null checks
	if(!fs_reg)
		return ERR_FAILURE;
	if(!fs_reg->hreg)
		return ERR_FAILURE;
	if(!preg_policy_type)
		return ERR_FAILURE;
	if(!pregex)
		return ERR_FAILURE;

	// ensure string is reasonable/printable
	if(!BP_StringIsReasonablePrintableString(pregex, bpstrlen(pregex), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// these hold the selected regexp set (based on type switch)
	char ** regexp_set = NULL;
	size_t  regexp_n   = 0;

	// select set based on type
	switch(preg_policy_type)
	{

		case BP_FSREG_PREG_POLICY_TYPE_FNAME_MATCH:

			// attempt to realloc policies
			fs_reg->crawl_policies.preg_fname_policies = (char **) ht_realloc
			(
					fs_reg->hreg,
					fs_reg->crawl_policies.preg_fname_policies,
					(fs_reg->crawl_policies.preg_fname_policies_n + 2) * sizeof(char *)
			);

			// overwrite the fname data
			fs_reg->crawl_policies.preg_fname_policies[fs_reg->crawl_policies.preg_fname_policies_n] = ht_strdup(fs_reg->hreg, pregex);
			fs_reg->crawl_policies.preg_fname_policies_n++;
			break;

		case BP_FSREG_PREG_POLICY_TYPE_PATHONLY_MATCH:

			// attempt to realloc policies
			fs_reg->crawl_policies.preg_pathonly_matching_policies = (char **) ht_realloc
			(
					fs_reg->hreg,
					fs_reg->crawl_policies.preg_pathonly_matching_policies,
					(fs_reg->crawl_policies.preg_pathonly_matching_policies_n + 2) * sizeof(char *)
			);

			// overwrite the fname data
			fs_reg->crawl_policies.preg_pathonly_matching_policies[fs_reg->crawl_policies.preg_pathonly_matching_policies_n] = ht_strdup(fs_reg->hreg, pregex);
			fs_reg->crawl_policies.preg_pathonly_matching_policies_n++;
			break;

		case BP_FSREG_PREG_POLICY_TYPE_FULLPATH_MATCH:

			// attempt to realloc policies
			fs_reg->crawl_policies.preg_fullpath_matching_policies = (char **) ht_realloc
			(
					fs_reg->hreg,
					fs_reg->crawl_policies.preg_fullpath_matching_policies,
					(fs_reg->crawl_policies.preg_fullpath_matching_policies_n + 2) * sizeof(char *)
			);

			// overwrite the fname data
			fs_reg->crawl_policies.preg_fullpath_matching_policies[fs_reg->crawl_policies.preg_fullpath_matching_policies_n] = ht_strdup(fs_reg->hreg, pregex);
			fs_reg->crawl_policies.preg_fullpath_matching_policies_n++;

			break;

		// exit immediately if we can't match a set
		default:
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE);
			break;

	}


	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating success
	return ERR_SUCCESS;

}

// set the policy
BP_ERROR_T BP_FSRegPolicySetFlag
(
	P_BP_FSREG           fs_reg,
	BP_FSREG_POLICY_TYPE policy_type,
	size_t               flag_id,
	BP_BOOL              flag_value

)
{


	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// set initial return value to false (set via switch below)
	BP_ERROR_T ret_val = ERR_FAILURE;

	// switch on the policy type and act where necessary
	switch(policy_type)
	{

		case BP_FSREG_POLICY_TYPE_UNSET:
			break;

		case BP_FSREG_POLICY_TYPE_SYMLINK:
			ret_val = BP_FSRegSetSymlinkPolicy
			(
				fs_reg,
				(BP_FSREG_SYMLINK_POLICY_FLAG) flag_id,
				flag_value
			);
			break;

		case BP_FSREG_POLICY_TYPE_HIDDEN:
			ret_val = BP_FSRegSetHiddenPolicy
			(
				fs_reg,
				(BP_FSREG_HIDDEN_POLICY_FLAG) flag_id,
				flag_value
			);
			break;

		case BP_FSREG_POLICY_TYPE_FIFO:
			ret_val = BP_FSRegSetFifoPolicy
			(
				fs_reg,
				(BP_FSREG_FIFO_POLICY_FLAG) flag_id,
				flag_value
			);
			break;

		case BP_FSREG_POLICY_TYPE_DIRECTORY:
			ret_val = BP_FSRegSetDirectoryPolicy
			(
				fs_reg,
				(BP_FSREG_DIRECTORY_POLICY_FLAG) flag_id,
				flag_value
			);
			break;

		case BP_FSREG_POLICY_TYPE_FILE:
			ret_val = BP_FSRegSetFilePolicy
			(
				fs_reg,
				(BP_FSREG_FILE_POLICY_FLAG) flag_id,
				flag_value
			);
			break;

		case BP_FSREG_POLICY_TYPE_CHR:
			ret_val = BP_FSRegSetChrPolicy
			(
				fs_reg,
				(BP_FSREG_CHR_POLICY_FLAG) flag_id,
				flag_value
			);
			break;

		case BP_FSREG_POLICY_TYPE_BLK:
			ret_val = BP_FSRegSetBlkPolicy
			(
				fs_reg,
				(BP_FSREG_BLK_POLICY_FLAG) flag_id,
				flag_value
			);
			break;

		case BP_FSREG_POLICY_TYPE_SOCK:
			ret_val = BP_FSRegSetSockPolicy
			(
				fs_reg,
				(BP_FSREG_SOCK_POLICY_FLAG) flag_id,
				flag_value
			);
			break;

		default:
			break;

	}

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating success
	return ret_val;

}


// set the policy flags.
BP_ERROR_T BP_FSRegPolicySetFlags
(
	P_BP_FSREG               fs_reg,
	BP_FSREG_POLICY_TYPE     policy_type,
	P_BP_FSREG_POLICY_FLAG_SET policy_flag_set
)
{

	// ensure we have a policy type
	if(!fs_reg)
		return ERR_FAILURE;
	if(!policy_type)
		return ERR_FAILURE;
	if(!policy_flag_set)
		return ERR_FAILURE;

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);


	// terminator set.
	BP_FSREG_POLICY_FLAG_SET terminator_set = BP_FSREG_FLAG_SET_TERMINATOR;

	// loop through policy flag sets till we find a terminator.
	size_t n = 0;
	for
	(
		;
		policy_flag_set[n].flag_id != 0;
		n++
	)
	{

		// set policy flag
		BP_FSRegPolicySetFlag
		(
			fs_reg,
			policy_type,
			policy_flag_set[n].flag_id,
			policy_flag_set[n].flag_value

		);

	}

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Filesystem Record Addition %%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This adds an arbitrary entry to fs_reg->hreg registry
// based on the table pointer supplied.
P_BP_FSREG_ENTRY BP_FSRegAddRecord
(
		P_BP_FSREG                  fs_reg,
		P_BP_HASH_TABLE_KEYED_ENTRY add_to_this_table,
		struct dirent *             add_this_entry
)
{

	// ensure we have a valid registry
	if(!fs_reg)
		return NULL;
	if(!fs_reg->hreg)
		return NULL;
	if(!add_this_entry)
		return NULL;

	// printf("\n Total Alloc Size for Registry?: %u", fs_reg->hreg->memory_table->managed_heap_alloc_total_size);

	// copy in dirent first off (to ensure we actually have it)
	struct dirent local_dirent_copy;
	BP_ZERO_STACK(local_dirent_copy);
	memcpy(&local_dirent_copy, add_this_entry, sizeof(local_dirent_copy));



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section / Run Basic Checks %%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// enter critical section before creating new record
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// ensure we have a top directory
	if(!fs_reg->top_dir)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, NULL);

	// ensure we have a table to add to
	if(!add_to_this_table)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, NULL);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Limit Checks %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enforce limits (this is now done in the crawl and crawl_r functions)
	BP_FSREG_ENFORCE_LIMITS_CRITICAL_SECTION(fs_reg, NULL);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Init Policy Check Strings/Vars %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// declare check table
	P_BP_HASH_TABLE_KEYED_ENTRY initial_policy_check_table = NULL;

	// set policy check table based on type
	if(add_this_entry->d_type == DT_DIR)
	{
		initial_policy_check_table = (P_BP_HASH_TABLE_KEYED_ENTRY) add_to_this_table->parent;
	}
	else
	{
		initial_policy_check_table = add_to_this_table;
	}

	// create path from selected table
	char * parent_dir_path = BP_FSRegLookupFilesystemPathFromEntry
	(
			fs_reg,
			initial_policy_check_table
	);

	// set path lookup
	char *path_lookup[] =
	{
			parent_dir_path,
			"/",
			(char *)&add_this_entry->d_name,
			NULL
	};

	// generate the full file path initially
	char * initial_test_full_file_path = BP_JoinStringArray
	(
		(char **) &path_lookup,
		3,
		NULL
	);

	// clean the path
	char * initial_test_full_file_path_clean = BP_FindReplace
	(
		initial_test_full_file_path,
		(char *) "//",
		(char *) "/",
		1,
		BP_KILOBYTE * 2
	);


	// set final test buff
	char * final_test_buff = bpstrdup
	(
			initial_test_full_file_path_clean ?
					initial_test_full_file_path_clean : initial_test_full_file_path
	);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Fill lstat() Info %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// temporary stat structure
	struct stat tmp_lstat;
	BP_ZERO_STACK(tmp_lstat);

	// attempt to stat the file (lstat for link preservation)
	lstat(final_test_buff, &tmp_lstat);


	// --- Fill stat() Info ---

	// temporary stat structure
	struct stat tmp_stat;
	BP_ZERO_STACK(tmp_stat);

	// attempt to stat the file (lstat for link preservation)
	stat(final_test_buff, &tmp_stat);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% run policy checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create and zero a policy analysis structure
	BP_FSREG_FILE_POLICY_ANALYSIS policy_analysis;
	BP_ZERO_STACK(policy_analysis);

	// attempt to check policies
	BP_BOOL crawl_policy_check_result = BP_FSRegCheckFileAgainstCrawlPolicy
	(
			fs_reg,
			&fs_reg->crawl_policies,
			add_to_this_table,
			(char *)&add_this_entry->d_name,
			parent_dir_path,
			final_test_buff,
			&tmp_stat,
			&tmp_lstat,
			add_this_entry,
			&policy_analysis
	);

	// BP_FSRegDisplayFilePolicyAnalysis(&policy_analysis,BP_TRUE, (char *) "\n BOOO:  -- ");

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check Crawl Policy Results %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if the policy check fails, exit here (destroying data first)
	if(!crawl_policy_check_result)
	{
		if(initial_test_full_file_path)
		{
			bpfree(initial_test_full_file_path);
			initial_test_full_file_path = NULL;
		}
		if(initial_test_full_file_path_clean)
		{
			bpfree(initial_test_full_file_path_clean);
			initial_test_full_file_path_clean = NULL;
		}
		if(final_test_buff)
		{
			bpfree(final_test_buff);
			final_test_buff = NULL;
		}
		if(parent_dir_path)
		{
			bpfree(parent_dir_path);
			parent_dir_path = NULL;
		}

		// EXIT HERE IF POLICY FAILS
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, NULL);

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Prevent Duplicate Records %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure entry doesn't already exist
	P_BP_HASH_TABLE_KEYED_ENTRY keyed_entry = BP_HashRegLookupEntryFromHashKeyViaRelativePosition
	(
			fs_reg->hreg,
			add_to_this_table,
			(char *) &add_this_entry->d_name
	);

	if(keyed_entry)
	{
		if(initial_test_full_file_path)
		{
			bpfree(initial_test_full_file_path);
			initial_test_full_file_path = NULL;
		}
		if(initial_test_full_file_path_clean)
		{
			bpfree(initial_test_full_file_path_clean);
		}
		if(final_test_buff)
		{
			bpfree(final_test_buff);
			final_test_buff = NULL;
		}
		if(parent_dir_path)
		{
			bpfree(parent_dir_path);
			parent_dir_path = NULL;
		}
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, NULL);
	}
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add to Table After Policy Checks %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to add the data to the table
	BP_HashRegAddDataToTable
	(
		fs_reg->hreg,
		add_to_this_table,
		(char *) &add_this_entry->d_name,
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_FSREG_ENTRY)
	);

	// attempt to lookup the keyed entry
	keyed_entry = BP_HashRegLookupEntryFromHashKeyViaRelativePosition
	(
			fs_reg->hreg,
			add_to_this_table,
			(char *) &add_this_entry->d_name
	);


	// ensure we can lookup the entry
	if(!keyed_entry)
	{
		if(initial_test_full_file_path)
		{
			bpfree(initial_test_full_file_path);
			initial_test_full_file_path = NULL;
		}
		if(initial_test_full_file_path_clean)
		{
			bpfree(initial_test_full_file_path_clean);
		}
		if(final_test_buff)
		{
			bpfree(final_test_buff);
			final_test_buff = NULL;
		}
		if(parent_dir_path)
		{
			bpfree(parent_dir_path);
			parent_dir_path = NULL;
		}
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, NULL);
	}

	// set the fsreg entry from data (always populated at correct size if successful)
	P_BP_FSREG_ENTRY fsreg_entry = (P_BP_FSREG_ENTRY) keyed_entry->data;

	// increment the total entry count
	fs_reg->entry_n++;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Copy in Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// copy in the results of policy analysis
	memcpy((P_BP_FSREG_FILE_POLICY_ANALYSIS)&fsreg_entry->policy_analysis, &policy_analysis, sizeof(BP_FSREG_FILE_POLICY_ANALYSIS));
	fsreg_entry->policy_matched = BP_TRUE;

	// copy in directory entry
	memcpy(&fsreg_entry->dirent_data, &local_dirent_copy, sizeof(fsreg_entry->dirent_data));



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Lookup Individual Paths %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create initial type
	BP_FSREG_ENTRY_TYPE type = (BP_FSREG_ENTRY_TYPE) 0;

	// this will be set to the filesystem path (based around types)
	char *filesystem_path = NULL;

	// switch on the type and translate it to internal enum
	switch(add_this_entry->d_type)
	{

		case DT_DIR:

			// set type
			type = BP_FSREG_ENTRY_TYPE_DIR;

			// create path from parent since it's a dir entry
			filesystem_path = BP_FSRegLookupFilesystemPathFromEntry
			(
					fs_reg,
					(P_BP_HASH_TABLE_KEYED_ENTRY) keyed_entry->parent
			);
			fs_reg->directory_n++;
			break;

		case DT_CHR:

			// set type
			type = BP_FSREG_ENTRY_TYPE_CHR;

			// attempt to lookup path from the new keyed entry
			filesystem_path = BP_FSRegLookupFilesystemPathFromEntry
			(
					fs_reg,
					keyed_entry
			);
			fs_reg->chr_n++;
			break;

		case DT_BLK:

			// set type
			type = BP_FSREG_ENTRY_TYPE_BLK;

			// attempt to lookup path from the new keyed entry
			filesystem_path = BP_FSRegLookupFilesystemPathFromEntry
			(
					fs_reg,
					keyed_entry
			);
			fs_reg->blk_n++;
			break;

		case DT_REG:

			// set type
			type = BP_FSREG_ENTRY_TYPE_REG;

			// attempt to lookup path from the new keyed entry
			filesystem_path = BP_FSRegLookupFilesystemPathFromEntry
			(
					fs_reg,
					keyed_entry
			);
			printf("\n Adding file: %s", filesystem_path);
			fs_reg->file_n++;
			break;

		case DT_FIFO:

			// set type
			type = BP_FSREG_ENTRY_TYPE_FIFO;

			// attempt to lookup path from the new keyed entry
			filesystem_path = BP_FSRegLookupFilesystemPathFromEntry
			(
					fs_reg,
					keyed_entry
			);
			fs_reg->fifo_n++;
			break;

		case DT_LNK:

			// set type
			type = BP_FSREG_ENTRY_TYPE_LNK;

			// attempt to lookup path from the new keyed entry
			filesystem_path = BP_FSRegLookupFilesystemPathFromEntry
			(
					fs_reg,
					keyed_entry
			);
			// printf("\n HERE HERE FOUND SYMLINK?");
			fs_reg->symlink_n++;
			break;

		case DT_SOCK:

			// set type
			type = BP_FSREG_ENTRY_TYPE_SOCK;

			// attempt to lookup path from the new keyed entry
			filesystem_path = BP_FSRegLookupFilesystemPathFromEntry
			(
					fs_reg,
					keyed_entry
			);
			fs_reg->sock_n++;
			break;

		default:
			type = BP_FSREG_ENTRY_TYPE_UNSET;
			break;

	}


	// set entry type
	fsreg_entry->type = type;

	// fileystem path
	if(filesystem_path)
	{

		// set filesystem path
		fsreg_entry->full_path = (char *) ht_strdup(fs_reg->hreg, filesystem_path);

		// destroy the filesystem
		bpfree(filesystem_path);
		filesystem_path = NULL;


		// --- Fill lstat() Info ---

		// temporary stat structure
		BP_ZERO_STACK(tmp_stat);

		// attempt to stat the file (lstat for link preservation)
		lstat(fsreg_entry->full_path, &tmp_stat);

		// copy in the stat data
		memcpy((struct stat *) &fsreg_entry->lstat_data, &tmp_stat, sizeof(struct stat));


		// --- Fill stat() Info ---

		// temporary stat structure
		BP_ZERO_STACK(tmp_stat);

		// attempt to stat the file (lstat for link preservation)
		stat(fsreg_entry->full_path, &tmp_stat);

		// copy in the stat data
		memcpy((struct stat *) &fsreg_entry->stat_data, &tmp_stat, sizeof(struct stat));


	}

	// run final checks before exit
	if(initial_test_full_file_path)
	{
		bpfree(initial_test_full_file_path);
		initial_test_full_file_path = NULL;
	}
	if(initial_test_full_file_path_clean)
	{
		bpfree(initial_test_full_file_path_clean);
	}
	if(final_test_buff)
	{
		bpfree(final_test_buff);
		final_test_buff = NULL;
	}
	if(parent_dir_path)
	{
		bpfree(parent_dir_path);
		parent_dir_path = NULL;
	}

	// leave the critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating success
	return fsreg_entry;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Filesystem Crawler %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// USE THIS: Attempt to crawl the filesystem based on the registry configuration.  This should be used
// unless you've studied BP_FSRegCrawl_r and understand how to use it safely on it's own.  Basically,
// use this in all cases, don't use BP_FSRegCrawl_r directly.
BP_ERROR_T BP_FSRegCrawl
(
		P_BP_FSREG fs_reg
)
{


	if(!fs_reg)
		return ERR_FAILURE;
	if(!fs_reg->hreg)
		return ERR_FAILURE;

	// enter critical section before crawling
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// ensure we have a top directory
	if(!fs_reg->top_dir)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Limit Checks %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enforce limits
	BP_FSREG_ENFORCE_LIMITS_CRITICAL_SECTION(fs_reg, ERR_FAILURE);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Preliminary Top Dir Checks %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure that the directory is not ./
	if(bpstrlen(fs_reg->top_dir) == 1)
	if(fs_reg->top_dir[0] == '.')
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE);

	// ensure that the directory is not ../
	if(bpstrlen(fs_reg->top_dir) == 2)
	if(fs_reg->top_dir[0] == '.')
	if(fs_reg->top_dir[1] == '.')
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE);

	// ensure that we have a reasonable directory to open
	if(!BP_StringIsReasonableForUnixEXT4Directory(fs_reg->top_dir, bpstrlen(fs_reg->top_dir), BP_FALSE))
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Open Directory %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// directory and entity pointers (used in loop logic)
	DIR *           dir              = NULL;
	struct dirent * dir_entry_result = NULL;
	struct dirent   dir_entry;
	BP_ZERO_STACK(dir_entry);




	// attempt to open the directory
	dir = opendir(fs_reg->top_dir);
	if(!dir)
	{
		printf("\n [!!] %s could not be opened for filesystem walking.", fs_reg->top_dir);
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE);
	}


	// generated crawl directory
	char * crawl_dir       = NULL;
	char * join_strings[4] = {0};
	size_t join_iter       = 0;

	// exploded lookup path
	char ** lookup_path = NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt To Add Top Record %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// added records
	P_BP_HASH_TABLE_KEYED_ENTRY add_to_this_table = NULL;
	P_BP_HASH_TABLE_KEYED_ENTRY top_table         = NULL;

	// this is the initial addition path used for the initial table add
	char *initial_add_path[3] = {0};

	// set initial path
	initial_add_path[0] = (char *) fs_reg->top_dir;
	initial_add_path[1] = NULL;

	// attempt to add table to the registry
	BP_HashRegAddTableByPath(fs_reg->hreg, (char **) &initial_add_path);

	// attempt to lookup the entry after addition
	top_table = BP_HashRegLookupTableByPath(fs_reg->hreg, (char **) &initial_add_path);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Recursing Loop for Dirs %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// read directory entries
	while
	(
		!readdir_r(dir, &dir_entry, &dir_entry_result)
	)
	{

		// if there is no result, break
		if(!dir_entry_result)
			break;

		switch(dir_entry.d_type)
		{

			// store directory type, recurse here
			case DT_DIR:

				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				// %%% Run Initial Sanity Checks %%%%%%%%%%%%%%%
				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

				// ensure that we have a reasonable directory to open
				if(!BP_StringIsReasonablePrintableString(dir_entry.d_name, strnlen(dir_entry.d_name, 255), BP_FALSE, BP_FALSE))
					BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE);

				// ensure that the directory is not ./
				if(bpstrlen(dir_entry.d_name) == 1)
				if(dir_entry.d_name[0] == '.')
					break;

				// ensure that the directory is not ../
				if(bpstrlen(dir_entry.d_name) == 2)
				if(dir_entry.d_name[0] == '.')
				if(dir_entry.d_name[1] == '.')
					break;

				// ensure that the directory doesn't somehow have a
				// forward slash in it.
				if(strstr(dir_entry.d_name, "/"))
					break;



				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				// %%% Join Path for Recursion %%%%%%%%%%%%%%%%%%%%
				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

				// reset join itero to zero
				join_iter = 0;

				// set strings to join (middle / is only set if not set previously)
				join_strings[join_iter] = fs_reg->top_dir;
				join_iter++;

				// add / to the joining strings
				if(fs_reg->top_dir[bpstrlen(fs_reg->top_dir)] == '/')
				{
						join_strings[join_iter] = "/";
						join_iter++;
				}

				// add new directory name
				join_strings[join_iter] = (char *) &dir_entry.d_name;
				join_iter++;

				// join the crawl directories
				crawl_dir = BP_JoinStringArray(join_strings, join_iter, NULL);

				// attempt to add table path
				if(crawl_dir)
				{

					// set initial path
					initial_add_path[0] = (char *) fs_reg->top_dir;
					initial_add_path[1] = (char *) &dir_entry.d_name;
					initial_add_path[2] = NULL;

					// attempt to add table to the registry
					BP_HashRegAddTableByPath(fs_reg->hreg, (char **) &initial_add_path);

					// attempt to lookup the entry after addition
					add_to_this_table = BP_HashRegLookupTableByPath(fs_reg->hreg, (char **) &initial_add_path);

					// exit if data addition failed
					if(!add_to_this_table)
					{
						if(crawl_dir)
							bpfree(crawl_dir);
						BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE);
					}


					// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
					// %%% Add At Position %%%%%%%%%%%%%%%%%%%%%%%%%%%%
					// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

					// attempt to add directory record within directory
					BP_FSRegAddRecord
					(
							fs_reg,
							add_to_this_table,
							&dir_entry
					);
					BP_ZERO_STACK(dir_entry);

					// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
					// %%% Crawl Recusion %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
					// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

					// run recursing from top position (pass null pointer)
					BP_FSRegCrawl_r(fs_reg, crawl_dir, add_to_this_table);

				}

				if(crawl_dir)
				{
					bpfree(crawl_dir);
					crawl_dir = NULL;
				}
				break;

			default:
				break;
		}

	}

	// close the directory
	if(dir)
		closedir(dir);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Second Limit Checks %%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enforce limits
	BP_FSREG_ENFORCE_LIMITS_CRITICAL_SECTION(fs_reg, ERR_FAILURE);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Top Directory Additive Loop %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// innter dir (when a directory is encountered, this is opened for
	// secondary inner parsing)
	DIR *           dir_inner              = NULL;
	struct dirent * dir_entry_inner_result = NULL;
	struct dirent   dir_entry_inner;
	BP_ZERO_STACK(dir_entry_inner);


	// attempt to re-oopen the directory
	dir_inner = opendir(fs_reg->top_dir);
	if(!dir_inner)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE);


	// read directory entries
	while
	(
		!readdir_r(dir_inner, &dir_entry_inner, &dir_entry_inner_result)
	)
	{

		// break if there is no result
		if(!dir_entry_inner_result)
			break;

		// switch on the entry type
		switch(dir_entry_inner.d_type)
		{

			// regular file type
			case DT_REG:

				// attempt to add a regular file to the top table
				BP_FSRegAddRecord
				(
						fs_reg,
						(P_BP_HASH_TABLE_KEYED_ENTRY) top_table,
						&dir_entry_inner
				);
				BP_ZERO_STACK(dir_entry_inner);
				break;

			// record character type
			case DT_CHR:

				// attempt to add character device to the top table
				BP_FSRegAddRecord
				(
						fs_reg,
						(P_BP_HASH_TABLE_KEYED_ENTRY) top_table,
						&dir_entry_inner
				);
				BP_ZERO_STACK(dir_entry_inner);
				break;

			// record block character type
			case DT_BLK:

				// attempt to add block device to the top table
				BP_FSRegAddRecord
				(
						fs_reg,
						(P_BP_HASH_TABLE_KEYED_ENTRY) top_table,
						&dir_entry_inner
				);
				BP_ZERO_STACK(dir_entry_inner);
				break;

			// record fifo character type
			case DT_FIFO:

				// attempt to add directory record to the top of the hive
				BP_FSRegAddRecord
				(
						fs_reg,
						(P_BP_HASH_TABLE_KEYED_ENTRY) top_table,
						&dir_entry_inner
				);
				BP_ZERO_STACK(dir_entry_inner);
				break;

			// record symlink file type
			case DT_LNK:

				// attempt to add symbolic link
				BP_FSRegAddRecord
				(
						fs_reg,
						(P_BP_HASH_TABLE_KEYED_ENTRY) top_table,
						&dir_entry_inner
				);
				BP_ZERO_STACK(dir_entry_inner);
				break;

				// record socket file type
			case DT_SOCK:

				// attempt to add socket record to the top of the hive
				BP_FSRegAddRecord
				(
						fs_reg,
						(P_BP_HASH_TABLE_KEYED_ENTRY) top_table,
						&dir_entry_inner
				);
				BP_ZERO_STACK(dir_entry_inner);
				break;

			// break loop here
			default:
				break;

		}

	}

	// close the directory
	if(dir_inner)
	{
		closedir(dir_inner);
		dir_inner = NULL;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Exit Critical Section %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave when crawling is finished
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating success
	return ERR_SUCCESS;


}


// DO NOT USE THIS DIRECTLY:
// Attempt to crawl the filesystem based on the registry configuration, recursively with a
// recursing handle set.
BP_ERROR_T BP_FSRegCrawl_r
(
		P_BP_FSREG fs_reg,
		char * crawl_dir,
		P_BP_HASH_TABLE_KEYED_ENTRY parent_dir
)
{

	// basic null checks
	if(!fs_reg)
		return ERR_FAILURE;
	if(!fs_reg->hreg)
		return ERR_FAILURE;
	if(!parent_dir)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Limit Checks %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enforce limits
	BP_FSREG_ENFORCE_LIMITS_CRITICAL_SECTION(fs_reg, ERR_FAILURE);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Lock Registry + Basic Checks %%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section before crawling
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// ensure we have a top directory
	if(!fs_reg->top_dir)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE);

	// ensure the crawl dir is within our top dir
	if(memcmp(crawl_dir, fs_reg->top_dir, bpstrlen(fs_reg->top_dir)) != 0)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE);

	// the crawl dir must be longer than the top dir
	if(bpstrlen(crawl_dir) <= bpstrlen(fs_reg->top_dir))
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE);

	// set the adjusted relative file path
	char * adjusted_relative_file_path = (char *) &crawl_dir[bpstrlen(fs_reg->top_dir)];
	char * adjusted_explode_file_path  = (char *) NULL;

	// ensure that the relative file path is set
	if(!bpstrlen(adjusted_relative_file_path))
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Open Directory %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// directory and entity pointers (used in loop logic)
	DIR *           dir              = NULL;
	struct dirent * dir_entry_result = NULL;
	struct dirent   dir_entry;
	BP_ZERO_STACK(dir_entry);

	// inner dir (when a directory is encountered, this is opened for
	// secondary inner parsing)
	DIR *           dir_inner              = NULL;
	struct dirent * dir_entry_inner_result = NULL;
	struct dirent   dir_entry_inner;
	BP_ZERO_STACK(dir_entry_inner);

	// generated crawl directory
	char * recurse_crawl_dir = NULL;
	char * join_strings[4]   = {0};
	size_t join_iter         = 0;

	// added records
	P_BP_HASH_TABLE_KEYED_ENTRY add_to_this_table   = NULL;


	// this will hold a dynamic table path as is encountered
	char ** table_path = (char **) NULL;

	// adjustment path (don't ever free this)
	char ** table_path_adjusted = (char **) NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% First Crawl Only Adds Directories %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to open the directory
	dir = opendir(crawl_dir);
	if(!dir)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE);

	// verbosity
	printf("\n Crawl directory?: %s", crawl_dir);


	// read directory entries
	while
	(
		!readdir_r(dir, &dir_entry, &dir_entry_result)
	)
	{

		// if there is no result, break
		if(!dir_entry_result)
			break;

		switch(dir_entry.d_type)
		{

			// store directory type, recurse here
			case DT_DIR:

				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				// %%% Run Preliminary Checks %%%%%%%%%%%
				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

				// ensure that we have a reasonable directory to open
				if(!BP_StringIsReasonablePrintableString(dir_entry.d_name, strnlen(dir_entry.d_name, 255), BP_FALSE, BP_FALSE))
					BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE);

				// ensure that the directory is not ./
				if(bpstrlen((char *) &dir_entry.d_name) == 1)
				if(dir_entry.d_name[0] == '.')
					break;

				// ensure that the directory is not ../
				if(bpstrlen((char *) &dir_entry.d_name) == 2)
				if(dir_entry.d_name[0] == '.')
				if(dir_entry.d_name[1] == '.')
					break;

				// ensure that the directory doesn't somehow have a
				// forward slash in it.
				if(strstr(dir_entry.d_name, "/"))
					break;


				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				// %%% Prepare Join String %%%%%%%%%%%%%%
				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

				// zero out the join string position before advancing (clears stack data)
				BP_ZERO_STACK(join_strings);

				// reset join itero to zero
				join_iter = 0;

				// set top dir
				join_strings[join_iter] = fs_reg->top_dir;
				join_iter++;

				// set strings to join (middle / is only set if not set previously)
				join_strings[join_iter] = adjusted_relative_file_path;
				join_iter++;

				// add / to the joining strings if necessary
				if(crawl_dir[bpstrlen(crawl_dir)] != '/')
				{
						join_strings[join_iter] = "/";
						join_iter++;
				}

				// add new directory name
				join_strings[join_iter] = (char *) &dir_entry.d_name;
				join_iter++;

				// join the crawl directories
				recurse_crawl_dir = BP_JoinStringArray(join_strings, join_iter, NULL);



				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				// %%% Add Directory Table Here %%%%%%%%%%%%
				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

				// set the adjusted explode path
				adjusted_explode_file_path = (char *) &recurse_crawl_dir[bpstrlen(fs_reg->top_dir)];

				// The directory has 2 entries, one is actually
				// a keyed path within our hash registry and is a
				// hash table type.  The other is a representation within
				// that directory of the directory itself.
				table_path = BP_ExplodeBufferBin(adjusted_explode_file_path, bpstrlen(adjusted_explode_file_path), "/", 1, 512);


				// pickup work here tomorrow, we need to identify directories from files/special types
				// and create entries for them.
				//printf("\n [+] Dir Entry Here2: %p - %p - %s - %s - %s", table_path, dir_entry, dir_entry->d_name, recurse_crawl_dir, crawl_dir);
				printf("\n recurse crawl dir: %s - %p (path)\n", recurse_crawl_dir, table_path);

				// attempt to add table path
				if(table_path)
				{


					// set initial adjusted path (don't free)
					table_path_adjusted = table_path;




					// null terminated pointer count (for expanding)
					size_t ptr_n = BP_CountNullTerminatedPtrArray(table_path);

					// increment the pointer count
					ptr_n++;

					// attempt to allocate new space
					table_path_adjusted = (char **) bpcalloc( sizeof(char *) * (ptr_n + 4), 1);

					// set the top directory
					table_path_adjusted[0] = bpstrdup(fs_reg->top_dir);

					// set the adjustments in place
					size_t n = 1;
					for(; (n < ptr_n) && table_path[n-1]; n++)
					{
						table_path_adjusted[n] = table_path[n-1];
					}

					// printf("\n Attempting to add sub adjusted: \n");
					// BP_DisplayStringArray(" -- ", 1, 1, (char **) table_path_adjusted);
					// printf("\n");

					// attempt to add table to the registry
					BP_HashRegAddTableByPath(fs_reg->hreg, (char **) table_path_adjusted);

					// attempt to lookup the entry after addition
					add_to_this_table = BP_HashRegLookupTableByPath(fs_reg->hreg, (char **) table_path_adjusted);

					// set adjusted
					if(table_path_adjusted[0])
					{
						bpfree(table_path_adjusted[0]);
					}
					if(table_path_adjusted)
					{
						bpfree(table_path_adjusted);
					}

					printf("\n Got new table?!: %p", add_to_this_table);

					// exit if data addition failed
					if(!add_to_this_table)
					{
						BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE);
					}


					// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
					// %%% Add At Position %%%%%%%%%%%%%%%%%%%%%%%%%%%%
					// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

					// attempt to add directory record within directory
					BP_FSRegAddRecord
					(
							fs_reg,
							add_to_this_table,
							&dir_entry
					);
					BP_ZERO_STACK(dir_entry);

					// attempt to recurse
					if(recurse_crawl_dir)
					{

						// run recursing from top position (pass null pointer)
						BP_FSRegCrawl_r(fs_reg, recurse_crawl_dir, add_to_this_table);

					}

					// destroy the ACTUAL table path, not table_path_adjusted.
					BP_DestroyPointerArrayNullTerminated((void **) table_path);
					bpfree(table_path);
					table_path = NULL;

				}

				// destroy table path if it exists
				if(table_path)
				{
					size_t iter_n = 0;
					for(; table_path[iter_n]; iter_n++){}
					BP_DestroyPointerArray((void **) table_path, iter_n);
					bpfree(table_path);
				}

				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				// %%% inner loop for adding files %%%%%%%%%%%%%%%%%%%%%%
				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

				printf("\n Inner is opening dir: %s", crawl_dir);

				// attempt to re-oopen the directory
				dir_inner = opendir(crawl_dir);
				if(!dir_inner)
					BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE);


				// read directory entries
				while
				(
					!readdir_r(dir_inner, &dir_entry_inner, &dir_entry_inner_result)
				)
				{

					// exit if we have no result
					if(!dir_entry_inner_result)
						break;


					// switch on the entry type
					switch(dir_entry_inner.d_type)
					{

						// regular file type
						case DT_REG:

							// set the adjusted explode path
							adjusted_explode_file_path = (char *) &recurse_crawl_dir[bpstrlen(fs_reg->top_dir)];

							// The directory has 2 entries, one is actually
							// a keyed path within our hash registry and is a
							// hash table type.  The other is a representation within
							// that directory of the directory itself.
							table_path = BP_ExplodeBufferBin(adjusted_explode_file_path, bpstrlen(adjusted_explode_file_path), "/", 1, 512);

							// printf("\n Adjusted path is what?: \n%s\n%s\n\n", adjusted_explode_file_path, recurse_crawl_dir);

							// pickup work here tomorrow, we need to identify directories from files/special types
							// and create entries for them.
							//printf("\n [+] Dir Entry Here2: %p - %p - %s - %s - %s", table_path, dir_entry, dir_entry->d_name, recurse_crawl_dir, crawl_dir);

							// attempt to add table path
							if(table_path)
							{


								// set initial adjusted path (don't free)
								table_path_adjusted = table_path;

								// null terminated pointer count (for expanding)
								size_t ptr_n = BP_CountNullTerminatedPtrArray(table_path);

								// increment the pointer count
								ptr_n++;

								// attempt to allocate new space
								table_path_adjusted = (char **) bpcalloc( sizeof(char *) * (ptr_n + 4), 1);

								// set the top directory
								table_path_adjusted[0] = bpstrdup(fs_reg->top_dir);

								// set the adjustments in place
								size_t n = 1;
								for(; (n < (ptr_n-1)) && table_path[n-1]; n++)
								{
									table_path_adjusted[n] = table_path[n-1];
								}

								// printf("\n OH GOD MY BRAIN: \n");
								// BP_DisplayStringArray(" -- ", 1, 1, (char **) table_path_adjusted);
								// printf("\n");

								// attempt to lookup the entry after addition
								add_to_this_table = BP_HashRegLookupTableByPath(fs_reg->hreg, (char **) table_path_adjusted);

								// exit if data addition failed
								if(!add_to_this_table)
								{

									// destroy all memory
									if(table_path_adjusted[0])
										bpfree(table_path_adjusted[0]);
									if(table_path_adjusted)
										bpfree(table_path_adjusted);
									if(table_path)
									{
										BP_DestroyPointerArrayNullTerminated((void **) table_path);
										bpfree(table_path);
									}

									BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, ERR_FAILURE);
								}

								// attempt to add directory record within directory
								BP_FSRegAddRecord
								(
										fs_reg,
										add_to_this_table,
										&dir_entry_inner
								);

								// destroy all memory
								if(table_path_adjusted[0])
									bpfree(table_path_adjusted[0]);
								if(table_path_adjusted)
									bpfree(table_path_adjusted);
								if(table_path)
								{
									BP_DestroyPointerArrayNullTerminated((void **) table_path);
									bpfree(table_path);
								}

								BP_ZERO_STACK(dir_entry_inner);
							}

							break;

						// record character type
						case DT_CHR:
							BP_FSRegAddRecord
							(
									fs_reg,
									add_to_this_table,
									&dir_entry_inner
							);
							BP_ZERO_STACK(dir_entry_inner);
							break;

						// record block character type
						case DT_BLK:
							BP_FSRegAddRecord
							(
									fs_reg,
									add_to_this_table,
									&dir_entry_inner
							);
							BP_ZERO_STACK(dir_entry_inner);
							break;

						// record fifo character type
						case DT_FIFO:
							BP_FSRegAddRecord
							(
									fs_reg,
									add_to_this_table,
									&dir_entry_inner
							);
							BP_ZERO_STACK(dir_entry_inner);
							break;

						// record symlink character type
						case DT_LNK:
							BP_FSRegAddRecord
							(
									fs_reg,
									add_to_this_table,
									&dir_entry_inner
							);
							BP_ZERO_STACK(dir_entry_inner);
							break;

						// record socket character type
						case DT_SOCK:
							BP_FSRegAddRecord
							(
									fs_reg,
									add_to_this_table,
									&dir_entry_inner
							);
							BP_ZERO_STACK(dir_entry_inner);
							break;

						// break loop here
						default:
							break;

					}

				}

				// close the directory
				closedir(dir_inner);
				dir_inner = NULL;

				// %%% END INNER ADDITIVE LOOP %%%%%%%%%%%%

				break;


			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Remaining Inner Addition Loop %%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// regular file type
			case DT_REG:

				// attempt to add directory record within directory
				BP_FSRegAddRecord
				(
						fs_reg,
						parent_dir,
						&dir_entry
				);
				BP_ZERO_STACK(dir_entry);
				break;

			// record character type
			case DT_CHR:
				BP_FSRegAddRecord
				(
						fs_reg,
						parent_dir,
						&dir_entry
				);
				BP_ZERO_STACK(dir_entry);
				break;

			// record block character type
			case DT_BLK:
				BP_FSRegAddRecord
				(
						fs_reg,
						parent_dir,
						&dir_entry
				);
				BP_ZERO_STACK(dir_entry);
				break;

			// record fifo character type
			case DT_FIFO:
				BP_FSRegAddRecord
				(
						fs_reg,
						parent_dir,
						&dir_entry
				);
				BP_ZERO_STACK(dir_entry);
				break;

			// record symlink character type
			case DT_LNK:
				BP_FSRegAddRecord
				(
						fs_reg,
						parent_dir,
						&dir_entry
				);
				BP_ZERO_STACK(dir_entry);
				break;

			// record socket character type
			case DT_SOCK:
				BP_FSRegAddRecord
				(
						fs_reg,
						parent_dir,
						&dir_entry
				);
				BP_ZERO_STACK(dir_entry);
				break;

			default:
				break;
		}

	}

	// close the directory
	closedir(dir);

	// destroy the recurse crawl directory if its set
	if(recurse_crawl_dir)
	{
		bpfree(recurse_crawl_dir);
		recurse_crawl_dir = NULL;
	}

	// leave when crawling is finished
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);


}


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
	char *     filesystem_path
)
{

	// ensure we have a filesystem registry
	if(!fs_reg)
		return NULL;

	// ensure we have a filesystem path
	if(!filesystem_path)
		return NULL;

	// ensure the filesystem path is reasonable
	if(!BP_StringIsReasonablePrintableString(filesystem_path, bpstrlen(filesystem_path), BP_FALSE, BP_FALSE))
		return NULL;

	// attempt to explode the buffer
	char ** table_path = BP_ExplodeBuffer(filesystem_path, "/");
	if(!table_path)
		return NULL;

	// display the string array
	// printf("\n Exploding: %s", filesystem_path);
	// BP_DisplayStringArray("path: ", 1, 1, table_path);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% attempt lookup %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt to lookup the entry as a table-entry or data-entry.
	P_BP_HASH_TABLE_KEYED_ENTRY lookup_entry = BP_HashRegLookupTableByPath(fs_reg->hreg, table_path);
	if(!lookup_entry)
		lookup_entry = BP_HashRegLookupTableEntryByPath(fs_reg->hreg, table_path);


	// destroy the exploded data
	if(table_path)
		BP_DestroyPointerArrayNullTerminated((void **) table_path);

	// return the looked up entry (or null if it's not there)
	return lookup_entry;

}


// This attempts to lookup a filesystem path from an entry by crawling backwards
// to the root directory.  The result is stored in the hash table, so it
// must be free'd later with ht_free.  The result from this should be valid
// for a filesystem lookup (aka BP_FOpen etc).  This routine automatically
// strips out double slashes.
char * BP_FSRegLookupFilesystemPathFromEntry
(
		P_BP_FSREG                  fs_reg,
		P_BP_HASH_TABLE_KEYED_ENTRY lookup_entry
)
{

	if(!fs_reg)
		return NULL;
	if(!fs_reg->hreg)
		return NULL;
	if(!lookup_entry)
		return NULL;

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// this will hold the final path to resources
	char *final_path = NULL;

	// attempt to create a path
	char * created_path = BP_HashRegCreateReasonableStringPathFromKeyedEntry
	(
		fs_reg->hreg,
		lookup_entry
	);

	// ensure we can create a path
	if(!created_path)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(fs_reg->hreg, NULL);

	// run find and replace all double slashes
	char * adjusted_path       = bpstrdup(created_path);
	char * adjusted_path_saved = NULL;

	// destroy old created path (since it's allocated via hash table)
	ht_free(fs_reg->hreg, created_path);


	do
	{

		// preserve adjusted path
		adjusted_path_saved = adjusted_path;

		/// attempt find and replace
		adjusted_path = BP_FindReplace(adjusted_path, "//", "/", 1, 100000);
		if(!adjusted_path)
		{

			// exit with the last saved adjustment if there
			// was no adjustment to be made during this iteration.
			adjusted_path = adjusted_path_saved;
			break;

		}

		// destroy the saved path
		bpfree(adjusted_path_saved);
		adjusted_path_saved = NULL;

	} while(1);

	// ensure final path is zeroed
	final_path = NULL;

	// move up path one character if we have a relative path start
	if(bpstrlen(adjusted_path) > 3)
	{
		if(memcmp(adjusted_path, "/./", 3) == 0)
		{
			final_path = bpstrdup((char *) &adjusted_path[1]);
		}
	}

	// set final path as adjusted path
	if(!final_path)
		final_path = bpstrdup(adjusted_path);

	// destroy the adjusted path if necessary
	if(adjusted_path)
		bpfree(adjusted_path);

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return the created path
	return final_path;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Display an entier filesystem registry
BP_ERROR_T BP_FSRegDisplay(P_BP_FSREG fs_reg)
{

	if(!fs_reg)
		return ERR_FAILURE;
	if(!fs_reg->hreg)
		return ERR_FAILURE;

	// enter critical section before crawling
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// display top part
	printf("\n [+] Displaying file registry for %s", fs_reg->top_dir);
	printf("\n");
	printf("\n      hreg:                                           %p", fs_reg->hreg);
	printf("\n      top_dir:                                        %s", fs_reg->top_dir);
	printf("\n      full_size_of_all_files:                         %u", fs_reg->full_size_of_all_files);
	printf("\n      full_size_of_selected_files:                    %u", fs_reg->full_size_of_selected_files);
	printf("\n      lowest_recursion_directory:                     %u", fs_reg->lowest_recursion_directory);
	printf("\n      crawling_finished:                              %s", fs_reg->crawling_finished ? "yes" : "no");
	printf("\n      crawling_finished_ok:                           %s", fs_reg->crawling_finished_ok ? "yes" : "no");
	printf("\n      recursion_max_directory_depth_reached:          %s", fs_reg->recursion_max_directory_depth_reached ? "yes" : "no");
	printf("\n      recursed_bottom_directory_has_child_directory:  %s", fs_reg->recursed_bottom_directory_has_child_directory ? "yes" : "no");
	printf("\n      contains_directory:                             %s", fs_reg->contains_directory ? "yes" : "no");
	printf("\n      contains_hidden_directories:                    %s", fs_reg->contains_hidden_directories ? "yes" : "no");
	printf("\n      contains_fifo:                                  %s", fs_reg->contains_fifo ? "yes" : "no");
	printf("\n      contains_symlink:                               %s", fs_reg->contains_symlink ? "yes" : "no");
	printf("\n  [-] Counters and Corresponding Current Limit Caps:");
	printf("\n      ");
	// count of entries encountered
	printf("\n       entry_n:              %u", fs_reg->entry_n);
	printf("\n       limit_at_entry_n:     %u", fs_reg->limit_at_entry_n);

	printf("\n       directory_n:          %u", fs_reg->directory_n);
	printf("\n       limit_at_directory_n: %u", fs_reg->limit_at_directory_n);

	printf("\n       file_n:               %u", fs_reg->file_n);
	printf("\n       limit_at_file_n:      %u", fs_reg->limit_at_file_n);

	printf("\n       sock_n:               %u", fs_reg->sock_n);
	printf("\n       limit_at_sock_n:      %u", fs_reg->limit_at_sock_n);

	printf("\n       chr_n:                %u", fs_reg->chr_n);
	printf("\n       limit_at_chr_n:       %u", fs_reg->limit_at_chr_n);

	printf("\n       blk_n:                %u", fs_reg->blk_n);
	printf("\n       limit_at_blk_n:       %u", fs_reg->limit_at_blk_n);

	printf("\n       symlink_n:            %u", fs_reg->symlink_n);
	printf("\n       limit_at_symlink_n:   %u", fs_reg->limit_at_symlink_n);

	printf("\n       fifo_n:               %u", fs_reg->fifo_n);
	printf("\n       limit_at_fifo_n:      %u", fs_reg->limit_at_fifo_n);




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Crawl Policies %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// display newline
	printf("\n");


	// display the symlink policy
	printf("\n      symlink_policy:                                 ");
	BP_FSRegDisplaySymlinkPolicy(fs_reg->crawl_policies.symlink_policy);
	printf("\n");

	// display the hidden policy
	printf("\n      hidden_policy:                                  ");
	BP_FSRegDisplayHiddenPolicy(fs_reg->crawl_policies.hidden_policy);
	printf("\n");

	// display the fifo policy
	printf("\n      fifo_policy:                                    ");
	BP_FSRegDisplayFifoPolicy(fs_reg->crawl_policies.fifo_policy);
	printf("\n");

	// display the directory policy
	printf("\n      directory_policy:                               ");
	BP_FSRegDisplayDirectoryPolicy(fs_reg->crawl_policies.directory_policy);
	printf("\n");

	// display the file policy
	printf("\n      file_policy:                                    ");
	BP_FSRegDisplayFilePolicy(fs_reg->crawl_policies.file_policy);
	printf("\n");

	// display the chr policy
	printf("\n     chr_policy:");
	BP_FSRegDisplayChrPolicy(fs_reg->crawl_policies.chr_policy);
	printf("\n");

	// display the blk policy
	printf("\n     blk_policy:");
	BP_FSRegDisplayBlkPolicy(fs_reg->crawl_policies.blk_policy);
	printf("\n");

	// display the sock policy
	printf("\n     sock_policy:");
	BP_FSRegDisplaySockPolicy(fs_reg->crawl_policies.sock_policy);
	printf("\n");



	// display the preg poilcy
	printf("\n      preg_policy:                                    \n");
	BP_FSRegDisplayPregPolicy(&fs_reg->crawl_policies);
	printf("\n");

	// set policy filter
	BP_FSREG_POLICIES policy_filter = {0};


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Filesystem %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// display the filesystem
	BP_FSRegDisplayFilesystem
	(
			fs_reg,
			BP_TRUE,
			BP_TRUE,
			BP_TRUE,
			(P_BP_FSREG_POLICIES) &policy_filter
	);


	// display registry
	// BP_HashRegDisplay(fs_reg->hreg, BP_FALSE, BP_TRUE);


	// leave when crawling is finished
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);


	// return indicating success
	return ERR_SUCCESS;

}

// Display a filesystem path in a registry.
BP_ERROR_T BP_FSRegDisplayPath(P_BP_FSREG fs_reg, char ** path)
{

	// return indicating success
	return ERR_SUCCESS;

}

// Display a filesystem path in a registry, recursing.
BP_ERROR_T BP_FSRegDisplayPath_r(P_BP_FSREG fs_reg, char ** path)
{

	// return indicating success
	return ERR_SUCCESS;

}

// ---- Display Policies -------------

BP_ERROR_T BP_FSRegDisplayPregPolicy(P_BP_FSREG_POLICIES policies)
{

	if(!policies)
		return ERR_FAILURE;

	// display filename policies if they're set
	if(policies->preg_fname_policies)
	{
		printf("\n\t filename preg match policies (%u total): \n", policies->preg_fname_policies_n);
		BP_DisplayStringArray("|__ expression: ", 1, 3, policies->preg_fname_policies);
	}

	// display pathonly policies if they're set
	if(policies->preg_pathonly_matching_policies)
	{
		printf("\n\t path-only preg match policies (%u total): \n", policies->preg_pathonly_matching_policies_n);
		BP_DisplayStringArray("|__ expression: ", 1, 3, policies->preg_pathonly_matching_policies);
	}

	// display fullpath policies if they're set
	if(policies->preg_fullpath_matching_policies)
	{
		printf("\n\t full path preg policies: (%u total): \n", policies->preg_fullpath_matching_policies_n);
		BP_DisplayStringArray("|__ expression: ", 1, 3, policies->preg_fullpath_matching_policies);
	}

	// return indicating success
	return ERR_SUCCESS;

}

// flags indicating the symlink policy
BP_ERROR_T BP_FSRegDisplaySymlinkPolicy(BP_FSREG_SYMLINK_POLICY symlink_policy)
{

	if(BP_FLAG_IS_SET(symlink_policy, BP_FS_SYMLINK_POLICY_FLAG_IGNORE_ALL))
		printf("\n\t\t BP_FS_SYMLINK_POLICY_FLAG_IGNORE_ALL ");

	if(BP_FLAG_IS_SET(symlink_policy, BP_FS_SYMLINK_POLICY_FLAG_ONLY))
			printf("\n\t\t BP_FS_SYMLINK_POLICY_FLAG_ONLY ");

	if(BP_FLAG_IS_SET(symlink_policy, BP_FS_SYMLINK_POLICY_FLAG_FOLLOW_DIRECTORIES))
		printf("\n\t\t BP_FS_SYMLINK_POLICY_FLAG_FOLLOW_DIRECTORIES ");

	// return indicating success
	return ERR_SUCCESS;

}

// flags indicating hidden file policy
BP_ERROR_T BP_FSRegDisplayHiddenPolicy(BP_FSREG_HIDDEN_POLICY hidden_policy)
{

	if(BP_FLAG_IS_SET(hidden_policy, BP_FS_HIDDEN_POLICY_FLAG_IGNORE_ALL))
		printf("\n\t\t BP_FS_HIDDEN_POLICY_FLAG_IGNORE_ALL ");

	if(BP_FLAG_IS_SET(hidden_policy, BP_FS_HIDDEN_POLICY_FLAG_ONLY))
		printf("\n\t\t BP_FS_HIDDEN_POLICY_FLAG_ONLY ");

	if(BP_FLAG_IS_SET(hidden_policy, BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_HIDDEN_DIRECTORIES))
		printf("\n\t\t BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_HIDDEN_DIRECTORIES ");

	if(BP_FLAG_IS_SET(hidden_policy, BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_HIDDEN_FILES))
		printf("\n\t\t BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_HIDDEN_FILES ");

	if(BP_FLAG_IS_SET(hidden_policy, BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_ONLY_HIDDEN_DIRECTORIES))
		printf("\n\t\t BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_ONLY_HIDDEN_DIRECTORIES ");

	if(BP_FLAG_IS_SET(hidden_policy, BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_ONLY_HIDDEN_FILES))
		printf("\n\t\t BP_FS_HIDDEN_POLICY_FLAG_EXAMINE_ONLY_HIDDEN_FILES ");

	// return indicating success
	return ERR_SUCCESS;

}

// flags indicating fifo policy
BP_ERROR_T BP_FSRegDisplayFifoPolicy(BP_FSREG_FIFO_POLICY fifo_policy)
{

	if(BP_FLAG_IS_SET(fifo_policy, BP_FS_FIFO_POLICY_FLAG_ONLY))
		printf("\n\t\t BP_FS_FIFO_POLICY_FLAG_ONLY ");

	if(BP_FLAG_IS_SET(fifo_policy, BP_FS_FIFO_POLICY_FLAG_IGNORE_ALL))
			printf("\n\t\t BP_FS_FIFO_POLICY_FLAG_IGNORE_ALL ");

	// return indicating success
	return ERR_SUCCESS;

}

// flags indicating the directory crawl policy
BP_ERROR_T BP_FSRegDisplayDirectoryPolicy(BP_FSREG_DIRECTORY_POLICY directory_policy)
{

	if(BP_FLAG_IS_SET(directory_policy, BP_FS_DIRECTORY_POLICY_FLAG_IGNORE_ALL))
		printf("\n\t\t BP_FS_DIRECTORY_POLICY_FLAG_IGNORE_ALL ");

	if(BP_FLAG_IS_SET(directory_policy, BP_FS_DIRECTORY_POLICY_FLAG_ONLY))
		printf("\n\t\t BP_FS_DIRECTORY_POLICY_FLAG_ONLY ");

	if(BP_FLAG_IS_SET(directory_policy, BP_FS_DIRECTORY_POLICY_FLAG_READABLE_BY_CURRENT_USER))
		printf("\n\t\t BP_FS_DIRECTORY_POLICY_FLAG_READABLE_BY_CURRENT_USER ");

	if(BP_FLAG_IS_SET(directory_policy, BP_FS_DIRECTORY_POLICY_FLAG_WRITABLE_BY_CURRENT_USER))
		printf("\n\t\t BP_FS_DIRECTORY_POLICY_FLAG_WRITABLE_BY_CURRENT_USER ");

	if(BP_FLAG_IS_SET(directory_policy, BP_FS_DIRECTORY_POLICY_FLAG_EXECUTABLE_BY_CURRENT_USER))
		printf("\n\t\t BP_FS_DIRECTORY_POLICY_FLAG_EXECUTABLE_BY_CURRENT_USER ");

	if(BP_FLAG_IS_SET(directory_policy, BP_FS_DIRECTORY_POLICY_FLAG_SUID))
		printf("\n\t\t BP_FS_DIRECTORY_POLICY_FLAG_SUID ");

	if(BP_FLAG_IS_SET(directory_policy, BP_FS_DIRECTORY_POLICY_FLAG_SGID))
		printf("\n\t\t BP_FS_DIRECTORY_POLICY_FLAG_SGID ");

	// return indicating success
	return ERR_SUCCESS;

}

// flags indicating file matching policy
BP_ERROR_T BP_FSRegDisplayFilePolicy(BP_FSREG_FILE_POLICY file_policy)
{

	if(BP_FLAG_IS_SET(file_policy, BP_FS_FILE_POLICY_FLAG_IGNORE_ALL))
		printf("\n\t\t BP_FS_FILE_POLICY_FLAG_IGNORE_ALL ");

	if(BP_FLAG_IS_SET(file_policy, BP_FS_FILE_POLICY_FLAG_ONLY))
		printf("\n\t\t BP_FS_FILE_POLICY_FLAG_ONLY ");

	if(BP_FLAG_IS_SET(file_policy, BP_FS_FILE_POLICY_FLAG_OWNED_BY_CURRENT_USER))
		printf("\n\t\t BP_FS_FILE_POLICY_FLAG_OWNED_BY_CURRENT_USER ");

	if(BP_FLAG_IS_SET(file_policy, BP_FS_FILE_POLICY_FLAG_NOT_OWNED_BY_CURRENT_USER))
		printf("\n\t\t BP_FS_FILE_POLICY_FLAG_NOT_OWNED_BY_CURRENT_USER ");

	if(BP_FLAG_IS_SET(file_policy, BP_FS_FILE_POLICY_FLAG_READABLE_BY_CURRENT_USER))
		printf("\n\t\t BP_FS_FILE_POLICY_FLAG_READABLE_BY_CURRENT_USER ");

	if(BP_FLAG_IS_SET(file_policy, BP_FS_FILE_POLICY_FLAG_WRITABLE_BY_CURRENT_USER))
		printf("\n\t\t BP_FS_FILE_POLICY_FLAG_WRITABLE_BY_CURRENT_USER ");

	if(BP_FLAG_IS_SET(file_policy, BP_FS_FILE_POLICY_FLAG_EXECUTABLE_BY_CURRENT_USER))
		printf("\n\t\t BP_FS_FILE_POLICY_FLAG_EXECUTABLE_BY_CURRENT_USER ");

	if(BP_FLAG_IS_SET(file_policy, BP_FS_FILE_POLICY_FLAG_SUID))
		printf("\n\t\t BP_FS_FILE_POLICY_FLAG_SUID ");

	if(BP_FLAG_IS_SET(file_policy, BP_FS_FILE_POLICY_FLAG_SGID))
		printf("\n\t\t BP_FS_FILE_POLICY_FLAG_SGID ");

	if(BP_FLAG_IS_SET(file_policy, BP_FS_FILE_POLICY_FLAG_OWNER_HAS_HIGHER_UID_THAN_CURRENT))
		printf("\n\t\t BP_FS_FILE_POLICY_FLAG_OWNER_HAS_HIGHER_UID_THAN_CURRENT ");

	if(BP_FLAG_IS_SET(file_policy, BP_FS_FILE_POLICY_FLAG_OWNER_HAS_LOWER_UID_THAN_CURRENT))
		printf("\n\t\t BP_FS_FILE_POLICY_FLAG_OWNER_HAS_LOWER_UID_THAN_CURRENT ");

	// return indicating success
	return ERR_SUCCESS;

}


// flags indicating chr matching policy
BP_ERROR_T BP_FSRegDisplayChrPolicy(BP_FSREG_CHR_POLICY chr_policy)
{

	if(BP_FLAG_IS_SET(chr_policy, BP_FS_CHR_POLICY_FLAG_IGNORE_ALL))
		printf("\n\t\t BP_FS_CHR_POLICY_FLAG_IGNORE_ALL ");

	if(BP_FLAG_IS_SET(chr_policy, BP_FS_CHR_POLICY_FLAG_ONLY))
		printf("\n\t\t BP_FS_CHR_POLICY_FLAG_ONLY ");

	// return indicating success
	return ERR_SUCCESS;

}



// flags indicating blk matching policy
BP_ERROR_T BP_FSRegDisplayBlkPolicy(BP_FSREG_CHR_POLICY blk_policy)
{

	if(BP_FLAG_IS_SET(blk_policy, BP_FS_BLK_POLICY_FLAG_IGNORE_ALL))
		printf("\n\t\t BP_FS_BLK_POLICY_FLAG_IGNORE_ALL ");

	if(BP_FLAG_IS_SET(blk_policy, BP_FS_BLK_POLICY_FLAG_ONLY))
		printf("\n\t\t BP_FS_BLK_POLICY_FLAG_ONLY ");

	// return indicating success
	return ERR_SUCCESS;

}



// flags indicating sock matching policy
BP_ERROR_T BP_FSRegDisplaySockPolicy(BP_FSREG_SOCK_POLICY sock_policy)
{

	if(BP_FLAG_IS_SET(sock_policy, BP_FS_SOCK_POLICY_FLAG_IGNORE_ALL))
		printf("\n\t\t BP_FS_SOCK_POLICY_FLAG_IGNORE_ALL ");

	if(BP_FLAG_IS_SET(sock_policy, BP_FS_SOCK_POLICY_FLAG_ONLY))
		printf("\n\t\t BP_FS_SOCK_POLICY_FLAG_ONLY ");

	// return indicating success
	return ERR_SUCCESS;

}

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
)
{

	if(!fs_reg)
		return ERR_FAILURE;
	if(!fs_reg->hreg)
		return ERR_FAILURE;

	// enter critical section before crawling
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// temporary iterator values used during the HASH_ITER loop below
	P_BP_HASH_TABLE_KEYED_ENTRY entry = NULL;
	P_BP_HASH_TABLE_KEYED_ENTRY tmp   = NULL;

	// increase the display tab depth in case of recursion (keeps
	// things lined up)
	size_t display_tab_depth = 0;

	// iterate through all hashes and display
	HASH_ITER(hh, fs_reg->hreg->hive, entry, tmp)
	{

		if(entry->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{
			printf("\n [+] Displaying Filesystem Entries for FS Registry %p: ", fs_reg);
			BP_FSRegDisplayFilesystemDirectory
			(
				fs_reg,
				entry,
				recurse,
				0
			);

		}

	}



	// leave when crawling is finished
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating success
	return ERR_SUCCESS;
}


// display directory entry (recurses)
BP_ERROR_T BP_FSRegDisplayFilesystemDirectory
(
		P_BP_FSREG                  fs_reg,
		P_BP_HASH_TABLE_KEYED_ENTRY dir,
		BP_BOOL                     recurse,
		size_t                      tab_depth

)
{


	// temporary iterator values used during the HASH_ITER loop below
	P_BP_HASH_TABLE_KEYED_ENTRY entry = NULL;
	P_BP_HASH_TABLE_KEYED_ENTRY tmp   = NULL;

	// increase the display tab depth in case of recursion (keeps
	// things lined up)
	size_t display_tab_depth = tab_depth;
	display_tab_depth++;

	// display tab counter
	size_t display_tab_n = 0;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display the Directory Header %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	printf("\n");
	for
	(
		display_tab_n = 0;
		display_tab_n < display_tab_depth;
		display_tab_n++
	)
	{
		printf("    |");
	}

	printf("[DIR] %s", dir->key);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Recurse if Necessary %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	P_BP_HASH_TABLE_KEYED_ENTRY dir_recurse = (P_BP_HASH_TABLE_KEYED_ENTRY) dir->data;

	// iterate through all hashes and display
	HASH_ITER(hh, dir_recurse, entry, tmp)
	{

		if(entry->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{


			BP_FSRegDisplayFilesystemDirectory
			(
				fs_reg,
				entry,
				recurse,
				display_tab_depth
			);

		}
		else
		{

			BP_FSRegDisplayFilesystemEntry
			(
				fs_reg,
				entry,
				BP_TRUE,
				BP_TRUE,
				BP_TRUE,
				display_tab_depth
			);

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

BP_ERROR_T BP_FSRegDisplayFilesystemEntry
(
		P_BP_FSREG                  fs_reg,
		P_BP_HASH_TABLE_KEYED_ENTRY file_entry,
		BP_BOOL                     display_dirent,
		BP_BOOL                     display_stat,
		BP_BOOL                     display_path,
		size_t                      tab_depth
)
{

	// set fsreg entry
	P_BP_FSREG_ENTRY fsreg_entry = (P_BP_FSREG_ENTRY) file_entry->data;
	if(!fsreg_entry)
		return ERR_FAILURE;

	if(fsreg_entry->type == BP_FSREG_ENTRY_TYPE_DIR)
		return ERR_SUCCESS;

	// increase the display tab depth in case of recursion (keeps
	// things lined up)
	size_t display_tab_depth = tab_depth;

	// display tab counter
	size_t display_tab_n = 0;

	printf("\n");
	for
	(
		display_tab_n = 0;
		display_tab_n < display_tab_depth;
		display_tab_n++
	)
	{
		printf("    |");
	}

	printf("__ [");



	switch(fsreg_entry->type)
	{


		case BP_FSREG_ENTRY_TYPE_UNSET:
			printf("unset");
			break;

		case BP_FSREG_ENTRY_TYPE_DIR:
			printf("dir");
			break;

		case BP_FSREG_ENTRY_TYPE_CHR:
			printf("chr");
			break;

		case BP_FSREG_ENTRY_TYPE_BLK:
			printf("blk");
			break;

		case BP_FSREG_ENTRY_TYPE_REG:
			printf("reg");
			break;

		case BP_FSREG_ENTRY_TYPE_FIFO:
			printf("fifo");
			break;

		case BP_FSREG_ENTRY_TYPE_LNK:
			printf("lnk");
			break;

		case BP_FSREG_ENTRY_TYPE_SOCK:
			printf("sock");
			break;

		default:
			printf("badval");
			break;
	}

	printf("] \"%s\"", file_entry->key);

	// create the tab buffer
	char * tab_buff = NULL;
	for
	(
		display_tab_n = 0;
		display_tab_n < (display_tab_depth+1);
		display_tab_n++
	)
	{
		if(!tab_buff)
		{
			tab_buff = bpstrdup("    |");
		}
		else
		{
			tab_buff = BP_JoinStrings(tab_buff, "    |");
		}

	}

	if(display_path)
	{
		printf(" (%s)", fsreg_entry->full_path);
	}

	// if the user specified that they want to display the contents
	// of dirent, go ahead and do it here.
	if(display_dirent)
	{

		// display dirent structure
		printf("\n%s__ dirent:", tab_buff);
		printf("\n%s   {", tab_buff);
		printf("\n%s        d_name:   %s", tab_buff, (char *) &fsreg_entry->dirent_data.d_name);
		printf("\n%s        d_ino:    %u", tab_buff, fsreg_entry->dirent_data.d_ino);
		printf("\n%s        d_off:    %u", tab_buff, fsreg_entry->dirent_data.d_off);
		printf("\n%s        d_reclen: %u", tab_buff, fsreg_entry->dirent_data.d_reclen);
		printf("\n%s        d_type:   %u", tab_buff, fsreg_entry->dirent_data.d_type);
		printf("\n%s   }", tab_buff);

	}

	// display stat structure
	if(display_stat)
	{

		// create empty stack buffer for holding time
		char buff[60] = {0};



		// display dirent structure
		printf("\n%s__ lstat:", tab_buff);
		printf("\n%s   {", tab_buff);
		printf("\n%s       st_dev:        %u",  tab_buff, fsreg_entry->lstat_data.st_dev);
		printf("\n%s       st_ino:        %u",  tab_buff, fsreg_entry->lstat_data.st_ino);

		// attempt to get mode string for all types
		char * filetype_mode_str     = BP_FStatGetFiletypeModeString          (&fsreg_entry->lstat_data);
		char * owner_perm_mode_str   = BP_FStatGetOwnerPermissionsModeString  (&fsreg_entry->lstat_data);
		char * group_perm_mode_str   = BP_FStatGetGroupPermissionsModeString  (&fsreg_entry->lstat_data);
		char * others_perm_mode_str  = BP_FStatGetOthersPermissionsModeString (&fsreg_entry->lstat_data);
		char * special_perm_mode_str = BP_FStatGetSpecialPermissionsModeString(&fsreg_entry->lstat_data);

		// display the mode string
		printf
		(
			"\n%s       st_mode:       %u ( type:%s %s %s %s %s)",
			tab_buff,
			fsreg_entry->lstat_data.st_mode,
			filetype_mode_str,
			owner_perm_mode_str   ? owner_perm_mode_str : "",
			group_perm_mode_str   ? group_perm_mode_str : "",
			others_perm_mode_str  ? others_perm_mode_str : "",
			special_perm_mode_str ? special_perm_mode_str : ""
		);

		// destroy permission strings if any are set
		if(filetype_mode_str)
			bpfree(filetype_mode_str);
		if(owner_perm_mode_str)
			bpfree(owner_perm_mode_str);
		if(group_perm_mode_str)
			bpfree(group_perm_mode_str);
		if(others_perm_mode_str)
			bpfree(others_perm_mode_str);
		if(special_perm_mode_str)
			bpfree(special_perm_mode_str);

		// display link
		printf("\n%s       st_nlink:      %u",  tab_buff, fsreg_entry->lstat_data.st_nlink);

		// gather and display username string
		char * username_string = BP_LinuxUserGetUsernameFromUid(fsreg_entry->lstat_data.st_uid);
		printf("\n%s       st_uid:        %u (%s)",  tab_buff, fsreg_entry->lstat_data.st_uid, username_string);
		if(username_string)
			bpfree(username_string);

		// gather and display groupname string
		char * groupname_string = BP_LinuxUserGetGroupnameFromGid(fsreg_entry->lstat_data.st_gid);
		printf("\n%s       st_gid:        %u (%s)",  tab_buff, fsreg_entry->lstat_data.st_gid, groupname_string);
		if(groupname_string)
			bpfree(groupname_string);

		printf("\n%s       st_rdev:       %u",  tab_buff, fsreg_entry->lstat_data.st_rdev);
		printf("\n%s       st_size:       %u",  tab_buff, fsreg_entry->lstat_data.st_size);
		printf("\n%s       st_blksize:    %u",  tab_buff, fsreg_entry->lstat_data.st_blksize);
		printf("\n%s       st_blocks:     %u",  tab_buff, fsreg_entry->lstat_data.st_blocks);

		// create access time string
		strftime(buff, 40, "%Y-%m-%d %H:%M:%S", localtime(&fsreg_entry->lstat_data.st_atime));
		printf("\n%s       st_atime:      %s (%u) ", tab_buff, (char *) &buff, fsreg_entry->lstat_data.st_atime);
		BP_ZERO_STACK(buff);

		strftime(buff, 40, "%Y-%m-%d %H:%M:%S", localtime(&fsreg_entry->lstat_data.st_mtime));
		printf("\n%s       st_mtime:      %s (%u) ", tab_buff, (char *) &buff, fsreg_entry->lstat_data.st_mtime);
		BP_ZERO_STACK(buff);

		strftime(buff, 40, "%Y-%m-%d %H:%M:%S", localtime(&fsreg_entry->lstat_data.st_ctime));
		printf("\n%s       st_ctime:      %s (%u) ", tab_buff, (char *) &buff, fsreg_entry->lstat_data.st_ctime);
		BP_ZERO_STACK(buff);
		printf("\n%s   }", tab_buff);

		if(fsreg_entry->dirent_data.d_type == DT_LNK)
		{

			// display dirent structure
			printf("\n%s__ stat: (displaying since this file is a symlink)", tab_buff);
			printf("\n%s   {", tab_buff);
			printf("\n%s       st_dev:        %u",  tab_buff, fsreg_entry->stat_data.st_dev);
			printf("\n%s       st_ino:        %u",  tab_buff, fsreg_entry->stat_data.st_ino);

			// attempt to get mode string for all types
			char * filetype_mode_str     = BP_FStatGetFiletypeModeString          (&fsreg_entry->stat_data);
			char * owner_perm_mode_str   = BP_FStatGetOwnerPermissionsModeString  (&fsreg_entry->stat_data);
			char * group_perm_mode_str   = BP_FStatGetGroupPermissionsModeString  (&fsreg_entry->stat_data);
			char * others_perm_mode_str  = BP_FStatGetOthersPermissionsModeString (&fsreg_entry->stat_data);
			char * special_perm_mode_str = BP_FStatGetSpecialPermissionsModeString(&fsreg_entry->stat_data);

			// display the mode string
			printf
			(
				"\n%s       st_mode:       %u ( type:%s %s %s %s %s)",
				tab_buff,
				fsreg_entry->stat_data.st_mode,
				filetype_mode_str,
				owner_perm_mode_str   ? owner_perm_mode_str : "",
				group_perm_mode_str   ? group_perm_mode_str : "",
				others_perm_mode_str  ? others_perm_mode_str : "",
				special_perm_mode_str ? special_perm_mode_str : ""
			);

			// destroy permission strings if any are set
			if(filetype_mode_str)
				bpfree(filetype_mode_str);
			if(owner_perm_mode_str)
				bpfree(owner_perm_mode_str);
			if(group_perm_mode_str)
				bpfree(group_perm_mode_str);
			if(others_perm_mode_str)
				bpfree(others_perm_mode_str);
			if(special_perm_mode_str)
				bpfree(special_perm_mode_str);

			// display link
			printf("\n%s       st_nlink:      %u",  tab_buff, fsreg_entry->stat_data.st_nlink);

			// gather and display username string
			char * username_string = BP_LinuxUserGetUsernameFromUid(fsreg_entry->stat_data.st_uid);
			printf("\n%s       st_uid:        %u (%s)",  tab_buff, fsreg_entry->stat_data.st_uid, username_string);
			if(username_string)
				bpfree(username_string);

			// gather and display groupname string
			char * groupname_string = BP_LinuxUserGetGroupnameFromGid(fsreg_entry->stat_data.st_gid);
			printf("\n%s       st_gid:        %u (%s)",  tab_buff, fsreg_entry->stat_data.st_gid, groupname_string);
			if(groupname_string)
				bpfree(groupname_string);

			printf("\n%s       st_rdev:       %u",  tab_buff, fsreg_entry->stat_data.st_rdev);
			printf("\n%s       st_size:       %u",  tab_buff, fsreg_entry->stat_data.st_size);
			printf("\n%s       st_blksize:    %u",  tab_buff, fsreg_entry->stat_data.st_blksize);
			printf("\n%s       st_blocks:     %u",  tab_buff, fsreg_entry->stat_data.st_blocks);

			// create access time string
			strftime(buff, 40, "%Y-%m-%d %H:%M:%S", localtime(&fsreg_entry->stat_data.st_atime));
			printf("\n%s       st_atime:      %s (%u) ", tab_buff, (char *) &buff, fsreg_entry->stat_data.st_atime);
			BP_ZERO_STACK(buff);

			strftime(buff, 40, "%Y-%m-%d %H:%M:%S", localtime(&fsreg_entry->lstat_data.st_mtime));
			printf("\n%s       st_mtime:      %s (%u) ", tab_buff, (char *) &buff, fsreg_entry->stat_data.st_mtime);
			BP_ZERO_STACK(buff);

			strftime(buff, 40, "%Y-%m-%d %H:%M:%S", localtime(&fsreg_entry->lstat_data.st_ctime));
			printf("\n%s       st_ctime:      %s (%u) ", tab_buff, (char *) &buff, fsreg_entry->stat_data.st_ctime);
			BP_ZERO_STACK(buff);
			printf("\n%s   }", tab_buff);
		}

	}

	// display the policy analysis
	BP_FSRegDisplayFilePolicyAnalysis(&fsreg_entry->policy_analysis, BP_TRUE, tab_buff);


	// display trailing space if we displayed stat or dirent
	if(display_stat || display_dirent)
		printf("\n%s ", tab_buff);


	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Display Files %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// this will display all items in a filesystem.  A policy filter can be applied to selection.
BP_ERROR_T BP_FSRegDisplayFiles
(
		P_BP_FSREG fs_reg,
		BP_BOOL display_files,
		BP_BOOL display_directories,
		BP_BOOL recurse,
		P_BP_FSREG_POLICIES policy_filter
)
{

	if(!fs_reg)
		return ERR_FAILURE;
	if(!fs_reg->hreg)
		return ERR_FAILURE;

	// enter critical section before crawling
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// temporary iterator values used during the HASH_ITER loop below
	P_BP_HASH_TABLE_KEYED_ENTRY entry = NULL;
	P_BP_HASH_TABLE_KEYED_ENTRY tmp   = NULL;

	// increase the display tab depth in case of recursion (keeps
	// things lined up)
	size_t display_tab_depth = 0;

	// iterate through all hashes and display
	HASH_ITER(hh, fs_reg->hreg->hive, entry, tmp)
	{

		if(entry->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{
			BP_FSRegDisplayFilesDirectory
			(
				fs_reg,
				entry,
				recurse,
				0
			);

		}

	}



	// leave when crawling is finished
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return indicating success
	return ERR_SUCCESS;
}


// display directory entry (recurses)
BP_ERROR_T BP_FSRegDisplayFilesDirectory
(
		P_BP_FSREG                  fs_reg,
		P_BP_HASH_TABLE_KEYED_ENTRY dir,
		BP_BOOL                     recurse,
		size_t                      tab_depth

)
{


	// temporary iterator values used during the HASH_ITER loop below
	P_BP_HASH_TABLE_KEYED_ENTRY entry = NULL;
	P_BP_HASH_TABLE_KEYED_ENTRY tmp   = NULL;

	// increase the display tab depth in case of recursion (keeps
	// things lined up)
	size_t display_tab_depth = tab_depth;
	display_tab_depth++;

	// display tab counter
	size_t display_tab_n = 0;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display the Directory Header %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// printf("[DIR] %s", dir->key);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Recurse if Necessary %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	P_BP_HASH_TABLE_KEYED_ENTRY dir_recurse = (P_BP_HASH_TABLE_KEYED_ENTRY) dir->data;

	// iterate through all hashes and display
	HASH_ITER(hh, dir_recurse, entry, tmp)
	{

		if(entry->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{


			BP_FSRegDisplayFilesDirectory
			(
				fs_reg,
				entry,
				recurse,
				display_tab_depth
			);

		}
		else
		{

			BP_FSRegDisplayFilesEntry
			(
				fs_reg,
				entry,
				BP_TRUE,
				BP_TRUE,
				BP_TRUE,
				display_tab_depth
			);

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

BP_ERROR_T BP_FSRegDisplayFilesEntry
(
		P_BP_FSREG                  fs_reg,
		P_BP_HASH_TABLE_KEYED_ENTRY file_entry,
		BP_BOOL                     display_dirent,
		BP_BOOL                     display_stat,
		BP_BOOL                     display_path,
		size_t                      tab_depth
)
{

	// set fsreg entry
	P_BP_FSREG_ENTRY fsreg_entry = (P_BP_FSREG_ENTRY) file_entry->data;
	if(!fsreg_entry)
		return ERR_FAILURE;

	if(fsreg_entry->type == BP_FSREG_ENTRY_TYPE_DIR)
		return ERR_SUCCESS;


	printf("\n%s", fsreg_entry->full_path);


	// return indicating success
	return ERR_SUCCESS;

}



// ----- Policy Analysis ------------------

BP_ERROR_T BP_FSRegDisplayFilePolicyAnalysis
(
	P_BP_FSREG_FILE_POLICY_ANALYSIS policy_analysis,
	BP_BOOL                         only_display_matched,
	char *                          optional_front_buffer
)
{

	// ensure we have a policy analysis
	if(!policy_analysis)
		return ERR_FAILURE;

	printf("\n%s__policy_analysis",optional_front_buffer ? optional_front_buffer : "");
	printf("\n%s   {", optional_front_buffer);

	// run inline file type displays
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, is_symlink,                                                   "is_symlink","                                                      ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, is_hidden,                                                    "is_hidden","                                                       ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, is_fifo,                                                      "is_fifo","                                                         ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, is_directory,                                                 "is_directory","                                                    ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, is_file,                                                      "is_file","                                                         ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, is_chr,                                                       "is_chr","                                                          ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, is_blk,                                                       "is_blk","                                                          ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, is_sock,                                                      "is_sock","                                                         ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, symlink_ignore_all_match,                                     "symlink_ignore_all_match","                                        ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, symlink_only_match,                                           "symlink_only_match","                                              ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, symlink_follow_directories_match,                             "symlink_follow_directories_match","                                ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, hidden_only_match,                                            "hidden_only_match","                                               ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, hidden_only_within_hidden_parent_match,                       "hidden_only_within_hidden_parent_match","                          ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, hidden_ignore_all_match,                                      "hidden_ignore_all_match","                                         ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, hidden_examine_directories_match,                             "hidden_examine_directories_match","                                ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, hidden_examine_files_match,                                   "hidden_examine_files_match","                                      ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, hidden_examine_only_directories_match,                        "hidden_examine_only_directories_match","                           ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, hidden_examine_only_files_match,                              "hidden_examine_only_files_match","                                 ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, fifo_only_match,                                              "fifo_only_match","                                                 ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, fifo_ignore_all_match,                                        "fifo_ignore_all_match","                                           ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, directory_only_match,                                         "directory_only_match","                                            ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, directory_ignore_all_match,                                   "directory_ignore_all_match","                                      ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, directory_readable_by_current_user_match,                     "directory_readable_by_current_user_match","                        ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, directory_writable_by_current_user_match,                     "directory_writable_by_current_user_match","                        ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, directory_executable_by_current_user_match,                   "directory_executable_by_current_user_match","                      ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, directory_executable_by_current_user_match,                   "directory_executable_by_current_user_match","                      ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, directory_suid_bit_match,                                     "directory_suid_bit_match","                                        ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, file_only_match,                                              "file_only_match","                                                 ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, file_ignore_all_match,                                        "file_ignore_all_match","                                           ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, file_owned_by_current_user,                                   "file_owned_by_current_user","                                      ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, file_not_owned_by_current_user,                               "file_not_owned_by_current_user","                                  ");

	// display readable match flags
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, file_readable_by_current_user_match,                          "file_readable_by_current_user_match","                             ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, file_readable_by_current_user_because_user_is_root_match,     "file_readable_by_current_user_because_user_is_root_match","        ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, file_readable_by_current_user_because_user_is_owner_match,    "file_readable_by_current_user_because_user_is_owner_match","       ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, file_readable_by_current_user_because_group_match,            "file_readable_by_current_user_because_group_match","               ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, file_readable_by_current_user_because_world_match,            "file_readable_by_current_user_because_world_match","               ");

	// display writable match flags
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, file_writable_by_current_user_match,                          "file_writable_by_current_user_match","                             ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, file_writable_by_current_user_because_user_is_root_match,     "file_writable_by_current_user_because_user_is_root_match","        ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, file_writable_by_current_user_because_user_is_owner_match,    "file_writable_by_current_user_because_user_is_owner_match","       ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, file_writable_by_current_user_because_group_match,            "file_writable_by_current_user_because_group_match","               ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, file_writable_by_current_user_because_world_match,            "file_writable_by_current_user_because_world_match","               ");


	// display executable match flags
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, file_executable_by_current_user_match,                        "file_executable_by_current_user_match","                           ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, file_executable_by_current_user_because_user_is_root_match,   "file_executable_by_current_user_because_user_is_root_match","      ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, file_executable_by_current_user_because_user_is_owner_match,  "file_executable_by_current_user_because_user_is_owner_match","     ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, file_executable_by_current_user_because_group_match,          "file_executable_by_current_user_because_group_match","             ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, file_executable_by_current_user_because_world_match,          "file_executable_by_current_user_because_world_match","             ");




	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, file_suid_bit_match,                                          "file_suid_bit_match","                                             ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, file_sgid_bit_match,                                          "file_sgid_bit_match","                                             ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, chr_only_match,                                               "chr_only_match","                                                  ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, chr_ignore_all_match,                                         "chr_ignore_allmatch","                                             ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, blk_only_match,                                               "blk_only_match","                                                  ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, blk_ignore_all_match,                                         "blk_ignore_all_match","                                            ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, fname_preg_policies_match,                                    "fname_preg_policies_match","                                       ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, pathonly_preg_policies_match,                                 "pathonly_preg_policies_match","                                    ");
	BP_FSREG_DISPLAY_FILE_POLICY_ANALYSIS_INLINE_CHECK(optional_front_buffer, fullpath_preg_policies_match,                                 "fullpath_preg_policies_match","                                    ");

	printf("\n%s   }", optional_front_buffer);


	// return indicating success
	return ERR_SUCCESS;

}


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
)
{


	if(!fs_reg)
		return NULL;
	if(!fs_reg->hreg)
		return NULL;

	// enter critical section before crawling
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(fs_reg->hreg);

	// temporary iterator values used during the HASH_ITER loop below
	P_BP_HASH_TABLE_KEYED_ENTRY entry = NULL;
	P_BP_HASH_TABLE_KEYED_ENTRY tmp   = NULL;

	// increase the display tab depth in case of recursion (keeps
	// things lined up)
	size_t display_tab_depth = 0;


	// entries here
	char ** entries_in_out = NULL;
	size_t  entries_n      = 0;

	// iterate through all hashes and display
	HASH_ITER(hh, fs_reg->hreg->hive, entry, tmp)
	{

		if(entry->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{

			BP_FSRegRetrieveEntryFullPathsAsBPCallocedNullTerminatedPointerArray__dir_r
			(
				fs_reg,
				entry,
				optional_policy_analysis_filter,
				(char ***) &entries_in_out,
				&entries_n
			);

		}

	}
	printf("\n Got entries damnit?: \n");
	BP_DisplayStringArray("---", 1, 1, entries_in_out);

	// leave when crawling is finished
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(fs_reg->hreg);

	// return entries
	return entries_in_out;

}


// this is an internal function, do not use this
// Simple internal routine to parse individual entries (needed for recursion)
BP_ERROR_T BP_FSRegRetrieveEntryFullPathsAsBPCallocedNullTerminatedPointerArray__dir_r
(
		P_BP_FSREG                      fs_reg,
		P_BP_HASH_TABLE_KEYED_ENTRY     dir,
		P_BP_FSREG_FILE_POLICY_ANALYSIS optional_policy_analysis_filter,
		char ***                        entries_in_out,
		size_t *                        entries_n
)
{

	// temporary iterator values used during the HASH_ITER loop below
	P_BP_HASH_TABLE_KEYED_ENTRY entry = NULL;
	P_BP_HASH_TABLE_KEYED_ENTRY tmp   = NULL;


	// display tab counter
	size_t display_tab_n = 0;

	// recursion pointer
	P_BP_HASH_TABLE_KEYED_ENTRY dir_recurse = (P_BP_HASH_TABLE_KEYED_ENTRY) dir->data;

	// iterate through all hashes and display
	HASH_ITER(hh, dir_recurse, entry, tmp)
	{

		if(entry->type == BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
		{

			BP_FSRegRetrieveEntryFullPathsAsBPCallocedNullTerminatedPointerArray__dir_r
			(
					fs_reg,
					entry,
					optional_policy_analysis_filter,
					entries_in_out,
					entries_n
			);

		}
		else
		{

			printf("\n before: %u", *entries_n);
			BP_FSRegRetrieveEntryFullPathsAsBPCallocedNullTerminatedPointerArray__entry_r
			(
					fs_reg,
					entry,
					optional_policy_analysis_filter,
					entries_in_out,
					entries_n
			);
			printf("\n after: %u", *entries_n);
		}

	}

	// return indicating success
	return ERR_SUCCESS;

}


// this is an internal function, do not use this
// Simple internal routine to parse individual entries (needed for recursion)
BP_ERROR_T BP_FSRegRetrieveEntryFullPathsAsBPCallocedNullTerminatedPointerArray__entry_r
(
		P_BP_FSREG                      fs_reg,
		P_BP_HASH_TABLE_KEYED_ENTRY     file_entry,
		P_BP_FSREG_FILE_POLICY_ANALYSIS optional_policy_analysis_filter,
		char ***                        entries_in_out,
		size_t *                        entries_n
)
{


	// set fsreg entry
	P_BP_FSREG_ENTRY fsreg_entry = (P_BP_FSREG_ENTRY) file_entry->data;
	if(!fsreg_entry)
		return ERR_FAILURE;

	// return immediately if the type is dir
	if(fsreg_entry->type == BP_FSREG_ENTRY_TYPE_DIR)
		return ERR_SUCCESS;

	// attempt to lookup the filesystem registry
	char * entry = BP_FSRegLookupFilesystemPathFromEntry
	(
			fs_reg,
			file_entry
	);

	// ensure we have an entry
	if(!entry)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Alloc Entry and Store %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// allocate space for entry
	entries_in_out[0] = (char **) bprealloc((char *) entries_in_out[0], sizeof(char *) * (*entries_n + 2));
	if(!entries_in_out)
		return ERR_FAILURE;

	// duplicate the entry and increment counter
	entries_in_out[0][*entries_n]   = bpstrdup(entry);
	entries_in_out[0][*entries_n+1] = NULL;

	// printf("\n son of a gun: %u - %s", *entries_n, entries_in_out[0][*entries_n]);
	*entries_n = *entries_n + 1;

	// destroy the entry
	if(entry)
		bpfree(entry);

	// return indicating success
	return ERR_SUCCESS;

}

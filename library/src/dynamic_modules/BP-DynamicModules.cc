// include blockparty main header
#include "../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% GHT Module Hooks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// All loaded modules are kept referenced in the GHT.  These routines allow us to modify
// the GHT to hold loaded modules.

// Checks to see if a dynamic module is already loaded in the GHT.
BP_BOOL BP_DynamicModuleIsAlreadyLoadedInGHT(P_BP_DYNAMIC_MODULE module)
{

	if(!module)
		return BP_FALSE;

	// ensure module name passes registry index filter test
	if(!BP_HashRegPathArrayIndexFilterTest(module->so_module_name, bpstrlen(module->so_module_name)))
	{
		module->load_check_failed = BP_TRUE;
		return BP_FALSE;
	}

	// attempt to lookup the module by name within the ght
	P_BP_HTKE mod_lookup = BP_HashRegLookupTableEntryByDataKeyIndexAndTablePathQuery
	(
		(P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry,
		"/system/dynamic_modules",
		module->so_module_name
	);

	// if the lookup succeeded, return true immediately
	if(mod_lookup)
	{
		module->already_loaded                     = BP_TRUE;
		module->already_loaded_matched_module_name = BP_TRUE;
		return BP_TRUE;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check By SHA1() %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// attempt modules table lookup
	P_BP_HTKE modules_table_lookup = BP_HashRegLookupTableByPathQuery((P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry, "/system/dynamic_modules");


	// walk through each entry
	BP_HASH_ITER_OPEN(modules_table_lookup, table_entry_iter)
	{

		// ensure we have a keyed entry
		if(!table_entry_iter)
			break;
		if(!table_entry_iter->key)
			break;


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Attempt SHA1 Lookup %%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// allocate table lookup path
		char * table_entry_lookup[] =
		{
			"system",
			"dynamic_modules",
			table_entry_iter->key,
			"so_module_filesystem_sha1_hash",
			NULL
		};

		// attempt to lookup the sha1 hash for this module
		P_BP_HTKE so_module_filesystem_sha1_hash_lookup = BP_HashRegLookupTableEntryByPath((P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry, (char **) &table_entry_lookup);
		if(!so_module_filesystem_sha1_hash_lookup)
		{
			continue;
		}

		// continue the loop if the type is mismatched
		if(so_module_filesystem_sha1_hash_lookup->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING)
			continue;

		// ensure the data size is at least 40 bytes (sha1 length)
		if(so_module_filesystem_sha1_hash_lookup->data_size < 40)
			continue;

		// attempt memory compare
		if(ht_memmatch_entry(so_module_filesystem_sha1_hash_lookup, module->so_module_filesystem_sha1_hash, bpstrlen(module->so_module_filesystem_sha1_hash)))
		{

			// set the already loaded flag to true
			module->already_loaded = BP_TRUE;

			// set the matched sha1
			module->already_loaded_matched_bin_sha1 = BP_TRUE;

		}


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Attempt Filesystem Path Lookup %%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// allocate table lookup path
		char * fs_table_entry_lookup[] =
		{
			"system",
			"dynamic_modules",
			table_entry_iter->key,
			"so_module_load_path",
			NULL
		};

		// attempt to lookup the sha1 hash for this module
		P_BP_HTKE so_module_filesystem_load_path_hash_lookup = BP_HashRegLookupTableEntryByPath((P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry, (char **) &table_entry_lookup);
		if(!so_module_filesystem_sha1_hash_lookup)
		{
			continue;
		}

		// continue the loop if the type is mismatched
		if(so_module_filesystem_load_path_hash_lookup->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING)
			continue;

		// ensure the data size is at least 40 bytes (sha1 length)
		if(so_module_filesystem_load_path_hash_lookup->data_size < 40)
			continue;

		// attempt memory compare
		if(ht_memmatch_entry(so_module_filesystem_load_path_hash_lookup, module->so_module_load_path, bpstrlen(module->so_module_load_path)))
		{

			// set the already loaded flag to true
			module->already_loaded = BP_TRUE;

			// set the matched sha1
			module->already_loaded_matched_module_filesystem_path = BP_TRUE;

		}

		// if we detected that or module is already loaded, exit here
		if(module->already_loaded == BP_TRUE)
			break;


	}
	BP_HASH_ITER_CLOSE;

	// if the module is already loaded, return indicating so
	if(module->already_loaded == BP_TRUE)
		return BP_TRUE;

	// return unfound by default
	return BP_FALSE;

}

// Attempt to add a module into the "/dynamic_modules" section of the GHT.  Will return false
// if the module is already loaded or load cannot
BP_ERROR_T BP_DynamicModuleLoadIntoGHT(P_BP_DYNAMIC_MODULE module)
{

	// global hash init must be ok
	if(!global_hash_table_init_ok)
		return ERR_FAILURE;

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION((P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry);

	// attempt to lookup /system/dynamic_modules table within the GHT
	P_BP_HTKE modules_table = BP_HashRegLookupTableByPathQuery((P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry, "/system/dynamic_modules");

	// if there is no modules table, attempt to create it
	if(!modules_table)
	{

		// attempt to add table
		if(!BP_HashRegAddTableByPathQuery((P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry, "/system/dynamic_modules"))
		{
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC((P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry, ERR_FAILURE);
		}

	}

	// check to see if the modules table was created ok
	modules_table = BP_HashRegLookupTableByPathQuery((P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry, "/system/dynamic_modules");
	if(!modules_table)
	{
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC((P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry, ERR_FAILURE);
	}


	// -- Check to see if module is already loaded -----------------

	// attempt to check and see if the module is already loaded
	if(BP_DynamicModuleIsAlreadyLoadedInGHT(module))
	{

		// set the inout flag to indicate that the module appears to already be loaded
		module->already_loaded = BP_TRUE;
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC((P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry, ERR_FAILURE);

	}

	// -- Module is Not Loaded, Attempt Loading

	// set path for new entry
	char * new_mod_table_entry[] =
	{
		"system",
		"dynamic_modules",
		module->so_module_name,
		NULL
	};

	// exit if we can't add our new entry
	if(!BP_HashRegAddTableByPath((P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry, (char **) &new_mod_table_entry))
	{
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC((P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry, ERR_FAILURE);
	}


	// -- add individual entries ---------------

	// add module name
	BP_HashRegAddDataToTableByPath
	(
			(P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry,
			(char **) &new_mod_table_entry,
			"so_module_name",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
			module->so_module_name,
			bpstrlen(module->so_module_name)
	);

	// add module handle
	BP_HashRegAddDataToTableByPath
	(
			(P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry,
			(char **) &new_mod_table_entry,
			"so_module_handle",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
			&module->so_handle,
			sizeof(module->so_handle)
	);

	// add module initial configuration JSON
	BP_HashRegAddDataToTableByPath
	(
			(P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry,
			(char **) &new_mod_table_entry,
			"so_module_config_json",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
			module->so_module_config_json,
			bpstrlen(module->so_module_config_json)
	);

	// add module load path
	BP_HashRegAddDataToTableByPath
	(
			(P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry,
			(char **) &new_mod_table_entry,
			"so_module_load_path",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
			module->so_module_load_path,
			bpstrlen(module->so_module_load_path)
	);

	// add module sha1 hash to registry
	BP_HashRegAddDataToTableByPath
	(
			(P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry,
			(char **) &new_mod_table_entry,
			"so_module_filesystem_sha1_hash",
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING,
			module->so_module_filesystem_sha1_hash,
			bpstrlen(module->so_module_filesystem_sha1_hash)
	);


	// --- mark module and exit routine after addition ------------

	// mark the module as having loaded ok
	module->loaded_ok = BP_TRUE;

	// return indicating that we've successfully loaded our module
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC((P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry, ERR_SUCCESS);

}

// Attempt to unload the dynamic module from the GHT
BP_ERROR_T BP_DynamicModuleUnloadFromGHTByName(char *module_name)
{

	// basic null check
	if(!module_name)
		return ERR_FAILURE;

	// filter test check
	if(!BP_HashRegPathArrayIndexFilterTest(module_name, bpstrlen(module_name)))
		return ERR_FAILURE;


	// -- enter critical section after module passes basic checks ----

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION((P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry);

	// set the module lookup path
	char *module_lookup_path[] =
	{
			"system",
			"dynamic_modules",
			module_name,
			NULL
	};


	// look for module table
	P_BP_HTKE module_table = BP_HashRegLookupTableByPath((P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry, (char **) &module_lookup_path);
	if(!module_table)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC((P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry, ERR_FAILURE);

	// declare keyed entries
	P_BP_HTKE so_module_name                 = NULL;
	P_BP_HTKE so_module_handle               = NULL;
	P_BP_HTKE so_module_config_json          = NULL;
	P_BP_HTKE so_module_load_path            = NULL;
	P_BP_HTKE so_module_filesystem_sha1_hash = NULL;

	// lookup entry data
	so_module_name                 = BP_HashRegLookupEntryFromHashKeyViaRelativePosition((P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry, module_table, "so_module_name");
	so_module_handle               = BP_HashRegLookupEntryFromHashKeyViaRelativePosition((P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry, module_table, "so_module_handle");
	so_module_config_json          = BP_HashRegLookupEntryFromHashKeyViaRelativePosition((P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry, module_table, "so_module_config_json");
	so_module_load_path            = BP_HashRegLookupEntryFromHashKeyViaRelativePosition((P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry, module_table, "so_module_load_path");
	so_module_filesystem_sha1_hash = BP_HashRegLookupEntryFromHashKeyViaRelativePosition((P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry, module_table, "so_module_filesystem_sha1_hash");

	// ---- Attempt to Close Handle to Library -------------

	// IMPORTANT NOTE: This does not call the library destructor as the library itself
	//                 does not unload from the process here.  It's a quirk of development
	//                 that dlclose() doesn't actually unload anything.

	// attempt to reference handle
	void *so_handle_actual = NULL;
	if(so_module_handle)
	{
		if(so_module_handle->data)
		{
			memcpy(&so_handle_actual, so_module_handle->data, sizeof(so_handle_actual));
		}
	}

	// close handle after refrence
	if(so_handle_actual)
	{
		dlclose(so_handle_actual);
	}



	// -- destroy individual entries ----------------

	BP_HashRegDestroyEntry
	(
			(P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry,
			so_module_name
	);
	BP_HashRegDestroyEntry
	(
			(P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry,
			so_module_handle
	);
	BP_HashRegDestroyEntry
	(
			(P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry,
			so_module_config_json
	);
	BP_HashRegDestroyEntry
	(
			(P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry,
			so_module_load_path
	);
	BP_HashRegDestroyEntry
	(
			(P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry,
			so_module_filesystem_sha1_hash
	);

	// --- destroy table itself -----

	BP_HashRegDestroyTable
	(
		(P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry,
		module_table
	);

	// return indicating that we've successfully loaded our module
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC((P_BP_HASH_TABLE_REGISTRY) &global_hash_table_registry, ERR_SUCCESS);

}




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Module Loader %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// All application wide dynamic loads are stored in the GHT only.  They are tracked and
// monitored via the GHT only.  The reason this routine takes a json module
// config and not a JSON string is so that you as a developer can attach json
// data which may have been created dynamically to the JSON structure without
// having to do a lot of sprintf() conversion nonsense.  You can just add JSON nodes by
// lookup path query instead. (BP_JSONAddNodeByPathQuery/BP_JSONAddTableByPathQuery)
BP_ERROR_T BP_LoadDynamicModulesByParsedJSON
(
	P_BP_DYNAMIC_MODULE_LOAD_HOOKS load_hooks,
	P_BP_JSON                      module_config
)
{

	// basic null checks
	if(!load_hooks)
		return ERR_FAILURE;
	if(!load_hooks->event_manager)
		return ERR_FAILURE;
	if(!load_hooks->httpd_session)
		return ERR_FAILURE;
	if(!module_config)
		return ERR_FAILURE;
	if(!module_config->json_out_hreg)
		return ERR_FAILURE;
	if(!module_config->json_out_hreg->hive)
		return ERR_FAILURE;
	if(!module_config->json_out_hreg->hive->data)
		return ERR_FAILURE;

	// display the global hash table registry
	BP_HashRegDisplay(&global_hash_table_registry, BP_FALSE, BP_TRUE);


	// Temporary module used during iteration.  It is filled out with REFERENCE DATA ONLY
	// and passed off to BP_DynamicModuleLoadIntoGHT().  Do not attempt to free it.
	BP_DYNAMIC_MODULE tmp_module;
	BP_ZERO_STACK(tmp_module);

	// iterate through the modules
	BP_HASH_ITER_OPEN((P_BP_HTKE) module_config->json_out_hreg->hive->data, current_module_iter)
	{

		// zero out the temporary module before loop
		BP_ZERO_STACK(tmp_module);

		// if the type is not a table, continue the loop
		if(current_module_iter->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_NAMED_HASH_TABLE)
			continue;

		// key must be reasonable/printable with no newlines/carriage returns
		if(!BP_StringIsReasonablePrintableString(current_module_iter->key, bpstrlen(current_module_iter->key), BP_FALSE, BP_FALSE))
			continue;

		// set parent table key (should always be top level table)
		P_BP_HTKE parent = (P_BP_HTKE) current_module_iter->parent;
		if(!parent)
			continue;

		// set module path
		char *mod_path[] =
		{
				parent->key,
				current_module_iter->key,
				NULL
		};

		// declare current module table path (must be freed)
		char * current_module_table_path = BP_HashRegPathArrayToStringLookup((char **) &mod_path);

		// lookup static entries
		P_BP_HTKE enabled         = BP_HashRegLookupTableEntryByDataKeyIndexAndTablePathQuery(module_config->json_out_hreg, current_module_table_path, "enabled");
		P_BP_HTKE so_path         = BP_HashRegLookupTableEntryByDataKeyIndexAndTablePathQuery(module_config->json_out_hreg, current_module_table_path, "so_path");
		P_BP_HTKE dev_description = BP_HashRegLookupTableEntryByDataKeyIndexAndTablePathQuery(module_config->json_out_hreg, current_module_table_path, "dev_description");
		P_BP_HTKE dev_notes       = BP_HashRegLookupTableEntryByDataKeyIndexAndTablePathQuery(module_config->json_out_hreg, current_module_table_path, "dev_notes");


		// --- Null checks on mandatory data lookups

		if(!enabled)
		{
			bpfree(current_module_table_path);
			continue;
		}

		if(!so_path)
		{
			bpfree(current_module_table_path);
			continue;
		}

		if(!dev_description)
		{
			bpfree(current_module_table_path);
			continue;
		}

		if(!dev_notes)
		{
			bpfree(current_module_table_path);
			continue;
		}


		// --- Data type checks


		if(enabled->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING)
		{
			bpfree(current_module_table_path);
			continue;
		}

		if(so_path->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING)
		{
			bpfree(current_module_table_path);
			continue;
		}

		if(dev_description->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING)
		{
			bpfree(current_module_table_path);
			continue;
		}

		if(dev_notes->type != BP_HASH_TABLE_KEYED_ENTRY_TYPE_UTF8_STRING)
		{
			bpfree(current_module_table_path);
			continue;
		}

		printf("\n Getting here too!?: %s - %p - %p - %p -%p",current_module_table_path,  enabled, so_path, dev_description, dev_notes);


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Process Data and Load Module %%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


		// attempt to enter critical section
		BP_HASH_TABLE_ENTER_CRITICAL_SECTION(NULL);

		printf("\n Attempting to load module: (%s) %s", (char *) so_path->data, (char *) current_module_iter->key);

		// create some temporary space on the stack to store our so sha1 value
		char so_sha1[50];
		BP_ZERO_STACK(so_sha1);

		// attempt to sha1 the contents
		BP_FileSHA1Contents((char *) so_path->data, (char *) &so_sha1);

		// printf("\n Module SHA1?: %s", so_sha1);

		// check that we've been able to create our sha1 hash ok
		if
		(
			bpstrlen((char *) &so_sha1) &&
			BP_FileAtPathExistsAndIsExecutable((char *) so_path->data) == ERR_SUCCESS
		)
		{


			// attempt to open the library
			void * so_handle = dlopen((char *) so_path->data, RTLD_NOW|RTLD_GLOBAL);
			if(so_handle)
			{


				printf("\n DLOPEN REPORTED THAT LIBRARY OPENED OK!: %p - %p", &global_hash_table_registry, so_handle);

				// attempt to retrieve the module configuration
				char **mod_config_from_lib = (char **) dlsym(so_handle, "global_mod_init_config_json");
				if(mod_config_from_lib)
				{

					// ensure we have a reference pointer
					if(mod_config_from_lib[0])
					{

						// ensure reference is reasonable printable
						if(BP_StringIsReasonablePrintableString(mod_config_from_lib[0], bpstrlen(mod_config_from_lib[0]), BP_TRUE, BP_TRUE))
						{

							// create new json parser
							P_BP_JSON mod_json = BP_JSONCreate();
							if(BP_JSONParseFromBuffer(mod_json, mod_config_from_lib[0], bpstrlen(mod_config_from_lib[0]), 100, 100))
							{

								BP_JSONDisplay(mod_json);
								BP_JSONFinalizeOutput(mod_json);
								BP_JSONDisplay(mod_json);

								// after all cascading if() checks are passed, perform tmp_module data assignments
								// so that the module can be added to the ght dynamic_modules table.
								tmp_module.so_module_name                 = current_module_iter->key;
								tmp_module.so_handle                      = so_handle;
								tmp_module.so_module_config_json          = mod_config_from_lib[0];
								tmp_module.so_module_load_path            = (char *) so_path->data;
								tmp_module.so_module_filesystem_sha1_hash = (char *) &so_sha1;

								// attempt to load the dynamic module via parsed json
								if(!BP_DynamicModuleLoadIntoGHT((P_BP_DYNAMIC_MODULE) &tmp_module))
									printf("\n [!!] The dynamic module '%s' Could not be loaded into the process space due to one or more errors (dynamic loader has failed)", tmp_module.so_module_name);

							}
							else
							{
								printf("\n [!!] The dynamic module '%s' failed to load due to being unable to parse module configuration data from global module symbol lookup (JSON examined is within the module itself, not within this app).", tmp_module.so_module_name);

							}

							// destroy the json session if it's set
							if(mod_json)
								BP_JSONDestroy(mod_json);

						}

					}

				}

			}

		}

		// attempt to leave critical section
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(NULL);


		// --- Extra data


		// This is extra data which must be parsed down into json and provided to the module
		// when it's entry point is called.
		P_BP_HTKE extra_data_table = BP_HashRegLookupTableEntryByDataKeyIndexAndTablePathQuery(module_config->json_out_hreg, current_module_table_path, "extra_data");

		// destroy table path
		if(current_module_table_path)
			bpfree(current_module_table_path);



		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Process Module Load Failures %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// if the mod hasn't loaded ok, exit indicating failure
		if(!tmp_module.loaded_ok)
		{
			printf("\n [!!] Module load failed for %s", tmp_module.so_module_name);
			return ERR_FAILURE;
		}

	}
	BP_HASH_ITER_CLOSE;


	// return indicating success
	return ERR_SUCCESS;

}


/*
 * BP-HTTPdUsers.cc
 *
 *  Created on: Jul 16, 2015
 *      Author: root
 */


#include "../../../include/BP-Main.h"

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Add Users %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Adds a user to the httpd database.
BP_ERROR_T BP_HTTPdUserAdd
(
		P_BP_HTTPD_SESSION               httpd,
		BP_HTTPD_USER_ACCESS             access,
		BP_HTTPD_USER_TYPE               type,
		BP_HTTPD_USER_PASSWORD_HASH_TYPE hash_type,
		char *                           username,
		char *                           password,
		char *                           password_salt,
		char *                           email,
		char *                           bio,
		char *                           role,
		BP_BOOL                          password_has_been_prehashed,
		P_BP_JSON                        extra_data
)
{


	// ensure we have a session structure and a registry to work with
	if(!httpd)
		return ERR_FAILURE;
	if(!httpd->system_hreg)
		return ERR_FAILURE;


	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Sanity Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Ensure we have a username and password.  These are non-optional.
	if(!username)
		return ERR_FAILURE;
	if(!password)
		return ERR_FAILURE;

	// check sanity on all values
	if(BP_StringIsReasonablePrintableString(username, bpstrlen(username), BP_FALSE, BP_FALSE) != BP_TRUE)
		return ERR_FAILURE;
	if(BP_StringIsReasonablePrintableString(password, bpstrlen(password), BP_FALSE, BP_FALSE) != BP_TRUE)
		return ERR_FAILURE;

	// check sanity on non-optional values.
	if(email)
	if(BP_StringIsReasonablePrintableString(email,    bpstrlen(email),    BP_FALSE, BP_FALSE) != BP_TRUE)
		return ERR_FAILURE;
	if(bio)
	if(BP_StringIsReasonablePrintableString(bio,      bpstrlen(bio),      BP_FALSE, BP_FALSE) != BP_TRUE)
		return ERR_FAILURE;
	if(role)
	if(BP_StringIsReasonablePrintableString(role,     bpstrlen(role),     BP_FALSE, BP_FALSE) != BP_TRUE)
		return ERR_FAILURE;




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check if User Exists %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section before making changes to the httpd hash
	// table registry.
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(httpd->system_hreg);

	// attempt to lookup user by username only (cannot have duplicate usernames)
	P_BP_HTTPD_USER user = BP_HTTPdUserLookup
	(
			httpd,
			username,
			NULL,
			NULL,
			NULL,
			NULL
	);

	// if the user already exists, exit immediately
	if(user)
	{
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(httpd->system_hreg, ERR_FAILURE);
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Create New User Structure %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the initial path
	table_path[0] = "users";
	table_path[1] = NULL;

	// attempt to add data to the table
	BP_ERROR_T err_status = BP_HashRegAddDataToTableByPath
	(
		httpd->system_hreg,
		table_path,
		username,
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_HTTPD_USER)
	);

	// attempt to lookup the user after adding
	user = BP_HTTPdUserLookup(httpd,username,NULL,NULL,NULL,NULL);
	if(!user)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(httpd->system_hreg, ERR_FAILURE);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Fill In User Details %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the specifiers 1 to 1
	user->access    = access;
	user->type      = type;


	// set username from dup
	user->username = bpstrdup(username);

	// generate a random ascii character salt
	user->password_salt = bpstrdup(password_salt);


	// set password based on hash type
	switch(hash_type)
	{

		case BP_HTTPD_USER_PASSWORD_HASH_TYPE_MD5:
			user->hash_type = hash_type;
			break;

		case BP_HTTPD_USER_PASSWORD_HASH_TYPE_SHA1:
			user->hash_type = hash_type;
			break;

		case BP_HTTPD_USER_PASSWORD_HASH_TYPE_SHA256:
			user->hash_type = hash_type;
			break;

		default:
			break;
	}


	// if the password isn't pre-hashed, go ahead and try to hash it
	if(password_has_been_prehashed == BP_FALSE)
	{

		// create salted hash
		user->password_hash = BP_HTTPdUserCreateSaltedHash
		(
			user->hash_type,
			user->password_salt,
			password
		);

	}
	else
	{

		// set password hash directly
		user->password_hash = bpstrdup(password);

	}

	// add email/bio/role
	if(email)
		user->email = bpstrdup(email);
	if(bio)
		user->bio = bpstrdup(bio);
	if(role)
		user->role = bpstrdup(role);


	// add extra data JSON registry if set
	if(extra_data)
		user->externally_managed_extra_json_data = extra_data;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Unlock and Exit %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section before exiting
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(httpd->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}


// Attempts to lookup a user within the table.
P_BP_HTTPD_USER BP_HTTPdUserLookup
(
		P_BP_HTTPD_SESSION httpd,
		char * username,
		char * password,
		char * email,
		char * bio,
		char * role

)
{

	// ensure we have a session structure and a registry to work with
	if(!httpd)
		return NULL;
	if(!httpd->system_hreg)
		return NULL;


	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Sanity Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Ensure we have a username and password.  These are non-optional.
	if(!username)
		return NULL;

	// check sanity on non-optional values
	if(BP_StringIsReasonablePrintableString(username, bpstrlen(username), BP_FALSE, BP_FALSE) != BP_TRUE)
		return NULL;

	// check sanity on optional values.
	if(password)
	if(BP_StringIsReasonablePrintableString(password, bpstrlen(password), BP_FALSE, BP_FALSE) != BP_TRUE)
		return NULL;
	if(email)
	if(BP_StringIsReasonablePrintableString(email,    bpstrlen(email),    BP_FALSE, BP_FALSE) != BP_TRUE)
		return NULL;
	if(bio)
	if(BP_StringIsReasonablePrintableString(bio,      bpstrlen(bio),      BP_FALSE, BP_FALSE) != BP_TRUE)
		return NULL;
	if(role)
	if(BP_StringIsReasonablePrintableString(role,     bpstrlen(role),     BP_FALSE, BP_FALSE) != BP_TRUE)
		return NULL;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check if User Exists %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section before making changes to the httpd hash
	// table registry.
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(httpd->system_hreg);


	// declare the lookup entry
	P_BP_HASH_TABLE_KEYED_ENTRY lookup_entry = NULL;

	// set the table path for lookup
	table_path[0] = "users";
	table_path[1] = username;
	table_path[2] = NULL;

	// attempt to get entry
	lookup_entry = BP_HashRegLookupTableEntryByPath(httpd->system_hreg,table_path);

	// ensure we can lookup the new entry
	if(!lookup_entry)
	{
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(httpd->system_hreg, NULL);
	}

	// set user
	P_BP_HTTPD_USER user = (P_BP_HTTPD_USER) lookup_entry->data;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%% Run Password Check %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// run password check
	if(password)
	{

		// if there is no password set, and we're looking for a record with a password
		// exit.
		if(!user->password_hash)
		{
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(httpd->system_hreg, NULL);
		}

		// attempt to create a salted hash
		char * salted_pw = BP_HTTPdUserCreateSaltedHash
		(
				user->hash_type,
				user->password_salt,
				password
		);

		// if the salted password couldn't be generated, exit
		if(!salted_pw)
		{
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(httpd->system_hreg, NULL);
		}

		// run simple string comparison to determine if hashes match (safe due to
		// prechecking)
		if(strcmp(salted_pw, user->password_hash) != 0)
		{

			// destroy salted password
			if(salted_pw)
				bpfree(salted_pw);

			// exit
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(httpd->system_hreg, NULL);

		}


		// destroy salted password
		if(salted_pw)
			bpfree(salted_pw);

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Email / Bio / Role Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(email)
	{

		// ensure we have an email
		if(!user->email)
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(httpd->system_hreg, NULL);

		// compare email
		if(strcmp(email, user->email) != 0)
		{
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(httpd->system_hreg, NULL);
		}


	}

	if(bio)
	{

		// ensure we have an bio
		if(!user->bio)
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(httpd->system_hreg, NULL);

		// compare bio
		if(strcmp(bio, user->bio) != 0)
		{
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(httpd->system_hreg, NULL);
		}

	}

	if(role)
	{

		// ensure we have an role
		if(!user->role)
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(httpd->system_hreg, NULL);

		// compare role
		if(strcmp(role, user->role) != 0)
		{
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(httpd->system_hreg, NULL);
		}

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Unlock and Exit %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section before exiting
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(httpd->system_hreg);

	// return null if the record cannot be found
	return user;

}



// Attempts to generate a salted hash buffer provided a given password.  Result
// must be free'd.
char * BP_HTTPdUserCreateSaltedHash
(
		BP_HTTPD_USER_PASSWORD_HASH_TYPE hash_type,
		char * salt,
		char * pass
)
{

	if(!hash_type)
		return NULL;
	if(!salt)
		return NULL;
	if(!pass)
		return NULL;

	// set the salted password buff length
	size_t salted_password_buff_len = bpstrlen(salt)+bpstrlen(pass)+10;

	// allocate space for the salted buffer
	char *salted_password_buff = (char *) bpcalloc(salted_password_buff_len, 1);

	// generate the password buff (with some extra buffer)
	snprintf(salted_password_buff, bpstrlen(salt) + bpstrlen(pass) + 8, "%s%s", salt, pass);

	// the heap allocated hash to return
	char * ret_hash = (char *) NULL;

	// set password based on hash type
	switch(hash_type)
	{

		case BP_HTTPD_USER_PASSWORD_HASH_TYPE_MD5:
			ret_hash = BP_GenMD5(salted_password_buff, bpstrlen(salted_password_buff));
			break;

		case BP_HTTPD_USER_PASSWORD_HASH_TYPE_SHA1:
			ret_hash = BP_GenSHA1(salted_password_buff, bpstrlen(salted_password_buff));
			break;

		case BP_HTTPD_USER_PASSWORD_HASH_TYPE_SHA256:
			ret_hash = BP_GenSHA256(salted_password_buff, bpstrlen(salted_password_buff));
			break;

		default:
			break;
	}

	// zero out the salted password buffer (prevents it from sitting in memory)
	memset(salted_password_buff, 0x00, salted_password_buff_len-1);

	// destroy the salted buffer after the hash is calculated
	bpfree(salted_password_buff);

	// return the buffer
	return ret_hash;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% JSON User Loading %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// load from json buffer
BP_ERROR_T BP_HTTPdUserLoadFromJSONBuffer
(
	P_BP_HTTPD_SESSION httpd,
	char *json_buffer
)
{


	// basic null checks
	if(!httpd)
		return ERR_FAILURE;
	if(!json_buffer)
		return ERR_FAILURE;

	// ensure content is reasonable/printable (newlines ok, carriage returns are not)
	if(!BP_StringIsReasonablePrintableString(json_buffer, bpstrlen(json_buffer), BP_TRUE, BP_FALSE))
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Load JSON %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new registry
	P_BP_JSON user_json = BP_JSONCreate();
	if(!user_json)
	{

		// free the loaded file and return indicating failure
		return ERR_FAILURE;

	}

	// attempt to parse users (500 tokens total)
	if(!BP_JSONParseFromBuffer(user_json, json_buffer, bpstrlen(json_buffer), 30, 500))
	{

		// debug message
		printf("\n [!!] Error: Failed to parse users within from json buffer.  Displaying json parse errors below.");

		// display parser problem data and destroy json
		BP_JSONDisplayParseMessages(user_json);
		BP_JSONDestroy(user_json);

		// return indicating failure
		return ERR_FAILURE;

	}


	// finalize and display
	BP_JSONFinalizeOutput(user_json);
	BP_JSONDisplay(user_json);

	/*
	BP_ERROR_T BP_HTTPdUserAdd
	(
			P_BP_HTTPD_SESSION               httpd,
			BP_HTTPD_USER_ACCESS             access,
			BP_HTTPD_USER_TYPE               type,
			BP_HTTPD_USER_PASSWORD_HASH_TYPE hash_type,
			char *                           username,
			char *                           password,
			char *                           password_salt,
			char *                           email,
			char *                           bio,
			char *                           role,
			BP_BOOL                          password_has_been_prehashed
	);
	*/

	// return indicating success
	return ERR_SUCCESS;

}

// Attempt to load user tables via JSON file.
BP_ERROR_T BP_HTTPdUserLoadFromJSONFile(P_BP_HTTPD_SESSION httpd, char *path_to_json_file)
{


	// ensure there's a json file to load from
	if(!path_to_json_file)
		return ERR_FAILURE;
	if(!BP_StringIsReasonableForUnixEXT4Filename(path_to_json_file, bpstrlen(path_to_json_file), BP_TRUE))
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Secure Load Steps %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create space to hold sha1
	char json_file_sha1[60];
	BP_ZERO_STACK(json_file_sha1);

	// attempt to sha1 contents
	BP_FileSHA1Contents(path_to_json_file, (char *) &json_file_sha1);
	if(bpstrlen(json_file_sha1) != 40)
	{
		printf("\n [!!] Error: Failed to generate SHA1 sum for user json file: (file len: %zu)", bpstrlen(json_file_sha1));
		return ERR_FAILURE;
	}

	// setup local extra info structure
	BP_FILE_LOAD_EXTRA_INFO load_extra_info;
	BP_ZERO_STACK(load_extra_info);

	// mark flag to create sha1 content during content retrieval
	load_extra_info.check_contents_sha1 = BP_TRUE;

	// attempt to actually load buffer
	char * file_load = BP_FileGetContents(path_to_json_file, &load_extra_info);

	// if the file failed to load
	if(!file_load)
	{
		printf("\n [!!] Error: Failed to load user json file.  Could not read file into memory.");
		return ERR_FAILURE;
	}

	// check to ensure that sha1 sum generated ok
	if(load_extra_info.sha1_sum_generated_ok != BP_TRUE)
	{
		bpfree(file_load);
		printf("\n [!!] SHA1 sum failed when loading user json file.  Potential TOCTOU tampering detected.");
		return ERR_FAILURE;
	}

	// ensure hashes match
	if(!BP_StringsMatchAsReasonableStrings((char *) &load_extra_info.sha1_sum, (char *) &json_file_sha1))
	{
		bpfree(file_load);
		printf("\n [!!] SHA1 sum mismatch when loading user json file.  Potential TOCTOU tampering detected.");
		return ERR_FAILURE;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Load Users from JSON Directly %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// load from json buffer
	BP_ERROR_T load_status = BP_HTTPdUserLoadFromJSONBuffer
	(
		httpd,
		file_load
	);

	// load status
	if(!load_status)
	{
		// destroy the loaded file before exiting
		bpfree(file_load);
		printf("\n [!!] Error: Failed to add users from JSON buffer.");
		return ERR_FAILURE;
	}


	// return indicating success
	return ERR_SUCCESS;

}


// attempts to create a JSON tree containing a valid httpd user record.  (used for modifying user registry)
P_BP_JSON BP_HTTPdCreateUserJSON
(
	BP_HTTPD_USER_ACCESS             access,
	BP_HTTPD_USER_TYPE               type,
	BP_HTTPD_USER_PASSWORD_HASH_TYPE hash_type,
	char *                           username,
	char *                           password,
	char *                           email,
	char *                           bio,
	char *                           role
)
{



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Basic Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// check username
	if(!BP_StringIsReasonableForUsernameMariaDB(username, bpstrlen(username)))
	{
		return NULL;
	}

	// check password
	if(!BP_StringIsReasonableForPasswordMariaDB(password, bpstrlen(password)))
	{
		return NULL;
	}

	// check email
	if(!BP_StringIsValidEmailAddress(email))
	{
		return NULL;
	}

	// check bio
	if(!BP_StringIsReasonablePrintableString(bio, bpstrlen(bio), BP_FALSE, BP_FALSE))
	{
		return NULL;
	}

	// check role
	if(!BP_StringIsReasonablePrintableString(role, bpstrlen(role), BP_FALSE, BP_FALSE))
	{
		return NULL;
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create JSON and Add Table Entries %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new json object
	P_BP_JSON new_user_json = BP_JSONCreate();
	if(!new_user_json)
		return NULL;

	// set path info
	char *user_path[20];
	BP_ZERO_STACK(user_path);




	// --- add access -------------------------

	// add access level
	user_path[1] = "access_level";

	// switch on the access level and determine privilege
	switch(access)
	{

		case BP_HTTPD_USER_ACCESS_ANONYMOUS_GUEST:
			BP_JSONAddNodeByPath(new_user_json, (char **) &user_path, "anonymous_guest", bpstrlen("anonymous_guest"), BP_FALSE);
			break;

		case BP_HTTPD_USER_ACCESS_DEMIGOD:
			BP_JSONAddNodeByPath(new_user_json, (char **) &user_path, "demigod", bpstrlen("demigod"), BP_FALSE);
			break;

		case BP_HTTPD_USER_ACCESS_GOD:
			BP_JSONAddNodeByPath(new_user_json, (char **) &user_path, "god", bpstrlen("god"), BP_FALSE);
			break;

		case BP_HTTPD_USER_ACCESS_GUEST:
			BP_JSONAddNodeByPath(new_user_json, (char **) &user_path, "guest", bpstrlen("guest"), BP_FALSE);
			break;

		case BP_HTTPD_USER_ACCESS_MORTAL:
			BP_JSONAddNodeByPath(new_user_json, (char **) &user_path, "mortal", bpstrlen("mortal"), BP_FALSE);
			break;

		case BP_HTTPD_USER_ACCESS_SEMIGOD:
			BP_JSONAddNodeByPath(new_user_json, (char **) &user_path, "semigod", bpstrlen("semigod"), BP_FALSE);
			break;

		default:
			BP_JSONAddNodeByPath(new_user_json, (char **) &user_path, "invalid", bpstrlen("invalid"), BP_FALSE);
			break;

	}

	// --- add type ------------------------------

	// switch on the type
	user_path[1] = "type";
	switch(type)
	{

		case BP_HTTPD_USER_TYPE_ANONYMOUS_GUEST:
			BP_JSONAddNodeByPath(new_user_json, (char **) &user_path, "anonymous_guest", bpstrlen("anonymous_guest"), BP_FALSE);
			break;

		case BP_HTTPD_USER_TYPE_LOCAL:
			BP_JSONAddNodeByPath(new_user_json, (char **) &user_path, "local", bpstrlen("local"), BP_FALSE);
			break;

		case BP_HTTPD_USER_TYPE_GLOBALDB:
			BP_JSONAddNodeByPath(new_user_json, (char **) &user_path, "globaldb", bpstrlen("globaldb"), BP_FALSE);
			break;

		default:
			BP_JSONAddNodeByPath(new_user_json, (char **) &user_path, "invalid", bpstrlen("invalid"), BP_FALSE);
			break;
	}


	// --- add hash type -------------------------

	// switch on the type
	user_path[1] = "hash_type";
	switch(hash_type)
	{

		// md5 not supported
		case BP_HTTPD_USER_PASSWORD_HASH_TYPE_MD5:
			printf("\n [!!] MD5 User Passwords are not supported for security reasons (too easy to crack)");
			break;

		// sha1
		case BP_HTTPD_USER_PASSWORD_HASH_TYPE_SHA1:
			BP_JSONAddNodeByPath(new_user_json, (char **) &user_path, "sha1", bpstrlen("sha1"), BP_FALSE);
			break;

		// sha256
		case BP_HTTPD_USER_PASSWORD_HASH_TYPE_SHA256:
			BP_JSONAddNodeByPath(new_user_json, (char **) &user_path, "sha256", bpstrlen("sha256"), BP_FALSE);
			break;

		// default is invalid case
		default:
			BP_JSONAddNodeByPath(new_user_json, (char **) &user_path, "invalid", bpstrlen("invalid"), BP_FALSE);
			break;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add String Vals To JSON %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add user table to json
	user_path[0] = username;
	BP_JSONAddTableByPath(new_user_json, (char **) &user_path, BP_FALSE);

	// add username
	user_path[1] = "enabled";
	BP_JSONAddNodeByPath(new_user_json, (char **) &user_path, "yes", bpstrlen("yes"), BP_FALSE);

	// add username
	user_path[1] = "email";
	BP_JSONAddNodeByPath(new_user_json, (char **) &user_path, email, bpstrlen(email), BP_FALSE);

	// add bio
	user_path[1] = "bio";
	BP_JSONAddNodeByPath(new_user_json, (char **) &user_path, bio, bpstrlen(bio), BP_FALSE);

	// add role
	user_path[1] = "role";
	BP_JSONAddNodeByPath(new_user_json, (char **) &user_path, role, bpstrlen(role), BP_FALSE);


	// return the JSON registry
	return new_user_json;

}




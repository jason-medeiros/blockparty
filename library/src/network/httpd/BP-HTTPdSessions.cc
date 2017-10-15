/*
 * BP-HTTPdSessions.cc
 *
 *  Created on: Jul 18, 2015
 *      Author: root
 */


// main include file
#include "../../../include/BP-Main.h"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Sessions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Add a session for a user.
P_BP_HTTPD_USER_SESSION BP_HTTPdSessionAdd
(
		P_BP_HTTPD_SESSION           httpd,
		BP_HTTPD_USER_SESSION_TYPE   session_type,
		P_BP_HTTPD_USER              user,
		size_t                       session_lifetime_in_seconds,
		size_t                       xsrf_token_count
)
{

	// ensure we have a daemon
	if(!httpd)
		return NULL;

	// ensure we have a system registry
	if(!httpd->system_hreg)
		return NULL;

	// ensure we have a session length (in seconds)
	if(!session_lifetime_in_seconds)
		return NULL;

	// ensure we have a user
	if(!user)
		return NULL;

	// xsrf tokens are required whether you use them or not
	if(!xsrf_token_count)
		return NULL;



	// declare dynamic table path
	char * table_path[10];
	BP_ZERO_STACK(table_path);

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// enter critical section
	BP_HTTPD_ENTER_CRITICAL_SECTION;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt to Create New User Structure %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the initial path
	table_path[0] = "sessions";
	table_path[1] = NULL;

	// generate random uuid
	char * new_session_uuid = BP_GenRandomUUID();

	// attempt to add data to the table
	BP_ERROR_T err_status = BP_HashRegAddDataToTableByPath
	(
		httpd->system_hreg,
		table_path,
		new_session_uuid,
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_HTTPD_USER_SESSION)
	);

	// attempt to lookup the user session after adding to registry
	P_BP_HTTPD_USER_SESSION user_session = BP_HTTPdSessionLookup(httpd, new_session_uuid);
	if(!user_session)
	{
		bpfree(new_session_uuid);
		BP_HTTPD_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);
	}

	// set the identifier here (don't ht_strdup, it's freed directly using bpfree in the session destructor)
	user_session->identifier = new_session_uuid;

	// set the session user here
	user_session->user = user;

	// set the session type
	user_session->type = session_type;

	// set default session life to 12 hours
	// user_session->life_in_seconds = BP_HOUR * 12;
	user_session->life_in_seconds = session_lifetime_in_seconds;

	// set issued time
	user_session->issued_time = (size_t) BP_TimeGetSecondsSinceUnixEpoch();

	// set cyclical lookup for the session (cast as void pointer)
	user->session = (void *) user_session;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create XSRF Registry %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create xsrf registry
	user_session->xsrf_reg = BP_CreateHashTableRegistry();
	if(!user_session->xsrf_reg)
	{

		// destroy session before exit
		BP_HTTPdSessionDestroy(	httpd, user_session);

		// exit here
		BP_HTTPD_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	}

	// add the table path
	char *xsrf_table_add_path[10];
	BP_ZERO_STACK(xsrf_table_add_path);

	// set tokens path
	xsrf_table_add_path[0] = "tokens";

	// add table
	BP_HashRegAddTableByPath(user_session->xsrf_reg, (char **) &xsrf_table_add_path);



	// add xsrf tokens to new session
	if(!BP_HTTPdSessionXSRFTokensAdd(user_session, xsrf_token_count))
	{

		// destroy session before exit
		BP_HTTPdSessionDestroy(	httpd, user_session);

		// exit here
		BP_HTTPD_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	}

	printf("\n HERE HERE ADDED TOKENS DISPLAYING REGISTRY!!");
	BP_HashRegDisplay(user_session->xsrf_reg, BP_FALSE, BP_TRUE);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create User Session Persistent Data Registry %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create persistent data storage registry
	user_session->session_data_hreg = BP_CreateHashTableRegistry();
	if(!user_session->session_data_hreg)
	{

		// destroy session before exit
		BP_HTTPdSessionDestroy(	httpd, user_session );

		// exit here
		BP_HTTPD_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	}

	// dbg optional
	// printf("\n New user session created ok?: %p", user_session);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add API Key %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// generate random uuid for api key
	char * api_key = BP_GenRandomUUID();

	// api key privileges set here (they are copied during addition)
	BP_HTTPD_API_KEY_PRIVS api_key_privs;
	BP_ZERO_STACK(api_key_privs);


	// debug apikey
	printf("\n [APIKEY] Adding new API key!: %s", api_key);
	printf("\n");
	printf("\n");
	// __asm("int3");

	// attempt to add random api key
	if(!BP_HTTPdSessionAddAPIKey(httpd, user_session, api_key, &api_key_privs))
	{
		bpfree(api_key);
		BP_HTTPD_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);
	}

	// free api key buffer
	bpfree(api_key);

	// leave the critical section
	BP_HTTPD_LEAVE_CRITICAL_SECTION;

	// return the new user session
	return user_session;

}


// Destroy a user session.
BP_ERROR_T BP_HTTPdSessionDestroy
(
		P_BP_HTTPD_SESSION      httpd,
		P_BP_HTTPD_USER_SESSION user_session
)
{

	if(!httpd)
		return ERR_FAILURE;
	if(!user_session)
		return ERR_FAILURE;


	// declare dynamic table path
	char * table_path[10] = {0};

	// declare the lookup pointer
	P_BP_HASH_TABLE_KEYED_ENTRY tmp_lookup = NULL;

	// enter critical section before making changes to the httpd hash
	// table registry.
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(httpd->system_hreg);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy Member Data %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	if(user_session->user)
	{

		// set the user session to null (pevent loop lookups)
		user_session->user->session = NULL;

	}

	// create space on stack to save the request uuid (needed for deletion)
	char saved_uuid[BP_UUID_LEN+10];
	BP_ZERO_STACK(saved_uuid);

	// save uuid if present (should always be present)
	if(user_session->identifier)
	{
		// copy in the uuid (so we can delete it from parent hreg)
		memcpy((char *) &saved_uuid, user_session->identifier, BP_UUID_LEN);

		// free heap memory
		bpfree(user_session->identifier);
		user_session->identifier = NULL;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Remove From Request Registry %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the requests table
	table_path[0] = "sessions";
	table_path[1] = (char *) &saved_uuid;
	table_path[2] = NULL;

	// attempt to lookup the entry to delete
	P_BP_HASH_TABLE_KEYED_ENTRY session_entry = BP_HashRegLookupTableEntryByPath(httpd->system_hreg, table_path);

	// if we can't look it up, exit immediately
	if(!session_entry)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(httpd->system_hreg, ERR_FAILURE);

	// destroy the entry
	BP_HashRegDestroyEntry(httpd->system_hreg, session_entry);

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(httpd->system_hreg);


	// return indicating success
	return ERR_SUCCESS;

}

// Retrieves a valid user if a valid api key is matched
P_BP_HTTPD_USER_SESSION BP_HTTPdSessionAuthUserFromAPIKey
(
	P_BP_HTTPD_SESSION httpd,
	char * api_key
)
{

	if(!httpd)
		return NULL;
	if(!httpd->system_hreg)
		return NULL;


	// enter critical section
	BP_HTTPD_ENTER_CRITICAL_SECTION;

	// attempt to lookup the api key
	P_BP_HTTPD_API_KEY api_key_lookup = BP_HTTPdSessionLookupAPIKeyByIdentifier(httpd, api_key);
	if(!api_key_lookup)
		BP_HTTPD_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);
	if(!api_key_lookup->user_session)
		BP_HTTPD_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	// set session lookup from api key
	P_BP_HTTPD_USER_SESSION session_lookup = (P_BP_HTTPD_USER_SESSION) api_key_lookup->user_session;

	// check if the session is expired
	if(!BP_HTTPdSessionExpired((P_BP_HTTPD_USER_SESSION) session_lookup))
	{

		// if the session isn't expired, return it
		BP_HTTPD_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC((P_BP_HTTPD_USER_SESSION) session_lookup);

	}

	// destroy the user session
	BP_HTTPdSessionDestroy(httpd, (P_BP_HTTPD_USER_SESSION) session_lookup);

	// set session ptr to null
	session_lookup = NULL;

	// return null if the session was expired
	BP_HTTPD_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(session_lookup);

}

// Retrieves a valid user if a valid session id is matched.
P_BP_HTTPD_USER_SESSION BP_HTTPdSessionAuthUserFromSessionId
(
		P_BP_HTTPD_SESSION httpd,
		char * session_id
)
{

	if(!httpd)
		return NULL;
	if(!httpd->system_hreg)
		return NULL;

	// enter critical section
	BP_HTTPD_ENTER_CRITICAL_SECTION;

	// attempt to lookup the session
	P_BP_HTTPD_USER_SESSION session_lookup = BP_HTTPdSessionLookup
	(
			httpd,
			session_id
	);

	printf("\n After Session Lookup: %p - %s", session_lookup, session_id);
	printf("\n");
	printf("\n");
	// __asm("int3");

	// check if the session is expired
	if(BP_HTTPdSessionExpired((P_BP_HTTPD_USER_SESSION) session_lookup) != BP_TRUE)
	{

		// if the session isn't expired, return it
		BP_HTTPD_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC((P_BP_HTTPD_USER_SESSION) session_lookup);

	}

	// destroy the user session
	BP_HTTPdSessionDestroy(httpd, (P_BP_HTTPD_USER_SESSION) session_lookup);

	// set session ptr to null
	session_lookup = NULL;

	// return null if the session was expired
	BP_HTTPD_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(session_lookup);

}

// Creates a session by looking at the user database and selecting
// a user.
P_BP_HTTPD_USER_SESSION BP_HTTPdSessionAuthUserFromCreds
(
		P_BP_HTTPD_SESSION httpd,
		char             * user,
		char             * pass
)
{


	// ensure we have a daemon
	if(!httpd)
		return NULL;

	// ensure we have a user
	if(!user)
		return NULL;

	// ensure we have a password
	if(!pass)
		return NULL;



	// Attempts to lookup a user within the table.
	P_BP_HTTPD_USER user_lookup = BP_HTTPdUserLookup
	(
			httpd,
			user,
			pass,
			NULL,
			NULL,
			NULL
	);

	// ensure we can lookup the user
	if(!user_lookup)
		return NULL;

	// check if we already have a session
	if(user_lookup->session)
	{

		// note: always destroy session on successful login, this permits multiple user logins
		//       to exist without too much conflict.

		/*
		// check if the session is expired
		if(!BP_HTTPdSessionExpired((P_BP_HTTPD_USER_SESSION)user_lookup->session))
		{

			// if the session isn't expired, return it
			return (P_BP_HTTPD_USER_SESSION) user_lookup->session;

		}
		*/

		printf("\n WE WERE ABLE TO LOOKUP THE SESSION OK HERE?!");

		// destroy the user session
		BP_HTTPdSessionDestroy(httpd, (P_BP_HTTPD_USER_SESSION) user_lookup->session);

		// set session ptr to null
		user_lookup->session = NULL;

	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add New Session %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// session
	P_BP_HTTPD_USER_SESSION session = (P_BP_HTTPD_USER_SESSION) NULL;

	// 10 tokens seems large enough for any and all tasks
	size_t default_xsrf_token_count = 10;


	// add new session with 10 random xsrf tokens
	session = BP_HTTPdSessionAdd
	(
			httpd,
			BP_HTTPD_USER_SESSION_TYPE_AUTHENTICATED_USER,
			user_lookup,
			BP_HOUR,
			default_xsrf_token_count
	);

	// set session
	user_lookup->session = session;

	printf("\n Looked up user ok?: %p - %s", user_lookup, user_lookup->username);


	// return the session
	return session;

}

// Creates an anonymous session which only grants access to xsrf
// token generation.  This provides xsrf tokens which can be utilized
// with a pre-authentication environment.  Typically this is used to
// prevent xsrf on login forms while still allowing login.
P_BP_HTTPD_USER_SESSION BP_HTTPdSessionCreateAnonymousGuest
(
		P_BP_HTTPD_SESSION httpd
)
{

	// ensure we have a daemon
	if(!httpd)
		return NULL;

	// create temporary user for this session
	P_BP_HTTPD_USER tmp_user = NULL;

	// generate a temporary username and password
	char *tmp_user_name = BP_GenRandomUUID();
	char *tmp_password  = BP_GenRandomUUID();
	char *tmp_user_salt = BP_GenRandomUUID();


	// attempt to add guest user (will be expired later in the main application loop)
    BP_HTTPdUserAdd
    (
            httpd,
			BP_HTTPD_USER_ACCESS_ANONYMOUS_GUEST,
			BP_HTTPD_USER_TYPE_ANONYMOUS_GUEST,
            BP_HTTPD_USER_PASSWORD_HASH_TYPE_SHA256,
			tmp_user_name,
			tmp_password,
			tmp_user_salt,
            "tmp_guest@localhost",
            "temporary_guest",
            "guest"
    );




	// Attempts to lookup the new temporary lookup.
	P_BP_HTTPD_USER user_lookup = BP_HTTPdUserLookup
	(
			httpd,
			tmp_user_name,
			tmp_password,
			NULL,
			NULL,
			NULL
	);

	// ensure we can lookup the user
	if(!user_lookup)
		return NULL;

	// check if we already have a session
	if(user_lookup->session)
	{

		// check if the session is expired
		if(!BP_HTTPdSessionExpired((P_BP_HTTPD_USER_SESSION)user_lookup->session))
		{

			// if the session isn't expired, return it
			return (P_BP_HTTPD_USER_SESSION) user_lookup->session;

		}

		// destroy the user session
		BP_HTTPdSessionDestroy(httpd, (P_BP_HTTPD_USER_SESSION) user_lookup->session);

		// set session ptr to null
		user_lookup->session = NULL;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add New Session %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// session
	P_BP_HTTPD_USER_SESSION session = (P_BP_HTTPD_USER_SESSION) NULL;

	// 10 tokens seems large enough for any and all tasks
	size_t default_xsrf_token_count = 10;


	// add new anon session, lives for 1 minute max to prevent backup
	session = BP_HTTPdSessionAdd
	(
			httpd,
			BP_HTTPD_USER_SESSION_TYPE_ANONYMOUS_GUEST,
			user_lookup,
			BP_MINUTE,
			default_xsrf_token_count
	);

	printf("\n Looked up user ok?: %p - %s", user_lookup, user_lookup->username);


	// return the session
	return session;

}

// Attempts to lookup request handler within the table.
P_BP_HTTPD_USER_SESSION BP_HTTPdSessionLookup
(
		P_BP_HTTPD_SESSION httpd,
		char *             session_id
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

	// Ensure we have a handler
	if(!session_id)
		return NULL;

	// check sanity on non-optional values
	if(BP_StringIsReasonablePrintableString(session_id, bpstrlen(session_id), BP_FALSE, BP_FALSE) != BP_TRUE)
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
	table_path[0] = "sessions";
	table_path[1] = session_id;
	table_path[2] = NULL;

	// attempt to get entry
	lookup_entry = BP_HashRegLookupTableEntryByPath(httpd->system_hreg, table_path);

	// ensure we can lookup the new entry
	if(!lookup_entry)
	{
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(httpd->system_hreg, NULL);
	}

	// set session from lookup
	P_BP_HTTPD_USER_SESSION session = (P_BP_HTTPD_USER_SESSION) lookup_entry->data;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Unlock and Exit %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section before exiting
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(httpd->system_hreg);

	// return null if the record cannot be found
	return session;

}

// Returns BP_TRUE if the session has expired, or BP_FALSE if it's
// still valid.
BP_BOOL BP_HTTPdSessionExpired
(
	P_BP_HTTPD_USER_SESSION user_session
)
{

	// session is automatically expired if null
	if(!user_session)
		return BP_TRUE;

	// if we have life left, return false
	if(user_session->life_in_seconds >= (BP_TimeGetSecondsSinceUnixEpoch() - user_session->issued_time))
		return BP_FALSE;

	// return true
	return BP_TRUE;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Session Persistent (cache) Data %%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Attempts to add persistent data to a session
BP_ERROR_T BP_HTTPdSessionAddPersistentData
(
	P_BP_HTTPD_USER_SESSION user_session,
	char                   *key,
	void                   *data,
	size_t                  data_len,
	char                   *data_description,
	char                   *data_type
)
{

	// return indicating success
	return ERR_SUCCESS;

}

// Attempts to lookup session persistent data by UUID
P_BP_HTTPD_USER_SESSION_PERSISTENT_DATA BP_HTTPdSessionLookupDataByKey
(
	P_BP_HTTPD_USER_SESSION user_session,
	char                   *key
)
{

	return NULL;
}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Add XSRF Tokens %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Adds an arbitrary number of valid xsrf tokens to a user session.
BP_ERROR_T BP_HTTPdSessionXSRFTokensAdd
(
	P_BP_HTTPD_USER_SESSION user_session,
	size_t                  token_count
)
{

	// run basic zero/null checks
	if(!user_session)
		return ERR_FAILURE;
	if(!token_count)
		return ERR_FAILURE;

	// gonna set token count to a 1k entries max to prevent developer
	// mishaps.
	if(token_count > BP_KILOBYTE)
		return ERR_FAILURE;

	// if the api key already exists, exit immediately
	//if(BP_HTTPdSessionLookupAPIKeyByIdentifier(httpd, key_identifier))
	//	BP_HTTPD_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);



	// generate xsrf tokens linearly
	size_t token_gen_iter = 0;
	for
	(
		;
		token_gen_iter < token_count;
		token_gen_iter++
	)
	{

		printf("\n [!!] Attempting to add XSRF token here?!");
		// add new session
		BP_HTTPdSessionXSRFTokenAdd(user_session);

	}

	// return indicating success
	return ERR_SUCCESS;

}


// Adds one xsrf token to a user session.
BP_ERROR_T BP_HTTPdSessionXSRFTokenAdd
(
		P_BP_HTTPD_USER_SESSION user_session
)
{

	// ensure we have a user session and a xsrf_registry
	if(!user_session)
		return ERR_FAILURE;
	if(!user_session->xsrf_reg)
		return ERR_FAILURE;

	// generate a new xsrf token
	char *new_xsrf_token_uuid = BP_GenRandomUUID();



	// attempt lookup to ensure xsrf token doesn't already exist
	P_BP_HTTPD_SESSION_XSRF_TOKEN lookup_token = BP_HTTPdSessionXSRFTokenLookup
	(
			user_session,
			new_xsrf_token_uuid
	);

	// ensure the token doesn't already exist
	if(lookup_token)
	{
		bpfree(new_xsrf_token_uuid);
		return ERR_FAILURE;
	}


	// xsrf_tokens
	char *new_xsrf_token_add_path[10];
	BP_ZERO_STACK(new_xsrf_token_add_path);

	// set path for lookup
	new_xsrf_token_add_path[0] = "tokens";
	// new_xsrf_token_add_path[1] = new_xsrf_token_uuid;

	// attempt to add new BP_HTTPD_SESSION_XSRF_TOKEN structure to the table using the uuid for a name
	BP_HashRegAddDataToTableByPath
	(
			user_session->xsrf_reg,
			(char **) &new_xsrf_token_add_path,
			new_xsrf_token_uuid,
			BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
			NULL,
			sizeof(BP_HTTPD_SESSION_XSRF_TOKEN)
	);


	// attempt to lookup table entry after addition
	P_BP_HASH_TABLE_KEYED_ENTRY table_entry_lookup = BP_HashRegLookupTableEntryByPath(user_session->xsrf_reg, (char **) &new_xsrf_token_add_path);
	if(!table_entry_lookup)
	{

		// return indicating failure
		bpfree(new_xsrf_token_uuid);
		return ERR_FAILURE;

	}

	// set xsrf token from lookup
	P_BP_HTTPD_SESSION_XSRF_TOKEN new_xsrf_token = (P_BP_HTTPD_SESSION_XSRF_TOKEN) table_entry_lookup->data;

	// duplicate token
	new_xsrf_token->token = (char *) ht_strdup(user_session->xsrf_reg, new_xsrf_token_uuid);

	// destroy token uuid
	bpfree(new_xsrf_token_uuid);

	// return indicating success
	return ERR_SUCCESS;

}

// Checks to see if an xsrf token matches one generated
P_BP_HTTPD_SESSION_XSRF_TOKEN BP_HTTPdSessionXSRFTokenLookup
(
		P_BP_HTTPD_USER_SESSION user_session,
		char                   *xsrf_token
)
{


	// basic null checks
	if(!user_session)
		return NULL;
	if(!xsrf_token)
		return NULL;

	// ensure the token is a uuid
	if(!BP_StringIsUUID(xsrf_token))
		return NULL;

	// xsrf_tokens
	char *new_xsrf_token_lookup_path[10];
	BP_ZERO_STACK(new_xsrf_token_lookup_path);

	// set path for lookup
	new_xsrf_token_lookup_path[0] = "tokens";
	new_xsrf_token_lookup_path[1] = xsrf_token;

	// attempt to lookup the entry
	P_BP_HASH_TABLE_KEYED_ENTRY lookup_entry = BP_HashRegLookupTableEntryByPath(user_session->xsrf_reg, (char **) new_xsrf_token_lookup_path);
	if(!lookup_entry)
		return NULL;
	if(!lookup_entry->data)
		return NULL;

	// return the looked up data if we got a lookup
	return (P_BP_HTTPD_SESSION_XSRF_TOKEN) lookup_entry->data;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% API Keys %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// attempts to add an api key to a user session within the hash registry
BP_ERROR_T BP_HTTPdSessionAddAPIKey
(
	P_BP_HTTPD_SESSION           httpd,
	P_BP_HTTPD_USER_SESSION      user_session,
	char *                       key_identifier,
	P_BP_HTTPD_API_KEY_PRIVS privs
)
{

	// ensure we have basic parameters
	if(!httpd)
		return ERR_FAILURE;
	if(!user_session)
		return ERR_FAILURE;
	if(!key_identifier)
		return ERR_FAILURE;
	if(!privs)
		return ERR_FAILURE;

	// run reasonable check on identifier
	if(!BP_StringIsReasonablePrintableString(key_identifier, bpstrlen(key_identifier), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// enter critical section
	BP_HTTPD_ENTER_CRITICAL_SECTION;


	// if the api key already exists, exit immediately
	if(BP_HTTPdSessionLookupAPIKeyByIdentifier(httpd, key_identifier))
		BP_HTTPD_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Attempt API Key Addition %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// set table addition path
	char * table_addition_path[]
	{
		"api_keys",
		NULL
	};

	// add api key to table
	BP_ERROR_T err_ret = BP_HashRegAddDataToTableByPath
	(
		httpd->system_hreg,
		(char **) &table_addition_path,
		key_identifier,
		BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
		NULL,
		sizeof(BP_HTTPD_API_KEY)
	);

	// if api key addition failed, return immediately
	if(!err_ret)
	{
		BP_HTTPD_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);
	}

	// ensure we can lookup the key before returning
	P_BP_HTTPD_API_KEY new_key = BP_HTTPdSessionLookupAPIKeyByIdentifier
	(
		httpd,
		key_identifier
	);


	// api key identifier
	new_key->key_identifier = (char *) ht_strndup(httpd->system_hreg, key_identifier, bpstrlen(key_identifier));

	// set user session
	new_key->user_session = (P_BP_HTTPD_USER_SESSION)  user_session;

	// copy in privileges
	memcpy(&new_key->privs, privs, sizeof(BP_HTTPD_API_KEY_PRIVS));


	// ensure we have looked up our key ok
	if(!new_key)
		BP_HTTPD_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(ERR_FAILURE);

	// set apikey in session here
	user_session->api_key = new_key;

	// leave critical section
	BP_HTTPD_LEAVE_CRITICAL_SECTION;


	// return indicating success
	return ERR_SUCCESS;

}

// searches a httpd for an api key (hash table search, very fast)
P_BP_HTTPD_API_KEY BP_HTTPdSessionLookupAPIKeyByIdentifier
(
	P_BP_HTTPD_SESSION httpd,
	char * key_identifier
)
{

	// ensure we have a httpd and a key identifier
	if(!httpd)
		return NULL;
	if(!key_identifier)
		return NULL;

	// ensure the identifier is reasonable
	if(!BP_StringIsReasonablePrintableString(key_identifier, bpstrlen(key_identifier), BP_FALSE, BP_FALSE))
		return NULL;

	// enter critical section
	BP_HTTPD_ENTER_CRITICAL_SECTION;

	// set lookup path
	char * lookup_path[]
	{
		"api_keys",
		key_identifier,
		NULL
	};

	// attempt to lookup api key
	P_BP_HASH_TABLE_KEYED_ENTRY api_key_entry = BP_HashRegLookupTableEntryByPath(httpd->system_hreg, (char **) &lookup_path);
	if(!api_key_entry)
		BP_HTTPD_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	// set api key from data
	P_BP_HTTPD_API_KEY api_key = (P_BP_HTTPD_API_KEY) api_key_entry->data;
	if(!api_key)
		BP_HTTPD_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(NULL);

	// leave critical section
	BP_HTTPD_LEAVE_CRITICAL_SECTION;

	// return looked up key
	return api_key;

}


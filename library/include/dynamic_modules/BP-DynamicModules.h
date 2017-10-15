#ifndef __BP_DYNAMIC_MODULES_H
#define __BP_DYNAMIC_MODULES_H

// these hooks are passed back to the loader on load event, at which point the
// module has full control of the application and can hook in however they need.
// The event loop is re-established after the
typedef struct __BP_DYNAMIC_MODULE_LOAD_HOOKS {

	// event manager is set here if one is being passed in (null if not provided)
	P_BP_EVENT_MANAGER event_manager;

	// httpd session  is set here if one is being passed in (null if not provided)
	P_BP_HTTPD_SESSION httpd_session;

} BP_DYNAMIC_MODULE_LOAD_HOOKS, *P_BP_DYNAMIC_MODULE_LOAD_HOOKS;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Module Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

typedef enum BP_DYNAMIC_MODULE_STATE {
	BP_DYNAMIC_MODULE_STATE_UNSET,
	BP_DYNAMIC_MODULE_STATE_UNLOADED,
	BP_DYNAMIC_MODULE_STATE_LOADED_OK,
	BP_DYNAMIC_MODULE_STATE_LOAD_FAILED
};

// Define the dynamic module structure.
typedef struct __BP_DYNAMIC_MODULE {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Mod Load Information %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// system hash table registry
	P_BP_HASH_TABLE_REGISTRY system_hreg;

	// load hooks
	BP_DYNAMIC_MODULE_LOAD_HOOKS load_hooks;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Module Load Information In-Out %%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if the module was loaded ok, this is set
	BP_BOOL loaded_ok;

	// if a module whitelist is in place
	BP_BOOL file_sha1_matched_in_whitelist;

	// --- generic loader fail info --------------

	// --- "already loaded" flags ----------------

	// set to true if our load check failed
	BP_BOOL load_check_failed;

	// if the module is already loaded
	BP_BOOL already_loaded;

	// if the already matched module matches our bin sha1, this is set
	BP_BOOL already_loaded_matched_bin_sha1;

	// this is set to true if the module has no sha1 property within the ght
	BP_BOOL already_loaded_module_has_no_sha1;

	// if the already loaded module had the same module name
	BP_BOOL already_loaded_matched_module_name;

	// set if a module in our registry matches this filesystem path
	BP_BOOL already_loaded_matched_module_filesystem_path;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Module Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// module name
	char *so_module_name;

	// handle to the module
	void *so_handle;

	// handle to module configuration json (copied locally and sanitized before parse)
	char *so_module_config_json;

	// loaded module path
	char *so_module_load_path;

	// sha1 hash of the module
	char *so_module_filesystem_sha1_hash;

	// current state of the module
	BP_DYNAMIC_MODULE_STATE state;

} BP_DYNAMIC_MODULE, *P_BP_DYNAMIC_MODULE;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Entry Points %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Module entry point define.
#define BP_MODULE_ENTRY_POINT \
	BP_ERROR_T BP_ModuleEntryPoint(P_BP_DYNAMIC_MODULE_LOAD_HOOKS load_hooks)


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Module Creation Simplificiation Macros %%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Anonymous user request handler setup prologue.
#define BP_MODULE_ANONYMOUS_USER_REQUEST_HANDLER_SETUP\
		/* attempt setup of request (required for all secure requests) */												\
        BP_ERROR_T req_setup_result = BP_HTTPDRequestHandlerSecureAuthEntrySetupJSON(BP_HTTPD_ACCESS_HANDLER_PARAMS);	\
        if(!req_setup_result)																							\
                return ERR_FAILURE;																						\
                																										\
		/* convert type so we can have an easier time using it */														\
        P_BP_HTTPD_REQUEST req = (P_BP_HTTPD_REQUEST) request;															\
        																												\
		/* set usersession */																							\
        P_BP_HTTPD_USER_SESSION user_session = (P_BP_HTTPD_USER_SESSION) req->session;									\
        																												\
																														\
		/* ensure we have extra data (this pointer will be to our webchat structure) */									\
        if(!req->httpd->extra_data)																						\
        {																												\
                return BP_HTTPdRequestHandlerRespondJSONFromStaticText(req, ERR_FAILURE, "{\n\t\"command_msg\": \"no_webchat_set\"\n}");\
        }																												\

// basic user request handler setup
#define BP_MODULE_BASIC_USER_REQUEST_HANDLER_SETUP \
		/* attempt setup of request (required for all secure requests) */												\
        BP_ERROR_T req_setup_result = BP_HTTPDRequestHandlerSecureAuthEntrySetupJSON(BP_HTTPD_ACCESS_HANDLER_PARAMS);	\
        if(!req_setup_result)																							\
                return ERR_FAILURE;																						\
                																										\
		/* convert type so we can have an easier time using it */														\
        P_BP_HTTPD_REQUEST req = (P_BP_HTTPD_REQUEST) request;															\
        																												\
		/* set usersession */																							\
        P_BP_HTTPD_USER_SESSION user_session = (P_BP_HTTPD_USER_SESSION) req->session;									\
        																												\
																														\
		/* ensure we have extra data (this pointer will be to our webchat structure) */									\
        if(!req->httpd->extra_data)																						\
        {																												\
                return BP_HTTPdRequestHandlerRespondJSONFromStaticText(req, ERR_FAILURE, "{\n\t\"command_msg\": \"no_webchat_set\"\n}");\
        }																												\
																														\
		/* anonymous guests aren't allowed */																			\
        if(user_session->user->type == BP_HTTPD_USER_TYPE_ANONYMOUS_GUEST)												\
        {																												\
                return BP_HTTPdRequestHandlerRespondJSONFromStaticText(req, ERR_FAILURE, "{\n\t\"command_msg\": \"anonymous_guest_forbidden\"\n}");\
        }																												\
																														\
        /* this api endpoint is xsrf protected */																		\
        if(!req->xsrf_checks_passed_ok)																					\
        {																												\
                return BP_HTTPdRequestHandlerRespondJSONFromStaticText(req, ERR_FAILURE, "{\n\t\"command_msg\": \"xsrf_check_failed\"\n}");\
        }



// Admin user request handler setup.
#define BP_MODULE_ADMIN_USER_REQUEST_HANDLER_SETUP \
		/* attempt setup of request (required for all secure requests) */												\
        BP_ERROR_T req_setup_result = BP_HTTPDRequestHandlerSecureAuthEntrySetupJSON(BP_HTTPD_ACCESS_HANDLER_PARAMS);	\
        if(!req_setup_result)																							\
                return ERR_FAILURE;																						\
                																										\
		/* convert type so we can have an easier time using it */														\
        P_BP_HTTPD_REQUEST req = (P_BP_HTTPD_REQUEST) request;															\
        																												\
		/* set usersession */																							\
        P_BP_HTTPD_USER_SESSION user_session = (P_BP_HTTPD_USER_SESSION) req->session;									\
        																												\
																														\
		/* ensure we have extra data (this pointer will be to our webchat structure) */									\
        if(!req->httpd->extra_data)																						\
        {																												\
                return BP_HTTPdRequestHandlerRespondJSONFromStaticText(req, ERR_FAILURE, "{\n\t\"command_msg\": \"no_webchat_set\"\n}");\
        }																												\
																														\
		/* anonymous guests aren't allowed */																			\
        if(user_session->user->type == BP_HTTPD_USER_TYPE_ANONYMOUS_GUEST)												\
        {																												\
                return BP_HTTPdRequestHandlerRespondJSONFromStaticText(req, ERR_FAILURE, "{\n\t\"command_msg\": \"anonymous_guest_forbidden\"\n}");\
        }																												\
																														\
        /* this api endpoint is xsrf protected */																		\
        if(!req->xsrf_checks_passed_ok)																					\
        {																												\
                return BP_HTTPdRequestHandlerRespondJSONFromStaticText(req, ERR_FAILURE, "{\n\t\"command_msg\": \"xsrf_check_failed\"\n}");\
        }


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Module Constructor or Destructor %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// module initializer
#define BP_MOD_INIT(mod_name)\
	__attribute__((constructor)) void BP_MODINIT_##mod_name(void)

// module unload routine
#define BP_MOD_FINI(mod_name)\
	__attribute__((destructor)) void BP_MODFINI_##mod_name(void)

// dynamic module loader entry point
#define BP_MOD_ENTRY(mod_name)\
	BP_ERROR_T BP_MODENTRY_##mod_name(P_BP_DYNAMIC_MODULE mod_load_hooks)


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% GHT Module Hooks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// All loaded modules are kept referenced in the GHT.  These routines allow us to modify
// the GHT to hold loaded modules.

// Checks to see if a dynamic module is already loaded in the GHT.
BP_BOOL BP_DynamicModuleIsAlreadyLoadedInGHT(P_BP_DYNAMIC_MODULE module);

// Attempt to add a module into the "/dynamic_modules" section of the GHT.  Will return false
// if the module is already loaded or load cannot
BP_ERROR_T BP_DynamicModuleLoadIntoGHT(P_BP_DYNAMIC_MODULE module);

// Attempt to unload the dynamic module from the GHT by name (module names must be unique)
BP_ERROR_T BP_DynamicModuleUnloadFromGHTByName(char *module_name);



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
		P_BP_JSON                module_config
);


#endif /* __BP_DYNAMIC_MODULES_H */

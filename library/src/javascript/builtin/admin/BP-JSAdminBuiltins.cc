/*
 * BP-JSAdminBuiltins.cc
 *
 *  Created on: May 17, 2015
 *      Author: root
 */


// blockparty main include
#include "../../include/BP-Main.h"


// functions that are only available to local admin managers
JSFunctionSpec BP_JS_GLOBAL_ADMIN_PRIV_FUNCTION_TABLE[] =
{
		JS_FS("BP_JSSystemShellExec", BP_JSSystemShellExec, 1, 0),
		JS_FS_END
};

// === Admin Priv Functions ================================

static bool BP_JSSystemShellExec(JSContext *cx, unsigned argc, jsval *vp)
{

	// get arguments
	JS::CallArgs args = CallArgsFromVp(argc, vp);


	// display the running message
	printf("\n [+] BP_JSSystemShellExec running.");

	// exit callback
	return true;

}

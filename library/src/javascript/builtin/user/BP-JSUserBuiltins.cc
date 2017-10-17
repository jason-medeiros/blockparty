/*
 * BP-JSUserBuiltins.cc
 *
 *  Created on: May 17, 2015
 *      Author: root
 */

// include
#include "../../../../include/BP-Main.h"

// functions that are available to users
JSFunctionSpec BP_JS_GLOBAL_USER_PRIV_FUNCTION_TABLE[] =
{
		JS_FS("BP_JSReadThreadRegistryByPath", BP_JSReadThreadRegistryByPath, 1, 0),
		JS_FS_END
};


// === User Priv Functions =================================

static bool BP_JSReadThreadRegistryByPath(JSContext *cx, unsigned argc, jsval *vp)
{

	// get arguments
	JS::CallArgs args = CallArgsFromVp(argc, vp);

	// display run message
	printf("\n [+] BP_JSReadThreadRegistryByPath running.");

	// exit
	return true;

}

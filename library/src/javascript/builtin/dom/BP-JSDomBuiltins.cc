/*
 * BP-JSDomBuiltins.cc
 *
 *  Created on: May 17, 2015
 *      Author: root
 */


// blockparty main include
#include "../../include/BP-Main.h"

// functions that are only available to local DOM managers
JSFunctionSpec BP_JS_GLOBAL_DOM_PRIV_FUNCTION_TABLE[] =
{
		JS_FS("BP_JSDomWorkerStart", BP_JSDomWorkerStart, 1, 0),
		JS_FS_END
};



static bool BP_JSDomWorkerStart(JSContext *cx, unsigned argc, jsval *vp)
{

	// get arguments
	JS::CallArgs args = CallArgsFromVp(argc, vp);

	// display run message
	printf("\n [+] BP_JSDomWorkerStart running.");

	// exit
	return true;

}

/*
 * BP-LinuxSignalAutoconfigurations.cc
 *
 *  Created on: 21/12/2013
 *      Author: root
 */

/*
 * DO NOT USE THESE ROUTINES UNLESS YOU KNOW WHAT YOU ARE DOING.  SOME DISTRIBUTIONS
 * DO NOT ALLOW A PROCESS TO ATTACH A DEBUGGER TO THEMSELVES (aka ubuntu), SO BE
 * AWARE THAT PROBLEMS MAY ARISE WITH USE.
 *
 */


// include main functions
#include "../../include/BP-Main.h"

// Run this method to install the BP_LSAC_GDBOnSigSegv() signal handler
// which automatically calls gdb when a sigsegv is encountered.
BP_ERROR_T BP_LSAC_InstallGDBOnSigSegv()
{

	// install function handler
	signal(SIGSEGV, &BP_LSAC_GDBOnSigSegv);

	// return indicating success
	return ERR_SUCCESS;

}

// Called whenever a blockparty application encounters a segmentation fault.
void BP_LSAC_GDBOnSigSegv(int signum)
{

	// create new buffer
	char buff[1024] = {0};

	// create the buffer
	snprintf(buff, sizeof(buff)-1, "gdb --pid=%d", getpid());

	// exit here
	exit(0);

}

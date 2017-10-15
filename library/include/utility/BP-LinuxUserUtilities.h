/*
 * BP-LinuxUserUtilities.h
 *
 *  Created on: Jul 23, 2015
 *      Author: root
 */

#ifndef INCLUDE_UTILITY_BP_LINUXUSERUTILITIES_H_
#define INCLUDE_UTILITY_BP_LINUXUSERUTILITIES_H_

// Returns a bpcalloc'ed printable string version of a username.
char * BP_LinuxUserGetUsernameFromUid(uid_t uid);

// Returns a bpcalloc'ed printable string version of a group name
char * BP_LinuxUserGetGroupnameFromGid(gid_t gid);


#endif /* INCLUDE_UTILITY_BP_LINUXUSERUTILITIES_H_ */

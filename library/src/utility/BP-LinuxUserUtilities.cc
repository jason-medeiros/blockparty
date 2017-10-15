/*
 * BP-LinuxUserUtilities.cc
 *
 *  Created on: Jul 23, 2015
 *      Author: root
 */


// include main functions
#include "../../include/BP-Main.h"


// Returns a bpcalloc'ed printable string version of a username.
char * BP_LinuxUserGetUsernameFromUid(uid_t uid)
{

	// get pw buffsize
	size_t bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
	if (bufsize == -1)          /* Value was indeterminate */
		bufsize = 16384;        /* Should be more than enough */

	// allocate space
	char * buf = (char *) bpcalloc(bufsize, 1);
	if (!buf)
		return NULL;

	// set pwd and result
    struct passwd pwd = {0};
    struct passwd *result = NULL;

    // attempt to get the user data
	int s = getpwuid_r(uid, &pwd, buf, bufsize, &result);

	if (result == NULL)
	{
		if (s == 0)
		{
			bpfree(buf);
			return NULL;
		}
		else
		{
			bpfree(buf);
			return NULL;
		}

	}

	// duplicate the string
	char * user_final = bpstrdup(pwd.pw_gecos);

	// destroy buf if its set
	if(buf)
		bpfree(buf);

	// return the username
	return user_final;

}

// Returns a bpcalloc'ed printable string version of a group name
char * BP_LinuxUserGetGroupnameFromGid(gid_t gid)
{

	// create struct
	struct group g;
	BP_ZERO_STACK(g);

	// group pointer
	struct group * grp_ptr                = &g;
	struct group * tmp_grp_ptr            = NULL;
	char           grp_buffer[NAME_MAX]   = {0};

	// attempt to gather the group
	int ret_val = getgrgid_r(gid, grp_ptr, grp_buffer, NAME_MAX-1, &tmp_grp_ptr );

	// any non-zero value is error
	if(ret_val)
	{
		return NULL;
	}

	// set group name string
	char * group_name_string = bpstrdup(grp_buffer);

	// return the group name string
	return group_name_string;

}


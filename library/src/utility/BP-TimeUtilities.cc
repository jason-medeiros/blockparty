// include main blockparty library include
#include "../include/BP-Main.h"

// Returns the number of seconds since the unix epoch in a size_t
BP_SECONDS_SINCE_UNIX_EPOCH BP_TimeGetSecondsSinceUnixEpoch()
{

	// attempt to retrieve time (time_t is a long int)
	time_t time_since_epoch = time(NULL);

	// ensure we didn't somehow get erroneous data
	if(time_since_epoch <= 0)
		return 0;

	// assign value to unsigned
	BP_SECONDS_SINCE_UNIX_EPOCH seconds_since_epoch = time_since_epoch;

	// return 0 if we
	return seconds_since_epoch;

}

// Returns the number of seconds since the unix epoch in a size_t
time_t BP_TimeGetSecondsSinceUnixEpochAsTimeT()
{

	// attempt to retrieve time (time_t is a long int)
	time_t time_since_epoch = time(NULL);

	// ensure we didn't somehow get erroneous data
	if(time_since_epoch <= 0)
		return 0;

	// return the time value
	return time_since_epoch;

}

// Returns a formatted string from timestamps.
char * BP_TimeConvertTimeTToAscii(time_t * time_in)
{

	// create buffer
	char * out_buff = (char *) bpstrdup(ctime(time_in));

	// if buffer failed creation, exit here
	if(!out_buff)
		return NULL;

	// if the buffer is zero length, exit here
	if(!strnlen(out_buff, 10))
	{

		// still need to free it, as it was allocated OK
		bpfree(out_buff);
		return NULL;

	}

	// return the converted time string
	return out_buff;

}

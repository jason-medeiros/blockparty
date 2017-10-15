#ifndef __BP_TIME_UTILITIES_H
#define __BP_TIME_UTILITIES_H


// create a typedef
typedef size_t BP_SECONDS_SINCE_UNIX_EPOCH;

// Returns the number of seconds since the unix epoch in a size_t
BP_SECONDS_SINCE_UNIX_EPOCH BP_TimeGetSecondsSinceUnixEpoch();

// Returns the number of seconds since the unix epoch in a time_t
time_t BP_TimeGetSecondsSinceUnixEpochAsTimeT();

// Returns a formatted string from timestamps.
char * BP_TimeConvertTimeTToAscii(time_t * time_in);


#endif // __BP_TIME_UTILITIES_H


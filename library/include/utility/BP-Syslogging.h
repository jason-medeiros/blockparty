/*
 * BP-Syslogging.h
 *
 *  Created on: Nov 10, 2011
 *      Author: root
 */

#ifndef BPSYSLOGGING_H_
#define BPSYSLOGGING_H_

// Logging functionality.  Uses syslog if the EAB_LOG_TO_SYSLOG
// flag is set.  If not, it just prints the message out to the screen.
// NOTE: NEVER PASS THIS FUNCTION ANY DATA THAT CAN CONTAIN FORMAT
// STRING CHARACTERS.  DATA GOES DIRECTLY TO syslog().
void BP_LogSyslog(char *log_msg, char *syslog_session_name, size_t syslog_option, size_t syslog_priority);


#endif /* BPSYSLOGGING_H_ */

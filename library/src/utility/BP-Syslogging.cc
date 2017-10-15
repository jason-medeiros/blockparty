
// include main functions
#include "../../include/BP-Main.h"


// Logging functionality.  Uses syslog if the EAB_LOG_TO_SYSLOG
// flag is set.  If not, it just prints the message out to the screen.
// NOTE: NEVER PASS THIS FUNCTION ANY DATA THAT CAN CONTAIN FORMAT
// STRING CHARACTERS.  DATA GOES DIRECTLY TO syslog()!
void BP_LogSyslog(char *log_msg, char *syslog_session_name, size_t syslog_option, size_t syslog_priority)
{

	// opensyslog
	openlog(syslog_session_name, syslog_option,LOG_LOCAL1);

	// log message
	syslog(syslog_priority, log_msg);

	// close log
	closelog();

	// exit routine (no return eax required)
	return;

}


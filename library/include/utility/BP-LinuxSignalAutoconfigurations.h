#ifndef BPLINUXSIGNALAUTOCONFIGURATIONS_H_
#define BPLINUXSIGNALAUTOCONFIGURATIONS_H_


// Run this method to install the BP_LSAC_GDBOnSigSegv() signal handler
// which automatically calls gdb when a sigsegv is encountered.
BP_ERROR_T BP_LSAC_InstallGDBOnSigSegv();

// this is the actual handler function; creates
// a gdb processes and attaches automatically.
void BP_LSAC_GDBOnSigSegv(int signum);



#endif /* BPLINUXSIGNALAUTOCONFIGURATIONS_H_ */

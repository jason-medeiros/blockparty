/*
 * BP-LinuxTerminal.h
 *
 *  Created on: Aug 2, 2014
 *      Author: root
 */

#ifndef BP_LINUXTERMINAL_H_
#define BP_LINUXTERMINAL_H_

// only add this code if we're building on linux
#ifdef __GNUC__
#ifdef __linux__



// Terminal Information structure
typedef struct __BP_LINUX_TERMINAL_INFO {


	// Flag indicating whether or not the data we're getting is for
	// the current terminal.
	BP_BOOL is_current_terminal;

	// tail queue allocator associated with this terminal information
	P_BP_LINKL tq;

	// window size
	winsize w;

	// file descrptor associated with this terminal information
	size_t file_descriptor;

	// flag indicating whether or not the provided fd is a tty (determined by isatty(fd))
	BP_BOOL file_descriptor_is_tty;

	// tty name gathered fropm ttyname(fd);
	char *tty_name;

	// if the information was gathered by opening a dev file
	// this is the file  name used
	char * device_file_name;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Termios Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// termios data
	struct termios termios_info;

	// set to BP_TRUE if the termios request retrieved data successfully
	BP_BOOL termios_retrieved_successfully;



} BP_LINUX_TERMINAL_INFO, *P_BP_LINUX_TERMINAL_INFO;




// Gets information about the terminal which is currently being used.
P_BP_LINUX_TERMINAL_INFO BP_CreateCurrentTerminalInformationLinux();

// Gets information about the terminal which is currently being used (retrieves data by file name)
P_BP_LINUX_TERMINAL_INFO BP_CreateTerminalInformationByDeviceFileLinux(char * device_file_name);

// All terminal information requests fall back to this.
P_BP_LINUX_TERMINAL_INFO BP_CreateTerminalInformationByFDLinux(int fd);

// Destroys all data within a P_BP_LINUX_TERMINAL_INFO structure, including the
// structure itself.
BP_ERROR_T BP_DestroyTerminalInformationLinux(P_BP_LINUX_TERMINAL_INFO terminal_info);

// Displays Terminal Information
BP_ERROR_T BP_DisplayTerminalInformationLinux(P_BP_LINUX_TERMINAL_INFO terminal_info, BP_BOOL display_termios_info);

// Displays termios information
BP_ERROR_T BP_DisplayTermiosInformation(struct termios * termios_data);


#endif // __linux__
#endif //__GNUC__


#endif /* BP_LINUXTERMINAL_H_ */

/*
 * BP-LinuxTerminal.cc
 *
 *  Created on: Aug 2, 2014
 *      Author: root
 */

// main blockparty include
#include "../include/BP-Main.h"

// only add this code if we're building on linux
#ifdef __GNUC__
#ifdef __linux__


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Simple Utility Routines Used to Retrieve Linux Terminal Information %%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Gets information about the terminal which is currently being used.
P_BP_LINUX_TERMINAL_INFO BP_CreateCurrentTerminalInformationLinux()
{
	// attempt to create space for the terminal information
	P_BP_LINUX_TERMINAL_INFO terminal_info = (P_BP_LINUX_TERMINAL_INFO) bpcalloc(sizeof(BP_LINUX_TERMINAL_INFO), 1);
	if(!terminal_info)
		return NULL;

	// create a new linkl for allocations
	terminal_info->tq = BP_CreateLinkL("terminfo", BP_FALSE);

	// set this if we've determined that the file descriptor is a tty
	if(isatty(0))
		terminal_info->file_descriptor_is_tty = BP_TRUE;

	// get the tty name
	char *tty_name = ttyname(0);
	if(tty_name)
		terminal_info->tty_name = tq_strndup(terminal_info->tq, tty_name, 1024, BPLN);

	// set the info flag to true
	terminal_info->is_current_terminal = BP_TRUE;

	// attempt to get terminal information via ioctl
	ioctl(0, TIOCGWINSZ, &terminal_info->w);

	// attempt to get termios information, set flag if we got the info ok
	if(tcgetattr(0, &terminal_info->termios_info) == 0)
		terminal_info->termios_retrieved_successfully = BP_TRUE;

	// return the terminal information
	return terminal_info;

}

// Gets information about the terminal which is currently being used (retrieves data by file name)
P_BP_LINUX_TERMINAL_INFO BP_CreateTerminalInformationByDeviceFileLinux(char * device_file_name)
{
	// attempt to create space for the terminal information
	P_BP_LINUX_TERMINAL_INFO terminal_info = (P_BP_LINUX_TERMINAL_INFO) bpcalloc(sizeof(BP_LINUX_TERMINAL_INFO), 1);
	if(!terminal_info)
		return NULL;


	// attempt to open the device
	int fd = open(device_file_name, O_RDWR);

	// create a new linkl for allocations
	terminal_info->tq = BP_CreateLinkL("terminfo", BP_FALSE);

	// set this if we've determined that the file descriptor is a tty
	if(isatty(fd))
		terminal_info->file_descriptor_is_tty = BP_TRUE;

	// get the tty name
	char *tty_name = ttyname(fd);
	if(tty_name)
		terminal_info->tty_name = tq_strndup(terminal_info->tq, tty_name, 1024, BPLN);

	// set file descriptor as fd
	terminal_info->file_descriptor = fd;

	// set the info flag to true
	if((fd == 0) || (fd == 1) || (fd==2) )
		terminal_info->is_current_terminal = BP_TRUE;

	// attempt to get terminal information via ioctl
	ioctl(fd, TIOCGWINSZ, &terminal_info->w);

	// attempt to get termios information, set flag if we got the info ok
	if(tcgetattr(fd, &terminal_info->termios_info) == 0)
		terminal_info->termios_retrieved_successfully = BP_TRUE;

	// now close the file descriptor which was opened earlier
	close(fd);

	// return the terminal information
	return terminal_info;

}


// All terminal information requests fall back to this.
P_BP_LINUX_TERMINAL_INFO BP_CreateTerminalInformationByFDLinux(int fd)
{
	// attempt to create space for the terminal information
	P_BP_LINUX_TERMINAL_INFO terminal_info = (P_BP_LINUX_TERMINAL_INFO) bpcalloc(sizeof(BP_LINUX_TERMINAL_INFO), 1);
	if(!terminal_info)
		return NULL;



	// create a new linkl for allocations
	terminal_info->tq = BP_CreateLinkL("terminfo", BP_FALSE);

	// set this if we've determined that the file descriptor is a tty
	if(isatty(fd))
		terminal_info->file_descriptor_is_tty = BP_TRUE;

	// get the tty name
	char *tty_name = ttyname(fd);
	if(tty_name)
		terminal_info->tty_name = tq_strndup(terminal_info->tq, tty_name, 1024, BPLN);

	// set file descriptor as fd
	terminal_info->file_descriptor = fd;

	// set the info flag to true
	if((fd == 0) || (fd == 1) || (fd==2))
		terminal_info->is_current_terminal = BP_TRUE;

	// attempt to get terminal information via ioctl
	ioctl(fd, TIOCGWINSZ, &terminal_info->w);

	// attempt to get termios information, set flag if we got the info ok
	if(tcgetattr(fd, &terminal_info->termios_info) == 0)
		terminal_info->termios_retrieved_successfully = BP_TRUE;

	// return the terminal information
	return terminal_info;

}

// Destroys all data within a P_BP_LINUX_TERMINAL_INFO structure, including the
// structure itself.
BP_ERROR_T BP_DestroyTerminalInformationLinux(P_BP_LINUX_TERMINAL_INFO terminal_info)
{

	// ensure we have a pointer to destroy
	if(!terminal_info)
		return ERR_FAILURE;

	// destroy the tail queue allocator if necessary
	if(terminal_info->tq)
	{
		BP_DestroyLinkL(terminal_info->tq, BP_TRUE);
	}

	// destroy the structure itself
	bpfree(terminal_info);

	// return indicating success
	return ERR_SUCCESS;

}

// Displays Terminal Information
BP_ERROR_T BP_DisplayTerminalInformationLinux(P_BP_LINUX_TERMINAL_INFO terminal_info, BP_BOOL display_termios_info)
{

	// ensure we have a pointer
	if(!terminal_info)
		return ERR_FAILURE;

	// display structure information
	printf("\n\t [+] Terminal Information (%p)", terminal_info);
	printf("\n\t     is_current_terminal:    %s", terminal_info->is_current_terminal ? "yes" : "no");
	printf("\n\t     tailqueue allocator:    %p", terminal_info->tq);
	printf("\n\t     file_descriptor:        %u", terminal_info->file_descriptor);
	printf("\n\t     device_file_name:       %s", terminal_info->device_file_name ? terminal_info->device_file_name : "unset");
	printf("\n\t     is a tty:               %s", terminal_info->file_descriptor_is_tty ? "yes" : "no");
	printf("\n\t     tty name:               %s", terminal_info->tty_name ? terminal_info->tty_name : "unset");
	printf("\n\t     terminal columns:       %u", terminal_info->w.ws_col);
	printf("\n\t     terminal rows:          %u", terminal_info->w.ws_row);
	printf("\n\t     termios retrieved ok:   %s", terminal_info->termios_retrieved_successfully ? "yes" : "no");

	// display termios info if requested
	if(display_termios_info && terminal_info->termios_retrieved_successfully)
	{

		printf("\n");
		BP_DisplayTermiosInformation(&terminal_info->termios_info);

	}

	printf("\n");

	// return indicating success
	return ERR_SUCCESS;
}

// Displays termios information (extrapolated into it's own function since
// the termios data is significant in length)
BP_ERROR_T BP_DisplayTermiosInformation(struct termios * termios_data)
{

	// ensure we have a termios structure
	if(!termios_data)
		return ERR_FAILURE;

	// display termios info
	printf("\n\t\t [-] Termios Info (%p)", termios_data);
	printf("\n\t\t   c_ispeed:  %u ", termios_data->c_ispeed);
	printf("\n\t\t   c_ospeed:  %u ", termios_data->c_ospeed);
	printf("\n\t\t   c_line:    %u ", termios_data->c_line);

	// display input mode flags
	if(termios_data->c_iflag)
	{
		printf("\n");

		printf("\n\t\t [-] c_iflag(s) set:  input mode flags ");

		if(termios_data->c_iflag & IGNBRK)
			printf("\n\t\t\t IGNBRK - Ignore break condition.");

		if(termios_data->c_iflag & BRKINT)
			printf("\n\t\t\t BRKINT - Signal interrupt on break");

		if(termios_data->c_iflag & IGNPAR)
			printf("\n\t\t\t IGNPAR - Ignore characters with parity errors.");

		if(termios_data->c_iflag & PARMRK)
			printf("\n\t\t\t PARMRK - Mark parity errors.");

		if(termios_data->c_iflag & INPCK)
			printf("\n\t\t\t INPCK - Enable input parity check.");

		if(termios_data->c_iflag & ISTRIP)
			printf("\n\t\t\t ISTRIP - Strip character.");

		if(termios_data->c_iflag & INLCR)
			printf("\n\t\t\t INLCR - Map NL to CR on input.");

		if(termios_data->c_iflag & IGNCR)
			printf("\n\t\t\t IGNCR - Ignore CR.");

		if(termios_data->c_iflag & ICRNL)
			printf("\n\t\t\t ICRNL - Map CR to NL on input.");

		if(termios_data->c_iflag & IUCLC)
			printf("\n\t\t\t IUCLC - Map upper case to lower case on input. (LEGACY)");

		if(termios_data->c_iflag & IXON)
			printf("\n\t\t\t IXON - Enable start/stop output control.");

		if(termios_data->c_iflag & IXANY)
			printf("\n\t\t\t IXANY - Enable any character to restart output.");

		if(termios_data->c_iflag & IXOFF)
			printf("\n\t\t\t IXOFF - Enable start/stop input control.");

		if(termios_data->c_iflag & IMAXBEL)
			printf("\n\t\t\t IMAXBEL - Filling terminal sends BEL character.");

		if(termios_data->c_iflag & IUTF8)
			printf("\n\t\t\t IUTF8 - Input is UTF-8 encoded.");


	}

	// display input mode flags
	if(termios_data->c_oflag)
	{
		printf("\n");

		printf("\n\t\t [-] c_oflag(s) set:  output mode flags ");

		if(termios_data->c_oflag & OPOST)
			printf("\n\t\t\t OPOST - Perform output processing.");

		if(termios_data->c_oflag & OLCUC)
			printf("\n\t\t\t OLCUC - Map lower case to upper on output.");

		if(termios_data->c_oflag & ONLCR)
			printf("\n\t\t\t ONLCR - Map NL to CR-NL on output.");

		if(termios_data->c_oflag & OCRNL)
			printf("\n\t\t\t OCRNL - Map CR to NL on output.");

		if(termios_data->c_oflag & ONOCR)
			printf("\n\t\t\t ONOCR - No CR output at column 0. ");

		if(termios_data->c_oflag & ONLRET)
			printf("\n\t\t\t ONLRET - NL performs CR function. ");

		if(termios_data->c_oflag & OFILL)
			printf("\n\t\t\t OFILL - Use fill characters for delay. ");

		if(termios_data->c_oflag & OFDEL)
			printf("\n\t\t\t OFDEL - Fill is DEL else NUL. ");

		if(termios_data->c_oflag & NLDLY)
			printf("\n\t\t\t NLDLY - Select new-line delays: NL0 NL1. ");

		if(termios_data->c_oflag & CRDLY)
			printf("\n\t\t\t CRDLY - Select carriage-return delays: CR0 CR1 CR2 CR3 ");

		if(termios_data->c_oflag & TABDLY)
			printf("\n\t\t\t TABDLY - Select horizontal-tab delays: TAB0 TAB1 TAB2 ");

		if(termios_data->c_oflag & BSDLY)
			printf("\n\t\t\t BSDLY - Select backspace delays: BS0 BS1");

		if(termios_data->c_oflag & FFDLY)
			printf("\n\t\t\t FFDLY - Select form-feed delays: FF0 FF1");

		if(termios_data->c_oflag & VTDLY)
			printf("\n\t\t\t VTDLY - Select vertical tab delays: VT0 VT1");

		if(termios_data->c_oflag & XTABS)
			printf("\n\t\t\t XTABS - Expand tabs to spaces. ");

	}

	// control mode flags
	if(termios_data->c_cflag)
	{
		printf("\n");

		printf("\n\t\t [-] c_cflag(s) set:  control mode flags ");


		if(termios_data->c_cflag & CBAUD)
		{
			printf("\n");
			printf("\n\t\t\t CBAUD - Control baud rate set (supported rates expanded below)");
			if(termios_data->c_cflag & B0)
				printf("\n\t\t\t B0 - hang up");

			if(termios_data->c_cflag & B50)
				printf("\n\t\t\t B50 - 50 baud");

			if(termios_data->c_cflag & B75)
				printf("\n\t\t\t B75 - 75 baud");

			if(termios_data->c_cflag & B110)
				printf("\n\t\t\t B110 - 110 baud");

			if(termios_data->c_cflag & B134)
				printf("\n\t\t\t B134 - 134 baud");

			if(termios_data->c_cflag & B150)
				printf("\n\t\t\t B150 - 150 baud ");

			if(termios_data->c_cflag & B200)
				printf("\n\t\t\t B200 - 200 baud");

			if(termios_data->c_cflag & B300)
				printf("\n\t\t\t B300 - 300 baud");

			if(termios_data->c_cflag & B600)
				printf("\n\t\t\t B600 - 600 baud");

			if(termios_data->c_cflag & B1200)
				printf("\n\t\t\t B1200 - 1200 baud");

			if(termios_data->c_cflag & B1800)
				printf("\n\t\t\t B1800 - 1800 baud");

			if(termios_data->c_cflag & B2400)
				printf("\n\t\t\t B2400 - 2400 baud");

			if(termios_data->c_cflag & B4800)
				printf("\n\t\t\t B4800 - 4800 baud");

			if(termios_data->c_cflag & B9600)
				printf("\n\t\t\t B9600 - 9600 baud");

			if(termios_data->c_cflag & B19200)
				printf("\n\t\t\t B19200 - 19200 baud");

			if(termios_data->c_cflag & B38400)
				printf("\n\t\t\t B38400 - 38400 baud");

			if(termios_data->c_cflag & EXTA)
				printf("\n\t\t\t EXTA - 19200 baud (redefined in termios.h)");

			if(termios_data->c_cflag & EXTB)
				printf("\n\t\t\t EXTB - 38400 baud (redefined in termios.h)");

			if(termios_data->c_cflag & B57600)
				printf("\n\t\t\t B57600 - 57600 baud");

			if(termios_data->c_cflag & B115200)
				printf("\n\t\t\t B115200 - 115200 baud");

			if(termios_data->c_cflag & B230400)
				printf("\n\t\t\t B230400 - 230400 baud");

			if(termios_data->c_cflag & B460800)
				printf("\n\t\t\t B460800 - 460800 baud");

			if(termios_data->c_cflag & B500000)
				printf("\n\t\t\t B500000 - 500000 baud");

			if(termios_data->c_cflag & B576000)
				printf("\n\t\t\t B576000 - 576000");

			if(termios_data->c_cflag & B921600)
				printf("\n\t\t\t B921600 - 921600 baud");

			if(termios_data->c_cflag & B1000000)
				printf("\n\t\t\t B1000000 - 1000000 baud");

			if(termios_data->c_cflag & B1152000)
				printf("\n\t\t\t B1152000 - 1152000 baud");

			if(termios_data->c_cflag & B1500000)
				printf("\n\t\t\t B1500000 - 1500000 baud");

			if(termios_data->c_cflag & B2000000)
				printf("\n\t\t\t B2000000 - 2000000 baud");

			if(termios_data->c_cflag & B2500000)
				printf("\n\t\t\t B2500000 - 2500000 baud");

			if(termios_data->c_cflag & B3000000)
				printf("\n\t\t\t B3000000 - 3000000 baud");

			if(termios_data->c_cflag & B3500000)
				printf("\n\t\t\t B3500000 - 3500000 baud");

			if(termios_data->c_cflag & B4000000)
				printf("\n\t\t\t B4000000 - 4000000 baud");

		}

		if(termios_data->c_cflag & CSIZE)
		{
			printf("\n");
			printf("\n\t\t\t CSIZE - Number of bits per byte (supported bits expanded below)");
			if(termios_data->c_cflag & CS5)
				printf("\n\t\t\t\t CS5 - 5bits");
			if(termios_data->c_cflag & CS6)
				printf("\n\t\t\t\t CS6 - 6bits");
			if(termios_data->c_cflag & CS7)
				printf("\n\t\t\t\t CS7 - 7bits");
			if(termios_data->c_cflag & CS8)
				printf("\n\t\t\t\t CS8 - 8bits");
		}

		if(termios_data->c_cflag & CSTOPB)
			printf("\n\t\t\t CSTOPB - Send two stop bits, if unset, send just one.");
		if(termios_data->c_cflag & CREAD)
			printf("\n\t\t\t CREAD - Enable reciever.");
		if(termios_data->c_cflag & PARENB)
			printf("\n\t\t\t PARENB - Parity enable.");
		if(termios_data->c_cflag & PARODD)
			printf("\n\t\t\t PARODD - Odd parity, if unset, even parity.");
		if(termios_data->c_cflag & HUPCL)
			printf("\n\t\t\t HUPCL - Hangup on last close.");
		if(termios_data->c_cflag & CLOCAL)
			printf("\n\t\t\t CLOCAL - Ignore modern status lines.");

		if(termios_data->c_cflag & CBAUDEX)
			printf("\n\t\t\t CBAUDEX - Extra baud speed mask. (not POSIX)");


		if(termios_data->c_cflag & CIBAUD)
			printf("\n\t\t\t CIBAUD - Mask for input speed. (not POSIX)");
		if(termios_data->c_cflag & CMSPAR)
			printf("\n\t\t\t CMSPAR - Use \"stick\" (mark/space) parity (supported on certain serial devices).");
		if(termios_data->c_cflag & CRTSCTS)
			printf("\n\t\t\t CRTSCTS - Enable RTS/CTS (hardware) flow control.");

	}

	// control mode flags
	if(termios_data->c_lflag)
	{
		printf("\n");

		printf("\n\t\t [-] c_lflag(s) set:  local mode flags ");

		if(termios_data->c_lflag & ISIG)
			printf("\n\t\t\t ISIG - If INTR,QUIT,SUSP, or DSUSP are recieved, generate corresponding signal.");
		if(termios_data->c_lflag & ICANON)
			printf("\n\t\t\t ICANON - Enable canonical mode ");
		if(termios_data->c_lflag & XCASE)
			printf("\n\t\t\t XCASE - if ICANON is set, terminal is upper case only.  If not, input is converted to lowercase except for characters preceded by a \\. (period) On output, uppercase characters are preceded by \\ and lowercase characters are converted to uppercase.");
		if(termios_data->c_lflag & ECHO)
			printf("\n\t\t\t ECHO - Echo input characters. ");
		if(termios_data->c_lflag & ECHOE)
			printf("\n\t\t\t ECHOE - If ICANON is also set, the ERASE character erases preceding input char, and WERASE, the preceding word.");
		if(termios_data->c_lflag & ECHOK)
			printf("\n\t\t\t ECHOK - If ICANON is also set, the KILL character erases the current line.");
		if(termios_data->c_lflag & ECHONL)
			printf("\n\t\t\t ECHONL - If ICANON is also set, echo NL characters even if ECHO is not set.");
		if(termios_data->c_lflag & NOFLSH)
			printf("\n\t\t\t NOFLSH - Disable flushing the input and output queues when generating signals for INT, QUIT, and SUSP chars.");
		if(termios_data->c_lflag & TOSTOP)
			printf("\n\t\t\t TOSTOP - Send the SIGTTOU signal to the process group of a background process which tries to write to its controlling terminal.");
		if(termios_data->c_lflag & ECHOCTL)
			printf("\n\t\t\t ECHOCTL - If ECHO is also set, terminal special characters are echoed as ^X (where X is ASCII char greater than 0x40)");
		if(termios_data->c_lflag & ECHOPRT)
			printf("\n\t\t\t ECHOPRT - If ICANON and ECHO are also set, characters are printed as they are being erased.");
		if(termios_data->c_lflag & ECHOKE)
			printf("\n\t\t\t ECHOKE - If ICANON is also set, KILL is echoed by erasing each character on the line, as specified by ECHOE and ECHOPRT.");
		if(termios_data->c_lflag & FLUSHO)
			printf("\n\t\t\t FLUSHO - Output is being flushed.  This flag is toggled by typing the DISCARD character.");
		if(termios_data->c_lflag & PENDIN)
			printf("\n\t\t\t PENDIN - All characters in the input queue are reprinted when the next character is read.");
		if(termios_data->c_lflag & IEXTEN)
			printf("\n\t\t\t IEXTEN - Enable implementation-defined input processing.");
		if(termios_data->c_lflag & EXTPROC)
			printf("\n\t\t\t EXTPROC - External processing. ");


	}


	// create a new character map
	P_BP_ASCII_CHARACTER_MAP char_map = BP_CreateASCIICharacterMap();
	if(!char_map)
		return ERR_FAILURE;

	printf("\n\t\t [+] Terminal Control Characters ");
	unsigned char curr_idx = 0x00;
	curr_idx = termios_data->c_cc[VINTR];
	printf("\n\t\t\t VINTR    - 0x%.02x - %s", char_map->ascii_chart[curr_idx].ascii_char, char_map->ascii_chart[curr_idx].symbol_description);

	curr_idx = termios_data->c_cc[VQUIT];
	printf("\n\t\t\t VQUIT    - 0x%.02x - %s", char_map->ascii_chart[curr_idx].ascii_char, char_map->ascii_chart[curr_idx].symbol_description);

	curr_idx = termios_data->c_cc[VERASE];
	printf("\n\t\t\t VERASE   - 0x%.02x - %s", char_map->ascii_chart[curr_idx].ascii_char, char_map->ascii_chart[curr_idx].symbol_description);

	curr_idx = termios_data->c_cc[VKILL];
	printf("\n\t\t\t VKILL    - 0x%.02x - %s", char_map->ascii_chart[curr_idx].ascii_char, char_map->ascii_chart[curr_idx].symbol_description);

	curr_idx = termios_data->c_cc[VEOF];
	printf("\n\t\t\t VEOF     - 0x%.02x - %s", char_map->ascii_chart[curr_idx].ascii_char, char_map->ascii_chart[curr_idx].symbol_description);

	curr_idx = termios_data->c_cc[VTIME];
	printf("\n\t\t\t VTIME    - 0x%.02x - %s", char_map->ascii_chart[curr_idx].ascii_char, char_map->ascii_chart[curr_idx].symbol_description);

	curr_idx = termios_data->c_cc[VMIN];
	printf("\n\t\t\t VMIN     - 0x%.02x - %s", char_map->ascii_chart[curr_idx].ascii_char, char_map->ascii_chart[curr_idx].symbol_description);

	curr_idx = termios_data->c_cc[VSWTC];
	printf("\n\t\t\t VSWTC    - 0x%.02x - %s", char_map->ascii_chart[curr_idx].ascii_char, char_map->ascii_chart[curr_idx].symbol_description);

	curr_idx = termios_data->c_cc[VSTART];
	printf("\n\t\t\t VSTART   - 0x%.02x - %s", char_map->ascii_chart[curr_idx].ascii_char, char_map->ascii_chart[curr_idx].symbol_description);

	curr_idx = termios_data->c_cc[VSTOP];
	printf("\n\t\t\t VSTOP    - 0x%.02x - %s", char_map->ascii_chart[curr_idx].ascii_char, char_map->ascii_chart[curr_idx].symbol_description);

	curr_idx = termios_data->c_cc[VSUSP];
	printf("\n\t\t\t VSUSP    - 0x%.02x - %s", char_map->ascii_chart[curr_idx].ascii_char, char_map->ascii_chart[curr_idx].symbol_description);

	curr_idx = termios_data->c_cc[VEOL];
	printf("\n\t\t\t VEOL     - 0x%.02x - %s", char_map->ascii_chart[curr_idx].ascii_char, char_map->ascii_chart[curr_idx].symbol_description);

	curr_idx = termios_data->c_cc[VREPRINT];
	printf("\n\t\t\t VREPRINT - 0x%.02x - %s", char_map->ascii_chart[curr_idx].ascii_char, char_map->ascii_chart[curr_idx].symbol_description);

	curr_idx = termios_data->c_cc[VDISCARD];
	printf("\n\t\t\t VDISCARD - 0x%.02x - %s", char_map->ascii_chart[curr_idx].ascii_char, char_map->ascii_chart[curr_idx].symbol_description);

	curr_idx = termios_data->c_cc[VWERASE];
	printf("\n\t\t\t VWERASE  - 0x%.02x - %s", char_map->ascii_chart[curr_idx].ascii_char, char_map->ascii_chart[curr_idx].symbol_description);

	curr_idx = termios_data->c_cc[VLNEXT];
	printf("\n\t\t\t VLNEXT   - 0x%.02x - %s", char_map->ascii_chart[curr_idx].ascii_char, char_map->ascii_chart[curr_idx].symbol_description);

	curr_idx = termios_data->c_cc[VEOL2];
	printf("\n\t\t\t VEOL2    - 0x%.02x - %s", char_map->ascii_chart[curr_idx].ascii_char, char_map->ascii_chart[curr_idx].symbol_description);

	// destroy the map now that control characters are done
	BP_DestroyASCIICharacterMap(char_map);

	// return indicating success
	return ERR_SUCCESS;

}

#endif
#endif

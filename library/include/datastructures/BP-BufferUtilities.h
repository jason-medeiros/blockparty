/***************************************************************************
 *             BP-BufferUtilities.h
 *
 *  Thu Jan 23 18:52:08 2003
 *  Copyright  2006  Jason Medeiros
 *  Email
 ****************************************************************************/



int PrintBuffer(char *buffer, int size);

// different styles of shellcode
enum {
		SC_C_STYLE,				// signed char buffer  style
		SC_C_STYLE_UNSIGNED,	// unsigned char buffer style
		SC_PERL_ARGV,			// `perl -e 'print("");'` style
		SC_PERL_SCALAR			// perl $var = "" style
};

// prints shellcode styled hexidecimal, given one
// of the above style specifiers.
int PrintShellcodeToStyle(char *sc, int scLen, int style);
char * GetShellcodeToStyle(char *sc, int scLen, int style);
char * GetBufferToStyle(char *buffer, char *bufferName, int scLen, int style);
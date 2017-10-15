/***************************************************************************
 *             BP-BufferUtilities.cc
 *
 *  Thu Jan 23 18:54:15 2003
 *  Copyright  2003  User
 *  Email
 ****************************************************************************/
#include "../../include/BP-Main.h"


// prints a buffer.  If the buffer contains non-ascii characters
// they are printed out as \x escaped bytes
int PrintBuffer(char *buffer, int size)
{

	int i;
	
	for
	(
			i = 0;
			i < size;
			i++
	)
	{

		// print out non-ascii characters
		if(buffer[i] < 0)
		{
			printf("[\\x%.2X]", (unsigned char) buffer[i]);

		}
		else
		{
		
			// seperate non-printables from printables			
			// if non-printable, print as [character]
			switch(buffer[i])
			{
				
				// Null
				case 0x00:
					printf("[NUL]");
					break;
				
				// Start of Heading
				case 0x01:
					printf("[SOH]");
					break;
				
				// Start of Text
				case 0x02:
					printf("[STX]");
					break;
				
				// End of Text
				case 0x03:
					printf("[ETX]");
					break;
				
				// End of Transmission
				case 0x04:
					printf("[EOT]");
					break;
				
				// Enquiry
				case 0x05:
					printf("[ENQ]");
					break;
				
				// Acknoledge
				case 0x06:
					printf("[ACK]");
					break;
				
				// Bell
				case 0x07:
					printf("[BEL]");
					break;
				
				// Backspace
				case 0x08:
					printf("[BS]");
					break;
				
				// Horizontal tab
				//case 0x09:
				//	printf("[TAB]");
				//	break;
				
				// NL line feed, new line
				//case 0x0a:
				//	printf("[LF]");
				//	break;
				
				// Vertical tab
				case 0x0b:
					printf("[VT]");
					break;
				
				// NP form feed, new page
				case 0x0c:
					printf("[FF]");
					break;
				
				// Carriage Return
				//case 0x0d:
				//	printf("[CR]");
				//	break;
				
				// Shift Out
				case 0x0e:
					printf("[SO]");
					break;
				
				// Shift In
				case 0x0f:
					printf("[SI]");
					break;
				
				// Data Link Escape
				case 0x10:
					printf("[DLE]");
					break;
				
				// Device Control 1
				case 0x11:
					printf("[DC1]");
					break;
				
				// Device Control 2
				case 0x12:
					printf("[DC2]");
					break;
					
				// Device Control 3
				case 0x13:
					printf("[DC3]");
					break;
				
				// Device Control 4
				case 0x14:
					printf("[DC4]");
					break;
				
				// Negative Acknoledge
				case 0x15:
					printf("[NAK]");
					break;
		
				// Synchronous Idle
				case 0x16:
					printf("[SYN]");
					break;
				
				// End Transmission Block
				case 0x17:
					printf("[ETB]");
					break;
				
				// Cancel
				case 0x18:
					printf("[CAN]");
					break;
				
				// End of Medium
				case 0x19:
					printf("[EM]");
					break;
				
				// Substitute
				case 0x1a:
					printf("[SUB]");
					break;
				
				// Escape
				case 0x1b:
					printf("[ESC]");
					break;
				
				// File Separator
				case 0x1c:
					printf("[FS]");
					break;
				
				// Group Separator
				case 0x1d:
					printf("[GS]");
					break;
				
				// Record Separator
				case 0x1e:
					printf("[RS]");
					break;
					
				// Unit Separator
				case 0x1f:
					printf("[US]");
					break;
				
				//print out the actual character if printable
				default:
					printf("%c", buffer[i]);
					break;
			}

		}
	
	}
	
	// return indicating success
	return ERR_SUCCESS;

}


// prints shellcode styled hexidecimal, given one
// of the above style specifiers
int PrintShellcodeToStyle(char *sc, int scLen, int style)
{
	
	int i = 0;
	
	if(scLen <= 0)
		return ERR_FAILURE;
	
	switch(style){
		case SC_C_STYLE:
			printf("char shellcode[] = \"");
			break;			// signed char buffer  style
		case SC_C_STYLE_UNSIGNED:
			printf("unsigned char shellcode[] = \"");
			break;			// unsigned char buffer style
		case SC_PERL_ARGV:			// `perl -e 'print("");'` style
			printf("`perl -e 'print(\"");
			break;
		case SC_PERL_SCALAR:			// perl $var = "" style
			printf("$shellcode = \"");
			break;
		default:
			return ERR_FAILURE;
	}
	
     for(i = 0; i <=scLen; i++){
            printf("\\x%.2x", (unsigned char) sc[i]);
     }
	 
	 switch(style){
		case SC_C_STYLE:
			printf("\";");
			break;			// signed char buffer  style
		case SC_C_STYLE_UNSIGNED:
			printf("\";");
			break;			// unsigned char buffer style
		case SC_PERL_ARGV:			// `perl -e 'print("");'` style
			printf("\");'`");
			break;
		case SC_PERL_SCALAR:			// perl $var = "" style
			printf("\";");
			break;
		default:
			return ERR_FAILURE;
	}
	
   	printf("\n");
	 
	return ERR_SUCCESS;
}

// retrieves shellcode styled hexidecimal, 
char * GetShellcodeToStyle(char *sc, int scLen, int style)
{
	
	int i = 0;
	char *retScBuff = (char *) bpcalloc(scLen+100, 1);
	if(scLen <= 0)
		return NULL;

	switch(style)
	{
	
		case SC_C_STYLE:
			sprintf(&retScBuff[strlen(retScBuff)], "char shellcode[] = \"");
			break;			// signed char buffer  style
		case SC_C_STYLE_UNSIGNED:
			sprintf(&retScBuff[strlen(retScBuff)], "unsigned char shellcode[] = \"");
			break;			// unsigned char buffer style
		case SC_PERL_ARGV:			// `perl -e 'print("");'` style
			sprintf(&retScBuff[strlen(retScBuff)], "`perl -e 'print(\"");
			break;
		case SC_PERL_SCALAR:			// perl $var = "" style
			sprintf(&retScBuff[strlen(retScBuff)],"$shellcode = \"");
			break;
		default:
			return NULL;
	}
	
    for(i = 0; i <=scLen; i++)
    {
    	sprintf(&retScBuff[strlen(retScBuff)], "\\x%.2x", (unsigned char) sc[i]);
    }
	 
	switch(style)
	{

		case SC_C_STYLE:
			sprintf(&retScBuff[strlen(retScBuff)], "\";");
			break;			// signed char buffer  style

		case SC_C_STYLE_UNSIGNED:
			sprintf(&retScBuff[strlen(retScBuff)], "\";");
			break;			// unsigned char buffer style

		case SC_PERL_ARGV:			// `perl -e 'print("");'` style
			sprintf(&retScBuff[strlen(retScBuff)], "\");'`");
			break;

		case SC_PERL_SCALAR:			// perl $var = "" style
			sprintf(&retScBuff[strlen(retScBuff)], "\";");
			break;

		default:
			return NULL;

	}
	
	// add new line to buffer
   	sprintf(&retScBuff[strlen(retScBuff)], "\n");
	
   	// return the filled out buffer
	return retScBuff;

}


// retrieves shellcode styled hexidecimal, (Style should be C
char * GetBufferToStyle(char *buff, char *buffName, int len, int style)
{
	
	int i = 0;
	char *retBuff = (char *) bpmalloc(len+10000);
	memset(retBuff, 0x00, len+10000);
	
	if(len <= 0)
		return NULL;
	
	
	switch(style)
	{

		case SC_C_STYLE:
			sprintf(&retBuff[strlen(retBuff)], "char %s[] = \"", buffName);
			break;			// signed char buffer  style

		case SC_C_STYLE_UNSIGNED:
			sprintf(&retBuff[strlen(retBuff)], "unsigned char %s[] = \"", buffName);
			break;			// unsigned char buffer style

		case SC_PERL_ARGV:			// `perl -e 'print("");'` style
			sprintf(&retBuff[strlen(retBuff)], "`perl -e 'print(\"");
			break;

		case SC_PERL_SCALAR:			// perl $var = "" style
			sprintf(&retBuff[strlen(retBuff)],"$%s = \"", buffName);
			break;

		default:
			return NULL;

	}
	
     for(i = 0; i <=len; i++)
     {
            sprintf(&retBuff[strlen(retBuff)], "\\x%.2x", (unsigned char) buff[i]);
     }
	 
	 switch(style)
	 {

		case SC_C_STYLE:
			sprintf(&retBuff[strlen(retBuff)], "\";");
			break;			// signed char buffer  style

		case SC_C_STYLE_UNSIGNED:
			sprintf(&retBuff[strlen(retBuff)], "\";");
			break;			// unsigned char buffer style

		case SC_PERL_ARGV:			// `perl -e 'print("");'` style
			sprintf(&retBuff[strlen(retBuff)], "\");'`");
			break;

		case SC_PERL_SCALAR:			// perl $var = "" style
			sprintf(&retBuff[strlen(retBuff)], "\";");
			break;

		default:
			return NULL;

	}
	
	// add trailing newline
   	sprintf(&retBuff[strlen(retBuff)], "\n");
	
   	// return the ret buffer
	return retBuff;

}




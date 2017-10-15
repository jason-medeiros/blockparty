/*
 * BP-ASCIICharts.cc
 *
 *  Created on: Jul 14, 2014
 *      Author: root
 */


// include main functions
#include "../../include/BP-Main.h"


// generates a searchable ascii character map
P_BP_ASCII_CHARACTER_MAP BP_CreateASCIICharacterMap()
{

	// create space for the map
	P_BP_ASCII_CHARACTER_MAP char_map = (P_BP_ASCII_CHARACTER_MAP) bpcalloc(sizeof(BP_ASCII_CHARACTER_MAP), 1);

	// ensure we allocated space for the map, if not, exit
	if(!char_map)
		return NULL;

	// since character maps are read only except for initial generation, generate tq as non thread safe
	char_map->tq = BP_CreateLinkL("ASCII Character Map", BP_FALSE);

	// ensure we created the allocator
	if(!char_map->tq)
	{
		BP_DestroyASCIICharacterMap(char_map);
		return NULL;
	}

	// attempt to add the map, if this is impossible, destroy data and return
	if(BP_AddASCIIChartToCharacterMap(char_map) != ERR_SUCCESS)
	{
		BP_DestroyASCIICharacterMap(char_map);
		return NULL;
	}

	// return the filled out map
	return char_map;

}

// destroys an ascii character map (frees all memory, including the P_BP_ASCII_CHARACTER_MAP structure)
BP_ERROR_T BP_DestroyASCIICharacterMap(P_BP_ASCII_CHARACTER_MAP char_map)
{

	// ensure we have a map to destroy
	if(!char_map)
		return ERR_FAILURE;

	// destroy the tailqueue first (destroys all created data within
	// the character map)
	if(char_map->tq)
	{
		BP_DestroyLinkL(char_map->tq, BP_TRUE);
	}



	// destroy the last piece of the structure
	bpfree(char_map);

	// return indicating success
	return ERR_SUCCESS;

}


// fills a character map with a standard ascii chart
BP_ERROR_T BP_AddASCIIChartToCharacterMap(P_BP_ASCII_CHARACTER_MAP char_map)
{

	// ensure we have a character map
	if(!char_map)
		return ERR_FAILURE;

	// ensure we have a tail queue allocator
	if(!char_map->tq)
		return ERR_FAILURE;

	// 255 entries in the map
	char_map->ascii_chart_entry_n = 0xff;

	// attempt to allocate data
	char_map->ascii_chart = (P_BP_ASCII_CHART) tq_calloc
	(
			char_map->tq,
			sizeof(BP_ASCII_CHART) * (char_map->ascii_chart_entry_n+1),
			1,
			BPLN
	);

	// ensure we allocated space ok
	if(!char_map->ascii_chart)
	{
		char_map->ascii_chart_entry_n = 0;
		return ERR_FAILURE;
	}

	// The current index into the stack of structures in which
	// we are operating.  This is used by the ASCII_CHART_MOVE_NEXT macro.
	size_t idx = 0;

	// reference used for iterating through the map (makes things
	// appear less convoluted).  This advancement is done automatically
	// via a macro set in the ascii charts header file.  It's only done
	// once here so you get an idea of what the macro is doing
	P_BP_ASCII_CHART chart = char_map->ascii_chart;

	// NULL
	chart->ascii_char = (unsigned char) 0x00;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("NUL");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Null character");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// start of heading
	chart->ascii_char = (unsigned char) 0x01;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("SOH");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Start of heading");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// start of text
	chart->ascii_char = (unsigned char) 0x02;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("STX");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Start of text");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// end of text
	chart->ascii_char = (unsigned char) 0x03;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("ETX");
	chart->symbol_description  = ASCII_CHART_ADDSTR("End of text");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// end of transmission
	chart->ascii_char = (unsigned char) 0x04;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("EOT");
	chart->symbol_description  = ASCII_CHART_ADDSTR("End of transmission");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// enquiry
	chart->ascii_char = (unsigned char) 0x05;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("ENQ");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Enquiry");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// acknowledge
	chart->ascii_char = (unsigned char) 0x06;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("ACK");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Acknowledge");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;


	// Bell
	chart->ascii_char = (unsigned char) 0x07;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("BEL");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Bell");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Backspace
	chart->ascii_char = (unsigned char) 0x08;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("BS");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Backspace");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Horizontal tab
	chart->ascii_char = (unsigned char) 0x09;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("TAB");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Horizontal Tab");
	chart->flags.is_control      = 1;
	chart->flags.is_whitespace   = 1;
	ASCII_CHART_MOVE_NEXT;

	// Newline character
	chart->ascii_char = (unsigned char) 0x0a;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("LF");
	chart->symbol_description  = ASCII_CHART_ADDSTR("NL Line feed, new line");
	chart->flags.is_control      = 1;
	chart->flags.is_whitespace   = 1;
	ASCII_CHART_MOVE_NEXT;

	// Vertical tab
	chart->ascii_char = (unsigned char) 0x0b;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("VT");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Vertical Tab");
	chart->flags.is_control      = 1;
	chart->flags.is_whitespace   = 1;
	ASCII_CHART_MOVE_NEXT;

	// form feed
	chart->ascii_char = (unsigned char) 0x0c;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("FF");
	chart->symbol_description  = ASCII_CHART_ADDSTR("NP form feed, new page");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	chart->flags.is_whitespace   = 1;
	ASCII_CHART_MOVE_NEXT;

	// carrige return
	chart->ascii_char = (unsigned char) 0x0d;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("CR");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Carriage Return");
	chart->flags.is_control      = 1;
	chart->flags.is_whitespace   = 1;
	ASCII_CHART_MOVE_NEXT;

	// shift out
	chart->ascii_char = (unsigned char) 0x0e;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("SO");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Shift Out");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	chart->flags.is_whitespace   = 1;
	ASCII_CHART_MOVE_NEXT;

	// shift in
	chart->ascii_char = (unsigned char) 0x0f;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("SI");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Shift In");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	chart->flags.is_whitespace   = 1;
	ASCII_CHART_MOVE_NEXT;

	// Data link escape
	chart->ascii_char = (unsigned char) 0x10;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("DLE");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Data Link Escape");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Device Control 1
	chart->ascii_char = (unsigned char) 0x11;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("DC1");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Device Control 1");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Device Control 2
	chart->ascii_char = (unsigned char) 0x12;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("DC2");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Device Control 2");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Device Control 3
	chart->ascii_char = (unsigned char) 0x13;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("DC3");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Device Control 3");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Device Control 4
	chart->ascii_char = (unsigned char) 0x14;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("DC4");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Device Control 4");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Negative Acknowledge
	chart->ascii_char = (unsigned char) 0x15;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("NAK");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Negative Acknowledge");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Synchronous Idle
	chart->ascii_char = (unsigned char) 0x16;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("SYN");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Synchronous Idle");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// End of Transmission Block
	chart->ascii_char = (unsigned char) 0x17;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("ETB");
	chart->symbol_description  = ASCII_CHART_ADDSTR("End of Transmission Block");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Cancel
	chart->ascii_char = (unsigned char) 0x18;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("CAN");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Cancel");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// End of Medium
	chart->ascii_char = (unsigned char) 0x19;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("EM");
	chart->symbol_description  = ASCII_CHART_ADDSTR("End of Medium");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Substitute
	chart->ascii_char = (unsigned char) 0x1a;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("SUB");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Substitute");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Escape
	chart->ascii_char = (unsigned char) 0x1b;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("ESC");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Escape");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// File Separator
	chart->ascii_char = (unsigned char) 0x1c;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("FS");
	chart->symbol_description  = ASCII_CHART_ADDSTR("File Separator");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Group Separator
	chart->ascii_char = (unsigned char) 0x1d;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("GS");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Group Separator");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Record Separator
	chart->ascii_char = (unsigned char) 0x1e;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("RS");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Record Separator");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Unit Separator
	chart->ascii_char = (unsigned char) 0x1f;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("US");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Unit Separator");
	chart->flags.is_nonprintable = 1;
	chart->flags.is_control      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Space
	chart->ascii_char = (unsigned char) 0x20;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("Space");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Space");
	chart->flags.is_printable = 1;
	chart->flags.is_whitespace   = 1;
	ASCII_CHART_MOVE_NEXT;

	// Exclamation Mark
	chart->ascii_char = (unsigned char) 0x21;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("!");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Exclamation Mark");
	chart->flags.is_printable   = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	chart->flags.is_math        = 1;
	ASCII_CHART_MOVE_NEXT;

	// Double Quote
	chart->ascii_char = (unsigned char) 0x22;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\"");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Double Quote");
	chart->flags.is_printable   = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	chart->flags.is_quotation   = 1;
	ASCII_CHART_MOVE_NEXT;

	// Octothorpe
	chart->ascii_char = (unsigned char) 0x23;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("#");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Octothorpe");
	chart->flags.is_printable   = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Dollar Sign
	chart->ascii_char = (unsigned char) 0x24;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("$");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Dollar Sign");
	chart->flags.is_printable   = 1;
	chart->flags.is_currency    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Percentage Sign
	chart->ascii_char = (unsigned char) 0x25;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("%%");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Percentage Sign");
	chart->flags.is_printable   = 1;
	chart->flags.is_math        = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Ampersand
	chart->ascii_char = (unsigned char) 0x26;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("&");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Ampersand");
	chart->flags.is_printable   = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Single Quote
	chart->ascii_char = (unsigned char) 0x27;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("'");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Single Quote");
	chart->flags.is_printable   = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	chart->flags.is_quotation   = 1;
	ASCII_CHART_MOVE_NEXT;

	// Open Parenthesis
	chart->ascii_char = (unsigned char) 0x28;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("(");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Open Parenthesis");
	chart->flags.is_printable   = 1;
	chart->flags.is_math        = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Close Parenthesis
	chart->ascii_char = (unsigned char) 0x29;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR(")");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Close Parenthesis");
	chart->flags.is_printable   = 1;
	chart->flags.is_math        = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Asterisk
	chart->ascii_char = (unsigned char) 0x2a;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("*");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Asterisk");
	chart->flags.is_printable   = 1;
	chart->flags.is_math        = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Plus Sign
	chart->ascii_char = (unsigned char) 0x2b;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("+");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Plus Sign");
	chart->flags.is_printable   = 1;
	chart->flags.is_math        = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Comma
	chart->ascii_char = (unsigned char) 0x2c;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR(",");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Comma");
	chart->flags.is_printable   = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Hyphen
	chart->ascii_char = (unsigned char) 0x2d;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("-");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Hyphen");
	chart->flags.is_printable   = 1;
	chart->flags.is_math        = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Period
	chart->ascii_char = (unsigned char) 0x2e;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR(".");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Period");
	chart->flags.is_printable   = 1;
	chart->flags.is_math        = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Forward Slash
	chart->ascii_char = (unsigned char) 0x2f;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("/");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Forward Slash");
	chart->flags.is_printable   = 1;
	chart->flags.is_math        = 1;
	chart->flags.is_slash       = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// 0
	chart->ascii_char = (unsigned char) 0x30;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("0");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Zero");
	chart->flags.is_printable   = 1;
	chart->flags.is_number      = 1;
	ASCII_CHART_MOVE_NEXT;

	// 1
	chart->ascii_char = (unsigned char) 0x31;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("1");
	chart->symbol_description  = ASCII_CHART_ADDSTR("One");
	chart->flags.is_printable   = 1;
	chart->flags.is_number      = 1;
	ASCII_CHART_MOVE_NEXT;

	// 2
	chart->ascii_char = (unsigned char) 0x32;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("2");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Two");
	chart->flags.is_printable   = 1;
	chart->flags.is_number      = 1;
	ASCII_CHART_MOVE_NEXT;

	// 3
	chart->ascii_char = (unsigned char) 0x33;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("3");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Three");
	chart->flags.is_printable   = 1;
	chart->flags.is_number      = 1;
	ASCII_CHART_MOVE_NEXT;

	// 4
	chart->ascii_char = (unsigned char) 0x34;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("4");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Four");
	chart->flags.is_printable   = 1;
	chart->flags.is_number      = 1;
	ASCII_CHART_MOVE_NEXT;

	// 5
	chart->ascii_char = (unsigned char) 0x35;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("5");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Five");
	chart->flags.is_printable   = 1;
	chart->flags.is_number      = 1;
	ASCII_CHART_MOVE_NEXT;

	// 6
	chart->ascii_char = (unsigned char) 0x36;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("6");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Six");
	chart->flags.is_printable   = 1;
	chart->flags.is_number      = 1;
	ASCII_CHART_MOVE_NEXT;

	// 7
	chart->ascii_char = (unsigned char) 0x37;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("7");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Seven");
	chart->flags.is_printable   = 1;
	chart->flags.is_number      = 1;
	ASCII_CHART_MOVE_NEXT;

	// 8
	chart->ascii_char = (unsigned char) 0x38;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("8");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Eight");
	chart->flags.is_printable   = 1;
	chart->flags.is_number      = 1;
	ASCII_CHART_MOVE_NEXT;

	// 9
	chart->ascii_char = (unsigned char) 0x39;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("9");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Nine");
	chart->flags.is_printable   = 1;
	chart->flags.is_number      = 1;
	ASCII_CHART_MOVE_NEXT;

	// colon
	chart->ascii_char = (unsigned char) 0x3a;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR(":");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Colon");
	chart->flags.is_printable   = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Semicolon
	chart->ascii_char = (unsigned char) 0x3b;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR(";");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Semicolon");
	chart->flags.is_printable   = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Less than
	chart->ascii_char = (unsigned char) 0x3c;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("<");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Less than");
	chart->flags.is_printable   = 1;
	chart->flags.is_math        = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Equals
	chart->ascii_char = (unsigned char) 0x3d;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("=");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Equals");
	chart->flags.is_printable   = 1;
	chart->flags.is_math        = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Greater than
	chart->ascii_char = (unsigned char) 0x3e;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR(">");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Greater than");
	chart->flags.is_math        = 1;
	chart->flags.is_printable   = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Question mark
	chart->ascii_char = (unsigned char) 0x3f;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("?");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Question Mark");
	chart->flags.is_printable   = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// At
	chart->ascii_char = (unsigned char) 0x40;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("@");
	chart->symbol_description  = ASCII_CHART_ADDSTR("At (each at)");
	chart->flags.is_printable   = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// A
	chart->ascii_char = (unsigned char) 0x41;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("A");
	chart->symbol_description  = ASCII_CHART_ADDSTR("A");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// B
	chart->ascii_char = (unsigned char) 0x42;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("B");
	chart->symbol_description  = ASCII_CHART_ADDSTR("B");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// C
	chart->ascii_char = (unsigned char) 0x43;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("C");
	chart->symbol_description  = ASCII_CHART_ADDSTR("C");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// D
	chart->ascii_char = (unsigned char) 0x44;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("D");
	chart->symbol_description  = ASCII_CHART_ADDSTR("D");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// E
	chart->ascii_char = (unsigned char) 0x45;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("E");
	chart->symbol_description  = ASCII_CHART_ADDSTR("E");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// F
	chart->ascii_char = (unsigned char) 0x46;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("F");
	chart->symbol_description  = ASCII_CHART_ADDSTR("F");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// G
	chart->ascii_char = (unsigned char) 0x47;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("G");
	chart->symbol_description  = ASCII_CHART_ADDSTR("G");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// H
	chart->ascii_char = (unsigned char) 0x48;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("H");
	chart->symbol_description  = ASCII_CHART_ADDSTR("H");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// I
	chart->ascii_char = (unsigned char) 0x49;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("I");
	chart->symbol_description  = ASCII_CHART_ADDSTR("I");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// J
	chart->ascii_char = (unsigned char) 0x4a;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("J");
	chart->symbol_description  = ASCII_CHART_ADDSTR("J");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// K
	chart->ascii_char = (unsigned char) 0x4b;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("K");
	chart->symbol_description  = ASCII_CHART_ADDSTR("K");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// L
	chart->ascii_char = (unsigned char) 0x4c;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("L");
	chart->symbol_description  = ASCII_CHART_ADDSTR("L");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// M
	chart->ascii_char = (unsigned char) 0x4d;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("M");
	chart->symbol_description  = ASCII_CHART_ADDSTR("M");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// N
	chart->ascii_char = (unsigned char) 0x4e;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("N");
	chart->symbol_description  = ASCII_CHART_ADDSTR("N");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// O
	chart->ascii_char = (unsigned char) 0x4f;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("O");
	chart->symbol_description  = ASCII_CHART_ADDSTR("O");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// P
	chart->ascii_char = (unsigned char) 0x50;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("P");
	chart->symbol_description  = ASCII_CHART_ADDSTR("P");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// Q
	chart->ascii_char = (unsigned char) 0x51;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("Q");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Q");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// R
	chart->ascii_char = (unsigned char) 0x52;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("R");
	chart->symbol_description  = ASCII_CHART_ADDSTR("R");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// S
	chart->ascii_char = (unsigned char) 0x53;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("S");
	chart->symbol_description  = ASCII_CHART_ADDSTR("S");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// T
	chart->ascii_char = (unsigned char) 0x54;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("T");
	chart->symbol_description  = ASCII_CHART_ADDSTR("T");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// U
	chart->ascii_char = (unsigned char) 0x55;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("U");
	chart->symbol_description  = ASCII_CHART_ADDSTR("U");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// V
	chart->ascii_char = (unsigned char) 0x56;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("V");
	chart->symbol_description  = ASCII_CHART_ADDSTR("V");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// W
	chart->ascii_char = (unsigned char) 0x57;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("W");
	chart->symbol_description  = ASCII_CHART_ADDSTR("W");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// X
	chart->ascii_char = (unsigned char) 0x58;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("X");
	chart->symbol_description  = ASCII_CHART_ADDSTR("X");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// Y
	chart->ascii_char = (unsigned char) 0x59;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("Y");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Y");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// Z
	chart->ascii_char = (unsigned char) 0x5a;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("Z");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Z");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphacaps   = 1;
	ASCII_CHART_MOVE_NEXT;

	// [
	chart->ascii_char = (unsigned char) 0x5b;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("[");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Open Bracket");
	chart->flags.is_printable   = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Backslash
	chart->ascii_char = (unsigned char) 0x5c;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\\");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Backslash");
	chart->flags.is_printable   = 1;
	chart->flags.is_slash       = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// ]
	chart->ascii_char = (unsigned char) 0x5d;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("]");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Close Bracket");
	chart->flags.is_printable   = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// ^
	chart->ascii_char = (unsigned char) 0x5e;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("^");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Carat");
	chart->flags.is_printable   = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// _
	chart->ascii_char = (unsigned char) 0x5f;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("_");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Underscore");
	chart->flags.is_printable   = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// `
	chart->ascii_char = (unsigned char) 0x60;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("`");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Tick");
	chart->flags.is_printable   = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// a
	chart->ascii_char = (unsigned char) 0x61;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("a");
	chart->symbol_description  = ASCII_CHART_ADDSTR("a");
	chart->flags.is_printable    = 1;
	chart->flags.is_alphalower   = 1;
	ASCII_CHART_MOVE_NEXT;

	// b
	chart->ascii_char = (unsigned char) 0x62;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("b");
	chart->symbol_description  = ASCII_CHART_ADDSTR("b");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// c
	chart->ascii_char = (unsigned char) 0x63;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("c");
	chart->symbol_description  = ASCII_CHART_ADDSTR("c");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// d
	chart->ascii_char = (unsigned char) 0x64;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("d");
	chart->symbol_description  = ASCII_CHART_ADDSTR("d");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// e
	chart->ascii_char = (unsigned char) 0x65;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("e");
	chart->symbol_description  = ASCII_CHART_ADDSTR("e");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// f
	chart->ascii_char = (unsigned char) 0x66;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("f");
	chart->symbol_description  = ASCII_CHART_ADDSTR("f");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// g
	chart->ascii_char = (unsigned char) 0x67;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("g");
	chart->symbol_description  = ASCII_CHART_ADDSTR("g");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// h
	chart->ascii_char = (unsigned char) 0x68;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("h");
	chart->symbol_description  = ASCII_CHART_ADDSTR("h");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// i
	chart->ascii_char = (unsigned char) 0x69;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("i");
	chart->symbol_description  = ASCII_CHART_ADDSTR("i");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// j
	chart->ascii_char = (unsigned char) 0x6a;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("j");
	chart->symbol_description  = ASCII_CHART_ADDSTR("j");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// k
	chart->ascii_char = (unsigned char) 0x6b;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("k");
	chart->symbol_description  = ASCII_CHART_ADDSTR("k");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// l
	chart->ascii_char = (unsigned char) 0x6c;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("l");
	chart->symbol_description  = ASCII_CHART_ADDSTR("l");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// m
	chart->ascii_char = (unsigned char) 0x6d;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("m");
	chart->symbol_description  = ASCII_CHART_ADDSTR("m");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// n
	chart->ascii_char = (unsigned char) 0x6e;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("n");
	chart->symbol_description  = ASCII_CHART_ADDSTR("n");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// o
	chart->ascii_char = (unsigned char) 0x6f;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("o");
	chart->symbol_description  = ASCII_CHART_ADDSTR("o");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// p
	chart->ascii_char = (unsigned char) 0x70;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("p");
	chart->symbol_description  = ASCII_CHART_ADDSTR("p");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// q
	chart->ascii_char = (unsigned char) 0x71;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("q");
	chart->symbol_description  = ASCII_CHART_ADDSTR("q");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// r
	chart->ascii_char = (unsigned char) 0x72;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("r");
	chart->symbol_description  = ASCII_CHART_ADDSTR("r");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// s
	chart->ascii_char = (unsigned char) 0x73;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("s");
	chart->symbol_description  = ASCII_CHART_ADDSTR("s");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// t
	chart->ascii_char = (unsigned char) 0x74;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("t");
	chart->symbol_description  = ASCII_CHART_ADDSTR("t");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// u
	chart->ascii_char = (unsigned char) 0x75;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("u");
	chart->symbol_description  = ASCII_CHART_ADDSTR("u");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// v
	chart->ascii_char = (unsigned char) 0x76;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("v");
	chart->symbol_description  = ASCII_CHART_ADDSTR("v");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// w
	chart->ascii_char = (unsigned char) 0x77;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("w");
	chart->symbol_description  = ASCII_CHART_ADDSTR("w");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// x
	chart->ascii_char = (unsigned char) 0x78;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("x");
	chart->symbol_description  = ASCII_CHART_ADDSTR("x");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// y
	chart->ascii_char = (unsigned char) 0x79;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("y");
	chart->symbol_description  = ASCII_CHART_ADDSTR("y");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// z
	chart->ascii_char = (unsigned char) 0x7a;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("z");
	chart->symbol_description  = ASCII_CHART_ADDSTR("z");
	chart->flags.is_printable   = 1;
	chart->flags.is_alphalower  = 1;
	ASCII_CHART_MOVE_NEXT;

	// {
	chart->ascii_char = (unsigned char) 0x7b;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("{");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Open Curly Bracket");
	chart->flags.is_printable   = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// |
	chart->ascii_char = (unsigned char) 0x7c;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("|");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Pipe");
	chart->flags.is_printable   = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// }
	chart->ascii_char = (unsigned char) 0x7d;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("}");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Close Curly Bracket");
	chart->flags.is_printable   = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// ~
	chart->ascii_char = (unsigned char) 0x7e;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("~");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Tilde");
	chart->flags.is_printable   = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// DEL
	chart->ascii_char = (unsigned char) 0x7f;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("DEL");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Delete");
	chart->flags.is_nonprintable = 1;
	ASCII_CHART_MOVE_NEXT;


	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!! STANDARD TABLE FINISHED, BEGIN EXTENDED TABLE !!!
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Euro Sign
	chart->ascii_char = (unsigned char) 0x80;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x80");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Euro Sign");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_currency    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// undefined 0x81
	chart->ascii_char = (unsigned char) 0x81;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("<0x81 undefined>");
	chart->symbol_description  = ASCII_CHART_ADDSTR("The symbol corresponding to 0x81 is undefined.");
	chart->flags.is_extended    = 1;
	chart->flags.is_undefined   = 1;
	ASCII_CHART_MOVE_NEXT;

	// 	Single low-9 quotation mark
	chart->ascii_char = (unsigned char) 0x82;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x82");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Single low-9 quotation mark");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	chart->flags.is_quotation   = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter f with hook
	chart->ascii_char = (unsigned char) 0x83;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x83");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter f with hook");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Double low-9 quotation mark
	chart->ascii_char = (unsigned char) 0x84;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x84");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Double low-9 quotation mark");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	chart->flags.is_quotation   = 1;
	ASCII_CHART_MOVE_NEXT;

	// Horizontal ellipsis
	chart->ascii_char = (unsigned char) 0x85;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x85");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Horizontal ellipsis");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Dagger
	chart->ascii_char = (unsigned char) 0x86;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x86");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Dagger");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Double Dagger
	chart->ascii_char = (unsigned char) 0x87;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x86");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Double dagger");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Modifier letter circumflex accent
	chart->ascii_char = (unsigned char) 0x88;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x88");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Modifier letter circumflex accent");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_circumflex  = 1;
	ASCII_CHART_MOVE_NEXT;

	// 	Per mille sign
	chart->ascii_char = (unsigned char) 0x89;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x89");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Per mille sign");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_math        = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter S with caron
	chart->ascii_char = (unsigned char) 0x8a;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x8a");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter S with caron");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_caron       = 1;
	chart->flags.is_caron       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Single left-pointing angle quotation
	chart->ascii_char = (unsigned char) 0x8b;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x8b");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Single left-pointing angle quotation");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	chart->flags.is_quotation   = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital ligature OE
	chart->ascii_char = (unsigned char) 0x8c;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x8c");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital ligature OE");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// undefined 0x8d
	chart->ascii_char = (unsigned char) 0x8d;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("<0x8d undefined>");
	chart->symbol_description  = ASCII_CHART_ADDSTR("The symbol corresponding to 0x81 is undefined.");
	chart->flags.is_extended    = 1;
	chart->flags.is_undefined   = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin captial letter Z with caron
	chart->ascii_char = (unsigned char) 0x8e;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x83");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin captial letter Z with caron");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_caron       = 1;
	chart->flags.is_caron       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// undefined 0x8f
	chart->ascii_char = (unsigned char) 0x8f;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("<0x8f undefined>");
	chart->symbol_description  = ASCII_CHART_ADDSTR("The symbol corresponding to 0x81 is undefined.");
	chart->flags.is_extended    = 1;
	chart->flags.is_undefined   = 1;
	ASCII_CHART_MOVE_NEXT;

	// undefined 0x90
	chart->ascii_char = (unsigned char) 0x90;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x90");
	chart->symbol_description  = ASCII_CHART_ADDSTR("The symbol corresponding to 0x90 is undefined.");
	chart->flags.is_extended    = 1;
	chart->flags.is_undefined   = 1;
	ASCII_CHART_MOVE_NEXT;

	// 	Left single quotation mark
	chart->ascii_char = (unsigned char) 0x91;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x91");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Left single quotation mark");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	chart->flags.is_quotation   = 1;
	ASCII_CHART_MOVE_NEXT;

	// Right single quotation mark
	chart->ascii_char = (unsigned char) 0x92;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x92");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Right single quotation mark");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	chart->flags.is_quotation   = 1;
	ASCII_CHART_MOVE_NEXT;

	// 	Left double quotation mark
	chart->ascii_char = (unsigned char) 0x93;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x93");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Left double quotation mark");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	chart->flags.is_quotation   = 1;
	ASCII_CHART_MOVE_NEXT;

	// Right double quotation mark
	chart->ascii_char = (unsigned char) 0x94;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x94");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Right double quotation mark");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	chart->flags.is_quotation   = 1;
	ASCII_CHART_MOVE_NEXT;

	// Bullet mark
	chart->ascii_char = (unsigned char) 0x95;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x95");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Bullet mark");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// En dash
	chart->ascii_char = (unsigned char) 0x96;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x96");
	chart->symbol_description  = ASCII_CHART_ADDSTR("En dash");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Em dash
	chart->ascii_char = (unsigned char) 0x97;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x97");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Em dash");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Small tilde
	chart->ascii_char = (unsigned char) 0x98;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x98");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Small tilde");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Trademark
	chart->ascii_char = (unsigned char) 0x99;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x99");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Trademark");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter S with caron
	chart->ascii_char = (unsigned char) 0x9a;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x9a");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter S with caron");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_caron       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// 	Single right-pointing angle quotation mark
	chart->ascii_char = (unsigned char) 0x9b;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\9b");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Single right-pointing angle quotation mark");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	chart->flags.is_quotation   = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small ligature oe
	chart->ascii_char = (unsigned char) 0x9c;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x9c");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small ligature oe");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// undefined 0x9d
	chart->ascii_char = (unsigned char) 0x9d;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x9d");
	chart->symbol_description  = ASCII_CHART_ADDSTR("The symbol corresponding to 0x90 is undefined.");
	chart->flags.is_extended    = 1;
	chart->flags.is_undefined   = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter z with caron
	chart->ascii_char = (unsigned char) 0x9e;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x9e");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter z with caron");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_caron       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter Y with diaeresis
	chart->ascii_char = (unsigned char) 0x9f;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x9f");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter Y with diaeresis");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Non-breaking space
	chart->ascii_char = (unsigned char) 0xa0;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xa0");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Non-breaking space");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_whitespace   = 1;
	ASCII_CHART_MOVE_NEXT;

	// Inverted exclamation mark
	chart->ascii_char = (unsigned char) 0xa1;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xa1");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Inverted exclamation mark");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Cent sign
	chart->ascii_char = (unsigned char) 0xa2;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xa2");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Cent sign");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_currency    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// British Pound sign (as in currency)
	chart->ascii_char = (unsigned char) 0xa3;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\x83");
	chart->symbol_description  = ASCII_CHART_ADDSTR("British Pound sign");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_currency    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Currency sign
	chart->ascii_char = (unsigned char) 0xa4;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xa4");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Currency sign");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_currency    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Yen sign
	chart->ascii_char = (unsigned char) 0xa5;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xa5");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Yen sign");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_currency    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Pipe, Broken vertical bar
	chart->ascii_char = (unsigned char) 0xa6;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xa6");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Pipe, Broken vertical bar");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Section sign
	chart->ascii_char = (unsigned char) 0xa7;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xa7");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Section sign");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Spacing diaeresis - umlaut
	chart->ascii_char = (unsigned char) 0xa8;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xa8");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Spacing diaeresis - umlaut");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Copyright sign
	chart->ascii_char = (unsigned char) 0xa9;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xa9");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Copyright sign");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Feminine ordinal indicator
	chart->ascii_char = (unsigned char) 0xaa;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xaa");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Feminine ordinal indicator");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Left double angle quotes
	chart->ascii_char = (unsigned char) 0xab;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xab");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Left double angle quotes");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	chart->flags.is_quotation   = 1;
	ASCII_CHART_MOVE_NEXT;

	// Not sign
	chart->ascii_char = (unsigned char) 0xac;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xac");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Not sign");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Soft hyphen
	chart->ascii_char = (unsigned char) 0xad;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xad");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Soft hyphen");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Registered trade mark sign
	chart->ascii_char = (unsigned char) 0xae;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xae");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Registered trade mark sign");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Spacing macron - overline
	chart->ascii_char = (unsigned char) 0xaf;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xaf");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Spacing macron - overline");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_macron      = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Degree sign
	chart->ascii_char = (unsigned char) 0xb0;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xb0");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Degree sign");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_math        = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// 	Plus-or-minus sign
	chart->ascii_char = (unsigned char) 0xb1;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xb1");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Plus-or-minus sign");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_math        = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Superscript two - squared
	chart->ascii_char = (unsigned char) 0xb2;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xb2");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Superscript two - squared");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_math        = 1;
	chart->flags.is_superscript = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Superscript three - cubed
	chart->ascii_char = (unsigned char) 0xb3;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xb3");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Superscript three - cubed");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_math        = 1;
	chart->flags.is_superscript = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Acute accent - spacing acute
	chart->ascii_char = (unsigned char) 0xb4;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xb4");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Acute accent - spacing acute");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_acute       = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Micro sign
	chart->ascii_char = (unsigned char) 0xb5;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xb5");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Micro sign");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_math        = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Pilcrow sign - paragraph sign
	chart->ascii_char = (unsigned char) 0xb6;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xb6");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Pilcrow sign - paragraph sign");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Middle dot - Georgian comma
	chart->ascii_char = (unsigned char) 0xb7;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xb7");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Middle dot - Georgian comma");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Spacing cedilla
	chart->ascii_char = (unsigned char) 0xb8;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xb8");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Spacing cedilla");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Superscript one
	chart->ascii_char = (unsigned char) 0xb9;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xb9");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Superscript one");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_math        = 1;
	chart->flags.is_superscript = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Masculine ordinal indicator
	chart->ascii_char = (unsigned char) 0xba;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xba");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Masculine ordinal indicator");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Right double angle quotes
	chart->ascii_char = (unsigned char) 0xbb;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xbb");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Right double angle quotes");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	chart->flags.is_quotation   = 1;
	ASCII_CHART_MOVE_NEXT;

	// Fraction one quarter
	chart->ascii_char = (unsigned char) 0xbc;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xbc");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Fraction one quarter");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_math        = 1;
	chart->flags.is_fraction    = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Fraction one half
	chart->ascii_char = (unsigned char) 0xbd;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xbd");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Fraction one half");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_math        = 1;
	chart->flags.is_fraction    = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Fraction three quarters
	chart->ascii_char = (unsigned char) 0xbe;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xbe");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Fraction three quarters");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_math        = 1;
	chart->flags.is_fraction    = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Inverted question mark
	chart->ascii_char = (unsigned char) 0xbf;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xbf");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Inverted question mark");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter A with grave
	chart->ascii_char = (unsigned char) 0xc0;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xc0");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter A with grave");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_grave       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter A with acute
	chart->ascii_char = (unsigned char) 0xc1;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xc1");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter A with acute");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_acute       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// 	Latin capital letter A with circumflex
	chart->ascii_char = (unsigned char) 0xc2;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xc2");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter A with circumflex");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_circumflex  = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter A with tilde
	chart->ascii_char = (unsigned char) 0xc3;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xc3");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter A with tilde");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter A with diaeresis
	chart->ascii_char = (unsigned char) 0xc4;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xc4");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter A with diaeresis");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_diaeresis   = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter A with ring above
	chart->ascii_char = (unsigned char) 0xc5;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xc5");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter A with ring above");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter AE
	chart->ascii_char = (unsigned char) 0xc6;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xc6");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter AE");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter C with cedilla
	chart->ascii_char = (unsigned char) 0xc7;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xc7");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter C with cedilla");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter E with grave
	chart->ascii_char = (unsigned char) 0xc8;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xc8");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter E with grave");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_grave       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter E with acute
	chart->ascii_char = (unsigned char) 0xc9;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xc9");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter E with acute");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_acute       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter E with circumflex
	chart->ascii_char = (unsigned char) 0xca;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xca");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter E with circumflex");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_circumflex  = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter E with diaeresis
	chart->ascii_char = (unsigned char) 0xcb;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xcb");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter E with diaeresis");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_diaeresis   = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter I with grave
	chart->ascii_char = (unsigned char) 0xcc;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xcc");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter I with grave");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_grave       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter I with acute
	chart->ascii_char = (unsigned char) 0xcd;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xcd");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter I with acute");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_acute       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter I with circumflex
	chart->ascii_char = (unsigned char) 0xce;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xce");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter I with circumflex");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_circumflex  = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter I with diaeresis
	chart->ascii_char = (unsigned char) 0xcf;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xcf");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter I with diaeresis");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_diaeresis   = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter ETH
	chart->ascii_char = (unsigned char) 0xd0;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xd0");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter ETH");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter N with tilde
	chart->ascii_char = (unsigned char) 0xd1;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xd1");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter N with tilde");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter O with grave
	chart->ascii_char = (unsigned char) 0xd2;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xd2");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter O with grave");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_grave       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter O with acute
	chart->ascii_char = (unsigned char) 0xd3;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xd3");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter O with acute");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_acute       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter O with circumflex
	chart->ascii_char = (unsigned char) 0xd4;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xd4");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter O with circumflex");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_circumflex  = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter O with tilde
	chart->ascii_char = (unsigned char) 0xd5;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xd5");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter O with tilde");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter O with diaeresis
	chart->ascii_char = (unsigned char) 0xd6;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xd6");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter O with diaeresis");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_diaeresis   = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Multiplication sign
	chart->ascii_char = (unsigned char) 0xd7;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xd7");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Multiplication sign");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_math        = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter O with slash
	chart->ascii_char = (unsigned char) 0xd8;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xd8");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter O with slash");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_slash       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter U with grave
	chart->ascii_char = (unsigned char) 0xd9;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xd9");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter U with grave");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_grave       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter U with acute
	chart->ascii_char = (unsigned char) 0xda;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xda");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter U with acute");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_acute       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter U with circumflex
	chart->ascii_char = (unsigned char) 0xdb;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xdb");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter U with circumflex");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_circumflex  = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter U with diaeresis
	chart->ascii_char = (unsigned char) 0xdc;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xdc");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter U with diaeresis");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_diaeresis   = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter Y with acute
	chart->ascii_char = (unsigned char) 0xdd;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xdd");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter Y with acute");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_acute       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin capital letter THORN
	chart->ascii_char = (unsigned char) 0xde;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xde");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin capital letter THORN");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_caps = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter sharp s - ess-zed
	chart->ascii_char = (unsigned char) 0xdf;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xdf");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter sharp s - ess-zed");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter a with grave
	chart->ascii_char = (unsigned char) 0xe0;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xe0");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter a with grave");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_grave       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter a with acute
	chart->ascii_char = (unsigned char) 0xe1;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xe1");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter a with acute");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_acute       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter a with circumflex
	chart->ascii_char = (unsigned char) 0xe2;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xe2");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter a with circumflex");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_circumflex  = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter a with tilde
	chart->ascii_char = (unsigned char) 0xe3;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xe3");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter a with tilde");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter a with diaeresis
	chart->ascii_char = (unsigned char) 0xe4;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xe4");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter a with diaeresis");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_diaeresis   = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter a with ring above
	chart->ascii_char = (unsigned char) 0xe5;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xe5");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter a with ring above");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter ae
	chart->ascii_char = (unsigned char) 0xe6;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xe6");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter ae");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter c with cedilla
	chart->ascii_char = (unsigned char) 0xe7;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xe7");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter c with cedilla");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter e with grave
	chart->ascii_char = (unsigned char) 0xe8;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xe8");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter e with grave");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_grave       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter e with acute
	chart->ascii_char = (unsigned char) 0xe9;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xe9");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter e with acute");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_acute       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter e with circumflex
	chart->ascii_char = (unsigned char) 0xea;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xea");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter e with circumflex");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_circumflex  = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter e with diaeresis
	chart->ascii_char = (unsigned char) 0xeb;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xeb");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter e with diaeresis");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_diaeresis   = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter i with grave
	chart->ascii_char = (unsigned char) 0xec;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xec");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter i with grave");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_grave       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter i with acute
	chart->ascii_char = (unsigned char) 0xed;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xed");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter i with acute");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_acute       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// 	Latin small letter i with circumflex
	chart->ascii_char = (unsigned char) 0xee;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xee");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter i with circumflex");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_circumflex  = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter i with diaeresis
	chart->ascii_char = (unsigned char) 0xef;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xef");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter i with diaeresis");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_diaeresis   = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter eth
	chart->ascii_char = (unsigned char) 0xf0;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xf0");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter eth");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// 	Latin small letter n with tilde
	chart->ascii_char = (unsigned char) 0xf1;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xf1");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter n with tilde");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter o with grave
	chart->ascii_char = (unsigned char) 0xf2;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xf2");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter o with grave");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_grave       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter o with acute
	chart->ascii_char = (unsigned char) 0xf3;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xf3");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter o with acute");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_acute       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter o with circumflex
	chart->ascii_char = (unsigned char) 0xf4;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xf4");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter o with circumflex");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_circumflex  = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter o with tilde
	chart->ascii_char = (unsigned char) 0xf5;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xf5");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter o with tilde");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter o with diaeresis
	chart->ascii_char = (unsigned char) 0xf6;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xf6");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter o with diaeresis");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_diaeresis   = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// 	Division sign
	chart->ascii_char = (unsigned char) 0xf7;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xf7");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Division sign");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_math        = 1;
	chart->flags.is_punctuation = 1;
	chart->flags.is_symbol      = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter o with slash
	chart->ascii_char = (unsigned char) 0xf8;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xf8");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter o with slash");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_slash       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// 	Latin small letter u with grave
	chart->ascii_char = (unsigned char) 0xf9;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xf9");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter u with grave");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_grave       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter u with acute
	chart->ascii_char = (unsigned char) 0xfa;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xfa");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter u with acute");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_acute       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter u with circumflex
	chart->ascii_char = (unsigned char) 0xfb;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xfb");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter u with circumflex");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_circumflex  = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter u with diaeresis
	chart->ascii_char = (unsigned char) 0xfc;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xfc");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter u with diaeresis");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_diaeresis   = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter y with acute
	chart->ascii_char = (unsigned char) 0xfd;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xfd");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter y with acute");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_acute       = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter thorn
	chart->ascii_char = (unsigned char) 0xfe;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xfe");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter thorn");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;
	ASCII_CHART_MOVE_NEXT;

	// Latin small letter y with diaeresis
	chart->ascii_char = (unsigned char) 0xff;
	chart->symbol_abbreviation = ASCII_CHART_ADDSTR("\xff");
	chart->symbol_description  = ASCII_CHART_ADDSTR("Latin small letter y with diaeresis");
	chart->flags.is_printable   = 1;
	chart->flags.is_extended    = 1;
	chart->flags.is_diaeresis   = 1;
	chart->flags.is_latin       = 1;
	chart->flags.is_latin_nonstandard_lower = 1;


	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TABLE FINISHED
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// return indicating success
	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Chart Search Routines (fast/additive index lookups) %%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Returns a chart entry based on the search character provided, and the assorted flags.  Will return
// null if a character does not match with the provided flags given.  To omit flag checks and to just
// run a fast lookup, provide the flags parameter as a NULL pointer value.
P_BP_ASCII_CHART_ENTRY BP_LookupASCIIChar(P_BP_ASCII_CHARACTER_MAP char_map, unsigned char character, P_BP_ASCII_CHAR_FLAGS flags)
{

	// first ensure that we have a map to search
	if(!char_map)
		return NULL;

	// ensure we have an ascii chart and the correct number of
	// entries.
	if(!char_map->ascii_chart || (char_map->ascii_chart_entry_n != 0xff))
		return NULL;

	// set the entry
	P_BP_ASCII_CHART_ENTRY char_entry = &char_map->ascii_chart[character];

	// if the flags value has been passed in, attempt a compare.
	if(flags)
	if(memcmp(flags, &char_entry->flags, sizeof(BP_ASCII_CHAR_FLAGS)) != 0)
		return NULL;

	// return the entry if found
	return char_entry;

}

// search the ascii table for entries matching the provided flags
P_BP_ASCII_CHARACTER_MAP_SEARCH_RESULTS BP_CreateSearchMapFromASCIITable(P_BP_ASCII_CHARACTER_MAP char_map, P_BP_ASCII_CHAR_FLAGS flags, BP_ASCII_CHARACTER_SEARCH_FLAG_VALUES search_flags)
{

	// first ensure that we have a map to search
	if(!char_map)
		return NULL;

	// ensure we have a number of entries
	if(!char_map->ascii_chart_entry_n || !char_map->ascii_chart)
		return NULL;

	// simple flag set for determining flag match configurations
	BP_FLAGS_T match_flags = 0;

	// attempt to allocate initial space for the search results
	P_BP_ASCII_CHARACTER_MAP_SEARCH_RESULTS search_results = (P_BP_ASCII_CHARACTER_MAP_SEARCH_RESULTS) bpcalloc(sizeof(BP_ASCII_CHARACTER_MAP_SEARCH_RESULTS), 1);

	// ensure we've created the results structure, if not, exit
	if(!search_results)
		return NULL;

	// attempt to allocate new tail queue allocator
	search_results->tq = BP_CreateLinkL("search results", BP_FALSE);

	// exit if we cannot create tail queue
	if(!search_results->tq)
	{
		bpfree(search_results);
		return NULL;
	}


	// Walk the chart and look for matches.
	size_t n = 0;
	for(; n < char_map->ascii_chart_entry_n; n++)
	{

		// unset all match flags
		BP_UNSET_ALL_FLAGS(match_flags);

		// Compare character flags and create compare flags by analyzing both (don't think
		// too hard about this)
		match_flags = BP_CompareASCIICharFlags(flags, &char_map->ascii_chart[n].flags);

		// Uncomment for debugging if you want.  Looks messy.
		// BP_DisplayASCIICompareMatchFlags(match_flags);

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Now See Which Flags Are Set %%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// switch on the search flag configuration the user has supplied (this
		// is provided to the routine as a function parameter
		switch(search_flags)
		{

			// if the user has selected to search on negative maps, check/add here
			case BP_ASCII_CHAR_SEARCH_NEGATIVE_MATCH:

				// check if the negative flag has been set (no perfect match)
				if(BP_FLAG_IS_SET(match_flags, BP_ASCII_CHAR_SEARCH_NEGATIVE_MATCH))
					BP_AddCharacterReferenceToSearchResults(search_results, &char_map->ascii_chart[n]);

				break;

			// if the user has selected to search on a perfect positive match, check/add here
			case BP_ASCII_CHAR_SEARCH_POSITIVE_MATCH:

				// check if the perfect positive match has been set
				if(BP_FLAG_IS_SET(match_flags, BP_ASCII_CHAR_SEARCH_POSITIVE_MATCH))
					BP_AddCharacterReferenceToSearchResults(search_results, &char_map->ascii_chart[n]);

				break;

			// if the user has selected to search on a partial positive match, check/add here
			case BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH:

				// check if the partial positive flag has been set (no perfect match)
				if(BP_FLAG_IS_SET(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH))
					BP_AddCharacterReferenceToSearchResults(search_results, &char_map->ascii_chart[n]);

				break;

			// if the user has selected to search on a partial negative match, check/add here
			case BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH:

				// check if the partial negative flag has been set
				if(BP_FLAG_IS_SET(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH))
					BP_AddCharacterReferenceToSearchResults(search_results, &char_map->ascii_chart[n]);

				break;

			// if the user has selected to search on all flag criteria matched, check/add here
			case BP_ASCII_CHAR_SEARCH_ALL_SEARCH_FLAG_CONDITIONS_MET:

				// check if search conditions flag is set
				if(BP_FLAG_IS_SET(match_flags, BP_ASCII_CHAR_SEARCH_ALL_SEARCH_FLAG_CONDITIONS_MET))
					BP_AddCharacterReferenceToSearchResults(search_results, &char_map->ascii_chart[n]);

				break;

			// if the user has selected to search on all flag criteria matched negatively, check/add here
			case BP_ASCII_CHAR_SEARCH_ALL_SEARCH_FLAG_CONDITIONS_MET_NEGATIVELY:

				// check if negative match is set
				if(BP_FLAG_IS_SET(match_flags, BP_ASCII_CHAR_SEARCH_ALL_SEARCH_FLAG_CONDITIONS_MET_NEGATIVELY))
					BP_AddCharacterReferenceToSearchResults(search_results, &char_map->ascii_chart[n]);

			default:
				break;

		}

	}

	// return the generate search results
	return search_results;


}

// This simply adds a specified char to a search result stack.  It is used primarily
// by BP_SearchASCIITable() to stack up matches.
BP_ERROR_T BP_AddCharacterReferenceToSearchResults(P_BP_ASCII_CHARACTER_MAP_SEARCH_RESULTS results, P_BP_ASCII_CHART_ENTRY char_entry)
{

	// ensure we have a map search reference
	if(!results)
		return ERR_FAILURE;

	// ensure we have a character entry to add
	if(!char_entry)
		return ERR_FAILURE;

	// increase the number of results
	results->result_n++;

	// reallocate space to allow entry storage
	results->results = (P_BP_ASCII_CHART_ENTRIES) tq_realloc
	(
		results->tq,
		results->results,
		((results->result_n+1) * sizeof(P_BP_ASCII_CHART_ENTRY)),
		BPLN
	);

	// reallocation can fail, if it does, return indicating so
	if(!results->results)
		return ERR_FAILURE;

	// set reference
	results->results[results->result_n-1] = char_entry;

	// null terminate array
	results->results[results->result_n] = NULL;

	// return indicating success
	return ERR_SUCCESS;

}

// Destroys the results.  After calling this on a valid results structure, all memory including
// the results structure itself will be freed.
BP_ERROR_T BP_DestroyASCIISearchResults(P_BP_ASCII_CHARACTER_MAP_SEARCH_RESULTS results)
{

	// NOTE:  Do not destroy individual entries as they are just references to an
	//        existing character map.  They are destroyed when the actual character
	//        map is destroyed.

	// ensure we have a results structure
	if(!results)
		return ERR_FAILURE;

	// destroy the tail queue allocator (destroys all allocated memory)
	if(results->tq)
	{
		BP_DestroyLinkL(results->tq, BP_TRUE);
	}

	// destroy the actual structure
	bpfree(results);

	// return indicating success
	return ERR_SUCCESS;

}

// Return the number of flags which are set (BP_TRUE) within a BP_ASCII_CHAR_FLAGS structure.  Primarily used for
// matching flags.
size_t BP_CountNumberASCIICharFlagsSet(P_BP_ASCII_CHAR_FLAGS flags)
{

	// ensure we have a valid pointer
	if(!flags)
		return ERR_FAILURE;

	// flag counts
	size_t flag_count = 0;

	// set flags as appropriate
	if(flags->is_control)
		flag_count++;
	if(flags->is_alphalower)
		flag_count++;
	if(flags->is_alphacaps)
		flag_count++;
	if(flags->is_number)
		flag_count++;
	if(flags->is_printable)
		flag_count++;
	if(flags->is_nonprintable)
		flag_count++;
	if(flags->is_extended)
		flag_count++;
	if(flags->is_math)
		flag_count++;
	if(flags->is_superscript)
		flag_count++;
	if(flags->is_currency)
		flag_count++;
	if(flags->is_fraction)
		flag_count++;
	if(flags->is_acute)
		flag_count++;
	if(flags->is_grave)
		flag_count++;
	if(flags->is_diaeresis)
		flag_count++;
	if(flags->is_slash)
		flag_count++;
	if(flags->is_circumflex)
		flag_count++;
	if(flags->is_caron)
		flag_count++;
	if(flags->is_macron)
		flag_count++;
	if(flags->is_punctuation)
		flag_count++;
	if(flags->is_symbol)
		flag_count++;
	if(flags->is_latin)
		flag_count++;
	if(flags->is_latin_nonstandard_caps)
		flag_count++;
	if(flags->is_latin_nonstandard_lower)
		flag_count++;
	if(flags->is_quotation)
		flag_count++;
	if(flags->is_whitespace)
		flag_count++;
	if(flags->is_undefined)
		flag_count++;

	// return the total flag count
	return flag_count;

}


// Returns a BP_FLAGS_T value set with any combination of BP_ASCII_CHARACTER_SEARCH_FLAG_VALUES enumerated
// values.
BP_FLAGS_T BP_CompareASCIICharFlags(P_BP_ASCII_CHAR_FLAGS flags_compare_this, P_BP_ASCII_CHAR_FLAGS flags_compare_with_this)
{

	// ensure we have two sets of flags to compare
	if(!flags_compare_this || !flags_compare_with_this)
		return BP_ASCII_CHAR_SEARCH_NONE;

	// count for matches (used to determine if we have an inbound search perfect partial match)
	size_t compare_flag_count = BP_CountNumberASCIICharFlagsSet(flags_compare_this);

	// match flag count
	size_t match_flag_count = 0;

	// match negative count
	size_t match_flag_negative_count = 0;


	// declare and initialize flags (this is a bitfield flag-set which
	// is the eventual return value for this routine)
	BP_FLAGS_T match_flags = 0;


	// set based on pure positive/negative match
	if(memcmp(flags_compare_this, flags_compare_with_this, sizeof(BP_ASCII_CHAR_FLAGS)) == 0)
	{

		// set indicator showing we have a perfect match
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_POSITIVE_MATCH);

		// since we have a perfect match, we know all flag values were set ok
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_ALL_SEARCH_FLAG_CONDITIONS_MET);

		// return the flags
		return match_flags;

	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_NEGATIVE_MATCH);
	}

	// is control match
	if(flags_compare_this->is_control == 1)
	if(flags_compare_this->is_control == flags_compare_with_this->is_control)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	// is alphalower match
	if(flags_compare_this->is_alphalower)
	if(flags_compare_this->is_alphalower == flags_compare_with_this->is_alphalower)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	// is alphacaps match
	if(flags_compare_this->is_alphacaps)
	if(flags_compare_this->is_alphacaps == flags_compare_with_this->is_alphacaps)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_number)
	if(flags_compare_this->is_number  == flags_compare_with_this->is_number)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_printable)
	if(flags_compare_this->is_printable  == flags_compare_with_this->is_printable)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_nonprintable)
	if(flags_compare_this->is_nonprintable  == flags_compare_with_this->is_nonprintable)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_extended)
	if(flags_compare_this->is_extended  == flags_compare_with_this->is_extended)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_math)
	if(flags_compare_this->is_math  == flags_compare_with_this->is_math)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_superscript)
	if(flags_compare_this->is_superscript  == flags_compare_with_this->is_superscript)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_currency)
	if(flags_compare_this->is_currency  == flags_compare_with_this->is_currency)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_fraction)
	if(flags_compare_this->is_fraction  == flags_compare_with_this->is_fraction)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_acute)
	if(flags_compare_this->is_acute  == flags_compare_with_this->is_acute)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_grave)
	if(flags_compare_this->is_grave  == flags_compare_with_this->is_grave)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_diaeresis)
	if(flags_compare_this->is_diaeresis  == flags_compare_with_this->is_diaeresis)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_slash)
	if(flags_compare_this->is_slash  == flags_compare_with_this->is_slash)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_circumflex)
	if(flags_compare_this->is_circumflex  == flags_compare_with_this->is_circumflex)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_caron)
	if(flags_compare_this->is_caron  == flags_compare_with_this->is_caron)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_macron)
	if(flags_compare_this->is_macron  == flags_compare_with_this->is_macron)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_punctuation)
	if(flags_compare_this->is_punctuation  == flags_compare_with_this->is_punctuation)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_symbol)
	if(flags_compare_this->is_symbol  == flags_compare_with_this->is_symbol)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_latin)
	if(flags_compare_this->is_latin  == flags_compare_with_this->is_latin)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_latin_nonstandard_caps)
	if(flags_compare_this->is_latin_nonstandard_caps  == flags_compare_with_this->is_latin_nonstandard_caps)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_latin_nonstandard_lower)
	if(flags_compare_this->is_latin_nonstandard_lower  == flags_compare_with_this->is_latin_nonstandard_lower)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_quotation)
	if(flags_compare_this->is_quotation  == flags_compare_with_this->is_quotation)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_whitespace)
	if(flags_compare_this->is_whitespace  == flags_compare_with_this->is_whitespace)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}

	if(flags_compare_this->is_undefined)
	if(flags_compare_this->is_undefined  == flags_compare_with_this->is_undefined)
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH);
		match_flag_count++;
	}
	else
	{
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH);
		match_flag_negative_count++;
	}



	// if we have matched the correct number of flags, we can safely consider
	// all conditions met.
	if(compare_flag_count == match_flag_count)
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_ALL_SEARCH_FLAG_CONDITIONS_MET);

	if(compare_flag_count == match_flag_negative_count)
		BP_SET_FLAG(match_flags, BP_ASCII_CHAR_SEARCH_ALL_SEARCH_FLAG_CONDITIONS_MET_NEGATIVELY);



	// return the matched flags
	return match_flags;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Chart Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// displays one singular chart entry
BP_ERROR_T BP_DisplayASCIIChartEntry(P_BP_ASCII_CHART_ENTRY chart_entry)
{

	// display the chart entry
	if(!chart_entry)
		return ERR_FAILURE;

	// display the chart entry
	printf("\n\t [-] Chart Entry: %p (\\x%.02x)", chart_entry, chart_entry->ascii_char);

	// display character if marked as printable
	if(chart_entry->flags.is_printable)
	{

		if(chart_entry->flags.is_extended)
			printf("\n\t\t character:     (extended ascii, not suitable for terminal display)");
		else
			printf("\n\t\t character:     %c", chart_entry->ascii_char);

	}
	else
	{

		printf("\n\t\t character: (character is marked non-printable)", chart_entry->ascii_char);

	}

	// display the symbol abbreviation
	if(chart_entry->symbol_abbreviation)
	{
		if(chart_entry->flags.is_extended)
			printf("\n\t\t abbreviation:  (no abbreviation for extended ascii)");
		else
			printf("\n\t\t abbreviation:  %s", chart_entry->symbol_abbreviation);
	}

	// display the symbol description
	if(chart_entry->symbol_description)
	{
		printf("\n\t\t description:   %s", chart_entry->symbol_description);
	}

	// display the html numeric code
	if(chart_entry->html_numeric_code)
	{
		printf("\n\t\t html numeric:  %s", chart_entry->html_numeric_code);
	}

	// display the html name
	if(chart_entry->html_name)
	{
		printf("\n\t\t html_name:     %s", chart_entry->html_name);
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Now display set flags %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create a zero flags entry to check to see if the character has any flags
	BP_ASCII_CHAR_FLAGS zero_flags;

	// zero out entry
	memset(&zero_flags, 0x00, sizeof(BP_ASCII_CHAR_FLAGS));

	// if we have no character flags to display, return here.
	if(memcmp(&chart_entry->flags, &zero_flags, sizeof(BP_ASCII_CHAR_FLAGS)) == 0)
		return ERR_SUCCESS;

	printf("\n\n\t\t  [-] flags: ");

	if(chart_entry->flags.is_control)
	{
		printf("\n\t\t    flags.is_control");
	}

	if(chart_entry->flags.is_alphalower)
	{
		printf("\n\t\t    flags.is_alphalower");
	}

	if(chart_entry->flags.is_alphacaps)
	{
		printf("\n\t\t    flags.is_alphacaps");
	}

	if(chart_entry->flags.is_number)
	{
		printf("\n\t\t    flags.is_number");
	}

	if(chart_entry->flags.is_printable)
	{
		printf("\n\t\t    flags.is_printable");
	}

	if(chart_entry->flags.is_nonprintable)
	{
		printf("\n\t\t    flags.is_nonprintable");
	}

	if(chart_entry->flags.is_extended)
	{
		printf("\n\t\t    flags.is_extended");
	}

	if(chart_entry->flags.is_math)
	{
		printf("\n\t\t    flags.is_math");
	}

	if(chart_entry->flags.is_superscript)
	{
		printf("\n\t\t    flags.is_superscript");
	}

	if(chart_entry->flags.is_currency)
	{
		printf("\n\t\t    flags.is_currency");
	}

	if(chart_entry->flags.is_fraction)
	{
		printf("\n\t\t    flags.is_fraction");
	}

	if(chart_entry->flags.is_acute)
	{
		printf("\n\t\t    flags.is_acute");
	}

	if(chart_entry->flags.is_grave)
	{
		printf("\n\t\t    flags.is_grave");
	}

	if(chart_entry->flags.is_diaeresis)
	{
		printf("\n\t\t    flags.is_diaeresis");
	}

	if(chart_entry->flags.is_slash)
	{
		printf("\n\t\t    flags.is_slash");
	}

	if(chart_entry->flags.is_circumflex)
	{
		printf("\n\t\t    flags.is_circumflex");
	}

	if(chart_entry->flags.is_caron)
	{
		printf("\n\t\t    flags.is_caron");
	}

	if(chart_entry->flags.is_macron)
	{
		printf("\n\t\t    flags.is_macron");
	}

	if(chart_entry->flags.is_punctuation)
	{
		printf("\n\t\t    flags.is_punctuation");
	}

	if(chart_entry->flags.is_symbol)
	{
		printf("\n\t\t    flags.is_symbol");
	}

	if(chart_entry->flags.is_latin)
	{
		printf("\n\t\t    flags.is_latin");
	}

	if(chart_entry->flags.is_latin_nonstandard_caps)
	{
		printf("\n\t\t    flags.is_latin_nonstandard_caps");
	}

	if(chart_entry->flags.is_latin_nonstandard_lower)
	{
		printf("\n\t\t    flags.is_latin_nonstandard_lower");
	}

	if(chart_entry->flags.is_quotation)
	{
		printf("\n\t\t    flags.is_quotation");
	}

	if(chart_entry->flags.is_whitespace)
	{
		printf("\n\t\t    flags.is_whitespace");
	}

	if(chart_entry->flags.is_undefined)
	{
		printf("\n\t\t    flags.is_undefined");
	}

	// return indicating success
	return ERR_SUCCESS;

}

// Displays entries in the character map provided.  If a flags parameter is passed in, only the entries
// that match the specific flag configuration are displayed.
BP_ERROR_T BP_DisplayASCIICharacterMap(P_BP_ASCII_CHARACTER_MAP char_map, P_BP_ASCII_CHAR_FLAGS flags)
{

	// ensure we have a character map pointer
	if(!char_map)
		return ERR_FAILURE;

	// display ascii chart header information
	printf("\n [+] Displaying ASCII Character Map (%p)\n", char_map);
	printf("\n\t tail queue allocator:        %p", char_map->tq);
	printf("\n\t ascii_chart:                 %p", char_map->ascii_chart);
	printf("\n\t ascii_chart_entry_n:         %zu", char_map->ascii_chart_entry_n);

	// if we have no entries or whatnot, return here
	if(!char_map->ascii_chart_entry_n || !char_map->ascii_chart)
		return ERR_SUCCESS;

	// walk all entries in the chart and display individual results
	size_t n = 0;
	for(; n < char_map->ascii_chart_entry_n; n++)
	{

		// add a newline
		printf("\n");

		// display the individual ascii chart entry
		BP_DisplayASCIIChartEntry(&char_map->ascii_chart[n]);

	}

	// return indicating success
	return ERR_SUCCESS;

}

// Displays ascii character map search results.  If a flags parameter is passed in, only the entries
// that match the specific flag configuration are displayed.
BP_ERROR_T BP_DisplayASCIICharacterMapSearchResults(P_BP_ASCII_CHARACTER_MAP_SEARCH_RESULTS search_results, P_BP_ASCII_CHAR_FLAGS flags)
{

	// always display search map pointer
	printf("\n [+] ASCII Character Map Search Results (%p)", search_results);

	// ensure we have search results
	if(!search_results)
		return ERR_FAILURE;

	// display search results header
	printf("\n\t tail queue allocator:  %p", search_results->tq);
	printf("\n\t results:               %p", search_results->results);
	printf("\n\t result_n:              %zu", search_results->result_n);
	printf("\n");

	// display header if we have entries
	if(search_results->result_n)
		printf("\n\t [-] Search Result Entries");

	// iterate through match entries
	size_t n = 0;
	for(; n < search_results->result_n; n++)
	{

		// display the individual ascii chart entry
		BP_DisplayASCIIChartEntry(search_results->results[n]);

		// display newline between entries
		printf("\n");

	}

	printf("\n");

	// return indicating success
	return ERR_SUCCESS;

}

// Simple routine designed to display the settings within a flags bitfield.  The display routine
// will display which flags have been set.  This is not to be confused with the flags structure
// used within the ascii character map.  This works as a BITFIELD.
BP_ERROR_T BP_DisplayASCIICompareMatchFlags(BP_FLAGS_T match_flags)
{

	// display the matches within the flag set
	printf("\n [+] ASCII Compare Match Flags:");
	printf("\n\t - pure negative match:                     %s", BP_FLAG_IS_SET(match_flags, BP_ASCII_CHAR_SEARCH_NEGATIVE_MATCH) ? "yes" : "no");
	printf("\n\t - pure positive match:                     %s", BP_FLAG_IS_SET(match_flags, BP_ASCII_CHAR_SEARCH_POSITIVE_MATCH) ? "yes" : "no");
	printf("\n\t - partial positive match:                  %s", BP_FLAG_IS_SET(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_POSITIVE_MATCH) ? "yes" : "no");
	printf("\n\t - partial negative match:                  %s", BP_FLAG_IS_SET(match_flags, BP_ASCII_CHAR_SEARCH_PARTIAL_NEGATIVE_MATCH) ? "yes" : "no");
	printf("\n\t - all search conditions met:               %s", BP_FLAG_IS_SET(match_flags, BP_ASCII_CHAR_SEARCH_ALL_SEARCH_FLAG_CONDITIONS_MET) ? "yes" : "no");
	printf("\n\t - all search conditions met (negatively):  %s", BP_FLAG_IS_SET(match_flags, BP_ASCII_CHAR_SEARCH_ALL_SEARCH_FLAG_CONDITIONS_MET_NEGATIVELY) ? "yes" : "no");

	// return indicating success
	return ERR_SUCCESS;

}



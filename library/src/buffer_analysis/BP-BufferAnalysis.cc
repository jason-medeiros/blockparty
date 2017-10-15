/*
 * BP-BufferAnalysis.cc
 *
 *  Created on: Jul 24, 2014
 *      Author: root
 */

// include main functions
#include "../../include/BP-Main.h"



/*
 *
 * Main Buffer Analysis Routine:
 * ==================================================
 * Pass in NULL values to disable any analysis configuration subsystem.  The
 * output structure needs to be freed with BP_DestroyBufferAnalysis.
 *
 */

P_BP_BUFFER_ANALYSIS BP_CreateBufferAnalysis
(
		unsigned char *buffer,
		size_t         buffer_n,
		P_BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION ascii_config
)
{


	// ensure we have a buffer and a length count
	if(!buffer || !buffer_n)
		return NULL;

	// create local buffer
	BP_BUFFER_ANALYSIS_ASCII_CONFIGURATION ascii_config_local;

	// zero out the structure (automagic macro)
	BP_ZERO_STACK(ascii_config_local);


	// create default ascii config if no parameter is supplied
	if(!ascii_config)
	{

		// override parameter here
		ascii_config = &ascii_config_local;

		// set some flags in the structure (default is BP_FALSE aka 0)
		ascii_config->count_ascii                      = BP_TRUE;
		ascii_config->count_extended                   = BP_TRUE;
		ascii_config->count_individual_characters      = BP_TRUE;

	}

	// attempt to create outermost allocation
	P_BP_BUFFER_ANALYSIS analysis = (P_BP_BUFFER_ANALYSIS) bpcalloc(sizeof(BP_BUFFER_ANALYSIS), 1);

	// ensure analysis was allocated ok
	if(!analysis)
		return NULL;

	// now attempt to create tailqueue allocator for this analysis
	analysis->tq = BP_CreateLinkL("Buffer Analysis Allocator", BP_TRUE);

	// ensure that the tail queue allocator was created ok
	if(!analysis->tq)
	{
		BP_DestroyBufferAnalysis(analysis);
		return NULL;
	}


	// now set buffer and buffer size values within the
	// analysis structure.
	analysis->buffer   = buffer;
	analysis->buffer_n = buffer_n;


	// if the user passed in an ascii configuration option, run here
	if(ascii_config)
	{

		// Attempt to create the analysis, if we can't create the analysis and a valid
		// config was provided, we've likley run out of memory, so exit after destroying
		// the analysis structure.
		if(BP_CreateBufferAnalysisASCII(analysis, ascii_config) == ERR_FAILURE)
		{
			BP_DestroyBufferAnalysis(analysis);
			return NULL;
		}

	}


	// return the full buffer analysis
	return analysis;

}


// destroys any memory associated with a buffer analysis, including that which was generated by any subsystems
BP_ERROR_T BP_DestroyBufferAnalysis(P_BP_BUFFER_ANALYSIS analysis)
{

	// exit if no analysis is provided
	if(!analysis)
		return ERR_FAILURE;

	// destroy sub structures first
	if(analysis->ascii)
	{

		// destroy the char map (only dangling non-tq allocated memory)
		if(analysis->ascii->char_map)
			BP_DestroyASCIICharacterMap(analysis->ascii->char_map);

	}

	// destroy the tailqueue allocator
	if(analysis->tq)
	{

		// destroy the linkl
		BP_DestroyLinkL(analysis->tq, BP_TRUE);

	}

	// destroy the analysis
	bpfree(analysis);

	// return indicating success
	return ERR_SUCCESS;

}

// displays a buffer analysis structure and all provided sub-structures
BP_ERROR_T BP_DisplayBufferAnalysis(P_BP_BUFFER_ANALYSIS analysis)
{

	// ensure we have an analysis structure at the bare minimum
	if(!analysis)
		return ERR_FAILURE;

	// display the analysis header
	printf("\n [+] Buffer Analysis (%p)", analysis);
	printf("\n");
	printf("\n\t tailqueue allocator:    %p", analysis->tq);
	printf("\n\t buffer:                 %p", analysis->buffer);
	printf("\n\t buffer_n:               %zu", analysis->buffer_n);

	// display ascii analysis if a structure is set
	if(analysis->ascii)
	{

		// add newline
		printf("\n");

		// display the ascii analysis (takes a buff analysis structure, not a specific
		// ascii analysis structure)
		BP_DisplayBufferAnalysisASCII(analysis);


	}

	// return indicating success
	return ERR_SUCCESS;

}


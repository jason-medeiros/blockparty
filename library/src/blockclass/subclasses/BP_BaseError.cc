/*
 * BP_BaseError.cc
 *
 *  Created on: Feb 22, 2011
 *      Author: root
 */

/*
 * BP_BaseError.cpp
 *
 *  Created on: Feb 3, 2011
 *      Author: root
 */

#include "../../../include/BP-Main.h"

//! Class constructor.
BP_BaseError::BP_BaseError()
{

	// set errors and warnings to null
	this->error_stack   = NULL;
	this->warning_stack = NULL;
	this->contains_errors   = false;
	this->contains_warnings = false;
	size_t random_id = BP_GenRandomUnsignedLong(1, 0xffffffff);
	this->m = BP_CreateLinkL("bperror", BP_TRUE);

}


// deconstructor
BP_BaseError::~BP_BaseError()
{

	if(this->contains_warnings)
		tq_free(this->m, (void *) this->warning_stack);

	if(this->contains_errors)
		tq_free(this->m, (void *) this->error_stack);

	if(this->m->total_allocations)
	{
		printf("\n [+] Error: Destroying BP_BaseError object with chunks remaining.");
		BP_DisplayLinkL(this->m);
	}

	// clean up the allocations (nothing if they're empty)
	BP_DestroyLinkL(this->m, BP_TRUE);
	this->m = NULL;

}

//! Adds an error to the internal error stack.
bool BP_BaseError::AddErr(char *msg)
{

	// allocate a new errors structure if needed
	if(!this->error_stack)
	{
		this->error_stack = (P_BP_ERROR) bpcalloc(sizeof(BP_ERROR), 1);
	}

	// increment error count
	this->error_stack->error_count++;

	// bpreallocate the array to create space for new item
	this->error_stack->errors = (char **) bprealloc
	(
		this->error_stack->errors,
		sizeof(char *) * this->error_stack->error_count
	);


	// store the new error message in the error stack
	this->error_stack->errors[this->error_stack->error_count-1] = (char *) bpstrndup(msg, BP_BASE_ERROR_MAX_ERR_LEN);

	// set error flag to true
	this->contains_errors = true;

	// return indicating success
	return true;
}

//! Adds a warning to the internal warning stack.
bool BP_BaseError::AddWarn(char *msg)
{

	// allocate a new warnings structure if needed
	if(!this->warning_stack)
	{
		this->warning_stack = (P_BP_WARNING) bpcalloc(sizeof(BP_WARNING), 1);
	}

	// increment warning count
	this->warning_stack->warning_count++;

	// bpreallocate the array to create space for new item
	this->warning_stack->warnings = (char **) bprealloc
	(
		this->warning_stack->warnings,
		sizeof(char *) * this->warning_stack->warning_count
	);


	// store the new warning message in the warning stack
	this->warning_stack->warnings[this->warning_stack->warning_count-1] = (char *) bpstrndup(msg, BP_BASE_ERROR_MAX_ERR_LEN);

	// set warning flag to true
	this->contains_warnings = true;

	return true;
}

//! Display all errors in error stack.
bool BP_BaseError::DisplayErrors()
{

	// check to verify we have an error stack before printing
	if(!this->error_stack)
	{

		printf("\n [+] Error stack is empty.");
		return false;

	}

	// loop iterator for error messages
	size_t i = 0;
	for
	(
		;
		this->error_stack->error_count > i;
		i++
	)
	{
		// display error to stdout
		printf("\n[x] %s", this->error_stack->errors[i]);
	}

	return true;
}

//! Display all errors in error stack.
bool BP_BaseError::DisplayWarnings()
{

	// check to verify we have an warning stack before printing
	if(!this->warning_stack)
	{

		printf("\n [+] Warning stack is empty.");
		return false;

	}

	// loop iterator for warning messages
	size_t i = 0;
	for
	(
		;
		this->warning_stack->warning_count > i;
		i++
	)
	{
		// display warning to stdout
		printf("\n[+] %s", this->warning_stack->warnings[i]);
	}

	return true;

}

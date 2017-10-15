/*
 * BP_BaseError.h
 *
 *  Created on: Feb 22, 2011
 *      Author: root
 */

#ifndef BP_BASEERROR_H_
#define BP_BASEERROR_H_



//! Max length in bytes a error message can fill.
#define BP_BASE_ERROR_MAX_ERR_LEN 4096



//! holds errors that are added in the class
typedef struct _BP_ERRORS {

	// error messages
	char ** errors;

	// error count
	size_t error_count;

} BP_ERROR, *P_BP_ERROR;


//! holds warnings that are added in the class
typedef struct _BP_WARNINGS {

	// warning messages
	char ** warnings;

	// warning count
	size_t warning_count;

} BP_WARNING, *P_BP_WARNING;


//! This is the base error class from which ALL project classes inherit from.  This class allows the manipulation of error reporting related capability.
class BP_BaseError
{

	public:

		// allocator
		P_BP_LINKL m;

		// error and warnings stacks
		P_BP_ERROR   error_stack;
		P_BP_WARNING warning_stack;

		// simple flags for determining if an error/warning occured
		bool contains_warnings;
		bool contains_errors;

		// constructor (sets local parms)
		BP_BaseError();

		// deconstructor
		~BP_BaseError();

		// add error (must watch bpstrlen as msg is strdup'd, also input must be watched for format strings if printf visualization is used)
		bool AddErr(char * msg);

		// add a warning to the warning stack (must watch bpstrlen as msg is strdup'd, also input must be watched for format strings if printf visualization is used)
		bool AddWarn(char *msg);

		// display errors
		bool DisplayErrors();

		// display warnings
		bool DisplayWarnings();

};

#endif /* BP_BASEERROR_H_ */

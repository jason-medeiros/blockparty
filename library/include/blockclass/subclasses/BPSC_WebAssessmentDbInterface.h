/*
 * BPSC_WebAssessmentDbInterface.h
 *
 *  Created on: Nov 19, 2011
 *      Author: root
 */

#ifndef BPSC_WEBASSESSMENTDBINTERFACE_H_
#define BPSC_WEBASSESSMENTDBINTERFACE_H_


//! Class for operating on the web assessment db.
class BPSC_WebAssessmentDbInterface : public BP_BaseError
{

	// mysql interface
	Block *mysqli;

public:

	//! Class constructor.
	BPSC_WebAssessmentDbInterface(Block *mysqli);

	//! Assessment data.
	P_BP_WEB_ASSESSMENT assessment;

	//! Store the assessment data.
	BP_ERROR_T storeAssessment();

	//! Store sqli test results.
	BP_ERROR_T storeSqliData();

	//! Store xss test results.
	BP_ERROR_T storeXssData();

	//! Store get/post data for request.
	BP_ERROR_T storeGetData();

	//! Store post data.
	BP_ERROR_T storePostData();

};

#endif /* BPSC_WEBASSESSMENTDBINTERFACE_H_ */

/*
 * BP_WebFormSubmitter.h
 *
 *  Created on: May 17, 2013
 *      Author: root
 */

#ifndef BP_WEBFORMSUBMITTER_H_
#define BP_WEBFORMSUBMITTER_H_

// values to override when making a submit (e.g. username/password fields)
typedef struct __FORM_SUBMIT_OVERRIDE_INPUT_FIELDS {

	char *input_name;
	char *input_override_val;

} FORM_SUBMIT_OVERRIDE_INPUT_FIELDS, *P_FORM_SUBMIT_OVERRIDE_INPUT_FIELDS;

// add form field custom behavior
typedef struct __FORM_FIELD_SUBMIT_CUSTOM_BEHAIVOR {

	// name of the field to manipulate
	char *field_name;

	// if this is true, never send the field in a request
	bool do_not_send;

} FORM_FIELD_SUBMIT_CUSTOM_BEHAIVOR, *P_FORM_FIELD_SUBMIT_CUSTOM_BEHAIVOR;

class BP_WebFormSubmitter : BP_BaseError
{
public:

	// request object to use
	Block * request;

	// constructor
	BP_WebFormSubmitter();

	// destructor
	~BP_WebFormSubmitter();

	// set the override fields
	P_FORM_SUBMIT_OVERRIDE_INPUT_FIELDS override_fields;

	// count of fields in the override set
	size_t override_fields_n;

	// stack of fields which pertain to custom behaivor
	P_FORM_FIELD_SUBMIT_CUSTOM_BEHAIVOR field_custom_behaivors;

	//  number of items in the array
	size_t field_custom_behavior_n;

	// base url request to use in the case of a relative link

	// add field override element
	BP_ERROR_T addInputOverrideField(char * input_name, char *new_input_value);

	// retrieve an input override field by name (if it exists, null if not)
	P_FORM_SUBMIT_OVERRIDE_INPUT_FIELDS getInputOverrideByName(char * input_name);

	// updates an input override field by name (frees content (not name) and replaces it with specified
	// data (which is allocated)
	BP_ERROR_T updateInputOverrideByNmae(char * input_name, char *new_content);

	// add a field custom behavior
	BP_ERROR_T addInputFieldBehaviorModification(char * input_name, bool do_not_send);

	// retrieve a custom behavior by name
	P_FORM_FIELD_SUBMIT_CUSTOM_BEHAIVOR getFieldBehaviorModificationByName(char * input_name);

	// destroy the overrride fields
	BP_ERROR_T destroyInputOverrides();


	// submit the form (result data will be in the request data)
	BP_ERROR_T submitForm(P_BP_HTML_FORM_LIST form_element, P_FORM_SUBMIT_OVERRIDE_INPUT_FIELDS override_fields);

};


#endif /* BP_WEBFORMSUBMITTER_H_ */

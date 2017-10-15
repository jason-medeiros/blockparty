/*
 * BP_WebFormSubmitter.cc
 *
 *  Created on: May 17, 2013
 *      Author: root
 */

#include "../../../include/BP-Main.h"

// constructor
BP_WebFormSubmitter::BP_WebFormSubmitter()
{

	// set the request object to NULL
	this->request = NULL;

	// set the default override fields to 0/null
	this->override_fields = NULL;
	this->override_fields_n = 0;

	// stack of fields which pertain to custom behaivor
	this->field_custom_behaivors = NULL;
	field_custom_behavior_n = 0;

}

// destructor
BP_WebFormSubmitter::~BP_WebFormSubmitter()
{
	// this class does not allocate memory, and only acts through
	// a provided request object
}


// add field override element
BP_ERROR_T BP_WebFormSubmitter::addInputOverrideField(char * input_name, char *new_input_value)
{

	// ensure we have an input name and input value
	if(!input_name && !new_input_value)
	{
		this->AddErr("Cannot add input override field without input name and input value.  Pass empty strings for empty values.");
		return ERR_FAILURE;
	}

	// create a new element in the field
	this->override_fields = (P_FORM_SUBMIT_OVERRIDE_INPUT_FIELDS) bprealloc(this->override_fields, (this->override_fields_n+1) * sizeof(FORM_SUBMIT_OVERRIDE_INPUT_FIELDS));

	// zero out the memory
	memset((void *)&this->override_fields[this->override_fields_n], 0x00, sizeof(FORM_SUBMIT_OVERRIDE_INPUT_FIELDS));

	// set the input name and new input value
	this->override_fields[this->override_fields_n].input_name         = bpstrdup(input_name);
	this->override_fields[this->override_fields_n].input_override_val = bpstrdup(new_input_value);

	// increment the override fields counter
	this->override_fields_n++;

	// return indicating success
	return ERR_SUCCESS;

}




// add field override element
BP_ERROR_T BP_WebFormSubmitter::addInputFieldBehaviorModification(char * input_name, bool do_not_send)
{

	// ensure we have an input name and input value
	if(!input_name)
	{
		this->AddErr("Cannot add a behavior modifier without an input name.");
		return ERR_FAILURE;
	}

	// create a new element in the field
	this->field_custom_behaivors = (P_FORM_FIELD_SUBMIT_CUSTOM_BEHAIVOR) bprealloc(this->field_custom_behaivors, (this->field_custom_behavior_n+1) * sizeof(FORM_FIELD_SUBMIT_CUSTOM_BEHAIVOR));

	// zero out the memory
	memset((void *)&this->field_custom_behaivors[this->field_custom_behavior_n], 0x00, sizeof(FORM_FIELD_SUBMIT_CUSTOM_BEHAIVOR));

	// set the input name and new input value
	this->field_custom_behaivors[this->field_custom_behavior_n].field_name         = bpstrdup(input_name);
	this->field_custom_behaivors[this->field_custom_behavior_n].do_not_send        = do_not_send;

	// increment the override fields counter
	this->field_custom_behavior_n++;

	// return indicating success
	return ERR_SUCCESS;

}


// retrieve a custom behavior by name
P_FORM_FIELD_SUBMIT_CUSTOM_BEHAIVOR BP_WebFormSubmitter::getFieldBehaviorModificationByName(char * input_name)
{

	if(!input_name)
	{
		this->AddErr("Input name is needed for search of behavior mod structures.");
		return NULL;
	}

	size_t n = 0;
	for(; n < this->field_custom_behavior_n; n++)
	{
		if(BP_strncasecmp(input_name, this->field_custom_behaivors[n].field_name, bpstrlen(input_name)) == 0)
		{
			return (P_FORM_FIELD_SUBMIT_CUSTOM_BEHAIVOR) &this->field_custom_behaivors[n];
		}
	}

	// return null if we cant find an item
	return NULL;
}

// try to retrieve a form override by name
P_FORM_SUBMIT_OVERRIDE_INPUT_FIELDS BP_WebFormSubmitter::getInputOverrideByName(char * input_name)
{

	if(!input_name)
	{
		this->AddErr("Looking up a null input name.  Input name should be a valid string.");
		return NULL;
	}

	// check input name
	if(strnlen(input_name, 1024) == 0)
	{
		this->AddErr("Input name has to actually be a non-zero length string.");
		return NULL;
	}

	// walk the fields and look for the value
	size_t n = 0;
	for(; n < this->override_fields_n; n++)
	{
		if(strncmp(this->override_fields[n].input_name, input_name, strnlen(input_name, 2048) ) == 0)
		{
			return &this->override_fields[n];
		}
	}


	// if we cant find the form override, return null
	return NULL;

}

// updates an input override field by name (frees content (not name) and replaces it with specified
// data (which is allocated)
BP_ERROR_T BP_WebFormSubmitter::updateInputOverrideByNmae(char * input_name, char *new_content)
{

	// check for null pointers
	if(!input_name || !new_content)
	{
		this->AddErr("Cannot update input override value with empty input/content.  For empty, pass a empty string not a null pointer.");
		return ERR_FAILURE;
	}

	// attempt to retrieve the input override
	P_FORM_SUBMIT_OVERRIDE_INPUT_FIELDS override = this->getInputOverrideByName(input_name);

	// if the input doesn't exist, return failure
	if(!override)
		return ERR_FAILURE;

	// attempt to override value
	if(override->input_override_val)
	{
		bpfree(override->input_override_val);
		override->input_override_val = bpstrdup(new_content);
	}

	// return indicating success
	return ERR_SUCCESS;

}

// destroy the overrride fields
BP_ERROR_T BP_WebFormSubmitter::destroyInputOverrides()
{

	// if there is nothing to destroy, just return
	if(!this->override_fields)
		return ERR_SUCCESS;

	// check for corrupted data
	if(!this->override_fields_n)
	{
		this->AddErr("Error, data corrupted.  Element count is 0 where the element pointer contains a value.");
		return ERR_FAILURE;
	}

	size_t n = 0;
	for(; n < this->override_fields_n; n++)
	{

		// destroy name if it exists
		if(this->override_fields[n].input_name)
			bpfree(this->override_fields[n].input_name);

		// destroy override value if it exists
		if(this->override_fields[n].input_override_val)
			bpfree(this->override_fields[n].input_override_val);

	}

	// destroy the fields
	bpfree(this->override_fields);

	// set fields and count to null/zero
	this->override_fields = NULL;
	this->override_fields_n = 0;

	// return indicating success (data has been released)
	return ERR_SUCCESS;

}



// submit the form (result data will be in the request data)
BP_ERROR_T BP_WebFormSubmitter::submitForm(P_BP_HTML_FORM_LIST form_element, P_FORM_SUBMIT_OVERRIDE_INPUT_FIELDS override_fields)
{

	// ensure we have a form element
	if(!form_element)
		return ERR_FAILURE;

	// will store the data generated via the form elements
	char *data = NULL;

	// initial form pointer saved
	P_BP_HTML_FORM_LIST form = form_element;

	// save form properties
	P_BP_HTML_FORM_PROPERTY_LIST form_props = form_element->properties;

	// save form inputs
	P_BP_HTML_INPUT_LIST form_inputs = form_element->inputs->first;

	// get form method
	form_props = form_element->properties->first;

	// 0 for get, 1 for post, 2 for etc
	size_t form_method = 0;

	// form action (url) to post to
	char * form_action = NULL;

	// iterate through properties and find method
	for
	(
			form_element->properties = form_element->properties->first;
			form_element->properties;
			form_element->properties = form_element->properties->next
	)
	{

		// check to see if the property name is "method"
		if(BP_strncasecmp((char *) form_element->properties->name, "method", bpstrlen("method")) == 0)
		{

			// if the value is post, set the form method to post (aka 1)
			if(BP_strncasecmp(form_element->properties->content, "post", bpstrlen("post")) == 0)
			{
				form_method = 1;
			}
		}

		// check to see if the property name is "method"
		if(BP_strncasecmp((char *) form_element->properties->name, "action", bpstrlen("action")) == 0)
		{

			// allocate form action
			form_action = bpstrdup(form_element->properties->content);

		}

	}

	// reset the properties
	form_element->properties = form_props;


	P_BP_HTML_INPUT_PROPERTY_LIST tmp_prop_list = NULL;

	// create new string builder
	BP_StringUtils * str_builder = new BP_StringUtils(data);

	// used in the loop below in order to set parameter names and values
	char * param_name  = NULL;
	char * param_val   = NULL;


	// walk form inputs and create string
	for(form_inputs = form_inputs->first; form_inputs; form_inputs = form_inputs->next)
	{

		// set values to null on loop
		param_name = NULL;
		param_val  = NULL;

		for(tmp_prop_list = form_inputs->properties->first; tmp_prop_list; tmp_prop_list = tmp_prop_list->next)
		{

			// get name
			if(BP_strncasecmp(tmp_prop_list->name, "name", 4) == 0)
			{
				printf("\n Got value?: %s", tmp_prop_list->content);
				param_name = tmp_prop_list->content;
			}


		}

		// ========= Now Extract the Value Property ====================

		for(tmp_prop_list = form_inputs->properties->first; tmp_prop_list; tmp_prop_list = tmp_prop_list->next)
		{

			if(BP_strncasecmp(tmp_prop_list->name, "value", 5) == 0)
			{

				printf("\n Got value of form elem?: %s", tmp_prop_list->content);
				param_val = tmp_prop_list->content;

			}

		}

		// ======== Now Build Submit parameters ===============================
		if(this->getFieldBehaviorModificationByName(param_name))
		{
			// do nothing if behavior is being modified
		}
		else if(this->getInputOverrideByName(param_name))
		{
			// add the string in
			str_builder->BuildString(param_name);
			str_builder->BuildString("=");
			str_builder->BuildString(this->getInputOverrideByName(param_name)->input_override_val);

			// if we have a next element, add an ampersand
			if(form_inputs->next)
				str_builder->BuildString("&");
		}
		else
		{
			// add the string in
			str_builder->BuildString(param_name);
			str_builder->BuildString("=");
			str_builder->BuildString(param_val);

			// if we have a next element, add an ampersand
			if(form_inputs->next)
				str_builder->BuildString("&");

		}



	}

	// display built string
	printf("\n Built String?: %s", str_builder->build_str);


	// create new request url builder
	BP_StringUtils * request_url_builder = new BP_StringUtils(NULL);

	// get last requested href parsed down
	char * base_path = BP_ParseURLToPath(this->request->httpSession->lastRequestedHref, 1);
	request_url_builder->BuildString(base_path);
	if(base_path)
		bpfree(base_path);

	// append action
	request_url_builder->BuildString(form_action);

	// set the post data for the request
	// this->request->HttpSetPostData(str_builder->build_str);

	// set method to post
	// curl_easy_setopt(this->request->httpSession->curl, CURLOPT_POST, 1);

	// set the build string as post data
	// curl_easy_setopt(this->request->httpSession->curl, CURLOPT_POSTFIELDS, str_builder->build_str);

	// HTTPGetCookieFromLastHeadersRecievedSetCookie(this->request->httpSession);

	this->request->HttpSetPostData(str_builder->build_str);

	// make the request
	this->request->HttpMakeRequest(request_url_builder->build_str);

	printf("\n Final Action: %s", request_url_builder->build_str);

	printf("\n Making login request!");

	/*
	*/
	// destroy the string builders
	delete str_builder;
	delete request_url_builder;

}


#include "../../../include/BP-Main.h"

// ---------------------------------------------------------------------------
// BEGIN DECONSTRUCTORS:

// destroy a delimiter set
int HTMLDestroyAnchorDelimiters(P_BP_HTML_ANCHOR_DELIMITERS delims, P_BP_GARBAGE_COLLECTOR_DATA gc){

	// Destroy Delimiters
	if(delims != NULL){

		// check and bpfree next delimiter
		if(delims->nextDelimiter != NULL){
			BP_free(gc, NULL, delims->nextDelimiter);
			delims->nextDelimiter = NULL;
		}

		// check and bpfree start delimiter
		if(delims->startDelimiter != NULL){
			BP_free(gc, NULL, delims->startDelimiter);
			delims->startDelimiter = NULL;
		}

		// check and bpfree value delimiter
		if(delims->valueDelimiter != NULL){
			BP_free(gc, NULL, delims->valueDelimiter);
			delims->valueDelimiter = NULL;
		}

		// bpfree entire structure
		BP_free(gc, NULL, delims);
	}

	return ERR_SUCCESS;
}

// destroy an input lists, properties sub-list
int HTMLDestroyFormInputPropertiesList(P_BP_HTML_INPUT_PROPERTY_LIST propList, P_BP_HTML_FULL_EXTRACTION extraction){

	// default NULL check
	if(propList)
		return ERR_FAILURE;


	// traverse list and deconstruct elements
	for(propList = propList->first; propList != NULL; )
	{


		// destroy content buffer
		if(propList->content != NULL)
		{
			tq_free(extraction->tq, propList->content);
			propList->content = NULL;
		}

		// destroy name buffer
		if(propList->name != NULL)
		{
			tq_free(extraction->tq, propList->name);
			propList->name = NULL;
		}

		// Free proplist structure, logic
		if(propList->next != NULL)
		{

			// move to the next
			propList = propList->next;

			// bpfree the previous
			if(propList->prev != NULL)
			{
				tq_free(extraction->tq, propList->prev);
			}

			// terminate behind us
			propList->prev = NULL;

		} else {

			if(propList != NULL)
			{
				tq_free(extraction->tq, propList);
			}

			propList = NULL;
			break;

		}


	}

	return ERR_SUCCESS;
}

// destroy an input list and sub properties list
int HTMLDestroyFormInputList(P_BP_HTML_INPUT_LIST inputList, P_BP_HTML_FULL_EXTRACTION extraction)
{

	// default null checks
	if(!inputList)
		return ERR_FAILURE;

	// destroy a form input list and its sub-property lists
	if(inputList)
	for
	(
		inputList = inputList->first;
		inputList;
		inputList = inputList->next
	)
	{

		// if we have properties, destroy them
		if(inputList->properties)
		{
			HTMLDestroyFormInputPropertiesList(inputList->properties->first, extraction);
		}

		// destroy the input list element if we have no next
		if(!inputList->next)
		{
			tq_free(extraction->tq, inputList);
			inputList = NULL;
			break;

		}
		else
		{

			// since we have a next, move to it
			inputList = inputList->next;

			// if we have a previous, destroy it
			if(inputList->prev)
				tq_free(extraction->tq, inputList->prev);

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// destroys the properties list internal to a form element (not input based at all, 100% form)
int HTMLDestroyFormPropertiesList(P_BP_HTML_FORM_PROPERTY_LIST inList, P_BP_HTML_FULL_EXTRACTION extraction){

	// ensure we have an inlist
	if(!inList)
		return ERR_FAILURE;
	if(!inList->first)
		return ERR_FAILURE;

	// ensure we have an extraction
	if(!extraction)
		return ERR_FAILURE;


	// deconstruct list
	for
	(
		inList = inList->first;
		inList;
	)
	{

		// destroy allocated name buffer
		if(inList->name)
		{
			tq_free(extraction->tq, inList->name);
			inList->name = NULL;
		}

		// destroy allocated content buffer
		if(inList->content)
		{
			tq_free(extraction->tq, inList->content);
			inList->content = NULL;
		}

		// List Element Deconstruction Logic
		if(!inList->next)
		{

			// since we're at the last element, free and exit loop
			tq_free(extraction->tq, inList);
			inList = NULL;
			break;

		}
		else
		{
			// move to next pointer
			inList = inList->next;

			if(inList->prev != NULL)
				tq_free(extraction->tq, inList->prev);

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}


// destroy a form list and its child input list
int HTMLDestroyFormList(P_BP_HTML_FORM_LIST formList, P_BP_HTML_FULL_EXTRACTION extraction)
{

	// default null check
	if(formList == NULL)
		return ERR_FAILURE;

	for
	(
			formList = formList->first;
			formList;
	)
	{

		// destroy inputs
		if(formList->inputs)
		{
			HTMLDestroyFormInputList(formList->inputs->first, extraction);
		}

		// destroy properties
		if(formList->properties != NULL)
		{
			HTMLDestroyFormPropertiesList(formList->properties->first, extraction);
		}

		// list termination logic
		if(!formList->next)
		{

			tq_free(extraction->tq, formList);
			formList = NULL;
			break;

		}

		// move to the next position
		formList = formList->next;

		if(formList->prev)
		{
			tq_free(extraction->tq, formList->prev);
		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// destroy an anchor lists sub variable list
int HTMLDestroyAnchorVarSublist(P_BP_HTML_ANCHOR_VAR_LIST anchorVars, P_BP_GARBAGE_COLLECTOR_DATA gc){

	// default NULL checks
	if(anchorVars == NULL) return ERR_FAILURE;



	// deconstruct the list
	if(anchorVars->first != NULL)
	for(anchorVars = anchorVars->first; anchorVars != NULL; ){

		// destroy content
		if(anchorVars->content != NULL){
			BP_free(gc, NULL, anchorVars->content);
			anchorVars->content = NULL;
		}

		// destroy name string
		if(anchorVars->name != NULL){
			BP_free(gc, NULL, anchorVars->name);
			anchorVars->name = NULL;
		}

		// list element deconstruction logic
		if(anchorVars->next == NULL){
			BP_free(gc, NULL, anchorVars);
			anchorVars = NULL;
			break;
		} else {
			anchorVars = anchorVars->next;
			if(anchorVars->prev != NULL){
				BP_free(gc, NULL, anchorVars->prev);
				anchorVars->prev = NULL;
			}
		}

	}

	// return indicating success
	return ERR_SUCCESS;
}

// destroy an existing anchor list
int HTMLDestroyAnchorList(P_BP_HTML_ANCHOR_LIST anchorList, P_BP_GARBAGE_COLLECTOR_DATA gc){

	// default NULL checks
	if(anchorList == NULL)
		return ERR_FAILURE;

	if(anchorList->first == NULL)
		return ERR_FAILURE;

	// Destroy Anchor List
	for
	(
		anchorList = anchorList->first;
		anchorList;
	)
	{

		// destroy href
		if(anchorList->href)
		{
			BP_free(gc, NULL, anchorList->href);
			anchorList->href = NULL;
		}

		// bpfree and nullify protocol string
		if(anchorList->protoStr != NULL)
		{

			// bpfree is unnecessary because string is stored in bss
			// bpfree(anchorList->protoStr);
			anchorList->protoStr = NULL;
		}

		// bpfree and nullify the relative path
		if(anchorList->relativePath != NULL){
			BP_free(gc, NULL, anchorList->relativePath);
			anchorList->relativePath = NULL;
		}

		// bpfree and nullify variable list
		if(anchorList->vars != NULL){
			HTMLDestroyAnchorVarSublist(anchorList->vars->first, gc);
		}

		// list destruction logic
		if(anchorList->next == NULL){

			BP_free(gc, NULL, anchorList);
			anchorList = NULL;
			break;
		} else {

			anchorList = anchorList->next;

			// destroy previous if we set next
			if(anchorList->prev != NULL)
				BP_free(gc, NULL, anchorList->prev);
			anchorList->prev = NULL;
		}

	}


	return ERR_SUCCESS;
}

// destroy all allocated parts of an extraction
int HTMLDestroyExtraction(P_BP_HTML_FULL_EXTRACTION extracted)
{

	// default NULL checks
	if(!extracted)
		return ERR_FAILURE;

	// destroy anchors and sub-var list
	if(extracted->anchors)
	{
		HTMLDestroyAnchorList(extracted->anchors, extracted->gc);
		extracted->anchors = NULL;
	}

	if(extracted->delims)
	{
		HTMLDestroyAnchorDelimiters(extracted->delims, extracted->gc);
		extracted->delims = NULL;
	}

	// destroy forms
	if(extracted->forms)
	{
		HTMLDestroyFormList(extracted->forms, extracted);
		extracted->forms = NULL;
	}

	if(extracted->gc)
	{

		// destroy the garbage collector
		BP_DestroyGarbageCollector(extracted->gc);

		// set null
		extracted->gc = NULL;

	}

	// destroy the linkl here
	BP_DestroyLinkL(extracted->tq, BP_TRUE);

	// destroy final structure
	bpfree(extracted);

	// return indicating success
	return ERR_SUCCESS;

}

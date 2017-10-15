/*
 * BP-GeneralPurposeTemplateSubstitution.cc
 *
 *  Created on: Dec 18, 2011
 *      Author: root
 */

// include main functions
#include "../../include/BP-Main.h"

// This function finds and replaces all items in a buffer, creating a working
// filled out template.  The filled out template is allocated and returned in
// a pointer (which must later after use, be bpfreed).  This function uses
// Posix Regular Expressions.
char * BP_GenerateSubstitutedTemplateREGEXP(char *template_data, char **find_strs, char **replace_strs)
{

	// check template_data parameters first
	if(!template_data || !find_strs || !replace_strs)
		return NULL;

	// make sure we have first elements
	if(!find_strs[0] || !replace_strs[0])
		return NULL;

	// counts the elements in the find and replace columns
	size_t find_item_count = 0;
	size_t replace_count   = 0;

	// loop counter
	size_t n = 0;
	for(n = 0; find_strs[n];    n++){ find_item_count++; }
	for(n = 0; replace_strs[n]; n++){ replace_count++; }

	// check find item counts
	if(find_item_count != replace_count)
	{
		printf("\n Item count  mismatch?!");
		return NULL;
	}

	// set loop iter
	size_t j = 0;

	// temporary loop iteration string
	char *tmp_str = NULL;

	// ensure we have a string to replace in
	if(!template_data)
	{
		return NULL;
	}

	// duplicate string before loop
	char * mod_string = bpstrdup(template_data);
	char * replaced_str = NULL;
	char *replace_this = NULL;
	char *replace_with = NULL;

	// set tmp string as mod string
	replaced_str = mod_string;

	// run replacement loop
	for(; j < replace_count; j++)
	{

		// setup find and replace items
		replace_this = find_strs[j];
		replace_with = replace_strs[j];

		// ensure we have pointers to find/replace strings
		if(!replace_this || !replace_with)
			break;

		// replace length used for updating
		size_t replace_len = 0;
		if(replace_with)
			replace_len = bpstrlen(replace_with);

		// attempt to run find and replace
		tmp_str  = BP_RegFindReplace
		(
			replaced_str,
			replace_this,
			replace_with,
			replace_len,    // replace size
			1, 				// max replacements
			0
		);

		// check to see that we have a temporary string
		if(tmp_str)
		{

			// free only if the replace string and mod string aren't the same
			if(replaced_str != mod_string)
				bpfree(replaced_str);

			// set the replaced string here
			replaced_str = tmp_str;

		}

		// if the data couldn't be acted on, restore the old ptr on loop
		if(!replaced_str )
		{
			replaced_str = mod_string;
		}

	}

	// free string if necessary (should always be necessary)
	if(mod_string)
		bpfree(mod_string);

	// return the replaced value
	return replaced_str;

}


// This function finds and replaces all items in a buffer, creating a working
// filled out template.  The filled out template is allocated and returned in
// a pointer (which must later after use, be bpfreed).  This function uses
// PCRE / Perl Compatible Regular Expressions.
char * BP_GenerateSubstitutedTemplatePCRE(char *template_data, char **find_strs, char **replace_strs)
{

	// check template_data parameters first
	if(!template_data || !find_strs || !replace_strs)
		return NULL;

	// make sure we have first elements
	if(!find_strs[0] || !replace_strs[0])
		return NULL;

	// counts the elements in the find and replace columns
	size_t find_item_count = 0;
	size_t replace_count   = 0;

	// loop counter
	size_t n = 0;
	for(n = 0; find_strs[n];    n++){ find_item_count++; }
	for(n = 0; replace_strs[n]; n++){ replace_count++; }

	// check find item counts
	if(find_item_count != replace_count)
	{
		printf("\n Item count  mismatch?!");
		return NULL;
	}

	// set loop iter
	size_t j = 0;

	// temporary loop iteration string
	char *tmp_str = NULL;

	// ensure we have a string to replace in
	if(!template_data)
	{
		return NULL;
	}

	// duplicate string before loop
	char * mod_string = bpstrdup(template_data);
	char * replaced_str = NULL;
	char *replace_this = NULL;
	char *replace_with = NULL;

	// set tmp string as mod string
	replaced_str = mod_string;

	// run replacement loop
	for(; j < replace_count; j++)
	{

		// setup find and replace items
		replace_this = find_strs[j];
		replace_with = replace_strs[j];

		// ensure we have pointers to find/replace strings
		if(!replace_this || !replace_with)
			break;

		// replace length used for updating
		size_t replace_len = 0;
		if(replace_with)
			replace_len = bpstrlen(replace_with);

		// attempt to run find and replace
		tmp_str  = BP_RegFindReplacePCRE
		(
			replaced_str,
			bpstrlen(replaced_str),
			replace_this,
			replace_with,
			replace_len,    // replace size
			1, 				// max replacements
			0
		);

		// check to see that we have a temporary string
		if(tmp_str)
		{

			// free only if the replace string and mod string aren't the same
			if(replaced_str != mod_string)
				bpfree(replaced_str);

			// set the replaced string here
			replaced_str = tmp_str;

		}

		// if the data couldn't be acted on, restore the old ptr on loop
		if(!replaced_str )
		{
			replaced_str = mod_string;
		}

	}

	// free string if necessary (should always be necessary)
	if(mod_string)
		bpfree(mod_string);

	// return the replaced value
	return replaced_str;

}




// This function finds and replaces all items in a buffer, creating a working
// filled out template.  The filled out template is allocated and returned in
// a pointer (which must later after use, be bpfreed).  This function is literal
// and does not replace regular expressions.
char * BP_GenerateSubstitutedTemplate(char *template_data, char **find_strs, char **replace_strs)
{

	// check template_data parameters first
	if(!template_data || !find_strs || !replace_strs)
		return NULL;

	// make sure we have first elements
	if(!find_strs[0] || !replace_strs[0])
		return NULL;

	// counts the elements in the find and replace columns
	size_t find_item_count = 0;
	size_t replace_count   = 0;

	// loop counter
	size_t n = 0;
	for(n = 0; find_strs[n];    n++){ find_item_count++; }
	for(n = 0; replace_strs[n]; n++){ replace_count++; }

	// check find item counts
	if(find_item_count != replace_count)
	{
		printf("\n Item count  mismatch?!");
		return NULL;
	}

	size_t j = 0;


	// temporary loop iteration string
	char *tmp_str = NULL;

	// ensure we have a string to replace in
	if(!template_data)
	{
		return NULL;
	}

	// duplicate string before loop
	char * mod_string     = NULL;
	char * replace_string = NULL;
	char * replace_string_last = NULL;
	char *replace_this    = NULL;
	char *replace_with    = NULL;

	// duplicate string here
	mod_string = bpstrdup(template_data);
	replace_string = mod_string;

	// run replacement loop
	for(; j < replace_count; j++)
	{

		// setup find and replace items
		replace_this = find_strs[j];
		replace_with = replace_strs[j];

		// ensure we have replace pointers
		if(!replace_this || !replace_with)
			break;

		// replace length used for updating
		size_t replace_len = 0;
		if(replace_with)
			replace_len = bpstrlen(replace_with);

		if(!replace_string)
			replace_string = mod_string;

		// printf("\n Got: %p", replace_string);

		// replace the string (no regexp)
		replace_string_last  = BP_FindReplace
		(
			replace_string,
			replace_this,
			replace_with,
			replace_len,  	// replace size
			1
		);

		// set replace string as mod string
		if(!replace_string_last)
		{

			if(!replace_string)
				replace_string = mod_string;
		}
		else
		{

			// check replace/mod string before freeing
			if(mod_string != replace_string)
				bpfree(replace_string);

			// set pointers here
			replace_string = replace_string_last;

		}

	}

	// check mod string
	if(mod_string)
	if(mod_string != replace_string)
	{
		bpfree(mod_string);
	}

	// return indicating success
	return replace_string;

}

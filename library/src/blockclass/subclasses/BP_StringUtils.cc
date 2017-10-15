
// include main includes
#include "../../../include/BP-Main.h"

//! String utility class constructor.  Initializes member variables to zero and
//  creates allocators/base datasets.
BP_StringUtils::BP_StringUtils(char * string)
{

	// array of strings to use as input to find and replace
	this->find_strs       = NULL;
	this->replace_strs    = NULL;
	this->find_item_count = 0;
	this->mod_string      = NULL;

	// perl compatible regular expression member initializations.
	this->mod_string_pcre      = NULL;
	this->find_strs_pcre       = NULL;
	this->replace_strs_pcre    = NULL;
	this->find_item_count_pcre = NULL;

	// set initial parsing hint to error (aka zero).
	this->csv_parsing_hint = BP_STRINGUTILS_BUILD_ERROR;

	// set defaults to 0
	this->csv_num_rows = 0;
	this->csv = NULL;

	this->exploded = NULL;
	this->exploded_count = 0;

	// set flags to true
	this->dont_explode_between_doublequotes = true;
	this->dont_explode_between_singlequotes = true;;

	// set builder string to null
	this->build_str = NULL;

	// contains unique strings
	this->unique_strings = NULL;
	this->unique_str_cnt = NULL;

	// set index as null
	this->unique_string_index_counts = NULL;

	// generate a random arbitrary id to assign the tail queue allocator
	size_t random_id = BP_GenRandomUnsignedLong(1, 0xffffffff);

	// initialze local linked list
	this->tq = BP_CreateLinkL("BP_StringUtils", BP_TRUE);

	// set duplication of strings to true
	this->duplicate_unique_strings = true;

	// set null file name
	this->xml_file_name = NULL;

	// set xml information here
	this->xml_doc       = NULL;
	this->xml_root_node = NULL;
	this->xml_curr_node = NULL;

	// current properties
	this->xml_curr_prop         = NULL;
	this->xml_curr_prop_name    = NULL;
	this->xml_curr_prop_content = NULL;

	// xml parsed flag
	this->xml_parsed_ok = false;


	// Duplicate string if it's passed in.  Allocate it within the class
	// tail queue memory allocator.
	if(string)
		this->string = tq_strdup(this->tq, string, BPLN);
	else
		this->string = NULL;

}

//! class deconstructor
BP_StringUtils::~BP_StringUtils()
{

	// check for / delete string
	if(this->mod_string)
	{
		tq_free(this->tq, this->mod_string);
		this->mod_string = NULL;
	}

	// check for /delete csv
	if(this->csv)
	{

		// walk rows
		size_t n = 0;
		for(; n < this->csv_num_rows; n++)
		{

			// ensure we have a record at the read position
			if(!this->csv[n])
				break;

			// now walk and destroy csv
			size_t j = 0;
			for(; this->csv[n][j]; j++)
			{
				tq_free(this->tq, this->csv[n][j]);
				this->csv[n][j] = NULL;
			}

			// free the row itself
			tq_free(this->tq, this->csv[n]);
			this->csv[n] = NULL;

		}

		// destroy the csv
		tq_free(this->tq, this->csv);
		this->csv = NULL;

	}

	// destroy exploded data if necessary
	if(this->exploded)
	{

		// destroy exploded data here
		size_t n = 0;
		for(; n < this->exploded_count; n++)
		{

			if(this->exploded[n])
				tq_free(this->tq, this->exploded[n]);

		}

		// now destroy entire structure
		tq_free(this->tq, this->exploded);

	}

	// set builder string to null
	if(this->build_str)
		tq_free(this->tq, this->build_str);


	// destroy unique strings
	if(this->duplicate_unique_strings == true)
	{
		// destroy the pointers
		if(this->unique_strings)
			BP_DestroyPointerArrayTAILQ(this->tq, (void **) this->unique_strings, this->unique_str_cnt);

	}

	// destroy the unique strings where necessary
	if(this->unique_strings)
		tq_free(this->tq, this->unique_strings);
	if(this->unique_string_index_counts)
		tq_free(this->tq, this->unique_string_index_counts);



	// destroys reg find replace for normal posix regex
	this->DestroyRegFindReplaceData();

	// destroy the base string here
	if(this->string)
		tq_free(this->tq, this->string);


	// Display error message in the condition that we have arbitrary chunks left over.  The
	// chunks are still destroyed, but they should be addressed by the developer.  This is
	// done to assist with RAD.
	if(this->tq->total_allocations != 0)
	{
		printf("\n [+] Error: Destroying BP_StringUtils obj which still currently has allocations.");
		printf("\n [+] Information Below: ");
		BP_DisplayLinkL(this->tq);
	}


	// destroy all allocated memory
	BP_DestroyLinkL(this->tq, BP_TRUE);
	this->tq = NULL;

}


// Adds a find/replace set to the stack.  Find is regular expression.
bool BP_StringUtils::AddRegFind(char *replace_this, char *replace_with)
{

	// replace this and replace with are required parameters
	if(!replace_this)
		return false;
	if(!replace_with)
		return false;

	// array of strings to use as input to find and replace
	this->find_item_count++;

	// bpreallocate space for find/replace strings
	this->find_strs    = (char **) tq_realloc(this->tq, this->find_strs,    sizeof(char *) * (this->find_item_count+1), BPLN);
	this->replace_strs = (char **) tq_realloc(this->tq, this->replace_strs, sizeof(char *) * (this->find_item_count+1), BPLN);

	// set the find/replace item in the array
	this->find_strs[this->find_item_count-1]    = tq_strdup(this->tq, replace_this, BPLN);

	// terminate the array
	this->find_strs[this->find_item_count]    = NULL;

	if(replace_with)
	{
		this->replace_strs[this->find_item_count-1] = tq_strdup(this->tq, replace_with, BPLN);
		this->replace_strs[this->find_item_count]   = NULL;
	}
	else
		this->replace_strs[this->find_item_count-1] = NULL;

	// return indicating success
	return true;

}

//! Run find and replace.
bool BP_StringUtils::RegFindReplace()
{

	// set iterator
	size_t j = 0;

	// set item count
	size_t replace_count = this->find_item_count;

	// set tmp string
	char *tmp_str = NULL;

	// ensure we have a string
	if(!this->string)
	{
		this->AddErr("No string, cannot run replace routine.");
		return false;
	}

	if(this->mod_string)
	{
		tq_free(this->tq, this->mod_string);
		this->mod_string = NULL;
	}

	// generate the modified string
	this->mod_string = BP_GenerateSubstitutedTemplateREGEXP(this->string, this->find_strs, this->replace_strs);

	// if the mod string was created ok, add it to our tail queue
	if(this->mod_string)
	{

		BP_AddDataToLinkLTail
		(
			this->tq,
			(void *) this->mod_string,
			bpstrlen(this->mod_string),
			BP_MEMPROF_ALLOC_TYPE_USER_DEFINED_ASCII,
			BPLN
		);

	}

	// return indicating success
	return true;

}

// destroy the registry find and replace
bool BP_StringUtils::DestroyRegFindReplaceData()
{

	if(this->mod_string)
	{
		tq_free(this->tq, mod_string);
	}

	// walk each entry and destroy
	size_t n = 0;
	for(; n < this->find_item_count; n++)
	{
		if(this->find_strs[n])
			tq_free(this->tq, this->find_strs[n]);
		if(this->replace_strs[n])
			tq_free(this->tq, this->replace_strs[n]);
	}

	// check for find strings
	if(this->find_strs)
	{
		tq_free(this->tq, this->find_strs);
	}

	// check for find strings
	if(this->replace_strs)
	{
		tq_free(this->tq, this->replace_strs);
	}

	// return indicating success
	return true;

}


// randomizes the pointers found in a char ** (useful for randomizing arguments)
char ** BP_StringUtils::RandomizePointerArray(char **ptrs, size_t num_ptrs)
{

	// return null on failure
	if(!num_ptrs)
		return NULL;

	// create space for new pointers
	char ** new_ptrs = (char **) tq_calloc(this->tq, sizeof(char *) *  (num_ptrs+1), 1, BPLN);
	size_t * matches = (size_t *) tq_calloc(this->tq, sizeof(size_t) * (num_ptrs+1), 1, BPLN);

	size_t curr_guess = 0;

	// attempt to seed the global pnrg singleton if not already set
	BP_SeedPNRG();
	// srandom(time(NULL));



	size_t i = 0;
	size_t j = 0;
	size_t num_already_matched = 0;
	size_t num_matched_in_loop = 0;
	for(;;)
	{

		// create a different random number
		curr_guess = rand() % (num_ptrs + 1);
		if(curr_guess == 0)
			continue;

		size_t x = 0;
		bool guess_failed = false;
		num_matched_in_loop = 0;

		// walk matches list and see if the match has already been made
		for(; x < num_ptrs; x++ )
		{
			if(curr_guess == matches[x])
			{
				num_matched_in_loop++;
			}

		}

		// if the number matched in the loop is > 0 continue (needs to be 0)
		if(num_matched_in_loop && (num_matched_in_loop != num_ptrs))
		{
			continue;
		}


		// ===================================================

		// increase number matched
		num_already_matched++;

		// save guess in matches array so its not reused
		matches[j] = curr_guess;

		// set prt in new position
		new_ptrs[curr_guess-1] = ptrs[j];

		// move index up
		j++;

		// break loop if we fetched the correct number of ptr elements
		if(j == num_ptrs)
			break;

	}

	// return new array
	return new_ptrs;

}


// Creates a character * pointer array containing a set of exploded() values.
bool BP_StringUtils::Explode(char *reg_delim)
{

	// ensure we have a delimiter to explode buffer with
	if(!reg_delim)
		return false;

	// ensure we have a string set in the class
	if(!this->string)
		return false;

	// first get number of replacements
	char ** replacements    = NULL;
	size_t num_replacements = 0;


	// create match info loop
	BP_REGEX_MATCH_INFO   reg_match = {0};
	reg_match.matches      = NULL;
	reg_match.num_matched = 0;

	// for detecting bugs in string
	size_t is_match_you_want = 0;
	if(memcmp(reg_delim, " | ", 3) == 0)
	{
		is_match_you_want++;
	}

	// walk regular expressions and create duplication strings
	size_t num_toks = 0;
	char *tok_str = this->string;
	char *element = NULL;
	for(;
		BP_MatchRegularExpression(tok_str, bpstrlen(tok_str), reg_delim, 0, 1, &reg_match) == ERR_SUCCESS;
		num_toks++
		)
	{



		if(reg_match.num_matched != 0)
		{

			// %%% Done quote detection %%%%%%%%%%%%%%%%%%%%%%%

			// duplicate element
			element = tq_strndup(this->tq, tok_str, reg_match.matches->rm_so+1, BPLN);

			// terminate element if necessary
			element[bpstrlen(element)-1] = 0x00;

			// move token string forward
			tok_str = (char *)&tok_str[reg_match.matches->rm_eo];

			// bprealloc space for replacements
			num_replacements++;
			replacements = (char **) tq_realloc(this->tq,  replacements, sizeof(char*) * (num_replacements + 1) , BPLN);

			// set replacements in array
			replacements[num_replacements-1] = element;
			replacements[num_replacements]   = NULL;

		}
		else
		{

			BP_DestroyRegexpMatchInfo(&reg_match);

			// break the loop as we've hit the last element
			break;
		}

		BP_DestroyRegexpMatchInfo(&reg_match);

		reg_match.matches      = NULL;
		reg_match.num_matched = 0;

	}

	// add tail end of string as element
	if(num_replacements)
	{

		// increment replacements counter
		num_replacements++;

		// bprealloc space for replacements
		replacements = (char **) tq_realloc(this->tq, replacements, sizeof(char*) * (num_replacements+1) , BPLN);

		// create final element
		element = tq_strdup(this->tq, tok_str, BPLN);

		// set replacements in array
		replacements[num_replacements - 1] = element;
		replacements[num_replacements] = NULL;
	}

	// first get number of replacements
	this->exploded       = replacements;
	this->exploded_count = num_replacements;

	// return indicating success
	return true;

}

// Creates a duplicated string array given a csv buffer (\n between rows).  Resultant csv member lists have null termination.
bool BP_StringUtils::ParseCsv(bool remove_quotes)
{

	// ensure we have a string before trying to parse it
	if(!this->string)
	{
		printf("\n [+] No string set.");
		return false;
	}

	// string to use for exploding
	char *explode_string = NULL;

	// add newline if needed
	if(!strstr(this->string, "\n"))
	{
		explode_string = (char *) tq_calloc(this->tq, bpstrlen(this->string) + 5, 1, BPLN);
		sprintf(explode_string, "%s%s", this->string, "\n");
	}
	else
	{
		explode_string = this->string;
	}


	// create new string utility object to manipulate string
	BP_StringUtils * str_util = new BP_StringUtils(explode_string);

	// break the buffer up into newlines
	str_util->Explode("\n");

	char *new_string = NULL;

	// add newline if needed
	if(!strstr(this->string, "\n"))
	{
		new_string = (char *) tq_calloc(this->tq, bpstrlen(str_util->string) + 5, 1, BPLN);
		sprintf(new_string, "%s%s", str_util->string, "\n");
	}

	// walk each line and parse individual csv items
	size_t i = 0;

	// array returned outbound
	char *** out_array = NULL;
	size_t out_element_count = 0;

	// exploded array since csv is a special case and cannot be exploded
	char ** exploded_array      = NULL;
	size_t exploded_array_count = 0;

	// Walk the exploded data.  Each element is a line of csv.
	for(;
		i < str_util->exploded_count;
		i++
		)
	{

		// only process non-null lines
		if(! str_util->exploded[i] )
			continue;

		// counters must be even in order to signify matching ""
		size_t double_quote_counter = 0;
		size_t quote_counter        = 0;

		// these are set to true while calculating mismatches
		bool quote_mismatch        = false;
		bool double_quote_mismatch = false;

		// for detecting string lengths
		size_t x       = 0;
		size_t saved_x = 0;

		// set token string before working on object
		char *tok_string = str_util->exploded[i];

		// ensure the string has an actual length
		if(!strnlen(str_util->exploded[i], 10))
		{
			// printf("\n HERE? str_util->exploded[i]: %u , %s",i, str_util->exploded[i]);
			continue;
		}

		// printf("\n str_util->exploded[i]: %u , %s",i, str_util->exploded[i]);
		// printf("\n test string: %s", tok_string);

		// replacement count
		size_t replace_size_count = 0;

		// JMP loop label, used if string comma count mismatch (i is eventually the length of string till real comma)
		calculate_str_offset:

		// save x before loop
		for(;
			tok_string[x] != ',' && tok_string[x] != '\0';
			x++
			)
		{

			if(tok_string[x] == '"')
			{
				double_quote_counter++;
			}

			if(tok_string[x] == '\'')
			{
				quote_counter++;
			}

		}

		// increase the size of the replacement counter the size of x
		replace_size_count += x;

		// check the quote counters to determine if we have a mismatch
		if((double_quote_counter % 2) != 0)
		{
			double_quote_mismatch = true;
		}
		if((quote_counter %2) != 0)
		{
			quote_mismatch = true;
		}

		// if there is a mismatch, the comma matched is not the correct one, and you must loop
		if(double_quote_mismatch)
		{

			// reset indicator flags before looping
			double_quote_mismatch = false;
			quote_mismatch        = false;

			// increase x counter
			x++;

			// save x
			saved_x = x;

			// JMP loop
			goto calculate_str_offset;

		}

		// add element to the exploded array
		exploded_array_count++;
		exploded_array                             = (char **) tq_realloc(this->tq, exploded_array, sizeof(char *) * (exploded_array_count + 1) , BPLN);
		exploded_array[exploded_array_count -1 ]   = tq_strndup(this->tq, tok_string, x , BPLN);
		exploded_array[exploded_array_count]       = NULL;

		// reset quote counter
		quote_counter = 0;

		// reset double quote counter
		double_quote_counter = 0;

		// printf("\n Before: tok_string -- %s  -- %u", tok_string, x );
		// printf("\n Saved String: %s",                exploded_array[exploded_array_count -1 ]);

		// reassign token string
		tok_string = (char *) &tok_string[x+1];

		// printf("\n After: tok_string -- %s", tok_string);
		// printf("\n");

		// if there is more data in the string, go ahead and loop to populate next exploded array element
		if(bpstrlen(tok_string))
		{
			// increase position
			x=0;
			goto calculate_str_offset;
		}


		// %%%%%%%%%%% DEV NOTE %%%%%%%%%%%%%%%%%%%%%%%%%
		// at this point the exploded array is populated

		// line_utils->exploded now contains array of csv values
		out_element_count++;
		out_array = (char ***) tq_realloc(this->tq, out_array, ( out_element_count + 2) * sizeof(char *), BPLN);

		// loop the exploded array now
		size_t l = 0;
		for(;
			l < exploded_array_count;
			l++
			)
		{

			// exploded string
			if(!exploded_array[l])
				continue;

		}

		// add out array element
		out_array[out_element_count-1] = exploded_array;
		out_array[out_element_count]   = NULL;

		exploded_array       = NULL;
		exploded_array_count = 0;

	}

	// delete str utils if necessary
	if(str_util)
		delete str_util;

	// set class members
	this->csv          = out_array;
	this->csv_num_rows = out_element_count;

	// return indicating success
	return true;

}

// Creates a duplicated string array given a csv buffer (\n between rows).  Resultant csv member lists have null termination.
bool BP_StringUtils::DisplayParsedCsv()
{

	// walk parsed csv rows
	size_t i = 0;
	for(;
		i < this->csv_num_rows;
		i++
		)
	{


		if(!this->csv[i])
			break;

		printf("\n [+] Csv Row (%u :: %u) : %p -- ", i, this->csv_num_rows, this->csv[i]);
		// walk individual strings
		size_t x = 0;
		for(;
			this->csv[i][x];
			x++
			)
		{
			printf("\n - element: %s", this->csv[i][x]);
		}



	}

	// return indicating success
	return true;

}


// Destroys csv if possible.
bool BP_StringUtils::DestroyCsv()
{

	// walk parsed csv rows
	size_t i = 0;
	for(;i < this->csv_num_rows -1;	i++	)
	{

		if(!this->csv[i])
			break;

		size_t x = 0;
		for(;this->csv[i][x];x++)
		{
			if(this->csv[i][x])
				tq_free(this->tq, this->csv[i][x]);
		}

	}

	// destroy csv
	tq_free(this->tq, this->csv);

	// set csv vars to null
	this->csv = NULL;
	this->csv_num_rows = 0;

	// return indicating success
	return true;

}




// When matches are made in the same string.  This can determine if match_inner, is between match_outer.  Useful for handling
// quoted nested data.
bool BP_StringUtils::MatchIsWithinOtherMatch(P_BP_REGEX_MATCH_INFO match_outer, P_BP_REGEX_MATCH_INFO match_inner)
{

	// return failure if the matches don't exist
	if(!match_outer || !match_inner)
		return false;

	// This is the check done to check if a string exists within a string.
	if(
			(match_outer->matches->rm_so >= match_inner->matches->rm_so) &&
			(match_outer->matches->rm_eo <= match_inner->matches->rm_eo)
	)
	{
		return true;
	}

	// return false in the case that the string is not found within a string
	return false;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% XML Parsing %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// this fileparses an xml file into the local class xmlDoc pointer
BP_ERROR_T BP_StringUtils::xmlParseFile(char * file_name)
{

	// ensure we have a file name
	if(!file_name)
		return ERR_FAILURE;

	// check for xmldoc, and destroy all.
	if(this->xml_doc)
	{
		 this->xmlDestroyAll();
	}

	 LIBXML_TEST_VERSION

	// read document
	this->xml_doc = xmlReadFile(file_name, NULL, 0);

	// check to ensure we have an xml doc
	if(!this->xml_doc)
		 return ERR_FAILURE;

	// store the file name
	this->xml_file_name = (char *) tq_strdup(this->m, file_name, BPLN);

	// set flag to true
	this->xml_parsed_ok = true;

	// return indicating success (parse ok)
	return ERR_SUCCESS;

}

// destroy the local xml_doc
BP_ERROR_T BP_StringUtils::xmlDestroyAll()
{

	// ensure we have parsed xml
	if(!this->xml_parsed_ok)
		return ERR_FAILURE;

	// check for xmldoc
	if(this->xml_doc)
	{

		// free xml document
		xmlFreeDoc(this->xml_doc);

		// reset document pointer
		this->xml_doc = NULL;

		// reset root node
		this->xml_root_node         = NULL;

		// reset current node
		this->xml_curr_node         = NULL;
		this->xml_curr_node_name    = NULL;

		// reset properties
		this->xml_curr_prop         = NULL;
		this->xml_curr_prop_name    = NULL;
		this->xml_curr_prop_content = NULL;

		// destroy the file name
		tq_free(this->m, this->xml_file_name);
		this->xml_file_name = NULL;

	}

	// return indicating success
	return ERR_SUCCESS;

}

// this gets the xml root element
xmlNode * BP_StringUtils::xmlGetRootNodeElement()
{

	// ensure we have parsed xml
	if(!this->xml_parsed_ok)
		return NULL;

	// check that we have a xml doc pointer
	if(!this->xml_doc)
		return NULL;

	// return the xml root element
	this->xml_root_node = xmlDocGetRootElement(this->xml_doc);

	// return with bad value
	return this->xml_root_node;

}

// gets the next xml element or NULL if failed.
xmlNode * BP_StringUtils::xmlGetNextNodeElement()
{

	// ensure we have parsed xml
	if(!this->xml_parsed_ok)
		return NULL;

	// if we haven't resolved a current node by now, we can return with failure
	if(!this->xml_curr_node)
	{
		return NULL;
	}

	// set current node as next node
	xml_curr_node = xml_curr_node->next;


	// return the next node
	return this->xml_curr_node;

}


//gets an xml element by name or NULL if failed.
xmlNode * BP_StringUtils::xmlGetElementNodeByNamePCRE(char * pcre_regexp)
{

	// ensure we have parsed xml
	if(!this->xml_parsed_ok)
		return NULL;

	// check that we have a root node
	if(!this->xml_curr_node)
		return NULL;

	// set a temp node from current
	xmlNode * xml_node = this->xml_curr_node;

	// walk xml nodes
	for(; xml_node; xml_node = xml_node->next)
	{

		// ensure we have a node element
		if (xml_node->type == XML_ELEMENT_NODE)
		{

			// set name length
			size_t name_len = 0;

			// calculate name
			if(xml_node->name)
			{

				// set name length
				name_len = strnlen((char *) xml_node->name, 8096);

				// create new match structure (required for regexp search)
				BP_REGEX_MATCH_INFO match_info;
				memset(&match_info, 0x00, sizeof(match_info));

				// attempt to match node by provided regexp
				if(BP_MatchRegularExpressionPCRE((char *) xml_node->name, name_len, pcre_regexp, 0, 1024, &match_info, 0) == ERR_SUCCESS)
				{
					// destroy match information
					BP_DestroyRegexpMatchInfo(&match_info);

					// return the matched node
					return xml_node;
				}

				// destroy regular expression match information
				BP_DestroyRegexpMatchInfo(&match_info);

			}

		}
	}

	// return the node
	return NULL;
}

// === XML Display Routines ==================================

// print the node
BP_ERROR_T BP_StringUtils::xmlPrintNode(xmlNode * node, size_t depth)
{

	if(!node)
		return ERR_FAILURE;

	// create new string builder
	BP_StringUtils * builder = new BP_StringUtils(NULL);

	// add empty string (frontload)
	builder->BuildString((char *) "");

	// add depth
	size_t n = 0;
	for(; n < depth; n++)
		builder->BuildString((char *) "\t");

	// display node data
	printf("\n%s [+] Displaying Node: %p - %s",builder->build_str, node, node->name);
	printf("\n%s\t name:     %s", builder->build_str, node->name);
	printf("\n%s\t private:  %p", builder->build_str, node->_private);
	printf("\n%s\t content:  %s", builder->build_str, node->content);
	printf("\n%s\t doc:      %p", builder->build_str, node->doc);
	printf("\n%s\t extra:    %u", builder->build_str, node->extra);
	printf("\n%s\t parent:   %p", builder->build_str, node->parent);
	printf("\n%s\t last:     %p", builder->build_str, node->last);
	printf("\n%s\t next:     %p", builder->build_str, node->next);
	printf("\n%s\t children: %p", builder->build_str, node->children);
	printf("\n%s\t line:     %u", builder->build_str, node->line);
	printf("\n%s\t ns:       %p", builder->build_str, node->ns);
	printf("\n%s\t nsDef:    %p", builder->build_str, node->nsDef);
	printf("\n%s\t prev:     %p", builder->build_str, node->prev);
	printf("\n%s\t props:    %p", builder->build_str, node->properties);
	printf("\n%s\t psvi:     %p", builder->build_str, node->psvi);
	printf("\n%s\t type:     %u", builder->build_str, node->type);
	printf("\n");

	// destroy the builder
	delete builder;

	// return indicating success
	return ERR_SUCCESS;

}

// displays xml properties
BP_ERROR_T BP_StringUtils::xmlPrintNodeProperty(xmlAttr  * prop, size_t depth)
{

	// ensure we have a property list to print
	if(!prop)
		return ERR_FAILURE;

	// create new string builder
	BP_StringUtils * builder = new BP_StringUtils(NULL);

	// add empty string (frontload)
	builder->BuildString((char *) "");

	// add depth
	size_t n = 0;
	for(; n < depth; n++)
		builder->BuildString((char *) "\t");

	printf("\n%s\t [+] Property %p - %s", builder->build_str,  prop, prop->name);

	printf("\n%s\t name:          %s", builder->build_str,  prop->name);
	printf("\n%s\t _private:      %p", builder->build_str,  prop->_private);
	printf("\n%s\t atype:         %u", builder->build_str,  prop->atype);
	printf("\n%s\t children:      %p", builder->build_str,  prop->children);
	if(prop->children)
	printf("\n%s\t child content: %s", builder->build_str,  prop->children->content);
	printf("\n%s\t doc:           %p", builder->build_str,  prop->doc);
	printf("\n%s\t last:          %p", builder->build_str,  prop->last);
	printf("\n%s\t next:          %p", builder->build_str,  prop->next);
	printf("\n%s\t ns:            %p", builder->build_str,  prop->ns);
	printf("\n%s\t parent:        %p", builder->build_str,  prop->parent);
	printf("\n%s\t prev:          %p", builder->build_str,  prop->prev);
	printf("\n%s\t psvi:          %p", builder->build_str,  prop->psvi);
	printf("\n%s\t type:          %u", builder->build_str,  prop->type);
	printf("\n");

	// destroy the builder
	delete builder;

	// return indicating success
	return ERR_SUCCESS;

}

// == XML property operations ================================
//
//  (properties are members of nodes)
//
// If you supply a NULL node to any of the following methods
// they ill use this->curr_node as a parameter.  This can be
// used for easy recursing.
//
//
xmlAttr * BP_StringUtils::xmlGetTopPropertyForNode(xmlNode * node)
{

	// ensure we have parsed xml
	if(!this->xml_parsed_ok)
		return NULL;

	// top node
	xmlNode * top_node = NULL;

	// set current node
	if(!node)
		node = this->xml_curr_node;

	// ensure we have a node, if not exit
	if(!node)
		return NULL;

	// ensure the node has properties
	if(!node->properties)
		return NULL;

	// check to see if we have a prev node link (if not
	// we are the top link and can return without seaching)
	if(!node->properties->prev)
		return node->properties;

	// set properties
	xmlAttr * iter = node->properties->prev;

	// walk backwards
	for(; iter->prev; iter = iter->prev){}

	// return the top property
	return iter;

}

// searches xml properties
xmlAttr * BP_StringUtils::xmlSearchNodePropertiesByNamePCRE(xmlNode * node, char *pcre_regexp)
{

	// ensure we have parsed xml
	if(!this->xml_parsed_ok)
		return NULL;

	// ensure we have a regex
	if(!pcre_regexp)
		return NULL;

	// resolve to current node if arg is null
	if(!node)
		node = this->xml_curr_node;

	// ensure we have a resolved node
	if(!node)
		return NULL;

	// set properties
	xmlAttr * iter = node->properties;

	// walk forwards and search for match
	for(; iter; iter = iter->next)
	{

		// check iter
		size_t name_len = 0;
		if(iter->name)
		{
			// set name length
			name_len = strnlen((char *) iter->name, 8096);

			// create new match structure (required for regexp search)
			BP_REGEX_MATCH_INFO match_info;
			memset(&match_info, 0x00, sizeof(match_info));

			// attempt to match node by provided regexp
			if(BP_MatchRegularExpressionPCRE((char *) iter->name, name_len, pcre_regexp, 0, 1024, &match_info, 0) == ERR_SUCCESS)
			{

				// destroy match information
				BP_DestroyRegexpMatchInfo(&match_info);

				// return the matched node
				return iter;

			}

			// destroy regular expression match information
			BP_DestroyRegexpMatchInfo(&match_info);

		}

	}

	// return element
	return NULL;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%% Creating Unique String Stacks %%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Joins a string on an internal allocated buffer (good for creating dynamic strings).  Be careful with overly long strings.
bool BP_StringUtils::BuildString(char * str_to_add)
{

	if(!str_to_add)
		return false;

	// set build and add string lengths
	size_t build_str_len = 0;
	size_t add_str_len   = bpstrlen(str_to_add);

	if(this->build_str)
		build_str_len = bpstrlen(this->build_str);

	// bpreallocate space
	this->build_str = (char *) tq_realloc(this->tq, this->build_str, build_str_len + add_str_len + 10, BPLN);
	if(!this->build_str)
	{
		printf("\n [+] build string alloc failed for string: %s", str_to_add);
		return false;
	}

	// zero out memory
	memset(&this->build_str[build_str_len], 0x00, add_str_len+4);

	// copy in string up till length
	strncat(this->build_str, str_to_add, add_str_len);

	// return indicating success
	return true;
}

// builds on the current string using a supplied buffer
bool BP_StringUtils::BuildMySQLHexFromBuff(char *buff, size_t buff_n)
{

	if(!buff)
		buff = "empty";
	if(!buff_n)
		buff_n = bpstrlen("empty");

	// build the mysql hex string
	char *buff_mysql_s = (char *) BP_BufferToMySQLHexString(buff, buff_n);

	// check to see that we have created the string
	if(!buff_mysql_s)
		return false;

	// build the string inwards
	this->BuildString((char *) buff_mysql_s);

	// free the chunk
	bpfree(buff_mysql_s);

	// return indicating success
	return true;

}

// builds on the current string using a supplied buffer
bool BP_StringUtils::BuildMySQLHexFromSizeT(size_t val)
{

	// build the mysql hex string
	char *buff_mysql_s = (char *) BP_SizeTToMySQLBuffer(val);

	// check to see that we have created the string
	if(!buff_mysql_s)
		return false;

	// build the string inwards
	this->BuildString((char *) buff_mysql_s);

	// free the chunk
	bpfree(buff_mysql_s);

	// return indicating success
	return true;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Overloaded Operators for String Building %%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// STRING BUILDING OPERATORS

// overloaded operator for equals
BP_StringUtils& BP_StringUtils::operator+=(char *str_to_add)
{
	this->BuildString(str_to_add);
	return *this;
}

// STRING SUBTRACTION OPERATORS

// removes all instances of the specified string from this->string.  This is an
// alias for RegFindReplacePCRE.
// BP_StringUtils& operator-=(char *string_to_remove)



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Perl Compatible Regular Expressions %%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// matches a perl compatible regular expression (simple bool; uses local this->string)
bool BP_StringUtils::RegMatchPCRE(char *match_regexp)
{
	return true;
}

// Adds a find/replace set to the stack.  Find is a valid perl compatible regular expression.
bool BP_StringUtils::AddRegFindPCRE(char *replace_this, char *replace_with)
{
	return true;
}

// Runs a find and replace against the string data.
bool BP_StringUtils::RegFindReplacePCRE()
{
	return true;
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%% Regular Expression Matching %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// matches a regular expression (simple bool; uses local this->string)
bool BP_StringUtils::RegMatch(char *match_regexp)
{

	// string to test
	char * test_string = NULL;

	// select string
	if(this->string)
		test_string = this->string;
	if(this->build_str)
		test_string = this->build_str;

	// ensure we have a string before continuing
	if(!test_string)
		return false;

	// return false if string is empty
	if(!this->string || !bpstrlen(this->string))
	if(!this->build_str || !bpstrlen(this->build_str))
	if(!match_regexp)
		return false;

	// create match info structure
	BP_REGEX_MATCH_INFO match_info = {0};

	// return true if the regular expression matches
	if(BP_MatchRegularExpression(this->string, bpstrlen(this->string), match_regexp, 0, 1, (P_BP_REGEX_MATCH_INFO) NULL) == ERR_SUCCESS)
		return true;

	// return indicating failure (nomatch)
	return false;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%% Creating Unique String Stacks %%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Adds a unique string to the stack.  Returns false if the string already exists, true if added.
bool BP_StringUtils::AddUniqueString(char *str)
{

	// ensure we have a string being passed in
	if(!str)
	{
		this->AddErr("No string passed to unique stacker.");
		return false;
	}

	// walk strings and check if string exists
	size_t i = 0;
	for(; i< this->unique_str_cnt; i++)
	{

		size_t min_check = strnlen(this->unique_strings[i], 4096);
		if(!min_check)
			continue;

		size_t max_check = strnlen(str, 4096);
		if(min_check < max_check)
			continue;

		if(memcmp(this->unique_strings[i], str, bpstrlen(str) ) == 0)
		{
			this->unique_string_index_counts[i]++;
			return false;
		}
	}

	// add string to list if its not found
	this->unique_str_cnt++;
	this->unique_strings = (char **) tq_realloc(this->tq, this->unique_strings, sizeof(char *) * ( this->unique_str_cnt + 1 ) , BPLN);

	// add space for iterative counter
	this->unique_string_index_counts = (size_t *) tq_realloc(this->tq, this->unique_string_index_counts, sizeof(size_t) * (this->unique_str_cnt +1 ), BPLN );

	// since this is the first iteration, add the update count as first value (1)
	this->unique_string_index_counts[this->unique_str_cnt-1] = 1;

	// duplicate the string where necessary (auto-free'd on delete)
	if(this->duplicate_unique_strings)
		this->unique_strings[this->unique_str_cnt-1] = tq_strdup(this->tq, str, BPLN);
	else
		this->unique_strings[this->unique_str_cnt-1] = str;

	this->unique_strings[this->unique_str_cnt]   = NULL;

	// return indicating success (string added)
	return true;
}

// simple mechanism for displaying unique strings
BP_ERROR_T BP_StringUtils::DisplayUniqueStrings()
{

	if(!this->unique_str_cnt)
		return ERR_FAILURE;

	// iterate through the unique strings stored
	size_t n = 0;
	for(; n < this->unique_str_cnt; n++)
	{
		printf("\n%s", this->unique_strings[n]);
	}

	// return indicating success
	return ERR_SUCCESS;

}


// Uses BuildString method to build a string containing CSV for each item.
BP_ERROR_T BP_StringUtils::buildUniqueStringStackAsCSV(BP_STRINGUTILS_BUILD_UNIQUE_SELECTION build_selection)
{

	// check to ensure we have some uniqe strings
	if(!this->unique_str_cnt)
		return ERR_FAILURE;
	if(!this->unique_strings)
		return ERR_FAILURE;

	// build the stack
	size_t n = 0;
	for(; n < this->unique_str_cnt; n++)
	{

		// add open quote
		if(build_selection == BP_STRINGUTILS_BUILD_DOUBLE_QUOTED_CSV)
			this->BuildString((char *) "\"");
		if(build_selection == BP_STRINGUTILS_BUILD_SINGLE_QUOTED_CSV)
			this->BuildString((char *) "'");
		if(build_selection == BP_STRINGUTILS_BUILD_UNQUOTED_CSV){}


		// build the csv
		this->BuildString(this->unique_strings[n]);

		// add closing terminator
		if(build_selection == BP_STRINGUTILS_BUILD_DOUBLE_QUOTED_CSV)
			this->BuildString((char *) "\"");
		if(build_selection == BP_STRINGUTILS_BUILD_SINGLE_QUOTED_CSV)
			this->BuildString((char *) "'");
		if(build_selection == BP_STRINGUTILS_BUILD_UNQUOTED_CSV){}

		// skip adding comma if we're at the last value
		if((n+1) >= this->unique_str_cnt)
			break;

		// add comma on loop
		this->BuildString((char *) ", ");

	}

	return ERR_SUCCESS;
}


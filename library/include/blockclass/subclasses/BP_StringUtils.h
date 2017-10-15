/*
 * BP_StringUtils.h
 *
 *  Created on: Feb 22, 2011
 *      Author: root
 */

#ifndef BP_STRINGUTILS_H_
#define BP_STRINGUTILS_H_


// CSV builder enumerations.  See forward comments for descriptions of each field.
typedef enum
{

	BP_STRINGUTILS_BUILD_ERROR,               // erroneous conditions
	BP_STRINGUTILS_BUILD_SINGLE_QUOTED_CSV,   // all fields were detected as being single quoted csv
	BP_STRINGUTILS_BUILD_DOUBLE_QUOTED_CSV,   // all fields were detected as being double quoted csv
	BP_STRINGUTILS_BUILD_UNQUOTED_CSV,        // all fields were unquoted literal strings seperated directly at their boundries
	BP_STRINGUTILS_BUILD_MIXED_CSV            // fields contain a mix of awkward character encapsulations.

} BP_STRINGUTILS_BUILD_UNIQUE_SELECTION;


//! This is a purely utilitarian class designed to handle a variety of
//  string-manipulation related tasks.  All allocations should be done
//  via the tailqueue allocator (using tq_alloc functions) such that
//  a delete capsizes all memory chunks.  This method should not be used
//  as anything other than by way of new.
class BP_StringUtils : public BP_BaseError
{


public:


	// Class memory allocator (tail queue allocator).  Fast allocs.
	P_BP_LINKL tq;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %% Class Constructor and Destructor %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// class constructor.  Pass in a valid string and it will be duplicated
	// and stored in the this->string member.  This member will be used for
	// all pivoting string operations.
	BP_StringUtils(char *string);

	// Deconstruct and destroy all associated memory.  Easy cleanup.
	~BP_StringUtils();

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %% Main String Input %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This string member is used by all the methods below, and is
	// set automatically to whatever value the class constructor is
	// passed in as.  This should be a null terminated ascii string.

	// string used as input (initialized as null)
	char *string;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Overloaded Operators for String Building %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// STRING BUILDING OPERATORS

	// overloaded operator for string building.  Just simply adds the
	// string to add to the current this->string.  The this->string pointer
	// is tq_reallocated and just truncated with the contents of the add-in string.
	BP_StringUtils& operator+=(char *str_to_add);

	// STRING SUBTRACTION OPERATORS

	// removes all instances of the specified string from this->string.  This is an
	// alias for RegFindReplacePCRE.
	// BP_StringUtils& operator-=(char *string_to_remove)



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%% Building / Joining Strings %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// NOTE: Joining is done via reallocation.

	// string used for building
	char *build_str;



	// joins a string on an internal allocated buffer (good for creating dynamic strings)
	bool BuildString(char * str_to_add);

	// builds on the current string using a supplied buffer
	bool BuildMySQLHexFromBuff(char *buff, size_t buff_n);

	// builds on the current string using a supplied buffer
	bool BuildMySQLHexFromSizeT(size_t val);




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%% Regular Expressions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This is the POSIX regular expression version of regular expression matching.  Do
	// not use these methods/members unless you want POSIX expressions.  For all
	// PCRE expression, use the similarly named RegMatchPREG (etc) functions found in the
	// subsections below.


	// matches a regular expression (simple bool; uses local this->string)
	bool RegMatch(char *match_regexp);

	// result of output (for find/replace etc)
	char *mod_string;

	// array of strings to use as input to find and replace
	char ** find_strs;
	char ** replace_strs;

	// total number of items to find
	size_t find_item_count;

	// Adds a find/replace set to the stack.  Find is regular expression.
	bool AddRegFind(char *replace_this, char *replace_with);

	// runs find and replace loop
	bool RegFindReplace();

	// destroy the registry find and replace
	bool DestroyRegFindReplaceData();


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Perl Compatible Regular Expressions %%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This is the POSIX regular expression version of regular expression matching.  Do
	// not use these methods/members unless you want POSIX expressions.  For all
	// PCRE expression, use the similarly named RegMatchPREG (etc) functions found in the
	// subsections below.


	// result of output (for find/replace etc)
	char *mod_string_pcre;

	// array of strings to use as input to find and replace
	char ** find_strs_pcre;
	char ** replace_strs_pcre;

	// total number of items to find
	size_t find_item_count_pcre;


	// matches a regular expression (simple bool; uses local this->string)
	bool RegMatchPCRE(char *match_regexp);

	// Adds a find/replace set to the stack.  Find is regular expression.
	bool AddRegFindPCRE(char *replace_this, char *replace_with);

	// runs find and replace loop
	bool RegFindReplacePCRE();


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Explode creates a char ** from char * based on token.  %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Doesn't modify original string.  Returns strdup()'d strings.
	char ** exploded;
	size_t  exploded_count;

	// if this is set to true, explode will not trigger between quoted values
	bool dont_explode_between_doublequotes;
	bool dont_explode_between_singlequotes;

	// explode based on regular expression delimiter
	bool Explode(char *reg_delim);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%% Parse CSV Data from Buffer %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Blockparty string utility for parsing a varied set of CSV data.  This data relies
	// on the content of this->string being set.  It will first explode the content of
	// the string content by newline, and then attempt to filter/parse each resultant
	// newline.  This capacity is aggressive, and can result in faulty parsing in the case
	// that incorrect parser hints being provided.  If in doubt, the parser hint of:
	//
	// 		this->csv_parser_hint = BP_STRINGUTILS_BUILD_MIXED_CSV;
	//
	// is the safest bet you can make.  It's really what should be chosen in almost
	// every situation, but it is marginally slower than the direct single/double quote
	// hints.
	//

	// Final parse output.  Data appears as follows. Row data length is not fixed,
	// which means rows can have different numbers of entries.
	// this->csv[0][0][0]
	// [index]
	//    |______[row0] <- selected
	//    |        |____ [entry0] <- selected
	//    |        |____ [entry1]
	//    |        |____ [entryn]
	//    |        |____ [NULL]
	//    |
	//    |______[rowN] <- not selected

	// NOTE: All parsing is done in memory, so the more memory you have available, the
	//       better.  A file based version of this capacity may be added in the future
	//       to deal with potentially large csv sets, but that is a certain todo.


	// This hint can be set to any value valid to the type enumerator of which
	// it belongs.  Mainly this is used to give hints to the CSV parser as to what
	// type of data will be parsed.  The plain types, aka unmixed types, are faster
	// due to a lesser constraint on parsing mechanics, however adding a mixed parsing
	// hint will attempt to safely parse out csv data using methodical detection of
	// constraints.
	BP_STRINGUTILS_BUILD_UNIQUE_SELECTION csv_parsing_hint;



	// This contains the parsed csv output, terminated at each dimension by a null pointer.
	char ***csv;

	// number of rows created
	size_t csv_num_rows;

	// csv parser (fills in this->csv)
	bool ParseCsv(bool remove_quotes);

	// display parsed csv (utility)
	bool DisplayParsedCsv();

	// destroys csv and all pointers
	bool DestroyCsv();

	// checks if a match made exists within another match (useful for detecting nesting.  True on the condition that match is found within match.
	bool MatchIsWithinOtherMatch(P_BP_REGEX_MATCH_INFO match_outer, P_BP_REGEX_MATCH_INFO match_inner);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% XML Parsing %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// The following methods and members are used specifically for the purpose
	// of parsing out arbitrary xml content.  This was added directly in
	// this class for the purpose of providing an easy and clear mechanism
	// for handling generic XML parsing.




	// NOTE: XML Parsing is done via libxml2.  For examples on how
	// to understand the process of accessing elements, see the xmlparsing
	// example in the blockparty library examples folder.
	bool xml_parsed_ok;

	// filename of document currently being worked on
	char * xml_file_name;

	// parsed xml document
	xmlDoc *  xml_doc;

	// root node associated with the xml document
	xmlNode * xml_root_node;

	// current xml node
	xmlNode * xml_curr_node;
	char *    xml_curr_node_name;
	char *    xml_curr_node_content;

	// set current xml property if so set
	xmlAttr  * xml_curr_prop;
	char     * xml_curr_prop_name;
	char     * xml_curr_prop_content;

	// this fileparses an xml file into the local class xmlDoc pointer
	BP_ERROR_T xmlParseFile(char * file_name);

	// destroy the local xml_doc
	BP_ERROR_T xmlDestroyAll();

	// === XML Display Routines ==================================

	// display xml node
	BP_ERROR_T xmlPrintNode(xmlNode * node, size_t depth);

	// displays xml properties
	BP_ERROR_T xmlPrintNodeProperty(xmlAttr  * prop, size_t depth);



	// ==== xml node operations (top level) =================

	// this gets the xml root element or NULL if failed.
	xmlNode * xmlGetRootNodeElement();

	// gets the next xml element or NULL if failed.
	xmlNode * xmlGetNextNodeElement();

	// gets an xml element by name or NULL if failed.  You can search
	// recursively using returned data.
	xmlNode * xmlGetElementNodeByNamePCRE(char * pcre_regexp);


	// == xml property operations ===========================
	//
	//  (properties are members of nodes)
	//
	// If you supply a NULL node to any of the following methods
	// they ill use this->curr_node as a parameter.  This can be
	// used for easy recursing.
	//
	xmlAttr * xmlGetTopPropertyForNode(xmlNode * node);

	// searches xml properties
	xmlAttr * xmlSearchNodePropertiesByNamePCRE(xmlNode * node, char *pcre_regexp);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%% Creating Unique String Stacks %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// These methods/members are used for the purpose of building up
	// stacks of unique strings.  The uniqueness requires a linear search currently,
	// which will be changed to use hash tables in the future.  Still, pretty
	// damn fast as the linear search is done via simple byte comparisons from
	// left->right.

	// contains unique strings (strings are not duplicated by default, they are just pointers)
	char **unique_strings;

	// a count for each index of the unique string table above
	size_t * unique_string_index_counts;

	// total number of strings in the tables
	size_t unique_str_cnt;

	// if this flag is set, unique strings are duplicated (this also means that they
	// are free'd on delete).
	bool duplicate_unique_strings;

	// Adds a unique string to the stack.  Returns false if the string already exists, true if added.
	bool AddUniqueString(char *str);

	// Uses BuildString method to build a string containing CSV for each item.
	BP_ERROR_T buildUniqueStringStackAsCSV(BP_STRINGUTILS_BUILD_UNIQUE_SELECTION build_selection);

	// display the unique string stack
	BP_ERROR_T DisplayUniqueStrings();

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%% Pure utility methods for operating on data %%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// randomizes the pointers found in a char ** (useful for randomizing arguments, or other ptr array data)
	char ** RandomizePointerArray(char **ptrs, size_t num_ptrs);




};
#endif /* BP_STRINGUTILS_H_ */

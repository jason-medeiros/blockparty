#ifndef __BP_HTMLDECODER_H
#define __BP_HTMLDECODER_H

/*
 * The HTML decoder was one of the first parts of the blockparty that was
 * written.  It was absolutely needed for several parts of the library
 * and it was written hastily.  It was also written long before any
 * garbage collection was written, so initially, it was very leaky.  I'm
 * working at cleaning the code up, and bringing all allocations into
 * the scope of garbage collection, but the process is not yet finished.
 *
 * Long story short, if the design looks odd, it's because a lot of
 * modern functionality was tacked onto a functional parser.
 *
 * JM
 * 9/1/2014
 *
 */


// anchor protocol types, each protocol in the enum BP_ANCHOR_PROTO
// corresponds to one of these sub-archetypes.  These sub-archetypes then are used
// for information control.
enum BP_ANCHOR_PROTO_TYPE
{
	BP_ANCHOR_TYPE_RECON,
	BP_ANCHOR_TYPE_SENSITIVE_INFO,
	BP_ANCHOR_TYPE_ONSITE_LINK,
	BP_ANCHOR_TYPE_OFFSITE_LINK,
	BP_ANCHOR_TYPE_ONSITE_LINK_CGI,
	BP_ANCHOR_TYPE_OFFSITE_LINK_CGI,
	BP_ANCHOR_TYPE_MISC
};

// anchor protocol types used for decoding href values
enum BP_ANCHOR_PROTO
{
	   BP_ANCHOR_PROTO_DEFAULT,
	   BP_ANCHOR_PROTO_BASE_RELATIVE_LINK,
	   BP_ANCHOR_PROTO_RELATIVE_LINK,
	   BP_ANCHOR_PROTO_DOT_SLASH_RELATIVE_LINK,
	   BP_ANCHOR_PROTO_ACAP,
	   BP_ANCHOR_PROTO_ADDBOOK,
	   BP_ANCHOR_PROTO_AFP,
	   BP_ANCHOR_PROTO_AFS,
	   BP_ANCHOR_PROTO_AIM,
	   BP_ANCHOR_PROTO_CALLTO,
	   BP_ANCHOR_PROTO_CASTANET,
	   BP_ANCHOR_PROTO_CHTTP,
	   BP_ANCHOR_PROTO_CID,
	   BP_ANCHOR_PROTO_DATA,
	   BP_ANCHOR_PROTO_DAV,
	   BP_ANCHOR_PROTO_DICT,
	   BP_ANCHOR_PROTO_DNS,
	   BP_ANCHOR_PROTO_EID,
	   BP_ANCHOR_PROTO_FAX,
	   BP_ANCHOR_PROTO_FILE,
	   BP_ANCHOR_PROTO_FINGER,
	   BP_ANCHOR_PROTO_FREENET,
	   BP_ANCHOR_PROTO_FTP,
	   BP_ANCHOR_PROTO_FTPS,
	   BP_ANCHOR_PROTO_GOPHER,
	   BP_ANCHOR_PROTO_GSM_SMS,
	   BP_ANCHOR_PROTO_H323,
	   BP_ANCHOR_PROTO_H324,
	   BP_ANCHOR_PROTO_HDL,
	   BP_ANCHOR_PROTO_HNEWS,
	   BP_ANCHOR_PROTO_HTTP,
	   BP_ANCHOR_PROTO_HTTPS,
	   BP_ANCHOR_PROTO_IIOPLOC,
	   BP_ANCHOR_PROTO_ILU,
	   BP_ANCHOR_PROTO_IOR,
	   BP_ANCHOR_PROTO_IMAP,
	   BP_ANCHOR_PROTO_IPP,
	   BP_ANCHOR_PROTO_IRC,
	   BP_ANCHOR_PROTO_JAR,
	   BP_ANCHOR_PROTO_JAVA,
	   BP_ANCHOR_PROTO_JAVASCRIPT,
	   BP_ANCHOR_PROTO_JDBC,
	   BP_ANCHOR_PROTO_LDAP,
	   BP_ANCHOR_PROTO_LIFN,
	   BP_ANCHOR_PROTO_LIVESCRIPT,
	   BP_ANCHOR_PROTO_ICQ,
	   BP_ANCHOR_PROTO_IRQ,
	   BP_ANCHOR_PROTO_MAILBOX,
	   BP_ANCHOR_PROTO_MAILTO,
	   BP_ANCHOR_PROTO_MD5,
	   BP_ANCHOR_PROTO_MID,
	   BP_ANCHOR_PROTO_MIDI,
	   BP_ANCHOR_PROTO_MOCHA,
	   BP_ANCHOR_PROTO_MODEM,
	   BP_ANCHOR_PROTO_NEWS,
	   BP_ANCHOR_PROTO_NFS,
	   BP_ANCHOR_PROTO_NNTP,
	   BP_ANCHOR_PROTO_OPAQUELOCKTOKEN,
	   BP_ANCHOR_PROTO_PHONE,
	   BP_ANCHOR_PROTO_POP,
	   BP_ANCHOR_PROTO_POP3,
	   BP_ANCHOR_PROTO_PRINTER,
	   BP_ANCHOR_PROTO_PROSPERO,
	   BP_ANCHOR_PROTO_RES,
	   BP_ANCHOR_PROTO_RSTP,
	   BP_ANCHOR_PROTO_RVP,
	   BP_ANCHOR_PROTO_RWHOIS,
	   BP_ANCHOR_PROTO_RX,
	   BP_ANCHOR_PROTO_SDP,
	   BP_ANCHOR_PROTO_SERVICE,
	   BP_ANCHOR_PROTO_SIP,
	   BP_ANCHOR_PROTO_SMB,
	   BP_ANCHOR_PROTO_SNEWS,
	   BP_ANCHOR_PROTO_SOAP_BEEP,
	   BP_ANCHOR_PROTO_SOAP_BEEPS,
	   BP_ANCHOR_PROTO_T120,
	   BP_ANCHOR_PROTO_TCP,
	   BP_ANCHOR_PROTO_TEL,
	   BP_ANCHOR_PROTO_TELNET,
	   BP_ANCHOR_PROTO_TIP,
	   BP_ANCHOR_PROTO_TN3270,
	   BP_ANCHOR_PROTO_TV,
	   BP_ANCHOR_PROTO_URN,
	   BP_ANCHOR_PROTO_UUID,
	   BP_ANCHOR_PROTO_VEMMI,
	   BP_ANCHOR_PROTO_VIDEOTEX,
	   BP_ANCHOR_PROTO_VIEW_SOURCE,
	   BP_ANCHOR_PROTO_WAIS,
	   BP_ANCHOR_PROTO_WHODP,
	   BP_ANCHOR_PROTO_WHOIS,
	   BP_ANCHOR_PROTO_Z39_50R,
	   BP_ANCHOR_PROTO_Z39_50S,
	   BP_ANCHOR_PROTO_UNKNOWN
};

// form method enum
enum BP_HTML_FORM_METHOD
{
	BP_HTML_FORM_METHOD_UNKNOWN,
	BP_HTML_FORM_METHOD_GET,
	BP_HTML_FORM_METHOD_POST,
	BP_HTML_FORM_METHOD_UNSET
};


// ------------------------------------------------------------------------------------
// Important String Manipulation Function
int BP_strncasecmp(char *in1, char *in2, size_t len);

// --------------------------------------------------------------------------------------
// Begin anchor extraction structures


// used to parse anchors for variables based on "start var=val delimit var2=val delmit"
// format.
typedef struct _BP_HTML_ANCHOR_DELIMITERS {

	char *startDelimiter;
		size_t sLen;

	char *valueDelimiter;
		size_t vLen;

	char *nextDelimiter;
		size_t nLen;

} BP_HTML_ANCHOR_DELIMITERS, *P_BP_HTML_ANCHOR_DELIMITERS;


// Variable List, name value length
typedef struct _BP_HTML_ANCHOR_VAR_LIST {

	// variable name
	char *name;
	size_t nameLen;

	// content
	char *content;
	size_t contentLen;

	// List ptrs
	struct _BP_HTML_ANCHOR_VAR_LIST *first;
	struct _BP_HTML_ANCHOR_VAR_LIST *prev;
	struct _BP_HTML_ANCHOR_VAR_LIST *next;

} BP_HTML_ANCHOR_VAR_LIST, *P_BP_HTML_ANCHOR_VAR_LIST;


// Anchor Structure
typedef struct _BP_HTML_ANCHOR_LIST{

	// anchor number
	size_t num;

	// anchor leads offsite, TRUE/FALSE
	size_t offsite;

	// delimiter set used to extract variables from the string
	P_BP_HTML_ANCHOR_DELIMITERS delims;

	// RAW unparsed href
	char * href;

	// Anchor Protocol
	BP_ANCHOR_PROTO proto;

	// strnduped protocol string
	char * protoStr;

	// All embedded variables found in the string
	P_BP_HTML_ANCHOR_VAR_LIST vars;

	// Anchor Path relative to site rootdir
	char * relativePath;

	// flag to indicate if this form is blacklisted or not
	size_t blacklisted;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Parsed Values (Added 2014) %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// These values are always valid so long as the, and can be used for any purpose
	// extractor is provided a valid HTTP session.  They are useful for building
	// any sort of testing strings desired.

	// the host which this anchor belongs to
	char * host;

	// the base (aka http://localhost/) which this anchor is parsed into
	char * base;

	// the path (aka http://localhost/blah/) which this anchor is parsed into
	char * path;

	// the script (aka http://localhost/blah/test.php) which this anchor is parsed into
	char * script;

	// the full url, including embedded parameters (aka http://localhost/blah/test.php?1=1&2=2)
	char * full_url;



	// Linked List Pointers
	struct _BP_HTML_ANCHOR_LIST *first;
	struct _BP_HTML_ANCHOR_LIST *prev;
	struct _BP_HTML_ANCHOR_LIST *next;

} BP_HTML_ANCHOR_LIST, *P_BP_HTML_ANCHOR_LIST;


typedef struct _BP_HTML_FORM_PROPERTY_LIST {
	char * name;
		int nLen;
	char * content;
		int cLen;

	struct _BP_HTML_FORM_PROPERTY_LIST *first;
	struct _BP_HTML_FORM_PROPERTY_LIST *prev;
	struct _BP_HTML_FORM_PROPERTY_LIST *next;

}BP_HTML_FORM_PROPERTY_LIST, *P_BP_HTML_FORM_PROPERTY_LIST;


typedef struct _BP_HTML_INPUT_PROPERTY_LIST {

	char * name;
		int nLen;

	char * content;
		int cLen;

	struct _BP_HTML_INPUT_PROPERTY_LIST *first;
	struct _BP_HTML_INPUT_PROPERTY_LIST *prev;
	struct _BP_HTML_INPUT_PROPERTY_LIST *next;

}BP_HTML_INPUT_PROPERTY_LIST, *P_BP_HTML_INPUT_PROPERTY_LIST;


typedef struct _BP_HTML_INPUT_LIST {
	P_BP_HTML_INPUT_PROPERTY_LIST properties;

	// List pointers
	struct _BP_HTML_INPUT_LIST  * first;
	struct _BP_HTML_INPUT_LIST  * prev;
	struct _BP_HTML_INPUT_LIST  * next;

} BP_HTML_INPUT_LIST, *P_BP_HTML_INPUT_LIST;


typedef struct _BP_HTML_FORM_LIST {

	// Properties for this form list (aka action=this, name=that)
	P_BP_HTML_FORM_PROPERTY_LIST properties;

	// Inputs for this form (including select/textbox/etc inputs)
	P_BP_HTML_INPUT_LIST inputs;

	// flag to indicate if this form is blacklisted or not
	size_t blacklisted;

	// form method
	BP_HTML_FORM_METHOD method;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Parsed Values (Added 2014) %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// These values are always valid so long as the, and can be used for any purpose
	// extractor is provided a valid HTTP session.  They are useful for building
	// any sort of testing strings desired.

	// the host which this form belongs to
	char * host;

	// the base (aka http://localhost/) which this form is parsed into
	char * base;

	// the path (aka http://localhost/blah/) which this form is parsed into
	char * path;

	// the script (aka http://localhost/blah/test.php) which this form is parsed into
	char * script;

	// the full url, including embedded parameters (aka http://localhost/blah/test.php?1=1&2=2)
	char * full_url;


	// List Pointers
	struct _BP_HTML_FORM_LIST * first;
	struct _BP_HTML_FORM_LIST * prev;
	struct _BP_HTML_FORM_LIST * next;

} BP_HTML_FORM_LIST, *P_BP_HTML_FORM_LIST;

// Final Cumulative Website Extraction Structure
typedef struct _BP_HTML_FULL_EXTRACTION {

	// HTTP session associated with this extraction.  DO NOT FREE THIS
	// THIS IS FREED BY ITSELF.
	P_BP_HTTP_SESSION http_session_reference;

	// tail queue allocator
	P_BP_LINKL tq;

	// Delimiters used
	P_BP_HTML_ANCHOR_DELIMITERS delims;

	// Forms List
	P_BP_HTML_FORM_LIST forms;

	// Extracted Anchors List
	P_BP_HTML_ANCHOR_LIST anchors;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% OLD ALLOCATOR GARBAGE COLLECTOR %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Garbage collector.  Monitors data use.
	P_BP_GARBAGE_COLLECTOR_DATA gc;

} BP_HTML_FULL_EXTRACTION, *P_BP_HTML_FULL_EXTRACTION;


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Main Decoder Routine %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This is the main workhorse function responsible for decoding
// html.  It should be used directly, and used to build your
// assertions regarding the HTML document.
P_BP_HTML_FULL_EXTRACTION HTMLDecodeBuffer
(
		char *buff,
		int size,
		char *URL,
		char *encoding,
		P_BP_HTML_ANCHOR_DELIMITERS delims,
		int options,
		P_BP_GARBAGE_COLLECTOR_DATA gc_in,
		P_BP_HTTP_SESSION http_session = NULL
);

// Destroys a HTML extraction.
int HTMLDestroyExtraction(P_BP_HTML_FULL_EXTRACTION extracted);

// Visualize a full extraction, including all sublists and data.
void HTMLPrintFullExtraction(P_BP_HTML_FULL_EXTRACTION extraction, size_t display_blacklisted);





// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Anchor Related Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Entry version and recursive version of the parse anchors function.  If
// unsure, use HTMLParseAnchors with a root xmlNode.  The HTMLParseAnchors
// function will automatically recurse where needed.
P_BP_HTML_ANCHOR_LIST HTMLParseAnchors
(
		xmlNode *                   node,
		P_BP_HTML_ANCHOR_DELIMITERS delims,
		P_BP_GARBAGE_COLLECTOR_DATA gc,
		P_BP_HTML_FULL_EXTRACTION extraction = NULL
);

// recursive version (don't use directly)
// recursive version (don't use directly)
P_BP_HTML_ANCHOR_LIST HTMLParseAnchorsRecurse
(
		xmlNode *node,
		P_BP_HTML_ANCHOR_DELIMITERS delims,
		P_BP_HTML_ANCHOR_LIST appendList,
		bool isFirst,
		P_BP_GARBAGE_COLLECTOR_DATA gc,
		P_BP_HTML_FULL_EXTRACTION extraction = NULL
);




// Retrieves variables embedded in anchors using a delimiter set
P_BP_HTML_ANCHOR_VAR_LIST HTMLGetAnchorEmbeddedVariables(char *href, P_BP_HTML_ANCHOR_DELIMITERS delims, size_t len, P_BP_GARBAGE_COLLECTOR_DATA gc);

// Sub-function used in the extraction of embedded variable names
char * HTMLGetAnchorEmbeddedVariableName(char *inStr, P_BP_HTML_ANCHOR_DELIMITERS delims, size_t len, P_BP_GARBAGE_COLLECTOR_DATA gc);

// Sub-function used in the extraction of embedded variable values
char * HTMLGetAnchorEmbeddedVariableValue(char *inStr, P_BP_HTML_ANCHOR_DELIMITERS delims, size_t len, P_BP_GARBAGE_COLLECTOR_DATA gc);


// Returns a NULL terminated c style character string based off a BP anchor
// protocol value. NULL on failure.
char * HTMLGetAnchorProtocolStringFromVal(size_t val);

// Retrieves a value that correlates with the BP_ANCHOR_PROTO enum.  Default
// indicates that there is no protocol associated with the anchor (
BP_ANCHOR_PROTO HTMLGetAnchorProtocol(char *anchorVal, size_t len);

// Variable extraction routine, depends on


// Duplicates an anchor list with new memory and pointers (used for attacks)
P_BP_HTML_ANCHOR_LIST HTMLDuplicateAnchorVariables(P_BP_HTML_ANCHOR_VAR_LIST vars, P_BP_GARBAGE_COLLECTOR_DATA gc);

// Retrieves the number of anchor variables from an anchor var list (walks)
size_t HTMLGetAnchorVarCount(P_BP_HTML_ANCHOR_VAR_LIST vars);

// Counts the elements in a set of inputs
size_t HTMLGetInputCount(P_BP_HTML_INPUT_LIST inputs);

// == Visualizers ===

// Anchor Structure Printing Algorithms
void HTMLPrintAnchorVarList(P_BP_HTML_ANCHOR_VAR_LIST inList);

// prints full anchor struct with vars
void HTMLPrintAnchors(P_BP_HTML_ANCHOR_LIST anchorFirst, size_t display_blacklisted);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Form Related Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// == Parsers ==

P_BP_HTML_FORM_LIST HTMLParseForms(xmlNode *node, P_BP_HTML_FULL_EXTRACTION extraction);
P_BP_HTML_FORM_LIST HTMLParseFormsRecurse(xmlNode *node, P_BP_HTML_FORM_LIST appendList, P_BP_HTML_FULL_EXTRACTION extraction);


// == Destructors ==

// destroy a form list and its child input list
int HTMLDestroyFormList(P_BP_HTML_FORM_LIST formList, P_BP_HTML_FULL_EXTRACTION extraction);

// destroy an input list and sub properties list
int HTMLDestroyFormInputList(P_BP_HTML_INPUT_LIST inputList, P_BP_HTML_FULL_EXTRACTION extraction);

// destroy an input lists, properties sub-list
int HTMLDestroyFormInputPropertiesList(P_BP_HTML_INPUT_PROPERTY_LIST propList, P_BP_HTML_FULL_EXTRACTION extraction);

// destroys the properties list internal to a form element (not input based at all, 100% form)
int HTMLDestroyFormPropertiesList(P_BP_HTML_FORM_PROPERTY_LIST inList, P_BP_HTML_FULL_EXTRACTION extraction);

// == Visualizers ==

// Anchor Structure Printing Algorithms
void HTMLPrintFormPropertyList( P_BP_HTML_FORM_PROPERTY_LIST properties);

// Prints out form inputs and properties
void HTMLPrintFormInputList(P_BP_HTML_INPUT_LIST inputs);

// Prints out the input properties associated with the form
void HTMLPrintFormInputPropertyList(P_BP_HTML_INPUT_PROPERTY_LIST properties);

// prints full anchor struct with vars
void HTMLPrintForms(P_BP_HTML_FORM_LIST formFirst, size_t display_blacklisted);

// == Get-ers ==

// Gets a input property by name
P_BP_HTML_INPUT_PROPERTY_LIST HTMLGetInputPropertyByName(P_BP_HTML_INPUT_PROPERTY_LIST properties, char *name);

// Gets a input property by name (case insensitive)
P_BP_HTML_INPUT_PROPERTY_LIST HTMLGetInputPropertyByNameI(P_BP_HTML_INPUT_PROPERTY_LIST properties, char *name);



// Gets a form property by name
P_BP_HTML_FORM_PROPERTY_LIST HTMLGetFormPropertyByName(P_BP_HTML_FORM_PROPERTY_LIST properties, char *name);

// Gets a form property by name (case insensitive)
P_BP_HTML_FORM_PROPERTY_LIST HTMLGetFormPropertyByNameI(P_BP_HTML_FORM_PROPERTY_LIST properties, char *name);


// Gets a property by name
P_BP_HTML_ANCHOR_VAR_LIST HTMLGetAnchorVarByNameI(P_BP_HTML_ANCHOR_VAR_LIST vars, char *name);

// Gets a anchor var by value
P_BP_HTML_ANCHOR_VAR_LIST HTMLGetAnchorVarByValueI(P_BP_HTML_ANCHOR_VAR_LIST vars, char *value);

// Gets a anchor var by position
P_BP_HTML_ANCHOR_VAR_LIST HTMLGetAnchorVarByPosition(P_BP_HTML_ANCHOR_VAR_LIST vars, size_t position);


// Returns a form method based around an input string
BP_HTML_FORM_METHOD HTMLGetFormMethodFromString(char *methodStr);

// Print all element names via a given node
void HTMLPrintElementNamesAll(xmlNode * node);


// Retrieves the number of form variables found in a form list
size_t HTMLGetFormVarCount(P_BP_HTML_FORM_LIST vars);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Secondary Data Extractors %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// parse out secondary data
BP_ERROR_T HTMLParseFormSecondaryData(P_BP_HTML_FORM_LIST forms, P_BP_HTML_FULL_EXTRACTION extraction);

// parse out secondary data for anchors
BP_ERROR_T HTMLParseAnchorsSecondaryData(P_BP_HTML_ANCHOR_LIST anchors, P_BP_HTML_FULL_EXTRACTION extraction);

// ---------------------------------------------------------------------------
// BEGIN DECONSTRUCTORS:

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Clean/Destroy Extraction %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// destroy a delimiter set
int HTMLDestroyAnchorDelimiters(P_BP_HTML_ANCHOR_DELIMITERS delims, P_BP_GARBAGE_COLLECTOR_DATA gc);



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Blacklisting %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Removes elements from anchor/form target results based on blacklisting

// Sets the blacklist property in the anchor list to true for each matching entry in the blacklist_urls array.
BP_ERROR_T HTMLApplyAnchorBlacklist(P_BP_HTML_ANCHOR_LIST anchors, char **blacklist_urls, size_t entry_count);

// Sets the blacklist property in the form list to true for each matching entry in the blacklist_urls array.
BP_ERROR_T HTMLApplyFormBlacklist(P_BP_HTML_FORM_LIST forms, char **blacklist_urls, size_t entry_count);

// Apply a blacklist to a full extraction, replaces the anchors and form list pointers with sanitized
// ones, and destroy the old ones.
BP_ERROR_T HTMLApplyBlacklist(P_BP_HTML_FULL_EXTRACTION full_extraction, char **blacklist_urls, size_t entry_count);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Utility Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Get a null terminated char ** from an anchor list, set use_blacklisted to true to display
// all hosts, regardless of if they've been blacklisted.
char ** HTMLGetUrlStringArrayFromAnchors(P_BP_HTML_ANCHOR_LIST anchors, size_t use_blacklisted, P_BP_GARBAGE_COLLECTOR_DATA gc);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Substitution String Generation %%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This routine generates an anchor substitution string.
char * HTMLGenerateAnchorSubstitutionString
(
	P_BP_HTML_ANCHOR_LIST     anchor,
	P_BP_HTML_ANCHOR_VAR_LIST anchor_vars,
	char *                    sub_var_name,
	char *                    replace_var_name,
	char *                    replace_var_value,
	BP_BOOL                   generate_relative_path,
	P_BP_LINKL                tq_in
);




#endif /* __BP_HTMLDECODER_H */

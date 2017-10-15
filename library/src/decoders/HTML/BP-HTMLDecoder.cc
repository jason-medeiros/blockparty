#include "../../../include/BP-Main.h"

#include <string.h>


// Entry version and recursive version of the parse anchors function.  If
// unsure, use HTMLParseAnchors with a root xmlNode.  The HTMLParseAnchors
// function will automatically recurse where needed.
P_BP_HTML_ANCHOR_LIST HTMLParseAnchors
(
		xmlNode *                   node,
		P_BP_HTML_ANCHOR_DELIMITERS delims,
		P_BP_GARBAGE_COLLECTOR_DATA gc,
		P_BP_HTML_FULL_EXTRACTION extraction
)
{

	// Dummy comparison funciton used in order to verify struct content
	BP_HTML_ANCHOR_LIST dummyCmp;
	BP_ZERO_STACK(dummyCmp);

	// callos new anchor list element
	P_BP_HTML_ANCHOR_LIST appendList = (P_BP_HTML_ANCHOR_LIST) BP_calloc(gc, NULL, sizeof(BP_HTML_ANCHOR_LIST), 1);

	// list pointers
	appendList->first = appendList;
	appendList->prev = NULL;
	appendList->next = NULL;

	// run recursive parse
	P_BP_HTML_ANCHOR_LIST retAnchors = HTMLParseAnchorsRecurse(node, delims, appendList, true, gc, extraction);

	// log the chunk in the list
	BP_GarbageCollectorLogChunk(gc, ".", (void *) retAnchors, 4, BP_GARBAGE_COLLECTOR_DATA_ALLOC_CALLOC);

	// return if we have a good pointer
	if(retAnchors != NULL)
	{
		return retAnchors->first;
	}

	// return null on failure
	return NULL;
}

// Recursive version of the HTMLParseAnchors function, used to recursively rip out
// anchor meat and move it into the anchor list structures.
P_BP_HTML_ANCHOR_LIST HTMLParseAnchorsRecurse
(
		xmlNode *node,
		P_BP_HTML_ANCHOR_DELIMITERS delims,
		P_BP_HTML_ANCHOR_LIST appendList,
		bool isFirst,
		P_BP_GARBAGE_COLLECTOR_DATA gc,
		P_BP_HTML_FULL_EXTRACTION extraction
)
{


	if(node == NULL)
		return NULL;

	xmlNode *cur_node = NULL;
	struct _xmlAttr *inputNode = NULL;

	// Unsigned counters used to find variables
	size_t i = 0;
	size_t x = 0;


	// Examine all nodes in a xml node tree and find all rooted anchor
	// tags.
	for
	(
		cur_node = node;
		cur_node != NULL;
		cur_node = cur_node->next
	)
	{

		// verify that the node is an element node
		if(cur_node->type == XML_ELEMENT_NODE)
		{

			// Make sure that the tag is represented properly as an anchor, also
			// verify that the anchor tag is only one character long.
			if(cur_node->name != NULL)
			if
			(
					( (char)cur_node->name[0]== 'a' || (char)cur_node->name[0] == 'A')	&&
					strnlen((char *) cur_node->name, 10) == 1
			)
			{


				// Now find the href property of the anchor
				for
				(
						inputNode = cur_node->properties;
						inputNode != NULL;
						inputNode = inputNode->next
				)
				{

					// Anchor HREF Extraction IF()
					if( strncmp((char *) inputNode->name, "href", 3 ) == 0 )
					{


						// if the input linked list is null allocate and assign first ptr
						if(isFirst != true)
						{

							// allocate new element
							if(appendList->next == NULL)
								appendList->next = (P_BP_HTML_ANCHOR_LIST) BP_calloc(gc, NULL, sizeof(BP_HTML_ANCHOR_LIST), 1);

							// reassign pointers and move list forward
							appendList->next->prev = appendList;
							appendList->next->next = NULL;
							appendList->next->first = appendList->first;
							appendList = appendList->next;

						}

						// Retrieve Anchor Protocol
						if(inputNode->children != NULL)
						{


							// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
							// %%% End Recurse / Add Anchors Here %%%%%%%%%%%%%%%%%%%%%%%%%%%%
							// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

							// This is where the buck stops.  This is where all anchor assignments occur,
							// and where we can run our individual calculations.


							// set the href
							appendList->href  = (char *) BP_strndup(gc, NULL, (char *) inputNode->children->content, bpstrlen((char *) inputNode->children->content));

							// if we have an extraction, we do some extra work to calculate base/path/script/host
							if(extraction)
							{

								// if we have a session reference, we can further refine
								// href generation.
								if(appendList->href)
								if(extraction->http_session_reference)
								{


									// calculate anchor length
									size_t anchor_len = 0;
									anchor_len = bpstrlen(appendList->href);

									// temporary string for joining together string elements
									char *tmp_joined = NULL;

									// create last requested string
									char *last_requested = NULL;


									if(extraction->http_session_reference->lastRequestedHref[bpstrlen(extraction->http_session_reference->lastRequestedHref)-1] != '/')
										last_requested = BP_JoinStrings(extraction->http_session_reference->lastRequestedHref, "/");
									else
										last_requested = bpstrdup(extraction->http_session_reference->lastRequestedHref);


									// printf("\n href:       %s - %s", appendList->href, inputNode->children->content);
									// calculate the base
									char * tmp_base = BP_ParseURLToBasePCRE(appendList->href);
									if(!tmp_base)
										tmp_base = BP_ParseURLToBasePCRE(last_requested);


									char * tmp_path = BP_ParseURLToPathPCRE(appendList->href);
									if(!tmp_path)
										tmp_path = BP_ParseURLToPathPCRE(last_requested);

									if(last_requested && !tmp_path)
									{

									}




									// due to the loose matching on the host matching
									// expression, we can only use the base as input if
									// we want things to appear correctly.
									char * tmp_host   = NULL;
									if(tmp_base)
										tmp_host = BP_ParseURLToHostPCRE(tmp_base);

									// after path is parsed, this will get set with the
									// full URL.
									char *tmp_full_url = NULL;


									// create a join stack for testing things
									char * tmp_join_stack[10];
									BP_ZERO_STACK(tmp_join_stack);

									// if the path couldn't be created in either of both ways, run
									// some checks to try to determine relative path.
									if(!tmp_path)
									{

										// calculate the length of the href
										size_t tmp_path_len = bpstrlen(appendList->href);

										// check to make sure that we're not just pointing
										// to our current directory
										if( tmp_path_len == 0)
										{
											tmp_path     = strdup(tmp_base);
											tmp_full_url = strdup(tmp_base);

										}
										else if( tmp_path_len == 2)
										{


											// if it's a basic path from root, set it here
											if(memcmp(appendList->href, "./", 2) == 0)
											{
												if(tmp_base)
												{
													tmp_path     = strdup(tmp_base);
													tmp_full_url = strdup(tmp_base);
												}
											}


										}

										// if the tmp path hasn't been set yet, set it
										// forcibly here
										if(tmp_path_len && !tmp_path)
										{


											// check for relative path and attempt construction
											// if possible.
											if(tmp_path_len >= 2)
											{
												if
												(
													(appendList->href[0] == '.') &&
													(appendList->href[1] == '/')
												)
												{


													// destroy joiner string if required
													if(tmp_joined)
													{
														bpfree(tmp_joined);
														tmp_joined = NULL;
													}

													// set join elements
													tmp_join_stack[0] = tmp_base;
													tmp_join_stack[1] = (char *) &appendList->href[2];

													// attempt to join the string
													tmp_joined = BP_JoinStringArray((char **) &tmp_join_stack, 2);

													// set full url here
													tmp_full_url = bpstrdup(tmp_joined);

													// parse full url to path
													tmp_path = BP_ParseURLToPathPCRE(tmp_joined);
													if(!tmp_path)
														tmp_path = strdup(tmp_base);



												}
												else if
												(
													(appendList->href[0] == '/')
												)
												{

													// set join elements
													tmp_join_stack[0] = tmp_base;
													tmp_join_stack[1] = (char *) &appendList->href[1];

													// attempt to join the string
													tmp_joined = BP_JoinStringArray((char **) &tmp_join_stack, 2);

													// set full url here
													tmp_full_url = bpstrdup(tmp_joined);

													// parse full url to path
													tmp_path = BP_ParseURLToPathPCRE(tmp_joined);
													if(!tmp_path)
														tmp_path = strdup(tmp_base);
												}


											}


											// destroy joiner string if required
											if(tmp_joined)
											{
												bpfree(tmp_joined);
												tmp_joined = NULL;
											}

											// if it still hasn't been set, run the last test/set
											if(!tmp_path)
											{

												// destroy joiner string if required
												if(tmp_joined)
												{
													bpfree(tmp_joined);
													tmp_joined = NULL;
												}


												// set join elements
												tmp_join_stack[0] = tmp_base;
												if(appendList->href[0] == '/')
													tmp_join_stack[1] = (char *) &appendList->href[1];
												else
													tmp_join_stack[1] = (char *) appendList->href;

												// attempt to join the string
												tmp_joined = BP_JoinStringArray((char **) &tmp_join_stack, 2);

												// set full url here
												tmp_full_url = bpstrdup(tmp_joined);


												tmp_path = BP_ParseURLToPathPCRE(tmp_joined);
												if(!tmp_path)
													tmp_path = strdup(tmp_base);


											}

											// destroy joiner string if required
											if(tmp_joined)
											{
												bpfree(tmp_joined);
												tmp_joined = NULL;
											}

										}

									}

									// build full url
									char * tmp_check_path = BP_ParseURLToPathPCRE(appendList->href);
									char * join_position = NULL;
									if(!tmp_check_path)
									{

										if(appendList->href[0] == '/')
											join_position = &appendList->href[1];
										else if (appendList->href[0] == '.' && appendList->href[1] == '/')
											join_position = &appendList->href[2];
										else
											join_position = appendList->href;

										tmp_full_url = BP_JoinStrings(tmp_path, join_position);

										// destroy tmp path before recreating path
										if(tmp_path)
											bpfree(tmp_path);

										// reparse the path since we have a new url to parse from
										tmp_path = BP_ParseURLToPathPCRE(tmp_full_url);


									}
									else
									{

										// printf("\n It's not relative: %s - %s", appendList->href, tmp_path);
										size_t href_len     = bpstrlen(appendList->href);
										size_t tmp_path_len = bpstrlen(tmp_path);

										// check to see if the path and the href are identical
										if(href_len == tmp_path_len)
										if(memcmp(appendList->href, tmp_path, tmp_path_len) == 00)
											tmp_full_url = strdup(appendList->href);

										if(!tmp_full_url)
											tmp_full_url = strdup(appendList->href);

										// destroy the check path
										bpfree(tmp_check_path);

									}



									// now calculate script, since we've got the full url at
									// this point.
									char * tmp_script = NULL;

									// parse the full url as the script
									tmp_script = BP_ParseURLToScriptPCRE(tmp_full_url);

									// set the protocol
									appendList->proto = HTMLGetAnchorProtocol((char *) tmp_full_url, strnlen((char *) tmp_full_url, 10000));


									// Set values in anchor element by duplicating them
									// via the tailqueue allocator.
									appendList->host     = tq_strdup(extraction->tq, tmp_host, BPLN);
									appendList->base     = tq_strdup(extraction->tq, tmp_base, BPLN);
									appendList->path     = tq_strdup(extraction->tq, tmp_path, BPLN);
									appendList->script   = tq_strdup(extraction->tq, tmp_script, BPLN);
									appendList->full_url = tq_strdup(extraction->tq, tmp_full_url, BPLN);

									// Uncomment this if you need to debug.
									/*
									printf("\n anchor host:     %s", appendList->host);
									printf("\n anchor base:     %s", appendList->base);
									printf("\n anchor path:     %s", appendList->path);
									printf("\n anchor script:   %s", appendList->script);
									printf("\n anchor full_url: %s", appendList->full_url);
									*/

									// destroy temporary values
									if(tmp_host)
										bpfree(tmp_host);
									if(tmp_base)
										bpfree(tmp_base);
									if(tmp_path)
										bpfree(tmp_path);
									if(tmp_script)
										bpfree(tmp_script);
									if(tmp_full_url)
										bpfree(tmp_full_url);
									if(last_requested)
										bpfree(last_requested);

									// nullify all potentially freed items
									tmp_host = NULL;
									tmp_base = NULL;
									tmp_path = NULL;
									tmp_script = NULL;
									tmp_full_url = NULL;
									last_requested = NULL;


								}

							}

						}


						/*
						 * Now retrive and store linked list elements in order to create a
						 * proper list chain.
						 */

						// assign the delimiter pointer
						appendList->delims = delims;

						// NON-HEAP STRING RETURNED HERE / DO NOT FREE.
						// get protocol string
						appendList->protoStr = HTMLGetAnchorProtocolStringFromVal(appendList->proto);

						// retrieve variables from embedded href
						if(inputNode->children != NULL)
							appendList->vars = HTMLGetAnchorEmbeddedVariables((char *) inputNode->children->content, delims, strnlen((char *) inputNode->children->content, 1000), gc);

						// log the chunk
						BP_GarbageCollectorLogChunk(gc, ".", (void *) appendList->vars, 4, BP_GARBAGE_COLLECTOR_DATA_ALLOC_CALLOC);

						// attempt to find base url using delimiter set and variable accounts as provisions
						if(appendList->vars)
						{

							// walk each character in the url untill the start delimiter is found, when it is, allocate space
							// for the base url.
							for
							(
								i = 0;
								i < bpstrlen((char *) inputNode->children->content);
								i++
							)
							{
							if(memcmp(&inputNode->children->content[i], appendList->delims->startDelimiter, bpstrlen(appendList->delims->startDelimiter)) == 0)
							{

									// free data on loop if necessary
									if(appendList->relativePath)
									{
										BP_free(gc, ".", (void *) appendList->relativePath);
										appendList->relativePath = NULL;
									}

									// copy in data
									appendList->relativePath = BP_ParseURLToScript((char *) inputNode->children->content, 1, 1);
									if(appendList->relativePath)
									{
										BP_GarbageCollectorLogChunk(gc, ".", appendList->relativePath, bpstrlen(appendList->relativePath), BP_GARBAGE_COLLECTOR_DATA_ALLOC_UNKNOWN);
									}

									// printf("\n Using relative path: %s", appendList->relativePath);

								}
							}
						}
					}
				}
			}
		}

		// move list to the last element before passing it off
		for
		(
			appendList = appendList->first;
			appendList->next;
			appendList = appendList->next
		){}

		// create entry for chunk
		P_BP_HTML_ANCHOR_LIST al = HTMLParseAnchorsRecurse(cur_node->children, delims, appendList, false, gc, extraction);

		// log chunk
		BP_GarbageCollectorLogChunk(gc, ".", al, 4, BP_GARBAGE_COLLECTOR_DATA_ALLOC_CALLOC);

	}

	// return the list first elment provided the list is not null
	if(appendList != NULL)
	if(appendList->first != NULL)
		return appendList;

	// default error return
	return NULL;
}

// Retrieves a value that correlates with the BP_ANCHOR_PROTO enum.  Default
// indicates that there is no protocol associated with the anchor (
BP_ANCHOR_PROTO HTMLGetAnchorProtocol(char *anchorVal, size_t len){

	// anchor protocols must be longer
	if(!len){
		return BP_ANCHOR_PROTO_DEFAULT;
	}

	// base relative link
	if(anchorVal[0] == '/')
		return BP_ANCHOR_PROTO_BASE_RELATIVE_LINK;

	// relative lik
	if(anchorVal[0] == '.' && anchorVal[1] == '/')
		return BP_ANCHOR_PROTO_DOT_SLASH_RELATIVE_LINK;

	// Decoders,
	// ACAP, ADDBOOK, AFP, AFS, AIM
	if(anchorVal[0] == 'a' || anchorVal[0] == 'A'){

		// acap
		if(len >= bpstrlen("acap://"))
		{
			if(BP_strncasecmp(anchorVal, "acap://", bpstrlen("acap://")) == 0)
			{
				return BP_ANCHOR_PROTO_ACAP;
			}
		}

		if(len >= bpstrlen("addbook://"))
		{
			if(BP_strncasecmp(anchorVal, "addbook://", bpstrlen("addbook://")) == 0)
			{
				return BP_ANCHOR_PROTO_ADDBOOK;
			}
		}

		if(len >= bpstrlen("afp://"))
		{
			if(BP_strncasecmp(anchorVal, (char *) "afp://", bpstrlen("afp://")) == 0)
			{
				return BP_ANCHOR_PROTO_AFP;
			}
		}

		if(len >= bpstrlen("afs://"))
		{
			if(BP_strncasecmp(anchorVal, (char *) "afs://", bpstrlen("afs://")) == 0)
			{
				return BP_ANCHOR_PROTO_AFS;
			}
		}

		if(len >= bpstrlen("aim://"))
		{
			if(BP_strncasecmp(anchorVal, (char *) "aim://", bpstrlen("aim://")) == 0)
			{
				return BP_ANCHOR_PROTO_AIM;
			}
		}

	}

	// c
	if(anchorVal[0] == 'c' || anchorVal[0] == 'C')
	{

		// callto
		if(len >= bpstrlen("callto://"))
		{
			if(BP_strncasecmp(anchorVal, (char *) "callto://", bpstrlen("callto://")) == 0)
			{
				return BP_ANCHOR_PROTO_CALLTO;
			}
		}

		// castanet
		if(len >= bpstrlen("castanet://"))
		if(BP_strncasecmp(anchorVal, (char *) "castanet://", bpstrlen("castanet://")) == 0){
			return BP_ANCHOR_PROTO_CASTANET;
		}

		// cid
		if(len >= bpstrlen("cid://"))
		if(BP_strncasecmp(anchorVal, (char *) "cid://", bpstrlen("cid://")) == 0){
			return BP_ANCHOR_PROTO_CID;
		}


	}

	// data, dav, dict, dns
	if(anchorVal[0] == 'd' || anchorVal[0] == 'D'){

		// data
		if(len >= bpstrlen("data:"))
		if(BP_strncasecmp(anchorVal,(char *)  "data:", bpstrlen("data:")) == 0){
			return BP_ANCHOR_PROTO_DATA;
		}

		// dav
		if(len >= bpstrlen("dav:"))
		if(BP_strncasecmp(anchorVal, (char *) "dav:", bpstrlen("dav:")) == 0){
			return BP_ANCHOR_PROTO_DAV;
		}

		// dict
		if(len >= bpstrlen("dict:"))
		if(BP_strncasecmp(anchorVal, (char *) "dict:", bpstrlen("dict:")) == 0){
			return BP_ANCHOR_PROTO_DICT;
		}

		// dns
		if(len >= bpstrlen("dns:"))
		if(BP_strncasecmp(anchorVal, (char *) "dns:", bpstrlen("dns:")) == 0){
			return BP_ANCHOR_PROTO_DNS;
		}

	}

	// EID
	if(anchorVal[0] == 'e' || anchorVal[0] == 'E'){

		// eid
		if(len >= bpstrlen("eid:"))
		if(BP_strncasecmp(anchorVal, (char *) "eid:", bpstrlen("eid:")) == 0){
			return BP_ANCHOR_PROTO_EID;
		}

	}


	// FAX, FILE, FINGER, FREENET,FTP, FTPS
	if(anchorVal[0] == 'f' || anchorVal[0] == 'F'){

		// fax
		if(len >= bpstrlen("fax:"))
		if(BP_strncasecmp(anchorVal, (char *) "fax:", bpstrlen("fax:")) == 0){
			return BP_ANCHOR_PROTO_FAX;
		}

		// file
		if(len >= bpstrlen("file:"))
		if(BP_strncasecmp(anchorVal, (char *) "file:", bpstrlen("file:")) == 0){
			return BP_ANCHOR_PROTO_FILE;
		}

		// finger
		if(len >= bpstrlen("finger:"))
		if(BP_strncasecmp(anchorVal, (char *) "finger:", bpstrlen("finger:")) == 0){
			return BP_ANCHOR_PROTO_FINGER;
		}

		// bpfreenet
		if(len >= bpstrlen("freenet:"))
		if(BP_strncasecmp(anchorVal, (char *) "freenet:", bpstrlen("freenet:")) == 0){
			return BP_ANCHOR_PROTO_FREENET;
		}

		// ftp
		if(len >= bpstrlen("ftp:"))
		if(BP_strncasecmp(anchorVal, (char *) "ftp:", bpstrlen("ftp:")) == 0){
			return BP_ANCHOR_PROTO_FTP;
		}

		// ftps
		if(len >= bpstrlen("ftps:"))
		if(BP_strncasecmp(anchorVal, (char *) "ftps:", bpstrlen("ftps:")) == 0){
			return BP_ANCHOR_PROTO_FTPS;
		}

	}

	// GOPHER, GSMSMS
	if(anchorVal[0] == 'g' || anchorVal[0] == 'G'){

		// gopher
		if(len >= bpstrlen("gopher:"))
		if(BP_strncasecmp(anchorVal, (char *) "gopher:", bpstrlen("gopher:")) == 0){
			return BP_ANCHOR_PROTO_GOPHER;
		}

		// gsm-sms
		if(len >= bpstrlen("gsm-sms:"))
		if(BP_strncasecmp(anchorVal, (char *) "gsm-sms:", bpstrlen("gsm-sms:")) == 0){
			return BP_ANCHOR_PROTO_GSM_SMS;
		}

	}

	// h323, h324, hdl, hnews, http, https
	if(anchorVal[0] == 'h' || anchorVal[0] == 'H'){

		// h323:
		if(len >= bpstrlen("h323:"))
		if(BP_strncasecmp(anchorVal, (char *) "h323:", bpstrlen("h323:")) == 0){
			return BP_ANCHOR_PROTO_H323;
		}

		// h324
		if(len >= bpstrlen("h324:"))
		if(BP_strncasecmp(anchorVal, (char *) "h324:", bpstrlen("h324:")) == 0){
			return BP_ANCHOR_PROTO_H324;
		}

		// http:
		if(len >= bpstrlen("http:"))
		if(BP_strncasecmp(anchorVal, (char *) "http:", bpstrlen("http:")) == 0){
			return BP_ANCHOR_PROTO_HTTP;
		}

		// https
		if(len >= bpstrlen("https:"))
		if(BP_strncasecmp(anchorVal, (char *) "https:", bpstrlen("https:")) == 0){
			return BP_ANCHOR_PROTO_HTTPS;
		}


	}

	// IIOPLOC, ILU, IOR, IMAP, IPP, IRC
	if(anchorVal[0] == 'i' || anchorVal[0] == 'I'){
		// IIOPLOC
		if(len >= bpstrlen("iioploc:"))
		if(BP_strncasecmp(anchorVal, (char *) "iioploc:", bpstrlen("iioploc:")) == 0){
			return BP_ANCHOR_PROTO_IIOPLOC;
		}

		// ilu
		if(len >= bpstrlen("ilu:"))
		if(BP_strncasecmp(anchorVal, (char *) "ilu:", bpstrlen("ilu:")) == 0){
			return BP_ANCHOR_PROTO_ILU;
		}

		// IOR
		if(len >= bpstrlen("ior:"))
		if(BP_strncasecmp(anchorVal, (char *) "ior:", bpstrlen("ior:")) == 0){
			return BP_ANCHOR_PROTO_IOR;
		}

		// IMAP
		if(len >= bpstrlen("imap:"))
		if(BP_strncasecmp(anchorVal, (char *) "imap:", bpstrlen("imap:")) == 0){
			return BP_ANCHOR_PROTO_IMAP;
		}

		// IPP
		if(len >= bpstrlen("ipp:"))
		if(BP_strncasecmp(anchorVal, (char *) "ipp:", bpstrlen("ipp:")) == 0){
			return BP_ANCHOR_PROTO_IPP;
		}

		// IRC
		if(len >= bpstrlen("irc:"))
		if(BP_strncasecmp(anchorVal, (char *) "ipp:", bpstrlen("irc:")) == 0){
			return BP_ANCHOR_PROTO_IRC;
		}

	}

	// JAR, JAVA, JAVASCRIPT, JDBC
	if(anchorVal[0] == 'j' || anchorVal[0] == 'J'){

		// JAR
		if(len >= bpstrlen("jar:"))
		if(BP_strncasecmp(anchorVal, (char *) "jar:", bpstrlen("jar:")) == 0){
			return BP_ANCHOR_PROTO_IRC;
		}

		// JAVA
		if(len >= bpstrlen("java:"))
		if(BP_strncasecmp(anchorVal, (char *) "java:", bpstrlen("java:")) == 0){
			return BP_ANCHOR_PROTO_JAVA;
		}

		// JAVASCRIPT
		if(len >= bpstrlen("javascript:"))
		if(BP_strncasecmp(anchorVal,(char *)  "javascript:", bpstrlen("javascript:")) == 0){
			return BP_ANCHOR_PROTO_JAVASCRIPT;
		}

		// JDBC
		if(len >= bpstrlen("jdbc:"))
		if(BP_strncasecmp(anchorVal, (char *) "jdbc:", bpstrlen("jdbc:")) == 0){
			return BP_ANCHOR_PROTO_JDBC;
		}
	}

	// LDAP, LIFN, LIVESCRIPT
	if(anchorVal[0] == 'l' || anchorVal[0] == 'L'){

		// ldap
		if(len >= bpstrlen("ldap:"))
		if(BP_strncasecmp(anchorVal,(char *)  "ldap:", bpstrlen("ldap:")) == 0){
			return BP_ANCHOR_PROTO_LDAP;
		}

		// lifn
		if(len >= bpstrlen("lifn:"))
		if(BP_strncasecmp(anchorVal, (char *) "lifn:", bpstrlen("lifn:")) == 0){
			return BP_ANCHOR_PROTO_LIFN;
		}

		// livescript
		if(len >= bpstrlen("livescript:"))
		if(BP_strncasecmp(anchorVal,(char *)  "livescript:", bpstrlen("livescript:")) == 0){
			return BP_ANCHOR_PROTO_LIVESCRIPT;
		}
	}

	// mailto:
	if(anchorVal[0] == 'm' || anchorVal[0] == 'M'){
		// mailto
		if(len >= bpstrlen("mailto:"))
		if(BP_strncasecmp(anchorVal,(char *)  "mailto:", bpstrlen("mailto:")) == 0){
			return BP_ANCHOR_PROTO_MAILTO;
		}

		// mailbox
		if(len >= bpstrlen("mailbox:"))
		if(BP_strncasecmp(anchorVal,(char *)  "mailbox:", bpstrlen("mailbox:")) == 0){
			return BP_ANCHOR_PROTO_MAILBOX;
		}

		// md5
		if(len >= bpstrlen("md5:"))
		if(BP_strncasecmp(anchorVal,(char *)  "md5:", bpstrlen("md5:")) == 0){
			return BP_ANCHOR_PROTO_MD5;
		}

		// mid
		if(len >= bpstrlen("mid:"))
		if(BP_strncasecmp(anchorVal,(char *)  "mid:", bpstrlen("mid:")) == 0){
			return BP_ANCHOR_PROTO_MID;
		}

		// midi
		if(len >= bpstrlen("midi:"))
		if(BP_strncasecmp(anchorVal,(char *)  "midi:", bpstrlen("midi:")) == 0){
			return BP_ANCHOR_PROTO_MIDI;
		}

		// mocha
		if(len >= bpstrlen("mocha:"))
		if(BP_strncasecmp(anchorVal,(char *)  "mocha:", bpstrlen("mocha:")) == 0){
			return BP_ANCHOR_PROTO_MOCHA;
		}

		// modem
		if(len >= bpstrlen("modem:"))
		if(BP_strncasecmp(anchorVal,(char *)  "modem:", bpstrlen("modem:")) == 0){
			return BP_ANCHOR_PROTO_MODEM;
		}

	}

	// NEWS, NFS, NNTP
	if(anchorVal[0] == 'n' || anchorVal[0] == 'N'){
		// news
		if(len >= bpstrlen("news:"))
		if(BP_strncasecmp(anchorVal,(char *)  "news:", bpstrlen("news:")) == 0){
			return BP_ANCHOR_PROTO_NEWS;
		}

		// NFS
		if(len >= bpstrlen("nfs:"))
		if(BP_strncasecmp(anchorVal,(char *)  "nfs:", bpstrlen("nfs:")) == 0){
			return BP_ANCHOR_PROTO_NFS;
		}

		// NNTP
		if(len >= bpstrlen("nntp:"))
		if(BP_strncasecmp(anchorVal,(char *)  "nntp:", bpstrlen("nntp:")) == 0){
			return BP_ANCHOR_PROTO_NNTP;
		}

	}

	// Opaquelocktoken
	if(anchorVal[0] == 'o' || anchorVal[0] == 'O'){
		// opaquelocktoken
		if(len >= bpstrlen("opaquelocktoken:"))
		if(BP_strncasecmp(anchorVal,(char *)  "opaquelocktoken:", bpstrlen("opaquelocktoken:")) == 0){
			return BP_ANCHOR_PROTO_OPAQUELOCKTOKEN;
		}
	}

	// PHONE, POP, POP3, PRINTER, PROSPERO
	if(anchorVal[0] == 'p' || anchorVal[0] == 'P'){

		// PHONE
		if(len >= bpstrlen("PHONE:"))
		if(BP_strncasecmp(anchorVal,(char *)  "PHONE:", bpstrlen("PHONE:")) == 0){
			return BP_ANCHOR_PROTO_PHONE;
		}

		// POP
		if(len >= bpstrlen("POP:"))
		if(BP_strncasecmp(anchorVal,(char *)  "POP:", bpstrlen("POP:")) == 0){
			return BP_ANCHOR_PROTO_POP;
		}


		// POP3
		if(len >= bpstrlen("POP3:"))
		if(BP_strncasecmp(anchorVal,(char *)  "POP:", bpstrlen("POP3:")) == 0){
			return BP_ANCHOR_PROTO_POP3;
		}

		// PRINTER
		if(len >= bpstrlen("PRINTER:"))
		if(BP_strncasecmp(anchorVal,(char *)  "PRINTER:", bpstrlen("PRINTER:")) == 0){
			return BP_ANCHOR_PROTO_PRINTER;
		}

		// PROSPERO
		if(len >= bpstrlen("PROSPERO:"))
		if(BP_strncasecmp(anchorVal,(char *)  "PROSPERO:", bpstrlen("PROSPERO:")) == 0){
			return BP_ANCHOR_PROTO_PROSPERO;
		}

		// RES
		if(len >= bpstrlen("RES:"))
		if(BP_strncasecmp(anchorVal,(char *)  "RES:", bpstrlen("RES:")) == 0){
			return BP_ANCHOR_PROTO_RES;
		}

	}

	// RES, RSTP, RVP, RWHOIS, RX
	if(anchorVal[0] == 'r' || anchorVal[0] == 'R'){
		// RES
		if(len >= bpstrlen("res:"))
		if(BP_strncasecmp(anchorVal,(char *)  "res:", bpstrlen("res:")) == 0){
			return BP_ANCHOR_PROTO_RES;
		}

		// RSTP
		if(len >= bpstrlen("rvp:"))
		if(BP_strncasecmp(anchorVal,(char *)  "rwhois:", bpstrlen("rwhois:")) == 0){
			return BP_ANCHOR_PROTO_RWHOIS;
		}

		// RX
		if(len >= bpstrlen("rx:"))
		if(BP_strncasecmp(anchorVal,(char *)  "rx:", bpstrlen("rx:")) == 0){
			return BP_ANCHOR_PROTO_RX;
		}
	}

	// SDP, SERVICE, SIP, SMB, SNEWS, SOAP.BEEP, SOAP.BEEPS
	if(anchorVal[0] == 's' || anchorVal[0] == 'S'){
		// SDP
		if(len >= bpstrlen("sdp:"))
		if(BP_strncasecmp(anchorVal,(char *)  "sdp:", bpstrlen("sdp:")) == 0){
			return BP_ANCHOR_PROTO_SDP;
		}

		// SERVICE
		if(len >= bpstrlen("service:"))
		if(BP_strncasecmp(anchorVal,(char *)  "service:", bpstrlen("service:")) == 0){
			return BP_ANCHOR_PROTO_SERVICE;
		}

		// SIP
		if(len >= bpstrlen("sip:"))
		if(BP_strncasecmp(anchorVal,(char *)  "sip:", bpstrlen("sip:")) == 0){
			return BP_ANCHOR_PROTO_SIP;
		}

		// SMB
		if(len >= bpstrlen("smb:"))
		if(BP_strncasecmp(anchorVal,(char *)  "smb:", bpstrlen("smbs:")) == 0){
			return BP_ANCHOR_PROTO_SMB;
		}

		// SNEWS
		if(len >= bpstrlen("snews:"))
		if(BP_strncasecmp(anchorVal,(char *)  "snews:", bpstrlen("snews:")) == 0){
			return BP_ANCHOR_PROTO_SNEWS;
		}

		// SOAP BEEP
		if(len >= bpstrlen("soap.beep:"))
		if(BP_strncasecmp(anchorVal,(char *)  "soap.beep:", bpstrlen("rx:")) == 0){
			return BP_ANCHOR_PROTO_SOAP_BEEP;
		}

		// SOAP.BEEPS
		if(len >= bpstrlen("soap.beeps:"))
		if(BP_strncasecmp(anchorVal, (char *) "soap.beeps:", bpstrlen("soap.beeps:")) == 0){
			return BP_ANCHOR_PROTO_SOAP_BEEPS;
		}

	}


	// T120, TCP, TEL, TELNET, TIP, TN3270, TV
	if(anchorVal[0] == 't' || anchorVal[0] == 'T')
	{

		// T120
		if(len >= bpstrlen("t120:"))
		if(BP_strncasecmp(anchorVal, "t120:", bpstrlen("t120:")) == 0){
			return BP_ANCHOR_PROTO_T120;
		}

		// TCP
		if(len >= bpstrlen("tcp:"))
		if(BP_strncasecmp(anchorVal, "tcp:", bpstrlen("tcp:")) == 0){
			return BP_ANCHOR_PROTO_TCP;
		}

		// TEL
		if(len >= bpstrlen("tel:"))
		if(BP_strncasecmp(anchorVal, "tel:", bpstrlen("tel:")) == 0){
			return BP_ANCHOR_PROTO_TEL;
		}

		// telnet
		if(len >= bpstrlen("telnet:"))
		if(BP_strncasecmp(anchorVal, "telnet:", bpstrlen("telnet:")) == 0){
			return BP_ANCHOR_PROTO_TELNET;
		}

		// TIP
		if(len >= bpstrlen("tip:"))
		if(BP_strncasecmp(anchorVal, "tip:", bpstrlen("tip:")) == 0){
			return BP_ANCHOR_PROTO_TIP;
		}

		// TN3270
		if(len >= bpstrlen("tn3270:"))
		if(BP_strncasecmp(anchorVal, "tn3270:", bpstrlen("tn3270:")) == 0){
			return BP_ANCHOR_PROTO_TN3270;
		}

		// TV
		if(len >= bpstrlen("tv:"))
		if(BP_strncasecmp(anchorVal, "tv:", bpstrlen("tv:")) == 0){
			return BP_ANCHOR_PROTO_TV;
		}


	}

	// URN, UUID,
	if(anchorVal[0] == 'u' || anchorVal[0] == 'U')
	{

		// URN
		if(len >= bpstrlen("urn:"))
		if(BP_strncasecmp(anchorVal, "urn:", bpstrlen("urn:")) == 0){
			return BP_ANCHOR_PROTO_URN;
		}

		// UUID
		if(len >= bpstrlen("uuid:"))
		if(BP_strncasecmp(anchorVal, "uuid:", bpstrlen("uuid:")) == 0){
			return BP_ANCHOR_PROTO_UUID;
		}

	}

	// VEMMI, VIDEOTEX, VIEW_SOURCE
	if(anchorVal[0] == 'v' || anchorVal[0] == 'V')
	{

		// VEMMI
		if(len >= bpstrlen("vemmi:"))
		if(BP_strncasecmp(anchorVal, "vemmi:", bpstrlen("vemmi:")) == 0){
			return BP_ANCHOR_PROTO_VEMMI;
		}


		// VIDEOTEX
		if(len >= bpstrlen("videotex:"))
		if(BP_strncasecmp(anchorVal, "videotex:", bpstrlen("videotex:")) == 0){
			return BP_ANCHOR_PROTO_VIDEOTEX;
		}

		// VIEW_SOURCE
		if(len >= bpstrlen("view_source:"))
		if(BP_strncasecmp(anchorVal, "view_source:", bpstrlen("view_source:")) == 0){
			return BP_ANCHOR_PROTO_VIEW_SOURCE;
		}

	}


	// WAIS, WHOOP, WHOIS
	if(anchorVal[0] == 'w' || anchorVal[0] == 'W'){

		// WAIS
		if(len >= bpstrlen("wais:"))
		if(BP_strncasecmp(anchorVal, "wais:", bpstrlen("wais:")) == 0){
			return BP_ANCHOR_PROTO_WAIS;
		}


		// WHOOP
		if(len >= bpstrlen("whodp:"))
		if(BP_strncasecmp(anchorVal, "whodp:", bpstrlen("whodp:")) == 0){
			return BP_ANCHOR_PROTO_WHODP;
		}

		// WHOIS
		if(len >= bpstrlen("whois:"))
		if(BP_strncasecmp(anchorVal, "whois:", bpstrlen("whois:")) == 0){
			return BP_ANCHOR_PROTO_WHOIS;
		}
	}

	// Z39_50R, Z39_50S
	if(anchorVal[0] == 'z' || anchorVal[0] == 'z')
	{
		// Z39_50R
		if(len >= bpstrlen("z39.50r:"))
		if(BP_strncasecmp(anchorVal, "z39.50r:", bpstrlen("z39.50r:")) == 0){
			return BP_ANCHOR_PROTO_Z39_50R;
		}

		// Z39_50S
			if(len >= bpstrlen("z39.50s:"))
			if(BP_strncasecmp(anchorVal, "z39.50s:", bpstrlen("z39.50s:")) == 0){
				return BP_ANCHOR_PROTO_Z39_50S;
			}
	}

	// If the anchor  has a length, and we haven't returned yet, its most likely a direct
	// relative link in the form of blah.html, or index.php.  No forward ./ characters in the
	// front of this relative site layout.
	if(strnlen(anchorVal, 10))
	{
		return BP_ANCHOR_PROTO_RELATIVE_LINK;
	}

	// return unknown protocol
	return BP_ANCHOR_PROTO_UNKNOWN;

}

// blockparty non OS specific-case indifferent string compare
int BP_strncasecmp(char *in1, char *in2, size_t len){

	#ifndef _WIN32
		return strncasecmp(in1, in2, len);
	#endif

	return 0;
}

// Prints out an entire anchor variable list chain
void HTMLPrintAnchorVarList(P_BP_HTML_ANCHOR_VAR_LIST inList){

	// default null check
	if(inList == NULL)
		return;

	printf("\n\t[+] Printing Anchor Var-List: %p", inList);

	// iterate through the list printing out elements
	for(;inList != NULL; inList=inList->next){

		printf("\n\t  List Element:  %p", inList);
		printf("\n\t\t list->name:       %s", inList->name);
		printf("\n\t\t list->nameLen:    %i", inList->nameLen);
		printf("\n\t\t list->content:    %s", inList->content);
		printf("\n\t\t list->contentLen: %i", inList->contentLen);
		printf("\n\t\t\t listptrs:(f) %p, (p) %p, (n) %p", inList->first, inList->prev, inList->next);
		printf("\n");

	}
	// void type return EAX
}

void HTMLPrintAnchors(P_BP_HTML_ANCHOR_LIST anchorFirst, size_t display_blacklisted){

	// default null check
	if(anchorFirst == NULL)
		return;

	// iterate through the anchor list printing out variables
	for(anchorFirst = anchorFirst->first; anchorFirst != NULL; anchorFirst = anchorFirst->next){

			if(anchorFirst->blacklisted && !display_blacklisted)
				continue;
		printf("\n [+] Anchor Struct: %p ", anchorFirst);
		printf("\n    blacklisted: %s", anchorFirst->blacklisted ? "Yes" : "No");
		printf("\n\t  href:       %s", anchorFirst->href);
		printf("\n\t  host:       %s", anchorFirst->host);
		printf("\n\t  base:       %s", anchorFirst->base);
		printf("\n\t  path:       %s", anchorFirst->path);
		printf("\n\t  script:     %s", anchorFirst->script);
		printf("\n\t  full_url:   %s", anchorFirst->full_url);
		printf("\n\t  delimiters: %p", anchorFirst->delims);
		printf("\n\t  protocol:   %s", anchorFirst->protoStr);

		if(anchorFirst->vars != NULL){
			HTMLPrintAnchorVarList(anchorFirst->vars->first);
		}
		printf("\n");
	}

	printf("\n");

	// void type return EAX
	return;
}

// Retrieves variables embedded in anchors using a delimiter set
P_BP_HTML_ANCHOR_VAR_LIST HTMLGetAnchorEmbeddedVariables(char *href, P_BP_HTML_ANCHOR_DELIMITERS delims, size_t len, P_BP_GARBAGE_COLLECTOR_DATA gc)
{

	// default null checks
	if(delims == NULL) return NULL;
	if(href == NULL) return NULL;

	// get string length (MAX 10K CHARS)
	size_t hrefLen = strnlen(href, 10000);

	// null terminate at 10000 if not terminated (potentially could be a null byte write bug)
	if(hrefLen == 10000)
		href[10000] = '\0';

	// find the first instance of the delimiter in the string
	char * startString = strstr(href, delims->startDelimiter);

	if(startString == NULL)
		return NULL;
	if( bpstrlen(startString) == 0  )
		return NULL;

	//----------------------------------------------------------------------
	// Main Variable Extraction Loop
	char * varNamePosition = NULL;

	// holds the name string
	char *varName;

	// initialize
	char *varValue = NULL;
	int i = 0;

	// initialize
	P_BP_HTML_ANCHOR_VAR_LIST varList = NULL;

	// iterate through embedded variables
	for( varNamePosition = startString; varNamePosition != NULL; ){

		// initial list creation
		if(varList == NULL){

			// create anchor list element
			varList = (P_BP_HTML_ANCHOR_VAR_LIST) BP_calloc(gc, NULL, sizeof(BP_HTML_ANCHOR_VAR_LIST), 1);
			varList->first = varList;
			varList->prev = NULL;
			varList->next = NULL;

		} else {

			if(varList->next == NULL)
				varList->next =(P_BP_HTML_ANCHOR_VAR_LIST) BP_calloc(gc, NULL, sizeof(BP_HTML_ANCHOR_VAR_LIST), 1);
			varList->next->first = varList->first;
			varList->next->prev = varList;
			varList->next->next = NULL;

			// move pointer forward
			varList = varList->next;

		}

		// -------------------------------------------------------------------
		// List should now be prepared to recieve one variable set

		i++;

		varName = HTMLGetAnchorEmbeddedVariableName(varNamePosition, delims, bpstrlen(varNamePosition), gc);
		if(varName)
			varValue = HTMLGetAnchorEmbeddedVariableValue(&varNamePosition[bpstrlen(varName)+1], delims, bpstrlen(&varNamePosition[bpstrlen(varName)]+1), gc);

		//	printf("\n\t\t extracted var: %s =- %s", varName, varValue);

		// set name directly
		varList->name = varName;

		// calculate name length
		if(varName)
			varList->nameLen = bpstrlen(varName);

		// set content directly
		varList->content    = varValue;

		// get content
		if(varList->content)
			varList->contentLen = bpstrlen(varValue);


		varNamePosition = strstr(&varNamePosition[1], delims->nextDelimiter);
		if(varNamePosition == NULL)
			break;

	}

	// return the var list
	return varList->first;
}

// Sub-function used in the extraction of embedded variable names
char * HTMLGetAnchorEmbeddedVariableName(char *inStr, P_BP_HTML_ANCHOR_DELIMITERS delims, size_t len, P_BP_GARBAGE_COLLECTOR_DATA gc){

	// default NULL checks
	if(inStr  == NULL) return NULL;
	if(delims == NULL) return NULL;

	size_t i = 0;

	if( strncmp(inStr, delims->valueDelimiter, delims->vLen) == 0){
			inStr+=delims->vLen;
	}

	if( strncmp(inStr, delims->nextDelimiter, delims->nLen) == 0){
		inStr+=delims->nLen;
	}

	if( strncmp(inStr, delims->startDelimiter, delims->sLen) == 0){
			inStr+=delims->sLen;
	}

	// loop to count till the next delimiter, next value, or null
	for(i = 0; (inStr[i] != '\0') && (i < len); 	i++){
		if( strncmp(&inStr[i], delims->valueDelimiter, delims->vLen) == 0){
			break;
		}

		if( strncmp(&inStr[i], delims->nextDelimiter, delims->nLen) == 0){
			break;
		}

	}
	// duplicate the instr from the variable position
	char *retStr = BP_strndup(gc, NULL, inStr, i);
	if(retStr != NULL)
		return(retStr);

	return NULL;
}

// Sub-function used in the extraction of embedded variable values
char * HTMLGetAnchorEmbeddedVariableValue(char *inStr, P_BP_HTML_ANCHOR_DELIMITERS delims, size_t len, P_BP_GARBAGE_COLLECTOR_DATA gc)
{

	// Default NULL checks
	if(inStr  == NULL) return NULL;
	if(delims == NULL) return NULL;

	size_t i = 0;

	if( strncmp(inStr, delims->valueDelimiter, delims->vLen) == 0)
			inStr+=delims->vLen;


	if( strncmp(inStr, delims->nextDelimiter, delims->nLen) == 0)
		inStr+=delims->nLen;


	if( strncmp(inStr, delims->startDelimiter, delims->sLen) == 0)
			inStr+=delims->sLen;

	// loop to count till the next delimiter, next value, or null
	for(i = 0; (inStr[i] != '\0') && (i < len); i++){

		if( strncmp(&inStr[i], delims->valueDelimiter, delims->vLen) == 0){
			break;
		}

		if( strncmp(&inStr[i], delims->nextDelimiter, delims->nLen) == 0)
			break;

	}


	char *retStr = BP_strndup(gc, NULL, inStr, i);
	if(retStr != NULL)
		return(retStr);

	return NULL;
}


// Returns a NULL terminated c style character string based off a BP anchor
// protocol value. NULL on failure.
char * HTMLGetAnchorProtocolStringFromVal(size_t val)
{

	switch(val){
		case BP_ANCHOR_PROTO_DEFAULT:
			return "default\0";
			break;
		case BP_ANCHOR_PROTO_ACAP:
			return "acap\0";
			break;
		case BP_ANCHOR_PROTO_ADDBOOK:
			return "addbook\0";
			break;
		case BP_ANCHOR_PROTO_AFP:
			return "afp\0";
			break;
		case BP_ANCHOR_PROTO_AFS:
			return "afs\0";
			break;
		case BP_ANCHOR_PROTO_AIM:
			return "aim\0";
			break;
		case BP_ANCHOR_PROTO_CALLTO:
			return "callto\0";
			break;
		case BP_ANCHOR_PROTO_CASTANET:
			return "castanet\0";
			break;
		case BP_ANCHOR_PROTO_CHTTP:
			return "chttp\0";
			break;
		case BP_ANCHOR_PROTO_CID:
			return "cid\0";
			break;
		case BP_ANCHOR_PROTO_DATA:
			return "data\0";
			break;
		case BP_ANCHOR_PROTO_DAV:
			return "dav\0";
			break;
		case BP_ANCHOR_PROTO_DICT:
			return "dict\0";
			break;
		case BP_ANCHOR_PROTO_DNS:
			return "dns\0";
			break;
		case BP_ANCHOR_PROTO_EID:
			return "eid\0";
			break;
		case BP_ANCHOR_PROTO_FAX:
			return "fax\0";
			break;
		case BP_ANCHOR_PROTO_FILE:
			return "file\0";
			break;
		case BP_ANCHOR_PROTO_FINGER:
			return "finger\0";
			break;
		case BP_ANCHOR_PROTO_FREENET:
			return "freenet\0";
			break;
		case BP_ANCHOR_PROTO_FTP:
			return "ftp\0";
			break;
		case BP_ANCHOR_PROTO_FTPS:
			return "ftps\0";
			break;
		case BP_ANCHOR_PROTO_GOPHER:
			return "gopher\0";
			break;
		case BP_ANCHOR_PROTO_GSM_SMS:
			return "gsm-sms\0";
			break;
		case BP_ANCHOR_PROTO_H323:
			return "h323\0";
			break;
		case BP_ANCHOR_PROTO_H324:
			return "h324\0";
			break;
		case BP_ANCHOR_PROTO_HDL:
			return "hdl\0";
			break;
		case BP_ANCHOR_PROTO_HNEWS:
			return "hnews\0";
			break;
		case BP_ANCHOR_PROTO_HTTP:
			return "http\0";
			break;
		case BP_ANCHOR_PROTO_HTTPS:
			return "https\0";
			break;
		case BP_ANCHOR_PROTO_IIOPLOC:
			return "iioploc\0";
			break;
		case BP_ANCHOR_PROTO_ILU:
			return "ILU\0";
			break;
		case BP_ANCHOR_PROTO_IOR:
			return "IOR\0";
			break;
		case BP_ANCHOR_PROTO_IMAP:
			return "IMAP\0";
			break;
		case BP_ANCHOR_PROTO_IPP:
			return "IPP\0";
			break;
		case BP_ANCHOR_PROTO_IRC:
			return "IRC\0";
			break;
		case BP_ANCHOR_PROTO_JAR:
			return "JAR\0";
			break;
		case BP_ANCHOR_PROTO_JAVA:
			return "Java\0";
			break;
		case BP_ANCHOR_PROTO_JAVASCRIPT:
			return "javascript\0";
			break;
		case BP_ANCHOR_PROTO_JDBC:
			return "JDBC\0";
			break;
		case BP_ANCHOR_PROTO_LDAP:
			return "LDAP\0";
			break;
		case BP_ANCHOR_PROTO_LIFN:
			return "LIFN\0";
			break;
		case BP_ANCHOR_PROTO_LIVESCRIPT:
			return "LIVESCRIPT\0";
			break;
		case BP_ANCHOR_PROTO_ICQ:
			return "ICQ\0";
			break;
		case BP_ANCHOR_PROTO_IRQ:
			return "IRQ\0";
			break;
		case BP_ANCHOR_PROTO_MAILBOX:
			return "Mailbox\0";
			break;
		case BP_ANCHOR_PROTO_MAILTO:
			return "Mailto\0";
			break;
		case BP_ANCHOR_PROTO_MD5:
			return "MD5\0";
			break;
		case BP_ANCHOR_PROTO_MID:
			return "MID\0";
			break;
		case BP_ANCHOR_PROTO_MIDI:
			return "MIDI\0";
			break;
		case BP_ANCHOR_PROTO_MOCHA:
			return "MOCHA\0";
			break;
		case BP_ANCHOR_PROTO_MODEM:
			return "MODEM\0";
			break;
		case BP_ANCHOR_PROTO_NEWS:
			return "News\0";
			break;
		case BP_ANCHOR_PROTO_NFS:
			return "NFS\0";
			break;
		case BP_ANCHOR_PROTO_NNTP:
			return "NNTP\0";
			break;
		case BP_ANCHOR_PROTO_OPAQUELOCKTOKEN:
			return "OPAQUELOCKTOKEN\0";
			break;
		case BP_ANCHOR_PROTO_PHONE:
			return "PHONE\0";
			break;
		case BP_ANCHOR_PROTO_POP:
			return "POP\0";
			break;
		case BP_ANCHOR_PROTO_POP3:
			return "POP3\0";
			break;
		case BP_ANCHOR_PROTO_PRINTER:
			return "Printer\0";
			break;
		case BP_ANCHOR_PROTO_PROSPERO:
			return "Prospero\0";
			break;
		case BP_ANCHOR_PROTO_RES:
			return "Res\0";
			break;
		case BP_ANCHOR_PROTO_RSTP:
			return "RSTP\0";
			break;
		case BP_ANCHOR_PROTO_RVP:
			return "RVP\0";
			break;
		case BP_ANCHOR_PROTO_RWHOIS:
			return "RWHOIS\0";
			break;
		case BP_ANCHOR_PROTO_RX:
			return "RX\0";
			break;
		case BP_ANCHOR_PROTO_SDP:
			return "SDP\0";
			break;
		case BP_ANCHOR_PROTO_SERVICE:
			return "Service\0";
			break;
		case BP_ANCHOR_PROTO_SIP:
			return "SIP\0";
			break;
		case BP_ANCHOR_PROTO_SMB:
			return "SMB\0";
			break;
		case BP_ANCHOR_PROTO_SNEWS:
			return "SNEWS\0";
			break;
		case BP_ANCHOR_PROTO_SOAP_BEEP:
			return "Soap.Beep\0";
			break;
		case BP_ANCHOR_PROTO_SOAP_BEEPS:
			return "Soap.Beeps\0";
			break;
		case BP_ANCHOR_PROTO_T120:
			return "T128\0";
			break;
		case BP_ANCHOR_PROTO_TCP:
			return "TCP\0";
			break;
		case BP_ANCHOR_PROTO_TEL:
			return "TEL\0";
			break;
		case BP_ANCHOR_PROTO_TELNET:
			return "TELNET\0";
			break;
		case BP_ANCHOR_PROTO_TIP:
			return "TIP\0";
			break;
		case BP_ANCHOR_PROTO_TN3270:
			return "TN3270\0";
			break;
		case BP_ANCHOR_PROTO_TV:
			return "TV\0";
			break;
		case BP_ANCHOR_PROTO_URN:
			return "URN\0";
			break;
		case BP_ANCHOR_PROTO_UUID:
			return "UUID\0";
			break;
		case BP_ANCHOR_PROTO_VEMMI:
			return "Vemmi\0";
			break;
		case BP_ANCHOR_PROTO_VIDEOTEX:
			return "Videotex\0";
			break;
		case BP_ANCHOR_PROTO_VIEW_SOURCE:
			return "view-source\0";
			break;
		case BP_ANCHOR_PROTO_WAIS:
			return "wais\0";
			break;
		case BP_ANCHOR_PROTO_WHODP:
			return "WHODP\0";
			break;
		case BP_ANCHOR_PROTO_WHOIS:
			return "Whois\0";
			break;
		case BP_ANCHOR_PROTO_Z39_50R:
			return "z39.50r\0";
			break;
		case BP_ANCHOR_PROTO_Z39_50S:
			return "z39.50s\0";
			break;
		case BP_ANCHOR_PROTO_UNKNOWN:
			break;

	}
	return NULL;
}


// ------------------------------------------------------------------------------
// Forms

P_BP_HTML_FORM_LIST HTMLParseForms(xmlNode *node, P_BP_HTML_FULL_EXTRACTION extraction)
{

	// ensure we have an initial node
	if(!node)
		return NULL;

	// ensure we have an extraction
	if(!extraction)
		return NULL;

	// attempt to allocate the list
	P_BP_HTML_FORM_LIST appendList = (P_BP_HTML_FORM_LIST) tq_calloc(extraction->tq, sizeof(BP_HTML_FORM_LIST), 1, BPLN);

	// set list pointers
	appendList->first = appendList;

	// parse the forms recursively
	P_BP_HTML_FORM_LIST retList = HTMLParseFormsRecurse(node, appendList, extraction);

	// return the list
	return retList;

}

// re-entrant, recursive version of the function
P_BP_HTML_FORM_LIST HTMLParseFormsRecurse(xmlNode *node, P_BP_HTML_FORM_LIST appendList, P_BP_HTML_FULL_EXTRACTION extraction)
{

	// ensure we have an xml node
	if(!node)
		return NULL;

	// ensure we have an append list
	if(!appendList)
		return NULL;

	// ensure we have an extraction
	if(!extraction)
		return NULL;

	// declare current node
	xmlNode *cur_node = NULL;

	// declare input node
	struct _xmlAttr *inputNode = NULL;

	for(cur_node = node; cur_node != NULL; cur_node = cur_node->next)
	{

		if(cur_node->type == XML_ELEMENT_NODE)
		{

			if(strncmp((char *) cur_node->name, "form", 3) == 0)
			{

				// if the input linked list is null allocate and assign first ptr
				if(appendList == NULL)
				{

					appendList = (P_BP_HTML_FORM_LIST) tq_calloc(extraction->tq, sizeof(BP_HTML_FORM_LIST), 1, BPLN);
					appendList->first = appendList;
					appendList->prev = NULL;
					appendList->next = NULL;

				}
				else
				{

					if(appendList->next == NULL)
					{
						appendList->next = (P_BP_HTML_FORM_LIST) tq_calloc(extraction->tq, sizeof(BP_HTML_FORM_LIST), 1, BPLN);
					}

					// reassign pointers and move list forward
					appendList->next->prev  = appendList;
					appendList->next->next  = NULL;
					appendList->next->first = appendList->first;
					appendList              = appendList->next;

				}

				// Store Form Properties
				for(inputNode = cur_node->properties; inputNode != NULL; inputNode = inputNode->next)
				{

					// list logic
					if(appendList->properties == NULL)
					{

						// allocate space for properties
						appendList->properties = (P_BP_HTML_FORM_PROPERTY_LIST) tq_calloc(extraction->tq, sizeof(BP_HTML_FORM_PROPERTY_LIST), 1, BPLN);


						appendList->properties->first = appendList->properties;
						appendList->properties->prev = NULL;
						appendList->properties->next = NULL;

					}
					else
					{

						// allocate the next pointer
						appendList->properties->next = (P_BP_HTML_FORM_PROPERTY_LIST) tq_calloc(extraction->tq, sizeof(BP_HTML_FORM_PROPERTY_LIST), 1, BPLN);

						// move pointers to indicate moving position
						appendList->properties->next->first = appendList->properties->first;
						appendList->properties->next->prev  = appendList->properties;
						appendList->properties->next->next  = NULL;
						appendList->properties = appendList->properties->next;

					}

					// calculate length
					appendList->properties->nLen = bpstrlen((char *) inputNode->name);

					// allocate space for the name
					appendList->properties->name = tq_strndup(extraction->tq, (char *) inputNode->name, appendList->properties->nLen+1, BPLN);

					// if chlidren exist, add here
					if(inputNode->children)
					{

						// calculate content length first
						appendList->properties->cLen = strnlen((char *) inputNode->children->content, 1024);

						// allocate and save content
						appendList->properties->content = tq_strndup(extraction->tq, (char *) inputNode->children->content, appendList->properties->cLen+1, BPLN);

					}
					else
					{
						appendList->properties->content = tq_strndup(extraction->tq, (char *) "", 2, BPLN);
						appendList->properties->cLen = 0;
					}


					//	printf("\n\t <.form value>: %s = %s\n", (char *) inputNode->name, inputNode->children->content);

				}

			}

			// Begin Parsing out form inputs
			if(BP_strncasecmp((char *) cur_node->name, "input", 5) == 0)
			{

				// input list logic (not values) values below
				if(appendList == NULL)
					continue;

				// if we don't have inputs, create initial inputs
				if(!appendList->inputs)
				{

					// allocate space for inputs
					appendList->inputs = (P_BP_HTML_INPUT_LIST) tq_calloc(extraction->tq, sizeof(BP_HTML_INPUT_LIST), 1, BPLN);

					// move pointers
					appendList->inputs->first = appendList->inputs;
					appendList->inputs->prev = NULL;
					appendList->inputs->next = NULL;

				}
				else
				{

					// allocate space for next pointer
					appendList->inputs->next = (P_BP_HTML_INPUT_LIST) tq_calloc(extraction->tq, sizeof(BP_HTML_INPUT_LIST), 1, BPLN);

					// move pointers
					appendList->inputs->next->first = appendList->inputs->first;
					appendList->inputs->next->prev  = appendList->inputs;
					appendList->inputs->next->next  = NULL;
					appendList->inputs = appendList->inputs->next;

				}

				if(cur_node)
				for
				(
					inputNode = cur_node->properties;
					inputNode;
					inputNode = inputNode->next
				)
				{

					// list logic
					if(appendList->inputs->properties == NULL)
					{

						// allocate space for properties
						appendList->inputs->properties = (P_BP_HTML_INPUT_PROPERTY_LIST) tq_calloc(extraction->tq, sizeof(BP_HTML_INPUT_PROPERTY_LIST), 1, BPLN);

						// set list pointers
						appendList->inputs->properties->first = appendList->inputs->properties;
						appendList->inputs->properties->prev = NULL;
						appendList->inputs->properties->next = NULL;

					}
					else
					{

						// allocate space for next property
						appendList->inputs->properties->next = (P_BP_HTML_INPUT_PROPERTY_LIST) tq_calloc(extraction->tq, sizeof(BP_HTML_INPUT_PROPERTY_LIST), 1, BPLN);

						// set list pointers
						appendList->inputs->properties->next->first = appendList->inputs->properties->first;
						appendList->inputs->properties->next->prev  = appendList->inputs->properties;
						appendList->inputs->properties->next->next  = NULL;
						appendList->inputs->properties = appendList->inputs->properties->next;

					}

					// set property data if we have an inputNode
					if(inputNode)
					{

						// calculate length
						appendList->inputs->properties->nLen = strnlen((char *) inputNode->name, 1000);

						// set name
						appendList->inputs->properties->name = tq_strndup(extraction->tq, (char *) inputNode->name, appendList->inputs->properties->nLen+1, BPLN);

					}

					// store children data if it's available for storage
					if(inputNode->children)
					{

						// calculate content length
						appendList->inputs->properties->cLen = strnlen((char *) inputNode->children->content, 1000);

						// store content
						appendList->inputs->properties->content = tq_strndup(extraction->tq, (char *) inputNode->children->content, appendList->inputs->properties->cLen+1, BPLN);

					}

				}

			}

			// forms can also use <SELECT> tags as input.  Check for them here.
			// Begin Parsing out form inputs
			if(BP_strncasecmp((char *) cur_node->name, "select", 6) == 0)
			{

				// input list logic (not values) values below
				if(!appendList)
					continue;

				if(!appendList->inputs)
				{

					// allocate space for input
					appendList->inputs = (P_BP_HTML_INPUT_LIST) tq_calloc(extraction->tq, sizeof(BP_HTML_INPUT_LIST), 1, BPLN);

					// move pointers
					appendList->inputs->first = appendList->inputs;
					appendList->inputs->prev = NULL;
					appendList->inputs->next = NULL;

				}
				else
				{

					// allocate space for next if necessary
					appendList->inputs->next = (P_BP_HTML_INPUT_LIST) tq_calloc(extraction->tq, sizeof(BP_HTML_INPUT_LIST), 1, BPLN);

					// set list pointers
					appendList->inputs->next->first = appendList->inputs->first;
					appendList->inputs->next->prev  = appendList->inputs;
					appendList->inputs->next->next  = NULL;
					appendList->inputs = appendList->inputs->next;

				}

				if(cur_node)
				for
				(
					inputNode = cur_node->properties;
					inputNode != NULL;
					inputNode = inputNode->next
				)
				{

					// list logic
					if(!appendList->inputs->properties)
					{

						// allocate space for properties
						appendList->inputs->properties = (P_BP_HTML_INPUT_PROPERTY_LIST) tq_calloc(extraction->tq, sizeof(BP_HTML_INPUT_PROPERTY_LIST), 1, BPLN);

						// assign list pointers
						appendList->inputs->properties->first = appendList->inputs->properties;
						appendList->inputs->properties->prev  = NULL;
						appendList->inputs->properties->next  = NULL;

					}
					else
					{

						// allocate space for properties
						appendList->inputs->properties->next = (P_BP_HTML_INPUT_PROPERTY_LIST) tq_calloc(extraction->tq, sizeof(BP_HTML_INPUT_PROPERTY_LIST), 1, BPLN);

						// move pointers up
						appendList->inputs->properties->next->first = appendList->inputs->properties->first;
						appendList->inputs->properties->next->prev  = appendList->inputs->properties;
						appendList->inputs->properties->next->next  = NULL;
						appendList->inputs->properties              = appendList->inputs->properties->next;

					}

					if(inputNode)
					{

						// calculate name length
						appendList->inputs->properties->nLen = strnlen((char *) inputNode->name, 1000);

						// allocate space
						appendList->inputs->properties->name = tq_strndup(extraction->tq, (char *) inputNode->name, appendList->inputs->properties->nLen+1, BPLN);

					}

					// if the node has children, copy properties
					if(inputNode)
					if(inputNode->children)
					{

						// calculate content length
						appendList->inputs->properties->cLen = strnlen((char *) inputNode->children->content, 1000);

						// store content
						appendList->inputs->properties->content = tq_strndup(extraction->tq, (char *) inputNode->children->content, appendList->inputs->properties->cLen, BPLN);

					}

				}

			}

		}

		// move the list to the end before recursion
		if(appendList != NULL)
		for
		(
			appendList        = appendList->first;
			appendList->next != NULL;
			appendList        = appendList->next
		)
		{}

		// parse the forms
		P_BP_HTML_FORM_LIST fl = HTMLParseFormsRecurse(cur_node->children, appendList, extraction);

	}

	// return list on success
	if(appendList != NULL)
		return appendList->first;

	// return null on failure
	return NULL;

}



// Anchor Structure Printing Algorithms
// void HTMLPrintAnchorVarList(P_BP_HTML_ANCHOR_VAR_LIST inList);
void HTMLPrintFormPropertyList( P_BP_HTML_FORM_PROPERTY_LIST properties)
{

	size_t i = 0;
	if(properties != NULL)
	for(properties = properties->first; properties != NULL; properties = properties->next){
		printf("\n\t\t *[printing property struct: %i]--------------------", i);
		printf("\n\t\t\t [-] name: %s", properties->name);
		printf("\n\t\t\t\t nLen: %i", properties->nLen);
		printf("\n\t\t\t [-] content: %s", properties->content);
		printf("\n\t\t\t\t cLen: %i", properties->cLen);
		i++;
	}

}

// Prints out form inputs and properties
void HTMLPrintFormInputList(P_BP_HTML_INPUT_LIST inputs)
{

	size_t i = 0;
	if(inputs != NULL)
	for(inputs = inputs->first; inputs != NULL; inputs = inputs->next){
			printf("\n\t\t\t printing form input struct: %i", i);
			printf("\n\t\t\t properties struct: %p", inputs->properties);

			if(inputs->properties != NULL)
				HTMLPrintFormInputPropertyList(inputs->properties);
			printf("\n");

	}
}

void  HTMLPrintFormInputPropertyList(P_BP_HTML_INPUT_PROPERTY_LIST properties)
{

	size_t i = 0;
	if(properties != NULL)
	for(properties = properties->first; properties != NULL; properties = properties->next)
	{

			printf("\n\n\t\t\t\t [+] Input property struct: %i", i);
			printf("\n\t\t\t\t name: %s", properties->name);
			printf("\n\t\t\t\t\t nLen: %i", properties->nLen);
			printf("\n\t\t\t\t content: %s", properties->content);
			printf("\n\t\t\t\t\t cLen: %i", properties->cLen);
			i++;
	}

}

// Root extracted form structure printing function
void HTMLPrintForms(P_BP_HTML_FORM_LIST formFirst, size_t display_blacklisted){

	size_t i = 0;

	if(formFirst == NULL){
		printf("\n [+] HTML Forms -- No Forms Were Found on Page.");
	}

	if(formFirst != NULL)
	for( i = 0 ; formFirst != NULL; formFirst = formFirst->next){


		if(!formFirst->properties)
			continue;

		if(formFirst->blacklisted && !display_blacklisted)
			continue;

		printf("\n");
		printf("\n [+] Displaying Form %i: (%p)", i, formFirst);
		printf("\n     blacklisted: %s", formFirst->blacklisted ? "Yes" : "No");
		printf("\n     properties:  %p", formFirst->properties);
		printf("\n     host:        %s", formFirst->host);
		printf("\n     base:        %s", formFirst->base);
		printf("\n     path:        %s", formFirst->path);
		printf("\n     script:      %s", formFirst->script);
		printf("\n     full_url:    %s", formFirst->full_url);

		// display properties if they exist
		if(formFirst->properties != NULL)
		{
			printf("\n");
			HTMLPrintFormPropertyList(formFirst->properties);
		}

		if(formFirst->inputs != NULL)
		{
			printf("\n");
			printf("\n               [+] Form Inputs:     %p", formFirst->inputs);
			if(formFirst->inputs != NULL)
				HTMLPrintFormInputList(formFirst->inputs);
		}
		printf("\n ");
		i++;
	}
}

// print out an extraction and all of its sub-structures (blacklisted == t/f)
void HTMLPrintFullExtraction(P_BP_HTML_FULL_EXTRACTION extraction, size_t display_blacklisted){

	// default NULL checks
	if(extraction == NULL)
		return;

	// print out anchors
	if(extraction->anchors != NULL)
		HTMLPrintAnchors(extraction->anchors, display_blacklisted);

	// print out forms
	if(extraction->forms != NULL)
		HTMLPrintForms(extraction->forms, display_blacklisted);

	return;
}

void HTMLPrintElementNamesAll(xmlNode * node){

	xmlNode *cur_node = NULL;
	struct _xmlAttr *inputNode = NULL;

	for(cur_node = node; cur_node; cur_node = cur_node->next){

		if(cur_node->type == XML_ELEMENT_NODE){

			if(strncmp((char *) cur_node->name, "form", 3) == 0){

				for(inputNode = cur_node->properties; inputNode != NULL; inputNode = inputNode->next){
					printf("\n\t <.form value>: %s = %s\n", (char *) inputNode->name, inputNode->children->content);
				}
			}

			if(strncmp((char *) cur_node->name, "input", 4) == 0){
				printf("\n\t\t <.input type>: %s\n", (char *) cur_node->name);

				for(inputNode = cur_node->properties; inputNode != NULL; inputNode = inputNode->next){
					printf("\n\t\t\t <.inputvalue>:    %s = %s\n", (char *) inputNode->name, inputNode->children->content);
				}
			}
		}

		HTMLPrintElementNamesAll(cur_node->children);

	}

}

//note: options could be encoded here later

// Will decode a retrieved html buffer into a DOM parsable structure. NULL for default delimiter set.
P_BP_HTML_FULL_EXTRACTION HTMLDecodeBuffer
(
		char *buff,
		int size,
		char *URL,
		char *encoding,
		P_BP_HTML_ANCHOR_DELIMITERS delims,
		int options,
		P_BP_GARBAGE_COLLECTOR_DATA gc_in,
		P_BP_HTTP_SESSION http_session
)
{

	// ensure we have a buffer and size
	if(!buff || !size)
		return NULL;

	// create local pointers (extracted is final structure ptr)
	P_BP_HTML_FULL_EXTRACTION extracted = (P_BP_HTML_FULL_EXTRACTION) bpcalloc(sizeof(BP_HTML_FULL_EXTRACTION), 1);

	// create new linkl
	extracted->tq = BP_CreateLinkL("html extractor", BP_FALSE);

	// set reference to the http session used if desired
	extracted->http_session_reference = http_session;


	// declare anchors
	P_BP_HTML_ANCHOR_LIST anchors       = NULL;

	// declare forms
	P_BP_HTML_FORM_LIST forms           = NULL;

	// set local garbage collector
	P_BP_GARBAGE_COLLECTOR_DATA gc      = NULL;


	// enable garbage collection
	if(!gc_in)
		gc = BP_CreateNewGarbageCollector("Decode buff gc.", true);
	else
		gc = gc_in;


	// Libxml html parser pointers
	htmlDocPtr  htmlDoc        = NULL;
	htmlNodePtr rootElement   = NULL;
	htmlNodePtr passedElement = NULL;

	// could cause problems later, investigate
	LIBXML_TEST_VERSION

	// read memory
	htmlDoc = htmlReadMemory
	(
		buff,
		size,
		URL,
		encoding,
		options
	);

	if(htmlDoc == NULL)
	{
		return NULL;
	}

	// arbitary delimiter pointer
	P_BP_HTML_ANCHOR_DELIMITERS defaultDelims = NULL;

	// create default delimiters if pointer to struct has not been passed
	if(delims == NULL)
	{

		// default delimiter set
		defaultDelims = (P_BP_HTML_ANCHOR_DELIMITERS) BP_calloc(gc, NULL, sizeof(BP_HTML_ANCHOR_DELIMITERS), 1);

		// set default asp/php style delimiters
		defaultDelims->startDelimiter = BP_strdup(gc, NULL, "?\0");
			defaultDelims->sLen = 1;
		defaultDelims->nextDelimiter = BP_strdup(gc, NULL, "&\0");
			defaultDelims->nLen = 1;
		defaultDelims->valueDelimiter = BP_strdup(gc, NULL, "=\0");
			defaultDelims->vLen = 1;

	}
	else
	{
		// switch value of passed delims parameter into the default delimiter position
		defaultDelims = delims;
	}

	if(defaultDelims == NULL)
		goto end;


	// set delimiter pointer in extracted structure
	extracted->delims = defaultDelims;

	// Retrieve root element from the html parser
	rootElement = xmlDocGetRootElement(htmlDoc);
	passedElement = rootElement;

	// parse out the anchors
	anchors = HTMLParseAnchors(passedElement, defaultDelims, gc, extracted);

	// log the chunk in the garbage collector
	BP_GarbageCollectorLogChunk(gc, ".", anchors, 4, BP_GARBAGE_COLLECTOR_DATA_ALLOC_CALLOC);

	// set anchors in the main retrieval struct
	extracted->anchors = anchors;

	// set passed ellement
	passedElement = rootElement;

	// parse forms here
	forms = HTMLParseForms(rootElement, extracted);

	// log the forms as a chunk itself
	BP_GarbageCollectorLogChunk(gc, ".", (void *) forms, 4, BP_GARBAGE_COLLECTOR_DATA_ALLOC_CALLOC);

	//forms = NULL;

	// set forms in main extraction returning struct
	extracted->forms = forms;

	// Parse secondary form data if we've got forms to examine
	if(extracted->forms)
	{
		HTMLParseFormSecondaryData(extracted->forms, extracted);
	}

	// reset the default delims position to null if a valid
	// delimiter parameter was passed. (to prevent bpfree'ing)
	if(delims != NULL)
		defaultDelims = NULL;


	//------------------------------------------------
	end:

	// Destroy xml parsing
	xmlFreeDoc(htmlDoc);

	// set garbage collector in extraction structure
	extracted->gc = gc;

	// return the extracted form
	return extracted;
}



// TODO: (appears to be unused, perhaps remove?) Duplicates an anchor list with new memory and pointers (used for attacks)
P_BP_HTML_ANCHOR_LIST HTMLDuplicateAnchorVariables(P_BP_HTML_ANCHOR_VAR_LIST vars, P_BP_GARBAGE_COLLECTOR_DATA gc){

	// Null check
	if(!vars)
		return NULL;

	// assigning anchor list
	P_BP_HTML_ANCHOR_VAR_LIST tmpList = vars;

	// actual list
	P_BP_HTML_ANCHOR_VAR_LIST newList = NULL;


	// used for assigning pointers below
	bool isFirst;


	// Walk the varlist and recreate elements
	for(tmpList = tmpList->first;
		tmpList != NULL;
		tmpList = tmpList->next){


		// determine if we're at the lists first position
		if(tmpList->prev == NULL)
			isFirst = true;
		else
			isFirst = false;


		// reassign the pointers
		if(isFirst){

			// allocate space for the new element
			newList = (P_BP_HTML_ANCHOR_VAR_LIST) BP_calloc(gc, NULL, sizeof(P_BP_HTML_ANCHOR_VAR_LIST), 1);
			if(!newList)
				return NULL;

			// Set arbitrary list pointers
			newList->prev  = NULL;
			newList->first = newList;
			newList->next  = NULL;

		} else {

			// if its not first, allocate space for next and move pointer up
			newList->next = (P_BP_HTML_ANCHOR_VAR_LIST) BP_calloc(gc, NULL, sizeof(P_BP_HTML_ANCHOR_VAR_LIST), 1);

			// append list item and move list up
			newList->next->prev = newList;
			newList->next->first = newList->first;
			newList = newList->next;

		}

		// copy in the old list
		memcpy(newList, tmpList, sizeof(BP_HTML_ANCHOR_LIST));


	}

	return NULL;
}


// Retrieves the number of anchor variables from an anchor var list (walks)
size_t HTMLGetAnchorVarCount(P_BP_HTML_ANCHOR_VAR_LIST vars){
	if(!vars)
		return 0;

	size_t count = 0;
	for(; vars; vars = vars->next)
		count++;

	return count;
}

// Counts the elements in a set of inputs
size_t HTMLGetInputCount(P_BP_HTML_INPUT_LIST inputs)
{

	// ensure we have inputs
	if(!inputs)
		return NULL;

	// return this counter
	size_t count = 0;

	// iterate through list and return count
	for(; inputs; inputs = inputs->next)
		count++;

	// return count
	return count;

}




// Retrieves the number of form variables found in a form list
size_t HTMLGetFormVarCount(P_BP_HTML_FORM_LIST vars){

	if(!vars)
		return 0;

	if(!vars->inputs)
		return 0;

	P_BP_HTML_INPUT_LIST inputs = vars->inputs->first;

	size_t count = 0;
	for(; inputs; inputs = inputs->next)
		count++;

	return count;
}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Secondary Data Extractors %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Parse Secondary Data (script/path/base/host/full_url)
BP_ERROR_T HTMLParseFormSecondaryData(P_BP_HTML_FORM_LIST forms, P_BP_HTML_FULL_EXTRACTION extraction)
{

	// ensure we have forms to search
	if(!forms)
		return ERR_FAILURE;

	// ensure we have extraction
	if(!extraction)
		return ERR_FAILURE;

	// ensure we have a http session
	if(!extraction->http_session_reference)
		return ERR_FAILURE;

	// ensure we have a tailqueue allocator
	if(!extraction->tq)
		return ERR_FAILURE;

	// save the first element
	P_BP_HTML_FORM_LIST forms_saved_first = forms->first;

	// form action
	P_BP_HTML_FORM_PROPERTY_LIST form_action = NULL;

	// temporary form action string to use
	char *form_action_target = NULL;

	// form method
	BP_HTML_FORM_METHOD form_method = BP_HTML_FORM_METHOD_UNKNOWN;




	// walk the forms
	for
	(
		;
		forms;
		forms = forms->next
	)
	{

		// must have a form
		if(!forms)
			continue;

		// form must have properties
		if(!forms->properties && !forms->inputs)
			continue;

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Set Action %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		P_BP_HTML_FORM_PROPERTY_LIST method = HTMLGetFormPropertyByNameI(forms->properties, "method");

		// set default to http get
		if(!method)
			forms->method = BP_HTML_FORM_METHOD_GET;

		// check ot see that we have a method
		if(method)
		{

			// see if it's post, if not, set it to http get
			if(BP_strncasecmp(method->content, "post", 4) == 0)
				forms->method = BP_HTML_FORM_METHOD_POST;
			else
				forms->method = BP_HTML_FORM_METHOD_GET;

		}

		// attempt to retrieve the action
		form_action = HTMLGetFormPropertyByNameI(forms->properties, "action");
		if(!form_action)
		{
			form_action_target = "./";
		}
		else
		{
			form_action_target = form_action->content;
			if(!bpstrlen(form_action_target))
				form_action_target = "./";
		}


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Build Secondary Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// calculate anchor length
		size_t anchor_len = 0;
		anchor_len = bpstrlen(form_action_target);

		// temporary string for joining together string elements
		char *tmp_joined = NULL;

		char * last_requested = NULL;
		if(extraction->http_session_reference->lastRequestedHref[bpstrlen(extraction->http_session_reference->lastRequestedHref)-1] != '/')
			last_requested = BP_JoinStrings(extraction->http_session_reference->lastRequestedHref, "/");
		else
			last_requested = bpstrdup(extraction->http_session_reference->lastRequestedHref);


		// calculate the base
		char * tmp_base = BP_ParseURLToBasePCRE(form_action_target);
		if(!tmp_base)
			tmp_base = BP_ParseURLToBasePCRE(last_requested);

		char * tmp_path = BP_ParseURLToPathPCRE(form_action_target);
		if(!tmp_path)
			tmp_path = BP_ParseURLToPathPCRE(last_requested);

		// due to the loose matching on the host matching
		// expression, we can only use the base as input if
		// we want things to appear correctly.
		char * tmp_host   = NULL;
		if(tmp_base)
			tmp_host = BP_ParseURLToHostPCRE(tmp_base);

		// after path is parsed, this will get set with the
		// full URL.
		char *tmp_full_url = NULL;

		// create a join stack for testing things
		char * tmp_join_stack[10];
		BP_ZERO_STACK(tmp_join_stack);

		// if the path couldn't be created in either of both ways, run
		// some checks to try to determine relative path.
		if(!tmp_path)
		{

			// calculate the length of the href
			size_t tmp_path_len = bpstrlen(form_action_target);

			// check to make sure that we're not just pointing
			// to our current directory
			if( tmp_path_len == 0)
			{
				tmp_path     = strdup(tmp_base);
				tmp_full_url = strdup(tmp_base);

			}
			else if( tmp_path_len == 2)
			{


				// if it's a basic path from root, set it here
				if(memcmp(form_action_target, "./", 2) == 0)
				{
					if(tmp_base)
					{
						tmp_path     = strdup(tmp_base);
						tmp_full_url = strdup(tmp_base);
					}
				}


			}

			// if the tmp path hasn't been set yet, set it
			// forcibly here
			if(tmp_path_len && !tmp_path)
			{


				// check for relative path and attempt construction
				// if possible.
				if(tmp_path_len >= 2)
				{
					if
					(
						(form_action_target[0] == '.') &&
						(form_action_target[1] == '/')
					)
					{

						// destroy joiner string if required
						if(tmp_joined)
						{
							bpfree(tmp_joined);
							tmp_joined = NULL;
						}

						// set join elements
						tmp_join_stack[0] = tmp_base;
						tmp_join_stack[1] = (char *) &form_action_target[2];

						// attempt to join the string
						tmp_joined = BP_JoinStringArray((char **) &tmp_join_stack, 2);

						// set full url here
						tmp_full_url = bpstrdup(tmp_joined);

						// parse full url to path
						tmp_path = BP_ParseURLToPathPCRE(tmp_joined);
						if(!tmp_path)
							tmp_path = strdup(tmp_base);

					}

				}


				// destroy joiner string if required
				if(tmp_joined)
				{
					bpfree(tmp_joined);
					tmp_joined = NULL;
				}

				// if it still hasn't been set, run the last test/set
				if(!tmp_path)
				{

					// destroy joiner string if required
					if(tmp_joined)
					{
						bpfree(tmp_joined);
						tmp_joined = NULL;
					}

					// set join elements
					tmp_join_stack[0] = tmp_base;
					if(form_action_target[0] == '/')
						tmp_join_stack[1] = (char *) &form_action_target[1];
					else
						tmp_join_stack[1] = (char *) form_action_target;

					// attempt to join the string
					tmp_joined = BP_JoinStringArray((char **) &tmp_join_stack, 2);

					// set full url here
					tmp_full_url = bpstrdup(tmp_joined);

					tmp_path = BP_ParseURLToPathPCRE(tmp_joined);
					if(!tmp_path)
						tmp_path = strdup(tmp_base);

				}

				// destroy joiner string if required
				if(tmp_joined)
				{
					bpfree(tmp_joined);
					tmp_joined = NULL;
				}

			}

		}



		// build full url
		char * tmp_check_path = BP_ParseURLToPathPCRE(form_action_target);
		char * join_position = NULL;
		if(!tmp_check_path)
		{

			if(form_action_target[0] == '/')
				join_position = &form_action_target[1];
			else if (form_action_target[0] == '.' && form_action_target[1] == '/')
				join_position = &form_action_target[2];
			else
				join_position = form_action_target;

			tmp_full_url = BP_JoinStrings(tmp_path, join_position);

			// destroy tmp path before recreating path
			if(tmp_path)
				bpfree(tmp_path);

			// reparse the path since we have a new url to parse from
			tmp_path = BP_ParseURLToPathPCRE(tmp_full_url);


		}
		else
		{


			size_t href_len     = bpstrlen(form_action_target);
			size_t tmp_path_len = bpstrlen(tmp_path);

			// check to see if the path and the href are identical
			if(href_len == tmp_path_len)
			if(memcmp(form_action_target, tmp_path, tmp_path_len) == 00)
				tmp_full_url = strdup(form_action_target);

			if(!tmp_full_url)
				tmp_full_url = strdup(form_action_target);

			// destroy the check path
			bpfree(tmp_check_path);

		}


		// now calculate script, since we've got the full url at
		// this point.
		char * tmp_script = NULL;

		// parse the full url as the script
		tmp_script = BP_ParseURLToScriptPCRE(tmp_full_url);

		// destroy last requested if necessary
		if(last_requested)
			bpfree(last_requested);

		// Set values in anchor element by duplicating them
		// via the tailqueue allocator.
		forms->host     = tq_strdup(extraction->tq, tmp_host, BPLN);
		forms->base     = tq_strdup(extraction->tq, tmp_base, BPLN);
		forms->path     = tq_strdup(extraction->tq, tmp_path, BPLN);

		// set the temporary script
		if(tmp_script)
			forms->script   = tq_strdup(extraction->tq, tmp_script, BPLN);
		else
			forms->script   = tq_strdup(extraction->tq, tmp_path, BPLN);

		// set the full url
		if(!tmp_full_url)
			forms->full_url = tq_strdup(extraction->tq, tmp_path, BPLN);
		else
			forms->full_url = tq_strdup(extraction->tq, tmp_full_url, BPLN);


		// Uncomment this if you need to debug.
		/*
		// get the form name and content
		printf("\n Got Action: %s", form_action_target);
		printf("\n form host:     %s", forms->host);
		printf("\n form base:     %s", forms->base);
		printf("\n form path:     %s", forms->path);
		printf("\n form script:   %s", forms->script);
		printf("\n form full_url: %s", forms->full_url);
		*/

		// destroy temporary values
		bpfree(tmp_host);
		bpfree(tmp_base);
		bpfree(tmp_path);
		bpfree(tmp_script);
		bpfree(tmp_full_url);




	}


	// return indicating success
	return ERR_SUCCESS;

}




// Parse Secondary Anchor Data
BP_ERROR_T HTMLParseAnchorsSecondaryData(P_BP_HTML_ANCHOR_LIST anchors, P_BP_HTML_FULL_EXTRACTION extraction)
{

	// ensure we have forms to search
	if(!anchors)
		return ERR_FAILURE;

	// ensure we have extraction
	if(!extraction)
		return ERR_FAILURE;

	// ensure we have a http session
	if(!extraction->http_session_reference)
		return ERR_FAILURE;

	// ensure we have a tailqueue allocator
	if(!extraction->tq)
		return ERR_FAILURE;

	// set anchor first position
	P_BP_HTML_ANCHOR_LIST anchor_iter = anchors->first;

	// walk the forms
	for
	(
		;
		anchor_iter;
		anchor_iter = anchor_iter->next
	)
	{



	}


	// return indicating success
	return ERR_SUCCESS;

}

// -------------------------------------------------------------
// Input Property Retrieval


// Gets a property by name
P_BP_HTML_INPUT_PROPERTY_LIST HTMLGetInputPropertyByName(P_BP_HTML_INPUT_PROPERTY_LIST properties, char *name){

	// null checks
	if(!properties || !name)
		return NULL;

	// walk properties list
	for(properties = properties->first; properties; properties = properties->next){

		// make sure buffers dont exceed
		if(bpstrlen(properties->name) > bpstrlen(name))
			continue;

		// if memcmp == 0 we have a match
		if(!memcmp(name, properties->name, properties->nLen))
			return properties;

	}

	// if property was not found, return null
	return NULL;
}

// JM NOTE: CASE INSENSITIVE VERSION OF ABOVE FUNCTION
// Gets a property by name
P_BP_HTML_INPUT_PROPERTY_LIST HTMLGetInputPropertyByNameI(P_BP_HTML_INPUT_PROPERTY_LIST properties, char *name){

	// null checks
	if(!properties || !name)
		return NULL;

	char *inStr = bpstrdup(name);
	char *matchStr = NULL;

	size_t x = 0;
	for(x = 0; inStr[x]; x++)
		inStr[x] = tolower(inStr[x]);

	// walk properties list
	for(properties = properties->first; properties; properties = properties->next){

		// make sure buffers dont exceed
		if(bpstrlen(properties->name) > bpstrlen(name))
			continue;

		matchStr = bpstrdup(properties->name);
		if(!matchStr)
			return NULL;

		// convert to lowercase
		for(x = 0; matchStr[x]; x++)
			matchStr[x] = tolower(matchStr[x]);

		// if memcmp == 0 we have a match
		if(!memcmp(inStr, matchStr, properties->nLen)){
			bpfree(matchStr);
			bpfree(inStr);
			return properties;
		}

		bpfree(matchStr);
	}

	if(inStr)
		bpfree(inStr);

	// if property was not found, return null
	return NULL;
}


// Gets a anchor var by name
P_BP_HTML_ANCHOR_VAR_LIST HTMLGetAnchorVarByNameI(P_BP_HTML_ANCHOR_VAR_LIST vars, char *name)
{

	// null checks
	if(!vars || !name)
		return NULL;

	// set ensure we nave a name length
	size_t name_len = bpstrlen(name);
	if(!name_len)
		return NULL;

	// set to the current variable names length
	size_t current_var_name_len = 0;

	// walk properties list
	for(vars = vars->first; vars; vars = vars->next)
	{


		// we need a name
		if(!vars->name)
			continue;

		// set name length
		current_var_name_len = bpstrlen(vars->name);

		// ensure lengths match
		if(current_var_name_len != name_len)
			continue;

		// return the list if we have a match
		if(BP_strncasecmp(name, vars->name, current_var_name_len) == 0)
			return vars;

	}

	// if property was not found, return null
	return NULL;

}



// Gets a anchor var by value.
P_BP_HTML_ANCHOR_VAR_LIST HTMLGetAnchorVarByValueI(P_BP_HTML_ANCHOR_VAR_LIST vars, char *value)
{

	// null checks
	if(!vars || !value)
		return NULL;

	// set ensure we nave a value length
	size_t value_len = bpstrlen(value);
	if(!value_len)
		return NULL;

	// set to the current variable names length
	size_t current_var_value_len = 0;

	// walk properties list, find value
	for(vars = vars->first; vars; vars = vars->next)
	{

		// we need a name
		if(!vars->content)
			continue;

		// set name length
		current_var_value_len = bpstrlen(vars->content);

		// ensure lengths match
		if(current_var_value_len != value_len)
			continue;

		// return the list if we have a match
		if(BP_strncasecmp(value, vars->content, current_var_value_len) == 0)
			return vars;

	}

	// if property was not found, return null
	return NULL;

}




// Gets a anchor var by position
P_BP_HTML_ANCHOR_VAR_LIST HTMLGetAnchorVarByPosition(P_BP_HTML_ANCHOR_VAR_LIST vars, size_t position)
{

	// null checks
	if(!vars)
		return NULL;

	// index used for counting
	size_t index = 1;

	// walk properties list
	for(vars = vars->first; vars; vars = vars->next)
	{

		// if we got the index, return here
		if(index == position)
			return vars;

	}

	// if property was not found, return null
	return NULL;

}


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
)
{


	// ensure we have an anchor and anchor vars
	if(!anchor)
		return NULL;
	if(!anchor_vars)
		return NULL;


	// these are used for lookups
	P_BP_HTML_ANCHOR_VAR_LIST sub_var_name_list  = NULL;

	// create a tailqueue so we can just repeat joins without
	// saving references.
	P_BP_LINKL tq = BP_CreateLinkL("tmp", BP_FALSE);

	// if we're planning to sub out a variable name, set here
	if(sub_var_name)
	{

		sub_var_name_list = HTMLGetAnchorVarByNameI(anchor_vars, sub_var_name);
		if(!sub_var_name_list)
			return NULL;

	}

	// string used for building
	char * builder_str = NULL;

	// create the initial string
	builder_str = BP_JoinStrings(anchor->script, "?", tq);

	// walk anchors and perform replacements
	for
	(
		anchor_vars = anchor_vars->first;
		anchor_vars;
		anchor_vars = anchor_vars->next
	)
	{

		// if we've found our replacement, run substitutions
		if(anchor_vars == sub_var_name_list)
		{
			if(replace_var_name)
				builder_str = BP_JoinStrings(builder_str, replace_var_name, tq);
			else
				builder_str = BP_JoinStrings(builder_str, anchor_vars->name, tq);

			// add seperator
			builder_str = BP_JoinStrings(builder_str, "=", tq);

			if(replace_var_value)
				builder_str = BP_JoinStrings(builder_str, replace_var_value, tq);
			else
				builder_str = BP_JoinStrings(builder_str, anchor_vars->content, tq);
		}
		else
		{

			// set content
			builder_str = BP_JoinStrings(builder_str, anchor_vars->name, tq);
			builder_str = BP_JoinStrings(builder_str, "=", tq);
			builder_str = BP_JoinStrings(builder_str, anchor_vars->content, tq);

		}

		// break if we have no next element
		if(!anchor_vars->next)
			break;

		// add trailing ampersand if we have a next element
		builder_str = BP_JoinStrings(builder_str, "&", tq);

	}

	// create the return value at this point
	char * ret_val = NULL;
	if(tq_in)
		ret_val =  tq_strdup(tq_in, builder_str, BPLN);
	else
		ret_val =  bpstrdup(builder_str);

	// destroy the linkl before exiting
	BP_DestroyLinkL(tq, BP_TRUE);

	// return the filled out value
	return ret_val;

}

// -------------------------------------------------------------
// Form Property Retrieval


// Gets a property by name
P_BP_HTML_FORM_PROPERTY_LIST HTMLGetFormPropertyByName(P_BP_HTML_FORM_PROPERTY_LIST properties, char *name){

	// null checks
	if(!properties || !name)
		return NULL;

	// walk properties list
	for(properties = properties->first; properties; properties = properties->next){

		// make sure buffers dont exceed
		if(bpstrlen(properties->name) > bpstrlen(name))
			continue;

		// if memcmp == 0 we have a match
		if(!memcmp(name, properties->name, properties->nLen))
			return properties;

	}

	// if property was not found, return null
	return NULL;
}

// JM NOTE: CASE INSENSITIVE VERSION OF ABOVE FUNCTION
// Gets a property by name
P_BP_HTML_FORM_PROPERTY_LIST HTMLGetFormPropertyByNameI(P_BP_HTML_FORM_PROPERTY_LIST properties, char *name){

	// null checks
	if(!properties || !name)
		return NULL;

	char *inStr = bpstrdup(name);
	char *matchStr = NULL;

	size_t x = 0;
	for(x = 0; inStr[x]; x++)
		inStr[x] = tolower(inStr[x]);

	// walk properties list
	for(properties = properties->first; properties; properties = properties->next){

		// make sure buffers dont exceed
		if(bpstrlen(properties->name) > bpstrlen(name))
			continue;

		matchStr = bpstrdup(properties->name);
		if(!matchStr)
			return NULL;

		// convert to lowercase
		for(x = 0; matchStr[x]; x++)
			matchStr[x] = tolower(matchStr[x]);

		// if memcmp == 0 we have a match
		if(!memcmp(inStr, matchStr, properties->nLen)){
			bpfree(matchStr);
			bpfree(inStr);
			return properties;
		}

		bpfree(matchStr);
	}

	if(inStr)
		bpfree(inStr);

	// if property was not found, return null
	return NULL;
}


// Returns a form method based around an input string
BP_HTML_FORM_METHOD HTMLGetFormMethodFromString(char *methodStr){

	if(!methodStr)
		return BP_HTML_FORM_METHOD_UNSET;

	if(BP_strncasecmp(methodStr, "post", 4) == 0)
	{
		return BP_HTML_FORM_METHOD_POST;
	}

	if(BP_strncasecmp(methodStr, "get", 3) == 0)
	{
		return BP_HTML_FORM_METHOD_GET;
	}

	// return default as http get
	return BP_HTML_FORM_METHOD_GET;

}


// --------------------------------------------------------------------------
// URL BLACKLISTING: Removes elements from anchor/form target results based on blacklisting


// Sets the blacklist property in the anchor list to true for each matching entry in the blacklist_urls array.
BP_ERROR_T HTMLApplyAnchorBlacklist(P_BP_HTML_ANCHOR_LIST anchors, char **blacklist_urls, size_t entry_count)
{

	if(!anchors)
		return ERR_FAILURE;

	if(!blacklist_urls)
		return ERR_FAILURE;

	if(!blacklist_urls[0])
		return ERR_FAILURE;

	// loop index for blacklist iteration loop
	size_t i = 0;

	// walk the form list and run string matches
	for(; anchors != NULL; anchors = anchors->next)
	{

		// Ensure anchor pointer is set
		if(anchors->href)
		{

			// walk every entry in the blacklist array, and compare it with the anchor href
			for(i = 0; i < entry_count; i++)
			{

				// if the first character is not a ":", we have a base match type
				if(blacklist_urls[i][0] != ':')
				{

					// if the blacklist matches, set the blacklist flag in the form property (from the start)
					if(strstr(anchors->href, blacklist_urls[i] ) == anchors->href)
					{

						// set the flag to true, this form will not be used when running loops
						anchors->blacklisted = 1;

					}
				}
				else // if it is a ":" we have a regexp match
				{

					// store match information
					BP_REGEX_MATCH_INFO match_info = {0};

					// match regular expression
					BP_ERROR_T match_code = BP_MatchRegularExpression(anchors->href,
							bpstrlen(anchors->href),
							(char *)&blacklist_urls[i][1],
							0,
							1,
							(P_BP_REGEX_MATCH_INFO) &match_info
					);

					// check for a successful match
					if(match_code == ERR_SUCCESS)
						anchors->blacklisted = 1;

					// BP_Regmatch_info.matches
					// TODO: May need to bpfree matches here.

				}

			}

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// Sets the blacklist property in the form list to true for each matching entry in the blacklist_urls array.
BP_ERROR_T HTMLApplyFormBlacklist(P_BP_HTML_FORM_LIST forms, char **blacklist_urls, size_t entry_count)
{

	if(!forms)
		return ERR_FAILURE;

	if(!blacklist_urls)
		return ERR_FAILURE;

	if(!blacklist_urls[0])
		return ERR_FAILURE;

	// loop index for blacklist iteration loop
	size_t i = 0;

	// walk the form list and run string matches
	for(; forms != NULL; forms = forms->next)
	{

		// save the first property if property exists
		P_BP_HTML_FORM_PROPERTY_LIST firstElem = NULL;
		if(forms->properties)
			firstElem = forms->properties->first;

		// walk properties if they exist
		if(forms->properties)
		for(
				forms->properties = forms->properties->first;
				forms->properties;
				forms->properties = forms->properties->next
		)
		{

			// check to see if the form element has a  property matching the name "target" and run
			// url checks on its value if so (effectively the main blacklist routine)
			if(BP_strncasecmp(forms->properties->name, "action", bpstrlen("action"))==0)
			{

				// walk every entry in the blacklist array, and compare it with the target value
				for(i = 0; i < entry_count; i++)
				{

					if(blacklist_urls[i][0] != ':')
					{
						// if the blacklist matches, set the blacklist flag in the form property
						if(
								forms->properties->content == (char *) strstr(forms->properties->content, blacklist_urls[i])
						)
						{

							// set the flag to true
							forms->blacklisted = 1;

						}

					}
					else // if it is a ":" we have a regexp match
					{

						// store match information
						BP_REGEX_MATCH_INFO match_info = {0};

						// match regular expression
						BP_ERROR_T match_code = BP_MatchRegularExpression(forms->properties->content,
																		bpstrlen(forms->properties->content),
																		(char *)&blacklist_urls[i][1],
																		0,
																		1,
																		(P_BP_REGEX_MATCH_INFO) &match_info
												);

						// check for a successful match
						if(match_code == ERR_SUCCESS)
							forms->blacklisted = 1;

					}

				}

			}

			// reset first element after O(n) walk
			forms->properties->first = firstElem;

		}

	}

	// return indicating success
	return ERR_SUCCESS;
}

// Sets the blacklist flags in the anchor/form elements if they contain anything in the blacklist_urls array.
BP_ERROR_T HTMLApplyBlacklist(P_BP_HTML_FULL_EXTRACTION full_extraction, char **blacklist_urls, size_t entry_count)
{

	if(!full_extraction)
		return ERR_FAILURE;

	if(!blacklist_urls)
		return ERR_FAILURE;


	// apply the anchor blacklist
	HTMLApplyAnchorBlacklist(full_extraction->anchors, blacklist_urls, entry_count);

	// apply the form blacklist
	HTMLApplyFormBlacklist(full_extraction->forms, blacklist_urls, entry_count);


	// return indicating success
	return ERR_SUCCESS;
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%% Utility Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Get a null terminated char ** from an anchor list
char ** HTMLGetUrlStringArrayFromAnchors(P_BP_HTML_ANCHOR_LIST anchors, size_t use_blacklisted, P_BP_GARBAGE_COLLECTOR_DATA gc)
{

	if(!anchors)
		return NULL;

	// save first pointer
	P_BP_HTML_ANCHOR_LIST first = anchors->first;

	// char ** of null ptr terminated character pointers representing urls
	char ** hrefs = NULL;

	// element counter
	size_t elements = 0;

	// iterate through anchors
	for(anchors = anchors->first; anchors; anchors = anchors->next)
	{

		// if blacklisting is enabled, and the anchor is blacklisted, continue the loop
		if(!use_blacklisted)
		if(anchors->blacklisted)
			continue;

		if(!anchors->href)
			continue;

		// increment the element count
		elements++;

		// bprealloc the buffer
		hrefs = (char **) BP_realloc(gc, NULL, hrefs, sizeof(char **) * (elements + 2));

		// duplicate the anchors href in the hrefs buffer
		// printf("\n Duplicating anchor?: %s",anchors->href);
		hrefs[elements-1] = bpstrndup(anchors->href, 2048);
		hrefs[elements]   = NULL;
		printf("\n [+] search_engine_result [%u]:  %s", elements-1, hrefs[elements-1]);

	}

	// return hrefs
	return hrefs;

}

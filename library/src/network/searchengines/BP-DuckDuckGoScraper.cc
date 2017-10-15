/*
 * BP-DdgScraper.cc
 * -----------------------------------------------
 * This file holds the routines responsible for extracting and storing results from ddg searches.  It
 * only uses libcurl and libxml for making requests/scraping data.  See src/libtest/ddg/ for usage
 * information.
 *
 */


#include "../../../include/BP-Main.h"


// Default search path for ddg, change this if ddg changes.  Format characters are substituted at run.
// REMOVED &hl=en when we added TLD jumping.
const char * bp_ddg_default_search = "https://3g2upl4pq6kufc4m.onion/html?q=%s";

// Default search without query data
const char * bp_ddg_default_search_without_query = "https://3g2upl4pq6kufc4m.onion/html";

// this indicates a successful page retrieval
const char * ddg_completion_indicator = "";



// Creates a new ddgscrape session out of an existing HTTP_SESSION structure.  Use the functions
// in BP-NetworkWeb.h to create the session before passing it here.  YOU ARE RESPONSIBLE FOR SEARCH INPUT
// FORMATTING IN search_query.
P_BP_DDGSCRAPE_SESSION BP_OpenNewDdgSearch(char *search_query, P_BP_HTTP_SESSION http_session)
{

	// check to make sure we have a search pointer
	if(!search_query)
		return NULL;

	// ensure we have a http session pointer
	if(!http_session)
		return NULL;

	// create a new self allocator
	P_BP_LINKL self_allocator = BP_CreateLinkL("ddg_search", BP_TRUE);

	// create new session
	P_BP_DDGSCRAPE_SESSION session = (P_BP_DDGSCRAPE_SESSION) tq_calloc(self_allocator, sizeof(BP_DDGSCRAPE_SESSION), 1, BPLN);

	// set self referential allocator
	session->tq = self_allocator;


	// create new garbage collector
	// session->gc = BP_CreateNewGarbageCollector("ddgscrape collector", 1);

	// --- Fill in Structure Values -------------------

	// set the web request pointer in our ddgscrape session
	session->http_session = http_session;

	// duplicate the default search  search string (used as a basis for a sprintf replace later)
	session->ddg_default_search = tq_strdup(session->tq, (char *) bp_ddg_default_search, BPLN);

	// set the default number of results to return per query (100 by default, set manually in the structure later if wanted)
	session->retrieve_results = 100;

	// set the search string now
	session->search_query = tq_strdup(session->tq, (char *) search_query, BPLN);

	// set the session timeout to 3
	session->http_session->http_session_timeout = 15;

	// set the connect timeout to 2
	session->http_session->http_connect_timeout = 15;

	// if we have a proxy set, override default captcha behavior
	if(!session->http_session->db_populated_proxy_n)
	{
		session->ddg_retry_with_new_proxy_on_captcha = 1;
		session->ddg_retry_on_bad_connect            = 1;
	}
	else
	{

		// set random sleep between 0 and 5 if there are no proxies
		// being used.
		session->http_session->request_time_modulo = 30;

	}


	// return filled out structure
	return session;
}


// Extracts results when called on a valid ddg scrape session.  Will return session->retrieve_results records per
// call.  Returns ERR_SUCCESS if it retrieved a valid set of results.  Returns ERR_FAILURE when there are no more results to extract.
// Second parameter is time between request (for evasion).
BP_ERROR_T BP_RetrieveDdgResult(P_BP_DDGSCRAPE_SESSION session, size_t seconds_between_request)
{

	// ensure session exists
	if(!session)
	{
		printf("\n [!!] No session.");
		return ERR_FAILURE;
	}

	// ensure http session
	if(!session->http_session)
	{
		printf("\n [!!] No http session.");
		return ERR_FAILURE;
	}

	// exit in the case that the search completed successfully
	if(session->search_completed_successfully)
	{
		// printf("\n [!!] Ddg search did not complete successfully");
		return ERR_FAILURE;
	}

	// ensure we have a populated db proxy set
	if(!session->http_session->db_populated_proxy_n)
	{
		return ERR_FAILURE;
	}

	// --------- Format Search Request ---------------------

	// search request will be dynamically generated
	char *search_request = NULL;

	// if there is no search advancement postdata string (nothing to post) we just do a normal
	// basic query.
	if(!session->search_advancement_post_string)
	{

		// set builder size
		size_t size_builder = 0;
		size_builder += bpstrlen(session->ddg_default_search);
		size_builder += bpstrlen(session->search_query);
		size_builder += 512;

		// allocate space for the new request
		search_request = (char *) tq_calloc(session->tq, size_builder, 1, BPLN);

		// encode the query for utf8
		char *encoded_query = BP_UTF8StringEncode(session->search_query, bpstrlen(session->search_query));

		// build the new request url
		snprintf(search_request, size_builder, session->ddg_default_search, session->search_query, encoded_query);

	}
	else
	{

		// set search request here
		search_request = (char *) tq_strdup(session->tq, (char *) bp_ddg_default_search_without_query, BPLN);

		if(memmem((void*) session->search_advancement_post_string, bpstrlen(session->search_advancement_post_string), (void*) "nextParams", bpstrlen("nextParams")))
		{
			// set request post vars
			BP_HTTPSetPostVars(session->http_session, session->search_advancement_post_string);
		}
		else
		{

			printf("\n [+] Search advancement indicates there are no more forward parameter movements.  Returning.");
			return ERR_FAILURE;

		}


	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Request %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// declare session clearer
	size_t clean_session = 0;

	// repeat request label, any number of conditions below can trigger this,
	// but typically only when a request fails.
	repeat_request:

	// attempt to make the actual request
	while(BP_HTTPRequest(session->http_session, search_request, clean_session) != ERR_SUCCESS)
	{

		printf("\n [!!] DdgSubsystem: Could not make search request for %s (retrying w/ new proxy if availabile)", search_request);

	}


	// optional: display connect codes
	printf("\n Got Connect Code: %u - %u", session->http_session->http_connect_code, session->http_session->http_status_code);

	// optional: Display data
	printf("\n\n Data[%u]: %s\n\n", session->http_session->page->page_data_size, session->http_session->page->page_data);




	// try again if we have a 400 result
	if(session->http_session->http_status_code >= 400 && session->http_session->http_status_code < 500)
	{

		// set the clean session indicator
		clean_session = BP_TRUE;

		// go to the repeat label
		goto repeat_request;

	}


	// ensure we have page data size
	if(!session->http_session->page->page_data_size)
	{

		// set the clean session indicator
		clean_session = BP_TRUE;

		// go to the repeat label
		goto repeat_request;

	}

	// a bad ddg request is < 1k in size consistently (legit requests are > 100000 normally)
	if(session->http_session->page->page_data_size < 1000)
	{

		// display debugging message
		printf("\n [!!] DdgSubsystem: Data size too small.  Most likely ddg captcha has activated.  Try enabling the proxy subsystem.  Retrying (with new proxy if available).");

		// retry on too small of a size
		if(!session->ddg_retry_on_too_small_size)
		{
			session->captcha_activated = 1;
			return ERR_FAILURE;
		}

		// set the clean session indicator
		clean_session = BP_TRUE;

		// go to the repeat label
		goto repeat_request;

	}

	// ensure the http status code is 200
	if(session->http_session->http_status_code != 200)
	{

		// set the clean session indicator
		clean_session = BP_TRUE;

		// go to the repeat label
		goto repeat_request;

	}




	// set flags (caseless by default)
	int    regexp_flags    = 0;
	regexp_flags           |= PCRE_MULTILINE;
	regexp_flags           |= PCRE_DOTALL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Generic Regexp Output Checks %%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ensure we hwave strings
	if(!BP_MatchRegularExpressionPCRE(session->http_session->page->page_data, session->http_session->page->page_data_size, ".*DuckDuckGo.*", regexp_flags, 1000, NULL, BP_FALSE))
	{

		// ensure the page results are reasonable and contin the word ddg.
		printf("\n [+] Checked page content for regexp string 'ddg'.  String was not found.  Trying again.");

		// set the clean session indicator
		clean_session = BP_TRUE;

		// go to the repeat label
		goto repeat_request;

	}

	// display the search result after it's done (not before, unless you're debugging)
	printf("\n [+] sr: %s", search_request);


	// --------- Extract Results ---------------------------

	if(session->last_extraction)
	{
		// HTMLDestroyExtraction(session->last_extraction);
		// session->last_extraction = NULL;
	}


	// Decode Request HTML Data
	session->last_extraction = HTMLDecodeBuffer
	(
		session->http_session->page->page_data,
		session->http_session->page->page_data_size,
		search_request,
		NULL,
		NULL,
		HTML_PARSE_NOERROR,
		NULL
	);


	// destroy page data if set (unnecessary)
	if(session->http_session->page)
	{


	}

    // ---------------------------------------
    // Search Parameters

	// static ddg host blacklist
    char * ddg_blacklist[] = {
        	"http://r.search.yahoo.com",
			"/html/",
			"https://duck.co/",
			"http://duck.co/",
            "#",
            "\n"
    };


    // ========== Extract Ddg Search Parameters Before Applying Blacklists ================

    // anchors
    char ** anchors = NULL;
    size_t total_anchor_count = 0;

    // set pointer to last extraction
    P_BP_HTML_FULL_EXTRACTION last_extraction = session->last_extraction;

    // set anchors so we can parse out search elements
    P_BP_HTML_ANCHOR_LIST page_anchors = last_extraction->anchors;

    // iterator for anchor var lists (allows us to find the search session id)
    P_BP_HTML_ANCHOR_VAR_LIST anchor_var_iter = NULL;

    // set forms so we can parse out search items
    P_BP_HTML_FORM_LIST page_forms = last_extraction->forms;


    // this array will be bpreallocated upon discovery of a valid search item
    char ** ddg_search_queries = NULL;

    // used to determine the total number of queries found
    session->total_queries = 0;

    // reset session information before parsing (don't worry about bpfreeing, it's done via garbage collector)
    session->ddg_search_queries            = NULL;
    session->ddg_search_start_positions    = NULL;
    session->total_queries                 = 0;

    // flag that indicates that the current anchor contains a url link (holds data we want)
    size_t current_anchor_contains_url_link = 0;

    // examine page anchors to find session links
    if(page_anchors)
    for
	(
		page_anchors = page_anchors->first;
    	page_anchors;
    	page_anchors = page_anchors->next
    )
    {

    	// indicate that the current anchor contains a url link
    	current_anchor_contains_url_link = 0;

		// since we've found a good link, increment the total query count
		session->total_queries++;

		// bprealloc the search queries with space for the new query
		session->ddg_search_queries = (char **) bprealloc((void *) session->ddg_search_queries, sizeof(char *) * (session->total_queries+1));

		// bpreallocate start positions
		session->ddg_search_start_positions = (size_t *) bprealloc((void *) session->ddg_search_start_positions, sizeof(char *) * (session->total_queries+1));

		// set the query in the search queries array
		session->ddg_search_queries[session->total_queries - 1] = (char *) page_anchors->href;

		// terminate the array
		session->ddg_search_queries[session->total_queries] = NULL;

	}


    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Extract Post Parameters for Next Request %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // walk page forms
    if(page_forms)
    for(page_forms = page_forms->first; page_forms; page_forms = page_forms->next)
    {

    	// attempt to retrieve the action property
    	P_BP_HTML_FORM_PROPERTY_LIST form_action_property = HTMLGetFormPropertyByName(page_forms->properties, "action");
    	if(!form_action_property)
    	{
    		printf("\n Failed to retrieve action property for form.");
    		continue;
    	}


    	// check to ensure that the action is set correctly
    	if(form_action_property->content)
    	{

    		// continue the loop of we have a length mismatch
    		if(form_action_property->cLen != bpstrlen("/html/"))
    			continue;

    		// continue the loop if we have a compare mismatch
    		if(memcmp(form_action_property->content, "/html/", form_action_property->cLen) != 0)
    			continue;

    		// set local form input for iteration
    		P_BP_HTML_INPUT_LIST form_inputs = page_forms->inputs;

    		// create a linkl for building post data
    		P_BP_LINKL post_data_builder = BP_CreateLinkL(".", BP_TRUE);

    		// walk form inputs and generate post parameters
    		for(form_inputs = form_inputs->first; form_inputs; form_inputs = form_inputs->next)
    		{

    			// retrieve name and value inputs
    			P_BP_HTML_INPUT_PROPERTY_LIST name_element  = HTMLGetInputPropertyByNameI(form_inputs->properties, "name");
    			P_BP_HTML_INPUT_PROPERTY_LIST value_element = HTMLGetInputPropertyByNameI(form_inputs->properties, "value");

    			// property must have a name
    			if(!name_element)
    				continue;

    			if(name_element)
    			{
    				printf("\n WOO WOO GOT NAME: %s - %s", name_element->name, name_element->content);



    				tq_strdup(post_data_builder, name_element->content, BPLN);
    			}

    			if(value_element)
    			{

    				// add equal sign
    				tq_strdup(post_data_builder, "=", BPLN);

    				// add utf8 encoded value (this is not created as a string tq entry, it is tq_calloced)
    				char * utf8_encode_str = BP_UTF8StringEncode(value_element->content, value_element->cLen, post_data_builder);

    				// add the encoded string into our string set (for flattening)
    				tq_strdup(post_data_builder, utf8_encode_str, BPLN);

    			}

    			// add ampersign if necessary
    			if(form_inputs->next != NULL)
    				tq_strdup(post_data_builder, "&", BPLN);

    		}

    		// flatten string elements
    		BP_LinkLFlattenJoinStringElements(post_data_builder);

    		// display flat buff
    		printf("\n Got Flat Buff: %s", (char *) post_data_builder->flat_buff);

    		if(session->search_advancement_post_string)
    		{

    			// destroy old data before adding new data
    			tq_free(session->tq, session->search_advancement_post_string);

    			// set the advancement post data
    			session->search_advancement_post_string = tq_strdup(session->tq, (char *) post_data_builder->flat_buff, BPLN);

    		}
    		else
    		{

    			// set the advancement post data
    			session->search_advancement_post_string = tq_strdup(session->tq, (char *) post_data_builder->flat_buff, BPLN);

    		}


    		// destroy the linkl after use
    		BP_DestroyLinkL(post_data_builder, BP_TRUE);


    	}

    }

	// HTMLPrintForms(page_forms, 1);


    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%% Apply URL Blacklist %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // get the size of each element in the structure (requred for blacklist)
    size_t ddg_blacklist_count = sizeof(ddg_blacklist) / sizeof(char *);


    // apply html blacklist here to remove servers from anchors and forms set
    HTMLApplyBlacklist(session->last_extraction, (char **) &ddg_blacklist, ddg_blacklist_count);

    // apply the custom user blacklist
    HTMLApplyBlacklist(session->last_extraction, session->custom_blacklist, session->custom_blacklist_count);


	// build character array of results
	anchors = HTMLGetUrlStringArrayFromAnchors(session->last_extraction->anchors, false, session->last_extraction->gc);
	if(!anchors)
	{
		printf("\n [!!] No anchors, returning failure");
		printf("\n");
		printf("\n");
		return ERR_FAILURE;
	}


	// find the size of the anchors array ( for bpreallocing into struct array )
	size_t i = 0;
	for(i = 0; ; i++)
	{

		if(!anchors)
			break;

		if(!anchors[i])
			break;

		printf("\n [+] search_engine_result [%u]:  %s", i, anchors[i]);

	}

	// --------- Update Structure Values -------------------

	// only update if we have urls to add
	if(i)
	{



		// bprealloc for space
		session->extracted_urls = (char **) bprealloc(session->extracted_urls, ( session->url_count + i ) * (sizeof(char *) + 1));

		// copy anchors into the end of the extracted urls stack
		memcpy(&session->extracted_urls[session->url_count], anchors, sizeof(char *) * i);

		// terminate extracted  urls with null ptr
		session->extracted_urls[session->url_count + i -1] = NULL;

		// increase the extracted url count to reflect the new stack size
		session->url_count += i;

		// set the start at result number
		session->start_at_result_number_xx += i;

	}



	// Sleep between requests if desired.  This is typically only necessary
	// for when we're not using proxy database items.
	if(seconds_between_request)
		sleep(seconds_between_request);

	// exit with success code
	return ERR_SUCCESS;

}


// Destroys an existing ddg session
size_t BP_DestroyDdgSession(P_BP_DDGSCRAPE_SESSION session)
{
	if(!session)
		return ERR_FAILURE;

	if(session->http_session)
		return ERR_FAILURE;

	if(session->gc)
	{
		printf("\n [+] Destroying Ddg Memory.");
		// BP_GarbageCollectorFreeAllChunks(session->gc);

	}

	// now destroy the session
	bpfree(session);
	session = NULL;

	return ERR_FAILURE;

}

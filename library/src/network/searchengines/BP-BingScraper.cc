/*
 * BP-BingScraper.cc
 *
 *  Created on: Sep 6, 2010
 *      Author: root
 */

/*
 * BP-BingScraper.cc
 * -----------------------------------------------
 * This file holds the routines responsible for extracting and storing results from google searches.  It
 * only uses libcurl and libxml for making requests/scraping data.  See src/libtest/bing/ for usage
 * information.
 *
 */


#include "../../../include/BP-Main.h"


// Default search path for bing, change this if bing changes.  Format characters are substituted at run.
const char * bp_bing_default_search = "http://www.bing.com/search?q=%s&go=&qs=ns&first=%d&FORM=PERE";

// this indicates a successful page retrieval
const char * bing_completion_indicator = "Searches related to <em>";

// no more results (secondary request completion indicator)
const char * bing_completion_indicator_2 = ">Next</a></li>";

// captcha flagged indicator
const char * bing_captcha_indicator_1 = "sorry.bing.com/sorry";

// Creates a new bingscrape session out of an existing HTTP_SESSION structure.  Use the functions
// in BP-NetworkWeb.h to create the session before passing it here.  YOU ARE RESPONSIBLE FOR SEARCH INPUT
// FORMATTING IN search_query.
P_BP_BINGSCRAPE_SESSION BP_OpenNewBingSearch(char *search_query, P_BP_HTTP_SESSION http_session)
{

	// check to make sure we have a search pointer
	if(!search_query)
		return NULL;

	// ensure we have a http session pointer
	if(!http_session)
		return NULL;

	// allocate new session structure
	P_BP_BINGSCRAPE_SESSION session = (P_BP_BINGSCRAPE_SESSION) bpcalloc(sizeof(BP_BINGSCRAPE_SESSION), 1);

	// create new garbage collector
	session->gc = BP_CreateNewGarbageCollector("bingscrape collector", 1);

	// --- Fill in Structure Values -------------------

	// set the web request pointer in our bingscrape session
	session->http_session = http_session;

	// duplicate the default search  search string (used as a basis for a sprintf replace later)
	session->bing_default_search = bpstrdup((char *) bp_bing_default_search);

	// set the default number of results to return per query (100 by default, set manually in the structure later if wanted)
	session->retrieve_results = 100;

	// set the search string now
	session->search_query = bpstrdup(search_query);

	// return filled out structure
	return session;
}


// Extracts results when called on a valid bing scrape session.  Will return session->retrieve_results records per
// call.  Returns ERR_SUCCESS if it retrieved a valid set of results.  Returns ERR_FAILURE when there are no more results to extract.
// Second parameter is time between request (for evasion).
BP_ERROR_T BP_RetrieveBingResult(P_BP_BINGSCRAPE_SESSION session, size_t seconds_between_request)
{

	// create pointer for easier use below
	P_BP_GARBAGE_COLLECTOR_DATA gc = session->gc;

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


	if(session->search_completed_successfully)
	{
		printf("\n [!!] Bing search did not complete successfully");
		return ERR_FAILURE;
	}

	// --------- Format Search Request ---------------------

	// set builder size
	size_t size_builder = 0;
	size_builder += bpstrlen(session->bing_default_search);
	size_builder += bpstrlen(session->search_query);
	size_builder += 512;

	// allocate space for the new request
	char *search_request = (char *) BP_calloc(gc, NULL, size_builder, 1);

	// encode the query for utf8
	char *encoded_query = BP_UTF8StringEncode(session->search_query, bpstrlen(session->search_query));

	// log the encoded query in the garbage collector
	BP_GarbageCollectorLogChunk(gc, NULL, encoded_query, bpstrlen(session->search_query), BP_GARBAGE_COLLECTOR_DATA_ALLOC_UNKNOWN);

	// build the new request url
	snprintf(search_request, size_builder, session->bing_default_search, encoded_query, session->start_at_result_number_xx);


	// --------- Run Request -------------------------------

	size_t bing_retries = 0;

	repeat_request:
	while(BP_HTTPRequest(session->http_session, search_request) != ERR_SUCCESS)
	{
		printf("\n [!!] BingSubsystem: Could not make search request for (retrying w/ new proxy if availabile) %s", search_request);
	}

	// a bad bing request is < 1k in size consistently (legit requests are > 100000 normally)
	if(session->http_session->page->page_data_size < 1000)
	{
		printf("\n [!!] BingSubsystem: Data size too small.  Most likely bing captcha has activated.  Try enabling the proxy subsystem.  Retrying (with new proxy if available).");
		bing_retries++;
		if(bing_retries < 10)
			goto repeat_request;
	}

	printf("\n [+] sr (%u): %s", bing_retries, search_request);


	// --------- Extract Results ---------------------------

	if(session->last_extraction)
	{
		// HTMLDestroyExtraction(session->last_extraction);
		// session->last_extraction = NULL;
	}


	// Decode Request HTML Data
	session->last_extraction = HTMLDecodeBuffer(
			session->http_session->page->page_data,
			session->http_session->page->page_data_size,
			search_request,
			NULL,
			NULL,
			HTML_PARSE_NOERROR,
			NULL
	);

	// if the "Next" anchor is not found, mark it as incomplete.
	if(!strstr(session->http_session->page->page_data, bing_completion_indicator_2))
	{
		session->search_completed_successfully = 1;
	}

	if(strstr(session->http_session->page->page_data, bing_captcha_indicator_1))
	{
		printf("\n [!!] Notice! Bing captcha has been activated.  Stopping search.");
		session->captcha_activated = 1;
		return ERR_FAILURE;
	}


	// destroy page data if set
	if(session->http_session->page)
	{


	}

    // ---------------------------------------
    // Search Parameters

    char * bing_blacklist[] = {
            ":bing.com",
			":bingj.com",
            ":msn.com",
            ":microsoft.com",
			":microsofttranslator.com",
            ":microsoft"
            ":live.com",
            "javascript",
            "/",
            "#",
            "n"
    };


    // get the size of each element in the structure (requred for blacklist)
    size_t bing_blacklist_count = sizeof(bing_blacklist) / sizeof(char *);

    // apply html blacklist here to remove servers from anchors and forms set
    HTMLApplyBlacklist(session->last_extraction, (char **) &bing_blacklist, bing_blacklist_count);

    // apply the custom user blacklist
    HTMLApplyBlacklist(session->last_extraction, session->custom_blacklist, session->custom_blacklist_count);

	// build character array of results
	char ** anchors = HTMLGetUrlStringArrayFromAnchors(session->last_extraction->anchors, false, session->last_extraction->gc);
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
		if(!anchors[i])
			break;
	}

	// --------- Update Structure Values -------------------

	// bprealloc for space
	session->extracted_urls = (char **) bprealloc(session->extracted_urls, ( session->url_count + i ) * sizeof(char *) + 1);

	// copy anchors into the end of the extracted urls stack
	memcpy(&session->extracted_urls[session->url_count], anchors, sizeof(char *) * i);

	// terminate extracted  urls with null ptr
	session->extracted_urls[session->url_count + i -1] = NULL;

	// increase the extracted url count to reflect the new stack size
	session->url_count += i;

	// set the start at result number
	session->start_at_result_number_xx += i;

	// --------- Sleep If Set (so we dont get overzealous) ---

	if(seconds_between_request)
		sleep(seconds_between_request);

	// exit with success code
	return ERR_SUCCESS;

}


// Destroys an existing bing session
size_t BP_DestroyBingSession(P_BP_BINGSCRAPE_SESSION session)
{
	if(!session)
		return ERR_FAILURE;

	if(session->http_session)
		return ERR_FAILURE;

	if(session->gc)
	{
		printf("\n [+] Destroying Bing Memory.");
		// BP_GarbageCollectorFreeAllChunks(session->gc);

	}

	// now destroy the session
	bpfree(session);
	session = NULL;

	return ERR_FAILURE;

}


/*
 * BP-BingScraper.h
 * -----------------------------------------------
 * This file holds the definitions for routines responsible for extracting and storing results from bing searches.  It
 * only uses libcurl and libxml for making requests/scraping data.  See src/libtest/bing/ for usage
 * information.
 *
 */
#ifndef BP_BINGSCRAPER_H_
#define BP_BINGSCRAPER_H_


// ------------ Structure Definitions ------------------------

// This structure is the primary bing scraping
typedef struct _BP_BINGSCRAPE_SESSION {

	// http session to be used when making bing requests
	P_BP_HTTP_SESSION http_session;

	// base string to use for making searches (const char *bp_bing_default_search in BP-BingScraper.cc) (strdup'd must be freed)
	char *bing_default_search;

	// user provided adv_search string (eg. inurl:php site:gs.com), (strdup'd, must be free'd)
	char *search_query;

	// number of results to return per "search instance" Must be between 10 and 100 (default 100)
	size_t retrieve_results;

	// current start at result number xx (default start is 0, it is incremented by this->retrieve_results
	// each time BP_RetrieveBingResult() is called.
	size_t start_at_result_number_xx;

	// set to true if the search has exausted all records without hitting captcha screen
	size_t search_completed_successfully;

	// captcha bot detction
	size_t captcha_activated;

	// ------------- Custom Blacklisting -----------------------------0

	// custom blacklist applied at runtime (NULL if not set)
	char ** custom_blacklist;

	// custom blacklist count
	size_t custom_blacklist_count;

	// ------------- Result URL's -------------------------------------

	// urls extracted from the bing search (realloced and refilled on search page advance) (calloc/realloc)
	char ** extracted_urls;

	// counter for how many urls are in the url array
	size_t url_count;


	// ------------ Internal Structures for Decode --------------------

	// last full html extraction (destroyed/rebuilt on each call to BP_RetrieveBingResult())
	P_BP_HTML_FULL_EXTRACTION last_extraction;

	// garbage collector
	P_BP_GARBAGE_COLLECTOR_DATA gc;

} BP_BINGSCRAPE_SESSION, *P_BP_BINGSCRAPE_SESSION;



// ------------ Function Definitions ------------------------



// Creates a new bingscrape session out of an existing HTTP_SESSION structure.  Use the functions
// in BP-NetworkWeb.h to create the session before passing it here.
P_BP_BINGSCRAPE_SESSION BP_OpenNewBingSearch(char *search_query, P_BP_HTTP_SESSION http_session);

// Extracts results when called on a valid bing scrape session.  Will return session->retrieve_results records per
// call.  Returns ERR_SUCCESS if it retrieved a valid set of results.  Returns ERR_FAILURE when there are no more results to extract.
// Second parameter is time between request (for evasion).
BP_ERROR_T BP_RetrieveBingResult(P_BP_BINGSCRAPE_SESSION session, size_t seconds_between_request);

// Destroys an existing bing session
size_t BP_DestroyBingSession(P_BP_BINGSCRAPE_SESSION session);

#endif /* BP_BINGSCRAPER_H_ */

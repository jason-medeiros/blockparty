
/*
 * BP-GoogleScraper.h
 * -----------------------------------------------
 * This file holds the definitions for routines responsible for extracting and storing results from google searches.  It
 * only uses libcurl and libxml for making requests/scraping data.  See src/libtest/google/ for usage
 * information.
 *
 */
#ifndef BP_GOOGLESCRAPER_H_
#define BP_GOOGLESCRAPER_H_


// ------------ Structure Definitions ------------------------



// This structure is the primary google scraping
typedef struct _BP_GOOGLESCRAPE_SESSION {

	// http session to be used when making google requests
	P_BP_HTTP_SESSION http_session;

	// base string to use for making searches (const char *bp_google_default_search in BP-GoogleScraper.cc) (strdup'd must be freed)
	char *google_default_search;

	// user provided adv_search string (eg. inurl:php site:gs.com), (strdup'd, must be free'd)
	char *search_query;


	// ==== New On Jan 23, 2013, added Session Number Identifying Search Queries =================

	/*
	 * Google has added session identifiers into their search requests so now we have to parse out actual search
	 * items (anchors) and store them directly rather than building them ourself.  This is new as of january 2013.
	 */

	// this is populated automatically every time a search is made (due to the session key changing each time)
	char ** google_search_queries;

	// allocated set of google search start positions
	size_t * google_search_start_positions;

	// count of elements in the search query stack
	size_t  total_queries;

	// this is the current value of the anchor "start" variable.  This lets us figure out what the next link is.
	size_t  last_starting_point;


	// ==== End New / Experimental Code ==========================================================

	// number of results to return per "search instance" Must be between 10 and 100 (default 100)
	size_t retrieve_results;

	// current start at result number xx (default start is 0, it is incremented by this->retrieve_results
	// each time BP_RetrieveGoogleResult() is called.
	size_t start_at_result_number_xx;

	// set to true if the search has exausted all records without hitting captcha screen
	size_t search_completed_successfully;

	// captcha bot detction
	size_t captcha_activated;

	// retry with new proxy on captcha
	size_t google_retry_with_new_proxy_on_captcha;

	// flag to indicate behavior on a return with too small size (1 == retry indefinitely, 0 == exit with status ERR_FAILURE)
	size_t google_retry_on_too_small_size;

	// flag indicating whether or not to exit on bad connection  (1 == retry, 0 == exit)
	size_t google_retry_on_bad_connect;

	// ------------- Custom Blacklisting -----------------------------0

	// custom blacklist applied at runtime (NULL if not set)
	char ** custom_blacklist;

	// custom blacklist count
	size_t custom_blacklist_count;

	// ------------- Result URL's -------------------------------------

	// urls extracted from the google search (realloced and refilled on search page advance) (calloc/realloc)
	char ** extracted_urls;

	// counter for how many urls are in the url array
	size_t url_count;


	// ------------ Internal Structures for Decode --------------------

	// last full html extraction (destroyed/rebuilt on each call to BP_RetrieveGoogleResult())
	P_BP_HTML_FULL_EXTRACTION last_extraction;

	// garbage collector
	P_BP_GARBAGE_COLLECTOR_DATA gc;

} BP_GOOGLESCRAPE_SESSION, *P_BP_GOOGLESCRAPE_SESSION;



// ------------ Function Definitions ------------------------



// Creates a new googlescrape session out of an existing HTTP_SESSION structure.  Use the functions
// in BP-NetworkWeb.h to create the session before passing it here.
P_BP_GOOGLESCRAPE_SESSION BP_OpenNewGoogleSearch(char *search_query, P_BP_HTTP_SESSION http_session);

// Extracts results when called on a valid google scrape session.  Will return session->retrieve_results records per
// call.  Returns ERR_SUCCESS if it retrieved a valid set of results.  Returns ERR_FAILURE when there are no more results to extract.
// Second parameter is time between request (for evasion).
BP_ERROR_T BP_RetrieveGoogleResult(P_BP_GOOGLESCRAPE_SESSION session, size_t seconds_between_request);

// Destroys an existing google session
size_t BP_DestroyGoogleSession(P_BP_GOOGLESCRAPE_SESSION session);

#endif /* BP_GOOGLESCRAPER_H_ */

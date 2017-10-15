/*
 * BP-GoogleScraper.cc
 * -----------------------------------------------
 * This file holds the routines responsible for extracting and storing results from google searches.  It
 * only uses libcurl and libxml for making requests/scraping data.  See src/libtest/google/ for usage
 * information.
 *
 */


#include "../../../include/BP-Main.h"


// Default search path for google, change this if google changes.  Format characters are substituted at run.
// REMOVED &hl=en when we added TLD jumping.
const char * bp_google_default_search = "http://www.google%s/search?start=%d&as_q=%s&tbo=d&lr=&filter=0";

// /search?q=hello&hl=en&lr=&tbo=d&as_qdr=all&start=20&sa=N
// this indicates a successful page retrieval
const char * google_completion_indicator = "Searches related to <em>";

// no more results (secondary request completion indicator)
const char * google_completion_indicator_2 = "Next</span>";

// captcha flagged indicator
const char * google_captcha_indicator_1 = "google.com/sorry";


// these are the google domains found within https://www.google.com/supported_domains
char * google_valid_tlds[] =
{
		".com",
		".ad",
		".ae",
		".com.af",
		".com.ag",
		".com.ai",
		".al",
		".am",
		".co.ao",
		".com.ar",
		".as",
		".at",
		".com.au",
		".az",
		".ba",
		".com.bd",
		".be",
		".bf",
		".bg",
		".com.bh",
		".bi",
		".bj",
		".com.bn",
		".com.bo",
		".com.br",
		".bs",
		".bt",
		".co.bw",
		".by",
		".com.bz",
		".ca",
		".cd",
		".cf",
		".cg",
		".ch",
		".ci",
		".co.ck",
		".cl",
		".cm",
		".cn",
		".com.co",
		".co.cr",
		".com.cu",
		".cv",
		".com.cy",
		".cz",
		".de",
		".dj",
		".dk",
		".dm",
		".com.do",
		".dz",
		".com.ec",
		".ee",
		".com.eg",
		".es",
		".com.et",
		".fi",
		".com.fj",
		".fm",
		".fr",
		".ga",
		".ge",
		".gg",
		".com.gh",
		".com.gi",
		".gl",
		".gm",
		".gp",
		".gr",
		".com.gt",
		".gy",
		".com.hk",
		".hn",
		".hr",
		".ht",
		".hu",
		".co.id",
		".ie",
		".co.il",
		".im",
		".co.in",
		".iq",
		".is",
		".it",
		".je",
		".com.jm",
		".jo",
		".co.jp",
		".co.ke",
		".com.kh",
		".ki",
		".kg",
		".co.kr",
		".com.kw",
		".kz",
		".la",
		".com.lb",
		".li",
		".lk",
		".co.ls",
		".lt",
		".lu",
		".lv",
		".com.ly",
		".co.ma",
		".md",
		".me",
		".mg",
		".mk",
		".ml",
		".com.mm",
		".mn",
		".ms",
		".com.mt",
		".mu",
		".mv",
		".mw",
		".com.mx",
		".com.my",
		".co.mz",
		".com.na",
		".com.nf",
		".com.ng",
		".com.ni",
		".ne",
		".nl",
		".no",
		".com.np",
		".nr",
		".nu",
		".co.nz",
		".com.om",
		".com.pa",
		".com.pe",
		".com.pg",
		".com.ph",
		".com.pk",
		".pl",
		".pn",
		".com.pr",
		".ps",
		".pt",
		".com.py",
		".com.qa",
		".ro",
		".ru",
		".rw",
		".com.sa",
		".com.sb",
		".sc",
		".se",
		".com.sg",
		".sh",
		".si",
		".sk",
		".com.sl",
		".sn",
		".so",
		".sm",
		".sr",
		".st",
		".com.sv",
		".td",
		".tg",
		".co.th",
		".com.tj",
		".tk",
		".tl",
		".tm",
		".tn",
		".to",
		".com.tr",
		".tt",
		".com.tw",
		".co.tz",
		".com.ua",
		".co.ug",
		".co.uk",
		".com.uy",
		".co.uz",
		".com.vc",
		".co.ve",
		".vg",
		".co.vi",
		".com.vn",
		".vu",
		".ws",
		".rs",
		".co.za",
		".co.zm",
		".co.zw",
		".cat",
		NULL
};




// Creates a new googlescrape session out of an existing HTTP_SESSION structure.  Use the functions
// in BP-NetworkWeb.h to create the session before passing it here.  YOU ARE RESPONSIBLE FOR SEARCH INPUT
// FORMATTING IN search_query.
P_BP_GOOGLESCRAPE_SESSION BP_OpenNewGoogleSearch(char *search_query, P_BP_HTTP_SESSION http_session)
{

	// check to make sure we have a search pointer
	if(!search_query)
		return NULL;

	// ensure we have a http session pointer
	if(!http_session)
		return NULL;

	// allocate new session structure
	P_BP_GOOGLESCRAPE_SESSION session = (P_BP_GOOGLESCRAPE_SESSION) bpcalloc(sizeof(BP_GOOGLESCRAPE_SESSION), 1);

	// create new garbage collector
	// session->gc = BP_CreateNewGarbageCollector("googlescrape collector", 1);

	// --- Fill in Structure Values -------------------

	// set the web request pointer in our googlescrape session
	session->http_session = http_session;

	// duplicate the default search  search string (used as a basis for a sprintf replace later)
	session->google_default_search = bpstrdup((char *) bp_google_default_search);

	// set the default number of results to return per query (100 by default, set manually in the structure later if wanted)
	session->retrieve_results = 100;

	// set the search string now
	session->search_query = bpstrdup(search_query);

	// set the session timeout to 3
	session->http_session->http_session_timeout = 2;

	// set the connect timeout to 2
	session->http_session->http_connect_timeout = 2;

	// if we have a proxy set, override default captcha behavior
	if(!session->http_session->db_populated_proxy_n)
	{
		session->google_retry_with_new_proxy_on_captcha = 1;
		session->google_retry_on_bad_connect            = 1;
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


// Extracts results when called on a valid google scrape session.  Will return session->retrieve_results records per
// call.  Returns ERR_SUCCESS if it retrieved a valid set of results.  Returns ERR_FAILURE when there are no more results to extract.
// Second parameter is time between request (for evasion).
BP_ERROR_T BP_RetrieveGoogleResult(P_BP_GOOGLESCRAPE_SESSION session, size_t seconds_between_request)
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
		// printf("\n [!!] Google search did not complete successfully");
		return ERR_FAILURE;
	}

	if(!session->http_session->db_populated_proxy_n)
	{

		// set random sleep between 0 and 5 if there are no proxies
		// being used.
		session->http_session->request_time_modulo = 30;

	}

	// --------- Format Search Request ---------------------


	// count valid tlds
	size_t valid_tld_n = BP_CountNullTerminatedPtrArray(google_valid_tlds);

	// set top level index randomly
	size_t tld_index = BP_GenRandomUnsignedLong(0, valid_tld_n );

	// set domain
	char * random_google_domain = google_valid_tlds[BP_GenRandomUnsignedLong(0, valid_tld_n )];

	// set builder size
	size_t size_builder = 0;
	size_builder += bpstrlen(session->google_default_search);
	size_builder += bpstrlen(session->search_query);
	size_builder += bpstrlen(random_google_domain);
	size_builder += 512;

	// allocate space for the new request
	char *search_request = (char *) bpcalloc(size_builder, 1);

	// encode the query for utf8
	char *encoded_query = BP_UTF8StringEncode(session->search_query, bpstrlen(session->search_query));

	// build the new request url
	snprintf(search_request, size_builder, session->google_default_search, random_google_domain, session->start_at_result_number_xx, encoded_query);

	// alternate search run in the case of a dynamic search link from google
	char alternate_search[1024] = {0};


	// --------- Run Request -------------------------------

	size_t j = 0;
	if(session->google_search_queries)
	{
		if(!session->last_starting_point)
		{
			random_google_domain = google_valid_tlds[BP_GenRandomUnsignedLong(0, valid_tld_n )];
			session->last_starting_point = session->google_search_start_positions[0];
			snprintf(alternate_search, sizeof(alternate_search)-1, "http://www.google%s%s", random_google_domain, session->google_search_queries[0]);
			search_request = (char *) &alternate_search;

		}
		else
		{

			for(; j < session->total_queries; j++)
			{

				if(session->google_search_start_positions[j] > session->last_starting_point)
				{
					random_google_domain = google_valid_tlds[BP_GenRandomUnsignedLong(0, valid_tld_n )];
					session->last_starting_point = session->google_search_start_positions[j];
					snprintf(alternate_search, sizeof(alternate_search)-1, "http://www.google%s%s", random_google_domain, session->google_search_queries[j]);
					search_request = (char *) &alternate_search;
					break;

				}

			}

		}

	}

	// declare session clearer
	size_t clean_session = 0;

	// repeat request label, any number of conditions below can trigger this,
	// but typically only when a request fails.
	repeat_request:

	// attempt to make the actual request
	while(BP_HTTPRequest(session->http_session, search_request, clean_session) != ERR_SUCCESS)
	{

		printf("\n [!!] GoogleSubsystem: Could not make search request for %s (retrying w/ new proxy if availabile)", search_request);
		if(!session->google_retry_on_bad_connect)
		{
			return ERR_FAILURE;
		}


		if(!session->http_session->db_populated_proxy_n)
		{

			// set random sleep between 0 and 5 if there are no proxies
			// being used.
			session->http_session->request_time_modulo = 30;

		}

		// update random top level domain
		random_google_domain = google_valid_tlds[BP_GenRandomUnsignedLong(0, valid_tld_n )];

		// if we have search queries already update from there
		if(session->google_search_queries)
		{

			// change google tld on a bad request update
			snprintf(alternate_search, sizeof(alternate_search)-1, "http://www.google%s%s", random_google_domain, session->google_search_queries[j]);
			search_request = (char *) &alternate_search;

		}
		else
		{

			// zero string before new request
			memset(search_request, 0x00, size_builder);

			// build the new
			snprintf(search_request, size_builder, session->google_default_search, random_google_domain, session->start_at_result_number_xx, encoded_query);

		}

	}


	printf("\n Got Connect Code: %u - %u", session->http_session->http_connect_code, session->http_session->http_status_code);
	printf("\n\n Data[%u]: %s\n\n", session->http_session->page->page_data_size, session->http_session->page->page_data);




	// check for captcha indicator 1
	if(strstr(session->http_session->page->page_data, google_captcha_indicator_1))
	{

		printf("\n [!!] Notice! Google captcha has been activated.  Retrying request (shuffles proxy choice).");

		// set the clean session indicator
		clean_session = BP_TRUE;

		// go to the repeat label
		goto repeat_request;

	}

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

	// a bad google request is < 1k in size consistently (legit requests are > 100000 normally)
	if(session->http_session->page->page_data_size < 1000)
	{

		// display debugging message
		printf("\n [!!] GoogleSubsystem: Data size too small.  Most likely google captcha has activated.  Try enabling the proxy subsystem.  Retrying (with new proxy if available).");

		// retry on too small of a size
		if(!session->google_retry_on_too_small_size)
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
	if(!BP_MatchRegularExpressionPCRE(session->http_session->page->page_data, session->http_session->page->page_data_size, ".*google.*", regexp_flags, 1000, NULL, BP_FALSE))
	{

		// ensure the page results are reasonable and contin the word google.
		printf("\n [+] Checked page content for regexp string 'google'.  String was not found.  Trying again.");

		// set the clean session indicator
		clean_session = BP_TRUE;

		// go to the repeat label
		goto repeat_request;

	}

	// ensure this isn't the framebusting version of google results
	if(BP_MatchRegularExpressionPCRE(session->http_session->page->page_data, session->http_session->page->page_data_size, ".*lick here to proceed.*", regexp_flags, 1000, NULL, BP_FALSE))
	{

		// ensure the page results are reasonable and contin the word google.
		printf("\n [+] Checked for click to proceed string failed (string was found), repeating request with diff proxy/tld selected.");

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


	// check for captcha indicator 2
	if(!strstr(session->http_session->page->page_data, google_completion_indicator_2))
	{
		session->search_completed_successfully = 1;
	}

	// check for captcha indicator 1
	if(strstr(session->http_session->page->page_data, google_captcha_indicator_1))
	{

		if(session->google_retry_with_new_proxy_on_captcha)
		{

			// display brute force item
			printf("\n [!!] Captcha encountered, brute forcing forward.");

			// set the clean session indicator
			clean_session = BP_TRUE;

			// go to the repeat label
			goto repeat_request;

		}

		printf("\n [!!] Notice! Google captcha has been activated and brute force flag is disabled, exiting here.");

		session->captcha_activated = 1;
		return ERR_FAILURE;
	}


	// destroy page data if set (unnecessary)
	if(session->http_session->page)
	{


	}

    // ---------------------------------------
    // Search Parameters

	// static google host blacklist
    char * google_blacklist[] = {
            "http://www.google.com",
            "http://webcache.googleusercontent.com",
            "https://webcache.googleusercontent.com",
            "/url?q=http://webcache.googleusercontent.com",
            "http://webcache.google.com",
            "https://webcache.google.com",
            "http://google.com",
            "https://www.google.com",
            "http://mail.google.com",
            "https://mail.google.com",
            "http://drive.google.com",
            "https://drive.google.com",
            "http://wallet.google.com",
            "https://wallet.google.com",
            "http://picasaweb.google.com",
            "https://picasaweb.google.com",
            "http://plus.google.com",
            "https://plus.google.com",
            "http://play.google.com",
            "https://play.google.com",
            "http://accounts.google.com",
            "https://accounts.google.com",
            "http://webcache.googleusercontent.com",
            "http://maps.google.com",
            "http://docs.google.com",
            "http://news.google.com",
            "http://translate.google.com",
            "http://scholar.google.com",
            "http://books.google.com/",
            "#",
            "\n"
    };


    // ========== Extract Google Search Parameters Before Applying Blacklists ================

    // anchors
    char ** anchors = NULL;
    size_t total_anchor_count = 0;

    // set pointer to last extraction
    P_BP_HTML_FULL_EXTRACTION last_extraction = session->last_extraction;

    // set anchors so we can parse out search elements
    P_BP_HTML_ANCHOR_LIST page_anchors = last_extraction->anchors;

    // iterator for anchor var lists (allows us to find the search session id)
    P_BP_HTML_ANCHOR_VAR_LIST anchor_var_iter = NULL;

    // this array will be bpreallocated upon discovery of a valid search item
    char ** google_search_queries = NULL;

    // used to determine the total number of queries found
    session->total_queries = 0;

    // reset session information before parsing (don't worry about bpfreeing, it's done via garbage collector)
    session->google_search_queries         = NULL;
    session->google_search_start_positions = NULL;
    session->total_queries                 = 0;

    // flag that indicates that the current anchor contains a url link (holds data we want)
    size_t current_anchor_contains_url_link = 0;

    // examine page anchors to find session links
    if(page_anchors)
    for(page_anchors = page_anchors->first; page_anchors; page_anchors = page_anchors->next)
    {

    	// indicate that the current anchor contains a url link
    	current_anchor_contains_url_link = 0;

    	// save urls as a pointer array
    	if(page_anchors->href)
    	if(memcmp((void *) page_anchors->href, "/url", 4) == 0)
    	{
    		current_anchor_contains_url_link = BP_TRUE;
    	}


    	if(page_anchors->href)
    	if(memcmp((void *) page_anchors->href, (void *) "/url?q=http://webcache.googleusercontent.com", bpstrlen("/url?q=http://webcache.googleusercontent.com")))
    	if(
    			(memcmp((void *) page_anchors->href,    (void *)  "/search", bpstrlen("/search")) == 0) ||
    			(memcmp((void *) page_anchors->href,    (void *)  "/url",    bpstrlen("/url")) == 0)
    	)
    	{

    		// set the var iter from the vars encoded in the anchor
    		anchor_var_iter = page_anchors->vars;

    		// if this anchor has no vars, we've found an invalid record and we can loop
    		if(!anchor_var_iter)
    			continue;

    		// set flag here to indicate the link is bad by default (assigned in loop to true/false)
    		size_t this_link_is_a_valid_search_link = 0;
    		size_t valid_search_link_start_position = 0;

    		// iterate through the anchor vars and isolate the ei (google session) variable
    		for(anchor_var_iter = anchor_var_iter->first; anchor_var_iter; anchor_var_iter = anchor_var_iter->next)
    		{

    			if(current_anchor_contains_url_link)
    			if(memcmp((void *) anchor_var_iter->name, "q", 1) == 0)
    			{
    				total_anchor_count++;
    				anchors = (char **) bprealloc(anchors, (total_anchor_count+1) * sizeof(char *));

    				char * url_decoded_content = BP_URLDecode(anchor_var_iter->content);

    				// duplicate the string
    				anchors[total_anchor_count-1] = url_decoded_content;
    				//printf("\n current anchor (%u) : %s", total_anchor_count, anchors[total_anchor_count-1]);
    				anchors[total_anchor_count]   = NULL;

    			}


    			// valid links must have start positions
    			if(memcmp(anchor_var_iter->name, "start", 5)  == 0)
    			{
    				if(!anchor_var_iter->content)
    					continue;

    				valid_search_link_start_position = strtoul(anchor_var_iter->content, NULL, 10);
    				this_link_is_a_valid_search_link++;
    			}

    			// if the query variable matches our query, we are looking good
    			if(memcmp(anchor_var_iter->name, "q", 1)  == 0)
    			if(bpstrlen(anchor_var_iter->content) == bpstrlen(session->search_query))
    			if(memcmp(anchor_var_iter->content, session->search_query, strnlen(session->search_query, 256)) == 0)
    				this_link_is_a_valid_search_link++;

    			// check if we have an ei variable (increment in this case)
    			if(memcmp(anchor_var_iter->name, "ei", 2) == 0)
    				this_link_is_a_valid_search_link++;

    		}

    		// continue the loop if we have a bad search link
    		if(this_link_is_a_valid_search_link != 3)
    			continue;

    		// since we've found a good link, increment the total query count
    		session->total_queries++;

    		// bprealloc the search queries with space for the new query
    		session->google_search_queries = (char **) bprealloc((void *) session->google_search_queries, sizeof(char *) * (session->total_queries+1));

    		// bpreallocate start positions
    		session->google_search_start_positions = (size_t *) bprealloc((void *) session->google_search_start_positions, sizeof(char *) * (session->total_queries+1));

    		// set the query in the search queries array
    		session->google_search_queries[session->total_queries - 1] = (char *) page_anchors->href;

    		// set the start position
    		session->google_search_start_positions[session->total_queries - 1] = valid_search_link_start_position;

    		// terminate the array
    		session->google_search_queries[session->total_queries] = NULL;

    	}

    }

    // debug messages
    size_t n = 0;
    for(; n < session->total_queries; n++)
    {
    	//printf("\n [+] Got query!: http://www.google.com%s", session->google_search_queries[n]);
    	//printf("\n [+] Got Index For query: %u", session->google_search_start_positions[n]);
    }

    // ============


    // ============ Apply Blacklists ================================================

    // get the size of each element in the structure (requred for blacklist)
    size_t google_blacklist_count = sizeof(google_blacklist) / sizeof(char *);


    // apply html blacklist here to remove servers from anchors and forms set
    HTMLApplyBlacklist(session->last_extraction, (char **) &google_blacklist, google_blacklist_count);

    // apply the custom user blacklist
    HTMLApplyBlacklist(session->last_extraction, session->custom_blacklist, session->custom_blacklist_count);

    /*
	// build character array of results
	char ** anchors = HTMLGetUrlStringArrayFromAnchors(session->last_extraction->anchors, false, session->last_extraction->gc);
	if(!anchors)
	{
		printf("\n [!!] No anchors, returning failure");
		printf("\n");
		printf("\n");
		return ERR_FAILURE;
	}
	*/

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


// Destroys an existing google session
size_t BP_DestroyGoogleSession(P_BP_GOOGLESCRAPE_SESSION session)
{
	if(!session)
		return ERR_FAILURE;

	if(session->http_session)
		return ERR_FAILURE;

	if(session->gc)
	{
		printf("\n [+] Destroying Google Memory.");
		// BP_GarbageCollectorFreeAllChunks(session->gc);

	}

	// now destroy the session
	bpfree(session);
	session = NULL;

	return ERR_FAILURE;

}

/*
 * BP-DisconnectMeScraper.h
 *
 *  Created on: Aug 21, 2015
 *      Author: root
 */

#ifndef INCLUDE_NETWORK_SEARCHENGINES_BP_DISCONNECTMESCRAPER_H_
#define INCLUDE_NETWORK_SEARCHENGINES_BP_DISCONNECTMESCRAPER_H_


// NOTICE: This is the first search engine scraper that was
//         developed specifically for tor.  It is also the first to
//         be using the blockparty hash table registry functionality
//         for data managment.
//

typedef enum
{
	BP_DISCONNECT_ME_SEARCH_TARGET_UNSET,
	BP_DISCONNECT_ME_SEARCH_TARGET_GOOGLE,
	BP_DISCONNECT_ME_SEARCH_TARGET_YAHOO,
	BP_DISCONNECT_ME_SEARCH_TARGET_BING,
	BP_DISCONNECT_ME_SEARCH_TARGET_DUCK_DUCK_GO
} BP_DISCONNECT_ME_SEARCH_TARGET;

// This session is used to search the disconnect.me service.
typedef struct __BP_DISCONNECTME_SCRAPE_SESSION {


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Hash Registries %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// System hash registry, used for generic allocations within
	// the session.  This also holds the BP_DISCONNECTME_SCRAPE_SESSION
	// itself.
	P_BP_HASH_TABLE_REGISTRY system_hreg;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Query Defaults %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Session Parameters %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// search target to use for this session
	BP_DISCONNECT_ME_SEARCH_TARGET search_target;

	// http session (passed in from BP_DisconnectMeSearchOpen.  Does not
	// get freed during close)
	P_BP_HTTP_SESSION http_session;

	// total number of results in the registry
	size_t total_results_retrieved;



} BP_DISCONNECTME_SCRAPE_SESSION, *P_BP_DISCONNECTME_SCRAPE_SESSION;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Open and Close Sessions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Open Session
P_BP_DISCONNECTME_SCRAPE_SESSION BP_DisconnectMeSearchOpen(P_BP_HTTP_SESSION http_session);

// close the search and destroy all records
BP_ERROR_T BP_DisconnectMeSearchClose(P_BP_DISCONNECTME_SCRAPE_SESSION dmss);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Search Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Search for a query for a provided search target.
BP_ERROR_T BP_DisconnectMeSearch
(
		P_BP_DISCONNECTME_SCRAPE_SESSION dmss,
		char *search_query,
		BP_DISCONNECT_ME_SEARCH_TARGET search_target,
		size_t max_dupes_before_exit
);

// Attempt to pregenerate a stack of queries for retrieval.
BP_ERROR_T BP_DisconnectMePreGenQueries
(
		P_BP_DISCONNECTME_SCRAPE_SESSION dmss,
		char *search_query,
		BP_DISCONNECT_ME_SEARCH_TARGET search_target
);

#endif /* INCLUDE_NETWORK_SEARCHENGINES_BP_DISCONNECTMESCRAPER_H_ */

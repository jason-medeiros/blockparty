/*
 * BP-DisconnectMeScraper.c
 *
 *  Created on: Aug 21, 2015
 *      Author: root
 */


#include "../../../include/BP-Main.h"

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Open and Close Sessions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Open Session
P_BP_DISCONNECTME_SCRAPE_SESSION BP_DisconnectMeSearchOpen(P_BP_HTTP_SESSION http_session)
{

	// ensure we have a correct session
	if(!http_session)
		return NULL;

	// create new system hash table registry
	P_BP_HASH_TABLE_REGISTRY system_hreg = BP_CreateHashTableRegistry();

	// create new scrape session with table registry
	P_BP_DISCONNECTME_SCRAPE_SESSION dmss = (P_BP_DISCONNECTME_SCRAPE_SESSION) ht_calloc(system_hreg, sizeof(BP_DISCONNECTME_SCRAPE_SESSION), 1);

	// set system hash registry
	dmss->system_hreg = system_hreg;

	// set httpsession
	dmss->http_session = http_session;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Add Default Values to HREG %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// -- add search queries table -------

	// set add path
	char * add_path[10] = {0};

	BP_ZERO_STACK(add_path);
	add_path[0] = "search_queries";
	add_path[1] = NULL;

	// add search results table
	BP_HashRegAddTableByPath(dmss->system_hreg, (char **) &add_path);


	// -- add search results table --------


	// set add path
	add_path[0] = "search_results";
	add_path[1] = NULL;

	// add search results table
	BP_HashRegAddTableByPath(dmss->system_hreg, (char **) &add_path);



	// BP_HashRegAddDataToTableByPath()
	// return the disconnect me scrape session
	return dmss;

}

// close the search and destroy all records
BP_ERROR_T BP_DisconnectMeSearchClose(P_BP_DISCONNECTME_SCRAPE_SESSION dmss)
{

	// destroying the system registry destroys all associated memory (except for http session)
	BP_DestroyHashTableRegistry(dmss->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}

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
)
{

	if(!dmss)
		return ERR_FAILURE;
	if(!search_query)
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(search_query, bpstrlen(search_query), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;
	if(!search_target)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Begin Search %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Attempt to pregenerate a stack of queries for retrieval.
	BP_DisconnectMePreGenQueries
	(
			dmss,
			search_query,
			search_target
	);

	// create lookup table and get queries table
	char * lookup_table[10];
	lookup_table[0] = "search_queries";
	lookup_table[1] = NULL;

	// lookup search queries
	P_BP_HASH_TABLE_KEYED_ENTRY queries_table = BP_HashRegLookupTableByPath(dmss->system_hreg, (char **) &lookup_table);
	if(!queries_table)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(dmss->system_hreg, ERR_FAILURE);
	if(!queries_table->data)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(dmss->system_hreg, ERR_FAILURE);


	// overwrite old lookup with new search results lookup
	lookup_table[0] = "search_results";
	lookup_table[1] = NULL;

	// lookup results table
	P_BP_HASH_TABLE_KEYED_ENTRY query_results_table = BP_HashRegLookupTableByPath(dmss->system_hreg, (char **) &lookup_table);
	if(!query_results_table)
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(dmss->system_hreg, ERR_FAILURE);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Queries and Store Results %%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// add user agent
	BP_HTTPAddRequestHeader
	(
			dmss->http_session,
			"User-Agent",
			"Header Descriptionh",
			"Mozilla Firefox 25.0.1 (Build 15r)",
			BP_TRUE
	);


	// current record index
	size_t curr_record_idx = 0;

	// if we found the end of our records, set this here
	BP_BOOL found_end_of_records = BP_FALSE;

	// total number of duplicates encountered
	size_t total_dupe_count = 0;

	// total number of duplicates which can be encountered before we force exit
	size_t total_dupes_before_exit = 40;

	// set max dupes before exit
	if(max_dupes_before_exit)
		total_dupes_before_exit = max_dupes_before_exit;

	// iterate through queries
	BP_HASH_ITER_OPEN((P_BP_HASH_TABLE_KEYED_ENTRY) queries_table->data, query_iter)
	{

		// printf("\n Search Query: %s - %s", (char *) query_iter->key, (char *) query_iter->data);

		// declare session clearer
		size_t clean_session = 1;


		// attempt to make the actual request
		while(BP_HTTPRequest(dmss->http_session, (char *) query_iter->data, clean_session) != ERR_SUCCESS)
		{

		}

		// ensure we have data
		if(!dmss->http_session->page->page_data)
			continue;

		// Decode Request HTML Data
		P_BP_HTML_FULL_EXTRACTION html_extraction = HTMLDecodeBuffer
		(
				dmss->http_session->page->page_data,
				dmss->http_session->page->page_data_size,
				(char *) query_iter->data,
				NULL,
				NULL,
				HTML_PARSE_NOERROR,
				NULL
		);


		// ensure we have a html extraction
		if(!html_extraction)
			continue;

		// ensure we parsed anchors
		if(!html_extraction->anchors)
			continue;

		// print out anchors
		// HTMLPrintAnchors(html_extraction->anchors, BP_TRUE);

		// set anchor iterator
		size_t iter_n = 0;

		// static host blacklist applied to anchors
		char * static_host_blacklist[] =
		{
				"/",
				"./",
				"https://www.google.com",
				"https://disconnect.me",
				"https://duckduckgo.com",
				"https://books.google.com",
				"http://books.google.com",
				NULL
		};


		printf("\n Displaying anchors here?!!");

		// this is used to detect when we run out of results
		char * anchor_last_preserved = NULL;

		// this is set to the number of records that actually were filtered from search
		size_t anchor_retrieved_that_passed_filter_count = 0;

		// anchor iterator
		P_BP_HTML_ANCHOR_LIST anchor_iter = html_extraction->anchors->first;
		for
		(
				;
				anchor_iter;
				anchor_iter = anchor_iter->next

		)
		{



			// increment the iter count
			iter_n++;

			// skip first arbitrary 7 anchors (they're always garbage)
			if(iter_n < 7)
				continue;

			// only gather reasonable anchors
			if(!BP_StringIsReasonablePrintableString(anchor_iter->href, bpstrlen(anchor_iter->href), BP_FALSE, BP_FALSE))
				continue;

			// calculate anchor length
			size_t anchor_len = bpstrlen(anchor_iter->href);

			// must be at least ftp:// (in case ftp links)
			if(anchor_len < 6)
				continue;


			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Run Blacklist Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// set if the blacklist was triggered
			BP_BOOL blacklist_flagged = BP_FALSE;

			// blacklist checks
			size_t blacklist_iter_n = 0;
			for(; static_host_blacklist[blacklist_iter_n]; blacklist_iter_n++)
			{

				// skip google links
				if(anchor_len >= bpstrlen(static_host_blacklist[blacklist_iter_n]))
				if(strncmp(anchor_iter->href, static_host_blacklist[blacklist_iter_n], bpstrlen(static_host_blacklist[blacklist_iter_n])) == 0)
				{
					blacklist_flagged = BP_TRUE;
					break;
				}

			}

			// continue lookp if the blacklist is flagged
			if(blacklist_flagged == BP_TRUE)
				continue;


			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Check for Duplicate Entry %%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// if we have a hive, attempt to lookup
			if(dmss->system_hreg->hive)
			{

				// check if already exists
				char * lookup_path[10];
				BP_ZERO_STACK(lookup_path);

				// set lookup path
				lookup_path[0] = "search_results";
				lookup_path[1] = anchor_iter->href;
				lookup_path[2] = NULL;


				// attempt lookup
				P_BP_HASH_TABLE_KEYED_ENTRY lookup_test = BP_HashRegLookupTableEntryByPath(dmss->system_hreg, (char **) &lookup_path);
				if(lookup_test)
				{


					// increment the total dupe count
					total_dupe_count++;

					// if the total dupe count matches, force exit
					if(total_dupe_count >= total_dupes_before_exit)
					{
						found_end_of_records = BP_TRUE;
						break;
					}

				}

			}

			// break if we're at the end
			if(found_end_of_records)
				break;

			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Attempt Record Addition %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// set add path
			char * add_path[10];
			BP_ZERO_STACK(add_path);

			// set path to search_results
			add_path[0] = "search_results";
			add_path[1] = NULL;

			// create index buffer
			BP_ERROR_T ret_status = BP_HashRegAddDataToTableByPath
			(
					dmss->system_hreg,
					(char **) &add_path,
					anchor_iter->href,
					BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
					anchor_iter->href,
					bpstrlen(anchor_iter->href)
			);

			// display added
			if(ret_status)
			{
				printf("\n [+] Added: %u - %s", ret_status, anchor_iter->href);

				// increment the number of records
				anchor_retrieved_that_passed_filter_count++;
			}



			// increment the current record index only after
			// all sanity checks.
			curr_record_idx++;

		}

		// ensure we have retrieved some anchors that passed checks
		if(!anchor_retrieved_that_passed_filter_count)
		{
			found_end_of_records = BP_TRUE;
			break;
		}

		// if we have no records, we've found the end and can exit
		// no matter what
		if(!curr_record_idx)
		{
			found_end_of_records = BP_TRUE;
			break;
		}

		// exit if we're at the end of records
		if(found_end_of_records)
			break;

	} BP_HASH_ITER_CLOSE;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Leave Critical Section and Return %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// leave critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(dmss->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}


// Attempt to pregenerate a stack of queries
BP_ERROR_T BP_DisconnectMePreGenQueries
(
		P_BP_DISCONNECTME_SCRAPE_SESSION dmss,
		char *search_query,
		BP_DISCONNECT_ME_SEARCH_TARGET search_target
)
{

	if(!dmss)
		return ERR_FAILURE;
	if(!search_query)
		return ERR_FAILURE;
	if(!search_target)
		return ERR_FAILURE;

	// ensure we have a reasonable printable string
	if(!BP_StringIsReasonablePrintableString(search_query, bpstrlen(search_query), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Enter Critical Section and Generate Queries %%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(dmss->system_hreg);

	// set lookup table
	char * lookup_table[10];
	BP_ZERO_STACK(lookup_table);

	// set query table
	lookup_table[0] = "search_queries";
	lookup_table[1] = NULL;

	// lookup search queries
	P_BP_HASH_TABLE_KEYED_ENTRY queries_table = BP_HashRegLookupTableByPath(dmss->system_hreg, (char **) &lookup_table);

	// generate query stacks
	size_t n = 0;
	for(; n < 5000; n+=10)
	{


		// create linkl for adding google parameters
		P_BP_LINKL ddg_tq = BP_CreateLinkL(".", BP_TRUE);


		// create stack buffer for initial front part
		char ddg_front_part_initial[256];
		BP_ZERO_STACK(ddg_front_part_initial);

		// convert index to a buffer
		char * index_as_string = BP_SizeTToBuffer(n);

		// create the initial front part
		sprintf((char *) &ddg_front_part_initial, "/search?q=&hl=en&start=%u&sa=N", n);

		// generate ddg front part
		char * ddg_front_part = BP_UTF8StringEncode((char *) &ddg_front_part_initial, bpstrlen(ddg_front_part_initial));

		// create new tailqueue allocator
		P_BP_LINKL tmp_tq = BP_CreateLinkL(".", BP_TRUE);

		// https://search.disconnect.me/searchTerms/search?start=nav&option=Web&query=inurl%3Ablah&ses=Google&location_option=US&nextDDG=%2Fsearch%3Fq%3D%26hl%3Den%26start%3D100%26sa%3DN&showIcons=false&filterIcons=none&js_enabled=1&source=tor
		// https://search.disconnect.me/searchTerms/serp?search=c2c65f31-c43c-46d8-a50b-32413a859579


		// %2Fsearch%3Fq%3D%26hl%3Den%26start%3D
		//100
		// %26sa%3DN
		tq_strdup(tmp_tq, "http://search.disconnect.me/searchTerms/search?", BPLN);
		tq_strdup(tmp_tq, "start=nav", BPLN);
		tq_strdup(tmp_tq, "&option=Web", BPLN);
		tq_strdup(tmp_tq, "&option=Web", BPLN);
		tq_strdup(tmp_tq, "&query=", BPLN);
		tq_strdup(tmp_tq, search_query, BPLN);
		tq_strdup(tmp_tq, "&sess=Google", BPLN);
		tq_strdup(tmp_tq, "&location_option=US", BPLN);
		tq_strdup(tmp_tq, "&nextDDG=", BPLN);// %2Fsearch%3Fq%3D%26hl%3Den%26start%3D10%26sa%3DN");
		tq_strdup(tmp_tq, ddg_front_part, BPLN);
		tq_strdup(tmp_tq, "&showIcons=false", BPLN);
		tq_strdup(tmp_tq, "&filterIcons=none", BPLN);
		tq_strdup(tmp_tq, "&js_enabled=0", BPLN);
		tq_strdup(tmp_tq, "&source=tor", BPLN);

		// flatten string elements
		BP_LinkLFlattenJoinStringElements(tmp_tq);

		// add data to table
		BP_HashRegAddDataToTable
		(
				dmss->system_hreg,
				queries_table,
				index_as_string,
				BP_HASH_TABLE_KEYED_ENTRY_TYPE_BINARY_BLOB,
				(void *) tmp_tq->flat_buff,
				bpstrlen((char *) tmp_tq->flat_buff)
		);

		// printf("\n Got flat string?: %s", tmp_tq->flat_buff);

		// destroy linkl
		BP_DestroyLinkL(tmp_tq, BP_TRUE);
		bpfree(index_as_string);

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Leave Critical Section and Return %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// exit critical section
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION(dmss->system_hreg);

	// return indicating success
	return ERR_SUCCESS;

}

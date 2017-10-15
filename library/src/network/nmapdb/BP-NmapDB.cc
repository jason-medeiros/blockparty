/*
 * BP-NmapDB.cc
 *
 *  Created on: Mar 14, 2010
 *      Author: Jason Medeiros
 */

#include "../../../include/BP-Main.h"


/*
 * Using Search Functions:
 * ----------------------------------------------------
 * Each search function has a min, and max parameter.  Searching is done by
 * building a SQL query that basically matches all min/max items set in the
 * structures provided.  Min uses "<" operator, and Max uses ">" operator.
 * The like parameter matches using the "LIKE" keyword, and the equal parameter
 * matches with the "=" operator.  NULL parameters are not processed.  Like parameters
 * only work on character entries.
 */



// search the portstats database table
P_BP_NMAPDB_RESULTS BP_NDBSearchPortStats
(
		P_BP_MYSQL_SESSION       mysql_db,
		P_BP_NMAPDB_PORTSTATS    min,
		P_BP_NMAPDB_PORTSTATS    max,
		P_BP_NMAPDB_PORTSTATS    like,
		P_BP_NMAPDB_PORTSTATS    equal,
		size_t limitBottom, size_t limitTop
)
{
	if(!mysql_db)
		return NULL;

	// query parts
	char * select_part = NULL;
	char * min_part    = NULL;
	char * max_part    = NULL;
	char * like_part   = NULL;
	char * equal_part  = NULL;

	// final query when built
	char *final_query  = NULL;

	// ------------------- Min --------------------------------
	if(min)
	{
		// create string to hold the min part of the query
		min_part = (char *) bpcalloc(sizeof(BP_NMAPDB_PORTSTATS)*3, 1);

		// add date to query
		if(bpstrlen(min->d))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->d)-1+40, " d < \"%s\" AND ", min->d);

		// add time to query
		if(bpstrlen(min->t))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->d)-1+40, " t < \"%s\" AND ", min->t);

		// add hostid to query
		if(min->hostid > 0)
			snprintf(&min_part[bpstrlen(min_part)], 40, " hostid < \"%d\" AND ", min->hostid);

		// add target_ip to query
		if(bpstrlen(min->target_ip))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->target_ip)-1+40, " target_ip < \"%s\" AND ", min->target_ip);

		// add port to query
		if(min->port > 0)
			snprintf(&min_part[bpstrlen(min_part)], 40, " port < \"%d\" AND ", min->port);

		// add protocol to query
		if(bpstrlen(min->protocol))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->protocol)-1+40, " protocol < \"%s\" AND ", min->protocol);

		// add service to query
		if(bpstrlen(min->service))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->service)-1+40, " service < \"%s\" AND ", min->service);

		// add state to query
		if(bpstrlen(min->state))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->state)-1+40, " state < \"%s\" AND ", min->state);

		// add full version to query
		if(bpstrlen(min->fullversion))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->fullversion)-1+40, " fullversion < \"%s\" AND ", min->fullversion);

		// add scannerid
		if(min->scannerid > 0)
			snprintf(&min_part[bpstrlen(min_part)], 40, " scannerid < \"%d\" AND ", min->scannerid);

		// add runid to query
		if(min->runid > 0)
			snprintf(&min_part[bpstrlen(min_part)], 40, " runid < \"%d\" AND ", min->runid);

		// add rec_id to query
		if(min->rec_id > 0)
			snprintf(&min_part[bpstrlen(min_part)], 40, " rec_id < \"%d\" AND ", min->rec_id);

	}

	// ------------------- Max --------------------------------
	if(max)
	{
		// create string to hold the min part of the query
		max_part = (char *) bpcalloc(sizeof(BP_NMAPDB_PORTSTATS)*3, 1);

		// add date to query
		if(bpstrlen(max->d))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->d)-1+40, " d > \"%s\" AND ", max->d);

		// add time to query
		if(bpstrlen(max->t))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->d)-1+40, " t > \"%s\" AND ", max->t);

		// add hostid to query
		if(max->hostid > 0)
			snprintf(&max_part[bpstrlen(max_part)], 40, " hostid > \"%d\" AND ", max->hostid);

		// add target_ip to query
		if(bpstrlen(max->target_ip))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->target_ip)-1+40, " target_ip > \"%s\" AND ", max->target_ip);

		// add port to query
		if(max->port > 0)
			snprintf(&max_part[bpstrlen(max_part)], 40, " port > \"%d\" AND ", max->port);

		// add protocol to query
		if(bpstrlen(max->protocol))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->protocol)-1+40, " protocol > \"%s\" AND ", max->protocol);

		// add service to query
		if(bpstrlen(max->service))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->service)-1+40, " service > \"%s\" AND ", max->service);

		// add state to query
		if(bpstrlen(max->state))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->state)-1+40, " state > \"%s\" AND ", max->state);

		// add full version to query
		if(bpstrlen(max->fullversion))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->fullversion)-1+40, " fullversion > \"%s\" AND ", max->fullversion);

		// add scannerid
		if(max->scannerid > 0)
			snprintf(&max_part[bpstrlen(max_part)], 40, " scannerid > \"%d\" AND ", max->scannerid);

		// add runid to query
		if(max->runid > 0)
			snprintf(&max_part[bpstrlen(max_part)], 40, " runid > \"%d\" AND ", max->runid);

		// add rec_id to query
		if(max->rec_id > 0)
			snprintf(&max_part[bpstrlen(max_part)], 40, " rec_id > \"%d\" AND ", max->rec_id);

	}

	// ------------------- Like --------------------------------
	if(like)
	{
		// create string to hold the min part of the query
		like_part = (char *) bpcalloc(sizeof(BP_NMAPDB_PORTSTATS)*3, 1);

		// add date to query
		if(bpstrlen(like->d))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->d)-1+40, " d like \"%%%s%%\" AND ", like->d);

		// add time to query
		if(bpstrlen(like->t))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->d)-1+40, " t like \"%%%s%%\" AND ", like->t);

		// add hostid to query
		if(like->hostid > 0)
			snprintf(&like_part[bpstrlen(like_part)], 40, " hostid like \"%%%d%%\" AND ", like->hostid);

		// add target_ip to query
		if(bpstrlen(like->target_ip))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->target_ip)-1+40, " target_ip like \"%%%s%%\" AND ", like->target_ip);

		// add port to query
		if(like->port > 0)
			snprintf(&like_part[bpstrlen(like_part)], 40, " port like \"%%%d%%\" AND ", like->port);

		// add protocol to query
		if(bpstrlen(like->protocol))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->protocol)-1+40, " protocol like \"%%%s%%\" AND ", like->protocol);

		// add service to query
		if(bpstrlen(like->service))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->service)-1+40, " service like \"%%%s%%\" AND ", like->service);

		// add state to query
		if(bpstrlen(like->state))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->state)-1+40, " state like \"%%%s%%\" AND ", like->state);

		// add full version to query
		if(bpstrlen(like->fullversion))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->fullversion)-1+40, " fullversion like \"%%%s%%\" AND ", like->fullversion);

		// add scannerid
		if(like->scannerid > 0)
			snprintf(&like_part[bpstrlen(like_part)], 40, " scannerid like \"%%%d%%\" AND ", like->scannerid);

		// add runid to query
		if(like->runid > 0)
			snprintf(&like_part[bpstrlen(like_part)], 40, " runid like \"%%%d%%\" AND ", like->runid);

		// add rec_id to query
		if(like->rec_id > 0)
			snprintf(&like_part[bpstrlen(like_part)], 40, " rec_id like \"%%%d%%\" AND ", like->rec_id);

	}

	// ------------------- Equal --------------------------------
	if(equal)
	{

		// create string to hold the min part of the query
		equal_part = (char *) bpcalloc(sizeof(BP_NMAPDB_PORTSTATS)*3, 1);

		// add date to query
		if(bpstrlen(equal->d))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->d)-1+40, " d = \"%s\" AND ", equal->d);

		// add time to query
		if(bpstrlen(equal->t))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->d)-1+40, " t = \"%s\" AND ", equal->t);

		// add hostid to query
		if(equal->hostid > 0)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " hostid = \"%d\" AND ", equal->hostid);

		// add target_ip to query
		if(bpstrlen(equal->target_ip))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->target_ip)-1+40, " target_ip = \"%s\" AND ", equal->target_ip);

		// add port to query
		if(equal->port > 0)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " port = \"%d\" AND ", equal->port);

		// add protocol to query
		if(bpstrlen(equal->protocol))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->protocol)-1+40, " protocol = \"%s\" AND ", equal->protocol);

		// add service to query
		if(bpstrlen(equal->service))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->service)-1+40, " service = \"%s\" AND ", equal->service);

		// add state to query
		if(bpstrlen(equal->state))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->state)-1+40, " state = \"%s\" AND ", equal->state);

		// add full version to query
		if(bpstrlen(equal->fullversion))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->fullversion)-1+40, " fullversion = \"%s\" AND ", equal->fullversion);

		// add scannerid
		if(equal->scannerid > 0)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " scannerid = \"%d\" AND ", equal->scannerid);

		// add runid to query
		if(equal->runid > 0)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " runid = \"%d\" AND ", equal->runid);

		// add rec_id to query
		if(equal->rec_id > 0)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " rec_id = \"%d\" AND ", equal->rec_id);

	}


	// ---------------- Run Query --------------------------------

	// query parts
	select_part = "select * from portstat ";

	// max length variable
	size_t max_len = bpstrlen(select_part);

	if(min_part)
		max_len += bpstrlen(min_part);

	if(max_part)
		max_len += bpstrlen(max_part);

	if(like_part)
		max_len += bpstrlen(like_part);

	if(equal_part)
		max_len += bpstrlen(equal_part);

	// allocate space for final query (2x max for paranoia)
	final_query = (char *) bpcalloc(max_len * 2, 1);

	// implement select path
	snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", select_part);

	// add where if necessary
	if(min_part || max_part || like_part || equal_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, " WHERE ");

	if(min_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", min_part);

	if(max_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", max_part);

	if(like_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", like_part);

	if(equal_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", equal_part);

	// add terminating 1=1 to last AND if necessary
	if(min_part || max_part || like_part || equal_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, " 1=1 ");


	// -------------- Resolve Query in Database -------------------------------

	// MySQL Query Bindings
	// Use With null terminated strings only, results are accessed through struct MYSQL pointer
	if(BP_MySQLQuery(mysql_db, final_query))
		return NULL;

	// Returns stored results after a query, NULL on failure
	P_BP_MYSQL_RESULT result = BP_MySQLStoreResult(mysql_db);

	// Returns a result set from a query
	MYSQL_ROW row = (MYSQL_ROW) NULL;

	// array of ports
	BP_NMAPDB_PORTSTATS * portstats_array = NULL;

	// size index
	size_t i = 0;
	size_t x = 0;

	// recover results from the database
	for(
			i = 0;
			row = BP_MySQLFetchRow(result);
			i++
	)
	{

		// set mysql row index to 0 on loop
		x=0;

		// allocate space for the array
		portstats_array = (BP_NMAPDB_PORTSTATS *) bprealloc(portstats_array, sizeof(BP_NMAPDB_PORTSTATS) * (i+1));

		// copy in date
		strncpy( portstats_array[i].d, row[x], sizeof( portstats_array[i].d ) -1);
		x++;

		// copy in time
		strncpy( portstats_array[i].t, row[x], sizeof( portstats_array[i].t ) -1);
		x++;

		// copy in host id
		portstats_array[i].hostid = atoi(row[x]);
		x++;

		// copy in target ip
		strncpy( portstats_array[i].target_ip, row[x], sizeof( portstats_array[i].target_ip ) -1);
		x++;

		// copy in port
		portstats_array[i].port = atoi(row[x]);
		x++;

		// copy in protocol
		strncpy( portstats_array[i].protocol, row[x], sizeof( portstats_array[i].protocol ) -1);
		x++;

		// copy in service
		strncpy( portstats_array[i].service, row[x], sizeof( portstats_array[i].service ) -1);
		x++;

		// copy in state
		strncpy( portstats_array[i].state, row[x], sizeof( portstats_array[i].state ) -1);
		x++;

		// copy in fullversion
		strncpy( portstats_array[i].fullversion, row[x], sizeof( portstats_array[i].fullversion ) -1);
		x++;

		// copy in scannerid
		portstats_array[i].scannerid = atoi(row[x]);
		x++;

		// copy in runid
		portstats_array[i].runid = atoi(row[x]);
		x++;

		// copy in port
		portstats_array[i].rec_id = atoi(row[x]);

		// -- loop to next record --

	}

	// ---------------- Create Returnable Structure -------------

	// if the portstats array structure is filled out, we have a proper result and can
	// populate return values.
	P_BP_NMAPDB_RESULTS final_data = NULL;
	if(portstats_array)
	{

		// opaque result data
		final_data = (P_BP_NMAPDB_RESULTS) bpcalloc(sizeof(BP_NMAPDB_RESULTS), 1);

		// save query
		final_data->query = bpstrndup(final_query, 4096);

		// save opaque data as porstats_array
		final_data->opaque_data = (void *) portstats_array;

		// set type to portstat
		final_data->type = BP_NMAPDB_RESULT_PORTSTATS;

		// set record count to i + 1
		final_data->record_count = i+1;

	}
	// ---------------- Free Allocated Data ----------------------

	if(min_part)
		bpfree(min_part);

	if(max_part)
		bpfree(max_part);

	if(like_part)
		bpfree(like_part);

	if(equal_part)
		bpfree(equal_part);

	if(final_query)
		bpfree(final_query);

	// return the final filled out data structure
	return final_data;
}


// search the hoststats database table
P_BP_NMAPDB_RESULTS BP_NDBSearchHostStats(
		P_BP_MYSQL_SESSION mysql_db,
		P_BP_NMAPDB_HOSTSTATS    min,
		P_BP_NMAPDB_HOSTSTATS    max,
		P_BP_NMAPDB_HOSTSTATS    like,
		P_BP_NMAPDB_HOSTSTATS    equal,
		size_t limitBottom, size_t limitTop
)
{

	if(!mysql_db)
		return NULL;

	// query parts
	char * select_part = NULL;
	char * min_part    = NULL;
	char * max_part    = NULL;
	char * like_part   = NULL;
	char * equal_part  = NULL;

	// final query when built
	char *final_query  = NULL;

	// ------------------- Min --------------------------------
	if(min)
	{

		// create string to hold the min part of the query
		min_part = (char *) bpcalloc(sizeof(BP_NMAPDB_HOSTSTATS)*3, 1);

		// add hostid to query
		if(min->hostid)
			snprintf(&min_part[bpstrlen(min_part)], 40, " hostid < \"%d\" AND ", min->hostid);

		// add ip to query
		if(bpstrlen(min->ip))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->ip)-1+40, " ip < \"%s\" AND ", min->ip);

		// add user to query
		if(bpstrlen(min->user))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->user)-1+40, " user < \"%s\" AND ", min->user);

		// add userid to query
		if(min->userid)
			snprintf(&min_part[bpstrlen(min_part)], 40, " userid < \"%d\" AND ", min->userid);

		// add date to query
		if(bpstrlen(min->d))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->d)-1+40, " d < \"%s\" AND ", min->d);

		// add time to query
		if(bpstrlen(min->t))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->t)-1+40, " t < \"%s\" AND ", min->t);

		// add open_ports to query
		if(min->open_ports)
			snprintf(&min_part[bpstrlen(min_part)], 40, " open_ports < \"%d\" AND ", min->open_ports);

		// add ports_scanned to query
		if(min->ports_scanned)
			snprintf(&min_part[bpstrlen(min_part)], 40, " ports_scanned < \"%d\" AND ", min->ports_scanned);

		// add runid to query
		if(min->runid)
			snprintf(&min_part[bpstrlen(min_part)], 40, " runid < \"%d\" AND ", min->runid);

		// add scanner_id to query
		if(min->scanner_id)
			snprintf(&min_part[bpstrlen(min_part)], 40, " scanner_id < \"%d\" AND ", min->scanner_id);

		// add rec_id to query
		if(min->userid)
			snprintf(&min_part[bpstrlen(min_part)], 40, " rec_id < \"%d\" AND ", min->rec_id);


	}

	// ------------------- Max --------------------------------
	if(max)
	{
		// create string to hold the min part of the query
		max_part = (char *) bpcalloc(sizeof(BP_NMAPDB_HOSTSTATS)*3, 1);

		// add hostid to query
		if(max->hostid)
			snprintf(&max_part[bpstrlen(max_part)], 40, " hostid > \"%d\" AND ", max->hostid);

		// add ip to query
		if(bpstrlen(max->ip))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->ip)-1+40, " ip > \"%s\" AND ", max->ip);

		// add user to query
		if(bpstrlen(max->user))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->user)-1+40, " user > \"%s\" AND ", max->user);

		// add userid to query
		if(max->userid)
			snprintf(&max_part[bpstrlen(max_part)], 40, " userid > \"%d\" AND ", max->userid);

		// add date to query
		if(bpstrlen(max->d))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->d)-1+40, " d > \"%s\" AND ", max->d);

		// add time to query
		if(bpstrlen(max->t))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->t)-1+40, " t > \"%s\" AND ", max->t);

		// add open_ports to query
		if(max->open_ports)
			snprintf(&max_part[bpstrlen(max_part)], 40, " open_ports > \"%d\" AND ", max->open_ports);

		// add ports_scanned to query
		if(max->ports_scanned)
			snprintf(&max_part[bpstrlen(max_part)], 40, " ports_scanned > \"%d\" AND ", max->ports_scanned);

		// add runid to query
		if(max->runid)
			snprintf(&max_part[bpstrlen(max_part)], 40, " runid > \"%d\" AND ", max->runid);

		// add scanner_id to query
		if(max->scanner_id)
			snprintf(&max_part[bpstrlen(max_part)], 40, " scanner_id > \"%d\" AND ", max->scanner_id);

		// add rec_id to query
		if(max->userid)
			snprintf(&max_part[bpstrlen(max_part)], 40, " rec_id > \"%d\" AND ", max->rec_id);

	}

	// ------------------- Like --------------------------------
	if(like)
	{
		// create string to hold the min part of the query
		like_part = (char *) bpcalloc(sizeof(BP_NMAPDB_HOSTSTATS)*3, 1);

		// add hostid to query
		if(like->hostid)
			snprintf(&like_part[bpstrlen(like_part)], 40, " hostid like \"%%%d%%\" AND ", like->hostid);

		// add ip to query
		if(bpstrlen(like->ip))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->ip)-1+40, " ip like \"%%%s%%\" AND ", like->ip);

		// add user to query
		if(bpstrlen(like->user))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->user)-1+40, " user like \"%%%s%%\" AND ", like->user);

		// add userid to query
		if(like->userid)
			snprintf(&like_part[bpstrlen(like_part)], 40, " userid like \"%%%d%%\" AND ", like->userid);

		// add date to query
		if(bpstrlen(like->d))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->d)-1+40, " d like \"%%%s%%\" AND ", like->d);

		// add time to query
		if(bpstrlen(like->t))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->t)-1+40, " t like \"%%%s%%\" AND ", like->t);

		// add open_ports to query
		if(like->open_ports)
			snprintf(&like_part[bpstrlen(like_part)], 40, " open_ports like \"%%%d%%\" AND ", like->open_ports);

		// add ports_scanned to query
		if(like->ports_scanned)
			snprintf(&like_part[bpstrlen(like_part)], 40, " ports_scanned like \"%%%d%%\" AND ", like->ports_scanned);

		// add runid to query
		if(like->runid)
			snprintf(&like_part[bpstrlen(like_part)], 40, " runid like \"%%%d%%\" AND ", like->runid);

		// add scanner_id to query
		if(like->scanner_id)
			snprintf(&like_part[bpstrlen(like_part)], 40, " scanner_id like \"%%%d%%\" AND ", like->scanner_id);

		// add rec_id to query
		if(like->userid)
			snprintf(&like_part[bpstrlen(like_part)], 40, " rec_id like \"%%%d%%\" AND ", like->rec_id);


	}

	// ------------------- Equal --------------------------------
	if(equal)
	{

		// create string to hold the min part of the query
		equal_part = (char *) bpcalloc(sizeof(BP_NMAPDB_HOSTSTATS)*3, 1);

		// add hostid to query
		if(equal->hostid)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " hostid = \"%d\" AND ", equal->hostid);

		// add ip to query
		if(bpstrlen(equal->ip))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->ip)-1+40, " ip = \"%s\" AND ", equal->ip);

		// add user to query
		if(bpstrlen(equal->user))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->user)-1+40, " user = \"%s\" AND ", equal->user);

		// add userid to query
		if(equal->userid)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " userid = \"%d\" AND ", equal->userid);

		// add date to query
		if(bpstrlen(equal->d))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->d)-1+40, " d = \"%s\" AND ", equal->d);

		// add time to query
		if(bpstrlen(equal->t))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->t)-1+40, " t = \"%s\" AND ", equal->t);

		// add open_ports to query
		if(equal->open_ports)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " open_ports = \"%d\" AND ", equal->open_ports);

		// add ports_scanned to query
		if(equal->ports_scanned)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " ports_scanned = \"%d\" AND ", equal->ports_scanned);

		// add runid to query
		if(equal->runid)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " runid = \"%d\" AND ", equal->runid);

		// add scanner_id to query
		if(equal->scanner_id)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " scanner_id = \"%d\" AND ", equal->scanner_id);

		// add rec_id to query
		if(equal->userid)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " rec_id = \"%d\" AND ", equal->rec_id);

	}


	// ---------------- Run Query --------------------------------

	// query parts
	select_part = "select * from hoststats ";

	// max length variable
	size_t max_len = bpstrlen(select_part);

	if(min_part)
		max_len += bpstrlen(min_part);

	if(max_part)
		max_len += bpstrlen(max_part);

	if(like_part)
		max_len += bpstrlen(like_part);

	if(equal_part)
		max_len += bpstrlen(equal_part);

	// allocate space for final query (2x max for paranoia)
	final_query = (char *) bpcalloc(max_len * 2, 1);

	// implement select path
	snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", select_part);

	// add where if necessary
	if(min_part || max_part || like_part || equal_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, " WHERE ");

	if(min_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", min_part);

	if(max_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", max_part);

	if(like_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", like_part);

	if(equal_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", equal_part);

	// add terminating 1=1 to last AND if necessary
	if(min_part || max_part || like_part || equal_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, " 1=1 ");


	// -------------- Resolve Query in Database -------------------------------

	// MySQL Query Bindings
	// Use With null terminated strings only, results are accessed through struct MYSQL pointer
	if(BP_MySQLQuery(mysql_db, final_query))
		return NULL;

	// Returns stored results after a query, NULL on failure
	P_BP_MYSQL_RESULT result = BP_MySQLStoreResult(mysql_db);

	// Returns a result set from a query
	MYSQL_ROW row = (MYSQL_ROW) NULL;

	// array of ports
	BP_NMAPDB_HOSTSTATS * hoststats_array = NULL;

	// size index
	size_t i = 0;
	size_t x = 0;

	// recover results from the database
	for(
			i = 0;
			row = BP_MySQLFetchRow(result);
			i++
	)
	{

		// set mysql row index to 0 on loop
		x=0;

		// allocate space for the array
		hoststats_array = (BP_NMAPDB_HOSTSTATS *) bprealloc(hoststats_array, sizeof(BP_NMAPDB_HOSTSTATS) * (i+1));


		// copy in host id
		hoststats_array[i].hostid = atoi(row[x]);
		x++;

		// copy in ip
		strncpy( hoststats_array[i].ip, row[x], sizeof( hoststats_array[i].ip) -1);
		x++;

		// copy in user
		strncpy( hoststats_array[i].user, row[x], sizeof( hoststats_array[i].user) -1);
		x++;

		// copy in hostid
		hoststats_array[i].hostid = atoi(row[x]);
		x++;

		// copy in date (d)
		strncpy( hoststats_array[i].d, row[x], sizeof( hoststats_array[i].d) -1);
		x++;

		// copy in time (t)
		strncpy( hoststats_array[i].t, row[x], sizeof( hoststats_array[i].t) -1);
		x++;

		// copy in open_ports
		hoststats_array[i].open_ports = atoi(row[x]);
		x++;

		// copy in ports_scanned
		hoststats_array[i].ports_scanned = atoi(row[x]);
		x++;

		// copy in runid
		hoststats_array[i].runid = atoi(row[x]);
		x++;

		// copy in scanner_id
		hoststats_array[i].scanner_id = atoi(row[x]);
		x++;

		// copy in rec_id
		hoststats_array[i].rec_id = atoi(row[x]);
		x++;

		// -- loop to next record --

	}

	// ---------------- Create Returnable Structure -------------

	// if the portstats array structure is filled out, we have a proper result and can
	// populate return values.
	P_BP_NMAPDB_RESULTS final_data = NULL;
	if(hoststats_array)
	{

		// opaque result data
		final_data = (P_BP_NMAPDB_RESULTS) bpcalloc(sizeof(BP_NMAPDB_RESULTS), 1);

		// save query
		final_data->query = bpstrndup(final_query, 4096);

		// save opaque data as porstats_array
		final_data->opaque_data = (void *) hoststats_array;

		// set type to hoststat
		final_data->type = BP_NMAPDB_RESULT_HOSTSTATS;

		// set record count to i + 1
		final_data->record_count = i+1;

	}
	// ---------------- Free Allocated Data ----------------------

	if(min_part)
		bpfree(min_part);

	if(max_part)
		bpfree(max_part);

	if(like_part)
		bpfree(like_part);

	if(equal_part)
		bpfree(equal_part);

	if(final_query)
		bpfree(final_query);

	// return the final filled out data structure
	return final_data;

}


// search the targets database table
P_BP_NMAPDB_RESULTS BP_NDBSearchTargets(
		P_BP_MYSQL_SESSION mysql_db,
		P_NMAPDB_TARGETS         min,
		P_NMAPDB_TARGETS         max,
		P_NMAPDB_TARGETS         like,
		P_NMAPDB_TARGETS         equal,
		size_t limitBottom, size_t limitTop
)
{

	if(!mysql_db)
		return NULL;

	// query parts
	char * select_part = NULL;
	char * min_part    = NULL;
	char * max_part    = NULL;
	char * like_part   = NULL;
	char * equal_part  = NULL;

	// final query when built
	char *final_query  = NULL;

	// ------------------- Min --------------------------------
	if(min)
	{
		// create string to hold the min part of the query
		min_part = (char *) bpcalloc(sizeof(BP_NMAPDB_TARGETS)*3, 1);

		// add host to query
		if(bpstrlen(min->host))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->host)-1+40, " host < \"%s\" AND ", min->host);

		// add date to query
		if(bpstrlen(min->d))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->d)-1+40, " d < \"%s\" AND ", min->d);

		// add time to query
		if(bpstrlen(min->t))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->d)-1+40, " t < \"%s\" AND ", min->t);

		// add ip to query
		if(bpstrlen(min->ip))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->ip)-1+40, " ip < \"%s\" AND ", min->ip);

		// add ipnum to query
		if(min->ipnum)
			snprintf(&min_part[bpstrlen(min_part)], 40, " ipnum < \"%d\" AND ", min->ipnum);

		// add mac to query
		if(bpstrlen(min->mac))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->mac)-1+40, " mac < \"%s\" AND ", min->mac);

		// add macvendor to query
		if(bpstrlen(min->macvendor))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->mac)-1+40, " macvendor < \"%s\" AND ", min->macvendor);

		// add flag to query
		if(min->flag)
			snprintf(&min_part[bpstrlen(min_part)], 40, " flag < \"%d\" AND ", min->flag);

		// add ipnum to query
		if(min->hostid)
			snprintf(&min_part[bpstrlen(min_part)], 40, " hostid < \"%d\" AND ", min->hostid);

		// add description to query
		if(bpstrlen(min->description))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->description)-1+40, " description < \"%s\" AND ", min->description);

		// add os_guessed to query
		if(bpstrlen(min->os_guessed))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->os_guessed)-1+40, " os_guessed < \"%s\" AND ", min->os_guessed);

		// add os_known to query
		if(bpstrlen(min->os_known))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->os_known)-1+40, " os_known < \"%s\" AND ", min->os_known);

		// add macvendor to query
		if(bpstrlen(min->fingerprint))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->fingerprint)-1+40, " fingerprint < \"%s\" AND ", min->fingerprint);

		// add rec_id to query
		if(min->rec_id)
			snprintf(&min_part[bpstrlen(min_part)], 40, " rec_id < \"%d\" AND ", min->rec_id);

	}

	// ------------------- Max --------------------------------
	if(max)
	{
		// create string to hold the min part of the query
		max_part = (char *) bpcalloc(sizeof(BP_NMAPDB_TARGETS)*3, 1);

		// add host to query
		if(bpstrlen(max->host))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->host)-1+40, " host > \"%s\" AND ", max->host);

		// add date to query
		if(bpstrlen(max->d))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->d)-1+40, " d > \"%s\" AND ", max->d);

		// add time to query
		if(bpstrlen(max->t))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->d)-1+40, " t > \"%s\" AND ", max->t);

		// add ip to query
		if(bpstrlen(max->ip))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->ip)-1+40, " ip > \"%s\" AND ", max->ip);

		// add ipnum to query
		if(max->ipnum)
			snprintf(&max_part[bpstrlen(max_part)], 40, " ipnum > \"%d\" AND ", max->ipnum);

		// add mac to query
		if(bpstrlen(max->mac))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->mac)-1+40, " mac > \"%s\" AND ", max->mac);

		// add macvendor to query
		if(bpstrlen(max->macvendor))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->mac)-1+40, " macvendor > \"%s\" AND ", max->macvendor);

		// add flag to query
		if(max->flag)
			snprintf(&max_part[bpstrlen(max_part)], 40, " flag > \"%d\" AND ", max->flag);

		// add ipnum to query
		if(max->hostid)
			snprintf(&max_part[bpstrlen(max_part)], 40, " hostid > \"%d\" AND ", max->hostid);

		// add description to query
		if(bpstrlen(max->description))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->description)-1+40, " description > \"%s\" AND ", max->description);

		// add os_guessed to query
		if(bpstrlen(max->os_guessed))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->os_guessed)-1+40, " os_guessed > \"%s\" AND ", max->os_guessed);

		// add os_known to query
		if(bpstrlen(max->os_known))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->os_known)-1+40, " os_known > \"%s\" AND ", max->os_known);

		// add macvendor to query
		if(bpstrlen(max->fingerprint))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->fingerprint)-1+40, " fingerprint > \"%s\" AND ", max->fingerprint);

		// add rec_id to query
		if(max->rec_id)
			snprintf(&max_part[bpstrlen(max_part)], 40, " rec_id > \"%d\" AND ", max->rec_id);

	}

	// ------------------- Like --------------------------------
	if(like)
	{
		// create string to hold the min part of the query
		like_part = (char *) bpcalloc(sizeof(BP_NMAPDB_TARGETS)*3, 1);

		// add host to query
		if(bpstrlen(like->host))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->host)-1+40, " host like \"%%%s%%\" AND ", like->host);

		// add date to query
		if(bpstrlen(like->d))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->d)-1+40, " d like \"%%%s%%\" AND ", like->d);

		// add time to query
		if(bpstrlen(like->t))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->d)-1+40, " t like \"%%%s%%\" AND ", like->t);

		// add ip to query
		if(bpstrlen(like->ip))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->ip)-1+40, " ip like \"%%%s%%\" AND ", like->ip);

		// add ipnum to query
		if(like->ipnum)
			snprintf(&like_part[bpstrlen(like_part)], 40, " ipnum like \"%%%d%%\" AND ", like->ipnum);

		// add mac to query
		if(bpstrlen(like->mac))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->mac)-1+40, " mac like \"%%%s%%\" AND ", like->mac);

		// add macvendor to query
		if(bpstrlen(like->macvendor))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->mac)-1+40, " macvendor like \"%%%s%%\" AND ", like->macvendor);

		// add flag to query
		if(like->flag)
			snprintf(&like_part[bpstrlen(like_part)], 40, " flag like \"%%%d%%\" AND ", like->flag);

		// add ipnum to query
		if(like->hostid)
			snprintf(&like_part[bpstrlen(like_part)], 40, " hostid like \"%%%d%%\" AND ", like->hostid);

		// add description to query
		if(bpstrlen(like->description))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->description)-1+40, " description like \"%%%s%%\" AND ", like->description);

		// add os_guessed to query
		if(bpstrlen(like->os_guessed))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->os_guessed)-1+40, " os_guessed like \"%%%s%%\" AND ", like->os_guessed);

		// add os_known to query
		if(bpstrlen(like->os_known))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->os_known)-1+40, " os_known like \"%%%s%%\" AND ", like->os_known);

		// add macvendor to query
		if(bpstrlen(like->fingerprint))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->fingerprint)-1+40, " fingerprint like \"%%%s%%\" AND ", like->fingerprint);

		// add rec_id to query
		if(like->rec_id)
			snprintf(&like_part[bpstrlen(like_part)], 40, " rec_id like \"%%%d%%\" AND ", like->rec_id);

	}

	// ------------------- Equal --------------------------------
	if(equal)
	{

		// create string to hold the min part of the query
		equal_part = (char *) bpcalloc(sizeof(BP_NMAPDB_TARGETS)*3, 1);

		// add host to query
		if(bpstrlen(equal->host))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->host)-1+40, " host = \"%s\" AND ", equal->host);

		// add date to query
		if(bpstrlen(equal->d))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->d)-1+40, " d = \"%s\" AND ", equal->d);

		// add time to query
		if(bpstrlen(equal->t))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->d)-1+40, " t = \"%s\" AND ", equal->t);

		// add ip to query
		if(bpstrlen(equal->ip))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->ip)-1+40, " ip = \"%s\" AND ", equal->ip);

		// add ipnum to query
		if(equal->ipnum)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " ipnum = \"%d\" AND ", equal->ipnum);

		// add mac to query
		if(bpstrlen(equal->mac))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->mac)-1+40, " mac = \"%s\" AND ", equal->mac);

		// add macvendor to query
		if(bpstrlen(equal->macvendor))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->mac)-1+40, " macvendor = \"%s\" AND ", equal->macvendor);

		// add flag to query
		if(equal->flag)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " flag = \"%d\" AND ", equal->flag);

		// add ipnum to query
		if(equal->hostid)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " hostid = \"%d\" AND ", equal->hostid);

		// add description to query
		if(bpstrlen(equal->description))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->description)-1+40, " description = \"%s\" AND ", equal->description);

		// add os_guessed to query
		if(bpstrlen(equal->os_guessed))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->os_guessed)-1+40, " os_guessed = \"%s\" AND ", equal->os_guessed);

		// add os_known to query
		if(bpstrlen(equal->os_known))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->os_known)-1+40, " os_known = \"%s\" AND ", equal->os_known);

		// add macvendor to query
		if(bpstrlen(equal->fingerprint))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->fingerprint)-1+40, " fingerprint = \"%s\" AND ", equal->fingerprint);

		// add rec_id to query
		if(equal->rec_id)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " rec_id = \"%d\" AND ", equal->rec_id);

	}


	// ---------------- Run Query --------------------------------

	// query parts
	select_part = "select * from targets ";

	// max length variable
	size_t max_len = bpstrlen(select_part);

	if(min_part)
		max_len += bpstrlen(min_part);

	if(max_part)
		max_len += bpstrlen(max_part);

	if(like_part)
		max_len += bpstrlen(like_part);

	if(equal_part)
		max_len += bpstrlen(equal_part);

	// allocate space for final query (2x max for paranoia)
	final_query = (char *) bpcalloc(max_len * 2, 1);

	// implement select path
	snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", select_part);

	// add where if necessary
	if(min_part || max_part || like_part || equal_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, " WHERE ");

	if(min_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", min_part);

	if(max_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", max_part);

	if(like_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", like_part);

	if(equal_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", equal_part);

	// add terminating 1=1 to last AND if necessary
	if(min_part || max_part || like_part || equal_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, " 1=1 ");


	// -------------- Resolve Query in Database -------------------------------

	// MySQL Query Bindings
	// Use With null terminated strings only, results are accessed through struct MYSQL pointer
	if(BP_MySQLQuery(mysql_db, final_query))
		return NULL;

	// Returns stored results after a query, NULL on failure
	P_BP_MYSQL_RESULT result = BP_MySQLStoreResult(mysql_db);

	// Returns a result set from a query
	MYSQL_ROW row = (MYSQL_ROW) NULL;

	// array of ports
	BP_NMAPDB_TARGETS * targets_array = NULL;

	// size index
	size_t i = 0;
	size_t x = 0;

	// recover results from the database
	for(
			i = 0;
			row = BP_MySQLFetchRow(result);
			i++
	)
	{

		// set mysql row index to 0 on loop
		x=0;

		// allocate space for the array
		targets_array = (BP_NMAPDB_TARGETS *) bprealloc(targets_array, sizeof(BP_NMAPDB_TARGETS) * (i+1));

		// copy in host id
		targets_array[i].hostid = atoi(row[x]);
		x++;

		// copy in host
		strncpy( targets_array[i].host, row[x], sizeof( targets_array[i].host ) -1);
		x++;

		// copy in date (d)
		strncpy( targets_array[i].d, row[x], sizeof( targets_array[i].d ) -1);
		x++;

		// copy in time (t)
		strncpy( targets_array[i].t, row[x], sizeof( targets_array[i].t ) -1);
		x++;

		// copy in ip
		strncpy( targets_array[i].ip, row[x], sizeof( targets_array[i].ip ) -1);
		x++;

		// copy in ipnum
		targets_array[i].ipnum = atoi(row[x]);
		x++;

		// copy in mac
		strncpy( targets_array[i].mac, row[x], sizeof( targets_array[i].mac ) -1);
		x++;

		// copy in macvendor
		strncpy( targets_array[i].macvendor, row[x], sizeof( targets_array[i].macvendor ) -1);
		x++;

		// copy in flag
		targets_array[i].flag = atoi(row[x]);
		x++;

		// copy in descripton
		strncpy( targets_array[i].description, row[x], sizeof( targets_array[i].description ) -1);
		x++;

		// copy in os_guessed
		strncpy( targets_array[i].os_guessed, row[x], sizeof( targets_array[i].os_guessed ) -1);
		x++;

		// copy in os_known
		strncpy( targets_array[i].os_known, row[x], sizeof( targets_array[i].os_known ) -1);
		x++;

		// copy in fingerprint
		strncpy( targets_array[i].fingerprint, row[x], sizeof( targets_array[i].fingerprint ) -1);
		x++;

		// copy in rec_id
		targets_array[i].rec_id = atoi(row[x]);
		x++;

		// -- loop to next record --

	}

	// ---------------- Create Returnable Structure -------------

	// if the portstats array structure is filled out, we have a proper result and can
	// populate return values.
	P_BP_NMAPDB_RESULTS final_data = NULL;
	if(targets_array)
	{

		// opaque result data
		final_data = (P_BP_NMAPDB_RESULTS) bpcalloc(sizeof(BP_NMAPDB_RESULTS), 1);

		// save query
		final_data->query = bpstrndup(final_query, 4096);

		// save opaque data as targets_array
		final_data->opaque_data = (void *) targets_array;

		// set type to portstat
		final_data->type = BP_NMAPDB_RESULT_TARGETS;

		// set record count to i + 1
		final_data->record_count = i+1;

	}
	// ---------------- Free Allocated Data ----------------------

	if(min_part)
		bpfree(min_part);

	if(max_part)
		bpfree(max_part);

	if(like_part)
		bpfree(like_part);

	if(equal_part)
		bpfree(equal_part);

	if(final_query)
		bpfree(final_query);

	// return the final filled out data structure
	return final_data;

}

// search the scanners database table
P_BP_NMAPDB_RESULTS BP_NDBSearchScanners(
		P_BP_MYSQL_SESSION mysql_db,
		P_BP_NMAPDB_SCANNERS     min,
		P_BP_NMAPDB_SCANNERS     max,
		P_BP_NMAPDB_SCANNERS     like,
		P_BP_NMAPDB_SCANNERS     equal,
		size_t limitBottom, size_t limitTop
)
{
	if(!mysql_db)
		return NULL;

	// query parts
	char * select_part = NULL;
	char * min_part    = NULL;
	char * max_part    = NULL;
	char * like_part   = NULL;
	char * equal_part  = NULL;

	// final query when built
	char *final_query  = NULL;

	// ------------------- Min --------------------------------
	if(min)
	{
		// create string to hold the min part of the query
		min_part = (char *) bpcalloc(sizeof(BP_NMAPDB_SCANNERS)*3, 1);

		// add scanner to query
		if(bpstrlen(min->scanner))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->scanner)-1+40, " scanner < \"%s\" AND ", min->scanner);

		// add scanner_ip to query
		if(bpstrlen(min->scanner_ip))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->scanner_ip)-1+40, " scanner_ip < \"%s\" AND ", min->scanner_ip);

		// add description to query
		if(bpstrlen(min->description))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->description)-1+40, " description < \"%s\" AND ", min->description);

		// add date to query
		if(bpstrlen(min->d))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->d)-1+40, " d < \"%s\" AND ", min->d);

		// add time to query
		if(bpstrlen(min->t))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->t)-1+40, " t < \"%s\" AND ", min->t);

		// add flag to query
		if(min->flag)
			snprintf(&min_part[bpstrlen(min_part)], 40, " flag < \"%d\" AND ", min->flag);

		// add scanner_id to query
		if(min->scanner_id)
			snprintf(&min_part[bpstrlen(min_part)], 40, " scanner_id < \"%d\" AND ", min->scanner_id);

		// add rec_id to query
		if(min->rec_id)
			snprintf(&min_part[bpstrlen(min_part)], 40, " rec_id < \"%d\" AND ", min->rec_id);

	}

	// ------------------- Max --------------------------------
	if(max)
	{
		// create string to hold the min part of the query
		max_part = (char *) bpcalloc(sizeof(BP_NMAPDB_SCANNERS)*3, 1);

		// add scanner to query
		if(bpstrlen(max->scanner))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->scanner)-1+40, " scanner > \"%s\" AND ", max->scanner);

		// add scanner_ip to query
		if(bpstrlen(max->scanner_ip))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->scanner_ip)-1+40, " scanner_ip > \"%s\" AND ", max->scanner_ip);

		// add description to query
		if(bpstrlen(max->description))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->description)-1+40, " description > \"%s\" AND ", max->description);

		// add date to query
		if(bpstrlen(max->d))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->d)-1+40, " d > \"%s\" AND ", max->d);

		// add time to query
		if(bpstrlen(max->t))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->t)-1+40, " t > \"%s\" AND ", max->t);

		// add flag to query
		if(max->flag)
			snprintf(&max_part[bpstrlen(max_part)], 40, " flag > \"%d\" AND ", max->flag);

		// add scanner_id to query
		if(max->scanner_id)
			snprintf(&max_part[bpstrlen(max_part)], 40, " scanner_id > \"%d\" AND ", max->scanner_id);

		// add rec_id to query
		if(max->rec_id)
			snprintf(&max_part[bpstrlen(max_part)], 40, " rec_id > \"%d\" AND ", max->rec_id);


	}

	// ------------------- Like --------------------------------
	if(like)
	{
		// create string to hold the min part of the query
		like_part = (char *) bpcalloc(sizeof(BP_NMAPDB_SCANNERS)*3, 1);

		// add scanner to query
		if(bpstrlen(like->scanner))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->scanner)-1+40, " scanner like \"%%%s%%\" AND ", like->scanner);

		// add scanner_ip to query
		if(bpstrlen(like->scanner_ip))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->scanner_ip)-1+40, " scanner_ip like \"%%%s%%\" AND ", like->scanner_ip);

		// add description to query
		if(bpstrlen(like->description))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->description)-1+40, " description like \"%%%s%%\" AND ", like->description);

		// add date to query
		if(bpstrlen(like->d))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->d)-1+40, " d like \"%%%s%%\" AND ", like->d);

		// add time to query
		if(bpstrlen(like->t))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->t)-1+40, " t like \"%%%s%%\" AND ", like->t);

		// add flag to query
		if(like->flag)
			snprintf(&like_part[bpstrlen(like_part)], 40, " flag like \"%%%d%%\" AND ", like->flag);

		// add scanner_id to query
		if(like->scanner_id)
			snprintf(&like_part[bpstrlen(like_part)], 40, " scanner_id like \"%%%d%%\" AND ", like->scanner_id);

		// add rec_id to query
		if(like->rec_id)
			snprintf(&like_part[bpstrlen(like_part)], 40, " rec_id like \"%%%d%%\" AND ", like->rec_id);


	}

	// ------------------- Equal --------------------------------
	if(equal)
	{

		// create string to hold the like part of the query
		equal_part = (char *) bpcalloc(sizeof(BP_NMAPDB_SCANNERS)*3, 1);

		// add scanner to query
		if(bpstrlen(equal->scanner))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->scanner)-1+40, " scanner = \"%s\" AND ", equal->scanner);

		// add scanner_ip to query
		if(bpstrlen(equal->scanner_ip))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->scanner_ip)-1+40, " scanner_ip = \"%s\" AND ", equal->scanner_ip);

		// add description to query
		if(bpstrlen(equal->description))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->description)-1+40, " description = \"%s\" AND ", equal->description);

		// add date to query
		if(bpstrlen(equal->d))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->d)-1+40, " d = \"%s\" AND ", equal->d);

		// add time to query
		if(bpstrlen(equal->t))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->t)-1+40, " t = \"%s\" AND ", equal->t);

		// add flag to query
		if(equal->flag)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " flag = \"%d\" AND ", equal->flag);

		// add scanner_id to query
		if(equal->scanner_id)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " scanner_id = \"%d\" AND ", equal->scanner_id);

		// add rec_id to query
		if(equal->rec_id)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " rec_id = \"%d\" AND ", equal->rec_id);


	}


	// ---------------- Run Query --------------------------------

	// query parts
	select_part = "select * from scanners ";

	// max length variable
	size_t max_len = bpstrlen(select_part);

	if(min_part)
		max_len += bpstrlen(min_part);

	if(max_part)
		max_len += bpstrlen(max_part);

	if(like_part)
		max_len += bpstrlen(like_part);

	if(equal_part)
		max_len += bpstrlen(equal_part);

	// allocate space for final query (2x max for paranoia)
	final_query = (char *) bpcalloc(max_len * 2, 1);

	// implement select path
	snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", select_part);

	// add where if necessary
	if(min_part || max_part || like_part || equal_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, " WHERE ");

	if(min_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", min_part);

	if(max_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", max_part);

	if(like_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", like_part);

	if(equal_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", equal_part);

	// add terminating 1=1 to last AND if necessary
	if(min_part || max_part || like_part || equal_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, " 1=1 ");


	// -------------- Resolve Query in Database -------------------------------

	// MySQL Query Bindings
	// Use With null terminated strings only, results are accessed through struct MYSQL pointer
	if(BP_MySQLQuery(mysql_db, final_query))
		return NULL;

	// Returns stored results after a query, NULL on failure
	P_BP_MYSQL_RESULT result = BP_MySQLStoreResult(mysql_db);

	// Returns a result set from a query
	MYSQL_ROW row = (MYSQL_ROW) NULL;

	// array of scanners
	BP_NMAPDB_SCANNERS * scanners_array = NULL;

	// size index
	size_t i = 0;
	size_t x = 0;

	// recover results from the database
	for(
			i = 0;
			row = BP_MySQLFetchRow(result);
			i++
	)
	{

		// set mysql row index to 0 on loop
		x=0;

		// allocate space for the array
		scanners_array = (BP_NMAPDB_SCANNERS *) bprealloc(scanners_array, sizeof(BP_NMAPDB_SCANNERS) * (i+1));

		// copy in scanner
		strncpy( scanners_array[i].scanner, row[x], sizeof( scanners_array[i].scanner ) -1);
		x++;

		// copy in scanner_ip
		strncpy( scanners_array[i].scanner_ip, row[x], sizeof( scanners_array[i].scanner_ip ) -1);
		x++;

		// copy in description
		strncpy( scanners_array[i].description, row[x], sizeof( scanners_array[i].description ) -1);
		x++;

		// copy in date (d)
		strncpy( scanners_array[i].d, row[x], sizeof( scanners_array[i].d ) -1);
		x++;

		// copy in time (t)
		strncpy( scanners_array[i].t, row[x], sizeof( scanners_array[i].t ) -1);
		x++;

		// copy in flag
		scanners_array[i].flag = atoi(row[x]);
		x++;

		// copy in scanner_id
		scanners_array[i].scanner_id = atoi(row[x]);
		x++;

		// copy in rec_id
		scanners_array[i].rec_id = atoi(row[x]);
		x++;

		// -- loop to next record --

	}

	// ---------------- Create Returnable Structure -------------

	// if the scanners array structure is filled out, we have a proper result and can
	// populate return values.
	P_BP_NMAPDB_RESULTS final_data = NULL;
	if(scanners_array)
	{

		// opaque result data
		final_data = (P_BP_NMAPDB_RESULTS) bpcalloc(sizeof(BP_NMAPDB_RESULTS), 1);

		// save query
		final_data->query = bpstrndup(final_query, 4096);

		// save opaque data as porstats_array
		final_data->opaque_data = (void *) scanners_array;

		// set type to portstat
		final_data->type = BP_NMAPDB_RESULT_SCANNERS;

		// set record count to i + 1
		final_data->record_count = i+1;

	}
	// ---------------- Free Allocated Data ----------------------

	if(min_part)
		bpfree(min_part);

	if(max_part)
		bpfree(max_part);

	if(like_part)
		bpfree(like_part);

	if(equal_part)
		bpfree(equal_part);

	if(final_query)
		bpfree(final_query);

	// return the final filled out data structure
	return final_data;

}

// search the run list database table
P_BP_NMAPDB_RESULTS BP_NDBSearchRunlist(
		P_BP_MYSQL_SESSION mysql_db,
		P_BP_NMAPDB_RUNLIST      min,
		P_BP_NMAPDB_RUNLIST      max,
		P_BP_NMAPDB_RUNLIST      like,
		P_BP_NMAPDB_RUNLIST      equal,
		size_t limitBottom, size_t limitTop
)
{
	if(!mysql_db)
		return NULL;

	// query parts
	char * select_part = NULL;
	char * min_part    = NULL;
	char * max_part    = NULL;
	char * like_part   = NULL;
	char * equal_part  = NULL;

	// final query when built
	char *final_query  = NULL;

	// ------------------- Min --------------------------------
	if(min)
	{
		// create string to hold the min part of the query
		min_part = (char *) bpcalloc(sizeof(BP_NMAPDB_RUNLIST)*3, 1);


		// add date (d) to query
		if(bpstrlen(min->d))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->d)-1+40, " d < \"%s\" AND ", min->d);

		// add time (t) to query
		if(bpstrlen(min->t))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->t)-1+40, " t < \"%s\" AND ", min->t);

		// add run_type to query
		if(min->run_type)
			snprintf(&min_part[bpstrlen(min_part)], 40, " run_type < \"%d\" AND ", min->run_type);

		// add user to query
		if(bpstrlen(min->user))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->user)-1+40, " user < \"%s\" AND ", min->user);

		// add userid to query
		if(min->userid)
			snprintf(&min_part[bpstrlen(min_part)], 40, " userid < \"%d\" AND ", min->userid);

		// add host to query
		if(bpstrlen(min->host))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->host)-1+40, " user < \"%s\" AND ", min->host);

		// add host_ip to query
		if(bpstrlen(min->host_ip))
			snprintf(&min_part[bpstrlen(min_part)], sizeof(min->user)-1+40, " host_ip < \"%s\" AND ", min->host_ip);

		// add numscanned to query
		if(min->numscanned)
			snprintf(&min_part[bpstrlen(min_part)], 40, " numscanned < \"%d\" AND ", min->numscanned);

		// add userid to query
		if(min->numup)
			snprintf(&min_part[bpstrlen(min_part)], 40, " numup < \"%d\" AND ", min->numup);

		// add runid to query
		if(min->runid)
			snprintf(&min_part[bpstrlen(min_part)], 40, " runid < \"%d\" AND ", min->runid);

		// add rec_id to query
		if(min->rec_id)
			snprintf(&min_part[bpstrlen(min_part)], 40, " rec_id < \"%d\" AND ", min->rec_id);


	}

	// ------------------- Max --------------------------------
	if(max)
	{
		// create string to hold the min part of the query
		max_part = (char *) bpcalloc(sizeof(BP_NMAPDB_RUNLIST)*3, 1);


		// add date (d) to query
		if(bpstrlen(max->d))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->d)-1+40, " d > \"%s\" AND ", max->d);

		// add time (t) to query
		if(bpstrlen(max->t))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->t)-1+40, " t > \"%s\" AND ", max->t);

		// add run_type to query
		if(max->run_type)
			snprintf(&max_part[bpstrlen(max_part)], 40, " run_type > \"%d\" AND ", max->run_type);

		// add user to query
		if(bpstrlen(max->user))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->user)-1+40, " user > \"%s\" AND ", max->user);

		// add userid to query
		if(max->userid)
			snprintf(&max_part[bpstrlen(max_part)], 40, " userid > \"%d\" AND ", max->userid);

		// add host to query
		if(bpstrlen(max->host))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->host)-1+40, " user > \"%s\" AND ", max->host);

		// add host_ip to query
		if(bpstrlen(max->host_ip))
			snprintf(&max_part[bpstrlen(max_part)], sizeof(max->user)-1+40, " host_ip > \"%s\" AND ", max->host_ip);

		// add numscanned to query
		if(max->numscanned)
			snprintf(&max_part[bpstrlen(max_part)], 40, " numscanned > \"%d\" AND ", max->numscanned);

		// add userid to query
		if(max->numup)
			snprintf(&max_part[bpstrlen(max_part)], 40, " numup > \"%d\" AND ", max->numup);

		// add runid to query
		if(max->runid)
			snprintf(&max_part[bpstrlen(max_part)], 40, " runid > \"%d\" AND ", max->runid);

		// add rec_id to query
		if(max->rec_id)
			snprintf(&max_part[bpstrlen(max_part)], 40, " rec_id > \"%d\" AND ", max->rec_id);

	}

	// ------------------- Like --------------------------------
	if(like)
	{
		// create string to hold the min part of the query
		like_part = (char *) bpcalloc(sizeof(BP_NMAPDB_RUNLIST)*3, 1);

		// add date (d) to query
		if(bpstrlen(like->d))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->d)-1+40, " d like \"%%%s%%\" AND ", like->d);

		// add time (t) to query
		if(bpstrlen(like->t))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->t)-1+40, " t like \"%%%s%%\" AND ", like->t);

		// add run_type to query
		if(like->run_type)
			snprintf(&like_part[bpstrlen(like_part)], 40, " run_type like \"%%%d%%\" AND ", like->run_type);

		// add user to query
		if(bpstrlen(like->user))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->user)-1+40, " user like \"%%%s%%\" AND ", like->user);

		// add userid to query
		if(like->userid)
			snprintf(&like_part[bpstrlen(like_part)], 40, " userid like \"%%%d%%\" AND ", like->userid);

		// add host to query
		if(bpstrlen(like->host))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->host)-1+40, " user like \"%%%s%%\" AND ", like->host);

		// add host_ip to query
		if(bpstrlen(like->host_ip))
			snprintf(&like_part[bpstrlen(like_part)], sizeof(like->user)-1+40, " host_ip like \"%%%s%%\" AND ", like->host_ip);

		// add numscanned to query
		if(like->numscanned)
			snprintf(&like_part[bpstrlen(like_part)], 40, " numscanned like \"%%%d%%\" AND ", like->numscanned);

		// add userid to query
		if(like->numup)
			snprintf(&like_part[bpstrlen(like_part)], 40, " numup like \"%%%d%%\" AND ", like->numup);

		// add runid to query
		if(like->runid)
			snprintf(&like_part[bpstrlen(like_part)], 40, " runid like \"%%%d%%\" AND ", like->runid);

		// add rec_id to query
		if(like->rec_id)
			snprintf(&like_part[bpstrlen(like_part)], 40, " rec_id like \"%%%d%%\" AND ", like->rec_id);

	}

	// ------------------- Equal --------------------------------
	if(equal)
	{

		// create string to hold the min part of the query
		equal_part = (char *) bpcalloc(sizeof(BP_NMAPDB_RUNLIST)*3, 1);


		// add date (d) to query
		if(bpstrlen(equal->d))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->d)-1+40, " d = \"%s\" AND ", equal->d);

		// add time (t) to query
		if(bpstrlen(equal->t))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->t)-1+40, " t = \"%s\" AND ", equal->t);

		// add run_type to query
		if(equal->run_type)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " run_type = \"%d\" AND ", equal->run_type);

		// add user to query
		if(bpstrlen(equal->user))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->user)-1+40, " user = \"%s\" AND ", equal->user);

		// add userid to query
		if(equal->userid)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " userid = \"%d\" AND ", equal->userid);

		// add host to query
		if(bpstrlen(equal->host))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->host)-1+40, " user = \"%s\" AND ", equal->host);

		// add host_ip to query
		if(bpstrlen(equal->host_ip))
			snprintf(&equal_part[bpstrlen(equal_part)], sizeof(equal->user)-1+40, " host_ip = \"%s\" AND ", equal->host_ip);

		// add numscanned to query
		if(equal->numscanned)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " numscanned = \"%d\" AND ", equal->numscanned);

		// add userid to query
		if(equal->numup)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " numup = \"%d\" AND ", equal->numup);

		// add runid to query
		if(equal->runid)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " runid = \"%d\" AND ", equal->runid);

		// add rec_id to query
		if(equal->rec_id)
			snprintf(&equal_part[bpstrlen(equal_part)], 40, " rec_id = \"%d\" AND ", equal->rec_id);

	}


	// ---------------- Run Query --------------------------------

	// query parts
	select_part = "select * from runlist ";

	// max length variable
	size_t max_len = bpstrlen(select_part);

	if(min_part)
		max_len += bpstrlen(min_part);

	if(max_part)
		max_len += bpstrlen(max_part);

	if(like_part)
		max_len += bpstrlen(like_part);

	if(equal_part)
		max_len += bpstrlen(equal_part);

	// allocate space for final query (2x max for paranoia)
	final_query = (char *) bpcalloc(max_len * 2, 1);

	// implement select path
	snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", select_part);

	// add where if necessary
	if(min_part || max_part || like_part || equal_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, " WHERE ");

	if(min_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", min_part);

	if(max_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", max_part);

	if(like_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", like_part);

	if(equal_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, "%s", equal_part);

	// add terminating 1=1 to last AND if necessary
	if(min_part || max_part || like_part || equal_part)
		snprintf(&final_query[bpstrlen(final_query)], max_len * 2 -1, " 1=1 ");


	// -------------- Resolve Query in Database -------------------------------

	// MySQL Query Bindings
	// Use With null terminated strings only, results are accessed through struct MYSQL pointer
	if(BP_MySQLQuery(mysql_db, final_query))
		return NULL;

	// Returns stored results after a query, NULL on failure
	P_BP_MYSQL_RESULT result = BP_MySQLStoreResult(mysql_db);

	// Returns a result set from a query
	MYSQL_ROW row = (MYSQL_ROW) NULL;

	// array of ports
	BP_NMAPDB_RUNLIST * runlist_array = NULL;

	// size index
	size_t i = 0;
	size_t x = 0;

	// recover results from the database
	for(
			i = 0;
			row = BP_MySQLFetchRow(result);
			i++
	)
	{

		// set mysql row index to 0 on loop
		x=0;

		// allocate space for the array
		runlist_array = (BP_NMAPDB_RUNLIST *) bprealloc(runlist_array, sizeof(BP_NMAPDB_RUNLIST) * (i+1));

		// copy in date (d)
		strncpy( runlist_array[i].d, row[x], sizeof( runlist_array[i].d ) -1);
		x++;

		// copy in time (t)
		strncpy( runlist_array[i].t, row[x], sizeof( runlist_array[i].t ) -1);
		x++;

		// copy in run_type
		runlist_array[i].run_type = atoi(row[x]);
		x++;

		// copy in user
		strncpy( runlist_array[i].user, row[x], sizeof( runlist_array[i].user ) -1);
		x++;

		// copy in userid
		runlist_array[i].userid = atoi(row[x]);
		x++;

		// copy in host
		strncpy( runlist_array[i].host, row[x], sizeof( runlist_array[i].host ) -1);
		x++;

		// copy in host_ip
		strncpy( runlist_array[i].host_ip, row[x], sizeof( runlist_array[i].host_ip ) -1);
		x++;

		// copy in numscanned
		runlist_array[i].numscanned = atoi(row[x]);
		x++;

		// copy in numup
		runlist_array[i].numup = atoi(row[x]);
		x++;

		// copy in runid
		runlist_array[i].runid = atoi(row[x]);
		x++;

		// copy in rec_id
		runlist_array[i].rec_id = atoi(row[x]);
		x++;

		// -- loop to next record --

	}

	// ---------------- Create Returnable Structure -------------

	// if the portstats array structure is filled out, we have a proper result and can
	// populate return values.
	P_BP_NMAPDB_RESULTS final_data = NULL;
	if(runlist_array)
	{

		// opaque result data
		final_data = (P_BP_NMAPDB_RESULTS) bpcalloc(sizeof(BP_NMAPDB_RESULTS), 1);

		// save query
		final_data->query = bpstrndup(final_query, 4096);

		// save opaque data as porstats_array
		final_data->opaque_data = (void *) runlist_array;

		// set type to portstat
		final_data->type = BP_NMAPDB_RESULT_RUNLIST;

		// set record count to i + 1
		final_data->record_count = i+1;

	}
	// ---------------- Free Allocated Data ----------------------

	if(min_part)
		bpfree(min_part);

	if(max_part)
		bpfree(max_part);

	if(like_part)
		bpfree(like_part);

	if(equal_part)
		bpfree(equal_part);

	if(final_query)
		bpfree(final_query);

	// return the final filled out data structure
	return final_data;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%% VISUALIZATION FUNCTIONS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// This function autodetects a result type and prints out the result.
int BP_NDBVisualizeResults(P_BP_NMAPDB_RESULTS result, size_t displayQuery)
{

	// make sure we have a result structure
	if(!result)
		return ERR_FAILURE;

	if(!result->opaque_data || !result->record_count)
		return ERR_FAILURE;

	size_t i = 0;

	// temporary pointers used for indexing through opaque result data
	BP_NMAPDB_RUNLIST   *tmp_runlist   = NULL;
	BP_NMAPDB_SCANNERS  *tmp_scanners  = NULL;
	BP_NMAPDB_TARGETS   *tmp_targets   = NULL;
	BP_NMAPDB_HOSTSTATS *tmp_hoststats = NULL;
	BP_NMAPDB_PORTSTATS *tmp_portstats = NULL;


	// switch to process data types
	switch(result->type)
	{
		case BP_NMAPDB_RESULT_RUNLIST:

			printf("\n [+] Runlist NmapDB Information: %u total records", result->record_count);

			// set opaque data as *structure
			tmp_runlist = (P_BP_NMAPDB_RUNLIST) result->opaque_data;

			// display individual data
			for(i = 0; i < result->record_count; i++)
				 BP_NDBVisualizeRunlist(&tmp_runlist[i]);

			break;

		case BP_NMAPDB_RESULT_SCANNERS:

			printf("\n [+] Scanners NmapDB Information: %u total records", result->record_count);

			// set opaque data as *structure
			tmp_scanners = (P_BP_NMAPDB_SCANNERS) result->opaque_data;

			// display individual data
			for(i = 0; i < result->record_count; i++)
				 BP_NDBVisualizeScanners(&tmp_scanners[i]);

			break;

		case BP_NMAPDB_RESULT_TARGETS:

			printf("\n [+] Targets NmapDB Information: %u total records", result->record_count);

			// set opaque data as *structure
			tmp_scanners = (P_BP_NMAPDB_SCANNERS) result->opaque_data;

			// display individual data
			for(i = 0; i < result->record_count; i++)
				 BP_NDBVisualizeTargets(&tmp_targets[i]);

			break;

		case BP_NMAPDB_RESULT_HOSTSTATS:

			printf("\n [+] Host Stats NmapDB Information: %u total records", result->record_count);

			// set opaque data as *structure
			tmp_hoststats = (P_BP_NMAPDB_HOSTSTATS) result->opaque_data;

			// display individual data
			for(i = 0; i < result->record_count; i++)
				 BP_NDBVisualizeHostStats(&tmp_hoststats[i]);

			break;

		case BP_NMAPDB_RESULT_PORTSTATS:

			printf("\n [+] Port Stats NmapDB Information: %u total records", result->record_count);

			// set opaque data as *structure
			tmp_portstats = (P_BP_NMAPDB_PORTSTATS) result->opaque_data;

			// display individual data
			for(i = 0; i < result->record_count; i++)
				BP_NDBVisualzePortStats(&tmp_portstats[i]);

			break;

		default:
			return ERR_FAILURE;
	}

	return ERR_SUCCESS;
}


// Visualizer for P_BP_NMAPDB_PORTSTATS structure
int BP_NDBVisualzePortStats(P_BP_NMAPDB_PORTSTATS data)
{
	if(!data)
		return ERR_FAILURE;

	printf("\n\tP_BP_NMAPDB_PORTSTATS: %p", data);
	printf("\n\t  ->d (date):     %s", data->d);
	printf("\n\t  ->t (time):     %s", data->t);
	printf("\n\t  ->hostid        %u", data->hostid);
	printf("\n\t  ->target_ip:    %s", data->target_ip);
	printf("\n\t  ->port:         %u", data->port);
	printf("\n\t  ->protocol:     %s", data->protocol);
	printf("\n\t  ->service:      %s", data->service);
	printf("\n\t  ->state:        %s", data->state);
	printf("\n\t  ->fullversion:  %s", data->fullversion);
	printf("\n\t  ->scannerid:    %u", data->scannerid);
	printf("\n\t  ->runid:        %u", data->runid);
	printf("\n\t  ->rec_id:       %u", data->rec_id);
	printf("\n");



	return ERR_SUCCESS;
}

// Visualizer for P_BP_NMAPDB_HOSTSTATS structure
int BP_NDBVisualizeHostStats(P_BP_NMAPDB_HOSTSTATS data)
{
	if(!data)
		return ERR_FAILURE;

	printf("\n\tP_BP_NMAPDB_HOSTSTATS: %p", data);
	printf("\n\t  ->hostid         %u", data->hostid);
	printf("\n\t  ->ip:            %s", data->ip);
	printf("\n\t  ->user:          %s", data->user);
	printf("\n\t  ->userid:        %u", data->userid);
	printf("\n\t  ->d (date):      %s", data->d);
	printf("\n\t  ->t (time):      %s", data->t);
	printf("\n\t  ->open_ports:    %u", data->open_ports);
	printf("\n\t  ->ports_scanned: %u", data->ports_scanned);
	printf("\n\t  ->runid:         %u", data->runid);
	printf("\n\t  ->scanner_id:    %u", data->scanner_id);
	printf("\n\t  ->rec_id:        %u", data->rec_id);
	printf("\n");

	return ERR_SUCCESS;
}

// Visualizer for P_NMAPDB_TARGETS structure
int BP_NDBVisualizeTargets(P_NMAPDB_TARGETS data)
{

	if(!data)
		return ERR_FAILURE;

	printf("\n\tP_BP_NMAPDB_TARGETS: %p", data);
	printf("\n\t  ->host:        %s", data->host);
	printf("\n\t  ->d (date):    %s", data->d);
	printf("\n\t  ->t (time):    %s", data->t);
	printf("\n\t  ->ip :         %s", data->ip);
	printf("\n\t  ->ipnum:       %u", data->ipnum);
	printf("\n\t  ->mac:         %s", data->mac);
	printf("\n\t  ->macvendor:   %s", data->macvendor);
	printf("\n\t  ->flag:        %u", data->flag);
	printf("\n\t  ->hostid:      %u", data->hostid);
	printf("\n\t  ->description: %s", data->description);
	printf("\n\t  ->os_guessed:  %s", data->os_guessed);
	printf("\n\t  ->os_known:    %s", data->os_known);
	printf("\n\t  ->fingerprint: %s", data->fingerprint);
	printf("\n\t  ->rec_id:      %u", data->rec_id);
	printf("\n");

	return ERR_SUCCESS;

}

// Visualizer for P_BP_NMAPDB_SCANNERS structure
int BP_NDBVisualizeScanners(P_BP_NMAPDB_SCANNERS data)
{

	if(!data)
		return ERR_FAILURE;

	printf("\n\tP_BP_NMAPDB_SCANNERS: %p", data);
	printf("\n\t  ->scanner:     %s", data->scanner);
	printf("\n\t  ->scanner_ip:  %s", data->scanner);
	printf("\n\t  ->description: %s", data->description);
	printf("\n\t  ->d (date):    %s", data->d);
	printf("\n\t  ->t (time):    %s", data->t);
	printf("\n\t  ->flag:        %u", data->flag);
	printf("\n\t  ->scanner_id:  %u", data->scanner_id);
	printf("\n\t  ->rec_id:      %u", data->rec_id);
	printf("\n");

	return ERR_SUCCESS;

}

// Visualizer for P_BP_NMAPDB_RUNLIST structure
int BP_NDBVisualizeRunlist(P_BP_NMAPDB_RUNLIST data)
{

	if(!data)
		return ERR_FAILURE;

	printf("\n\tP_BP_NMAPDB_RUNLIST: %p", data);
	printf("\n\t  ->d (date):   %s", data->d);
	printf("\n\t  ->t (time):   %s", data->t);
	printf("\n\t  ->run_type:   %u", data->run_type);
	printf("\n\t  ->user:       %s", data->user);
	printf("\n\t  ->userid:     %u", data->userid);
	printf("\n\t  ->host:       %s", data->host);
	printf("\n\t  ->host_ip:    %s", data->host_ip);
	printf("\n\t  ->numscanned: %u", data->numscanned);
	printf("\n\t  ->numup:      %u", data->numup);
	printf("\n\t  ->runid:      %u", data->runid);
	printf("\n\t  ->rec_id:     %u", data->rec_id);
	printf("\n");

	return ERR_SUCCESS;

}


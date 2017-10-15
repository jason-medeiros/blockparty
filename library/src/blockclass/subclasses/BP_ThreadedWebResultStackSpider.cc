/*
 * BP_ThreadedWebResultStackSpider.cc
 *
 *  Created on: Aug 23, 2013
 *      Author: root
 */

// include mainBP_ThreadedWebResultStackSpider
#include "../../../include/BP-Main.h"

// class constructor
BP_ThreadedWebResultStackSpider::BP_ThreadedWebResultStackSpider()
{

	// set the mysql pool to NULL
	this->pool = NULL;

	// managed blocks
	this->managed_blocks = NULL;
	this->managed_block_n = 0;

	// thread manager
	this->thread_mgr = NULL;

	// set the result db to null
	this->web_result_db = NULL;

	// set initialization flag to flase
	this->initialized = false;

}

// class deconstructor
BP_ThreadedWebResultStackSpider::~BP_ThreadedWebResultStackSpider()
{

	// destroy the pool if it exists
	if(this->pool)
		delete this->pool;
	if(this->thread_mgr)
		delete this->thread_mgr;
	if(this->web_result_db)
		delete this->web_result_db;

}


// intialization routine
BP_ERROR_T BP_ThreadedWebResultStackSpider::initConfig(BP_MySQLMultiThreadConnectionPool * pool, size_t num_threads)
{

	// ensure we have a pool
	if(!pool)
	{
		this->AddErr((char *) "[!!] Spider requires an accompanying mysql connection pool.");
		return ERR_FAILURE;
	}

	// check for threads
	if(!num_threads)
	{
		this->AddErr((char *) "[!!] Spider requires a non-zero thread count.  Zero provided.");
		return ERR_FAILURE;
	}

	// set pool
	this->pool = pool;
	if(!this->pool)
		return ERR_FAILURE;

	// set thread_mgr;
	this->thread_mgr = new Block();
	if(!this->thread_mgr)
		return ERR_FAILURE;

	// create block used for pulling results from the db
	this->web_result_db = new Block();
	if(!this->web_result_db)
		return ERR_FAILURE;

	// set initialization flag to indicate class has been initialized
	this->initialized = true;

	// return indicating success
	return ERR_SUCCESS;

}

// set stack generation query (matches parameters passed into fetchWebResultStackFromDB)
BP_ERROR_T BP_ThreadedWebResultStackSpider::setWebResultStackGenerationParameters
(
		_vin size_t lock_for_audit,
		_vin size_t lock_for_spidering,
		_vdbin size_t id,                        /* id = conditional */
		_vdbin size_t assessment_id,             /* assessment_id = conditional */
		_vdbin size_t type,                      /* type = conditional */
		_vdbin char * sha1,                      /* sha1 = str_conditional */
		_vdbin char * href_literal,              /* literal href match (can be null) */
		_vdbin char * href_like,                 /* LIKE href match condition (can be null) */
		_vdbin char * href_regexp,               /* REGEXP href match condition (can be null) */
		_vdbin char * base_literal,              /* literal base match */
		_vdbin char * base_like,                 /* LIKE base match */
		_vdbin char * base_regexp,               /* REGEXP base match */
		_vdbin char * path_literal,              /* literal path match */
		_vdbin char * path_like,                 /* LIKE path match */
		_vdbin char * path_regexp,               /* REGEXP path match */
		_vdbin size_t status,                    /* general audit status condition */
		_vdbin char * commit_ts_ge,              /* commit timestame greater equal */
		_vdbin char * commit_ts_le,              /* commit_timestamp less than equal to */
		_vdbin char * audit_start_ts_ge,         /* audit start greater equal */
		_vdbin char * audit_start_ts_le,         /* audit start less than equal */
		_vdbin char * audit_completed_ts_ge,     /* audit completed greater equal */
		_vdbin char * audit_completed_ts_le,     /* audit completed less than equal */
		_vdbin size_t spidered,                  /* spidered status */
		_vdbin size_t spidered_parent_result_id, /* parent id of entry */
		_vdbin char * spidered_ts_ge,            /* spidered ts greater equal */
		_vdbin char * spidered_ts_le ,           /* spidered ts less equal */
		_vdbin size_t limit_bottom,              /* limit bottom, top */
		_vdbin size_t limit_top,                 /* limit bottom, top */
		_vin   size_t randomize_select_ids,      /* set to non-zero to return random ids */
		_vdbin char * join_injection_sql,        /* this is direct SQL (dangerous) */
		_vdbin char * where_injection_sql        /* direct sql in the WHERE position.  be careful */
)
{

	if(!this->initialized)
	{
		this->AddErr((char *) "[!!] This spider class must be initialized before use.");
	}

	// return indicating success
	return ERR_SUCCESS;

}

/*
 * BP_ThreadedWebResultStackSpider.h
 *
 *  Created on: Aug 23, 2013
 *      Author: root
 */

#ifndef BP_THREADEDWEBRESULTSTACKSPIDER_H_
#define BP_THREADEDWEBRESULTSTACKSPIDER_H_

// spider thread routine (communicates with ThreadedWebResultStackSpiderDirectly)


class BP_ThreadedWebResultStackSpider : BP_BaseError
{

	// connection pool to use when doing updates
	BP_MySQLMultiThreadConnectionPool * pool;

	// web result db block.  Used for pulling in targets from the db.  The
	// results pulled are then divided and shared among the threads.
	Block * web_result_db;

	// actual references to managed blocks in individual threads.  Should only be accessed
	// when block dispatched thread is stopped.  (easy to petition)
	Block ** managed_blocks;
	size_t   managed_block_n;

	// thread manager class responsible for dispatching threads.
	Block *  thread_mgr;

	// initialization flag, used for detecting bad syntax use
	bool initialized;

	// constructor and deconstructor
	BP_ThreadedWebResultStackSpider();
	~BP_ThreadedWebResultStackSpider();

	// intialization routine
	BP_ERROR_T initConfig(BP_MySQLMultiThreadConnectionPool * pool, size_t num_threads);

	// set stack generation query (matches parameters passed into fetchWebResultStackFromDB)
	BP_ERROR_T setWebResultStackGenerationParameters
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
	);


};


#endif /* BP_THREADEDWEBRESULTSTACKSPIDER_H_ */

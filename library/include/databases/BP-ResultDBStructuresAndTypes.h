/*
 * BP-ResultDBStructuresAndTypes.h
 *
 *  Created on: Sep 2, 2013
 *      Author: root
 */

#ifndef BP_RESULTDBSTRUCTURESANDTYPES_H_
#define BP_RESULTDBSTRUCTURESANDTYPES_H_



// Result type enum
enum BP_DB_TARGET_RESULT_TYPE {
	BP_DB_TARGET_RESULT_TYPE_DEFAULT,             /* 0 */
	BP_DB_TARGET_RESULT_TYPE_GOOGLE,              /* 1 */
	BP_DB_TARGET_RESULT_TYPE_BING,                /* 2 */
	BP_DB_TARGET_RESULT_TYPE_YAHOO,               /* 3 */
	BP_DB_TARGET_RESULT_TYPE_BLOCKPARTY_CRAWLER,  /* 4 */
	BP_DB_TARGET_RESULT_TYPE_BLOCKPARTY_MISC1,    /* 5 */
	BP_DB_TARGET_RESULT_TYPE_BLOCKPARTY_MISC2,    /* 6 */
	BP_DB_TARGET_RESULT_TYPE_BLOCKPARTY_MISC3,    /* 7 */
	BP_DB_TARGET_RESULT_TYPE_BLOCKPARTY_MISC4,    /* 8 */
	BP_DB_TARGET_RESULT_TYPE_BLOCKPARTY_MISC5,    /* 9 */
	BP_DB_TARGET_RESULT_TYPE_PREGEXP_SCOUR,       /* 10 */
	BP_DB_TARGET_RESULT_TYPE_DUCKDUCKGO           /* 11 */
};


// can be passed in to a web result lookup stack
typedef struct __BP_WEB_RESULT_STACK_LOOKUP_PARMS {

	size_t lock_for_audit;
	size_t lock_for_spidering;
	size_t lock_for_resolution_update;
	size_t id;                        /* id = conditional */
	size_t assessment_id;             /* assessment_id = conditional */
	size_t type;                      /* type = conditional */
	char * sha1;                      /* sha1 = str_conditional */
	char * href_literal;              /* literal href match (can be null) */
	char * href_like;                 /* LIKE href match condition (can be null) */
	char * href_regexp;               /* REGEXP href match condition (can be null) */
	size_t host;               		  /* associated base id */
	size_t host_distinct;             /* wraps select base in a DISTINCT() clause */
	size_t base;               		  /* associated base id */
	size_t base_distinct;             /* wraps select base in a DISTINCT() clause */
	size_t path;             		  /* associated path id */
	size_t path_distinct;             /* wraps select base in a DISTINCT() clause */
	size_t script;            		  /* associated script id */
	size_t script_distinct;           /* wraps select base in a DISTINCT() clause */
	size_t result_is_host;            /* joins on the result_hosts table, returning a host */
	size_t result_is_base;            /* joins on the result_base_uris table, returning a base instead of a the value in the results table */
	size_t result_is_path;            /* joins on the path table, returning a path instead of a the value in the results table */
	size_t result_is_script;          /* joins on the script table, returning a script instead of a the value in the results table */
	size_t status;                    /* general audit status condition */
	size_t update_resolution_status;  /* when a record is being resolved, this flag locks the record */
	size_t origin_type_form_or_anchor_code; /* hard code from blockparty indicating what type of anchor/form or whatever combination this was built from */
	char * commit_ts_ge;              /* commit timestame greater equal */
	char * commit_ts_le;              /* commit_timestamp less than equal to */
	char * audit_start_ts_ge;         /* audit start greater equal */
	char * audit_start_ts_le;         /* audit start less than equal */
	char * audit_completed_ts_ge;     /* audit completed greater equal */
	char * audit_completed_ts_le;     /* audit completed less than equal */
	size_t spidered;                  /* spidered status */
	size_t spidered_parent_result_id; /* parent id of entry */
	char * spidered_ts_ge;            /* spidered ts greater equal */
	char * spidered_ts_le;            /* spidered ts less equal */
	size_t hits;                      /* literal hit count (-1 to disable)*/
	size_t hits_ge;                   /* hits greater than or equal this value */
	size_t hits_le;                   /* hits less than or equal this value */
	size_t disabled;                  /* disabled match */
	size_t disable_code;              /* code indicating why the record was disabled */
	size_t disabled_path;             /* disabled path match */
	size_t disabled_path_code;        /* disabled path code match */
	size_t disabled_host;             /* disabled host match */
	size_t disabled_host_code;        /* disabled_host_code */
	size_t disabled_base;             /* disabled base match */
	size_t disabled_base_code;        /* disabled_base_code */
	size_t disabled_script;           /* disabled script match */
	size_t disabled_script_code;      /* disabled script code */
	size_t limit_bottom;              /* limit bottom, top */
	size_t limit_top;                 /* limit bottom, top */
	size_t randomize_select_ids;      /* set to non-zero to return random ids */
	char * join_injection_sql;       /* this is direct SQL suitable for the JOIN position (dangerous) */
	char * where_injection_sql;        /* direct sql in the WHERE position.  be careful */
} BP_WEB_RESULT_STACK_LOOKUP_PARMS, *P_BP_WEB_RESULT_STACK_LOOKUP_PARMS;



#endif /* BP_RESULTDBSTRUCTURESANDTYPES_H_ */

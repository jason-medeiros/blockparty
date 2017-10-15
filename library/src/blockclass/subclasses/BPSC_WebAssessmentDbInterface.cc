// include main
#include "../../../include/BP-Main.h"

//! Class constructor.
BPSC_WebAssessmentDbInterface::BPSC_WebAssessmentDbInterface(Block *mysqli)
{

	// set mysql object
	this->mysqli = mysqli;

	// set assessment as null
	this->assessment = NULL;

}

//! Store the assessment data.
BP_ERROR_T BPSC_WebAssessmentDbInterface::storeAssessment()
{

	// verify that we have an assessment
	if(!this->assessment)
	{
		this->AddErr("No assessment to store.");
		return ERR_FAILURE;
	}

	// store the sqli data in the database
	this->storeSqliData();

	// store the xss data in the database
	this->storeXssData();

	// return indicating success
	return ERR_SUCCESS;

}

//! Store sqli test results.
BP_ERROR_T BPSC_WebAssessmentDbInterface::storeSqliData()
{

	// check for sql results
	if(!this->assessment->sqlResults)
	{
		this->AddErr("No sql reqults (null).");
		return ERR_FAILURE;
	}

	// create a list iterator
	P_BP_WEB_SQLINJ_TEST_RESULT sqli_iter = this->assessment->sqlResults->first;

	// base insertion query
	char * sql_result_insertion_query = "INSERT INTO sql_messages ( result_index, http_get_input, http_post_input ) VALUES ( ?, ?, ? ); ";

	// empty string
	char *empty = "";

	// set form action target
	char * form_action_target = NULL;

	// walk the list
	for(
		;
		sqli_iter;
		sqli_iter = sqli_iter->next
		)
	{

		// list_iter->anchor
		this->mysqli->mysqlPrepareQuery(sql_result_insertion_query);

		size_t result_id = 0;
		this->mysqli->mysqlAddParameter(MYSQL_TYPE_LONG,   (char *) &result_id, sizeof(size_t));

		// - http_get_input
		if (sqli_iter->anchorActualTest)
			this->mysqli->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) sqli_iter->anchorActualTest, bpstrlen(sqli_iter->anchorActualTest));
		else
			this->mysqli->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) form_action_target, bpstrlen(form_action_target));

		// - http_post_input
		if (sqli_iter->actualInputTest)
			this->mysqli->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) sqli_iter->actualInputTest, bpstrlen(sqli_iter->actualInputTest));
		else
			this->mysqli->mysqlAddParameter(MYSQL_TYPE_STRING, (char *) empty, bpstrlen(empty));

		// bind parameters
		this->mysqli->mysqlBindParameters();

		// execute statement
		if(this->mysqli->mysqlExecuteStatement() != ERR_SUCCESS)
		{
			printf("\n Failed to execute insert query.");
			return ERR_FAILURE;
		}

		printf("\n Query executed.");

		// IN THE AM: Insert request data.


	}

	// return indicating success
	return ERR_SUCCESS;

}

//! Store xss test results.
BP_ERROR_T BPSC_WebAssessmentDbInterface::storeXssData()
{


	return ERR_SUCCESS;
}

//! Store get data.
BP_ERROR_T BPSC_WebAssessmentDbInterface::storeGetData()
{

	// post values
	char * get_name_query = "INSERT INTO `grief`.`get_name_data` ( `id` ,`result_id` ,`var_name`) VALUES ( NULL , ?, ? );";
	char * get_val_query  = "INSERT INTO `grief`.`get_val_data`  ( `id` ,`name_id` ,`var_val`)  VALUES ( NULL , ?, ? );";

	// return indicating success
	return ERR_SUCCESS;

}
//! Store post data.
BP_ERROR_T BPSC_WebAssessmentDbInterface::storePostData()
{

	// post values
	char * post_name_query = "INSERT INTO `grief`.`post_name_data` ( `id` ,`result_id` ,`var_name`) VALUES ( NULL , ?, ? );";
	char * post_val_query  = "INSERT INTO `grief`.`post_val_data`  ( `id` ,`name_id` ,  `var_val`)  VALUES ( NULL , ?, ? );";

	// return indicating success
	return ERR_SUCCESS;

}

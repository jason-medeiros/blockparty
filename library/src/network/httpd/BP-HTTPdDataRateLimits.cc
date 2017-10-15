// include main header
#include "../../../include/BP-Main.h"


// --- init/update timmers -----------------------

// initialize timers
BP_ERROR_T BP_HTTPdRateLimitInitTimers(P_BP_HTTPD_DATA_RATE_LIMITER rate_limits)
{

	// check rate limits
	if(!rate_limits)
		return ERR_FAILURE;

	// retrieve the initial timestamp
	rate_limits->initial_timestamp = BP_TimeGetSecondsSinceUnixEpoch();

	// if we can't set our initial timestamp, return indicating failure
	if(!rate_limits->initial_timestamp)
		return ERR_FAILURE;

	// set current as initial
	rate_limits->current_sample_timestamp = rate_limits->initial_timestamp;

	// set last sample timestamp (should always be zero, but set it anyway)
	rate_limits->last_sample_timestamp = 0;

	// return indicating success
	return ERR_SUCCESS;

}


// update timers
BP_ERROR_T BP_HTTPdRateLimitUpdateTimers(P_BP_HTTPD_DATA_RATE_LIMITER rate_limits)
{

	// check rate limits
	if(!rate_limits)
		return ERR_FAILURE;

	// if we can't set our initial timestamp, return indicating failure
	if(!rate_limits->initial_timestamp)
		return ERR_FAILURE;

	// ensure our current sample timestamp is set
	if(!rate_limits->current_sample_timestamp)
		return ERR_FAILURE;

	// set last sample timestamp (should always be zero, but set it anyway)
	rate_limits->last_sample_timestamp = rate_limits->current_sample_timestamp;

	// set current as initial
	rate_limits->current_sample_timestamp = BP_TimeGetSecondsSinceUnixEpoch();

	// exit failure if we were unable to update the current timestamp
	if(!rate_limits->current_sample_timestamp)
		return ERR_FAILURE;

	// calculate timestamp difference
	rate_limits->difference_in_seconds_between_current_and_last_timestamp = \
			rate_limits->current_sample_timestamp - rate_limits->last_sample_timestamp;

	// calculate initial and current timestamp difference
	rate_limits->difference_in_seconds_between_initial_and_current_timestamp = \
			rate_limits->current_sample_timestamp - rate_limits->initial_timestamp ;

	// return indicating success
	return ERR_SUCCESS;

}


// --- rate adjustments -------------



// Adjust rate for user based on new add data.
BP_ERROR_T BP_HTTPdRatesLimitAdjustRateForUser
(
	P_BP_HTTPD_USER user,
	size_t send_rate_add,
	size_t recv_rate_add
)
{

	// return indicating success
	return ERR_SUCCESS;

}

// Adjust rate for request handler based on new add data.
BP_ERROR_T BP_HTTPdRateLimitAdjustRatesForRequestHandler
(
	P_BP_HTTPD_REQUEST_HANDLER request_handler,
	size_t send_rate_add,
	size_t recv_rate_add
)
{

	// return indicating success
	return ERR_SUCCESS;

}

// Adjust rate for websocket based on new add data.
BP_ERROR_T BP_HTTPdRateLimitAdjustRatesForWebsocket
(
	P_BP_HTTPD_WEBSOCKET websocket,
	size_t               send_rate_bytes_add,
	size_t               recv_rate_bytes_add,
	size_t               send_rate_frames_add,
	size_t               recv_rate_frames_add
)
{

	// ensure we have a websocket and a registry
	if(!websocket)
		return ERR_FAILURE;
	if(!websocket->hreg)
		return ERR_FAILURE;

	// enter critical section
	BP_HASH_TABLE_ENTER_CRITICAL_SECTION(websocket->hreg);



	// --- dereference rate limits from websocket -----

	// create local reference to rate limits
	P_BP_HTTPD_DATA_RATE_LIMITER rate_limits = &websocket->connection_policy.socket_rate_limits;

	// initialize the timer if we have no initial timestamp
	if(!rate_limits->initial_timestamp)
	{

		// initialize the timers
		if(!BP_HTTPdRateLimitInitTimers(rate_limits))
		{
			BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(websocket->hreg, ERR_FAILURE);
		}

	}


	// --- adjust send/recv bytes -----------------------


	// add bytes
	if(send_rate_bytes_add)
	{
		rate_limits->total_websocket_send_bytes += send_rate_bytes_add;
	}

	// add recv bytes
	if(recv_rate_bytes_add)
	{
		rate_limits->total_websocket_recv_bytes += recv_rate_bytes_add;
	}

	// --- adjust send/recv frames ---------------------

	// add to the total sent frames
	if(send_rate_frames_add)
	{
		rate_limits->total_websocket_send_frames += send_rate_frames_add;
	}

	// add to the total recv frames
	if(recv_rate_frames_add)
	{
		rate_limits->total_websocket_recv_frames += recv_rate_frames_add;
	}


	// --- preserve rate limits before calculation ----

	// create and preserve rate limits
	BP_HTTPD_DATA_RATE_LIMITER rate_limits_preserved;
	BP_ZERO_STACK(rate_limits_preserved);

	// copy in the rate limiter to preserve values
	memcpy(&rate_limits_preserved, rate_limits, sizeof(BP_HTTPD_DATA_RATE_LIMITER));


	// --- update timers after adjustment ------------

	// update timers
	if(!BP_HTTPdRateLimitUpdateTimers(rate_limits))
	{
		BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(websocket->hreg, ERR_FAILURE);
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Adjust Recv Bytes %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// check if we have a difference in seconds, if there is no difference, simply increase
	// the rate linearally.  If we do have a difference, divide by rate (disgard remainders).
	if(rate_limits->difference_in_seconds_between_initial_and_current_timestamp)
	{

		// if the initial timestamp isn't set, set it here
		if(!rate_limits->avg_websocket_recv_bytes_per_second_since_initial_timestamp)
		{
			rate_limits->avg_websocket_recv_bytes_per_second_since_initial_timestamp = recv_rate_bytes_add;
		}

		// calculate the average since initial
		rate_limits->avg_websocket_recv_bytes_per_second_since_initial_timestamp =\
				rate_limits->total_websocket_recv_bytes / rate_limits->difference_in_seconds_between_initial_and_current_timestamp;

	}

	// adjust bytes per second
	if(rate_limits->difference_in_seconds_between_current_and_last_timestamp)
	{

		// set curr websocket recv bytes per second
		if(!rate_limits->curr_websocket_recv_bytes_per_second)
		{
			rate_limits->curr_websocket_recv_bytes_per_second          = recv_rate_bytes_add;
			rate_limits->curr_websocket_recv_bytes_between_measurement = recv_rate_bytes_add;
		}

		// create the rate from previous and bytes
		rate_limits->curr_websocket_recv_bytes_per_second =\
				rate_limits->curr_websocket_recv_bytes_between_measurement \
				/ rate_limits->difference_in_seconds_between_current_and_last_timestamp;

		// reset the preserved measurement
		rate_limits->curr_websocket_recv_bytes_between_measurement = 0;

	}
	else
	{

		// perform adjustment
		rate_limits->curr_websocket_recv_bytes_between_measurement += recv_rate_bytes_add;
		rate_limits->curr_websocket_recv_bytes_per_second          += recv_rate_bytes_add;
		// increase the bytes added
		// rate_limits->curr_websocket_recv_bytes_per_second          += recv_rate_bytes_add;


	}

	// check recv exception
	BP_HTTPdRateLimitCheckRecvExceedsLimit(&websocket->connection_policy.socket_rate_limits);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Adjust Send Bytes %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%





	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Adjust Recv Frames %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Adjust Send Frames %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	printf("\n Adjusting rates for websocket?!:                              %p", websocket->connection_policy.socket_rate_limits);
	printf("\n recv_rate_bytes_add:                                          %u", recv_rate_bytes_add);
	printf("\n curr_websocket_recv_bytes_per_second?:                        %u", rate_limits->curr_websocket_recv_bytes_per_second);
	printf("\n total_websocket_recv_bytes?:                                  %u", rate_limits->total_websocket_recv_bytes);
	printf("\n avg_websocket_recv_bytes_per_second_since_initial_timestamp:  %u", rate_limits->avg_websocket_recv_bytes_per_second_since_initial_timestamp);
	printf("\n curr_websocket_recv_frames_per_second:                        %u", rate_limits->curr_websocket_recv_frames_per_second);
	printf("\n avg_websocket_recv_frames_per_second_since_initial_timestamp: %u", rate_limits->avg_websocket_recv_frames_per_second_since_initial_timestamp);

	printf("\n");
	printf("\n");


	// leave critical section and return
	BP_HASH_TABLE_LEAVE_CRITICAL_SECTION_AND_RETURN_FROM_FUNC(websocket->hreg, ERR_SUCCESS);

}


// --- check rate limits -------------------

// Simple check to determine if any of the rates within the rate limit structure, has been
// exceeded.
BP_BOOL BP_HTTPdRateLimitCheckAnyRateExceedsLimit(P_BP_HTTPD_DATA_RATE_LIMITER rate_limit)
{

	// ensure we have a rate limit
	if(!rate_limit)
		return BP_FALSE;

	// run checks
	if(BP_HTTPdRateLimitCheckSendRateExceedsLimit(rate_limit))
		return BP_TRUE;
	if(BP_HTTPdRateLimitCheckRecvExceedsLimit(rate_limit))
		return BP_TRUE;

	// return indicating that the rate limit is not above our set limit
	return BP_FALSE;

}


// One or more send rates has been exceeded.
BP_BOOL BP_HTTPdRateLimitCheckSendRateExceedsLimit(P_BP_HTTPD_DATA_RATE_LIMITER rate_limit)
{

	// ensure we have a rate limit
	if(!rate_limit)
		return BP_FALSE;

	// return indicating that no send rate has been exceeded
	return BP_FALSE;

}

// One or more recv rates has been exceeded.
BP_BOOL BP_HTTPdRateLimitCheckRecvExceedsLimit(P_BP_HTTPD_DATA_RATE_LIMITER rate_limit)
{

	// check to see if we have a rate limit
	if(!rate_limit)
		return BP_FALSE;

	// if the number of websocket recv bytes per second is greater than the
	// number of recv bytes capped, return indicating so.
	if(rate_limit->curr_websocket_recv_bytes_per_second > rate_limit->max_websocket_recv_bytes_per_second)
	{

		// sleep test
		if((rate_limit->curr_websocket_recv_bytes_per_second / rate_limit->max_websocket_recv_bytes_per_second) >= 1)
		{

			// calculate the byte difference
			float byte_difference = rate_limit->curr_websocket_recv_bytes_per_second - rate_limit->max_websocket_recv_bytes_per_second;

			// perform floating point calculation
			float tmp = (byte_difference / rate_limit->max_websocket_recv_bytes_per_second) * 100;

			// set the percentage of a second
			size_t percentage_of_second = (size_t) tmp;

			// set the sleep duration
			size_t sleep_duration = percentage_of_second * 10000;

			// reduce the recv bytes per second
			rate_limit->curr_websocket_recv_bytes_per_second -= byte_difference;

			printf("\n Sleeping to decrease kbps: %f - %u - %u - %f", byte_difference, rate_limit->max_websocket_recv_bytes_per_second, sleep_duration, tmp);
			printf("\n");
			printf("\n");

			// sleep for the nececessary duration
			usleep(sleep_duration);

		}
		return BP_TRUE;
	}

	// return indicating that no send rate has been exceeded
	return BP_FALSE;
}




// --- timer limits -----------------------


// Reset the rate limits.
BP_ERROR_T BP_HTTPdRateLimitResetLimits(P_BP_HTTPD_DATA_RATE_LIMITER rate_limits)
{


	// return indicating success
	return ERR_SUCCESS;

}

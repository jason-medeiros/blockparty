#include "../../../../include/BP-Main.h"

// BP_PromiseDefaultRequestHandler defined here is used as a default handler for handling
// promisary requests.
BP_HTTPD_DECLARE_ACCESS_HANDLER_CALLBACK (BP_PromiseDefaultRequestHandler )
{

	// run basic prologue
	BP_REQUEST_HANDLER_BASIC_PROLOGUE;

    // attempt setup of request (required for all secure requests)
    BP_ERROR_T req_setup_result = BP_HTTPDRequestHandlerSecureAuthEntrySetupJSON(BP_HTTPD_ACCESS_HANDLER_PARAMS);
    if(!req_setup_result)
            return ERR_FAILURE;

    // ensure we have a httpd
    if(!req->httpd)
    	return ERR_FAILURE;


    /*
     *
     * TOMORROW MORNING RETURN HERE:
     *   - We need to get a hook into a running event system.
     *   - We need to determine the request query syntax by parsing json.
     *   - We need to create a new promise.
     *   - We need to route the promise to a request fufilment engine by adding an event.
     *   - We need to create an event addition reciept for the promise.
     *   - Send the reciept back to the JS UI.
     *
     */



	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Peer Reciept Handlers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// TODO: Build out peer-reciept handlers (peers request reciept tables between hosts for easier spread pickup)
BP_HTTPD_DECLARE_ACCESS_HANDLER_CALLBACK(BP_PromiseAbsorbRemotePromiseReciepts)
{

	// run basic prologue
	BP_REQUEST_HANDLER_BASIC_PROLOGUE;

	// attempt setup of request (required for all secure requests)
	BP_ERROR_T req_setup_result = BP_HTTPDRequestHandlerSecureAuthEntrySetupJSON(BP_HTTPD_ACCESS_HANDLER_PARAMS);
	if(!req_setup_result)
			return ERR_FAILURE;

	// return indicating success
	return ERR_SUCCESS;

}

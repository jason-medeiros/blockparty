/*
 * BP-HTTPdRequestHandlerHelpers.h
 *
 *  Created on: Apr 2, 2016
 *      Author: root
 */

#ifndef INCLUDE_NETWORK_HTTPD_BP_HTTPDREQUESTHANDLERHELPERS_H_
#define INCLUDE_NETWORK_HTTPD_BP_HTTPDREQUESTHANDLERHELPERS_H_

// This function runs secure auth entry requests.  This is a rewrite of BP_HTTPD_REQUEST_HANDLER_SECURE_AUTH_ENTRY_SETUP_JSON.
BP_ERROR_T BP_HTTPDRequestHandlerSecureAuthEntrySetupJSON
(
		void                          * request, /* P_BP_HTTPD_REQUEST */
		struct MHD_Connection *         connection,
		const char *                    url,
		const char *                    method,
		const char *                    version,
		const char *                    upload_data,
		size_t     *                    upload_data_size,
		void       **                   con_cls
);

// respond json from static text (this was originally BP_HTTPdRequestHandlerRespondJSONFromStaticText)
BP_ERROR_T BP_HTTPdRequestHandlerRespondJSONFromStaticText(P_BP_HTTPD_REQUEST req, BP_ERROR_T ret_val, char *ret_json);


#endif /* INCLUDE_NETWORK_HTTPD_BP_HTTPDREQUESTHANDLERHELPERS_H_ */

/*
 * BP-EmailSMTP.cc
 *
 *  Created on: Dec 31, 2011
 *      Author: root
 */


#include "../../../include/BP-Main.h"

static const char *payload_text[]={
  "Subject: Test test.\n",
  "\n", /* empty line to divide headers from body, see RFC5322 */
  "Test.\n",
  "\n",
  "Test.\n",
  NULL
};

// upload status structure
struct upload_status
{
	char **payload_text;
	int lines_read;
};

static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp)
{


	struct upload_status *upload_ctx = (struct upload_status *)userp;
	const char *data;

	if ((size == 0) || (nmemb == 0) || ((size*nmemb) < 1))
	{
		return 0;
	}

	// set data
	data = upload_ctx->payload_text[upload_ctx->lines_read];

	if(data)
	{

		size_t len = bpstrlen(data);

		memcpy(ptr, data, len);

		upload_ctx->lines_read ++;

		return len;

	}

  return 0;

}

// opens a new curl session for the purposes of sending email via SMTP
P_BP_EMAIL_SMTP_SESSION BP_OpenSMTPEmailSession(char *username, char *password,	char *smtp_server, size_t smtp_port)
{

	// check for server and port
	if(!smtp_server || !smtp_port)
		return NULL;

	// session pointer (returned)
	P_BP_EMAIL_SMTP_SESSION session = NULL;

	// allocate space for the session
	session = (P_BP_EMAIL_SMTP_SESSION) bpcalloc(sizeof(BP_EMAIL_SMTP_SESSION), 1);
	if(!session)
		return NULL;

	// create new curl object
	session->curl = curl_easy_init();
	if(!session->curl)
	{
		bpfree(session);
		return NULL;
	}

	// create buffer to store url composite
	char url_str[8096] = {0};

	// create the connect string
	snprintf(url_str, 4096, "smtp://%s:%u", smtp_server, smtp_port);

	// #########################################################
	// ### Set CURL Parameters #################################
	// #########################################################

	// set the url string
	curl_easy_setopt(session->curl, CURLOPT_URL, url_str);

	// set username and password in curl structure
	if(username)
	 	curl_easy_setopt(session->curl, CURLOPT_USERNAME, username);
    if(password)
    	curl_easy_setopt(session->curl, CURLOPT_PASSWORD, password);

    // set tlsv123 as default ssl data
    curl_easy_setopt(session->curl, CURLOPT_USE_SSL, (long) CURLUSESSL_TRY);

    // set ssl verification policy
    curl_easy_setopt(session->curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(session->curl, CURLOPT_SSL_VERIFYHOST, 0L);

    // set the read function
    curl_easy_setopt(session->curl, CURLOPT_READFUNCTION, payload_source);

	// return session
	return session;

}


// destroys a smtp email session ( destroy the smtp session )
BP_ERROR_T BP_CloseDestroySMTPEmailSession(P_BP_EMAIL_SMTP_SESSION session)
{

	// check for session
	if(!session)
		return ERR_FAILURE;

	// set n to zero
	size_t n = 0;

	// clean curl
    curl_easy_cleanup(session->curl);

    // bpfree the session data
    bpfree(session);

    // return indicating success
    return ERR_SUCCESS;

}



// creates a new message structure.  All pointer arrays must be null ptr terminated.  Suffer under
// peril if you omit this warning.
P_BP_EMAIL_SMTP_MESSAGE BP_CreateSMTPMessage(char **recipients, char *mail_from, char **headers, char **message_body)
{

	// create new message
	P_BP_EMAIL_SMTP_MESSAGE message = (P_BP_EMAIL_SMTP_MESSAGE) bpcalloc(sizeof(BP_EMAIL_SMTP_MESSAGE), 1);

	// loop iterator for counting arrays
	size_t n = 0;

	// set mail from if its there
	if(mail_from)
	{
		snprintf(message->mail_from, 1000, "%s", mail_from);
	}

	// set recipients if we have any
	if(recipients)
	if(recipients[0])
	{

		// count the elements
		for(n=0; recipients[n]; n++)
		{

			// bpreallocate space on loop
			message->recipients = (char **) bprealloc(message->recipients, sizeof(char *) * (n+2));

			// set recipients (duplicate string)
			message->recipients[n]   = bpstrndup(recipients[n], 512);
			message->recipients[n+1] = NULL;

		}

		// set the count
		message->recipient_cnt = n;

	}

	// -- now set headers ---

	// set recipients if we have any
	if(headers)
	if(headers[0])
	{

		// count the elements
		for(n=0; headers[n]; n++)
		{

			// bpreallocate space on loop
			message->headers = (char **) bprealloc(message->headers, sizeof(char *) * (n+2));

			// set headers (duplicate string)
			message->headers[n]   = bpstrndup(headers[n], 512);
			message->headers[n+1] = NULL;

		}

		// set the count
		message->header_cnt = n;

	}

	// -- message body ---

	// set recipients if we have any
	if(message_body)
	if(message_body[0])
	{

		// count the elements
		for(n=0; message_body[n]; n++)
		{

			// bpreallocate space on loop
			message->message_body = (char **) bprealloc(message->message_body, sizeof(char *) * (n+2));

			// set headers (duplicate string)
			message->message_body[n]   = bpstrndup(message_body[n], 512);
			message->message_body[n+1] = NULL;

		}

		// set the count
		message->message_line_cnt = n;

	}

	// return the new message
	return message;

}

// destroys a message structure (null pointer after destroyed to prevent reuse)
BP_ERROR_T BP_DestroySMTPMessage(P_BP_EMAIL_SMTP_MESSAGE message)
{

	// check to see that we have a message
	if(!message)
		return ERR_FAILURE;

	// generic loop iterator used to process unallocation
	size_t n = 0;

	// list of recipients
	if(message->recipient_cnt)
	for(n=0; n < message->recipient_cnt; n++)
	{

		// check for a pointer and bpfree
		if(message->recipients[n])
			bpfree(message->recipients[n]);

		// zero out pointer
		message->recipients[n] = NULL;

	}

	// set message recipients
	if(message->recipients)
		bpfree(message->recipients);

	// bpfree list of headers
	if(message->header_cnt)
	for(n=0; n < message->header_cnt; n++)
	{

		// check for a pointer and bpfree
		if(message->headers[n])
			bpfree(message->headers[n]);

		// zero out pointer
		message->headers[n] = NULL;

	}

	// bpfree actual headers array
	if(message->headers)
		bpfree(message->headers);

	// bpfree list of message lines
	if(message->message_line_cnt)
	for(n=0; n < message->message_line_cnt; n++)
	{

		// check for a pointer and bpfree
		if(message->message_body[n])
			bpfree(message->message_body[n]);

		// zero out pointer
		message->message_body[n] = NULL;

	}

	// bpfree actual message body array
	if(message->message_body)
		bpfree(message->message_body);

	// finally, destroy the message structure
	bpfree(message);

	// return indicating success
	return ERR_SUCCESS;

}


// displays an email message
BP_ERROR_T BP_DisplayEmailSMTPMessage(P_BP_EMAIL_SMTP_MESSAGE message)
{

	// check to ensure we have a message
	if(!message)
		return ERR_FAILURE;

	// general array iterator
	size_t n = 0;

	// display the message banner/ptr
	printf("\n [+] Displaying Message: %p", message);

	// display the from data
	printf("\n\t [+] Mail From: %s", message->mail_from);

	// list of recipients
	if(message->recipient_cnt)
	{

		// display the recipients
		printf("\n\t [+] Recipients: %p", message->recipients);

		// display individual recipients
		for(n=0; n < message->recipient_cnt; n++)
			printf("\n\t\t - %s", message->recipients[n]);

	}

	// list of set headers
	if(message->header_cnt)
	{

		// display the recipients
		printf("\n\t [+] Headers: %p", message->headers);

		// display individual recipients
		for(n=0; n < message->header_cnt; n++)
			printf("\n\t\t - %s", message->headers[n]);

	}


	// message body
	if(message->message_line_cnt)
	{

		// display the message body
		printf("\n\t [+] Message Body: %p", message->message_body);

		// display individual recipients
		for(n=0; n < message->message_line_cnt; n++)
			printf("\n\t\t - %s", message->message_body[n]);

	}

	// print trailing newline
	printf("\n");

	// return indicating success
	return ERR_SUCCESS;

}

// sends an email over a given session
BP_ERROR_T BP_SendSMTPEmailMessage(P_BP_EMAIL_SMTP_SESSION session, P_BP_EMAIL_SMTP_MESSAGE message)
{

	// check for session and message and insure that they are set
	if(!session || !message)
		return ERR_FAILURE;

	// general purpose iterator
	size_t n = 0;
	CURLcode res;

	// upload context structure
	struct upload_status upload_ctx;

	// payload text
	upload_ctx.payload_text = (char **) message->message_body;
	upload_ctx.lines_read = 0;

	// mail from
	curl_easy_setopt(session->curl, CURLOPT_MAIL_FROM, message->mail_from);

	// set recipients
	struct curl_slist *recipients = NULL;

	// set recipients from array
	for(n=0; n < message->recipient_cnt; n++)
	{
		recipients = curl_slist_append(recipients, message->recipients[n]);
	}

	// set recipients in curl
	curl_easy_setopt(session->curl, CURLOPT_MAIL_RCPT, recipients);

	// set read data function
	curl_easy_setopt(session->curl, CURLOPT_READFUNCTION, payload_source);

	// set read data
	curl_easy_setopt(session->curl, CURLOPT_READDATA,     &upload_ctx);

	// set verbosity level (debug)
	curl_easy_setopt(session->curl, CURLOPT_VERBOSE, 1L);

	// perform curl task
	res = curl_easy_perform(session->curl);

	// destroy slist before returning
	curl_slist_free_all(recipients);

	// return indicating success
	return ERR_SUCCESS;

}

/*
 * BP-EmailSMTP.h
 * ---------------------------------------------------------
 * New CURL SMTP email functionality (function/struct declarations)
 *
 */

#ifndef __BP_EMAILSMTP_H
#define __BP_EMAILSMTP_H

// Email smtp message
typedef struct _BP_EMAIL_SMTP_MESSAGE {

	// mail from string
	char mail_from[1024];

	// list of recipients
	char **recipients;
	size_t recipient_cnt;

	// email headers ( should appear before email )
	char **headers;
	size_t header_cnt;

	// message body
	char **message_body;
	size_t message_line_cnt;

} BP_EMAIL_SMTP_MESSAGE, *P_BP_EMAIL_SMTP_MESSAGE;


// curl based email session
typedef struct _BP_EMAIL_SMTP_SESSION {

	// curl pointer for the session
	CURL * curl;

} BP_EMAIL_SMTP_SESSION, *P_BP_EMAIL_SMTP_SESSION;


// opens a new curl session for the purposes of sending email via SMTP
P_BP_EMAIL_SMTP_SESSION BP_OpenSMTPEmailSession(char *username, char *password,	char *smtp_server, size_t smtp_port);

// destroys a smtp email session ( destroy the smtp session )
BP_ERROR_T BP_CloseDestroySMTPEmailSession(P_BP_EMAIL_SMTP_SESSION session);

// creates a new message structure
P_BP_EMAIL_SMTP_MESSAGE BP_CreateSMTPMessage(char **recipients, char *mail_from, char **headers, char **message_body);

// sends an email over a given session
BP_ERROR_T BP_SendSMTPEmailMessage(P_BP_EMAIL_SMTP_SESSION session, P_BP_EMAIL_SMTP_MESSAGE message);

// destroys a message structure (null pointer after destroyed to prevent reuse)
BP_ERROR_T BP_DestroySMTPMessage(P_BP_EMAIL_SMTP_MESSAGE message);

// displays an email message
BP_ERROR_T BP_DisplayEmailSMTPMessage(P_BP_EMAIL_SMTP_MESSAGE message);

#endif // __BP_EMAILSMTP_H

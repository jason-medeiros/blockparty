/*
 * BP-Email.h
 *
 *  Created on: Aug 17, 2010
 *      Author: root
 *  NOTICE: EMAIL IS DEPENDENT ON CPP
 */

#ifndef BP_EMAIL_H_
#define BP_EMAIL_H_

#include <iostream>
#include <sstream>
#include <vector>
#include <map>

#include "vmime/vmime.hpp"
#include "vmime/platforms/posix/posixHandler.hpp"

// MessageList type definition
typedef std::map <int, vmime::ref <vmime::net::message> > MessageList;

// Email Message Structure
typedef struct _BP_EMAIL_MESSAGE {

	// message inbox number
	size_t msg_inbox_number;

	// character pointer to hold raw message
	char * message;

	// character pointer to hold subject header (if exists)
	char * subject;

	// from string
	char * from;
	char * from_email;

	// message length
	size_t message_length;

} BP_EMAIL_MESSAGE, *P_BP_EMAIL_MESSAGE;

// email sesssion (CPP REQUIRED)
typedef struct _BP_EMAIL_SESSION {

	// %%%%%%%%%%%%% Host Information %%%%%%%%%%%%%%%%%%


	// save remote host we're connected to
	vmime::string * remote_host;

	// C string representing the remote host (strdup'd from remote_host)
	char * remote_host_c_str;

	// save the remote port we're connected on
	size_t remote_port;

	// whether or not the connection is secured
	size_t connection_secure;

	// session folder
	vmime::ref <vmime::net::folder> f;

	// messages retrieved count
	MessageList * message_list;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%% Raw Emails %%%%%%%%%%%%%%%%%%%%%%

	// array of messages
	BP_EMAIL_MESSAGE * messages;

	// emails fetched count
	size_t messages_fetched;

	// message count (total count)
	size_t message_count;


	// %%%%%%%%%% Email Internals %%%%%%%%%%%%%%%%%%%%%%

	// url string
	vmime::string *urlString;

	// url object
	vmime::utility::url * url_object;

	// message store
	vmime::ref <vmime::net::store> st;

	// connection information
	vmime::ref <vmime::net::connectionInfos> ci;

	// message information
	vmime::ref <vmime::net::message> msg;

} BP_EMAIL_SESSION, *P_BP_EMAIL_SESSION;

// Create an email session POP/IMAP etc.
P_BP_EMAIL_SESSION BP_CreateNewEmailSession(char *connect_str, char *folder_name);

// Print Current Folder
BP_ERROR_T BP_PrintCurrentFolder(P_BP_EMAIL_SESSION email_session);

// Retrieve A Message At Position
BP_ERROR_T BP_RecieveMessagesFromSession(P_BP_EMAIL_SESSION email_session, size_t msg_start_number, size_t msg_retrieve_count);

// Print out a message
BP_ERROR_T BP_PrintEmailMessage(P_BP_EMAIL_MESSAGE message);

// Send message (SMTP)
BP_ERROR_T BP_SendEmailMessage(char *user, char *pass, char *connect_str, char *from_email_addr, char **send_to,  char *subject, char *body);


#endif /* BP_EMAIL_H_ */

/*
 * BP-Email.cc
 *
 *  Created on: Aug 17, 2010
 *      Author: Jaon Medeiros
 *
 */

#include "../../../include/BP-Main.h"



// Global session object
static vmime::ref <vmime::net::session> g_session	= vmime::create <vmime::net::session>();


#if VMIME_HAVE_SASL_SUPPORT

// SASL authentication handler
class interactiveAuthenticator : public vmime::security::sasl::defaultSASLAuthenticator
{
	const std::vector <vmime::ref <vmime::security::sasl::SASLMechanism> > getAcceptableMechanisms
		(const std::vector <vmime::ref <vmime::security::sasl::SASLMechanism> >& available,
		 vmime::ref <vmime::security::sasl::SASLMechanism> suggested) const
	{
		std::cout << std::endl << "Available SASL mechanisms:" << std::endl;

		for (size_t i = 0 ; i < available.size() ; ++i)
		{
			std::cout << "  " << available[i]->getName();

			if (suggested && available[i]->getName() == suggested->getName())
				std::cout << "(suggested)";
		}

		std::cout << std::endl << std::endl;

		return defaultSASLAuthenticator::getAcceptableMechanisms(available, suggested);
	}

	void setSASLMechanism(vmime::ref <vmime::security::sasl::SASLMechanism> mech)
	{
		std::cout << "Trying '" << mech->getName() << "' authentication mechanism" << std::endl;

		defaultSASLAuthenticator::setSASLMechanism(mech);
	}

	const vmime::string getUsername() const
	{
		if (m_username.empty())
			m_username = getUserInput("Username");

		return m_username;
	}

	const vmime::string getPassword() const
	{
		if (m_password.empty())
			m_password = getUserInput("Password");

		return m_password;
	}

	static const vmime::string getUserInput(const std::string& prompt)
	{
		std::cout << prompt << ": ";
		std::cout.flush();

		vmime::string res;
		std::getline(std::cin, res);

		return res;
	}

private:

	mutable vmime::string m_username;
	mutable vmime::string m_password;
};

#else // !VMIME_HAVE_SASL_SUPPORT

// Simple authentication handler
class interactiveAuthenticator : public vmime::security::defaultAuthenticator
{
	const vmime::string getUsername() const
	{
		if (m_username.empty())
			m_username = getUserInput("Username");

		return m_username;
	}

	const vmime::string getPassword() const
	{
		if (m_password.empty())
			m_password = getUserInput("Password");

		return m_password;
	}

	static const vmime::string getUserInput(const std::string& prompt)
	{
		std::cout << prompt << ": ";
		std::cout.flush();

		vmime::string res;
		std::getline(std::cin, res);

		return res;
	}

private:

	mutable vmime::string m_username;
	mutable vmime::string m_password;
};

#endif // VMIME_HAVE_SASL_SUPPORT


#if VMIME_HAVE_TLS_SUPPORT

// Certificate verifier (TLS/SSL)
class interactiveCertificateVerifier : public vmime::security::cert::defaultCertificateVerifier
{
public:

	void verify(vmime::ref <vmime::security::cert::certificateChain> chain)
	{
		try
		{
			setX509TrustedCerts(m_trustedCerts);

			defaultCertificateVerifier::verify(chain);
		}
		catch (vmime::exceptions::certificate_verification_exception&)
		{
			// Obtain subject's certificate
			vmime::ref <vmime::security::cert::certificate> cert = chain->getAt(0);

			std::cout << std::endl;
			std::cout << "Server sent a '" << cert->getType() << "'" << " certificate." << std::endl;
			std::cout << "Do you want to accept this certificate? (Y/n) ";
			std::cout.flush();

			std::string answer;
			std::getline(std::cin, answer);

			if (answer.length() != 0 &&
			    (answer[0] == 'Y' || answer[0] == 'y'))
			{
				// Accept it, and remember user's choice for later
				if (cert->getType() == "X.509")
				{
					m_trustedCerts.push_back(cert.dynamicCast
						<vmime::security::cert::X509Certificate>());
				}

				return;
			}

			throw vmime::exceptions::certificate_verification_exception
				("User did not accept the certificate.");
		}
	}

private:

	static std::vector <vmime::ref <vmime::security::cert::X509Certificate> > m_trustedCerts;
};


std::vector <vmime::ref <vmime::security::cert::X509Certificate> >
	interactiveCertificateVerifier::m_trustedCerts;

#endif // VMIME_HAVE_TLS_SUPPORT


/** Returns the messaging protocols supported by VMime.
  *
  * @param type service type (vmime::net::service::TYPE_STORE or
  * vmime::net::service::TYPE_TRANSPORT)
  */
static const std::string findAvailableProtocols(const vmime::net::service::Type type)
{
	vmime::net::serviceFactory* sf = vmime::net::serviceFactory::getInstance();

	std::ostringstream res;
	int count = 0;

	for (int i = 0 ; i < sf->getServiceCount() ; ++i)
	{
		const vmime::net::serviceFactory::registeredService& serv = *sf->getServiceAt(i);

		if (serv.getType() == type)
		{
			if (count != 0)
				res << ", ";

			res << serv.getName();
			++count;
		}
	}

	return res.str();
}


// Exception helper
static std::ostream& operator<<(std::ostream& os, const vmime::exception& e)
{
	os << "* vmime::exceptions::" << e.name() << std::endl;
	os << "    what = " << e.what() << std::endl;

	// More information for special exceptions
	if (dynamic_cast <const vmime::exceptions::command_error*>(&e))
	{
		const vmime::exceptions::command_error& cee =
			dynamic_cast <const vmime::exceptions::command_error&>(e);

		os << "    command = " << cee.command() << std::endl;
		os << "    response = " << cee.response() << std::endl;
	}

	if (dynamic_cast <const vmime::exceptions::invalid_response*>(&e))
	{
		const vmime::exceptions::invalid_response& ir =
			dynamic_cast <const vmime::exceptions::invalid_response&>(e);

		os << "    response = " << ir.response() << std::endl;
	}

	if (dynamic_cast <const vmime::exceptions::connection_greeting_error*>(&e))
	{
		const vmime::exceptions::connection_greeting_error& cgee =
			dynamic_cast <const vmime::exceptions::connection_greeting_error&>(e);

		os << "    response = " << cgee.response() << std::endl;
	}

	if (dynamic_cast <const vmime::exceptions::authentication_error*>(&e))
	{
		const vmime::exceptions::authentication_error& aee =
			dynamic_cast <const vmime::exceptions::authentication_error&>(e);

		os << "    response = " << aee.response() << std::endl;
	}

	if (dynamic_cast <const vmime::exceptions::filesystem_exception*>(&e))
	{
		const vmime::exceptions::filesystem_exception& fse =
			dynamic_cast <const vmime::exceptions::filesystem_exception&>(e);

		os << "    path = " << vmime::platform::getHandler()->
			getFileSystemFactory()->pathToString(fse.path()) << std::endl;
	}

	if (e.other() != NULL)
		os << *e.other();

	return os;
}


/** Print the MIME structure of a message on the standard output.
  *
  * @param s structure object
  * @param level current depth
  */
static void printStructure(vmime::ref <const vmime::net::structure> s, const int level = 0)
{
	for (int i = 0 ; i < s->getPartCount() ; ++i)
	{
		vmime::ref <const vmime::net::part> part = s->getPartAt(i);

		for (int j = 0 ; j < level * 2 ; ++j)
			std::cout << " ";

		std::cout << (part->getNumber() + 1) << ". "
				<< part->getType().generate()
				<< " [" << part->getSize() << " byte(s)]"
				<< std::endl;

		printStructure(part->getStructure(), level + 1);
	}
}


static const vmime::string getFolderPathString(vmime::ref <vmime::net::folder> f)
{
	const vmime::string n = f->getName().getBuffer();

	if (n.empty()) // root folder
	{
		return "/";
	}
	else
	{
		vmime::ref <vmime::net::folder> p = f->getParent();
		return getFolderPathString(p) + n + "/";
	}
}


/** Print folders and sub-folders on the standard output.
  *
  * @param folder current folder
  */
static void printFolders(vmime::ref <vmime::net::folder> folder, const int level = 0)
{
	for (int j = 0 ; j < level * 2 ; ++j)
		std::cout << " ";

	std::cout << getFolderPathString(folder) << std::endl;

	std::vector <vmime::ref <vmime::net::folder> > subFolders = folder->getFolders(false);

	for (size_t i = 0 ; i < subFolders.size() ; ++i)
		printFolders(subFolders[i], level + 1);
}


/** Print a menu on the standard output.
  *
  * @param choices menu choices
  */
static size_t printMenu(const std::vector <std::string>& choices)
{
	std::cout << std::endl;

	for (size_t i = 0 ; i < choices.size() ; ++i)
		std::cout << "   " << (i + 1) << ". " << choices[i] << std::endl;

	std::cout << std::endl;
	std::cout << "   Your choice? [1-" << choices.size() << "] ";
	std::cout.flush();

	std::string line;
	std::getline(std::cin, line);

	std::istringstream iss(line);

	size_t choice = 0;
	iss >> choice;

	std::cout << std::endl;

	if (choice < 1 || choice > choices.size())
		return 0;
	else
		return choice;
}


/** Send a message interactively.
  */
static void sendMessage(char *user, char *pass, char *connect_str, char *from_email_addr, char **send_to, char *body)
{
	try
	{

		// set url string as connect_str (sendmail:://localhost etc)
		vmime::string urlString = connect_str;

		// url utility
		vmime::utility::url url(urlString);

		// get a transport
		vmime::ref <vmime::net::transport> tr =	g_session->getTransport(url, vmime::create <interactiveAuthenticator>());

#if VMIME_HAVE_TLS_SUPPORT

		// Enable TLS support if available
		tr->setProperty("connection.tls", true);

		// Set the object responsible for verifying certificates, in the
		// case a secured connection is used (TLS/SSL)
		tr->setCertificateVerifier	(vmime::create <interactiveCertificateVerifier>());

#endif // VMIME_HAVE_TLS_SUPPORT

		// You can also set some properties (see example7 to know the properties
		// available for each service). For example, for SMTP:
//		tr->setProperty("options.need-authentication", true);

		// set from email address
		vmime::string fromString = from_email_addr;

		// create mailbox from the above string
		vmime::mailbox from(fromString);

		vmime::mailboxList to;
		size_t i = 0;
		for (i = 0 ; send_to[i] ; i++ )
		{

			// set the send to from our parameter array
			vmime::string toString = send_to[i];

			// append mailbox
			to.appendMailbox(vmime::create <vmime::mailbox>(toString));

		}



		// put body content in a vmime structure
		vmime::string bodyContent = body;


		// Connect to server
		tr->connect();

		// Send the message
		vmime::string msgData = bodyContent;
		vmime::utility::inputStreamStringAdapter vis(msgData);

		// send the actual message
		tr->send(from, to, vis, msgData.length());

		// disconnect after you send
		tr->disconnect();

	}
	catch (vmime::exception& e)
	{
		std::cerr << std::endl;
		std::cerr << e << std::endl;
		throw;
	}
	catch (std::exception& e)
	{
		std::cerr << std::endl;
		std::cerr << "std::exception: " << e.what() << std::endl;
		throw;
	}
}


/** Connect to a message store interactively.
  */
P_BP_EMAIL_SESSION BP_CreateNewEmailSession(char *connect_str, char *folder_name)
{

	// VMime initialization
	vmime::platform::setHandler<vmime::platforms::posix::posixHandler>();

	// check connect string (cannot be null)
	if(!connect_str)
		return NULL;

	// create new email session structure
	P_BP_EMAIL_SESSION email_session = (P_BP_EMAIL_SESSION) bpcalloc(sizeof(BP_EMAIL_SESSION), 1);
	if(!email_session)
		return NULL;


		// create new string
		vmime::string * url_str = new vmime::string(connect_str);

		// set class pointer
		email_session->urlString = url_str; // new std::string(connect_str, bpstrlen(connect_str));

		// create new url object
		email_session->url_object = new vmime::utility::url(*email_session->urlString);


		// If the user/pass wasn't set in the url string, attempt to get them here.
		if (
				email_session->url_object->getUsername().empty() ||
				email_session->url_object->getPassword().empty()
		)
		{
			email_session->st = g_session->getStore(*email_session->url_object, vmime::create <interactiveAuthenticator>());
		}
		else
		{
			email_session->st = g_session->getStore(*email_session->url_object);
		}

		// Additional TLS checks below.
#if VMIME_HAVE_TLS_SUPPORT

		// Enable TLS support if available
		email_session->st->setProperty("connection.tls", true);

		// Set the object responsible for verifying certificates, in the
		// case a secured connection is used (TLS/SSL)
		email_session->st->setCertificateVerifier(vmime::create <interactiveCertificateVerifier>());


#endif // VMIME_HAVE_TLS_SUPPORT


		// Connect to the mail store
		email_session->st->connect();

		// Display some information about the connection
		email_session->ci = email_session->st->getConnectionInfos();


		// save remote host we're connected to
		email_session->remote_host = new vmime::string(email_session->ci->getHost());

		// save the remote port we're connected on
		email_session->remote_port = email_session->ci->getPort();

		// save whether or not the connection is secured
		email_session->connection_secure = email_session->st->isSecuredConnection();

		// set the C styled string in the struct for ease of use
		email_session->remote_host_c_str = bpstrdup((char *) email_session->remote_host->c_str());

		if(!folder_name)
			email_session->f = email_session->st->getDefaultFolder();
		else
			email_session->f = email_session->st->getFolder(vmime::utility::path(folder_name));


		// open default folder
		email_session->f->open(vmime::net::folder::MODE_READ_WRITE);

		// retrieve the number of messages that are in the inbox
		email_session->message_count = email_session->f->getMessageCount();

		// message info
		printf("\n Remote Connection: %s", email_session->remote_host_c_str);
		printf("\n Remote Port:       %d", email_session->remote_port);
		printf("\n Message Count:     %d", email_session->message_count);

		printf("\n");

		// create new messagelist for later message retrieval
		email_session->message_list = new MessageList();

		// return the new session
		return email_session;


		/*

		// retrieve the foldern_name or default for opening




*/




/*
		for (bool cont = true ; cont ; )
		{
			typedef std::map <int, vmime::ref <vmime::net::message> > MessageList;
			MessageList msgList;

			try
			{


				// Show message flags
//				case 1:

					f->fetchMessage(msg, vmime::net::folder::FETCH_FLAGS);

					if (msg->getFlags() & vmime::net::message::FLAG_SEEN)
						std::cout << "FLAG_SEEN" << std::endl;
					if (msg->getFlags() & vmime::net::message::FLAG_RECENT)
						std::cout << "FLAG_RECENT" << std::endl;
					if (msg->getFlags() & vmime::net::message::FLAG_REPLIED)
						std::cout << "FLAG_REPLIED" << std::endl;
					if (msg->getFlags() & vmime::net::message::FLAG_DELETED)
						std::cout << "FLAG_DELETED" << std::endl;
					if (msg->getFlags() & vmime::net::message::FLAG_MARKED)
						std::cout << "FLAG_MARKED" << std::endl;
					if (msg->getFlags() & vmime::net::message::FLAG_PASSED)
						std::cout << "FLAG_PASSED" << std::endl;

					break;

				// Show message structure
	//			case 2:

					f->fetchMessage(msg, vmime::net::folder::FETCH_STRUCTURE);
					printStructure(msg->getStructure());
					break;

				// Show message header
		//		case 3:

					f->fetchMessage(msg, vmime::net::folder::FETCH_FULL_HEADER);
					std::cout << msg->getHeader()->generate() << std::endl;
					break;

				// Show message envelope
			//	case 4:

					f->fetchMessage(msg, vmime::net::folder::FETCH_ENVELOPE);

#define ENV_HELPER(x) \
	try { std::cout << msg->getHeader()->x()->generate() << std::endl; } \
	catch (vmime::exception) {  }

					ENV_HELPER(From)
					ENV_HELPER(To)
					ENV_HELPER(Date)
					ENV_HELPER(Subject)

#undef ENV_HELPER

				//	break;

				// Extract whole message
				// case 5:
				{
					vmime::utility::outputStreamAdapter out(std::cout);
					msg->extract(out);

				//	break;
				}




				// Change folder
//				case 7:

					std::cout << "Enter folder path (eg. /root/subfolder):" << std::endl;
					std::cout.flush();

					std::string path;
					std::getline(std::cin, path);

					vmime::ref<vmime::net::folder> newFolder = st->getRootFolder();

					for (std::string::size_type s = 0, p = 0;; s = p + 1) {
						p = path.find_first_of('/', s);

						const std::string x = (p == std::string::npos) ? std::string(path.begin() + s, path.end())
								: std::string(path.begin() + s, path.begin() + p);

						if (!x.empty())
							newFolder = newFolder->getFolder(x);

						if (p == std::string::npos)
							break;
					}

					newFolder->open(vmime::net::folder::MODE_READ_WRITE);

					count = newFolder->getMessageCount();

					std::cout << std::endl;
					std::cout << count << " message(s) in this folder" << std::endl;

					f->close(true); // 'true' to expunge deleted messages
					f = newFolder;


				// Main menu
				// case 8:

					f->close(true);  // 'true' to expunge deleted messages
					cont = false;
					break;




		// Append message
		std::istringstream iss(
			"From: me@localhost\r\n"
			"To: you@localhost\r\n"
			"Subject: Message Text\r\n"
			"\r\n"
			"This is a test message...\r\n"
			"Bye bye!\r\n"
		);

		f->addMessage(iss, iss.str().size());

		// Folder renaming
		{
			vmime::ref <vmime::net::folder> f = st->getFolder(vmime::net::folder::path("c"));
			f->rename(vmime::net::folder::path("c2"));

			vmime::ref <vmime::net::folder> g = st->getFolder(vmime::net::folder::path("c2"));
			g->rename(vmime::net::folder::path("c"));
		}

		// Message copy: copy all messages from 'f' to 'g'
		{
			vmime::ref <vmime::net::folder> g = st->getFolder(vmime::net::folder::path("TEMP"));

			if (!g->exists())
				g->create(vmime::net::folder::TYPE_CONTAINS_MESSAGES);

			f->copyMessages(g->getFullPath());
		}

			}


			catch (vmime::exception& e)
			{
				std::cerr << std::endl;
				std::cerr << e << std::endl;
			}
			catch (std::exception& e)
			{
				std::cerr << std::endl;
				std::cerr << "std::exception: " << e.what() << std::endl;
			}

		// }
			*/


}

// Retrieve A Messages At The Specified Start Position, and Retrieve X Records
BP_ERROR_T BP_RecieveMessagesFromSession(P_BP_EMAIL_SESSION email_session, size_t msg_start_number, size_t msg_retrieve_count)
{

	// return indicating failure
	if(!email_session || !msg_retrieve_count)
		return ERR_FAILURE;


	// local message structure
    vmime::ref <vmime::net::message> msg;

    // message list
    MessageList * msgList = email_session->message_list;

    // set message retrieval loop index base at start index
    size_t i = msg_start_number;
    for(; i < msg_start_number + msg_retrieve_count; i++)
    {


    	// get message
    	msg = email_session->f->getMessage(i);

    	// insert message into list
    	msgList->insert(MessageList::value_type(i, msg));

    	// message output stream
    	vmime::utility::outputStreamAdapter out(std::cout);

    	// extract email
    	vmime::string email_content;
    	vmime::utility::outputStreamStringAdapter bufferStream(email_content);
    	msg->extract(bufferStream);

    	// create stored message
    	vmime::message * stored_parsed_message = new vmime::message;

    	// parse out email content
    	stored_parsed_message->parse(email_content);


    	// -------------------------------------------
    	// store message in session

   		// increase the message count and allocate space for message
   		email_session->messages_fetched++;

   		// bpreallocate message structure
   		email_session->messages = (P_BP_EMAIL_MESSAGE) bprealloc(email_session->messages, sizeof(BP_EMAIL_MESSAGE) * email_session->message_count);

   		// zero out the new last element
   		memset(&email_session->messages[email_session->messages_fetched - 1], 0x00, sizeof(BP_EMAIL_MESSAGE));

   		// set current message for ease of use
   		BP_EMAIL_MESSAGE * current_message = &email_session->messages[email_session->messages_fetched - 1];

   		// set current message content, length, and inbox number in the message structure (ease of use)
   		current_message->msg_inbox_number = i;
   		current_message->message          = bpstrdup((char *) email_content.c_str());
   		current_message->message_length   = email_content.length();

   		// set subject
   		// current_message->subject
   		vmime::text outText;
   		vmime:: charset ch(vmime::charsets::UTF_8);
   		vmime:: charset ch2(vmime::charsets::UTF_8);

   		// extract header
   		vmime::ref <vmime::header> hdr = stored_parsed_message->getHeader();

   		// get subject
   		vmime::string subject = hdr->Subject()->getValue().dynamicCast <vmime::text>()->getConvertedText(ch);

   		// save the subject
   		current_message->subject = bpstrdup((char *) subject.c_str());

   		// get subject
   		vmime::string from = hdr->From()->getValue().dynamicCast<vmime::mailbox>()->getName().getConvertedText(ch);

   		// get email
   		vmime::string from_email = hdr->From()->getValue().dynamicCast<vmime::mailbox>()->getEmail();

   		// save the from name and email address
   		current_message->from       = bpstrdup((char *) from.c_str());
   		current_message->from_email = bpstrdup((char *) from_email.c_str());

    }

	return ERR_FAILURE;
}

// Print out a message
BP_ERROR_T BP_PrintEmailMessage(P_BP_EMAIL_MESSAGE message)
{

	if(!message)
		return ERR_FAILURE;

	// message inbox number
	printf("\n\n [+] Message Number %i", message->msg_inbox_number);

	printf("\n\t From: %s (%s)", message->from, message->from_email);
	printf("\n\t Subject: %s", message->subject);
	printf("\n\n %%%%%%%%%%%%%%%%%%%%%% Message Below (%d bytes) %%%%%%%%%%%%%%%%%%%%\n", message->message_length);

	printf("\n %s", message->message);
	printf("\n");

	// return indicating success
	return ERR_SUCCESS;

}


// Print Current Folder
BP_ERROR_T BP_PrintCurrentFolder(P_BP_EMAIL_SESSION email_session)
{

	// ensure we have an email session
	if(!email_session)
		return ERR_FAILURE;

	// print folders
	printFolders(email_session->f);

}


// Displays downloaded messages


// Send message (SMTP)
BP_ERROR_T BP_SendEmailMessage(char *user, char *pass, char *connect_str, char *from_email_addr, char **send_to, char *subject, char *body)
{

	if(!user)
		user = "";
	if(!pass)
		pass = "";
	if(!connect_str)
		connect_str = "sendmail://localhost";
	if(!from_email_addr)
		from_email_addr = "blockparty@localhost";
	// VMime initialization
	vmime::platform::setHandler<vmime::platforms::posix::posixHandler>();


	char *final_body = (char *) bpcalloc(bpstrlen(body)+bpstrlen(subject)+1024, 1);
	sprintf(final_body, "%s\n\r%s\n\r", subject,  body);

	// send message
	sendMessage(user, pass, connect_str, from_email_addr, send_to,  final_body);

	// bpfree the final body
	bpfree(final_body);

	// return indicating success
	return ERR_SUCCESS;

}


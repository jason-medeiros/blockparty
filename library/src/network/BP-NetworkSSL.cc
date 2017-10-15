// blockparty main include file
#include "../../include/BP-Main.h"


// Initializes all the components required to reliably and transparently
// utilize the openssl library.
BP_ERROR_T BP_SSLLibInit()
{

	// initialize the library
	SSL_library_init();

	// load all ssl error strings
	SSL_load_error_strings();

	// add all algorithms for use
	OpenSSL_add_all_algorithms();

	// return indicating success
	return ERR_SUCCESS;

}


// --- Callbacks ---

// callback to get password
int BP_SSLPasswordCallback(char * buff, int size, int rwflag, void *password)
{

	if(!password)
		return 0;
	if(!BP_StringIsReasonablePrintableString((char *) password, bpstrlen((char *) password), BP_FALSE, BP_FALSE))
		return 0;

	// copy in password
	strncpy(buff, (char*) password, size);

	// ensure that the item is null terminated
	if(size > 0)
		buff[size - 1] = '\0';
	else
		buff[0] = '\0';
	
	// return the size of the string + null terminator
	return(bpstrlen(buff)+1);

}


// --- Setup and Action Contexts ---------

// only uses RSA keys currently
BP_ERROR_T BP_SSLSetup(P_BP_NETWORK_HANDLE netHandle, char * serverCert, char * serverKey, char * password, SSL_METHOD * sslMethod)
{

	// ensure we have a network handle
	if(!netHandle)
		return ERR_FAILURE;

	// ensure we have a ssl opts structure
	if(!netHandle->sslOpts)
		return ERR_FAILURE;

	
	// Set method with user parameters.
	/* can be either
	 * 	SSLv3_method();
	 * 	SSLv2_method();
	 *  TLSv1_method();
	 */
	if(sslMethod != NULL)
	{
		netHandle->sslOpts->context = SSL_CTX_new(sslMethod);
	}
	
	// gather password if set
	if(password)
	{

		// set default password callback
		SSL_CTX_set_default_passwd_cb(netHandle->sslOpts->context, BP_SSLPasswordCallback);

		// set default password callback
		SSL_CTX_set_default_passwd_cb_userdata(netHandle->sslOpts->context, (void *) password);

	}
	else
	{

		return ERR_FAILURE;
	}
		
	// Set Certificate
	if(SSL_CTX_use_certificate_file(netHandle->sslOpts->context, serverCert, SSL_FILETYPE_PEM) != 1)
	{
		printf("\n Failed in SSL_CTX_use_certificate_file");
		return ERR_FAILURE;
	}
		
	// Set Key
	if(SSL_CTX_use_PrivateKey_file(netHandle->sslOpts->context, serverKey, SSL_FILETYPE_PEM) != 1)
	{
		printf("\n Failed in SSL_CTX_use_PrivateKey_file");
		return ERR_FAILURE;
	}
	
	// Checks private key
	if(SSL_CTX_check_private_key(netHandle->sslOpts->context) != 1)
	{
		printf("\n Failed in SSL_CTX_check_private_key");
		return ERR_FAILURE;
	}

	// return indicating success
	return ERR_SUCCESS;

}

// Connects on an Socket (ssl)
BP_ERROR_T BP_SSLConnect(P_BP_NETWORK_HANDLE netHandle)
{
	
	if(netHandle == NULL)
		return ERR_FAILURE;
	
	// create new ssl object if one isn't there already
	if(netHandle->sslOpts->ssl == NULL)
	{

		if(netHandle->sslOpts->context != NULL)
		{
			netHandle->sslOpts->ssl = SSL_new(netHandle->sslOpts->context);

		}
		else
		{
			return ERR_FAILURE;
		}

	}
	
	// set fd
	if(SSL_set_fd(netHandle->sslOpts->ssl, netHandle->sock) != 1)
		return ERR_FAILURE;
		
	// connect
	if(SSL_connect(netHandle->sslOpts->ssl) != 1)
		return ERR_FAILURE;
	
	// return indicating success
	return ERR_SUCCESS;
}

// Accepts on a XLT Socket
BP_ERROR_T BP_SSLAccept(P_BP_NETWORK_HANDLE netHandle)
{
	
	if(!netHandle)
		return ERR_FAILURE;
	
	if(netHandle->sslOpts->ssl == NULL)
	{

		if(netHandle->sslOpts->context != NULL)
		{

			// attempt to allocate new ssl structure
			netHandle->sslOpts->ssl = SSL_new(netHandle->sslOpts->context);

			// ensure we created new ssl structure
			if(!netHandle->sslOpts->ssl)
				return ERR_FAILURE;

		}
		else
		{

			// exit indicating failure if we already have a context set for some reason
			return ERR_FAILURE;

		}

	}
	
	// set fd
	if(SSL_set_fd(netHandle->sslOpts->ssl, netHandle->sock) != 1)
		return ERR_FAILURE;
		
	// connect
	if(SSL_accept(netHandle->sslOpts->ssl) != 1)
		return ERR_FAILURE;
		
	// return indicating success
	return ERR_SUCCESS;
}

// writes size bytes of buff to sock's ssl connection
int BP_SSLWrite(P_BP_NETWORK_HANDLE netHandle, BYTE * buff, int size)
{

	int length = 0;
	int totalWrite = 0;
	
	if(netHandle == 0)
		return 0;
	
	if(buff == 0)
		return 0;
	
	// this loop isn't really necessary, but in case the openssl
	// crazy people decide to make write act like read, then this'll
	// automagically fix the "problem"
	do
	{

		length = SSL_write(netHandle->sslOpts->ssl, buff, size);

		buff = &buff[length];

		size -= length;

		totalWrite += length;

	} while(length > 0 && size > 0);
	
	if(length <= 0)
		return totalWrite;
	
	return totalWrite;

}
	
// reads size bytes from socks ssl connection into buff
// btw, ssl is lame in the sense that you cannot read more then
// 4096 bytes at a time for sslv2 and 16kb for v3/tlsv1.
int BP_SSLRead(P_BP_NETWORK_HANDLE netHandle, BYTE *buff, int size)
{

	int length = 0;
	int totalRead = 0;
	
	if(netHandle == NULL)
		return 0;
	
	if(buff == NULL)
		return 0;
	
	// read in all bytes from a SSL object, need to loop due to short
	// comings of openssl library :(
	do
	{

		// attempt to read from network handle
		length = SSL_read(netHandle->sslOpts->ssl, buff, size);

		// set buffer position
		buff = &buff[length];

		// set length
		size -= length;

		// set total read
		totalRead += length;

	} while(length > 0 && size > 0);
	
	// return the total amount read
	return totalRead;

}

//! Shuts down a ssl connection
int BP_SSLShutdown(P_BP_NETWORK_HANDLE netHandle)
{

	// run basic null checks
	if(!netHandle)
		return ERR_FAILURE;
	if(!netHandle->sslOpts)
		return ERR_FAILURE;
	if(!netHandle->sslOpts->ssl)
		return ERR_FAILURE;

	if(SSL_shutdown(netHandle->sslOpts->ssl) == 1)
		return ERR_SUCCESS;
	
	// return indicating failure
	return ERR_FAILURE;

}


//! Sets a ssl context
BP_ERROR_T BP_SSLSetContext(P_BP_NETWORK_HANDLE netHandle, SSL_METHOD *sslMethod)
{
	
	
	// ensure we have a network handle
	if(!netHandle)
		return ERR_FAILURE;
	if(!sslMethod)
		return ERR_FAILURE;

	netHandle->sslOpts->context = SSL_CTX_new(sslMethod);

	return ERR_SUCCESS;
}

//! Generates a ssl server key
// TODO: This code is some of the oldest in blockparty and must be updated with value checks and filtering
//       to prevent code injection scenarios.
BP_ERROR_T BP_SSLGenrsaServerKey(char *password, char * randomSeedFile, char * outKeyFile)
{




	// run basic string hecks
	if(!BP_StringIsReasonablePrintableString(password, bpstrlen(password), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(randomSeedFile, bpstrlen(randomSeedFile), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(outKeyFile, bpstrlen(outKeyFile), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// ensure the inCsr exists
	if(!BP_StringIsReasonableForUnixEXT4Filename(randomSeedFile,     bpstrlen(randomSeedFile),     BP_TRUE))
		return ERR_FAILURE;
	if(!BP_StringIsReasonableForUnixEXT4Filename(outKeyFile,   bpstrlen(outKeyFile),   BP_FALSE))
		return ERR_FAILURE;


	char keyArgs[MAX_CMD_LENGTH+1];
	int charCount = 0;
	//struct stat fileStats;
	
	// check parameters for escapes
	if(strstr(password, "`") || strstr(password, ";") || strstr(password, "|") || strstr(password, ".."))
		return ERR_FAILURE;
	
	if(strstr(randomSeedFile, "`") || strstr(randomSeedFile, ";") || strstr(randomSeedFile, "|") || strstr(randomSeedFile, ".."))
		return ERR_FAILURE;
		
	if(strstr(outKeyFile, "`") || strstr(outKeyFile, ";") || strstr(outKeyFile, "|") || strstr(outKeyFile, ".."))
		return ERR_FAILURE;
	
	memset(keyArgs, 0x00, MAX_CMD_LENGTH);
	
	strcat(keyArgs, "openssl genrsa -passout pass:");
	charCount += bpstrlen("openssl genrsa -passout pass:");
	
	if((bpstrlen(password) + charCount) > MAX_CMD_LENGTH)
		return ERR_FAILURE;

	strncat(keyArgs, password, MAX_CMD_LENGTH-charCount);
	charCount += bpstrlen(password);
	
	strcat(keyArgs, " -des3");
	charCount += bpstrlen(" -des3");
	
	strcat(keyArgs, " -rand ");
	charCount += bpstrlen(" -rand ");
	
	if( (bpstrlen(randomSeedFile) + charCount) > MAX_CMD_LENGTH)
		return ERR_FAILURE;
	
	strncat(keyArgs, randomSeedFile, MAX_CMD_LENGTH-charCount);
	charCount += bpstrlen(randomSeedFile);
	
	strcat(keyArgs, " -out ");
	charCount += bpstrlen(" -out ");
	
	if((bpstrlen(outKeyFile) + charCount) > MAX_CMD_LENGTH)
		return ERR_FAILURE;
	
	strncat(keyArgs, outKeyFile, MAX_CMD_LENGTH-charCount);
	charCount += bpstrlen(outKeyFile);
	
	system(keyArgs);
	
	return ERR_SUCCESS;
}

//! generates a ssl CSR
// TODO: This code is some of the oldest in blockparty and must be updated with value checks and filtering
//       to prevent code injection scenarios.
BP_ERROR_T BP_SSLGenCertificateSignRequest(char * password, char * inKey, char * outCsr)
{

	// run basic string hecks
	if(!BP_StringIsReasonablePrintableString(password, bpstrlen(password), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(inKey, bpstrlen(inKey), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(outCsr, bpstrlen(outCsr), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// ensure the inCsr exists
	if(!BP_StringIsReasonableForUnixEXT4Filename(inKey,     bpstrlen(inKey),     BP_TRUE))
		return ERR_FAILURE;
	if(!BP_StringIsReasonableForUnixEXT4Filename(outCsr,   bpstrlen(outCsr),   BP_FALSE))
		return ERR_FAILURE;


	char keyArgs[MAX_CMD_LENGTH+1];
	int charCount = 0;
	//struct stat fileStats;
	
	// actual request building strings
	char * openSslReqPassin = "openssl req -passin pass:";
	// pass var here
	char * newKey = " -new -key ";
	// key var here
	char * out = " -out ";
	// cert var here
	char * batch = " -batch";
	
	
	if(strstr(password, "`") || strstr(password, ";") || strstr(password, "|") || strstr(password, ".."))
		return ERR_FAILURE;
	if(strstr(inKey, "`") || strstr(inKey, ";") || strstr(inKey, "|") || strstr(inKey, ".."))
		return ERR_FAILURE;
	if(strstr(outCsr, "`") || strstr(outCsr, ";") || strstr(outCsr, "|") || strstr(outCsr, ".."))
		return ERR_FAILURE;
	
	memset(keyArgs, 0x00, MAX_CMD_LENGTH+1);
	
	// set first element
	strcat(keyArgs, openSslReqPassin);
	charCount = bpstrlen(keyArgs);
	
	// set password
	if( (bpstrlen(password) + charCount) > MAX_CMD_LENGTH)
		return ERR_FAILURE;
	
	strncat(keyArgs, password, MAX_CMD_LENGTH - charCount);
	charCount += bpstrlen(password);
	
	
	// set second element
	if( (bpstrlen(newKey) + charCount) > MAX_CMD_LENGTH)
		return ERR_FAILURE;
	
	strcat(keyArgs, newKey);
	charCount += bpstrlen(newKey);
	
	// set key
	if( (bpstrlen(inKey) + charCount) > MAX_CMD_LENGTH)
		return ERR_FAILURE;
	
	strncat(keyArgs, inKey, MAX_CMD_LENGTH - charCount);
	charCount += bpstrlen(inKey);
	
	
	// set third element
	if( (bpstrlen(out) + charCount) > MAX_CMD_LENGTH)
		return ERR_FAILURE;
	
	strcat(keyArgs, out);
	charCount += bpstrlen(out);
	
	// set out cert here
	if( (bpstrlen(outCsr) + charCount) > MAX_CMD_LENGTH)
		return ERR_FAILURE;
	
	strcat(keyArgs, outCsr);
	charCount += bpstrlen(outCsr);
	
	// set batch
	if( (bpstrlen(batch) + charCount) > MAX_CMD_LENGTH)
		return ERR_FAILURE;
	
	strcat(keyArgs, batch);
	charCount += bpstrlen(outCsr);
	
	system(keyArgs);
	return ERR_SUCCESS;
}

//! Generates a certificate
// TODO: This code is some of the oldest in blockparty and must be updated with value checks and filtering
//       to prevent code injection scenarios.
BP_ERROR_T BP_SSLGenCertificate
(
		char *password,
		char *daysValid,
		char *inCsr,
		char *signKey,
		char *serverCrt
)
{


	// run basic string hecks
	if(!BP_StringIsReasonablePrintableString(password, bpstrlen(password), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(daysValid, bpstrlen(daysValid), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(signKey, bpstrlen(signKey), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;
	if(!BP_StringIsReasonablePrintableString(serverCrt, bpstrlen(serverCrt), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// ensure the inCsr exists
	if(!BP_StringIsReasonableForUnixEXT4Filename(inCsr,     bpstrlen(inCsr),     BP_TRUE))
		return ERR_FAILURE;
	if(!BP_StringIsReasonableForUnixEXT4Filename(signKey,   bpstrlen(signKey),   BP_FALSE))
		return ERR_FAILURE;
	if(!BP_StringIsReasonableForUnixEXT4Filename(serverCrt, bpstrlen(serverCrt), BP_FALSE))
		return ERR_FAILURE;
	


	char keyArgs[MAX_CMD_LENGTH+1];
	int charCount = 0;
	//struct stat fileStats;
	
	char * opensslx509Passin = "openssl x509 -passin pass:";
	// set password
	char * reqDays = " -req -days ";
	// set days
	char * in = " -in ";
	// set inCsr
	char * signkey = " -signkey ";
	// set signkey
	char * out = " -out ";
	// set cert
	
	
	// Check for invalids
	if(strstr(password, "`") || strstr(password, ";") || strstr(password, "|") || strstr(password, ".."))
		return ERR_FAILURE;
		
	if(strstr(daysValid, "`") || strstr(daysValid, ";") || strstr(daysValid, "|") || strstr(daysValid, ".."))
		return ERR_FAILURE;
		
	if(strstr(inCsr, "`") || strstr(inCsr, ";") || strstr(inCsr, "|") || strstr(inCsr, ".."))
		return ERR_FAILURE;
		
	if(strstr(signKey, "`") || strstr(signKey, ";") || strstr(signKey, "|") || strstr(signKey, ".."))
		return ERR_FAILURE;
		
	if(strstr(serverCrt, "`") || strstr(serverCrt, ";") || strstr(serverCrt, "|") || strstr(serverCrt, ".."))
		return ERR_FAILURE;
	
	// zero out buff
	memset(keyArgs, 0x00, MAX_CMD_LENGTH+1);
	
	// set first element
	strncat(keyArgs, opensslx509Passin, MAX_CMD_LENGTH);
	charCount = bpstrlen(keyArgs);
	
	// set password
	if( (bpstrlen(password) + charCount) > MAX_CMD_LENGTH)
		return ERR_FAILURE;
	
	strncat(keyArgs, password, MAX_CMD_LENGTH - charCount);
	charCount += bpstrlen(password);

	// set reqDays
	if( (bpstrlen(reqDays) + charCount) > MAX_CMD_LENGTH)
		return ERR_FAILURE;
	
	strncat(keyArgs, reqDays, MAX_CMD_LENGTH - charCount);
	charCount += bpstrlen(reqDays);

	// set daysValid
	if( (bpstrlen(daysValid) + charCount) > MAX_CMD_LENGTH)
		return ERR_FAILURE;
	
	strncat(keyArgs, daysValid, MAX_CMD_LENGTH - charCount);
	charCount += bpstrlen(daysValid);
	
	// set in
	if( (bpstrlen(in) + charCount) > MAX_CMD_LENGTH)
		return ERR_FAILURE;
	
	strncat(keyArgs, in, MAX_CMD_LENGTH - charCount);
	charCount += bpstrlen(in);
	
	// set inCsr
	if( (bpstrlen(inCsr) + charCount) > MAX_CMD_LENGTH)
		return ERR_FAILURE;
	
	strncat(keyArgs, inCsr, MAX_CMD_LENGTH - charCount);
	charCount += bpstrlen(inCsr);
	
	// set signkey (lowercase)
	if( (bpstrlen(signkey) + charCount) > MAX_CMD_LENGTH)
		return ERR_FAILURE;
	
	strncat(keyArgs, signkey, MAX_CMD_LENGTH - charCount);
	charCount += bpstrlen(signkey);

	// set signKey (user value)
	if( (bpstrlen(signKey) + charCount) > MAX_CMD_LENGTH)
		return ERR_FAILURE;
	
	strncat(keyArgs, signKey, MAX_CMD_LENGTH - charCount);
	charCount += bpstrlen(signKey);
	
	// set out
	if( (bpstrlen(out) + charCount) > MAX_CMD_LENGTH)
		return ERR_FAILURE;
	
	strncat(keyArgs, out, MAX_CMD_LENGTH - charCount);
	charCount += bpstrlen(out);

	// set serverCrt
	if( (bpstrlen(serverCrt) + charCount) > MAX_CMD_LENGTH)
		return ERR_FAILURE;
	
	strncat(keyArgs, serverCrt, MAX_CMD_LENGTH - charCount);
	charCount += bpstrlen(serverCrt);

	// generate CSR
	system(keyArgs);

	// return indicating success
	return ERR_SUCCESS;

}


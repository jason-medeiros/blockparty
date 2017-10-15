/***************************************************************************
 *            BP-NetworkSSL.h
 *
 *  Mon Jan 27 15:51:02 2003
 *  Copyright  2003  User
 *  Email
 ****************************************************************************/

// SSL cert path
#define SSL_CERT_PATH "/usr/lib/ssl/certs"

// Max Command Length: Interactive with openssl tool
#define MAX_CMD_LENGTH 500


// --- Global Library Initializer --------

// initialize ssl library
BP_ERROR_T BP_SSLLibInit();



// --- Callbacks ---

// callback defined for providing SSL password data
int BP_SSLPasswordCallback(char * buff, int size, int rwflag, void *password);



// --- Setup and Action Contexts ---------

// Setup a ssl connection
BP_ERROR_T BP_SSLSetup(P_BP_NETWORK_HANDLE netHandle, char * server_cert, char * server_key, char * password, SSL_METHOD *sslMethod);

// set ssl context (need to be defined before connect attempt)
BP_ERROR_T BP_SSLSetContext(P_BP_NETWORK_HANDLE netHandle, SSL_METHOD *sslMethod);

// attempt to establish ssl connection over the current network handle
BP_ERROR_T BP_SSLConnect(P_BP_NETWORK_HANDLE netHandle);

// attempt to accept on a standard network handle
BP_ERROR_T BP_SSLAccept(P_BP_NETWORK_HANDLE netHandle);


int BP_SSLWrite(P_BP_NETWORK_HANDLE netHandle, BYTE *buff, int size);
int BP_SSLRead(P_BP_NETWORK_HANDLE netHandle, BYTE *buff, int size);
int BP_SSLShutdown(P_BP_NETWORK_HANDLE netHandle);

// key and certificate generation
BP_ERROR_T BP_SSLGenrsaServerKey(char *password, char * randomSeedFile, char * outKeyFile);
BP_ERROR_T BP_SSLGenCertificateSignRequest(char * password, char * inKey, char * outCsr);
BP_ERROR_T BP_SSLGenCertificate(char *password, char *daysValid, char *inCsr, char *signKey, char *serverCrt);


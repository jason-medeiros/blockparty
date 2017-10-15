#include "../../../include/BP-Main.h"


// Static global array of common send headers.
BP_HTTP_REQUEST_HEADER BP_HTTP_COMMON_REQUEST_HEADERS[] =
{
		{BP_FALSE, "Accept",              "Content-Types that are acceptable for the response.", "text/plain" },
		{BP_FALSE, "Accept-Charset",      "Character sets that are acceptable.", "utf-8" },
		{BP_FALSE, "Accept-Encoding",     "List of acceptable encodings.", "gzip, deflate" },
		{BP_FALSE, "Accept-Language",     "List of acceptable human languages for response.", "en-US" },
		{BP_FALSE, "Accept-Datetime",     "Acceptable version in time.", "Thu, 31 May 2013 20:35:00 GMT" },
		{BP_FALSE, "Authorization",       "Authentication credentials for HTTP authentication.", "Basic YmV5b25k" },
		{BP_FALSE, "Cache-Control",       "Used to specify directives that must be obeyed by all caching mechanisms along the request-response chain.", "no-cache" },
		{BP_FALSE, "Connection",          "What type of connection the user-agent would prefer.", "keep-alive" },
		{BP_FALSE, "Cookie",              "A HTTP cookie previously sent by the server with Set-Cookie.", "$Version=1; Skin=new;" },
		{BP_FALSE, "Content-Length",      "The length of the request body in octets (8-bit bytes).", "348" },
		{BP_FALSE, "Content-MD5",         "A Base64-encoded binary MD5 sum of the content in the request body.", "Q2hlY2sgSW50zWdyaXR5IQ==" },
		{BP_FALSE, "Content-Type",        "The MIME type of the body of the request (used with POST and PUT requests).", "application/x-www-form-urlencoded" },
		{BP_FALSE, "Date",                "The date and time that the message was sent.", "Tue, 15 Nov 1994 08:12:31 GMT" },
		{BP_FALSE, "Expect",              "Indicates that particular server behaviors are required by the client.", "100-continue" },
		{BP_FALSE, "From",                "The email address of the user making the request.", "user@example.com" },
		{BP_FALSE, "Host",                "The domain name of the server (for virtual hosting), and the TCP port number on which the server is listening.  The port number may be omitted if the port is the standard port for the service requested.", "en.wikipedia.org:80" },
		{BP_FALSE, "If-Match",            "Only perform the action if the client supplied entity matches the same entity on the server.  This is mainly for methods like PUT to only update a resource if it has not been modified since the user last updated it.", "737060cd8c284d8af7ad3082f209582d" },
		{BP_FALSE, "If-Modified-Since",   "Allows a 304 Not Modified to be returned if content is unchanged.", "If-Modified-Since: Sat, 29 Oct 1994 19:43:31 GMT" },
		{BP_FALSE, "If-None-Match",       "Allows a 304 Not Modified to be returned if content is unchanged, see HTTP ETag.", "737060cd8c284d8af7ad3082f209582d" },
		{BP_FALSE, "If-Range",            "If the entity is unchanged, send me the part(s) that I am missing; otherwise, send me the entire new entity.", "737060cd8c284d8af7ad3082f209582d" },
		{BP_FALSE, "If-Unmodified-Since", "Only send the response if the entity has not been modified since a specific time.", "Sat, 29 Oct 1994 19:43:31 GMT" },
		{BP_FALSE, "Max-Forwards",        "Limit the number of times the message can be forwarded through proxies or gateways.", "10" },
		{BP_FALSE, "Origin",              "Initiates a request for cross-origin resource sharing (asks server for an 'Access-Control-Allow-Origin' response field.", "http://www.example.com" },
		{BP_FALSE, "Pragma",              "Implementation-specific fields that may have various effects anywhere along the request-response chain.", "no-cache" },
		{BP_FALSE, "Proxy-Authorization", "Authorization credentials for connecting to a proxy.", "Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==" },
		{BP_FALSE, "Range",               "Request only part of an entity.  Bytes are numbered from 0.", "bytes=500-999" },
		{BP_FALSE, "Referer",             "This is the address of the previous web page from which a link to the currently requested page was followed. (spelled incorrectly on purpose).", "http://previous-page.com" },
		{BP_FALSE, "TE",                  "The transfer encodings the user agent is willing to accept:  the same values as for the response header field Transfer-Encoding can be used, plus the 'trailers' value (related to the 'chunked' transfer method) to notify the server it expects to recieve additional fields in the trailer after the last, zero-sized, chunk.", "trailers, deflate" },
		{BP_FALSE, "User-Agent",          "The user agent string of the user agent..", "Mozilla/5.0" },
		{BP_FALSE, "Upgrade",             "Ask the server to upgrade to another protocol.", "HTTP/2.0, SHTTP/1.3, IRC/6.9, RTA/x11" },
		{BP_FALSE, "Via",                 "Informs the server of proxies through which the request was sent.", "1.0 fred, 1.1 example.com (Apache/1.1)" },
		{BP_FALSE, "Warning",             "A general warning about possible problems with the entire body.", "199 Miscellaneous warning" },
		{BP_FALSE, "X-Requested-With",    "Mainly used to identify Ajax requests.  Most javascript frameworks send this field with a value of XMLHttpRequest.", "XMLHttpRequest" },
		{BP_FALSE, "DNT",                 "Requests a web application to disable their tracking of a user.", "1" },
		{BP_FALSE, "X-Forwarded-For",     "A de facto standard for identifying the originating IP address of a client connecting to a web server through an HTTP proxy or load balancer.", "client1, proxy1, proxy2, 127.0.0.1" },
		{BP_FALSE, "X-Forwarded-Proto",   "A de facto standard for identifying the originating protocol of an HTTP request, since a reverse proxy (load balancer) may communicate with a web server using HTTP even if the request to the reverse proxy is HTTPS.", "https" },
		{BP_FALSE, "Front-End-Https",     "A non-standard header field used by Microsoft applications and load-balancers.", "on" },
		{BP_FALSE, "X-ATT-DeviceId",      "Allows easier parsing of the MakeModel/Firmware that is usually found in the User-Agent String of AT&T Devices.", "GT-P7320/P732XXLPG" },
		{BP_FALSE, "X-Wap-Profile",       "Links to an XML file on the internet with a full description and details about the device currently connecting.", "http://wap.samsungmobile.com/uaprof/SGH-I777.xml" },
		{BP_FALSE, "Proxy-Connection",    "Implemented as a misunderstanding of the HTTP specifications.  Common because of mistakes in implementations of early HTTP versions.  Has exactly the same functionality as standard connection field.", "keep-alive" }
};



// *NON-INLINE VERSION* Function will take a C string and convert it to a ASM
char * BP_ASMStringEncode(char *str, int len)
{

		// allocate/clear memory
        char *encodedStr = (char *) bpcalloc( len * 10 + ( (len/4) * 20), 1);

        // calculate lengths
        int newLen = len - (len%4);
        int filler = 4 - (len - newLen) ;

        // initialize pop count (for argument removal)
        int popCount = 0;

        // index
        int i = 0;
        int preBytes ;

        // add asm header (c)
        strcat(encodedStr, "asm(\"");

        // add initial push data
        if(len % 4 > 0){
                // add one pop
                popCount++;

                strcat(encodedStr, "\n\"push $0x");
                for(preBytes = 1; preBytes < filler; preBytes++){
                        strcat(encodedStr, "00");
                }

                for(i = newLen+(len%4); i >= newLen; i--){
                        sprintf(&encodedStr[bpstrlen(encodedStr)], "%.2x", str[i]);
                }
                strcat(encodedStr, ";\"");
        }

        // add push data
        for(i = newLen-1; i > 0; i-=4){
                popCount++;
                sprintf(&encodedStr[bpstrlen(encodedStr)],
                        "\n\"push $0x%.2x%.2x%.2x%.2x;\" ", str[i], str[i-1], str[i-2], str[i-3]);

        }

        // add asm
        sprintf(&encodedStr[bpstrlen(encodedStr)], "\"mov $4, %%eax\";\""
												 "\n\"mov $1,\" %%ebx;\"\n"
												 "\"mov %%esp, %%ecx;\"\n"
												 "\"mov $%i, %%edx;\"\n"
												 "\"int $0x80;\"\n", len);

        // drop in pop count
        for(i = 0; i < popCount; i++)
        {
        	strcat(encodedStr, "pop %eax;");
        }

        // terminate asm
        strcat(encodedStr, "\");");

        // return the encoded string
        return encodedStr;

}


// *INLINE VERSION* Function will take a C string and convert it to a ASM
char * BP_InlineASMStringEncode(char *str, int len)
{

	char *encodedStr = (char *) bpmalloc( len * 10 + ( (len/4) * 20));
	memset(encodedStr, 0x00, len * 10+((len/4) * 20));

	int newLen = len - (len%4);
	int filler = 4 - (len - newLen) ;

	int popCount = 0;

	int i = 0;
	int preBytes ;
	strcat(encodedStr, "asm(\"");
	if(len % 4 > 0){
		// add one pop
		popCount++;

		strcat(encodedStr, "push $0x");
		for(preBytes = 1; preBytes < filler; preBytes++){
			strcat(encodedStr, "00");
		}

		for(i = newLen+(len%4); i >= newLen; i--){
			sprintf(&encodedStr[bpstrlen(encodedStr)], "%.2x", str[i]);
		}
		strcat(encodedStr, ";");
	}

	for(i = newLen-1; i > 0; i-=4){
		popCount++;
		sprintf(&encodedStr[bpstrlen(encodedStr)],
			"push $0x%.2x%.2x%.2x%.2x; ", str[i], str[i-1], str[i-2], str[i-3]);

	}


	sprintf(&encodedStr[bpstrlen(encodedStr)], "mov $4, %%eax;mov $1, %%ebx;mov %%esp, %%ecx;mov $%i, %%edx;int $0x80;", len);
	for(i = 0; i < popCount; i++){
		strcat(encodedStr, "pop %eax;");
	}
	strcat(encodedStr, "\");");


	// return the encoded string
	return encodedStr;

}

// Function will take a C ascii string and return the value
// as a 0x hex encoded string for use with mysql.
char * BP_PHPInjectionStringEncode(char *str, int len){

	char * encodedStr = (char *) bpmalloc(len * 5);
	memset(encodedStr, 0x00, len*5);

	strncat(encodedStr, "0x", 2);
	int x = 0, i = 0;
	for(i = bpstrlen(encodedStr); i <= (bpstrlen(str) * 2); i+=2){
		sprintf(&encodedStr[i], "%.02x", str[x]);
		x++;
	}

	// bpfree when done
	return encodedStr;
}


// Function will take a C ascii string and return the value
// as a 0x hex encoded string for use with mysql.
char * BP_CInjectionStringEncode(char *str, int len)
{
	char * encodedStr = (char *) bpmalloc(len * 5);
    memset(encodedStr, 0x00, len*5);

    int x = 0, i = 0;
    for(i = bpstrlen(encodedStr); i <= (bpstrlen(str) * 4)-1; i+=4){
                sprintf(&encodedStr[i], "\\x%.02x", str[x]);
                x++;
        }

        // bpfree when done
        return encodedStr;
}


//
// Function will take a C ascii string and return the value
// as a CHAR() encoded string
char * BP_MYSQLCharFuncInjectionStringEncode(char *str, int len){

        char * encodedStr = (char *) bpcalloc(len * 100, 1);
        strncat(encodedStr, "CHAR(", 5);

        int x = 0;
        int i = 0;

        for(i = 0; i < len; i++) {
                sprintf(&encodedStr[bpstrlen(encodedStr)], "%i,", str[x]);
                x++;
        }

        // terminate string
        encodedStr[bpstrlen(encodedStr)-1] = ')';

        // bpfree when done
        return encodedStr;

}

// Encodes an ascii string in MSQL CHAR() + Format
char * BP_MSQLCharFuncInjectionStringEncode(char *str, int len)
{
	// set encoded string
	char * encodedStr = (char *) bpmalloc(len * 100);
    memset(encodedStr, 0x00, len*100);

        size_t x = 0, i = 0;


        for(i = 0; i < len; i++) {
        	strncat(encodedStr, "CHAR(", 5);
               	sprintf(&encodedStr[bpstrlen(encodedStr)], "%i)+", str[x]);

                x++;
        }

        // terminate string
        encodedStr[bpstrlen(encodedStr)-1] = '\0';

        // bpfree when done
        return encodedStr;

}

// convert buffer to hex string. eg \x00 = 00.  String
// needs to be bpfreed' after allocation.  Null on return.
char * BP_BufferToHexString(char *str, size_t len)
{
	if(!str || !len)
		return NULL;

	// bpreallocated buffer to return
	char * out_buff = NULL;

	// allocate space for storing output
	out_buff = (char *) bpcalloc((len*2)+1, 1);

	// tmp buff for holding encoding
	char tmp_buff[4];

	// walk each character in set and sprintf
	size_t n = 0;
	for(; n < len; n++)
	{

		// zero the buffer
		BP_ZERO_STACK(tmp_buff);

		// create hex conversion
		sprintf(tmp_buff, "%.02x", (unsigned char) str[n]);

		// append the converted string
		strcat(out_buff, (char *) tmp_buff);

	}

	// return the buffer
	return out_buff;

}

// convert buffer to hex string. eg \x00 = 00.  String
// needs to be bpfreed' after allocation.  Null on return.
char * BP_BufferToMySQLHexString(char *str, size_t len, P_BP_LINKL tq)
{

	if(!str || !len)
	{

		if(!tq)
			return bpstrdup("NULL");
		else
			return tq_strdup(tq, "NULL", BPLN);

	}

	// bpreallocated buffer to return
	char * out_buff = NULL;

	// temp buff in loop scope for holding conversion
	char buff[16] = {0};

	// walk each character in set and sprintf
	size_t n = 0;
	size_t alloc_len = 16;
	size_t curr_idx = 0;
	size_t first_iter = 0;
	for(; n < len; n++)
	{

		if(first_iter == 0)
		{
			// allocate space for 2 bytes + null terminator
			if(!tq)
				out_buff = (char *) bprealloc(out_buff, alloc_len);
			else
				out_buff = (char *) tq_realloc(tq, out_buff, alloc_len, BPLN);

			memset((void *) out_buff, 0x00, alloc_len);
			strcat(out_buff, (char *) "0x");
			curr_idx+=2;
			// remove the first iteration constraint
			first_iter=1;

		}
		else
		{

			// allocate space for 2 bytes + null terminator
			if(!tq)
				out_buff = (char *) bprealloc(out_buff, alloc_len);
			else
				out_buff = (char *) tq_realloc(tq, out_buff, alloc_len, BPLN);

		}
		// zero allocation on loop
		memset((char *) &out_buff[curr_idx], 0x00, 3);

		// zero out the buffer
		memset((void *)&buff, 0x00, sizeof(buff));

		// create hex conversion
		sprintf((char *) &buff, "%.02x", str[n]);

		// append the converted string
		strcat(out_buff, (char *) &buff);

		// zero buffer after appending
		memset((char *) &buff, 0x00, sizeof(buff));

		// increase allocation by 3
		alloc_len+=3;
		curr_idx+=2;
	}

	// return the buffer
	return out_buff;

}


// Ascii to UTF8 Convertor
char * BP_UTF8StringEncode(char *str, int len, P_BP_LINKL tq)
{

	if(!str)
		return NULL;
	if(!len)
		return NULL;

	// encoded string which will be returned
	char * encodedStr = NULL;

	// allocate on the tq if one is supplied
	if(!tq)
		encodedStr = (char *) bpcalloc(len * 5, 1);
	else
		encodedStr = (char *) tq_calloc(tq, (len*5), 1, BPLN);

	memset(encodedStr, 0x00, len*5);

	int x = 0, i = 0;
	for(i = bpstrlen(encodedStr); i <= (bpstrlen(str) * 3)-1; i+=3){
			sprintf(&encodedStr[i], "%%%.02x", str[x]);
			x++;
	}

	// bpfree when done
	return encodedStr;

}

// String From Char Code XSS Encoder
char * BP_XSSInjectionStringEncode(char *str, int len){

        char * encodedStr = (char *) bpmalloc(len * 5);
                memset(encodedStr, 0x00, len*5);

        strncat(encodedStr, "String.fromCharCode(", 20);
        int x = 0, i = 0;


        for(i = 0; i < len; i++) {
                sprintf(&encodedStr[bpstrlen(encodedStr)], "%i,", str[x]);
                x++;
        }

	// terminate string
	encodedStr[bpstrlen(encodedStr)-1] = ')';

        // bpfree when done
        return encodedStr;

}

// ------------------------------------------------------------------------------
// End String Encoding Routines:

// Begin CURL Web Page Manipulation Routines


// Static Reallocation Routine
void * BP_myrealloc(void *ptr, size_t size){
	if(ptr)
		return bprealloc(ptr, size);
	else
		return bpmalloc(size);
}

// Create a callback to handle read data
size_t BP_GetPageCallback(void *ptr, size_t size, size_t nmemb, void *data)
{

	// pointer to elements
	size_t realsize = size * nmemb;

	// set page data
	P_CURL_READ_WEBPAGE mem = (P_CURL_READ_WEBPAGE) data;

	// allocate space for the page
	mem->memPtr = (char *) BP_myrealloc(mem->memPtr, mem->size + realsize + 1);

	// copy in data, if not, return failure
	if(mem->memPtr != NULL)
	{
		// copy in the data
		memcpy(&(mem->memPtr[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memPtr[mem->size] = 0;
	}
	else
	{
		// cannot read page, returning failure
		return ERR_FAILURE;
	}

	return nmemb;

}

// Searches for our error string, this is how we know the enumeration is
// done.


// NULL for blank in either value (Allocates new userPass Chunk)
int BP_HTTPSetUserPass(P_BP_HTTP_SESSION session, char *user, char *pass){

	// ensure we have a session
	if(session == NULL)
		return ERR_FAILURE;

	int userLen = 0;
	int passLen = 0;

	// make sure we have u/p values
	if(user != NULL)
		userLen = strnlen(user, 255);
	if(pass != NULL)
		passLen = strnlen(pass, 255);

	// allocate space for the user/pass data
	char * userPass = (char *) bpcalloc(1024, 1);

	// ensure we have a valid pointer, if not return
	if(!userPass)
		return ERR_FAILURE;

	// place user and password in the buffer
	snprintf(userPass, 1023, "%s:%s", user, pass);

	// set the option in curl
	curl_easy_setopt(session->curl, CURLOPT_USERPWD, userPass);

	// return indicating success
	return ERR_SUCCESS;
}

// Sets cookie
int BP_HTTPSetCookie(P_BP_HTTP_SESSION session, char *cookie){
	return curl_easy_setopt(session->curl, CURLOPT_COOKIE, cookie);
}

// Sets cookie jar (holds cookies done with authentication)
int BP_HTTPSetCookieJar(P_BP_HTTP_SESSION session, char *cookie_jar_file){
	return curl_easy_setopt(session->curl, CURLOPT_COOKIEJAR, cookie_jar_file);
}

// Gets a duplicated cookie string from the last set of headers recieved
char * BP_HTTPGetCookieFromLastHeadersRecievedSetCookie(P_BP_HTTP_SESSION session)
{

	if(!session)
		return NULL;
	if(!session->headers)
		return NULL;


	size_t i = 0;
	size_t n = 0;
	for(; n < session->headers->header_count; n++)
	{

		// only work with non-null headers
		if(!session->headers->headers[n].header_index)
			continue;

		// check if the header is a cookie
		if(BP_strncasecmp(session->headers->headers[n].header_index, "set-cookie", bpstrlen("set-cookie")) == 0)
		{
			printf("\n Got Cookie: %s", session->headers->headers[n].value);
			curl_easy_setopt(session->curl, CURLOPT_COOKIE, session->headers->headers[n].value);
		}

	}

	return NULL;
}

// Sets HTTP POST Arguments
int BP_HTTPSetPostVars(P_BP_HTTP_SESSION session, char *postString)
{

	// ensure we have a session
	if(!session)
		return ERR_FAILURE;

	// destroy previous post data if it exists
	if(session->httpPostData)
	{
		if(!session->is_clone)
			bpfree(session->httpPostData);

		session->httpPostData = NULL;
	}

	// set the post data if necessary
	if(!session->is_clone)
		session->httpPostData = bpstrdup(postString);
	else
		session->httpPostData = tq_strdup(session->m, postString, BPLN);

	// attempt to set option and return
	return curl_easy_setopt(session->curl, CURLOPT_POSTFIELDS, session->httpPostData);

}

// Sets HTTP User Agent
int BP_HTTPUserAgent(P_BP_HTTP_SESSION session, char *agent)
{

	// attempt to set option and return
	return curl_easy_setopt(session->curl, CURLOPT_USERAGENT, agent);

}

// Destroys a sessions iterative data only.  This can be safely used
// in web result stack (see: Block class) iterative loops.
BP_ERROR_T BP_HTTPDestroySessionIterativeData(P_BP_HTTP_SESSION session)
{


	// check session
	if(!session)
		return ERR_FAILURE;



	// clean up curl danglers first
	if(session->curl != NULL)
	{

		curl_easy_cleanup(session->curl);
		session->curl = NULL;

		// destroy slist here if necessary
		if(session->request_header_chunk_list)
		{
			curl_slist_free_all(session->request_header_chunk_list);
			session->request_header_chunk_list = NULL;
		}


	}

	// destroy cookie
	if(session->cookie != NULL)
	{
		if(!session->is_clone)
			bpfree(session->cookie);
		else
			tq_free(session->m, session->cookie);

		session->cookie = NULL;
	}

	// destroy post data if set
	if(session->httpPostData)
	{

		if(!session->is_clone)
			bpfree(session->httpPostData);
		else
			tq_free(session->m, session->httpPostData);

		session->httpPostData = NULL;
	}

	// destroy cookie from file if set
	if(session->cookieFromFile != NULL)
	{

		if(!session->is_clone)
			bpfree(session->cookieFromFile);
		else
			tq_free(session->m, session->cookieFromFile);

		session->cookieFromFile = NULL;
	}

	// destroy jit checks
	if(!session->is_clone)
	if(session->jit_pcre_regexp_checks)
	{
		BP_DestroyJITMatchEntries(session->jit_pcre_regexp_checks);
		session->jit_pcre_regexp_checks = NULL;
		session->jit_pcre_regexp_check_n = 0;
	}

	// destroy headers
	if(!session->is_clone)
	if(session->headers)
	{
		BP_HTTPDestroyParsedHeaders(session->headers);
		session->headers      = NULL;
		session->header_count = 0;
	}

	// destroy blacklist
	if(!session->is_clone)
	if(session->request_regexp_blacklist)
	{

		// calculate index count
		size_t n = 0;
		for(; session->request_regexp_blacklist[n]; n++){}

		// destroy individual pointers in the array
		BP_DestroyPointerArray((void **) session->request_regexp_blacklist, n);

		// free the actual allocated block
		bpfree(session->request_regexp_blacklist);

		// set pointer to null
		session->request_regexp_blacklist = NULL;

	}

	if(session->lastRequestedHref)
	{

		if(!session->is_clone)
			bpfree(session->lastRequestedHref);
		else
			tq_free(session->m, session->lastRequestedHref);

		session->lastRequestedHref = NULL;
	}


	// return indicating success
	return ERR_SUCCESS;

}

// destroy a cloned session
BP_ERROR_T BP_HTTPDestroyClonedSession(P_BP_HTTP_SESSION session)
{

	// ensure we have a session
	if(!session)
		return ERR_FAILURE;

	// ensure that the session is a clone
	if(session->is_clone)
	{
		if(session->page->memPtr)
		{
			bpfree(session->page->memPtr);
			session->page->memPtr = NULL;
		}

		// clean up curl danglers first
		if(session->curl != NULL)
		{
			curl_easy_cleanup(session->curl);
			session->curl = NULL;
		}

		// destroy slist here if necessary
		if(session->request_header_chunk_list)
		{
			curl_slist_free_all(session->request_header_chunk_list);
			session->request_header_chunk_list = NULL;
		}

		BP_DestroyLinkL(session->m, BP_TRUE);


		if(session->headers)
		{
			BP_HTTPDestroyParsedHeaders(session->headers);
			session->headers      = NULL;
			session->header_count = 0;
		}



		bpfree(session);
	}

	// return indicating success
	return ERR_SUCCESS;
}

// Destroys a HTTP Session
int BP_HTTPDestroySession(P_BP_HTTP_SESSION session)
{

	// cannot destroy cloned sessions using this
	if(session->is_clone)
	{
		printf("\n [ERR] To destroy a cloned HTTP session use BP_HTTPDestroyClonedSession() instead. ");
		return ERR_FAILURE;
	}

	// check to see if we have a dangling href
	if(session->lastRequestedHref)
	{
		bpfree(session->lastRequestedHref);
		session->lastRequestedHref = NULL;
	}

	// clean up curl danglers first
	if(session->curl != NULL)
	{
		curl_easy_cleanup(session->curl);
		session->curl = NULL;
	}

	if(session->cookie != NULL)
	{
		bpfree(session->cookie);
		session->cookie = NULL;
	}

	if(session->cookieFromFile != NULL)
	{
		session->cookieFromFile = NULL;
		bpfree(session->cookieFromFile);
	}

	if(session->fileName != NULL)
	{
		bpfree(session->fileName);
		session->fileName = NULL;
	}

	if(session->httpPostData != NULL)
	{
		bpfree(session->httpPostData);
		session->httpPostData = NULL;
	}

	if(session->interface != NULL)
	{
		bpfree(session->interface);
		session->interface = NULL;
	}

	if(session->netrcFile != NULL)
	{
		bpfree(session->netrcFile);
		session->netrcFile = NULL;
	}

	if(session->proxies != NULL)
	{
		bpfree(session->proxies);
		session->proxies = NULL;
	}

	if(session->proxyCredentials != NULL)
	{
		bpfree(session->proxyCredentials);
		session->proxyCredentials = NULL;
	}

	if(session->jit_pcre_regexp_checks)
	{
		BP_DestroyJITMatchEntries(session->jit_pcre_regexp_checks);
		session->jit_pcre_regexp_checks = NULL;
		session->jit_pcre_regexp_check_n = 0;
	}

	if(session->headers)
	{
		BP_HTTPDestroyParsedHeaders(session->headers);
		session->headers      = NULL;
		session->header_count = 0;
	}

	if(session->request_regexp_blacklist)
	{

		// destroy individual pointers in the array
		BP_DestroyPointerArrayTAILQ(session->m, (void **) session->request_regexp_blacklist, session->request_regexp_blacklist_n);

		// free the actual allocated block
		tq_free(session->m, session->request_regexp_blacklist);

		// set pointer to null
		session->request_regexp_blacklist = NULL;

	}

	// check memptr.  ONLY MEMPTR NEEDS TO BE FREED, OTHER DATA POINTERS
	// POINT INTO MEMPTR AND ARE REFERENCE POINTERS WITHIN THE CHUNK.
	if(session->page)
	if(session->page->memPtr)
	{
		bpfree(session->page->memPtr);
		session->page->memPtr = NULL;
	}

	// destroy page
	if(session->page)
	{
		bpfree(session->page);
		session->page = NULL;
	}

	// destroy the search results
	BP_HTTPDestroyRequestHeadersSearchResults(session);

	// destroy the request headers
	BP_HTTPDestroyRequestHeaders(session);

	// destroy slist here if necessary
	if(session->request_header_chunk_list)
	{
		curl_slist_free_all(session->request_header_chunk_list);
		session->request_header_chunk_list = NULL;
	}

	// destroy tailqueue allocator
	if(session->m)
		BP_DestroyLinkL(session->m, BP_TRUE);

	// free the session
	bpfree(session);

	// return indicating success
	return ERR_SUCCESS;

}

// This does not destroy an entire session, but only bpfrees its
// CURL pointer data.  This function is primarily used with the
// proxy subsystem in order to create new curl sessions on the fly
// without taking down the entire http session.
int BP_HTTPDestroySessionCurlOnly(P_BP_HTTP_SESSION session)
{

	if(!session)
		return ERR_FAILURE;

	// destroy the curl data (easy)
	if(session->curl)
		curl_easy_cleanup(session->curl);

	// reset the pointer
	session->curl = NULL;

	// return indicating success
	return ERR_SUCCESS;

}

// add a pcre blacklist items
BP_ERROR_T BP_HTTPAddPCREBlacklistItem(P_BP_HTTP_SESSION session, char * pcre)
{

	// check to see if we have a session
	if(!session)
		return ERR_FAILURE;

	// check to see if we have a pcre
	if(!pcre)
		return ERR_FAILURE;

	// check to ensure that the length is ok
	if(!bpstrlen(pcre))
		return ERR_FAILURE;


	// attempt to reallocate blacklist
	session->request_regexp_blacklist = (char **) tq_realloc(session->m, session->request_regexp_blacklist, (session->request_regexp_blacklist_n+1) * sizeof(char *), BPLN);
	if(!session->request_regexp_blacklist)
	{
		return ERR_FAILURE;
	}

	// set the blacklist
	session->request_regexp_blacklist[session->request_regexp_blacklist_n] = tq_strdup(session->m, pcre, BPLN);

	// increment the regexp blacklist
	session->request_regexp_blacklist_n++;

	// return indicating success
	return ERR_SUCCESS;

}

// validate entries
BP_ERROR_T BP_HTTPValidatePCREBlacklistAgainstURL(P_BP_HTTP_SESSION session, char *URL)
{

	// ensure we have a session
	if(!session)
		return ERR_FAILURE;

	// ensure we have a url
	if(!URL)
		return ERR_FAILURE;

	// ensure we have a blacklist count
	if(!session->request_regexp_blacklist_n)
		return ERR_FAILURE;

	// create zeroed out inout structure
	BP_REGEX_MATCH_INFO match_info;

	printf("\n REGEX VALIDATING: %s", URL);

	// walk all blacklist items
	size_t n = 0;
	for(; n < session->request_regexp_blacklist_n; n++)
	{

		// zero out the match data before loop
		BP_ZERO_STACK(match_info);

		// run match check
		if(BP_MatchRegularExpressionPCRE(URL, bpstrlen(URL)-1, session->request_regexp_blacklist[n], 0, 1024, &match_info, 0) != ERR_SUCCESS)
		{

			// if we couldn't match the regexp, destroy match data and exit routine
			BP_DestroyRegexpMatchInfo(&match_info);
			return ERR_FAILURE;

		}

		// destroy the regexp match information
		BP_DestroyRegexpMatchInfo(&match_info);

	}

	// if we passed all checks, return indicating success
	return ERR_SUCCESS;

}


// Gets a new HTTP String (clean_session is true or false, and indicates whether or not to destroy iterative data and clear
// out curl data.
int BP_HTTPRequest(P_BP_HTTP_SESSION session, char *URL, size_t clean_session)
{

	// ensure we have a session
	if(!session)
		return ERR_FAILURE;
	if(!URL)
		return ERR_FAILURE;

	// ensure the string is reasonable/printable
	if(!BP_StringIsReasonablePrintableString(URL, bpstrlen(URL), BP_FALSE, BP_FALSE))
		return ERR_FAILURE;

	// needs to at least have http/https
	if(bpstrlen(URL) < 4)
		return ERR_FAILURE;

	// get url length
	size_t url_len = bpstrlen(URL);

	// ensure we're working with http
	if(memcmp(URL, "http", 4) != 0)
		return ERR_FAILURE;

	// create zeroed out inout structure
	BP_REGEX_MATCH_INFO match_info = {0};



	// check for regexp blacklist items
	if(session->request_regexp_blacklist)
	{

		// validate entries
		if(BP_HTTPValidatePCREBlacklistAgainstURL(session, URL) != ERR_SUCCESS)
			return ERR_FAILURE;

	}


	// Destroy any dangling data before making a next request.  You must save this data
	// by this point if you have not already done so as it is to be destroyed herein.
	if(session->page)
	if(session->page->memPtr)
	{

		// destroy memory pointer
		bpfree(session->page->memPtr);

		// zero out parameters
		session->page->memPtr = NULL;
		session->page->size    = 0;

	}


	// clean the session if so specified (default)
	if(clean_session)
	{

		// destroy any dangling data
		BP_HTTPDestroySessionIterativeData(session);

		// invoke upon the new session
		BP_HTTPInvokeSession(session);

	}

	// sleep random amount for evasion if configured to do so
	if(session->use_request_timing)
	{
		// srand(time(NULL));

		// attempt to set global pnrg singleton if not already set
		BP_SeedPNRG();

		// sleep based on random value
		sleep(rand()%session->request_time_modulo);

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %% Destroy JIT matches before run (if necessary) %%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// never have more than one match set in memory at once
	size_t i = 0;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %% Load Proxy Configuration (Non-DB/OLD) %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// NOTE: If using DB proxy capability, proceed to section directly below this.

	if(!session->disable_proxy_check_in_http_request)
	if(!session->use_db_proxy_set) /* only one type of proxy set can be used / natively prefer db oriented if both are set */
	if(session->use_web_proxy_set)
	{


		// Destroy the session / create new.  This is required in order to
		// establish a new proxy connection.
		BP_HTTPDestroySessionCurlOnly(session);
		BP_HTTPInvokeSession(session);

		// disable signals for threads
		curl_easy_setopt(session->curl, CURLOPT_NOSIGNAL, 1);

		// reset proxy index if necessary
		if(session->proxy_set)
		if(session->proxy_set->last_proxy_idx >= session->proxy_set->proxy_cnt)
		{
			session->proxy_set->last_proxy_idx = 0;
		}

		// Now run a disabled proxy check.  The while loop will only exit if it
		// finds a good proxy, if not it will return to the previous application
		// with an error code.
		size_t cycle_check = 0;

		if(session->proxy_set)
		while(session->proxy_set->proxies[session->proxy_set->last_proxy_idx].disabled_due_to_bad_validation)
		{

			if(session->proxy_set->last_proxy_idx >= session->proxy_set->proxy_cnt)
			{

				cycle_check++;
				if(cycle_check == 2)
				{
					printf("\n [!!] When attempting to make a web request, the proxy pool was checked for available proxies.  None were found.  (all records were marked disabled).");
					return ERR_FAILURE;
				}
				else
				{
					// reset index to 0 and cycle all proxies again
					session->proxy_set->last_proxy_idx = 0;
				}

			}
			else
			{

				session->proxy_set->last_proxy_idx++;

			}

		}

		// set temporary last proxy index from stored value
		size_t proxy_idx = session->proxy_set->last_proxy_idx;

		// HTTPSetProxy(session, session->proxy_set->proxies[proxy_idx].connect_string);
		size_t test = curl_easy_setopt(session->curl, CURLOPT_PROXY, session->proxy_set->proxies[proxy_idx].connect_string);

		// Sets a proxy with a defined type
		switch(session->proxy_set->proxies[proxy_idx].proxy_type)
		{

			case BP_WEB_PROXY_TYPE_HTTP:
				BP_HTTPSetProxyType(session, CURLPROXY_HTTP);
				break;

			case BP_WEB_PROXY_TYPE_SOCKS4:
				BP_HTTPSetProxyType(session, CURLPROXY_SOCKS4);
				break;

			case BP_WEB_PROXY_TYPE_SOCKS4A:
				BP_HTTPSetProxyType(session, CURLPROXY_SOCKS4A);
				break;

			case BP_WEB_PROXY_TYPE_SOCKS5:
				BP_HTTPSetProxyType(session, CURLPROXY_SOCKS5);
				break;

			default:
				BP_HTTPSetProxyType(session, CURLPROXY_SOCKS5);
				break;
		}

		// set flag to one to skip this proxy check (prevents recursion)
		session->disable_proxy_check_in_http_request = 1;

		// Validate HTTP Proxy
		BP_HTTPSetTimeout(session, 2);

		// Now validate proxy in specified session
		if(BP_HTTPValidateCurrentProxy(session, "http://localhost/valid", "validated") != ERR_SUCCESS)
		{

			if(!(session->proxy_set->last_proxy_idx >= session->proxy_set->proxy_cnt))
			{

				// printf("\n Could not verify current proxy: %s", session->proxy_set->proxies[proxy_idx].connect_string);
				session->proxy_set->proxies[proxy_idx].disabled_due_to_bad_validation = 1;

				// move proxy index in the case of a bad proxy
				session->proxy_set->last_proxy_idx++;

				// reset flag to zero and continue request
				session->disable_proxy_check_in_http_request = 0;

				// return indicating failure
				return ERR_FAILURE;

			}

		}
		else
		{
			// printf("\n Proxy verified successfully: %s", session->proxy_set->proxies[proxy_idx].connect_string);
		}

		// reset flag to zero and continue request
		session->disable_proxy_check_in_http_request = 0;

	}




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %% Proxy Database Integration (NEW/Use This) %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// reentry loop label
proxy_db_reentry:


	// flag to indicate that a proxy has been selected, chosen, and ready to work
	size_t proxy_confirmed_working_and_ready = 0;

	size_t db_proxy_idx = 0;
	size_t db_validation_test_idx = 0;

	// Flag indicating whether or not a proxy has been
	// successfully validated in the JIT logic below.
	size_t proxy_db_jit_validation_success = ERR_FAILURE;

	// run loop logic to set/validate proxy
	if(session->use_db_proxy_set)
	if(!session->disable_proxy_check_in_http_request)
	do
	{

		// check to ensure that we have a proxy count; if not, exit routine
		if(!session->db_populated_proxy_n)
		{
			printf("\n [!!] ERROR, no proxies found in proxy set which is being invoked upon a web request.  Empty data.  Check proxy retrieval.");
			return ERR_FAILURE;
		}

		// reset validation success on loop
		proxy_db_jit_validation_success = ERR_FAILURE;

		// reset indexes on loop
		db_proxy_idx           = 0;
		db_validation_test_idx = 0;

		// Ensure we have both validation test records as well as proxy sets to test.  Always exit here, as we do
		// not want to attempt to run a request with an invalid or unpopulated proxy set.
		if( !session->db_populated_proxy_n || !session->db_populated_proxy_validation_group_n)
		{

			// display error message indicating that we have a mismatch
			printf("\n [!!] ERROR, session->db_populated_proxy_n = %u, session->db_populated_proxy_validation_group_n = %u (when both should be > 0) ", session->db_populated_proxy_n, session->db_populated_proxy_validation_group_n);
			return ERR_FAILURE;

		}

		// create the proxy index to use (randomly set)
		db_proxy_idx = BP_GenRandomUnsignedLong(0, session->db_populated_proxy_n-1);

		// create the validation test index to use (randomly chosen)
		db_validation_test_idx = BP_GenRandomUnsignedLong(0, session->db_populated_proxy_validation_group_n-1);

		// only attempt to run on validated proxy entries
		if
		(
			!session->db_populated_proxy_sets[db_proxy_idx].enabled ||
			!session->db_populated_proxy_sets[db_proxy_idx].validation_status
		)
		{
			continue;
		}

		// establish a new proxy connection.
		BP_HTTPDestroySessionCurlOnly(session);
		BP_HTTPInvokeSession(session);

		// disable signals for threads
		curl_easy_setopt(session->curl, CURLOPT_NOSIGNAL, 1);

		// allocate and zero space on stack for host:port combination (4096 is > max)
		char conn_string[4096] = {0};

		// format connection string
		snprintf
		(
			(char *) &conn_string,
			sizeof(conn_string)-1,
			"%s:%u",
			session->db_populated_proxy_sets[db_proxy_idx].host,
			session->db_populated_proxy_sets[db_proxy_idx].port
		);

		// HTTPSetProxy(session, session->proxy_set->proxies[proxy_idx].connect_string);
		size_t test = curl_easy_setopt(session->curl, CURLOPT_PROXY, conn_string);

		// Sets a proxy with a defined type
		switch(session->db_populated_proxy_sets[db_proxy_idx].proxy_type)
		{

			case BP_WEB_PROXY_TYPE_HTTP:
				BP_HTTPSetProxyType(session, CURLPROXY_HTTP);
				break;
			case BP_WEB_PROXY_TYPE_SOCKS4:
				BP_HTTPSetProxyType(session, CURLPROXY_SOCKS4);
				break;
			case BP_WEB_PROXY_TYPE_SOCKS4A:
				BP_HTTPSetProxyType(session, CURLPROXY_SOCKS4A);
				break;
			case BP_WEB_PROXY_TYPE_SOCKS5:
				BP_HTTPSetProxyType(session, CURLPROXY_SOCKS5);
				break;
			default:
				BP_HTTPSetProxyType(session, CURLPROXY_HTTP);
				break;
		}

		// only validate if we we aren't marked to skip, and have validation groups
		if((session->skip_jit_validation_checks == false) && session->db_populated_proxy_validation_group_n)
		{

			// set flag to one to skip this proxy check (prevents recursion)
			session->disable_proxy_check_in_http_request = 1;

			// Validate HTTP Proxy
			BP_HTTPSetTimeout(session, 3);
			printf("\n px connect: %s", conn_string);

			// now attempt to validate the proxy using the request
			proxy_db_jit_validation_success = BP_HTTPValidateCurrentProxy
			(
					session,
					session->db_populated_proxy_validation_tests[db_validation_test_idx].url_fetch,
					session->db_populated_proxy_validation_tests[db_validation_test_idx].regexp_match
			);

		}
		else
			proxy_db_jit_validation_success = ERR_SUCCESS;

		// if the validation was successful we can mark the loop
		// branch for exit.
		if(proxy_db_jit_validation_success == ERR_SUCCESS)
			proxy_confirmed_working_and_ready = true;

	} while( !proxy_confirmed_working_and_ready);




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %% Set URL and Make Request %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// set the url first
	curl_easy_setopt(session->curl, CURLOPT_URL, URL);



	// set post data here if necessary
	if(session->httpPostData)
	{
		// copy the post data so it can be duplicated and
		// reset in the routine below without double free
		char * tmp_str = bpstrdup(session->httpPostData);

		// copy set post data
		BP_HTTPSetPostVars(session, tmp_str);

		// destroy the copy to remove any possibility of leak
		bpfree(tmp_str);

	}



	// destroy href after request
	if(session->lastRequestedHref)
	{

		if(!session->is_clone)
			bpfree(session->lastRequestedHref);
		else
			tq_free(session->m, session->lastRequestedHref);

		session->lastRequestedHref = NULL;

	}


	if(!session->is_clone)
	{
		session->lastRequestedHref = bpstrndup(URL, url_len);
	}
	else
	{
		session->lastRequestedHref = tq_strndup(session->m, URL, url_len, BPLN);
	}


	// reset times
	session->request_end_time   = 0;
	session->request_start_time = 0;
	session->req_time_s         = 0;

	// set request start time
	session->request_start_time = time(NULL);

	// code variables
	size_t conn_code  = 0;
	size_t http_code  = 0;

	// If any REQUEST headers are enabled to be set, this is where that happens.
	BP_HTTPApplyAllEnabledRequestHeadersToCurrentRequest(session);

	// destroy the RECIEVED (not request) headers before sending.
	if(session->headers)
	{
		BP_HTTPDestroyParsedHeaders(session->headers);
		session->headers      = NULL;
		session->header_count = 0;
	}


	// destroy memptr if its set
	if(session->page->memPtr)
	{
		bpfree(session->page->memPtr);
		session->page->memPtr = NULL;
	}

	// now make the request
	if(curl_easy_perform(session->curl))
	{

		// check
		if(session->db_populated_proxy_n)
			session->disable_proxy_check_in_http_request = false;

		// exit
		return ERR_FAILURE;

	}
	else
	{

		// get the connect
		curl_easy_getinfo(session->curl, CURLINFO_HTTP_CONNECTCODE, &conn_code);
		session->http_connect_code = conn_code;

		// get the http code
		curl_easy_getinfo(session->curl, CURLINFO_HTTP_CODE, &http_code);
		session->http_status_code = http_code;

	}

	// set request end time
	session->request_end_time = time(NULL);

	// calculate the session run length
	session->req_time_s = session->request_end_time - session->request_start_time;

	// If proxy sets are used, increment to the next proxy (overflow prevention
	// is done during loop iteration.)
	if(session->use_web_proxy_set)
	{
		session->proxy_set->last_proxy_idx++;
	}

	// return failure if there is no page data
	if(session->page == NULL)
	{
		return ERR_FAILURE;
	}

	// set var for header retrieval
	size_t header_size = 0;

	// get size of headers
	curl_easy_getinfo(session->curl, CURLINFO_HEADER_SIZE, &header_size);

	// set header data
	session->page->hdr_data      = session->page->memPtr;
	session->page->hdr_data_size = header_size;

	// set page data (offset in memPtr), and size
	session->page->page_data      = (char *) &session->page->memPtr[header_size];
	session->page->page_data_size = session->page->size - header_size;

	// destroy headers if they've already been sent
	if(session->headers)
	{
		bpfree(session->headers);
		session->headers = NULL;
	}

	// now parse out headers from the existing page
	session->headers = BP_HTTPParseHeaders(session->page);

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Enable Redirects %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// enable 302 redirect following in the provided curl handle
BP_ERROR_T BP_HTTPSetFollowRedirects(P_BP_HTTP_SESSION session, BP_BOOL set_val, size_t max_redirects)
{

	// ensure we have a value to set
	if(!session)
		return ERR_FAILURE;


	// set follow location
	if(set_val)
	{

		// set the follow indicator
		session->httpFollowLocation = BP_TRUE;

		// set max redirect count
		session->httpMaxFollowRedirs = max_redirects;

		// set follow location
		curl_easy_setopt(session->curl, CURLOPT_FOLLOWLOCATION , 1);

		// set redirects
		curl_easy_setopt(session->curl, CURLOPT_MAXREDIRS, session->httpMaxFollowRedirs);

		// return indicating success
		return ERR_SUCCESS;

	}
	else
	{
		// set the follow indicator
		session->httpFollowLocation = BP_FALSE;

		// set max redirect count
		session->httpMaxFollowRedirs = 0;

		// set follow location
		curl_easy_setopt(session->curl, CURLOPT_FOLLOWLOCATION , 0);

		// set redirects
		curl_easy_setopt(session->curl, CURLOPT_MAXREDIRS, 0);

		// return indicating success
		return ERR_SUCCESS;
	}

	// should never really get here
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Session Creation/Deletion/Invocation %%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Creates a new session structure, used by invoke session
P_BP_HTTP_SESSION BP_HTTPCreateSession()
{

	// attempt to allocate space for new session
	P_BP_HTTP_SESSION newSession = (P_BP_HTTP_SESSION) bpcalloc(sizeof(BP_HTTP_SESSION), 1);
	if(!newSession)
		return NULL;

	// create tail queue allocator to handle dynamic allocations
	newSession->m = BP_CreateLinkL("http session alloc", BP_FALSE);

	// allocate data for storing page information
	newSession->page = (P_CURL_READ_WEBPAGE) bpcalloc(sizeof(CURL_READ_WEBPAGE),1);

	// invoke the new session (set relevant data)
	BP_HTTPInvokeSession(newSession);

	// attempt to add all common headers
	BP_HTTPAddAllCommonRequestHeadersToSession(newSession);

	// return the newly filled out session
	return newSession;

}

// HTTP Global Initialization Routine
int BP_HTTPGlobalInit()
{

	curl_global_init(CURL_GLOBAL_ALL);
	return ERR_SUCCESS;

}


// Blockpartys HTTP Interface, starts a session and retrieves a new page, storing
// it in a local buffer.
int BP_HTTPInvokeSession(P_BP_HTTP_SESSION session)
{
	if(!session)
		return ERR_FAILURE;

	if(session->curl)
	{
		curl_easy_cleanup(session->curl);
		session->curl = NULL;
	}

	// zero out memory in the structure (page data)
	memset(session->page, 0x00, sizeof(CURL_READ_WEBPAGE));

	// create a new curl handle
	session->curl = curl_easy_init();

	// set follow location
	if(session->httpFollowLocation)
	{

		// set follow location
		curl_easy_setopt(session->curl, CURLOPT_FOLLOWLOCATION , 1);

		// set max redirects
		if(session->httpMaxFollowRedirs)
			curl_easy_setopt(session->curl, CURLOPT_MAXREDIRS, session->httpMaxFollowRedirs);

	}
	else
		curl_easy_setopt(session->curl, CURLOPT_FOLLOWLOCATION , 0);


	// set ssl peer verification via settings (default = no verify)
	if(session->sslVerifyPeer)
		curl_easy_setopt(session->curl, CURLOPT_SSL_VERIFYPEER, 1);
	else
		curl_easy_setopt(session->curl, CURLOPT_SSL_VERIFYPEER, 0);

	// set ssl host verification via settings (default = no verify)
	if(session->sslVerifyHost)
		curl_easy_setopt(session->curl, CURLOPT_SSL_VERIFYHOST, 1);
	else
		curl_easy_setopt(session->curl, CURLOPT_SSL_VERIFYHOST, 0);

	// set flag to record request headers as well as data
	curl_easy_setopt(session->curl, CURLOPT_HEADER , 1);

	// set the write callback to use when making curl requests
	curl_easy_setopt(session->curl, CURLOPT_WRITEFUNCTION,      BP_GetPageCallback);

	// set the pointer in which data will be written
	curl_easy_setopt(session->curl, CURLOPT_WRITEDATA, (void *) session->page);


	// set the connect timeout if the member exists in the struct
	if(session->http_connect_timeout)
		BP_HTTPSetConnectTimeout(session, session->http_connect_timeout);
	else
		BP_HTTPSetConnectTimeout(session, 3);

	// set the session timeout if it exists
	if(session->http_session_timeout)
		BP_HTTPSetTimeout(session, session->http_session_timeout);
	else
		BP_HTTPSetTimeout(session, 5);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Debug Mechanisms %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// cURL debug functionality provides us with the ability
	// to introspect outbound data, such as the headers we're
	// sending.  This can give us interesting data which we
	// can use for tampering, obfuscating, or just all around
	// general funtimes.  The following automatically provides
	// hooks into this functionality so that we might have it
	// available at the ready.

	// set the session debug callback
	curl_easy_setopt(session->curl, CURLOPT_DEBUGFUNCTION, BP_HTTPDebugCallback);

	// set the user pointer for the debug data callback to the session itself.  This
	// allows us to read and fill header (etc) data into the session easily and effectively.
	curl_easy_setopt(session->curl, CURLOPT_DEBUGDATA, session);

	// This is required to enable the debugging features.  Without this set, debugging
	// will have no effect.
	curl_easy_setopt(session->curl, CURLOPT_VERBOSE, 1L);

	// return indicating success
	return ERR_SUCCESS;

}


// Clones relevant session data, including proxies, settings, and most non
// extant data.  This data is mostly used in threads so that threads can easily
// gather http session settings before invoking their own connections.
P_BP_HTTP_SESSION BP_HTTPCloneSession(P_BP_HTTP_SESSION session)
{

	// ensure we have a session to work with
	if(!session)
		return NULL;

	// attempt to allocate space for new session
	P_BP_HTTP_SESSION new_session = (P_BP_HTTP_SESSION) bpcalloc(sizeof(BP_HTTP_SESSION), 1);
	if(!new_session)
		return NULL;

	// set the clone flag
	new_session->is_clone = BP_TRUE;

	// create tail queue allocator to handle dynamic allocations
	new_session->m = BP_CreateLinkL("http session alloc", BP_FALSE);

	// allocate data for storing page information
	new_session->page = (P_CURL_READ_WEBPAGE) tq_calloc(new_session->m, sizeof(CURL_READ_WEBPAGE),1, BPLN);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Set Basic Parameters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set user/password if necessary
	if(session->userPass)
		new_session->userPass = tq_strdup(new_session->m, session->userPass, BPLN);


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%% Clone Direct Settings %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	new_session->use_request_timing = session->use_request_timing;

	new_session->max_request_retries = session->max_request_retries;

	new_session->request_time_modulo = session->request_time_modulo;

	new_session->request_start_time = session->request_start_time;

	new_session->request_end_time = session->request_end_time;

	new_session->req_time_s = session->req_time_s;

	new_session->http_status_code = session->http_status_code;

	new_session->http_connect_code = session->http_connect_code;

	new_session->http_connect_timeout = session->http_connect_timeout;

	new_session->http_session_timeout = session->http_session_timeout;

	new_session->last_request_was_blacklisted = session->last_request_was_blacklisted;

	new_session->use_db_proxy_set = session->use_db_proxy_set;

	new_session->skip_jit_validation_checks = session->skip_jit_validation_checks;

	new_session->httpAuthNonDefault = session->httpAuthNonDefault;

	new_session->httpFollowLocation = session->httpFollowLocation;

	new_session->httpMaxFollowRedirs = session->httpMaxFollowRedirs;

	new_session->isHttpPut = session->isHttpPut;

	new_session->fileName = session->fileName;

	new_session->fileSize = session->fileSize;

	new_session->isHttpGet = session->isHttpGet;

	new_session->isHttpPost = session->isHttpPost;

	if(session->httpPostData)
		new_session->httpPostData = tq_strdup(new_session->m, session->httpPostData, BPLN);

	new_session->interfaceIsSet = session->interfaceIsSet;

	if(session->interface)
		new_session->interface = tq_strdup(new_session->m, session->interface, BPLN);

	new_session->netrcIsSet = session->netrcIsSet;

	if(session->netrcFile)
		new_session->netrcFile = tq_strdup(new_session->m, session->netrcFile, BPLN);

	new_session->sslVerifyPeer = session->sslVerifyPeer;

	new_session->sslVerifyHost = session->sslVerifyHost;

	new_session->cookieIsSet = session->cookieIsSet;

	new_session->useCookieFromFile = session->useCookieFromFile;

	if(session->cookie)
		new_session->cookie = tq_strdup(new_session->m, session->cookie, BPLN);

	if(session->cookieFromFile)
		new_session->cookieFromFile = tq_strdup(new_session->m, session->cookieFromFile, BPLN);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%% Clone Complex Datatypes %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Clone Regular Expression Blacklists %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(session->request_regexp_blacklist_n)
	{

		// allocate space for new blacklist
		new_session->request_regexp_blacklist = (char **) tq_calloc
		(
				new_session->m,
				sizeof(char *) * (session->request_regexp_blacklist_n + 1),
				1,
				BPLN
		);

		// set the blacklist count directly
		new_session->request_regexp_blacklist_n = session->request_regexp_blacklist_n;

		// walk blacklists
		size_t blacklist_iter = 0;
		for(; blacklist_iter < session->request_regexp_blacklist_n; blacklist_iter++)
		{

			// set the item from the array
			new_session->request_regexp_blacklist[blacklist_iter] = tq_strdup(new_session->m, session->request_regexp_blacklist[blacklist_iter], BPLN);

		}

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Clone Proxy Sets %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(session->db_populated_proxy_group_n)
	{

		// set the count
		new_session->db_populated_proxy_group_n = session->db_populated_proxy_group_n;

		// allocate space
		new_session->db_populated_proxy_groups = (P_BP_PROXY_DB_GROUP) tq_calloc
		(
			new_session->m,
			session->db_populated_proxy_group_n * sizeof(BP_PROXY_DB_GROUP),
			1,
			BPLN
		);

		// proxy db groups are static structures with static buffers
		// and as such can be copied directly.
		memcpy
		(
				new_session->db_populated_proxy_groups,
				session->db_populated_proxy_groups,
				session->db_populated_proxy_group_n * sizeof(BP_PROXY_DB_GROUP)
		);

	}

	if(session->db_populated_proxy_n)
	{

		// set the count
		new_session->db_populated_proxy_n = session->db_populated_proxy_n;

		// allocate space
		new_session->db_populated_proxy_sets = (P_BP_PROXY_DB_ENTRY) tq_calloc
		(
			new_session->m,
			session->db_populated_proxy_n * sizeof(BP_PROXY_DB_ENTRY),
			1,
			BPLN
		);

		// proxy db groups are static structures with static buffers
		// and as such can be copied directly.
		memcpy
		(
				new_session->db_populated_proxy_sets,
				session->db_populated_proxy_sets,
				session->db_populated_proxy_n * sizeof(BP_PROXY_DB_ENTRY)
		);

	}

	if(session->db_populated_proxy_validation_group_n)
	{

		// set the count
		new_session->db_populated_proxy_validation_group_n = session->db_populated_proxy_validation_group_n;

		// allocate space
		new_session->db_populated_proxy_validation_groups = (P_BP_PROXY_DB_VALIDATION_TEST_GROUP) tq_calloc
		(
			new_session->m,
			session->db_populated_proxy_validation_group_n * sizeof(BP_PROXY_DB_VALIDATION_TEST_GROUP),
			1,
			BPLN
		);

		// proxy db groups are static structures with static buffers
		// and as such can be copied directly.
		memcpy
		(
				new_session->db_populated_proxy_validation_groups,
				session->db_populated_proxy_validation_groups,
				session->db_populated_proxy_validation_group_n * sizeof(BP_PROXY_DB_VALIDATION_TEST_GROUP)
		);

	}


	if(session->db_populated_proxy_validation_test_n)
	{

		// set the count
		new_session->db_populated_proxy_validation_test_n = session->db_populated_proxy_validation_test_n;

		// allocate space
		new_session->db_populated_proxy_validation_tests = (P_BP_PROXY_DB_VALIDATION_TEST_ENTRY) tq_calloc
		(
			new_session->m,
			session->db_populated_proxy_validation_test_n * sizeof(BP_PROXY_DB_VALIDATION_TEST_ENTRY),
			1,
			BPLN
		);

		// proxy db groups are static structures with static buffers
		// and as such can be copied directly.
		memcpy
		(
				new_session->db_populated_proxy_validation_tests,
				session->db_populated_proxy_validation_tests,
				session->db_populated_proxy_validation_test_n * sizeof(BP_PROXY_DB_VALIDATION_TEST_ENTRY)
		);

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Clone Request Headers %%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(session->request_headers_n)
	{

		// set the header count
		new_session->request_headers_n = session->request_headers_n;

		// set the headers
		new_session->request_headers = (P_BP_HTTP_REQUEST_HEADER) tq_calloc
		(
				new_session->m,
				sizeof(BP_HTTP_REQUEST_HEADER) * new_session->request_headers_n,
				1,
				BPLN
		);

		size_t request_header_iter = 0;
		for
		(
				;
				request_header_iter < new_session->request_headers_n;
				request_header_iter++
		)
		{

			// header send indicator
			new_session->request_headers[request_header_iter].send_header = session->request_headers[request_header_iter].send_header;

			new_session->request_headers[request_header_iter].field_name = tq_strdup
			(
					new_session->m,
					session->request_headers[request_header_iter].field_name,
					BPLN
			);

			new_session->request_headers[request_header_iter].field_description = tq_strdup
			(
					new_session->m,
					session->request_headers[request_header_iter].field_description,
					BPLN
			);

			new_session->request_headers[request_header_iter].value = tq_strdup
			(
					new_session->m,
					session->request_headers[request_header_iter].value,
					BPLN
			);

		}


	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Recieved Headers From Server %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(session->header_count)
	{

		// set header count
		new_session->header_count = session->header_count;

		// allocate space for header info structure
		new_session->headers = (P_BP_HTTP_PARSED_HEADER_INFO) tq_calloc
		(
				new_session->m,
				sizeof(BP_HTTP_PARSED_HEADER_INFO),
				1,
				BPLN
		);

		size_t parsed_header_iter = 0;

		// allocate space for individual headers
		new_session->headers->headers = (P_BP_HTTP_PARSED_HEADER) tq_calloc
		(
				new_session->m,
				sizeof(BP_HTTP_PARSED_HEADER) * new_session->header_count,
				1,
				BPLN
		);

		// set the header count
		new_session->headers->header_count = session->headers->header_count;

		// header pointer for readability
		P_BP_HTTP_PARSED_HEADER new_header = NULL;
		P_BP_HTTP_PARSED_HEADER old_header = NULL;

		// now set header items
		for
		(
				;
				parsed_header_iter < new_session->headers->header_count;
				parsed_header_iter++
		)
		{

			// set the old header
			old_header = &session->headers->headers[parsed_header_iter];

			// set the new header
			new_header = &new_session->headers->headers[parsed_header_iter];

			// set strings
			new_header->header_index = tq_strdup(new_session->m, old_header->header_index, BPLN);
			new_header->value        = tq_strdup(new_session->m, old_header->value, BPLN);
			new_header->unparsed     = tq_strdup(new_session->m, old_header->unparsed, BPLN);

			// set lengths
			new_header->index_len = old_header->index_len;
			new_header->value_len = old_header->value_len;
			new_header->unparsed_len = old_header->unparsed_len;

		}


	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Invoke Session %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// invoke the new session (set relevant data)
	BP_HTTPInvokeSession(new_session);




	// return the newly cloned session
	return new_session;

}




// This is a prototyped function (from cURL documentation) to be used for the purposes
// of collecting cURL debug information outbound.
static int BP_HTTPDebugCallback(CURL *handle, curl_infotype type, char *data, size_t size, void * userp)
{

	// set the session from the user pointer
	P_BP_HTTP_SESSION session = (P_BP_HTTP_SESSION) userp;
	char * new_data = (char *) bpcalloc(size+10, 1);

	switch(type)
	{
		case CURLINFO_HEADER_OUT:

			memcpy(new_data, data, size);
			printf("\n [DBG REQUEST HEADERS]: \n%s", new_data);

			break;
		case CURLINFO_DATA_OUT:
			break;
		case CURLINFO_SSL_DATA_OUT:
			break;
		case CURLINFO_HEADER_IN:
			// printf("\n IN_HDR: [%s]", data);
			break;
		case CURLINFO_DATA_IN:
			break;
		case CURLINFO_SSL_DATA_IN:
			break;
		default:
			break;
	}
	bpfree(new_data);

	// function calls for a zero return status on success
	return 0;

}


// set the connect timeout
BP_ERROR_T BP_HTTPSetConnectTimeout(P_BP_HTTP_SESSION session, size_t seconds)
{

	// make sure we have a valid session
	if(!session)
		return ERR_FAILURE;

	// set the curl timeout
	curl_easy_setopt(session->curl, CURLOPT_CONNECTTIMEOUT, seconds);

	// return indicating success
	return ERR_SUCCESS;

}

// Set timeout in curl
int BP_HTTPSetTimeout(P_BP_HTTP_SESSION session, size_t seconds)
{

	// make sure we have a valid session
	if(!session)
		return ERR_FAILURE;

	// set the curl timeout
	curl_easy_setopt(session->curl, CURLOPT_TIMEOUT, seconds);

	// return indicating success
	return ERR_SUCCESS;

}



// ------------ Proxy Setters/Validators/Randomizers -------------

// Sets a proxy
int BP_HTTPSetProxy(P_BP_HTTP_SESSION session, char *proxy)
{

	// ensure we have a session and a proxy
	if(!session || !proxy)
		return ERR_FAILURE;

	// printf("\n Setting Proxy: %s", proxy);

	// set curl option
	curl_easy_setopt(session->curl, CURLOPT_PROXY, proxy);

	// return indicating success
	return ERR_SUCCESS;
}

// Set proxy user:pass string
int BP_HTTPSetProxyUserPass(P_BP_HTTP_SESSION session, char *userpass)
{

	if(!session || !userpass)
		return ERR_FAILURE;

	curl_easy_setopt(session->curl, CURLOPT_PROXYUSERPWD, userpass);

	return ERR_SUCCESS;
}

// Sets a proxy teype
int BP_HTTPSetProxyType(P_BP_HTTP_SESSION session, int proxyType)
{

	if(!session)
		return ERR_FAILURE;

	curl_easy_setopt(session->curl, CURLOPT_PROXYTYPE, proxyType);

	return ERR_SUCCESS;
}


// Validates a proxy by requesting a validation URL
int BP_HTTPValidateCurrentProxy(P_BP_HTTP_SESSION session, char *validateURL, char *successStr)
{

	// check if we have valid pointers coming in
	if(!session || !validateURL || !successStr)
		return ERR_FAILURE;

	// declare/initialize test
	char *test = NULL;



	// make a request
	if(BP_HTTPRequest(session, validateURL) == ERR_SUCCESS)
	{

		if(!session->page->memPtr)
			return ERR_FAILURE;

		// run simple memory matching test against page content
		test = (char *) memmem(session->page->memPtr, session->page->size, successStr, bpstrlen(successStr));

		// make sure we encounter the correct test string
		if(!test)
			return ERR_FAILURE;


	}
	else
	{

		// return indicating failure in the case that we cannot make
		// the request.
		return ERR_FAILURE;

	}

	// if we made a match, and made a successful request, return indicating success
	return ERR_SUCCESS;

}

// Attempts to determine (by BRUTE FORCE) what type of proxy a given connect string alludes to.
// It will return either ERR_FAILURE, or CURLPROXY_(SOCKS4/SOCKS5/HTTP).  Can use any connect string
// usable by curl.  Requires a HTTP session in order to work.
size_t BP_HTTPValidateProxyTypeByBruteForce(P_BP_HTTP_SESSION session, char *connect_string, char *validateURL, char *successStr, size_t http_timeout)
{

	// check to ensure we have valid pointers before continuing
	if(!session)
		return ERR_FAILURE;
	if(!connect_string)
		return ERR_FAILURE;
	if(!validateURL)
		return ERR_FAILURE;
	if(!successStr)
		return ERR_FAILURE;

	// type iterator used for trying each proxy type (via case)
	size_t type_iter = 0;

	// flag to indicate when to exit the do{} loop below
	size_t exit_loop = 0;


	do
	{


		//  Set proxy connect string.
		curl_easy_setopt(session->curl, CURLOPT_PROXY, connect_string);

		// set http timeout
		BP_HTTPSetTimeout(session, http_timeout);

		// set only a specific proxy type per iter
		switch(type_iter)
		{

			// CURLPROXY_SOCKS5_HOSTNAME
			case 0:
				BP_HTTPSetProxyType(session, CURLPROXY_SOCKS5_HOSTNAME);
				// Now validate proxy.
				if(BP_HTTPValidateCurrentProxy(session, validateURL, successStr) == ERR_SUCCESS)
				{
					return CURLPROXY_SOCKS5_HOSTNAME;
				}
				break;

			// CURLPROXY_SOCKS4A
			case 1:
				// printf("\n Testing 1");
				BP_HTTPSetProxyType(session, CURLPROXY_SOCKS4A);
				// Now validate proxy.
				if(BP_HTTPValidateCurrentProxy(session, validateURL, successStr) == ERR_SUCCESS)
				{
					return CURLPROXY_SOCKS4A;
				}
				break;

			// CURLPROXY_SOCKS5
			case 2:
				// printf("\n Testing 2");
				BP_HTTPSetProxyType(session, CURLPROXY_SOCKS5);
				// Now validate proxy.
				if(BP_HTTPValidateCurrentProxy(session, validateURL, successStr) == ERR_SUCCESS)
				{
					return CURLPROXY_SOCKS5;
				}
				break;

			// CURLPROXY_SOCKS4
			case 3:
				// printf("\n Testing 3");
				BP_HTTPSetProxyType(session, CURLPROXY_SOCKS4);
				// Now validate proxy.
				if(BP_HTTPValidateCurrentProxy(session, validateURL, successStr) == ERR_SUCCESS)
				{
					return CURLPROXY_SOCKS4;
				}
				break;

			// CURLPROXY_HTTP
			case 4:
				// printf("\n Testing 4");
				BP_HTTPSetProxyType(session, CURLPROXY_HTTP);
				// Now validate proxy.
				if(BP_HTTPValidateCurrentProxy(session, validateURL, successStr) == ERR_SUCCESS)
				{
					return CURLPROXY_HTTP;
				}
				break;

			// exit the loop here
			default:
				exit_loop=1;
				break;

		}

		// increment the type iter
		type_iter++;

	} while (!exit_loop);


	// return indicating failure if no proxy was matched
	return ERR_FAILURE;

}


// This version of the above validation function (HTTPValidateProxyTypeByBruteForce) is used for validating
// proxies in a threaded model.  It can be called by pthread_create or the native Block() class bindings in order
// to attempt to validate a proxy.
void * BP_HTTPValidateProxyTypeByBruteForce_thinit(/* P_BP_DB_VALIDATION_THREAD_PARMS */ void *validation_parms)
{


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Gather Parameters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	// set the managed thread data
	P_BP_MANAGED_THREAD managed_thread = (P_BP_MANAGED_THREAD) validation_parms;

	// lock the thread
	BP_MANAGED_THREAD_LOCK;

	// mark thread as running while the managed thread is locked.
	managed_thread->current_status = BP_THREAD_STATUS_RUNNING;

	BP_MANAGED_THREAD_UNLOCK;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// ==== Thread Has Been Marked As Running, Begin Execution ======
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// printf("\n [thread: %u] Woo Gets Here!!!", pthread_self());

	// set the proxy to test from the thread data
	P_BP_DB_VALIDATION_THREAD_PARMS parms = (P_BP_DB_VALIDATION_THREAD_PARMS) managed_thread->thread_parameter;

	// exit immediately if we have no parameters
	if(!parms)
		return NULL;

	// cant validate anything without validation set
	if(!parms->test_entry_n)
	{
		printf("\n [!!] Database proxy validation requires at least 1 validation test set.  Currently, there are none. (0)");
		return NULL;
	}


	// create new http session
	P_BP_HTTP_SESSION httpSession = BP_HTTPCreateSession();

	// call global init (does nothing if already called)
	BP_HTTPGlobalInit();

	// since this is assumed to be a threaded venture, disable signals
	curl_easy_setopt(httpSession->curl, CURLOPT_NOSIGNAL, 1);

	// HTTPUserAgent(httpSession, agent);
	// HTTPSetUserPass(httpSession, user, pass);
	// HTTPSetPostVars(httpSession, postStr);
	// HTTPSetCookie(httpSession, cookie);
	// HTTPRequest(httpSession, url);
	// HTTPSetProxy(httpSession, proxy_str);
	// HTTPSetProxyViaProxyDBEntryRecord(httpSession, parms->proxy_entry);

	// BP_GenRandomASCIICharString(32, BP_GE)

	// set the timeout to 5 seconds
	BP_HTTPSetTimeout(httpSession, parms->timeout);

	// loop and attempt to validate proxy using randomly chosen tests
	size_t n = 0;

	// flag which is set upon a successful proxy validation
	size_t proxy_validated = 0;

	// a random index (set below) used for determining which test to attempt
	size_t random_index = 0;

	// try a variety of attempts
	for(; n < parms->test_attempt_before_failing_validation; n++)
	{

		// generate a random index (from /dev/urandom)
		if(parms->test_entry_n-1 != 0 )
			random_index = BP_GenRandomUnsignedLong(0, parms->test_entry_n-1);

		// attempt to validate proxy
		if(BP_HTTPValidateProxyViaProxyDBTestRecord(httpSession, parms->proxy_entry, &parms->test_entries[random_index], parms->brute_force_proxy_type) == ERR_SUCCESS)
		{
			proxy_validated = 1;
			break;
		}

	}

	// if the proxy was validated, go ahead and mark it
	if(proxy_validated)
	{
		parms->proxy_entry->proxy_has_been_validated_at_runtime = true;
	}

	// destroy http session before exiting thread (cleanup)
	BP_HTTPDestroySession(httpSession);

	// set proxy test
	// HTTPDisplayProxyDBEntryRecord(parms->proxy_entry);



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%% Exit Thread / Decoupling %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the status as exited
	BP_MANAGED_THREAD_LOCK;
	managed_thread->current_status = BP_THREAD_STATUS_EXITED;
	BP_MANAGED_THREAD_UNLOCK;

	// actually exit thread
	return NULL;


}

// Attempts to validate a proxy via a proxydb validation test record.
BP_ERROR_T BP_HTTPValidateProxyViaProxyDBTestRecord(P_BP_HTTP_SESSION session, P_BP_PROXY_DB_ENTRY proxy, P_BP_PROXY_DB_VALIDATION_TEST_ENTRY test, size_t brute_force_proxy_type)
{

	// simple pointer check before continuing
	if(!session || !proxy || !test)
		return ERR_FAILURE;


	// take user request from argv[1]
	 BP_ANCHOR_PROTO target_type = HTMLGetAnchorProtocol(test->url_fetch, strnlen(test->url_fetch, 1023));

	// check to make sure the target type is http or https
	if(target_type != BP_ANCHOR_PROTO_HTTP && target_type != BP_ANCHOR_PROTO_HTTPS)
	{
		printf("\n [!!] Validation test contained invalid URL argument (%s)", test->url_fetch);
	    return ERR_FAILURE;
	}

	// set values based on parameters
	test->count_invoked++;

	// set post data if required
	if(strnlen(test->post_args, 1024))
		BP_HTTPSetPostVars(session, (char *) &test->post_args);

	// set user agent strings (allocated, must be destroyed)
	char ** browserUserAgentStrings = BP_GenerateBrowserAgentOnlyArray();

	// pick user agent string
	size_t browser_index = BP_GenRandomUnsignedLong(0, BP_HTTPUserAgentCount(browserUserAgentStrings) - 1);

	// allocate user agent string from global browser strings array (found in library)
	char *ua = (char *) bpstrndup(browserUserAgentStrings[browser_index], 256);

	// destroy the pointer array now that we no longer need it
	BP_DestroyPointerArray((void **) browserUserAgentStrings, BP_HTTPUserAgentCount(browserUserAgentStrings));

	// now destroy the structure itself
	bpfree(browserUserAgentStrings);

	// set the user agent (random browser string) before request
	BP_HTTPUserAgent(session, ua);

	printf("\n [+] Attempting to validate: %s:%zu", proxy->host, proxy->port);

	// attempt to brute force proxy type if set to do so
	if(brute_force_proxy_type)
	{


		// test to see if its a socks5 hostname proxy (this is ideal, as the proxy does name resolution)
		BP_HTTPSetProxyViaProxyDBEntryRecord(session, proxy);
		BP_HTTPSetProxyType(session, CURLPROXY_SOCKS5_HOSTNAME);
		if(BP_HTTPValidateCurrentProxy(session, test->url_fetch, test->regexp_match) == ERR_SUCCESS)
		{
			bpfree(ua);
			proxy->proxy_type = CURLPROXY_SOCKS5_HOSTNAME;
			printf("\n [+] Proxy validated: %s is CURLPROXY_SOCKS5_HOSTNAME", proxy->host);
			return ERR_SUCCESS;
		}
		// printf("\n Proxy: %s", proxy->host);
		// test to see if its a socks5 proxy
		BP_HTTPSetProxyViaProxyDBEntryRecord(session, proxy);
		BP_HTTPSetProxyType(session, CURLPROXY_SOCKS5);
		if(BP_HTTPValidateCurrentProxy(session, test->url_fetch, test->regexp_match) == ERR_SUCCESS)
		{
			bpfree(ua);
			printf("\n [+] Proxy validated: %s is CURLPROXY_SOCKS5", proxy->host);
			proxy->proxy_type = CURLPROXY_SOCKS5;
			return ERR_SUCCESS;
		}

		// printf("\n Proxy2: %s", proxy->host);
		// test to see if it's an http proxy
		BP_HTTPSetProxyViaProxyDBEntryRecord(session, proxy);
		BP_HTTPSetProxyType(session, CURLPROXY_HTTP);
		if(BP_HTTPValidateCurrentProxy(session, test->url_fetch, test->regexp_match) == ERR_SUCCESS)
		{
			bpfree(ua);
			proxy->proxy_type = CURLPROXY_HTTP;
			printf("\n [+] Proxy validated: %s is CURLPROXY_HTTP", proxy->host);
			return ERR_SUCCESS;
		}

		// printf("\n Proxy3: %s", proxy->host);
		// test to see if its a socks4 proxy
		BP_HTTPSetProxyViaProxyDBEntryRecord(session, proxy);
		BP_HTTPSetProxyType(session, CURLPROXY_SOCKS4);
		if(BP_HTTPValidateCurrentProxy(session, test->url_fetch, test->regexp_match) == ERR_SUCCESS)
		{
			bpfree(ua);
			printf("\n [+] Proxy validated: %s is CURLPROXY_SOCKS4", proxy->host);
			proxy->proxy_type = CURLPROXY_SOCKS4;
			return ERR_SUCCESS;
		}

		// test to see if it's a http 1.0 proxy
		// HTTPSetProxyType(session, CURLPROXY_HTTP_1_0);
		// if(HTTPValidateCurrentProxy(session, test->url_fetch, test->regexp_match) == ERR_SUCCESS)
		//	return ERR_SUCCESS;
		// printf("\n Proxy4: %s", proxy->host);
		// test to see if it's a socks4a proxy
		BP_HTTPSetProxyViaProxyDBEntryRecord(session, proxy);
		BP_HTTPSetProxyType(session, CURLPROXY_SOCKS4A);
		if(BP_HTTPValidateCurrentProxy(session, test->url_fetch, test->regexp_match) == ERR_SUCCESS)
		{
			bpfree(ua);
			proxy->proxy_type = CURLPROXY_SOCKS4A;
			printf("\n [+] Proxy validated: %s is CURLPROXY_SOCKS4A", proxy->host);
			return ERR_SUCCESS;
		}

		bpfree(ua);

		// since we were unable to match the proxy type, set the type as unknown.
		proxy->proxy_type = BP_PROXY_TYPE_UNKNOWN_NONWORKING;
		// printf("\n here failed!");
		// exit here in the case that we were unable to verify the proxy type
		return ERR_FAILURE;

	}
	else
	{

		// only attempt to set the proxy type if it's not 0xffffffff
		if(proxy->proxy_type != (size_t) BP_PROXY_TYPE_UNKNOWN_NONWORKING)
			BP_HTTPSetProxyType(session, proxy->proxy_type);

		// set the proxy in the class
		if(BP_HTTPSetProxyViaProxyDBEntryRecord(session, proxy) != ERR_SUCCESS)
		{
			bpfree(ua);
			printf("\n [!!] unable to set proxy before validation attempt.  Bad proxy element most likely.");
			return ERR_FAILURE;
		}

		// attempt to validate the proxy now
		if(BP_HTTPValidateCurrentProxy(session, test->url_fetch, test->regexp_match) != ERR_SUCCESS)
		{
			bpfree(ua);
			return ERR_FAILURE;
		}

		printf("\n [+] Proxy validated: %s is %u ", proxy->host, proxy->proxy_type);

	}
	bpfree(ua);
	// return indicating success
	return ERR_SUCCESS;

}

// This routine attempts to set the proxy for the session using only a proxy DB entry structure.
BP_ERROR_T BP_HTTPSetProxyViaProxyDBEntryRecord(P_BP_HTTP_SESSION session, P_BP_PROXY_DB_ENTRY record)
{


	// ensure we have a session/record pointers
	if(!session)
		return ERR_FAILURE;
	if(!record)
		return ERR_FAILURE;

	// CURL PROXY FORMAT:
	//  -- http://user:pass@host:port
	// note: the prefix is ignored (as per the documentation)

	// calculate the size of the new buffer
	size_t proxy_string_len = 0;

	// buffer should be the length of all the elements used to compose the string
	proxy_string_len += strnlen(record->username, sizeof(record->username)-1);
	proxy_string_len += strnlen(record->password, sizeof(record->password)-1);
	proxy_string_len += strnlen(record->host,     sizeof(record->host)-1);
	proxy_string_len += 16; // strnlen(record->port,     sizeof(record->port)-1);

	// add some extra space for compositing data
	proxy_string_len += 32;

	// allocate the string
	char * proxy_string = (char *) bpcalloc(proxy_string_len, 1);


	// if a username / password are set, add them here
	if(strnlen(record->username, sizeof(record->username) - 1))
	{

		// add the record elements
		snprintf(proxy_string, proxy_string_len-1, "%s:%s@%s:%u", record->username, record->password, record->host, record->port);

	}
	else
	{

		// add the record w/o user/pass parameters.
		snprintf(proxy_string, proxy_string_len-1, "%s:%u", record->host, record->port);

	}

	// set the proxy here
	if(BP_HTTPSetProxy(session, proxy_string) != ERR_SUCCESS)
	{
		bpfree(proxy_string);
		return ERR_FAILURE;
	}

	// destroy the proxy string, as it's been copied by the internal curl routines
	bpfree(proxy_string);

	// return indicating success
	return ERR_SUCCESS;

}



// Generates a nullptr terminated char ** array of proxies from a file
char ** BP_HTTPGenerateProxyListFromFile(char *filePath){

	// pointer array which will be returned
	char **proxyArray = NULL;

	// create stack buffer to hold data
	char strBuff[65535];
	memset(&strBuff, 0x00, sizeof(strBuff));

	FILE * fPtr = fopen(filePath, "r");
	if(!fPtr)
		return NULL;

	// read a line from the file
	size_t x = 0;
	for(x = 0; fgets(strBuff, 65533, fPtr); x++){

		strBuff[bpstrlen(strBuff)-1] = 0x00;

		// bpreallocate pointers
		proxyArray = (char **) bprealloc(proxyArray, (x+1) * sizeof(char *));

		// duplicate extracted string
		proxyArray[x] = bpstrndup(strBuff, 2048);
		if(!proxyArray[x]){
			fclose(fPtr);
			return NULL;
		}

		// zero out buffer before reset
		memset(strBuff, 0x00, sizeof(strBuff));
	}

	// terminate the array
	x++;
	proxyArray = (char **) bprealloc(proxyArray, (x+1) * sizeof(char *));
	proxyArray[x] = NULL;

	// close the file
	fclose(fPtr);

	return proxyArray;
}

// Set Proxies in Session and assigns count
int BP_HTTPSetProxyArray(P_BP_HTTP_SESSION session, char **proxies){

	if(!session || !proxies)
		return ERR_FAILURE;

	// set proxy count
	session->proxies = proxies;
	session->proxyCount = BP_HTTPGetProxyCount(proxies);

	return ERR_SUCCESS;
}


// Gets a count from the char ** generated above
size_t BP_HTTPGetProxyCount(char **proxyArray){

	if(!proxyArray)
		return 0;

	// count array elements
	size_t x = 0;
	for(x = 0; proxyArray[x]; x++){}

	// return full count
	return x;
}


// Allocates a new character buffer which takes an anchor URL which
// contains a script name or extraenous data, and removes the data
// until it finds a precursory /.

// eg. /test/something.php will be returned in a new buffer as /test/

// Fails if no previous / is found in which case NULL is returned.
char * BP_HTTPStripScript(char *url, size_t len){
	if(!url)
		return NULL;


	// precursory / check
	if(!memmem(url, len, "/", 1))
		return NULL;

	// allocate buffer and fill with data
	char *fixed = (char *) bpcalloc(len, 1);
		memcpy(fixed, url, len);

	size_t i = len;
	for(; i > 0; i--){

		// check for our base
		if(fixed[i-1] == '/')
			break;

		// nullify character code
		fixed[i-1] = 0x00;
	}


	// return the fixed buffer
	return fixed;
}



// Display proxy record (non-class oriented version)
BP_ERROR_T BP_HTTPDisplayProxyDBEntryRecord(P_BP_PROXY_DB_ENTRY record)
{

	if(!record)
		return ERR_FAILURE;

	// display record
	printf("\n\t [+] DB Proxy Entry %p", record);
	printf("\n\t\t id:                %u", record->id);
	printf("\n\t\t user_id:           %u", record->user_id);
	printf("\n\t\t group_id:          %u", record->group_id);
	printf("\n\t\t proxy_type:        %u", record->proxy_type);
	printf("\n\t\t host:              %s", record->host);
	printf("\n\t\t port:              %u", record->port);
	printf("\n\t\t username:          %s", record->username);
	printf("\n\t\t password:          %s", record->password);
	printf("\n\t\t user_notes:        %s", record->user_notes);
	printf("\n\t\t created_ts:        %s", record->created_ts);
	printf("\n\t\t last_validated_ts: %s", record->last_validated_ts);
	printf("\n\t\t runtime validated: %s", record->proxy_has_been_validated_at_runtime ? "yes" : "no");

	// exit indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%% CURL VALUE GETTRS %%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Gets a null terminated ascii string containing the ip address of the
// last connection made by curl.
char * BP_HTTPGetRemoteLastIP(P_BP_HTTP_SESSION session)
{

	if(!session)
		return NULL;
	if(!session->curl)
		return NULL;

	// allocate space for remote IP
	char * remote_ip = (char *) bpcalloc(1024, 1);

	// make information request
	curl_easy_getinfo( session->curl, CURLINFO_PRIMARY_IP , &remote_ip);

	// return remote ip
	return remote_ip;

}

// Display a Page Structure
int BP_HTTPDisplayWebpageStructure(P_CURL_READ_WEBPAGE data, size_t display_content)
{
	printf("\n [+] Displaying P_CURL_READ_WEBPAGE structure at %p", data);
	printf("\n     - memPtr:           %p", data->memPtr);
	printf("\n     - size:             %u", data->size);
	printf("\n     - header_data:      %p", data->hdr_data);
	printf("\n     - header_data_size: %u", data->hdr_data_size);
	printf("\n     - page_data:        %p", data->page_data);
	printf("\n     - page_data_size:   %u", data->page_data_size);
	printf("\n ");

	//display headers/content on demand
	if(display_content)
	{

		// extract header data raw
		char *tmp_buff = (char *) bpcalloc(data->hdr_data_size+1, 1);
		memcpy(tmp_buff, data->hdr_data, data->hdr_data_size);

		// display data
		printf("\n [+] Http Header Data Recieved\n");
		printf("\n -------------------------------\n\n");
		printf("%s", tmp_buff);
		printf("\n -------------------------------\n");

		// display the rest
		printf("\n [+] Http Body Data Recieved\n");
		printf("\n -------------------------------\n\n");
		printf("\n%s", data->page_data);
		printf("\n -------------------------------\n\n");

	}


	return ERR_SUCCESS;
}

// ============= Http Utilities for Submitting Forms ========================

// this allows you to submit a form that may have been retrieved via a parse attempt
BP_ERROR_T BP_submitForm(P_BP_HTML_FORM_LIST form)
{

	// return indicating success
	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% HTTP Request Headers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// actually apply all enabled headers which the user has set, to the cURL object
// within the session.  This is called automatically within BP_HTTPRequest()
// before any request is performed, so don't call it directly unless
// you know what you're doing.
BP_ERROR_T BP_HTTPApplyAllEnabledRequestHeadersToCurrentRequest(P_BP_HTTP_SESSION session)
{

	// ensure we have a request header count
	if(!session->request_headers_n)
		return ERR_FAILURE;

	// ensure we have request headers
	if(!session->request_headers)
		return ERR_FAILURE;

	// set option as null first
	// curl_easy_setopt(session->curl, CURLOPT_HEADER, NULL);

	// destroy slist here if necessary
	if(session->request_header_chunk_list)
	{
		curl_slist_free_all(session->request_header_chunk_list);
		session->request_header_chunk_list = NULL;
	}



	// join string array
	char *join_strs[4];

	// the joined string
	char * join_str;

	// walk all the request headers and add them to slist
	size_t n = 0;
	for(; n < session->request_headers_n; n++)
	{

		// ensure we have the send header flag set
		if(session->request_headers[n].send_header != BP_TRUE)
			continue;

		// zero out the joiner
		BP_ZERO_STACK(join_strs);

		// set the join strings in place
		join_strs[0] = session->request_headers[n].field_name;
		join_strs[1] = ": ";
		join_strs[2] = session->request_headers[n].value;

		// join em
		join_str = BP_JoinStringArray((char **) &join_strs, 3);

		// add the new header to the list
		session->request_header_chunk_list = curl_slist_append(session->request_header_chunk_list, join_str);

		// destroy the joined string on loop
		bpfree(join_str);

	}

	// set the header in the curl right here
	if(session->request_header_chunk_list)
	{
		curl_easy_setopt(session->curl, CURLOPT_HTTPHEADER, session->request_header_chunk_list);
	}

	// return indicating success
	return ERR_SUCCESS;

}


// Adds all common request headers to the current HTTP session.
BP_ERROR_T BP_HTTPAddAllCommonRequestHeadersToSession(P_BP_HTTP_SESSION session)
{

	// ensure we have a session
	if(!session)
		return ERR_FAILURE;

	// method requires a valid tail queue allocator
	if(!session->m)
		return ERR_FAILURE;

	// calculate the total number of entries in the common set
	size_t total_entries_in_common_set = sizeof(BP_HTTP_COMMON_REQUEST_HEADERS) / sizeof(BP_HTTP_REQUEST_HEADER);

	// copy in the data via the built in methods
	size_t n = 0;
	for(; n < total_entries_in_common_set; n++)
	{

		// add the request header at the current position
		BP_HTTPAddRequestHeader
		(
			session,
			BP_HTTP_COMMON_REQUEST_HEADERS[n].field_name,
			BP_HTTP_COMMON_REQUEST_HEADERS[n].field_description,
			BP_HTTP_COMMON_REQUEST_HEADERS[n].value,
			BP_HTTP_COMMON_REQUEST_HEADERS[n].send_header
		);

	}

	// return indicating success
	return ERR_SUCCESS;

}

// Adds a request header to the provided http session.
BP_ERROR_T BP_HTTPAddRequestHeader(P_BP_HTTP_SESSION session, char * field_name, char *field_description, char *value, BP_BOOL enabled_for_send)
{

	// ensure we have a session
	if(!session)
		return ERR_FAILURE;

	// method requires a valid tail queue allocator
	if(!session->m)
		return ERR_FAILURE;

	// allocate space for the new header
	session->request_headers = (P_BP_HTTP_REQUEST_HEADER) tq_realloc
	(
		session->m,
		session->request_headers,
		(session->request_headers_n+1) * sizeof(BP_HTTP_REQUEST_HEADER),
		BPLN
	);

	// if allocation failed, exit, obviously
	if(!session->request_headers)
		return ERR_FAILURE;

	// create a reference to increase readability
	P_BP_HTTP_REQUEST_HEADER header = &session->request_headers[session->request_headers_n];

	// set description, name and value if they were provided
	if(field_description)
		header->field_description = tq_strdup(session->m, field_description, BPLN);
	if(field_name)
		header->field_name        = tq_strdup(session->m, field_name, BPLN);
	if(value)
		header->value             = tq_strdup(session->m, value, BPLN);

	// set the header send status indicator
	header->send_header       = enabled_for_send;

	// after successful allocation, increment header count to indicate
	// the new addition.
	session->request_headers_n++;

	// return indicating success
	return ERR_SUCCESS;

}

// Searches for request headers in the http session that match a specified
// criteria.  Set enabled to BP_INVALID_UNSET to omit searching the enabled field.
BP_ERROR_T BP_HTTPSearchRequestHeadersByCriteria(P_BP_HTTP_SESSION session, char **field_names, BP_BOOL enabled)
{

	// ensure we have a session
	if(!session)
		return ERR_FAILURE;

	// ensure we have a header count
	if(!session->request_headers_n)
		return ERR_FAILURE;

	// ensure we have a header pointer
	if(!session->request_headers)
		return ERR_FAILURE;

	// destroy search results if they already exist
	if(session->request_header_search_results)
		BP_HTTPDestroyRequestHeadersSearchResults(session);

	// field name length, for compare
	size_t field_name_len     = 0;
	size_t field_name_len_hdr = 0;


	// allocate new structure
	P_BP_HTTP_REQUEST_HEADER_SEARCH_RESULTS search_results = (P_BP_HTTP_REQUEST_HEADER_SEARCH_RESULTS) tq_calloc
	(
			session->m,
			sizeof(BP_HTTP_REQUEST_HEADER_SEARCH_RESULTS),
			1,
			BPLN
	);

	// create a reference for added readability
	P_BP_HTTP_REQUEST_HEADER header = NULL;

	// loop iterator
	size_t n = 0;

	// if we have field names to compare, run them here
	size_t j = 0;
	if(field_names)
	for(; field_names[j]; j++)
	{

		// lookup field names now
		for(n = 0; n < session->request_headers_n; n++)
		{

			// set reference
			header = &session->request_headers[n];

			// set field name len
			if(field_names[j])
				field_name_len = bpstrlen(field_names[j]);
			if(header->field_name)
				field_name_len_hdr = bpstrlen(header->field_name);

			// ensure the lengths match
			if(field_name_len)
			if(field_name_len != field_name_len_hdr)
				continue;

			// run binary compare to ensure that names match
			if(memcmp(field_names[j], header->field_name, field_name_len) != 0)
				continue;

			// compare the header if necessary
			if(enabled != BP_INVALID_UNSET)
			if(enabled != header->send_header)
				continue;

			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			// %%% Match is OK %%%%%%%%%%%%%%%%%%%%%%
			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// allocate space for headers
			search_results->headers = (P_BP_HTTP_REQUEST_HEADER *) tq_realloc
			(
				session->m,
				search_results->headers,
				sizeof(P_BP_HTTP_REQUEST_HEADER *) * (search_results->headers_n+1),
				BPLN
			);

			// set the reference
			search_results->headers[search_results->headers_n] = header;

			// increment the search result count
			search_results->headers_n++;

		}


	}

	// iterate through the request headers and identify
	// the selected header.
	if(!field_names)
	for(n = 0; n < session->request_headers_n; n++)
	{

		// set reference
		header = &session->request_headers[n];

		// compare the header if necessary
		if(enabled != BP_INVALID_UNSET)
		if(enabled != header->send_header)
			continue;

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Match is OK %%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// allocate space for headers
		search_results->headers = (P_BP_HTTP_REQUEST_HEADER *) tq_realloc
		(
			session->m,
			search_results->headers,
			sizeof(P_BP_HTTP_REQUEST_HEADER *) * (search_results->headers_n+1),
			BPLN
		);

		// set the reference
		search_results->headers[search_results->headers_n] = header;

		// increment the search result count
		search_results->headers_n++;

	}

	// set search results
	session->request_header_search_results = search_results;

	// return indicating success
	return ERR_SUCCESS;

}


// Destroys search results if necessary.
BP_ERROR_T BP_HTTPDestroyRequestHeadersSearchResults(P_BP_HTTP_SESSION session)
{

	// ensure we have parameters
	if(!session)
		return ERR_FAILURE;
	if(!session->m)
		return ERR_FAILURE;
	if(!session->request_header_search_results)
		return ERR_FAILURE;

	// set result search
	P_BP_HTTP_REQUEST_HEADER_SEARCH_RESULTS search_results = session->request_header_search_results;

	// destroy header data if set
	if(search_results->headers)
	{

		tq_free(session->m, search_results->headers);
		search_results->headers = NULL;
	}

	// destroy search results if set
	tq_free(session->m, search_results);

	// now nullify pointer to prevent re-freeing it later
	session->request_header_search_results = NULL;

	// return indicating success
	return ERR_SUCCESS;

}

// destroy all the request headers in the session (uses tail queue allocator)
BP_ERROR_T BP_HTTPDestroyRequestHeaders(P_BP_HTTP_SESSION session)
{

	// ensure we have a session to work with
	if(!session)
		return ERR_FAILURE;

	// ensure we have a header count
	if(!session->request_headers_n)
		return ERR_FAILURE;

	// ensure we have a header pointer
	if(!session->request_headers)
		return ERR_FAILURE;

	// create a reference for added readability
	P_BP_HTTP_REQUEST_HEADER header = NULL;

	// iterate through the request headers and identify
	// the selected header.
	size_t n = 0;
	for(; n < session->request_headers_n; n++)
	{

		// set reference
		header = &session->request_headers[n];

		// destroy individual elements
		if(header->field_description)
			tq_free(session->m, header->field_description);
		if(header->field_name)
			tq_free(session->m, header->field_name);
		if(header->value)
			tq_free(session->m, header->value);

	}

	// destroy the header
	tq_free(session->m, session->request_headers);

	// reset the header count
	session->request_headers_n = 0;

	// return indicating success
	return ERR_SUCCESS;

}

// == Update ==

// Update a request header with a new value or status.
BP_ERROR_T BP_HTTPUpdateRequestHeader(P_BP_HTTP_SESSION session, P_BP_HTTP_REQUEST_HEADER header, char *new_val, BP_BOOL new_enabled_status)
{

	// ensure we have a session to work with
	if(!session)
		return ERR_FAILURE;

	// ensure we have a header
	if(!header)
		return ERR_FAILURE;

	if(new_val)
	{
		tq_free(session->m, header->value);
		header->value = tq_strdup(session->m, new_val, BPLN);
	}
	if(new_enabled_status != BP_INVALID_UNSET)
	{
		header->send_header = new_enabled_status ? BP_TRUE : BP_FALSE;
	}

	// return indicating success
	return ERR_SUCCESS;

}

// Update all the items in current search results with the provided data.
BP_ERROR_T BP_HTTPUpdateRequestHeadersFromSearchResults(P_BP_HTTP_SESSION session, char **new_vals, size_t new_val_n, BP_BOOL * new_enabled_statuses, size_t status_n)
{

	// run pointer checks
	if(!session)
		return ERR_FAILURE;
	if(!session->request_header_search_results)
		return ERR_FAILURE;
	if(!session->request_header_search_results->headers_n)
		return ERR_FAILURE;
	if(!session->request_header_search_results->headers)
		return ERR_FAILURE;

	// walk headers and perform assignments
	size_t n = 0;
	for(; session->request_header_search_results->headers_n; n++)
	{

		// ensure we're under bound
		if(n >= new_val_n)
			break;

		// attempt to update the request header
		BP_HTTPUpdateRequestHeader(session, session->request_header_search_results->headers[n], new_vals[n], BP_INVALID_UNSET);

	}

	// now set statuses
	for(n = 0; session->request_header_search_results->headers_n; n++)
	{

		// ensure we're under bound
		if(n >= status_n)
			break;

		// attempt to update the request header
		BP_HTTPUpdateRequestHeader(session, session->request_header_search_results->headers[n], NULL, new_enabled_statuses[n]);

	}


	// return indicating success
	return ERR_SUCCESS;

}


// == Display Results ==

// Display request header search results
BP_ERROR_T BP_HTTPDisplayRequestHeaderSearchResults(P_BP_HTTP_SESSION session)
{

	if(!session)
		return ERR_FAILURE;
	if(!session->request_header_search_results)
		return ERR_FAILURE;

	// search result pointer
	P_BP_HTTP_REQUEST_HEADER_SEARCH_RESULTS search_results = session->request_header_search_results;

	if(!search_results)
		return ERR_FAILURE;

	printf("\n [+] Search Results (%p)", search_results);
	printf("\n\t    headers:   %p", search_results->headers);
	printf("\n\t    headers_n: %zu", search_results->headers_n);

	// create a reference for added readability
	P_BP_HTTP_REQUEST_HEADER header = NULL;

	size_t n = 0;
	for(; n < search_results->headers_n; n++)
	{

		// set reference
		header = search_results->headers[n];

		// display header data
		printf("\n");
		printf("\n\t [-] Search Result Request Header: %p", header);
		printf("\n\t     field_name:        %s", header->field_name);
		printf("\n\t     send_header:       %s", header->send_header ? "yes" : "no");
		printf("\n\t     value:             %s", header->value);
		printf("\n\t     field_description: %s", header->field_description);

	}

	printf("\n");

	// return indicating success
	return ERR_SUCCESS;

}

// Display the request headers for the current session.  Set second parameter to BP_TRUE, BP_FALSE to match.  Set to BP_INVALID_UNSET
// to display all.
BP_ERROR_T BP_HTTPDisplayRequestHeaders(P_BP_HTTP_SESSION session, BP_BOOL only_display_enabled_matches)
{

	// ensure we have a session to work with
	if(!session)
		return ERR_FAILURE;

	// ensure we have a header count
	if(!session->request_headers_n)
		return ERR_FAILURE;

	// ensure we have a header pointer
	if(!session->request_headers)
		return ERR_FAILURE;

	// create a reference for added readability
	P_BP_HTTP_REQUEST_HEADER header = NULL;

	printf("\n [+] HTTP Request Headers (session: %p)", session);
	printf("\n     request_headers:    %p", session->request_headers);
	printf("\n     request_headers_n:  %p", session->request_headers_n);


	// iterate through the request headers and identify
	// the selected header.
	size_t n = 0;
	for(; n < session->request_headers_n; n++)
	{

		// set reference
		header = &session->request_headers[n];

		// display header data
		printf("\n");
		printf("\n\t [-] Request Header: %p", header);
		printf("\n\t     field_name:        %s", header->field_name);
		printf("\n\t     send_header:       %s", header->send_header ? "yes" : "no");
		printf("\n\t     value:             %s", header->value);
		printf("\n\t     field_description: %s", header->field_description);

	}

	// add trailing newline
	printf("\n");

	// return indicating success
	return ERR_SUCCESS;

}



// All in one display routine, attempts to print out http info.
BP_ERROR_T BP_HTTPDisplaySession(P_BP_HTTP_SESSION session)
{

	// ensure we have a session
	if(!session)
		return ERR_FAILURE;

	printf("\n [+] HTTP Session (%p)", session);
	printf("\n     is_clone:                              %s", session->is_clone ? "Yes" : "No");
	printf("\n     m:                                     %p", session->m);
	printf("\n     lastRequestedHref:                     %s", session->lastRequestedHref);
	printf("\n     curl:                                  %p", session->curl);
	printf("\n     userPass:                              %s", session->userPass);
	printf("\n     use_web_proxy_set:                     %s", session->use_db_proxy_set ? "Yes" : "No");
	printf("\n     skip_jit_validation_checks:            %s", session->skip_jit_validation_checks ? "Yes" : "No");
	printf("\n     db_populated_proxy_groups:             %p", session->db_populated_proxy_groups);
	printf("\n     db_populated_proxy_group_n:            %zu", session->db_populated_proxy_n);
	printf("\n     db_populated_proxy_sets:               %p", session->db_populated_proxy_sets);
	printf("\n     db_populated_proxy_n:                  %zu", session->db_populated_proxy_n);
	printf("\n     db_populated_proxy_validation_groups:  %p", session->db_populated_proxy_validation_groups);
	printf("\n     db_populated_proxy_validation_group_n: %zu", session->db_populated_proxy_validation_group_n);
	printf("\n     db_populated_proxy_validation_tests:   %p", session->db_populated_proxy_validation_tests);
	printf("\n     db_populated_proxy_validation_test_n:  %zu", session->db_populated_proxy_validation_test_n);
	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Http Returned Header Manipulation/Extraction %%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Creates a pointer to a structure of header elements.  The elements are
// seperated by index if one is available.
P_BP_HTTP_PARSED_HEADER_INFO BP_HTTPParseHeaders(P_CURL_READ_WEBPAGE data)
{

	// ensure we have inbound data for parsing
	if(!data)
	{
		printf("\n[!!] Err: Cannot parse http headers (there is no data)");
		return NULL;
	}

	// set the regex delimiter to header field seperators
	char *reg_delim = (char *)"\r\n";

	if(!reg_delim)
		return false;

	// first get number of replacements
	char ** replacements    = NULL;
	size_t num_replacements = 0;


	// create match info loop
	BP_REGEX_MATCH_INFO   reg_match = {0};
	reg_match.matches      = NULL;
	reg_match.num_matched = 0;

	// for detecting bugs in string
	size_t is_match_you_want = 0;
	if(memcmp(reg_delim, " | ", 3) == 0)
	{
		is_match_you_want++;
	}

	// walk regular expressions and create duplication strings
	size_t num_toks = 0;

	// allocate space for the token string
	char *tok_str =  (char *) bpcalloc(data->hdr_data_size+10, 1);

	// copy in data
	memcpy(tok_str, data->hdr_data, data->hdr_data_size);

	// ensure we have a token string
	if(!tok_str)
		return NULL;

	// save original pointer to the allocated data (because token moves string poistions below)
	char *free_tok_str = tok_str;

	// pointer to currently iterating element
	char *element = NULL;

	// This loop explodes the header information into a char ** using the delimiter
	// \n\r.
	for(;
		BP_MatchRegularExpression(tok_str, bpstrlen(tok_str), reg_delim, 0, 1, &reg_match) == ERR_SUCCESS;
		num_toks++
		)
	{

		//
		if(reg_match.num_matched != 0)
		{

			// %%% Done quote detection %%%%%%%%%%%%%%%%%%%%%%%

			// duplicate element
			element = bpstrndup(tok_str, reg_match.matches->rm_so+1);
			if(!element)
			{
				continue;
			}
			if(!strnlen(element, 10))
			{
				bpfree(element);
				element = NULL;
				continue;
			}

			// move token string forward
			tok_str = (char *)&tok_str[reg_match.matches->rm_eo];

			// increment the counter before alloc
			num_replacements++;

			// bprealloc space for replacement pointer index position
			replacements = (char **) bprealloc( replacements, sizeof(char*) * (num_replacements + 1) );

			// set newly allocated element in the array
			replacements[num_replacements-1] = element;
			replacements[num_replacements]   = NULL;

		}
		else
		{
			// break the loop as we've hit the last element
			break;
		}

		// destroy match information
		BP_DestroyRegexpMatchInfo(&reg_match);

	}

	// this logic allows for us to add the tail of the header array(which is not processed
	// in the above loop) into the set of indexes.  This only occurs if we've already had headers
	// supplied in the index.
	if(num_replacements)
	{

		// increment replacements counter
		num_replacements++;

		// bprealloc space for replacements
		replacements = (char **) bprealloc(replacements, sizeof(char*) * (num_replacements+1) );

		// create final element
		element = bpstrdup(tok_str);

		// set replacements in array
		replacements[num_replacements - 1] = element;
		replacements[num_replacements] = NULL;

	}

	// At this point, replacements has been populated with header information.


	// set a pointer to replacements that we can manipulate below
	char ** exploded       = replacements;

	// zero out empty parameters
	size_t i          = 0;
	size_t subtracted = 0;

	// check explode
	for(; i < num_replacements; i++)
	{

		// if the exploded string is zero length, free it, as it's no longer useful
		// for us in terms of information gain.
		if(!strnlen(exploded[i], 10))
		{
			bpfree(exploded[i]);
			exploded[i] = NULL;
			subtracted++;
		}

	}

	// destroy the token string used above
	bpfree(free_tok_str);

	// reset pointers
	free_tok_str = NULL;
	tok_str      = NULL;

	// === Done Parsing Initial Exploded (non-delineated) Headers ==================

	// allocate header managment structure
	P_BP_HTTP_PARSED_HEADER_INFO parsed_headers = (P_BP_HTTP_PARSED_HEADER_INFO) bpcalloc(sizeof(BP_HTTP_PARSED_HEADER_INFO), 1);

	// set the header count in the managment struct
	parsed_headers->header_count = num_replacements - subtracted;

	// allocate space for the headers (actual header entries)
	parsed_headers->headers = (P_BP_HTTP_PARSED_HEADER) bpcalloc(sizeof(BP_HTTP_PARSED_HEADER) * parsed_headers->header_count, 1);

	// now fill in header elements
	for(i = 0; i < parsed_headers->header_count; i++)
	{
		if(exploded[i])
			parsed_headers->headers[i].unparsed = bpstrdup(exploded[i]);
		else
			parsed_headers->headers[i].unparsed = NULL;
	}

	size_t j = 0;
	for(i = 0; i < parsed_headers->header_count; i++)
	{

		// never run a null header through the below logic (they can crop up due to bad server data)
		if(!parsed_headers->headers[i].unparsed)
			continue;

		printf("\n  unparsed - %s", parsed_headers->headers[i]. unparsed);

		// skip over http status message
		if(i == 0)
			continue;

		// now parse out fields
		for(j = 0; parsed_headers->headers[i].unparsed[j]; j++)
		{

			// break the loop if we find a null terminator
			if(!parsed_headers->headers[i].unparsed[j])
				break;

			if(parsed_headers->headers[i].unparsed[j] == ':')
			{

				// set index as it appears
				parsed_headers->headers[i].index_len = j;

				// allocate space for header
				parsed_headers->headers[i].header_index = (char *) bpcalloc(j+1, 1);

				 // copy in index
				memcpy( parsed_headers->headers[i].header_index,  (char *) parsed_headers->headers[i].unparsed, j);

				// loop to skip past any spaces
				size_t k = 0;
				for(; k < bpstrlen((char *) &parsed_headers->headers[i].unparsed[j+1]); k++)
				{

					// skip past spaces
					if(parsed_headers->headers[i].unparsed[j+1+k] == ' ')
						continue;
					if(parsed_headers->headers[i].unparsed[j+1+k] == '\t')
						continue;

					// break the loop in the case that we have a non whitespace character
					break;

				}

				// set length and allocate data of value
				parsed_headers->headers[i].value_len = bpstrlen((char *) &parsed_headers->headers[i].unparsed[j+1+k]);
				parsed_headers->headers[i].value     = (char *) bpcalloc(parsed_headers->headers[i].value_len+1, 1);

				// copy in value itself
				memcpy(parsed_headers->headers[i].value, (char *) &parsed_headers->headers[i].unparsed[j+1+k], parsed_headers->headers[i].value_len);

				// exit loop
				break;

			}

		}

	}

	// destroy the replacements where necessary
	if(replacements)
	{
		BP_DestroyPointerArray((void **) replacements, 	num_replacements);
		bpfree(replacements);
	}

	// return the header info structure (or null if unset)
	return parsed_headers;

}

// print out headers
BP_ERROR_T BP_HTTPDisplayHeaderData(P_BP_HTTP_PARSED_HEADER_INFO header_info)
{
	if(!header_info)
		return ERR_FAILURE;

	P_BP_HTTP_PARSED_HEADER header = NULL;
	size_t j = 0;
	for(; j < header_info->header_count; j++)
	{
		printf("\n [+] Header Index %u - %p", j, (void *) &header_info->headers[j]);
		printf("\n\t - header name:  %s", header_info->headers[j].header_index);
		printf("\n\t - header value: %s", header_info->headers[j].value);
		printf("\n");
	}

	// exit routine indicating success
	return ERR_SUCCESS;
}


// destroys parsed header information
BP_ERROR_T BP_HTTPDestroyParsedHeaders(P_BP_HTTP_PARSED_HEADER_INFO header_info)
{
	if(!header_info)
		return ERR_FAILURE;

	P_BP_HTTP_PARSED_HEADER header = NULL;
	size_t j = 0;
	for(; j < header_info->header_count; j++)
	{

		// destroy unparsed data if necessary
		if(header_info->headers[j].unparsed)
		{
			bpfree(header_info->headers[j].unparsed);
			header_info->headers[j].unparsed = NULL;
		}

		// destroy header index if necessary
		if(header_info->headers[j].header_index)
		{
			bpfree(header_info->headers[j].header_index);
			header_info->headers[j].header_index = NULL;
		}

		// destroy header value fields
		if(header_info->headers[j].value)
		{
			bpfree(header_info->headers[j].value);
			header_info->headers[j].value = NULL;
		}

	}

	if(header_info->headers)
	{
		bpfree(header_info->headers);
	}

	// destroy the header struct itself
	bpfree(header_info);

	// exit routine indicating success
	return ERR_SUCCESS;

}


// cleans out a  http session of all allocated memory
BP_ERROR_T BP_HTTPCleanSession(P_BP_HTTP_SESSION session)
{

	session->cookie;
	session->cookieFromFile;
	session->fileName;
	session->headers;
	session->lastRequestedHref;
	session->httpPostData;
	session->interface;
	session->proxies;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%% PREG JIT Match Tests %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// after a page has been retrieved, the memory buffers are populated.  At this point
// we can run our JIT check set.
BP_ERROR_T BP_HTTPRunJITExpressionChecks(P_BP_HTTP_SESSION session)
{

	// run quick data checks
	if(!session)
		return ERR_FAILURE;
	if(!session->jit_pcre_regexp_check_n)
		return ERR_FAILURE;
	if(!session->page)
		return ERR_FAILURE;
	if(!session->page->page_data)
		return ERR_FAILURE;
	if(!session->page->page_data_size)
		return ERR_FAILURE;

	// reference to individual entry used in loop logic below
	P_BP_PCRE_JIT_MATCH_ENTRY entry = NULL;

	// flag which is set if any match is made (used to determine exit code)
	size_t match_has_been_made_somewhere = 0;

	// run each check
	size_t n = 0;
	for(; n < session->jit_pcre_regexp_check_n; n++)
	{

		// set the entry from the check index
		entry = &session->jit_pcre_regexp_checks[n];


		// attempt to match jit check set
		if(BP_MatchRegularExpressionPCRE(session->page->page_data, session->page->page_data_size, entry->regexp, 0, 4096, &entry->match_info, true) == ERR_SUCCESS)
		{
			match_has_been_made_somewhere=1;
		}


	}

	// return indicating success if a match has been made within one of the
	// test sets.
	if(match_has_been_made_somewhere)
		return ERR_SUCCESS;


	// exit indicating failure in the case that no match has been made
	return ERR_FAILURE;

}
// displays regular expression tests
BP_ERROR_T BP_DisplayJITRegexpTest(P_BP_PCRE_JIT_MATCH_ENTRY jit_test)
{

	// check that the structure pointer is not null
	if(!jit_test)
		return ERR_FAILURE;

	// display entry
	printf("\n [+] JIT PREG Check: %p", jit_test);
	printf("\n\t db id:        %zu",    jit_test->id);
	printf("\n\t user_id:      %zu",    jit_test->user_id);
	printf("\n\t group_id:     %zu",    jit_test->group_id);
	printf("\n\t regexp:       %s",     jit_test->regexp);
	printf("\n\t description:  %s",     jit_test->description);
	printf("\n\t created_ts:   %s",     jit_test->created_ts);
	printf("\n");

	// display individual matches
	if(jit_test->match_info.pcre_enabled)
	if(jit_test->match_info.pcre_num_matched)
	{

		// user message
		printf("\n [+] Matches in Order:");

		// loop and display entries
		size_t j = 0;
		for(; j < jit_test->match_info.pcre_num_matched; j++)
		{

			// display individual matched string
			if(jit_test->match_info.preg_matches[j].matched_string)
				printf("\n - %s", jit_test->match_info.preg_matches[j].matched_string);

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// destroy a set of match entries, free'ing all relevant data
BP_ERROR_T BP_DestroyJITMatchEntries(P_BP_PCRE_JIT_MATCH_ENTRY jit_test)
{

	// check that the structure pointer is not null
	if(!jit_test)
		return ERR_FAILURE;

	// destroy individual matches
	if(jit_test->match_info.pcre_num_matched)
	{

		// loop and display entries
		size_t j = 0;
		for(; j < jit_test->match_info.pcre_num_matched; j++)
		{

			// destroy the match information at the current index
			BP_DestroyRegexpMatchInfo(&jit_test[j].match_info);

		}

	}

	// now free the actual structure
	bpfree(jit_test);

	// return indicating success
	return ERR_SUCCESS;

}



// TAILQ VERSION: destroy a set of match entries, free'ing all relevant data
BP_ERROR_T BP_DestroyJITMatchEntriesTQ(P_BP_PCRE_JIT_MATCH_ENTRY jit_test, P_BP_LINKL tq)
{

	// check that the structure pointer is not null
	if(!jit_test)
		return ERR_FAILURE;

	// destroy individual matches
	if(jit_test->match_info.pcre_num_matched)
	{

		// loop and display entries
		size_t j = 0;
		for(; j < jit_test->match_info.pcre_num_matched; j++)
		{

			// destroy the match information at the current index
			BP_DestroyRegexpMatchInfo(&jit_test[j].match_info);

		}

	}

	// now free the actual structure
	tq_free(tq, jit_test);

	// return indicating success
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%% Database Oriented Web Request Balancer %%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Create new DB web request balancer for use with a HTTP session.
P_BP_DB_WEB_REQUEST_BALANCER BP_CreateDBWebRequestBalancer()
{

	// allocate space for the web balancer, and zero
	P_BP_DB_WEB_REQUEST_BALANCER web_balancer = (P_BP_DB_WEB_REQUEST_BALANCER)calloc(sizeof(BP_DB_WEB_REQUEST_BALANCER), 1);
	if(!web_balancer)
		return NULL;

	// create new garbage collector
	web_balancer->gc = BP_CreateNewGarbageCollector("web request balancer", 1);

	// create new basic balancer
	web_balancer->balancer = BP_CreateNewBalancer();

	// return the structure
	return web_balancer;

}

// destroy a web request balancer
BP_ERROR_T BP_DestroyDBWebRequestBalancer(P_BP_DB_WEB_REQUEST_BALANCER balancer)
{

	// ensure we have a balancer pointer
	if(!balancer)
		return ERR_FAILURE;

	// destroy individual balancer
	if(balancer->balancer)
	{
		BP_DestroyBalancer(balancer->balancer);
		balancer->balancer=NULL;
	}

	// destroy garbage collector if necessary
	if(balancer->gc)
	{
		BP_DestroyGarbageCollector(balancer->gc);
		balancer->gc=NULL;
	}

	// return indicating success
	return ERR_SUCCESS;

}

// display the web reuest balancer
BP_ERROR_T BP_DisplayDBWebRequestBalancer(P_BP_DB_WEB_REQUEST_BALANCER balancer, size_t display_balance_table)
{

	// exit if the balancer isn't set
	if(!balancer)
		return ERR_FAILURE;

	printf("\n [+] Web Request Balancer %p", balancer);
	printf("\n\t entries:           %p", balancer->entries);
	printf("\n\t entry_n:           %u", balancer->entry_n);
	printf("\n\t requires_update:   %u", balancer->requires_update);
	printf("\n\t balancer:          %p", balancer->balancer);
	printf("\n\t garbage collector: %p", balancer->gc);

	// display entries
	printf("\n\n\t [+] Displaying Balance Entries:");
	size_t n = 0;
	for(; n < balancer->entry_n; n++)
	{
		printf("\n\t [%u] Entry:", n);
		printf("\n\t whitelist: %s", balancer->entries[n].whitelist);
		printf("\n\t weight:    %u", balancer->entries[n].weight);
		printf("\n");
	}

	// display the balance table if the parameter was set
	if(display_balance_table)
	{

		printf("\n[+] Now displaying balancer as requested. ");
		BP_DisplayBalancer(balancer->balancer);

	}

	// return indicating success
	return ERR_SUCCESS;

}


// run a url through the balancer
BP_ERROR_T BP_DBWebRequestBalancerProcessTarget(P_BP_DB_WEB_REQUEST_BALANCER balancer, char * target)
{

	// ensure we have a balancer
	if(!balancer)
		return ERR_FAILURE;

	// ensure we have some entries in the stack
	if(!balancer->entry_n)
		return ERR_FAILURE;

	// match info
	BP_REGEX_MATCH_INFO match_info = {0};
	memset(&match_info, 0x00, sizeof(match_info));



	// walk entries and make checks
	size_t n = 0;
	for(; n < balancer->entry_n; n++)
	{

		// run regular expression match check
		if(BP_MatchRegularExpressionPCRE(target, bpstrlen(target), (char *)&balancer->entries[n].whitelist, 0, 1,  &match_info, 0) == ERR_SUCCESS)
		{

			// increase the weight of the item if we matched the
			// blacklist.
			if(BP_BalancerIncWeightEntryAtIndex(balancer->balancer , n) == ERR_SUCCESS)
			{

				// update balancer weighting after successful increment
				BP_BalancerUpdate(balancer->balancer);

				// destroy match info where necessary
				BP_DestroyRegexpMatchInfo(&match_info);

			}
			else
			{

				// destroy match info where necessary
				BP_DestroyRegexpMatchInfo(&match_info);


			}

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// push a whitelist element onto the balancer stack
BP_ERROR_T BP_DBWebRequestBalancerPushWhitelistItem(P_BP_DB_WEB_REQUEST_BALANCER balancer, P_BP_DB_WEB_WEIGHTED_WHITELIST_ELEMENT item)
{

	// check pointers
	if(!balancer || !item)
		return ERR_FAILURE;

	// require a garbage collector
	if(!balancer->gc)
		return ERR_FAILURE;

	// increase the entry count
	balancer->entry_n++;

	// allocate space
	balancer->entries = (P_BP_DB_WEB_WEIGHTED_WHITELIST_ELEMENT) BP_realloc(balancer->gc, NULL,(void *) balancer->entries, sizeof(BP_DB_WEB_WEIGHTED_WHITELIST_ELEMENT)*balancer->entry_n);

	// copy in structure
	memcpy((void *) &balancer->entries[balancer->entry_n-1], item, sizeof(BP_DB_WEB_WEIGHTED_WHITELIST_ELEMENT));

	// mark that table requires analysis and exit
	balancer->requires_update = 1;


	// ===== add to base balancer ====

	// create stack entry
	BP_WEIGHT_HIT_ENTRY entry = {0};
	memset(&entry, 0x00, sizeof(entry));

	// set current weight from indexed weight before push
	entry.current_weight = balancer->entries[balancer->entry_n-1].weight;

	// add item on the stack (base balancer)
	BP_BalancerPushItemOnStack(balancer->balancer, &entry);

	// update balancer weights
	BP_BalancerUpdate(balancer->balancer);

	// return indicating success
	return ERR_SUCCESS;

}


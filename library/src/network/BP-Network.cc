/***************************************************************************
 *            BP-Network.cc
 *
 *  Tue Nov 14 22:20:04 2006
 *  Copyright  2006  User
 *  Email
 ****************************************************************************/

#include "../../include/BP-Main.h"

// creates a socket to a specified host and connects, works on udp or tcp.
// Any positive value in the setListen parameter will be considered true, and the
// socket will be created to as a listening port, waiting on accept.

// Listening Socket Creation - 
// 	To create a listening socket, pass a nonzero value in the setListen
// 	parameter.  This will cause the function to use the addr parameter
//  as a source host, and will set the destination address as INADDR_ANY.
// 	The socket will then wait on a NetAccept() call to begin transport 
//	action.

// Connect Socket Creation -
// 	To create a socket to connect to a remote host, pass a 0 to the
// 	function in the setListen field.  This will set the source information
//  as inaddr any.
//
// This function will create a new network handle. If no network handle is
// provided (NULL ptr value) this function will create a new network handle
// and return it.
P_BP_NETWORK_HANDLE BP_NetCreateSocket
(
	char              * netAddr,
	short               netPort,
	int                 proto,
	int                 setListen,
	P_BP_NETWORK_HANDLE net_handle_in
)
{

	// ensure we have an address and a port
	if(!netAddr || !netPort)
		return NULL;

	// attempt to create a new network handle
	P_BP_NETWORK_HANDLE net_handle = NULL;

	// flag indicating whether or not the net_handle was
	// passed in, or if it was locally allocated.
	size_t local_handle = BP_FALSE;

	// set the handle from parameter if set
	if(net_handle_in)
	{
		net_handle = net_handle_in;
	}
	else
	{
		net_handle = BP_CreateNetworkHandle();
		local_handle = BP_TRUE;
	}

	// host ent
	struct hostent *hostPtr = NULL;

	// option flag which will be set via setsockopt
	int optionFlag = 1;
	
	// Create socket based around protocol
	if( (proto == IPPROTO_TCP) || (proto == 0x00) )
	{

		// create the new socket
		net_handle->sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

		// set options (TCP_NODELAY disables nagle, SO_REUSEADDR allows the address to be reused)
		if(setsockopt(net_handle->sock, IPPROTO_TCP, TCP_NODELAY, (char *) &optionFlag, sizeof(int) ) == -1)
		{

			// destroy the network handle
			BP_DestroyNetworkHandle(net_handle);

			// return indicating failure
			return NULL;

		}

		// set additional options for reusing addresses (prevents congestion)
		if(setsockopt(net_handle->sock, SOL_SOCKET, SO_REUSEADDR, (char *) &optionFlag, sizeof(int)) == -1)
		{

			// destroy the network handle
			BP_DestroyNetworkHandle(net_handle);

			// return indicating failure
			return NULL;

		}
		
		
	}
	else if(proto == IPPROTO_UDP)
	{

		// check if the user is proxychained or not
		if(net_handle->proxy_chain_established_successfully == BP_TRUE)
		{

		}
		else
		{
			// create a UDP socket if the dev wants unproxied UDP
			net_handle->sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
		}

	}
	else
	{
		BP_DestroyNetworkHandle(net_handle);
		return NULL;
	}
	
	
	// destination gethostbyname
	hostPtr = gethostbyname(netAddr);
	if(!hostPtr)
	{
		BP_DestroyNetworkHandle(net_handle);
		return NULL;
	}
	
	
	// if its a not a listening port
	if(setListen == 0)
	{
		
		// source setup for non listening socket
		net_handle->source.sin_family      = AF_INET;
		net_handle->source.sin_addr.s_addr = INADDR_ANY;
		net_handle->source.sin_port		   = 0;
		
		// destination setup for non listening socket
		net_handle->dest.sin_family = hostPtr->h_addrtype;
		memcpy(&net_handle->dest.sin_addr.s_addr, hostPtr->h_addr, hostPtr->h_length);
		net_handle->dest.sin_port = htons(netPort);
		
	}
	else
	{
		
		// source setup for listening socket
		net_handle->source.sin_family	   = AF_INET;
		net_handle->source.sin_port        = htons(netPort);
		net_handle->source.sin_addr.s_addr = INADDR_ANY;

	}
	
	
	// if not set to listen, attempt to connect
	if(setListen == 0)
	{

		// attempt connect if we're not listening
		if(connect(net_handle->sock, (struct sockaddr *) &net_handle->dest, sizeof(struct sockaddr)) == -1)
		{
			BP_DestroyNetworkHandle(net_handle);
			return NULL;
		}

		// indicate that the last tcp connect was established OK
		net_handle->last_tcp_connect_succeeded = BP_TRUE;

	}
	
	// if listening is a nonzero value, attempt to listen
	if(setListen != 0)
	{
		
		// attempt to bind
		if( bind(net_handle->sock, (struct sockaddr *) &net_handle->source, sizeof(struct sockaddr)) == -1)
		{
			BP_DestroyNetworkHandle(net_handle);
			return NULL;
		}			
		
		// attempt to listen
		if(listen(net_handle->sock, DEFAULT_BACKLOG) != 0)
		{
			BP_DestroyNetworkHandle(net_handle);
			return NULL;
		}
		
		// indicate that the last tcp listen succeded
		net_handle->last_tcp_listen_succeeded = BP_TRUE;

	}
	
	// if we got here we've got a successful connection made
	return net_handle;

}

// accepts on a listening connection
P_BP_NETWORK_HANDLE BP_NetAccept(P_BP_NETWORK_HANDLE net_handle)
{

	// ensure we have a network handle
	if(!net_handle)
		return NULL;

	
	// attempt to create the accept socket
	P_BP_NETWORK_HANDLE acceptSock = BP_CreateNetworkHandle();

	// set sockaddr len
	size_t structLen = sizeof(struct sockaddr);

	// attempt accept
	acceptSock->sock = accept(net_handle->sock, (struct sockaddr *) &acceptSock->source, (socklen_t*) &structLen);

	// return null if the sock is null
	if(acceptSock->sock == -1)
	{

		// set the sock to zero before destroy
		acceptSock->sock = 0;

		// destroy the network handle
		BP_DestroyNetworkHandle(acceptSock);

		// return indicating bad
		return NULL;

	}

	// return the attempt sock
	return acceptSock;

}


// Sends A Specified Number Of Bytes Across A Socket
int BP_NetSend(P_BP_NETWORK_HANDLE net_handle, char * data, int size)
{

	// no handle
	if(!net_handle)
		return -1;
	
	// no socket
	if(!net_handle->sock)
		return -1;
	
	// no data
	if(!data)
		return -1;

	// reset last send counter before doing anything
	net_handle->last_send_counter = 0;

	// counter for send
	int sendBytes = 0;

	// send data over the socket
	sendBytes = send(net_handle->sock, data, size, 0);

	// ensure we have valid send bytes
	if(sendBytes <= 0)
		return -1;

	// set the last send counter
	net_handle->last_send_counter = sendBytes;

	// increment the tcp total send counter
	net_handle->total_tcp_send_counter += sendBytes;

	// increment the total send counter
	net_handle->total_all_send_counter += sendBytes;

	// return the size written
	return sendBytes;

}


// writes data onto a socket
int BP_NetSendASCII(P_BP_NETWORK_HANDLE net_handle, char * data)
{

	// ensure we have parameters
	if(!net_handle)
		return -1;
	if(!data)
		return -1;

	// send the data
	return BP_NetSend(net_handle, data, bpstrlen(data));

}

// Reads A Specified Number Of Bytes From A Socket
int BP_NetRecv(P_BP_NETWORK_HANDLE net_handle, char * data, int size)
{
	
	// ensure we have a network handle
	if(!net_handle)
		return -1;
	
	// ensure we have a socket
	if(!net_handle->sock)
		return -1;
	
	// ensure that we have data to send, and a valid positive size
	if(!data || !size)
		return -1;
	
	// create and initialize var to hold read bytes
	int readBytes = 0;

	// reset last recv counter before doing anything
	net_handle->last_recv_counter = 0;

	// attempt to read data
	readBytes = recv(net_handle->sock, data, size, 0);
	
	// return error if the read bytes are wrong
	if(readBytes <= 0)
		return -1;
	
	// set the last recv counter
	net_handle->last_recv_counter = readBytes;

	// increment the total recv counter
	net_handle->total_all_recv_counter += readBytes;

	// increment the tcp recv counter
	net_handle->total_tcp_recv_counter += readBytes;

	// return the postiive value representing bytes read
	return readBytes;

}

// Thread safe version of inet_ntoa.  Returns string on the heap rather
// than within a static buffer.
char * BP_NetInAddrToAscii(struct in_addr * in)
{

	// return null if we have no in structure
	if(!in)
		return NULL;

	// create output buffer
	char * out_buff = NULL;

	// set a buffer from the s_addr portion
	uint8_t * buff = (uint8_t *) &in->s_addr;

	// create a buffer for the IP address
	out_buff = (char *) bpcalloc(17, 1);

	// create the buffer (max 16 bytes)
	sprintf(out_buff, "%u.%u.%u.%u", buff[0], buff[1], buff[2], buff[3]);

	// return the output buffer
	return out_buff;

}

// Creates our socket tree
P_BP_NETWORK_HANDLE BP_CreateNetworkHandle()
{

	// create a new network handle
	P_BP_NETWORK_HANDLE net_handle = (P_BP_NETWORK_HANDLE) bpcalloc(sizeof(BP_NETWORK_HANDLE), 1);
	
	// ensure data was created
	if(!net_handle)
		return NULL;

	// create tail queue allocator for this handle
	net_handle->tq = (P_BP_LINKL) BP_CreateLinkL("net", BP_TRUE);

	// if no tail queue allocator can be created, free handle and exit
	if(!net_handle->tq)
	{
		bpfree(net_handle);
		return NULL;
	}

	// allocate space for ssl options
	net_handle->sslOpts = (P_NETWORK_SSL) tq_calloc(net_handle->tq, sizeof(NETWORK_SSL), 1, BPLN);

	// if not ssl options were created, destroy and exit
	if(!net_handle->sslOpts)
	{

		// destroy the tail queue
		BP_DestroyLinkL(net_handle->tq, BP_TRUE);

		// destroy the net handle
		bpfree(net_handle);

		// exit routine
		return NULL;
	}

	// if everything went as expected, return the new handle
	return net_handle;

}

// Destroys a network handle and all of its allocations.
BP_ERROR_T BP_DestroyNetworkHandle(P_BP_NETWORK_HANDLE net_handle)
{

	// ensure we have a handle
	if(!net_handle)
		return ERR_FAILURE;

	// ensure we have ssl options (should always be allocated, and thus set)
	if(!net_handle->sslOpts)
		return ERR_FAILURE;
	
	// destroy the ssl context here
	if(net_handle->sslOpts->context)
		SSL_CTX_free(net_handle->sslOpts->context);
	
	// destroy ssl object
	if(net_handle->sslOpts->ssl)
		SSL_free(net_handle->sslOpts->ssl);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Shutdown Socket %%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we have a socket, shut it down (don't use close, as close can get stuck
	// waiting for data transmission to finish).
	if(net_handle->sock)
	{
		shutdown(net_handle->sock, 2);
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Destroy the LinkL and Exit %%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// destroy the tail queue
	BP_DestroyLinkL(net_handle->tq, BP_TRUE);

	// destroy the net handle
	bpfree(net_handle);
	
	// return
	return ERR_SUCCESS;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Network Handle Proxy Managment %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// adds a proxy to the chain stack
BP_ERROR_T BP_NetProxyAddChainElement
(
	P_BP_NETWORK_HANDLE   net_handle,
	char *                description,
	char *                proxy_ip_addr,
	unsigned short        proxy_port,
	char *                destination_host,
	unsigned short        destination_port,
	int                   destination_proto,
	BP_NETWORK_PROXY_TYPE type,
	char *                username,
	char *                passwd
)
{

	// ensure we have a network handle
	if(!net_handle)
		return ERR_FAILURE;

	// check to make sure we have a valid type (zero value
	// is a bad type)
	if(!type)
		return ERR_FAILURE;

	// check we have an ip address to connect to
	if(!proxy_ip_addr)
		return ERR_FAILURE;

	// ensure the network handle has a valid tail queue allocator
	if(!net_handle->tq)
		return ERR_FAILURE;

	// if proxy info structure hasn't been created yet, create it
	if(!net_handle->proxy_info)
	{

		// allocate space for proxy info
		net_handle->proxy_info = (P_BP_NETWORK_PROXIES) tq_calloc(net_handle->tq, sizeof(BP_NETWORK_PROXIES), 1, BPLN);

		// if we couldn't create the structure, return failure
		if(!net_handle->proxy_info)
			return ERR_FAILURE;

	}

	// increase the proxy chain count
	net_handle->proxy_info->proxy_chain_n++;

	// (re)allocate space for the proxy chain element
	net_handle->proxy_info->proxy_chain = (P_BP_NETWORK_PROXY) tq_realloc
										  (
												net_handle->tq,
												net_handle->proxy_info->proxy_chain,
												sizeof(BP_NETWORK_PROXY) * net_handle->proxy_info->proxy_chain_n,
												BPLN
											);




	// ensure that the proxy chain was allocated ok
	if(!net_handle->proxy_info->proxy_chain)
		return ERR_FAILURE;

	// zero out the newly allocated memory
	memset
	(
			&net_handle->proxy_info->proxy_chain[net_handle->proxy_info->proxy_chain_n-1],
			0x00,
			sizeof(BP_NETWORK_PROXY)
	);

	// set local reference
	P_BP_NETWORK_PROXY current_proxy = &net_handle->proxy_info->proxy_chain[net_handle->proxy_info->proxy_chain_n-1];

	// create a new random uuid
	char * new_uuid = BP_GenRandomUUID();

	// set the description if one was passed in
	if(description)
		current_proxy->description = tq_strdup(net_handle->tq, strdup(description), BPLN);

	// link the new uuid in our tail queue allocator (memory managment)
	tq_link_str(net_handle->tq, new_uuid, BPLN);

	// set the current proxy uuid as the newly created one
	current_proxy->uuid = new_uuid;

	// set connection type
	current_proxy->type = type;

	// set ip address
	if(proxy_ip_addr)
	current_proxy->proxy_ip_addr = (char *) tq_strdup(net_handle->tq, proxy_ip_addr,  BPLN);

	// set port
	if(proxy_port)
	current_proxy->proxy_port    = proxy_port;

	// set ip address
	if(proxy_ip_addr)
	current_proxy->proxy_ip_addr = (char *) tq_strdup(net_handle->tq, proxy_ip_addr,  BPLN);

	// set port
	if(proxy_port)
	current_proxy->proxy_port    = proxy_port;

	// set ip address
	if(destination_host)
	current_proxy->destination_host = (char *) tq_strdup(net_handle->tq, destination_host,  BPLN);

	// set destination port
	if(destination_port)
	current_proxy->destination_port    = destination_port;

	// set username
	if(username)
	current_proxy->user    = (char *) tq_strdup(net_handle->tq, username, BPLN);

	// set password
	if(passwd)
	current_proxy->pass    = (char *) tq_strdup(net_handle->tq, passwd,   BPLN);


	// set connected flag to false (should already be set by calloc, but
	// assign here just in case BP_FALSE changes for whatever insane reason)
	current_proxy->connected = BP_FALSE;

	// return indicating success
	return ERR_SUCCESS;

}


// This checks a proxy chain to ensure that data can be sent across it.  This routine
// creates it's own network handle to accomplish this, and does not modifigy the handle
// passed in as a parameter, except to modify values found in the proxy_info structure.
BP_ERROR_T BP_NetProxyChainCheckWorking(P_BP_NETWORK_HANDLE net_handle)
{

	// ensure we have a network handle
	if(!net_handle)
		return ERR_FAILURE;

	// ensure we have a proxy info structure
	if(!net_handle->proxy_info)
		return ERR_FAILURE;

	// ensure we have proxies within the proxy info
	// structure.
	if(!net_handle->proxy_info->proxy_chain_n || !net_handle->proxy_info->proxy_chain)
		return ERR_FAILURE;

	// walk the chain and attempt connects
	size_t n = 0;
	for(; n < net_handle->proxy_info->proxy_chain_n; n++)
	{

	}

	// return indicating success
	return ERR_SUCCESS;
}


// This routine attempts to connect all the proxies found within a provided network handle.
BP_ERROR_T BP_NetProxyConnectChain(P_BP_NETWORK_HANDLE net_handle)
{

	// ensure we have a network handle
	if(!net_handle)
		return ERR_FAILURE;

	// ensure we have a proxy info structure
	if(!net_handle->proxy_info)
		return ERR_FAILURE;

	// ensure we have proxies within the proxy info
	// structure.
	if(!net_handle->proxy_info->proxy_chain_n || !net_handle->proxy_info->proxy_chain)
		return ERR_FAILURE;

	// reference pointer used for convienence within the lower loop
	P_BP_NETWORK_PROXY current_proxy = NULL;

	// walk the chain and attempt connects
	size_t n = 0;
	for(; n < net_handle->proxy_info->proxy_chain_n; n++)
	{

		// set handle to current proxy
		current_proxy = &net_handle->proxy_info->proxy_chain[n];

		// switch on the proxy type to determine which connect routine to
		// utilize when making the connection.
		switch(current_proxy->type)
		{

			// if the type is socks4a, attempt to connect
			case BP_NETWORK_PROXY_TYPE_SOCKS4A:

				// attempt to connect through a socks4 proxy
				BP_NetProxyConnectSOCKS4A(net_handle, current_proxy);

				// check to see if the proxy has connected OK, if not, exit
				if(current_proxy->connected == BP_FALSE)
					return ERR_FAILURE;

				// if we connected successfully, set the current proxy depth here
				net_handle->current_proxy_depth++;

				// continue the loop
				break;

			// connect socks5
			case BP_NETWORK_PROXY_TYPE_SOCKS5:

				// attempt to connect through a socks4 proxy
				BP_NetProxyConnectSOCKS5(net_handle, current_proxy);

				// check to see if the proxy has connected OK, if not, exit
				if(current_proxy->connected == BP_FALSE)
					return ERR_FAILURE;

				// if we connected successfully, set the current proxy depth here
				net_handle->current_proxy_depth++;

				// continue the loop
				break;

			// connect via http connect proxy
			case BP_NETWORK_PROXY_TYPE_CONNECT:
				break;

			// default condition is a bad or unset proxy value, this causes chain to return
			default:
				return ERR_FAILURE;
				break;

		}

	}

	// If we've exited here, we've successfully established a proxy
	// chain through this network handle.
	net_handle->proxy_chain_established_successfully = true;

	// return indicating success
	return ERR_SUCCESS;

}


// This routine will clear out all proxy flags which may have been set
// during previous connect calls.  This is typically used when tearing
// down established chains.
BP_ERROR_T BP_NetProxyResetAllProxyFlags(P_BP_NETWORK_HANDLE net_handle)
{

	// ensure we have a network handle
	if(!net_handle)
		return ERR_FAILURE;

	// ensure we have a proxy info structure
	if(!net_handle->proxy_info)
		return ERR_FAILURE;

	// ensure we have proxies within the proxy info
	// structure.
	if(!net_handle->proxy_info->proxy_chain_n || !net_handle->proxy_info->proxy_chain)
		return ERR_FAILURE;

	// reference pointer used for convienence within the lower loop
	P_BP_NETWORK_PROXY current_proxy = NULL;

	// walk the chain and attempt connects
	size_t n = 0;
	for(; n < net_handle->proxy_info->proxy_chain_n; n++)
	{

		// set handle to current proxy
		current_proxy = &net_handle->proxy_info->proxy_chain[n];

		// reset the proxy flags
		BP_NetProxyResetFlags(&net_handle->proxy_info->proxy_chain[n]);

	}

	// return indicating success
	return ERR_SUCCESS;

}

// Reset flags for an individual proxy.  Does not clear configuration
// data, but only data which was set.
BP_ERROR_T BP_NetProxyResetFlags(P_BP_NETWORK_PROXY proxy)
{

	// ensure we have a proxy pointer
	if(!proxy)
		return ERR_FAILURE;

	// set connected flag to BP_FALSE (default)
	proxy->connected = BP_FALSE;

	// reset the connect timestamp
	proxy->connected_ts = (time_t) 0;

	// return indicating success
	return ERR_SUCCESS;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Network Oriented Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// displays the current network handle
BP_ERROR_T BP_NetDisplayHandle(P_BP_NETWORK_HANDLE net_handle)
{

	// ensure we have a network handle to operate on
	if(!net_handle)
		return ERR_FAILURE;

	// display heading
	printf("\n [+] Network Handle (%p) ==================================", net_handle);
	printf("\n");
	printf("\n\t lock: %p",  &net_handle->lock);
	printf("\n\t tq:   %p", net_handle->tq);
	printf("\n");

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Socket Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create new linkl
	P_BP_LINKL alloc = (P_BP_LINKL) BP_CreateLinkL("none", BP_TRUE);

	// source information
	char * socket_source_ip    = BP_NetInAddrToAscii(&net_handle->source.sin_addr);

	// link the chunks in the linkl
	tq_link(alloc, socket_source_ip,    bpstrlen(socket_source_ip), BPLN);

	// destination information
	char * socket_dest_ip     = BP_NetInAddrToAscii(&net_handle->dest.sin_addr);

	// link the chunks in the linkl
	tq_link(alloc, socket_dest_ip,    bpstrlen(socket_dest_ip), BPLN);


	// socket info
	printf("\n\t [+] Handle Socket Info (if proxychaining, this will be the first proxy)");
	printf("\n\t\t socket:      %i ", net_handle->sock);
	printf("\n\t\t source:      %s:%u", socket_source_ip, net_handle->source.sin_port);
	printf("\n\t\t dest:        %s:%u", socket_dest_ip, net_handle->dest.sin_port);
	printf("\n");

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Connect Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// connect info
	printf("\n\t [+] Handle Connect Info (destination)");
	printf("\n\t\t last TCP connect succeeded: %s",  net_handle->last_tcp_connect_succeeded ? "yes" : "no");
	printf("\n\t\t last TCP listen  succeeded: %s",  net_handle->last_tcp_listen_succeeded  ? "yes" : "no");
	printf("\n\t\t last TCP send    succeeded: %s",  net_handle->last_tcp_send_succeeded    ? "yes" : "no");
	printf("\n\t\t last TCP recv    succeeded: %s",  net_handle->last_tcp_recv_succeeded    ? "yes" : "no");
	printf("\n\t\t last UDP send    succeeded: %s",  net_handle->last_udp_send_succeeded    ? "yes" : "no");
	printf("\n\t\t last UDP recv    succeeded: %s",  net_handle->last_udp_recv_succeeded    ? "yes" : "no");
	printf("\n\t\t last SSL connect succeeded: %s",  net_handle->last_ssl_connect_succeeded ? "yes" : "no");
	printf("\n");


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Transmission Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	printf("\n\t [+] Handle Send/Recv Statistics:");

	// Cumulative/ALL packet oriented values
	printf("\n\t\t total send bytes (all protos): %llu", net_handle->total_all_send_counter);
	printf("\n\t\t total recv bytes (all protos): %llu", net_handle->total_all_recv_counter);

	// TCP oriented values
	printf("\n\t\t total TCP send bytes: %llu", net_handle->total_tcp_send_counter);
	printf("\n\t\t total TCP recv bytes: %llu", net_handle->total_tcp_recv_counter);

	// UDP oriented values
	printf("\n\t\t total UDP send bytes: %llu", net_handle->total_udp_send_counter);
	printf("\n\t\t total UDP recv bytes: %llu", net_handle->total_udp_recv_counter);

	// SSL oriented values
	printf("\n\t\t total SSL send bytes: %llu", net_handle->total_ssl_send_counter);
	printf("\n\t\t total SSL recv bytes: %llu", net_handle->total_ssl_recv_counter);
	printf("\n");


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display SSL Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we have ssl options, display them
	if(net_handle->sslOpts)
	{
		BP_NetDisplaySSLOptions(net_handle->sslOpts);
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Display Proxy Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we have set proxy information, display here
	if(net_handle->proxy_info)
	{
		printf("\n");
		printf("\n\t [+] Network Handle Proxy Information: ");
		printf("\n\t\t proxy_info:                           %p", net_handle->proxy_info);
		printf("\n\t\t proxy chain established successfully: %s", (net_handle->proxy_chain_established_successfully == BP_TRUE) ? "yes" : "no");
		printf("\n\t\t proxy chain current depth:            %u", net_handle->current_proxy_depth);
		printf("\n");
		BP_NetDisplayProxyInfo(net_handle->proxy_info);

	}


	// add trailing newlines
	printf("\n\n ");

	// destroy the linkl here (frees blocks)
	if(alloc)
	{
		BP_DestroyLinkL(alloc, BP_TRUE);
	}

	// return indicating success
	return ERR_SUCCESS;

}


// displays all elements within a proxy information structure.
BP_ERROR_T BP_NetDisplayProxyInfo(P_BP_NETWORK_PROXIES proxy_info)
{

	// ensure we have proxy information
	if(!proxy_info)
		return ERR_FAILURE;

	// display proxy information
	printf("\n\t [+] Proxy Info: %p", proxy_info);
	printf("\n\t\t proxy_chain:    %p", proxy_info->proxy_chain);
	printf("\n\t\t proxy_chain_n:  %u", proxy_info->proxy_chain_n);
	printf("\n");

	// if we have no proxies in the chain, don't display them
	if(!proxy_info->proxy_chain || !proxy_info->proxy_chain_n)
		return ERR_SUCCESS;

	// walk chain elements and display proxy information
	size_t n = 0;
	for(; n < proxy_info->proxy_chain_n; n++)
	{

		// display the proxy as it's encountered
		BP_NetDisplayProxy(&proxy_info->proxy_chain[n]);

		printf("\n");
	}

	// return indicating success
	return ERR_SUCCESS;

}


// displays one proxy element
BP_ERROR_T BP_NetDisplayProxy(P_BP_NETWORK_PROXY proxy)
{

	// ensure we have a proxy
	if(!proxy)
		return ERR_FAILURE;


	// get proxy type string
	char * proxy_type = (char *) BP_NetProxyGetAsciiFromProxyType(proxy->type);

	// get network protocol string
	char * net_proto = (char *) BP_NetIPProtoToAscii(proxy->destination_proto);

	// proxy connect timestamp
	char * proxy_connect_ts = BP_TimeConvertTimeTToAscii(&proxy->connected_ts);

	// proxy created timestamp
	char * proxy_created_ts = BP_TimeConvertTimeTToAscii(&proxy->created_ts);


	printf("\n\t\t [+] Proxy: %p : %s", proxy, proxy->description);
	printf("\n");
	printf("\n\t\t\t description:         %s", proxy->description);
	printf("\n\t\t\t uuid:                %s", proxy->uuid);
	printf("\n\t\t\t type:                %s", proxy_type);
	printf("\n\t\t\t destination host:    %s", proxy->destination_host);
	printf("\n\t\t\t destination port:    %u", proxy->destination_port);
	printf("\n\t\t\t destination proto:   %s", net_proto);
	printf("\n\t\t\t proxy ip address:    %s", proxy->proxy_ip_addr);
	printf("\n\t\t\t proxy username:      %s", proxy->user);
	printf("\n\t\t\t proxy password:      %s", proxy->pass);
	printf("\n\t\t\t connected:           %s", (proxy->connected == BP_TRUE) ? "yes" : "no");
	printf("\n\t\t\t created_ts:          %s", proxy->created_ts   ? proxy_created_ts : "unset");
	printf("\n\t\t\t connected time:      %s", proxy->connected_ts ? proxy_connect_ts : "unset");
	printf("\n\t\t\t proxy response IP:   %s", proxy->proxy_response_ip);
	printf("\n\t\t\t proxy response port: %u", proxy->proxy_response_port);

	// destroy type string if necessary
	if(proxy_type)
	{
		bpfree(proxy_type);
	}

	if(net_proto)
	{
		bpfree(net_proto);
	}
	if(proxy_connect_ts)
	{
		bpfree(proxy_connect_ts);
	}
	if(proxy_created_ts)
	{
		bpfree(proxy_created_ts);
	}

	// return indicating success
	return ERR_SUCCESS;

}


// Displays SSL related information.
BP_ERROR_T BP_NetDisplaySSLOptions(P_NETWORK_SSL ssl_options)
{

	// ensure we have a pointer
	if(!ssl_options)
		return ERR_FAILURE;

	printf("\n\t [+] SSL Options:");
	printf("\n\t\t lock:    %p", &ssl_options->lock);
	printf("\n\t\t ssl:     %p", ssl_options->ssl);
	printf("\n\t\t bio:     %p", ssl_options->bio);
	printf("\n\t\t ctx:     %p", ssl_options->context);
	printf("\n\t\t method:  %p", ssl_options->method);
	printf("\n\t\t rsa_key: %p", ssl_options->rsaKey);

	// return indicating success
	return ERR_SUCCESS;

}




// This routine converts a proxy type to an ASCII character string.  Used
// primarily for displaying proxy information.
char * BP_NetProxyGetAsciiFromProxyType(BP_NETWORK_PROXY_TYPE proxy_type)
{

	// ensure we've gotten a proxy type (zero = unset)
	if(!proxy_type)
		return NULL;

	// switch on the proxy type
	switch(proxy_type)
	{

		// socks4a type
		case BP_NETWORK_PROXY_TYPE_SOCKS4A:
			return bpstrdup("BP_NETWORK_PROXY_TYPE_SOCKS4A");

		// socks5 type
		case BP_NETWORK_PROXY_TYPE_SOCKS5:
			return bpstrdup("BP_NETWORK_PROXY_TYPE_SOCKS5");

		// connect type
		case BP_NETWORK_PROXY_TYPE_CONNECT:
			return bpstrdup("BP_NETWORK_PROXY_TYPE_CONNECT");
			break;

		// default
		default:
			break;

	}


	// return null on failure
	return NULL;

}


// Converts an ip protocol to an ascii string.  String must be freed on return.
char * BP_NetIPProtoToAscii(size_t ip_proto)
{

	// ensure the protocol isn't at or above cap
	if(ip_proto >= IPPROTO_MAX)
		return NULL;

	switch(ip_proto)
	{

		/* Dummy protocol for TCP.  */
		case IPPROTO_IP:
			return strdup("IPPROTO_IP or IPPROTO_HOPOPTS or ZERO/UNSET");
			break;

		/* Internet Control Message Protocol.  */
		case IPPROTO_ICMP:
			return strdup("IPPROTO_ICMP");
			break;

		/* Internet Group Management Protocol. */
		case IPPROTO_IGMP:
			return strdup("IPPROTO_IGMP");
			break;

		/* IPIP tunnels (older KA9Q tunnels use 94).  */
		case IPPROTO_IPIP:
			return strdup("IPPROTO_IPIP");
			break;

		/* Transmission Control Protocol.  */
		case IPPROTO_TCP:
			return strdup("IPPROTO_TCP");
			break;

		/* Exterior Gateway Protocol.  */
		case IPPROTO_EGP:
			return strdup("IPPROTO_EGP");
			break;

		/* PUP protocol.  */
		case IPPROTO_PUP:
			return strdup("IPPROTO_PUP");
			break;

		/* User Datagram Protocol.  */
		case IPPROTO_UDP:
			return strdup("IPPROTO_UDP");
			break;

		/* XNS IDP protocol.  */
		case IPPROTO_IDP:
			return strdup("IPPROTO_IDP");
			break;

		/* SO Transport Protocol Class 4.  */
		case IPPROTO_TP:
			return strdup("IPPROTO_TP");
			break;

		/* Datagram Congestion Control Protocol.  */
		case IPPROTO_DCCP:
			return strdup("IPPROTO_DCCP");
			break;

		/* IPv6 header.  */
		case IPPROTO_IPV6:
			return strdup("IPPROTO_IPV6");
			break;

		/* IPv6 routing header.  */
		case IPPROTO_ROUTING:
			return strdup("IPPROTO_ROUTING");
			break;

		/* IPv6 fragmentation header.  */
		case IPPROTO_FRAGMENT:
			return strdup("IPPROTO_FRAGMENT");
			break;

		/* Reservation Protocol.  */
		case IPPROTO_RSVP:
			return strdup("IPPROTO_RSVP");
			break;

		/* General Routing Encapsulation.  */
		case IPPROTO_GRE:
			return strdup("IPPROTO_GRE");
			break;

		/* encapsulating security payload.  */
		case IPPROTO_ESP:
			return strdup("IPPROTO_ESP");
			break;

		/* authentication header.  */
		case IPPROTO_AH:
			return strdup("IPPROTO_AH");
			break;

		/* ICMPv6.  */
		case IPPROTO_ICMPV6:
			return strdup("IPPROTO_ICMPV6");
			break;

		/* IPv6 no next header.  */
		case IPPROTO_NONE:
			return strdup("IPPROTO_NONE");
			break;

		/* IPv6 destination options.  */
		case IPPROTO_DSTOPTS:
			return strdup("IPPROTO_DSTOPTS");
			break;

		/* Multicast Transport Protocol.  */
		case IPPROTO_MTP:
			return strdup("IPPROTO_MTP");
			break;

		/* Encapsulation Header.  */
		case IPPROTO_ENCAP:
			return strdup("IPPROTO_ENCAP");
			break;

		/* Protocol Independent Multicast.  */
		case IPPROTO_PIM:
			return strdup("IPPROTO_PIM");
			break;

		/* Compression Header Protocol.  */
		case IPPROTO_COMP:
			return strdup("IPPROTO_COMP");
			break;

		/* Stream Control Transmission Protocol.  */
		case IPPROTO_SCTP:
			return strdup("IPPROTO_SCTP");
			break;

		/* UDP-Lite protocol.  */
		case IPPROTO_UDPLITE:
			return strdup("IPPROTO_UDPLITE");
			break;

		/* Raw IP packets.  */
		case IPPROTO_RAW:
			return strdup("IPPROTO_RAW");
			break;

		// bad value
		default:
			break;
	}
}

// Checks to see if a TCP port is open on a host by attempting to connect to it directly.
BP_BOOL BP_NetTCPPortIsOpen(char *host, unsigned short port)
{

	// attempt to create new network socket
	P_BP_NETWORK_HANDLE new_sock = BP_NetCreateSocket
	(
		(char *) host,
		port,
		IPPROTO_TCP,
		BP_TRUE,
		NULL
	);

	// If the socket can be opened, it's open, so exit
	// immediately.
	if(new_sock)
	{
		BP_DestroyNetworkHandle(new_sock);
		return BP_FALSE;
	}

	// return true indicating that the port is open
	return BP_TRUE;

}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Proxy Connection Oriented %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// connect socks4a proxy
BP_ERROR_T BP_NetProxyConnectSOCKS4A(P_BP_NETWORK_HANDLE net_handle, P_BP_NETWORK_PROXY proxy)
{


	if(!net_handle)
		return ERR_FAILURE;

	if(!proxy)
		return ERR_FAILURE;

	if(!proxy->proxy_ip_addr)
		return ERR_FAILURE;

	if(!proxy->type)
		return ERR_FAILURE;

	if(!proxy->proxy_port)
		return ERR_FAILURE;


	// first we have to see if we've connected successfully yet
	if(net_handle->last_tcp_connect_succeeded != BP_TRUE)
	{

		// attempt to create a network socket
		if(!BP_NetCreateSocket(proxy->proxy_ip_addr, (short) proxy->proxy_port, IPPROTO_TCP, 0, net_handle))
			return ERR_FAILURE;

	}


	// create the greeting (returned as packet, correctly packed for send)
	P_BP_NETWORK_PACKET_DATA socks4_greeting = BP_NetProxyCreateSOCKS4AGreeting(net_handle, proxy);

    // create a new data list
    P_BP_DATA_LIST data_list = BP_CreateDataList("SOCKS4 Connect List");
    if(!data_list)
            return ERR_FAILURE;

    //  Send prepared socks4a greeting.
    BP_DataListAddEntry
    (
            data_list,
            "Send SOCKS4A Greeting",
            (BYTE *) socks4_greeting->data,
            socks4_greeting->data_n,
            BP_DATA_ENTRY_TYPE_BINARY,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
    );


    // Read socks 4 response.
    BP_DataListAddEntry
    (
            data_list,
            "Read Response",
            (BYTE *) NULL,
            100,
            BP_DATA_ENTRY_TYPE_READ,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
    );


    // set the default network callback in the list to use the network handle provided
    BP_DataListSetDefaultNetworkCallbacks(data_list, net_handle);

    // perform socket connect recv sequence
    BP_DataListActivateAllEntries(data_list);

    // display the data list to stdout
    BP_DisplayDataList(data_list);

    // if the second byte is not 0x5a (request granted) return indicating failure
    if(data_list->entries[1].data[1] != 0x5a)
    {

    	// destroy the datalist
    	BP_DestroyDataList(data_list);

    	// return indicating failure if we have a bad match value
    	return ERR_FAILURE;

    }

	// set connected flag to true
	proxy->connected = BP_TRUE;

    // destroy the datalist
    BP_DestroyDataList(data_list);

	// return indicating success
	return ERR_SUCCESS;

}


// connect socks5 proxy
BP_ERROR_T BP_NetProxyConnectSOCKS5(P_BP_NETWORK_HANDLE net_handle, P_BP_NETWORK_PROXY proxy)
{

	if(!net_handle)
		return ERR_FAILURE;
	if(!proxy)
		return ERR_FAILURE;
	if(!proxy->proxy_ip_addr)
		return ERR_FAILURE;
	if(!proxy->type)
		return ERR_FAILURE;
	if(!proxy->proxy_port)
		return ERR_FAILURE;


	// first we have to see if we've connected successfully yet



	// create the greeting (returned as packet, correctly packed for send)
	P_BP_NETWORK_PACKET_DATA socks5_greeting = BP_NetProxyCreateSOCKS5InitialNegotiation(net_handle, proxy);

    // create a new data list
    P_BP_DATA_LIST data_list = BP_CreateDataList("SOCKS4 Connect List");
    if(!data_list)
    {
    	return ERR_FAILURE;
    }

    //  Send prepared socks4a greeting.
    BP_DataListAddEntry
    (
            data_list,
            "Send SOCKS5 Greeting",
            (BYTE *) socks5_greeting->data,
            socks5_greeting->data_n,
            BP_DATA_ENTRY_TYPE_BINARY,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
    );


    // Read socks 5 response
    BP_DataListAddEntry
    (
            data_list,
            "Read Response",
            (BYTE *) NULL,
            100,
            BP_DATA_ENTRY_TYPE_READ,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
    );


    // set the default network callback in the list to use the network handle provided
    BP_DataListSetDefaultNetworkCallbacks(data_list, net_handle);

    // perform socket connect recv sequence
    BP_DataListActivateAllEntries(data_list);

    // display the data list to stdout
    BP_DisplayDataList(data_list);

    // destroy the datalist
    BP_DestroyDataList(data_list);




	// return indicating success
	return ERR_SUCCESS;

}


// generate a SOCKS4A Greeting Packet for Proxy
P_BP_NETWORK_PACKET_DATA BP_NetProxyCreateSOCKS4AGreeting(P_BP_NETWORK_HANDLE net_handle, P_BP_NETWORK_PROXY proxy)
{

	// ensure we have a handle and a proxy pointer
	if(!net_handle)
		return NULL;
	if(!proxy)
		return NULL;



	// greeting packet
	P_BP_NETWORK_PACKET_DATA greeting_packet = (P_BP_NETWORK_PACKET_DATA) bpcalloc(sizeof(BP_NETWORK_PACKET_DATA), 1);

	// ensure we created the structure
	if(!greeting_packet)
		return NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Packet using LinkL %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// LinkL is later flattened to become the packet.

	// create linkl for generating greeting (ez mode)
	P_BP_LINKL socks4_greeting = BP_CreateLinkL("socks4a greeting", BP_TRUE);

	// allocate one byte for version number
	BYTE * version_number     = (BYTE *)     tq_calloc(socks4_greeting, 1, 1, BPLN);

	// allocate one byte for command code
	BYTE * command_code       = (BYTE *)     tq_calloc(socks4_greeting, 1, 1, BPLN);

	// allocate two bytes for port number
	uint16_t * dst_port       = (uint16_t *) tq_calloc(socks4_greeting, 2, 1, BPLN);

	// allocate 4 bytes for dst_ip (should always be 0xff000000 (0.0.0.255)
	uint32_t * dst_ip         = (uint32_t *) tq_calloc(socks4_greeting, 4, 1, BPLN);




	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Perform Data Assignments %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// set the version number to 4
	*version_number = (BYTE) 0x04;

	// set the command to 0x01 (for connect)
	*command_code = (BYTE) 0x01;



	// set the destination IP (always bad value)
	*dst_ip = (uint32_t) 0xff000000;


	// set user id here if the user supplied one, if not, add a null byte (created via calloc)
	if(!proxy->user)
	{
		tq_calloc(socks4_greeting, 1, 1, BPLN);
	}
	else
	{
		tq_strdup(socks4_greeting, proxy->user, BPLN);
	}

	// set the destination host here if its provided in the structure, if not
	// assume that the destination is the next proxy in the chain
	if(proxy->destination_host)
	{

		// add the destination host
		tq_strdup(socks4_greeting, proxy->destination_host, BPLN);

		// set the destination port
		*dst_port = (uint16_t) htons(proxy->destination_port);

	}
	else
	{

		// Since the items were null, we assume that the next chain element is
		// the proxy destination.

		// first get proxy postion
		size_t n = 0;
		for(; n < net_handle->proxy_info->proxy_chain_n; n++)
			if(proxy == (P_BP_NETWORK_PROXY) &net_handle->proxy_info->proxy_chain[n])
				break;

		// ensure that the position is correct (must contain a next element)
		if(n < net_handle->proxy_info->proxy_chain_n-1)
		{

			// set destination from next element in chain
			tq_strdup(socks4_greeting, net_handle->proxy_info->proxy_chain[n+1].destination_host, BPLN);

			// set port from next element in chain
			*dst_port = (uint16_t) htons(net_handle->proxy_info->proxy_chain[n+1].destination_port);

		}


	}

	// Flatten the tail queue to generate socks4a greeting packet
	BP_LinkLFlattenRaw(socks4_greeting);


	// allocate space for returning buffer
	greeting_packet->data   = (BYTE *) bpcalloc(socks4_greeting->flat_buff_n, 1);

	// copy in data
	memcpy(greeting_packet->data, socks4_greeting->flat_buff, socks4_greeting->flat_buff_n);

	// set the size in the packet
	greeting_packet->data_n = socks4_greeting->flat_buff_n;


	// destroy the linkl
	BP_DestroyLinkL(socks4_greeting, BP_TRUE);

	// return the greeting packet
	return greeting_packet;

}


// generate a SOCKS5 Greeting Packet for Proxy
P_BP_NETWORK_PACKET_DATA BP_NetProxyCreateSOCKS5InitialNegotiation(P_BP_NETWORK_HANDLE net_handle, P_BP_NETWORK_PROXY proxy)
{

	// ensure we have a handle and a proxy pointer
	if(!net_handle)
		return NULL;
	if(!proxy)
		return NULL;


	// greeting packet
	P_BP_NETWORK_PACKET_DATA greeting_packet = (P_BP_NETWORK_PACKET_DATA) bpcalloc(sizeof(BP_NETWORK_PACKET_DATA), 1);

	// ensure we created the structure
	if(!greeting_packet)
		return NULL;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Create Packet using LinkL %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// LinkL is later flattened to become the packet.

	// create linkl for generating greeting (ez mode)
	P_BP_LINKL socks5_greeting = BP_CreateLinkL("socks5 greeting", BP_TRUE);

	// allocate one byte for version number
	BYTE * version_number     = (BYTE *)     tq_calloc(socks5_greeting, 1, 1, BPLN);

	// number of authentication methods supported
	BYTE * num_auth_methods_supported = (BYTE *) tq_calloc(socks5_greeting, 1, 1, BPLN);

	// set version number (5 for socks5)
	version_number[0] = (BYTE) 0x05;

	// set the number of methods supported (will always be 1 pass or no-pass)
	num_auth_methods_supported[0] = (BYTE) 0x01;

	// check if a username/password was provided
	BYTE * user_password_given = (BYTE *) NULL;
	if(proxy->user || proxy->pass)
	{

		// create space in buffer
		user_password_given = (BYTE *) tq_calloc(socks5_greeting, 1, 1, BPLN);

		// set the buffer
		user_password_given[0] = (BYTE) 0x02;

	}
	else // no user/password set
	{

		// create space in buffer
		user_password_given = (BYTE *) tq_calloc(socks5_greeting, 1, 1, BPLN);

		// set the buffer
		user_password_given[0] = (BYTE) 0x00;

	}


	// Flatten the tail queue to generate socks4a greeting packet
	BP_LinkLFlattenRaw(socks5_greeting);


	// allocate space for returning buffer
	greeting_packet->data   = (BYTE *) bpcalloc(socks5_greeting->flat_buff_n, 1);

	// copy in data
	memcpy(greeting_packet->data, socks5_greeting->flat_buff, socks5_greeting->flat_buff_n);

	// set the size in the packet
	greeting_packet->data_n = socks5_greeting->flat_buff_n;


	// destroy the linkl
	BP_DestroyLinkL(socks5_greeting, BP_TRUE);


	// return the greeting packet
	return greeting_packet;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Data List Network Read/Write Callbacks %%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// this assigns the provided data list with network callbacks oriented with the provided net_handle.  They
// can be overridden before entry activation to perform whatever action the user prefers but by default
// they utilize BP_NetRead and BP_NetWrite depending on the chunk type.
BP_ERROR_T BP_DataListSetDefaultNetworkCallbacks(P_BP_DATA_LIST data_list, P_BP_NETWORK_HANDLE net_handle)
{

	// ensure we have a datalist and a network handle
	if(!data_list)
		return ERR_FAILURE;
	if(!net_handle)
		return ERR_FAILURE;

	// reference pointer used in the loop below for
	// simply increasing code readability.
	P_BP_DATA_LIST_ENTRY entry = (P_BP_DATA_LIST_ENTRY) NULL;

	// walk the available entries, and plumb them with
	// the network oriented functions.
	size_t n = 0;
	for(; n < data_list->entries_n; n++)
	{

			// set the entry
			entry = &data_list->entries[n];

			// check entry type
			if(entry->data_type != BP_DATA_ENTRY_TYPE_READ)
			{
				entry->custom_write_params = (void *) net_handle;
				entry->custom_write = BP_DataListNetSend;
			}
			else
			{
				entry->custom_read_params = (void *) net_handle;
				entry->custom_read = BP_DataListNetRead;
			}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// typecast for the data list generic send function
void BP_DataListNetSend(void *data_list_entry, void *net_send_parameters)
{

	// ensure we have both data pointers
	if(!data_list_entry)
		return;
	if(!net_send_parameters)
		return;

	// set the network handle to use
	P_BP_NETWORK_HANDLE net_handle = (P_BP_NETWORK_HANDLE) net_send_parameters;

	// set the entry
	P_BP_DATA_LIST_ENTRY entry = (P_BP_DATA_LIST_ENTRY) data_list_entry;

	// read the data
	entry->last_write_size = BP_NetSend(net_handle, (char *) entry->data, entry->size);


	entry->last_write_successful = BP_TRUE;

	// exit
	return;

}

// typecast for the data list generic send function
void BP_DataListNetRead(void *data_list_entry, void *net_read_parameters)
{

	// ensure we have both data pointers
	if(!data_list_entry)
		return;
	if(!net_read_parameters)
		return;

	// set the network handle to use
	P_BP_NETWORK_HANDLE net_handle = (P_BP_NETWORK_HANDLE) net_read_parameters;

	// set the entry
	P_BP_DATA_LIST_ENTRY entry = (P_BP_DATA_LIST_ENTRY) data_list_entry;

	// read the data
	entry->last_read_size = BP_NetRecv(net_handle, (char *) entry->data, entry->size);

	// exit callback
	return;

}


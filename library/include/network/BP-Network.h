
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Enumerations and Definitions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// maximum backlog for listening connections is 255
// set to 127 for measure.
#define DEFAULT_BACKLOG 127

// network return values
enum {
	NETWORK_SUCCESS,
	CLASS_NETWORK_OBJECT_NULL_ERROR,
	SEND_BUFFER_NULL_ERROR,
	RECV_BUFFER_NULL_ERROR,
	SOCKET_ERROR
};

// enumerator defining connect sockets
enum {
	BP_NETWORK_CONNECT_SOCKET,
	BP_NETWORK_LISTEN_SOCKET
};


// This structure is usually returned via protocol configuration routines,
// such as those that generate contiguous packet data (aka headers, raw data
// etc)
typedef struct __BP_NETWORK_PACKET_DATA {

	// prepared data
	BYTE * data;

	// size of the data to be sent
	size_t data_n;

} BP_NETWORK_PACKET_DATA, *P_BP_NETWORK_PACKET_DATA;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Proxy Related Structures and Definitions %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// define a network proxy type indicator (size_t)
typedef size_t BP_NETWORK_PROXY_TYPE;

// provide individual types within an enumerator
typedef enum BP_NETWORK_PROXY_TYPE_ENUM
{
	BP_NETWORK_PROXY_TYPE_UNSET,
	BP_NETWORK_PROXY_TYPE_SOCKS4A,
	BP_NETWORK_PROXY_TYPE_SOCKS5,
	BP_NETWORK_PROXY_TYPE_CONNECT
};


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% SOCKS4A Structures %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// SOCKS4A Greeting header.  Must be followed by 2 more null terminated strings,
// the first string being the user id (can be a null byte if no user id), and then
// the domain name you wish to connect through.
typedef struct __BP_SOCKS4A_GREETING_HEADER {

	// first byte is a version number
	BYTE version_number;

	// second byte is a command code
	BYTE command_code;

	// next two bytes are the network ordered destination port
	uint16_t dst_port;

	// next two bytes are 0xff00000000 (bad ip 0.0.0.255)
	uint32_t dst_ip;

} BP_SOCKS4A_GREETING_HEADER, *P_BP_SOCKS4A_GREETING_HEADER;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Proxy Chaining %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// holds a network proxy
typedef struct __BP_NETWORK_PROXY {


	// randomly generated uuid
	char * uuid;

	// proxy description
	char * description;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Proxy Config Items %%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// type of proxy
	BP_NETWORK_PROXY_TYPE type;

	// Destination (required for a connection to be established)
	// this is the host that the proxy will be connecting to.  It is
	// typically sent as part of a connect greeting message.
	char * destination_host;

	// destination port to connect to
	uint16_t destination_port;

	// destination protocol to use (IPPROTO_TCP or IPPROTO_UDP). The
	// use of UDP is only supported by socks proxies.
	size_t destination_proto;

	// ip address to connect to
	char * proxy_ip_addr;

	// port to connect on
	unsigned short proxy_port;

	// user name to connect with if necessary
	char * user;

	// password to connect with, if necessary
	char * pass;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Status Flags %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// flag indicating whether or not the proxy connected
	// ok (BP_TRUE/BP_FALSE)
	size_t connected;

	// the timestamp of when a connection takes place
	time_t connected_ts;

	// created timestamp
	time_t created_ts;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Proxy Response Data %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// character string representing the ip the proxy
	// responded with.
	char *proxy_response_ip;

	// host byte order port
	uint16_t proxy_response_port;



} BP_NETWORK_PROXY, *P_BP_NETWORK_PROXY;



// This structure can be found within the P_NETWORK structure and is
// used as a primary resource for chains.
typedef struct __BP_NETWORK_PROXIES {

	// stack of proxy chain entries
	P_BP_NETWORK_PROXY proxy_chain;

	// number of proxies in the stack
	size_t proxy_chain_n;

} BP_NETWORK_PROXIES, *P_BP_NETWORK_PROXIES;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% SSL Structures %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This structure holds ssl related information.  It is used
// in conjunction with the network structure to provide ssl related
// functionality to network sockets.
typedef struct _NETWORK_SSL {


	// semaphore for locking the ssl structure
	sem_t lock;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% OpenSSL Library Structures %%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// main ssl structure
	SSL * ssl;

	// basic input/output object, used for mapping an existing socket
	// to a ssl connection.
	BIO * bio;

	// context to use (TLS, SSLv2, etc)
	SSL_CTX * context;

	// method to connect
	SSL_METHOD * method;

	// rsa key if necessary
	RSA * rsaKey;

}NETWORK_SSL, *P_NETWORK_SSL;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Network Structures %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Main network structure.  Holds all network related information.
typedef struct __BP_NETWORK_HANDLE {


	// semaphore for locking if necessary
	sem_t lock;

	// tail queue allocator to use when allocating
	// data within this network handle.
	P_BP_LINKL tq;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Connect Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// hostname which was connected on
	char *host;

	// IP address resolved
	char *ip_address;

	// port connected on
	uint16_t port;

	// if the last tcp listen succeeded this flag is set to
	// BP_TURE, else it will be BP_FALSE
	uint8_t last_tcp_listen_succeeded;

	// if the last tcp connect succeeded, this flag is set
	// to BP_TRUE, else it wil be BP_FALSE
	uint8_t last_tcp_connect_succeeded;

	// if the last tcp send was successful, this flag is set
	// to BP_TRUE, else it will be BP_FALSE
	uint8_t last_tcp_send_succeeded;

	// if the last tcp send was successful, this flag is set
	// to BP_TRUE, else it will be BP_FALSE
	uint8_t last_tcp_recv_succeeded;


	// if the last udp send succeeded, this flag is set
	// to BP_TRUE, else it wil be BP_FALSE
	uint8_t last_udp_send_succeeded;

	// if the last udp recv succeeded, this flag is set
	// to BP_TRUE, else it wil be BP_FALSE
	uint8_t last_udp_recv_succeeded;

	// if the last ssl connect succeeded, this flag is set
	// to BP_TRUE, else it will be BP_FALSE
	uint8_t last_ssl_connect_succeeded;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Socket Options %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// socket created
	int sock;

	// source information
	struct sockaddr_in source;

	// destination information
	struct sockaddr_in dest;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Utility Information %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// The time (in seconds since unix epoch) when this connection was
	// created.
	time_t handle_created_ts;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Recv / Send Counters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// --- Totals ----

	// this contains the total send counter
	uint64_t total_all_send_counter;

	// this contains the total recv
	uint64_t total_all_recv_counter;

	// total send counter
	uint64_t total_tcp_send_counter;

	// total recv counter
	uint64_t total_tcp_recv_counter;

	// total send counter
	uint64_t total_udp_send_counter;

	// total recv counter
	uint64_t total_udp_recv_counter;


	// total ssl send counter
	uint64_t total_ssl_send_counter;

	// total ssl recv counter
	uint64_t total_ssl_recv_counter;

	// --- Last ---

	// last send counter
	uint32_t last_send_counter;

	// last recv counter
	uint64_t last_recv_counter;

	// last ssl send counter
	uint32_t last_ssl_send_counter;

	// last ssl recv counter
	uint64_t last_ssl_recv_counter;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% SSL Options %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// ssl options
	P_NETWORK_SSL sslOpts;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Proxy Options %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// proxy related information
	P_BP_NETWORK_PROXIES proxy_info;

	// flag indicating if chain was successful
	uint8_t proxy_chain_established_successfully;

	// current number of proxy connections (max 65534)
	uint32_t current_proxy_depth;


} BP_NETWORK_HANDLE, *P_BP_NETWORK_HANDLE;



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Network Related Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Creates the NETWORK and NETWORK_SSL object tree
P_BP_NETWORK_HANDLE BP_CreateNetworkHandle();

// Destroys the NETWORK and NETWORK_SSL object tree
BP_ERROR_T BP_DestroyNetworkHandle(P_BP_NETWORK_HANDLE net_handle);

// Creates a socket of the specified type: IPPROTO_TCP or IPPROTO_UDP
P_BP_NETWORK_HANDLE BP_NetCreateSocket
(
	char              * netAddr,
	short               netPort,
	int                 proto,
	int                 setListen,
	P_BP_NETWORK_HANDLE net_handle_in
);

// Accepts on a listening socket
P_BP_NETWORK_HANDLE BP_NetAccept(P_BP_NETWORK_HANDLE net_handle);

// Reads data from a socket
int BP_NetRecv(P_BP_NETWORK_HANDLE net_handle, char * data, int size);

// writes data onto a socket
int BP_NetSend(P_BP_NETWORK_HANDLE net_handle, char * data, int size);

// writes data onto a socket
int BP_NetSendASCII(P_BP_NETWORK_HANDLE net_handle, char * data);

// Thread safe version of inet_ntoa.  Returns string on the heap rather
// than within a static buffer.
char * BP_NetInAddrToAscii(struct in_addr *in);

// Converts an ip protocol to an ascii string.  String must be freed on return.
char * BP_NetIPProtoToAscii(size_t ip_proto);

// Checks to see if a TCP port is open on localhost by attempting to connect to it directly.
BP_BOOL BP_NetTCPPortIsOpen(char *host, unsigned short port);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Network Handle Proxy Managment %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Adds a proxy to the network handles chain stack
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
);


// This checks a proxy chain to ensure that data can be sent across it.  This routine
// creates it's own network handle to accomplish this, and does not modifigy the handle
// passed in as a parameter, except to modify values found in the proxy_info structure.
BP_ERROR_T BP_NetProxyChainCheckWorking(P_BP_NETWORK_HANDLE net_handle);

// This routine attempts to connect all the proxies found within a provided network handle.
BP_ERROR_T BP_NetProxyConnectChain(P_BP_NETWORK_HANDLE net_handle);

// Reset flags for an individual proxy.  Does not clear configuration
// data, but only data which was set.
BP_ERROR_T BP_NetProxyResetFlags(P_BP_NETWORK_PROXY proxy);

// This routine will clear out all proxy flags which may have been set
// during previous connect calls.  This is typically used when tearing
// down established chains.
BP_ERROR_T BP_NetProxyResetAllProxyFlags(P_BP_NETWORK_HANDLE net_handle);

// This routine converts a proxy type to an ASCII character string.  Used
// primarily for displaying proxy information.
char * BP_NetProxyGetAsciiFromProxyType(BP_NETWORK_PROXY_TYPE proxy_type);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Network Oriented Display Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// displays the current network handle
BP_ERROR_T BP_NetDisplayHandle(P_BP_NETWORK_HANDLE net_handle);

// displays all elements within a proxy information structure.
BP_ERROR_T BP_NetDisplayProxyInfo(P_BP_NETWORK_PROXIES proxy_info);

// displays one proxy element
BP_ERROR_T BP_NetDisplayProxy(P_BP_NETWORK_PROXY proxy);

// displays ssl options
BP_ERROR_T BP_NetDisplaySSLOptions(P_NETWORK_SSL ssl_options);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Proxy Connection Oriented Routines %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// connect socks4a proxy
BP_ERROR_T BP_NetProxyConnectSOCKS4A(P_BP_NETWORK_HANDLE net_handle, P_BP_NETWORK_PROXY proxy);

// connect socks5 proxy
BP_ERROR_T BP_NetProxyConnectSOCKS5(P_BP_NETWORK_HANDLE net_handle, P_BP_NETWORK_PROXY proxy);

// generate a SOCKS4A Greeting Packet for Proxy
P_BP_NETWORK_PACKET_DATA BP_NetProxyCreateSOCKS4AGreeting(P_BP_NETWORK_HANDLE net_handle, P_BP_NETWORK_PROXY proxy);

// generate a SOCKS5 initial negotiation
P_BP_NETWORK_PACKET_DATA BP_NetProxyCreateSOCKS5InitialNegotiation(P_BP_NETWORK_HANDLE net_handle, P_BP_NETWORK_PROXY proxy);


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Data List Network Read/Write Callbacks %%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// this assigns the provided data list with network callbacks oriented with the provided net_handle.  They
// can be overridden before entry activation to perform whatever action the user prefers but by default
// they utilize BP_NetRead and BP_NetWrite depending on the chunk type.
BP_ERROR_T BP_DataListSetDefaultNetworkCallbacks(P_BP_DATA_LIST data_list, P_BP_NETWORK_HANDLE net_handle);

// typecast for the data list generic send function
void BP_DataListNetSend(void * data_list_entry, void *net_send_parameters);

// typecast for the data list generic send function
void BP_DataListNetRead(void * data_list_entry, void *net_read_parameters);




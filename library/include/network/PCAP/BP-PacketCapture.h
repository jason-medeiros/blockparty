#ifndef BP_PACKET_CAPTURE_H
#define BP_PACKET_CAPTURE_H


typedef u_int tcp_seq;

// Ethernet Header
#define ETHER_ADDR_LEN	6

/* Ethernet header */
typedef struct _BP_ETHERNET_HEADER {

		u_char ether_dhost[ETHER_ADDR_LEN]; /* Destination host address */

		u_char ether_shost[ETHER_ADDR_LEN]; /* Source host address */

		u_short ether_type; /* IP? ARP? RARP? etc */

} BP_ETHERNET_HEADER, *P_BP_ETHERNET_HEADER;


// IP/TCP Headers

#define IP_V(ip)    (((ip)->ip_vhl & 0xf0) >> 4)
#define IP_HL(ip)   ((ip)->ip_vhl & 0x0f)

#define IP_DF 0x4000                 /* dont fragment flag        */
#define IP_MF 0x2000                 /* more fragments flag       */
#define IP_OFFMASK 0x1fff            /* mask for fragmenting bits */

typedef struct _BP_IP_HEADER {

    // header length, version
    u_int8_t        ip_vhl;

    // type of service
    u_int8_t        ip_tos;

    // total length
    u_int16_t       ip_len;

    // identification
    u_int16_t       ip_id;

    // fragment offset field
    u_int16_t       ip_off;

    // time to live
    u_int8_t        ip_ttl;

    // protocol
    u_int8_t        ip_protocol;

    // ip checksum
    u_int16_t       ip_sum;

    // ip source and destination
    struct  in_addr ip_src;
    struct  in_addr ip_dst;

} BP_IP_HEADER, *P_BP_IP_HEADER;


#define TH_FIN 0x01
#define TH_SYN 0x02
#define TH_RST 0x04
#define TH_PUSH 0x08
#define TH_ACK 0x10
#define TH_URG 0x20
#define TH_ECE 0x40
#define TH_CWR 0x80

// TCP Header
typedef struct _BP_TCP_HEADER {

	// source port
    u_short th_sport;

    // destination port
    u_short th_dport;

	// tcp sequence number
    tcp_seq th_seq;

    // tcp acknowledgement number
    tcp_seq th_ack;

#if BYTE_ORDER == LITTLE_ENDIAN
    u_int th_x2:4,    /* (unused)    */
    th_off:4;         /* data offset */
#endif

#if BYTE_ORDER == BIG_ENDIAN
    u_int th_off:4,   /* data offset */
    th_x2:4;          /* (unused)    */
#endif

    // tcp flags
    u_char th_flags;

    // tcp window
    u_short th_win;

    // tcp checksum
    u_short th_sum;

    // urgent pointer
    u_short th_urp;

} BP_TCP_HEADER, *P_BP_TCP_HEADER ;



// Packet capture configuration structure
typedef struct _BP_PACKET_CAPTURE_CONF {


	// bpf filtering expression (compiled into bpf)
	char * filter_expression;

	// pcap device
	char *dev;

	// pcap error buffer
	char errbuf[PCAP_ERRBUF_SIZE];

	// pcap object
	pcap_t* descr;

	// packet data
	const u_char *packet;

	// packet header structure
	struct pcap_pkthdr hdr;

	// ethernet header structure
	struct ether_header *eptr;


	// network for filter
	bpf_u_int32 net;

	// network mask for filter
	bpf_u_int32 mask;

	// bpf compiled program
	struct bpf_program fp;

} BP_PACKET_CAPTURE_CONF, *P_BP_PACKET_CAPTURE_CONF;


// This filter structure is used with default packet filter
// included with blockparty.  Set values to your specification.
typedef struct _BP_PACKET_CAPTURE_FILTER{

	// packet capture conf
	P_BP_PACKET_CAPTURE_CONF captureConf;

	// log packet to file (opened/locked if this value is not null)
	char *logFilePath;

	// actual log file pointer
	FILE *logFile;

	// interactive mode (stops for each packet)
	bool interactiveMode;

} BP_PACKET_CAPTURE_FILTER, *P_BP_PACKET_CAPTURE_FILTER;

/*
 * This function starts a packet capture given a device, a berkley packet filter
 * and a filter callback function.  The callback function should be in the following
 * format.  Populate userData with datastructure of your choice in a custom filter, or
 * use P_BP_CAPTURE_FILTER for the default BP_CaptureFilter function.
 *
 * Example Callback: (provided in library)
 *
 * 		void BP_CaptureFilter(u_char *userData, const struct pcap_pkthdr* pkthdr, const u_char*  packet);
 *
 *
 * When packet is captured, that function will be called and packet will be processed by passing all parameters
 * to the Pfilter.
 *
 * Example Usage:
 *
 *		BP_StartPacketCapture("eth0", "not port 22", 150, BP_CaptureFilter,
 *
 */

int BP_StartPacketCapture(char * device, char * bpfilter, size_t maxPackets, pcap_handler filterCallback, void *userData);


/*
 * This is a default pcap callback function which can be used with BP_StartPacketCapture if no
 * capture is set.
 */

void BP_CaptureFilter(u_char *userData, const struct pcap_pkthdr* pkthdr, const u_char*  packet);

// prints out IP header data
void BP_PrintIPHeader(P_BP_IP_HEADER ipHeader);

// prints out tcp header data
//void BP_PrintTCPHeader(P_BP_TCP_HEADER tcpHeader);


#endif // BP_PACKET_CAPTURE_H

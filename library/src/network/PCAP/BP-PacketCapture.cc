#include "../../../include/BP-Main.h"

/*
 * Blockparty Packet Capture Utilities:
 * ------------------------------------------
 *
 */

/*
 * This is a default pcap callback function which can be used with BP_StartPacketCapture if no
 * capture is set.
 */

void BP_CaptureFilter(u_char *userData, const struct pcap_pkthdr* pkthdr, const u_char*  packet)
{
	if(pkthdr->len == 0 || pkthdr->len == sizeof(BP_ETHERNET_HEADER)){
		memset((void*) packet, 0x00, pkthdr->len);
		return;
	}

	// set capture filter
	P_BP_PACKET_CAPTURE_FILTER captureFilter = (P_BP_PACKET_CAPTURE_FILTER) userData;

	// if the log file path is set try to open/lock the file
	if(captureFilter->logFilePath && !captureFilter->logFile)
	{
		// open the file for appending
		captureFilter->logFile = fopen(captureFilter->logFilePath, "w+");

	}

	// if the log file is non-null, we can write the file
	if(captureFilter->logFile && !captureFilter->interactiveMode)
	{
		// write the header to the file
		fwrite(pkthdr, sizeof(struct pcap_pkthdr), 1, captureFilter->logFile);

		// write the packet to the file
		fwrite(packet, pkthdr->len, 1, captureFilter->logFile);

	}

	// Interactive:  keep/disgard/exit_pcap
	if(captureFilter->interactiveMode)
	{

		// flush out standard input
		fflush(stdin);
		fflush(stdout);


		// set ethernet header
		P_BP_ETHERNET_HEADER ethernetHeader = (P_BP_ETHERNET_HEADER) packet;

		// set ip header in packet
		P_BP_IP_HEADER  ipHeader  = (P_BP_IP_HEADER) &packet[sizeof(BP_ETHERNET_HEADER)];

		// set tcp header in packet
		P_BP_TCP_HEADER tcpHeader = (P_BP_TCP_HEADER) &packet[sizeof(BP_IP_HEADER)];

		// display ip header
		BP_PrintIPHeader(ipHeader);



		// print indicator message
		fprintf(stdout, "\nCaptured Packet: (k)eep,(d)isgard,e(x)it:");

		// get character choice from user
		switch(fgetc(stdin))
		{

			// keep
			case 'k':

				// write the header to the file
				fwrite(pkthdr, sizeof(struct pcap_pkthdr), 1, captureFilter->logFile);

				// write the packet to the file
				fwrite(packet, pkthdr->len, 1, captureFilter->logFile);

				break;

			// disgard (ie do nothing)
			case 'd':
				printf("\n");
				break;

			// exit pcap looping
			case 'e':
				pcap_breakloop(captureFilter->captureConf->descr);
				break;

			// default switch
			default:
				break;

		}

		memset((void*) packet, 0x00, pkthdr->len);

	}

}


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

int BP_StartPacketCapture(char * device, char * bpfilter, size_t maxPackets, pcap_handler filterCallback, void *userData){

    // allocate space for configuration structure
    P_BP_PACKET_CAPTURE_CONF pcapConf = (P_BP_PACKET_CAPTURE_CONF) bpcalloc(sizeof(BP_PACKET_CAPTURE_CONF), 1);
    if(!pcapConf)
		return ERR_FAILURE;


    // device name should never be longer then 64bytes i'd imagine.
    pcapConf->dev = bpstrndup(device, 64);

    // open the device
    pcapConf->descr = pcap_open_live( pcapConf->dev, BUFSIZ, 0, -1, pcapConf->errbuf);


    // if the interface cannot be opened, report why and exit function
    if(pcapConf->descr == NULL)
		return ERR_FAILURE;


	if (pcap_lookupnet(pcapConf->dev, &pcapConf->net, &pcapConf->mask, pcapConf->errbuf) == -1)
		return ERR_FAILURE;

	if (pcap_compile(pcapConf->descr, &pcapConf->fp, pcapConf->filter_expression, 0, pcapConf->net) == -1)
		return ERR_FAILURE;

	if (pcap_setfilter(pcapConf->descr, &pcapConf->fp) == -1)
		return ERR_FAILURE;

	// set the first ptrsize of userData to the pointer to our packet configuration
	void ** tmp = (void **) userData;

	if(userData)
		tmp[0] = pcapConf;

    // main pcap loop
	pcap_loop(pcapConf->descr, maxPackets, (pcap_handler) filterCallback, (u_char *) userData);

	// return indicating success
    return ERR_SUCCESS;

}

// prints out IP header data
void BP_PrintIPHeader(P_BP_IP_HEADER ipHeader)
{
	if(!ipHeader)
		return;

    // header length, version
    // u_int8_t        ip_vhl;
	printf("\n [+] -----------------IP Header ---------------------------- [+]");

    printf("\n          %s (src)  --> %s (dst) ", inet_ntoa(ipHeader->ip_src), inet_ntoa(ipHeader->ip_dst));
    printf("\n");

    // vhl and TOS
    printf("\n\t VHL:......0x%.08x   TOS: 0x%.08x", ipHeader->ip_vhl, ipHeader->ip_tos);


    // total length and ip id
    printf("\n\t Length: ..0x%.08x   ID:  0x%.08x", ipHeader->ip_len, ipHeader->ip_id);

    // fragment offset field & ttl
    printf("\n\t Offset: ..0x%.08x   TTL: 0x%.08x", ipHeader->ip_off, ipHeader->ip_ttl);


    // protocol
    // u_int8_t        ip_protocol;
    printf("\n\t Protocol:..%s (0x%x)", ipHeader->ip_protocol == IPPROTO_TCP ? "tcp" : "non-tcp", ipHeader->ip_protocol );

    // ip checksum
    // u_int16_t       ip_sum;
    printf("\n\t Checksum: .0x%.08x", ipHeader->ip_sum);

    // ip source and destination
    // struct  in_addr ip_src, ip_dst;


}

// prints out tcp header data
void BP_PrintTCPHeader(P_BP_TCP_HEADER tcpHeader)
{
	if(!tcpHeader)
			return;

}


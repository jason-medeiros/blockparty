// include main project build functionality
#include "../../include/BP-Main.h"



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Websocket Accept Key Calculator %%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// This will return a variable length base64 key.  The "client_nonce_base64" value
// is the value you get from Sec-WebSocket-Key from the client request.  This process
// is specified in the websocket RFC on page 8 (as of right 2017), and documented in
// the stackoverflow answer: http://stackoverflow.com/questions/34778321/websocket-handshake-sec-websocket-accept-header-value-is-incorrect
//
// Result is bpcalloced and must be bpfree'd.
//
//
char * BP_HTTPdWebsocketCalculateSecAcceptKey
(
	char *client_nonce_base64
)
{

	// run basic checks
	if(!client_nonce_base64)
		return NULL;
	if(!bpstrlen(client_nonce_base64))
		return NULL;
	if(!BP_StringIsReasonablePrintableString(client_nonce_base64, bpstrlen(client_nonce_base64), BP_FALSE, BP_FALSE))
		return NULL;


	// join the nonce and the rfc GUID string
	char *joined_tmp = BP_JoinStrings
	(
		client_nonce_base64,
		"258EAFA5-E914-47DA-95CA-C5AB0DC85B11",
		NULL
	);

	// ensure we joined the strings successfully
	if(!joined_tmp)
		return NULL;


	// generate sha1
	char * sha1_ascii = BP_GenSHA1(joined_tmp, bpstrlen(joined_tmp));
	if(!sha1_ascii)
	{
		bpfree(joined_tmp);
		return NULL;
	}

	// create binary blob from the sha1 string
	size_t bin_len = 0;
	char *bin_str = BP_StringHexToBinaryBlob(sha1_ascii, &bin_len);
	if(!bin_str)
	{
		bpfree(joined_tmp);
		bpfree(sha1_ascii);
		return NULL;
	}


	char *calculated_key = BP_Base64Encode((unsigned char *) bin_str, bin_len);
	if(!calculated_key)
	{
		bpfree(joined_tmp);
		bpfree(sha1_ascii);
		bpfree(bin_str);
		return NULL;

	}

	// return indicating NULL
	return calculated_key;

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% Frame Decoders %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// This routine will decode a frame header based on raw frame data.  It takes data
// from a raw buffer and translates it into a union bitfield which can be used
// directly.
BP_ERROR_T BP_HTTPdWebsocketDecodeFrameHeader
(
		unsigned char *                           raw_frame_data,
		size_t                                    raw_frame_data_len,
		P_BP_HTTPD_WEBSOCKET_DECODED_RECV_FRAME_HEADER empty_header_in_out
)
{

	// run basic checks
	if(!raw_frame_data)
		return ERR_FAILURE;
	if(!empty_header_in_out)
		return ERR_FAILURE;

	// always zero the frame header before use
	BP_ZERO(empty_header_in_out, sizeof(BP_HTTPD_WEBSOCKET_DECODED_RECV_FRAME_HEADER));

	// if the data is too short to decode data, mark it and exit
	if(raw_frame_data_len < 2)
	{
		empty_header_in_out->data_len_is_too_short_to_decode_header = BP_TRUE;
		empty_header_in_out->decoded_ok = BP_FALSE;
		return ERR_FAILURE;
	}

	/*
	printf("\n frame data 0 in bits:");
	BP_PrintBitsInByte(raw_frame_data[0]);
	printf("\n frame data 1 in bits:");make
	BP_PrintBitsInByte(raw_frame_data[1]);
	*/

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Check Basic Frame Flags (first byte) %%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	if(BP_CHECK_BIT_IS_SET(raw_frame_data[0], 7))
	{
		empty_header_in_out->fin_flag = BP_TRUE;
	}

	if(BP_CHECK_BIT_IS_SET(raw_frame_data[0], 6))
	{
		empty_header_in_out->rsv_1_flag = BP_TRUE;
	}

	if(BP_CHECK_BIT_IS_SET(raw_frame_data[0], 5))
	{
		empty_header_in_out->rsv_2_flag = BP_TRUE;
	}

	if(BP_CHECK_BIT_IS_SET(raw_frame_data[0], 4))
	{
		empty_header_in_out->rsv_3_flag = BP_TRUE;
	}



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Calculate Opcode (first byte) %%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	empty_header_in_out->opcode = 0;

	if(BP_CHECK_BIT_IS_SET(raw_frame_data[0], 3))
	{
		empty_header_in_out->opcode |= (1u << 3);
	}

	if(BP_CHECK_BIT_IS_SET(raw_frame_data[0], 2))
	{
		empty_header_in_out->opcode |= (1u << 2);
	}

	if(BP_CHECK_BIT_IS_SET(raw_frame_data[0], 1))
	{
		empty_header_in_out->opcode |= (1u << 1);
	}

	if(BP_CHECK_BIT_IS_SET(raw_frame_data[0], 0))
	{
		empty_header_in_out->opcode |= (1u << 0);
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Calculate Mask (second byte) %%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(BP_CHECK_BIT_IS_SET(raw_frame_data[1], 7))
	{
		empty_header_in_out->mask_flag = BP_TRUE;
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Calculate Payload Length (second byte) %%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// This is set to the byte position in which we expect
	// to find our mask.  It is set based on our payload length.
	size_t mask_offset_in_buff = 0;

	// set payload length
	empty_header_in_out->payload_len = 0;

	if(BP_CHECK_BIT_IS_SET(raw_frame_data[1], 0))
		empty_header_in_out->payload_len |= (1u << 0);
	if(BP_CHECK_BIT_IS_SET(raw_frame_data[1], 1))
		empty_header_in_out->payload_len |= (1u << 1);
	if(BP_CHECK_BIT_IS_SET(raw_frame_data[1], 2))
		empty_header_in_out->payload_len |= (1u << 2);
	if(BP_CHECK_BIT_IS_SET(raw_frame_data[1], 3))
		empty_header_in_out->payload_len |= (1u << 3);
	if(BP_CHECK_BIT_IS_SET(raw_frame_data[1], 4))
		empty_header_in_out->payload_len |= (1u << 4);
	if(BP_CHECK_BIT_IS_SET(raw_frame_data[1], 5))
		empty_header_in_out->payload_len |= (1u << 5);
	if(BP_CHECK_BIT_IS_SET(raw_frame_data[1], 6))
		empty_header_in_out->payload_len |= (1u << 6);

	// If the data length is only 2, only decode the first
	// two bytes.
	//if(raw_frame_data_len == 2)
	//	return ERR_SUCCESS;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Calculate Extended Payload Length  %%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// If the payload length is less than 126, we can
	// go ahead and set the mask offset without extracting
	// any extra data
	if(empty_header_in_out->payload_len < 126)
	{
		mask_offset_in_buff = 2;
	}
	else if(empty_header_in_out->payload_len == 126)
	{

		// If the raw frame data length is less than four,
		// we can't decode the frame and need to read more
		// data.
		if(raw_frame_data_len < 4)
		{
			empty_header_in_out->data_len_is_too_short_to_decode_header = BP_TRUE;
			return ERR_FAILURE;
		}

		// reset all bits in the payload length since
		empty_header_in_out->payload_len = 0;

		// process byte 2 as extended payload length
		if(BP_CHECK_BIT_IS_SET(raw_frame_data[3], 0))
			empty_header_in_out->payload_len |= (1u << 0);
		if(BP_CHECK_BIT_IS_SET(raw_frame_data[3], 1))
			empty_header_in_out->payload_len |= (1u << 1);
		if(BP_CHECK_BIT_IS_SET(raw_frame_data[3], 2))
			empty_header_in_out->payload_len |= (1u << 2);
		if(BP_CHECK_BIT_IS_SET(raw_frame_data[3], 3))
			empty_header_in_out->payload_len |= (1u << 3);
		if(BP_CHECK_BIT_IS_SET(raw_frame_data[3], 4))
			empty_header_in_out->payload_len |= (1u << 4);
		if(BP_CHECK_BIT_IS_SET(raw_frame_data[3], 5))
			empty_header_in_out->payload_len |= (1u << 5);
		if(BP_CHECK_BIT_IS_SET(raw_frame_data[3], 6))
			empty_header_in_out->payload_len |= (1u << 6);
		if(BP_CHECK_BIT_IS_SET(raw_frame_data[3], 7))
			empty_header_in_out->payload_len |= (1u << 7);

		// process byte 3 as extended payload length
		if(BP_CHECK_BIT_IS_SET(raw_frame_data[2], 0))
			empty_header_in_out->payload_len |= (1u << 8);
		if(BP_CHECK_BIT_IS_SET(raw_frame_data[2], 1))
			empty_header_in_out->payload_len |= (1u << 9);
		if(BP_CHECK_BIT_IS_SET(raw_frame_data[2], 2))
			empty_header_in_out->payload_len |= (1u << 10);
		if(BP_CHECK_BIT_IS_SET(raw_frame_data[2], 3))
			empty_header_in_out->payload_len |= (1u << 11);
		if(BP_CHECK_BIT_IS_SET(raw_frame_data[2], 4))
			empty_header_in_out->payload_len |= (1u << 12);
		if(BP_CHECK_BIT_IS_SET(raw_frame_data[2], 5))
			empty_header_in_out->payload_len |= (1u << 13);
		if(BP_CHECK_BIT_IS_SET(raw_frame_data[2], 6))
			empty_header_in_out->payload_len |= (1u << 14);
		if(BP_CHECK_BIT_IS_SET(raw_frame_data[2], 7))
			empty_header_in_out->payload_len |= (1u << 15);

		// If we're using the 126 version, and the mask flag is set
		// our start position is going to be byte 4.
		if(empty_header_in_out->mask_flag)
			mask_offset_in_buff = 4;

	}
	else if(empty_header_in_out->payload_len == 127)
	{

		printf("\n\n");
		printf("\n Using 127 header here?");
		printf("\n\n");

		// If the raw frame data length is less than ten (8 byte length member),
		// we can't decode the frame and need to read more
		// data.
		if(raw_frame_data_len < 10)
		{
			empty_header_in_out->data_len_is_too_short_to_decode_header = BP_TRUE;
			return ERR_FAILURE;
		}

		// reset all bits in the payload length since
		empty_header_in_out->payload_len = 0;

		// get payload length pointer
		unsigned char * payload_len_ptr = (unsigned char *) &empty_header_in_out->payload_len;

		// set payload through raw byte re-arrangment
		payload_len_ptr[7] = (unsigned char) raw_frame_data[2];
		payload_len_ptr[6] = (unsigned char) raw_frame_data[3];
		payload_len_ptr[5] = (unsigned char) raw_frame_data[4];
		payload_len_ptr[4] = (unsigned char) raw_frame_data[5];
		payload_len_ptr[3] = (unsigned char) raw_frame_data[6];
		payload_len_ptr[2] = (unsigned char) raw_frame_data[7];
		payload_len_ptr[1] = (unsigned char) raw_frame_data[8];
		payload_len_ptr[0] = (unsigned char) raw_frame_data[9];

		// If we're using the 127 version, and the mask flag is set
		// our start position is going to be byte 10.
		if(empty_header_in_out->mask_flag)
			mask_offset_in_buff = 10;

	}

	// check if our data length is too small
	if(empty_header_in_out->mask_flag && ((mask_offset_in_buff+4) > raw_frame_data_len))
	{
		empty_header_in_out->data_len_is_too_short_to_decode_header = BP_TRUE;
		return ERR_FAILURE;
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Now Extract XOR Mask Value %%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(empty_header_in_out->mask_flag && mask_offset_in_buff)
	{

		// copy in the masking key
		memcpy(&empty_header_in_out->masking_key, &raw_frame_data[mask_offset_in_buff], 4);

		// we may need to htonl here but not sure yet

	}

	// set the payload offset
	empty_header_in_out->payload_offset_from_frame_start = mask_offset_in_buff + 4;

	// set header length as payload offset (same value, added for code clarity)
	empty_header_in_out->header_len = empty_header_in_out->payload_offset_from_frame_start;

	// set remainder as total length since there has been no actual reading yet
	empty_header_in_out->payload_bytes_remaining_for_read = empty_header_in_out->payload_len;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Final Checks %%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// reserved bits should never be set
	if
	(
			empty_header_in_out->rsv_1_flag ||
			empty_header_in_out->rsv_2_flag ||
			empty_header_in_out->rsv_3_flag
	)
	{

		// mark reserved header flags as set
		empty_header_in_out->reserved_header_flags_are_set = BP_TRUE;

	}



	// if we have a bad opcode, exit immediatley
	if(empty_header_in_out->opcode >= BP_HTTPD_WEBSOCKET_FRAME_OPCODE_INVALID)
		empty_header_in_out->opcode_is_invalid_or_reserved = BP_TRUE;


	// check opcode
	switch(empty_header_in_out->opcode)
	{

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_CONTINUATION:
			empty_header_in_out->opcode_is_valid = BP_TRUE;
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_TEXT:
			empty_header_in_out->opcode_is_valid = BP_TRUE;
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_BINARY:
			empty_header_in_out->opcode_is_valid = BP_TRUE;
			break;

		// --- reserved non-control frames --

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_1_NON_CONTROL:
			empty_header_in_out->opcode_is_invalid_or_reserved = BP_TRUE;
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_2_NON_CONTROL:
			empty_header_in_out->opcode_is_invalid_or_reserved = BP_TRUE;
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_3_NON_CONTROL:
			empty_header_in_out->opcode_is_invalid_or_reserved = BP_TRUE;
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_4_NON_CONTROL:
			empty_header_in_out->opcode_is_invalid_or_reserved = BP_TRUE;
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_5_NON_CONTROL:
			empty_header_in_out->opcode_is_invalid_or_reserved = BP_TRUE;
			break;


		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_CONNECTION_CLOSE:
			empty_header_in_out->opcode_is_valid = BP_TRUE;
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_PING:
			empty_header_in_out->opcode_is_valid = BP_TRUE;
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_PONG:
			empty_header_in_out->opcode_is_valid = BP_TRUE;
			break;


		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_1_CONTROL:
			empty_header_in_out->opcode_is_invalid_or_reserved = BP_TRUE;
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_2_CONTROL:
			empty_header_in_out->opcode_is_invalid_or_reserved = BP_TRUE;
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_3_CONTROL:
			empty_header_in_out->opcode_is_invalid_or_reserved = BP_TRUE;
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_4_CONTROL:
			empty_header_in_out->opcode_is_invalid_or_reserved = BP_TRUE;
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_5_CONTROL:
			empty_header_in_out->opcode_is_invalid_or_reserved = BP_TRUE;
			break;

		default:
			break;


	}

	// exit if we have bad
	if
	(
		empty_header_in_out->opcode_is_invalid_or_reserved ||
		empty_header_in_out->opcode_is_invalid_or_reserved ||
		empty_header_in_out->reserved_header_flags_are_set
	)
	{
		printf("\n Invalid opcode or reserved flag is set?");

		BP_HTTPdWebsocketDisplayDecodedFrameHeader(	empty_header_in_out	);

		return ERR_FAILURE;
	}


	// mark header as decoded ok
	empty_header_in_out->decoded_ok = BP_TRUE;



	// return indicating success
	return ERR_SUCCESS;

}


// Decode frame data using a frame header.
BP_ERROR_T BP_HTTPdWebsocketDecodeFrameData
(
		P_BP_HTTPD_WEBSOCKET_DECODED_RECV_FRAME_HEADER frame_header,
		unsigned char                                 *data,
		size_t                                         data_len,
		P_BP_LINKL                                     data_aggregate_tq
)
{

	// basic parameter checks
	if(!frame_header)
		return ERR_FAILURE;
	if(!data)
		return ERR_FAILURE;
	if(!data_len)
		return ERR_FAILURE;
	if(!frame_header->decoded_ok)
	{
		printf("\n Could not decode due to header not decoded ok");
		return ERR_FAILURE;
	}
	if(!data_aggregate_tq)
	{
		printf("\n Could not decode due to missing data aggregator");
		return ERR_FAILURE;
	}

	// iterate through data
	size_t n = 0;
	for(n = 0; n < data_len; n++)
	{

		// perform decryption
		data[n] = data[n] ^ frame_header->masking_key[frame_header->last_decode_byte_idx % 4];

		// increment the decode byte index (this will be preserved throughout the full decode)
		frame_header->last_decode_byte_idx++;

	}

	// allocate data bucket
	void *tmp_data_bucket = tq_malloc(data_aggregate_tq, data_len, BPLN);

	// copy in the data
	memcpy(tmp_data_bucket, data, data_len);

	// add data length read
	frame_header->payload_bytes_read += data_len;

	// bytes remaining for read
	frame_header->payload_bytes_remaining_for_read = frame_header->payload_len - frame_header->payload_bytes_read;

	// mark the last decode length
	frame_header->last_payload_decode_data_len = data_len;


	// set marker if we completed our read
	if(frame_header->payload_bytes_read == frame_header->payload_len)
	{
		frame_header->payload_finished_reading = BP_TRUE;
	}



	// return indicating success
	return ERR_SUCCESS;

}

// display decoded frame header
BP_ERROR_T BP_HTTPdWebsocketDisplayDecodedFrameHeader
(
	P_BP_HTTPD_WEBSOCKET_DECODED_RECV_FRAME_HEADER decoded_frame_header
)
{

	// ensure we have a httpd
	if(!decoded_frame_header)
		return ERR_FAILURE;

	// set human readable opcode
	char *human_readable_opcode = (char *) NULL;

	// check opcode
	switch(decoded_frame_header->opcode)
	{

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_CONTINUATION:
			human_readable_opcode = "BP_HTTPD_WEBSOCKET_FRAME_OPCODE_CONTINUATION";
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_TEXT:
			human_readable_opcode = "BP_HTTPD_WEBSOCKET_FRAME_OPCODE_TEXT";
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_BINARY:
			human_readable_opcode = "BP_HTTPD_WEBSOCKET_FRAME_OPCODE_BINARY";
			break;

		// --- reserved non-control frames --

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_1_NON_CONTROL:
			human_readable_opcode = "BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_1_NON_CONTROL";
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_2_NON_CONTROL:
			human_readable_opcode = "BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_2_NON_CONTROL";
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_3_NON_CONTROL:
			human_readable_opcode = "BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_3_NON_CONTROL";
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_4_NON_CONTROL:
			human_readable_opcode = "BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_4_NON_CONTROL";
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_5_NON_CONTROL:
			human_readable_opcode = "BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_5_NON_CONTROL";
			break;


		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_CONNECTION_CLOSE:
			human_readable_opcode = "BP_HTTPD_WEBSOCKET_FRAME_OPCODE_CONNECTION_CLOSE";
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_PING:
			human_readable_opcode = "BP_HTTPD_WEBSOCKET_FRAME_OPCODE_PING";
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_PONG:
			human_readable_opcode = "BP_HTTPD_WEBSOCKET_FRAME_OPCODE_PONG";
			break;


		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_1_CONTROL:
			human_readable_opcode = "BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_1_CONTROL";
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_2_CONTROL:
			human_readable_opcode = "BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_2_CONTROL";
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_3_CONTROL:
			human_readable_opcode = "BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_3_CONTROL";
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_4_CONTROL:
			human_readable_opcode = "BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_4_CONTROL";
			break;

		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_5_CONTROL:
			human_readable_opcode = "BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_5_CONTROL";
			break;

		default:
			human_readable_opcode = "UNKNOWN_WEBSOCKET_OPCODE";
			break;


	}


	printf("\n [+] decoded_frame_header %p", decoded_frame_header);
	printf("\n\t  frame data:");
	printf("\n\t\t fin_flag:                               %u", decoded_frame_header->fin_flag);
	printf("\n\t\t rsv_1_flag:                             %u", decoded_frame_header->rsv_1_flag);
	printf("\n\t\t rsv_2_flag:                             %u", decoded_frame_header->rsv_2_flag);
	printf("\n\t\t rsv_3_flag:                             %u", decoded_frame_header->rsv_3_flag);
	printf("\n\t\t opcode:                                 %s (%u)", human_readable_opcode, decoded_frame_header->opcode);
	printf("\n\t\t mask_flag:                              %u", decoded_frame_header->mask_flag);
	printf("\n\t\t payload_len:                            %llu", decoded_frame_header->payload_len);
	printf("\n\t\t masking_key:                            [%.02x][%.02x][%.02x][%.02x]", decoded_frame_header->masking_key[0], decoded_frame_header->masking_key[1],decoded_frame_header->masking_key[2],decoded_frame_header->masking_key[3]);
	printf("\n\t\t payload_offset_from_frame_start:        %llu", decoded_frame_header->payload_offset_from_frame_start);
	printf("\n\t\t payload_bytes_read:                     %llu", decoded_frame_header->payload_bytes_read);
	printf("\n\t\t payload_bytes_remaining_for_read:       %llu", decoded_frame_header->payload_bytes_remaining_for_read);
	printf("\n\t\t last_payload_decode_data_len:           %u", decoded_frame_header->last_payload_decode_data_len);
	printf("\n\t\t last_decode_byte_idx:                   %u", decoded_frame_header->last_decode_byte_idx);
	printf("\n\t\t reserved_header_flags_are_set:          %u", decoded_frame_header->reserved_header_flags_are_set);
	printf("\n\t\t is_control_frame:                       %u", decoded_frame_header->is_control_frame);
	printf("\n\t\t decoded_ok:                             %u", decoded_frame_header->decoded_ok);
	printf("\n\t\t opcode_is_valid:                        %u", decoded_frame_header->opcode_is_valid);
	printf("\n\t\t opcode_is_invalid_or_reserved:          %u", decoded_frame_header->opcode_is_invalid_or_reserved);
	printf("\n\t\t payload_finished_reading:               %u", decoded_frame_header->payload_finished_reading);
	printf("\n\t\t data_len_is_too_short_to_decode_header: %u", decoded_frame_header->data_len_is_too_short_to_decode_header);



	// return indicating success
	return ERR_SUCCESS;

}

// --- frame encoding ----------

// This will encode a send frame header based around configured flags within the structure.  The
// structure itself holds the encoded data and a flag indicating whether encode was successful.  This
// INTENTIONALLY can create bad frame headers if you pass in bad data.  This is for the purpose of
// creating headers for fuzzers which may be doing protocol testing.
BP_ERROR_T BP_HTTPdWebsocketEncodeSendFrameHeader
(
	P_BP_HTTPD_WEBSOCKET_ENCODED_SEND_FRAME_HEADER frame_header_in_out
)
{

	// ensure we have a frame header to work with
	if(!frame_header_in_out)
		return ERR_FAILURE;

	// --- reset any encoded header that might be in place ----------

	// zero the encoded header
	BP_ZERO_STACK(frame_header_in_out->encoded_header);

	// set the header length to zero
	frame_header_in_out->encoded_header_full_length = 0;

	// size encoding has finished
	size_t encode_idx = 0;



	// --- set initial flags ------------------

	// set fin flag if necessary
	if(frame_header_in_out->fin_flag)
	{
		BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 8);
	}

	// set rsv 1 if necessary
	if(frame_header_in_out->rsv_1_flag)
	{
		BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 7);
	}

	// set rsv 2 if necessary
	if(frame_header_in_out->rsv_2_flag)
	{
		BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 6);
	}

	// set rsv 3 if necessary
	if(frame_header_in_out->rsv_3_flag)
	{
		BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 5);
	}


	// --- set opcode -----------------------

	// set opcode using bit configuration
	switch(frame_header_in_out->opcode)
	{

		// 0: opcode continuation
		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_CONTINUATION:
			// zero position requires no bit modification
			break;

		// 1: opcode text
		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_TEXT:
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 1);
			break;

		// 2: opcode binary
		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_BINARY:
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 2);
			break;

		// 3: opcode non control 1
		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_1_NON_CONTROL:
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 1);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 2);
			break;

		// 4: opcode non control 2
		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_2_NON_CONTROL:
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 3);
			break;

		// 5: opcode non control 3
		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_3_NON_CONTROL:
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 3);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 1);
			break;

		// 6: opcode non control 4
		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_4_NON_CONTROL:
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 3);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 2);
			break;

		// 7: opcode non control 5
		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_5_NON_CONTROL:
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 3);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 2);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 1);
			break;

		// 8: connection close
		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_CONNECTION_CLOSE:
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 4);
			break;

		// 9: ping
		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_PING:
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 4);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 1);
			break;

		// 10: pong
		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_PONG:
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 4);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 2);
			break;

		// 11: rsv control 1
		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_1_CONTROL:
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 4);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 2);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 1);
			break;

		// 12: rsv control 2
		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_2_CONTROL:
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 4);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 3);
			break;

		// 13: rsv control 3
		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_3_CONTROL:
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 4);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 3);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 1);
			break;

		// 14: rsv control 4
		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_4_CONTROL:
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 4);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 3);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 2);
			break;

		// 15: rsv control 5
		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_RSV_5_CONTROL:
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 4);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 3);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 2);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 1);
			break;

		// 16: invalid / default
		case BP_HTTPD_WEBSOCKET_FRAME_OPCODE_INVALID:
		default:
			frame_header_in_out->header_encoded_ok = BP_FALSE;
			return ERR_FAILURE;


	}

	// increment the encode index at this point
	encode_idx++;

	// --- add mask flag if necessary ------------

	if(frame_header_in_out->mask_flag)
	{
		BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 8);
	}



	// --- encode payload ------------------------



	// check if the payload length is less than 126, if so encode it here
	if(frame_header_in_out->payload_len < 126)
	{


		// frame_header_in_out->payload_len
		if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 0))
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 1);
		if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 1))
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 2);
		if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 2))
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 3);
		if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 3))
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 4);
		if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 4))
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 5);
		if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 5))
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 6);
		if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 6))
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 7);


		// set the finished offset
		encode_idx++;

	}
	else
	if(frame_header_in_out->payload_len >= 126)
	{

		// if the value is less than
		if(frame_header_in_out->payload_len <= (uint16_t) -1)
		{

			// -- first, set length to 126 -------------------

			// frame_header_in_out->payload_len
			// BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 1);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 2);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 3);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 4);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 5);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 6);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 7);

			// increment the encoder index
			encode_idx++;

			// frame_header_in_out->payload_len
			if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 8))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 1);
			if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 9))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 2);
			if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 10))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 3);
			if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 11))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 4);
			if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 12))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 5);
			if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 13))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 6);
			if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 14))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 7);
			if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 15))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 8);

			// set the finished offset
			encode_idx++;

			// frame_header_in_out->payload_len
			if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 0))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 1);
			if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 1))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 2);
			if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 2))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 3);
			if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 3))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 4);
			if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 4))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 5);
			if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 5))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 6);
			if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 6))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 7);
			if(BP_CHECK_BIT_IS_SET(frame_header_in_out->payload_len, 7))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 8);

			// set the finished offset
			encode_idx++;

		}
		else
		if(frame_header_in_out->payload_len <= (uint64_t) -1)
		{

			printf("\n Performing 64bit decode");

			// set the encoded header directly (127 == 8byte payload)
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 1);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 2);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 3);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 4);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 5);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 6);
			BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 7);

			// increment the encoder index
			encode_idx++;

			// set payload length as buffer
			unsigned char * payload_len_as_buff = (unsigned char *) &frame_header_in_out->payload_len;

			// payload buff index
			size_t payload_buff_idx = 7;

			// -- byte 1 ---

			// set bits as necessary
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 0))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 1);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 1))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 2);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 2))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 3);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 3))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 4);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 4))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 5);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 5))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 6);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 6))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 7);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 7))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 8);

			payload_buff_idx--;
			encode_idx++;

			// -- byte 2 ---

			// set bits as necessary
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 0))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 1);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 1))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 2);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 2))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 3);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 3))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 4);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 4))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 5);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 5))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 6);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 6))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 7);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 7))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 8);

			payload_buff_idx--;
			encode_idx++;

			// -- byte 3 ---

			// set bits as necessary
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 0))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 1);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 1))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 2);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 2))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 3);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 3))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 4);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 4))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 5);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 5))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 6);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 6))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 7);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 7))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 8);

			payload_buff_idx--;
			encode_idx++;

			// -- byte 4 ---

			// set bits as necessary
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 0))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 1);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 1))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 2);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 2))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 3);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 3))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 4);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 4))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 5);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 5))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 6);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 6))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 7);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 7))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 8);

			payload_buff_idx--;
			encode_idx++;

			// -- byte 5 ---

			// set bits as necessary
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 0))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 1);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 1))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 2);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 2))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 3);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 3))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 4);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 4))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 5);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 5))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 6);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 6))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 7);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 7))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 8);

			payload_buff_idx--;
			encode_idx++;


			// -- byte 6 ---

			// set bits as necessary
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 0))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 1);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 1))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 2);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 2))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 3);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 3))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 4);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 4))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 5);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 5))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 6);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 6))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 7);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 7))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 8);

			payload_buff_idx--;
			encode_idx++;


			// -- byte 7 ---

			// set bits as necessary
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 0))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 1);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 1))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 2);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 2))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 3);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 3))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 4);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 4))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 5);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 5))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 6);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 6))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 7);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 7))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 8);

			payload_buff_idx--;
			encode_idx++;

			// -- byte 8 ---

			// set bits as necessary
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 0))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 1);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 1))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 2);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 2))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 3);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 3))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 4);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 4))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 5);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 5))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 6);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 6))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 7);
			if(BP_CHECK_BIT_IS_SET(payload_len_as_buff[payload_buff_idx], 7))
				BP_SET_FLAG(frame_header_in_out->encoded_header[encode_idx], 8);

			payload_buff_idx--;
			encode_idx++;


		}

	}


	// --- set masking key --------------------


	// set masking key if necessary
	if(frame_header_in_out->mask_flag)
	{

		// set masking key in place
		frame_header_in_out->encoded_header[encode_idx]   = frame_header_in_out->masking_key[0];
		frame_header_in_out->encoded_header[encode_idx+1] = frame_header_in_out->masking_key[1];
		frame_header_in_out->encoded_header[encode_idx+2] = frame_header_in_out->masking_key[2];
		frame_header_in_out->encoded_header[encode_idx+3] = frame_header_in_out->masking_key[3];

		// move the offset up 4 bytes
		encode_idx+=4;

	}


	// encode header ok
	frame_header_in_out->header_encoded_ok = BP_TRUE;

	// set finished offset
	frame_header_in_out->encoded_header_full_length = encode_idx; // size_finished_offset;

	// return indicating success
	return ERR_SUCCESS;

}

// Use masking key to encode send data.  Data will be encoded IN PLACE so ensure
// to supply data you are fine with being modified (pre-copy if necessary).  The
// masking key modulo position is stored within the P_BP_HTTPD_WEBSOCKET_ENCODED_SEND_FRAME_HEADER
// structure.
BP_ERROR_T BP_HTTPdWebsocketEncodeSendData
(
	P_BP_HTTPD_WEBSOCKET_ENCODED_SEND_FRAME_HEADER frame_header_in_out,
	unsigned char                                 *data_to_encode_in_out,
	size_t                                         data_to_encode_size
)
{

	// run basic checks
	if(!frame_header_in_out)
		return ERR_FAILURE;
	if(!data_to_encode_in_out)
		return ERR_FAILURE;
	if(!data_to_encode_size)
		return ERR_FAILURE;

	// Never encode/mask data unless the mask flag is set.
	if(frame_header_in_out->mask_flag == BP_TRUE)
	{

		// iterate through data
		size_t n = 0;
		for(n = 0; n < data_to_encode_size; n++)
		{

			// perform xor encoding
			data_to_encode_in_out[n] = data_to_encode_in_out[n] ^ frame_header_in_out->masking_key[frame_header_in_out->last_payload_encode_byte_idx % 4];

			// increment the encode byte index
			frame_header_in_out->last_payload_encode_byte_idx++;

		}

	}

	// return indicating success
	return ERR_SUCCESS;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%% TO BE DETERMINED / UNFINISHED %%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



// This will add a websocket type request handler that will allow us handle websockets as they come in.
BP_ERROR_T BP_HTTPdRequestHandlerAddWebsocket
(
		P_BP_HTTPD_SESSION                       httpd,
		char *                                   handler_path,
		BP_HTTPdRequestHandlerFptr               handler,
		P_BP_HTTPD_REQUEST_HANDLER_SETUP_CONFIGS setup_configs,
		void *                                   external_handler_copy_parameter,
		size_t                                   external_handler_copy_parameter_size_n,
		BP_HTTPdLoginFormCallback                login_form_cb
)
{


	// return indicating success
	return ERR_SUCCESS;

}

// --- Create Plaintext Sockets --------------

// create websocket
P_BP_HTTPD_WEBSOCKET BP_HTTPdCreateWebsocket
(
	P_BP_HTTPD_SESSION                       httpd,
	char                                    *websocket_app_name,
	BP_HTTPD_WEBSOCKET_TYPE                  socket_type,
	P_BP_HTTPD_REQUEST                       request,
	char                                    *calculated_websocket_accept_key,
	char                                    *url,
	P_BP_HTTPD_WEBSOCKET_CONNECTION_POLICY   initial_connection_policy,
	size_t                                   max_read_len,
	size_t                                   max_epoll_events,
	P_BP_WEBSOCKET_FRAME_READER_CB           frame_reader_cb,
	P_BP_WEBSOCKET_READ_FRAME_CB             read_frame_cb,
	P_BP_WEBSOCKET_SEND_FRAME_CB             send_frame_cb,
	P_BP_WEBSOCKET_EVENT_LOOP_START_CB       event_loop_start_cb,
	P_BP_WEBSOCKET_EVENT_LOOP_STOP_CB        event_loop_stop_cb,
	P_BP_WEBSOCKET_CONNECTION_ERROR_CB       connection_err_cb
)
{

	// run null checks
	if(!httpd)
		return NULL;
	if(!websocket_app_name)
		return NULL;
	if(!socket_type)
		return NULL;
	if(!request)
		return NULL;
	if(!calculated_websocket_accept_key)
		return NULL;
	if(!url)
		return NULL;
	if(!initial_connection_policy)
		return NULL;
	if(!frame_reader_cb)
		return NULL;


	// create websocket pointer
	P_BP_HTTPD_WEBSOCKET websocket = (P_BP_HTTPD_WEBSOCKET) NULL;

	// create new hash table registry
	P_BP_HASH_TABLE_REGISTRY hreg = BP_CreateHashTableRegistry();
	if(!hreg)
		return NULL;


	// allocate space for the websocket
	websocket = (P_BP_HTTPD_WEBSOCKET) ht_calloc(hreg, sizeof(BP_HTTPD_WEBSOCKET), 1);

	// ensure we have a websocket
	if(!websocket)
	{
		BP_DestroyHashTableRegistry(hreg);
		return NULL;
	}

	// set hash registry within websocket
	websocket->hreg = hreg;

	// set the websocket application name (determines where to route messages)
	websocket->websocket_app_name = ht_strdup(websocket->hreg, websocket_app_name);

	// copy in the connection policy since it's just a linear structure
	memcpy(&websocket->connection_policy, initial_connection_policy, sizeof(BP_HTTPD_WEBSOCKET_CONNECTION_POLICY));

	// set httpd
	websocket->httpd = httpd;



	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Populate Values %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// generate random uuid and store it
	BP_GenRandomUUID_StoreInBuff((char *) &websocket->uuid);

	// set the socket type
	websocket->type = socket_type;

	// set state as pending connection
	websocket->state = BP_HTTPD_WEBSOCKET_STATE_PENDING_CONNECTION;

	// set callbacks
	websocket->frame_reader_cb     = frame_reader_cb;
	websocket->read_frame_cb       = read_frame_cb;
	websocket->send_frame_cb       = send_frame_cb;
	websocket->event_loop_start_cb = event_loop_start_cb;
	websocket->event_loop_stop_cb  = event_loop_stop_cb;
	websocket->connection_err_cb   = connection_err_cb;

	// set epoll and read limits
	websocket->max_epoll_events = max_epoll_events;
	websocket->max_read_len     = max_read_len;

	// set request in websocket
	websocket->request = request;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Mark the Request Where Necessary %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// mark the request as a websocket
	request->is_websocket_and_request_destruction_must_be_handled_externally = BP_TRUE;

	// return indicating success
	return websocket;


}

// --- Send Data on Websocket ----------------

// Send data on a websocket using a raw preconfigured header.  This
// requires you to preconfigure a P_BP_HTTPD_WEBSOCKET_ENCODED_SEND_FRAME_HEADER
// and pass it in as a parameter.  This routine is used for raw sends and
// situational protocol fuzzing (for security testing).
BP_ERROR_T BP_HTTPdWebsocketSendRaw
(
	P_BP_HTTPD_WEBSOCKET                           websocket,
	P_BP_HTTPD_WEBSOCKET_ENCODED_SEND_FRAME_HEADER raw_frame_header,
	unsigned char                                 *send_data,
	size_t                                         send_data_size,
	size_t                                         send_chunk_max_size,
	int                                            additional_tcp_flags
)
{

	// check required parameters
	if(!websocket)
		return ERR_FAILURE;
	if(!raw_frame_header)
		return ERR_FAILURE;
	if(!send_chunk_max_size)
		return ERR_FAILURE;


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Send Frame Only if No Data Is Sent %%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// if we're not sending data, send the header only
	if(!send_data || !send_data_size)
	{

		// check for parameter mismatches
		if(!send_data && send_data_size)
			return ERR_FAILURE;
		if(send_data && !send_data_size)
			return ERR_FAILURE;

		// ensure we have a header length
		if(!raw_frame_header->encoded_header_full_length)
			return ERR_FAILURE;

		// ensure the header length isn't too long
		if(raw_frame_header->encoded_header_full_length > sizeof(raw_frame_header->encoded_header))
			return ERR_FAILURE;

		// ensure the header is larger than the chunk size
		if(raw_frame_header->encoded_header_full_length > send_chunk_max_size)
			return ERR_FAILURE;

		// attempt to send results (header alone does not need to be encoded)
		ssize_t send_result = send(websocket->sock, (void *) &raw_frame_header->encoded_header, raw_frame_header->encoded_header_full_length, additional_tcp_flags);
		if(!send_result)
			return ERR_FAILURE;
		if(send_result == -1)
			return ERR_FAILURE;

		// return indicating success
		return ERR_SUCCESS;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Send Header and Data in Single Send %%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// create linkl for capping send buffers
	P_BP_LINKL send_linkl = (P_BP_LINKL) NULL;

	// if we're sending the header + payload all in one chunk, do it now
	if((raw_frame_header->encoded_header_full_length + send_data_size) <= send_chunk_max_size)
	{

		// exit immediately if we have any sort of data mismatch
		if(!send_data || !send_data_size)
			return ERR_FAILURE;

		// create the send linkl
		send_linkl = BP_CreateLinkL(".", BP_FALSE);

		// add data to the send frame
		tq_memdup(send_linkl, (void *) &raw_frame_header->encoded_header,  raw_frame_header->encoded_header_full_length,    BPLN);

		// send chunk
		void * send_chunk = tq_memdup(send_linkl, (void *)  send_data,  send_data_size, BPLN);

		// encode send data
		BP_HTTPdWebsocketEncodeSendData(raw_frame_header, (unsigned char *) send_chunk, send_data_size);

		// flatten the linkl after joining the buffers
		BP_LinkLFlattenRaw(send_linkl);


		// send the data immediately
		retry_send_header:
		ssize_t sent_data_size = send(websocket->sock, send_linkl->flat_buff, send_linkl->flat_buff_n, additional_tcp_flags);

		// if the send failed, return -1
		if(sent_data_size == -1)
		{

			// try to resend if the socket is blocking, sending a small amount (this is in
			// the case that the send buffer is full)
			if(errno == EAGAIN || errno == EWOULDBLOCK)
			{
				usleep(1000);
				goto retry_send_header;
			}

			// destroy the joined data
			BP_DestroyLinkL(send_linkl, BP_TRUE);
			return ERR_FAILURE;
		}

		// ensure the bytes sent were the bytes expected to be sent
		if(sent_data_size != send_linkl->flat_buff_n)
		{
			// destroy the joined data
			BP_DestroyLinkL(send_linkl, BP_TRUE);
			return ERR_FAILURE;
		}

		// return indicating success
		BP_DestroyLinkL(send_linkl, BP_TRUE);
		return ERR_SUCCESS;

	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Send in Chunks if Necessary %%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// exit immediately if we have any sort of data mismatch
	if(!send_data || !send_data_size)
		return ERR_FAILURE;

	// current chunk size
	uint64_t curr_chunk_size = 0;

	// since we know that the data size is greater than
	uint64_t total_send_chunk_count = send_data_size / send_chunk_max_size;

	// remainder chunk size
	uint64_t send_chunk_remainder = send_data_size % send_chunk_max_size;

	// current index into the send buffer to use when reading
	uint64_t send_buff_idx = 0;

	// add chunk send iter
	uint64_t chunk_send_iter = 0;

	// mark the header as having been sent
	BP_BOOL header_was_sent = BP_FALSE;

	// current chunk to send
	uint64_t current_chunk_to_send_size = 0;

	// long named variable, used for calculating how much space remains
	// in a send chunk for adding data
	uint64_t send_remainder_bytes_for_adding_data_after_header;

	// set the total remainder bytes
	uint64_t total_send_remainder_bytes = send_data_size + raw_frame_header->encoded_header_full_length;

	// attempt to send data as chunks
	while(total_send_remainder_bytes)
	{

		// set current chunk to send size
		current_chunk_to_send_size = 0;



		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Send Header with Data %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// if the header wasn't sent, go ahead and add it
		if(!header_was_sent)
		{

			// create the linkl
			send_linkl = BP_CreateLinkL(".", BP_FALSE);

			// we should only be sending headers + data when
			if(send_buff_idx != 0)
			{
				BP_DestroyLinkL(send_linkl, BP_TRUE);
				return ERR_FAILURE;
			}

			// add header length
			current_chunk_to_send_size += raw_frame_header->encoded_header_full_length;

			// add header to tailqueue
			tq_memdup(send_linkl, (void *) &raw_frame_header->encoded_header, raw_frame_header->encoded_header_full_length, BPLN);

			// calculate remainder length
			current_chunk_to_send_size += send_chunk_max_size - raw_frame_header->encoded_header_full_length;

			// calculate remainder chunk size
			size_t remainder_chunk_size = send_chunk_max_size - current_chunk_to_send_size;

			// duplicate from the remainder
			void *send_chunk = tq_memdup(send_linkl, (void *)  send_data, remainder_chunk_size, BPLN);

			// encode send data
			BP_HTTPdWebsocketEncodeSendData(raw_frame_header, (unsigned char *) send_chunk, remainder_chunk_size);

			// add the chunk size, to send size
			current_chunk_to_send_size += remainder_chunk_size;



			// -- flatten and send ---

			// flatten the linkl to create final run data
			BP_LinkLFlattenRaw(send_linkl);

			// send the data immediately
			retry_send_header_and_chunk:
			ssize_t sent_data_size = send(websocket->sock, send_linkl->flat_buff, send_linkl->flat_buff_n, additional_tcp_flags);

			// if the send failed, return -1
			if(sent_data_size == -1)
			{

				// try to resend if the socket is blocking, sending a small amount (this is in
				// the case that the send buffer is full)
				if(errno == EAGAIN || errno == EWOULDBLOCK)
				{
					usleep(1000);
					goto retry_send_header_and_chunk;
				}

				// destroy the joined data
				BP_DestroyLinkL(send_linkl, BP_TRUE);
				return ERR_FAILURE;

			}

			// ensure the bytes sent were the bytes expected to be sent
			if(sent_data_size != send_linkl->flat_buff_n)
			{
				// destroy the joined data
				BP_DestroyLinkL(send_linkl, BP_TRUE);
				return ERR_FAILURE;
			}

			// mark header as having been sent
			header_was_sent = BP_TRUE;

			// set send buffer index
			send_buff_idx = sent_data_size - send_linkl->flat_buff_n;

			// total send remainder bytes
			total_send_remainder_bytes -= send_linkl->flat_buff_n;

			// destroy the linkl if we have one
			BP_DestroyLinkL(send_linkl, BP_TRUE);
			send_linkl = NULL;

		}

		// if we have no remainder bytes to send, break
		if(!total_send_remainder_bytes)
			break;
		if(!header_was_sent)
			break;


		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// %%% Send Full Chunk if Required %%%%%%%%%%%%%%%%%%%%%%%%%
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// send a full chunk here
		if(total_send_remainder_bytes >= send_chunk_max_size)
		{

			// set read position for code clarity
			unsigned char *read_pos = &send_data[send_buff_idx];

			// create send chunk
			unsigned char send_chunk[send_chunk_max_size+1];
			BP_ZERO_STACK(send_chunk);

			// copy data into local stack buffer
			memcpy(&send_chunk, read_pos, send_chunk_max_size);

			// encode send data
			BP_HTTPdWebsocketEncodeSendData(raw_frame_header, (unsigned char *) &send_chunk, send_chunk_max_size);

			// send the encoded data immediately
			retry_send_full_chunk:
			ssize_t sent_data_size = send(websocket->sock, (unsigned char *) &send_chunk, send_chunk_max_size, additional_tcp_flags);

			// if the send failed, return -1
			if(sent_data_size == -1)
			{

				// try to resend if the socket is blocking, sending a small amount (this is in
				// the case that the send buffer is full)
				if(errno == EAGAIN || errno == EWOULDBLOCK)
				{
					usleep(1000);
					goto retry_send_full_chunk;
				}
				return ERR_FAILURE;
			}

			// ensure the bytes sent were the bytes expected to be sent
			if(sent_data_size != send_chunk_max_size)
			{
				return ERR_FAILURE;
			}

			// increment send index
			send_buff_idx += send_chunk_max_size;

			// subtract from send chunk max size
			total_send_remainder_bytes -= send_chunk_max_size;

		}

		// if we have no remainder bytes to send, break
		if(!total_send_remainder_bytes)
			break;
		if(!header_was_sent)
			break;

		// if we have only a trailing set of remainder bytes, send them here
		if(total_send_remainder_bytes < send_chunk_max_size)
		{

			// set read position for code clarity
			unsigned char *read_pos = &send_data[send_buff_idx];

			// create send chunk
			unsigned char send_chunk[total_send_remainder_bytes+1];
			BP_ZERO_STACK(send_chunk);

			// copy data into local stack buffer
			memcpy(&send_chunk, read_pos, total_send_remainder_bytes);

			// encode send data
			BP_HTTPdWebsocketEncodeSendData(raw_frame_header, (unsigned char *) &send_chunk, total_send_remainder_bytes);

			// send the data immediately
			retry_send_remainder:
			ssize_t sent_data_size = send(websocket->sock, (unsigned char *) &send_chunk, total_send_remainder_bytes, additional_tcp_flags);

			// if the send failed, return -1
			if(sent_data_size == -1)
			{

				// try to resend if the socket is blocking, sending a small amount (this is in
				// the case that the send buffer is full)
				if(errno == EAGAIN || errno == EWOULDBLOCK)
				{
					usleep(1000);
					goto retry_send_remainder;
				}
				return ERR_FAILURE;
			}

			// ensure the bytes sent were the bytes expected to be sent
			if(sent_data_size != send_chunk_max_size)
			{
				return ERR_FAILURE;
			}

			// increment send index
			send_buff_idx += total_send_remainder_bytes;

			// subtract from send chunk max size
			total_send_remainder_bytes -= total_send_remainder_bytes;

		}

		// if we have no remainder bytes to send, break
		if(!total_send_remainder_bytes)
			break;
		if(!header_was_sent)
			break;

	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%% Run Checks before Exiting %%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// return indicating success
	return ERR_SUCCESS;

}


// Send text data over a websocket.  Text data will be
// marked with a text opocode set within the frame/header when sent.
BP_ERROR_T BP_HTTPdWebsocketSend
(
	P_BP_HTTPD_WEBSOCKET            websocket,
	BP_HTTPD_WEBSOCKET_FRAME_OPCODE opcode,
	char                           *send_data,
	size_t                          send_data_size,
	size_t                          send_chunk_max_size,
	int                             additional_tcp_flags,
	BP_BOOL                         perform_masking_before_send
)
{

	// run basic checks
	if(!websocket)
		return ERR_FAILURE;
	if(!opcode)
		return ERR_FAILURE;
	if(!send_data)
		return ERR_FAILURE;
	if(!send_data_size)
		return ERR_FAILURE;
	if(!send_chunk_max_size)
		return ERR_FAILURE;


	// --- encode send frame -----

	// create a raw header to send
	BP_HTTPD_WEBSOCKET_ENCODED_SEND_FRAME_HEADER header_to_send;
	BP_ZERO_STACK(header_to_send);

	// set fin flag
	header_to_send.fin_flag   = BP_TRUE;

	// set opcode
	header_to_send.opcode    = opcode;

	// set mask flag and masking key if necessary
	if(perform_masking_before_send == BP_TRUE)
	{

		// ensure mask flag is enabled
		header_to_send.mask_flag = BP_TRUE;

		// create random masking key
		header_to_send.masking_key[0] = BP_GenerateRandomUChar(0x00, 0xff);
		header_to_send.masking_key[1] = BP_GenerateRandomUChar(0x00, 0xff);
		header_to_send.masking_key[2] = BP_GenerateRandomUChar(0x00, 0xff);
		header_to_send.masking_key[3] = BP_GenerateRandomUChar(0x00, 0xff);

	}

	// set the payload length
	header_to_send.payload_len = (uint64_t) send_data_size;

	// encode the send frame header
	BP_HTTPdWebsocketEncodeSendFrameHeader(	&header_to_send	);

	// attempt to send encoded data
	BP_HTTPdWebsocketSendRaw
	(
		websocket,
		&header_to_send,
		(unsigned char *) send_data,
		send_data_size,
		send_chunk_max_size,
		additional_tcp_flags
	);

	// return indicating success
	return ERR_SUCCESS;

}

// This attempts to send a file over a websocket
BP_ERROR_T BP_HTTPdWebsocketSendFromFile
(
	P_BP_HTTPD_WEBSOCKET            websocket,
	BP_HTTPD_WEBSOCKET_FRAME_OPCODE opcode,
	char                           *send_file_path,
	size_t                          file_read_and_send_chunk_max_size,
	int                             additional_tcp_flags,
	BP_BOOL                         perform_masking_before_send
)
{

	// run basic checks
	if(!websocket)
		return ERR_FAILURE;
	if(!opcode)
		return ERR_FAILURE;
	if(!send_file_path)
		return ERR_FAILURE;
	if(!file_read_and_send_chunk_max_size)
		return ERR_FAILURE;

	// create our read and send buffer
	unsigned char read_and_send_buff[file_read_and_send_chunk_max_size+1];
	BP_ZERO_STACK(read_and_send_buff);

	// ensure the file path points to a file
	if(!BP_StringIsReasonableForUnixEXT4Filename(send_file_path, bpstrlen(send_file_path), BP_TRUE))
		return ERR_FAILURE;

	// attempt to read file size
	size_t file_size = BP_FGetSize(send_file_path);

	if(!file_size)
		return ERR_FAILURE;

	// attempt to open file
	FILE *file_to_read_from = fopen(send_file_path, "r");
	if(!file_to_read_from)
		return ERR_FAILURE;



	// --- encode send frame -----

	// create a raw header to send
	BP_HTTPD_WEBSOCKET_ENCODED_SEND_FRAME_HEADER header_to_send;
	BP_ZERO_STACK(header_to_send);

	// set fin flag
	header_to_send.fin_flag   = BP_TRUE;

	// set opcode
	header_to_send.opcode    = opcode;

	// set masking flag if necessary
	if(perform_masking_before_send == BP_TRUE)
	{

		// ensure mask flag is enabled
		header_to_send.mask_flag = BP_TRUE;

		// create random masking key
		header_to_send.masking_key[0] = BP_GenerateRandomUChar(0x00, 0xff);
		header_to_send.masking_key[1] = BP_GenerateRandomUChar(0x00, 0xff);
		header_to_send.masking_key[2] = BP_GenerateRandomUChar(0x00, 0xff);
		header_to_send.masking_key[3] = BP_GenerateRandomUChar(0x00, 0xff);

	}

	// set the payload length
	header_to_send.payload_len = (uint64_t) file_size;

	// encode the send frame header
	BP_HTTPdWebsocketEncodeSendFrameHeader(	&header_to_send	);

	// send header
	send_header:
	ssize_t send_header_result = send(websocket->sock, &header_to_send.encoded_header, header_to_send.encoded_header_full_length, additional_tcp_flags);
	if(send_header_result == -1)
	{
		if(errno == EAGAIN || errno == EWOULDBLOCK)
		{
			usleep(1000);
			goto send_header;
		}

		// return indicating failure if we can't send our header
		fclose(file_to_read_from);
		return ERR_FAILURE;

	}

	// set the number of bytes read
	size_t bytes_read = 0;

	// read bytes from file
	while((bytes_read = fread(&read_and_send_buff, 1, sizeof(read_and_send_buff)-1, file_to_read_from)) > 0)
	{

		// encode the data before sending
		BP_HTTPdWebsocketEncodeSendData(&header_to_send, (unsigned char *) &read_and_send_buff, bytes_read);

		// send data
		send_data:
		ssize_t send_data_result = send(websocket->sock, &read_and_send_buff, bytes_read, additional_tcp_flags);
		if(send_header_result == -1)
		{
			if(errno == EAGAIN || errno == EWOULDBLOCK)
			{
				usleep(1000);
				goto send_data;
			}

			// return indicating failure if we can't send our header
			fclose(file_to_read_from);
			return ERR_FAILURE;

		}

		// zero out the buffer on loop
		BP_ZERO_STACK(read_and_send_buff);

	}

	// zero out sent buffer
	BP_ZERO_STACK(read_and_send_buff);

	// return indicating success
	fclose(file_to_read_from);
	return ERR_SUCCESS;

}

// --- Send Special Data on Websocket -------

// Send websocket protocol connection close frame.
BP_ERROR_T BP_HTTPdWebsocketSendConnectionClose
(
		P_BP_HTTPD_WEBSOCKET websocket,
		int                  additional_tcp_flags
)
{

	// run basic checks
	if(!websocket)
		return ERR_FAILURE;

	// create a raw header to send
	BP_HTTPD_WEBSOCKET_ENCODED_SEND_FRAME_HEADER header_to_send;
	BP_ZERO_STACK(header_to_send);

	// set fin flag
	header_to_send.fin_flag   = BP_TRUE;

	// set connection close
	header_to_send.opcode = BP_HTTPD_WEBSOCKET_FRAME_OPCODE_CONNECTION_CLOSE;

	// encode the send frame header
	BP_HTTPdWebsocketEncodeSendFrameHeader(	&header_to_send	);

	// attempt to send encoded data
	BP_HTTPdWebsocketSendRaw
	(
		websocket,
		&header_to_send,
		(unsigned char *) NULL,
		0,
		BP_KILOBYTE,
		additional_tcp_flags
	);

	// return indicating success
	return ERR_SUCCESS;

}

// Send websocket protocol ping.
BP_ERROR_T BP_HTTPdWebsocketSendPing
(
		P_BP_HTTPD_WEBSOCKET websocket,
		int                  additional_tcp_flags
)
{
	// run basic checks
	if(!websocket)
		return ERR_FAILURE;

	// create a raw header to send
	BP_HTTPD_WEBSOCKET_ENCODED_SEND_FRAME_HEADER header_to_send;
	BP_ZERO_STACK(header_to_send);

	// set fin flag
	header_to_send.fin_flag   = BP_TRUE;

	// set ping
	header_to_send.opcode = BP_HTTPD_WEBSOCKET_FRAME_OPCODE_PING;

	// encode the send frame header
	BP_HTTPdWebsocketEncodeSendFrameHeader(	&header_to_send	);

	// attempt to send encoded data
	BP_HTTPdWebsocketSendRaw
	(
		websocket,
		&header_to_send,
		(unsigned char *) NULL,
		0,
		BP_KILOBYTE,
		additional_tcp_flags
	);

	// increase the ping send count
	websocket->pings_sent++;

	// return indicating success
	return ERR_SUCCESS;

}

// Send websocket protocol pong.
BP_ERROR_T BP_HTTPdWebsocketSendPong
(
		P_BP_HTTPD_WEBSOCKET websocket,
		int                  additional_tcp_flags
)
{

	// run basic checks
	if(!websocket)
		return ERR_FAILURE;

	// create a raw header to send
	BP_HTTPD_WEBSOCKET_ENCODED_SEND_FRAME_HEADER header_to_send;
	BP_ZERO_STACK(header_to_send);

	// set fin flag
	header_to_send.fin_flag   = BP_TRUE;

	// set pong
	header_to_send.opcode = BP_HTTPD_WEBSOCKET_FRAME_OPCODE_PONG;


	// encode the send frame header
	BP_HTTPdWebsocketEncodeSendFrameHeader(	&header_to_send	);

	// attempt to send encoded data
	BP_HTTPdWebsocketSendRaw
	(
		websocket,
		&header_to_send,
		(unsigned char *) NULL,
		0,
		BP_KILOBYTE,
		additional_tcp_flags
	);

	// increment the number of pongs sent
	websocket->pongs_sent++;

	// return indicating success
	return ERR_SUCCESS;

}


// --- Close Websockets ----------------------

// close a websocket connection
BP_ERROR_T BP_HTTPdWebsocketClose
(
		P_BP_HTTPD_SESSION httpd
)
{

	// return indicating success
	return ERR_SUCCESS;

}






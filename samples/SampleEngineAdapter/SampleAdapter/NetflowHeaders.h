// =============================================================================
//	NetFlow.h
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#pragma once

#include <PacketHeaders.h>
#include <WinSock2.h>


// =============================================================================
//		Data Structures and Constants
// =============================================================================

typedef UInt32 AdapterID;

enum _peek_flags {
	PEEK_CONTROL = 0x01,
	PEEK_CRC_ERR = 0x02,
	PEEK_FRAME_ERR = 0x04
};


enum peek_status {
	PEEK_ENCRYPTED = 0x04,
	PEEK_DECRYPT_ERR = 0x08,
	PEEK_SHORT_PREAMBLE = 0x40
};


#include "wppushpack.h"

struct peek_hdr {
	UInt8	signal_dBm;			// signal dBm
	UInt8	noise_dBm;			// noise dBm
	UInt16	packet_length;		// contains correct info
	UInt16	slice_length;		// contains correct info
	UInt8	flags;				// only PEEK_CONTROL is filled based on 
								// IEEE80211 control frame or not
	UInt8	status;				// PEEK_ENCRYPTED and PEEK_SHORT_PREAMBLE
								// are the only valid bits
	struct timeval ts;			// replaced the original 64-bit field with
								// what gettimeofday uses
								// equivalent to the wireless_80211_private_hdr

	UInt8	data_rate;			// data rate in 500Kbps
	UInt8	channel;			// channel numbers 1-14 for 2.4GHz, 36-161 for 5GHz
	UInt8	signal_strength;	// signal strength is a number between 0-100
	UInt8	noise_strength;		// noise strength
};

#define kHeaderSize sizeof(peek_hdr)


struct eth_hdr {
	UInt8	nDstMac[6];
	UInt8	nSrcMac[6];
	UInt16	nEthProto;

};


struct ip_hdr {
	UInt8	nVersion;
	UInt8	nServices;
	UInt16	nLength;
	UInt16	nID;
	UInt8	nFlags;
	UInt8	nFragOffset;
	UInt8	nTTL;
	UInt8	nProtocol;
	UInt16	nChecksum;
	UInt32	nSrcAddress;
	UInt32	nDstAddress;
};


struct tcp_hdr {
	UInt16	nSrcPort;
	UInt16	nDstPort;
	UInt32	nSeq;
	UInt32	nAck;
	UInt8	nTCPOffset;
	UInt8	nTCPFlags;
	UInt16	nWindow;
	UInt16	nChecksum;
	UInt16	nUrgentPointer;
};


struct udp_hdr {
	UInt16	nSrcPort;
	UInt16	nDstPort;
	UInt16	nLength;
	UInt16	nChecksum;
};


struct icmp_hdr {
	UInt8	nType;
	UInt8	nCode;
	UInt16  nChecksum;
	UInt16  nUnused;
	UInt16	nNextHop;
};


struct arp_hdr {
	UInt16	nHardware;
	UInt16	nProtocol;
	UInt8	nHardwareAddrLength;
	UInt16	nOperation;
	UInt8	nSenderHardwareAddr[6];
	UInt8	nSenderInternetAddr[6];
	UInt8	nTargetHardwareAddr[6];
	UInt8	nTargetInternetAddr[6];
};

struct netflow_v5_hdr {
	UInt16	nVersion;
	UInt16	nCount;
	UInt32	nUptime;
	UInt32	nSeconds;
	UInt32	nNanoSeconds;
	UInt32	nSequence;
	UInt8	nEngineType;
	UInt8	nEngineID;
	UInt16	nInterval;
};


struct netflow_v5_flow {
	UInt32	nSrcAddress;
	UInt32	nDstAddress;
	UInt32	nNextHop;
	UInt16	nInputSNMP;
	UInt16	nOutputSNMP;
	UInt32	nPackets;
	UInt32	nBytes;
	UInt32	nUpTimeFirst;
	UInt32	nUpTimeLast;
	UInt16	nSrcPort;
	UInt16	nDstPort;
	UInt8	nPad1;
	UInt8	nTCPFlags;
	UInt8	nProtocol;
	UInt8	nTOS;
	UInt16	nSrcAS;
	UInt16	nDstAS;
	UInt8	nSrcMask;
	UInt8	nDstMask;
	UInt16	nPad2;
};


struct netflow_v9_hdr {
	UInt16	nVersion;
	UInt16	nCount;
	UInt32	nUptime;
	UInt32	nSeconds;
	UInt32	nSequence;
	UInt32	nSourceID;
};


struct netflow_v9_flow_set {
	UInt16	nFlowSetID;
	UInt16	nLength;
};


struct netflow_v9_template_flow_set {
	UInt16	nTemplateID;
	UInt16	nFieldCount;
};


struct netflow_v9_flow_type {
	UInt16	nType;
	UInt16	nLength;
};


struct netflow_v9_template_256 {
	UInt32	nSrcAddress;
	UInt32	nDstAddress;
	UInt32	nInputSNMP;
	UInt32	nFlowSamplerID;
	UInt16	nSrcPort;
	UInt16	nDstPort;
	UInt8	nSrcTOS;
	UInt8	nProtocol;
	UInt8	nTCPFlags;
	UInt8	nSrcMask;
	UInt8	nDstMask;
	UInt16	nSrcAS;
	UInt16	nDstAS;
	UInt32	nOutputSNMP;
	UInt32	nNextHop;
	UInt32	nFirstSwitched;
	UInt32	nLastSwitched;
	UInt32	nInPackets;
	UInt32	nInBytes;
};


typedef struct NetFlowPacketHeader {	
	MediaSpecificPrivateHeader	StdHeader;
	UInt32	nInputInterface;
	UInt32	nOutputInterface;
	UInt32	nRouterIP;
} WP_PACK_STRUCT NetFlowPacketHeader;

#include "wppoppack.h"

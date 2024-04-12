// ============================================================================
//	Ethernet.h
//		definition for various Ethernet types.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2011. All rights reserved.

#pragma once

#include "WPTypes.h"

enum {
	kDNS_OpCode_Query,			// QUERY - standard query
	kDNS_OpCode_IQuery,			// IQUERY - inverse query
	kDNS_OpCode_Status			// STATUS - server status
};

enum {
	kDNS_RCode_NoError,			// No error
	kDNS_RCode_Format,			// Format error
	kDNS_RCode_Server,			// Server failure
	kDNS_RCode_Name,			// Name error
	kDNS_RCode_Unimplemented,	// Not implemented
	kDNS_RCode_Refused			// Server refused the request
};

#include "wppushpack.h"

typedef union _tEthernetAddress {
	UInt8	bytes[6];
	struct {
		UInt32	msb;
		UInt16	lsb;
	} words;
} tEthernetAddress;

typedef union _tIpV4Address {
	UInt32	word;
	UInt8	bytes[4];
} tIpV4Address;

typedef union _tIpV6Address {
	UInt64	longs[2];
	UInt32	words[4];
	UInt16	shorts[8];
	UInt8	bytes[16];
} tIpV6Address;

typedef struct _tEthernetHeader {
	tEthernetAddress	Destination;
	tEthernetAddress	Source;
	UInt16				ProtocolType;
} tEthernetHeader;

typedef struct _t80211Header {
	UInt16				VersionTypeSubType;
	UInt16				Duration;
	tEthernetAddress	BSSID;
	tEthernetAddress	Source;
	tEthernetAddress	Destination;
	UInt16				ProtocolType;
} t80211Header;

typedef struct _tIpV4Header {
	UInt8			VersionLength;
	UInt8			TypeOfService;
	UInt16			TotalLength;
	UInt16			Identifier;
	UInt16			FlagsFragmentOffset;
	UInt8			TimeToLive;
	UInt8			Protocol;
	UInt16			HeaderChecksum;
	tIpV4Address	Source;
	tIpV4Address	Destination;
} tIpV4Header;

typedef struct _tIpV6Header {
	UInt32			VersionClassLabel;
	UInt16			PayloadLength;
	UInt8			NextHeader;
	UInt8			HopLimit;
	tIpV6Address	Source;
	tIpV6Address	Destination;
} tIpV6Header;

typedef struct _tTcpHeader {
	UInt16	SourcePort;
	UInt16	DestinationPort;
	UInt32	SequenceNumber;
	UInt32	AckNumber;
	UInt8	Offset;
	UInt8	Flags;
	UInt16	WindowSize;
	UInt16	Checksum;
	UInt16	UrgentPointer;
} tTcpHeader;

typedef struct _tUdpHeader {
	UInt16	SourcePort;
	UInt16	DestinationPort;
	UInt16	Length;
	UInt16	Checksum;
} tUdpHeader;

typedef struct _tDnsHeader {
	unsigned short	id;			// query identification number

	unsigned short	rd:		1;	// recursion desired
	unsigned short	tc:		1;	// truncated message
	unsigned short	aa:		1;	// authoritative answer
	unsigned short	opcode: 4;	// purpose of message
	unsigned short	qr:     1;	// response flag

	unsigned short	rcode:  4;	// response code
	unsigned short	unused: 1;	// Z - unused bits
	unsigned short	ad:		1;	// authentic data from named
	unsigned short	cd:		1;	// checking disabled by resolver
	unsigned short	ra:		1;	// recursion available

	unsigned short	qdcount;	// number of question entries
	unsigned short	ancount;	// number of answer entries
	unsigned short	nscount;	// number of authority entries
	unsigned short	arcount;	// number of resource entries
} tDnsHeader;

#include "wppoppack.h"

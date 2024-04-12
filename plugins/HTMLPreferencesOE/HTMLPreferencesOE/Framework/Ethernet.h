// ============================================================================
//	Ethernet.h
//		definition for various Ethernet types.
// ============================================================================
//	Copyright (c) 2009-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "WPTypes.h"
#include "PeekHash.h"


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Various Ethernet constants and data structures: address and header types.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// ============================================================================
//		Constants and Data Structures
// ============================================================================

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


union tEthernetAddress {
	UInt8	bytes[6];
	struct {
		UInt32	msb;
		UInt16	lsb;
	} words WP_PACK_STRUCT;

	size_t	Hash( int inAlg = kPeekHash_FNV ) const {
		return CALLTYPEHASH( inAlg, union tEthernetAddress, bytes[0], size_t );
	}
} WP_PACK_STRUCT;
typedef union tEthernetAddress	tEthernetAddress;

union tIpV4Address {
	UInt8	bytes[4];
	UInt32	word;

	size_t	Hash( int inAlg = kPeekHash_FNV ) const {
		(void)inAlg;
		return static_cast<size_t>( word );
	}
} WP_PACK_STRUCT;
typedef union tIpV4Address	tIpV4Address;

union tIpV6Address {
	UInt8	bytes[16];
	UInt16	shorts[8];
	UInt32	words[4];
	UInt64	longs[2];

	size_t	Hash( int inAlg = kPeekHash_FNV ) const {
		return CALLTYPEHASH( inAlg, union tIpV6Address, bytes[0], size_t );
	}
} WP_PACK_STRUCT;
typedef union tIpV6Address	tIpV6Address;

struct tEthernetHeader {
	tEthernetAddress	Destination;
	tEthernetAddress	Source;
	UInt16				ProtocolType;

	size_t	Hash( int inAlg = kPeekHash_FNV ) const {
		// return CALLTYPEHASH( inAlg, struct tEthernetHeader, Destination, size_t );
		return (Destination.Hash( inAlg ) ^ Source.Hash( inAlg ) ^ ProtocolType);
	}
} WP_PACK_STRUCT;
typedef struct tEthernetHeader	tEthernetHeader;

struct t80211Header {
	UInt16				VersionTypeSubType;
	UInt16				Duration;
	tEthernetAddress	BSSID;
	tEthernetAddress	Source;
	tEthernetAddress	Destination;
	UInt16				ProtocolType;

	size_t	Hash( int inAlg = kPeekHash_FNV ) const {
		// return CALLTYPEHASH( inAlg, struct t80211Header, VersionTypeSubType, size_t );
		return
			(VersionTypeSubType << 16 | (Duration << 8) | ProtocolType) ^
			BSSID.Hash( inAlg ) ^
			Source.Hash( inAlg ) ^
			Destination.Hash( inAlg );
	}
} WP_PACK_STRUCT;
typedef struct t80211Header	t80211Header;

struct tIpV4Header {
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

	size_t	Hash( int inAlg = kPeekHash_FNV ) const {
		// return CALLTYPEHASH( inAlg, struct tIpV4Header, VersionLength, size_t );
		const size_t	nSize = sizeof( struct tIpV4Header ) - (sizeof( tIpV4Address ) * 2);
		return
			(CALLTYPESIZEHASH( inAlg, struct tIpV4Header, nSize, VersionLength, size_t )) ^
			Source.Hash( inAlg ) ^
			Destination.Hash( inAlg );
	}
} WP_PACK_STRUCT;
typedef struct tIpV4Header	tIpV4Header;

struct tIpV6Header {
	UInt32			VersionClassLabel;
	UInt16			PayloadLength;
	UInt8			NextHeader;
	UInt8			HopLimit;
	tIpV6Address	Source;
	tIpV6Address	Destination;

	size_t	Hash( int inAlg = kPeekHash_FNV ) const {
		//return CALLTYPEHASH( inAlg, struct tIpV6Header, VersionClassLabel, size_t );
		const size_t	nSize = sizeof( struct tIpV6Header ) - (sizeof( tIpV6Address ) * 2);
		return
			(CALLTYPESIZEHASH( inAlg, struct tIpV6Header, nSize, VersionClassLabel, size_t )) ^
			Source.Hash( inAlg ) ^
			Destination.Hash( inAlg );
	}
} WP_PACK_STRUCT;
typedef struct tIpV6Header	tIpV6Header;

struct tTcpHeader {
	UInt16	SourcePort;
	UInt16	DestinationPort;
	UInt32	SequenceNumber;
	UInt32	AckNumber;
	UInt8	Offset;
	UInt8	Flags;
	UInt16	WindowSize;
	UInt16	Checksum;
	UInt16	UrgentPointer;

	size_t	Hash( int inAlg = kPeekHash_FNV ) const {
		// return CALLTYPEHASH( inAlg, struct tTcpHeader, SourcePort, size_t );
		const size_t	nSize = sizeof( struct tTcpHeader ) - (sizeof( UInt16 ) * 2);
		return
			(CALLTYPESIZEHASH( inAlg, struct _tTcpHeader, nSize, SequenceNumber, size_t )) ^
			SourcePort ^
			DestinationPort;
	}
} WP_PACK_STRUCT;
typedef struct tTcpHeader	tTcpHeader;

struct tUdpHeader {
	UInt16	SourcePort;
	UInt16	DestinationPort;
	UInt16	Length;
	UInt16	Checksum;

	size_t	Hash( int inAlg = kPeekHash_FNV ) const {
		// return CALLTYPEHASH( inAlg, struct tUdpHeader, SourcePort, size_t );
		const size_t	nSize = sizeof( struct tUdpHeader ) - (sizeof( UInt16 ) * 2);
		return
			(CALLTYPESIZEHASH( inAlg, struct _tUdpHeader, nSize, Length, size_t )) ^
			SourcePort ^
			DestinationPort;
	}
} WP_PACK_STRUCT;
typedef struct tUdpHeader	tUdpHeader;

struct tDnsHeader {
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

	size_t	Hash( int inAlg = kPeekHash_FNV ) const {
		const size_t	nSize = sizeof( struct tDnsHeader );
		return CALLTYPESIZEHASH( inAlg, struct tDnsHeader, nSize, id, size_t );
	}
} WP_PACK_STRUCT;
typedef struct tDnsHeader	tDnsHeader;

#include "wppoppack.h"

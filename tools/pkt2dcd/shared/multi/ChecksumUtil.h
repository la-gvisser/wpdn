// =============================================================================
//	ChecksumUtil.h
// =============================================================================
//	Copyright (c) 1996-2012 WildPackets, Inc. All rights reserved.

#ifndef CHECKSUMUTIL_H
#define CHECKSUMUTIL_H

#include "AGTypes.h"

#include "wppushpack.h"
struct PseudoIPHeader
{
	UInt32	Source;
	UInt32	Destination;
	UInt8	Zero;
	UInt8	Protocol;
	UInt16	Length;
} WP_PACK_STRUCT;
struct PseudoIPv6Header
{
	UInt8	Source[16];
	UInt8	Destination[16];
	UInt32	Length;
	UInt8	Zeroes[3];
	UInt8	Protocol;
} WP_PACK_STRUCT;
#include "wppoppack.h"

class UChecksum
{
public:
	enum
	{
		kChecksumType_None,
		kChecksumType_AppleTalk_DDP,
		kChecksumType_IP_Header,
		kChecksumType_TCP_UDP_ICMP_IGMP
	};

	enum
	{
		kChecksumSubType_None,
		kChecksumSubType_IP_ICMP,
		kChecksumSubType_IP_IGMP,
		kChecksumSubType_IP_TCP,
		kChecksumSubType_IP_UDP
	};

	static bool		CheckDDPChecksum( const UInt8* pDdpHdr, UInt16 nDdpBytes,
						UInt16* pOriginalChecksum, UInt16* pCalculatedChecksum );
	static bool		CheckIPHeaderChecksum( const UInt8* pIpHdr, UInt16 pIpBytes,
						UInt16* pOriginalChecksum, UInt16* pCalculatedChecksum );
	static bool		CheckIPDataChecksum( const UInt8* pIpHdr, UInt16 nIpBytes,
						UInt16* pOriginalChecksum, UInt16* pCalculatedChecksum,
						UInt8* outChecksumSubType );

	static UInt16	CalculateDDPChecksum( UInt16 inChecksum,
						const UInt8* inData, UInt16 inDataLength );
	static UInt16	CalculateChecksum16( UInt16 inChecksum,
						const UInt8* inBuffer, UInt16 inSize );
	static UInt16	CalculateChecksum16( UInt16 inChecksum,
						const PseudoIPHeader* inHeader,
						const UInt8* inBuffer, UInt16 inSize );
	static UInt16	CalculateChecksum16( UInt16 inChecksum,
						const PseudoIPv6Header* inHeader,
						const UInt8* inBuffer, UInt16 inSize );
};

#endif /* CHECKSUMUTIL_H */

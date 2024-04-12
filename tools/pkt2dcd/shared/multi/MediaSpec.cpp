// =============================================================================
//	MediaSpec.cpp
// =============================================================================
//	Copyright (c) 1996-2014 WildPackets, Inc. All rights reserved.

#include "stdafx.h"
#include "MediaSpec.h"
#include "MemUtil.h"
#include <stdlib.h>
#include <stdio.h>

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif


// -----------------------------------------------------------------------------
//		Compare
// -----------------------------------------------------------------------------

int
CMediaSpec::Compare(
	const CMediaSpec&	inMediaSpec ) const
{
	// Compare the types.
	if ( inMediaSpec.fType != fType )
	{
		return (int)(((int)inMediaSpec.fType) - ((int)fType));
	}
	
	// Compare the mask.
	if ( inMediaSpec.fMask != fMask )
	{
		return (int)(((int)inMediaSpec.fMask) - ((int)fMask));
	}

	// Compare the spec data.
	return memcmp( inMediaSpec.fData, fData, GetDataLength() );
}


// -----------------------------------------------------------------------------
//		CompareWithMask
// -----------------------------------------------------------------------------

bool
CMediaSpec::CompareWithMask(
	const CMediaSpec&	inMediaSpec ) const
{
	if ( fClass != inMediaSpec.fClass ) return false;
	if ( fType  != inMediaSpec.fType  ) return false;

	if ( fType == kMediaSpecType_IPAddr )
	{
		const UInt32	nAddr1 = NETWORKTOHOST32( *(UInt32*)inMediaSpec.fData );
		const UInt32	nAddr2 = NETWORKTOHOST32( *(UInt32*)fData );
		const UInt32	nMask  = fMask & inMediaSpec.fMask;
		return ((nAddr1 & nMask) == (nAddr2 & nMask));
	}

	if ( fType == kMediaSpecType_IPv6Addr )
	{
		const UInt16	nPrefix1 = (UInt16)(inMediaSpec.fMask & 0xffff);
		const UInt16	nPrefix2 = (UInt16)(fMask & 0xffff);
		if ( (nPrefix1 != 0) || (nPrefix2 != 0) )
		{
			const UInt16	nPrefix = (nPrefix1 > nPrefix2) ? nPrefix1 : nPrefix2;
			if ( nPrefix >= 128 )
			{
				return (memcmp( inMediaSpec.fData, fData, 16 ) == 0);
			}
			else
			{
				const size_t	nBytes = nPrefix / 8;
				if ( memcmp( inMediaSpec.fData, fData, nBytes ) != 0 )
				{
					return false;
				}
				const UInt8	nMask  = (UInt8) ~(0xff >> (nPrefix % 8));
				return ((inMediaSpec.fData[nBytes] & nMask) == (fData[nBytes] & nMask));
			}
		}
	}

	if ( fType == kMediaSpecType_DECnetAddr )
	{
		UInt16	nMask = (UInt16)((fMask >> 16) & (inMediaSpec.fMask >> 16));
			
		if ( nMask == 0xC000 )
		{
			nMask = 0xFFFF;
		}
		else if ( nMask == 0x8000 )
		{
			nMask = 0xFC00;
		}
		else if ( nMask == 0x4000 )
		{
			nMask = 0x03FF;
		}
		else
		{
			nMask = 0;
		}
			
		nMask = LITTLETOHOST16( nMask );
			
		return (((*(UInt16*)inMediaSpec.fData) & nMask) == ((*(UInt16*)fData) & nMask));
	}

	const UInt8*	pData1   = inMediaSpec.fData;
	const UInt8*	pData2   = fData;
	UInt32			nMask    = 0x80000000;
	const size_t	nDataLen = GetDataLength();
	for ( size_t i = 0; i < nDataLen; ++i )
	{
		if ( ((fMask & nMask) != 0) &&
			((inMediaSpec.fMask & nMask) != 0) &&
			(*pData1 != *pData2) )
		{
			return false;
		}

		pData1++;
		pData2++;

		nMask >>= 1;
	}

	return true;
}


// -----------------------------------------------------------------------------
//		GetDataLength [static]
// -----------------------------------------------------------------------------

UInt16
CMediaSpec::GetDataLength(
	TMediaSpecType	type )
{
	static const UInt16 kMediaSpecDataLengths[] =
	{
		0,	// 0  kMediaSpecType_Null
		2,	// 1  kMediaSpecType_EthernetProto
		1,	// 2  kMediaSpecType_LSAP
		5,	// 3  kMediaSpecType_SNAP
		1,	// 4  kMediaSpecType_LAP
		1,	// 5  kMediaSpecType_DDP
		1,	// 6  kMediaSpecType_MACCtl
		0,	// 7  Unused
		8,	// 8  kMediaSpecType_ApplicationID
		4,	// 9  kMediaSpecType_ProtoSpec
		6,	// 10 kMediaSpecType_EthernetAddr
		6,	// 11 kMediaSpecType_TokenRingAddr
		6,	// 12 kMediaSpecType_LAPAddr
		6,	// 13 kMediaSpecType_WirelessAddr
		0,	// 14 Unused
		0,	// 15 Unused
		0,	// 16 Unused
		0,	// 17 Unused
		0,	// 18 Unused
		0,	// 19 Unused
		3,	// 20 kMediaSpecType_AppleTalkAddr
		4,	// 21 kMediaSpecType_IPAddr
		2,	// 22 kMediaSpecType_DECnetAddr
		0,	// 23 kMediaSpecType_OtherAddr
		16,	// 24 kMediaSpecType_IPv6Addr
		10,	// 25 kMediaSpecType_IPXAddr
		0,	// 26 Unused
		0,	// 27 Unused
		0,	// 28 Unused
		0,	// 29 Unused
		0,	// 30 Unused
		0,	// 31 Unused
		1,	// 32 kMediaSpecType_Error
		1,	// 33 kMediaSpecType_ATPort
		2,	// 34 kMediaSpecType_IPPort
		2,	// 35 kMediaSpecType_NWPort
		4,	// 36 kMediaSpecType_TCPPortPair
		2,	// 37 kMediaSpecType_WAN_PPP_Proto
		2,	// 38 kMediaSpecType_WAN_FrameRelay_Proto
		0,	// 39 kMediaSpecType_WAN_X25_Proto
		0,  // 40 kMediaSpecType_WAN_X25e_Proto
		0,	// 41 kMediaSpecType_WAN_U200_Proto
		0,	// 42 kMediaSpecType_WAN_Ipars_Proto
		2,	// 43 kMediaSpecType_WAN_DLCIAddr
		0   // 44 kMediaSpecType_WAN_Q931_Proto
	};

	const size_t	i = static_cast<size_t>(type);
	if ( i < (sizeof(kMediaSpecDataLengths)/sizeof(UInt16)) )
	{
		return kMediaSpecDataLengths[i];
	}

	return 0;
}


// -----------------------------------------------------------------------------
//		GetTypeMask [static]
// -----------------------------------------------------------------------------

UInt32
CMediaSpec::GetTypeMask(
	TMediaSpecType	type )
{
	// Handle cached types.
	// More or less arranged by approximate popularity.
	switch ( type )
	{
		case kMediaSpecType_EthernetAddr:
			return 0xFC000000;

		case kMediaSpecType_IPAddr:
			return 0xFFFFFFFF;

		case kMediaSpecType_EthernetProto:
		case kMediaSpecType_WAN_PPP_Proto:
		case kMediaSpecType_WAN_FrameRelay_Proto:
			return 0xC0000000;

		case kMediaSpecType_WAN_X25_Proto:
		case kMediaSpecType_WAN_X25e_Proto:
		case kMediaSpecType_WAN_U200_Proto:
		case kMediaSpecType_WAN_Ipars_Proto:
		case kMediaSpecType_WAN_Q931_Proto:
			return 0x00000000;	// no!

		case kMediaSpecType_IPPort:
			return 0xC0000000;

		case kMediaSpecType_ApplicationID:
			return 0xFF000000;

		case kMediaSpecType_ProtoSpec:
			return 0xC0000000;

		case kMediaSpecType_TCPPortPair:
			return 0xF0000000;

		case kMediaSpecType_IPv6Addr:
			return 0xFFFF0000;
		
		case kMediaSpecType_WirelessAddr:
			return 0xFC000000;

		case kMediaSpecType_LSAP:
			return 0x80000000;

		case kMediaSpecType_SNAP:
			return 0xF8000000;

		case kMediaSpecType_IPXAddr:
			return 0xFFC00000;

		case kMediaSpecType_NWPort:
			return 0xC0000000;

		case kMediaSpecType_AppleTalkAddr:
			return 0xE0000000;

		case kMediaSpecType_ATPort:
			return 0x80000000;

		case kMediaSpecType_DECnetAddr:
			return 0xC0000000;

		case kMediaSpecType_TokenRingAddr:
			return 0xFC000000;

		case kMediaSpecType_WAN_DLCIAddr:
			// It's only 10 bits but the resolution is bytes.
			return 0xC0000000;

		default:
			break;
	}

	// Default - calculate based on data length.
	UInt32			nTypeMask = 0;
	const size_t	nDataLen  = GetDataLength( type );
	for ( size_t i = 0; i < nDataLen; ++i )
	{
		nTypeMask >>= 1;
		nTypeMask = nTypeMask | 0x80000000;
	}

	return nTypeMask;
}


// -----------------------------------------------------------------------------
//		IsMulticast/IsBroadcast Helpers
// -----------------------------------------------------------------------------

inline static bool IsEthernetMulticastOrMaybeBroadcast( const UInt8 fData[] )
{
	// Check for the Group Address bit, which indicates that the incoming
	// address is a multicast address. If this check succeeds, it doesn't
	// preclude the possibility that the incoming address is more specifically
	// a broadcast address, so further checks may be necessary to precisely
	// conclude which. If this check fails, however, it conclusively
	// eliminates the address from being either a multicast or broadcast and
	// no further checks are necessary.
	return (fData[0] & 0x01) != 0;
}

inline static bool IsEthernetBroadcast( const UInt8 fData[] )
{
	// Check for FF:FF:FF:FF:FF:FF.
	return ((*(UInt32*) &fData[0]) == 0xFFFFFFFF) && 
			((*(UInt16*) &fData[4]) == 0xFFFF);
}

inline static bool IsTokenRingMulticastOrMaybeBroadcast( const UInt8 fData[] )
{
	// Check that the Group Address and the Functional Address bits are set.
	// If this check succeeds, it doesn't preclude the possibility that the
	// incoming address is more specifically a broadcast address. If this
	// check fails, it conclusively eliminates the address from being
	// multicast, but not necessarily broadcast. So in either case, further
	// checks are necessary.
	return ((fData[0] & 0x80) != 0) && ((fData[2] & 0x80) != 0);
}

inline static bool IsTokenRingBroadcast( const UInt8 fData[] )
{
	// Check for FF:FF:FF:FF:FF:FF (all stations broadcast)
	// or C0:00:FF:FF:FF:FF (ring broadcast).
	const UInt16 nFirstTwoBytes = MemUtil::GetShort( fData );
	return ((nFirstTwoBytes == 0xFFFF) || (nFirstTwoBytes == 0xC000)) &&
		((*(UInt32*) &fData[2]) == 0xFFFFFFFF);
}

inline static bool IsWirelessMulticastOrMaybeBroadcast( const UInt8 fData[] )
{
	return IsEthernetMulticastOrMaybeBroadcast( fData );
}

inline static bool IsWirelessBroadcast( const UInt8 fData[] )
{
	return IsEthernetBroadcast( fData );
}

inline static bool IsIPMulticast( const UInt8 fData[] )
{
	// IP multicast addresses are Class D:
	// ------------------------------------
	// | 1110 | Multicast address 28 bits |
	// ------------------------------------
	return (fData[0] & 0xF0) == 0xE0;
}

inline static bool IsIPBroadcast( const UInt8 fData[] )
{
	// Check for 255.255.255.255.
	// TODO: Check broadcast for each address class.
	return (*(UInt32*) &fData[0]) == 0xFFFFFFFF;
}

inline static bool IsIPXBroadcast( const UInt8 fData[] )
{
	// IPX broadcast packets are of the form  FFFFFFFF.* (network broadcast)
	// or *.FFFFFFFFFFFF (host broadcast).
	return ((*(UInt32*) &fData[0]) == 0xFFFFFFFF) ||
		(((*(UInt32*) &fData[4]) == 0xFFFFFFFF) && ((*(UInt16*) &fData[8]) == 0xFFFF));
}


// -----------------------------------------------------------------------------
//		IsMulticast
// -----------------------------------------------------------------------------

bool
CMediaSpec::IsMulticast() const
{
	switch ( fType )
	{
		case kMediaSpecType_EthernetAddr:
			return IsEthernetMulticastOrMaybeBroadcast( fData )
				&& !IsEthernetBroadcast( fData );

		case kMediaSpecType_IPAddr:
			// There is no need to check that the address is not a broadcast
			// address here because being an IP multicast implies that it is
			// not a broadcast.
			return IsIPMulticast( fData );

		case kMediaSpecType_WirelessAddr:
			return IsWirelessMulticastOrMaybeBroadcast( fData ) &&
				!IsWirelessBroadcast( fData );

		case kMediaSpecType_IPv6Addr:
			// TODO
			return false;

		case kMediaSpecType_AppleTalkAddr:
			// TODO
			return false;

		case kMediaSpecType_DECnetAddr:
			// TODO
			return false;

		case kMediaSpecType_IPXAddr:
			// There are no IPX multicast addresses.
			return false;

		case kMediaSpecType_TokenRingAddr:
			return IsTokenRingMulticastOrMaybeBroadcast( fData ) &&
				!IsTokenRingBroadcast( fData );

		default:
			return false;
	}
}


// -----------------------------------------------------------------------------
//		IsBroadcast
// -----------------------------------------------------------------------------

bool
CMediaSpec::IsBroadcast() const
{
	switch ( fType )
	{
		case kMediaSpecType_EthernetAddr:
			return IsEthernetBroadcast( fData );

		case kMediaSpecType_IPAddr:
			return IsIPBroadcast( fData );

		case kMediaSpecType_WirelessAddr:
			return IsWirelessBroadcast( fData );

		case kMediaSpecType_IPv6Addr:
			// TODO
			return false;

		case kMediaSpecType_AppleTalkAddr:
			// TODO
			return false;

		case kMediaSpecType_DECnetAddr:
			// TODO
			return false;

		case kMediaSpecType_IPXAddr:
			return IsIPXBroadcast( fData );

		case kMediaSpecType_TokenRingAddr:
			return IsTokenRingBroadcast( fData );

		default:
			return false;
	}
}


// -----------------------------------------------------------------------------
//		IsBroadcastOrMulticast
// -----------------------------------------------------------------------------

bool
CMediaSpec::IsBroadcastOrMulticast() const
{
	switch ( fType )
	{
		case kMediaSpecType_EthernetAddr:
			return IsEthernetMulticastOrMaybeBroadcast( fData );

		case kMediaSpecType_IPAddr:
			return IsIPMulticast( fData ) || IsIPBroadcast( fData );

		case kMediaSpecType_WirelessAddr:
			return IsWirelessMulticastOrMaybeBroadcast( fData );

		case kMediaSpecType_IPv6Addr:
			// TODO
			return false;

		case kMediaSpecType_AppleTalkAddr:
			// TODO
			return false;

		case kMediaSpecType_DECnetAddr:
			// TODO
			return false;

		case kMediaSpecType_IPXAddr:
			// There are no IPX multicast addresses.
			return IsIPXBroadcast( fData );

		case kMediaSpecType_TokenRingAddr:
			return IsTokenRingMulticastOrMaybeBroadcast( fData ) ||
				IsTokenRingBroadcast( fData );

		default:
			return false;
	}
}


// -----------------------------------------------------------------------------
//		CalculateHash
// -----------------------------------------------------------------------------

UInt32
CMediaSpec::CalculateHash() const
{
	const size_t	nSize = GetDataLength();
	if ( nSize >= sizeof(UInt32) )
	{
		// Use the last four bytes.
		return *(UInt32*) &fData[nSize - sizeof(UInt32)];
	}
	else if ( nSize == sizeof(UInt16) )
	{
		return *(UInt16*) fData;
	}

	UInt32	nHash = 0;
	for ( size_t i = 0; i < nSize; ++i )
	{
		nHash  = nHash << 8;
		nHash |= fData[i];
	}

	return nHash;
}


// -----------------------------------------------------------------------------
//		FromProtoSpecInstID [static]
// -----------------------------------------------------------------------------

CMediaSpec
CMediaSpec::FromProtoSpecInstID(
	UInt32	inProtoSpec )
{
	return CMediaSpec( kMediaSpecClass_Protocol, kMediaSpecType_ProtoSpec,
			0xC0000000, reinterpret_cast<const UInt8*>(&inProtoSpec) );
}


// -----------------------------------------------------------------------------
//		ToProtoSpecInstID [static]
// -----------------------------------------------------------------------------

UInt32
CMediaSpec::ToProtoSpecInstID(
	const TMediaSpec&	spec )
{
	if ( spec.fType != kMediaSpecType_ProtoSpec ) return 0;
	return *(UInt32*) spec.fData;
}


// -----------------------------------------------------------------------------
//		FromProtoSpecID [static]
// -----------------------------------------------------------------------------

CMediaSpec
CMediaSpec::FromProtoSpecID(
	UInt16	inProtoSpec )
{
	// Convert 16-bit PSID into a 32-bit PROTOSPEC_INSTID with an empty 
	// instance ID, and return a media spec for that.  
	const UInt32	nIDWithoutInst	= static_cast<UInt32>(inProtoSpec);
	return FromProtoSpecInstID( nIDWithoutInst );
}


// -----------------------------------------------------------------------------
//		ToProtoSpecID [static]
// -----------------------------------------------------------------------------

UInt16
CMediaSpec::ToProtoSpecID(
	const TMediaSpec&	spec )
{
	if ( spec.fType != kMediaSpecType_ProtoSpec ) return 0;
	return static_cast<UInt16>(*(UInt32*) spec.fData);
}


// -----------------------------------------------------------------------------
//		FromIPv4Address [static]
// -----------------------------------------------------------------------------

CMediaSpec
CMediaSpec::FromIPv4Address(
	UInt32	inIPv4Address )
{
	const UInt32	n = HostToNetwork( inIPv4Address );
	return CMediaSpec( kMediaSpecClass_Address, kMediaSpecType_IPAddr,
			0xFFFFFFFF, reinterpret_cast<const UInt8*>(&n) );
}


// -----------------------------------------------------------------------------
//		ToIPv4Address [static]
// -----------------------------------------------------------------------------

UInt32
CMediaSpec::ToIPv4Address(
	const TMediaSpec&	spec )
{
	if ( spec.fType != kMediaSpecType_IPAddr ) return 0;
	return NETWORKTOHOST32( *(UInt32*) spec.fData );
}


// -----------------------------------------------------------------------------
//		FromTCPUDPPort [static]
// -----------------------------------------------------------------------------

CMediaSpec
CMediaSpec::FromTCPUDPPort(
	UInt16	inPortNumber )
{
	const UInt16	n = HostToNetwork( inPortNumber );
	return CMediaSpec( kMediaSpecClass_Port, kMediaSpecType_IPPort,
			0xC0000000, reinterpret_cast<const UInt8*>(&n) );
}


// -----------------------------------------------------------------------------
//		ToTCPUDPPort [static]
// -----------------------------------------------------------------------------

UInt16
CMediaSpec::ToTCPUDPPort(
	const TMediaSpec&	spec )
{
	if ( spec.fType != kMediaSpecType_IPPort ) return 0;
	return NETWORKTOHOST16( *(UInt16*) spec.fData );
}


#if defined(_DEBUG)
// -----------------------------------------------------------------------------
//		Dump
// -----------------------------------------------------------------------------

void
CMediaSpec::Dump(
	char*	inStream,
	UInt32	ulCount ) const
{
	const char*	kMediaSpecClassNames[] =
	{
		"kMediaSpecClass_Null",				// 0
		"kMediaSpecClass_Protocol",			// 1
		"kMediaSpecClass_Address",			// 2
		"kMediaSpecClass_Port"				// 3
	};

	const char*	kMediaSpecTypeNames[] =
	{
		"kMediaSpecType_Null",				// 0
		"kMediaSpecType_EthernetProto",		// 1
		"kMediaSpecType_LSAP",				// 2
		"kMediaSpecType_SNAP",				// 3
		"kMediaSpecType_LAP",				// 4
		"kMediaSpecType_DDP",				// 5
		"kMediaSpecType_MACCtl",			// 6
		"INVALID",							// 7
		"kMediaSpecType_ApplicationID",		// 8
		"kMediaSpecType_ProtoSpec",			// 9
		"kMediaSpecType_EthernetAddr",		// A
		"kMediaSpecType_TokenRingAddr",		// B
		"kMediaSpecType_LAPAddr",			// C
		"kMediaSpecType_WirelessAddr",		// D
		"INVALID",							// E
		"INVALID",							// F
		"INVALID",							// 10
		"INVALID",							// 11
		"INVALID",							// 12
		"INVALID",							// 13
		"kMediaSpecType_AppleTalkAddr",		// 14
		"kMediaSpecType_IPAddr",			// 15
		"kMediaSpecType_DECnetAddr",		// 16
		"kMediaSpecType_OtherAddr",			// 17
		"kMediaSpecType_IPv6Addr",			// 18
		"kMediaSpecType_IPXAddr",			// 19
		"INVALID",							// 1A
		"INVALID",							// 1B
		"INVALID",							// 1C
		"INVALID",							// 1D
		"INVALID",							// 1E
		"INVALID",							// 1F
		"kMediaSpecType_Error",				// 20
		"kMediaSpecType_ATPort",			// 21
		"kMediaSpecType_IPPort",			// 22
		"kMediaSpecType_NWPort"				// 23
		"kMediaSpecType_TCPPort",			// 24
		"kMediaSpecType_WAN_PPP_Proto",		// 25
		"kMediaSpecType_WAN_FrameRelay_Proto",// 26
		"kMediaSpecType_WAN_X25_Proto",		// 27
		"kMediaSpecType_WAN_X25e_Proto",	// 28
		"kMediaSpecType_WAN_U200_Proto",	// 29
		"kMediaSpecType_WAN_Ipars_Proto",	// 2A
		"kMediaSpecType_WAN_DLCIAddr",		// 2B
		"kMediaSpecType_WAN_Q931_Proto"		// 2C
	};

	// Output type, class, mask.
	char	szString1[1024];
	sprintf( szString1, "class=%s,type=%s,mask=0x%8.8X\n",
		kMediaSpecClassNames[fClass], kMediaSpecTypeNames[fType],
		static_cast<unsigned int>(fMask) );

	// Output data.
	char	szString2[1024];
	strcpy( szString2, "data=" );
	for ( size_t i = 0; i < sizeof(fData); i++ )
	{
		char	szData[16];
		sprintf( szData, "%2.2X ", static_cast<unsigned int>(fData[i]) );
		strcat( szString2, szData );
	}
	strcat( szString2, "\n" );

	if ( strlen( szString1 ) + strlen( szString2 ) >= ulCount )
		return;	// forget it
	strcpy( inStream, szString1 );
	strcat( inStream, szString2 );
}
#endif

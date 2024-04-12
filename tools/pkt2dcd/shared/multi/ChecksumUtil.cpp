// ============================================================================
//	ChecksumUtil.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2012. All rights reserved.
//	Copyright (c) AG Group, Inc. 1996-2000. All rights reserved.

#include "stdafx.h"
#include "MemUtil.h"
#include "ChecksumUtil.h"

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

// ----------------------------------------------------------------------------
//		CheckDDPChecksum
// ----------------------------------------------------------------------------

bool
UChecksum::CheckDDPChecksum(
	const UInt8*	pDdpHdr,
	UInt16			nDdpBytes,
	UInt16* 		pOriginalChecksum,
	UInt16* 		pCalculatedChecksum )
{
	// Check the length.
	if ( nDdpBytes < 2 ) return false;

	// Get the DDP length.
	UInt16	nDdpDataLen = (UInt16)(NETWORKTOHOST16( *(UInt16*) &pDdpHdr[0] ) & 0x03FF);

	// Check the DDP length.
	if ( nDdpDataLen < 4 ) return false;

	// Check the length.
	if ( nDdpBytes < (nDdpDataLen - 4) ) return false;

	// Get the original DDP checksum.
	*pOriginalChecksum = NETWORKTOHOST16( *(UInt16*) &pDdpHdr[2] );

	// Calculate the actual DDP checksum.
	*pCalculatedChecksum = CalculateDDPChecksum( 0,
		&pDdpHdr[4], (UInt16)(nDdpDataLen - 4) );

	return true;
}


// ----------------------------------------------------------------------------
//		CheckIPHeaderChecksum
// ----------------------------------------------------------------------------

bool
UChecksum::CheckIPHeaderChecksum(
	const UInt8*	pIpHdr,
	UInt16			nIpBytes,
	UInt16* 		pOriginalChecksum,
	UInt16* 		pCalculatedChecksum )
{
	// Make sure that the first data byte is available.
	if ( nIpBytes < 1 ) return false;

	// Get the length of the header.
	UInt16	nIpHdrLen = (UInt16)((pIpHdr[0] & 0x0F) * 4);

	// Make sure the entire header is available.
	if ( nIpBytes < nIpHdrLen ) return false;

	// Get the original IP header checksum.
	if ( nIpBytes < 12 ) return false;
	*pOriginalChecksum = NETWORKTOHOST16( *(UInt16*) &pIpHdr[10] );

	// Calculate the IP header checksum.
	*pCalculatedChecksum = CalculateChecksum16(
		*pOriginalChecksum, pIpHdr, nIpHdrLen );

	// The result is zero when they're the same.
	if ( *pCalculatedChecksum == 0 )
	{
		*pCalculatedChecksum = *pOriginalChecksum;
	}

	return true;
}


// ----------------------------------------------------------------------------
//		GetIPDataChecksum
// ----------------------------------------------------------------------------
#define kIpv6HeaderLen	40
#define kIpv6Version	6

bool
UChecksum::CheckIPDataChecksum(
	const UInt8*	pIpHdr,
	UInt16			nIpBytes,
	UInt16* 		pOriginalChecksum,
	UInt16* 		pCalculatedChecksum,
	UInt8*			pChecksumSubType )
{
	UInt16			nIpHdrLen;
	UInt16			nIpDataLen;
	UInt8			nIpProtoType;

	// Get the IP version
	UInt8	nIpId = (UInt8)((pIpHdr[0] & 0xF0) >> 4);

	if ( nIpId == kIpv6Version )
	{
		nIpHdrLen	= kIpv6HeaderLen;
		nIpDataLen	= (UInt16)(NETWORKTOHOST16( *(UInt16*) &pIpHdr[4] ));

		// TODO: Handle	fragments and other	extension headers.

		// Get the IP protocol type, found in the "next	header"	field in the IPv6 header
		nIpProtoType = (UInt8)(pIpHdr[6]);
	}
	else
	{
		// Get the fragmention flags.
		if ( nIpBytes < 7 ) return false;
		UInt8	nFragFlags = (UInt8)((pIpHdr[6] & 0xE0) >> 5);
		if ( (nFragFlags & 0x01) != 0 ) return false;

		// Get the fragment offset.
		UInt16	nFragOffset = (UInt16)(NETWORKTOHOST16( *(UInt16*) &pIpHdr[6] ) & 0x1FFF);
		if ( nFragOffset != 0 ) return false;

		// Get the length of the header.
		nIpHdrLen= (UInt16)((pIpHdr[0] & 0x0F) * 4);

		// Calculate the IP data length.
		nIpDataLen = NETWORKTOHOST16( *(UInt16*) &pIpHdr[2] );
		nIpDataLen = (UInt16)(nIpDataLen - nIpHdrLen);

		// Get the IP protocol type.
		nIpProtoType = pIpHdr[9];
	}
	
	// Check remaining packet length.
	if ( nIpBytes < (nIpHdrLen + nIpDataLen) ) return false;

	// Get a pointer to the IP data.
	const UInt8*	pIpData = &pIpHdr[nIpHdrLen];

	switch ( nIpProtoType )
	{
		case 1: 	// ICMP
		case 2: 	// IGMP
		case 88: 	// EIGRP
		{
			// Set the checksum sub type.
			*pChecksumSubType = (UInt8)((nIpProtoType == 1) ?
				kChecksumSubType_IP_ICMP : kChecksumSubType_IP_IGMP);

			// Get the original checksum.
			*pOriginalChecksum = NETWORKTOHOST16( *(UInt16*) &pIpData[2] );

			// Calculate the checksum.
			*pCalculatedChecksum = CalculateChecksum16(
				*pOriginalChecksum, pIpData, nIpDataLen );

			// The result is zero when they're the same.
			if ( *pCalculatedChecksum == 0 )
			{
				*pCalculatedChecksum = *pOriginalChecksum;
			}
		}
		break;

		case 6: 	// TCP
		case 17:	// UDP
		{
			if ( nIpProtoType == 6 )
			{
				// Set the checksum sub type.
				*pChecksumSubType = kChecksumSubType_IP_TCP;

				// Get the original checksum.
				*pOriginalChecksum = NETWORKTOHOST16( *(UInt16*) &pIpData[16] );
			}
			else
			{
				// Set the checksum sub type.
				*pChecksumSubType = kChecksumSubType_IP_UDP;

				// Get the original checksum.
				*pOriginalChecksum = NETWORKTOHOST16( *(UInt16*) &pIpData[6] );
			}

			if ( nIpId == kIpv6Version )
			{
				// Setup a pseudo header (in network byte order).
				PseudoIPv6Header	thePseudoHeader;
				memset( &thePseudoHeader, 0, sizeof(thePseudoHeader) );
				memcpy(	thePseudoHeader.Source,	&pIpHdr[8],	16);
				memcpy(	thePseudoHeader.Destination, &pIpHdr[24], 16);
				thePseudoHeader.Protocol = nIpProtoType;
				if ( nIpProtoType == 6 )
				{
					thePseudoHeader.Length = HOSTTONETWORK32( (UInt32) nIpDataLen );
				}
				else
				{
					// Use the length from the UDP header.
					UInt16 nLength = NETWORKTOHOST16( *(UInt16*) &pIpData[4] );
					thePseudoHeader.Length = HOSTTONETWORK32( (UInt32) nLength );
				}

				// Calculate the checksum.
				*pCalculatedChecksum = CalculateChecksum16(
					*pOriginalChecksum,	&thePseudoHeader,
					pIpData, nIpDataLen	);
			}
			else
			{
				// Setup a pseudo header (in network byte order).
				PseudoIPHeader	thePseudoHeader;
				thePseudoHeader.Source = *(UInt32*)	&pIpHdr[12];
				thePseudoHeader.Destination	= *(UInt32*) &pIpHdr[16];
				thePseudoHeader.Zero = 0;
				thePseudoHeader.Protocol = nIpProtoType;
				thePseudoHeader.Length = NETWORKTOHOST16( nIpDataLen );

				// Calculate the checksum.
				*pCalculatedChecksum = CalculateChecksum16(
					*pOriginalChecksum,	&thePseudoHeader,
					pIpData, nIpDataLen	);
			}

			// The result is zero when they're the same.
			if ( *pCalculatedChecksum == 0 )
			{
				*pCalculatedChecksum = *pOriginalChecksum;
			}
		}
		break;

		case 58:	//ICMPv6
		{
			// Set the checksum sub type.
			//*pChecksumSubType = kChecksumSubType_ICMPv6;

			// Get the original checksum.
			*pOriginalChecksum = NETWORKTOHOST16( *(UInt16*) &pIpData[2] );

			// Setup a pseudo header (in network byte order).
			PseudoIPv6Header	thePseudoHeader;
			memset( &thePseudoHeader, 0, sizeof(thePseudoHeader) );
			memcpy( thePseudoHeader.Source, &pIpHdr[8], 16);
			memcpy( thePseudoHeader.Destination, &pIpHdr[24], 16);
			thePseudoHeader.Protocol = nIpProtoType;
			thePseudoHeader.Length = NETWORKTOHOST16( nIpDataLen );

			// Calculate the checksum.
			*pCalculatedChecksum = CalculateChecksum16(
				*pOriginalChecksum, &thePseudoHeader,
				pIpData, nIpDataLen );

			// The result is zero when they're the same.
			if ( *pCalculatedChecksum == 0 )
			{
				*pCalculatedChecksum = *pOriginalChecksum;
			}
		}
		break;

		default:
			return false;
	}

	return true;
}


// ----------------------------------------------------------------------------
//		CalculateDDPChecksum
// ----------------------------------------------------------------------------

UInt16
UChecksum::CalculateDDPChecksum(
	UInt16			inChecksum,
	const UInt8*	inData,
	UInt16			inDataLength )
{
	while ( inDataLength-- > 0 )
	{
		inChecksum = (UInt16)(inChecksum + *inData++);

		UInt32	v = inChecksum;
		v = v << 1;
		v |= (v >> 16);
		inChecksum = (UInt16) v;
	}

	if ( inChecksum == 0 )
	{
		inChecksum = 0xFFFF;
	}

	return inChecksum;
}


// ----------------------------------------------------------------------------
//		CalculateChecksum16
// ----------------------------------------------------------------------------

UInt16
UChecksum::CalculateChecksum16(
	UInt16			inChecksum,
	const UInt8*	inData,
	UInt16			inDataLength )
{
	UInt32	theSum = 0;
	UInt16* theData16 = (UInt16*) inData;
	UInt16	theData16Length = inDataLength;

	// Do the pseudo header.
	while ( theData16Length > 1 )
	{
		// Add up the bytes two at a time.
		theSum += NETWORKTOHOST16( *theData16++ );
		theData16Length -= sizeof(UInt16);
	}

	if ( theData16Length > 0 )
	{
		// Add the left-over byte.
		theSum += ((UInt32)(*(UInt8*)theData16)) << 8;
	}

	// Add the high and low 16 bits. Add the carry.
	while ( theSum >> 16 )
	{
		theSum = (theSum >> 16) + (theSum & 0x0000FFFF);
	}

	// Take the one's complement.
	theSum = ~theSum;

	if ( theSum == 0xFFFF0000 )
	{
		return 0;
	}
	else
	{
		theSum = theSum - (UInt16) ~inChecksum;

		while ( theSum >> 16 )
		{
			theSum = (theSum >> 16) + (theSum & 0x0000FFFF);
		}

		return (UInt16) theSum;
	}
}


// ----------------------------------------------------------------------------
//		CalculateChecksum16
// ----------------------------------------------------------------------------

UInt16
UChecksum::CalculateChecksum16(
	UInt16					inChecksum,
	const PseudoIPHeader*	inPseudoHeader,
	const UInt8*			inData,
	UInt16					inDataLength )
{
	UInt32	theSum = 0;
	UInt16* theData16 = (UInt16*) inPseudoHeader;
	UInt16	theData16Length = sizeof(PseudoIPHeader);

	// Do the pseudo header.
	while ( theData16Length > 1 )
	{
		// Add up the bytes two at a time.
		theSum += NETWORKTOHOST16( *theData16++ );
		theData16Length -= sizeof(UInt16);
	}

	// Do the data.
	theData16 = (UInt16*) inData;
	theData16Length = inDataLength;
	while ( theData16Length > 1 )
	{
		// Add up the bytes two at a time.
		theSum += NETWORKTOHOST16( *theData16++ );
		theData16Length -= sizeof(UInt16);
	}

	if ( theData16Length > 0 )
	{
		// Add the left-over byte.
		theSum += ((UInt32)(*(UInt8*)theData16)) << 8;
	}

	// Add the high and low 16 bits. Add the carry.
	while ( theSum >> 16 )
	{
		theSum = (theSum >> 16) + (theSum & 0x0000FFFF);
	}

	// Take the one's complement.
	theSum = ~theSum;

	if ( theSum == 0xFFFF0000 )
	{
		return 0;
	}
	else
	{
		theSum = theSum - (UInt16) ~inChecksum;

		while ( theSum >> 16 )
		{
			theSum = (theSum >> 16) + (theSum & 0x0000FFFF);
		}

		return (UInt16) theSum;
	}
}


// ----------------------------------------------------------------------------
//		CalculateChecksum16
// ----------------------------------------------------------------------------

UInt16
UChecksum::CalculateChecksum16(
	UInt16						inChecksum,
	const PseudoIPv6Header*		inPseudoHeader,
	const UInt8*				inData,
	UInt16						inDataLength )
{
	UInt32	theSum = 0;
	UInt16* theData16 = (UInt16*) inPseudoHeader;
	UInt16	theData16Length = sizeof(PseudoIPv6Header);

	// Do the pseudo header.
	while ( theData16Length > 1 )
	{
		// Add up the bytes two at a time.
		theSum += NETWORKTOHOST16( *theData16++ );
		theData16Length -= sizeof(UInt16);
	}

	// Do the data.
	theData16 = (UInt16*) inData;
	theData16Length = inDataLength;
	while ( theData16Length > 1 )
	{
		// Add up the bytes two at a time.
		theSum += NETWORKTOHOST16( *theData16++ );
		theData16Length -= sizeof(UInt16);
	}

	if ( theData16Length > 0 )
	{
		// Add the left-over byte.
		theSum += ((UInt32)(*(UInt8*)theData16)) << 8;
	}

	// Add the high and low 16 bits. Add the carry.
	while ( theSum >> 16 )
	{
		theSum = (theSum >> 16) + (theSum & 0x0000FFFF);
	}

	// Take the one's complement.
	theSum = ~theSum;

	if ( theSum == 0xFFFF0000 )
	{
		return 0;
	}
	else
	{
		theSum = theSum - (UInt16) ~inChecksum;

		while ( theSum >> 16 )
		{
			theSum = (theSum >> 16) + (theSum & 0x0000FFFF);
		}

		return (UInt16) theSum;
	}
}

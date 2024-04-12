// ============================================================================
//	MediaSpec.cpp
// ============================================================================
//	Coypright (c) WildPackets, Inc. 2000-2004. All rights reserved.
//	Coypright (c) AG Group, Inc. 1996-2000. All rights reserved.

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include "MemUtil.h"
#include "MediaSpec.h"

#if TARGET_OS_WIN32
#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif
#endif

// ----------------------------------------------------------------------------
//		CMediaSpec(TMediaSpecClass,TMediaSpecType,UInt32,const UInt8*)
// ----------------------------------------------------------------------------

CMediaSpec::CMediaSpec(
	TMediaSpecClass	inClass,
	TMediaSpecType	inType,
	UInt32			inMask,
	const UInt8*	inData )
{
	fClass = static_cast<UInt8>(inClass);
	fType = static_cast<UInt8>(inType);
	fMask = inMask;
	if ( fMask == 0 )
	{
		fMask = GetTypeMask();
	}
	if ( inData != NULL )
	{
		memmove( fData, inData, GetDataLength() );
	}
}


// ----------------------------------------------------------------------------
//		operator==
// ----------------------------------------------------------------------------

bool
CMediaSpec::operator==(
	const CMediaSpec&	inMediaSpec ) const
{
	// Compare the class, type, and data.
	return ( (inMediaSpec.fClass == fClass) && (inMediaSpec.fType == fType) &&
		(inMediaSpec.fMask == fMask ) &&
		(memcmp( inMediaSpec.fData, fData, GetDataLength()) == 0) );
}


// ----------------------------------------------------------------------------
//		Compare
// ----------------------------------------------------------------------------

int
CMediaSpec::Compare(
	const CMediaSpec&	inMediaSpec ) const
{
	// Compare the types.
	if ( inMediaSpec.fType != fType )
	{
		return (int)(((int)inMediaSpec.fType) - ((int)fType));
	}
	
	// Compare the Mask
	if ( inMediaSpec.fMask != fMask )
	{
		return (int)(((int)inMediaSpec.fMask) - ((int)fMask));
	}

	if ( fType == kMediaSpecType_WAN_DLCIAddr )
	{	// compare them as 2 WORDs
		return (int)(*((UInt16*)inMediaSpec.fData) - *((UInt16*)fData));
	}

	// Compare the spec data.
	return memcmp( inMediaSpec.fData, fData, GetDataLength() );
}


// ----------------------------------------------------------------------------
//		CompareWithMask
// ----------------------------------------------------------------------------

bool
CMediaSpec::CompareWithMask(
	const CMediaSpec&	inMediaSpec ) const
{
	if ( (inMediaSpec.fClass == fClass) &&
		(inMediaSpec.fType == fType) )
	{
		const UInt8*	pData1 = (UInt8*) inMediaSpec.fData;
		const UInt8*	pData2 = (UInt8*) fData;
		
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
			
			if ( ((*(UInt16*)pData1) & nMask) != ((*(UInt16*)pData2) & nMask) )
			{
				return false;
			}
		}
		else if ( fType == kMediaSpecType_IPAddr )
		{
			UInt32	nAddr1 = NETWORKTOHOST32( *(UInt32*)inMediaSpec.fData );
			UInt32	nAddr2 = NETWORKTOHOST32( *(UInt32*)fData );
			
			UInt32	nMask = fMask & inMediaSpec.fMask;
			
			return ((nAddr1 & nMask) == (nAddr2 & nMask));
		}
		else
		{
			UInt32	nMask = 0x80000000;
			UInt16	nDataLen = GetDataLength();

			for ( UInt16 i = 0; i < nDataLen; i++ )
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
		}
		
		return true;
	}

	return false;
}


// ----------------------------------------------------------------------------
//		GetDataLength
// ----------------------------------------------------------------------------

UInt16
CMediaSpec::GetDataLength() const
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
		0,	// 8  Unused
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
		0,  // 44 kMediaSpecType_WAN_Q931_Proto
	};

	if ( fType < (sizeof(kMediaSpecDataLengths)/sizeof(UInt16)) )
	{
		return kMediaSpecDataLengths[fType];
	}

	return 0;
}


// ----------------------------------------------------------------------------
//		GetTypeMask
// ----------------------------------------------------------------------------

UInt32
CMediaSpec::GetTypeMask() const
{
	// Handle cached types.
	// More or less arranged by approximate popularity.
	switch ( fType )
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

		case kMediaSpecType_WAN_DLCIAddr:	// it's only 10 bits but the resolution is bytes
			return 0xC0000000;
	}

	// Default - calculate based on data length.
	UInt32	nTypeMask = 0;
	UInt16	nDataLen = GetDataLength();
	for ( ; nDataLen > 0; nDataLen-- )
	{
		nTypeMask >>= 1;
		nTypeMask = nTypeMask | 0x80000000;
	}

	return nTypeMask;
}


// ----------------------------------------------------------------------------
//		IsWildcard
// ----------------------------------------------------------------------------

bool
CMediaSpec::IsWildcard() const
{
	UInt32	nTypeMask = GetTypeMask();
	return ((nTypeMask & fMask) != nTypeMask);
}


// ----------------------------------------------------------------------------
//		IsMulticast
// ----------------------------------------------------------------------------

bool
CMediaSpec::IsMulticast() const
{
	bool	bResult = false;

	switch ( fType )
	{
		case kMediaSpecType_EthernetAddr:
		{
			if ( fMask == GetTypeMask() )
			{
				bResult = ((fData[0] & 0x01) != 0) &&
					!(((*(UInt32*) &fData[0]) == 0xFFFFFFFF) &&
					  ((*(UInt16*) &fData[4]) == 0xFFFF));
			}
		}
		break;

		case kMediaSpecType_TokenRingAddr:
		{
			if ( fMask == GetTypeMask() )
			{
				bResult = (((fData[0] & 0x80) != 0) &&
					((fData[2] & 0x80) != 0)) &&
					!((((*(UInt32*) &fData[0]) == 0xFFFFFFFF) ||
					   ((*(UInt32*) &fData[0]) == 0xC000FFFF)) &&
					   ((*(UInt16*) &fData[4]) == 0xFFFF));
			}
		}
		break;

		case kMediaSpecType_WirelessAddr:
		{
			if ( fMask == GetTypeMask() )
			{
				bResult = ((fData[0] & 0x01) != 0) &&
					!(((*(UInt32*) &fData[0]) == 0xFFFFFFFF) &&
					  ((*(UInt16*) &fData[4]) == 0xFFFF));
			}
		}
		break;

		case kMediaSpecType_AppleTalkAddr:
		{
			// TODO
		}
		break;

		case kMediaSpecType_IPAddr:
		{
			if ( fMask == GetTypeMask() )
			{
				// IP multicast addresses are Class D:
				// ------------------------------------
				// | 1110 | Multicast address 28 bits |
				// ------------------------------------
				bResult = ((fData[0] & 0xF0) == 0xE0);
			}
		}
		break;

		case kMediaSpecType_DECnetAddr:
		{
			// TODO
		}
		break;

		case kMediaSpecType_IPv6Addr:
		{
			// TODO
		}
		break;

		case kMediaSpecType_IPXAddr:
		{
			// There are no IPX multicast addresses.
		}
		break;
	}

	return bResult;
}


// ----------------------------------------------------------------------------
//		IsBroadcast
// ----------------------------------------------------------------------------

bool
CMediaSpec::IsBroadcast() const
{
	bool	bResult = false;

	switch ( fType )
	{
		case kMediaSpecType_EthernetAddr:
		{
			if ( fMask == GetTypeMask() )
			{
				bResult = (((*(UInt32*) &fData[0]) == 0xFFFFFFFF) &&
						   ((*(UInt16*) &fData[4]) == 0xFFFF));
			}
		}
		break;

		case kMediaSpecType_TokenRingAddr:
		{
			if ( fMask == GetTypeMask() )
			{
				bResult = ((((*(UInt32*) &fData[0]) == 0xFFFFFFFF) ||
							((*(UInt32*) &fData[0]) == 0xC000FFFF)) &&
							((*(UInt16*) &fData[4]) == 0xFFFF));
			}
		}
		break;

		case kMediaSpecType_WirelessAddr:
		{
			if ( fMask == GetTypeMask() )
			{
				bResult = (((*(UInt32*) &fData[0]) == 0xFFFFFFFF) &&
						   ((*(UInt16*) &fData[4]) == 0xFFFF));
			}
		}
		break;

		case kMediaSpecType_AppleTalkAddr:
		{
			// TODO
		}
		break;

		case kMediaSpecType_IPAddr:
		{
			if ( fMask == GetTypeMask() )
			{
				// Check 255.255.255.255 first.
				bResult = ((*(UInt32*) &fData[0]) == 0xFFFFFFFF);

				if ( !bResult )
				{
					// TODO: Check broadcast for each address class.
				}
			}
		}
		break;

		case kMediaSpecType_DECnetAddr:
		{
			// TODO
		}
		break;

		case kMediaSpecType_IPv6Addr:
		{
			// TODO
		}
		break;

		case kMediaSpecType_IPXAddr:
		{
			if ( fMask == GetTypeMask() )
			{
				// IPX broadcast packets are of the form  FFFFFFFF.*
				// (network broadcast) or *.FFFFFFFFFFFF (host 
				// broadcast).
				bResult = ((*(UInt32*) &fData[0]) == 0xFFFFFFFF) ||
						 (((*(UInt32*) &fData[4]) == 0xFFFFFFFF) &&
						  ((*(UInt16*) &fData[8]) == 0xFFFF) );
			}
		}
		break;
	}

	return bResult;
}


// ----------------------------------------------------------------------------
//		IsBroadcastOrMulticast
// ----------------------------------------------------------------------------

bool
CMediaSpec::IsBroadcastOrMulticast() const
{
	bool	bResult = false;

	switch ( fType )
	{
		case kMediaSpecType_EthernetAddr:
		{
			if ( fMask == GetTypeMask() )
			{
				bResult = ((fData[0] & 0x01) != 0);
			}
		}
		break;

		case kMediaSpecType_TokenRingAddr:
		{
			if ( fMask == GetTypeMask() )
			{
				bResult = ((fData[0] & 0x80) != 0) && ((fData[2] & 0x80) != 0);

				if ( !bResult )
				{
					bResult = ((((*(UInt32*) &fData[0]) == 0xFFFFFFFF) ||
								((*(UInt32*) &fData[0]) == 0xC000FFFF)) &&
								((*(UInt16*) &fData[4]) == 0xFFFF));
				}
			}
		}
		break;

		case kMediaSpecType_WirelessAddr:
		{
			if ( fMask == GetTypeMask() )
			{
				bResult = ((fData[0] & 0x01) != 0);
			}
		}
		break;

		case kMediaSpecType_AppleTalkAddr:
		{
			// TODO
		}
		break;

		case kMediaSpecType_IPAddr:
		{
			if ( fMask == GetTypeMask() )
			{
				// IP multicast addresses are Class D:
				// ------------------------------------
				// | 1110 | Multicast address 28 bits |
				// ------------------------------------
				bResult = ((fData[0] & 0xF0) == 0xE0);

				if ( !bResult )
				{
					// Check 255.255.255.255.
					bResult = ((*(UInt32*) &fData[0]) == 0xFFFFFFFF);

					if ( !bResult )
					{
						// TODO: Check broadcast for each address class.
					}
				}
			}
		}
		break;

		case kMediaSpecType_DECnetAddr:
		{
			// TODO
		}
		break;

		case kMediaSpecType_IPv6Addr:
		{
			// TODO
		}
		break;

		case kMediaSpecType_IPXAddr:
		{
			// IPX broadcast packets are of the form  FFFFFFFF.*
			// (network broadcast) or *.FFFFFFFFFFFF (host 
			// broadcast).
			// There are no IPX multicast addresses.
			bResult = ((*(UInt32*) &fData[0]) == 0xFFFFFFFF) ||
						(((*(UInt32*) &fData[4]) == 0xFFFFFFFF) &&
						((*(UInt16*) &fData[8]) == 0xFFFF) );
		}
		break;
	}

	return bResult;
}


// ----------------------------------------------------------------------------
//		CalculateHash
// ----------------------------------------------------------------------------

UInt32
CMediaSpec::CalculateHash() const
{
	UInt32	nHash = 0;
	UInt16	nSize = GetDataLength();
	for	( UInt16 i = 0; i < nSize; i++ )
	{
		nHash += fData[i];
	}

	return nHash;
}


#if defined(_DEBUG)
// ----------------------------------------------------------------------------
//		Dump
// ----------------------------------------------------------------------------

void
CMediaSpec::Dump(
	CByteStream&	inStream ) const
{
	const TCHAR*	kMediaSpecClassNames[] =
	{
		_T("kMediaSpecClass_Null"),				// 0
		_T("kMediaSpecClass_Protocol"),			// 1
		_T("kMediaSpecClass_Address"),			// 2
		_T("kMediaSpecClass_Port")				// 3
	};

	const TCHAR*	kMediaSpecTypeNames[] =
	{
		_T("kMediaSpecType_Null"),				// 0
		_T("kMediaSpecType_EthernetProto"),		// 1
		_T("kMediaSpecType_LSAP"),				// 2
		_T("kMediaSpecType_SNAP"),				// 3
		_T("kMediaSpecType_LAP"),				// 4
		_T("kMediaSpecType_DDP"),				// 5
		_T("kMediaSpecType_MACCtl"),			// 6
		_T("INVALID"),							// 7
		_T("INVALID"),							// 8
		_T("kMediaSpecType_ProtoSpec"),			// 9
		_T("kMediaSpecType_EthernetAddr"),		// A
		_T("kMediaSpecType_TokenRingAddr"),		// B
		_T("kMediaSpecType_LAPAddr"),			// C
		_T("kMediaSpecType_WirelessAddr"),		// D
		_T("INVALID"),							// E
		_T("INVALID"),							// F
		_T("INVALID"),							// 10
		_T("INVALID"),							// 11
		_T("INVALID"),							// 12
		_T("INVALID"),							// 13
		_T("kMediaSpecType_AppleTalkAddr"),		// 14
		_T("kMediaSpecType_IPAddr"),			// 15
		_T("kMediaSpecType_DECnetAddr"),		// 16
		_T("kMediaSpecType_OtherAddr"),			// 17
		_T("kMediaSpecType_IPv6Addr"),			// 18
		_T("kMediaSpecType_IPXAddr"),			// 19
		_T("INVALID"),							// 1A
		_T("INVALID"),							// 1B
		_T("INVALID"),							// 1C
		_T("INVALID"),							// 1D
		_T("INVALID"),							// 1E
		_T("INVALID"),							// 1F
		_T("kMediaSpecType_Error"),				// 20
		_T("kMediaSpecType_ATPort"),			// 21
		_T("kMediaSpecType_IPPort"),			// 22
		_T("kMediaSpecType_NWPort")				// 23
		_T("kMediaSpecType_TCPPort"),			// 24
		_T("kMediaSpecType_WAN_PPP_Proto"),		// 25
		_T("kMediaSpecType_WAN_FrameRelay_Proto"),// 26
		_T("kMediaSpecType_WAN_X25_Proto"),		// 27
		_T("kMediaSpecType_WAN_X25e_Proto"),	// 28
		_T("kMediaSpecType_WAN_U200_Proto"),	// 29
		_T("kMediaSpecType_WAN_Ipars_Proto"),	// 2A
		_T("kMediaSpecType_WAN_DLCIAddr"),		// 2B
		_T("kMediaSpecType_WAN_Q931_Proto"),	// 2C
	};

	// Output type, class, mask.
	TCHAR	szString[1024];
	_stprintf( szString, _T("class=%s,type=%s,mask=0x%8.8lX\n"),
		kMediaSpecClassNames[fClass], kMediaSpecTypeNames[fType], fMask );
	inStream.Write( szString );

	// Output data.
	_tcscpy( szString, _T("data=") );
	for ( size_t i = 0; i < sizeof(fData); i++ )
	{
		TCHAR	szData[16];
		_stprintf( szData, _T("%2.2X "), fData[i] );
		_tcscat( szString, szData );
	}
	_tcscat( szString, _T("\n") );
	inStream.Write( szString );
}
#endif

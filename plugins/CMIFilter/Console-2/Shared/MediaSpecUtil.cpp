// =============================================================================
//	MediaSpecUtil.cpp
// =============================================================================
//	Coypright (c) WildPackets, Inc. 2000-2004. All rights reserved.
//	Coypright (c) AG Group, Inc. 1996-2000. All rights reserved.

#include "stdafx.h"
#include <stdlib.h>
#include "MemUtil.h"
#ifndef _NPROTOSPECS
#include "Protospecs.h"
#include "PSIDs.h"
#endif
#include "PacketHeaders.h"
#include "MediaSpecUtil.h"

#if TARGET_OS_WIN32
#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif
#endif

// Strings for TMediaType.
static LPCTSTR	s_pszMediaStrings[] =
{
	_T("Ethernet"),
	_T("Token Ring"),
	_T("FDDI"),
	_T("WAN"),
	_T("LocalTalk"),
	_T("DIX"),
	_T("ARCNET (raw)"),
	_T("ARCNET (878.2)"),
	_T("ATM"),
	_T("Wireless WAN"),
	_T("Infrared (IrDA)"),
	_T("Broadcast Architecture"),
	_T("Connection-oriented WAN"),
	_T("IEEE 1394 (FireWire)")
};


// Strings for TMediaSubType.
static LPCTSTR	s_pszMediaSubTypeStrings[] =
{
	_T("Native"),
	_T("Wireless 802.11b"),
	_T("Wireless 802.11a"),
	_T("Wireless 802.11"),
	_T("WAN PPP"),
	_T("Frame Relay"),
	_T("X.25"),
	_T("X.25e"),
	_T("IPARS"),
	_T("U200"),
	_T("Q931")
};


// Strings for TMediaSpecClass.
static LPCTSTR	s_pszClassStrings[] =
{
	_T(""),
	_T("Protocol"),
	_T("Address"),
	_T("Port")
};


// Strings for TMediaSpecType.
static LPCTSTR	s_pszTypeStrings[] =
{
	_T(""),
	_T("Ethernet Protocol"),
	_T("LSAP"),
	_T("SNAP"),
	_T("LAP"),
	_T("DDP"),
	_T("MacCtl"),
	_T(""),
	_T(""),
	_T("ProtoSpec"),
	_T("Ethernet"),
	_T("TokenRing"),
	_T("LAP Address"),
	_T("Wireless"),
	_T(""),
	_T(""),
	_T(""),
	_T(""),
	_T(""),
	_T(""),
	_T("AppleTalk"),
	_T("IP"),
	_T("DECnet"),
	_T("Other Address"),
	_T("IPv6"),
	_T("IPX"),
	_T(""),
	_T(""),
	_T(""),
	_T(""),
	_T(""),
	_T(""),
	_T("Error"),
	_T("AT Port"),
	_T("TCP-UDP Port"),
	_T("NW Port"),
	_T("TCP Port Pair"),
};


#ifndef _NPROTOSPECS

// -----------------------------------------------------------------------------
//		NetworkToHostMediaSpec [static]
// -----------------------------------------------------------------------------

void
UMediaSpecUtils::NetworkToHostMediaSpec(
	TMediaSpec&	inSpec )
{
	inSpec.fMask = NETWORKTOHOST32( inSpec.fMask );
	if ( inSpec.fType == kMediaSpecType_ProtoSpec )
	{
		*(PROTOSPEC_INSTID*)inSpec.fData = NETWORKTOHOST32( *(PROTOSPEC_INSTID*)inSpec.fData );
	}
}


// -----------------------------------------------------------------------------
//		HostToNetworkMediaSpec [static]
// -----------------------------------------------------------------------------

void
UMediaSpecUtils::HostToNetworkMediaSpec(
	TMediaSpec&	inSpec )
{
	inSpec.fMask = HOSTTONETWORK32( inSpec.fMask );
	if ( inSpec.fType == kMediaSpecType_ProtoSpec )
	{
		*(PROTOSPEC_INSTID*)inSpec.fData = HOSTTONETWORK32( *(PROTOSPEC_INSTID*)inSpec.fData );
	}
}
#endif

// -----------------------------------------------------------------------------
//		GetMediaString [static]
// -----------------------------------------------------------------------------

LPCTSTR
UMediaSpecUtils::GetMediaString(
	TMediaType	inType )
{
	// Check the value against available strings.
	ASSERT( (size_t) inType < COUNTOF(s_pszMediaStrings) );
	if ( (size_t) inType < COUNTOF(s_pszMediaStrings) )
	{
		// Get the string.
		LPCTSTR	pszString = s_pszMediaStrings[inType];

		if ( pszString[0] != 0 )
		{
			return pszString;
		}
	}

	return NULL;
}


// -----------------------------------------------------------------------------
//		GetMediaSubTypeString [static]
// -----------------------------------------------------------------------------

LPCTSTR
UMediaSpecUtils::GetMediaSubTypeString(
	TMediaSubType	inSubType )
{
	// Check the value against available strings.
	ASSERT( (size_t) inSubType < COUNTOF(s_pszMediaSubTypeStrings) );
	if ( (size_t) inSubType < COUNTOF(s_pszMediaSubTypeStrings) )
	{
		// Get the string.
		LPCTSTR	pszString = s_pszMediaSubTypeStrings[inSubType];

		if ( pszString[0] != 0 )
		{
			return pszString;
		}
	}

	return NULL;
}


// -----------------------------------------------------------------------------
//		GetMediaClassString [static]
// -----------------------------------------------------------------------------

LPCTSTR
UMediaSpecUtils::GetMediaClassString(
	TMediaSpecClass	inClass )
{
	// Check the value against available strings.
	ASSERT( (size_t) inClass < COUNTOF(s_pszClassStrings) );
	if ( (size_t) inClass < COUNTOF(s_pszClassStrings) )
	{
		// Get the string.
		LPCTSTR	pszString = s_pszClassStrings[inClass];

		if ( pszString[0] != 0 )
		{
			return pszString;
		}
	}

	return NULL;
}


// -----------------------------------------------------------------------------
//		GetMediaTypeString [static]
// -----------------------------------------------------------------------------

LPCTSTR
UMediaSpecUtils::GetMediaTypeString(
	TMediaSpecType	inType )
{
	// Check the value against available strings.
	ASSERT( (size_t) inType < COUNTOF(s_pszTypeStrings) );
	if ( (size_t) inType < COUNTOF(s_pszTypeStrings) )
	{
		// Get the string.
		LPCTSTR	pszString = s_pszTypeStrings[inType];

		if ( pszString[0] != 0 )
		{
			return pszString;
		}
	}

	return NULL;
}


// -----------------------------------------------------------------------------
//		GetMediaTypeFromString [static]
// -----------------------------------------------------------------------------

TMediaSpecType	
UMediaSpecUtils::GetMediaTypeFromString( 
	LPCTSTR	inString )
{
	// Sanity checks.
	ASSERT( inString != NULL );
	if ( inString == NULL ) return kMediaSpecType_Null;
	ASSERT( *inString != 0 );
	if ( *inString == 0 ) return kMediaSpecType_Null;

	// Check the string against available strings.
	for ( size_t x = 0; x < COUNTOF(s_pszTypeStrings); x++ )
	{
		if ( _tcsicmp( s_pszTypeStrings[x], inString ) == 0 )
		{
			return (TMediaSpecType) x;
		}
	}

	return kMediaSpecType_Null;
}


// -----------------------------------------------------------------------------
//		GetMediaTypePrefix [static]
// -----------------------------------------------------------------------------

LPCTSTR
UMediaSpecUtils::GetMediaTypePrefix(
	TMediaSpecType	inType )
{
	static LPCTSTR	s_pszPrefixStrings[] =
	{
		_T(""),
		_T("ETHER-"),
		_T("LSAP-"),
		_T("SNAP-"),
		_T("LAP-"),
		_T("DDP-"),
		_T("MACCTL-"),
		_T(""),
		_T(""),
		_T(""),
		_T(""),
		_T(""),
		_T(""),
		_T(""),
		_T(""),
		_T(""),
		_T(""),
		_T(""),
		_T(""),
		_T(""),
		_T("AT-"),
		_T("IP-"),
		_T("DEC-"),
		_T(""),
		_T("IPv6-"),
		_T("IPX-"),
		_T(""),
		_T(""),
		_T(""),
		_T(""),
		_T(""),
		_T(""),
		_T(""),
		_T("AT-"),
		_T("IP-"),
		_T("NW-"),
		_T("TCP-"),
		_T("PPP-"),
		_T("FR-"),
		_T("X.25"),
		_T("X.25/128-"),
		_T("IPARS-"),
		_T("U200-"),
		_T("DLCI-"),
		_T("Q.921"),
	};

	// Check the type against available strings.
	ASSERT( (size_t) inType < COUNTOF(s_pszPrefixStrings) );
	if ( (size_t) inType < COUNTOF(s_pszPrefixStrings) )
	{
		// Get the string.
		LPCTSTR	pszString = s_pszPrefixStrings[inType];

		if ( pszString[0] != 0 )
		{
			return pszString;
		}
	}

	return NULL;
}


// -----------------------------------------------------------------------------
//		GetMediaTypeDefault [static]
// -----------------------------------------------------------------------------

LPCTSTR
UMediaSpecUtils::GetMediaTypeDefault(
	TMediaSpecType	inType )
{
	static LPCTSTR	s_pszDefaultStrings[] =
	{
		_T(""),
		_T("00-00"),
		_T("00"),
		_T("00-00-00-00-00"),
		_T("0"),
		_T("0"),
		_T("0"),
		_T(""),
		_T(""),
		_T(""),
		_T("00:00:00:00:00:00"),
		_T("00:00:00:00:00:00"),
		_T("0"),
		_T("00:00:00:00:00:00"),
		_T(""),
		_T(""),
		_T(""),
		_T(""),
		_T(""),
		_T(""),
		_T("0.0"),
		_T("0.0.0.0"),
		_T("0.0"),
		_T(""),
		_T("0000:0000:0000:0000:0000:0000:0000:0000"),
		_T("0.000000000000"), // -RB- default IPX address.
		_T(""),
		_T(""),
		_T(""),
		_T(""),
		_T(""),
		_T(""),
		_T(""),
		_T("0"),
		_T("0"),
		_T("0"),
		_T("0"),
		_T("00-00"),
		_T("00-00"),
		_T("00-00"),
		_T("00-00"),
		_T("00-00"),
		_T("0"),
		_T("0"),
		_T("0"),
	};

	// Check the type against available strings.
	ASSERT( (size_t) inType < COUNTOF(s_pszDefaultStrings) );
	if ( (size_t) inType < COUNTOF(s_pszDefaultStrings) )
	{
		// Get the string.
		LPCTSTR	pszString = s_pszDefaultStrings[inType];

		if ( pszString[0] != 0 )
		{
			return pszString;
		}
	}

	return NULL;
}


// -----------------------------------------------------------------------------
//		SpecToString [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::SpecToString(
	const CMediaSpec&	inSpec,
	LPTSTR				outString )
{
	// Sanity check.
	ASSERT( outString != NULL );
	if ( outString == NULL ) return false;
//	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return false;

	// Start with an empty string.
	*outString = 0;

	switch ( inSpec.GetClass() )
	{
		case kMediaSpecClass_Protocol:
		{
			switch ( inSpec.GetType() )
			{
				case kMediaSpecType_EthernetProto:
				{
					return FormatEtherProto( inSpec, outString );
				}
				break;

				case kMediaSpecType_LSAP:
				{
					return FormatLSAPProto( inSpec, outString );
				}
				break;

				case kMediaSpecType_SNAP:
				{
					return FormatSNAPProto( inSpec, outString );
				}
				break;

				case kMediaSpecType_ProtoSpec:
				{
#ifndef _NPROTOSPECS
#ifndef AGNETTOOLS				
					return SUCCEEDED( ProtoSpecs::GetPSShortName( *(PROTOSPEC_INSTID*) inSpec.fData, outString, 256 ) );
#endif
#endif
				}
				break;

				case kMediaSpecType_WAN_PPP_Proto:
				{
					return FormatWanPPPProto( inSpec, outString );
				}
				break;

				case kMediaSpecType_WAN_FrameRelay_Proto:
				{
					return FormatWanFrameRelayProto( inSpec, outString );
				}
				break;

				case kMediaSpecType_WAN_X25_Proto:
				{
					return false; // FormatWanX25Proto( inSpec, outString );
				}
				break;

				case kMediaSpecType_WAN_X25e_Proto:
				{
					return false; // FormatWanFrameRelayProto( inSpec, outString );
				}
				break;

				case kMediaSpecType_WAN_Ipars_Proto:
				{
					return false; // FormatWanFrameRelayProto( inSpec, outString );
				}
				break;

				case kMediaSpecType_WAN_U200_Proto:
				{
					return false; // FormatWanFrameRelayProto( inSpec, outString );
				}
				break;

				case kMediaSpecType_WAN_Q931_Proto:
				{
					return false; // FormatWanFrameRelayProto( inSpec, outString );
				}
				break;

				default:
				{
				}
				break;
			}
		}
		break;

		case kMediaSpecClass_Address:
		{
			switch ( inSpec.GetType() )
			{
				case kMediaSpecType_EthernetAddr:
				case kMediaSpecType_TokenRingAddr:
				{
					return FormatPhysicalAddr( inSpec, outString );
				}
				break;

				case kMediaSpecType_WAN_DLCIAddr:
				{
					return FormatWANDLCIAddr( inSpec, outString );
				}

				case kMediaSpecType_LAPAddr:
				{
					// LocalPeek address type not supported.
					ASSERT( inSpec.GetType() != kMediaSpecType_LAPAddr );
				}
				break;

				case kMediaSpecType_WirelessAddr:
				{
					return FormatPhysicalAddr( inSpec, outString );
				}
				break;

				case kMediaSpecType_AppleTalkAddr:
				{
					return FormatAppleTalkAddr( inSpec, outString );
				}
				break;

				case kMediaSpecType_IPAddr:
				{
					return FormatIPAddr( inSpec, outString );
				}
				break;

				case kMediaSpecType_DECnetAddr:
				{
					return FormatDECnetAddr( inSpec, outString );
				}
				break;

				case kMediaSpecType_IPv6Addr:
				{
					return FormatIPv6Addr( inSpec, outString );
				}
				break;

				case kMediaSpecType_IPXAddr:
				{
					return FormatIPXAddr( inSpec, outString );
				}
				break;

				default:
				{
				}
				break;
			}
		}
		break;

		case kMediaSpecClass_Port:
		{
			switch ( inSpec.GetType() )
			{
				case kMediaSpecType_ATPort:
				{
					return FormatATPort( inSpec, outString );
				}
				break;

				case kMediaSpecType_IPPort:
				{
					return FormatIPPort( inSpec, outString );
				}
				break;

				case kMediaSpecType_NWPort:
				{
					return FormatNWPort( inSpec, outString );
				}
				break;

				case kMediaSpecType_TCPPortPair:
				{
					return FormatTCPPortPair( inSpec, outString );
				}
				break;

				default:
				{
				}
				break;
			}
		}
		break;

		default:
		{
		}
		break;
	}

	return false;
}


// -----------------------------------------------------------------------------
//		StringToSpec [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::StringToSpec(
	LPCTSTR			inString,
	TMediaSpecType	inType,
	CMediaSpec&		outSpec )
{
	// Sanity check.
	ASSERT( inString != NULL );
	if ( inString == NULL ) return false;

	// Parse the string based on type.
	switch ( inType )
	{
		case kMediaSpecType_EthernetProto:
		{
			return ParseEtherProto( inString, outSpec );
		}
		break;

		case kMediaSpecType_LSAP:
		{
			return ParseLSAPProto( inString, outSpec );
		}
		break;

		case kMediaSpecType_SNAP:
		{
			return ParseSNAPProto( inString, outSpec );
		}
		break;

		case kMediaSpecType_EthernetAddr:
		case kMediaSpecType_TokenRingAddr:
		case kMediaSpecType_WirelessAddr:
		{
			if ( ParsePhysicalAddr( inString, inType, outSpec ) )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		break;

		case kMediaSpecType_WAN_DLCIAddr:
		{
			if ( ParseWANDLCIAddr( inString, outSpec ) )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		break;

		case kMediaSpecType_AppleTalkAddr:
		{
			return ParseAppleTalkAddr( inString, outSpec );
		}
		break;

		case kMediaSpecType_IPAddr:
		{
			return ParseIPAddr( inString, outSpec );
		}
		break;

		case kMediaSpecType_DECnetAddr:
		{
			return ParseDECnetAddr( inString, outSpec );
		}
		break;

		case kMediaSpecType_IPv6Addr:
		{
			return ParseIPv6Addr( inString, outSpec );
		}
		break;

		case kMediaSpecType_IPXAddr:
		{
			return ParseIPXAddr( inString, outSpec );
		}
		break;

		case kMediaSpecType_IPPort:
		{
			return ParseIPPort( inString, outSpec );
		}
		break;

		case kMediaSpecType_NWPort:
		{
			return ParseNWPort( inString, outSpec );
		}
		break;

		case kMediaSpecType_ATPort:
		{
			return ParseATPort( inString, outSpec );
		}
		break;

		case kMediaSpecType_WAN_PPP_Proto:
		{
			return ParseWANPPPProto( inString, outSpec );
		}
		break;

		case kMediaSpecType_WAN_FrameRelay_Proto:
		{
			return ParseWANFrameRelayProto( inString, outSpec );
		}
		break;

		case kMediaSpecType_WAN_X25_Proto:
		{
			return false; // ParseWANPPPProto( inString, outSpec );
		}
		break;

		case kMediaSpecType_WAN_X25e_Proto:
		{
			return false; // ParseWANPPPProto( inString, outSpec );
		}
		break;

		case kMediaSpecType_WAN_U200_Proto:
		{
			return false; // ParseWANPPPProto( inString, outSpec );
		}
		break;

		case kMediaSpecType_WAN_Ipars_Proto:
		{
			return false; // ParseWANPPPProto( inString, outSpec );
		}
		break;

		case kMediaSpecType_WAN_Q931_Proto:
		{
			return false; // ParseWANPPPProto( inString, outSpec );
		}
		break;
		default:
		{
#if TARGET_OS_MAC
			SignalCStr_( "CNameTable::StringToSpec doesn't support this type." );
#endif
		}
		break;
	}

	return false;
}


// -----------------------------------------------------------------------------
//		FormatEtherProto [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::FormatEtherProto(
	const CMediaSpec&	inSpec,
	LPTSTR				outString )
{
	// Sanity check.
	ASSERT( outString != NULL );
	if ( outString == NULL ) return false;
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return false;
	ASSERT( inSpec.GetClass() == kMediaSpecClass_Protocol );
	ASSERT( inSpec.GetType() == kMediaSpecType_EthernetProto );

	// Start with an empty string.
	*outString = 0;

	// Special case: full mask.
	if ( inSpec.fMask == inSpec.GetTypeMask() )
	{
		_stprintf( outString, _T("%2.2X-%2.2X"),
			inSpec.fData[0], inSpec.fData[1] );
	}
	else
	{
		UInt32	nMask = 0x80000000;
		for ( int i = 0; i < 2; i++ )
		{
			if ( (inSpec.fMask & nMask) != 0 )
			{
				TCHAR	szText[8];
				_stprintf( szText, _T("%2.2X"), inSpec.fData[i] );
				_tcscat( outString, szText );
			}
			else
			{
				// Wildcard character.
				_tcscat( outString, _T("*") );
			}

			if ( i < 1 )
			{
				// Add a delimiter.
				_tcscat( outString, _T("-") );
			}

			// Move the mask.
			nMask >>= 1;
		}
	}

	return true;
}

// -----------------------------------------------------------------------------
//		FormatWanPPPProto [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::FormatWanPPPProto(
	const CMediaSpec&	inSpec,
	LPTSTR				outString )
{
	// Sanity check.
	ASSERT( outString != NULL );
	if ( outString == NULL ) return false;
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return false;
	ASSERT( inSpec.GetClass() == kMediaSpecClass_Protocol );
	ASSERT( inSpec.GetType() == kMediaSpecType_WAN_PPP_Proto );

	// Start with an empty string.
	*outString = 0;

	// Special case: full mask.
	if ( inSpec.fMask == inSpec.GetTypeMask() )
	{
		_stprintf( outString, _T("%2.2X-%2.2X"),
			inSpec.fData[0], inSpec.fData[1] );
	}
	else
	{
		UInt32	nMask = 0x80000000;
		for ( int i = 0; i < 2; i++ )
		{
			if ( (inSpec.fMask & nMask) != 0 )
			{
				TCHAR	szText[8];
				_stprintf( szText, _T("%2.2X"), inSpec.fData[i] );
				_tcscat( outString, szText );
			}
			else
			{
				// Wildcard character.
				_tcscat( outString, _T("*") );
			}

			if ( i < 1 )
			{
				// Add a delimiter.
				_tcscat( outString, _T("-") );
			}

			// Move the mask.
			nMask >>= 1;
		}
	}

	return true;
}

// -----------------------------------------------------------------------------
//		FormatWanFrameRelayProto [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::FormatWanFrameRelayProto(
	const CMediaSpec&	inSpec,
	LPTSTR				outString )
{
	// Sanity check.
	ASSERT( outString != NULL );
	if ( outString == NULL ) return false;
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return false;
	ASSERT( inSpec.GetClass() == kMediaSpecClass_Protocol );
	ASSERT( inSpec.GetType() == kMediaSpecType_WAN_FrameRelay_Proto );

	// Start with an empty string.
	*outString = 0;

	// Special case: full mask.
	if ( inSpec.fMask == inSpec.GetTypeMask() )
	{
		_stprintf( outString, _T("%2.2X-%2.2X"),
			inSpec.fData[0], inSpec.fData[1] );
	}
	else
	{
		UInt32	nMask = 0x80000000;
		for ( int i = 0; i < 2; i++ )
		{
			if ( (inSpec.fMask & nMask) != 0 )
			{
				TCHAR	szText[8];
				_stprintf( szText, _T("%2.2X"), inSpec.fData[i] );
				_tcscat( outString, szText );
			}
			else
			{
				// Wildcard character.
				_tcscat( outString, _T("*") );
			}

			if ( i < 1 )
			{
				// Add a delimiter.
				_tcscat( outString, _T("-") );
			}

			// Move the mask.
			nMask >>= 1;
		}
	}

	return true;
}

// -----------------------------------------------------------------------------
//		FormatWanX25Proto [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::FormatWanX25Proto(
	const CMediaSpec&	inSpec,
	LPTSTR				outString )
{
	// Sanity check.
	ASSERT( outString != NULL );
	if ( outString == NULL ) return false;
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return false;
	ASSERT( inSpec.GetClass() == kMediaSpecClass_Protocol );
	ASSERT( inSpec.GetType() == kMediaSpecType_WAN_X25_Proto );

	// Start with an empty string.
	*outString = 0;

	// Special case: full mask.
	if ( inSpec.fMask == inSpec.GetTypeMask() )
	{
		_stprintf( outString, _T("%2.2X-%2.2X"),
			inSpec.fData[0], inSpec.fData[1] );
	}
	else
	{
		UInt32	nMask = 0x80000000;
		for ( int i = 0; i < 2; i++ )
		{
			if ( (inSpec.fMask & nMask) != 0 )
			{
				TCHAR	szText[8];
				_stprintf( szText, _T("%2.2X"), inSpec.fData[i] );
				_tcscat( outString, szText );
			}
			else
			{
				// Wildcard character.
				_tcscat( outString, _T("*") );
			}

			if ( i < 1 )
			{
				// Add a delimiter.
				_tcscat( outString, _T("-") );
			}

			// Move the mask.
			nMask >>= 1;
		}
	}

	return true;
}


// -----------------------------------------------------------------------------
//		FormatLSAPProto [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::FormatLSAPProto(
	const CMediaSpec&	inSpec,
	LPTSTR				outString )
{
	// Sanity check.
	ASSERT( outString != NULL );
	if ( outString == NULL ) return false;
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return false;
	ASSERT( inSpec.GetClass() == kMediaSpecClass_Protocol );
	ASSERT( inSpec.GetType() == kMediaSpecType_LSAP );

	// Start with an empty string.
	*outString = 0;

	// Special case: full mask.
	if ( inSpec.fMask == inSpec.GetTypeMask() )
	{
		_stprintf( outString, _T("%2.2X"), inSpec.fData[0] );
	}
	else
	{
		// Wildcard LSAP value: kinda useless.
		ASSERT( inSpec.fMask == 0 );
		_tcscpy( outString, _T("*") );
	}

	return true;
}


// -----------------------------------------------------------------------------
//		FormatSNAPProto [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::FormatSNAPProto(
	const CMediaSpec&	inSpec,
	LPTSTR				outString )
{
	// Sanity check.
	ASSERT( outString != NULL );
	if ( outString == NULL ) return false;
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return false;
	ASSERT( inSpec.GetClass() == kMediaSpecClass_Protocol );
	ASSERT( inSpec.GetType() == kMediaSpecType_SNAP );

	// Start with an empty string.
	*outString = 0;

	// Special case: full mask.
	if ( inSpec.fMask == inSpec.GetTypeMask() )
	{
		_stprintf( outString, _T("%2.2X-%2.2X-%2.2X-%2.2X-%2.2X"),
			inSpec.fData[0], inSpec.fData[1], inSpec.fData[2],
			inSpec.fData[3], inSpec.fData[4] );
	}
	else
	{
		UInt32	nMask = 0x80000000;
		for ( int i = 0; i < 5; i++ )
		{
			if ( (inSpec.fMask & nMask) != 0 )
			{
				TCHAR	szText[8];
				_stprintf( szText, _T("%2.2X"), inSpec.fData[i] );
				_tcscat( outString, szText );
			}
			else
			{
				// Wildcard character.
				_tcscat( outString, _T("*") );
			}

			if ( i < 4 )
			{
				// Add a delimiter.
				_tcscat( outString, _T("-") );
			}

			// Move the mask.
			nMask >>= 1;
		}
	}

	return true;
}


// -----------------------------------------------------------------------------
//		FormatPhysicalAddr [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::FormatPhysicalAddr(
	const CMediaSpec&	inSpec,
	LPTSTR				outString )
{
	// Sanity check.
	ASSERT( outString != NULL );
	if ( outString == NULL ) return false;
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return false;
	ASSERT( inSpec.GetClass() == kMediaSpecClass_Address );
	ASSERT( (inSpec.GetType() == kMediaSpecType_EthernetAddr) ||
		(inSpec.GetType() == kMediaSpecType_TokenRingAddr) ||
		(inSpec.GetType() == kMediaSpecType_WirelessAddr) );

	// Start with an empty string.
	*outString = 0;

	// Special case: full mask.
	if ( inSpec.fMask == inSpec.GetTypeMask() )
	{
		_stprintf( outString, _T("%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X"),
			inSpec.fData[0], inSpec.fData[1], inSpec.fData[2],
			inSpec.fData[3], inSpec.fData[4], inSpec.fData[5] );
	}
	else
	{
		UInt32	nMask = 0x80000000;
		for ( int i = 0; i < 6; i++ )
		{
			if ( (inSpec.fMask & nMask) != 0 )
			{
				TCHAR	szText[8];
				_stprintf( szText, _T("%2.2X"), inSpec.fData[i] );
				_tcscat( outString, szText );
			}
			else
			{
				// Wildcard character.
				_tcscat( outString, _T("*") );
			}

			if ( i < 5 )
			{
				// Add a delimiter.
				_tcscat( outString, _T(":") );
			}

			// Move the mask.
			nMask >>= 1;
		}
	}

	return true;
}

// -----------------------------------------------------------------------------
//		FormatWANDLCIAddr [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::FormatWANDLCIAddr(
	const CMediaSpec&	inSpec,
	LPTSTR				outString )
{
	// Sanity check.
	ASSERT( outString != NULL );
	if ( outString == NULL ) return false;
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return false;
	ASSERT( inSpec.GetClass() == kMediaSpecClass_Address );
	ASSERT( (inSpec.GetType() == kMediaSpecType_WAN_DLCIAddr) );

	// Start with an empty string.
	*outString = 0;

	// Special case: full mask.
	if ( inSpec.fMask == inSpec.GetTypeMask() )
	{
		_stprintf( outString, _T("%d"),
			*((UInt16*) inSpec.fData) );
	}
	else
	{
		UInt32	nMask = 0x80000000;
		for ( int i = 0; i < 6; i++ )
		{
			if ( (inSpec.fMask & nMask) != 0 )
			{
				TCHAR	szText[8];
				_stprintf( szText, _T("%2.2X"), inSpec.fData[i] );
				_tcscat( outString, szText );
			}
			else
			{
				// Wildcard character.
				_tcscat( outString, _T("*") );
			}

			if ( i < 5 )
			{
				// Add a delimiter.
				_tcscat( outString, _T(":") );
			}

			// Move the mask.
			nMask >>= 1;
		}
	}

	return true;
}

// -----------------------------------------------------------------------------
//		FormatAppleTalkAddr [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::FormatAppleTalkAddr(
	const CMediaSpec&	inSpec,
	LPTSTR				outString )
{
	// Sanity check.
	ASSERT( outString != NULL );
	if ( outString == NULL ) return false;
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return false;
	ASSERT( inSpec.GetClass() == kMediaSpecClass_Address );
	ASSERT( inSpec.GetType() == kMediaSpecType_AppleTalkAddr );

	// Start with an empty string.
	*outString = 0;

	// Extract the AppleTalk network and node.
	// Note: data in a media spec is always network byte order.
	UInt16	nNetwork = NETWORKTOHOST16( *(UInt16*) &inSpec.fData[0] );
	UInt8	nNode = inSpec.fData[2];

	// There are a limited number of possibilities
	// for the mask, so handle each case and be strict
	// about allowed mask values.
	if ( inSpec.fMask == 0xE0000000 )
	{
		// Nothing wild.
		_stprintf( outString, _T("%u.%u"), nNetwork, nNode );
	}
	else if ( inSpec.fMask == 0xC0000000 )
	{
		// Node is wild.
		_stprintf( outString, _T("%u.*"), nNetwork );
	}
	else if ( inSpec.fMask == 0x20000000 )
	{
		// Network is wild.
		_stprintf( outString, _T("*.%u"), nNode );
	}
	else
	{
		// Invalid mask.
	}

	return true;
}


// -----------------------------------------------------------------------------
//		FormatIPAddr [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::FormatIPAddr(
	const CMediaSpec&	inSpec,
	LPTSTR				outString )
{
	// Sanity check.
	ASSERT( outString != NULL );
	if ( outString == NULL ) return false;
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return false;
	ASSERT( inSpec.GetClass() == kMediaSpecClass_Address );
	ASSERT( inSpec.GetType() == kMediaSpecType_IPAddr );

	// Start with an empty string.
	*outString = 0;

	// Special case: full mask.
	if ( inSpec.fMask == inSpec.GetTypeMask() )
	{
		_stprintf( outString, _T("%u.%u.%u.%u"),
			inSpec.fData[0], inSpec.fData[1],
			inSpec.fData[2], inSpec.fData[3] );
	}
	else
	{
		UInt32	nMask = 0xFF000000;
		bool	bIsSimpleMask = true;
		
		// figure out if mask is simple class A, B, C, or none
		int	i = 0;
		for ( i = 0; i < 4; i++ )
		{
			if ( (inSpec.fMask & nMask) != nMask )
			{
				bIsSimpleMask = false;
				break;
			}
			
			nMask >>= 8;
		}
		
		// if it's simple, print it out as nnn.nnn.nnn.* as appropriate
		if ( bIsSimpleMask )
		{
			nMask = 0xFF000000;
			
			for ( int i = 0; i < 4; i++ )
			{
				if ( (inSpec.fMask & nMask) != 0 )
				{
					TCHAR	szText[8];
					_stprintf( szText, _T("%u"), inSpec.fData[i] );
					_tcscat( outString, szText );
				}
				else
				{
					// Wildcard character.
					_tcscat( outString, _T("*") );
				}

				if ( i < 3 )
				{
					// Add a delimiter.
					_tcscat( outString, _T(".") );
				}

				// Move the mask.
				nMask >>= 8;
			}
		}
		else
		{
			// not simple mask, print as nnn.nnn.nnn.nnn/mm
			// get the IP address in host byte order
			UInt32	nValue = NETWORKTOHOST32( *(UInt32*)inSpec.fData );
			
			// apply the mask (mask is in host byte order)
			nValue = nValue & inSpec.fMask;
			
			// convert back to network byte order
			nValue = HOSTTONETWORK32( nValue );
			
			// get a pointer to the bytes
			UInt8*	pBytes = (UInt8*)&nValue;
			
			// count the number of network bits
			UInt8	nCount = 0;
			UInt32	nMask = 0x80000000;
			while ( (nMask & inSpec.fMask) != 0 )
			{
				nCount++;
				nMask >>= 1;
			}
			
			// convert data to string
			_stprintf( outString, _T("%u.%u.%u.%u/%u"),
				pBytes[0], pBytes[1], pBytes[2], pBytes[3], nCount );
		}
	}

	return true;
}


// -----------------------------------------------------------------------------
//		FormatDECnetAddr [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::FormatDECnetAddr(
	const CMediaSpec&	inSpec,
	LPTSTR				outString )
{
	// Sanity check.
	ASSERT( outString != NULL );
	if ( outString == NULL ) return false;
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return false;
	ASSERT( inSpec.GetClass() == kMediaSpecClass_Address );
	ASSERT( inSpec.GetType() == kMediaSpecType_DECnetAddr );

	// Start with an empty string.
	*outString = 0;

	UInt16	nAddr = LITTLETOHOST16( *(UInt16*) inSpec.fData );
	UInt16	nArea = (UInt16)(nAddr >> 10);
	UInt16	nNode = (UInt16)(nAddr & 0x03FF);

	if ( (inSpec.fMask & 0x80000000) != 0 )
	{
		_stprintf( outString, _T("%u."), nArea );
	}
	else
	{
		_stprintf( outString, _T("*.") );
	}

	if ( (inSpec.fMask & 0x40000000) != 0 )
	{
		_stprintf( outString, _T("%s%u"), outString, nNode );
	}
	else
	{
		_stprintf( outString, _T("%s*"), outString );
	}

	return true;
}


// -----------------------------------------------------------------------------
//		FormatIPv6Addr [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::FormatIPv6Addr(
	const CMediaSpec&	inSpec,
	LPTSTR				outString )
{
	// Sanity check.
	ASSERT( outString != NULL );
	if ( outString == NULL ) return false;
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return false;
	ASSERT( inSpec.GetClass() == kMediaSpecClass_Address );
	ASSERT( inSpec.GetType() == kMediaSpecType_IPv6Addr );

	// Start with an empty string.
	*outString = 0;

	// Special case: full mask.
	if ( inSpec.fMask == inSpec.GetTypeMask() )
	{
		// Are there any zero runs?
        int nZeroes = 0;
		int nMostZeroes = 0;
		int nZeroStartPos = 0;
		int nMostZeroStartPos = 0;
		for( int i = 0; i < 16; i += 2 )
		{
			if( *(inSpec.fData + i) == 0 &&
				*(inSpec.fData + i + 1) == 0)
			{
				if( nZeroes == 0)
				{
					nZeroStartPos = i / 2;
				}
				nZeroes++;
			}
			else
			{
				if( nZeroes >= nMostZeroes )
				{
					nMostZeroes = nZeroes;
					nMostZeroStartPos = nZeroStartPos;
					nZeroes = 0;
					nZeroStartPos = 0;
				}
				if( nMostZeroes <= 1 )
				{
					// Don't bother unless it's at least 2.
					nMostZeroes = 0;
					nMostZeroStartPos = 0;
				}
			}
		}

		if( nMostZeroes <= 1 )
		{
			_stprintf( outString,
				_T("%.1X:%.1X:%.1X:%.1X:%.1X:%.1X:%.1X:%.1X"),
				NETWORKTOHOST16( *((UInt16*) (inSpec.fData)) ), 
				NETWORKTOHOST16( *((UInt16*) (inSpec.fData + 2)) ), 
				NETWORKTOHOST16( *((UInt16*) (inSpec.fData + 4)) ),  
				NETWORKTOHOST16( *((UInt16*) (inSpec.fData + 6)) ),  
				NETWORKTOHOST16( *((UInt16*) (inSpec.fData + 8)) ),  
				NETWORKTOHOST16( *((UInt16*) (inSpec.fData + 10)) ),  
				NETWORKTOHOST16( *((UInt16*) (inSpec.fData + 12)) ),  
				NETWORKTOHOST16( *((UInt16*) (inSpec.fData + 14)) )  );
		}
		else
		{
			outString[0] = 0;

			// Output the part before the zeroes.
			TCHAR	szTemp[256];
			for( int i = 0; i < nMostZeroStartPos; i++ )
			{
				_stprintf( szTemp, _T("%.1X"),
					NETWORKTOHOST16( *((UInt16*) (inSpec.fData + (2*i))) ) );
				_tcscat( outString, szTemp );
				_tcscat( outString, _T(":") );
			}

			// Extra colon (2 if no parts before zero run).
			_tcscat( outString, nMostZeroStartPos == 0 ? _T("::") : _T(":") );

			// Now the part after the zeroes.
			for( int i = (nMostZeroStartPos + nMostZeroes); i < 8; i++ )
			{
				_stprintf( szTemp, _T("%.1X"),
					NETWORKTOHOST16( *((UInt16*) (inSpec.fData + (2*i))) ) );
				_tcscat( outString, szTemp );
				if( i < 7 )
				{
					_tcscat( outString, _T(":") );
				}
			}
		}

	}
	else
	{
		UInt32	nMask = 0xC0000000;
		for ( int i = 0; i < 16; i += 2 )
		{
			if ( (nMask & inSpec.fMask) == nMask )
			{
				TCHAR	szText[8];
				_stprintf( szText, _T("%.1X"), NETWORKTOHOST16( *((UInt16*) (inSpec.fData + i)) ) );
				_tcscat( outString, szText );
			}
			else
			{
				// Can't mask only 1 byte from IPv6 addr.
				ASSERT( (nMask & inSpec.fMask) == 0 );

				// Wildcard character.
				_tcscat( outString, _T("*") );
			}

			if ( i < 14 )
			{
				// Add a delimiter.
				_tcscat( outString, _T(":") );
			}

			// Move the mask.
			nMask >>= 2;
		}
	}

	return true;
}


// -----------------------------------------------------------------------------
//		FormatIPXAddr [static]
// -----------------------------------------------------------------------------
// TODO: UMediaSpecUtils::FormatIPXAddr() does not handle wildcards/masks

bool
UMediaSpecUtils::FormatIPXAddr(
	const CMediaSpec&	inSpec,
	LPTSTR				outString )
{
	// Sanity check.
	ASSERT( outString != NULL );
	if ( outString == NULL ) return false;
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return false;
	ASSERT( inSpec.GetClass() == kMediaSpecClass_Address );
	ASSERT( inSpec.GetType() == kMediaSpecType_IPXAddr );
	ASSERT( inSpec.fMask == inSpec.GetTypeMask() );

	// Start with an empty string.
	*outString = 0;

	_stprintf( outString, _T("%lX.%.8lX%.4X"), 
		NETWORKTOHOST32( (*(UInt32*) &inSpec.fData[0]) ),
		NETWORKTOHOST32( (*(UInt32*) &inSpec.fData[4]) ),
		NETWORKTOHOST16( (*(UInt16*) &inSpec.fData[8]) ) );

	return true;
}


// -----------------------------------------------------------------------------
//		FormatIPPort [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::FormatIPPort(
	const CMediaSpec&	inSpec,
	LPTSTR				outString )
{
	// Sanity check.
	ASSERT( outString != NULL );
	if ( outString == NULL ) return false;
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return false;
	ASSERT( inSpec.GetClass() == kMediaSpecClass_Port );
	ASSERT( inSpec.GetType() == kMediaSpecType_IPPort );
	ASSERT( inSpec.fMask == 0xC0000000 );

	UInt16	nValue = NETWORKTOHOST16( *(UInt16*) inSpec.fData );
	_stprintf( outString, _T("%u"), nValue );

	return true;
}


// -----------------------------------------------------------------------------
//		FormatTCPPPortPair [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::FormatTCPPortPair(
	const CMediaSpec&	inSpec,
	LPTSTR				outString )
{
	// Sanity check.
	ASSERT( outString != NULL );
	if ( outString == NULL ) return false;
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return false;
	ASSERT( inSpec.GetClass() == kMediaSpecClass_Port );
	ASSERT( inSpec.GetType() == kMediaSpecType_TCPPortPair );
	ASSERT( inSpec.fMask == 0xF0000000 );

	UInt16	nSrcPort = NETWORKTOHOST16( *(UInt16*) inSpec.fData );
	UInt16  nDestPort = NETWORKTOHOST16( *(((UInt16*) inSpec.fData) + 1) );

	_stprintf( outString, _T("%u->%u"), nSrcPort, nDestPort );

	return true;
}

// -----------------------------------------------------------------------------
//		FormatATPort [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::FormatATPort(
	const CMediaSpec&	inSpec,
	LPTSTR				outString )
{
	// Sanity check.
	ASSERT( outString != NULL );
	if ( outString == NULL ) return false;
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return false;
	ASSERT( inSpec.GetClass() == kMediaSpecClass_Port );
	ASSERT( inSpec.GetType() == kMediaSpecType_ATPort );
	ASSERT( inSpec.fMask == 0x80000000 );

	_stprintf( outString, _T("%u"), inSpec.fData[0] );

	return true;
}


// -----------------------------------------------------------------------------
//		FormatNWPort [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::FormatNWPort(
	const CMediaSpec&	inSpec,
	LPTSTR				outString )
{
	// Sanity check.
	ASSERT( outString != NULL );
	if ( outString == NULL ) return false;
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return false;
	ASSERT( inSpec.GetClass() == kMediaSpecClass_Port );
	ASSERT( inSpec.GetType() == kMediaSpecType_NWPort );
	ASSERT( inSpec.fMask == 0xC0000000 );

	_stprintf( outString, _T("0x%2.2X%2.2X"),
		inSpec.fData[0], inSpec.fData[1] );

	return true;
}


// -----------------------------------------------------------------------------
//		ParseEtherProto [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::ParseEtherProto(
	LPCTSTR		inString,
	CMediaSpec&	outSpec )
{
	// Sanity check.
	ASSERT( inString != NULL );
	if ( inString == NULL ) return false;

	// Start with a clear spec.
	outSpec.Clear();

	// Valid formats: 0x809B, $809B, 809B, 80-9B, 60-*, *-9B.

	// Skip leading $ or 0x.
	if ( *inString == _T('$') )
	{
		inString++;
	}
	else if ( (inString[0] == _T('0')) &&
		((inString[1] == _T('x')) || (inString[1] == _T('X'))) )
	{
		inString += 2;
	}

	UInt32	nMask = 0x80000000;
	for ( int i = 0; i < 2; i++ )
	{
		if ( *inString == 0 )
		{
			// Invalid format: ran out of string.
			return false;
		}

		if ( *inString == _T('*') )
		{
			// Wildcard character.
			// Skip to the next character.
			inString += 1;
		}
		else
		{
			// Parse the next hex byte.
			if ( !HexStrToByte( inString, outSpec.fData[i] ) )
			{
				// Invalid format.
				return false;
			}

			// Add the valid mask bits.
			outSpec.fMask |= nMask;

			// Skip two characters.
			inString += 2;
		}

		// Skip delimiters.
		if ( *inString == _T('-') )
		{
			inString++;
		}

		// Shift the mask.
		nMask >>= 1;
	}

	// Make sure there's nothing extra on the end.
	if ( *inString != 0 )
	{
		return false;
	}

	// Setup the media spec.
	outSpec.SetClass( kMediaSpecClass_Protocol );
	outSpec.SetType( kMediaSpecType_EthernetProto );

	return true;
}


// -----------------------------------------------------------------------------
//		ParseWANPPPProto [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::ParseWANPPPProto(
	LPCTSTR		inString,
	CMediaSpec&	outSpec )
{
	// Sanity check.
	ASSERT( inString != NULL );
	if ( inString == NULL ) return false;

	// Start with a clear spec.
	outSpec.Clear();

	// Valid formats: 0x809B, $809B, 809B, 80-9B, 60-*, *-9B.

	// Skip leading $ or 0x.
	if ( *inString == _T('$') )
	{
		inString++;
	}
	else if ( (inString[0] == _T('0')) &&
		((inString[1] == _T('x')) || (inString[1] == _T('X'))) )
	{
		inString += 2;
	}

	UInt32	nMask = 0x80000000;
	for ( int i = 0; i < 2; i++ )
	{
		if ( *inString == 0 )
		{
			// Invalid format: ran out of string.
			return false;
		}

		if ( *inString == _T('*') )
		{
			// Wildcard character.
			// Skip to the next character.
			inString += 1;
		}
		else
		{
			// Parse the next hex byte.
			if ( !HexStrToByte( inString, outSpec.fData[i] ) )
			{
				// Invalid format.
				return false;
			}

			// Add the valid mask bits.
			outSpec.fMask |= nMask;

			// Skip two characters.
			inString += 2;
		}

		// Skip delimiters.
		if ( *inString == _T('-') )
		{
			inString++;
		}

		// Shift the mask.
		nMask >>= 1;
	}

	// Make sure there's nothing extra on the end.
	if ( *inString != 0 )
	{
		return false;
	}

	// Setup the media spec.
	outSpec.SetClass( kMediaSpecClass_Protocol );
	outSpec.SetType( kMediaSpecType_WAN_PPP_Proto );

	return true;
}

// -----------------------------------------------------------------------------
//		ParseWANFrameRelayProto [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::ParseWANFrameRelayProto(
	LPCTSTR		inString,
	CMediaSpec&	outSpec )
{
	// Sanity check.
	ASSERT( inString != NULL );
	if ( inString == NULL ) return false;

	// Start with a clear spec.
	outSpec.Clear();

	// Valid formats: 0x809B, $809B, 809B, 80-9B, 60-*, *-9B.

	// Skip leading $ or 0x.
	if ( *inString == _T('$') )
	{
		inString++;
	}
	else if ( (inString[0] == _T('0')) &&
		((inString[1] == _T('x')) || (inString[1] == _T('X'))) )
	{
		inString += 2;
	}

	UInt32	nMask = 0x80000000;
	for ( int i = 0; i < 2; i++ )
	{
		if ( *inString == 0 )
		{
			// Invalid format: ran out of string.
			return false;
		}

		if ( *inString == _T('*') )
		{
			// Wildcard character.
			// Skip to the next character.
			inString += 1;
		}
		else
		{
			// Parse the next hex byte.
			if ( !HexStrToByte( inString, outSpec.fData[i] ) )
			{
				// Invalid format.
				return false;
			}

			// Add the valid mask bits.
			outSpec.fMask |= nMask;

			// Skip two characters.
			inString += 2;
		}

		// Skip delimiters.
		if ( *inString == _T('-') )
		{
			inString++;
		}

		// Shift the mask.
		nMask >>= 1;
	}

	// Make sure there's nothing extra on the end.
	if ( *inString != 0 )
	{
		return false;
	}

	// Setup the media spec.
	outSpec.SetClass( kMediaSpecClass_Protocol );
	outSpec.SetType( kMediaSpecType_WAN_FrameRelay_Proto );

	return true;
}

// -----------------------------------------------------------------------------
//		ParseWANDLCIAddr [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::ParseWANDLCIAddr(
	LPCTSTR		inString,
	CMediaSpec&	outSpec )
{
	// Sanity check.
	ASSERT( inString != NULL );
	if ( inString == NULL ) return false;

	// Start with a clear spec.
	outSpec.Clear();

	// Valid formats: (decimal number).
	TCHAR* pszLast = NULL;
	UInt32 nCvt = _tcstoul( inString, &pszLast, 10 );
	if (pszLast != inString + _tcslen(inString))	
		return false;	// unknown decimal chars here

	if (nCvt > (1 << 10))
		return false;

	// Setup the media spec.
	*((UInt16*) outSpec.fData) = (UInt16) nCvt;
	outSpec.SetClass( kMediaSpecClass_Address );
	outSpec.SetType( kMediaSpecType_WAN_DLCIAddr );
	outSpec.fMask = outSpec.GetTypeMask();

	return true;
}

// -----------------------------------------------------------------------------
//		ParseLSAPProto [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::ParseLSAPProto(
	LPCTSTR		inString,
	CMediaSpec&	outSpec )
{
	// Sanity check.
	ASSERT( inString != NULL );
	if ( inString == NULL ) return false;

	// Start with a clear spec.
	outSpec.Clear();

	// Valid formats: 0xE0, $E0, E0.

	// Skip leading $ or 0x.
	if ( *inString == _T('$') )
	{
		inString++;
	}
	else if ( (inString[0] == _T('0')) &&
		((inString[1] == _T('x')) || (inString[1] == _T('X'))) )
	{
		inString += 2;
	}

	if ( *inString == 0 )
	{
		// Invalid format: ran out of string.
		return false;
	}

	if ( *inString == _T('*') )
	{
		// Wildcard LSAP value: kinda useless, but still valid.
		outSpec.SetClass( kMediaSpecClass_Protocol );
		outSpec.SetType( kMediaSpecType_LSAP );
		outSpec.fMask = 0;
	}
	else
	{
		// Parse the next hex byte.
		if ( HexStrToByte( inString, outSpec.fData[0] ) )
		{
			// Valid LSAP value.
			outSpec.SetClass( kMediaSpecClass_Protocol );
			outSpec.SetType( kMediaSpecType_LSAP );
			outSpec.fMask = outSpec.GetTypeMask();
		}
		else
		{
			// Invalid format.
			return false;
		}
	}

	return true;
}


// -----------------------------------------------------------------------------
//		ParseSNAPProto [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::ParseSNAPProto(
	LPCTSTR		inString,
	CMediaSpec&	outSpec )
{
	// Sanity check.
	ASSERT( inString != NULL );
	if ( inString == NULL ) return false;

	// Start with a clear spec.
	outSpec.Clear();

	// Valid formats: 0x800007809B, $800007809B,
	// 800007809B, 80-00-07-80-9B, 80-*-07-*-9B.

	// Skip leading $ or 0x.
	if ( *inString == _T('$') )
	{
		inString++;
	}
	else if ( (inString[0] == _T('0')) &&
		((inString[1] == _T('x')) || (inString[1] == _T('X'))) )
	{
		inString += 2;
	}

	UInt32	nMask = 0x80000000;
	for ( int i = 0; i < 5; i++ )
	{
		if ( *inString == 0 )
		{
			// Invalid format: ran out of string.
			return false;
		}

		if ( *inString == _T('*') )
		{
			// Wildcard character.
			// Skip to the next character.
			inString += 1;
		}
		else
		{
			// Parse the next hex byte.
			if ( !HexStrToByte( inString, outSpec.fData[i] ) )
			{
				// Invalid format.
				return false;
			}

			// Add the valid mask bits.
			outSpec.fMask |= nMask;

			// Skip two characters.
			inString += 2;
		}

		// Skip delimiters.
		if ( *inString == _T('-') )
		{
			inString++;
		}

		// Shift the mask.
		nMask >>= 1;
	}

	// Make sure there's nothing extra on the end.
	if ( *inString != 0 )
	{
		return false;
	}

	// Setup the media spec.
	outSpec.SetClass( kMediaSpecClass_Protocol );
	outSpec.SetType( kMediaSpecType_SNAP );

	return true;
}


// -----------------------------------------------------------------------------
//		ParsePhysicalAddr [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::ParsePhysicalAddr(
	LPCTSTR			inString,
	TMediaSpecType	inType,
	CMediaSpec&		outSpec )
{
	// Sanity check.
	ASSERT( inString != NULL );
	if ( inString == NULL ) return false;

	// Start with a clear spec.
	outSpec.Clear();

	// Valid formats: 08002B107640, 08:00:2B:10:76:40,
	// 08-00-2B-10-76-40, 08.00.2B.10.76.40.

	UInt32	nMask = 0x80000000;
	for ( int i = 0; i < 6; i++ )
	{
		if ( *inString == 0 )
		{
			// Invalid format: ran out of string.
			return false;
		}

		if ( *inString == _T('*') )
		{
			// Wildcard character.
			// Skip to the next character.
			inString += 1;
		}
		else
		{
			// Parse the next hex byte.
			if ( !HexStrToByte( inString, outSpec.fData[i] ) )
			{
				// Invalid format.
				return false;
			}

			// Add the valid mask bits.
			outSpec.fMask |= nMask;

			// Skip two characters.
			inString += 2;
		}

		// Skip delimiters.
		if ( (*inString == _T(':')) ||
			(*inString == _T('-')) ||
			(*inString == _T('.')) )
		{
			inString++;
		}

		// Shift the mask.
		nMask >>= 1;
	}

	// Make sure there's nothing extra on the end.
	if ( *inString != 0 )
	{
		return false;
	}

	// Setup the media spec.
	outSpec.SetClass( kMediaSpecClass_Address );
	outSpec.SetType( inType );

	return true;
}


// -----------------------------------------------------------------------------
//		ParseAppleTalkAddr [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::ParseAppleTalkAddr(
	LPCTSTR		inString,
	CMediaSpec&	outSpec )
{
	// Sanity check.
	ASSERT( inString != NULL );
	if ( inString == NULL ) return false;

	// Start with a clear spec.
	outSpec.Clear();

	// Valid formats: 1000.86, *.255, 1000.*.

	if ( *inString == _T('*') )
	{
		// Network is wild.
		inString++;
	}
	else
	{
		// Read in the decimal network value.
		UInt32	nNetwork = 0;
		while ( *inString != 0 )
		{
			if ( (*inString >= _T('0')) && (*inString <= _T('9')) )
			{
				nNetwork = (UInt32)((nNetwork * 10) + (*inString - _T('0')) );
			}
			else if ( *inString == _T('.') )
			{
				// Hit the delimiter.
				break;
			}
			else
			{
				// Invalid character.
				return false;
			}

			// Move to the next character.
			inString++;
		}

		if ( (nNetwork & 0xFFFF0000) != 0 )
		{
			// Network number too big.
			return false;
		}

		// Setup the network part of the media spec.
		outSpec.fMask |= 0xC0000000;
		*(UInt16*) outSpec.fData = HOSTTONETWORK16( (UInt16) nNetwork );
	}

	if ( *inString == 0 )
	{
		// Invalid format: ran out of string.
		return false;
	}

	if ( *inString != _T('.') )
	{
		// Missing the delimiter.
		return false;
	}

	// Skip the delimiter.
	inString++;

	if ( *inString == 0 )
	{
		// Invalid format: ran out of string.
		return false;
	}

	if ( *inString == _T('*') )
	{
		// Node is wild.
		inString++;
	}
	else
	{
		// Read in the decimal node value.
		UInt32	nNode = 0;
		while ( *inString != 0 )
		{
			if ( (*inString >= _T('0')) && (*inString <= _T('9')) )
			{
				nNode = (UInt32)((nNode * 10) + (*inString - _T('0')) );
			}
			else
			{
				// Invalid character.
				return false;
			}

			// Move to the next character.
			inString++;
		}

		if ( (nNode & 0xFFFFFF00) != 0 )
		{
			// Node number too big.
			return false;
		}

		// Setup the node part of the media spec.
		outSpec.fMask |= 0x20000000;
		outSpec.fData[2] = (UInt8) nNode;
	}

	// Make sure there's nothing extra on the end.
	if ( *inString != 0 )
	{
		return false;
	}

	// Setup the media spec class and type.
	outSpec.SetClass( kMediaSpecClass_Address );
	outSpec.SetType( kMediaSpecType_AppleTalkAddr );

	return true;
}


// -----------------------------------------------------------------------------
//		ParseIPAddr [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::ParseIPAddr(
	LPCTSTR		inString,
	CMediaSpec&	outSpec )
{
	// Sanity check.
	ASSERT( inString != NULL );
	if ( inString == NULL ) return false;

	// Start with a clear spec.
	outSpec.Clear();

	// Valid formats: 192.216.124.1, 192.216.124.*.

	UInt32	nMask = 0xFF000000;
	for ( int i = 0; i < 4; i++ )
	{
		if ( *inString == 0 )
		{
			// Invalid format: ran out of string.
			return false;
		}

		if ( *inString == _T('*') )
		{
			// This part is wild.
			inString++;
		}
		else
		{
			UInt32	nValue = 0;
			while ( *inString != 0 )
			{
				if ( (*inString >= _T('0')) && (*inString <= _T('9')) )
				{
					nValue = (UInt32)((nValue * 10) + (*inString - _T('0')) );
				}
				else if ( *inString == _T('.') ||
					( (*inString == _T('/')) && ( i == 3 ) ) )
				{
					// Hit a delimiter.
					break;
				}
				else
				{
					// Invalid character.
					return false;
				}

				// Move to the next character.
				inString++;
			}

			if ( (nValue & 0xFFFFFF00) != 0 )
			{
				// IP number too big.
				return false;
			}

			// Setup the node part of the media spec.
			outSpec.fMask |= nMask;
			outSpec.fData[i] = (UInt8) nValue;
		}

		if ( *inString == _T('.') )
		{
			// Skip the delimiter.
			inString++;
		}

		// Move the mask.
		nMask >>= 8;
	}
	
	// check for net mask specified using / notation
	if ( *inString == _T( '/' ) )
	{
		inString++;
		
		if ( outSpec.fMask != 0xFFFFFFFF )
		{
			// can't do something like "192.216.124.*/24"
			// must be either "192.216.124.*" or "192.216.124.12/24"
			return false;
		}
		
		int		nCount	= _ttoi( inString );
		
		while ( *inString != 0 )
		{
			inString++;
		}
		
		if ( nCount > 31 || nCount < 1 )
		{
			return false;
		}
		
		outSpec.fMask = (UInt32)(0xFFFFFFFF << (32 - nCount));
		
		// 0 out all bits that aren't part of the network
		*(UInt32*)outSpec.fData = HOSTTONETWORK32( outSpec.fMask & HOSTTONETWORK32( *(UInt32*)outSpec.fData ) );
	}

	// Make sure there's nothing extra on the end.
	if ( *inString != 0 )
	{
		return false;
	}

	// Setup the media spec class and type.
	outSpec.SetClass( kMediaSpecClass_Address );
	outSpec.SetType( kMediaSpecType_IPAddr );

	return true;
}


// -----------------------------------------------------------------------------
//		ParseDECnetAddr [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::ParseDECnetAddr(
	LPCTSTR		inString,
	CMediaSpec&	outSpec )
{
	// Sanity check.
	ASSERT( inString != NULL );
	if ( inString == NULL ) return false;

	// Start with a clear spec.
	outSpec.Clear();

	// Valid formats: ?.

	UInt16	nDECnetAddr = 0;

	// Read in the 6 bit part.
	if ( *inString == _T('*') )
	{
		// Area is wild.
		inString++;
	}
	else
	{
		// Read in the decimal string.
		UInt16	nArea = 0;
		while ( *inString != 0 )
		{
			if ( (*inString >= _T('0')) && (*inString <= _T('9')) )
			{
				nArea = (UInt16)((nArea * 10) + (*inString - _T('0')) );
			}
			else if ( *inString == _T('.') )
			{
				// Hit a delimiter.
				break;
			}
			else
			{
				// Invalid character.
				return false;
			}

			// Move to the next character.
			inString++;
		}

		if ( (nArea & 0xFFC0) != 0 )
		{
			// Area value too big.
			return false;
		}

		nDECnetAddr = (UInt16)(nArea << 10);
		outSpec.fMask |= 0x80000000;
	}

	if ( *inString == 0 )
	{
		// Invalid format: ran out of string.
		return false;
	}

	if ( *inString != _T('.') )
	{
		// Missing the delimiter.
		return false;
	}

	// Skip the delimiter.
	inString++;

	if ( *inString == 0 )
	{
		// Invalid format: ran out of string.
		return false;
	}

	// Read in the node.
	if ( *inString == _T('*') )
	{
		inString++;
	}
	else
	{
		// Read in the decimal string.
		UInt16	nNode = 0;
		while ( *inString != 0 )
		{
			if ( (*inString >= _T('0')) && (*inString <= _T('9')) )
			{
				nNode = (UInt16)((nNode * 10) + (*inString - _T('0')) );
			}
			else
			{
				// Invalid character.
				return false;
			}

			// Move to the next character.
			inString++;
		}

		if ( (nNode & 0xFC00) != 0 )
		{
			// Node value too big.
			return false;
		}

		nDECnetAddr |= nNode;
		outSpec.fMask |= 0x40000000;
	}

	// Make sure there's nothing extra on the end.
	if ( *inString != 0 )
	{
		return false;
	}

	// Setup the media spec.
	outSpec.SetClass( kMediaSpecClass_Address );
	outSpec.SetType( kMediaSpecType_DECnetAddr );
	*(UInt16*) outSpec.fData = HOSTTOLITTLE16( nDECnetAddr );

	return true;
}


// -----------------------------------------------------------------------------
//		ParseIPv6Addr [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::ParseIPv6Addr(
	LPCTSTR		inString,
	CMediaSpec&	outSpec )
{
	// Sanity check.
	ASSERT( inString != NULL );
	if ( inString == NULL ) return false;

	// Start with a clear spec.
	outSpec.Clear();

	// Valid formats: 
	// 0000:00CF:BA4C:1344:0000:00CF:BA4C:1344
	// 0:CF:BA4C:1344:0:CF:BA4C:1344
	// 0:cf:ba4c:1344:0:cf:ab4c:*
	// ::1234
	// 12::34
	// 1:2:3::4

	// IPv6 addresses have 8 2-byte parts.
	UInt32	nMask = 0xC0000000;
	bool	bHasZeroRun = false;
	int		nParts = 0;
	for ( int i = 0; i < 16; i+=2 )
	{
		if ( *inString == 0 )
		{
			// Invalid format: ran out of string.
			return false;
		}

		// Get a part.
		UInt8	nNibble = 0;
		UInt32	nValue = 0;
		if( *inString == _T('*') )
		{
			// Wildcard character, skip it.
			inString++;
		}
		else if( *inString == _T(':') )
		{
			// Zero run.
			bHasZeroRun = true;
			inString++;
			break;
		}
		else
		{
			while( HexCharToValue( *inString, nNibble ) )
			{
				nValue <<= 4;
				nValue += nNibble;
				inString++;
			}

			// Should fit in 16-bits.
			if ( (nValue & 0xFFFF0000) != 0 )
			{
				return false;
			}

			// Pack it into fData.
			outSpec.fData[i] = (UInt8) ((nValue & 0xFF00) >> 8);
			outSpec.fData[i+1] = (UInt8) (nValue & 0x00FF);
			outSpec.fMask |= nMask;
			nMask >>= 2;
			nParts++;
		}

		// Skip delimiter.
		if ( (*inString == _T('.')) || (*inString == _T(':')) )
		{
			*inString++;
		}
		else if ( *inString != 0 )
		{
			return false;
		}
	}

	if( bHasZeroRun && nParts < 8 )
	{
		// Skip past the ':'.
		if( *inString == _T(':') )
		{
			inString++;
		}

		// Save our place.
		LPCTSTR pMarker = inString;

		// Count how many more parts we have.
		int	nPartsAfterZeroRun = 1;
		while( *inString != 0 )
		{
			if( *inString == _T(':') )
			{
				nPartsAfterZeroRun++;
			}
			inString++;
		}

		// Now given the number of parts before and after the zero run,
		// we know how many zero parts we need.
		int nZeroParts = 8 - (nParts + nPartsAfterZeroRun);
		for( int i = 0; i < nZeroParts; i++ )
		{
			// Pack it into fData.
			outSpec.fData[i+(nParts*2)] = (UInt8) 0;
			outSpec.fData[i+(nParts*2)+1] = (UInt8) 0;
			outSpec.fMask |= nMask;
			nMask >>= 2;
			nParts++;
		}


		// Go back to where we were.
		inString = pMarker;

		// Now put in the final parts.
		int nPartsLeft = 8 - nParts;
		for ( int i = 0; i < (nPartsLeft*2); i+=2 )
		{
			// Get a part.
			UInt8	nNibble = 0;
			UInt32	nValue = 0;
			if( *inString == _T('*') )
			{
				// Wildcard character, skip it.
				inString++;
			}
			else if( *inString == _T(':') )
			{
				// Zero run.
				// Invalid; already had a zero run.
				return false;
			}
			else
			{
				while( HexCharToValue( *inString, nNibble ) )
				{
					nValue <<= 4;
					nValue += nNibble;
					inString++;
				}

				// Should fit in 16-bits.
				if ( (nValue & 0xFFFF0000) != 0 )
				{
					return false;
				}

				// Pack it into fData.
				outSpec.fData[nParts*2] = (UInt8) ((nValue & 0xFF00) >> 8);
				outSpec.fData[(nParts*2)+1] = (UInt8) (nValue & 0x00FF);
				outSpec.fMask |= nMask;
				nMask >>= 2;
				nParts++;
			}

			// Skip delimiter.
			if ( (*inString == _T('.')) || (*inString == _T(':')) )
			{
				*inString++;
			}
			else if ( *inString != 0 )
			{
				return false;
			}
		}
	}

	// Make sure there's nothing extra on the end.
	if ( *inString != 0 )
	{
		return false;
	}

	// Setup the media spec class and type.
	outSpec.SetClass( kMediaSpecClass_Address );
	outSpec.SetType( kMediaSpecType_IPv6Addr );

	return true;
}


// -----------------------------------------------------------------------------
//		ParseIPXAddr [static]
// -----------------------------------------------------------------------------
// TODO: UMediaSpecUtils::ParseIPXAddr does not handle wildcards/masks

bool
UMediaSpecUtils::ParseIPXAddr(
	LPCTSTR		inString,
	CMediaSpec&	outSpec )
{
	// Sanity check.
	ASSERT( inString != NULL );
	if ( inString == NULL ) return false;

	// Start with a clear spec.
	outSpec.Clear();

	// Valid formats: (case insensitive)
	// xxxxxxxx.xxxxxxxxxxxx
	// ^       ^^ 
	// |       ||
	// |       |+----------- Host ID:       12 characters long in hexadecimal 
	// |       |                            format. ':', '-', and ' ' 
	// |       |                            characters are accepted but
	// |       |                            ignored.
	// |       |
	// |       +------------ Dot separator: Must be present to separate  
	// |                                    variable length Network ID and 
	// |                                    fixed length Host ID portions.
	// |
	// +--------------------- Network ID:   1 to 8 characters long in 
	//                                      hexadecimal format with no "0x" or 
	//                                      '$' prefix.

	// Parse the Network ID.
	UInt32 nNetworkID;
	UInt8 nNibble;

	// The first character must be a valid hexadecimal character.
	if ( !HexCharToValue( *inString, nNibble ) )
	{
		return false;
	}

	++inString;
	nNetworkID = nNibble;

	// Allow up to eight hexadecimal characters to preceed the dot. This loop 
	// performs 7 iterations because the first character has already been 
	// read/checked.
	for (int i = 0; (i < 7) && (*inString != _T('.')); i++, inString++)
	{
		if ( !HexCharToValue( *inString, nNibble ) )
		{
			return false;
		}

		nNetworkID <<= 4;
		nNetworkID |= nNibble;
	}

	// The dot must be after the first character and before or at the ninth
	// character.
	if ( *inString != _T('.') )
	{
		return false;
	}
	
	// Skip past the dot.
	inString++;

	// Copy the Network ID to the address buffer.
	*(UInt32*) &outSpec.fData[0] = HOSTTONETWORK32( nNetworkID );

	// Parse the Host ID.
	UInt8 nAddrIndex = 4;
	
	// Require 12 hexadecimal characters, accepting but ignoring ':', '-', and ' '.
	while ( (*inString != 0) && (nAddrIndex < 10) )
	{
		if ( (*inString == _T(':')) || 
			 (*inString == _T('-')) ||
			 (*inString == _T(' ')) )
		{
			inString++;
		}
		else if ( HexCharToValue( *inString, nNibble ) )
		{
			UInt8 nValue;

			inString++;
			nValue = nNibble;

			if ( !HexCharToValue( *inString, nNibble ) )
			{
				// Must have two hex characters next to each other 
				// representing a byte.
				return false;
			}

			inString++;
			nValue <<= 4;
			nValue |= nNibble;

			outSpec.fData[nAddrIndex] = nValue;
			nAddrIndex++;
		}
		else
		{
			// Invalid character.
			return false;
		}
	}

	if ( (*inString != 0) || (nAddrIndex != 10) )
	{
		// The Host ID did not have exactly 12 hexadecimal characters.
		return false;
	}

	outSpec.SetClass( kMediaSpecClass_Address );
	outSpec.SetType( kMediaSpecType_IPXAddr );
	outSpec.fMask = outSpec.GetTypeMask();

	return true;
}


// -----------------------------------------------------------------------------
//		ParseIPPort [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::ParseIPPort(
	LPCTSTR		inString,
	CMediaSpec&	outSpec )
{
	// Sanity check.
	ASSERT( inString != NULL );
	if ( inString == NULL ) return false;

	// Start with a clear spec.
	outSpec.Clear();

	// Valid formats: 1021, 0x1234, $6767, no wildcards.

	// Skip leading $ or 0x.
	if ( *inString == _T('$') )
	{
		inString++;
	}
	else if ( (inString[0] == _T('0')) &&
		((inString[1] == _T('x')) || (inString[1] == _T('X'))) )
	{
		inString += 2;
	}

	if ( *inString == 0 )
	{
		// Invalid format: ran out of string.
		return false;
	}

	UInt32	nValue = 0;
	while ( *inString != 0 )
	{
		if ( (*inString >= _T('0')) && (*inString <= _T('9')) )
		{
			nValue = (UInt32)((nValue * 10) + (*inString - _T('0')) );
		}
		else
		{
			// Invalid character.
			return false;
		}

		// Move to the next character.
		inString++;
	}

	if ( (nValue & 0xFFFF0000) != 0 )
	{
		// Number too big.
		return false;
	}

	// Make sure there's nothing extra on the end.
	if ( *inString != 0 )
	{
		return false;
	}

	// Setup the media spec.
	outSpec.SetClass( kMediaSpecClass_Port );
	outSpec.SetType( kMediaSpecType_IPPort );
	outSpec.fMask = 0xC0000000;
	*(UInt16*) outSpec.fData = HOSTTONETWORK16( (UInt16) nValue );

	return true;
}


// -----------------------------------------------------------------------------
//		ParseATPort [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::ParseATPort(
	LPCTSTR		inString,
	CMediaSpec&	outSpec )
{
	// Sanity check.
	ASSERT( inString != NULL );
	if ( inString == NULL ) return false;

	// Start with a clear spec.
	outSpec.Clear();

	// Valid formats: 204, 0x45, $88, no wildcards.

	// Skip leading $ or 0x.
	if ( *inString == _T('$') )
	{
		inString++;
	}
	else if ( (inString[0] == _T('0')) &&
		((inString[1] == _T('x')) || (inString[1] == _T('X'))) )
	{
		inString += 2;
	}

	if ( *inString == 0 )
	{
		// Invalid format: ran out of string.
		return false;
	}

	UInt32	nValue = 0;
	while ( *inString != 0 )
	{
		if ( (*inString >= _T('0')) && (*inString <= _T('9')) )
		{
			nValue = (UInt32)((nValue * 10) + (*inString - _T('0')) );
		}
		else
		{
			// Invalid character.
			return false;
		}

		// Move to the next character.
		inString++;
	}

	if ( (nValue & 0xFFFFFF00) != 0 )
	{
		// Number too big.
		return false;
	}

	// Make sure there's nothing extra on the end.
	if ( *inString != 0 )
	{
		return false;
	}

	// Setup the media spec.
	outSpec.SetClass( kMediaSpecClass_Port );
	outSpec.SetType( kMediaSpecType_ATPort );
	outSpec.fMask = 0x80000000;
	outSpec.fData[0] = (UInt8) nValue;

	return true;
}


// -----------------------------------------------------------------------------
//		ParseNWPort [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::ParseNWPort(
	LPCTSTR		inString,
	CMediaSpec&	outSpec )
{
	// Sanity check.
	ASSERT( inString != NULL );
	if ( inString == NULL ) return false;

	// Start with a clear spec.
	outSpec.Clear();

	// Valid formats: 0xC021, $C021, C021, C0-21, no wildcards.

	// Skip leading $ or 0x.
	if ( *inString == _T('$') )
	{
		inString++;
	}
	else if ( (inString[0] == _T('0')) &&
		((inString[1] == _T('x')) || (inString[1] == _T('X'))) )
	{
		inString += 2;
	}

	if ( *inString == 0 )
	{
		// Invalid format: ran out of string.
		return false;
	}

	for ( int i = 0; i < 2; i++ )
	{
		if ( *inString == 0 )
		{
			// Invalid format: ran out of string.
			return false;
		}

		// Parse the next hex byte.
		if ( !HexStrToByte( inString, outSpec.fData[i] ) )
		{
			// Invalid format.
			return false;
		}

		// Skip two characters.
		inString += 2;

		// Skip delimiters.
		if ( *inString == _T('-') )
		{
			inString++;
		}
	}

	// Make sure there's nothing extra on the end.
	if ( *inString != 0 )
	{
		return false;
	}

	// Setup the media spec.
	outSpec.SetClass( kMediaSpecClass_Port );
	outSpec.SetType( kMediaSpecType_NWPort );
	outSpec.fMask = outSpec.GetTypeMask();

	return true;
}


// -----------------------------------------------------------------------------
//		HexStrToByte [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::HexStrToByte(
	LPCTSTR	inString,
	UInt8&	outByte )
{
	UInt8	nByte = 0;

	for ( int i = 0; i < 2; i++ )
	{
		// Get the next character.
		TCHAR	c = (TCHAR) *inString;
		if ( c == 0 ) return false;

		nByte = (UInt8)(nByte << 4);

		if ( (c >= _T('0')) && (c <= _T('9')) )
		{
			// Number.
			nByte = (UInt8)(nByte + (c - _T('0')));
		}
		else if ( (c >= _T('A')) && (c <= _T('F')) )
		{
			// Uppercase hex.
			nByte = (UInt8)(nByte + (10 + (c - _T('A'))));
		}
		else if ( (c >= _T('a')) && (c <= _T('f')) )
		{
			// Lowercase hex.
			nByte = (UInt8)(nByte + (10 + (c - _T('a'))));
		}
		else
		{
			// Invalid character.
			return false;
		}

		// Move to the next character.
		inString++;
	}

	// Set the return value.
	outByte = nByte;

	return true;
}


// -----------------------------------------------------------------------------
//		HexCharToValue [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::HexCharToValue(
	TCHAR	inChar,
	UInt8&	outValue )
{
	UInt8	nByte = 0;

	if ( (inChar >= _T('0')) && (inChar <= _T('9')) )
	{
		// Number.
		nByte = (UInt8)(inChar - _T('0'));
	}
	else if ( (inChar >= _T('A')) && (inChar <= _T('F')) )
	{
		// Uppercase hex.
		nByte = (UInt8)(10 + (inChar - _T('A')));
	}
	else if ( (inChar >= _T('a')) && (inChar <= _T('f')) )
	{
		// Lowercase hex.
		nByte = (UInt8)(10 + (inChar - _T('a')));
	}
	else
	{
		// Invalid character.
		return false;
	}

	// Set the return value.
	outValue = nByte;

	return true;
}


// -----------------------------------------------------------------------------
//		MaskToBitfield [static]
// -----------------------------------------------------------------------------

bool
UMediaSpecUtils::MaskToBitfield(
	const CMediaSpec&	inMediaSpec,
	UInt8*				outBitField )
{
	switch ( inMediaSpec.fClass )
	{
	case  kMediaSpecClass_Address:
		switch ( inMediaSpec.fType )
		{
			case kMediaSpecType_DECnetAddr:
			{
				UInt16	nMask = (UInt16)(inMediaSpec.fMask >> 16);
				
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
				
				memcpy( outBitField, &nMask, sizeof(nMask) );
			}
			return true;

			case kMediaSpecType_IPAddr:
			{
				// mask is really a bitfield
				// TT8612: IP addr bitmask is wrong
				// the mask in fMask is in reverse, so we need to reverse copy
				const UInt8* pTheMask = (const UInt8*) &inMediaSpec.fMask;
				for ( size_t i = 0; i < sizeof(inMediaSpec.fMask); i++ )
				{
#if TARGET_OS_WIN32 || _LINUX	// TODO: probably not correct for linux
					outBitField[i] = pTheMask[ sizeof(inMediaSpec.fMask) - 1 - i ];
#elif TARGET_OS_MAC
					outBitField[i] = pTheMask[ i ];
#else
#pragma error("TARGET unknown")
#endif
				}
			}
			return true;

			default:
			{
				// mask is a byte mask, each bit indicating which bytes should be included/excluded
				UInt32	nMask = 0x80000000;
				UInt16	nDataLen = sizeof(inMediaSpec.fData);

				for ( UInt16 i = 0; i < nDataLen; i++ )
				{
					*outBitField = (nMask & inMediaSpec.fMask) ? 
										0xff :	// the bit is set
										0x00;	// the bit is not set
					outBitField++;	// next byte please
					nMask >>= 1;
				}
			}		
			return true;
		}
		break;

	default:
		{	// mask is a byte mask, each bit indicating which bytes should be included/excluded
			UInt32	nMask = 0x80000000;
			UInt16	nDataLen = sizeof(inMediaSpec.fData);

			for ( UInt16 i = 0; i < nDataLen; i++ )
			{
				*outBitField = (nMask & inMediaSpec.fMask) ? 
									0xff :	// the bit is set
									0x00;	// the bit is not set
				outBitField++;	// next byte please
				nMask >>= 1;
			}
		}		
		return true;
	}
}


// -----------------------------------------------------------------------------
//		GetSubTypeFromProtocol [static]
// -----------------------------------------------------------------------------
// In the WAN product, the Sub media is dependent is f(x) of the protocol used.

DWORD
UMediaSpecUtils::GetSubTypeFromProtocol(
	TMediaType		inMediaType, 
	DWORD			inWanProtocol,
	TMediaSubType*	outMediaSubType )
{
#ifndef _NPROTOSPECS
	switch (inMediaType)
	{
	case kMediaType_Wan:
	case kMediaType_CoWan:
	{		
		switch (inWanProtocol)	// extra info in the WAN header
		{
		case PEEK_WAN_PROT_PPP:				// PPP, cHDLC
			*outMediaSubType  = kMediaSubType_wan_ppp;
			return ERROR_SUCCESS;

		case PEEK_WAN_PROT_FRLY:			// Frame Relay
			*outMediaSubType  = kMediaSubType_wan_frameRelay;
			return ERROR_SUCCESS;

		case PEEK_WAN_PROT_X25:				// X.25
			*outMediaSubType  = kMediaSubType_wan_x25;
			return ERROR_SUCCESS;

		case PEEK_WAN_PROT_X25E:			// X.25 mod 128
			*outMediaSubType  = kMediaSubType_wan_x25e;
			return ERROR_SUCCESS;

		case PEEK_WAN_PROT_IPARS:			// IPARS
			*outMediaSubType  = kMediaSubType_wan_ipars;
			return ERROR_SUCCESS;

		case PEEK_WAN_PROT_U200:			// U200
			*outMediaSubType  = kMediaSubType_wan_u200;
			return ERROR_SUCCESS;

		case PEEK_WAN_PROT_Q931:			// Q.931
			*outMediaSubType  = kMediaSubType_wan_Q931;
			return ERROR_SUCCESS;

		case PEEK_WAN_PROT_SNA:				// SNA (Not yet supported)
		case PEEK_WAN_PROT_SS7:				// SS7 (Not yet supported)
		case PEEK_WAN_PROT_INVALID:			// This one will never be supported (duh!)
		case PEEK_WAN_PROT_0800:			// Old one used bu Logix Hack.
		default:
			ASSERT(0);
			*outMediaSubType = kMediaSubType_Max;
			return ERROR_NOT_SUPPORTED;
		}
	}
	
	default:
		//nothing to do;
		return ERROR_SUCCESS;
	}
#endif
	return ERROR_SUCCESS;
}
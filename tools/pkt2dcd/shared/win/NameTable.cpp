// =============================================================================
//	NameTable.cpp
// =============================================================================
//	Copyright (c) 1998-2008 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "NameTable.h"
#include "ByteStream.h"
#include "Color.h"
#include "FileUtil.h"
#include "MediaSpecUtil.h"
#include "MemUtil.h"
//#include "NameTableFile.h"
//#include "NameTableParser.h"
#include "OutputFileStream.h"
//#include "PerfUtil.h"
#include "Protospecs.h"
#include "Resource.h"
//#include "ToolBarImages.h"
#include <string.h>
#include <stdio.h>

#if defined(OPT_PROFILE_TIMINGS)
#include "PerfCollector.h"
#endif

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

// =============================================================================
//	CNameSubTable
// =============================================================================

// =============================================================================
//	CNameTable
// =============================================================================

// -----------------------------------------------------------------------------
//		operator=(const CNameTableOptions&)
// -----------------------------------------------------------------------------

CNameTable&
CNameTable::operator=(
	const CNameTableOptions&	rhs )
{
	CNameTableOptions::operator =( rhs );

	return *this;
}


// -----------------------------------------------------------------------------
//		GetNameEntryTypeString [static]
// -----------------------------------------------------------------------------

bool
CNameTable::GetNameEntryTypeString(
	const UInt8	nNameEntryType,
	CString&	strNameEntryType )
{
	int i = 0;
	printf("%d", i );
#ifdef TODO
	static const UInt8	kNameEntryType[] =
	{
		kNameEntryType_Unknown,
		kNameEntryType_Workstation,
		kNameEntryType_Server,
		kNameEntryType_Router,
		kNameEntryType_Switch,
		kNameEntryType_Repeater,
		kNameEntryType_Printer,
		kNameEntryType_AccessPoint
	};

	static const UINT	kNameEntryTypeStringID[] =
	{
		IDS_NODE_TYPE_UNKNOWN,
		IDS_NODE_TYPE_WORKSTATION,
		IDS_NODE_TYPE_SERVER,
		IDS_NODE_TYPE_ROUTER,
		IDS_NODE_TYPE_SWITCH,
		IDS_NODE_TYPE_REPEATER,
		IDS_NODE_TYPE_PRINTER,
		IDS_NODE_TYPE_ACCESS_POINT
	};

	for ( size_t i = 0; i < COUNTOF(kNameEntryType); ++i )
	{
		if ( nNameEntryType == kNameEntryType[i] )
		{
			return LoadResourceString( strNameEntryType, kNameEntryTypeStringID[i] );
		}
	}

	strNameEntryType.Empty();
#endif
	return false;
}


// -----------------------------------------------------------------------------
//		GetTrustString [static]
// -----------------------------------------------------------------------------

bool
CNameTable::GetTrustString(
	const UInt8	nTrust,
	CString&	strTrust )
{
#ifdef TODO
	static const UInt8	kTrustType[] =
	{
		kNameTrust_Unknown,
		kNameTrust_Known,
		kNameTrust_Trusted
	};

	static const UINT	kTrustStringID[] =
	{
		IDS_TRUST_UNKNOWN,
		IDS_TRUST_KNOWN,
		IDS_TRUST_TRUSTED
	};

	for ( size_t i = 0; i < COUNTOF(kTrustType); ++i )
	{
		if ( nTrust == kTrustType[i] )
		{
			return LoadResourceString( strTrust, kTrustStringID[i] );
		}
	}

	strTrust.Empty();
#endif
	return false;
}


// -----------------------------------------------------------------------------
//		GenerateAutoColor [static]
// -----------------------------------------------------------------------------

COLORREF
CNameTable::GenerateAutoColor(
	const CMediaSpec&	inSpec )
{
	// Sanity check.
	ASSERT( inSpec.IsValid() );
	if ( !inSpec.IsValid() ) return RGB(0,0,0);

#if 1
	const size_t	cb = static_cast<size_t>(inSpec.GetDataLength());
	size_t			h  = 2166136261;
	for ( size_t i = 0; i < cb; ++i )
	{
		h += (h << 1) + (h << 4) + (h << 7) + (h << 8) + (h << 24);
		h ^= inSpec.fData[i];
	}
	return WPColor::Chart( static_cast<int>(h) );
#else
	static const COLORREF
	kColorTable[] =
	{
		RGB(194,76,72),
		RGB(85,0,0),
		RGB(85,63,0),
		RGB(42,95,0),
		RGB(0,127,0),
		RGB(0,127,85),
		RGB(42,31,85),
		RGB(42,0,85),
		RGB(85,0,85),
		RGB(127,63,0),
		RGB(127,159,0),
		RGB(0,159,0),
		RGB(0,159,85),
		RGB(0,128,128),
		RGB(0,0,128),
		RGB(128,0,128),
		RGB(127,0,85),
		RGB(212,127,0),
		RGB(0,191,0),
		RGB(0,191,170),
		RGB(0,127,170),
		RGB(85,0,170),
		RGB(212,0,170),
		RGB(212,0,0)
	};

	UInt8			nIndex = 0;
	UInt16			nDataLen = inSpec.GetDataLength();
	const UInt8*	pData = inSpec.fData;
	for ( int i = 0; i < nDataLen; i++ )
	{
		nIndex = (UInt8)(nIndex + *pData);
		pData++;
	}

	nIndex %= 24;

	return kColorTable[nIndex];
#endif
}


// -----------------------------------------------------------------------------
//		SpecToColor [static]
// -----------------------------------------------------------------------------

COLORREF
CNameTable::SpecToColor(
	const CMediaSpec&	inSpec )
{
	ASSERT( inSpec.IsValid() );

	COLORREF	theColor;
	if ( inSpec.GetType() == kMediaSpecType_ProtoSpec )
	{
		//TODO 
		// Use the protospec color.
		//ProtoSpecs::GetColor( *(PROTOSPEC_INSTID*)inSpec.fData, theColor );
		theColor = WPColor::BLACK;
	}
	else if ( inSpec.GetType() == kMediaSpecType_ApplicationID )
	{
		theColor = GenerateAutoColor( inSpec );
	}
	else
	{
		// Default to the window text color.
		theColor = ::GetSysColor( COLOR_WINDOWTEXT );
	}

	return theColor;
}


// -----------------------------------------------------------------------------
//		IconFromType
// -----------------------------------------------------------------------------

int
CNameTable::IconFromType(
	TMediaSpecType	inSpecType,
	UInt8			inNameEntryType ) const
{
#ifdef TODO
	int		nIcon = -1;

	if ( inNameEntryType != kNameEntryType_Unknown )
	{
		// Use the name entry type for the icon.
		switch ( inNameEntryType )
		{
			case kNameEntryType_Workstation:
				nIcon = tbWorkstation;
				break;
			case kNameEntryType_Server:
				nIcon = tbServer;
				break;
			case kNameEntryType_Router:
				nIcon = tbRouter;
				break;
			case kNameEntryType_Switch:
				nIcon = tbSwitch;
				break;
			case kNameEntryType_Repeater:
				nIcon = tbRepeater;
				break;
			case kNameEntryType_Printer:
				nIcon = tbPrint;
				break;
			case kNameEntryType_AccessPoint:
				nIcon = tbAccessPoint;
				break;
		}
	}
	
	if ( nIcon == -1 )
	{
		// Use the spec type for the icon.
		switch ( inSpecType )
		{
			case kMediaSpecType_EthernetAddr:
			case kMediaSpecType_WirelessAddr:
			case kMediaSpecType_WAN_DLCIAddr:
			case kMediaSpecType_TokenRingAddr:
				nIcon = tbAdapter;
				break;

			case kMediaSpecType_IPAddr:
			case kMediaSpecType_IPv6Addr:
			case kMediaSpecType_IPXAddr:
			case kMediaSpecType_AppleTalkAddr:
			case kMediaSpecType_DECnetAddr:
			case kMediaSpecType_LAPAddr:
				nIcon = tbComputer;
				break;

			case kMediaSpecType_ProtoSpec:
			case kMediaSpecType_EthernetProto:
			case kMediaSpecType_LSAP:
			case kMediaSpecType_SNAP:
			case kMediaSpecType_LAP:
			case kMediaSpecType_DDP:
			case kMediaSpecType_MACCtl:
			case kMediaSpecType_WAN_PPP_Proto:
			case kMediaSpecType_WAN_FrameRelay_Proto:
			case kMediaSpecType_WAN_X25_Proto:
			case kMediaSpecType_WAN_X25e_Proto:
			case kMediaSpecType_WAN_Ipars_Proto:
			case kMediaSpecType_WAN_U200_Proto:
			case kMediaSpecType_WAN_Q931_Proto:
				nIcon = tbProtocol;
				break;

			case kMediaSpecType_IPPort:
			case kMediaSpecType_NWPort:
			case kMediaSpecType_ATPort:
			case kMediaSpecType_TCPPortPair:
				nIcon = tbPort;
				break;
		}
	}

	return nIcon;
#else
	return 0;
#endif
}


// -----------------------------------------------------------------------------
//		ReceiveResult
// -----------------------------------------------------------------------------

void
CNameTable::ReceiveResult(
	const CMediaSpec&	inLogical,
	const CMediaSpec&	inPhysical,
	LPCTSTR				inName )
{
	SNameTableEntry	theEntry;
	::memset( &theEntry, 0, sizeof( theEntry ) );
	
	// Copy the spec.
	theEntry.fSpec = inLogical;

	// Copy the name.
	_tcsncpy( theEntry.fName, inName, kMaxNameLength );
	theEntry.fName[kMaxNameLength] = 0;

	theEntry.fColor = GenerateAutoColor( theEntry.fSpec );
	theEntry.fDateModified = time( NULL );
	theEntry.fDateLastUsed = theEntry.fDateModified;
	theEntry.fType = kNameEntryType_Unknown;
	theEntry.fSource = kNameEntrySource_ResolveActive;
	if ( inLogical.GetType() == kMediaSpecType_IPAddr )
	{
		theEntry.fSource |= kNameEntrySource_TypeIP;
	}
	else if ( inLogical.GetType() == kMediaSpecType_AppleTalkAddr )
	{
		theEntry.fSource |= kNameEntrySource_TypeAT;
	}
	theEntry.fGroup = kGroupID_None;
	theEntry.fFlags = 0;

	AddEntryWithCurrentOptions( theEntry );

	if ( ( inPhysical.IsValid() ) && 
		 (!inPhysical.IsMulticast() ) && // Don't assign names to multicast addresses!
		 ( m_bAssignPhysicalNames ) )
	{
		theEntry.fSpec = inPhysical;
		theEntry.fColor = GenerateAutoColor( theEntry.fSpec );
		theEntry.fType = kNameEntryType_Unknown;
		
		if ( m_bAppendPhysicalName )
		{
			// TBD: adding the Name Suffix could case fName to be larger than kMaxNameLength
			_tcscat( theEntry.fName, m_strNameSuffix );
		}

		// Assigning names to HW addresses doesn't honor resolver options.
		AddEntry( theEntry, 
			kResolveOption_NoMatchAdd |
			kResolveOption_NameMatchReplace |
			kResolveOption_AddrMatchSkip );
	}
}

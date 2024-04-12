// =============================================================================
//	Options.cpp
// =============================================================================
//	Copyright (c) 2011-2017 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "Options.h"
#include "PeekDataModeler.h"


// =============================================================================
//		OptionsTags
// =============================================================================

namespace OptionsTags
{
	const CPeekString	kRootName( L"CMIFilters" );

	const CPeekString	kDisabled( L"Disabled" );
	const CPeekString	kEnabled( L"Enabled" );
	const CPeekString	kFalse( L"0" );
	const CPeekString	kTrue( L"1" );
	const CPeekString	kCount( L"Count" );
	const CPeekString	kUnits( L"Units" );
	const CPeekString	kSeconds( L"Seconds" );

	const CPeekString	kProtocolType( L"ProtocolType" );
	const CPeekString	kInterceptId( L"InteceptId" );
	const CPeekString	kFilterList( L"FilterList" );
	const CPeekString	kFilter( L"Filter" );
	const CPeekString	kAddress1( L"Address1" );
	const CPeekString	kAddress2( L"Address2" );
	const CPeekString	kAnyAddressEnabled( L"AnyAddressEnabled" );
	const CPeekString	kDirection( L"Direction" );
	const CPeekString	kPort( L"Port" );
	const CPeekString	kPortEnabled( L"PortEnabled" );
	const CPeekString	kMACHeader( L"MACHeader" );
	const CPeekString	kSrcMAC( L"SrcMAC" );
	const CPeekString	kDstMAC( L"DstMAC" );
	const CPeekString	kMACProtocolType( L"ProtoType" );
	const CPeekString	kMACProtocolIP( L"ProtoIP" );
	const CPeekString	kVLANID( L"VLANID" );
	const CPeekString	kVLANTag( L"VLANTag" );
	const CPeekString	kFragmentAge( L"FragmentAge" );
	const CPeekString	kSaveOrphans( L"SaveOrphans" );
	const CPeekString	kFilename( L"Filename" );
	const CPeekString	kInterval( L"Interval" );
	const CPeekString	kType( L"Type" );
}

const CPeekString	g_strComma = L",";
const CPeekString	g_strSpace = L" ";


// ============================================================================
//		CInterceptId
// ============================================================================

CInterceptId::CInterceptId(
	const CPeekString&	inValue )
{
	Parse( kIdType_Ericsson, inValue );
}


// ----------------------------------------------------------------------------
//		Format
// ----------------------------------------------------------------------------

CPeekString
CInterceptId::Format(
	bool	inPad /*= true*/) const
{
	CPeekString	strValue;
	if ( !IsNull() ) {
		switch ( m_nType ) {
		case kIdType_PCLI:
			break;

		case kIdType_SecurityId:
			{
				CPeekOutString	str;
				CPeekString		strPad;
				str << std::hex << std::uppercase;
				if ( inPad ) {
					str << std::setfill( L'0' );
					strPad = L" ";
				}

				UInt32	nMask = 0x0F000000;
				UInt32	nShift = 24;
				
				str << std::setw( 1 ) << ((m_Id & nMask) >> nShift) << strPad;
				nMask = 0x00FF0000;
				nShift -= 8;

				str << std::setw( 2 ) << ((m_Id & nMask) >> nShift) << strPad;
				nMask >>= 8;
				nShift -= 8;

				str << std::setw( 2 ) << ((m_Id & nMask) >> nShift) << strPad;
				nMask >>= 8;
				nShift -= 8;

				str << std::setw( 2 ) << (m_Id & nMask);
				strValue = str;
			}
			break;

		case kIdType_Ericsson:
			{
				CPeekOutString	str;
				str << m_Id << g_strComma << m_strSite.c_str();
				strValue = str;
			}
			break;

		case kIdType_Arris:
		case kIdType_Nokia:
			{
				CPeekOutString	str;
				str << m_Id;
				strValue = str;
			}
			break;
		}
	}

	return strValue;
}


// ----------------------------------------------------------------------------
//		Parse
// ----------------------------------------------------------------------------

void
CInterceptId::Parse(
	int					inType,
	const CPeekString&	inValue )
{
	Reset();
	if ( inValue.IsEmpty() ) return;

	switch ( inType ) {
	case kIdType_PCLI:
		break;

	case kIdType_SecurityId:
		{
			CPeekString	strId( inValue );
			strId.Remove( L' ' );
			strId.Remove( L'-' );

			UInt32	nId( 0 );
			size_t	nCount = strId.GetLength();
			for ( size_t i = 0; i < nCount; i++ ) {
				UInt16	nDigit( strId[i] );
				UInt8	nValue( 0 );
				if ( (nDigit >= L'0') && (nDigit <= L'9') ) {
					nValue = static_cast<UInt8>( nDigit - L'0' );
				}
				else if ( (nDigit >= L'a') && (nDigit <= L'f') ) {
					nValue = static_cast<UInt8>( 10 + (nDigit - L'a') );
				}
				else if ( (nDigit >= L'A') && (nDigit <= L'F') ) {
					nValue =  static_cast<UInt8>( 10 + (nDigit - L'A') );
				}

				nId = (nId << 4) | (nValue & 0x0F);
			}
			Set( kIdType_SecurityId, nId );
		}
		break;

	case kIdType_Ericsson:
		{
			size_t		nIndex( 0 );
			CPeekString	strValue = inValue;
			CPeekString strId = strValue.Tokenize( L",", nIndex );
			UInt32	nId( wcstol( strId, nullptr, 10 ) );
			CPeekString strSite = strValue.Tokenize( L",", nIndex );
			Set( nId, strSite );
		}
		break;

	case kIdType_Arris:
		{
			UInt32	nId( wcstol( inValue, nullptr, 10 ) );
			Set( kIdType_Arris, nId );
		}
		break;

	case kIdType_Nokia:
		{
			UInt32	nId( wcstol( inValue, nullptr, 10 ) );
			Set( kIdType_Nokia, nId );
		}
		break;
	}
}


// ============================================================================
//		CSecurityId
// ============================================================================

CSecurityId::CSecurityId(
	const CPeekString&	inValue )
{
	Reset();

	CPeekString	strValue( inValue );
	strValue.Remove( L' ' );
	strValue.Remove( L'-' );

	if ( inValue.IsEmpty() ) return;

	if ( strValue.GetLength() % 2 ) {
		strValue.Insert( 0, L"0" );
	}

	for ( int i = (strValue.GetLength32() - (s_nSize * 2)); i < s_nSize; i++ ) {
		wchar_t	sz[4] = { strValue[i*2], strValue[i*2+1], 0, 0 };
		fId.fByte[i] = static_cast<UInt8>( wcstoul( sz, nullptr, s_nSize ) );
	}

	SetInterceptId();
}


// ----------------------------------------------------------------------------
//		Format
// ----------------------------------------------------------------------------

CPeekString
CSecurityId::Format() const
{
	CPeekString	strValue;
	if ( !IsNull() ) {
		CPeekOutString	str;
		str << std::hex << std::uppercase;
		for ( int i = 0; i < 16; i++ ) {
			if ( ((i + 1) % 4) == 0 ) {
				str << L" ";
			}
			str << std::setw( 2 ) << fId.fByte[i];
		}
		strValue = str;
	}

	return strValue;
}


// ============================================================================
//		CEricsson
// ============================================================================

// ----------------------------------------------------------------------------
//		Parse
// ----------------------------------------------------------------------------

bool
CEricsson::Parse(
	size_t			inLength,
	const UInt8*	inData )
{
	const UInt8*	pNext = inData;
	const UInt8*	pEnd = inData + inLength;
	bool			bDone = false;
	int				nId = 0;
	std::string		strSite;
	while ( (pNext < pEnd) && !bDone ) {
		UInt8	nType = *pNext++;
		UInt8	nLength = *pNext++;
		if ( nType == 0x02 ) {
			nId = MemUtil::Swap32( *reinterpret_cast<const UInt32*>( pNext ) );
		}
		else if ( nType == 0x14 ) {
			strSite.assign( pNext, (pNext + nLength) );
		}
		else if ( nType == 0x00 ) {
			bDone = true;
		}
		pNext += nLength;
	}

	m_InterceptId.Set( nId, strSite );
	m_nLength = static_cast<UInt16>( pNext - inData );

	return true;
}


// ============================================================================
//		CArris
// ============================================================================

// ----------------------------------------------------------------------------
//		Parse
// ----------------------------------------------------------------------------

bool
CArris::Parse(
	size_t			/*inLength*/,
	const UInt8*	inData )
{
	UInt32	nId = MemUtil::Swap32( *((const UInt32*)inData) );
	m_InterceptId.Set( CInterceptId::kIdType_Arris, nId );

	return true;
}


// ============================================================================
//		CNokia
// ============================================================================

// ----------------------------------------------------------------------------
//		Parse
// ----------------------------------------------------------------------------

bool
CNokia::Parse(
	size_t			/*inLength*/,
	const UInt8*	inData )
{
	// 30-bit Intercept Id.
	UInt32	nId = MemUtil::Swap32( *((const UInt32*)inData) ) & 0x3FFFFFFF;
	m_InterceptId.Set( CInterceptId::kIdType_Nokia, nId );

	return true;
}


// ============================================================================
//		CIpFilter
// ============================================================================

// ----------------------------------------------------------------------------
//		Format
// ----------------------------------------------------------------------------

CPeekString
CIpFilter::Format() const
{
	CPeekOutString	str;
	str << m_Address1.Format() << g_strSpace << FormatDirection() << g_strSpace << m_Address2.Format();

// 	str.Format( L"%s %s %s" ), m_Address1.Format(), FormatDirection(), m_Address2.Format() );
// 	if ( IsPort() ) {
// 		str.AppendFormat( L" : %s" ), m_Port.Format() );
// 	}

	return str;
}


// ----------------------------------------------------------------------------
//		IsMatch
// ----------------------------------------------------------------------------

bool
CIpFilter::IsMatch(
	const CIpAddressPair&	inOther,
	bool					inIsUdp,
	const CPort&			inPort ) const
{
	bool			bAccept = false;
	CIpAddressPair	iap( GetAddress1(), GetAddress2() );

	switch ( GetDirection() ) {
		case CIpFilter::kDirection_Sent:			// Address 1 (Source) to 2 (Destination)
			bAccept = (IsAnyOtherAddress())
				? (inOther.GetSource() == iap.GetSource())
				: (inOther == iap);				// exact match
			break;

		case CIpFilter::kDirection_Received:		// Address 2 (Destination) to 1 (Source)
			bAccept = (IsAnyOtherAddress())
				? (inOther.GetDestination() == iap.GetSource())
				: inOther.IsInverse( iap );		// exact opposite
			break;

		case CIpFilter::kDirection_Either:
		default:
			bAccept = (IsAnyOtherAddress())
				? (inOther.GetSource() == iap.GetSource()) || (inOther.GetDestination() == iap.GetSource())
				: (inOther == iap) || inOther.IsInverse( iap );	// exact or exact opposite
			break;
	}
	if ( !bAccept ) return false;

	// If not UDP, then there is no port to check.
	if ( inIsUdp ) {
		// If a Port was configured, check the Port.
		if ( IsPort() ) {
			// Test Port
			if ( inPort != GetPort() ) return false;
		}
	}

	return true;
}


// -----------------------------------------------------------------------------
//		Model
// -----------------------------------------------------------------------------

bool
CIpFilter::Model(
	CPeekDataElement& ioElement )
{
	ioElement.Attribute( OptionsTags::kAddress1, m_Address1 );
	ioElement.Attribute( (const wchar_t*)OptionsTags::kDirection, m_Direction );
	ioElement.Attribute( OptionsTags::kAddress2, m_Address2 );
	ioElement.Attribute( OptionsTags::kAnyAddressEnabled, m_bAnyOtherAddress );
	ioElement.Attribute( OptionsTags::kPortEnabled, m_bPort );
	ioElement.Attribute( OptionsTags::kPort, m_Port );

	return true;
}


// ============================================================================
//		CIpFilterList
// ============================================================================

// ----------------------------------------------------------------------------
//		Compare
// ----------------------------------------------------------------------------

bool
CIpFilterList::Compare(
	const CIpFilterList& inOther ) const
{
	std::vector<CIpFilter>::const_iterator	itr;
	for ( itr = inOther.begin(); itr != inOther.end(); ++itr ) {
		if ( Find( *itr ) ) return true;
	}

	return true;
}


// ----------------------------------------------------------------------------
//		Find
// ----------------------------------------------------------------------------

bool
CIpFilterList::Find(
	const CIpFilter& inIpFilter ) const
{
	std::vector<CIpFilter>::const_iterator	itr;
	for ( itr = begin(); itr != end(); ++itr ) {
		if ( *itr == inIpFilter ) return true;
	}

	return false;
}


// ----------------------------------------------------------------------------
//		Format
// ----------------------------------------------------------------------------

CPeekString
CIpFilterList::Format() const
{
	CPeekString	strResult;
	std::vector<CIpFilter>::const_iterator	itr;
	for ( itr = begin(); itr != end(); ++itr ) {
		CPeekString	str = itr->Format();
		strResult.Append( str );
		strResult.Append( L"\n" );
	}
	strResult.TrimRight( L"\n" );

	return strResult;
}


// ----------------------------------------------------------------------------
//		IsMatch
// ----------------------------------------------------------------------------

bool
CIpFilterList::IsMatch(
	const CIpAddressPair&	inOther,
	bool					inIsUdp,
	const CPort&			inPort ) const
{
	std::vector<CIpFilter>::const_iterator	itr;
	for ( itr = begin(); itr != end(); ++itr ) {
		if ( itr->IsMatch( inOther, inIsUdp, inPort ) ) return true;
	}

	return false;
}


// =============================================================================
//		COptions
// =============================================================================

const CPeekString	COptions::s_ayProtocolNames[COptions::kType_Max] = { 
	L"CMI", 
	L"PCLI", 
	L"SecurityId", 
	L"Ericsson", 
	L"Arris",
	L"Juniper/Nokia"
};


// -----------------------------------------------------------------------------
//		Copy
// -----------------------------------------------------------------------------

void
COptions::Copy(
	const COptions& inOther )
{
	SetProtocolType( inOther.GetProtocolType() );
	SetInterceptId( inOther.GetInterceptId() );
	SetIpFilterList( inOther.GetIpFilterList() );
	SetInsertMAC( inOther.IsInsertMAC() );
	SetSrcEthernet( inOther.GetSrcEthernet() );
	SetDstEthernet( inOther.GetDstEthernet() );
	SetMACProtocolType( inOther.IsMACProtocolIp(), inOther.GetMACProtocolType() );
	SetInsertVlan( inOther.IsInsertVlan() );
	SetVlanId( inOther.GetVlanId() );
	SetFragmentAge( inOther.GetFragmentAge() );
	SetSaveOrphans( inOther.IsSaveOrphans() );
	SetSaveInterval( inOther.GetSaveInterval() );
	SetFileName( inOther.GetFileName() );
}


// -----------------------------------------------------------------------------
//		IsEqual
// -----------------------------------------------------------------------------

bool
COptions::IsEqual(
	const COptions& inOther )
{
	if (	(GetProtocolType() != inOther.GetProtocolType())		||
			(GetInterceptId() != inOther.GetInterceptId())			||
			(GetIpFilterList() != inOther.GetIpFilterList())		||
			(IsInsertMAC() != inOther.IsInsertMAC())				||
			(GetSrcEthernet() != inOther.GetSrcEthernet())			||
			(GetDstEthernet() != inOther.GetDstEthernet())			||
			(GetMACProtocolType() != inOther.GetMACProtocolType())	||
			(IsInsertVlan() != inOther.IsInsertVlan())				||
			(GetVlanId() != inOther.GetVlanId())					||
			(GetFragmentAge() != inOther.GetFragmentAge())			||
			(IsSaveOrphans() != inOther.IsSaveOrphans())			||
			(GetSaveInterval() != inOther.GetSaveInterval())		||
			(GetFileName() != inOther.GetFileName())				) {
		return false;
	}

	return true;
}


// -----------------------------------------------------------------------------
//		Model
// -----------------------------------------------------------------------------
  #ifdef IMPLEMENT_NOTIFICATIONS
	#define kOptionsReceiveNotifies	L"ReceiveNotifies"
  #endif // IMPLEMENT_NOTIFICATIONS

bool
COptions::Model(
	CPeekDataModeler& ioPrefs )
{
	CPeekDataElement	elemOptions( L"Options", ioPrefs );
	if ( elemOptions ) {
		return Model( elemOptions );
	}
	return false;
}

bool
COptions::Model(
	CPeekDataElement& ioElement )
{
	UInt32		nSaveCount( 0 );
	int			nSaveUnits( 0 );

	// Set up interim variables if we are sending (storing) data
	if ( ioElement.IsStoring() ) {
		GetSaveInterval( nSaveCount, nSaveUnits );
	}

	bool	bLoading = ioElement.IsLoading();

	// Perform the data modeling
	CPeekDataElement	elemRoot( OptionsTags::kRootName, ioElement );

	// Protocol Options, includes Secure Id options.
	CPeekDataElement	elemProtocolType( OptionsTags::kProtocolType, elemRoot );
	elemProtocolType.Attribute( OptionsTags::kType, m_ProtocolType );
	if ( ioElement.IsStoring() ) {
		CPeekString	strId = m_InterceptId.Format( false );
		elemProtocolType.Attribute( OptionsTags::kInterceptId, strId );
	}
	else {
		CPeekString	strId;
		elemProtocolType.Attribute( OptionsTags::kInterceptId, strId );
		m_InterceptId.Parse( m_ProtocolType, strId );
	}
	elemProtocolType.End();

	// IP Filter List Options
	CPeekDataElement	elemFilterList( OptionsTags::kFilterList, elemRoot );
	{
		UInt32				nCount = 0;
		if ( bLoading ) {
			m_ayIpFilters.clear();
			nCount = elemFilterList.GetChildCount( OptionsTags::kFilter );
		}
		else {
			nCount = static_cast<UInt32>( m_ayIpFilters.size() );
		}
		for ( UInt32 i = 0; i < nCount; i++ ) {
			CPeekDataElement	elemFilter( OptionsTags::kFilter, elemFilterList, i );
			if ( elemFilter.IsValid() ) {
				if ( bLoading ) {
					CIpFilter	ipFilter;
					m_ayIpFilters.push_back( ipFilter );
				}
				CIpFilter&	filter = m_ayIpFilters[i];
				filter.Model( elemFilter );
			}
		}
	}
	elemFilterList.End();

	// MACHeader Options
	CPeekDataElement	elemMACHeader( OptionsTags::kMACHeader, elemRoot );
	elemMACHeader.AttributeBOOL( OptionsTags::kEnabled, m_bInsertMAC );
	elemMACHeader.Attribute( OptionsTags::kSrcMAC, m_SrcEthernet );
	elemMACHeader.Attribute( OptionsTags::kDstMAC, m_DstEthernet );
	if ( bLoading ) {
		UInt16 nProtocolType = 0;
		elemMACHeader.Attribute( OptionsTags::kProtocolType, nProtocolType );
		if ( nProtocolType != 0 ) {
			m_bIP = FALSE;
			m_nMACProtocolType = nProtocolType;
		}
		else {
			elemMACHeader.Attribute( OptionsTags::kMACProtocolType, m_nMACProtocolType );
			elemMACHeader.Attribute( OptionsTags::kMACProtocolIP, m_bIP );
		}
	}
	else {
		// The DeleteAttribute call doesn't work due to a bug in xmldom.cpp.
		//   DeleteAttribute -> removeAttribute -> removeNamedItem(name, NULL)
		//   removeNamedItem checks the second argument for NULL and fails.
		//elemMACHeader.DeleteAttribute( OptionsTags::kProtocolType );
		elemMACHeader.Attribute( OptionsTags::kMACProtocolType, m_nMACProtocolType );
		elemMACHeader.Attribute( OptionsTags::kMACProtocolIP, m_bIP );
	}
	elemMACHeader.End();

	// VLAN Options
	CPeekDataElement	elemVLAN( OptionsTags::kVLANID, elemRoot );
	elemVLAN.AttributeBOOL( OptionsTags::kEnabled, m_bInsertVLAN );
	elemVLAN.Attribute( OptionsTags::kVLANID, m_nVlanId );
	elemVLAN.End();

	// Fragment Age Options
	CPeekDataElement	elemFragmentAge( OptionsTags::kFragmentAge, elemRoot );
	elemFragmentAge.Attribute( OptionsTags::kFragmentAge, m_nFragmentAge );
	elemFragmentAge.End();

	// Save Orphans Options
	CPeekDataElement	elemSaveOrphans( OptionsTags::kSaveOrphans, elemRoot );
	elemSaveOrphans.AttributeBOOL( OptionsTags::kEnabled, m_bSaveOrphans );
	elemSaveOrphans.Attribute( OptionsTags::kFilename, m_strFileName );
	elemSaveOrphans.Attribute( OptionsTags::kCount, nSaveCount );
	elemSaveOrphans.Attribute( OptionsTags::kUnits, nSaveUnits );
	elemSaveOrphans.End();

	elemRoot.End();

	// Read interim variables if we are receiving data
	if ( ioElement.IsLoading() ) {
		SetSaveInterval( nSaveCount, nSaveUnits );
	}

	return true;
}


// -----------------------------------------------------------------------------
//		Reset
// -----------------------------------------------------------------------------

void
COptions::Reset()
{
	m_ProtocolType = kType_CMI;
	m_InterceptId.Reset();
	m_ayIpFilters.clear();
	m_bInsertMAC = FALSE;
	m_SrcEthernet.Reset();
	m_DstEthernet.Reset();
	m_bIP = FALSE;
	m_nMACProtocolType = 0;
	m_bInsertVLAN = FALSE;
	m_nVlanId = 0;
	m_nFragmentAge = 5;
	m_bSaveOrphans = FALSE;
	m_nSaveInterval = kSecondsInADay;
	m_strFileName.Empty();
}

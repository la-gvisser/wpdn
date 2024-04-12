// ============================================================================
// Options.cpp:
//      implementation of the COptions class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "stdafx.h"
#include "CMIFilter.h"
#include "Options.h"
#include "MediaSpec.h"
#include "MediaSpecUtil.h"
#include "XMLDoc.h"
#include <vector>

using std::vector;


// =============================================================================
//		ConfigTags
// =============================================================================

namespace ConfigTags
{
	const XString	kRootName( _T( "CMIFilter" ) );

	const XString	kDisabled( _T( "Disabled" ) );
	const XString	kEnabled( _T( "Enabled" ) );
	const XString	kFalse( _T( "0" ) );
	const XString	kTrue( _T( "1" ) );
	const XString	kCount( _T( "Count" ) );
	const XString	kUnits( _T( "Units" ) );
	const XString	kSeconds( _T( "Seconds" ) );

	const XString	kProtocolType( _T( "ProtocolType" ) );
	const XString	kInterceptId( _T( "InteceptId" ) );
	const XString	kFilterList( _T( "FilterList" ) );
	const XString	kFilter( _T( "Filter" ) );
	const XString	kAddress1( _T( "Address1" ) );
	const XString	kAddress2( _T( "Address2" ) );
	const XString	kAnyAddressEnabled( _T( "AnyAddressEnabled" ) );
	const XString	kDirection( _T( "Direction" ) );
	const XString	kPortEnabled( _T( "PortEnabled" ) );
	const XString	kPort( _T( "Port" ) );
	const XString	kMACHeader( _T( "MACHeader" ) );
	const XString	kSrcMAC( _T( "SrcMAC" ) );
	const XString	kDstMAC( _T( "DstMAC" ) );
	const XString	kMACProtocolIp( _T( "ProtoIp" ) );
	const XString	kMACProtocolType( _T( "ProtoType" ) );
	const XString	kVLANID( _T( "VLANID" ) );
	const XString	kVLANTag( _T( "VLANTag" ) );
	const XString	kFragmentAge( _T( "FragmentAge" ) );
	const XString	kSaveOrphans( _T( "SaveOrphans" ) );
	const XString	kFilename( _T( "Filename" ) );
	const XString	kInterval( _T( "Interval" ) );
	const XString	kType( _T( "Type" ) );
}


// ============================================================================
//		CInterceptId
// ============================================================================

CInterceptId::CInterceptId(
	const CString inValue )
{
	Parse( kIdType_Ericsson, inValue );
}


// ----------------------------------------------------------------------------
//		Format
// ----------------------------------------------------------------------------

CString
CInterceptId::Format(
	bool	inPad /*= true*/) const
{
	CString	strValue;
	if ( !IsNull() ) {
		switch ( m_nType ) {
		case kIdType_PCLI:
			break;

		case kIdType_SecurityId:
			if ( inPad ) {
				strValue.Format( L"%1X %02X %02X %02X",
					((m_Id & 0x0F000000) >> 24), ((m_Id & 0x00FF0000) >> 16),
					((m_Id & 0x0000FF00) >> 8), (m_Id & 0x000000FF) );
			}
			else {
				strValue.Format( L"%1X%02X%02X%02X",
					((m_Id & 0x0F000000) >> 24), ((m_Id & 0x00FF0000) >> 16),
					((m_Id & 0x0000FF00) >> 8), (m_Id & 0x000000FF) );
			}
			break;

		case kIdType_Ericsson:
			{
				CStringA	strA;
				strA.Format( "%u,%s", m_Id, m_strSite.c_str() );
				strValue = strA;
			}
			break;

		case kIdType_Arris:
			strValue.Format( L"%u", m_Id );
			break;

		case kIdType_Juniper:
			strValue.Format( L"%u", m_Id );
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
	int				inType,
	const CString	inValue )
{
	Reset();
	if ( inValue.IsEmpty() ) return;

	switch ( inType ) {
	case kIdType_PCLI:
		break;

	case kIdType_SecurityId:
		{
			CString	strId( inValue );
			strId.Remove( L' ' );
			strId.Remove( L'-' );

			UInt32	nId( 0 );
			size_t	nCount = strId.GetLength();
			for ( size_t i = 0; i < nCount; i++ ) {
				UInt16	nDigit( strId[static_cast<int>( i )] );
				UInt8	nValue( 0 );
				if ( (nDigit >= L'0') && (nDigit <= L'9') ) {
					nValue = nDigit - L'0';
				}
				else if ( (nDigit >= L'a') && (nDigit <= L'f') ) {
					nValue = 10 + (nDigit - L'a');
				}
				else if ( (nDigit >= L'A') && (nDigit <= L'F') ) {
					nValue = 10 + (nDigit - L'A');
				}

				nId = (nId << 4) | (nValue & 0x0F);
			}
			Set( kIdType_SecurityId, nId );
		}
		break;

	case kIdType_Ericsson:
		{
			int		nIndex( 0 );
			CString strId = inValue.Tokenize( L",", nIndex );
			UInt32	nId( _wtoi( strId ) );
			CString strSite = inValue.Tokenize( L",", nIndex );
			Set( nId, strSite );
		}
		break;

	case kIdType_Arris:
		{
			UInt32	nId( _wtoi( inValue ) );
			Set( kIdType_Arris, nId );
		}
		break;

	case kIdType_Juniper:
		{
			UInt32	nId( _wtoi( inValue ) );
			// Set will mask the Id.
			Set( kIdType_Juniper, nId );
		}
		break;
	}
}


// ============================================================================
//		CSecurityId
// ============================================================================

CSecurityId::CSecurityId(
	const CString inValue )
{
	Reset();

	CString	strValue( inValue );
	strValue.Remove( L' ' );
	strValue.Remove( L'-' );

	if ( inValue.IsEmpty() ) return;

	if ( strValue.GetLength() % 2 ) {
		strValue.Insert( 0, L"0" );
	}

	for ( int i = (strValue.GetLength() - (s_nSize * 2)); i < s_nSize; i++ ) {
		wchar_t	sz[4] = { strValue[i*2], strValue[i*2+1], 0, 0 };
		fId.fByte[i] = static_cast<UInt8>( wcstoul( sz, NULL, s_nSize ) );
	}

	SetInterceptId();
}


// ----------------------------------------------------------------------------
//		Format
// ----------------------------------------------------------------------------

CString
CSecurityId::Format() const
{
	CString	strValue;
	if ( !IsNull() ) {
		strValue.Format( L"%02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X",
			fId.fByte[0], fId.fByte[1], fId.fByte[2], fId.fByte[3], 
			fId.fByte[4], fId.fByte[5], fId.fByte[6], fId.fByte[7], 
			fId.fByte[8], fId.fByte[9], fId.fByte[10], fId.fByte[11], 
			fId.fByte[12], fId.fByte[13], fId.fByte[14], fId.fByte[15] );
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
	size_t			inLength,
	const UInt8*	inData )
{
	UInt32	nId = MemUtil::Swap32( *(reinterpret_cast<const UInt32*>( inData )) );
	m_InterceptId.Set( CInterceptId::kIdType_Arris, nId );

	return true;
}


// ============================================================================
//		CJuniper
// ============================================================================

// ----------------------------------------------------------------------------
//		Parse
// ----------------------------------------------------------------------------

bool
CJuniper::Parse(
	size_t			inLength,
	const UInt8*	inData )
{
	if ( inLength < (sizeof(UInt32) * 2) ) return false;
	
	// Upper 2 bits of the first word is the Mirror Header Value.
	// Lower 30 bits of the first word is the Intercept Id.
	// The second word is the Session ID.
	UInt32	nId = MemUtil::Swap32( *(reinterpret_cast<const UInt32*>( inData )) );
	
	// Set clears the 2 upper bits.
	m_InterceptId.Set( CInterceptId::kIdType_Juniper, nId );

	return true;
}


// ============================================================================
//		CIpFilter
// ============================================================================

// ----------------------------------------------------------------------------
//		Format
// ----------------------------------------------------------------------------

CString
CIpFilter::Format() const
{
	CString	str;
	str.Format( _T( "%s %s %s" ), m_Address1.Format(), FormatDirection(), m_Address2.Format() );
	if ( IsPort() ) {
		str.AppendFormat( _T( " : %s" ), m_Port.Format() );
	}

	return str;
}


// ----------------------------------------------------------------------------
//		IsMatch
// ----------------------------------------------------------------------------

bool
CIpFilter::IsMatch(
	const CIpAddressPair&	inOther,
	bool					inIsUdp,
	const CIpPort&			inPort ) const
{
	bool			bAccept = false;
	CIpAddressPair	iap( GetAddress1(), GetAddress2() );

	switch ( GetDirection() ) {
		case CIpFilter::kDirection_Sent:			// Address 1 (Source) to 2 (Destination)
			bAccept = (IsAnyOtherAddress())
				? (inOther.GetSource() == iap.GetSource())
				: inOther.IsExact( iap  );
			break;

		case CIpFilter::kDirection_Received:		// Address 2 (Destination) to 1 (Source)
			bAccept = (IsAnyOtherAddress())
				? (inOther.GetDestination() == iap.GetSource())
				: inOther.IsExactOpposite( iap );
			break;

		case CIpFilter::kDirection_Either:
		default:
			bAccept = (IsAnyOtherAddress())
				? inOther.EitherIs( iap.GetSource() )
				: (inOther == iap);	// exact or exact opposite.
			break;
	}
	if ( !bAccept ) return false;

	// If a Port was configured and there is a port.
	// Only first CMI packets have UDP header and thus a port to check.
	if ( IsPort() && inIsUdp ) {
		// Test Port
		if ( inPort != GetPort() ) return false;
	}

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

CString
CIpFilterList::Format() const
{
	CString	strResult;
	std::vector<CIpFilter>::const_iterator	itr;
	for ( itr = begin(); itr != end(); ++itr ) {
		CString	str = itr->Format();
		strResult.Append( str );
		strResult.Append( _T( "\n" ) );
	}
	strResult.TrimRight( _T( "\n" ) );

	return strResult;
}


// ----------------------------------------------------------------------------
//		IsMatch
// ----------------------------------------------------------------------------

bool
CIpFilterList::IsMatch(
	const CIpAddressPair&	inOther,
	bool					inIsUdp,
	const CIpPort&			inPort ) const
{
	std::vector<CIpFilter>::const_iterator	itr;
	for ( itr = begin(); itr != end(); ++itr ) {
		if ( itr->IsMatch( inOther, inIsUdp, inPort ) ) return true;
	}

	return false;
}


// ============================================================================
//		COptions
// ============================================================================

CString	COptions::s_ayProtocolNames[] = {
	L"CMI",
	L"PCLI",
	L"SecurityId",
	L"Ericsson",
	L"Arris",
	L"Juniper/Nokia"
};


// ----------------------------------------------------------------------------
//		Copy
// ----------------------------------------------------------------------------

void
COptions::Copy(
	const COptions&	in )
{
	SetProtocolType( in.GetProtocolType() );
	SetInterceptId( in.GetInterceptId() );
	SetIpFilterList( in.GetIpFilterList() );
	SetInsertMAC( in.IsInsertMAC() );
	SetSrcMAC( in.GetSrcMAC() );
	SetDstMAC( in.GetDstMAC() );
	SetMACProtocolType( in.IsMACProtocolIp(), in.GetMACProtocolType() );
	SetInsertVLAN( in.IsInsertVLAN() );
	SetVLANID( in.GetVLANID() );
	SetFragmentAge( in.GetFragmentAge() );
	SetSaveOrphans( in.IsSaveOrphans() );
	SetSaveInterval( in.GetSaveInterval() );
	SetFileName( in.GetFileName() );
}


// ----------------------------------------------------------------------------
//		GetContextPrefs
// ----------------------------------------------------------------------------

CStringA
COptions::GetContextPrefs() const
{
	CStringA	strXml;

	try {
		XXMLDoc		theDoc;
		XXMLElement	theRoot( ConfigTags::kRootName, false );
		theDoc.SetRootElement( &theRoot );

		// Protocol Options, includes Secure Id Options
		XXMLElement theProtocolTypeTag( ConfigTags::kProtocolType, false );
		theRoot.AddChild( &theProtocolTypeTag );
		theProtocolTypeTag.AddAttribute( ConfigTags::kType, FormatProtocolType() );
		theProtocolTypeTag.AddAttribute( ConfigTags::kInterceptId, GetInterceptId().Format( false ) );

		// IP Filter List Options
		vector<XXMLElement*>	ayIpFilters;
		XXMLElement	theIpFilterList( ConfigTags::kFilterList, false );
		theRoot.AddChild( &theIpFilterList );
		for ( size_t it = 0; it < m_ayIpFilters.size(); ++it ) {
			const CIpFilter*	pIpFilter = &m_ayIpFilters[it];
			XXMLElement*		pFilterElement = new XXMLElement( ConfigTags::kFilter, false );
			pFilterElement->AddAttribute( ConfigTags::kAddress1, pIpFilter->FormatAddress1() );
			pFilterElement->AddAttribute( ConfigTags::kDirection, pIpFilter->FormatDirection() );
			pFilterElement->AddAttribute( ConfigTags::kAddress2, pIpFilter->FormatAddress2() );
			pFilterElement->AddAttribute( ConfigTags::kAnyAddressEnabled, pIpFilter->IsAnyOtherAddress() ? ConfigTags::kTrue : ConfigTags::kFalse );
			pFilterElement->AddAttribute( ConfigTags::kPortEnabled, pIpFilter->IsPort() ? ConfigTags::kTrue : ConfigTags::kFalse );
			pFilterElement->AddAttribute( ConfigTags::kPort, pIpFilter->FormatPort() );

			theIpFilterList.AddChild( pFilterElement );
			ayIpFilters.push_back( pFilterElement );
		}

		// MACHeader Options
		XXMLElement	theMACHeader( ConfigTags::kMACHeader, false );
		theRoot.AddChild( &theMACHeader );
		theMACHeader.AddAttribute( ConfigTags::kEnabled, IsInsertMAC() ? ConfigTags::kTrue : ConfigTags::kFalse );
		theMACHeader.AddAttribute( ConfigTags::kSrcMAC, GetSrcMAC().Format() );
		theMACHeader.AddAttribute( ConfigTags::kDstMAC, GetDstMAC().Format() );
		theMACHeader.AddAttribute( ConfigTags::kMACProtocolIp, IsMACProtocolIp() ? ConfigTags::kTrue : ConfigTags::kFalse );
		theMACHeader.AddAttribute( ConfigTags::kMACProtocolType, GetMACProtocolType().Format() );

		// VLAN Options
		XXMLElement theVLANTag( ConfigTags::kVLANTag, false );
		theRoot.AddChild( &theVLANTag );
		theVLANTag.AddAttribute( ConfigTags::kEnabled, IsInsertVLAN() ? ConfigTags::kTrue : ConfigTags::kFalse );
		theVLANTag.AddAttribute( ConfigTags::kVLANID, GetVLANID().Format() );

		// Save Options
		XXMLElement theSave( ConfigTags::kSaveOrphans, false );
		theRoot.AddChild( &theSave );
		theSave.AddAttribute( ConfigTags::kEnabled, IsSaveOrphans() ? ConfigTags::kTrue : ConfigTags::kFalse );
		theSave.AddAttribute( ConfigTags::kFilename, GetFileName() );
		UInt32	nSaveCount( 0 );
		int		nSaveUnits( 0 );
		GetSaveInterval( nSaveCount, nSaveUnits );
		theSave.AddAttribute( ConfigTags::kCount, nSaveCount );
		theSave.AddAttribute( ConfigTags::kUnits, GetUnits( nSaveUnits ) );

		// Age Options
		XXMLElement theFragmentAge( ConfigTags::kFragmentAge, false );
		theRoot.AddChild( &theFragmentAge );
		theFragmentAge.AddAttribute( ConfigTags::kSeconds, GetFragmentAge() );

		// build the stream
		CByteStream	bsPrefs;
		theDoc.BuildStream( bsPrefs );

		char*	pszXml = strXml.GetBufferSetLength( bsPrefs.GetCount() );
		memcpy( pszXml, bsPrefs.GetData(), bsPrefs.GetCount() );
		strXml.ReleaseBuffer();

		theDoc.SetRootElement( NULL );

		vector<XXMLElement*>::iterator itr;
		for ( itr = ayIpFilters.begin(); itr != ayIpFilters.end(); ++itr ) {
			delete *itr;
		}
	}
	catch ( ... ) {
		;	// squelch
	}
	return strXml;
}


// ----------------------------------------------------------------------------
//		SetContextPrefs
// ----------------------------------------------------------------------------

bool
COptions::SetContextPrefs(
	CStringA	inPrefs )
{
	bool	bReturnValue( true );
	HRESULT	hr = S_OK;
	bool	bResult;

	try {
		CByteStream	theByteStream( inPrefs.GetBuffer(), inPrefs.GetLength(), false );

		// Parse the stream into an XMLDoc
		XXMLDoc	doc;
		doc.ParseStream( theByteStream );
		const XXMLElement* pRoot = doc.GetRootElement();
		if ( !pRoot ) {
			return false;
		}

		// This is the place to put values from the xml stream
		XString theValue;

		// Get the Encapsulation Protocol options
		const XXMLElement* pProtocolTypeTag = pRoot->GetNamedChild( ConfigTags::kProtocolType );
		if ( pProtocolTypeTag ) {
			bResult = pProtocolTypeTag->GetNamedAttributeValue( ConfigTags::kType, theValue );
			ASSERT( bResult != FALSE );
			if ( bResult != FALSE ) {
				SetProtocolType( static_cast<const TCHAR*>( theValue ) );
			}
			else {
				bReturnValue = false;
			}

			CInterceptId	id;
			bResult = pProtocolTypeTag->GetNamedAttributeValue( ConfigTags::kInterceptId, theValue );
			if ( bResult != FALSE ) {
				id.Parse( m_ProtocolType, static_cast<const TCHAR*>( theValue ) );
			}
			SetInterceptId( id );
		}
		else {
			SetProtocolType( kType_CMI );
		}

		// Get the IP Filter List Options
		m_ayIpFilters.clear();
		const XXMLElement* pIpFilterList = pRoot->GetNamedChild( ConfigTags::kFilterList );
		if ( pIpFilterList ) {
			size_t	nCount = pIpFilterList->GetNamedChildCount( ConfigTags::kFilter );
			for ( size_t i = 0; i < nCount; ++i ) {
				CIpFilter			theIpFilter;
				const XXMLElement*	pIpFilter = pIpFilterList->GetNamedChild( ConfigTags::kFilter, static_cast<SInt32>( i ) );
				if ( !pIpFilter ) continue;

				bResult = pIpFilter->GetNamedAttributeValue( ConfigTags::kAddress1, theValue );
				ASSERT( bResult != FALSE );
				if ( bResult != FALSE ) {
					theIpFilter.SetAddress1( NETWORKTOHOST32( inet_addr( CT2CA( theValue ) ) ) );
				}
				else {
					bReturnValue = false;
				}

				bResult = pIpFilter->GetNamedAttributeValue( ConfigTags::kDirection, theValue );
				ASSERT( bResult != FALSE );
				if ( bResult != FALSE ) {
					theIpFilter.SetDirection( static_cast<const TCHAR*>( theValue ) );
				}
				else {
					bReturnValue = false;
				}

				bResult = pIpFilter->GetNamedAttributeValue( ConfigTags::kAddress2, theValue );
				ASSERT( bResult != FALSE );
				if ( bResult != FALSE ) {
					theIpFilter.SetAddress2( NETWORKTOHOST32( inet_addr( CT2CA( theValue ) ) ) );
				}
				else {
					bReturnValue = false;
				}

				bResult = pIpFilter->GetNamedAttributeValue( ConfigTags::kAnyAddressEnabled, theValue );
				ASSERT( bResult != FALSE );
				if ( bResult != FALSE ) {
					theIpFilter.SetAnyAddress( theValue == ConfigTags::kTrue );
				}
				else {
					bReturnValue = false;
				}

				bResult = pIpFilter->GetNamedAttributeValue( ConfigTags::kPortEnabled, theValue );
				ASSERT( bResult != FALSE );
				if ( bResult != FALSE ) {
					theIpFilter.SetPort( theValue == ConfigTags::kTrue );
				}
				else {
					bReturnValue = false;
				}
				bResult = pIpFilter->GetNamedAttributeValue( ConfigTags::kPort, theValue );
				ASSERT( bResult != FALSE );
				if ( bResult != FALSE ) {
					CIpPort	thePort( _ttoi( theValue ) );
					theIpFilter.SetPort( thePort );
				}
				else {
					bReturnValue = false;
				}

				m_ayIpFilters.push_back( theIpFilter );
			}
		}
		else {
			bReturnValue = false;
		}

		// Get the MACHeader options
		const XXMLElement* pMACHeader = pRoot->GetNamedChild( ConfigTags::kMACHeader );
		ASSERT( pMACHeader );
		if ( pMACHeader ) {
			bResult = pMACHeader->GetNamedAttributeValue( ConfigTags::kEnabled, theValue );
			ASSERT( bResult != FALSE );
			if ( bResult != FALSE ) {
				SetInsertMAC( theValue == ConfigTags::kTrue );
			}
			else {
				bReturnValue = false;
			}
			bResult = pMACHeader->GetNamedAttributeValue( ConfigTags::kSrcMAC, theValue );
			ASSERT( bResult != FALSE );
			if ( bResult != FALSE ) {
				SetSrcMAC( static_cast<const TCHAR*>( theValue ) );
			}
			else {
				bReturnValue = false;
			}
			bResult = pMACHeader->GetNamedAttributeValue( ConfigTags::kDstMAC, theValue );
			ASSERT( bResult != FALSE );
			if ( bResult != FALSE ) {
				SetDstMAC( static_cast<const TCHAR*>( theValue ) );
			}
			else {
				bReturnValue = false;
			}

			bool	bMacProtocolIp = false;
			bResult = pMACHeader->GetNamedAttributeValue( ConfigTags::kMACProtocolIp, theValue );
			ASSERT( bResult != FALSE );
			if ( bResult != FALSE ) {
				bMacProtocolIp = (theValue == ConfigTags::kTrue);
			}
			bResult = pMACHeader->GetNamedAttributeValue( ConfigTags::kMACProtocolType, theValue );
			ASSERT( bResult != FALSE );
			if ( bResult != FALSE ) {
				SetMACProtocolType( bMacProtocolIp, _ttoi( theValue ) );
			}
			else {
				bReturnValue = false;
			}
		}
		else {
			bReturnValue = false;
		}

		// Get the VLAN options
		const XXMLElement* pVLANTag = pRoot->GetNamedChild( ConfigTags::kVLANTag );
		ASSERT( pVLANTag );
		if ( pVLANTag ) {
			bResult = pVLANTag->GetNamedAttributeValue( ConfigTags::kEnabled, theValue );
			ASSERT( bResult != FALSE );
			if ( bResult != FALSE ) {
				SetInsertVLAN( theValue == ConfigTags::kTrue );
			}
			else {
				bReturnValue = false;
			}
			bResult = pVLANTag->GetNamedAttributeValue( ConfigTags::kVLANID, theValue );
			ASSERT( bResult != FALSE );
			if ( bResult != FALSE ) {
				SetVLANID( _ttoi( theValue ) );
			}
			else {
				bReturnValue = false;
			}
		}
		else {
			bReturnValue = false;
		}

		// Get the Save Orphans Options
		const XXMLElement* pSave = pRoot->GetNamedChild( ConfigTags::kSaveOrphans );
		ASSERT( pSave );
		if ( pSave ) {
			bResult = pSave->GetNamedAttributeValue( ConfigTags::kEnabled, theValue );
			ASSERT( bResult != FALSE );
			if ( bResult != FALSE ) {
				SetSaveOrphans( theValue == ConfigTags::kTrue );
			}
			else {
				bReturnValue = false;
			}
			bResult = pSave->GetNamedAttributeValue( ConfigTags::kFilename, theValue );
			ASSERT( bResult != FALSE );
			if ( bResult != FALSE ) {
				SetFileName( static_cast<const TCHAR*>( theValue ) );
			}
			else {
				bReturnValue = false;
			}
			UInt32	nSaveCount( 0 );
			bResult = pSave->GetNamedAttributeValue( ConfigTags::kCount, theValue );
			ASSERT( bResult != FALSE );
			if ( bResult != FALSE ) {
				nSaveCount = _ttoi( theValue );
			}
			else {
				bReturnValue = false;
			}
			bResult = pSave->GetNamedAttributeValue( ConfigTags::kUnits, theValue );
			ASSERT( bResult != FALSE );
			if ( bResult == FALSE ) {
				bReturnValue = false;
			}
			SetSaveInterval( nSaveCount, static_cast<CString>( theValue ) );
		}
		else {
			bReturnValue = false;
		}

		// Get the Age Options
		const XXMLElement* pAge = pRoot->GetNamedChild( ConfigTags::kFragmentAge );
		ASSERT( pAge );
		if ( pAge ) {
			bResult = pAge->GetNamedAttributeValue( ConfigTags::kSeconds, theValue );
			ASSERT( bResult != FALSE );
			if ( bResult != FALSE ) {
				SetFragmentAge( _ttoi( theValue ) );
			}
			else {
				bReturnValue = false;
			}
		}
		else {
			bReturnValue = false;
		}
	}
	catch ( ... ) {
		; // squelch
	}

	return true;	// bReturnValue;
}


// ----------------------------------------------------------------------------
//		SetProtocolType
// ----------------------------------------------------------------------------

void
COptions::SetProtocolType(
	const CString	inType )
{
	for ( int i = 0; i < kType_Max; i++ ) {
		if ( inType == s_ayProtocolNames[i] ) {
			SetProtocolType( i );
			break;
		}
	}
}


// ----------------------------------------------------------------------------
//		SetSaveInterval
// ----------------------------------------------------------------------------

void
COptions::SetSaveInterval(
	UInt32	inCount,
	CString	inUnits )
{
	int	nUnits = -1;
	for ( int i = 0; i < 4; i++ ) {
		if ( inUnits.CompareNoCase( Tags::kTimeUnitsPlural[i] ) == 0 ) {
			nUnits = i;
			break;
		}
	}
	if ( nUnits == -1 ) {
		for ( int i = 0; i < 4; i++ ) {
			if ( inUnits.CompareNoCase( Tags::kTimeUnitsSingle[i] ) == 0 ) {
				nUnits = i;
				break;
			}
		}
	}
	if ( nUnits == -1 ) {
		nUnits = 0;
	}
	SetSaveInterval( inCount, nUnits );
}

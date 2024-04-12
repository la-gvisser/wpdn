// ============================================================================
// DnsEntry.cpp:
//		implementation of the CDnsEntry classes.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2009. All rights reserved.

#include "StdAfx.h"
#include "DnsEntry.h"


// ============================================================================
//		Globals
// ============================================================================

static CDnsEntry	gkNullEntry( CDnsEntry::kEntry_Unknown, *(reinterpret_cast<CDnsEntryList*>( NULL )) );


// ============================================================================
//		DNSEntry
// ============================================================================

namespace DNSEntry
{
	enum
	{
		kUnknown = 0,
		kQuestions,
		kAnswers,
		kAuthorities,
		kResources,
		kEntries
	};
}


// ============================================================================
//		CDnsEntry
// ============================================================================

// ----------------------------------------------------------------------------
//		Format
// ----------------------------------------------------------------------------

CPeekString
CDnsEntry::Format() const
{
	static CPeekString strEntries[] = {
		L"Qest",
		L"Ans ",
		L"Auth",
		L"Res ",
		L"****"
	};
	ASSERT( m_nType < 4 );
	CPeekOutString	strOut1;
	strOut1 << strEntries[m_Entry] << L" " << m_nType << L" " << m_nClass << L" " << m_strName;

	CPeekString str;
	if ( m_Entry != kEntry_Question ) {
		CPeekOutString strOut2;
		strOut2 << L" " << m_nTimeToLive;
		str = strOut2;
		if ( !m_strAltName.IsEmpty() ) {
			str += m_strAltName;
		}
		else {
			str += m_IpAddress.Format();
		}
	}
	else {
		str = strOut1;
	}

	return str;
}


// ----------------------------------------------------------------------------
//		ParseQuestion
// ----------------------------------------------------------------------------

int
CDnsEntry::ParseQuestion(
	UInt32			inLength,
	const UInt8*	inData )
{
	UInt32			nLength( inLength );
	const UInt8*	pData( inData );

	// Check for a NULL terminated string in the data.
	int	nNameLength = CDnsEntryList::SafeStrLen( nLength, pData );
	if ( nNameLength < 0 ) {
		ParseError( L"Error in DNS Parse for question - no name or name too long." );
		return -1;
	}

	m_strName = m_Parent.ParseCName( nNameLength, pData, 0 );
	pData += (nNameLength + 1);
	nLength -= (nNameLength + 1);

	// Check for enough space for the Type and Class.
	if ( nLength < (sizeof( UInt16 ) * 2) ) {
		ParseError( L"Error in DNS Parse for question - Type or Class data length issue." );
		return -1;
	}

	m_nType = NETWORKTOHOST16( *(reinterpret_cast<const UInt16*>( pData )) );
	pData += 2;
	m_nClass = NETWORKTOHOST16( *(reinterpret_cast<const UInt16*>( pData )) );
	pData += 2;

	return static_cast<int>(pData - inData);	// return number of bytes parsed.
}


// ----------------------------------------------------------------------------
//		ParseRecord
// ----------------------------------------------------------------------------

int
CDnsEntry::ParseRecord(
	UInt32			inLength,
	const UInt8*	inData )
{
	UInt32			nLength( inLength );
	const UInt8*	pData( inData );

	// Check for a NULL terminated string in the data.
	int	nNameLength = CDnsEntryList::SafeStrLen( nLength, pData );
	if ( nNameLength < 0 ) {
		ParseError( L"Error in DNS Parse for record - no name or name too long." );
		return -1;
	}

	m_strName = m_Parent.ParseCName( nNameLength, pData, 0 );
	nLength -= (nNameLength + 1);
	pData += (nNameLength + 1);

	// Check for enough space for the Type and Class.
	if ( nLength < ((sizeof( UInt16 ) * 3) + sizeof( UInt32 )) ) {
		ParseError( L"Error in DNS Parse for record - Type or Class data length issue." );
		return -1;
	}

	m_nType = NETWORKTOHOST16( *(reinterpret_cast<const UInt16*>( pData )) );
	nLength -= sizeof( UInt16 );
	pData += sizeof( UInt16 );

	m_nClass = NETWORKTOHOST16( *(reinterpret_cast<const UInt16*>( pData )) );
	nLength -= sizeof( UInt16 );
	pData += sizeof( UInt16 );

	m_nTimeToLive = NETWORKTOHOST32( *(reinterpret_cast<const UInt32*>( pData )) );
	nLength -= sizeof( UInt32 );
	pData += sizeof( UInt32 );

	UInt32	nDataLen = NETWORKTOHOST16( *(reinterpret_cast<const UInt16*>( pData )) );
	nLength -= sizeof( UInt16 );
	pData += sizeof( UInt16 );

	if ( nDataLen > 0 ) {
		if ( nDataLen > nLength ) {
			ParseError( L"Error in DNS Parse for record - data length calculation issue." );
			return -1;
		}
		m_Data.SetSize( nDataLen );
		memcpy( m_Data.GetData(), pData, nDataLen );
		switch ( m_nType ) {
		case kType_HostAddress:
			m_IpAddress = reinterpret_cast<const _tIpV4Address*>( pData );
			break;
		case kType_CononicalName:
		case kType_AuthNameServer:
			m_strAltName = m_Parent.ParseCName( nDataLen, pData, 0 );
			break;
		}

		nLength -= nDataLen;
		pData += nDataLen;
	}
	return static_cast<int>(pData - inData);	// return number of bytes parsed.
}


// ----------------------------------------------------------------------------
//		ParseError
// ----------------------------------------------------------------------------

void
CDnsEntry::ParseError(
	CPeekString inMessage ) 
{
	m_Parent.ParseError( inMessage );
}


// ============================================================================
//		CEntryList
// ============================================================================

// ----------------------------------------------------------------------------
//		GetResolved
// ----------------------------------------------------------------------------

void
CDnsEntryList::GetResolved(
	CResolvedDomainList& outList ) const
{
	outList.RemoveAll();
	for ( size_t i = 0; i < m_nQuestions; i++ ) {
		const CDnsEntry&	theEntry( GetAt( i ) );
		CPeekString				strName( theEntry.GetName() );
		size_t				nIndex( 0 );
		if ( !outList.Find( strName, nIndex ) ) {
			CResolvedDomainList		theResovledList;
			if ( ResolveDnsEntry( theEntry, theResovledList ) ) {
				outList.Append( theResovledList );
			}
		}
	}
}


// ----------------------------------------------------------------------------
//		ResolveDnsEntry
// ----------------------------------------------------------------------------

bool
CDnsEntryList::ResolveDnsEntry(
	const CDnsEntry&		inEntry,
	CResolvedDomainList&	outResolvedList ) const
{
	int		nIndex = FindQuestion( inEntry.GetName() );
	if ( nIndex < 0 ) return false;

	outResolvedList.RemoveAll();
	{
		// modifying outResolvedList invalidates theResolved.
		outResolvedList.Add();
		CResolvedDomain&	theResolved( outResolvedList.GetAt( 0 ) );
		theResolved.SetName( inEntry.GetName() );
	}

	size_t	nLastAnswer = m_nQuestions + m_nAnswers;
	for ( size_t i = m_nQuestions; i < nLastAnswer; i++ ) {
		const CDnsEntry&	theEntry( GetAt( i ) );
		if ( theEntry.IsType( kType_CononicalName ) ) {
			CPeekString	strName( theEntry.GetName() );
			if ( strName.CompareNoCase( inEntry.GetName() ) == 0 ) {
				size_t	nNew = outResolvedList.Add();
				CResolvedDomain&	newResolved( outResolvedList.GetAt( nNew ) );
				CResolvedDomain&	theResolved( outResolvedList.GetAt( 0 ) );
				newResolved.SetName( theEntry.GetAltName() );
				newResolved.SetAddressList( theResolved.GetAddressList() );
			}
		}
		else if ( theEntry.IsType( kType_HostAddress ) ) {
			outResolvedList.AddAddress( theEntry.GetAddress() );
		}
	}
	return !outResolvedList.IsEmpty();
}


// ----------------------------------------------------------------------------
//		Parse
// ----------------------------------------------------------------------------

int
CDnsEntryList::Parse(
	UInt32				inLength,
	const tDnsHeader*	inHeader )
{
	ASSERT( inLength >= sizeof( tDnsHeader ) );
	ASSERT( inHeader );
	if ( inLength < sizeof( tDnsHeader ) || inHeader == NULL ) {
		ParseError( L"Error in DNS Parse - invalid length or header value." );
		return -1;
	}

	m_nLength = inLength;
	m_pHeader = inHeader;

	m_nDNSType = DNSEntry::kUnknown;

	m_sDNSDataLength = inLength - sizeof( tDnsHeader );
	
	const UInt8*	pDNSData( reinterpret_cast<const UInt8*>( inHeader ) + sizeof( tDnsHeader ) );

	// Max Counts
	// 1518 - 14 (MAC Header) - 20 (IP Header) - 8 (UDP Header) - 4 (CRC) = 1472
	// 1472 - 12 (DNS Header) = 1460
	//   1460 / 6 (Min QName) = 243
	//   1460 / 14 (Min Resource) = 104

	UInt32	nMaxQuestions = static_cast<UInt32>( m_sDNSDataLength / 6 ) + 1;
	UInt32	nMaxResources = static_cast<UInt32>( m_sDNSDataLength / 14 ) + 1;

	m_nQuestions = NETWORKTOHOST16( inHeader->qdcount );
	m_nDNSType = DNSEntry::kQuestions;
	if ( m_nQuestions > nMaxQuestions ) {
		ParseError();
		return -1;
	}
	m_nAnswers = NETWORKTOHOST16( inHeader->ancount );
	m_nDNSType = DNSEntry::kAnswers;
	if ( m_nAnswers > nMaxResources ) {
		ParseError();
		return -1;
	}
	m_nAuthorities = NETWORKTOHOST16( inHeader->nscount );
	m_nDNSType = DNSEntry::kAuthorities;
	if ( m_nAuthorities > nMaxResources ) {
		ParseError();
		return -1;
	}
	m_nResources = NETWORKTOHOST16( inHeader->arcount );
	m_nDNSType = DNSEntry::kResources;
	if ( m_nResources > nMaxResources ) {
		ParseError();
		return -1;
	}
	
	m_nDNSType = DNSEntry::kEntries;
	if ( (m_nQuestions + m_nAnswers + m_nAuthorities + m_nResources) > nMaxResources ) {
		ParseError();
		return -1;
	}

	try
	{
		// First parse the questions (assume one and only one).
		for ( UInt32 i = 0; i < m_nQuestions; i++ ) {	
			CDnsEntry	theEntry( CDnsEntry::kEntry_Question, *this );
			int			nParsed = theEntry.ParseQuestion( m_sDNSDataLength, pDNSData );
			if ( nParsed < 0 ) {
				ParseError( L"Error in DNS Parse - ParseQuestion invalid." );
				return -1;
			}

			m_sDNSDataLength -= nParsed;
			ASSERT( m_sDNSDataLength >= 0 );
			if ( m_sDNSDataLength < 0 ) {
				ParseError( L"Error in DNS Parse - Attempt to parse question entry gives calculated length of zero." );
				return -1;
			}

			pDNSData += nParsed;

			Add( theEntry );
		}
		for ( UInt32 i = 0; i < m_nAnswers; i++ ) {	
			CDnsEntry	theEntry( CDnsEntry::kEntry_Answer, *this );
			int			nParsed = theEntry.ParseRecord( m_sDNSDataLength, pDNSData );
			if ( nParsed < 0 ) break;
			m_sDNSDataLength -= nParsed;
			ASSERT( m_sDNSDataLength >= 0 );
			if ( m_sDNSDataLength < 0 ) {
				ParseError( L"Error in DNS Parse - Attempt to parse answer entry gives calculated length of zero." );
				return -1;
			}
			pDNSData += nParsed;

			Add( theEntry );
		}

		for ( UInt32 i = 0; i < m_nAuthorities; i++ ) {	
			CDnsEntry	theEntry( CDnsEntry::kEntry_Authority, *this );
			int			nParsed = theEntry.ParseRecord( m_sDNSDataLength, pDNSData );
			if ( nParsed < 0 ) break;
			m_sDNSDataLength -= nParsed;
			ASSERT( m_sDNSDataLength >= 0 );
			if ( m_sDNSDataLength < 0 ) {
				ParseError( L"Error in DNS Parse - Attempt to parse authority entry gives calculated length less than zero." );
				return -1;
			}
			pDNSData += nParsed;

			Add( theEntry );
		}

		for ( UInt32 i = 0; i < m_nResources; i++ ) {	
			CDnsEntry	theEntry( CDnsEntry::kEntry_Resource, *this );
			int			nParsed = theEntry.ParseRecord( m_sDNSDataLength, pDNSData );
			if ( nParsed < 0 ) break;
			m_sDNSDataLength -= nParsed;
			ASSERT( m_sDNSDataLength >= 0 );
			if ( m_sDNSDataLength < 0 ) {
				ParseError( L"Error in DNS Parse - Attempt to parse resource entry gives calculated length less than zero." );
				return -1;
			}
			pDNSData += nParsed;

			Add( theEntry );
		}
	}
	catch(...)
	{
		ParseError( L"Exception error in DNS Parse." );
		return -1;
	}

	return static_cast<int>( GetCount() );
}


// ----------------------------------------------------------------------------
//		ParseError
// ----------------------------------------------------------------------------

void
CDnsEntryList::ParseError()
{
	CPeekString strLeader( L"Failure in DNS Parse: Too many " );
	CPeekString strType;

	switch( m_nDNSType )
	{
		case DNSEntry::kQuestions:
			strType = L"questions";
			break;
		case DNSEntry::kAnswers:
			strType = L"answers";
			break;
		case DNSEntry::kAuthorities:
			strType = L"authorities";
			break;
		case DNSEntry::kResources:
			strType = L"resources";
			break;
		case DNSEntry::kEntries:
			strType = L"entries";
			break;
		case DNSEntry::kUnknown:
		default:
			ParseError( L"Failure in DNS EntryList Parse: unknown error" );
			return;
	}

	CPeekOutString strOutMessage;
	strOutMessage << strLeader << strType << L" - " << m_sDNSDataLength << L":" << m_nQuestions 
		<< L"/" << m_nAnswers <<  L"/" << m_nAuthorities << L"/" << m_nResources;

	ParseError( strOutMessage );
}


// ----------------------------------------------------------------------------
//		ParseCName
// ----------------------------------------------------------------------------

CPeekString
CDnsEntryList::ParseCName(
	UInt32			inLength,
	const UInt8*	inData,
	int				inDepth )
{
	ASSERT( inDepth < 500 );
	if ( inDepth >= 500 ) {
		ParseError( L"ParseCName: Excessive recursion, raising a exception." );
		Peek::Throw( -1 );
	}

	const UInt8*	pData( inData );
	UInt32			nLength( inLength );
	CPeekString		strCName;
	while ( *pData ) {
		if ( IsCompressed( pData ) ) {
			const UInt16*	pOffset( reinterpret_cast<const UInt16*>( pData ) );
			UInt32			nOffset = NETWORKTOHOST16( *pOffset ) & 0x3FFF;
			ASSERT( nOffset < m_nLength );
			if ( nOffset > m_nLength ) {
				ParseError( L"ParseCName: Unable to parse the CName due to offset error, raising a exception." );
				Peek::Throw( -1 );
			}

			pData += 2;		// 16-bit offset.
			UInt32			nCNLen = m_nLength - nOffset;
			const UInt8*	pCN = reinterpret_cast<const UInt8*>( m_pHeader ) + nOffset;
			CPeekString		strCN = ParseCName( nCNLen, pCN, (inDepth + 1) );
			if ( !strCName.IsEmpty() ) strCName += L'.';
			strCName.Append( strCN );
			break;
		}
		else {
			UInt8	nLen = *pData & 0x3F;
			if ( nLen > nLength ) return CPeekString();
			pData++;

			CPeekStringA	strNameA( reinterpret_cast<const char*>( pData ), nLen );

			pData += nLen;

			if ( !strCName.IsEmpty() ) strCName += L'.';
			strCName.Append( CPeekString( strNameA ) );
		}
	}
	return strCName;
}


// ----------------------------------------------------------------------------
//		SafeStrLen
// ----------------------------------------------------------------------------

int
CDnsEntryList::SafeStrLen(
	UInt32			inLength,
	const UInt8*	inData )
{
	const UInt8*	pData( inData );
	const UInt8*	pEnd( inData + inLength );
	while ( pData < pEnd ) {
		if ( *pData == 0 ) break;
		if ( IsCompressed( pData ) ) {
			pData++;	// LSB of compression length
			break;
		}
		UInt8	nLen( *pData & 0x3F );
		pData += (nLen + 1);
	}
	if ( pData >= pEnd ) return -1;

	return static_cast<int>( pData - inData );
}

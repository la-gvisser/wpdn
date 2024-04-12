// ============================================================================
//	TargetAddress.h
//		interface for the CTargetAddress.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "TargetAddress.h"
#include "GlobalTags.h"
#include "RegExps.h"
#include "PeekDataModeler.h"


// ============================================================================
//		Default Values
// ============================================================================

namespace Defaults
{
	extern const bool	bCaptureAll( false );
}


// ============================================================================
//		Global Tags
// ============================================================================

namespace Tags
{
	const CPeekStringA	kLogAddressListA( " Target Address List set to:\r\n" );
}


// ============================================================================
//		Configuration Tags
// ============================================================================

namespace ConfigTags
{
	const CPeekString	kCaptureAll( L"CaptureAll" );
	const CPeekString	kAddress( L"Address" );
}

// ============================================================================
//		CTargetAddress
// ============================================================================

// ----------------------------------------------------------------------------
//		Format
// ----------------------------------------------------------------------------

CPeekString
CTargetAddress::Format() const
{
	const CPeekString*	ayStrs[] = {
		m_strAddressX.RefW(),
		Tags::kxSpace.RefW(),
		Tags::kxParenLeft.RefW(),
		FormatEnabledRef( IsEnabled() ),
		Tags::kxParenRight.RefW(),
		NULL
	};

	return FastCat( ayStrs );
}


// ----------------------------------------------------------------------------
//		FormatA
// ----------------------------------------------------------------------------

CPeekStringA
CTargetAddress::FormatA() const
{
#if (TOVERIFY)
	const CPeekStringA*	ayStrs[] = {
		m_strAddressX.RefA(),
		Tags::kxSpace.RefA(),
		Tags::kxParenLeft.RefA(),
		FormatEnabledRefA( IsEnabled() ),
		Tags::kxParenRight.RefA(),
		NULL
	};

	return FastCat( ayStrs );
#else
	CPeekStringA	str;
	str.Format(
		"%s (%s)",
		(CPeekStringA) m_strAddressX,
		((IsEnabled()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
	return str;
#endif // TOVERIFY
}


// ----------------------------------------------------------------------------
//		Get
// ----------------------------------------------------------------------------

const CPeekStringX&
CTargetAddress::Get(
	BOOL&	outCheck ) const
{
	outCheck = m_bEnabled;
	return m_strAddressX;
}



// ----------------------------------------------------------------------------
//		IsMatch
// ----------------------------------------------------------------------------

bool
CTargetAddress::IsMatch(
	const CPeekString&	inAddress ) const
{
	if ( m_bEnabled ) {
		switch ( m_Type ) {
		case kTypeAddress:
			return (m_strAddressX.CompareNoCase( inAddress ) == 0);

		case kTypeDomain:
			{
				int	nAtIndex = inAddress.Find( L"@" );
				if ( nAtIndex < 0 ) return false;
				nAtIndex++;	// skip the at-sign

				size_t	nLen = inAddress.GetLength();
				if ( nAtIndex > static_cast<int>( inAddress.GetLength() ) ) return false;
				size_t	nDomainLength = nLen - nAtIndex;

				return (m_strAddressX.CompareNoCase( inAddress.Right( nDomainLength ) ) == 0);
			}
		}
	}
	return false;
}


// ----------------------------------------------------------------------------
//		Log
// ----------------------------------------------------------------------------

void
CTargetAddress::Log(
	const CPeekStringA&	inPreAmble,
	CFileEx&			inFile ) const
{
#if (TOVERIFY)
	const CPeekStringA	strFormatA( FormatA() );

	const CPeekStringA*	ayStrs[] = {
		&inPreAmble,
		Tags::kxSpace.RefA(),
		&strFormatA,
		Tags::kxNewLine.RefA(),
		NULL
	};

	inFile.Write( FastCat( ayStrs ) );
#else
	CPeekStringA	str;
	str.Format( "%s %s\r\n", inPreAmble, FormatA() );
	inFile.Write( str );
#endif // TOVERIFY
}


// ----------------------------------------------------------------------------
//		Model
// ----------------------------------------------------------------------------

bool
CTargetAddress::Model(
	CPeekDataElement&	ioAddress )
{
	ioAddress.Enabled( m_bEnabled );
	ioAddress.Value( m_strAddressX );

#if (0)
	m_Type = (CRegExps::GetRegExps().IsAddress( m_strAddressX ))
		? kTypeAddress
		: kTypeDomain;
#else
	m_Type = kTypeAddress;
#endif

	return true;
}


// ----------------------------------------------------------------------------
//		Put
// ----------------------------------------------------------------------------

void
CTargetAddress::Put(
	BOOL			inCheck,
	CPeekStringX	inAddress )
{
	m_bEnabled = inCheck;
	m_strAddressX = inAddress;

#if (0)
	m_Type = (CRegExps::GetRegExps().IsAddress( m_strAddressX ))
		? kTypeAddress
		: kTypeDomain;
#else
	m_Type = kTypeAddress;
#endif
}


// ----------------------------------------------------------------------------
//		Validate
// ----------------------------------------------------------------------------

bool
CTargetAddress::Validate(
	const CPeekString&	inAddress )
{
#if (0)
	if ( CRegExps::GetRegExps().IsDomain( inAddress ) ) return true;
	if ( CRegExps::GetRegExps().IsAddress( inAddress ) ) return true;
#endif
	return false;
}


// ============================================================================
//		CTargetAddressList
// ============================================================================

CTargetAddressList::CTargetAddressList()
	 :	m_bCaptureAll( FALSE )
{
}


// ----------------------------------------------------------------------------
//		Contains
// ----------------------------------------------------------------------------

bool
CTargetAddressList::Contains(
	const CPeekString&	inStr ) const
{
	for ( size_t i = 0; i < size(); i++ ) {
		const CTargetAddress&	Address( at( i ) );
		if ( Address.IsMatch( inStr ) ) {
			return true;
		}
	}
	return false;
}


// ----------------------------------------------------------------------------
//		FormatLongA
// ----------------------------------------------------------------------------

CPeekStringA
CTargetAddressList::FormatLongA() const
{
	const size_t	nCount( size() );
	const size_t	nLineCount( 2 + (nCount * 2) );
	CFastCatA		fcAddress( nLineCount );
	std::vector<CPeekStringA>	ayAddress( nCount );

	if ( m_bCaptureAll ) {
		fcAddress.Add( Tags::kxCaptureAll.RefA() );
		fcAddress.Add( Tags::kxCommaSpace.RefA() );
	}

	for ( size_t i = 0; i < nCount; i++ ) {
		const CTargetAddress&	Address( at( i ) );
		ayAddress[i] = Address.FormatA();
		fcAddress.Add( &ayAddress[i] );
		fcAddress.Add( Tags::kxSemiSpace.RefA() );
	}

	_ASSERTE( fcAddress.size() <= nLineCount );
	CPeekStringA	strList = fcAddress.FastCat();
	strList.TrimRight( "; " );

	return strList;
}


// ----------------------------------------------------------------------------
//		Format
// ----------------------------------------------------------------------------

CPeekString
CTargetAddressList::Format() const
{
	CPeekString	strFormat;
	if ( m_bCaptureAll ) {
		return Tags::kxCaptureAll.GetW();
	}

	for ( size_t i = 0; i < size(); i++ ) {
		const CTargetAddress&	Address( at( i ) );
		if ( Address.IsEnabled() ) {
			if ( !strFormat.IsEmpty() ) {
				strFormat.Append( Tags::kxCommaSpace.GetW() );
			}
			strFormat.Append( Address.GetAddress() );
		}
	}

	return strFormat;
}


// ----------------------------------------------------------------------------
//		FormatCount
// ----------------------------------------------------------------------------

CPeekString
CTargetAddressList::FormatCount() const
{
	const size_t	nCount( size() );
	size_t			nAddressCount( 0 );
	if ( !m_bCaptureAll ) {
		for ( size_t i = 0; i < nCount; i++ ) {
			const CTargetAddress&	Address( at( i ) );
			if ( Address.IsEnabled() ) {
				nAddressCount++;
			}
		}
	}

	CPeekString	strCount;
	strCount.Format( L"%d", nCount );

	CPeekString		strAddressCount;
	strAddressCount.Format( L"%d", nAddressCount );

	const CPeekString*	ayStrs[] = {
		&strCount,
		Tags::kxSpace.RefW(),
		Tags::kxParenLeft.RefW(),
		&strAddressCount,
		Tags::kxParenRight.RefW(),
		NULL
	};

	return FastCat( ayStrs );
}


// ----------------------------------------------------------------------------
//		GetFirst
// ----------------------------------------------------------------------------

bool
CTargetAddressList::GetFirst(
	CTargetAddress&	outItem,
	size_t&			outPos ) const
{
	for ( size_t i = 0; i < size(); i++ ) {
		const CTargetAddress&	Address( at( i ) );
		if ( Address.IsEnabled() ) {
			if ( i < kMaxUInt32 ) {
				outItem = Address;
				outPos = i;
				return true;
			}
			break;
		}
	}
	return false;
}


// ----------------------------------------------------------------------------
//		GetNext
// ----------------------------------------------------------------------------

bool
CTargetAddressList::GetNext(
	CTargetAddress&	outItem,
	size_t&			ioPos ) const
{
	if ( ioPos > size() ) return false;

	for ( size_t i = (ioPos + 1); i < size(); i++ ) {
		const CTargetAddress&	Address( at( i ) );
		if ( Address.IsEnabled() ) {
			if ( i < kMaxUInt32 ) {
				outItem = Address;
				ioPos = i;
				return true;
			}
			break;
		}
	}
	return false;
}


// ----------------------------------------------------------------------------
//		HasAddress
// ----------------------------------------------------------------------------

bool
CTargetAddressList::HasAddress(
	const CPeekStringA&	inStr,
	bool&				outChecked ) const
{
	for ( size_t i = 0; i < size(); i++ ) {
		const CTargetAddress&	Address( at( i ) );
		if ( Address.CompareAddress( inStr ) ) {
			outChecked = Address.IsEnabled();
			return true;
		}
	}
	return false;
}


// ----------------------------------------------------------------------------
//		IsAddressOfInterest
// ----------------------------------------------------------------------------

bool
CTargetAddressList::IsAddressOfInterest(
	const CPeekString&	inStr ) const
{
	if ( m_bCaptureAll ) return true;

	for ( size_t i = 0; i < size(); i++ ) {
		const CTargetAddress&	Address( at( i ) );
		if ( Address.IsMatch( inStr ) ) {
			return true;
		}
	}
	return false;
}


// ----------------------------------------------------------------------------
//		IsMatch
// ----------------------------------------------------------------------------

bool
CTargetAddressList::IsMatch(
	const CTargetAddressList&	inAddressList ) const
{
	CPeekStringA	strValue( FormatLongA() );
	CPeekStringA	strInValue( inAddressList.FormatLongA() );
	return (strValue.CompareNoCase( strInValue ) == 0);
}


// ----------------------------------------------------------------------------
//		Log
// ----------------------------------------------------------------------------

void
CTargetAddressList::Log(
	const CPeekStringA&	inPreAmble,
	CFileEx&			inFile ) const
{
#if (TOVERIFY)
	{
		const CPeekStringA*	ayStrs[] = {
			&inPreAmble,
			&Tags::kLogAddressListA,
			NULL
		};
		CPeekStringA	strWrite( FastCat( ayStrs ) );
		inFile.Write( strWrite );
	}
#else
	CPeekStringA	str;
	str.Format( "%s Target Address List set to:\r\n", inPreAmble );
	inFile.Write( str );
#endif

	CPeekStringA	strPreAmble;
#if (TOVERIFY)
	{
		const CPeekStringA*	ayStrs[] = {
			&inPreAmble,
			Tags::kxSpaceDash.RefA(),
			NULL
		};
		strPreAmble = FastCat( ayStrs );
	}
#else
	strPreAmble.Format( "%s -", inPreAmble );
#endif

	const size_t	nCount( size() );
	for ( size_t i = 0; i < nCount; i++ ) {
		const CTargetAddress&	Address( at( i ) );
		Address.Log( strPreAmble, inFile );
	}
}


// ----------------------------------------------------------------------------
//		LogUpdate
// ----------------------------------------------------------------------------

void
CTargetAddressList::LogUpdate(
	const CPeekStringA&			inPreAmble,
	const CTargetAddressList&	inAddressList,
	CFileEx&					inFile ) const
{
	if ( !IsMatch( inAddressList ) ) {
		inAddressList.Log( inPreAmble, inFile );
	}
}


// ----------------------------------------------------------------------------
//		Model
// ----------------------------------------------------------------------------

bool
CTargetAddressList::Model(
	CPeekDataElement&	ioAddressList )
{
	ioAddressList.AttributeBOOL( ConfigTags::kCaptureAll, m_bCaptureAll );

	bool	bLoading = ioAddressList.IsLoading();
	UInt32	nCount = 0;

	if ( bLoading ) {
		clear();
		nCount = ioAddressList.GetChildCount( ConfigTags::kAddress );
	}
	else {
		nCount = GetCount32();
	}

	for ( UInt32 i = 0; i < nCount; i++ ) {
		CPeekDataElement	elemAddress( ConfigTags::kAddress, ioAddressList, i );
		if ( elemAddress.IsValid() ) {
			if ( bLoading ) {
				push_back( CTargetAddress() );
			}
			CTargetAddress&	cTarget = at( i );
			cTarget.Model( elemAddress );
		}
	}

	return true;
}

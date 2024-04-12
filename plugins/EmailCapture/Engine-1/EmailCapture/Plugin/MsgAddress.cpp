// ============================================================================
//	MsgAddress.cpp
//		implementation of the CMsgAddress class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "MsgAddress.h"
#include "GlobalTags.h"


// ============================================================================
//		Global Tags
// ============================================================================

namespace Tags
{
	static const CPeekStringX	kxAddressBegin( "\" <" );
};


// ============================================================================
//		CMsgAddress
// ============================================================================

CPeekStringA	CMsgAddress::m_strNoAddress( "null" );


// -----------------------------------------------------------------------------
//		GetTypeString
// -----------------------------------------------------------------------------

const CPeekString&
CMsgAddress::GetTypeString() const
{
	return Tags::kxTypesEml[ m_Type ].GetW();
}

const CPeekStringA&
CMsgAddress::GetTypeStringA() const
{
	return Tags::kxTypesEml[ m_Type ].GetA();
}


// ----------------------------------------------------------------------------
//		IsMatch
// ----------------------------------------------------------------------------

bool
CMsgAddress::IsMatch(
	const CPeekStringA&	inAddress ) const
{
	return (m_strAddress.CompareNoCase( inAddress ) == 0);
}


// ----------------------------------------------------------------------------
//		Format
// ----------------------------------------------------------------------------

CPeekString
CMsgAddress::Format() const
{
#if (TOVERIFY)
	if ( m_strName.IsEmpty() ) {
		const CPeekString*	ayStrs[] = {
			Tags::kxAngleLeft.RefW(),
			m_strAddress.RefW(),
			Tags::kxAngleRight.RefW(),
			NULL
		};

		return FastCat( ayStrs );
	}

	const CPeekString*	ayStrs[] = {
		Tags::kxDouble.RefW(),
		m_strName.RefW(),
		Tags::kxAddressBegin.RefW(),
		m_strAddress.RefW(),
		Tags::kxAngleRight.RefW(),
		NULL
	};
	return FastCat( ayStrs );
#else
	CPeekOutString	ssAddress;

	if ( !m_strName.IsEmpty() ) {
		ssAddress <<
			L"\"" <<
			m_strName.GetW() <<
			L"\" <" <<
			m_strAddress.GetW() <<
			L">";
	}
	else {
		ssAddress << L"<" << m_strAddress << L">";
	}
	return ssAddress;
#endif
}


// ----------------------------------------------------------------------------
//		FormatA
// ----------------------------------------------------------------------------

CPeekStringA
CMsgAddress::FormatA() const
{
	if ( m_strName.IsEmpty() ) {
		const CPeekStringA*	ayStrs[] = {
			Tags::kxAngleLeft.RefA(),
			m_strAddress.RefA(),
			Tags::kxAngleRight.RefA(),
			NULL
		};

		return FastCat( ayStrs );
	}

#if (TOVERIFY)
	const CPeekStringA*	ayStrs[] = {
		Tags::kxDouble.RefA(),
		m_strName.RefA(),
		Tags::kxAddressBegin.RefA(),
		m_strAddress.RefA(),
		Tags::kxAngleRight.RefA(),
		NULL
	};
	return FastCat( ayStrs );
#else
	CPeekOutStringA	ssAddress;

	if ( !m_strName.IsEmpty() ) {
		ssAddress <<
			"\"" <<
			m_strName.GetA() <<
			"\" <" <<
			m_strAddress.GetA() <<
			">";
	}
	else {
		ssAddress << "<" << m_strAddress << ">";
	}
	return ssAddress;
#endif // TOVERIFY
}


// ----------------------------------------------------------------------------
//		FormatLong
// ----------------------------------------------------------------------------

CPeekString
CMsgAddress::FormatLong() const
{
	if ( m_strName.IsEmpty() ) {
		const CPeekString*	ayStrs[] = {
			Tags::kxTypesTxt[m_Type].RefW(),
			Tags::kxSpace.RefW(),
			Tags::kxAngleLeft.RefW(),
			m_strAddress.RefW(),
			Tags::kxAngleRight.RefW(),
			NULL
		};
		return FastCat( ayStrs );
	}

#if (TOVERIFY)
	const CPeekString*	ayStrs[] = {
		Tags::kxTypesTxt[m_Type].RefW(),
		Tags::kxSpaceDouble.RefW(),
		m_strName.RefW(),
		Tags::kxAddressBegin.RefW(),
		m_strAddress.RefW(),
		Tags::kxAngleRight.RefW(),
		NULL
	};
	return FastCat( ayStrs );
#else
	CPeekOutString	ssAddress;
	if ( !m_strName.IsEmpty() ) {
		ssAddress <<
			Tags::kxTypesTxt[m_Type].GetW() <<
			L" <" <<
			m_strAddress.GetW() <<
			L">";
		return ssAddress;
	}
	ssAddress <<
		Tags::kxTypesTxt[m_Type].GetW() <<
		L" \"" <<
		m_strName.GetW() <<
		L"\" <" <<
		m_strAddress.GetW() <<
		L">";
	return ssAddress;
#endif // TOVERIFY
}

CPeekStringA
CMsgAddress::FormatLongA() const
{
#if (TOVERIFY)
	if ( m_strName.IsEmpty() ) {
		const CPeekStringA*	ayStrs[] = {
			Tags::kxTypesTxt[m_Type].RefA(),
			Tags::kxSpace.RefA(),
			Tags::kxAngleLeft.RefA(),
			m_strAddress.RefA(),
			Tags::kxAngleRight.RefA(),
			NULL
		};
		return FastCat( ayStrs );
	}

	const CPeekStringA*	ayStrs[] = {
		Tags::kxTypesTxt[m_Type].RefA(),
		Tags::kxSpaceDouble.RefA(),
		m_strName.RefA(),
		Tags::kxAddressBegin.RefA(),
		m_strAddress.RefA(),
		Tags::kxAngleRight.RefA(),
		NULL
	};
	return FastCat( ayStrs );
#else
	CPeekOutStringA	ssAddress;
	if ( !m_strName.IsEmpty() ) {
		CPeekOutStringA	ssAddress;
		ssAddress <<
			Tags::kTypesTxt[m_Type].GetA() <<
			" <" <<
			m_strAddress.GetA() <<
			">";
		return ssAddress;
	}
	ssAddress << 
		Tags::kTypesTxt[m_Type].GetA() <<
		" \"" <<
		m_strName.GetA() << 
		"\" <" <<
		m_strAddress.GetA() <<
		">";
	return ssAddress;
#endif
}


// ============================================================================
//		CMsgAddressList
// ============================================================================

// ----------------------------------------------------------------------------
//		Parse
// ----------------------------------------------------------------------------

size_t
CMsgAddressList::Parse(
	CMsgAddress::MsgAddressType	inType,
	const CMsgText&				inMsgHdr )
{
	RemoveAll();

	// find MAIL From: and RCPT TO: lines, parse out the Addresses
	// Add unique items to the list.
	switch ( inType ) {
	case CMsgAddress::kType_From:
		{
			CPeekString	strMailFrom = inMsgHdr.GetMailFrom();
			if ( !strMailFrom.IsEmpty() ) {
				size_t		nIndex( Add( strMailFrom ) );
				CMsgAddress	&maItem( GetAt( nIndex ) );
				maItem.SetType( CMsgAddress::kType_From );
				maItem.SetExplicit();
			}
		}
		break;

	case CMsgAddress::kType_To:
	case CMsgAddress::kType_CC:
	case CMsgAddress::kType_BCC:
		{
			CArrayStringA	strsRecipientList;
			if ( inMsgHdr.GetRecipientTo( strsRecipientList ) ) {
				const size_t	nCount( strsRecipientList.GetCount() );
				for ( size_t i( 0 ); i < nCount; i++ ) {
					size_t		nIndex( Add( strsRecipientList[i] ) );
					CMsgAddress	&maItem( GetAt( nIndex ) );
					maItem.SetExplicit();
				}
			}
		}
		break;
	}

	return GetCount();
}


// ----------------------------------------------------------------------------
//		ResolveName
// ----------------------------------------------------------------------------

void
CMsgAddressList::ResolveName(
	CMsgAddress&	inTarget,
	bool			bSetType,
	bool			bAddMissing )
{
	const size_t	nCount( GetCount() );
	size_t			nTargetCount( 0 );

	for ( size_t i = 0; i < nCount; i++ ) {
		CMsgAddress&	maItem( GetAt( i ) );
		if ( maItem.IsMatch( inTarget.GetAddress() ) ) {
			if ( bSetType ) maItem.SetType( inTarget.GetType() );
			maItem.SetName( inTarget.GetName() );
			nTargetCount++;
		}
	}
	if ( (nTargetCount == 0) && bAddMissing ) {
		Add( inTarget );
	}
}


// ----------------------------------------------------------------------------
//		FormatEml
// ----------------------------------------------------------------------------

CPeekStringA
CMsgAddressList::FormatEmlA(
	CMsgAddress::MsgAddressType	inType )
{
	const size_t	nCount( GetCount() );

	// Capture All Version
	std::vector<size_t>	ayIndexes;
	ayIndexes.reserve( nCount );
	for ( size_t i = 0; i < nCount; i++ ) {
		CMsgAddress&	maItem( GetAt( i ) );
		if ( maItem.GetType() == inType ) {
			ayIndexes.push_back( i );
		}
	}

	const size_t				nAddressCount( ayIndexes.size() );
	const size_t				nLineCount( 2 + (nAddressCount * 2) );
	CFastCatA					fcEml( nLineCount );
	std::vector<CPeekStringA>	ayAddress( nAddressCount );
	if ( nAddressCount > 0 ) {
		fcEml.Add( Tags::kxTypesEml[inType].RefA() );
		fcEml.Add( Tags::kxColonSpace.RefA() );

		for ( size_t i( 0 ); i < nAddressCount; i++ ) {
			CMsgAddress&	maItem( GetAt( ayIndexes[i] ) );
			ayAddress[i] = maItem.FormatA();
			fcEml.Add( &ayAddress[i] );
			fcEml.Add(
				((i + 1) < nAddressCount)
					? Tags::kxCommaSpace.RefA()
					: Tags::kxEmpty.RefA() );
		}
	}

	_ASSERTE( fcEml.GetCount() <= nLineCount );
	return fcEml.FastCat();
}


// ----------------------------------------------------------------------------
//		FormatTxt
// ----------------------------------------------------------------------------

CPeekString
CMsgAddressList::FormatTxt(
	CMsgAddress::MsgAddressType	inType )
{
	const size_t				nCount( GetCount() );
	std::vector<CPeekString>	ayAddress;
	CFastCat					cfAddressList( nCount );
	ayAddress.resize( nCount );
	for ( size_t i( 0 ); i < nCount; i++ ) {
		CMsgAddress&	maItem( GetAt( i ) );
		if ( maItem.GetType() == inType ) {
			if ( !cfAddressList.IsEmpty() ) {
				cfAddressList.Add( Tags::kxCommaSpace.RefW() );
			}

			CPeekString			strAddress( maItem.Format() );
			const CPeekString*	ayStrs[] = {
				Tags::kxTypesTxt[inType].RefW(),
				Tags::kxSpace.RefW(),
				&strAddress,
				NULL
			};
			ayAddress[i] = FastCat( ayStrs );
	
			cfAddressList.Add( &ayAddress[i] );
		}
	}

	return cfAddressList.FastCat();
}


// ----------------------------------------------------------------------------
//		FormatTxtA
// ----------------------------------------------------------------------------

CPeekStringA
CMsgAddressList::FormatTxtA(
	CMsgAddress::MsgAddressType	inType )
{
#if (TOVERIFY)
	const size_t				nCount( GetCount() );
	std::vector<CPeekStringA>	ayAddress;
	CFastCatA					cfAddressList( nCount );
	ayAddress.resize( nCount );
	for ( size_t i( 0 ); i < nCount; i++ ) {
		CMsgAddress&	maItem( GetAt( i ) );
		if ( maItem.GetType() == inType ) {
			if ( !cfAddressList.IsEmpty() ) {
				cfAddressList.Add( Tags::kxCommaSpace.RefA() );
			}

			CPeekStringA		strAddress( maItem.FormatA() );
			const CPeekStringA*	ayStrs[] = {
				Tags::kxTypesTxt[inType].RefA(),
				Tags::kxSpace.RefA(),
				&strAddress,
				NULL
			};
			ayAddress[i] = FastCat( ayStrs );
	
			cfAddressList.Add( &ayAddress[i] );
		}
	}

	return cfAddressList.FastCat();
#endif // TOVERIFY
}


// ----------------------------------------------------------------------------
//		Find
// ----------------------------------------------------------------------------

bool
CMsgAddressList::Find(
	const CTargetAddress&	inItem,
	size_t&					outIndex ) const
{
	const size_t	nCount( GetCount() );
	for ( size_t i( 0 ); i < nCount; i++ ) {
		if ( inItem.IsMatch( GetAt( i ).GetAddress() ) ) {
			outIndex = i;
			return true;
		}
	}
	return false;
}


// ----------------------------------------------------------------------------
//		Contains
// ----------------------------------------------------------------------------

bool
CMsgAddressList::Contains(
	const CTargetAddress&	inItem ) const
{
	const size_t	nCount( GetCount() );
	for ( size_t i( 0 ); i < nCount; i++ ) {
		if ( inItem.IsMatch( GetAt( i ).GetAddress() ) ) return true;
	}
	return false;
}

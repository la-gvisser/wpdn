// ============================================================================
//	MsgInfo.cpp
//		implementation of the CMsgInfo class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "MsgInfo.h"
#include "GlobalTags.h"
#include "MsgStream.h"
#include "mimepp.h"


// ============================================================================
//		CFieldNameMap
// ============================================================================

CMsgInfo::CFieldNameMap::CFieldNameMap()
{
	CPeekStringA	strOrigDate( "date" );
	CPeekStringA	strFrom( "from" );
	CPeekStringA	strMailFrom( "mail from" );
	CPeekStringA	strSender( "sender" );
	CPeekStringA	strReplyTo( "reply-to" );
	CPeekStringA	strTo( "to" );
	CPeekStringA	strRcptTo( "rcpt to" );
	CPeekStringA	strCC( "cc" );
	CPeekStringA	strBCC( "bcc" );
	CPeekStringA	strMessageId( "message-id" );
	CPeekStringA	strSubject( "subject" );
	CPeekStringA	strReceived( "received" );
	CPeekStringA	strReturnPath( "returnpath" );
	CPeekStringA	strComments( "comments" );
	CPeekStringA	strAttachments( "attachments" );


	(*this)[strOrigDate]	= CMsgInfo::CFieldName( kOrigDate, strOrigDate );
	(*this)[strMailFrom]	= CMsgInfo::CFieldName( kMailFrom, strMailFrom );
	(*this)[strRcptTo]		= CMsgInfo::CFieldName( kRcptTo, strRcptTo );
	(*this)[strFrom]		= CMsgInfo::CFieldName( kFrom, strFrom );
	(*this)[strSender]		= CMsgInfo::CFieldName( kSender, strSender );
	(*this)[strReplyTo]		= CMsgInfo::CFieldName( kReplyTo, strReplyTo );
	(*this)[strTo]			= CMsgInfo::CFieldName( kTo, strTo );
	(*this)[strCC]			= CMsgInfo::CFieldName( kCC, strCC );
	(*this)[strBCC]			= CMsgInfo::CFieldName( kBCC, strBCC );
	(*this)[strMessageId]	= CMsgInfo::CFieldName( kMessageId, strMessageId );
	(*this)[strSubject]		= CMsgInfo::CFieldName( kSubject, strSubject );
	(*this)[strReceived]	= CMsgInfo::CFieldName( kReceived, strReceived );
	(*this)[strReturnPath]	= CMsgInfo::CFieldName( kReturnPath, strReturnPath );
	(*this)[strComments]	= CMsgInfo::CFieldName( kComments, strComments );
	(*this)[strAttachments]	= CMsgInfo::CFieldName( kAttachments, strAttachments );
}

CMsgInfo::CFieldNameMap	CMsgInfo::m_FieldNameMap;


// ============================================================================
//		CMsgInfo
// ============================================================================

CMsgInfo::CMsgInfo(
	CMsgStream*		inMsgStream )
	:	m_pMsgStream( inMsgStream )
	,	m_Headers( mimepp::String( inMsgStream->GetPenMsg() ) )
	,	m_strProtocol( inMsgStream->GetProtocol() )
	,	m_addrProtocol( CMsgAddress::kType_From, inMsgStream->GetProtocol() )
{
	try {
		const CMsgText	strPenMsg( inMsgStream->GetPenMsg() );

		m_InterceptTime.SetNow();
		inMsgStream->GetIpAndPorts( m_IpPortPair );
		inMsgStream->GetMsgSizeInfo( m_SizeInfo );

		m_Headers.parse();
		
		m_MailFromList.Parse( CMsgAddress::kType_From, strPenMsg );
		m_RcptToList.Parse( CMsgAddress::kType_BCC, strPenMsg );

#if UNUSED_CODE
		{
			const mimepp::MailboxList&	mlFromList = hdrsFieldList.from();
			int		nMailboxCount = mlFromList.numMailboxes();
			for ( int nMailbox = 0; nMailbox < nMailboxCount; nMailbox++ ) {
				const mimepp::Mailbox&	mbAddress = mlFromList.mailboxAt( nMailbox );
				CPeekStringA	strName( mbAddress.displayNameUtf8().c_str() );
				CMsgAddress		maAddr( CMsgAddress::kType_From, FormatAddress( mbAddress ), strName );
				m_MailFromList.Add( maAddr );
			}
		}

		{
			const mimepp::AddressList&	alRcpts = hdrsFieldList.to();
			int		nAddressCount = alRcpts.numAddresses();
			for ( int nAddress = 0; nAddress < nAddressCount; ++nAddress ) {
				const mimepp::Address&	adrRecipient = alRcpts.addressAt( nAddress );
				if ( adrRecipient.class_().isA( mimepp::MAILBOX_CLASS ) ) {
					const mimepp::Mailbox&	mbAddress = (const mimepp::Mailbox&) adrRecipient;
					CPeekStringA	strName( mbAddress.displayNameUtf8().c_str() );
					CMsgAddress		maAddr(
						CMsgAddress::kType_To, FormatAddress( mbAddress ), strName );
					m_RcptToList.Add( maAddr );
				}
				else if ( adrRecipient.class_().isA( mimepp::GROUP_CLASS ) ) {
					const mimepp::Group&		group = (const mimepp::Group&) adrRecipient;
					const mimepp::MailboxList&	mlRcptList = group.mailboxList();
					int		nMailboxCount = mlRcptList.numMailboxes();
					for ( int nMailbox = 0; nMailbox < nMailboxCount; nMailbox++ ) {
						const mimepp::Mailbox&	mbAddress = mlRcptList.mailboxAt( nMailbox );
						CPeekStringA	strName( mbAddress.displayNameUtf8().c_str() );
						CMsgAddress		maAddr(	CMsgAddress::kType_To, FormatAddress( mbAddress ), strName );
						m_RcptToList.Add( maAddr );
					}
				}
			}
		}

		{
			const mimepp::DateTime&	dtReceived = hdrsFieldList.date();
			m_strDate = dtReceived.getString().c_str();
		}
#endif

		int	nFieldCount( m_Headers.numFields() );
		for ( int nField = 0; nField < nFieldCount; nField++ ) {
			const mimepp::Field&		fldField( m_Headers.fieldAt( nField ) );
			CPeekStringA				strFieldName( fldField.fieldName().c_str() );
			const mimepp::FieldBody&	fbBody( fldField.fieldBody() );
			const mimepp::Class			fbClass( fbBody.class_() );

			strFieldName.MakeLower();
			FieldTypes	nType( m_FieldNameMap[strFieldName].m_Type );

			switch ( nType ) {
			case kOrigDate:
				ASSERT( fbClass.isA( mimepp::DATE_TIME_CLASS ) );
				if ( fbClass.isA( mimepp::DATE_TIME_CLASS ) ) {
					const mimepp::DateTime&	dtReceived( static_cast<const mimepp::DateTime&>( fldField.fieldBody() ) );
					m_strDate = dtReceived.getString().c_str();
				}
				break;

#if UNUSED_CODE
			case kMailFrom:
				{
					const mimepp::FieldBody&	fbAddress = (const mimepp::FieldBody&) fldField.fieldBody();
					const mimepp::String&	stAddress = fbAddress.text();
					if ( stAddress.length() ) {
						CPeekStringA	strAddress( stAddress.c_str() );
						CMsgAddress		maAddr( CMsgAddress::kType_From, strAddress );
						m_MailFromList.ResolveName( maAddr, false, true );
					}
				}
				break;

			case kRcptTo:
				{
					const mimepp::FieldBody&	fbAddress = (const mimepp::FieldBody&) fldField.fieldBody();
					const mimepp::String&	stAddress = fbAddress.text();
					if ( stAddress.length() ) {
						CPeekStringA	strAddress( stAddress.c_str() );
						CMsgAddress		maAddr( CMsgAddress::kType_BCC, strAddress );
						m_RcptToList.ResolveName( maAddr, false, true );
					}
				}
				break;
#endif

			case kFrom:
			case kSender:
				if ( fbClass.isA( mimepp::MAILBOX_CLASS ) ) {
					const mimepp::Mailbox&	mbAddress( static_cast<const mimepp::Mailbox&>( fbBody ) );
					CPeekStringA			strName( mbAddress.displayNameUtf8().c_str() );
					CMsgAddress				maAddr( CMsgAddress::kType_From, FormatAddressA( mbAddress ), strName );
					m_MailFromList.ResolveName( maAddr, true, true );
					m_RcptToList.ResolveName( maAddr, false, false );
				}
				else if ( fbClass.isA( mimepp::MAILBOX_LIST_CLASS ) ) {
					const mimepp::MailboxList&	mlFromList( static_cast<const mimepp::MailboxList&>( fldField.fieldBody() ) );
					int							nMailboxCount( mlFromList.numMailboxes() );
					ASSERT( nMailboxCount <= 10000 );
					if ( nMailboxCount > 10000 ) continue;	// sanity check.

					for ( int nMailbox = 0; nMailbox < nMailboxCount; nMailbox++ ) {
						const mimepp::Mailbox&	mbAddress( mlFromList.mailboxAt( nMailbox ) );
						CPeekStringA			strName( mbAddress.displayNameUtf8().c_str() );
						CMsgAddress				maAddr( CMsgAddress::kType_From, FormatAddressA( mbAddress ), strName );
						m_MailFromList.ResolveName( maAddr, true, true );
						m_RcptToList.ResolveName( maAddr, false, false );
					}
				}
				else {
					ASSERT( 0 );
				}
				break;

			case kTo:
				ASSERT( fbClass.isA( mimepp::ADDRESS_LIST_CLASS ) );
				if ( fbClass.isA( mimepp::ADDRESS_LIST_CLASS ) ) {
					const mimepp::AddressList&	alRcpts( static_cast<const mimepp::AddressList&>( fldField.fieldBody() ) );
					int							nAddressCount( alRcpts.numAddresses() );
					ASSERT( nAddressCount <= 10000 );
					if ( nAddressCount > 10000 ) continue;	// sanity check.

					for ( int nAddress = 0; nAddress < nAddressCount; ++nAddress ) {
						const mimepp::Address&	adrRecipient( alRcpts.addressAt( nAddress ) );
						if ( adrRecipient.class_().isA( mimepp::MAILBOX_CLASS ) ) {
							const mimepp::Mailbox&	mbAddress( static_cast<const mimepp::Mailbox&>( adrRecipient ) );
							CPeekStringA			strName( mbAddress.displayNameUtf8().c_str() );
							CMsgAddress				maAddr( CMsgAddress::kType_To, FormatAddressA( mbAddress ), strName );
							m_RcptToList.ResolveName( maAddr, true, true );
						}
						else if ( adrRecipient.class_().isA( mimepp::GROUP_CLASS ) ) {
							const mimepp::Group&		group( static_cast<const mimepp::Group&>( adrRecipient ) );
							const mimepp::MailboxList&	mlRcptList( group.mailboxList() );
							int							nMailboxCount( mlRcptList.numMailboxes() );
							for ( int nMailbox = 0; nMailbox < nMailboxCount; nMailbox++ ) {
								const mimepp::Mailbox&	mbAddress( mlRcptList.mailboxAt( nMailbox ) );
								CPeekStringA			strName( mbAddress.displayNameUtf8().c_str() );
								CMsgAddress				maAddr( CMsgAddress::kType_To, FormatAddressA( mbAddress ), strName );
								m_RcptToList.ResolveName( maAddr, true, true );
							}
						}
					}
				}
				break;

			case kCC:
				ASSERT( fbClass.isA( mimepp::ADDRESS_LIST_CLASS ) );
				if ( fbClass.isA( mimepp::ADDRESS_LIST_CLASS ) ) {
					const mimepp::AddressList&	rcptList( static_cast<const mimepp::AddressList&>( fldField.fieldBody() ) );
					int							nAddressCount( rcptList.numAddresses() );
					ASSERT( nAddressCount <= 10000 );
					if ( nAddressCount > 10000 ) continue;	// sanity check.

					for ( int nAddress = 0; nAddress < nAddressCount; ++nAddress ) {
						const mimepp::Address&	adrRecipient( rcptList.addressAt( nAddress ) );
						if ( adrRecipient.class_().isA( mimepp::MAILBOX_CLASS ) ) {
							const mimepp::Mailbox&	mbAddress( static_cast<const mimepp::Mailbox&>( adrRecipient ) );
							CPeekStringA			strName( mbAddress.displayNameUtf8().c_str() );
							CMsgAddress				maAddr( CMsgAddress::kType_CC, FormatAddressA( mbAddress ), strName );
							m_RcptToList.ResolveName( maAddr, true, true );
						}
						else if ( adrRecipient.class_().isA( mimepp::GROUP_CLASS ) ) {
							const mimepp::Group&		group( static_cast<const mimepp::Group&>( adrRecipient ) );
							const mimepp::MailboxList&	mlRcptList( group.mailboxList() );
							int							nMailboxCount( mlRcptList.numMailboxes() );
							for ( int nMailbox = 0; nMailbox < nMailboxCount; nMailbox++ ) {
								const mimepp::Mailbox&	mbAddress( mlRcptList.mailboxAt( nMailbox ) );
								CPeekStringA			strName( mbAddress.displayNameUtf8().c_str() );
								CMsgAddress				maAddr( CMsgAddress::kType_CC, FormatAddressA( mbAddress ), strName );
								m_RcptToList.ResolveName( maAddr, true, true );
							}
						}
					}
				}
				break;

			case kBCC:
				ASSERT( fbClass.isA( mimepp::ADDRESS_LIST_CLASS ) );
				if ( fbClass.isA( mimepp::ADDRESS_LIST_CLASS ) ) {
					const mimepp::AddressList&	rcptList( static_cast<const mimepp::AddressList&>( fldField.fieldBody() ) );
					int							nAddressCount( rcptList.numAddresses() );
					ASSERT( nAddressCount <= 10000 );
					if ( nAddressCount > 10000 ) continue;	// sanity check.

					for ( int nAddress = 0; nAddress < nAddressCount; ++nAddress ) {
						const mimepp::Address&	adrRecipient( rcptList.addressAt( nAddress ) );
						if ( adrRecipient.class_().isA( mimepp::MAILBOX_CLASS ) ) {
							const mimepp::Mailbox&	mbAddress( static_cast<const mimepp::Mailbox&>( adrRecipient ) );
							CPeekStringA			strName( mbAddress.displayNameUtf8().c_str() );
							CMsgAddress				maAddr( CMsgAddress::kType_BCC, FormatAddressA( mbAddress ), strName );
							m_RcptToList.ResolveName( maAddr, true, true );
						}
						else if ( adrRecipient.class_().isA( mimepp::GROUP_CLASS ) ) {
							const mimepp::Group&		group( static_cast<const mimepp::Group&>( adrRecipient ) );
							const mimepp::MailboxList&	mlRcptList( group.mailboxList() );
							int		nMailboxCount = mlRcptList.numMailboxes();
							for ( int nMailbox = 0; nMailbox < nMailboxCount; nMailbox++ ) {
								const mimepp::Mailbox&	mbAddress( mlRcptList.mailboxAt( nMailbox ) );
								CPeekStringA			strName( mbAddress.displayNameUtf8().c_str() );
								CMsgAddress				maAddr( CMsgAddress::kType_BCC, FormatAddressA( mbAddress ), strName );
								m_RcptToList.ResolveName( maAddr, true, true );
							}
						}
					}
				}
				break;

			case kUndefined:
			case kMailFrom:
			case kRcptTo:
			case kReplyTo:
			case kMessageId:
			case kSubject:
			case kReceived:
			case kReturnPath:
			case kComments:
			case kAttachments:
			case kOther:
			case kNonField:
			case kEndOfHeaders:
			default:
				break;
			}
		}

		if ( m_strDate.IsEmpty() ) {
			m_strDate = strPenMsg.GetDate();
		}
	}
	catch ( ... ) {
		;
	}
}


// ----------------------------------------------------------------------------
//		FormatAddressA			[static]
// ----------------------------------------------------------------------------

CPeekStringA
CMsgInfo::FormatAddressA(
	const mimepp::Mailbox&	inAddress )
{
	CPeekStringA	strLocalPart( inAddress.localPart().c_str() );
	CPeekStringA	strDomain( inAddress.domain().c_str() );

	const CPeekStringA*	ayStrs[] = {
		&strLocalPart,
		Tags::kxAt.RefA(),
		&strDomain,
		NULL
	};

	CPeekStringA	strAddress( FastCat( ayStrs ));
	return strAddress;
}


// ----------------------------------------------------------------------------
//		GetFirstSender
// ----------------------------------------------------------------------------

const CMsgAddress*
CMsgInfo::GetFirstSender() const
{
	if ( m_MailFromList.empty() ) {
		return NULL;
	}
	return &m_MailFromList[0];
}


// ----------------------------------------------------------------------------
//		GetFirstSenderAddress
// ----------------------------------------------------------------------------

CPeekString
CMsgInfo::GetFirstSenderAddress() const
{
	CPeekString			strAddress;
	const CMsgAddress*	pFirstSender( GetFirstSender() );
	if ( pFirstSender != NULL ) {
		strAddress = pFirstSender->GetAddress().GetW();
	}
	return strAddress;
}


// ----------------------------------------------------------------------------
//		FormatAddressList
// ----------------------------------------------------------------------------

CPeekString
CMsgInfo::FormatAddressList()
{
	const size_t	nLineCount( 7 );
	CFastCat		cfAddressList( nLineCount );

	// From:
	CPeekString strFromList( m_MailFromList.FormatTxt( CMsgAddress::kType_From ) );
	cfAddressList.Add( &strFromList );

	// To:
	CPeekString strToList( m_RcptToList.FormatTxt( CMsgAddress::kType_To ) );
	if ( !strToList.IsEmpty() ) {
		cfAddressList.Add( Tags::kxCommaSpace.RefW() );
		cfAddressList.Add( &strToList );
	}

	// CC:
	CPeekString	strCCList( m_RcptToList.FormatTxt( CMsgAddress::kType_CC ) );
	if ( !strCCList.IsEmpty() ) {
		cfAddressList.Add( Tags::kxCommaSpace.RefW() );
		cfAddressList.Add( &strCCList );
	}

	// BCC:
	CPeekString	strBCCList( m_RcptToList.FormatTxt( CMsgAddress::kType_BCC ) );
	if ( !strBCCList.IsEmpty() ) {
		cfAddressList.Add( Tags::kxCommaSpace.RefW() );
		cfAddressList.Add( &strBCCList );
	}

	// Convert the vector of strings into one big string.
	_ASSERTE( cfAddressList.GetCount() <= nLineCount );
	return cfAddressList.FastCat();
}


// ----------------------------------------------------------------------------
//		FormatTarget
// ----------------------------------------------------------------------------

CPeekString
CMsgInfo::FormatTarget(
	const CMsgAddress&	inTarget )
{
	// From:
	CPeekString	strFrom( m_MailFromList.FormatTxtA( CMsgAddress::kType_From ) );

	// To:
	// CC:
	// BCC:
	CPeekString	strRecipient( inTarget.FormatLongA() );

#if (TOVERIFY)
	const CPeekString*	ayStrs[] = {
		&strFrom,
		Tags::kxSemiSpace.RefW(),
		&strRecipient,
		NULL };
	return FastCat( ayStrs );
#else
	CPeekString		strTarget;
	strTarget.Format( L"%s; %s", strFrom, strRecipient );
	return strTarget;
#endif // TOVERIFY
}


// ----------------------------------------------------------------------------
//		AddFormattedAddress
// ----------------------------------------------------------------------------

void
CMsgInfo::AddFormattedAddress(
	CArrayString&	outAddresses )
{
	CPeekString	strAddressList( FormatAddressList() );
	outAddresses.push_back( strAddressList );
}

void
CMsgInfo::AddFormattedAddress(
	const CMsgAddress&	inTarget,
	CArrayString&		outAddresses )
{
	CPeekString	strTarget( FormatTarget( inTarget ) );
	outAddresses.push_back( strTarget );
}


// ----------------------------------------------------------------------------
//		FormatEmlHeaderA
// ----------------------------------------------------------------------------

CPeekStringA
CMsgInfo::FormatEmlHeaderA()
{
	size_t			nFromCount = m_MailFromList.size();
	size_t			nToCount = m_RcptToList.size();
	const size_t	nLineCount = 8 + nFromCount + nToCount;
	CFastCatA		cfHeaders( nLineCount );

	// From:
	CPeekStringA	strFromList( m_MailFromList.FormatEmlA( CMsgAddress::kType_From ) );
	if ( !strFromList.IsEmpty() ) {
		cfHeaders.push_back( &strFromList );
		cfHeaders.push_back( Tags::kxNewLine.RefA() );
	}

	// To:
	CPeekStringA	strToList( m_RcptToList.FormatEmlA( CMsgAddress::kType_To ) );
	if ( !strToList.IsEmpty() ) {
		cfHeaders.push_back( &strToList );
		cfHeaders.push_back( Tags::kxNewLine.RefA() );
	}

	// CC:
	CPeekStringA	strCCList( m_RcptToList.FormatEmlA( CMsgAddress::kType_CC ) );
	if ( !strCCList.IsEmpty() ) {
		cfHeaders.push_back( &strCCList );
		cfHeaders.push_back( Tags::kxNewLine.RefA() );
	}

	// BCC:
	CPeekStringA	strBCCList( m_RcptToList.FormatEmlA( CMsgAddress::kType_BCC ) );
	if ( !strBCCList.IsEmpty() ) {
		cfHeaders.push_back( &strBCCList );
		cfHeaders.push_back( Tags::kxNewLine.RefA() );
	}

	// X-MAIL-FROM
	CPeekStringA	strMailFrom;
	for ( size_t i = 0; i < nFromCount; i++ ) {
		if ( m_MailFromList[i].IsExplicit() ) {
			const CPeekStringA*	ayFromStrs[] = {
				Tags::kxXMailFrom.RefA(),
				&m_MailFromList[i].GetAddress().GetA(),
				Tags::kxNewLine.RefA(),
				NULL
			};
			strMailFrom = FastCat( ayFromStrs );
			cfHeaders.push_back( &strMailFrom );
		}
	}

	// X-RCPT-TO
	CPeekStringA	strRcptTo;
	for ( size_t i = 0; i < nToCount; i++ ) {
		if ( m_RcptToList[i].IsExplicit() ) {
			const CPeekStringA*	ayToStrs[] = {
				Tags::kxXRcptTo.RefA(),
				m_RcptToList[i].GetAddress().RefA(),
				Tags::kxNewLine.RefA(),
				NULL
			};
			strRcptTo = FastCat( ayToStrs );
			cfHeaders.push_back( &strRcptTo );
		}
	}

	// Convert the Array of strings into one big string.
	_ASSERTE( cfHeaders.GetCount() <= nLineCount );
	return cfHeaders.FastCat();
}


// ----------------------------------------------------------------------------
//		FormatEmlHeaderA
// ----------------------------------------------------------------------------

CPeekStringA
CMsgInfo::FormatEmlHeaderA(
	const CTargetAddress&	inTarget )
{
	// If target is in the From List, then get all the recipients.
	if ( m_MailFromList.Contains( inTarget ) ) {
		return FormatEmlHeaderA();
	}

	// Check the Receipt To list, if its not there, then return.
	if ( !m_RcptToList.Contains( inTarget ) ) return Tags::kxEmpty.GetA();

	const size_t	nToCount = m_RcptToList.size();
	const size_t	nFromCount = m_MailFromList.size();
	const size_t	nHeaderCount = 3 + (nToCount * 2) + nFromCount;
	CFastCatA		cfHeaders( nHeaderCount );

	// Target is not in the From List, build the From clause.
	CPeekStringA	strFromList( m_MailFromList.FormatEmlA( CMsgAddress::kType_From ) );
	if ( !strFromList.IsEmpty() ) {
		cfHeaders.push_back( &strFromList );
		cfHeaders.push_back( Tags::kxNewLine.RefA() );
	}

	CPeekStringA				strTarget;
	std::vector<CPeekStringA>	ayTarget;
	if ( inTarget.GetType() == CTargetAddress::kTypeAddress ) {
		size_t	nRecipient( kIndex_Invalid );
		if ( !m_RcptToList.Find( inTarget, nRecipient ) ) return Tags::kxEmpty.GetA();

		const CMsgAddress&	RcptTo = m_RcptToList.at( nRecipient );
		//CPeekStringA		strAddress = RcptTo.FormatA();
#if (TOVERIFY)
		CPeekStringA		strRcptTo = RcptTo.GetTypeStringA();
		const CPeekStringA*	ayToStrs[] = {
			&strRcptTo,
			Tags::kxColonSpace.RefA(),
			RcptTo.GetAddress().RefA(),
			Tags::kxNewLine.RefA(),
			NULL
		};
		strTarget = FastCat( ayToStrs );
		cfHeaders.push_back( &strTarget );
#else
		str.Format( "%s: %s\r\n", RcptTo.GetTypeStringA(), strAddress );
		aStrs.Add( str );
#endif // TOVERIFY
	}
	else {
		// Target is a domain: kTypeDomain
		// Add all the recipients from the domain
		ayTarget.resize( nToCount );
		for ( size_t i = 0; i < nToCount; i++ ) {
			const CMsgAddress&	RcptTo = m_RcptToList.at( i );
			if ( inTarget.IsMatch( RcptTo.GetAddress() ) ) {
				CPeekStringA	strAddress = RcptTo.FormatA();
#if (TOVERIFY)
				CPeekStringA		strRcptTo = RcptTo.GetTypeStringA();
				const CPeekStringA*	ayToStrs[] = {
					&strRcptTo,
					Tags::kxColonSpace.RefA(),
					&strAddress,
					Tags::kxNewLine.RefA(),
					NULL
				};
				ayTarget[i] = FastCat( ayToStrs );
				cfHeaders.push_back( &ayTarget[i] );
#else
				str.Format( "%s: %s\r\n", RcptTo.GetTypeStringA(), strAddress );
				aStrs.Add( str );
#endif // TOVERIFY
			}
		}
	}

	// X-MAIL-FROM
	std::vector<CPeekStringA>	ayMailFrom( nFromCount );
	for ( size_t i = 0; i < nFromCount; i++ ) {
		if ( m_MailFromList[i].IsExplicit() ) {
#if (TOVERIFY)
			const CPeekStringA*	ayFromStrs[] = {
				Tags::kxXMailFrom.RefA(),
				&m_MailFromList[i].GetAddress().GetA(),
				Tags::kxNewLine.RefA(),
				NULL
			};
			ayMailFrom[i] = FastCat( ayFromStrs );
			cfHeaders.push_back( &ayMailFrom[i] );
#else
			str.Format( "X-MAIL-FROM: %s\r\n", m_MailFromList[i].GetAddress().GetA() );
			aStrs.Add( str );
#endif // TOVERIFY
		}
	}

	// X-RCPT-TO
	std::vector<CPeekStringA>	ayRcptTo( nToCount );
	for ( size_t i = 0; i < nToCount; i++ ) {
		const CPeekStringX&	strAddress( m_RcptToList[i].GetAddress() );
		if ( m_RcptToList[i].IsExplicit() && inTarget.IsMatch( strAddress ) ) {
#if (TOVERIFY)
			const CPeekStringA*	ayToStrs[] = {
				Tags::kxXRcptTo.RefA(),
				strAddress.RefA(),
				Tags::kxNewLine.RefA(),
				NULL
			};
			ayRcptTo[i] = FastCat( ayToStrs );
			cfHeaders.push_back( &ayRcptTo[i] );
#else
			str.Format( "X-RCPT-TO: %s\r\n", m_RcptToList[i].GetAddress().GetA() );
			aStrs.Add( str );
#endif // TOVERIFY
		}
	}

	// Convert the Array of strings into one big string.
	_ASSERTE( cfHeaders.GetCount() <= nHeaderCount );
	return cfHeaders.FastCat();
}


// ----------------------------------------------------------------------------
//		FormatTxtHeaderA
// ----------------------------------------------------------------------------

CPeekStringA
CMsgInfo::FormatTxtHeaderA()
{
	size_t			nIndex( 0 );
	size_t			nRecipientIndex( 0 );

	const size_t	nLineCount = 8;
	CFastCatA		fcHeader( nLineCount );

	// From:
	CPeekStringA	strFromList( m_MailFromList.FormatTxtA( CMsgAddress::kType_From ) );
	fcHeader.push_back( &strFromList );
	fcHeader.push_back( Tags::kxTab.RefA() );
	nRecipientIndex = fcHeader.size();

	// To:
	// CC:
	// BCC:
	CPeekStringA	strToList( m_RcptToList.FormatTxtA( CMsgAddress::kType_To ) );
	if ( !strToList.IsEmpty() ) {
		fcHeader.push_back( &strToList );
		nIndex = fcHeader.size();
	}
	CPeekStringA	strCCList = m_RcptToList.FormatTxtA( CMsgAddress::kType_CC );
	if ( !strCCList.IsEmpty() ) {
		if ( nIndex > nRecipientIndex ) {
			fcHeader.push_back( Tags::kxCommaSpace.RefA() );
		}
		fcHeader.push_back( &strCCList );
		nIndex = fcHeader.size();
	}
	CPeekStringA	strBCCList = m_RcptToList.FormatTxtA( CMsgAddress::kType_BCC );
	if ( !strBCCList.IsEmpty() ) {
		if ( nIndex > nRecipientIndex ) {
			fcHeader.push_back( Tags::kxCommaSpace.RefA() );
		}
		fcHeader.push_back( &strBCCList );
		nIndex = fcHeader.size();
	}
	fcHeader.push_back( Tags::kxTab.RefA() );

	// Convert the Array of strings into one big string.
	_ASSERTE( fcHeader.GetCount() <= nLineCount );
	return fcHeader.FastCat();
}

CPeekStringA
CMsgInfo::FormatTxtHeaderA(
	const CMsgAddress&	inTarget )
{
	const size_t	nLineCount = 4;
	CFastCatA		fcHeader( nLineCount );

	// From:
	CPeekStringA	strFromList = m_MailFromList.FormatTxtA( CMsgAddress::kType_From );
	fcHeader.Add( &strFromList );
	fcHeader.Add( Tags::kxTab.RefA() );

	// To:
	// CC:
	// BCC:
	CPeekStringA	strTarget = inTarget.FormatLongA();
	fcHeader.Add( &strTarget );
	fcHeader.Add( Tags::kxTab.RefA() );

	// Convert the Array of strings into one big string.
	_ASSERTE( fcHeader.GetCount() <= nLineCount );
	return fcHeader.FastCat();
}


// ----------------------------------------------------------------------------
//		WriteFullEml
// ----------------------------------------------------------------------------

void
CMsgInfo::WriteFullEml(
	const CPeekString&	inFileName,
	CArrayString&		outTargets )
{
	if ( m_pMsgStream->WritePayload( inFileName ) ) {
		AddFormattedAddress( outTargets );
	}
}

bool
CMsgInfo::WriteFullEml(
	const CTargetAddress&	inTargetAddress,
	const CPeekString&		inFileName )
{
	if ( HasTarget( inTargetAddress ) ) {
		return m_pMsgStream->WritePayload( inFileName );
	}

	return false;
}

bool
CMsgInfo::WriteFullEml(
	const CTargetAddress&	inTargetAddress,
	CFileEx&				inFile )
{
	if ( HasTarget( inTargetAddress ) ) {
		return m_pMsgStream->WritePayload( inFile );
	}

	return false;
}


// ----------------------------------------------------------------------------
//		WriteFullPkt
// ----------------------------------------------------------------------------

bool
CMsgInfo::WriteFullPkt(
	const CTargetAddress&	inTargetAddress,
	const CPeekString&		inFileName )
{
	if ( HasTarget( inTargetAddress ) ) {
		return m_pMsgStream->WritePackets( inFileName );
	}
	return false;
}

// ============================================================================
//	PenInfo.cpp
//		implementation of the CPenInfo class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008-1011. All rights reserved.

#include "StdAfx.h"
#include "GlobalTags.h"
#include "PenInfo.h"
#include "MsgStream.h"
#include "Case.h"
#include "TargetAddress.h"
#include "FileEx.h"


// ============================================================================
//		Global Tags
// ============================================================================

namespace Tags
{
	const CPeekStringA	kHeadingsA =
		"Date Sent\tDate Intercepted\tProtocol\tSender\tRecipients\tClient IP\tMail Server IP\t"
		"Mail Server Port\tMail Size\tMessage-ID\tReturn-Path\tReply-To\tX-Originating-IP\t"
		"X-Apparently-To\tX-Apparently-From\tReceived"
		"\r\n";
	const CPeekString	kPenEmails( L"PenEmails" );
	const CPeekString	kEmail( L"Email" );

	const CPeekString	kAddress( L"Address" );
	const CPeekString	kContact( L"Contact" );
	const CPeekString	kDateIntercepted( L"DateIntercepted" );
	const CPeekString	kDateSent( L"DateSent" );
	const CPeekString	kEmailClient( L"EmailClient" );
	const CPeekString	kIpAddress( L"IpAddress" );
	const CPeekString	kList( L"List" );
	const CPeekString	kMailServer( L"MailServer" );
	const CPeekString	kName( L"Name" );
	const CPeekString	kPort( L"Port" );
	const CPeekString	kProtocol( L"Protocol" );
	const CPeekString	kReceived( L"Received" );
	const CPeekString	kReceivedList( L"ReceivedList" );
	const CPeekString	kRecipientList( L"RecipientList" );
	const CPeekString	kReplyTo( L"ReplayTo" );
	const CPeekString	kSize( L"Size" );
	const CPeekString	kSenderList( L"SenderList" );

	const CPeekStringX	kxDate( "Date" );
	const CPeekStringX	kxReplyTo( "Reply-To" );
	const CPeekStringX	kxReturnPath( "Return-Path" );
	const CPeekStringX	kxXApparentlyFrom( "X-Apparently-From" );
	const CPeekStringX	kxXApparentlyTo( "X-Apparently-To" );
	const CPeekStringX	kxXOriginatingIp( "X-Originating-IP" );
	const CPeekStringX	kxXDateIntercepted( "X-Date-Intercepted" );
	const CPeekStringX	kxXMailProtocol( "X-Mail-Protocol" );
	const CPeekStringX	kxXMailClientIp( "X-MailClient-IP" );
	const CPeekStringX	kxXMailServerIp( "X-MailServer-IP" );
	const CPeekStringX	kxXMailServerPort( "X-MailServer-Port" );
	const CPeekStringX	kxXMailSize( "X-Mail-Size" );


	const CPeekStringA	kReceivedFromLabelA( "Received: from " );
	const CPeekStringA	kReceivedLabelA( "Received: " );
	const CPeekStringA	kClosePenEmailsA( "</PenEmails>\r\n" );
}


// ============================================================================
//		Global XmlTags
// ============================================================================

namespace XmlTags
{
	const CPeekString	kReturnPath( L"ReturnPath" );
}


// ============================================================================
//		CPenInfo
// ============================================================================

CPenInfo::CPenInfo(
	CMsgStream*	inMsgStream )
	:	CMsgInfo( inMsgStream )
{
	try {
		int	nFieldCount( m_Headers.numFields() );
		for ( int nField = 0; nField < nFieldCount; nField++ ) {
			const mimepp::Field&		fldField( m_Headers.fieldAt( nField ) );
			CPeekStringA				strFieldName( fldField.fieldName().c_str() );
			const mimepp::FieldBody&	fbBody( fldField.fieldBody() );
			const mimepp::Class			fbClass( fbBody.class_() );

			strFieldName.MakeLower();

			FieldTypes	nType( m_FieldNameMap[strFieldName].m_Type );

			// All ways put mimepp::Strings from Fields into a CPeekStringA. This avoids the error of
			//  passing a mimepp::String into the printf class of functions.
			switch ( nType ) {
			case kMessageId:
				ASSERT( fbClass.isA( mimepp::MSG_ID_CLASS ) );
				if ( fbClass.isA( mimepp::MSG_ID_CLASS ) ) {
					const mimepp::MsgId&	MsgId( static_cast<const mimepp::MsgId&>( fldField.fieldBody() ) );
					CPeekStringA			strLocal( MsgId.localPart().c_str() );
					CPeekStringA			strDomain( MsgId.domain().c_str() );

					const CPeekStringA*	ayStrs[] = {
						Tags::kxAngleLeft.RefA(),
						&strLocal,
						Tags::kxAt.RefA(),
						&strDomain,
						Tags::kxAngleRight.RefA(),
						NULL
					};
					m_strMessageId = FastCat( ayStrs );
				}
				break;

			case kReturnPath:
				m_strReturnPath = fldField.fieldBody().getString().c_str();
				break;

			case kReplyTo:
				{
					CPeekStringA	strReplyTo( fldField.fieldBody().getString().c_str() );
					CMsgAddress		maReplyTo( CMsgAddress::kType_From, strReplyTo );
					m_strReplyTo = maReplyTo.FormatA();
				}
				break;

			case kReceived:
				ASSERT( fbClass.isA( mimepp::STAMP_CLASS ) );
				if ( fbClass.isA( mimepp::STAMP_CLASS ) ) {
					mimepp::Stamp&	stReceived( static_cast<mimepp::Stamp&>( fldField.fieldBody() ) );
					CPeekStringA	strReceived( stReceived.getString().c_str() );
					strReceived.Remove( '\t' );
					size_t	nOffset = strReceived.rfind( '\r' );
					while ( nOffset != std::string::npos ) {
						if ( strReceived[nOffset + 1] == '\n' ) {
							strReceived.replace( nOffset, 2, " " );
						}
						nOffset = strReceived.rfind( '\r', nOffset );
					}
					strReceived.Trim();

					size_t	nEnd = strReceived.rfind( "  " );
					while ( nEnd != std::string::npos ) {
						size_t	nBegin = nEnd;
						nEnd += 2;
						while ( strReceived[nBegin - 1] == ' ' ) {
							--nBegin;
							if ( nBegin == 0 ) break;
						}
						strReceived.replace( nBegin, (nEnd - nBegin), " " );
						nEnd = strReceived.rfind( "  " );
					}

#ifdef ENCODE_RECEIVE
					//Received: from xyz.net by mx.org via TELENET with SMTP
					//	id M12345 for smith@aol.com ; Tue, 6 Oct 1998 09:23:59 -0700

					if ( stReceived.from().c_str() ) {
						CPeekStringA	str( stReceived.from().c_str() );
						strReceived.AppendFormat( " from %s", str );
					}
					if ( stReceived.by().c_str() ) {
						CPeekStringA	str( stReceived.by().c_str() );
						strReceived.AppendFormat( " by %s", str );
					}
					if ( stReceived.via().c_str() ) {
						CPeekStringA	str( stReceived.via().c_str() );
						strReceived.AppendFormat( " via %s", str );
					}
					if ( stReceived.with().c_str() ) {
						CPeekStringA	str( stReceived.with().c_str() );
						strReceived.AppendFormat( " with %s", str  );
					}
					if ( stReceived.id().c_str() ) {
						CPeekStringA	str( stReceived.id().c_str() );
						strReceived.AppendFormat( " id %s", str );
					}
					if ( stReceived.for_().c_str() ) {
						CPeekStringA	str( stReceived.for_().c_str() );
						strReceived.AppendFormat( " for %s", str );
					}

					if ( stReceived.date().getString().c_str() ) {
						CPeekStringA	str( stReceived.date().getString().c_str() );
						strReceived.AppendFormat( " ; %s", str );
					}
#endif

					const CPeekStringA*	ayStrs[] = {
						&Tags::kReceivedLabelA,
						&strReceived,
						NULL
					};
					CPeekStringA	strLabel( FastCat( ayStrs ));
					m_strsReceived.push_back( strLabel );
				}
				break;

			case kOrigDate:
			case kMailFrom:
			case kRcptTo:
			case kFrom:
			case kSender:
			case kTo:
			case kCC:
			case kBCC:
			case kSubject:
			case kComments:
			case kAttachments:
			case kOther:
			case kNonField:
			case kEndOfHeaders:
			default:
				break;
			}
		}

		if ( m_strsReceived.empty() ) {
			// If there are no Received fields, then build a Received field.
			CPeekStringA	strClientIp( m_IpPortPair.GetSrcAddress().FormatA() );
			CPeekStringA	strTimeStamp( FormatTimeStampA( m_InterceptTime, kUtil_Flag_No_TimeZone ) );
			const CPeekStringA*	ayStrs[] = {
				&Tags::kReceivedFromLabelA,
				Tags::kxSquareLeft.RefA(),
				&strClientIp,
				Tags::kxSquareRight.RefA(),
				Tags::kxSemiSpace.RefA(),
				&strTimeStamp,
				NULL
			};

			m_strsReceived.push_back( FastCat( ayStrs ) );
		}
	}
	catch ( ... ) {
		;
	}
}


#if 0
// -----------------------------------------------------------------------------
//		AddXmlContactList
// -----------------------------------------------------------------------------

void
CPenInfo::AddXmlContactList(
	CEasyXMLElement*	inList,
	CMsgAddressList&	inMsgList )
{
	ASSERT( inList != NULL );
	if ( inList == NULL ) return;

	for ( size_t i = 0; i < inMsgList.GetCount(); i++ ) {
		CMsgAddress&	Contact = inMsgList.GetAt( i );
		CPeekString			strType = Contact.GetTypeString();
		CPeekStringX	strName = Contact.GetName();
		CPeekStringX	strAddress = Contact.GetAddress();

		CEasyXMLElement*	pContact = inList->AddChild( Tags::kContact );
		if ( pContact != NULL ) {
			pContact->AddAttribute( Tags::kList, strType );
			if ( !strName.IsEmpty() ) {
				pContact->AddChild( Tags::kName, strName );
			}
			if ( !strAddress.IsEmpty() ) {
				pContact->AddChild( Tags::kAddress, strAddress );
			}
		}
	}
}
#endif


// ----------------------------------------------------------------------------
//		FormatEmlBody
// ----------------------------------------------------------------------------

void
CPenInfo::FormatEmlBody()
{
	m_strEmlBody.Empty();

	const size_t	nLineCount = (m_strsReceived.size() * 2) + (13 * 4) + 2;
	CFastCatA		fcStrs( nLineCount );

	// Received
	for ( size_t i = 0; i < m_strsReceived.size(); i++ ) {
		fcStrs.push_back( &m_strsReceived[i] );
		fcStrs.push_back( Tags::kxNewLine.RefA() );
	}

	// Message-ID
	if ( !m_strMessageId.IsEmpty() ) {
		fcStrs.push_back( Tags::kxMessageId.RefA() );
		fcStrs.push_back( Tags::kxColonSpace.RefA() );
		fcStrs.push_back( m_strMessageId.RefA() );
		fcStrs.push_back( Tags::kxNewLine.RefA() );
	}

	// Date
	if ( !m_strDate.IsEmpty() ) {
		fcStrs.push_back( Tags::kxDate.RefA() );
		fcStrs.push_back( Tags::kxColonSpace.RefA() );
		fcStrs.push_back( &m_strDate );
		fcStrs.push_back( Tags::kxNewLine.RefA() );
	}

	// Reply-To
	CPeekStringA	strReplyTo;		// Outside of if to maintain scope.
	if ( !m_strReplyTo.IsEmpty() ) {
		strReplyTo = CleanAddress( m_strReplyTo );
		fcStrs.push_back( Tags::kxReplyTo.RefA() );
		fcStrs.push_back( Tags::kxColonSpace.RefA() );
		fcStrs.push_back( &strReplyTo );
		fcStrs.push_back( Tags::kxNewLine.RefA() );
	}

	// Return-Path
	if ( !m_strReturnPath.IsEmpty() ) {
		fcStrs.push_back( Tags::kxReturnPath.RefA() );
		fcStrs.push_back( Tags::kxColonSpace.RefA() );
		fcStrs.push_back( &m_strReturnPath );
		fcStrs.push_back( Tags::kxNewLine.RefA() );
	}

	// X-Originating-IP
	if ( !m_strXOriginatingIp.IsEmpty() ) {
		fcStrs.push_back( Tags::kxXOriginatingIp.RefA() );
		fcStrs.push_back( Tags::kxColonSpace.RefA() );
		fcStrs.push_back( &m_strXOriginatingIp );
		fcStrs.push_back( Tags::kxNewLine.RefA() );
	}

	// X-Apparently-To
	if ( !m_strXApparentlyTo.IsEmpty() ) {
		fcStrs.push_back( Tags::kxXApparentlyTo.RefA() );
		fcStrs.push_back( Tags::kxColonSpace.RefA() );
		fcStrs.push_back( &m_strXApparentlyTo );
		fcStrs.push_back( Tags::kxNewLine.RefA() );
	}

	// X-Apparently-From
	if ( !m_strXApparentlyFrom.IsEmpty() ) {
		fcStrs.push_back( Tags::kxXApparentlyFrom.RefA() );
		fcStrs.push_back( Tags::kxColonSpace.RefA() );
		fcStrs.push_back( &m_strXApparentlyFrom );
		fcStrs.push_back( Tags::kxNewLine.RefA() );
	}

	// X-Date-Intercepted
	CPeekStringA	strIntercept = FormatTimeStampA( m_InterceptTime, kUtil_Flag_No_TimeZone );
	fcStrs.push_back( Tags::kxXDateIntercepted.RefA() );
	fcStrs.push_back( Tags::kxColonSpace.RefA() );
	fcStrs.push_back( &strIntercept );
	fcStrs.push_back( Tags::kxNewLine.RefA() );

	// X-Mail-Protocol
	if ( !m_strProtocol.IsEmpty() ) {
		fcStrs.push_back( Tags::kxXMailProtocol.RefA() );
		fcStrs.push_back( Tags::kxColonSpace.RefA() );
		fcStrs.push_back( m_strProtocol.RefA() );
		fcStrs.push_back( Tags::kxNewLine.RefA() );
	}

	// X-MailClient-IP
	CPeekStringA	strMailClientIp = m_IpPortPair.GetSrcAddress().FormatA();
	fcStrs.push_back( Tags::kxXMailClientIp.RefA() );
	fcStrs.push_back( Tags::kxColonSpace.RefA() );
	fcStrs.push_back( &strMailClientIp );
	fcStrs.push_back( Tags::kxNewLine.RefA() );

	// X-MailServer-IP
	CPeekStringA	strMailServerIp = m_IpPortPair.GetDstAddress().FormatA();
	fcStrs.push_back( Tags::kxXMailServerIp.RefA() );
	fcStrs.push_back( Tags::kxColonSpace.RefA() );
	fcStrs.push_back( &strMailServerIp );
	fcStrs.push_back( Tags::kxNewLine.RefA() );

	// X-MailServer-Port
	CPeekStringA	strMailServerPort = m_IpPortPair.GetDstPort().FormatA();
	fcStrs.push_back( Tags::kxXMailServerPort.RefA() );
	fcStrs.push_back( Tags::kxColonSpace.RefA() );
	fcStrs.push_back( &strMailServerPort );
	fcStrs.push_back( Tags::kxNewLine.RefA() );

	// X-Mail-Size
	CPeekOutStringA	ssMessageSize;
	ssMessageSize << m_SizeInfo.nMsgSize;
	CPeekStringA	strMailSize( ssMessageSize );
	fcStrs.push_back( Tags::kxXMailSize.RefA() );
	fcStrs.push_back( Tags::kxColonSpace.RefA() );
	fcStrs.push_back( &strMailSize );
	fcStrs.push_back( Tags::kxNewLine.RefA() );

	// ending CRLFCRLF
	fcStrs.push_back( Tags::kxNewLine.RefA() );
	fcStrs.push_back( Tags::kxNewLine.RefA() );

	// Convert the vector of strings into one big string.
	_ASSERTE( fcStrs.GetCount()< nLineCount );
	m_strEmlBody = fcStrs.FastCat();
}


// ----------------------------------------------------------------------------
//		FormatTimeStampA
// ----------------------------------------------------------------------------

CPeekStringA
CPenInfo::FormatTimeStampA(
	const CPeekTime&	inTime,
	int					inFlags )
{
	CPeekStringA			strFormat;
#if (0)
	SYSTEMTIME				stTime( inTime );
	LONG					nTimeZone( 0 );
	TIME_ZONE_INFORMATION	tzInfo;
	DWORD	dwResult = ::GetTimeZoneInformation( &tzInfo );

	if ( (dwResult == TIME_ZONE_ID_STANDARD) || (dwResult == TIME_ZONE_ID_DAYLIGHT) ) {
		nTimeZone = -tzInfo.Bias;		// Invert for display.
	}

	char	szDate[32];
	if ( inFlags && kUtil_Flag_No_Day ) {
		::GetDateFormatA( LOCALE_SYSTEM_DEFAULT, 0, &stTime, "MMMM dd, yyyy", szDate, sizeof( szDate ) );
	}
	else {
		::GetDateFormatA( LOCALE_SYSTEM_DEFAULT, 0, &stTime, "dddd, MMMM dd, yyyy", szDate, sizeof( szDate ) );
	}

	char	szTime[16];
	::GetTimeFormatA( LOCALE_SYSTEM_DEFAULT, 0, &stTime, "hh:mm:ss tt", szTime, sizeof( szTime ) );

	CPeekStringA	strDate( szDate );
	CPeekStringA	strTime( szTime );
	if ( inFlags && kUtil_Flag_No_TimeZone ) {
		const CPeekStringA*	ayStrs[] = {
			&strDate,
			Tags::kxSpace.RefA(),
			&strTime,
			NULL
		};
		strFormat = FastCat( ayStrs );
	}
	else {
		SInt32			nTzHours( nTimeZone / 60 );
		SInt32			nTzMinutes( abs( nTimeZone ) % 60 );
		CPeekStringA	strTzHours;
		CPeekStringA	strTzMinutes;
		strTzMinutes.Format( "%d", nTzHours );
		if ( strTzHours.size() == 1 ) {
			strTzHours.Insert( 0, "0" );
		}
		strTzMinutes.Format( "%d", nTzMinutes );
		if ( strTzMinutes.size() == 1 ) {
			strTzMinutes.Insert( 0, "0" );
		}
		
		const CPeekStringA*	ayStrs[] = {
			&strDate,
			Tags::kxSpace.RefA(),
			&strTime,
			Tags::kxSpace.RefA(),
			&strTzHours,
			Tags::kxColon.RefA(),
			&strTzMinutes,
			NULL
		};
		strFormat = FastCat( ayStrs );
	}
#else
	(void) inTime;
	(void) inFlags;
#endif
	return strFormat;
}


// ----------------------------------------------------------------------------
//		FormatTxtDateBody
// ----------------------------------------------------------------------------

void
CPenInfo::FormatTxtDateBody()
{
	// size_t	nIndex( 0 );

	const size_t	nDateLines = 6;
	CFastCatA		fcDateStrs( nDateLines );

	// Date Sent
	fcDateStrs.push_back( &m_strDate );
	fcDateStrs.push_back( Tags::kxTab.RefA() );

	// X-Date-Intercepted
	CPeekStringA	strIntercept( FormatTimeStampA( m_InterceptTime, kUtil_Flag_No_TimeZone ) );
	fcDateStrs.push_back( &strIntercept );
	fcDateStrs.push_back( Tags::kxTab.RefA() );

	// X-Mail-Protocol
	fcDateStrs.push_back( m_strProtocol.RefA() );
	fcDateStrs.push_back( Tags::kxTab.RefA() );

	// Convert the Array of strings into one big string.
	_ASSERTE( fcDateStrs.GetCount() <= nDateLines );
	m_strTxtDate = fcDateStrs.FastCat();

	const size_t	nReceiveCount( m_strsReceived.size() );
	const size_t	nLineCount( 21 + nReceiveCount );
	CFastCatA		fcStrs( nLineCount );

	// X-MailClient-IP
	CPeekStringA	strSource( m_IpPortPair.GetSrcAddress().FormatA() );
	fcStrs.push_back( &strSource );
	fcStrs.push_back( Tags::kxTab.RefA() );

	// X-MailServer-IP
	CPeekStringA	strDestination( m_IpPortPair.GetDstAddress().FormatA() );
	fcStrs.push_back( &strDestination );
	fcStrs.push_back( Tags::kxTab.RefA() );

	// X-MailServer-Port
	CPeekStringA	strPort( m_IpPortPair.GetDstPort().FormatA() );
	fcStrs.push_back( &strPort );
	fcStrs.push_back( Tags::kxTab.RefA() );
	// nIndex = fcStrs.size();

	// X-Mail-Size
	CPeekOutStringA	ssMessageSize;
	ssMessageSize << m_SizeInfo.nMsgSize;
	CPeekStringA	strMessageSize( ssMessageSize );
	fcStrs.push_back( &strMessageSize );
	fcStrs.push_back( Tags::kxTab.RefA() );

	// Message-ID
	CPeekStringA	strMessageId( m_strMessageId.GetA() );
	fcStrs.push_back( &strMessageId );
	fcStrs.push_back( Tags::kxTab.RefA() );

	// Return-Path
	fcStrs.push_back( &m_strReturnPath );
	fcStrs.push_back( Tags::kxTab.RefA() );

	// Reply-To
	CPeekStringA	strReplyTo( CleanAddress( m_strReplyTo ) );
	fcStrs.push_back( &strReplyTo );
	fcStrs.push_back( Tags::kxTab.RefA() );

	// X-Originating-IP
	fcStrs.push_back( &m_strXOriginatingIp );
	fcStrs.push_back( Tags::kxTab.RefA() );

	// X-Apparently-To
	fcStrs.push_back( &m_strXApparentlyTo );
	fcStrs.push_back( Tags::kxTab.RefA() );

	// X-Apparently-From
	fcStrs.push_back( &m_strXApparentlyFrom );
	fcStrs.push_back( Tags::kxTab.RefA() );

	// Received
	CPeekStringA	strReceived;
	for ( size_t i = 0; i < nReceiveCount; i++ ) {
		const CPeekStringA*	ayReceiveStrs[] = {
			Tags::kxCurlyLeft.RefA(),
			&m_strsReceived[i],
			Tags::kxCurlyRight.RefA(),
			NULL
		};
		strReceived = FastCat( ayReceiveStrs );
		fcStrs.push_back( &strReceived );
	}

	fcStrs.push_back( Tags::kxNewLine.RefA() );

	// Concatenate the strings into one big string.
	_ASSERTE( fcStrs.GetCount() <= nLineCount );
	m_strTxtBody = fcStrs.FastCat();
}


// -----------------------------------------------------------------------------
//		FormatXmlContacts
// -----------------------------------------------------------------------------

#if (TOVERIFY)
void
CPenInfo::FormatXmlContacts()
{
//	typedef CMsgAddress::MsgAddressType	MsgAddressType;

#if (TODO)
	CEasyXMLElement*	pRoot = m_xmlDoc.GetRootElement();
	if ( pRoot == NULL ) return;

	CEasyXMLElement*	pEmail = pRoot->GetChild( 0 );
	if ( pEmail == NULL ) return;

	CEasyXMLElement*	pSenderList = pEmail->GetNamedChild( Tags::kSenderList );
	if ( pSenderList != NULL ) {
		AddXmlContactList( pSenderList, m_MailFromList );
	}

	CEasyXMLElement*	pRecipeintList = pEmail->GetNamedChild( Tags::kRecipientList );
	if ( pRecipeintList != NULL ) {
		AddXmlContactList( pRecipeintList, m_RcptToList );
	}
#endif
}
#endif

void
CPenInfo::FormatXmlContacts(
	const CMsgAddress& inTarget )
{
#if (TODO)
	typedef CMsgAddress::MsgAddressType	MsgAddressType;

	CEasyXMLElement*	pRoot = m_xmlDoc.GetRootElement();
	if ( pRoot == NULL ) return;

	CEasyXMLElement*	pEmail = pRoot->GetChild( 0 );
	if ( pEmail == NULL ) return;

	CEasyXMLElement*	pSenderList = pEmail->GetNamedChild( Tags::kSenderList );
	if ( pSenderList != NULL ) {
		AddXmlContactList( pSenderList, m_MailFromList );
	}

	CEasyXMLElement*	pRecipientList = pEmail->GetNamedChild( Tags::kSenderList );
	if ( pRecipientList != NULL ) {
		for ( size_t i = 0; i < m_RcptToList.GetCount(); i++ ) {
			CMsgAddress&	Contact = m_RcptToList.GetAt( i );
			CPeekStringX	strAddress = Contact.GetAddress();

			if ( inTarget.IsMatch( strAddress ) ) {
				CPeekString			strType = Contact.GetTypeString();
				CPeekStringX	strName = Contact.GetName();

				CEasyXMLElement*	pContact = pRecipientList->AddChild( Tags::kContact );
				if ( pContact != NULL ) {
					pContact->AddAttribute( Tags::kList, strType );
					if ( !strName.IsEmpty() ) {
						pContact->AddChild( Tags::kName, strName );
					}
					if ( !strAddress.IsEmpty() ) {
						pContact->AddChild( Tags::kAddress, strAddress );
					}
				}
				break;
			}
		}
	}
#else
	(void) inTarget;
#endif
}


// -----------------------------------------------------------------------------
//		FormatXmlDocument
// -----------------------------------------------------------------------------

void
CPenInfo::FormatXmlDocument(
	bool	inReset )
{
#if (TOVERIFY)
	if ( inReset ) {
		m_spModeler = auto_ptr<CPeekDataModeler>( new CPeekDataModeler( Tags::kPenEmails ) );
	}

	ASSERT( m_spModeler->IsStoring() );
	if ( !m_spModeler->IsStoring() ) return;

	CPeekXmlTransferPtr	pTransferRoot = m_spModeler->Child( Tags::kPenEmails );
	CPeekXmlStore*	pRoot = dynamic_cast<CPeekXmlStore*>( pTransferRoot.get() );
	ASSERT( pRoot );
	if ( pRoot == NULL ) return;

	CPeekXmlTransferPtr	pEmail = pRoot->Child( Tags::kEmail, 0 );

	pEmail->Attribute( Tags::kProtocol, m_strProtocol );
	CPeekString	strMsgSize;
#if (0)
	strMsgSize.Format( "%d", m_SizeInfo.nMsgSize );
#endif
	pEmail->Attribute( Tags::kSize, strMsgSize );

	CPeekXmlTransferPtr	pDateSent = pEmail->Child( Tags::kDateSent );
	CPeekString	strDate( m_strDate );
	pDateSent->Value( strDate );

	CPeekXmlTransferPtr	pDateIntercepted = pEmail->Child( Tags::kDateIntercepted );
	CPeekString	strInterceptTime( FormatTimeStampA( m_InterceptTime, kUtil_Flag_No_TimeZone ) );
	pDateIntercepted->Value( strInterceptTime );
#else
	CEasyXMLElement*	pRoot( NULL );

	if ( inReset ) {
		m_xmlDoc.Reset( inXmlEncoding );
	}
	pRoot = m_xmlDoc.AddRootElement( Tags::kPenEmails );
	ASSERT( pRoot != NULL );
	if ( pRoot == NULL ) return;

	CEasyXMLElement*	pEmail( pRoot->AddChild( Tags::kEmail ) );
	ASSERT( pEmail != NULL );
	if ( pEmail == NULL ) return;

	pEmail->AddAttribute( Tags::kProtocol, m_strProtocol );
	pEmail->AddAttribute( Tags::kSize, m_SizeInfo.nMsgSize );

	pEmail->AddChild( Tags::kDateSent, CString( m_strDate ) );

	pEmail->AddChild( Tags::kDateIntercepted, CString( FormatTimeStamp( &m_InterceptTime ) ) );

	pEmail->AddChild( Tags::kSenderList );

	pEmail->AddChild( Tags::kRecipientList );

	CEasyXMLElement*	pEmailClient = pEmail->AddChild( Tags::kEmailClient );
	if ( pEmailClient != NULL ) {
		pEmailClient->AddChild( Tags::kIpAddress, m_IpPortPair.GetSrcAddress().Format() );
	}

	CEasyXMLElement*	pMailServer = pEmail->AddChild( Tags::kMailServer );
	if ( pMailServer != NULL ) {
		pMailServer->AddChild( Tags::kIpAddress, m_IpPortPair.GetDstAddress().Format() );
		pMailServer->AddChildUnsigned( Tags::kPort, m_IpPortPair.GetDstPort() );
	}

	pEmail->AddChild( Tags::kMessageId, m_strMessageId.Get() );

	pEmail->AddChild( XmlTags::kReturnPath, CString( m_strReturnPath ) );

	pEmail->AddChild( Tags::kReplyTo, CString( m_strReplyTo ) );

	pEmail->AddChild( Tags::kxXOriginatingIp.GetW(), CString( m_strXOriginatingIp ) );

	pEmail->AddChild( Tags::kxXApparentlyTo.GetW(), CString( m_strXApparentlyTo ) );

	pEmail->AddChild( Tags::kxXApparentlyFrom.GetW(), CString( m_strXApparentlyFrom ) );

	CEasyXMLElement*	pReceived = pEmail->AddChild( Tags::kReceivedList );
	if ( pReceived != NULL ) {
		for ( size_t i = 0; i < m_strsReceived.GetCount(); i++ ) {
			pReceived->AddChild( Tags::kReceived, CA2CT( m_strsReceived[i] ) );
		}
	}
#endif // TOVERIFY
}


// ----------------------------------------------------------------------------
//		WritePenEml
// ----------------------------------------------------------------------------

bool
CPenInfo::WritePenEml(
	const CPeekString&	inFileName )
{
	if ( m_strEmlBody.IsEmpty() ) {
		FormatEmlBody();
	}

	// Format all the recipients.
	CPeekStringA	strEmlHeaderA( FormatEmlHeaderA() );

	UInt32	nFlags( CFileEx::modeCreate | CFileEx::modeWrite );
	CFileEx	theFile;
	BOOL	bResult( theFile.Open( inFileName, nFlags ) );
	if ( bResult == FALSE ) {
		m_pMsgStream->NoteFileError( inFileName );
		return false;
	}

	try {
		theFile.Write( strEmlHeaderA );
		theFile.Write( m_strEmlBody );
	}
	catch ( ... ) {
		m_pMsgStream->NoteWriteError( inFileName );
		return false;
	}
	return true;
}

bool
CPenInfo::WritePenEml(
	CFileEx&	inFile )
{
	if ( m_strEmlBody.IsEmpty() ) {
		FormatEmlBody();
	}

	// Format all the recipients.
	CPeekStringA	strEmlHeader( FormatEmlHeaderA() );

	try {
		inFile.Write( strEmlHeader );
		inFile.Write( m_strEmlBody );
	}
	catch ( ... ) {
		m_pMsgStream->NoteWriteError( inFile.GetFileName() );
		return false;
	}
	return true;
}


// ----------------------------------------------------------------------------
//		WritePenEml
// ----------------------------------------------------------------------------

bool
CPenInfo::WritePenEml(
	const CTargetAddress&	inTargetAddress,
	const CPeekString&		inFileName )
{
	CPeekStringA	strEmlHeader( FormatEmlHeaderA( inTargetAddress ) );
	if ( strEmlHeader.IsEmpty() ) return false;

	if ( m_strEmlBody.IsEmpty() ) {
		FormatEmlBody();
	}

	UInt32	nFlags( CFileEx::modeCreate | CFileEx::modeWrite );
	CFileEx	theFile;
	BOOL	bResult( theFile.Open( inFileName, nFlags ) );
	if ( bResult == FALSE ) {
		m_pMsgStream->NoteFileError( inFileName );
		return false;
	}

	try {
		theFile.Write( strEmlHeader );
		theFile.Write( m_strEmlBody );
	}
	catch ( ... ) {
		m_pMsgStream->NoteWriteError( inFileName );
		return false;
	}
	return true;
}

bool
CPenInfo::WritePenEml(
	const CTargetAddress&	inTargetAddress,
	CFileEx&				inFile )
{
	CPeekStringA	strEmlHeader( FormatEmlHeaderA( inTargetAddress ) );
	if ( strEmlHeader.IsEmpty() ) return false;

	if ( m_strEmlBody.IsEmpty() ) {
		FormatEmlBody();
	}

	try {
		inFile.Write( strEmlHeader );
		inFile.Write( m_strEmlBody );
	}
	catch ( ... ) {
		m_pMsgStream->NoteWriteError( inFile.GetFileName() );
		return false;
	}
	return true;
}


// ----------------------------------------------------------------------------
//		WritePenText
// ----------------------------------------------------------------------------

bool
CPenInfo::WritePenText(
	CFileEx*	inFile )
{
	if ( m_strTxtDate.IsEmpty() ) {
		FormatTxtDateBody();
	}

	ASSERT( inFile != NULL );
	if ( inFile == NULL ) {
		return false;
	}

	try {
		CPeekStringA	strTxtHeader( FormatTxtHeaderA() );

		if ( inFile->SeekToEnd() == 0 ) {
			inFile->Write( Tags::kHeadingsA );
		}
		inFile->Write( m_strTxtDate );
		inFile->Write( strTxtHeader );
		inFile->Write( m_strTxtBody );
	}
	catch ( ... ) {
		CPeekString	strFileName = inFile->GetFileName();
		m_pMsgStream->NoteWriteError( strFileName );
		return false;
	}
	return true;
}

bool
CPenInfo::WritePenText(
	const CTargetAddress&	inTargetAddress,
	CFileEx*				inFile )
{
	if ( m_strTxtDate.IsEmpty() ) {
		FormatTxtDateBody();
	}

	size_t			nIndex( kIndex_Invalid );
	CPeekStringA	strTxtHeader;
	if ( m_MailFromList.Find( inTargetAddress, nIndex ) ) {
		strTxtHeader = FormatTxtHeaderA();
	}
	else {
		if ( m_RcptToList.Find( inTargetAddress, nIndex ) ) {
			CMsgAddress&	maItem = m_RcptToList[nIndex];
			strTxtHeader = FormatTxtHeaderA( maItem );
		}
	}
	if ( nIndex < 0 ) return false;

	ASSERT( inFile != NULL );
	if ( inFile == NULL ) {
		return false;
	}

	try {
		if ( inFile->SeekToEnd() == 0 ) {
			inFile->Write( Tags::kHeadingsA );
		}
		inFile->Write( m_strTxtDate );
		inFile->Write( strTxtHeader );
		inFile->Write( m_strTxtBody );
	}
	catch ( ... ) {
		m_pMsgStream->NoteWriteError( inFile->GetFileName() );
		return false;
	}

	return true;
}


// -----------------------------------------------------------------------------
//		WritePenXml
// -----------------------------------------------------------------------------

#if (TOVERIFY)
bool
CPenInfo::WritePenXml(
	CFileEx*	inFile )
{
	//ASSERT( inFile.IsValid() );
	//if ( !inFile.IsValid() ) {
	//	return false;
	//}

	try {
		bool	bNewFile = (inFile->SeekToEnd() == 0);

		FormatXmlDocument( bNewFile );
		FormatXmlContacts();

		CPeekPersistFile	spFile = m_spModeler->GetXml();
		if ( spFile ) spFile->Save( inFile->GetFileName(), FALSE );

#if (TODO)
		CPeekStringA	strXml;
		if ( bNewFile ) {
			strXml = m_xmlDoc.FormatA();
		}
		else {
			inFile->Seek( -(Tags::kClosePenEmails.GetLength()), CFile::end );
			CEasyXMLElement*	pRoot( m_xmlDoc.GetRootElement() );
			ASSERT( pRoot != NULL );
			if ( pRoot == NULL ) return false;

			CEasyXMLElement*	pEmail( pRoot->GetChild( 0 ) );
			ASSERT( pEmail != NULL );
			if ( pEmail == NULL ) return false;

			strXml = pEmail->FormatA( 1 );
		}
		
		inFile->Write( strXml );
		if ( !bNewFile ) {
			inFile->Write( Tags::kxNewLine.GetA() );
			inFile->Write( Tags::kClosePenEmailsA );
		}
#endif // TODO
	}
	catch ( ... ) {
		CPeekString	strFileName = inFile->GetFileName();
		m_pMsgStream->NoteWriteError( strFileName );
		return false;
	}
	return true;
}
#endif // TOVERIFY

bool
CPenInfo::WritePenXml(
	const CTargetAddress&	inTargetAddress,
	CFileEx*				inFile )
{
	ASSERT( inFile != NULL );
	if ( inFile == NULL ) {
		return false;
	}

	try {
		bool	bNewFile = (inFile->SeekToEnd() == 0);
#if (TOVERIFY)
		FormatXmlDocument( bNewFile );
#endif // TOVERIFY

		size_t	nIndex( kIndex_Invalid );
		if ( m_MailFromList.Find( inTargetAddress, nIndex ) ) {
			//FormatXmlContacts();
		}
		else {
			if ( m_RcptToList.Find( inTargetAddress, nIndex ) ) {
				//CMsgAddress&	maItem = m_RcptToList[nIndex];
				//FormatXmlContacts( maItem );
			}
		}
		if ( nIndex == kIndex_Invalid ) return false;

		CPeekStream		psData;
#if (TOVERIFY)
		if ( bNewFile ) {
			m_spModeler->Store( psData );
		}
#else
		if ( bNewFile ) {
			strXml = m_xmlDoc.FormatA();
		}
		else {
			inFile->Seek( -(Tags::kClosePenEmails.GetLength()), CFile::end );
			CEasyXMLElement*	pRoot( m_xmlDoc.GetRootElement() );
			ASSERT( pRoot != NULL );
			if ( pRoot == NULL ) return false;

			CEasyXMLElement*	pEmail( pRoot->GetChild( 0 ) );
			ASSERT( pEmail != NULL );
			if ( pEmail == NULL ) return false;

			strXml = pEmail->FormatA( 1 );
		}
#endif // TOVERIFY

		inFile->Write( psData.GetDataRef() );
		if ( !bNewFile ) {
			inFile->Write( Tags::kxNewLine.GetA() );
			inFile->Write( Tags::kClosePenEmailsA );
		}
	}
	catch ( ... ) {
		m_pMsgStream->NoteWriteError( inFile->GetFileName() );
		return false;
	}

	return true;
}

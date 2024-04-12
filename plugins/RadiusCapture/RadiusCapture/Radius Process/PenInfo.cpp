// =============================================================================
//	PenInfo.cpp
//		implementation of the CPenInfo class.
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2004. All rights reserved.

#include "StdAfx.h"
#include "PenInfo.h"

static CStringA	g_strHeadings =
	"Date Sent\tDate Intercepted\tProtocol\tSender\tRecipients\tClient IP\tMail Server IP\t"
	"Mail Server Port\tMail Size\tMessage-ID\tReturn-Path\tReply-To\tX-Originating-IP\t"
	"X-Apparently-To\tX-Apparently-From\tReceived"
	"\r\n";


///////////////////////////////////////////////////////////////////////////////
//		CPenInfo
///////////////////////////////////////////////////////////////////////////////

CPenInfo::CPenInfo(
	CMsgStream*		inMsgStream )
	:	CMsgInfo( inMsgStream )
{
	try {
		int	nFieldCount = m_Headers.numFields();
		for ( int nField = 0; nField < nFieldCount; nField++ ) {
			const mimepp::Field&	fldField = m_Headers.fieldAt( nField );
			CFieldNameMap::CPair*	pPair = m_FieldNameMap.Lookup( fldField.fieldName().c_str() );
			FieldTypes				nType = (pPair) ? pPair->m_value.m_Type : kOther;

			// All ways put mimepp::Strings from Fields into a CStringA. This avoids the error of
			//  passing a mimepp::String into the printf class of functions.
			switch ( nType ) {
			case kMessageId:
				{
					const mimepp::MsgId&	MsgId = (const mimepp::MsgId&) fldField.fieldBody();
					CStringA				strLocal( MsgId.localPart().c_str() );
					CStringA				strDomain( MsgId.domain().c_str() );
					m_strMessageId.Format( "<%s@%s>", strLocal, strDomain );
				}
				break;

			case kReturnPath:
				m_strReturnPath = fldField.fieldBody().getString().c_str();
				break;

			case kReplyTo:
				{
					CStringA		strReplyTo( fldField.fieldBody().getString().c_str() );
					CTargetAddress	taReplyTo( CTargetAddress::kType_From, strReplyTo );
					taReplyTo.Format( m_strReplyTo );
				}
				break;

			case kReceived:
				{
					mimepp::Stamp&	stReceived = (mimepp::Stamp&) fldField.fieldBody();
					CStringA		strReceived( stReceived.getString().c_str() );
					strReceived.Remove( '\t' );
					strReceived.Replace( "\r\n", " " );
					strReceived.Trim();

					int	nCount;
					int nMax = strReceived.GetLength();
					do {
						nCount = strReceived.Replace( "  ", " " );
						nMax--;
					} while ( (nCount > 0) && (nMax > 0) );

#ifdef ENCODE_RECEIVE
					//Received: from xyz.net by mx.org via TELENET with SMTP
					//	id M12345 for smith@aol.com ; Tue, 6 Oct 1998 09:23:59 -0700

					if ( stReceived.from().c_str() ) {
						CStringA	str( stReceived.from().c_str() );
						strReceived.AppendFormat( " from %s", str );
					}
					if ( stReceived.by().c_str() ) {
						CStringA	str( stReceived.by().c_str() );
						strReceived.AppendFormat( " by %s", str );
					}
					if ( stReceived.via().c_str() ) {
						CStringA	str( stReceived.via().c_str() );
						strReceived.AppendFormat( " via %s", str );
					}
					if ( stReceived.with().c_str() ) {
						CStringA	str( stReceived.with().c_str() );
						strReceived.AppendFormat( " with %s", str  );
					}
					if ( stReceived.id().c_str() ) {
						CStringA	str( stReceived.id().c_str() );
						strReceived.AppendFormat( " id %s", str );
					}
					if ( stReceived.for_().c_str() ) {
						CStringA	str( stReceived.for_().c_str() );
						strReceived.AppendFormat( " for %s", str );
					}

					if ( stReceived.date().getString().c_str() ) {
						CStringA	str( stReceived.date().getString().c_str() );
						strReceived.AppendFormat( " ; %s", str );
					}
#endif

					CStringA	str;
					str.Format( "Received: %s", strReceived );
					m_strsReceived.Add( str );
				}
				break;
			}
		}

		if ( m_strsReceived.GetCount() == 0 ) {
			// If there are no Received fields, then build a Received field.
			CStringA	strClientIp = m_IpPortPair.GetSrcAddress().FormatA();
			CStringA	strTimeStamp = FormatTimeStamp( &m_InterceptTime );
			size_t	nIndex = m_strsReceived.Add();
			m_strsReceived[nIndex].Format( "Received: from [%s]; %s", strClientIp, strTimeStamp );
		}
	}
	catch ( ... ) {
		;
	}
}


// -----------------------------------------------------------------------------
//		FormatEmlBody
// -----------------------------------------------------------------------------

void
CPenInfo::FormatEmlBody()
{
	m_strEmlBody.Empty();

	size_t			nIndex = 0;
	CStringA		str;
	CArrayStringA	aStrs;

	// Received
	for ( size_t i = 0; i < m_strsReceived.GetCount(); i++ ) {
		nIndex = aStrs.Add();
		aStrs[nIndex].Format( "%s\r\n", m_strsReceived[i] );
	}

	// Message-ID
	if ( m_strMessageId.GetLength() ) {
		nIndex = aStrs.Add();
		aStrs[nIndex].Format( "Message-ID: %s\r\n", m_strMessageId );
	}

	// Date
	if ( m_strDate.GetLength() ) {
		nIndex = aStrs.Add();
		aStrs[nIndex].Format( "Date: %s\r\n", m_strDate );
	}

	// Reply-To
	if ( m_strReplyTo.GetLength() ) {
		str.Empty();
		CleanAddress( m_strReplyTo, str );
		nIndex = aStrs.Add();
		aStrs[nIndex].Format( "Reply-To: %s\r\n", str );
	}

	// Return-Path
	if ( m_strReturnPath.GetLength() ) {
		nIndex = aStrs.Add();
		aStrs[nIndex].Format( "Return-Path: %s\r\n", m_strReturnPath );
	}

	// X-Originating-IP
	if ( m_strXOriginatingIp.GetLength() ) {
		nIndex = aStrs.Add();
		aStrs[nIndex].Format( "X-Originating-IP: %s\r\n", m_strXOriginatingIp );
	}

	// X-Apparently-To
	if ( m_strXApparentlyTo.GetLength() ) {
		nIndex = aStrs.Add();
		aStrs[nIndex].Format( "X-Apparently-To: %s\r\n", m_strXApparentlyTo );
	}

	// X-Apparently-From
	if ( m_strXApparentlyFrom.GetLength() ) {
		nIndex = aStrs.Add();
		aStrs[nIndex].Format( "X-Apparently-From: %s\r\n", m_strXApparentlyFrom );
	}

	// X-Date-Intercepted
	str = FormatTimeStamp( &m_InterceptTime );
	nIndex = aStrs.Add();
	aStrs[nIndex].Format( "X-Date-Intercepted: %s\r\n", str );

	// X-Mail-Protocol
	if ( m_strProtocolA.GetLength() ) {
		nIndex = aStrs.Add();
		aStrs[nIndex].Format( "X-Mail-Protocol: %s\r\n", m_strProtocolA );
	}

	// X-MailClient-IP
	str = m_IpPortPair.GetSrcAddress().FormatA();
	nIndex = aStrs.Add();
	aStrs[nIndex].Format( "X-MailClient-IP: %s\r\n", str );

	// X-MailServer-IP
	str = m_IpPortPair.GetDstAddress().FormatA();
	nIndex = aStrs.Add();
	aStrs[nIndex].Format( "X-MailServer-IP: %s\r\n", str );

	// X-MailServer-Port
	nIndex = aStrs.Add();
	aStrs[nIndex].Format( "X-MailServer-Port: %u\r\n", (UInt32) m_IpPortPair.GetDstPort() );

	// X-Mail-Size
	nIndex = aStrs.Add();
	aStrs[nIndex].Format( "X-Mail-Size: %u\r\n", m_SizeInfo.nMsgSize );

	// ending CRLFCRLF
	nIndex = aStrs.Add();
	aStrs[nIndex].Format( "\r\n\r\n" );

	// Convert the Array of strings into one big string.
	ArrayStringToString( aStrs, m_strEmlBody );
}


// -----------------------------------------------------------------------------
//		FormatTxtDateBody
// -----------------------------------------------------------------------------

void
CPenInfo::FormatTxtDateBody()
{
	CStringA		str;
	CArrayStringA	aStrs;
	size_t			nIndex = 0;

	// Date Sent
	str.Format( "%s\t", m_strDate );
	nIndex = aStrs.Add( str );

	// X-Date-Intercepted
	str = FormatTimeStamp( &m_InterceptTime );
	nIndex = aStrs.Add();
	aStrs[nIndex].Format( "%s\t", str );

	// X-Mail-Protocol
	nIndex = aStrs.Add();
	aStrs[nIndex].Format( "%s\t", (m_strProtocolA) ? m_strProtocolA : "" );

	// Convert the Array of strings into one big string.
	ArrayStringToString( aStrs, m_strTxtDate );

	aStrs.RemoveAll();

	// X-MailClient-IP
	str = m_IpPortPair.GetSrcAddress().FormatA();
	nIndex = aStrs.Add();
	aStrs[nIndex].Format( "%s\t", str );

	// X-MailServer-IP
	str = m_IpPortPair.GetDstAddress().FormatA();
	nIndex = aStrs.Add();
	aStrs[nIndex].Format( "%s\t", str );

	// X-MailServer-Port
	nIndex = aStrs.Add();
	aStrs[nIndex].Format( "%u\t", (UInt32) m_IpPortPair.GetDstPort() );

	// X-Mail-Size
	nIndex = aStrs.Add();
	aStrs[nIndex].Format( "%u\t", m_SizeInfo.nMsgSize );

	// Message-ID
	nIndex = aStrs.Add();
	aStrs[nIndex].Format( "%s\t", m_strMessageId );

	// Return-Path
	nIndex = aStrs.Add();
	aStrs[nIndex].Format( "%s\t", m_strReturnPath );

	// Reply-To
	str.Empty();
	CleanAddress( m_strReplyTo, str );
	nIndex = aStrs.Add();
	aStrs[nIndex].Format( "%s\t", str );

	// X-Originating-IP
	nIndex = aStrs.Add();
	aStrs[nIndex].Format( "%s\t", m_strXOriginatingIp );

	// X-Apparently-To
	nIndex = aStrs.Add();
	aStrs[nIndex].Format( "%s\t", m_strXApparentlyTo );

	// X-Apparently-From
	nIndex = aStrs.Add();
	aStrs[nIndex].Format( "%s\t", m_strXApparentlyFrom );

	// Received
	for ( size_t i = 0; i < m_strsReceived.GetCount(); i++ ) {
		nIndex = aStrs.Add();
		aStrs[nIndex].Format( "{%s}", m_strsReceived[i] );
	}

	aStrs.Add( "\r\n" );

	// Convert the Array of strings into one big string.
	ArrayStringToString( aStrs, m_strTxtBody );
}


// -----------------------------------------------------------------------------
//		WritePenEml
// -----------------------------------------------------------------------------

void
CPenInfo::WritePenEml(
	const CString&	inFileName,
	CArrayString&	outTargets )
{
	if ( m_strEmlBody.GetLength() == 0 ) {
		FormatEmlBody();
	}

	CStringA	strEmlHeader;
	FormatEmlHeader( strEmlHeader );

	CFile	theFile;
	BOOL	bResult = theFile.Open( inFileName, (CFile::modeCreate | CFile::modeWrite) );
	if ( bResult == FALSE ) {
		m_pMsgStream->NoteFileError( inFileName );
		return;
	}

	try {
		theFile.Write( strEmlHeader.GetBuffer(), strEmlHeader.GetLength() );
		theFile.Write( m_strEmlBody.GetBuffer(), m_strEmlBody.GetLength() );
		AddFormattedTarget( outTargets );
	}
	catch ( ... ) {
		m_pMsgStream->NoteWriteError( inFileName );
	}
	return;
}


// -----------------------------------------------------------------------------
//		WritePenEmls
// -----------------------------------------------------------------------------

void
CPenInfo::WritePenEmls(
    const CString&	inPrefix,
	const CString&	inOutputDirectory,
	CArrayString&	outTargets )
{
	if ( m_strEmlBody.GetLength() == 0 ) {
		FormatEmlBody();
	}

	for ( size_t i = 0; i < m_MailFromList.GetCount(); i++ ) {
		CTargetAddress&	taItem = m_MailFromList[i];
		if ( taItem.IsTarget() ) {
			CStringA	strEmlHeader;
			FormatEmlHeader( strEmlHeader );

			CString	strFileName = BuildUniqueName( inPrefix,
				inOutputDirectory, m_strProtocol, kFileTypeEml, kEmlTypeOther );
			
			CFile	theFile;
			if ( theFile.Open( strFileName, (CFile::modeCreate | CFile::modeWrite) ) ) {
				try {
					theFile.Write( strEmlHeader.GetBuffer(), strEmlHeader.GetLength() );
					theFile.Write( m_strEmlBody.GetBuffer(), m_strEmlBody.GetLength() );
					AddFormattedTarget( outTargets );
				}
				catch ( ... ) {
					m_pMsgStream->NoteWriteError( strFileName );
				}
			}
			else {
				m_pMsgStream->NoteFileError( strFileName );
			}
		}
	}

	for ( size_t i = 0; i < m_RcptToList.GetCount(); i++ ) {
		CTargetAddress&	taItem = m_RcptToList[i];
		if ( taItem.IsTarget() ) {
			CStringA	strEmlHeader;
			FormatEmlHeader( taItem, strEmlHeader );

			CString	strFileName = BuildUniqueName( inPrefix,
				inOutputDirectory, m_strProtocol, kFileTypeEml, kEmlTypeOther );
			
			CFile	theFile;
			if ( theFile.Open( strFileName, (CFile::modeCreate | CFile::modeWrite) ) ) {
				try {
					theFile.Write( strEmlHeader.GetBuffer(), strEmlHeader.GetLength() );
					theFile.Write( m_strEmlBody.GetBuffer(), m_strEmlBody.GetLength() );
					AddFormattedTarget( taItem, outTargets );
				}
				catch ( ... ) {
					m_pMsgStream->NoteWriteError( strFileName );
				}
			}
			else {
				m_pMsgStream->NoteFileError( strFileName );
			}
		}
	}
	return;
}


// -----------------------------------------------------------------------------
//		WritePenText
// -----------------------------------------------------------------------------

void
CPenInfo::WritePenText(
	const CString&	inFileName,
	CArrayString&	outTargets )
{
	if ( m_strTxtDate.GetLength() == 0 ) {
		FormatTxtDateBody();
	}

	CFile	theFile;
	if ( theFile.Open( inFileName, (CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite) ) ) {
		try {
			CStringA	strTxtHeader;
			FormatTxtHeader( strTxtHeader );

			if ( theFile.SeekToEnd() == 0 ) {
				theFile.Write( g_strHeadings, g_strHeadings.GetLength() );
			}
			theFile.Write( m_strTxtDate.GetBuffer(), m_strTxtDate.GetLength() );
			theFile.Write( strTxtHeader.GetBuffer(), strTxtHeader.GetLength() );
			theFile.Write( m_strTxtBody.GetBuffer(), m_strTxtBody.GetLength() );

			AddFormattedTarget( outTargets );
		}
		catch ( ... ) {
			m_pMsgStream->NoteWriteError( inFileName );
		}
		return;
	}
	m_pMsgStream->NoteFileError( inFileName );
	return;
}


// -----------------------------------------------------------------------------
//		WritePenTexts
// -----------------------------------------------------------------------------

void
CPenInfo::WritePenTexts(
	const CString&	inPrefix,
	const CString&	inOutputDirectory,
	CArrayString&	outTargets )
{
	if ( m_strTxtDate.GetLength() == 0 ) {
		FormatTxtDateBody();
	}
	MakePath( inOutputDirectory );

	for ( size_t i = 0; i < m_MailFromList.GetCount(); i++ ) {
		CTargetAddress&	taItem = m_MailFromList[i];
		if ( taItem.IsTarget() ) {
			CStringA	strTxtHeader;
			FormatTxtHeader( strTxtHeader );

			//CString	strFileName;
			//BuildFileName( taItem.GetAddress(), inOutputDirectory, strFileName );
	
			CString	strFileName = BuildUniqueName( inPrefix,
				inOutputDirectory, m_strProtocol, kFileTypeTxt, kEmlTypeOther );

			CFile	theFile;
			if ( theFile.Open( strFileName, (CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite) ) ) {
				try {
					if ( theFile.SeekToEnd() == 0 ) {
						theFile.Write( g_strHeadings, g_strHeadings.GetLength() );
					}
					theFile.Write( m_strTxtDate.GetBuffer(), m_strTxtDate.GetLength() );
					theFile.Write( strTxtHeader.GetBuffer(), strTxtHeader.GetLength() );
					theFile.Write( m_strTxtBody.GetBuffer(), m_strTxtBody.GetLength() );

					AddFormattedTarget( outTargets );
				}
				catch ( ... ) {
					m_pMsgStream->NoteWriteError( strFileName );
				}
			}
			else {
				m_pMsgStream->NoteFileError( strFileName );
			}
		}
	}

	for ( size_t i = 0; i < m_RcptToList.GetCount(); i++ ) {
		CTargetAddress&	taItem = m_RcptToList[i];
		if ( taItem.IsTarget() ) {
			CStringA	strTxtHeader;
			FormatTxtHeader( taItem, strTxtHeader );

			//CString	strFileName;
			//BuildFileName( taItem.GetAddress(), inOutputDirectory, strFileName );
			
			CString	strFileName = BuildUniqueName( inPrefix,
				inOutputDirectory, m_strProtocol, kFileTypeTxt, kEmlTypeOther );

			CFile	theFile;
			if ( theFile.Open( strFileName, (CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite) ) ) {
				try {
					if ( theFile.SeekToEnd() == 0 ) {
						theFile.Write( g_strHeadings, g_strHeadings.GetLength() );
					}
					theFile.Write( m_strTxtDate.GetBuffer(), m_strTxtDate.GetLength() );
					theFile.Write( strTxtHeader.GetBuffer(), strTxtHeader.GetLength() );
					theFile.Write( m_strTxtBody.GetBuffer(), m_strTxtBody.GetLength() );

					AddFormattedTarget( taItem, outTargets );
				}
				catch ( ... ) {
					m_pMsgStream->NoteWriteError( strFileName );
				}
			}
			else {
				m_pMsgStream->NoteFileError( strFileName );
			}
		}
	}
	return;
}

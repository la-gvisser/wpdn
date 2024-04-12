// ============================================================================
//	SmtpMsgStream.cpp
//		implementation of the CSmtpMsgStream class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008-2011. All rights reserved.

#include "StdAfx.h"
#include "Case.h"
#include "SmtpMsgStream.h"
#include "PenInfo.h"
#include "OmniEngineContext.h"
#include "PeekStream.h"


// ============================================================================
//		CSmtpMsgStream
// ============================================================================

CSmtpMsgStream::CSmtpMsgStream(
	const CIpAddressPortPair&	inIpPortPair,
	COmniEngineContext&			inContext,
	size_t						inIndex,
	const CMsgAddress&			inCase )
	:	CMsgStream( kProtocol_SMTP, inIpPortPair, inContext, inIndex, inCase, kMsgState_Open )
{
}


// ----------------------------------------------------------------------------
//		PenAnalysis
// ----------------------------------------------------------------------------

void
CSmtpMsgStream::PenAnalysis(
	const CCase&	inCase )
{
	try {
		const CCaseSettings&	Settings( inCase.GetSettings() );

		bool	bLogMessages( Settings.IsLogMessages() );
		m_Context.Increment( CEmailStats::kStat_SmtpPenEmailsCaptured );


		CPenInfo*			pPenInfo( GetPenInfo() );
		const CPeekString&	strPrefix( Settings.GetFileNamePrefix() );
		CPeekString			strAddressList;

		if ( Settings.IsPenEmail() ) {
			CStreamFileGroup	ayFileGroup;
			ayFileGroup.push_back( CStreamFileItem( kFileTypeEml ) );

			// bool	bResult( false );
			/* bResult = */ OpenCaseFiles( strPrefix, inCase, ayFileGroup );

			CStreamFileItem&	file( ayFileGroup.at( 0 ) );
			if ( !file.IsNull() ) {
				if ( pPenInfo->WritePenEml( file.GetFile() ) ) {
					strAddressList = pPenInfo->FormatAddressList();
					m_Context.NoteTarget( bLogMessages, strAddressList,
						L"Email Capture - Saved SMTP Pen email " );
				}
			}
		}

#if (0)
		if ( Settings.IsPenText() ) {
			CFileEx*	pFile( GetTextReportFile( strPrefix, inCase ) );
			if ( pFile != NULL ) {
				if ( pPenInfo->WritePenText( pFile ) ) {
					if ( strAddressList.IsEmpty() ) {
						strAddressList = pPenInfo->FormatAddressList();
					}
					m_Context.NoteTarget( bLogMessages, strPrefix,
						L"Email Capture - Wrote SMTP Pen Text entry " );
				}
			}
			else {
				NoteFileError( strPrefix );	// Need a file name.
			}
		}
#endif

#if (0)
		if ( Settings.IsPenXml() ) {
			CFileEx*	pFile = GetXmlReportFile( strPrefix, inCase );
			if ( pFile != NULL ) {
				if ( pPenInfo->WritePenXml( pFile ) ) {
					if ( strAddressList.GetLength() == 0 ) strAddressList = pPenInfo->FormatAddressList();
					m_Context.NoteTarget( bLogMessages, strPrefix,
						L"Email Capture - Wrote SMTP Pen XML entry " );
				}
			}
			else {
				NoteFileError( strPrefix );	// Need a file name.
			}
		}
#endif
	}
	catch ( ... ) {
		m_Context.NoteMalformed( CEmailStats::kStat_TypeSmtp );
	}
}

void
CSmtpMsgStream::PenAnalysis(
	const CCase&			inCase,
	const CTargetAddress&	inTargetAddress )
{
	try {
		const CCaseSettings	Settings( inCase.GetSettings() );

		bool	bLogMessages( Settings.IsLogMessages() );
		m_Context.Increment( CEmailStats::kStat_SmtpPenEmailsCaptured );

		CPenInfo*			pPenInfo( GetPenInfo() );
		const CPeekString&	strPrefix( Settings.GetFileNamePrefix() );
		const CPeekString&	strAddress( inTargetAddress.GetAddress() );

		if ( Settings.IsPenEmail() ) {
			CStreamFileGroup	ayFileGroup;
			ayFileGroup.push_back( CStreamFileItem( kFileTypeEml ) );

			// bool				bResult( false );
			if ( Settings.IsAppendEmail() ) {
				/* bResult = */ OpenCaseFiles( strPrefix, inTargetAddress.GetAddress(), inCase, ayFileGroup );
			}
			else {
				/* bResult = */ OpenCaseFiles( strPrefix, inCase, ayFileGroup );
			}

			CStreamFileItem&	file( ayFileGroup.at( 0 ) );
			if ( !file.IsNull() ) {
				if ( pPenInfo->WritePenEml( inTargetAddress, file.GetFile() ) ) {
					m_Context.NoteTarget( bLogMessages, strAddress,
						L"Email Capture - Saved SMTP Pen email " );
				}
			}
		}
#if (0)
		if ( Settings.IsPenText() ) {
			CFileEx*	pFile( GetTextReportFile( strAddress, inCase ) );
			if ( pFile != NULL ) {
				//const CReportMode&	Mode( Settings.GetReportMode() );
				if ( pPenInfo->WritePenText( inTargetAddress, pFile ) ) {
					m_Context.NoteTarget( bLogMessages, strAddress,
						L"Email Capture - Wrote SMTP Pen Text entry " );
				}
			}
		}
#endif

#if (0)
		if ( Settings.IsPenXml() ) {
			CFileEx*	pFile = GetXmlReportFile( strAddress, inCase );
			if ( pFile != NULL ) {
				//const CReportMode&	Mode = Settings.GetReportMode();
				if ( pPenInfo->WritePenXml( inTargetAddress, pFile ) ) {
					m_Context.NoteTarget( bLogMessages, strAddress,
						L"Email Capture - Wrote SMTP Pen XML entry " );
				}
			}
		}
#endif
	}
	catch ( ... ) {
		m_Context.NoteMalformed( CEmailStats::kStat_TypeSmtp );
	}
}


// ----------------------------------------------------------------------------
//		FullAnalysis
// ----------------------------------------------------------------------------

void
CSmtpMsgStream::FullAnalysis(
	const CCase&	inCase )
{
	try {
		const CCaseSettings	Settings( inCase.GetSettings() );

		bool	bLogMessages( Settings.IsLogMessages() );
		m_Context.Increment( CEmailStats::kStat_SmtpFullEmailsCaptured );

		CPenInfo*			pPenInfo( GetPenInfo() );
		const CPeekString&	strPrefix( Settings.GetFileNamePrefix() );
		CPeekString			strAddressList;

#if (0)
		tFileNameType	fntEmail =
#ifdef OPT_BDAT
			(m_MsgFormat == kMsgFormat_Binary) ? kFileTypeBdat : kFileTypeEml;
#else
			kFileTypeEml;
#endif
#endif

		CStreamFileGroup	ayFileGroup;
		if ( Settings.IsFullEmail() ) ayFileGroup.push_back( CStreamFileItem( kFileTypeEml ) );
		if ( Settings.IsFullPacket() ) ayFileGroup.push_back( CStreamFileItem( Settings.GetPacketFileFormat() ) );
		if ( !ayFileGroup.empty() ) {
			// bool	bResult( false );
			/* bResult = */ OpenCaseFiles( strPrefix, inCase, ayFileGroup );

			if ( Settings.IsFullEmail() ) {
				CStreamFileItem&	file( ayFileGroup.Find( kFileTypeEml ) );
				if ( !file.IsNull() ) {
					if ( WritePayload( file.GetFile() ) ) {
						strAddressList = pPenInfo->FormatAddressList();
						m_Context.NoteTarget( bLogMessages, strAddressList,
							L"Email Capture - Saved SMTP Full email " );
					}
				}
			}
			if ( Settings.IsFullPacket() ) {
				CStreamFileItem&	file( ayFileGroup.Find( Settings.GetPacketFileFormat() ) );
				if ( !file.IsNull() ) {
					// This is a hack, since CaptureBuffer will not write to an open file.
					// But will it write to an existing file...
					CPeekString	strFileName = file.GetName();
					file.GetFile().Close();
					if ( WritePackets( strFileName ) ) {
						if ( strAddressList.IsEmpty() ) {
							strAddressList = pPenInfo->FormatAddressList();
						}
						m_Context.NoteTarget( bLogMessages,  strAddressList,
							L"Email Capture - Saved SMTP Full pkt file " );
					}
					else {
						CPeekString	strMsg( L"Email Capture - Failed to save SMTP Full pkt file " );
						strMsg.Append( strFileName );
						m_Context.NoteFailure( strMsg );
					}
				}
			}
		}
	}
	catch ( ... ) {
		m_Context.NoteMalformed( CEmailStats::kStat_TypeSmtp );
	}
}

void
CSmtpMsgStream::FullAnalysis(
	const CCase&			inCase,
	const CTargetAddress&	inTargetAddress )
{
	try {
		const CCaseSettings	Settings( inCase.GetSettings() );

		bool	bLogMessages( Settings.IsLogMessages() );
		m_Context.Increment( CEmailStats::kStat_SmtpFullEmailsCaptured );

		CPenInfo*			pPenInfo( GetPenInfo() );
		const CPeekString&	strPrefix( Settings.GetFileNamePrefix() );
		const CPeekString&	strAddress( inTargetAddress.GetAddress() );

#if (0)
		tFileNameType	fntEmail =
#ifdef OPT_BDAT
			(m_MsgFormat == kMsgFormat_Binary) ? kFileTypeBdat : kFileTypeEml;
#else
			kFileTypeEml;
#endif
#endif

		CStreamFileGroup	ayFileGroup;
		if ( Settings.IsFullEmail() ) ayFileGroup.push_back( CStreamFileItem( kFileTypeEml ) );
		if ( Settings.IsFullPacket() ) ayFileGroup.push_back( CStreamFileItem( Settings.GetPacketFileFormat() ) );
		if ( !ayFileGroup.empty() ) {
			// bool	bResult( false );
			if ( Settings.IsAppendEmail() ) {
				/* bResult = */ OpenCaseFiles( strPrefix, inTargetAddress.GetAddress(), inCase, ayFileGroup );
			}
			else {
				/* bResult = */ OpenCaseFiles( strPrefix, inCase, ayFileGroup );
			}

			if ( Settings.IsFullEmail() ) {
				CStreamFileItem&	file( ayFileGroup.Find( kFileTypeEml ) );
				if ( !file.IsNull() ) {
					if ( pPenInfo->WriteFullEml( inTargetAddress, file.GetFile() ) ) {
						m_Context.NoteTarget( bLogMessages,  strAddress,
							L"Email Capture - Saved SMTP Full email " );
					}
				}
			}
			if ( Settings.IsFullPacket() ) {
				CStreamFileItem&	file( ayFileGroup.Find( Settings.GetPacketFileFormat() ) );
				if ( !file.IsNull() ) {
					// This is a hack, since CaptureBuffer will not write to an open file.
					// But will it write to an existing file...
					CPeekString	strFileName = file.GetName();
					file.GetFile().Close();
					if ( pPenInfo->WriteFullPkt( inTargetAddress, strFileName ) ) {
						m_Context.NoteTarget( bLogMessages,  strAddress,
							L"Email Capture - Saved SMTP Full pkt file " );
					}
				}
			}
		}
	}
	catch ( ... ) {
		m_Context.NoteMalformed( CEmailStats::kStat_TypeSmtp );
	}
}


// ----------------------------------------------------------------------------
//		GetMsgStringA
// ----------------------------------------------------------------------------

CPeekStringA&
CSmtpMsgStream::GetMsgStringA()
{
	if ( !m_strMsg.IsEmpty() && !m_bDirty ) {
		return m_strMsg;
	}

	UInt32	nFirst( 0 );
	UInt32	nLast( 0 );
	// UInt32	nWrap( 0 );

	m_strMsg.Empty();

	size_t	nCount( m_MsgPacketList.size() );
	if ( nCount == 0 ) return m_strMsg;

	for ( size_t i = 0; i < nCount; i++ ) {
		const CMsgPacket&	miItem( *m_MsgPacketList[i] );
		if ( miItem.IsToServer() ) {
			nFirst = miItem.GetFirstSequenceNumber();
			nLast = miItem.GetLastSequenceNumber();
			break;
		}
	}
#ifdef DEBUG
	size_t	nServerCount( 0 );
#endif // DEBUG
	for ( size_t i = 1; i < nCount; i++ ) {
		const CMsgPacket&	miItem( *m_MsgPacketList[i] );
		if ( miItem.IsToServer() ) {
#ifdef DEBUG
			nServerCount++;
#endif // DEBUG
			if ( miItem.GetFirstSequenceNumber() < nFirst ) {
				nFirst = miItem.GetFirstSequenceNumber();
			}
			if ( miItem.GetLastSequenceNumber() > nLast ) {
				nLast = miItem.GetLastSequenceNumber();
			}
			if (miItem.DoesSequenceNumberWrap() ) {
				/* nWrap = */ miItem.GetLastSequenceNumber();
			}
		}
	}

	if ( nLast <= nFirst ) return m_strMsg;
	size_t	nMsgLen( nLast - nFirst );

	const size_t	nMaxLen( CalculateMaxLength( kDirection_ToServer ) );
	if ( nMsgLen > nMaxLen ) return m_strMsg;	// Sequence Numbers wrapped.

	// Any gaps will be filled with spaces.
	m_strMsg.assign( nMsgLen, ' ' );

	for ( size_t i = 0; i < nCount; i++ ) {
		const CMsgPacket&	miItem( *m_MsgPacketList[i] );
		if ( miItem.IsToServer() ) {
			ptrdiff_t	nOffset( miItem.GetFirstSequenceNumber() - nFirst );
			size_t		nLength( static_cast<UInt32>( miItem.GetLength() ) );
			m_strMsg.replace( nOffset, nLength, miItem.GetStringA() );
		}
	}

	m_bDirty = false;
	m_bVerified = false;
	return m_strMsg;
}


// ----------------------------------------------------------------------------
//		VerifyTextMsg
// ----------------------------------------------------------------------------

bool
CSmtpMsgStream::VerifyTextMsg()
{
	if ( m_bVerified ) return true;

	CPeekStringA&	strMsg( GetMsgStringA() );

	if ( strMsg.IsEmpty() ) return false;

	m_nCommandsIndex = 0;	// there are no Responses.
	m_nMessageIndex = strMsg.Find( "DATA\r\n" );
	m_nBodyIndex = strMsg.Find( "\r\n\r\n" );
	m_nEndIndex = strMsg.Find( "\r\n.\r\n" );

	if ( (m_nMessageIndex < 0)            ||
		 (m_nBodyIndex < 0)               ||
		 (m_nEndIndex < 0)                ||
		 (m_nMessageIndex > m_nBodyIndex) ||
		 (m_nMessageIndex > m_nEndIndex)  ||
		 (m_nBodyIndex > m_nEndIndex)   ) {
		return false;
	}

	m_nMessageIndex += 6;	// sizeof DATA\r\n
	m_nBodyIndex += 4;		// sizeof \r\n\r\n
	m_nEndIndex += 5;		// sizeof \r\n.\r\n
	
	m_strPenMsg = strMsg.Left( m_nBodyIndex );

	m_bVerified = true;
	
	return true;
}


// ----------------------------------------------------------------------------
//		VerifyBinaryMsg
// ----------------------------------------------------------------------------

bool
CSmtpMsgStream::VerifyBinaryMsg()
{
	CPeekStream&	osMsg( GetByteStream( kDirection_ToServer ) );
	UInt32			nMsgLength( osMsg.GetLength32() );
	if ( nMsgLength == 0 ) {
		return false;
	}
	const char*	pMsg( reinterpret_cast<const char*>( osMsg.GetData() ) );
	const char*	pMsgEnd( pMsg + nMsgLength );

	m_nMessageIndex = 0;
	m_nBodyIndex = 0;

	const char*	pBody( pMsg );
	while ( (pBody < pMsgEnd) && ((m_nMessageIndex == 0) || (m_nBodyIndex == 0)) ) {
		pBody = static_cast<const char*>( memchr( pBody, '\r', (pMsgEnd - pBody) ) );
		if ( pBody == NULL ) break;
#if (0)
		if ( _strnicmp( pBody, "\r\nBDAT ", 7 ) == 0 ) {
			pBody += 2;		// skip the \r\n.

			const char* pLineEnd( static_cast<const char*>( memchr( pBody, '\r', (pMsgEnd - pBody) ) ) );
			pBody = pLineEnd + 2;	// skip the /r/n.
			ptrdiff_t	nIndex( pBody - pMsg );
			if ( nIndex < kMaxUInt32 ) {
				m_nMessageIndex = nIndex & 0x0FFFFFFFF;
			}
		}
		else if ( strncmp( pBody, "\r\n\r\n", 4 ) == 0 ) {
			pBody += 4;
			ptrdiff_t	nIndex( pBody - pMsg );
			if ( nIndex < kMaxUInt32 ) {
				m_nBodyIndex = nIndex & 0x0FFFFFFFF;
			}
		}
		else {
			pBody++;
		}
#else
		pBody++;
#endif
	}
	if ( (pBody == NULL) || (pBody >= pMsgEnd) ) return false;
	if ( (m_nMessageIndex == 0) || (m_nBodyIndex == 0) ) return false;

	if ( m_nBodyIndex > 0 ) {
		m_strPenMsg.assign( pMsg, m_nBodyIndex );
	}

	m_nCommandsIndex = 0;	// there are no Responses.
	m_nEndIndex = nMsgLength;

	m_bVerified = true;

	return true;
}


// ----------------------------------------------------------------------------
//		VerifyMsg
// ----------------------------------------------------------------------------

bool
CSmtpMsgStream::VerifyMsg()
{
	if ( m_bVerified ) return true;

#ifdef OPT_BDAT
	if ( m_MsgFormat == kMsgFormat_Binary ) {
		return VerifyBinaryMsg();
	}
#endif
	return VerifyTextMsg();
}


// ----------------------------------------------------------------------------
//		Detect
// ----------------------------------------------------------------------------

bool
CSmtpMsgStream::Detect(
	const CCaseList&	inCaseList,
	const CMsgPacket*	inMsgPacket,
	CMsgAddress&		outTarget,
	COmniEngineContext*	/*pContext*/,
	size_t&				outIndex )
{
	ASSERT( inMsgPacket != NULL );
	if ( inMsgPacket == NULL ) return false;

	// Msg must start with either "MAIL FROM:" or "RCPT TO:"
	//  and end with CRLF.
	// Since the first item to not be a Mail From or Rcpt To
	//  causes false to be returned, the above is covered.

	CMsgTextList	MsgTextList;
	size_t			nItemCount;

	nItemCount = MsgTextList.Parse( inMsgPacket->GetStringA() );

	for ( size_t i = 0; i < nItemCount; i++ ) {
		const CMsgText*	pMsgText( &MsgTextList.at( i ) );
		if ( pMsgText == NULL ) {
			continue;
		}

		bool	bMailFrom( pMsgText->IsMailFrom() );
		bool	bMailAddress( bMailFrom );

		// All this is to keep from call IsRcptTo twice when
		// the first line is a RCPT TO.
		if ( i == 0 ) {
			if ( !bMailAddress ) {
				bMailAddress = pMsgText->IsRcptTo();
				if ( !bMailAddress ) break;
			}
		}
		else {
			bMailAddress |= pMsgText->IsRcptTo();
		}

		if ( bMailAddress ) {
			CPeekString	strAddress( pMsgText->GetAddress() );
			bool		bEmptyAddress( strAddress.IsEmpty() );
			if ( !bEmptyAddress ) {
				size_t	nCaseIndex( kIndex_Invalid );
				if ( inCaseList.IsAddressOfInterest( strAddress, nCaseIndex ) ) {
					outTarget.SetAddress( strAddress );
					outTarget.SetType(
						(bMailFrom)
						? CMsgAddress::kType_From
						: CMsgAddress::kType_To );
					outIndex = nCaseIndex;
					return true;	// return on first target found.
				}
			}
		}
	}
	return false;
}


// ----------------------------------------------------------------------------
//		Analyze
// ----------------------------------------------------------------------------

void
CSmtpMsgStream::Analyze(
	const CCase&	inCase )
{
	if ( !m_bVerified ) return;

	if ( inCase.GetSettings().IsMode( kMode_Pen ) ) {
		PenAnalysis( inCase );
	}
	else {
		FullAnalysis( inCase );
	}
}


void
CSmtpMsgStream::Analyze(
	const CCase&			inCase,
	const CTargetAddress&	inTargetAddress )
{
	if ( !m_bVerified ) return;

	bool	bTargetOfInterest( HasTarget( inTargetAddress ) );
	if ( !bTargetOfInterest ) return;

	if ( inCase.GetSettings().IsMode( kMode_Pen ) ) {
		PenAnalysis( inCase, inTargetAddress );
	}
	else {
		FullAnalysis( inCase, inTargetAddress );
	}
}

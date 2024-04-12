// ============================================================================
//	Pop3MsgStream.cpp
//		implementation of the CPop3MsgStream class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "Options.h"
#include "Case.h"
#include "Pop3MsgStream.h"
#include "PenInfo.h"
#include "OmniEngineContext.h"


// ============================================================================
//		CPop3MsgStream
// ============================================================================

CPop3MsgStream::CPop3MsgStream(
	const CIpAddressPortPair&	inIpPortPair,
	COmniEngineContext&			inContext,
	UInt32						inIndex )
	:	CMsgStream( kProtocol_POP3, inIpPortPair, inContext, inIndex, kMsgState_Text )
{
}


// ----------------------------------------------------------------------------
//		PenAnalysis
// ----------------------------------------------------------------------------

void
CPop3MsgStream::PenAnalysis(
	const CCase&	inCase )
{
	try {
		const CCaseSettings	Settings = inCase.GetSettings();

		bool	bLogMessages( Settings.IsLogMessages() );
		m_Context.Increment( CEmailStats::kStat_Pop3PenEmailsCaptured );

		CPenInfo*			pPenInfo( GetPenInfo() );
		const CPeekString&	strPrefix( Settings.GetFileNamePrefix() );
		CPeekString			strAddressList;

		if ( Settings.IsPenEmail() ) {
			CStreamFileGroup	ayFileGroup;
			ayFileGroup.Add( CStreamFileItem( kFileTypeEml ) );

			bool	bResult( false );
			bResult = OpenCaseFiles( strPrefix, inCase, ayFileGroup );

			CStreamFileItem&	file( ayFileGroup.GetAt( 0 ) );
			if ( !file.IsNull() ) {
				if ( pPenInfo->WritePenEml( file.GetFile() ) ) {
					strAddressList = pPenInfo->FormatAddressList();
					m_Context.NoteTarget( bLogMessages, strAddressList,
						L"Email Capture - Saved POP3 Pen email " );
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
						L"Email Capture - Wrote POP3 Pen Text entry " );
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
						L"Email Capture - Wrote POP3 Pen Xml entry " );
				}
			}
			else {
				NoteFileError( strPrefix );	// Need a file name.
			}
		}
#endif
	}
	catch ( ... ) {
		m_Context.NoteMalformed( CEmailStats::kStat_TypePop3 );
	}
}

void
CPop3MsgStream::PenAnalysis(
	const CCase&			inCase,
	const CTargetAddress&	inTargetAddress )
{
	try {
		const CCaseSettings	Settings( inCase.GetSettings() );

		bool	bLogMessages( Settings.IsLogMessages() );
		m_Context.Increment( CEmailStats::kStat_Pop3PenEmailsCaptured );

		CPenInfo*			pPenInfo( GetPenInfo() );
		const CPeekString&	strPrefix( Settings.GetFileNamePrefix() );
		const CPeekString&	strAddress( inTargetAddress.GetAddress() );

		if ( Settings.IsPenEmail() ) {
			CStreamFileGroup	ayFileGroup;
			ayFileGroup.Add( CStreamFileItem( kFileTypeEml ) );

			bool	bResult( false );
			if ( Settings.IsAppendEmail() ) {
				bResult = OpenCaseFiles( strPrefix, inTargetAddress.GetAddress(), inCase, ayFileGroup );
			}
			else {
				bResult = OpenCaseFiles( strPrefix, inCase, ayFileGroup );
			}

			CStreamFileItem&	file( ayFileGroup.GetAt( 0 ) );
			if ( !file.IsNull() ) {
				if ( pPenInfo->WritePenEml( inTargetAddress, file.GetFile() ) ) {
					m_Context.NoteTarget( bLogMessages, strAddress,
						L"Email Capture - Saved POP3 Pen email " );
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
						L"Email Capture - Wrote POP3 Pen Text entry " );
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
						L"Email Capture - Wrote POP3 Pen XML entry " );
				}
			}
		}
#endif
	}
	catch ( ... ) {
		m_Context.NoteMalformed( CEmailStats::kStat_TypePop3 );
	}
}


// ----------------------------------------------------------------------------
//		FullAnalysis
// ----------------------------------------------------------------------------

void
CPop3MsgStream::FullAnalysis(
	const CCase&	inCase )
{
	// Write the Msg to disk
	try {
		const CCaseSettings	Settings( inCase.GetSettings() );

		bool	bLogMessages( Settings.IsLogMessages() );
		m_Context.Increment( CEmailStats::kStat_Pop3FullEmailsCaptured );

		CPenInfo*			pPenInfo( GetPenInfo() );
		const CPeekString&	strPrefix( Settings.GetFileNamePrefix() );
		CPeekString			strAddressList;

		CStreamFileGroup	ayFileGroup;
		if ( Settings.IsFullEmail() ) ayFileGroup.Add( CStreamFileItem( kFileTypeEml ) );
		if ( Settings.IsFullPacket() ) ayFileGroup.Add( CStreamFileItem( Settings.GetPacketFileFormat() ) );
		if ( !ayFileGroup.IsEmpty() ) {
			bool	bResult( false );
			bResult = OpenCaseFiles( strPrefix, inCase, ayFileGroup );

			if ( Settings.IsFullEmail() ) {
				CStreamFileItem&	file( ayFileGroup.Find( kFileTypeEml ) );
				if ( !file.IsNull() ) {
					if ( WriteString( file.GetFile() ) ) {
						strAddressList = pPenInfo->FormatAddressList();
						m_Context.NoteTarget( bLogMessages, strAddressList,
							L"Email Capture - Saved POP3 Full email " );
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
							L"Email Capture - Saved POP3 Full pkt file " );
					}
					else {
						CPeekString	strMsg( L"Email Capture - Failed to save POP3 Full pkt file " );
						strMsg.Append( strFileName );
						m_Context.NoteFailure( strMsg );
					}
				}
			}
		}
	}
	catch ( ... ) {
		m_Context.NoteMalformed( CEmailStats::kStat_TypePop3 );
	}
}

void
CPop3MsgStream::FullAnalysis(
	const CCase&			inCase,
	const CTargetAddress&	inTargetAddress )
{
	// Write the Msg to disk
	try {
		const CCaseSettings	Settings( inCase.GetSettings() );

		bool	bLogMessages( Settings.IsLogMessages() );
		m_Context.Increment( CEmailStats::kStat_Pop3FullEmailsCaptured );

		CPenInfo*			pPenInfo( GetPenInfo() );
		const CPeekString&	strPrefix( Settings.GetFileNamePrefix() );
		const CPeekString&	strAddress( inTargetAddress.GetAddress() );

		CStreamFileGroup	ayFileGroup;
		if ( Settings.IsFullEmail() ) ayFileGroup.Add( CStreamFileItem( kFileTypeEml ) );
		if ( Settings.IsFullPacket() ) ayFileGroup.Add( CStreamFileItem( Settings.GetPacketFileFormat() ) );
		if ( !ayFileGroup.IsEmpty() ) {
			bool	bResult( false );
			if ( Settings.IsAppendEmail() ) {
				bResult = OpenCaseFiles( strPrefix, inTargetAddress.GetAddress(), inCase, ayFileGroup );
			}
			else {
				bResult = OpenCaseFiles( strPrefix, inCase, ayFileGroup );
			}

			if ( Settings.IsFullEmail() ) {
				CStreamFileItem&	file( ayFileGroup.Find( kFileTypeEml ) );
				if ( !file.IsNull() ) {
					if ( pPenInfo->WriteFullEml( inTargetAddress, file.GetFile() ) ) {
						m_Context.NoteTarget( bLogMessages,  strAddress,
							L"Email Capture - Saved POP3 Full email " );
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
							L"Email Capture - Saved POP3 Full pkt file " );
					}
				}
			}
		}
	}
	catch ( ... ) {
		m_Context.NoteMalformed( CEmailStats::kStat_TypePop3 );
	}
}


// ----------------------------------------------------------------------------
//		GetMsgStringA
// ----------------------------------------------------------------------------

CPeekStringA&
CPop3MsgStream::GetMsgStringA()
{
	if ( !m_strMsg.IsEmpty() && !m_bDirty ) {
		return m_strMsg;
	}

	m_strMsg.Empty();

	size_t	nCount( m_MsgPacketList.GetCount() );
	if ( nCount == 0 ) return m_strMsg;

	UInt32	nFirst( 0 );
	UInt32	nLast( 0 );
#ifdef DEBUG
	UInt32	nWrap( 0 );
#endif

	for ( size_t i = 0; i < nCount; i++ ) {
		const CMsgPacket&	miItem( *m_MsgPacketList[i] );
		if ( miItem.IsFromServer() ) {
			nFirst = m_MsgPacketList[i]->GetFirstSequenceNumber();
			nLast = m_MsgPacketList[i]->GetLastSequenceNumber();
			break;
		}
	}
	for ( size_t i = 1; i < nCount; i++ ) {
		const CMsgPacket&	miItem( *m_MsgPacketList[i] );
		if ( miItem.IsFromServer() ) {
			if ( miItem.GetFirstSequenceNumber() < nFirst ) {
				nFirst = miItem.GetFirstSequenceNumber();
			}
			if ( miItem.GetLastSequenceNumber() > nLast ) {
				nLast = miItem.GetLastSequenceNumber();
			}
#ifdef DEBUG
			if ( miItem.DoesSequenceNumberWrap() ) {
				nWrap = miItem.GetLastSequenceNumber();
			}
#endif
		}
	}

	if ( nLast <= nFirst ) return m_strMsg;
	size_t	nMsgLen( nLast - nFirst );

	const size_t	nMaxLen( CalculateMaxLength( kDirection_FromServer ) );
	if ( nMsgLen > nMaxLen ) return m_strMsg;	// Sequence Numbers wrapped.

	// Any gaps will be filled with spaces.
	m_strMsg.assign( nMsgLen, ' ' );

	for ( size_t i = 0; i < nCount; i++ ) {
		const CMsgPacket&	miItem( *m_MsgPacketList[i] );
		if ( miItem.IsFromServer() ) {
			ptrdiff_t	nOffset( miItem.GetFirstSequenceNumber() - nFirst );
			size_t		nLength( miItem.GetLength() );
			m_strMsg.replace( nOffset, nLength, miItem.GetStringA() );
		}
	}

	m_bDirty = false;
	m_bVerified = false;
	return m_strMsg;
}


// ----------------------------------------------------------------------------
//		VerifyMsg
// ----------------------------------------------------------------------------

bool
CPop3MsgStream::VerifyMsg()
{
	if ( m_bVerified ) return true;
	CPeekStringA&	strMsg( GetMsgStringA() );

	if ( strMsg.IsEmpty() ) return false;

	m_nCommandsIndex = 0;
	m_nMessageIndex = 0;
	m_nBodyIndex = strMsg.Find( "\r\n\r\n" );
	m_nEndIndex = strMsg.Find( "\r\n.\r\n" );

	if ( (m_nBodyIndex < 0)           ||
		 (m_nEndIndex < 0)            ||
		 (m_nBodyIndex > m_nEndIndex) ) {
		return false;
	}

	m_nBodyIndex += 4;	// size of \r\n\r\n
	m_nEndIndex += 5;	// size of \r\n.\r\n

	m_strPenMsg = strMsg.Left( m_nBodyIndex );
	CMsgText	strPenMsgLower( m_strPenMsg );
	strPenMsgLower.MakeLower();
	
	size_t	nPlusOKIndex( kIndex_Invalid );
	size_t	nSubjectIndex( kIndex_Invalid );
	if ( strPenMsgLower.Find( "subject:", nSubjectIndex ) ) {
		CMsgText	strPreHdr( strPenMsgLower.Left( static_cast<int>( nSubjectIndex ) ) );
		nPlusOKIndex = strPreHdr.FindLast( "+ok" );
	}
	else {
		nPlusOKIndex = strPenMsgLower.FindLast( "+ok" );
	}
	if ( nPlusOKIndex == kIndex_Invalid ) {
		return false;
	}

	m_nMessageIndex = nPlusOKIndex;
	if ( !strPenMsgLower.Find( "\r\n", m_nMessageIndex ) ) {
		return false;
	}

	m_nMessageIndex += 2;	// sizeof \r\n
	m_nCommandsIndex = m_nMessageIndex;
	if ( m_nCommandsIndex < m_nBodyIndex ) {
		size_t	nPenSize( m_nBodyIndex -  m_nCommandsIndex );
		m_strPenMsg = strMsg.Mid( m_nCommandsIndex, nPenSize );
	}
	else {
		m_strPenMsg = strMsg.Left( m_nBodyIndex );
	}
	m_bVerified = true;
	return true;
}


// ----------------------------------------------------------------------------
//		Analyze
// ----------------------------------------------------------------------------

void
CPop3MsgStream::Analyze(
	const CCase&	inCase )
{
	if ( !m_bVerified ) return;

	if ( inCase.GetSettings().IsMode( kMode_Pen ) ) {
		PenAnalysis( inCase );
	}
	else {
		FullAnalysis( inCase );
	}
	return;
}

void
CPop3MsgStream::Analyze(
	const CCase&			inCase,
	const CTargetAddress&	inTargetAddress )
{
	if ( !m_bVerified ) return;

	if ( Detect( inTargetAddress ) ) {
		if ( inCase.GetSettings().IsMode( kMode_Pen ) ) {
			PenAnalysis( inCase, inTargetAddress );
		}
		else {
			FullAnalysis( inCase, inTargetAddress );
		}
	}
	return;
}

// ============================================================================
//	MsgStream.cpp
//		implementation of the CMsgStream class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "MsgStream.h"
#include "GlobalTags.h"
#include "OmniEngineContext.h"
#include "FileMap.h"
#include "PenFileEx.h"
#include "CaptureBuffer.h"
#include <memory>
#include <vector>

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// ============================================================================
//		Statics
// ============================================================================

CMsgStream::CStreamFileItem	CMsgStream::s_NullFileItem( kFileTypeMax );


// ============================================================================
//		CMsgStream
// ============================================================================

CMsgStream::CMsgStream(
	tProtocolTypes				inType,
	const CIpAddressPortPair&	inIpPortPair,
	COmniEngineContext&			inContext,
	size_t						inIndex,
	const CMsgAddress&			inTarget,
	kMsgStates					inState )
	:	m_ProtocolType( inType )
	,	m_pMsgStreamList( NULL )
	,	m_Context( inContext )
	,	m_nTargetIndex( inIndex )
	,	m_MsgState( inState )
	,	m_MsgFormat( kMsgFormat_Unknown )
	,	m_MsgBlock( kMsgBlock_Unknown )
	,	m_bOpen( true )
	,	m_bEOMReceived( false )
	,	m_IpPortPair( inIpPortPair )
	,	m_LastPacketTime( WPTIME_INVALID )
	,	m_bDirty( true )
	,	m_bVerified( false )
	,	m_nCommandsIndex( 0 )
	,	m_nMessageIndex( 0 )
	,	m_nBodyIndex( 0 )
	,	m_nEndIndex( 0 )
{
	m_strIdA = inTarget.FormatA();
}

CMsgStream::CMsgStream(
	tProtocolTypes				inType,
	const CIpAddressPortPair&	inIpPortPair,
	COmniEngineContext&			inContext,
	size_t						inIndex,
	kMsgStates					inState )
	:	m_ProtocolType( inType )
	,	m_pMsgStreamList( NULL )
	,	m_Context( inContext )
	,	m_nTargetIndex( inIndex )
	,	m_MsgState( inState )
	,	m_MsgFormat( kMsgFormat_Unknown )
	,	m_MsgBlock( kMsgBlock_Unknown )
	,	m_bOpen( true )
	,	m_bEOMReceived( false )
	,	m_IpPortPair( inIpPortPair )
	,	m_LastPacketTime( WPTIME_INVALID )
	,	m_bDirty( true )
	,	m_bVerified( false )
	,	m_nCommandsIndex( 0 )
	,	m_nMessageIndex( 0 )
	,	m_nBodyIndex( 0 )
	,	m_nEndIndex( 0 )
{
#if (TOVERIFY)
	{
		CPeekStringA	strAddressA( inIpPortPair.GetSrcAddress().FormatA() );
		CPeekStringA	strPortA( inIpPortPair.GetSrcPort().Format() );
		const CPeekStringA*	ayStrs[] = {
			&strAddressA,
			Tags::kxColon.RefA(),
			&strPortA,
			NULL
		};
		m_strIdA = FastCat( ayStrs );
	}
#else
	m_strIdA.Format( "%u:%u", inIpPortPair.GetSrcAddress(), inIpPortPair.GetSrcPort() );
#endif	// TOVERIFY
}


// ----------------------------------------------------------------------------
//		CalculateMaxLength
// ----------------------------------------------------------------------------

size_t
CMsgStream::CalculateMaxLength(
	MsgDirection inDirection )
{
	const size_t	nCount( m_MsgPacketList.size() );
	size_t			nMaxLength( 0 );
	for ( size_t i = 0; i < nCount; i++ ) {
		const CMsgPacket&	miItem( *m_MsgPacketList[i] );
		if ( miItem.IsDirection( inDirection ) ) {
			nMaxLength += miItem.GetLength();
		}
	}

	return nMaxLength;
}


// ----------------------------------------------------------------------------
//		OpenCaseFiles
// ----------------------------------------------------------------------------

bool
CMsgStream::OpenCaseFiles(
	const CPeekString&	inPrefix,
	const CCase&		inCase,
	CStreamFileGroup&	ioFiles )
{
	CPeekString	strPrefix( CFileEx::LegalizeFileName( inPrefix ) );
	CPeekString	strOutputDirectory( inCase.GetSettings().GetOutputDirectory().GetW() );

	if ( !CFileEx::MakePath( strOutputDirectory ) ) return false;

	for ( int x = 0; x < 100; x++ ) {
		CPeekString	strTimeStamp( CPeekTime::GetTimeStringFileName( true ) );
		const CPeekString*	ayStrs[] = {
			&strOutputDirectory,
			Tags::kxBackSlash.RefW(),
			&strPrefix,
			&strTimeStamp,
			NULL
		};
		CPeekString	strRootFileName( FastCat( ayStrs ) );

		UInt32	nFlags( CFileEx::modeNew | CFileEx::modeWrite );
		for ( size_t i = 0; i < ioFiles.size(); i++ ) {
			CStreamFileItem&	item( ioFiles.at( i ) );
			CPeekString	strFileName( strRootFileName );
			strFileName.Append( CFileItem::s_ayTypeExt.GetW( item.GetType() ) );
			item.SetName( strFileName );
		}
		
		if ( !ioFiles.Exists() ) {
			BOOL	bResult( TRUE );
			for ( size_t i = 0; i < ioFiles.size(); i++ ) {
				CStreamFileItem&	item( ioFiles.at( i ) );
				bResult = item.Open( nFlags );
				if ( bResult == FALSE ) {
					ioFiles.DeleteOpen();
					break;	// try outer loop again.
				}
			}
			if ( bResult ) {
				return true;
			}
		}
	}
	return false;
}


// ----------------------------------------------------------------------------
//		GetAttachment
// ----------------------------------------------------------------------------

bool
CMsgStream::GetAttachmentText(
	size_t&			ioPosition,
	CPeekStringA&	outAttachment )
{
	CPeekStringA	strMsg( GetPayload() );
	if ( strMsg.IsEmpty() ) return false;

	if ( ioPosition == 0 ) {
		ioPosition = NextBoundaryOffset( 0, NULL );
		if ( ioPosition < 0 ) return false;

		ioPosition = NextBoundaryOffset( ioPosition, NULL );
		if ( ioPosition < 0 ) return false;
	}

	size_t nPosAttachBegin( NextBoundaryOffset( ioPosition, NULL ) );
	if ( nPosAttachBegin < kIndex_Invalid ) return false;

	size_t	nPosAttachEnd( 0 );
	ioPosition = NextBoundaryOffset( nPosAttachBegin, &nPosAttachEnd );
	if ( ioPosition == kIndex_Invalid ) return false;

	const char*	pszEnd( strMsg.c_str() + nPosAttachEnd );
	while ( (nPosAttachEnd > nPosAttachBegin) && (*pszEnd != '\r') ) {
		pszEnd--;
		nPosAttachEnd--;
	}

	size_t nAttachLength( nPosAttachEnd - nPosAttachBegin );
	outAttachment = strMsg.Mid( nPosAttachBegin, nAttachLength );
	return true;
}


// ----------------------------------------------------------------------------
//		GetPenInfo
// ----------------------------------------------------------------------------

CPenInfo*
CMsgStream::GetPenInfo()
{
	if ( m_PenInfoPtr.get() == nullptr ) {
		m_PenInfoPtr = make_unique<CPenInfo>( CPenInfo( this ) );
		ASSERT( m_PenInfoPtr != nullptr );
		if ( m_PenInfoPtr == nullptr ) throw( 1 );
	}
	return m_PenInfoPtr.get();
}


// ----------------------------------------------------------------------------
//		GetByteStream
// ----------------------------------------------------------------------------

CPeekStream&
CMsgStream::GetByteStream(
	MsgDirection	inDirection )
{
	CPeekStream&	bsMsg( (inDirection == kDirection_ToServer) ? m_bsMsg[0] : m_bsMsg[1] );

	if ( bsMsg.GetLength() > 0 ) {
		return bsMsg;
	}

	UInt32	nFirst( 0 );
	UInt32	nLast( 0 );
//	UInt32	nWrap( 0 );
	size_t	nCount( m_MsgPacketList.size() );
	if ( nCount == 0 ) return bsMsg;

	for ( size_t i = 0; i < nCount; i++ ) {
		const CMsgPacket&	miItem( *m_MsgPacketList[i] );
		if ( miItem.IsDirection( inDirection ) ) {
			nFirst = m_MsgPacketList[0]->GetFirstSequenceNumber();
			nLast = m_MsgPacketList[0]->GetLastSequenceNumber();
			break;
		}
	}

	for ( size_t i = 0; i < nCount; i++ ) {
		const CMsgPacket&	miItem( *m_MsgPacketList[i] );
		if ( miItem.IsDirection( inDirection ) ) {
			if ( miItem.GetFirstSequenceNumber() < nFirst ) {
				nFirst = miItem.GetFirstSequenceNumber();
			}
			if ( miItem.GetLastSequenceNumber() > nLast ) {
				nLast = miItem.GetLastSequenceNumber();
			}
			if ( miItem.DoesSequenceNumberWrap() ) {
				/* nWrap = */ miItem.GetLastSequenceNumber();
			}
		}
	}

	// Don't copy the XEXCH50 block into the Message Byte Stream.
	UInt32	nSkipStart( m_Xexch50Block.GetSequenceStart() );
	UInt32	nSkipLength( m_Xexch50Block.GetSize() );

	if ( nFirst >= nLast ) return bsMsg;

	size_t	nMaxLen( m_MsgPacketList.GetTotalPacketLength() );
	UInt32	nMsgLen( nLast - nFirst );
	if ( nMsgLen == 0 ) return bsMsg;			// Should never happen...
	if ( nMsgLen > nMaxLen ) return bsMsg;		// Sequence Numbers wrapped?

	if ( nSkipLength > nMsgLen ) return bsMsg;
	nMsgLen -= nSkipLength;

	try {
		bsMsg.SetSize( nMsgLen, 0 );

		for ( size_t i = 0; i < nCount; i++ ) {
			const CMsgPacket&	miItem( *m_MsgPacketList[i] );
			if ( !miItem.IsDirection( inDirection ) ) continue;

			CByteVectRef	bvData( miItem.GetPayload() );
			if ( bvData.IsEmpty() ) continue;

			int		nOffset( -1 );
			UInt32	nStart( miItem.GetFirstSequenceNumber() );
			if ( nStart < nSkipStart ) {	// Is this before the XEXCH50 block.
				nOffset = nStart - nFirst;
			}
			else if ( nStart >= (nSkipStart + nSkipLength) ) {	// Is this after the XEXCH50 block.
				nOffset = nStart - nFirst - nSkipLength;
			}
			if ( nOffset >= 0 ) {		// Not set if within the XEXCH50 block.
				bsMsg.Seek( nOffset, STREAM_SEEK_SET );
				bsMsg.Write( bvData.GetCount(), bvData.GetData( bvData.GetCount() ) );
			}
		}
	}
	catch( ... ) {
		bsMsg.Reset();
		return bsMsg;
	}

	return bsMsg;
}


// ----------------------------------------------------------------------------
//		GetPayload
// ----------------------------------------------------------------------------

CPeekStringA
CMsgStream::GetPayload()
{
	CPeekStringA	strPayload;

#ifdef OPT_BDAT
	if ( m_MsgFormat == kMsgFormat_Binary ) {
		const CPeekStream&	bsMsg = GetByteStream( kDirection_ToServer );
		if ( bsMsg.GetCount() > 0 ) {
			PSTR	pPayload = strPayload.GetBufferSetLength( bsMsg.GetCount() );
			if ( pPayload ) {
				bsMsg.SetMarker( 0, CPeekStream::kMarker_Begin );
				bsMsg.ReadBytes( pPayload, bsMsg.GetCount() );
				strPayload.ReleaseBuffer( bsMsg.GetCount() );
			}
		}
	}
	else {
		strPayload = GetMsgStringA();
	}
#else
	strPayload = GetMsgStringA();
#endif
	return strPayload;
}


// ----------------------------------------------------------------------------
//		GetPenMsg
// ----------------------------------------------------------------------------

const CPeekStringA&
CMsgStream::GetPenMsg()
{
	// builds m_strMsg if needed and checks it.
	if ( !VerifyMsg() ) {
		m_strPenMsg.Empty();	
	}

	return m_strPenMsg;
}


// ----------------------------------------------------------------------------
//		GetMsgSizeInfo
// ----------------------------------------------------------------------------

bool
CMsgStream::GetMsgSizeInfo(
	tMsgSizeInfo&	outSizeInfo )
{
	if ( !VerifyMsg() ) return false;	// builds m_strMsg if needed and checks it.

	outSizeInfo.nMsgSize = m_nEndIndex - m_nMessageIndex;
	outSizeInfo.nHdrSize = m_nBodyIndex - m_nCommandsIndex;	// this includes Commands(?)

	return true;
}


// ----------------------------------------------------------------------------
//		ErrorFileName
// ----------------------------------------------------------------------------

CPeekString
CMsgStream::ErrorFileName(
	const CPeekString&	inType,
	const CPeekString&	inOutputDirectory )
{
	CPeekString	strName;

	CFileEx::MakePath( inOutputDirectory );

	int	x( 0 );
	do {
		if ( x++ > 1000 ) {
			strName.IsEmpty();
			break;
		}

		CPeekString	strTimeStamp = CPeekTime::GetTimeStringFileName( true );
#if (TOVERIFY)
		{
			const CPeekString*	ayStrs[] = {
				&inOutputDirectory,
				Tags::kxBackSlash.RefW(),
				&strTimeStamp,
				Tags::kxSpace.RefW(),
				&inType,
				NULL
			};
			strName = FastCat( ayStrs );
		}
#else
		strName.Format(
			L"%s\\%s %s",
			inOutputDirectory,
			strTimeStamp,
			inType );
#endif // TOVERIFY
	} while ( /* ::PathFileExists( strName ) */ false );

	return strName;
}


// ----------------------------------------------------------------------------
//		GetBoundaryStringA
// ----------------------------------------------------------------------------

const CPeekStringA&
CMsgStream::GetBoundaryStringA()
{
	if ( m_strBoundary.IsEmpty() ) {
		CMsgText	strMsg( GetMsgStringA() );

		CPeekStringA	strCT( "content-type:" );
		CPeekStringA	strMP( "multipart" );	// multipart/mixed multipart/related
		CPeekStringA	strBd( "boundary=" );

		size_t	nPos( 0 );
		if ( !strMsg.FindNoCase( strCT, nPos ) ) return m_strBoundary;
		nPos += strCT.GetLength() + 1;

		if ( !strMsg.FindNoCase( strMP, nPos ) ) return m_strBoundary;
		if ( !strMsg.Find( ";", nPos ) ) return m_strBoundary;
		if ( !strMsg.FindNoCase( strBd, nPos ) ) return m_strBoundary;
		nPos += strBd.GetLength() + 1;

		size_t	nPosBoundaryEnd( nPos );
		if ( !strMsg.Find( "\"", nPosBoundaryEnd ) ) return m_strBoundary;

		size_t	nBoundaryLength( nPosBoundaryEnd - nPos );
		m_strBoundary = strMsg.Mid( nPos, nBoundaryLength );
	}
	return m_strBoundary;
}


// ----------------------------------------------------------------------------
//		HasTarget
// ----------------------------------------------------------------------------

bool
CMsgStream::HasTarget(
	const CTargetAddress&	inTargetAddress )
{
	CPenInfo*	pPenInfo( GetPenInfo() );
	if ( pPenInfo ) {
		return pPenInfo->HasTarget( inTargetAddress );
	}
	return false;
}


// ----------------------------------------------------------------------------
//		NextBoundaryOffset
// ----------------------------------------------------------------------------
//  return the offset to the end of the boundary string.
//  outBoundaryBegins is set to the offset of where the boundary begins.

size_t
CMsgStream::NextBoundaryOffset(
	size_t	inPosition,
	size_t*	outBoundaryBegins )
{
	const CPeekStringA	strBoundary( GetBoundaryStringA() );
	if ( strBoundary.IsEmpty() ) return kIndex_Invalid;

	CPeekStringA	strMsg( GetPayload() );
	size_t			nPos( 0 );
	if ( !strMsg.Find( nPos, strBoundary, inPosition ) ) return kIndex_Invalid;
	if ( outBoundaryBegins != NULL ) {
		*outBoundaryBegins = nPos;
	}

	nPos += strBoundary.GetLength();
	while ( nPos < strMsg.GetLength() ) {
		if ( strMsg[nPos] == '\n' ) {
			nPos++;
			break;
		}
		nPos++;
	}
	if ( nPos > strMsg.GetLength() ) return kIndex_Invalid;
	return nPos;
}


// ----------------------------------------------------------------------------
//		NoteFileError
// ----------------------------------------------------------------------------

void
CMsgStream::NoteFileError(
	const CPeekString&	inFileName )
{
	m_Context.NoteFileError( inFileName );
}


// ----------------------------------------------------------------------------
//		NoteWriteError
// ----------------------------------------------------------------------------

void
CMsgStream::NoteWriteError(
	const CPeekString&	inFileName )
{
	m_Context.NoteWriteError( inFileName );
}


// ----------------------------------------------------------------------------
//		TestAndAddMsg
// ----------------------------------------------------------------------------

CMsgPacket*
CMsgStream::TestAndAddMsg(
	CMsgPacketPtr&	inMsgPacket )
{
	m_LastPacketTime.SetNow();
	if ( !inMsgPacket->IsEmpty() ) {
		m_LastBody.Append( inMsgPacket->GetStringA() );
		if ( m_LastBody.IsEndOfMsg() ) {
			m_bEOMReceived = true;
		}
		m_LastBody = inMsgPacket->GetStringA();
	}
#ifdef OPT_BDAT
	switch ( GetState() ) {
	case kMsgState_Open:
		if ( inMsgPacket->IsData() ) {
			SetMsgFormat( kMsgFormat_Text );
			SetMsgState( kMsgState_Text );
			inMsgPacket->SetType( CMsgPacket::kPacketType_Text );
		}
		else if ( inMsgPacket->IsBdat() ) {
			SetMsgFormat( kMsgFormat_Binary );
			CMsgText	txt( inMsgPacket->GetStringA() );
			UInt32		nSize( 0 );
			bool		bLast( false );
			if ( txt.GetBdatInfo( nSize, bLast ) ) {
				SetMsgState( kMsgState_BDAT );
				inMsgPacket->SetType( CMsgPacket::kPacketType_BDAT );
				m_BdatBlock.Start( inMsgPacket->GetLength(), inMsgPacket->GetFirstSequenceNumber(), nSize, bLast );
			}
		}
		else if ( inMsgPacket->IsXexch50() ) {
			CMsgText	txt( inMsgPacket->GetStringA() );
			UInt32		nSize( 0 );
			int			nCount( 0 );
			if ( txt.GetXexch50Info( nSize, nCount ) ) {
				SetMsgState( kMsgState_Xexch50Post );
				inMsgPacket->SetType( CMsgPacket::kPacketType_Xexch50 );
				m_Xexch50Block.Start( inMsgPacket->GetLength(), inMsgPacket->GetFirstSequenceNumber(), nSize, (nCount == 0) );
			}./Plugin/MsgStream.cpp:686:41:
		}
		break;

	case kMsgState_Text:
		inMsgPacket->SetType( CMsgPacket::kPacketType_Text );
		if ( !inMsgPacket->IsEmpty() ) {
			m_LastBody.Append( inMsgPacket->GetStringA() );
			if ( m_LastBody.IsEndOfMsg() ) {
				SetMsgState( kMsgState_Closed );
				m_bEOMReceived = true;
			}
			else {
				m_LastBody = inMsgPacket->GetStringA();
			}
		}
		break;

	case kMsgState_BDAT:
		{
			inMsgPacket->SetType( CMsgPacket::kPacketType_BDAT );
			UInt32	nResponse( 0 );
			if ( inMsgPacket->GetResponse( nResponse ) ) {
				// if response is not 250 OK, reject the email...
				if ( nResponse >= 500 ) {
					m_BdatBlock.Cancel();
				}
				if ( m_BdatBlock.IsLast() ) {
					SetMsgState( kMsgState_Closed );
					m_bEOMReceived = true;
				}
			}
		}
		break;

	case kMsgState_Xexch50Post:
		{
			inMsgPacket->SetType( CMsgPacket::kPacketType_Xexch50 );
			UInt32	nResponse( 0 );
			if ( inMsgPacket->GetResponse( nResponse ) ) {
				if ( nResponse >= 500 ) {./Plugin/MsgStream.cpp:686:41:
					SetMsgState( kMsgState_Open );
					m_Xexch50Block.Cancel();
				}
				else {	// nResponse should be 354...
					SetMsgState( kMsgState_Xexch50 );
				}
			}
		}
		break;

	case kMsgState_Xexch50:
		{
			inMsgPacket->SetType( CMsgPacket::kPacketType_Xexch50 );
			UInt32	nResponse( 0 );
			if ( inMsgPacket->GetResponse( nResponse ) ) {
				SetMsgState( kMsgState_Open );
				// Don't really care what the response is.
				// 250: OK
				// 5xx: Failure
			}
		}
		break;

	case kMsgState_Closed:
		{
			inMsgPacket->SetType( CMsgPacket::kPacketType_Text );
			CMsgText	txt( inMsgPacket->GetStringA() );
			UInt32		nResponse = 0;
			if ( txt.GetResponse( nResponse ) ) {
				if ( nResponse == 250 ) {
					m_bEOMReceived = true;
				}
			}
		}
		break;
	}

	//if ( m_MsgFormat == kMsgFormat_Unknown ) {
	//	if ( inMsgPacket->IsData() ) {
	//		m_MsgFormat = kMsgFormat_Text;
	//	}
	//	else if ( inMsgPacket->IsBdat() || inMsgPacket->IsXexch50() ) {
	//		m_MsgFormat = kMsgFormat_Binary;
	//	}
	//}
	//else if ( m_MsgFormat == kMsgFormat_Binary ) {
	//	CMsgText	txt( inMsgPacket->GetStringA() );
	//	UInt32		nResponse = 0;
	//	if ( txt.GetResponse( nResponse ) ) {
	//		if ( nResponse == 250 ) {
	//			m_bEOMReceived = true;
	//		}
	//	}
	//}
#endif

	// Add the Packet Data
	m_bDirty = true;
	m_bVerified = false;
	if ( inMsgPacket->IsClose() ) {
		m_bOpen = false;
	}
	m_MsgPacketList.push_back( make_unique<CMsgPacket>( inMsgPacket.release() ) );
	size_t	nIndex( m_MsgPacketList.size() );
	return m_MsgPacketList[nIndex].get();
}


// ----------------------------------------------------------------------------
//		WritePackets
// ----------------------------------------------------------------------------

bool
CMsgStream::WritePackets(
	const CPeekString&	inFileName )
{
	bool	bResult = true;
	size_t	nPacketCount( m_MsgPacketList.size() );
	if ( nPacketCount == 0 ) return true;

	try {
		UInt64			nBufferSize( m_MsgPacketList.GetBufferSize() );
		CCaptureBuffer	CaptureBuffer( nBufferSize );

		// Write each packet.
//		bool	fRetried = false;
		while ( CaptureBuffer.GetCount() < nPacketCount ) {
			for ( size_t i = 0; i < nPacketCount; i++ ) {
				// Get the packet.
				const CMsgPacket*	pMsgPacket( m_MsgPacketList.at( i ).get() );
				ASSERT( pMsgPacket );
				if ( pMsgPacket == NULL ) break;
				try {
					CaptureBuffer.WritePacket( pMsgPacket->GetPacket() );
				}
				catch ( ... ) {
					// Only one retry.
//					if ( fRetried ) {
						CaptureBuffer.Save( inFileName );
						return false;
//					}

// 					nBufferSize = CaptureBuffer.GetCapacity() + m_MsgPacketList.GetBufferSize( i );
// 					// SetCapacity clears the CaptureBuffer.
// 					CaptureBuffer.SetCapacity( nBufferSize );
// 					fRetried = true;
// 					break;
				}
			}
		}

		CaptureBuffer.Save( inFileName );
	}
	catch ( ... ) {
		return false;
	}

	return bResult;
}

#if 0
bool
CMsgStream::WritePackets(
	CFileEx& inFile )
{
	UInt32	nPacketCount( m_MsgPacketList.GetCount32() );
	if ( nPacketCount == 0 ) return true;

	inFile;
	tIntegrity	Integrity;
	m_Context.GetIntegrity( Integrity );
	try {
		CPeekFile	PeekFile( Integrity );

		BOOL	bResult( PeekFile.Attach( inFile.ReleaseFileHandle(), inFile.GetFileName() ) );
		if ( bResult == FALSE ) {
			PeekFile.Close();
			NoteFileError( inFile.GetFileName() );
			return false;
		}

		// Write each packet.
		for ( UInt32 i = 0; i < nPacketCount; i++ ) {
			// Get the packet.
			const CMsgPacket*	pMsgPacket( m_MsgPacketList.GetAt( i ) );
			ASSERT( pMsgPacket );
			if ( pMsgPacket == NULL ) break;
			if ( !PeekFile.AddMsg( pMsgPacket ) ) break;
		}

		// Close the file handle.
		PeekFile.Close();
	}
	catch ( ... ) {
		return false;
	}

	return true;
}
#endif


// ----------------------------------------------------------------------------
//		WriteString
// ----------------------------------------------------------------------------

bool
CMsgStream::WriteString(
	const CPeekString&	inFileName )
{
	CPeekStringA	strMsg( GetMsgStringA() );

	UInt32	nFlags( CFileEx::modeCreate | CFileEx::modeWrite );
	CFileEx	theFile;
	BOOL	bResult( theFile.Open( inFileName, nFlags ) );
	if ( bResult == FALSE ) {
		NoteFileError( inFileName );
		return false;
	}

	try {
		const char*	pMsg( strMsg.c_str() );
		size_t		nLength( strMsg.GetLength() );
		pMsg += m_nCommandsIndex;
		nLength -= m_nCommandsIndex;
		_ASSERTE( nLength < kMaxUInt32 );
		UInt32	nLength32 = nLength & kMaxUInt32;
		theFile.Write( nLength32, pMsg );
	}
	catch ( ... ) {
		NoteWriteError( inFileName );
		return false;
	}
	return true;
}

bool
CMsgStream::WriteString(
	CFileEx&	inFile )
{
	CPeekStringA	strMsg( GetMsgStringA() );

	if ( !inFile.IsOpen() ) {
		NoteFileError( inFile.GetFileName() );
		return false;
	}

	try {
		const char*	pMsg( strMsg.c_str() );
		size_t		nLength( strMsg.GetLength() );
		pMsg += m_nCommandsIndex;
		nLength -= m_nCommandsIndex;
		_ASSERTE( nLength < kMaxUInt32 );
		UInt32	nLength32( nLength & kMaxUInt32 );
		inFile.Write( nLength32, pMsg );
	}
	catch ( ... ) {
		NoteWriteError( inFile.GetFileName() );
		return false;
	}
	return true;
}


// ----------------------------------------------------------------------------
//		WritePayload
// ----------------------------------------------------------------------------

bool
CMsgStream::WritePayload(
	const CPeekString&	inFileName )
{
#ifdef OPT_BDAT
	if ( m_MsgFormat == kMsgFormat_Binary ) {
		const CPeekStream&	bsMsg = GetByteStream( kDirection_ToServer );
		return bsMsg.WriteToFile( inFileName );
	}
	else {
		return WriteString( inFileName );
	}
	return false;
#else
	return WriteString( inFileName );
#endif
}

bool
CMsgStream::WritePayload(
	CFileEx&	inFile )
{
#ifdef OPT_BDAT
	if ( m_MsgFormat == kMsgFormat_Binary ) {
		const CPeekStream&	bsMsg = GetByteStream( kDirection_ToServer );
		inFile.Write( bsMsg.GetData(), bsMsg.GetLength() );
		return true;
	}
	else {
		return WriteString( inFile );
	}
	return false;
#else
	return WriteString( inFile );
#endif
}

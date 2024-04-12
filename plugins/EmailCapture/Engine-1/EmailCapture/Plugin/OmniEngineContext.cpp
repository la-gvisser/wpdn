// =============================================================================
//	OmniEngineContext.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "OmniEngineContext.h"
#include "OmniPlugin.h"
#include "RemotePlugin.h"
#include "MemUtil.h"
#include "Packet.h"
#include "PacketArray.h"
#include "PluginMessages.h"
#include "PeekSummaryStats.h"

class CRemotePlugin;


// =============================================================================
//		Globals
// =============================================================================

enum {
	kLabel_SmtpOpen,
	kLabel_Pop3Open,
	kLabel_Max
};

static CPeekString	s_strLabels[] = {
    L"Open SMTP Emails",
    L"Open POP3 Emails"
};


// =============================================================================
//		COmniEngineContext
// =============================================================================

COmniEngineContext::COmniEngineContext(
	CGlobalId&		inId )
	:	CPeekEngineContext( inId )
	,	m_bDefaultOptions( true )
	,	m_nFailure( 0 )
{
}

COmniEngineContext::~COmniEngineContext()
{
}


// ----------------------------------------------------------------------------
//		AnalyzeAndDeleteMsgStream
// ----------------------------------------------------------------------------

void
COmniEngineContext::AnalyzeAndDeleteMsgStream(
	COptions*	inOptions,
	CMsgStream*	inMsgStream )
{
	ASSERT( inOptions != NULL );
	ASSERT( inMsgStream != NULL );
	if ( (inOptions == NULL) || (inMsgStream == NULL) ) return;

	inOptions->Analyze( inMsgStream );

	delete inMsgStream;
}


// ----------------------------------------------------------------------------
//		GetLogFileName
// ----------------------------------------------------------------------------

const CPeekString&
COmniEngineContext::GetLogFileName() const
{
	COmniPlugin*	pPlugin( GetPlugin() );
	_ASSERTE( pPlugin );

	if ( pPlugin ) {
		return pPlugin->GetLogFileName();
	}

	return Tags::kxInvalid.GetW();
}


// ----------------------------------------------------------------------------
//		NoteDetect
// ----------------------------------------------------------------------------

void
COmniEngineContext::NoteDetect(
	bool						inLogMessages, 
	CEmailStats::EmailStatType	inType )
{
	static CEmailStats::EmailStat	Detects[CEmailStats::kStat_TypeCount] = {
		CEmailStats::kStat_SmtpEmailsDetected,
		CEmailStats::kStat_Pop3EmailsDetected
	};

	Increment( Detects[inType] );
	if ( inLogMessages ) m_LogQueue.AddDetectMessage( inType );
}

void
COmniEngineContext::NoteDetect(
	bool						inLogMessages, 
	CEmailStats::EmailStatType	inType,
	const CMsgAddress&			inTarget )
{
	static CEmailStats::EmailStat	Detects[CEmailStats::kStat_TypeCount] = {
		CEmailStats::kStat_SmtpEmailsDetected,
		CEmailStats::kStat_Pop3EmailsDetected
	};

	Increment( Detects[inType] );
	if ( inLogMessages ) m_LogQueue.AddDetectMessage( inType, inTarget );
}


// ----------------------------------------------------------------------------
//		NoteFailure
// ----------------------------------------------------------------------------

void
COmniEngineContext::NoteFailure(
	const CPeekString&	inMessage )
{
	m_LogQueue.AddFailureMessage( inMessage );
}


// ----------------------------------------------------------------------------
//		NoteFileSave
// ----------------------------------------------------------------------------

void
COmniEngineContext::NoteFileSave(
	bool				inLogMessages, 
	const CPeekString&	inTarget )
{
	if ( inLogMessages ) m_LogQueue.AddSaveMessage( inTarget );
}


// ----------------------------------------------------------------------------
//		NoteFileError
// ----------------------------------------------------------------------------

void
COmniEngineContext::NoteFileError(
	const CPeekString&	inFileName )
{
	Increment( CEmailStats::kStat_FileError );
	m_LogQueue.AddFileErrorMessage( inFileName );
}


// ----------------------------------------------------------------------------
//		NoteWriteError
// ----------------------------------------------------------------------------

void
COmniEngineContext::NoteWriteError(
	const CPeekString&	inFileName )
{
	Increment( CEmailStats::kStat_FileError );
	m_LogQueue.AddWriteErrorMessage( inFileName );
}


// ----------------------------------------------------------------------------
//		NoteReset
// ----------------------------------------------------------------------------

void
COmniEngineContext::NoteReset(
	bool						inLogMessages, 
	CEmailStats::EmailStatType	inType )
{
	static CEmailStats::EmailStat	Resets[CEmailStats::kStat_TypeCount] = {
		CEmailStats::kStat_SmtpEmailResets,
		CEmailStats::kStat_Pop3EmailResets
	};

	Increment( Resets[inType] );
	if ( inLogMessages ) m_LogQueue.AddResetMessage( inType );
}

void
COmniEngineContext::NoteReset(
	bool						inLogMessages, 
	CEmailStats::EmailStatType	inType,
	const CMsgAddress&			inTarget )
{
	static CEmailStats::EmailStat	Resets[CEmailStats::kStat_TypeCount] = {
		CEmailStats::kStat_SmtpEmailResets,
		CEmailStats::kStat_Pop3EmailResets
	};

	Increment( Resets[inType] );
	if ( inLogMessages ) m_LogQueue.AddResetMessage( inType, inTarget );
}


// ----------------------------------------------------------------------------
//		NoteMalformed
// ----------------------------------------------------------------------------

void
COmniEngineContext::NoteMalformed(
	CEmailStats::EmailStatType	inType )
{
	static CEmailStats::EmailStat	Aborts[CEmailStats::kStat_TypeCount] = {
		CEmailStats::kStat_SmtpEmailMalformed,
		CEmailStats::kStat_Pop3EmailMalformed
	};

	Increment( Aborts[inType] );
}


// ----------------------------------------------------------------------------
//		NoteTarget
// ----------------------------------------------------------------------------

void
COmniEngineContext::NoteTarget(
	bool				inLogMessages, 
	const CPeekString&	inTarget,
	const CPeekString&	inMessage )
{
	if ( inLogMessages ) {
		CPeekOutString	ssMsg;
		ssMsg << inMessage << inTarget;
		m_LogQueue.AddMessage( ssMsg );
	}
}


// ----------------------------------------------------------------------------
//		NoteTargets
// ----------------------------------------------------------------------------

void
COmniEngineContext::NoteTargets(
	bool				inLogMessages, 
	const CArrayString&	inTargets,
	const CPeekString&	inMessage )
{
	if ( inLogMessages ) {
		for ( size_t i = 0; i < inTargets.GetCount(); i++ ) {
			CPeekOutString	ssMsg;
			ssMsg << inMessage << inTargets.ToString();
			m_LogQueue.AddMessage( ssMsg );
		}
	}
}


// ----------------------------------------------------------------------------
//		NoteTimedOutMessages
// ----------------------------------------------------------------------------

void
COmniEngineContext::NoteTimedOutMessages(
	bool						/*inLogMessages*/, 
	CEmailStats::EmailStatType	inType,
	const CArrayString&			inRemovedList )
{
	static CEmailStats::EmailStat	Resets[CEmailStats::kStat_TypeCount] = {
		CEmailStats::kStat_SmtpSessionsTimeOut,
			CEmailStats::kStat_Pop3SessionsTimeOut
	};

	m_Stats.Increment( Resets[inType], inRemovedList.GetCount() );
	// Per customer request 1/28/2008
	// if ( inDisplayMessage && inOptions->DisplayMessages() ) m_LogQueue.AddTimedOutMessages( inType, inRemovedList );
}


// -----------------------------------------------------------------------------
//		OnCreateContext
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnCreateContext()
{
	int	nResult = CPeekEngineContext::OnCreateContext();

	if ( !IsGlobalContext() ) {
		COmniPlugin*	pPlugin( GetPlugin() );
		_ASSERTE( pPlugin );
		if ( pPlugin ) SetOptions( pPlugin->GetOptions() );
	}

	m_LogQueue.Init( this );

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnDisposeContext
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnDisposeContext()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	nResult = CPeekEngineContext::OnDisposeContext();

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnFilter
//		: Implements IFilterPacket::FilterPacket
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnFilter(
	CPacket&	inPacket,
	UInt16*		outBytesToAccept,
	CGlobalId&	/*outFilterId*/ )
{
	UInt16		nProtoSpec = inPacket.GetProtoSpecId();
	COptionsPtr	spOptions = GetOptions();
	if ( spOptions == NULL ) return PLUGIN_PACKET_REJECT;
	if ( !spOptions->IsProtocolOfInterest( nProtoSpec ) ) return PLUGIN_PACKET_REJECT;

	bool	bAcceptPacket = false;
	if ( nProtoSpec == ProtoSpecDefs::SMTP ) {
		UInt16	nSliceLength = PLUGIN_ACCEPT_WHOLE_PACKET;
		bAcceptPacket = ProcessSMTP( spOptions, inPacket, nSliceLength );
		if ( bAcceptPacket ) {
			*outBytesToAccept = nSliceLength;
		}
	}
	else if ( nProtoSpec == ProtoSpecDefs::POP3 ) {
		ProcessPOP3( spOptions, inPacket );
	}

	m_LogQueue.PostMessages();

	return (bAcceptPacket) ? PLUGIN_PACKET_ACCEPT : PLUGIN_PACKET_REJECT;
}


// -----------------------------------------------------------------------------
//		OnNotify
//		: Implements INotify::Notify
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnNotify(
	 const CGlobalId&	inContextId,
	 const CGlobalId&	inSourceId,
	 UInt64				inTimeStamp,
	 PeekSeverity		inSeverity,
	 const CPeekString& inShortMessage,
	 const CPeekString& inLongMessage )
{
	inContextId;
	inSourceId;
	inTimeStamp;
	inSeverity;
	inShortMessage;
	inLongMessage;

	int	nResult = PEEK_PLUGIN_SUCCESS;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnProcessPacket
//		: Implements IProcessPacket::ProcessPacket
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnProcessPacket(
	CPacket&	inPacket )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	inPacket;

	return nResult;
}


#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		OnProcessPluginMessage
//		: Implements IProcessPluginMessage::ProcessPluginMessage
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnProcessPluginMessage(
	CPeekStream& inMessage,
	CPeekStream& outResponse )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	inMessage;
	outResponse;

	// Create and Load the appropriate Message object.
	CPeekMessagePtr	spMessage = MessageFactory( inMessage );
	if ( spMessage.get() != NULL ) {
		switch ( spMessage->GetType() ) {
			case CMessageGetOptions::s_nMessageType:
				nResult = ProcessGetOptions( spMessage.get() );
				break;

		default:
			nResult = PEEK_PLUGIN_FAILURE;
		}
	}

	if ( nResult == PEEK_PLUGIN_SUCCESS ) {
		spMessage->StoreResponse( outResponse );
	}

	return nResult;
}
#endif // IMPLEMENT_PLUGINMESSAGE


// -----------------------------------------------------------------------------
//		OnProcessTime
//		: Implements IProcessTime::ProcessTime
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnProcessTime(
	UInt64	/*inCurrentTime*/ )
{
	{
		COptionsPtr	pOptions( GetOptions() );
		if ( pOptions == NULL ) return PEEK_PLUGIN_SUCCESS;

		CPeekDuration	pdTimeOut( kMinutes, 5 );
		CArrayString	SmtpRemovedList;
		CArrayString	Pop3RemovedList;

		try {
			CSmtpMsgStreamListPtr	pMsgStreamList( m_SafeSmtpMsgStreamList );
			if ( pMsgStreamList != NULL ) {
				pMsgStreamList->Monitor( pdTimeOut, SmtpRemovedList );	// 5 minutes
				NoteTimedOutMessages( true, CEmailStats::kStat_TypeSmtp, SmtpRemovedList );
			}
		}
		catch ( ... ) {
			m_nFailure = 'MTR1';
		}

		try {
			CPop3MsgStreamListPtr	pMsgStreamList( m_SafePop3MsgStreamList );
			if ( pMsgStreamList != NULL ) {
				pMsgStreamList->Monitor( pdTimeOut, Pop3RemovedList );
				NoteTimedOutMessages( true, CEmailStats::kStat_TypePop3, Pop3RemovedList );
			}
		}
		catch ( ... ) {
			m_nFailure = 'MTR2';
		}
	}

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnReset
//		: Implements IResetProcessing::ResetProcessing
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnReset()
{
	m_Stats.Reset();

	try {
		CSmtpMsgStreamListPtr	pMsgStreamList( m_SafeSmtpMsgStreamList );
		if ( pMsgStreamList != NULL ) {
			pMsgStreamList->Reset();
		}
	}
	catch ( ... ) {
		m_nFailure = 'RST1';
	}

	try {
		CPop3MsgStreamListPtr	pMsgStreamList( m_SafePop3MsgStreamList );
		if ( pMsgStreamList != NULL ) {
			pMsgStreamList->Reset();
		}
	}
	catch ( ... ) {
		m_nFailure = 'RST2';
	}

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnResetCapture
//		: Implements IPacketProcessorEvents::OnResetPacketProcessing
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnResetCapture()
{
	m_Stats.Reset();

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnStartCapture
//		: Implements IPacketProcessorEvents::OnStartPacketProcessing
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnStartCapture(
	UInt64	/*inStartTime*/ )
{
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnStopCapture
//		: Implements IPacketProcessorEvents::OnStopPacketProcessing
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnStopCapture(
	UInt64	/*inStopTime*/ )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	try {
		CSmtpMsgStreamListPtr	pMsgStreamList( m_SafeSmtpMsgStreamList );
		if ( pMsgStreamList != NULL ) {
			pMsgStreamList->Reset();
		}
	}
	catch ( ... ) {
		m_nFailure = 'STP1';
	}

	try {
		CPop3MsgStreamListPtr	pMsgStreamList( m_SafePop3MsgStreamList );
		if ( pMsgStreamList != NULL ) {
			pMsgStreamList->Reset();
		}
	}
	catch ( ... ) {
		m_nFailure = 'STP2';
	}

	return nResult;
}


#ifdef IMPLEMENT_ALL_UPDATESUMMARY
// -----------------------------------------------------------------------------
//		OnSummary
//		: Implements IUpdateSummaryStats::UpdateSummaryStats
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnSummary(
#ifdef IMPLEMENT_UPDATESUMMARYSTATS
	CSnapshot&	/*inSnapshot*/ )
#else
	)
#endif
{
	{
		// Minimize the scope of pOptions.
		COptionsPtr	pOptions( GetOptions() );
		if ( pOptions == NULL ) return PEEK_PLUGIN_FAILURE;
		if ( !pOptions->IsSummaryStats() ) return PEEK_PLUGIN_SUCCESS;
	}

	const CPeekString&	strPluginName( COmniPlugin::GetName() );

	for ( CEmailStats::EmailStat s = m_Stats.FirstIndex();
			s < CEmailStats::kStat_StatCount;
			s = m_Stats.NextIndex( s ) ) {
		size_t nValue( m_Stats.Get( s ) );
		DoSummaryModifyEntry(
			m_Stats.Label( s ),
			strPluginName,
			(kSummaryType_OtherCount | kSummarySize_UInt32),
			static_cast<void *>( &nValue ) );
	}

	CSmtpMsgStreamListPtr	pSmtpMsgStreamList( m_SafeSmtpMsgStreamList );
	if ( pSmtpMsgStreamList != NULL ) {
		size_t	nSmtpCount( pSmtpMsgStreamList->GetCount() );
		DoSummaryModifyEntry(
			s_strLabels[kLabel_SmtpOpen],
			strPluginName,
			(kSummaryType_OtherCount | kSummarySize_UInt32),
			static_cast<void *>( &nSmtpCount ) );
	}

	CPop3MsgStreamListPtr	pPop3MsgStreamList( m_SafePop3MsgStreamList );
	if ( pPop3MsgStreamList != NULL ) {
		size_t	nPop3Count( pPop3MsgStreamList->GetCount() );
		DoSummaryModifyEntry(
			s_strLabels[kLabel_Pop3Open],
			strPluginName,
			(kSummaryType_OtherCount | kSummarySize_UInt32),
			static_cast<void *>( &nPop3Count ) );
	}

	return PEEK_PLUGIN_SUCCESS;
}
#endif // IMPLEMENT_ALL_UPDATESUMMARY


// -----------------------------------------------------------------------------
//		OnContextPrefs
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnContextPrefs(
	CPeekDataModeler& ioPrefs )
{
	CPeekContext::OnContextPrefs( ioPrefs );

	if ( ioPrefs.IsLoading() ) {
		if ( m_bDefaultOptions ) {
			m_pPlugin->SetOptions( ioPrefs );

		}
		SetOptions( ioPrefs );
	}
	else {
		COptionsPtr	pOptions = GetOptions();
		pOptions->Model( ioPrefs );
	}

	return PEEK_PLUGIN_SUCCESS;
}


#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		ProcessGetOptions
// -----------------------------------------------------------------------------

int
COmniEngineContext::ProcessGetOptions(
	CPeekMessage* ioMessage )
{
	CMessageGetOptions*	pMessage = dynamic_cast<CMessageGetOptions*>( ioMessage );
	_ASSERTE( pMessage != NULL );
	if ( pMessage == NULL ) return PEEK_PLUGIN_FAILURE;

	// Create the modeler.
	if ( !pMessage->StartResponse() ) {
		return PEEK_PLUGIN_FAILURE;
	}

	// Set the response (output).
	{
		COptionsPtr	pOptions = GetOptions();
		pMessage->SetOptions( pOptions );
	}

	return PEEK_PLUGIN_SUCCESS;
}
#endif // IMPLEMENT_PLUGINMESSAGE


// ----------------------------------------------------------------------------
//		ProcessPOP3
// ----------------------------------------------------------------------------

void
COmniEngineContext::ProcessPOP3(
	COptions*	inOptions,
	CPacket&	inPacket )
{
	// CEmailStats::EmailStatType	nStatType( CEmailStats::kStat_TypePop3 );
	CMsgPacketPtr				ptrMsgPacket;
	CPop3MsgStreamListPtr		ptrMsgStreamList;
	CPop3MsgStream*				pMsgStream( NULL );

	try {
		ASSERT( inOptions != NULL );
		if ( inOptions == NULL ) return;

		ASSERT( inPacket.IsValid() );
		if ( (inPacket.IsNotValid()) || (inPacket.GetPacketDataLength() == 0) ) return;

		// Create a new Msg Packet, creates a string from the payload.
		// Delete if not Added to a Msg Stream.
		CMsgPacket*	pMsgPacket( NULL );
		try {
			pMsgPacket = new CMsgPacket( inPacket );
			if ( pMsgPacket == NULL ) return;
			ptrMsgPacket.Attach( pMsgPacket );
		}
		catch ( ... ) {
			m_nFailure = 'POPA';
			return;
		}

		bool	bResult = false;
		try {
			CPeekString	strMessage;
			bResult = ptrMsgPacket->SetInfo( m_nFailure, strMessage );
			if ( !bResult ) {
				if ( m_nFailure != 0 ) {
					if ( !strMessage.IsEmpty() ) {
						m_LogQueue.AddMessage( strMessage );
					}
				}
				return ;
			}
		}
		catch ( ... ) {
			m_nFailure = 'POPB';
			return;
		}

		try {
			ptrMsgStreamList = m_SafePop3MsgStreamList;
			if ( ptrMsgStreamList == NULL ) return;
		}
		catch ( ... ) {
			m_nFailure = 'POPC';
			return;
		}
	}
	catch ( ... ) {
		m_nFailure = 'POP1';
		return;
	}

	// Find the Msg Stream
	try {
		CPop3MsgStream*	pMsgStream( NULL );
		try {
			pMsgStream = ptrMsgStreamList->Find( ptrMsgPacket->GetIpPortPair() );
		}
		catch ( ... ) {
			m_nFailure = 'POP5';
			pMsgStream = NULL;
		}
		if ( pMsgStream != NULL ) {
			if ( ptrMsgPacket->GetDirection() == kDirection_FromServer ) {
				// Also need to check for RST/FIN
				if ( ptrMsgPacket->IsError() ) {
					Increment( CEmailStats::kStat_Pop3EmailResets );
					ptrMsgStreamList->RemoveAndDelete( pMsgStream );
					pMsgStream = NULL;
				}
				else {
					pMsgStream->TestAndAddMsg( ptrMsgPacket );
					if ( pMsgStream->IsEOMReceived() || !pMsgStream->IsOpen() ) {
						ptrMsgStreamList->Remove( pMsgStream );
						AnalyzeAndDeleteMsgStream( inOptions, pMsgStream );
						pMsgStream = NULL;
					}
				}
				return;
			}

			// Direction is kDirection_ToServer
			// Add the ACKs to the Msg Stream.
			if ( ptrMsgPacket->IsAck() ) {
				pMsgStream->TestAndAddMsg( ptrMsgPacket );
				if ( !pMsgStream->IsOpen() ) {
					ptrMsgStreamList->Remove( pMsgStream );
					AnalyzeAndDeleteMsgStream( inOptions, pMsgStream );
					pMsgStream = NULL;
				}
				return;
			}

			// Client has issued a command, so its done receiving the email:
			//  Analyze the email and then delete it.
			// Continue to evaluate the command.
			ptrMsgStreamList->Remove( pMsgStream );
			AnalyzeAndDeleteMsgStream( inOptions, pMsgStream );
			pMsgStream = NULL;

			// Msg Item has a new command, continue and check for Retrieve.
		}
	}
	catch ( ... ) {
		m_nFailure = 'POP2';
		return;
	}

	ASSERT( pMsgStream == NULL );

	// Is this a request to Retrieve an email message?
	try {
		if ( ptrMsgPacket->IsRetrieve() ) {
			// Do not add message to the log. Too many that are not of interest.
			// NoteDetect( inOptions->IsLogMessages(), nStatType );
			Increment( CEmailStats::kStat_Pop3EmailsDetected );

			// Create a new Msg Stream in m_MsgStreams.
			pMsgStream = new CPop3MsgStream( ptrMsgPacket->GetIpPortPair(), *this, 0 );
			if ( (pMsgStream != NULL) && (ptrMsgStreamList->Add( pMsgStream )) ) {
				// Add the Msg Item to the Msg Stream.
				pMsgStream->TestAndAddMsg( ptrMsgPacket );
			}
			else {
				; // log the error.
			}
		}
	}
	catch ( ... ) {
		m_nFailure = 'POP3';
		return;
	}
}


// ----------------------------------------------------------------------------
//		ProcessSMTP
// ----------------------------------------------------------------------------

bool
COmniEngineContext::ProcessSMTP(
	COptions*	inOptions,
	CPacket&	inPacket,
	UInt16&		outSliceLength )
{
	UInt16						nSliceLength( PLUGIN_ACCEPT_WHOLE_PACKET );
	CEmailStats::EmailStatType	nStatType( CEmailStats::kStat_TypeSmtp );
	CMsgPacketPtr				ptrMsgPacket;
	CSmtpMsgStreamListPtr		pMsgStreamList;
	CSmtpMsgStream*				pMsgStream( NULL );

	try {
		ASSERT( inOptions != NULL );
		if ( inOptions == NULL ) return false;

		ASSERT( inPacket.IsValid() );
		if ( (inPacket.IsNotValid()) || (inPacket.GetPacketDataLength() == 0) ) return false;

		// Create a new Msg Packet, creates a string from the payload.
		// Delete if not Added to a Msg Stream.
		CMsgPacket*	pMsgPacket( NULL );
		try {
			pMsgPacket = new CMsgPacket( inPacket );
			if ( pMsgPacket == NULL ) return false;
			ptrMsgPacket.Attach( pMsgPacket );
		}
		catch ( ... ) {
			m_nFailure = 'SMPA';
			return false;
		}

		bool	bResult = false;
		try {
			CPeekString	strMessage;
			bResult = ptrMsgPacket->SetInfo( m_nFailure, strMessage );
			if ( !bResult ) {
				if ( m_nFailure != 0 ) {
					if ( !strMessage.IsEmpty() ) {
						m_LogQueue.AddMessage( strMessage );
					}
				}
				return false;
			}

			nSliceLength = ptrMsgPacket->GetSliceLength();
		}
		catch ( ... ) {
			m_nFailure = 'SMPB';
			return false;
		}

		pMsgStreamList = m_SafeSmtpMsgStreamList;
		if ( pMsgStreamList == NULL ) return false;

		// Find the Msg Stream
		try {
			pMsgStream = pMsgStreamList->Find( ptrMsgPacket->GetIpPortPair() );
		}
		catch( ... ) {
			m_nFailure = 'SMPC';
			return false;
		}
	}
	catch ( ... ) {
		m_nFailure = 'SMP1';
		return false;
	}

	try {
		if ( pMsgStream != NULL ) {
			CMsgPacket*		pItem( NULL );
			MsgDirection	nDirection = ptrMsgPacket->GetDirection();
			pItem = pMsgStream->TestAndAddMsg( ptrMsgPacket );
			if ( pItem == NULL ) return false;

			if ( nDirection == kDirection_ToServer ) {
				if ( pItem->IsReset() || pItem->IsQuit() || !pMsgStream->IsOpen() ) {
					// 1) Client requested to RSET (reset) the connection or
					// 2) Client requested to QUIT (abort) the connect
					// Abort the tracking of this data stream
					NoteReset( inOptions->IsLogMessages(), nStatType, pMsgStream->GetIdA() );
					pMsgStreamList->RemoveAndDelete( pMsgStream );
					pMsgStream = NULL;
				}
			}
			else {
				if ( !pMsgStream->IsOpen() ) {
					NoteReset( inOptions->IsLogMessages(), nStatType, pMsgStream->GetIdA() );
#ifdef _DEBUG
					//CByteStream	theStream;
					//if ( pMsgStream->GetByteStream( theStream ) ) {
					//	// Write the stream to disk
					//	CPeekString	strOutputDirectory = pMsgStream->GetOutputDirectory( inOptions );
					//	CPeekString	strFileName = pMsgStream->ErrorFileName(
					//		CPeekString( L"SMTPRESETQUIT.txt", strOutputDirectory );
					//	try {
					//		if ( !theStream.Write( strFileName ) ) {
					//			NoteFileError( strFileName );
					//		}
					//	}
					//	catch ( ... ) {
					//		NoteFileError( strFileName );
					//	}
					//}
					//else {
					//	NoteFailure( L"Failed to get ByteStream" );
					//}
#endif

					pMsgStreamList->RemoveAndDelete( pMsgStream );
					pMsgStream = NULL;
				}
			}

			if ( (pMsgStream != NULL) && pMsgStream->IsEOMReceived() && (pItem->GetLength() > 0) ) {
				pMsgStreamList->Remove( pMsgStream );
				AnalyzeAndDeleteMsgStream( inOptions, pMsgStream );
				pMsgStream = NULL;
			}

			if ( inOptions->IsAcceptPackets() ) {
				if ( inOptions->IsSlicePackets() ) {
					outSliceLength = nSliceLength;
				}
				return true;
			}
			return false;
		}
	}
	catch ( ... ) {
		m_nFailure = 'SMP2';
		return false;
	}

	try {
		if ( ptrMsgPacket == NULL ) return false;

		// Is this Msg Item of interest?
		CMsgAddress	MsgAddress;
		size_t		nTargetIndex( kIndex_Invalid );
		bool		bDetect( CSmtpMsgStream::Detect(
			inOptions->GetCases().GetList(), ptrMsgPacket, MsgAddress, this, nTargetIndex ) );
		if ( bDetect ) {
			NoteDetect( inOptions->IsLogMessages(), nStatType, MsgAddress );

			// Create a new Msg Stream in m_MsgStreams.
			pMsgStream = new CSmtpMsgStream( ptrMsgPacket->GetIpPortPair(), *this, nTargetIndex, MsgAddress );
			if ( pMsgStreamList->Add( pMsgStream ) ) {
				// Add the Msg Item to the Msg Stream.
				pMsgStream->TestAndAddMsg( ptrMsgPacket );
			}
			else {
				; // log the error
			}
			if ( inOptions->IsAcceptPackets() ) {
				if ( inOptions->IsSlicePackets() ) {
					outSliceLength = nSliceLength;
				}
				return true;
			}
			return false;
		}
	}
	catch ( ... ) {
		m_nFailure = 'SMP3';
		return false;
	}

	return false;
}


// -----------------------------------------------------------------------------
//		SetOptions
// -----------------------------------------------------------------------------

void
COmniEngineContext::SetOptions(
	CPeekDataModeler&	inPrefs )
{
	COptions	optNew;
	optNew.Model( inPrefs );

	COptionsPtr			pOptions( GetOptions() );
	CPeekStringA		strName( GetCaptureName() );
	const CPeekString&	strLogFileName( GetLogFileName() );
	if ( IsDefault() ) {
		pOptions->Copy( optNew );
		pOptions->Log( strName, strLogFileName );
		m_bDefaultOptions = false;
	}
	else {
		if ( !pOptions->LogUpdate( strName, optNew, strLogFileName ) ) {
			m_nFailure = 'SeOp';
		}
	}
	pOptions->Copy( optNew );
}

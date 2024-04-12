// =============================================================================
//	OmniEngineContext.cpp
// =============================================================================
//	Copyright (c) 2011-2015 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "OmniEngineContext.h"
#include "RemotePlugin.h"
#include "MemUtil.h"
#include "Packet.h"
#include "PacketArray.h"
#include "PluginMessages.h"
#include "PeekSummaryStats.h"

#include "PeekMessage.h"
#include "PeekFile.h"
#include "PeekPacket.h"
#include "PeekTime.h"
//#include "TimeConv.h"
#include "FileEx.h"
#include "Version.h"
#include <memory>


template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}


class CRemotePlugin;

////////////////////////////////////////////////////////////////////////
// Code Examples
// Caution: defining either of the following symbols in the .h file
// will cause writes to disk on the OmniEngine computer:
//   EXAMPLE_DECODE
//   EXAMPLE_FILECAPTUREBUFFER
//
////////////////////////////////////////////////////////////////////////


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
	CGlobalId&		inId,
	CRemotePlugin*	inRemotePlugin )
	:	CPeekEngineContext( inId, inRemotePlugin )
	,	m_bDefaultOptions( true )
	,	m_nFailure( 0 )
{
	PeekLock lock( m_Mutex );
}

COmniEngineContext::~COmniEngineContext()
{
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
		for ( size_t i = 0; i < inTargets.size(); i++ ) {
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

	m_Stats.Increment( Resets[inType], inRemovedList.size() );
	// Per customer request 1/28/2008
	// if ( inDisplayMessage && inOptions->DisplayMessages() ) m_LogQueue.AddTimedOutMessages( inType, inRemovedList );
}


// -----------------------------------------------------------------------------
//		OnCreateContext
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnCreateContext()
{
	PeekLock lock( m_Mutex );

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
//		OnCaptureStarting											  [Omni 7.0]
//
//  The capture context will begin setup for capture.
//	The engine has entered its StartCapture() fuction.
//	Initialize plugin specific resources, but Capture Context specific objects
//	have not been created yet.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnCaptureStarting()
{
	PeekLock lock( m_Mutex );

	int	nResult = PEEK_PLUGIN_SUCCESS;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnCaptureStarted											  [Omni 7.0]
//
//  The capture context is capturing.
//	The engine is about to exit its StartCapture() fuction.
//	The Capture Context is now capturing, all Capture Context specific objects
//	have been created and initialized.
//	The plugin may now Insert Packets into the Capture Context.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnCaptureStarted()
{
	PeekLock lock( m_Mutex );

	int	nResult = PEEK_PLUGIN_SUCCESS;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnCaptureStopping											  [Omni 7.0]
//
//	The capture context will start to shutdown capturing.	
//	The engine has entered its StopCapture() fuction.
//	All Capture Context specific objects are still initialized and functional.
//	The plugin may still Insert Packets into the Capture Context.
//	The StopCapture() function will not continue until all plugins have
//	returned from this function.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnCaptureStopping()
{
	PeekLock lock( m_Mutex );

	int	nResult = PEEK_PLUGIN_SUCCESS;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnCaptureStopped											  [Omni 7.0]
//
//	The capture context has torn down the capture.
//	The engine is about to exit its StopCapture() fuction.
//	All Capture Context specific objects have been destroyed.
//	The plugin may not Insert Packets into the Capture Context.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnCaptureStopped()
{
	PeekLock lock( m_Mutex );

	int	nResult = PEEK_PLUGIN_SUCCESS;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnDisposeContext
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnDisposeContext()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	PeekLock lock( m_Mutex );

	nResult = CPeekEngineContext::OnDisposeContext();

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnFilter
//		: Implements IFilterPacket::FilterPacket
//
//	This method is called when an Advanced Filter contains an Analysis Module
//	node that set to 'this' type of plugin.
//
//	To accept the packet into the Capture Buffer return PLUGIN_PACKET_ACCEPT.
//	To reject the packet return PEEK_PLUGIN_REJECT (or any non-zero value).
//	To Slice the packet set OutBytesToAccept to the number of bytes
//	the plugin will accept of the packet.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnFilter(
	CPacket&	inPacket,
	UInt16*		outBytesToAccept,
	CGlobalId&	/*outFilterId*/ )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	PeekLock lock( m_Mutex );

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

	return nResult;
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
	int	nResult = PEEK_PLUGIN_SUCCESS;

	(void)inContextId;
	(void)inSourceId;
	(void)inTimeStamp;
	(void)inSeverity;
	(void)inShortMessage;
	(void)inLongMessage;

	PeekLock lock( m_Mutex );

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnProcessPacket
//		: Implements IProcessPacket::ProcessPacket
//
//	The Packet has been inserted into the Capture Buffer.
//	Perform any analysis of the packet at this time.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnProcessPacket(
	CPacket&	inPacket )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	(void)inPacket;

	PeekLock lock( m_Mutex );

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnProcessPluginMessage
//		: Implements IProcessPluginMessage::ProcessPluginMessage
//
//	An instance of 'this' plugin in OmniPeek has sent this instance of the
//	plugin on OmniEngine a message. The contents and responses are private to
//	this plugin.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnProcessPluginMessage(
	CPeekStream& inMessage,
	CPeekStream& outResponse )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	PeekLock lock( m_Mutex );

	// Create and Load the appropriate Message object.
	std::unique_ptr<CPeekMessage>	spMessage = MessageFactory( inMessage );
	if ( spMessage.get() != nullptr ) {
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


// -----------------------------------------------------------------------------
//		OnProcessTime
//		: Implements IProcessTime::ProcessTime
//
//	While the Capture Context is capturing, this method is called once a
//	second.
//	When a capture file is being processed, this method is called at one second
//	intervals based on the timestamps of the packets.
//	If the duration between timestamps is excessive, then this method may be
//	called multiple times between packets. And the difference between time
//	stamps bay be longer than one second.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnProcessTime(
	UInt64	/*inCurrentTime*/ )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	PeekLock lock( m_Mutex );

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

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnReset
//		: Implements IResetProcessing::ResetProcessing
//
//	The Capture Context has been reset. The Capture Context is reset before
//	capturing begins.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnReset()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	PeekLock lock( m_Mutex );

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

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnResetCapture
//		: Implements IPacketProcessorEvents::OnResetPacketProcessing
//
//	The Capture Context has processed 2^64 packets and the  packet index is
//	about to rollover.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnResetCapture()
{
	PeekLock lock( m_Mutex );

	m_Stats.Reset();

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnStartCapture
//		: Implements IPacketProcessorEvents::OnStartPacketProcessing
//
//	The Capture Context has been setup to capture.
//	The plugin may Insert Packets.
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
//
//	The Capture Context has stopped capturing.
//	Most of the Capture Context specific objects have been reset or destroyed.
//	The plugin may not Insert Packets into the Capture Context.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnStopCapture(
	UInt64	/*inStopTime*/ )
{
	PeekLock lock( m_Mutex );

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

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnSummary
//		: Implements IUpdateSummaryStats::UpdateSummaryStats
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnSummary(
	CSnapshot&	/*inSnapshot*/ )
{
	PeekLock lock( m_Mutex );

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


// -----------------------------------------------------------------------------
//		OnContextPrefs
//
//	Preferences (settings) specific to this Capture Context are being loaded or
//	stored into this instance of this plugin.
//	The Peek Data Modeler handles both loading and storing via the Model method.
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnContextPrefs(
	CPeekDataModeler& ioPrefs )
{
	PeekLock lock( m_Mutex );

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


// -----------------------------------------------------------------------------
//		ProcessGetOptions
//
//	Process the Peek Plugin Message: Get Options.
//	This protocol is private to this plugin.
// -----------------------------------------------------------------------------

int
COmniEngineContext::ProcessGetOptions(
	CPeekMessage* ioMessage )
{
	PeekLock lock( m_Mutex );

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
			ptrMsgPacket = make_unique<CMsgPacket>( pMsgPacket );
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
		pMsgStream = NULL;
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
			ptrMsgPacket = make_unique<CMsgPacket>( pMsgPacket );
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
			inOptions->GetCases().GetList(), ptrMsgPacket.get(), MsgAddress, this, nTargetIndex ) );
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

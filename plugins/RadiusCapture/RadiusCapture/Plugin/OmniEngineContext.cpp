// =============================================================================
//	OmniEngineContext.cpp
// =============================================================================
//	Copyright (c) 2010-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "OmniEngineContext.h"
#include "RemotePlugin.h"
#include "MemUtil.h"
#include "Packet.h"
#include "PacketArray.h"
#include "PluginMessages.h"

#ifdef IMPLEMENT_SUMMARYMODIFYENTRY
static CPeekString	s_strPacketsProcessed( L"Packets Processed" );
#endif // IMPLEMENT_SUMMARYMODIFYENTRY

const UInt64	kResolutionPerSecond = 1000000000ull;
const UInt64	kScreenLogCycleTime = SCREEN_LOG_CYCLE_TIME * kResolutionPerSecond;
#ifndef TEST_NO_MONITORING
const UInt64	kMonitorCycleTime = MONINTOR_CYCLE_TIME * kResolutionPerSecond;
#endif


// =============================================================================
//		COmniEngineContext
// =============================================================================

COmniEngineContext::COmniEngineContext(
	CGlobalId&		inId,
	CRemotePlugin*	inRemotePlugin )
	:	CPeekEngineContext( inId, inRemotePlugin )
	,	m_bCapturing( false )
#ifndef TEST_NO_MONITORING
	,	m_nMonitorTrigger( 0 )
#endif // TEST_NO_MONITORING
	,	m_nScreenLogTrigger( 0 )
{
	inRemotePlugin;
}

COmniEngineContext::~COmniEngineContext()
{
}


// -----------------------------------------------------------------------------
//		OnCreateContext
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnCreateContext()
{
	int	nResult = CPeekEngineContext::OnCreateContext();

#ifdef IMPLEMENT_UPDATESUMMARYSTATS
	ClearSummaryStats();
#endif

	m_LogQueue.Init( reinterpret_cast<IRadiusContext*>( this ) );

	m_RadiusProcess.Init( reinterpret_cast<IRadiusContext*>( this ) );

	SetCapturing( false );

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
	CGlobalId&	outFilterId )
{
	int	nResult = PEEK_PLUGIN_FAILURE;

	bool bPacketToScreen = false;

	outBytesToAccept;
	outFilterId;

	if ( IsCapturing() ) {
		bPacketToScreen = m_RadiusProcess.ProcessPacket( inPacket, GetOptions() );
	}
	else {
		COptions& thePluginOptions( GetOptions() );
		COptions& theContextOptions( GetOptions() );

		CreateUniqueCaseCaptureIds( thePluginOptions, theContextOptions, true );

		StartCaptureForReal( theContextOptions );

		bPacketToScreen = m_RadiusProcess.ProcessPacket( inPacket, GetOptions() );
	}

	nResult = bPacketToScreen ? PEEK_PLUGIN_SUCCESS : PEEK_PLUGIN_FAILURE;

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
	UInt64	inCurrentTime )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	PeriodicProcessing( inCurrentTime );

#ifdef IMPLEMENT_UPDATESUMMARYSTATS
	ClearSummaryStats();
#endif

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnReset
//		: Implements IResetProcessing::ResetProcessing
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnReset()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

#ifdef IMPLEMENT_UPDATESUMMARYSTATS
	ClearSummaryStats();
#endif

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnResetCapture
//		: Implements IPacketProcessorEvents::OnResetPacketProcessing
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnResetCapture()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnStartCapture
//		: Implements IPacketProcessorEvents::OnStartPacketProcessing
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnStartCapture(
	UInt64	inStartTime )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	inStartTime;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnStopCapture
//		: Implements IPacketProcessorEvents::OnStopPacketProcessing
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnStopCapture(
	UInt64	inStopTime )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	inStopTime;

	SetCapturing( false );

	m_RadiusProcess.StopCapture();

	PeriodicProcessing( inStopTime, true );

	m_LogQueue.RemoveAll();


	return nResult;
}


// -----------------------------------------------------------------------------
//		PeriodicProcessing
//		: Implements IPacketProcessorEvents::OnStopPacketProcessing
// -----------------------------------------------------------------------------

void
COmniEngineContext::PeriodicProcessing(
	UInt64	inCurrentTime,
	bool	bOverride ) 
{
	if ( !IsCapturing() && !bOverride ) return;

  #ifndef TEST_NO_MONITORING
	if ( (inCurrentTime > m_nMonitorTrigger) || bOverride ) {
		m_RadiusProcess.Monitor( GetOptions() );
		m_nMonitorTrigger = kMonitorCycleTime + inCurrentTime;
	}
  #endif // TEST_NO_MONITORING
	
	if ( (inCurrentTime > m_nScreenLogTrigger ) || bOverride ) {
		m_LogQueue.PostMessages();
 		m_nScreenLogTrigger = kScreenLogCycleTime + inCurrentTime;
	}
}


#ifdef IMPLEMENT_UPDATESUMMARYSTATS
// -----------------------------------------------------------------------------
//		ClearSummaryStats
// -----------------------------------------------------------------------------

void
COmniEngineContext::ClearSummaryStats()
{
	memset( &m_Stats, 0, sizeof(m_Stats) );
}
#endif // IMPLEMENT_UPDATESUMMARYSTATS


// -----------------------------------------------------------------------------
//		OnContextPrefs
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnContextPrefs(
	CPeekDataModeler& ioPrefs )
{
	CPeekContext::OnContextPrefs( ioPrefs );

	UpdateOptions( ioPrefs );

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		UpdateOptions
// -----------------------------------------------------------------------------

void
COmniEngineContext::UpdateOptions(
	CPeekDataModeler& ioPrefs )
{
	ASSERT( ioPrefs.IsLoading() );
	if ( !ioPrefs.IsLoading() ) return;

	COptions& theCurrentOptions( m_Options );
	COptions theNewOptions;

	theNewOptions.Model( ioPrefs );

	m_pPlugin->SetOptions( ioPrefs );

	m_RadiusProcess.UpdateOptions( &theCurrentOptions, &theNewOptions );
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
	ASSERT( pMessage != NULL );
	if ( pMessage == NULL ) return PEEK_PLUGIN_FAILURE;

	// Create the modeler.
	if ( !pMessage->StartResponse() ) {
		return PEEK_PLUGIN_FAILURE;
	}

	// Set the response (output).
	pMessage->SetOptions( GetOptions() );

	return PEEK_PLUGIN_SUCCESS;
}
#endif // IMPLEMENT_PLUGINMESSAGE


// -----------------------------------------------------------------------------
//		StartCaptureForReal
// -----------------------------------------------------------------------------

int
COmniEngineContext::StartCaptureForReal(
	COptions&	inContextOptions )
{

#ifdef DISPLAY_PACKET_NUMBERS
	m_nNumScreenPackets = 0;
#endif
	m_RadiusProcess.Initialize( inContextOptions );

	ValidateOptions(); // pContextOptions );

	SetCapturing( true ); //, true );

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		ValidateOptions
// -----------------------------------------------------------------------------

void
COmniEngineContext::ValidateOptions()
{
	if ( m_Options.GetOutputRootDirectory() == L"" ) {
		CPeekString theOutputDir = kDefaultOutputDir;
		if ( theOutputDir.Right(1) == L"\\" ) {
			theOutputDir = theOutputDir.Left(theOutputDir.GetLength() - 1 );
		}

		m_Options.SetOutputRootDirectory( theOutputDir );
	}

	m_Options.DisableInvalidCases();

	bool bIsTheTimeActive = m_Options.IsTimeActive();
	bool bThereIsAnActivePort = m_Options.IsThereAnActivePort();
	bool bThereIsAnActiveCase = m_RadiusProcess.IsThereAnActiveCase();

	if ( !bIsTheTimeActive || !bThereIsAnActivePort || !bThereIsAnActiveCase ) {
		CPeekString csMessage = L"Radius - due to the current options settings no data will be captured.";
		if ( !bThereIsAnActiveCase ) csMessage += L"\r\n   There is no active case.";

		if ( m_Options.IsLogMsgsToScreen() ) {
			AddLogToScreenMessage( csMessage );
		}
	}
}


// ----------------------------------------------------------------------------
//      CreateUniqueCaseCaptureIds
// ----------------------------------------------------------------------------

void
COmniEngineContext::CreateUniqueCaseCaptureIds( 
	COptions&	inPluginOptions, 
	COptions&	inContextOptions, 
	bool bWritePrefs )
{
	bWritePrefs;

	CCaseOptionsList& thePluginCaseOptionsList = inPluginOptions.GetCaseOptionsList();
	CCaseOptionsList& theContextCaseOptionsList = inContextOptions.GetCaseOptionsList();
	CPeekString strCaseName;
	bool bOptionsChanged = false;
	UInt32 nContextCaseCaptureId = 0;
	UInt32 nPluginCaseCaptureId = 0;

	const size_t nContextCaseCount = theContextCaseOptionsList.GetCount();
	for ( size_t i = 0; i < nContextCaseCount; i++ ) {
		CCaseOptions* pContextCaseOptions = theContextCaseOptionsList.GetAt( i );
		if ( pContextCaseOptions->IsActive() ) {
			strCaseName =  pContextCaseOptions->GetName();
			ASSERT( strCaseName.GetLength() > 0 );
			CCaseOptions* pPluginCaseOptions = thePluginCaseOptionsList.Lookup( strCaseName );
			if ( pPluginCaseOptions ) {
				nContextCaseCaptureId = pContextCaseOptions->GetCaseCaptureId();
				nPluginCaseCaptureId = pPluginCaseOptions->GetCaseCaptureId();

				if ( pContextCaseOptions->GetCaseCaptureId() > pPluginCaseOptions->GetCaseCaptureId() ) {
					pContextCaseOptions->IncrementCaseCaptureId();
					pPluginCaseOptions->SetCaseCaptureId( pContextCaseOptions->GetCaseCaptureId() );
				}
				else {
					pPluginCaseOptions->IncrementCaseCaptureId();
					pContextCaseOptions->SetCaseCaptureId( pPluginCaseOptions->GetCaseCaptureId() );
				}
			}
			else {
				pContextCaseOptions->IncrementCaseCaptureId();
			}
			bOptionsChanged = true;
		}
	}
}

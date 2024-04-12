// =============================================================================
//	OmniEngineContext.h
// =============================================================================
//	Copyright (c) 2010-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekPlugin.h"
#include "PeekEngineContext.h"
#include "PluginMessages.h"

#include "RadiusProcess.h"
#include "LogQueue.h"

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Omni Engine Context
//
//	An Omni Engine Context is the plugin specific that is created by the
//	OmniEngine (the engine).
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		COmniEngineContext
// =============================================================================

class COmniEngineContext
	:	public CPeekEngineContext
{
protected:
	static COptions			s_DefaultOptions;

protected:
	bool					m_bCapturing;
	CLogQueue				m_LogQueue;
	COptions				m_Options;
	CRadiusProcess			m_RadiusProcess;

//	UInt64 m_MonitorIntervalElapsed;
#ifndef TEST_NO_MONITORING
	UInt64					m_nMonitorTrigger;
#endif // TEST_NO_MONITORING
	UInt64					m_nScreenLogTrigger;

#ifdef DISPLAY_PACKET_NUMBERS
	UInt32					m_nNumScreenPackets;
#endif

#ifdef IMPLEMENT_UPDATESUMMARYSTATS
	struct SummaryStats {
		Peek::SummaryStatEntry	ExampleStat;
	};
	SummaryStats			m_Stats;

	void			ClearSummaryStats();
#endif // IMPLEMENT_UPDATESUMMARYSTATS

	void		CreateUniqueCaseCaptureIds( COptions& inPluginOptions, COptions& inContextOptions, bool bWritePrefs );

	void		PeriodicProcessing( UInt64 inCurrentTime, bool bOverride = false );
#ifdef IMPLEMENT_PLUGINMESSAGE
	int			ProcessGetOptions( CPeekMessage* ioMessage );

#endif // IMPLEMENT_PLUGINMESSAGE

	void		SetCapturing( const bool inCapturing ) { m_bCapturing = inCapturing; }
	void		SetCaptureStopping( bool bCaptureIsStopping ); //, bool bHaveLock  )
	int			StartCaptureForReal( COptions& inContextOptions );

	void		UpdateOptions( CPeekDataModeler& ioPrefs );

	void		ValidateOptions();


public:
	;				COmniEngineContext( CGlobalId& inId, CRemotePlugin*	inRemotePlugin );
	virtual			~COmniEngineContext();

	virtual COptions&	GetOptions() { return m_Options; }

	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs );
	virtual int		OnCreateContext();
	virtual int		OnDisposeContext();
	virtual int		OnFilter( CPacket& inPacket, UInt16* outBytesToAccept, CGlobalId& outFilterId );
	virtual int		OnNotify( const CGlobalId&, const CGlobalId&, UInt64,
		PeekSeverity, const CPeekString&, const CPeekString& ) { return PEEK_PLUGIN_SUCCESS; }
	virtual	int		OnProcessPacket( CPacket& ) { return PEEK_PLUGIN_SUCCESS; }
#ifdef IMPLEMENT_PLUGINMESSAGE
	virtual	int		OnProcessPluginMessage( CPeekStream& inMessage, CPeekStream& outResponse );
#endif // IMPLEMENT_PLUGINMESSAGE
	virtual int		OnProcessTime( UInt64 inCurrentTime );
	virtual	int		OnReset();
	virtual	int		OnResetCapture();
	virtual void	SetOptions( const COptions& inOptions ) { m_Options = inOptions; }
	virtual int		OnStartCapture( UInt64 inStartTime );
	virtual int		OnStopCapture( UInt64 inStopTime );

#ifdef IMPLEMENT_ALL_UPDATESUMMARY
  #ifdef IMPLEMENT_UPDATESUMMARYSTATS
	virtual	int		OnSummary( CSnapshot& inSnapshot ) {
		inSnapshot;
	};
  #else
	virtual	int		OnSummary() {
		return PEEK_PLUGIN_SUCCESS;
	}
  #endif // IMPLEMENT_UPDATESUMMARYSTATS
#endif // IMPLEMENT_ALL_UPDATESUMMARY


	void		IncrementPacketsDisplayedCount() {
		m_nNumScreenPackets++;
	}

	// IRadiusContext methods
	void		AddLogToScreenMessage( CPeekString strMsg ) {
		m_LogQueue.AddMessage( strMsg );
	}
	void		CreateAnUniqueCaseCaptureId( CCaseOptions* pPluginCaseOptions, CCaseOptions* pContextCaseOptions ) {
		pPluginCaseOptions->IncrementCaseCaptureId();
		pContextCaseOptions->SetCaseCaptureId( pPluginCaseOptions->GetCaseCaptureId() );
	}
	void		DoLogMessage( int inSeverity, const CPeekString& inMessage ) {
		__super::DoLogMessage( inSeverity, inMessage );
	}
#ifdef DISPLAY_PACKET_NUMBERS
	UInt32		GetCurrentDisplayPacketNumber() {
		return m_nNumScreenPackets + 1;
	}
#endif // DISPLAY_PACKET_NUMBERS
	const bool	IsCapturing() {
			return m_bCapturing;
	}
};

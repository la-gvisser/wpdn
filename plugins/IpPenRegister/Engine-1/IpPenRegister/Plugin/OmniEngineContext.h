// =============================================================================
//	OmniEngineContext.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekPlugin.h"
#include "PeekEngineContext.h"
#include "PluginMessages.h"


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
	COptions				m_Options;

protected:
#ifdef IMPLEMENT_UPDATESUMMARYSTATS
	struct SummaryStats {
		Peek::SummaryStatEntry	ExampleStat;
	};
	SummaryStats			m_Stats;

	void			ClearSummaryStats();
#endif // IMPLEMENT_UPDATESUMMARYSTATS

#ifdef IMPLEMENT_PLUGINMESSAGE
	int				ProcessGetOptions( CPeekMessage* ioMessage );
#endif // IMPLEMENT_PLUGINMESSAGE

public:
	;				COmniEngineContext( CGlobalId& inId, CRemotePlugin*	inRemotePlugin );
	virtual			~COmniEngineContext();

	virtual COptions&	GetOptions() { return m_Options; }

	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs );
	virtual int		OnCreateContext();
	virtual int		OnDisposeContext();
	virtual int		OnFilter( CPacket& inPacket, UInt16* outBytesToAccept, CGlobalId& outFilterId );
	virtual int		OnNotify( const CGlobalId& inContextId, const CGlobalId& inSourceId, UInt64 inTimeStamp,
						PeekSeverity inSeverity, const CPeekString& inShortMessage, const CPeekString& inLongMessage );
	virtual	int		OnProcessPacket( CPacket& inPacket );
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
	virtual	int		OnSummary( CSnapshot& inSnapshot );
#else
	virtual	int		OnSummary();
#endif // IMPLEMENT_UPDATESUMMARYSTATS
#endif // IMPLEMENT_ALL_UPDATESUMMARY
};

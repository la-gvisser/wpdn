// =============================================================================
//	OmniEngineContext.h
// =============================================================================
//	Copyright (c) 2010-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "PeekPlugin.h"
#include "PeekEngineContext.h"
#include "PluginMessages.h"
#include "PeekLock.h"


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Omni Engine Context
//
//	An Omni Engine Context is the plugin specific that is created by the
//	OmniEngine (the engine).
//
//  Capture Messages:
//  OnReset()
//  OnResetCapture();
//  OnCaptureStarting();	The capture context will begin setup for capture.		Omni 7.0
//  OnStartCaptureing();	The capture context has been setup to capture.
//  OnCaptureStarted();		The capture context is capturing.						Omni 7.0
//  OnCaptureStopping();	The capture context will start to shutdown capturing.	Omni 7.0
//  OnStopCaptureing();		The capture context has stopped capturing.
//  OnCaptureStopped();		The capture context has torn down the capture.			Omni 7.0
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
	CPeekCriticalSection	m_Mutex;
	CStatsHandler			m_StatsHandler;

#ifdef IMPLEMENT_PLUGINMESSAGE
	int		ProcessGetAdapterOptions( CPeekMessage* ioMessage );
#endif	// IMPLEMENT_PLUGINMESSAGE

public:
	;				COmniEngineContext( CGlobalId& inId, CRemotePlugin*	inRemotePlugin );
#ifdef IMPLEMENT_SUMMARY
	;				COmniEngineContext( CGlobalId& inId, CRemoteStatsPlugin* inRemotePlugin );
#endif	// IMPLEMENT_SUMMARY
	virtual			~COmniEngineContext();

	virtual COptions&	GetOptions() { return m_Options; }

	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs );
	virtual int		OnCreateContext();
	virtual int		OnDisposeContext();
	virtual int		OnNotify( const CGlobalId& /*inContextId*/, const CGlobalId& /*inSourceId*/,
		UInt64 /*inTimeStamp*/, PeekSeverity /*inSeverity*/, const CPeekString& /*inShortMessage*/,
		const CPeekString& /*inLongMessage */) {
		return PEEK_PLUGIN_SUCCESS;
	}
#ifdef IMPLEMENT_PLUGINMESSAGE
	virtual	int		OnProcessPluginMessage( CPeekStream& inMessage, CPeekStream& outResponse );
#endif	// IMPLEMENT_PLUGINMESSAGE
	virtual	int		OnReset();
	virtual int		OnProcessTime( UInt64 /*inCurrentTime*/ ) {
		return PEEK_PLUGIN_SUCCESS;
	}
	virtual	int		OnSummary( IHeUnknown* inSnapshot );

	virtual void	SetOptions( const COptions& inOptions ) { m_Options = inOptions; }
};

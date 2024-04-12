// =============================================================================
//	OmniEngineContext.h
// =============================================================================
//	Copyright (c) 2010-2017 Savvius, Inc. All rights reserved.

#pragma once

#include "PeekPlugin.h"
#include "PeekEngineContext.h"
#include "PluginMessages.h"
#include "PeekLock.h"

[!if NOTIFY_CODEEXAMPLES]

////////////////////////////////////////////////////////////////////////
// Code Examples
// Caution: uncommented either of the following #defines will cause
// writes to disk on the OmniEngine computer:
//#define EXAMPLE_DECODE
//#define EXAMPLE_FILECAPTUREBUFFER
//#define EXAMPLE_DATATABLE

#ifdef EXAMPLE_DECODE
#include "CaptureBuffer.h"
#endif // EXAMPLE_DECODE
#ifdef EXAMPLE_FILECAPTUREBUFFER
#include "FileCaptureBuffer.h"
#endif // EXAMPLE_FILECAPTUREBUFFER
#ifdef EXAMPLE_DATATABLE
#include "PeekTable.h"
#endif // EXAMPLE_DATATABLE
//
////////////////////////////////////////////////////////////////////////
[!endif]

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

[!if NOTIFY_CODEEXAMPLES]
#ifdef IMPLEMENT_CODEEXAMPLES
////////////////////////////////////////////////////////////////////////
// Code Examples
protected:
  #ifdef EXAMPLE_DECODE
	CPeekString		m_strFileName;
	CCaptureBuffer	m_CaptureBuffer;
	int				ExampleDecodePacket( CPacket& inPacket );
  #endif // EXAMPLE_DECODE
	int				ExamplePacketMembers( CPacket& inPacket );

  #ifdef EXAMPLE_FILECAPTUREBUFFER
	CFileCaptureBuffer	m_FileCaptureBuffer;
	void				ExampleBufferOperations();
  #endif // EXAMPLE_FILECAPTUREBUFFER

	void			ExampleContextPrefs();
	void			ExampleCreateContext();
	void			ExampleDisposeContext();
	void			ExampleFilter( CPacket& inPacket );
	void			ExampleInitialize();
#ifdef IMPLEMENT_PLUGINMESSAGE
[!if NOTIFY_TAB && NOTIFY_UIHANDLER]
	int				ExampleProcessTabMessage( CPeekMessage* ioMessage );
[!endif]
[!if !NOTIFY_UIHANDLER]
	int				ExampleProcessExample( CPeekMessage* ioMessage );
[!endif]
#endif // IMPLEMENT_PLUGINMESSAGE
	void			ExampleProcessPacket( CPacket& inPacket );
	void			ExampleProcessTime();
	void			ExampleReset();
	void			ExampleStartCapture();
	void			ExampleStopCapture();
	void			ExampleSummary();
#ifdef EXAMPLE_DATATABLE
	void			DataTableExample();
#endif
//
////////////////////////////////////////////////////////////////////////
#endif // IMPLEMENT_CODEEXAMPLES

[!endif]
public:
	;				COmniEngineContext( CGlobalId& inId, CRemotePlugin*	inRemotePlugin );
	virtual			~COmniEngineContext();

	virtual COptions&	GetOptions() { return m_Options; }
	virtual void		SetOptions( const COptions& inOptions ) { m_Options = inOptions; }

#ifdef IMPLEMENT_PACKETPROCESSOREVENTS
	virtual int		OnCaptureStarting();
	virtual int		OnCaptureStarted();
	virtual int		OnCaptureStopping();
	virtual int		OnCaptureStopped();
#endif	// IMPLEMENT_PACKETPROCESSOREVENTS
	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs );
	virtual int		OnCreateContext();
	virtual int		OnDisposeContext();
#ifdef IMPLEMENT_FILTERPACKET
	virtual int		OnFilter( CPacket& inPacket, UInt16* outBytesToAccept, CGlobalId& outFilterId );
#endif	 // IMPLEMENT_FILTERPACKET
	virtual int		OnNotify( const CGlobalId& inContextId, const CGlobalId& inSourceId, UInt64 inTimeStamp,
						PeekSeverity inSeverity, const CPeekString& inShortMessage, const CPeekString& inLongMessage );
#ifdef IMPLEMENT_PROCESSPACKET
	virtual	int		OnProcessPacket( CPacket& inPacket );
#endif	// IMPLEMENT_PROCESSPACKET
#ifdef IMPLEMENT_PLUGINMESSAGE
	virtual	int		OnProcessPluginMessage( CPeekStream& inMessage, CPeekStream& outResponse );
#endif // IMPLEMENT_PLUGINMESSAGE
	virtual int		OnProcessTime( UInt64 inCurrentTime );
 #ifdef IMPLEMENT_RESETPROCESSING
	virtual	int		OnReset();
#endif	// IMPLEMENT_RESETPROCESSING
#ifdef IMPLEMENT_PACKETPROCESSOREVENTS
	virtual	int		OnResetCapture();
	virtual int		OnStartCapture( UInt64 inStartTime );
	virtual int		OnStopCapture( UInt64 inStopTime );
#endif	// IMPLEMENT_PACKETPROCESSOREVENTS

#ifdef IMPLEMENT_ALL_UPDATESUMMARY
#ifdef IMPLEMENT_UPDATESUMMARYSTATS
	virtual	int		OnSummary( CSnapshot& inSnapshot );
#else
	virtual	int		OnSummary();
#endif // IMPLEMENT_UPDATESUMMARYSTATS
#endif // IMPLEMENT_ALL_UPDATESUMMARY
};

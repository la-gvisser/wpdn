// =============================================================================
//	OmniEngineContext.h
// =============================================================================
//	Copyright (c) 2010-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "PeekPlugin.h"
#include "PeekEngineContext.h"
#include "PluginMessages.h"
#include "PeekLock.h"
#include "LogQueue.h"
#include "EmailStats.h"
#include "MsgAddress.h"
#include "Pop3MsgStream.h"
#include "SmtpMsgStream.h"
#include "RemotePlugin.h"


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
	bool					m_bDefaultOptions;
	CSafeOptions			m_SafeOptions;
	CPeekCriticalSection	m_Mutex;

	CLogQueue				m_LogQueue;
	CEmailStats				m_Stats;
	UInt32					m_nFailure;

	// Message Streams
	CSafeSmtpMsgStreamList	m_SafeSmtpMsgStreamList;
	CSafePop3MsgStreamList	m_SafePop3MsgStreamList;

protected:
	void			AnalyzeAndDeleteMsgStream( COptions* inOptions, CMsgStream*	inMsgStream );

	void			ProcessPOP3( COptions* inOptions, CPacket& inPacket );
	bool			ProcessSMTP( COptions* inOptions, CPacket& inPacket, UInt16& outSliceLength );

protected:
	int				ProcessGetOptions( CPeekMessage* ioMessage );

public:
	;				COmniEngineContext( CGlobalId& inId, CRemotePlugin*	inRemotePlugin );
	virtual			~COmniEngineContext();

	const CPeekString&	GetLogFileName() const;
	COmniPlugin*	GetPlugin() const {
		return dynamic_cast<COmniPlugin*>( m_pPlugin );
	}
	virtual COptionsPtr	GetOptions() { return m_SafeOptions.Get(); }

	void			Increment( CEmailStats::EmailStat inStat ) { m_Stats.Increment( inStat ); }
	bool			IsDefault() const { return m_bDefaultOptions; }

	void			NoteDetect( bool inLogMessages, CEmailStats::EmailStatType inType );
	void			NoteDetect( bool inLogMessages, CEmailStats::EmailStatType inType, const CMsgAddress& inTarget );
	void			NoteTarget( bool inLogMessages, const CPeekString& inTarget, const CPeekString& inMessage );
	void			NoteTargets( bool inLogMessages, const CArrayString& inTargets, const CPeekString& inMessage );
	void			NoteFileSave( bool inLogMessages, const CPeekString& inTarget );
	void			NoteReset( bool inLogMessages, CEmailStats::EmailStatType inType );
	void			NoteReset( bool inLogMessages, CEmailStats::EmailStatType inType, const CMsgAddress& inTarget );
	void			NoteTimedOutMessages( bool inLogMessages, CEmailStats::EmailStatType inType, const CArrayString& inRemovedList );
	void			NoteMalformed( CEmailStats::EmailStatType inType );
	void			NoteFileError( const CPeekString& inFileName );
	void			NoteWriteError( const CPeekString& inFileName );
	void			NoteFailure( const CPeekString& inMessage );

	virtual int		OnCaptureStarting();
	virtual int		OnCaptureStarted();
	virtual int		OnCaptureStopping();
	virtual int		OnCaptureStopped();
	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs );
	virtual int		OnCreateContext();
	virtual int		OnDisposeContext();
	virtual int		OnFilter( CPacket& inPacket, UInt16* outBytesToAccept, CGlobalId& outFilterId );
	virtual int		OnNotify( const CGlobalId& inContextId, const CGlobalId& inSourceId, UInt64 inTimeStamp,
						PeekSeverity inSeverity, const CPeekString& inShortMessage, const CPeekString& inLongMessage );
	virtual	int		OnProcessPacket( CPacket& inPacket );
	virtual	int		OnProcessPluginMessage( CPeekStream& inMessage, CPeekStream& outResponse );
	virtual int		OnProcessTime( UInt64 inCurrentTime );
	virtual	int		OnReset();
	virtual	int		OnResetCapture();
	virtual int		OnStartCapture( UInt64 inStartTime );
	virtual int		OnStopCapture( UInt64 inStopTime );
	virtual	int		OnSummary( CSnapshot& inSnapshot );

	virtual void	SetOptions( const COptions& inOptions ) {
		COptionsPtr	pOptions = GetOptions();
		pOptions->Copy( inOptions );
	}
	void			SetOptions( CPeekDataModeler& inPrefs );
};

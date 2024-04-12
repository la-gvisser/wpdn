// =============================================================================
//	OmniEngineContext.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekPlugin.h"
#include "PeekEngineContext.h"
#include "PluginMessages.h"
#include "LogQueue.h"
#include "EmailStats.h"
#include "MsgAddress.h"
#include "Pop3MsgStream.h"
#include "SmtpMsgStream.h"
#include "RemotePlugin.h"


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Omni Engine Context
//
//	An Omni Engine Context is the plugin specific that is created by the
//	OmniEngine (the engine).
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

class COmniPlugin;


// =============================================================================
//		COmniEngineContext
// =============================================================================

class COmniEngineContext
	:	public CPeekEngineContext
{
protected:
	static COptions	s_DefaultOptions;

protected:
	bool			m_bDefaultOptions;
	CSafeOptions	m_SafeOptions;

	CLogQueue		m_LogQueue;
	CEmailStats		m_Stats;
	UInt32			m_nFailure;

	// Message Streams
	CSafeSmtpMsgStreamList	m_SafeSmtpMsgStreamList;
	CSafePop3MsgStreamList	m_SafePop3MsgStreamList;

protected:
	void			AnalyzeAndDeleteMsgStream( COptions* inOptions, CMsgStream*	inMsgStream );

	void			ProcessPOP3( COptions* inOptions, CPacket& inPacket );
	bool			ProcessSMTP( COptions* inOptions, CPacket& inPacket, UInt16& outSliceLength );

protected:
#ifdef IMPLEMENT_PLUGINMESSAGE
	int				ProcessGetOptions( CPeekMessage* ioMessage );
#endif // IMPLEMENT_PLUGINMESSAGE

public:
	;				COmniEngineContext( CGlobalId& inId );
	virtual			~COmniEngineContext();

	const CPeekString&	GetLogFileName() const;
	COmniPlugin*	GetPlugin() const {
		return dynamic_cast<COmniPlugin*>( m_pPlugin );
	}
	COptionsPtr		GetOptions() { return m_SafeOptions.Get(); }

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
	virtual void	SetOptions( const COptions& inOptions ) {
		COptionsPtr	pOptions = GetOptions();
		pOptions->Copy( inOptions );
	}
	virtual int		OnStartCapture( UInt64 inStartTime );
	virtual int		OnStopCapture( UInt64 inStopTime );

#ifdef IMPLEMENT_ALL_UPDATESUMMARY
#ifdef IMPLEMENT_UPDATESUMMARYSTATS
	virtual	int		OnSummary( CSnapshot& inSnapshot );
#else
	virtual	int		OnSummary();
#endif // IMPLEMENT_UPDATESUMMARYSTATS
#endif // IMPLEMENT_ALL_UPDATESUMMARY

	void			SetOptions( CPeekDataModeler& inPrefs );
};

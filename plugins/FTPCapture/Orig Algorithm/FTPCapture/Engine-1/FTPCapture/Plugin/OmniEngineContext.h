	// =============================================================================
//	OmniEngineContext.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekPlugin.h"
#include "PeekEngineContext.h"
#ifdef IMPLEMENT_PLUGINMESSAGE
  #include "PluginMessages.h"
#endif // IMPLEMENT_PLUGINMESSAGE
#include "FtpSession.h"
#include "DataChannel.h"
#include "LogQueue.h"

extern const UInt32	kMonitorTime;


// =============================================================================
//		COmniEngineContext
// =============================================================================

class COmniEngineContext
	:	public CPeekEngineContext
{
	friend class CPluginStatistics;

public:
	enum {
		kAction_Logon,
		kAction_Retrieve,
		kAction_Store,
		kAction_Max
	};
	enum {
		kFtpCmd_Retrieve,
		kFtpCmd_Store,
		kFtpCmd_Max
	};

protected:
	static COptions			s_DefaultOptions;

	CLogQueue				m_Log;
	CPluginStatistics		m_Stats;
	CFtpSessionList			m_FtpSessions;
	CDataChannelList		m_DataChannels;

protected:
	COptions				m_Options;

	void					Clean() {
	  #ifdef IMPLEMENT_MONITORING
		DoMonitor( kMonitorTime );
	  #endif // IMPLEMENT_MONITORING

		m_FtpSessions.Close();
		m_DataChannels.Close();
		m_Log.Clean();
	}

	void			InitFtp() {
		m_Log.Init( this );
		m_Stats.Init( this );
	}

	void			ReportAlert( int inAction, CFtpSession* inSession, CPeekString inFileName );

public:
	;				COmniEngineContext( CGlobalId& inId, CRemotePlugin*	inPlugin );
	virtual			~COmniEngineContext();

	virtual COptions&	GetOptions() { return m_Options; }

#ifdef IMPLEMENT_MONITORING
	void			DoMonitor( DWORD dwTimeOut );
#endif // IMPLEMENT_MONITORING

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
	virtual void	SetOptions( const COptions& inOptions ) { m_Options = inOptions; }
	virtual int		OnStartCapture( UInt64 inStartTime );
	virtual int		OnStopCapture( UInt64 inStopTime );

	virtual	int		OnSummary();
};

// =============================================================================
//	FtpProcess.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc.

#include "FtpSession.h"
#include "LogQueue.h"
#include "Options.h"
#include "PeekEngineContext.h"


// =============================================================================
//	CFtpProcess
// =============================================================================

class CFtpProcess
	: IFtpProcess
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

	COptions				m_Options;

	CLogQueue				m_Log;
	CPluginStatistics		m_Stats;
	CFtpSessionList			m_FtpSessions;
	CDataChannelList		m_DataChannels;
	CPeekEngineContext*		m_PeekContext;
  #ifdef IMPLEMENT_MONITORING
	UInt32					m_nMonitorCount;
  #endif // IMPLEMENT_MONITORING

	COptions&	GetOptions() { return m_Options; }

	bool		IsLoggingToScreen() { return m_Options.IsLoggingToScreen(); }

	void		ReportAlert( const int inAction, CFtpSession* inSession, const CPeekString& inFileName );

public:

	;					CFtpProcess() 
		:	m_PeekContext( NULL )
		  #ifdef IMPLEMENT_MONITORING
		,	m_nMonitorCount( 0 ) 
		  #endif // IMPLEMENT_MONITORING
		{}
	virtual				~CFtpProcess(){}

	virtual void		AddDataChannel( CFtpSession* inFtpSession ) {
		m_DataChannels.Add( inFtpSession );
	}
	virtual void		AddLogMessage( const CPeekString& inMessage ) {
		ASSERT( IsLoggingToScreen() );
		m_Log.AddMessage( inMessage );
	}
	virtual void		AddLogMessage( const CPeekOutString& inMessage ) {
		ASSERT( IsLoggingToScreen() );
		m_Log.AddMessage( const_cast<CPeekOutString&>( inMessage ) );
	}
	virtual void		AddSession( CFtpSession* inFtpSession ) {
		m_FtpSessions.Add( inFtpSession );
	}

	virtual void	IncrementStats( size_t nStat ) {
		m_Stats.Increment( nStat );
	}

	virtual void		RemoveDataChannel(  CFtpSession* inFtpSession ) {
		m_DataChannels.Remove( inFtpSession );
	}
	virtual void		RemoveFtpSession(  CFtpSession* inFtpSession ) {
		m_FtpSessions.Remove( inFtpSession );
	}

	void		Clean() {
	  #ifdef IMPLEMENT_MONITORING
		DoMonitor( true );
	  #endif // IMPLEMENT_MONITORING
		ProcessSummary();
		m_FtpSessions.Reset();
		m_DataChannels.Reset();
		m_Log.Reset();
	}

	void		InitFtp( COmniEngineContext* inContext ) {
		m_PeekContext = reinterpret_cast<CPeekEngineContext*>( inContext );
		m_Log.Init( reinterpret_cast<CPeekEngineContext*>( inContext ) );
		m_Stats.Init( this );
	}


  #ifdef IMPLEMENT_MONITORING
	void		DoMonitor( bool bImmediate = false );
  #endif // IMPLEMENT_MONITORING

	bool		FilterPacket( 	CPacket& inPacket );

  #ifdef IMPLEMENT_ALL_UPDATESUMMARY
	void		ProcessSummary();
  #endif // IMPLEMENT_ALL_UPDATESUMMARY

	void		Reset() {
		Clean();
		ResetStats();
	}

	void ResetStats() {
		m_Stats.Reset();
	}

	void	SetOptions( const COptions& inOptions ) {
		m_Options = inOptions;
	}
};

// =============================================================================
//	PeekEngineContext.h
// =============================================================================
//	Copyright (c) 2009-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekProxy.h"
#include "PeekContext.h"
#include "Packet.h"
#include "PeekMessage.h"
#include "Snapshot.h"
#include "PeekContextProxy.h"
#include "Options.h"


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Peek Engine Context
//
//	A Peek Engine Context is a Framework object that is created by the
//	OmniEngine (the engine).
//
//	Peek Engine Context is the base class of an Omni Engine Context.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		CPeekEngineContext
// =============================================================================

class CPeekEngineContext
	:	public CPeekContext
{
	friend class CRemotePlugin;

protected:
	CPeekEngineProxy		m_EngineProxy;

public:
	;	CPeekEngineContext( const CGlobalId& inId, CRemotePlugin* inRemotePlugin )
		:	CPeekContext( inId, inRemotePlugin )
	{
	}
	virtual		~CPeekEngineContext() {}

	// Framework

	void		DoLogMessage( const CPeekString& inMessage ) {
		GetLogService().DoLogMessage( m_Id,  m_Id, 0, 0, peekSeverityInformational, inMessage, inMessage );
	}
	void		DoLogMessage( int inSeverity, const CPeekString& inMessage ) {
		GetLogService().DoLogMessage( m_Id,  m_Id, 0, 0, inSeverity, inMessage, inMessage );
	}
	void		DoLogMessage( CGlobalId& inContextId, CGlobalId& inSourceId, UInt32 inSourceKey, UInt64 inTimeStamp,
		int inSeverity, const CPeekString& inShortMessage, const CPeekString& inLongMessage ) {
			GetLogService().DoLogMessage( inContextId, inSourceId, inSourceKey, inTimeStamp,
				inSeverity, inShortMessage, inLongMessage );
	}

  #ifdef IMPLEMENT_NOTIFICATIONS
	void		DoNotify( const CPeekString& inNotifyMessage ) {
		GetNotifyService().DoNotify( m_Id,  m_Id, 0, 0, 0, inNotifyMessage, inNotifyMessage );
	}
	void		DoNotify( CGlobalId& inContextId, CGlobalId& inSourceId, UInt32 inSourceKey, UInt64 inTimeStamp,
		int inSeverity, const CPeekString& inShortMessage, const CPeekString& inLongMessage ) {
		GetNotifyService().DoNotify( inContextId, inSourceId, inSourceKey, inTimeStamp,
			inSeverity, inShortMessage, inLongMessage );
	}
  #endif // IMPLEMENT_NOTIFICATIONS

	//
	// Context-Specific Callback Implementation
	//
	void		DoInsertPacket( CPacket& inPacket ) {
		m_PeekContextProxy.GetPacketHandler().HandlePacket( inPacket );
	}
  #ifdef IMPLEMENT_SUMMARYMODIFYENTRY
	int			DoSummaryModifyEntry( const CPeekString& inLabel, const CPeekString& inGroup, UInt32 inType, void* inData ) {
		if ( CPeekProxy::IsConsole() ) return PEEK_PLUGIN_SUCCESS;
		return ( m_PeekContextProxy.SummaryModifyEntry( inLabel, inGroup, inType, inData ) );
	}
  #endif // IMPLEMENT_SUMMARYMODIFYENTRY

	const CPeekString&			GetCaptureName() const { return m_PeekContextProxy.GetCaptureName(); }

	virtual CPeekEngineProxy*	GetProxy() { return &m_EngineProxy; }

	CLogService&		GetLogService() { return m_EngineProxy.GetLogService(); }
	CMediaUtils			GetMediaUtils() { return m_EngineProxy.GetMediaUtils(); }

  #ifdef IMPLEMENT_NOTIFICATIONS
	CNotifyService&		GetNotifyService() { return m_EngineProxy.GetNotifyService(); }
  #endif // IMPLEMENT_NOTIFICATIONS
	CNameTable			GetNameTable() { return m_EngineProxy.GetNameTable(); }
	virtual COptions&	GetOptions() = 0;
	CPeekContextProxy*	GetPeekContextProxy() { return &m_PeekContextProxy; }
	CProtospecs			GetProtospecs() { return m_EngineProxy.GetProtospecs(); }

	//
	// Functions Invoked From the Engine or Console
	//
	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs ) = 0;
	virtual int		OnCreateContext() { return CPeekContext::OnCreateContext(); };
	virtual int		OnDisposeContext() { return CPeekContext::OnDisposeContext(); };
//	virtual int		OnInitGraphsStats() = 0;
//	virtual int		OnTerminateGraphsStats() = 0;
	virtual int		OnFilter( CPacket& inPacket, UInt16* outBytesToAccept, CGlobalId& outFilterId ) = 0;
	virtual int		OnNotify( const CGlobalId& inContextId, const CGlobalId& inSourceId, UInt64 inTimeStamp,
						PeekSeverity inSeverity, const CPeekString& inShortMessage, const CPeekString& inLongMessage ) = 0;
	virtual	int		OnProcessPacket( CPacket& inPacket ) = 0;
  #ifdef IMPLEMENT_PLUGINMESSAGE
	virtual	int		OnProcessPluginMessage( CPeekStream& inMessage, CPeekStream& outResponse ) = 0;
  #endif // IMPLEMENT_PLUGINMESSAGE
	virtual int		OnProcessTime( UInt64 inCurrentTime ) = 0;
	virtual	int		OnReset() = 0;
//	virtual int		OnResetGraphsStats() = 0;
	virtual	int		OnResetCapture() = 0;
	virtual int		OnStartCapture( UInt64 inStartTime ) = 0;
	virtual int		OnStopCapture(UInt64 inStopTime ) = 0;

	virtual void	SetInterfacePointers( IHeUnknown* inSite, CPeekPlugin* inPlugin ) {
		CPeekContext::SetInterfacePointers( inSite, inPlugin );
		if ( inSite ) {
			CRemotePlugin*	pRemotePlugin = NULL;
			if ( inPlugin != NULL ) {
				pRemotePlugin = inPlugin->GetRemotePlugin();
			}
			m_EngineProxy.SetInterfacePointers( inSite, pRemotePlugin );
		}
	}
	virtual void	SetOptions( const COptions& inOptions ) = 0;

  #ifdef IMPLEMENT_ALL_UPDATESUMMARY
    #ifdef IMPLEMENT_UPDATESUMMARYSTATS
	  virtual	int		OnSummary( CSnapshot& outSnapshot ) = 0;
//	  virtual	int		OnUpdateGraphsStats( CSnapshot& outSnapshot ) = 0;
    #else
	  virtual	int		OnSummary() = 0;
    #endif // IMPLEMENT_UPDATESUMMARYSTATS
  #endif // IMPLEMENT_ALL_UPDATESUMMARY
};

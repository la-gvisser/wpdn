// =============================================================================
//	OmniEngineContext.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekPlugin.h"
#include "PeekEngineContext.h"
#include "CMIPacketList.h"
#ifdef IMPLEMENT_PLUGINMESSAGE
#include "PluginMessages.h"
#endif // IMPLEMENT_PLUGINMESSAGE
#include <vector>


typedef std::unique_ptr<CPacket>	CPacketPtr;
typedef std::vector<CPacketPtr>		CPacketPtrList;


// =============================================================================
//		COmniEngineContext
// =============================================================================

class COmniEngineContext
	:	public CPeekEngineContext
{
protected:
	typedef enum {
		kMode_CaptureThread,
		kMode_OnSummary,
		kMode_OnProcessTime,
		kMode_Max
	} MonitorMode;

	COptions				m_Options;
	bool					m_bFailure;
 	CFragmentMap			m_FragmentMap;
	MonitorMode				m_MonitorMode;
	UInt64					m_nLastSaveTime;
	CCmiIpV4PacketList		m_Orphans;
	mutable bool			m_bIsInserting;

#ifdef _DEBUG
  #define DEBUG_STATS
  #ifdef DEBUG_STATS
	UInt64					m_nCmiPacketNumber;
	UInt64					m_nInputPacketNumber;
	UInt64					m_nNonSpuriousTestPacketNum;
	UInt64					m_nInsertErrors;
	UInt64					m_nPackets;
	UInt64					m_nPacketsCreated;
	UInt64					m_nPacketsDeleted;
	UInt64					m_nPacketsDeletedInInsert;
	UInt64					m_nPacketsDeletedInProcess;
	UInt64					m_nOrphans;
	UInt64					m_nOrphanFiles;
	UInt64					m_nIPIDOrphans;
  #endif
#endif // _DEBUG

	void			LogError( CPeekString strErrorMessage, bool	bFormatMessage = false );

#ifdef IMPLEMENT_PLUGINMESSAGE
	int				ProcessGetOptions( CPeekMessage* ioMessage );
#endif // IMPLEMENT_PLUGINMESSAGE

public:
	;				COmniEngineContext( CGlobalId& inId, CRemotePlugin*	inRemotePlugin );
	virtual			~COmniEngineContext();

	bool			BuildNewPacket( COptions& inOptions, CCmiIpV4PacketListPtr& inPacketList,
		std::vector<UInt8>& inPayload, CCmiPacketPtr& outPacket );

	void			CheckForOrphans();
	bool			CheckAndSetMonitorMode( MonitorMode inMonitorMode ) {
		if ( inMonitorMode < m_MonitorMode ) return false;
		m_MonitorMode = inMonitorMode; 
		return true;
	}
	void			CheckSaveOrphans();
	void			ClearMaps() { m_FragmentMap.clear(); }
	void			ClearPackets();

	UInt64			GetCurrentPeekTime() { return (CPeekTime::Now()).i; }

	void			InsertPackets( CCmiPacketPtrList& inPacketList );
	bool			IsCapturing() { return m_PeekContextProxy.GetCapture().IsCapturing(); }
	bool			IsInserting() { return m_bIsInserting; }
	bool			IsMonitorMode( MonitorMode inMonitorMode ) const { return (m_MonitorMode == inMonitorMode); }

	void			LogMessage( CPeekString	inMessage );

	void			ProcessPacket( COptions& inOptions, CCmiIpV4PacketPtr& inPacket,
		CCmiPacketPtrList& inNewPacketList );

	void			ResetMonitorMode() { m_MonitorMode = kMode_CaptureThread; }

	int				Save( const CPeekString& inFilePath, const CCmiIpV4PacketList& inPacketList, UInt32& outFileSize );
	bool			SaveOrphans();
	void			SetInserting( bool bInserting ) { m_bIsInserting = bInserting; }
	void			SetMonitorMode( MonitorMode inMonitorMode ) { m_MonitorMode = inMonitorMode; }

	virtual COptions&	GetOptions() { return m_Options; }

#ifdef I_NOT_USED
	virtual int		OnCaptureStarting();
	virtual int		OnCaptureStarted();
	virtual int		OnCaptureStopping();
	virtual int		OnCaptureStopped();
#endif // I_NOT_USED
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

#if defined(IMPLEMENT_ALL_UPDATESUMMARY) && defined(IMPLEMENT_UPDATESUMMARYSTATS)
	virtual	int		OnSummary( CSnapshot& inSnapshot );
#else
	virtual	int		OnSummary();
#endif // IMPLEMENT_UPDATESUMMARYSTATS && IMPLEMENT_ALL_UPDATESUMMARY
};


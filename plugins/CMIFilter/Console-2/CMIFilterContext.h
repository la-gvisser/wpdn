// ============================================================================
// CMIFilterContext.h:
//      interface for the CCMIFilterContext class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "PeekApp.h"
#include "PeekContext.h"
#include "PacketList.h"
#include "XMutex.h"
#include "Comparator.h"

class CParentStatic;
class COptionsStatus;

#define OPT_EVENT	0


// ============================================================================
//		CCMIFilterContext
// ============================================================================

class CCMIFilterContext
	:	virtual public CPeekContext
	,	public CBroadcaster
{
private:
	typedef enum {
		kMode_CaptureThread,
		kMode_OnSummary,
		kMode_OnProcessTime,
		kMode_Max
	} MonitorMode;

	class CFragmentOffsetComparator
		:	public CComparator
	{
	public:
		virtual SInt32		Compare( const void* inItem1, const void* inItem2,
			UInt32 /*inSize1*/, UInt32 /*inSize2*/ ) const
		{
			const CIPv4Packet*	pPacket1 = *reinterpret_cast<CIPv4Packet* const *>( inItem1 );
			const CIPv4Packet*	pPacket2 = *reinterpret_cast<CIPv4Packet* const *>( inItem2 );

			UInt16 t1 = pPacket1->GetIPHeader().GetFragmentOffset();
			UInt16 t2 = pPacket2->GetIPHeader().GetFragmentOffset();

			if ( t1 > t2 ) {
				return 1;
			}
			if ( t1 < t2 ) {
				return -1;
			}
			return 0;
		}
	};

	CFragmentOffsetComparator m_FragmentOffsetComparator;

	class CTimestampComparator
		:	public CComparator
	{
	public:
		virtual SInt32	Compare( const void* inItem1, const void* inItem2,
			UInt32 /*inSize1*/, UInt32 /*inSize2*/ ) const
		{
			const CIPv4Packet* pPacket1 = *reinterpret_cast<CIPv4Packet* const *>( inItem1 );
			const CIPv4Packet* pPacket2 = *reinterpret_cast<CIPv4Packet* const *>( inItem2 );

			UInt64 t1 = pPacket1->GetTimeStamp();
			UInt64 t2 = pPacket2->GetTimeStamp();

			if ( t1 > t2 ) {
				return 1;
			}
			if ( t1 < t2 ) {
				return -1;
			}
			return 0;
		}
	};

private:
    CCMIFilterPlugin*		m_pPlugin;
	CSafeOptions			m_SafeOptions;
	CString					m_strPrefs;
	HWND					m_hTabWnd;
	CParentStatic*			m_pTabParent;
	COptionsStatus*			m_pOptionsTab;
	bool					m_bFailure;

#if _DEBUG
	UInt64					m_PacketNumber;
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

#if OPT_EVENT
	HANDLE					m_hEvent;
	CWinThread*				m_pThread;
#endif
	MonitorMode				m_MonitorMode;
	bool					m_bCapturing;
	UInt64					m_nLastSaveTime;

	mutable const UInt8*	m_pInsertedPacket;
	CSafePacketList			m_SafeNewPackets;
	CIPv4PacketList			m_Orphans;
	CSafeMaps				m_SafeMaps;

	bool			BuildNewPacket( COptions* inOptions, const CIPv4PacketList* inPacketList,
						CByteArray& inData, CAutoPtr<CPacket>& outPacket );
	void			CheckForOrphans( COptions* inOptions, CFragmentMap* inFragMap,
						CAddressMap* inAddrMap );
	void			CheckSaveOrphans( COptions* inOptions );
	void			ClearPackets();
	void			InsertPackets( CPacketList* inPacketList );
	void			LogMessage( CString inMessage );
	bool			ProcessPacket( COptions* inOptions, CAutoPtr<CIPv4Packet> inPacket,
						CFragmentMap* inFragmentMap, CAddressMap* inAddressMap,
						CPacketList* inNewPacketList );
#if OPT_EVENT
	//void			ProcessPackets( COptions* inOptions );
	//void			RunCaptureThread();
#endif
	DWORD			Save( const CString& inFilePath, CIPv4PacketList* inBuffer,
						int inVersion, UInt32* outFileSize );
	bool			SaveOrphans( COptions* inOptions );

	bool			CheckAndSetMonitorMode( MonitorMode inMonitorMode ) {
		if ( inMonitorMode < m_MonitorMode ) return false;
		m_MonitorMode = inMonitorMode; 
		return true;
	}
	void			ResetMonitorMode() { m_MonitorMode = kMode_CaptureThread; }
	void			SetMonitorMode( MonitorMode inMonitorMode ) { m_MonitorMode = inMonitorMode; }
	bool			IsMonitorMode( MonitorMode inMonitorMode ) const { return (m_MonitorMode == inMonitorMode); }

public:
	;				CCMIFilterContext( CCMIFilterPlugin* pPlugin );
	virtual			~CCMIFilterContext();

	virtual int		Init( PluginCreateContextParam* ioParams );
	virtual int		Term();
	bool			GetMaps( CFragmentMapPtr& spFragMap, CAddressMapPtr& spAddrMap ) {
		return m_SafeMaps.Get( spFragMap, spAddrMap );
	}
	CPacketListPtr	GetNewPacketList() { return m_SafeNewPackets.Get(); }
	COptionsPtr		GetOptions() { return m_SafeOptions.Get(); }
	bool			IsCapturing() const { return m_bCapturing; }
	void			SetOptions( const COptions& inOptions ) { m_SafeOptions.Copy( inOptions ); }

	// CListener
	virtual void	ListenToMessage( CBLMessage& ioMessage );

	virtual int		OnReset( PluginResetParam* ioParams );
	virtual int		OnStartCapture( PluginStartCaptureParam* ioParams );
	virtual int		OnStopCapture( PluginStopCaptureParam* ioParams );
	virtual int		OnProcessPacket( PluginProcessPacketParam* ioParams );
	virtual int		OnGetPacketString( PluginGetPacketStringParam* ioParams );
	virtual int		OnApply( PluginApplyParam* ioParams );
	virtual int		OnSelect( PluginSelectParam* ioParams );
	virtual int		OnPacketsLoaded( PluginPacketsLoadedParam* ioParams );
	virtual int		OnSummary( PluginSummaryParam* ioParams );
	virtual int		OnFilter( PluginFilterParam* ioParams );
	virtual int		OnGetPacketAnalysis( PluginGetPacketStringParam* ioParams );
	virtual int		OnProcessPluginMessage( PluginProcessPluginMessageParam* ioParams );
	virtual int		OnHandleNotify( PluginHandleNotifyParam* ioParams );
	virtual int		OnContextOptions( PluginContextOptionsParam* ioParams );
	virtual int		OnSetContextPrefs( PluginContextPrefsParam* ioParams );
	virtual int		OnGetContextPrefs( PluginContextPrefsParam* ioParams );
	virtual int		OnProcessTime( PluginProcessTimeParam* ioParams );
};

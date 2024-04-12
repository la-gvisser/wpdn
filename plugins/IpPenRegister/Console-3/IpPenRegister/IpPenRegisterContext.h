// ============================================================================
// IpPenRegisterContext.h:
//      interface for the CIpPenRegisterContext class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "PeekApp.h"
#include "PeekContext.h"
#include "Options.h"

class CIpPenRegisterPlugin;


// ============================================================================
//		CIpPenRegisterContext
// ============================================================================

class CIpPenRegisterContext
	:	virtual public CPeekContext
{
public:
	typedef struct _tMacAddresses {
		tMacAddress	Source;
		tMacAddress	Destination;
	} tMacAddresses;

protected:
	CIpPenRegisterPlugin*	m_pPlugin;
	COptions				m_Options;

public:
	;					CIpPenRegisterContext( CPeekApp* pPeekApp, CIpPenRegisterPlugin* pPlugin );
	virtual				~CIpPenRegisterContext() {};

	virtual int			Init( PluginCreateContextParam* ioParams );
	virtual int			Term();

	virtual COptions*		GetOptions() { return &m_Options; }
	CIpPenRegisterPlugin*	GetPlugin() { return m_pPlugin; }

	// CListener
	virtual void		ListenToMessage( const CBLMessage& inMessage );

	virtual int			OnReset( PluginResetParam* ioParams );
	virtual int			OnStartCapture( PluginStartCaptureParam* ioParams );
	virtual int			OnStopCapture( PluginStopCaptureParam* ioParams );
	virtual int			OnProcessPacket( PluginProcessPacketParam* ioParams );
	virtual int			OnGetPacketString( PluginGetPacketStringParam* ioParams );
	virtual int			OnApply( PluginApplyParam* ioParams );
	virtual int			OnSelect( PluginSelectParam* ioParams );
	virtual int			OnPacketsLoaded( PluginPacketsLoadedParam* ioParams );
	virtual int			OnSummary( PluginSummaryParam* ioParams );
	virtual int			OnFilter( PluginFilterParam* ioParams );
	virtual int			OnGetPacketAnalysis( PluginGetPacketStringParam* ioParams );
	virtual int			OnDecodePacket( PluginDecodePacketParam* ioParams );
	virtual int			OnProcessPluginMessage( PluginProcessPluginMessageParam* ioParams );
	virtual int			OnHandleNotify( PluginHandleNotifyParam* ioParams );
	virtual int			OnGetTextForPacketListCell( PluginGetTextForPacketListCellParam* ioParams );
	virtual int			OnMeasurePacketListCell( PluginMeasurePacketListCellParam* ioParams );
	virtual int			OnDrawPacketListCell( PluginDrawPacketListCellParam* ioParams );
	virtual int			OnContextOptions( PluginContextOptionsParam* ioParams );
	virtual int			OnSetContextPrefs( PluginContextPrefsParam* ioParams );
	virtual int			OnGetContextPrefs( PluginContextPrefsParam* ioParams );
	virtual int			OnProcessTime( PluginProcessTimeParam* ioParams );
};


// ============================================================================
//		CIpPenRegisterContextList
// ============================================================================

class CIpPenRegisterContextList
	:	public CAtlArray<CIpPenRegisterContext*>
{
public:
	void	Remove( CIpPenRegisterContext* inContext );
#ifdef __GNUC__
	size_t	GetCount() const { return size(); }
	CIpPenRegisterContext*	GetAt( size_t nIndex ) { return *(begin() + nIndex); }
	void	RemoveAt( size_t nIndex ) { erase( begin() + nIndex ); }
	void	Add( CIpPenRegisterContext* inContext ) { push_back( inContext ); }
	void	Copy( const CIpPenRegisterContextList& inSrc ) {
		clear();
		CIpPenRegisterContextList::iterator	pItem = begin();
		while ( pItem < end() ) {
			push_back( *pItem );
			pItem++;
		}
	}
#endif
};


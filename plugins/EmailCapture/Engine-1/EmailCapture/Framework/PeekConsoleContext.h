// =============================================================================
//	PeekConsoleContext.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekProxy.h"
#include "PeekContext.h"
#include "Packet.h"
#include "PeekPlugin.h"
#include "PeekMessage.h"
#include "Snapshot.h"
#include "PeekContextProxy.h"


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Peek Console Context
//
//	A Peek Console Context is a Framework object that is created by OmniPeek
//	(the console).
//
//	Peek Console Context is the base class of an Omni Console Context.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		CPeekConsoleContext
// =============================================================================

class CPeekConsoleContext
	:	public CPeekContext
{
	friend class CRemotePlugin;

protected:
	CPeekConsoleProxy	m_ConsoleProxy;
	CConsoleUI			m_ConsoleUI;
#ifdef IMPLEMENT_PLUGINMESSAGE
	CPeekMessageQueue	m_MessageQueue;
#endif // IMPLEMENT_PLUGINMESSAGE

public:
	;	CPeekConsoleContext( const CGlobalId& inId )
		:	CPeekContext( inId )
		#ifdef IMPLEMENT_PLUGINMESSAGE
			,	m_MessageQueue( this )
		#endif // IMPLEMENT_PLUGINMESSAGE
	{
	}
	virtual			~CPeekConsoleContext() {}

#ifdef IMPLEMENT_PLUGINMESSAGE
	bool				PostPluginMessage( CPeekMessagePtr inMessage,
		CPeekMessageProcess* inMsgProc, FnResponseProcessor inResponseProc );
	virtual bool		SendPluginMessage( CPeekStream& inMessage,
		DWORD inTimeout, DWORD& outTransId );
#endif // IMPLEMENT_PLUGINMESSAGE

	CMediaUtils			GetMediaUtils() { return m_ConsoleProxy.GetMediaUtils(); }
	virtual COptionsPtr	GetOptions() = 0;
	CProtospecs			GetProtospecs() { return m_ConsoleProxy.GetProtospecs(); }

	virtual CConsoleUI			GetConsoleUI() { return m_ConsoleUI; }
	virtual CPeekConsoleProxy*	GetProxy() { return &m_ConsoleProxy; }

	virtual int			OnContextPrefs( CPeekDataModeler& ioPrefs ) = 0;
	virtual int			OnCreateContext() { return CPeekContext::OnCreateContext(); }
	virtual int			OnDisposeContext() { return CPeekContext::OnDisposeContext(); }
#ifdef IMPLEMENT_SUMMARIZEPACKET
	virtual	int			OnGetPacketString( CPacket& inPacket, CPeekString& outSummary,
		UInt32& outColor, Peek::SummaryLayer* inSummaryLayer ) = 0;
#endif // IMPLEMENT_SUMMARIZEPACKET
	virtual	int			OnLoadView() { return PEEK_PLUGIN_SUCCESS; }
	virtual	bool		OnOptions() = 0;
#ifdef IMPLEMENT_PLUGINMESSAGE
	virtual	int			OnProcessPluginResponse(
		CPeekStream& inMessage, HRESULT inMsgResult, DWORD inTransId ) = 0;
#endif // IMPLEMENT_PLUGINMESSAGE
	virtual	int			OnUnloadView() { return PEEK_PLUGIN_SUCCESS; }

	virtual void		SetConsoleUI( IConsoleUI* inConsoleUI ) { m_ConsoleUI.SetPtr( inConsoleUI ); }
	virtual void		SetInterfacePointers( IHeUnknown* inSite, CPeekPlugin* inPlugin ) {
		CPeekContext::SetInterfacePointers( inSite, inPlugin );
		if ( inSite ) {
			m_ConsoleProxy.SetInterfacePointers( inSite );
		}
	}
	virtual void		SetOptions( const COptions& inOptions ) = 0;
};

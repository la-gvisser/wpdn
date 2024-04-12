// =============================================================================
//	PeekConsoleContext.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekProxy.h"
#include "PeekContext.h"
#include "Packet.h"
#include "PeekMessage.h"
#include "Snapshot.h"
#include "PeekContextProxy.h"


// =============================================================================
//		CPeekConsoleContext
// =============================================================================

class CPeekConsoleContext
	:	public CPeekContext
{
	friend class CRemotePlugin;

protected:
	CPeekConsoleProxy	m_ConsoleProxy;

public:
	;	CPeekConsoleContext( const CGlobalId& inId )
		:	CPeekContext( inId )
	{
	}
	virtual			~CPeekConsoleContext() {}

#ifdef IMPLEMENT_PLUGINMESSAGE
	virtual bool		DoSendPluginMessage( CPeekStream& inMessage, DWORD inTimeout, DWORD& outTransId );
#endif // IMPLEMENT_PLUGINMESSAGE

	CMediaUtils			GetMediaUtils() { return m_ConsoleProxy.GetMediaUtils(); }
	virtual COptions&	GetOptions() = 0;
	CProtospecs			GetProtospecs() { return m_ConsoleProxy.GetProtospecs(); }

	virtual CPeekConsoleProxy*	GetProxy() { return &m_ConsoleProxy; }

	virtual int			OnContextPrefs( CPeekDataModeler& ioPrefs ) = 0;
	virtual int			OnCreateContext() { return CPeekContext::OnCreateContext(); }
	virtual int			OnDisposeContext() { return CPeekContext::OnDisposeContext(); }
#ifdef IMPLEMENT_SUMMARIZEPACKET
	virtual	int			OnGetPacketString( CPacket& inPacket, CPeekString& outSummary, UInt32& outColor, Peek::SummaryLayer* inSummaryLayer ) = 0;
#endif // IMPLEMENT_SUMMARIZEPACKET
#ifdef IMPLEMENT_PLUGINMESSAGE
	virtual	int			OnProcessPluginResponse( CPeekStream& inMessage, HRESULT inMsgResult, DWORD inTransId ) = 0;
#endif // IMPLEMENT_PLUGINMESSAGE

	virtual void	SetInterfacePointers( IHeUnknown* inSite, CRemotePlugin* inPlugin ) {
		CPeekContext::SetInterfacePointers( inSite, inPlugin );
		if ( inSite ) {
			m_ConsoleProxy.SetInterfacePointers( inSite, inPlugin );
		}
	}
	virtual void		SetOptions( const COptions& inOptions ) = 0;
};

// =============================================================================
//	OmniConsoleContext.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekPlugin.h"
#include "PeekConsoleContext.h"
#include "Options.h"
#include "TabHost.h"
#ifdef IMPLEMENT_PLUGINMESSAGE
  #include "PluginMessages.h"
#endif // IMPLEMENT_PLUGINMESSAGE


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Omni Console Context
//
//	An Omni Console Context is a plugin specific object that is created by
//	OmniPeek (the console).
//
//	An Omni Console Context may have a Context Tab.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		OmniConsoleContext
// =============================================================================

class COmniConsoleContext
	:	public CPeekConsoleContext
  #ifdef IMPLEMENT_PLUGINMESSAGE
	,	public CPeekMessageProcess
  #endif // IMPLEMENT_PLUGINMESSAGE
{
protected:
	COptions	m_Options;
	CTabHost	m_TabHost;

#ifdef IMPLEMENT_PLUGINMESSAGE
protected:
	void		GetOptionsResponse( HeResult inResult, CPeekMessagePtr inMessage,
		const CPeekStream& inResponse );
#endif // IMPLEMENT_PLUGINMESSAGE

public:
	;		COmniConsoleContext( CGlobalId& inId, CRemotePlugin* inRemotePlugin )
		: CPeekConsoleContext( inId, inRemotePlugin )
	{
	}
	virtual	~COmniConsoleContext() {}

	virtual COptions&	GetOptions() { return m_Options; }

	bool			HasTab() const { return m_TabHost.IsValid(); }

	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs );
	virtual int		OnCreateContext();
	virtual int		OnDisposeContext();
#ifdef IMPLEMENT_SUMMARIZEPACKET
	virtual	int		OnGetPacketString( CPacket& inPacket, CPeekString& outSummary,
		UInt32& outColor, Peek::SummaryLayer* inSummaryLayer );
#endif // IMPLEMENT_SUMMARIZEPACKET
	virtual int		OnLoadView();
	virtual	bool	OnOptions();
#ifdef IMPLEMENT_PLUGINMESSAGE
	virtual	int		OnProcessPluginResponse( CPeekStream& inMessage,
		HRESULT inMsgResult, DWORD inTransId );
#endif // IMPLEMENT_PLUGINMESSAGE
	virtual int		OnUnloadView();

#ifdef IMPLEMENT_PLUGINMESSAGE
	// CPeekMessageProcess
	void		ProcessPluginResponse( HeResult inResult, CPeekMessagePtr inMessage,
		const CPeekStream& inResponse );
#endif // IMPLEMENT_PLUGINMESSAGE

	virtual void	SetOptions( const COptions& inOptions ) {
		m_Options = inOptions;
	}
};

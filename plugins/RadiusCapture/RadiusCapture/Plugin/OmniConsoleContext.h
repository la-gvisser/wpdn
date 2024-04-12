// =============================================================================
//	OmniConsoleContext.h
// =============================================================================
//	Copyright (c) 2010-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekPlugin.h"
#include "PeekConsoleContext.h"
#include "Options.h"
#include "TabHost.h"
#include "PluginMessages.h"


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
#ifdef IMPLEMENT_TAB
	CTabHost	m_TabHost;
#endif // IMPLEMENT_TAB

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

#ifdef IMPLEMENT_TAB
	bool			HasTab() const { return m_TabHost.IsValid(); }
#endif // IMPLEMENT_TAB

	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs );
	virtual int		OnCreateContext();
	virtual int		OnDisposeContext();
	virtual int		OnLoadView();
	virtual	bool	OnOptions();
	virtual int		OnUnloadView();

#ifdef IMPLEMENT_PLUGINMESSAGE
	virtual	int		OnProcessPluginResponse( CPeekStream& inMessage,
		HRESULT inMsgResult, DWORD inTransId );

	// CPeekMessageProcess
	void		ProcessPluginResponse( HeResult inResult, CPeekMessagePtr inMessage,
		const CPeekStream& inResponse );
#endif // IMPLEMENT_PLUGINMESSAGE

	virtual void	SetOptions( const COptions& inOptions ) {
		m_Options = inOptions;
	}
};

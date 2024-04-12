// =============================================================================
//	OmniConsoleContext.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

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
	,	public CPeekMessageProcess
{
protected:
	CSafeOptions	m_SafeOptions;
	CTabHost		m_TabHost;

protected:
	void		GetOptionsResponse( HeResult inResult, CPeekMessagePtr inMessage,
		const CPeekStream& inResponse );

public:
	;		COmniConsoleContext( CGlobalId& inId )
		:	CPeekConsoleContext( inId )
	{
	}
	virtual	~COmniConsoleContext() {}

	virtual COptionsPtr	GetOptions() { return m_SafeOptions.Get(); }

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
	virtual int		OnUnloadView();

	virtual	int		OnProcessPluginResponse( CPeekStream& inMessage,
		HRESULT inMsgResult, DWORD inTransId );

	// CPeekMessageProcess
	void		ProcessPluginResponse( HeResult inResult, CPeekMessagePtr inMessage,
		const CPeekStream& inResponse );

	virtual void	SetOptions( const COptions& inOptions ) {
		COptionsPtr	pOptions = GetOptions();
		pOptions->Copy( inOptions );
	}
};

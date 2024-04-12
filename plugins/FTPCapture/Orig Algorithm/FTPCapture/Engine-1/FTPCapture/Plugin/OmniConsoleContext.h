// =============================================================================
//	OmniConsoleContext.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekPlugin.h"
#include "PeekConsoleContext.h"
#include "Options.h"
#ifdef IMPLEMENT_PLUGINMESSAGE
  #include "PluginMessages.h"
#endif // IMPLEMENT_PLUGINMESSAGE


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

#ifdef IMPLEMENT_PLUGINMESSAGE
	CPeekMessageQueue	m_MessageQueue;

#endif // IMPLEMENT_PLUGINMESSAGE

public:
	;				COmniConsoleContext( CGlobalId& inId )
		: CPeekConsoleContext( inId )
	  #ifdef IMPLEMENT_PLUGINMESSAGE
		, m_MessageQueue( this )
	  #endif // IMPLEMENT_PLUGINMESSAGE
	{
	}
	virtual			~COmniConsoleContext() {}

	virtual COptions&	GetOptions() { return m_Options; }

	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs );
	virtual int		OnCreateContext();
	virtual int		OnDisposeContext();
#ifdef IMPLEMENT_PLUGINMESSAGE
	virtual	int		OnProcessPluginResponse( CPeekStream& inMessage,
		HRESULT inMsgResult, DWORD inTransId );
#endif // IMPLEMENT_PLUGINMESSAGE
	virtual void	SetOptions( const COptions& inOptions ) { m_Options = inOptions; }

#ifdef IMPLEMENT_SUMMARIZEPACKET
	virtual	int		OnGetPacketString( CPacket& inPacket,
		CPeekString& outSummary, UInt32& outColor,
		Peek::SummaryLayer* inSummaryLayer );
#endif // IMPLEMENT_SUMMARIZEPACKET

#ifdef IMPLEMENT_PLUGINMESSAGE
	// CPeekMessageProcess
	void		ProcessPluginResponse( HeResult inResult, CPeekMessagePtr inMessage,
		const CPeekStream& inResponse );
#endif // IMPLEMENT_PLUGINMESSAGE
};

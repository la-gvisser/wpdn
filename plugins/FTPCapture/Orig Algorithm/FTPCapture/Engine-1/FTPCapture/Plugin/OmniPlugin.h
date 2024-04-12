// =============================================================================
//	OmniPlugin.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "resource.h"
#include "PeekPlugin.h"
#include "PeekProxy.h"
#include "Options.h"
#include "GlobalId.h"
#include "TabHost.h"
#ifdef IMPLEMENT_PLUGINMESSAGE
  #include "PluginMessages.h"
#endif


// =============================================================================
//		COmniPlugin
// =============================================================================

class COmniPlugin
	:	public CPeekPlugin
{
protected:
	static CGlobalId	s_idClass;
	static CPeekString	s_strName;

protected:
	bool				m_bInitialized;
	CTabHost			m_TabHost;
#ifdef IMPLEMENT_PLUGINMESSAGE
	CPeekMessageQueue	m_MessageQueue;
#endif

public:
	static CGlobalId			GetClassId() { return s_idClass; }
	static const CPeekString&	GetName();

protected:
#ifdef IMPLEMENT_PLUGINMESSAGE
	int					ProcessGetAllOptions( CPeekMessage* ioMessage );
	int					ProcessSetOptions( CPeekMessage* ioMessage );
#endif //IMPLEMENT_PLUGINMESSAGE

public:
	;				COmniPlugin();
	virtual			~COmniPlugin() {}

#ifdef IMPLEMENT_PLUGINMESSAGE
	virtual bool	DoSendPluginMessage( CGlobalId inId, CPeekStream& inMessage,
			DWORD inTimeout, DWORD& outTransId );
	virtual bool	DoSendPluginMessage( CPeekStream& inMessage,
			DWORD inTimeout, DWORD& outTransId ) {
		return DoSendPluginMessage( GUID_NULL, inMessage, inTimeout, outTransId );
	}
	virtual bool	DoPostPluginMessage( CPeekMessagePtr inMessage,
		CPeekMessageProcess* inMsgProc, FnResponseProcessor inResponseProc );
#endif //IMPLEMENT_PLUGINMESSAGE

	HINSTANCE		GetInstanceHandle() const;

	bool			IsInitialized() const { return m_bInitialized; }

	virtual	int		OnAbout();
	virtual	int		OnLoad( CRemotePlugin* inPlugin, const CPeekProxy& inPeekProxy );
	virtual	bool	OnOptions( COptions& ioOptions );
#ifdef IMPLEMENT_PLUGINMESSAGE
	virtual	int		OnProcessPluginMessage( CPeekStream& inMessage,
		CPeekStream& outResponse );
	virtual	int		OnProcessPluginResponse( CPeekStream& inMessage,
		HRESULT inMsgResult, DWORD inTransId );
#endif //IMPLEMENT_PLUGINMESSAGE
	virtual int		OnReadPrefs();
	virtual	int		OnUnload();
	virtual int		OnWritePrefs();
};

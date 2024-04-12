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


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Omni Plugin
//
//	An Omni Plugin is the plugin specific Plugin object. It is created by both
//	OmniPeek (the console) and OmniEngine (the engine).
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		COmniPlugin
// =============================================================================

class COmniPlugin
	:	public CPeekPlugin
#ifdef IMPLEMENT_PLUGINMESSAGE
	,	public CPeekMessageProcess
#endif // IMPLEMENT_PLUGINMESSAGE
{
protected:
	static CGlobalId	s_idClass;
	static CPeekString	s_strName;

protected:
	bool				m_bInitialized;
	int					m_nOnEngine;		// 0: Unknown, 1: Yes, -1: No.
	CPeekString			m_strPrefsFileName;
	COptions			m_Options;			// Forensics Options
	CTabHost			m_TabHost;
#ifdef IMPLEMENT_PLUGINMESSAGE
	CPeekMessageQueue	m_MessageQueue;
#endif

public:
	static CGlobalId			GetClassId() { return s_idClass; }
	static const CPeekString&	GetName();

protected:
#ifdef IMPLEMENT_TAB
	void				CreateTab();
#endif

	const CPeekString&	GetPrefsFileName() const { return m_strPrefsFileName; }

#ifdef IMPLEMENT_PLUGINMESSAGE
	int					ProcessPing( CPeekMessage* ioMessage );
	int					ProcessGetAllOptions( CPeekMessage* ioMessage );
	int					ProcessSetOptions( CPeekMessage* ioMessage );
#endif //IMPLEMENT_PLUGINMESSAGE

public:
	;				COmniPlugin();
	virtual			~COmniPlugin() {}

#ifdef IMPLEMENT_PLUGINMESSAGE
	virtual bool	SendPluginMessage( CPeekStream& inMessage,
			DWORD inTimeout, DWORD& outTransId ) {
		return CPeekPlugin::SendPluginMessage( GUID_NULL, inMessage, inTimeout, outTransId );
	}
	virtual bool	PostPluginMessage( CPeekMessagePtr inMessage,
		CPeekMessageProcess* inMsgProc, FnResponseProcessor inResponseProc );
#endif //IMPLEMENT_PLUGINMESSAGE

	HINSTANCE		GetInstanceHandle() const;
	virtual const COptions&	GetOptions() const { return m_Options; }

	bool			IsInitialized() const { return m_bInitialized; }

	virtual	int		OnAbout();
	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs );
	virtual	int		OnLoad( CRemotePlugin* inRemotePlugin );
	virtual	int		OnLoadView();
	virtual	bool	OnOptions();
#ifdef IMPLEMENT_PLUGINMESSAGE
	virtual	int		OnProcessPluginMessage( CPeekStream& inMessage,
		CPeekStream& outResponse );
	virtual	int		OnProcessPluginResponse( CPeekStream& inMessage,
		HRESULT inMsgResult, DWORD inTransId );
#endif //IMPLEMENT_PLUGINMESSAGE
	virtual int		OnReadPrefs();
	virtual	int		OnUnload();
	virtual int		OnWritePrefs();

	void			SetOptions( CPeekDataModeler& inPrefs );

#ifdef IMPLEMENT_PLUGINMESSAGE
	// CPeekMessageProcess
	void		ProcessPluginResponse( HeResult inResult, CPeekMessagePtr inMessage,
					const CPeekStream& inResponse );
#endif // IMPLEMENT_PLUGINMESSAGE
};


// =============================================================================
//		COmniPluginPtr
// =============================================================================

class COmniPluginPtr
	:	public NSTR1::shared_ptr<COmniPlugin>
{
public:
	;		COmniPluginPtr( COmniPlugin* inPlugin = NULL )
		:	NSTR1::shared_ptr<COmniPlugin>( inPlugin )
	{}

	;		operator COmniPlugin*() { return get(); }

	bool	IsNotValid() const { return (get() == NULL); }
	bool	IsValid() const { return (get() != NULL); }
};
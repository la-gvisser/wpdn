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
#include "PluginMessages.h"

#if defined(_WIN32) && defined(IMPLEMENT_TAB)
#include "TabHost.h"
#endif // _WIN32 && IMPLEMENT_TAB


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
#if defined(_WIN32) && defined(IMPLEMENT_PLUGINMESSAGE)
	,	public CPeekMessageProcess
#endif // _WIN32 && IMPLEMENT_PLUGINMESSAGE
{
protected:
	static CGlobalId	s_idClass;
	static CPeekString	s_strName;

protected:
	bool				m_bInitialized;
	CPeekString			m_strPrefsFileName;
	COptions			m_Options;		// Defaults from file.
#ifdef _WIN32
#ifdef IMPLEMENT_TAB
	CTabHost			m_TabHost;
#endif // IMPLEMENT_TAB
	CPeekMessageQueue	m_MessageQueue;
#endif // _WIN32

public:
	static CGlobalId			GetClassId() { return s_idClass; }
	static const CPeekString&	GetName();

protected:
#if defined(_WIN32) && defined(IMPLEMENT_TAB)
	void				CreateTab();
#endif // _WIN32 && IMPLEMENT_TAB

	const CPeekString&	GetPrefsFileName();

#ifdef IMPLEMENT_PLUGINMESSAGE
	int				ProcessPing( CPeekMessage* ioMessage );
	int				ProcessGetAllOptions( CPeekMessage* ioMessage );
	int				ProcessSetOptions( CPeekMessage* ioMessage );
#endif //IMPLEMENT_PLUGINMESSAGE

public:
	;				COmniPlugin();
	virtual			~COmniPlugin() {}

	virtual const COptions&	GetOptions() const { return m_Options; }

	bool			IsInitialized() const { return m_bInitialized; }

	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs );
	virtual	int		OnLoad( CRemotePlugin* inRemotePlugin );
	virtual	int		OnUnload();
	virtual int		OnReadPrefs();
	virtual int		OnWritePrefs();
	void			SetOptions( CPeekDataModeler& inPrefs );

	// Engine-Only Methods
	virtual int		OnGetAboutData( UInt32* outFlags, CPeekStream& outData );
	virtual int		OnGetOptionsData( UInt32* outFlags, CPeekStream& outData );
	virtual int		OnGetOptionsHandlerId( CGlobalId& outId );
#ifdef IMPLEMENT_PLUGINMESSAGE
	virtual	int		OnProcessPluginMessage( CPeekStream& inMessage,
		CPeekStream& outResponse );
#endif // IMPLEMENT_PLUGINMESSAGE

#ifdef _WIN32
	// Console-Only Methods
	HINSTANCE		GetInstanceHandle() const;

	virtual	int		OnAbout();
	virtual	int		OnLoadView();
	virtual	bool	OnOptions();

#ifdef IMPLEMENT_PLUGINMESSAGE
	virtual	int		OnProcessPluginResponse( CPeekStream& inMessage,
		HRESULT inMsgResult, UInt32 inTransId );
	virtual bool	PostPluginMessage( CPeekMessagePtr inMessage,
		CPeekMessageProcess* inMsgProc, FnResponseProcessor inResponseProc );
	virtual bool	SendPluginMessage( CPeekStream& inMessage,
			UInt32 inTimeout, UInt32& outTransId ) {
		return CPeekPlugin::SendPluginMessage(
			GUID_NULL, inMessage, inTimeout, outTransId );
	}

	// CPeekMessageProcess
	void			ProcessPluginResponse( HeResult inResult,
		CPeekMessagePtr inMessage, const CPeekStream& inResponse );
#endif // IMPLEMENT_PLUGINMESSAGE
#endif // _WIN32

};


// =============================================================================
//		COmniPluginPtr
// =============================================================================

class COmniPluginPtr
	:	public std::shared_ptr<COmniPlugin>
{
public:
	;		COmniPluginPtr( COmniPlugin* inPlugin = nullptr )
		:	std::shared_ptr<COmniPlugin>( inPlugin )
	{}

	;		operator COmniPlugin*() { return get(); }

	bool	IsNotValid() const { return (get() == nullptr); }
	bool	IsValid() const { return (get() != nullptr); }
};


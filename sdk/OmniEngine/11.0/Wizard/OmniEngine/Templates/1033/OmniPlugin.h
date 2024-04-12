// =============================================================================
//	OmniPlugin.h
// =============================================================================
//	Copyright (c) 2010-2017 Savvius, Inc. All rights reserved.

#pragma once

#include "resource.h"
#include "PeekPlugin.h"
#include "PeekProxy.h"
#include "Options.h"
#include "GlobalId.h"
[!if NOTIFY_TAB && !NOTIFY_UIHANDLER]
#include "TabHost.h"
[!endif]
#include "PeekStrings.h"
#include "PluginMessages.h"
#include "Version.h"


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
[!if !NOTIFY_UIHANDLER]
#if defined(_WIN32) && defined(IMPLEMENT_PLUGINMESSAGE)
	,	public CPeekMessageProcess
#endif // _WIN32 && IMPLEMENT_PLUGINMESSAGE
[!endif]
{
protected:
	static CGlobalId	s_idClass;
	static CPeekString	s_strName;
	static CPeekString	s_strPublisher;
	static CVersion		s_Version;
[!if NOTIFY_UIHANDLER]
#ifdef _WIN32
	static int			s_nAboutId;
	static int			s_nCaptureTabId;
	static int			s_nEngineTabId;
	static int			s_nOptionsId;
#endif // _WIN32
[!endif]

protected:
	bool				m_bInitialized;
	CPeekString			m_strPrefsFileName;
	COptions			m_Options;		// Defaults from file.
[!if NOTIFY_UIHANDLER]
	CPeekString			m_strPluginPath;
	UInt32				m_nAboutFlags;
	CPeekString			m_strAboutFilePath;
	UInt32				m_nCaptureTabFlags;
	CPeekString			m_strCaptureTabFilePath;
	UInt32				m_nEngineTabFlags;
	CPeekString			m_strEngineTabFilePath;
	UInt32				m_nOptionsFlags;
	CPeekString			m_strOptionsFilePath;
[!else]
[!if NOTIFY_TAB]
#if defined(_WIN32) && defined(IMPLEMENT_TAB)
	CTabHost			m_TabHost;
#endif // _WIN32 && IMPLEMENT_TAB
[!endif]
#if defined(_WIN32) && defined(IMPLEMENT_PLUGINMESSAGE)
	CPeekMessageQueue	m_MessageQueue;
#endif // _WIN32 && IMPLEMENT_PLUGINMESSAGE
[!endif]

public:
	static CGlobalId			GetClassId() { return s_idClass; }
	static const CPeekString&	GetName();
	static const CPeekString&	GetPublisher();
	static const CVersion&		GetVersion();

protected:
[!if NOTIFY_TAB && !NOTIFY_UIHANDLER]
#ifdef IMPLEMENT_TAB
	void				CreateTab();
#endif

[!endif]
[!if NOTIFY_UIHANDLER]
	void				BuildPluginPath();
[!endif]
#ifdef IMPLEMENT_PLUGINMESSAGE
	int					ProcessGetAllOptions( CPeekMessage* ioMessage );
[!if !NOTIFY_UIHANDLER]
	int					ProcessSetOptions( CPeekMessage* ioMessage );
[!endif]
[!if NOTIFY_CODEEXAMPLES && NOTIFY_TAB && NOTIFY_UIHANDLER]
	int					ExampleProcessTabMessage( CPeekMessage* ioMessage );
[!endif]
[!if !NOTIFY_UIHANDLER]
	int					ProcessPing( CPeekMessage* ioMessage );
[!endif]
#endif //IMPLEMENT_PLUGINMESSAGE

public:
	;				COmniPlugin();
	virtual			~COmniPlugin() {}

	virtual const COptions&	GetOptions() const { return m_Options; }

	bool			IsInitialized() const { return m_bInitialized; }

	virtual int		OnCaptureCreated( CPeekCapture inCapture );
	virtual int		OnCaptureDeleted( CPeekCapture inCapture );
	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs );
	virtual	int		OnLoad( CRemotePlugin* inRemotePlugin );
#ifdef IMPLEMENT_PLUGINMESSAGE
	virtual	int		OnProcessPluginMessage( CPeekStream& inMessage,
		CPeekStream& outResponse );
#endif // IMPLEMENT_PLUGINMESSAGE
	virtual int		OnReadPrefs();
	virtual	int		OnUnload();
	virtual int		OnWritePrefs();

	void			SetOptions( CPeekDataModeler& inPrefs );

	// Engine-Only Methods
[!if NOTIFY_UIHANDLER]
	virtual int		OnGetAboutData( UInt32* outFlags, CPeekStream& outData );
	virtual int		OnGetCaptureTabData( UInt32* outFlags, CPeekStream& outData );
	virtual int		OnGetEngineTabData( UInt32* outFlags, CPeekStream& outData );
	virtual int		OnGetOptionsData( UInt32* outFlags, CPeekStream& outData );
	virtual int		OnGetOptionsHandlerId( CGlobalId& outId );
[!else]
	// Hack: Have to support About and Options on
	// Linux so OmniPeek enables the UI.
	// In future releases OmniPeek will check the
	// local copy of the plugin.
	virtual	int		OnAbout();
	virtual	bool	OnOptions();

#ifdef _WIN32
	// Console-Only Methods
	HINSTANCE		GetInstanceHandle() const;

	virtual	int		OnLoadView();

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
[!endif]
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

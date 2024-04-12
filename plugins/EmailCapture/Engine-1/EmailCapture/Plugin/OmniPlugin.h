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
#include "PluginMessages.h"


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
	,	public CPeekMessageProcess
{
protected:
	static CGlobalId	s_idClass;
	static CPeekString	s_strName;

protected:
	bool				m_bInitialized;
	CPeekString			m_strPrefsFileName;
	CPeekString			m_strChangeLogFileName;
	COptions			m_Options;		// Defaults from file.
	CTabHost			m_TabHost;
	CPeekMessageQueue	m_MessageQueue;

public:
	static CGlobalId			GetClassId() { return s_idClass; }
	static const CPeekString&	GetName();

protected:
	void				CreateTab();

	int					ProcessPing( CPeekMessage* ioMessage );
	int					ProcessGetAllOptions( CPeekMessage* ioMessage );
	int					ProcessSetOptions( CPeekMessage* ioMessage );

public:
	;				COmniPlugin();
	virtual			~COmniPlugin() {}

	virtual bool	SendPluginMessage( CPeekStream& inMessage,
			DWORD inTimeout, DWORD& outTransId ) {
		return CPeekPlugin::SendPluginMessage( GUID_NULL, inMessage, inTimeout, outTransId );
	}
	virtual bool	PostPluginMessage( CPeekMessagePtr inMessage,
		CPeekMessageProcess* inMsgProc, FnResponseProcessor inResponseProc );

	HINSTANCE			GetInstanceHandle() const;
	const COptions&		GetOptions() const { return m_Options; }
	const CPeekString&	GetPrefsFileName() const { return m_strPrefsFileName; }
	const CPeekString&	GetLogFileName() const { return m_strChangeLogFileName; }

	bool			IsInitialized() const { return m_bInitialized; }

	virtual	int		OnAbout();
	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs );
	virtual	int		OnLoad( CRemotePlugin* inRemotePlugin );
	virtual	int		OnLoadView();
	virtual	bool	OnOptions();
	virtual	int		OnProcessPluginMessage( CPeekStream& inMessage,
		CPeekStream& outResponse );
	virtual	int		OnProcessPluginResponse( CPeekStream& inMessage,
		HRESULT inMsgResult, DWORD inTransId );
	virtual int		OnReadPrefs();
	virtual	int		OnUnload();
	virtual int		OnWritePrefs();

	void			SetOptions( CPeekDataModeler& inPrefs );

	// CPeekMessageProcess
	void			ProcessPluginResponse( HeResult inResult,
		CPeekMessagePtr inMessage, const CPeekStream& inResponse );
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
// =============================================================================
//	OmniPlugin.h
// =============================================================================
//	Copyright (c) 2010-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "resource.h"
#include "PeekPlugin.h"
#include "PeekProxy.h"
#include "Options.h"
#include "GlobalId.h"
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
{
protected:
	static CGlobalId	s_idClass;
	static CPeekString	s_strName;
	static CPeekString	s_strPublisher;
	static CVersion		s_Version;
#ifdef _WIN32
	static int			s_nAboutId;
	static int			s_nCaptureTabId;
	static int			s_nEngineTabId;
	static int			s_nOptionsId;
#endif // _WIN32

protected:
	bool				m_bInitialized;
	CPeekString			m_strPrefsFileName;
	COptions			m_Options;		// Defaults from file.
	CPeekString			m_strPluginPath;
	UInt32				m_nAboutFlags;
	CPeekString			m_strAboutFilePath;
	UInt32				m_nCaptureTabFlags;
	CPeekString			m_strCaptureTabFilePath;
	UInt32				m_nEngineTabFlags;
	CPeekString			m_strEngineTabFilePath;
	UInt32				m_nOptionsFlags;
	CPeekString			m_strOptionsFilePath;

public:
	static CGlobalId			GetClassId() { return s_idClass; }
	static const CPeekString&	GetName();
	static const CPeekString&	GetPublisher();
	static const CVersion&		GetVersion();

protected:
	void				BuildPluginPath();

#ifdef IMPLEMENT_PLUGINMESSAGE
	int					ProcessPing( CPeekMessage* ioMessage );
	int					ProcessTabMessage( CPeekMessage* ioMessage );
	int					ProcessGetAllOptions( CPeekMessage* ioMessage );
	int					ProcessSetOptions( CPeekMessage* ioMessage );
	int					ProcessGetPrefsPage( CPeekMessage* ioMessage );
#endif //IMPLEMENT_PLUGINMESSAGE

public:
	;				COmniPlugin();
	virtual			~COmniPlugin() {}

#ifdef _WIN32
	HINSTANCE				GetInstanceHandle() const;
#endif // _WIN32
	virtual const COptions&	GetOptions() const { return m_Options; }

	bool			IsInitialized() const { return m_bInitialized; }

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
	virtual int		OnGetAboutData( UInt32* outFlags, CPeekStream& outData );
	virtual int		OnGetCaptureTabData( UInt32* outFlags, CPeekStream& outData );
	virtual int		OnGetEngineTabData( UInt32* outFlags, CPeekStream& outData );
	virtual int		OnGetOptionsData( UInt32* outFlags, CPeekStream& outData );
	virtual int		OnGetOptionsHandlerId( CGlobalId& outId );
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
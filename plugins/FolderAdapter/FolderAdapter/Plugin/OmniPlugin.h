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
#include "PeekStrings.h"
#include "OmniAdapter.h"
#include "PluginMessages.h"
#include "Version.h"
#include <vector>


class CAdapterOptions;
class CPeekXml;

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
	static CPeekString	s_strDisplayName;
	static CPeekString	s_strPublisher;
	static CVersion		s_Version;
#ifdef _WIN32
	static int			s_nCaptureTabId;
	static int			s_nOptionsId;
#endif // _WIN32

protected:
	bool				m_bInitialized;
	size_t				m_nRemoteClientCount;
	CPeekString			m_strPrefsFileName;
	COptions			m_Options;
	CPeekString			m_strPluginPath;
	UInt32				m_nCaptureTabFlags;
	CPeekString			m_strCaptureTabFilePath;
	UInt32				m_nOptionsFlags;
	CPeekString			m_strOptionsFilePath;

public:
	static CGlobalId			GetClassId() { return s_idClass; }
	static const CPeekString&	GetName();
	static const CPeekString&	GetPublisher();
	static const CVersion&		GetVersion();

protected:
	void				BuildPluginPath();
	bool				CreateAdapter( CAdapterOptions& inAdapterOptions, bool inWritePrefs );
	void				DeleteAdapter( CAdapter& inAdapter );
	void				DeleteAllAdapters();
	bool				ModifyAdapter( CAdapter& inAdapter, const CAdapterOptions& inOptions );
	int					ProcessCreateAdapter( CPeekMessage* ioMessage );
	int					ProcessGetAdapter( CPeekMessage* ioMessage );
	int					ProcessModifyAdapter( CPeekMessage* ioMessage );

public:
	;				COmniPlugin();
	virtual			~COmniPlugin() {}

	size_t			AddRemotePlugin() {
		m_nRemoteClientCount++;
		return m_nRemoteClientCount;
	}
	size_t			RemoveRemotePlugin() {
		m_nRemoteClientCount--;
		return m_nRemoteClientCount;
	}

	void			DoLogMessage( UInt64 inTimeStamp, int inSeverity,
						const CPeekString& inShortMessage, const CPeekString& inLongMessage );

	virtual const COptions&	GetOptions() const { return m_Options; }

	bool			IsInitialized() const { return m_bInitialized; }

	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs );
	virtual	int		OnLoad( CRemotePlugin* inRemotePlugin );
	virtual	int		OnProcessPluginMessage( CPeekStream& inMessage,
		CPeekStream& outResponse );
	virtual int		OnReadPrefs();
	virtual	int		OnUnload( CRemotePlugin* inRemotePlugin );
	virtual int		OnWritePrefs();

	void			SetOptions( CPeekDataModeler& inPrefs );

	// Engine-Only Methods
	virtual int		OnDeleteAdapter( CGlobalId inAdapterId );
	virtual int		OnGetCaptureTabData( UInt32* outFlags, CPeekStream& outData );
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

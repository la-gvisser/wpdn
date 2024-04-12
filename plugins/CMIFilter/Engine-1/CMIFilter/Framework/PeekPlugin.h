// =============================================================================
//	PeekPlugin.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "GlobalId.h"
#include "PeekProxy.h"
#include "PeekMessage.h"

class CRemotePlugin;
class COptions;


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Peek Plugin
//
//	A Peek Plugin is the Framework object of the Plugin. It is created by both
//	OmniPeek (the console) and OmniEngine (the engine).
//	The Peek Plugin is the base class of the Omni Plugin.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		CPeekPlugin
// =============================================================================

class CPeekPlugin
{
public:
	enum {
		kOnEngine_Unknown,
		kOnEngine_True,
		kOnEngine_False = -1
	};

protected:
	CRemotePlugin*	m_pRemotePlugin;
	int				m_nOnEngine;	// 0: Unknown, 1: Yes, -1: No.
#ifdef _WIN32
	CConsoleSend	m_ConsoleSend;
	CEngineInfo		m_EngineInfo;
#endif	// _WIN32

public:
	;				CPeekPlugin()
		:	m_pRemotePlugin( nullptr )
#ifdef IMPLEMENT_PLUGINMESSAGE
		,	m_nOnEngine( kOnEngine_Unknown )
#else  // IMPLEMENT_PLUGINMESSAGE
		,	m_nOnEngine( kOnEngine_True )
#endif // IMPLEMENT_PLUGINMESSAGE
	{}
	virtual			~CPeekPlugin() {}

#ifdef _WIN32
	CConsoleSend	GetConsoleSend() const { return m_ConsoleSend; }
	CEngineInfo		GetEngineInfo() { return m_EngineInfo; }
#endif	// _WIN32
	virtual const COptions&	GetOptions() const = 0;
	CRemotePlugin*	GetRemotePlugin() const { return m_pRemotePlugin; }

#ifdef _WIN32
	bool			HasConsoleSend() const { return m_ConsoleSend.IsValid(); }
	bool			HasEngineInfo() const { return m_EngineInfo.IsValid(); }
#else
	bool			HasConsoleSend() const { return false; }
	bool			HasEngineInfo() const { return false; }
#endif
	bool			HasRemotePlugin() const { return (m_pRemotePlugin != nullptr); }

#ifdef _WIN32
	bool			IsInstalledOnEngine() const { return (m_nOnEngine == kOnEngine_True); }
#else
	bool			IsInstalledOnEngine() const { return true; }
#endif

	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs ) = 0;
	virtual	int		OnLoad( CRemotePlugin* inPlugin );

	virtual int		OnReadPrefs() = 0;
	virtual	int		OnUnload() = 0;
	virtual int		OnWritePrefs() = 0;

	virtual void	SetOptions( CPeekDataModeler& inPrefs ) = 0;
#if _WIN32
	void			SetInstalledOnEngine( int inState ) { m_nOnEngine = inState; }
#else
	void			SetInstalledOnEngine( int /*inState*/ ) {}
#endif

	// Engine-Only Methods
	virtual int		OnGetAboutData( UInt32* outFlags, CPeekStream& outData ) = 0;
	virtual int		OnGetOptionsData( UInt32* outFlags, CPeekStream& outData ) = 0;
	virtual int		OnGetOptionsHandlerId( CGlobalId& outId ) = 0;

#ifdef _WIN32
	// Console-Only Methods
	virtual	int		OnAbout() = 0;
	virtual	int		OnLoadView() { return PEEK_PLUGIN_SUCCESS; }
	virtual	bool	OnOptions() = 0;

#ifdef IMPLEMENT_PLUGINMESSAGE
	virtual bool	SendPluginMessage( CGlobalId inContextId,
		CPeekStream& inMessage, UInt32 inTimeout, UInt32& outTransId );
#endif // IMPLEMENT_PLUGINMESSAGE

	void			SetConsoleSend( IConsoleSend* inConsoleSend ) {
		m_ConsoleSend.SetPtr( inConsoleSend );
	}
	void			SetEngineInfo( IEngineInfo* inEngineInfo ) {
		m_EngineInfo.SetPtr( inEngineInfo );
	}
#endif // _WIN32
};

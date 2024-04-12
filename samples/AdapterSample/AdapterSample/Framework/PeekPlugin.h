// =============================================================================
//	PeekPlugin.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

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
#ifdef IMPLEMENT_PLUGINMESSAGE
	int				m_nOnEngine;	// 0: Unknown, 1: Yes, -1: No.
#endif
	CConsoleSend	m_ConsoleSend;
	CEngineInfo		m_EngineInfo;

public:
	;				CPeekPlugin() : m_pRemotePlugin( NULL ), m_nOnEngine( kOnEngine_Unknown ) {}
	virtual			~CPeekPlugin() {}

#ifdef IMPLEMENT_PLUGINMESSAGE
	virtual bool	SendPluginMessage( CGlobalId inContextId,
		CPeekStream& inMessage, DWORD inTimeout, DWORD& outTransId );
#endif // IMPLEMENT_PLUGINMESSAGE

	CConsoleSend	GetConsoleSend() const { return m_ConsoleSend; }
	CEngineInfo		GetEngineInfo() { return m_EngineInfo; }
#ifdef IMPLEMENT_PLUGINMESSAGE
	int				GetInstalledOnEngine() const { return m_nOnEngine; }
#endif
	virtual const COptions&	GetOptions() const = 0;
	CRemotePlugin*	GetRemotePlugin() const { return m_pRemotePlugin; }

	bool			HasConsoleSend() const { return m_ConsoleSend.IsValid(); }
	bool			HasEngineInfo() const { return m_EngineInfo.IsValid(); }
	bool			HasRemotePlugin() const { return (m_pRemotePlugin != NULL); }

#ifdef IMPLEMENT_PLUGINMESSAGE
	bool			IsInstalledOnEngine() const { return (m_nOnEngine == kOnEngine_True); }
#endif

	virtual	int		OnAbout() = 0;
	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs ) = 0;
	virtual	int		OnLoad( CRemotePlugin* inPlugin );
	virtual	int		OnLoadView() { return PEEK_PLUGIN_SUCCESS; }
	virtual	bool	OnOptions() = 0;
	virtual int		OnReadPrefs() = 0;
	virtual	int		OnUnload() = 0;
	virtual int		OnWritePrefs() = 0;

	void			SetConsoleSend( IConsoleSend* inConsoleSend ) {
		m_ConsoleSend.SetPtr( inConsoleSend );
	}
	void			SetEngineInfo( IEngineInfo* inEngineInfo ) {
		m_EngineInfo.SetPtr( inEngineInfo );
	}
	void			SetInstalledOnEngine( int inState ) {
		m_nOnEngine = inState;
	}
	virtual void	SetOptions( CPeekDataModeler& inPrefs ) = 0;
};

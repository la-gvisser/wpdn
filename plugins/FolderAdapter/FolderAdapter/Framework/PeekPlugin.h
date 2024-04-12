// =============================================================================
//	PeekPlugin.h
// =============================================================================
//	Copyright (c) 2008-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "GlobalId.h"
#include "PeekDataModeler.h"
#include "PeekProxy.h"
#include "PeekMessage.h"

#include <vector>

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
protected:
	mutable CPeekCriticalSection			m_Mutex;
	mutable std::vector<CRemotePlugin*>		m_ayRemotePlugins;

	CRemoteStatsPlugin*	m_pRemoteStatsPlugin;
	CPeekEngineProxy	m_EngineProxy;

public:
	;				CPeekPlugin()
		:	m_pRemoteStatsPlugin( nullptr )
	{}
	virtual			~CPeekPlugin() {
		PeekLock lock( m_Mutex );
		m_ayRemotePlugins.clear();
		m_pRemoteStatsPlugin = nullptr;
	}

	virtual const COptions&	GetOptions() const = 0;
	CRemotePlugin*			GetRemotePlugin() const; // { return m_pRemotePlugin; }
	CRemoteStatsPlugin*		GetRemoteStatsPlugin() const { return m_pRemoteStatsPlugin; }

	bool			HasRemotePlugin() const {
		PeekLock lock( m_Mutex );
		return !m_ayRemotePlugins.empty();
	}

	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs ) = 0;
	virtual	int		OnLoad( CRemotePlugin* inPlugin );

	virtual int		OnReadPrefs() = 0;
	virtual	int		OnUnload( CRemotePlugin* inPlugin );
	virtual int		OnWritePrefs() = 0;

	virtual void	SetOptions( CPeekDataModeler& inPrefs ) = 0;

	// Engine-Only Methods
#ifdef IS_ADAPTER
	virtual int		OnDeleteAdapter( CGlobalId inAdapterId ) = 0;
#else
	virtual int		OnGetAboutData( UInt32* outFlags, CPeekStream& outData ) = 0;
#endif
#ifdef IMPLEMENT_HAS_TABS
	virtual int		OnGetCaptureTabData( UInt32* outFlags, CPeekStream& outData ) = 0;
	virtual int		OnGetEngineTabData( UInt32* outFlags, CPeekStream& outData ) = 0;
#endif
	virtual int		OnGetOptionsData( UInt32* outFlags, CPeekStream& outData ) = 0;
	virtual int		OnGetOptionsHandlerId( CGlobalId& outId ) = 0;

	bool			RegisterAdapter( CAdapter inAdapter );
	bool			ReleaseAdapter( CAdapter inAdapter );

	void			SetInterfacePointers( Helium::IHeUnknown* inUnkSite ) {
		m_EngineProxy.SetInterfacePointers( inUnkSite, nullptr );
	}
};

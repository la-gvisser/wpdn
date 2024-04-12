// =============================================================================
//	PeekPlugin.h
// =============================================================================
//	Copyright (c) 2008-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "GlobalId.h"
#include "PeekDataModeler.h"
#include "PeekProxy.h"
#include "PeekMessage.h"

class CRemotePlugin;
class COptions;

#ifdef IMPLEMENT_SUMMARY
class CRemoteStatsPlugin;
#endif	// IMPLEMENT_SUMMARY


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
	CRemotePlugin*		m_pRemotePlugin;
#ifdef IMPLEMENT_SUMMARY
	CRemoteStatsPlugin*	m_pRemoteStatsPlugin;
#endif	// IMPLEMENT_SUMMARY
	CPeekEngineProxy	m_EngineProxy;

public:
	;				CPeekPlugin()
		:	m_pRemotePlugin( nullptr )
#ifdef IMPLEMENT_SUMMARY
		,	m_pRemoteStatsPlugin( nullptr )
#endif	// IMPLEMENT_SUMMARY
	{}
	virtual			~CPeekPlugin() {}

	virtual const COptions&	GetOptions() const = 0;
	CRemotePlugin*			GetRemotePlugin() const { return m_pRemotePlugin; }
#ifdef IMPLEMENT_SUMMARY
	CRemoteStatsPlugin*		GetRemoteStatsPlugin() const { return m_pRemoteStatsPlugin; }
#endif	// IMPLEMENT_SUMMARY

	bool			HasRemotePlugin() const { return (m_pRemotePlugin != nullptr); }

	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs ) = 0;
	virtual	int		OnLoad( CRemotePlugin* inPlugin );

	virtual int		OnReadPrefs() = 0;
	virtual	int		OnUnload() = 0;
	virtual int		OnWritePrefs() = 0;

	virtual void	SetOptions( CPeekDataModeler& inPrefs ) = 0;

	// Engine-Only Methods
#ifdef IS_ADAPTER
	virtual int		OnDeleteAdapter( CGlobalId inAdapterId ) = 0;
#else
	virtual int		OnGetAboutData( UInt32* outFlags, CPeekStream& outData ) = 0;
#endif	// IS_ADAPTER
#ifdef IMPLEMENT_HAS_TABS
	virtual int		OnGetCaptureTabData( UInt32* outFlags, CPeekStream& outData ) = 0;
	virtual int		OnGetEngineTabData( UInt32* outFlags, CPeekStream& outData ) = 0;
#endif	// IMPLEMENT_HAS_TABS
	virtual int		OnGetOptionsData( UInt32* outFlags, CPeekStream& outData ) = 0;
	virtual int		OnGetOptionsHandlerId( CGlobalId& outId ) = 0;

	bool			RegisterAdapter( CAdapter inAdapter );
	bool			ReleaseAdapter( CAdapter inAdapter );

	void			SetInterfacePointers( Helium::IHeUnknown* inUnkSite ) {
		m_EngineProxy.SetInterfacePointers( inUnkSite, nullptr );
	}
};

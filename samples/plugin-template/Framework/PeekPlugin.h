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
	CRemotePlugin*	m_pRemotePlugin;

public:
	;				CPeekPlugin()
		:	m_pRemotePlugin( nullptr )
	{}
	virtual			~CPeekPlugin() {}

	virtual const COptions&	GetOptions() const = 0;
	CRemotePlugin*	GetRemotePlugin() const { return m_pRemotePlugin; }

	bool			HasRemotePlugin() const { return (m_pRemotePlugin != nullptr); }

	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs ) = 0;
	virtual	int		OnLoad( CRemotePlugin* inPlugin );

	virtual int		OnReadPrefs() = 0;
	virtual	int		OnUnload() = 0;
	virtual int		OnWritePrefs() = 0;

	virtual void	SetOptions( CPeekDataModeler& inPrefs ) = 0;

	// Engine-Only Methods
	virtual int		OnGetAboutData( UInt32* outFlags, CPeekStream& outData ) = 0;
	virtual int		OnGetCaptureTabData( UInt32* outFlags, CPeekStream& outData ) = 0;
	virtual int		OnGetEngineTabData( UInt32* outFlags, CPeekStream& outData ) = 0;
	virtual int		OnGetOptionsData( UInt32* outFlags, CPeekStream& outData ) = 0;
	virtual int		OnGetOptionsHandlerId( CGlobalId& outId ) = 0;
};

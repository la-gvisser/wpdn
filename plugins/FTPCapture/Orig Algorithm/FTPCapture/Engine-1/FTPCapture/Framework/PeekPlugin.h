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


// =============================================================================
//		CPeekPlugin
// =============================================================================

class CPeekPlugin
{
protected:
	CPeekProxy		m_PeekProxy;
	CRemotePlugin*	m_pRemotePlugin;

public:
	;				CPeekPlugin() : m_pRemotePlugin( NULL ) {}
	virtual			~CPeekPlugin() {}

	CConsoleUI		GetConsoleUI();
	CPeekProxy&		GetPeekProxy() { return m_PeekProxy; }

	virtual	int		OnAbout() = 0;
	virtual	int		OnLoad( CRemotePlugin* inPlugin, const CPeekProxy& inPeekProxy ) {
		m_pRemotePlugin = inPlugin;
		m_PeekProxy = inPeekProxy;
		return PEEK_PLUGIN_SUCCESS;
	}
	virtual	bool	OnOptions( COptions& ioOptions ) = 0;
	virtual int		OnReadPrefs() = 0;
	virtual	int		OnUnload() = 0;
	virtual int		OnWritePrefs() = 0;
};

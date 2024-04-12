// =============================================================================
//	PluginTab.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#ifdef HE_WIN32
#ifdef IMPLEMENT_TAB

#include "resource.h"
#include "afxcmn.h"
#include "PeekMessage.h"

class COmniPlugin;


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Plugin Tab
//
//	A Plugin Tab is created by the Omni Plugin and is displayed within the
//	OmniEngine's view of OmniPeek.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		CPluginTab
// =============================================================================

class CPluginTab
	:	public CDialog
	,	public CPeekMessageProcess
{
public:
	enum { IDD = IDD_PLUGIN_TAB };

public:
	;		CPluginTab( COmniPlugin*, CWnd* inParent = NULL )
		:	CDialog( CPluginTab::IDD, inParent )
	{
	}
	virtual	~CPluginTab() {}

	// CPeekMessageProcess
	void		ProcessPluginResponse( HeResult, CPeekMessagePtr, const CPeekStream& ) {
		return;
	}

	DECLARE_MESSAGE_MAP()
};

#endif // IMPLEMENT_TAB
#endif // HE_WIN32

// =============================================================================
//	ContextTab.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#ifdef HE_WIN32
#ifdef IMPLEMENT_TAB

#include "resource.h"
#include "afxcmn.h"
#include "PeekMessage.h"

class COmniConsoleContext;


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Context Tab
//
//	A Context Tab is a context (capture) specific window that is created within
//	the captures main window. It is listed in the pane on the left side of the
//	the capture window.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		CContextTab
// =============================================================================

class CContextTab
	:	public CDialog
	,	public CPeekMessageProcess
{
public:
	enum { IDD = IDD_CONTEXT_TAB };

public:
	;			CContextTab( COmniConsoleContext*, CWnd* inParent = NULL )
		:	CDialog( CContextTab::IDD, inParent )
	{
	}
	virtual		~CContextTab() {}

	// CPeekMessageProcess
	void		ProcessPluginResponse( HeResult, CPeekMessagePtr, const CPeekStream& ) {
		return;
	}

	DECLARE_MESSAGE_MAP()
};

#endif // IMPLEMENT_TAB
#endif // HE_WIN32

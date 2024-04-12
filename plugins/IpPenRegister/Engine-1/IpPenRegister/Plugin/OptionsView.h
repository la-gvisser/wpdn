// =============================================================================
//	OptionsView.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#ifdef HE_WIN32
#ifdef IMPLEMENT_TAB

#include "resource.h"
#include "afxcmn.h"
#include "PluginMessages.h"

class CPluginTab;
class COmniPlugin;


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Options View
//
//	An Options View displays the individual members of an Options object. This
//	View may be used by multiple Dialogs.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		COptionsView
// =============================================================================

class COptionsView
	:	public CDialog
	,	public CPeekMessageProcess
{
public:
	enum { IDD = IDD_OPTIONS_VIEW };

public:
	;		COptionsView( CWnd* inParent = NULL )
		:	CDialog( COptionsView::IDD, inParent )
	{
	}
	virtual	~COptionsView() {}

	// CPeekMessageProcess
	void		ProcessPluginResponse( HeResult, CPeekMessagePtr, const CPeekStream& ) {
		return;
	}

	DECLARE_MESSAGE_MAP()
};

#endif // IMPLEMENT_TAB
#endif // HE_WIN32

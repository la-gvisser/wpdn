// =============================================================================
//	OptionsView.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#ifdef HE_WIN32
#ifdef IMPLEMENT_TAB

#include "resource.h"
#include "afxcmn.h"
#include "Options.h"
#include "PluginMessages.h"
#include "PeekArray.h"

#include "OptionsDialog.h"

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
	:	public COptionsDialog
#ifdef IMPLEMENT_PLUGINMESSAGE
	,	public CPeekMessageProcess
#endif // IMPLEMENT_PLUGINMESSAGE
{
public:

protected:
	CPluginTab*	m_pParent;
	CGlobalId	m_idContext;
	CPeekString	m_strName;
	bool		m_bInitialized;
	COptions	m_Options;

protected:
	void		DoDataExchange( CDataExchange* pDX );

public:
	;			COptionsView( CWnd* inParent = NULL );
	virtual		~COptionsView() {}

	void		Refresh( CGlobalId inId, const CPeekString& inName, const COptions& inOptions );

#ifdef IMPLEMENT_PLUGINMESSAGE
	// CPeekMessageProcess
	void		ProcessPluginResponse( HeResult inResult, CPeekMessagePtr inMessage,
		const CPeekStream& inResponse );
#endif // IMPLEMENT_PLUGINMESSAGE

};

#endif // IMPLEMENT_TAB
#endif // HE_WIN32

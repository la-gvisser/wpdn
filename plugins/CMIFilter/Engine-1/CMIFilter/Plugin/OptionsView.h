// =============================================================================
//	OptionsView.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#if defined(_WIN32) && defined(IMPLEMENT_TAB)

#include "resource.h"
#include "afxcmn.h"
#include "Options.h"
#include "PluginMessages.h"
#include "PeekArray.h"

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
#ifdef IMPLEMENT_PLUGINMESSAGE
	,	public CPeekMessageProcess
#endif // IMPLEMENT_PLUGINMESSAGE
{
public:
	enum { IDD = IDD_OPTIONS_VIEW };

protected:
	CPluginTab*	m_pParent;
	CGlobalId	m_idContext;
	CPeekString	m_strName;
	bool		m_bInitialized;
	COptions	m_Options;

protected:
	void		DoDataExchange( CDataExchange* pDX );
	void		Update( CGlobalId inId, const CPeekString& inName, const COptions& inOptions );
	void		UpdateIpFilterList();

public:
	;			COptionsView( CWnd* inParent = nullptr );
	virtual		~COptionsView() {}

	BOOL		OnInitDialog();

	void		Refresh( CGlobalId inId, const CPeekString& inName, const COptions& inOptions );

#ifdef IMPLEMENT_PLUGINMESSAGE
	// CPeekMessageProcess
	void		ProcessPluginResponse( HeResult inResult, CPeekMessagePtr inMessage,
		const CPeekStream& inResponse );
#endif // IMPLEMENT_PLUGINMESSAGE

	DECLARE_MESSAGE_MAP()
	afx_msg	void	OnShowWindow( BOOL bShow, UINT nStatus );
	afx_msg void	OnSize( UINT nType, int cx, int cy );
};

#endif // _WIN32 && IMPLEMENT_TAB

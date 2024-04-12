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
	bool		m_bInitialized;
	COptions	m_Options;

protected:
	bool		ActiveWindowDisplay();

	void		DoDataExchange( CDataExchange* pDX );

public:
	;			COptionsView( CWnd* inParent = NULL );
	virtual		~COptionsView() {}

	BOOL		OnInitDialog();

	void		Refresh( const COptions& inOptions );

	afx_msg	void	OnShowWindow( BOOL bShow, UINT nStatus );
	afx_msg void	OnSize( UINT nType, int cx, int cy );

#ifdef IMPLEMENT_PLUGINMESSAGE
	// CPeekMessageProcess
	void		ProcessPluginResponse( HeResult inResult, CPeekMessagePtr inMessage,
		const CPeekStream& inResponse );
#endif // IMPLEMENT_PLUGINMESSAGE

	DECLARE_MESSAGE_MAP()

//	void			Reset() { Update( true ); }

	void			Update();
	void			UpdateResolvedDomains();
	void			UpdateCountedDomains();

//	afx_msg void	OnBnClickedModifiyNames();
};

#endif // IMPLEMENT_TAB
#endif // HE_WIN32

// =============================================================================
//	PluginTab.h
// =============================================================================
//	Copyright (c) 2010-2011 WildPackets, Inc. All rights reserved.

#pragma once

#ifdef HE_WIN32
#ifdef IMPLEMENT_TAB

#include "resource.h"
#include "afxcmn.h"
#include "DNSProcess.h"
#include "PluginMessages.h"
#include "OptionsView.h"
#include "PeekArray.h"

class CPluginTab;
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
#ifdef IMPLEMENT_PLUGINMESSAGE
	,	public CPeekMessageProcess
#endif // IMPLEMENT_PLUGINMESSAGE
{
public:
	enum { IDD = IDD_PLUGIN_TAB };

protected:
	COmniPlugin*	m_pPlugin;
	CListBox*		m_pContextList;
	COptionsView*	m_pView;

#ifdef IMPLEMENT_PLUGINMESSAGE
	CMessageGetAllOptions::CNamedOptionsList	m_ayNamedOptions;
#endif // IMPLEMENT_PLUGINMESSAGE

protected:
	void		DoDataExchange( CDataExchange* pDX );

public:
	;			CPluginTab( COmniPlugin* inPlugin, CWnd* inParent = NULL );
	virtual		~CPluginTab() { delete m_pView; }

	BOOL		OnInitDialog();

	void		Refresh();

#ifdef IMPLEMENT_PLUGINMESSAGE
	void		GetAllOptions();
	void		ProcessGetAllOptions( HeResult inResult, CPeekMessagePtr inMessage,
		const CPeekStream& inResponse );

	// CPeekMessageProcess
	void		ProcessPluginResponse( HeResult inResult, CPeekMessagePtr inMessage,
					const CPeekStream& inResponse );
#endif // IMPLEMENT_PLUGINMESSAGE

	DECLARE_MESSAGE_MAP()
	afx_msg	void	OnShowWindow( BOOL bShow, UINT nStatus );
	afx_msg void	OnSize( UINT nType, int cx, int cy );
	afx_msg void	OnLbnSelchangeContextList();
	afx_msg void	OnBnClickedRefresh();
};

#endif // IMPLEMENT_TAB
#endif // HE_WIN32
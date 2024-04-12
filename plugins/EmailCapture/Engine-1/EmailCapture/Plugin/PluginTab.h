// =============================================================================
//	PluginTab.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#ifdef PEEK_UI

#include "resource.h"
#include "afxcmn.h"
#include "Options.h"
#include "OptionsStatus.h"
#include "PluginMessages.h"

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
	,	public CPeekMessageProcess
{
public:
	enum { IDD = IDD_PLUGIN_TAB };

protected:
	COmniPlugin*		m_pPlugin;
	COptionsStatusList	m_ayOptStat;
	CMessageGetAllOptions::CNamedOptionsList	m_ayNamedOptions;

protected:
	void		DoDataExchange( CDataExchange* pDX );

	void		ProcessGetAllOptions( HeResult inResult, CPeekMessagePtr inMessage,
		const CPeekStream& inResponse );

	void		Refresh();

public:
	;			CPluginTab( COmniPlugin* inPlugin, CWnd* inParent = NULL );
	virtual		~CPluginTab();

	BOOL		OnInitDialog();

	void		SendGetAllOptions();

	// CPeekMessageProcess
	void		ProcessPluginResponse( HeResult inResult, CPeekMessagePtr inMessage,
					const CPeekStream& inResponse );

	DECLARE_MESSAGE_MAP()
	afx_msg void	OnBnClickedRefresh();
	afx_msg void	OnLbnSelchangeContextList();
	afx_msg	void	OnShowWindow( BOOL bShow, UINT nStatus );
	afx_msg void	OnSize( UINT nType, int cx, int cy );
};

#endif // PEEK_UI

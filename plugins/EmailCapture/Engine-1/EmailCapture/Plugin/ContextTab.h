// =============================================================================
//	ContextTab.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#ifdef PEEK_UI

#include "resource.h"
#include "afxcmn.h"
#include "Options.h"
#include "OptionsStatus.h"
#include "PluginMessages.h"

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

protected:
	COmniConsoleContext*	m_pContext;
	COptions				m_Options;
	COptionsStatus			m_OptStat;

protected:
	void		DoDataExchange( CDataExchange* pDX );

	void		ProcessGetOptions( HeResult inResult, CPeekMessagePtr inMessage,
		const CPeekStream& inResponse );

	void		Refresh();

public:
	;			CContextTab( COmniConsoleContext* inContext, const COptions& inOptions, CWnd* inParent = NULL );
	virtual		~CContextTab() {}

	BOOL		OnInitDialog();

	void		SendGetOptions();

	// CPeekMessageProcess
	void		ProcessPluginResponse( HeResult inResult, CPeekMessagePtr inMessage,
		const CPeekStream& inResponse );

	DECLARE_MESSAGE_MAP()
	afx_msg void	OnBnClickedRefresh();
	afx_msg void	OnSize( UINT nType, int cx, int cy );
};

#endif // PEEK_UI

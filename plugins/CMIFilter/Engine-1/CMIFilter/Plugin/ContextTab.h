// =============================================================================
//	ContextTab.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#if defined(_WIN32) && defined(IMPLEMENT_TAB)

#include "resource.h"
#include "afxcmn.h"
#include "Options.h"
#include "PluginMessages.h"
#include "OptionsView.h"

class CContextTab;
class COmniConsoleContext;


// =============================================================================
//		CContextTab
// =============================================================================

class CContextTab
	:	public CDialog
#ifdef IMPLEMENT_PLUGINMESSAGE
	,	public CPeekMessageProcess
#endif // IMPLEMENT_PLUGINMESSAGE
{
public:
	enum { IDD = IDD_CONTEXT_TAB };

protected:
	COmniConsoleContext*	m_pContext;
	COptionsView*			m_pView;

protected:
	void			DoDataExchange( CDataExchange* pDX );

#ifdef IMPLEMENT_PLUGINMESSAGE
	void		GetOptionsResponse( HeResult inResult, CPeekMessagePtr inMessage,
		const CPeekStream& inResponse );
#endif // IMPLEMENT_PLUGINMESSAGE

public:
	;			CContextTab( COmniConsoleContext* inContext, CWnd* inParent = nullptr );
	virtual		~CContextTab() { delete m_pView; }

#ifdef IMPLEMENT_PLUGINMESSAGE
	void		GetOptions();
#endif // IMPLEMENT_PLUGINMESSAGE

	BOOL		OnInitDialog();

	// CPeekMessageProcess
	void		ProcessPluginResponse( HeResult inResult, CPeekMessagePtr inMessage,
		const CPeekStream& inResponse );

	DECLARE_MESSAGE_MAP()
	afx_msg void	OnBnClickedRefresh();
	afx_msg	void	OnShowWindow( BOOL bShow, UINT nStatus );
	afx_msg void	OnSize( UINT nType, int cx, int cy );
};

#endif // _WIN32 && IMPLEMENT_TAB

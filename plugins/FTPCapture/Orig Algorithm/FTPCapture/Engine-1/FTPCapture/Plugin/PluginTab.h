// =============================================================================
//	PluginTab.h
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
	CGlobalId		m_idContext;
	CPeekString		m_strName;
	COptions	m_Options;

protected:
	void		DoDataExchange( CDataExchange* pDX );

public:
	;			COptionsView( CWnd* inParent = NULL );
	virtual		~COptionsView() {}

	BOOL		OnInitDialog();

	void		Update( CGlobalId inId, const CPeekString& inName, const COptions& inOptions );

#ifdef IMPLEMENT_PLUGINMESSAGE
	// CPeekMessageProcess
	void		ProcessPluginResponse( HeResult inResult, CPeekMessagePtr inMessage,
		const CPeekStream& inResponse );
#endif // IMPLEMENT_PLUGINMESSAGE

	DECLARE_MESSAGE_MAP()
	afx_msg	void	OnShowWindow( BOOL bShow, UINT nStatus );
	afx_msg void	OnSize( UINT nType, int cx, int cy );
};


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

	void		GetAllOptions();

	BOOL		OnInitDialog();

	void		ProcessGetAllOptions( HeResult inResult, CPeekMessagePtr inMessage,
		const CPeekStream& inResponse );

	void		Update();

	// CPeekMessageProcess
	void		ProcessPluginResponse( HeResult inResult, CPeekMessagePtr inMessage,
					const CPeekStream& inResponse );

	DECLARE_MESSAGE_MAP()
	afx_msg	void	OnShowWindow( BOOL bShow, UINT nStatus );
	afx_msg void	OnSize( UINT nType, int cx, int cy );
	afx_msg void	OnLbnSelchangeContextList();
	afx_msg void	OnBnClickedUpdate();
};

#endif // IMPLEMENT_TAB
#endif // HE_WIN32

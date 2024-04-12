// ============================================================================
//	ExportOptionsDlg.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2009. All rights reserved.

#pragma once

#include "Options.h"
#include "resource.h"
#include "afxcmn.h"
#include "PeekMessage.h"
#include "OmniPlugin.h"


////////////////////////////////////////////////////////////////////////////////
//		CExportOptionsDlg
////////////////////////////////////////////////////////////////////////////////

class CExportOptionsDlg 
	: public CDialog
#ifdef IMPLEMENT_PLUGINMESSAGE
	,	public CPeekMessageProcess
#endif // IMPLEMENT_PLUGINMESSAGE
{
	DECLARE_DYNAMIC(CExportOptionsDlg)

public:

	CExportOptionsDlg( bool bIsCapturing, COptions& inOptions, COmniPlugin* inPlugin, CWnd* pParent = NULL );
	virtual ~CExportOptionsDlg() {}

	enum { IDD_MODAL = IDD_EXPORT_OPTS_DLG };

protected:

#ifdef IMPLEMENT_PLUGINMESSAGE
	void		RequestOptionsExport( CPeekString& inFilePath );
	void		GetOptionsResponse( HeResult inResult, CPeekMessagePtr inMessage,
		const CPeekStream& inResponse );
#endif // IMPLEMENT_PLUGINMESSAGE

	void		ProcessPluginResponse( HeResult inResult, CPeekMessagePtr inMessage,
		const CPeekStream& inResponse );

	void		SetWaitForResponse( bool inStartWait );

	DECLARE_MESSAGE_MAP()

	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual BOOL	OnInitDialog();
	virtual BOOL	OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message );

	void			RefreshOptionsDisplay();
	bool			ValidateExportPath( CPeekString& inExportFile );
	void			OnTimer( UINT_PTR nIDEvent );

//	afx_msg void OnBnClickedBrowseForFile();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeExportFile();

protected:
	COptions&				m_Options;
	COmniPlugin*			m_pPlugin;
	bool					m_bWaitingForOptions;
	size_t					m_nTimer;
};

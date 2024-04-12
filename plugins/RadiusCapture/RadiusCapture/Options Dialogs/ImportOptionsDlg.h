// ============================================================================
//	ImportOptionsDlg.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2009. All rights reserved.

#pragma once

#include "Options.h"
#include "resource.h"
#include "afxcmn.h"
#include "PeekMessage.h"
#include "OmniPlugin.h"

////////////////////////////////////////////////////////////////////////////////
//		CImportOptionsDlg
////////////////////////////////////////////////////////////////////////////////

class CImportOptionsDlg 
	: public CDialog
#ifdef IMPLEMENT_PLUGINMESSAGE
	,	public CPeekMessageProcess
#endif // IMPLEMENT_PLUGINMESSAGE
{
	DECLARE_DYNAMIC(CImportOptionsDlg)

public:

	CImportOptionsDlg( bool bIsCapturing, COptions& inOptions, COptions& inParentOptions,
						COmniPlugin* inPlugin, CWnd* pParent = NULL );
	virtual ~CImportOptionsDlg() {}

	enum { IDD_MODAL = IDD_IMPORT_OPTS_DLG };

	bool IsImportEntire() {
		return m_bImportEntire;
	}

protected:

#ifdef IMPLEMENT_PLUGINMESSAGE
	void		RequestOptionsImport( CPeekString& inFilePath );
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
	void			AddCase( CCaseOptions* CaseOptions );
	void			SetCaseData( CCaseOptions* CaseOptions, int nIndex );
	bool			InitializeCaseList();
	void			RetrieveListItemsSelected();
	void			SetControlState();
	void			OnBnClickedView();
	void			OnNMDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	void			OnLvnItemchangedList(NMHDR* pNMHDR, LRESULT *pResult);
	void			ProcessReturnedOptions( COptions& theNewOptions );
	void			SetEditControlsSelected();
	bool			ValidateImportPath( CPeekString& inImportFile );
	void			ClearImportOptions();
	void			OnTimer( UINT_PTR nIDEvent );

//	afx_msg void OnBnClickedBrowseForFile();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnRadioImportEntire();
	afx_msg void OnRadioImportCases();
	afx_msg void OnBnClickedReadDisplayFile();
	afx_msg void OnEnChangeImportFile();

	void EmptyCaseOptionsList() {
		CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_IMPORT_CASE_LIST );
		ASSERT(pList);
		if (pList == NULL) return;
		pList->DeleteAllItems();
		ASSERT(pList->GetItemCount() == 0);
	}

protected:
	bool					m_bInitializing;
	bool					m_bIsCapturing;
	COptions&				m_Options;
	COptions&				m_ParentOptions;
	CCaseOptionsList*		m_pImportCaseOptionsList;
	bool					m_bImportEntire;
	bool					m_bHaveImportFile;
	COmniPlugin*			m_pPlugin;
	bool					m_bWaitingForOptions;
	size_t					m_nTimer;
};

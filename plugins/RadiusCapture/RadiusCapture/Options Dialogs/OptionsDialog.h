// ============================================================================
//	OptionsDialog.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2009. All rights reserved.

#pragma once

#include "resource.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "Options.h"
#include "OmniPlugin.h"

#define SHOW_CHILD_DIALOG_ON_INVOKE


////////////////////////////////////////////////////////////////////////////////
//		CCaptureOptionsDlg
////////////////////////////////////////////////////////////////////////////////

class COptionsDialog 
	: public CDialog
{
	DECLARE_DYNAMIC(COptionsDialog)

public:

	COptionsDialog( COptions* inOptions, bool bIsCapturing, bool bIsDockedTab = false, COmniPlugin* inContext = NULL, 
						int nCaseItem = -1, CWnd* pParent = NULL );
	virtual ~COptionsDialog() {}

	enum { IDD_MODAL = IDD_RADIUS_OPTS_DLG,
		   IDD = IDD_OPTIONS_VIEW };

	void RefreshOptionsDisplay( COptions* inOptions = NULL );

	void SetCapturing( bool bIsCapturing ) {
		m_bIsCapturing = bIsCapturing;
	}

protected:

	DECLARE_MESSAGE_MAP()

	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual BOOL	OnInitDialog();
	virtual void	OnCancel();

	void MoveRadiusButton( int nId, int nTop );
	void MoveOKCancelButtons( int nTop, int nMiddle );
	void PortListToControl();
	void SetPortDisplay( bool bInit );
	bool InitializeCaseList();
	void SetControlState();
	void InitializeWindowSize();
	bool ValidateRootDirectory( CPeekString& strOutputRootDir );
	bool ValidatePort();
	bool ValidateCaptureOptions();
	void DeselectInvalidCases();
	void InitializeDuplicateFlag();
	void SetEditControlsSelected();
	void SortAndDisplayList( CPeekString strName = L"" );
	void SetMenuItemsSelected( CMenu* pPopup );
	void AddCase( CCaseOptions* Options );
	void SetCaseData( CCaseOptions* Options, int nIndex );
	void DeleteCase( CPeekString* pName );
	void SetAgeValues();
	void SetSizeValues();
	void DefaultCaptureSettingsToList();
	void BnClickedCaptureToFileAction( bool bSyncToList = false );
	void BnClickedCaptureToScreenAction( bool bSyncToList = false );
	void BnClickedDeleteAction();
	void DeleteAllCases();
	int	 CallReplaceCaseDlg( CPeekString strCaseName );
			
public:
	afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg void EnableAll();
	afx_msg void DisableAll();
	afx_msg void OnBnClickedImport();
	afx_msg void OnBnClickedExport();
	afx_msg void OnBnClickedLogCaseMsgs();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedEdit();
	afx_msg void OnBnClickedView();
	afx_msg void OnBnClickedDuplicate();
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnUpdateMenuAdd( CCmdUI* pCmdUI );
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnDeleteitemList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnBnClickedSuggestOutput();
	afx_msg void OnBnClickedPorts();
	afx_msg void OnUpdateMenuEdit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMenuView(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMenuDuplicate(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMenuDelete(CCmdUI *pCmdUI);
	afx_msg void OnBnClickedFileAge();
	afx_msg void OnBnClickedFileSize();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeOutputRootEdit();
	afx_msg void OnBnClickedUseFileNumber();
	afx_msg void OnEnChangeFileAgeCount();
	afx_msg void OnCbnSelchangeFileAgeUnits();
	afx_msg void OnEnChangeFileSizeCount();
	afx_msg void OnCbnSelchangeFileSizeUnits();
	afx_msg void OnBnClickedCaptureToFile();
	afx_msg void OnBnClickedCaptureToScreen();
	afx_msg void OnBnClickedLogMsgsToFile();
	afx_msg void OnBnClickedFoldersForCases();
	afx_msg void OnBnClickedBtnMakeChanges();
	afx_msg void OnBnClickedLogMsgsToScreen();
#ifdef SHOW_CHILD_DIALOG_ON_INVOKE
	afx_msg void OnShowWindow( BOOL bShow, UINT nStatus );
#endif

protected:
	void CaptureOptionsToData() {
		UpdateData();
	}
	void SyncCaseCaptureOptionsToGlobal() {
		m_pCaseOptionsList->SyncCaptureOptionsToGlobal( m_pOptions->GetCaptureOptions() );
	}
	void SyncCasePortsToGlobal() {
		m_pCaseOptionsList->SyncPortsToGlobal( m_pOptions->GetPortNumberList() );
	}
	void SyncGeneralOptionsToGlobal() {
		m_pCaseOptionsList->SyncGeneralOptionsToGlobal( m_pOptions->GetOutputRootDirectory(), m_pOptions->IsUseFileNumber() );
	}
	void EmptyCaseOptionsList() {
		CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_LIST_CASES );
		ASSERT(pList);
		if (pList == NULL) return;
		pList->DeleteAllItems();
		ASSERT(pList->GetItemCount() == 0);
	}
	bool IsDockedTab() {
		return m_bIsDockedTab;
	}

	protected:
		COptions*			m_pOptions;
		CCaseOptionsList*	m_pCaseOptionsList;
		bool				m_bIsDockedTab;
		bool				m_bReset;
		CMenu				m_Menu;
		CButton				m_BtnDisplayWarning;
		bool				m_bInitializing;
		bool				m_bIsCapturing;
		CString				m_strCurrentPortDisplay;
		int					m_nCaseItem;
		COmniPlugin*		m_pPlugin;
};
